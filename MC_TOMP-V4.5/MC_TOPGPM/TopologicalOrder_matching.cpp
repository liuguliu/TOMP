//#include <dhcpsapi.h>
#include<iostream>
#include<vector>
#include<queue>
#include<list>
#include<stack>
#include <time.h> 
#include<string.h>
#include<fstream>
// ��ȡ�ڴ�ʹ�����
#include <windows.h>
#include <Psapi.h>
// utf-8 �����ļ�
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#include <string.h>
#include"TopologicalOrder_matching.h"

using namespace std;

//#pragma comment(lib,"dhcpsapi.lib")
#pragma comment(lib, "psapi.lib")

#define	MEMBERSHIP 0.9
//#define EDGECACHEN 500

map<int, LISTINT> nodeCandidate;
//����ȫ�ֱ���
vector<int> BrDOT;
PATTERN_ANSWER pattern_answer[MAX];
float T_membership = 0.0, R_membership = 0.0, Rou_membership = 0.0;
int graph_sum = 0;
double this_start_time, this_finish_time; /* ÿ�����Ŀ�ʼʱ�䣬����ʱ�� */

// ����һ�����ڴ洢�м������ļ���
char folderPath[256];

//���屾����ʹ�õ�ȫ�ֱ���
vector<MATCHNODECACHE> matchCache;
//SUITABLE_PATH ss[EDGECACHEN];

void init() {
	TopologicalNodeTraverse(1);
	for (int i = 0; i <= P.parcnum; i++) {
		MATCHNODECACHE nodeCacheTemp;
		matchCache.push_back(nodeCacheTemp);
	}

	nodeCandidate[1] = GetNodeCandidate(1); // ��ȡԴ�ڵ�ĺ�ѡ�ڵ㼯��
	//for (int i = 1; i <= P.pvexnum; i++) {
	//	if (P.pnodeList[i].indegree == 0) {
	//		nodeCandidate[i] = GetNodeCandidate(i); // ��ȡԴ�ڵ�ĺ�ѡ�ڵ㼯��
	//	}
	//}
}


void TopologicalNodeTraverse(int nodeNo) {
	queue<int> Q;
	int indegreetemp[20];
	memset(indegreetemp, 0, 20 * sizeof(int));
	for (int i = 1; i <= P.pvexnum; i++) {	// ����ģʽͼ��ÿ���ڵ�ĳ��Ⱥ����
		PArcNode *parctemp = P.pnodeList[i].fstoutArc;
		while (parctemp != NULL) {
			P.pnodeList[i].outdegree++;
			P.pnodeList[parctemp->toNode].indegree++;
			indegreetemp[parctemp->toNode]++;
			parctemp = parctemp->nextout;
		}
	}

	// ����ʼ�ڵ㿪ʼ��������ģʽͼ����ΪģʽͼΪ��ͨͼ�����Դ�����ڵ㿪ʼ�������ͼ�ı���
	int TravEdgeNum = 0;
	int TravNodeCur = nodeNo;
	int TravNodeNum = 1;
	BrDOT.push_back(-1); // ռ��BrDOT[0]��ʹ�ô�ƥ��ڵ���±��1��ʼ
	BrDOT.push_back(TravNodeCur);
	while (TravEdgeNum < P.parcnum) {
		PArcNode *parcintmp = P.pnodeList[TravNodeCur].fstinArc;
		PArcNode *parcouttmp = P.pnodeList[TravNodeCur].fstoutArc;
		vector<int> invec;
		while (parcintmp != NULL) {
			if (parcintmp->visited == false) {
				parcintmp->edgeNo = TravEdgeNum;
				if (find(BrDOT.begin(), BrDOT.end(), parcintmp->fromNode) == BrDOT.end()) {
					invec.push_back(parcintmp->fromNode);
				}
				parcintmp->flag = true;
				parcintmp->visited = true;
				TravEdgeNum++;
			}
			parcintmp = parcintmp->nextin;

		}
		while (parcouttmp != NULL) {
			if (parcouttmp->visited == false) {
				parcouttmp->edgeNo = TravEdgeNum;
				if (find(BrDOT.begin(), BrDOT.end(), parcouttmp->toNode) == BrDOT.end()) {
					BrDOT.push_back(parcouttmp->toNode);
				}
				parcouttmp->visited = true;
				TravEdgeNum++;
			}
			parcouttmp = parcouttmp->nextout;
		}
		for (int i = 0; i < invec.size(); i++) {
			if (find(BrDOT.begin(), BrDOT.end(), invec[i]) == BrDOT.end()) {
				BrDOT.push_back(invec[i]);
			}
		}
		//BrDOT.insert(BrDOT.end(), invec.begin(), invec.end());
		TravNodeCur = BrDOT[TravNodeNum++];
	}
}

list<int> GetNodeCandidate(int nodeID) {
	list<int> nodeCandidate;
	PNode node = P.pnodeList[nodeID];
	for (int i = 0; i < G.vexnum; i++) {
		if (G.NodeList[i].nodelable == node.PNodeLable && (G.NodeList[i].factor - node.factor) > -0.000001) {
			nodeCandidate.push_back(i);
		}
	}
	return nodeCandidate;
}

/**
* ����һ�����ݵ�������ϣ�m��ʾ��Ԫ�ظ�����k��ʾһ�������Ԫ�صĸ���
*/
void comb(vector<VEC> &a, vector<int> b, vector<pair<int,bool>> &curr_nod_list, int m, int k, int counts) {
	for (int i = m; i >= k; i--) {
		if (curr_nod_list[i].second) {
			b.push_back(curr_nod_list[i].first);
			if (k>1) {
				comb(a, b, curr_nod_list, i - 1, k - 1, counts);
				b.pop_back();
			}
			else {
				counts++;
				a.push_back(b);
				b.pop_back();
			}
		}
	}
}


