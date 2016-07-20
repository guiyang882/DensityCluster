//
// Created by 贵阳 on 6/17/16.
//

#include "densitycluster.h"

DensityCluster::DensityCluster() {
    saveprefix = "/Users/liuguiyang/Documents/CodeProj/ConsoleProj/DensityCluster/data/";
}

DensityCluster::~DensityCluster() {

}

void DensityCluster::initFeaturesLocal(string filename) {
    ifstream in(filename, std::ifstream::in);
    while(in.good()) {
        string strFeature;
        getline(in, strFeature);
        vector<double> vals;
        splitString(strFeature, vals, ',');
        if(vals.size() > 0) {
            m_features.push_back(vals);
        }
    }
    in.close();
    cout << "features size is (" << m_features.size() << "," << m_features[0].size() << ")" << endl;
}

void DensityCluster::splitString(string str, vector<double> &res, char ch) {
    //trim the free space from the head and tail
    str.erase(0,str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    if(str.size() == 0) return ;
    int start=0, end=0;
    while(start <= end && end < str.size()) {
        while(end<str.size() && str[end]!=ch) end++;
        string sub = str.substr(start, end-start);
        res.push_back(stod(sub, 0));
        while(end<str.size() && str[end]==ch) end++;
        start = end;
    }
}

void DensityCluster::generateFeatures(int row, int col) {
    clock_t t;
    t = clock();

    std::default_random_engine generator;
    std::normal_distribution<double> distribution01(0.0,1.0);
    std::normal_distribution<double> distribution02(10.0,1.0);
    std::normal_distribution<double> distribution03(-10.0,1.0);

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
    if(saveType.compare("CENTER") == 0) {
        for(int i=0;i<m_centers.size();++i) {
            out << m_centers[i] << endl;
        }
    }
    if(saveType.compare("RESULT") == 0) {
        for(auto &kv : m_result) {
            out << kv.first;
            for(auto &val : kv.second) {
                out << "," << val;
            }
            out << endl;
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
    /* 使用掺入排序进行数据插入,并且按照从大到小进行排序 */
    int sorted_index = 0;
    m_density_pair[0] = make_pair(m_density[0], 0);
    for(int i=1;i<row;++i) {
        pair<int, int> tmp = make_pair(m_density[i], i);
        if(sorted_index == 0) {
            if(tmp.first > m_density_pair[0].first) {
                m_density_pair[1] = m_density_pair[0];
                m_density_pair[0] = tmp;
            } else {
                m_density_pair[1] = tmp;
            }
            sorted_index++;
        } else {
            int insertPos = -1;
            for(int k=1;k<=sorted_index;++k) {
                if(m_density_pair[k-1].first <= tmp.first) {
                    insertPos = 0;
                    break;
                }else if(m_density_pair[k-1].first > tmp.first && m_density_pair[k].first < tmp.first) {
                    insertPos = k;
                    break;
                }
            }
            if(insertPos == -1) {
                m_density_pair[sorted_index+1] = tmp;
                sorted_index++;
            } else {
                for(int j=sorted_index;j>=insertPos;--j) {
                    m_density_pair[j+1] = m_density_pair[j];
                }
                m_density_pair[insertPos] = tmp;
                sorted_index++;
            }
        }
    }

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

void DensityCluster::findClusterCenters(double ratio) {
    int total_len = m_minDist2Higher.size();
    vector<pair<int,double>> tmp;
    for(int i=0;i<total_len;++i) {
        tmp.push_back(make_pair(i, m_minDist2Higher[i] * m_density[i]));
        cout << m_density[i] << "," << m_minDist2Higher[i] << endl;
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

    for(int i=0; i<total_len; ++i) {
        cout << tmp[i].first << "," << tmp[i].second << endl;
    }
    saveData("centers.csv", "CENTER");
    cout << "Save Centers Data !" << endl;
}

