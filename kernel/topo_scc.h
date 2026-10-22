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
    प्रकारव्याख्या पूर्णांक गाठ_type;

    संरचना successor_enumerator {
        std::vector<std::pair<पूर्णांक, पूर्णांक>>::const_iterator current, end;
        bool finished() स्थिरांक { परत current == end; }
        गाठ_type next() {
            log_assert(!finished());
            गाठ_type result = current->second;
            ++current;
            परत result;
        }
    };

    संरचना गाठ_enumerator {
        पूर्णांक current, end;
        bool finished() स्थिरांक { परत current == end; }
        गाठ_type next() {
            log_assert(!finished());
            गाठ_type result = current;
            ++current;
            परत result;
        }
    };

खाजगी:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::SigBit> sig_ids;
    std::vector<std::pair<पूर्णांक, पूर्णांक>> edges;
    std::vector<std::pair<पूर्णांक, पूर्णांक>> edge_ranges;
    std::vector<पूर्णांक> indices_;
    पूर्णांक offset;
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
        पूर्णांक range_begin = 0;
        साठी (पूर्णांक गाठ = -offset, गाठ_end = GetSize(cell_ids); गाठ != गाठ_end; ++गाठ) {
            पर्यंत (edge != edge_end && edge->first <= गाठ)
                ++edge;
            पूर्णांक range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

सार्वजनिक:
    गाठ_type गाठ(RTLIL::Cell *cell) { परत cell_ids(cell); }
    गाठ_type गाठ(SigBit स्थिरांक &bit) { परत ~sig_ids(bit); }

    bool is_cell(गाठ_type गाठ) { परत गाठ >= 0; }
    bool is_sig(गाठ_type गाठ) { परत गाठ < 0; }

    Cell *cell(गाठ_type गाठ) { परत गाठ >= 0 ? cell_ids[गाठ] : nullptr; }
    SigBit sig(गाठ_type गाठ) { परत गाठ < 0 ? sig_ids[~गाठ] : SigBit(); }

    ढाँचा<typename Src, typename Dst>
    शून्य add_edge(Src &&src, Dst &&dst) {
        computed = false;
        गाठ_type src_गाठ = गाठ(std::forward<Src>(src));
        गाठ_type dst_गाठ = गाठ(std::forward<Dst>(dst));
        edges.emplace_back(std::make_pair(src_गाठ, dst_गाठ));
    }

    गाठ_enumerator enumerate_nodes() {
        जर (!computed) compute();
        परत {-GetSize(sig_ids), GetSize(cell_ids)};
    }

    successor_enumerator enumerate_successors(गाठ_type स्थिरांक &गाठ) स्थिरांक {
        स्वतः range = edge_ranges[गाठ + offset];
        परत {edges.begin() + range.first, edges.begin() + range.second};
    }

    पूर्णांक &dfs_index(गाठ_type स्थिरांक &गाठ) {
        परत indices_[गाठ + offset];
    }
};


class IntGraph {
सार्वजनिक:
    प्रकारव्याख्या पूर्णांक गाठ_type;

    संरचना successor_enumerator {
        std::vector<std::pair<पूर्णांक, पूर्णांक>>::const_iterator current, end;
        bool finished() स्थिरांक { परत current == end; }
        गाठ_type next() {
            log_assert(!finished());
            गाठ_type result = current->second;
            ++current;
            परत result;
        }
    };

    संरचना गाठ_enumerator {
        पूर्णांक current, end;
        bool finished() स्थिरांक { परत current == end; }
        गाठ_type next() {
            log_assert(!finished());
            गाठ_type result = current;
            ++current;
            परत result;
        }
    };

खाजगी:
    std::vector<std::pair<पूर्णांक, पूर्णांक>> edges;
    std::vector<std::pair<पूर्णांक, पूर्णांक>> edge_ranges;
    std::vector<पूर्णांक> indices_;
    bool computed = false;

