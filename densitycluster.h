//
// Created by 贵阳 on 6/17/16.
//

#ifndef DEMO_DENSITYCLUSTER_H
#define DEMO_DENSITYCLUSTER_H

#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <functional>
#include <map>
#include <set>

#define _OPENMP

#ifdef _OPENMP

#if defined(__clang__)
#include <libiomp/omp.h>
#elif defined(__GNUG__) || defined(__GNUC__)
#include <omp.h>
#endif

#endif

using namespace std;

class DensityCluster {
public:
    DensityCluster();
    ~DensityCluster();

public:
    void initFeatures(vector<vector<double>> features);
    void generateFeatures(int row, int col);

public:
    inline double calcDist(vector<double>& v1, vector<double>& v2);
    bool calculateDistMatrix();
    double getDCDist(double neighborRateLow, double neighborRateHigh);
    void findDensity(double dc);
    void findDistanceToHigherDensity(double dc);
    void findClusterCenters(double ratio);
    void findClusterDesignation();
    int findSingleFeatureClusterDesignation(int nearIndex);
    vector<vector<int>> fetchFeaturesInClusters();

public:
    static void showVector(string header, vector<double> datas);

private:
    vector<vector<double>> m_features;

    vector<vector<double>> m_distMatrix;
    map<pair<int, int>, double> m_distMap;
    bool isUseDistMatrix;

    vector<int> m_density;
    vector<double> m_minDist2Higher;
    vector<int> m_nearestNeighborOfHigherDensity;
    vector<int> m_centers;
    vector<int> m_clusterDesignation;
    set<vector<double>> m_selected;
};


#endif //DEMO_DENSITYCLUSTER_H
