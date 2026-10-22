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

#شامل "kernel/json.h"

USING_YOSYS_NAMESPACE

خالی PrettyJson::emit_مقصد_log()
{
    ساخت Log_ہدف : عوامی ہدف {
        خالی emit(ثابت حرف *ڈیٹا) override { لوگارتھم("%s", ڈیٹا); }
    };

    targets.ڈالو_عقب(std::unique_ptr<ہدف>(نیا Log_ہدف));
}

خالی PrettyJson::append_مقصد_string(std::string &ہدف_2)
{
    ساخت Append_String_ہدف : عوامی ہدف {
        std::string &ہدف_2;
        Append_String_ہدف(std::string &ہدف_2) : ہدف_2(ہدف_2) {}
        خالی emit(ثابت حرف *ڈیٹا) override { ہدف_2 += ڈیٹا; }
    };

    targets.ڈالو_عقب(std::unique_ptr<ہدف>(نیا Append_String_ہدف(ہدف_2)));
}

منطقی PrettyJson::write_مقصد_file(ثابت std::string &راستہ_2)
{
    ساخت Write_File_ہدف : عوامی ہدف {
        std::ofstream ہدف_2;
        خالی emit(ثابت حرف *ڈیٹا) override { ہدف_2 << ڈیٹا; }
        خالی flush() override { ہدف_2.flush(); }
    };

    خودکار ہدف_2 = std::unique_ptr<Write_File_ہدف>(نیا Write_File_ہدف);
    ہدف_2->ہدف_2.open(راستہ_2);
    اگر (ہدف_2->ہدف_2.fail())
        واپس جھوٹ;
    targets.ڈالو_عقب(std::unique_ptr<ہدف>(ہدف_2.release()));
    واپس سچ;
}

خالی PrettyJson::سطر(منطقی space_if_inline)
{
    اگر (compact_گہرائی != زیادہ_سے_زیادہ_عدد) {
        اگر (space_if_inline)
            raw(" ");
        واپس;
    }
    صحیح indent = state.سائز() - (state.خالی_خانے() ? 0 : state.عقب() == قدر_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

خالی PrettyJson::raw(ثابت حرف *raw_json)
{
    برائے (خودکار &ہدف_2 : targets)
        ہدف_2->emit(raw_json);
}

خالی PrettyJson::flush()
{
    برائے (خودکار &ہدف_2 : targets)
        ہدف_2->flush();
}

خالی PrettyJson::begin_object()
{
    begin_قدر();
    raw("{");
    state.ڈالو_عقب(OBJECT_پہلا);
}

خالی PrettyJson::begin_array()
{
    begin_قدر();
    raw("[");
    state.ڈالو_عقب(ARRAY_پہلا);
}

خالی PrettyJson::انتہا_object()
{
    Scope چوٹی_scope = state.عقب();
    state.نکالو_عقب();
    اگر (چوٹی_scope == OBJECT)
        سطر(جھوٹ);
    ورنہ
        log_assert(چوٹی_scope == OBJECT_پہلا);
    raw("}");
    انتہا_قدر();
}

خالی PrettyJson::انتہا_array()
{
    Scope چوٹی_scope = state.عقب();
    state.نکالو_عقب();
    اگر (چوٹی_scope == ARRAY)
        سطر(جھوٹ);
    ورنہ
        log_assert(چوٹی_scope == ARRAY_پہلا);
    raw("]");
    انتہا_قدر();
}

خالی PrettyJson::name(ثابت حرف *name)
{
    اگر (state.عقب() == OBJECT_پہلا) {
        state.عقب() = OBJECT;
        سطر(جھوٹ);
    } ورنہ {
        raw(",");
        سطر();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.ڈالو_عقب(قدر_2);
}

خالی PrettyJson::begin_قدر()
{
    اگر (state.عقب() == ARRAY_پہلا) {
        سطر(جھوٹ);
        state.عقب() = ARRAY;
    } ورنہ اگر (state.عقب() == ARRAY) {
        raw(",");
        سطر();
    } ورنہ {
        log_assert(state.عقب() == قدر_2);
        state.نکالو_عقب();
    }
}

خالی PrettyJson::انتہا_قدر()
{
    اگر (state.خالی_خانے()) {
        raw("\n");
        flush();
    }
    اگر (Get_سائز(state) < compact_گہرائی)
        compact_گہرائی = زیادہ_سے_زیادہ_عدد;
}

خالی PrettyJson::قدر_json(ثابت Json &قدر)
{
    begin_قدر();
    raw(قدر.dump().c_str());
    انتہا_قدر();
}

خالی PrettyJson::اندراج_json(ثابت حرف *name, ثابت Json &قدر)
{
    یہ->name(name);
    یہ->قدر(قدر);
}
