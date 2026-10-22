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

    targets.تولق_back(std::unique_ptr<Target>(جديد LogTarget));
}

فراغ PrettyJson::append_to_string(std::string &target)
{
    بنية AppendStringTarget : عام Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        فراغ emit(ثابت حرف *data) override { target += data; }
    };

    targets.تولق_back(std::unique_ptr<Target>(جديد AppendStringTarget(target)));
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
    targets.تولق_back(std::unique_ptr<Target>(target.release()));
    أرجع true;
}

فراغ PrettyJson::باريس(منطقي space_if_inline)
{
    إذا (compact_depth != INT_MAX) {
        إذا (space_if_inline)
            raw(" ");
        أرجع;
    }
    صحيح indent = state.size() - (state.empty() ? 0 : state.back() == نيلاي_2);
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
    begin_نيلاي();
    raw("{");
    state.تولق_back(OBJECT_FIRST);
}

فراغ PrettyJson::begin_array()
{
    begin_نيلاي();
    raw("[");
    state.تولق_back(ARRAY_FIRST);
}

فراغ PrettyJson::end_object()
{
    Scope اتس_scope = state.back();
    state.کلوار_back();
    إذا (اتس_scope == OBJECT)
        باريس(خطأ);
    وإلا
        log_assert(اتس_scope == OBJECT_FIRST);
    raw("}");
    end_نيلاي();
}

فراغ PrettyJson::end_array()
{
    Scope اتس_scope = state.back();
    state.کلوار_back();
    إذا (اتس_scope == ARRAY)
        باريس(خطأ);
    وإلا
        log_assert(اتس_scope == ARRAY_FIRST);
    raw("]");
    end_نيلاي();
}

فراغ PrettyJson::name(ثابت حرف *name)
{
    إذا (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        باريس(خطأ);
    } وإلا {
        raw(",");
        باريس();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.تولق_back(نيلاي_2);
}

فراغ PrettyJson::begin_نيلاي()
{
    إذا (state.back() == ARRAY_FIRST) {
        باريس(خطأ);
        state.back() = ARRAY;
    } وإلا إذا (state.back() == ARRAY) {
        raw(",");
        باريس();
    } وإلا {
        log_assert(state.back() == نيلاي_2);
        state.کلوار_back();
    }
}

فراغ PrettyJson::end_نيلاي()
{
    إذا (state.empty()) {
        raw("\n");
        flush();
    }
    إذا (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

فراغ PrettyJson::نيلاي_json(ثابت Json &نيلاي)
{
    begin_نيلاي();
    raw(نيلاي.dump().c_str());
    end_نيلاي();
}

فراغ PrettyJson::entry_json(ثابت حرف *name, ثابت Json &نيلاي)
{
    هذا->name(name);
    هذا->نيلاي(نيلاي);
}
