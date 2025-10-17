#include "tcp_receiver.hh"
#include <algorithm>
#include <cstdint>

using namespace std;

void TCPReceiver::receive(TCPSenderMessage message){
if(message.RST){
reassembler_.reader().set_error();
return;
}
if(message.SYN && !isn_.has_value()){
isn_=message.seqno;
}
if(isn_.has_value() && (!message.payload.empty()||message.FIN)){
uint64_t checkpoint=reassembler_.writer().bytes_pushed()+1;
uint64_t abs_seqno=message.seqno.unwrap(isn_.value(),checkpoint);
uint64_t abs_index=abs_seqno-1+message.SYN;
reassembler_.insert(abs_index, message.payload,message.FIN);
}
}

TCPReceiverMessage TCPReceiver::send() const{
TCPReceiverMessage msg;
msg.RST=reassembler_.reader().has_error();
if(isn_.has_value()){
uint64_t first_unassembled=reassembler_.writer().bytes_pushed();
uint64_t ack_abs=first_unassembled+1+reassembler_.writer().is_closed();
msg.ackno=Wrap32::wrap(ack_abs,isn_.value());
}
uint64_t avail=reassembler_.writer().available_capacity();
msg.window_size=min(avail,static_cast<uint64_t>(UINT16_MAX));
return msg;
}
