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

#अगर_अघोषित SEXPR_H
#घोषणा SEXPR_H

#समावेश "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

class SExpr {
खुला:
	std::variant<std::vector<SExpr>, std::string> _v;
खुला:
	SExpr(std::string a) : _v(std::move(a)) {}
    SExpr(स्थिर अक्षर *a) : _v(a) {}
    // FIXME: should maybe be defined for all integral types
	SExpr(पूर्णांक n) : _v(std::to_string(n)) {}
	SExpr(std::vector<SExpr> स्थिर &l) : _v(l) {}
	SExpr(std::vector<SExpr> &&l) : _v(std::move(l)) {}
    // It would be nicer to have an std::initializer_list constructor,
    // but that causes confusing issues with overload resolution sometimes.
    ढाँचा<typename... Args> अटल SExpr list(Args&&... args) {
	    वापस SExpr(std::vector<SExpr>{std::forward<Args>(args)...});
    }
    bool is_atom() स्थिर { वापस std::holds_alternative<std::string>(_v); }
    std::string स्थिर &atom() स्थिर { वापस std::get<std::string>(_v); }
    bool is_list() स्थिर { वापस std::holds_alternative<std::vector<SExpr>>(_v); }
    std::vector<SExpr> स्थिर &list() स्थिर { वापस std::get<std::vector<SExpr>>(_v); }
	std::string to_string() स्थिर;
};

std::ostream &चालक<<(std::ostream &os, SExpr स्थिर &sexpr);

namespace SExprUtil {
    // A little hack so that `using SExprUtil::list` lets you import a shortcut to `SExpr::list`
    ढाँचा<typename... Args> SExpr list(Args&&... args) {
	    वापस SExpr(std::vector<SExpr>{std::forward<Args>(args)...});
    }
}

// SExprWriter is a pretty printer for s-expr. It does not try very hard to get a good layout.
class SExprWriter {
    std::ostream &os;
    पूर्णांक max_पंक्ति_width;
    पूर्णांक _indent = 0;
    पूर्णांक _pos = 0;
    // If _pending_nl is set, print a newline before the next character.
    // This lets us "undo" the last newline so we can put
    // closing parentheses or a hanging comment on the same line.
    bool _pending_nl = false;
    // Unclosed parentheses (boolean stored is indent_rest)
	vector<bool> _unclosed;
    // Used only for push() and pop() (stores _unclosed.size())
	vector<माप_प्रकार> unclosed_ढेर;
	व्योम nl_if_pending();
    व्योम माला_दो(std::string_view s);
    पूर्णांक check_fit(SExpr स्थिर &sexpr, पूर्णांक space);
    व्योम print(SExpr स्थिर &sexpr, bool बंद = true, bool indent_rest = true);
खुला:
    SExprWriter(std::ostream &os, पूर्णांक max_पंक्ति_width_2 = 80)
        : os(os)
        , max_पंक्ति_width(max_पंक्ति_width_2)
    {}
    // Print an s-expr.
    SExprWriter &चालक <<(SExpr स्थिर &sexpr) {
        print(sexpr);
        _pending_nl = true;
        वापस *this;
    }
    // Print an s-expr (which must be a list), but leave room for extra elements
    // which may be printed using either << or further calls to open.
    // If indent_rest = false, the remaining elements are not intended
    // (for avoiding unreasonable indentation on deeply nested structures).
    व्योम खोलो(SExpr स्थिर &sexpr, bool indent_rest = true) {
        log_assert(sexpr.is_list());
        print(sexpr, false, indent_rest);
    }
    // Close the s-expr opened with the last call to open
    // (if an argument is given, close that many s-exprs).
    व्योम बंद(माप_प्रकार n = 1);
    // push() remembers how many s-exprs are currently open
	व्योम धकेलो() {
		unclosed_ढेर.धकेलो_back(_unclosed.आकार());
	}
    // pop() closes all s-expr opened since the corresponding call to push()
	व्योम निकालो() {
		स्वतः t = unclosed_ढेर.back();
		log_assert(_unclosed.आकार() >= t);
		बंद(_unclosed.आकार() - t);
		unclosed_ढेर.निकालो_back();
	}
    // Print a comment.
    // If hanging = true, append it to the end of the last printed s-expr.
	व्योम comment(std::string स्थिर &str, bool hanging = false);
    // Flush any unprinted characters to the std::ostream, but does not close unclosed parentheses.
    व्योम flush() {
        nl_if_pending();
    }
    // Destructor closes any unclosed parentheses and flushes.
    ~SExprWriter();
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
