#include "tcp_sender.hh"
#include "tcp_config.hh"
#include <algorithm>

using namespace std;

uint64_t TCPSender::sequence_numbers_in_flight() const
{
uint64_t total = 0;
queue<TCPSenderMessage> temp = outstanding_queue;
while (!temp.empty()) {
total += temp.front().sequence_length();
temp.pop();
}
return total;
}

uint64_t TCPSender::consecutive_retransmissions() const
{
return retransmit_count;
}

TCPSenderMessage TCPSender::make_empty_message() const
{
TCPSenderMessage msg;
msg.seqno = isn_ + next_abs_seqno;
msg.SYN = false;
msg.FIN = false;
msg.payload = "";
msg.RST = reader().has_error();
return msg;
}

void TCPSender::receive(const TCPReceiverMessage& msg)
{
if (msg.RST) {
reader().set_error();
return;
}

receiver_window_size = msg.window_size;

if (!msg.ackno.has_value()) {
return;
}

uint64_t received_ackno = msg.ackno.value().unwrap(isn_, next_abs_seqno);

if (received_ackno > next_abs_seqno) {
return;
}
if (received_ackno <= abs_ackno) {
return;
}

bool new_ack = false;
if (received_ackno > abs_ackno) {
new_ack = true;
}
abs_ackno = received_ackno;

while (!outstanding_queue.empty()) {
const TCPSenderMessage& segment = outstanding_queue.front();
uint64_t seg_start = segment.seqno.unwrap(isn_, next_abs_seqno);
uint64_t seg_end = seg_start + segment.sequence_length();
if (seg_end <= received_ackno) {
outstanding_queue.pop();
} else {
break;
}
}

if (new_ack) {
timer.reset_rto(initial_RTO_ms_);
retransmit_count = 0;
if (!outstanding_queue.empty()) {
timer.stop();
timer.start();
} else {
timer.stop();
}
}
}

void TCPSender::push(const TransmitFunction& transmit)
{
uint64_t window_to_use = receiver_window_size;
if (window_to_use == 0) {
window_to_use = 1;
}
uint64_t window_right_edge = abs_ackno + window_to_use;

while (next_abs_seqno < window_right_edge) {
bool send_syn = false;
if (next_abs_seqno == 0) {
send_syn = true;
}

bool have_data = false;
if (reader().bytes_buffered() > 0) {
have_data = true;
}

bool stream_ended = reader().is_finished();

if (!send_syn && !have_data && !stream_ended) {
break;
}

TCPSenderMessage msg;
msg.seqno = isn_ + next_abs_seqno;
msg.SYN = send_syn;
msg.FIN = false;
msg.RST = reader().has_error();

if (stream_ended && fin_flag_sent) {
break;
}

uint64_t space_available = window_right_edge - next_abs_seqno;
if (msg.SYN) {
if (space_available > 0) {
space_available = space_available - 1;
}
}

size_t max_payload = TCPConfig::MAX_PAYLOAD_SIZE;
size_t payload_limit = space_available;
if (payload_limit > max_payload) {
payload_limit = max_payload;
}

string payload;
if (payload_limit > 0) {
read(reader(), payload_limit, payload);
}
msg.payload = move(payload);

if (reader().is_finished() && !fin_flag_sent) {
if (space_available > msg.payload.size()) {
msg.FIN = true;
fin_flag_sent = true;
}
}

if (msg.sequence_length() == 0) {
break;
}

transmit(msg);

outstanding_queue.push(msg);
if (!timer.is_running()) {
timer.start();
}

next_abs_seqno += msg.sequence_length();

if (msg.FIN) {
break;
}

if (reader().bytes_buffered() == 0 && !reader().is_finished()) {
break;
}
}
}

void TCPSender::tick(uint64_t ms_since_last_tick, const TransmitFunction& transmit)
{
bool expired = timer.tick(ms_since_last_tick);
if (expired) {
if (!outstanding_queue.empty()) {
const TCPSenderMessage& segment = outstanding_queue.front();
transmit(segment);

if (receiver_window_size > 0) {
retransmit_count = retransmit_count + 1;
timer.double_rto();
}

timer.stop();
timer.start();
}
}
}
