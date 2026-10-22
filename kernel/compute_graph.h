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

#यदि_अपरिभाषितम् COMPUTE_GRAPH_H
#परिभाषय COMPUTE_GRAPH_H

#अन्तर्भाव <tuple>
#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

प्रतिरूपम्<
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
        पूर्णाङ्क fn_सूचकांक;
        पूर्णाङ्क arg_विस्थापनम्;
        पूर्णाङ्क arg_गणनम्;
        Attr attr;

        ग्रन्थिः_2(पूर्णाङ्क fn_सूचकांक, Attr &&attr, पूर्णाङ्क arg_विस्थापनम्, पूर्णाङ्क arg_गणनम् = 0)
            : fn_सूचकांक(fn_सूचकांक), arg_विस्थापनम्(arg_विस्थापनम्), arg_गणनम्(arg_गणनम्), attr(std::move(attr)) {}

        ग्रन्थिः_2(पूर्णाङ्क fn_सूचकांक, Attr ध्रुव &attr, पूर्णाङ्क arg_विस्थापनम्, पूर्णाङ्क arg_गणनम् = 0)
            : fn_सूचकांक(fn_सूचकांक), arg_विस्थापनम्(arg_विस्थापनम्), arg_गणनम्(arg_गणनम्), attr(attr) {}
    };


    std::सदिशम्<ग्रन्थिः_2> nodes;
    std::सदिशम्<पूर्णाङ्क> तर्काः_2;
    dict<कुञ्चिका_2, पूर्णाङ्क> कुञ्चिकाः;
    dict<पूर्णाङ्क, SparseAttr> sparse_attrs;

