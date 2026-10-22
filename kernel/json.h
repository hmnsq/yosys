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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "libs/json11/json11.hpp"
#ಅನ್ತರ್ಭಾವ <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    ಗಣನಾ Scope {
        ಮೌಲ್ಯ_2,
        OBJECT_FIRST,
        OBJECT,
        ARRAY_FIRST,
        ARRAY,
    };

    ಸಂರಚನಾ Target {
        ಆಭಾಸ ಶೂನ್ಯ emit(ಧ್ರುವ ವರ್ಣ *data) = 0;
        ಆಭಾಸ ಶೂನ್ಯ flush() {};
        ಆಭಾಸ ~Target() {};
    };

    std::string newline_indent = "\n";
    std::vector<std::unique_ptr<Target>> targets;
    std::vector<Scope> state = {ಮೌಲ್ಯ_2};
    ಪೂರ್ಣಾಙ್ಕ compact_depth = INT_MAX;
ಸಾರ್ವಜನಿಕ:

    ಶೂನ್ಯ emit_to_log();
    ಶೂನ್ಯ append_to_string(std::string &target);
    bool write_to_file(ಧ್ರುವ std::string &path);

    bool active() { ಪ್ರತ್ಯಾಗಮ !targets.empty(); }

    ಶೂನ್ಯ compact() { compact_depth = GetSize(state); }

    ಶೂನ್ಯ ಸಾಲು(bool space_if_inline = true);
    ಶೂನ್ಯ raw(ಧ್ರುವ ವರ್ಣ *raw_json);
    ಶೂನ್ಯ flush();
    ಶೂನ್ಯ begin_object();
    ಶೂನ್ಯ begin_array();
    ಶೂನ್ಯ end_object();
    ಶೂನ್ಯ end_array();
    ಶೂನ್ಯ name(ಧ್ರುವ ವರ್ಣ *name);
    ಶೂನ್ಯ begin_ಮೌಲ್ಯ();
    ಶೂನ್ಯ end_ಮೌಲ್ಯ();
    ಶೂನ್ಯ ಮೌಲ್ಯ_json(ಧ್ರುವ Json &ಮೌಲ್ಯ);
    ಶೂನ್ಯ ಮೌಲ್ಯ(unsigned ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ) { ಮೌಲ್ಯ_json(Json((ಪೂರ್ಣಾಙ್ಕ)ಮೌಲ್ಯ)); }
    template<typename T>
    ಶೂನ್ಯ ಮೌಲ್ಯ(T &&ಮೌಲ್ಯ) { ಮೌಲ್ಯ_json(Json(std::forward<T>(ಮೌಲ್ಯ))); };

    ಶೂನ್ಯ entry_json(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ Json &ಮೌಲ್ಯ);
    ಶೂನ್ಯ entry(ಧ್ರುವ ವರ್ಣ *name, unsigned ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ) { entry_json(name, Json((ಪೂರ್ಣಾಙ್ಕ)ಮೌಲ್ಯ)); }
    template<typename T>
    ಶೂನ್ಯ entry(ಧ್ರುವ ವರ್ಣ *name, T &&ಮೌಲ್ಯ) { entry_json(name, Json(std::forward<T>(ಮೌಲ್ಯ))); };

    template<typename T>
    ಶೂನ್ಯ object(ಧ್ರುವ T &&ಮೌಲ್ಯಗಳು)
    {
        begin_object();
        ಪ್ರತಿ (auto &item : ಮೌಲ್ಯಗಳು)
            entry(item.first, item.second);
        end_object();
    }

    template<typename T>
    ಶೂನ್ಯ array(ಧ್ರುವ T &&ಮೌಲ್ಯಗಳು)
    {
        begin_array();
        ಪ್ರತಿ (auto &item : ಮೌಲ್ಯಗಳು)
            ಮೌಲ್ಯ(item);
        end_array();
    }
};



YOSYS_NAMESPACE_END

#endif
