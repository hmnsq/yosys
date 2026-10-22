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
        จำนวนเต็ม fn_index;
        จำนวนเต็ม arg_offset;
        จำนวนเต็ม arg_count;
        Attr attr;

        Node(จำนวนเต็ม fn_index, Attr &&attr, จำนวนเต็ม arg_offset, จำนวนเต็ม arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(std::move(attr)) {}

        Node(จำนวนเต็ม fn_index, Attr const &attr, จำนวนเต็ม arg_offset, จำนวนเต็ม arg_count = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_count(arg_count), attr(attr) {}
    };


    std::vector<Node> nodes;
    std::vector<จำนวนเต็ม> args;
    dict<Key, จำนวนเต็ม> keys_;
    dict<จำนวนเต็ม, SparseAttr> sparse_attrs;

public:
    template<typename Graph>
    sa_nracanaa BaseRef
    {
    rak_sita:
        friend sa_nracanaa ComputeGraph;
        Graph *graph_;
        จำนวนเต็ม index_;
        BaseRef(Graph *graph, จำนวนเต็ม index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        void check() const { log_assert(index_ < graph_->size()); }

        Node const &deref() const { check(); คืน graph_->nodes[index_]; }

    public:
        ComputeGraph const &graph() const { คืน graph_; }
        จำนวนเต็ม index() const { คืน index_; }

        จำนวนเต็ม size() const { คืน deref().arg_count; }

        BaseRef arg(จำนวนเต็ม n) const
        {
            Node const &node = deref();
            log_assert(n >= 0 && n < node.arg_count);
            คืน BaseRef(graph_, graph_->args[node.arg_offset + n]);
        }

        std::vector<จำนวนเต็ม>::const_iterator arg_indices_cbegin() const
        {
            Node const &node = deref();
            คืน graph_->args.cbegin() + node.arg_offset;
        }

        std::vector<จำนวนเต็ม>::const_iterator arg_indices_cend() const
        {
            Node const &node = deref();
            คืน graph_->args.cbegin() + node.arg_offset + node.arg_count;
        }

        Fn const &function() const { คืน graph_->functions[deref().fn_index]; }
        Attr const &attr() const { คืน deref().attr; }

        bool has_sparse_attr() const { คืน graph_->sparse_attrs.count(index_); }

        SparseAttr const &sparse_attr() const
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            คืน found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph const>;

    sa_nracanaa Ref : public BaseRef<ComputeGraph>
    {
    private:
        friend sa_nracanaa ComputeGraph;
        Ref(ComputeGraph *graph, จำนวนเต็ม index) : BaseRef<ComputeGraph>(graph, index) {}
        Node &deref() const { this->check(); คืน this->graph_->nodes[this->index_]; }

    public:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        void set_function(Fn const &function) const
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() const { คืน deref().attr; }

        void append_arg(ConstRef arg) const
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        void append_arg(จำนวนเต็ม arg) const
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            Node &node = deref();
            ถ้า (node.arg_offset + node.arg_count != GetSize(this->graph_->args))
                move_args(node);
            this->graph_->args.push_back(arg);
            node.arg_count++;
        }

        operator ConstRef() const
        {
            คืน ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() const
        {
            คืน this->graph_->sparse_attrs[this->index_];
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
            จำนวนเต็ม old_offset = node.arg_offset;
            node.arg_offset = GetSize(args);
            สำหรับ (จำนวนเต็ม i = 0; i != node.arg_count; ++i)
                args.push_back(args[old_offset + i]);
        }

    };

    bool has_key(Key const &key) const
    {
        คืน keys_.count(key);
    }

    dict<Key, จำนวนเต็ม> const &keys() const
    {
        คืน keys_;
    }

    ConstRef operator()(Key const &key) const
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        คืน (*this)[it->second];
    }

    Ref operator()(Key const &key)
    {
        auto it = keys_.find(key);
        log_assert(it != keys_.end());
        คืน (*this)[it->second];
    }

    จำนวนเต็ม size() const { คืน GetSize(nodes); }

    ConstRef operator[](จำนวนเต็ม index) const { คืน ConstRef(this, index); }
    Ref operator[](จำนวนเต็ม index) { คืน Ref(this, index); }

    Ref add(Fn const &function, Attr &&attr)
    {
        จำนวนเต็ม index = GetSize(nodes);
        จำนวนเต็ม fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        คืน Ref(this, index);
    }

    Ref add(Fn const &function, Attr const &attr)
    {
        จำนวนเต็ม index = GetSize(nodes);
        จำนวนเต็ม fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        คืน Ref(this, index);
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T &&args)
    {
        Ref added = add(function, attr);
        สำหรับ (auto arg : args)
            added.append_arg(arg);
        คืน added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        สำหรับ (auto arg : args)
            added.append_arg(arg);
        คืน added;
    }

    Ref add(Fn const &function, Attr const &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        สำหรับ (auto arg : args)
            added.append_arg(arg);
        คืน added;
    }

    Ref add(Fn const &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        สำหรับ (auto arg : args)
            added.append_arg(arg);
        คืน added;
    }

    template<typename T>
    Ref add(Fn const &function, Attr const &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        สำหรับ (; begin != end; ++begin)
            added.append_arg(*begin);
        คืน added;
    }

    void compact_args()
    {
        std::vector<จำนวนเต็ม> new_args;
        สำหรับ (auto &node : nodes)
        {
            จำนวนเต็ม new_offset = GetSize(new_args);
            สำหรับ (จำนวนเต็ม i = 0; i < node.arg_count; i++)
                new_args.push_back(args[node.arg_offset + i]);
            node.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    void permute(std::vector<จำนวนเต็ม> const &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<จำนวนเต็ม> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        สำหรับ (จำนวนเต็ม i = 0; i < GetSize(perm); ++i)
        {
            จำนวนเต็ม j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    void permute(std::vector<จำนวนเต็ม> const &perm, std::vector<จำนวนเต็ม> const &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<Node> new_nodes;
        new_nodes.reserve(perm.size());
        dict<จำนวนเต็ม, SparseAttr> new_sparse_attrs;
        สำหรับ (จำนวนเต็ม i : perm)
        {
            จำนวนเต็ม j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            ถ้า (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        สำหรับ (จำนวนเต็ม &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        สำหรับ (auto &key : keys_)
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
        std::vector<จำนวนเต็ม> indices_;
    public:
        SccAdaptor(ComputeGraph const &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        typedef จำนวนเต็ม node_type;

        sa_nracanaa node_enumerator {
        private:
            friend sa_nracanaa SccAdaptor;
            จำนวนเต็ม current, end;
            node_enumerator(จำนวนเต็ม current, จำนวนเต็ม end) : current(current), end(end) {}

        public:

            bool finished() const { คืน current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = current;
                ++current;
                คืน result;
            }
        };

        node_enumerator enumerate_nodes() {
            คืน node_enumerator(0, GetSize(indices_));
        }


        sa_nracanaa successor_enumerator {
        private:
            friend sa_nracanaa SccAdaptor;
            std::vector<จำนวนเต็ม>::const_iterator current, end;
            successor_enumerator(std::vector<จำนวนเต็ม>::const_iterator current, std::vector<จำนวนเต็ม>::const_iterator end) :
                current(current), end(end) {}

        public:
            bool finished() const { คืน current == end; }
            node_type next() {
                log_assert(!finished());
                node_type result = *current;
                ++current;
                คืน result;
            }
        };

        successor_enumerator enumerate_successors(จำนวนเต็ม index) const {
            auto const &ref = graph_[index];
            คืน successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        จำนวนเต็ม &dfs_index(node_type const &node) { คืน indices_[node]; }

        std::vector<จำนวนเต็ม> const &dfs_indices() { คืน indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
