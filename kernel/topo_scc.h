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

#यदि_अपरिभाषितम् TOPO_SCC_H
#परिभाषय TOPO_SCC_H

#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SigCellGraph {
सार्वजनिक:
    प्रकारनिर्देश पूर्णाङ्क ग्रन्थिः_type;

    संरचना successor_enumerator {
        std::सदिशम्<std::pair<पूर्णाङ्क, पूर्णाङ्क>>::const_iterator current, अन्तः;
        bool परिसमाप्तम्() ध्रुव { प्रत्यागम current == अन्तः; }
        ग्रन्थिः_type अग्र() {
            log_assert(!परिसमाप्तम्());
            ग्रन्थिः_type परिणाम = current->second;
            ++current;
            प्रत्यागम परिणाम;
        }
    };

    संरचना ग्रन्थिः_enumerator {
        पूर्णाङ्क current, अन्तः;
        bool परिसमाप्तम्() ध्रुव { प्रत्यागम current == अन्तः; }
        ग्रन्थिः_type अग्र() {
            log_assert(!परिसमाप्तम्());
            ग्रन्थिः_type परिणाम = current;
            ++current;
            प्रत्यागम परिणाम;
        }
    };

गुप्त:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::सदिशम्<std::pair<पूर्णाङ्क, पूर्णाङ्क>> धाराः;
    std::सदिशम्<std::pair<पूर्णाङ्क, पूर्णाङ्क>> edge_ranges;
    std::सदिशम्<पूर्णाङ्क> indices_;
    पूर्णाङ्क विस्थापनम्;
    bool computed = false;

    शून्य compute() {
        विस्थापनम् = Get_आकार(sig_ids);
        edge_ranges.clear();
        indices_.clear();
        indices_.resize(Get_आकार(sig_ids) + Get_आकार(cell_ids), -1);

        std::क्रमय(धाराः.begin(), धाराः.अन्तः());
        स्वचालित चरमः = std::unique(धाराः.begin(), धाराः.अन्तः());
        धाराः.erase(चरमः, धाराः.अन्तः());
        स्वचालित edge = धाराः.begin();
        स्वचालित edge_अन्तः = धाराः.अन्तः();
        पूर्णाङ्क range_begin = 0;
        प्रति (पूर्णाङ्क ग्रन्थिः = -विस्थापनम्, ग्रन्थिः_अन्तः = Get_आकार(cell_ids); ग्रन्थिः != ग्रन्थिः_अन्तः; ++ग्रन्थिः) {
            यावत् (edge != edge_अन्तः && edge->first <= ग्रन्थिः)
                ++edge;
            पूर्णाङ्क range_अन्तः_2 = edge - धाराः.begin();
            edge_ranges.emplace_पृष्ठभागः(std::make_pair(range_begin, range_अन्तः_2));
            range_begin = range_अन्तः_2;
        }
    }

सार्वजनिक:
    ग्रन्थिः_type ग्रन्थिः(RTLIL::Cell *cell) { प्रत्यागम cell_ids(cell); }
    ग्रन्थिः_type ग्रन्थिः(SigBit ध्रुव &bit) { प्रत्यागम ~sig_ids(bit); }

    bool is_cell(ग्रन्थिः_type ग्रन्थिः) { प्रत्यागम ग्रन्थिः >= 0; }
    bool is_sig(ग्रन्थिः_type ग्रन्थिः) { प्रत्यागम ग्रन्थिः < 0; }

    Cell *cell(ग्रन्थिः_type ग्रन्थिः) { प्रत्यागम ग्रन्थिः >= 0 ? cell_ids[ग्रन्थिः] : nullptr; }
    SigBit sig(ग्रन्थिः_type ग्रन्थिः) { प्रत्यागम ग्रन्थिः < 0 ? sig_ids[~ग्रन्थिः] : SigBit(); }

    प्रतिरूपम्<typename Src, typename Dst>
    शून्य सङ्कलय_edge(Src &&src, Dst &&dst) {
        computed = false;
        ग्रन्थिः_type src_ग्रन्थिः = ग्रन्थिः(std::forward<Src>(src));
        ग्रन्थिः_type dst_ग्रन्थिः = ग्रन्थिः(std::forward<Dst>(dst));
        धाराः.emplace_पृष्ठभागः(std::make_pair(src_ग्रन्थिः, dst_ग्रन्थिः));
    }

    ग्रन्थिः_enumerator enumerate_nodes() {
        यदि (!computed) compute();
        प्रत्यागम {-Get_आकार(sig_ids), Get_आकार(cell_ids)};
    }

    successor_enumerator enumerate_उत्तराधिकारिणः(ग्रन्थिः_type ध्रुव &ग्रन्थिः) ध्रुव {
        स्वचालित range = edge_ranges[ग्रन्थिः + विस्थापनम्];
        प्रत्यागम {धाराः.begin() + range.first, धाराः.begin() + range.second};
    }

    पूर्णाङ्क &dfs_सूचकांक(ग्रन्थिः_type ध्रुव &ग्रन्थिः) {
        प्रत्यागम indices_[ग्रन्थिः + विस्थापनम्];
    }
};


