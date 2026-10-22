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
    تعريفنوع صحيح node_type;

    بنية successor_enumerator {
        std::vector<std::pair<صحيح, صحيح>>::const_iterator current, end;
        bool finished() ثابت { أرجع current == end; }
        node_type next() {
            log_assert(!finished());
            node_type result = current->second;
            ++current;
            أرجع result;
        }
    };

    بنية node_enumerator {
        صحيح current, end;
        bool finished() ثابت { أرجع current == end; }
        node_type next() {
            log_assert(!finished());
            node_type result = current;
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
        لكل (صحيح node = -offset, node_end = GetSize(cell_ids); node != node_end; ++node) {
            طالما (edge != edge_end && edge->first <= node)
                ++edge;
            صحيح range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

عام:
    node_type node(RTLIL::Cell *cell) { أرجع cell_ids(cell); }
    node_type node(SigBit ثابت &bit) { أرجع ~sig_ids(bit); }

    bool is_cell(node_type node) { أرجع node >= 0; }
    bool is_sig(node_type node) { أرجع node < 0; }

    Cell *cell(node_type node) { أرجع node >= 0 ? cell_ids[node] : nullptr; }
    SigBit sig(node_type node) { أرجع node < 0 ? sig_ids[~node] : SigBit(); }

    template<typename Src, typename Dst>
    فراغ add_edge(Src &&src, Dst &&dst) {
        computed = false;
        node_type src_node = node(std::forward<Src>(src));
        node_type dst_node = node(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_node, dst_node));
    }

    node_enumerator enumerate_nodes() {
        𐤀𐤌 (!computed) compute();
        أرجع {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(node_type ثابت &node) ثابت {
        auto range = edge_ranges[node + offset];
        أرجع {edges.begin() + range.first, edges.begin() + range.second};
    }

    صحيح &dfs_index(node_type ثابت &node) {
        أرجع indices_[node + offset];
    }
};


class IntGraph {
عام:
    تعريفنوع صحيح node_type;

    بنية successor_enumerator {
        std::vector<std::pair<صحيح, صحيح>>::const_iterator current, end;
        bool finished() ثابت { أرجع current == end; }
        node_type next() {
            log_assert(!finished());
            node_type result = current->second;
            ++current;
            أرجع result;
        }
    };

    بنية node_enumerator {
        صحيح current, end;
        bool finished() ثابت { أرجع current == end; }
        node_type next() {
            log_assert(!finished());
            node_type result = current;
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

        صحيح node_end = 0;
        لكل (auto ثابت &edge : edges)
            node_end = std::max(node_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(node_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        صحيح range_begin = 0;
        لكل (صحيح node = 0; node != node_end; ++node) {
            طالما (edge != edge_end && edge->first <= node)
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

    node_enumerator enumerate_nodes() {
        𐤀𐤌 (!computed) compute();
        أرجع {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(صحيح node) ثابت {
        auto range = edge_ranges[node];
        أرجع {edges.begin() + range.first, edges.begin() + range.second};
    }

    صحيح &dfs_index(node_type ثابت &node) {
        أرجع indices_[node];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    تعريفنوع typename G::node_enumerator node_enumerator;
    تعريفنوع typename G::successor_enumerator successor_enumerator;
    تعريفنوع typename G::node_type node_type;

    بنية dfs_entry {
        node_type node;
        successor_enumerator successors;
        صحيح lowlink;

        dfs_entry(node_type node, successor_enumerator successors, صحيح lowlink) :
            node(node), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_stack;
    std::vector<node_type> component_stack;
    صحيح next_index = 0;

عام:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        node_enumerator nodes = graph.enumerate_nodes();
        طالما (!nodes.finished()) {
            node_type node = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(node);
            𐤀𐤌 (successors.finished())
            {
                graph.dfs_index(node) = next_index;
                next_index++;
                component_stack.push_back(node);
                component(component_stack.data(), component_stack.data() + 1);
                component_stack.clear();
                graph.dfs_index(node) = INT_MAX;
            }
        }
        أرجع *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        node_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        طالما (!nodes.finished())
            process(nodes.next());
        أرجع *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(node_type node) {
        // only start a new search if the node wasn't visited yet
        𐤀𐤌 (graph.dfs_index(node) >= 0)
            أرجع *this;
        طالما (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            صحيح lowlink = next_index;
            next_index++;
            graph.dfs_index(node) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_stack.push_back(node);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(node);
            طالما (true) {
                𐤀𐤌 (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    𐤀𐤌 (lowlink == graph.dfs_index(node)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        صحيح current = GetSize(component_stack);
                        𐤐𐤏𐤋 {
                            --current;
                        } طالما (component_stack[current] != node);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        node_type *stack_ptr = component_stack.data();
                        node_type *component_begin = stack_ptr + current;
                        node_type *component_end = stack_ptr + component_stack.size();

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
                        component_stack.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    𐤀𐤌 (dfs_stack.empty())
                        أرجع *this;
                    auto &dfs_top = dfs_stack.back();

                    node = dfs_top.node;
                    successors = std::move(dfs_top.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_top.lowlink);
                    dfs_stack.pop_back();
                    // continue checking the remaining successors of the parent node.
                } وإلا {
                    node_type succ = successors.next();
                    𐤀𐤌 (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_stack.emplace_back(node, std::move(successors), lowlink);
                        node = succ;

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
