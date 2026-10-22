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

#నిర్వచితం_కాకపోతే TOPO_SCC_H
#నిర్వచించు TOPO_SCC_H

#అన్తర్భావ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class Sig_Cell_లేఖాచిత్రం {
బహిరంగ:
    రకనిర్వచనం పూర్ణాంకం కణుపు_type;

    నిర్మాణం successor_enumerator {
        std::సదిశ<std::pair<పూర్ణాంకం, పూర్ణాంకం>>::const_iterator current, చివర;
        bool ముగిసిన() స్థిరరాశి { తిరిగి current == చివర; }
        కణుపు_type తదుపరి() {
            log_assert(!ముగిసిన());
            కణుపు_type ఫలితం = current->రెండవ;
            ++current;
            తిరిగి ఫలితం;
        }
    };

    నిర్మాణం కణుపు_enumerator {
        పూర్ణాంకం current, చివర;
        bool ముగిసిన() స్థిరరాశి { తిరిగి current == చివర; }
        కణుపు_type తదుపరి() {
            log_assert(!ముగిసిన());
            కణుపు_type ఫలితం = current;
            ++current;
            తిరిగి ఫలితం;
        }
    };

అంతరంగ:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::Sig_బిట్టు> sig_ids;
    std::సదిశ<std::pair<పూర్ణాంకం, పూర్ణాంకం>> అంచులు;
    std::సదిశ<std::pair<పూర్ణాంకం, పూర్ణాంకం>> అంచు_ranges;
    std::సదిశ<పూర్ణాంకం> indices_;
    పూర్ణాంకం స్థానభ్రంశం;
    bool computed = false;

    శూన్య compute() {
        స్థానభ్రంశం = Get_కొలత(sig_ids);
        అంచు_ranges.clear();
        indices_.clear();
        indices_.resize(Get_కొలత(sig_ids) + Get_కొలత(cell_ids), -1);

        std::క్రమీకరించు(అంచులు.begin(), అంచులు.చివర());
        స్వయంచాలక చివరి = std::unique(అంచులు.begin(), అంచులు.చివర());
        అంచులు.erase(చివరి, అంచులు.చివర());
        స్వయంచాలక అంచు = అంచులు.begin();
        స్వయంచాలక అంచు_చివర = అంచులు.చివర();
        పూర్ణాంకం range_begin = 0;
        కొరకు (పూర్ణాంకం కణుపు = -స్థానభ్రంశం, కణుపు_చివర = Get_కొలత(cell_ids); కణుపు != కణుపు_చివర; ++కణుపు) {
            అయితే (అంచు != అంచు_చివర && అంచు->మొదటి <= కణుపు)
                ++అంచు;
            పూర్ణాంకం range_చివర_2 = అంచు - అంచులు.begin();
            అంచు_ranges.emplace_వెనుక(std::make_pair(range_begin, range_చివర_2));
            range_begin = range_చివర_2;
        }
    }

బహిరంగ:
    కణుపు_type కణుపు(RTLIL::Cell *cell) { తిరిగి cell_ids(cell); }
    కణుపు_type కణుపు(Sig_బిట్టు స్థిరరాశి &బిట్టు) { తిరిగి ~sig_ids(బిట్టు); }

    bool is_cell(కణుపు_type కణుపు) { తిరిగి కణుపు >= 0; }
    bool is_sig(కణుపు_type కణుపు) { తిరిగి కణుపు < 0; }

    Cell *cell(కణుపు_type కణుపు) { తిరిగి కణుపు >= 0 ? cell_ids[కణుపు] : nullptr; }
    Sig_బిట్టు sig(కణుపు_type కణుపు) { తిరిగి కణుపు < 0 ? sig_ids[~కణుపు] : Sig_బిట్టు(); }

    మూస<typename Src, typename Dst>
    శూన్య కూడు_అంచు(Src &&src, Dst &&dst) {
        computed = false;
        కణుపు_type src_కణుపు = కణుపు(std::forward<Src>(src));
        కణుపు_type dst_కణుపు = కణుపు(std::forward<Dst>(dst));
        అంచులు.emplace_వెనుక(std::make_pair(src_కణుపు, dst_కణుపు));
    }

    కణుపు_enumerator enumerate_nodes() {
        ఒకవేళ (!computed) compute();
        తిరిగి {-Get_కొలత(sig_ids), Get_కొలత(cell_ids)};
    }

    successor_enumerator enumerate_వారసులు(కణుపు_type స్థిరరాశి &కణుపు) స్థిరరాశి {
        స్వయంచాలక range = అంచు_ranges[కణుపు + స్థానభ్రంశం];
        తిరిగి {అంచులు.begin() + range.మొదటి, అంచులు.begin() + range.రెండవ};
    }

    పూర్ణాంకం &dfs_సూచీ(కణుపు_type స్థిరరాశి &కణుపు) {
        తిరిగి indices_[కణుపు + స్థానభ్రంశం];
    }
};


