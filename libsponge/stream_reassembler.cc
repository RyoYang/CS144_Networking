#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) 
    : _output(capacity), _capacity(capacity), _first_unacceptable(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    _first_unread = _output.bytes_read();
    _first_unacceptable = _first_unread + _capacity;
    _segment seg;
    seg.index = index;
    seg.length = data.size();
    seg.str = data;
    _add_new_seg(seg, eof);
    _stitch_output();
    if(empty() && _eof)
    {
        _output.end_input();
    }
}

void StreamReassembler::_add_new_seg(_segment &seg, const bool eof)
{
    if(seg.index >= _first_unacceptable)
    {
        return ;
    }

    bool eof_of_this_seg = eof;
    int overflow_bytes = seg.index + seg.length - _first_unacceptable;
    if(overflow_bytes > 0)
    {
        int new_length = seg.length - overflow_bytes;
        if(new_length <= 0)
        {
            return ;
        }
        eof_of_this_seg = false;
        seg.length = new_length;
        seg.str = seg.str.substr(0, seg.length); 
    }

    if(seg.index < _first_unassembled)
    {
        int new_length = seg.length - (_first_unassembled - seg.index);
        if(new_length < 0)
        {
            return ;
        }
        seg.length = new_length;
        seg.str = seg.str.substr(_first_unassembled - seg.index, seg.length);
        seg.index = _first_unassembled;
    }
    _handle_overlap(seg);
    _eof = _eof || eof_of_this_seg;
}

void StreamReassembler::_handle_overlap(_segment &seg)
{
    for(auto iter = _buffer.begin(); iter != _buffer.end();)
    {
        auto next_iter = ++iter;
        --iter;
        if(((seg.index >= iter->index) && seg.index < iter->index + iter->length)
        || ((iter->index >= seg.index) && (iter->index < seg.index + seg.length)))
        {
            _merge_seg(seg, *iter);
            _buffer.erase(iter);
        }
        iter = next_iter;
    }
    _buffer.insert(seg);
}

void StreamReassembler::_stitch_one_seg(const _segment &seg)
{
    _output.write(seg.str);
    _first_unassembled += seg.length;
}

void StreamReassembler::_stitch_output()
{
    while(!_buffer.empty() && _buffer.begin()->index == _first_unassembled)
    {
        _stitch_one_seg(*_buffer.begin());
        _buffer.erase(_buffer.begin());
    }
}

void StreamReassembler::_merge_seg(_segment &new_seg, const _segment &other){
    size_t n_index = new_seg.index;
    size_t n_end = new_seg.index + new_seg.length;
    size_t o_index = other.index;
    size_t o_end = other.index + other.length;
    string new_data;
    if (n_index <= o_index && n_end <= o_end) {
        new_data = new_seg.str + other.str.substr(n_end - o_index, n_end - o_end);
    } else if (n_index <= o_index && n_end >= o_end) {
        new_data = new_seg.str;
    } else if (n_index >= o_index && n_end <= o_end) {
        new_data =
            other.str.substr(0, n_index - o_index) + new_seg.str + other.str.substr(n_end - o_index, n_end - o_end);
    } else /* if (n_index >= o_index && n_end <= o_end) */ {
        new_data = other.str.substr(0, n_index - o_index) + new_seg.str;
    }
    new_seg.index = n_index < o_index ? n_index : o_index;
    new_seg.length = (n_end > o_end ? n_end : o_end) - new_seg.index;
    new_seg.str = new_data;
}


size_t StreamReassembler::unassembled_bytes() const {
    size_t unassembled_bytes = 0;
    for(auto iter = _buffer.begin(); iter != _buffer.end(); ++iter)
    {
        unassembled_bytes += iter->length;
    }
    return unassembled_bytes;
}

bool StreamReassembler::empty() const { return unassembled_bytes() == 0; }
