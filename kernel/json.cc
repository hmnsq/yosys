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

#समावेश "kernel/json.h"

USING_YOSYS_NAMESPACE

व्योम PrettyJson::emit_to_log()
{
    काष्ठा LogTarget : खुला Target {
        व्योम emit(स्थिर अक्षर *तथ्य) override { log("%s", तथ्य); }
    };

    targets.धकेलो_back(std::unique_ptr<Target>(नया LogTarget));
}

व्योम PrettyJson::append_to_string(std::string &target)
{
    काष्ठा AppendStringTarget : खुला Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        व्योम emit(स्थिर अक्षर *तथ्य) override { target += तथ्य; }
    };

    targets.धकेलो_back(std::unique_ptr<Target>(नया AppendStringTarget(target)));
}

bool PrettyJson::write_to_file(स्थिर std::string &path)
{
    काष्ठा WriteFileTarget : खुला Target {
        std::बख_प्रवाह target;
        व्योम emit(स्थिर अक्षर *तथ्य) override { target << तथ्य; }
        व्योम सफाई() override { target.सफाई(); }
    };

    स्वतः target = std::unique_ptr<WriteFileTarget>(नया WriteFileTarget);
    target->target.खोलो(path);
    अगर (target->target.हार())
        वापस false;
    targets.धकेलो_back(std::unique_ptr<Target>(target.release()));
    वापस true;
}

व्योम PrettyJson::पंक्ति(bool space_if_inline)
{
    अगर (compact_depth != पूर्णांक_उच्च) {
        अगर (space_if_inline)
            raw(" ");
        वापस;
    }
    पूर्णांक indent = state.आकार() - (state.empty() ? 0 : state.back() == मूल्य_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

व्योम PrettyJson::raw(स्थिर अक्षर *raw_json)
{
    क्रम (स्वतः &target : targets)
        target->emit(raw_json);
}

व्योम PrettyJson::सफाई()
{
    क्रम (स्वतः &target : targets)
        target->सफाई();
}

व्योम PrettyJson::begin_object()
{
    begin_मूल्य();
    raw("{");
    state.धकेलो_back(OBJECT_FIRST);
}

व्योम PrettyJson::begin_array()
{
    begin_मूल्य();
    raw("[");
    state.धकेलो_back(ARRAY_FIRST);
}

व्योम PrettyJson::end_object()
{
    Scope शीर्ष_scope = state.back();
    state.निकालो_back();
    अगर (शीर्ष_scope == OBJECT)
        पंक्ति(false);
    अन्यथा
        log_assert(शीर्ष_scope == OBJECT_FIRST);
    raw("}");
    end_मूल्य();
}

व्योम PrettyJson::end_array()
{
    Scope शीर्ष_scope = state.back();
    state.निकालो_back();
    अगर (शीर्ष_scope == ARRAY)
        पंक्ति(false);
    अन्यथा
        log_assert(शीर्ष_scope == ARRAY_FIRST);
    raw("]");
    end_मूल्य();
}

व्योम PrettyJson::name(स्थिर अक्षर *name)
{
    अगर (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        पंक्ति(false);
    } अन्यथा {
        raw(",");
        पंक्ति();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.धकेलो_back(मूल्य_2);
}

व्योम PrettyJson::begin_मूल्य()
{
    अगर (state.back() == ARRAY_FIRST) {
        पंक्ति(false);
        state.back() = ARRAY;
    } अन्यथा अगर (state.back() == ARRAY) {
        raw(",");
        पंक्ति();
    } अन्यथा {
        log_assert(state.back() == मूल्य_2);
        state.निकालो_back();
    }
}

व्योम PrettyJson::end_मूल्य()
{
    अगर (state.empty()) {
        raw("\n");
        सफाई();
    }
    अगर (Get_आकार(state) < compact_depth)
        compact_depth = पूर्णांक_उच्च;
}

व्योम PrettyJson::मूल्य_json(स्थिर Json &मूल्य)
{
    begin_मूल्य();
    raw(मूल्य.dump().c_str());
    end_मूल्य();
}

व्योम PrettyJson::entry_json(स्थिर अक्षर *name, स्थिर Json &मूल्य)
{
    यह->name(name);
    यह->मूल्य(मूल्य);
}
