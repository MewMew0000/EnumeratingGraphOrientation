#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <future>
#include <mutex>
#include <functional>

#include "tdzdd/DdSpec.hpp"
#include "tdzdd/DdEval.hpp"
#include "tdzdd/eval/Cardinality.hpp"
#include "tdzdd/DdStructure.hpp"
#include "tdzdd/util/Graph.hpp"

using namespace tdzdd;

#include "FrontierManager.hpp"
#include "FrontierSingleCycle.hpp"
#include "FrontierSingleHamiltonianCycle.hpp"
#include "FrontierSTPath.hpp"
#include "FrontierForest.hpp"
#include "FrontierTree.hpp"
#include "FrontierMatching.hpp"
#include "FrontierMate.hpp"
#include "FrontierDegreeDistribution.hpp"
#include "MySpec.hpp"
#include "EulerOrientation.hpp"
#include "DagOrientation.hpp"
#include "DagOp.hpp"

#include "EnumSubgraphs.hpp"


std::string getVertex(int i, int j) {
    std::ostringstream oss;
    oss << i << ":" << j;
    return oss.str();
}

void makeGridGraph(tdzdd::Graph& graph, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (j < n - 1) {
                graph.addEdge(getVertex(i, j), getVertex(i, j + 1));
            }
            if (i < n - 1) {
                graph.addEdge(getVertex(i, j), getVertex(i + 1, j));
            }
        }
    }
    graph.update();
}