class Int_లేఖాచిత్రం {
బహిరంగ:
    రకనిర్వచనం పూర్ణాంకం కణుపు_type;

    నిర్మాణం successor_enumerator {
        std::సదిశ<std::pair<పూర్ణాంకం, పూర్ణాంకం>>::const_iterator current, చివర;
        bool ముగిసిన() స్థిరరాశి { తిరిగి current == చివర; }
        కణుపు_type తదుపరి() {
            log_assert(!ముగిసిన());
            కణుపు_type ఫలితం = current->రెండవ;
            ++current;
            తిరిగి ఫలితం;
        }
    };

    నిర్మాణం కణుపు_enumerator {
        పూర్ణాంకం current, చివర;
        bool ముగిసిన() స్థిరరాశి { తిరిగి current == చివర; }
        కణుపు_type తదుపరి() {
            log_assert(!ముగిసిన());
            కణుపు_type ఫలితం = current;
            ++current;
            తిరిగి ఫలితం;
        }
    };

అంతరంగ:
    std::సదిశ<std::pair<పూర్ణాంకం, పూర్ణాంకం>> అంచులు;
    std::సదిశ<std::pair<పూర్ణాంకం, పూర్ణాంకం>> అంచు_ranges;
    std::సదిశ<పూర్ణాంకం> indices_;
    bool computed = false;

    శూన్య compute() {
        అంచు_ranges.clear();

        పూర్ణాంకం కణుపు_చివర = 0;
        కొరకు (స్వయంచాలక స్థిరరాశి &అంచు : అంచులు)
            కణుపు_చివర = std::max(కణుపు_చివర, std::max(అంచు.మొదటి, అంచు.రెండవ) + 1);
        indices_.clear();
        indices_.resize(కణుపు_చివర, -1);

        std::క్రమీకరించు(అంచులు.begin(), అంచులు.చివర());
        స్వయంచాలక చివరి = std::unique(అంచులు.begin(), అంచులు.చివర());
        అంచులు.erase(చివరి, అంచులు.చివర());
        స్వయంచాలక అంచు = అంచులు.begin();
        స్వయంచాలక అంచు_చివర = అంచులు.చివర();
        పూర్ణాంకం range_begin = 0;
        కొరకు (పూర్ణాంకం కణుపు = 0; కణుపు != కణుపు_చివర; ++కణుపు) {
            అయితే (అంచు != అంచు_చివర && అంచు->మొదటి <= కణుపు)
                ++అంచు;
            పూర్ణాంకం range_చివర_2 = అంచు - అంచులు.begin();
            అంచు_ranges.emplace_వెనుక(std::make_pair(range_begin, range_చివర_2));
            range_begin = range_చివర_2;
        }
    }

బహిరంగ:
    శూన్య కూడు_అంచు(పూర్ణాంకం src, పూర్ణాంకం dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        అంచులు.emplace_వెనుక(std::make_pair(src, dst));
    }

    కణుపు_enumerator enumerate_nodes() {
        ఒకవేళ (!computed) compute();
        తిరిగి {0, Get_కొలత(indices_)};
    }

    successor_enumerator enumerate_వారసులు(పూర్ణాంకం కణుపు) స్థిరరాశి {
        స్వయంచాలక range = అంచు_ranges[కణుపు];
        తిరిగి {అంచులు.begin() + range.మొదటి, అంచులు.begin() + range.రెండవ};
    }

    పూర్ణాంకం &dfs_సూచీ(కణుపు_type స్థిరరాశి &కణుపు) {
        తిరిగి indices_[కణుపు];
    }
};

మూస<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    రకనిర్వచనం typename G::కణుపు_enumerator కణుపు_enumerator;
    రకనిర్వచనం typename G::successor_enumerator successor_enumerator;
    రకనిర్వచనం typename G::కణుపు_type కణుపు_type;

    నిర్మాణం dfs_నమోదు {
        కణుపు_type కణుపు;
        successor_enumerator వారసులు;
        పూర్ణాంకం lowlink;

        dfs_నమోదు(కణుపు_type కణుపు, successor_enumerator వారసులు, పూర్ణాంకం lowlink) :
            కణుపు(కణుపు), వారసులు(వారసులు), lowlink(lowlink)
        {}
    };

    G &లేఖాచిత్రం_3;
    ComponentCallback component;

    std::సదిశ<dfs_నమోదు> dfs_దొంతర;
    std::సదిశ<కణుపు_type> component_దొంతర;
    పూర్ణాంకం తదుపరి_సూచీ = 0;

