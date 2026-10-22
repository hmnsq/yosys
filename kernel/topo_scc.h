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

#അന്തര്ഭാവ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SigCellGraph {
പൊതു:
    തരനിർവചനം പൂർണ്ണസംഖ്യ കണ്ണി_type;

    ഘടന successor_enumerator {
        std::vector<std::pair<പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ>>::const_iterator current, end;
        bool finished() സ്ഥിരം { തിരികെ current == end; }
        കണ്ണി_type next() {
            log_assert(!finished());
            കണ്ണി_type result = current->second;
            ++current;
            തിരികെ result;
        }
    };

    ഘടന കണ്ണി_enumerator {
        പൂർണ്ണസംഖ്യ current, end;
        bool finished() സ്ഥിരം { തിരികെ current == end; }
        കണ്ണി_type next() {
            log_assert(!finished());
            കണ്ണി_type result = current;
            ++current;
            തിരികെ result;
        }
    };

സ്വകാര്യ:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::vector<std::pair<പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ>> edges;
    std::vector<std::pair<പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ>> edge_ranges;
    std::vector<പൂർണ്ണസംഖ്യ> indices_;
    പൂർണ്ണസംഖ്യ offset;
    bool computed = false;

    ശൂന്യം compute() {
        offset = GetSize(sig_ids);
        edge_ranges.clear();
        indices_.clear();
        indices_.resize(GetSize(sig_ids) + GetSize(cell_ids), -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        പൂർണ്ണസംഖ്യ range_begin = 0;
        വേണ്ടി (പൂർണ്ണസംഖ്യ കണ്ണി = -offset, കണ്ണി_end = GetSize(cell_ids); കണ്ണി != കണ്ണി_end; ++കണ്ണി) {
            ആകുമ്പോൾ (edge != edge_end && edge->first <= കണ്ണി)
                ++edge;
            പൂർണ്ണസംഖ്യ range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

പൊതു:
    കണ്ണി_type കണ്ണി(RTLIL::Cell *cell) { തിരികെ cell_ids(cell); }
    കണ്ണി_type കണ്ണി(SigBit സ്ഥിരം &bit) { തിരികെ ~sig_ids(bit); }

    bool is_cell(കണ്ണി_type കണ്ണി) { തിരികെ കണ്ണി >= 0; }
    bool is_sig(കണ്ണി_type കണ്ണി) { തിരികെ കണ്ണി < 0; }

    Cell *cell(കണ്ണി_type കണ്ണി) { തിരികെ കണ്ണി >= 0 ? cell_ids[കണ്ണി] : nullptr; }
    SigBit sig(കണ്ണി_type കണ്ണി) { തിരികെ കണ്ണി < 0 ? sig_ids[~കണ്ണി] : SigBit(); }

    template<typename Src, typename Dst>
    ശൂന്യം add_edge(Src &&src, Dst &&dst) {
        computed = false;
        കണ്ണി_type src_കണ്ണി = കണ്ണി(std::forward<Src>(src));
        കണ്ണി_type dst_കണ്ണി = കണ്ണി(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_കണ്ണി, dst_കണ്ണി));
    }

    കണ്ണി_enumerator enumerate_nodes() {
        എങ്കിൽ (!computed) compute();
        തിരികെ {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(കണ്ണി_type സ്ഥിരം &കണ്ണി) സ്ഥിരം {
        auto range = edge_ranges[കണ്ണി + offset];
        തിരികെ {edges.begin() + range.first, edges.begin() + range.second};
    }

    പൂർണ്ണസംഖ്യ &dfs_index(കണ്ണി_type സ്ഥിരം &കണ്ണി) {
        തിരികെ indices_[കണ്ണി + offset];
    }
};


class IntGraph {
പൊതു:
    തരനിർവചനം പൂർണ്ണസംഖ്യ കണ്ണി_type;

    ഘടന successor_enumerator {
        std::vector<std::pair<പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ>>::const_iterator current, end;
        bool finished() സ്ഥിരം { തിരികെ current == end; }
        കണ്ണി_type next() {
            log_assert(!finished());
            കണ്ണി_type result = current->second;
            ++current;
            തിരികെ result;
        }
    };

    ഘടന കണ്ണി_enumerator {
        പൂർണ്ണസംഖ്യ current, end;
        bool finished() സ്ഥിരം { തിരികെ current == end; }
        കണ്ണി_type next() {
            log_assert(!finished());
            കണ്ണി_type result = current;
            ++current;
            തിരികെ result;
        }
    };

സ്വകാര്യ:
    std::vector<std::pair<പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ>> edges;
    std::vector<std::pair<പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ>> edge_ranges;
    std::vector<പൂർണ്ണസംഖ്യ> indices_;
    bool computed = false;

    ശൂന്യം compute() {
        edge_ranges.clear();

        പൂർണ്ണസംഖ്യ കണ്ണി_end = 0;
        വേണ്ടി (auto സ്ഥിരം &edge : edges)
            കണ്ണി_end = std::max(കണ്ണി_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(കണ്ണി_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        പൂർണ്ണസംഖ്യ range_begin = 0;
        വേണ്ടി (പൂർണ്ണസംഖ്യ കണ്ണി = 0; കണ്ണി != കണ്ണി_end; ++കണ്ണി) {
            ആകുമ്പോൾ (edge != edge_end && edge->first <= കണ്ണി)
                ++edge;
            പൂർണ്ണസംഖ്യ range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

പൊതു:
    ശൂന്യം add_edge(പൂർണ്ണസംഖ്യ src, പൂർണ്ണസംഖ്യ dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        edges.emplace_back(std::make_pair(src, dst));
    }

    കണ്ണി_enumerator enumerate_nodes() {
        എങ്കിൽ (!computed) compute();
        തിരികെ {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(പൂർണ്ണസംഖ്യ കണ്ണി) സ്ഥിരം {
        auto range = edge_ranges[കണ്ണി];
        തിരികെ {edges.begin() + range.first, edges.begin() + range.second};
    }

    പൂർണ്ണസംഖ്യ &dfs_index(കണ്ണി_type സ്ഥിരം &കണ്ണി) {
        തിരികെ indices_[കണ്ണി];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    തരനിർവചനം typename G::കണ്ണി_enumerator കണ്ണി_enumerator;
    തരനിർവചനം typename G::successor_enumerator successor_enumerator;
    തരനിർവചനം typename G::കണ്ണി_type കണ്ണി_type;

    ഘടന dfs_entry {
        കണ്ണി_type കണ്ണി;
        successor_enumerator successors;
        പൂർണ്ണസംഖ്യ lowlink;

        dfs_entry(കണ്ണി_type കണ്ണി, successor_enumerator successors, പൂർണ്ണസംഖ്യ lowlink) :
            കണ്ണി(കണ്ണി), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_അട്ടി;
    std::vector<കണ്ണി_type> component_അട്ടി;
    പൂർണ്ണസംഖ്യ next_index = 0;

പൊതു:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        കണ്ണി_enumerator nodes = graph.enumerate_nodes();
        ആകുമ്പോൾ (!nodes.finished()) {
            കണ്ണി_type കണ്ണി = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(കണ്ണി);
            എങ്കിൽ (successors.finished())
            {
                graph.dfs_index(കണ്ണി) = next_index;
                next_index++;
                component_അട്ടി.തള്ളുക_back(കണ്ണി);
                component(component_അട്ടി.data(), component_അട്ടി.data() + 1);
                component_അട്ടി.clear();
                graph.dfs_index(കണ്ണി) = INT_MAX;
            }
        }
        തിരികെ *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        കണ്ണി_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        ആകുമ്പോൾ (!nodes.finished())
            process(nodes.next());
        തിരികെ *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(കണ്ണി_type കണ്ണി) {
        // only start a new search if the node wasn't visited yet
        എങ്കിൽ (graph.dfs_index(കണ്ണി) >= 0)
            തിരികെ *this;
        ആകുമ്പോൾ (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            പൂർണ്ണസംഖ്യ lowlink = next_index;
            next_index++;
            graph.dfs_index(കണ്ണി) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_അട്ടി.തള്ളുക_back(കണ്ണി);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(കണ്ണി);
            ആകുമ്പോൾ (true) {
                എങ്കിൽ (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    എങ്കിൽ (lowlink == graph.dfs_index(കണ്ണി)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        പൂർണ്ണസംഖ്യ current = GetSize(component_അട്ടി);
                        ചെയ്യുക {
                            --current;
                        } ആകുമ്പോൾ (component_അട്ടി[current] != കണ്ണി);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        കണ്ണി_type *അട്ടി_ptr = component_അട്ടി.data();
                        കണ്ണി_type *component_begin = അട്ടി_ptr + current;
                        കണ്ണി_type *component_end = അട്ടി_ptr + component_അട്ടി.size();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_end);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        വേണ്ടി (; component_begin != component_end; ++component_begin)
                            graph.dfs_index(*component_begin) = INT_MAX;
                        component_അട്ടി.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    എങ്കിൽ (dfs_അട്ടി.empty())
                        തിരികെ *this;
                    auto &dfs_മുകൾ = dfs_അട്ടി.back();

                    കണ്ണി = dfs_മുകൾ.കണ്ണി;
                    successors = std::move(dfs_മുകൾ.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_മുകൾ.lowlink);
                    dfs_അട്ടി.എടുക്കുക_back();
                    // continue checking the remaining successors of the parent node.
                } അല്ലെങ്കിൽ {
                    കണ്ണി_type succ = successors.next();
                    എങ്കിൽ (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_അട്ടി.emplace_back(കണ്ണി, std::move(successors), lowlink);
                        കണ്ണി = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        നിർത്തുക;
                    } അല്ലെങ്കിൽ {
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
