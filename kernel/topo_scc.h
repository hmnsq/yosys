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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TOPO_SCC_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TOPO_SCC_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class Sig_Cell_ਗ੍ਰਾਫ {
ਜਨਤਕ:
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ ਗੰਢ_type;

    ਬਣਤਰ successor_enumerator {
        std::ਸਦਿਸ਼<std::pair<ਪੂਰਨ, ਪੂਰਨ>>::const_iterator current, ਸਿਰਾ;
        bool ਮੁਕੰਮਲ() ਅਚਲ { ਵਾਪਸ current == ਸਿਰਾ; }
        ਗੰਢ_type ਅਗਲਾ() {
            log_assert(!ਮੁਕੰਮਲ());
            ਗੰਢ_type ਨਤੀਜਾ = current->ਦੂਜਾ;
            ++current;
            ਵਾਪਸ ਨਤੀਜਾ;
        }
    };

    ਬਣਤਰ ਗੰਢ_enumerator {
        ਪੂਰਨ current, ਸਿਰਾ;
        bool ਮੁਕੰਮਲ() ਅਚਲ { ਵਾਪਸ current == ਸਿਰਾ; }
        ਗੰਢ_type ਅਗਲਾ() {
            log_assert(!ਮੁਕੰਮਲ());
            ਗੰਢ_type ਨਤੀਜਾ = current;
            ++current;
            ਵਾਪਸ ਨਤੀਜਾ;
        }
    };

ਨਿਜੀ:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> sig_ids;
    std::ਸਦਿਸ਼<std::pair<ਪੂਰਨ, ਪੂਰਨ>> ਕਿਨਾਰੇ;
    std::ਸਦਿਸ਼<std::pair<ਪੂਰਨ, ਪੂਰਨ>> ਕਿਨਾਰਾ_ranges;
    std::ਸਦਿਸ਼<ਪੂਰਨ> indices_;
    ਪੂਰਨ ਵਿੱਥ;
    bool computed = false;

    ਖਾਲੀ compute() {
        ਵਿੱਥ = Get_ਮਾਪ(sig_ids);
        ਕਿਨਾਰਾ_ranges.clear();
        indices_.clear();
        indices_.resize(Get_ਮਾਪ(sig_ids) + Get_ਮਾਪ(cell_ids), -1);

        std::ਛਾਂਟੋ(ਕਿਨਾਰੇ.begin(), ਕਿਨਾਰੇ.ਸਿਰਾ());
        ਸਵੈਚਾਲਿਤ ਆਖਰੀ = std::unique(ਕਿਨਾਰੇ.begin(), ਕਿਨਾਰੇ.ਸਿਰਾ());
        ਕਿਨਾਰੇ.erase(ਆਖਰੀ, ਕਿਨਾਰੇ.ਸਿਰਾ());
        ਸਵੈਚਾਲਿਤ ਕਿਨਾਰਾ = ਕਿਨਾਰੇ.begin();
        ਸਵੈਚਾਲਿਤ ਕਿਨਾਰਾ_ਸਿਰਾ = ਕਿਨਾਰੇ.ਸਿਰਾ();
        ਪੂਰਨ range_begin = 0;
        ਲਈ (ਪੂਰਨ ਗੰਢ = -ਵਿੱਥ, ਗੰਢ_ਸਿਰਾ = Get_ਮਾਪ(cell_ids); ਗੰਢ != ਗੰਢ_ਸਿਰਾ; ++ਗੰਢ) {
            ਜਦੋਂ (ਕਿਨਾਰਾ != ਕਿਨਾਰਾ_ਸਿਰਾ && ਕਿਨਾਰਾ->ਪਹਿਲਾ <= ਗੰਢ)
                ++ਕਿਨਾਰਾ;
            ਪੂਰਨ range_ਸਿਰਾ_2 = ਕਿਨਾਰਾ - ਕਿਨਾਰੇ.begin();
            ਕਿਨਾਰਾ_ranges.emplace_ਪਿੱਠ(std::make_pair(range_begin, range_ਸਿਰਾ_2));
            range_begin = range_ਸਿਰਾ_2;
        }
    }

ਜਨਤਕ:
    ਗੰਢ_type ਗੰਢ(RTLIL::Cell *cell) { ਵਾਪਸ cell_ids(cell); }
    ਗੰਢ_type ਗੰਢ(Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਅਚਲ &ਦੋਆਧਾਰੀ_ਅੰਕ) { ਵਾਪਸ ~sig_ids(ਦੋਆਧਾਰੀ_ਅੰਕ); }

    bool is_cell(ਗੰਢ_type ਗੰਢ) { ਵਾਪਸ ਗੰਢ >= 0; }
    bool is_sig(ਗੰਢ_type ਗੰਢ) { ਵਾਪਸ ਗੰਢ < 0; }

    Cell *cell(ਗੰਢ_type ਗੰਢ) { ਵਾਪਸ ਗੰਢ >= 0 ? cell_ids[ਗੰਢ] : nullptr; }
    Sig_ਦੋਆਧਾਰੀ_ਅੰਕ sig(ਗੰਢ_type ਗੰਢ) { ਵਾਪਸ ਗੰਢ < 0 ? sig_ids[~ਗੰਢ] : Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(); }

    ਸਾਂਚਾ<typename Src, typename Dst>
    ਖਾਲੀ ਜੋੜੋ_ਕਿਨਾਰਾ(Src &&src, Dst &&dst) {
        computed = false;
        ਗੰਢ_type src_ਗੰਢ = ਗੰਢ(std::forward<Src>(src));
        ਗੰਢ_type dst_ਗੰਢ = ਗੰਢ(std::forward<Dst>(dst));
        ਕਿਨਾਰੇ.emplace_ਪਿੱਠ(std::make_pair(src_ਗੰਢ, dst_ਗੰਢ));
    }

    ਗੰਢ_enumerator enumerate_nodes() {
        ਜੇ (!computed) compute();
        ਵਾਪਸ {-Get_ਮਾਪ(sig_ids), Get_ਮਾਪ(cell_ids)};
    }

    successor_enumerator enumerate_ਉੱਤਰਾਧਿਕਾਰੀ(ਗੰਢ_type ਅਚਲ &ਗੰਢ) ਅਚਲ {
        ਸਵੈਚਾਲਿਤ range = ਕਿਨਾਰਾ_ranges[ਗੰਢ + ਵਿੱਥ];
        ਵਾਪਸ {ਕਿਨਾਰੇ.begin() + range.ਪਹਿਲਾ, ਕਿਨਾਰੇ.begin() + range.ਦੂਜਾ};
    }

    ਪੂਰਨ &dfs_ਸੂਚਕਾਂਕ(ਗੰਢ_type ਅਚਲ &ਗੰਢ) {
        ਵਾਪਸ indices_[ਗੰਢ + ਵਿੱਥ];
    }
};


