//
// Created by 贵阳 on 6/17/16.
//

#include "densitycluster.h"

DensityCluster::DensityCluster(string savedir, int index) {
    saveprefix = savedir;
    if(saveprefix[saveprefix.size()-1] != '/')
        saveprefix += "/";
    saveprefix += to_string(index);
    saveprefix += "_";
}

DensityCluster::~DensityCluster() {

}

void DensityCluster::initFeaturesLocal(vector<vector<double>>& data) {
    m_features = data;
    saveData("features.csv", "FEATURE");
    cout << "Save Feature.csv" << endl;
}

void DensityCluster::generateFeatures(int row, int col) {
    clock_t t;
    t = clock();

    std::default_random_engine generator;
    std::normal_distribution<double> distribution01(0.0,1.0);
    std::normal_distribution<double> distribution02(10.0,1.0);
    std::normal_distribution<double> distribution03(-10.0,1.0);

    m_features.resize(row, vector<double>(col,0.0));
    m_realClassType.resize(row, 0);
    int curClass = 0;
    double number = 0.0;
    for(int k=0; k<row; ++k) {
        for(int i=0; i<col; ++i) {
            if(k / (row/3) == 0) {
                number = distribution01(generator);
                curClass = 1;
            } else if(k / (row/3) == 1) {
                number = distribution02(generator);
                curClass = 2;
            } else {
                number = distribution03(generator);
                curClass = 3;
            }
            m_features[k][i] = number;
        }
        m_realClassType[k] = curClass;
    }

    t = clock() - t;
    cout << "generateFeatures took me " << t << " clicks (" << ((float)t)/CLOCKS_PER_SEC << " seconds)." << endl;
    saveData("features.csv", "FEATURE");
}

void DensityCluster::saveData(string filename, string saveType) {
    ofstream out(saveprefix + filename, ofstream::out);
    if(saveType.compare("FEATURE") == 0) {
        for(int i=0;i<m_features.size();++i) {
            for(int j=0;j<m_features[i].size()-1;++j) {
                out << m_features[i][j] << ",";
            }
            out << m_features[i][m_features[i].size()-1] << endl;
        }
    }
    if(saveType.compare("CLASSTYPE") == 0) {
        out << "AlgorithmClass" << endl;
        for(int i=0;i<m_classType.size();++i) {
            out << m_classType[i] << endl;
        }
    }
    out.close();
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

double DensityCluster::getDCDist(double dc_percent, double &maxd) {
    clock_t t;
    t = clock();

    int row = m_features.size();
    double avgNeighbourNum = row * dc_percent;

    double dc = 0.0;
    vector<double> dis;
    for(int i=0;i<row;++i) {
        for(int j=i+1;j<row;++j) {
            double d = 0.0;
            if(isUseDistMatrix)
                d = m_distMatrix[i][j];
            else
                d = m_distMap[make_pair(i,j)];
            dis.push_back(d);
        }
    }
    sort(dis.begin(), dis.end());
    dc = dis[int(avgNeighbourNum * row)];
    maxd = dis[dis.size()-1];

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
    cout << "Finished Find Density !" << endl;
}

void DensityCluster::findDistanceToHigherDensity(double dc, double maxd) {
    int row = m_features.size();
    m_density_pair.resize(row, make_pair(0,0));
    for(int i=0;i<m_density_pair.size();i++) {
        m_density_pair[i] = make_pair(m_density[i], i);
    }
    sort(m_density_pair.begin(), m_density_pair.end(), [](const pair<int, int> &left, const pair<int, int> &right) {
        return left.first > right.first;
    });

    m_minDist2Higher.resize(row, maxd);
    m_nearestNeighbor.resize(row, -1);

    for(int i=1; i<row; ++i) {
        int ind_pos1 = m_density_pair[i].second;
        for(int j=0;j<i;++j) {
            int ind_pos2 = m_density_pair[j].second;
            double tmp_dist = 0.0;
            if(isUseDistMatrix) {
                tmp_dist = m_distMatrix[ind_pos1][ind_pos2];
            } else {
                tmp_dist = m_distMap[make_pair(ind_pos1>ind_pos2?ind_pos2:ind_pos1, ind_pos1>ind_pos2?ind_pos1:ind_pos2)];
            }
            if(tmp_dist < m_minDist2Higher[ind_pos1]) {
                m_minDist2Higher[ind_pos1] = tmp_dist;
                m_nearestNeighbor[ind_pos1] = ind_pos2;
            }
        }
    }
    cout << "Finished findDistanceToHigherDensity !" << endl;
}

void DensityCluster::findClusterCentersByRatio(double ratio) {
    int total_len = m_minDist2Higher.size();
    vector<pair<int,double>> tmp;
    for(int i=0;i<total_len;++i) {
        tmp.push_back(make_pair(i, m_minDist2Higher[i] * m_density[i]));
//        cout << m_density[i] << "," << m_minDist2Higher[i] << endl;
    }

    sort(tmp.begin(), tmp.end(), [](pair<int, double>&left, pair<int, double>& right) {
        return left.second > right.second;
    });

    int selectInd = total_len * ratio;
    for(int i=0; i<total_len; ++i) {
        if(i <= selectInd && tmp[i].second) {
            m_centers.push_back(tmp[i].first);
        }
    }
}

void DensityCluster::findClusterCenters() {
    int total_len = m_minDist2Higher.size();
    vector<pair<int,double>> tmp;
    for(int i=0;i<total_len;++i) {
        tmp.push_back(make_pair(i, m_minDist2Higher[i] * m_density[i]));
    }

    sort(tmp.begin(), tmp.end(), [](pair<int, double>&left, pair<int, double>& right) {
        return left.second > right.second;
    });

    vector<int> diff1;
    int split_index = 0;
    diff1.resize(total_len, 0);
    double total_sum = 0.0;
    for(int i=0;i<total_len-1;++i) {
        diff1[i] = (int)(tmp[i].second - tmp[i+1].second);
        total_sum += diff1[i];
    }
    double prefix_sum = diff1[0];
    for(int i=1;i<diff1.size();++i) {
        prefix_sum += diff1[i];
        if(prefix_sum / total_sum >= 0.95) {
            split_index = i;
            break;
        }
    }
    cout << "Find Center are :" << endl;
    for(int i=0;i<=split_index;++i) {
        m_centers.push_back(tmp[i].first);
//        cout << tmp[i].first << endl;
    }
    ofstream out(saveprefix + "desiciontree.csv", std::ofstream::out);
    out << "m_minDist2Higher,m_density,PointInd,Poduct" << endl;
    for(int i=0;i<tmp.size();++i) {
        out << m_minDist2Higher[i] << "," << m_density[i] << "," << tmp[i].first << "," << tmp[i].second << endl;
    }
    out.close();
}

void DensityCluster::classifyFeatures2Centers() {
    m_classType.resize(m_features.size(), -1);
    for(int i=0;i<m_centers.size();++i) {
        m_classType[m_centers[i]] = i+1;
    }
    for(int i=0;i<m_density_pair.size();++i) {
        int ind = m_density_pair[i].second;
        if(m_classType[ind] == -1 && m_classType[m_nearestNeighbor[ind]] != -1) {
            m_classType[ind] = m_classType[m_nearestNeighbor[ind]];
        }
    }
    saveData("classifyType.csv", "CLASSTYPE");
}