//
// Created by MewMew on 4/20/2025.
//

#ifndef DAGORIENTATION_HPP
#define DAGORIENTATION_HPP

#include "FrontierManager.hpp"
#include "tdzdd/DdSpec.hpp"
#include "tdzdd/util/Graph.hpp"

#define SZ 100

class FrontierAdjData {
public:
    std::vector<std::vector<bool> > adj;//vector is not good
    bool operator==(const FrontierAdjData& other) const {
        return adj == other.adj;
    }
};

class DagOrientationSpec: public tdzdd::DdSpec<DagOrientationSpec, FrontierAdjData, 2> {
    const tdzdd::Graph& graph_;
    const short n_;
    const int m_;
    const FrontierManager fm_;
    void initialize(FrontierAdjData& data) {
        data.adj = std::vector<std::vector<bool> >(SZ, std::vector<bool>(SZ, false));
        std::vector<std::vector<bool> >& r = data.adj;
        int n = r.size();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i == j) r[i][j] = true;
            }
        }
    }
    void transClosure_(std::vector<std::vector<bool> >& r, const int v1, const int v2) {
        int n = r.size();
        assert(v1 > 0 && v1 < n && v2 > 0 && v2 < n);
        r[v1][v2] = true;
        for (int i = 0; i < n; i++) {
            if (r[i][v1]) {
                for (int j = 0; j < n; j++) {
                    if (r[v2][j]) {
                        r[i][j] = true;
                    }
                }
            }
        }
    }
    void erase_(std::vector<std::vector<bool> >& r, const int v) {
        int n = r.size();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if ((i == v) || (j == v)) r[i][j] = false;
            }
        }
    }

public:
    explicit DagOrientationSpec(const tdzdd::Graph& graph)
    : graph_(graph),
      n_(static_cast<short>(graph_.vertexSize())),
      m_(graph_.edgeSize()),
      fm_(graph_){}
    int getRoot(FrontierAdjData& data){
        initialize(data);
        return m_;
    }
    int getChild(FrontierAdjData& data, int level, int value) {
        //0-arc represents edge.v1->edge.v2，while 1-arc represents edge.v2->edge.v1
        const int edge_index = m_ - level;
        const tdzdd::Graph::EdgeInfo& edge = graph_.edgeInfo(edge_index);
        std::vector<std::vector<bool> >& r = data.adj;
        if (value == 1) { //1-arc
            if (r[edge.v1][edge.v2]) return 0;
            transClosure_(r, edge.v2, edge.v1);
        }
        else {
            if (r[edge.v2][edge.v1]) return 0;
            transClosure_(r, edge.v1, edge.v2);
        }
        //leaving
        const std::vector<int>& leaving_vs = fm_.getLeavingVs(edge_index);
        for (const int v : leaving_vs) {
            erase_(r, v);
        }
        return (level == 1) ? -1 : level - 1;
    }
};

#endif //DAGORIENTATION_HPP
