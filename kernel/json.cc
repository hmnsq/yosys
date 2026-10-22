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

#include "kernel/json.h"

USING_YOSYS_NAMESPACE

void PrettyJson::emit_to_log()
{
    sa_nracanaa LogTarget : public Target {
        void emit(const var_na *data) override { log("%s", data); }
    };

    targets.push_back(std::unique_ptr<Target>(new LogTarget));
}

void PrettyJson::append_to_string(std::string &target)
{
    sa_nracanaa AppendStringTarget : public Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        void emit(const var_na *data) override { target += data; }
    };

    targets.push_back(std::unique_ptr<Target>(new AppendStringTarget(target)));
}

bool PrettyJson::write_to_file(const std::string &path)
{
    sa_nracanaa WriteFileTarget : public Target {
        std::ofstream target;
        void emit(const var_na *data) override { target << data; }
        void flush() override { target.flush(); }
    };

    auto target = std::unique_ptr<WriteFileTarget>(new WriteFileTarget);
    target->target.open(path);
    ถ้า (target->target.fail())
        คืน false;
    targets.push_back(std::unique_ptr<Target>(target.release()));
    คืน true;
}

void PrettyJson::line(bool space_if_inline)
{
    ถ้า (compact_depth != INT_MAX) {
        ถ้า (space_if_inline)
            raw(" ");
        คืน;
    }
    จำนวนเต็ม indent = state.size() - (state.empty() ? 0 : state.back() == VALUE);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

void PrettyJson::raw(const var_na *raw_json)
{
    สำหรับ (auto &target : targets)
        target->emit(raw_json);
}

void PrettyJson::flush()
{
    สำหรับ (auto &target : targets)
        target->flush();
}

void PrettyJson::begin_object()
{
    begin_value();
    raw("{");
    state.push_back(OBJECT_FIRST);
}

void PrettyJson::begin_array()
{
    begin_value();
    raw("[");
    state.push_back(ARRAY_FIRST);
}

void PrettyJson::end_object()
{
    Scope top_scope = state.back();
    state.pop_back();
    ถ้า (top_scope == OBJECT)
        line(false);
    มิฉะนั้น
        log_assert(top_scope == OBJECT_FIRST);
    raw("}");
    end_value();
}

void PrettyJson::end_array()
{
    Scope top_scope = state.back();
    state.pop_back();
    ถ้า (top_scope == ARRAY)
        line(false);
    มิฉะนั้น
        log_assert(top_scope == ARRAY_FIRST);
    raw("]");
    end_value();
}

void PrettyJson::name(const var_na *name)
{
    ถ้า (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        line(false);
    } มิฉะนั้น {
        raw(",");
        line();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.push_back(VALUE);
}

void PrettyJson::begin_value()
{
    ถ้า (state.back() == ARRAY_FIRST) {
        line(false);
        state.back() = ARRAY;
    } มิฉะนั้น ถ้า (state.back() == ARRAY) {
        raw(",");
        line();
    } มิฉะนั้น {
        log_assert(state.back() == VALUE);
        state.pop_back();
    }
}

void PrettyJson::end_value()
{
    ถ้า (state.empty()) {
        raw("\n");
        flush();
    }
    ถ้า (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

void PrettyJson::value_json(const Json &value)
{
    begin_value();
    raw(value.dump().c_str());
    end_value();
}

void PrettyJson::entry_json(const var_na *name, const Json &value)
{
    this->name(name);
    this->value(value);
}
