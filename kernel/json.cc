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

    targets.پاڵنان_back(std::unique_ptr<Target>(جديد LogTarget));
}

فراغ PrettyJson::append_to_string(std::string &target)
{
    بنية AppendStringTarget : عام Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        فراغ emit(ثابت حرف *data) override { target += data; }
    };

    targets.پاڵنان_back(std::unique_ptr<Target>(جديد AppendStringTarget(target)));
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
    targets.پاڵنان_back(std::unique_ptr<Target>(target.release()));
    أرجع true;
}

فراغ PrettyJson::هێڵ(منطقي space_if_inline)
{
    إذا (compact_depth != INT_MAX) {
        إذا (space_if_inline)
            raw(" ");
        أرجع;
    }
    صحيح indent = state.size() - (state.empty() ? 0 : state.back() == بەها_2);
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
    begin_بەها();
    raw("{");
    state.پاڵنان_back(OBJECT_FIRST);
}

فراغ PrettyJson::begin_array()
{
    begin_بەها();
    raw("[");
    state.پاڵنان_back(ARRAY_FIRST);
}

فراغ PrettyJson::end_object()
{
    Scope لووتکە_scope = state.back();
    state.دەرهێنان_back();
    إذا (لووتکە_scope == OBJECT)
        هێڵ(خطأ);
    وإلا
        log_assert(لووتکە_scope == OBJECT_FIRST);
    raw("}");
    end_بەها();
}

فراغ PrettyJson::end_array()
{
    Scope لووتکە_scope = state.back();
    state.دەرهێنان_back();
    إذا (لووتکە_scope == ARRAY)
        هێڵ(خطأ);
    وإلا
        log_assert(لووتکە_scope == ARRAY_FIRST);
    raw("]");
    end_بەها();
}

فراغ PrettyJson::name(ثابت حرف *name)
{
    إذا (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        هێڵ(خطأ);
    } وإلا {
        raw(",");
        هێڵ();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.پاڵنان_back(بەها_2);
}

فراغ PrettyJson::begin_بەها()
{
    إذا (state.back() == ARRAY_FIRST) {
        هێڵ(خطأ);
        state.back() = ARRAY;
    } وإلا إذا (state.back() == ARRAY) {
        raw(",");
        هێڵ();
    } وإلا {
        log_assert(state.back() == بەها_2);
        state.دەرهێنان_back();
    }
}

فراغ PrettyJson::end_بەها()
{
    إذا (state.empty()) {
        raw("\n");
        flush();
    }
    إذا (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

فراغ PrettyJson::بەها_json(ثابت Json &بەها)
{
    begin_بەها();
    raw(بەها.dump().c_str());
    end_بەها();
}

فراغ PrettyJson::entry_json(ثابت حرف *name, ثابت Json &بەها)
{
    هذا->name(name);
    هذا->بەها(بەها);
}
