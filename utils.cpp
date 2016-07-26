//
// Created by 贵阳 on 7/26/16.
//

#include "utils.h"

void ListAllFile(const char *path) {
    struct dirent* ent = NULL;
    DIR *pDir;
    pDir=opendir(path);
    while (pDir != NULL && NULL != (ent=readdir(pDir))) {
        switch (ent->d_type) {
            case DT_REG:
                if(strncmp(ent->d_name, ".", 1) == 0)
                    continue;
                cout << ent->d_name << endl;
                break;
            case DT_DIR:
            {
                if(strncmp(ent->d_name, ".", 1) == 0)
                    continue;
                cout << ent->d_name << endl;
                string absPath(path);
                if(absPath[absPath.size() - 1] == '/')
                    absPath += string(ent->d_name);
                else
                    absPath = absPath + "/" + string(ent->d_name);
                ListAllFile(absPath.c_str());
                break;
            }
            default:
                cout << ent->d_name << endl;
        }
    }
}

void getDirent(const char* topPath, vector<string> &dirs) {
    struct dirent* ent = NULL;
    DIR *pDir;
    pDir=opendir(topPath);
    while (pDir != NULL && NULL != (ent=readdir(pDir))) {
        switch (ent->d_type) {
            case DT_DIR:
            {
                if(strncmp(ent->d_name, ".", 1) == 0)
                    continue;
                cout << ent->d_name << endl;
                string absPath(topPath);
                if(absPath[absPath.size() - 1] == '/')
                    absPath += string(ent->d_name);
                else
                    absPath = absPath + "/" + string(ent->d_name);
                dirs.push_back(absPath);
                break;
            }
        }
    }
}

void getThisDirentAllFiles(const char* curPath, vector<string> &absFilePath) {
    struct dirent* ent = NULL;
    DIR *pDir;
    pDir=opendir(curPath);
    while (pDir != NULL && NULL != (ent=readdir(pDir))) {
        switch (ent->d_type) {
            case DT_REG:
            {
                if(strncmp(ent->d_name, ".", 1) == 0)
                    continue;
                cout << ent->d_name << endl;
                string abspath(curPath);
                if(abspath[abspath.size()-1] == '/')
                    abspath += string(ent->d_name);
                else
                    abspath = abspath + "/" + string(ent->d_name);
                absFilePath.push_back(abspath);
                break;
            }
        }
    }
}

void readOneFileData(string filepath, vector<vector<double >>& data) {
    ifstream in(filepath, std::ifstream::in);
    string strValue;
    while(getline(in,strValue)) {
        vector<double > tmp;
        splitStr(strValue, tmp, ',');
        data.push_back(tmp);
    }
    in.close();
}

void splitStr(string str, vector<double >& res, char ch) {
    //trim the free space from the head and tail
    str.erase(0,str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    if(str.size() == 0) return ;
    int start=0, end=0;
    while(start <= end && end < str.size()) {
        while(end<str.size() && str[end]!=ch) end++;
        string sub = str.substr(start, end-start);
        res.push_back(stoi(sub));
        while(end<str.size() && str[end]==ch) end++;
        start = end;
    }
}