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

#شامل <tuple>
#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

template<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename کلید_2 = std::tuple<> // Stable keys to refer to nodes
>
ساختار ComputeGraph
{
    ساختار Ref;
خصوصی:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    ساختار گره_2 {
        صحیح fn_index;
        صحیح arg_offset;
        صحیح arg_شمار;
        Attr attr;

        گره_2(صحیح fn_index, Attr &&attr, صحیح arg_offset, صحیح arg_شمار = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_شمار(arg_شمار), attr(std::move(attr)) {}

        گره_2(صحیح fn_index, Attr ثابت &attr, صحیح arg_offset, صحیح arg_شمار = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_شمار(arg_شمار), attr(attr) {}
    };


    std::vector<گره_2> nodes;
    std::vector<صحیح> args;
    dict<کلید_2, صحیح> کلیدها;
    dict<صحیح, SparseAttr> sparse_attrs;

عمومی:
    template<typename Graph>
    ساختار BaseRef
    {
    محافظتشده:
        friend ساختار ComputeGraph;
        Graph *graph_;
        صحیح index_;
        BaseRef(Graph *graph, صحیح index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        تهی check() ثابت { log_assert(index_ < graph_->size()); }

        گره_2 ثابت &deref() ثابت { check(); بازگشت graph_->nodes[index_]; }

    عمومی:
        ComputeGraph ثابت &graph() ثابت { بازگشت graph_; }
        صحیح index() ثابت { بازگشت index_; }

        صحیح size() ثابت { بازگشت deref().arg_شمار; }

        BaseRef arg(صحیح n) ثابت
        {
            گره_2 ثابت &گره = deref();
            log_assert(n >= 0 && n < گره.arg_شمار);
            بازگشت BaseRef(graph_, graph_->args[گره.arg_offset + n]);
        }

        std::vector<صحیح>::const_iterator arg_indices_cbegin() ثابت
        {
            گره_2 ثابت &گره = deref();
            بازگشت graph_->args.cbegin() + گره.arg_offset;
        }

        std::vector<صحیح>::const_iterator arg_indices_cend() ثابت
        {
            گره_2 ثابت &گره = deref();
            بازگشت graph_->args.cbegin() + گره.arg_offset + گره.arg_شمار;
        }

        Fn ثابت &function() ثابت { بازگشت graph_->functions[deref().fn_index]; }
        Attr ثابت &attr() ثابت { بازگشت deref().attr; }

        bool has_sparse_attr() ثابت { بازگشت graph_->sparse_attrs.شمار(index_); }

        SparseAttr ثابت &sparse_attr() ثابت
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            بازگشت found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph ثابت>;

    ساختار Ref : عمومی BaseRef<ComputeGraph>
    {
    خصوصی:
        friend ساختار ComputeGraph;
        Ref(ComputeGraph *graph, صحیح index) : BaseRef<ComputeGraph>(graph, index) {}
        گره_2 &deref() ثابت { this->check(); بازگشت this->graph_->nodes[this->index_]; }

    عمومی:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        تهی set_function(Fn ثابت &function) ثابت
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() ثابت { بازگشت deref().attr; }

        تهی append_arg(ConstRef arg) ثابت
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        تهی append_arg(صحیح arg) ثابت
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            گره_2 &گره = deref();
            اگر (گره.arg_offset + گره.arg_شمار != GetSize(this->graph_->args))
                move_args(گره);
            this->graph_->args.افزودن_back(arg);
            گره.arg_شمار++;
        }

        operator ConstRef() ثابت
        {
            بازگشت ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() ثابت
        {
            بازگشت this->graph_->sparse_attrs[this->index_];
        }

        تهی clear_sparse_attr() ثابت
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        تهی assign_کلید(کلید_2 ثابت &کلید) ثابت
        {
            this->graph_->کلیدها.emplace(کلید, this->index_);
        }

    خصوصی:
        تهی move_args(گره_2 &گره) ثابت
        {
            auto &args = this->graph_->args;
            صحیح old_offset = گره.arg_offset;
            گره.arg_offset = GetSize(args);
            برای (صحیح i = 0; i != گره.arg_شمار; ++i)
                args.افزودن_back(args[old_offset + i]);
        }

    };

    bool has_کلید(کلید_2 ثابت &کلید) ثابت
    {
        بازگشت کلیدها.شمار(کلید);
    }

    dict<کلید_2, صحیح> ثابت &کلیدها_2() ثابت
    {
        بازگشت کلیدها;
    }

    ConstRef operator()(کلید_2 ثابت &کلید) ثابت
    {
        auto it = کلیدها.find(کلید);
        log_assert(it != کلیدها.end());
        بازگشت (*this)[it->second];
    }

    Ref operator()(کلید_2 ثابت &کلید)
    {
        auto it = کلیدها.find(کلید);
        log_assert(it != کلیدها.end());
        بازگشت (*this)[it->second];
    }

    صحیح size() ثابت { بازگشت GetSize(nodes); }

    ConstRef operator[](صحیح index) ثابت { بازگشت ConstRef(this, index); }
    Ref operator[](صحیح index) { بازگشت Ref(this, index); }

    Ref add(Fn ثابت &function, Attr &&attr)
    {
        صحیح index = GetSize(nodes);
        صحیح fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        بازگشت Ref(this, index);
    }

    Ref add(Fn ثابت &function, Attr ثابت &attr)
    {
        صحیح index = GetSize(nodes);
        صحیح fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        بازگشت Ref(this, index);
    }

    template<typename T>
    Ref add(Fn ثابت &function, Attr ثابت &attr, T &&args)
    {
        Ref added = add(function, attr);
        برای (auto arg : args)
            added.append_arg(arg);
        بازگشت added;
    }

    template<typename T>
    Ref add(Fn ثابت &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        برای (auto arg : args)
            added.append_arg(arg);
        بازگشت added;
    }

    Ref add(Fn ثابت &function, Attr ثابت &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        برای (auto arg : args)
            added.append_arg(arg);
        بازگشت added;
    }

    Ref add(Fn ثابت &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        برای (auto arg : args)
            added.append_arg(arg);
        بازگشت added;
    }

    template<typename T>
    Ref add(Fn ثابت &function, Attr ثابت &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        برای (; begin != end; ++begin)
            added.append_arg(*begin);
        بازگشت added;
    }

    تهی compact_args()
    {
        std::vector<صحیح> new_args;
        برای (auto &گره : nodes)
        {
            صحیح new_offset = GetSize(new_args);
            برای (صحیح i = 0; i < گره.arg_شمار; i++)
                new_args.افزودن_back(args[گره.arg_offset + i]);
            گره.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    تهی permute(std::vector<صحیح> ثابت &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<صحیح> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        برای (صحیح i = 0; i < GetSize(perm); ++i)
        {
            صحیح j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    تهی permute(std::vector<صحیح> ثابت &perm, std::vector<صحیح> ثابت &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<گره_2> new_nodes;
        new_nodes.reserve(perm.size());
        dict<صحیح, SparseAttr> new_sparse_attrs;
        برای (صحیح i : perm)
        {
            صحیح j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            اگر (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        برای (صحیح &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        برای (auto &کلید : کلیدها)
        {
            log_assert(کلید.second < GetSize(inv_perm));
            log_assert(inv_perm[کلید.second] >= 0);
            کلید.second = inv_perm[کلید.second];
        }
    }

    ساختار SccAdaptor
    {
    خصوصی:
        ComputeGraph ثابت &graph_;
        std::vector<صحیح> indices_;
    عمومی:
        SccAdaptor(ComputeGraph ثابت &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        تعریفنوع صحیح گره_type;

        ساختار گره_enumerator {
        خصوصی:
            friend ساختار SccAdaptor;
            صحیح current, end;
            گره_enumerator(صحیح current, صحیح end) : current(current), end(end) {}

        عمومی:

            bool finished() ثابت { بازگشت current == end; }
            گره_type next() {
                log_assert(!finished());
                گره_type result = current;
                ++current;
                بازگشت result;
            }
        };

        گره_enumerator enumerate_nodes() {
            بازگشت گره_enumerator(0, GetSize(indices_));
        }


        ساختار successor_enumerator {
        خصوصی:
            friend ساختار SccAdaptor;
            std::vector<صحیح>::const_iterator current, end;
            successor_enumerator(std::vector<صحیح>::const_iterator current, std::vector<صحیح>::const_iterator end) :
                current(current), end(end) {}

        عمومی:
            bool finished() ثابت { بازگشت current == end; }
            گره_type next() {
                log_assert(!finished());
                گره_type result = *current;
                ++current;
                بازگشت result;
            }
        };

        successor_enumerator enumerate_successors(صحیح index) ثابت {
            auto ثابت &ref = graph_[index];
            بازگشت successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        صحیح &dfs_index(گره_type ثابت &گره) { بازگشت indices_[گره]; }

        std::vector<صحیح> ثابت &dfs_indices() { بازگشت indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
