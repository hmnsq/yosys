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
        मूल्य_2,
        OBJECT_FIRST,
        OBJECT,
        ARRAY_FIRST,
        ARRAY,
    };

    संरचना Target {
        आभासी शून्य emit(स्थिरांक अक्षर *data) = 0;
        आभासी शून्य flush() {};
        आभासी ~Target() {};
    };

    std::string newline_indent = "\n";
    std::vector<std::unique_ptr<Target>> targets;
    std::vector<Scope> state = {मूल्य_2};
    पूर्णांक compact_depth = पूर्णांक_उच्च;
सार्वजनिक:

    शून्य emit_to_log();
    शून्य append_to_string(std::string &target);
    bool write_to_file(स्थिरांक std::string &path);

    bool active() { परत !targets.empty(); }

    शून्य compact() { compact_depth = GetSize(state); }

    शून्य ओळ(bool space_if_inline = true);
    शून्य raw(स्थिरांक अक्षर *raw_json);
    शून्य flush();
    शून्य begin_object();
    शून्य begin_array();
    शून्य end_object();
    शून्य end_array();
    शून्य name(स्थिरांक अक्षर *name);
    शून्य begin_मूल्य();
    शून्य end_मूल्य();
    शून्य मूल्य_json(स्थिरांक Json &मूल्य);
    शून्य मूल्य(अचिन्हित पूर्णांक मूल्य) { मूल्य_json(Json((पूर्णांक)मूल्य)); }
    ढाँचा<typename T>
    शून्य मूल्य(T &&मूल्य) { मूल्य_json(Json(std::forward<T>(मूल्य))); };

    शून्य entry_json(स्थिरांक अक्षर *name, स्थिरांक Json &मूल्य);
    शून्य entry(स्थिरांक अक्षर *name, अचिन्हित पूर्णांक मूल्य) { entry_json(name, Json((पूर्णांक)मूल्य)); }
    ढाँचा<typename T>
    शून्य entry(स्थिरांक अक्षर *name, T &&मूल्य) { entry_json(name, Json(std::forward<T>(मूल्य))); };

    ढाँचा<typename T>
    शून्य object(स्थिरांक T &&मूल्ये)
    {
        begin_object();
        साठी (स्वतः &item : मूल्ये)
            entry(item.first, item.second);
        end_object();
    }

    ढाँचा<typename T>
    शून्य array(स्थिरांक T &&मूल्ये)
    {
        begin_array();
        साठी (स्वतः &item : मूल्ये)
            मूल्य(item);
        end_array();
    }
};



YOSYS_NAMESPACE_END

#endif
