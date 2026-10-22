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

تهی PrettyJson::emit_to_log()
{
    ساختار LogTarget : عمومی Target {
        تهی emit(ثابت نویسه *data) override { log("%s", data); }
    };

    targets.افزودن_back(std::unique_ptr<Target>(جدید LogTarget));
}

تهی PrettyJson::append_to_string(std::string &target)
{
    ساختار AppendStringTarget : عمومی Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        تهی emit(ثابت نویسه *data) override { target += data; }
    };

    targets.افزودن_back(std::unique_ptr<Target>(جدید AppendStringTarget(target)));
}

منطقی PrettyJson::write_to_file(ثابت std::string &path)
{
    ساختار WriteFileTarget : عمومی Target {
        std::ofstream target;
        تهی emit(ثابت نویسه *data) override { target << data; }
        تهی flush() override { target.flush(); }
    };

    auto target = std::unique_ptr<WriteFileTarget>(جدید WriteFileTarget);
    target->target.open(path);
    اگر (target->target.fail())
        بازگشت نادرست;
    targets.افزودن_back(std::unique_ptr<Target>(target.release()));
    بازگشت درست;
}

تهی PrettyJson::سطر(منطقی space_if_inline)
{
    اگر (compact_depth != INT_MAX) {
        اگر (space_if_inline)
            raw(" ");
        بازگشت;
    }
    صحیح indent = state.size() - (state.empty() ? 0 : state.back() == مقدار_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

تهی PrettyJson::raw(ثابت نویسه *raw_json)
{
    برای (auto &target : targets)
        target->emit(raw_json);
}

تهی PrettyJson::flush()
{
    برای (auto &target : targets)
        target->flush();
}

تهی PrettyJson::begin_object()
{
    begin_مقدار();
    raw("{");
    state.افزودن_back(OBJECT_FIRST);
}

تهی PrettyJson::begin_array()
{
    begin_مقدار();
    raw("[");
    state.افزودن_back(ARRAY_FIRST);
}

تهی PrettyJson::end_object()
{
    Scope بالاترین_scope = state.back();
    state.برداشت_back();
    اگر (بالاترین_scope == OBJECT)
        سطر(نادرست);
    وگرنه
        log_assert(بالاترین_scope == OBJECT_FIRST);
    raw("}");
    end_مقدار();
}

تهی PrettyJson::end_array()
{
    Scope بالاترین_scope = state.back();
    state.برداشت_back();
    اگر (بالاترین_scope == ARRAY)
        سطر(نادرست);
    وگرنه
        log_assert(بالاترین_scope == ARRAY_FIRST);
    raw("]");
    end_مقدار();
}

تهی PrettyJson::name(ثابت نویسه *name)
{
    اگر (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        سطر(نادرست);
    } وگرنه {
        raw(",");
        سطر();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.افزودن_back(مقدار_2);
}

تهی PrettyJson::begin_مقدار()
{
    اگر (state.back() == ARRAY_FIRST) {
        سطر(نادرست);
        state.back() = ARRAY;
    } وگرنه اگر (state.back() == ARRAY) {
        raw(",");
        سطر();
    } وگرنه {
        log_assert(state.back() == مقدار_2);
        state.برداشت_back();
    }
}

تهی PrettyJson::end_مقدار()
{
    اگر (state.empty()) {
        raw("\n");
        flush();
    }
    اگر (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

تهی PrettyJson::مقدار_json(ثابت Json &مقدار)
{
    begin_مقدار();
    raw(مقدار.dump().c_str());
    end_مقدار();
}

تهی PrettyJson::entry_json(ثابت نویسه *name, ثابت Json &مقدار)
{
    این->name(name);
    این->مقدار(مقدار);
}
