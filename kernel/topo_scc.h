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

#جيڪڏهن_وصف_نه_ٿيل TOPO_SCC_H
#وصف_ڪريو TOPO_SCC_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class Sig_Cell_گراف {
عام:
    تعريفنوع صحيح ڳنڍ_type;

    بنية successor_enumerator {
        std::ويڪٽر<std::pair<صحيح, صحيح>>::const_iterator current, پڇاڙي;
        bool ختم() ثابت { أرجع current == پڇاڙي; }
        ڳنڍ_type ايندڙ() {
            log_assert(!ختم());
            ڳنڍ_type نتيجو = current->ٻيو;
            ++current;
            أرجع نتيجو;
        }
    };

    بنية ڳنڍ_enumerator {
        صحيح current, پڇاڙي;
        bool ختم() ثابت { أرجع current == پڇاڙي; }
        ڳنڍ_type ايندڙ() {
            log_assert(!ختم());
            ڳنڍ_type نتيجو = current;
            ++current;
            أرجع نتيجو;
        }
    };

خاص:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::Sig_ٻٽو_انگ> sig_ids;
    std::ويڪٽر<std::pair<صحيح, صحيح>> ڪنارا;
    std::ويڪٽر<std::pair<صحيح, صحيح>> ڪنارو_ranges;
    std::ويڪٽر<صحيح> indices_;
    صحيح وٿي;
    bool computed = false;

    فراغ compute() {
        وٿي = Get_سائيز(sig_ids);
        ڪنارو_ranges.clear();
        indices_.clear();
        indices_.resize(Get_سائيز(sig_ids) + Get_سائيز(cell_ids), -1);

        std::ترتيب_ڏيو(ڪنارا.begin(), ڪنارا.پڇاڙي());
        خودڪار آخري = std::unique(ڪنارا.begin(), ڪنارا.پڇاڙي());
        ڪنارا.erase(آخري, ڪنارا.پڇاڙي());
        خودڪار ڪنارو = ڪنارا.begin();
        خودڪار ڪنارو_پڇاڙي = ڪنارا.پڇاڙي();
        صحيح range_begin = 0;
        لكل (صحيح ڳنڍ = -وٿي, ڳنڍ_پڇاڙي = Get_سائيز(cell_ids); ڳنڍ != ڳنڍ_پڇاڙي; ++ڳنڍ) {
            طالما (ڪنارو != ڪنارو_پڇاڙي && ڪنارو->پهريون <= ڳنڍ)
                ++ڪنارو;
            صحيح range_پڇاڙي_2 = ڪنارو - ڪنارا.begin();
            ڪنارو_ranges.emplace_پٺ(std::make_pair(range_begin, range_پڇاڙي_2));
            range_begin = range_پڇاڙي_2;
        }
    }

عام:
    ڳنڍ_type ڳنڍ(RTLIL::Cell *cell) { أرجع cell_ids(cell); }
    ڳنڍ_type ڳنڍ(Sig_ٻٽو_انگ ثابت &ٻٽو_انگ) { أرجع ~sig_ids(ٻٽو_انگ); }

    bool is_cell(ڳنڍ_type ڳنڍ) { أرجع ڳنڍ >= 0; }
    bool is_sig(ڳنڍ_type ڳنڍ) { أرجع ڳنڍ < 0; }

    Cell *cell(ڳنڍ_type ڳنڍ) { أرجع ڳنڍ >= 0 ? cell_ids[ڳنڍ] : nullptr; }
    Sig_ٻٽو_انگ sig(ڳنڍ_type ڳنڍ) { أرجع ڳنڍ < 0 ? sig_ids[~ڳنڍ] : Sig_ٻٽو_انگ(); }

    سانچو<typename Src, typename Dst>
    فراغ جوڙيو_ڪنارو(Src &&src, Dst &&dst) {
        computed = false;
        ڳنڍ_type src_ڳنڍ = ڳنڍ(std::forward<Src>(src));
        ڳنڍ_type dst_ڳنڍ = ڳنڍ(std::forward<Dst>(dst));
        ڪنارا.emplace_پٺ(std::make_pair(src_ڳنڍ, dst_ڳنڍ));
    }

    ڳنڍ_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {-Get_سائيز(sig_ids), Get_سائيز(cell_ids)};
    }

    successor_enumerator enumerate_جانشين(ڳنڍ_type ثابت &ڳنڍ) ثابت {
        خودڪار range = ڪنارو_ranges[ڳنڍ + وٿي];
        أرجع {ڪنارا.begin() + range.پهريون, ڪنارا.begin() + range.ٻيو};
    }

    صحيح &dfs_اشاريو(ڳنڍ_type ثابت &ڳنڍ) {
        أرجع indices_[ڳنڍ + وٿي];
    }
};


