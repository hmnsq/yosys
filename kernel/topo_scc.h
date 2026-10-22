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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TOPO_SCC_H
#ವ್ಯಾಖ್ಯಾನಿಸು TOPO_SCC_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class Sig_Cell_ನಕ್ಷೆ {
ಸಾರ್ವಜನಿಕ:
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ ಗಂಟು_type;

    ರಚನೆ successor_enumerator {
        std::ಸದಿಶ<std::pair<ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ>>::const_iterator current, ಕೊನೆ;
        bool ಪೂರ್ಣಗೊಂಡ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ current == ಕೊನೆ; }
        ಗಂಟು_type ಮುಂದಿನ() {
            log_assert(!ಪೂರ್ಣಗೊಂಡ());
            ಗಂಟು_type ಫಲಿತಾಂಶ = current->ಎರಡನೇ;
            ++current;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
    };

    ರಚನೆ ಗಂಟು_enumerator {
        ಪೂರ್ಣಾಂಕ current, ಕೊನೆ;
        bool ಪೂರ್ಣಗೊಂಡ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ current == ಕೊನೆ; }
        ಗಂಟು_type ಮುಂದಿನ() {
            log_assert(!ಪೂರ್ಣಗೊಂಡ());
            ಗಂಟು_type ಫಲಿತಾಂಶ = current;
            ++current;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
    };

ಖಾಸಗಿ:
    idict<RTLIL::Cell *> cell_ids;
    idict<RTLIL::Sig_ಬಿಟ್> sig_ids;
    std::ಸದಿಶ<std::pair<ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ>> ಅಂಚುಗಳು;
    std::ಸದಿಶ<std::pair<ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ>> ಅಂಚು_ranges;
    std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> indices_;
    ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;
    bool computed = false;

    ಶೂನ್ಯ compute() {
        ಸ್ಥಾನಾಂತರ = Get_ಗಾತ್ರ(sig_ids);
        ಅಂಚು_ranges.clear();
        indices_.clear();
        indices_.resize(Get_ಗಾತ್ರ(sig_ids) + Get_ಗಾತ್ರ(cell_ids), -1);

        std::ವಿಂಗಡಿಸು(ಅಂಚುಗಳು.begin(), ಅಂಚುಗಳು.ಕೊನೆ());
        ಸ್ವಯಂಚಾಲಿತ ಕೊನೆಯ = std::unique(ಅಂಚುಗಳು.begin(), ಅಂಚುಗಳು.ಕೊನೆ());
        ಅಂಚುಗಳು.erase(ಕೊನೆಯ, ಅಂಚುಗಳು.ಕೊನೆ());
        ಸ್ವಯಂಚಾಲಿತ ಅಂಚು = ಅಂಚುಗಳು.begin();
        ಸ್ವಯಂಚಾಲಿತ ಅಂಚು_ಕೊನೆ = ಅಂಚುಗಳು.ಕೊನೆ();
        ಪೂರ್ಣಾಂಕ range_begin = 0;
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಗಂಟು = -ಸ್ಥಾನಾಂತರ, ಗಂಟು_ಕೊನೆ = Get_ಗಾತ್ರ(cell_ids); ಗಂಟು != ಗಂಟು_ಕೊನೆ; ++ಗಂಟು) {
            ಆಗ (ಅಂಚು != ಅಂಚು_ಕೊನೆ && ಅಂಚು->ಮೊದಲ <= ಗಂಟು)
                ++ಅಂಚು;
            ಪೂರ್ಣಾಂಕ range_ಕೊನೆ_2 = ಅಂಚು - ಅಂಚುಗಳು.begin();
            ಅಂಚು_ranges.emplace_ಹಿಂಭಾಗ(std::make_pair(range_begin, range_ಕೊನೆ_2));
            range_begin = range_ಕೊನೆ_2;
        }
    }

