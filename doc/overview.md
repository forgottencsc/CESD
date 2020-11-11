## Overview

模块：

Compress：字节串的压缩与解压缩。

Package：将一堆东西压成字节串和将字节串。

Encrypt：字节串的加密与解密与验证。

Web：将字节串发送至服务器端。

## Main

流程：

`bytevec backup(const string& path_str)`

将`path_str`指向的东西压成一个字节串，即

`void ungao(const string& path_str, const bytevec& data)`

将`data`里的东西释放至`path_str`下。

```cpp
typedef vector<uint8_t> bytevec;

bytevec gao(const string& path_str,) {
    bytevec pack_data = pack(path_str);
    
    
}

void ungao(const string& path_str, const bytevec& data) {
    
    
}
```

## Compress

```cpp
bytevec compress(const bytevec& data);
bytevec decompress(const bytevec& data);
```

## Package

定义

```cpp
bytevec pack(const string& path_str);
void unpack(const string& path_str, const bytevec& data);
```

## Encrypt

`class cryptor`是一个接口，其子类实现了某种加密方法的加密/解密/验证。

encrypt将key的md5值放在data首部。

decrypt计算key的md5值与data首部的md5值比较，若失败则抛出异常。

```cpp
class cryptor {
public:
    bytevec encrypt(const bytevec& data, uint64_t key);
    bytevec decrypt(const bytevec& data, uint64_t key);
};

class xor_cryptor : public cryptor;
```

