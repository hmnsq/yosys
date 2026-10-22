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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/json.h"

USING_YOSYS_NAMESPACE

ਖਾਲੀ PrettyJson::emit_ਮੰਜ਼ਿਲ_log()
{
    ਬਣਤਰ Log_ਨਿਸ਼ਾਨਾ : ਜਨਤਕ ਨਿਸ਼ਾਨਾ {
        ਖਾਲੀ emit(ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ) override { ਲਘੂਗਣਕ("%s", ਅੰਕੜੇ); }
    };

    targets.ਧੱਕੋ_ਪਿੱਠ(std::unique_ptr<ਨਿਸ਼ਾਨਾ>(ਨਵਾਂ Log_ਨਿਸ਼ਾਨਾ));
}

ਖਾਲੀ PrettyJson::append_ਮੰਜ਼ਿਲ_string(std::string &ਨਿਸ਼ਾਨਾ_2)
{
    ਬਣਤਰ Append_String_ਨਿਸ਼ਾਨਾ : ਜਨਤਕ ਨਿਸ਼ਾਨਾ {
        std::string &ਨਿਸ਼ਾਨਾ_2;
        Append_String_ਨਿਸ਼ਾਨਾ(std::string &ਨਿਸ਼ਾਨਾ_2) : ਨਿਸ਼ਾਨਾ_2(ਨਿਸ਼ਾਨਾ_2) {}
        ਖਾਲੀ emit(ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ) override { ਨਿਸ਼ਾਨਾ_2 += ਅੰਕੜੇ; }
    };

    targets.ਧੱਕੋ_ਪਿੱਠ(std::unique_ptr<ਨਿਸ਼ਾਨਾ>(ਨਵਾਂ Append_String_ਨਿਸ਼ਾਨਾ(ਨਿਸ਼ਾਨਾ_2)));
}