class Int_ਗ੍ਰਾਫ {
ਜਨਤਕ:
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ ਗੰਢ_type;

    ਬਣਤਰ successor_enumerator {
        std::ਸਦਿਸ਼<std::pair<ਪੂਰਨ, ਪੂਰਨ>>::const_iterator current, ਸਿਰਾ;
        bool ਮੁਕੰਮਲ() ਅਚਲ { ਵਾਪਸ current == ਸਿਰਾ; }
        ਗੰਢ_type ਅਗਲਾ() {
            log_assert(!ਮੁਕੰਮਲ());
            ਗੰਢ_type ਨਤੀਜਾ = current->ਦੂਜਾ;
            ++current;
            ਵਾਪਸ ਨਤੀਜਾ;
        }
    };

    ਬਣਤਰ ਗੰਢ_enumerator {
        ਪੂਰਨ current, ਸਿਰਾ;
        bool ਮੁਕੰਮਲ() ਅਚਲ { ਵਾਪਸ current == ਸਿਰਾ; }
        ਗੰਢ_type ਅਗਲਾ() {
            log_assert(!ਮੁਕੰਮਲ());
            ਗੰਢ_type ਨਤੀਜਾ = current;
            ++current;
            ਵਾਪਸ ਨਤੀਜਾ;
        }
    };

ਨਿਜੀ:
    std::ਸਦਿਸ਼<std::pair<ਪੂਰਨ, ਪੂਰਨ>> ਕਿਨਾਰੇ;
    std::ਸਦਿਸ਼<std::pair<ਪੂਰਨ, ਪੂਰਨ>> ਕਿਨਾਰਾ_ranges;
    std::ਸਦਿਸ਼<ਪੂਰਨ> indices_;
    bool computed = false;

