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

ढाँचा<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename कुंजी_2 = std::tuple<> // Stable keys to refer to nodes
>
संरचना ComputeGraph
{
    संरचना Ref;
गुप्त:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    संरचना गाँठ_2 {
        पूर्णाङ्क fn_index;
        पूर्णाङ्क arg_offset;
        पूर्णाङ्क arg_गिनती;
        Attr attr;

        गाँठ_2(पूर्णाङ्क fn_index, Attr &&attr, पूर्णाङ्क arg_offset, पूर्णाङ्क arg_गिनती = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_गिनती(arg_गिनती), attr(std::move(attr)) {}

        गाँठ_2(पूर्णाङ्क fn_index, Attr ध्रुव &attr, पूर्णाङ्क arg_offset, पूर्णाङ्क arg_गिनती = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_गिनती(arg_गिनती), attr(attr) {}
    };


    std::vector<गाँठ_2> nodes;
    std::vector<पूर्णाङ्क> args;
    dict<कुंजी_2, पूर्णाङ्क> कुंजियाँ;
    dict<पूर्णाङ्क, SparseAttr> sparse_attrs;

सार्वजनिक:
    ढाँचा<typename Graph>
    संरचना BaseRef
    {
    रक्षित:
        मित्र संरचना ComputeGraph;
        Graph *graph_;
        पूर्णाङ्क index_;
        BaseRef(Graph *graph, पूर्णाङ्क index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        शून्य check() ध्रुव { log_assert(index_ < graph_->size()); }

        गाँठ_2 ध्रुव &deref() ध्रुव { check(); प्रत्यागम graph_->nodes[index_]; }

    सार्वजनिक:
        ComputeGraph ध्रुव &graph() ध्रुव { प्रत्यागम graph_; }
        पूर्णाङ्क index() ध्रुव { प्रत्यागम index_; }

        पूर्णाङ्क size() ध्रुव { प्रत्यागम deref().arg_गिनती; }

        BaseRef arg(पूर्णाङ्क n) ध्रुव
        {
            गाँठ_2 ध्रुव &गाँठ = deref();
            log_assert(n >= 0 && n < गाँठ.arg_गिनती);
            प्रत्यागम BaseRef(graph_, graph_->args[गाँठ.arg_offset + n]);
        }

        std::vector<पूर्णाङ्क>::const_iterator arg_indices_cbegin() ध्रुव
        {
            गाँठ_2 ध्रुव &गाँठ = deref();
            प्रत्यागम graph_->args.cbegin() + गाँठ.arg_offset;
        }

        std::vector<पूर्णाङ्क>::const_iterator arg_indices_cend() ध्रुव
        {
            गाँठ_2 ध्रुव &गाँठ = deref();
            प्रत्यागम graph_->args.cbegin() + गाँठ.arg_offset + गाँठ.arg_गिनती;
        }

        Fn ध्रुव &function() ध्रुव { प्रत्यागम graph_->functions[deref().fn_index]; }
        Attr ध्रुव &attr() ध्रुव { प्रत्यागम deref().attr; }

        bool has_sparse_attr() ध्रुव { प्रत्यागम graph_->sparse_attrs.गिनती(index_); }

        SparseAttr ध्रुव &sparse_attr() ध्रुव
        {
            स्वतः found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            प्रत्यागम found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph ध्रुव>;

    संरचना Ref : सार्वजनिक BaseRef<ComputeGraph>
    {
    गुप्त:
        मित्र संरचना ComputeGraph;
        Ref(ComputeGraph *graph, पूर्णाङ्क index) : BaseRef<ComputeGraph>(graph, index) {}
        गाँठ_2 &deref() ध्रुव { this->check(); प्रत्यागम this->graph_->nodes[this->index_]; }

    सार्वजनिक:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        शून्य set_function(Fn ध्रुव &function) ध्रुव
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() ध्रुव { प्रत्यागम deref().attr; }

        शून्य append_arg(ConstRef arg) ध्रुव
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        शून्य append_arg(पूर्णाङ्क arg) ध्रुव
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            गाँठ_2 &गाँठ = deref();
            यदि (गाँठ.arg_offset + गाँठ.arg_गिनती != GetSize(this->graph_->args))
                move_args(गाँठ);
            this->graph_->args.धकेलो_back(arg);
            गाँठ.arg_गिनती++;
        }

        चालक ConstRef() ध्रुव
        {
            प्रत्यागम ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() ध्रुव
        {
            प्रत्यागम this->graph_->sparse_attrs[this->index_];
        }

        शून्य clear_sparse_attr() ध्रुव
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        शून्य assign_कुंजी(कुंजी_2 ध्रुव &कुंजी) ध्रुव
        {
            this->graph_->कुंजियाँ.emplace(कुंजी, this->index_);
        }

    गुप्त:
        शून्य move_args(गाँठ_2 &गाँठ) ध्रुव
        {
            स्वतः &args = this->graph_->args;
            पूर्णाङ्क old_offset = गाँठ.arg_offset;
            गाँठ.arg_offset = GetSize(args);
            प्रति (पूर्णाङ्क i = 0; i != गाँठ.arg_गिनती; ++i)
                args.धकेलो_back(args[old_offset + i]);
        }

    };

    bool has_कुंजी(कुंजी_2 ध्रुव &कुंजी) ध्रुव
    {
        प्रत्यागम कुंजियाँ.गिनती(कुंजी);
    }

    dict<कुंजी_2, पूर्णाङ्क> ध्रुव &कुंजियाँ_2() ध्रुव
    {
        प्रत्यागम कुंजियाँ;
    }

    ConstRef चालक()(कुंजी_2 ध्रुव &कुंजी) ध्रुव
    {
        स्वतः it = कुंजियाँ.find(कुंजी);
        log_assert(it != कुंजियाँ.end());
        प्रत्यागम (*this)[it->second];
    }

    Ref चालक()(कुंजी_2 ध्रुव &कुंजी)
    {
        स्वतः it = कुंजियाँ.find(कुंजी);
        log_assert(it != कुंजियाँ.end());
        प्रत्यागम (*this)[it->second];
    }

    पूर्णाङ्क size() ध्रुव { प्रत्यागम GetSize(nodes); }

    ConstRef चालक[](पूर्णाङ्क index) ध्रुव { प्रत्यागम ConstRef(this, index); }
    Ref चालक[](पूर्णाङ्क index) { प्रत्यागम Ref(this, index); }

    Ref add(Fn ध्रुव &function, Attr &&attr)
    {
        पूर्णाङ्क index = GetSize(nodes);
        पूर्णाङ्क fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        प्रत्यागम Ref(this, index);
    }

    Ref add(Fn ध्रुव &function, Attr ध्रुव &attr)
    {
        पूर्णाङ्क index = GetSize(nodes);
        पूर्णाङ्क fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        प्रत्यागम Ref(this, index);
    }

    ढाँचा<typename T>
    Ref add(Fn ध्रुव &function, Attr ध्रुव &attr, T &&args)
    {
        Ref added = add(function, attr);
        प्रति (स्वतः arg : args)
            added.append_arg(arg);
        प्रत्यागम added;
    }

    ढाँचा<typename T>
    Ref add(Fn ध्रुव &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        प्रति (स्वतः arg : args)
            added.append_arg(arg);
        प्रत्यागम added;
    }

    Ref add(Fn ध्रुव &function, Attr ध्रुव &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        प्रति (स्वतः arg : args)
            added.append_arg(arg);
        प्रत्यागम added;
    }

    Ref add(Fn ध्रुव &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        प्रति (स्वतः arg : args)
            added.append_arg(arg);
        प्रत्यागम added;
    }

    ढाँचा<typename T>
    Ref add(Fn ध्रुव &function, Attr ध्रुव &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        प्रति (; begin != end; ++begin)
            added.append_arg(*begin);
        प्रत्यागम added;
    }

    शून्य compact_args()
    {
        std::vector<पूर्णाङ्क> new_args;
        प्रति (स्वतः &गाँठ : nodes)
        {
            पूर्णाङ्क new_offset = GetSize(new_args);
            प्रति (पूर्णाङ्क i = 0; i < गाँठ.arg_गिनती; i++)
                new_args.धकेलो_back(args[गाँठ.arg_offset + i]);
            गाँठ.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    शून्य permute(std::vector<पूर्णाङ्क> ध्रुव &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<पूर्णाङ्क> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        प्रति (पूर्णाङ्क i = 0; i < GetSize(perm); ++i)
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
        std::vector<गाँठ_2> new_nodes;
        new_nodes.reserve(perm.size());
        dict<पूर्णाङ्क, SparseAttr> new_sparse_attrs;
        प्रति (पूर्णाङ्क i : perm)
        {
            पूर्णाङ्क j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            स्वतः found = sparse_attrs.find(i);
            यदि (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        प्रति (पूर्णाङ्क &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        प्रति (स्वतः &कुंजी : कुंजियाँ)
        {
            log_assert(कुंजी.second < GetSize(inv_perm));
            log_assert(inv_perm[कुंजी.second] >= 0);
            कुंजी.second = inv_perm[कुंजी.second];
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


        प्रकारनिर्देश पूर्णाङ्क गाँठ_type;

        संरचना गाँठ_enumerator {
        गुप्त:
            मित्र संरचना SccAdaptor;
            पूर्णाङ्क current, end;
            गाँठ_enumerator(पूर्णाङ्क current, पूर्णाङ्क end) : current(current), end(end) {}

        सार्वजनिक:

            bool finished() ध्रुव { प्रत्यागम current == end; }
            गाँठ_type next() {
                log_assert(!finished());
                गाँठ_type result = current;
                ++current;
                प्रत्यागम result;
            }
        };

        गाँठ_enumerator enumerate_nodes() {
            प्रत्यागम गाँठ_enumerator(0, GetSize(indices_));
        }


        संरचना successor_enumerator {
        गुप्त:
            मित्र संरचना SccAdaptor;
            std::vector<पूर्णाङ्क>::const_iterator current, end;
            successor_enumerator(std::vector<पूर्णाङ्क>::const_iterator current, std::vector<पूर्णाङ्क>::const_iterator end) :
                current(current), end(end) {}

        सार्वजनिक:
            bool finished() ध्रुव { प्रत्यागम current == end; }
            गाँठ_type next() {
                log_assert(!finished());
                गाँठ_type result = *current;
                ++current;
                प्रत्यागम result;
            }
        };

        successor_enumerator enumerate_successors(पूर्णाङ्क index) ध्रुव {
            स्वतः ध्रुव &ref = graph_[index];
            प्रत्यागम successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        पूर्णाङ्क &dfs_index(गाँठ_type ध्रुव &गाँठ) { प्रत्यागम indices_[गाँठ]; }

        std::vector<पूर्णाङ्क> ध्रुव &dfs_indices() { प्रत्यागम indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