bool comb2(int matEdgNo, int num, vector<int> b, vector<pair<int, bool>> &curr_nod_list,int candCount, int m, int k) {
	int matEdgNotmp = matEdgNo;
	for (int i = m; i >= k; i--) {
		if (curr_nod_list[i].second) { //�жϸú�ѡ�ڵ��Ƿ��ѱ��ų�
			b.push_back(curr_nod_list[i].first);
			if (k>1) {
				bool returnFlag = comb2(matEdgNo,num,b, curr_nod_list, candCount, i - 1, k - 1);
				if (returnFlag) return true;
				b.pop_back();
			}
			else {
				//counts++;
				//a.push_back(b);
				// ����Ͻ���ƥ�����
				int flag1 = combineTesing(b, curr_nod_list, &matEdgNo, candCount, num);
				if (flag1) {
					// ����Ƿ��б���δ�õ��Ľڵ�ĺ�ѡ�ڵ㼯û�м��뵽��ǰ��
					for (int i = 1; i <= P.pvexnum; i++) {
						if (pattern_answer[num - 1].patternnode[i].size() > 0 && pattern_answer[num].patternnode[i].size() == 0) {
							pattern_answer[num].patternnode[i] = pattern_answer[num - 1].patternnode[i];
						}
					}
					bool rflag = TopologicalMatching4(num + 1, matEdgNo);
					if (rflag) return true; // ��ý���󣬲��ٱ���Ѱ������ƥ������
					else {
						for (int x = num; x <= P.pvexnum; x++) {
							pattern_answer[num].patternnode[BrDOT[x]].clear();
						}
						matEdgNo = matEdgNotmp;
					}
				}
				else {
					//���ڵ�ƥ��û�����⣬�����²�ڵ�ƥ��������⣬����������һ�����
					for (int x = num; x <= P.pvexnum; x++) {
						pattern_answer[num].patternnode[BrDOT[x]].clear();
					}
					matEdgNo = matEdgNotmp;
				}
				b.pop_back();
			}
		}
	}
	return false;
}


int edge_match(int starti, int num, PArcNode *p, vector<int> *connected_node_list, int existFlag) {//��������ͼ,������ͼ�б�ǩ��ģʽ�ߵ���ʼ�ڵ���ͬ�Ľڵ�����н��������

	int returnFlag = 0;
	map<int, SUITABLE_PATH> edgeCandidate;
	SUITABLE_PATH_LIST nodeSuitCache; // ��¼�뵱ǰƥ��ڵ���ص�ƥ���	
	queue<ArcPath> q;	//����һ�����и��������������	
	ArcPath pathi;
	pathi.suitable_path.startnode = pathi.suitable_path.endnode = starti;
	pathi.suitable_path.path_list.push_back(starti);
	pathi.suitable_path.rou = G.NodeList[starti].factor;
	pathi.suitable_path.r = 1.0;
	pathi.suitable_path.t = 1.0;
	q.push(pathi);
	while (!q.empty()) {
		bool flag = false;
		ArcPath* pathj = new ArcPath();
		(*pathj) = q.front();	//��ȡ��ͷԪ��
		q.pop();	//��ͷԪ�س���
		if ((G.NodeList[(*pathj).suitable_path.endnode].nodelable == P.pnodeList[(*p).toNode].PNodeLable) &&
			(G.NodeList[(*pathj).suitable_path.endnode].factor - P.pnodeList[(*p).toNode].factor>-0.000001)) {	//�ҵ�һ������bounded length��·��																												// ����ƥ��·�����ۺ����Ե�ֵ
			ArcPath store_path = *pathj;
			store_path.suitable_path.rou = (*pathj).suitable_path.rou / ((*pathj).path.size() + 1);
			if ((store_path.suitable_path.rou - (*p).pfactor) > -0.000001) {
				store_path.suitable_path.Length = store_path.suitable_path.path_list.size() - 1;
				// �ж�ƥ��·���Ƿ�����滻ԭ��ƥ��
				map<int, SUITABLE_PATH>::iterator iter = edgeCandidate.find(store_path.suitable_path.endnode);
				if (iter == edgeCandidate.end()) {
					edgeCandidate[store_path.suitable_path.endnode] = store_path.suitable_path;
				}
				else {
					double compF = (*iter).second.r + (*iter).second.rou + (*iter).second.t;
					if ((compF - store_path.suitable_path.r - store_path.suitable_path.t - store_path.suitable_path.rou) < 0.000001) {
						edgeCandidate[store_path.suitable_path.endnode] = store_path.suitable_path;
					}
				}
			}
			else {
				// ������·��rouֵԼ��������
			}
		}
		if ((*pathj).path.size() < (*p).boundlength) {
			ArcNode *arctemp;
			arctemp = G.NodeList[(*pathj).suitable_path.endnode].firstoutarc; // ��ȡ��ǰ����ĵ�һ���ڽӱ�
			while (arctemp != NULL) {
				ArcPath cur_path;
				cur_path.path = (*pathj).path;
				cur_path.path.push_back((*arctemp));
				cur_path.suitable_path.startnode = starti;
				cur_path.suitable_path.endnode = (*arctemp).adjvex;
				cur_path.suitable_path.path_list = (*pathj).suitable_path.path_list;
				cur_path.suitable_path.path_list.push_back((*arctemp).adjvex);
				// ������pathj�Ļ����Ͻ��������
				cur_path.suitable_path.rou = (*pathj).suitable_path.rou + G.NodeList[(*arctemp).adjvex].factor;
				cur_path.suitable_path.t = (*pathj).suitable_path.t * (*arctemp).trust;
				cur_path.suitable_path.r = (*pathj).suitable_path.r * (*arctemp).intimacy;

				// �жϸ�����ֵ�Ƿ������Լ��ƥ��Ҫ����Ϊ����ֵ����0-1֮�䣬���Ե����ڲ�����Լ��������ֵʱ�ɽ��м�֦��
				if ((cur_path.suitable_path.r - (*p).pintimacy) > -0.000001 && (cur_path.suitable_path.t - (*p).ptrust) > -0.000001) {
					if (cur_path.path.size() == (*p).boundlength) {
						if ((G.NodeList[cur_path.suitable_path.endnode].nodelable == P.pnodeList[(*p).toNode].PNodeLable)) {
							q.push(cur_path);
						}
						else {
							// �ﵽԼ��·�����ȣ����յ㲻����Ҫ��
						}
					}
					else {
						q.push(cur_path);
					}
				}
				arctemp = (*arctemp).nextarc;
			}
		}
		delete pathj;
	} // while
	  // ������ƥ�����������ѡƥ��߼�����
	for (map<int, SUITABLE_PATH>::iterator iter = edgeCandidate.begin(); iter != edgeCandidate.end(); iter++) { //ѭ�������Բ�ѯ���ı�ƥ���б�
																												//��ƥ��߼��뵽ƥ������
		if (existFlag == 1) { // Ϊ 1 ��num������
							  // �жϵ�ǰƥ����Ƿ������ӵ����е�ƥ�䶥����
			if (pattern_answer[num].patternnode[(*p).toNode].find((*iter).second.endnode) != pattern_answer[num].patternnode[(*p).toNode].end()) {
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.endnode);
			}
		}
		else if (existFlag == 2) { // Ϊ 2 ��num-1������
			map<int, NODEMATCH>::iterator toiter = pattern_answer[num - 1].patternnode[(*p).toNode].find((*iter).second.endnode);
			if (toiter != pattern_answer[num - 1].patternnode[(*p).toNode].end()) {
				// �ȴ�num-1���õ���Ӧ��ֵ���ٽ���ǰƥ��߼��뵽�м����У�����ƥ��߶��ڽ������Ҫ���Կ����Ȳ���ӣ�
				pattern_answer[num].patternnode[(*p).toNode][toiter->first] = toiter->second;
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.endnode);
			}
		}
		else if (existFlag == 4) { // Ϊ 4 ��num������
			if (pattern_answer[num].patternnode[(*p).fromNode].find((*iter).second.startnode) != pattern_answer[num].patternnode[(*p).fromNode].end()) {
				// �ȴ�num-1���õ���Ӧ��ֵ���ٽ���ǰƥ��߼��뵽�м����У�����ƥ��߶��ڽ������Ҫ���Կ����Ȳ���ӣ�
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.startnode);
			}
		}
		else if (existFlag == 5) { // Ϊ 5 ��num-1������
			map<int, NODEMATCH>::iterator fromiter = pattern_answer[num - 1].patternnode[(*p).fromNode].find((*iter).second.startnode);
			if (fromiter != pattern_answer[num - 1].patternnode[(*p).fromNode].end()) {
				pattern_answer[num].patternnode[(*p).fromNode][fromiter->first] = fromiter->second;
				// �ȴ�num-1���õ���Ӧ��ֵ���ٽ���ǰƥ��߼��뵽�м����У�����ƥ��߶��ڽ������Ҫ���Կ����Ȳ���ӣ�
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.startnode);
			}
		}
		else { // existFlag = 0 or 3
			AddSuitablePathToAnswer(p, (*iter).second, num);
			returnFlag++;
		}
		nodeSuitCache.push_back((*iter).second);
	}
	if (matchCache[(*p).edgeNo].size() == 0) {
		matchCache[(*p).edgeNo][starti] = nodeSuitCache;
	}
	else if (matchCache[(*p).edgeNo].find(starti) == matchCache[(*p).edgeNo].end()) { // ��ʵ��������жϣ���Ϊ�����˾Ϳ϶�������������cache��
		matchCache[(*p).edgeNo][starti] = nodeSuitCache;
	}
	else {
		// �Ѵ����ڻ����У������������
		cout << "�Ѵ����ڻ����У������������" << endl;
	}
	return returnFlag;
}

