#include<iostream>
#include"FilecompressHuff.h"
using namespace std;
int main() {
			FilecompressHuff fc;
			fc.compressFile("test.txt");
			fc.uncompressFile("fout.txt");

	return 0;
}
