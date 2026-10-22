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

#અન્તર્ભાવ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SExpr {
જાહેર:
	std::variant<std::vector<SExpr>, std::string> _v;
જાહેર:
	SExpr(std::string a) : _v(std::move(a)) {}
    SExpr(અચલ અક્ષર *a) : _v(a) {}
    // FIXME: should maybe be defined for all integral types
	SExpr(પૂર્ણાંક n) : _v(std::to_string(n)) {}
	SExpr(std::vector<SExpr> અચલ &l) : _v(l) {}
	SExpr(std::vector<SExpr> &&l) : _v(std::move(l)) {}
    // It would be nicer to have an std::initializer_list constructor,
    // but that causes confusing issues with overload resolution sometimes.
    template<typename... Args> સ્થિર SExpr list(Args&&... args) {
	    પાછા SExpr(std::vector<SExpr>{std::forward<Args>(args)...});
    }
    bool is_atom() અચલ { પાછા std::holds_alternative<std::string>(_v); }
    std::string અચલ &atom() અચલ { પાછા std::get<std::string>(_v); }
    bool is_list() અચલ { પાછા std::holds_alternative<std::vector<SExpr>>(_v); }
    std::vector<SExpr> અચલ &list() અચલ { પાછા std::get<std::vector<SExpr>>(_v); }
	std::string to_string() અચલ;
};

std::ostream &operator<<(std::ostream &os, SExpr અચલ &sexpr);

namespace SExprUtil {
    // A little hack so that `using SExprUtil::list` lets you import a shortcut to `SExpr::list`
    template<typename... Args> SExpr list(Args&&... args) {
	    પાછા SExpr(std::vector<SExpr>{std::forward<Args>(args)...});
    }
}

// SExprWriter is a pretty printer for s-expr. It does not try very hard to get a good layout.
class SExprWriter {
    std::ostream &os;
    પૂર્ણાંક max_લીટી_width;
    પૂર્ણાંક _indent = 0;
    પૂર્ણાંક _pos = 0;
    // If _pending_nl is set, print a newline before the next character.
    // This lets us "undo" the last newline so we can put
    // closing parentheses or a hanging comment on the same line.
    bool _pending_nl = false;
    // Unclosed parentheses (boolean stored is indent_rest)
	vector<bool> _unclosed;
    // Used only for push() and pop() (stores _unclosed.size())
	vector<size_t> unclosed_ઢગલો;
	શૂન્ય nl_if_pending();
    શૂન્ય puts(std::string_view s);
    પૂર્ણાંક check_fit(SExpr અચલ &sexpr, પૂર્ણાંક space);
    શૂન્ય print(SExpr અચલ &sexpr, bool close = true, bool indent_rest = true);
જાહેર:
    SExprWriter(std::ostream &os, પૂર્ણાંક max_લીટી_width_2 = 80)
        : os(os)
        , max_લીટી_width(max_લીટી_width_2)
    {}
    // Print an s-expr.
    SExprWriter &operator <<(SExpr અચલ &sexpr) {
        print(sexpr);
        _pending_nl = true;
        પાછા *this;
    }
    // Print an s-expr (which must be a list), but leave room for extra elements
    // which may be printed using either << or further calls to open.
    // If indent_rest = false, the remaining elements are not intended
    // (for avoiding unreasonable indentation on deeply nested structures).
    શૂન્ય open(SExpr અચલ &sexpr, bool indent_rest = true) {
        log_assert(sexpr.is_list());
        print(sexpr, false, indent_rest);
    }
    // Close the s-expr opened with the last call to open
    // (if an argument is given, close that many s-exprs).
    શૂન્ય close(size_t n = 1);
    // push() remembers how many s-exprs are currently open
	શૂન્ય ધકેલો() {
		unclosed_ઢગલો.ધકેલો_back(_unclosed.size());
	}
    // pop() closes all s-expr opened since the corresponding call to push()
	શૂન્ય કાઢો() {
		auto t = unclosed_ઢગલો.back();
		log_assert(_unclosed.size() >= t);
		close(_unclosed.size() - t);
		unclosed_ઢગલો.કાઢો_back();
	}
    // Print a comment.
    // If hanging = true, append it to the end of the last printed s-expr.
	શૂન્ય comment(std::string અચલ &str, bool hanging = false);
    // Flush any unprinted characters to the std::ostream, but does not close unclosed parentheses.
    શૂન્ય flush() {
        nl_if_pending();
    }
    // Destructor closes any unclosed parentheses and flushes.
    ~SExprWriter();
};

YOSYS_NAMESPACE_END

#endif
