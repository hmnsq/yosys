/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2024  Jannis Harder <jix@yosyshq.com> <me@jix.one>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ifndef TOPO_SCC_H
#define TOPO_SCC_H

#অন্তর্ভাব "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SigCellGraph {
সর্বজনীন:
    প্রকারলেখ পূর্ণসংখ্যা গ্রন্থি_type;

    কাঠামো successor_enumerator {
        std::vector<std::pair<পূর্ণসংখ্যা, পূর্ণসংখ্যা>>::const_iterator current, end;
        bool finished() ধ্রুবক { ফেরত current == end; }
        গ্রন্থি_type পরবর্তী() {
            log_assert(!finished());
            গ্রন্থি_type ফলাফল = current->second;
            ++current;
            ফেরত ফলাফল;
        }
    };

    কাঠামো গ্রন্থি_enumerator {
        পূর্ণসংখ্যা current, end;
        bool finished() ধ্রুবক { ফেরত current == end; }
        গ্রন্থি_type পরবর্তী() {
            log_assert(!finished());
            গ্রন্থি_type ফলাফল = current;
            ++current;
            ফেরত ফলাফল;
        }
    };

ব্যক্তিগত:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::vector<std::pair<পূর্ণসংখ্যা, পূর্ণসংখ্যা>> edges;
    std::vector<std::pair<পূর্ণসংখ্যা, পূর্ণসংখ্যা>> edge_ranges;
    std::vector<পূর্ণসংখ্যা> indices_;
    পূর্ণসংখ্যা offset;
    bool computed = false;

    শূন্য compute() {
        offset = Get_আকার(sig_ids);
        edge_ranges.clear();
        indices_.clear();
        indices_.resize(Get_আকার(sig_ids) + Get_আকার(cell_ids), -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        পূর্ণসংখ্যা range_begin = 0;
        জন্য (পূর্ণসংখ্যা গ্রন্থি = -offset, গ্রন্থি_end = Get_আকার(cell_ids); গ্রন্থি != গ্রন্থি_end; ++গ্রন্থি) {
            যখন (edge != edge_end && edge->first <= গ্রন্থি)
                ++edge;
            পূর্ণসংখ্যা range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

সর্বজনীন:
    গ্রন্থি_type গ্রন্থি(RTLIL::Cell *cell) { ফেরত cell_ids(cell); }
    গ্রন্থি_type গ্রন্থি(SigBit ধ্রুবক &bit) { ফেরত ~sig_ids(bit); }

    bool is_cell(গ্রন্থি_type গ্রন্থি) { ফেরত গ্রন্থি >= 0; }
    bool is_sig(গ্রন্থি_type গ্রন্থি) { ফেরত গ্রন্থি < 0; }

    Cell *cell(গ্রন্থি_type গ্রন্থি) { ফেরত গ্রন্থি >= 0 ? cell_ids[গ্রন্থি] : nullptr; }
    SigBit sig(গ্রন্থি_type গ্রন্থি) { ফেরত গ্রন্থি < 0 ? sig_ids[~গ্রন্থি] : SigBit(); }

    template<typename Src, typename Dst>
    শূন্য add_edge(Src &&src, Dst &&dst) {
        computed = false;
        গ্রন্থি_type src_গ্রন্থি = গ্রন্থি(std::forward<Src>(src));
        গ্রন্থি_type dst_গ্রন্থি = গ্রন্থি(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_গ্রন্থি, dst_গ্রন্থি));
    }

    গ্রন্থি_enumerator enumerate_nodes() {
        যদি (!computed) compute();
        ফেরত {-Get_আকার(sig_ids), Get_আকার(cell_ids)};
    }

    successor_enumerator enumerate_successors(গ্রন্থি_type ধ্রুবক &গ্রন্থি) ধ্রুবক {
        auto range = edge_ranges[গ্রন্থি + offset];
        ফেরত {edges.begin() + range.first, edges.begin() + range.second};
    }

    পূর্ণসংখ্যা &dfs_সূচক(গ্রন্থি_type ধ্রুবক &গ্রন্থি) {
        ফেরত indices_[গ্রন্থি + offset];
    }
};


class IntGraph {
সর্বজনীন:
    প্রকারলেখ পূর্ণসংখ্যা গ্রন্থি_type;

    কাঠামো successor_enumerator {
        std::vector<std::pair<পূর্ণসংখ্যা, পূর্ণসংখ্যা>>::const_iterator current, end;
        bool finished() ধ্রুবক { ফেরত current == end; }
        গ্রন্থি_type পরবর্তী() {
            log_assert(!finished());
            গ্রন্থি_type ফলাফল = current->second;
            ++current;
            ফেরত ফলাফল;
        }
    };

    কাঠামো গ্রন্থি_enumerator {
        পূর্ণসংখ্যা current, end;
        bool finished() ধ্রুবক { ফেরত current == end; }
        গ্রন্থি_type পরবর্তী() {
            log_assert(!finished());
            গ্রন্থি_type ফলাফল = current;
            ++current;
            ফেরত ফলাফল;
        }
    };

ব্যক্তিগত:
    std::vector<std::pair<পূর্ণসংখ্যা, পূর্ণসংখ্যা>> edges;
    std::vector<std::pair<পূর্ণসংখ্যা, পূর্ণসংখ্যা>> edge_ranges;
    std::vector<পূর্ণসংখ্যা> indices_;
    bool computed = false;

    শূন্য compute() {
        edge_ranges.clear();

        পূর্ণসংখ্যা গ্রন্থি_end = 0;
        জন্য (auto ধ্রুবক &edge : edges)
            গ্রন্থি_end = std::max(গ্রন্থি_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(গ্রন্থি_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        পূর্ণসংখ্যা range_begin = 0;
        জন্য (পূর্ণসংখ্যা গ্রন্থি = 0; গ্রন্থি != গ্রন্থি_end; ++গ্রন্থি) {
            যখন (edge != edge_end && edge->first <= গ্রন্থি)
                ++edge;
            পূর্ণসংখ্যা range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

সর্বজনীন:
    শূন্য add_edge(পূর্ণসংখ্যা src, পূর্ণসংখ্যা dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        edges.emplace_back(std::make_pair(src, dst));
    }

    গ্রন্থি_enumerator enumerate_nodes() {
        যদি (!computed) compute();
        ফেরত {0, Get_আকার(indices_)};
    }

    successor_enumerator enumerate_successors(পূর্ণসংখ্যা গ্রন্থি) ধ্রুবক {
        auto range = edge_ranges[গ্রন্থি];
        ফেরত {edges.begin() + range.first, edges.begin() + range.second};
    }

    পূর্ণসংখ্যা &dfs_সূচক(গ্রন্থি_type ধ্রুবক &গ্রন্থি) {
        ফেরত indices_[গ্রন্থি];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    প্রকারলেখ typename G::গ্রন্থি_enumerator গ্রন্থি_enumerator;
    প্রকারলেখ typename G::successor_enumerator successor_enumerator;
    প্রকারলেখ typename G::গ্রন্থি_type গ্রন্থি_type;

    কাঠামো dfs_entry {
        গ্রন্থি_type গ্রন্থি;
        successor_enumerator successors;
        পূর্ণসংখ্যা lowlink;

        dfs_entry(গ্রন্থি_type গ্রন্থি, successor_enumerator successors, পূর্ণসংখ্যা lowlink) :
            গ্রন্থি(গ্রন্থি), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_স্তূপ;
    std::vector<গ্রন্থি_type> component_স্তূপ;
    পূর্ণসংখ্যা পরবর্তী_সূচক = 0;

সর্বজনীন:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        গ্রন্থি_enumerator nodes = graph.enumerate_nodes();
        যখন (!nodes.finished()) {
            গ্রন্থি_type গ্রন্থি = nodes.পরবর্তী();
            successor_enumerator successors = graph.enumerate_successors(গ্রন্থি);
            যদি (successors.finished())
            {
                graph.dfs_সূচক(গ্রন্থি) = পরবর্তী_সূচক;
                পরবর্তী_সূচক++;
                component_স্তূপ.ঠেলো_back(গ্রন্থি);
                component(component_স্তূপ.তথ্য(), component_স্তূপ.তথ্য() + 1);
                component_স্তূপ.clear();
                graph.dfs_সূচক(গ্রন্থি) = INT_MAX;
            }
        }
        ফেরত *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        গ্রন্থি_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        যখন (!nodes.finished())
            process(nodes.পরবর্তী());
        ফেরত *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(গ্রন্থি_type গ্রন্থি) {
        // only start a new search if the node wasn't visited yet
        যদি (graph.dfs_সূচক(গ্রন্থি) >= 0)
            ফেরত *this;
        যখন (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            পূর্ণসংখ্যা lowlink = পরবর্তী_সূচক;
            পরবর্তী_সূচক++;
            graph.dfs_সূচক(গ্রন্থি) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_স্তূপ.ঠেলো_back(গ্রন্থি);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(গ্রন্থি);
            যখন (true) {
                যদি (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    যদি (lowlink == graph.dfs_সূচক(গ্রন্থি)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        পূর্ণসংখ্যা current = Get_আকার(component_স্তূপ);
                        করো {
                            --current;
                        } যখন (component_স্তূপ[current] != গ্রন্থি);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        গ্রন্থি_type *স্তূপ_ptr = component_স্তূপ.তথ্য();
                        গ্রন্থি_type *component_begin = স্তূপ_ptr + current;
                        গ্রন্থি_type *component_end = স্তূপ_ptr + component_স্তূপ.আকার_2();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_end);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        জন্য (; component_begin != component_end; ++component_begin)
                            graph.dfs_সূচক(*component_begin) = INT_MAX;
                        component_স্তূপ.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    যদি (dfs_স্তূপ.empty())
                        ফেরত *this;
                    auto &dfs_শীর্ষ = dfs_স্তূপ.back();

                    গ্রন্থি = dfs_শীর্ষ.গ্রন্থি;
                    successors = std::move(dfs_শীর্ষ.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_শীর্ষ.lowlink);
                    dfs_স্তূপ.বের_করো_back();
                    // continue checking the remaining successors of the parent node.
                } অন্যথা {
                    গ্রন্থি_type succ = successors.পরবর্তী();
                    যদি (graph.dfs_সূচক(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_স্তূপ.emplace_back(গ্রন্থি, std::move(successors), lowlink);
                        গ্রন্থি = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        বিরতি;
                    } অন্যথা {
                        // the textbook version guards this update with a check
                        // whether the successor is still on the component
                        // stack. If the successor node was already visisted
                        // but is not on the component stack, it must be part
                        // of an already emitted SCC. We can avoid this check
                        // by setting the DFS index of all nodes in a SCC to
                        // INT_MAX when the SCC is emitted.
                        lowlink = min(lowlink, graph.dfs_সূচক(succ));
                    }
                }
            }
        }
    }
};

YOSYS_NAMESPACE_END

#endif
