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

#अगर_अघोषित COMPUTE_GRAPH_H
#घोषणा COMPUTE_GRAPH_H

#समावेश <tuple>
#समावेश "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ढाँचा<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename कुंजी_2 = std::tuple<> // Stable keys to refer to nodes
>
काष्ठा ComputeGraph
{
    काष्ठा Ref;
निजी:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    काष्ठा गाँठ_2 {
        पूर्णांक fn_सूचकांक;
        पूर्णांक arg_offset;
        पूर्णांक arg_गिनती;
        Attr attr;

        गाँठ_2(पूर्णांक fn_सूचकांक, Attr &&attr, पूर्णांक arg_offset, पूर्णांक arg_गिनती = 0)
            : fn_सूचकांक(fn_सूचकांक), arg_offset(arg_offset), arg_गिनती(arg_गिनती), attr(std::move(attr)) {}

        गाँठ_2(पूर्णांक fn_सूचकांक, Attr स्थिर &attr, पूर्णांक arg_offset, पूर्णांक arg_गिनती = 0)
            : fn_सूचकांक(fn_सूचकांक), arg_offset(arg_offset), arg_गिनती(arg_गिनती), attr(attr) {}
    };


    std::vector<गाँठ_2> nodes;
    std::vector<पूर्णांक> args;
    dict<कुंजी_2, पूर्णांक> कुंजियाँ;
    dict<पूर्णांक, SparseAttr> sparse_attrs;

खुला:
    ढाँचा<typename Graph>
    काष्ठा BaseRef
    {
    रक्षित:
        मित्र काष्ठा ComputeGraph;
        Graph *graph_;
        पूर्णांक सूचकांक_2;
        BaseRef(Graph *graph, पूर्णांक सूचकांक) : graph_(graph), सूचकांक_2(सूचकांक) {
            log_assert(सूचकांक_2 >= 0);
            check();
        }

        व्योम check() स्थिर { log_assert(सूचकांक_2 < graph_->आकार()); }

        गाँठ_2 स्थिर &deref() स्थिर { check(); वापस graph_->nodes[सूचकांक_2]; }

    खुला:
        ComputeGraph स्थिर &graph() स्थिर { वापस graph_; }
        पूर्णांक सूचकांक() स्थिर { वापस सूचकांक_2; }

        पूर्णांक आकार() स्थिर { वापस deref().arg_गिनती; }

        BaseRef arg(पूर्णांक n) स्थिर
        {
            गाँठ_2 स्थिर &गाँठ = deref();
            log_assert(n >= 0 && n < गाँठ.arg_गिनती);
            वापस BaseRef(graph_, graph_->args[गाँठ.arg_offset + n]);
        }

        std::vector<पूर्णांक>::const_iterator arg_indices_cbegin() स्थिर
        {
            गाँठ_2 स्थिर &गाँठ = deref();
            वापस graph_->args.cbegin() + गाँठ.arg_offset;
        }

        std::vector<पूर्णांक>::const_iterator arg_indices_cend() स्थिर
        {
            गाँठ_2 स्थिर &गाँठ = deref();
            वापस graph_->args.cbegin() + गाँठ.arg_offset + गाँठ.arg_गिनती;
        }

        Fn स्थिर &function() स्थिर { वापस graph_->functions[deref().fn_सूचकांक]; }
        Attr स्थिर &attr() स्थिर { वापस deref().attr; }

        bool has_sparse_attr() स्थिर { वापस graph_->sparse_attrs.गिनती(सूचकांक_2); }

        SparseAttr स्थिर &sparse_attr() स्थिर
        {
            स्वतः found = graph_->sparse_attrs.find(सूचकांक_2);
            log_assert(found != graph_->sparse_attrs.end());
            वापस found->second;
        }
    };

    using ConstRef = BaseRef<ComputeGraph स्थिर>;

    काष्ठा Ref : खुला BaseRef<ComputeGraph>
    {
    निजी:
        मित्र काष्ठा ComputeGraph;
        Ref(ComputeGraph *graph, पूर्णांक सूचकांक) : BaseRef<ComputeGraph>(graph, सूचकांक) {}
        गाँठ_2 &deref() स्थिर { this->check(); वापस this->graph_->nodes[this->सूचकांक_2]; }

    खुला:
        Ref(BaseRef<ComputeGraph> ref) : Ref(ref.graph_, ref.सूचकांक_2) {}

        व्योम set_function(Fn स्थिर &function) स्थिर
        {
            deref().fn_सूचकांक = this->graph_->functions(function);
        }

        Attr &attr() स्थिर { वापस deref().attr; }

        व्योम append_arg(ConstRef arg) स्थिर
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.सूचकांक());
        }

        व्योम append_arg(पूर्णांक arg) स्थिर
        {
            log_assert(arg >= 0 && arg < this->graph_->आकार());
            गाँठ_2 &गाँठ = deref();
            अगर (गाँठ.arg_offset + गाँठ.arg_गिनती != Get_आकार(this->graph_->args))
                move_args(गाँठ);
            this->graph_->args.धकेलो_back(arg);
            गाँठ.arg_गिनती++;
        }

        चालक ConstRef() स्थिर
        {
            वापस ConstRef(this->graph_, this->सूचकांक_2);
        }

        SparseAttr &sparse_attr() स्थिर
        {
            वापस this->graph_->sparse_attrs[this->सूचकांक_2];
        }

        व्योम clear_sparse_attr() स्थिर
        {
            this->graph_->sparse_attrs.erase(this->सूचकांक_2);
        }

        व्योम assign_कुंजी(कुंजी_2 स्थिर &कुंजी) स्थिर
        {
            this->graph_->कुंजियाँ.emplace(कुंजी, this->सूचकांक_2);
        }

    निजी:
        व्योम move_args(गाँठ_2 &गाँठ) स्थिर
        {
            स्वतः &args = this->graph_->args;
            पूर्णांक old_offset = गाँठ.arg_offset;
            गाँठ.arg_offset = Get_आकार(args);
            क्रम (पूर्णांक i = 0; i != गाँठ.arg_गिनती; ++i)
                args.धकेलो_back(args[old_offset + i]);
        }

    };

    bool has_कुंजी(कुंजी_2 स्थिर &कुंजी) स्थिर
    {
        वापस कुंजियाँ.गिनती(कुंजी);
    }

    dict<कुंजी_2, पूर्णांक> स्थिर &कुंजियाँ_2() स्थिर
    {
        वापस कुंजियाँ;
    }

    ConstRef चालक()(कुंजी_2 स्थिर &कुंजी) स्थिर
    {
        स्वतः it = कुंजियाँ.find(कुंजी);
        log_assert(it != कुंजियाँ.end());
        वापस (*this)[it->second];
    }

    Ref चालक()(कुंजी_2 स्थिर &कुंजी)
    {
        स्वतः it = कुंजियाँ.find(कुंजी);
        log_assert(it != कुंजियाँ.end());
        वापस (*this)[it->second];
    }

    पूर्णांक आकार() स्थिर { वापस Get_आकार(nodes); }

    ConstRef चालक[](पूर्णांक सूचकांक) स्थिर { वापस ConstRef(this, सूचकांक); }
    Ref चालक[](पूर्णांक सूचकांक) { वापस Ref(this, सूचकांक); }

    Ref add(Fn स्थिर &function, Attr &&attr)
    {
        पूर्णांक सूचकांक = Get_आकार(nodes);
        पूर्णांक fn_सूचकांक = functions(function);
        nodes.emplace_back(fn_सूचकांक, std::move(attr), Get_आकार(args));
        वापस Ref(this, सूचकांक);
    }

    Ref add(Fn स्थिर &function, Attr स्थिर &attr)
    {
        पूर्णांक सूचकांक = Get_आकार(nodes);
        पूर्णांक fn_सूचकांक = functions(function);
        nodes.emplace_back(fn_सूचकांक, attr,  Get_आकार(args));
        वापस Ref(this, सूचकांक);
    }

    ढाँचा<typename T>
    Ref add(Fn स्थिर &function, Attr स्थिर &attr, T &&args)
    {
        Ref added = add(function, attr);
        क्रम (स्वतः arg : args)
            added.append_arg(arg);
        वापस added;
    }

    ढाँचा<typename T>
    Ref add(Fn स्थिर &function, Attr &&attr, T &&args)
    {
        Ref added = add(function, std::move(attr));
        क्रम (स्वतः arg : args)
            added.append_arg(arg);
        वापस added;
    }

    Ref add(Fn स्थिर &function, Attr स्थिर &attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, attr);
        क्रम (स्वतः arg : args)
            added.append_arg(arg);
        वापस added;
    }

    Ref add(Fn स्थिर &function, Attr &&attr, std::initializer_list<Ref> args)
    {
        Ref added = add(function, std::move(attr));
        क्रम (स्वतः arg : args)
            added.append_arg(arg);
        वापस added;
    }

    ढाँचा<typename T>
    Ref add(Fn स्थिर &function, Attr स्थिर &attr, T begin, T end)
    {
        Ref added = add(function, attr);
        क्रम (; begin != end; ++begin)
            added.append_arg(*begin);
        वापस added;
    }

    व्योम compact_args()
    {
        std::vector<पूर्णांक> new_args;
        क्रम (स्वतः &गाँठ : nodes)
        {
            पूर्णांक new_offset = Get_आकार(new_args);
            क्रम (पूर्णांक i = 0; i < गाँठ.arg_गिनती; i++)
                new_args.धकेलो_back(args[गाँठ.arg_offset + i]);
            गाँठ.arg_offset = new_offset;
        }
        std::swap(args, new_args);
    }

    व्योम permute(std::vector<पूर्णांक> स्थिर &perm)
    {
        log_assert(perm.आकार() <= nodes.आकार());
        std::vector<पूर्णांक> inv_perm;
        inv_perm.resize(nodes.आकार(), -1);
        क्रम (पूर्णांक i = 0; i < Get_आकार(perm); ++i)
        {
            पूर्णांक j = perm[i];
            log_assert(j >= 0 && j < Get_आकार(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    व्योम permute(std::vector<पूर्णांक> स्थिर &perm, std::vector<पूर्णांक> स्थिर &inv_perm)
    {
        log_assert(inv_perm.आकार() == nodes.आकार());
        std::vector<गाँठ_2> new_nodes;
        new_nodes.reserve(perm.आकार());
        dict<पूर्णांक, SparseAttr> new_sparse_attrs;
        क्रम (पूर्णांक i : perm)
        {
            पूर्णांक j = Get_आकार(new_nodes);
            new_nodes.emplace_back(std::move(nodes[i]));
            स्वतः found = sparse_attrs.find(i);
            अगर (found != sparse_attrs.end())
                new_sparse_attrs.emplace(j, std::move(found->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_args();
        क्रम (पूर्णांक &arg : args)
        {
            log_assert(arg < Get_आकार(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        क्रम (स्वतः &कुंजी : कुंजियाँ)
        {
            log_assert(कुंजी.second < Get_आकार(inv_perm));
            log_assert(inv_perm[कुंजी.second] >= 0);
            कुंजी.second = inv_perm[कुंजी.second];
        }
    }

    काष्ठा SccAdaptor
    {
    निजी:
        ComputeGraph स्थिर &graph_;
        std::vector<पूर्णांक> indices_;
    खुला:
        SccAdaptor(ComputeGraph स्थिर &graph) : graph_(graph)
        {
            indices_.resize(graph.आकार(), -1);
        }


        प्रकार पूर्णांक गाँठ_type;

        काष्ठा गाँठ_enumerator {
        निजी:
            मित्र काष्ठा SccAdaptor;
            पूर्णांक current, end;
            गाँठ_enumerator(पूर्णांक current, पूर्णांक end) : current(current), end(end) {}

        खुला:

            bool finished() स्थिर { वापस current == end; }
            गाँठ_type अग्र() {
                log_assert(!finished());
                गाँठ_type परिणाम = current;
                ++current;
                वापस परिणाम;
            }
        };

        गाँठ_enumerator enumerate_nodes() {
            वापस गाँठ_enumerator(0, Get_आकार(indices_));
        }


        काष्ठा successor_enumerator {
        निजी:
            मित्र काष्ठा SccAdaptor;
            std::vector<पूर्णांक>::const_iterator current, end;
            successor_enumerator(std::vector<पूर्णांक>::const_iterator current, std::vector<पूर्णांक>::const_iterator end) :
                current(current), end(end) {}

        खुला:
            bool finished() स्थिर { वापस current == end; }
            गाँठ_type अग्र() {
                log_assert(!finished());
                गाँठ_type परिणाम = *current;
                ++current;
                वापस परिणाम;
            }
        };

        successor_enumerator enumerate_successors(पूर्णांक सूचकांक) स्थिर {
            स्वतः स्थिर &ref = graph_[सूचकांक];
            वापस successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        पूर्णांक &dfs_सूचकांक(गाँठ_type स्थिर &गाँठ) { वापस indices_[गाँठ]; }

        std::vector<पूर्णांक> स्थिर &dfs_indices() { वापस indices_; }
    };

};



YOSYS_NAMESPACE_END


#पूर्ण_अगर
