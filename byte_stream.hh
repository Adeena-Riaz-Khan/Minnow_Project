#pragma once

#include <cstdint>
#include <queue>
#include <string>
#include <string_view>
using namespace std;

class Reader;
class Writer;

class ByteStream
{
public:
    explicit ByteStream(uint64_t cap);

    Reader& reader();
    const Reader& reader() const;
    Writer& writer();
    const Writer& writer() const;

    void set_error() { error = true; }
    bool has_error() const { return error; }

protected:
    uint64_t capacity;
    bool error;
    bool closed;
    queue<char> buffer;
    uint64_t pushed;   //bytes written
    uint64_t popped;   //bytes read
};

class Writer : public ByteStream
{
public:
    using ByteStream::ByteStream;

    void push(string data);
    void close();

    bool is_closed() const;
    uint64_t available_capacity() const;
    uint64_t bytes_pushed() const;
};

class Reader : public ByteStream
{
public:
    using ByteStream::ByteStream;

    string_view peek() const;
    void pop(uint64_t len);

    bool is_finished() const;
    uint64_t bytes_buffered() const;
    uint64_t bytes_popped() const;
};

// helper
void read(Reader& reader, uint64_t max_len, string& out);

