#ifndef GAO_HPP
#define GAO_HPP

#include "common.hpp"
#include "pack.hpp"
#include "compress.hpp"
#include "encrypt.hpp"

namespace gao {

using namespace detail;

bytevec gao(const wstring& path_str, const compressor& comp, const cryptor& crypt) {
    bytevec packed_data = pack::pack(path_str);
    bytevec compressed_data = comp.compress(packed_data);
    bytevec encrypted_data = crypt.encrypt(compressed_data);
    return encrypted_data;
}

int ungao(const wstring& path_str, const bytevec& encrypted_data, const compressor& comp, const cryptor& crypt) {
    try {
        bytevec decrypted_data = crypt.decrypt(encrypted_data);
        bytevec decompressed_data = comp.decompress(decrypted_data);
        pack::unpack(path_str, decompressed_data);
    }
    catch (std::runtime_error e) {
        if (e.what() == encrypt::decrypt_failed_str)
            ;
        //if (e.what() ==)
    }
    catch (std::exception e) {
        //cerr << e.what() << endl;
    }
    return 0;
}


}

#endif