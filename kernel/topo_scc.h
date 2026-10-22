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

#اگر_تعریف_نشده TOPO_SCC_H
#تعریف_کن TOPO_SCC_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class Sig_Cell_گراف {
عام:
    تعريفنوع صحيح گره_type;

    بنية successor_enumerator {
        std::وکتور<std::pair<صحيح, صحيح>>::const_iterator current, انتها;
        bool پایانیافته() ثابت { أرجع current == انتها; }
        گره_type بعدی() {
            log_assert(!پایانیافته());
            گره_type نتیجه = current->دومی;
            ++current;
            أرجع نتیجه;
        }
    };

    بنية گره_enumerator {
        صحيح current, انتها;
        bool پایانیافته() ثابت { أرجع current == انتها; }
        گره_type بعدی() {
            log_assert(!پایانیافته());
            گره_type نتیجه = current;
            ++current;
            أرجع نتیجه;
        }
    };

خاص:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::Sig_بیت> sig_ids;
    std::وکتور<std::pair<صحيح, صحيح>> یالها;
    std::وکتور<std::pair<صحيح, صحيح>> یال_ranges;
    std::وکتور<صحيح> indices_;
    صحيح جابه_جایی;
    bool computed = false;

    فراغ compute() {
        جابه_جایی = Get_اندازه(sig_ids);
        یال_ranges.clear();
        indices_.clear();
        indices_.resize(Get_اندازه(sig_ids) + Get_اندازه(cell_ids), -1);

        std::مرتب_سازی(یالها.begin(), یالها.انتها());
        خودکار آخرین = std::unique(یالها.begin(), یالها.انتها());
        یالها.erase(آخرین, یالها.انتها());
        خودکار یال = یالها.begin();
        خودکار یال_انتها = یالها.انتها();
        صحيح range_begin = 0;
        لكل (صحيح گره = -جابه_جایی, گره_انتها = Get_اندازه(cell_ids); گره != گره_انتها; ++گره) {
            طالما (یال != یال_انتها && یال->اولی <= گره)
                ++یال;
            صحيح range_انتها_2 = یال - یالها.begin();
            یال_ranges.emplace_پشت(std::make_pair(range_begin, range_انتها_2));
            range_begin = range_انتها_2;
        }
    }

عام:
    گره_type گره(RTLIL::Cell *cell) { أرجع cell_ids(cell); }
    گره_type گره(Sig_بیت ثابت &بیت) { أرجع ~sig_ids(بیت); }

    bool is_cell(گره_type گره) { أرجع گره >= 0; }
    bool is_sig(گره_type گره) { أرجع گره < 0; }

    Cell *cell(گره_type گره) { أرجع گره >= 0 ? cell_ids[گره] : nullptr; }
    Sig_بیت sig(گره_type گره) { أرجع گره < 0 ? sig_ids[~گره] : Sig_بیت(); }

    قالب<typename Src, typename Dst>
    فراغ جمع_کن_یال(Src &&src, Dst &&dst) {
        computed = false;
        گره_type src_گره = گره(std::forward<Src>(src));
        گره_type dst_گره = گره(std::forward<Dst>(dst));
        یالها.emplace_پشت(std::make_pair(src_گره, dst_گره));
    }

    گره_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {-Get_اندازه(sig_ids), Get_اندازه(cell_ids)};
    }

    successor_enumerator enumerate_جانشینها(گره_type ثابت &گره) ثابت {
        خودکار range = یال_ranges[گره + جابه_جایی];
        أرجع {یالها.begin() + range.اولی, یالها.begin() + range.دومی};
    }

    صحيح &dfs_شاخص(گره_type ثابت &گره) {
        أرجع indices_[گره + جابه_جایی];
    }
};


class Int_گراف {
عام:
    تعريفنوع صحيح گره_type;

    بنية successor_enumerator {
        std::وکتور<std::pair<صحيح, صحيح>>::const_iterator current, انتها;
        bool پایانیافته() ثابت { أرجع current == انتها; }
        گره_type بعدی() {
            log_assert(!پایانیافته());
            گره_type نتیجه = current->دومی;
            ++current;
            أرجع نتیجه;
        }
    };

    بنية گره_enumerator {
        صحيح current, انتها;
        bool پایانیافته() ثابت { أرجع current == انتها; }
        گره_type بعدی() {
            log_assert(!پایانیافته());
            گره_type نتیجه = current;
            ++current;
            أرجع نتیجه;
        }
    };

خاص:
    std::وکتور<std::pair<صحيح, صحيح>> یالها;
    std::وکتور<std::pair<صحيح, صحيح>> یال_ranges;
    std::وکتور<صحيح> indices_;
    bool computed = false;