int inverse_edge_match(int endi, int num, PArcNode *p, vector<int> *connected_node_list, int existFlag) {

	int returnFlag = 0;
	map<int, SUITABLE_PATH> edgeCandidate;
	SUITABLE_PATH_LIST nodeSuitCache; // ��¼�뵱ǰƥ��ڵ���ص�ƥ���
	queue<ArcPath> q;	//����һ�����и��������������
						//int *answerexist = new int[G.vexnum + 1];
	ArcPath pathi;
	pathi.suitable_path.startnode = pathi.suitable_path.endnode = endi;
	pathi.suitable_path.path_list.push_back(endi);
	pathi.suitable_path.rou = G.NodeList[endi].factor;
	pathi.suitable_path.r = 1.0;
	pathi.suitable_path.t = 1.0;

	q.push(pathi);
	while (!q.empty()) {
		bool flag = false;
		ArcPath* pathj = new ArcPath();
		(*pathj) = q.front();	//��ȡ��ͷԪ��
		q.pop();	//��ͷԪ�س���
		if ((G.NodeList[(*pathj).suitable_path.startnode].nodelable == P.pnodeList[(*p).fromNode].PNodeLable)
			&& (G.NodeList[(*pathj).suitable_path.startnode].factor - P.pnodeList[(*p).fromNode].factor>-0.000001)) {	//�ҵ�һ������Լ����·��
																														// ����ƥ��·�����ۺ����Ե�ֵ
			ArcPath store_path = *pathj;
			store_path.suitable_path.rou = (*pathj).suitable_path.rou / ((*pathj).path.size() + 1);
			if ((store_path.suitable_path.rou - (*p).pfactor) > -0.000001) {
				//(*pathj).suitable_path.r = (*pathj).suitable_path.r / (*pathj).path.size();
				//(*pathj).suitable_path.t = (*pathj).suitable_path.t / (*pathj).path.size();
				store_path.suitable_path.Length = store_path.suitable_path.path_list.size() - 1;
				// �ж�ƥ��·���Ƿ�����滻ԭ��ƥ��
				map<int, SUITABLE_PATH>::iterator iter = edgeCandidate.find(store_path.suitable_path.startnode);
				if (iter == edgeCandidate.end()) {
					edgeCandidate[store_path.suitable_path.startnode] = store_path.suitable_path;
				}
				else {
					double compF = (*iter).second.r + (*iter).second.rou + (*iter).second.t;
					if ((compF - store_path.suitable_path.r - store_path.suitable_path.t - store_path.suitable_path.rou) < 0.000001) {
						edgeCandidate[store_path.suitable_path.startnode] = store_path.suitable_path;
					}
				}
			}
			else {
				// ������·��rouֵԼ��������
			}

		}
		if ((*pathj).path.size() < (*p).boundlength) {
			ArcNode *arctemp;
			arctemp = G.NodeList[(*pathj).suitable_path.startnode].firstinarc; // ��ȡ��ǰ����ĵ�һ���ڽӱ�
			while (arctemp != NULL) {
				ArcPath cur_path;
				cur_path.path = (*pathj).path;
				cur_path.path.push_back((*arctemp));
				cur_path.suitable_path.startnode = (*arctemp).adjvex;
				cur_path.suitable_path.endnode = endi;
				cur_path.suitable_path.path_list = (*pathj).suitable_path.path_list;
				cur_path.suitable_path.path_list.push_front((*arctemp).adjvex);
				// ������pathj�Ļ����Ͻ��������
				cur_path.suitable_path.rou = (*pathj).suitable_path.rou + G.NodeList[(*arctemp).adjvex].factor;
				cur_path.suitable_path.t = (*pathj).suitable_path.t * (*arctemp).trust;
				cur_path.suitable_path.r = (*pathj).suitable_path.r * (*arctemp).intimacy;

				// �жϸ�����ֵ�Ƿ������Լ��ƥ��Ҫ����Ϊ����ֵ����0-1֮�䣬���Ե����ڲ�����Լ��������ֵʱ�ɽ��м�֦��
				if ((cur_path.suitable_path.r - (*p).pintimacy) > -0.000001 && (cur_path.suitable_path.t - (*p).ptrust) > -0.000001) {
					if (cur_path.path.size() == (*p).boundlength) {
						if ((G.NodeList[cur_path.suitable_path.startnode].nodelable == P.pnodeList[(*p).fromNode].PNodeLable)) {
							q.push(cur_path);
						}
						else {
							// �ﵽԼ��·�����ȣ����յ㲻����Ҫ��
						}
					}
					else {
						q.push(cur_path);
					}
				}
				else {
					//��֦
				}
				arctemp = (*arctemp).nextarc;
			}
		}
		delete pathj;
	}//while
	 // ������ƥ�����������ѡƥ��߼�����
	for (map<int, SUITABLE_PATH>::iterator iter = edgeCandidate.begin(); iter != edgeCandidate.end(); iter++) { //ѭ�������Բ�ѯ���ı�ƥ���б�
																												//��ƥ��߼��뵽ƥ������
		if (existFlag == 1) { // Ϊ 1 ��num������
							  // �жϵ�ǰƥ����Ƿ������ӵ����е�ƥ�䶥����
			if (pattern_answer[num].patternnode[(*p).toNode].find((*iter).second.endnode) != pattern_answer[num].patternnode[(*p).toNode].end()) {
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.endnode);
			}
		}
		else if (existFlag == 2) { // Ϊ 2 ��num-1������
			map<int, NODEMATCH>::iterator toiter = pattern_answer[num - 1].patternnode[(*p).toNode].find((*iter).second.endnode);
			if (toiter != pattern_answer[num - 1].patternnode[(*p).toNode].end()) {
				// �ȴ�num-1���õ���Ӧ��ֵ���ٽ���ǰƥ��߼��뵽�м����У�����ƥ��߶��ڽ������Ҫ���Կ����Ȳ���ӣ�
				pattern_answer[num].patternnode[(*p).toNode][toiter->first] = toiter->second;
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.endnode);
			}
		}
		else if (existFlag == 4) { // Ϊ 4 ��num������
			if (pattern_answer[num].patternnode[(*p).fromNode].find((*iter).second.startnode) != pattern_answer[num].patternnode[(*p).fromNode].end()) {
				// �ȴ�num-1���õ���Ӧ��ֵ���ٽ���ǰƥ��߼��뵽�м����У�����ƥ��߶��ڽ������Ҫ���Կ����Ȳ���ӣ�
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.startnode);
			}
		}
		else if (existFlag == 5) { // Ϊ 5 ��num-1������
			map<int, NODEMATCH>::iterator fromiter = pattern_answer[num - 1].patternnode[(*p).fromNode].find((*iter).second.startnode);
			if (fromiter != pattern_answer[num - 1].patternnode[(*p).fromNode].end()) {
				pattern_answer[num].patternnode[(*p).fromNode][fromiter->first] = fromiter->second;
				// �ȴ�num-1���õ���Ӧ��ֵ���ٽ���ǰƥ��߼��뵽�м����У�����ƥ��߶��ڽ������Ҫ���Կ����Ȳ���ӣ�
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.startnode);
			}
		}
		else { // existFlag = 0 or 3
			AddSuitablePathToAnswer(p, (*iter).second, num);
			returnFlag++;
		}
		nodeSuitCache.push_back((*iter).second);
	}
	if (matchCache[(*p).edgeNo].size() == 0) {
		matchCache[(*p).edgeNo][endi] = nodeSuitCache;
	}
	else if (matchCache[(*p).edgeNo].find(endi) == matchCache[(*p).edgeNo].end()) { // ��ʵ��������жϣ���Ϊ�����˾Ϳ϶�������������cache��
		matchCache[(*p).edgeNo][endi] = nodeSuitCache;
	}
	else {
		// �Ѵ����ڻ����У������������
		cout << "�Ѵ����ڻ����У������������" << endl;
	}
	return returnFlag;
}


