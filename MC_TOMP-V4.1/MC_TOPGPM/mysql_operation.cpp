//读取数据图的时间复杂度为O(E+V)
//#include"stdafx.h"
#include<iostream>
#include"mysql_operation.h"
using namespace std;

MYSQL *my_connection;//这是一个数据库连接
MYSQL_RES *res_ptr; //指向查询结果的指针
MYSQL_FIELD *field; //字段结构指针
MYSQL_ROW result_row; //按行返回的查询信息

//定义全局变量
Graph G;


void conn()
{	//连接数据库函数
	my_connection = mysql_init(0);//初始化指针

	if (mysql_real_connect(my_connection, HOST, USERNAME, PASSWORD, DATABASE, 0, NULL, 0))
	{	//MYSQL *mysql_real_connect(MYSQL *mysql,const char *host,const char *user,const char *passwd,const char *db,
		//unsigned int port,const char *unix_socket,unsigned long client_flag)	
		printf("database connected!\n");
	}
	else
	{
		printf("database connect error!\n");
	}
}


void read_Node_Info(char* query)
{	//读取顶点信息
	int flag;//用来表示数据库查询的结果
	flag = mysql_query(my_connection, query);
	if (flag != 0)
	{
		printf("The query fails!\n");
	}
	else
	{
		res_ptr = mysql_store_result(my_connection);
		if (res_ptr)
		{
			int row, column;
			column = mysql_num_fields(res_ptr); //表的列数
			row = mysql_num_rows(res_ptr);		//表的行数
			printf("The query reaches %lu rows \n", row);
			for (int i = 0; i<row; i++)
			{	//将表中数据读到结构体数组中
				result_row = mysql_fetch_row(res_ptr);
				G.NodeList[atoi(result_row[0])].nodenum = atoi(result_row[0]);
				G.NodeList[atoi(result_row[0])].factor = atof(result_row[1]);
				G.NodeList[atoi(result_row[0])].nodelable = atoi(result_row[2]);
			}
			G.vexnum = atoi(result_row[0]);	//用图的最后一个顶点的下标作为图的顶点数
			printf(" Node data read is completed!\n");
		}
	}
	//mysql_close(my_connection);
}


void read_Arc_Info(char* query)
{	//读取边上的信息
	int flag;//用来表示数据库查询的结果
	flag = mysql_query(my_connection, query);
	if (flag != 0)
	{
		printf("The query fails!\n");
	}
	else
	{
		res_ptr = mysql_store_result(my_connection);
		if (res_ptr)
		{
			int row, column;
			column = mysql_num_fields(res_ptr); //表的列数
			row = mysql_num_rows(res_ptr);		//表的行数
			printf("The query reaches %lu rows \n", row);
			for (int i = 0; i<row; i++)
			{	//将表中数据读到结构体数组中
				ArcNode *arctemp = new ArcNode;
				result_row = mysql_fetch_row(res_ptr);

				(*arctemp).adjvex = atoi(result_row[1]);
				(*arctemp).trust = atof(result_row[2]);
				(*arctemp).intimacy = atof(result_row[3]);

				int num = atoi(result_row[0]);
				if (G.NodeList[num].firstarc == NULL)
				{
					G.NodeList[num].firstarc = arctemp;
				}
				else
				{
					ArcNode *arcnodetemp = G.NodeList[num].firstarc;
					while (arcnodetemp->nextarc != NULL) {
						arcnodetemp = arcnodetemp->nextarc;
					}
					arcnodetemp->nextarc = arctemp;
				}
				//cout << (*arccurrent).adjvex << " " << (*arccurrent).trust << "  " << (*arccurrent).intimacy << "  " << (*arccurrent).factor << endl;
			}
			G.arcnum = row;
			printf("Edge data read is completed!\n");
		}
	}
	//mysql_close(my_connection);
}

void close() {
	mysql_close(my_connection);
}
