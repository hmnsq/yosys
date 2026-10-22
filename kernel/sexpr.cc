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

std::ostream &चालक<<(std::ostream &os, SExpr ध्रुव &sexpr) {
    यदि(sexpr.is_atom())
        os << sexpr.atom();
    अन्यथा यदि(sexpr.is_list()){
        os << "(";
        स्वतः l = sexpr.list();
        प्रति(माप_प्रकार i = 0; i < l.size(); i++) {
            यदि(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }अन्यथा
        os << "<invalid>";
    प्रत्यागम os;
}

 std::string SExpr::to_string() ध्रुव {
    std::stringstream ss;
    ss << *एतत्;
    प्रत्यागम ss.str();
}

शून्य SExprWriter::nl_if_pending() {
    यदि(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = असत्य;
    }
}

शून्य SExprWriter::माला_दो(std::string_view s) {
    यदि(s.empty()) प्रत्यागम;
    nl_if_pending();
    प्रति(स्वतः c : s) {
        यदि(c == '\n') {
            os << c;
            _pos = 0;
        } अन्यथा {
            यदि(_pos == 0) {
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
    यदि(sexpr.is_atom())
        प्रत्यागम space - sexpr.atom().size();
    अन्यथा यदि(sexpr.is_list()) {
        space -= 2;
        यदि(sexpr.list().size() > 1)
            space -= sexpr.list().size() - 1;
        प्रति(स्वतः arg : sexpr.list()) {
            यदि(space < 0) भङ्ग;
            space = check_fit(arg, space);
        }
        प्रत्यागम space;
    } अन्यथा
        प्रत्यागम -1;
}

शून्य SExprWriter::print(SExpr ध्रुव &sexpr, तार्किक बंद, तार्किक indent_rest) {
    यदि(sexpr.is_atom())
        माला_दो(sexpr.atom());
    अन्यथा यदि(sexpr.is_list()) {
        स्वतः args = sexpr.list();
        माला_दो("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        तार्किक vertical = args.size() > 1 && check_fit(sexpr, max_पंक्ति_width - _pos + 1) < 0;
        यदि(vertical) _indent++;
        प्रति(माप_प्रकार i = 0; i < args.size(); i++) {
            यदि(i > 0) माला_दो(vertical ? "\n" : " ");
            print(args[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!बंद && indent_rest) - vertical;
        यदि(बंद)
            माला_दो(")");
        अन्यथा {
            _unclosed.धकेलो_back(indent_rest);
            _pending_nl = सत्य;
        }
    }अन्यथा
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.to_string());
}

शून्य SExprWriter::बंद(माप_प्रकार n) {
    log_assert(_unclosed.size() - (unclosed_ढेर.empty() ? 0 : unclosed_ढेर.back()) >= n);
    यावत्(n-- > 0) {
        तार्किक indented = _unclosed[_unclosed.size() - 1];
        _unclosed.निकालो_back();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= max_पंक्ति_width;
        यदि(indented)
            _indent--;
        माला_दो(")");
        _pending_nl = सत्य;
    }
}

शून्य SExprWriter::comment(std::string ध्रुव &str, तार्किक hanging) {
    यदि(hanging) {
        यदि(_pending_nl) {
            _pending_nl = असत्य;
            माला_दो(" ");
        }
    }
    माप_प्रकार i = 0, e;
    कुरु{
        e = str.find('\n', i);
        माला_दो("; ");
        माला_दो(std::string_view(str).substr(i, e - i));
        माला_दो("\n");
        i = e + 1;
    }यावत्(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    यावत्(!unclosed_ढेर.empty())
        निकालो();
    बंद(_unclosed.size());
    nl_if_pending();
}

YOSYS_NAMESPACE_END
