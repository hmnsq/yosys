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

#شامل "kernel/yosys.h"
#شامل "libs/json11/json11.hpp"
#شامل <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    شمارش Scope {
        مقدار_2,
        OBJECT_FIRST,
        OBJECT,
        ARRAY_FIRST,
        ARRAY,
    };

    ساختار Target {
        مجازی تهی emit(ثابت نویسه *data) = 0;
        مجازی تهی flush() {};
        مجازی ~Target() {};
    };

    std::string newline_indent = "\n";
    std::vector<std::unique_ptr<Target>> targets;
    std::vector<Scope> state = {مقدار_2};
    صحیح compact_depth = INT_MAX;
عمومی:

    تهی emit_to_log();
    تهی append_to_string(std::string &target);
    bool write_to_file(ثابت std::string &path);

    bool active() { بازگشت !targets.empty(); }

    تهی compact() { compact_depth = GetSize(state); }

    تهی سطر(bool space_if_inline = true);
    تهی raw(ثابت نویسه *raw_json);
    تهی flush();
    تهی begin_object();
    تهی begin_array();
    تهی end_object();
    تهی end_array();
    تهی name(ثابت نویسه *name);
    تهی begin_مقدار();
    تهی end_مقدار();
    تهی مقدار_json(ثابت Json &مقدار);
    تهی مقدار(unsigned صحیح مقدار) { مقدار_json(Json((صحیح)مقدار)); }
    template<typename T>
    تهی مقدار(T &&مقدار) { مقدار_json(Json(std::forward<T>(مقدار))); };

    تهی entry_json(ثابت نویسه *name, ثابت Json &مقدار);
    تهی entry(ثابت نویسه *name, unsigned صحیح مقدار) { entry_json(name, Json((صحیح)مقدار)); }
    template<typename T>
    تهی entry(ثابت نویسه *name, T &&مقدار) { entry_json(name, Json(std::forward<T>(مقدار))); };

    template<typename T>
    تهی object(ثابت T &&مقادیر)
    {
        begin_object();
        برای (auto &item : مقادیر)
            entry(item.first, item.second);
        end_object();
    }

    template<typename T>
    تهی array(ثابت T &&مقادیر)
    {
        begin_array();
        برای (auto &item : مقادیر)
            مقدار(item);
        end_array();
    }
};



YOSYS_NAMESPACE_END

#endif