ಸಾರ್ವಜನಿಕ:
    ಗಂಟು_type ಗಂಟು(RTLIL::Cell *cell) { ಹಿಂತಿರುಗಿ cell_ids(cell); }
    ಗಂಟು_type ಗಂಟು(Sig_ಬಿಟ್ ಸ್ಥಿರಾಂಕ &ಬಿಟ್) { ಹಿಂತಿರುಗಿ ~sig_ids(ಬಿಟ್); }

    bool is_cell(ಗಂಟು_type ಗಂಟು) { ಹಿಂತಿರುಗಿ ಗಂಟು >= 0; }
    bool is_sig(ಗಂಟು_type ಗಂಟು) { ಹಿಂತಿರುಗಿ ಗಂಟು < 0; }

    Cell *cell(ಗಂಟು_type ಗಂಟು) { ಹಿಂತಿರುಗಿ ಗಂಟು >= 0 ? cell_ids[ಗಂಟು] : nullptr; }
    Sig_ಬಿಟ್ sig(ಗಂಟು_type ಗಂಟು) { ಹಿಂತಿರುಗಿ ಗಂಟು < 0 ? sig_ids[~ಗಂಟು] : Sig_ಬಿಟ್(); }

    ಮಾದರಿ<typename Src, typename Dst>
    ಶೂನ್ಯ ಕೂಡಿಸು_ಅಂಚು(Src &&src, Dst &&dst) {
        computed = false;
        ಗಂಟು_type src_ಗಂಟು = ಗಂಟು(std::forward<Src>(src));
        ಗಂಟು_type dst_ಗಂಟು = ಗಂಟು(std::forward<Dst>(dst));
        ಅಂಚುಗಳು.emplace_ಹಿಂಭಾಗ(std::make_pair(src_ಗಂಟು, dst_ಗಂಟು));
    }

    ಗಂಟು_enumerator enumerate_nodes() {
        ಆದರೆ (!computed) compute();
        ಹಿಂತಿರುಗಿ {-Get_ಗಾತ್ರ(sig_ids), Get_ಗಾತ್ರ(cell_ids)};
    }

    successor_enumerator enumerate_ಉತ್ತರಾಧಿಕಾರಿಗಳು(ಗಂಟು_type ಸ್ಥಿರಾಂಕ &ಗಂಟು) ಸ್ಥಿರಾಂಕ {
        ಸ್ವಯಂಚಾಲಿತ range = ಅಂಚು_ranges[ಗಂಟು + ಸ್ಥಾನಾಂತರ];
        ಹಿಂತಿರುಗಿ {ಅಂಚುಗಳು.begin() + range.ಮೊದಲ, ಅಂಚುಗಳು.begin() + range.ಎರಡನೇ};
    }

    ಪೂರ್ಣಾಂಕ &dfs_ಸೂಚ್ಯಂಕ(ಗಂಟು_type ಸ್ಥಿರಾಂಕ &ಗಂಟು) {
        ಹಿಂತಿರುಗಿ indices_[ಗಂಟು + ಸ್ಥಾನಾಂತರ];
    }
};


class Int_ನಕ್ಷೆ {
ಸಾರ್ವಜನಿಕ:
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ ಗಂಟು_type;

    ರಚನೆ successor_enumerator {
        std::ಸದಿಶ<std::pair<ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ>>::const_iterator current, ಕೊನೆ;
        bool ಪೂರ್ಣಗೊಂಡ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ current == ಕೊನೆ; }
        ಗಂಟು_type ಮುಂದಿನ() {
            log_assert(!ಪೂರ್ಣಗೊಂಡ());
            ಗಂಟು_type ಫಲಿತಾಂಶ = current->ಎರಡನೇ;
            ++current;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
    };

    ರಚನೆ ಗಂಟು_enumerator {
        ಪೂರ್ಣಾಂಕ current, ಕೊನೆ;
        bool ಪೂರ್ಣಗೊಂಡ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ current == ಕೊನೆ; }
        ಗಂಟು_type ಮುಂದಿನ() {
            log_assert(!ಪೂರ್ಣಗೊಂಡ());
            ಗಂಟು_type ಫಲಿತಾಂಶ = current;
            ++current;
            ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        }
    };

