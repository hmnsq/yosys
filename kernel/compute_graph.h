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
        целое fn_index;
        целое arg_offset;
        целое arg_count;
        Attr attr;

        Node(целое fn_index, Attr &&attr, целое arg_offset, целое arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(std::move(attr)) {}

        Node(целое fn_index, Attr const &attr, целое arg_offset, целое arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(attr) {}
    };


    std::vector<Node> nodes;
    std::vector<целое> args;
    dict<Key, целое> keys_;
    dict<целое, SparseAttr> sparse_attrs;

public:
    template<typename Graph>
    struct BaseRef
    {
    protected:
        friend struct ComputeGraph;
        Graph *graph_;
        целое index_;
        BaseRef(Graph *graph, целое index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        void check() const { log_assert(index_ < graph_->size()); }

        Node const &deref() const { check(); вернуть graph_->nodes[index_]; }

    public:
        ComputeGraph const &graph() const { вернуть graph_; }
        целое index() const { вернуть index_; }

        целое size() const { вернуть deref().arg_count; }

        BaseRef arg(целое n) const
        {
            Node const &node = deref();
            log_assert(n >= 0 && n < node.arg_count);
            вернуть BaseRef(graph_, graph_->args[node.arg_offset + n]);
        }

        std::vector<целое>::const_iterator arg_indices_cbegin() const
        {
            Node const &node = deref();
            вернуть graph_->args.cbegin() + node.arg_offset;
        }

        std::vector<целое>::const_iterator arg_indices_cend() const
        {
            Node const &node = deref();
            вернуть graph_->args.cbegin() + node.arg_offset + node.arg_count;
        }

        Fn const &function() const { вернуть graph_->functions[deref().fn_index]; }
        Attr const &attr() const { вернуть deref().attr; }

        bool has_sparse_attr() const { вернуть graph_->sparse_attrs.count(index_); }

        SparseAttr const &sparse_attr() const
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            вернуть found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph const>;

    struct Ref : public BaseRef<ComputeGraph>
    {
    private:
        friend struct ComputeGraph;
        Ref(ComputeGraph *graph, целое index) : BaseRef<ComputeGraph>(graph, index) {}
        Node &deref() const { this->check(); вернуть this->graph_->nodes[this->index_]; }

    public:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        void set_function(Fn const &function) const
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() const { вернуть deref().attr; }

        void append_arg(ConstRef arg) const
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        void append_arg(целое arg) const
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            Node &node = deref();
            если (node.arg_offset + node.arg_count != GetSize(this->graph_->args))
                move_args(node);
            this->graph_->args.push_back(arg);
            node.arg_count++;
        }

        operator ConstRef() const
        {
            вернуть ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() const
        {
            вернуть this->graph_->sparse_attrs[this->index_];
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
            целое old_offset = node.arg_offset;
            node.arg_offset = GetSize(args);
            для (целое i = 0; i != node.arg_count; ++i)
                args.push_back(args[old_offset + i]);
        }

    };

    bool has_key(Key const &key) const
    {
        вернуть keys_.count(key);
    }

    dict<Key, целое> const &keys() const
    {
        вернуть keys_;
    }

    ConstRef operator()(Key const &key) const
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        вернуть (*this)[it->second];
    }

    Ref operator()(Key const &key)
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        вернуть (*this)[it->second];
    }

    целое size() const { вернуть GetSize(nodes); }

    ConstRef operator[](целое index) const { вернуть ConstRef(this, index); }
    Ref operator[](целое index) { вернуть Ref(this, index); }

    Ref add(Fn const &function, Attr &&attr)
    {
        целое index = GetSize(nodes);
        целое fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        вернуть Ref(this, index);
    }

    Ref add(Fn const &function, Attr const &attr)
    {
        целое index = GetSize(nodes);
        целое fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        вернуть Ref(this, index);
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T &&args)
    {
        Ref added = add(function, attr);
        для (auto arg : args)
            added.append_arg(arg);
        вернуть added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        для (auto arg : args)
            added.append_arg(arg);
        вернуть added;
    }

    Ref add(Fn const &function, Attr const &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        для (auto arg : args)
            added.append_arg(arg);
        вернуть added;
    }

    Ref add(Fn const &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        для (auto arg : args)
            added.append_arg(arg);
        вернуть added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        для (; begin != end; ++begin)
            added.append_arg(*begin);
        вернуть added;
    }

    void compact_args()
    {
        std::vector<целое> new_args;
        для (auto &node : nodes)
        {
            целое new_offset = GetSize(new_args);
            для (целое i = 0; i < node.arg_count; i++)
                new_args.push_back(args[node.arg_offset + i]);
            node.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    void permute(std::vector<целое> const &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<целое> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        для (целое i = 0; i < GetSize(perm); ++i)
        {
            целое j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    void permute(std::vector<целое> const &perm, std::vector<целое> const &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<Node> new_nodes;
        new_nodes.reserve(perm.size());
        dict<целое, SparseAttr> new_sparse_attrs;
        для (целое i : perm)
        {
            целое j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            если (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        для (целое &arg : args)
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
        std::vector<целое> indices_;
    public:
        SccAdaptor(ComputeGraph const &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        typedef целое node_type;

        struct node_enumerator {
        private:
            friend struct SccAdaptor;
            целое current, end;
            node_enumerator(целое current, целое end) : current(current), end(end) {}

        public:

            bool finished() const { вернуть current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = current;
                ++current;
                вернуть result;
            }
        };

        node_enumerator enumerate_nodes() {
            вернуть node_enumerator(0, GetSize(indices_));
        }


        struct successor_enumerator {
        private:
            friend struct SccAdaptor;
            std::vector<целое>::const_iterator current, end;
            successor_enumerator(std::vector<целое>::const_iterator current, std::vector<целое>::const_iterator end) :
                current(current), end(end) {}

        public:
            bool finished() const { вернуть current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = *current;
                ++current;
                вернуть result;
            }
        };

        successor_enumerator enumerate_successors(целое index) const {
            auto const &ref = graph_[index];
            вернуть successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        целое &dfs_index(node_type const &node) { вернуть indices_[node]; }

        std::vector<целое> const &dfs_indices() { вернуть indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
