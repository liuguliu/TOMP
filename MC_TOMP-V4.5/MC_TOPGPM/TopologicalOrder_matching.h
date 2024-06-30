#pragma once

#include<vector>
#include<list>
#include<map>
#include<string.h>
#include"txt_operation.h"
#include"pattern_graph_read.h"


struct SUITABLE_PATH
{
	int startnode, endnode;
	float t;
	float r;
	float rou;
	int Length;
	double path_fuzzy;
	list<int> path_list; //��������¼һ��·�������ĵ�
};

struct ArcPath {	// �����ݽṹ��������suitable path list
	SUITABLE_PATH suitable_path;
	vector<ArcNode> path;
	ArcPath* nextSaEndP=NULL;
};

struct ArcPathN { 	// ģʽ�ߵķ�װ
	//int tonode; // ģʽ�ߵ�(tonode)
	map<int,SUITABLE_PATH> suitable;  //endnode, match edge
};


struct INEDGE {
	int tonode; // ģʽ�ߵ�(tonode)
	map<int, int> in; //startnode, endnode
};


struct NODEMATCH {  //��ƥ�䶥��C1��node=C1, descendent_pathlist=> ��C-B�� => ��C-D��;
	int matchnode;
	//int count; //���ڻ����˵count�������1
	//map<int,INEDGE> in_edge;  //���ڷ���ɾ�������������ı�ƥ��<fromnode,inedge>
	map<int, ArcPathN> out_edge; //<tonode,arcpathn>
	NODEMATCH(){
		matchnode = -1;
		//count = 0;
	}
};

struct DELETED {  // ���ڴ洢������ƥ�������Ĵ�ɾ���ڵ����
	int node;
	int matchnode;
};

typedef vector<SUITABLE_PATH> SUITABLE_PATH_LIST;
typedef map<int, SUITABLE_PATH_LIST> MATCHNODECACHE;


struct PATTERN_ANSWER {
	map<int, NODEMATCH> patternnode[MAX]; //�������±��ʾģʽͼ�ж�Ӧid�Ľڵ�
};

typedef list<int> LISTINT;
typedef pair<int, bool> combi;
typedef vector<int> VEC;

//�����ⲿȫ�ֱ���
extern vector<int> TopoOPN;
extern PATTERN_ANSWER pattern_answer[MAX];
extern int graph_sum;
extern double this_start_time, this_finish_time;
extern char folderPath[256];
extern map<int, LISTINT> nodeCandidate;

void init();
void TopologicalNodeTraverse(int nodeNo);
list<int> GetNodeCandidate(int nodeID);
bool comb2(int matEdgNo,int num, vector<int> b, vector<pair<int,bool>> &curr_nod_list,int candCount, int m, int k);

int edge_match(int starti, int num, PArcNode *p, vector<int> *connected_node_list, int existFlag);
int inverse_edge_match(int endi, int num, PArcNode *p, vector<int> *connected_node_list, int existFlag);

//void edge_match(int starti, PArcNode *p, vector<ArcPath> *edgelengthmatch);
//void inverse_edge_match(int endi, PArcNode *p, vector<ArcPath> *edgelengthmatch);
//int Create_suitable_path(int startnode, int num, PArcNode *p, vector<ArcPath> *edgelengthmatch, vector<int> *connected_node_list, int existFlag);

void AddSuitablePathToAnswer(PArcNode *p, SUITABLE_PATH suitable_path, int num);
int AddSuitableCacheNodeToAnswer(PArcNode *p, vector<int> *connected_node_list, int num, int sourcenode, int type);

bool TopologicalMatching3(int FNode, int num);
bool TopologicalMatching4(int num, int matEdgNo);
bool combineTesing(VEC combine, vector<pair<int, bool>> &curr_nod_list, int *matEdgNo,int candCount, int num);

void Answer_Store3(char* filename, int num);
void GetMemoryAndTime(char* filename, time_t starttime, time_t finishtime, int answernum, char *info);
void GetMemoryAndTime2(char* filename, char* algorithm, char* pattern_graph, char* datasetname, time_t starttime, time_t finishtime, int answernum);
