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

#ಅನ್ತರ್ಭಾವ "kernel/json.h"

USING_YOSYS_NAMESPACE

ಶೂನ್ಯ PrettyJson::emit_ಗಮ್ಯ_log()
{
    ರಚನೆ Log_ಗುರಿ : ಸಾರ್ವಜನಿಕ ಗುರಿ {
        ಶೂನ್ಯ emit(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ) override { ಲಘುಗಣಕ("%s", ದತ್ತಾಂಶ); }
    };

    targets.ತಳ್ಳು_ಹಿಂಭಾಗ(std::unique_ptr<ಗುರಿ>(ಹೊಸ Log_ಗುರಿ));
}

ಶೂನ್ಯ PrettyJson::append_ಗಮ್ಯ_string(std::string &ಗುರಿ_2)
{
    ರಚನೆ Append_String_ಗುರಿ : ಸಾರ್ವಜನಿಕ ಗುರಿ {
        std::string &ಗುರಿ_2;
        Append_String_ಗುರಿ(std::string &ಗುರಿ_2) : ಗುರಿ_2(ಗುರಿ_2) {}
        ಶೂನ್ಯ emit(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ) override { ಗುರಿ_2 += ದತ್ತಾಂಶ; }
    };

    targets.ತಳ್ಳು_ಹಿಂಭಾಗ(std::unique_ptr<ಗುರಿ>(ಹೊಸ Append_String_ಗುರಿ(ಗುರಿ_2)));
}

ತಾರ್ಕಿಕ PrettyJson::write_ಗಮ್ಯ_file(ಸ್ಥಿರಾಂಕ std::string &ಮಾರ್ಗ_2)
{
    ರಚನೆ Write_File_ಗುರಿ : ಸಾರ್ವಜನಿಕ ಗುರಿ {
        std::ofstream ಗುರಿ_2;
        ಶೂನ್ಯ emit(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ) override { ಗುರಿ_2 << ದತ್ತಾಂಶ; }
        ಶೂನ್ಯ flush() override { ಗುರಿ_2.flush(); }
    };

    ಸ್ವಯಂಚಾಲಿತ ಗುರಿ_2 = std::unique_ptr<Write_File_ಗುರಿ>(ಹೊಸ Write_File_ಗುರಿ);
    ಗುರಿ_2->ಗುರಿ_2.open(ಮಾರ್ಗ_2);
    ಆದರೆ (ಗುರಿ_2->ಗುರಿ_2.fail())
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    targets.ತಳ್ಳು_ಹಿಂಭಾಗ(std::unique_ptr<ಗುರಿ>(ಗುರಿ_2.release()));
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಶೂನ್ಯ PrettyJson::ಸಾಲು(ತಾರ್ಕಿಕ space_if_inline)
{
    ಆದರೆ (compact_ಆಳ != ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) {
        ಆದರೆ (space_if_inline)
            raw(" ");
        ಹಿಂತಿರುಗಿ;
    }
    ಪೂರ್ಣಾಂಕ indent = state.ಗಾತ್ರ_2() - (state.ಖಾಲಿಗಳು() ? 0 : state.ಹಿಂಭಾಗ() == ಮೌಲ್ಯ_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

ಶೂನ್ಯ PrettyJson::raw(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *raw_json)
{
    ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಗುರಿ_2 : targets)
        ಗುರಿ_2->emit(raw_json);
}

ಶೂನ್ಯ PrettyJson::flush()
{
    ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಗುರಿ_2 : targets)
        ಗುರಿ_2->flush();
}

ಶೂನ್ಯ PrettyJson::begin_object()
{
    begin_ಮೌಲ್ಯ();
    raw("{");
    state.ತಳ್ಳು_ಹಿಂಭಾಗ(OBJECT_ಮೊದಲ);
}

ಶೂನ್ಯ PrettyJson::begin_array()
{
    begin_ಮೌಲ್ಯ();
    raw("[");
    state.ತಳ್ಳು_ಹಿಂಭಾಗ(ARRAY_ಮೊದಲ);
}

ಶೂನ್ಯ PrettyJson::ಕೊನೆ_object()
{
    Scope ತುದಿ_scope = state.ಹಿಂಭಾಗ();
    state.ತೆಗೆ_ಹಿಂಭಾಗ();
    ಆದರೆ (ತುದಿ_scope == OBJECT)
        ಸಾಲು(ಸುಳ್ಳು);
    ಇಲ್ಲದಿದ್ದರೆ
        log_assert(ತುದಿ_scope == OBJECT_ಮೊದಲ);
    raw("}");
    ಕೊನೆ_ಮೌಲ್ಯ();
}

ಶೂನ್ಯ PrettyJson::ಕೊನೆ_array()
{
    Scope ತುದಿ_scope = state.ಹಿಂಭಾಗ();
    state.ತೆಗೆ_ಹಿಂಭಾಗ();
    ಆದರೆ (ತುದಿ_scope == ARRAY)
        ಸಾಲು(ಸುಳ್ಳು);
    ಇಲ್ಲದಿದ್ದರೆ
        log_assert(ತುದಿ_scope == ARRAY_ಮೊದಲ);
    raw("]");
    ಕೊನೆ_ಮೌಲ್ಯ();
}

ಶೂನ್ಯ PrettyJson::name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
    ಆದರೆ (state.ಹಿಂಭಾಗ() == OBJECT_ಮೊದಲ) {
        state.ಹಿಂಭಾಗ() = OBJECT;
        ಸಾಲು(ಸುಳ್ಳು);
    } ಇಲ್ಲದಿದ್ದರೆ {
        raw(",");
        ಸಾಲು();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.ತಳ್ಳು_ಹಿಂಭಾಗ(ಮೌಲ್ಯ_2);
}

ಶೂನ್ಯ PrettyJson::begin_ಮೌಲ್ಯ()
{
    ಆದರೆ (state.ಹಿಂಭಾಗ() == ARRAY_ಮೊದಲ) {
        ಸಾಲು(ಸುಳ್ಳು);
        state.ಹಿಂಭಾಗ() = ARRAY;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state.ಹಿಂಭಾಗ() == ARRAY) {
        raw(",");
        ಸಾಲು();
    } ಇಲ್ಲದಿದ್ದರೆ {
        log_assert(state.ಹಿಂಭಾಗ() == ಮೌಲ್ಯ_2);
        state.ತೆಗೆ_ಹಿಂಭಾಗ();
    }
}

ಶೂನ್ಯ PrettyJson::ಕೊನೆ_ಮೌಲ್ಯ()
{
    ಆದರೆ (state.ಖಾಲಿಗಳು()) {
        raw("\n");
        flush();
    }
    ಆದರೆ (Get_ಗಾತ್ರ(state) < compact_ಆಳ)
        compact_ಆಳ = ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;
}

ಶೂನ್ಯ PrettyJson::ಮೌಲ್ಯ_json(ಸ್ಥಿರಾಂಕ Json &ಮೌಲ್ಯ)
{
    begin_ಮೌಲ್ಯ();
    raw(ಮೌಲ್ಯ.dump().c_str());
    ಕೊನೆ_ಮೌಲ್ಯ();
}

ಶೂನ್ಯ PrettyJson::ನಮೂದು_json(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ Json &ಮೌಲ್ಯ)
{
    ಇದು->name(name);
    ಇದು->ಮೌಲ್ಯ(ಮೌಲ್ಯ);
}
