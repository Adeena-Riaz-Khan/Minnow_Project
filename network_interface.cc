#include <iostream>
#include "arp_message.hh"
#include "debug.hh"
#include "ethernet_frame.hh"
#include "exception.hh"
#include "helpers.hh"
#include "network_interface.hh"
using namespace std;

NetworkInterface::NetworkInterface( string_view name,
shared_ptr<OutputPort> port,
const EthernetAddress& ethernet_address,
const Address& ip_address )
: name_( name )
, port_( notnull( "OutputPort", move( port ) ) )
, ethernet_address_( ethernet_address )
, ip_address_( ip_address )
{
cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address_ ) << " and IP address "
<< ip_address.ip() << "\n";
}

void NetworkInterface::send_datagram( const InternetDatagram& dgram, const Address& next_hop )
{
uint32_t target_ip = next_hop.ipv4_numeric();

if ( arp_cache_.count( target_ip ) > 0 ) {
EthernetFrame outgoing;
outgoing.header.type = EthernetHeader::TYPE_IPv4;
outgoing.header.src = ethernet_address_;
outgoing.header.dst = arp_cache_.at( target_ip );
outgoing.payload = serialize( dgram );
transmit( outgoing );
return;
}

pending_datagrams_[target_ip].push_back( { dgram, current_time_ms_ } );

bool should_send_request = false;
if ( arp_request_times_.count( target_ip ) == 0 ) {
should_send_request = true;
} else {
size_t time_since_request = current_time_ms_ - arp_request_times_.at( target_ip );
if ( time_since_request >= ARP_REQUEST_TIMEOUT_MS ) {
should_send_request = true;
}
}

if ( should_send_request ) {
ARPMessage arp_req;
arp_req.opcode = ARPMessage::OPCODE_REQUEST;
arp_req.sender_ethernet_address = ethernet_address_;
arp_req.sender_ip_address = ip_address_.ipv4_numeric();
arp_req.target_ip_address = target_ip;
arp_req.target_ethernet_address = {};

EthernetFrame arp_frame;
arp_frame.header.type = EthernetHeader::TYPE_ARP;
arp_frame.header.src = ethernet_address_;
arp_frame.header.dst = ETHERNET_BROADCAST;
arp_frame.payload = serialize( arp_req );
transmit( arp_frame );

arp_request_times_[target_ip] = current_time_ms_;
}
}

void NetworkInterface::recv_frame( EthernetFrame frame )
{
bool is_for_us = ( frame.header.dst == ethernet_address_ );
bool is_broadcast = ( frame.header.dst == ETHERNET_BROADCAST );

if ( !is_for_us && !is_broadcast ) {
return;
}

if ( frame.header.type == EthernetHeader::TYPE_IPv4 ) {
InternetDatagram dgram;
if ( parse( dgram, frame.payload ) ) {
datagrams_received_.push( dgram );
}
return;
}

if ( frame.header.type == EthernetHeader::TYPE_ARP ) {
ARPMessage arp_msg;
if ( !parse( arp_msg, frame.payload ) ) {
return;
}

uint32_t sender_ip = arp_msg.sender_ip_address;
EthernetAddress sender_eth = arp_msg.sender_ethernet_address;

arp_cache_[sender_ip] = sender_eth;
arp_cache_times_[sender_ip] = current_time_ms_;

if ( pending_datagrams_.count( sender_ip ) > 0 ) {
list<pair<InternetDatagram, size_t>>& waiting_list = pending_datagrams_.at( sender_ip );

list<pair<InternetDatagram, size_t>>::iterator it = waiting_list.begin();
while ( it != waiting_list.end() ) {
size_t dgram_age = current_time_ms_ - it->second;
if ( dgram_age < ARP_REQUEST_TIMEOUT_MS ) {
EthernetFrame outgoing;
outgoing.header.type = EthernetHeader::TYPE_IPv4;
outgoing.header.src = ethernet_address_;
outgoing.header.dst = sender_eth;
outgoing.payload = serialize( it->first );
transmit( outgoing );
}
++it;
}
pending_datagrams_.erase( sender_ip );
}

if ( arp_msg.opcode == ARPMessage::OPCODE_REQUEST ) {
if ( arp_msg.target_ip_address == ip_address_.ipv4_numeric() ) {
ARPMessage arp_reply;
arp_reply.opcode = ARPMessage::OPCODE_REPLY;
arp_reply.sender_ethernet_address = ethernet_address_;
arp_reply.sender_ip_address = ip_address_.ipv4_numeric();
arp_reply.target_ethernet_address = sender_eth;
arp_reply.target_ip_address = sender_ip;

EthernetFrame reply_frame;
reply_frame.header.type = EthernetHeader::TYPE_ARP;
reply_frame.header.src = ethernet_address_;
reply_frame.header.dst = sender_eth;
reply_frame.payload = serialize( arp_reply );
transmit( reply_frame );
}
}
}
}

void NetworkInterface::tick( const size_t ms_since_last_tick )
{
current_time_ms_ += ms_since_last_tick;

unordered_map<uint32_t, size_t>::iterator cache_it = arp_cache_times_.begin();
while ( cache_it != arp_cache_times_.end() ) {
uint32_t ip = cache_it->first;
size_t entry_age = current_time_ms_ - cache_it->second;

if ( entry_age >= ARP_CACHE_TTL_MS ) {
arp_cache_.erase( ip );
cache_it = arp_cache_times_.erase( cache_it );
} else {
++cache_it;
}
}

unordered_map<uint32_t, list<pair<InternetDatagram, size_t>>>::iterator pending_it = pending_datagrams_.begin();
while ( pending_it != pending_datagrams_.end() ) {
list<pair<InternetDatagram, size_t>>& dgram_list = pending_it->second;

list<pair<InternetDatagram, size_t>>::iterator dgram_it = dgram_list.begin();
while ( dgram_it != dgram_list.end() ) {
size_t dgram_age = current_time_ms_ - dgram_it->second;
if ( dgram_age >= ARP_REQUEST_TIMEOUT_MS ) {
dgram_it = dgram_list.erase( dgram_it );
} else {
++dgram_it;
}
}

if ( dgram_list.empty() ) {
pending_it = pending_datagrams_.erase( pending_it );
} else {
++pending_it;
}
}
}
