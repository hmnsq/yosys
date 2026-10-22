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

#ಅನ್ತರ್ಭಾವ "kernel/json.h"

USING_YOSYS_NAMESPACE

ಶೂನ್ಯ PrettyJson::emit_to_log()
{
    ಸಂರಚನಾ LogTarget : ಸಾರ್ವಜನಿಕ Target {
        ಶೂನ್ಯ emit(ಧ್ರುವ ವರ್ಣ *data) override { log("%s", data); }
    };

    targets.ತಳ್ಳು_back(std::unique_ptr<Target>(ನೂತನ LogTarget));
}

ಶೂನ್ಯ PrettyJson::append_to_string(std::string &target)
{
    ಸಂರಚನಾ AppendStringTarget : ಸಾರ್ವಜನಿಕ Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        ಶೂನ್ಯ emit(ಧ್ರುವ ವರ್ಣ *data) override { target += data; }
    };

    targets.ತಳ್ಳು_back(std::unique_ptr<Target>(ನೂತನ AppendStringTarget(target)));
}

ತಾರ್ಕಿಕ PrettyJson::write_to_file(ಧ್ರುವ std::string &path)
{
    ಸಂರಚನಾ WriteFileTarget : ಸಾರ್ವಜನಿಕ Target {
        std::ofstream target;
        ಶೂನ್ಯ emit(ಧ್ರುವ ವರ್ಣ *data) override { target << data; }
        ಶೂನ್ಯ flush() override { target.flush(); }
    };

    auto target = std::unique_ptr<WriteFileTarget>(ನೂತನ WriteFileTarget);
    target->target.open(path);
    ಯದಿ (target->target.fail())
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    targets.ತಳ್ಳು_back(std::unique_ptr<Target>(target.release()));
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಶೂನ್ಯ PrettyJson::ಸಾಲು(ತಾರ್ಕಿಕ space_if_inline)
{
    ಯದಿ (compact_depth != INT_MAX) {
        ಯದಿ (space_if_inline)
            raw(" ");
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಪೂರ್ಣಾಙ್ಕ indent = state.size() - (state.empty() ? 0 : state.back() == ಮೌಲ್ಯ_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

ಶೂನ್ಯ PrettyJson::raw(ಧ್ರುವ ವರ್ಣ *raw_json)
{
    ಪ್ರತಿ (auto &target : targets)
        target->emit(raw_json);
}

ಶೂನ್ಯ PrettyJson::flush()
{
    ಪ್ರತಿ (auto &target : targets)
        target->flush();
}

ಶೂನ್ಯ PrettyJson::begin_object()
{
    begin_ಮೌಲ್ಯ();
    raw("{");
    state.ತಳ್ಳು_back(OBJECT_FIRST);
}

ಶೂನ್ಯ PrettyJson::begin_array()
{
    begin_ಮೌಲ್ಯ();
    raw("[");
    state.ತಳ್ಳು_back(ARRAY_FIRST);
}

ಶೂನ್ಯ PrettyJson::end_object()
{
    Scope ತುದಿ_scope = state.back();
    state.ತೆಗೆ_back();
    ಯದಿ (ತುದಿ_scope == OBJECT)
        ಸಾಲು(ಅಸತ್ಯ);
    ಅನ್ಯಥಾ
        log_assert(ತುದಿ_scope == OBJECT_FIRST);
    raw("}");
    end_ಮೌಲ್ಯ();
}

ಶೂನ್ಯ PrettyJson::end_array()
{
    Scope ತುದಿ_scope = state.back();
    state.ತೆಗೆ_back();
    ಯದಿ (ತುದಿ_scope == ARRAY)
        ಸಾಲು(ಅಸತ್ಯ);
    ಅನ್ಯಥಾ
        log_assert(ತುದಿ_scope == ARRAY_FIRST);
    raw("]");
    end_ಮೌಲ್ಯ();
}

ಶೂನ್ಯ PrettyJson::name(ಧ್ರುವ ವರ್ಣ *name)
{
    ಯದಿ (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        ಸಾಲು(ಅಸತ್ಯ);
    } ಅನ್ಯಥಾ {
        raw(",");
        ಸಾಲು();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.ತಳ್ಳು_back(ಮೌಲ್ಯ_2);
}

ಶೂನ್ಯ PrettyJson::begin_ಮೌಲ್ಯ()
{
    ಯದಿ (state.back() == ARRAY_FIRST) {
        ಸಾಲು(ಅಸತ್ಯ);
        state.back() = ARRAY;
    } ಅನ್ಯಥಾ ಯದಿ (state.back() == ARRAY) {
        raw(",");
        ಸಾಲು();
    } ಅನ್ಯಥಾ {
        log_assert(state.back() == ಮೌಲ್ಯ_2);
        state.ತೆಗೆ_back();
    }
}

ಶೂನ್ಯ PrettyJson::end_ಮೌಲ್ಯ()
{
    ಯದಿ (state.empty()) {
        raw("\n");
        flush();
    }
    ಯದಿ (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

ಶೂನ್ಯ PrettyJson::ಮೌಲ್ಯ_json(ಧ್ರುವ Json &ಮೌಲ್ಯ)
{
    begin_ಮೌಲ್ಯ();
    raw(ಮೌಲ್ಯ.dump().c_str());
    end_ಮೌಲ್ಯ();
}

ಶೂನ್ಯ PrettyJson::entry_json(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ Json &ಮೌಲ್ಯ)
{
    ಏತತ್->name(name);
    ಏತತ್->ಮೌಲ್ಯ(ಮೌಲ್ಯ);
}
