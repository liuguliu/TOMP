#pragma once
#include<mysql.h>


#define MAXNUMV 1650000
//定义了一些数据库连接需要的宏 
#define HOST "localhost" 
#define USERNAME "root"
#define PASSWORD "123456"
#define DATABASE "mc_gpm" 
//const char table[] = "epinions";
extern MYSQL *my_connection;//这是一个数据库连接
extern MYSQL_RES *res_ptr; //指向查询结果的指针
extern MYSQL_FIELD *field; //字段结构指针
extern MYSQL_ROW result_row; //按行返回的查询信息


//定义存取数据的结构体
struct ArcNode {
	int adjvex;	//该弧所指向的顶点的编号
	struct ArcNode *nextarc;
	float trust;	//social trust(T)
	float intimacy; //social intimacy degree(R)

	ArcNode() {
		adjvex = -1;
		nextarc = NULL;
		trust = 0.0;
		intimacy = 0.0;

	}
};
struct VNode {
	int nodenum;	//节点编号（因为可以用数组下标来表示所以不用）	
	float factor; //role impact factor(Rou)
	int nodelable;	//节点标签
	ArcNode *firstarc;	//指向第一条依附于该顶点的弧的指针

	VNode() {
		factor = 0.0;
		nodelable = -1;
		firstarc = NULL;
	}
};
struct Graph {
	VNode NodeList[MAXNUMV];
	int vexnum, arcnum;
};

//定义全局变量
extern Graph G;

//声明函数
void conn();	//连接数据库函数
void read_Node_Info(char* query);	//读取数据库中数据
void read_Arc_Info(char* query);	//读取边上的信息
void close();	//关闭数据库


