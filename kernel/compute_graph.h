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

#جيڪڏهن_وصف_نه_ٿيل COMPUTE_گراف_H
#وصف_ڪريو COMPUTE_گراف_H

#ضمن <tuple>
#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

سانچو<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename ڪنجي_2 = std::tuple<> // Stable keys to refer to nodes
>
بنية Compute_گراف
{
    بنية Ref;
خاص:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    بنية ڳنڍ_2 {
        صحيح fn_اشاريو;
        صحيح arg_وٿي;
        صحيح arg_ڳڻپ;
        Attr attr;

        ڳنڍ_2(صحيح fn_اشاريو, Attr &&attr, صحيح arg_وٿي, صحيح arg_ڳڻپ = 0)
            : fn_اشاريو(fn_اشاريو), arg_وٿي(arg_وٿي), arg_ڳڻپ(arg_ڳڻپ), attr(std::move(attr)) {}

        ڳنڍ_2(صحيح fn_اشاريو, Attr ثابت &attr, صحيح arg_وٿي, صحيح arg_ڳڻپ = 0)
            : fn_اشاريو(fn_اشاريو), arg_وٿي(arg_وٿي), arg_ڳڻپ(arg_ڳڻپ), attr(attr) {}
    };


    std::ويڪٽر<ڳنڍ_2> nodes;
    std::ويڪٽر<صحيح> دليلون_2;
    dict<ڪنجي_2, صحيح> ڪنجيون;
    dict<صحيح, SparseAttr> sparse_attrs;

عام:
    سانچو<typename گراف>
    بنية بنياد_Ref
    {
    محمي:
        دوست بنية Compute_گراف;
        گراف *گراف_2;
        صحيح اشاريو_2;
        بنياد_Ref(گراف *گراف_3, صحيح اشاريو) : گراف_2(گراف_3), اشاريو_2(اشاريو) {
            log_assert(اشاريو_2 >= 0);
            check();
        }

        فراغ check() ثابت { log_assert(اشاريو_2 < گراف_2->سائيز()); }

        ڳنڍ_2 ثابت &deref() ثابت { check(); أرجع گراف_2->nodes[اشاريو_2]; }

    عام:
        Compute_گراف ثابت &گراف_3() ثابت { أرجع گراف_2; }
        صحيح اشاريو() ثابت { أرجع اشاريو_2; }

        صحيح سائيز() ثابت { أرجع deref().arg_ڳڻپ; }

        بنياد_Ref arg(صحيح n) ثابت
        {
            ڳنڍ_2 ثابت &ڳنڍ = deref();
            log_assert(n >= 0 && n < ڳنڍ.arg_ڳڻپ);
            أرجع بنياد_Ref(گراف_2, گراف_2->دليلون_2[ڳنڍ.arg_وٿي + n]);
        }

        std::ويڪٽر<صحيح>::const_iterator arg_indices_cbegin() ثابت
        {
            ڳنڍ_2 ثابت &ڳنڍ = deref();
            أرجع گراف_2->دليلون_2.cbegin() + ڳنڍ.arg_وٿي;
        }

        std::ويڪٽر<صحيح>::const_iterator arg_indices_cend() ثابت
        {
            ڳنڍ_2 ثابت &ڳنڍ = deref();
            أرجع گراف_2->دليلون_2.cbegin() + ڳنڍ.arg_وٿي + ڳنڍ.arg_ڳڻپ;
        }

        Fn ثابت &function() ثابت { أرجع گراف_2->functions[deref().fn_اشاريو]; }
        Attr ثابت &attr() ثابت { أرجع deref().attr; }

        bool has_sparse_attr() ثابت { أرجع گراف_2->sparse_attrs.ڳڻپ(اشاريو_2); }

        SparseAttr ثابت &sparse_attr() ثابت
        {
            خودڪار مليو = گراف_2->sparse_attrs.ڳوليو(اشاريو_2);
            log_assert(مليو != گراف_2->sparse_attrs.پڇاڙي());
            أرجع مليو->ٻيو;
        }
    };

    using ConstRef = بنياد_Ref<Compute_گراف ثابت>;

    بنية Ref : عام بنياد_Ref<Compute_گراف>
    {
    خاص:
        دوست بنية Compute_گراف;
        Ref(Compute_گراف *گراف_3, صحيح اشاريو) : بنياد_Ref<Compute_گراف>(گراف_3, اشاريو) {}
        ڳنڍ_2 &deref() ثابت { this->check(); أرجع this->گراف_2->nodes[this->اشاريو_2]; }

    عام:
        Ref(بنياد_Ref<Compute_گراف> ref) : Ref(ref.گراف_2, ref.اشاريو_2) {}

        فراغ سيٽ_function(Fn ثابت &function) ثابت
        {
            deref().fn_اشاريو = this->گراف_2->functions(function);
        }

        Attr &attr() ثابت { أرجع deref().attr; }

        فراغ append_arg(ConstRef arg) ثابت
        {
            log_assert(arg.گراف_2 == this->گراف_2);
            append_arg(arg.اشاريو());
        }

        فراغ append_arg(صحيح arg) ثابت
        {
            log_assert(arg >= 0 && arg < this->گراف_2->سائيز());
            ڳنڍ_2 &ڳنڍ = deref();
            إذا (ڳنڍ.arg_وٿي + ڳنڍ.arg_ڳڻپ != Get_سائيز(this->گراف_2->دليلون_2))
                move_دليلون(ڳنڍ);
            this->گراف_2->دليلون_2.ڌڪو_پٺ(arg);
            ڳنڍ.arg_ڳڻپ++;
        }

        عامل ConstRef() ثابت
        {
            أرجع ConstRef(this->گراف_2, this->اشاريو_2);
        }

        SparseAttr &sparse_attr() ثابت
        {
            أرجع this->گراف_2->sparse_attrs[this->اشاريو_2];
        }

        فراغ clear_sparse_attr() ثابت
        {
            this->گراف_2->sparse_attrs.erase(this->اشاريو_2);
        }

        فراغ سونپيو_ڪنجي(ڪنجي_2 ثابت &ڪنجي) ثابت
        {
            this->گراف_2->ڪنجيون.emplace(ڪنجي, this->اشاريو_2);
        }

    خاص:
        فراغ move_دليلون(ڳنڍ_2 &ڳنڍ) ثابت
        {
            خودڪار &دليلون_2 = this->گراف_2->دليلون_2;
            صحيح old_وٿي = ڳنڍ.arg_وٿي;
            ڳنڍ.arg_وٿي = Get_سائيز(دليلون_2);
            لكل (صحيح i = 0; i != ڳنڍ.arg_ڳڻپ; ++i)
                دليلون_2.ڌڪو_پٺ(دليلون_2[old_وٿي + i]);
        }

    };

    bool has_ڪنجي(ڪنجي_2 ثابت &ڪنجي) ثابت
    {
        أرجع ڪنجيون.ڳڻپ(ڪنجي);
    }

    dict<ڪنجي_2, صحيح> ثابت &ڪنجيون_2() ثابت
    {
        أرجع ڪنجيون;
    }

    ConstRef عامل()(ڪنجي_2 ثابت &ڪنجي) ثابت
    {
        خودڪار it = ڪنجيون.ڳوليو(ڪنجي);
        log_assert(it != ڪنجيون.پڇاڙي());
        أرجع (*this)[it->ٻيو];
    }

    Ref عامل()(ڪنجي_2 ثابت &ڪنجي)
    {
        خودڪار it = ڪنجيون.ڳوليو(ڪنجي);
        log_assert(it != ڪنجيون.پڇاڙي());
        أرجع (*this)[it->ٻيو];
    }

    صحيح سائيز() ثابت { أرجع Get_سائيز(nodes); }

    ConstRef عامل[](صحيح اشاريو) ثابت { أرجع ConstRef(this, اشاريو); }
    Ref عامل[](صحيح اشاريو) { أرجع Ref(this, اشاريو); }

    Ref جوڙيو(Fn ثابت &function, Attr &&attr)
    {
        صحيح اشاريو = Get_سائيز(nodes);
        صحيح fn_اشاريو = functions(function);
        nodes.emplace_پٺ(fn_اشاريو, std::move(attr), Get_سائيز(دليلون_2));
        أرجع Ref(this, اشاريو);
    }

    Ref جوڙيو(Fn ثابت &function, Attr ثابت &attr)
    {
        صحيح اشاريو = Get_سائيز(nodes);
        صحيح fn_اشاريو = functions(function);
        nodes.emplace_پٺ(fn_اشاريو, attr,  Get_سائيز(دليلون_2));
        أرجع Ref(this, اشاريو);
    }

    سانچو<typename T>
    Ref جوڙيو(Fn ثابت &function, Attr ثابت &attr, T &&دليلون_2)
    {
        Ref added = جوڙيو(function, attr);
        لكل (خودڪار arg : دليلون_2)
            added.append_arg(arg);
        أرجع added;
    }

    سانچو<typename T>
    Ref جوڙيو(Fn ثابت &function, Attr &&attr, T &&دليلون_2)
    {
        Ref added = جوڙيو(function, std::move(attr));
        لكل (خودڪار arg : دليلون_2)
            added.append_arg(arg);
        أرجع added;
    }

    Ref جوڙيو(Fn ثابت &function, Attr ثابت &attr, std::initializer_list<Ref> دليلون_2)
    {
        Ref added = جوڙيو(function, attr);
        لكل (خودڪار arg : دليلون_2)
            added.append_arg(arg);
        أرجع added;
    }

    Ref جوڙيو(Fn ثابت &function, Attr &&attr, std::initializer_list<Ref> دليلون_2)
    {
        Ref added = جوڙيو(function, std::move(attr));
        لكل (خودڪار arg : دليلون_2)
            added.append_arg(arg);
        أرجع added;
    }

    سانچو<typename T>
    Ref جوڙيو(Fn ثابت &function, Attr ثابت &attr, T begin, T پڇاڙي)
    {
        Ref added = جوڙيو(function, attr);
        لكل (; begin != پڇاڙي; ++begin)
            added.append_arg(*begin);
        أرجع added;
    }

    فراغ compact_دليلون()
    {
        std::ويڪٽر<صحيح> new_دليلون;
        لكل (خودڪار &ڳنڍ : nodes)
        {
            صحيح new_وٿي = Get_سائيز(new_دليلون);
            لكل (صحيح i = 0; i < ڳنڍ.arg_ڳڻپ; i++)
                new_دليلون.ڌڪو_پٺ(دليلون_2[ڳنڍ.arg_وٿي + i]);
            ڳنڍ.arg_وٿي = new_وٿي;
        }
        std::مٽا_سٽا(دليلون_2, new_دليلون);
    }

    فراغ ترتيب_بدلايو(std::ويڪٽر<صحيح> ثابت &perm)
    {
        log_assert(perm.سائيز() <= nodes.سائيز());
        std::ويڪٽر<صحيح> inv_perm;
        inv_perm.resize(nodes.سائيز(), -1);
        لكل (صحيح i = 0; i < Get_سائيز(perm); ++i)
        {
            صحيح j = perm[i];
            log_assert(j >= 0 && j < Get_سائيز(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        ترتيب_بدلايو(perm, inv_perm);
    }

    فراغ ترتيب_بدلايو(std::ويڪٽر<صحيح> ثابت &perm, std::ويڪٽر<صحيح> ثابت &inv_perm)
    {
        log_assert(inv_perm.سائيز() == nodes.سائيز());
        std::ويڪٽر<ڳنڍ_2> new_nodes;
        new_nodes.reserve(perm.سائيز());
        dict<صحيح, SparseAttr> new_sparse_attrs;
        لكل (صحيح i : perm)
        {
            صحيح j = Get_سائيز(new_nodes);
            new_nodes.emplace_پٺ(std::move(nodes[i]));
            خودڪار مليو = sparse_attrs.ڳوليو(i);
            إذا (مليو != sparse_attrs.پڇاڙي())
                new_sparse_attrs.emplace(j, std::move(مليو->ٻيو));
        }

        std::مٽا_سٽا(nodes, new_nodes);
        std::مٽا_سٽا(sparse_attrs, new_sparse_attrs);

        compact_دليلون();
        لكل (صحيح &arg : دليلون_2)
        {
            log_assert(arg < Get_سائيز(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        لكل (خودڪار &ڪنجي : ڪنجيون)
        {
            log_assert(ڪنجي.ٻيو < Get_سائيز(inv_perm));
            log_assert(inv_perm[ڪنجي.ٻيو] >= 0);
            ڪنجي.ٻيو = inv_perm[ڪنجي.ٻيو];
        }
    }

    بنية SccAdaptor
    {
    خاص:
        Compute_گراف ثابت &گراف_2;
        std::ويڪٽر<صحيح> indices_;
    عام:
        SccAdaptor(Compute_گراف ثابت &گراف_3) : گراف_2(گراف_3)
        {
            indices_.resize(گراف_3.سائيز(), -1);
        }


        تعريفنوع صحيح ڳنڍ_type;

        بنية ڳنڍ_enumerator {
        خاص:
            دوست بنية SccAdaptor;
            صحيح current, پڇاڙي;
            ڳنڍ_enumerator(صحيح current, صحيح پڇاڙي) : current(current), پڇاڙي(پڇاڙي) {}

        عام:

            bool ختم() ثابت { أرجع current == پڇاڙي; }
            ڳنڍ_type ايندڙ() {
                log_assert(!ختم());
                ڳنڍ_type نتيجو = current;
                ++current;
                أرجع نتيجو;
            }
        };

        ڳنڍ_enumerator enumerate_nodes() {
            أرجع ڳنڍ_enumerator(0, Get_سائيز(indices_));
        }


        بنية successor_enumerator {
        خاص:
            دوست بنية SccAdaptor;
            std::ويڪٽر<صحيح>::const_iterator current, پڇاڙي;
            successor_enumerator(std::ويڪٽر<صحيح>::const_iterator current, std::ويڪٽر<صحيح>::const_iterator پڇاڙي) :
                current(current), پڇاڙي(پڇاڙي) {}

        عام:
            bool ختم() ثابت { أرجع current == پڇاڙي; }
            ڳنڍ_type ايندڙ() {
                log_assert(!ختم());
                ڳنڍ_type نتيجو = *current;
                ++current;
                أرجع نتيجو;
            }
        };

        successor_enumerator enumerate_جانشين(صحيح اشاريو) ثابت {
            خودڪار ثابت &ref = گراف_2[اشاريو];
            أرجع successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        صحيح &dfs_اشاريو(ڳنڍ_type ثابت &ڳنڍ) { أرجع indices_[ڳنڍ]; }

        std::ويڪٽر<صحيح> ثابت &dfs_indices() { أرجع indices_; }
    };

};



YOSYS_NAMESPACE_پڇاڙي


#جيڪڏهن_پورو
