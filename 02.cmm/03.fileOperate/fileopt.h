#ifndef __FILEOPT_H_
#define __FILEOPT_H_


/*
** 分析是否是.bin文件以及获取下标参数
** @return:返回版本信息下标
*/
int16 fileNameVerSuffix(uint8* data,uint16 len);

bool FileNameVerOpt(uint8*data,uint16 len);

#endif

