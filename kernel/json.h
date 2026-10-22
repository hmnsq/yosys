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

#که_نه_تعریف_شوی JSON_H
#تعریف_کړه JSON_H

#ضمن "kernel/yosys.h"
#ضمن "libs/json11/json11.hpp"
#ضمن <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    تعداد Scope {
        ارزښت_2,
        OBJECT_لومړی,
        OBJECT,
        ARRAY_لومړی,
        ARRAY,
    };

    بنية موخه {
        مجازي فراغ emit(ثابت حرف *معلومات) = 0;
        مجازي فراغ flush() {};
        مجازي ~موخه() {};
    };

    std::string newline_indent = "\n";
    std::ویکټور<std::unique_ptr<موخه>> targets;
    std::ویکټور<Scope> state = {ارزښت_2};
    صحيح compact_ژوروالی = لوی_عدد;
عام:

    فراغ emit_منزل_log();
    فراغ append_منزل_string(std::string &موخه_2);
    bool write_منزل_file(ثابت std::string &لار_2);

    bool فعال() { أرجع !targets.تشه(); }

    فراغ compact() { compact_ژوروالی = Get_اندازه(state); }

    فراغ کرښه(bool space_if_inline = true);
    فراغ raw(ثابت حرف *raw_json);
    فراغ flush();
    فراغ begin_object();
    فراغ begin_array();
    فراغ پای_ټکی_object();
    فراغ پای_ټکی_array();
    فراغ name(ثابت حرف *name);
    فراغ begin_ارزښت();
    فراغ پای_ټکی_ارزښت();
    فراغ ارزښت_json(ثابت Json &ارزښت);
    فراغ ارزښت(بینښه صحيح ارزښت) { ارزښت_json(Json((صحيح)ارزښت)); }
    کينډۍ<typename T>
    فراغ ارزښت(T &&ارزښت) { ارزښت_json(Json(std::forward<T>(ارزښت))); };

    فراغ ثبت_json(ثابت حرف *name, ثابت Json &ارزښت);
    فراغ ثبت(ثابت حرف *name, بینښه صحيح ارزښت) { ثبت_json(name, Json((صحيح)ارزښت)); }
    کينډۍ<typename T>
    فراغ ثبت(ثابت حرف *name, T &&ارزښت) { ثبت_json(name, Json(std::forward<T>(ارزښت))); };

    کينډۍ<typename T>
    فراغ object(ثابت T &&ارزښتونه)
    {
        begin_object();
        لكل (خپلکاری &توکی : ارزښتونه)
            ثبت(توکی.لومړی, توکی.دوهم);
        پای_ټکی_object();
    }

    کينډۍ<typename T>
    فراغ array(ثابت T &&ارزښتونه)
    {
        begin_array();
        لكل (خپلکاری &توکی : ارزښتونه)
            ارزښت(توکی);
        پای_ټکی_array();
    }
};



YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
