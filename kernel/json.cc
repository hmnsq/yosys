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

#அந்தர்பாவ "kernel/json.h"

USING_YOSYS_NAMESPACE

வெறுமை PrettyJson::emit_to_log()
{
    கட்டமைப்பு LogTarget : பொது Target {
        வெறுமை emit(மாறிலி எழுத்து *data) override { log("%s", data); }
    };

    targets.தள்ளு_back(std::unique_ptr<Target>(புதிய LogTarget));
}

வெறுமை PrettyJson::append_to_string(std::string &target)
{
    கட்டமைப்பு AppendStringTarget : பொது Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        வெறுமை emit(மாறிலி எழுத்து *data) override { target += data; }
    };

    targets.தள்ளு_back(std::unique_ptr<Target>(புதிய AppendStringTarget(target)));
}

தருக்க PrettyJson::write_to_file(மாறிலி std::string &path)
{
    கட்டமைப்பு WriteFileTarget : பொது Target {
        std::ofstream target;
        வெறுமை emit(மாறிலி எழுத்து *data) override { target << data; }
        வெறுமை flush() override { target.flush(); }
    };

    auto target = std::unique_ptr<WriteFileTarget>(புதிய WriteFileTarget);
    target->target.open(path);
    எனில் (target->target.fail())
        திரும்பு பொய்;
    targets.தள்ளு_back(std::unique_ptr<Target>(target.release()));
    திரும்பு உண்மை;
}

வெறுமை PrettyJson::வரி(தருக்க space_if_inline)
{
    எனில் (compact_depth != INT_MAX) {
        எனில் (space_if_inline)
            raw(" ");
        திரும்பு;
    }
    முழு indent = state.size() - (state.empty() ? 0 : state.back() == மதிப்பு_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

வெறுமை PrettyJson::raw(மாறிலி எழுத்து *raw_json)
{
    க்கு (auto &target : targets)
        target->emit(raw_json);
}

வெறுமை PrettyJson::flush()
{
    க்கு (auto &target : targets)
        target->flush();
}

வெறுமை PrettyJson::begin_object()
{
    begin_மதிப்பு();
    raw("{");
    state.தள்ளு_back(OBJECT_FIRST);
}

வெறுமை PrettyJson::begin_array()
{
    begin_மதிப்பு();
    raw("[");
    state.தள்ளு_back(ARRAY_FIRST);
}

வெறுமை PrettyJson::end_object()
{
    Scope உச்சி_scope = state.back();
    state.எடு_back();
    எனில் (உச்சி_scope == OBJECT)
        வரி(பொய்);
    இல்லையெனில்
        log_assert(உச்சி_scope == OBJECT_FIRST);
    raw("}");
    end_மதிப்பு();
}

வெறுமை PrettyJson::end_array()
{
    Scope உச்சி_scope = state.back();
    state.எடு_back();
    எனில் (உச்சி_scope == ARRAY)
        வரி(பொய்);
    இல்லையெனில்
        log_assert(உச்சி_scope == ARRAY_FIRST);
    raw("]");
    end_மதிப்பு();
}

வெறுமை PrettyJson::name(மாறிலி எழுத்து *name)
{
    எனில் (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        வரி(பொய்);
    } இல்லையெனில் {
        raw(",");
        வரி();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.தள்ளு_back(மதிப்பு_2);
}

வெறுமை PrettyJson::begin_மதிப்பு()
{
    எனில் (state.back() == ARRAY_FIRST) {
        வரி(பொய்);
        state.back() = ARRAY;
    } இல்லையெனில் எனில் (state.back() == ARRAY) {
        raw(",");
        வரி();
    } இல்லையெனில் {
        log_assert(state.back() == மதிப்பு_2);
        state.எடு_back();
    }
}

வெறுமை PrettyJson::end_மதிப்பு()
{
    எனில் (state.empty()) {
        raw("\n");
        flush();
    }
    எனில் (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

வெறுமை PrettyJson::மதிப்பு_json(மாறிலி Json &மதிப்பு)
{
    begin_மதிப்பு();
    raw(மதிப்பு.dump().c_str());
    end_மதிப்பு();
}

வெறுமை PrettyJson::entry_json(மாறிலி எழுத்து *name, மாறிலி Json &மதிப்பு)
{
    இது->name(name);
    இது->மதிப்பு(மதிப்பு);
}
