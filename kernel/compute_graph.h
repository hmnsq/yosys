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

#ضمن <tuple>
#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

template<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename مفتاح_2 = std::tuple<> // Stable keys to refer to nodes
>
بنية ComputeGraph
{
    بنية Ref;
خاص:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    بنية عقدة_2 {
        صحيح fn_index;
        صحيح arg_offset;
        صحيح arg_تعداد;
        Attr attr;

        عقدة_2(صحيح fn_index, Attr &&attr, صحيح arg_offset, صحيح arg_تعداد = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_تعداد(arg_تعداد), attr(std::move(attr)) {}

        عقدة_2(صحيح fn_index, Attr ثابت &attr, صحيح arg_offset, صحيح arg_تعداد = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_تعداد(arg_تعداد), attr(attr) {}
    };


    std::vector<عقدة_2> nodes;
    std::vector<صحيح> args;
    dict<مفتاح_2, صحيح> مفاتيح;
    dict<صحيح, SparseAttr> sparse_attrs;

عام:
    template<typename Graph>
    بنية BaseRef
    {
    محمي:
        friend بنية ComputeGraph;
        Graph *graph_;
        صحيح index_;
        BaseRef(Graph *graph, صحيح index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        فراغ check() ثابت { log_assert(index_ < graph_->size()); }

        عقدة_2 ثابت &deref() ثابت { check(); إرجاع graph_->nodes[index_]; }

    عام:
        ComputeGraph ثابت &graph() ثابت { إرجاع graph_; }
        صحيح index() ثابت { إرجاع index_; }

        صحيح size() ثابت { إرجاع deref().arg_تعداد; }

        BaseRef arg(صحيح n) ثابت
        {
            عقدة_2 ثابت &عقدة = deref();
            log_assert(n >= 0 && n < عقدة.arg_تعداد);
            إرجاع BaseRef(graph_, graph_->args[عقدة.arg_offset + n]);
        }

        std::vector<صحيح>::const_iterator arg_indices_cbegin() ثابت
        {
            عقدة_2 ثابت &عقدة = deref();
            إرجاع graph_->args.cbegin() + عقدة.arg_offset;
        }

        std::vector<صحيح>::const_iterator arg_indices_cend() ثابت
        {
            عقدة_2 ثابت &عقدة = deref();
            إرجاع graph_->args.cbegin() + عقدة.arg_offset + عقدة.arg_تعداد;
        }

        Fn ثابت &function() ثابت { إرجاع graph_->functions[deref().fn_index]; }
        Attr ثابت &attr() ثابت { إرجاع deref().attr; }

        bool has_sparse_attr() ثابت { إرجاع graph_->sparse_attrs.تعداد_2(index_); }

        SparseAttr ثابت &sparse_attr() ثابت
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            إرجاع found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph ثابت>;

    بنية Ref : عام BaseRef<ComputeGraph>
    {
    خاص:
        friend بنية ComputeGraph;
        Ref(ComputeGraph *graph, صحيح index) : BaseRef<ComputeGraph>(graph, index) {}
        عقدة_2 &deref() ثابت { this->check(); إرجاع this->graph_->nodes[this->index_]; }

    عام:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        فراغ set_function(Fn ثابت &function) ثابت
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() ثابت { إرجاع deref().attr; }

        فراغ append_arg(ConstRef arg) ثابت
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        فراغ append_arg(صحيح arg) ثابت
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            عقدة_2 &عقدة = deref();
            إذا (عقدة.arg_offset + عقدة.arg_تعداد != GetSize(this->graph_->args))
                move_args(عقدة);
            this->graph_->args.ادفع_back(arg);
            عقدة.arg_تعداد++;
        }

        operator ConstRef() ثابت
        {
            إرجاع ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() ثابت
        {
            إرجاع this->graph_->sparse_attrs[this->index_];
        }

        فراغ clear_sparse_attr() ثابت
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        فراغ assign_مفتاح(مفتاح_2 ثابت &مفتاح) ثابت
        {
            this->graph_->مفاتيح.emplace(مفتاح, this->index_);
        }

    خاص:
        فراغ move_args(عقدة_2 &عقدة) ثابت
        {
            auto &args = this->graph_->args;
            صحيح old_offset = عقدة.arg_offset;
            عقدة.arg_offset = GetSize(args);
            لكل (صحيح i = 0; i != عقدة.arg_تعداد; ++i)
                args.ادفع_back(args[old_offset + i]);
        }

    };

    bool has_مفتاح(مفتاح_2 ثابت &مفتاح) ثابت
    {
        إرجاع مفاتيح.تعداد_2(مفتاح);
    }

    dict<مفتاح_2, صحيح> ثابت &مفاتيح_2() ثابت
    {
        إرجاع مفاتيح;
    }

    ConstRef operator()(مفتاح_2 ثابت &مفتاح) ثابت
    {
        auto it = مفاتيح.find(مفتاح);
        log_assert(it != مفاتيح.end());
        إرجاع (*this)[it->second];
    }

    Ref operator()(مفتاح_2 ثابت &مفتاح)
    {
        auto it = مفاتيح.find(مفتاح);
        log_assert(it != مفاتيح.end());
        إرجاع (*this)[it->second];
    }

    صحيح size() ثابت { إرجاع GetSize(nodes); }

    ConstRef operator[](صحيح index) ثابت { إرجاع ConstRef(this, index); }
    Ref operator[](صحيح index) { إرجاع Ref(this, index); }

    Ref add(Fn ثابت &function, Attr &&attr)
    {
        صحيح index = GetSize(nodes);
        صحيح fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        إرجاع Ref(this, index);
    }

    Ref add(Fn ثابت &function, Attr ثابت &attr)
    {
        صحيح index = GetSize(nodes);
        صحيح fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        إرجاع Ref(this, index);
    }

    template<typename T>
    Ref add(Fn ثابت &function, Attr ثابت &attr, T &&args)
    {
        Ref added = add(function, attr);
        لكل (auto arg : args)
            added.append_arg(arg);
        إرجاع added;
    }

    template<typename T>
    Ref add(Fn ثابت &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        لكل (auto arg : args)
            added.append_arg(arg);
        إرجاع added;
    }

    Ref add(Fn ثابت &function, Attr ثابت &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        لكل (auto arg : args)
            added.append_arg(arg);
        إرجاع added;
    }

    Ref add(Fn ثابت &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        لكل (auto arg : args)
            added.append_arg(arg);
        إرجاع added;
    }

    template<typename T>
    Ref add(Fn ثابت &function, Attr ثابت &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        لكل (; begin != end; ++begin)
            added.append_arg(*begin);
        إرجاع added;
    }

    فراغ compact_args()
    {
        std::vector<صحيح> new_args;
        لكل (auto &عقدة : nodes)
        {
            صحيح new_offset = GetSize(new_args);
            لكل (صحيح i = 0; i < عقدة.arg_تعداد; i++)
                new_args.ادفع_back(args[عقدة.arg_offset + i]);
            عقدة.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    فراغ permute(std::vector<صحيح> ثابت &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<صحيح> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        لكل (صحيح i = 0; i < GetSize(perm); ++i)
        {
            صحيح j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    فراغ permute(std::vector<صحيح> ثابت &perm, std::vector<صحيح> ثابت &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<عقدة_2> new_nodes;
        new_nodes.reserve(perm.size());
        dict<صحيح, SparseAttr> new_sparse_attrs;
        لكل (صحيح i : perm)
        {
            صحيح j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            إذا (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        لكل (صحيح &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        لكل (auto &مفتاح : مفاتيح)
        {
            log_assert(مفتاح.second < GetSize(inv_perm));
            log_assert(inv_perm[مفتاح.second] >= 0);
            مفتاح.second = inv_perm[مفتاح.second];
        }
    }

    بنية SccAdaptor
    {
    خاص:
        ComputeGraph ثابت &graph_;
        std::vector<صحيح> indices_;
    عام:
        SccAdaptor(ComputeGraph ثابت &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        تعريفنوع صحيح عقدة_type;

        بنية عقدة_enumerator {
        خاص:
            friend بنية SccAdaptor;
            صحيح current, end;
            عقدة_enumerator(صحيح current, صحيح end) : current(current), end(end) {}

        عام:

            bool finished() ثابت { إرجاع current == end; }
            عقدة_type next() {
                log_assert(!finished());
                عقدة_type result = current;
                ++current;
                إرجاع result;
            }
        };

        عقدة_enumerator enumerate_nodes() {
            إرجاع عقدة_enumerator(0, GetSize(indices_));
        }


        بنية successor_enumerator {
        خاص:
            friend بنية SccAdaptor;
            std::vector<صحيح>::const_iterator current, end;
            successor_enumerator(std::vector<صحيح>::const_iterator current, std::vector<صحيح>::const_iterator end) :
                current(current), end(end) {}

        عام:
            bool finished() ثابت { إرجاع current == end; }
            عقدة_type next() {
                log_assert(!finished());
                عقدة_type result = *current;
                ++current;
                إرجاع result;
            }
        };

        successor_enumerator enumerate_successors(صحيح index) ثابت {
            auto ثابت &ref = graph_[index];
            إرجاع successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        صحيح &dfs_index(عقدة_type ثابت &عقدة) { إرجاع indices_[عقدة]; }

        std::vector<صحيح> ثابت &dfs_indices() { إرجاع indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
