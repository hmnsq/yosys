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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ JSON_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ JSON_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "libs/json11/json11.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    ਗਿਣਤੀ Scope {
        ਮੁੱਲ_2,
        OBJECT_ਪਹਿਲਾ,
        OBJECT,
        ARRAY_ਪਹਿਲਾ,
        ARRAY,
    };

    ਬਣਤਰ ਨਿਸ਼ਾਨਾ {
        ਆਭਾਸੀ ਖਾਲੀ emit(ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ) = 0;
        ਆਭਾਸੀ ਖਾਲੀ flush() {};
        ਆਭਾਸੀ ~ਨਿਸ਼ਾਨਾ() {};
    };

    std::string newline_indent = "\n";
    std::ਸਦਿਸ਼<std::unique_ptr<ਨਿਸ਼ਾਨਾ>> targets;
    std::ਸਦਿਸ਼<Scope> state = {ਮੁੱਲ_2};
    ਪੂਰਨ compact_ਡੂੰਘਾਈ = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
ਜਨਤਕ:

    ਖਾਲੀ emit_ਮੰਜ਼ਿਲ_log();
    ਖਾਲੀ append_ਮੰਜ਼ਿਲ_string(std::string &ਨਿਸ਼ਾਨਾ_2);
    bool write_ਮੰਜ਼ਿਲ_file(ਅਚਲ std::string &ਰਸਤਾ_2);

    bool ਸਰਗਰਮ() { ਵਾਪਸ !targets.ਖਾਲੀ_ਥਾਵਾਂ(); }

    ਖਾਲੀ compact() { compact_ਡੂੰਘਾਈ = Get_ਮਾਪ(state); }

    ਖਾਲੀ ਸਤਰ(bool space_if_inline = true);
    ਖਾਲੀ raw(ਅਚਲ ਅੱਖਰ *raw_json);
    ਖਾਲੀ flush();
    ਖਾਲੀ begin_object();
    ਖਾਲੀ begin_array();
    ਖਾਲੀ ਸਿਰਾ_object();
    ਖਾਲੀ ਸਿਰਾ_array();
    ਖਾਲੀ name(ਅਚਲ ਅੱਖਰ *name);
    ਖਾਲੀ begin_ਮੁੱਲ();
    ਖਾਲੀ ਸਿਰਾ_ਮੁੱਲ();
    ਖਾਲੀ ਮੁੱਲ_json(ਅਚਲ Json &ਮੁੱਲ);
    ਖਾਲੀ ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਮੁੱਲ) { ਮੁੱਲ_json(Json((ਪੂਰਨ)ਮੁੱਲ)); }
    ਸਾਂਚਾ<typename T>
    ਖਾਲੀ ਮੁੱਲ(T &&ਮੁੱਲ) { ਮੁੱਲ_json(Json(std::forward<T>(ਮੁੱਲ))); };

    ਖਾਲੀ ਇੰਦਰਾਜ_json(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ Json &ਮੁੱਲ);
    ਖਾਲੀ ਇੰਦਰਾਜ(ਅਚਲ ਅੱਖਰ *name, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਮੁੱਲ) { ਇੰਦਰਾਜ_json(name, Json((ਪੂਰਨ)ਮੁੱਲ)); }
    ਸਾਂਚਾ<typename T>
    ਖਾਲੀ ਇੰਦਰਾਜ(ਅਚਲ ਅੱਖਰ *name, T &&ਮੁੱਲ) { ਇੰਦਰਾਜ_json(name, Json(std::forward<T>(ਮੁੱਲ))); };

    ਸਾਂਚਾ<typename T>
    ਖਾਲੀ object(ਅਚਲ T &&ਮੁੱਲਾਂ)
    {
        begin_object();
        ਲਈ (ਸਵੈਚਾਲਿਤ &ਵਸਤੂ_2 : ਮੁੱਲਾਂ)
            ਇੰਦਰਾਜ(ਵਸਤੂ_2.ਪਹਿਲਾ, ਵਸਤੂ_2.ਦੂਜਾ);
        ਸਿਰਾ_object();
    }

    ਸਾਂਚਾ<typename T>
    ਖਾਲੀ array(ਅਚਲ T &&ਮੁੱਲਾਂ)
    {
        begin_array();
        ਲਈ (ਸਵੈਚਾਲਿਤ &ਵਸਤੂ_2 : ਮੁੱਲਾਂ)
            ਮੁੱਲ(ਵਸਤੂ_2);
        ਸਿਰਾ_array();
    }
};



YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
