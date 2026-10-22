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
    typename مبودي_2 = std::tuple<> // Stable keys to refer to nodes
>
بنية ComputeGraph
{
    بنية Ref;
خاص:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    بنية كلي_2 {
        صحيح fn_index;
        صحيح arg_offset;
        صحيح arg_كدي;
        Attr attr;

        كلي_2(صحيح fn_index, Attr &&attr, صحيح arg_offset, صحيح arg_كدي = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_كدي(arg_كدي), attr(std::move(attr)) {}

        كلي_2(صحيح fn_index, Attr ثابت &attr, صحيح arg_offset, صحيح arg_كدي = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_كدي(arg_كدي), attr(attr) {}
    };


    std::vector<كلي_2> nodes;
    std::vector<صحيح> حجوجي_2;
    dict<مبودي_2, صحيح> مبوداي;
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

        كلي_2 ثابت &deref() ثابت { check(); أرجع graph_->nodes[index_]; }

    عام:
        ComputeGraph ثابت &graph() ثابت { أرجع graph_; }
        صحيح index() ثابت { أرجع index_; }

        صحيح size() ثابت { أرجع deref().arg_كدي; }

        BaseRef arg(صحيح n) ثابت
        {
            كلي_2 ثابت &كلي = deref();
            log_assert(n >= 0 && n < كلي.arg_كدي);
            أرجع BaseRef(graph_, graph_->حجوجي_2[كلي.arg_offset + n]);
        }

        std::vector<صحيح>::const_iterator arg_indices_cbegin() ثابت
        {
            كلي_2 ثابت &كلي = deref();
            أرجع graph_->حجوجي_2.cbegin() + كلي.arg_offset;
        }

        std::vector<صحيح>::const_iterator arg_indices_cend() ثابت
        {
            كلي_2 ثابت &كلي = deref();
            أرجع graph_->حجوجي_2.cbegin() + كلي.arg_offset + كلي.arg_كدي;
        }

        Fn ثابت &function() ثابت { أرجع graph_->functions[deref().fn_index]; }
        Attr ثابت &attr() ثابت { أرجع deref().attr; }

        bool has_sparse_attr() ثابت { أرجع graph_->sparse_attrs.كدي(index_); }

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
        كلي_2 &deref() ثابت { this->check(); أرجع this->graph_->nodes[this->index_]; }

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
            كلي_2 &كلي = deref();
            إذا (كلي.arg_offset + كلي.arg_كدي != GetSize(this->graph_->حجوجي_2))
                move_حجوجي(كلي);
            this->graph_->حجوجي_2.تور_back(arg);
            كلي.arg_كدي++;
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

        فراغ assign_مبودي(مبودي_2 ثابت &مبودي) ثابت
        {
            this->graph_->مبوداي.emplace(مبودي, this->index_);
        }

    خاص:
        فراغ move_حجوجي(كلي_2 &كلي) ثابت
        {
            auto &حجوجي_2 = this->graph_->حجوجي_2;
            صحيح old_offset = كلي.arg_offset;
            كلي.arg_offset = GetSize(حجوجي_2);
            لكل (صحيح i = 0; i != كلي.arg_كدي; ++i)
                حجوجي_2.تور_back(حجوجي_2[old_offset + i]);
        }

    };

    bool has_مبودي(مبودي_2 ثابت &مبودي) ثابت
    {
        أرجع مبوداي.كدي(مبودي);
    }

    dict<مبودي_2, صحيح> ثابت &مبوداي_2() ثابت
    {
        أرجع مبوداي;
    }

    ConstRef operator()(مبودي_2 ثابت &مبودي) ثابت
    {
        auto it = مبوداي.find(مبودي);
        log_assert(it != مبوداي.end());
        أرجع (*this)[it->second];
    }

    Ref operator()(مبودي_2 ثابت &مبودي)
    {
        auto it = مبوداي.find(مبودي);
        log_assert(it != مبوداي.end());
        أرجع (*this)[it->second];
    }

    صحيح size() ثابت { أرجع GetSize(nodes); }

    ConstRef operator[](صحيح index) ثابت { أرجع ConstRef(this, index); }
    Ref operator[](صحيح index) { أرجع Ref(this, index); }

    Ref add(Fn ثابت &function, Attr &&attr)
    {
        صحيح index = GetSize(nodes);
        صحيح fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(حجوجي_2));
        أرجع Ref(this, index);
    }

    Ref add(Fn ثابت &function, Attr ثابت &attr)
    {
        صحيح index = GetSize(nodes);
        صحيح fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(حجوجي_2));
        أرجع Ref(this, index);
    }

    template<typename T>
    Ref add(Fn ثابت &function, Attr ثابت &attr, T &&حجوجي_2)
    {
        Ref added = add(function, attr);
        لكل (auto arg : حجوجي_2)
            added.append_arg(arg);
        أرجع added;
    }

    template<typename T>
    Ref add(Fn ثابت &function, Attr &&attr, T &&حجوجي_2)
    {
        Ref added = add(function, std::move(attr));
        لكل (auto arg : حجوجي_2)
            added.append_arg(arg);
        أرجع added;
    }

    Ref add(Fn ثابت &function, Attr ثابت &attr, std::initializer_list<Ref> حجوجي_2)
    {
        Ref added = add(function, attr);
        لكل (auto arg : حجوجي_2)
            added.append_arg(arg);
        أرجع added;
    }

    Ref add(Fn ثابت &function, Attr &&attr, std::initializer_list<Ref> حجوجي_2)
    {
        Ref added = add(function, std::move(attr));
        لكل (auto arg : حجوجي_2)
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

    فراغ compact_حجوجي()
    {
        std::vector<صحيح> new_حجوجي;
        لكل (auto &كلي : nodes)
        {
            صحيح new_offset = GetSize(new_حجوجي);
            لكل (صحيح i = 0; i < كلي.arg_كدي; i++)
                new_حجوجي.تور_back(حجوجي_2[كلي.arg_offset + i]);
            كلي.arg_offset = new_offset;
        }
        std::swap(حجوجي_2, new_حجوجي);
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
        std::vector<كلي_2> new_nodes;
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

        compact_حجوجي();
        لكل (صحيح &arg : حجوجي_2)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        لكل (auto &مبودي : مبوداي)
        {
            log_assert(مبودي.second < GetSize(inv_perm));
            log_assert(inv_perm[مبودي.second] >= 0);
            مبودي.second = inv_perm[مبودي.second];
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


        تعريفنوع صحيح كلي_type;

        بنية كلي_enumerator {
        خاص:
            friend بنية SccAdaptor;
            صحيح current, end;
            كلي_enumerator(صحيح current, صحيح end) : current(current), end(end) {}

        عام:

            bool finished() ثابت { أرجع current == end; }
            كلي_type next() {
                log_assert(!finished());
                كلي_type result = current;
                ++current;
                أرجع result;
            }
        };

        كلي_enumerator enumerate_nodes() {
            أرجع كلي_enumerator(0, GetSize(indices_));
        }


        بنية successor_enumerator {
        خاص:
            friend بنية SccAdaptor;
            std::vector<صحيح>::const_iterator current, end;
            successor_enumerator(std::vector<صحيح>::const_iterator current, std::vector<صحيح>::const_iterator end) :
                current(current), end(end) {}

        عام:
            bool finished() ثابت { أرجع current == end; }
            كلي_type next() {
                log_assert(!finished());
                كلي_type result = *current;
                ++current;
                أرجع result;
            }
        };

        successor_enumerator enumerate_successors(صحيح index) ثابت {
            auto ثابت &ref = graph_[index];
            أرجع successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        صحيح &dfs_index(كلي_type ثابت &كلي) { أرجع indices_[كلي]; }

        std::vector<صحيح> ثابت &dfs_indices() { أرجع indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
