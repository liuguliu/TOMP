//#pragma once
//#include<vector>
//#include"mysql_operation.h"
//#include"pattern_graph_read.h"
//
////定义需要的宏
//#define MAX 10
//
//struct ArcPath {	//匹配边
//	int startnode, endnode;//用于模式图的连接
//	int length;
//	float answertrust;
//	float answerintimacy;
//	float answerfactor;
//	float obscure;
//	vector<ArcNode> path;
//	ArcPath() {
//		startnode = endnode = -1;
//		length = -1;
//		answertrust = 0.0;
//		answerintimacy = 0.0;
//		answerfactor = 0.0;
//		obscure = 0.0;
//	}
//};
//struct ArcPathList {	//匹配边的pool
//	int fromnode, tonode;	//结点编号
//	vector<ArcPath> pathset;
//	ArcPathList() {
//		fromnode = tonode = -1;
//	}
//};
//
////声明外部全局变量
//extern vector<PArcNode> breadth_P;
//
//void breadth_first_Traverse();
//void edge_match(PArcNode *p, int num);