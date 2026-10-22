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

#अगर_अघोषित TOPO_SCC_H
#घोषणा TOPO_SCC_H

#समावेश "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SigCellGraph {
खुला:
    प्रकार पूर्णांक गाँठ_type;

    काष्ठा successor_enumerator {
        std::vector<std::pair<पूर्णांक, पूर्णांक>>::const_iterator current, end;
        bool finished() स्थिर { वापस current == end; }
        गाँठ_type अग्र() {
            log_assert(!finished());
            गाँठ_type परिणाम = current->second;
            ++current;
            वापस परिणाम;
        }
    };

    काष्ठा गाँठ_enumerator {
        पूर्णांक current, end;
        bool finished() स्थिर { वापस current == end; }
        गाँठ_type अग्र() {
            log_assert(!finished());
            गाँठ_type परिणाम = current;
            ++current;
            वापस परिणाम;
        }
    };

निजी:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::vector<std::pair<पूर्णांक, पूर्णांक>> edges;
    std::vector<std::pair<पूर्णांक, पूर्णांक>> edge_ranges;
    std::vector<पूर्णांक> indices_;
    पूर्णांक offset;
    bool computed = false;

    व्योम compute() {
        offset = Get_आकार(sig_ids);
        edge_ranges.clear();
        indices_.clear();
        indices_.resize(Get_आकार(sig_ids) + Get_आकार(cell_ids), -1);

        std::sort(edges.begin(), edges.end());
        स्वतः last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        स्वतः edge = edges.begin();
        स्वतः edge_end = edges.end();
        पूर्णांक range_begin = 0;
        क्रम (पूर्णांक गाँठ = -offset, गाँठ_end = Get_आकार(cell_ids); गाँठ != गाँठ_end; ++गाँठ) {
            जबतक (edge != edge_end && edge->first <= गाँठ)
                ++edge;
            पूर्णांक range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

खुला:
    गाँठ_type गाँठ(RTLIL::Cell *cell) { वापस cell_ids(cell); }
    गाँठ_type गाँठ(SigBit स्थिर &bit) { वापस ~sig_ids(bit); }

    bool is_cell(गाँठ_type गाँठ) { वापस गाँठ >= 0; }
    bool is_sig(गाँठ_type गाँठ) { वापस गाँठ < 0; }

    Cell *cell(गाँठ_type गाँठ) { वापस गाँठ >= 0 ? cell_ids[गाँठ] : nullptr; }
    SigBit sig(गाँठ_type गाँठ) { वापस गाँठ < 0 ? sig_ids[~गाँठ] : SigBit(); }

    ढाँचा<typename Src, typename Dst>
    व्योम add_edge(Src &&src, Dst &&dst) {
        computed = false;
        गाँठ_type src_गाँठ = गाँठ(std::forward<Src>(src));
        गाँठ_type dst_गाँठ = गाँठ(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_गाँठ, dst_गाँठ));
    }

    गाँठ_enumerator enumerate_nodes() {
        अगर (!computed) compute();
        वापस {-Get_आकार(sig_ids), Get_आकार(cell_ids)};
    }

    successor_enumerator enumerate_successors(गाँठ_type स्थिर &गाँठ) स्थिर {
        स्वतः range = edge_ranges[गाँठ + offset];
        वापस {edges.begin() + range.first, edges.begin() + range.second};
    }

    पूर्णांक &dfs_सूचकांक(गाँठ_type स्थिर &गाँठ) {
        वापस indices_[गाँठ + offset];
    }
};


class IntGraph {
खुला:
    प्रकार पूर्णांक गाँठ_type;

    काष्ठा successor_enumerator {
        std::vector<std::pair<पूर्णांक, पूर्णांक>>::const_iterator current, end;
        bool finished() स्थिर { वापस current == end; }
        गाँठ_type अग्र() {
            log_assert(!finished());
            गाँठ_type परिणाम = current->second;
            ++current;
            वापस परिणाम;
        }
    };

    काष्ठा गाँठ_enumerator {
        पूर्णांक current, end;
        bool finished() स्थिर { वापस current == end; }
        गाँठ_type अग्र() {
            log_assert(!finished());
            गाँठ_type परिणाम = current;
            ++current;
            वापस परिणाम;
        }
    };

निजी:
    std::vector<std::pair<पूर्णांक, पूर्णांक>> edges;
    std::vector<std::pair<पूर्णांक, पूर्णांक>> edge_ranges;
    std::vector<पूर्णांक> indices_;
    bool computed = false;

