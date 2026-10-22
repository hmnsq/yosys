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

#ضمن "kernel/yosys.h"
#ضمن "libs/json11/json11.hpp"
#ضمن <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    تعداد Scope {
        درج_2,
        OBJECT_FIRST,
        OBJECT,
        ARRAY_FIRST,
        ARRAY,
    };

    بنية Target {
        virtual فراغ emit(ثابت حرف *data) = 0;
        virtual فراغ flush() {};
        virtual ~Target() {};
    };

    std::string newline_indent = "\n";
    std::vector<std::unique_ptr<Target>> targets;
    std::vector<Scope> state = {درج_2};
    صحيح compact_depth = INT_MAX;
عام:

    فراغ emit_to_log();
    فراغ append_to_string(std::string &target);
    bool write_to_file(ثابت std::string &path);

    bool active() { أرجع !targets.empty(); }

    فراغ compact() { compact_depth = GetSize(state); }

    فراغ لي(bool space_if_inline = true);
    فراغ raw(ثابت حرف *raw_json);
    فراغ flush();
    فراغ begin_object();
    فراغ begin_array();
    فراغ end_object();
    فراغ end_array();
    فراغ name(ثابت حرف *name);
    فراغ begin_درج();
    فراغ end_درج();
    فراغ درج_json(ثابت Json &درج);
    فراغ درج(unsigned صحيح درج) { درج_json(Json((صحيح)درج)); }
    template<typename T>
    فراغ درج(T &&درج) { درج_json(Json(std::forward<T>(درج))); };

    فراغ entry_json(ثابت حرف *name, ثابت Json &درج);
    فراغ entry(ثابت حرف *name, unsigned صحيح درج) { entry_json(name, Json((صحيح)درج)); }
    template<typename T>
    فراغ entry(ثابت حرف *name, T &&درج) { entry_json(name, Json(std::forward<T>(درج))); };

    template<typename T>
    فراغ object(ثابت T &&درجوجي)
    {
        begin_object();
        لكل (auto &item : درجوجي)
            entry(item.first, item.second);
        end_object();
    }

    template<typename T>
    فراغ array(ثابت T &&درجوجي)
    {
        begin_array();
        لكل (auto &item : درجوجي)
            درج(item);
        end_array();
    }
};



YOSYS_NAMESPACE_END

#endif
