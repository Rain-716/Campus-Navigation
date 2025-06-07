#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>
#include <limits>
#include "LGraph/LGraph.h"
#include "Algorithm/Algorithm.h"
#include "LocationInfo.h"
#include "GraphException.h"

using namespace Graph;
using namespace Graph::Algorithm;

static const std::string nodes_path="data/nodes.csv";
static const std::string edges_path="data/edges.csv";

// 工具函数声明
std::vector<LocationInfo> ReadNodes(const std::string& path);
std::vector<std::tuple<std::string,std::string,int>> ReadEdges(const std::string& path);
void init(LGraph& graph);
void ShowAllNodes(const LGraph& graph);
void ShowAllEdges(const LGraph& graph);
void StoreNodes(const std::string& path,const LGraph& graph);
void StoreEdges(const std::string& path,const LGraph& graph);

int main()
{
    LGraph graph;
    try { init(graph); }
    catch (const GraphException& e){
        std::cerr<<"初始化失败: "<<e.what()<<std::endl;
        return -1;
    }

    while (true)
    {
        std::cout<<R"(欢迎使用校园导航系统！
        请选择操作：
        1. 顶点操作
        2. 边操作
        3. 重新加载数据
        4. 判断欧拉回路
        5. 最短距离
        6. 最小生成树
        7. 拓扑受限路径
        8. 退出 )";
        int choice;
        if (!(std::cin>>choice)){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            continue;
        }

        try {
            switch (choice)
            {
                case 1:
                {
                    std::cout<<"顶点操作：1.查询 2.列出所有 3.添加 4.删除 5.保存 6.返回\n";
                    int sub;
                    std::cin>>sub;
                    switch (sub)
                    {
                        case 1:
                        {
                            std::cout<<"名称：";
                            std::string name;
                            std::cin>>name;
                            LocationInfo info=graph.GetVertex(name);
                            std::cout<<"顶点名称："<<info.name
                                     <<"顶点类型："<<info.type
                                     <<"建议游览时间："<<info.visitTime<<" 分钟\n";
                            break;
                        }
                        case 2:
                        {
                            std::cout<<"所有顶点信息：\n";
                            ShowAllNodes(graph);
                            break;
                        }
                        case 3:
                        {
                            std::cout<<"输入 名称 类型 时间：";
                            std::string name,type;
                            int visitTime;
                            std::cin>>name>>type>>visitTime;
                            graph.InsertVertex({name,type,visitTime});
                            break;
                        }
                        case 4:
                        {
                            std::cout<<"名称：";
                            std::string name;
                            std::cin>>name;
                            graph.DeleteVertex(name);
                            break;
                        }
                        case 5:
                        {
                            std::cout<<"正在将顶点存储到 test_nodes.csv ...\n";
                            StoreNodes("test_nodes.csv",graph);
                            std::cout<<"存储成功！\n";
                            break;
                        }
                        case 6:
                            break;
                        default:
                            std::cout<<"无效选项，请重试。\n";
                    }
                    break;
                }
                case 2:
                {
                    int sub;
                    std::cout<< "边操作：1.查询 2.列出所有 3.添加 4.删除 5.保存 6.返回\n";
                    std::cin>>sub;
                    switch (sub)
                    {
                        case 1:
                        {
                            std::cout<<"请输入边的起点与终点：";
                            std::string u,v;
                            std::cin>>u>>v;
                            std::cout<<u<<" <---> "<<v<<" 距离为："<<graph.GetEdge(u,v)<<"\n";
                            break;
                        }
                        case 2:
                        {
                            std::cout<<"所有边信息（起点,终点,权重）：\n";
                            ShowAllEdges(graph);
                            break;
                        }
                        case 3:
                        {
                            std::cout<<"请输入起点名称、终点名称、距离：";
                            std::string u,v;
                            int w;
                            std::cin>>u>>v>>w;
                            graph.InsertEdge(u,v,w);
                            break;
                        }
                        case 4:
                        {
                            std::cout<<"请输入起点名称、终点名称：";
                            std::string u,v;
                            std::cin>>u>>v;
                            graph.DeleteEdge(u,v);
                            break;
                        }
                        case 5:
                        {
                            std::cout<<"正在将边存储到 test_edges.csv ...\n";
                            StoreEdges("test_edges.csv",graph);
                            std::cout<<"存储成功！\n";
                            break;
                        }
                        case 6:
                            break;
                        default:
                            std::cout<<"无效选项，请重试。\n";
                    }
                    break;
                }
                case 3:
                {
                    init(graph);
                    std::cout<<"已重新加载点与边。\n";
                    break;
                }
                case 4:
                {
                    std::cout<<(ExistEulerCircuit(graph) ? "存在欧拉回路" : "不存在欧拉回路")<<"\n";
                    break;
                }
                case 5:
                {
                    std::cout<<"请输入两个地点（空格分隔）：";
                    std::string x,y;
                    std::cin>>x>>y;
                    int d=GetShortestPath(graph,x,y);
                    if (d<0){
                        std::cout<<"两点不可达或不存在顶点。\n";
                    }
                    else{
                        std::cout<<x<<" 和 "<<y<<" 之间的最短距离为："<<d<<"\n";
                    }
                    break;
                }
                case 6:
                {
                    if (!IsConnected(graph)){
                        std::cout<<"图不连通，无法构造最小生成树。\n";
                    }
                    else{
                        std::vector <Edge> mst=MinimumSpanningTree(graph);
                        if (mst.empty()){
                            std::cout<<"图无法形成生成树（可能存在孤立顶点）。\n";
                        }
                        else {
                            int sum=0;
                            std::cout<<"最小生成树的边如下（起点,终点,权重）：\n";
                            for (const Edge& e : mst){
                                std::cout<<graph.GetVertex(e.from).name<<" - "<<graph.GetVertex(e.to).name<<" : "<<e.weight<<"\n";
                                sum+=e.weight;
                            }
                            std::cout<<"总权重="<<sum<<"\n";
                        }
                    }
                    break;
                }
                case 7:
                {
                    std::cout<<"请输入拓扑序：第一行 n，第二行 n 个地点名称（空格分隔）\n";
                    int n;
                    std::cin>>n;
                    if (n<=0){
                        std::cout<<"拓扑序长度必须大于 0。\n";
                        break;
                    }
                    std::vector <std::string> seq(n);
                    for (std::string& s : seq){
                        std::cin>>s;
                    }
                    int d=TopologicalShortestPath(graph,seq);
                    if (d<0){
                        std::cout<<"路径不可达或存在不存在的顶点。\n";
                    }
                    else{
                        std::cout<<"拓扑受限最短路径总距离="<<d<<"\n";
                    }
                    break;
                }
                case 8:
                {
                    std::cout<<"感谢使用，再见！开发者：Rain\n";
                    return 0;
                }
                default:
                    std::cout<<"无效选项，请输入 1-8 之间的数字。\n";
            }
        }
        catch (const GraphException& e){
            std::cerr<<"操作失败: "<<e.what()<<"\n";
        }
        catch (const std::exception& e){
            std::cerr<<"未知错误: "<<e.what()<<"\n";
        }
    }
    return 0;
}

