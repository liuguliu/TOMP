#pragma once

#include <fstream>
//#define MAXNUMV 1500
//#define MAXNUMV 80000
//#define MAXNUMV 90000
//#define MAXNUMV 1650000  //最大节点数 165万
#define MAXNUMV 5000000


//定义存取数据的结构体
struct ArcNode {
	int adjvex;	//该弧所指向的顶点的编号
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
	int nodenum;	//节点编号（因为可以用数组下标来表示所以不用）	
	float factor; //role impact factor(Rou)
	int nodelable;	//节点标签
	ArcNode *firstarc;	//指向第一条依附于该顶点的弧的指针

	VNode() {
		factor = 0.0;
		nodelable = -1;
		firstarc = NULL;
	}
};
struct Graph {
	VNode NodeList[MAXNUMV];
	int vexnum, arcnum;
};

//定义全局变量
extern Graph G;

//声明函数
void read_Node_Info(char* query);	//读取数据库中数据
void read_Arc_Info(char* query);	//读取边上的信息



