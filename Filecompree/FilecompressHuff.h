#pragma once
#include<string>
#include<assert.h>
#include<vector>
#include"huffmantree.hpp"
#pragma warning(disable:4996)
struct charInfo {
	char _ch;
	size_t _count;
	std::string _strcode;
	charInfo(const int count) 
		:_count(count)
	{}
	charInfo()
	{}
	charInfo operator+(const charInfo& c) {
		charInfo temp;
		temp._count = _count + c._count;
		return temp;
	}
	bool operator>(const charInfo& c) {
		return _count > c._count;
	}
	bool operator==(const charInfo& c) {
		return _count == c._count;
	}
};
class FilecompressHuff {

public:
	FilecompressHuff()
	
	{
		_Filechar.resize(256);
		for (int i=0; i < 256; i++) {
			_Filechar[i]._ch = i;
			_Filechar[i]._count = 0;
	}
	}
	void compressFile(const std::string& path);
	void uncompressFile(const std::string& path);
private:
	void writehead(FILE* fout,std::string filename);
	std::string getfilepostfix(const std::string strfile);
	void gerenatehuffmancode(HuffManTreeNode<charInfo>* proot);
	void readline(FILE* file, std::string& strinfo);
private:
	std::vector<charInfo> _Filechar;
};
