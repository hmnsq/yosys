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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ COMPUTE_ਗ੍ਰਾਫ_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COMPUTE_ਗ੍ਰਾਫ_H

#ਅਨ੍ਤਰ੍ਭਾਵ <tuple>
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ਸਾਂਚਾ<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename ਕੁੰਜੀ_2 = std::tuple<> // Stable keys to refer to nodes
>
ਬਣਤਰ Compute_ਗ੍ਰਾਫ
{
    ਬਣਤਰ Ref;
ਨਿਜੀ:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    ਬਣਤਰ ਗੰਢ_2 {
        ਪੂਰਨ fn_ਸੂਚਕਾਂਕ;
        ਪੂਰਨ arg_ਵਿੱਥ;
        ਪੂਰਨ arg_ਗਿਣਤੀ;
        Attr attr;

        ਗੰਢ_2(ਪੂਰਨ fn_ਸੂਚਕਾਂਕ, Attr &&attr, ਪੂਰਨ arg_ਵਿੱਥ, ਪੂਰਨ arg_ਗਿਣਤੀ = 0)
            : fn_ਸੂਚਕਾਂਕ(fn_ਸੂਚਕਾਂਕ), arg_ਵਿੱਥ(arg_ਵਿੱਥ), arg_ਗਿਣਤੀ(arg_ਗਿਣਤੀ), attr(std::move(attr)) {}

        ਗੰਢ_2(ਪੂਰਨ fn_ਸੂਚਕਾਂਕ, Attr ਅਚਲ &attr, ਪੂਰਨ arg_ਵਿੱਥ, ਪੂਰਨ arg_ਗਿਣਤੀ = 0)
            : fn_ਸੂਚਕਾਂਕ(fn_ਸੂਚਕਾਂਕ), arg_ਵਿੱਥ(arg_ਵਿੱਥ), arg_ਗਿਣਤੀ(arg_ਗਿਣਤੀ), attr(attr) {}
    };


    std::ਸਦਿਸ਼<ਗੰਢ_2> nodes;
    std::ਸਦਿਸ਼<ਪੂਰਨ> ਦਲੀਲਾਂ_2;
    dict<ਕੁੰਜੀ_2, ਪੂਰਨ> ਕੁੰਜੀਆਂ;
    dict<ਪੂਰਨ, SparseAttr> sparse_attrs;

ਜਨਤਕ:
    ਸਾਂਚਾ<typename ਗ੍ਰਾਫ>
    ਬਣਤਰ ਅਧਾਰ_Ref
    {
    ਸੁਰੱਖਿਅਤ:
        ਮਿੱਤਰ ਬਣਤਰ Compute_ਗ੍ਰਾਫ;
        ਗ੍ਰਾਫ *ਗ੍ਰਾਫ_2;
        ਪੂਰਨ ਸੂਚਕਾਂਕ_2;
        ਅਧਾਰ_Ref(ਗ੍ਰਾਫ *ਗ੍ਰਾਫ_3, ਪੂਰਨ ਸੂਚਕਾਂਕ) : ਗ੍ਰਾਫ_2(ਗ੍ਰਾਫ_3), ਸੂਚਕਾਂਕ_2(ਸੂਚਕਾਂਕ) {
            log_assert(ਸੂਚਕਾਂਕ_2 >= 0);
            check();
        }

        ਖਾਲੀ check() ਅਚਲ { log_assert(ਸੂਚਕਾਂਕ_2 < ਗ੍ਰਾਫ_2->ਮਾਪ()); }

        ਗੰਢ_2 ਅਚਲ &deref() ਅਚਲ { check(); ਵਾਪਸ ਗ੍ਰਾਫ_2->nodes[ਸੂਚਕਾਂਕ_2]; }

    ਜਨਤਕ:
        Compute_ਗ੍ਰਾਫ ਅਚਲ &ਗ੍ਰਾਫ_3() ਅਚਲ { ਵਾਪਸ ਗ੍ਰਾਫ_2; }
        ਪੂਰਨ ਸੂਚਕਾਂਕ() ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ_2; }

        ਪੂਰਨ ਮਾਪ() ਅਚਲ { ਵਾਪਸ deref().arg_ਗਿਣਤੀ; }

        ਅਧਾਰ_Ref arg(ਪੂਰਨ n) ਅਚਲ
        {
            ਗੰਢ_2 ਅਚਲ &ਗੰਢ = deref();
            log_assert(n >= 0 && n < ਗੰਢ.arg_ਗਿਣਤੀ);
            ਵਾਪਸ ਅਧਾਰ_Ref(ਗ੍ਰਾਫ_2, ਗ੍ਰਾਫ_2->ਦਲੀਲਾਂ_2[ਗੰਢ.arg_ਵਿੱਥ + n]);
        }

        std::ਸਦਿਸ਼<ਪੂਰਨ>::const_iterator arg_indices_cbegin() ਅਚਲ
        {
            ਗੰਢ_2 ਅਚਲ &ਗੰਢ = deref();
            ਵਾਪਸ ਗ੍ਰਾਫ_2->ਦਲੀਲਾਂ_2.cbegin() + ਗੰਢ.arg_ਵਿੱਥ;
        }

        std::ਸਦਿਸ਼<ਪੂਰਨ>::const_iterator arg_indices_cend() ਅਚਲ
        {
            ਗੰਢ_2 ਅਚਲ &ਗੰਢ = deref();
            ਵਾਪਸ ਗ੍ਰਾਫ_2->ਦਲੀਲਾਂ_2.cbegin() + ਗੰਢ.arg_ਵਿੱਥ + ਗੰਢ.arg_ਗਿਣਤੀ;
        }

        Fn ਅਚਲ &function() ਅਚਲ { ਵਾਪਸ ਗ੍ਰਾਫ_2->functions[deref().fn_ਸੂਚਕਾਂਕ]; }
        Attr ਅਚਲ &attr() ਅਚਲ { ਵਾਪਸ deref().attr; }

        bool has_sparse_attr() ਅਚਲ { ਵਾਪਸ ਗ੍ਰਾਫ_2->sparse_attrs.ਗਿਣਤੀ_2(ਸੂਚਕਾਂਕ_2); }

        SparseAttr ਅਚਲ &sparse_attr() ਅਚਲ
        {
            ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ = ਗ੍ਰਾਫ_2->sparse_attrs.ਲੱਭੋ(ਸੂਚਕਾਂਕ_2);
            log_assert(ਮਿਲਿਆ != ਗ੍ਰਾਫ_2->sparse_attrs.ਸਿਰਾ());
            ਵਾਪਸ ਮਿਲਿਆ->ਦੂਜਾ;
        }
    };

    using ConstRef = ਅਧਾਰ_Ref<Compute_ਗ੍ਰਾਫ ਅਚਲ>;

    ਬਣਤਰ Ref : ਜਨਤਕ ਅਧਾਰ_Ref<Compute_ਗ੍ਰਾਫ>
    {
    ਨਿਜੀ:
        ਮਿੱਤਰ ਬਣਤਰ Compute_ਗ੍ਰਾਫ;
        Ref(Compute_ਗ੍ਰਾਫ *ਗ੍ਰਾਫ_3, ਪੂਰਨ ਸੂਚਕਾਂਕ) : ਅਧਾਰ_Ref<Compute_ਗ੍ਰਾਫ>(ਗ੍ਰਾਫ_3, ਸੂਚਕਾਂਕ) {}
        ਗੰਢ_2 &deref() ਅਚਲ { this->check(); ਵਾਪਸ this->ਗ੍ਰਾਫ_2->nodes[this->ਸੂਚਕਾਂਕ_2]; }

    ਜਨਤਕ:
        Ref(ਅਧਾਰ_Ref<Compute_ਗ੍ਰਾਫ> ref) : Ref(ref.ਗ੍ਰਾਫ_2, ref.ਸੂਚਕਾਂਕ_2) {}

        ਖਾਲੀ ਸੈੱਟ_function(Fn ਅਚਲ &function) ਅਚਲ
        {
            deref().fn_ਸੂਚਕਾਂਕ = this->ਗ੍ਰਾਫ_2->functions(function);
        }

        Attr &attr() ਅਚਲ { ਵਾਪਸ deref().attr; }

        ਖਾਲੀ append_arg(ConstRef arg) ਅਚਲ
        {
            log_assert(arg.ਗ੍ਰਾਫ_2 == this->ਗ੍ਰਾਫ_2);
            append_arg(arg.ਸੂਚਕਾਂਕ());
        }

        ਖਾਲੀ append_arg(ਪੂਰਨ arg) ਅਚਲ
        {
            log_assert(arg >= 0 && arg < this->ਗ੍ਰਾਫ_2->ਮਾਪ());
            ਗੰਢ_2 &ਗੰਢ = deref();
            ਜੇ (ਗੰਢ.arg_ਵਿੱਥ + ਗੰਢ.arg_ਗਿਣਤੀ != Get_ਮਾਪ(this->ਗ੍ਰਾਫ_2->ਦਲੀਲਾਂ_2))
                move_ਦਲੀਲਾਂ(ਗੰਢ);
            this->ਗ੍ਰਾਫ_2->ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(arg);
            ਗੰਢ.arg_ਗਿਣਤੀ++;
        }

        ਸੰਚਾਲਕ ConstRef() ਅਚਲ
        {
            ਵਾਪਸ ConstRef(this->ਗ੍ਰਾਫ_2, this->ਸੂਚਕਾਂਕ_2);
        }

        SparseAttr &sparse_attr() ਅਚਲ
        {
            ਵਾਪਸ this->ਗ੍ਰਾਫ_2->sparse_attrs[this->ਸੂਚਕਾਂਕ_2];
        }

        ਖਾਲੀ clear_sparse_attr() ਅਚਲ
        {
            this->ਗ੍ਰਾਫ_2->sparse_attrs.erase(this->ਸੂਚਕਾਂਕ_2);
        }

        ਖਾਲੀ ਸੌਂਪੋ_ਕੁੰਜੀ(ਕੁੰਜੀ_2 ਅਚਲ &ਕੁੰਜੀ) ਅਚਲ
        {
            this->ਗ੍ਰਾਫ_2->ਕੁੰਜੀਆਂ.emplace(ਕੁੰਜੀ, this->ਸੂਚਕਾਂਕ_2);
        }

    ਨਿਜੀ:
        ਖਾਲੀ move_ਦਲੀਲਾਂ(ਗੰਢ_2 &ਗੰਢ) ਅਚਲ
        {
            ਸਵੈਚਾਲਿਤ &ਦਲੀਲਾਂ_2 = this->ਗ੍ਰਾਫ_2->ਦਲੀਲਾਂ_2;
            ਪੂਰਨ old_ਵਿੱਥ = ਗੰਢ.arg_ਵਿੱਥ;
            ਗੰਢ.arg_ਵਿੱਥ = Get_ਮਾਪ(ਦਲੀਲਾਂ_2);
            ਲਈ (ਪੂਰਨ i = 0; i != ਗੰਢ.arg_ਗਿਣਤੀ; ++i)
                ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(ਦਲੀਲਾਂ_2[old_ਵਿੱਥ + i]);
        }

    };

    bool has_ਕੁੰਜੀ(ਕੁੰਜੀ_2 ਅਚਲ &ਕੁੰਜੀ) ਅਚਲ
    {
        ਵਾਪਸ ਕੁੰਜੀਆਂ.ਗਿਣਤੀ_2(ਕੁੰਜੀ);
    }

    dict<ਕੁੰਜੀ_2, ਪੂਰਨ> ਅਚਲ &ਕੁੰਜੀਆਂ_2() ਅਚਲ
    {
        ਵਾਪਸ ਕੁੰਜੀਆਂ;
    }

    ConstRef ਸੰਚਾਲਕ()(ਕੁੰਜੀ_2 ਅਚਲ &ਕੁੰਜੀ) ਅਚਲ
    {
        ਸਵੈਚਾਲਿਤ it = ਕੁੰਜੀਆਂ.ਲੱਭੋ(ਕੁੰਜੀ);
        log_assert(it != ਕੁੰਜੀਆਂ.ਸਿਰਾ());
        ਵਾਪਸ (*this)[it->ਦੂਜਾ];
    }

    Ref ਸੰਚਾਲਕ()(ਕੁੰਜੀ_2 ਅਚਲ &ਕੁੰਜੀ)
    {
        ਸਵੈਚਾਲਿਤ it = ਕੁੰਜੀਆਂ.ਲੱਭੋ(ਕੁੰਜੀ);
        log_assert(it != ਕੁੰਜੀਆਂ.ਸਿਰਾ());
        ਵਾਪਸ (*this)[it->ਦੂਜਾ];
    }

    ਪੂਰਨ ਮਾਪ() ਅਚਲ { ਵਾਪਸ Get_ਮਾਪ(nodes); }

    ConstRef ਸੰਚਾਲਕ[](ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ { ਵਾਪਸ ConstRef(this, ਸੂਚਕਾਂਕ); }
    Ref ਸੰਚਾਲਕ[](ਪੂਰਨ ਸੂਚਕਾਂਕ) { ਵਾਪਸ Ref(this, ਸੂਚਕਾਂਕ); }

    Ref ਜੋੜੋ(Fn ਅਚਲ &function, Attr &&attr)
    {
        ਪੂਰਨ ਸੂਚਕਾਂਕ = Get_ਮਾਪ(nodes);
        ਪੂਰਨ fn_ਸੂਚਕਾਂਕ = functions(function);
        nodes.emplace_ਪਿੱਠ(fn_ਸੂਚਕਾਂਕ, std::move(attr), Get_ਮਾਪ(ਦਲੀਲਾਂ_2));
        ਵਾਪਸ Ref(this, ਸੂਚਕਾਂਕ);
    }

    Ref ਜੋੜੋ(Fn ਅਚਲ &function, Attr ਅਚਲ &attr)
    {
        ਪੂਰਨ ਸੂਚਕਾਂਕ = Get_ਮਾਪ(nodes);
        ਪੂਰਨ fn_ਸੂਚਕਾਂਕ = functions(function);
        nodes.emplace_ਪਿੱਠ(fn_ਸੂਚਕਾਂਕ, attr,  Get_ਮਾਪ(ਦਲੀਲਾਂ_2));
        ਵਾਪਸ Ref(this, ਸੂਚਕਾਂਕ);
    }

    ਸਾਂਚਾ<typename T>
    Ref ਜੋੜੋ(Fn ਅਚਲ &function, Attr ਅਚਲ &attr, T &&ਦਲੀਲਾਂ_2)
    {
        Ref added = ਜੋੜੋ(function, attr);
        ਲਈ (ਸਵੈਚਾਲਿਤ arg : ਦਲੀਲਾਂ_2)
            added.append_arg(arg);
        ਵਾਪਸ added;
    }

    ਸਾਂਚਾ<typename T>
    Ref ਜੋੜੋ(Fn ਅਚਲ &function, Attr &&attr, T &&ਦਲੀਲਾਂ_2)
    {
        Ref added = ਜੋੜੋ(function, std::move(attr));
        ਲਈ (ਸਵੈਚਾਲਿਤ arg : ਦਲੀਲਾਂ_2)
            added.append_arg(arg);
        ਵਾਪਸ added;
    }

    Ref ਜੋੜੋ(Fn ਅਚਲ &function, Attr ਅਚਲ &attr, std::initializer_list<Ref> ਦਲੀਲਾਂ_2)
    {
        Ref added = ਜੋੜੋ(function, attr);
        ਲਈ (ਸਵੈਚਾਲਿਤ arg : ਦਲੀਲਾਂ_2)
            added.append_arg(arg);
        ਵਾਪਸ added;
    }

    Ref ਜੋੜੋ(Fn ਅਚਲ &function, Attr &&attr, std::initializer_list<Ref> ਦਲੀਲਾਂ_2)
    {
        Ref added = ਜੋੜੋ(function, std::move(attr));
        ਲਈ (ਸਵੈਚਾਲਿਤ arg : ਦਲੀਲਾਂ_2)
            added.append_arg(arg);
        ਵਾਪਸ added;
    }

    ਸਾਂਚਾ<typename T>
    Ref ਜੋੜੋ(Fn ਅਚਲ &function, Attr ਅਚਲ &attr, T begin, T ਸਿਰਾ)
    {
        Ref added = ਜੋੜੋ(function, attr);
        ਲਈ (; begin != ਸਿਰਾ; ++begin)
            added.append_arg(*begin);
        ਵਾਪਸ added;
    }

    ਖਾਲੀ compact_ਦਲੀਲਾਂ()
    {
        std::ਸਦਿਸ਼<ਪੂਰਨ> new_ਦਲੀਲਾਂ;
        ਲਈ (ਸਵੈਚਾਲਿਤ &ਗੰਢ : nodes)
        {
            ਪੂਰਨ new_ਵਿੱਥ = Get_ਮਾਪ(new_ਦਲੀਲਾਂ);
            ਲਈ (ਪੂਰਨ i = 0; i < ਗੰਢ.arg_ਗਿਣਤੀ; i++)
                new_ਦਲੀਲਾਂ.ਧੱਕੋ_ਪਿੱਠ(ਦਲੀਲਾਂ_2[ਗੰਢ.arg_ਵਿੱਥ + i]);
            ਗੰਢ.arg_ਵਿੱਥ = new_ਵਿੱਥ;
        }
        std::ਅਦਲਾ_ਬਦਲੀ(ਦਲੀਲਾਂ_2, new_ਦਲੀਲਾਂ);
    }

    ਖਾਲੀ ਕ੍ਰਮ_ਬਦਲੋ(std::ਸਦਿਸ਼<ਪੂਰਨ> ਅਚਲ &perm)
    {
        log_assert(perm.ਮਾਪ() <= nodes.ਮਾਪ());
        std::ਸਦਿਸ਼<ਪੂਰਨ> inv_perm;
        inv_perm.resize(nodes.ਮਾਪ(), -1);
        ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(perm); ++i)
        {
            ਪੂਰਨ j = perm[i];
            log_assert(j >= 0 && j < Get_ਮਾਪ(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        ਕ੍ਰਮ_ਬਦਲੋ(perm, inv_perm);
    }

    ਖਾਲੀ ਕ੍ਰਮ_ਬਦਲੋ(std::ਸਦਿਸ਼<ਪੂਰਨ> ਅਚਲ &perm, std::ਸਦਿਸ਼<ਪੂਰਨ> ਅਚਲ &inv_perm)
    {
        log_assert(inv_perm.ਮਾਪ() == nodes.ਮਾਪ());
        std::ਸਦਿਸ਼<ਗੰਢ_2> new_nodes;
        new_nodes.reserve(perm.ਮਾਪ());
        dict<ਪੂਰਨ, SparseAttr> new_sparse_attrs;
        ਲਈ (ਪੂਰਨ i : perm)
        {
            ਪੂਰਨ j = Get_ਮਾਪ(new_nodes);
            new_nodes.emplace_ਪਿੱਠ(std::move(nodes[i]));
            ਸਵੈਚਾਲਿਤ ਮਿਲਿਆ = sparse_attrs.ਲੱਭੋ(i);
            ਜੇ (ਮਿਲਿਆ != sparse_attrs.ਸਿਰਾ())
                new_sparse_attrs.emplace(j, std::move(ਮਿਲਿਆ->ਦੂਜਾ));
        }

        std::ਅਦਲਾ_ਬਦਲੀ(nodes, new_nodes);
        std::ਅਦਲਾ_ਬਦਲੀ(sparse_attrs, new_sparse_attrs);

        compact_ਦਲੀਲਾਂ();
        ਲਈ (ਪੂਰਨ &arg : ਦਲੀਲਾਂ_2)
        {
            log_assert(arg < Get_ਮਾਪ(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        ਲਈ (ਸਵੈਚਾਲਿਤ &ਕੁੰਜੀ : ਕੁੰਜੀਆਂ)
        {
            log_assert(ਕੁੰਜੀ.ਦੂਜਾ < Get_ਮਾਪ(inv_perm));
            log_assert(inv_perm[ਕੁੰਜੀ.ਦੂਜਾ] >= 0);
            ਕੁੰਜੀ.ਦੂਜਾ = inv_perm[ਕੁੰਜੀ.ਦੂਜਾ];
        }
    }

    ਬਣਤਰ SccAdaptor
    {
    ਨਿਜੀ:
        Compute_ਗ੍ਰਾਫ ਅਚਲ &ਗ੍ਰਾਫ_2;
        std::ਸਦਿਸ਼<ਪੂਰਨ> indices_;
    ਜਨਤਕ:
        SccAdaptor(Compute_ਗ੍ਰਾਫ ਅਚਲ &ਗ੍ਰਾਫ_3) : ਗ੍ਰਾਫ_2(ਗ੍ਰਾਫ_3)
        {
            indices_.resize(ਗ੍ਰਾਫ_3.ਮਾਪ(), -1);
        }


        ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ ਗੰਢ_type;

        ਬਣਤਰ ਗੰਢ_enumerator {
        ਨਿਜੀ:
            ਮਿੱਤਰ ਬਣਤਰ SccAdaptor;
            ਪੂਰਨ current, ਸਿਰਾ;
            ਗੰਢ_enumerator(ਪੂਰਨ current, ਪੂਰਨ ਸਿਰਾ) : current(current), ਸਿਰਾ(ਸਿਰਾ) {}

        ਜਨਤਕ:

            bool ਮੁਕੰਮਲ() ਅਚਲ { ਵਾਪਸ current == ਸਿਰਾ; }
            ਗੰਢ_type ਅਗਲਾ() {
                log_assert(!ਮੁਕੰਮਲ());
                ਗੰਢ_type ਨਤੀਜਾ = current;
                ++current;
                ਵਾਪਸ ਨਤੀਜਾ;
            }
        };

        ਗੰਢ_enumerator enumerate_nodes() {
            ਵਾਪਸ ਗੰਢ_enumerator(0, Get_ਮਾਪ(indices_));
        }


        ਬਣਤਰ successor_enumerator {
        ਨਿਜੀ:
            ਮਿੱਤਰ ਬਣਤਰ SccAdaptor;
            std::ਸਦਿਸ਼<ਪੂਰਨ>::const_iterator current, ਸਿਰਾ;
            successor_enumerator(std::ਸਦਿਸ਼<ਪੂਰਨ>::const_iterator current, std::ਸਦਿਸ਼<ਪੂਰਨ>::const_iterator ਸਿਰਾ) :
                current(current), ਸਿਰਾ(ਸਿਰਾ) {}

        ਜਨਤਕ:
            bool ਮੁਕੰਮਲ() ਅਚਲ { ਵਾਪਸ current == ਸਿਰਾ; }
            ਗੰਢ_type ਅਗਲਾ() {
                log_assert(!ਮੁਕੰਮਲ());
                ਗੰਢ_type ਨਤੀਜਾ = *current;
                ++current;
                ਵਾਪਸ ਨਤੀਜਾ;
            }
        };

        successor_enumerator enumerate_ਉੱਤਰਾਧਿਕਾਰੀ(ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ {
            ਸਵੈਚਾਲਿਤ ਅਚਲ &ref = ਗ੍ਰਾਫ_2[ਸੂਚਕਾਂਕ];
            ਵਾਪਸ successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        ਪੂਰਨ &dfs_ਸੂਚਕਾਂਕ(ਗੰਢ_type ਅਚਲ &ਗੰਢ) { ਵਾਪਸ indices_[ਗੰਢ]; }

        std::ਸਦਿਸ਼<ਪੂਰਨ> ਅਚਲ &dfs_indices() { ਵਾਪਸ indices_; }
    };

};



YOSYS_NAMESPACE_ਸਿਰਾ


#ਜੇ_ਅੰਤ
