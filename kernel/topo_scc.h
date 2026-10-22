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

#جے_بنا_تعریف TOPO_SCC_H
#تعریف_کرو TOPO_SCC_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class Sig_Cell_گراف {
عام:
    تعريفنوع صحيح گرہ_type;

    بنية successor_enumerator {
        std::سمتیہ<std::pair<صحيح, صحيح>>::const_iterator current, سرا;
        bool مکمل() ثابت { أرجع current == سرا; }
        گرہ_type اگلا() {
            log_assert(!مکمل());
            گرہ_type نتیجہ = current->دوجا;
            ++current;
            أرجع نتیجہ;
        }
    };

    بنية گرہ_enumerator {
        صحيح current, سرا;
        bool مکمل() ثابت { أرجع current == سرا; }
        گرہ_type اگلا() {
            log_assert(!مکمل());
            گرہ_type نتیجہ = current;
            ++current;
            أرجع نتیجہ;
        }
    };

خاص:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::Sig_ثنائی_ہندسہ> sig_ids;
    std::سمتیہ<std::pair<صحيح, صحيح>> کنارے;
    std::سمتیہ<std::pair<صحيح, صحيح>> کنارا_ranges;
    std::سمتیہ<صحيح> indices_;
    صحيح وتھ;
    bool computed = false;

    فراغ compute() {
        وتھ = Get_ناپ(sig_ids);
        کنارا_ranges.clear();
        indices_.clear();
        indices_.resize(Get_ناپ(sig_ids) + Get_ناپ(cell_ids), -1);

        std::ترتیب_دیو(کنارے.begin(), کنارے.سرا());
        خودکار آخری = std::unique(کنارے.begin(), کنارے.سرا());
        کنارے.erase(آخری, کنارے.سرا());
        خودکار کنارا = کنارے.begin();
        خودکار کنارا_سرا = کنارے.سرا();
        صحيح range_begin = 0;
        لكل (صحيح گرہ = -وتھ, گرہ_سرا = Get_ناپ(cell_ids); گرہ != گرہ_سرا; ++گرہ) {
            طالما (کنارا != کنارا_سرا && کنارا->پہلا <= گرہ)
                ++کنارا;
            صحيح range_سرا_2 = کنارا - کنارے.begin();
            کنارا_ranges.emplace_پٹھ(std::make_pair(range_begin, range_سرا_2));
            range_begin = range_سرا_2;
        }
    }

عام:
    گرہ_type گرہ(RTLIL::Cell *cell) { أرجع cell_ids(cell); }
    گرہ_type گرہ(Sig_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ) { أرجع ~sig_ids(ثنائی_ہندسہ); }

    bool is_cell(گرہ_type گرہ) { أرجع گرہ >= 0; }
    bool is_sig(گرہ_type گرہ) { أرجع گرہ < 0; }

    Cell *cell(گرہ_type گرہ) { أرجع گرہ >= 0 ? cell_ids[گرہ] : nullptr; }
    Sig_ثنائی_ہندسہ sig(گرہ_type گرہ) { أرجع گرہ < 0 ? sig_ids[~گرہ] : Sig_ثنائی_ہندسہ(); }

    سانچا<typename Src, typename Dst>
    فراغ جوڑو_کنارا(Src &&src, Dst &&dst) {
        computed = false;
        گرہ_type src_گرہ = گرہ(std::forward<Src>(src));
        گرہ_type dst_گرہ = گرہ(std::forward<Dst>(dst));
        کنارے.emplace_پٹھ(std::make_pair(src_گرہ, dst_گرہ));
    }

    گرہ_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {-Get_ناپ(sig_ids), Get_ناپ(cell_ids)};
    }

    successor_enumerator enumerate_جانشین(گرہ_type ثابت &گرہ) ثابت {
        خودکار range = کنارا_ranges[گرہ + وتھ];
        أرجع {کنارے.begin() + range.پہلا, کنارے.begin() + range.دوجا};
    }

    صحيح &dfs_اشاریہ(گرہ_type ثابت &گرہ) {
        أرجع indices_[گرہ + وتھ];
    }
};


class Int_گراف {
عام:
    تعريفنوع صحيح گرہ_type;

    بنية successor_enumerator {
        std::سمتیہ<std::pair<صحيح, صحيح>>::const_iterator current, سرا;
        bool مکمل() ثابت { أرجع current == سرا; }
        گرہ_type اگلا() {
            log_assert(!مکمل());
            گرہ_type نتیجہ = current->دوجا;
            ++current;
            أرجع نتیجہ;
        }
    };

    بنية گرہ_enumerator {
        صحيح current, سرا;
        bool مکمل() ثابت { أرجع current == سرا; }
        گرہ_type اگلا() {
            log_assert(!مکمل());
            گرہ_type نتیجہ = current;
            ++current;
            أرجع نتیجہ;
        }
    };

خاص:
    std::سمتیہ<std::pair<صحيح, صحيح>> کنارے;
    std::سمتیہ<std::pair<صحيح, صحيح>> کنارا_ranges;
    std::سمتیہ<صحيح> indices_;
    bool computed = false;

