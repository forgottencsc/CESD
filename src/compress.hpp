#ifndef COMPRESS_HPP
#define COMPRESS_HPP
#include <bits/stdc++.h>

namespace compress {

using namespace std;

typedef vector<uint16_t> bytevec;
typedef unsigned int uint;

class obitstr {
public:
    bytevec& v;
    uint16_t b, t;
    obitstr(bytevec& v_) : v(v_), b(0), t(1) {
        
    }

};

class compressor {
public:
    virtual bytevec compress(const bytevec& data) = 0;
    virtual bytevec decompress(const bytevec& data) = 0;
};

class not_a_compressor : public compressor {
public:
    virtual bytevec compress(const bytevec& data) { return data; }
    virtual bytevec decompress(const bytevec& data) { return data; }
};

template<size_t L = 8>
class huffman_compessor : public compressor {
public:
    virtual bytevec compress(const bytevec& data) {
        const size_t N = 1 << L;
        array<int, N> c; c.fill(0);
        array<int, 2 * N> f; f.fill(0);
        int n = N;
        for (uint16_t x : data) c[x]++;

        return 0;
    }
    
    virtual bytevec decompress(const bytevec& data) { 

    }

};

}
#endif

