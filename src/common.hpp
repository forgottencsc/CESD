#ifndef COMMON_HPP
#define COMMON_HPP

#include <bits/stdc++.h>

typedef std::vector<uint16_t> bytevec;
typedef unsigned int uint;

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