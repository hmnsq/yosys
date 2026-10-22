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
        VALUE,
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
    std::vector<Scope> state = {VALUE};
    صحيح compact_depth = INT_MAX;
عام:

    فراغ emit_to_log();
    فراغ append_to_string(std::string &target);
    bool write_to_file(ثابت std::string &path);

    bool active() { החזר !targets.empty(); }

    فراغ compact() { compact_depth = GetSize(state); }

    فراغ line(bool space_if_inline = true);
    فراغ raw(ثابت حرف *raw_json);
    فراغ flush();
    فراغ begin_object();
    فراغ begin_array();
    فراغ end_object();
    فراغ end_array();
    فراغ name(ثابت حرف *name);
    فراغ begin_value();
    فراغ end_value();
    فراغ value_json(ثابت Json &value);
    فراغ value(unsigned صحيح value) { value_json(Json((صحيح)value)); }
    template<typename T>
    فراغ value(T &&value) { value_json(Json(std::forward<T>(value))); };

    فراغ entry_json(ثابت حرف *name, ثابت Json &value);
    فراغ entry(ثابت حرف *name, unsigned صحيح value) { entry_json(name, Json((صحيح)value)); }
    template<typename T>
    فراغ entry(ثابت حرف *name, T &&value) { entry_json(name, Json(std::forward<T>(value))); };

    template<typename T>
    فراغ object(ثابت T &&values)
    {
        begin_object();
        עבור (auto &item : values)
            entry(item.first, item.second);
        end_object();
    }

    template<typename T>
    فراغ array(ثابت T &&values)
    {
        begin_array();
        עבור (auto &item : values)
            value(item);
        end_array();
    }
};



YOSYS_NAMESPACE_END

#endif
