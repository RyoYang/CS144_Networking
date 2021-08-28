#include "byte_stream.hh"

#include <deque>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) {
    // MAX_CAPACITY = capacity;
    RESIDENCE_CAPACITY = capacity;
}

size_t ByteStream::write(const string &data) {
    size_t byteCount = 0;
    for (auto &c : data) {
        if (RESIDENCE_CAPACITY == 0) {
            set_error();
            return byteCount;
        }
        buf.push_back(c);
        byteCount++;
        _byte_write++;
        RESIDENCE_CAPACITY--;
    }
    return byteCount;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    std::string res = "";
    for (auto iter = buf.cbegin(); iter < buf.cbegin() + len; iter++) {
        res.push_back(*iter);
    }
    return res;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    for (size_t i = 0; i < len; ++i) {
        buf.pop_front();
        _byte_read++;
    }
    RESIDENCE_CAPACITY += len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    std::string res = "";
    if(len > buf.size())
    {
        res = peek_output(buf.size());
        pop_output(buf.size());
    }else{
        res = peek_output(len);
        pop_output(len);
    }
    return res;
}

void ByteStream::end_input() { _is_end = true; }

bool ByteStream::input_ended() const { return _is_end; }

size_t ByteStream::buffer_size() const { return buf.size(); }

bool ByteStream::buffer_empty() const { return buf.empty(); }

bool ByteStream::eof() const { return buffer_empty() && _is_end; }

size_t ByteStream::bytes_written() const { return _byte_write; }

size_t ByteStream::bytes_read() const { return _byte_read; }

size_t ByteStream::remaining_capacity() const { return RESIDENCE_CAPACITY; }