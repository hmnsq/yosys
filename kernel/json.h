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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "libs/json11/json11.hpp"
#ଅନ୍ତର୍ଭାଵ <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    ଗଣନା Scope {
        ମୂଲ୍ୟ_2,
        OBJECT_FIRST,
        OBJECT,
        ARRAY_FIRST,
        ARRAY,
    };

    ସଂରଚନା Target {
        ଆଭାସୀ ଶୂନ୍ୟ emit(ଅଚଳ ଅକ୍ଷର *data) = 0;
        ଆଭାସୀ ଶୂନ୍ୟ flush() {};
        ଆଭାସୀ ~Target() {};
    };

    std::string newline_indent = "\n";
    std::vector<std::unique_ptr<Target>> targets;
    std::vector<Scope> state = {ମୂଲ୍ୟ_2};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା compact_depth = INT_MAX;
ସର୍ବସାଧାରଣ:

    ଶୂନ୍ୟ emit_to_log();
    ଶୂନ୍ୟ append_to_string(std::string &target);
    bool write_to_file(ଅଚଳ std::string &path);

    bool active() { ଫେରନ୍ତୁ !targets.empty(); }

    ଶୂନ୍ୟ compact() { compact_depth = GetSize(state); }

    ଶୂନ୍ୟ ଧାଡ଼ି(bool space_if_inline = true);
    ଶୂନ୍ୟ raw(ଅଚଳ ଅକ୍ଷର *raw_json);
    ଶୂନ୍ୟ flush();
    ଶୂନ୍ୟ begin_object();
    ଶୂନ୍ୟ begin_array();
    ଶୂନ୍ୟ end_object();
    ଶୂନ୍ୟ end_array();
    ଶୂନ୍ୟ name(ଅଚଳ ଅକ୍ଷର *name);
    ଶୂନ୍ୟ begin_ମୂଲ୍ୟ();
    ଶୂନ୍ୟ end_ମୂଲ୍ୟ();
    ଶୂନ୍ୟ ମୂଲ୍ୟ_json(ଅଚଳ Json &ମୂଲ୍ୟ);
    ଶୂନ୍ୟ ମୂଲ୍ୟ(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ) { ମୂଲ୍ୟ_json(Json((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ମୂଲ୍ୟ)); }
    template<typename T>
    ଶୂନ୍ୟ ମୂଲ୍ୟ(T &&ମୂଲ୍ୟ) { ମୂଲ୍ୟ_json(Json(std::forward<T>(ମୂଲ୍ୟ))); };

    ଶୂନ୍ୟ entry_json(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ Json &ମୂଲ୍ୟ);
    ଶୂନ୍ୟ entry(ଅଚଳ ଅକ୍ଷର *name, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ) { entry_json(name, Json((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ମୂଲ୍ୟ)); }
    template<typename T>
    ଶୂନ୍ୟ entry(ଅଚଳ ଅକ୍ଷର *name, T &&ମୂଲ୍ୟ) { entry_json(name, Json(std::forward<T>(ମୂଲ୍ୟ))); };

    template<typename T>
    ଶୂନ୍ୟ object(ଅଚଳ T &&ମୂଲ୍ୟଗୁଡ଼ିକ)
    {
        begin_object();
        ପାଇଁ (auto &item : ମୂଲ୍ୟଗୁଡ଼ିକ)
            entry(item.first, item.second);
        end_object();
    }

    template<typename T>
    ଶୂନ୍ୟ array(ଅଚଳ T &&ମୂଲ୍ୟଗୁଡ଼ିକ)
    {
        begin_array();
        ପାଇଁ (auto &item : ମୂଲ୍ୟଗୁଡ଼ିକ)
            ମୂଲ୍ୟ(item);
        end_array();
    }
};



YOSYS_NAMESPACE_END

#endif
