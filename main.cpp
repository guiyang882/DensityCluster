#include <iostream>
#include "densitycluster.h"

using namespace std;

int main() {
    DensityCluster obj;
    obj.initFeaturesLocal("/Users/liuguiyang/Documents/CodeProj/ConsoleProj/DensityCluster/data/features.csv");
//    obj.generateFeatures(3*10, 2);
    obj.calculateDistMatrix();
    double dc = obj.getDCDist(0.01, 0.02);
    cout << "DC is " << dc << endl;
    obj.findDensity(dc);
    obj.findDistanceToHigherDensity(dc);
    obj.findClusterCenters(1/3.0);
    obj.makeCenters();
    obj.findClusterDesignation();
    obj.fetchFeaturesInClusters();
    cout << "Density Cluster Run Done !" << endl;
    return 0;
}
