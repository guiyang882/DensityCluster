//
// Created by 贵阳 on 6/17/16.
//

#include "densitycluster.h"

DensityCluster::DensityCluster() {

}

DensityCluster::~DensityCluster() {

}

void DensityCluster::initFeatures(vector<vector<double>> features) {
    m_features = features;
}

void DensityCluster::generateFeatures(int row, int col) {
    clock_t t;
    t = clock();

    std::default_random_engine generator;
    std::normal_distribution<double> distribution01(0.0,2.0);
    std::normal_distribution<double> distribution02(5.0,2.0);
    std::normal_distribution<double> distribution03(-5.0,2.0);

    m_features.resize(row, vector<double>(col,0.0));

    for(int k=0; k<row; ++k) {
        for(int i=0; i<col; ++i) {
            double number = 0.0;
            if(k / (row/3) == 0) {
                number = distribution01(generator);
            } else if(k / (row/3) == 1) {
                number = distribution02(generator);
            } else {
                number = distribution03(generator);
            }
            m_features[k][i] = number;
        }
    }

    t = clock() - t;
    cout << "generateFeatures took me " << t << " clicks (" << ((float)t)/CLOCKS_PER_SEC << " seconds)." << endl;
}

double DensityCluster::calcDist(vector<double> &v1, vector<double> &v2) {
    if(v1.size() != v2.size()) {
        return -1.0;
    }
    double total_norm = 0.0;
    for(int i=0;i<v1.size();++i) {
        total_norm += pow(v1[i] - v2[i], 2);
    }
    return pow(total_norm, 0.5);
}

bool DensityCluster::calculateDistMatrix() {
    clock_t t;
    t = clock();

    int row = m_features.size();
    if(row <= 5000) {
        isUseDistMatrix = true;
        m_distMatrix.resize(row, vector<double>(row, 0.0));

#pragma omp parallel for
        for(int i=0;i<row;++i) {
            for(int j=i+1;j<row;++j) {
                double dist = calcDist(m_features[i], m_features[j]);
                m_distMatrix[i][j] = dist;
                m_distMatrix[j][i] = dist;
            }
        }
    } else {
        isUseDistMatrix = false;

#pragma omp parallel for
        for(int i=0;i<row;++i) {
            for(int j=i;j<row;++j) {
                if(i == j) {
                    m_distMap[make_pair(i, j)] = 0.0;
                } else {
                    double dist = calcDist(m_features[i], m_features[j]);
                    m_distMap[make_pair(i, j)] = dist;
                }
            }
        }
    }

    t = clock() - t;
    cout << "calculateDistMatrix took me " << t << " clicks (" << ((float)t)/CLOCKS_PER_SEC << " seconds)." << endl;
    return true;
}

double DensityCluster::getDCDist(double neighborRateLow, double neighborRateHigh) {
    clock_t t;
    t = clock();

    int row = m_features.size();
    int neighborsDownLimit = row * neighborRateLow;
    int neighborsUpLimit = row * neighborRateHigh + 1;

    double dc = 0.0;
    vector<double> boundaryDown(row, 0.0);
    vector<double> boundaryUp(row, 0.0);

    if(isUseDistMatrix) {
        vector<vector<double>> matrix = m_distMatrix;

#pragma omp parallel for
        for (int i = 0; i < row; ++i) {
            sort(matrix[i].begin(), matrix[i].end());
            boundaryDown[i] = matrix[i][neighborsDownLimit];
            boundaryUp[i] = matrix[i][neighborsUpLimit];
        }
    } else {
#pragma omp parallel for
        for (int i=0; i<row; ++i) {
            vector<double> t_dist(row, 0.0);
            for(int j=0; j<row; ++j) {
                if(i <= j) {
                    t_dist[j] = m_distMap[make_pair(i, j)];
                } else {
                    t_dist[j] = m_distMap[make_pair(j, i)];
                }
            }
            sort(t_dist.begin(), t_dist.end());
            boundaryDown[i] = t_dist[neighborsDownLimit];
            boundaryUp[i] = t_dist[neighborsUpLimit];
        }
    }

    sort(boundaryDown.begin(), boundaryDown.end(), greater<double>());
    sort(boundaryUp.begin(), boundaryUp.end());

    for(int i=0; i<row; ++i) {
        if(boundaryUp[i] >= boundaryDown[i]) {
            dc = (boundaryUp[i] + boundaryDown[i]) / 2.0;
            break;
        }
    }

    t = clock() - t;
    cout << "getDCDist took me " << t << " clicks (" << ((float)t)/CLOCKS_PER_SEC << " seconds)." << endl;

    return dc;
}

