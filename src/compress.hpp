#ifndef COMPRESS_HPP
#define COMPRESS_HPP
#include "common.hpp"
#include "bitstream.hpp"

namespace compress {

using namespace detail;

class compressor {
public:
    virtual bytevec compress(const bytevec& data) const = 0;
    virtual bytevec decompress(const bytevec& data) const = 0;
};

class not_a_compressor : public compressor {
public:
    virtual bytevec compress(const bytevec& data) const { return data; }
    virtual bytevec decompress(const bytevec& data) const { return data; }
};

class huffman_compessor : public compressor {
public:

    virtual bytevec compress(const bytevec& data) const {
        const size_t L = 8, N = 1 << L;
        array<size_t, N> c; c.fill(0);
        array<uint16_t, 2 * N> f; f.fill(~0);
        array<int, 2 * N> t; t.fill(0);
        array<string, N> s;
        ibitstream is(data);

        while (!is.eof())
            c[is.geti<uint8_t>()]++;
        
        priority_queue<pair<size_t, uint16_t>, vector<pair<size_t, uint16_t>>, greater<pair<size_t, uint16_t>>> pq;
        for (uint32_t i = 0; i < N; ++i)
            pq.push({ c[i], i });
        
        for (int i = 0; i < N - 1; ++i) {
            pair<size_t, uint16_t> p1, p2, p3;
            p1 = pq.top(); pq.pop();
            p2 = pq.top(); pq.pop();
            if (p1.second > p2.second) swap(p1, p2);
            p3 = { p1.first + p2.first, N + i };
            f[p1.second] = f[p2.second] = p3.second;
            t[p1.second] = 0;
            t[p2.second] = 1;
            pq.push(p3);
        }

        assert(pq.size() == 1);

        int r = pq.top().second;

        for (uint16_t i = 0; i < N; ++i) {
            for (uint16_t j = i; j != r; j = f[j])
                s[i] += '0' + t[j];
            reverse(s[i].begin(), s[i].end());
        }

        bytevec res;
        obitstream os(res);

        size_t len = 0;
        for (uint16_t i = 0; i < N; ++i)
            len += c[i] * s[i].size();
        
        assert(f[2 * N - 2] == (uint16_t)(~0));
        for (uint16_t i = 0; i < 2 * N - 2; ++i)
            os.puti<uint16_t>(f[i]);
        
        os.puti<uint32_t>(len);

        is.reset();
        while (!is.eof())
            os.puts(s[is.geti<uint8_t>()]);

        os.flush();
        return res;
    }
    
    virtual bytevec decompress(const bytevec& data) const { 
        const size_t L = 8, N = 1 << L;
        bytevec res;
        ibitstream is(data);
        obitstream os(res);
        array<uint16_t, 2 * N> ls, rs;
        ls.fill(~0);
        rs.fill(~0);
        for (int i = 0; i < 2 * N - 2; ++i) {
            uint16_t f = is.geti<uint16_t>();
            if (ls[f] == (uint16_t) ~0) ls[f] = i;
            else rs[f] = i;
        }
        uint32_t len = is.geti<uint32_t>();
        uint16_t p = 2 * N - 2;
        for (uint32_t i = 0; i < len; ++i) {
            p = (is.get() ? rs : ls)[p];
            if (p < N) {
                os.puti<uint8_t>(p);
                p = 2 * N - 2;
            }
        }
        os.flush();
        return res;
    }

};

}

using compress::compressor;
using compress::huffman_compessor;
using compress::not_a_compressor;

#endif

