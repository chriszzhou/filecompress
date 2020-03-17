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
//ch :����ƥ����ַ����������ַ�)�����һ���ַ�
//���εĹ�ϣ��ַ������һ�εĻ������������
void  LZhashtable::hashfunc(USH& hashaddr, UCH ch) {
	 hashaddr = (((hashaddr) << H_SHIFT()) ^ (ch))&HASH_MASK;

}
USH LZhashtable::H_SHIFT() {
	return (HASH_BITS + MIN_MATCH - 1) / MIN_MATCH;
}
void LZhashtable::insert(USH& matchhead, UCH ch, USH pos, USH& hashaddr) {
	hashfunc(hashaddr, ch);
	//�ҵ��뵱ǰƥ���ַ������ƥ����
	matchhead = _head[hashaddr];
	//pos���ܻᳬ��32k��&MASKĿ�ģ���Խ��
	_prev[pos&HASH_MASK] = _head[hashaddr];
	_head[hashaddr] = pos;

}
USH LZhashtable::getnext(USH matchhead) {
	return _prev[matchhead&HASH_MASK];
}