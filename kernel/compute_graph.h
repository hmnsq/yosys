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
        ຈຳນວນເຕັມ fn_index;
        ຈຳນວນເຕັມ arg_offset;
        ຈຳນວນເຕັມ arg_count;
        Attr attr;

        Node(ຈຳນວນເຕັມ fn_index, Attr &&attr, ຈຳນວນເຕັມ arg_offset, ຈຳນວນເຕັມ arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(std::move(attr)) {}

        Node(ຈຳນວນເຕັມ fn_index, Attr const &attr, ຈຳນວນເຕັມ arg_offset, ຈຳນວນເຕັມ arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(attr) {}
    };


    std::vector<Node> nodes;
    std::vector<ຈຳນວນເຕັມ> args;
    dict<Key, ຈຳນວນເຕັມ> keys_;
    dict<ຈຳນວນເຕັມ, SparseAttr> sparse_attrs;

public:
    template<typename Graph>
    sa_nracanaa BaseRef
    {
    rak_sita:
        friend sa_nracanaa ComputeGraph;
        Graph *graph_;
        ຈຳນວນເຕັມ index_;
        BaseRef(Graph *graph, ຈຳນວນເຕັມ index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        void check() const { log_assert(index_ < graph_->size()); }

        Node const &deref() const { check(); ສົ່ງຄືນ graph_->nodes[index_]; }

    public:
        ComputeGraph const &graph() const { ສົ່ງຄືນ graph_; }
        ຈຳນວນເຕັມ index() const { ສົ່ງຄືນ index_; }

        ຈຳນວນເຕັມ size() const { ສົ່ງຄືນ deref().arg_count; }

        BaseRef arg(ຈຳນວນເຕັມ n) const
        {
            Node const &node = deref();
            log_assert(n >= 0 && n < node.arg_count);
            ສົ່ງຄືນ BaseRef(graph_, graph_->args[node.arg_offset + n]);
        }

        std::vector<ຈຳນວນເຕັມ>::const_iterator arg_indices_cbegin() const
        {
            Node const &node = deref();
            ສົ່ງຄືນ graph_->args.cbegin() + node.arg_offset;
        }

        std::vector<ຈຳນວນເຕັມ>::const_iterator arg_indices_cend() const
        {
            Node const &node = deref();
            ສົ່ງຄືນ graph_->args.cbegin() + node.arg_offset + node.arg_count;
        }

        Fn const &function() const { ສົ່ງຄືນ graph_->functions[deref().fn_index]; }
        Attr const &attr() const { ສົ່ງຄືນ deref().attr; }

        bool has_sparse_attr() const { ສົ່ງຄືນ graph_->sparse_attrs.count(index_); }

        SparseAttr const &sparse_attr() const
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            ສົ່ງຄືນ found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph const>;

    sa_nracanaa Ref : public BaseRef<ComputeGraph>
    {
    private:
        friend sa_nracanaa ComputeGraph;
        Ref(ComputeGraph *graph, ຈຳນວນເຕັມ index) : BaseRef<ComputeGraph>(graph, index) {}
        Node &deref() const { this->check(); ສົ່ງຄືນ this->graph_->nodes[this->index_]; }

    public:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        void set_function(Fn const &function) const
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() const { ສົ່ງຄືນ deref().attr; }

        void append_arg(ConstRef arg) const
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        void append_arg(ຈຳນວນເຕັມ arg) const
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            Node &node = deref();
            ຖ້າ (node.arg_offset + node.arg_count != GetSize(this->graph_->args))
                move_args(node);
            this->graph_->args.push_back(arg);
            node.arg_count++;
        }

        operator ConstRef() const
        {
            ສົ່ງຄືນ ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() const
        {
            ສົ່ງຄືນ this->graph_->sparse_attrs[this->index_];
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
            ຈຳນວນເຕັມ old_offset = node.arg_offset;
            node.arg_offset = GetSize(args);
            ສຳລັບ (ຈຳນວນເຕັມ i = 0; i != node.arg_count; ++i)
                args.push_back(args[old_offset + i]);
        }

    };

    bool has_key(Key const &key) const
    {
        ສົ່ງຄືນ keys_.count(key);
    }

    dict<Key, ຈຳນວນເຕັມ> const &keys() const
    {
        ສົ່ງຄືນ keys_;
    }

    ConstRef operator()(Key const &key) const
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        ສົ່ງຄືນ (*this)[it->second];
    }

    Ref operator()(Key const &key)
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        ສົ່ງຄືນ (*this)[it->second];
    }

    ຈຳນວນເຕັມ size() const { ສົ່ງຄືນ GetSize(nodes); }

    ConstRef operator[](ຈຳນວນເຕັມ index) const { ສົ່ງຄືນ ConstRef(this, index); }
    Ref operator[](ຈຳນວນເຕັມ index) { ສົ່ງຄືນ Ref(this, index); }

    Ref add(Fn const &function, Attr &&attr)
    {
        ຈຳນວນເຕັມ index = GetSize(nodes);
        ຈຳນວນເຕັມ fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        ສົ່ງຄືນ Ref(this, index);
    }

    Ref add(Fn const &function, Attr const &attr)
    {
        ຈຳນວນເຕັມ index = GetSize(nodes);
        ຈຳນວນເຕັມ fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        ສົ່ງຄືນ Ref(this, index);
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T &&args)
    {
        Ref added = add(function, attr);
        ສຳລັບ (auto arg : args)
            added.append_arg(arg);
        ສົ່ງຄືນ added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        ສຳລັບ (auto arg : args)
            added.append_arg(arg);
        ສົ່ງຄືນ added;
    }

    Ref add(Fn const &function, Attr const &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        ສຳລັບ (auto arg : args)
            added.append_arg(arg);
        ສົ່ງຄືນ added;
    }

    Ref add(Fn const &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        ສຳລັບ (auto arg : args)
            added.append_arg(arg);
        ສົ່ງຄືນ added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        ສຳລັບ (; begin != end; ++begin)
            added.append_arg(*begin);
        ສົ່ງຄືນ added;
    }

    void compact_args()
    {
        std::vector<ຈຳນວນເຕັມ> new_args;
        ສຳລັບ (auto &node : nodes)
        {
            ຈຳນວນເຕັມ new_offset = GetSize(new_args);
            ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < node.arg_count; i++)
                new_args.push_back(args[node.arg_offset + i]);
            node.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    void permute(std::vector<ຈຳນວນເຕັມ> const &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<ຈຳນວນເຕັມ> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < GetSize(perm); ++i)
        {
            ຈຳນວນເຕັມ j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    void permute(std::vector<ຈຳນວນເຕັມ> const &perm, std::vector<ຈຳນວນເຕັມ> const &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<Node> new_nodes;
        new_nodes.reserve(perm.size());
        dict<ຈຳນວນເຕັມ, SparseAttr> new_sparse_attrs;
        ສຳລັບ (ຈຳນວນເຕັມ i : perm)
        {
            ຈຳນວນເຕັມ j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            ຖ້າ (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        ສຳລັບ (ຈຳນວນເຕັມ &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        ສຳລັບ (auto &key : keys_)
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
        std::vector<ຈຳນວນເຕັມ> indices_;
    public:
        SccAdaptor(ComputeGraph const &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        typedef ຈຳນວນເຕັມ node_type;

        sa_nracanaa node_enumerator {
        private:
            friend sa_nracanaa SccAdaptor;
            ຈຳນວນເຕັມ current, end;
            node_enumerator(ຈຳນວນເຕັມ current, ຈຳນວນເຕັມ end) : current(current), end(end) {}

        public:

            bool finished() const { ສົ່ງຄືນ current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = current;
                ++current;
                ສົ່ງຄືນ result;
            }
        };

        node_enumerator enumerate_nodes() {
            ສົ່ງຄືນ node_enumerator(0, GetSize(indices_));
        }


        sa_nracanaa successor_enumerator {
        private:
            friend sa_nracanaa SccAdaptor;
            std::vector<ຈຳນວນເຕັມ>::const_iterator current, end;
            successor_enumerator(std::vector<ຈຳນວນເຕັມ>::const_iterator current, std::vector<ຈຳນວນເຕັມ>::const_iterator end) :
                current(current), end(end) {}

        public:
            bool finished() const { ສົ່ງຄືນ current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = *current;
                ++current;
                ສົ່ງຄືນ result;
            }
        };

        successor_enumerator enumerate_successors(ຈຳນວນເຕັມ index) const {
            auto const &ref = graph_[index];
            ສົ່ງຄືນ successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        ຈຳນວນເຕັມ &dfs_index(node_type const &node) { ສົ່ງຄືນ indices_[node]; }

        std::vector<ຈຳນວນເຕັມ> const &dfs_indices() { ສົ່ງຄືນ indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