/**
 * ���һ��ƥ��ߵ�ƥ������
*/
void AddSuitablePathToAnswer(PArcNode *p, SUITABLE_PATH suitable_path, int num) {
	int fromNode = (*p).fromNode;
	int toNode = (*p).toNode;
	// ��ƥ��߼����Ӧ�ߵ�ƥ���б���
	map<int, NODEMATCH>* fromTemp = & pattern_answer[num].patternnode[fromNode]; //��ȡ��Ӧ�ڵ�ĺ�ѡ����
	map<int, NODEMATCH>::iterator fromIter = (*fromTemp).find(suitable_path.startnode); //��ȡ��ѡ�ڵ�
	if (fromIter != pattern_answer[num].patternnode[fromNode].end()) {  // �ж���ʼ�ڵ��Ƿ���ڣ�������
		map<int,ArcPathN>* outEdgeTemp = & (fromIter->second).out_edge;
		if ((*outEdgeTemp).find(toNode) != (*outEdgeTemp).end()) {
			(*outEdgeTemp)[toNode].suitable[suitable_path.endnode] = suitable_path; //��ƥ��߼����Ӧ��map�� 
		}
		else {
			ArcPathN arcpathNTemp;
			arcpathNTemp.suitable[suitable_path.endnode] = suitable_path;
			(*outEdgeTemp)[toNode] = arcpathNTemp;
		}		
	}
	else {	// ��������
		NODEMATCH nodematchTemp;
		nodematchTemp.matchnode = suitable_path.startnode; //���ƥ�䶥��
		ArcPathN arcpathNTemp;
		arcpathNTemp.suitable[suitable_path.endnode] = suitable_path; //��ƥ��߼����Ӧ��map��
		nodematchTemp.out_edge[toNode] = arcpathNTemp;
		pattern_answer[num].patternnode[fromNode][suitable_path.startnode] = nodematchTemp;	 //��ƥ����ʼ�ڵ�����Ӧ�ĺ�ѡ�ڵ�map��
	}
	// �ж�ƥ��ߵ�endnode�Ƿ�����ڶ�Ӧ�ڵ�ĺ�ѡ�ڵ��У��������������
	map<int, NODEMATCH>* toTemp = &pattern_answer[num].patternnode[toNode];
	map<int, NODEMATCH>::iterator toIter = (*toTemp).find(suitable_path.endnode);
	//if (toIter != pattern_answer[num].patternnode[toNode].end()) {
	//	map<int, INEDGE>* inEdgeTemp = & (toIter->second).in_edge;
	//	if ((*inEdgeTemp).find(fromNode) != (*inEdgeTemp).end()) {
	//		//(*inEdgeTemp)[fromNode].tonode = toNode;
	//		(*inEdgeTemp)[fromNode].in[suitable_path.startnode]=suitable_path.endnode;
	//	}
	//	else {
	//		INEDGE newinedge;
	//		newinedge.tonode = toNode;
	//		newinedge.in[suitable_path.startnode] = suitable_path.endnode;
	//		(*inEdgeTemp)[fromNode] = newinedge;
	//	}
	//}
	if (toIter == pattern_answer[num].patternnode[toNode].end()){
		NODEMATCH nodematchTemp;
		nodematchTemp.matchnode = suitable_path.endnode;  //���ƥ�䶥��
		//INEDGE inedge;
		//inedge.tonode = toNode;
		//inedge.in[suitable_path.startnode] = suitable_path.endnode;
		//nodematchTemp.in_edge[fromNode] = inedge;
		pattern_answer[num].patternnode[toNode][suitable_path.endnode]= nodematchTemp;
	}
}

