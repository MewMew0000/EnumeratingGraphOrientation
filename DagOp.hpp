#ifndef DAGOP_HPP
#define DAGOP_HPP

#include "FrontierManager.hpp"
#include "tdzdd/DdSpec.hpp"
#include "tdzdd/util/Graph.hpp"
#include "unordered_set"
#include "unordered_map"

class FrontierClosure {
public:
    std::map<int, std::unordered_set<int> > rel;
    bool operator==(const FrontierClosure& other) const {
        return rel == other.rel;
    }
};

class DagOpSpec: public tdzdd::DdSpec<DagOpSpec, FrontierClosure, 2> {
    const tdzdd::Graph& graph_;
    const short n_;
    const int m_;
    const FrontierManager fm_;
    void initialize(FrontierClosure& data) {
        data.rel = std::map<int, std::unordered_set<int> >();
        std::map<int, std::unordered_set<int> >& r = data.rel;
    }
    // 在原有的closure上添加边成为新的closure
    void nextClosure_(std::map<int, std::unordered_set<int> >& r, const int v1, const int v2) {
        r[v1].insert(v1);
        r[v2].insert(v2);
        std::vector<int> to_update;
        for (const auto& [v, adj] : r) {
            if (adj.find(v1) != adj.end()) {
                to_update.push_back(v);
            }
        }
        // 单独做更新，避免在遍历中修改 r
        for (int v : to_update) {
            r[v].insert(r[v2].begin(), r[v2].end());
        }
    }

    void erase_(std::map<int, std::unordered_set<int> >& r, const int v1) {
        std::vector<int> to_update;
        for (const auto& [v, adj] : r) {
            if (adj.find(v1) != adj.end()) {
                to_update.push_back(v);
            }
        }

        for (int v : to_update) {
            r[v].erase(v1);
        }
        r.erase(v1);
    }

public:
    explicit DagOpSpec(const tdzdd::Graph& graph)
            : graph_(graph),
              n_(static_cast<short>(graph_.vertexSize())),
              m_(graph_.edgeSize()),
              fm_(graph_){}
    int getRoot(FrontierClosure& data){
        initialize(data);
        return m_;
    }
    int getChild(FrontierClosure& data, int level, int value) {
        //0-arc represents edge.v1->edge.v2，while 1-arc represents edge.v2->edge.v1
        const int edge_index = m_ - level;
        const tdzdd::Graph::EdgeInfo& edge = graph_.edgeInfo(edge_index);
        std::map<int, std::unordered_set<int> >& r = data.rel;
        if (value == 1) { //1-arc
            auto it = r.find(edge.v1);
            if (it != r.end() && it->second.find(edge.v2) != it->second.end()) return 0;
            nextClosure_(r, edge.v2, edge.v1);
        }
        else {
            auto it = r.find(edge.v2);
            if (it != r.end() && it->second.find(edge.v1) != it->second.end()) return 0;
            nextClosure_(r, edge.v1, edge.v2);
        }
        //leaving
        const std::vector<int>& leaving_vs = fm_.getLeavingVs(edge_index);
        for (const int v : leaving_vs) {
            erase_(r, v);
        }
        return (level == 1) ? -1 : level - 1;
    }
};


#endif //DAGOP_HPP