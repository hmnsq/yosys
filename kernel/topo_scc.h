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
    تعريفنوع صحيح تۈگۈن_type;

    بنية successor_enumerator {
        std::vector<std::pair<صحيح, صحيح>>::const_iterator current, end;
        bool finished() ثابت { أرجع current == end; }
        تۈگۈن_type next() {
            log_assert(!finished());
            تۈگۈن_type result = current->second;
            ++current;
            أرجع result;
        }
    };

    بنية تۈگۈن_enumerator {
        صحيح current, end;
        bool finished() ثابت { أرجع current == end; }
        تۈگۈن_type next() {
            log_assert(!finished());
            تۈگۈن_type result = current;
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
        لكل (صحيح تۈگۈن = -offset, تۈگۈن_end = GetSize(cell_ids); تۈگۈن != تۈگۈن_end; ++تۈگۈن) {
            طالما (edge != edge_end && edge->first <= تۈگۈن)
                ++edge;
            صحيح range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

عام:
    تۈگۈن_type تۈگۈن(RTLIL::Cell *cell) { أرجع cell_ids(cell); }
    تۈگۈن_type تۈگۈن(SigBit ثابت &bit) { أرجع ~sig_ids(bit); }

    bool is_cell(تۈگۈن_type تۈگۈن) { أرجع تۈگۈن >= 0; }
    bool is_sig(تۈگۈن_type تۈگۈن) { أرجع تۈگۈن < 0; }

    Cell *cell(تۈگۈن_type تۈگۈن) { أرجع تۈگۈن >= 0 ? cell_ids[تۈگۈن] : nullptr; }
    SigBit sig(تۈگۈن_type تۈگۈن) { أرجع تۈگۈن < 0 ? sig_ids[~تۈگۈن] : SigBit(); }

    template<typename Src, typename Dst>
    فراغ add_edge(Src &&src, Dst &&dst) {
        computed = false;
        تۈگۈن_type src_تۈگۈن = تۈگۈن(std::forward<Src>(src));
        تۈگۈن_type dst_تۈگۈن = تۈگۈن(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_تۈگۈن, dst_تۈگۈن));
    }

    تۈگۈن_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(تۈگۈن_type ثابت &تۈگۈن) ثابت {
        auto range = edge_ranges[تۈگۈن + offset];
        أرجع {edges.begin() + range.first, edges.begin() + range.second};
    }

    صحيح &dfs_index(تۈگۈن_type ثابت &تۈگۈن) {
        أرجع indices_[تۈگۈن + offset];
    }
};


class IntGraph {
عام:
    تعريفنوع صحيح تۈگۈن_type;

    بنية successor_enumerator {
        std::vector<std::pair<صحيح, صحيح>>::const_iterator current, end;
        bool finished() ثابت { أرجع current == end; }
        تۈگۈن_type next() {
            log_assert(!finished());
            تۈگۈن_type result = current->second;
            ++current;
            أرجع result;
        }
    };

    بنية تۈگۈن_enumerator {
        صحيح current, end;
        bool finished() ثابت { أرجع current == end; }
        تۈگۈن_type next() {
            log_assert(!finished());
            تۈگۈن_type result = current;
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

        صحيح تۈگۈن_end = 0;
        لكل (auto ثابت &edge : edges)
            تۈگۈن_end = std::max(تۈگۈن_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(تۈگۈن_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        صحيح range_begin = 0;
        لكل (صحيح تۈگۈن = 0; تۈگۈن != تۈگۈن_end; ++تۈگۈن) {
            طالما (edge != edge_end && edge->first <= تۈگۈن)
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

    تۈگۈن_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(صحيح تۈگۈن) ثابت {
        auto range = edge_ranges[تۈگۈن];
        أرجع {edges.begin() + range.first, edges.begin() + range.second};
    }

    صحيح &dfs_index(تۈگۈن_type ثابت &تۈگۈن) {
        أرجع indices_[تۈگۈن];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    تعريفنوع typename G::تۈگۈن_enumerator تۈگۈن_enumerator;
    تعريفنوع typename G::successor_enumerator successor_enumerator;
    تعريفنوع typename G::تۈگۈن_type تۈگۈن_type;

    بنية dfs_entry {
        تۈگۈن_type تۈگۈن;
        successor_enumerator successors;
        صحيح lowlink;

        dfs_entry(تۈگۈن_type تۈگۈن, successor_enumerator successors, صحيح lowlink) :
            تۈگۈن(تۈگۈن), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_دۆۋە;
    std::vector<تۈگۈن_type> component_دۆۋە;
    صحيح next_index = 0;

عام:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        تۈگۈن_enumerator nodes = graph.enumerate_nodes();
        طالما (!nodes.finished()) {
            تۈگۈن_type تۈگۈن = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(تۈگۈن);
            إذا (successors.finished())
            {
                graph.dfs_index(تۈگۈن) = next_index;
                next_index++;
                component_دۆۋە.ئىتتىرىش_back(تۈگۈن);
                component(component_دۆۋە.data(), component_دۆۋە.data() + 1);
                component_دۆۋە.clear();
                graph.dfs_index(تۈگۈن) = INT_MAX;
            }
        }
        أرجع *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        تۈگۈن_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        طالما (!nodes.finished())
            process(nodes.next());
        أرجع *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(تۈگۈن_type تۈگۈن) {
        // only start a new search if the node wasn't visited yet
        إذا (graph.dfs_index(تۈگۈن) >= 0)
            أرجع *this;
        طالما (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            صحيح lowlink = next_index;
            next_index++;
            graph.dfs_index(تۈگۈن) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_دۆۋە.ئىتتىرىش_back(تۈگۈن);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(تۈگۈن);
            طالما (true) {
                إذا (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    إذا (lowlink == graph.dfs_index(تۈگۈن)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        صحيح current = GetSize(component_دۆۋە);
                        افعل {
                            --current;
                        } طالما (component_دۆۋە[current] != تۈگۈن);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        تۈگۈن_type *دۆۋە_ptr = component_دۆۋە.data();
                        تۈگۈن_type *component_begin = دۆۋە_ptr + current;
                        تۈگۈن_type *component_end = دۆۋە_ptr + component_دۆۋە.size();

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
                        component_دۆۋە.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    إذا (dfs_دۆۋە.empty())
                        أرجع *this;
                    auto &dfs_چوققا = dfs_دۆۋە.back();

                    تۈگۈن = dfs_چوققا.تۈگۈن;
                    successors = std::move(dfs_چوققا.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_چوققا.lowlink);
                    dfs_دۆۋە.چىقىرىش_back();
                    // continue checking the remaining successors of the parent node.
                } وإلا {
                    تۈگۈن_type succ = successors.next();
                    إذا (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_دۆۋە.emplace_back(تۈگۈن, std::move(successors), lowlink);
                        تۈگۈن = succ;

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
