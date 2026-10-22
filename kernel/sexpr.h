/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2024  Emily Schmidt <emily@yosyshq.com>
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

#ifndef SEXPR_H
#define SEXPR_H

#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SExpr {
सार्वजनिक:
	std::variant<std::vector<SExpr>, std::string> _v;
सार्वजनिक:
	SExpr(std::string a) : _v(std::move(a)) {}
    SExpr(स्थिरांक अक्षर *a) : _v(a) {}
    // FIXME: should maybe be defined for all integral types
	SExpr(पूर्णांक n) : _v(std::to_string(n)) {}
	SExpr(std::vector<SExpr> स्थिरांक &l) : _v(l) {}
	SExpr(std::vector<SExpr> &&l) : _v(std::move(l)) {}
    // It would be nicer to have an std::initializer_list constructor,
    // but that causes confusing issues with overload resolution sometimes.
    ढाँचा<typename... Args> स्थिर SExpr list(Args&&... args) {
	    परत SExpr(std::vector<SExpr>{std::forward<Args>(args)...});
    }
    bool is_atom() स्थिरांक { परत std::holds_alternative<std::string>(_v); }
    std::string स्थिरांक &atom() स्थिरांक { परत std::get<std::string>(_v); }
    bool is_list() स्थिरांक { परत std::holds_alternative<std::vector<SExpr>>(_v); }
    std::vector<SExpr> स्थिरांक &list() स्थिरांक { परत std::get<std::vector<SExpr>>(_v); }
	std::string to_string() स्थिरांक;
};

std::ostream &चालक<<(std::ostream &os, SExpr स्थिरांक &sexpr);

namespace SExprUtil {
    // A little hack so that `using SExprUtil::list` lets you import a shortcut to `SExpr::list`
    ढाँचा<typename... Args> SExpr list(Args&&... args) {
	    परत SExpr(std::vector<SExpr>{std::forward<Args>(args)...});
    }
}

// SExprWriter is a pretty printer for s-expr. It does not try very hard to get a good layout.
class SExprWriter {
    std::ostream &os;
    पूर्णांक max_ओळ_width;
    पूर्णांक _indent = 0;
    पूर्णांक _pos = 0;
    // If _pending_nl is set, print a newline before the next character.
    // This lets us "undo" the last newline so we can put
    // closing parentheses or a hanging comment on the same line.
    bool _pending_nl = false;
    // Unclosed parentheses (boolean stored is indent_rest)
	vector<bool> _unclosed;
    // Used only for push() and pop() (stores _unclosed.size())
	vector<माप_प्रकार> unclosed_चळत;
	शून्य nl_if_pending();
    शून्य माला_दो(std::string_view s);
    पूर्णांक check_fit(SExpr स्थिरांक &sexpr, पूर्णांक space);
    शून्य print(SExpr स्थिरांक &sexpr, bool बंद = true, bool indent_rest = true);
सार्वजनिक:
    SExprWriter(std::ostream &os, पूर्णांक max_ओळ_width_2 = 80)
        : os(os)
        , max_ओळ_width(max_ओळ_width_2)
    {}
    // Print an s-expr.
    SExprWriter &चालक <<(SExpr स्थिरांक &sexpr) {
        print(sexpr);
        _pending_nl = true;
        परत *this;
    }
    // Print an s-expr (which must be a list), but leave room for extra elements
    // which may be printed using either << or further calls to open.
    // If indent_rest = false, the remaining elements are not intended
    // (for avoiding unreasonable indentation on deeply nested structures).
    शून्य खोलो(SExpr स्थिरांक &sexpr, bool indent_rest = true) {
        log_assert(sexpr.is_list());
        print(sexpr, false, indent_rest);
    }
    // Close the s-expr opened with the last call to open
    // (if an argument is given, close that many s-exprs).
    शून्य बंद(माप_प्रकार n = 1);
    // push() remembers how many s-exprs are currently open
	शून्य ढकला() {
		unclosed_चळत.ढकला_back(_unclosed.size());
	}
    // pop() closes all s-expr opened since the corresponding call to push()
	शून्य काढा() {
		स्वतः t = unclosed_चळत.back();
		log_assert(_unclosed.size() >= t);
		बंद(_unclosed.size() - t);
		unclosed_चळत.काढा_back();
	}
    // Print a comment.
    // If hanging = true, append it to the end of the last printed s-expr.
	शून्य comment(std::string स्थिरांक &str, bool hanging = false);
    // Flush any unprinted characters to the std::ostream, but does not close unclosed parentheses.
    शून्य flush() {
        nl_if_pending();
    }
    // Destructor closes any unclosed parentheses and flushes.
    ~SExprWriter();
};

YOSYS_NAMESPACE_END

#endif
