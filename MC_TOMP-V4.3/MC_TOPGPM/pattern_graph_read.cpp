//��ȡģʽͼ��ʱ�临�Ӷ�ΪO(E+V)

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
		//cout << "��ȡ������������" << endl;
		for (int i = 1; i <= P.pvexnum; i++) {//��ȡ������������
			int temp;//û���õ���Ϣ����ȡ������(�ڵ��ţ���������������±�)
			file >> temp;
			file >> P.pnodeList[i].factor;
			file >> P.pnodeList[i].PNodeLable;
			file >> P.pnodeList[i].needc;
		}
		//cout << "��ȡ����������" << endl;
		for (int i = 0; i < P.parcnum; i++) {//��ȡ����������
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
			if (P.pnodeList[fromNode].fstoutArc == NULL) {
				P.pnodeList[fromNode].fstoutArc = parc;
			}
			else {
				(*parc_previous).nextout = parc;
			}
			if (P.pnodeList[toNode].fstinArc == NULL) {
				P.pnodeList[toNode].fstinArc = parc;
			}
			else {
				PArcNode *parcin = P.pnodeList[toNode].fstinArc;
				while (parcin->nextin != NULL) {
					parcin = parcin->nextin;
				}
				parcin->nextin = parc;
			}
			parc_previous = parc;
			//P.pnodeList[fromNode].factor = (*parc).pfactor;
			//P.pnodeList[toNode].factor = (*parc).pfactor;
		}
	}
	cout << "The pattern graph was read successfully!" << endl;
	return true;
}