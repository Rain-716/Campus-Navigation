#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>
#include <limits>
#include <queue>
#include <stack>
#include <algorithm>
#include <functional>
#include <numeric>
#include "LGraph/LGraph.h"
#include "Algorithm/Algorithm.h"
#include "LocationInfo.h"
#include "GraphException.h"

using namespace Graph;
using namespace Graph::Algorithm;

static const std::string nodes_path = "data/nodes.csv";
static const std::string edges_path = "data/edges.csv";
static const std::string command_path = "cmd/command.txt";
static const std::string answer_path = "cmd/answer.txt";

// 前置声明
std::vector<LocationInfo> ReadNodes(const std::string& path);
std::vector<std::tuple<std::string, std::string, int>> ReadEdges(const std::string& path);
void initGraph(LGraph& graph);
bool existEulerPath(const LGraph& graph);
std::pair<int, std::vector<std::string>> shortestPathWithTrace(const LGraph& graph, const std::string& src, const std::string& dst);

int main() {
    LGraph graph;
    try { initGraph(graph); }
    catch (const GraphException& e) {
        std::cerr << "初始化失败: " << e.what() << std::endl;
        return -1;
    }

    std::ifstream cmdIn(command_path);
    std::ofstream ansOut(answer_path);
    if (!cmdIn || !ansOut) {
        std::cerr << "无法打开命令或答案文件" << std::endl;
        return -1;
    }
    std::string line;
    while (std::getline(cmdIn, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;
        if (cmd == "SHORTEST_PATH") {
            std::string u, v;
            ss >> u >> v;
            auto [dist, path] = shortestPathWithTrace(graph, u, v);
            if (dist < 0) {
                ansOut << "NA" << std::endl;
            } else {
                ansOut << "DIST " << dist << " PATH";
                for (auto& name : path) ansOut << " " << name;
                ansOut << std::endl;
            }
        } else if (cmd == "ADJ_EDGES") {
            std::string u;
            ss >> u;
            if (!graph.ExistVertex(u)) {
                ansOut << "NONE" << std::endl;
                continue;
            }
            size_t uid = graph.Map().at(u);
            const auto& adj = graph.List()[uid].adj;
            if (adj.empty()) {
                ansOut << "NONE" << std::endl;
                continue;
            }
            // 收集并按顶点名排序
            std::vector<std::pair<std::string, int>> neigh;
            for (const auto& e : adj) {
                neigh.emplace_back(graph.GetVertex(e.to).name, e.weight);
            }
            std::sort(neigh.begin(), neigh.end(), [](auto& a, auto& b){ return a.first < b.first; });
            bool first=true;
            for (auto& p : neigh) {
                if (!first) ansOut << " "; first=false;
                ansOut << p.first << "(" << p.second << ")";
            }
            ansOut << std::endl;
        } else if (cmd == "FIND_TYPE") {
            std::string type;
            ss >> type;
            bool first = true;
            for (auto& vnode : graph.List()) {
                if (vnode.info.type == type) {
                    if (!first) ansOut << " "; first = false;
                    ansOut << vnode.info.name;
                }
            }
            ansOut << std::endl;
        } else if (cmd == "INSERT_EDGE") {
            std::string u, v; int w;
            ss >> u >> v >> w;
            graph.InsertEdge(u, v, w);
            ansOut << "OK" << std::endl;
        } else if (cmd == "DELETE_EDGE") {
            std::string u, v;
            ss >> u >> v;
            graph.DeleteEdge(u, v);
            ansOut << "OK" << std::endl;
        } else if (cmd == "MODIFY_EDGE_WEIGHT") {
            std::string u, v; int w;
            ss >> u >> v >> w;
            graph.UpdateEdge(u, v, w);
            ansOut << "OK" << std::endl;
        } else if (cmd == "INSERT_NODE") {
            std::string name, type; int vt;
            ss >> name >> type >> vt;
            graph.InsertVertex(LocationInfo(name, type, vt));
            ansOut << "OK" << std::endl;
        } else if (cmd == "DELETE_NODE") {
            std::string name;
            ss >> name;
            graph.DeleteVertex(name);
            ansOut << "OK" << std::endl;
        } else if (cmd == "EULERIAN_PATH") {
            ansOut << (existEulerPath(graph) ? "YES" : "NO") << std::endl;
        } else if (cmd == "MST_INFO") {
            auto edges = MinimumSpanningTree(graph);
            // 判断是否连通（应有 n-1 条边）
            if (edges.size() != graph.VertexCount() ? graph.VertexCount() - 1 : 0) {
                // 修正：当节点>0且边数不足时视为断开
            }
            if (graph.VertexCount() > 0 && edges.size() != graph.VertexCount() - 1) {
                ansOut << "DISCONNECTED" << std::endl;
            } else {
                int total = 0;
                std::vector<std::pair<std::string,std::string>> names;
                for (auto& e : edges) {
                    total += e.weight;
                    names.emplace_back(graph.GetVertex(e.from).name, graph.GetVertex(e.to).name);
                }
                std::vector<size_t> idx(names.size()); std::iota(idx.begin(), idx.end(), 0);
                std::sort(idx.begin(), idx.end(), [&](size_t a, size_t b) { return names[a].first < names[b].first; });
                ansOut << "MST " << total;
                for (auto i : idx) {
                    auto& p = names[i];
                    int w = graph.GetEdge(p.first, p.second);
                    ansOut << " " << p.first << "-" << p.second << ":" << w;
                }
                ansOut << std::endl;
            }
        }
    }
    return 0;
}

std::vector<LocationInfo> ReadNodes(const std::string& path) {
    std::ifstream fin(path);
    if (!fin) throw GraphException("无法打开节点文件: " + path);
    std::vector<LocationInfo> nodes;
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string name, type, vt_str;
        if (!std::getline(ss, name, ',')) continue;
        if (!std::getline(ss, type, ',')) continue;
        if (!std::getline(ss, vt_str, ',')) continue;
        nodes.emplace_back(name, type, std::stoi(vt_str));
    }
    return nodes;
}