/**
* �ӻ����ж�ȡ�����һ��ƥ��ڵ㵽ƥ������
*/
int AddSuitableCacheNodeToAnswer(PArcNode *p, vector<int> *connected_node_list, int num, int sourcenode, int type) {
	int fromNode = (*p).fromNode;
	int toNode = (*p).toNode;
	if (matchCache[(*p).edgeNo][sourcenode].size() == 0) {
		return false;
	}
	int flag = 0;
	for (int i = 0; i<matchCache[(*p).edgeNo][sourcenode].size(); i++) {
		SUITABLE_PATH suitable_path = matchCache[(*p).edgeNo][sourcenode][i];
		if (type == 0 || type == 3) { // ����ƥ�䣨������ȷ����ʼ�ڵ���ڣ�Ҳ����ȷ����ֹ�ڵ���ڣ�
			flag++;
			AddSuitablePathToAnswer(p, suitable_path, num);
		}
		else if (type == 1) { // type=1�뵱ǰƥ������������
			map<int, NODEMATCH>* toTemp = &pattern_answer[num].patternnode[toNode];
			map<int, NODEMATCH>::iterator toIter = (*toTemp).find(suitable_path.endnode);
			if (toIter != (*toTemp).end()) {
				flag++;
				// ��ƥ��߼����Ӧ�ߵ�ƥ���б���
				AddSuitablePathToAnswer(p, suitable_path, num);
				connected_node_list->push_back(suitable_path.endnode);
			}
			else {
				// do-nothing, ��ǰƥ��߲���������Ҫ��
			}
		}
		else if (type == 2) { // type=2����һ�׶�ƥ������������
			map<int, NODEMATCH>* toTemp = &pattern_answer[num - 1].patternnode[toNode];
			map<int, NODEMATCH>::iterator toIter = (*toTemp).find(suitable_path.endnode);
			if (toIter != (*toTemp).end()) {
				flag++;
				pattern_answer[num].patternnode[(*p).toNode][toIter->first] = toIter->second;
				// ��ƥ��߼����Ӧ�ߵ�ƥ���б���
				AddSuitablePathToAnswer(p, suitable_path, num);
				connected_node_list->push_back(suitable_path.endnode);
			}
			else {
				// do-nothing, ��ǰƥ��߲���������Ҫ��
			}
		}
		else if (type == 4) { // type=4�뵱ǰ�׶�ƥ�����������ӣ�����ƥ�䣩
			map<int, NODEMATCH>* fromTemp = &pattern_answer[num].patternnode[fromNode];
			map<int, NODEMATCH>::iterator fromIter = (*fromTemp).find(suitable_path.startnode);
			if (fromIter != (*fromTemp).end()) {
				flag++;
				// ��ƥ��߼����Ӧ�ߵ�ƥ���б���
				AddSuitablePathToAnswer(p, suitable_path, num);
				connected_node_list->push_back(suitable_path.startnode);
			}
			else {
				// do-nothing, ��ǰƥ��߲���������Ҫ��
			}
		}
		else if (type == 5) { // type=5����һ�׶�ƥ�����������ӣ�����ƥ�䣩
			map<int, NODEMATCH>* fromTemp = &pattern_answer[num - 1].patternnode[fromNode];
			map<int, NODEMATCH>::iterator fromIter = (*fromTemp).find(suitable_path.startnode);
			if (fromIter != (*fromTemp).end()) {
				flag++;
				pattern_answer[num].patternnode[(*p).fromNode][fromIter->first] = fromIter->second;
				// ��ƥ��߼����Ӧ�ߵ�ƥ���б���
				AddSuitablePathToAnswer(p, suitable_path, num);
				connected_node_list->push_back(suitable_path.startnode);
			}
			else {
				// do-nothing, ��ǰƥ��߲���������Ҫ��
			}
		}
		else {
			// typeδ֪������Ӧ�ò���ִ�е���
		}
	}
	return flag;
}



