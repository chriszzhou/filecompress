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
	//���Դ�ļ�С��MIN_MATCH ���򲻴���
	//��ȡ�ļ���С
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
	//��ѹ���ļ���ȡһ��������������
	FILE *fout = fopen("2.lzp", "wr");
	assert(fout);
	USH lookahead = fread(_pwin, 1, 2 * WSIZE, fin);//ʵ�ʶ�ȡ����
	USH start = 0;
	USH hashaddr = 0;
	USH matchhead = 0;
	USH curmatchlength = 0;
	USH curmatchdist = 0;
	//һ��һ���ַ������
	//����ǰ�����ֽ�
	for (USH i = 0; i < MIN_MATCH - 1; ++i) {
		_ht.hashfunc(hashaddr, _pwin[i]);
	}
	//ѹ��
	while (lookahead) {
		//1.����ǰ�ַ��������ϣ������ȡƥ��ͷ
		_ht.insert(matchhead, _pwin[start + 2],start,hashaddr);
		//2.��֤�ڲ��һ��������Ƿ��ҵ�ƥ��
		if (matchhead != 0) {
			//���ƥ��,�������Ⱦ����
			curmatchlength = longestmatch(matchhead, curmatchdist);
		}
		if (curmatchlength < MIN_MATCH) {
			//û�ҵ�
			//��startλ���ַ�д��ѹ���ļ�
			++start;
			lookahead--;
		}
		else {
			//�ҵ���
			//�����Ⱦ����д��ѹ���ļ�  
			//��д���ȣ���д���롣Ϊ�˺�Huffman���
			fputc(curmatchlength, fout);
			fwrite(&curmatchdist, sizeof(curmatchdist), 1, fout);
			lookahead -= curmatchlength;//�������л�������ʣ���ֽ���
			//���Ѿ�ƥ����ַ���������һ�齫����뵽��ϣ����
			--curmatchlength;//��ǰ�ַ����Ѿ�����
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