    فراغ compute() {
        یال_ranges.clear();

        صحيح گره_انتها = 0;
        لكل (خودکار ثابت &یال : یالها)
            گره_انتها = std::max(گره_انتها, std::max(یال.اولی, یال.دومی) + 1);
        indices_.clear();
        indices_.resize(گره_انتها, -1);

        std::مرتب_سازی(یالها.begin(), یالها.انتها());
        خودکار آخرین = std::unique(یالها.begin(), یالها.انتها());
        یالها.erase(آخرین, یالها.انتها());
        خودکار یال = یالها.begin();
        خودکار یال_انتها = یالها.انتها();
        صحيح range_begin = 0;
        لكل (صحيح گره = 0; گره != گره_انتها; ++گره) {
            طالما (یال != یال_انتها && یال->اولی <= گره)
                ++یال;
            صحيح range_انتها_2 = یال - یالها.begin();
            یال_ranges.emplace_پشت(std::make_pair(range_begin, range_انتها_2));
            range_begin = range_انتها_2;
        }
    }

عام:
    فراغ جمع_کن_یال(صحيح src, صحيح dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        یالها.emplace_پشت(std::make_pair(src, dst));
    }

    گره_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {0, Get_اندازه(indices_)};
    }

    successor_enumerator enumerate_جانشینها(صحيح گره) ثابت {
        خودکار range = یال_ranges[گره];
        أرجع {یالها.begin() + range.اولی, یالها.begin() + range.دومی};
    }

    صحيح &dfs_شاخص(گره_type ثابت &گره) {
        أرجع indices_[گره];
    }
};

قالب<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    تعريفنوع typename G::گره_enumerator گره_enumerator;
    تعريفنوع typename G::successor_enumerator successor_enumerator;
    تعريفنوع typename G::گره_type گره_type;

    بنية dfs_مدخل {
        گره_type گره;
        successor_enumerator جانشینها;
        صحيح lowlink;

        dfs_مدخل(گره_type گره, successor_enumerator جانشینها, صحيح lowlink) :
            گره(گره), جانشینها(جانشینها), lowlink(lowlink)
        {}
    };

    G &گراف_3;
    ComponentCallback component;

    std::وکتور<dfs_مدخل> dfs_پشته;
    std::وکتور<گره_type> component_پشته;
    صحيح بعدی_شاخص = 0;

عام:
    TopoSortedSccs(G &گراف_3, ComponentCallback component)
    : گراف_3(گراف_3), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        گره_enumerator nodes = گراف_3.enumerate_nodes();
        طالما (!nodes.پایانیافته()) {
            گره_type گره = nodes.بعدی();
            successor_enumerator جانشینها = گراف_3.enumerate_جانشینها(گره);
            إذا (جانشینها.پایانیافته())
            {
                گراف_3.dfs_شاخص(گره) = بعدی_شاخص;
                بعدی_شاخص++;
                component_پشته.افزودن_پشت(گره);
                component(component_پشته.داده(), component_پشته.داده() + 1);
                component_پشته.clear();
                گراف_3.dfs_شاخص(گره) = بیشترین_صحیح;
            }
        }
        أرجع *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        گره_enumerator nodes = گراف_3.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        طالما (!nodes.پایانیافته())
            process(nodes.بعدی());
        أرجع *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(گره_type گره) {
        // only start a new search if the node wasn't visited yet
        إذا (گراف_3.dfs_شاخص(گره) >= 0)
            أرجع *this;
        طالما (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            صحيح lowlink = بعدی_شاخص;
            بعدی_شاخص++;
            گراف_3.dfs_شاخص(گره) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_پشته.افزودن_پشت(گره);

            // then we start iterating over the successors of this node
            successor_enumerator جانشینها = گراف_3.enumerate_جانشینها(گره);
            طالما (true) {
                إذا (جانشینها.پایانیافته()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    إذا (lowlink == گراف_3.dfs_شاخص(گره)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        صحيح current = Get_اندازه(component_پشته);
                        افعل {
                            --current;
                        } طالما (component_پشته[current] != گره);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        گره_type *پشته_ptr = component_پشته.داده();
                        گره_type *component_begin = پشته_ptr + current;
                        گره_type *component_انتها = پشته_ptr + component_پشته.اندازه();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_انتها);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        لكل (; component_begin != component_انتها; ++component_begin)
                            گراف_3.dfs_شاخص(*component_begin) = بیشترین_صحیح;
                        component_پشته.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    إذا (dfs_پشته.خالی())
                        أرجع *this;
                    خودکار &dfs_بالاترین = dfs_پشته.پشت();

                    گره = dfs_بالاترین.گره;
                    جانشینها = std::move(dfs_بالاترین.جانشینها);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_بالاترین.lowlink);
                    dfs_پشته.برداشت_پشت();
                    // continue checking the remaining successors of the parent node.
                } وإلا {
                    گره_type succ = جانشینها.بعدی();
                    إذا (گراف_3.dfs_شاخص(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_پشته.emplace_پشت(گره, std::move(جانشینها), lowlink);
                        گره = succ;

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
                        lowlink = min(lowlink, گراف_3.dfs_شاخص(succ));
                    }
                }
            }
        }
    }
};

YOSYS_NAMESPACE_انتها

#پایان_اگر
