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

#که_نه_تعریف_شوی TOPO_SCC_H
#تعریف_کړه TOPO_SCC_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class Sig_Cell_ګراف {
عام:
    تعريفنوع صحيح غوټه_type;

    بنية successor_enumerator {
        std::ویکټور<std::pair<صحيح, صحيح>>::const_iterator current, پای_ټکی;
        bool پای_ته_رسېدلی() ثابت { أرجع current == پای_ټکی; }
        غوټه_type بل() {
            log_assert(!پای_ته_رسېدلی());
            غوټه_type پایله = current->دوهم;
            ++current;
            أرجع پایله;
        }
    };

    بنية غوټه_enumerator {
        صحيح current, پای_ټکی;
        bool پای_ته_رسېدلی() ثابت { أرجع current == پای_ټکی; }
        غوټه_type بل() {
            log_assert(!پای_ته_رسېدلی());
            غوټه_type پایله = current;
            ++current;
            أرجع پایله;
        }
    };

خاص:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::Sig_بټ> sig_ids;
    std::ویکټور<std::pair<صحيح, صحيح>> څنډې;
    std::ویکټور<std::pair<صحيح, صحيح>> څنډه_ranges;
    std::ویکټور<صحيح> indices_;
    صحيح دننه_واټن;
    bool computed = false;

    فراغ compute() {
        دننه_واټن = Get_اندازه(sig_ids);
        څنډه_ranges.clear();
        indices_.clear();
        indices_.resize(Get_اندازه(sig_ids) + Get_اندازه(cell_ids), -1);

        std::ترتیبول(څنډې.begin(), څنډې.پای_ټکی());
        خپلکاری وروستی = std::unique(څنډې.begin(), څنډې.پای_ټکی());
        څنډې.erase(وروستی, څنډې.پای_ټکی());
        خپلکاری څنډه = څنډې.begin();
        خپلکاری څنډه_پای_ټکی = څنډې.پای_ټکی();
        صحيح range_begin = 0;
        لكل (صحيح غوټه = -دننه_واټن, غوټه_پای_ټکی = Get_اندازه(cell_ids); غوټه != غوټه_پای_ټکی; ++غوټه) {
            طالما (څنډه != څنډه_پای_ټکی && څنډه->لومړی <= غوټه)
                ++څنډه;
            صحيح range_پای_ټکی_2 = څنډه - څنډې.begin();
            څنډه_ranges.emplace_شا(std::make_pair(range_begin, range_پای_ټکی_2));
            range_begin = range_پای_ټکی_2;
        }
    }

عام:
    غوټه_type غوټه(RTLIL::Cell *cell) { أرجع cell_ids(cell); }
    غوټه_type غوټه(Sig_بټ ثابت &بټ) { أرجع ~sig_ids(بټ); }

    bool is_cell(غوټه_type غوټه) { أرجع غوټه >= 0; }
    bool is_sig(غوټه_type غوټه) { أرجع غوټه < 0; }

    Cell *cell(غوټه_type غوټه) { أرجع غوټه >= 0 ? cell_ids[غوټه] : nullptr; }
    Sig_بټ sig(غوټه_type غوټه) { أرجع غوټه < 0 ? sig_ids[~غوټه] : Sig_بټ(); }

    کينډۍ<typename Src, typename Dst>
    فراغ جمع_کړه_څنډه(Src &&src, Dst &&dst) {
        computed = false;
        غوټه_type src_غوټه = غوټه(std::forward<Src>(src));
        غوټه_type dst_غوټه = غوټه(std::forward<Dst>(dst));
        څنډې.emplace_شا(std::make_pair(src_غوټه, dst_غوټه));
    }

    غوټه_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {-Get_اندازه(sig_ids), Get_اندازه(cell_ids)};
    }

    successor_enumerator enumerate_ځایناستي(غوټه_type ثابت &غوټه) ثابت {
        خپلکاری range = څنډه_ranges[غوټه + دننه_واټن];
        أرجع {څنډې.begin() + range.لومړی, څنډې.begin() + range.دوهم};
    }

    صحيح &dfs_شاخص(غوټه_type ثابت &غوټه) {
        أرجع indices_[غوټه + دننه_واټن];
    }
};


class Int_ګراف {
عام:
    تعريفنوع صحيح غوټه_type;

    بنية successor_enumerator {
        std::ویکټور<std::pair<صحيح, صحيح>>::const_iterator current, پای_ټکی;
        bool پای_ته_رسېدلی() ثابت { أرجع current == پای_ټکی; }
        غوټه_type بل() {
            log_assert(!پای_ته_رسېدلی());
            غوټه_type پایله = current->دوهم;
            ++current;
            أرجع پایله;
        }
    };

    بنية غوټه_enumerator {
        صحيح current, پای_ټکی;
        bool پای_ته_رسېدلی() ثابت { أرجع current == پای_ټکی; }
        غوټه_type بل() {
            log_assert(!پای_ته_رسېدلی());
            غوټه_type پایله = current;
            ++current;
            أرجع پایله;
        }
    };

خاص:
    std::ویکټور<std::pair<صحيح, صحيح>> څنډې;
    std::ویکټور<std::pair<صحيح, صحيح>> څنډه_ranges;
    std::ویکټور<صحيح> indices_;
    bool computed = false;

