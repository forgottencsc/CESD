#ifndef VERIFY_HPP
#define VERIFY_HPP
#include "common.hpp"
#include "pack.hpp"


namespace verify{
using namespace std;
using namespace detail;
using namespace pack;

typedef pair<string, char> file;
map<file, bytevec>unpack_to_map(const bytevec& data) {
    map<file, bytevec>files;

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

    struct info{
        string name;
        uint number_of_sons;
    };
    
    queue<info>q;
    q.push({"", 1}); 
    while (q.size()) {
        auto item = q.front(); q.pop();
        string thisdir = item.name;
        uint sz = item.number_of_sons;
        while (sz--) {
            string filename = get_filename();
            string filepath = (thisdir == "") ? filename : thisdir + "\\" + filename;
            char dir_tag = data[ptr++];
            if (dir_tag == '1') {
                cerr << "Get dir " << filepath << endl;
                files[{filepath, dir_tag}] = {};
            }
            else {
                cerr << "Get file " << filepath << endl;
                bytevec content = get_content();
                files[{filepath, dir_tag}] = content;
            }
            uint number_of_sons = get_sonnum();
            q.push({filepath, number_of_sons});
        }
    }
    return files;
}

void outlog(string &s, string log) {
    s += log + '\n';
    // cerr << log << endl;
}

string verify(const bytevec& data, const string& path_str) {
    string logs = "";
    auto files = unpack_to_map(data);

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

    string pre = path_str.substr(0, lb);
    string name = path_str.substr(lb, rb - lb);
    
    map<file, bool>thisfiles;

    queue<info>q;
    q.push({name, name});
    while (q.size()) {
        info item = q.front(); q.pop();
        cerr << "goto " << item.filename << endl;
        uint number_of_sons = 0;
        struct stat s_buf;
        string path = pre + item.path;
        stat(path.c_str(), &s_buf);
        if (!S_ISDIR(s_buf.st_mode)) { // not a directory
            thisfiles[{item.path, '0'}] = 1;

            ifstream fin(path, ios::binary);
            bytevec data = readfile(fin); // get data
            fin.close();
            if (!files.count({item.path, '0'})) {
                outlog(logs, "files " + item.path + " is not in compressed data.");
            }
            else {
                if (files[{item.path, '0'}] != data) {
                    outlog(logs, "files " + item.path + " is inconsistent in content.");
                }
            }
        }
        else {
            DIR *dir = NULL;
            struct dirent *entry;
            if ((dir = opendir(path.c_str())) == NULL) {
                cerr << "Directory Open failed." << endl;
            }
            else {
                thisfiles[{item.path, '1'}] = 1;
                if (!files.count({item.path, '1'})) {
                    outlog(logs, "directory " + item.path + " is not in compressed data.");
                }
                else {
                    // do nothing
                }
                while (entry = readdir(dir)) {
                    string filename_str = entry->d_name;
                    if (filename_str != "." && filename_str != "..") {
                        q.push({item.path + "\\" + filename_str, filename_str});
                        number_of_sons++;
                    }
                }
            }
        }
    }

    for (auto item: files) {
        if (!thisfiles.count(item.first)) {
            if (item.first.second == '0') {
                outlog(logs, "files " + item.first.first + " is not in " + path_str + ".");
            }
            else {
                outlog(logs, "directory " + item.first.first + " is not in " + path_str + ".");
            }
        }
    }
    return logs;
}

};

#endif