// 读取节点文件
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
        std::istringstream sin(line);
        std::string name,type;
        int visitTime;
        if (!std::getline(sin,name,',')){
            continue;
        }
        if (!std::getline(sin,type,',')){
            continue;
        }
        if (!(sin>>visitTime)){
            continue;
        }
        nodes.emplace_back(name,type,visitTime);
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
        std::istringstream sin(line);
        std::string from,to;
        int weight;
        if (!std::getline(sin,from,',')){
            continue;
        }
        if (!std::getline(sin,to,',')){
            continue;
        }
        if (!(sin>>weight)){
            continue;
        }
        edges.push_back({from,to,weight});
    }
    return edges;
}

void init(LGraph& graph)        // 初始化图：从文件加载节点与边
{
    graph=LGraph();             // 重置为新图
    std::vector <LocationInfo> nodes=ReadNodes(nodes_path);
    std::vector<std::tuple<std::string,std::string,int>> edges=ReadEdges(edges_path);
    for (const LocationInfo& v : nodes){
        graph.InsertVertex(v);
    }
    for (std::tuple<std::string,std::string,int>& e : edges){
        const auto& [u,v,w]=e;
        graph.InsertEdge(u,v,w);
    }
}

void ShowAllNodes(const LGraph& graph)      // 显示所有顶点
{
    for (const VertexNode& v : graph.List()){
        const LocationInfo& info=v.info;
        std::cout<<info.name<<","<<info.type<<","<<info.visitTime<<std::endl;
    }
}

void ShowAllEdges(const LGraph& graph)      // 显示所有边
{
    std::vector <Edge> edges=graph.SortedEdges();
    for (const Edge& e : edges){
        const std::string& name_u=graph.GetVertex(e.from).name;
        const std::string& name_v=graph.GetVertex(e.to).name;
        std::cout<<name_u<<","<<name_v<<","<<e.weight<<std::endl;
    }
}

void StoreNodes(const std::string& path,const LGraph& graph)    // 将顶点存储到文件
{
    std::ofstream fout(path);
    if (!fout){
        throw GraphException("无法创建文件: "+path);
    }
    for (const VertexNode& v : graph.List()){
        const LocationInfo& info=v.info;
        fout<<info.name<<","<<info.type<<","<<info.visitTime<<"\n";
    }
}

void StoreEdges(const std::string& path,const LGraph& graph)    // 将边存储到文件
{
    std::ofstream fout(path);
    if (!fout){
        throw GraphException("无法创建文件: "+path);
    }
    std::vector<Edge> edges=graph.SortedEdges();
    for (const Edge& e : edges){
        std::string name_u=graph.GetVertex(e.from).name;
        std::string name_v=graph.GetVertex(e.to).name;
        fout<<name_u<<","<<name_v<<","<<e.weight<<"\n";
    }
}