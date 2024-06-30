#pragma once
#include<mysql.h>


#define MAXNUMV 1650000
//������һЩ���ݿ�������Ҫ�ĺ� 
#define HOST "localhost" 
#define USERNAME "root"
#define PASSWORD "123456"
#define DATABASE "mc_gpm" 
//const char table[] = "epinions";
extern MYSQL *my_connection;//����һ�����ݿ�����
extern MYSQL_RES *res_ptr; //ָ���ѯ�����ָ��
extern MYSQL_FIELD *field; //�ֶνṹָ��
extern MYSQL_ROW result_row; //���з��صĲ�ѯ��Ϣ


//�����ȡ���ݵĽṹ��
struct ArcNode {
	int adjvex;	//�û���ָ��Ķ���ı��
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
	int nodenum;	//�ڵ��ţ���Ϊ�����������±�����ʾ���Բ��ã�	
	float factor; //role impact factor(Rou)
	int nodelable;	//�ڵ��ǩ
	ArcNode *firstarc;	//ָ���һ�������ڸö���Ļ���ָ��

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

//����ȫ�ֱ���
extern Graph G;

//��������
void conn();	//�������ݿ⺯��
void read_Node_Info(char* query);	//��ȡ���ݿ�������
void read_Arc_Info(char* query);	//��ȡ���ϵ���Ϣ
void close();	//�ر����ݿ�


