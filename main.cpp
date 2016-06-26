#include <iostream>
#include "densitycluster.h"

using namespace std;

int main() {
    DensityCluster obj;
    obj.generateFeatures(20*5000, 128);
    obj.calculateDistMatrix();
    double dc = obj.getDCDist(0.01, 0.02);
    obj.findDensity(dc);
    obj.findDistanceToHigherDensity(dc);
    obj.findClusterCenters(1/3.0);

    return 0;
}
