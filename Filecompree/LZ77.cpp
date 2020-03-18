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
	//���Դ�ļ�С��MIN_MATCH ���򲻴���
	//��ȡ�ļ���С
	FILE* fin = fopen(strfilepath.c_str(), "rb");
	if (nullptr == fin) {
		std::cout << "open false" << std::endl;
		return;
	}
	fseek(fin, 0, SEEK_END);
	ULL filesize = ftell(fin);//Դ�ļ���size
	if (filesize <= MIN_MATCH) {
		std::cout << "file small" << std::endl;
		return;
	}
	//��ѹ���ļ���ȡһ��������������
	fseek(fin, 0, SEEK_SET);
	USH  lookahead = fread(_pwin, 1, 2 * WSIZE, fin);//ʵ�ʶ�ȡ����
	USH start = 0;
	//�������ر���
	USH hashaddr = 0;
	USH matchhead = 0;
	USH curmatchlength = 0;
	USH curmatchdist = 0;
	//��д�����ر���
	UCH chflag = 0;
	UCH bitcount = 0;
	bool islen = false;
	FILE *fout = fopen("2.lzp", "wb");
	assert(fout);
	FILE* foutf = fopen("3.txt", "wb");
	assert(foutf);
	//һ��һ���ַ������
	//����ǰ�����ֽ�
	for (USH i = 0; i < MIN_MATCH - 1; ++i) {
		_ht.hashfunc(hashaddr, _pwin[i]);
	}
	//
	//ѹ��
	while (lookahead) {
		curmatchdist = 0;
		curmatchlength = 0;
		//1.����ǰ�ַ��������ϣ������ȡƥ��ͷ
		_ht.insert(matchhead, _pwin[start + 2],start,hashaddr);
		//2.��֤�ڲ��һ��������Ƿ��ҵ�ƥ��
		if (matchhead != 0) {
			//���ƥ��,�������Ⱦ����
			curmatchlength = longestmatch(matchhead, curmatchdist,start);
		}
		if (curmatchlength < MIN_MATCH) {
			//û�ҵ�
			//��startλ���ַ�д��ѹ���ļ�
			fputc(_pwin[start], fout);
			//д��ǰԭ�ַ���Ӧ�ı��
			writeflage(foutf,chflag,bitcount,false);
			++start;
			lookahead--;
			
		}
		else {
			//�ҵ���
			//�����Ⱦ����д��ѹ���ļ�  
			//��д���ȣ���д���롣Ϊ�˺�Huffman���
			UCH chlen = curmatchlength - 3;
			fputc(chlen, fout);//������3����������0��ʾ3
			fwrite(&curmatchdist, sizeof(curmatchdist), 1, fout);
			//д���
			writeflage(foutf, chflag, bitcount, true);
			lookahead -= curmatchlength;//�������л�������ʣ���ֽ���
			//���Ѿ�ƥ����ַ���������һ�齫����뵽��ϣ����
			--curmatchlength;//��ǰ�ַ����Ѿ�����
			while (curmatchlength) {
				start++;
				_ht.insert(matchhead, _pwin[start+2], start,hashaddr);
				--curmatchlength;
			}
			start++;
		}
	}
	//���λ������8λ
	if (bitcount > 0 && bitcount < 8) {
		chflag <<= (8 - bitcount);
		fputc(chflag,foutf);
	}
	//�������ļ�+����ļ��ϲ�
	fflush(foutf);
	mergefile(fout, filesize);
	fclose(fin);
	fclose(fout);
	fclose(foutf);
}
void LZ77::uncompressfile(const std::string & strfilepath) {
	//��ѹ��,����ļ�ָ��
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
	//��ȡԴ�ļ���С
	ULL filesize = 0;
	fseek(fin2, 0 - sizeof(filesize), SEEK_END);
	fread(&filesize, sizeof(filesize), 1, fin2);
	//��ȡ����ֽ���
	size_t flagsize = 0;
	fseek(fin2, 0 - sizeof(flagsize) - sizeof(filesize), SEEK_END);
	fread(&flagsize, sizeof(flagsize), 1, fin2);
	//�����ָ���ƶ���������ݵ���ʼλ��
	fseek(fin2, 0 - sizeof(flagsize) - flagsize-sizeof(filesize), SEEK_END);
	//��ʼ��ѹ��
	//д���ѹ������
	FILE* fout = fopen("4.txt", "wb");
	assert(fout);
	FILE* f = fopen("4.txt", "rb");//��ȡƥ������
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
			//��len
			matchlen = fgetc(fin1) + 3;
			encodecount += matchlen;
			fflush(fout);//���Ե���ջ�����
			fread(&matchdist, sizeof(matchdist), 1, fin1);
			//��λ�ļ�ָ��
			fseek(f, 0-matchdist, SEEK_END);
			while (matchlen) {
				//chΪ255����Ϊ�����ڻ���������ûд��
				ch = fgetc(f);
				fputc(ch, fout);
				matchlen--;
			}
		}
		else {
			//ԭ�ַ�
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