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

#ଅନ୍ତର୍ଭାଵ "kernel/json.h"

USING_YOSYS_NAMESPACE

ଶୂନ୍ୟ PrettyJson::emit_to_log()
{
    ସଂରଚନା LogTarget : ସର୍ବସାଧାରଣ Target {
        ଶୂନ୍ୟ emit(ଅଚଳ ଅକ୍ଷର *data) override { log("%s", data); }
    };

    targets.ଠେଲ_back(std::unique_ptr<Target>(ନୂତନ LogTarget));
}

ଶୂନ୍ୟ PrettyJson::append_to_string(std::string &target)
{
    ସଂରଚନା AppendStringTarget : ସର୍ବସାଧାରଣ Target {
        std::string &target;
        AppendStringTarget(std::string &target) : target(target) {}
        ଶୂନ୍ୟ emit(ଅଚଳ ଅକ୍ଷର *data) override { target += data; }
    };

    targets.ଠେଲ_back(std::unique_ptr<Target>(ନୂତନ AppendStringTarget(target)));
}

ତାର୍କିକ PrettyJson::write_to_file(ଅଚଳ std::string &path)
{
    ସଂରଚନା WriteFileTarget : ସର୍ବସାଧାରଣ Target {
        std::ofstream target;
        ଶୂନ୍ୟ emit(ଅଚଳ ଅକ୍ଷର *data) override { target << data; }
        ଶୂନ୍ୟ flush() override { target.flush(); }
    };

    auto target = std::unique_ptr<WriteFileTarget>(ନୂତନ WriteFileTarget);
    target->target.open(path);
    ଯଦି (target->target.fail())
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    targets.ଠେଲ_back(std::unique_ptr<Target>(target.release()));
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ PrettyJson::ଧାଡ଼ି(ତାର୍କିକ space_if_inline)
{
    ଯଦି (compact_depth != INT_MAX) {
        ଯଦି (space_if_inline)
            raw(" ");
        ଫେରନ୍ତୁ;
    }
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent = state.size() - (state.empty() ? 0 : state.back() == ମୂଲ୍ୟ_2);
    newline_indent.resize(1 + 2 * indent, ' ');
    raw(newline_indent.c_str());
}

ଶୂନ୍ୟ PrettyJson::raw(ଅଚଳ ଅକ୍ଷର *raw_json)
{
    ପାଇଁ (auto &target : targets)
        target->emit(raw_json);
}

ଶୂନ୍ୟ PrettyJson::flush()
{
    ପାଇଁ (auto &target : targets)
        target->flush();
}

ଶୂନ୍ୟ PrettyJson::begin_object()
{
    begin_ମୂଲ୍ୟ();
    raw("{");
    state.ଠେଲ_back(OBJECT_FIRST);
}

ଶୂନ୍ୟ PrettyJson::begin_array()
{
    begin_ମୂଲ୍ୟ();
    raw("[");
    state.ଠେଲ_back(ARRAY_FIRST);
}

ଶୂନ୍ୟ PrettyJson::end_object()
{
    Scope ଶୀର୍ଷ_scope = state.back();
    state.କାଢ଼_back();
    ଯଦି (ଶୀର୍ଷ_scope == OBJECT)
        ଧାଡ଼ି(ମିଥ୍ୟା);
    ଅନ୍ୟଥା
        log_assert(ଶୀର୍ଷ_scope == OBJECT_FIRST);
    raw("}");
    end_ମୂଲ୍ୟ();
}

ଶୂନ୍ୟ PrettyJson::end_array()
{
    Scope ଶୀର୍ଷ_scope = state.back();
    state.କାଢ଼_back();
    ଯଦି (ଶୀର୍ଷ_scope == ARRAY)
        ଧାଡ଼ି(ମିଥ୍ୟା);
    ଅନ୍ୟଥା
        log_assert(ଶୀର୍ଷ_scope == ARRAY_FIRST);
    raw("]");
    end_ମୂଲ୍ୟ();
}

ଶୂନ୍ୟ PrettyJson::name(ଅଚଳ ଅକ୍ଷର *name)
{
    ଯଦି (state.back() == OBJECT_FIRST) {
        state.back() = OBJECT;
        ଧାଡ଼ି(ମିଥ୍ୟା);
    } ଅନ୍ୟଥା {
        raw(",");
        ଧାଡ଼ି();
    }
    raw(Json(name).dump().c_str());
    raw(": ");
    state.ଠେଲ_back(ମୂଲ୍ୟ_2);
}

ଶୂନ୍ୟ PrettyJson::begin_ମୂଲ୍ୟ()
{
    ଯଦି (state.back() == ARRAY_FIRST) {
        ଧାଡ଼ି(ମିଥ୍ୟା);
        state.back() = ARRAY;
    } ଅନ୍ୟଥା ଯଦି (state.back() == ARRAY) {
        raw(",");
        ଧାଡ଼ି();
    } ଅନ୍ୟଥା {
        log_assert(state.back() == ମୂଲ୍ୟ_2);
        state.କାଢ଼_back();
    }
}

ଶୂନ୍ୟ PrettyJson::end_ମୂଲ୍ୟ()
{
    ଯଦି (state.empty()) {
        raw("\n");
        flush();
    }
    ଯଦି (GetSize(state) < compact_depth)
        compact_depth = INT_MAX;
}

ଶୂନ୍ୟ PrettyJson::ମୂଲ୍ୟ_json(ଅଚଳ Json &ମୂଲ୍ୟ)
{
    begin_ମୂଲ୍ୟ();
    raw(ମୂଲ୍ୟ.dump().c_str());
    end_ମୂଲ୍ୟ();
}

ଶୂନ୍ୟ PrettyJson::entry_json(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ Json &ମୂଲ୍ୟ)
{
    ଏହି->name(name);
    ଏହି->ମୂଲ୍ୟ(ମୂଲ୍ୟ);
}
