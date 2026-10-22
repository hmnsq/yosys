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

#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SigCellGraph {
सार्वजनिक:
    प्रकारनिर्देश पूर्णाङ्क ग्रन्थिः_type;

    संरचना successor_enumerator {
        std::vector<std::pair<पूर्णाङ्क, पूर्णाङ्क>>::const_iterator current, end;
        bool finished() ध्रुव { प्रत्यागम current == end; }
        ग्रन्थिः_type next() {
            log_assert(!finished());
            ग्रन्थिः_type result = current->second;
            ++current;
            प्रत्यागम result;
        }
    };

    संरचना ग्रन्थिः_enumerator {
        पूर्णाङ्क current, end;
        bool finished() ध्रुव { प्रत्यागम current == end; }
        ग्रन्थिः_type next() {
            log_assert(!finished());
            ग्रन्थिः_type result = current;
            ++current;
            प्रत्यागम result;
        }
    };

गुप्त:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::vector<std::pair<पूर्णाङ्क, पूर्णाङ्क>> edges;
    std::vector<std::pair<पूर्णाङ्क, पूर्णाङ्क>> edge_ranges;
    std::vector<पूर्णाङ्क> indices_;
    पूर्णाङ्क offset;
    bool computed = false;

    शून्य compute() {
        offset = GetSize(sig_ids);
        edge_ranges.clear();
        indices_.clear();
        indices_.resize(GetSize(sig_ids) + GetSize(cell_ids), -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        पूर्णाङ्क range_begin = 0;
        प्रति (पूर्णाङ्क ग्रन्थिः = -offset, ग्रन्थिः_end = GetSize(cell_ids); ग्रन्थिः != ग्रन्थिः_end; ++ग्रन्थिः) {
            जाव (edge != edge_end && edge->first <= ग्रन्थिः)
                ++edge;
            पूर्णाङ्क range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

सार्वजनिक:
    ग्रन्थिः_type ग्रन्थिः(RTLIL::Cell *cell) { प्रत्यागम cell_ids(cell); }
    ग्रन्थिः_type ग्रन्थिः(SigBit ध्रुव &bit) { प्रत्यागम ~sig_ids(bit); }

    bool is_cell(ग्रन्थिः_type ग्रन्थिः) { प्रत्यागम ग्रन्थिः >= 0; }
    bool is_sig(ग्रन्थिः_type ग्रन्थिः) { प्रत्यागम ग्रन्थिः < 0; }

    Cell *cell(ग्रन्थिः_type ग्रन्थिः) { प्रत्यागम ग्रन्थिः >= 0 ? cell_ids[ग्रन्थिः] : nullptr; }
    SigBit sig(ग्रन्थिः_type ग्रन्थिः) { प्रत्यागम ग्रन्थिः < 0 ? sig_ids[~ग्रन्थिः] : SigBit(); }

    template<typename Src, typename Dst>
    शून्य add_edge(Src &&src, Dst &&dst) {
        computed = false;
        ग्रन्थिः_type src_ग्रन्थिः = ग्रन्थिः(std::forward<Src>(src));
        ग्रन्थिः_type dst_ग्रन्थिः = ग्रन्थिः(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_ग्रन्थिः, dst_ग्रन्थिः));
    }

    ग्रन्थिः_enumerator enumerate_nodes() {
        जइ (!computed) compute();
        प्रत्यागम {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(ग्रन्थिः_type ध्रुव &ग्रन्थिः) ध्रुव {
        auto range = edge_ranges[ग्रन्थिः + offset];
        प्रत्यागम {edges.begin() + range.first, edges.begin() + range.second};
    }

    पूर्णाङ्क &dfs_index(ग्रन्थिः_type ध्रुव &ग्रन्थिः) {
        प्रत्यागम indices_[ग्रन्थिः + offset];
    }
};


class IntGraph {
सार्वजनिक:
    प्रकारनिर्देश पूर्णाङ्क ग्रन्थिः_type;

    संरचना successor_enumerator {
        std::vector<std::pair<पूर्णाङ्क, पूर्णाङ्क>>::const_iterator current, end;
        bool finished() ध्रुव { प्रत्यागम current == end; }
        ग्रन्थिः_type next() {
            log_assert(!finished());
            ग्रन्थिः_type result = current->second;
            ++current;
            प्रत्यागम result;
        }
    };

    संरचना ग्रन्थिः_enumerator {
        पूर्णाङ्क current, end;
        bool finished() ध्रुव { प्रत्यागम current == end; }
        ग्रन्थिः_type next() {
            log_assert(!finished());
            ग्रन्थिः_type result = current;
            ++current;
            प्रत्यागम result;
        }
    };

गुप्त:
    std::vector<std::pair<पूर्णाङ्क, पूर्णाङ्क>> edges;
    std::vector<std::pair<पूर्णाङ्क, पूर्णाङ्क>> edge_ranges;
    std::vector<पूर्णाङ्क> indices_;
    bool computed = false;

    शून्य compute() {
        edge_ranges.clear();

        पूर्णाङ्क ग्रन्थिः_end = 0;
        प्रति (auto ध्रुव &edge : edges)
            ग्रन्थिः_end = std::max(ग्रन्थिः_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(ग्रन्थिः_end, -1);

        std::sort(edges.begin(), edges.end());
        auto last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        auto edge = edges.begin();
        auto edge_end = edges.end();
        पूर्णाङ्क range_begin = 0;
        प्रति (पूर्णाङ्क ग्रन्थिः = 0; ग्रन्थिः != ग्रन्थिः_end; ++ग्रन्थिः) {
            जाव (edge != edge_end && edge->first <= ग्रन्थिः)
                ++edge;
            पूर्णाङ्क range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

सार्वजनिक:
    शून्य add_edge(पूर्णाङ्क src, पूर्णाङ्क dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        edges.emplace_back(std::make_pair(src, dst));
    }

    ग्रन्थिः_enumerator enumerate_nodes() {
        जइ (!computed) compute();
        प्रत्यागम {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(पूर्णाङ्क ग्रन्थिः) ध्रुव {
        auto range = edge_ranges[ग्रन्थिः];
        प्रत्यागम {edges.begin() + range.first, edges.begin() + range.second};
    }

    पूर्णाङ्क &dfs_index(ग्रन्थिः_type ध्रुव &ग्रन्थिः) {
        प्रत्यागम indices_[ग्रन्थिः];
    }
};

template<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    प्रकारनिर्देश typename G::ग्रन्थिः_enumerator ग्रन्थिः_enumerator;
    प्रकारनिर्देश typename G::successor_enumerator successor_enumerator;
    प्रकारनिर्देश typename G::ग्रन्थिः_type ग्रन्थिः_type;

    संरचना dfs_entry {
        ग्रन्थिः_type ग्रन्थिः;
        successor_enumerator successors;
        पूर्णाङ्क lowlink;

        dfs_entry(ग्रन्थिः_type ग्रन्थिः, successor_enumerator successors, पूर्णाङ्क lowlink) :
            ग्रन्थिः(ग्रन्थिः), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_राशिः;
    std::vector<ग्रन्थिः_type> component_राशिः;
    पूर्णाङ्क next_index = 0;

सार्वजनिक:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        ग्रन्थिः_enumerator nodes = graph.enumerate_nodes();
        जाव (!nodes.finished()) {
            ग्रन्थिः_type ग्रन्थिः = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(ग्रन्थिः);
            जइ (successors.finished())
            {
                graph.dfs_index(ग्रन्थिः) = next_index;
                next_index++;
                component_राशिः.प्रक्षिप_back(ग्रन्थिः);
                component(component_राशिः.data(), component_राशिः.data() + 1);
                component_राशिः.clear();
                graph.dfs_index(ग्रन्थिः) = INT_MAX;
            }
        }
        प्रत्यागम *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        ग्रन्थिः_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        जाव (!nodes.finished())
            process(nodes.next());
        प्रत्यागम *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(ग्रन्थिः_type ग्रन्थिः) {
        // only start a new search if the node wasn't visited yet
        जइ (graph.dfs_index(ग्रन्थिः) >= 0)
            प्रत्यागम *this;
        जाव (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            पूर्णाङ्क lowlink = next_index;
            next_index++;
            graph.dfs_index(ग्रन्थिः) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_राशिः.प्रक्षिप_back(ग्रन्थिः);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(ग्रन्थिः);
            जाव (true) {
                जइ (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    जइ (lowlink == graph.dfs_index(ग्रन्थिः)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        पूर्णाङ्क current = GetSize(component_राशिः);
                        करेहि {
                            --current;
                        } जाव (component_राशिः[current] != ग्रन्थिः);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        ग्रन्थिः_type *राशिः_ptr = component_राशिः.data();
                        ग्रन्थिः_type *component_begin = राशिः_ptr + current;
                        ग्रन्थिः_type *component_end = राशिः_ptr + component_राशिः.size();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_end);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        प्रति (; component_begin != component_end; ++component_begin)
                            graph.dfs_index(*component_begin) = INT_MAX;
                        component_राशिः.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    जइ (dfs_राशिः.empty())
                        प्रत्यागम *this;
                    auto &dfs_शिखरम् = dfs_राशिः.back();

                    ग्रन्थिः = dfs_शिखरम्.ग्रन्थिः;
                    successors = std::move(dfs_शिखरम्.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_शिखरम्.lowlink);
                    dfs_राशिः.निष्कासय_back();
                    // continue checking the remaining successors of the parent node.
                } अण्णहा {
                    ग्रन्थिः_type succ = successors.next();
                    जइ (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_राशिः.emplace_back(ग्रन्थिः, std::move(successors), lowlink);
                        ग्रन्थिः = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        भङ्ग;
                    } अण्णहा {
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
