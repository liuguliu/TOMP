//#include<vector>
//#include<queue>
//#include<iostream>
//#include"breadth_first_matchtest.h"
//using namespace std;
//
////定义全局变量
//ArcPathList arcpathlist[MAX];	//存储边匹配的结果
//vector<PArcNode> breadth_P;
//
//void breadth_first_Traverse() {
//	queue<PArcNode> Q;
//	for (int i = 1; i <= P.pvexnum; i++) {
//		PArcNode *parctemp = P.pnodeList[i].firstArc;
//		if (parctemp!=NULL&&!(*parctemp).visited) {
//			while (parctemp != NULL) {
//				Q.push(*parctemp);
//				(*parctemp).visited = true;
//				parctemp = (*parctemp).next;
//			}		
//			while (!Q.empty()) {
//				PArcNode parc = Q.front();
//				Q.pop();
//				breadth_P.push_back(parc);
//				PArcNode *parcnext = P.pnodeList[parc.toNode].firstArc;
//				while (parcnext != NULL && !(*parcnext).visited) {
//					Q.push(*parcnext);
//					(*parcnext).visited = true;
//					parcnext = (*parcnext).next;
//				}
//			}
//		}
//	}
//}
//
//void edge_match(PArcNode *p, int num) {//遍历数据图,对数据图中标签与模式边的起始节点相同的节点进行有届深度搜索
//	arcpathlist[num].fromnode = (*p).fromNode;
//	arcpathlist[num].tonode = (*p).toNode;
//	for (int i = 0; i <= G.vexnum; i++) {
//		if (G.NodeList[i].nodelable == P.pnodeList[(*p).fromNode].PNodeLable&&G.NodeList[i].factor - P.pnodeList[(*p).fromNode].factor >= 0) {	//对节点G.NodeList[i]进行有限深度,广度优先搜索
//			int deepth = 0;
//			ArcPath pathi;
//			queue<ArcPath> q;	//定义一个队列辅助广度优先搜索	
//								//int *answerexist = new int[G.vexnum + 1];
//			pathi.startnode = pathi.endnode = i;
//			q.push(pathi);
//			while (!q.empty()) {
//				bool flag = true;
//				ArcPath pathj = q.front();	//获取队头元素			
//				q.pop();	//队头元素出队
//				if (G.NodeList[pathj.endnode].nodelable == P.pnodeList[(*p).toNode].PNodeLable&&G.NodeList[pathj.endnode].factor - P.pnodeList[(*p).toNode].factor >= 0) {	//找到一个满足bounded length的路径
//					if (pathj.path.size()>0) {	//计算其他限制条件，并判断是否满足限制条件
//						pathj.length = pathj.path.size();
//						pathj.answerfactor = G.NodeList[pathj.startnode].factor;
//						pathj.answertrust = 1.0;
//						pathj.answerintimacy = 1.0;
//						for (int k = 0; k < pathj.path.size(); k++) {
//							pathj.answerfactor += G.NodeList[pathj.path[k].adjvex].factor;
//							pathj.answertrust *= pathj.path[k].trust;
//							pathj.answerintimacy *= pathj.path[k].intimacy;
//						}
//						pathj.answerfactor = pathj.answerfactor / (pathj.length + 1);
//
//						if ((pathj.answerfactor - (*p).pfactor) >= 0 && (pathj.answertrust - (*p).ptrust) >= 0 && (pathj.answerintimacy - (*p).pintimacy) >= 0) {	//所有限制条件都满足
//																																									//所有条件都满足，判断与已知解是否重复
//							bool flag0 = false;
//							for (int k = (arcpathlist[num].pathset.size() - 1); k >= 0; k--) {
//								if (pathj.startnode == arcpathlist[num].pathset[k].startnode) {
//									if (pathj.endnode == arcpathlist[num].pathset[k].endnode) {
//										flag0 = true;
//										if ((pathj.answerfactor - arcpathlist[num].pathset[k].answerfactor) > 0 && (pathj.answertrust - arcpathlist[num].pathset[k].answertrust) > 0
//											&& (pathj.answerintimacy - arcpathlist[num].pathset[k].answerintimacy) > 0) {
//											//(1)重复但比原来的好，替换	
//											arcpathlist[num].pathset[k] = pathj;
//										}
//										//(2)重复，不比原来的好，丢弃
//										flag = false;	//一旦重复就不用再对该节点进行进一步分析了									
//										break;
//									}
//								}
//								else { //一旦起始节点不一样就可以结束循环了
//									break;
//								}
//							}
//							if (!flag0) {	//(3)不重复，添加
//								arcpathlist[num].pathset.push_back(pathj);
//							}
//						}
//						else {
//							flag = false;
//						}
//					}
//				}
//				if (flag) {
//					ArcNode *arctemp;
//					arctemp = G.NodeList[pathj.endnode].firstarc;
//					deepth = pathj.path.size();
//					while (deepth<(*p).boundlength&&arctemp != NULL) {
//						//while (deepth<(*p).boundlength&&arctemp != NULL && G.NodeList[(*arctemp).adjvex].factor - P.pnodeList[(*p).toNode].factor >= -0.001){
//						if (G.NodeList[(*arctemp).adjvex].factor - P.pnodeList[(*p).toNode].factor >= 0 && (*arctemp).trust - (*p).ptrust>-0.1 && (*arctemp).intimacy - (*p).pintimacy>-0.1) {
//							//if (G.NodeList[(*arctemp).adjvex].factor - P.pnodeList[(*p).toNode].factor >= 0){
//							ArcPath cur_path;
//							//cur_path.startnode = pathj.startnode;
//							cur_path.startnode = i;
//							cur_path.endnode = (*arctemp).adjvex;
//							cur_path.path = pathj.path;
//							cur_path.path.push_back((*arctemp));
//							q.push(cur_path);	//入队
//						}
//						arctemp = (*arctemp).nextarc;
//					}
//				}
//			}//while
//		}
//		else {
//			continue;
//		}
//	}//for
//}