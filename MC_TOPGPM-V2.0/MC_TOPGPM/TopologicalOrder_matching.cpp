#include<iostream>
#include<vector>
#include<queue>
#include<list>
#include<stack>
#include <time.h> 
#include<string.h>
#include<fstream>
#include"TopologicalOrder_matching.h"
// 获取系统内存使用情况
#include<Windows.h>
#include<Psapi.h>
using namespace std;

//#pragma comment(lib,"dhcpsapi.lib")




//定义全局变量
int indegree[MAX][2];
ArcPathList arcpathlist[MAX];	//存储边匹配的结果
vector<PArcNode> TopoOrderPatternEdge;
PATTERN_ANSWER pattern_answer_temp;
vector<PATTERN_ANSWER> pattern_answer;
list<ExperimentResult> resultList;
list<ExperimentDetailedResult> detailedResultList;
float T_membership = 0.0, R_membership = 0.0, Rou_membership = 0.0;
int graph_sum = 0;
double this_start_time, this_finish_time; /* 每组结果的开始时间，结束时间 */

void TopologicalTraverse() {
	queue<int> S;
	int indegreetemp[20];
	memset(indegree, 0, sizeof(int));
	memset(indegreetemp, 0, 20*sizeof(int));
	for (int i = 1; i <= P.pvexnum; i++) {	// 计算indegreetemp数组
		PArcNode *parctemp = P.pnodeList[i].firstArc;
		while (parctemp != NULL) {		
			indegreetemp[parctemp->toNode]++;
			parctemp = parctemp->next;
		}
	}
	for (int i = 1; i <= P.pvexnum; i++) {
		if (!indegreetemp[i]) {
			S.push(i);	//入度为零的点进栈
		}
	}
	while (!S.empty()) {
		int nodeindex = S.front();
		S.pop();
		PArcNode *parctemp = P.pnodeList[nodeindex].firstArc;
		while (parctemp != NULL) {
			TopoOrderPatternEdge.push_back(*parctemp);
			indegreetemp[(*parctemp).toNode]--;
			if (indegree[parctemp->toNode][0] == 0) {
				indegree[parctemp->toNode][1] = parctemp->fromNode;
			}
			indegree[parctemp->toNode][0]++;
			if (!(indegreetemp[(*parctemp).toNode])) {
				S.push((*parctemp).toNode);
			}
			parctemp = parctemp->next;
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
		if ((G.NodeList[(*pathj).suitable_path.endnode].nodelable == P.pnodeList[(*p).toNode].PNodeLable) 
			&& (G.NodeList[(*pathj).suitable_path.endnode].factor - P.pnodeList[(*p).toNode].factor> -0.000001)) {	//找到一个满足bounded length的路径
			for (int i = edgelengthmatch->size() - 1; i >= 0; i--) { //循环遍历以查询到的边匹配列表，将新的匹配结果加入相同匹配终点的匹配结果的后面
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
			arctemp = G.NodeList[(*pathj).suitable_path.endnode].firstarc; // 获取当前顶点的第一条邻接边
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
		}
		delete pathj;
	}//while
}

void Create_suitable_path(PArcNode *p, int num, vector<ArcPath> *edgelengthmatch) {

	float T = 0.0, R = 0.0, Rou = 0.0,minist_attr=0.0;
	float T_temp = 1.0, R_temp = 1.0, Rou_temp = 0.0, minist_attr_temp = 0.0;
	
	arcpathlist[num].fromnode = (*p).fromNode;
	arcpathlist[num].tonode = (*p).toNode;

	int count = edgelengthmatch->size();
	for(int i=0;i<count;i++){
		ArcPath* arcpath=NULL;
		ArcPath* arcpath_temp = &(*edgelengthmatch)[i];
		minist_attr = 0.0,minist_attr_temp = 0.0;
		do {
			T_temp = 1.0, R_temp = 1.0, Rou_temp = 0.0;
			for (vector<ArcNode>::iterator arcnode = arcpath_temp->path.begin(); arcnode != arcpath_temp->path.end(); arcnode++) {
				T_temp *= arcnode->trust;
				R_temp *= arcnode->intimacy;
				Rou_temp += G.NodeList[arcnode->adjvex].factor;
			}
			Rou_temp = (Rou_temp + G.NodeList[arcpath_temp->suitable_path.startnode].factor) / (arcpath_temp->path.size() + 1);
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
		}while (1);
		if ((T - (*p).ptrust) > -0.000001 && (R - (*p).pintimacy) > -0.000001 && (Rou - (*p).pfactor) > -0.000001  && arcpath != NULL) {
		//if (T_membership >=MEMBERSHIP && R_membership >= MEMBERSHIP && Rou_membership >= MEMBERSHIP && arcpath!=NULL) {
			arcpath->suitable_path.path_fuzzy = 0.0;  // ?
			//arcpath->suitable_path.path_fuzzy = (T_membership + R_membership + Rou_membership) / 3;  // ?
			arcpath->suitable_path.Length = arcpath->path.size();	//最后才计算省去一些不必要的计算
			arcpath->suitable_path.t = T;
			arcpath->suitable_path.r = R;
			arcpath->suitable_path.rou = Rou;
			//arcpath->nextSaEndP = NULL;
			arcpathlist[num].pathset.push_back(arcpath->suitable_path);
		}
	}
}

bool TopoligicalMatching2(int startnode, int num,char* storefile, char* infofile) {
	vector<ArcPath> edgelengthmatch;

	edge_match(startnode, &TopoOrderPatternEdge[num], &edgelengthmatch);
	Create_suitable_path(&TopoOrderPatternEdge[num], num, &edgelengthmatch);	//构造满足多约束的匹配边arcpathlist[num]
	ReleaseSpace(&edgelengthmatch);
	vector<ArcPath>().swap(edgelengthmatch); // 清空向量，并释放内存
	if (arcpathlist[num].pathset.size()) {			// 如果存在满足条件的匹配边,(匹配下一条边或节点连接)
		int conswap = 0;			// 先判断是否需要子图连接，例如1->2, 3->2; 2->4, 3->4.若需要进行连接则先进行连接，确定到达顶点的匹配
		if (indegree[arcpathlist[num].tonode][0] > 1 && indegree[arcpathlist[num].tonode][1] != arcpathlist[num].fromnode) {	// 达到节点的入度大于1，且当前边不是第一个到达该节点的边		
			for (int cur = 0; cur < arcpathlist[num].pathset.size(); cur++) {
				int tonode = arcpathlist[num].tonode;
				int commonindex[MAX];	// 记录每条参与连接的模式边的匹配边的位置，这里没有对该数组进行清理，因为每一次连接都会更新对应的值。
				int count = 0;
				int count1 = 0; // 记录到达tonode节点的边的个数
				for (int common = 0; common < num; common++) { // 这样写是考虑到有多条边指向同一个节点
					if (TopoOrderPatternEdge[common].toNode == tonode) {
						count1++;
						for (int k = 0; k < arcpathlist[common].pathset.size(); k++) {
							if (arcpathlist[common].pathset[k].endnode == arcpathlist[num].pathset[cur].endnode) {
								count++;
								commonindex[common] = k;
								break;
							}
						}
					}
				}
				if (count == count1) { // 每条指向该类顶点的模式边，都有一个匹配
					swap(arcpathlist[num].pathset[conswap], arcpathlist[num].pathset[cur]);
					for (int common = 0; common < num; common++) {
						if (TopoOrderPatternEdge[common].toNode == tonode) {
							swap(arcpathlist[common].pathset[conswap], arcpathlist[common].pathset[commonindex[common]]);
						}
					}
					conswap++; //记录连接的边个数
				}
			}

			if (!conswap) { // 进行连接后，无满足条件的节点。
				arcpathlist[num].curindex = 0;
				arcpathlist[num].pathset.clear();
				return false;	// 如果没有满足条件的匹配边,(回溯替换，回溯到第一条边则结束)
			}
			else {
				for (int common = 0; common < num; common++) {
					if (TopoOrderPatternEdge[common].toNode == TopoOrderPatternEdge[num].toNode) {
						arcpathlist[common].curindex = 0;
						arcpathlist[common].usefulcur = conswap;
					}
				}
				arcpathlist[num].curindex = 0;
			}
			arcpathlist[num].usefulcur = conswap;
		}
		else {	//不需要进行连接
			arcpathlist[num].curindex = 0;
			arcpathlist[num].usefulcur = arcpathlist[num].pathset.size();
		}
		//递归进入下一层
		while (arcpathlist[num].curindex < arcpathlist[num].usefulcur) {
			int curindex = arcpathlist[num].curindex;
			//for (int i = 0; i < num; i++) {
			//	if (pattern_answer_temp.answer_edgelist[i].startnode == arcpathlist[i].pathset[curindex].startnode && 
			//		pattern_answer_temp.answer_edgelist[i].endnode == arcpathlist[i].pathset[curindex].endnode) {
			//		if (arcpathlist[num].curindex + 1 < arcpathlist[num].usefulcur) {
			//			curindex += 1;
			//			break;
			//		}				
			//	}
			//}
			if (indegree[arcpathlist[num].tonode][0] > 1 && indegree[arcpathlist[num].tonode][1] != arcpathlist[num].fromnode) {	//如果不是第一个到达该节点的边C->D
				for (int i = num - 1; i >= 0; i--) {
					if (TopoOrderPatternEdge[i].toNode == TopoOrderPatternEdge[num].toNode) {
						arcpathlist[i].curindex = curindex;
						pattern_answer_temp.answer_edgelist[i] = arcpathlist[i].pathset[curindex];	//更改原匹配边B->D（第一次执行时可能有问题，逻辑尚未理清）
						//pattern_answer_temp.vnode[arcpathlist[i].fromnode] = arcpathlist[i].pathset[curindex].startnode;	//可有可无
						//pattern_answer_temp.vnode[arcpathlist[i].tonode] = arcpathlist[i].pathset[curindex].endnode;
					}
				}
				pattern_answer_temp.answer_edgelist.push_back(arcpathlist[num].pathset[curindex]);
				pattern_answer_temp.vnode[arcpathlist[num].fromnode] = arcpathlist[num].pathset[curindex].startnode;
				pattern_answer_temp.vnode[arcpathlist[num].tonode] = arcpathlist[num].pathset[curindex].endnode;
				if (num < P.parcnum - 1) {
					int startid = pattern_answer_temp.vnode[TopoOrderPatternEdge[num + 1].fromNode];  
					if (startid != -1) {
						bool rflag = TopoligicalMatching2(startid, num + 1, storefile, infofile);  //递归匹配下一条边
						if (rflag) {
							return true;
						}
					}
					else { // 如果下一条要匹配的边的起始顶点无匹配顶点时，重新查找该节点的所有候选节点再循环进行下一条边的匹配（可用逆邻接表优化）
						list<int> nodeCandidate = GetNodeCandidate(TopoOrderPatternEdge[num + 1].fromNode);
						list<int>::iterator p;
						for (p = nodeCandidate.begin(); p != nodeCandidate.end(); p++) {
							pattern_answer_temp.vnode[TopoOrderPatternEdge[num + 1].fromNode] = *p;
							bool rflag = TopoligicalMatching2(*p, num + 1, storefile, infofile);  //递归匹配下一条边
							if (rflag) {
								return true;
							}
						}
					}
				}
				else {					
					vector<int> nodelist;
					bool correctResultFlag = true;
					for (int i = 1; i <= P.pvexnum; i++) {
						vector<int>::iterator it = find(nodelist.begin(), nodelist.end(), pattern_answer_temp.vnode[i]);
						if (it != nodelist.end()) {
							correctResultFlag = false;
							break;
						}
						else {
							nodelist.push_back(pattern_answer_temp.vnode[i]);
						}
					}
					if (correctResultFlag) {
						pattern_answer.push_back(pattern_answer_temp);
						graph_sum++;
						return true;
						//存储到文件
						if (graph_sum % 10 == 0)
						{
							this_finish_time = (double)clock();
							Answer_Store2(storefile, graph_sum);
							//记录内存使用情况
							GetMemoryAndTime(infofile, this_start_time, this_finish_time, graph_sum, "Returns 10 results ");
							pattern_answer.clear();
						}
					}

				}
				pattern_answer_temp.answer_edgelist.pop_back();
			}
			else {
				pattern_answer_temp.answer_edgelist.push_back(arcpathlist[num].pathset[curindex]);
				pattern_answer_temp.vnode[arcpathlist[num].fromnode] = arcpathlist[num].pathset[curindex].startnode;
				pattern_answer_temp.vnode[arcpathlist[num].tonode] = arcpathlist[num].pathset[curindex].endnode;
				if (num < P.parcnum - 1) {
					int startid = pattern_answer_temp.vnode[TopoOrderPatternEdge[num + 1].fromNode];
					if (startid != -1) {
						bool rflag = TopoligicalMatching2(startid, num + 1, storefile, infofile);  //递归匹配下一条边
						if (rflag) {
							return true;
						}
					}
					else { // 如果下一条要匹配的边的起始顶点无匹配顶点时，重新查找该节点的所有候选节点再循环进行下一条边的匹配（可用逆邻接表优化）
						list<int> nodeCandidate = GetNodeCandidate(TopoOrderPatternEdge[num + 1].fromNode);
						list<int>::iterator p;
						for (p = nodeCandidate.begin(); p != nodeCandidate.end(); p++) {
							pattern_answer_temp.vnode[TopoOrderPatternEdge[num + 1].fromNode] = *p;
							bool rflag = TopoligicalMatching2(*p, num + 1, storefile, infofile);  //递归匹配下一条边
							if (rflag) {
								return true;
							}
						}
					}
				}
				else {
					pattern_answer.push_back(pattern_answer_temp);	//这个在本实验中不可能执行
					graph_sum++;
					return true;
					//存储到文件
					if (graph_sum % 10 == 0)
					{
						this_finish_time = (double)clock();
						Answer_Store2(storefile, graph_sum);
						//记录内存使用情况
						GetMemoryAndTime(infofile,this_start_time, this_finish_time, graph_sum, "Returns 10 results ");
						pattern_answer.clear();
					}
				}
				pattern_answer_temp.answer_edgelist.pop_back();
			}
			arcpathlist[num].curindex = arcpathlist[num].curindex + 1;
		} // while
		 // 返回上一条边进行匹配(回溯替换，回溯到第一条边则结束)
		arcpathlist[num].curindex = 0;
		arcpathlist[num].pathset.clear();

		return false;
	}
	else {	//如果没有满足条件的匹配边,(回溯替换，回溯到第一条边则结束)
		return false;
	}
}

list<int> GetNodeCandidate(int nodeID) {
	list<int> nodeCandidate;
	PNode node = P.pnodeList[nodeID];
	for (int i = 0; i < G.vexnum; i++) {
		if (G.NodeList[i].nodelable == node.PNodeLable && (G.NodeList[i].factor/node.factor> MEMBERSHIP)) {
			nodeCandidate.push_back(i);
		}
	}
	return nodeCandidate;
}

void Answer_Store(char* filename,int graph_sum) {
	ofstream file(filename, ios::app);
	vector<PATTERN_ANSWER>::iterator p;
	int i = 1;
	int j = 1;
	int slength = pattern_answer.size();
	double membership = 0.0;

	for (p = pattern_answer.begin(); p != pattern_answer.end(); p++) {

		j = 1;
		membership = 0.0;
		vector<SUITABLE_PATH>::iterator e;
		for (e = (*p).answer_edgelist.begin(); e != (*p).answer_edgelist.end(); e++) {
			file << "edge" << j << ": ";
			membership += (*e).path_fuzzy;
			file << (*e).startnode << " " << (*e).endnode << " " << (*e).path_list.size() - 1 << "  " << (*e).t << "  " << (*e).r << "  " << (*e).rou << "  " << (*e).path_fuzzy << endl;
			list<int>::iterator path;
			for (path = (*e).path_list.begin(); path != (*e).path_list.end(); path++) {
				file << (*path) << " ";
			}
			file << endl;
			j++;
		}
		file << "pattern answer" << graph_sum-slength +i << ":" << membership / P.parcnum << endl;
		i++;
	}
}

void Answer_Store2(char* filename, int graph_sum) {
	ofstream file(filename, ios::app);
	vector<PATTERN_ANSWER>::iterator p;
	int i = 1;
	int j = 1;
	int slength = pattern_answer.size();
	double membership = 0.0;

	for (p = pattern_answer.begin(); p != pattern_answer.end(); p++) {

		j = 1;
		membership = 0.0;
		for (int x = 1; x <= P.pvexnum; x++) {
			file << "vertex" << x << ": ";
			file << G.NodeList[(*p).vnode[x]].nodenum << " " << G.NodeList[(*p).vnode[x]].nodelable<< " " << G.NodeList[(*p).vnode[x]].factor<<endl;
		}
		vector<SUITABLE_PATH>::iterator e;
		for (e = (*p).answer_edgelist.begin(); e != (*p).answer_edgelist.end(); e++) {
			file << "edge" << j << ": ";
			membership += (*e).path_fuzzy;
			file << G.NodeList[(*e).startnode].nodenum << " " << G.NodeList[(*e).endnode].nodenum << " " << (*e).path_list.size() - 1 << "  " << (*e).t << "  " << (*e).r << "  " << (*e).rou << "  " << (*e).path_fuzzy << endl;
			list<int>::iterator path;
			for (path = (*e).path_list.begin(); path != (*e).path_list.end(); path++) {
				file << G.NodeList[(*path)].nodenum << " ";
			}
			file << endl;
			j++;
		}
		file << "pattern answer" << graph_sum - slength + i << ":" << membership / P.parcnum << endl;
		i++;
	}
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
