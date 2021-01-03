#ifndef ENCRYPT_HPP
#define ENCRYPT_HPP
#include "common.hpp"
#include "bitstream.hpp"

namespace encrypt {

using namespace detail;

const string decrypt_failed_str = "Decryption failed";

namespace hash_impl {

const uint32_t P = 4294967087, G = 810008351, C1 = 1145141919, C2 = 1919810810;
const uint32_t B1 = 364364364, B2 = 893893893;

uint64_t mix(uint32_t a, uint32_t b) {
    uint64_t r = 0;
    for (int i = 0; i < 32; ++i) {
        r = (r << 1) | (a & 1);
        r = (r << 1) | (b & 1);
        a >>= 1;
        b >>= 1;
    }
    return r;
}

uint32_t mul(uint32_t a, uint32_t b) { return uint64_t(a) * uint64_t(b) % P; }
uint32_t pow(uint32_t a, uint32_t b) {
    uint32_t r = 1;
    do if (b & 1) r = mul(r, a);
    while (a = mul(a, a), b >>= 1);
    return r;
}

uint64_t cal_hash(uint64_t a) {
    uint64_t b = mix(a, a >> 32), hi = pow(G, b >> 32) + C1, lo = pow(G, b) + C2;
    return (pow(lo, hi) ^ pow(hi, lo)) | ((uint64_t)(pow(lo, lo) ^ pow(hi, hi)) << 32);
}

template<class C>
uint64_t cal_hash(const C& s, std::enable_if_t<is_integral<typename C::value_type>::value, int> = 0) {
    uint32_t r1 = 0, r2 = 0;
    for (auto i : s) {
        r1 = r1 * B1 + (i + 1);
        r2 = r2 * B2 + (i + 1);
    }
    return cal_hash(mix(r1, r2));
}

}

using hash_impl::cal_hash;

class cryptor {
public:
    virtual bytevec encrypt(const bytevec& data) const = 0;
    virtual bytevec decrypt(const bytevec& data) const = 0;
};

class not_a_cryptor : public cryptor {
public:
    virtual bytevec encrypt(const bytevec& data) const { return data; }
    virtual bytevec decrypt(const bytevec& data) const { return data; }
};

class xorplus_cryptor : public cryptor {
public:
    uint64_t k;
    xorplus_cryptor(uint64_t key) : k(key) {}
    virtual bytevec encrypt(const bytevec& data) const {
        bytevec b;
        obitstream os(b);
        uint64_t h = cal_hash(k);
        os.puti<uint64_t>(cal_hash(h));
        int c = 0;
        for (const uint8_t& v : data) {
            uint8_t f1 = h >> ((c & 7) << 3), f2 = h >> (((c + 4) & 7) << 3);
            os.puti<uint8_t>((v ^ f1) + f2);
        }
        os.flush();
        return b;
    }

    virtual bytevec decrypt(const bytevec& data) const {
        bytevec b;
        ibitstream is(data);
        uint64_t h = cal_hash(k);
        if (cal_hash(h) != is.geti<uint64_t>())
            throw std::runtime_error(decrypt_failed_str);
        int c = 0;
        while (!is.eof()) {
            uint8_t f1 = h >> ((c & 7) << 3), f2 = h >> (((c + 2) & 7) << 3);
            b.push_back((is.geti<uint8_t>() - f2) ^ f1);
        }
        return b;
    }
};

}

using encrypt::cryptor;
using encrypt::xorplus_cryptor;

#endif