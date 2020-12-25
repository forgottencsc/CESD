#include "..\compress.hpp"
#include <bits/stdc++.h>
using namespace std;

int main(void) {

    const int N = 114514;
    bytevec b(N);
    mt19937_64 mt;
    
    compress::huffman_compessor h;
    compress::compressor& c = h;

    bytevec b2 = c.compress(b);
    bytevec b3 = c.decompress(b2);

    cout << b.size() << ' ' << b2.size() << ' ' << b3.size();

    return 0;
}