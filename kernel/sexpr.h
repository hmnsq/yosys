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
    SExpr(ध्रुव वर्ण *a) : _v(a) {}
    // FIXME: should maybe be defined for all integral types
	SExpr(पूर्णाङ्क n) : _v(std::to_string(n)) {}
	SExpr(std::vector<SExpr> ध्रुव &l) : _v(l) {}
	SExpr(std::vector<SExpr> &&l) : _v(std::move(l)) {}
    // It would be nicer to have an std::initializer_list constructor,
    // but that causes confusing issues with overload resolution sometimes.
    template<typename... Args> स्थिर SExpr list(Args&&... args) {
	    पटिनिवत्त SExpr(std::vector<SExpr>{std::forward<Args>(args)...});
    }
    bool is_atom() ध्रुव { पटिनिवत्त std::holds_alternative<std::string>(_v); }
    std::string ध्रुव &atom() ध्रुव { पटिनिवत्त std::get<std::string>(_v); }
    bool is_list() ध्रुव { पटिनिवत्त std::holds_alternative<std::vector<SExpr>>(_v); }
    std::vector<SExpr> ध्रुव &list() ध्रुव { पटिनिवत्त std::get<std::vector<SExpr>>(_v); }
	std::string to_string() ध्रुव;
};

std::ostream &operator<<(std::ostream &os, SExpr ध्रुव &sexpr);

namespace SExprUtil {
    // A little hack so that `using SExprUtil::list` lets you import a shortcut to `SExpr::list`
    template<typename... Args> SExpr list(Args&&... args) {
	    पटिनिवत्त SExpr(std::vector<SExpr>{std::forward<Args>(args)...});
    }
}

// SExprWriter is a pretty printer for s-expr. It does not try very hard to get a good layout.
class SExprWriter {
    std::ostream &os;
    पूर्णाङ्क max_पङ्क्तिः_width;
    पूर्णाङ्क _indent = 0;
    पूर्णाङ्क _pos = 0;
    // If _pending_nl is set, print a newline before the next character.
    // This lets us "undo" the last newline so we can put
    // closing parentheses or a hanging comment on the same line.
    bool _pending_nl = false;
    // Unclosed parentheses (boolean stored is indent_rest)
	vector<bool> _unclosed;
    // Used only for push() and pop() (stores _unclosed.size())
	vector<size_t> unclosed_राशिः;
	शून्य nl_if_pending();
    शून्य puts(std::string_view s);
    पूर्णाङ्क check_fit(SExpr ध्रुव &sexpr, पूर्णाङ्क space);
    शून्य print(SExpr ध्रुव &sexpr, bool close = true, bool indent_rest = true);
सार्वजनिक:
    SExprWriter(std::ostream &os, पूर्णाङ्क max_पङ्क्तिः_width_2 = 80)
        : os(os)
        , max_पङ्क्तिः_width(max_पङ्क्तिः_width_2)
    {}
    // Print an s-expr.
    SExprWriter &operator <<(SExpr ध्रुव &sexpr) {
        print(sexpr);
        _pending_nl = true;
        पटिनिवत्त *this;
    }
    // Print an s-expr (which must be a list), but leave room for extra elements
    // which may be printed using either << or further calls to open.
    // If indent_rest = false, the remaining elements are not intended
    // (for avoiding unreasonable indentation on deeply nested structures).
    शून्य open(SExpr ध्रुव &sexpr, bool indent_rest = true) {
        log_assert(sexpr.is_list());
        print(sexpr, false, indent_rest);
    }
    // Close the s-expr opened with the last call to open
    // (if an argument is given, close that many s-exprs).
    शून्य close(size_t n = 1);
    // push() remembers how many s-exprs are currently open
	शून्य प्रक्षिप() {
		unclosed_राशिः.प्रक्षिप_back(_unclosed.size());
	}
    // pop() closes all s-expr opened since the corresponding call to push()
	शून्य निष्कासय() {
		auto t = unclosed_राशिः.back();
		log_assert(_unclosed.size() >= t);
		close(_unclosed.size() - t);
		unclosed_राशिः.निष्कासय_back();
	}
    // Print a comment.
    // If hanging = true, append it to the end of the last printed s-expr.
	शून्य comment(std::string ध्रुव &str, bool hanging = false);
    // Flush any unprinted characters to the std::ostream, but does not close unclosed parentheses.
    शून्य flush() {
        nl_if_pending();
    }
    // Destructor closes any unclosed parentheses and flushes.
    ~SExprWriter();
};

YOSYS_NAMESPACE_END

#endif
