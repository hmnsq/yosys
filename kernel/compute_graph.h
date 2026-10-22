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
        puur_naa_nka fn_index;
        puur_naa_nka arg_offset;
        puur_naa_nka arg_count;
        Attr attr;

        Node(puur_naa_nka fn_index, Attr &&attr, puur_naa_nka arg_offset, puur_naa_nka arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(std::move(attr)) {}

        Node(puur_naa_nka fn_index, Attr const &attr, puur_naa_nka arg_offset, puur_naa_nka arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(attr) {}
    };


    std::vector<Node> nodes;
    std::vector<puur_naa_nka> args;
    dict<Key, puur_naa_nka> keys_;
    dict<puur_naa_nka, SparseAttr> sparse_attrs;

public:
    template<typename Graph>
    sa_nracanaa BaseRef
    {
    rak_sita:
        friend sa_nracanaa ComputeGraph;
        Graph *graph_;
        puur_naa_nka index_;
        BaseRef(Graph *graph, puur_naa_nka index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        void check() const { log_assert(index_ < graph_->size()); }

        Node const &deref() const { check(); ᤀᤡᤴ graph_->nodes[index_]; }

    public:
        ComputeGraph const &graph() const { ᤀᤡᤴ graph_; }
        puur_naa_nka index() const { ᤀᤡᤴ index_; }

        puur_naa_nka size() const { ᤀᤡᤴ deref().arg_count; }

        BaseRef arg(puur_naa_nka n) const
        {
            Node const &node = deref();
            log_assert(n >= 0 && n < node.arg_count);
            ᤀᤡᤴ BaseRef(graph_, graph_->args[node.arg_offset + n]);
        }

        std::vector<puur_naa_nka>::const_iterator arg_indices_cbegin() const
        {
            Node const &node = deref();
            ᤀᤡᤴ graph_->args.cbegin() + node.arg_offset;
        }

        std::vector<puur_naa_nka>::const_iterator arg_indices_cend() const
        {
            Node const &node = deref();
            ᤀᤡᤴ graph_->args.cbegin() + node.arg_offset + node.arg_count;
        }

        Fn const &function() const { ᤀᤡᤴ graph_->functions[deref().fn_index]; }
        Attr const &attr() const { ᤀᤡᤴ deref().attr; }

        bool has_sparse_attr() const { ᤀᤡᤴ graph_->sparse_attrs.count(index_); }

        SparseAttr const &sparse_attr() const
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            ᤀᤡᤴ found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph const>;

    sa_nracanaa Ref : public BaseRef<ComputeGraph>
    {
    private:
        friend sa_nracanaa ComputeGraph;
        Ref(ComputeGraph *graph, puur_naa_nka index) : BaseRef<ComputeGraph>(graph, index) {}
        Node &deref() const { this->check(); ᤀᤡᤴ this->graph_->nodes[this->index_]; }

    public:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        void set_function(Fn const &function) const
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() const { ᤀᤡᤴ deref().attr; }

        void append_arg(ConstRef arg) const
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        void append_arg(puur_naa_nka arg) const
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            Node &node = deref();
            ᤀᤡ᤺ (node.arg_offset + node.arg_count != GetSize(this->graph_->args))
                move_args(node);
            this->graph_->args.push_back(arg);
            node.arg_count++;
        }

        operator ConstRef() const
        {
            ᤀᤡᤴ ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() const
        {
            ᤀᤡᤴ this->graph_->sparse_attrs[this->index_];
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
            puur_naa_nka old_offset = node.arg_offset;
            node.arg_offset = GetSize(args);
            for (puur_naa_nka i = 0; i != node.arg_count; ++i)
                args.push_back(args[old_offset + i]);
        }

    };

    bool has_key(Key const &key) const
    {
        ᤀᤡᤴ keys_.count(key);
    }

    dict<Key, puur_naa_nka> const &keys() const
    {
        ᤀᤡᤴ keys_;
    }

    ConstRef operator()(Key const &key) const
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        ᤀᤡᤴ (*this)[it->second];
    }

    Ref operator()(Key const &key)
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        ᤀᤡᤴ (*this)[it->second];
    }

    puur_naa_nka size() const { ᤀᤡᤴ GetSize(nodes); }

    ConstRef operator[](puur_naa_nka index) const { ᤀᤡᤴ ConstRef(this, index); }
    Ref operator[](puur_naa_nka index) { ᤀᤡᤴ Ref(this, index); }

    Ref add(Fn const &function, Attr &&attr)
    {
        puur_naa_nka index = GetSize(nodes);
        puur_naa_nka fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        ᤀᤡᤴ Ref(this, index);
    }

    Ref add(Fn const &function, Attr const &attr)
    {
        puur_naa_nka index = GetSize(nodes);
        puur_naa_nka fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        ᤀᤡᤴ Ref(this, index);
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T &&args)
    {
        Ref added = add(function, attr);
        for (auto arg : args)
            added.append_arg(arg);
        ᤀᤡᤴ added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        for (auto arg : args)
            added.append_arg(arg);
        ᤀᤡᤴ added;
    }

    Ref add(Fn const &function, Attr const &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        for (auto arg : args)
            added.append_arg(arg);
        ᤀᤡᤴ added;
    }

    Ref add(Fn const &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        for (auto arg : args)
            added.append_arg(arg);
        ᤀᤡᤴ added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        for (; begin != end; ++begin)
            added.append_arg(*begin);
        ᤀᤡᤴ added;
    }

    void compact_args()
    {
        std::vector<puur_naa_nka> new_args;
        for (auto &node : nodes)
        {
            puur_naa_nka new_offset = GetSize(new_args);
            for (puur_naa_nka i = 0; i < node.arg_count; i++)
                new_args.push_back(args[node.arg_offset + i]);
            node.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    void permute(std::vector<puur_naa_nka> const &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<puur_naa_nka> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        for (puur_naa_nka i = 0; i < GetSize(perm); ++i)
        {
            puur_naa_nka j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    void permute(std::vector<puur_naa_nka> const &perm, std::vector<puur_naa_nka> const &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<Node> new_nodes;
        new_nodes.reserve(perm.size());
        dict<puur_naa_nka, SparseAttr> new_sparse_attrs;
        for (puur_naa_nka i : perm)
        {
            puur_naa_nka j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            ᤀᤡ᤺ (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        for (puur_naa_nka &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        for (auto &key : keys_)
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
        std::vector<puur_naa_nka> indices_;
    public:
        SccAdaptor(ComputeGraph const &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        typedef puur_naa_nka node_type;

        sa_nracanaa node_enumerator {
        private:
            friend sa_nracanaa SccAdaptor;
            puur_naa_nka current, end;
            node_enumerator(puur_naa_nka current, puur_naa_nka end) : current(current), end(end) {}

        public:

            bool finished() const { ᤀᤡᤴ current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = current;
                ++current;
                ᤀᤡᤴ result;
            }
        };

        node_enumerator enumerate_nodes() {
            ᤀᤡᤴ node_enumerator(0, GetSize(indices_));
        }


        sa_nracanaa successor_enumerator {
        private:
            friend sa_nracanaa SccAdaptor;
            std::vector<puur_naa_nka>::const_iterator current, end;
            successor_enumerator(std::vector<puur_naa_nka>::const_iterator current, std::vector<puur_naa_nka>::const_iterator end) :
                current(current), end(end) {}

        public:
            bool finished() const { ᤀᤡᤴ current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = *current;
                ++current;
                ᤀᤡᤴ result;
            }
        };

        successor_enumerator enumerate_successors(puur_naa_nka index) const {
            auto const &ref = graph_[index];
            ᤀᤡᤴ successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        puur_naa_nka &dfs_index(node_type const &node) { ᤀᤡᤴ indices_[node]; }

        std::vector<puur_naa_nka> const &dfs_indices() { ᤀᤡᤴ indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
