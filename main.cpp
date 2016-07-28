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
        vector<string> dirs;
        getDirent(argv[1], dirs);
        for(int i=0;i<dirs.size();++i) {
            vector<string> filelist;
            cout << dirs[i] << endl;
            getThisDirentAllFiles(dirs[i].c_str(), filelist);
            if(filelist.size() > 0) {
                int start=0, end=10, index=0;
                int alreadyRead = 0;
                while(alreadyRead < filelist.size() && start < filelist.size()) {
                    vector<string> readlists;
                    fetchFileList(start, end, filelist, readlists);
                    alreadyRead += readlists.size();
                    start = end;
                    end += 10;
                    index++;
                    if(readlists.size() == 0)
                        break;
                    cout << "Read Lists " << readlists.size() << endl;
                    vector<vector<double>> data;
                    for(int k=0;k<readlists.size();++k) {
                        readOneFileData(readlists[k], data);
                    }

                    DensityCluster obj(dirs[i], index);
                    obj.initFeaturesLocal(data);
                    obj.calculateDistMatrix();
                    double maxd = 0.0;
                    double dc = obj.getDCDist(0.015, maxd);
                    cout << "DC is " << dc << endl;
                    obj.findDensity(dc);
                    obj.findDistanceToHigherDensity(dc, maxd);
                    obj.findClusterCenters();
                    obj.classifyFeatures2Centers();
                    cout << "Please See the File which in the data dirent !" << endl;
                    vector<vector<double>>().swap(data);
                }
            }
        }
    }
    return 0;
}

int cluster_geo(int argc, char* argv[]) {
    string path = "/Users/liuguiyang/Documents/CodeProj/PyProj/Kaggle/MobileUserDemographics/data/geo_position.csv";
    vector<vector<double>> data;
    readOneFileData(path, data, true);
    set<pair<double, double>> sourceSet;
    int srcLen = 0;
    for(int i=0;i<data.size();++i) {
        pair<double, double> tmp = make_pair(data[i][0], data[i][1]);
        if(sourceSet.count(tmp) == 0) {
            sourceSet.insert(tmp);
            srcLen++;
        }
    }
    vector<vector<double>> newData(srcLen, vector<double>(2,0.0));
    int ind = 0;
    for(set<pair<double, double>>::iterator it=sourceSet.begin();it!=sourceSet.end();it++) {
        newData[ind][0] = (*it).first;
        newData[ind][1] = (*it).second;
        ind++;
    }
    cout << newData.size() << "," << newData[0].size() << endl;
    DensityCluster obj("/Users/liuguiyang/Documents/CodeProj/PyProj/Kaggle/MobileUserDemographics/data/", 1);
    obj.initFeaturesLocal(newData);
    obj.calculateDistMatrix();
    double maxd = 0.0;
    double dc = obj.getDCDist(0.015, maxd);
    cout << "DC is " << dc << endl;
    obj.findDensity(dc);
    obj.findDistanceToHigherDensity(dc, maxd);
    obj.findClusterCenters();
    obj.classifyFeatures2Centers();
    cout << "Please See the File which in the data dirent !" << endl;
    return 0;
}

int main(int argc, char* argv[]) {
    clusterDirs(argc, argv);
    return 0;
}