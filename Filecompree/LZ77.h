#pragma once
#include"lz77hashtable.h"
#include"common.h"
#include<string>

class LZ77 {
public:
	LZ77();
	~LZ77();
	void compressfile(const std::string & strfilepath);
	void uncompressfile(const std::string & strfilepath);
private:
	//是在查找缓冲区中进行，查找缓冲区中可能会找到多个匹配，取最长的匹配链
	//输出最长匹配
	//遇到环状链，解决----设置最大匹配次数MAX_DIST
	USH longestmatch(USH matchhead, USH &curmatchdist,USH _start) {
		UCH curmatchlen = 0;//一次匹配的长度
		UCH maxmatch = curmatchlen;
		UCH maxmatchcount = 255;
		USH curmatchstart = 0;//当前匹配在查找缓冲区中起始位置
		//在先行缓冲区查找匹配时不能超过max_dist
		USH limit = _start > MAX_DIST ? _start - MAX_DIST : 0;
		do {
			//匹配范围
			//先行缓冲区
			UCH* pstart = _pwin + _start;
			UCH* pend = pstart + MAX_MATCH;
			//查找缓冲区的匹配串的起始
			UCH * pmatchstart = _pwin + matchhead;
			curmatchlen = 0;
			//进行匹配
			while (pstart < pend&&*pstart == *pmatchstart) {
				curmatchlen++;
				pstart++;
				pmatchstart++;
			}
			//一次匹配结束
			if (curmatchlen > maxmatch) {
				maxmatch = curmatchlen;
				curmatchstart = matchhead;
			}
		} while ((matchhead=_ht.getnext(matchhead))>limit&&maxmatchcount--);
		curmatchdist = _start - curmatchstart;
		return maxmatch;
	}
	//chflag :用来区分当前字节是原字符还是长度
	// 0：原字符
	//1：长度
	//bitcount：该字节中有几位bit位已被设置
	//islen：该字节是原字符还是长度
	void writeflage(FILE* fout, UCH& chflag, UCH& bitcount, bool islen) {
		chflag <<= 1;
		if (islen) {
			chflag |= 1;
		}
		bitcount++;
		if (bitcount == 8) {
			//将该字节压缩
			fputc(chflag, fout);
			chflag = 0;
			bitcount = 0;
		}
	}
private:
	UCH* _pwin;  //用来保存待压缩数据的缓冲区
	LZhashtable _ht;
};

