#pragma once
#include"common.h"
class LZhashtable {
public:
	LZhashtable(USH size);
	~LZhashtable();
	void insert(USH& matchhead, UCH ch, USH pos, USH& hashaddr);
	void hashfunc(USH& hashaddr, UCH ch);
	USH H_SHIFT();
	USH getnext(USH matchhead);
private:
	USH * _prev;
	USH * _head;
};