class IntGraph {
सार्वजनिक:
    प्रकारनिर्देश पूर्णाङ्क ग्रन्थिः_type;

    संरचना successor_enumerator {
        std::सदिशम्<std::pair<पूर्णाङ्क, पूर्णाङ्क>>::const_iterator current, अन्तः;
        bool परिसमाप्तम्() ध्रुव { प्रत्यागम current == अन्तः; }
        ग्रन्थिः_type अग्र() {
            log_assert(!परिसमाप्तम्());
            ग्रन्थिः_type परिणाम = current->second;
            ++current;
            प्रत्यागम परिणाम;
        }
    };

    संरचना ग्रन्थिः_enumerator {
        पूर्णाङ्क current, अन्तः;
        bool परिसमाप्तम्() ध्रुव { प्रत्यागम current == अन्तः; }
        ग्रन्थिः_type अग्र() {
            log_assert(!परिसमाप्तम्());
            ग्रन्थिः_type परिणाम = current;
            ++current;
            प्रत्यागम परिणाम;
        }
    };

गुप्त:
    std::सदिशम्<std::pair<पूर्णाङ्क, पूर्णाङ्क>> धाराः;
    std::सदिशम्<std::pair<पूर्णाङ्क, पूर्णाङ्क>> edge_ranges;
    std::सदिशम्<पूर्णाङ्क> indices_;
    bool computed = false;

    शून्य compute() {
        edge_ranges.clear();

        पूर्णाङ्क ग्रन्थिः_अन्तः = 0;
        प्रति (स्वचालित ध्रुव &edge : धाराः)
            ग्रन्थिः_अन्तः = std::max(ग्रन्थिः_अन्तः, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(ग्रन्थिः_अन्तः, -1);

        std::क्रमय(धाराः.begin(), धाराः.अन्तः());
        स्वचालित चरमः = std::unique(धाराः.begin(), धाराः.अन्तः());
        धाराः.erase(चरमः, धाराः.अन्तः());
        स्वचालित edge = धाराः.begin();
        स्वचालित edge_अन्तः = धाराः.अन्तः();
        पूर्णाङ्क range_begin = 0;
        प्रति (पूर्णाङ्क ग्रन्थिः = 0; ग्रन्थिः != ग्रन्थिः_अन्तः; ++ग्रन्थिः) {
            यावत् (edge != edge_अन्तः && edge->first <= ग्रन्थिः)
                ++edge;
            पूर्णाङ्क range_अन्तः_2 = edge - धाराः.begin();
            edge_ranges.emplace_पृष्ठभागः(std::make_pair(range_begin, range_अन्तः_2));
            range_begin = range_अन्तः_2;
        }
    }

सार्वजनिक:
    शून्य सङ्कलय_edge(पूर्णाङ्क src, पूर्णाङ्क dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        धाराः.emplace_पृष्ठभागः(std::make_pair(src, dst));
    }

    ग्रन्थिः_enumerator enumerate_nodes() {
        यदि (!computed) compute();
        प्रत्यागम {0, Get_आकार(indices_)};
    }

    successor_enumerator enumerate_उत्तराधिकारिणः(पूर्णाङ्क ग्रन्थिः) ध्रुव {
        स्वचालित range = edge_ranges[ग्रन्थिः];
        प्रत्यागम {धाराः.begin() + range.first, धाराः.begin() + range.second};
    }

    पूर्णाङ्क &dfs_सूचकांक(ग्रन्थिः_type ध्रुव &ग्रन्थिः) {
        प्रत्यागम indices_[ग्रन्थिः];
    }
};

प्रतिरूपम्<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    प्रकारनिर्देश typename G::ग्रन्थिः_enumerator ग्रन्थिः_enumerator;
    प्रकारनिर्देश typename G::successor_enumerator successor_enumerator;
    प्रकारनिर्देश typename G::ग्रन्थिः_type ग्रन्थिः_type;