/************************************************************  �����   ******************************************************/
bool TopologicalMatching3(int FNode, int num) {

	// ��ʼ��ƥ��߱��
	int matEdgNo = 0;

	// ��ƥ����ʼ�ڵ�����
	PArcNode *panINF = P.pnodeList[1].fstinArc;
	while (panINF != NULL) {
		if (panINF->edgeNo != matEdgNo) {
			cout << "��һ��ƥ��ڵ㣬���ƥ�����" << endl;
			panINF = panINF->nextin;
			continue;
		}
		int flag1 = 0;
		vector<int> connected_node_list;
		flag1 = inverse_edge_match(FNode, num, panINF, &connected_node_list, 3); // ����
		if (flag1 < P.pnodeList[panINF->fromNode].needc) { // û�еõ����ʵ�ƥ��ߣ�ƥ���Ϊ�ռ���ɾ����
			for (int x = 1; x <= P.pvexnum; x++) {
				pattern_answer[num].patternnode[x].clear();
			}
			return false;
		}
		panINF = panINF->nextin;
		matEdgNo++;
	}

	// ��ƥ����ʼ�ڵ�ĳ���
	PArcNode *panF = P.pnodeList[num].fstoutArc;
	while (panF != NULL) { // ѭ��ƥ����ʼ�ڵ�����г���
		if (panF->edgeNo != matEdgNo) {
			cout << "��һ��ƥ��ڵ㣬����ƥ�����" << endl;
			panF = panF->nextout;
			continue;
		}
		int flag2 = 0;
		// �ж��Ƿ���Ҫ�������ӣ����Ӳ��ϵ�ɾ�������Ӳ��ϵĵ���ƥ���Ϊ�ռ���
		int flag1 = 0;
		if (pattern_answer[num].patternnode[(*panF).fromNode].size() && pattern_answer[num].patternnode[(*panF).toNode].size()) {
			flag1 = 1; //���б�ƥ��ʱ��Ҫ��������
		}
		vector<int> connected_node_list;
		flag2 = edge_match(FNode, num, panF, &connected_node_list, flag1); //����
		// �������ƥ����ȼ���̽ڵ�����Ƿ�����Ҫ��
		//if (pattern_answer[num].patternnode[pan->toNode].size() < P.pnodeList[pan->toNode].needc) {
		//	flag1 = false;
		//}
		if (flag2 < P.pnodeList[panF->toNode].needc) { // û�еõ����ʵ�ƥ��ߣ�ƥ���Ϊ�ռ���ɾ����
			for (int x = 1; x <= P.pvexnum; x++) {
				pattern_answer[num].patternnode[x].clear();
			}
			return false;
		}
		// ����һ�����Ӻ�Ҫɾ��û�������ϵĽڵ�
		if (flag1) {
			map<int, NODEMATCH>::iterator iter = pattern_answer[num].patternnode[panF->toNode].begin();
			while (iter != pattern_answer[num].patternnode[panF->toNode].end()) {
				if (find(connected_node_list.begin(), connected_node_list.end(), iter->first) == connected_node_list.end()) {
					iter = pattern_answer[num].patternnode[panF->toNode].erase(iter);
				}
				else {
					++iter;
				}
			}
		}
		panF = panF->nextout;
		matEdgNo++;
	}
	//�ݹ�ƥ���ǰ���Ѿ�ƥ����ĵ�һ���ڵ����ʣ��ڵ�
	bool rflag = TopologicalMatching4(num + 1, matEdgNo);
	if (rflag)return true;
	else {
		for (int x = 1; x <= P.pvexnum; x++) {
			pattern_answer[num].patternnode[x].clear();
		}
		return false;
	}
}


bool TopologicalMatching4(int num, int matEdgNo){

	/* ƥ��ǰ�Ȼ�ȡ��ƥ��ڵ���������*/
	vector<VEC> a; // �洢���п��ܵ����
	VEC b; // �ݴ�һ�����
	//int matEdgNotmp = *matEdgNo;
	int curr_nodeNo = BrDOT[num];
	// ����һ���ƥ���м����л�ȡ��ѡ�ڵ�ӵ�����ƥ���м�����
	for (int i = 1; i < num; i++) {
		pattern_answer[num].patternnode[BrDOT[i]] = pattern_answer[num - 1].patternnode[BrDOT[i]];
	}
	// ��������һ����ƥ��ڵ㣬��ѡ����Ҫ��ƥ���ѡ�ڵ����ƥ���м����У�����
	if (num == P.pvexnum) {
		if (pattern_answer[num - 1].patternnode[curr_nodeNo].size() >= P.pnodeList[curr_nodeNo].needc) {
			map<int, NODEMATCH>::iterator iter = pattern_answer[num - 1].patternnode[curr_nodeNo].begin();
			for (int i = 0; i < P.pnodeList[curr_nodeNo].needc; i++,iter++) {
				pattern_answer[num].patternnode[curr_nodeNo][iter->first] = iter->second;
			}
			return true;
		}
		else {
			return false;
		}
	}

	// ����һ���ƥ���м����л�ȡ��ǰ��ƥ��ڵ�ĺ�ѡ�ڵ��б�
	vector<pair<int, bool>> curr_nod_list;
	curr_nod_list.push_back(pair<int,bool>(0,false));
	map<int, NODEMATCH>::reverse_iterator riter = pattern_answer[num-1].patternnode[curr_nodeNo].rbegin();
	for (; riter != pattern_answer[num-1].patternnode[curr_nodeNo].rend(); riter++) {
		curr_nod_list.push_back(pair<int, bool>(riter->first,true));
	}
	// �������е����
	int returnFlag = comb2(matEdgNo, num, b, curr_nod_list, pattern_answer[num - 1].patternnode[curr_nodeNo].size(), pattern_answer[num - 1].patternnode[curr_nodeNo].size(), P.pnodeList[curr_nodeNo].needc);
	if (returnFlag) {
		return true;
	}
	else {
		for (int x = num; x <= P.pvexnum; x++) {
			pattern_answer[num].patternnode[BrDOT[x]].clear();
		}
		return false; // ����������Ϻ���û���ƥ����
	}
	///*��ÿ����Ͻ�����̽�ݹ�ƥ��*/
	//for (vector<VEC>::iterator iter = a.begin(); iter != a.end();iter++) {
	//	int flag1 = combineTesing(*iter, &matEdgNo, num);
	//	if (flag1) {
	//		// ����Ƿ��б���δ�õ��Ľڵ�ĺ�ѡ�ڵ㼯û�м��뵽��ǰ��
	//		for (int i = 1; i <= P.pvexnum; i++) {
	//			if (pattern_answer[num - 1].patternnode[i].size() > 0 && pattern_answer[num].patternnode[i].size() == 0) {
	//				pattern_answer[num].patternnode[i] = pattern_answer[num - 1].patternnode[i];
	//			}
	//		}
	//		bool rflag = TopologicalMatching4(num + 1, matEdgNo);
	//		if (rflag) return true; // ��ý���󣬲��ٱ���Ѱ������ƥ������
	//	}
	//	else {
	//		//���ڵ�ƥ��û�����⣬�����²�ڵ�ƥ��������⣬����������һ�����
	//		//*matEdgNo = matEdgNotmp;
	//	}
	//}
}



