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

    targets.تور_back(std::unique_ptr<Target>(جديد LogTarget));
}

فراغ PrettyJson::append_to_string(std::string &target)
{
    بنية AppendStringTarget : عام Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        فراغ emit(ثابت حرف *data) override { target += data; }
    };

    targets.تور_back(std::unique_ptr<Target>(جديد AppendStringTarget(target)));
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
    targets.تور_back(std::unique_ptr<Target>(target.release()));
    أرجع true;
}

فراغ PrettyJson::لي(منطقي space_if_inline)
{
    إذا (compact_depth != INT_MAX) {
        إذا (space_if_inline)
            raw(" ");
        أرجع;
    }
    صحيح indent = state.size() - (state.empty() ? 0 : state.back() == درج_2);
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
    begin_درج();
    raw("{");
    state.تور_back(OBJECT_FIRST);
}

فراغ PrettyJson::begin_array()
{
    begin_درج();
    raw("[");
    state.تور_back(ARRAY_FIRST);
}

فراغ PrettyJson::end_object()
{
    Scope كولي_scope = state.back();
    state.چرو_back();
    إذا (كولي_scope == OBJECT)
        لي(خطأ);
    وإلا
        log_assert(كولي_scope == OBJECT_FIRST);
    raw("}");
    end_درج();
}

فراغ PrettyJson::end_array()
{
    Scope كولي_scope = state.back();
    state.چرو_back();
    إذا (كولي_scope == ARRAY)
        لي(خطأ);
    وإلا
        log_assert(كولي_scope == ARRAY_FIRST);
    raw("]");
    end_درج();
}

فراغ PrettyJson::name(ثابت حرف *name)
{
    إذا (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        لي(خطأ);
    } وإلا {
        raw(",");
        لي();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.تور_back(درج_2);
}

فراغ PrettyJson::begin_درج()
{
    إذا (state.back() == ARRAY_FIRST) {
        لي(خطأ);
        state.back() = ARRAY;
    } وإلا إذا (state.back() == ARRAY) {
        raw(",");
        لي();
    } وإلا {
        log_assert(state.back() == درج_2);
        state.چرو_back();
    }
}

فراغ PrettyJson::end_درج()
{
    إذا (state.empty()) {
        raw("\n");
        flush();
    }
    إذا (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

فراغ PrettyJson::درج_json(ثابت Json &درج)
{
    begin_درج();
    raw(درج.dump().c_str());
    end_درج();
}

فراغ PrettyJson::entry_json(ثابت حرف *name, ثابت Json &درج)
{
    هذا->name(name);
    هذا->درج(درج);
}
