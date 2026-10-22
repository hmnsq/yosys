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

#অন্তর্ভাৱ "kernel/json.h"

USING_YOSYS_NAMESPACE

শূন্য PrettyJson::emit_to_log()
{
    সংরচনা LogTarget : সার্ৱজনিক Target {
        শূন্য emit(ধ্রুৱ ৱর্ণ *data) override { log("%s", data); }
    };

    targets.ঠেলো_back(std::unique_ptr<Target>(নূতন LogTarget));
}

শূন্য PrettyJson::append_to_string(std::string &target)
{
    সংরচনা AppendStringTarget : সার্ৱজনিক Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        শূন্য emit(ধ্রুৱ ৱর্ণ *data) override { target += data; }
    };

    targets.ঠেলো_back(std::unique_ptr<Target>(নূতন AppendStringTarget(target)));
}

তার্কিক PrettyJson::write_to_file(ধ্রুৱ std::string &path)
{
    সংরচনা WriteFileTarget : সার্ৱজনিক Target {
        std::ofstream target;
        শূন্য emit(ধ্রুৱ ৱর্ণ *data) override { target << data; }
        শূন্য flush() override { target.flush(); }
    };

    auto target = std::unique_ptr<WriteFileTarget>(নূতন WriteFileTarget);
    target->target.open(path);
    যদি (target->target.fail())
        ঘূৰাই মিছা;
    targets.ঠেলো_back(std::unique_ptr<Target>(target.release()));
    ঘূৰাই সঁচা;
}

শূন্য PrettyJson::সাৰি(তার্কিক space_if_inline)
{
    যদি (compact_depth != INT_MAX) {
        যদি (space_if_inline)
            raw(" ");
        ঘূৰাই;
    }
    পূর্ণাঙ্ক indent = state.size() - (state.empty() ? 0 : state.back() == মান_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

শূন্য PrettyJson::raw(ধ্রুৱ ৱর্ণ *raw_json)
{
    বাবে (auto &target : targets)
        target->emit(raw_json);
}

শূন্য PrettyJson::flush()
{
    বাবে (auto &target : targets)
        target->flush();
}

শূন্য PrettyJson::begin_object()
{
    begin_মান();
    raw("{");
    state.ঠেলো_back(OBJECT_FIRST);
}

শূন্য PrettyJson::begin_array()
{
    begin_মান();
    raw("[");
    state.ঠেলো_back(ARRAY_FIRST);
}

শূন্য PrettyJson::end_object()
{
    Scope শীৰ্ষ_scope = state.back();
    state.বেৰ_কৰো_back();
    যদি (শীৰ্ষ_scope == OBJECT)
        সাৰি(মিছা);
    অন্যথা
        log_assert(শীৰ্ষ_scope == OBJECT_FIRST);
    raw("}");
    end_মান();
}

শূন্য PrettyJson::end_array()
{
    Scope শীৰ্ষ_scope = state.back();
    state.বেৰ_কৰো_back();
    যদি (শীৰ্ষ_scope == ARRAY)
        সাৰি(মিছা);
    অন্যথা
        log_assert(শীৰ্ষ_scope == ARRAY_FIRST);
    raw("]");
    end_মান();
}

শূন্য PrettyJson::name(ধ্রুৱ ৱর্ণ *name)
{
    যদি (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        সাৰি(মিছা);
    } অন্যথা {
        raw(",");
        সাৰি();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.ঠেলো_back(মান_2);
}

শূন্য PrettyJson::begin_মান()
{
    যদি (state.back() == ARRAY_FIRST) {
        সাৰি(মিছা);
        state.back() = ARRAY;
    } অন্যথা যদি (state.back() == ARRAY) {
        raw(",");
        সাৰি();
    } অন্যথা {
        log_assert(state.back() == মান_2);
        state.বেৰ_কৰো_back();
    }
}

শূন্য PrettyJson::end_মান()
{
    যদি (state.empty()) {
        raw("\n");
        flush();
    }
    যদি (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

শূন্য PrettyJson::মান_json(ধ্রুৱ Json &মান)
{
    begin_মান();
    raw(মান.dump().c_str());
    end_মান();
}

শূন্য PrettyJson::entry_json(ধ্রুৱ ৱর্ণ *name, ধ্রুৱ Json &মান)
{
    এতত্->name(name);
    এতত্->মান(মান);
}
