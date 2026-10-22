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
    प्रकारनिर्देश पूर्णाङ्क गाँठ_type;

    संरचना successor_enumerator {
        std::vector<std::pair<पूर्णाङ्क, पूर्णाङ्क>>::const_iterator current, end;
        bool finished() ध्रुव { प्रत्यागम current == end; }
        गाँठ_type next() {
            log_assert(!finished());
            गाँठ_type result = current->second;
            ++current;
            प्रत्यागम result;
        }
    };

    संरचना गाँठ_enumerator {
        पूर्णाङ्क current, end;
        bool finished() ध्रुव { प्रत्यागम current == end; }
        गाँठ_type next() {
            log_assert(!finished());
            गाँठ_type result = current;
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
        स्वतः last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        स्वतः edge = edges.begin();
        स्वतः edge_end = edges.end();
        पूर्णाङ्क range_begin = 0;
        प्रति (पूर्णाङ्क गाँठ = -offset, गाँठ_end = GetSize(cell_ids); गाँठ != गाँठ_end; ++गाँठ) {
            यावत् (edge != edge_end && edge->first <= गाँठ)
                ++edge;
            पूर्णाङ्क range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

सार्वजनिक:
    गाँठ_type गाँठ(RTLIL::Cell *cell) { प्रत्यागम cell_ids(cell); }
    गाँठ_type गाँठ(SigBit ध्रुव &bit) { प्रत्यागम ~sig_ids(bit); }

    bool is_cell(गाँठ_type गाँठ) { प्रत्यागम गाँठ >= 0; }
    bool is_sig(गाँठ_type गाँठ) { प्रत्यागम गाँठ < 0; }

    Cell *cell(गाँठ_type गाँठ) { प्रत्यागम गाँठ >= 0 ? cell_ids[गाँठ] : nullptr; }
    SigBit sig(गाँठ_type गाँठ) { प्रत्यागम गाँठ < 0 ? sig_ids[~गाँठ] : SigBit(); }

    ढाँचा<typename Src, typename Dst>
    शून्य add_edge(Src &&src, Dst &&dst) {
        computed = false;
        गाँठ_type src_गाँठ = गाँठ(std::forward<Src>(src));
        गाँठ_type dst_गाँठ = गाँठ(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_गाँठ, dst_गाँठ));
    }

    गाँठ_enumerator enumerate_nodes() {
        यदि (!computed) compute();
        प्रत्यागम {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(गाँठ_type ध्रुव &गाँठ) ध्रुव {
        स्वतः range = edge_ranges[गाँठ + offset];
        प्रत्यागम {edges.begin() + range.first, edges.begin() + range.second};
    }

    पूर्णाङ्क &dfs_index(गाँठ_type ध्रुव &गाँठ) {
        प्रत्यागम indices_[गाँठ + offset];
    }
};


class IntGraph {
सार्वजनिक:
    प्रकारनिर्देश पूर्णाङ्क गाँठ_type;

    संरचना successor_enumerator {
        std::vector<std::pair<पूर्णाङ्क, पूर्णाङ्क>>::const_iterator current, end;
        bool finished() ध्रुव { प्रत्यागम current == end; }
        गाँठ_type next() {
            log_assert(!finished());
            गाँठ_type result = current->second;
            ++current;
            प्रत्यागम result;
        }
    };

    संरचना गाँठ_enumerator {
        पूर्णाङ्क current, end;
        bool finished() ध्रुव { प्रत्यागम current == end; }
        गाँठ_type next() {
            log_assert(!finished());
            गाँठ_type result = current;
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

        पूर्णाङ्क गाँठ_end = 0;
        प्रति (स्वतः ध्रुव &edge : edges)
            गाँठ_end = std::max(गाँठ_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(गाँठ_end, -1);

        std::sort(edges.begin(), edges.end());
        स्वतः last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        स्वतः edge = edges.begin();
        स्वतः edge_end = edges.end();
        पूर्णाङ्क range_begin = 0;
        प्रति (पूर्णाङ्क गाँठ = 0; गाँठ != गाँठ_end; ++गाँठ) {
            यावत् (edge != edge_end && edge->first <= गाँठ)
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

    गाँठ_enumerator enumerate_nodes() {
        यदि (!computed) compute();
        प्रत्यागम {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(पूर्णाङ्क गाँठ) ध्रुव {
        स्वतः range = edge_ranges[गाँठ];
        प्रत्यागम {edges.begin() + range.first, edges.begin() + range.second};
    }

    पूर्णाङ्क &dfs_index(गाँठ_type ध्रुव &गाँठ) {
        प्रत्यागम indices_[गाँठ];
    }
};

ढाँचा<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    प्रकारनिर्देश typename G::गाँठ_enumerator गाँठ_enumerator;
    प्रकारनिर्देश typename G::successor_enumerator successor_enumerator;
    प्रकारनिर्देश typename G::गाँठ_type गाँठ_type;

    संरचना dfs_entry {
        गाँठ_type गाँठ;
        successor_enumerator successors;
        पूर्णाङ्क lowlink;

        dfs_entry(गाँठ_type गाँठ, successor_enumerator successors, पूर्णाङ्क lowlink) :
            गाँठ(गाँठ), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_ढेर;
    std::vector<गाँठ_type> component_ढेर;
    पूर्णाङ्क next_index = 0;

सार्वजनिक:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        गाँठ_enumerator nodes = graph.enumerate_nodes();
        यावत् (!nodes.finished()) {
            गाँठ_type गाँठ = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(गाँठ);
            यदि (successors.finished())
            {
                graph.dfs_index(गाँठ) = next_index;
                next_index++;
                component_ढेर.धकेलो_back(गाँठ);
                component(component_ढेर.data(), component_ढेर.data() + 1);
                component_ढेर.clear();
                graph.dfs_index(गाँठ) = पूर्णांक_उच्च;
            }
        }
        प्रत्यागम *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        गाँठ_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        यावत् (!nodes.finished())
            process(nodes.next());
        प्रत्यागम *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(गाँठ_type गाँठ) {
        // only start a new search if the node wasn't visited yet
        यदि (graph.dfs_index(गाँठ) >= 0)
            प्रत्यागम *this;
        यावत् (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            पूर्णाङ्क lowlink = next_index;
            next_index++;
            graph.dfs_index(गाँठ) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_ढेर.धकेलो_back(गाँठ);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(गाँठ);
            यावत् (true) {
                यदि (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    यदि (lowlink == graph.dfs_index(गाँठ)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        पूर्णाङ्क current = GetSize(component_ढेर);
                        कुरु {
                            --current;
                        } यावत् (component_ढेर[current] != गाँठ);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        गाँठ_type *ढेर_ptr = component_ढेर.data();
                        गाँठ_type *component_begin = ढेर_ptr + current;
                        गाँठ_type *component_end = ढेर_ptr + component_ढेर.size();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_end);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        प्रति (; component_begin != component_end; ++component_begin)
                            graph.dfs_index(*component_begin) = पूर्णांक_उच्च;
                        component_ढेर.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    यदि (dfs_ढेर.empty())
                        प्रत्यागम *this;
                    स्वतः &dfs_शीर्ष = dfs_ढेर.back();

                    गाँठ = dfs_शीर्ष.गाँठ;
                    successors = std::move(dfs_शीर्ष.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_शीर्ष.lowlink);
                    dfs_ढेर.निकालो_back();
                    // continue checking the remaining successors of the parent node.
                } अन्यथा {
                    गाँठ_type succ = successors.next();
                    यदि (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_ढेर.emplace_back(गाँठ, std::move(successors), lowlink);
                        गाँठ = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        भङ्ग;
                    } अन्यथा {
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
