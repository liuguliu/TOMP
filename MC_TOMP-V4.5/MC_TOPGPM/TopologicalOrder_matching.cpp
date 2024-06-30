//#include <dhcpsapi.h>
#include<iostream>
#include<vector>
#include<queue>
#include<list>
#include<stack>
#include <time.h> 
#include<string.h>
#include<fstream>
// 获取内存使用情况
#include <windows.h>
#include <Psapi.h>
// utf-8 编码文件
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
//定义全局变量
vector<int> BrDOT;
PATTERN_ANSWER pattern_answer[MAX];
float T_membership = 0.0, R_membership = 0.0, Rou_membership = 0.0;
int graph_sum = 0;
double this_start_time, this_finish_time; /* 每组结果的开始时间，结束时间 */

// 创建一个用于存储中间结果的文件夹
char folderPath[256];

//定义本程序使用的全局变量
vector<MATCHNODECACHE> matchCache;
//SUITABLE_PATH ss[EDGECACHEN];

void init() {
	TopologicalNodeTraverse(1);
	for (int i = 0; i <= P.parcnum; i++) {
		MATCHNODECACHE nodeCacheTemp;
		matchCache.push_back(nodeCacheTemp);
	}

	nodeCandidate[1] = GetNodeCandidate(1); // 获取源节点的候选节点集合
	//for (int i = 1; i <= P.pvexnum; i++) {
	//	if (P.pnodeList[i].indegree == 0) {
	//		nodeCandidate[i] = GetNodeCandidate(i); // 获取源节点的候选节点集合
	//	}
	//}
}