int main(int argc, char** argv) {

    if (argc == 1) {
        // from https://oeis.org/A140517
        std::string solutions[] = {"0", "0", "1", "13", "213", "9349",
                                   "1222363", "487150371", "603841648931",
                                   "2318527339461265",
                                   "27359264067916806101"};

        for (int n = 2; n <= 10; ++n) {
            tdzdd::Graph graph;
            makeGridGraph(graph, n);
            FrontierManager fm(graph);
            FrontierSingleCycleSpec spec(graph);
            DdStructure<2> dd(spec);
            std::cerr << "n = " << n << ", # of solutions = "
                      << dd.zddCardinality();
            if (dd.zddCardinality() != solutions[n]) {
                std::cerr << ", which is different from expected number "
                          << solutions[n] << ".";
            }
            std::cerr << std::endl;
        }
    } else {
        tdzdd::Graph graph;
        bool is_path = false;
        bool is_ham_path = false;
        bool is_cycle = false;
        bool is_ham_cycle = false;
        bool is_path_m = false;
        bool is_ham_path_m = false;
        bool is_cycle_m = false;
        bool is_ham_cycle_m = false;

        bool is_forest = false;
        bool is_tree = false;
        bool is_stree = false;
        bool is_matching = false;
        bool is_cmatching = false;
        bool is_letter_I = false;
        bool is_letter_P = false;
        bool is_my = false;
        bool is_euler = false;
        bool is_dag = false;
        bool is_dagop = false;

        bool is_dot = false;
        bool is_show_fs = false;
        bool is_enum = false;

        bool readfirst = false;
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == std::string("--path")) {
                is_path = true;
            } else if (std::string(argv[i]) == std::string("--hampath")) {
                is_ham_path = true;
            } else if (std::string(argv[i]) == std::string("--cycle") ||
                       std::string(argv[i]) == std::string("--letter_O")) {
                is_cycle = true;
            } else if (std::string(argv[i]) == std::string("--hamcycle")) {
                is_ham_cycle = true;
            } else if (std::string(argv[i]) == std::string("--path_m")) {
                is_path_m = true;
            } else if (std::string(argv[i]) == std::string("--hampath_m")) {
                is_ham_path_m = true;
            } else if (std::string(argv[i]) == std::string("--cycle_m")) {
                is_cycle_m = true;
            } else if (std::string(argv[i]) == std::string("--hamcycle_m")) {
                is_ham_cycle_m = true;
            } else if (std::string(argv[i]) == std::string("--forest")) {
                is_forest = true;
            } else if (std::string(argv[i]) == std::string("--tree")) {
                is_tree = true;
            } else if (std::string(argv[i]) == std::string("--stree")) {
                is_stree = true;
            } else if (std::string(argv[i]) == std::string("--matching")) {
                is_matching = true;
            } else if (std::string(argv[i]) == std::string("--cmatching")) {
                is_cmatching = true;
            } else if (std::string(argv[i]) == std::string("--letter_I") ||
                       std::string(argv[i]) == std::string("--letter_L")) {
                is_letter_I = true;
            } else if (std::string(argv[i]) == std::string("--letter_P")) {
                is_letter_P = true;
            } else if (std::string(argv[i]) == std::string("--show")) {
                tdzdd::MessageHandler::showMessages(true);
            } else if (std::string(argv[i]) == std::string("--dot")) {
                is_dot = true;
            } else if (std::string(argv[i]) == std::string("--show-fs")) {
                is_show_fs = true;
            } else if (std::string(argv[i]) == std::string("--enum")) {
                is_enum = true;
            } else if (std::string(argv[i]) == std::string("--my-spec")) {
                is_my = true;
            } else if (std::string(argv[i]) == std::string("--euler")) {
                is_euler = true;
            }
            else if (std::string(argv[i]) == std::string("--dag")) {
                is_dag = true;
            }
            else if (std::string(argv[i]) == std::string("--dagop")) {
                is_dagop = true;
            }
            else if (argv[i][0] == '-') {
                std::cerr << "unknown option " << argv[i] << std::endl;
                return 1;
            } else {
                if (!readfirst) {
                    readfirst = true;
                    graph.readEdges(argv[i]);
                } else {
                    std::cerr << "illegal argument " << argv[i] << std::endl;
                    return 1;
                }
            }
        }
        //use tarjan algorithm here to decompose to graph
        FrontierManager fm(graph);

        std::cerr << "# of vertices = " << graph.vertexSize() << std::endl;
        std::cerr << "# of edges = " << graph.edgeSize() << std::endl;

        if (is_show_fs) {
            fm.print();
        }

        DdStructure<2> dd;

        std::ostringstream oss;
        oss << graph.vertexSize();
        const int endPoint = graph.getVertex(oss.str());

        if (is_path) {
            FrontierSTPathSpec spec(graph, false, graph.getVertex("1"), endPoint);
            dd = DdStructure<2>(spec);
        } else if (is_ham_path) {
            FrontierSTPathSpec spec(graph, true, graph.getVertex("1"), endPoint);
            dd = DdStructure<2>(spec);
        } else if (is_cycle) {
            FrontierSingleCycleSpec spec(graph);
            dd = DdStructure<2>(spec);
        } else if (is_ham_cycle) {
            FrontierSingleHamiltonianCycleSpec spec(graph);
            dd = DdStructure<2>(spec);
        } else if (is_path_m) {
            FrontierMateSpec spec(graph, false, graph.getVertex("1"), endPoint);
            dd = DdStructure<2>(spec);
        } else if (is_ham_path_m) {
            FrontierMateSpec spec(graph, true, graph.getVertex("1"), endPoint);
            dd = DdStructure<2>(spec);
        } else if (is_cycle_m) {
            FrontierMateSpec spec(graph, false);
            dd = DdStructure<2>(spec);
        } else if (is_ham_cycle_m) {
            FrontierMateSpec spec(graph, true);
            dd = DdStructure<2>(spec);
        } else if (is_forest) {
            FrontierForestSpec spec(graph);
            dd = DdStructure<2>(spec);
        } else if (is_tree) {
            FrontierTreeSpec spec(graph, false);
            dd = DdStructure<2>(spec);
        } else if (is_stree) {
            FrontierTreeSpec spec(graph, true);
            dd = DdStructure<2>(spec);
        } else if (is_matching) {
            FrontierMatchingSpec spec(graph, false);
            dd = DdStructure<2>(spec);
        } else if (is_cmatching) {
            FrontierMatchingSpec spec(graph, true);
            dd = DdStructure<2>(spec);
        } else if (is_letter_I) {
            std::vector<IntSubset*> degRanges;
            degRanges.push_back(new IntRange(0, INT_MAX));
            degRanges.push_back(new IntRange(2, 2));
            degRanges.push_back(new IntRange(0, INT_MAX));
            FrontierDegreeDistributionSpec spec(graph, degRanges, true);
            dd = DdStructure<2>(spec);
        } else if (is_letter_P) {
            std::vector<IntSubset*> degRanges;
            degRanges.push_back(new IntRange(0, INT_MAX));
            degRanges.push_back(new IntRange(1, 1));
            degRanges.push_back(new IntRange(0, INT_MAX));
            degRanges.push_back(new IntRange(1, 1));
            FrontierDegreeDistributionSpec spec(graph, degRanges, true);
            dd = DdStructure<2>(spec);
        } else if (is_my) {
            Combination spec(5, 2);
            spec.dumpDot(std::cout);
        } else if (is_euler) {
            EulerOrientationSpec spec(graph);
            dd = DdStructure<2>(spec);
        }
        else if (is_dag) {
            DagOrientationSpec spec(graph);
            dd = DdStructure<2>(spec);
            dd.zddReduce();
        }
        else if (is_dagop) {
            // 分解图为连通分量
            std::vector<tdzdd::Graph> components = graph.decomposeToBCCAndBridges();

            std::cerr << "Graph decomposed into " << components.size() << " connected components." << std::endl;

            if (components.size() == 1) {
                // 只有一个连通分量，直接处理
                DagOpSpec spec(graph);
                dd = DdStructure<2>(spec);
                dd.zddReduce();
            } else {
                // 多个连通分量，使用受控的多线程处理
                const size_t maxThreads = std::min(components.size(),
                                                   static_cast<size_t>(std::thread::hardware_concurrency()));

                std::vector<DdStructure<2> > componentDDs(components.size());
                std::vector<std::thread> threads;
                std::mutex consoleMutex;

                // 使用 std::function 包装 lambda
                std::function<void(size_t, size_t)> processComponent = [&](size_t startIdx, size_t endIdx) {
                    for (size_t i = startIdx; i < endIdx; ++i) {
                        const tdzdd::Graph& componentGraph = components[i];

                        {
                            std::lock_guard<std::mutex> lock(consoleMutex);
                            std::cerr << "Processing component " << i
                                      << " with " << componentGraph.vertexSize() << " vertices and "
                                      << componentGraph.edgeSize() << " edges in thread "
                                      << std::this_thread::get_id() << std::endl;
                        }

                        DagOpSpec spec(componentGraph);
                        componentDDs[i] = DdStructure<2>(spec);
                        componentDDs[i].zddReduce();

                        {
                            std::lock_guard<std::mutex> lock(consoleMutex);
                            std::cerr << "Component " << i << " completed with "
                                      << componentDDs[i].zddCardinality() << " solutions" << std::endl;
                        }
                    }
                };

                // 分配工作给线程
                size_t componentsPerThread = (components.size() + maxThreads - 1) / maxThreads;

                for (size_t t = 0; t < maxThreads; ++t) {
                    size_t startIdx = t * componentsPerThread;
                    size_t endIdx = std::min(startIdx + componentsPerThread, components.size());

                    if (startIdx < components.size()) {
                        threads.emplace_back(processComponent, startIdx, endIdx);
                    }
                }

                // 等待所有线程完成
                for (auto& thread : threads) {
                    thread.join();
                }

                std::cerr << "All components processed. Combining results..." << std::endl;
                size_t total_size = 0;
                unsigned long long total_solutions = 1;
                // 合并结果
                if (!componentDDs.empty()) {
                    for (size_t i = 1; i < componentDDs.size(); ++i) {
                        std::cerr << "Combining component " << i << " result..." << std::endl;
                        std::cerr << "size of " <<  i  << " = " << componentDDs[i].size() << " ZDD nodes, "
                                  << "solution of " <<  i  << " = " << std::stoull(componentDDs[i].zddCardinality()) << " total solutions" << std::endl;
                        // 根据实际需要实现合并逻辑
                        total_size += componentDDs[i].size();
                        total_solutions *= std::stoull(componentDDs[i].zddCardinality());
                    }
                }

                std::cerr << "Combined result: " << total_size << " ZDD nodes, "
                          << total_solutions << " total solutions" << std::endl;
            }
        }
        else {
            std::cerr << "Please specify a kind of subgraphs." << std::endl;
            exit(1);
        }

//        std::cerr << "# of ZDD nodes = " << dd.size() << std::endl;
//        std::cerr << "# of solutions = " << dd.zddCardinality() << std::endl;

        if (is_dot) {
            dd.dumpDot(std::cout);
        }
        if (is_enum) {
            EnumSubgraphs::enumSubgraphs(std::cout, graph, dd);
        }
    }

    return 0;
}