सार्वजनिक:
    प्रतिरूपम्<typename Graph>
    संरचना आधारः_Ref
    {
    रक्षित:
        मित्रम् संरचना ComputeGraph;
        Graph *graph_;
        पूर्णाङ्क सूचकांक_2;
        आधारः_Ref(Graph *graph, पूर्णाङ्क सूचकांक) : graph_(graph), सूचकांक_2(सूचकांक) {
            log_assert(सूचकांक_2 >= 0);
            check();
        }

        शून्य check() ध्रुव { log_assert(सूचकांक_2 < graph_->आकार()); }

        ग्रन्थिः_2 ध्रुव &deref() ध्रुव { check(); प्रत्यागम graph_->nodes[सूचकांक_2]; }

    सार्वजनिक:
        ComputeGraph ध्रुव &graph() ध्रुव { प्रत्यागम graph_; }
        पूर्णाङ्क सूचकांक() ध्रुव { प्रत्यागम सूचकांक_2; }

        पूर्णाङ्क आकार() ध्रुव { प्रत्यागम deref().arg_गणनम्; }

        आधारः_Ref arg(पूर्णाङ्क n) ध्रुव
        {
            ग्रन्थिः_2 ध्रुव &ग्रन्थिः = deref();
            log_assert(n >= 0 && n < ग्रन्थिः.arg_गणनम्);
            प्रत्यागम आधारः_Ref(graph_, graph_->तर्काः_2[ग्रन्थिः.arg_विस्थापनम् + n]);
        }

        std::सदिशम्<पूर्णाङ्क>::const_iterator arg_indices_cbegin() ध्रुव
        {
            ग्रन्थिः_2 ध्रुव &ग्रन्थिः = deref();
            प्रत्यागम graph_->तर्काः_2.cbegin() + ग्रन्थिः.arg_विस्थापनम्;
        }

        std::सदिशम्<पूर्णाङ्क>::const_iterator arg_indices_cend() ध्रुव
        {
            ग्रन्थिः_2 ध्रुव &ग्रन्थिः = deref();
            प्रत्यागम graph_->तर्काः_2.cbegin() + ग्रन्थिः.arg_विस्थापनम् + ग्रन्थिः.arg_गणनम्;
        }

        Fn ध्रुव &function() ध्रुव { प्रत्यागम graph_->functions[deref().fn_सूचकांक]; }
        Attr ध्रुव &attr() ध्रुव { प्रत्यागम deref().attr; }

        bool has_sparse_attr() ध्रुव { प्रत्यागम graph_->sparse_attrs.गणनम्(सूचकांक_2); }

        SparseAttr ध्रुव &sparse_attr() ध्रुव
        {
            स्वचालित प्राप्तम् = graph_->sparse_attrs.find(सूचकांक_2);
            log_assert(प्राप्तम् != graph_->sparse_attrs.अन्तः());
            प्रत्यागम प्राप्तम्->second;
        }
    };

    using ConstRef = आधारः_Ref<ComputeGraph ध्रुव>;

    संरचना Ref : सार्वजनिक आधारः_Ref<ComputeGraph>
    {
    गुप्त:
        मित्रम् संरचना ComputeGraph;
        Ref(ComputeGraph *graph, पूर्णाङ्क सूचकांक) : आधारः_Ref<ComputeGraph>(graph, सूचकांक) {}
        ग्रन्थिः_2 &deref() ध्रुव { this->check(); प्रत्यागम this->graph_->nodes[this->सूचकांक_2]; }

    सार्वजनिक:
        Ref(आधारः_Ref<ComputeGraph> ref) : Ref(ref.graph_, ref.सूचकांक_2) {}

        शून्य समुच्चयः_function(Fn ध्रुव &function) ध्रुव
        {
            deref().fn_सूचकांक = this->graph_->functions(function);
        }

        Attr &attr() ध्रुव { प्रत्यागम deref().attr; }

        शून्य append_arg(ConstRef arg) ध्रुव
        {
            log_assert(arg.graph_ == this->graph_);
            append_arg(arg.सूचकांक());
        }

        शून्य append_arg(पूर्णाङ्क arg) ध्रुव
        {
            log_assert(arg >= 0 && arg < this->graph_->आकार());
            ग्रन्थिः_2 &ग्रन्थिः = deref();
            यदि (ग्रन्थिः.arg_विस्थापनम् + ग्रन्थिः.arg_गणनम् != Get_आकार(this->graph_->तर्काः_2))
                move_तर्काः(ग्रन्थिः);
            this->graph_->तर्काः_2.प्रक्षिप_पृष्ठभागः(arg);
            ग्रन्थिः.arg_गणनम्++;
        }

        संक्रियकः ConstRef() ध्रुव
        {
            प्रत्यागम ConstRef(this->graph_, this->सूचकांक_2);
        }

        SparseAttr &sparse_attr() ध्रुव
        {
            प्रत्यागम this->graph_->sparse_attrs[this->सूचकांक_2];
        }

        शून्य clear_sparse_attr() ध्रुव
        {
            this->graph_->sparse_attrs.erase(this->सूचकांक_2);
        }

        शून्य नियोजय_कुञ्चिका(कुञ्चिका_2 ध्रुव &कुञ्चिका) ध्रुव
        {
            this->graph_->कुञ्चिकाः.emplace(कुञ्चिका, this->सूचकांक_2);
        }

    गुप्त:
        शून्य move_तर्काः(ग्रन्थिः_2 &ग्रन्थिः) ध्रुव
        {
            स्वचालित &तर्काः_2 = this->graph_->तर्काः_2;
            पूर्णाङ्क old_विस्थापनम् = ग्रन्थिः.arg_विस्थापनम्;
            ग्रन्थिः.arg_विस्थापनम् = Get_आकार(तर्काः_2);
            प्रति (पूर्णाङ्क i = 0; i != ग्रन्थिः.arg_गणनम्; ++i)
                तर्काः_2.प्रक्षिप_पृष्ठभागः(तर्काः_2[old_विस्थापनम् + i]);
        }

    };

    bool has_कुञ्चिका(कुञ्चिका_2 ध्रुव &कुञ्चिका) ध्रुव
    {
        प्रत्यागम कुञ्चिकाः.गणनम्(कुञ्चिका);
    }

    dict<कुञ्चिका_2, पूर्णाङ्क> ध्रुव &कुञ्चिकाः_2() ध्रुव
    {
        प्रत्यागम कुञ्चिकाः;
    }

    ConstRef संक्रियकः()(कुञ्चिका_2 ध्रुव &कुञ्चिका) ध्रुव
    {
        स्वचालित it = कुञ्चिकाः.find(कुञ्चिका);
        log_assert(it != कुञ्चिकाः.अन्तः());
        प्रत्यागम (*this)[it->second];
    }

    Ref संक्रियकः()(कुञ्चिका_2 ध्रुव &कुञ्चिका)
    {
        स्वचालित it = कुञ्चिकाः.find(कुञ्चिका);
        log_assert(it != कुञ्चिकाः.अन्तः());
        प्रत्यागम (*this)[it->second];
    }

    पूर्णाङ्क आकार() ध्रुव { प्रत्यागम Get_आकार(nodes); }

    ConstRef संक्रियकः[](पूर्णाङ्क सूचकांक) ध्रुव { प्रत्यागम ConstRef(this, सूचकांक); }
    Ref संक्रियकः[](पूर्णाङ्क सूचकांक) { प्रत्यागम Ref(this, सूचकांक); }

    Ref सङ्कलय(Fn ध्रुव &function, Attr &&attr)
    {
        पूर्णाङ्क सूचकांक = Get_आकार(nodes);
        पूर्णाङ्क fn_सूचकांक = functions(function);
        nodes.emplace_पृष्ठभागः(fn_सूचकांक, std::move(attr), Get_आकार(तर्काः_2));
        प्रत्यागम Ref(this, सूचकांक);
    }

    Ref सङ्कलय(Fn ध्रुव &function, Attr ध्रुव &attr)
    {
        पूर्णाङ्क सूचकांक = Get_आकार(nodes);
        पूर्णाङ्क fn_सूचकांक = functions(function);
        nodes.emplace_पृष्ठभागः(fn_सूचकांक, attr,  Get_आकार(तर्काः_2));
        प्रत्यागम Ref(this, सूचकांक);
    }

    प्रतिरूपम्<typename T>
    Ref सङ्कलय(Fn ध्रुव &function, Attr ध्रुव &attr, T &&तर्काः_2)
    {
        Ref added = सङ्कलय(function, attr);
        प्रति (स्वचालित arg : तर्काः_2)
            added.append_arg(arg);
        प्रत्यागम added;
    }

    प्रतिरूपम्<typename T>
    Ref सङ्कलय(Fn ध्रुव &function, Attr &&attr, T &&तर्काः_2)
    {
        Ref added = सङ्कलय(function, std::move(attr));
        प्रति (स्वचालित arg : तर्काः_2)
            added.append_arg(arg);
        प्रत्यागम added;
    }

    Ref सङ्कलय(Fn ध्रुव &function, Attr ध्रुव &attr, std::initializer_list<Ref> तर्काः_2)
    {
        Ref added = सङ्कलय(function, attr);
        प्रति (स्वचालित arg : तर्काः_2)
            added.append_arg(arg);
        प्रत्यागम added;
    }

    Ref सङ्कलय(Fn ध्रुव &function, Attr &&attr, std::initializer_list<Ref> तर्काः_2)
    {
        Ref added = सङ्कलय(function, std::move(attr));
        प्रति (स्वचालित arg : तर्काः_2)
            added.append_arg(arg);
        प्रत्यागम added;
    }

    प्रतिरूपम्<typename T>
    Ref सङ्कलय(Fn ध्रुव &function, Attr ध्रुव &attr, T begin, T अन्तः)
    {
        Ref added = सङ्कलय(function, attr);
        प्रति (; begin != अन्तः; ++begin)
            added.append_arg(*begin);
        प्रत्यागम added;
    }

    शून्य compact_तर्काः()
    {
        std::सदिशम्<पूर्णाङ्क> new_तर्काः;
        प्रति (स्वचालित &ग्रन्थिः : nodes)
        {
            पूर्णाङ्क new_विस्थापनम् = Get_आकार(new_तर्काः);
            प्रति (पूर्णाङ्क i = 0; i < ग्रन्थिः.arg_गणनम्; i++)
                new_तर्काः.प्रक्षिप_पृष्ठभागः(तर्काः_2[ग्रन्थिः.arg_विस्थापनम् + i]);
            ग्रन्थिः.arg_विस्थापनम् = new_विस्थापनम्;
        }
        std::swap(तर्काः_2, new_तर्काः);
    }

    शून्य permute(std::सदिशम्<पूर्णाङ्क> ध्रुव &perm)
    {
        log_assert(perm.आकार() <= nodes.आकार());
        std::सदिशम्<पूर्णाङ्क> inv_perm;
        inv_perm.resize(nodes.आकार(), -1);
        प्रति (पूर्णाङ्क i = 0; i < Get_आकार(perm); ++i)
        {
            पूर्णाङ्क j = perm[i];
            log_assert(j >= 0 && j < Get_आकार(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        permute(perm, inv_perm);
    }

    शून्य permute(std::सदिशम्<पूर्णाङ्क> ध्रुव &perm, std::सदिशम्<पूर्णाङ्क> ध्रुव &inv_perm)
    {
        log_assert(inv_perm.आकार() == nodes.आकार());
        std::सदिशम्<ग्रन्थिः_2> new_nodes;
        new_nodes.reserve(perm.आकार());
        dict<पूर्णाङ्क, SparseAttr> new_sparse_attrs;
        प्रति (पूर्णाङ्क i : perm)
        {
            पूर्णाङ्क j = Get_आकार(new_nodes);
            new_nodes.emplace_पृष्ठभागः(std::move(nodes[i]));
            स्वचालित प्राप्तम् = sparse_attrs.find(i);
            यदि (प्राप्तम् != sparse_attrs.अन्तः())
                new_sparse_attrs.emplace(j, std::move(प्राप्तम्->second));
        }

        std::swap(nodes, new_nodes);
        std::swap(sparse_attrs, new_sparse_attrs);

        compact_तर्काः();
        प्रति (पूर्णाङ्क &arg : तर्काः_2)
        {
            log_assert(arg < Get_आकार(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        प्रति (स्वचालित &कुञ्चिका : कुञ्चिकाः)
        {
            log_assert(कुञ्चिका.second < Get_आकार(inv_perm));
            log_assert(inv_perm[कुञ्चिका.second] >= 0);
            कुञ्चिका.second = inv_perm[कुञ्चिका.second];
        }
    }

    संरचना SccAdaptor
    {
    गुप्त:
        ComputeGraph ध्रुव &graph_;
        std::सदिशम्<पूर्णाङ्क> indices_;
    सार्वजनिक:
        SccAdaptor(ComputeGraph ध्रुव &graph) : graph_(graph)
        {
            indices_.resize(graph.आकार(), -1);
        }


        प्रकारनिर्देश पूर्णाङ्क ग्रन्थिः_type;

        संरचना ग्रन्थिः_enumerator {
        गुप्त:
            मित्रम् संरचना SccAdaptor;
            पूर्णाङ्क current, अन्तः;
            ग्रन्थिः_enumerator(पूर्णाङ्क current, पूर्णाङ्क अन्तः) : current(current), अन्तः(अन्तः) {}

        सार्वजनिक:

            bool परिसमाप्तम्() ध्रुव { प्रत्यागम current == अन्तः; }
            ग्रन्थिः_type अग्र() {
                log_assert(!परिसमाप्तम्());
                ग्रन्थिः_type परिणाम = current;
                ++current;
                प्रत्यागम परिणाम;
            }
        };

        ग्रन्थिः_enumerator enumerate_nodes() {
            प्रत्यागम ग्रन्थिः_enumerator(0, Get_आकार(indices_));
        }


        संरचना successor_enumerator {
        गुप्त:
            मित्रम् संरचना SccAdaptor;
            std::सदिशम्<पूर्णाङ्क>::const_iterator current, अन्तः;
            successor_enumerator(std::सदिशम्<पूर्णाङ्क>::const_iterator current, std::सदिशम्<पूर्णाङ्क>::const_iterator अन्तः) :
                current(current), अन्तः(अन्तः) {}

        सार्वजनिक:
            bool परिसमाप्तम्() ध्रुव { प्रत्यागम current == अन्तः; }
            ग्रन्थिः_type अग्र() {
                log_assert(!परिसमाप्तम्());
                ग्रन्थिः_type परिणाम = *current;
                ++current;
                प्रत्यागम परिणाम;
            }
        };

        successor_enumerator enumerate_उत्तराधिकारिणः(पूर्णाङ्क सूचकांक) ध्रुव {
            स्वचालित ध्रुव &ref = graph_[सूचकांक];
            प्रत्यागम successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        पूर्णाङ्क &dfs_सूचकांक(ग्रन्थिः_type ध्रुव &ग्रन्थिः) { प्रत्यागम indices_[ग्रन्थिः]; }

        std::सदिशम्<पूर्णाङ्क> ध्रुव &dfs_indices() { प्रत्यागम indices_; }
    };

};



YOSYS_NAMESPACE_अन्तः


#यदि_अन्तः
