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

#अन्तर्भाव "sexpr.h"

YOSYS_NAMESPACE_BEGIN

std::ostream &operator<<(std::ostream &os, SExpr ध्रुव &sexpr) {
    सचे(sexpr.is_atom())
        os << sexpr.atom();
    अञ्ञथा सचे(sexpr.is_list()){
        os << "(";
        auto l = sexpr.list();
        अत्थाय(size_t i = 0; i < l.size(); i++) {
            सचे(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }अञ्ञथा
        os << "<invalid>";
    पटिनिवत्त os;
}

 std::string SExpr::to_string() ध्रुव {
    std::stringstream ss;
    ss << *एतत्;
    पटिनिवत्त ss.str();
}

शून्य SExprWriter::nl_if_pending() {
    सचे(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = मिच्छा;
    }
}

शून्य SExprWriter::puts(std::string_view s) {
    सचे(s.empty()) पटिनिवत्त;
    nl_if_pending();
    अत्थाय(auto c : s) {
        सचे(c == '\n') {
            os << c;
            _pos = 0;
        } अञ्ञथा {
            सचे(_pos == 0) {
                अत्थाय(पूर्णाङ्क i = 0; i < _indent; i++)
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
पूर्णाङ्क SExprWriter::check_fit(SExpr ध्रुव &sexpr, पूर्णाङ्क space) {
    सचे(sexpr.is_atom())
        पटिनिवत्त space - sexpr.atom().size();
    अञ्ञथा सचे(sexpr.is_list()) {
        space -= 2;
        सचे(sexpr.list().size() > 1)
            space -= sexpr.list().size() - 1;
        अत्थाय(auto arg : sexpr.list()) {
            सचे(space < 0) भङ्ग;
            space = check_fit(arg, space);
        }
        पटिनिवत्त space;
    } अञ्ञथा
        पटिनिवत्त -1;
}

शून्य SExprWriter::print(SExpr ध्रुव &sexpr, तार्किक close, तार्किक indent_rest) {
    सचे(sexpr.is_atom())
        puts(sexpr.atom());
    अञ्ञथा सचे(sexpr.is_list()) {
        auto args = sexpr.list();
        puts("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        तार्किक vertical = args.size() > 1 && check_fit(sexpr, max_पङ्क्तिः_width - _pos + 1) < 0;
        सचे(vertical) _indent++;
        अत्थाय(size_t i = 0; i < args.size(); i++) {
            सचे(i > 0) puts(vertical ? "\n" : " ");
            print(args[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!close && indent_rest) - vertical;
        सचे(close)
            puts(")");
        अञ्ञथा {
            _unclosed.प्रक्षिप_back(indent_rest);
            _pending_nl = सच्च;
        }
    }अञ्ञथा
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.to_string());
}

शून्य SExprWriter::close(size_t n) {
    log_assert(_unclosed.size() - (unclosed_राशिः.empty() ? 0 : unclosed_राशिः.back()) >= n);
    याव(n-- > 0) {
        तार्किक indented = _unclosed[_unclosed.size() - 1];
        _unclosed.निष्कासय_back();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= max_पङ्क्तिः_width;
        सचे(indented)
            _indent--;
        puts(")");
        _pending_nl = सच्च;
    }
}

शून्य SExprWriter::comment(std::string ध्रुव &str, तार्किक hanging) {
    सचे(hanging) {
        सचे(_pending_nl) {
            _pending_nl = मिच्छा;
            puts(" ");
        }
    }
    size_t i = 0, e;
    करोहि{
        e = str.find('\n', i);
        puts("; ");
        puts(std::string_view(str).substr(i, e - i));
        puts("\n");
        i = e + 1;
    }याव(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    याव(!unclosed_राशिः.empty())
        निष्कासय();
    close(_unclosed.size());
    nl_if_pending();
}

YOSYS_NAMESPACE_END
