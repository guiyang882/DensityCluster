//
// Created by 贵阳 on 7/26/16.
//

#ifndef DENSITYCLUSTER_UTILS_H
#define DENSITYCLUSTER_UTILS_H

#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

void ListAllFile(const char *path);
void getDirent(const char* topPath, vector<string> &dirs);
void getThisDirentAllFiles(const char* curPath, vector<string> &absFilePath);
void readOneFileData(string filepath, vector<vector<double >>& data, bool skipHeader = false);
void splitStr(string str, vector<double >& res, char ch);

#endif //DENSITYCLUSTER_UTILS_H
