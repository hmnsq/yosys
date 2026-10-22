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

std::ostream &चालक<<(std::ostream &os, SExpr स्थिरांक &sexpr) {
    जर(sexpr.is_atom())
        os << sexpr.atom();
    अन्यथा जर(sexpr.is_list()){
        os << "(";
        स्वतः l = sexpr.list();
        साठी(माप_प्रकार i = 0; i < l.size(); i++) {
            जर(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }अन्यथा
        os << "<invalid>";
    परत os;
}

 std::string SExpr::to_string() स्थिरांक {
    std::stringstream ss;
    ss << *हे;
    परत ss.str();
}

शून्य SExprWriter::nl_if_pending() {
    जर(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = असत्य;
    }
}

शून्य SExprWriter::माला_दो(std::string_view s) {
    जर(s.empty()) परत;
    nl_if_pending();
    साठी(स्वतः c : s) {
        जर(c == '\n') {
            os << c;
            _pos = 0;
        } अन्यथा {
            जर(_pos == 0) {
                साठी(पूर्णांक i = 0; i < _indent; i++)
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
पूर्णांक SExprWriter::check_fit(SExpr स्थिरांक &sexpr, पूर्णांक space) {
    जर(sexpr.is_atom())
        परत space - sexpr.atom().size();
    अन्यथा जर(sexpr.is_list()) {
        space -= 2;
        जर(sexpr.list().size() > 1)
            space -= sexpr.list().size() - 1;
        साठी(स्वतः arg : sexpr.list()) {
            जर(space < 0) थांबा;
            space = check_fit(arg, space);
        }
        परत space;
    } अन्यथा
        परत -1;
}

शून्य SExprWriter::print(SExpr स्थिरांक &sexpr, तार्किक बंद, तार्किक indent_rest) {
    जर(sexpr.is_atom())
        माला_दो(sexpr.atom());
    अन्यथा जर(sexpr.is_list()) {
        स्वतः args = sexpr.list();
        माला_दो("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        तार्किक vertical = args.size() > 1 && check_fit(sexpr, max_ओळ_width - _pos + 1) < 0;
        जर(vertical) _indent++;
        साठी(माप_प्रकार i = 0; i < args.size(); i++) {
            जर(i > 0) माला_दो(vertical ? "\n" : " ");
            print(args[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!बंद && indent_rest) - vertical;
        जर(बंद)
            माला_दो(")");
        अन्यथा {
            _unclosed.ढकला_back(indent_rest);
            _pending_nl = सत्य;
        }
    }अन्यथा
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.to_string());
}

शून्य SExprWriter::बंद(माप_प्रकार n) {
    log_assert(_unclosed.size() - (unclosed_चळत.empty() ? 0 : unclosed_चळत.back()) >= n);
    पर्यंत(n-- > 0) {
        तार्किक indented = _unclosed[_unclosed.size() - 1];
        _unclosed.काढा_back();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= max_ओळ_width;
        जर(indented)
            _indent--;
        माला_दो(")");
        _pending_nl = सत्य;
    }
}

शून्य SExprWriter::comment(std::string स्थिरांक &str, तार्किक hanging) {
    जर(hanging) {
        जर(_pending_nl) {
            _pending_nl = असत्य;
            माला_दो(" ");
        }
    }
    माप_प्रकार i = 0, e;
    करा{
        e = str.find('\n', i);
        माला_दो("; ");
        माला_दो(std::string_view(str).substr(i, e - i));
        माला_दो("\n");
        i = e + 1;
    }पर्यंत(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    पर्यंत(!unclosed_चळत.empty())
        काढा();
    बंद(_unclosed.size());
    nl_if_pending();
}

YOSYS_NAMESPACE_END