bool combineTesing(VEC combine, vector<pair<int, bool>> &curr_nod_list, int *matEdgNo,int candCount, int num) {

	int curr_nodeNo = BrDOT[num]; //��ǰ��ƥ��ģʽ�ڵ�
	int matEdgNoTmp = *matEdgNo;
	// ����ǰ��ϼ����ѡ�ڵ㼯��
	for (VEC::iterator iter = combine.begin(); iter != combine.end(); iter++) {
		pattern_answer[num].patternnode[curr_nodeNo][*iter] = pattern_answer[num - 1].patternnode[curr_nodeNo][*iter];
	}
	// ��ȡ��ѡ�ڵ㼯
	map<int, NODEMATCH> Candidate = pattern_answer[num].patternnode[curr_nodeNo];
	// ��ȡ��ѡ�ڵ㼯�ĵ�����
	map<int, NODEMATCH>::iterator strmap_iter1 = Candidate.begin();
	// ѭ����ÿһ����ѡ�ڵ����ģʽ��ƥ��
	for (; strmap_iter1 != Candidate.end(); strmap_iter1++) { // ����ÿһ����ѡ�ڵ�ѭ��ƥ��
		*matEdgNo = matEdgNoTmp; // ��ǰ�ߵ�ƥ����������¸�ֵ
		// ��ƥ�䵱ǰ��ƥ��ڵ�����
		PArcNode *panIN = P.pnodeList[curr_nodeNo].fstinArc;
		while (panIN != NULL) {
			if (panIN->edgeNo != *matEdgNo) {
				panIN = panIN->nextin;
				continue;
			}
			int tonode = strmap_iter1->first;
			vector<int> connected_node_list;
			// �ж��Ƿ���Ҫ�������ӣ����Ӳ��ϵ�ɾ�������Ӳ��ϵĵ���ƥ���Ϊ�ռ���
			int flag1 = 3;
			if (pattern_answer[num].patternnode[(*panIN).fromNode].size()) {	//���б�ƥ��ʱ��Ҫ��ǰ��ƥ������������
				flag1 = 4; 
			}
			else if (pattern_answer[num-1].patternnode[(*panIN).fromNode].size()) {	//���б�ƥ��ʱ��Ҫ����һ��ƥ������������
				flag1 = 5; 
			}

			int flag2 = 0;
			/* ���б�ƥ��  */
			if (matchCache[(*panIN).edgeNo].find(tonode) != matchCache[(*panIN).edgeNo].end()) {
				flag2 = AddSuitableCacheNodeToAnswer(panIN, &connected_node_list, num, tonode, flag1);
			}
			else {
				flag2 = inverse_edge_match(tonode, num, panIN, &connected_node_list, flag1); //����
			}
			// ���е�ǰ��ƥ��󣬼����ѡ�ڵ�����Ƿ�����Ҫ��
			if (flag2 < P.pnodeList[panIN->fromNode].needc) { // û�еõ��㹻������ƥ��ߣ�ɾ����ǰ��֮��ڵ��ƥ���м��������أ�
				for (int x = num; x <= P.pvexnum; x++) {
					pattern_answer[num].patternnode[BrDOT[x]].clear();
				}
				// �жϵ�ǰ��ƥ��ڵ�����ƥ���Ƿ��������Ҫ��
				if (matchCache[(*panIN).edgeNo][tonode].size() < P.pnodeList[panIN->fromNode].needc) {
					for (int i = 1; i <= candCount; i++) {
						if (curr_nod_list[i].first == tonode) {
							curr_nod_list[i].second = false;
							break;
						}
					}
				}
				return false;
			}
			// ����һ�����Ӻ�Ҫɾ��û�������ϵĽڵ�
			//if (flag1>3) {
			//	map<int, NODEMATCH>::reverse_iterator riter = pattern_answer[num].patternnode[panIN->fromNode].rbegin();
			//	for (; riter != pattern_answer[num].patternnode[panIN->fromNode].rend(); riter++) {
			//		if (find(connected_node_list.begin(), connected_node_list.end(), riter->first) == connected_node_list.end()) {
			//			pattern_answer[num].patternnode[panIN->fromNode].erase(riter->first);
			//		}
			//	}
			//}
			if (flag1>3) {
				map<int, NODEMATCH>::iterator iter = pattern_answer[num].patternnode[panIN->fromNode].begin();
				while (iter != pattern_answer[num].patternnode[panIN->fromNode].end()) {
					if (find(connected_node_list.begin(), connected_node_list.end(), iter->first) == connected_node_list.end()) {
						iter = pattern_answer[num].patternnode[panIN->fromNode].erase(iter);
					}
					else {
						++iter;
					}
				}
			}
			panIN = panIN->nextin;
			(*matEdgNo)++;
		}
		// ��ƥ�䵱ǰ��ƥ��ڵ�ĳ���
		PArcNode *panOut = P.pnodeList[curr_nodeNo].fstoutArc;
		while (panOut != NULL) {
			if (panOut->edgeNo != *matEdgNo) {
				panOut = panOut->nextout;
				continue;
			}
			int startnode = strmap_iter1->first;
			vector<int> connected_node_list;
			// �ж��Ƿ���Ҫ�������ӣ��Լ���ô�������ӣ����Ӳ��ϵ�ɾ�������Ӳ��ϵĵ���ƥ���Ϊ�ռ���
			int flag1 = 0;
			int flag2 = 0;
			if (pattern_answer[num].patternnode[(*panOut).toNode].size() > 0) { // ��ǰ��ƥ��ߵ���ֹ�ڵ�ĺ�ѡ�ڵ㲻�գ�һ��Ҫ��������
				flag1 = 1;
			}
			//else if (P.pnodeList[(*panOut).toNode].indegree > 1 && P.pnodeList[(*panOut).toNode].firstinfrom != curr_nodeNo) { // �ǵ�һ�ε�����C->D����������� (Ҳ��������ߵ������жϷ�ʽ�����ж�)
			else if(pattern_answer[num-1].patternnode[(*panOut).toNode].size() > 0){
				flag1 = 2;
			}
			/* ���б�ƥ��  */
			if (matchCache[(*panOut).edgeNo].find(startnode) != matchCache[(*panOut).edgeNo].end()) {
				flag2 = AddSuitableCacheNodeToAnswer(panOut, &connected_node_list, num, startnode, flag1);
			}
			else {
				flag2 = edge_match(startnode, num, panOut, &connected_node_list, flag1); // ����, ��flag2�����Ƿ���Ҫɾ����ǰƥ�䶥��
			}
			// ���е�ǰ��ƥ��󣬼����ѡ�ڵ�����Ƿ�����Ҫ��
			if (flag2 < P.pnodeList[panOut->toNode].needc) { // û�еõ��㹻������ƥ��ߣ�ɾ����ǰ��֮��ڵ��ƥ���м��������أ�
				for (int x = num; x <= P.pvexnum; x++) {
					pattern_answer[num].patternnode[BrDOT[x]].clear();
				}
				// �жϵ�ǰ��ƥ��ڵ�����ƥ���Ƿ��������Ҫ��
				if (matchCache[(*panOut).edgeNo][startnode].size() < P.pnodeList[panOut->toNode].needc) {
					for (int i = 1; i <= candCount; i++) {
						if (curr_nod_list[i].first == startnode) {
							curr_nod_list[i].second = false;
							break;
						}
					}
				}
				return false;
			}
			// ����һ�����Ӻ�Ҫɾ��û�������ϵĽڵ�
			if (flag1) {
				map<int, NODEMATCH>::iterator iter = pattern_answer[num].patternnode[panOut->toNode].begin();
				while (iter != pattern_answer[num].patternnode[panOut->toNode].end()) {
					if (find(connected_node_list.begin(), connected_node_list.end(), iter->first) == connected_node_list.end()) {
						iter = pattern_answer[num].patternnode[panOut->toNode].erase(iter);
					}
					else {
						++iter;
					}
				}
			}
			panOut = panOut->nextout;
			(*matEdgNo)++;
		}
	}
	return true;
}


