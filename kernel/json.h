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

#اگر_تعریف_نشده JSON_H
#تعریف_کن JSON_H

#ضمن "kernel/yosys.h"
#ضمن "libs/json11/json11.hpp"
#ضمن <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    تعداد Scope {
        مقدار_2,
        OBJECT_اولی,
        OBJECT,
        ARRAY_اولی,
        ARRAY,
    };

    بنية هدف {
        مجازی فراغ emit(ثابت حرف *داده) = 0;
        مجازی فراغ flush() {};
        مجازی ~هدف() {};
    };

    std::string newline_indent = "\n";
    std::وکتور<std::unique_ptr<هدف>> targets;
    std::وکتور<Scope> state = {مقدار_2};
    صحيح compact_عمق = بیشترین_صحیح;
عام:

    فراغ emit_مقصد_log();
    فراغ append_مقصد_string(std::string &هدف_2);
    bool write_مقصد_file(ثابت std::string &مسیر_2);

    bool فعال() { أرجع !targets.خالی(); }

    فراغ compact() { compact_عمق = Get_اندازه(state); }

    فراغ سطر(bool space_if_inline = true);
    فراغ raw(ثابت حرف *raw_json);
    فراغ flush();
    فراغ begin_object();
    فراغ begin_array();
    فراغ انتها_object();
    فراغ انتها_array();
    فراغ name(ثابت حرف *name);
    فراغ begin_مقدار();
    فراغ انتها_مقدار();
    فراغ مقدار_json(ثابت Json &مقدار);
    فراغ مقدار(بیعلامت صحيح مقدار) { مقدار_json(Json((صحيح)مقدار)); }
    قالب<typename T>
    فراغ مقدار(T &&مقدار) { مقدار_json(Json(std::forward<T>(مقدار))); };

    فراغ مدخل_json(ثابت حرف *name, ثابت Json &مقدار);
    فراغ مدخل(ثابت حرف *name, بیعلامت صحيح مقدار) { مدخل_json(name, Json((صحيح)مقدار)); }
    قالب<typename T>
    فراغ مدخل(ثابت حرف *name, T &&مقدار) { مدخل_json(name, Json(std::forward<T>(مقدار))); };

    قالب<typename T>
    فراغ object(ثابت T &&مقادیر)
    {
        begin_object();
        لكل (خودکار &قلم : مقادیر)
            مدخل(قلم.اولی, قلم.دومی);
        انتها_object();
    }

    قالب<typename T>
    فراغ array(ثابت T &&مقادیر)
    {
        begin_array();
        لكل (خودکار &قلم : مقادیر)
            مقدار(قلم);
        انتها_array();
    }
};



YOSYS_NAMESPACE_انتها

#پایان_اگر
