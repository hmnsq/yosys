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

#অন্তর্ভাৱ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SigCellGraph {
সার্ৱজনিক:
    প্রকারনির্দেশ পূর্ণাঙ্ক গ্ৰন্থি_type;

    সংরচনা successor_enumerator {
        std::vector<std::pair<পূর্ণাঙ্ক, পূর্ণাঙ্ক>>::const_iterator current, end;
        bool finished() ধ্রুৱ { ঘূৰাই current == end; }
        গ্ৰন্থি_type next() {
            log_assert(!finished());
            গ্ৰন্থি_type result = current->second;
            ++current;
            ঘূৰাই result;
        }
    };

    সংরচনা গ্ৰন্থি_enumerator {
        পূর্ণাঙ্ক current, end;
        bool finished() ধ্রুৱ { ঘূৰাই current == end; }
        গ্ৰন্থি_type next() {
            log_assert(!finished());
            গ্ৰন্থি_type result = current;
            ++current;
            ঘূৰাই result;
        }
    };

গুপ্ত:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::vector<std::pair<পূর্ণাঙ্ক, পূর্ণাঙ্ক>> edges;
    std::vector<std::pair<পূর্ণাঙ্ক, পূর্ণাঙ্ক>> edge_ranges;
    std::vector<পূর্ণাঙ্ক> indices_;
    পূর্ণাঙ্ক offset;
    bool computed = false;

    শূন্য compute() {
        offset = GetSize(sig_ids);
        edge_ranges.clear();
        indices_.clear();
        indices_.resize(GetSize(sig_ids) + GetSize(cell_ids), -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        পূর্ণাঙ্ক range_begin = 0;
        বাবে (পূর্ণাঙ্ক গ্ৰন্থি = -offset, গ্ৰন্থি_end = GetSize(cell_ids); গ্ৰন্থি != গ্ৰন্থি_end; ++গ্ৰন্থি) {
            যেতিয়ালৈ (edge != edge_end && edge->first <= গ্ৰন্থি)
                ++edge;
            পূর্ণাঙ্ক range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

সার্ৱজনিক:
    গ্ৰন্থি_type গ্ৰন্থি(RTLIL::Cell *cell) { ঘূৰাই cell_ids(cell); }
    গ্ৰন্থি_type গ্ৰন্থি(SigBit ধ্রুৱ &bit) { ঘূৰাই ~sig_ids(bit); }

    bool is_cell(গ্ৰন্থি_type গ্ৰন্থি) { ঘূৰাই গ্ৰন্থি >= 0; }
    bool is_sig(গ্ৰন্থি_type গ্ৰন্থি) { ঘূৰাই গ্ৰন্থি < 0; }

    Cell *cell(গ্ৰন্থি_type গ্ৰন্থি) { ঘূৰাই গ্ৰন্থি >= 0 ? cell_ids[গ্ৰন্থি] : nullptr; }
    SigBit sig(গ্ৰন্থি_type গ্ৰন্থি) { ঘূৰাই গ্ৰন্থি < 0 ? sig_ids[~গ্ৰন্থি] : SigBit(); }

    template<typename Src, typename Dst>
    শূন্য add_edge(Src &&src, Dst &&dst) {
        computed = false;
        গ্ৰন্থি_type src_গ্ৰন্থি = গ্ৰন্থি(std::forward<Src>(src));
        গ্ৰন্থি_type dst_গ্ৰন্থি = গ্ৰন্থি(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_গ্ৰন্থি, dst_গ্ৰন্থি));
    }

    গ্ৰন্থি_enumerator enumerate_nodes() {
        যদি (!computed) compute();
        ঘূৰাই {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(গ্ৰন্থি_type ধ্রুৱ &গ্ৰন্থি) ধ্রুৱ {
        auto range = edge_ranges[গ্ৰন্থি + offset];
        ঘূৰাই {edges.begin() + range.first, edges.begin() + range.second};
    }

    পূর্ণাঙ্ক &dfs_index(গ্ৰন্থি_type ধ্রুৱ &গ্ৰন্থি) {
        ঘূৰাই indices_[গ্ৰন্থি + offset];
    }
};


class IntGraph {
সার্ৱজনিক:
    প্রকারনির্দেশ পূর্ণাঙ্ক গ্ৰন্থি_type;

    সংরচনা successor_enumerator {
        std::vector<std::pair<পূর্ণাঙ্ক, পূর্ণাঙ্ক>>::const_iterator current, end;
        bool finished() ধ্রুৱ { ঘূৰাই current == end; }
        গ্ৰন্থি_type next() {
            log_assert(!finished());
            গ্ৰন্থি_type result = current->second;
            ++current;
            ঘূৰাই result;
        }
    };

    সংরচনা গ্ৰন্থি_enumerator {
        পূর্ণাঙ্ক current, end;
        bool finished() ধ্রুৱ { ঘূৰাই current == end; }
        গ্ৰন্থি_type next() {
            log_assert(!finished());
            গ্ৰন্থি_type result = current;
            ++current;
            ঘূৰাই result;
        }
    };

গুপ্ত:
    std::vector<std::pair<পূর্ণাঙ্ক, পূর্ণাঙ্ক>> edges;
    std::vector<std::pair<পূর্ণাঙ্ক, পূর্ণাঙ্ক>> edge_ranges;
    std::vector<পূর্ণাঙ্ক> indices_;
    bool computed = false;

    শূন্য compute() {
        edge_ranges.clear();

        পূর্ণাঙ্ক গ্ৰন্থি_end = 0;
        বাবে (auto ধ্রুৱ &edge : edges)
            গ্ৰন্থি_end = std::max(গ্ৰন্থি_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(গ্ৰন্থি_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        পূর্ণাঙ্ক range_begin = 0;
        বাবে (পূর্ণাঙ্ক গ্ৰন্থি = 0; গ্ৰন্থি != গ্ৰন্থি_end; ++গ্ৰন্থি) {
            যেতিয়ালৈ (edge != edge_end && edge->first <= গ্ৰন্থি)
                ++edge;
            পূর্ণাঙ্ক range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

সার্ৱজনিক:
    শূন্য add_edge(পূর্ণাঙ্ক src, পূর্ণাঙ্ক dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        edges.emplace_back(std::make_pair(src, dst));
    }

    গ্ৰন্থি_enumerator enumerate_nodes() {
        যদি (!computed) compute();
        ঘূৰাই {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(পূর্ণাঙ্ক গ্ৰন্থি) ধ্রুৱ {
        auto range = edge_ranges[গ্ৰন্থি];
        ঘূৰাই {edges.begin() + range.first, edges.begin() + range.second};
    }

    পূর্ণাঙ্ক &dfs_index(গ্ৰন্থি_type ধ্রুৱ &গ্ৰন্থি) {
        ঘূৰাই indices_[গ্ৰন্থি];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    প্রকারনির্দেশ typename G::গ্ৰন্থি_enumerator গ্ৰন্থি_enumerator;
    প্রকারনির্দেশ typename G::successor_enumerator successor_enumerator;
    প্রকারনির্দেশ typename G::গ্ৰন্থি_type গ্ৰন্থি_type;

    সংরচনা dfs_entry {
        গ্ৰন্থি_type গ্ৰন্থি;
        successor_enumerator successors;
        পূর্ণাঙ্ক lowlink;

        dfs_entry(গ্ৰন্থি_type গ্ৰন্থি, successor_enumerator successors, পূর্ণাঙ্ক lowlink) :
            গ্ৰন্থি(গ্ৰন্থি), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_স্তূপ;
    std::vector<গ্ৰন্থি_type> component_স্তূপ;
    পূর্ণাঙ্ক next_index = 0;

সার্ৱজনিক:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        গ্ৰন্থি_enumerator nodes = graph.enumerate_nodes();
        যেতিয়ালৈ (!nodes.finished()) {
            গ্ৰন্থি_type গ্ৰন্থি = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(গ্ৰন্থি);
            যদি (successors.finished())
            {
                graph.dfs_index(গ্ৰন্থি) = next_index;
                next_index++;
                component_স্তূপ.ঠেলো_back(গ্ৰন্থি);
                component(component_স্তূপ.data(), component_স্তূপ.data() + 1);
                component_স্তূপ.clear();
                graph.dfs_index(গ্ৰন্থি) = INT_MAX;
            }
        }
        ঘূৰাই *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        গ্ৰন্থি_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        যেতিয়ালৈ (!nodes.finished())
            process(nodes.next());
        ঘূৰাই *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(গ্ৰন্থি_type গ্ৰন্থি) {
        // only start a new search if the node wasn't visited yet
        যদি (graph.dfs_index(গ্ৰন্থি) >= 0)
            ঘূৰাই *this;
        যেতিয়ালৈ (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            পূর্ণাঙ্ক lowlink = next_index;
            next_index++;
            graph.dfs_index(গ্ৰন্থি) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_স্তূপ.ঠেলো_back(গ্ৰন্থি);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(গ্ৰন্থি);
            যেতিয়ালৈ (true) {
                যদি (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    যদি (lowlink == graph.dfs_index(গ্ৰন্থি)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        পূর্ণাঙ্ক current = GetSize(component_স্তূপ);
                        কৰা {
                            --current;
                        } যেতিয়ালৈ (component_স্তূপ[current] != গ্ৰন্থি);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        গ্ৰন্থি_type *স্তূপ_ptr = component_স্তূপ.data();
                        গ্ৰন্থি_type *component_begin = স্তূপ_ptr + current;
                        গ্ৰন্থি_type *component_end = স্তূপ_ptr + component_স্তূপ.size();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_end);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        বাবে (; component_begin != component_end; ++component_begin)
                            graph.dfs_index(*component_begin) = INT_MAX;
                        component_স্তূপ.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    যদি (dfs_স্তূপ.empty())
                        ঘূৰাই *this;
                    auto &dfs_শীৰ্ষ = dfs_স্তূপ.back();

                    গ্ৰন্থি = dfs_শীৰ্ষ.গ্ৰন্থি;
                    successors = std::move(dfs_শীৰ্ষ.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_শীৰ্ষ.lowlink);
                    dfs_স্তূপ.বেৰ_কৰো_back();
                    // continue checking the remaining successors of the parent node.
                } অন্যথা {
                    গ্ৰন্থি_type succ = successors.next();
                    যদি (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_স্তূপ.emplace_back(গ্ৰন্থি, std::move(successors), lowlink);
                        গ্ৰন্থি = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        ভঙ্গ;
                    } অন্যথা {
                        // the textbook version guards this update with a check
                        // whether the successor is still on the component
                        // stack. If the successor node was already visisted
                        // but is not on the component stack, it must be part
                        // of an already emitted SCC. We can avoid this check
                        // by setting the DFS index of all nodes in a SCC to
                        // INT_MAX when the SCC is emitted.
                        lowlink = min(lowlink, graph.dfs_index(succ));
                    }
                }
            }
        }
    }
};

YOSYS_NAMESPACE_END

#endif