void DensityCluster::findDensity(double dc) {
    int row = m_features.size();
    m_density.resize(row, 0);

#pragma omp parallel for
    for(int i=0; i<row; ++i) {
        int cnt = 0;
        for(int j=0; j<row; ++j) {
            if(isUseDistMatrix && j != i && m_distMatrix[i][j] < dc) {
                ++cnt;
            }
            if(isUseDistMatrix == false && j != i && m_distMap[make_pair(i>j?j:i, i>j?i:j)] < dc) {
                ++cnt;
            }
        }
        m_density[i] = cnt;
    }
}

void DensityCluster::findDistanceToHigherDensity(double dc) {
    int row = m_features.size();
    m_minDist2Higher.resize(row, 0.0);
    m_nearestNeighborOfHigherDensity.resize(row, 0);

#pragma omp parallel for
    for(int i=0; i<row; ++i) {
        int index_density = m_density[i];
        double minDist = dc;
        for(int candidate=0; candidate<row; ++candidate) {
            if(candidate != i) {
                double candidate_dist = 0.0;
                if(isUseDistMatrix) {
                    candidate_dist = m_distMatrix[i][candidate];
                } else {
                    candidate_dist = m_distMap[make_pair(i>candidate?candidate:i, i>candidate?i:candidate)];
                }
                if(candidate_dist < minDist && m_density[candidate] > index_density) {
                    minDist = candidate_dist;
                    m_nearestNeighborOfHigherDensity[i] = candidate;
                }
            }
        }
        m_minDist2Higher[i] = minDist;
    }
}

void DensityCluster::findClusterCenters(double ratio) {
    int total_len = m_minDist2Higher.size();
    vector<pair<int,double>> tmp;
    for(int i=0;i<total_len;++i) {
        tmp.push_back(make_pair(i, m_minDist2Higher[i] * m_density[i]));
    }

    sort(tmp.begin(), tmp.end(), [](pair<int, double>&left, pair<int, double>& right) {
        return left.second > right.second;
    });

    int selectInd = total_len * ratio;
    for(int i=0; i<total_len; ++i) {
        if(i <= selectInd && tmp[i].second) {
            m_centers.push_back(tmp[i].first);
            m_selected.insert(m_features[tmp[i].first]);
        } else if(tmp[i].second == 0.0) {
            m_selected.insert(m_features[tmp[i].first]);
        }
    }

//    for(int i=0; i<total_len; ++i) {
//        cout << tmp[i].first << " ," << tmp[i].second << endl;
//    }

    cout << "Finished Done !" << endl;
}

void DensityCluster::findClusterDesignation() {
    int row = m_features.size();
    for(int i=0; i<row; ++i) {
        findSingleFeatureClusterDesignation(i);
    }
}

int DensityCluster::findSingleFeatureClusterDesignation(int nearIndex) {
    int status = m_clusterDesignation[nearIndex];
    if(status == 0) {
        int ind = m_nearestNeighborOfHigherDensity[nearIndex];
        int resInd = findSingleFeatureClusterDesignation(ind);
        m_clusterDesignation[nearIndex] = resInd;
    }
    return 0;
}

vector<vector<int>> DensityCluster::fetchFeaturesInClusters() {
    int row = m_features.size();
    vector<vector<int>> classifyCluster(m_centers.size(), vector<int>());

#pragma omp parallel for
    for(int i=0; i<m_centers.size(); ++i) {
        vector<int> indCluster;
        for(int j=0; j<row; ++j) {
            if(m_clusterDesignation[j] == m_centers[i]) {
                indCluster.push_back(j);
            }
        }
        classifyCluster[i] = indCluster;
    }
    return classifyCluster;
}

void DensityCluster::showVector(string header, vector<double> datas) {
    cout << header << endl;
    for(int i=0; i<datas.size(); ++i) {
        cout << datas[i] << endl;
    }
    cout << endl;
}