ಖಾಸಗಿ:
    std::ಸದಿಶ<std::pair<ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ>> ಅಂಚುಗಳು;
    std::ಸದಿಶ<std::pair<ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ>> ಅಂಚು_ranges;
    std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> indices_;
    bool computed = false;

    ಶೂನ್ಯ compute() {
        ಅಂಚು_ranges.clear();

        ಪೂರ್ಣಾಂಕ ಗಂಟು_ಕೊನೆ = 0;
        ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ಅಂಚು : ಅಂಚುಗಳು)
            ಗಂಟು_ಕೊನೆ = std::max(ಗಂಟು_ಕೊನೆ, std::max(ಅಂಚು.ಮೊದಲ, ಅಂಚು.ಎರಡನೇ) + 1);
        indices_.clear();
        indices_.resize(ಗಂಟು_ಕೊನೆ, -1);

        std::ವಿಂಗಡಿಸು(ಅಂಚುಗಳು.begin(), ಅಂಚುಗಳು.ಕೊನೆ());
        ಸ್ವಯಂಚಾಲಿತ ಕೊನೆಯ = std::unique(ಅಂಚುಗಳು.begin(), ಅಂಚುಗಳು.ಕೊನೆ());
        ಅಂಚುಗಳು.erase(ಕೊನೆಯ, ಅಂಚುಗಳು.ಕೊನೆ());
        ಸ್ವಯಂಚಾಲಿತ ಅಂಚು = ಅಂಚುಗಳು.begin();
        ಸ್ವಯಂಚಾಲಿತ ಅಂಚು_ಕೊನೆ = ಅಂಚುಗಳು.ಕೊನೆ();
        ಪೂರ್ಣಾಂಕ range_begin = 0;
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಗಂಟು = 0; ಗಂಟು != ಗಂಟು_ಕೊನೆ; ++ಗಂಟು) {
            ಆಗ (ಅಂಚು != ಅಂಚು_ಕೊನೆ && ಅಂಚು->ಮೊದಲ <= ಗಂಟು)
                ++ಅಂಚು;
            ಪೂರ್ಣಾಂಕ range_ಕೊನೆ_2 = ಅಂಚು - ಅಂಚುಗಳು.begin();
            ಅಂಚು_ranges.emplace_ಹಿಂಭಾಗ(std::make_pair(range_begin, range_ಕೊನೆ_2));
            range_begin = range_ಕೊನೆ_2;
        }
    }

ಸಾರ್ವಜನಿಕ:
    ಶೂನ್ಯ ಕೂಡಿಸು_ಅಂಚು(ಪೂರ್ಣಾಂಕ src, ಪೂರ್ಣಾಂಕ dst) {
        log_assert(src >= 0);
        log_assert(dst >= 0);
        computed = false;
        ಅಂಚುಗಳು.emplace_ಹಿಂಭಾಗ(std::make_pair(src, dst));
    }

    ಗಂಟು_enumerator enumerate_nodes() {
        ಆದರೆ (!computed) compute();
        ಹಿಂತಿರುಗಿ {0, Get_ಗಾತ್ರ(indices_)};
    }

    successor_enumerator enumerate_ಉತ್ತರಾಧಿಕಾರಿಗಳು(ಪೂರ್ಣಾಂಕ ಗಂಟು) ಸ್ಥಿರಾಂಕ {
        ಸ್ವಯಂಚಾಲಿತ range = ಅಂಚು_ranges[ಗಂಟು];
        ಹಿಂತಿರುಗಿ {ಅಂಚುಗಳು.begin() + range.ಮೊದಲ, ಅಂಚುಗಳು.begin() + range.ಎರಡನೇ};
    }

    ಪೂರ್ಣಾಂಕ &dfs_ಸೂಚ್ಯಂಕ(ಗಂಟು_type ಸ್ಥಿರಾಂಕ &ಗಂಟು) {
        ಹಿಂತಿರುಗಿ indices_[ಗಂಟು];
    }
};

ಮಾದರಿ<typename G, typename ComponentCallback>
class TopoSortedSccs
{
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ typename G::ಗಂಟು_enumerator ಗಂಟು_enumerator;
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ typename G::successor_enumerator successor_enumerator;
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ typename G::ಗಂಟು_type ಗಂಟು_type;

    ರಚನೆ dfs_ನಮೂದು {
        ಗಂಟು_type ಗಂಟು;
        successor_enumerator ಉತ್ತರಾಧಿಕಾರಿಗಳು;
        ಪೂರ್ಣಾಂಕ lowlink;

        dfs_ನಮೂದು(ಗಂಟು_type ಗಂಟು, successor_enumerator ಉತ್ತರಾಧಿಕಾರಿಗಳು, ಪೂರ್ಣಾಂಕ lowlink) :
            ಗಂಟು(ಗಂಟು), ಉತ್ತರಾಧಿಕಾರಿಗಳು(ಉತ್ತರಾಧಿಕಾರಿಗಳು), lowlink(lowlink)
        {}
    };

