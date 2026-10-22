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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "libs/json11/json11.hpp"
#અન્તર્ભાવ <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    ગણના Scope {
        કિંમત_2,
        OBJECT_FIRST,
        OBJECT,
        ARRAY_FIRST,
        ARRAY,
    };

    સંરચના Target {
        આભાસી શૂન્ય emit(અચલ અક્ષર *data) = 0;
        આભાસી શૂન્ય flush() {};
        આભાસી ~Target() {};
    };

    std::string newline_indent = "\n";
    std::vector<std::unique_ptr<Target>> targets;
    std::vector<Scope> state = {કિંમત_2};
    પૂર્ણાંક compact_depth = INT_MAX;
જાહેર:

    શૂન્ય emit_to_log();
    શૂન્ય append_to_string(std::string &target);
    bool write_to_file(અચલ std::string &path);

    bool active() { પાછા !targets.empty(); }

    શૂન્ય compact() { compact_depth = GetSize(state); }

    શૂન્ય લીટી(bool space_if_inline = true);
    શૂન્ય raw(અચલ અક્ષર *raw_json);
    શૂન્ય flush();
    શૂન્ય begin_object();
    શૂન્ય begin_array();
    શૂન્ય end_object();
    શૂન્ય end_array();
    શૂન્ય name(અચલ અક્ષર *name);
    શૂન્ય begin_કિંમત();
    શૂન્ય end_કિંમત();
    શૂન્ય કિંમત_json(અચલ Json &કિંમત);
    શૂન્ય કિંમત(unsigned પૂર્ણાંક કિંમત) { કિંમત_json(Json((પૂર્ણાંક)કિંમત)); }
    template<typename T>
    શૂન્ય કિંમત(T &&કિંમત) { કિંમત_json(Json(std::forward<T>(કિંમત))); };

    શૂન્ય entry_json(અચલ અક્ષર *name, અચલ Json &કિંમત);
    શૂન્ય entry(અચલ અક્ષર *name, unsigned પૂર્ણાંક કિંમત) { entry_json(name, Json((પૂર્ણાંક)કિંમત)); }
    template<typename T>
    શૂન્ય entry(અચલ અક્ષર *name, T &&કિંમત) { entry_json(name, Json(std::forward<T>(કિંમત))); };

    template<typename T>
    શૂન્ય object(અચલ T &&કિંમતો)
    {
        begin_object();
        માટે (auto &item : કિંમતો)
            entry(item.first, item.second);
        end_object();
    }

    template<typename T>
    શૂન્ય array(અચલ T &&કિંમતો)
    {
        begin_array();
        માટે (auto &item : કિંમતો)
            કિંમત(item);
        end_array();
    }
};



YOSYS_NAMESPACE_END

#endif
