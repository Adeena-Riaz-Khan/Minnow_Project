#pragma once

#include "byte_stream.hh"
#include <map>
#include <string>
#include <cstdint>
using namespace std;

class Reassembler {
public:
    explicit Reassembler(ByteStream&& out);

    void insert(uint64_t idx, string s, bool last);

    uint64_t count_bytes_pending() const;

    Reader& reader() { return out_.reader(); }
    const Reader& reader() const { return out_.reader(); }
    const Writer& writer() const { return out_.writer(); }

private:
    ByteStream out_;
    uint64_t next_idx = 0;
    bool end_seen_flag = false;
    uint64_t end_pos = 0;
    map<uint64_t, string> frags;

    void flush();
};

