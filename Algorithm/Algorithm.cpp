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
            for (Vertex u=0;u<n;u++){                       // 只处理 u<v 的边，避免重复
                for (const Edge& e : graph.List()[u].adj){
                    if (u<e.to){
                        dsu.Union(u,e.to);
                    }
                }
            }
            Vertex root=dsu.Find(0);
            for (int i=0;i<n;i++){
                if (graph.List()[i].adj.empty()||dsu.Find(i)!=root){
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
            for (const VertexNode& v : graph.List()){
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
            if (!n){
                return {};
            }
            if (start>=n){
                throw GraphException("起始顶点ID越界");
            }
            std::vector<std::vector<std::pair<Vertex,size_t>>> adj(n);
            size_t edgeId=0;
            for (Vertex u=0;u<n;u++){
                for (const Edge& e : graph.List()[u].adj){
                    if (u<e.to){
                        adj[u].emplace_back(e.to,edgeId);
                        adj[e.to].emplace_back(u,edgeId);
                        edgeId++;
                    }
                }
            }
            std::vector <bool> used(2*edgeId,false);
            std::vector <size_t> idx(n,0);
            std::stack <Vertex> stk;
            std::list <Vertex> res;
            stk.push(start);
            while (!stk.empty()){
                Vertex u=stk.top();
                while (idx[u]<adj[u].size()&&used[2*adj[u][idx[u]].second+(adj[u][idx[u]].first<u)]){
                    idx[u]++;
                }
                if (idx[u]==adj[u].size()){
                    stk.pop();
                    res.push_back(u);
                }
                else {
                    auto [v,eid]=adj[u][idx[u]];
                    used[2*eid]=used[2*eid+1]=true;
                    stk.push(v);
                }
            }
            res.reverse();
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
            for (Vertex u=0;u<n;u++){                               // 只收集 u<v 的那半边
                for (const Edge& e : graph.List()[u].adj){
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
            return res.size()==n-1 ? res : std::vector<Edge>{};
        }

        bool ExistEulerPath(const LGraph& graph)
        {
            Vertex n=graph.VertexCount();
            if (!n){
                return true;
            }
            if (!IsConnected(graph)){
                return false;
            }
            int odd=0;
            for (const VertexNode& v : graph.List()){
                if (v.adj.size()%2){
                    odd++;
                }
            }
            return odd==0||odd==2;
        }

        std::pair<int, std::vector<std::string>> ShortestPathwithTrace(const LGraph& graph, const std::string& xName, const std::string& yName)
        {
            if (!graph.ExistVertex(xName)||!graph.ExistVertex(yName)){
                return {-1,{}};
            }
            const std::map<std::string,Vertex>& map=graph.Map();
            Vertex xid=map.at(xName),yid=map.at(yName);
            int n=graph.VertexCount();
            const std::vector<VertexNode>& list=graph.List();
            const long long INF=std::numeric_limits<long long>::max();
            std::vector <long long> dist(n,INF);
            std::vector <int> prev(n,-1);
            dist[xid]=0;
            std::priority_queue<std::pair<long long,size_t>,std::vector<std::pair<long long,size_t>>,std::greater<std::pair<long long,size_t>>> pq;
            pq.push({0,xid});
            while (!pq.empty()){
                auto [d,u]=pq.top();
                pq.pop();
                if (d>dist[u]){
                    continue;
                }
                if (u==yid){
                    break;
                }
                for (const Edge& e : list[u].adj){
                    Vertex v=e.to;
                    long long nd=d+e.weight;
                    if (nd<dist[v]){
                        dist[v]=nd;
                        prev[v]=u;
                        pq.push({nd,v});
                    }
                }
            }
            if (dist[yid]==INF){
                return {-1,{}};
            }
            std::vector <std::string> path;
            for (size_t to=yid;to!=-1;to=prev[to]){
                path.push_back(graph.GetVertex(to).name);
            }
            std::reverse(path.begin(),path.end());
            return {(int)dist[yid],path};
        }
    }
}