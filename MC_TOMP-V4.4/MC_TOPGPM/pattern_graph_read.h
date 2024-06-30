#pragma once

#include<vector>
//#include"mysql_operation.h"
using namespace std;

//������Ҫ�ĺ�
#define MAX 20

//�����ȡ���ݵĽṹ��
struct PArcNode {
	bool visited;
	int fromNode, toNode;	// �õ��Ƕ�����ģʽͼ�е������±�
	int boundlength;
	int edgeNo; // �ߵı�ţ����ڻ�����ƥ���
	float ptrust;	// social trust(T)
	float pintimacy; // social intimacy degree(R)
	float pfactor; // role impact factor(Rou)
	bool flag; //��ʶ��ƥ�����ƥ�������������ƥ�仹�Ƿ���ƥ��
	struct PArcNode *nextout;
	struct PArcNode *nextin;
	PArcNode() {
		visited = false;
		fromNode = -1;
		toNode = -1;
		edgeNo = -1;
		boundlength = -1;
		ptrust = 0.0;
		pintimacy = 0.0;
		pfactor = 0.0;
		nextout = NULL;
		nextin = NULL;
	}
};
struct PNode {
	int PNodeLable;	// ��ʵ��ʹ�õĽڵ�Լ��
	float factor;
	int indegree, outdegree;
	int firstinfrom;
	int needc;
	// string Classfication; ����ڵ�Լ��
	PArcNode *fstoutArc;	//ָ���Ըö���Ϊ��β�ĵ�һ����
	PArcNode *fstinArc;	//ָ���Ըö���Ϊ��β�ĵ�һ����
	PNode() {
		PNodeLable = -1;
		indegree = outdegree = 0;
		factor = 0.0;
		fstoutArc = NULL;
		fstinArc = NULL;
		needc = -1;
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