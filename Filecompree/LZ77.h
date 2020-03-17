#pragma once
#include"lz77hashtable.h"
#include<string>

class LZ77 {
public:
	LZ77();
	~LZ77();
	void compressfile(const std::string & strfilepath);
	void uncompressfile(const std::string & strfilepath);
private:
	UCH* _pwin;  //用来保存待压缩数据的缓冲区
	LZhashtable _ht;
};

