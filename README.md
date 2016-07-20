# Density Cluster
使用基于密度的聚类算法,进行高维特征的聚类分析,从高维数据中提取出类似的有用信息,从而简化了特征数量,并且去除了部分冗余信息。
在聚类算法中,有这样几种算法:
- 划分的算法, K-Means
- 层次的方法, CURE
- 基于密度的算法, DBSCAN,DPCA(Desity Peaks Clusering Algorithm)
- 基于网格的算法, CLIQUE
- 基于模型的算法, 主要是一些概率的算法

由Alex Rodriguez和Alessandro Laio发表的《Clustering by fast search and find of density peaks》的主要思想是寻找被低密度区域分离的高密度区域。
基于这样的一种假设：
对于一个数据集，聚类中心被一些低局部密度的数据点包围，而且这些低局部密度的点距离其他有高局部密度的点的距离都比较大。

## 如何定义局部密度?
找到与某个数据点之间的距离小于截断距离的数据点的数量

## 如何寻找与高密度之间的距离?
- 找到所有比第i个数据点局部密度都打的数据点中,与第i个数据点之间的距离最小的值;
- 而对于有最大密度的数据点,通常取 $\sigma_i = max_{j}(d_{ij})$;

## 如何确定聚类中心、外点?
- DPCA中将那些具有较大距离$\sigma_i$,且同时具有较大局部密度的$\ro_i$的点定义为聚类中心。
- 同时具有较高的距离,但是密度却较小的数据点称为异常点。
- 根据论文中的决策图和乘积曲线去寻找潜在的聚类中心
  - 一条线中,去掉为零的部分,然后取出指定的前百分之几的数据即可
  - 将数据按照层次聚类,将曲线分层,找到可能的聚类中心

## Requirements
1. g++-4.7以上版本
2. 内存最好够大,因为至少要存储任意两个向量之间的距离
3. 使用libopm进行算法的并行化,提高运行效率

## 程序运行的框架
![算法的执行框架](https://github.com/DengZhuangSouthRd/DensityCluster/blob/master/images/flow.png =200x20)

## 程序运行展示
### 测试数据的分布
- 样本数据的展示
![原始测试数据的分布](https://github.com/DengZhuangSouthRd/DensityCluster/blob/master/images/features.png =200x200)
- 按照论文中的方法去寻找聚类中心
![按照论文中寻找聚类中心的结果图](https://github.com/DengZhuangSouthRd/DensityCluster/blob/master/images/findcenter.png)

## References
1. [Clustering by fast search and find of density peaks](http://people.sissa.it/~laio/Research/Res_clustering.php)
2. [Science论文"Clustering by fast search and find of density peaks"学习笔记](http://blog.csdn.net/jdplus/article/details/40351541)
3. [发表在 Science 上的一种新聚类算法](http://blog.csdn.net/itplus/article/details/38926837)
4. [超级赞的文章,写的很好!](http://blog.findshine.com/2014/08/12/fast-cluster.html)
5. [论文中的机器学习算法——基于密度峰值的聚类算法](http://blog.csdn.net/google19890102/article/details/37330471)
6. [Clustering datasets](http://cs.joensuu.fi/sipu/datasets/)