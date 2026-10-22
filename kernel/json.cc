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

#অন্তর্ভাব "kernel/json.h"

USING_YOSYS_NAMESPACE

শূন্য PrettyJson::emit_to_log()
{
    কাঠামো LogTarget : সর্বজনীন Target {
        শূন্য emit(ধ্রুবক অক্ষর *তথ্য) override { log("%s", তথ্য); }
    };

    targets.ঠেলো_back(std::unique_ptr<Target>(নতুন LogTarget));
}

শূন্য PrettyJson::append_to_string(std::string &target)
{
    কাঠামো AppendStringTarget : সর্বজনীন Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        শূন্য emit(ধ্রুবক অক্ষর *তথ্য) override { target += তথ্য; }
    };

    targets.ঠেলো_back(std::unique_ptr<Target>(নতুন AppendStringTarget(target)));
}

যৌক্তিক PrettyJson::write_to_file(ধ্রুবক std::string &path)
{
    কাঠামো WriteFileTarget : সর্বজনীন Target {
        std::ofstream target;
        শূন্য emit(ধ্রুবক অক্ষর *তথ্য) override { target << তথ্য; }
        শূন্য flush() override { target.flush(); }
    };

    auto target = std::unique_ptr<WriteFileTarget>(নতুন WriteFileTarget);
    target->target.open(path);
    যদি (target->target.fail())
        ফেরত মিথ্যা;
    targets.ঠেলো_back(std::unique_ptr<Target>(target.release()));
    ফেরত সত্য;
}

শূন্য PrettyJson::সারি(যৌক্তিক space_if_inline)
{
    যদি (compact_depth != INT_MAX) {
        যদি (space_if_inline)
            raw(" ");
        ফেরত;
    }
    পূর্ণসংখ্যা indent = state.আকার_2() - (state.empty() ? 0 : state.back() == মান_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

শূন্য PrettyJson::raw(ধ্রুবক অক্ষর *raw_json)
{
    জন্য (auto &target : targets)
        target->emit(raw_json);
}

শূন্য PrettyJson::flush()
{
    জন্য (auto &target : targets)
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
    Scope শীর্ষ_scope = state.back();
    state.বের_করো_back();
    যদি (শীর্ষ_scope == OBJECT)
        সারি(মিথ্যা);
    অন্যথা
        log_assert(শীর্ষ_scope == OBJECT_FIRST);
    raw("}");
    end_মান();
}

শূন্য PrettyJson::end_array()
{
    Scope শীর্ষ_scope = state.back();
    state.বের_করো_back();
    যদি (শীর্ষ_scope == ARRAY)
        সারি(মিথ্যা);
    অন্যথা
        log_assert(শীর্ষ_scope == ARRAY_FIRST);
    raw("]");
    end_মান();
}

শূন্য PrettyJson::name(ধ্রুবক অক্ষর *name)
{
    যদি (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        সারি(মিথ্যা);
    } অন্যথা {
        raw(",");
        সারি();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.ঠেলো_back(মান_2);
}

শূন্য PrettyJson::begin_মান()
{
    যদি (state.back() == ARRAY_FIRST) {
        সারি(মিথ্যা);
        state.back() = ARRAY;
    } অন্যথা যদি (state.back() == ARRAY) {
        raw(",");
        সারি();
    } অন্যথা {
        log_assert(state.back() == মান_2);
        state.বের_করো_back();
    }
}

শূন্য PrettyJson::end_মান()
{
    যদি (state.empty()) {
        raw("\n");
        flush();
    }
    যদি (Get_আকার(state) < compact_depth)
        compact_depth = INT_MAX;
}

শূন্য PrettyJson::মান_json(ধ্রুবক Json &মান)
{
    begin_মান();
    raw(মান.dump().c_str());
    end_মান();
}

শূন্য PrettyJson::entry_json(ধ্রুবক অক্ষর *name, ধ্রুবক Json &মান)
{
    এই->name(name);
    এই->মান(মান);
}
