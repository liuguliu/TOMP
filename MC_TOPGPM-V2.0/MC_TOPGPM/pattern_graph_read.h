#pragma once

#include<vector>
//#include"mysql_operation.h"
using namespace std;

//定义需要的宏
#define MAX 10

//定义存取数据的结构体
struct PArcNode {
	bool visited;
	int fromNode, toNode;	// 用的是顶点在模式图中的数组下标
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
	int PNodeLable;	// 本实验使用的节点约束
	float factor;
	// string Classfication; 常规节点约束
	PArcNode *firstArc;	//指向以该顶点为弧尾的第一条边
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

////声明全局变量
extern Pattern P;

//声明函数
bool pattern_graph_read(char *filename);