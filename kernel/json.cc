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

#अन्तर्भाव "kernel/json.h"

USING_YOSYS_NAMESPACE

शून्य PrettyJson::emit_to_log()
{
    संरचना LogTarget : सार्वजनिक Target {
        शून्य emit(स्थिरांक अक्षर *data) override { log("%s", data); }
    };

    targets.ढकला_back(std::unique_ptr<Target>(नवीन LogTarget));
}

शून्य PrettyJson::append_to_string(std::string &target)
{
    संरचना AppendStringTarget : सार्वजनिक Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        शून्य emit(स्थिरांक अक्षर *data) override { target += data; }
    };

    targets.ढकला_back(std::unique_ptr<Target>(नवीन AppendStringTarget(target)));
}

तार्किक PrettyJson::write_to_file(स्थिरांक std::string &path)
{
    संरचना WriteFileTarget : सार्वजनिक Target {
        std::ofstream target;
        शून्य emit(स्थिरांक अक्षर *data) override { target << data; }
        शून्य flush() override { target.flush(); }
    };

    स्वतः target = std::unique_ptr<WriteFileTarget>(नवीन WriteFileTarget);
    target->target.खोलो(path);
    जर (target->target.fail())
        परत असत्य;
    targets.ढकला_back(std::unique_ptr<Target>(target.release()));
    परत सत्य;
}

शून्य PrettyJson::ओळ(तार्किक space_if_inline)
{
    जर (compact_depth != पूर्णांक_उच्च) {
        जर (space_if_inline)
            raw(" ");
        परत;
    }
    पूर्णांक indent = state.size() - (state.empty() ? 0 : state.back() == मूल्य_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

शून्य PrettyJson::raw(स्थिरांक अक्षर *raw_json)
{
    साठी (स्वतः &target : targets)
        target->emit(raw_json);
}

शून्य PrettyJson::flush()
{
    साठी (स्वतः &target : targets)
        target->flush();
}

शून्य PrettyJson::begin_object()
{
    begin_मूल्य();
    raw("{");
    state.ढकला_back(OBJECT_FIRST);
}

शून्य PrettyJson::begin_array()
{
    begin_मूल्य();
    raw("[");
    state.ढकला_back(ARRAY_FIRST);
}

शून्य PrettyJson::end_object()
{
    Scope शीर्ष_scope = state.back();
    state.काढा_back();
    जर (शीर्ष_scope == OBJECT)
        ओळ(असत्य);
    अन्यथा
        log_assert(शीर्ष_scope == OBJECT_FIRST);
    raw("}");
    end_मूल्य();
}

शून्य PrettyJson::end_array()
{
    Scope शीर्ष_scope = state.back();
    state.काढा_back();
    जर (शीर्ष_scope == ARRAY)
        ओळ(असत्य);
    अन्यथा
        log_assert(शीर्ष_scope == ARRAY_FIRST);
    raw("]");
    end_मूल्य();
}

शून्य PrettyJson::name(स्थिरांक अक्षर *name)
{
    जर (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        ओळ(असत्य);
    } अन्यथा {
        raw(",");
        ओळ();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.ढकला_back(मूल्य_2);
}

शून्य PrettyJson::begin_मूल्य()
{
    जर (state.back() == ARRAY_FIRST) {
        ओळ(असत्य);
        state.back() = ARRAY;
    } अन्यथा जर (state.back() == ARRAY) {
        raw(",");
        ओळ();
    } अन्यथा {
        log_assert(state.back() == मूल्य_2);
        state.काढा_back();
    }
}

शून्य PrettyJson::end_मूल्य()
{
    जर (state.empty()) {
        raw("\n");
        flush();
    }
    जर (GetSize(state) < compact_depth)
        compact_depth = पूर्णांक_उच्च;
}

शून्य PrettyJson::मूल्य_json(स्थिरांक Json &मूल्य)
{
    begin_मूल्य();
    raw(मूल्य.dump().c_str());
    end_मूल्य();
}

शून्य PrettyJson::entry_json(स्थिरांक अक्षर *name, स्थिरांक Json &मूल्य)
{
    हे->name(name);
    हे->मूल्य(मूल्य);
}
