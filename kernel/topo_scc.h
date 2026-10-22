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
    تعريفنوع صحيح عقدة_type;

    بنية successor_enumerator {
        std::vector<std::pair<صحيح, صحيح>>::const_iterator current, end;
        bool finished() ثابت { أرجع current == end; }
        عقدة_type next() {
            log_assert(!finished());
            عقدة_type result = current->second;
            ++current;
            أرجع result;
        }
    };

    بنية عقدة_enumerator {
        صحيح current, end;
        bool finished() ثابت { أرجع current == end; }
        عقدة_type next() {
            log_assert(!finished());
            عقدة_type result = current;
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
        لكل (صحيح عقدة = -offset, عقدة_end = GetSize(cell_ids); عقدة != عقدة_end; ++عقدة) {
            طالما (edge != edge_end && edge->first <= عقدة)
                ++edge;
            صحيح range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

عام:
    عقدة_type عقدة(RTLIL::Cell *cell) { أرجع cell_ids(cell); }
    عقدة_type عقدة(SigBit ثابت &bit) { أرجع ~sig_ids(bit); }

    bool is_cell(عقدة_type عقدة) { أرجع عقدة >= 0; }
    bool is_sig(عقدة_type عقدة) { أرجع عقدة < 0; }

    Cell *cell(عقدة_type عقدة) { أرجع عقدة >= 0 ? cell_ids[عقدة] : nullptr; }
    SigBit sig(عقدة_type عقدة) { أرجع عقدة < 0 ? sig_ids[~عقدة] : SigBit(); }

    template<typename Src, typename Dst>
    فراغ add_edge(Src &&src, Dst &&dst) {
        computed = false;
        عقدة_type src_عقدة = عقدة(std::forward<Src>(src));
        عقدة_type dst_عقدة = عقدة(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_عقدة, dst_عقدة));
    }

    عقدة_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(عقدة_type ثابت &عقدة) ثابت {
        auto range = edge_ranges[عقدة + offset];
        أرجع {edges.begin() + range.first, edges.begin() + range.second};
    }

    صحيح &dfs_index(عقدة_type ثابت &عقدة) {
        أرجع indices_[عقدة + offset];
    }
};


class IntGraph {
عام:
    تعريفنوع صحيح عقدة_type;

    بنية successor_enumerator {
        std::vector<std::pair<صحيح, صحيح>>::const_iterator current, end;
        bool finished() ثابت { أرجع current == end; }
        عقدة_type next() {
            log_assert(!finished());
            عقدة_type result = current->second;
            ++current;
            أرجع result;
        }
    };

    بنية عقدة_enumerator {
        صحيح current, end;
        bool finished() ثابت { أرجع current == end; }
        عقدة_type next() {
            log_assert(!finished());
            عقدة_type result = current;
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

        صحيح عقدة_end = 0;
        لكل (auto ثابت &edge : edges)
            عقدة_end = std::max(عقدة_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(عقدة_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        صحيح range_begin = 0;
        لكل (صحيح عقدة = 0; عقدة != عقدة_end; ++عقدة) {
            طالما (edge != edge_end && edge->first <= عقدة)
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

    عقدة_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(صحيح عقدة) ثابت {
        auto range = edge_ranges[عقدة];
        أرجع {edges.begin() + range.first, edges.begin() + range.second};
    }

    صحيح &dfs_index(عقدة_type ثابت &عقدة) {
        أرجع indices_[عقدة];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    تعريفنوع typename G::عقدة_enumerator عقدة_enumerator;
    تعريفنوع typename G::successor_enumerator successor_enumerator;
    تعريفنوع typename G::عقدة_type عقدة_type;

    بنية dfs_entry {
        عقدة_type عقدة;
        successor_enumerator successors;
        صحيح lowlink;

        dfs_entry(عقدة_type عقدة, successor_enumerator successors, صحيح lowlink) :
            عقدة(عقدة), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_مكدس;
    std::vector<عقدة_type> component_مكدس;
    صحيح next_index = 0;

عام:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        عقدة_enumerator nodes = graph.enumerate_nodes();
        طالما (!nodes.finished()) {
            عقدة_type عقدة = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(عقدة);
            إذا (successors.finished())
            {
                graph.dfs_index(عقدة) = next_index;
                next_index++;
                component_مكدس.ادفع_back(عقدة);
                component(component_مكدس.data(), component_مكدس.data() + 1);
                component_مكدس.clear();
                graph.dfs_index(عقدة) = INT_MAX;
            }
        }
        أرجع *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        عقدة_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        طالما (!nodes.finished())
            process(nodes.next());
        أرجع *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(عقدة_type عقدة) {
        // only start a new search if the node wasn't visited yet
        إذا (graph.dfs_index(عقدة) >= 0)
            أرجع *this;
        طالما (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            صحيح lowlink = next_index;
            next_index++;
            graph.dfs_index(عقدة) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_مكدس.ادفع_back(عقدة);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(عقدة);
            طالما (true) {
                إذا (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    إذا (lowlink == graph.dfs_index(عقدة)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        صحيح current = GetSize(component_مكدس);
                        افعل {
                            --current;
                        } طالما (component_مكدس[current] != عقدة);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        عقدة_type *مكدس_ptr = component_مكدس.data();
                        عقدة_type *component_begin = مكدس_ptr + current;
                        عقدة_type *component_end = مكدس_ptr + component_مكدس.size();

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
                        component_مكدس.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    إذا (dfs_مكدس.empty())
                        أرجع *this;
                    auto &dfs_قمة = dfs_مكدس.back();

                    عقدة = dfs_قمة.عقدة;
                    successors = std::move(dfs_قمة.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_قمة.lowlink);
                    dfs_مكدس.اسحب_back();
                    // continue checking the remaining successors of the parent node.
                } وإلا {
                    عقدة_type succ = successors.next();
                    إذا (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_مكدس.emplace_back(عقدة, std::move(successors), lowlink);
                        عقدة = succ;

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