class Int_گراف {
عام:
    تعريفنوع صحيح ڳنڍ_type;

    بنية successor_enumerator {
        std::ويڪٽر<std::pair<صحيح, صحيح>>::const_iterator current, پڇاڙي;
        bool ختم() ثابت { أرجع current == پڇاڙي; }
        ڳنڍ_type ايندڙ() {
            log_assert(!ختم());
            ڳنڍ_type نتيجو = current->ٻيو;
            ++current;
            أرجع نتيجو;
        }
    };

    بنية ڳنڍ_enumerator {
        صحيح current, پڇاڙي;
        bool ختم() ثابت { أرجع current == پڇاڙي; }
        ڳنڍ_type ايندڙ() {
            log_assert(!ختم());
            ڳنڍ_type نتيجو = current;
            ++current;
            أرجع نتيجو;
        }
    };

خاص:
    std::ويڪٽر<std::pair<صحيح, صحيح>> ڪنارا;
    std::ويڪٽر<std::pair<صحيح, صحيح>> ڪنارو_ranges;
    std::ويڪٽر<صحيح> indices_;
    bool computed = false;

    فراغ compute() {
        ڪنارو_ranges.clear();

        صحيح ڳنڍ_پڇاڙي = 0;
        لكل (خودڪار ثابت &ڪنارو : ڪنارا)
            ڳنڍ_پڇاڙي = std::max(ڳنڍ_پڇاڙي, std::max(ڪنارو.پهريون, ڪنارو.ٻيو) + 1);
        indices_.clear();
        indices_.resize(ڳنڍ_پڇاڙي, -1);

        std::ترتيب_ڏيو(ڪنارا.begin(), ڪنارا.پڇاڙي());
        خودڪار آخري = std::unique(ڪنارا.begin(), ڪنارا.پڇاڙي());
        ڪنارا.erase(آخري, ڪنارا.پڇاڙي());
        خودڪار ڪنارو = ڪنارا.begin();
        خودڪار ڪنارو_پڇاڙي = ڪنارا.پڇاڙي();
        صحيح range_begin = 0;
        لكل (صحيح ڳنڍ = 0; ڳنڍ != ڳنڍ_پڇاڙي; ++ڳنڍ) {
            طالما (ڪنارو != ڪنارو_پڇاڙي && ڪنارو->پهريون <= ڳنڍ)
                ++ڪنارو;
            صحيح range_پڇاڙي_2 = ڪنارو - ڪنارا.begin();
            ڪنارو_ranges.emplace_پٺ(std::make_pair(range_begin, range_پڇاڙي_2));
            range_begin = range_پڇاڙي_2;
        }
    }

عام:
    فراغ جوڙيو_ڪنارو(صحيح src, صحيح dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        ڪنارا.emplace_پٺ(std::make_pair(src, dst));
    }

    ڳنڍ_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {0, Get_سائيز(indices_)};
    }

    successor_enumerator enumerate_جانشين(صحيح ڳنڍ) ثابت {
        خودڪار range = ڪنارو_ranges[ڳنڍ];
        أرجع {ڪنارا.begin() + range.پهريون, ڪنارا.begin() + range.ٻيو};
    }

    صحيح &dfs_اشاريو(ڳنڍ_type ثابت &ڳنڍ) {
        أرجع indices_[ڳنڍ];
    }
};

