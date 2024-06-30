#pragma once

#include<iostream>
#include<stdio.h>
#include<fstream>
using namespace std;
//定义存取数据的结构体
typedef struct ArcNodeR {
	int fromnode, tonode;
	float trust;	//social trust(T)
	float intimacy; //social intimacy degree(R)
}E;
typedef struct VNodeR {
	int id;
	int nodenum;	//节点编号（因为可以用数组下标来表示所以不用）	
	float factor; //role impact factor(Rou)
	int nodelable;	//节点标签
}V;

void read_Node_Info_B(char* newnodefile);
void read_Arc_Info_B(char* newedgefile);