    ਖਾਲੀ compute() {
        ਕਿਨਾਰਾ_ranges.clear();

        ਪੂਰਨ ਗੰਢ_ਸਿਰਾ = 0;
        ਲਈ (ਸਵੈਚਾਲਿਤ ਅਚਲ &ਕਿਨਾਰਾ : ਕਿਨਾਰੇ)
            ਗੰਢ_ਸਿਰਾ = std::max(ਗੰਢ_ਸਿਰਾ, std::max(ਕਿਨਾਰਾ.ਪਹਿਲਾ, ਕਿਨਾਰਾ.ਦੂਜਾ) + 1);
        indices_.clear();
        indices_.resize(ਗੰਢ_ਸਿਰਾ, -1);

        std::ਛਾਂਟੋ(ਕਿਨਾਰੇ.begin(), ਕਿਨਾਰੇ.ਸਿਰਾ());
        ਸਵੈਚਾਲਿਤ ਆਖਰੀ = std::unique(ਕਿਨਾਰੇ.begin(), ਕਿਨਾਰੇ.ਸਿਰਾ());
        ਕਿਨਾਰੇ.erase(ਆਖਰੀ, ਕਿਨਾਰੇ.ਸਿਰਾ());
        ਸਵੈਚਾਲਿਤ ਕਿਨਾਰਾ = ਕਿਨਾਰੇ.begin();
        ਸਵੈਚਾਲਿਤ ਕਿਨਾਰਾ_ਸਿਰਾ = ਕਿਨਾਰੇ.ਸਿਰਾ();
        ਪੂਰਨ range_begin = 0;
        ਲਈ (ਪੂਰਨ ਗੰਢ = 0; ਗੰਢ != ਗੰਢ_ਸਿਰਾ; ++ਗੰਢ) {
            ਜਦੋਂ (ਕਿਨਾਰਾ != ਕਿਨਾਰਾ_ਸਿਰਾ && ਕਿਨਾਰਾ->ਪਹਿਲਾ <= ਗੰਢ)
                ++ਕਿਨਾਰਾ;
            ਪੂਰਨ range_ਸਿਰਾ_2 = ਕਿਨਾਰਾ - ਕਿਨਾਰੇ.begin();
            ਕਿਨਾਰਾ_ranges.emplace_ਪਿੱਠ(std::make_pair(range_begin, range_ਸਿਰਾ_2));
            range_begin = range_ਸਿਰਾ_2;
        }
    }

ਜਨਤਕ:
    ਖਾਲੀ ਜੋੜੋ_ਕਿਨਾਰਾ(ਪੂਰਨ src, ਪੂਰਨ dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        ਕਿਨਾਰੇ.emplace_ਪਿੱਠ(std::make_pair(src, dst));
    }

    ਗੰਢ_enumerator enumerate_nodes() {
        ਜੇ (!computed) compute();
        ਵਾਪਸ {0, Get_ਮਾਪ(indices_)};
    }

