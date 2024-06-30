// MC_TOPGPM.cpp : 定义控制台应用程序的入口点。
#include<iostream>
#include<time.h>
#include<windows.h>
#include"txt_operation.h"
#include"pattern_graph_read.h"
#include"TopologicalOrder_matching.h"
#include"MC_TOPGPM.h"
#include"dat_operation.h"
using namespace std;


void ETOF_KGPM(char** array);
//void edge_match_check(char** array);

int main() {
	//V4.4
	// 运行之前记得改 MAX 参数

	//char* array[] = { "V4.4","email-Eu-core","label_4", "new04_1" };
	char* array[] = { "V4.4","","", "ex4_3" };
	char* datasetname[] = { "soc-Epinions1","soc-Slashdot","soc-Pokec","soc-LiveJournal1","synthetic_data_1" };
	char* label_scrope[] = { "label_4", "label_20","label_40","label_60","label_80","label_100" };
	int i = 1, j = 1;
	array[1] = datasetname[i];
	array[2] = label_scrope[j];
	ETOF_KGPM(array);
	cout << "Finish!" << endl;
	system("pause");
	return 0;
}


void ETOF_KGPM(char** array)
{
	cout << array[0] << " " << array[1] << " " << array[2] << " " << array[3] << endl;
	char storefile[256];
	sprintf_s(storefile, 256, "E:/experimental/experiment3/compare2/MC_TOMP-%s/answer/%s/%s_%s_patternanswer_%s.txt", array[0], array[2], array[2], array[1], array[3]);
	char infofile[256];
	sprintf_s(infofile, 256, "E:/experimental/experiment3/compare2/MC_TOMP-%s/answer/%s//%s_%s_time_information_%s.txt", array[0], array[2], array[2], array[1], array[3]);
	char statisfile[256];
	sprintf_s(statisfile, 256, "E:/experimental/experiment3/compare2/time_statistic.txt");
	char nodefile[256];
	sprintf_s(nodefile, 256, "E:/experimental/experiment3/data/%s/%snode.txt", array[2], array[1]);
	char edgefile[256];
	sprintf_s(edgefile, 256, "E:/experimental/experiment3/data/%sedge.txt", array[1]);
	char patternfile[256];
	sprintf_s(patternfile, 256, "E:/experimental/experiment3/DPGForComp/pattern_graph_%s.txt", array[3]);
	//cout << patternfile << endl;
	// .dat文件
	char newnodefile[256];
	sprintf_s(newnodefile, 256, "E:/experimental/experiment3/data/%s/%snode.dat", array[2], array[1]);
	char newedgefile[256];
	sprintf_s(newedgefile, 256, "E:/experimental/experiment3/data/%sedge.dat", array[1]);

	char inverseedgefile[256];
	sprintf_s(inverseedgefile, 256, "E:/experimental/experiment3/data/%sinverseedge.dat", array[1]);

	time_t start, end;
	time(&start);
	pattern_graph_read(patternfile);
	read_Node_Info_B(newnodefile);	// 读取数据库中数据
	read_Arc_Info_B(newedgefile);	// 读取边上的信息
	read_Arc_Info_BBR(inverseedgefile);	// 读取逆领接表边上的信息
	time(&end);
	int usingtime = int(difftime(end, start));
	cout << "耗时：" << usingtime << " second" << endl;

	// 开始记录匹配时间
	this_start_time = (double)clock();

	init();
	list<int>::iterator p;

	for (p = nodeCandidate[1].begin(); p != nodeCandidate[1].end(); p++) {
		bool sflag = TopologicalMatching3(*p,1);
		// 对pattern_answer进行判断，并存储当前匹配结果，记录匹配时间
		if (sflag) {
			this_finish_time = (double)clock();
			graph_sum++;
			Answer_Store3(storefile,P.pvexnum);
			// 记录内存使用情况
			GetMemoryAndTime(infofile, this_start_time, this_finish_time, graph_sum, "Returns 1 results ");
			for (int x = 1; x <= P.pvexnum; x++) {
				for (int y = 1; y <= P.pvexnum; y++) {
					pattern_answer[x].patternnode[y].clear();
				}
			}
		}
	}
	this_finish_time = (double)clock();
	GetMemoryAndTime(infofile, this_start_time, this_finish_time, graph_sum, "End of program! ");
	//char* array[] = { "V4.1","email-Eu-core","label_4", "new04_1" };
	GetMemoryAndTime2(statisfile, array[0], array[3], array[1], this_start_time, this_finish_time, graph_sum);
}