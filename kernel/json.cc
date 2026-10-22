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
    بنية Log_موخه : عام موخه {
        فراغ emit(ثابت حرف *معلومات) override { لوګاریتم("%s", معلومات); }
    };

    targets.ټېل_شا(std::unique_ptr<موخه>(جديد Log_موخه));
}

فراغ PrettyJson::append_منزل_string(std::string &موخه_2)
{
    بنية Append_String_موخه : عام موخه {
        std::string &موخه_2;
        Append_String_موخه(std::string &موخه_2) : موخه_2(موخه_2) {}
        فراغ emit(ثابت حرف *معلومات) override { موخه_2 += معلومات; }
    };

    targets.ټېل_شا(std::unique_ptr<موخه>(جديد Append_String_موخه(موخه_2)));
}

منطقي PrettyJson::write_منزل_file(ثابت std::string &لار_2)
{
    بنية Write_File_موخه : عام موخه {
        std::ofstream موخه_2;
        فراغ emit(ثابت حرف *معلومات) override { موخه_2 << معلومات; }
        فراغ flush() override { موخه_2.flush(); }
    };

    خپلکاری موخه_2 = std::unique_ptr<Write_File_موخه>(جديد Write_File_موخه);
    موخه_2->موخه_2.open(لار_2);
    إذا (موخه_2->موخه_2.fail())
        أرجع خطأ;
    targets.ټېل_شا(std::unique_ptr<موخه>(موخه_2.release()));
    أرجع ریښتیا;
}

فراغ PrettyJson::کرښه(منطقي space_if_inline)
{
    إذا (compact_ژوروالی != لوی_عدد) {
        إذا (space_if_inline)
            raw(" ");
        أرجع;
    }
    صحيح indent = state.اندازه() - (state.تشه() ? 0 : state.شا() == ارزښت_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

فراغ PrettyJson::raw(ثابت حرف *raw_json)
{
    لكل (خپلکاری &موخه_2 : targets)
        موخه_2->emit(raw_json);
}

فراغ PrettyJson::flush()
{
    لكل (خپلکاری &موخه_2 : targets)
        موخه_2->flush();
}

فراغ PrettyJson::begin_object()
{
    begin_ارزښت();
    raw("{");
    state.ټېل_شا(OBJECT_لومړی);
}

فراغ PrettyJson::begin_array()
{
    begin_ارزښت();
    raw("[");
    state.ټېل_شا(ARRAY_لومړی);
}

فراغ PrettyJson::پای_ټکی_object()
{
    Scope سر_scope = state.شا();
    state.راوباسه_شا();
    إذا (سر_scope == OBJECT)
        کرښه(خطأ);
    وإلا
        log_assert(سر_scope == OBJECT_لومړی);
    raw("}");
    پای_ټکی_ارزښت();
}

فراغ PrettyJson::پای_ټکی_array()
{
    Scope سر_scope = state.شا();
    state.راوباسه_شا();
    إذا (سر_scope == ARRAY)
        کرښه(خطأ);
    وإلا
        log_assert(سر_scope == ARRAY_لومړی);
    raw("]");
    پای_ټکی_ارزښت();
}

فراغ PrettyJson::name(ثابت حرف *name)
{
    إذا (state.شا() == OBJECT_لومړی) {
        state.شا() = OBJECT;
        کرښه(خطأ);
    } وإلا {
        raw(",");
        کرښه();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.ټېل_شا(ارزښت_2);
}

فراغ PrettyJson::begin_ارزښت()
{
    إذا (state.شا() == ARRAY_لومړی) {
        کرښه(خطأ);
        state.شا() = ARRAY;
    } وإلا إذا (state.شا() == ARRAY) {
        raw(",");
        کرښه();
    } وإلا {
        log_assert(state.شا() == ارزښت_2);
        state.راوباسه_شا();
    }
}

فراغ PrettyJson::پای_ټکی_ارزښت()
{
    إذا (state.تشه()) {
        raw("\n");
        flush();
    }
    إذا (Get_اندازه(state) < compact_ژوروالی)
        compact_ژوروالی = لوی_عدد;
}

فراغ PrettyJson::ارزښت_json(ثابت Json &ارزښت)
{
    begin_ارزښت();
    raw(ارزښت.dump().c_str());
    پای_ټکی_ارزښت();
}

فراغ PrettyJson::ثبت_json(ثابت حرف *name, ثابت Json &ارزښت)
{
    هذا->name(name);
    هذا->ارزښت(ارزښت);
}