    संरचना dfs_प्रविष्टिः {
        ग्रन्थिः_type ग्रन्थिः;
        successor_enumerator उत्तराधिकारिणः;
        पूर्णाङ्क lowlink;

        dfs_प्रविष्टिः(ग्रन्थिः_type ग्रन्थिः, successor_enumerator उत्तराधिकारिणः, पूर्णाङ्क lowlink) :
            ग्रन्थिः(ग्रन्थिः), उत्तराधिकारिणः(उत्तराधिकारिणः), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::सदिशम्<dfs_प्रविष्टिः> dfs_राशिः;
    std::सदिशम्<ग्रन्थिः_type> component_राशिः;
    पूर्णाङ्क अग्र_सूचकांक = 0;

सार्वजनिक:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        ग्रन्थिः_enumerator nodes = graph.enumerate_nodes();
        यावत् (!nodes.परिसमाप्तम्()) {
            ग्रन्थिः_type ग्रन्थिः = nodes.अग्र();
            successor_enumerator उत्तराधिकारिणः = graph.enumerate_उत्तराधिकारिणः(ग्रन्थिः);
            यदि (उत्तराधिकारिणः.परिसमाप्तम्())
            {
                graph.dfs_सूचकांक(ग्रन्थिः) = अग्र_सूचकांक;
                अग्र_सूचकांक++;
                component_राशिः.प्रक्षिप_पृष्ठभागः(ग्रन्थिः);
                component(component_राशिः.दत्त(), component_राशिः.दत्त() + 1);
                component_राशिः.clear();
                graph.dfs_सूचकांक(ग्रन्थिः) = अधिकतम_पूर्णाङ्कः;
            }
        }
        प्रत्यागम *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        ग्रन्थिः_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        यावत् (!nodes.परिसमाप्तम्())
            process(nodes.अग्र());
        प्रत्यागम *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(ग्रन्थिः_type ग्रन्थिः) {
        // only start a new search if the node wasn't visited yet
        यदि (graph.dfs_सूचकांक(ग्रन्थिः) >= 0)
            प्रत्यागम *this;
        यावत् (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            पूर्णाङ्क lowlink = अग्र_सूचकांक;
            अग्र_सूचकांक++;
            graph.dfs_सूचकांक(ग्रन्थिः) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_राशिः.प्रक्षिप_पृष्ठभागः(ग्रन्थिः);

            // then we start iterating over the successors of this node
            successor_enumerator उत्तराधिकारिणः = graph.enumerate_उत्तराधिकारिणः(ग्रन्थिः);
            यावत् (true) {
                यदि (उत्तराधिकारिणः.परिसमाप्तम्()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    यदि (lowlink == graph.dfs_सूचकांक(ग्रन्थिः)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        पूर्णाङ्क current = Get_आकार(component_राशिः);
                        कुरु {
                            --current;
                        } यावत् (component_राशिः[current] != ग्रन्थिः);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        ग्रन्थिः_type *राशिः_ptr = component_राशिः.दत्त();
                        ग्रन्थिः_type *component_begin = राशिः_ptr + current;
                        ग्रन्थिः_type *component_अन्तः = राशिः_ptr + component_राशिः.आकार();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_अन्तः);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        प्रति (; component_begin != component_अन्तः; ++component_begin)
                            graph.dfs_सूचकांक(*component_begin) = अधिकतम_पूर्णाङ्कः;
                        component_राशिः.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    यदि (dfs_राशिः.रिक्तानि())
                        प्रत्यागम *this;
                    स्वचालित &dfs_शिखरम् = dfs_राशिः.पृष्ठभागः();

                    ग्रन्थिः = dfs_शिखरम्.ग्रन्थिः;
                    उत्तराधिकारिणः = std::move(dfs_शिखरम्.उत्तराधिकारिणः);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_शिखरम्.lowlink);
                    dfs_राशिः.निष्कासय_पृष्ठभागः();
                    // continue checking the remaining successors of the parent node.
                } अन्यथा {
                    ग्रन्थिः_type succ = उत्तराधिकारिणः.अग्र();
                    यदि (graph.dfs_सूचकांक(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_राशिः.emplace_पृष्ठभागः(ग्रन्थिः, std::move(उत्तराधिकारिणः), lowlink);
                        ग्रन्थिः = succ;

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
                        lowlink = min(lowlink, graph.dfs_सूचकांक(succ));
                    }
                }
            }
        }
    }
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
