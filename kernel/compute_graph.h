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

#অন্তর্ভাব <tuple>
#অন্তর্ভাব "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

template<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename চাবি_2 = std::tuple<> // Stable keys to refer to nodes
>
কাঠামো ComputeGraph
{
    কাঠামো Ref;
ব্যক্তিগত:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    কাঠামো গ্রন্থি_2 {
        পূর্ণসংখ্যা fn_সূচক;
        পূর্ণসংখ্যা arg_offset;
        পূর্ণসংখ্যা arg_গুনতি;
        Attr attr;

        গ্রন্থি_2(পূর্ণসংখ্যা fn_সূচক, Attr &&attr, পূর্ণসংখ্যা arg_offset, পূর্ণসংখ্যা arg_গুনতি = 0)
            : fn_সূচক(fn_সূচক), arg_offset(arg_offset), arg_গুনতি(arg_গুনতি), attr(std::move(attr)) {}

        গ্রন্থি_2(পূর্ণসংখ্যা fn_সূচক, Attr ধ্রুবক &attr, পূর্ণসংখ্যা arg_offset, পূর্ণসংখ্যা arg_গুনতি = 0)
            : fn_সূচক(fn_সূচক), arg_offset(arg_offset), arg_গুনতি(arg_গুনতি), attr(attr) {}
    };


    std::vector<গ্রন্থি_2> nodes;
    std::vector<পূর্ণসংখ্যা> args;
    dict<চাবি_2, পূর্ণসংখ্যা> চাবিগুলি;
    dict<পূর্ণসংখ্যা, SparseAttr> sparse_attrs;

সর্বজনীন:
    template<typename Graph>
    কাঠামো BaseRef
    {
    সুরক্ষিত:
        friend কাঠামো ComputeGraph;
        Graph *graph_;
        পূর্ণসংখ্যা সূচক_2;
        BaseRef(Graph *graph, পূর্ণসংখ্যা সূচক) : graph_(graph), সূচক_2(সূচক) {
            log_assert(সূচক_2 >= 0);
            check();
        }

        শূন্য check() ধ্রুবক { log_assert(সূচক_2 < graph_->আকার_2()); }

        গ্রন্থি_2 ধ্রুবক &deref() ধ্রুবক { check(); ফেরত graph_->nodes[সূচক_2]; }

    সর্বজনীন:
        ComputeGraph ধ্রুবক &graph() ধ্রুবক { ফেরত graph_; }
        পূর্ণসংখ্যা সূচক() ধ্রুবক { ফেরত সূচক_2; }

        পূর্ণসংখ্যা আকার_2() ধ্রুবক { ফেরত deref().arg_গুনতি; }

        BaseRef arg(পূর্ণসংখ্যা n) ধ্রুবক
        {
            গ্রন্থি_2 ধ্রুবক &গ্রন্থি = deref();
            log_assert(n >= 0 && n < গ্রন্থি.arg_গুনতি);
            ফেরত BaseRef(graph_, graph_->args[গ্রন্থি.arg_offset + n]);
        }

        std::vector<পূর্ণসংখ্যা>::const_iterator arg_indices_cbegin() ধ্রুবক
        {
            গ্রন্থি_2 ধ্রুবক &গ্রন্থি = deref();
            ফেরত graph_->args.cbegin() + গ্রন্থি.arg_offset;
        }

        std::vector<পূর্ণসংখ্যা>::const_iterator arg_indices_cend() ধ্রুবক
        {
            গ্রন্থি_2 ধ্রুবক &গ্রন্থি = deref();
            ফেরত graph_->args.cbegin() + গ্রন্থি.arg_offset + গ্রন্থি.arg_গুনতি;
        }

        Fn ধ্রুবক &function() ধ্রুবক { ফেরত graph_->functions[deref().fn_সূচক]; }
        Attr ধ্রুবক &attr() ধ্রুবক { ফেরত deref().attr; }

        bool has_sparse_attr() ধ্রুবক { ফেরত graph_->sparse_attrs.গুনতি(সূচক_2); }

        SparseAttr ধ্রুবক &sparse_attr() ধ্রুবক
        {
            auto found = graph_->sparse_attrs.find(সূচক_2);
            log_assert(found != graph_->sparse_attrs.end());
            ফেরত found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph ধ্রুবক>;

    কাঠামো Ref : সর্বজনীন BaseRef<ComputeGraph>
    {
    ব্যক্তিগত:
        friend কাঠামো ComputeGraph;
        Ref(ComputeGraph *graph, পূর্ণসংখ্যা সূচক) : BaseRef<ComputeGraph>(graph, সূচক) {}
        গ্রন্থি_2 &deref() ধ্রুবক { this->check(); ফেরত this->graph_->nodes[this->সূচক_2]; }

    সর্বজনীন:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.সূচক_2) {}

        শূন্য set_function(Fn ধ্রুবক &function) ধ্রুবক
        {
            deref().fn_সূচক = this->graph_->functions(function);
        }

        Attr &attr() ধ্রুবক { ফেরত deref().attr; }

        শূন্য append_arg(ConstRef arg) ধ্রুবক
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.সূচক());
        }

        শূন্য append_arg(পূর্ণসংখ্যা arg) ধ্রুবক
        {
            log_assert(arg >= 0 && arg < this->graph_->আকার_2());
            গ্রন্থি_2 &গ্রন্থি = deref();
            যদি (গ্রন্থি.arg_offset + গ্রন্থি.arg_গুনতি != Get_আকার(this->graph_->args))
                move_args(গ্রন্থি);
            this->graph_->args.ঠেলো_back(arg);
            গ্রন্থি.arg_গুনতি++;
        }

        operator ConstRef() ধ্রুবক
        {
            ফেরত ConstRef(this->graph_, this->সূচক_2);
        }

        SparseAttr &sparse_attr() ধ্রুবক
        {
            ফেরত this->graph_->sparse_attrs[this->সূচক_2];
        }

        শূন্য clear_sparse_attr() ধ্রুবক
        {
            this->graph_->sparse_attrs.erase(this->সূচক_2);
        }

        শূন্য assign_চাবি(চাবি_2 ধ্রুবক &চাবি) ধ্রুবক
        {
            this->graph_->চাবিগুলি.emplace(চাবি, this->সূচক_2);
        }

    ব্যক্তিগত:
        শূন্য move_args(গ্রন্থি_2 &গ্রন্থি) ধ্রুবক
        {
            auto &args = this->graph_->args;
            পূর্ণসংখ্যা old_offset = গ্রন্থি.arg_offset;
            গ্রন্থি.arg_offset = Get_আকার(args);
            জন্য (পূর্ণসংখ্যা i = 0; i != গ্রন্থি.arg_গুনতি; ++i)
                args.ঠেলো_back(args[old_offset + i]);
        }

    };

    bool has_চাবি(চাবি_2 ধ্রুবক &চাবি) ধ্রুবক
    {
        ফেরত চাবিগুলি.গুনতি(চাবি);
    }

    dict<চাবি_2, পূর্ণসংখ্যা> ধ্রুবক &চাবিগুলি_2() ধ্রুবক
    {
        ফেরত চাবিগুলি;
    }

    ConstRef operator()(চাবি_2 ধ্রুবক &চাবি) ধ্রুবক
    {
        auto it = চাবিগুলি.find(চাবি);
        log_assert(it != চাবিগুলি.end());
        ফেরত (*this)[it->second];
    }

    Ref operator()(চাবি_2 ধ্রুবক &চাবি)
    {
        auto it = চাবিগুলি.find(চাবি);
        log_assert(it != চাবিগুলি.end());
        ফেরত (*this)[it->second];
    }

    পূর্ণসংখ্যা আকার_2() ধ্রুবক { ফেরত Get_আকার(nodes); }

    ConstRef operator[](পূর্ণসংখ্যা সূচক) ধ্রুবক { ফেরত ConstRef(this, সূচক); }
    Ref operator[](পূর্ণসংখ্যা সূচক) { ফেরত Ref(this, সূচক); }

    Ref add(Fn ধ্রুবক &function, Attr &&attr)
    {
        পূর্ণসংখ্যা সূচক = Get_আকার(nodes);
        পূর্ণসংখ্যা fn_সূচক = functions(function);
        nodes.emplace_back(fn_সূচক, std::move(attr), Get_আকার(args));
        ফেরত Ref(this, সূচক);
    }

    Ref add(Fn ধ্রুবক &function, Attr ধ্রুবক &attr)
    {
        পূর্ণসংখ্যা সূচক = Get_আকার(nodes);
        পূর্ণসংখ্যা fn_সূচক = functions(function);
        nodes.emplace_back(fn_সূচক, attr,  Get_আকার(args));
        ফেরত Ref(this, সূচক);
    }

    template<typename T>
    Ref add(Fn ধ্রুবক &function, Attr ধ্রুবক &attr, T &&args)
    {
        Ref added = add(function, attr);
        জন্য (auto arg : args)
            added.append_arg(arg);
        ফেরত added;
    }

    template<typename T>
    Ref add(Fn ধ্রুবক &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        জন্য (auto arg : args)
            added.append_arg(arg);
        ফেরত added;
    }

    Ref add(Fn ধ্রুবক &function, Attr ধ্রুবক &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        জন্য (auto arg : args)
            added.append_arg(arg);
        ফেরত added;
    }

    Ref add(Fn ধ্রুবক &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        জন্য (auto arg : args)
            added.append_arg(arg);
        ফেরত added;
    }

    template<typename T>
    Ref add(Fn ধ্রুবক &function, Attr ধ্রুবক &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        জন্য (; begin != end; ++begin)
            added.append_arg(*begin);
        ফেরত added;
    }

    শূন্য compact_args()
    {
        std::vector<পূর্ণসংখ্যা> new_args;
        জন্য (auto &গ্রন্থি : nodes)
        {
            পূর্ণসংখ্যা new_offset = Get_আকার(new_args);
            জন্য (পূর্ণসংখ্যা i = 0; i < গ্রন্থি.arg_গুনতি; i++)
                new_args.ঠেলো_back(args[গ্রন্থি.arg_offset + i]);
            গ্রন্থি.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    শূন্য permute(std::vector<পূর্ণসংখ্যা> ধ্রুবক &perm)
    {
        log_assert(perm.আকার_2() <= nodes.আকার_2());
        std::vector<পূর্ণসংখ্যা> inv_perm;
        inv_perm.resize(nodes.আকার_2(), -1);
        জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(perm); ++i)
        {
            পূর্ণসংখ্যা j = perm[i];
            log_assert(j >= 0 && j < Get_আকার(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    শূন্য permute(std::vector<পূর্ণসংখ্যা> ধ্রুবক &perm, std::vector<পূর্ণসংখ্যা> ধ্রুবক &inv_perm)
    {
        log_assert(inv_perm.আকার_2() == nodes.আকার_2());
        std::vector<গ্রন্থি_2> new_nodes;
        new_nodes.reserve(perm.আকার_2());
        dict<পূর্ণসংখ্যা, SparseAttr> new_sparse_attrs;
        জন্য (পূর্ণসংখ্যা i : perm)
        {
            পূর্ণসংখ্যা j = Get_আকার(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            যদি (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        জন্য (পূর্ণসংখ্যা &arg : args)
        {
            log_assert(arg < Get_আকার(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        জন্য (auto &চাবি : চাবিগুলি)
        {
            log_assert(চাবি.second < Get_আকার(inv_perm));
            log_assert(inv_perm[চাবি.second] >= 0);
            চাবি.second = inv_perm[চাবি.second];
        }
    }

    কাঠামো SccAdaptor
    {
    ব্যক্তিগত:
        ComputeGraph ধ্রুবক &graph_;
        std::vector<পূর্ণসংখ্যা> indices_;
    সর্বজনীন:
        SccAdaptor(ComputeGraph ধ্রুবক &graph) : graph_(graph)
        {
            indices_.resize(graph.আকার_2(), -1);
        }


        প্রকারলেখ পূর্ণসংখ্যা গ্রন্থি_type;

        কাঠামো গ্রন্থি_enumerator {
        ব্যক্তিগত:
            friend কাঠামো SccAdaptor;
            পূর্ণসংখ্যা current, end;
            গ্রন্থি_enumerator(পূর্ণসংখ্যা current, পূর্ণসংখ্যা end) : current(current), end(end) {}

        সর্বজনীন:

            bool finished() ধ্রুবক { ফেরত current == end; }
            গ্রন্থি_type পরবর্তী() {
                log_assert(!finished());
                গ্রন্থি_type ফলাফল = current;
                ++current;
                ফেরত ফলাফল;
            }
        };

        গ্রন্থি_enumerator enumerate_nodes() {
            ফেরত গ্রন্থি_enumerator(0, Get_আকার(indices_));
        }


        কাঠামো successor_enumerator {
        ব্যক্তিগত:
            friend কাঠামো SccAdaptor;
            std::vector<পূর্ণসংখ্যা>::const_iterator current, end;
            successor_enumerator(std::vector<পূর্ণসংখ্যা>::const_iterator current, std::vector<পূর্ণসংখ্যা>::const_iterator end) :
                current(current), end(end) {}

        সর্বজনীন:
            bool finished() ধ্রুবক { ফেরত current == end; }
            গ্রন্থি_type পরবর্তী() {
                log_assert(!finished());
                গ্রন্থি_type ফলাফল = *current;
                ++current;
                ফেরত ফলাফল;
            }
        };

        successor_enumerator enumerate_successors(পূর্ণসংখ্যা সূচক) ধ্রুবক {
            auto ধ্রুবক &ref = graph_[সূচক];
            ফেরত successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        পূর্ণসংখ্যা &dfs_সূচক(গ্রন্থি_type ধ্রুবক &গ্রন্থি) { ফেরত indices_[গ্রন্থি]; }

        std::vector<পূর্ণসংখ্যা> ধ্রুবক &dfs_indices() { ফেরত indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
