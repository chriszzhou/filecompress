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
	//���ڲ��һ������н��У����һ������п��ܻ��ҵ����ƥ�䣬ȡ���ƥ����
	//����ƥ��
	//������״�������----�������ƥ�����MAX_DIST
	USH longestmatch(USH matchhead, USH &curmatchdist,USH _start) {
		UCH curmatchlen = 0;//һ��ƥ��ĳ���
		UCH maxmatch = curmatchlen;
		UCH maxmatchcount = 255;
		USH curmatchstart = 0;//��ǰƥ���ڲ��һ���������ʼλ��
		//�����л���������ƥ��ʱ���ܳ���max_dist
		USH limit = _start > MAX_DIST ? _start - MAX_DIST : 0;
		do {
			//ƥ�䷶Χ
			//���л�����
			UCH* pstart = _pwin + _start;
			UCH* pend = pstart + MAX_MATCH;
			//���һ�������ƥ�䴮����ʼ
			UCH * pmatchstart = _pwin + matchhead;
			curmatchlen = 0;
			//����ƥ��
			while (pstart < pend&&*pstart == *pmatchstart) {
				curmatchlen++;
				pstart++;
				pmatchstart++;
			}
			//һ��ƥ�����
			if (curmatchlen > maxmatch) {
				maxmatch = curmatchlen;
				curmatchstart = matchhead;
			}
		} while ((matchhead=_ht.getnext(matchhead))>limit&&maxmatchcount--);
		curmatchdist = _start - curmatchstart;
		return maxmatch;
	}
	//chflag :�������ֵ�ǰ�ֽ���ԭ�ַ����ǳ���
	// 0��ԭ�ַ�
	//1������
	//bitcount�����ֽ����м�λbitλ�ѱ�����
	//islen�����ֽ���ԭ�ַ����ǳ���
	void writeflage(FILE* fout, UCH& chflag, UCH& bitcount, bool islen) {
		chflag <<= 1;
		if (islen) {
			chflag |= 1;
		}
		bitcount++;
		if (bitcount == 8) {
			//�����ֽ�ѹ��
			fputc(chflag, fout);
			chflag = 0;
			bitcount = 0;
		}
	}
private:
	UCH* _pwin;  //���������ѹ�����ݵĻ�����
	LZhashtable _ht;
};

