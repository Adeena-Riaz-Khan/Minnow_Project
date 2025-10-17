#pragma once
#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"
#include <functional>
#include <queue>
#include <cstdint>
using namespace std;

class RetransmissionTimer {
public:
RetransmissionTimer(uint64_t initial_rto_ms)
: rto_ms_(initial_rto_ms), time_elapsed_(0), running_(false) {}

void start() {
if (!running_) {
time_elapsed_ = 0;
running_ = true;
}
}

void stop() {
running_ = false;
}

bool tick(uint64_t ms_since_last_tick) {
if (running_) {
time_elapsed_ += ms_since_last_tick;
if (time_elapsed_ >= rto_ms_) {
return true;
}
}
return false;
}

void double_rto() {
rto_ms_ *= 2;
}

void reset_rto(uint64_t initial_rto_ms) {
rto_ms_ = initial_rto_ms;
}

bool is_running() const {
return running_;
}

private:
uint64_t rto_ms_;
uint64_t time_elapsed_;
bool running_;
};

class TCPSender
{
public:
TCPSender( ByteStream&& input, Wrap32 isn, uint64_t initial_RTO_ms )
: input_( std::move( input ) )
, isn_( isn )
, initial_RTO_ms_( initial_RTO_ms )
, next_abs_seqno( 0 )
, abs_ackno( 0 )
, receiver_window_size( 1 )
, outstanding_queue()
, retransmit_count( 0 )
, timer( initial_RTO_ms_ )
, fin_flag_sent( false )
{}

TCPSenderMessage make_empty_message() const;
void receive( const TCPReceiverMessage& msg );
using TransmitFunction = std::function<void( const TCPSenderMessage& )>;
void push( const TransmitFunction& transmit );
void tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit );
uint64_t sequence_numbers_in_flight() const;
uint64_t consecutive_retransmissions() const;
const Writer& writer() const { return input_.writer(); }
const Reader& reader() const { return input_.reader(); }
Writer& writer() { return input_.writer(); }

private:
Reader& reader() { return input_.reader(); }
ByteStream input_;
Wrap32 isn_;
uint64_t initial_RTO_ms_;
uint64_t next_abs_seqno;
uint64_t abs_ackno;
uint32_t receiver_window_size;
queue<TCPSenderMessage> outstanding_queue;
uint32_t retransmit_count;
RetransmissionTimer timer;
bool fin_flag_sent;
};
