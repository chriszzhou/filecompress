#include"LZ77.h"
#include<iostream>
#include<assert.h>
#include<stdlib.h>
#pragma warning(disable:4996)
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
	ULL filesize = ftell(fin);//源文件的size
	if (filesize <= MIN_MATCH) {
		std::cout << "file small" << std::endl;
		return;
	}
	//从压缩文件读取一个缓冲区的数据
	fseek(fin, 0, SEEK_SET);
	USH  lookahead = fread(_pwin, 1, 2 * WSIZE, fin);//实际读取多少
	USH start = 0;
	//与查找相关变量
	USH hashaddr = 0;
	USH matchhead = 0;
	USH curmatchlength = 0;
	USH curmatchdist = 0;
	//与写标记相关变量
	UCH chflag = 0;
	UCH bitcount = 0;
	bool islen = false;
	FILE *fout = fopen("2.lzp", "wb");
	assert(fout);
	FILE* foutf = fopen("3.txt", "wb");
	assert(foutf);
	//一个一个字符计算的
	//处理前两个字节
	for (USH i = 0; i < MIN_MATCH - 1; ++i) {
		_ht.hashfunc(hashaddr, _pwin[i]);
	}
	//
	//压缩
	while (lookahead) {
		curmatchdist = 0;
		curmatchlength = 0;
		//1.将当前字符串插入哈希表，并获取匹配头
		_ht.insert(matchhead, _pwin[start + 2],start,hashaddr);
		//2.验证在查找缓冲区中是否找到匹配
		if (matchhead != 0) {
			//找最长匹配,带出长度距离对
			curmatchlength = longestmatch(matchhead, curmatchdist,start);
		}
		if (curmatchlength < MIN_MATCH) {
			//没找到
			//将start位置字符写入压缩文件
			fputc(_pwin[start], fout);
			//写当前原字符对应的标记
			writeflage(foutf,chflag,bitcount,false);
			++start;
			lookahead--;
			
		}
		else {
			//找到了
			//将长度距离对写入压缩文件  
			//先写长度，在写距离。为了和Huffman结合
			UCH chlen = curmatchlength - 3;
			fputc(chlen, fout);//最少是3个，所以用0表示3
			fwrite(&curmatchdist, sizeof(curmatchdist), 1, fout);
			//写标记
			writeflage(foutf, chflag, bitcount, true);
			lookahead -= curmatchlength;//更新先行缓冲区中剩余字节数
			//将已经匹配的字符串按三个一组将其插入到哈希表中
			--curmatchlength;//当前字符串已经插入
			while (curmatchlength) {
				start++;
				_ht.insert(matchhead, _pwin[start+2], start,hashaddr);
				--curmatchlength;
			}
			start++;
		}
	}
	//标记位数不足8位
	if (bitcount > 0 && bitcount < 8) {
		chflag <<= (8 - bitcount);
		fputc(chflag,foutf);
	}
	//将数据文件+标记文件合并
	fflush(foutf);
	mergefile(fout, filesize);
	fclose(fin);
	fclose(fout);
	fclose(foutf);
}
void LZ77::uncompressfile(const std::string & strfilepath) {
	//打开压缩,标记文件指针
	FILE* fin1 = fopen(strfilepath.c_str(), "rb");
	if (nullptr == fin1) {
		std::cout << "open false" << std::endl;
		return;
	}
	FILE* fin2 = fopen(strfilepath.c_str(), "rb");
	if (nullptr == fin2) {
		std::cout << "open false" << std::endl;
		return;
	}
	//获取源文件大小
	ULL filesize = 0;
	fseek(fin2, 0 - sizeof(filesize), SEEK_END);
	fread(&filesize, sizeof(filesize), 1, fin2);
	//获取标记字节数
	size_t flagsize = 0;
	fseek(fin2, 0 - sizeof(flagsize) - sizeof(filesize), SEEK_END);
	fread(&flagsize, sizeof(flagsize), 1, fin2);
	//将标记指针移动到标记数据的起始位置
	fseek(fin2, 0 - sizeof(flagsize) - flagsize-sizeof(filesize), SEEK_END);
	//开始解压缩
	//写入解压缩数据
	FILE* fout = fopen("4.txt", "wb");
	assert(fout);
	FILE* f = fopen("4.txt", "rb");//读取匹配内容
	if (nullptr == f) {
		std::cout << "open false" << std::endl;
		return;
	}
	UCH bitcount = 0;
	UCH chflag = 0;
	UCH ch = 0;
	UCH matchlen = 0;
	USH matchdist = 0;
	ULL encodecount = 0;
	while (encodecount<filesize) {
		if (bitcount == 0) {
			chflag = fgetc(fin2);
			bitcount = 8;
		}
		if (chflag & 0x80) {
			//是len
			matchlen = fgetc(fin1) + 3;
			encodecount += matchlen;
			fflush(fout);//所以得清空缓冲区
			fread(&matchdist, sizeof(matchdist), 1, fin1);
			//定位文件指针
			fseek(f, 0-matchdist, SEEK_END);
			while (matchlen) {
				//ch为255，因为数据在缓冲区，还没写入
				ch = fgetc(f);
				fputc(ch, fout);
				matchlen--;
			}
		}
		else {
			//原字符
			ch = fgetc(fin1);
			fputc(ch, fout);
			encodecount++;
		}
		chflag <<= 1;
		bitcount--;
	}
	fclose(fin1);
	fclose(fin2);
	fclose(fout);
	fclose(f);
}
void LZ77::mergefile(FILE* fout,ULL filesize) {
	FILE* finf = fopen("3.txt", "rb");
	size_t flagsize = 0;
	UCH* preadbuff = new UCH[1024];
	while (true) {
		size_t rdsize = fread(preadbuff, 1, 1024, finf);
		if (0 == rdsize)
			break;
		fwrite(preadbuff, 1, rdsize, fout);
		flagsize += rdsize;
	}
	fwrite(&flagsize, sizeof(flagsize), 1, fout);
	fwrite(&filesize, sizeof(filesize), 1, fout);
	delete[] preadbuff;
	fclose(finf);
}