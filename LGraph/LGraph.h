#ifndef LGRAPH_LGRAPH_H
#define LGRAPH_LGRAPH_H

#include <list>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include "LocationInfo.h"
#include "GraphException.h"

namespace Graph
{
    using Vertex=size_t; // 顶点 ID 类型
    using EWeight=int;   // 边权类型

    struct Edge
    {
        Vertex from,to;
        EWeight weight;
        Edge(Vertex f,Vertex t,EWeight w) noexcept : from(f),to(t),weight(w) {}
    };

    struct VertexNode
    {
        std::list <Edge> adj;
        LocationInfo info;
        explicit VertexNode(const LocationInfo& i) : adj(),info(i) {}
    };

    class LGraph
    {
        private:
            size_t vertNum=0;      // 顶点数
            size_t edgeNum=0;      // 边数（无向图中每条边只记一次）
            std::vector <VertexNode> ver_list;
            std::map <std::string,Vertex> ver_map;

        public:
            LGraph()=default;
            ~LGraph()=default;

            size_t VertexCount() const noexcept { return vertNum; }     // 顶点数量
            size_t EdgesCount() const noexcept { return edgeNum; }      // 边数量（单向）

            bool ExistVertex(const std::string& name) const;                    // 是否存在顶点
            bool ExistEdge(const std::string& u,const std::string& v) const;    // 是否存在边

            void InsertVertex(const LocationInfo& vertexInfo);                              // 插入顶点
            void DeleteVertex(const std::string& name);                                     // 删除顶点（通过名称）
            void UpdateVertex(const std::string& oldName,const LocationInfo& newInfo);      // 更新顶点信息（名称不变）
            LocationInfo GetVertex(const std::string& name) const;                          // 通过名称查询顶点信息
            LocationInfo GetVertex(Vertex vertex) const;                                    // 通过顶点 ID 查询顶点信息

            void InsertEdge(const std::string& u,const std::string& v,EWeight weight);      // 插入边（无向）
            void DeleteEdge(const std::string& u,const std::string& v);                     // 通过名称删除边
            void DeleteEdge(Vertex u,Vertex v);                                             // 通过顶点 ID 删除边
            void UpdateEdge(const std::string& u,const std::string& v,EWeight newWeight);   // 更新边权
            EWeight GetEdge(const std::string& u,const std::string& v) const;               // 查询边权

            const std::vector<VertexNode>& List() const noexcept { return ver_list; }        // 返回邻接表（非const）
            std::vector<VertexNode>& List() noexcept { return ver_list; }                    // 返回邻接表（const）
            const std::map<std::string,Vertex>& Map() const noexcept { return ver_map; }     // 返回名称到 ID 的映射（非const）
            std::map<std::string,Vertex>& Map() noexcept { return ver_map; }                 // 返回名称到 ID 的映射（const）

            // 返回按权重排序后的所有无向边（只保留 u < v 的那一半）
            std::vector<Edge> SortedEdges(std::function<bool(const EWeight&,const EWeight&)> cmp=std::less<>()) const;
        };
}

#endif // LGRAPH_LGRAPH_H