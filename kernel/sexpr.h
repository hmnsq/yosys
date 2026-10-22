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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SEXPR_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SEXPR_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SExpr {
ਜਨਤਕ:
	std::variant<std::ਸਦਿਸ਼<SExpr>, std::string> _v;
ਜਨਤਕ:
	SExpr(std::string a) : _v(std::move(a)) {}
    SExpr(ਅਚਲ ਅੱਖਰ *a) : _v(a) {}
    // FIXME: should maybe be defined for all integral types
	SExpr(ਪੂਰਨ n) : _v(std::ਮੰਜ਼ਿਲ_string(n)) {}
	SExpr(std::ਸਦਿਸ਼<SExpr> ਅਚਲ &l) : _v(l) {}
	SExpr(std::ਸਦਿਸ਼<SExpr> &&l) : _v(std::move(l)) {}
    // It would be nicer to have an std::initializer_list constructor,
    // but that causes confusing issues with overload resolution sometimes.
    ਸਾਂਚਾ<typename... ਦਲੀਲਾਂ_3> ਸਥਿਰ SExpr list(ਦਲੀਲਾਂ_3&&... ਦਲੀਲਾਂ_2) {
	    ਵਾਪਸ SExpr(std::ਸਦਿਸ਼<SExpr>{std::forward<ਦਲੀਲਾਂ_3>(ਦਲੀਲਾਂ_2)...});
    }
    bool is_atom() ਅਚਲ { ਵਾਪਸ std::holds_alternative<std::string>(_v); }
    std::string ਅਚਲ &atom() ਅਚਲ { ਵਾਪਸ std::get<std::string>(_v); }
    bool is_list() ਅਚਲ { ਵਾਪਸ std::holds_alternative<std::ਸਦਿਸ਼<SExpr>>(_v); }
    std::ਸਦਿਸ਼<SExpr> ਅਚਲ &list() ਅਚਲ { ਵਾਪਸ std::get<std::ਸਦਿਸ਼<SExpr>>(_v); }
	std::string ਮੰਜ਼ਿਲ_string() ਅਚਲ;
};

std::ostream &ਸੰਚਾਲਕ<<(std::ostream &os, SExpr ਅਚਲ &sexpr);

namespace SExprUtil {
    // A little hack so that `using SExprUtil::list` lets you import a shortcut to `SExpr::list`
    ਸਾਂਚਾ<typename... ਦਲੀਲਾਂ_3> SExpr list(ਦਲੀਲਾਂ_3&&... ਦਲੀਲਾਂ_2) {
	    ਵਾਪਸ SExpr(std::ਸਦਿਸ਼<SExpr>{std::forward<ਦਲੀਲਾਂ_3>(ਦਲੀਲਾਂ_2)...});
    }
}

// SExprWriter is a pretty printer for s-expr. It does not try very hard to get a good layout.
class SExprWriter {
    std::ostream &os;
    ਪੂਰਨ max_ਸਤਰ_ਚੌੜਾਈ;
    ਪੂਰਨ _indent = 0;
    ਪੂਰਨ _pos = 0;
    // If _pending_nl is set, print a newline before the next character.
    // This lets us "undo" the last newline so we can put
    // closing parentheses or a hanging comment on the same line.
    bool _pending_nl = false;
    // Unclosed parentheses (boolean stored is indent_rest)
	ਸਦਿਸ਼<bool> _unclosed;
    // Used only for push() and pop() (stores _unclosed.size())
	ਸਦਿਸ਼<ਮਾਪ_t> unclosed_ਢੇਰ;
	ਖਾਲੀ nl_if_pending();
    ਖਾਲੀ puts(std::string_view s);
    ਪੂਰਨ check_fit(SExpr ਅਚਲ &sexpr, ਪੂਰਨ space);
    ਖਾਲੀ print(SExpr ਅਚਲ &sexpr, bool close = true, bool indent_rest = true);
ਜਨਤਕ:
    SExprWriter(std::ostream &os, ਪੂਰਨ max_ਸਤਰ_ਚੌੜਾਈ_2 = 80)
        : os(os)
        , max_ਸਤਰ_ਚੌੜਾਈ(max_ਸਤਰ_ਚੌੜਾਈ_2)
    {}
    // Print an s-expr.
    SExprWriter &ਸੰਚਾਲਕ <<(SExpr ਅਚਲ &sexpr) {
        print(sexpr);
        _pending_nl = true;
        ਵਾਪਸ *this;
    }
    // Print an s-expr (which must be a list), but leave room for extra elements
    // which may be printed using either << or further calls to open.
    // If indent_rest = false, the remaining elements are not intended
    // (for avoiding unreasonable indentation on deeply nested structures).
    ਖਾਲੀ open(SExpr ਅਚਲ &sexpr, bool indent_rest = true) {
        log_assert(sexpr.is_list());
        print(sexpr, false, indent_rest);
    }
    // Close the s-expr opened with the last call to open
    // (if an argument is given, close that many s-exprs).
    ਖਾਲੀ close(ਮਾਪ_t n = 1);
    // push() remembers how many s-exprs are currently open
	ਖਾਲੀ ਧੱਕੋ() {
		unclosed_ਢੇਰ.ਧੱਕੋ_ਪਿੱਠ(_unclosed.ਮਾਪ());
	}
    // pop() closes all s-expr opened since the corresponding call to push()
	ਖਾਲੀ ਕੱਢੋ() {
		ਸਵੈਚਾਲਿਤ t = unclosed_ਢੇਰ.ਪਿੱਠ();
		log_assert(_unclosed.ਮਾਪ() >= t);
		close(_unclosed.ਮਾਪ() - t);
		unclosed_ਢੇਰ.ਕੱਢੋ_ਪਿੱਠ();
	}
    // Print a comment.
    // If hanging = true, append it to the end of the last printed s-expr.
	ਖਾਲੀ comment(std::string ਅਚਲ &str, bool hanging = false);
    // Flush any unprinted characters to the std::ostream, but does not close unclosed parentheses.
    ਖਾਲੀ flush() {
        nl_if_pending();
    }
    // Destructor closes any unclosed parentheses and flushes.
    ~SExprWriter();
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
