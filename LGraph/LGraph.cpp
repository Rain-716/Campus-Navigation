#include <algorithm>
#include "LGraph.h"

namespace Graph
{
    bool LGraph::ExistVertex(const std::string& name) const
    {
        return ver_map.find(name)!=ver_map.end();
    }

    bool LGraph::ExistEdge(const std::string& u,const std::string& v) const
    {
        auto it_u=ver_map.find(u);
        auto it_v=ver_map.find(v);
        if (it_u==ver_map.end()||it_v==ver_map.end()){
            return false;
        }
        Vertex uid=it_u->second;
        Vertex vid=it_v->second;
        for (const Edge& e : ver_list[uid].adj){
            if (e.to==vid){
                return true;
            }
        }
        return false;
    }

    void LGraph::InsertVertex(const LocationInfo& vertexInfo)
    {
        if (ExistVertex(vertexInfo.name)){
            throw GraphException("顶点"+vertexInfo.name+"已存在");
        }
        ver_list.emplace_back(vertexInfo);
        ver_map[vertexInfo.name]=vertNum++;
    }

    void LGraph::DeleteVertex(const std::string& name)
    {
        auto it=ver_map.find(name);
        if (it==ver_map.end()){
            throw GraphException("顶点"+name+"不存在");
        }
        Vertex id=it->second;
        for (Edge& e : ver_list[id].adj){
            std::list <Edge>& adjList=ver_list[e.to].adj;
            adjList.remove_if([id](const Edge& ed){ return ed.to==id; });
            edgeNum--;
        }
        ver_list.erase(ver_list.begin()+id);
        ver_map.erase(it);
        vertNum--;
        for (auto& [key,val] : ver_map){    // 更新映射中所有 ID > id 的值
            if (val>id){
                val--;
            }
        }
        for (VertexNode& v : ver_list){     // 更新所有剩余边的 from/to
            for (Edge& e : v.adj){
                if (e.from>id){
                    e.from--;
                }
                if (e.to>id){
                    e.to--;
                }
            }
        }
    }

    void LGraph::UpdateVertex(const std::string& oldName,const LocationInfo& newInfo)
    {
        auto it=ver_map.find(oldName);
        if (it==ver_map.end()){
            throw GraphException("顶点"+oldName+"不存在");
        }
        ver_list[it->second].info=newInfo;
    }

    LocationInfo LGraph::GetVertex(const std::string& name) const
    {
        auto it=ver_map.find(name);
        if (it==ver_map.end()){
            throw GraphException("顶点"+name+"不存在");
        }
        return ver_list[it->second].info;
    }

    LocationInfo LGraph::GetVertex(Vertex vertex) const
    {
        if (vertex>=vertNum){
            throw GraphException("顶点ID越界: "+std::to_string(vertex));
        }
        return ver_list[vertex].info;
    }

    void LGraph::InsertEdge(const std::string& u,const std::string& v,EWeight weight)
    {
        auto it_u=ver_map.find(u);
        auto it_v=ver_map.find(v);
        if (it_u==ver_map.end()||it_v==ver_map.end()){
            throw GraphException("插入边时，顶点不存在");
        }
        if (ExistEdge(u,v)){
            throw GraphException("边"+u+" - "+v+"已存在");
        }
        Vertex uid=it_u->second;
        Vertex vid=it_v->second;
        ver_list[uid].adj.emplace_back(uid,vid,weight);
        ver_list[vid].adj.emplace_back(vid,uid,weight);
        edgeNum++;
    }

    void LGraph::DeleteEdge(const std::string& u,const std::string& v)
    {
        auto it_u=ver_map.find(u);
        auto it_v=ver_map.find(v);
        if (it_u==ver_map.end()||it_v==ver_map.end()){
            throw GraphException("删除边时，顶点不存在");
        }
        if (!ExistEdge(u,v)){
            throw GraphException("要删除的边"+u+" - "+v+"不存在");
        }
        Vertex uid=it_u->second;
        Vertex vid=it_v->second;
        DeleteEdge(uid,vid);
        edgeNum--;
    }

    void LGraph::DeleteEdge(Vertex u,Vertex v)
    {
        std::list <Edge>& adj_u=ver_list[u].adj;
        adj_u.remove_if([v](const Edge& e) { return e.to==v; });
        std::list <Edge>& adj_v=ver_list[v].adj;
        adj_v.remove_if([u](const Edge& e) { return e.to==u; });
    }

    void LGraph::UpdateEdge(const std::string& u,const std::string& v,EWeight newWeight)
    {
        auto it_u=ver_map.find(u);
        auto it_v=ver_map.find(v);
        if (it_u==ver_map.end()||it_v==ver_map.end()){
            throw GraphException("更新边时，顶点不存在");
        }
        Vertex uid=it_u->second;
        Vertex vid=it_v->second;
        bool found=false;
        for (Edge& e : ver_list[uid].adj){
            if (e.to==vid){
                e.weight=newWeight;
                found=true;
                break;
            }
        }
        if (!found){
            throw GraphException("要更新的边"+u+" - "+v+"不存在");
        }
        for (Edge& e : ver_list[vid].adj){
            if (e.to==uid){
                e.weight=newWeight;
                break;
            }
        }
    }

    EWeight LGraph::GetEdge(const std::string& u,const std::string& v) const
    {
        auto it_u=ver_map.find(u);
        auto it_v=ver_map.find(v);
        if (it_u==ver_map.end()||it_v==ver_map.end()){
            throw GraphException("查询边时，顶点不存在");
        }
        Vertex uid=it_u->second;
        Vertex vid=it_v->second;
        for (const Edge& e : ver_list[uid].adj){
            if (e.to==vid){
                return e.weight;
            }
        }
        throw GraphException("要查询的边"+u+" - "+v+"不存在");
    }

    std::vector<Edge> LGraph::SortedEdges(std::function<bool(const EWeight&,const EWeight&)> cmp) const
    {
        std::vector <Edge> edges;
        edges.reserve(edgeNum);
        for (Vertex u=0;u<vertNum;u++){
            for (const Edge& e : ver_list[u].adj){
                if (u<e.to){
                    edges.push_back(e);
                }
            }
        }
        std::sort(edges.begin(),edges.end(),[&](const Edge& e1,const Edge& e2){ return e1.weight>e2.weight; });
        return edges;
    }
}