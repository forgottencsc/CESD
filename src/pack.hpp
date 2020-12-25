#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <experimental/filesystem>

namespace pack{


using namespace std;
using namespace experimental::filesystem;
typedef std::vector<uint16_t> bytevec;
typedef unsigned int uint;
const int NUMLEN = 2;
const int TAGLEN = 1;
bytevec itob32(uint x) {
    bytevec res;
    for (int i = 0; i < NUMLEN; i++) {
        res.push_back(x & ((1 << 16) - 1));
        x >>= 16;
    }
    reverse(res.begin(), res.end());
    return res;
}

uint btoi32(const bytevec& s) {
    uint res = 0;
    for (uint16_t c: s) {
        res = res << 16 | c;
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

wstring subwstr(const wstring& s, const uint& first, const uint& last) {
    wstring res = L"";
    for (int i = first; i < last; i++)
        res += s[i];
    return res;
}

bytevec getcontent(ifstream &in) {
    bytevec bdata;
    while (!in.eof()) {
        char byte; in.get(byte);
        if (in.eof()) break;
        bdata.push_back(byte);
    }
    return bdata;
}


bytevec pack(const wstring& path_str) {
    bytevec packed_data;
    vector<path>files;
    queue<path>q;
    path root(path_str);
    q.push(root);
    // uint prelen = root.parent_path().string().length();
    while (q.size()) {
        path u = q.front(); q.pop(); files.push_back(u);
        wstring filename = u.wstring();
        uint prelen = u.parent_path().wstring().size();
        // cout << "before" << filename << endl;
        // cout << "prelen = " << prelen << endl;
        // cout << "size = " << filename.size() << endl;
        filename = subwstr(filename, prelen + (prelen > 0), filename.size());
        // cout << "after " << filename << endl;
        bytevec namelen = itob32(filename.size());
        packed_data.insert(packed_data.end(), namelen.begin(), namelen.end());
        packed_data.insert(packed_data.end(), filename.begin(), filename.end());
        uint son_num = 0;
        if (is_directory(u)) {
            for (auto v: directory_iterator(u)) {
                q.push(v);
                son_num++;
            }
            packed_data.push_back(L'1');
        }
        else {
            packed_data.push_back(L'0');
            ifstream fin(u.string().c_str(), ios::binary);
            bytevec file_content = getcontent(fin);
            fin.close();
            bytevec content_len = itob32(file_content.size());
            packed_data.insert(packed_data.end(), content_len.begin(), content_len.end());
            packed_data.insert(packed_data.end(), file_content.begin(), file_content.end());
        }
        bytevec len = itob32(son_num);
        packed_data.insert(packed_data.end(), len.begin(), len.end());

    }
    return packed_data;
}


void unpack(const wstring &path_str, const bytevec& data) {
    uint ptr = 0;

    auto get_filename = [&]() {
        int namelen = btoi32(subvec(data, ptr, ptr + NUMLEN));
        ptr += NUMLEN;
        bytevec namevec = subvec(data, ptr, ptr + namelen);
        ptr += namelen;
        wstring filename = L""; for (auto c: namevec) filename += c;
        // cout << "Get filename = " << filename << endl;
        return filename;
    };
    
    auto get_content = [&]() {
        int content_len = btoi32(subvec(data, ptr, ptr + NUMLEN));
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


    wstring root = path_str;
    struct file_info{
        wstring name;
        path parent;
        uint sz;
    };
    queue<file_info>q;
    q.push({root, path(path_str), 1}); 
    while (q.size()) {
        auto item = q.front(); q.pop();
        wstring u = item.name;
        uint u_sz = item.sz;
        while (u_sz--) {
            wstring v = get_filename();
            wchar_t dir_tag = data[ptr++];
            path dir_path(item.parent);
            dir_path /= path(v);
            if (dir_tag == L'1') {
                if (!exists(dir_path)) {
                    create_directory(dir_path);
                    cout << "Create " << dir_path.wstring() << endl;
                }
            }
            else {
                ofstream fout(dir_path.string(), ios::binary);
                bytevec content = get_content();
                for (auto c: content) {
                    fout.put(c);
                }
                fout.close();
            }
            uint v_sz = get_sonnum();
            q.push({v, dir_path, v_sz});
        }
    }
}

};
