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

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SigCellGraph {
عام:
    تعريفنوع صحيح گرێ_type;

    بنية successor_enumerator {
        std::vector<std::pair<صحيح, صحيح>>::const_iterator current, end;
        bool finished() ثابت { أرجع current == end; }
        گرێ_type next() {
            log_assert(!finished());
            گرێ_type result = current->second;
            ++current;
            أرجع result;
        }
    };

    بنية گرێ_enumerator {
        صحيح current, end;
        bool finished() ثابت { أرجع current == end; }
        گرێ_type next() {
            log_assert(!finished());
            گرێ_type result = current;
            ++current;
            أرجع result;
        }
    };

خاص:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::vector<std::pair<صحيح, صحيح>> edges;
    std::vector<std::pair<صحيح, صحيح>> edge_ranges;
    std::vector<صحيح> indices_;
    صحيح offset;
    bool computed = false;

    فراغ compute() {
        offset = GetSize(sig_ids);
        edge_ranges.clear();
        indices_.clear();
        indices_.resize(GetSize(sig_ids) + GetSize(cell_ids), -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        صحيح range_begin = 0;
        لكل (صحيح گرێ = -offset, گرێ_end = GetSize(cell_ids); گرێ != گرێ_end; ++گرێ) {
            طالما (edge != edge_end && edge->first <= گرێ)
                ++edge;
            صحيح range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

عام:
    گرێ_type گرێ(RTLIL::Cell *cell) { أرجع cell_ids(cell); }
    گرێ_type گرێ(SigBit ثابت &bit) { أرجع ~sig_ids(bit); }

    bool is_cell(گرێ_type گرێ) { أرجع گرێ >= 0; }
    bool is_sig(گرێ_type گرێ) { أرجع گرێ < 0; }

    Cell *cell(گرێ_type گرێ) { أرجع گرێ >= 0 ? cell_ids[گرێ] : nullptr; }
    SigBit sig(گرێ_type گرێ) { أرجع گرێ < 0 ? sig_ids[~گرێ] : SigBit(); }

    template<typename Src, typename Dst>
    فراغ add_edge(Src &&src, Dst &&dst) {
        computed = false;
        گرێ_type src_گرێ = گرێ(std::forward<Src>(src));
        گرێ_type dst_گرێ = گرێ(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_گرێ, dst_گرێ));
    }

    گرێ_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(گرێ_type ثابت &گرێ) ثابت {
        auto range = edge_ranges[گرێ + offset];
        أرجع {edges.begin() + range.first, edges.begin() + range.second};
    }

    صحيح &dfs_index(گرێ_type ثابت &گرێ) {
        أرجع indices_[گرێ + offset];
    }
};


class IntGraph {
عام:
    تعريفنوع صحيح گرێ_type;

    بنية successor_enumerator {
        std::vector<std::pair<صحيح, صحيح>>::const_iterator current, end;
        bool finished() ثابت { أرجع current == end; }
        گرێ_type next() {
            log_assert(!finished());
            گرێ_type result = current->second;
            ++current;
            أرجع result;
        }
    };

    بنية گرێ_enumerator {
        صحيح current, end;
        bool finished() ثابت { أرجع current == end; }
        گرێ_type next() {
            log_assert(!finished());
            گرێ_type result = current;
            ++current;
            أرجع result;
        }
    };

خاص:
    std::vector<std::pair<صحيح, صحيح>> edges;
    std::vector<std::pair<صحيح, صحيح>> edge_ranges;
    std::vector<صحيح> indices_;
    bool computed = false;

    فراغ compute() {
        edge_ranges.clear();

        صحيح گرێ_end = 0;
        لكل (auto ثابت &edge : edges)
            گرێ_end = std::max(گرێ_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(گرێ_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        صحيح range_begin = 0;
        لكل (صحيح گرێ = 0; گرێ != گرێ_end; ++گرێ) {
            طالما (edge != edge_end && edge->first <= گرێ)
                ++edge;
            صحيح range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

عام:
    فراغ add_edge(صحيح src, صحيح dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        edges.emplace_back(std::make_pair(src, dst));
    }

    گرێ_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(صحيح گرێ) ثابت {
        auto range = edge_ranges[گرێ];
        أرجع {edges.begin() + range.first, edges.begin() + range.second};
    }

    صحيح &dfs_index(گرێ_type ثابت &گرێ) {
        أرجع indices_[گرێ];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    تعريفنوع typename G::گرێ_enumerator گرێ_enumerator;
    تعريفنوع typename G::successor_enumerator successor_enumerator;
    تعريفنوع typename G::گرێ_type گرێ_type;

    بنية dfs_entry {
        گرێ_type گرێ;
        successor_enumerator successors;
        صحيح lowlink;

        dfs_entry(گرێ_type گرێ, successor_enumerator successors, صحيح lowlink) :
            گرێ(گرێ), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_کەڵەکە;
    std::vector<گرێ_type> component_کەڵەکە;
    صحيح next_index = 0;

عام:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        گرێ_enumerator nodes = graph.enumerate_nodes();
        طالما (!nodes.finished()) {
            گرێ_type گرێ = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(گرێ);
            إذا (successors.finished())
            {
                graph.dfs_index(گرێ) = next_index;
                next_index++;
                component_کەڵەکە.پاڵنان_back(گرێ);
                component(component_کەڵەکە.data(), component_کەڵەکە.data() + 1);
                component_کەڵەکە.clear();
                graph.dfs_index(گرێ) = INT_MAX;
            }
        }
        أرجع *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        گرێ_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        طالما (!nodes.finished())
            process(nodes.next());
        أرجع *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(گرێ_type گرێ) {
        // only start a new search if the node wasn't visited yet
        إذا (graph.dfs_index(گرێ) >= 0)
            أرجع *this;
        طالما (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            صحيح lowlink = next_index;
            next_index++;
            graph.dfs_index(گرێ) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_کەڵەکە.پاڵنان_back(گرێ);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(گرێ);
            طالما (true) {
                إذا (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    إذا (lowlink == graph.dfs_index(گرێ)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        صحيح current = GetSize(component_کەڵەکە);
                        افعل {
                            --current;
                        } طالما (component_کەڵەکە[current] != گرێ);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        گرێ_type *کەڵەکە_ptr = component_کەڵەکە.data();
                        گرێ_type *component_begin = کەڵەکە_ptr + current;
                        گرێ_type *component_end = کەڵەکە_ptr + component_کەڵەکە.size();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_end);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        لكل (; component_begin != component_end; ++component_begin)
                            graph.dfs_index(*component_begin) = INT_MAX;
                        component_کەڵەکە.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    إذا (dfs_کەڵەکە.empty())
                        أرجع *this;
                    auto &dfs_لووتکە = dfs_کەڵەکە.back();

                    گرێ = dfs_لووتکە.گرێ;
                    successors = std::move(dfs_لووتکە.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_لووتکە.lowlink);
                    dfs_کەڵەکە.دەرهێنان_back();
                    // continue checking the remaining successors of the parent node.
                } وإلا {
                    گرێ_type succ = successors.next();
                    إذا (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_کەڵەکە.emplace_back(گرێ, std::move(successors), lowlink);
                        گرێ = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        اكسر;
                    } وإلا {
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
