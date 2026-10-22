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

فراغ PrettyJson::emit_to_log()
{
    بنية LogTarget : عام Target {
        فراغ emit(ثابت حرف *data) override { log("%s", data); }
    };

    targets.ادفع_back(std::unique_ptr<Target>(جديد LogTarget));
}

فراغ PrettyJson::append_to_string(std::string &target)
{
    بنية AppendStringTarget : عام Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        فراغ emit(ثابت حرف *data) override { target += data; }
    };

    targets.ادفع_back(std::unique_ptr<Target>(جديد AppendStringTarget(target)));
}

منطقي PrettyJson::write_to_file(ثابت std::string &path)
{
    بنية WriteFileTarget : عام Target {
        std::ofstream target;
        فراغ emit(ثابت حرف *data) override { target << data; }
        فراغ flush() override { target.flush(); }
    };

    auto target = std::unique_ptr<WriteFileTarget>(جديد WriteFileTarget);
    target->target.open(path);
    إذا (target->target.fail())
        أرجع خطأ;
    targets.ادفع_back(std::unique_ptr<Target>(target.release()));
    أرجع true;
}

فراغ PrettyJson::سطر(منطقي space_if_inline)
{
    إذا (compact_depth != INT_MAX) {
        إذا (space_if_inline)
            raw(" ");
        أرجع;
    }
    صحيح indent = state.size() - (state.empty() ? 0 : state.back() == قيمة_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

فراغ PrettyJson::raw(ثابت حرف *raw_json)
{
    لكل (auto &target : targets)
        target->emit(raw_json);
}

فراغ PrettyJson::flush()
{
    لكل (auto &target : targets)
        target->flush();
}

فراغ PrettyJson::begin_object()
{
    begin_قيمة();
    raw("{");
    state.ادفع_back(OBJECT_FIRST);
}

فراغ PrettyJson::begin_array()
{
    begin_قيمة();
    raw("[");
    state.ادفع_back(ARRAY_FIRST);
}

فراغ PrettyJson::end_object()
{
    Scope قمة_scope = state.back();
    state.اسحب_back();
    إذا (قمة_scope == OBJECT)
        سطر(خطأ);
    وإلا
        log_assert(قمة_scope == OBJECT_FIRST);
    raw("}");
    end_قيمة();
}

فراغ PrettyJson::end_array()
{
    Scope قمة_scope = state.back();
    state.اسحب_back();
    إذا (قمة_scope == ARRAY)
        سطر(خطأ);
    وإلا
        log_assert(قمة_scope == ARRAY_FIRST);
    raw("]");
    end_قيمة();
}

فراغ PrettyJson::name(ثابت حرف *name)
{
    إذا (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        سطر(خطأ);
    } وإلا {
        raw(",");
        سطر();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.ادفع_back(قيمة_2);
}

فراغ PrettyJson::begin_قيمة()
{
    إذا (state.back() == ARRAY_FIRST) {
        سطر(خطأ);
        state.back() = ARRAY;
    } وإلا إذا (state.back() == ARRAY) {
        raw(",");
        سطر();
    } وإلا {
        log_assert(state.back() == قيمة_2);
        state.اسحب_back();
    }
}

فراغ PrettyJson::end_قيمة()
{
    إذا (state.empty()) {
        raw("\n");
        flush();
    }
    إذا (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

فراغ PrettyJson::قيمة_json(ثابت Json &قيمة)
{
    begin_قيمة();
    raw(قيمة.dump().c_str());
    end_قيمة();
}

فراغ PrettyJson::entry_json(ثابت حرف *name, ثابت Json &قيمة)
{
    هذا->name(name);
    هذا->قيمة(قيمة);
}
