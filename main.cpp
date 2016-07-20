#include <iostream>
#include "densitycluster.h"

using namespace std;

int main() {
    DensityCluster obj;
//    obj.initFeaturesLocal("/Users/liuguiyang/Documents/CodeProj/ConsoleProj/DensityCluster/data/features.csv");
    obj.generateFeatures(3*100, 2);
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
