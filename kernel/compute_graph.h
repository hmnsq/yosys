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
        số_nguyên fn_index;
        số_nguyên arg_offset;
        số_nguyên arg_count;
        Attr attr;

        Node(số_nguyên fn_index, Attr &&attr, số_nguyên arg_offset, số_nguyên arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(std::move(attr)) {}

        Node(số_nguyên fn_index, Attr const &attr, số_nguyên arg_offset, số_nguyên arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(attr) {}
    };


    std::vector<Node> nodes;
    std::vector<số_nguyên> args;
    dict<Key, số_nguyên> keys_;
    dict<số_nguyên, SparseAttr> sparse_attrs;

public:
    template<typename Graph>
    struct BaseRef
    {
    protected:
        friend struct ComputeGraph;
        Graph *graph_;
        số_nguyên index_;
        BaseRef(Graph *graph, số_nguyên index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        void check() const { log_assert(index_ < graph_->size()); }

        Node const &deref() const { check(); trả_về graph_->nodes[index_]; }

    public:
        ComputeGraph const &graph() const { trả_về graph_; }
        số_nguyên index() const { trả_về index_; }

        số_nguyên size() const { trả_về deref().arg_count; }

        BaseRef arg(số_nguyên n) const
        {
            Node const &node = deref();
            log_assert(n >= 0 && n < node.arg_count);
            trả_về BaseRef(graph_, graph_->args[node.arg_offset + n]);
        }

        std::vector<số_nguyên>::const_iterator arg_indices_cbegin() const
        {
            Node const &node = deref();
            trả_về graph_->args.cbegin() + node.arg_offset;
        }

        std::vector<số_nguyên>::const_iterator arg_indices_cend() const
        {
            Node const &node = deref();
            trả_về graph_->args.cbegin() + node.arg_offset + node.arg_count;
        }

        Fn const &function() const { trả_về graph_->functions[deref().fn_index]; }
        Attr const &attr() const { trả_về deref().attr; }

        bool has_sparse_attr() const { trả_về graph_->sparse_attrs.count(index_); }

        SparseAttr const &sparse_attr() const
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            trả_về found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph const>;

    struct Ref : public BaseRef<ComputeGraph>
    {
    private:
        friend struct ComputeGraph;
        Ref(ComputeGraph *graph, số_nguyên index) : BaseRef<ComputeGraph>(graph, index) {}
        Node &deref() const { this->check(); trả_về this->graph_->nodes[this->index_]; }

    public:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        void set_function(Fn const &function) const
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() const { trả_về deref().attr; }

        void append_arg(ConstRef arg) const
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        void append_arg(số_nguyên arg) const
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            Node &node = deref();
            nếu (node.arg_offset + node.arg_count != GetSize(this->graph_->args))
                move_args(node);
            this->graph_->args.push_back(arg);
            node.arg_count++;
        }

        operator ConstRef() const
        {
            trả_về ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() const
        {
            trả_về this->graph_->sparse_attrs[this->index_];
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
            số_nguyên old_offset = node.arg_offset;
            node.arg_offset = GetSize(args);
            cho (số_nguyên i = 0; i != node.arg_count; ++i)
                args.push_back(args[old_offset + i]);
        }

    };

    bool has_key(Key const &key) const
    {
        trả_về keys_.count(key);
    }

    dict<Key, số_nguyên> const &keys() const
    {
        trả_về keys_;
    }

    ConstRef operator()(Key const &key) const
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        trả_về (*this)[it->second];
    }

    Ref operator()(Key const &key)
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        trả_về (*this)[it->second];
    }

    số_nguyên size() const { trả_về GetSize(nodes); }

    ConstRef operator[](số_nguyên index) const { trả_về ConstRef(this, index); }
    Ref operator[](số_nguyên index) { trả_về Ref(this, index); }

    Ref add(Fn const &function, Attr &&attr)
    {
        số_nguyên index = GetSize(nodes);
        số_nguyên fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        trả_về Ref(this, index);
    }

    Ref add(Fn const &function, Attr const &attr)
    {
        số_nguyên index = GetSize(nodes);
        số_nguyên fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        trả_về Ref(this, index);
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T &&args)
    {
        Ref added = add(function, attr);
        cho (auto arg : args)
            added.append_arg(arg);
        trả_về added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        cho (auto arg : args)
            added.append_arg(arg);
        trả_về added;
    }

    Ref add(Fn const &function, Attr const &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        cho (auto arg : args)
            added.append_arg(arg);
        trả_về added;
    }

    Ref add(Fn const &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        cho (auto arg : args)
            added.append_arg(arg);
        trả_về added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        cho (; begin != end; ++begin)
            added.append_arg(*begin);
        trả_về added;
    }

    void compact_args()
    {
        std::vector<số_nguyên> new_args;
        cho (auto &node : nodes)
        {
            số_nguyên new_offset = GetSize(new_args);
            cho (số_nguyên i = 0; i < node.arg_count; i++)
                new_args.push_back(args[node.arg_offset + i]);
            node.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    void permute(std::vector<số_nguyên> const &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<số_nguyên> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        cho (số_nguyên i = 0; i < GetSize(perm); ++i)
        {
            số_nguyên j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    void permute(std::vector<số_nguyên> const &perm, std::vector<số_nguyên> const &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<Node> new_nodes;
        new_nodes.reserve(perm.size());
        dict<số_nguyên, SparseAttr> new_sparse_attrs;
        cho (số_nguyên i : perm)
        {
            số_nguyên j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            nếu (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        cho (số_nguyên &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        cho (auto &key : keys_)
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
        std::vector<số_nguyên> indices_;
    public:
        SccAdaptor(ComputeGraph const &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        typedef số_nguyên node_type;

        struct node_enumerator {
        private:
            friend struct SccAdaptor;
            số_nguyên current, end;
            node_enumerator(số_nguyên current, số_nguyên end) : current(current), end(end) {}

        public:

            bool finished() const { trả_về current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = current;
                ++current;
                trả_về result;
            }
        };

        node_enumerator enumerate_nodes() {
            trả_về node_enumerator(0, GetSize(indices_));
        }


        struct successor_enumerator {
        private:
            friend struct SccAdaptor;
            std::vector<số_nguyên>::const_iterator current, end;
            successor_enumerator(std::vector<số_nguyên>::const_iterator current, std::vector<số_nguyên>::const_iterator end) :
                current(current), end(end) {}

        public:
            bool finished() const { trả_về current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = *current;
                ++current;
                trả_về result;
            }
        };

        successor_enumerator enumerate_successors(số_nguyên index) const {
            auto const &ref = graph_[index];
            trả_về successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        số_nguyên &dfs_index(node_type const &node) { trả_về indices_[node]; }

        std::vector<số_nguyên> const &dfs_indices() { trả_về indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
