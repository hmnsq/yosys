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

#অন্তর্ভাৱ "kernel/yosys.h"
#অন্তর্ভাৱ "libs/json11/json11.hpp"
#অন্তর্ভাৱ <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    গণনা Scope {
        মান_2,
        OBJECT_FIRST,
        OBJECT,
        ARRAY_FIRST,
        ARRAY,
    };

    সংরচনা Target {
        আভাস শূন্য emit(ধ্রুৱ ৱর্ণ *data) = 0;
        আভাস শূন্য flush() {};
        আভাস ~Target() {};
    };

    std::string newline_indent = "\n";
    std::vector<std::unique_ptr<Target>> targets;
    std::vector<Scope> state = {মান_2};
    পূর্ণাঙ্ক compact_depth = INT_MAX;
সার্ৱজনিক:

    শূন্য emit_to_log();
    শূন্য append_to_string(std::string &target);
    bool write_to_file(ধ্রুৱ std::string &path);

    bool active() { ঘূৰাই !targets.empty(); }

    শূন্য compact() { compact_depth = GetSize(state); }

    শূন্য সাৰি(bool space_if_inline = true);
    শূন্য raw(ধ্রুৱ ৱর্ণ *raw_json);
    শূন্য flush();
    শূন্য begin_object();
    শূন্য begin_array();
    শূন্য end_object();
    শূন্য end_array();
    শূন্য name(ধ্রুৱ ৱর্ণ *name);
    শূন্য begin_মান();
    শূন্য end_মান();
    শূন্য মান_json(ধ্রুৱ Json &মান);
    শূন্য মান(unsigned পূর্ণাঙ্ক মান) { মান_json(Json((পূর্ণাঙ্ক)মান)); }
    template<typename T>
    শূন্য মান(T &&মান) { মান_json(Json(std::forward<T>(মান))); };

    শূন্য entry_json(ধ্রুৱ ৱর্ণ *name, ধ্রুৱ Json &মান);
    শূন্য entry(ধ্রুৱ ৱর্ণ *name, unsigned পূর্ণাঙ্ক মান) { entry_json(name, Json((পূর্ণাঙ্ক)মান)); }
    template<typename T>
    শূন্য entry(ধ্রুৱ ৱর্ণ *name, T &&মান) { entry_json(name, Json(std::forward<T>(মান))); };

    template<typename T>
    শূন্য object(ধ্রুৱ T &&মানসমূহ)
    {
        begin_object();
        বাবে (auto &item : মানসমূহ)
            entry(item.first, item.second);
        end_object();
    }

    template<typename T>
    শূন্য array(ধ্রুৱ T &&মানসমূহ)
    {
        begin_array();
        বাবে (auto &item : মানসমূহ)
            মান(item);
        end_array();
    }
};



YOSYS_NAMESPACE_END

#endif
