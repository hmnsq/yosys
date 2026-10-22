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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SigCellGraph {
ಸಾರ್ವಜನಿಕ:
    ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ ಗಂಟು_type;

    ಸಂರಚನಾ successor_enumerator {
        std::vector<std::pair<ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ>>::const_iterator current, end;
        bool finished() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ current == end; }
        ಗಂಟು_type next() {
            log_assert(!finished());
            ಗಂಟು_type result = current->second;
            ++current;
            ಪ್ರತ್ಯಾಗಮ result;
        }
    };

    ಸಂರಚನಾ ಗಂಟು_enumerator {
        ಪೂರ್ಣಾಙ್ಕ current, end;
        bool finished() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ current == end; }
        ಗಂಟು_type next() {
            log_assert(!finished());
            ಗಂಟು_type result = current;
            ++current;
            ಪ್ರತ್ಯಾಗಮ result;
        }
    };

ಗುಪ್ತ:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::vector<std::pair<ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ>> edges;
    std::vector<std::pair<ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ>> edge_ranges;
    std::vector<ಪೂರ್ಣಾಙ್ಕ> indices_;
    ಪೂರ್ಣಾಙ್ಕ offset;
    bool computed = false;

    ಶೂನ್ಯ compute() {
        offset = GetSize(sig_ids);
        edge_ranges.clear();
        indices_.clear();
        indices_.resize(GetSize(sig_ids) + GetSize(cell_ids), -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        ಪೂರ್ಣಾಙ್ಕ range_begin = 0;
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ ಗಂಟು = -offset, ಗಂಟು_end = GetSize(cell_ids); ಗಂಟು != ಗಂಟು_end; ++ಗಂಟು) {
            ಯಾವತ್ (edge != edge_end && edge->first <= ಗಂಟು)
                ++edge;
            ಪೂರ್ಣಾಙ್ಕ range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

ಸಾರ್ವಜನಿಕ:
    ಗಂಟು_type ಗಂಟು(RTLIL::Cell *cell) { ಪ್ರತ್ಯಾಗಮ cell_ids(cell); }
    ಗಂಟು_type ಗಂಟು(SigBit ಧ್ರುವ &bit) { ಪ್ರತ್ಯಾಗಮ ~sig_ids(bit); }

    bool is_cell(ಗಂಟು_type ಗಂಟು) { ಪ್ರತ್ಯಾಗಮ ಗಂಟು >= 0; }
    bool is_sig(ಗಂಟು_type ಗಂಟು) { ಪ್ರತ್ಯಾಗಮ ಗಂಟು < 0; }

    Cell *cell(ಗಂಟು_type ಗಂಟು) { ಪ್ರತ್ಯಾಗಮ ಗಂಟು >= 0 ? cell_ids[ಗಂಟು] : nullptr; }
    SigBit sig(ಗಂಟು_type ಗಂಟು) { ಪ್ರತ್ಯಾಗಮ ಗಂಟು < 0 ? sig_ids[~ಗಂಟು] : SigBit(); }

    template<typename Src, typename Dst>
    ಶೂನ್ಯ add_edge(Src &&src, Dst &&dst) {
        computed = false;
        ಗಂಟು_type src_ಗಂಟು = ಗಂಟು(std::forward<Src>(src));
        ಗಂಟು_type dst_ಗಂಟು = ಗಂಟು(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_ಗಂಟು, dst_ಗಂಟು));
    }

    ಗಂಟು_enumerator enumerate_nodes() {
        ಯದಿ (!computed) compute();
        ಪ್ರತ್ಯಾಗಮ {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(ಗಂಟು_type ಧ್ರುವ &ಗಂಟು) ಧ್ರುವ {
        auto range = edge_ranges[ಗಂಟು + offset];
        ಪ್ರತ್ಯಾಗಮ {edges.begin() + range.first, edges.begin() + range.second};
    }

    ಪೂರ್ಣಾಙ್ಕ &dfs_index(ಗಂಟು_type ಧ್ರುವ &ಗಂಟು) {
        ಪ್ರತ್ಯಾಗಮ indices_[ಗಂಟು + offset];
    }
};


class IntGraph {
ಸಾರ್ವಜನಿಕ:
    ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ ಗಂಟು_type;

    ಸಂರಚನಾ successor_enumerator {
        std::vector<std::pair<ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ>>::const_iterator current, end;
        bool finished() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ current == end; }
        ಗಂಟು_type next() {
            log_assert(!finished());
            ಗಂಟು_type result = current->second;
            ++current;
            ಪ್ರತ್ಯಾಗಮ result;
        }
    };

    ಸಂರಚನಾ ಗಂಟು_enumerator {
        ಪೂರ್ಣಾಙ್ಕ current, end;
        bool finished() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ current == end; }
        ಗಂಟು_type next() {
            log_assert(!finished());
            ಗಂಟು_type result = current;
            ++current;
            ಪ್ರತ್ಯಾಗಮ result;
        }
    };

ಗುಪ್ತ:
    std::vector<std::pair<ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ>> edges;
    std::vector<std::pair<ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ>> edge_ranges;
    std::vector<ಪೂರ್ಣಾಙ್ಕ> indices_;
    bool computed = false;

    ಶೂನ್ಯ compute() {
        edge_ranges.clear();

        ಪೂರ್ಣಾಙ್ಕ ಗಂಟು_end = 0;
        ಪ್ರತಿ (auto ಧ್ರುವ &edge : edges)
            ಗಂಟು_end = std::max(ಗಂಟು_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(ಗಂಟು_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        ಪೂರ್ಣಾಙ್ಕ range_begin = 0;
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ ಗಂಟು = 0; ಗಂಟು != ಗಂಟು_end; ++ಗಂಟು) {
            ಯಾವತ್ (edge != edge_end && edge->first <= ಗಂಟು)
                ++edge;
            ಪೂರ್ಣಾಙ್ಕ range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

ಸಾರ್ವಜನಿಕ:
    ಶೂನ್ಯ add_edge(ಪೂರ್ಣಾಙ್ಕ src, ಪೂರ್ಣಾಙ್ಕ dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        edges.emplace_back(std::make_pair(src, dst));
    }

    ಗಂಟು_enumerator enumerate_nodes() {
        ಯದಿ (!computed) compute();
        ಪ್ರತ್ಯಾಗಮ {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(ಪೂರ್ಣಾಙ್ಕ ಗಂಟು) ಧ್ರುವ {
        auto range = edge_ranges[ಗಂಟು];
        ಪ್ರತ್ಯಾಗಮ {edges.begin() + range.first, edges.begin() + range.second};
    }

    ಪೂರ್ಣಾಙ್ಕ &dfs_index(ಗಂಟು_type ಧ್ರುವ &ಗಂಟು) {
        ಪ್ರತ್ಯಾಗಮ indices_[ಗಂಟು];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    ಪ್ರಕಾರನಿರ್ದೇಶ typename G::ಗಂಟು_enumerator ಗಂಟು_enumerator;
    ಪ್ರಕಾರನಿರ್ದೇಶ typename G::successor_enumerator successor_enumerator;
    ಪ್ರಕಾರನಿರ್ದೇಶ typename G::ಗಂಟು_type ಗಂಟು_type;

    ಸಂರಚನಾ dfs_entry {
        ಗಂಟು_type ಗಂಟು;
        successor_enumerator successors;
        ಪೂರ್ಣಾಙ್ಕ lowlink;

        dfs_entry(ಗಂಟು_type ಗಂಟು, successor_enumerator successors, ಪೂರ್ಣಾಙ್ಕ lowlink) :
            ಗಂಟು(ಗಂಟು), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_ರಾಶಿ;
    std::vector<ಗಂಟು_type> component_ರಾಶಿ;
    ಪೂರ್ಣಾಙ್ಕ next_index = 0;

ಸಾರ್ವಜನಿಕ:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        ಗಂಟು_enumerator nodes = graph.enumerate_nodes();
        ಯಾವತ್ (!nodes.finished()) {
            ಗಂಟು_type ಗಂಟು = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(ಗಂಟು);
            ಯದಿ (successors.finished())
            {
                graph.dfs_index(ಗಂಟು) = next_index;
                next_index++;
                component_ರಾಶಿ.ತಳ್ಳು_back(ಗಂಟು);
                component(component_ರಾಶಿ.data(), component_ರಾಶಿ.data() + 1);
                component_ರಾಶಿ.clear();
                graph.dfs_index(ಗಂಟು) = INT_MAX;
            }
        }
        ಪ್ರತ್ಯಾಗಮ *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        ಗಂಟು_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        ಯಾವತ್ (!nodes.finished())
            process(nodes.next());
        ಪ್ರತ್ಯಾಗಮ *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(ಗಂಟು_type ಗಂಟು) {
        // only start a new search if the node wasn't visited yet
        ಯದಿ (graph.dfs_index(ಗಂಟು) >= 0)
            ಪ್ರತ್ಯಾಗಮ *this;
        ಯಾವತ್ (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            ಪೂರ್ಣಾಙ್ಕ lowlink = next_index;
            next_index++;
            graph.dfs_index(ಗಂಟು) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_ರಾಶಿ.ತಳ್ಳು_back(ಗಂಟು);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(ಗಂಟು);
            ಯಾವತ್ (true) {
                ಯದಿ (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    ಯದಿ (lowlink == graph.dfs_index(ಗಂಟು)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        ಪೂರ್ಣಾಙ್ಕ current = GetSize(component_ರಾಶಿ);
                        ಕುರು {
                            --current;
                        } ಯಾವತ್ (component_ರಾಶಿ[current] != ಗಂಟು);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        ಗಂಟು_type *ರಾಶಿ_ptr = component_ರಾಶಿ.data();
                        ಗಂಟು_type *component_begin = ರಾಶಿ_ptr + current;
                        ಗಂಟು_type *component_end = ರಾಶಿ_ptr + component_ರಾಶಿ.size();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_end);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        ಪ್ರತಿ (; component_begin != component_end; ++component_begin)
                            graph.dfs_index(*component_begin) = INT_MAX;
                        component_ರಾಶಿ.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    ಯದಿ (dfs_ರಾಶಿ.empty())
                        ಪ್ರತ್ಯಾಗಮ *this;
                    auto &dfs_ತುದಿ = dfs_ರಾಶಿ.back();

                    ಗಂಟು = dfs_ತುದಿ.ಗಂಟು;
                    successors = std::move(dfs_ತುದಿ.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_ತುದಿ.lowlink);
                    dfs_ರಾಶಿ.ತೆಗೆ_back();
                    // continue checking the remaining successors of the parent node.
                } ಅನ್ಯಥಾ {
                    ಗಂಟು_type succ = successors.next();
                    ಯದಿ (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_ರಾಶಿ.emplace_back(ಗಂಟು, std::move(successors), lowlink);
                        ಗಂಟು = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        ಭಙ್ಗ;
                    } ಅನ್ಯಥಾ {
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
