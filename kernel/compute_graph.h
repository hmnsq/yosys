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

#அந்தர்பாவ <tuple>
#அந்தர்பாவ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

template<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename சாவி_2 = std::tuple<> // Stable keys to refer to nodes
>
கட்டமைப்பு ComputeGraph
{
    கட்டமைப்பு Ref;
தனிப்பட்ட:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    கட்டமைப்பு முனை_2 {
        முழு fn_index;
        முழு arg_offset;
        முழு arg_மொத்தம்;
        Attr attr;

        முனை_2(முழு fn_index, Attr &&attr, முழு arg_offset, முழு arg_மொத்தம் = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_மொத்தம்(arg_மொத்தம்), attr(std::move(attr)) {}

        முனை_2(முழு fn_index, Attr மாறிலி &attr, முழு arg_offset, முழு arg_மொத்தம் = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_மொத்தம்(arg_மொத்தம்), attr(attr) {}
    };


    std::vector<முனை_2> nodes;
    std::vector<முழு> args;
    dict<சாவி_2, முழு> சாவிகள்;
    dict<முழு, SparseAttr> sparse_attrs;

பொது:
    template<typename Graph>
    கட்டமைப்பு BaseRef
    {
    பாதுகாக்கப்பட்ட:
        friend கட்டமைப்பு ComputeGraph;
        Graph *graph_;
        முழு index_;
        BaseRef(Graph *graph, முழு index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        வெறுமை check() மாறிலி { log_assert(index_ < graph_->size()); }

        முனை_2 மாறிலி &deref() மாறிலி { check(); திரும்பு graph_->nodes[index_]; }

    பொது:
        ComputeGraph மாறிலி &graph() மாறிலி { திரும்பு graph_; }
        முழு index() மாறிலி { திரும்பு index_; }

        முழு size() மாறிலி { திரும்பு deref().arg_மொத்தம்; }

        BaseRef arg(முழு n) மாறிலி
        {
            முனை_2 மாறிலி &முனை = deref();
            log_assert(n >= 0 && n < முனை.arg_மொத்தம்);
            திரும்பு BaseRef(graph_, graph_->args[முனை.arg_offset + n]);
        }

        std::vector<முழு>::const_iterator arg_indices_cbegin() மாறிலி
        {
            முனை_2 மாறிலி &முனை = deref();
            திரும்பு graph_->args.cbegin() + முனை.arg_offset;
        }

        std::vector<முழு>::const_iterator arg_indices_cend() மாறிலி
        {
            முனை_2 மாறிலி &முனை = deref();
            திரும்பு graph_->args.cbegin() + முனை.arg_offset + முனை.arg_மொத்தம்;
        }

        Fn மாறிலி &function() மாறிலி { திரும்பு graph_->functions[deref().fn_index]; }
        Attr மாறிலி &attr() மாறிலி { திரும்பு deref().attr; }

        bool has_sparse_attr() மாறிலி { திரும்பு graph_->sparse_attrs.மொத்தம்(index_); }

        SparseAttr மாறிலி &sparse_attr() மாறிலி
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            திரும்பு found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph மாறிலி>;

    கட்டமைப்பு Ref : பொது BaseRef<ComputeGraph>
    {
    தனிப்பட்ட:
        friend கட்டமைப்பு ComputeGraph;
        Ref(ComputeGraph *graph, முழு index) : BaseRef<ComputeGraph>(graph, index) {}
        முனை_2 &deref() மாறிலி { this->check(); திரும்பு this->graph_->nodes[this->index_]; }

    பொது:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        வெறுமை set_function(Fn மாறிலி &function) மாறிலி
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() மாறிலி { திரும்பு deref().attr; }

        வெறுமை append_arg(ConstRef arg) மாறிலி
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        வெறுமை append_arg(முழு arg) மாறிலி
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            முனை_2 &முனை = deref();
            எனில் (முனை.arg_offset + முனை.arg_மொத்தம் != GetSize(this->graph_->args))
                move_args(முனை);
            this->graph_->args.தள்ளு_back(arg);
            முனை.arg_மொத்தம்++;
        }

        operator ConstRef() மாறிலி
        {
            திரும்பு ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() மாறிலி
        {
            திரும்பு this->graph_->sparse_attrs[this->index_];
        }

        வெறுமை clear_sparse_attr() மாறிலி
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        வெறுமை assign_சாவி(சாவி_2 மாறிலி &சாவி) மாறிலி
        {
            this->graph_->சாவிகள்.emplace(சாவி, this->index_);
        }

    தனிப்பட்ட:
        வெறுமை move_args(முனை_2 &முனை) மாறிலி
        {
            auto &args = this->graph_->args;
            முழு old_offset = முனை.arg_offset;
            முனை.arg_offset = GetSize(args);
            க்கு (முழு i = 0; i != முனை.arg_மொத்தம்; ++i)
                args.தள்ளு_back(args[old_offset + i]);
        }

    };

    bool has_சாவி(சாவி_2 மாறிலி &சாவி) மாறிலி
    {
        திரும்பு சாவிகள்.மொத்தம்(சாவி);
    }

    dict<சாவி_2, முழு> மாறிலி &சாவிகள்_2() மாறிலி
    {
        திரும்பு சாவிகள்;
    }

    ConstRef operator()(சாவி_2 மாறிலி &சாவி) மாறிலி
    {
        auto it = சாவிகள்.find(சாவி);
        log_assert(it != சாவிகள்.end());
        திரும்பு (*this)[it->second];
    }

    Ref operator()(சாவி_2 மாறிலி &சாவி)
    {
        auto it = சாவிகள்.find(சாவி);
        log_assert(it != சாவிகள்.end());
        திரும்பு (*this)[it->second];
    }

    முழு size() மாறிலி { திரும்பு GetSize(nodes); }

    ConstRef operator[](முழு index) மாறிலி { திரும்பு ConstRef(this, index); }
    Ref operator[](முழு index) { திரும்பு Ref(this, index); }

    Ref add(Fn மாறிலி &function, Attr &&attr)
    {
        முழு index = GetSize(nodes);
        முழு fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        திரும்பு Ref(this, index);
    }

    Ref add(Fn மாறிலி &function, Attr மாறிலி &attr)
    {
        முழு index = GetSize(nodes);
        முழு fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        திரும்பு Ref(this, index);
    }

    template<typename T>
    Ref add(Fn மாறிலி &function, Attr மாறிலி &attr, T &&args)
    {
        Ref added = add(function, attr);
        க்கு (auto arg : args)
            added.append_arg(arg);
        திரும்பு added;
    }

    template<typename T>
    Ref add(Fn மாறிலி &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        க்கு (auto arg : args)
            added.append_arg(arg);
        திரும்பு added;
    }

    Ref add(Fn மாறிலி &function, Attr மாறிலி &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        க்கு (auto arg : args)
            added.append_arg(arg);
        திரும்பு added;
    }

    Ref add(Fn மாறிலி &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        க்கு (auto arg : args)
            added.append_arg(arg);
        திரும்பு added;
    }

    template<typename T>
    Ref add(Fn மாறிலி &function, Attr மாறிலி &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        க்கு (; begin != end; ++begin)
            added.append_arg(*begin);
        திரும்பு added;
    }

    வெறுமை compact_args()
    {
        std::vector<முழு> new_args;
        க்கு (auto &முனை : nodes)
        {
            முழு new_offset = GetSize(new_args);
            க்கு (முழு i = 0; i < முனை.arg_மொத்தம்; i++)
                new_args.தள்ளு_back(args[முனை.arg_offset + i]);
            முனை.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    வெறுமை permute(std::vector<முழு> மாறிலி &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<முழு> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        க்கு (முழு i = 0; i < GetSize(perm); ++i)
        {
            முழு j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    வெறுமை permute(std::vector<முழு> மாறிலி &perm, std::vector<முழு> மாறிலி &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<முனை_2> new_nodes;
        new_nodes.reserve(perm.size());
        dict<முழு, SparseAttr> new_sparse_attrs;
        க்கு (முழு i : perm)
        {
            முழு j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            எனில் (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        க்கு (முழு &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        க்கு (auto &சாவி : சாவிகள்)
        {
            log_assert(சாவி.second < GetSize(inv_perm));
            log_assert(inv_perm[சாவி.second] >= 0);
            சாவி.second = inv_perm[சாவி.second];
        }
    }

    கட்டமைப்பு SccAdaptor
    {
    தனிப்பட்ட:
        ComputeGraph மாறிலி &graph_;
        std::vector<முழு> indices_;
    பொது:
        SccAdaptor(ComputeGraph மாறிலி &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        வகைவரை முழு முனை_type;

        கட்டமைப்பு முனை_enumerator {
        தனிப்பட்ட:
            friend கட்டமைப்பு SccAdaptor;
            முழு current, end;
            முனை_enumerator(முழு current, முழு end) : current(current), end(end) {}

        பொது:

            bool finished() மாறிலி { திரும்பு current == end; }
            முனை_type next() {
                log_assert(!finished());
                முனை_type result = current;
                ++current;
                திரும்பு result;
            }
        };

        முனை_enumerator enumerate_nodes() {
            திரும்பு முனை_enumerator(0, GetSize(indices_));
        }


        கட்டமைப்பு successor_enumerator {
        தனிப்பட்ட:
            friend கட்டமைப்பு SccAdaptor;
            std::vector<முழு>::const_iterator current, end;
            successor_enumerator(std::vector<முழு>::const_iterator current, std::vector<முழு>::const_iterator end) :
                current(current), end(end) {}

        பொது:
            bool finished() மாறிலி { திரும்பு current == end; }
            முனை_type next() {
                log_assert(!finished());
                முனை_type result = *current;
                ++current;
                திரும்பு result;
            }
        };

        successor_enumerator enumerate_successors(முழு index) மாறிலி {
            auto மாறிலி &ref = graph_[index];
            திரும்பு successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        முழு &dfs_index(முனை_type மாறிலி &முனை) { திரும்பு indices_[முனை]; }

        std::vector<முழு> மாறிலி &dfs_indices() { திரும்பு indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
