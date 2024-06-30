#include<iostream>
#include<vector>
#include<queue>
#include<list>
#include<stack>
#include <time.h> 
#include<string.h>
#include<fstream>
#include"TopologicalOrder_matching.h"
// ��ȡϵͳ�ڴ�ʹ�����
#include<Windows.h>
#include<Psapi.h>
using namespace std;

//#pragma comment(lib,"dhcpsapi.lib")




//����ȫ�ֱ���
int indegree[MAX][2];
ArcPathList arcpathlist[MAX];	//�洢��ƥ��Ľ��
vector<PArcNode> TopoOrderPatternEdge;
PATTERN_ANSWER pattern_answer_temp;
vector<PATTERN_ANSWER> pattern_answer;
list<ExperimentResult> resultList;
list<ExperimentDetailedResult> detailedResultList;
float T_membership = 0.0, R_membership = 0.0, Rou_membership = 0.0;
int graph_sum = 0;
double this_start_time, this_finish_time; /* ÿ�����Ŀ�ʼʱ�䣬����ʱ�� */

void TopologicalTraverse() {
	queue<int> S;
	int indegreetemp[20];
	memset(indegree, 0, sizeof(int));
	memset(indegreetemp, 0, 20*sizeof(int));
	for (int i = 1; i <= P.pvexnum; i++) {	// ����indegreetemp����
		PArcNode *parctemp = P.pnodeList[i].firstArc;
		while (parctemp != NULL) {		
			indegreetemp[parctemp->toNode]++;
			parctemp = parctemp->next;
		}
	}
	for (int i = 1; i <= P.pvexnum; i++) {
		if (!indegreetemp[i]) {
			S.push(i);	//���Ϊ��ĵ��ջ
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

void edge_match(int starti, PArcNode *p, vector<ArcPath> *edgelengthmatch) {//��������ͼ,������ͼ�б�ǩ��ģʽ�ߵ���ʼ�ڵ���ͬ�Ľڵ�����н��������
	// ���صĽ���� edgelengthmatch
	ArcPath pathi;
	queue<ArcPath> q;	//����һ�����и��������������	
	//int *answerexist = new int[G.vexnum + 1];
	pathi.suitable_path.startnode = pathi.suitable_path.endnode = starti;
	pathi.suitable_path.path_list.push_back(starti);
	q.push(pathi);
	while (!q.empty()) {
		bool flag = false;
		ArcPath* pathj = new ArcPath();
		(*pathj) = q.front();	//��ȡ��ͷԪ��
		q.pop();	//��ͷԪ�س���
		if ((G.NodeList[(*pathj).suitable_path.endnode].nodelable == P.pnodeList[(*p).toNode].PNodeLable) 
			&& (G.NodeList[(*pathj).suitable_path.endnode].factor - P.pnodeList[(*p).toNode].factor> -0.000001)) {	//�ҵ�һ������bounded length��·��
			for (int i = edgelengthmatch->size() - 1; i >= 0; i--) { //ѭ�������Բ�ѯ���ı�ƥ���б����µ�ƥ����������ͬƥ���յ��ƥ�����ĺ���
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
			arctemp = G.NodeList[(*pathj).suitable_path.endnode].firstarc; // ��ȡ��ǰ����ĵ�һ���ڽӱ�
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
						// �ﵽԼ��·�����ȣ����յ㲻����Ҫ��
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
			arcpath->suitable_path.Length = arcpath->path.size();	//���ż���ʡȥһЩ����Ҫ�ļ���
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
	Create_suitable_path(&TopoOrderPatternEdge[num], num, &edgelengthmatch);	//���������Լ����ƥ���arcpathlist[num]
	ReleaseSpace(&edgelengthmatch);
	vector<ArcPath>().swap(edgelengthmatch); // ������������ͷ��ڴ�
	if (arcpathlist[num].pathset.size()) {			// �����������������ƥ���,(ƥ����һ���߻�ڵ�����)
		int conswap = 0;			// ���ж��Ƿ���Ҫ��ͼ���ӣ�����1->2, 3->2; 2->4, 3->4.����Ҫ�����������Ƚ������ӣ�ȷ�����ﶥ���ƥ��
		if (indegree[arcpathlist[num].tonode][0] > 1 && indegree[arcpathlist[num].tonode][1] != arcpathlist[num].fromnode) {	// �ﵽ�ڵ����ȴ���1���ҵ�ǰ�߲��ǵ�һ������ýڵ�ı�		
			for (int cur = 0; cur < arcpathlist[num].pathset.size(); cur++) {
				int tonode = arcpathlist[num].tonode;
				int commonindex[MAX];	// ��¼ÿ���������ӵ�ģʽ�ߵ�ƥ��ߵ�λ�ã�����û�жԸ��������������Ϊÿһ�����Ӷ�����¶�Ӧ��ֵ��
				int count = 0;
				int count1 = 0; // ��¼����tonode�ڵ�ıߵĸ���
				for (int common = 0; common < num; common++) { // ����д�ǿ��ǵ��ж�����ָ��ͬһ���ڵ�
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
				if (count == count1) { // ÿ��ָ����ඥ���ģʽ�ߣ�����һ��ƥ��
					swap(arcpathlist[num].pathset[conswap], arcpathlist[num].pathset[cur]);
					for (int common = 0; common < num; common++) {
						if (TopoOrderPatternEdge[common].toNode == tonode) {
							swap(arcpathlist[common].pathset[conswap], arcpathlist[common].pathset[commonindex[common]]);
						}
					}
					conswap++; //��¼���ӵı߸���
				}
			}

			if (!conswap) { // �������Ӻ������������Ľڵ㡣
				arcpathlist[num].curindex = 0;
				arcpathlist[num].pathset.clear();
				return false;	// ���û������������ƥ���,(�����滻�����ݵ���һ���������)
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
		else {	//����Ҫ��������
			arcpathlist[num].curindex = 0;
			arcpathlist[num].usefulcur = arcpathlist[num].pathset.size();
		}
		//�ݹ������һ��
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
			if (indegree[arcpathlist[num].tonode][0] > 1 && indegree[arcpathlist[num].tonode][1] != arcpathlist[num].fromnode) {	//������ǵ�һ������ýڵ�ı�C->D
				for (int i = num - 1; i >= 0; i--) {
					if (TopoOrderPatternEdge[i].toNode == TopoOrderPatternEdge[num].toNode) {
						arcpathlist[i].curindex = curindex;
						pattern_answer_temp.answer_edgelist[i] = arcpathlist[i].pathset[curindex];	//����ԭƥ���B->D����һ��ִ��ʱ���������⣬�߼���δ���壩
						//pattern_answer_temp.vnode[arcpathlist[i].fromnode] = arcpathlist[i].pathset[curindex].startnode;	//���п���
						//pattern_answer_temp.vnode[arcpathlist[i].tonode] = arcpathlist[i].pathset[curindex].endnode;
					}
				}
				pattern_answer_temp.answer_edgelist.push_back(arcpathlist[num].pathset[curindex]);
				pattern_answer_temp.vnode[arcpathlist[num].fromnode] = arcpathlist[num].pathset[curindex].startnode;
				pattern_answer_temp.vnode[arcpathlist[num].tonode] = arcpathlist[num].pathset[curindex].endnode;
				if (num < P.parcnum - 1) {
					int startid = pattern_answer_temp.vnode[TopoOrderPatternEdge[num + 1].fromNode];  
					if (startid != -1) {
						bool rflag = TopoligicalMatching2(startid, num + 1, storefile, infofile);  //�ݹ�ƥ����һ����
						if (rflag) {
							return true;
						}
					}
					else { // �����һ��Ҫƥ��ıߵ���ʼ������ƥ�䶥��ʱ�����²��Ҹýڵ�����к�ѡ�ڵ���ѭ��������һ���ߵ�ƥ�䣨�������ڽӱ��Ż���
						list<int> nodeCandidate = GetNodeCandidate(TopoOrderPatternEdge[num + 1].fromNode);
						list<int>::iterator p;
						for (p = nodeCandidate.begin(); p != nodeCandidate.end(); p++) {
							pattern_answer_temp.vnode[TopoOrderPatternEdge[num + 1].fromNode] = *p;
							bool rflag = TopoligicalMatching2(*p, num + 1, storefile, infofile);  //�ݹ�ƥ����һ����
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
						//�洢���ļ�
						if (graph_sum % 10 == 0)
						{
							this_finish_time = (double)clock();
							Answer_Store2(storefile, graph_sum);
							//��¼�ڴ�ʹ�����
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
						bool rflag = TopoligicalMatching2(startid, num + 1, storefile, infofile);  //�ݹ�ƥ����һ����
						if (rflag) {
							return true;
						}
					}
					else { // �����һ��Ҫƥ��ıߵ���ʼ������ƥ�䶥��ʱ�����²��Ҹýڵ�����к�ѡ�ڵ���ѭ��������һ���ߵ�ƥ�䣨�������ڽӱ��Ż���
						list<int> nodeCandidate = GetNodeCandidate(TopoOrderPatternEdge[num + 1].fromNode);
						list<int>::iterator p;
						for (p = nodeCandidate.begin(); p != nodeCandidate.end(); p++) {
							pattern_answer_temp.vnode[TopoOrderPatternEdge[num + 1].fromNode] = *p;
							bool rflag = TopoligicalMatching2(*p, num + 1, storefile, infofile);  //�ݹ�ƥ����һ����
							if (rflag) {
								return true;
							}
						}
					}
				}
				else {
					pattern_answer.push_back(pattern_answer_temp);	//����ڱ�ʵ���в�����ִ��
					graph_sum++;
					return true;
					//�洢���ļ�
					if (graph_sum % 10 == 0)
					{
						this_finish_time = (double)clock();
						Answer_Store2(storefile, graph_sum);
						//��¼�ڴ�ʹ�����
						GetMemoryAndTime(infofile,this_start_time, this_finish_time, graph_sum, "Returns 10 results ");
						pattern_answer.clear();
					}
				}
				pattern_answer_temp.answer_edgelist.pop_back();
			}
			arcpathlist[num].curindex = arcpathlist[num].curindex + 1;
		} // while
		 // ������һ���߽���ƥ��(�����滻�����ݵ���һ���������)
		arcpathlist[num].curindex = 0;
		arcpathlist[num].pathset.clear();

		return false;
	}
	else {	//���û������������ƥ���,(�����滻�����ݵ���һ���������)
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

void ReleaseSpace(vector<ArcPath> *edgelengthmatch) {  //�ͷŶ�̬����Ĵ洢�ռ䣬���Ǻ���û��ʲôЧ��
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
