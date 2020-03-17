#include"FilecompressHuff.h"
#include"huffmantree.hpp"
#include<string>
void FilecompressHuff::compressFile(const std::string& path) {

	FILE* file1=fopen(path.c_str(),"rb");
	if (file1 == nullptr) {
		assert(false);
		return ;
	}
	//1.统计源文件中每个字符出现次数
	char* pReadBuff = new char[2048];
	int rdsize = 0;
	while (1) {
		rdsize = fread(pReadBuff, 1, 1024, file1);
		if (rdsize == 0)
			break;
		for (int i = 0; i < rdsize; i++) {
			_Filechar[pReadBuff[i]]._count++;
		}
	}
	//2.创建哈夫曼树
	HUffManTree<charInfo>t1(_Filechar,charInfo(0));
	//3.获得每个字符编码
	gerenatehuffmancode(t1.getroot());
	//4.重写文件内容
	FILE* fout = fopen("fout.txt","wb");
	if (fout == nullptr) {
		assert(false);
		return;
	}
	writehead(fout, path);
	std::string strcode;
	char ch = 0;
	size_t bitcount=0;
	fseek(file1,0,SEEK_SET);
	while (true) {

		rdsize = fread(pReadBuff, 1, 2048, file1);
		if (rdsize == 0)
			break;
		//对读取到的内容重写
		for (int i=0; i < rdsize; i++) {
			strcode=_Filechar[pReadBuff[i]]._strcode;
			for (size_t j = 0; j < strcode.size(); j++) {
				ch <<= 1;
				if ('1' == strcode[j]) {
					ch |= 1;
				}
				bitcount++;
				if (bitcount == 8) {
					fputc(ch, fout);
					ch = 0;
					bitcount = 0;
				}
			}
		}
	}
	if (bitcount < 8) {
		ch <<= (8 - bitcount);
		fputc(ch, fout);
	}
	fclose(file1);
	fclose(fout);
	delete[] pReadBuff;
}
void FilecompressHuff::gerenatehuffmancode(HuffManTreeNode<charInfo>* proot) {
	typedef HuffManTreeNode<charInfo> Node;
	if (proot == nullptr)
		return;
	else {
		gerenatehuffmancode(proot->_pleft);
		gerenatehuffmancode(proot->_pright);
	}
	if (nullptr == proot->_pleft&&nullptr == proot->_pleft) {
		Node* pcur = proot;
		Node* pParent = pcur->_pparent;
		std::string  strcode;
		while (pParent != nullptr) {
			if (pcur == pParent->_pleft) {
				strcode += '0';
			}
			if (pcur == pParent->_pright) {
				strcode += '1';
			}
			pcur = pParent;
			pParent = pParent->_pparent;
		}
		reverse(strcode.begin(), strcode.end());
		_Filechar[proot->_weight._ch]._strcode = strcode;
	}
}
void FilecompressHuff::writehead(FILE* fout,std::string filename) {
	assert(fout);
	std::string strhead; 
	strhead += getfilepostfix(filename);
	strhead += '\n';
	//行数
	size_t linecount = 0;
	std::string charcount ;
	char  value[32] = { 0 };
	for (int i = 0; i < 256; i++) {
		if (_Filechar[i]._count) {
			itoa(_Filechar[i]._count, value, 10);
			linecount++;
			charcount += _Filechar[i]._ch;
			charcount += ':';
			charcount += value;
			charcount += '\n';
		}
	}
	itoa(linecount, value, 10);
	strhead += value;
	strhead += '\n';
	//写字符次数信息
	strhead += charcount;
	fwrite(strhead.c_str(),1,strhead.size(),fout);
}
std::string FilecompressHuff::getfilepostfix(const std::string strfile) {
	return strfile.substr(strfile.rfind('.'));
}
void FilecompressHuff::uncompressFile(const std::string& path){
	FILE* file2 = fopen(path.c_str(), "rb");
	if (file2 == nullptr) {
		assert(file2);
		return;
	}
	//文件后缀
	std::string strfilepostsix;
	readline(file2, strfilepostsix);
	//字符信息行数
	std::string strfilecount;
	readline(file2, strfilecount);
	int linecount = atoi(strfilecount.c_str());
	//字符信息
	for (int i=0; i < linecount; i++) {
		std::string strfileinfo;
		readline(file2, strfileinfo);
		if (strfileinfo.empty()) {
			strfileinfo += '\n';
			readline(file2, strfileinfo);
		}
		_Filechar[strfileinfo[0]]._count = atoi(strfileinfo.c_str()+2);
	}
	//压缩信息
	//还原Huffman树
	HUffManTree<charInfo>t2(_Filechar, charInfo(0));
	//解压缩
	char* readbuff=new char[2048];
	char ch = 0;
	HuffManTreeNode<charInfo>* tnode = t2.getroot();
	size_t fcount = tnode->_weight._count;
	FILE* fout2 = fopen("3.txt", "wb");
	if (fout2 == nullptr) {
		assert(fout2);
		return;
	}
	while (true) {
		size_t rdsize=fread(readbuff,1,2048,file2);
		if (rdsize == 0)
			break;
		for (size_t i=0; i < rdsize; i++) {
			//8个bit位单独处理
			ch = readbuff[i];
			for (int j = 0; j < 8; j++) {
				if (ch & 0x80) {
					tnode = tnode->_pright;
				}
				else {
					tnode = tnode->_pleft;
				}
				ch = ch << 1;
				if (fcount == 0) {
					break;
				}
				if (tnode->_pleft == nullptr&&tnode->_pright == nullptr) {
					fputc(tnode->_weight._ch,fout2);
					tnode = t2.getroot();
					fcount--;
				}
			}
		}
	}
	delete[] readbuff;
	fclose(fout2);
	fclose(file2);

}
void FilecompressHuff::readline(FILE* file, std::string& strinfo) {
	assert(file);
	char ch;
	while (!feof(file)) {
		ch = fgetc(file);
		if (ch == '\n')
			break;
		strinfo += ch;
	}
}