    G &ನಕ್ಷೆ_3;
    ComponentCallback component;

    std::ಸದಿಶ<dfs_ನಮೂದು> dfs_ರಾಶಿ;
    std::ಸದಿಶ<ಗಂಟು_type> component_ರಾಶಿ;
    ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_ಸೂಚ್ಯಂಕ = 0;

ಸಾರ್ವಜನಿಕ:
    TopoSortedSccs(G &ನಕ್ಷೆ_3, ComponentCallback component)
    : ನಕ್ಷೆ_3(ನಕ್ಷೆ_3), component(component) {}

    // process all sources (nodes without a successor)
    TopoSortedSccs &process_sources() {
        ಗಂಟು_enumerator nodes = ನಕ್ಷೆ_3.enumerate_nodes();
        ಆಗ (!nodes.ಪೂರ್ಣಗೊಂಡ()) {
            ಗಂಟು_type ಗಂಟು = nodes.ಮುಂದಿನ();
            successor_enumerator ಉತ್ತರಾಧಿಕಾರಿಗಳು = ನಕ್ಷೆ_3.enumerate_ಉತ್ತರಾಧಿಕಾರಿಗಳು(ಗಂಟು);
            ಆದರೆ (ಉತ್ತರಾಧಿಕಾರಿಗಳು.ಪೂರ್ಣಗೊಂಡ())
            {
                ನಕ್ಷೆ_3.dfs_ಸೂಚ್ಯಂಕ(ಗಂಟು) = ಮುಂದಿನ_ಸೂಚ್ಯಂಕ;
                ಮುಂದಿನ_ಸೂಚ್ಯಂಕ++;
                component_ರಾಶಿ.ತಳ್ಳು_ಹಿಂಭಾಗ(ಗಂಟು);
                component(component_ರಾಶಿ.ದತ್ತಾಂಶ(), component_ರಾಶಿ.ದತ್ತಾಂಶ() + 1);
                component_ರಾಶಿ.clear();
                ನಕ್ಷೆ_3.dfs_ಸೂಚ್ಯಂಕ(ಗಂಟು) = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
            }
        }
        ಹಿಂತಿರುಗಿ *this;
    }

    // process all remaining nodes in the graph
    TopoSortedSccs &process_all() {
        ಗಂಟು_enumerator nodes = ನಕ್ಷೆ_3.enumerate_nodes();
        // iterate over all nodes to ensure we process the whole graph
        ಆಗ (!nodes.ಪೂರ್ಣಗೊಂಡ())
            process(nodes.ಮುಂದಿನ());
        ಹಿಂತಿರುಗಿ *this;
    }

