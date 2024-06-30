//��ȡ����ͼ��ʱ�临�Ӷ�ΪO(E+V)
//#include"stdafx.h"
#include<iostream>
#include<fstream>
//#include <cstringt.h>
//#include<windows.h>
#include"txt_operation.h"
using namespace std;


//����ȫ�ֱ���
Graph G;


void read_Node_Info(char* nodefile)
{	// ��ȡ������Ϣ
	int flag; // ������ʾ���ݿ��ѯ�Ľ��

	ifstream ReadFile;
	int n = 0;
	string tmp;
	ReadFile.open(nodefile, ios::in); // ios::in ��ʾ��ֻ���ķ�ʽ��ȡ�ļ�
	if (ReadFile.fail()) // �ļ���ʧ��:����0
	{
		printf("Node query fails!\n");
		return;
	}
	else//�ļ�����
	{
		int row = 0;
		while (!ReadFile.eof())
		{
			
			ReadFile >> row;
			ReadFile >> G.NodeList[row].nodenum;
			//G.NodeList[row].nodenum = row;
			ReadFile >> G.NodeList[row].factor;
			ReadFile >> G.NodeList[row].nodelable;
		}
		G.vexnum = row;
		ReadFile.close();
		printf("Node data read is completed!\n");
	}
	//mysql_close(my_connection);
}


void read_Arc_Info(char* edgefile)
{	//��ȡ���ϵ���Ϣ
	int flag;//������ʾ���ݿ��ѯ�Ľ��
	ifstream ReadFile;
	int n = 0;
	string tmp;
	ReadFile.open(edgefile, ios::in);//ios::in ��ʾ��ֻ���ķ�ʽ��ȡ�ļ�
	if (ReadFile.fail())//�ļ���ʧ��:����0
	{
		printf("Edge query fails!\n");
		return;
	}
	else//�ļ�����
	{
		int row = 0,startnum=0,endnum=0;
		while (!ReadFile.eof())
		{
			row++;
			//���������ݶ����ṹ��������
			ArcNode *arctemp = new ArcNode;
			ReadFile >> startnum;
			ReadFile >> (*arctemp).adjvex;
			//ReadFile >> (*arctemp).sourcesid;
			//ReadFile >> (*arctemp).sourceeid;
			ReadFile >> (*arctemp).trust;
			ReadFile >> (*arctemp).intimacy;

			if (G.NodeList[startnum].firstoutarc == NULL)
			{
				G.NodeList[startnum].firstoutarc = arctemp;
			}
			else
			{
				ArcNode *arcnodetemp = G.NodeList[startnum].firstoutarc;
				while (arcnodetemp->nextarc != NULL) {
					arcnodetemp = arcnodetemp->nextarc;
				}
				arcnodetemp->nextarc = arctemp;
			}
			ArcNode *arcintemp = new ArcNode;
			arcintemp->adjvex = startnum;
			arcintemp->trust = (*arctemp).trust;
			arcintemp->intimacy = (*arctemp).intimacy;
			if (G.NodeList[(*arctemp).adjvex].firstinarc == NULL) {
				G.NodeList[(*arctemp).adjvex].firstinarc = arcintemp;
			}
			else
			{
				ArcNode *intemp = G.NodeList[(*arctemp).adjvex].firstinarc;
				while (intemp->nextarc != NULL) {
					intemp = intemp->nextarc;
				}
				intemp->nextarc = arcintemp;
			}
			//cout << (*arccurrent).adjvex << " " << (*arccurrent).trust << "  " << (*arccurrent).intimacy << "  " << (*arccurrent).factor << endl;
		}
		G.arcnum = row;	
		ReadFile.close();
		printf("Edge data read is completed!\n");
	}
}

//void Memory_read_Node_Info(char* nodefile)
//{	//��ȡ������Ϣ
//	int flag;//������ʾ���ݿ��ѯ�Ľ��
//
//
//	//��������ļ��ں˶���;
//	HANDLE fileH = CreateFile("ldFeature.txt",
//		GENERIC_READ | GENERIC_WRITE,
//		FILE_SHARE_READ,
//		NULL,
//		OPEN_EXISTING,
//		FILE_ATTRIBUTE_NORMAL,
//		NULL);
//	if (fileH == INVALID_HANDLE_VALUE)
//	{
//		cout << "error in CreateFile" << endl;
//		return -1;
//	}
//
//	//����һ���ļ�ӳ���ں˶���;
//	HANDLE mapFileH = CreateFileMapping(fileH,
//		NULL,
//		PAGE_READWRITE,
//		0,
//		0,
//		"Resource ");
//	if (mapFileH == NULL)
//	{
//		cout << "error in CreateFileMapping" << endl;
//		return -1;
//	}
//
//	//���ļ�����ӳ�䵽���̵ĵ�ַ�ռ�;
//	char * mapH = (char *)MapViewOfFile(mapFileH,
//		FILE_MAP_ALL_ACCESS,
//		0,
//		0,
//		0);
//	if (mapH == NULL)
//	{
//		cout << "error in MapViewOfFile" << endl;
//		return -1;
//	}
//
//	//��ȡ����;
//	char *buf = mapH;
//	double k;
//	int times = 300000 * 34;
//	for (int i = 1; i <= times; i++)
//	{
//		k = atof(buf);
//		buf = strstr(buf + 9, " ");
//		//cout<<k<<endl;    
//	}
//	//�رվ��;
//	UnmapViewOfFile(mapH);
//	CloseHandle(mapFileH);
//	CloseHandle(fileH);
//}