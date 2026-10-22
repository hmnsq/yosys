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

    targets.push_back(std::unique_ptr<Target>(جديد LogTarget));
}

فراغ PrettyJson::append_to_string(std::string &target)
{
    بنية AppendStringTarget : عام Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        فراغ emit(ثابت حرف *data) override { target += data; }
    };

    targets.push_back(std::unique_ptr<Target>(جديد AppendStringTarget(target)));
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
    𐤀𐤌 (target->target.fail())
        أرجع خطأ;
    targets.push_back(std::unique_ptr<Target>(target.release()));
    أرجع true;
}

فراغ PrettyJson::line(منطقي space_if_inline)
{
    𐤀𐤌 (compact_depth != INT_MAX) {
        𐤀𐤌 (space_if_inline)
            raw(" ");
        أرجع;
    }
    صحيح indent = state.size() - (state.empty() ? 0 : state.back() == VALUE);
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
    begin_value();
    raw("{");
    state.push_back(OBJECT_FIRST);
}

فراغ PrettyJson::begin_array()
{
    begin_value();
    raw("[");
    state.push_back(ARRAY_FIRST);
}

فراغ PrettyJson::end_object()
{
    Scope top_scope = state.back();
    state.pop_back();
    𐤀𐤌 (top_scope == OBJECT)
        line(خطأ);
    وإلا
        log_assert(top_scope == OBJECT_FIRST);
    raw("}");
    end_value();
}

فراغ PrettyJson::end_array()
{
    Scope top_scope = state.back();
    state.pop_back();
    𐤀𐤌 (top_scope == ARRAY)
        line(خطأ);
    وإلا
        log_assert(top_scope == ARRAY_FIRST);
    raw("]");
    end_value();
}

فراغ PrettyJson::name(ثابت حرف *name)
{
    𐤀𐤌 (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        line(خطأ);
    } وإلا {
        raw(",");
        line();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.push_back(VALUE);
}

فراغ PrettyJson::begin_value()
{
    𐤀𐤌 (state.back() == ARRAY_FIRST) {
        line(خطأ);
        state.back() = ARRAY;
    } وإلا 𐤀𐤌 (state.back() == ARRAY) {
        raw(",");
        line();
    } وإلا {
        log_assert(state.back() == VALUE);
        state.pop_back();
    }
}

فراغ PrettyJson::end_value()
{
    𐤀𐤌 (state.empty()) {
        raw("\n");
        flush();
    }
    𐤀𐤌 (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

فراغ PrettyJson::value_json(ثابت Json &value)
{
    begin_value();
    raw(value.dump().c_str());
    end_value();
}

فراغ PrettyJson::entry_json(ثابت حرف *name, ثابت Json &value)
{
    هذا->name(name);
    هذا->value(value);
}
