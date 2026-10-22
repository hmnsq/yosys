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

#ଅନ୍ତର୍ଭାଵ <tuple>
#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

template<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename ଚାବି_2 = std::tuple<> // Stable keys to refer to nodes
>
ସଂରଚନା ComputeGraph
{
    ସଂରଚନା Ref;
ବ୍ୟକ୍ତିଗତ:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    ସଂରଚନା ଗଣ୍ଠି_2 {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fn_index;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_offset;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_ଗଣନା;
        Attr attr;

        ଗଣ୍ଠି_2(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fn_index, Attr &&attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_ଗଣନା = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_ଗଣନା(arg_ଗଣନା), attr(std::move(attr)) {}

        ଗଣ୍ଠି_2(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fn_index, Attr ଅଚଳ &attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_ଗଣନା = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_ଗଣନା(arg_ଗଣନା), attr(attr) {}
    };


    std::vector<ଗଣ୍ଠି_2> nodes;
    std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> args;
    dict<ଚାବି_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା> ଚାବିଗୁଡ଼ିକ;
    dict<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, SparseAttr> sparse_attrs;

ସର୍ବସାଧାରଣ:
    template<typename Graph>
    ସଂରଚନା BaseRef
    {
    ସୁରକ୍ଷିତ:
        friend ସଂରଚନା ComputeGraph;
        Graph *graph_;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା index_;
        BaseRef(Graph *graph, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        ଶୂନ୍ୟ check() ଅଚଳ { log_assert(index_ < graph_->size()); }

        ଗଣ୍ଠି_2 ଅଚଳ &deref() ଅଚଳ { check(); ଫେରନ୍ତୁ graph_->nodes[index_]; }

    ସର୍ବସାଧାରଣ:
        ComputeGraph ଅଚଳ &graph() ଅଚଳ { ଫେରନ୍ତୁ graph_; }
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା index() ଅଚଳ { ଫେରନ୍ତୁ index_; }

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ { ଫେରନ୍ତୁ deref().arg_ଗଣନା; }

        BaseRef arg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n) ଅଚଳ
        {
            ଗଣ୍ଠି_2 ଅଚଳ &ଗଣ୍ଠି = deref();
            log_assert(n >= 0 && n < ଗଣ୍ଠି.arg_ଗଣନା);
            ଫେରନ୍ତୁ BaseRef(graph_, graph_->args[ଗଣ୍ଠି.arg_offset + n]);
        }

        std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>::const_iterator arg_indices_cbegin() ଅଚଳ
        {
            ଗଣ୍ଠି_2 ଅଚଳ &ଗଣ୍ଠି = deref();
            ଫେରନ୍ତୁ graph_->args.cbegin() + ଗଣ୍ଠି.arg_offset;
        }

        std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>::const_iterator arg_indices_cend() ଅଚଳ
        {
            ଗଣ୍ଠି_2 ଅଚଳ &ଗଣ୍ଠି = deref();
            ଫେରନ୍ତୁ graph_->args.cbegin() + ଗଣ୍ଠି.arg_offset + ଗଣ୍ଠି.arg_ଗଣନା;
        }

        Fn ଅଚଳ &function() ଅଚଳ { ଫେରନ୍ତୁ graph_->functions[deref().fn_index]; }
        Attr ଅଚଳ &attr() ଅଚଳ { ଫେରନ୍ତୁ deref().attr; }

        bool has_sparse_attr() ଅଚଳ { ଫେରନ୍ତୁ graph_->sparse_attrs.ଗଣନା_2(index_); }

        SparseAttr ଅଚଳ &sparse_attr() ଅଚଳ
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            ଫେରନ୍ତୁ found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph ଅଚଳ>;

    ସଂରଚନା Ref : ସର୍ବସାଧାରଣ BaseRef<ComputeGraph>
    {
    ବ୍ୟକ୍ତିଗତ:
        friend ସଂରଚନା ComputeGraph;
        Ref(ComputeGraph *graph, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) : BaseRef<ComputeGraph>(graph, index) {}
        ଗଣ୍ଠି_2 &deref() ଅଚଳ { this->check(); ଫେରନ୍ତୁ this->graph_->nodes[this->index_]; }

    ସର୍ବସାଧାରଣ:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        ଶୂନ୍ୟ set_function(Fn ଅଚଳ &function) ଅଚଳ
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() ଅଚଳ { ଫେରନ୍ତୁ deref().attr; }

        ଶୂନ୍ୟ append_arg(ConstRef arg) ଅଚଳ
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        ଶୂନ୍ୟ append_arg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg) ଅଚଳ
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            ଗଣ୍ଠି_2 &ଗଣ୍ଠି = deref();
            ଯଦି (ଗଣ୍ଠି.arg_offset + ଗଣ୍ଠି.arg_ଗଣନା != GetSize(this->graph_->args))
                move_args(ଗଣ୍ଠି);
            this->graph_->args.ଠେଲ_back(arg);
            ଗଣ୍ଠି.arg_ଗଣନା++;
        }

        operator ConstRef() ଅଚଳ
        {
            ଫେରନ୍ତୁ ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() ଅଚଳ
        {
            ଫେରନ୍ତୁ this->graph_->sparse_attrs[this->index_];
        }

        ଶୂନ୍ୟ clear_sparse_attr() ଅଚଳ
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        ଶୂନ୍ୟ assign_ଚାବି(ଚାବି_2 ଅଚଳ &ଚାବି) ଅଚଳ
        {
            this->graph_->ଚାବିଗୁଡ଼ିକ.emplace(ଚାବି, this->index_);
        }

    ବ୍ୟକ୍ତିଗତ:
        ଶୂନ୍ୟ move_args(ଗଣ୍ଠି_2 &ଗଣ୍ଠି) ଅଚଳ
        {
            auto &args = this->graph_->args;
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_offset = ଗଣ୍ଠି.arg_offset;
            ଗଣ୍ଠି.arg_offset = GetSize(args);
            ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i != ଗଣ୍ଠି.arg_ଗଣନା; ++i)
                args.ଠେଲ_back(args[old_offset + i]);
        }

    };

    bool has_ଚାବି(ଚାବି_2 ଅଚଳ &ଚାବି) ଅଚଳ
    {
        ଫେରନ୍ତୁ ଚାବିଗୁଡ଼ିକ.ଗଣନା_2(ଚାବି);
    }

    dict<ଚାବି_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା> ଅଚଳ &ଚାବିଗୁଡ଼ିକ_2() ଅଚଳ
    {
        ଫେରନ୍ତୁ ଚାବିଗୁଡ଼ିକ;
    }

    ConstRef operator()(ଚାବି_2 ଅଚଳ &ଚାବି) ଅଚଳ
    {
        auto it = ଚାବିଗୁଡ଼ିକ.find(ଚାବି);
        log_assert(it != ଚାବିଗୁଡ଼ିକ.end());
        ଫେରନ୍ତୁ (*this)[it->second];
    }

    Ref operator()(ଚାବି_2 ଅଚଳ &ଚାବି)
    {
        auto it = ଚାବିଗୁଡ଼ିକ.find(ଚାବି);
        log_assert(it != ଚାବିଗୁଡ଼ିକ.end());
        ଫେରନ୍ତୁ (*this)[it->second];
    }

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ { ଫେରନ୍ତୁ GetSize(nodes); }

    ConstRef operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ { ଫେରନ୍ତୁ ConstRef(this, index); }
    Ref operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) { ଫେରନ୍ତୁ Ref(this, index); }

    Ref add(Fn ଅଚଳ &function, Attr &&attr)
    {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = GetSize(nodes);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        ଫେରନ୍ତୁ Ref(this, index);
    }

    Ref add(Fn ଅଚଳ &function, Attr ଅଚଳ &attr)
    {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = GetSize(nodes);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        ଫେରନ୍ତୁ Ref(this, index);
    }

    template<typename T>
    Ref add(Fn ଅଚଳ &function, Attr ଅଚଳ &attr, T &&args)
    {
        Ref added = add(function, attr);
        ପାଇଁ (auto arg : args)
            added.append_arg(arg);
        ଫେରନ୍ତୁ added;
    }

    template<typename T>
    Ref add(Fn ଅଚଳ &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        ପାଇଁ (auto arg : args)
            added.append_arg(arg);
        ଫେରନ୍ତୁ added;
    }

    Ref add(Fn ଅଚଳ &function, Attr ଅଚଳ &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        ପାଇଁ (auto arg : args)
            added.append_arg(arg);
        ଫେରନ୍ତୁ added;
    }

    Ref add(Fn ଅଚଳ &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        ପାଇଁ (auto arg : args)
            added.append_arg(arg);
        ଫେରନ୍ତୁ added;
    }

    template<typename T>
    Ref add(Fn ଅଚଳ &function, Attr ଅଚଳ &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        ପାଇଁ (; begin != end; ++begin)
            added.append_arg(*begin);
        ଫେରନ୍ତୁ added;
    }

    ଶୂନ୍ୟ compact_args()
    {
        std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> new_args;
        ପାଇଁ (auto &ଗଣ୍ଠି : nodes)
        {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_offset = GetSize(new_args);
            ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଗଣ୍ଠି.arg_ଗଣନା; i++)
                new_args.ଠେଲ_back(args[ଗଣ୍ଠି.arg_offset + i]);
            ଗଣ୍ଠି.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    ଶୂନ୍ୟ permute(std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> ଅଚଳ &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(perm); ++i)
        {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    ଶୂନ୍ୟ permute(std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> ଅଚଳ &perm, std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> ଅଚଳ &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<ଗଣ୍ଠି_2> new_nodes;
        new_nodes.reserve(perm.size());
        dict<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, SparseAttr> new_sparse_attrs;
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i : perm)
        {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            ଯଦି (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        ପାଇଁ (auto &ଚାବି : ଚାବିଗୁଡ଼ିକ)
        {
            log_assert(ଚାବି.second < GetSize(inv_perm));
            log_assert(inv_perm[ଚାବି.second] >= 0);
            ଚାବି.second = inv_perm[ଚାବି.second];
        }
    }

    ସଂରଚନା SccAdaptor
    {
    ବ୍ୟକ୍ତିଗତ:
        ComputeGraph ଅଚଳ &graph_;
        std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> indices_;
    ସର୍ବସାଧାରଣ:
        SccAdaptor(ComputeGraph ଅଚଳ &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି_type;

        ସଂରଚନା ଗଣ୍ଠି_enumerator {
        ବ୍ୟକ୍ତିଗତ:
            friend ସଂରଚନା SccAdaptor;
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା current, end;
            ଗଣ୍ଠି_enumerator(ପୂର୍ଣ୍ଣସଂଖ୍ୟା current, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end) : current(current), end(end) {}

        ସର୍ବସାଧାରଣ:

            bool finished() ଅଚଳ { ଫେରନ୍ତୁ current == end; }
            ଗଣ୍ଠି_type next() {
                log_assert(!finished());
                ଗଣ୍ଠି_type result = current;
                ++current;
                ଫେରନ୍ତୁ result;
            }
        };

        ଗଣ୍ଠି_enumerator enumerate_nodes() {
            ଫେରନ୍ତୁ ଗଣ୍ଠି_enumerator(0, GetSize(indices_));
        }


        ସଂରଚନା successor_enumerator {
        ବ୍ୟକ୍ତିଗତ:
            friend ସଂରଚନା SccAdaptor;
            std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>::const_iterator current, end;
            successor_enumerator(std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>::const_iterator current, std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>::const_iterator end) :
                current(current), end(end) {}

        ସର୍ବସାଧାରଣ:
            bool finished() ଅଚଳ { ଫେରନ୍ତୁ current == end; }
            ଗଣ୍ଠି_type next() {
                log_assert(!finished());
                ଗଣ୍ଠି_type result = *current;
                ++current;
                ଫେରନ୍ତୁ result;
            }
        };

        successor_enumerator enumerate_successors(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ {
            auto ଅଚଳ &ref = graph_[index];
            ଫେରନ୍ତୁ successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା &dfs_index(ଗଣ୍ଠି_type ଅଚଳ &ଗଣ୍ଠି) { ଫେରନ୍ତୁ indices_[ଗଣ୍ଠି]; }

        std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> ଅଚଳ &dfs_indices() { ଫେରନ୍ତୁ indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