std::vector<std::tuple<std::string, std::string, int>> ReadEdges(const std::string& path) {
    std::ifstream fin(path);
    if (!fin) throw GraphException("无法打开边文件: " + path);
    std::vector<std::tuple<std::string, std::string, int>> edges;
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string u, v, w_str;
        if (!std::getline(ss, u, ',')) continue;
        if (!std::getline(ss, v, ',')) continue;
        if (!std::getline(ss, w_str, ',')) continue;
        edges.emplace_back(u, v, std::stoi(w_str));
    }
    return edges;
}

void initGraph(LGraph& graph) {
    graph = LGraph();
    auto nodes = ReadNodes(nodes_path);
    for (auto& v : nodes) graph.InsertVertex(v);
    auto edges = ReadEdges(edges_path);
    for (auto& t : edges) graph.InsertEdge(std::get<0>(t), std::get<1>(t), std::get<2>(t));
}

bool existEulerPath(const LGraph& graph) {
    size_t n = graph.VertexCount();
    if (n == 0) return true;
    if (!IsConnected(graph)) return false;
    int odd = 0;
    for (auto& v : graph.List()) if (v.adj.size() % 2) odd++;
    return odd == 0 || odd == 2;
}

std::pair<int, std::vector<std::string>> shortestPathWithTrace(const LGraph& graph, const std::string& xName, const std::string& yName) {
    if (!graph.ExistVertex(xName) || !graph.ExistVertex(yName)) return {-1, {}};
    const auto& m = graph.Map();
    size_t xid = m.at(xName), yid = m.at(yName);
    int n = graph.VertexCount();
    const auto& vlist = graph.List();
    const long long INF = std::numeric_limits<long long>::max();
    std::vector<long long> dist(n, INF);
    std::vector<int> prev(n, -1);
    dist[xid] = 0;
    using PII = std::pair<long long,size_t>;
    std::priority_queue<PII, std::vector<PII>, std::greater<PII>> pq;
    pq.push({0, xid});
    while (!pq.empty()) {
        auto [d,u] = pq.top(); pq.pop();
        if (d > dist[u]) continue;
        if (u == yid) break;
        for (auto& e : vlist[u].adj) {
            size_t v = e.to;
            long long nd = d + e.weight;
            if (nd < dist[v]) {
                dist[v] = nd;
                prev[v] = u;
                pq.push({nd, v});
            }
        }
    }
    if (dist[yid] == INF) return {-1, {}};
    std::vector<std::string> path;
    for (int at = yid; at != -1; at = prev[at]) path.push_back(graph.GetVertex(at).name);
    std::reverse(path.begin(), path.end());
    return {(int)dist[yid], path};
}