#ifndef BITSTREAM_HPP
#define BITSTREAM_HPP

#include <vector>
#include <string>
#include "common.hpp"

namespace detail {

using std::vector;
using std::string;
    
class obitstream {
public:
    bytevec& v;
    uint16_t b, t;
    size_t c;
    obitstream(bytevec& v_) : v(v_), b(0), t(1), c(0) {}

    ~obitstream() {
        assert(!t); //  flush() before destroy!
    }

    void put(bool a) {
        assert(t);  //  no
        c++;
        if (a) b |= t;
        else b &= ~t;
        t <<= 1;
        if (!t) {
            v.push_back(b);
            t = 1;
        }
    }

    template<class I>
    void puti(I z) {
        static_assert(std::is_integral<I>::value);
        for (int i = 0; i < 8 * sizeof(I); ++i)
            put((z >> i) & 1);
    }

    void puts(const string& s) {
        for (char ch : s)
            put(ch == '1');
    }

    uint16_t flush() {
        uint16_t res = c + __builtin_ctz(t);
        if (t != 1) v.push_back(b);
        t = 0;
        return res;
    }

};

class ibitstream {
public:
    const bytevec& v;
    uint16_t t;
    size_t i, c;

    void reset() {
        i = 0;
        t = 1;
        c = 0;
    }

    ibitstream(const bytevec& v_) : v(v_) { reset(); }

    ~ibitstream() {}


    bool eof() {
        return i == v.size();
    }

    bool get() {
        bool res = (v[i] & t) != 0;
        c++;
        t <<= 1;
        if (!t) {
            i++;
            t = 1;
        }
        return res;
    }

    template<class I>
    I geti() {
        static_assert(std::is_integral<I>::value);
        I res = 0;
        for (int x = 0; x < 8 * sizeof(I); ++x)
            if (get())
                res |= ((I)1 << x);
        return res;
    }

};

}

#endif