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
        शून्य emit(ध्रुव वर्ण *data) override { log("%s", data); }
    };

    targets.प्रक्षिप_back(std::unique_ptr<Target>(नूतन LogTarget));
}

शून्य PrettyJson::append_to_string(std::string &target)
{
    संरचना AppendStringTarget : सार्वजनिक Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        शून्य emit(ध्रुव वर्ण *data) override { target += data; }
    };

    targets.प्रक्षिप_back(std::unique_ptr<Target>(नूतन AppendStringTarget(target)));
}

तार्किक PrettyJson::write_to_file(ध्रुव std::string &path)
{
    संरचना WriteFileTarget : सार्वजनिक Target {
        std::ofstream target;
        शून्य emit(ध्रुव वर्ण *data) override { target << data; }
        शून्य flush() override { target.flush(); }
    };

    auto target = std::unique_ptr<WriteFileTarget>(नूतन WriteFileTarget);
    target->target.open(path);
    जइ (target->target.fail())
        प्रत्यागम मिच्छ;
    targets.प्रक्षिप_back(std::unique_ptr<Target>(target.release()));
    प्रत्यागम सच्च;
}

शून्य PrettyJson::पङ्क्तिः(तार्किक space_if_inline)
{
    जइ (compact_depth != INT_MAX) {
        जइ (space_if_inline)
            raw(" ");
        प्रत्यागम;
    }
    पूर्णाङ्क indent = state.size() - (state.empty() ? 0 : state.back() == मानम्_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

शून्य PrettyJson::raw(ध्रुव वर्ण *raw_json)
{
    प्रति (auto &target : targets)
        target->emit(raw_json);
}

शून्य PrettyJson::flush()
{
    प्रति (auto &target : targets)
        target->flush();
}

शून्य PrettyJson::begin_object()
{
    begin_मानम्();
    raw("{");
    state.प्रक्षिप_back(OBJECT_FIRST);
}

शून्य PrettyJson::begin_array()
{
    begin_मानम्();
    raw("[");
    state.प्रक्षिप_back(ARRAY_FIRST);
}

शून्य PrettyJson::end_object()
{
    Scope शिखरम्_scope = state.back();
    state.निष्कासय_back();
    जइ (शिखरम्_scope == OBJECT)
        पङ्क्तिः(मिच्छ);
    अण्णहा
        log_assert(शिखरम्_scope == OBJECT_FIRST);
    raw("}");
    end_मानम्();
}

शून्य PrettyJson::end_array()
{
    Scope शिखरम्_scope = state.back();
    state.निष्कासय_back();
    जइ (शिखरम्_scope == ARRAY)
        पङ्क्तिः(मिच्छ);
    अण्णहा
        log_assert(शिखरम्_scope == ARRAY_FIRST);
    raw("]");
    end_मानम्();
}

शून्य PrettyJson::name(ध्रुव वर्ण *name)
{
    जइ (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        पङ्क्तिः(मिच्छ);
    } अण्णहा {
        raw(",");
        पङ्क्तिः();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.प्रक्षिप_back(मानम्_2);
}

शून्य PrettyJson::begin_मानम्()
{
    जइ (state.back() == ARRAY_FIRST) {
        पङ्क्तिः(मिच्छ);
        state.back() = ARRAY;
    } अण्णहा जइ (state.back() == ARRAY) {
        raw(",");
        पङ्क्तिः();
    } अण्णहा {
        log_assert(state.back() == मानम्_2);
        state.निष्कासय_back();
    }
}

शून्य PrettyJson::end_मानम्()
{
    जइ (state.empty()) {
        raw("\n");
        flush();
    }
    जइ (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

शून्य PrettyJson::मानम्_json(ध्रुव Json &मानम्)
{
    begin_मानम्();
    raw(मानम्.dump().c_str());
    end_मानम्();
}

शून्य PrettyJson::entry_json(ध्रुव वर्ण *name, ध्रुव Json &मानम्)
{
    एतत्->name(name);
    एतत्->मानम्(मानम्);
}
