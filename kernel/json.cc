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

#അന്തര്ഭാവ "kernel/json.h"

USING_YOSYS_NAMESPACE

ശൂന്യം PrettyJson::emit_to_log()
{
    ഘടന LogTarget : പൊതു Target {
        ശൂന്യം emit(സ്ഥിരം അക്ഷരം *data) override { log("%s", data); }
    };

    targets.തള്ളുക_back(std::unique_ptr<Target>(പുതിയ LogTarget));
}

ശൂന്യം PrettyJson::append_to_string(std::string &target)
{
    ഘടന AppendStringTarget : പൊതു Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        ശൂന്യം emit(സ്ഥിരം അക്ഷരം *data) override { target += data; }
    };

    targets.തള്ളുക_back(std::unique_ptr<Target>(പുതിയ AppendStringTarget(target)));
}

തർക്കം PrettyJson::write_to_file(സ്ഥിരം std::string &path)
{
    ഘടന WriteFileTarget : പൊതു Target {
        std::ofstream target;
        ശൂന്യം emit(സ്ഥിരം അക്ഷരം *data) override { target << data; }
        ശൂന്യം flush() override { target.flush(); }
    };

    auto target = std::unique_ptr<WriteFileTarget>(പുതിയ WriteFileTarget);
    target->target.open(path);
    എങ്കിൽ (target->target.fail())
        തിരികെ അസത്യം;
    targets.തള്ളുക_back(std::unique_ptr<Target>(target.release()));
    തിരികെ സത്യം;
}

ശൂന്യം PrettyJson::വരി(തർക്കം space_if_inline)
{
    എങ്കിൽ (compact_depth != INT_MAX) {
        എങ്കിൽ (space_if_inline)
            raw(" ");
        തിരികെ;
    }
    പൂർണ്ണസംഖ്യ indent = state.size() - (state.empty() ? 0 : state.back() == മൂല്യം_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

ശൂന്യം PrettyJson::raw(സ്ഥിരം അക്ഷരം *raw_json)
{
    വേണ്ടി (auto &target : targets)
        target->emit(raw_json);
}

ശൂന്യം PrettyJson::flush()
{
    വേണ്ടി (auto &target : targets)
        target->flush();
}

ശൂന്യം PrettyJson::begin_object()
{
    begin_മൂല്യം();
    raw("{");
    state.തള്ളുക_back(OBJECT_FIRST);
}

ശൂന്യം PrettyJson::begin_array()
{
    begin_മൂല്യം();
    raw("[");
    state.തള്ളുക_back(ARRAY_FIRST);
}

ശൂന്യം PrettyJson::end_object()
{
    Scope മുകൾ_scope = state.back();
    state.എടുക്കുക_back();
    എങ്കിൽ (മുകൾ_scope == OBJECT)
        വരി(അസത്യം);
    അല്ലെങ്കിൽ
        log_assert(മുകൾ_scope == OBJECT_FIRST);
    raw("}");
    end_മൂല്യം();
}

ശൂന്യം PrettyJson::end_array()
{
    Scope മുകൾ_scope = state.back();
    state.എടുക്കുക_back();
    എങ്കിൽ (മുകൾ_scope == ARRAY)
        വരി(അസത്യം);
    അല്ലെങ്കിൽ
        log_assert(മുകൾ_scope == ARRAY_FIRST);
    raw("]");
    end_മൂല്യം();
}

ശൂന്യം PrettyJson::name(സ്ഥിരം അക്ഷരം *name)
{
    എങ്കിൽ (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        വരി(അസത്യം);
    } അല്ലെങ്കിൽ {
        raw(",");
        വരി();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.തള്ളുക_back(മൂല്യം_2);
}

ശൂന്യം PrettyJson::begin_മൂല്യം()
{
    എങ്കിൽ (state.back() == ARRAY_FIRST) {
        വരി(അസത്യം);
        state.back() = ARRAY;
    } അല്ലെങ്കിൽ എങ്കിൽ (state.back() == ARRAY) {
        raw(",");
        വരി();
    } അല്ലെങ്കിൽ {
        log_assert(state.back() == മൂല്യം_2);
        state.എടുക്കുക_back();
    }
}

ശൂന്യം PrettyJson::end_മൂല്യം()
{
    എങ്കിൽ (state.empty()) {
        raw("\n");
        flush();
    }
    എങ്കിൽ (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

ശൂന്യം PrettyJson::മൂല്യം_json(സ്ഥിരം Json &മൂല്യം)
{
    begin_മൂല്യം();
    raw(മൂല്യം.dump().c_str());
    end_മൂല്യം();
}

ശൂന്യം PrettyJson::entry_json(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം Json &മൂല്യം)
{
    ഇത്->name(name);
    ഇത്->മൂല്യം(മൂല്യം);
}
