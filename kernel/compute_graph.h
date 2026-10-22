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
struct ComputeGraph
{
    struct Ref;
private:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    struct Node {
        ціле fn_index;
        ціле arg_offset;
        ціле arg_count;
        Attr attr;

        Node(ціле fn_index, Attr &&attr, ціле arg_offset, ціле arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(std::move(attr)) {}

        Node(ціле fn_index, Attr const &attr, ціле arg_offset, ціле arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(attr) {}
    };


    std::vector<Node> nodes;
    std::vector<ціле> args;
    dict<Key, ціле> keys_;
    dict<ціле, SparseAttr> sparse_attrs;

public:
    template<typename Graph>
    struct BaseRef
    {
    protected:
        friend struct ComputeGraph;
        Graph *graph_;
        ціле index_;
        BaseRef(Graph *graph, ціле index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        void check() const { log_assert(index_ < graph_->size()); }

        Node const &deref() const { check(); повернути graph_->nodes[index_]; }

    public:
        ComputeGraph const &graph() const { повернути graph_; }
        ціле index() const { повернути index_; }

        ціле size() const { повернути deref().arg_count; }

        BaseRef arg(ціле n) const
        {
            Node const &node = deref();
            log_assert(n >= 0 && n < node.arg_count);
            повернути BaseRef(graph_, graph_->args[node.arg_offset + n]);
        }

        std::vector<ціле>::const_iterator arg_indices_cbegin() const
        {
            Node const &node = deref();
            повернути graph_->args.cbegin() + node.arg_offset;
        }

        std::vector<ціле>::const_iterator arg_indices_cend() const
        {
            Node const &node = deref();
            повернути graph_->args.cbegin() + node.arg_offset + node.arg_count;
        }

        Fn const &function() const { повернути graph_->functions[deref().fn_index]; }
        Attr const &attr() const { повернути deref().attr; }

        bool has_sparse_attr() const { повернути graph_->sparse_attrs.count(index_); }

        SparseAttr const &sparse_attr() const
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            повернути found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph const>;

    struct Ref : public BaseRef<ComputeGraph>
    {
    private:
        friend struct ComputeGraph;
        Ref(ComputeGraph *graph, ціле index) : BaseRef<ComputeGraph>(graph, index) {}
        Node &deref() const { this->check(); повернути this->graph_->nodes[this->index_]; }

    public:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        void set_function(Fn const &function) const
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() const { повернути deref().attr; }

        void append_arg(ConstRef arg) const
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        void append_arg(ціле arg) const
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            Node &node = deref();
            якщо (node.arg_offset + node.arg_count != GetSize(this->graph_->args))
                move_args(node);
            this->graph_->args.push_back(arg);
            node.arg_count++;
        }

        operator ConstRef() const
        {
            повернути ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() const
        {
            повернути this->graph_->sparse_attrs[this->index_];
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
            ціле old_offset = node.arg_offset;
            node.arg_offset = GetSize(args);
            для (ціле i = 0; i != node.arg_count; ++i)
                args.push_back(args[old_offset + i]);
        }

    };

    bool has_key(Key const &key) const
    {
        повернути keys_.count(key);
    }

    dict<Key, ціле> const &keys() const
    {
        повернути keys_;
    }

    ConstRef operator()(Key const &key) const
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        повернути (*this)[it->second];
    }

    Ref operator()(Key const &key)
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        повернути (*this)[it->second];
    }

    ціле size() const { повернути GetSize(nodes); }

    ConstRef operator[](ціле index) const { повернути ConstRef(this, index); }
    Ref operator[](ціле index) { повернути Ref(this, index); }

    Ref add(Fn const &function, Attr &&attr)
    {
        ціле index = GetSize(nodes);
        ціле fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        повернути Ref(this, index);
    }

    Ref add(Fn const &function, Attr const &attr)
    {
        ціле index = GetSize(nodes);
        ціле fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        повернути Ref(this, index);
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T &&args)
    {
        Ref added = add(function, attr);
        для (auto arg : args)
            added.append_arg(arg);
        повернути added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        для (auto arg : args)
            added.append_arg(arg);
        повернути added;
    }

    Ref add(Fn const &function, Attr const &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        для (auto arg : args)
            added.append_arg(arg);
        повернути added;
    }

    Ref add(Fn const &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        для (auto arg : args)
            added.append_arg(arg);
        повернути added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        для (; begin != end; ++begin)
            added.append_arg(*begin);
        повернути added;
    }

    void compact_args()
    {
        std::vector<ціле> new_args;
        для (auto &node : nodes)
        {
            ціле new_offset = GetSize(new_args);
            для (ціле i = 0; i < node.arg_count; i++)
                new_args.push_back(args[node.arg_offset + i]);
            node.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    void permute(std::vector<ціле> const &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<ціле> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        для (ціле i = 0; i < GetSize(perm); ++i)
        {
            ціле j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    void permute(std::vector<ціле> const &perm, std::vector<ціле> const &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<Node> new_nodes;
        new_nodes.reserve(perm.size());
        dict<ціле, SparseAttr> new_sparse_attrs;
        для (ціле i : perm)
        {
            ціле j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            якщо (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        для (ціле &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        для (auto &key : keys_)
        {
            log_assert(key.second < GetSize(inv_perm));
            log_assert(inv_perm[key.second] >= 0);
            key.second = inv_perm[key.second];
        }
    }

    struct SccAdaptor
    {
    private:
        ComputeGraph const &graph_;
        std::vector<ціле> indices_;
    public:
        SccAdaptor(ComputeGraph const &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        typedef ціле node_type;

        struct node_enumerator {
        private:
            friend struct SccAdaptor;
            ціле current, end;
            node_enumerator(ціле current, ціле end) : current(current), end(end) {}

        public:

            bool finished() const { повернути current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = current;
                ++current;
                повернути result;
            }
        };

        node_enumerator enumerate_nodes() {
            повернути node_enumerator(0, GetSize(indices_));
        }


        struct successor_enumerator {
        private:
            friend struct SccAdaptor;
            std::vector<ціле>::const_iterator current, end;
            successor_enumerator(std::vector<ціле>::const_iterator current, std::vector<ціле>::const_iterator end) :
                current(current), end(end) {}

        public:
            bool finished() const { повернути current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = *current;
                ++current;
                повернути result;
            }
        };

        successor_enumerator enumerate_successors(ціле index) const {
            auto const &ref = graph_[index];
            повернути successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        ціле &dfs_index(node_type const &node) { повернути indices_[node]; }

        std::vector<ціле> const &dfs_indices() { повернути indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
