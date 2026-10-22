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

#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SigCellGraph {
عمومی:
    تعریفنوع صحیح گره_type;

    ساختار successor_enumerator {
        std::vector<std::pair<صحیح, صحیح>>::const_iterator current, end;
        bool finished() ثابت { بازگشت current == end; }
        گره_type next() {
            log_assert(!finished());
            گره_type result = current->second;
            ++current;
            بازگشت result;
        }
    };

    ساختار گره_enumerator {
        صحیح current, end;
        bool finished() ثابت { بازگشت current == end; }
        گره_type next() {
            log_assert(!finished());
            گره_type result = current;
            ++current;
            بازگشت result;
        }
    };

خصوصی:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::vector<std::pair<صحیح, صحیح>> edges;
    std::vector<std::pair<صحیح, صحیح>> edge_ranges;
    std::vector<صحیح> indices_;
    صحیح offset;
    bool computed = false;

    تهی compute() {
        offset = GetSize(sig_ids);
        edge_ranges.clear();
        indices_.clear();
        indices_.resize(GetSize(sig_ids) + GetSize(cell_ids), -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        صحیح range_begin = 0;
        برای (صحیح گره = -offset, گره_end = GetSize(cell_ids); گره != گره_end; ++گره) {
            وقتی (edge != edge_end && edge->first <= گره)
                ++edge;
            صحیح range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

عمومی:
    گره_type گره(RTLIL::Cell *cell) { بازگشت cell_ids(cell); }
    گره_type گره(SigBit ثابت &bit) { بازگشت ~sig_ids(bit); }

    bool is_cell(گره_type گره) { بازگشت گره >= 0; }
    bool is_sig(گره_type گره) { بازگشت گره < 0; }

    Cell *cell(گره_type گره) { بازگشت گره >= 0 ? cell_ids[گره] : nullptr; }
    SigBit sig(گره_type گره) { بازگشت گره < 0 ? sig_ids[~گره] : SigBit(); }

    template<typename Src, typename Dst>
    تهی add_edge(Src &&src, Dst &&dst) {
        computed = false;
        گره_type src_گره = گره(std::forward<Src>(src));
        گره_type dst_گره = گره(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_گره, dst_گره));
    }

    گره_enumerator enumerate_nodes() {
        اگر (!computed) compute();
        بازگشت {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(گره_type ثابت &گره) ثابت {
        auto range = edge_ranges[گره + offset];
        بازگشت {edges.begin() + range.first, edges.begin() + range.second};
    }

    صحیح &dfs_index(گره_type ثابت &گره) {
        بازگشت indices_[گره + offset];
    }
};


class IntGraph {
عمومی:
    تعریفنوع صحیح گره_type;

    ساختار successor_enumerator {
        std::vector<std::pair<صحیح, صحیح>>::const_iterator current, end;
        bool finished() ثابت { بازگشت current == end; }
        گره_type next() {
            log_assert(!finished());
            گره_type result = current->second;
            ++current;
            بازگشت result;
        }
    };

    ساختار گره_enumerator {
        صحیح current, end;
        bool finished() ثابت { بازگشت current == end; }
        گره_type next() {
            log_assert(!finished());
            گره_type result = current;
            ++current;
            بازگشت result;
        }
    };

خصوصی:
    std::vector<std::pair<صحیح, صحیح>> edges;
    std::vector<std::pair<صحیح, صحیح>> edge_ranges;
    std::vector<صحیح> indices_;
    bool computed = false;

    تهی compute() {
        edge_ranges.clear();

        صحیح گره_end = 0;
        برای (auto ثابت &edge : edges)
            گره_end = std::max(گره_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(گره_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        صحیح range_begin = 0;
        برای (صحیح گره = 0; گره != گره_end; ++گره) {
            وقتی (edge != edge_end && edge->first <= گره)
                ++edge;
            صحیح range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

عمومی:
    تهی add_edge(صحیح src, صحیح dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        edges.emplace_back(std::make_pair(src, dst));
    }

    گره_enumerator enumerate_nodes() {
        اگر (!computed) compute();
        بازگشت {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(صحیح گره) ثابت {
        auto range = edge_ranges[گره];
        بازگشت {edges.begin() + range.first, edges.begin() + range.second};
    }

    صحیح &dfs_index(گره_type ثابت &گره) {
        بازگشت indices_[گره];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    تعریفنوع typename G::گره_enumerator گره_enumerator;
    تعریفنوع typename G::successor_enumerator successor_enumerator;
    تعریفنوع typename G::گره_type گره_type;

    ساختار dfs_entry {
        گره_type گره;
        successor_enumerator successors;
        صحیح lowlink;

        dfs_entry(گره_type گره, successor_enumerator successors, صحیح lowlink) :
            گره(گره), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_پشته;
    std::vector<گره_type> component_پشته;
    صحیح next_index = 0;

عمومی:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        گره_enumerator nodes = graph.enumerate_nodes();
        وقتی (!nodes.finished()) {
            گره_type گره = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(گره);
            اگر (successors.finished())
            {
                graph.dfs_index(گره) = next_index;
                next_index++;
                component_پشته.افزودن_back(گره);
                component(component_پشته.data(), component_پشته.data() + 1);
                component_پشته.clear();
                graph.dfs_index(گره) = INT_MAX;
            }
        }
        بازگشت *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        گره_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        وقتی (!nodes.finished())
            process(nodes.next());
        بازگشت *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(گره_type گره) {
        // only start a new search if the node wasn't visited yet
        اگر (graph.dfs_index(گره) >= 0)
            بازگشت *this;
        وقتی (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            صحیح lowlink = next_index;
            next_index++;
            graph.dfs_index(گره) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_پشته.افزودن_back(گره);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(گره);
            وقتی (true) {
                اگر (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    اگر (lowlink == graph.dfs_index(گره)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        صحیح current = GetSize(component_پشته);
                        انجامبده {
                            --current;
                        } وقتی (component_پشته[current] != گره);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        گره_type *پشته_ptr = component_پشته.data();
                        گره_type *component_begin = پشته_ptr + current;
                        گره_type *component_end = پشته_ptr + component_پشته.size();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_end);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        برای (; component_begin != component_end; ++component_begin)
                            graph.dfs_index(*component_begin) = INT_MAX;
                        component_پشته.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    اگر (dfs_پشته.empty())
                        بازگشت *this;
                    auto &dfs_بالاترین = dfs_پشته.back();

                    گره = dfs_بالاترین.گره;
                    successors = std::move(dfs_بالاترین.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_بالاترین.lowlink);
                    dfs_پشته.برداشت_back();
                    // continue checking the remaining successors of the parent node.
                } وگرنه {
                    گره_type succ = successors.next();
                    اگر (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_پشته.emplace_back(گره, std::move(successors), lowlink);
                        گره = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        بشکن;
                    } وگرنه {
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
