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
    typename किल्ली_2 = std::tuple<> // Stable keys to refer to nodes
>
संरचना ComputeGraph
{
    संरचना Ref;
खाजगी:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    संरचना गाठ_2 {
        पूर्णांक fn_index;
        पूर्णांक arg_offset;
        पूर्णांक arg_मोजणी;
        Attr attr;

        गाठ_2(पूर्णांक fn_index, Attr &&attr, पूर्णांक arg_offset, पूर्णांक arg_मोजणी = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_मोजणी(arg_मोजणी), attr(std::move(attr)) {}

        गाठ_2(पूर्णांक fn_index, Attr स्थिरांक &attr, पूर्णांक arg_offset, पूर्णांक arg_मोजणी = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_मोजणी(arg_मोजणी), attr(attr) {}
    };


    std::vector<गाठ_2> nodes;
    std::vector<पूर्णांक> args;
    dict<किल्ली_2, पूर्णांक> किल्ल्या;
    dict<पूर्णांक, SparseAttr> sparse_attrs;

सार्वजनिक:
    ढाँचा<typename Graph>
    संरचना BaseRef
    {
    संरक्षित:
        मित्र संरचना ComputeGraph;
        Graph *graph_;
        पूर्णांक index_;
        BaseRef(Graph *graph, पूर्णांक index) : graph_(graph), index_(index) {
            log_assert(index_ >= 0);
            check();
        }

        शून्य check() स्थिरांक { log_assert(index_ < graph_->size()); }

        गाठ_2 स्थिरांक &deref() स्थिरांक { check(); परत graph_->nodes[index_]; }

    सार्वजनिक:
        ComputeGraph स्थिरांक &graph() स्थिरांक { परत graph_; }
        पूर्णांक index() स्थिरांक { परत index_; }

        पूर्णांक size() स्थिरांक { परत deref().arg_मोजणी; }

        BaseRef arg(पूर्णांक n) स्थिरांक
        {
            गाठ_2 स्थिरांक &गाठ = deref();
            log_assert(n >= 0 && n < गाठ.arg_मोजणी);
            परत BaseRef(graph_, graph_->args[गाठ.arg_offset + n]);
        }

        std::vector<पूर्णांक>::const_iterator arg_indices_cbegin() स्थिरांक
        {
            गाठ_2 स्थिरांक &गाठ = deref();
            परत graph_->args.cbegin() + गाठ.arg_offset;
        }

        std::vector<पूर्णांक>::const_iterator arg_indices_cend() स्थिरांक
        {
            गाठ_2 स्थिरांक &गाठ = deref();
            परत graph_->args.cbegin() + गाठ.arg_offset + गाठ.arg_मोजणी;
        }

        Fn स्थिरांक &function() स्थिरांक { परत graph_->functions[deref().fn_index]; }
        Attr स्थिरांक &attr() स्थिरांक { परत deref().attr; }

        bool has_sparse_attr() स्थिरांक { परत graph_->sparse_attrs.मोजणी(index_); }

        SparseAttr स्थिरांक &sparse_attr() स्थिरांक
        {
            स्वतः found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            परत found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph स्थिरांक>;

    संरचना Ref : सार्वजनिक BaseRef<ComputeGraph>
    {
    खाजगी:
        मित्र संरचना ComputeGraph;
        Ref(ComputeGraph *graph, पूर्णांक index) : BaseRef<ComputeGraph>(graph, index) {}
        गाठ_2 &deref() स्थिरांक { this->check(); परत this->graph_->nodes[this->index_]; }

    सार्वजनिक:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        शून्य set_function(Fn स्थिरांक &function) स्थिरांक
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() स्थिरांक { परत deref().attr; }

        शून्य append_arg(ConstRef arg) स्थिरांक
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        शून्य append_arg(पूर्णांक arg) स्थिरांक
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            गाठ_2 &गाठ = deref();
            जर (गाठ.arg_offset + गाठ.arg_मोजणी != GetSize(this->graph_->args))
                move_args(गाठ);
            this->graph_->args.ढकला_back(arg);
            गाठ.arg_मोजणी++;
        }

        चालक ConstRef() स्थिरांक
        {
            परत ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() स्थिरांक
        {
            परत this->graph_->sparse_attrs[this->index_];
        }

        शून्य clear_sparse_attr() स्थिरांक
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        शून्य assign_किल्ली(किल्ली_2 स्थिरांक &किल्ली) स्थिरांक
        {
            this->graph_->किल्ल्या.emplace(किल्ली, this->index_);
        }

    खाजगी:
        शून्य move_args(गाठ_2 &गाठ) स्थिरांक
        {
            स्वतः &args = this->graph_->args;
            पूर्णांक old_offset = गाठ.arg_offset;
            गाठ.arg_offset = GetSize(args);
            साठी (पूर्णांक i = 0; i != गाठ.arg_मोजणी; ++i)
                args.ढकला_back(args[old_offset + i]);
        }

    };

    bool has_किल्ली(किल्ली_2 स्थिरांक &किल्ली) स्थिरांक
    {
        परत किल्ल्या.मोजणी(किल्ली);
    }

    dict<किल्ली_2, पूर्णांक> स्थिरांक &किल्ल्या_2() स्थिरांक
    {
        परत किल्ल्या;
    }

    ConstRef चालक()(किल्ली_2 स्थिरांक &किल्ली) स्थिरांक
    {
        स्वतः it = किल्ल्या.find(किल्ली);
        log_assert(it != किल्ल्या.end());
        परत (*this)[it->second];
    }

    Ref चालक()(किल्ली_2 स्थिरांक &किल्ली)
    {
        स्वतः it = किल्ल्या.find(किल्ली);
        log_assert(it != किल्ल्या.end());
        परत (*this)[it->second];
    }

    पूर्णांक size() स्थिरांक { परत GetSize(nodes); }

    ConstRef चालक[](पूर्णांक index) स्थिरांक { परत ConstRef(this, index); }
    Ref चालक[](पूर्णांक index) { परत Ref(this, index); }

    Ref add(Fn स्थिरांक &function, Attr &&attr)
    {
        पूर्णांक index = GetSize(nodes);
        पूर्णांक fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        परत Ref(this, index);
    }

    Ref add(Fn स्थिरांक &function, Attr स्थिरांक &attr)
    {
        पूर्णांक index = GetSize(nodes);
        पूर्णांक fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        परत Ref(this, index);
    }

    ढाँचा<typename T>
    Ref add(Fn स्थिरांक &function, Attr स्थिरांक &attr, T &&args)
    {
        Ref added = add(function, attr);
        साठी (स्वतः arg : args)
            added.append_arg(arg);
        परत added;
    }

    ढाँचा<typename T>
    Ref add(Fn स्थिरांक &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        साठी (स्वतः arg : args)
            added.append_arg(arg);
        परत added;
    }

    Ref add(Fn स्थिरांक &function, Attr स्थिरांक &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        साठी (स्वतः arg : args)
            added.append_arg(arg);
        परत added;
    }

    Ref add(Fn स्थिरांक &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        साठी (स्वतः arg : args)
            added.append_arg(arg);
        परत added;
    }

    ढाँचा<typename T>
    Ref add(Fn स्थिरांक &function, Attr स्थिरांक &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        साठी (; begin != end; ++begin)
            added.append_arg(*begin);
        परत added;
    }

    शून्य compact_args()
    {
        std::vector<पूर्णांक> new_args;
        साठी (स्वतः &गाठ : nodes)
        {
            पूर्णांक new_offset = GetSize(new_args);
            साठी (पूर्णांक i = 0; i < गाठ.arg_मोजणी; i++)
                new_args.ढकला_back(args[गाठ.arg_offset + i]);
            गाठ.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    शून्य permute(std::vector<पूर्णांक> स्थिरांक &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<पूर्णांक> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        साठी (पूर्णांक i = 0; i < GetSize(perm); ++i)
        {
            पूर्णांक j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    शून्य permute(std::vector<पूर्णांक> स्थिरांक &perm, std::vector<पूर्णांक> स्थिरांक &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<गाठ_2> new_nodes;
        new_nodes.reserve(perm.size());
        dict<पूर्णांक, SparseAttr> new_sparse_attrs;
        साठी (पूर्णांक i : perm)
        {
            पूर्णांक j = GetSize(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            स्वतः found = sparse_attrs.find(i);
            जर (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        साठी (पूर्णांक &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        साठी (स्वतः &किल्ली : किल्ल्या)
        {
            log_assert(किल्ली.second < GetSize(inv_perm));
            log_assert(inv_perm[किल्ली.second] >= 0);
            किल्ली.second = inv_perm[किल्ली.second];
        }
    }

    संरचना SccAdaptor
    {
    खाजगी:
        ComputeGraph स्थिरांक &graph_;
        std::vector<पूर्णांक> indices_;
    सार्वजनिक:
        SccAdaptor(ComputeGraph स्थिरांक &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        प्रकारव्याख्या पूर्णांक गाठ_type;

        संरचना गाठ_enumerator {
        खाजगी:
            मित्र संरचना SccAdaptor;
            पूर्णांक current, end;
            गाठ_enumerator(पूर्णांक current, पूर्णांक end) : current(current), end(end) {}

        सार्वजनिक:

            bool finished() स्थिरांक { परत current == end; }
            गाठ_type next() {
                log_assert(!finished());
                गाठ_type result = current;
                ++current;
                परत result;
            }
        };

        गाठ_enumerator enumerate_nodes() {
            परत गाठ_enumerator(0, GetSize(indices_));
        }


        संरचना successor_enumerator {
        खाजगी:
            मित्र संरचना SccAdaptor;
            std::vector<पूर्णांक>::const_iterator current, end;
            successor_enumerator(std::vector<पूर्णांक>::const_iterator current, std::vector<पूर्णांक>::const_iterator end) :
                current(current), end(end) {}

        सार्वजनिक:
            bool finished() स्थिरांक { परत current == end; }
            गाठ_type next() {
                log_assert(!finished());
                गाठ_type result = *current;
                ++current;
                परत result;
            }
        };

        successor_enumerator enumerate_successors(पूर्णांक index) स्थिरांक {
            स्वतः स्थिरांक &ref = graph_[index];
            परत successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        पूर्णांक &dfs_index(गाठ_type स्थिरांक &गाठ) { परत indices_[गाठ]; }

        std::vector<पूर्णांक> स्थिरांक &dfs_indices() { परत indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
