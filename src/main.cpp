#include <bits/stdc++.h>
#include "gao.hpp"

using namespace std;

using namespace std::string_literals;

using std::experimental::filesystem::path;

int main(void) {
    uint64_t key;
    string type, ipath, opath;
    cin >> type;

    if (type == "compress") {
        cin >> ipath >> key >> opath;
        huffman_compessor c;
        xorplus_cryptor x(key);
        ofstream ofs(path(opath).string(), std::ios::binary);
        detail::writefile(ofs, gao::gao(ipath, c, x));
        ofs.close();
    }
    else if (type == "decompress") {
        cin >> ipath >> key >> opath;
        huffman_compessor c;
        xorplus_cryptor x(key);
        ifstream ifs(path(ipath).string(), std::ios::binary);
        gao::ungao(opath, detail::readfile(ifs), c, x);
        ifs.close();
    }
    else {
        cout << "Usage: <type> <ipath> <key> <opath>" << endl;
    }
        
    return 0;
}