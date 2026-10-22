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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ COMPUTE_ನಕ್ಷೆ_H
#ವ್ಯಾಖ್ಯಾನಿಸು COMPUTE_ನಕ್ಷೆ_H

#ಅನ್ತರ್ಭಾವ <tuple>
#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ಮಾದರಿ<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename ಕೀಲಿ_2 = std::tuple<> // Stable keys to refer to nodes
>
ರಚನೆ Compute_ನಕ್ಷೆ
{
    ರಚನೆ Ref;
ಖಾಸಗಿ:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    ರಚನೆ ಗಂಟು_2 {
        ಪೂರ್ಣಾಂಕ fn_ಸೂಚ್ಯಂಕ;
        ಪೂರ್ಣಾಂಕ arg_ಸ್ಥಾನಾಂತರ;
        ಪೂರ್ಣಾಂಕ arg_ಎಣಿಕೆ;
        Attr attr;

        ಗಂಟು_2(ಪೂರ್ಣಾಂಕ fn_ಸೂಚ್ಯಂಕ, Attr &&attr, ಪೂರ್ಣಾಂಕ arg_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ arg_ಎಣಿಕೆ = 0)
            : fn_ಸೂಚ್ಯಂಕ(fn_ಸೂಚ್ಯಂಕ), arg_ಸ್ಥಾನಾಂತರ(arg_ಸ್ಥಾನಾಂತರ), arg_ಎಣಿಕೆ(arg_ಎಣಿಕೆ), attr(std::move(attr)) {}

        ಗಂಟು_2(ಪೂರ್ಣಾಂಕ fn_ಸೂಚ್ಯಂಕ, Attr ಸ್ಥಿರಾಂಕ &attr, ಪೂರ್ಣಾಂಕ arg_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ arg_ಎಣಿಕೆ = 0)
            : fn_ಸೂಚ್ಯಂಕ(fn_ಸೂಚ್ಯಂಕ), arg_ಸ್ಥಾನಾಂತರ(arg_ಸ್ಥಾನಾಂತರ), arg_ಎಣಿಕೆ(arg_ಎಣಿಕೆ), attr(attr) {}
    };


    std::ಸದಿಶ<ಗಂಟು_2> nodes;
    std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ವಾದಗಳು_2;
    dict<ಕೀಲಿ_2, ಪೂರ್ಣಾಂಕ> ಕೀಲಿಗಳು;
    dict<ಪೂರ್ಣಾಂಕ, SparseAttr> sparse_attrs;

ಸಾರ್ವಜನಿಕ:
    ಮಾದರಿ<typename ನಕ್ಷೆ>
    ರಚನೆ ತಳ_Ref
    {
    ಸಂರಕ್ಷಿತ:
        ಮಿತ್ರ ರಚನೆ Compute_ನಕ್ಷೆ;
        ನಕ್ಷೆ *ನಕ್ಷೆ_2;
        ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ_2;
        ತಳ_Ref(ನಕ್ಷೆ *ನಕ್ಷೆ_3, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) : ನಕ್ಷೆ_2(ನಕ್ಷೆ_3), ಸೂಚ್ಯಂಕ_2(ಸೂಚ್ಯಂಕ) {
            log_assert(ಸೂಚ್ಯಂಕ_2 >= 0);
            check();
        }

        ಶೂನ್ಯ check() ಸ್ಥಿರಾಂಕ { log_assert(ಸೂಚ್ಯಂಕ_2 < ನಕ್ಷೆ_2->ಗಾತ್ರ_2()); }

        ಗಂಟು_2 ಸ್ಥಿರಾಂಕ &deref() ಸ್ಥಿರಾಂಕ { check(); ಹಿಂತಿರುಗಿ ನಕ್ಷೆ_2->nodes[ಸೂಚ್ಯಂಕ_2]; }

    ಸಾರ್ವಜನಿಕ:
        Compute_ನಕ್ಷೆ ಸ್ಥಿರಾಂಕ &ನಕ್ಷೆ_3() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ನಕ್ಷೆ_2; }
        ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ_2; }

        ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ deref().arg_ಎಣಿಕೆ; }

        ತಳ_Ref arg(ಪೂರ್ಣಾಂಕ n) ಸ್ಥಿರಾಂಕ
        {
            ಗಂಟು_2 ಸ್ಥಿರಾಂಕ &ಗಂಟು = deref();
            log_assert(n >= 0 && n < ಗಂಟು.arg_ಎಣಿಕೆ);
            ಹಿಂತಿರುಗಿ ತಳ_Ref(ನಕ್ಷೆ_2, ನಕ್ಷೆ_2->ವಾದಗಳು_2[ಗಂಟು.arg_ಸ್ಥಾನಾಂತರ + n]);
        }

        std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ>::const_iterator arg_indices_cbegin() ಸ್ಥಿರಾಂಕ
        {
            ಗಂಟು_2 ಸ್ಥಿರಾಂಕ &ಗಂಟು = deref();
            ಹಿಂತಿರುಗಿ ನಕ್ಷೆ_2->ವಾದಗಳು_2.cbegin() + ಗಂಟು.arg_ಸ್ಥಾನಾಂತರ;
        }

        std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ>::const_iterator arg_indices_cend() ಸ್ಥಿರಾಂಕ
        {
            ಗಂಟು_2 ಸ್ಥಿರಾಂಕ &ಗಂಟು = deref();
            ಹಿಂತಿರುಗಿ ನಕ್ಷೆ_2->ವಾದಗಳು_2.cbegin() + ಗಂಟು.arg_ಸ್ಥಾನಾಂತರ + ಗಂಟು.arg_ಎಣಿಕೆ;
        }

        Fn ಸ್ಥಿರಾಂಕ &function() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ನಕ್ಷೆ_2->functions[deref().fn_ಸೂಚ್ಯಂಕ]; }
        Attr ಸ್ಥಿರಾಂಕ &attr() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ deref().attr; }

        bool has_sparse_attr() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ನಕ್ಷೆ_2->sparse_attrs.ಎಣಿಕೆ_2(ಸೂಚ್ಯಂಕ_2); }

        SparseAttr ಸ್ಥಿರಾಂಕ &sparse_attr() ಸ್ಥಿರಾಂಕ
        {
            ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು = ನಕ್ಷೆ_2->sparse_attrs.ಕಂಡುಹಿಡಿ(ಸೂಚ್ಯಂಕ_2);
            log_assert(ಸಿಕ್ಕಿತು != ನಕ್ಷೆ_2->sparse_attrs.ಕೊನೆ());
            ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು->ಎರಡನೇ;
        }
    };

    using ConstRef = ತಳ_Ref<Compute_ನಕ್ಷೆ ಸ್ಥಿರಾಂಕ>;

    ರಚನೆ Ref : ಸಾರ್ವಜನಿಕ ತಳ_Ref<Compute_ನಕ್ಷೆ>
    {
    ಖಾಸಗಿ:
        ಮಿತ್ರ ರಚನೆ Compute_ನಕ್ಷೆ;
        Ref(Compute_ನಕ್ಷೆ *ನಕ್ಷೆ_3, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) : ತಳ_Ref<Compute_ನಕ್ಷೆ>(ನಕ್ಷೆ_3, ಸೂಚ್ಯಂಕ) {}
        ಗಂಟು_2 &deref() ಸ್ಥಿರಾಂಕ { this->check(); ಹಿಂತಿರುಗಿ this->ನಕ್ಷೆ_2->nodes[this->ಸೂಚ್ಯಂಕ_2]; }

    ಸಾರ್ವಜನಿಕ:
        Ref(ತಳ_Ref<Compute_ನಕ್ಷೆ> ref) : Ref(ref.ನಕ್ಷೆ_2, ref.ಸೂಚ್ಯಂಕ_2) {}

        ಶೂನ್ಯ ಗಣ_function(Fn ಸ್ಥಿರಾಂಕ &function) ಸ್ಥಿರಾಂಕ
        {
            deref().fn_ಸೂಚ್ಯಂಕ = this->ನಕ್ಷೆ_2->functions(function);
        }

        Attr &attr() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ deref().attr; }

        ಶೂನ್ಯ append_arg(ConstRef arg) ಸ್ಥಿರಾಂಕ
        {
            log_assert(arg.ನಕ್ಷೆ_2 == this->ನಕ್ಷೆ_2);
            append_arg(arg.ಸೂಚ್ಯಂಕ());
        }

        ಶೂನ್ಯ append_arg(ಪೂರ್ಣಾಂಕ arg) ಸ್ಥಿರಾಂಕ
        {
            log_assert(arg >= 0 && arg < this->ನಕ್ಷೆ_2->ಗಾತ್ರ_2());
            ಗಂಟು_2 &ಗಂಟು = deref();
            ಆದರೆ (ಗಂಟು.arg_ಸ್ಥಾನಾಂತರ + ಗಂಟು.arg_ಎಣಿಕೆ != Get_ಗಾತ್ರ(this->ನಕ್ಷೆ_2->ವಾದಗಳು_2))
                move_ವಾದಗಳು(ಗಂಟು);
            this->ನಕ್ಷೆ_2->ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(arg);
            ಗಂಟು.arg_ಎಣಿಕೆ++;
        }

        ಕ್ರಿಯಾಕಾರಕ ConstRef() ಸ್ಥಿರಾಂಕ
        {
            ಹಿಂತಿರುಗಿ ConstRef(this->ನಕ್ಷೆ_2, this->ಸೂಚ್ಯಂಕ_2);
        }

        SparseAttr &sparse_attr() ಸ್ಥಿರಾಂಕ
        {
            ಹಿಂತಿರುಗಿ this->ನಕ್ಷೆ_2->sparse_attrs[this->ಸೂಚ್ಯಂಕ_2];
        }

        ಶೂನ್ಯ clear_sparse_attr() ಸ್ಥಿರಾಂಕ
        {
            this->ನಕ್ಷೆ_2->sparse_attrs.erase(this->ಸೂಚ್ಯಂಕ_2);
        }

        ಶೂನ್ಯ ನಿಯೋಜಿಸು_ಕೀಲಿ(ಕೀಲಿ_2 ಸ್ಥಿರಾಂಕ &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ
        {
            this->ನಕ್ಷೆ_2->ಕೀಲಿಗಳು.emplace(ಕೀಲಿ, this->ಸೂಚ್ಯಂಕ_2);
        }

    ಖಾಸಗಿ:
        ಶೂನ್ಯ move_ವಾದಗಳು(ಗಂಟು_2 &ಗಂಟು) ಸ್ಥಿರಾಂಕ
        {
            ಸ್ವಯಂಚಾಲಿತ &ವಾದಗಳು_2 = this->ನಕ್ಷೆ_2->ವಾದಗಳು_2;
            ಪೂರ್ಣಾಂಕ old_ಸ್ಥಾನಾಂತರ = ಗಂಟು.arg_ಸ್ಥಾನಾಂತರ;
            ಗಂಟು.arg_ಸ್ಥಾನಾಂತರ = Get_ಗಾತ್ರ(ವಾದಗಳು_2);
            ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i != ಗಂಟು.arg_ಎಣಿಕೆ; ++i)
                ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(ವಾದಗಳು_2[old_ಸ್ಥಾನಾಂತರ + i]);
        }

    };

    bool has_ಕೀಲಿ(ಕೀಲಿ_2 ಸ್ಥಿರಾಂಕ &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ
    {
        ಹಿಂತಿರುಗಿ ಕೀಲಿಗಳು.ಎಣಿಕೆ_2(ಕೀಲಿ);
    }

    dict<ಕೀಲಿ_2, ಪೂರ್ಣಾಂಕ> ಸ್ಥಿರಾಂಕ &ಕೀಲಿಗಳು_2() ಸ್ಥಿರಾಂಕ
    {
        ಹಿಂತಿರುಗಿ ಕೀಲಿಗಳು;
    }

    ConstRef ಕ್ರಿಯಾಕಾರಕ()(ಕೀಲಿ_2 ಸ್ಥಿರಾಂಕ &ಕೀಲಿ) ಸ್ಥಿರಾಂಕ
    {
        ಸ್ವಯಂಚಾಲಿತ it = ಕೀಲಿಗಳು.ಕಂಡುಹಿಡಿ(ಕೀಲಿ);
        log_assert(it != ಕೀಲಿಗಳು.ಕೊನೆ());
        ಹಿಂತಿರುಗಿ (*this)[it->ಎರಡನೇ];
    }

    Ref ಕ್ರಿಯಾಕಾರಕ()(ಕೀಲಿ_2 ಸ್ಥಿರಾಂಕ &ಕೀಲಿ)
    {
        ಸ್ವಯಂಚಾಲಿತ it = ಕೀಲಿಗಳು.ಕಂಡುಹಿಡಿ(ಕೀಲಿ);
        log_assert(it != ಕೀಲಿಗಳು.ಕೊನೆ());
        ಹಿಂತಿರುಗಿ (*this)[it->ಎರಡನೇ];
    }

    ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ Get_ಗಾತ್ರ(nodes); }

    ConstRef ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ConstRef(this, ಸೂಚ್ಯಂಕ); }
    Ref ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) { ಹಿಂತಿರುಗಿ Ref(this, ಸೂಚ್ಯಂಕ); }

    Ref ಕೂಡಿಸು(Fn ಸ್ಥಿರಾಂಕ &function, Attr &&attr)
    {
        ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = Get_ಗಾತ್ರ(nodes);
        ಪೂರ್ಣಾಂಕ fn_ಸೂಚ್ಯಂಕ = functions(function);
        nodes.emplace_ಹಿಂಭಾಗ(fn_ಸೂಚ್ಯಂಕ, std::move(attr), Get_ಗಾತ್ರ(ವಾದಗಳು_2));
        ಹಿಂತಿರುಗಿ Ref(this, ಸೂಚ್ಯಂಕ);
    }

    Ref ಕೂಡಿಸು(Fn ಸ್ಥಿರಾಂಕ &function, Attr ಸ್ಥಿರಾಂಕ &attr)
    {
        ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = Get_ಗಾತ್ರ(nodes);
        ಪೂರ್ಣಾಂಕ fn_ಸೂಚ್ಯಂಕ = functions(function);
        nodes.emplace_ಹಿಂಭಾಗ(fn_ಸೂಚ್ಯಂಕ, attr,  Get_ಗಾತ್ರ(ವಾದಗಳು_2));
        ಹಿಂತಿರುಗಿ Ref(this, ಸೂಚ್ಯಂಕ);
    }

    ಮಾದರಿ<typename T>
    Ref ಕೂಡಿಸು(Fn ಸ್ಥಿರಾಂಕ &function, Attr ಸ್ಥಿರಾಂಕ &attr, T &&ವಾದಗಳು_2)
    {
        Ref added = ಕೂಡಿಸು(function, attr);
        ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ arg : ವಾದಗಳು_2)
            added.append_arg(arg);
        ಹಿಂತಿರುಗಿ added;
    }

    ಮಾದರಿ<typename T>
    Ref ಕೂಡಿಸು(Fn ಸ್ಥಿರಾಂಕ &function, Attr &&attr, T &&ವಾದಗಳು_2)
    {
        Ref added = ಕೂಡಿಸು(function, std::move(attr));
        ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ arg : ವಾದಗಳು_2)
            added.append_arg(arg);
        ಹಿಂತಿರುಗಿ added;
    }

    Ref ಕೂಡಿಸು(Fn ಸ್ಥಿರಾಂಕ &function, Attr ಸ್ಥಿರಾಂಕ &attr, std::initializer_list<Ref> ವಾದಗಳು_2)
    {
        Ref added = ಕೂಡಿಸು(function, attr);
        ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ arg : ವಾದಗಳು_2)
            added.append_arg(arg);
        ಹಿಂತಿರುಗಿ added;
    }

    Ref ಕೂಡಿಸು(Fn ಸ್ಥಿರಾಂಕ &function, Attr &&attr, std::initializer_list<Ref> ವಾದಗಳು_2)
    {
        Ref added = ಕೂಡಿಸು(function, std::move(attr));
        ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ arg : ವಾದಗಳು_2)
            added.append_arg(arg);
        ಹಿಂತಿರುಗಿ added;
    }

    ಮಾದರಿ<typename T>
    Ref ಕೂಡಿಸು(Fn ಸ್ಥಿರಾಂಕ &function, Attr ಸ್ಥಿರಾಂಕ &attr, T begin, T ಕೊನೆ)
    {
        Ref added = ಕೂಡಿಸು(function, attr);
        ಗಾಗಿ (; begin != ಕೊನೆ; ++begin)
            added.append_arg(*begin);
        ಹಿಂತಿರುಗಿ added;
    }

    ಶೂನ್ಯ compact_ವಾದಗಳು()
    {
        std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> new_ವಾದಗಳು;
        ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಗಂಟು : nodes)
        {
            ಪೂರ್ಣಾಂಕ new_ಸ್ಥಾನಾಂತರ = Get_ಗಾತ್ರ(new_ವಾದಗಳು);
            ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಗಂಟು.arg_ಎಣಿಕೆ; i++)
                new_ವಾದಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(ವಾದಗಳು_2[ಗಂಟು.arg_ಸ್ಥಾನಾಂತರ + i]);
            ಗಂಟು.arg_ಸ್ಥಾನಾಂತರ = new_ಸ್ಥಾನಾಂತರ;
        }
        std::ಅದಲುಬದಲು(ವಾದಗಳು_2, new_ವಾದಗಳು);
    }

    ಶೂನ್ಯ ಕ್ರಮಬದಲಾವಣೆ(std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ಸ್ಥಿರಾಂಕ &perm)
    {
        log_assert(perm.ಗಾತ್ರ_2() <= nodes.ಗಾತ್ರ_2());
        std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> inv_perm;
        inv_perm.resize(nodes.ಗಾತ್ರ_2(), -1);
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(perm); ++i)
        {
            ಪೂರ್ಣಾಂಕ j = perm[i];
            log_assert(j >= 0 && j < Get_ಗಾತ್ರ(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        ಕ್ರಮಬದಲಾವಣೆ(perm, inv_perm);
    }

    ಶೂನ್ಯ ಕ್ರಮಬದಲಾವಣೆ(std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ಸ್ಥಿರಾಂಕ &perm, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ಸ್ಥಿರಾಂಕ &inv_perm)
    {
        log_assert(inv_perm.ಗಾತ್ರ_2() == nodes.ಗಾತ್ರ_2());
        std::ಸದಿಶ<ಗಂಟು_2> new_nodes;
        new_nodes.reserve(perm.ಗಾತ್ರ_2());
        dict<ಪೂರ್ಣಾಂಕ, SparseAttr> new_sparse_attrs;
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i : perm)
        {
            ಪೂರ್ಣಾಂಕ j = Get_ಗಾತ್ರ(new_nodes);
            new_nodes.emplace_ಹಿಂಭಾಗ(std::move(nodes[i]));
            ಸ್ವಯಂಚಾಲಿತ ಸಿಕ್ಕಿತು = sparse_attrs.ಕಂಡುಹಿಡಿ(i);
            ಆದರೆ (ಸಿಕ್ಕಿತು != sparse_attrs.ಕೊನೆ())
                new_sparse_attrs.emplace(j, std::move(ಸಿಕ್ಕಿತು->ಎರಡನೇ));
        }

        std::ಅದಲುಬದಲು(nodes, new_nodes);
        std::ಅದಲುಬದಲು(sparse_attrs, new_sparse_attrs);

        compact_ವಾದಗಳು();
        ಗಾಗಿ (ಪೂರ್ಣಾಂಕ &arg : ವಾದಗಳು_2)
        {
            log_assert(arg < Get_ಗಾತ್ರ(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಕೀಲಿ : ಕೀಲಿಗಳು)
        {
            log_assert(ಕೀಲಿ.ಎರಡನೇ < Get_ಗಾತ್ರ(inv_perm));
            log_assert(inv_perm[ಕೀಲಿ.ಎರಡನೇ] >= 0);
            ಕೀಲಿ.ಎರಡನೇ = inv_perm[ಕೀಲಿ.ಎರಡನೇ];
        }
    }

    ರಚನೆ SccAdaptor
    {
    ಖಾಸಗಿ:
        Compute_ನಕ್ಷೆ ಸ್ಥಿರಾಂಕ &ನಕ್ಷೆ_2;
        std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> indices_;
    ಸಾರ್ವಜನಿಕ:
        SccAdaptor(Compute_ನಕ್ಷೆ ಸ್ಥಿರಾಂಕ &ನಕ್ಷೆ_3) : ನಕ್ಷೆ_2(ನಕ್ಷೆ_3)
        {
            indices_.resize(ನಕ್ಷೆ_3.ಗಾತ್ರ_2(), -1);
        }


        ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ ಗಂಟು_type;

        ರಚನೆ ಗಂಟು_enumerator {
        ಖಾಸಗಿ:
            ಮಿತ್ರ ರಚನೆ SccAdaptor;
            ಪೂರ್ಣಾಂಕ current, ಕೊನೆ;
            ಗಂಟು_enumerator(ಪೂರ್ಣಾಂಕ current, ಪೂರ್ಣಾಂಕ ಕೊನೆ) : current(current), ಕೊನೆ(ಕೊನೆ) {}

        ಸಾರ್ವಜನಿಕ:

            bool ಪೂರ್ಣಗೊಂಡ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ current == ಕೊನೆ; }
            ಗಂಟು_type ಮುಂದಿನ() {
                log_assert(!ಪೂರ್ಣಗೊಂಡ());
                ಗಂಟು_type ಫಲಿತಾಂಶ = current;
                ++current;
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }
        };

        ಗಂಟು_enumerator enumerate_nodes() {
            ಹಿಂತಿರುಗಿ ಗಂಟು_enumerator(0, Get_ಗಾತ್ರ(indices_));
        }


        ರಚನೆ successor_enumerator {
        ಖಾಸಗಿ:
            ಮಿತ್ರ ರಚನೆ SccAdaptor;
            std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ>::const_iterator current, ಕೊನೆ;
            successor_enumerator(std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ>::const_iterator current, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ>::const_iterator ಕೊನೆ) :
                current(current), ಕೊನೆ(ಕೊನೆ) {}

        ಸಾರ್ವಜನಿಕ:
            bool ಪೂರ್ಣಗೊಂಡ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ current == ಕೊನೆ; }
            ಗಂಟು_type ಮುಂದಿನ() {
                log_assert(!ಪೂರ್ಣಗೊಂಡ());
                ಗಂಟು_type ಫಲಿತಾಂಶ = *current;
                ++current;
                ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
            }
        };

        successor_enumerator enumerate_ಉತ್ತರಾಧಿಕಾರಿಗಳು(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
            ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &ref = ನಕ್ಷೆ_2[ಸೂಚ್ಯಂಕ];
            ಹಿಂತಿರುಗಿ successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        ಪೂರ್ಣಾಂಕ &dfs_ಸೂಚ್ಯಂಕ(ಗಂಟು_type ಸ್ಥಿರಾಂಕ &ಗಂಟು) { ಹಿಂತಿರುಗಿ indices_[ಗಂಟು]; }

        std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ಸ್ಥಿರಾಂಕ &dfs_indices() { ಹಿಂತಿರುಗಿ indices_; }
    };

};



YOSYS_NAMESPACE_ಕೊನೆ


#ಆದರೆ_ಅಂತ್ಯ
