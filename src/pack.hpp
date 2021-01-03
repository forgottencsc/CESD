#ifndef PACK_HPP
#define PACK_HPP
#include "common.hpp"
#include <windows.h>
#include <direct.h>
#include <dirent.h>
#include <sys/stat.h>

namespace pack {

typedef std::vector<uint8_t> bytevec;
typedef unsigned int uint;
using namespace std;

const int NUMLEN = 4;
const int TAGLEN = 1;

bytevec itob32(uint x) {
    bytevec res;
    for (int i = 0; i < NUMLEN; i++) {
        res.push_back(x & ((1 << 8) - 1));
        x >>= 8;
    }
    reverse(res.begin(), res.end());
    return res;
}

uint btoi32(const bytevec& s) {
    uint res = 0;
    for (auto c: s) {
        res = res << 8 | c;
    }
    return res;
}

bytevec subvec(const bytevec& vec, const uint& first, const uint& last) {
    // vec[first, last)
    bytevec res;
    for (auto it = vec.begin() + first; it != vec.begin() + last; it++) {
        res.push_back(*it);
    }
    return res;
}

bytevec readfile(istream& in) {
    bytevec bdata;
    const size_t sz = 1<<10;
    array<uint8_t, sz> buf;
    size_t len;
    while (len = in.readsome(reinterpret_cast<char*>(buf.data()), sz)) {
        for (int i = 0; i < len; i ++)
            bdata.push_back(buf[i]);
    }
    return bdata;
}

void writefile(ostream& out, const bytevec& vec) {
    out.write(reinterpret_cast<const char*>(vec.data()), vec.size());
}

bytevec pack(const string& path_str) {
    // format: [filename_len, filename, isdir_flag, data_len, data, number_of_sons]
    bytevec packed_data;
    struct info{
        string path;
        string filename;
    };

    // get the stem of the path
    uint lb = 0, rb = path_str.size();
    if (path_str.back() == '\\') {
        string tmp_s = path_str;
        tmp_s.pop_back();
        rb--;
        lb = tmp_s.find_last_of('\\') + 1;
    }
    else {
        lb = path_str.find_last_of('\\') + 1;
    }
    string name = path_str.substr(lb, rb - lb);
    
    queue<info>q;
    q.push({path_str, name});
    while (q.size()) {
        info item = q.front(); q.pop();
        bytevec namelen_str = itob32(item.filename.size()); // get len
        packed_data.insert(packed_data.end(), namelen_str.begin(), namelen_str.end()); // fill in filename_len
        packed_data.insert(packed_data.end(), item.filename.begin(), item.filename.end()); // fill in filename 

        uint number_of_sons = 0;
        struct stat s_buf;
        stat(item.path.c_str(), &s_buf);
        if (!S_ISDIR(s_buf.st_mode)) { // not a directory
            packed_data.push_back('0');

            ifstream fin(item.path, ios::binary);
            bytevec data = readfile(fin); // get data
            fin.close();

            bytevec datalen_str = itob32(data.size()); // get data_len
            packed_data.insert(packed_data.end(), datalen_str.begin(), datalen_str.end()); // fill in data_len
            packed_data.insert(packed_data.end(), data.begin(), data.end()); // fill in data
        }
        else {
            packed_data.push_back('1'); // is a directory
            DIR *dir = NULL;
            struct dirent *entry;
            if ((dir = opendir(item.path.c_str())) == NULL) {
                cerr << "Directory Open failed." << endl;
            }
            else {
                while (entry = readdir(dir)) {
                    string filename_str = entry->d_name;
                    if (filename_str != "." && filename_str != "..") {
                        q.push({item.path + "\\" + filename_str, filename_str});
                        number_of_sons++;
                    }
                }
            }
        }
        bytevec sons_str = itob32(number_of_sons); // get number_of_sons
        packed_data.insert(packed_data.end(), sons_str.begin(), sons_str.end()); // fill in number_of_sons
    }
    return packed_data;
}

void unpack(const string &path_str, const bytevec& data) {
    // format: [filename_len, filename, isdir_flag, data_len, data, number_of_sons]
    uint ptr = 0;

    auto get_filename = [&]() {
        uint namelen = btoi32(subvec(data, ptr, ptr + NUMLEN));
        ptr += NUMLEN;
        bytevec namevec = subvec(data, ptr, ptr + namelen);
        ptr += namelen;
        string filename = ""; for (auto c: namevec) filename += c;
        return filename;
    };
    
    auto get_content = [&]() {
        uint content_len = btoi32(subvec(data, ptr, ptr + NUMLEN));
        ptr += NUMLEN;
        bytevec content = subvec(data, ptr, ptr + content_len);
        ptr += content_len;
        return content;
    };

    auto get_sonnum = [&]() {
        uint res = btoi32(subvec(data, ptr, ptr + NUMLEN));
        ptr += NUMLEN;
        return res;
    };

    mkdir(path_str.c_str());
    struct info{
        string name;
        uint number_of_sons;
    };
    
    queue<info>q;
    q.push({path_str, 1}); 
    while (q.size()) {
        auto item = q.front(); q.pop();
        string thisdir = item.name;
        uint sz = item.number_of_sons;
        while (sz--) {
            string filename = get_filename();
            cerr << "Filename = " << filename << endl;
            string filepath = thisdir + "\\" + filename;
            char dir_tag = data[ptr++];
            if (dir_tag == '1') {
                cerr << "Create dir " << filepath << endl;
                mkdir(filepath.c_str());
            }
            else {
                cerr << "Create file " << filepath << endl;
                ofstream fout(filepath.c_str(), ios::binary);
                bytevec content = get_content();
                writefile(fout, content);
                fout.close();
            }
            uint number_of_sons = get_sonnum();
            q.push({filepath, number_of_sons});
        }
    }
}

}

#endif