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
    typename ئاچقۇچ_2 = std::tuple<> // Stable keys to refer to nodes
>
بنية ComputeGraph
{
    بنية Ref;
خاص:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    بنية تۈگۈن_2 {
        صحيح fn_index;
        صحيح arg_offset;
        صحيح arg_سانى;
        Attr attr;

        تۈگۈن_2(صحيح fn_index, Attr &&attr, صحيح arg_offset, صحيح arg_سانى = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_سانى(arg_سانى), attr(std::move(attr)) {}

        تۈگۈن_2(صحيح fn_index, Attr ثابت &attr, صحيح arg_offset, صحيح arg_سانى = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_سانى(arg_سانى), attr(attr) {}
    };


    std::vector<تۈگۈن_2> nodes;
    std::vector<صحيح> args;
    dict<ئاچقۇچ_2, صحيح> ئاچقۇچلار;
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

        تۈگۈن_2 ثابت &deref() ثابت { check(); أرجع graph_->nodes[index_]; }

    عام:
        ComputeGraph ثابت &graph() ثابت { أرجع graph_; }
        صحيح index() ثابت { أرجع index_; }

        صحيح size() ثابت { أرجع deref().arg_سانى; }

        BaseRef arg(صحيح n) ثابت
        {
            تۈگۈن_2 ثابت &تۈگۈن = deref();
            log_assert(n >= 0 && n < تۈگۈن.arg_سانى);
            أرجع BaseRef(graph_, graph_->args[تۈگۈن.arg_offset + n]);
        }

        std::vector<صحيح>::const_iterator arg_indices_cbegin() ثابت
        {
            تۈگۈن_2 ثابت &تۈگۈن = deref();
            أرجع graph_->args.cbegin() + تۈگۈن.arg_offset;
        }

        std::vector<صحيح>::const_iterator arg_indices_cend() ثابت
        {
            تۈگۈن_2 ثابت &تۈگۈن = deref();
            أرجع graph_->args.cbegin() + تۈگۈن.arg_offset + تۈگۈن.arg_سانى;
        }

        Fn ثابت &function() ثابت { أرجع graph_->functions[deref().fn_index]; }
        Attr ثابت &attr() ثابت { أرجع deref().attr; }

        bool has_sparse_attr() ثابت { أرجع graph_->sparse_attrs.سانى(index_); }

        SparseAttr ثابت &sparse_attr() ثابت
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            أرجع found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph ثابت>;

    بنية Ref : عام BaseRef<ComputeGraph>
    {
    خاص:
        friend بنية ComputeGraph;
        Ref(ComputeGraph *graph, صحيح index) : BaseRef<ComputeGraph>(graph, index) {}
        تۈگۈن_2 &deref() ثابت { this->check(); أرجع this->graph_->nodes[this->index_]; }

    عام:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        فراغ set_function(Fn ثابت &function) ثابت
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() ثابت { أرجع deref().attr; }

        فراغ append_arg(ConstRef arg) ثابت
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        فراغ append_arg(صحيح arg) ثابت
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            تۈگۈن_2 &تۈگۈن = deref();
            إذا (تۈگۈن.arg_offset + تۈگۈن.arg_سانى != GetSize(this->graph_->args))
                move_args(تۈگۈن);
            this->graph_->args.ئىتتىرىش_back(arg);
            تۈگۈن.arg_سانى++;
        }

        operator ConstRef() ثابت
        {
            أرجع ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() ثابت
        {
            أرجع this->graph_->sparse_attrs[this->index_];
        }

        فراغ clear_sparse_attr() ثابت
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        فراغ assign_ئاچقۇچ(ئاچقۇچ_2 ثابت &ئاچقۇچ) ثابت
        {
            this->graph_->ئاچقۇچلار.emplace(ئاچقۇچ, this->index_);
        }

    خاص:
        فراغ move_args(تۈگۈن_2 &تۈگۈن) ثابت
        {
            auto &args = this->graph_->args;
            صحيح old_offset = تۈگۈن.arg_offset;
            تۈگۈن.arg_offset = GetSize(args);
            لكل (صحيح i = 0; i != تۈگۈن.arg_سانى; ++i)
                args.ئىتتىرىش_back(args[old_offset + i]);
        }

    };

    bool has_ئاچقۇچ(ئاچقۇچ_2 ثابت &ئاچقۇچ) ثابت
    {
        أرجع ئاچقۇچلار.سانى(ئاچقۇچ);
    }

    dict<ئاچقۇچ_2, صحيح> ثابت &ئاچقۇچلار_2() ثابت
    {
        أرجع ئاچقۇچلار;
    }

    ConstRef operator()(ئاچقۇچ_2 ثابت &ئاچقۇچ) ثابت
    {
        auto it = ئاچقۇچلار.find(ئاچقۇچ);
        log_assert(it != ئاچقۇچلار.end());
        أرجع (*this)[it->second];
    }

    Ref operator()(ئاچقۇچ_2 ثابت &ئاچقۇچ)
    {
        auto it = ئاچقۇچلار.find(ئاچقۇچ);
        log_assert(it != ئاچقۇچلار.end());
        أرجع (*this)[it->second];
    }

    صحيح size() ثابت { أرجع GetSize(nodes); }

    ConstRef operator[](صحيح index) ثابت { أرجع ConstRef(this, index); }
    Ref operator[](صحيح index) { أرجع Ref(this, index); }

    Ref add(Fn ثابت &function, Attr &&attr)
    {
        صحيح index = GetSize(nodes);
        صحيح fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        أرجع Ref(this, index);
    }

    Ref add(Fn ثابت &function, Attr ثابت &attr)
    {
        صحيح index = GetSize(nodes);
        صحيح fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        أرجع Ref(this, index);
    }

    template<typename T>
    Ref add(Fn ثابت &function, Attr ثابت &attr, T &&args)
    {
        Ref added = add(function, attr);
        لكل (auto arg : args)
            added.append_arg(arg);
        أرجع added;
    }

    template<typename T>
    Ref add(Fn ثابت &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        لكل (auto arg : args)
            added.append_arg(arg);
        أرجع added;
    }

    Ref add(Fn ثابت &function, Attr ثابت &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        لكل (auto arg : args)
            added.append_arg(arg);
        أرجع added;
    }

    Ref add(Fn ثابت &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        لكل (auto arg : args)
            added.append_arg(arg);
        أرجع added;
    }

    template<typename T>
    Ref add(Fn ثابت &function, Attr ثابت &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        لكل (; begin != end; ++begin)
            added.append_arg(*begin);
        أرجع added;
    }

    فراغ compact_args()
    {
        std::vector<صحيح> new_args;
        لكل (auto &تۈگۈن : nodes)
        {
            صحيح new_offset = GetSize(new_args);
            لكل (صحيح i = 0; i < تۈگۈن.arg_سانى; i++)
                new_args.ئىتتىرىش_back(args[تۈگۈن.arg_offset + i]);
            تۈگۈن.arg_offset = new_offset;
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
        std::vector<تۈگۈن_2> new_nodes;
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

        لكل (auto &ئاچقۇچ : ئاچقۇچلار)
        {
            log_assert(ئاچقۇچ.second < GetSize(inv_perm));
            log_assert(inv_perm[ئاچقۇچ.second] >= 0);
            ئاچقۇچ.second = inv_perm[ئاچقۇچ.second];
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


        تعريفنوع صحيح تۈگۈن_type;

        بنية تۈگۈن_enumerator {
        خاص:
            friend بنية SccAdaptor;
            صحيح current, end;
            تۈگۈن_enumerator(صحيح current, صحيح end) : current(current), end(end) {}

        عام:

            bool finished() ثابت { أرجع current == end; }
            تۈگۈن_type next() {
                log_assert(!finished());
                تۈگۈن_type result = current;
                ++current;
                أرجع result;
            }
        };

        تۈگۈن_enumerator enumerate_nodes() {
            أرجع تۈگۈن_enumerator(0, GetSize(indices_));
        }


        بنية successor_enumerator {
        خاص:
            friend بنية SccAdaptor;
            std::vector<صحيح>::const_iterator current, end;
            successor_enumerator(std::vector<صحيح>::const_iterator current, std::vector<صحيح>::const_iterator end) :
                current(current), end(end) {}

        عام:
            bool finished() ثابت { أرجع current == end; }
            تۈگۈن_type next() {
                log_assert(!finished());
                تۈگۈن_type result = *current;
                ++current;
                أرجع result;
            }
        };

        successor_enumerator enumerate_successors(صحيح index) ثابت {
            auto ثابت &ref = graph_[index];
            أرجع successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        صحيح &dfs_index(تۈگۈن_type ثابت &تۈگۈن) { أرجع indices_[تۈگۈن]; }

        std::vector<صحيح> ثابت &dfs_indices() { أرجع indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