    successor_enumerator enumerate_ਉੱਤਰਾਧਿਕਾਰੀ(ਪੂਰਨ ਗੰਢ) ਅਚਲ {
        ਸਵੈਚਾਲਿਤ range = ਕਿਨਾਰਾ_ranges[ਗੰਢ];
        ਵਾਪਸ {ਕਿਨਾਰੇ.begin() + range.ਪਹਿਲਾ, ਕਿਨਾਰੇ.begin() + range.ਦੂਜਾ};
    }

    ਪੂਰਨ &dfs_ਸੂਚਕਾਂਕ(ਗੰਢ_type ਅਚਲ &ਗੰਢ) {
        ਵਾਪਸ indices_[ਗੰਢ];
    }
};

ਸਾਂਚਾ<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ typename G::ਗੰਢ_enumerator ਗੰਢ_enumerator;
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ typename G::successor_enumerator successor_enumerator;
    ਕਿਸਮਪਰਿਭਾਸ਼ਾ typename G::ਗੰਢ_type ਗੰਢ_type;

    ਬਣਤਰ dfs_ਇੰਦਰਾਜ {
        ਗੰਢ_type ਗੰਢ;
        successor_enumerator ਉੱਤਰਾਧਿਕਾਰੀ;
        ਪੂਰਨ lowlink;

        dfs_ਇੰਦਰਾਜ(ਗੰਢ_type ਗੰਢ, successor_enumerator ਉੱਤਰਾਧਿਕਾਰੀ, ਪੂਰਨ lowlink) :
            ਗੰਢ(ਗੰਢ), ਉੱਤਰਾਧਿਕਾਰੀ(ਉੱਤਰਾਧਿਕਾਰੀ), lowlink(lowlink)
        {}
    };

    G &ਗ੍ਰਾਫ_3;
    ComponentCallback component;

    std::ਸਦਿਸ਼<dfs_ਇੰਦਰਾਜ> dfs_ਢੇਰ;
    std::ਸਦਿਸ਼<ਗੰਢ_type> component_ਢੇਰ;
    ਪੂਰਨ ਅਗਲਾ_ਸੂਚਕਾਂਕ = 0;