سانچو<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    تعريفنوع typename G::ڳنڍ_enumerator ڳنڍ_enumerator;
    تعريفنوع typename G::successor_enumerator successor_enumerator;
    تعريفنوع typename G::ڳنڍ_type ڳنڍ_type;

    بنية dfs_داخلا {
        ڳنڍ_type ڳنڍ;
        successor_enumerator جانشين;
        صحيح lowlink;

        dfs_داخلا(ڳنڍ_type ڳنڍ, successor_enumerator جانشين, صحيح lowlink) :
            ڳنڍ(ڳنڍ), جانشين(جانشين), lowlink(lowlink)
        {}
    };

    G &گراف_3;
    ComponentCallback component;

    std::ويڪٽر<dfs_داخلا> dfs_ڍير;
    std::ويڪٽر<ڳنڍ_type> component_ڍير;
    صحيح ايندڙ_اشاريو = 0;

عام:
    TopoSortedSccs(G &گراف_3, ComponentCallback component)
    : گراف_3(گراف_3), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        ڳنڍ_enumerator nodes = گراف_3.enumerate_nodes();
        طالما (!nodes.ختم()) {
            ڳنڍ_type ڳنڍ = nodes.ايندڙ();
            successor_enumerator جانشين = گراف_3.enumerate_جانشين(ڳنڍ);
            إذا (جانشين.ختم())
            {
                گراف_3.dfs_اشاريو(ڳنڍ) = ايندڙ_اشاريو;
                ايندڙ_اشاريو++;
                component_ڍير.ڌڪو_پٺ(ڳنڍ);
                component(component_ڍير.معلومات(), component_ڍير.معلومات() + 1);
                component_ڍير.clear();
                گراف_3.dfs_اشاريو(ڳنڍ) = وڌ_کان_وڌ_عدد;
            }
        }
        أرجع *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        ڳنڍ_enumerator nodes = گراف_3.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        طالما (!nodes.ختم())
            process(nodes.ايندڙ());
        أرجع *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(ڳنڍ_type ڳنڍ) {
        // only start a new search if the node wasn't visited yet
        إذا (گراف_3.dfs_اشاريو(ڳنڍ) >= 0)
            أرجع *this;
        طالما (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            صحيح lowlink = ايندڙ_اشاريو;
            ايندڙ_اشاريو++;
            گراف_3.dfs_اشاريو(ڳنڍ) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_ڍير.ڌڪو_پٺ(ڳنڍ);

            // then we start iterating over the successors of this node
            successor_enumerator جانشين = گراف_3.enumerate_جانشين(ڳنڍ);
            طالما (true) {
                إذا (جانشين.ختم()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    إذا (lowlink == گراف_3.dfs_اشاريو(ڳنڍ)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        صحيح current = Get_سائيز(component_ڍير);
                        افعل {
                            --current;
                        } طالما (component_ڍير[current] != ڳنڍ);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        ڳنڍ_type *ڍير_ptr = component_ڍير.معلومات();
                        ڳنڍ_type *component_begin = ڍير_ptr + current;
                        ڳنڍ_type *component_پڇاڙي = ڍير_ptr + component_ڍير.سائيز();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_پڇاڙي);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        لكل (; component_begin != component_پڇاڙي; ++component_begin)
                            گراف_3.dfs_اشاريو(*component_begin) = وڌ_کان_وڌ_عدد;
                        component_ڍير.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    إذا (dfs_ڍير.خالي_جايون())
                        أرجع *this;
                    خودڪار &dfs_چوٽي = dfs_ڍير.پٺ();

                    ڳنڍ = dfs_چوٽي.ڳنڍ;
                    جانشين = std::move(dfs_چوٽي.جانشين);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_چوٽي.lowlink);
                    dfs_ڍير.ڪڍو_پٺ();
                    // continue checking the remaining successors of the parent node.
                } وإلا {
                    ڳنڍ_type succ = جانشين.ايندڙ();
                    إذا (گراف_3.dfs_اشاريو(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_ڍير.emplace_پٺ(ڳنڍ, std::move(جانشين), lowlink);
                        ڳنڍ = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        اكسر;
                    } وإلا {
                        // the textbook version guards this update with a check
                        // whether the successor is still on the component
                        // stack. If the successor node was already visisted
                        // but is not on the component stack, it must be part
                        // of an already emitted SCC. We can avoid this check
                        // by setting the DFS index of all nodes in a SCC to
                        // INT_MAX when the SCC is emitted.
                        lowlink = min(lowlink, گراف_3.dfs_اشاريو(succ));
                    }
                }
            }
        }
    }
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
