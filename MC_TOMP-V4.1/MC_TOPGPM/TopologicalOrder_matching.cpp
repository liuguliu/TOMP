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
int WorkingSetSize = 0, PeakWorkingSetSize = 0, PagefileUsage = 0, PeakPagefileUsage = 0;

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
	//int indegreetemp[20];
	//memset(indegreetemp, 0, 20 * sizeof(int));
	for (int i = 1; i <= P.pvexnum; i++) {	// 计算模式图中每个节点的出度和入度
		PArcNode *parctemp = P.pnodeList[i].fstoutArc;
		while (parctemp != NULL) {
			P.pnodeList[i].outdegree++;
			P.pnodeList[parctemp->toNode].indegree++;
			//indegreetemp[parctemp->toNode]++;
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

void comb(vector<VEC> &a, vector<int> b, vector<int> &curr_nod_list, int m, int k, int counts) {
	for (int i = m; i >= k; i--) {
		//cout<<"curr "<<curr_nod_list[i]<<endl;
		b.push_back(curr_nod_list[i]);
		if (k>1) {
			comb(a, b, curr_nod_list, i - 1, k - 1, counts);
			b.pop_back();
		}
		else {
			counts++;
			//cout<<" j "<<b[0]<<" "<<k<<endl;
			//for (int j = 0; j<b.size(); j++) {
			//	cout << b[j] << " ";
			//}
			a.push_back(b);
			b.pop_back();
			//cout << endl;
		}
	}
}

void edge_match(int starti, PArcNode *p, vector<ArcPath> *edgelengthmatch) {//遍历数据图,对数据图中标签与模式边的起始节点相同的节点进行有届深度搜索
	// 返回的结果是 edgelengthmatch
	ArcPath pathi;
	queue<ArcPath> q;	//定义一个队列辅助广度优先搜索	
	//int *answerexist = new int[G.vexnum + 1];
	pathi.suitable_path.startnode = pathi.suitable_path.endnode = starti;
	pathi.suitable_path.path_list.push_back(starti);
	q.push(pathi);
	while (!q.empty()) {
		bool flag = false;
		ArcPath* pathj = new ArcPath();
		(*pathj) = q.front();	//获取队头元素
		q.pop();	//队头元素出队
		if ((G.NodeList[(*pathj).suitable_path.endnode].nodelable == P.pnodeList[(*p).toNode].PNodeLable) &&
			(G.NodeList[(*pathj).suitable_path.endnode].factor - P.pnodeList[(*p).toNode].factor>-0.000001)) {	//找到一个满足bounded length的路径
			for (int i = int(edgelengthmatch->size()) - 1; i >= 0; i--) { //循环遍历以查询到的边匹配列表，将新的匹配结果加入相同匹配终点的匹配结果的后面
				if ((*pathj).suitable_path.endnode == (*edgelengthmatch)[i].suitable_path.endnode) {
					if ((*edgelengthmatch)[i].nextSaEndP) {
						//(*pathj).nextSaEndP = new ArcPath();
						ArcPath *store_path = new ArcPath();
						(*store_path) = *pathj;
						(*store_path).nextSaEndP = (*edgelengthmatch)[i].nextSaEndP;
						(*edgelengthmatch)[i].nextSaEndP = store_path;
						//delete store_path;
					}
					else {
						//(*edgelengthmatch)[i].nextSaEndP = new ArcPath();
						ArcPath *store_path = new ArcPath();
						(*store_path) = *pathj;
						(*store_path).nextSaEndP = (*edgelengthmatch)[i].nextSaEndP;
						(*edgelengthmatch)[i].nextSaEndP = store_path;
						//delete store_path;
					}
					flag = true;
					break;
				}
			}
			if (!flag) {
				ArcPath store_path = *pathj;
				edgelengthmatch->push_back(store_path);
			}
		}
		if ((*pathj).path.size() < (*p).boundlength) {
			ArcNode *arctemp;
			arctemp = G.NodeList[(*pathj).suitable_path.endnode].firstoutarc; // 获取当前顶点的第一条邻接边
			while (arctemp != NULL){
				ArcPath cur_path;					
				cur_path.path = (*pathj).path;
				cur_path.path.push_back((*arctemp));

				//cur_path.startnode = pathj.startnode;
				cur_path.suitable_path.startnode = starti;
				cur_path.suitable_path.endnode = (*arctemp).adjvex;
				cur_path.suitable_path.path_list = (*pathj).suitable_path.path_list;
				cur_path.suitable_path.path_list.push_back((*arctemp).adjvex);		

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
				arctemp = (*arctemp).nextarc;
			}
			//GetMemory();
		}
		delete pathj;
	} // while
}

void inverse_edge_match(int endi, PArcNode *p, vector<ArcPath> *edgelengthmatch) {
	ArcPath pathi;
	queue<ArcPath> q;	// 定义一个队列辅助广度优先搜索
	//int *answerexist = new int[G.vexnum + 1];
	pathi.suitable_path.startnode = pathi.suitable_path.endnode = endi;
	pathi.suitable_path.path_list.push_back(endi);
	q.push(pathi);
	while (!q.empty()) {
		bool flag = false;
		ArcPath* pathj = new ArcPath();
		(*pathj) = q.front();	// 获取队头元素
		q.pop();	// 队头元素出队
		if ((G.NodeList[(*pathj).suitable_path.startnode].nodelable == P.pnodeList[(*p).fromNode].PNodeLable) 
			&& (G.NodeList[(*pathj).suitable_path.startnode].factor - P.pnodeList[(*p).fromNode].factor> -0.000001)) {	// 找到一个满足bounded length的路径
			for (int i = int(edgelengthmatch->size()) - 1; i >= 0; i--) { // 循环遍历以查询到的边匹配列表，将新的匹配结果加入相同匹配终点的匹配结果的后面
				if ((*pathj).suitable_path.startnode == (*edgelengthmatch)[i].suitable_path.startnode) { // 不同路径的同一个匹配
					if ((*edgelengthmatch)[i].nextSaEndP) {
						//(*pathj).nextSaEndP = new ArcPath();
						ArcPath *store_path = new ArcPath();
						(*store_path) = *pathj;
						(*store_path).nextSaEndP = (*edgelengthmatch)[i].nextSaEndP;
						(*edgelengthmatch)[i].nextSaEndP = store_path;
						//delete store_path;
					}
					else {
						//(*edgelengthmatch)[i].nextSaEndP = new ArcPath();
						ArcPath *store_path = new ArcPath();
						(*store_path) = *pathj;
						(*store_path).nextSaEndP = (*edgelengthmatch)[i].nextSaEndP;
						(*edgelengthmatch)[i].nextSaEndP = store_path;
						//delete store_path;
					}
					flag = true;
					break;
				}
			}
			if (!flag) {
				ArcPath store_path = *pathj;
				edgelengthmatch->push_back(store_path);
			}
		}
		if ((*pathj).path.size() < (*p).boundlength) {
			ArcNode *arctemp;
			arctemp = G.NodeList[(*pathj).suitable_path.startnode].firstinarc; // 获取当前顶点的第一条邻接边
			while (arctemp != NULL) {
				ArcPath cur_path;
				cur_path.path = (*pathj).path;
				cur_path.path.push_back((*arctemp));

				// cur_path.startnode = pathj.startnode;
				cur_path.suitable_path.startnode = (*arctemp).adjvex;
				cur_path.suitable_path.endnode = endi;
				cur_path.suitable_path.path_list = (*pathj).suitable_path.path_list;
				cur_path.suitable_path.path_list.push_front((*arctemp).adjvex);

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
				arctemp = (*arctemp).nextarc;
			}
			// GetMemory();
		}
		delete pathj;
	}// while
}

int Create_suitable_path(int startnode,int num, PArcNode *p,  vector<ArcPath> *edgelengthmatch, vector<int> *connected_node_list, int existFlag) {

	int flag = 0; // 记录当前节点产生有效匹配边数量
	float T = 0.0, R = 0.0, Rou = 0.0, minist_attr = 0.0;
	float T_temp = 1.0, R_temp = 1.0, Rou_temp = 0.0, minist_attr_temp = 0.0;
	SUITABLE_PATH_LIST nodeSuitCache;

	int fromNode = (*p).fromNode;
	int toNode = (*p).toNode;
	map<int, NODEMATCH> fromTemp = pattern_answer[num].patternnode[fromNode];
	// 命名一个存储匹配边的文件并打开它

	int count = edgelengthmatch->size();
	for (int i = 0; i<count; i++) {
		ArcPath* arcpath = NULL;
		ArcPath* arcpath_temp = &(*edgelengthmatch)[i];
		minist_attr = 0.0, minist_attr_temp = 0.0;
		do {
			T_temp = 1.0, R_temp = 1.0, Rou_temp = 0.0;
			for (vector<ArcNode>::iterator arcnode = arcpath_temp->path.begin(); arcnode != arcpath_temp->path.end(); arcnode++) {
				T_temp *= arcnode->trust;
				R_temp *= arcnode->intimacy;
				Rou_temp += G.NodeList[arcnode->adjvex].factor;
			}
			if (existFlag > 2) { // 反向边匹配
				Rou_temp = (Rou_temp + G.NodeList[arcpath_temp->suitable_path.endnode].factor) / (arcpath_temp->path.size() + 1);
			}
			else {
				Rou_temp = (Rou_temp + G.NodeList[arcpath_temp->suitable_path.startnode].factor) / (arcpath_temp->path.size() + 1);
			}

			if ((T_temp - (*p).ptrust) > -0.000001 && (R_temp - (*p).pintimacy) > -0.000001 && (Rou_temp - (*p).pfactor) > -0.000001) {
				minist_attr_temp = T_temp + R_temp + Rou_temp; // utility function f = aT+bR+cRou ,这里a=b=c=1
				if (minist_attr - minist_attr_temp < 0.000001) {
					minist_attr = minist_attr_temp;
					arcpath = arcpath_temp;
					T = T_temp;
					R = R_temp;
					Rou = Rou_temp;
				}
			}
			if ((*arcpath_temp).nextSaEndP) {
				arcpath_temp = arcpath_temp->nextSaEndP;
			}
			else
				break;
		} while (1);
		if (arcpath != NULL) {
			//arcpath->suitable_path.path_fuzzy = (T_membership + R_membership + Rou_membership) / 3;  // ?
			arcpath->suitable_path.Length = arcpath->path.size();	//最后才计算省去一些不必要的计算
			arcpath->suitable_path.t = T;
			arcpath->suitable_path.r = R;
			arcpath->suitable_path.rou = Rou;
			arcpath->suitable_path.Length = arcpath->suitable_path.path_list.size();
			//arcpath->nextoutSaEndP = NULL;
			//将匹配边加入到匹配结果中
			if (existFlag == 1) { // 为 1 本num中连接
			// 判断当前匹配边是否能连接到已有的匹配顶点上
				if (pattern_answer[num].patternnode[(*p).toNode].find((*arcpath).suitable_path.endnode) != pattern_answer[num].patternnode[(*p).toNode].end()) {
					AddSuitablePathToAnswer(p, arcpath->suitable_path,num);
					flag++;
					connected_node_list->push_back(arcpath->suitable_path.endnode);
				}
			}
			else if (existFlag == 2) { // 为 2 与num-1中连接
				map<int, NODEMATCH>::iterator iter = pattern_answer[num - 1].patternnode[(*p).toNode].find((*arcpath).suitable_path.endnode);
				if (iter != pattern_answer[num-1].patternnode[(*p).toNode].end()) {
					// 先从num-1中拿到对应的值，再将当前匹配边加入到中间结果中（由于匹配边对于结果不重要所以可以先不添加）
					pattern_answer[num].patternnode[(*p).toNode][iter->first] = iter->second;
					AddSuitablePathToAnswer(p, arcpath->suitable_path, num);
					flag++;
					connected_node_list->push_back(arcpath->suitable_path.endnode);
				}
			}
			else if (existFlag == 4) { // 为 4 与num中连接
				if (pattern_answer[num].patternnode[(*p).fromNode].find((*arcpath).suitable_path.startnode) != pattern_answer[num].patternnode[(*p).fromNode].end()) {
					// 先从num-1中拿到对应的值，再将当前匹配边加入到中间结果中（由于匹配边对于结果不重要所以可以先不添加）
					AddSuitablePathToAnswer(p, arcpath->suitable_path, num);
					flag++;
					connected_node_list->push_back(arcpath->suitable_path.startnode);
				}
			}
			else if (existFlag == 5) { // 为 5 与num-1中连接
				map<int, NODEMATCH>::iterator iter = pattern_answer[num - 1].patternnode[(*p).fromNode].find((*arcpath).suitable_path.startnode);
				if (iter != pattern_answer[num - 1].patternnode[(*p).fromNode].end()) {
					pattern_answer[num].patternnode[(*p).fromNode][iter->first] = iter->second;
					// 先从num-1中拿到对应的值，再将当前匹配边加入到中间结果中（由于匹配边对于结果不重要所以可以先不添加）
					AddSuitablePathToAnswer(p, arcpath->suitable_path, num);
					flag++;
					connected_node_list->push_back(arcpath->suitable_path.startnode);
				}
			}
			else { // existFlag = 0 or 3
				AddSuitablePathToAnswer(p, arcpath->suitable_path,num);
				flag++;
			}			
			nodeSuitCache.push_back(arcpath->suitable_path);
		}
	}
	if (matchCache[(*p).edgeNo].size() == 0) {
		matchCache[(*p).edgeNo][startnode] = nodeSuitCache;
	}
	else if (matchCache[(*p).edgeNo].find(startnode) == matchCache[(*p).edgeNo].end()) { // 其实这个不用判断，因为进来了就肯定不存在于已有cache中
		matchCache[(*p).edgeNo][startnode] = nodeSuitCache;
	}
	else {
		// 已存在于缓存中，不用重新添加
		cout << "已存在于缓存中，不用重新添加" << endl;
	}

	return flag;
}

/**********************************废弃**************************************************/
int Create_suitable_path_old(int startnode, int num, PArcNode *p, vector<ArcPath> *edgelengthmatch, vector<int> *connected_node_list, int existFlag) {

	int flag = 0; // 记录当前节点产生有效匹配边数量
	float T = 0.0, R = 0.0, Rou = 0.0, minist_attr = 0.0;
	float T_temp = 1.0, R_temp = 1.0, Rou_temp = 0.0, minist_attr_temp = 0.0;
	SUITABLE_PATH_LIST nodeSuitCache;

	int fromNode = (*p).fromNode;
	int toNode = (*p).toNode;
	map<int, NODEMATCH> fromTemp = pattern_answer[num].patternnode[fromNode];
	//命名一个存储匹配边的文件并打开它

	int count = edgelengthmatch->size();
	for (int i = 0; i<count; i++) {
		ArcPath* arcpath = NULL;
		ArcPath* arcpath_temp = &(*edgelengthmatch)[i];
		minist_attr = 0.0, minist_attr_temp = 0.0;
		do {
			T_temp = 1.0, R_temp = 1.0, Rou_temp = 0.0;
			for (vector<ArcNode>::iterator arcnode = arcpath_temp->path.begin(); arcnode != arcpath_temp->path.end(); arcnode++) {
				T_temp *= arcnode->trust;
				R_temp *= arcnode->intimacy;
				Rou_temp += G.NodeList[arcnode->adjvex].factor;
			}
			if (existFlag > 2) { // 反向边匹配
				Rou_temp = (Rou_temp + G.NodeList[arcpath_temp->suitable_path.endnode].factor) / (arcpath_temp->path.size() + 1);
			}
			else {
				Rou_temp = (Rou_temp + G.NodeList[arcpath_temp->suitable_path.startnode].factor) / (arcpath_temp->path.size() + 1);
			}

			minist_attr_temp = (T_temp - R_temp > 0.00001) ? R_temp : T_temp;
			minist_attr_temp = (minist_attr_temp - Rou_temp > 0.00001) ? Rou_temp : minist_attr_temp;
			if (minist_attr_temp - minist_attr > 0.00001) {
				minist_attr = minist_attr_temp;
				arcpath = arcpath_temp;
				T = T_temp;
				R = R_temp;
				Rou = Rou_temp;
			}
			if ((*arcpath_temp).nextSaEndP) {
				arcpath_temp = arcpath_temp->nextSaEndP;
			}
			else
				break;
		} while (1);
		if (T - (*p).ptrust >= 0.00001) {
			T_membership = 1.0;
		}
		else {
			T_membership = T / (*p).ptrust;
		}
		if (R - (*p).pintimacy >= 0.00001) {
			R_membership = 1.0;
		}
		else {
			R_membership = R / (*p).pintimacy;
		}
		if (Rou - (*p).pfactor > 0.00001) {
			Rou_membership = 1.0;
		}
		else {
			Rou_membership = Rou / (*p).pfactor;
		}
		if (T_membership - MEMBERSHIP>-0.00001 && R_membership - MEMBERSHIP>-0.00001 && Rou_membership - MEMBERSHIP>-0.00001 && arcpath != NULL) {
			arcpath->suitable_path.path_fuzzy = (T_membership + R_membership + Rou_membership) / 3;  // ?
			arcpath->suitable_path.Length = arcpath->path.size();	//最后才计算省去一些不必要的计算
			arcpath->suitable_path.t = T;
			arcpath->suitable_path.r = R;
			arcpath->suitable_path.rou = Rou;
			arcpath->suitable_path.Length = arcpath->suitable_path.path_list.size();
			//arcpath->nextoutSaEndP = NULL;
			//将匹配边加入到匹配结果中
			if (existFlag == 1) { // 为 1 本num中连接
								  // 判断当前匹配边是否能连接到已有的匹配顶点上
				if (pattern_answer[num].patternnode[(*p).toNode].find((*arcpath).suitable_path.endnode) != pattern_answer[num].patternnode[(*p).toNode].end()) {
					AddSuitablePathToAnswer(p, arcpath->suitable_path, num);
					flag++;
					connected_node_list->push_back(arcpath->suitable_path.endnode);
				}
			}
			else if (existFlag == 2) { // 为 2 与num-1中连接
				if (pattern_answer[num - 1].patternnode[(*p).toNode].find((*arcpath).suitable_path.endnode) != pattern_answer[num - 1].patternnode[(*p).toNode].end()) {
					// 先从num-1中拿到对应的值，再将当前匹配边加入到中间结果中（由于匹配边对于结果不重要所以可以先不添加）
					AddSuitablePathToAnswer(p, arcpath->suitable_path, num);
					flag++;
					connected_node_list->push_back(arcpath->suitable_path.endnode);
				}
			}
			else if (existFlag == 4) { // 为 4 与num中连接
				if (pattern_answer[num].patternnode[(*p).fromNode].find((*arcpath).suitable_path.startnode) != pattern_answer[num].patternnode[(*p).fromNode].end()) {
					// 先从num-1中拿到对应的值，再将当前匹配边加入到中间结果中（由于匹配边对于结果不重要所以可以先不添加）
					AddSuitablePathToAnswer(p, arcpath->suitable_path, num);
					flag++;
					connected_node_list->push_back(arcpath->suitable_path.startnode);
				}
			}
			else if (existFlag == 5) { // 为 5 与num-1中连接
				if (pattern_answer[num - 1].patternnode[(*p).fromNode].find((*arcpath).suitable_path.startnode) != pattern_answer[num - 1].patternnode[(*p).fromNode].end()) {
					// 先从num-1中拿到对应的值，再将当前匹配边加入到中间结果中（由于匹配边对于结果不重要所以可以先不添加）
					AddSuitablePathToAnswer(p, arcpath->suitable_path, num);
					flag++;
					connected_node_list->push_back(arcpath->suitable_path.startnode);
				}
			}
			else { // existFlag = 0 or 3
				AddSuitablePathToAnswer(p, arcpath->suitable_path, num);
				flag++;
			}
			nodeSuitCache.push_back(arcpath->suitable_path);
		}
	}
	if (matchCache[(*p).edgeNo].size() == 0) {
		matchCache[(*p).edgeNo][startnode] = nodeSuitCache;
	}
	else if (matchCache[(*p).edgeNo].find(startnode) == matchCache[(*p).edgeNo].end()) { // 其实这个不用判断，因为进来了就肯定不存在于已有cache中
		matchCache[(*p).edgeNo][startnode] = nodeSuitCache;
	}
	else {
		// 已存在于缓存中，不用重新添加
		cout << "已存在于缓存中，不用重新添加" << endl;
	}

	return flag;
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
int AddSuitableCacheNodeToAnswer(PArcNode *p, vector<int> *connected_node_list,int num, int sourcenode, int type) {
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
			AddSuitablePathToAnswer(p, suitable_path,num);
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
			map<int, NODEMATCH>* toTemp = &pattern_answer[num-1].patternnode[toNode];
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
		vector<ArcPath> edgelengthmatch;
		int flag1 = 0;
		vector<int> connected_node_list;
		inverse_edge_match(FNode, panINF, &edgelengthmatch); // 反向
		flag1 = Create_suitable_path(FNode, num, panINF, &edgelengthmatch, &connected_node_list, 3); // 最后一个参数用于指明rou值的计算方式

		if (flag1 < P.pnodeList[panINF->fromNode].needc) { // 没有得到合适的匹配边（匹配边为空集的删除）
			for (int x = 1; x <= P.pvexnum; x++) {
				pattern_answer[num].patternnode[x].clear();
			}
			ReleaseSpace(&edgelengthmatch);
			vector<ArcPath>().swap(edgelengthmatch); // 清空向量，并释放内存
			return false;
		}
		ReleaseSpace(&edgelengthmatch);
		vector<ArcPath>().swap(edgelengthmatch); // 清空向量，并释放内存
		//connected_node_list.clear();
		//vector<int>().swap(connected_node_list);
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
		vector<ArcPath> edgelengthmatch;
		int flag2 = 0;
		// 判断是否需要进行连接（连接不上的删除，连接不上的等于匹配边为空集）
		int flag1 = 0;
		if (pattern_answer[num].patternnode[(*panF).fromNode].size() && pattern_answer[num].patternnode[(*panF).toNode].size()) {
			flag1 = 1; //进行边匹配时需要进行连接
		}
		vector<int> connected_node_list;
		edge_match(FNode, panF, &edgelengthmatch); //正向
		flag2 = Create_suitable_path(FNode,num, panF, &edgelengthmatch, &connected_node_list, flag1); //最后一个参数用于判断是否需要进行节点连接
		// 进行完边匹配后先检查后继节点个数是否满足要求
		if (flag2 < P.pnodeList[panF->toNode].needc) { // 没有得到合适的匹配边（匹配边为空集的删除）
			for (int x = 1; x <= P.pvexnum; x++) {
				pattern_answer[num].patternnode[x].clear();
			}
			ReleaseSpace(&edgelengthmatch);
			vector<ArcPath>().swap(edgelengthmatch); // 清空向量，并释放内存
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
		ReleaseSpace(&edgelengthmatch);
		vector<ArcPath>().swap(edgelengthmatch); // 清空向量，并释放内存
		//connected_node_list.clear();
		//vector<int>().swap(connected_node_list);
		panF = panF->nextout;
		matEdgNo++;
	}
	//递归匹配除前面已经匹配过的第一个节点外的剩余节点
	bool rflag = TopologicalMatching4(num + 1, &matEdgNo);
	if (rflag)return true;
	else {
		for (int x = 1; x <= P.pvexnum; x++) {
			pattern_answer[num].patternnode[x].clear();
		}
		return false;
	}
}


bool TopologicalMatching4(int num, int *matEdgNo){

	/* 匹配前先获取待匹配节点的所有组合*/
	vector<VEC> a; // 存储所有可能的组合
	VEC b; // 暂存一种组合
	int matEdgNotmp = *matEdgNo;
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
	vector<int> curr_nod_list;
	curr_nod_list.push_back(0);
	map<int, NODEMATCH>::reverse_iterator riter = pattern_answer[num-1].patternnode[curr_nodeNo].rbegin();
	for (; riter != pattern_answer[num-1].patternnode[curr_nodeNo].rend(); riter++) {
		curr_nod_list.push_back(riter->first);
	}
	// 获得所有组合	
	comb(a, b, curr_nod_list, pattern_answer[num - 1].patternnode[curr_nodeNo].size(), P.pnodeList[curr_nodeNo].needc, 0);
	//GetMemory();
	/*对每种组合进行试探递归匹配*/
	for (vector<VEC>::iterator iter = a.begin(); iter != a.end();iter++) {
		int flag1 = combineTesing(*iter, matEdgNo, num);
		if (flag1) {
			// 检测是否有本轮未用到的节点的候选节点集没有加入到当前层
			for (int i = 1; i <= P.pvexnum; i++) {
				if (pattern_answer[num - 1].patternnode[i].size() > 0 && pattern_answer[num].patternnode[i].size() == 0) {
					pattern_answer[num].patternnode[i] = pattern_answer[num - 1].patternnode[i];
				}
			}
			bool rflag = TopologicalMatching4(num + 1, matEdgNo);
			if (rflag) {
 			//	a.clear();
				//a.swap(vector<VEC>(a));
				return true; // 获得结果后，不再遍历寻找其他匹配结果；
			}
			else { //将组合拿出来，重新放
				for (int x = num; x <= P.pvexnum; x++) {
					pattern_answer[num].patternnode[BrDOT[x]].clear();
				}
				*matEdgNo = matEdgNotmp;
			}
		}
		else {
			//本节点匹配没有问题，但是下层节点匹配出现问题，继续测试下一种组合
			for (int x = num; x <= P.pvexnum; x++) {
				pattern_answer[num].patternnode[BrDOT[x]].clear();
			}
			*matEdgNo = matEdgNotmp;
		}
	}
	//a.clear();
	//a.swap(vector<VEC>(a));
	for (int x = num; x <= P.pvexnum; x++) {
		pattern_answer[num].patternnode[BrDOT[x]].clear();
	}
	return false; // 试完所有组合后仍没获得匹配结果
}



bool combineTesing(VEC combine,int *matEdgNo, int num) {

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
			vector<ArcPath> edgelengthmatch;
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
				inverse_edge_match(tonode, panIN, &edgelengthmatch); //反向
				flag2 = Create_suitable_path(tonode, num, panIN, &edgelengthmatch, &connected_node_list, flag1); //最后一个参数用于指明rou值的计算方式
			}
			// 进行当前边匹配后，计算候选节点个数是否满足要求
			if (flag2 < P.pnodeList[panIN->fromNode].needc) { // 没有得到足够数量的匹配边（删除当前及之后节点的匹配中间结果，返回）
				for (int x = num; x <= P.pvexnum; x++) {
					pattern_answer[num].patternnode[BrDOT[x]].clear();
				}
				ReleaseSpace(&edgelengthmatch);
				vector<ArcPath>().swap(edgelengthmatch); // 清空向量，并释放内存
				return false;
			}
			// 进行一次连接后，要删除没有连接上的节点
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
				//map<int, NODEMATCH>::reverse_iterator riter = pattern_answer[num].patternnode[panIN->fromNode].rbegin();
				//for (; riter != pattern_answer[num].patternnode[panIN->fromNode].rend(); riter++) {
				//	if (find(connected_node_list.begin(), connected_node_list.end(), riter->first) == connected_node_list.end()) {
				//		pattern_answer[num].patternnode[panIN->fromNode].erase(riter->first);
				//	}
				//}
			}
			/*******清空向量，并释放内存**********/
			ReleaseSpace(&edgelengthmatch);
			vector<ArcPath>().swap(edgelengthmatch);
			//connected_node_list.clear();
			//vector<int>().swap(connected_node_list);
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
			vector<ArcPath> edgelengthmatch;
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
				edge_match(startnode, panOut, &edgelengthmatch); // 正向, 连接放在 Create_suitable_path 函数中进行				
				flag2 = Create_suitable_path(startnode, num, panOut, &edgelengthmatch, &connected_node_list, flag1); // 由flag2决定是否需要删除当前匹配顶点
			}
			// 进行当前边匹配后，计算候选节点个数是否满足要求
			if (flag2 < P.pnodeList[panOut->toNode].needc) { // 没有得到足够数量的匹配边（删除当前及之后节点的匹配中间结果，返回）
				for (int x = num; x <= P.pvexnum; x++) {
					pattern_answer[num].patternnode[BrDOT[x]].clear();
				}
				ReleaseSpace(&edgelengthmatch);
				vector<ArcPath>().swap(edgelengthmatch); // 清空向量，并释放内存
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
				//map<int, NODEMATCH>::reverse_iterator riter = pattern_answer[num].patternnode[panOut->toNode].rbegin();
				//for (; riter != pattern_answer[num].patternnode[panOut->toNode].rend(); riter++) {
				//	if (find(connected_node_list.begin(), connected_node_list.end(), riter->first) == connected_node_list.end()) {
				//		pattern_answer[num].patternnode[panOut->toNode].erase(riter->first);
				//	}
				//}
			}
			ReleaseSpace(&edgelengthmatch);
			vector<ArcPath>().swap(edgelengthmatch); // 清空向量，并释放内存
			//connected_node_list.clear();
			//vector<int>().swap(connected_node_list);
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

void GetMemory()
{
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	if (pmc.PeakWorkingSetSize > PeakWorkingSetSize) {
		PeakWorkingSetSize = pmc.PeakWorkingSetSize;
	}
}
void OutputMemory(char* filename)
{

	//HANDLE handle = GetCurrentProcess();
	//PROCESS_MEMORY_COUNTERS pmc;
	//GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	ofstream f(filename, ios::app);
	if (!f.is_open()) {
		cout << "Message record file failed to open!" << '\n';
		return;
	}
	else {
		f << "memory usage:" << WorkingSetSize / 1000 << "K/" << PeakWorkingSetSize / 1000 << "K + " << PagefileUsage / 1000 << "K/" << PeakPagefileUsage / 1000 << "K" << '\n';
		f << '\n';
	}
	f.close();
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
			<< datasetname << "," << pmc.PeakWorkingSetSize / 1000 << "KB" ;
		f << '\n';
	}
	f.close();
}



void ReleaseSpace(vector<ArcPath> *edgelengthmatch) {  //释放动态申请的存储空间，但是好像没有什么效果
	vector<ArcPath>::iterator e;
	for (e = (*edgelengthmatch).begin(); e != (*edgelengthmatch).end(); e++) {
		ArcPath* p = e->nextSaEndP;
		while (p != NULL) {
			ArcPath* p_tmp = p->nextSaEndP;
			//cout << p << endl;
			delete p;
			p = p_tmp;
		}		
	}
}
