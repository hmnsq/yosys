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

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "libs/json11/json11.hpp"
#അന്തര്ഭാവ <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    എണ്ണൽ Scope {
        മൂല്യം_2,
        OBJECT_FIRST,
        OBJECT,
        ARRAY_FIRST,
        ARRAY,
    };

    ഘടന Target {
        മായാ ശൂന്യം emit(സ്ഥിരം അക്ഷരം *data) = 0;
        മായാ ശൂന്യം flush() {};
        മായാ ~Target() {};
    };

    std::string newline_indent = "\n";
    std::vector<std::unique_ptr<Target>> targets;
    std::vector<Scope> state = {മൂല്യം_2};
    പൂർണ്ണസംഖ്യ compact_depth = INT_MAX;
പൊതു:

    ശൂന്യം emit_to_log();
    ശൂന്യം append_to_string(std::string &target);
    bool write_to_file(സ്ഥിരം std::string &path);

    bool active() { തിരികെ !targets.empty(); }

    ശൂന്യം compact() { compact_depth = GetSize(state); }

    ശൂന്യം വരി(bool space_if_inline = true);
    ശൂന്യം raw(സ്ഥിരം അക്ഷരം *raw_json);
    ശൂന്യം flush();
    ശൂന്യം begin_object();
    ശൂന്യം begin_array();
    ശൂന്യം end_object();
    ശൂന്യം end_array();
    ശൂന്യം name(സ്ഥിരം അക്ഷരം *name);
    ശൂന്യം begin_മൂല്യം();
    ശൂന്യം end_മൂല്യം();
    ശൂന്യം മൂല്യം_json(സ്ഥിരം Json &മൂല്യം);
    ശൂന്യം മൂല്യം(unsigned പൂർണ്ണസംഖ്യ മൂല്യം) { മൂല്യം_json(Json((പൂർണ്ണസംഖ്യ)മൂല്യം)); }
    template<typename T>
    ശൂന്യം മൂല്യം(T &&മൂല്യം) { മൂല്യം_json(Json(std::forward<T>(മൂല്യം))); };

    ശൂന്യം entry_json(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം Json &മൂല്യം);
    ശൂന്യം entry(സ്ഥിരം അക്ഷരം *name, unsigned പൂർണ്ണസംഖ്യ മൂല്യം) { entry_json(name, Json((പൂർണ്ണസംഖ്യ)മൂല്യം)); }
    template<typename T>
    ശൂന്യം entry(സ്ഥിരം അക്ഷരം *name, T &&മൂല്യം) { entry_json(name, Json(std::forward<T>(മൂല്യം))); };

    template<typename T>
    ശൂന്യം object(സ്ഥിരം T &&മൂല്യങ്ങൾ)
    {
        begin_object();
        വേണ്ടി (auto &item : മൂല്യങ്ങൾ)
            entry(item.first, item.second);
        end_object();
    }

    template<typename T>
    ശൂന്യം array(സ്ഥിരം T &&മൂല്യങ്ങൾ)
    {
        begin_array();
        വേണ്ടി (auto &item : മൂല്യങ്ങൾ)
            മൂല്യം(item);
        end_array();
    }
};



YOSYS_NAMESPACE_END

#endif
