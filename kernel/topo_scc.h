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

#include "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SigCellGraph {
public:
    typedef ціле node_type;

    struct successor_enumerator {
        std::vector<std::pair<ціле, ціле>>::const_iterator current, end;
        bool finished() const { повернути current == end; }
        node_type next() {
            log_assert(!finished());
            node_type result = current->second;
            ++current;
            повернути result;
        }
    };

    struct node_enumerator {
        ціле current, end;
        bool finished() const { повернути current == end; }
        node_type next() {
            log_assert(!finished());
            node_type result = current;
            ++current;
            повернути result;
        }
    };

private:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::vector<std::pair<ціле, ціле>> edges;
    std::vector<std::pair<ціле, ціле>> edge_ranges;
    std::vector<ціле> indices_;
    ціле offset;
    bool computed = false;

    void compute() {
        offset = GetSize(sig_ids);
        edge_ranges.clear();
        indices_.clear();
        indices_.resize(GetSize(sig_ids) + GetSize(cell_ids), -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        ціле range_begin = 0;
        для (ціле node = -offset, node_end = GetSize(cell_ids); node != node_end; ++node) {
            поки (edge != edge_end && edge->first <= node)
                ++edge;
            ціле range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

public:
    node_type node(RTLIL::Cell *cell) { повернути cell_ids(cell); }
    node_type node(SigBit const &bit) { повернути ~sig_ids(bit); }

    bool is_cell(node_type node) { повернути node >= 0; }
    bool is_sig(node_type node) { повернути node < 0; }

    Cell *cell(node_type node) { повернути node >= 0 ? cell_ids[node] : nullptr; }
    SigBit sig(node_type node) { повернути node < 0 ? sig_ids[~node] : SigBit(); }

    template<typename Src, typename Dst>
    void add_edge(Src &&src, Dst &&dst) {
        computed = false;
        node_type src_node = node(std::forward<Src>(src));
        node_type dst_node = node(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_node, dst_node));
    }

    node_enumerator enumerate_nodes() {
        якщо (!computed) compute();
        повернути {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(node_type const &node) const {
        auto range = edge_ranges[node + offset];
        повернути {edges.begin() + range.first, edges.begin() + range.second};
    }

    ціле &dfs_index(node_type const &node) {
        повернути indices_[node + offset];
    }
};


class IntGraph {
public:
    typedef ціле node_type;

    struct successor_enumerator {
        std::vector<std::pair<ціле, ціле>>::const_iterator current, end;
        bool finished() const { повернути current == end; }
        node_type next() {
            log_assert(!finished());
            node_type result = current->second;
            ++current;
            повернути result;
        }
    };

    struct node_enumerator {
        ціле current, end;
        bool finished() const { повернути current == end; }
        node_type next() {
            log_assert(!finished());
            node_type result = current;
            ++current;
            повернути result;
        }
    };

private:
    std::vector<std::pair<ціле, ціле>> edges;
    std::vector<std::pair<ціле, ціле>> edge_ranges;
    std::vector<ціле> indices_;
    bool computed = false;

    void compute() {
        edge_ranges.clear();

        ціле node_end = 0;
        для (auto const &edge : edges)
            node_end = std::max(node_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(node_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        ціле range_begin = 0;
        для (ціле node = 0; node != node_end; ++node) {
            поки (edge != edge_end && edge->first <= node)
                ++edge;
            ціле range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

public:
    void add_edge(ціле src, ціле dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        edges.emplace_back(std::make_pair(src, dst));
    }

    node_enumerator enumerate_nodes() {
        якщо (!computed) compute();
        повернути {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(ціле node) const {
        auto range = edge_ranges[node];
        повернути {edges.begin() + range.first, edges.begin() + range.second};
    }

    ціле &dfs_index(node_type const &node) {
        повернути indices_[node];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    typedef typename G::node_enumerator node_enumerator;
    typedef typename G::successor_enumerator successor_enumerator;
    typedef typename G::node_type node_type;

    struct dfs_entry {
        node_type node;
        successor_enumerator successors;
        ціле lowlink;

        dfs_entry(node_type node, successor_enumerator successors, ціле lowlink) :
            node(node), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_stack;
    std::vector<node_type> component_stack;
    ціле next_index = 0;

public:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        node_enumerator nodes = graph.enumerate_nodes();
        поки (!nodes.finished()) {
            node_type node = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(node);
            якщо (successors.finished())
            {
                graph.dfs_index(node) = next_index;
                next_index++;
                component_stack.push_back(node);
                component(component_stack.data(), component_stack.data() + 1);
                component_stack.clear();
                graph.dfs_index(node) = INT_MAX;
            }
        }
        повернути *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        node_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        поки (!nodes.finished())
            process(nodes.next());
        повернути *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(node_type node) {
        // only start a new search if the node wasn't visited yet
        якщо (graph.dfs_index(node) >= 0)
            повернути *this;
        поки (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            ціле lowlink = next_index;
            next_index++;
            graph.dfs_index(node) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_stack.push_back(node);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(node);
            поки (true) {
                якщо (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    якщо (lowlink == graph.dfs_index(node)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        ціле current = GetSize(component_stack);
                        do {
                            --current;
                        } поки (component_stack[current] != node);

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
                        для (; component_begin != component_end; ++component_begin)
                            graph.dfs_index(*component_begin) = INT_MAX;
                        component_stack.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    якщо (dfs_stack.empty())
                        повернути *this;
                    auto &dfs_top = dfs_stack.back();

                    node = dfs_top.node;
                    successors = std::move(dfs_top.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_top.lowlink);
                    dfs_stack.pop_back();
                    // continue checking the remaining successors of the parent node.
                } інакше {
                    node_type succ = successors.next();
                    якщо (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_stack.emplace_back(node, std::move(successors), lowlink);
                        node = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        break;
                    } інакше {
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
