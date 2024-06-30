#pragma once

#include<vector>
//#include"mysql_operation.h"
using namespace std;

//定义需要的宏
#define MAX 20

//定义存取数据的结构体
struct PArcNode {
	bool visited;
	int fromNode, toNode;	// 用的是顶点在模式图中的数组下标
	int boundlength;
	int edgeNo; // 边的编号，用于缓存已匹配边
	float ptrust;	// social trust(T)
	float pintimacy; // social intimacy degree(R)
	float pfactor; // role impact factor(Rou)
	bool flag; //标识该匹配边在匹配策略中是正向匹配还是反向匹配
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
	int PNodeLable;	// 本实验使用的节点约束
	float factor;
	int indegree, outdegree;
	int firstinfrom;
	int needc;
	// string Classfication; 常规节点约束
	PArcNode *fstoutArc;	//指向以该顶点为弧尾的第一条边
	PArcNode *fstinArc;	//指向以该顶点为弧尾的第一条边
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

////声明全局变量
extern Pattern P;

//声明函数
bool pattern_graph_read(char *filename);