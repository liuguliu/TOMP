#pragma once

#include<vector>
#include<list>
#include<string.h>
#include"txt_operation.h"
#include"pattern_graph_read.h"

//������Ҫ�ĺ�
#define MAX 20
#define	MEMBERSHIP 0.9

struct SUITABLE_PATH
{
	int startnode, endnode;
	float t;
	float r;
	float rou;
	int Length;
	double path_fuzzy; // ?
	int connectflag; // ?
	list<int> path_list; //��������¼һ��·�������ĵ�
	SUITABLE_PATH() {
		connectflag = -1;
	}
};

struct ArcPath {	//ƥ���
	SUITABLE_PATH suitable_path;
	vector<ArcNode> path;
	ArcPath* nextSaEndP=NULL;
};

struct ArcPathList {	// ƥ��ߵ�pool
	int fromnode, tonode;	// �����
	int curindex;
	int usefulcur;
	vector<SUITABLE_PATH> pathset;
	ArcPathList() {
		curindex = 0;
		fromnode = tonode = -1;
	}
};



struct PATTERN_ANSWER {
	int vnode[MAX];
	vector<SUITABLE_PATH> answer_edgelist;
	double pattern_fuzzy;
	PATTERN_ANSWER() {
		memset(vnode, -1, MAX*sizeof(int));
	}
};
struct ExperimentResult
{
	int graphnumber;
	double costtime;
};
extern list<ExperimentResult> resultList;

struct ExperimentDetailedResult
{
	int index;
	int length;
};
extern list<ExperimentDetailedResult> detailedResultList;

//�����ⲿȫ�ֱ���
extern ArcPathList arcpathlist[MAX];	//�洢��ƥ��Ľ��
extern vector<PArcNode> TopoOrderPatternEdge;
extern PATTERN_ANSWER pattern_answer_temp;
extern vector<PATTERN_ANSWER> pattern_answer;
extern int graph_sum;
extern double this_start_time, this_finish_time;

void TopologicalTraverse();
void edge_match(int starti, PArcNode *p, vector<ArcPath> *edgelengthmatch);
void Create_suitable_path(PArcNode *p, int num, vector<ArcPath> *edgelengthmatch);
bool TopoligicalMatching2(int startnode, int num, char* storefile, char* infofile);
list<int> GetNodeCandidate(int nodeID);
void Answer_Store(char* filename, int graph_sum);
void Answer_Store2(char* filename, int graph_sum);
void GetMemoryAndTime(char* filename, time_t starttime, time_t finishtime, int answernum, char *info);
void GetMemoryAndTime2(char* filename, char* algorithm, char* pattern_graph, char* datasetname, time_t starttime, time_t finishtime, int answernum);
void ReleaseSpace(vector<ArcPath> *edgelengthmatch);