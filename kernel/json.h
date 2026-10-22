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

#अगर_अघोषित JSON_H
#घोषणा JSON_H

#समावेश "kernel/yosys.h"
#समावेश "libs/json11/json11.hpp"
#समावेश <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    क्रमागत Scope {
        मूल्य_2,
        OBJECT_FIRST,
        OBJECT,
        ARRAY_FIRST,
        ARRAY,
    };

    काष्ठा Target {
        भव व्योम emit(स्थिर अक्षर *तथ्य) = 0;
        भव व्योम flush() {};
        भव ~Target() {};
    };

    std::string newline_indent = "\n";
    std::vector<std::unique_ptr<Target>> targets;
    std::vector<Scope> state = {मूल्य_2};
    पूर्णांक compact_depth = पूर्णांक_उच्च;
खुला:

    व्योम emit_to_log();
    व्योम append_to_string(std::string &target);
    bool write_to_file(स्थिर std::string &path);

    bool active() { वापस !targets.empty(); }

    व्योम compact() { compact_depth = Get_आकार(state); }

    व्योम पंक्ति(bool space_if_inline = true);
    व्योम raw(स्थिर अक्षर *raw_json);
    व्योम flush();
    व्योम begin_object();
    व्योम begin_array();
    व्योम end_object();
    व्योम end_array();
    व्योम name(स्थिर अक्षर *name);
    व्योम begin_मूल्य();
    व्योम end_मूल्य();
    व्योम मूल्य_json(स्थिर Json &मूल्य);
    व्योम मूल्य(अचिन्हित पूर्णांक मूल्य) { मूल्य_json(Json((पूर्णांक)मूल्य)); }
    ढाँचा<typename T>
    व्योम मूल्य(T &&मूल्य) { मूल्य_json(Json(std::forward<T>(मूल्य))); };

    व्योम entry_json(स्थिर अक्षर *name, स्थिर Json &मूल्य);
    व्योम entry(स्थिर अक्षर *name, अचिन्हित पूर्णांक मूल्य) { entry_json(name, Json((पूर्णांक)मूल्य)); }
    ढाँचा<typename T>
    व्योम entry(स्थिर अक्षर *name, T &&मूल्य) { entry_json(name, Json(std::forward<T>(मूल्य))); };

    ढाँचा<typename T>
    व्योम object(स्थिर T &&मूल्यावली)
    {
        begin_object();
        क्रम (स्वतः &item : मूल्यावली)
            entry(item.first, item.second);
        end_object();
    }

    ढाँचा<typename T>
    व्योम array(स्थिर T &&मूल्यावली)
    {
        begin_array();
        क्रम (स्वतः &item : मूल्यावली)
            मूल्य(item);
        end_array();
    }
};



YOSYS_NAMESPACE_END

#पूर्ण_अगर
