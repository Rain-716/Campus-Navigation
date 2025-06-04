#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>
#include "LGraph/LGraph.h"
#include "Algorithm/Algorithm.h"
#include "LocationInfo.h"
#include "GraphException.h"

using namespace Graph;
using namespace Graph::Algorithm;

static const std::string nodes_path="data/nodes.csv";
static const std::string edges_path="data/edges.csv";

// 读取节点文件
std::vector<LocationInfo> ReadNodes(const std::string& path)
{
    std::ifstream fin(path);
    if (!fin.is_open()){
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

struct edge     // 读取边文件
{
    std::string from,to;
    int length;
};

std::vector<edge> ReadEdges(const std::string& path)
{
    std::ifstream fin(path);
    if (!fin.is_open()){
        throw GraphException("无法打开边文件: "+path);
    }
    std::vector <edge> edges;
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
    std::vector <edge> edges=ReadEdges(edges_path);
    for (const LocationInfo& v : nodes){
        graph.InsertVertex(v);
    }
    for (const edge& e : edges){
        graph.InsertEdge(e.from,e.to,e.length);
    }
}

void ShowAllNodes(const LGraph& graph)      // 显示所有顶点
{
    const std::vector <VertexNode>& ver_list=graph.List();
    for (const VertexNode& v : ver_list){
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
    if (!fout.is_open()){
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
    if (!fout.is_open()){
        throw GraphException("无法创建文件: "+path);
    }
    std::vector<Edge> edges=graph.SortedEdges();
    for (const Edge& e : edges){
        fout<<e.from<<","<<e.to<<","<<e.weight<<"\n";
    }
}

int main()
{
    LGraph graph;
    try {
        init(graph);
    }
    catch (const GraphException& e){
        std::cerr<<"初始化失败: "<<e.what()<<std::endl;
        return -1;
    }

    while (true)
    {
        int choice;
        std::cout<<"欢迎使用校园导航系统！\n"
                 <<"请选择您要进行的操作：\n"
                 <<"1. 顶点相关操作\n"
                 <<"2. 边相关操作\n"
                 <<"3. 从文件中重新加载点与边\n"
                 <<"4. 判断是否存在欧拉回路\n"
                 <<"5. 求任意两点间的最短距离\n"
                 <<"6. 求最小生成树\n"
                 <<"7. 求拓扑受限的最短路径\n"
                 <<"8. 退出程序\n"
                 <<"请输入选项编号：";
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
                    int sub;
                    std::cout<<"顶点相关操作：\n"
                             <<"1. 输出特定顶点信息\n"
                             <<"2. 输出所有顶点信息\n"
                             <<"3. 添加一个顶点\n"
                             <<"4. 删除一个顶点\n"
                             <<"5. 将顶点存储到文件\n"
                             <<"6. 返回上一级菜单\n"
                             <<"请输入选项编号：";
                    std::cin>>sub;
                    switch (sub)
                    {
                        case 1:
                        {
                            std::cout<<"请输入顶点名称：";
                            std::string name;
                            std::cin>>name;
                            LocationInfo info=graph.GetVertex(name);
                            std::cout<<"顶点名称："<<info.name<<"\n"
                                     <<"顶点类型："<<info.type<<"\n"
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
                            std::cout<<"请输入顶点名称、顶点类型、建议游览时间：";
                            std::string name,type;
                            int visitTime;
                            std::cin>>name>>type>>visitTime;
                            graph.InsertVertex({name,type,visitTime});
                            break;
                        }
                        case 4:
                        {
                            std::cout<<"请输入顶点名称：";
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
                    std::cout<<"边相关操作：\n"
                             <<"1. 输出特定边信息\n"
                             <<"2. 输出所有边信息\n"
                             <<"3. 添加一条边\n"
                             <<"4. 删除一条边\n"
                             <<"5. 将边存储到文件\n"
                             <<"6. 返回上一级菜单\n"
                             <<"请输入选项编号：";
                    std::cin>>sub;
                    switch (sub)
                    {
                        case 1:
                        {
                            std::cout<<"请输入边的起点与终点：";
                            std::string u,v;
                            std::cin>>u>>v;
                            EWeight w=graph.GetEdge(u,v);
                            std::cout<<u<<" <---> "<<v<<" 距离为："<<w<<"\n";
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
                        std::vector<Edge> mst=MinimumSpanningTree(graph);
                        if (mst.empty()){
                            std::cout<<"图无法形成生成树（可能存在孤立顶点）。\n";
                        }
                        else {
                            int sum=0;
                            std::cout<<"最小生成树的边如下（起点,终点,权重）：\n";
                            for (const Edge& e : mst){
                                std::string name_u=graph.GetVertex(e.from).name;
                                std::string name_v=graph.GetVertex(e.to).name;
                                std::cout<<name_u<<" - "<<name_v<<" : "<<e.weight<<"\n";
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
                    std::vector <std::string> order(n);
                    for (int i=0;i<n;i++){
                        std::cin>>order[i];
                    }
                    int d=TopologicalShortestPath(graph,order);
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
