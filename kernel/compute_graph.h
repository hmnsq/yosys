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

#ifndef COMPUTE_GRAPH_H
#define COMPUTE_GRAPH_H

#ಅನ್ತರ್ಭಾವ <tuple>
#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

template<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename ಕೀಲಿ_2 = std::tuple<> // Stable keys to refer to nodes
>
ಸಂರಚನಾ ComputeGraph
{
    ಸಂರಚನಾ Ref;
ಗುಪ್ತ:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    ಸಂರಚನಾ ಗಂಟು_2 {
        ಪೂರ್ಣಾಙ್ಕ fn_index;
        ಪೂರ್ಣಾಙ್ಕ arg_offset;
        ಪೂರ್ಣಾಙ್ಕ arg_ಎಣಿಕೆ;
        Attr attr;

        ಗಂಟು_2(ಪೂರ್ಣಾಙ್ಕ fn_index, Attr &&attr, ಪೂರ್ಣಾಙ್ಕ arg_offset, ಪೂರ್ಣಾಙ್ಕ arg_ಎಣಿಕೆ = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_ಎಣಿಕೆ(arg_ಎಣಿಕೆ), attr(std::move(attr)) {}

        ಗಂಟು_2(ಪೂರ್ಣಾಙ್ಕ fn_index, Attr ಧ್ರುವ &attr, ಪೂರ್ಣಾಙ್ಕ arg_offset, ಪೂರ್ಣಾಙ್ಕ arg_ಎಣಿಕೆ = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_ಎಣಿಕೆ(arg_ಎಣಿಕೆ), attr(attr) {}
    };


    std::vector<ಗಂಟು_2> nodes;
    std::vector<ಪೂರ್ಣಾಙ್ಕ> args;
    dict<ಕೀಲಿ_2, ಪೂರ್ಣಾಙ್ಕ> ಕೀಲಿಗಳು;
    dict<ಪೂರ್ಣಾಙ್ಕ, SparseAttr> sparse_attrs;

ಸಾರ್ವಜನಿಕ:
    template<typename Graph>
    ಸಂರಚನಾ BaseRef
    {
    ರಕ್ಷಿತ:
        friend ಸಂರಚನಾ ComputeGraph;
        Graph *graph_;
        ಪೂರ್ಣಾಙ್ಕ index_;
        BaseRef(Graph *graph, ಪೂರ್ಣಾಙ್ಕ index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        ಶೂನ್ಯ check() ಧ್ರುವ { log_assert(index_ < graph_->size()); }

        ಗಂಟು_2 ಧ್ರುವ &deref() ಧ್ರುವ { check(); ಪ್ರತ್ಯಾಗಮ graph_->nodes[index_]; }

    ಸಾರ್ವಜನಿಕ:
        ComputeGraph ಧ್ರುವ &graph() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ graph_; }
        ಪೂರ್ಣಾಙ್ಕ index() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index_; }

        ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ deref().arg_ಎಣಿಕೆ; }

        BaseRef arg(ಪೂರ್ಣಾಙ್ಕ n) ಧ್ರುವ
        {
            ಗಂಟು_2 ಧ್ರುವ &ಗಂಟು = deref();
            log_assert(n >= 0 && n < ಗಂಟು.arg_ಎಣಿಕೆ);
            ಪ್ರತ್ಯಾಗಮ BaseRef(graph_, graph_->args[ಗಂಟು.arg_offset + n]);
        }

        std::vector<ಪೂರ್ಣಾಙ್ಕ>::const_iterator arg_indices_cbegin() ಧ್ರುವ
        {
            ಗಂಟು_2 ಧ್ರುವ &ಗಂಟು = deref();
            ಪ್ರತ್ಯಾಗಮ graph_->args.cbegin() + ಗಂಟು.arg_offset;
        }

        std::vector<ಪೂರ್ಣಾಙ್ಕ>::const_iterator arg_indices_cend() ಧ್ರುವ
        {
            ಗಂಟು_2 ಧ್ರುವ &ಗಂಟು = deref();
            ಪ್ರತ್ಯಾಗಮ graph_->args.cbegin() + ಗಂಟು.arg_offset + ಗಂಟು.arg_ಎಣಿಕೆ;
        }

        Fn ಧ್ರುವ &function() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ graph_->functions[deref().fn_index]; }
        Attr ಧ್ರುವ &attr() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ deref().attr; }

        bool has_sparse_attr() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ graph_->sparse_attrs.ಎಣಿಕೆ(index_); }

        SparseAttr ಧ್ರುವ &sparse_attr() ಧ್ರುವ
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            ಪ್ರತ್ಯಾಗಮ found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph ಧ್ರುವ>;

    ಸಂರಚನಾ Ref : ಸಾರ್ವಜನಿಕ BaseRef<ComputeGraph>
    {
    ಗುಪ್ತ:
        friend ಸಂರಚನಾ ComputeGraph;
        Ref(ComputeGraph *graph, ಪೂರ್ಣಾಙ್ಕ index) : BaseRef<ComputeGraph>(graph, index) {}
        ಗಂಟು_2 &deref() ಧ್ರುವ { this->check(); ಪ್ರತ್ಯಾಗಮ this->graph_->nodes[this->index_]; }

    ಸಾರ್ವಜನಿಕ:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        ಶೂನ್ಯ set_function(Fn ಧ್ರುವ &function) ಧ್ರುವ
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ deref().attr; }

        ಶೂನ್ಯ append_arg(ConstRef arg) ಧ್ರುವ
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        ಶೂನ್ಯ append_arg(ಪೂರ್ಣಾಙ್ಕ arg) ಧ್ರುವ
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            ಗಂಟು_2 &ಗಂಟು = deref();
            ಯದಿ (ಗಂಟು.arg_offset + ಗಂಟು.arg_ಎಣಿಕೆ != GetSize(this->graph_->args))
                move_args(ಗಂಟು);
            this->graph_->args.ತಳ್ಳು_back(arg);
            ಗಂಟು.arg_ಎಣಿಕೆ++;
        }

        operator ConstRef() ಧ್ರುವ
        {
            ಪ್ರತ್ಯಾಗಮ ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() ಧ್ರುವ
        {
            ಪ್ರತ್ಯಾಗಮ this->graph_->sparse_attrs[this->index_];
        }

        ಶೂನ್ಯ clear_sparse_attr() ಧ್ರುವ
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        ಶೂನ್ಯ assign_ಕೀಲಿ(ಕೀಲಿ_2 ಧ್ರುವ &ಕೀಲಿ) ಧ್ರುವ
        {
            this->graph_->ಕೀಲಿಗಳು.emplace(ಕೀಲಿ, this->index_);
        }

    ಗುಪ್ತ:
        ಶೂನ್ಯ move_args(ಗಂಟು_2 &ಗಂಟು) ಧ್ರುವ
        {
            auto &args = this->graph_->args;
            ಪೂರ್ಣಾಙ್ಕ old_offset = ಗಂಟು.arg_offset;
            ಗಂಟು.arg_offset = GetSize(args);
            ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i != ಗಂಟು.arg_ಎಣಿಕೆ; ++i)
                args.ತಳ್ಳು_back(args[old_offset + i]);
        }

    };

    bool has_ಕೀಲಿ(ಕೀಲಿ_2 ಧ್ರುವ &ಕೀಲಿ) ಧ್ರುವ
    {
        ಪ್ರತ್ಯಾಗಮ ಕೀಲಿಗಳು.ಎಣಿಕೆ(ಕೀಲಿ);
    }

    dict<ಕೀಲಿ_2, ಪೂರ್ಣಾಙ್ಕ> ಧ್ರುವ &ಕೀಲಿಗಳು_2() ಧ್ರುವ
    {
        ಪ್ರತ್ಯಾಗಮ ಕೀಲಿಗಳು;
    }

    ConstRef operator()(ಕೀಲಿ_2 ಧ್ರುವ &ಕೀಲಿ) ಧ್ರುವ
    {
        auto it = ಕೀಲಿಗಳು.find(ಕೀಲಿ);
        log_assert(it != ಕೀಲಿಗಳು.end());
        ಪ್ರತ್ಯಾಗಮ (*this)[it->second];
    }

    Ref operator()(ಕೀಲಿ_2 ಧ್ರುವ &ಕೀಲಿ)
    {
        auto it = ಕೀಲಿಗಳು.find(ಕೀಲಿ);
        log_assert(it != ಕೀಲಿಗಳು.end());
        ಪ್ರತ್ಯಾಗಮ (*this)[it->second];
    }

    ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ GetSize(nodes); }

    ConstRef operator[](ಪೂರ್ಣಾಙ್ಕ index) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ConstRef(this, index); }
    Ref operator[](ಪೂರ್ಣಾಙ್ಕ index) { ಪ್ರತ್ಯಾಗಮ Ref(this, index); }

    Ref add(Fn ಧ್ರುವ &function, Attr &&attr)
    {
        ಪೂರ್ಣಾಙ್ಕ index = GetSize(nodes);
        ಪೂರ್ಣಾಙ್ಕ fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        ಪ್ರತ್ಯಾಗಮ Ref(this, index);
    }

    Ref add(Fn ಧ್ರುವ &function, Attr ಧ್ರುವ &attr)
    {
        ಪೂರ್ಣಾಙ್ಕ index = GetSize(nodes);
        ಪೂರ್ಣಾಙ್ಕ fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        ಪ್ರತ್ಯಾಗಮ Ref(this, index);
    }

    template<typename T>
    Ref add(Fn ಧ್ರುವ &function, Attr ಧ್ರುವ &attr, T &&args)
    {
        Ref added = add(function, attr);
        ಪ್ರತಿ (auto arg : args)
            added.append_arg(arg);
        ಪ್ರತ್ಯಾಗಮ added;
    }

    template<typename T>
    Ref add(Fn ಧ್ರುವ &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        ಪ್ರತಿ (auto arg : args)
            added.append_arg(arg);
        ಪ್ರತ್ಯಾಗಮ added;
    }

    Ref add(Fn ಧ್ರುವ &function, Attr ಧ್ರುವ &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        ಪ್ರತಿ (auto arg : args)
            added.append_arg(arg);
        ಪ್ರತ್ಯಾಗಮ added;
    }

    Ref add(Fn ಧ್ರುವ &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        ಪ್ರತಿ (auto arg : args)
            added.append_arg(arg);
        ಪ್ರತ್ಯಾಗಮ added;
    }

    template<typename T>
    Ref add(Fn ಧ್ರುವ &function, Attr ಧ್ರುವ &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        ಪ್ರತಿ (; begin != end; ++begin)
            added.append_arg(*begin);
        ಪ್ರತ್ಯಾಗಮ added;
    }

    ಶೂನ್ಯ compact_args()
    {
        std::vector<ಪೂರ್ಣಾಙ್ಕ> new_args;
        ಪ್ರತಿ (auto &ಗಂಟು : nodes)
        {
            ಪೂರ್ಣಾಙ್ಕ new_offset = GetSize(new_args);
            ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಗಂಟು.arg_ಎಣಿಕೆ; i++)
                new_args.ತಳ್ಳು_back(args[ಗಂಟು.arg_offset + i]);
            ಗಂಟು.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    ಶೂನ್ಯ permute(std::vector<ಪೂರ್ಣಾಙ್ಕ> ಧ್ರುವ &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<ಪೂರ್ಣಾಙ್ಕ> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(perm); ++i)
        {
            ಪೂರ್ಣಾಙ್ಕ j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    ಶೂನ್ಯ permute(std::vector<ಪೂರ್ಣಾಙ್ಕ> ಧ್ರುವ &perm, std::vector<ಪೂರ್ಣಾಙ್ಕ> ಧ್ರುವ &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<ಗಂಟು_2> new_nodes;
        new_nodes.reserve(perm.size());
        dict<ಪೂರ್ಣಾಙ್ಕ, SparseAttr> new_sparse_attrs;
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i : perm)
        {
            ಪೂರ್ಣಾಙ್ಕ j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            ಯದಿ (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        ಪ್ರತಿ (auto &ಕೀಲಿ : ಕೀಲಿಗಳು)
        {
            log_assert(ಕೀಲಿ.second < GetSize(inv_perm));
            log_assert(inv_perm[ಕೀಲಿ.second] >= 0);
            ಕೀಲಿ.second = inv_perm[ಕೀಲಿ.second];
        }
    }

    ಸಂರಚನಾ SccAdaptor
    {
    ಗುಪ್ತ:
        ComputeGraph ಧ್ರುವ &graph_;
        std::vector<ಪೂರ್ಣಾಙ್ಕ> indices_;
    ಸಾರ್ವಜನಿಕ:
        SccAdaptor(ComputeGraph ಧ್ರುವ &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ ಗಂಟು_type;

        ಸಂರಚನಾ ಗಂಟು_enumerator {
        ಗುಪ್ತ:
            friend ಸಂರಚನಾ SccAdaptor;
            ಪೂರ್ಣಾಙ್ಕ current, end;
            ಗಂಟು_enumerator(ಪೂರ್ಣಾಙ್ಕ current, ಪೂರ್ಣಾಙ್ಕ end) : current(current), end(end) {}

        ಸಾರ್ವಜನಿಕ:

            bool finished() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ current == end; }
            ಗಂಟು_type next() {
                log_assert(!finished());
                ಗಂಟು_type result = current;
                ++current;
                ಪ್ರತ್ಯಾಗಮ result;
            }
        };

        ಗಂಟು_enumerator enumerate_nodes() {
            ಪ್ರತ್ಯಾಗಮ ಗಂಟು_enumerator(0, GetSize(indices_));
        }


        ಸಂರಚನಾ successor_enumerator {
        ಗುಪ್ತ:
            friend ಸಂರಚನಾ SccAdaptor;
            std::vector<ಪೂರ್ಣಾಙ್ಕ>::const_iterator current, end;
            successor_enumerator(std::vector<ಪೂರ್ಣಾಙ್ಕ>::const_iterator current, std::vector<ಪೂರ್ಣಾಙ್ಕ>::const_iterator end) :
                current(current), end(end) {}

        ಸಾರ್ವಜನಿಕ:
            bool finished() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ current == end; }
            ಗಂಟು_type next() {
                log_assert(!finished());
                ಗಂಟು_type result = *current;
                ++current;
                ಪ್ರತ್ಯಾಗಮ result;
            }
        };

        successor_enumerator enumerate_successors(ಪೂರ್ಣಾಙ್ಕ index) ಧ್ರುವ {
            auto ಧ್ರುವ &ref = graph_[index];
            ಪ್ರತ್ಯಾಗಮ successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        ಪೂರ್ಣಾಙ್ಕ &dfs_index(ಗಂಟು_type ಧ್ರುವ &ಗಂಟು) { ಪ್ರತ್ಯಾಗಮ indices_[ಗಂಟು]; }

        std::vector<ಪೂರ್ಣಾಙ್ಕ> ಧ್ರುವ &dfs_indices() { ಪ್ರತ್ಯಾಗಮ indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
