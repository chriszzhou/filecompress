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
	UCH* _pwin;  //���������ѹ�����ݵĻ�����
	LZhashtable _ht;
};

