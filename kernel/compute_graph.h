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

#অন্তর্ভাৱ <tuple>
#অন্তর্ভাৱ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

template<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename চাবি_2 = std::tuple<> // Stable keys to refer to nodes
>
সংরচনা ComputeGraph
{
    সংরচনা Ref;
গুপ্ত:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    সংরচনা গ্ৰন্থি_2 {
        পূর্ণাঙ্ক fn_index;
        পূর্ণাঙ্ক arg_offset;
        পূর্ণাঙ্ক arg_গুনতি;
        Attr attr;

        গ্ৰন্থি_2(পূর্ণাঙ্ক fn_index, Attr &&attr, পূর্ণাঙ্ক arg_offset, পূর্ণাঙ্ক arg_গুনতি = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_গুনতি(arg_গুনতি), attr(std::move(attr)) {}

        গ্ৰন্থি_2(পূর্ণাঙ্ক fn_index, Attr ধ্রুৱ &attr, পূর্ণাঙ্ক arg_offset, পূর্ণাঙ্ক arg_গুনতি = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_গুনতি(arg_গুনতি), attr(attr) {}
    };


    std::vector<গ্ৰন্থি_2> nodes;
    std::vector<পূর্ণাঙ্ক> args;
    dict<চাবি_2, পূর্ণাঙ্ক> চাবিগুলি;
    dict<পূর্ণাঙ্ক, SparseAttr> sparse_attrs;

সার্ৱজনিক:
    template<typename Graph>
    সংরচনা BaseRef
    {
    রক্ষিত:
        friend সংরচনা ComputeGraph;
        Graph *graph_;
        পূর্ণাঙ্ক index_;
        BaseRef(Graph *graph, পূর্ণাঙ্ক index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        শূন্য check() ধ্রুৱ { log_assert(index_ < graph_->size()); }

        গ্ৰন্থি_2 ধ্রুৱ &deref() ধ্রুৱ { check(); ঘূৰাই graph_->nodes[index_]; }

    সার্ৱজনিক:
        ComputeGraph ধ্রুৱ &graph() ধ্রুৱ { ঘূৰাই graph_; }
        পূর্ণাঙ্ক index() ধ্রুৱ { ঘূৰাই index_; }

        পূর্ণাঙ্ক size() ধ্রুৱ { ঘূৰাই deref().arg_গুনতি; }

        BaseRef arg(পূর্ণাঙ্ক n) ধ্রুৱ
        {
            গ্ৰন্থি_2 ধ্রুৱ &গ্ৰন্থি = deref();
            log_assert(n >= 0 && n < গ্ৰন্থি.arg_গুনতি);
            ঘূৰাই BaseRef(graph_, graph_->args[গ্ৰন্থি.arg_offset + n]);
        }

        std::vector<পূর্ণাঙ্ক>::const_iterator arg_indices_cbegin() ধ্রুৱ
        {
            গ্ৰন্থি_2 ধ্রুৱ &গ্ৰন্থি = deref();
            ঘূৰাই graph_->args.cbegin() + গ্ৰন্থি.arg_offset;
        }

        std::vector<পূর্ণাঙ্ক>::const_iterator arg_indices_cend() ধ্রুৱ
        {
            গ্ৰন্থি_2 ধ্রুৱ &গ্ৰন্থি = deref();
            ঘূৰাই graph_->args.cbegin() + গ্ৰন্থি.arg_offset + গ্ৰন্থি.arg_গুনতি;
        }

        Fn ধ্রুৱ &function() ধ্রুৱ { ঘূৰাই graph_->functions[deref().fn_index]; }
        Attr ধ্রুৱ &attr() ধ্রুৱ { ঘূৰাই deref().attr; }

        bool has_sparse_attr() ধ্রুৱ { ঘূৰাই graph_->sparse_attrs.গুনতি(index_); }

        SparseAttr ধ্রুৱ &sparse_attr() ধ্রুৱ
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            ঘূৰাই found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph ধ্রুৱ>;

    সংরচনা Ref : সার্ৱজনিক BaseRef<ComputeGraph>
    {
    গুপ্ত:
        friend সংরচনা ComputeGraph;
        Ref(ComputeGraph *graph, পূর্ণাঙ্ক index) : BaseRef<ComputeGraph>(graph, index) {}
        গ্ৰন্থি_2 &deref() ধ্রুৱ { this->check(); ঘূৰাই this->graph_->nodes[this->index_]; }

    সার্ৱজনিক:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        শূন্য set_function(Fn ধ্রুৱ &function) ধ্রুৱ
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() ধ্রুৱ { ঘূৰাই deref().attr; }

        শূন্য append_arg(ConstRef arg) ধ্রুৱ
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        শূন্য append_arg(পূর্ণাঙ্ক arg) ধ্রুৱ
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            গ্ৰন্থি_2 &গ্ৰন্থি = deref();
            যদি (গ্ৰন্থি.arg_offset + গ্ৰন্থি.arg_গুনতি != GetSize(this->graph_->args))
                move_args(গ্ৰন্থি);
            this->graph_->args.ঠেলো_back(arg);
            গ্ৰন্থি.arg_গুনতি++;
        }

        operator ConstRef() ধ্রুৱ
        {
            ঘূৰাই ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() ধ্রুৱ
        {
            ঘূৰাই this->graph_->sparse_attrs[this->index_];
        }

        শূন্য clear_sparse_attr() ধ্রুৱ
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        শূন্য assign_চাবি(চাবি_2 ধ্রুৱ &চাবি) ধ্রুৱ
        {
            this->graph_->চাবিগুলি.emplace(চাবি, this->index_);
        }

    গুপ্ত:
        শূন্য move_args(গ্ৰন্থি_2 &গ্ৰন্থি) ধ্রুৱ
        {
            auto &args = this->graph_->args;
            পূর্ণাঙ্ক old_offset = গ্ৰন্থি.arg_offset;
            গ্ৰন্থি.arg_offset = GetSize(args);
            বাবে (পূর্ণাঙ্ক i = 0; i != গ্ৰন্থি.arg_গুনতি; ++i)
                args.ঠেলো_back(args[old_offset + i]);
        }

    };

    bool has_চাবি(চাবি_2 ধ্রুৱ &চাবি) ধ্রুৱ
    {
        ঘূৰাই চাবিগুলি.গুনতি(চাবি);
    }

    dict<চাবি_2, পূর্ণাঙ্ক> ধ্রুৱ &চাবিগুলি_2() ধ্রুৱ
    {
        ঘূৰাই চাবিগুলি;
    }

    ConstRef operator()(চাবি_2 ধ্রুৱ &চাবি) ধ্রুৱ
    {
        auto it = চাবিগুলি.find(চাবি);
        log_assert(it != চাবিগুলি.end());
        ঘূৰাই (*this)[it->second];
    }

    Ref operator()(চাবি_2 ধ্রুৱ &চাবি)
    {
        auto it = চাবিগুলি.find(চাবি);
        log_assert(it != চাবিগুলি.end());
        ঘূৰাই (*this)[it->second];
    }

    পূর্ণাঙ্ক size() ধ্রুৱ { ঘূৰাই GetSize(nodes); }

    ConstRef operator[](পূর্ণাঙ্ক index) ধ্রুৱ { ঘূৰাই ConstRef(this, index); }
    Ref operator[](পূর্ণাঙ্ক index) { ঘূৰাই Ref(this, index); }

    Ref add(Fn ধ্রুৱ &function, Attr &&attr)
    {
        পূর্ণাঙ্ক index = GetSize(nodes);
        পূর্ণাঙ্ক fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        ঘূৰাই Ref(this, index);
    }

    Ref add(Fn ধ্রুৱ &function, Attr ধ্রুৱ &attr)
    {
        পূর্ণাঙ্ক index = GetSize(nodes);
        পূর্ণাঙ্ক fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        ঘূৰাই Ref(this, index);
    }

    template<typename T>
    Ref add(Fn ধ্রুৱ &function, Attr ধ্রুৱ &attr, T &&args)
    {
        Ref added = add(function, attr);
        বাবে (auto arg : args)
            added.append_arg(arg);
        ঘূৰাই added;
    }

    template<typename T>
    Ref add(Fn ধ্রুৱ &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        বাবে (auto arg : args)
            added.append_arg(arg);
        ঘূৰাই added;
    }

    Ref add(Fn ধ্রুৱ &function, Attr ধ্রুৱ &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        বাবে (auto arg : args)
            added.append_arg(arg);
        ঘূৰাই added;
    }

    Ref add(Fn ধ্রুৱ &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        বাবে (auto arg : args)
            added.append_arg(arg);
        ঘূৰাই added;
    }

    template<typename T>
    Ref add(Fn ধ্রুৱ &function, Attr ধ্রুৱ &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        বাবে (; begin != end; ++begin)
            added.append_arg(*begin);
        ঘূৰাই added;
    }

    শূন্য compact_args()
    {
        std::vector<পূর্ণাঙ্ক> new_args;
        বাবে (auto &গ্ৰন্থি : nodes)
        {
            পূর্ণাঙ্ক new_offset = GetSize(new_args);
            বাবে (পূর্ণাঙ্ক i = 0; i < গ্ৰন্থি.arg_গুনতি; i++)
                new_args.ঠেলো_back(args[গ্ৰন্থি.arg_offset + i]);
            গ্ৰন্থি.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    শূন্য permute(std::vector<পূর্ণাঙ্ক> ধ্রুৱ &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<পূর্ণাঙ্ক> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        বাবে (পূর্ণাঙ্ক i = 0; i < GetSize(perm); ++i)
        {
            পূর্ণাঙ্ক j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    শূন্য permute(std::vector<পূর্ণাঙ্ক> ধ্রুৱ &perm, std::vector<পূর্ণাঙ্ক> ধ্রুৱ &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<গ্ৰন্থি_2> new_nodes;
        new_nodes.reserve(perm.size());
        dict<পূর্ণাঙ্ক, SparseAttr> new_sparse_attrs;
        বাবে (পূর্ণাঙ্ক i : perm)
        {
            পূর্ণাঙ্ক j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            যদি (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        বাবে (পূর্ণাঙ্ক &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        বাবে (auto &চাবি : চাবিগুলি)
        {
            log_assert(চাবি.second < GetSize(inv_perm));
            log_assert(inv_perm[চাবি.second] >= 0);
            চাবি.second = inv_perm[চাবি.second];
        }
    }

    সংরচনা SccAdaptor
    {
    গুপ্ত:
        ComputeGraph ধ্রুৱ &graph_;
        std::vector<পূর্ণাঙ্ক> indices_;
    সার্ৱজনিক:
        SccAdaptor(ComputeGraph ধ্রুৱ &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        প্রকারনির্দেশ পূর্ণাঙ্ক গ্ৰন্থি_type;

        সংরচনা গ্ৰন্থি_enumerator {
        গুপ্ত:
            friend সংরচনা SccAdaptor;
            পূর্ণাঙ্ক current, end;
            গ্ৰন্থি_enumerator(পূর্ণাঙ্ক current, পূর্ণাঙ্ক end) : current(current), end(end) {}

        সার্ৱজনিক:

            bool finished() ধ্রুৱ { ঘূৰাই current == end; }
            গ্ৰন্থি_type next() {
                log_assert(!finished());
                গ্ৰন্থি_type result = current;
                ++current;
                ঘূৰাই result;
            }
        };

        গ্ৰন্থি_enumerator enumerate_nodes() {
            ঘূৰাই গ্ৰন্থি_enumerator(0, GetSize(indices_));
        }


        সংরচনা successor_enumerator {
        গুপ্ত:
            friend সংরচনা SccAdaptor;
            std::vector<পূর্ণাঙ্ক>::const_iterator current, end;
            successor_enumerator(std::vector<পূর্ণাঙ্ক>::const_iterator current, std::vector<পূর্ণাঙ্ক>::const_iterator end) :
                current(current), end(end) {}

        সার্ৱজনিক:
            bool finished() ধ্রুৱ { ঘূৰাই current == end; }
            গ্ৰন্থি_type next() {
                log_assert(!finished());
                গ্ৰন্থি_type result = *current;
                ++current;
                ঘূৰাই result;
            }
        };

        successor_enumerator enumerate_successors(পূর্ণাঙ্ক index) ধ্রুৱ {
            auto ধ্রুৱ &ref = graph_[index];
            ঘূৰাই successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        পূর্ণাঙ্ক &dfs_index(গ্ৰন্থি_type ধ্রুৱ &গ্ৰন্থি) { ঘূৰাই indices_[গ্ৰন্থি]; }

        std::vector<পূর্ণাঙ্ক> ধ্রুৱ &dfs_indices() { ঘূৰাই indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
