#ifndef GAO_HPP
#define GAO_HPP

#include "common.hpp"
#include "pack.hpp"
#include "compress.hpp"
#include "encrypt.hpp"

namespace gao {

using namespace detail;

bytevec gao(const string& path_str, const compressor& comp, const cryptor& crypt) {
    bytevec packed_data = pack::pack(path_str);
    std::cerr << "packed" << '\n';
    bytevec compressed_data = comp.compress(packed_data);
    std::cerr << "compressed" << '\n';
    bytevec encrypted_data = crypt.encrypt(compressed_data);
    std::cerr << "crypted" << '\n';
    return encrypted_data;
}

int ungao(const string& path_str, const bytevec& encrypted_data, const compressor& comp, const cryptor& crypt) {
    //try {
        bytevec decrypted_data = crypt.decrypt(encrypted_data);
        std::cerr << "decryped" << '\n';
        bytevec decompressed_data = comp.decompress(decrypted_data);
        std::cerr << "decompressed" << '\n';
        pack::unpack(path_str, decompressed_data);
        std::cerr << "unpacked" << '\n';
    //}
    // catch (std::runtime_error e) {
    //     if (e.what() == encrypt::decrypt_failed_str)
    //         ;
    //     //if (e.what() ==)
    // }
    // catch (std::exception e) {
    //     std::cerr << e.what() << std::endl;
    // }
    return 0;
}


}

#endif