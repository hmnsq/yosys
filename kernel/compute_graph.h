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

#جے_بنا_تعریف COMPUTE_گراف_H
#تعریف_کرو COMPUTE_گراف_H

#ضمن <tuple>
#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

سانچا<
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

    بنية گرہ_2 {
        صحيح fn_اشاریہ;
        صحيح arg_وتھ;
        صحيح arg_گنتی;
        Attr attr;

        گرہ_2(صحيح fn_اشاریہ, Attr &&attr, صحيح arg_وتھ, صحيح arg_گنتی = 0)
            : fn_اشاریہ(fn_اشاریہ), arg_وتھ(arg_وتھ), arg_گنتی(arg_گنتی), attr(std::move(attr)) {}

        گرہ_2(صحيح fn_اشاریہ, Attr ثابت &attr, صحيح arg_وتھ, صحيح arg_گنتی = 0)
            : fn_اشاریہ(fn_اشاریہ), arg_وتھ(arg_وتھ), arg_گنتی(arg_گنتی), attr(attr) {}
    };


    std::سمتیہ<گرہ_2> nodes;
    std::سمتیہ<صحيح> دلیلاں_2;
    dict<کلید_2, صحيح> کلیدیں;
    dict<صحيح, SparseAttr> sparse_attrs;

عام:
    سانچا<typename گراف>
    بنية ادھار_Ref
    {
    محمي:
        یار بنية Compute_گراف;
        گراف *گراف_2;
        صحيح اشاریہ_2;
        ادھار_Ref(گراف *گراف_3, صحيح اشاریہ) : گراف_2(گراف_3), اشاریہ_2(اشاریہ) {
            log_assert(اشاریہ_2 >= 0);
            check();
        }

        فراغ check() ثابت { log_assert(اشاریہ_2 < گراف_2->ناپ()); }

        گرہ_2 ثابت &deref() ثابت { check(); أرجع گراف_2->nodes[اشاریہ_2]; }

    عام:
        Compute_گراف ثابت &گراف_3() ثابت { أرجع گراف_2; }
        صحيح اشاریہ() ثابت { أرجع اشاریہ_2; }

        صحيح ناپ() ثابت { أرجع deref().arg_گنتی; }

        ادھار_Ref arg(صحيح n) ثابت
        {
            گرہ_2 ثابت &گرہ = deref();
            log_assert(n >= 0 && n < گرہ.arg_گنتی);
            أرجع ادھار_Ref(گراف_2, گراف_2->دلیلاں_2[گرہ.arg_وتھ + n]);
        }

        std::سمتیہ<صحيح>::const_iterator arg_indices_cbegin() ثابت
        {
            گرہ_2 ثابت &گرہ = deref();
            أرجع گراف_2->دلیلاں_2.cbegin() + گرہ.arg_وتھ;
        }

        std::سمتیہ<صحيح>::const_iterator arg_indices_cend() ثابت
        {
            گرہ_2 ثابت &گرہ = deref();
            أرجع گراف_2->دلیلاں_2.cbegin() + گرہ.arg_وتھ + گرہ.arg_گنتی;
        }

        Fn ثابت &function() ثابت { أرجع گراف_2->functions[deref().fn_اشاریہ]; }
        Attr ثابت &attr() ثابت { أرجع deref().attr; }

        bool has_sparse_attr() ثابت { أرجع گراف_2->sparse_attrs.گنتی(اشاریہ_2); }

        SparseAttr ثابت &sparse_attr() ثابت
        {
            خودکار لبھیا = گراف_2->sparse_attrs.لبھو(اشاریہ_2);
            log_assert(لبھیا != گراف_2->sparse_attrs.سرا());
            أرجع لبھیا->دوجا;
        }
    };

    using ConstRef = ادھار_Ref<Compute_گراف ثابت>;

    بنية Ref : عام ادھار_Ref<Compute_گراف>
    {
    خاص:
        یار بنية Compute_گراف;
        Ref(Compute_گراف *گراف_3, صحيح اشاریہ) : ادھار_Ref<Compute_گراف>(گراف_3, اشاریہ) {}
        گرہ_2 &deref() ثابت { this->check(); أرجع this->گراف_2->nodes[this->اشاریہ_2]; }

    عام:
        Ref(ادھار_Ref<Compute_گراف> ref) : Ref(ref.گراف_2, ref.اشاریہ_2) {}

        فراغ سیٹ_function(Fn ثابت &function) ثابت
        {
            deref().fn_اشاریہ = this->گراف_2->functions(function);
        }

        Attr &attr() ثابت { أرجع deref().attr; }

        فراغ append_arg(ConstRef arg) ثابت
        {
            log_assert(arg.گراف_2 == this->گراف_2);
            append_arg(arg.اشاریہ());
        }

        فراغ append_arg(صحيح arg) ثابت
        {
            log_assert(arg >= 0 && arg < this->گراف_2->ناپ());
            گرہ_2 &گرہ = deref();
            إذا (گرہ.arg_وتھ + گرہ.arg_گنتی != Get_ناپ(this->گراف_2->دلیلاں_2))
                move_دلیلاں(گرہ);
            this->گراف_2->دلیلاں_2.ڈالو_پٹھ(arg);
            گرہ.arg_گنتی++;
        }

        عامل ConstRef() ثابت
        {
            أرجع ConstRef(this->گراف_2, this->اشاریہ_2);
        }

        SparseAttr &sparse_attr() ثابت
        {
            أرجع this->گراف_2->sparse_attrs[this->اشاریہ_2];
        }

        فراغ clear_sparse_attr() ثابت
        {
            this->گراف_2->sparse_attrs.erase(this->اشاریہ_2);
        }

        فراغ سونپو_کلید(کلید_2 ثابت &کلید) ثابت
        {
            this->گراف_2->کلیدیں.emplace(کلید, this->اشاریہ_2);
        }

    خاص:
        فراغ move_دلیلاں(گرہ_2 &گرہ) ثابت
        {
            خودکار &دلیلاں_2 = this->گراف_2->دلیلاں_2;
            صحيح old_وتھ = گرہ.arg_وتھ;
            گرہ.arg_وتھ = Get_ناپ(دلیلاں_2);
            لكل (صحيح i = 0; i != گرہ.arg_گنتی; ++i)
                دلیلاں_2.ڈالو_پٹھ(دلیلاں_2[old_وتھ + i]);
        }

    };

    bool has_کلید(کلید_2 ثابت &کلید) ثابت
    {
        أرجع کلیدیں.گنتی(کلید);
    }

    dict<کلید_2, صحيح> ثابت &کلیدیں_2() ثابت
    {
        أرجع کلیدیں;
    }

    ConstRef عامل()(کلید_2 ثابت &کلید) ثابت
    {
        خودکار it = کلیدیں.لبھو(کلید);
        log_assert(it != کلیدیں.سرا());
        أرجع (*this)[it->دوجا];
    }

    Ref عامل()(کلید_2 ثابت &کلید)
    {
        خودکار it = کلیدیں.لبھو(کلید);
        log_assert(it != کلیدیں.سرا());
        أرجع (*this)[it->دوجا];
    }

    صحيح ناپ() ثابت { أرجع Get_ناپ(nodes); }

    ConstRef عامل[](صحيح اشاریہ) ثابت { أرجع ConstRef(this, اشاریہ); }
    Ref عامل[](صحيح اشاریہ) { أرجع Ref(this, اشاریہ); }

    Ref جوڑو(Fn ثابت &function, Attr &&attr)
    {
        صحيح اشاریہ = Get_ناپ(nodes);
        صحيح fn_اشاریہ = functions(function);
        nodes.emplace_پٹھ(fn_اشاریہ, std::move(attr), Get_ناپ(دلیلاں_2));
        أرجع Ref(this, اشاریہ);
    }

    Ref جوڑو(Fn ثابت &function, Attr ثابت &attr)
    {
        صحيح اشاریہ = Get_ناپ(nodes);
        صحيح fn_اشاریہ = functions(function);
        nodes.emplace_پٹھ(fn_اشاریہ, attr,  Get_ناپ(دلیلاں_2));
        أرجع Ref(this, اشاریہ);
    }

    سانچا<typename T>
    Ref جوڑو(Fn ثابت &function, Attr ثابت &attr, T &&دلیلاں_2)
    {
        Ref added = جوڑو(function, attr);
        لكل (خودکار arg : دلیلاں_2)
            added.append_arg(arg);
        أرجع added;
    }

    سانچا<typename T>
    Ref جوڑو(Fn ثابت &function, Attr &&attr, T &&دلیلاں_2)
    {
        Ref added = جوڑو(function, std::move(attr));
        لكل (خودکار arg : دلیلاں_2)
            added.append_arg(arg);
        أرجع added;
    }

    Ref جوڑو(Fn ثابت &function, Attr ثابت &attr, std::initializer_list<Ref> دلیلاں_2)
    {
        Ref added = جوڑو(function, attr);
        لكل (خودکار arg : دلیلاں_2)
            added.append_arg(arg);
        أرجع added;
    }

    Ref جوڑو(Fn ثابت &function, Attr &&attr, std::initializer_list<Ref> دلیلاں_2)
    {
        Ref added = جوڑو(function, std::move(attr));
        لكل (خودکار arg : دلیلاں_2)
            added.append_arg(arg);
        أرجع added;
    }

    سانچا<typename T>
    Ref جوڑو(Fn ثابت &function, Attr ثابت &attr, T begin, T سرا)
    {
        Ref added = جوڑو(function, attr);
        لكل (; begin != سرا; ++begin)
            added.append_arg(*begin);
        أرجع added;
    }

    فراغ compact_دلیلاں()
    {
        std::سمتیہ<صحيح> new_دلیلاں;
        لكل (خودکار &گرہ : nodes)
        {
            صحيح new_وتھ = Get_ناپ(new_دلیلاں);
            لكل (صحيح i = 0; i < گرہ.arg_گنتی; i++)
                new_دلیلاں.ڈالو_پٹھ(دلیلاں_2[گرہ.arg_وتھ + i]);
            گرہ.arg_وتھ = new_وتھ;
        }
        std::وٹاندرا(دلیلاں_2, new_دلیلاں);
    }

    فراغ ترتیب_بدلو(std::سمتیہ<صحيح> ثابت &perm)
    {
        log_assert(perm.ناپ() <= nodes.ناپ());
        std::سمتیہ<صحيح> inv_perm;
        inv_perm.resize(nodes.ناپ(), -1);
        لكل (صحيح i = 0; i < Get_ناپ(perm); ++i)
        {
            صحيح j = perm[i];
            log_assert(j >= 0 && j < Get_ناپ(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        ترتیب_بدلو(perm, inv_perm);
    }

    فراغ ترتیب_بدلو(std::سمتیہ<صحيح> ثابت &perm, std::سمتیہ<صحيح> ثابت &inv_perm)
    {
        log_assert(inv_perm.ناپ() == nodes.ناپ());
        std::سمتیہ<گرہ_2> new_nodes;
        new_nodes.reserve(perm.ناپ());
        dict<صحيح, SparseAttr> new_sparse_attrs;
        لكل (صحيح i : perm)
        {
            صحيح j = Get_ناپ(new_nodes);
            new_nodes.emplace_پٹھ(std::move(nodes[i]));
            خودکار لبھیا = sparse_attrs.لبھو(i);
            إذا (لبھیا != sparse_attrs.سرا())
                new_sparse_attrs.emplace(j, std::move(لبھیا->دوجا));
        }

        std::وٹاندرا(nodes, new_nodes);
        std::وٹاندرا(sparse_attrs, new_sparse_attrs);

        compact_دلیلاں();
        لكل (صحيح &arg : دلیلاں_2)
        {
            log_assert(arg < Get_ناپ(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        لكل (خودکار &کلید : کلیدیں)
        {
            log_assert(کلید.دوجا < Get_ناپ(inv_perm));
            log_assert(inv_perm[کلید.دوجا] >= 0);
            کلید.دوجا = inv_perm[کلید.دوجا];
        }
    }

    بنية SccAdaptor
    {
    خاص:
        Compute_گراف ثابت &گراف_2;
        std::سمتیہ<صحيح> indices_;
    عام:
        SccAdaptor(Compute_گراف ثابت &گراف_3) : گراف_2(گراف_3)
        {
            indices_.resize(گراف_3.ناپ(), -1);
        }


        تعريفنوع صحيح گرہ_type;

        بنية گرہ_enumerator {
        خاص:
            یار بنية SccAdaptor;
            صحيح current, سرا;
            گرہ_enumerator(صحيح current, صحيح سرا) : current(current), سرا(سرا) {}

        عام:

            bool مکمل() ثابت { أرجع current == سرا; }
            گرہ_type اگلا() {
                log_assert(!مکمل());
                گرہ_type نتیجہ = current;
                ++current;
                أرجع نتیجہ;
            }
        };

        گرہ_enumerator enumerate_nodes() {
            أرجع گرہ_enumerator(0, Get_ناپ(indices_));
        }


        بنية successor_enumerator {
        خاص:
            یار بنية SccAdaptor;
            std::سمتیہ<صحيح>::const_iterator current, سرا;
            successor_enumerator(std::سمتیہ<صحيح>::const_iterator current, std::سمتیہ<صحيح>::const_iterator سرا) :
                current(current), سرا(سرا) {}

        عام:
            bool مکمل() ثابت { أرجع current == سرا; }
            گرہ_type اگلا() {
                log_assert(!مکمل());
                گرہ_type نتیجہ = *current;
                ++current;
                أرجع نتیجہ;
            }
        };

        successor_enumerator enumerate_جانشین(صحيح اشاریہ) ثابت {
            خودکار ثابت &ref = گراف_2[اشاریہ];
            أرجع successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        صحيح &dfs_اشاریہ(گرہ_type ثابت &گرہ) { أرجع indices_[گرہ]; }

        std::سمتیہ<صحيح> ثابت &dfs_indices() { أرجع indices_; }
    };

};



YOSYS_NAMESPACE_سرا


#جے_انت
