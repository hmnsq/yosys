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
    تعريفنوع صحيح كلي_type;

    بنية successor_enumerator {
        std::vector<std::pair<صحيح, صحيح>>::const_iterator current, end;
        bool finished() ثابت { أرجع current == end; }
        كلي_type next() {
            log_assert(!finished());
            كلي_type result = current->second;
            ++current;
            أرجع result;
        }
    };

    بنية كلي_enumerator {
        صحيح current, end;
        bool finished() ثابت { أرجع current == end; }
        كلي_type next() {
            log_assert(!finished());
            كلي_type result = current;
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
        لكل (صحيح كلي = -offset, كلي_end = GetSize(cell_ids); كلي != كلي_end; ++كلي) {
            طالما (edge != edge_end && edge->first <= كلي)
                ++edge;
            صحيح range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

عام:
    كلي_type كلي(RTLIL::Cell *cell) { أرجع cell_ids(cell); }
    كلي_type كلي(SigBit ثابت &bit) { أرجع ~sig_ids(bit); }

    bool is_cell(كلي_type كلي) { أرجع كلي >= 0; }
    bool is_sig(كلي_type كلي) { أرجع كلي < 0; }

    Cell *cell(كلي_type كلي) { أرجع كلي >= 0 ? cell_ids[كلي] : nullptr; }
    SigBit sig(كلي_type كلي) { أرجع كلي < 0 ? sig_ids[~كلي] : SigBit(); }

    template<typename Src, typename Dst>
    فراغ add_edge(Src &&src, Dst &&dst) {
        computed = false;
        كلي_type src_كلي = كلي(std::forward<Src>(src));
        كلي_type dst_كلي = كلي(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_كلي, dst_كلي));
    }

    كلي_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(كلي_type ثابت &كلي) ثابت {
        auto range = edge_ranges[كلي + offset];
        أرجع {edges.begin() + range.first, edges.begin() + range.second};
    }

    صحيح &dfs_index(كلي_type ثابت &كلي) {
        أرجع indices_[كلي + offset];
    }
};


class IntGraph {
عام:
    تعريفنوع صحيح كلي_type;

    بنية successor_enumerator {
        std::vector<std::pair<صحيح, صحيح>>::const_iterator current, end;
        bool finished() ثابت { أرجع current == end; }
        كلي_type next() {
            log_assert(!finished());
            كلي_type result = current->second;
            ++current;
            أرجع result;
        }
    };

    بنية كلي_enumerator {
        صحيح current, end;
        bool finished() ثابت { أرجع current == end; }
        كلي_type next() {
            log_assert(!finished());
            كلي_type result = current;
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

        صحيح كلي_end = 0;
        لكل (auto ثابت &edge : edges)
            كلي_end = std::max(كلي_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(كلي_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        صحيح range_begin = 0;
        لكل (صحيح كلي = 0; كلي != كلي_end; ++كلي) {
            طالما (edge != edge_end && edge->first <= كلي)
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

    كلي_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(صحيح كلي) ثابت {
        auto range = edge_ranges[كلي];
        أرجع {edges.begin() + range.first, edges.begin() + range.second};
    }

    صحيح &dfs_index(كلي_type ثابت &كلي) {
        أرجع indices_[كلي];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    تعريفنوع typename G::كلي_enumerator كلي_enumerator;
    تعريفنوع typename G::successor_enumerator successor_enumerator;
    تعريفنوع typename G::كلي_type كلي_type;

    بنية dfs_entry {
        كلي_type كلي;
        successor_enumerator successors;
        صحيح lowlink;

        dfs_entry(كلي_type كلي, successor_enumerator successors, صحيح lowlink) :
            كلي(كلي), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_تر;
    std::vector<كلي_type> component_تر;
    صحيح next_index = 0;

عام:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        كلي_enumerator nodes = graph.enumerate_nodes();
        طالما (!nodes.finished()) {
            كلي_type كلي = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(كلي);
            إذا (successors.finished())
            {
                graph.dfs_index(كلي) = next_index;
                next_index++;
                component_تر.تور_back(كلي);
                component(component_تر.data(), component_تر.data() + 1);
                component_تر.clear();
                graph.dfs_index(كلي) = INT_MAX;
            }
        }
        أرجع *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        كلي_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        طالما (!nodes.finished())
            process(nodes.next());
        أرجع *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(كلي_type كلي) {
        // only start a new search if the node wasn't visited yet
        إذا (graph.dfs_index(كلي) >= 0)
            أرجع *this;
        طالما (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            صحيح lowlink = next_index;
            next_index++;
            graph.dfs_index(كلي) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_تر.تور_back(كلي);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(كلي);
            طالما (true) {
                إذا (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    إذا (lowlink == graph.dfs_index(كلي)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        صحيح current = GetSize(component_تر);
                        افعل {
                            --current;
                        } طالما (component_تر[current] != كلي);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        كلي_type *تر_ptr = component_تر.data();
                        كلي_type *component_begin = تر_ptr + current;
                        كلي_type *component_end = تر_ptr + component_تر.size();

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
                        component_تر.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    إذا (dfs_تر.empty())
                        أرجع *this;
                    auto &dfs_كولي = dfs_تر.back();

                    كلي = dfs_كولي.كلي;
                    successors = std::move(dfs_كولي.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_كولي.lowlink);
                    dfs_تر.چرو_back();
                    // continue checking the remaining successors of the parent node.
                } وإلا {
                    كلي_type succ = successors.next();
                    إذا (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_تر.emplace_back(كلي, std::move(successors), lowlink);
                        كلي = succ;

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
