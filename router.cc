#include "router.hh"
#include "debug.hh"

#include <algorithm>
#include <iostream>

using namespace std;

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
void Router::add_route( const uint32_t route_prefix,
                        const uint8_t prefix_length,
                        const optional<Address> next_hop,
                        const size_t interface_num )
{

  routing_table.push_back({route_prefix, prefix_length, next_hop, interface_num});
  sort( routing_table.begin(), routing_table.end() );
}

// Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
void Router::route()
{

  for ( auto& interface_ptr : interfaces_ ) {

    auto& datagram_queue = interface_ptr->datagrams_received();

    while ( !datagram_queue.empty() ) {
      InternetDatagram incoming_datagram = datagram_queue.front();
      datagram_queue.pop();

      if ( incoming_datagram.header.ttl <= 1 ) {
        continue;
        // we drop this datagram as it either 0 or will be zero
      }
      incoming_datagram.header.ttl--;
      incoming_datagram.header.compute_checksum();

      const uint32_t destination_ip = incoming_datagram.header.dst;
      routeEntry* matched_entry = nullptr;

      for ( auto& route : routing_table ) {
        bool is_match = false;
        if ( route.prefix_length == 0 ) {
          // Default route (0.0.0.0/0) always matches
          is_match = true;
        } else {
          uint32_t mask = 0xFFFFFFFF << ( 32 - route.prefix_length );
          if ( ( destination_ip & mask ) == ( route.route_prefix & mask ) ) {
            is_match = true;
          }
        }

        if ( is_match ) {

          matched_entry = &route;
        }
      }
      if ( matched_entry == nullptr ) {
        continue;
        // if not route matches the destination ip
      }

      NetworkInterface& outgoing_interface = *( interfaces_.at( matched_entry->interface_num ) );

      Address next_hop_address = matched_entry->next_hop.has_value() ? matched_entry->next_hop.value()
                                                                     : Address::from_ipv4_numeric( destination_ip );

      outgoing_interface.send_datagram( incoming_datagram, next_hop_address );
    }
  }
}
