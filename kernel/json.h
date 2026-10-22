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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ JSON_H
#ವ್ಯಾಖ್ಯಾನಿಸು JSON_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "libs/json11/json11.hpp"
#ಅನ್ತರ್ಭಾವ <functional>

YOSYS_NAMESPACE_BEGIN

using json11::Json;

class PrettyJson
{
    ಎಣಿಕೆ Scope {
        ಮೌಲ್ಯ_2,
        OBJECT_ಮೊದಲ,
        OBJECT,
        ARRAY_ಮೊದಲ,
        ARRAY,
    };

    ರಚನೆ ಗುರಿ {
        ವಾಸ್ತವಿಕ ಶೂನ್ಯ emit(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ) = 0;
        ವಾಸ್ತವಿಕ ಶೂನ್ಯ flush() {};
        ವಾಸ್ತವಿಕ ~ಗುರಿ() {};
    };

    std::string newline_indent = "\n";
    std::ಸದಿಶ<std::unique_ptr<ಗುರಿ>> targets;
    std::ಸದಿಶ<Scope> state = {ಮೌಲ್ಯ_2};
    ಪೂರ್ಣಾಂಕ compact_ಆಳ = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
ಸಾರ್ವಜನಿಕ:

    ಶೂನ್ಯ emit_ಗಮ್ಯ_log();
    ಶೂನ್ಯ append_ಗಮ್ಯ_string(std::string &ಗುರಿ_2);
    bool write_ಗಮ್ಯ_file(ಸ್ಥಿರಾಂಕ std::string &ಮಾರ್ಗ_2);

    bool ಸಕ್ರಿಯ() { ಹಿಂತಿರುಗಿ !targets.ಖಾಲಿಗಳು(); }

    ಶೂನ್ಯ compact() { compact_ಆಳ = Get_ಗಾತ್ರ(state); }

    ಶೂನ್ಯ ಸಾಲು(bool space_if_inline = true);
    ಶೂನ್ಯ raw(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *raw_json);
    ಶೂನ್ಯ flush();
    ಶೂನ್ಯ begin_object();
    ಶೂನ್ಯ begin_array();
    ಶೂನ್ಯ ಕೊನೆ_object();
    ಶೂನ್ಯ ಕೊನೆ_array();
    ಶೂನ್ಯ name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
    ಶೂನ್ಯ begin_ಮೌಲ್ಯ();
    ಶೂನ್ಯ ಕೊನೆ_ಮೌಲ್ಯ();
    ಶೂನ್ಯ ಮೌಲ್ಯ_json(ಸ್ಥಿರಾಂಕ Json &ಮೌಲ್ಯ);
    ಶೂನ್ಯ ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ) { ಮೌಲ್ಯ_json(Json((ಪೂರ್ಣಾಂಕ)ಮೌಲ್ಯ)); }
    ಮಾದರಿ<typename T>
    ಶೂನ್ಯ ಮೌಲ್ಯ(T &&ಮೌಲ್ಯ) { ಮೌಲ್ಯ_json(Json(std::forward<T>(ಮೌಲ್ಯ))); };

    ಶೂನ್ಯ ನಮೂದು_json(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ Json &ಮೌಲ್ಯ);
    ಶೂನ್ಯ ನಮೂದು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ) { ನಮೂದು_json(name, Json((ಪೂರ್ಣಾಂಕ)ಮೌಲ್ಯ)); }
    ಮಾದರಿ<typename T>
    ಶೂನ್ಯ ನಮೂದು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, T &&ಮೌಲ್ಯ) { ನಮೂದು_json(name, Json(std::forward<T>(ಮೌಲ್ಯ))); };

    ಮಾದರಿ<typename T>
    ಶೂನ್ಯ object(ಸ್ಥಿರಾಂಕ T &&ಮೌಲ್ಯಗಳು)
    {
        begin_object();
        ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಅಂಶ : ಮೌಲ್ಯಗಳು)
            ನಮೂದು(ಅಂಶ.ಮೊದಲ, ಅಂಶ.ಎರಡನೇ);
        ಕೊನೆ_object();
    }

    ಮಾದರಿ<typename T>
    ಶೂನ್ಯ array(ಸ್ಥಿರಾಂಕ T &&ಮೌಲ್ಯಗಳು)
    {
        begin_array();
        ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಅಂಶ : ಮೌಲ್ಯಗಳು)
            ಮೌಲ್ಯ(ಅಂಶ);
        ಕೊನೆ_array();
    }
};



YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
