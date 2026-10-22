/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2022  Jannis Harder <jix@yosyshq.com> <me@jix.one>
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

#అన్తర్భావ "kernel/json.h"

USING_YOSYS_NAMESPACE

శూన్య PrettyJson::emit_గమ్యం_log()
{
    నిర్మాణం Log_లక్ష్యం : బహిరంగ లక్ష్యం {
        శూన్య emit(స్థిరరాశి అక్షరం *దత్తాంశం) override { సంవర్గమానం("%s", దత్తాంశం); }
    };

    targets.నెట్టు_వెనుక(std::unique_ptr<లక్ష్యం>(కొత్త Log_లక్ష్యం));
}

శూన్య PrettyJson::append_గమ్యం_string(std::string &లక్ష్యం_2)
{
    నిర్మాణం Append_String_లక్ష్యం : బహిరంగ లక్ష్యం {
        std::string &లక్ష్యం_2;
        Append_String_లక్ష్యం(std::string &లక్ష్యం_2) : లక్ష్యం_2(లక్ష్యం_2) {}
        శూన్య emit(స్థిరరాశి అక్షరం *దత్తాంశం) override { లక్ష్యం_2 += దత్తాంశం; }
    };

    targets.నెట్టు_వెనుక(std::unique_ptr<లక్ష్యం>(కొత్త Append_String_లక్ష్యం(లక్ష్యం_2)));
}

తార్కిక PrettyJson::write_గమ్యం_file(స్థిరరాశి std::string &మార్గం_2)
{
    నిర్మాణం Write_File_లక్ష్యం : బహిరంగ లక్ష్యం {
        std::ofstream లక్ష్యం_2;
        శూన్య emit(స్థిరరాశి అక్షరం *దత్తాంశం) override { లక్ష్యం_2 << దత్తాంశం; }
        శూన్య flush() override { లక్ష్యం_2.flush(); }
    };

    స్వయంచాలక లక్ష్యం_2 = std::unique_ptr<Write_File_లక్ష్యం>(కొత్త Write_File_లక్ష్యం);
    లక్ష్యం_2->లక్ష్యం_2.open(మార్గం_2);
    ఒకవేళ (లక్ష్యం_2->లక్ష్యం_2.fail())
        తిరిగి అబద్ధం;
    targets.నెట్టు_వెనుక(std::unique_ptr<లక్ష్యం>(లక్ష్యం_2.release()));
    తిరిగి నిజం;
}

శూన్య PrettyJson::పంక్తి(తార్కిక space_if_inline)
{
    ఒకవేళ (compact_లోతు != గరిష్ఠ_పూర్ణాంకం) {
        ఒకవేళ (space_if_inline)
            raw(" ");
        తిరిగి;
    }
    పూర్ణాంకం indent = state.కొలత() - (state.ఖాళీలు() ? 0 : state.వెనుక() == విలువ_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

శూన్య PrettyJson::raw(స్థిరరాశి అక్షరం *raw_json)
{
    కొరకు (స్వయంచాలక &లక్ష్యం_2 : targets)
        లక్ష్యం_2->emit(raw_json);
}

శూన్య PrettyJson::flush()
{
    కొరకు (స్వయంచాలక &లక్ష్యం_2 : targets)
        లక్ష్యం_2->flush();
}

శూన్య PrettyJson::begin_object()
{
    begin_విలువ();
    raw("{");
    state.నెట్టు_వెనుక(OBJECT_మొదటి);
}

శూన్య PrettyJson::begin_array()
{
    begin_విలువ();
    raw("[");
    state.నెట్టు_వెనుక(ARRAY_మొదటి);
}

శూన్య PrettyJson::చివర_object()
{
    Scope శిఖరం_scope = state.వెనుక();
    state.తీయి_వెనుక();
    ఒకవేళ (శిఖరం_scope == OBJECT)
        పంక్తి(అబద్ధం);
    లేకపోతే
        log_assert(శిఖరం_scope == OBJECT_మొదటి);
    raw("}");
    చివర_విలువ();
}

శూన్య PrettyJson::చివర_array()
{
    Scope శిఖరం_scope = state.వెనుక();
    state.తీయి_వెనుక();
    ఒకవేళ (శిఖరం_scope == ARRAY)
        పంక్తి(అబద్ధం);
    లేకపోతే
        log_assert(శిఖరం_scope == ARRAY_మొదటి);
    raw("]");
    చివర_విలువ();
}

శూన్య PrettyJson::name(స్థిరరాశి అక్షరం *name)
{
    ఒకవేళ (state.వెనుక() == OBJECT_మొదటి) {
        state.వెనుక() = OBJECT;
        పంక్తి(అబద్ధం);
    } లేకపోతే {
        raw(",");
        పంక్తి();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.నెట్టు_వెనుక(విలువ_2);
}

శూన్య PrettyJson::begin_విలువ()
{
    ఒకవేళ (state.వెనుక() == ARRAY_మొదటి) {
        పంక్తి(అబద్ధం);
        state.వెనుక() = ARRAY;
    } లేకపోతే ఒకవేళ (state.వెనుక() == ARRAY) {
        raw(",");
        పంక్తి();
    } లేకపోతే {
        log_assert(state.వెనుక() == విలువ_2);
        state.తీయి_వెనుక();
    }
}

శూన్య PrettyJson::చివర_విలువ()
{
    ఒకవేళ (state.ఖాళీలు()) {
        raw("\n");
        flush();
    }
    ఒకవేళ (Get_కొలత(state) < compact_లోతు)
        compact_లోతు = గరిష్ఠ_పూర్ణాంకం;
}

శూన్య PrettyJson::విలువ_json(స్థిరరాశి Json &విలువ)
{
    begin_విలువ();
    raw(విలువ.dump().c_str());
    చివర_విలువ();
}

శూన్య PrettyJson::నమోదు_json(స్థిరరాశి అక్షరం *name, స్థిరరాశి Json &విలువ)
{
    ఇది->name(name);
    ఇది->విలువ(విలువ);
}
