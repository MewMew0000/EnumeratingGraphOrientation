#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <fstream>
#include <set>
#include <utility>
#include <string>
#include <sstream>

using namespace std;

// 支持 unordered_map 以 pair<int,int> 作为key
struct pair_hash {
    size_t operator()(const pair<int,int>& p) const {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
    }
};

// 判断当前有向边集合是否有环（基于Kahn拓扑排序）
bool has_cycle(
    const vector<int>& vertices,
    const unordered_map<pair<int,int>, bool, pair_hash>& orientation
) {
    unordered_map<int, vector<int> > directed_adj;
    unordered_map<int, int> in_degree;

    for (int v : vertices) {
        in_degree[v] = 0;
    }

    for (const auto& [edge, direction] : orientation) {
        int u = edge.first;
        int v = edge.second;
        if (direction) { // u -> v
            directed_adj[u].push_back(v);
            in_degree[v]++;
        } else { // v -> u
            directed_adj[v].push_back(u);
            in_degree[u]++;
        }
    }

    queue<int> q;
    for (int v : vertices) {
        if (in_degree[v] == 0) q.push(v);
    }

    int processed = 0;
    while (!q.empty()) {
        int cur = q.front();
        q.pop();
        processed++;
        for (int nei : directed_adj[cur]) {
            if (--in_degree[nei] == 0)
                q.push(nei);
        }
    }

    return processed < (int)vertices.size();
}

// 暴力生成所有边方向的所有组合（未剪枝）
void generate_all_orientations(
    const vector<pair<int,int> >& edges,
    int index,
    unordered_map<pair<int,int>, bool, pair_hash>& orientation,
    vector<unordered_map<pair<int,int>, bool, pair_hash> >& results
) {
    if (index == (int)edges.size()) {
        results.push_back(orientation);
        return;
    }

    const auto& edge = edges[index];
    // 两个方向尝试
    orientation[edge] = true;
    generate_all_orientations(edges, index + 1, orientation, results);

    orientation[edge] = false;
    generate_all_orientations(edges, index + 1, orientation, results);
}

// 高效方法类定义
class EfficientAcyclicOrientationGenerator {
public:
    vector<int> vertices;
    int n;
    vector<pair<int,int> > edges;
    unordered_map<int, unordered_set<int> > adj;
    unordered_map<int,int> vertex_to_idx;

    EfficientAcyclicOrientationGenerator(const vector<int>& verts, const vector<pair<int,int> >& edgs)
    : vertices(verts), edges(edgs) {
        sort(vertices.begin(), vertices.end());
        n = (int)vertices.size();
        for (auto& [u,v] : edges) {
            adj[u].insert(v);
            adj[v].insert(u);
        }
        for (int i = 0; i < n; ++i)
            vertex_to_idx[vertices[i]] = i;
    }

    vector<int> get_subgraph_neighbors(int vertex, const unordered_set<int>& processed) {
        vector<int> neighbors;
        for (int nei : adj[vertex]) {
            if (processed.count(nei)) {
                neighbors.push_back(nei);
            }
        }
        sort(neighbors.begin(), neighbors.end());
        return neighbors;
    }

    vector<int> topological_sort_neighbors(
        const vector<int>& neighbors,
        const unordered_map<pair<int,int>, bool, pair_hash>& current_orientation
    ) {
        if (neighbors.empty()) return {};

        unordered_map<int,int> in_degree;
        unordered_map<int, vector<int> > adj_directed;
        for (int v : neighbors) in_degree[v] = 0;

        for (int u : neighbors) {
            for (int v : neighbors) {
                if (u == v) continue;
                pair<int,int> edge = (u < v) ? make_pair(u,v) : make_pair(v,u);
                auto it = current_orientation.find(edge);
                if (it != current_orientation.end()) {
                    bool direction = it->second;
                    bool u_to_v = (u < v) ? direction : !direction;
                    if (u_to_v) {
                        adj_directed[u].push_back(v);
                        in_degree[v]++;
                    }
                }
            }
        }

        queue<int> q;
        for (int v : neighbors)
            if (in_degree[v] == 0) q.push(v);

        vector<int> result;
        while (!q.empty()) {
            int cur = q.front(); q.pop();
            result.push_back(cur);
            for (int nxt : adj_directed[cur]) {
                if (--in_degree[nxt] == 0) q.push(nxt);
            }
        }

        return result;
    }

    void generate_direction_assignments(
        const vector<int>& neighbors,
        int index,
        unordered_map<int,bool>& assignment,
        vector<unordered_map<int,bool> >& all_assignments
    ) {
        if (index == (int)neighbors.size()) {
            all_assignments.push_back(assignment);
            return;
        }
        assignment[neighbors[index]] = true;
        generate_direction_assignments(neighbors, index + 1, assignment, all_assignments);

        assignment[neighbors[index]] = false;
        generate_direction_assignments(neighbors, index + 1, assignment, all_assignments);
    }

