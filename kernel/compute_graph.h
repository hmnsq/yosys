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
    typename साँचो_2 = std::tuple<> // Stable keys to refer to nodes
>
संरचना ComputeGraph
{
    संरचना Ref;
गुप्त:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    संरचना गाँठो_2 {
        पूर्णाङ्क fn_index;
        पूर्णाङ्क arg_offset;
        पूर्णाङ्क arg_गन्ती;
        Attr attr;

        गाँठो_2(पूर्णाङ्क fn_index, Attr &&attr, पूर्णाङ्क arg_offset, पूर्णाङ्क arg_गन्ती = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_गन्ती(arg_गन्ती), attr(std::move(attr)) {}

        गाँठो_2(पूर्णाङ्क fn_index, Attr अचर &attr, पूर्णाङ्क arg_offset, पूर्णाङ्क arg_गन्ती = 0)
            : fn_index(fn_index), arg_offset(arg_offset), arg_गन्ती(arg_गन्ती), attr(attr) {}
    };


    std::vector<गाँठो_2> nodes;
    std::vector<पूर्णाङ्क> args;
    dict<साँचो_2, पूर्णाङ्क> साँचोहरू;
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

        शून्य check() अचर { log_assert(index_ < graph_->size()); }

        गाँठो_2 अचर &deref() अचर { check(); फर्काऊ graph_->nodes[index_]; }

    सार्वजनिक:
        ComputeGraph अचर &graph() अचर { फर्काऊ graph_; }
        पूर्णाङ्क index() अचर { फर्काऊ index_; }

        पूर्णाङ्क size() अचर { फर्काऊ deref().arg_गन्ती; }

        BaseRef arg(पूर्णाङ्क n) अचर
        {
            गाँठो_2 अचर &गाँठो = deref();
            log_assert(n >= 0 && n < गाँठो.arg_गन्ती);
            फर्काऊ BaseRef(graph_, graph_->args[गाँठो.arg_offset + n]);
        }

        std::vector<पूर्णाङ्क>::const_iterator arg_indices_cbegin() अचर
        {
            गाँठो_2 अचर &गाँठो = deref();
            फर्काऊ graph_->args.cbegin() + गाँठो.arg_offset;
        }

        std::vector<पूर्णाङ्क>::const_iterator arg_indices_cend() अचर
        {
            गाँठो_2 अचर &गाँठो = deref();
            फर्काऊ graph_->args.cbegin() + गाँठो.arg_offset + गाँठो.arg_गन्ती;
        }

        Fn अचर &function() अचर { फर्काऊ graph_->functions[deref().fn_index]; }
        Attr अचर &attr() अचर { फर्काऊ deref().attr; }

        bool has_sparse_attr() अचर { फर्काऊ graph_->sparse_attrs.गन्ती(index_); }

        SparseAttr अचर &sparse_attr() अचर
        {
            स्वतः found = graph_->sparse_attrs.find(index_);
            log_assert(found != graph_->sparse_attrs.end());
            फर्काऊ found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph अचर>;

    संरचना Ref : सार्वजनिक BaseRef<ComputeGraph>
    {
    गुप्त:
        मित्र संरचना ComputeGraph;
        Ref(ComputeGraph *graph, पूर्णाङ्क index) : BaseRef<ComputeGraph>(graph, index) {}
        गाँठो_2 &deref() अचर { this->check(); फर्काऊ this->graph_->nodes[this->index_]; }

    सार्वजनिक:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.index_) {}

        शून्य set_function(Fn अचर &function) अचर
        {
            deref().fn_index = this->graph_->functions(function);
        }

        Attr &attr() अचर { फर्काऊ deref().attr; }

        शून्य append_arg(ConstRef arg) अचर
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.index());
        }

        शून्य append_arg(पूर्णाङ्क arg) अचर
        {
            log_assert(arg >= 0 && arg < this->graph_->size());
            गाँठो_2 &गाँठो = deref();
            यदि (गाँठो.arg_offset + गाँठो.arg_गन्ती != GetSize(this->graph_->args))
                move_args(गाँठो);
            this->graph_->args.थप_back(arg);
            गाँठो.arg_गन्ती++;
        }

        चालक ConstRef() अचर
        {
            फर्काऊ ConstRef(this->graph_, this->index_);
        }

        SparseAttr &sparse_attr() अचर
        {
            फर्काऊ this->graph_->sparse_attrs[this->index_];
        }

        शून्य clear_sparse_attr() अचर
        {
            this->graph_->sparse_attrs.erase(this->index_);
        }

        शून्य assign_साँचो(साँचो_2 अचर &साँचो) अचर
        {
            this->graph_->साँचोहरू.emplace(साँचो, this->index_);
        }

    गुप्त:
        शून्य move_args(गाँठो_2 &गाँठो) अचर
        {
            स्वतः &args = this->graph_->args;
            पूर्णाङ्क old_offset = गाँठो.arg_offset;
            गाँठो.arg_offset = GetSize(args);
            लागि (पूर्णाङ्क i = 0; i != गाँठो.arg_गन्ती; ++i)
                args.थप_back(args[old_offset + i]);
        }

    };

    bool has_साँचो(साँचो_2 अचर &साँचो) अचर
    {
        फर्काऊ साँचोहरू.गन्ती(साँचो);
    }

    dict<साँचो_2, पूर्णाङ्क> अचर &साँचोहरू_2() अचर
    {
        फर्काऊ साँचोहरू;
    }

    ConstRef चालक()(साँचो_2 अचर &साँचो) अचर
    {
        स्वतः it = साँचोहरू.find(साँचो);
        log_assert(it != साँचोहरू.end());
        फर्काऊ (*this)[it->second];
    }

    Ref चालक()(साँचो_2 अचर &साँचो)
    {
        स्वतः it = साँचोहरू.find(साँचो);
        log_assert(it != साँचोहरू.end());
        फर्काऊ (*this)[it->second];
    }

    पूर्णाङ्क size() अचर { फर्काऊ GetSize(nodes); }

    ConstRef चालक[](पूर्णाङ्क index) अचर { फर्काऊ ConstRef(this, index); }
    Ref चालक[](पूर्णाङ्क index) { फर्काऊ Ref(this, index); }

    Ref add(Fn अचर &function, Attr &&attr)
    {
        पूर्णाङ्क index = GetSize(nodes);
        पूर्णाङ्क fn_index = functions(function);
        nodes.emplace_back(fn_index, std::move(attr), GetSize(args));
        फर्काऊ Ref(this, index);
    }

    Ref add(Fn अचर &function, Attr अचर &attr)
    {
        पूर्णाङ्क index = GetSize(nodes);
        पूर्णाङ्क fn_index = functions(function);
        nodes.emplace_back(fn_index, attr,  GetSize(args));
        फर्काऊ Ref(this, index);
    }

    ढाँचा<typename T>
    Ref add(Fn अचर &function, Attr अचर &attr, T &&args)
    {
        Ref added = add(function, attr);
        लागि (स्वतः arg : args)
            added.append_arg(arg);
        फर्काऊ added;
    }

    ढाँचा<typename T>
    Ref add(Fn अचर &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        लागि (स्वतः arg : args)
            added.append_arg(arg);
        फर्काऊ added;
    }

    Ref add(Fn अचर &function, Attr अचर &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        लागि (स्वतः arg : args)
            added.append_arg(arg);
        फर्काऊ added;
    }

    Ref add(Fn अचर &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        लागि (स्वतः arg : args)
            added.append_arg(arg);
        फर्काऊ added;
    }

    ढाँचा<typename T>
    Ref add(Fn अचर &function, Attr अचर &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        लागि (; begin != end; ++begin)
            added.append_arg(*begin);
        फर्काऊ added;
    }

    शून्य compact_args()
    {
        std::vector<पूर्णाङ्क> new_args;
        लागि (स्वतः &गाँठो : nodes)
        {
            पूर्णाङ्क new_offset = GetSize(new_args);
            लागि (पूर्णाङ्क i = 0; i < गाँठो.arg_गन्ती; i++)
                new_args.थप_back(args[गाँठो.arg_offset + i]);
            गाँठो.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    शून्य permute(std::vector<पूर्णाङ्क> अचर &perm)
    {
        log_assert(perm.size() <= nodes.size());
        std::vector<पूर्णाङ्क> inv_perm;
        inv_perm.resize(nodes.size(), -1);
        लागि (पूर्णाङ्क i = 0; i < GetSize(perm); ++i)
        {
            पूर्णाङ्क j = perm[i];
            log_assert(j >= 0 && j < GetSize(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    शून्य permute(std::vector<पूर्णाङ्क> अचर &perm, std::vector<पूर्णाङ्क> अचर &inv_perm)
    {
        log_assert(inv_perm.size() == nodes.size());
        std::vector<गाँठो_2> new_nodes;
        new_nodes.reserve(perm.size());
        dict<पूर्णाङ्क, SparseAttr> new_sparse_attrs;
        लागि (पूर्णाङ्क i : perm)
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
        लागि (पूर्णाङ्क &arg : args)
        {
            log_assert(arg < GetSize(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        लागि (स्वतः &साँचो : साँचोहरू)
        {
            log_assert(साँचो.second < GetSize(inv_perm));
            log_assert(inv_perm[साँचो.second] >= 0);
            साँचो.second = inv_perm[साँचो.second];
        }
    }

    संरचना SccAdaptor
    {
    गुप्त:
        ComputeGraph अचर &graph_;
        std::vector<पूर्णाङ्क> indices_;
    सार्वजनिक:
        SccAdaptor(ComputeGraph अचर &graph) : graph_(graph)
        {
            indices_.resize(graph.size(), -1);
        }


        प्रकारनिर्देश पूर्णाङ्क गाँठो_type;

        संरचना गाँठो_enumerator {
        गुप्त:
            मित्र संरचना SccAdaptor;
            पूर्णाङ्क current, end;
            गाँठो_enumerator(पूर्णाङ्क current, पूर्णाङ्क end) : current(current), end(end) {}

        सार्वजनिक:

            bool finished() अचर { फर्काऊ current == end; }
            गाँठो_type next() {
                log_assert(!finished());
                गाँठो_type result = current;
                ++current;
                फर्काऊ result;
            }
        };

        गाँठो_enumerator enumerate_nodes() {
            फर्काऊ गाँठो_enumerator(0, GetSize(indices_));
        }


        संरचना successor_enumerator {
        गुप्त:
            मित्र संरचना SccAdaptor;
            std::vector<पूर्णाङ्क>::const_iterator current, end;
            successor_enumerator(std::vector<पूर्णाङ्क>::const_iterator current, std::vector<पूर्णाङ्क>::const_iterator end) :
                current(current), end(end) {}

        सार्वजनिक:
            bool finished() अचर { फर्काऊ current == end; }
            गाँठो_type next() {
                log_assert(!finished());
                गाँठो_type result = *current;
                ++current;
                फर्काऊ result;
            }
        };

        successor_enumerator enumerate_successors(पूर्णाङ्क index) अचर {
            स्वतः अचर &ref = graph_[index];
            फर्काऊ successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        पूर्णाङ्क &dfs_index(गाँठो_type अचर &गाँठो) { फर्काऊ indices_[गाँठो]; }

        std::vector<पूर्णाङ्क> अचर &dfs_indices() { फर्काऊ indices_; }
    };

};



YOSYS_NAMESPACE_END


#endif
