#include <queue>
#include <limits>
#include <algorithm>
#include <stack>
#include "Algorithm.h"

namespace Graph
{
    namespace Algorithm
    {
        Vertex DSU::Find(Vertex x)
        {
            return x==parent[x] ? x : parent[x]=Find(parent[x]);
        }

        bool DSU::Union(Vertex x,Vertex y) noexcept
        {
            x=Find(x);
            y=Find(y);
            if (x==y){
                return false;
            }
            parent[y]=x;
            return true;
        }

        bool IsConnected(const LGraph& graph) noexcept   // 判断连通性
        {
            size_t n=graph.VertexCount();
            if (!n){
                return true;
            }
            DSU dsu (n);
            const std::vector <VertexNode>& ver_list=graph.List();
            for (Vertex u=0;u<n;u++){                       // 只处理 u<v 的边，避免重复
                for (const Edge& e : ver_list[u].adj){
                    if (u<e.to){
                        dsu.Union(u,e.to);
                    }
                }
            }
            Vertex root=dsu.Find(0);
            for (int i=0;i<n;i++){
                if (ver_list[i].adj.empty()||dsu.Find(i)!=root){
                    return false;
                }
            }
            return true;
        }

        bool ExistEulerCircuit(const LGraph& graph) noexcept    // 判断是否存在欧拉回路
        {
            if (!IsConnected(graph)){
                return false;
            }
            const auto& ver_list=graph.List();
            for (const VertexNode& v : ver_list){
                if (v.adj.size()%2){
                    return false;
                }
            }
            return true;
        }

        std::list<Vertex> EulerCircuit(const LGraph& graph,Vertex start)     // 计算欧拉回路
        {
            if (!ExistEulerCircuit(graph)){
                return {};
            }
            size_t n=graph.VertexCount();
            const std::vector <VertexNode>& ver_list=graph.List();
            // 将每条无向边拆成两条有向边，编号为 0..2m-1
            // 用 vector <bool> 标记每条“有向边”是否已访问
            std::vector <std::vector<std::pair<Vertex,size_t>>> adj(n);
            size_t edgeId=0;
            for (Vertex u=0;u<n;u++){
                for (const Edge& e : ver_list[u].adj){
                    if (u<e.to){                                            // 只添加 u<v 的情况，对应两条有向边
                        adj[u].emplace_back(e.to,edgeId);
                        adj[e.to].emplace_back(u,edgeId);
                        edgeId++;
                    }
                }
            }
            std::vector <bool> used(2*edgeId,false);                    // 标记 2*edgeId 个“有向边”
            std::vector <size_t> idx(n,0);
            std::stack <Vertex> stk;
            std::list <Vertex> res;
            stk.push(start);
            while (!stk.empty()){
                Vertex u=stk.top();
                auto& it=idx[u];
                while (it<adj[u].size()&&used[2*adj[u][it].second+(adj[u][it].first<u ? 1 : 0)]){    // 寻找一条未访问的有向边
                    it++;
                }
                if (it==adj[u].size()){        // 没有可走的边时，输出顶点
                    stk.pop();
                    res.push_back(u);
                }
                else {
                    auto [v,eid]=adj[u][it];   // 标记对应的两条有向边已访问
                    used[2*eid]=true;
                    used[2*eid+1]=true;
                    stk.push(v);
                }
            }
            return res;
        }

        int GetShortestPath(const LGraph& graph,const std::string& xName,const std::string& yName)  // 单源最短路径（Dijkstra）
        {
            const std::map <std::string,Vertex>& ver_map=graph.Map();
            auto itx=ver_map.find(xName);
            auto ity=ver_map.find(yName);
            if (itx==ver_map.end()||ity==ver_map.end()){
                throw GraphException("顶点不存在");
            }
            Vertex xid=itx->second;
            Vertex yid=ity->second;
            int n=graph.VertexCount();
            const std::vector <VertexNode>& ver_list=graph.List();

            const long long INF=std::numeric_limits <long long>::max();
            std::vector <long long> dist(n,INF);
            dist[xid]=0;

            // 优先队列：<距离, 顶点>
            std::priority_queue<std::pair<long long,Vertex>,std::vector<std::pair<long long,Vertex>>,std::greater<>> pq;
            pq.push({0,xid});

            while (!pq.empty()){
                auto [d,u]=pq.top();
                pq.pop();
                if (d>dist[u]){
                    continue;
                }
                if (u==yid){
                    break;      // 提前退出
                }

                for (const Edge& e : ver_list[u].adj){
                    Vertex v=e.to;
                    long long plus=d+e.weight;
                    if (plus<dist[v]){
                        dist[v]=plus;
                        pq.push({plus,v});
                    }
                }
            }

            return dist[yid]==INF ? -1 : dist[yid];
        }

        int TopologicalShortestPath(const LGraph& graph,const std::vector<std::string>& path)   // 拓扑受限最短路径
        {
            if (path.empty()){
                return 0;
            }
            for (const std::string& name : path){       // 先检查所有顶点是否存在
                if (!graph.ExistVertex(name)){
                    throw GraphException("路径中包含不存在的顶点: " + name);
                }
            }
            int res=0;
            for (size_t i=0;i<path.size()-1;i++)
            {
                int d=GetShortestPath(graph,path[i],path[i+1]);
                if (d<0){
                    return -1;
                }
                res+=d;
            }
            return res;
        }

        std::vector<Edge> MinimumSpanningTree(const LGraph& graph)      // Kruskal 最小生成树
        {
            size_t n=graph.VertexCount();
            if (n<2){
                return {};
            }
            DSU dsu(n);
            std::vector <Edge> edges;
            edges.reserve(graph.EdgesCount());
            const std::vector <VertexNode>& ver_list=graph.List();
            for (Vertex u=0;u<n;u++){                               // 只收集 u<v 的那半边
                for (const Edge& e : ver_list[u].adj){
                    if (u<e.to){
                        edges.push_back(e);
                    }
                }
            }
            std::sort(edges.begin(),edges.end(),[](const Edge& a,const Edge& b){ return a.weight<b.weight; });
            std::vector <Edge> res;
            res.reserve(n-1);
            for (const Edge& e : edges){
                if (dsu.Union(e.from,e.to)){
                    res.push_back(e);
                    if (res.size()==n-1){
                        break;
                    }
                }
            }
            if (res.size()!=n-1){
                return {};
            }
            return res;
        }
    }
}