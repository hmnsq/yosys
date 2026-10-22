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

#અન્તર્ભાવ <tuple>
#અન્તર્ભાવ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

template<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename ચાવી_2 = std::tuple<> // Stable keys to refer to nodes
>
સંરચના ComputeGraph
{
    સંરચના Ref;
ખાનગી:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    સંરચના ગાંઠ_2 {
        પૂર્ણાંક fn_index;
        પૂર્ણાંક arg_offset;
        પૂર્ણાંક arg_ગણતરી;
        Attr attr;

        ગાંઠ_2(પૂર્ણાંક fn_index, Attr &&attr, પૂર્ણાંક arg_offset, પૂર્ણાંક arg_ગણતરી = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_ગણતરી(arg_ગણતરી), attr(std::move(attr)) {}

        ગાંઠ_2(પૂર્ણાંક fn_index, Attr અચલ &attr, પૂર્ણાંક arg_offset, પૂર્ણાંક arg_ગણતરી = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_ગણતરી(arg_ગણતરી), attr(attr) {}
    };


    std::vector<ગાંઠ_2> nodes;
    std::vector<પૂર્ણાંક> args;
    dict<ચાવી_2, પૂર્ણાંક> ચાવીઓ;
    dict<પૂર્ણાંક, SparseAttr> sparse_attrs;

જાહેર:
    template<typename Graph>
    સંરચના BaseRef
    {
    સુરક્ષિત:
        friend સંરચના ComputeGraph;
        Graph *graph_;
        પૂર્ણાંક index_;
        BaseRef(Graph *graph, પૂર્ણાંક index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        શૂન્ય check() અચલ { log_assert(index_ < graph_->size()); }

        ગાંઠ_2 અચલ &deref() અચલ { check(); પાછા graph_->nodes[index_]; }

    જાહેર:
        ComputeGraph અચલ &graph() અચલ { પાછા graph_; }
        પૂર્ણાંક index() અચલ { પાછા index_; }

        પૂર્ણાંક size() અચલ { પાછા deref().arg_ગણતરી; }

        BaseRef arg(પૂર્ણાંક n) અચલ
        {
            ગાંઠ_2 અચલ &ગાંઠ = deref();
            log_assert(n >= 0 && n < ગાંઠ.arg_ગણતરી);
            પાછા BaseRef(graph_, graph_->args[ગાંઠ.arg_offset + n]);
        }

        std::vector<પૂર્ણાંક>::const_iterator arg_indices_cbegin() અચલ
        {
            ગાંઠ_2 અચલ &ગાંઠ = deref();
            પાછા graph_->args.cbegin() + ગાંઠ.arg_offset;
        }

        std::vector<પૂર્ણાંક>::const_iterator arg_indices_cend() અચલ
        {
            ગાંઠ_2 અચલ &ગાંઠ = deref();
            પાછા graph_->args.cbegin() + ગાંઠ.arg_offset + ગાંઠ.arg_ગણતરી;
        }

        Fn અચલ &function() અચલ { પાછા graph_->functions[deref().fn_index]; }
        Attr અચલ &attr() અચલ { પાછા deref().attr; }

        bool has_sparse_attr() અચલ { પાછા graph_->sparse_attrs.ગણતરી(index_); }

        SparseAttr અચલ &sparse_attr() અચલ
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            પાછા found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph અચલ>;

    સંરચના Ref : જાહેર BaseRef<ComputeGraph>
    {
    ખાનગી:
        friend સંરચના ComputeGraph;
        Ref(ComputeGraph *graph, પૂર્ણાંક index) : BaseRef<ComputeGraph>(graph, index) {}
        ગાંઠ_2 &deref() અચલ { this->check(); પાછા this->graph_->nodes[this->index_]; }

    જાહેર:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        શૂન્ય set_function(Fn અચલ &function) અચલ
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() અચલ { પાછા deref().attr; }

        શૂન્ય append_arg(ConstRef arg) અચલ
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        શૂન્ય append_arg(પૂર્ણાંક arg) અચલ
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            ગાંઠ_2 &ગાંઠ = deref();
            જો (ગાંઠ.arg_offset + ગાંઠ.arg_ગણતરી != GetSize(this->graph_->args))
                move_args(ગાંઠ);
            this->graph_->args.ધકેલો_back(arg);
            ગાંઠ.arg_ગણતરી++;
        }

        operator ConstRef() અચલ
        {
            પાછા ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() અચલ
        {
            પાછા this->graph_->sparse_attrs[this->index_];
        }

        શૂન્ય clear_sparse_attr() અચલ
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        શૂન્ય assign_ચાવી(ચાવી_2 અચલ &ચાવી) અચલ
        {
            this->graph_->ચાવીઓ.emplace(ચાવી, this->index_);
        }

    ખાનગી:
        શૂન્ય move_args(ગાંઠ_2 &ગાંઠ) અચલ
        {
            auto &args = this->graph_->args;
            પૂર્ણાંક old_offset = ગાંઠ.arg_offset;
            ગાંઠ.arg_offset = GetSize(args);
            માટે (પૂર્ણાંક i = 0; i != ગાંઠ.arg_ગણતરી; ++i)
                args.ધકેલો_back(args[old_offset + i]);
        }

    };

    bool has_ચાવી(ચાવી_2 અચલ &ચાવી) અચલ
    {
        પાછા ચાવીઓ.ગણતરી(ચાવી);
    }

    dict<ચાવી_2, પૂર્ણાંક> અચલ &ચાવીઓ_2() અચલ
    {
        પાછા ચાવીઓ;
    }

    ConstRef operator()(ચાવી_2 અચલ &ચાવી) અચલ
    {
        auto it = ચાવીઓ.find(ચાવી);
        log_assert(it != ચાવીઓ.end());
        પાછા (*this)[it->second];
    }

    Ref operator()(ચાવી_2 અચલ &ચાવી)
    {
        auto it = ચાવીઓ.find(ચાવી);
        log_assert(it != ચાવીઓ.end());
        પાછા (*this)[it->second];
    }

    પૂર્ણાંક size() અચલ { પાછા GetSize(nodes); }

    ConstRef operator[](પૂર્ણાંક index) અચલ { પાછા ConstRef(this, index); }
    Ref operator[](પૂર્ણાંક index) { પાછા Ref(this, index); }

    Ref add(Fn અચલ &function, Attr &&attr)
    {
        પૂર્ણાંક index = GetSize(nodes);
        પૂર્ણાંક fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        પાછા Ref(this, index);
    }

    Ref add(Fn અચલ &function, Attr અચલ &attr)
    {
        પૂર્ણાંક index = GetSize(nodes);
        પૂર્ણાંક fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        પાછા Ref(this, index);
    }

    template<typename T>
    Ref add(Fn અચલ &function, Attr અચલ &attr, T &&args)
    {
        Ref added = add(function, attr);
        માટે (auto arg : args)
            added.append_arg(arg);
        પાછા added;
    }

    template<typename T>
    Ref add(Fn અચલ &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        માટે (auto arg : args)
            added.append_arg(arg);
        પાછા added;
    }

    Ref add(Fn અચલ &function, Attr અચલ &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        માટે (auto arg : args)
            added.append_arg(arg);
        પાછા added;
    }

    Ref add(Fn અચલ &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        માટે (auto arg : args)
            added.append_arg(arg);
        પાછા added;
    }

    template<typename T>
    Ref add(Fn અચલ &function, Attr અચલ &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        માટે (; begin != end; ++begin)
            added.append_arg(*begin);
        પાછા added;
    }

    શૂન્ય compact_args()
    {
        std::vector<પૂર્ણાંક> new_args;
        માટે (auto &ગાંઠ : nodes)
        {
            પૂર્ણાંક new_offset = GetSize(new_args);
            માટે (પૂર્ણાંક i = 0; i < ગાંઠ.arg_ગણતરી; i++)
                new_args.ધકેલો_back(args[ગાંઠ.arg_offset + i]);
            ગાંઠ.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    શૂન્ય permute(std::vector<પૂર્ણાંક> અચલ &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<પૂર્ણાંક> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        માટે (પૂર્ણાંક i = 0; i < GetSize(perm); ++i)
        {
            પૂર્ણાંક j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    શૂન્ય permute(std::vector<પૂર્ણાંક> અચલ &perm, std::vector<પૂર્ણાંક> અચલ &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<ગાંઠ_2> new_nodes;
        new_nodes.reserve(perm.size());
        dict<પૂર્ણાંક, SparseAttr> new_sparse_attrs;
        માટે (પૂર્ણાંક i : perm)
        {
            પૂર્ણાંક j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            જો (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        માટે (પૂર્ણાંક &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        માટે (auto &ચાવી : ચાવીઓ)
        {
            log_assert(ચાવી.second < GetSize(inv_perm));
            log_assert(inv_perm[ચાવી.second] >= 0);
            ચાવી.second = inv_perm[ચાવી.second];
        }
    }

    સંરચના SccAdaptor
    {
    ખાનગી:
        ComputeGraph અચલ &graph_;
        std::vector<પૂર્ણાંક> indices_;
    જાહેર:
        SccAdaptor(ComputeGraph અચલ &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        પ્રકારવ્યાખ્યા પૂર્ણાંક ગાંઠ_type;

        સંરચના ગાંઠ_enumerator {
        ખાનગી:
            friend સંરચના SccAdaptor;
            પૂર્ણાંક current, end;
            ગાંઠ_enumerator(પૂર્ણાંક current, પૂર્ણાંક end) : current(current), end(end) {}

        જાહેર:

            bool finished() અચલ { પાછા current == end; }
            ગાંઠ_type next() {
                log_assert(!finished());
                ગાંઠ_type result = current;
                ++current;
                પાછા result;
            }
        };

        ગાંઠ_enumerator enumerate_nodes() {
            પાછા ગાંઠ_enumerator(0, GetSize(indices_));
        }


        સંરચના successor_enumerator {
        ખાનગી:
            friend સંરચના SccAdaptor;
            std::vector<પૂર્ણાંક>::const_iterator current, end;
            successor_enumerator(std::vector<પૂર્ણાંક>::const_iterator current, std::vector<પૂર્ણાંક>::const_iterator end) :
                current(current), end(end) {}

        જાહેર:
            bool finished() અચલ { પાછા current == end; }
            ગાંઠ_type next() {
                log_assert(!finished());
                ગાંઠ_type result = *current;
                ++current;
                પાછા result;
            }
        };

        successor_enumerator enumerate_successors(પૂર્ણાંક index) અચલ {
            auto અચલ &ref = graph_[index];
            પાછા successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        પૂર્ણાંક &dfs_index(ગાંઠ_type અચલ &ગાંઠ) { પાછા indices_[ગાંઠ]; }

        std::vector<પૂર્ણાંક> અચલ &dfs_indices() { પાછા indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