void TopologicalNodeTraverse(int nodeNo) {
	queue<int> Q;
	int indegreetemp[20];
	memset(indegreetemp, 0, 20 * sizeof(int));
	for (int i = 1; i <= P.pvexnum; i++) {	// 计算模式图中每个节点的出度和入度
		PArcNode *parctemp = P.pnodeList[i].fstoutArc;
		while (parctemp != NULL) {
			P.pnodeList[i].outdegree++;
			P.pnodeList[parctemp->toNode].indegree++;
			indegreetemp[parctemp->toNode]++;
			parctemp = parctemp->nextout;
		}
	}

	// 从起始节点开始遍历整个模式图，因为模式图为连通图，所以从任意节点开始都能完成图的遍历
	int TravEdgeNum = 0;
	int TravNodeCur = nodeNo;
	int TravNodeNum = 1;
	BrDOT.push_back(-1); // 占用BrDOT[0]，使得待匹配节点的下标从1开始
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
* 计算一组数据的所有组合，m表示总元素个数，k表示一个组合中元素的个数
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
		if (curr_nod_list[i].second) { //判断该候选节点是否已被排除
			b.push_back(curr_nod_list[i].first);
			if (k>1) {
				bool returnFlag = comb2(matEdgNo,num,b, curr_nod_list, candCount, i - 1, k - 1);
				if (returnFlag) return true;
				b.pop_back();
			}
			else {
				//counts++;
				//a.push_back(b);
				// 对组合进行匹配测试
				int flag1 = combineTesing(b, curr_nod_list, &matEdgNo, candCount, num);
				if (flag1) {
					// 检测是否有本轮未用到的节点的候选节点集没有加入到当前层
					for (int i = 1; i <= P.pvexnum; i++) {
						if (pattern_answer[num - 1].patternnode[i].size() > 0 && pattern_answer[num].patternnode[i].size() == 0) {
							pattern_answer[num].patternnode[i] = pattern_answer[num - 1].patternnode[i];
						}
					}
					bool rflag = TopologicalMatching4(num + 1, matEdgNo);
					if (rflag) return true; // 获得结果后，不再遍历寻找其他匹配结果；
					else {
						for (int x = num; x <= P.pvexnum; x++) {
							pattern_answer[num].patternnode[BrDOT[x]].clear();
						}
						matEdgNo = matEdgNotmp;
					}
				}
				else {
					//本节点匹配没有问题，但是下层节点匹配出现问题，继续测试下一种组合
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


int edge_match(int starti, int num, PArcNode *p, vector<int> *connected_node_list, int existFlag) {//遍历数据图,对数据图中标签与模式边的起始节点相同的节点进行有届深度搜索

	int returnFlag = 0;
	map<int, SUITABLE_PATH> edgeCandidate;
	SUITABLE_PATH_LIST nodeSuitCache; // 记录与当前匹配节点相关的匹配边	
	queue<ArcPath> q;	//定义一个队列辅助广度优先搜索	
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
		(*pathj) = q.front();	//获取队头元素
		q.pop();	//队头元素出队
		if ((G.NodeList[(*pathj).suitable_path.endnode].nodelable == P.pnodeList[(*p).toNode].PNodeLable) &&
			(G.NodeList[(*pathj).suitable_path.endnode].factor - P.pnodeList[(*p).toNode].factor>-0.000001)) {	//找到一个满足bounded length的路径																												// 计算匹配路径各聚合属性的值
			ArcPath store_path = *pathj;
			store_path.suitable_path.rou = (*pathj).suitable_path.rou / ((*pathj).path.size() + 1);
			if ((store_path.suitable_path.rou - (*p).pfactor) > -0.000001) {
				store_path.suitable_path.Length = store_path.suitable_path.path_list.size() - 1;
				// 判断匹配路径是否可以替换原有匹配
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
				// 不符合路径rou值约束，继续
			}
		}
		if ((*pathj).path.size() < (*p).boundlength) {
			ArcNode *arctemp;
			arctemp = G.NodeList[(*pathj).suitable_path.endnode].firstoutarc; // 获取当前顶点的第一条邻接边
			while (arctemp != NULL) {
				ArcPath cur_path;
				cur_path.path = (*pathj).path;
				cur_path.path.push_back((*arctemp));
				cur_path.suitable_path.startnode = starti;
				cur_path.suitable_path.endnode = (*arctemp).adjvex;
				cur_path.suitable_path.path_list = (*pathj).suitable_path.path_list;
				cur_path.suitable_path.path_list.push_back((*arctemp).adjvex);
				// 都是在pathj的基础上进行运算的
				cur_path.suitable_path.rou = (*pathj).suitable_path.rou + G.NodeList[(*arctemp).adjvex].factor;
				cur_path.suitable_path.t = (*pathj).suitable_path.t * (*arctemp).trust;
				cur_path.suitable_path.r = (*pathj).suitable_path.r * (*arctemp).intimacy;

				// 判断各属性值是否满足多约束匹配要求（因为属性值均在0-1之间，所以当存在不满足约束的属性值时可进行剪枝）
				if ((cur_path.suitable_path.r - (*p).pintimacy) > -0.000001 && (cur_path.suitable_path.t - (*p).ptrust) > -0.000001) {
					if (cur_path.path.size() == (*p).boundlength) {
						if ((G.NodeList[cur_path.suitable_path.endnode].nodelable == P.pnodeList[(*p).toNode].PNodeLable)) {
							q.push(cur_path);
						}
						else {
							// 达到约束路径长度，但终点不符合要求
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
	  // 将所有匹配边逐个加入候选匹配边集合中
	for (map<int, SUITABLE_PATH>::iterator iter = edgeCandidate.begin(); iter != edgeCandidate.end(); iter++) { //循环遍历以查询到的边匹配列表
																												//将匹配边加入到匹配结果中
		if (existFlag == 1) { // 为 1 本num中连接
							  // 判断当前匹配边是否能连接到已有的匹配顶点上
			if (pattern_answer[num].patternnode[(*p).toNode].find((*iter).second.endnode) != pattern_answer[num].patternnode[(*p).toNode].end()) {
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.endnode);
			}
		}
		else if (existFlag == 2) { // 为 2 与num-1中连接
			map<int, NODEMATCH>::iterator toiter = pattern_answer[num - 1].patternnode[(*p).toNode].find((*iter).second.endnode);
			if (toiter != pattern_answer[num - 1].patternnode[(*p).toNode].end()) {
				// 先从num-1中拿到对应的值，再将当前匹配边加入到中间结果中（由于匹配边对于结果不重要所以可以先不添加）
				pattern_answer[num].patternnode[(*p).toNode][toiter->first] = toiter->second;
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.endnode);
			}
		}
		else if (existFlag == 4) { // 为 4 与num中连接
			if (pattern_answer[num].patternnode[(*p).fromNode].find((*iter).second.startnode) != pattern_answer[num].patternnode[(*p).fromNode].end()) {
				// 先从num-1中拿到对应的值，再将当前匹配边加入到中间结果中（由于匹配边对于结果不重要所以可以先不添加）
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.startnode);
			}
		}
		else if (existFlag == 5) { // 为 5 与num-1中连接
			map<int, NODEMATCH>::iterator fromiter = pattern_answer[num - 1].patternnode[(*p).fromNode].find((*iter).second.startnode);
			if (fromiter != pattern_answer[num - 1].patternnode[(*p).fromNode].end()) {
				pattern_answer[num].patternnode[(*p).fromNode][fromiter->first] = fromiter->second;
				// 先从num-1中拿到对应的值，再将当前匹配边加入到中间结果中（由于匹配边对于结果不重要所以可以先不添加）
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
	else if (matchCache[(*p).edgeNo].find(starti) == matchCache[(*p).edgeNo].end()) { // 其实这个不用判断，因为进来了就肯定不存在于已有cache中
		matchCache[(*p).edgeNo][starti] = nodeSuitCache;
	}
	else {
		// 已存在于缓存中，不用重新添加
		cout << "已存在于缓存中，不用重新添加" << endl;
	}
	return returnFlag;
}

int inverse_edge_match(int endi, int num, PArcNode *p, vector<int> *connected_node_list, int existFlag) {

	int returnFlag = 0;
	map<int, SUITABLE_PATH> edgeCandidate;
	SUITABLE_PATH_LIST nodeSuitCache; // 记录与当前匹配节点相关的匹配边
	queue<ArcPath> q;	//定义一个队列辅助广度优先搜索
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
		(*pathj) = q.front();	//获取队头元素
		q.pop();	//队头元素出队
		if ((G.NodeList[(*pathj).suitable_path.startnode].nodelable == P.pnodeList[(*p).fromNode].PNodeLable)
			&& (G.NodeList[(*pathj).suitable_path.startnode].factor - P.pnodeList[(*p).fromNode].factor>-0.000001)) {	//找到一个满足约束的路径
																														// 计算匹配路径各聚合属性的值
			ArcPath store_path = *pathj;
			store_path.suitable_path.rou = (*pathj).suitable_path.rou / ((*pathj).path.size() + 1);
			if ((store_path.suitable_path.rou - (*p).pfactor) > -0.000001) {
				//(*pathj).suitable_path.r = (*pathj).suitable_path.r / (*pathj).path.size();
				//(*pathj).suitable_path.t = (*pathj).suitable_path.t / (*pathj).path.size();
				store_path.suitable_path.Length = store_path.suitable_path.path_list.size() - 1;
				// 判断匹配路径是否可以替换原有匹配
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
				// 不符合路径rou值约束，继续
			}

		}
		if ((*pathj).path.size() < (*p).boundlength) {
			ArcNode *arctemp;
			arctemp = G.NodeList[(*pathj).suitable_path.startnode].firstinarc; // 获取当前顶点的第一条邻接边
			while (arctemp != NULL) {
				ArcPath cur_path;
				cur_path.path = (*pathj).path;
				cur_path.path.push_back((*arctemp));
				cur_path.suitable_path.startnode = (*arctemp).adjvex;
				cur_path.suitable_path.endnode = endi;
				cur_path.suitable_path.path_list = (*pathj).suitable_path.path_list;
				cur_path.suitable_path.path_list.push_front((*arctemp).adjvex);
				// 都是在pathj的基础上进行运算的
				cur_path.suitable_path.rou = (*pathj).suitable_path.rou + G.NodeList[(*arctemp).adjvex].factor;
				cur_path.suitable_path.t = (*pathj).suitable_path.t * (*arctemp).trust;
				cur_path.suitable_path.r = (*pathj).suitable_path.r * (*arctemp).intimacy;

				// 判断各属性值是否满足多约束匹配要求（因为属性值均在0-1之间，所以当存在不满足约束的属性值时可进行剪枝）
				if ((cur_path.suitable_path.r - (*p).pintimacy) > -0.000001 && (cur_path.suitable_path.t - (*p).ptrust) > -0.000001) {
					if (cur_path.path.size() == (*p).boundlength) {
						if ((G.NodeList[cur_path.suitable_path.startnode].nodelable == P.pnodeList[(*p).fromNode].PNodeLable)) {
							q.push(cur_path);
						}
						else {
							// 达到约束路径长度，但终点不符合要求
						}
					}
					else {
						q.push(cur_path);
					}
				}
				else {
					//剪枝
				}
				arctemp = (*arctemp).nextarc;
			}
		}
		delete pathj;
	}//while
	 // 将所有匹配边逐个加入候选匹配边集合中
	for (map<int, SUITABLE_PATH>::iterator iter = edgeCandidate.begin(); iter != edgeCandidate.end(); iter++) { //循环遍历以查询到的边匹配列表
																												//将匹配边加入到匹配结果中
		if (existFlag == 1) { // 为 1 本num中连接
							  // 判断当前匹配边是否能连接到已有的匹配顶点上
			if (pattern_answer[num].patternnode[(*p).toNode].find((*iter).second.endnode) != pattern_answer[num].patternnode[(*p).toNode].end()) {
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.endnode);
			}
		}
		else if (existFlag == 2) { // 为 2 与num-1中连接
			map<int, NODEMATCH>::iterator toiter = pattern_answer[num - 1].patternnode[(*p).toNode].find((*iter).second.endnode);
			if (toiter != pattern_answer[num - 1].patternnode[(*p).toNode].end()) {
				// 先从num-1中拿到对应的值，再将当前匹配边加入到中间结果中（由于匹配边对于结果不重要所以可以先不添加）
				pattern_answer[num].patternnode[(*p).toNode][toiter->first] = toiter->second;
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.endnode);
			}
		}
		else if (existFlag == 4) { // 为 4 与num中连接
			if (pattern_answer[num].patternnode[(*p).fromNode].find((*iter).second.startnode) != pattern_answer[num].patternnode[(*p).fromNode].end()) {
				// 先从num-1中拿到对应的值，再将当前匹配边加入到中间结果中（由于匹配边对于结果不重要所以可以先不添加）
				AddSuitablePathToAnswer(p, (*iter).second, num);
				returnFlag++;
				connected_node_list->push_back((*iter).second.startnode);
			}
		}
		else if (existFlag == 5) { // 为 5 与num-1中连接
			map<int, NODEMATCH>::iterator fromiter = pattern_answer[num - 1].patternnode[(*p).fromNode].find((*iter).second.startnode);
			if (fromiter != pattern_answer[num - 1].patternnode[(*p).fromNode].end()) {
				pattern_answer[num].patternnode[(*p).fromNode][fromiter->first] = fromiter->second;
				// 先从num-1中拿到对应的值，再将当前匹配边加入到中间结果中（由于匹配边对于结果不重要所以可以先不添加）
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
	else if (matchCache[(*p).edgeNo].find(endi) == matchCache[(*p).edgeNo].end()) { // 其实这个不用判断，因为进来了就肯定不存在于已有cache中
		matchCache[(*p).edgeNo][endi] = nodeSuitCache;
	}
	else {
		// 已存在于缓存中，不用重新添加
		cout << "已存在于缓存中，不用重新添加" << endl;
	}
	return returnFlag;
}


/**
 * 添加一条匹配边到匹配结果中
*/
void AddSuitablePathToAnswer(PArcNode *p, SUITABLE_PATH suitable_path, int num) {
	int fromNode = (*p).fromNode;
	int toNode = (*p).toNode;
	// 将匹配边加入对应边的匹配列表中
	map<int, NODEMATCH>* fromTemp = & pattern_answer[num].patternnode[fromNode]; //获取对应节点的候选集合
	map<int, NODEMATCH>::iterator fromIter = (*fromTemp).find(suitable_path.startnode); //获取候选节点
	if (fromIter != pattern_answer[num].patternnode[fromNode].end()) {  // 判断起始节点是否存在，若存在
		map<int,ArcPathN>* outEdgeTemp = & (fromIter->second).out_edge;
		if ((*outEdgeTemp).find(toNode) != (*outEdgeTemp).end()) {
			(*outEdgeTemp)[toNode].suitable[suitable_path.endnode] = suitable_path; //将匹配边加入对应的map中 
		}
		else {
			ArcPathN arcpathNTemp;
			arcpathNTemp.suitable[suitable_path.endnode] = suitable_path;
			(*outEdgeTemp)[toNode] = arcpathNTemp;
		}		
	}
	else {	// 若不存在
		NODEMATCH nodematchTemp;
		nodematchTemp.matchnode = suitable_path.startnode; //添加匹配顶点
		ArcPathN arcpathNTemp;
		arcpathNTemp.suitable[suitable_path.endnode] = suitable_path; //将匹配边加入对应的map中
		nodematchTemp.out_edge[toNode] = arcpathNTemp;
		pattern_answer[num].patternnode[fromNode][suitable_path.startnode] = nodematchTemp;	 //将匹配起始节点加入对应的候选节点map中
	}
	// 判断匹配边的endnode是否存在于对应节点的候选节点中，若不存在则添加
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
		nodematchTemp.matchnode = suitable_path.endnode;  //添加匹配顶点
		//INEDGE inedge;
		//inedge.tonode = toNode;
		//inedge.in[suitable_path.startnode] = suitable_path.endnode;
		//nodematchTemp.in_edge[fromNode] = inedge;
		pattern_answer[num].patternnode[toNode][suitable_path.endnode]= nodematchTemp;
	}
}

/**
* 从缓存中读取，添加一个匹配节点到匹配结果中
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
		if (type == 0 || type == 3) { // 正常匹配（即不能确定起始节点存在，也不能确定终止节点存在）
			flag++;
			AddSuitablePathToAnswer(p, suitable_path, num);
		}
		else if (type == 1) { // type=1与当前匹配结果进行连接
			map<int, NODEMATCH>* toTemp = &pattern_answer[num].patternnode[toNode];
			map<int, NODEMATCH>::iterator toIter = (*toTemp).find(suitable_path.endnode);
			if (toIter != (*toTemp).end()) {
				flag++;
				// 将匹配边加入对应边的匹配列表中
				AddSuitablePathToAnswer(p, suitable_path, num);
				connected_node_list->push_back(suitable_path.endnode);
			}
			else {
				// do-nothing, 当前匹配边不满足连接要求
			}
		}
		else if (type == 2) { // type=2与上一阶段匹配结果进行连接
			map<int, NODEMATCH>* toTemp = &pattern_answer[num - 1].patternnode[toNode];
			map<int, NODEMATCH>::iterator toIter = (*toTemp).find(suitable_path.endnode);
			if (toIter != (*toTemp).end()) {
				flag++;
				pattern_answer[num].patternnode[(*p).toNode][toIter->first] = toIter->second;
				// 将匹配边加入对应边的匹配列表中
				AddSuitablePathToAnswer(p, suitable_path, num);
				connected_node_list->push_back(suitable_path.endnode);
			}
			else {
				// do-nothing, 当前匹配边不满足连接要求
			}
		}
		else if (type == 4) { // type=4与当前阶段匹配结果进行连接（反向匹配）
			map<int, NODEMATCH>* fromTemp = &pattern_answer[num].patternnode[fromNode];
			map<int, NODEMATCH>::iterator fromIter = (*fromTemp).find(suitable_path.startnode);
			if (fromIter != (*fromTemp).end()) {
				flag++;
				// 将匹配边加入对应边的匹配列表中
				AddSuitablePathToAnswer(p, suitable_path, num);
				connected_node_list->push_back(suitable_path.startnode);
			}
			else {
				// do-nothing, 当前匹配边不满足连接要求
			}
		}
		else if (type == 5) { // type=5与上一阶段匹配结果进行连接（反向匹配）
			map<int, NODEMATCH>* fromTemp = &pattern_answer[num - 1].patternnode[fromNode];
			map<int, NODEMATCH>::iterator fromIter = (*fromTemp).find(suitable_path.startnode);
			if (fromIter != (*fromTemp).end()) {
				flag++;
				pattern_answer[num].patternnode[(*p).fromNode][fromIter->first] = fromIter->second;
				// 将匹配边加入对应边的匹配列表中
				AddSuitablePathToAnswer(p, suitable_path, num);
				connected_node_list->push_back(suitable_path.startnode);
			}
			else {
				// do-nothing, 当前匹配边不满足连接要求
			}
		}
		else {
			// type未知，出错，应该不会执行到此
		}
	}
	return flag;
}



/************************************************************  主入口   ******************************************************/
bool TopologicalMatching3(int FNode, int num) {

	// 初始化匹配边编号
	int matEdgNo = 0;

	// 先匹配起始节点的入边
	PArcNode *panINF = P.pnodeList[1].fstinArc;
	while (panINF != NULL) {
		if (panINF->edgeNo != matEdgNo) {
			cout << "第一个匹配节点，入边匹配出错" << endl;
			panINF = panINF->nextin;
			continue;
		}
		int flag1 = 0;
		vector<int> connected_node_list;
		flag1 = inverse_edge_match(FNode, num, panINF, &connected_node_list, 3); // 反向
		if (flag1 < P.pnodeList[panINF->fromNode].needc) { // 没有得到合适的匹配边（匹配边为空集的删除）
			for (int x = 1; x <= P.pvexnum; x++) {
				pattern_answer[num].patternnode[x].clear();
			}
			return false;
		}
		panINF = panINF->nextin;
		matEdgNo++;
	}

	// 再匹配起始节点的出边
	PArcNode *panF = P.pnodeList[num].fstoutArc;
	while (panF != NULL) { // 循环匹配起始节点的所有出边
		if (panF->edgeNo != matEdgNo) {
			cout << "第一个匹配节点，出边匹配出错" << endl;
			panF = panF->nextout;
			continue;
		}
		int flag2 = 0;
		// 判断是否需要进行连接（连接不上的删除，连接不上的等于匹配边为空集）
		int flag1 = 0;
		if (pattern_answer[num].patternnode[(*panF).fromNode].size() && pattern_answer[num].patternnode[(*panF).toNode].size()) {
			flag1 = 1; //进行边匹配时需要进行连接
		}
		vector<int> connected_node_list;
		flag2 = edge_match(FNode, num, panF, &connected_node_list, flag1); //正向
		// 进行完边匹配后先检查后继节点个数是否满足要求
		//if (pattern_answer[num].patternnode[pan->toNode].size() < P.pnodeList[pan->toNode].needc) {
		//	flag1 = false;
		//}
		if (flag2 < P.pnodeList[panF->toNode].needc) { // 没有得到合适的匹配边（匹配边为空集的删除）
			for (int x = 1; x <= P.pvexnum; x++) {
				pattern_answer[num].patternnode[x].clear();
			}
			return false;
		}
		// 进行一次连接后，要删除没有连接上的节点
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
	//递归匹配除前面已经匹配过的第一个节点外的剩余节点
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

	/* 匹配前先获取待匹配节点的所有组合*/
	vector<VEC> a; // 存储所有可能的组合
	VEC b; // 暂存一种组合
	//int matEdgNotmp = *matEdgNo;
	int curr_nodeNo = BrDOT[num];
	// 从上一层的匹配中间结果中获取候选节点加到本轮匹配中间结果中
	for (int i = 1; i < num; i++) {
		pattern_answer[num].patternnode[BrDOT[i]] = pattern_answer[num - 1].patternnode[BrDOT[i]];
	}
	// 如果是最后一个待匹配节点，则选择需要的匹配候选节点放入匹配中间结果中，返回
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

	// 从上一层的匹配中间结果中获取当前待匹配节点的候选节点列表
	vector<pair<int, bool>> curr_nod_list;
	curr_nod_list.push_back(pair<int,bool>(0,false));
	map<int, NODEMATCH>::reverse_iterator riter = pattern_answer[num-1].patternnode[curr_nodeNo].rbegin();
	for (; riter != pattern_answer[num-1].patternnode[curr_nodeNo].rend(); riter++) {
		curr_nod_list.push_back(pair<int, bool>(riter->first,true));
	}
	// 遍历所有的组合
	int returnFlag = comb2(matEdgNo, num, b, curr_nod_list, pattern_answer[num - 1].patternnode[curr_nodeNo].size(), pattern_answer[num - 1].patternnode[curr_nodeNo].size(), P.pnodeList[curr_nodeNo].needc);
	if (returnFlag) {
		return true;
	}
	else {
		for (int x = num; x <= P.pvexnum; x++) {
			pattern_answer[num].patternnode[BrDOT[x]].clear();
		}
		return false; // 试完所有组合后仍没获得匹配结果
	}
	///*对每种组合进行试探递归匹配*/
	//for (vector<VEC>::iterator iter = a.begin(); iter != a.end();iter++) {
	//	int flag1 = combineTesing(*iter, &matEdgNo, num);
	//	if (flag1) {
	//		// 检测是否有本轮未用到的节点的候选节点集没有加入到当前层
	//		for (int i = 1; i <= P.pvexnum; i++) {
	//			if (pattern_answer[num - 1].patternnode[i].size() > 0 && pattern_answer[num].patternnode[i].size() == 0) {
	//				pattern_answer[num].patternnode[i] = pattern_answer[num - 1].patternnode[i];
	//			}
	//		}
	//		bool rflag = TopologicalMatching4(num + 1, matEdgNo);
	//		if (rflag) return true; // 获得结果后，不再遍历寻找其他匹配结果；
	//	}
	//	else {
	//		//本节点匹配没有问题，但是下层节点匹配出现问题，继续测试下一种组合
	//		//*matEdgNo = matEdgNotmp;
	//	}
	//}
}



bool combineTesing(VEC combine, vector<pair<int, bool>> &curr_nod_list, int *matEdgNo,int candCount, int num) {

	int curr_nodeNo = BrDOT[num]; //当前待匹配模式节点
	int matEdgNoTmp = *matEdgNo;
	// 将当前组合加入候选节点集中
	for (VEC::iterator iter = combine.begin(); iter != combine.end(); iter++) {
		pattern_answer[num].patternnode[curr_nodeNo][*iter] = pattern_answer[num - 1].patternnode[curr_nodeNo][*iter];
	}
	// 获取候选节点集
	map<int, NODEMATCH> Candidate = pattern_answer[num].patternnode[curr_nodeNo];
	// 获取候选节点集的迭代器
	map<int, NODEMATCH>::iterator strmap_iter1 = Candidate.begin();
	// 循环对每一个候选节点进行模式边匹配
	for (; strmap_iter1 != Candidate.end(); strmap_iter1++) { // 对于每一个候选节点循环匹配
		*matEdgNo = matEdgNoTmp; // 当前边的匹配序号需重新赋值
		// 先匹配当前待匹配节点的入边
		PArcNode *panIN = P.pnodeList[curr_nodeNo].fstinArc;
		while (panIN != NULL) {
			if (panIN->edgeNo != *matEdgNo) {
				panIN = panIN->nextin;
				continue;
			}
			int tonode = strmap_iter1->first;
			vector<int> connected_node_list;
			// 判断是否需要进行连接（连接不上的删除，连接不上的等于匹配边为空集）
			int flag1 = 3;
			if (pattern_answer[num].patternnode[(*panIN).fromNode].size()) {	//进行边匹配时需要当前层匹配结果进行连接
				flag1 = 4; 
			}
			else if (pattern_answer[num-1].patternnode[(*panIN).fromNode].size()) {	//进行边匹配时需要与上一层匹配结果进行连接
				flag1 = 5; 
			}

			int flag2 = 0;
			/* 进行边匹配  */
			if (matchCache[(*panIN).edgeNo].find(tonode) != matchCache[(*panIN).edgeNo].end()) {
				flag2 = AddSuitableCacheNodeToAnswer(panIN, &connected_node_list, num, tonode, flag1);
			}
			else {
				flag2 = inverse_edge_match(tonode, num, panIN, &connected_node_list, flag1); //反向
			}
			// 进行当前边匹配后，计算候选节点个数是否满足要求
			if (flag2 < P.pnodeList[panIN->fromNode].needc) { // 没有得到足够数量的匹配边（删除当前及之后节点的匹配中间结果，返回）
				for (int x = num; x <= P.pvexnum; x++) {
					pattern_answer[num].patternnode[BrDOT[x]].clear();
				}
				// 判断当前待匹配节点的入边匹配是否满足个数要求
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
			// 进行一次连接后，要删除没有连接上的节点
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
		// 再匹配当前待匹配节点的出边
		PArcNode *panOut = P.pnodeList[curr_nodeNo].fstoutArc;
		while (panOut != NULL) {
			if (panOut->edgeNo != *matEdgNo) {
				panOut = panOut->nextout;
				continue;
			}
			int startnode = strmap_iter1->first;
			vector<int> connected_node_list;
			// 判断是否需要进行连接，以及怎么进行连接（连接不上的删除，连接不上的等于匹配边为空集）
			int flag1 = 0;
			int flag2 = 0;
			if (pattern_answer[num].patternnode[(*panOut).toNode].size() > 0) { // 当前待匹配边的终止节点的候选节点不空，一定要进行连接
				flag1 = 1;
			}
			//else if (P.pnodeList[(*panOut).toNode].indegree > 1 && P.pnodeList[(*panOut).toNode].firstinfrom != curr_nodeNo) { // 非第一次到达如C->D，需进行连接 (也可以用入边的连接判断方式进行判断)
			else if(pattern_answer[num-1].patternnode[(*panOut).toNode].size() > 0){
				flag1 = 2;
			}
			/* 进行边匹配  */
			if (matchCache[(*panOut).edgeNo].find(startnode) != matchCache[(*panOut).edgeNo].end()) {
				flag2 = AddSuitableCacheNodeToAnswer(panOut, &connected_node_list, num, startnode, flag1);
			}
			else {
				flag2 = edge_match(startnode, num, panOut, &connected_node_list, flag1); // 正向, 由flag2决定是否需要删除当前匹配顶点
			}
			// 进行当前边匹配后，计算候选节点个数是否满足要求
			if (flag2 < P.pnodeList[panOut->toNode].needc) { // 没有得到足够数量的匹配边（删除当前及之后节点的匹配中间结果，返回）
				for (int x = num; x <= P.pvexnum; x++) {
					pattern_answer[num].patternnode[BrDOT[x]].clear();
				}
				// 判断当前待匹配节点的入边匹配是否满足个数要求
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
			// 进行一次连接后，要删除没有连接上的节点
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
	//file << "pattern answer" << graph_sum << ":" << membership / P.parcnum << endl;  // 模糊度度量感觉不太好计算啊！

	membership = 0.0;
	// 输出匹配节点集合
	for (int x = 1; x <= P.pvexnum; x++) {
		file << "vertex" << x << ": 节点个数 " << pattern_answer[num].patternnode[x].size() << endl;
		//pattern_answer[num].patternnode[x].size();
		map<int, NODEMATCH>::iterator strmap_iter1 = pattern_answer[num].patternnode[x].begin();
		for (; strmap_iter1 != pattern_answer[num].patternnode[x].end(); strmap_iter1++) {
			int matchnode = strmap_iter1->first;
			file << G.NodeList[matchnode].nodenum << " " << G.NodeList[matchnode].nodelable << " " << G.NodeList[matchnode].factor << endl;
		}
	}

	// 输出匹配边集合
	for (int x = 1; x <= P.pvexnum; x++) {
		file << "edge_match"  << ": " << endl;
		//pattern_answer[num].patternnode[x].size();
		map<int, NODEMATCH>::iterator strmap_iter2 = pattern_answer[num].patternnode[x].begin();
		for (; strmap_iter2 != pattern_answer[num].patternnode[x].end(); strmap_iter2++) {
			int matchnode = strmap_iter2->first;
			map<int, ArcPathN>::iterator matchedge_iter1 = strmap_iter2->second.out_edge.begin();
			for (; matchedge_iter1 != strmap_iter2->second.out_edge.end(); matchedge_iter1++) { //可能包含多条后继边
				int tonode = matchedge_iter1->first;
				file << "from " << x << " to " << tonode << " matchnode " << matchnode  << endl<<endl;
				//file << "from " << x << " to " << tonode << " matchnode " << matchnode << " 边条数 " << matchedge_iter1->second.suitable.size() << endl << endl;
				map<int, SUITABLE_PATH>::iterator suitable_edge_iter1 = matchedge_iter1->second.suitable.begin();
				for (; suitable_edge_iter1 != matchedge_iter1->second.suitable.end(); suitable_edge_iter1++) { // 每条模式边可能包含多条匹配边
					// SUITABLE_PATH suitable_path_temp = suitable_edge_iter1->second;
					if (pattern_answer[P.pvexnum].patternnode[tonode].find(suitable_edge_iter1->first) == pattern_answer[P.pvexnum].patternnode[tonode].end()) {
						continue; //过滤掉不满足条件的匹配边
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
