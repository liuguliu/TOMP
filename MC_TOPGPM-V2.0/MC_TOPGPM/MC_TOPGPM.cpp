// MC_TOPGPM.cpp : 定义控制台应用程序的入口点。
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
	
	// .dat文件
	char newnodefile[256];
	sprintf_s(newnodefile, 256, "E:/experimental/experiment3/data/%s/%snode.dat", array[2], array[1]);
	char newedgefile[256];
	sprintf_s(newedgefile, 256, "E:/experimental/experiment3/data/%sedge.dat", array[1]);

	char inverseedgefile[256];
	sprintf_s(inverseedgefile, 256, "E:/experimental/experiment3/data/%sinverseedge.dat", array[1]);
	
	read_Node_Info_B(newnodefile);	//读取数据库中数据
	read_Arc_Info_B(newedgefile);	//读取边上的信息
	pattern_graph_read(patternfile);

	this_start_time = (double)clock();
	TopologicalTraverse();
	//从源节点开始匹配模式图
	for (int i = 0; i < G.vexnum; i++) {
 		if ((G.NodeList[i].nodelable == P.pnodeList[TopoOrderPatternEdge[0].fromNode].PNodeLable) 
			&& (G.NodeList[i].factor - P.pnodeList[TopoOrderPatternEdge[0].fromNode].factor> -0.000001)) {//找到与源节点节点标签相同的节点，作为匹配起始节点，第0条边。
			int num = 1;	//实际上num是从0开始的，只是递归的时候已经是第二条边了。
			vector<ArcPath> edgelengthmatch;
			edge_match(i,&TopoOrderPatternEdge[0], &edgelengthmatch);
			Create_suitable_path(&TopoOrderPatternEdge[0], 0, &edgelengthmatch);
			//edgelengthmatch.clear(); // 1 清空向量
			//vector<ArcPath>().swap(edgelengthmatch); // 2 清空向量，并释放内存
			//edgelengthmatch.clear(); // 3 清空向量
			//edgelengthmatch.shrink_to_fit(); // 3 释放内存
			//cout << "size:" << edgelengthmatch.size() << endl;
			//cout << "capacity:" << edgelengthmatch.capacity() << endl;

			ReleaseSpace(&edgelengthmatch);
			vector<ArcPath>().swap(edgelengthmatch); // 清空向量，并释放内存
			//cout << "after swap size:" << edgelengthmatch.size() << endl;
			//cout << "after swap capacity:" << edgelengthmatch.capacity() << endl;
			if (arcpathlist[0].pathset.size()) {	// 得到第一条边的匹配
				arcpathlist[0].curindex = 0;
				arcpathlist[0].usefulcur = arcpathlist[0].pathset.size();
				while (arcpathlist[0].curindex < arcpathlist[0].usefulcur) {
					pattern_answer_temp.answer_edgelist.push_back(arcpathlist[0].pathset[arcpathlist[0].curindex]);
					pattern_answer_temp.vnode[arcpathlist[0].fromnode] = arcpathlist[0].pathset[arcpathlist[0].curindex].startnode;
					pattern_answer_temp.vnode[arcpathlist[0].tonode] = arcpathlist[0].pathset[arcpathlist[0].curindex].endnode;
					if (num < P.parcnum) { //这里不用P.parcnum-1是因为num=1;若P.parcnum=2，则还需进递归函数进行第二条边的匹配
						int startid = pattern_answer_temp.vnode[TopoOrderPatternEdge[num].fromNode];	//num==1，第1条边。默认第0条边匹配完后可得到第1条边的起始节点。
						if (startid != -1) {
							bool rflag = TopoligicalMatching2(startid, num, storefile, infofile);  //递归匹配下一条边
							if (rflag) {
								break;
							}
						}
						else { // 如果下一条要匹配的边的起始顶点无匹配顶点时，重新查找该节点的所有候选节点再循环进行下一条边的匹配（可用逆邻接表优化）
							list<int> nodeCandidate = GetNodeCandidate(TopoOrderPatternEdge[num].fromNode);
							list<int>::iterator p;
							for (p = nodeCandidate.begin(); p != nodeCandidate.end(); p++) {
								pattern_answer_temp.vnode[TopoOrderPatternEdge[num].fromNode] = *p;
								bool rflag = TopoligicalMatching2(*p, num, storefile, infofile);  //递归匹配下一条边
								if (rflag) {
									break;
								}
							}
						}
					}
					else { // 模式图只有一条边
						pattern_answer.push_back(pattern_answer_temp);
						graph_sum++;
					}
					arcpathlist[0].curindex++;
					pattern_answer_temp.answer_edgelist.pop_back();	
					if (pattern_answer.size()) {
						this_finish_time = (double)clock();
						Answer_Store2(storefile, graph_sum);
						//记录内存使用情况
						GetMemoryAndTime(infofile, this_start_time, this_finish_time, graph_sum, "Returns the same first edge result, or the same first vertex result ");
						pattern_answer.clear();
					}
					//else {
					//	this_finish_time = (double)clock();
					//	GetMemoryAndTime(infofile, this_start_time, this_finish_time, graph_sum, "Returns the same first edge result, or the same first vertex result ");
					//}
				}
			}
			for (int j = 0; j < P.parcnum; j++) {	// 清空所有缓存的边匹配结果
				arcpathlist[j].curindex = 0;
				arcpathlist[j].pathset.clear();
			}
			pattern_answer_temp.answer_edgelist.clear(); // 清空所有缓存的模式图匹配结果
			memset(pattern_answer_temp.vnode, -1, MAX * sizeof(int));
		}
		if (pattern_answer.size()) {
			this_finish_time = (double)clock();
			Answer_Store2(storefile, graph_sum);
			//记录内存使用情况
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
	// 运行之前记得改 MAX 参数

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

