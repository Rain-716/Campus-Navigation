#ifndef CAMPUSNAVIGATION_ALGORITHM_H
#define CAMPUSNAVIGATION_ALGORITHM_H

#include <list>
#include <string>
#include <vector>
#include "LGraph/LGraph.h"
#include "GraphException.h"

namespace Graph
{
    namespace Algorithm
    {
        class DSU   // 并查集
        {
            private:
                std::vector <Vertex> parent;
            public:
                explicit DSU (Vertex n);
                Vertex Find (Vertex x);
                bool Union (Vertex x,Vertex y);
        };

        // 判断图是否连通
        bool IsConnected(const LGraph& graph);

        // 判断是否存在欧拉回路（所有顶点度为偶数且连通）
        bool ExistEulerCircuit(const LGraph& graph);

        // 计算欧拉回路，返回顶点访问顺序列表，若不存在则返回空列表
        std::list<Vertex> EulerCircuit(const LGraph& graph,const Vertex& start);

        // 单源最短路径，返回顶点 x 到 y 的最短距离，不可达返回 -1（使用 Dijkstra 算法）
        int GetShortestPath(const LGraph& graph,const std::string& vertex_x_name,const std::string& vertex_y_name);

        // 拓扑受限最短路径，输入一系列顶点名称，依序计算前后两点的最短路径并累加
        int TopologicalShortestPath(const LGraph& graph,const std::vector<std::string>& path);

        // Kruskal 算法计算最小生成树，返回组成 MST 的边列表，无则返回空
        std::vector<Edge> MinimumSpanningTree(const LGraph& graph);
    }
}

#endif // CAMPUSNAVIGATION_ALGORITHM_H