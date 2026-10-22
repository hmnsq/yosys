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
        शून्य emit(अचर वर्ण *data) override { log("%s", data); }
    };

    targets.थप_back(std::unique_ptr<Target>(नूतन LogTarget));
}

शून्य PrettyJson::append_to_string(std::string &target)
{
    संरचना AppendStringTarget : सार्वजनिक Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        शून्य emit(अचर वर्ण *data) override { target += data; }
    };

    targets.थप_back(std::unique_ptr<Target>(नूतन AppendStringTarget(target)));
}

तार्किक PrettyJson::write_to_file(अचर std::string &path)
{
    संरचना WriteFileTarget : सार्वजनिक Target {
        std::ofstream target;
        शून्य emit(अचर वर्ण *data) override { target << data; }
        शून्य flush() override { target.flush(); }
    };

    स्वतः target = std::unique_ptr<WriteFileTarget>(नूतन WriteFileTarget);
    target->target.खोलो(path);
    यदि (target->target.fail())
        फर्काऊ असत्य;
    targets.थप_back(std::unique_ptr<Target>(target.release()));
    फर्काऊ सत्य;
}

शून्य PrettyJson::पङ्क्ति(तार्किक space_if_inline)
{
    यदि (compact_depth != पूर्णांक_उच्च) {
        यदि (space_if_inline)
            raw(" ");
        फर्काऊ;
    }
    पूर्णाङ्क indent = state.size() - (state.empty() ? 0 : state.back() == मान_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

शून्य PrettyJson::raw(अचर वर्ण *raw_json)
{
    लागि (स्वतः &target : targets)
        target->emit(raw_json);
}

शून्य PrettyJson::flush()
{
    लागि (स्वतः &target : targets)
        target->flush();
}

शून्य PrettyJson::begin_object()
{
    begin_मान();
    raw("{");
    state.थप_back(OBJECT_FIRST);
}

शून्य PrettyJson::begin_array()
{
    begin_मान();
    raw("[");
    state.थप_back(ARRAY_FIRST);
}

शून्य PrettyJson::end_object()
{
    Scope शीर्ष_scope = state.back();
    state.झिक_back();
    यदि (शीर्ष_scope == OBJECT)
        पङ्क्ति(असत्य);
    अन्यथा
        log_assert(शीर्ष_scope == OBJECT_FIRST);
    raw("}");
    end_मान();
}

शून्य PrettyJson::end_array()
{
    Scope शीर्ष_scope = state.back();
    state.झिक_back();
    यदि (शीर्ष_scope == ARRAY)
        पङ्क्ति(असत्य);
    अन्यथा
        log_assert(शीर्ष_scope == ARRAY_FIRST);
    raw("]");
    end_मान();
}

शून्य PrettyJson::name(अचर वर्ण *name)
{
    यदि (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        पङ्क्ति(असत्य);
    } अन्यथा {
        raw(",");
        पङ्क्ति();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.थप_back(मान_2);
}

शून्य PrettyJson::begin_मान()
{
    यदि (state.back() == ARRAY_FIRST) {
        पङ्क्ति(असत्य);
        state.back() = ARRAY;
    } अन्यथा यदि (state.back() == ARRAY) {
        raw(",");
        पङ्क्ति();
    } अन्यथा {
        log_assert(state.back() == मान_2);
        state.झिक_back();
    }
}

शून्य PrettyJson::end_मान()
{
    यदि (state.empty()) {
        raw("\n");
        flush();
    }
    यदि (GetSize(state) < compact_depth)
        compact_depth = पूर्णांक_उच्च;
}

शून्य PrettyJson::मान_json(अचर Json &मान)
{
    begin_मान();
    raw(मान.dump().c_str());
    end_मान();
}

शून्य PrettyJson::entry_json(अचर वर्ण *name, अचर Json &मान)
{
    एतत्->name(name);
    एतत्->मान(मान);
}
