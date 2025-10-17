#include "byte_stream.hh"
using namespace std;

ByteStream::ByteStream(uint64_t cap)
    : capacity(cap), error(false), closed(false),
      buffer(), pushed(0), popped(0) {
}

void Writer::push(string data){
    uint64_t space=available_capacity();
    uint64_t write;

    if(data.size()<space){
        write=data.size();
    } else {
        write=space;
    }

    for(uint64_t i=0; i<write;i++){
        buffer.push(data[i]);
    }
    pushed=pushed+write;
}

void Writer::close(){
    closed = true;
}

bool Writer::is_closed() const {
    return closed;
}

uint64_t Writer::available_capacity() const {
    return capacity-buffer.size();
}

uint64_t Writer::bytes_pushed() const {
    return pushed;
}

string_view Reader::peek() const {
    if(buffer.empty()){
        return string_view();
    }
    return string_view(&buffer.front(),1);  
}

void Reader::pop(uint64_t length) {
    uint64_t pop;

    if(length<buffer.size()){
        pop=length;
    }
    else{
        pop =buffer.size();
    }

    for (uint64_t i=0;i<pop;i++){
        buffer.pop();
    }
    popped=popped+pop;
}

bool Reader::is_finished() const {
    return closed && buffer.empty();
}

uint64_t Reader::bytes_buffered() const {
    return buffer.size();
}

uint64_t Reader::bytes_popped() const {
    return popped;
}

