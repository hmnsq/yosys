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

#که_نه_تعریف_شوی COMPUTE_ګراف_H
#تعریف_کړه COMPUTE_ګراف_H

#ضمن <tuple>
#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

کينډۍ<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename کلی_2 = std::tuple<> // Stable keys to refer to nodes
>
بنية Compute_ګراف
{
    بنية Ref;
خاص:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    بنية غوټه_2 {
        صحيح fn_شاخص;
        صحيح arg_دننه_واټن;
        صحيح arg_شمېر;
        Attr attr;

        غوټه_2(صحيح fn_شاخص, Attr &&attr, صحيح arg_دننه_واټن, صحيح arg_شمېر = 0)
            : fn_شاخص(fn_شاخص), arg_دننه_واټن(arg_دننه_واټن), arg_شمېر(arg_شمېر), attr(std::move(attr)) {}

        غوټه_2(صحيح fn_شاخص, Attr ثابت &attr, صحيح arg_دننه_واټن, صحيح arg_شمېر = 0)
            : fn_شاخص(fn_شاخص), arg_دننه_واټن(arg_دننه_واټن), arg_شمېر(arg_شمېر), attr(attr) {}
    };


    std::ویکټور<غوټه_2> nodes;
    std::ویکټور<صحيح> دلیلونه_2;
    dict<کلی_2, صحيح> کلیګانې;
    dict<صحيح, SparseAttr> sparse_attrs;

عام:
    کينډۍ<typename ګراف>
    بنية بنسټ_Ref
    {
    محمي:
        ملګری بنية Compute_ګراف;
        ګراف *ګراف_2;
        صحيح شاخص_2;
        بنسټ_Ref(ګراف *ګراف_3, صحيح شاخص) : ګراف_2(ګراف_3), شاخص_2(شاخص) {
            log_assert(شاخص_2 >= 0);
            check();
        }

        فراغ check() ثابت { log_assert(شاخص_2 < ګراف_2->اندازه()); }

        غوټه_2 ثابت &deref() ثابت { check(); أرجع ګراف_2->nodes[شاخص_2]; }

    عام:
        Compute_ګراف ثابت &ګراف_3() ثابت { أرجع ګراف_2; }
        صحيح شاخص() ثابت { أرجع شاخص_2; }

        صحيح اندازه() ثابت { أرجع deref().arg_شمېر; }

        بنسټ_Ref arg(صحيح n) ثابت
        {
            غوټه_2 ثابت &غوټه = deref();
            log_assert(n >= 0 && n < غوټه.arg_شمېر);
            أرجع بنسټ_Ref(ګراف_2, ګراف_2->دلیلونه_2[غوټه.arg_دننه_واټن + n]);
        }

        std::ویکټور<صحيح>::const_iterator arg_indices_cbegin() ثابت
        {
            غوټه_2 ثابت &غوټه = deref();
            أرجع ګراف_2->دلیلونه_2.cbegin() + غوټه.arg_دننه_واټن;
        }

        std::ویکټور<صحيح>::const_iterator arg_indices_cend() ثابت
        {
            غوټه_2 ثابت &غوټه = deref();
            أرجع ګراف_2->دلیلونه_2.cbegin() + غوټه.arg_دننه_واټن + غوټه.arg_شمېر;
        }

        Fn ثابت &function() ثابت { أرجع ګراف_2->functions[deref().fn_شاخص]; }
        Attr ثابت &attr() ثابت { أرجع deref().attr; }

        bool has_sparse_attr() ثابت { أرجع ګراف_2->sparse_attrs.شمېر(شاخص_2); }

        SparseAttr ثابت &sparse_attr() ثابت
        {
            خپلکاری وموندل = ګراف_2->sparse_attrs.پیدا_کړه(شاخص_2);
            log_assert(وموندل != ګراف_2->sparse_attrs.پای_ټکی());
            أرجع وموندل->دوهم;
        }
    };

    using ConstRef = بنسټ_Ref<Compute_ګراف ثابت>;

    بنية Ref : عام بنسټ_Ref<Compute_ګراف>
    {
    خاص:
        ملګری بنية Compute_ګراف;
        Ref(Compute_ګراف *ګراف_3, صحيح شاخص) : بنسټ_Ref<Compute_ګراف>(ګراف_3, شاخص) {}
        غوټه_2 &deref() ثابت { this->check(); أرجع this->ګراف_2->nodes[this->شاخص_2]; }

    عام:
        Ref(بنسټ_Ref<Compute_ګراف> ref) : Ref(ref.ګراف_2, ref.شاخص_2) {}

        فراغ سېټ_function(Fn ثابت &function) ثابت
        {
            deref().fn_شاخص = this->ګراف_2->functions(function);
        }

        Attr &attr() ثابت { أرجع deref().attr; }

        فراغ append_arg(ConstRef arg) ثابت
        {
            log_assert(arg.ګراف_2 == this->ګراف_2);
            append_arg(arg.شاخص());
        }

        فراغ append_arg(صحيح arg) ثابت
        {
            log_assert(arg >= 0 && arg < this->ګراف_2->اندازه());
            غوټه_2 &غوټه = deref();
            إذا (غوټه.arg_دننه_واټن + غوټه.arg_شمېر != Get_اندازه(this->ګراف_2->دلیلونه_2))
                move_دلیلونه(غوټه);
            this->ګراف_2->دلیلونه_2.ټېل_شا(arg);
            غوټه.arg_شمېر++;
        }

        چلوونکی ConstRef() ثابت
        {
            أرجع ConstRef(this->ګراف_2, this->شاخص_2);
        }

        SparseAttr &sparse_attr() ثابت
        {
            أرجع this->ګراف_2->sparse_attrs[this->شاخص_2];
        }

        فراغ clear_sparse_attr() ثابت
        {
            this->ګراف_2->sparse_attrs.erase(this->شاخص_2);
        }

        فراغ ټاکل_کلی(کلی_2 ثابت &کلی) ثابت
        {
            this->ګراف_2->کلیګانې.emplace(کلی, this->شاخص_2);
        }

    خاص:
        فراغ move_دلیلونه(غوټه_2 &غوټه) ثابت
        {
            خپلکاری &دلیلونه_2 = this->ګراف_2->دلیلونه_2;
            صحيح old_دننه_واټن = غوټه.arg_دننه_واټن;
            غوټه.arg_دننه_واټن = Get_اندازه(دلیلونه_2);
            لكل (صحيح i = 0; i != غوټه.arg_شمېر; ++i)
                دلیلونه_2.ټېل_شا(دلیلونه_2[old_دننه_واټن + i]);
        }

    };

    bool has_کلی(کلی_2 ثابت &کلی) ثابت
    {
        أرجع کلیګانې.شمېر(کلی);
    }

    dict<کلی_2, صحيح> ثابت &کلیګانې_2() ثابت
    {
        أرجع کلیګانې;
    }

    ConstRef چلوونکی()(کلی_2 ثابت &کلی) ثابت
    {
        خپلکاری it = کلیګانې.پیدا_کړه(کلی);
        log_assert(it != کلیګانې.پای_ټکی());
        أرجع (*this)[it->دوهم];
    }

    Ref چلوونکی()(کلی_2 ثابت &کلی)
    {
        خپلکاری it = کلیګانې.پیدا_کړه(کلی);
        log_assert(it != کلیګانې.پای_ټکی());
        أرجع (*this)[it->دوهم];
    }

    صحيح اندازه() ثابت { أرجع Get_اندازه(nodes); }

    ConstRef چلوونکی[](صحيح شاخص) ثابت { أرجع ConstRef(this, شاخص); }
    Ref چلوونکی[](صحيح شاخص) { أرجع Ref(this, شاخص); }

    Ref جمع_کړه(Fn ثابت &function, Attr &&attr)
    {
        صحيح شاخص = Get_اندازه(nodes);
        صحيح fn_شاخص = functions(function);
        nodes.emplace_شا(fn_شاخص, std::move(attr), Get_اندازه(دلیلونه_2));
        أرجع Ref(this, شاخص);
    }

    Ref جمع_کړه(Fn ثابت &function, Attr ثابت &attr)
    {
        صحيح شاخص = Get_اندازه(nodes);
        صحيح fn_شاخص = functions(function);
        nodes.emplace_شا(fn_شاخص, attr,  Get_اندازه(دلیلونه_2));
        أرجع Ref(this, شاخص);
    }

    کينډۍ<typename T>
    Ref جمع_کړه(Fn ثابت &function, Attr ثابت &attr, T &&دلیلونه_2)
    {
        Ref added = جمع_کړه(function, attr);
        لكل (خپلکاری arg : دلیلونه_2)
            added.append_arg(arg);
        أرجع added;
    }

    کينډۍ<typename T>
    Ref جمع_کړه(Fn ثابت &function, Attr &&attr, T &&دلیلونه_2)
    {
        Ref added = جمع_کړه(function, std::move(attr));
        لكل (خپلکاری arg : دلیلونه_2)
            added.append_arg(arg);
        أرجع added;
    }

    Ref جمع_کړه(Fn ثابت &function, Attr ثابت &attr, std::initializer_list<Ref> دلیلونه_2)
    {
        Ref added = جمع_کړه(function, attr);
        لكل (خپلکاری arg : دلیلونه_2)
            added.append_arg(arg);
        أرجع added;
    }

    Ref جمع_کړه(Fn ثابت &function, Attr &&attr, std::initializer_list<Ref> دلیلونه_2)
    {
        Ref added = جمع_کړه(function, std::move(attr));
        لكل (خپلکاری arg : دلیلونه_2)
            added.append_arg(arg);
        أرجع added;
    }

    کينډۍ<typename T>
    Ref جمع_کړه(Fn ثابت &function, Attr ثابت &attr, T begin, T پای_ټکی)
    {
        Ref added = جمع_کړه(function, attr);
        لكل (; begin != پای_ټکی; ++begin)
            added.append_arg(*begin);
        أرجع added;
    }

    فراغ compact_دلیلونه()
    {
        std::ویکټور<صحيح> new_دلیلونه;
        لكل (خپلکاری &غوټه : nodes)
        {
            صحيح new_دننه_واټن = Get_اندازه(new_دلیلونه);
            لكل (صحيح i = 0; i < غوټه.arg_شمېر; i++)
                new_دلیلونه.ټېل_شا(دلیلونه_2[غوټه.arg_دننه_واټن + i]);
            غوټه.arg_دننه_واټن = new_دننه_واټن;
        }
        std::بدلون(دلیلونه_2, new_دلیلونه);
    }

    فراغ ترتیب_بدلول(std::ویکټور<صحيح> ثابت &perm)
    {
        log_assert(perm.اندازه() <= nodes.اندازه());
        std::ویکټور<صحيح> inv_perm;
        inv_perm.resize(nodes.اندازه(), -1);
        لكل (صحيح i = 0; i < Get_اندازه(perm); ++i)
        {
            صحيح j = perm[i];
            log_assert(j >= 0 && j < Get_اندازه(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        ترتیب_بدلول(perm, inv_perm);
    }

    فراغ ترتیب_بدلول(std::ویکټور<صحيح> ثابت &perm, std::ویکټور<صحيح> ثابت &inv_perm)
    {
        log_assert(inv_perm.اندازه() == nodes.اندازه());
        std::ویکټور<غوټه_2> new_nodes;
        new_nodes.reserve(perm.اندازه());
        dict<صحيح, SparseAttr> new_sparse_attrs;
        لكل (صحيح i : perm)
        {
            صحيح j = Get_اندازه(new_nodes);
            new_nodes.emplace_شا(std::move(nodes[i]));
            خپلکاری وموندل = sparse_attrs.پیدا_کړه(i);
            إذا (وموندل != sparse_attrs.پای_ټکی())
                new_sparse_attrs.emplace(j, std::move(وموندل->دوهم));
        }

        std::بدلون(nodes, new_nodes);
        std::بدلون(sparse_attrs, new_sparse_attrs);

        compact_دلیلونه();
        لكل (صحيح &arg : دلیلونه_2)
        {
            log_assert(arg < Get_اندازه(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        لكل (خپلکاری &کلی : کلیګانې)
        {
            log_assert(کلی.دوهم < Get_اندازه(inv_perm));
            log_assert(inv_perm[کلی.دوهم] >= 0);
            کلی.دوهم = inv_perm[کلی.دوهم];
        }
    }

    بنية SccAdaptor
    {
    خاص:
        Compute_ګراف ثابت &ګراف_2;
        std::ویکټور<صحيح> indices_;
    عام:
        SccAdaptor(Compute_ګراف ثابت &ګراف_3) : ګراف_2(ګراف_3)
        {
            indices_.resize(ګراف_3.اندازه(), -1);
        }


        تعريفنوع صحيح غوټه_type;

        بنية غوټه_enumerator {
        خاص:
            ملګری بنية SccAdaptor;
            صحيح current, پای_ټکی;
            غوټه_enumerator(صحيح current, صحيح پای_ټکی) : current(current), پای_ټکی(پای_ټکی) {}

        عام:

            bool پای_ته_رسېدلی() ثابت { أرجع current == پای_ټکی; }
            غوټه_type بل() {
                log_assert(!پای_ته_رسېدلی());
                غوټه_type پایله = current;
                ++current;
                أرجع پایله;
            }
        };

        غوټه_enumerator enumerate_nodes() {
            أرجع غوټه_enumerator(0, Get_اندازه(indices_));
        }


        بنية successor_enumerator {
        خاص:
            ملګری بنية SccAdaptor;
            std::ویکټور<صحيح>::const_iterator current, پای_ټکی;
            successor_enumerator(std::ویکټور<صحيح>::const_iterator current, std::ویکټور<صحيح>::const_iterator پای_ټکی) :
                current(current), پای_ټکی(پای_ټکی) {}

        عام:
            bool پای_ته_رسېدلی() ثابت { أرجع current == پای_ټکی; }
            غوټه_type بل() {
                log_assert(!پای_ته_رسېدلی());
                غوټه_type پایله = *current;
                ++current;
                أرجع پایله;
            }
        };

        successor_enumerator enumerate_ځایناستي(صحيح شاخص) ثابت {
            خپلکاری ثابت &ref = ګراف_2[شاخص];
            أرجع successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        صحيح &dfs_شاخص(غوټه_type ثابت &غوټه) { أرجع indices_[غوټه]; }

        std::ویکټور<صحيح> ثابت &dfs_indices() { أرجع indices_; }
    };

};



YOSYS_NAMESPACE_پای_ټکی


#د_که_پای