బహిరంగ:
    TopoSortedSccs(G &లేఖాచిత్రం_3, ComponentCallback component)
    : లేఖాచిత్రం_3(లేఖాచిత్రం_3), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        కణుపు_enumerator nodes = లేఖాచిత్రం_3.enumerate_nodes();
        అయితే (!nodes.ముగిసిన()) {
            కణుపు_type కణుపు = nodes.తదుపరి();
            successor_enumerator వారసులు = లేఖాచిత్రం_3.enumerate_వారసులు(కణుపు);
            ఒకవేళ (వారసులు.ముగిసిన())
            {
                లేఖాచిత్రం_3.dfs_సూచీ(కణుపు) = తదుపరి_సూచీ;
                తదుపరి_సూచీ++;
                component_దొంతర.నెట్టు_వెనుక(కణుపు);
                component(component_దొంతర.దత్తాంశం(), component_దొంతర.దత్తాంశం() + 1);
                component_దొంతర.clear();
                లేఖాచిత్రం_3.dfs_సూచీ(కణుపు) = గరిష్ఠ_పూర్ణాంకం;
            }
        }
        తిరిగి *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        కణుపు_enumerator nodes = లేఖాచిత్రం_3.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        అయితే (!nodes.ముగిసిన())
            process(nodes.తదుపరి());
        తిరిగి *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(కణుపు_type కణుపు) {
        // only start a new search if the node wasn't visited yet
        ఒకవేళ (లేఖాచిత్రం_3.dfs_సూచీ(కణుపు) >= 0)
            తిరిగి *this;
        అయితే (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            పూర్ణాంకం lowlink = తదుపరి_సూచీ;
            తదుపరి_సూచీ++;
            లేఖాచిత్రం_3.dfs_సూచీ(కణుపు) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_దొంతర.నెట్టు_వెనుక(కణుపు);

            // then we start iterating over the successors of this node
            successor_enumerator వారసులు = లేఖాచిత్రం_3.enumerate_వారసులు(కణుపు);
            అయితే (true) {
                ఒకవేళ (వారసులు.ముగిసిన()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    ఒకవేళ (lowlink == లేఖాచిత్రం_3.dfs_సూచీ(కణుపు)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        పూర్ణాంకం current = Get_కొలత(component_దొంతర);
                        చేయి {
                            --current;
                        } అయితే (component_దొంతర[current] != కణుపు);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        కణుపు_type *దొంతర_ptr = component_దొంతర.దత్తాంశం();
                        కణుపు_type *component_begin = దొంతర_ptr + current;
                        కణుపు_type *component_చివర = దొంతర_ptr + component_దొంతర.కొలత();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_చివర);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        కొరకు (; component_begin != component_చివర; ++component_begin)
                            లేఖాచిత్రం_3.dfs_సూచీ(*component_begin) = గరిష్ఠ_పూర్ణాంకం;
                        component_దొంతర.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    ఒకవేళ (dfs_దొంతర.ఖాళీలు())
                        తిరిగి *this;
                    స్వయంచాలక &dfs_శిఖరం = dfs_దొంతర.వెనుక();

                    కణుపు = dfs_శిఖరం.కణుపు;
                    వారసులు = std::move(dfs_శిఖరం.వారసులు);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_శిఖరం.lowlink);
                    dfs_దొంతర.తీయి_వెనుక();
                    // continue checking the remaining successors of the parent node.
                } లేకపోతే {
                    కణుపు_type succ = వారసులు.తదుపరి();
                    ఒకవేళ (లేఖాచిత్రం_3.dfs_సూచీ(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_దొంతర.emplace_వెనుక(కణుపు, std::move(వారసులు), lowlink);
                        కణుపు = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        ఆపు;
                    } లేకపోతే {
                        // the textbook version guards this update with a check
                        // whether the successor is still on the component
                        // stack. If the successor node was already visisted
                        // but is not on the component stack, it must be part
                        // of an already emitted SCC. We can avoid this check
                        // by setting the DFS index of all nodes in a SCC to
                        // INT_MAX when the SCC is emitted.
                        lowlink = min(lowlink, లేఖాచిత్రం_3.dfs_సూచీ(succ));
                    }
                }
            }
        }
    }
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
