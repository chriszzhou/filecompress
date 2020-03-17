#include"lz77hashtable.h"
#include<string.h>

LZhashtable::LZhashtable(USH size) 
	:_prev(new USH[size*2])
	,_head(_prev+size)
{
	memset(_prev, 0, size * 2 * sizeof(USH));
}
LZhashtable::~LZhashtable(){
	delete _prev;
	_prev = nullptr;
}
//ch :本次匹配的字符串（三个字符)的最后一个字符
//本次的哈希地址是在上一次的基础上算出来的
void  LZhashtable::hashfunc(USH& hashaddr, UCH ch) {
	 hashaddr = (((hashaddr) << H_SHIFT()) ^ (ch))&HASH_MASK;

}
USH LZhashtable::H_SHIFT() {
	return (HASH_BITS + MIN_MATCH - 1) / MIN_MATCH;
}
void LZhashtable::insert(USH& matchhead, UCH ch, USH pos, USH& hashaddr) {
	hashfunc(hashaddr, ch);
	//找到离当前匹配字符最近的匹配链
	matchhead = _head[hashaddr];
	//pos可能会超过32k，&MASK目的，不越界
	_prev[pos&HASH_MASK] = _head[hashaddr];
	_head[hashaddr] = pos;

}
USH LZhashtable::getnext(USH matchhead) {
	return _prev[matchhead&HASH_MASK];
}