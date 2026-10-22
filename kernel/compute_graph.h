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

#अन्तर्भाव <tuple>
#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

template<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename कुञ्चिका_2 = std::tuple<> // Stable keys to refer to nodes
>
संरचना ComputeGraph
{
    संरचना Ref;
गुप्त:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    संरचना ग्रन्थिः_2 {
        पूर्णाङ्क fn_index;
        पूर्णाङ्क arg_offset;
        पूर्णाङ्क arg_गणनम्;
        Attr attr;

        ग्रन्थिः_2(पूर्णाङ्क fn_index, Attr &&attr, पूर्णाङ्क arg_offset, पूर्णाङ्क arg_गणनम् = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_गणनम्(arg_गणनम्), attr(std::move(attr)) {}

        ग्रन्थिः_2(पूर्णाङ्क fn_index, Attr ध्रुव &attr, पूर्णाङ्क arg_offset, पूर्णाङ्क arg_गणनम् = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_गणनम्(arg_गणनम्), attr(attr) {}
    };


    std::vector<ग्रन्थिः_2> nodes;
    std::vector<पूर्णाङ्क> args;
    dict<कुञ्चिका_2, पूर्णाङ्क> कुञ्चिकाः;
    dict<पूर्णाङ्क, SparseAttr> sparse_attrs;

सार्वजनिक:
    template<typename Graph>
    संरचना BaseRef
    {
    रक्षित:
        friend संरचना ComputeGraph;
        Graph *graph_;
        पूर्णाङ्क index_;
        BaseRef(Graph *graph, पूर्णाङ्क index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        शून्य check() ध्रुव { log_assert(index_ < graph_->size()); }

        ग्रन्थिः_2 ध्रुव &deref() ध्रुव { check(); पटिनिवत्त graph_->nodes[index_]; }

    सार्वजनिक:
        ComputeGraph ध्रुव &graph() ध्रुव { पटिनिवत्त graph_; }
        पूर्णाङ्क index() ध्रुव { पटिनिवत्त index_; }

        पूर्णाङ्क size() ध्रुव { पटिनिवत्त deref().arg_गणनम्; }

        BaseRef arg(पूर्णाङ्क n) ध्रुव
        {
            ग्रन्थिः_2 ध्रुव &ग्रन्थिः = deref();
            log_assert(n >= 0 && n < ग्रन्थिः.arg_गणनम्);
            पटिनिवत्त BaseRef(graph_, graph_->args[ग्रन्थिः.arg_offset + n]);
        }

        std::vector<पूर्णाङ्क>::const_iterator arg_indices_cbegin() ध्रुव
        {
            ग्रन्थिः_2 ध्रुव &ग्रन्थिः = deref();
            पटिनिवत्त graph_->args.cbegin() + ग्रन्थिः.arg_offset;
        }

        std::vector<पूर्णाङ्क>::const_iterator arg_indices_cend() ध्रुव
        {
            ग्रन्थिः_2 ध्रुव &ग्रन्थिः = deref();
            पटिनिवत्त graph_->args.cbegin() + ग्रन्थिः.arg_offset + ग्रन्थिः.arg_गणनम्;
        }

        Fn ध्रुव &function() ध्रुव { पटिनिवत्त graph_->functions[deref().fn_index]; }
        Attr ध्रुव &attr() ध्रुव { पटिनिवत्त deref().attr; }

        bool has_sparse_attr() ध्रुव { पटिनिवत्त graph_->sparse_attrs.गणनम्(index_); }

        SparseAttr ध्रुव &sparse_attr() ध्रुव
        {
            auto found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            पटिनिवत्त found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph ध्रुव>;

    संरचना Ref : सार्वजनिक BaseRef<ComputeGraph>
    {
    गुप्त:
        friend संरचना ComputeGraph;
        Ref(ComputeGraph *graph, पूर्णाङ्क index) : BaseRef<ComputeGraph>(graph, index) {}
        ग्रन्थिः_2 &deref() ध्रुव { this->check(); पटिनिवत्त this->graph_->nodes[this->index_]; }

    सार्वजनिक:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        शून्य set_function(Fn ध्रुव &function) ध्रुव
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() ध्रुव { पटिनिवत्त deref().attr; }

        शून्य append_arg(ConstRef arg) ध्रुव
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        शून्य append_arg(पूर्णाङ्क arg) ध्रुव
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            ग्रन्थिः_2 &ग्रन्थिः = deref();
            सचे (ग्रन्थिः.arg_offset + ग्रन्थिः.arg_गणनम् != GetSize(this->graph_->args))
                move_args(ग्रन्थिः);
            this->graph_->args.प्रक्षिप_back(arg);
            ग्रन्थिः.arg_गणनम्++;
        }

        operator ConstRef() ध्रुव
        {
            पटिनिवत्त ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() ध्रुव
        {
            पटिनिवत्त this->graph_->sparse_attrs[this->index_];
        }

        शून्य clear_sparse_attr() ध्रुव
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        शून्य assign_कुञ्चिका(कुञ्चिका_2 ध्रुव &कुञ्चिका) ध्रुव
        {
            this->graph_->कुञ्चिकाः.emplace(कुञ्चिका, this->index_);
        }

    गुप्त:
        शून्य move_args(ग्रन्थिः_2 &ग्रन्थिः) ध्रुव
        {
            auto &args = this->graph_->args;
            पूर्णाङ्क old_offset = ग्रन्थिः.arg_offset;
            ग्रन्थिः.arg_offset = GetSize(args);
            अत्थाय (पूर्णाङ्क i = 0; i != ग्रन्थिः.arg_गणनम्; ++i)
                args.प्रक्षिप_back(args[old_offset + i]);
        }

    };

    bool has_कुञ्चिका(कुञ्चिका_2 ध्रुव &कुञ्चिका) ध्रुव
    {
        पटिनिवत्त कुञ्चिकाः.गणनम्(कुञ्चिका);
    }

    dict<कुञ्चिका_2, पूर्णाङ्क> ध्रुव &कुञ्चिकाः_2() ध्रुव
    {
        पटिनिवत्त कुञ्चिकाः;
    }

    ConstRef operator()(कुञ्चिका_2 ध्रुव &कुञ्चिका) ध्रुव
    {
        auto it = कुञ्चिकाः.find(कुञ्चिका);
        log_assert(it != कुञ्चिकाः.end());
        पटिनिवत्त (*this)[it->second];
    }

    Ref operator()(कुञ्चिका_2 ध्रुव &कुञ्चिका)
    {
        auto it = कुञ्चिकाः.find(कुञ्चिका);
        log_assert(it != कुञ्चिकाः.end());
        पटिनिवत्त (*this)[it->second];
    }

    पूर्णाङ्क size() ध्रुव { पटिनिवत्त GetSize(nodes); }

    ConstRef operator[](पूर्णाङ्क index) ध्रुव { पटिनिवत्त ConstRef(this, index); }
    Ref operator[](पूर्णाङ्क index) { पटिनिवत्त Ref(this, index); }

    Ref add(Fn ध्रुव &function, Attr &&attr)
    {
        पूर्णाङ्क index = GetSize(nodes);
        पूर्णाङ्क fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        पटिनिवत्त Ref(this, index);
    }

    Ref add(Fn ध्रुव &function, Attr ध्रुव &attr)
    {
        पूर्णाङ्क index = GetSize(nodes);
        पूर्णाङ्क fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        पटिनिवत्त Ref(this, index);
    }

    template<typename T>
    Ref add(Fn ध्रुव &function, Attr ध्रुव &attr, T &&args)
    {
        Ref added = add(function, attr);
        अत्थाय (auto arg : args)
            added.append_arg(arg);
        पटिनिवत्त added;
    }

    template<typename T>
    Ref add(Fn ध्रुव &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        अत्थाय (auto arg : args)
            added.append_arg(arg);
        पटिनिवत्त added;
    }

    Ref add(Fn ध्रुव &function, Attr ध्रुव &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        अत्थाय (auto arg : args)
            added.append_arg(arg);
        पटिनिवत्त added;
    }

    Ref add(Fn ध्रुव &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        अत्थाय (auto arg : args)
            added.append_arg(arg);
        पटिनिवत्त added;
    }

    template<typename T>
    Ref add(Fn ध्रुव &function, Attr ध्रुव &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        अत्थाय (; begin != end; ++begin)
            added.append_arg(*begin);
        पटिनिवत्त added;
    }

    शून्य compact_args()
    {
        std::vector<पूर्णाङ्क> new_args;
        अत्थाय (auto &ग्रन्थिः : nodes)
        {
            पूर्णाङ्क new_offset = GetSize(new_args);
            अत्थाय (पूर्णाङ्क i = 0; i < ग्रन्थिः.arg_गणनम्; i++)
                new_args.प्रक्षिप_back(args[ग्रन्थिः.arg_offset + i]);
            ग्रन्थिः.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    शून्य permute(std::vector<पूर्णाङ्क> ध्रुव &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<पूर्णाङ्क> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        अत्थाय (पूर्णाङ्क i = 0; i < GetSize(perm); ++i)
        {
            पूर्णाङ्क j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    शून्य permute(std::vector<पूर्णाङ्क> ध्रुव &perm, std::vector<पूर्णाङ्क> ध्रुव &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<ग्रन्थिः_2> new_nodes;
        new_nodes.reserve(perm.size());
        dict<पूर्णाङ्क, SparseAttr> new_sparse_attrs;
        अत्थाय (पूर्णाङ्क i : perm)
        {
            पूर्णाङ्क j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            auto found = sparse_attrs.find(i);
            सचे (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        अत्थाय (पूर्णाङ्क &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        अत्थाय (auto &कुञ्चिका : कुञ्चिकाः)
        {
            log_assert(कुञ्चिका.second < GetSize(inv_perm));
            log_assert(inv_perm[कुञ्चिका.second] >= 0);
            कुञ्चिका.second = inv_perm[कुञ्चिका.second];
        }
    }

    संरचना SccAdaptor
    {
    गुप्त:
        ComputeGraph ध्रुव &graph_;
        std::vector<पूर्णाङ्क> indices_;
    सार्वजनिक:
        SccAdaptor(ComputeGraph ध्रुव &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        प्रकारनिर्देश पूर्णाङ्क ग्रन्थिः_type;

        संरचना ग्रन्थिः_enumerator {
        गुप्त:
            friend संरचना SccAdaptor;
            पूर्णाङ्क current, end;
            ग्रन्थिः_enumerator(पूर्णाङ्क current, पूर्णाङ्क end) : current(current), end(end) {}

        सार्वजनिक:

            bool finished() ध्रुव { पटिनिवत्त current == end; }
            ग्रन्थिः_type next() {
                log_assert(!finished());
                ग्रन्थिः_type result = current;
                ++current;
                पटिनिवत्त result;
            }
        };

        ग्रन्थिः_enumerator enumerate_nodes() {
            पटिनिवत्त ग्रन्थिः_enumerator(0, GetSize(indices_));
        }


        संरचना successor_enumerator {
        गुप्त:
            friend संरचना SccAdaptor;
            std::vector<पूर्णाङ्क>::const_iterator current, end;
            successor_enumerator(std::vector<पूर्णाङ्क>::const_iterator current, std::vector<पूर्णाङ्क>::const_iterator end) :
                current(current), end(end) {}

        सार्वजनिक:
            bool finished() ध्रुव { पटिनिवत्त current == end; }
            ग्रन्थिः_type next() {
                log_assert(!finished());
                ग्रन्थिः_type result = *current;
                ++current;
                पटिनिवत्त result;
            }
        };

        successor_enumerator enumerate_successors(पूर्णाङ्क index) ध्रुव {
            auto ध्रुव &ref = graph_[index];
            पटिनिवत्त successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        पूर्णाङ्क &dfs_index(ग्रन्थिः_type ध्रुव &ग्रन्थिः) { पटिनिवत्त indices_[ग्रन्थिः]; }

        std::vector<पूर्णाङ्क> ध्रुव &dfs_indices() { पटिनिवत्त indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
