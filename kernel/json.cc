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

#ضمن "kernel/json.h"

USING_YOSYS_NAMESPACE

فراغ PrettyJson::emit_منزل_log()
{
    بنية Log_هدف : عام هدف {
        فراغ emit(ثابت حرف *معلومات) override { لاگرٿم("%s", معلومات); }
    };

    targets.ڌڪو_پٺ(std::unique_ptr<هدف>(جديد Log_هدف));
}

فراغ PrettyJson::append_منزل_string(std::string &هدف_2)
{
    بنية Append_String_هدف : عام هدف {
        std::string &هدف_2;
        Append_String_هدف(std::string &هدف_2) : هدف_2(هدف_2) {}
        فراغ emit(ثابت حرف *معلومات) override { هدف_2 += معلومات; }
    };

    targets.ڌڪو_پٺ(std::unique_ptr<هدف>(جديد Append_String_هدف(هدف_2)));
}

منطقي PrettyJson::write_منزل_file(ثابت std::string &رستو_2)
{
    بنية Write_File_هدف : عام هدف {
        std::ofstream هدف_2;
        فراغ emit(ثابت حرف *معلومات) override { هدف_2 << معلومات; }
        فراغ flush() override { هدف_2.flush(); }
    };

    خودڪار هدف_2 = std::unique_ptr<Write_File_هدف>(جديد Write_File_هدف);
    هدف_2->هدف_2.open(رستو_2);
    إذا (هدف_2->هدف_2.fail())
        أرجع خطأ;
    targets.ڌڪو_پٺ(std::unique_ptr<هدف>(هدف_2.release()));
    أرجع سچ;
}

فراغ PrettyJson::سٽ(منطقي space_if_inline)
{
    إذا (compact_اونهائي != وڌ_کان_وڌ_عدد) {
        إذا (space_if_inline)
            raw(" ");
        أرجع;
    }
    صحيح indent = state.سائيز() - (state.خالي_جايون() ? 0 : state.پٺ() == قدر_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

فراغ PrettyJson::raw(ثابت حرف *raw_json)
{
    لكل (خودڪار &هدف_2 : targets)
        هدف_2->emit(raw_json);
}

فراغ PrettyJson::flush()
{
    لكل (خودڪار &هدف_2 : targets)
        هدف_2->flush();
}

فراغ PrettyJson::begin_object()
{
    begin_قدر();
    raw("{");
    state.ڌڪو_پٺ(OBJECT_پهريون);
}

فراغ PrettyJson::begin_array()
{
    begin_قدر();
    raw("[");
    state.ڌڪو_پٺ(ARRAY_پهريون);
}

فراغ PrettyJson::پڇاڙي_object()
{
    Scope چوٽي_scope = state.پٺ();
    state.ڪڍو_پٺ();
    إذا (چوٽي_scope == OBJECT)
        سٽ(خطأ);
    وإلا
        log_assert(چوٽي_scope == OBJECT_پهريون);
    raw("}");
    پڇاڙي_قدر();
}

فراغ PrettyJson::پڇاڙي_array()
{
    Scope چوٽي_scope = state.پٺ();
    state.ڪڍو_پٺ();
    إذا (چوٽي_scope == ARRAY)
        سٽ(خطأ);
    وإلا
        log_assert(چوٽي_scope == ARRAY_پهريون);
    raw("]");
    پڇاڙي_قدر();
}

فراغ PrettyJson::name(ثابت حرف *name)
{
    إذا (state.پٺ() == OBJECT_پهريون) {
        state.پٺ() = OBJECT;
        سٽ(خطأ);
    } وإلا {
        raw(",");
        سٽ();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.ڌڪو_پٺ(قدر_2);
}

فراغ PrettyJson::begin_قدر()
{
    إذا (state.پٺ() == ARRAY_پهريون) {
        سٽ(خطأ);
        state.پٺ() = ARRAY;
    } وإلا إذا (state.پٺ() == ARRAY) {
        raw(",");
        سٽ();
    } وإلا {
        log_assert(state.پٺ() == قدر_2);
        state.ڪڍو_پٺ();
    }
}

فراغ PrettyJson::پڇاڙي_قدر()
{
    إذا (state.خالي_جايون()) {
        raw("\n");
        flush();
    }
    إذا (Get_سائيز(state) < compact_اونهائي)
        compact_اونهائي = وڌ_کان_وڌ_عدد;
}

فراغ PrettyJson::قدر_json(ثابت Json &قدر)
{
    begin_قدر();
    raw(قدر.dump().c_str());
    پڇاڙي_قدر();
}

فراغ PrettyJson::داخلا_json(ثابت حرف *name, ثابت Json &قدر)
{
    هذا->name(name);
    هذا->قدر(قدر);
}
