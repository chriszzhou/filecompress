#include"LZ77.h"
#include<iostream>
#include<assert.h>

LZ77::LZ77()
	:_pwin(new UCH[WSIZE*2])
	,_ht(WSIZE){}
LZ77::~LZ77() {
	delete[] _pwin;
	_pwin = nullptr;
}
void LZ77::compressfile(const std::string & strfilepath) {
	//如果源文件小于MIN_MATCH ，则不处理
	//获取文件大小
	FILE* fin = fopen(strfilepath.c_str(), "rb");
	if (nullptr == fin) {
		std::cout << "open false" << std::endl;
		return;
	}
	fseek(fin, 0, SEEK_END);
	ULL filesize = ftell(fin);
	if (filesize <= MIN_MATCH) {
		std::cout << "file small" << std::endl;
		return;
	}
	//从压缩文件读取一个缓冲区的数据
	FILE *fout = fopen("2.lzp", "wr");
	assert(fout);
	USH lookahead = fread(_pwin, 1, 2 * WSIZE, fin);//实际读取多少
	USH start = 0;
	USH hashaddr = 0;
	USH matchhead = 0;
	USH curmatchlength = 0;
	USH curmatchdist = 0;
	//一个一个字符计算的
	//处理前两个字节
	for (USH i = 0; i < MIN_MATCH - 1; ++i) {
		_ht.hashfunc(hashaddr, _pwin[i]);
	}
	//压缩
	while (lookahead) {
		//1.将当前字符串插入哈希表，并获取匹配头
		_ht.insert(matchhead, _pwin[start + 2],start,hashaddr);
		//2.验证在查找缓冲区中是否找到匹配
		if (matchhead != 0) {
			//找最长匹配,带出长度距离对
			curmatchlength = longestmatch(matchhead, curmatchdist);
		}
		if (curmatchlength < MIN_MATCH) {
			//没找到
			//将start位置字符写入压缩文件
			++start;
			lookahead--;
		}
		else {
			//找到了
			//将长度距离对写入压缩文件  
			//先写长度，在写距离。为了和Huffman结合
			fputc(curmatchlength, fout);
			fwrite(&curmatchdist, sizeof(curmatchdist), 1, fout);
			lookahead -= curmatchlength;//更新先行缓冲区中剩余字节数
			//将已经匹配的字符串按三个一组将其插入到哈希表中
			--curmatchlength;//当前字符串已经插入
			while (curmatchlength) {
				start++;
				_ht.insert(matchhead, _pwin[start+2], start,hashaddr);
				--curmatchlength;
			}
		}
	}
}
void LZ77::uncompressfile(const std::string & strfilepath) {

}