ਤਾਰਕਿਕ PrettyJson::write_ਮੰਜ਼ਿਲ_file(ਅਚਲ std::string &ਰਸਤਾ_2)
{
    ਬਣਤਰ Write_File_ਨਿਸ਼ਾਨਾ : ਜਨਤਕ ਨਿਸ਼ਾਨਾ {
        std::ofstream ਨਿਸ਼ਾਨਾ_2;
        ਖਾਲੀ emit(ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ) override { ਨਿਸ਼ਾਨਾ_2 << ਅੰਕੜੇ; }
        ਖਾਲੀ flush() override { ਨਿਸ਼ਾਨਾ_2.flush(); }
    };

    ਸਵੈਚਾਲਿਤ ਨਿਸ਼ਾਨਾ_2 = std::unique_ptr<Write_File_ਨਿਸ਼ਾਨਾ>(ਨਵਾਂ Write_File_ਨਿਸ਼ਾਨਾ);
    ਨਿਸ਼ਾਨਾ_2->ਨਿਸ਼ਾਨਾ_2.open(ਰਸਤਾ_2);
    ਜੇ (ਨਿਸ਼ਾਨਾ_2->ਨਿਸ਼ਾਨਾ_2.fail())
        ਵਾਪਸ ਝੂਠ;
    targets.ਧੱਕੋ_ਪਿੱਠ(std::unique_ptr<ਨਿਸ਼ਾਨਾ>(ਨਿਸ਼ਾਨਾ_2.release()));
    ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ PrettyJson::ਸਤਰ(ਤਾਰਕਿਕ space_if_inline)
{
    ਜੇ (compact_ਡੂੰਘਾਈ != ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) {
        ਜੇ (space_if_inline)
            raw(" ");
        ਵਾਪਸ;
    }
    ਪੂਰਨ indent = state.ਮਾਪ() - (state.ਖਾਲੀ_ਥਾਵਾਂ() ? 0 : state.ਪਿੱਠ() == ਮੁੱਲ_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

ਖਾਲੀ PrettyJson::raw(ਅਚਲ ਅੱਖਰ *raw_json)
{
    ਲਈ (ਸਵੈਚਾਲਿਤ &ਨਿਸ਼ਾਨਾ_2 : targets)
        ਨਿਸ਼ਾਨਾ_2->emit(raw_json);
}

ਖਾਲੀ PrettyJson::flush()
{
    ਲਈ (ਸਵੈਚਾਲਿਤ &ਨਿਸ਼ਾਨਾ_2 : targets)
        ਨਿਸ਼ਾਨਾ_2->flush();
}

ਖਾਲੀ PrettyJson::begin_object()
{
    begin_ਮੁੱਲ();
    raw("{");
    state.ਧੱਕੋ_ਪਿੱਠ(OBJECT_ਪਹਿਲਾ);
}

ਖਾਲੀ PrettyJson::begin_array()
{
    begin_ਮੁੱਲ();
    raw("[");
    state.ਧੱਕੋ_ਪਿੱਠ(ARRAY_ਪਹਿਲਾ);
}

ਖਾਲੀ PrettyJson::ਸਿਰਾ_object()
{
    Scope ਸਿਖਰ_scope = state.ਪਿੱਠ();
    state.ਕੱਢੋ_ਪਿੱਠ();
    ਜੇ (ਸਿਖਰ_scope == OBJECT)
        ਸਤਰ(ਝੂਠ);
    ਨਹੀਂਤਾਂ
        log_assert(ਸਿਖਰ_scope == OBJECT_ਪਹਿਲਾ);
    raw("}");
    ਸਿਰਾ_ਮੁੱਲ();
}

ਖਾਲੀ PrettyJson::ਸਿਰਾ_array()
{
    Scope ਸਿਖਰ_scope = state.ਪਿੱਠ();
    state.ਕੱਢੋ_ਪਿੱਠ();
    ਜੇ (ਸਿਖਰ_scope == ARRAY)
        ਸਤਰ(ਝੂਠ);
    ਨਹੀਂਤਾਂ
        log_assert(ਸਿਖਰ_scope == ARRAY_ਪਹਿਲਾ);
    raw("]");
    ਸਿਰਾ_ਮੁੱਲ();
}

ਖਾਲੀ PrettyJson::name(ਅਚਲ ਅੱਖਰ *name)
{
    ਜੇ (state.ਪਿੱਠ() == OBJECT_ਪਹਿਲਾ) {
        state.ਪਿੱਠ() = OBJECT;
        ਸਤਰ(ਝੂਠ);
    } ਨਹੀਂਤਾਂ {
        raw(",");
        ਸਤਰ();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.ਧੱਕੋ_ਪਿੱਠ(ਮੁੱਲ_2);
}

ਖਾਲੀ PrettyJson::begin_ਮੁੱਲ()
{
    ਜੇ (state.ਪਿੱਠ() == ARRAY_ਪਹਿਲਾ) {
        ਸਤਰ(ਝੂਠ);
        state.ਪਿੱਠ() = ARRAY;
    } ਨਹੀਂਤਾਂ ਜੇ (state.ਪਿੱਠ() == ARRAY) {
        raw(",");
        ਸਤਰ();
    } ਨਹੀਂਤਾਂ {
        log_assert(state.ਪਿੱਠ() == ਮੁੱਲ_2);
        state.ਕੱਢੋ_ਪਿੱਠ();
    }
}

ਖਾਲੀ PrettyJson::ਸਿਰਾ_ਮੁੱਲ()
{
    ਜੇ (state.ਖਾਲੀ_ਥਾਵਾਂ()) {
        raw("\n");
        flush();
    }
    ਜੇ (Get_ਮਾਪ(state) < compact_ਡੂੰਘਾਈ)
        compact_ਡੂੰਘਾਈ = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
}

ਖਾਲੀ PrettyJson::ਮੁੱਲ_json(ਅਚਲ Json &ਮੁੱਲ)
{
    begin_ਮੁੱਲ();
    raw(ਮੁੱਲ.dump().c_str());
    ਸਿਰਾ_ਮੁੱਲ();
}

ਖਾਲੀ PrettyJson::ਇੰਦਰਾਜ_json(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ Json &ਮੁੱਲ)
{
    ਇਹ->name(name);
    ਇਹ->ਮੁੱਲ(ਮੁੱਲ);
}
