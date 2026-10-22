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

#اگر_غیر_تعریف_شدہ JSON_H
#تعریف_کرو JSON_H

#شامل "kernel/yosys.h"
#شامل "libs/json11/json11.hpp"
#شامل <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    شمار Scope {
        قدر_2,
        OBJECT_پہلا,
        OBJECT,
        ARRAY_پہلا,
        ARRAY,
    };

    ساخت ہدف {
        مجازی خالی emit(ثابت حرف *ڈیٹا) = 0;
        مجازی خالی flush() {};
        مجازی ~ہدف() {};
    };

    std::string newline_indent = "\n";
    std::سمتیہ<std::unique_ptr<ہدف>> targets;
    std::سمتیہ<Scope> state = {قدر_2};
    صحیح compact_گہرائی = زیادہ_سے_زیادہ_عدد;
عوامی:

    خالی emit_مقصد_log();
    خالی append_مقصد_string(std::string &ہدف_2);
    bool write_مقصد_file(ثابت std::string &راستہ_2);

    bool فعال() { واپس !targets.خالی_خانے(); }

    خالی compact() { compact_گہرائی = Get_سائز(state); }

    خالی سطر(bool space_if_inline = true);
    خالی raw(ثابت حرف *raw_json);
    خالی flush();
    خالی begin_object();
    خالی begin_array();
    خالی انتہا_object();
    خالی انتہا_array();
    خالی name(ثابت حرف *name);
    خالی begin_قدر();
    خالی انتہا_قدر();
    خالی قدر_json(ثابت Json &قدر);
    خالی قدر(غیرعلامتی صحیح قدر) { قدر_json(Json((صحیح)قدر)); }
    سانچہ<typename T>
    خالی قدر(T &&قدر) { قدر_json(Json(std::forward<T>(قدر))); };

    خالی اندراج_json(ثابت حرف *name, ثابت Json &قدر);
    خالی اندراج_2(ثابت حرف *name, غیرعلامتی صحیح قدر) { اندراج_json(name, Json((صحیح)قدر)); }
    سانچہ<typename T>
    خالی اندراج_2(ثابت حرف *name, T &&قدر) { اندراج_json(name, Json(std::forward<T>(قدر))); };

    سانچہ<typename T>
    خالی object(ثابت T &&اقدار)
    {
        begin_object();
        برائے (خودکار &شے_2 : اقدار)
            اندراج_2(شے_2.پہلا, شے_2.دوسرا);
        انتہا_object();
    }

    سانچہ<typename T>
    خالی array(ثابت T &&اقدار)
    {
        begin_array();
        برائے (خودکار &شے_2 : اقدار)
            قدر(شے_2);
        انتہا_array();
    }
};



YOSYS_NAMESPACE_انتہا

#اگر_ختم
