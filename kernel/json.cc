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

فراغ PrettyJson::emit_مقصد_log()
{
    بنية Log_هدف : عام هدف {
        فراغ emit(ثابت حرف *داده) override { لگاریتم("%s", داده); }
    };

    targets.افزودن_پشت(std::unique_ptr<هدف>(جديد Log_هدف));
}

فراغ PrettyJson::append_مقصد_string(std::string &هدف_2)
{
    بنية Append_String_هدف : عام هدف {
        std::string &هدف_2;
        Append_String_هدف(std::string &هدف_2) : هدف_2(هدف_2) {}
        فراغ emit(ثابت حرف *داده) override { هدف_2 += داده; }
    };

    targets.افزودن_پشت(std::unique_ptr<هدف>(جديد Append_String_هدف(هدف_2)));
}

منطقي PrettyJson::write_مقصد_file(ثابت std::string &مسیر_2)
{
    بنية Write_File_هدف : عام هدف {
        std::ofstream هدف_2;
        فراغ emit(ثابت حرف *داده) override { هدف_2 << داده; }
        فراغ flush() override { هدف_2.flush(); }
    };

    خودکار هدف_2 = std::unique_ptr<Write_File_هدف>(جديد Write_File_هدف);
    هدف_2->هدف_2.open(مسیر_2);
    إذا (هدف_2->هدف_2.fail())
        أرجع خطأ;
    targets.افزودن_پشت(std::unique_ptr<هدف>(هدف_2.release()));
    أرجع درست;
}

فراغ PrettyJson::سطر(منطقي space_if_inline)
{
    إذا (compact_عمق != بیشترین_صحیح) {
        إذا (space_if_inline)
            raw(" ");
        أرجع;
    }
    صحيح indent = state.اندازه() - (state.خالی() ? 0 : state.پشت() == مقدار_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

فراغ PrettyJson::raw(ثابت حرف *raw_json)
{
    لكل (خودکار &هدف_2 : targets)
        هدف_2->emit(raw_json);
}

فراغ PrettyJson::flush()
{
    لكل (خودکار &هدف_2 : targets)
        هدف_2->flush();
}

فراغ PrettyJson::begin_object()
{
    begin_مقدار();
    raw("{");
    state.افزودن_پشت(OBJECT_اولی);
}

فراغ PrettyJson::begin_array()
{
    begin_مقدار();
    raw("[");
    state.افزودن_پشت(ARRAY_اولی);
}

فراغ PrettyJson::انتها_object()
{
    Scope بالاترین_scope = state.پشت();
    state.برداشت_پشت();
    إذا (بالاترین_scope == OBJECT)
        سطر(خطأ);
    وإلا
        log_assert(بالاترین_scope == OBJECT_اولی);
    raw("}");
    انتها_مقدار();
}

فراغ PrettyJson::انتها_array()
{
    Scope بالاترین_scope = state.پشت();
    state.برداشت_پشت();
    إذا (بالاترین_scope == ARRAY)
        سطر(خطأ);
    وإلا
        log_assert(بالاترین_scope == ARRAY_اولی);
    raw("]");
    انتها_مقدار();
}

فراغ PrettyJson::name(ثابت حرف *name)
{
    إذا (state.پشت() == OBJECT_اولی) {
        state.پشت() = OBJECT;
        سطر(خطأ);
    } وإلا {
        raw(",");
        سطر();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.افزودن_پشت(مقدار_2);
}

فراغ PrettyJson::begin_مقدار()
{
    إذا (state.پشت() == ARRAY_اولی) {
        سطر(خطأ);
        state.پشت() = ARRAY;
    } وإلا إذا (state.پشت() == ARRAY) {
        raw(",");
        سطر();
    } وإلا {
        log_assert(state.پشت() == مقدار_2);
        state.برداشت_پشت();
    }
}

فراغ PrettyJson::انتها_مقدار()
{
    إذا (state.خالی()) {
        raw("\n");
        flush();
    }
    إذا (Get_اندازه(state) < compact_عمق)
        compact_عمق = بیشترین_صحیح;
}

فراغ PrettyJson::مقدار_json(ثابت Json &مقدار)
{
    begin_مقدار();
    raw(مقدار.dump().c_str());
    انتها_مقدار();
}

فراغ PrettyJson::مدخل_json(ثابت حرف *name, ثابت Json &مقدار)
{
    هذا->name(name);
    هذا->مقدار(مقدار);
}
