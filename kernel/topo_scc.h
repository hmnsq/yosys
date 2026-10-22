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

#અન્તર્ભાવ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SigCellGraph {
જાહેર:
    પ્રકારવ્યાખ્યા પૂર્ણાંક ગાંઠ_type;

    સંરચના successor_enumerator {
        std::vector<std::pair<પૂર્ણાંક, પૂર્ણાંક>>::const_iterator current, end;
        bool finished() અચલ { પાછા current == end; }
        ગાંઠ_type next() {
            log_assert(!finished());
            ગાંઠ_type result = current->second;
            ++current;
            પાછા result;
        }
    };

    સંરચના ગાંઠ_enumerator {
        પૂર્ણાંક current, end;
        bool finished() અચલ { પાછા current == end; }
        ગાંઠ_type next() {
            log_assert(!finished());
            ગાંઠ_type result = current;
            ++current;
            પાછા result;
        }
    };

ખાનગી:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::vector<std::pair<પૂર્ણાંક, પૂર્ણાંક>> edges;
    std::vector<std::pair<પૂર્ણાંક, પૂર્ણાંક>> edge_ranges;
    std::vector<પૂર્ણાંક> indices_;
    પૂર્ણાંક offset;
    bool computed = false;

    શૂન્ય compute() {
        offset = GetSize(sig_ids);
        edge_ranges.clear();
        indices_.clear();
        indices_.resize(GetSize(sig_ids) + GetSize(cell_ids), -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        પૂર્ણાંક range_begin = 0;
        માટે (પૂર્ણાંક ગાંઠ = -offset, ગાંઠ_end = GetSize(cell_ids); ગાંઠ != ગાંઠ_end; ++ગાંઠ) {
            જ્યારે (edge != edge_end && edge->first <= ગાંઠ)
                ++edge;
            પૂર્ણાંક range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

જાહેર:
    ગાંઠ_type ગાંઠ(RTLIL::Cell *cell) { પાછા cell_ids(cell); }
    ગાંઠ_type ગાંઠ(SigBit અચલ &bit) { પાછા ~sig_ids(bit); }

    bool is_cell(ગાંઠ_type ગાંઠ) { પાછા ગાંઠ >= 0; }
    bool is_sig(ગાંઠ_type ગાંઠ) { પાછા ગાંઠ < 0; }

    Cell *cell(ગાંઠ_type ગાંઠ) { પાછા ગાંઠ >= 0 ? cell_ids[ગાંઠ] : nullptr; }
    SigBit sig(ગાંઠ_type ગાંઠ) { પાછા ગાંઠ < 0 ? sig_ids[~ગાંઠ] : SigBit(); }

    template<typename Src, typename Dst>
    શૂન્ય add_edge(Src &&src, Dst &&dst) {
        computed = false;
        ગાંઠ_type src_ગાંઠ = ગાંઠ(std::forward<Src>(src));
        ગાંઠ_type dst_ગાંઠ = ગાંઠ(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_ગાંઠ, dst_ગાંઠ));
    }

    ગાંઠ_enumerator enumerate_nodes() {
        જો (!computed) compute();
        પાછા {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(ગાંઠ_type અચલ &ગાંઠ) અચલ {
        auto range = edge_ranges[ગાંઠ + offset];
        પાછા {edges.begin() + range.first, edges.begin() + range.second};
    }

    પૂર્ણાંક &dfs_index(ગાંઠ_type અચલ &ગાંઠ) {
        પાછા indices_[ગાંઠ + offset];
    }
};


class IntGraph {
જાહેર:
    પ્રકારવ્યાખ્યા પૂર્ણાંક ગાંઠ_type;

    સંરચના successor_enumerator {
        std::vector<std::pair<પૂર્ણાંક, પૂર્ણાંક>>::const_iterator current, end;
        bool finished() અચલ { પાછા current == end; }
        ગાંઠ_type next() {
            log_assert(!finished());
            ગાંઠ_type result = current->second;
            ++current;
            પાછા result;
        }
    };

    સંરચના ગાંઠ_enumerator {
        પૂર્ણાંક current, end;
        bool finished() અચલ { પાછા current == end; }
        ગાંઠ_type next() {
            log_assert(!finished());
            ગાંઠ_type result = current;
            ++current;
            પાછા result;
        }
    };

ખાનગી:
    std::vector<std::pair<પૂર્ણાંક, પૂર્ણાંક>> edges;
    std::vector<std::pair<પૂર્ણાંક, પૂર્ણાંક>> edge_ranges;
    std::vector<પૂર્ણાંક> indices_;
    bool computed = false;

    શૂન્ય compute() {
        edge_ranges.clear();

        પૂર્ણાંક ગાંઠ_end = 0;
        માટે (auto અચલ &edge : edges)
            ગાંઠ_end = std::max(ગાંઠ_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(ગાંઠ_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        પૂર્ણાંક range_begin = 0;
        માટે (પૂર્ણાંક ગાંઠ = 0; ગાંઠ != ગાંઠ_end; ++ગાંઠ) {
            જ્યારે (edge != edge_end && edge->first <= ગાંઠ)
                ++edge;
            પૂર્ણાંક range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

જાહેર:
    શૂન્ય add_edge(પૂર્ણાંક src, પૂર્ણાંક dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        edges.emplace_back(std::make_pair(src, dst));
    }

    ગાંઠ_enumerator enumerate_nodes() {
        જો (!computed) compute();
        પાછા {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(પૂર્ણાંક ગાંઠ) અચલ {
        auto range = edge_ranges[ગાંઠ];
        પાછા {edges.begin() + range.first, edges.begin() + range.second};
    }

    પૂર્ણાંક &dfs_index(ગાંઠ_type અચલ &ગાંઠ) {
        પાછા indices_[ગાંઠ];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    પ્રકારવ્યાખ્યા typename G::ગાંઠ_enumerator ગાંઠ_enumerator;
    પ્રકારવ્યાખ્યા typename G::successor_enumerator successor_enumerator;
    પ્રકારવ્યાખ્યા typename G::ગાંઠ_type ગાંઠ_type;

    સંરચના dfs_entry {
        ગાંઠ_type ગાંઠ;
        successor_enumerator successors;
        પૂર્ણાંક lowlink;

        dfs_entry(ગાંઠ_type ગાંઠ, successor_enumerator successors, પૂર્ણાંક lowlink) :
            ગાંઠ(ગાંઠ), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_ઢગલો;
    std::vector<ગાંઠ_type> component_ઢગલો;
    પૂર્ણાંક next_index = 0;

જાહેર:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        ગાંઠ_enumerator nodes = graph.enumerate_nodes();
        જ્યારે (!nodes.finished()) {
            ગાંઠ_type ગાંઠ = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(ગાંઠ);
            જો (successors.finished())
            {
                graph.dfs_index(ગાંઠ) = next_index;
                next_index++;
                component_ઢગલો.ધકેલો_back(ગાંઠ);
                component(component_ઢગલો.data(), component_ઢગલો.data() + 1);
                component_ઢગલો.clear();
                graph.dfs_index(ગાંઠ) = INT_MAX;
            }
        }
        પાછા *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        ગાંઠ_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        જ્યારે (!nodes.finished())
            process(nodes.next());
        પાછા *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(ગાંઠ_type ગાંઠ) {
        // only start a new search if the node wasn't visited yet
        જો (graph.dfs_index(ગાંઠ) >= 0)
            પાછા *this;
        જ્યારે (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            પૂર્ણાંક lowlink = next_index;
            next_index++;
            graph.dfs_index(ગાંઠ) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_ઢગલો.ધકેલો_back(ગાંઠ);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(ગાંઠ);
            જ્યારે (true) {
                જો (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    જો (lowlink == graph.dfs_index(ગાંઠ)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        પૂર્ણાંક current = GetSize(component_ઢગલો);
                        કરો {
                            --current;
                        } જ્યારે (component_ઢગલો[current] != ગાંઠ);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        ગાંઠ_type *ઢગલો_ptr = component_ઢગલો.data();
                        ગાંઠ_type *component_begin = ઢગલો_ptr + current;
                        ગાંઠ_type *component_end = ઢગલો_ptr + component_ઢગલો.size();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_end);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        માટે (; component_begin != component_end; ++component_begin)
                            graph.dfs_index(*component_begin) = INT_MAX;
                        component_ઢગલો.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    જો (dfs_ઢગલો.empty())
                        પાછા *this;
                    auto &dfs_ટોચ = dfs_ઢગલો.back();

                    ગાંઠ = dfs_ટોચ.ગાંઠ;
                    successors = std::move(dfs_ટોચ.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_ટોચ.lowlink);
                    dfs_ઢગલો.કાઢો_back();
                    // continue checking the remaining successors of the parent node.
                } નહીંતર {
                    ગાંઠ_type succ = successors.next();
                    જો (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_ઢગલો.emplace_back(ગાંઠ, std::move(successors), lowlink);
                        ગાંઠ = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        તોડો;
                    } નહીંતર {
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
