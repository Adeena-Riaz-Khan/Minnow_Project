Checkpoint 3 Writeup
My name: Adeena Riaz and Ali Asad Khan

My SUNet ID: 23L-2501 and 23L-0802

I collaborated with: [none]

I would like to thank/reward these classmates for their help: [ ]

This lab took me about: 3 to 4 days to do. We did not attend the lab session.

Describe TCPSender structure and design: We implemented the TCPSender using a std::queue to manage outstanding segments, ensuring we always retransmit the oldest segment first in a FIFO manner. The sender's state is tracked with several member variables, including next_seqno_ for the next sequence number to send, last_ackno_ and window_size_ for the receiver's window, and multiple variables for the retransmission timer (rto_ms_, time_elapsed_ms_ and timer_expiration_ms_ ). The logic is split into three main functions: push() greedily creates and sends segments to fill the window, receive() processes incoming ACKs and window updates, and tick() manages the passage of time and triggers retransmissions with exponential backoff.
Approach taken: A straightforward implementation of a sliding window protocol with a single retransmission timer for all outstanding data. We focused on keeping the state management clear and ensuring each function had a distinct responsibility.
Alternative designs considered: We considered using a more complex data structure than a queue for outstanding segments, but a simple queue was sufficient for the lab's requirement of retransmitting the oldest segment. We also considered coalescing segments for retransmission but stuck to the simpler approach of resending them individually as specified.
Benefits: The design is simple to reason about. Using a queue is efficient for FIFO operations. The logic is robust and correctly handles all required TCP sender behavior, including connection setup/teardown, data transfer, and timeout-based retransmissions.
Weaknesses: The implementation does not include more advanced TCP mechanisms like Fast Retransmit or SACK, so its performance on a network with frequent, non-consecutive packet loss would be suboptimal compared to a full TCP stack.
Performance: All performance-related tests, such as send_connect (0.02 sec) and send_retx (0.45 sec), passed quickly. The design is efficient enough for the requirements.
Implementation difficulty: High. The logic for the push() method, especially calculating the exact payload size while accounting for SYN/FIN flags and the window limit, was very complex. Managing the state of the retransmission timer (starting, stopping, restarting, and doubling the RTO) was also challenging.
Implementation Challenges:
Window Calculation: The most difficult part was correctly calculating space in the push() method. We initially failed to account for the single byte of window space consumed by the SYN flag, causing us to send a payload that was one byte too large. A similar issue occurred with the FIN flag, where we failed to send it when the window was exactly size 1.
Retransmission Timer: We had a critical bug where tick() would reset our main clock variable (time_elapsed_ms_) to zero after a retransmission, causing an infinite loop of retransmissions. The correct solution was to restart the timer from the current time.
RST Flag Handling: We initially missed all three aspects of RST handling: setting it on outgoing segments when our own stream had an error, and putting our stream into an error state when we received a segment with the RST flag.
Compiler Errors: We ran into a -Werror=reorder build failure, which required us to reorder our constructor's initializer list to perfectly match the declaration order of member variables in the header file.
Remaining Bugs: None. All 37 tests, including send_extra, send_window, and send_retx, now pass.
Optional: I had unexpected difficulty with: The interaction between flags (SYN/FIN) and the window size. Off-by-one errors in this area were the cause of several difficult-to-debug test failures. It took multiple attempts to get the payload and flag logic in push() exactly right.
Optional: I was surprised by: How precise the TCP rules are. The requirement to treat a zero-sized window as size one for probing, and how strictly the FIN flag must obey the window while the SYN is a special case, were details we hadn't considered before.

