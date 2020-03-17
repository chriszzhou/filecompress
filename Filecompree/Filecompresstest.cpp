#include<iostream>
#if 0
#include"FilecompressHuff.h"
using namespace std;
int main() {
			FilecompressHuff fc;
			fc.compressFile("test.txt");
			fc.uncompressFile("fout.txt");

	return 0;
}
#endif
#include"LZ77.h"
int main() {
	LZ77 lz;
	lz.uncompressfile("2.lzp");
	return 0;
}