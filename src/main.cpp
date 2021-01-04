#include <bits/stdc++.h>
#include "gao.hpp"
#include "verify.hpp"
#include "common.hpp"
using namespace std;
using namespace detail;

int main(void) {
    uint64_t key;
    string type, ipath, opath;
    cin >> type;

    if (type == "compress") {
        cin >> ipath >> key >> opath;
        huffman_compessor c;
        xorplus_cryptor x(key);
        ofstream ofs(opath, std::ios::binary);
        detail::writefile(ofs, gao::gao(ipath, c, x));
        ofs.close();
    }
    else if (type == "decompress") {
        cin >> ipath >> key >> opath;
        huffman_compessor c;
        xorplus_cryptor x(key);
        ifstream ifs(ipath, std::ios::binary);
        gao::ungao(opath, detail::readfile(ifs), c, x);
        ifs.close();
    }
    else if (type == "verify") {
        cin >> ipath >> key >> opath;
        huffman_compessor c;
        xorplus_cryptor x(key);
        ifstream ifs(ipath, std::ios::binary);
        bytevec decrypted_data = x.decrypt(readfile(ifs));
        ifs.close();
        bytevec decompressed_data = c.decompress(decrypted_data);

        string result = verify::verify(decompressed_data, opath);
        if (result == "") {
            cout << "succ" << endl;
        }
        else {
            cout << result << endl;
        }
    }
    else {
        cout << "Usage: <type> <ipath> <key> <opath>" << endl;
    }
    return 0;
}