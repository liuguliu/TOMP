// MC_TOPGPM.cpp : �������̨Ӧ�ó������ڵ㡣
#include<iostream>
#include<time.h>
#include"txt_operation.h"
#include"pattern_graph_read.h"
#include"TopologicalOrder_matching.h"
#include"MC_TOPGPM.h"
#include"dat_operation.h"
using namespace std;

void ETOF_KGPM(char** array)
{
	cout << array[0] << " " << array[1] <<" "<<array[2]<<" "<<array[3]<< endl;
	char storefile[256];
	sprintf_s(storefile, 256, "E:/experimental/experiment3/compare2/MC_TOPGPM-%s/answer/%s/%s_%s_patternanswer_%s.txt", array[0], array[2], array[2], array[1], array[3]);
	char infofile[256];
	sprintf_s(infofile, 256, "E:/experimental/experiment3/compare2/MC_TOPGPM-%s/answer/%s//%s_%s_time_information_%s.txt", array[0], array[2], array[2], array[1], array[3]);
	char statisfile[256];
	sprintf_s(statisfile, 256, "E:/experimental/experiment3/compare2/time_statistic.txt");
	char nodefile[256];
	sprintf_s(nodefile, 256, "E:/experimental/experiment3/data/%s/%snode.txt", array[2], array[1]);
	char edgefile[256];
	sprintf_s(edgefile, 256, "E:/experimental/experiment3/data/%sedge.txt", array[1]);
	char patternfile[256];
	sprintf_s(patternfile, 256, "E:/experimental/experiment3/DPGForComp/pattern_graph_%s.txt", array[3]);
	
	// .dat�ļ�
	char newnodefile[256];
	sprintf_s(newnodefile, 256, "E:/experimental/experiment3/data/%s/%snode.dat", array[2], array[1]);
	char newedgefile[256];
	sprintf_s(newedgefile, 256, "E:/experimental/experiment3/data/%sedge.dat", array[1]);

	char inverseedgefile[256];
	sprintf_s(inverseedgefile, 256, "E:/experimental/experiment3/data/%sinverseedge.dat", array[1]);
	
	read_Node_Info_B(newnodefile);	//��ȡ���ݿ�������
	read_Arc_Info_B(newedgefile);	//��ȡ���ϵ���Ϣ
	pattern_graph_read(patternfile);

	this_start_time = (double)clock();
	TopologicalTraverse();
	//��Դ�ڵ㿪ʼƥ��ģʽͼ
	for (int i = 0; i < G.vexnum; i++) {
 		if ((G.NodeList[i].nodelable == P.pnodeList[TopoOrderPatternEdge[0].fromNode].PNodeLable) 
			&& (G.NodeList[i].factor - P.pnodeList[TopoOrderPatternEdge[0].fromNode].factor> -0.000001)) {//�ҵ���Դ�ڵ�ڵ��ǩ��ͬ�Ľڵ㣬��Ϊƥ����ʼ�ڵ㣬��0���ߡ�
			int num = 1;	//ʵ����num�Ǵ�0��ʼ�ģ�ֻ�ǵݹ��ʱ���Ѿ��ǵڶ������ˡ�
			vector<ArcPath> edgelengthmatch;
			edge_match(i,&TopoOrderPatternEdge[0], &edgelengthmatch);
			Create_suitable_path(&TopoOrderPatternEdge[0], 0, &edgelengthmatch);
			//edgelengthmatch.clear(); // 1 �������
			//vector<ArcPath>().swap(edgelengthmatch); // 2 ������������ͷ��ڴ�
			//edgelengthmatch.clear(); // 3 �������
			//edgelengthmatch.shrink_to_fit(); // 3 �ͷ��ڴ�
			//cout << "size:" << edgelengthmatch.size() << endl;
			//cout << "capacity:" << edgelengthmatch.capacity() << endl;

			ReleaseSpace(&edgelengthmatch);
			vector<ArcPath>().swap(edgelengthmatch); // ������������ͷ��ڴ�
			//cout << "after swap size:" << edgelengthmatch.size() << endl;
			//cout << "after swap capacity:" << edgelengthmatch.capacity() << endl;
			if (arcpathlist[0].pathset.size()) {	// �õ���һ���ߵ�ƥ��
				arcpathlist[0].curindex = 0;
				arcpathlist[0].usefulcur = arcpathlist[0].pathset.size();
				while (arcpathlist[0].curindex < arcpathlist[0].usefulcur) {
					pattern_answer_temp.answer_edgelist.push_back(arcpathlist[0].pathset[arcpathlist[0].curindex]);
					pattern_answer_temp.vnode[arcpathlist[0].fromnode] = arcpathlist[0].pathset[arcpathlist[0].curindex].startnode;
					pattern_answer_temp.vnode[arcpathlist[0].tonode] = arcpathlist[0].pathset[arcpathlist[0].curindex].endnode;
					if (num < P.parcnum) { //���ﲻ��P.parcnum-1����Ϊnum=1;��P.parcnum=2��������ݹ麯�����еڶ����ߵ�ƥ��
						int startid = pattern_answer_temp.vnode[TopoOrderPatternEdge[num].fromNode];	//num==1����1���ߡ�Ĭ�ϵ�0����ƥ�����ɵõ���1���ߵ���ʼ�ڵ㡣
						if (startid != -1) {
							bool rflag = TopoligicalMatching2(startid, num, storefile, infofile);  //�ݹ�ƥ����һ����
							if (rflag) {
								break;
							}
						}
						else { // �����һ��Ҫƥ��ıߵ���ʼ������ƥ�䶥��ʱ�����²��Ҹýڵ�����к�ѡ�ڵ���ѭ��������һ���ߵ�ƥ�䣨�������ڽӱ��Ż���
							list<int> nodeCandidate = GetNodeCandidate(TopoOrderPatternEdge[num].fromNode);
							list<int>::iterator p;
							for (p = nodeCandidate.begin(); p != nodeCandidate.end(); p++) {
								pattern_answer_temp.vnode[TopoOrderPatternEdge[num].fromNode] = *p;
								bool rflag = TopoligicalMatching2(*p, num, storefile, infofile);  //�ݹ�ƥ����һ����
								if (rflag) {
									break;
								}
							}
						}
					}
					else { // ģʽͼֻ��һ����
						pattern_answer.push_back(pattern_answer_temp);
						graph_sum++;
					}
					arcpathlist[0].curindex++;
					pattern_answer_temp.answer_edgelist.pop_back();	
					if (pattern_answer.size()) {
						this_finish_time = (double)clock();
						Answer_Store2(storefile, graph_sum);
						//��¼�ڴ�ʹ�����
						GetMemoryAndTime(infofile, this_start_time, this_finish_time, graph_sum, "Returns the same first edge result, or the same first vertex result ");
						pattern_answer.clear();
					}
					//else {
					//	this_finish_time = (double)clock();
					//	GetMemoryAndTime(infofile, this_start_time, this_finish_time, graph_sum, "Returns the same first edge result, or the same first vertex result ");
					//}
				}
			}
			for (int j = 0; j < P.parcnum; j++) {	// ������л���ı�ƥ����
				arcpathlist[j].curindex = 0;
				arcpathlist[j].pathset.clear();
			}
			pattern_answer_temp.answer_edgelist.clear(); // ������л����ģʽͼƥ����
			memset(pattern_answer_temp.vnode, -1, MAX * sizeof(int));
		}
		if (pattern_answer.size()) {
			this_finish_time = (double)clock();
			Answer_Store2(storefile, graph_sum);
			//��¼�ڴ�ʹ�����
			GetMemoryAndTime(infofile,this_start_time, this_finish_time, graph_sum, "Returns the same first vertex result ");
			pattern_answer.clear();
		}
		//else {
		//	this_finish_time = (double)clock();
		//	GetMemoryAndTime(infofile, this_start_time, this_finish_time, graph_sum, "Returns the same first vertex result ");
		//}
	}	
	Answer_Store2(storefile,graph_sum);
	this_finish_time = (double)clock();
	GetMemoryAndTime(infofile,this_start_time, this_finish_time, graph_sum, "End of program! ");
	pattern_answer.clear();
	GetMemoryAndTime2(statisfile, array[0], array[3], array[1], this_start_time, this_finish_time, graph_sum);
}

int main() {
	//V2.0
	// ����֮ǰ�ǵø� MAX ����

	//char* array[] = { "V2.0","email-Eu-core","label_4", "new_5" };
	//char* array[] = { "","soc-Epinions1","", "new_1" };
	//char* array[] = { "","soc-Slashdot","", "new_1" };
	//char* array[] = { "","soc-pokec","", "new_1" };
	//char* array[] = { "","soc-LiveJournal1","", "new_1" };
	char* array[] = { "V2.0","","", "ex4_1" };
	char* datasetname[] = { "soc-Epinions1","soc-Slashdot","soc-Pokec","soc-LiveJournal1" };
	char* label_scrope[] = { "label_20","label_40","label_60","label_80","label_100" };
	int i =3, j = 0;
	array[1] = datasetname[i];
	array[2] = label_scrope[j];
	ETOF_KGPM(array);
	cout << "Finish!" << endl;
	system("pause");
	return 0;
}

