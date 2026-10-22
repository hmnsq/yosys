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

#അന്തര്ഭാവ <tuple>
#അന്തര്ഭാവ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

template<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename താക്കോൽ_2 = std::tuple<> // Stable keys to refer to nodes
>
ഘടന ComputeGraph
{
    ഘടന Ref;
സ്വകാര്യ:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    ഘടന കണ്ണി_2 {
        പൂർണ്ണസംഖ്യ fn_index;
        പൂർണ്ണസംഖ്യ arg_offset;
        പൂർണ്ണസംഖ്യ arg_എണ്ണം;
        Attr attr;

        കണ്ണി_2(പൂർണ്ണസംഖ്യ fn_index, Attr &&attr, പൂർണ്ണസംഖ്യ arg_offset, പൂർണ്ണസംഖ്യ arg_എണ്ണം = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_എണ്ണം(arg_എണ്ണം), attr(std::move(attr)) {}

        കണ്ണി_2(പൂർണ്ണസംഖ്യ fn_index, Attr സ്ഥിരം &attr, പൂർണ്ണസംഖ്യ arg_offset, പൂർണ്ണസംഖ്യ arg_എണ്ണം = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_എണ്ണം(arg_എണ്ണം), attr(attr) {}
    };


    std::vector<കണ്ണി_2> nodes;
    std::vector<പൂർണ്ണസംഖ്യ> args;
    dict<താക്കോൽ_2, പൂർണ്ണസംഖ്യ> താക്കോലുകൾ;
    dict<പൂർണ്ണസംഖ്യ, SparseAttr> sparse_attrs;

പൊതു:
    template<typename Graph>
    ഘടന BaseRef
    {
    സംരക്ഷിത:
        friend ഘടന ComputeGraph;
        Graph *graph_;
        പൂർണ്ണസംഖ്യ index_;
        BaseRef(Graph *graph, പൂർണ്ണസംഖ്യ index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        ശൂന്യം check() സ്ഥിരം { log_assert(index_ < graph_->size()); }

        കണ്ണി_2 സ്ഥിരം &deref() സ്ഥിരം { check(); തിരികെ graph_->nodes[index_]; }

    പൊതു:
        ComputeGraph സ്ഥിരം &graph() സ്ഥിരം { തിരികെ graph_; }
        പൂർണ്ണസംഖ്യ index() സ്ഥിരം { തിരികെ index_; }

        പൂർണ്ണസംഖ്യ size() സ്ഥിരം { തിരികെ deref().arg_എണ്ണം; }

        BaseRef arg(പൂർണ്ണസംഖ്യ n) സ്ഥിരം
        {
            കണ്ണി_2 സ്ഥിരം &കണ്ണി = deref();
            log_assert(n >= 0 && n < കണ്ണി.arg_എണ്ണം);
            തിരികെ BaseRef(graph_, graph_->args[കണ്ണി.arg_offset + n]);
        }

        std::vector<പൂർണ്ണസംഖ്യ>::const_iterator arg_indices_cbegin() സ്ഥിരം
        {
            കണ്ണി_2 സ്ഥിരം &കണ്ണി = deref();
            തിരികെ graph_->args.cbegin() + കണ്ണി.arg_offset;
        }

        std::vector<പൂർണ്ണസംഖ്യ>::const_iterator arg_indices_cend() സ്ഥിരം
        {
            കണ്ണി_2 സ്ഥിരം &കണ്ണി = deref();
            തിരികെ graph_->args.cbegin() + കണ്ണി.arg_offset + കണ്ണി.arg_എണ്ണം;
        }

        Fn സ്ഥിരം &function() സ്ഥിരം { തിരികെ graph_->functions[deref().fn_index]; }
        Attr സ്ഥിരം &attr() സ്ഥിരം { തിരികെ deref().attr; }

        bool has_sparse_attr() സ്ഥിരം { തിരികെ graph_->sparse_attrs.എണ്ണം(index_); }

        SparseAttr സ്ഥിരം &sparse_attr() സ്ഥിരം
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            തിരികെ found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph സ്ഥിരം>;

    ഘടന Ref : പൊതു BaseRef<ComputeGraph>
    {
    സ്വകാര്യ:
        friend ഘടന ComputeGraph;
        Ref(ComputeGraph *graph, പൂർണ്ണസംഖ്യ index) : BaseRef<ComputeGraph>(graph, index) {}
        കണ്ണി_2 &deref() സ്ഥിരം { this->check(); തിരികെ this->graph_->nodes[this->index_]; }

    പൊതു:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        ശൂന്യം set_function(Fn സ്ഥിരം &function) സ്ഥിരം
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() സ്ഥിരം { തിരികെ deref().attr; }

        ശൂന്യം append_arg(ConstRef arg) സ്ഥിരം
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        ശൂന്യം append_arg(പൂർണ്ണസംഖ്യ arg) സ്ഥിരം
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            കണ്ണി_2 &കണ്ണി = deref();
            എങ്കിൽ (കണ്ണി.arg_offset + കണ്ണി.arg_എണ്ണം != GetSize(this->graph_->args))
                move_args(കണ്ണി);
            this->graph_->args.തള്ളുക_back(arg);
            കണ്ണി.arg_എണ്ണം++;
        }

        operator ConstRef() സ്ഥിരം
        {
            തിരികെ ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() സ്ഥിരം
        {
            തിരികെ this->graph_->sparse_attrs[this->index_];
        }

        ശൂന്യം clear_sparse_attr() സ്ഥിരം
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        ശൂന്യം assign_താക്കോൽ(താക്കോൽ_2 സ്ഥിരം &താക്കോൽ) സ്ഥിരം
        {
            this->graph_->താക്കോലുകൾ.emplace(താക്കോൽ, this->index_);
        }

    സ്വകാര്യ:
        ശൂന്യം move_args(കണ്ണി_2 &കണ്ണി) സ്ഥിരം
        {
            auto &args = this->graph_->args;
            പൂർണ്ണസംഖ്യ old_offset = കണ്ണി.arg_offset;
            കണ്ണി.arg_offset = GetSize(args);
            വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i != കണ്ണി.arg_എണ്ണം; ++i)
                args.തള്ളുക_back(args[old_offset + i]);
        }

    };

    bool has_താക്കോൽ(താക്കോൽ_2 സ്ഥിരം &താക്കോൽ) സ്ഥിരം
    {
        തിരികെ താക്കോലുകൾ.എണ്ണം(താക്കോൽ);
    }

    dict<താക്കോൽ_2, പൂർണ്ണസംഖ്യ> സ്ഥിരം &താക്കോലുകൾ_2() സ്ഥിരം
    {
        തിരികെ താക്കോലുകൾ;
    }

    ConstRef operator()(താക്കോൽ_2 സ്ഥിരം &താക്കോൽ) സ്ഥിരം
    {
        auto it = താക്കോലുകൾ.find(താക്കോൽ);
        log_assert(it != താക്കോലുകൾ.end());
        തിരികെ (*this)[it->second];
    }

    Ref operator()(താക്കോൽ_2 സ്ഥിരം &താക്കോൽ)
    {
        auto it = താക്കോലുകൾ.find(താക്കോൽ);
        log_assert(it != താക്കോലുകൾ.end());
        തിരികെ (*this)[it->second];
    }

    പൂർണ്ണസംഖ്യ size() സ്ഥിരം { തിരികെ GetSize(nodes); }

    ConstRef operator[](പൂർണ്ണസംഖ്യ index) സ്ഥിരം { തിരികെ ConstRef(this, index); }
    Ref operator[](പൂർണ്ണസംഖ്യ index) { തിരികെ Ref(this, index); }

    Ref add(Fn സ്ഥിരം &function, Attr &&attr)
    {
        പൂർണ്ണസംഖ്യ index = GetSize(nodes);
        പൂർണ്ണസംഖ്യ fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        തിരികെ Ref(this, index);
    }

    Ref add(Fn സ്ഥിരം &function, Attr സ്ഥിരം &attr)
    {
        പൂർണ്ണസംഖ്യ index = GetSize(nodes);
        പൂർണ്ണസംഖ്യ fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        തിരികെ Ref(this, index);
    }

    template<typename T>
    Ref add(Fn സ്ഥിരം &function, Attr സ്ഥിരം &attr, T &&args)
    {
        Ref added = add(function, attr);
        വേണ്ടി (auto arg : args)
            added.append_arg(arg);
        തിരികെ added;
    }

    template<typename T>
    Ref add(Fn സ്ഥിരം &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        വേണ്ടി (auto arg : args)
            added.append_arg(arg);
        തിരികെ added;
    }

    Ref add(Fn സ്ഥിരം &function, Attr സ്ഥിരം &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        വേണ്ടി (auto arg : args)
            added.append_arg(arg);
        തിരികെ added;
    }

    Ref add(Fn സ്ഥിരം &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        വേണ്ടി (auto arg : args)
            added.append_arg(arg);
        തിരികെ added;
    }

    template<typename T>
    Ref add(Fn സ്ഥിരം &function, Attr സ്ഥിരം &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        വേണ്ടി (; begin != end; ++begin)
            added.append_arg(*begin);
        തിരികെ added;
    }

    ശൂന്യം compact_args()
    {
        std::vector<പൂർണ്ണസംഖ്യ> new_args;
        വേണ്ടി (auto &കണ്ണി : nodes)
        {
            പൂർണ്ണസംഖ്യ new_offset = GetSize(new_args);
            വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < കണ്ണി.arg_എണ്ണം; i++)
                new_args.തള്ളുക_back(args[കണ്ണി.arg_offset + i]);
            കണ്ണി.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    ശൂന്യം permute(std::vector<പൂർണ്ണസംഖ്യ> സ്ഥിരം &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<പൂർണ്ണസംഖ്യ> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(perm); ++i)
        {
            പൂർണ്ണസംഖ്യ j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    ശൂന്യം permute(std::vector<പൂർണ്ണസംഖ്യ> സ്ഥിരം &perm, std::vector<പൂർണ്ണസംഖ്യ> സ്ഥിരം &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<കണ്ണി_2> new_nodes;
        new_nodes.reserve(perm.size());
        dict<പൂർണ്ണസംഖ്യ, SparseAttr> new_sparse_attrs;
        വേണ്ടി (പൂർണ്ണസംഖ്യ i : perm)
        {
            പൂർണ്ണസംഖ്യ j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            എങ്കിൽ (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        വേണ്ടി (പൂർണ്ണസംഖ്യ &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        വേണ്ടി (auto &താക്കോൽ : താക്കോലുകൾ)
        {
            log_assert(താക്കോൽ.second < GetSize(inv_perm));
            log_assert(inv_perm[താക്കോൽ.second] >= 0);
            താക്കോൽ.second = inv_perm[താക്കോൽ.second];
        }
    }

    ഘടന SccAdaptor
    {
    സ്വകാര്യ:
        ComputeGraph സ്ഥിരം &graph_;
        std::vector<പൂർണ്ണസംഖ്യ> indices_;
    പൊതു:
        SccAdaptor(ComputeGraph സ്ഥിരം &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        തരനിർവചനം പൂർണ്ണസംഖ്യ കണ്ണി_type;

        ഘടന കണ്ണി_enumerator {
        സ്വകാര്യ:
            friend ഘടന SccAdaptor;
            പൂർണ്ണസംഖ്യ current, end;
            കണ്ണി_enumerator(പൂർണ്ണസംഖ്യ current, പൂർണ്ണസംഖ്യ end) : current(current), end(end) {}

        പൊതു:

            bool finished() സ്ഥിരം { തിരികെ current == end; }
            കണ്ണി_type next() {
                log_assert(!finished());
                കണ്ണി_type result = current;
                ++current;
                തിരികെ result;
            }
        };

        കണ്ണി_enumerator enumerate_nodes() {
            തിരികെ കണ്ണി_enumerator(0, GetSize(indices_));
        }


        ഘടന successor_enumerator {
        സ്വകാര്യ:
            friend ഘടന SccAdaptor;
            std::vector<പൂർണ്ണസംഖ്യ>::const_iterator current, end;
            successor_enumerator(std::vector<പൂർണ്ണസംഖ്യ>::const_iterator current, std::vector<പൂർണ്ണസംഖ്യ>::const_iterator end) :
                current(current), end(end) {}

        പൊതു:
            bool finished() സ്ഥിരം { തിരികെ current == end; }
            കണ്ണി_type next() {
                log_assert(!finished());
                കണ്ണി_type result = *current;
                ++current;
                തിരികെ result;
            }
        };

        successor_enumerator enumerate_successors(പൂർണ്ണസംഖ്യ index) സ്ഥിരം {
            auto സ്ഥിരം &ref = graph_[index];
            തിരികെ successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        പൂർണ്ണസംഖ്യ &dfs_index(കണ്ണി_type സ്ഥിരം &കണ്ണി) { തിരികെ indices_[കണ്ണി]; }

        std::vector<പൂർണ്ണസംഖ്യ> സ്ഥിരം &dfs_indices() { തിരികെ indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
