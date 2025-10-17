Checkpoint 2 Writeup
====================

My name: Adeena Riaz and Ali Asad Khan

My SUNet ID: 23L-2501 and 23L-0802

I collaborated with: [none]

I would like to thank/reward these classmates for their help: [Maaz Mahmood, Mohib Mansoor,  Areeb Zahra, Khadija Asim]

This lab took me about: 3 to 4 days to do. We did not attend the lab session.

Describe Wrap32 and TCPReceiver structure and design:
Wrap32:
We used a uint32_t to store the 32-bit sequence number for Wrap32. The wrap function turns a big stream index (uint64_t) into a 32-bit sequence number by using modulo 2^32, based on the initial sequence number (ISN). The unwrap function does the opposite, turning a 32-bit sequence number back into a stream index, using a checkpoint to find the closest match. We used division and modulo to handle wrapping.
- Approach taken: Simple modulo math for wrap and checkpoint-based math for unwrap.
- Alternative designs considered: We thought about bit shifting instead of modulo, but modulo was easier to understand. Storing the ISN in Wrap32 was another idea, but keeping it separate was simpler.
- Benefits: Easy to code and understand, works for all test cases like wrapping_integers_roundtrip.
- Weaknesses: Unwrap might struggle with huge indices, but tests didn’t show this problem.
- Performance: Tests like wrapping_integers_roundtrip (0.45 sec) passed fast, so it’s efficient enough.
- Implementation difficulty: Moderate. Figuring out the unwrap math took some time.

TCPReceiver:
We used a std::optional<Wrap32> to hold the ISN and a Reassembler to manage data. The receive function checks for RST (sets error), saves the ISN for the first SYN, and puts data or FIN into the Reassembler after converting sequence numbers to stream indices. The send function makes a TCPReceiverMessage with RST (from Reassembler error), ackno (next expected sequence number), and window_size (ByteStream’s free space, capped at 65535). We adjust for SYN by subtracting 1 from the stream index.
- Approach taken: Use Reassembler’s map (from Checkpoint 1) to handle out-of-order data, keep logic simple.
- Alternative designs considered: We considered tracking stream indices in TCPReceiver, but Reassembler’s bytes_pushed was easier. We also thought about calling insert for SYN, but it wasn’t needed since SYN has no data.
- Benefits: Simple code, reuses Reassembler’s logic, handles SYN, FIN, and RST correctly.
- Weaknesses: Depends on Reassembler and ByteStream working right. If available_capacity is wrong, window_size fails.
- Performance: Tests like recv_connect (0.03 sec) and recv_window (0.04 sec) passed quickly.
- Implementation difficulty: Moderate. Getting window_size to ignore out-of-order data and setting ackno for SYN were tricky.

Implementation Challenges:
- Making window_size show ByteStream’s free space (4000 initially) without counting out-of-order data in Reassembler. Early versions got this wrong, failing recv_connect.
- Setting ackno to ISN + 1 for SYN-only messages. We first set it to ISN, which failed recv_connect.
- Handling out-of-order data in recv_window without changing window_size. Our Reassembler’s map helped here.
- Keeping the code short and clear while handling all cases like pre-SYN and RST.

Remaining Bugs:
None. All tests, including recv_connect, recv_transmit, and recv_window, now pass.

Optional: I had unexpected difficulty with:
The recv_connect test was hard. We didn’t realize window_size should be 4000 before any data, and fixing ackno for SYN took several tries.

Optional: I was surprised by:
How window_size only counts ByteStream’s used space, not out-of-order data in Reassembler. This was new to us.

Optional: I’m not sure about:
If our unwrap handles very large stream indices correctly. Tests passed, but we didn’t try extreme cases.

