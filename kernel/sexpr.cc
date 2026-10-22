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
    जइ(sexpr.is_atom())
        os << sexpr.atom();
    अण्णहा जइ(sexpr.is_list()){
        os << "(";
        auto l = sexpr.list();
        प्रति(size_t i = 0; i < l.size(); i++) {
            जइ(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }अण्णहा
        os << "<invalid>";
    प्रत्यागम os;
}

 std::string SExpr::to_string() ध्रुव {
    std::stringstream ss;
    ss << *एतत्;
    प्रत्यागम ss.str();
}

शून्य SExprWriter::nl_if_pending() {
    जइ(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = मिच्छ;
    }
}

शून्य SExprWriter::puts(std::string_view s) {
    जइ(s.empty()) प्रत्यागम;
    nl_if_pending();
    प्रति(auto c : s) {
        जइ(c == '\n') {
            os << c;
            _pos = 0;
        } अण्णहा {
            जइ(_pos == 0) {
                प्रति(पूर्णाङ्क i = 0; i < _indent; i++)
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
    जइ(sexpr.is_atom())
        प्रत्यागम space - sexpr.atom().size();
    अण्णहा जइ(sexpr.is_list()) {
        space -= 2;
        जइ(sexpr.list().size() > 1)
            space -= sexpr.list().size() - 1;
        प्रति(auto arg : sexpr.list()) {
            जइ(space < 0) भङ्ग;
            space = check_fit(arg, space);
        }
        प्रत्यागम space;
    } अण्णहा
        प्रत्यागम -1;
}

शून्य SExprWriter::print(SExpr ध्रुव &sexpr, तार्किक close, तार्किक indent_rest) {
    जइ(sexpr.is_atom())
        puts(sexpr.atom());
    अण्णहा जइ(sexpr.is_list()) {
        auto args = sexpr.list();
        puts("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        तार्किक vertical = args.size() > 1 && check_fit(sexpr, max_पङ्क्तिः_width - _pos + 1) < 0;
        जइ(vertical) _indent++;
        प्रति(size_t i = 0; i < args.size(); i++) {
            जइ(i > 0) puts(vertical ? "\n" : " ");
            print(args[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!close && indent_rest) - vertical;
        जइ(close)
            puts(")");
        अण्णहा {
            _unclosed.प्रक्षिप_back(indent_rest);
            _pending_nl = सच्च;
        }
    }अण्णहा
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.to_string());
}

शून्य SExprWriter::close(size_t n) {
    log_assert(_unclosed.size() - (unclosed_राशिः.empty() ? 0 : unclosed_राशिः.back()) >= n);
    जाव(n-- > 0) {
        तार्किक indented = _unclosed[_unclosed.size() - 1];
        _unclosed.निष्कासय_back();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= max_पङ्क्तिः_width;
        जइ(indented)
            _indent--;
        puts(")");
        _pending_nl = सच्च;
    }
}

शून्य SExprWriter::comment(std::string ध्रुव &str, तार्किक hanging) {
    जइ(hanging) {
        जइ(_pending_nl) {
            _pending_nl = मिच्छ;
            puts(" ");
        }
    }
    size_t i = 0, e;
    करेहि{
        e = str.find('\n', i);
        puts("; ");
        puts(std::string_view(str).substr(i, e - i));
        puts("\n");
        i = e + 1;
    }जाव(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    जाव(!unclosed_राशिः.empty())
        निष्कासय();
    close(_unclosed.size());
    nl_if_pending();
}

YOSYS_NAMESPACE_END
