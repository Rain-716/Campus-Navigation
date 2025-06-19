#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>
#include <algorithm>
#include "LGraph/LGraph.h"
#include "Algorithm/Algorithm.h"
#include "LocationInfo.h"
#include "GraphException.h"

using namespace Graph;
using namespace Graph::Algorithm;

static const std::string nodes_path="data/nodes.csv";
static const std::string edges_path="data/edges.csv";
static const std::string command_path="cmd/command.txt";
static const std::string answer_path="cmd/answer.txt";

// 前置声明
std::vector<LocationInfo> ReadNodes(const std::string& path);
std::vector<std::tuple<std::string,std::string,int>> ReadEdges(const std::string& path);
void initGraph(LGraph& graph);

int main()
{
    LGraph graph;
    try { initGraph(graph); }
    catch (const GraphException& e){
        std::cerr<<"初始化失败: "<<e.what()<<std::endl;
        return -1;
    }

    std::ifstream cmdIn(command_path);
    std::ofstream ansOut(answer_path);
    if (!cmdIn||!ansOut){
        std::cerr<<"无法打开命令或答案文件"<<std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(cmdIn,line)){
        if (line.empty()){
            continue;
        }
        std::istringstream iss(line);
        std::string cmd;
        iss>>cmd;
        if (cmd=="SHORTEST_PATH"){
            std::string u,v;
            iss>>u>>v;
            auto [dist,path]=ShortestPathwithTrace(graph,u,v);
            if (dist<0){
                ansOut<<"NA"<<std::endl;
            }
            else {
                ansOut<<"DIST "<<dist<<" PATH";
                for (std::string& name : path){
                    ansOut<<" "<<name;
                }
                ansOut<<std::endl;
            }
        }
        else if (cmd=="ADJ_EDGES"){
            std::string u;
            iss>>u;
            if (!graph.ExistVertex(u)){
                ansOut<<"NONE"<<std::endl;
                continue;
            }
            Vertex uid=graph.Map()[u];
            const std::list <Edge>& adj=graph.List()[uid].adj;
            if (adj.empty()){
                ansOut<<"NONE"<<std::endl;
                continue;
            }
            std::vector <std::pair<std::string,int>> res;
            for (const Edge& e : adj) {
                res.emplace_back(graph.GetVertex(e.to).name,e.weight);
            }
            std::sort(res.begin(),res.end(),[](auto& a,auto& b){ return a.first<b.first; });
            bool first=true;
            for (std::pair<std::string,int>& p : res){
                if (!first){
                    ansOut<<" ";
                }
                first=false;
                ansOut<<p.first<<"("<<p.second<<")";
            }
            ansOut<<std::endl;
        }
        else if (cmd=="FIND_TYPE"){
            std::string type;
            iss>>type;
            bool first=true;
            for (VertexNode& vnode : graph.List()){
                if (vnode.info.type==type){
                    if (!first){
                        ansOut<<" ";
                    }
                    first=false;
                    ansOut<<vnode.info.name;
                }
            }
            ansOut<<std::endl;
        }
        else if (cmd=="INSERT_EDGE"){
            std::string u,v;
            int w;
            iss>>u>>v>>w;
            graph.InsertEdge(u,v,w);
            ansOut<<"OK"<<std::endl;
        }
        else if (cmd=="DELETE_EDGE"){
            std::string u,v;
            iss>>u>>v;
            graph.DeleteEdge(u,v);
            ansOut<<"OK"<<std::endl;
        }
        else if (cmd=="MODIFY_EDGE_WEIGHT"){
            std::string u,v;
            int w;
            iss>>u>>v>>w;
            graph.UpdateEdge(u,v,w);
            ansOut<<"OK"<<std::endl;
        }
        else if (cmd=="INSERT_NODE"){
            std::string name,type;
            int vt;
            iss>>name>>type>>vt;
            graph.InsertVertex(LocationInfo(name,type,vt));
            ansOut<<"OK"<<std::endl;
        }
        else if (cmd=="DELETE_NODE"){
            std::string name;
            iss>>name;
            graph.DeleteVertex(name);
            ansOut<<"OK"<<std::endl;
        }
        else if (cmd=="EULERIAN_PATH"){
            ansOut<<(ExistEulerPath(graph) ? "YES" : "NO")<<std::endl;
        }
        else if (cmd=="MST_INFO"){
            std::vector<Edge> edges=MinimumSpanningTree(graph);
            if (graph.VertexCount()>0&&edges.size()!=graph.VertexCount()-1){
                ansOut<<"DISCONNECTED"<<std::endl;
            }
            else {
                int total=0;
                std::vector <std::pair<std::string,std::string>> names;
                for (Edge& e : edges){
                    total+=e.weight;
                    names.emplace_back(graph.GetVertex(e.from).name,graph.GetVertex(e.to).name);
                }
                std::vector<Vertex> idx(names.size());
                for (Vertex i=0;i<idx.size();i++){
                    idx[i]=i;
                }
                std::sort(idx.begin(),idx.end(),[&](Vertex a,Vertex b) { return names[a].first<names[b].first; });
                ansOut<<"MST "<<total;
                for (Vertex& i : idx){
                    std::pair<std::string,std::string>& p=names[i];
                    int w=graph.GetEdge(p.first,p.second);
                    ansOut<<" "<<p.first<<"-"<<p.second<<":"<<w;
                }
                ansOut<<std::endl;
            }
        }
    }
    return 0;
}

std::vector<LocationInfo> ReadNodes(const std::string& path)
{
    std::ifstream fin(path);
    if (!fin){
        throw GraphException("无法打开节点文件: "+path);
    }
    std::vector <LocationInfo> nodes;
    std::string line;
    while (std::getline(fin,line)){
        if (line.empty()){
            continue;
        }
        std::istringstream iss(line);
        std::string name,type,vt_str;
        if (!std::getline(iss,name,',')){
            continue;
        }
        if (!std::getline(iss,type,',')){
            continue;
        }
        if (!std::getline(iss,vt_str,',')){
            continue;
        }
        nodes.emplace_back(name,type,std::stoi(vt_str));
    }
    return nodes;
}

std::vector<std::tuple<std::string,std::string,int>> ReadEdges(const std::string& path)
{
    std::ifstream fin(path);
    if (!fin){
        throw GraphException("无法打开边文件: "+path);
    }
    std::vector<std::tuple<std::string,std::string,int>> edges;
    std::string line;
    while (std::getline(fin,line)){
        if (line.empty()){
            continue;
        }
        std::istringstream iss(line);
        std::string u,v,w_str;
        if (!std::getline(iss,u,',')){
            continue;
        }
        if (!std::getline(iss,v,',')){
            continue;
        }
        if (!std::getline(iss,w_str,',')){
            continue;
        }
        edges.emplace_back(u,v,std::stoi(w_str));
    }
    return edges;
}

void initGraph(LGraph& graph)
{
    graph=LGraph();
    std::vector<LocationInfo> nodes=ReadNodes(nodes_path);
    for (LocationInfo& v : nodes){
        graph.InsertVertex(v);
    }
    std::vector<std::tuple<std::string,std::string,int>> edges=ReadEdges(edges_path);
    for (auto [u,v,w] : edges){
        graph.InsertEdge(u,v,w);
    }
}