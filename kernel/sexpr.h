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

#অন্তর্ভাব "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SExpr {
সার্বজনিক:
	std::variant<std::vector<SExpr>, std::string> _v;
সার্বজনিক:
	SExpr(std::string a) : _v(std::move(a)) {}
    SExpr(ধ্রুব বর্ণ *a) : _v(a) {}
    // FIXME: should maybe be defined for all integral types
	SExpr(পূর্ণাঙ্ক n) : _v(std::to_string(n)) {}
	SExpr(std::vector<SExpr> ধ্রুব &l) : _v(l) {}
	SExpr(std::vector<SExpr> &&l) : _v(std::move(l)) {}
    // It would be nicer to have an std::initializer_list constructor,
    // but that causes confusing issues with overload resolution sometimes.
    template<typename... Args> স্থির SExpr list(Args&&... args) {
	    প্রত্যাগম SExpr(std::vector<SExpr>{std::forward<Args>(args)...});
    }
    bool is_atom() ধ্রুব { প্রত্যাগম std::holds_alternative<std::string>(_v); }
    std::string ধ্রুব &atom() ধ্রুব { প্রত্যাগম std::get<std::string>(_v); }
    bool is_list() ধ্রুব { প্রত্যাগম std::holds_alternative<std::vector<SExpr>>(_v); }
    std::vector<SExpr> ধ্রুব &list() ধ্রুব { প্রত্যাগম std::get<std::vector<SExpr>>(_v); }
	std::string to_string() ধ্রুব;
};

std::ostream &operator<<(std::ostream &os, SExpr ধ্রুব &sexpr);

namespace SExprUtil {
    // A little hack so that `using SExprUtil::list` lets you import a shortcut to `SExpr::list`
    template<typename... Args> SExpr list(Args&&... args) {
	    প্রত্যাগম SExpr(std::vector<SExpr>{std::forward<Args>(args)...});
    }
}

// SExprWriter is a pretty printer for s-expr. It does not try very hard to get a good layout.
class SExprWriter {
    std::ostream &os;
    পূর্ণাঙ্ক max_সারি_width;
    পূর্ণাঙ্ক _indent = 0;
    পূর্ণাঙ্ক _pos = 0;
    // If _pending_nl is set, print a newline before the next character.
    // This lets us "undo" the last newline so we can put
    // closing parentheses or a hanging comment on the same line.
    bool _pending_nl = false;
    // Unclosed parentheses (boolean stored is indent_rest)
	vector<bool> _unclosed;
    // Used only for push() and pop() (stores _unclosed.size())
	vector<size_t> unclosed_স্তূপ;
	শূন্য nl_if_pending();
    শূন্য puts(std::string_view s);
    পূর্ণাঙ্ক check_fit(SExpr ধ্রুব &sexpr, পূর্ণাঙ্ক space);
    শূন্য print(SExpr ধ্রুব &sexpr, bool close = true, bool indent_rest = true);
সার্বজনিক:
    SExprWriter(std::ostream &os, পূর্ণাঙ্ক max_সারি_width_2 = 80)
        : os(os)
        , max_সারি_width(max_সারি_width_2)
    {}
    // Print an s-expr.
    SExprWriter &operator <<(SExpr ধ্রুব &sexpr) {
        print(sexpr);
        _pending_nl = true;
        প্রত্যাগম *this;
    }
    // Print an s-expr (which must be a list), but leave room for extra elements
    // which may be printed using either << or further calls to open.
    // If indent_rest = false, the remaining elements are not intended
    // (for avoiding unreasonable indentation on deeply nested structures).
    শূন্য open(SExpr ধ্রুব &sexpr, bool indent_rest = true) {
        log_assert(sexpr.is_list());
        print(sexpr, false, indent_rest);
    }
    // Close the s-expr opened with the last call to open
    // (if an argument is given, close that many s-exprs).
    শূন্য close(size_t n = 1);
    // push() remembers how many s-exprs are currently open
	শূন্য ঠেলো() {
		unclosed_স্তূপ.ঠেলো_back(_unclosed.size());
	}
    // pop() closes all s-expr opened since the corresponding call to push()
	শূন্য বের_করো() {
		auto t = unclosed_স্তূপ.back();
		log_assert(_unclosed.size() >= t);
		close(_unclosed.size() - t);
		unclosed_স্তূপ.বের_করো_back();
	}
    // Print a comment.
    // If hanging = true, append it to the end of the last printed s-expr.
	শূন্য comment(std::string ধ্রুব &str, bool hanging = false);
    // Flush any unprinted characters to the std::ostream, but does not close unclosed parentheses.
    শূন্য flush() {
        nl_if_pending();
    }
    // Destructor closes any unclosed parentheses and flushes.
    ~SExprWriter();
};

YOSYS_NAMESPACE_END

#endif
