//#pragma once
//#include<vector>
//#include"mysql_operation.h"
//#include"pattern_graph_read.h"
//
////������Ҫ�ĺ�
//#define MAX 10
//
//struct ArcPath {	//ƥ���
//	int startnode, endnode;//����ģʽͼ������
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
//struct ArcPathList {	//ƥ��ߵ�pool
//	int fromnode, tonode;	//�����
//	vector<ArcPath> pathset;
//	ArcPathList() {
//		fromnode = tonode = -1;
//	}
//};
//
////�����ⲿȫ�ֱ���
//extern vector<PArcNode> breadth_P;
//
//void breadth_first_Traverse();
//void edge_match(PArcNode *p, int num);