#pragma once

#include <fstream>
//#define MAXNUMV 1500
//#define MAXNUMV 80000
//#define MAXNUMV 90000
//#define MAXNUMV 1650000  //���ڵ��� 165��
#define MAXNUMV 5000000


//�����ȡ���ݵĽṹ��
struct ArcNode {
	int adjvex;	//�û���ָ��Ķ���ı��
	struct ArcNode *nextarc;
	float trust;	//social trust(T)
	float intimacy; //social intimacy degree(R)

	ArcNode() {
		adjvex = -1;
		nextarc = NULL;
		trust = 0.0;
		intimacy = 0.0;

	}
};
struct VNode {
	int nodenum;	//�ڵ��ţ���Ϊ�����������±�����ʾ���Բ��ã�	
	float factor; //role impact factor(Rou)
	int nodelable;	//�ڵ��ǩ
	ArcNode *firstoutarc;	//ָ���һ���ö���ָ���Ļ���ָ��
	ArcNode *firstinarc;	//ָ���һ��ָ��ö���Ļ���ָ��

	VNode() {
		factor = 0.0;
		nodelable = -1;
		firstoutarc = NULL;
		firstinarc = NULL;
	}
};
struct Graph {
	VNode NodeList[MAXNUMV];
	int vexnum, arcnum;
};

//����ȫ�ֱ���
extern Graph G;

//��������
void read_Node_Info(char* query);	//��ȡ���ݿ�������
void read_Arc_Info(char* query);	//��ȡ���ϵ���Ϣ
