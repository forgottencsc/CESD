#include "common.hpp"
#include "pack.hpp"

using namespace std;
int main() {
    wstring src_path_str; wcin >> src_path_str;
    bytevec packed_data = pack::pack(src_path_str);

    wstring dst_path_str; wcin >> dst_path_str;
    pack::unpack(dst_path_str, packed_data);
    return 0;
}