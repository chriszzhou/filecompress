#pragma once 
#include<queue>
#include<vector>
template <class W>
struct HuffManTreeNode {
	HuffManTreeNode(const W& weight = W())
		: _pleft(nullptr)
		, _pright(nullptr)
		,_pparent(nullptr)
		, _weight(weight)
	{}
	HuffManTreeNode* _pleft;
	HuffManTreeNode* _pright;
	HuffManTreeNode* _pparent;
	W _weight;
};
template<class W>
class Less{
	typedef HuffManTreeNode<W> Node;
public:
	bool operator()( Node* pleft,const Node* pright) {
		return pleft->_weight > pright->_weight;
	}
};
template <class W>
class HUffManTree {
	typedef HuffManTreeNode<W> Node;
public:
	HUffManTree()
		:_proot()
	{}
	HUffManTree(const std::vector<W>& vweight,const W& invalid) {
		CreatHUffManTree(vweight, invalid);
	}
	~HUffManTree() {
		destroytree(_proot);
	}
	Node* getroot() {
		return _proot;
	}
	void CreatHUffManTree(const std::vector<W>& vweight, const W& invalid) {
		//1.构建森林
		std::priority_queue<Node*,std::vector<Node*>,Less<W>> q;
		for (auto e : vweight) {
			Node* a1 = new Node(e);
			if (e == invalid)
				continue;
			q.push(a1);
		}
		//.判断是否大于=2
		while (q.size() >= 2) {
			Node* pLeft = q.top();
			q.pop();
			Node* pRight = q.top();
			q.pop();
			Node* pParent = new Node(pLeft->_weight + pRight->_weight);
			pParent->_pleft = pLeft;
			pParent->_pright = pRight;
			pLeft->_pparent = pParent;
			pRight->_pparent = pParent;
			q.push(pParent);
		}
		_proot = q.top();
	}
private:
	void destroytree(Node * proot) {
		if (proot) {
			destroytree(proot->_pleft);
			destroytree(proot->_pright);
			delete proot;
			proot = nullptr;
		}
	}
private:
	Node* _proot;
};

