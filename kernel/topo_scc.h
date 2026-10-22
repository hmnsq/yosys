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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SigCellGraph {
ସର୍ବସାଧାରଣ:
    ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି_type;

    ସଂରଚନା successor_enumerator {
        std::vector<std::pair<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>>::const_iterator current, end;
        bool finished() ଅଚଳ { ଫେରନ୍ତୁ current == end; }
        ଗଣ୍ଠି_type next() {
            log_assert(!finished());
            ଗଣ୍ଠି_type result = current->second;
            ++current;
            ଫେରନ୍ତୁ result;
        }
    };

    ସଂରଚନା ଗଣ୍ଠି_enumerator {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା current, end;
        bool finished() ଅଚଳ { ଫେରନ୍ତୁ current == end; }
        ଗଣ୍ଠି_type next() {
            log_assert(!finished());
            ଗଣ୍ଠି_type result = current;
            ++current;
            ଫେରନ୍ତୁ result;
        }
    };

ବ୍ୟକ୍ତିଗତ:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::vector<std::pair<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>> edges;
    std::vector<std::pair<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>> edge_ranges;
    std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> indices_;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;
    bool computed = false;

    ଶୂନ୍ୟ compute() {
        offset = GetSize(sig_ids);
        edge_ranges.clear();
        indices_.clear();
        indices_.resize(GetSize(sig_ids) + GetSize(cell_ids), -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା range_begin = 0;
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି = -offset, ଗଣ୍ଠି_end = GetSize(cell_ids); ଗଣ୍ଠି != ଗଣ୍ଠି_end; ++ଗଣ୍ଠି) {
            ଯେତେବେଳେ (edge != edge_end && edge->first <= ଗଣ୍ଠି)
                ++edge;
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

ସର୍ବସାଧାରଣ:
    ଗଣ୍ଠି_type ଗଣ୍ଠି(RTLIL::Cell *cell) { ଫେରନ୍ତୁ cell_ids(cell); }
    ଗଣ୍ଠି_type ଗଣ୍ଠି(SigBit ଅଚଳ &bit) { ଫେରନ୍ତୁ ~sig_ids(bit); }

    bool is_cell(ଗଣ୍ଠି_type ଗଣ୍ଠି) { ଫେରନ୍ତୁ ଗଣ୍ଠି >= 0; }
    bool is_sig(ଗଣ୍ଠି_type ଗଣ୍ଠି) { ଫେରନ୍ତୁ ଗଣ୍ଠି < 0; }

    Cell *cell(ଗଣ୍ଠି_type ଗଣ୍ଠି) { ଫେରନ୍ତୁ ଗଣ୍ଠି >= 0 ? cell_ids[ଗଣ୍ଠି] : nullptr; }
    SigBit sig(ଗଣ୍ଠି_type ଗଣ୍ଠି) { ଫେରନ୍ତୁ ଗଣ୍ଠି < 0 ? sig_ids[~ଗଣ୍ଠି] : SigBit(); }

    template<typename Src, typename Dst>
    ଶୂନ୍ୟ add_edge(Src &&src, Dst &&dst) {
        computed = false;
        ଗଣ୍ଠି_type src_ଗଣ୍ଠି = ଗଣ୍ଠି(std::forward<Src>(src));
        ଗଣ୍ଠି_type dst_ଗଣ୍ଠି = ଗଣ୍ଠି(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_ଗଣ୍ଠି, dst_ଗଣ୍ଠି));
    }

    ଗଣ୍ଠି_enumerator enumerate_nodes() {
        ଯଦି (!computed) compute();
        ଫେରନ୍ତୁ {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(ଗଣ୍ଠି_type ଅଚଳ &ଗଣ୍ଠି) ଅଚଳ {
        auto range = edge_ranges[ଗଣ୍ଠି + offset];
        ଫେରନ୍ତୁ {edges.begin() + range.first, edges.begin() + range.second};
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା &dfs_index(ଗଣ୍ଠି_type ଅଚଳ &ଗଣ୍ଠି) {
        ଫେରନ୍ତୁ indices_[ଗଣ୍ଠି + offset];
    }
};


class IntGraph {
ସର୍ବସାଧାରଣ:
    ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି_type;

    ସଂରଚନା successor_enumerator {
        std::vector<std::pair<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>>::const_iterator current, end;
        bool finished() ଅଚଳ { ଫେରନ୍ତୁ current == end; }
        ଗଣ୍ଠି_type next() {
            log_assert(!finished());
            ଗଣ୍ଠି_type result = current->second;
            ++current;
            ଫେରନ୍ତୁ result;
        }
    };

    ସଂରଚନା ଗଣ୍ଠି_enumerator {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା current, end;
        bool finished() ଅଚଳ { ଫେରନ୍ତୁ current == end; }
        ଗଣ୍ଠି_type next() {
            log_assert(!finished());
            ଗଣ୍ଠି_type result = current;
            ++current;
            ଫେରନ୍ତୁ result;
        }
    };

ବ୍ୟକ୍ତିଗତ:
    std::vector<std::pair<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>> edges;
    std::vector<std::pair<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>> edge_ranges;
    std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> indices_;
    bool computed = false;

    ଶୂନ୍ୟ compute() {
        edge_ranges.clear();

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି_end = 0;
        ପାଇଁ (auto ଅଚଳ &edge : edges)
            ଗଣ୍ଠି_end = std::max(ଗଣ୍ଠି_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(ଗଣ୍ଠି_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା range_begin = 0;
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି = 0; ଗଣ୍ଠି != ଗଣ୍ଠି_end; ++ଗଣ୍ଠି) {
            ଯେତେବେଳେ (edge != edge_end && edge->first <= ଗଣ୍ଠି)
                ++edge;
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

ସର୍ବସାଧାରଣ:
    ଶୂନ୍ୟ add_edge(ପୂର୍ଣ୍ଣସଂଖ୍ୟା src, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        edges.emplace_back(std::make_pair(src, dst));
    }

    ଗଣ୍ଠି_enumerator enumerate_nodes() {
        ଯଦି (!computed) compute();
        ଫେରନ୍ତୁ {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି) ଅଚଳ {
        auto range = edge_ranges[ଗଣ୍ଠି];
        ଫେରନ୍ତୁ {edges.begin() + range.first, edges.begin() + range.second};
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା &dfs_index(ଗଣ୍ଠି_type ଅଚଳ &ଗଣ୍ଠି) {
        ଫେରନ୍ତୁ indices_[ଗଣ୍ଠି];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    ପ୍ରକାରବ୍ୟାଖ୍ୟା typename G::ଗଣ୍ଠି_enumerator ଗଣ୍ଠି_enumerator;
    ପ୍ରକାରବ୍ୟାଖ୍ୟା typename G::successor_enumerator successor_enumerator;
    ପ୍ରକାରବ୍ୟାଖ୍ୟା typename G::ଗଣ୍ଠି_type ଗଣ୍ଠି_type;

    ସଂରଚନା dfs_entry {
        ଗଣ୍ଠି_type ଗଣ୍ଠି;
        successor_enumerator successors;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା lowlink;

        dfs_entry(ଗଣ୍ଠି_type ଗଣ୍ଠି, successor_enumerator successors, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lowlink) :
            ଗଣ୍ଠି(ଗଣ୍ଠି), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_ଗଦା;
    std::vector<ଗଣ୍ଠି_type> component_ଗଦା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_index = 0;

ସର୍ବସାଧାରଣ:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        ଗଣ୍ଠି_enumerator nodes = graph.enumerate_nodes();
        ଯେତେବେଳେ (!nodes.finished()) {
            ଗଣ୍ଠି_type ଗଣ୍ଠି = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(ଗଣ୍ଠି);
            ଯଦି (successors.finished())
            {
                graph.dfs_index(ଗଣ୍ଠି) = next_index;
                next_index++;
                component_ଗଦା.ଠେଲ_back(ଗଣ୍ଠି);
                component(component_ଗଦା.data(), component_ଗଦା.data() + 1);
                component_ଗଦା.clear();
                graph.dfs_index(ଗଣ୍ଠି) = INT_MAX;
            }
        }
        ଫେରନ୍ତୁ *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        ଗଣ୍ଠି_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        ଯେତେବେଳେ (!nodes.finished())
            process(nodes.next());
        ଫେରନ୍ତୁ *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(ଗଣ୍ଠି_type ଗଣ୍ଠି) {
        // only start a new search if the node wasn't visited yet
        ଯଦି (graph.dfs_index(ଗଣ୍ଠି) >= 0)
            ଫେରନ୍ତୁ *this;
        ଯେତେବେଳେ (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା lowlink = next_index;
            next_index++;
            graph.dfs_index(ଗଣ୍ଠି) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_ଗଦା.ଠେଲ_back(ଗଣ୍ଠି);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(ଗଣ୍ଠି);
            ଯେତେବେଳେ (true) {
                ଯଦି (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    ଯଦି (lowlink == graph.dfs_index(ଗଣ୍ଠି)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା current = GetSize(component_ଗଦା);
                        କର {
                            --current;
                        } ଯେତେବେଳେ (component_ଗଦା[current] != ଗଣ୍ଠି);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        ଗଣ୍ଠି_type *ଗଦା_ptr = component_ଗଦା.data();
                        ଗଣ୍ଠି_type *component_begin = ଗଦା_ptr + current;
                        ଗଣ୍ଠି_type *component_end = ଗଦା_ptr + component_ଗଦା.size();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_end);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        ପାଇଁ (; component_begin != component_end; ++component_begin)
                            graph.dfs_index(*component_begin) = INT_MAX;
                        component_ଗଦା.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    ଯଦି (dfs_ଗଦା.empty())
                        ଫେରନ୍ତୁ *this;
                    auto &dfs_ଶୀର୍ଷ = dfs_ଗଦା.back();

                    ଗଣ୍ଠି = dfs_ଶୀର୍ଷ.ଗଣ୍ଠି;
                    successors = std::move(dfs_ଶୀର୍ଷ.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_ଶୀର୍ଷ.lowlink);
                    dfs_ଗଦା.କାଢ଼_back();
                    // continue checking the remaining successors of the parent node.
                } ଅନ୍ୟଥା {
                    ଗଣ୍ଠି_type succ = successors.next();
                    ଯଦି (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_ଗଦା.emplace_back(ଗଣ୍ଠି, std::move(successors), lowlink);
                        ଗଣ୍ଠି = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        ଭାଙ୍ଗ;
                    } ଅନ୍ୟଥା {
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
