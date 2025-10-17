Checkpoint 1 Writeup
====================

My name: Adeena Riaz and Ali Asad Khan

My SUNet ID: 23L-2501 and 23L-0802

I collaborated with: [none]

I would like to thank/reward these classmates for their help: [Bilal Ahmed Khan,Maaz Mahmood,Mariyam Akram Pasha and Areeb Zahra]

This lab took me about: 2 to 3 days to do.

---

I was surprised by or edified to learn that:
We were surprised to see how much care is needed to handle out-of-order and overlapping segments in the reassembler. At first, We thought it would be simple to just insert fragments, but the complexity of merging overlapping substrings and waiting for the correct next index taught us how real TCP reassembly works.

---

Report from the hands-on component of the lab checkpoint:
- From 2.1(4): We tested the ByteStream by pushing and popping data. It was interesting to see how the buffer’s capacity limited the amount of data we could push at once, and how bytes only left the buffer when we popped them.
- From 2.2: Our experience with the Reassembler was more challenging. We learned that even though it feels like we’re just storing strings, the main trick is to only send data into the ByteStream when it is exactly the next expected index. This means storing “future” data until it becomes contiguous. Getting that flush logic correct took some trial and error, especially with overlapping fragments.

---

Describe Reassembler structure and design:
We used a map<uint64_t, string> to store fragments, with the key being the starting index of the fragment. This choice made it easy to keep fragments sorted by their starting index and to merge overlapping ranges.
- Approach taken: When new data comes in, We trim it if it is fully before next_idx (already written). Otherwise, we insert it into the map, merge it with any overlapping fragments, and then attempt to flush from the map into the ByteStream starting at next_idx. If an EOF flag arrives and all data up to that point has been written, we close the ByteStream.
- Alternative designs considered: We briefly considered using a vector or manual array to store fragments, but that would have made merging and searching slower and more complex. The map keeps everything ordered automatically.
- Benefits: Simplicity in ordering and merging, correctness with overlaps, fewer chances of missing fragments.
- Weaknesses: Slightly higher memory and performance cost compared to a lower-level structure.
- Performance: The reassembler passes tests and works within expected runtime. The biggest bottleneck we faced was debugging merge logic, not speed.
- Implementation difficulty: Moderate — the hardest part was making sure overlaps are merged correctly and that the first byte always flushes when available.

---

Implementation Challenges:
- Handling overlapping fragments correctly without double-counting.
- Making sure the first inserted byte (index 0) actually flushes into the ByteStream immediately.
- Debugging test failures like reassembler_overlapping where our old logic only merged with one neighbor.

---

Remaining Bugs:
At this point, our implementation seems to pass all provided tests. No known bugs remain.

---


