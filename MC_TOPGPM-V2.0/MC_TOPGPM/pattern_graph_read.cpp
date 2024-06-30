//读取模式图的时间复杂度为O(E+V)

#include<iostream>
#include<fstream>
#include"pattern_graph_read.h"
using namespace std;
Pattern P;
bool pattern_graph_read(char *filename) {
	PArcNode *parc_previous = NULL;
	ifstream file(filename);
	if (!file.is_open())
	{
		cout << "Open file Error!";
		return false;
	}
	while (!file.eof())
	{
		file >> P.pvexnum >> P.parcnum;
		//cout << "读取顶点限制条件" << endl;
		for (int i = 1; i <= P.pvexnum; i++) {//读取顶点限制条件
			int temp;//没有用的信息，读取即丢弃(节点编号，在这里就是数组下标)
			file >> temp;
			file >> P.pnodeList[i].factor;
			file >> P.pnodeList[i].PNodeLable;
		}
		//cout << "读取边限制条件" << endl;
		for (int i = 0; i < P.parcnum; i++) {//读取边限制条件
			PArcNode *parc = new PArcNode;
			file >> (*parc).fromNode;
			file >> (*parc).toNode;
			file >> (*parc).boundlength;
			file >> (*parc).ptrust;
			file >> (*parc).pintimacy;
			file >> (*parc).pfactor;
			//cout << (*parc).boundlength << "  " << (*parc).ptrust << "  " << (*parc).pintimacy << endl;
			int fromNode = (*parc).fromNode;
			int toNode = (*parc).toNode;
			if (P.pnodeList[fromNode].firstArc == NULL) {
				P.pnodeList[fromNode].firstArc = parc;
			}
			else {
				(*parc_previous).next = parc;
			}
			parc_previous = parc;
			//P.pnodeList[fromNode].factor = (*parc).pfactor;
			//P.pnodeList[toNode].factor = (*parc).pfactor;
		}
	}
	cout << "The pattern graph was read successfully!" << endl;
	return true;
}