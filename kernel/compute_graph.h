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

#اگر_غیر_تعریف_شدہ COMPUTE_گراف_H
#تعریف_کرو COMPUTE_گراف_H

#شامل <tuple>
#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

سانچہ<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename کلید_2 = std::tuple<> // Stable keys to refer to nodes
>
ساخت Compute_گراف
{
    ساخت Ref;
نجی:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    ساخت گرہ_2 {
        صحیح fn_اشاریہ;
        صحیح arg_انحراف;
        صحیح arg_گنتی;
        Attr attr;

        گرہ_2(صحیح fn_اشاریہ, Attr &&attr, صحیح arg_انحراف, صحیح arg_گنتی = 0)
            : fn_اشاریہ(fn_اشاریہ), arg_انحراف(arg_انحراف), arg_گنتی(arg_گنتی), attr(std::move(attr)) {}

        گرہ_2(صحیح fn_اشاریہ, Attr ثابت &attr, صحیح arg_انحراف, صحیح arg_گنتی = 0)
            : fn_اشاریہ(fn_اشاریہ), arg_انحراف(arg_انحراف), arg_گنتی(arg_گنتی), attr(attr) {}
    };


    std::سمتیہ<گرہ_2> nodes;
    std::سمتیہ<صحیح> دلائل_2;
    dict<کلید_2, صحیح> کلیدیں;
    dict<صحیح, SparseAttr> sparse_attrs;

عوامی:
    سانچہ<typename گراف>
    ساخت بنیاد_Ref
    {
    محفوظ:
        دوست ساخت Compute_گراف;
        گراف *گراف_2;
        صحیح اشاریہ_2;
        بنیاد_Ref(گراف *گراف_3, صحیح اشاریہ) : گراف_2(گراف_3), اشاریہ_2(اشاریہ) {
            log_assert(اشاریہ_2 >= 0);
            check();
        }

        خالی check() ثابت { log_assert(اشاریہ_2 < گراف_2->سائز()); }

        گرہ_2 ثابت &deref() ثابت { check(); واپس گراف_2->nodes[اشاریہ_2]; }

    عوامی:
        Compute_گراف ثابت &گراف_3() ثابت { واپس گراف_2; }
        صحیح اشاریہ() ثابت { واپس اشاریہ_2; }

        صحیح سائز() ثابت { واپس deref().arg_گنتی; }

        بنیاد_Ref arg(صحیح n) ثابت
        {
            گرہ_2 ثابت &گرہ = deref();
            log_assert(n >= 0 && n < گرہ.arg_گنتی);
            واپس بنیاد_Ref(گراف_2, گراف_2->دلائل_2[گرہ.arg_انحراف + n]);
        }

        std::سمتیہ<صحیح>::const_iterator arg_indices_cbegin() ثابت
        {
            گرہ_2 ثابت &گرہ = deref();
            واپس گراف_2->دلائل_2.cbegin() + گرہ.arg_انحراف;
        }

        std::سمتیہ<صحیح>::const_iterator arg_indices_cend() ثابت
        {
            گرہ_2 ثابت &گرہ = deref();
            واپس گراف_2->دلائل_2.cbegin() + گرہ.arg_انحراف + گرہ.arg_گنتی;
        }

        Fn ثابت &function() ثابت { واپس گراف_2->functions[deref().fn_اشاریہ]; }
        Attr ثابت &attr() ثابت { واپس deref().attr; }

        bool has_sparse_attr() ثابت { واپس گراف_2->sparse_attrs.گنتی(اشاریہ_2); }

        SparseAttr ثابت &sparse_attr() ثابت
        {
            خودکار ملا = گراف_2->sparse_attrs.ڈھونڈو(اشاریہ_2);
            log_assert(ملا != گراف_2->sparse_attrs.انتہا());
            واپس ملا->دوسرا;
        }
    };

    using ConstRef = بنیاد_Ref<Compute_گراف ثابت>;

    ساخت Ref : عوامی بنیاد_Ref<Compute_گراف>
    {
    نجی:
        دوست ساخت Compute_گراف;
        Ref(Compute_گراف *گراف_3, صحیح اشاریہ) : بنیاد_Ref<Compute_گراف>(گراف_3, اشاریہ) {}
        گرہ_2 &deref() ثابت { this->check(); واپس this->گراف_2->nodes[this->اشاریہ_2]; }

    عوامی:
        Ref(بنیاد_Ref<Compute_گراف> ref) : Ref(ref.گراف_2, ref.اشاریہ_2) {}

        خالی سیٹ_function(Fn ثابت &function) ثابت
        {
            deref().fn_اشاریہ = this->گراف_2->functions(function);
        }

        Attr &attr() ثابت { واپس deref().attr; }

        خالی append_arg(ConstRef arg) ثابت
        {
            log_assert(arg.گراف_2 == this->گراف_2);
            append_arg(arg.اشاریہ());
        }

        خالی append_arg(صحیح arg) ثابت
        {
            log_assert(arg >= 0 && arg < this->گراف_2->سائز());
            گرہ_2 &گرہ = deref();
            اگر (گرہ.arg_انحراف + گرہ.arg_گنتی != Get_سائز(this->گراف_2->دلائل_2))
                move_دلائل(گرہ);
            this->گراف_2->دلائل_2.ڈالو_عقب(arg);
            گرہ.arg_گنتی++;
        }

        عامل ConstRef() ثابت
        {
            واپس ConstRef(this->گراف_2, this->اشاریہ_2);
        }

        SparseAttr &sparse_attr() ثابت
        {
            واپس this->گراف_2->sparse_attrs[this->اشاریہ_2];
        }

        خالی clear_sparse_attr() ثابت
        {
            this->گراف_2->sparse_attrs.erase(this->اشاریہ_2);
        }

        خالی تفویض_کلید(کلید_2 ثابت &کلید) ثابت
        {
            this->گراف_2->کلیدیں.emplace(کلید, this->اشاریہ_2);
        }

    نجی:
        خالی move_دلائل(گرہ_2 &گرہ) ثابت
        {
            خودکار &دلائل_2 = this->گراف_2->دلائل_2;
            صحیح old_انحراف = گرہ.arg_انحراف;
            گرہ.arg_انحراف = Get_سائز(دلائل_2);
            برائے (صحیح i = 0; i != گرہ.arg_گنتی; ++i)
                دلائل_2.ڈالو_عقب(دلائل_2[old_انحراف + i]);
        }

    };

    bool has_کلید(کلید_2 ثابت &کلید) ثابت
    {
        واپس کلیدیں.گنتی(کلید);
    }

    dict<کلید_2, صحیح> ثابت &کلیدیں_2() ثابت
    {
        واپس کلیدیں;
    }

    ConstRef عامل()(کلید_2 ثابت &کلید) ثابت
    {
        خودکار it = کلیدیں.ڈھونڈو(کلید);
        log_assert(it != کلیدیں.انتہا());
        واپس (*this)[it->دوسرا];
    }

    Ref عامل()(کلید_2 ثابت &کلید)
    {
        خودکار it = کلیدیں.ڈھونڈو(کلید);
        log_assert(it != کلیدیں.انتہا());
        واپس (*this)[it->دوسرا];
    }

    صحیح سائز() ثابت { واپس Get_سائز(nodes); }

    ConstRef عامل[](صحیح اشاریہ) ثابت { واپس ConstRef(this, اشاریہ); }
    Ref عامل[](صحیح اشاریہ) { واپس Ref(this, اشاریہ); }

    Ref جوڑو(Fn ثابت &function, Attr &&attr)
    {
        صحیح اشاریہ = Get_سائز(nodes);
        صحیح fn_اشاریہ = functions(function);
        nodes.emplace_عقب(fn_اشاریہ, std::move(attr), Get_سائز(دلائل_2));
        واپس Ref(this, اشاریہ);
    }

    Ref جوڑو(Fn ثابت &function, Attr ثابت &attr)
    {
        صحیح اشاریہ = Get_سائز(nodes);
        صحیح fn_اشاریہ = functions(function);
        nodes.emplace_عقب(fn_اشاریہ, attr,  Get_سائز(دلائل_2));
        واپس Ref(this, اشاریہ);
    }

    سانچہ<typename T>
    Ref جوڑو(Fn ثابت &function, Attr ثابت &attr, T &&دلائل_2)
    {
        Ref added = جوڑو(function, attr);
        برائے (خودکار arg : دلائل_2)
            added.append_arg(arg);
        واپس added;
    }

    سانچہ<typename T>
    Ref جوڑو(Fn ثابت &function, Attr &&attr, T &&دلائل_2)
    {
        Ref added = جوڑو(function, std::move(attr));
        برائے (خودکار arg : دلائل_2)
            added.append_arg(arg);
        واپس added;
    }

    Ref جوڑو(Fn ثابت &function, Attr ثابت &attr, std::initializer_list<Ref> دلائل_2)
    {
        Ref added = جوڑو(function, attr);
        برائے (خودکار arg : دلائل_2)
            added.append_arg(arg);
        واپس added;
    }

    Ref جوڑو(Fn ثابت &function, Attr &&attr, std::initializer_list<Ref> دلائل_2)
    {
        Ref added = جوڑو(function, std::move(attr));
        برائے (خودکار arg : دلائل_2)
            added.append_arg(arg);
        واپس added;
    }

    سانچہ<typename T>
    Ref جوڑو(Fn ثابت &function, Attr ثابت &attr, T begin, T انتہا)
    {
        Ref added = جوڑو(function, attr);
        برائے (; begin != انتہا; ++begin)
            added.append_arg(*begin);
        واپس added;
    }

    خالی compact_دلائل()
    {
        std::سمتیہ<صحیح> new_دلائل;
        برائے (خودکار &گرہ : nodes)
        {
            صحیح new_انحراف = Get_سائز(new_دلائل);
            برائے (صحیح i = 0; i < گرہ.arg_گنتی; i++)
                new_دلائل.ڈالو_عقب(دلائل_2[گرہ.arg_انحراف + i]);
            گرہ.arg_انحراف = new_انحراف;
        }
        std::تبادلہ(دلائل_2, new_دلائل);
    }

    خالی ترتیب_بدلو(std::سمتیہ<صحیح> ثابت &perm)
    {
        log_assert(perm.سائز() <= nodes.سائز());
        std::سمتیہ<صحیح> inv_perm;
        inv_perm.resize(nodes.سائز(), -1);
        برائے (صحیح i = 0; i < Get_سائز(perm); ++i)
        {
            صحیح j = perm[i];
            log_assert(j >= 0 && j < Get_سائز(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        ترتیب_بدلو(perm, inv_perm);
    }

    خالی ترتیب_بدلو(std::سمتیہ<صحیح> ثابت &perm, std::سمتیہ<صحیح> ثابت &inv_perm)
    {
        log_assert(inv_perm.سائز() == nodes.سائز());
        std::سمتیہ<گرہ_2> new_nodes;
        new_nodes.reserve(perm.سائز());
        dict<صحیح, SparseAttr> new_sparse_attrs;
        برائے (صحیح i : perm)
        {
            صحیح j = Get_سائز(new_nodes);
            new_nodes.emplace_عقب(std::move(nodes[i]));
            خودکار ملا = sparse_attrs.ڈھونڈو(i);
            اگر (ملا != sparse_attrs.انتہا())
                new_sparse_attrs.emplace(j, std::move(ملا->دوسرا));
        }

        std::تبادلہ(nodes, new_nodes);
        std::تبادلہ(sparse_attrs, new_sparse_attrs);

        compact_دلائل();
        برائے (صحیح &arg : دلائل_2)
        {
            log_assert(arg < Get_سائز(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        برائے (خودکار &کلید : کلیدیں)
        {
            log_assert(کلید.دوسرا < Get_سائز(inv_perm));
            log_assert(inv_perm[کلید.دوسرا] >= 0);
            کلید.دوسرا = inv_perm[کلید.دوسرا];
        }
    }

    ساخت SccAdaptor
    {
    نجی:
        Compute_گراف ثابت &گراف_2;
        std::سمتیہ<صحیح> indices_;
    عوامی:
        SccAdaptor(Compute_گراف ثابت &گراف_3) : گراف_2(گراف_3)
        {
            indices_.resize(گراف_3.سائز(), -1);
        }


        قسمتعریف صحیح گرہ_type;

        ساخت گرہ_enumerator {
        نجی:
            دوست ساخت SccAdaptor;
            صحیح current, انتہا;
            گرہ_enumerator(صحیح current, صحیح انتہا) : current(current), انتہا(انتہا) {}

        عوامی:

            bool ختم_شدہ() ثابت { واپس current == انتہا; }
            گرہ_type اگلا() {
                log_assert(!ختم_شدہ());
                گرہ_type نتیجہ = current;
                ++current;
                واپس نتیجہ;
            }
        };

        گرہ_enumerator enumerate_nodes() {
            واپس گرہ_enumerator(0, Get_سائز(indices_));
        }


        ساخت successor_enumerator {
        نجی:
            دوست ساخت SccAdaptor;
            std::سمتیہ<صحیح>::const_iterator current, انتہا;
            successor_enumerator(std::سمتیہ<صحیح>::const_iterator current, std::سمتیہ<صحیح>::const_iterator انتہا) :
                current(current), انتہا(انتہا) {}

        عوامی:
            bool ختم_شدہ() ثابت { واپس current == انتہا; }
            گرہ_type اگلا() {
                log_assert(!ختم_شدہ());
                گرہ_type نتیجہ = *current;
                ++current;
                واپس نتیجہ;
            }
        };

        successor_enumerator enumerate_جانشین(صحیح اشاریہ) ثابت {
            خودکار ثابت &ref = گراف_2[اشاریہ];
            واپس successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        صحیح &dfs_اشاریہ(گرہ_type ثابت &گرہ) { واپس indices_[گرہ]; }

        std::سمتیہ<صحیح> ثابت &dfs_indices() { واپس indices_; }
    };

};



YOSYS_NAMESPACE_انتہا


#اگر_ختم