    bool has_cycle_in_subgraph(
        const unordered_map<pair<int,int>, bool, pair_hash>& orientation,
        const unordered_set<int>& vertices_subset
    ) {
        unordered_map<int, vector<int> > directed_adj;
        unordered_map<int,int> in_degree;

        for (int v : vertices_subset) in_degree[v] = 0;

        for (const auto& [edge, direction] : orientation) {
            int u = edge.first;
            int v = edge.second;
            if (vertices_subset.count(u) && vertices_subset.count(v)) {
                if (direction) {
                    directed_adj[u].push_back(v);
                    in_degree[v]++;
                } else {
                    directed_adj[v].push_back(u);
                    in_degree[u]++;
                }
            }
        }

        queue<int> q;
        for (int v : vertices_subset)
            if (in_degree[v] == 0) q.push(v);

        int processed = 0;
        while (!q.empty()) {
            int cur = q.front(); q.pop();
            processed++;
            for (int nxt : directed_adj[cur]) {
                if (--in_degree[nxt] == 0) q.push(nxt);
            }
        }

        return processed < (int)vertices_subset.size();
    }

    bool is_legal_assignment(
        int vertex,
        const vector<int>& neighbors,
        const unordered_map<int,bool>& assignment,
        const unordered_map<pair<int,int>, bool, pair_hash>& current_orientation
    ) {
        auto test_orientation = current_orientation;

        for (int neighbor : neighbors) {
            pair<int,int> edge = (vertex < neighbor) ? make_pair(vertex, neighbor) : make_pair(neighbor, vertex);
            bool dir = assignment.at(neighbor);
            if (dir) { // neighbor -> vertex
                if (vertex < neighbor) test_orientation[edge] = false;
                else test_orientation[edge] = true;
            } else { // vertex -> neighbor
                if (vertex < neighbor) test_orientation[edge] = true;
                else test_orientation[edge] = false;
            }
        }

        unordered_set<int> vertices_subset(vertices.begin(), vertices.begin() + vertex_to_idx[vertex] + 1);
        return !has_cycle_in_subgraph(test_orientation, vertices_subset);
    }

    void acyclic_recursive(
        int vertex_idx,
        unordered_map<pair<int,int>, bool, pair_hash>& current_orientation,
        vector<unordered_map<pair<int,int>, bool, pair_hash> >& results
    ) {
        if (vertex_idx >= n) {
            results.push_back(current_orientation);
            return;
        }

        int current_vertex = vertices[vertex_idx];
        unordered_set<int> processed_vertices(vertices.begin(), vertices.begin() + vertex_idx);
        vector<int> neighbors = get_subgraph_neighbors(current_vertex, processed_vertices);

        if (neighbors.empty()) {
            acyclic_recursive(vertex_idx + 1, current_orientation, results);
            return;
        }

        vector<int> sorted_neighbors = topological_sort_neighbors(neighbors, current_orientation);

        vector<unordered_map<int,bool> > all_assignments;
        unordered_map<int,bool> assignment;
        generate_direction_assignments(sorted_neighbors, 0, assignment, all_assignments);

        for (const auto& assign : all_assignments) {
            if (is_legal_assignment(current_vertex, sorted_neighbors, assign, current_orientation)) {
                auto new_orientation = current_orientation;
                for (int neighbor : sorted_neighbors) {
                    pair<int,int> edge = (current_vertex < neighbor) ? make_pair(current_vertex, neighbor) : make_pair(neighbor, current_vertex);
                    bool dir = assign.at(neighbor);
                    if (dir) { // neighbor -> current_vertex
                        if (current_vertex < neighbor) new_orientation[edge] = false;
                        else new_orientation[edge] = true;
                    } else { // current_vertex -> neighbor
                        if (current_vertex < neighbor) new_orientation[edge] = true;
                        else new_orientation[edge] = false;
                    }
                }
                acyclic_recursive(vertex_idx + 1, new_orientation, results);
            }
        }
    }

    // 入口函数
    vector<unordered_map<pair<int,int>, bool, pair_hash> > generate_all() {
        unordered_map<pair<int,int>, bool, pair_hash> orientation;
        vector<unordered_map<pair<int,int>, bool, pair_hash> > results;
        acyclic_recursive(0, orientation, results);
        return results;
    }
};

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "in: " << argv[0] << " lack of filename" << endl;
        return 1;
    }

    string filename = "../benchmark/" + string(argv[1]);
    ifstream infile(filename);
    if (!infile) {
        cerr << "can't open the file " << filename << endl;
        return 1;
    }

    set<int> vertex_set;
    vector<pair<int,int> > edges;

    string line;
    while (getline(infile, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        int u, v;
        if (!(iss >> u >> v)) {
            cerr << "format error：" << line << endl;
            return 1;
        }
        vertex_set.insert(u);
        vertex_set.insert(v);
        edges.emplace_back(u, v);
    }

    vector<int> vertices(vertex_set.begin(), vertex_set.end());

    auto start = high_resolution_clock::now();

    EfficientAcyclicOrientationGenerator gen(vertices, edges);
    auto res = gen.generate_all();

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    cout << "number of DAG Orientations: " << res.size() << endl;
    cout << "execute time: " << duration.count() << " ms" << endl;

    return 0;
}