    शून्य compute() {
        edge_ranges.clear();

        पूर्णांक गाठ_end = 0;
        साठी (स्वतः स्थिरांक &edge : edges)
            गाठ_end = std::max(गाठ_end, std::max(edge.first, edge.second) + 1);
        indices_.clear();
        indices_.resize(गाठ_end, -1);

        std::sort(edges.begin(), edges.end());
        स्वतः last = std::unique(edges.begin(), edges.end());
        edges.erase(last, edges.end());
        स्वतः edge = edges.begin();
        स्वतः edge_end = edges.end();
        पूर्णांक range_begin = 0;
        साठी (पूर्णांक गाठ = 0; गाठ != गाठ_end; ++गाठ) {
            पर्यंत (edge != edge_end && edge->first <= गाठ)
                ++edge;
            पूर्णांक range_end = edge - edges.begin();
            edge_ranges.emplace_back(std::make_pair(range_begin, range_end));
            range_begin = range_end;
        }
    }

सार्वजनिक:
    शून्य add_edge(पूर्णांक src, पूर्णांक dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        edges.emplace_back(std::make_pair(src, dst));
    }

    गाठ_enumerator enumerate_nodes() {
        जर (!computed) compute();
        परत {0, GetSize(indices_)};
    }

    successor_enumerator enumerate_successors(पूर्णांक गाठ) स्थिरांक {
        स्वतः range = edge_ranges[गाठ];
        परत {edges.begin() + range.first, edges.begin() + range.second};
    }

    पूर्णांक &dfs_index(गाठ_type स्थिरांक &गाठ) {
        परत indices_[गाठ];
    }
};

ढाँचा<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    प्रकारव्याख्या typename G::गाठ_enumerator गाठ_enumerator;
    प्रकारव्याख्या typename G::successor_enumerator successor_enumerator;
    प्रकारव्याख्या typename G::गाठ_type गाठ_type;

    संरचना dfs_entry {
        गाठ_type गाठ;
        successor_enumerator successors;
        पूर्णांक lowlink;

        dfs_entry(गाठ_type गाठ, successor_enumerator successors, पूर्णांक lowlink) :
            गाठ(गाठ), successors(successors), lowlink(lowlink)
        {}
    };

    G &graph;
    ComponentCallback component;

    std::vector<dfs_entry> dfs_चळत;
    std::vector<गाठ_type> component_चळत;
    पूर्णांक next_index = 0;

सार्वजनिक:
    TopoSortedSccs(G &graph, ComponentCallback component)
    : graph(graph), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        गाठ_enumerator nodes = graph.enumerate_nodes();
        पर्यंत (!nodes.finished()) {
            गाठ_type गाठ = nodes.next();
            successor_enumerator successors = graph.enumerate_successors(गाठ);
            जर (successors.finished())
            {
                graph.dfs_index(गाठ) = next_index;
                next_index++;
                component_चळत.ढकला_back(गाठ);
                component(component_चळत.data(), component_चळत.data() + 1);
                component_चळत.clear();
                graph.dfs_index(गाठ) = पूर्णांक_उच्च;
            }
        }
        परत *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        गाठ_enumerator nodes = graph.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        पर्यंत (!nodes.finished())
            process(nodes.next());
        परत *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(गाठ_type गाठ) {
        // only start a new search if the node wasn't visited yet
        जर (graph.dfs_index(गाठ) >= 0)
            परत *this;
        पर्यंत (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            पूर्णांक lowlink = next_index;
            next_index++;
            graph.dfs_index(गाठ) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_चळत.ढकला_back(गाठ);

            // then we start iterating over the successors of this node
            successor_enumerator successors = graph.enumerate_successors(गाठ);
            पर्यंत (true) {
                जर (successors.finished()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    जर (lowlink == graph.dfs_index(गाठ)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        पूर्णांक current = GetSize(component_चळत);
                        करा {
                            --current;
                        } पर्यंत (component_चळत[current] != गाठ);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        गाठ_type *चळत_ptr = component_चळत.data();
                        गाठ_type *component_begin = चळत_ptr + current;
                        गाठ_type *component_end = चळत_ptr + component_चळत.size();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_end);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        साठी (; component_begin != component_end; ++component_begin)
                            graph.dfs_index(*component_begin) = पूर्णांक_उच्च;
                        component_चळत.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    जर (dfs_चळत.empty())
                        परत *this;
                    स्वतः &dfs_शीर्ष = dfs_चळत.back();

                    गाठ = dfs_शीर्ष.गाठ;
                    successors = std::move(dfs_शीर्ष.successors);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_शीर्ष.lowlink);
                    dfs_चळत.काढा_back();
                    // continue checking the remaining successors of the parent node.
                } अन्यथा {
                    गाठ_type succ = successors.next();
                    जर (graph.dfs_index(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_चळत.emplace_back(गाठ, std::move(successors), lowlink);
                        गाठ = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        थांबा;
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
