# Checkpoint 5 Writeup

# ====================

# 

# My name: Adeena Riaz and Ali Asad Khan

# 

# My SUNet ID: 23L-2501 and 23L-0802

# 

# I collaborated with: none

# 

# I would like to thank/reward these classmates for their help: \[none]

# 

# This checkpoint took me about 8-10 hours to do. We did not attend the lab session.

# 

# Program Structure and Design of the NetworkInterface:

# 

# Data Structures:

# We used several unordered\_maps to manage ARP functionality:

# 1\. arp\_cache\_ - Maps IP addresses (uint32\_t) to Ethernet addresses

# 2\. arp\_cache\_times\_ - Tracks cache entry timestamps for 30-second expiration

# 3\. arp\_request\_times\_ - Tracks last ARP request time per IP for 5-second rate limiting

# 4\. pending\_datagrams\_- Maps IPs to lists of queued datagrams with timestamps

# 5\. current\_time\_ms\_ - Accumulates elapsed time for all timestamp comparisons

# 

# \*Note: We used AI to understand ARP request rate limiting and how to structure the pending datagram queue with timestamps.\*

# 

# Approach: We chose unordered\_map for O(1) average lookups. Each pending datagram stores a timestamp to enable expiration. The current\_time\_ms\_ variable accumulates time in tick() for consistent comparisons.

# 

# Alternative designs: Initially used simple queues without timestamps. Considered std::map for sorted iteration but unordered\_map was faster. Also considered a single struct containing all related fields instead of separate maps.

# 

# Benefits: O(1) lookups, efficient timestamp-based expiration, organized by IP for quick ARP reply processing.

# 

# Weaknesses: Multiple maps require synchronization and more memory. No limit on queue size per IP.

# 

# Performance: All check5 tests passed quickly with constant-time operations.

# 

# send\_datagram():

# Converts next\_hop to uint32\_t via ipv4\_numeric(). If IP is in cache, creates Ethernet frame (TYPE\_IPv4), serializes datagram, and transmits immediately. Otherwise, queues datagram with timestamp and checks if ARP request should be sent (never sent before OR 5+ seconds since last request). If sending ARP, broadcasts OPCODE\_REQUEST and updates request timestamp.

# 

# recv\_frame():

# Filters frames not for us (checks dst against ethernet\_address\_ or ETHERNET\_BROADCAST). For TYPE\_IPv4, parses and pushes to datagrams\_received\_. For TYPE\_ARP, always learns sender's IP-to-Ethernet mapping with timestamp. If pending datagrams exist for sender\_ip, sends all non-expired ones (age < 5 seconds) and clears queue. For ARP requests targeting our IP, sends reply.

# 

# tick():

# Updates current\_time\_ms\_. Expires ARP cache entries older than 30 seconds using iterator-based erasure pattern. Expires queued datagrams older than 5 seconds. Removes empty lists from pending\_datagrams\_.

# 

# Implementation: ~150 lines total using constants ARP\_CACHE\_TTL\_MS (30000) and ARP\_REQUEST\_TIMEOUT\_MS (5000).

# 

# Implementation Challenges:

# 

# \- Iterator invalidation when erasing from maps. Initially wrote `erase(it); ++it;` which crashed. Fixed using `it = erase(it)` pattern. \*We used AI to identify and fix this iterator invalidation bug.\*

# 

# \- Storing timestamps with pending datagrams. Changed from simple list to `list<pair<InternetDatagram, size\_t>>` to enable expiration.

# 

# \- ARP request rate limiting logic required checking both "never sent" and "5+ seconds elapsed" conditions.

# 

# \- Processing pending datagrams immediately in recv\_frame() vs deferring to tick(). Immediate processing reduced latency.

# 

# \- Nested iteration in tick() for map-of-lists structure required careful erasure patterns.

# 

# Remaining Bugs:

# 

# None. All check5 tests pass including multiple queued datagrams, exact 30-second cache expiration, 5-second rate limiting, and opportunistic learning from requests.

# 

# Optional: I had unexpected difficulty with:

# 

# Iterator invalidation bugs were frustrating - code compiled but crashed during tests. The nested data structure (map of lists of pairs) and tick() iteration logic were more complex than expected.

# 

# Optional: I was surprised by:

# 

# Processing pending datagrams in recv\_frame() made code much cleaner. Learning from both ARP requests AND replies wasn't intuitive initially but makes sense for efficiency.

# 

# Optional: I'm not sure about:

# 

# Whether 5-second datagram expiration is optimal (spec was unclear). Also uncertain if we need maximum queue size limits to prevent memory exhaustion for unresponsive IPs.

# 

# Optional: I think you could make this lab better by:

# 

# Clearer guidance on datagram expiration timing. More edge case tests for iterator invalidation and long-running sessions would be helpful.