    فراغ compute() {
        څنډه_ranges.clear();

        صحيح غوټه_پای_ټکی = 0;
        لكل (خپلکاری ثابت &څنډه : څنډې)
            غوټه_پای_ټکی = std::max(غوټه_پای_ټکی, std::max(څنډه.لومړی, څنډه.دوهم) + 1);
        indices_.clear();
        indices_.resize(غوټه_پای_ټکی, -1);

        std::ترتیبول(څنډې.begin(), څنډې.پای_ټکی());
        خپلکاری وروستی = std::unique(څنډې.begin(), څنډې.پای_ټکی());
        څنډې.erase(وروستی, څنډې.پای_ټکی());
        خپلکاری څنډه = څنډې.begin();
        خپلکاری څنډه_پای_ټکی = څنډې.پای_ټکی();
        صحيح range_begin = 0;
        لكل (صحيح غوټه = 0; غوټه != غوټه_پای_ټکی; ++غوټه) {
            طالما (څنډه != څنډه_پای_ټکی && څنډه->لومړی <= غوټه)
                ++څنډه;
            صحيح range_پای_ټکی_2 = څنډه - څنډې.begin();
            څنډه_ranges.emplace_شا(std::make_pair(range_begin, range_پای_ټکی_2));
            range_begin = range_پای_ټکی_2;
        }
    }

عام:
    فراغ جمع_کړه_څنډه(صحيح src, صحيح dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        څنډې.emplace_شا(std::make_pair(src, dst));
    }

    غوټه_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {0, Get_اندازه(indices_)};
    }

    successor_enumerator enumerate_ځایناستي(صحيح غوټه) ثابت {
        خپلکاری range = څنډه_ranges[غوټه];
        أرجع {څنډې.begin() + range.لومړی, څنډې.begin() + range.دوهم};
    }

    صحيح &dfs_شاخص(غوټه_type ثابت &غوټه) {
        أرجع indices_[غوټه];
    }
};

کينډۍ<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    تعريفنوع typename G::غوټه_enumerator غوټه_enumerator;
    تعريفنوع typename G::successor_enumerator successor_enumerator;
    تعريفنوع typename G::غوټه_type غوټه_type;

    بنية dfs_ثبت {
        غوټه_type غوټه;
        successor_enumerator ځایناستي;
        صحيح lowlink;

        dfs_ثبت(غوټه_type غوټه, successor_enumerator ځایناستي, صحيح lowlink) :
            غوټه(غوټه), ځایناستي(ځایناستي), lowlink(lowlink)
        {}
    };

    G &ګراف_3;
    ComponentCallback component;

    std::ویکټور<dfs_ثبت> dfs_ډېری;
    std::ویکټور<غوټه_type> component_ډېری;
    صحيح بل_شاخص = 0;

عام:
    TopoSortedSccs(G &ګراف_3, ComponentCallback component)
    : ګراف_3(ګراف_3), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        غوټه_enumerator nodes = ګراف_3.enumerate_nodes();
        طالما (!nodes.پای_ته_رسېدلی()) {
            غوټه_type غوټه = nodes.بل();
            successor_enumerator ځایناستي = ګراف_3.enumerate_ځایناستي(غوټه);
            إذا (ځایناستي.پای_ته_رسېدلی())
            {
                ګراف_3.dfs_شاخص(غوټه) = بل_شاخص;
                بل_شاخص++;
                component_ډېری.ټېل_شا(غوټه);
                component(component_ډېری.معلومات(), component_ډېری.معلومات() + 1);
                component_ډېری.clear();
                ګراف_3.dfs_شاخص(غوټه) = لوی_عدد;
            }
        }
        أرجع *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        غوټه_enumerator nodes = ګراف_3.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        طالما (!nodes.پای_ته_رسېدلی())
            process(nodes.بل());
        أرجع *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(غوټه_type غوټه) {
        // only start a new search if the node wasn't visited yet
        إذا (ګراف_3.dfs_شاخص(غوټه) >= 0)
            أرجع *this;
        طالما (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            صحيح lowlink = بل_شاخص;
            بل_شاخص++;
            ګراف_3.dfs_شاخص(غوټه) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_ډېری.ټېل_شا(غوټه);

            // then we start iterating over the successors of this node
            successor_enumerator ځایناستي = ګراف_3.enumerate_ځایناستي(غوټه);
            طالما (true) {
                إذا (ځایناستي.پای_ته_رسېدلی()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    إذا (lowlink == ګراف_3.dfs_شاخص(غوټه)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        صحيح current = Get_اندازه(component_ډېری);
                        افعل {
                            --current;
                        } طالما (component_ډېری[current] != غوټه);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        غوټه_type *ډېری_ptr = component_ډېری.معلومات();
                        غوټه_type *component_begin = ډېری_ptr + current;
                        غوټه_type *component_پای_ټکی = ډېری_ptr + component_ډېری.اندازه();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_پای_ټکی);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        لكل (; component_begin != component_پای_ټکی; ++component_begin)
                            ګراف_3.dfs_شاخص(*component_begin) = لوی_عدد;
                        component_ډېری.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    إذا (dfs_ډېری.تشه())
                        أرجع *this;
                    خپلکاری &dfs_سر = dfs_ډېری.شا();

                    غوټه = dfs_سر.غوټه;
                    ځایناستي = std::move(dfs_سر.ځایناستي);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_سر.lowlink);
                    dfs_ډېری.راوباسه_شا();
                    // continue checking the remaining successors of the parent node.
                } وإلا {
                    غوټه_type succ = ځایناستي.بل();
                    إذا (ګراف_3.dfs_شاخص(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_ډېری.emplace_شا(غوټه, std::move(ځایناستي), lowlink);
                        غوټه = succ;

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
                        lowlink = min(lowlink, ګراف_3.dfs_شاخص(succ));
                    }
                }
            }
        }
    }
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