ਜਨਤਕ:
    TopoSortedSccs(G &ਗ੍ਰਾਫ_3, ComponentCallback component)
    : ਗ੍ਰਾਫ_3(ਗ੍ਰਾਫ_3), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        ਗੰਢ_enumerator nodes = ਗ੍ਰਾਫ_3.enumerate_nodes();
        ਜਦੋਂ (!nodes.ਮੁਕੰਮਲ()) {
            ਗੰਢ_type ਗੰਢ = nodes.ਅਗਲਾ();
            successor_enumerator ਉੱਤਰਾਧਿਕਾਰੀ = ਗ੍ਰਾਫ_3.enumerate_ਉੱਤਰਾਧਿਕਾਰੀ(ਗੰਢ);
            ਜੇ (ਉੱਤਰਾਧਿਕਾਰੀ.ਮੁਕੰਮਲ())
            {
                ਗ੍ਰਾਫ_3.dfs_ਸੂਚਕਾਂਕ(ਗੰਢ) = ਅਗਲਾ_ਸੂਚਕਾਂਕ;
                ਅਗਲਾ_ਸੂਚਕਾਂਕ++;
                component_ਢੇਰ.ਧੱਕੋ_ਪਿੱਠ(ਗੰਢ);
                component(component_ਢੇਰ.ਅੰਕੜੇ(), component_ਢੇਰ.ਅੰਕੜੇ() + 1);
                component_ਢੇਰ.clear();
                ਗ੍ਰਾਫ_3.dfs_ਸੂਚਕਾਂਕ(ਗੰਢ) = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
            }
        }
        ਵਾਪਸ *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        ਗੰਢ_enumerator nodes = ਗ੍ਰਾਫ_3.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        ਜਦੋਂ (!nodes.ਮੁਕੰਮਲ())
            process(nodes.ਅਗਲਾ());
        ਵਾਪਸ *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(ਗੰਢ_type ਗੰਢ) {
        // only start a new search if the node wasn't visited yet
        ਜੇ (ਗ੍ਰਾਫ_3.dfs_ਸੂਚਕਾਂਕ(ਗੰਢ) >= 0)
            ਵਾਪਸ *this;
        ਜਦੋਂ (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            ਪੂਰਨ lowlink = ਅਗਲਾ_ਸੂਚਕਾਂਕ;
            ਅਗਲਾ_ਸੂਚਕਾਂਕ++;
            ਗ੍ਰਾਫ_3.dfs_ਸੂਚਕਾਂਕ(ਗੰਢ) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_ਢੇਰ.ਧੱਕੋ_ਪਿੱਠ(ਗੰਢ);

            // then we start iterating over the successors of this node
            successor_enumerator ਉੱਤਰਾਧਿਕਾਰੀ = ਗ੍ਰਾਫ_3.enumerate_ਉੱਤਰਾਧਿਕਾਰੀ(ਗੰਢ);
            ਜਦੋਂ (true) {
                ਜੇ (ਉੱਤਰਾਧਿਕਾਰੀ.ਮੁਕੰਮਲ()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    ਜੇ (lowlink == ਗ੍ਰਾਫ_3.dfs_ਸੂਚਕਾਂਕ(ਗੰਢ)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        ਪੂਰਨ current = Get_ਮਾਪ(component_ਢੇਰ);
                        ਕਰੋ {
                            --current;
                        } ਜਦੋਂ (component_ਢੇਰ[current] != ਗੰਢ);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        ਗੰਢ_type *ਢੇਰ_ptr = component_ਢੇਰ.ਅੰਕੜੇ();
                        ਗੰਢ_type *component_begin = ਢੇਰ_ptr + current;
                        ਗੰਢ_type *component_ਸਿਰਾ = ਢੇਰ_ptr + component_ਢੇਰ.ਮਾਪ();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_ਸਿਰਾ);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        ਲਈ (; component_begin != component_ਸਿਰਾ; ++component_begin)
                            ਗ੍ਰਾਫ_3.dfs_ਸੂਚਕਾਂਕ(*component_begin) = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
                        component_ਢੇਰ.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    ਜੇ (dfs_ਢੇਰ.ਖਾਲੀ_ਥਾਵਾਂ())
                        ਵਾਪਸ *this;
                    ਸਵੈਚਾਲਿਤ &dfs_ਸਿਖਰ = dfs_ਢੇਰ.ਪਿੱਠ();

                    ਗੰਢ = dfs_ਸਿਖਰ.ਗੰਢ;
                    ਉੱਤਰਾਧਿਕਾਰੀ = std::move(dfs_ਸਿਖਰ.ਉੱਤਰਾਧਿਕਾਰੀ);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_ਸਿਖਰ.lowlink);
                    dfs_ਢੇਰ.ਕੱਢੋ_ਪਿੱਠ();
                    // continue checking the remaining successors of the parent node.
                } ਨਹੀਂਤਾਂ {
                    ਗੰਢ_type succ = ਉੱਤਰਾਧਿਕਾਰੀ.ਅਗਲਾ();
                    ਜੇ (ਗ੍ਰਾਫ_3.dfs_ਸੂਚਕਾਂਕ(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_ਢੇਰ.emplace_ਪਿੱਠ(ਗੰਢ, std::move(ਉੱਤਰਾਧਿਕਾਰੀ), lowlink);
                        ਗੰਢ = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        ਰੋਕੋ;
                    } ਨਹੀਂਤਾਂ {
                        // the textbook version guards this update with a check
                        // whether the successor is still on the component
                        // stack. If the successor node was already visisted
                        // but is not on the component stack, it must be part
                        // of an already emitted SCC. We can avoid this check
                        // by setting the DFS index of all nodes in a SCC to
                        // INT_MAX when the SCC is emitted.
                        lowlink = min(lowlink, ਗ੍ਰਾਫ_3.dfs_ਸੂਚਕਾਂਕ(succ));
                    }
                }
            }
        }
    }
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
