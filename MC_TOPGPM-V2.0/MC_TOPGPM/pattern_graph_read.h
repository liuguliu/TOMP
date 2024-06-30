#pragma once

#include<vector>
//#include"mysql_operation.h"
using namespace std;

//������Ҫ�ĺ�
#define MAX 10

//�����ȡ���ݵĽṹ��
struct PArcNode {
	bool visited;
	int fromNode, toNode;	// �õ��Ƕ�����ģʽͼ�е������±�
	int boundlength;
	float ptrust;	// social trust(T)
	float pintimacy; // social intimacy degree(R)
	float pfactor; // role impact factor(Rou)
	struct PArcNode *next;
	PArcNode() {
		visited = false;
		fromNode = -1;
		toNode = -1;
		boundlength = -1;
		ptrust = 0.0;
		pintimacy = 0.0;
		pfactor = 0.0;
		next = NULL;
	}
};
struct PNode {
	int PNodeLable;	// ��ʵ��ʹ�õĽڵ�Լ��
	float factor;
	// string Classfication; ����ڵ�Լ��
	PArcNode *firstArc;	//ָ���Ըö���Ϊ��β�ĵ�һ����
	PNode() {
		PNodeLable = -1;
		factor = 0.0;
		firstArc = NULL;
	}
};

struct Pattern {
	PNode pnodeList[MAX];
	int pvexnum;
	int parcnum;
};

////����ȫ�ֱ���
extern Pattern P;

//��������
bool pattern_graph_read(char *filename);