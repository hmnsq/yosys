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

#جيڪڏهن_وصف_نه_ٿيل JSON_H
#وصف_ڪريو JSON_H

#ضمن "kernel/yosys.h"
#ضمن "libs/json11/json11.hpp"
#ضمن <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    تعداد Scope {
        قدر_2,
        OBJECT_پهريون,
        OBJECT,
        ARRAY_پهريون,
        ARRAY,
    };

    بنية هدف {
        مجازي فراغ emit(ثابت حرف *معلومات) = 0;
        مجازي فراغ flush() {};
        مجازي ~هدف() {};
    };

    std::string newline_indent = "\n";
    std::ويڪٽر<std::unique_ptr<هدف>> targets;
    std::ويڪٽر<Scope> state = {قدر_2};
    صحيح compact_اونهائي = وڌ_کان_وڌ_عدد;
عام:

    فراغ emit_منزل_log();
    فراغ append_منزل_string(std::string &هدف_2);
    bool write_منزل_file(ثابت std::string &رستو_2);

    bool سرگرم() { أرجع !targets.خالي_جايون(); }

    فراغ compact() { compact_اونهائي = Get_سائيز(state); }

    فراغ سٽ(bool space_if_inline = true);
    فراغ raw(ثابت حرف *raw_json);
    فراغ flush();
    فراغ begin_object();
    فراغ begin_array();
    فراغ پڇاڙي_object();
    فراغ پڇاڙي_array();
    فراغ name(ثابت حرف *name);
    فراغ begin_قدر();
    فراغ پڇاڙي_قدر();
    فراغ قدر_json(ثابت Json &قدر);
    فراغ قدر(بنانشان صحيح قدر) { قدر_json(Json((صحيح)قدر)); }
    سانچو<typename T>
    فراغ قدر(T &&قدر) { قدر_json(Json(std::forward<T>(قدر))); };

    فراغ داخلا_json(ثابت حرف *name, ثابت Json &قدر);
    فراغ داخلا_2(ثابت حرف *name, بنانشان صحيح قدر) { داخلا_json(name, Json((صحيح)قدر)); }
    سانچو<typename T>
    فراغ داخلا_2(ثابت حرف *name, T &&قدر) { داخلا_json(name, Json(std::forward<T>(قدر))); };

    سانچو<typename T>
    فراغ object(ثابت T &&قدرون)
    {
        begin_object();
        لكل (خودڪار &شي : قدرون)
            داخلا_2(شي.پهريون, شي.ٻيو);
        پڇاڙي_object();
    }

    سانچو<typename T>
    فراغ array(ثابت T &&قدرون)
    {
        begin_array();
        لكل (خودڪار &شي : قدرون)
            قدر(شي);
        پڇاڙي_array();
    }
};



YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
