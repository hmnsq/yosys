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

शून्य PrettyJson::emit_गन्तव्यम्_log()
{
    संरचना LogTarget : सार्वजनिक Target {
        शून्य emit(ध्रुव वर्ण *दत्त) override { लघुगणकः("%s", दत्त); }
    };

    targets.प्रक्षिप_पृष्ठभागः(std::unique_ptr<Target>(नूतन LogTarget));
}

शून्य PrettyJson::append_गन्तव्यम्_string(std::string &target)
{
    संरचना AppendStringTarget : सार्वजनिक Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        शून्य emit(ध्रुव वर्ण *दत्त) override { target += दत्त; }
    };

    targets.प्रक्षिप_पृष्ठभागः(std::unique_ptr<Target>(नूतन AppendStringTarget(target)));
}

तार्किक PrettyJson::write_गन्तव्यम्_file(ध्रुव std::string &मार्गः_2)
{
    संरचना WriteFileTarget : सार्वजनिक Target {
        std::ofstream target;
        शून्य emit(ध्रुव वर्ण *दत्त) override { target << दत्त; }
        शून्य flush() override { target.flush(); }
    };

    स्वचालित target = std::unique_ptr<WriteFileTarget>(नूतन WriteFileTarget);
    target->target.उद्घाटय(मार्गः_2);
    यदि (target->target.fail())
        प्रत्यागम असत्य;
    targets.प्रक्षिप_पृष्ठभागः(std::unique_ptr<Target>(target.release()));
    प्रत्यागम सत्य;
}

शून्य PrettyJson::पङ्क्तिः(तार्किक space_if_inline)
{
    यदि (compact_गभीरता != अधिकतम_पूर्णाङ्कः) {
        यदि (space_if_inline)
            raw(" ");
        प्रत्यागम;
    }
    पूर्णाङ्क indent = state.आकार() - (state.रिक्तानि() ? 0 : state.पृष्ठभागः() == मानम्_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

शून्य PrettyJson::raw(ध्रुव वर्ण *raw_json)
{
    प्रति (स्वचालित &target : targets)
        target->emit(raw_json);
}

शून्य PrettyJson::flush()
{
    प्रति (स्वचालित &target : targets)
        target->flush();
}

शून्य PrettyJson::begin_object()
{
    begin_मानम्();
    raw("{");
    state.प्रक्षिप_पृष्ठभागः(OBJECT_FIRST);
}

शून्य PrettyJson::begin_array()
{
    begin_मानम्();
    raw("[");
    state.प्रक्षिप_पृष्ठभागः(ARRAY_FIRST);
}

शून्य PrettyJson::अन्तः_object()
{
    Scope शिखरम्_scope = state.पृष्ठभागः();
    state.निष्कासय_पृष्ठभागः();
    यदि (शिखरम्_scope == OBJECT)
        पङ्क्तिः(असत्य);
    अन्यथा
        log_assert(शिखरम्_scope == OBJECT_FIRST);
    raw("}");
    अन्तः_मानम्();
}

शून्य PrettyJson::अन्तः_array()
{
    Scope शिखरम्_scope = state.पृष्ठभागः();
    state.निष्कासय_पृष्ठभागः();
    यदि (शिखरम्_scope == ARRAY)
        पङ्क्तिः(असत्य);
    अन्यथा
        log_assert(शिखरम्_scope == ARRAY_FIRST);
    raw("]");
    अन्तः_मानम्();
}

शून्य PrettyJson::name(ध्रुव वर्ण *name)
{
    यदि (state.पृष्ठभागः() == OBJECT_FIRST) {
        state.पृष्ठभागः() = OBJECT;
        पङ्क्तिः(असत्य);
    } अन्यथा {
        raw(",");
        पङ्क्तिः();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.प्रक्षिप_पृष्ठभागः(मानम्_2);
}

शून्य PrettyJson::begin_मानम्()
{
    यदि (state.पृष्ठभागः() == ARRAY_FIRST) {
        पङ्क्तिः(असत्य);
        state.पृष्ठभागः() = ARRAY;
    } अन्यथा यदि (state.पृष्ठभागः() == ARRAY) {
        raw(",");
        पङ्क्तिः();
    } अन्यथा {
        log_assert(state.पृष्ठभागः() == मानम्_2);
        state.निष्कासय_पृष्ठभागः();
    }
}

शून्य PrettyJson::अन्तः_मानम्()
{
    यदि (state.रिक्तानि()) {
        raw("\n");
        flush();
    }
    यदि (Get_आकार(state) < compact_गभीरता)
        compact_गभीरता = अधिकतम_पूर्णाङ्कः;
}

शून्य PrettyJson::मानम्_json(ध्रुव Json &मानम्)
{
    begin_मानम्();
    raw(मानम्.dump().c_str());
    अन्तः_मानम्();
}

शून्य PrettyJson::प्रविष्टिः_json(ध्रुव वर्ण *name, ध्रुव Json &मानम्)
{
    एतत्->name(name);
    एतत्->मानम्(मानम्);
}
