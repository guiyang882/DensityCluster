//
// Created by 贵阳 on 6/17/16.
//

#ifndef DEMO_DENSITYCLUSTER_H
#define DEMO_DENSITYCLUSTER_H

#include <vector>
#include <iostream>
#include <fstream>
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
    void initFeaturesLocal(string filename);
    void splitString(string str, vector<double> &res, char ch);
    void generateFeatures(int row, int col);
    /*
     * filename : save user file name
     * saveType : save data type, such as FEATURE, CENTER, RESULT
     * */
    void saveData(string filename, string saveType);

public:
    inline double calcDist(vector<double>& v1, vector<double>& v2);
    bool calculateDistMatrix();
    double getDCDist(double dc_percent, double &maxd);

    /* 获取在给定截断距离下的各个特征点的个数信息 */
    void findDensity(double dc);

    /* 计算每个点到最近的高密度距离中的最大距离,同时确定每个点离那个点更近 */
    void findDistanceToHigherDensity(double dc, double maxd);
    void findClusterCenters(double ratio);

private:
    /* 主要用来存储待距离的特征向量 */
    vector<vector<double>> m_features;

    vector<vector<double>> m_distMatrix;
    map<pair<int, int>, double> m_distMap;

    /* 主要用来标记是否使用了距离矩阵来存储任意两个点之间的距离 */
    bool isUseDistMatrix;

    /* 该参数主要用来存储每个特征向量周边有多少个点是在DC距离内 */
    vector<int> m_density;
    /* 该参数主要用来存储每个特征向量周边的密度信息和该点在features中的编号信息 <密度(个数),编号> */
    vector<pair<int, int>> m_density_pair;

    vector<double> m_minDist2Higher;
    vector<int> m_nearestNeighbor;
    vector<int> m_centers;
    /*
     * 主要用来存储程序运行的结果
     * key : type is int, means the index of the centers in the m_features
     * value: type is vector<int>, means the index of the m_features where closest to the some centers
     * */
    map<int, vector<int>> m_result;

private:
    string saveprefix;
};


#endif //DEMO_DENSITYCLUSTER_H
