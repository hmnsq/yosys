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
    بنية Log_نشانہ : عام نشانہ {
        فراغ emit(ثابت حرف *انکڑے) override { لوگارتھم("%s", انکڑے); }
    };

    targets.ڈالو_پٹھ(std::unique_ptr<نشانہ>(جديد Log_نشانہ));
}

فراغ PrettyJson::append_منزل_string(std::string &نشانہ_2)
{
    بنية Append_String_نشانہ : عام نشانہ {
        std::string &نشانہ_2;
        Append_String_نشانہ(std::string &نشانہ_2) : نشانہ_2(نشانہ_2) {}
        فراغ emit(ثابت حرف *انکڑے) override { نشانہ_2 += انکڑے; }
    };

    targets.ڈالو_پٹھ(std::unique_ptr<نشانہ>(جديد Append_String_نشانہ(نشانہ_2)));
}

منطقي PrettyJson::write_منزل_file(ثابت std::string &رستہ_2)
{
    بنية Write_File_نشانہ : عام نشانہ {
        std::ofstream نشانہ_2;
        فراغ emit(ثابت حرف *انکڑے) override { نشانہ_2 << انکڑے; }
        فراغ flush() override { نشانہ_2.flush(); }
    };

    خودکار نشانہ_2 = std::unique_ptr<Write_File_نشانہ>(جديد Write_File_نشانہ);
    نشانہ_2->نشانہ_2.open(رستہ_2);
    إذا (نشانہ_2->نشانہ_2.fail())
        أرجع خطأ;
    targets.ڈالو_پٹھ(std::unique_ptr<نشانہ>(نشانہ_2.release()));
    أرجع سچ;
}

فراغ PrettyJson::سطر(منطقي space_if_inline)
{
    إذا (compact_ڈونگھائی != ودھ_توں_ودھ_عدد) {
        إذا (space_if_inline)
            raw(" ");
        أرجع;
    }
    صحيح indent = state.ناپ() - (state.خالی_تھاواں() ? 0 : state.پٹھ() == قدر_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

فراغ PrettyJson::raw(ثابت حرف *raw_json)
{
    لكل (خودکار &نشانہ_2 : targets)
        نشانہ_2->emit(raw_json);
}

فراغ PrettyJson::flush()
{
    لكل (خودکار &نشانہ_2 : targets)
        نشانہ_2->flush();
}

فراغ PrettyJson::begin_object()
{
    begin_قدر();
    raw("{");
    state.ڈالو_پٹھ(OBJECT_پہلا);
}

فراغ PrettyJson::begin_array()
{
    begin_قدر();
    raw("[");
    state.ڈالو_پٹھ(ARRAY_پہلا);
}

فراغ PrettyJson::سرا_object()
{
    Scope چوٹی_scope = state.پٹھ();
    state.نکالو_پٹھ();
    إذا (چوٹی_scope == OBJECT)
        سطر(خطأ);
    وإلا
        log_assert(چوٹی_scope == OBJECT_پہلا);
    raw("}");
    سرا_قدر();
}

فراغ PrettyJson::سرا_array()
{
    Scope چوٹی_scope = state.پٹھ();
    state.نکالو_پٹھ();
    إذا (چوٹی_scope == ARRAY)
        سطر(خطأ);
    وإلا
        log_assert(چوٹی_scope == ARRAY_پہلا);
    raw("]");
    سرا_قدر();
}

فراغ PrettyJson::name(ثابت حرف *name)
{
    إذا (state.پٹھ() == OBJECT_پہلا) {
        state.پٹھ() = OBJECT;
        سطر(خطأ);
    } وإلا {
        raw(",");
        سطر();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.ڈالو_پٹھ(قدر_2);
}

فراغ PrettyJson::begin_قدر()
{
    إذا (state.پٹھ() == ARRAY_پہلا) {
        سطر(خطأ);
        state.پٹھ() = ARRAY;
    } وإلا إذا (state.پٹھ() == ARRAY) {
        raw(",");
        سطر();
    } وإلا {
        log_assert(state.پٹھ() == قدر_2);
        state.نکالو_پٹھ();
    }
}

فراغ PrettyJson::سرا_قدر()
{
    إذا (state.خالی_تھاواں()) {
        raw("\n");
        flush();
    }
    إذا (Get_ناپ(state) < compact_ڈونگھائی)
        compact_ڈونگھائی = ودھ_توں_ودھ_عدد;
}

فراغ PrettyJson::قدر_json(ثابت Json &قدر)
{
    begin_قدر();
    raw(قدر.dump().c_str());
    سرا_قدر();
}

فراغ PrettyJson::اندراج_json(ثابت حرف *name, ثابت Json &قدر)
{
    هذا->name(name);
    هذا->قدر(قدر);
}
