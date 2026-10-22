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

#include <tuple>
#include "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

template<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename Key = std::tuple<> // Stable keys to refer to nodes
>
sa_nracanaa ComputeGraph
{
    sa_nracanaa Ref;
private:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    sa_nracanaa Node {
        ကိန်းပြည့် fn_index;
        ကိန်းပြည့် arg_offset;
        ကိန်းပြည့် arg_count;
        Attr attr;

        Node(ကိန်းပြည့် fn_index, Attr &&attr, ကိန်းပြည့် arg_offset, ကိန်းပြည့် arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(std::move(attr)) {}

        Node(ကိန်းပြည့် fn_index, Attr const &attr, ကိန်းပြည့် arg_offset, ကိန်းပြည့် arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(attr) {}
    };


    std::vector<Node> nodes;
    std::vector<ကိန်းပြည့်> args;
    dict<Key, ကိန်းပြည့်> keys_;
    dict<ကိန်းပြည့်, SparseAttr> sparse_attrs;

public:
    template<typename Graph>
    sa_nracanaa BaseRef
    {
    rak_sita:
        friend sa_nracanaa ComputeGraph;
        Graph *graph_;
        ကိန်းပြည့် index_;
        BaseRef(Graph *graph, ကိန်းပြည့် index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        void check() const { log_assert(index_ < graph_->size()); }

        Node const &deref() const { check(); ပြန်ပေး graph_->nodes[index_]; }

    public:
        ComputeGraph const &graph() const { ပြန်ပေး graph_; }
        ကိန်းပြည့် index() const { ပြန်ပေး index_; }

        ကိန်းပြည့် size() const { ပြန်ပေး deref().arg_count; }

        BaseRef arg(ကိန်းပြည့် n) const
        {
            Node const &node = deref();
            log_assert(n >= 0 && n < node.arg_count);
            ပြန်ပေး BaseRef(graph_, graph_->args[node.arg_offset + n]);
        }

        std::vector<ကိန်းပြည့်>::const_iterator arg_indices_cbegin() const
        {
            Node const &node = deref();
            ပြန်ပေး graph_->args.cbegin() + node.arg_offset;
        }

        std::vector<ကိန်းပြည့်>::const_iterator arg_indices_cend() const
        {
            Node const &node = deref();
            ပြန်ပေး graph_->args.cbegin() + node.arg_offset + node.arg_count;
        }

        Fn const &function() const { ပြန်ပေး graph_->functions[deref().fn_index]; }
        Attr const &attr() const { ပြန်ပေး deref().attr; }

        bool has_sparse_attr() const { ပြန်ပေး graph_->sparse_attrs.count(index_); }

        SparseAttr const &sparse_attr() const
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            ပြန်ပေး found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph const>;

    sa_nracanaa Ref : public BaseRef<ComputeGraph>
    {
    private:
        friend sa_nracanaa ComputeGraph;
        Ref(ComputeGraph *graph, ကိန်းပြည့် index) : BaseRef<ComputeGraph>(graph, index) {}
        Node &deref() const { this->check(); ပြန်ပေး this->graph_->nodes[this->index_]; }

    public:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        void set_function(Fn const &function) const
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() const { ပြန်ပေး deref().attr; }

        void append_arg(ConstRef arg) const
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        void append_arg(ကိန်းပြည့် arg) const
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            Node &node = deref();
            အကယ်၍ (node.arg_offset + node.arg_count != GetSize(this->graph_->args))
                move_args(node);
            this->graph_->args.push_back(arg);
            node.arg_count++;
        }

        operator ConstRef() const
        {
            ပြန်ပေး ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() const
        {
            ပြန်ပေး this->graph_->sparse_attrs[this->index_];
        }

        void clear_sparse_attr() const
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        void assign_key(Key const &key) const
        {
            this->graph_->keys_.emplace(key, this->index_);
        }

    private:
        void move_args(Node &node) const
        {
            auto &args = this->graph_->args;
            ကိန်းပြည့် old_offset = node.arg_offset;
            node.arg_offset = GetSize(args);
            အတွက် (ကိန်းပြည့် i = 0; i != node.arg_count; ++i)
                args.push_back(args[old_offset + i]);
        }

    };

    bool has_key(Key const &key) const
    {
        ပြန်ပေး keys_.count(key);
    }

    dict<Key, ကိန်းပြည့်> const &keys() const
    {
        ပြန်ပေး keys_;
    }

    ConstRef operator()(Key const &key) const
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        ပြန်ပေး (*this)[it->second];
    }

    Ref operator()(Key const &key)
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        ပြန်ပေး (*this)[it->second];
    }

    ကိန်းပြည့် size() const { ပြန်ပေး GetSize(nodes); }

    ConstRef operator[](ကိန်းပြည့် index) const { ပြန်ပေး ConstRef(this, index); }
    Ref operator[](ကိန်းပြည့် index) { ပြန်ပေး Ref(this, index); }

    Ref add(Fn const &function, Attr &&attr)
    {
        ကိန်းပြည့် index = GetSize(nodes);
        ကိန်းပြည့် fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        ပြန်ပေး Ref(this, index);
    }

    Ref add(Fn const &function, Attr const &attr)
    {
        ကိန်းပြည့် index = GetSize(nodes);
        ကိန်းပြည့် fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        ပြန်ပေး Ref(this, index);
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T &&args)
    {
        Ref added = add(function, attr);
        အတွက် (auto arg : args)
            added.append_arg(arg);
        ပြန်ပေး added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        အတွက် (auto arg : args)
            added.append_arg(arg);
        ပြန်ပေး added;
    }

    Ref add(Fn const &function, Attr const &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        အတွက် (auto arg : args)
            added.append_arg(arg);
        ပြန်ပေး added;
    }

    Ref add(Fn const &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        အတွက် (auto arg : args)
            added.append_arg(arg);
        ပြန်ပေး added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        အတွက် (; begin != end; ++begin)
            added.append_arg(*begin);
        ပြန်ပေး added;
    }

    void compact_args()
    {
        std::vector<ကိန်းပြည့်> new_args;
        အတွက် (auto &node : nodes)
        {
            ကိန်းပြည့် new_offset = GetSize(new_args);
            အတွက် (ကိန်းပြည့် i = 0; i < node.arg_count; i++)
                new_args.push_back(args[node.arg_offset + i]);
            node.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    void permute(std::vector<ကိန်းပြည့်> const &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<ကိန်းပြည့်> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        အတွက် (ကိန်းပြည့် i = 0; i < GetSize(perm); ++i)
        {
            ကိန်းပြည့် j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    void permute(std::vector<ကိန်းပြည့်> const &perm, std::vector<ကိန်းပြည့်> const &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<Node> new_nodes;
        new_nodes.reserve(perm.size());
        dict<ကိန်းပြည့်, SparseAttr> new_sparse_attrs;
        အတွက် (ကိန်းပြည့် i : perm)
        {
            ကိန်းပြည့် j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            အကယ်၍ (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        အတွက် (ကိန်းပြည့် &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        အတွက် (auto &key : keys_)
        {
            log_assert(key.second < GetSize(inv_perm));
            log_assert(inv_perm[key.second] >= 0);
            key.second = inv_perm[key.second];
        }
    }

    sa_nracanaa SccAdaptor
    {
    private:
        ComputeGraph const &graph_;
        std::vector<ကိန်းပြည့်> indices_;
    public:
        SccAdaptor(ComputeGraph const &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        typedef ကိန်းပြည့် node_type;

        sa_nracanaa node_enumerator {
        private:
            friend sa_nracanaa SccAdaptor;
            ကိန်းပြည့် current, end;
            node_enumerator(ကိန်းပြည့် current, ကိန်းပြည့် end) : current(current), end(end) {}

        public:

            bool finished() const { ပြန်ပေး current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = current;
                ++current;
                ပြန်ပေး result;
            }
        };

        node_enumerator enumerate_nodes() {
            ပြန်ပေး node_enumerator(0, GetSize(indices_));
        }


        sa_nracanaa successor_enumerator {
        private:
            friend sa_nracanaa SccAdaptor;
            std::vector<ကိန်းပြည့်>::const_iterator current, end;
            successor_enumerator(std::vector<ကိန်းပြည့်>::const_iterator current, std::vector<ကိန်းပြည့်>::const_iterator end) :
                current(current), end(end) {}

        public:
            bool finished() const { ပြန်ပေး current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = *current;
                ++current;
                ပြန်ပေး result;
            }
        };

        successor_enumerator enumerate_successors(ကိန်းပြည့် index) const {
            auto const &ref = graph_[index];
            ပြန်ပေး successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        ကိန်းပြည့် &dfs_index(node_type const &node) { ပြန်ပေး indices_[node]; }

        std::vector<ကိန်းပြည့်> const &dfs_indices() { ပြန်ပေး indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
