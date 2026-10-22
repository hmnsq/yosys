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

#شامل "sexpr.h"

YOSYS_NAMESPACE_BEGIN

std::ostream &operator<<(std::ostream &os, SExpr ثابت &sexpr) {
    اگر(sexpr.is_atom())
        os << sexpr.atom();
    وگرنه اگر(sexpr.is_list()){
        os << "(";
        auto l = sexpr.list();
        برای(size_t i = 0; i < l.size(); i++) {
            اگر(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }وگرنه
        os << "<invalid>";
    بازگشت os;
}

 std::string SExpr::to_string() ثابت {
    std::stringstream ss;
    ss << *این;
    بازگشت ss.str();
}

تهی SExprWriter::nl_if_pending() {
    اگر(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = نادرست;
    }
}

تهی SExprWriter::puts(std::string_view s) {
    اگر(s.empty()) بازگشت;
    nl_if_pending();
    برای(auto c : s) {
        اگر(c == '\n') {
            os << c;
            _pos = 0;
        } وگرنه {
            اگر(_pos == 0) {
                برای(صحیح i = 0; i < _indent; i++)
                    os << "  ";
                _pos = 2 * _indent;
            }
            os << c;
            _pos++;
        }
    }
}


// Calculate how much space would be left if expression was written
// out in full horizontally. Returns any negative value if it doesn't fit.
//
// (Ideally we would avoid recalculating the widths of subexpression,
// but I can't figure out how to store the widths. As an alternative,
// we bail out of the calculation as soon as we can tell the expression
// doesn't fit in the available space.)
صحیح SExprWriter::check_fit(SExpr ثابت &sexpr, صحیح space) {
    اگر(sexpr.is_atom())
        بازگشت space - sexpr.atom().size();
    وگرنه اگر(sexpr.is_list()) {
        space -= 2;
        اگر(sexpr.list().size() > 1)
            space -= sexpr.list().size() - 1;
        برای(auto arg : sexpr.list()) {
            اگر(space < 0) بشکن;
            space = check_fit(arg, space);
        }
        بازگشت space;
    } وگرنه
        بازگشت -1;
}

تهی SExprWriter::print(SExpr ثابت &sexpr, منطقی close, منطقی indent_rest) {
    اگر(sexpr.is_atom())
        puts(sexpr.atom());
    وگرنه اگر(sexpr.is_list()) {
        auto args = sexpr.list();
        puts("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        منطقی vertical = args.size() > 1 && check_fit(sexpr, max_سطر_width - _pos + 1) < 0;
        اگر(vertical) _indent++;
        برای(size_t i = 0; i < args.size(); i++) {
            اگر(i > 0) puts(vertical ? "\n" : " ");
            print(args[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!close && indent_rest) - vertical;
        اگر(close)
            puts(")");
        وگرنه {
            _unclosed.افزودن_back(indent_rest);
            _pending_nl = درست;
        }
    }وگرنه
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.to_string());
}

تهی SExprWriter::close(size_t n) {
    log_assert(_unclosed.size() - (unclosed_پشته.empty() ? 0 : unclosed_پشته.back()) >= n);
    وقتی(n-- > 0) {
        منطقی indented = _unclosed[_unclosed.size() - 1];
        _unclosed.برداشت_back();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= max_سطر_width;
        اگر(indented)
            _indent--;
        puts(")");
        _pending_nl = درست;
    }
}

تهی SExprWriter::comment(std::string ثابت &str, منطقی hanging) {
    اگر(hanging) {
        اگر(_pending_nl) {
            _pending_nl = نادرست;
            puts(" ");
        }
    }
    size_t i = 0, e;
    انجامبده{
        e = str.find('\n', i);
        puts("; ");
        puts(std::string_view(str).substr(i, e - i));
        puts("\n");
        i = e + 1;
    }وقتی(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    وقتی(!unclosed_پشته.empty())
        برداشت();
    close(_unclosed.size());
    nl_if_pending();
}

YOSYS_NAMESPACE_END
