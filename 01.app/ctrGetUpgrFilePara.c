#include "includes.h"

/*
** 控制板下载文件之参数变量 
*/
UpgrFilePara upgrFilePara;

/*
** 控制板下载文件之参数变量初始化
*/
void init_UpgrFilePara(void){
	
}

/*
** set控制板下载文件之参数变量 
*/
void setUpgrFilePara(UpgrFilePara u){
	upgrFilePara = u;
}

/*
** get控制板下载文件之参数变量 
*/
UpgrFilePara getUpgrFilePara(void){
	return upgrFilePara;
}

