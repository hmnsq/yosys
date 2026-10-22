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

#నిర్వచితం_కాకపోతే COMPUTE_లేఖాచిత్రం_H
#నిర్వచించు COMPUTE_లేఖాచిత్రం_H

#అన్తర్భావ <tuple>
#అన్తర్భావ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

మూస<
    typename Fn, // Function type (deduplicated across whole graph)
    typename Attr = std::tuple<>, // Call attributes (present in every node)
    typename SparseAttr = std::tuple<>, // Sparse call attributes (optional per node)
    typename కీలకం_2 = std::tuple<> // Stable keys to refer to nodes
>
నిర్మాణం Compute_లేఖాచిత్రం
{
    నిర్మాణం Ref;
అంతరంగ:

    // Functions are deduplicated by assigning unique ids
    idict<Fn> functions;

    నిర్మాణం కణుపు_2 {
        పూర్ణాంకం fn_సూచీ;
        పూర్ణాంకం arg_స్థానభ్రంశం;
        పూర్ణాంకం arg_లెక్క;
        Attr attr;

        కణుపు_2(పూర్ణాంకం fn_సూచీ, Attr &&attr, పూర్ణాంకం arg_స్థానభ్రంశం, పూర్ణాంకం arg_లెక్క = 0)
            : fn_సూచీ(fn_సూచీ), arg_స్థానభ్రంశం(arg_స్థానభ్రంశం), arg_లెక్క(arg_లెక్క), attr(std::move(attr)) {}

        కణుపు_2(పూర్ణాంకం fn_సూచీ, Attr స్థిరరాశి &attr, పూర్ణాంకం arg_స్థానభ్రంశం, పూర్ణాంకం arg_లెక్క = 0)
            : fn_సూచీ(fn_సూచీ), arg_స్థానభ్రంశం(arg_స్థానభ్రంశం), arg_లెక్క(arg_లెక్క), attr(attr) {}
    };


    std::సదిశ<కణుపు_2> nodes;
    std::సదిశ<పూర్ణాంకం> వాదనలు_2;
    dict<కీలకం_2, పూర్ణాంకం> కీలకాలు;
    dict<పూర్ణాంకం, SparseAttr> sparse_attrs;

బహిరంగ:
    మూస<typename లేఖాచిత్రం>
    నిర్మాణం పునాది_Ref
    {
    రక్షిత:
        మిత్ర నిర్మాణం Compute_లేఖాచిత్రం;
        లేఖాచిత్రం *లేఖాచిత్రం_2;
        పూర్ణాంకం సూచీ_2;
        పునాది_Ref(లేఖాచిత్రం *లేఖాచిత్రం_3, పూర్ణాంకం సూచీ) : లేఖాచిత్రం_2(లేఖాచిత్రం_3), సూచీ_2(సూచీ) {
            log_assert(సూచీ_2 >= 0);
            check();
        }

        శూన్య check() స్థిరరాశి { log_assert(సూచీ_2 < లేఖాచిత్రం_2->కొలత()); }

        కణుపు_2 స్థిరరాశి &deref() స్థిరరాశి { check(); తిరిగి లేఖాచిత్రం_2->nodes[సూచీ_2]; }

    బహిరంగ:
        Compute_లేఖాచిత్రం స్థిరరాశి &లేఖాచిత్రం_3() స్థిరరాశి { తిరిగి లేఖాచిత్రం_2; }
        పూర్ణాంకం సూచీ() స్థిరరాశి { తిరిగి సూచీ_2; }

        పూర్ణాంకం కొలత() స్థిరరాశి { తిరిగి deref().arg_లెక్క; }

        పునాది_Ref arg(పూర్ణాంకం n) స్థిరరాశి
        {
            కణుపు_2 స్థిరరాశి &కణుపు = deref();
            log_assert(n >= 0 && n < కణుపు.arg_లెక్క);
            తిరిగి పునాది_Ref(లేఖాచిత్రం_2, లేఖాచిత్రం_2->వాదనలు_2[కణుపు.arg_స్థానభ్రంశం + n]);
        }

        std::సదిశ<పూర్ణాంకం>::const_iterator arg_indices_cbegin() స్థిరరాశి
        {
            కణుపు_2 స్థిరరాశి &కణుపు = deref();
            తిరిగి లేఖాచిత్రం_2->వాదనలు_2.cbegin() + కణుపు.arg_స్థానభ్రంశం;
        }

        std::సదిశ<పూర్ణాంకం>::const_iterator arg_indices_cend() స్థిరరాశి
        {
            కణుపు_2 స్థిరరాశి &కణుపు = deref();
            తిరిగి లేఖాచిత్రం_2->వాదనలు_2.cbegin() + కణుపు.arg_స్థానభ్రంశం + కణుపు.arg_లెక్క;
        }

        Fn స్థిరరాశి &function() స్థిరరాశి { తిరిగి లేఖాచిత్రం_2->functions[deref().fn_సూచీ]; }
        Attr స్థిరరాశి &attr() స్థిరరాశి { తిరిగి deref().attr; }

        bool has_sparse_attr() స్థిరరాశి { తిరిగి లేఖాచిత్రం_2->sparse_attrs.లెక్క(సూచీ_2); }

        SparseAttr స్థిరరాశి &sparse_attr() స్థిరరాశి
        {
            స్వయంచాలక దొరికింది = లేఖాచిత్రం_2->sparse_attrs.కనుగొను(సూచీ_2);
            log_assert(దొరికింది != లేఖాచిత్రం_2->sparse_attrs.చివర());
            తిరిగి దొరికింది->రెండవ;
        }
    };

    using ConstRef = పునాది_Ref<Compute_లేఖాచిత్రం స్థిరరాశి>;

    నిర్మాణం Ref : బహిరంగ పునాది_Ref<Compute_లేఖాచిత్రం>
    {
    అంతరంగ:
        మిత్ర నిర్మాణం Compute_లేఖాచిత్రం;
        Ref(Compute_లేఖాచిత్రం *లేఖాచిత్రం_3, పూర్ణాంకం సూచీ) : పునాది_Ref<Compute_లేఖాచిత్రం>(లేఖాచిత్రం_3, సూచీ) {}
        కణుపు_2 &deref() స్థిరరాశి { this->check(); తిరిగి this->లేఖాచిత్రం_2->nodes[this->సూచీ_2]; }

    బహిరంగ:
        Ref(పునాది_Ref<Compute_లేఖాచిత్రం> ref) : Ref(ref.లేఖాచిత్రం_2, ref.సూచీ_2) {}

        శూన్య సమితి_function(Fn స్థిరరాశి &function) స్థిరరాశి
        {
            deref().fn_సూచీ = this->లేఖాచిత్రం_2->functions(function);
        }

        Attr &attr() స్థిరరాశి { తిరిగి deref().attr; }

        శూన్య append_arg(ConstRef arg) స్థిరరాశి
        {
            log_assert(arg.లేఖాచిత్రం_2 == this->లేఖాచిత్రం_2);
            append_arg(arg.సూచీ());
        }

        శూన్య append_arg(పూర్ణాంకం arg) స్థిరరాశి
        {
            log_assert(arg >= 0 && arg < this->లేఖాచిత్రం_2->కొలత());
            కణుపు_2 &కణుపు = deref();
            ఒకవేళ (కణుపు.arg_స్థానభ్రంశం + కణుపు.arg_లెక్క != Get_కొలత(this->లేఖాచిత్రం_2->వాదనలు_2))
                move_వాదనలు(కణుపు);
            this->లేఖాచిత్రం_2->వాదనలు_2.నెట్టు_వెనుక(arg);
            కణుపు.arg_లెక్క++;
        }

        క్రియాకారకం ConstRef() స్థిరరాశి
        {
            తిరిగి ConstRef(this->లేఖాచిత్రం_2, this->సూచీ_2);
        }

        SparseAttr &sparse_attr() స్థిరరాశి
        {
            తిరిగి this->లేఖాచిత్రం_2->sparse_attrs[this->సూచీ_2];
        }

        శూన్య clear_sparse_attr() స్థిరరాశి
        {
            this->లేఖాచిత్రం_2->sparse_attrs.erase(this->సూచీ_2);
        }

        శూన్య కేటాయించు_కీలకం(కీలకం_2 స్థిరరాశి &కీలకం) స్థిరరాశి
        {
            this->లేఖాచిత్రం_2->కీలకాలు.emplace(కీలకం, this->సూచీ_2);
        }

    అంతరంగ:
        శూన్య move_వాదనలు(కణుపు_2 &కణుపు) స్థిరరాశి
        {
            స్వయంచాలక &వాదనలు_2 = this->లేఖాచిత్రం_2->వాదనలు_2;
            పూర్ణాంకం old_స్థానభ్రంశం = కణుపు.arg_స్థానభ్రంశం;
            కణుపు.arg_స్థానభ్రంశం = Get_కొలత(వాదనలు_2);
            కొరకు (పూర్ణాంకం i = 0; i != కణుపు.arg_లెక్క; ++i)
                వాదనలు_2.నెట్టు_వెనుక(వాదనలు_2[old_స్థానభ్రంశం + i]);
        }

    };

    bool has_కీలకం(కీలకం_2 స్థిరరాశి &కీలకం) స్థిరరాశి
    {
        తిరిగి కీలకాలు.లెక్క(కీలకం);
    }

    dict<కీలకం_2, పూర్ణాంకం> స్థిరరాశి &కీలకాలు_2() స్థిరరాశి
    {
        తిరిగి కీలకాలు;
    }

    ConstRef క్రియాకారకం()(కీలకం_2 స్థిరరాశి &కీలకం) స్థిరరాశి
    {
        స్వయంచాలక it = కీలకాలు.కనుగొను(కీలకం);
        log_assert(it != కీలకాలు.చివర());
        తిరిగి (*this)[it->రెండవ];
    }

    Ref క్రియాకారకం()(కీలకం_2 స్థిరరాశి &కీలకం)
    {
        స్వయంచాలక it = కీలకాలు.కనుగొను(కీలకం);
        log_assert(it != కీలకాలు.చివర());
        తిరిగి (*this)[it->రెండవ];
    }

    పూర్ణాంకం కొలత() స్థిరరాశి { తిరిగి Get_కొలత(nodes); }

    ConstRef క్రియాకారకం[](పూర్ణాంకం సూచీ) స్థిరరాశి { తిరిగి ConstRef(this, సూచీ); }
    Ref క్రియాకారకం[](పూర్ణాంకం సూచీ) { తిరిగి Ref(this, సూచీ); }

    Ref కూడు(Fn స్థిరరాశి &function, Attr &&attr)
    {
        పూర్ణాంకం సూచీ = Get_కొలత(nodes);
        పూర్ణాంకం fn_సూచీ = functions(function);
        nodes.emplace_వెనుక(fn_సూచీ, std::move(attr), Get_కొలత(వాదనలు_2));
        తిరిగి Ref(this, సూచీ);
    }

    Ref కూడు(Fn స్థిరరాశి &function, Attr స్థిరరాశి &attr)
    {
        పూర్ణాంకం సూచీ = Get_కొలత(nodes);
        పూర్ణాంకం fn_సూచీ = functions(function);
        nodes.emplace_వెనుక(fn_సూచీ, attr,  Get_కొలత(వాదనలు_2));
        తిరిగి Ref(this, సూచీ);
    }

    మూస<typename T>
    Ref కూడు(Fn స్థిరరాశి &function, Attr స్థిరరాశి &attr, T &&వాదనలు_2)
    {
        Ref added = కూడు(function, attr);
        కొరకు (స్వయంచాలక arg : వాదనలు_2)
            added.append_arg(arg);
        తిరిగి added;
    }

    మూస<typename T>
    Ref కూడు(Fn స్థిరరాశి &function, Attr &&attr, T &&వాదనలు_2)
    {
        Ref added = కూడు(function, std::move(attr));
        కొరకు (స్వయంచాలక arg : వాదనలు_2)
            added.append_arg(arg);
        తిరిగి added;
    }

    Ref కూడు(Fn స్థిరరాశి &function, Attr స్థిరరాశి &attr, std::initializer_list<Ref> వాదనలు_2)
    {
        Ref added = కూడు(function, attr);
        కొరకు (స్వయంచాలక arg : వాదనలు_2)
            added.append_arg(arg);
        తిరిగి added;
    }

    Ref కూడు(Fn స్థిరరాశి &function, Attr &&attr, std::initializer_list<Ref> వాదనలు_2)
    {
        Ref added = కూడు(function, std::move(attr));
        కొరకు (స్వయంచాలక arg : వాదనలు_2)
            added.append_arg(arg);
        తిరిగి added;
    }

    మూస<typename T>
    Ref కూడు(Fn స్థిరరాశి &function, Attr స్థిరరాశి &attr, T begin, T చివర)
    {
        Ref added = కూడు(function, attr);
        కొరకు (; begin != చివర; ++begin)
            added.append_arg(*begin);
        తిరిగి added;
    }

    శూన్య compact_వాదనలు()
    {
        std::సదిశ<పూర్ణాంకం> new_వాదనలు;
        కొరకు (స్వయంచాలక &కణుపు : nodes)
        {
            పూర్ణాంకం new_స్థానభ్రంశం = Get_కొలత(new_వాదనలు);
            కొరకు (పూర్ణాంకం i = 0; i < కణుపు.arg_లెక్క; i++)
                new_వాదనలు.నెట్టు_వెనుక(వాదనలు_2[కణుపు.arg_స్థానభ్రంశం + i]);
            కణుపు.arg_స్థానభ్రంశం = new_స్థానభ్రంశం;
        }
        std::మార్పిడి(వాదనలు_2, new_వాదనలు);
    }

    శూన్య క్రమమార్పు(std::సదిశ<పూర్ణాంకం> స్థిరరాశి &perm)
    {
        log_assert(perm.కొలత() <= nodes.కొలత());
        std::సదిశ<పూర్ణాంకం> inv_perm;
        inv_perm.resize(nodes.కొలత(), -1);
        కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(perm); ++i)
        {
            పూర్ణాంకం j = perm[i];
            log_assert(j >= 0 && j < Get_కొలత(nodes));
            log_assert(inv_perm[j] == -1);
            inv_perm[j] = i;
        }
        క్రమమార్పు(perm, inv_perm);
    }

    శూన్య క్రమమార్పు(std::సదిశ<పూర్ణాంకం> స్థిరరాశి &perm, std::సదిశ<పూర్ణాంకం> స్థిరరాశి &inv_perm)
    {
        log_assert(inv_perm.కొలత() == nodes.కొలత());
        std::సదిశ<కణుపు_2> new_nodes;
        new_nodes.reserve(perm.కొలత());
        dict<పూర్ణాంకం, SparseAttr> new_sparse_attrs;
        కొరకు (పూర్ణాంకం i : perm)
        {
            పూర్ణాంకం j = Get_కొలత(new_nodes);
            new_nodes.emplace_వెనుక(std::move(nodes[i]));
            స్వయంచాలక దొరికింది = sparse_attrs.కనుగొను(i);
            ఒకవేళ (దొరికింది != sparse_attrs.చివర())
                new_sparse_attrs.emplace(j, std::move(దొరికింది->రెండవ));
        }

        std::మార్పిడి(nodes, new_nodes);
        std::మార్పిడి(sparse_attrs, new_sparse_attrs);

        compact_వాదనలు();
        కొరకు (పూర్ణాంకం &arg : వాదనలు_2)
        {
            log_assert(arg < Get_కొలత(inv_perm));
            log_assert(inv_perm[arg] >= 0);
            arg = inv_perm[arg];
        }

        కొరకు (స్వయంచాలక &కీలకం : కీలకాలు)
        {
            log_assert(కీలకం.రెండవ < Get_కొలత(inv_perm));
            log_assert(inv_perm[కీలకం.రెండవ] >= 0);
            కీలకం.రెండవ = inv_perm[కీలకం.రెండవ];
        }
    }

    నిర్మాణం SccAdaptor
    {
    అంతరంగ:
        Compute_లేఖాచిత్రం స్థిరరాశి &లేఖాచిత్రం_2;
        std::సదిశ<పూర్ణాంకం> indices_;
    బహిరంగ:
        SccAdaptor(Compute_లేఖాచిత్రం స్థిరరాశి &లేఖాచిత్రం_3) : లేఖాచిత్రం_2(లేఖాచిత్రం_3)
        {
            indices_.resize(లేఖాచిత్రం_3.కొలత(), -1);
        }


        రకనిర్వచనం పూర్ణాంకం కణుపు_type;

        నిర్మాణం కణుపు_enumerator {
        అంతరంగ:
            మిత్ర నిర్మాణం SccAdaptor;
            పూర్ణాంకం current, చివర;
            కణుపు_enumerator(పూర్ణాంకం current, పూర్ణాంకం చివర) : current(current), చివర(చివర) {}

        బహిరంగ:

            bool ముగిసిన() స్థిరరాశి { తిరిగి current == చివర; }
            కణుపు_type తదుపరి() {
                log_assert(!ముగిసిన());
                కణుపు_type ఫలితం = current;
                ++current;
                తిరిగి ఫలితం;
            }
        };

        కణుపు_enumerator enumerate_nodes() {
            తిరిగి కణుపు_enumerator(0, Get_కొలత(indices_));
        }


        నిర్మాణం successor_enumerator {
        అంతరంగ:
            మిత్ర నిర్మాణం SccAdaptor;
            std::సదిశ<పూర్ణాంకం>::const_iterator current, చివర;
            successor_enumerator(std::సదిశ<పూర్ణాంకం>::const_iterator current, std::సదిశ<పూర్ణాంకం>::const_iterator చివర) :
                current(current), చివర(చివర) {}

        బహిరంగ:
            bool ముగిసిన() స్థిరరాశి { తిరిగి current == చివర; }
            కణుపు_type తదుపరి() {
                log_assert(!ముగిసిన());
                కణుపు_type ఫలితం = *current;
                ++current;
                తిరిగి ఫలితం;
            }
        };

        successor_enumerator enumerate_వారసులు(పూర్ణాంకం సూచీ) స్థిరరాశి {
            స్వయంచాలక స్థిరరాశి &ref = లేఖాచిత్రం_2[సూచీ];
            తిరిగి successor_enumerator(ref.arg_indices_cbegin(), ref.arg_indices_cend());
        }

        పూర్ణాంకం &dfs_సూచీ(కణుపు_type స్థిరరాశి &కణుపు) { తిరిగి indices_[కణుపు]; }

        std::సదిశ<పూర్ణాంకం> స్థిరరాశి &dfs_indices() { తిరిగి indices_; }
    };

};



YOSYS_NAMESPACE_చివర


#అయితే_ముగింపు
