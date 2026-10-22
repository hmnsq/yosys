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

#यदि_अपरिभाषितम् JSON_H
#परिभाषय JSON_H

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "libs/json11/json11.hpp"
#अन्तर्भाव <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    गणना Scope {
        मानम्_2,
        OBJECT_FIRST,
        OBJECT,
        ARRAY_FIRST,
        ARRAY,
    };

    संरचना Target {
        आभास शून्य emit(ध्रुव वर्ण *दत्त) = 0;
        आभास शून्य flush() {};
        आभास ~Target() {};
    };

    std::string newline_indent = "\n";
    std::सदिशम्<std::unique_ptr<Target>> targets;
    std::सदिशम्<Scope> state = {मानम्_2};
    पूर्णाङ्क compact_गभीरता = अधिकतम_पूर्णाङ्कः;
सार्वजनिक:

    शून्य emit_गन्तव्यम्_log();
    शून्य append_गन्तव्यम्_string(std::string &target);
    bool write_गन्तव्यम्_file(ध्रुव std::string &मार्गः_2);

    bool सक्रिय() { प्रत्यागम !targets.रिक्तानि(); }

    शून्य compact() { compact_गभीरता = Get_आकार(state); }

    शून्य पङ्क्तिः(bool space_if_inline = true);
    शून्य raw(ध्रुव वर्ण *raw_json);
    शून्य flush();
    शून्य begin_object();
    शून्य begin_array();
    शून्य अन्तः_object();
    शून्य अन्तः_array();
    शून्य name(ध्रुव वर्ण *name);
    शून्य begin_मानम्();
    शून्य अन्तः_मानम्();
    शून्य मानम्_json(ध्रुव Json &मानम्);
    शून्य मानम्(अचिह्नित पूर्णाङ्क मानम्) { मानम्_json(Json((पूर्णाङ्क)मानम्)); }
    प्रतिरूपम्<typename T>
    शून्य मानम्(T &&मानम्) { मानम्_json(Json(std::forward<T>(मानम्))); };

    शून्य प्रविष्टिः_json(ध्रुव वर्ण *name, ध्रुव Json &मानम्);
    शून्य प्रविष्टिः(ध्रुव वर्ण *name, अचिह्नित पूर्णाङ्क मानम्) { प्रविष्टिः_json(name, Json((पूर्णाङ्क)मानम्)); }
    प्रतिरूपम्<typename T>
    शून्य प्रविष्टिः(ध्रुव वर्ण *name, T &&मानम्) { प्रविष्टिः_json(name, Json(std::forward<T>(मानम्))); };

    प्रतिरूपम्<typename T>
    शून्य object(ध्रुव T &&मानानि)
    {
        begin_object();
        प्रति (स्वचालित &item : मानानि)
            प्रविष्टिः(item.first, item.second);
        अन्तः_object();
    }

    प्रतिरूपम्<typename T>
    शून्य array(ध्रुव T &&मानानि)
    {
        begin_array();
        प्रति (स्वचालित &item : मानानि)
            मानम्(item);
        अन्तः_array();
    }
};



YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