    व्योम compute() {
        edge_ranges.clear();

        पूर्णांक गाँठ_end = 0;
        क्रम (स्वतः स्थिर &edge : edges)
            गाँठ_end = std::max(गाँठ_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(गाँठ_end, -1);

        std::sort(edges.begin(), edges.end());
        स्वतः last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        स्वतः edge = edges.begin();
        स्वतः edge_end = edges.end();
        पूर्णांक range_begin = 0;
        क्रम (पूर्णांक गाँठ = 0; गाँठ != गाँठ_end; ++गाँठ) {
            जबतक (edge != edge_end && edge->first <= गाँठ)
                ++edge;
            पूर्णांक range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

खुला:
    व्योम add_edge(पूर्णांक src, पूर्णांक dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        edges.emplace_back(std::make_pair(src, dst));
    }

    गाँठ_enumerator enumerate_nodes() {
        अगर (!computed) compute();
        वापस {0, Get_आकार(indices_)};
    }

    successor_enumerator enumerate_successors(पूर्णांक गाँठ) स्थिर {
        स्वतः range = edge_ranges[गाँठ];
        वापस {edges.begin() + range.first, edges.begin() + range.second};
    }

    पूर्णांक &dfs_सूचकांक(गाँठ_type स्थिर &गाँठ) {
        वापस indices_[गाँठ];
    }
};

ढाँचा<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    प्रकार typename G::गाँठ_enumerator गाँठ_enumerator;
    प्रकार typename G::successor_enumerator successor_enumerator;
    प्रकार typename G::गाँठ_type गाँठ_type;

    काष्ठा dfs_entry {
        गाँठ_type गाँठ;
        successor_enumerator successors;
        पूर्णांक lowlink;

        dfs_entry(गाँठ_type गाँठ, successor_enumerator successors, पूर्णांक lowlink) :
            गाँठ(गाँठ), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_ढेर;
    std::vector<गाँठ_type> component_ढेर;
    पूर्णांक अग्र_सूचकांक = 0;

खुला:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        गाँठ_enumerator nodes = graph.enumerate_nodes();
        जबतक (!nodes.finished()) {
            गाँठ_type गाँठ = nodes.अग्र();
            successor_enumerator successors = graph.enumerate_successors(गाँठ);
            अगर (successors.finished())
            {
                graph.dfs_सूचकांक(गाँठ) = अग्र_सूचकांक;
                अग्र_सूचकांक++;
                component_ढेर.धकेलो_back(गाँठ);
                component(component_ढेर.तथ्य(), component_ढेर.तथ्य() + 1);
                component_ढेर.clear();
                graph.dfs_सूचकांक(गाँठ) = पूर्णांक_उच्च;
            }
        }
        वापस *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        गाँठ_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        जबतक (!nodes.finished())
            process(nodes.अग्र());
        वापस *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(गाँठ_type गाँठ) {
        // only start a new search if the node wasn't visited yet
        अगर (graph.dfs_सूचकांक(गाँठ) >= 0)
            वापस *this;
        जबतक (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            पूर्णांक lowlink = अग्र_सूचकांक;
            अग्र_सूचकांक++;
            graph.dfs_सूचकांक(गाँठ) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_ढेर.धकेलो_back(गाँठ);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(गाँठ);
            जबतक (true) {
                अगर (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    अगर (lowlink == graph.dfs_सूचकांक(गाँठ)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        पूर्णांक current = Get_आकार(component_ढेर);
                        करो {
                            --current;
                        } जबतक (component_ढेर[current] != गाँठ);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        गाँठ_type *ढेर_ptr = component_ढेर.तथ्य();
                        गाँठ_type *component_begin = ढेर_ptr + current;
                        गाँठ_type *component_end = ढेर_ptr + component_ढेर.आकार();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_end);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        क्रम (; component_begin != component_end; ++component_begin)
                            graph.dfs_सूचकांक(*component_begin) = पूर्णांक_उच्च;
                        component_ढेर.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    अगर (dfs_ढेर.empty())
                        वापस *this;
                    स्वतः &dfs_शीर्ष = dfs_ढेर.back();

                    गाँठ = dfs_शीर्ष.गाँठ;
                    successors = std::move(dfs_शीर्ष.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_शीर्ष.lowlink);
                    dfs_ढेर.निकालो_back();
                    // continue checking the remaining successors of the parent node.
                } अन्यथा {
                    गाँठ_type succ = successors.अग्र();
                    अगर (graph.dfs_सूचकांक(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_ढेर.emplace_back(गाँठ, std::move(successors), lowlink);
                        गाँठ = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        अवरोध;
                    } अन्यथा {
                        // the textbook version guards this update with a check
                        // whether the successor is still on the component
                        // stack. If the successor node was already visisted
                        // but is not on the component stack, it must be part
                        // of an already emitted SCC. We can avoid this check
                        // by setting the DFS index of all nodes in a SCC to
                        // INT_MAX when the SCC is emitted.
                        lowlink = min(lowlink, graph.dfs_सूचकांक(succ));
                    }
                }
            }
        }
    }
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
