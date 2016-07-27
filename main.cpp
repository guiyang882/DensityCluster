#include <iostream>
#include "densitycluster.h"
#include "utils.h"

using namespace std;

int main(int argc, char* argv[]) {
    if(argc == 2) {
        vector<string> dirs;
        getDirent(argv[1], dirs);
        vector<vector<double>> data;
        for(int i=0;i<dirs.size();++i) {
            vector<string> filelist;
            getThisDirentAllFiles(dirs[i].c_str(), filelist);
            for(int k=0;k<filelist.size();++k) {
                readOneFileData(filelist[k], data);
            }
            DensityCluster obj(dirs[i]);
            obj.initFeaturesLocal(data);
//            obj.generateFeatures(3*20000, 2);
            obj.calculateDistMatrix();
            double maxd = 0.0;
            double dc = obj.getDCDist(0.015, maxd);
            cout << "DC is " << dc << endl;
            obj.findDensity(dc);
            obj.findDistanceToHigherDensity(dc, maxd);
            obj.findClusterCenters();
            obj.classifyFeatures2Centers();
            cout << "Please See the File which in the data dirent !" << endl;
            break;
        }
    }
    return 0;
}