    فراغ compute() {
        کنارا_ranges.clear();

        صحيح گرہ_سرا = 0;
        لكل (خودکار ثابت &کنارا : کنارے)
            گرہ_سرا = std::max(گرہ_سرا, std::max(کنارا.پہلا, کنارا.دوجا) + 1);
        indices_.clear();
        indices_.resize(گرہ_سرا, -1);

        std::ترتیب_دیو(کنارے.begin(), کنارے.سرا());
        خودکار آخری = std::unique(کنارے.begin(), کنارے.سرا());
        کنارے.erase(آخری, کنارے.سرا());
        خودکار کنارا = کنارے.begin();
        خودکار کنارا_سرا = کنارے.سرا();
        صحيح range_begin = 0;
        لكل (صحيح گرہ = 0; گرہ != گرہ_سرا; ++گرہ) {
            طالما (کنارا != کنارا_سرا && کنارا->پہلا <= گرہ)
                ++کنارا;
            صحيح range_سرا_2 = کنارا - کنارے.begin();
            کنارا_ranges.emplace_پٹھ(std::make_pair(range_begin, range_سرا_2));
            range_begin = range_سرا_2;
        }
    }

عام:
    فراغ جوڑو_کنارا(صحيح src, صحيح dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        کنارے.emplace_پٹھ(std::make_pair(src, dst));
    }

    گرہ_enumerator enumerate_nodes() {
        إذا (!computed) compute();
        أرجع {0, Get_ناپ(indices_)};
    }

    successor_enumerator enumerate_جانشین(صحيح گرہ) ثابت {
        خودکار range = کنارا_ranges[گرہ];
        أرجع {کنارے.begin() + range.پہلا, کنارے.begin() + range.دوجا};
    }

    صحيح &dfs_اشاریہ(گرہ_type ثابت &گرہ) {
        أرجع indices_[گرہ];
    }
};

سانچا<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    تعريفنوع typename G::گرہ_enumerator گرہ_enumerator;
    تعريفنوع typename G::successor_enumerator successor_enumerator;
    تعريفنوع typename G::گرہ_type گرہ_type;

    بنية dfs_اندراج {
        گرہ_type گرہ;
        successor_enumerator جانشین;
        صحيح lowlink;

        dfs_اندراج(گرہ_type گرہ, successor_enumerator جانشین, صحيح lowlink) :
            گرہ(گرہ), جانشین(جانشین), lowlink(lowlink)
        {}
    };

    G &گراف_3;
    ComponentCallback component;

    std::سمتیہ<dfs_اندراج> dfs_ڈھیر;
    std::سمتیہ<گرہ_type> component_ڈھیر;
    صحيح اگلا_اشاریہ = 0;

عام:
    TopoSortedSccs(G &گراف_3, ComponentCallback component)
    : گراف_3(گراف_3), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        گرہ_enumerator nodes = گراف_3.enumerate_nodes();
        طالما (!nodes.مکمل()) {
            گرہ_type گرہ = nodes.اگلا();
            successor_enumerator جانشین = گراف_3.enumerate_جانشین(گرہ);
            إذا (جانشین.مکمل())
            {
                گراف_3.dfs_اشاریہ(گرہ) = اگلا_اشاریہ;
                اگلا_اشاریہ++;
                component_ڈھیر.ڈالو_پٹھ(گرہ);
                component(component_ڈھیر.انکڑے(), component_ڈھیر.انکڑے() + 1);
                component_ڈھیر.clear();
                گراف_3.dfs_اشاریہ(گرہ) = ودھ_توں_ودھ_عدد;
            }
        }
        أرجع *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        گرہ_enumerator nodes = گراف_3.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        طالما (!nodes.مکمل())
            process(nodes.اگلا());
        أرجع *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(گرہ_type گرہ) {
        // only start a new search if the node wasn't visited yet
        إذا (گراف_3.dfs_اشاریہ(گرہ) >= 0)
            أرجع *this;
        طالما (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            صحيح lowlink = اگلا_اشاریہ;
            اگلا_اشاریہ++;
            گراف_3.dfs_اشاریہ(گرہ) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_ڈھیر.ڈالو_پٹھ(گرہ);

            // then we start iterating over the successors of this node
            successor_enumerator جانشین = گراف_3.enumerate_جانشین(گرہ);
            طالما (true) {
                إذا (جانشین.مکمل()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    إذا (lowlink == گراف_3.dfs_اشاریہ(گرہ)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        صحيح current = Get_ناپ(component_ڈھیر);
                        افعل {
                            --current;
                        } طالما (component_ڈھیر[current] != گرہ);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        گرہ_type *ڈھیر_ptr = component_ڈھیر.انکڑے();
                        گرہ_type *component_begin = ڈھیر_ptr + current;
                        گرہ_type *component_سرا = ڈھیر_ptr + component_ڈھیر.ناپ();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_سرا);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        لكل (; component_begin != component_سرا; ++component_begin)
                            گراف_3.dfs_اشاریہ(*component_begin) = ودھ_توں_ودھ_عدد;
                        component_ڈھیر.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    إذا (dfs_ڈھیر.خالی_تھاواں())
                        أرجع *this;
                    خودکار &dfs_چوٹی = dfs_ڈھیر.پٹھ();

                    گرہ = dfs_چوٹی.گرہ;
                    جانشین = std::move(dfs_چوٹی.جانشین);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_چوٹی.lowlink);
                    dfs_ڈھیر.نکالو_پٹھ();
                    // continue checking the remaining successors of the parent node.
                } وإلا {
                    گرہ_type succ = جانشین.اگلا();
                    إذا (گراف_3.dfs_اشاریہ(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_ڈھیر.emplace_پٹھ(گرہ, std::move(جانشین), lowlink);
                        گرہ = succ;

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
                        lowlink = min(lowlink, گراف_3.dfs_اشاریہ(succ));
                    }
                }
            }
        }
    }
};

YOSYS_NAMESPACE_سرا

#جے_انت
