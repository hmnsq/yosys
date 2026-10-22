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

#اگر_غیر_تعریف_شدہ TOPO_SCC_H
#تعریف_کرو TOPO_SCC_H

#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class Sig_Cell_گراف {
عوامی:
    قسمتعریف صحیح گرہ_type;

    ساخت successor_enumerator {
        std::سمتیہ<std::pair<صحیح, صحیح>>::const_iterator current, انتہا;
        bool ختم_شدہ() ثابت { واپس current == انتہا; }
        گرہ_type اگلا() {
            log_assert(!ختم_شدہ());
            گرہ_type نتیجہ = current->دوسرا;
            ++current;
            واپس نتیجہ;
        }
    };

    ساخت گرہ_enumerator {
        صحیح current, انتہا;
        bool ختم_شدہ() ثابت { واپس current == انتہا; }
        گرہ_type اگلا() {
            log_assert(!ختم_شدہ());
            گرہ_type نتیجہ = current;
            ++current;
            واپس نتیجہ;
        }
    };

نجی:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::Sig_ثنائی_ہندسہ> sig_ids;
    std::سمتیہ<std::pair<صحیح, صحیح>> کنارے;
    std::سمتیہ<std::pair<صحیح, صحیح>> کنارہ_ranges;
    std::سمتیہ<صحیح> indices_;
    صحیح انحراف;
    bool computed = false;

    خالی compute() {
        انحراف = Get_سائز(sig_ids);
        کنارہ_ranges.clear();
        indices_.clear();
        indices_.resize(Get_سائز(sig_ids) + Get_سائز(cell_ids), -1);

        std::ترتیب_دو(کنارے.begin(), کنارے.انتہا());
        خودکار آخری = std::unique(کنارے.begin(), کنارے.انتہا());
        کنارے.erase(آخری, کنارے.انتہا());
        خودکار کنارہ = کنارے.begin();
        خودکار کنارہ_انتہا = کنارے.انتہا();
        صحیح range_begin = 0;
        برائے (صحیح گرہ = -انحراف, گرہ_انتہا = Get_سائز(cell_ids); گرہ != گرہ_انتہا; ++گرہ) {
            جب (کنارہ != کنارہ_انتہا && کنارہ->پہلا <= گرہ)
                ++کنارہ;
            صحیح range_انتہا_2 = کنارہ - کنارے.begin();
            کنارہ_ranges.emplace_عقب(std::make_pair(range_begin, range_انتہا_2));
            range_begin = range_انتہا_2;
        }
    }

عوامی:
    گرہ_type گرہ(RTLIL::Cell *cell) { واپس cell_ids(cell); }
    گرہ_type گرہ(Sig_ثنائی_ہندسہ ثابت &ثنائی_ہندسہ) { واپس ~sig_ids(ثنائی_ہندسہ); }

    bool is_cell(گرہ_type گرہ) { واپس گرہ >= 0; }
    bool is_sig(گرہ_type گرہ) { واپس گرہ < 0; }

    Cell *cell(گرہ_type گرہ) { واپس گرہ >= 0 ? cell_ids[گرہ] : nullptr; }
    Sig_ثنائی_ہندسہ sig(گرہ_type گرہ) { واپس گرہ < 0 ? sig_ids[~گرہ] : Sig_ثنائی_ہندسہ(); }

    سانچہ<typename Src, typename Dst>
    خالی جوڑو_کنارہ(Src &&src, Dst &&dst) {
        computed = false;
        گرہ_type src_گرہ = گرہ(std::forward<Src>(src));
        گرہ_type dst_گرہ = گرہ(std::forward<Dst>(dst));
        کنارے.emplace_عقب(std::make_pair(src_گرہ, dst_گرہ));
    }

    گرہ_enumerator enumerate_nodes() {
        اگر (!computed) compute();
        واپس {-Get_سائز(sig_ids), Get_سائز(cell_ids)};
    }

    successor_enumerator enumerate_جانشین(گرہ_type ثابت &گرہ) ثابت {
        خودکار range = کنارہ_ranges[گرہ + انحراف];
        واپس {کنارے.begin() + range.پہلا, کنارے.begin() + range.دوسرا};
    }

    صحیح &dfs_اشاریہ(گرہ_type ثابت &گرہ) {
        واپس indices_[گرہ + انحراف];
    }
};


class Int_گراف {
عوامی:
    قسمتعریف صحیح گرہ_type;

    ساخت successor_enumerator {
        std::سمتیہ<std::pair<صحیح, صحیح>>::const_iterator current, انتہا;
        bool ختم_شدہ() ثابت { واپس current == انتہا; }
        گرہ_type اگلا() {
            log_assert(!ختم_شدہ());
            گرہ_type نتیجہ = current->دوسرا;
            ++current;
            واپس نتیجہ;
        }
    };

    ساخت گرہ_enumerator {
        صحیح current, انتہا;
        bool ختم_شدہ() ثابت { واپس current == انتہا; }
        گرہ_type اگلا() {
            log_assert(!ختم_شدہ());
            گرہ_type نتیجہ = current;
            ++current;
            واپس نتیجہ;
        }
    };

نجی:
    std::سمتیہ<std::pair<صحیح, صحیح>> کنارے;
    std::سمتیہ<std::pair<صحیح, صحیح>> کنارہ_ranges;
    std::سمتیہ<صحیح> indices_;
    bool computed = false;

