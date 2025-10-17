#pragma once

#include "reassembler.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"
#include <optional>

class TCPReceiver
{
public:
  explicit TCPReceiver( Reassembler&& reassembler ) : reassembler_( std::move( reassembler ) ), isn_{} {}
  void receive( TCPSenderMessage message );
  TCPReceiverMessage send() const;
  const Reassembler& reassembler() const { return reassembler_; }
  Reader& reader() { return reassembler_.reader(); }
  const Reader& reader() const { return reassembler_.reader(); }
  const Writer& writer() const { return reassembler_.writer(); }

private:
  Reassembler reassembler_;
  std::optional<Wrap32> isn_; 
};