void Answer_Store3(char* filename,int num) {

	ofstream file(filename, ios::app);
	int i = 1;
	int j = 1;
	double membership = 0.0;

	file << "pattern answer" << graph_sum << ":" << endl;
	//file << "pattern answer" << graph_sum << ":" << membership / P.parcnum << endl;  // ģ���ȶ����о���̫�ü��㰡��

	membership = 0.0;
	// ���ƥ��ڵ㼯��
	for (int x = 1; x <= P.pvexnum; x++) {
		file << "vertex" << x << ": �ڵ���� " << pattern_answer[num].patternnode[x].size() << endl;
		//pattern_answer[num].patternnode[x].size();
		map<int, NODEMATCH>::iterator strmap_iter1 = pattern_answer[num].patternnode[x].begin();
		for (; strmap_iter1 != pattern_answer[num].patternnode[x].end(); strmap_iter1++) {
			int matchnode = strmap_iter1->first;
			file << G.NodeList[matchnode].nodenum << " " << G.NodeList[matchnode].nodelable << " " << G.NodeList[matchnode].factor << endl;
		}
	}

	// ���ƥ��߼���
	for (int x = 1; x <= P.pvexnum; x++) {
		file << "edge_match"  << ": " << endl;
		//pattern_answer[num].patternnode[x].size();
		map<int, NODEMATCH>::iterator strmap_iter2 = pattern_answer[num].patternnode[x].begin();
		for (; strmap_iter2 != pattern_answer[num].patternnode[x].end(); strmap_iter2++) {
			int matchnode = strmap_iter2->first;
			map<int, ArcPathN>::iterator matchedge_iter1 = strmap_iter2->second.out_edge.begin();
			for (; matchedge_iter1 != strmap_iter2->second.out_edge.end(); matchedge_iter1++) { //���ܰ���������̱�
				int tonode = matchedge_iter1->first;
				file << "from " << x << " to " << tonode << " matchnode " << matchnode  << endl<<endl;
				//file << "from " << x << " to " << tonode << " matchnode " << matchnode << " ������ " << matchedge_iter1->second.suitable.size() << endl << endl;
				map<int, SUITABLE_PATH>::iterator suitable_edge_iter1 = matchedge_iter1->second.suitable.begin();
				for (; suitable_edge_iter1 != matchedge_iter1->second.suitable.end(); suitable_edge_iter1++) { // ÿ��ģʽ�߿��ܰ�������ƥ���
					// SUITABLE_PATH suitable_path_temp = suitable_edge_iter1->second;
					if (pattern_answer[P.pvexnum].patternnode[tonode].find(suitable_edge_iter1->first) == pattern_answer[P.pvexnum].patternnode[tonode].end()) {
						continue; //���˵�������������ƥ���
					}
					file << G.NodeList[suitable_edge_iter1->second.startnode].nodenum << " " << G.NodeList[suitable_edge_iter1->second.endnode].nodenum << " " << suitable_edge_iter1->second.path_list.size() - 1 << "  " << suitable_edge_iter1->second.t << "  " << suitable_edge_iter1->second.r << "  " << suitable_edge_iter1->second.rou << "  " << suitable_edge_iter1->second.path_fuzzy << endl;
				}
			}
		}
	}

	file << endl;
}


void GetMemoryAndTime(char* filename,time_t starttime, time_t finishtime, int answernum, char *info)
{

	int usingtime = int(difftime(finishtime, starttime));

	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	ofstream f(filename, ios::app);
	if (!f.is_open()) {
		cout << "Message record file failed to open!" << '\n';
		return;
	}
	else {
		f << info << "elapsed time: " << usingtime << " " << answernum << '\n';
		f << "memory usage:" << pmc.WorkingSetSize / 1000 << "K/" << pmc.PeakWorkingSetSize / 1000 << "K + " << pmc.PagefileUsage / 1000 << "K/" << pmc.PeakPagefileUsage / 1000 << "K" << '\n';
		f << '\n';
	}
	f.close();
	cout << info << "elapsed time: " << usingtime << " " << answernum << '\n';
	cout << "memory usage:" << pmc.WorkingSetSize / 1000 << "K/" << pmc.PeakWorkingSetSize / 1000 << "K + " << pmc.PagefileUsage / 1000 << "K/" << pmc.PeakPagefileUsage / 1000 << "K" << '\n';
}

void GetMemoryAndTime2(char* filename, char* algorithm, char* pattern_graph, char* datasetname, time_t starttime, time_t finishtime, int answernum)
{

	int usingtime = int(difftime(finishtime, starttime));

	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	ofstream f(filename, ios::app);
	if (!f.is_open()) {
		cout << "Message record file failed to open!" << '\n';
		return;
	}
	else {
		f << algorithm << "," << pattern_graph << "," << answernum << ',' << usingtime << ","
			<< datasetname << "," << pmc.PeakWorkingSetSize / 1000 << "KB";
		f << '\n';
	}
	f.close();
}
