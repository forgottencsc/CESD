#ifndef COMMON_HPP
#define COMMON_HPP

#include <bits/stdc++.h>

namespace detail {

using std::is_integral;
using std::string;
using std::wstring;
using std::vector;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::size_t;
using std::array;
using std::pair;
using std::swap;
using std::priority_queue;
using std::less;
using std::greater;
typedef vector<uint16_t> bytevec;

}

bytevec readfile(ifstream& in) {
    bytevec bdata;
    while (!in.eof()) {
        char byte; in.get(byte);
        if (in.eof()) break;
        bdata.push_back(byte);
    }
    return bdata;
}

void writefile(ofstream& out, const bytevec& vec) {
    for (auto c: vec) {
        out.put(c);
    }
}

#endif