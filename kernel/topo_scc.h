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

#அந்தர்பாவ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SigCellGraph {
பொது:
    வகைவரை முழு முனை_type;

    கட்டமைப்பு successor_enumerator {
        std::vector<std::pair<முழு, முழு>>::const_iterator current, end;
        bool finished() மாறிலி { திரும்பு current == end; }
        முனை_type next() {
            log_assert(!finished());
            முனை_type result = current->second;
            ++current;
            திரும்பு result;
        }
    };

    கட்டமைப்பு முனை_enumerator {
        முழு current, end;
        bool finished() மாறிலி { திரும்பு current == end; }
        முனை_type next() {
            log_assert(!finished());
            முனை_type result = current;
            ++current;
            திரும்பு result;
        }
    };

தனிப்பட்ட:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::vector<std::pair<முழு, முழு>> edges;
    std::vector<std::pair<முழு, முழு>> edge_ranges;
    std::vector<முழு> indices_;
    முழு offset;
    bool computed = false;

    வெறுமை compute() {
        offset = GetSize(sig_ids);
        edge_ranges.clear();
        indices_.clear();
        indices_.resize(GetSize(sig_ids) + GetSize(cell_ids), -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        முழு range_begin = 0;
        க்கு (முழு முனை = -offset, முனை_end = GetSize(cell_ids); முனை != முனை_end; ++முனை) {
            போது (edge != edge_end && edge->first <= முனை)
                ++edge;
            முழு range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

பொது:
    முனை_type முனை(RTLIL::Cell *cell) { திரும்பு cell_ids(cell); }
    முனை_type முனை(SigBit மாறிலி &bit) { திரும்பு ~sig_ids(bit); }

    bool is_cell(முனை_type முனை) { திரும்பு முனை >= 0; }
    bool is_sig(முனை_type முனை) { திரும்பு முனை < 0; }

    Cell *cell(முனை_type முனை) { திரும்பு முனை >= 0 ? cell_ids[முனை] : nullptr; }
    SigBit sig(முனை_type முனை) { திரும்பு முனை < 0 ? sig_ids[~முனை] : SigBit(); }

    template<typename Src, typename Dst>
    வெறுமை add_edge(Src &&src, Dst &&dst) {
        computed = false;
        முனை_type src_முனை = முனை(std::forward<Src>(src));
        முனை_type dst_முனை = முனை(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_முனை, dst_முனை));
    }

    முனை_enumerator enumerate_nodes() {
        எனில் (!computed) compute();
        திரும்பு {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(முனை_type மாறிலி &முனை) மாறிலி {
        auto range = edge_ranges[முனை + offset];
        திரும்பு {edges.begin() + range.first, edges.begin() + range.second};
    }

    முழு &dfs_index(முனை_type மாறிலி &முனை) {
        திரும்பு indices_[முனை + offset];
    }
};


class IntGraph {
பொது:
    வகைவரை முழு முனை_type;

    கட்டமைப்பு successor_enumerator {
        std::vector<std::pair<முழு, முழு>>::const_iterator current, end;
        bool finished() மாறிலி { திரும்பு current == end; }
        முனை_type next() {
            log_assert(!finished());
            முனை_type result = current->second;
            ++current;
            திரும்பு result;
        }
    };

    கட்டமைப்பு முனை_enumerator {
        முழு current, end;
        bool finished() மாறிலி { திரும்பு current == end; }
        முனை_type next() {
            log_assert(!finished());
            முனை_type result = current;
            ++current;
            திரும்பு result;
        }
    };

தனிப்பட்ட:
    std::vector<std::pair<முழு, முழு>> edges;
    std::vector<std::pair<முழு, முழு>> edge_ranges;
    std::vector<முழு> indices_;
    bool computed = false;

    வெறுமை compute() {
        edge_ranges.clear();

        முழு முனை_end = 0;
        க்கு (auto மாறிலி &edge : edges)
            முனை_end = std::max(முனை_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(முனை_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        முழு range_begin = 0;
        க்கு (முழு முனை = 0; முனை != முனை_end; ++முனை) {
            போது (edge != edge_end && edge->first <= முனை)
                ++edge;
            முழு range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

பொது:
    வெறுமை add_edge(முழு src, முழு dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        edges.emplace_back(std::make_pair(src, dst));
    }

    முனை_enumerator enumerate_nodes() {
        எனில் (!computed) compute();
        திரும்பு {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(முழு முனை) மாறிலி {
        auto range = edge_ranges[முனை];
        திரும்பு {edges.begin() + range.first, edges.begin() + range.second};
    }

    முழு &dfs_index(முனை_type மாறிலி &முனை) {
        திரும்பு indices_[முனை];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    வகைவரை typename G::முனை_enumerator முனை_enumerator;
    வகைவரை typename G::successor_enumerator successor_enumerator;
    வகைவரை typename G::முனை_type முனை_type;

    கட்டமைப்பு dfs_entry {
        முனை_type முனை;
        successor_enumerator successors;
        முழு lowlink;

        dfs_entry(முனை_type முனை, successor_enumerator successors, முழு lowlink) :
            முனை(முனை), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_அடுக்கு;
    std::vector<முனை_type> component_அடுக்கு;
    முழு next_index = 0;

பொது:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        முனை_enumerator nodes = graph.enumerate_nodes();
        போது (!nodes.finished()) {
            முனை_type முனை = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(முனை);
            எனில் (successors.finished())
            {
                graph.dfs_index(முனை) = next_index;
                next_index++;
                component_அடுக்கு.தள்ளு_back(முனை);
                component(component_அடுக்கு.data(), component_அடுக்கு.data() + 1);
                component_அடுக்கு.clear();
                graph.dfs_index(முனை) = INT_MAX;
            }
        }
        திரும்பு *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        முனை_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        போது (!nodes.finished())
            process(nodes.next());
        திரும்பு *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(முனை_type முனை) {
        // only start a new search if the node wasn't visited yet
        எனில் (graph.dfs_index(முனை) >= 0)
            திரும்பு *this;
        போது (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            முழு lowlink = next_index;
            next_index++;
            graph.dfs_index(முனை) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_அடுக்கு.தள்ளு_back(முனை);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(முனை);
            போது (true) {
                எனில் (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    எனில் (lowlink == graph.dfs_index(முனை)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        முழு current = GetSize(component_அடுக்கு);
                        செய் {
                            --current;
                        } போது (component_அடுக்கு[current] != முனை);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        முனை_type *அடுக்கு_ptr = component_அடுக்கு.data();
                        முனை_type *component_begin = அடுக்கு_ptr + current;
                        முனை_type *component_end = அடுக்கு_ptr + component_அடுக்கு.size();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_end);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        க்கு (; component_begin != component_end; ++component_begin)
                            graph.dfs_index(*component_begin) = INT_MAX;
                        component_அடுக்கு.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    எனில் (dfs_அடுக்கு.empty())
                        திரும்பு *this;
                    auto &dfs_உச்சி = dfs_அடுக்கு.back();

                    முனை = dfs_உச்சி.முனை;
                    successors = std::move(dfs_உச்சி.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_உச்சி.lowlink);
                    dfs_அடுக்கு.எடு_back();
                    // continue checking the remaining successors of the parent node.
                } இல்லையெனில் {
                    முனை_type succ = successors.next();
                    எனில் (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_அடுக்கு.emplace_back(முனை, std::move(successors), lowlink);
                        முனை = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        நிறுத்து;
                    } இல்லையெனில் {
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
