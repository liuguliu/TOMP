#pragma once

#include<iostream>
#include<stdio.h>
#include<fstream>
using namespace std;
//�����ȡ���ݵĽṹ��
typedef struct ArcNodeR {
	int fromnode, tonode;
	float trust;	//social trust(T)
	float intimacy; //social intimacy degree(R)
}E;
typedef struct VNodeR {
	int id;
	int nodenum;	//�ڵ��ţ���Ϊ�����������±�����ʾ���Բ��ã�	
	float factor; //role impact factor(Rou)
	int nodelable;	//�ڵ��ǩ
}V;

void read_Node_Info_B(char* newnodefile);
void read_Arc_Info_B(char* newedgefile);