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

#ifndef JSON_H
#define JSON_H

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "libs/json11/json11.hpp"
#अन्तर्भाव <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    गणना Scope {
        मान_2,
        OBJECT_FIRST,
        OBJECT,
        ARRAY_FIRST,
        ARRAY,
    };

    संरचना Target {
        आभास शून्य emit(अचर वर्ण *data) = 0;
        आभास शून्य flush() {};
        आभास ~Target() {};
    };

    std::string newline_indent = "\n";
    std::vector<std::unique_ptr<Target>> targets;
    std::vector<Scope> state = {मान_2};
    पूर्णाङ्क compact_depth = पूर्णांक_उच्च;
सार्वजनिक:

    शून्य emit_to_log();
    शून्य append_to_string(std::string &target);
    bool write_to_file(अचर std::string &path);

    bool active() { फर्काऊ !targets.empty(); }

    शून्य compact() { compact_depth = GetSize(state); }

    शून्य पङ्क्ति(bool space_if_inline = true);
    शून्य raw(अचर वर्ण *raw_json);
    शून्य flush();
    शून्य begin_object();
    शून्य begin_array();
    शून्य end_object();
    शून्य end_array();
    शून्य name(अचर वर्ण *name);
    शून्य begin_मान();
    शून्य end_मान();
    शून्य मान_json(अचर Json &मान);
    शून्य मान(अचिन्हित पूर्णाङ्क मान) { मान_json(Json((पूर्णाङ्क)मान)); }
    ढाँचा<typename T>
    शून्य मान(T &&मान) { मान_json(Json(std::forward<T>(मान))); };

    शून्य entry_json(अचर वर्ण *name, अचर Json &मान);
    शून्य entry(अचर वर्ण *name, अचिन्हित पूर्णाङ्क मान) { entry_json(name, Json((पूर्णाङ्क)मान)); }
    ढाँचा<typename T>
    शून्य entry(अचर वर्ण *name, T &&मान) { entry_json(name, Json(std::forward<T>(मान))); };

    ढाँचा<typename T>
    शून्य object(अचर T &&मानहरू)
    {
        begin_object();
        लागि (स्वतः &item : मानहरू)
            entry(item.first, item.second);
        end_object();
    }

    ढाँचा<typename T>
    शून्य array(अचर T &&मानहरू)
    {
        begin_array();
        लागि (स्वतः &item : मानहरू)
            मान(item);
        end_array();
    }
};



YOSYS_NAMESPACE_END

#endif
