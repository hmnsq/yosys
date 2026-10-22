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

#اگر_تعریف_نشده COMPUTE_گراف_H
#تعریف_کن COMPUTE_گراف_H

#ضمن <tuple>
#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

قالب<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename کلید_2 = std::tuple<> // Stable keys to refer to nodes
>
بنية Compute_گراف
{
    بنية Ref;
خاص:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    بنية گره_2 {
        صحيح fn_شاخص;
        صحيح arg_جابه_جایی;
        صحيح arg_شمار;
        Attr attr;

        گره_2(صحيح fn_شاخص, Attr &&attr, صحيح arg_جابه_جایی, صحيح arg_شمار = 0)
            : fn_شاخص(fn_شاخص), arg_جابه_جایی(arg_جابه_جایی), arg_شمار(arg_شمار), attr(std::move(attr)) {}

        گره_2(صحيح fn_شاخص, Attr ثابت &attr, صحيح arg_جابه_جایی, صحيح arg_شمار = 0)
            : fn_شاخص(fn_شاخص), arg_جابه_جایی(arg_جابه_جایی), arg_شمار(arg_شمار), attr(attr) {}
    };


    std::وکتور<گره_2> nodes;
    std::وکتور<صحيح> آرگومانها_2;
    dict<کلید_2, صحيح> کلیدها;
    dict<صحيح, SparseAttr> sparse_attrs;

عام:
    قالب<typename گراف>
    بنية پایه_Ref
    {
    محمي:
        دوست بنية Compute_گراف;
        گراف *گراف_2;
        صحيح شاخص_2;
        پایه_Ref(گراف *گراف_3, صحيح شاخص) : گراف_2(گراف_3), شاخص_2(شاخص) {
            log_assert(شاخص_2 >= 0);
            check();
        }

        فراغ check() ثابت { log_assert(شاخص_2 < گراف_2->اندازه()); }

        گره_2 ثابت &deref() ثابت { check(); أرجع گراف_2->nodes[شاخص_2]; }

    عام:
        Compute_گراف ثابت &گراف_3() ثابت { أرجع گراف_2; }
        صحيح شاخص() ثابت { أرجع شاخص_2; }

        صحيح اندازه() ثابت { أرجع deref().arg_شمار; }

        پایه_Ref arg(صحيح n) ثابت
        {
            گره_2 ثابت &گره = deref();
            log_assert(n >= 0 && n < گره.arg_شمار);
            أرجع پایه_Ref(گراف_2, گراف_2->آرگومانها_2[گره.arg_جابه_جایی + n]);
        }

        std::وکتور<صحيح>::const_iterator arg_indices_cbegin() ثابت
        {
            گره_2 ثابت &گره = deref();
            أرجع گراف_2->آرگومانها_2.cbegin() + گره.arg_جابه_جایی;
        }

        std::وکتور<صحيح>::const_iterator arg_indices_cend() ثابت
        {
            گره_2 ثابت &گره = deref();
            أرجع گراف_2->آرگومانها_2.cbegin() + گره.arg_جابه_جایی + گره.arg_شمار;
        }

        Fn ثابت &function() ثابت { أرجع گراف_2->functions[deref().fn_شاخص]; }
        Attr ثابت &attr() ثابت { أرجع deref().attr; }

        bool has_sparse_attr() ثابت { أرجع گراف_2->sparse_attrs.شمار(شاخص_2); }

        SparseAttr ثابت &sparse_attr() ثابت
        {
            خودکار یافت = گراف_2->sparse_attrs.پیدا_کن(شاخص_2);
            log_assert(یافت != گراف_2->sparse_attrs.انتها());
            أرجع یافت->دومی;
        }
    };

    using ConstRef = پایه_Ref<Compute_گراف ثابت>;

    بنية Ref : عام پایه_Ref<Compute_گراف>
    {
    خاص:
        دوست بنية Compute_گراف;
        Ref(Compute_گراف *گراف_3, صحيح شاخص) : پایه_Ref<Compute_گراف>(گراف_3, شاخص) {}
        گره_2 &deref() ثابت { this->check(); أرجع this->گراف_2->nodes[this->شاخص_2]; }

    عام:
        Ref(پایه_Ref<Compute_گراف> ref) : Ref(ref.گراف_2, ref.شاخص_2) {}

        فراغ مجموعه_function(Fn ثابت &function) ثابت
        {
            deref().fn_شاخص = this->گراف_2->functions(function);
        }

        Attr &attr() ثابت { أرجع deref().attr; }

        فراغ append_arg(ConstRef arg) ثابت
        {
            log_assert(arg.گراف_2 == this->گراف_2);
            append_arg(arg.شاخص());
        }

        فراغ append_arg(صحيح arg) ثابت
        {
            log_assert(arg >= 0 && arg < this->گراف_2->اندازه());
            گره_2 &گره = deref();
            إذا (گره.arg_جابه_جایی + گره.arg_شمار != Get_اندازه(this->گراف_2->آرگومانها_2))
                move_آرگومانها(گره);
            this->گراف_2->آرگومانها_2.افزودن_پشت(arg);
            گره.arg_شمار++;
        }

        عملگر ConstRef() ثابت
        {
            أرجع ConstRef(this->گراف_2, this->شاخص_2);
        }

        SparseAttr &sparse_attr() ثابت
        {
            أرجع this->گراف_2->sparse_attrs[this->شاخص_2];
        }

        فراغ clear_sparse_attr() ثابت
        {
            this->گراف_2->sparse_attrs.erase(this->شاخص_2);
        }

        فراغ تخصیص_بده_کلید(کلید_2 ثابت &کلید) ثابت
        {
            this->گراف_2->کلیدها.emplace(کلید, this->شاخص_2);
        }

    خاص:
        فراغ move_آرگومانها(گره_2 &گره) ثابت
        {
            خودکار &آرگومانها_2 = this->گراف_2->آرگومانها_2;
            صحيح old_جابه_جایی = گره.arg_جابه_جایی;
            گره.arg_جابه_جایی = Get_اندازه(آرگومانها_2);
            لكل (صحيح i = 0; i != گره.arg_شمار; ++i)
                آرگومانها_2.افزودن_پشت(آرگومانها_2[old_جابه_جایی + i]);
        }

    };

    bool has_کلید(کلید_2 ثابت &کلید) ثابت
    {
        أرجع کلیدها.شمار(کلید);
    }

    dict<کلید_2, صحيح> ثابت &کلیدها_2() ثابت
    {
        أرجع کلیدها;
    }

    ConstRef عملگر()(کلید_2 ثابت &کلید) ثابت
    {
        خودکار it = کلیدها.پیدا_کن(کلید);
        log_assert(it != کلیدها.انتها());
        أرجع (*this)[it->دومی];
    }

    Ref عملگر()(کلید_2 ثابت &کلید)
    {
        خودکار it = کلیدها.پیدا_کن(کلید);
        log_assert(it != کلیدها.انتها());
        أرجع (*this)[it->دومی];
    }

    صحيح اندازه() ثابت { أرجع Get_اندازه(nodes); }

    ConstRef عملگر[](صحيح شاخص) ثابت { أرجع ConstRef(this, شاخص); }
    Ref عملگر[](صحيح شاخص) { أرجع Ref(this, شاخص); }

    Ref جمع_کن(Fn ثابت &function, Attr &&attr)
    {
        صحيح شاخص = Get_اندازه(nodes);
        صحيح fn_شاخص = functions(function);
        nodes.emplace_پشت(fn_شاخص, std::move(attr), Get_اندازه(آرگومانها_2));
        أرجع Ref(this, شاخص);
    }

    Ref جمع_کن(Fn ثابت &function, Attr ثابت &attr)
    {
        صحيح شاخص = Get_اندازه(nodes);
        صحيح fn_شاخص = functions(function);
        nodes.emplace_پشت(fn_شاخص, attr,  Get_اندازه(آرگومانها_2));
        أرجع Ref(this, شاخص);
    }

    قالب<typename T>
    Ref جمع_کن(Fn ثابت &function, Attr ثابت &attr, T &&آرگومانها_2)
    {
        Ref added = جمع_کن(function, attr);
        لكل (خودکار arg : آرگومانها_2)
            added.append_arg(arg);
        أرجع added;
    }

    قالب<typename T>
    Ref جمع_کن(Fn ثابت &function, Attr &&attr, T &&آرگومانها_2)
    {
        Ref added = جمع_کن(function, std::move(attr));
        لكل (خودکار arg : آرگومانها_2)
            added.append_arg(arg);
        أرجع added;
    }

    Ref جمع_کن(Fn ثابت &function, Attr ثابت &attr, std::initializer_list<Ref> آرگومانها_2)
    {
        Ref added = جمع_کن(function, attr);
        لكل (خودکار arg : آرگومانها_2)
            added.append_arg(arg);
        أرجع added;
    }

    Ref جمع_کن(Fn ثابت &function, Attr &&attr, std::initializer_list<Ref> آرگومانها_2)
    {
        Ref added = جمع_کن(function, std::move(attr));
        لكل (خودکار arg : آرگومانها_2)
            added.append_arg(arg);
        أرجع added;
    }

    قالب<typename T>
    Ref جمع_کن(Fn ثابت &function, Attr ثابت &attr, T begin, T انتها)
    {
        Ref added = جمع_کن(function, attr);
        لكل (; begin != انتها; ++begin)
            added.append_arg(*begin);
        أرجع added;
    }

    فراغ compact_آرگومانها()
    {
        std::وکتور<صحيح> new_آرگومانها;
        لكل (خودکار &گره : nodes)
        {
            صحيح new_جابه_جایی = Get_اندازه(new_آرگومانها);
            لكل (صحيح i = 0; i < گره.arg_شمار; i++)
                new_آرگومانها.افزودن_پشت(آرگومانها_2[گره.arg_جابه_جایی + i]);
            گره.arg_جابه_جایی = new_جابه_جایی;
        }
        std::جابجا(آرگومانها_2, new_آرگومانها);
    }

    فراغ جایگشت(std::وکتور<صحيح> ثابت &perm)
    {
        log_assert(perm.اندازه() <= nodes.اندازه());
        std::وکتور<صحيح> inv_perm;
        inv_perm.resize(nodes.اندازه(), -1);
        لكل (صحيح i = 0; i < Get_اندازه(perm); ++i)
        {
            صحيح j = perm[i];
            log_assert(j >= 0 && j < Get_اندازه(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        جایگشت(perm, inv_perm);
    }

    فراغ جایگشت(std::وکتور<صحيح> ثابت &perm, std::وکتور<صحيح> ثابت &inv_perm)
    {
        log_assert(inv_perm.اندازه() == nodes.اندازه());
        std::وکتور<گره_2> new_nodes;
        new_nodes.reserve(perm.اندازه());
        dict<صحيح, SparseAttr> new_sparse_attrs;
        لكل (صحيح i : perm)
        {
            صحيح j = Get_اندازه(new_nodes);
            new_nodes.emplace_پشت(std::move(nodes[i]));
            خودکار یافت = sparse_attrs.پیدا_کن(i);
            إذا (یافت != sparse_attrs.انتها())
                new_sparse_attrs.emplace(j, std::move(یافت->دومی));
        }

        std::جابجا(nodes, new_nodes);
        std::جابجا(sparse_attrs, new_sparse_attrs);

        compact_آرگومانها();
        لكل (صحيح &arg : آرگومانها_2)
        {
            log_assert(arg < Get_اندازه(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        لكل (خودکار &کلید : کلیدها)
        {
            log_assert(کلید.دومی < Get_اندازه(inv_perm));
            log_assert(inv_perm[کلید.دومی] >= 0);
            کلید.دومی = inv_perm[کلید.دومی];
        }
    }

    بنية SccAdaptor
    {
    خاص:
        Compute_گراف ثابت &گراف_2;
        std::وکتور<صحيح> indices_;
    عام:
        SccAdaptor(Compute_گراف ثابت &گراف_3) : گراف_2(گراف_3)
        {
            indices_.resize(گراف_3.اندازه(), -1);
        }


        تعريفنوع صحيح گره_type;

        بنية گره_enumerator {
        خاص:
            دوست بنية SccAdaptor;
            صحيح current, انتها;
            گره_enumerator(صحيح current, صحيح انتها) : current(current), انتها(انتها) {}

        عام:

            bool پایانیافته() ثابت { أرجع current == انتها; }
            گره_type بعدی() {
                log_assert(!پایانیافته());
                گره_type نتیجه = current;
                ++current;
                أرجع نتیجه;
            }
        };

        گره_enumerator enumerate_nodes() {
            أرجع گره_enumerator(0, Get_اندازه(indices_));
        }


        بنية successor_enumerator {
        خاص:
            دوست بنية SccAdaptor;
            std::وکتور<صحيح>::const_iterator current, انتها;
            successor_enumerator(std::وکتور<صحيح>::const_iterator current, std::وکتور<صحيح>::const_iterator انتها) :
                current(current), انتها(انتها) {}

        عام:
            bool پایانیافته() ثابت { أرجع current == انتها; }
            گره_type بعدی() {
                log_assert(!پایانیافته());
                گره_type نتیجه = *current;
                ++current;
                أرجع نتیجه;
            }
        };

        successor_enumerator enumerate_جانشینها(صحيح شاخص) ثابت {
            خودکار ثابت &ref = گراف_2[شاخص];
            أرجع successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        صحيح &dfs_شاخص(گره_type ثابت &گره) { أرجع indices_[گره]; }

        std::وکتور<صحيح> ثابت &dfs_indices() { أرجع indices_; }
    };

};



YOSYS_NAMESPACE_انتها


#پایان_اگر
