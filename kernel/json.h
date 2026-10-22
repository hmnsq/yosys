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

#جے_بنا_تعریف JSON_H
#تعریف_کرو JSON_H

#ضمن "kernel/yosys.h"
#ضمن "libs/json11/json11.hpp"
#ضمن <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    تعداد Scope {
        قدر_2,
        OBJECT_پہلا,
        OBJECT,
        ARRAY_پہلا,
        ARRAY,
    };

    بنية نشانہ {
        مجازی فراغ emit(ثابت حرف *انکڑے) = 0;
        مجازی فراغ flush() {};
        مجازی ~نشانہ() {};
    };

    std::string newline_indent = "\n";
    std::سمتیہ<std::unique_ptr<نشانہ>> targets;
    std::سمتیہ<Scope> state = {قدر_2};
    صحيح compact_ڈونگھائی = ودھ_توں_ودھ_عدد;
عام:

    فراغ emit_منزل_log();
    فراغ append_منزل_string(std::string &نشانہ_2);
    bool write_منزل_file(ثابت std::string &رستہ_2);

    bool سرگرم() { أرجع !targets.خالی_تھاواں(); }

    فراغ compact() { compact_ڈونگھائی = Get_ناپ(state); }

    فراغ سطر(bool space_if_inline = true);
    فراغ raw(ثابت حرف *raw_json);
    فراغ flush();
    فراغ begin_object();
    فراغ begin_array();
    فراغ سرا_object();
    فراغ سرا_array();
    فراغ name(ثابت حرف *name);
    فراغ begin_قدر();
    فراغ سرا_قدر();
    فراغ قدر_json(ثابت Json &قدر);
    فراغ قدر(بنانشانی صحيح قدر) { قدر_json(Json((صحيح)قدر)); }
    سانچا<typename T>
    فراغ قدر(T &&قدر) { قدر_json(Json(std::forward<T>(قدر))); };

    فراغ اندراج_json(ثابت حرف *name, ثابت Json &قدر);
    فراغ اندراج(ثابت حرف *name, بنانشانی صحيح قدر) { اندراج_json(name, Json((صحيح)قدر)); }
    سانچا<typename T>
    فراغ اندراج(ثابت حرف *name, T &&قدر) { اندراج_json(name, Json(std::forward<T>(قدر))); };

    سانچا<typename T>
    فراغ object(ثابت T &&اقدار)
    {
        begin_object();
        لكل (خودکار &شے : اقدار)
            اندراج(شے.پہلا, شے.دوجا);
        سرا_object();
    }

    سانچا<typename T>
    فراغ array(ثابت T &&اقدار)
    {
        begin_array();
        لكل (خودکار &شے : اقدار)
            قدر(شے);
        سرا_array();
    }
};



YOSYS_NAMESPACE_سرا

#جے_انت
