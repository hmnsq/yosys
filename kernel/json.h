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

#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "libs/json11/json11.hpp"
#அந்தர்பாவ <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    எண்ணிக்கை Scope {
        மதிப்பு_2,
        OBJECT_FIRST,
        OBJECT,
        ARRAY_FIRST,
        ARRAY,
    };

    கட்டமைப்பு Target {
        மெய்நிகர் வெறுமை emit(மாறிலி எழுத்து *data) = 0;
        மெய்நிகர் வெறுமை flush() {};
        மெய்நிகர் ~Target() {};
    };

    std::string newline_indent = "\n";
    std::vector<std::unique_ptr<Target>> targets;
    std::vector<Scope> state = {மதிப்பு_2};
    முழு compact_depth = INT_MAX;
பொது:

    வெறுமை emit_to_log();
    வெறுமை append_to_string(std::string &target);
    bool write_to_file(மாறிலி std::string &path);

    bool active() { திரும்பு !targets.empty(); }

    வெறுமை compact() { compact_depth = GetSize(state); }

    வெறுமை வரி(bool space_if_inline = true);
    வெறுமை raw(மாறிலி எழுத்து *raw_json);
    வெறுமை flush();
    வெறுமை begin_object();
    வெறுமை begin_array();
    வெறுமை end_object();
    வெறுமை end_array();
    வெறுமை name(மாறிலி எழுத்து *name);
    வெறுமை begin_மதிப்பு();
    வெறுமை end_மதிப்பு();
    வெறுமை மதிப்பு_json(மாறிலி Json &மதிப்பு);
    வெறுமை மதிப்பு(unsigned முழு மதிப்பு) { மதிப்பு_json(Json((முழு)மதிப்பு)); }
    template<typename T>
    வெறுமை மதிப்பு(T &&மதிப்பு) { மதிப்பு_json(Json(std::forward<T>(மதிப்பு))); };

    வெறுமை entry_json(மாறிலி எழுத்து *name, மாறிலி Json &மதிப்பு);
    வெறுமை entry(மாறிலி எழுத்து *name, unsigned முழு மதிப்பு) { entry_json(name, Json((முழு)மதிப்பு)); }
    template<typename T>
    வெறுமை entry(மாறிலி எழுத்து *name, T &&மதிப்பு) { entry_json(name, Json(std::forward<T>(மதிப்பு))); };

    template<typename T>
    வெறுமை object(மாறிலி T &&மதிப்புகள்)
    {
        begin_object();
        க்கு (auto &item : மதிப்புகள்)
            entry(item.first, item.second);
        end_object();
    }

    template<typename T>
    வெறுமை array(மாறிலி T &&மதிப்புகள்)
    {
        begin_array();
        க்கு (auto &item : மதிப்புகள்)
            மதிப்பு(item);
        end_array();
    }
};



YOSYS_NAMESPACE_END

#endif
