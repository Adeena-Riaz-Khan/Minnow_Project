# Checkpoint 6 Writeup

# ====================

# 

# My name: Adeena Riaz and Ali Asad Khan

# 

# My SUNet ID: 23L-2501 and 23L-0802

# 

# I collaborated with: none

# 

# I would like to thank/reward these classmates for their help: [none]

# 

# This checkpoint took me about 4-6 hours to do. We did not attend the lab session.

# 

# Program Structure and Design of the Router:

# 

# Data Structures:

# We used a simple standard vector to store the routing table:

# 1. struct routeEntry - A private helper struct containing `route_prefix`, `prefix_length`, `next_hop` (optional), and `interface_num`. We also implemented a custom `<` operator to allow for sorting based on prefix length.

# 2. std::vector<routeEntry> routing_table_ - Stores all added routes.

# 

# *Note: We consulted AI resources to better understand the nuances of strict C++ compiler flags and to double-check our understanding of bitwise logic for prefix matching.*

# 

# Approach: 

# We implemented a "Sort-and-Scan" strategy for the Longest Prefix Match (LPM). 

# In `add_route`, we push the new route and immediately call `std::sort` on the vector. Our custom comparator ensures the table is always sorted by `prefix_length` in ascending order.

# 

# In `route`, we iterate linearly through the sorted table. Because the routes are ordered from shortest to longest prefix, we simply update our `matched_entry` pointer whenever a match is found. The last matching route encountered in the loop is guaranteed to be the one with the longest prefix length. This eliminates the need for complex conditional comparisons inside the loop.

# 

# Alternative designs: 

# We considered using a Trie (prefix tree), which would allow for faster lookups (O(1) or O(log N)). However, given that O(N) was permitted and the routing table size for this lab is small, the vector approach was significantly simpler to implement and less prone to memory errors.

# 

# Benefits: 

# Extremely clean code logic (due to sorting), low memory overhead, and cache-friendly iteration.

# 

# Weaknesses: 

# Linear lookup time O(N) scales poorly if the routing table were to grow to thousands of entries, though it is sufficient for this simulation.

# 

# Performance: 

# Passed all tests efficiently. The linear scan is fast enough for the simulated network conditions.

# 

# add_route():

# Constructs a `routeEntry` object using brace initialization `{...}` (to satisfy strict compiler requirements) and pushes it into the `routing_table_`. It then sorts the table to prepare for the routing logic.

# 

# route():

# Iterates through all interfaces to check their `datagrams_received` queues. For each datagram:

# 1. Checks TTL: If TTL <= 1, drops the packet. Otherwise, decrements TTL and *recomputes the checksum*.

# 2. Longest Prefix Match: Iterates the sorted vector. Uses `0xFFFFFFFF << (32 - length)` to mask and compare bits (handling the /0 edge case). Updates the match pointer on every success.

# 3. Forwarding: If a match is found, uses the ternary operator to determine the next hop (gateway IP vs. destination IP) and calls `send_datagram` on the correct interface.

# 

# Implementation Challenges:

# 

# - Compiler Errors (Strict Flags): We faced significant issues with the `-Weffc++` flags, specifically `error: ‘Router::routing_table’ should be initialized in the member initialization list`. We resolved this by explicitly initializing the vector with `{}` in the header file.

# 

# - Address Constructor Issues: We encountered `no default constructor exists for class "Address"` when declaring the next hop variable. We solved this using the ternary operator to initialize the `Address` in a single line, rather than declaring it empty and assigning it later.

# 

# - Logic Bugs: We initially had a typo `if (matched_entry = nullptr)` instead of `==`, which caused valid routes to be dropped. The compiler's warning about "assignment used as truth value" helped us catch this.

# 

# - Integration with Check 5: The tests initially failed because our `NetworkInterface::send_datagram` (from the previous check) was incomplete. We had to go back and implement the ARP queuing logic to make the router work.

# 

# Remaining Bugs:

# 

# None. All check6 tests pass, including the bug-checkers.

# 

# Optional: I had unexpected difficulty with:

# 

# Understanding that the Router doesn't have its own datagram queue, but rather has to poll the queues of all its managed interfaces.

# 

# Optional: I was surprised by:

# 

# How the `sort()` function simplified the actual routing loop. It removed the need to manually track "current max length" variables.

# 

# Optional: I'm not sure about:

# 

# Whether this sorting approach would be efficient if routes were added/removed frequently in a real-world scenario (since sorting is O(N log N)).

# 

# Optional: I think you could make this lab better by:

# 

# Providing a hint in the FAQs about the `Address` class lacking a default constructor, as this required a specific C++ coding pattern to resolve cleanly.
