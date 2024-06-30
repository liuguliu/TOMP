#include"txt_operation.h"
#include"dat_operation.h"

#define N 2000

void read_Node_Info_B(char* newnodefile)
{	// 读取顶点信息
	int flag; // 用来表示数据库查询的结果

	FILE *fp;
	if (fopen_s(&fp, newnodefile, "rb") != 0) {
		printf("Node query fails!\n");
		return;
	}
	else//文件存在
	{
		int row = 0;
		VNodeR ss[N];
		while (true) {
			int count = fread(ss, sizeof(V), N, fp);  //块读取
			for (int i = 0; i < count; i++) {
				G.NodeList[row].nodenum = ss[i].nodenum;
				G.NodeList[row].factor = ss[i].factor;
				G.NodeList[row].nodelable = ss[i].nodelable;
				row++;
			}
			if (count != N) {
				break;
			}
		}
		G.vexnum = row;
		fclose(fp);
		printf("Node data read is completed!\n");
	}
	//mysql_close(my_connection);
}


void read_Arc_Info_B(char* newedgefile)
{	//读取边上的信息
	int flag;//用来表示数据库查询的结果
	FILE *fp;
	if (fopen_s(&fp, newedgefile, "rb") != 0) {
		printf("Edge query fails!\n");
		return;
	}
	else//文件存在
	{
		int row = 0;
		ArcNodeR ss[N];
		while (true) {
			int count = fread(ss, sizeof(E), N, fp);  //块读取
			for (int i = 0; i < count; i++) {
				ArcNode *arctemp = new ArcNode;
				(*arctemp).adjvex = ss[i].tonode;
				(*arctemp).trust = ss[i].trust;
				(*arctemp).intimacy = ss[i].intimacy;
				if (G.NodeList[ss[i].fromnode].firstarc == NULL)
				{
					G.NodeList[ss[i].fromnode].firstarc = arctemp;
				}
				else
				{
					ArcNode *arcnodetemp = G.NodeList[ss[i].fromnode].firstarc;
					while (arcnodetemp->nextarc != NULL) {
						arcnodetemp = arcnodetemp->nextarc;
					}
					arcnodetemp->nextarc = arctemp;
				}
				row++;
			}
			if (count != N) {
				break;
			}
		}
		G.arcnum = row;
		fclose(fp);
		printf("Edge data read is completed!\n");
	}
}