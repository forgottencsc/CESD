#ifndef COMMON_HPP
#define COMMON_HPP

#include <bits/stdc++.h>

namespace detail {

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::size_t;
using std::is_integral;

using std::less;
using std::greater;

using std::array;
using std::string;
using std::wstring;
using std::vector;
using std::pair;
using std::swap;
using std::priority_queue;

using std::istream;
using std::ostream;
using std::ifstream;
using std::ofstream;

typedef vector<uint8_t> bytevec;

bytevec readfile(istream& in) {
    bytevec bdata;
    const size_t sz = 1<<10;
    array<uint8_t, sz> buf;
    size_t len;
    while (len = in.readsome(reinterpret_cast<char*>(buf.data()), sz * 2)) {
        for (int i = 0; i < len; i += 2)
            bdata.push_back(buf[i >> 1]);
    }
    return bdata;
}

void writefile(ostream& out, const bytevec& vec) {
    out.write(reinterpret_cast<const char*>(vec.data()), vec.size() * 2);
}


}

#endif