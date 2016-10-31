#include <iostream>
#include "densitycluster.h"
#include "utils.h"

using namespace std;

void fetchFileList(int start, int end, vector<string> srcFilelists, vector<string>& filelists) {
    filelists.clear();
    for(int i=start;i<end && i < srcFilelists.size();i++) {
        filelists.push_back(srcFilelists[i]);
    }
}

int clusterDirs(int argc, char* argv[]) {
    if(argc == 2) {
        string dirpath(argv[1]);
        vector<string> filelist;
        getThisDirentAllFiles(dirpath.c_str(), filelist);
        if(filelist.size() > 0) {
            int start=0, end=5, index=0;
            int alreadyRead = 0;
            while(alreadyRead < filelist.size() && start < filelist.size()) {
                vector<string> readlists;
                fetchFileList(start, end, filelist, readlists);
                alreadyRead += readlists.size();
                start = end;
                end += 5;
                index++;
                if(readlists.size() == 0)
                    break;
                cout << "Read Lists " << readlists.size() << endl;
                vector<vector<double>> data;
                for(int k=0;k<readlists.size();++k) {
                    readOneFileData(readlists[k], data);
                }

                DensityCluster obj(dirpath, index);
                cout << "initFeaturesLocal" << endl;
                obj.initFeaturesLocal(data);
                cout << "calculateDistMatrix" << endl;
                obj.calculateDistMatrix();
                double maxd = 0.0;
                double dc = obj.getDCDist(0.015, maxd);
                cout << "DC is " << dc << endl;
                cout << "findDensity" << endl;
                obj.findDensity(dc);
                cout << "findDistanceToHigherDensity" << endl;
                obj.findDistanceToHigherDensity(dc, maxd);
                cout << "findClusterCenters" << endl;
                obj.findClusterCenters();
                cout << "classifyFeatures2Centers" << endl;
                obj.classifyFeatures2Centers();
                cout << "Please See the File which in the data dirent !" << endl;
                data.clear();
            }
        }
    }
    return 0;
}

void match_brackets(string &dirpath) {
    /*
     * 在找到字符串中的左括号和右括号是在前面添加一下"\"
     * */
    int start=0;
    while(start<dirpath.size()) {
        if(dirpath[start] == ')' || dirpath[start] == '(') {
            dirpath.insert(start, 1, '\\');
            start++;
        }
        start++;
    }
}

void getAllDirPaths(int argc, char* argv[]) {
    if(argc != 3)
        return;
    vector<string> dirs;
    getDirent(argv[1], dirs);
    string cmd(argv[2]);
    string str_cmd;

    for(int i=0;i<dirs.size();i++) {
        str_cmd = cmd + " " + dirs[i] + " &";
        match_brackets(str_cmd);
        cout << str_cmd << endl;
        system(str_cmd.c_str());
    }
}

int main(int argc, char* argv[]) {
    getAllDirPaths(argc, argv);
    return 0;
}