    // process all nodes that are reachable from a given start node
    TopoSortedSccs &process(ಗಂಟು_type ಗಂಟು) {
        // only start a new search if the node wasn't visited yet
        ಆದರೆ (ನಕ್ಷೆ_3.dfs_ಸೂಚ್ಯಂಕ(ಗಂಟು) >= 0)
            ಹಿಂತಿರುಗಿ *this;
        ಆಗ (true) {
            // at this point we're visiting the node for the first time during
            // the DFS search

            // we record the timestamp of when we first visited the node as the
            // dfs_index
            ಪೂರ್ಣಾಂಕ lowlink = ಮುಂದಿನ_ಸೂಚ್ಯಂಕ;
            ಮುಂದಿನ_ಸೂಚ್ಯಂಕ++;
            ನಕ್ಷೆ_3.dfs_ಸೂಚ್ಯಂಕ(ಗಂಟು) = lowlink;

            // and we add the node to the component stack where it will remain
            // until all nodes of the component containing this node are popped
            component_ರಾಶಿ.ತಳ್ಳು_ಹಿಂಭಾಗ(ಗಂಟು);

            // then we start iterating over the successors of this node
            successor_enumerator ಉತ್ತರಾಧಿಕಾರಿಗಳು = ನಕ್ಷೆ_3.enumerate_ಉತ್ತರಾಧಿಕಾರಿಗಳು(ಗಂಟು);
            ಆಗ (true) {
                ಆದರೆ (ಉತ್ತರಾಧಿಕಾರಿಗಳು.ಪೂರ್ಣಗೊಂಡ()) {
                    // when we processed all successors, i.e. when we visited
                    // the complete DFS subtree rooted at the current node, we
                    // first check whether the current node is a SCC root
                    //
                    // (why this check identifies SCC roots is out of scope for
                    // this comment, see other material on Tarjan's SCC
                    // algorithm)
                    ಆದರೆ (lowlink == ನಕ್ಷೆ_3.dfs_ಸೂಚ್ಯಂಕ(ಗಂಟು)) {
                        // the SCC containing the current node is at the top of
                        // the component stack, with the current node at the bottom
                        ಪೂರ್ಣಾಂಕ current = Get_ಗಾತ್ರ(component_ರಾಶಿ);
                        ಮಾಡು {
                            --current;
                        } ಆಗ (component_ರಾಶಿ[current] != ಗಂಟು);

                        // we invoke the callback with a pointer range of the
                        // nodes in the SCC

                        ಗಂಟು_type *ರಾಶಿ_ptr = component_ರಾಶಿ.ದತ್ತಾಂಶ();
                        ಗಂಟು_type *component_begin = ರಾಶಿ_ptr + current;
                        ಗಂಟು_type *component_ಕೊನೆ = ರಾಶಿ_ptr + component_ರಾಶಿ.ಗಾತ್ರ_2();

                        // note that we allow the callback to permute the nodes
                        // in this range as well as to modify dfs_index of the
                        // nodes in the SCC.
                        component(component_begin, component_ಕೊನೆ);

                        // by setting the dfs_index of all already emitted
                        // nodes to INT_MAX, we don't need a separate check for
                        // whether successor nodes are still on the component
                        // stack before updating the lowlink value
                        ಗಾಗಿ (; component_begin != component_ಕೊನೆ; ++component_begin)
                            ನಕ್ಷೆ_3.dfs_ಸೂಚ್ಯಂಕ(*component_begin) = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
                        component_ರಾಶಿ.resize(current);
                    }

                    // after checking for a completed SCC the DFS either
                    // continues the search at the parent node or returns to
                    // the outer loop if we already are at the root node.
                    ಆದರೆ (dfs_ರಾಶಿ.ಖಾಲಿಗಳು())
                        ಹಿಂತಿರುಗಿ *this;
                    ಸ್ವಯಂಚಾಲಿತ &dfs_ತುದಿ = dfs_ರಾಶಿ.ಹಿಂಭಾಗ();

                    ಗಂಟು = dfs_ತುದಿ.ಗಂಟು;
                    ಉತ್ತರಾಧಿಕಾರಿಗಳು = std::move(dfs_ತುದಿ.ಉತ್ತರಾಧಿಕಾರಿಗಳು);

                    // the parent's lowlink is updated when returning
                    lowlink = min(lowlink, dfs_ತುದಿ.lowlink);
                    dfs_ರಾಶಿ.ತೆಗೆ_ಹಿಂಭಾಗ();
                    // continue checking the remaining successors of the parent node.
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಗಂಟು_type succ = ಉತ್ತರಾಧಿಕಾರಿಗಳು.ಮುಂದಿನ();
                    ಆದರೆ (ನಕ್ಷೆ_3.dfs_ಸೂಚ್ಯಂಕ(succ) < 0) {
                        // if the successor wasn't visted yet, the DFS recurses
                        // into the successor

                        // we save the state for this node and make the
                        // successor the current node.
                        dfs_ರಾಶಿ.emplace_ಹಿಂಭಾಗ(ಗಂಟು, std::move(ಉತ್ತರಾಧಿಕಾರಿಗಳು), lowlink);
                        ಗಂಟು = succ;

                        // this break gets us to the section corresponding to
                        // the function entry in the recursive version
                        ನಿಲ್ಲಿಸು;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        // the textbook version guards this update with a check
                        // whether the successor is still on the component
                        // stack. If the successor node was already visisted
                        // but is not on the component stack, it must be part
                        // of an already emitted SCC. We can avoid this check
                        // by setting the DFS index of all nodes in a SCC to
                        // INT_MAX when the SCC is emitted.
                        lowlink = min(lowlink, ನಕ್ಷೆ_3.dfs_ಸೂಚ್ಯಂಕ(succ));
                    }
                }
            }
        }
    }
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
