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

#समावेश "sexpr.h"

YOSYS_NAMESPACE_BEGIN

std::ब_प्रवाह &चालक<<(std::ब_प्रवाह &os, SExpr स्थिर &sexpr) {
    अगर(sexpr.is_atom())
        os << sexpr.atom();
    अन्यथा अगर(sexpr.is_list()){
        os << "(";
        स्वतः l = sexpr.list();
        क्रम(माप_प्रकार i = 0; i < l.आकार(); i++) {
            अगर(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }अन्यथा
        os << "<invalid>";
    वापस os;
}

 std::string SExpr::to_string() स्थिर {
    std::stringstream ss;
    ss << *यह;
    वापस ss.str();
}

व्योम SExprWriter::nl_if_pending() {
    अगर(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = false;
    }
}

व्योम SExprWriter::माला_दो(std::string_view s) {
    अगर(s.empty()) वापस;
    nl_if_pending();
    क्रम(स्वतः c : s) {
        अगर(c == '\n') {
            os << c;
            _pos = 0;
        } अन्यथा {
            अगर(_pos == 0) {
                क्रम(पूर्णांक i = 0; i < _indent; i++)
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
पूर्णांक SExprWriter::check_fit(SExpr स्थिर &sexpr, पूर्णांक space) {
    अगर(sexpr.is_atom())
        वापस space - sexpr.atom().आकार();
    अन्यथा अगर(sexpr.is_list()) {
        space -= 2;
        अगर(sexpr.list().आकार() > 1)
            space -= sexpr.list().आकार() - 1;
        क्रम(स्वतः arg : sexpr.list()) {
            अगर(space < 0) अवरोध;
            space = check_fit(arg, space);
        }
        वापस space;
    } अन्यथा
        वापस -1;
}

व्योम SExprWriter::print(SExpr स्थिर &sexpr, bool बंद, bool indent_rest) {
    अगर(sexpr.is_atom())
        माला_दो(sexpr.atom());
    अन्यथा अगर(sexpr.is_list()) {
        स्वतः args = sexpr.list();
        माला_दो("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        bool vertical = args.आकार() > 1 && check_fit(sexpr, max_पंक्ति_width - _pos + 1) < 0;
        अगर(vertical) _indent++;
        क्रम(माप_प्रकार i = 0; i < args.आकार(); i++) {
            अगर(i > 0) माला_दो(vertical ? "\n" : " ");
            print(args[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!बंद && indent_rest) - vertical;
        अगर(बंद)
            माला_दो(")");
        अन्यथा {
            _unclosed.धकेलो_back(indent_rest);
            _pending_nl = true;
        }
    }अन्यथा
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.to_string());
}

व्योम SExprWriter::बंद(माप_प्रकार n) {
    log_assert(_unclosed.आकार() - (unclosed_ढेर.empty() ? 0 : unclosed_ढेर.back()) >= n);
    जबतक(n-- > 0) {
        bool indented = _unclosed[_unclosed.आकार() - 1];
        _unclosed.निकालो_back();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= max_पंक्ति_width;
        अगर(indented)
            _indent--;
        माला_दो(")");
        _pending_nl = true;
    }
}

व्योम SExprWriter::comment(std::string स्थिर &str, bool hanging) {
    अगर(hanging) {
        अगर(_pending_nl) {
            _pending_nl = false;
            माला_दो(" ");
        }
    }
    माप_प्रकार i = 0, e;
    करो{
        e = str.find('\n', i);
        माला_दो("; ");
        माला_दो(std::string_view(str).substr(i, e - i));
        माला_दो("\n");
        i = e + 1;
    }जबतक(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    जबतक(!unclosed_ढेर.empty())
        निकालो();
    बंद(_unclosed.आकार());
    nl_if_pending();
}

YOSYS_NAMESPACE_END
