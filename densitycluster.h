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
    DensityCluster(string savedir);
    ~DensityCluster();

public:
    void initFeaturesLocal(vector<vector<double >>& data);
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

    /* 如何从论文中曲线去分析,使用策略寻找处潜在的聚类中心 */
    void findClusterCentersByRatio(double ratio = 0.3);

    /* 根据论文中乘积曲线的一阶差分进行分析,得到我们选取的聚类中心 */
    void findClusterCenters();

    /* 根据找到的聚类中心将其他的点进行分类,确定点的类别 */
    void classifyFeatures2Centers();

private:
    /* 主要用来存储待距离的特征向量 */
    vector<vector<double>> m_features;
    vector<int> m_realClassType;

    vector<vector<double>> m_distMatrix;
    map<pair<int, int>, double> m_distMap;

    /* 主要用来标记是否使用了距离矩阵来存储任意两个点之间的距离 */
    bool isUseDistMatrix;
    /* 该参数主要用来存储每个特征向量周边有多少个点是在DC距离内 */
    vector<int> m_density;
    /* 该参数主要用来存储每个特征向量周边的密度信息和该点在features中的编号信息 <密度(个数),编号> */
    vector<pair<int, int>> m_density_pair;
    /* 该参数主要是用来存储每个特征向量到更高一级的密度点中的最短的距离 */
    vector<double> m_minDist2Higher;
    /* 该参数主要是用来存储每个特征向量到更高密度点中的最近距离的index */
    vector<int> m_nearestNeighbor;
    /* 该参数主要是用来存储该算法找到的聚类中心的位置 */
    vector<int> m_centers;
    /* 该参数主要是用来存储算法对每个特征点的分类结果 */
    vector<int> m_classType;

private:
    string saveprefix;
};


#endif //DEMO_DENSITYCLUSTER_H