    خالی compute() {
        کنارہ_ranges.clear();

        صحیح گرہ_انتہا = 0;
        برائے (خودکار ثابت &کنارہ : کنارے)
            گرہ_انتہا = std::max(گرہ_انتہا, std::max(کنارہ.پہلا, کنارہ.دوسرا) + 1);
        indices_.clear();
        indices_.resize(گرہ_انتہا, -1);

        std::ترتیب_دو(کنارے.begin(), کنارے.انتہا());
        خودکار آخری = std::unique(کنارے.begin(), کنارے.انتہا());
        کنارے.erase(آخری, کنارے.انتہا());
        خودکار کنارہ = کنارے.begin();
        خودکار کنارہ_انتہا = کنارے.انتہا();
        صحیح range_begin = 0;
        برائے (صحیح گرہ = 0; گرہ != گرہ_انتہا; ++گرہ) {
            جب (کنارہ != کنارہ_انتہا && کنارہ->پہلا <= گرہ)
                ++کنارہ;
            صحیح range_انتہا_2 = کنارہ - کنارے.begin();
            کنارہ_ranges.emplace_عقب(std::make_pair(range_begin, range_انتہا_2));
            range_begin = range_انتہا_2;
        }
    }

عوامی:
    خالی جوڑو_کنارہ(صحیح src, صحیح dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        کنارے.emplace_عقب(std::make_pair(src, dst));
    }

    گرہ_enumerator enumerate_nodes() {
        اگر (!computed) compute();
        واپس {0, Get_سائز(indices_)};
    }

    successor_enumerator enumerate_جانشین(صحیح گرہ) ثابت {
        خودکار range = کنارہ_ranges[گرہ];
        واپس {کنارے.begin() + range.پہلا, کنارے.begin() + range.دوسرا};
    }

    صحیح &dfs_اشاریہ(گرہ_type ثابت &گرہ) {
        واپس indices_[گرہ];
    }
};

سانچہ<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    قسمتعریف typename G::گرہ_enumerator گرہ_enumerator;
    قسمتعریف typename G::successor_enumerator successor_enumerator;
    قسمتعریف typename G::گرہ_type گرہ_type;

    ساخت dfs_اندراج {
        گرہ_type گرہ;
        successor_enumerator جانشین;
        صحیح lowlink;

        dfs_اندراج(گرہ_type گرہ, successor_enumerator جانشین, صحیح lowlink) :
            گرہ(گرہ), جانشین(جانشین), lowlink(lowlink)
        {}
    };

    G &گراف_3;
    ComponentCallback component;

    std::سمتیہ<dfs_اندراج> dfs_ڈھیر;
    std::سمتیہ<گرہ_type> component_ڈھیر;
    صحیح اگلا_اشاریہ = 0;

عوامی:
    TopoSortedSccs(G &گراف_3, ComponentCallback component)
    : گراف_3(گراف_3), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        گرہ_enumerator nodes = گراف_3.enumerate_nodes();
        جب (!nodes.ختم_شدہ()) {
            گرہ_type گرہ = nodes.اگلا();
            successor_enumerator جانشین = گراف_3.enumerate_جانشین(گرہ);
            اگر (جانشین.ختم_شدہ())
            {
                گراف_3.dfs_اشاریہ(گرہ) = اگلا_اشاریہ;
                اگلا_اشاریہ++;
                component_ڈھیر.ڈالو_عقب(گرہ);
                component(component_ڈھیر.ڈیٹا(), component_ڈھیر.ڈیٹا() + 1);
                component_ڈھیر.clear();
                گراف_3.dfs_اشاریہ(گرہ) = زیادہ_سے_زیادہ_عدد;
            }
        }
        واپس *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        گرہ_enumerator nodes = گراف_3.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        جب (!nodes.ختم_شدہ())
            process(nodes.اگلا());
        واپس *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(گرہ_type گرہ) {
        // only start a new search if the node wasn't visited yet
        اگر (گراف_3.dfs_اشاریہ(گرہ) >= 0)
            واپس *this;
        جب (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            صحیح lowlink = اگلا_اشاریہ;
            اگلا_اشاریہ++;
            گراف_3.dfs_اشاریہ(گرہ) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_ڈھیر.ڈالو_عقب(گرہ);

            // then we start iterating over the successors of this node
            successor_enumerator جانشین = گراف_3.enumerate_جانشین(گرہ);
            جب (true) {
                اگر (جانشین.ختم_شدہ()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    اگر (lowlink == گراف_3.dfs_اشاریہ(گرہ)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        صحیح current = Get_سائز(component_ڈھیر);
                        کرو {
                            --current;
                        } جب (component_ڈھیر[current] != گرہ);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        گرہ_type *ڈھیر_ptr = component_ڈھیر.ڈیٹا();
                        گرہ_type *component_begin = ڈھیر_ptr + current;
                        گرہ_type *component_انتہا = ڈھیر_ptr + component_ڈھیر.سائز();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_انتہا);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        برائے (; component_begin != component_انتہا; ++component_begin)
                            گراف_3.dfs_اشاریہ(*component_begin) = زیادہ_سے_زیادہ_عدد;
                        component_ڈھیر.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    اگر (dfs_ڈھیر.خالی_خانے())
                        واپس *this;
                    خودکار &dfs_چوٹی = dfs_ڈھیر.عقب();

                    گرہ = dfs_چوٹی.گرہ;
                    جانشین = std::move(dfs_چوٹی.جانشین);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_چوٹی.lowlink);
                    dfs_ڈھیر.نکالو_عقب();
                    // continue checking the remaining successors of the parent node.
                } ورنہ {
                    گرہ_type succ = جانشین.اگلا();
                    اگر (گراف_3.dfs_اشاریہ(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_ڈھیر.emplace_عقب(گرہ, std::move(جانشین), lowlink);
                        گرہ = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        توڑو;
                    } ورنہ {
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

YOSYS_NAMESPACE_انتہا

#اگر_ختم
