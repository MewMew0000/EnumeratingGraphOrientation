#ifndef EULER_ORIENTATION_HPP
#define FRONTIER_ST_PATH_HPP

#include "FrontierManager.hpp"
#include "tdzdd/DdSpec.hpp"
#include "tdzdd/util/Graph.hpp"

class FrontierDegData {
public:
    short inDeg;
    short outDeg;
};

class EulerOrientationSpec: public tdzdd::PodArrayDdSpec<EulerOrientationSpec, FrontierDegData, 2> {
    const tdzdd::Graph& graph_;
    const short n_;
    const int m_;
    const FrontierManager fm_;

    short getInDeg(const FrontierDegData* data, short v) const {
        return data[fm_.vertexToPos(v)].inDeg;
    }

    short getOutDeg(const FrontierDegData* data, short v) const {
        return data[fm_.vertexToPos(v)].outDeg;
    }

    void setInDeg(FrontierDegData* data, short v, short d) const {
        data[fm_.vertexToPos(v)].inDeg = d;
    }

    void setOutDeg(FrontierDegData* data, short v, short d) const {
        data[fm_.vertexToPos(v)].outDeg = d;
    }

    void initialize(FrontierDegData* data) const {
        for (int i = 0; i < fm_.getMaxFrontierSize(); ++i) {
            data[i].inDeg = 0;
            data[i].outDeg = 0;
        }
    }
public:
    explicit EulerOrientationSpec(const tdzdd::Graph& graph)
    : graph_(graph),
      n_(static_cast<short>(graph_.vertexSize())),
      m_(graph_.edgeSize()),
      fm_(graph_)
    {
        if (graph_.vertexSize() > SHRT_MAX) { // SHRT_MAX == 32767
            std::cerr << "The number of vertices should be at most "
                      << SHRT_MAX << std::endl;
            exit(1);
        }
        setArraySize(fm_.getMaxFrontierSize());
    }
    int getRoot(FrontierDegData* data) {
        initialize(data);
        return m_;
    }
    int getChild(FrontierDegData* data, int level, int value) {
        //0-arc represents edge.v1->edge.v2，while 1-arc represents edge.v2->edge.v1
        const int edge_index = m_ - level;
        const Graph::EdgeInfo& edge = graph_.edgeInfo(edge_index);
        //check newly entered vs
        const std::vector<int>& entering_vs = fm_.getEnteringVs(edge_index);
        for (const int v : entering_vs) {
            setInDeg(data, v, 0);
            setOutDeg(data, v, 0);
        }
        //update indeg and outdeg
        if (value == 1) { //1-arc
            setInDeg(data, edge.v1, getInDeg(data, edge.v1) + 1);
            setOutDeg(data, edge.v2, getOutDeg(data, edge.v2) + 1);
        }
        else {
            setOutDeg(data, edge.v1, getOutDeg(data, edge.v1) + 1);
            setInDeg(data, edge.v2, getInDeg(data, edge.v2) + 1);
        }
        //check leaving vs
        const std::vector<int>& leaving_vs = fm_.getLeavingVs(edge_index);
        for (const int v : leaving_vs) {
            if (getInDeg(data, v) != getOutDeg(data, v)) {
                return 0;
            }
        }
        return (level == 1) ? -1 : level - 1;
    }
};

#endif