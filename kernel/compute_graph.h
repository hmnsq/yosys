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
        整数 fn_index;
        整数 arg_offset;
        整数 arg_count;
        Attr attr;

        Node(整数 fn_index, Attr &&attr, 整数 arg_offset, 整数 arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(std::move(attr)) {}

        Node(整数 fn_index, Attr const &attr, 整数 arg_offset, 整数 arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(attr) {}
    };


    std::vector<Node> nodes;
    std::vector<整数> args;
    dict<Key, 整数> keys_;
    dict<整数, SparseAttr> sparse_attrs;

public:
    template<typename Graph>
    struct BaseRef
    {
    protected:
        friend struct ComputeGraph;
        Graph *graph_;
        整数 index_;
        BaseRef(Graph *graph, 整数 index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        void check() const { log_assert(index_ < graph_->size()); }

        Node const &deref() const { check(); 返回 graph_->nodes[index_]; }

    public:
        ComputeGraph const &graph() const { 返回 graph_; }
        整数 index() const { 返回 index_; }

        整数 size() const { 返回 deref().arg_count; }

        BaseRef arg(整数 n) const
        {
            Node const &node = deref();
            log_assert(n >= 0 && n < node.arg_count);
            返回 BaseRef(graph_, graph_->args[node.arg_offset + n]);
        }

        std::vector<整数>::const_iterator arg_indices_cbegin() const
        {
            Node const &node = deref();
            返回 graph_->args.cbegin() + node.arg_offset;
        }

        std::vector<整数>::const_iterator arg_indices_cend() const
        {
            Node const &node = deref();
            返回 graph_->args.cbegin() + node.arg_offset + node.arg_count;
        }

        Fn const &function() const { 返回 graph_->functions[deref().fn_index]; }
        Attr const &attr() const { 返回 deref().attr; }

        bool has_sparse_attr() const { 返回 graph_->sparse_attrs.count(index_); }

        SparseAttr const &sparse_attr() const
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            返回 found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph const>;

    struct Ref : public BaseRef<ComputeGraph>
    {
    private:
        friend struct ComputeGraph;
        Ref(ComputeGraph *graph, 整数 index) : BaseRef<ComputeGraph>(graph, index) {}
        Node &deref() const { this->check(); 返回 this->graph_->nodes[this->index_]; }

    public:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        void set_function(Fn const &function) const
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() const { 返回 deref().attr; }

        void append_arg(ConstRef arg) const
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        void append_arg(整数 arg) const
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            Node &node = deref();
            如果 (node.arg_offset + node.arg_count != GetSize(this->graph_->args))
                move_args(node);
            this->graph_->args.push_back(arg);
            node.arg_count++;
        }

        operator ConstRef() const
        {
            返回 ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() const
        {
            返回 this->graph_->sparse_attrs[this->index_];
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
            整数 old_offset = node.arg_offset;
            node.arg_offset = GetSize(args);
            为 (整数 i = 0; i != node.arg_count; ++i)
                args.push_back(args[old_offset + i]);
        }

    };

    bool has_key(Key const &key) const
    {
        返回 keys_.count(key);
    }

    dict<Key, 整数> const &keys() const
    {
        返回 keys_;
    }

    ConstRef operator()(Key const &key) const
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        返回 (*this)[it->second];
    }

    Ref operator()(Key const &key)
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        返回 (*this)[it->second];
    }

    整数 size() const { 返回 GetSize(nodes); }

    ConstRef operator[](整数 index) const { 返回 ConstRef(this, index); }
    Ref operator[](整数 index) { 返回 Ref(this, index); }

    Ref add(Fn const &function, Attr &&attr)
    {
        整数 index = GetSize(nodes);
        整数 fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        返回 Ref(this, index);
    }

    Ref add(Fn const &function, Attr const &attr)
    {
        整数 index = GetSize(nodes);
        整数 fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        返回 Ref(this, index);
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T &&args)
    {
        Ref added = add(function, attr);
        为 (auto arg : args)
            added.append_arg(arg);
        返回 added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        为 (auto arg : args)
            added.append_arg(arg);
        返回 added;
    }

    Ref add(Fn const &function, Attr const &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        为 (auto arg : args)
            added.append_arg(arg);
        返回 added;
    }

    Ref add(Fn const &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        为 (auto arg : args)
            added.append_arg(arg);
        返回 added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        为 (; begin != end; ++begin)
            added.append_arg(*begin);
        返回 added;
    }

    void compact_args()
    {
        std::vector<整数> new_args;
        为 (auto &node : nodes)
        {
            整数 new_offset = GetSize(new_args);
            为 (整数 i = 0; i < node.arg_count; i++)
                new_args.push_back(args[node.arg_offset + i]);
            node.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    void permute(std::vector<整数> const &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<整数> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        为 (整数 i = 0; i < GetSize(perm); ++i)
        {
            整数 j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    void permute(std::vector<整数> const &perm, std::vector<整数> const &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<Node> new_nodes;
        new_nodes.reserve(perm.size());
        dict<整数, SparseAttr> new_sparse_attrs;
        为 (整数 i : perm)
        {
            整数 j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            如果 (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        为 (整数 &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        为 (auto &key : keys_)
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
        std::vector<整数> indices_;
    public:
        SccAdaptor(ComputeGraph const &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        typedef 整数 node_type;

        struct node_enumerator {
        private:
            friend struct SccAdaptor;
            整数 current, end;
            node_enumerator(整数 current, 整数 end) : current(current), end(end) {}

        public:

            bool finished() const { 返回 current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = current;
                ++current;
                返回 result;
            }
        };

        node_enumerator enumerate_nodes() {
            返回 node_enumerator(0, GetSize(indices_));
        }


        struct successor_enumerator {
        private:
            friend struct SccAdaptor;
            std::vector<整数>::const_iterator current, end;
            successor_enumerator(std::vector<整数>::const_iterator current, std::vector<整数>::const_iterator end) :
                current(current), end(end) {}

        public:
            bool finished() const { 返回 current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = *current;
                ++current;
                返回 result;
            }
        };

        successor_enumerator enumerate_successors(整数 index) const {
            auto const &ref = graph_[index];
            返回 successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        整数 &dfs_index(node_type const &node) { 返回 indices_[node]; }

        std::vector<整数> const &dfs_indices() { 返回 indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
