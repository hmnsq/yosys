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

#અન્તર્ભાવ "kernel/json.h"

USING_YOSYS_NAMESPACE

શૂન્ય PrettyJson::emit_to_log()
{
    સંરચના LogTarget : જાહેર Target {
        શૂન્ય emit(અચલ અક્ષર *data) override { log("%s", data); }
    };

    targets.ધકેલો_back(std::unique_ptr<Target>(નવું LogTarget));
}

શૂન્ય PrettyJson::append_to_string(std::string &target)
{
    સંરચના AppendStringTarget : જાહેર Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        શૂન્ય emit(અચલ અક્ષર *data) override { target += data; }
    };

    targets.ધકેલો_back(std::unique_ptr<Target>(નવું AppendStringTarget(target)));
}

તાર્કિક PrettyJson::write_to_file(અચલ std::string &path)
{
    સંરચના WriteFileTarget : જાહેર Target {
        std::ofstream target;
        શૂન્ય emit(અચલ અક્ષર *data) override { target << data; }
        શૂન્ય flush() override { target.flush(); }
    };

    auto target = std::unique_ptr<WriteFileTarget>(નવું WriteFileTarget);
    target->target.open(path);
    જો (target->target.fail())
        પાછા ખોટું;
    targets.ધકેલો_back(std::unique_ptr<Target>(target.release()));
    પાછા સાચું;
}

શૂન્ય PrettyJson::લીટી(તાર્કિક space_if_inline)
{
    જો (compact_depth != INT_MAX) {
        જો (space_if_inline)
            raw(" ");
        પાછા;
    }
    પૂર્ણાંક indent = state.size() - (state.empty() ? 0 : state.back() == કિંમત_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

શૂન્ય PrettyJson::raw(અચલ અક્ષર *raw_json)
{
    માટે (auto &target : targets)
        target->emit(raw_json);
}

શૂન્ય PrettyJson::flush()
{
    માટે (auto &target : targets)
        target->flush();
}

શૂન્ય PrettyJson::begin_object()
{
    begin_કિંમત();
    raw("{");
    state.ધકેલો_back(OBJECT_FIRST);
}

શૂન્ય PrettyJson::begin_array()
{
    begin_કિંમત();
    raw("[");
    state.ધકેલો_back(ARRAY_FIRST);
}

શૂન્ય PrettyJson::end_object()
{
    Scope ટોચ_scope = state.back();
    state.કાઢો_back();
    જો (ટોચ_scope == OBJECT)
        લીટી(ખોટું);
    નહીંતર
        log_assert(ટોચ_scope == OBJECT_FIRST);
    raw("}");
    end_કિંમત();
}

શૂન્ય PrettyJson::end_array()
{
    Scope ટોચ_scope = state.back();
    state.કાઢો_back();
    જો (ટોચ_scope == ARRAY)
        લીટી(ખોટું);
    નહીંતર
        log_assert(ટોચ_scope == ARRAY_FIRST);
    raw("]");
    end_કિંમત();
}

શૂન્ય PrettyJson::name(અચલ અક્ષર *name)
{
    જો (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        લીટી(ખોટું);
    } નહીંતર {
        raw(",");
        લીટી();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.ધકેલો_back(કિંમત_2);
}

શૂન્ય PrettyJson::begin_કિંમત()
{
    જો (state.back() == ARRAY_FIRST) {
        લીટી(ખોટું);
        state.back() = ARRAY;
    } નહીંતર જો (state.back() == ARRAY) {
        raw(",");
        લીટી();
    } નહીંતર {
        log_assert(state.back() == કિંમત_2);
        state.કાઢો_back();
    }
}

શૂન્ય PrettyJson::end_કિંમત()
{
    જો (state.empty()) {
        raw("\n");
        flush();
    }
    જો (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

શૂન્ય PrettyJson::કિંમત_json(અચલ Json &કિંમત)
{
    begin_કિંમત();
    raw(કિંમત.dump().c_str());
    end_કિંમત();
}

શૂન્ય PrettyJson::entry_json(અચલ અક્ષર *name, અચલ Json &કિંમત)
{
    આ->name(name);
    આ->કિંમત(કિંમત);
}
