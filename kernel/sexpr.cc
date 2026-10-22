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

#ಅನ್ತರ್ಭಾವ "sexpr.h"

YOSYS_NAMESPACE_BEGIN

std::ostream &operator<<(std::ostream &os, SExpr ಧ್ರುವ &sexpr) {
    ಯದಿ(sexpr.is_atom())
        os << sexpr.atom();
    ಅನ್ಯಥಾ ಯದಿ(sexpr.is_list()){
        os << "(";
        auto l = sexpr.list();
        ಪ್ರತಿ(size_t i = 0; i < l.size(); i++) {
            ಯದಿ(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }ಅನ್ಯಥಾ
        os << "<invalid>";
    ಪ್ರತ್ಯಾಗಮ os;
}

 std::string SExpr::to_string() ಧ್ರುವ {
    std::stringstream ss;
    ss << *ಏತತ್;
    ಪ್ರತ್ಯಾಗಮ ss.str();
}

ಶೂನ್ಯ SExprWriter::nl_if_pending() {
    ಯದಿ(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = ಅಸತ್ಯ;
    }
}

ಶೂನ್ಯ SExprWriter::puts(std::string_view s) {
    ಯದಿ(s.empty()) ಪ್ರತ್ಯಾಗಮ;
    nl_if_pending();
    ಪ್ರತಿ(auto c : s) {
        ಯದಿ(c == '\n') {
            os << c;
            _pos = 0;
        } ಅನ್ಯಥಾ {
            ಯದಿ(_pos == 0) {
                ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i = 0; i < _indent; i++)
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
ಪೂರ್ಣಾಙ್ಕ SExprWriter::check_fit(SExpr ಧ್ರುವ &sexpr, ಪೂರ್ಣಾಙ್ಕ space) {
    ಯದಿ(sexpr.is_atom())
        ಪ್ರತ್ಯಾಗಮ space - sexpr.atom().size();
    ಅನ್ಯಥಾ ಯದಿ(sexpr.is_list()) {
        space -= 2;
        ಯದಿ(sexpr.list().size() > 1)
            space -= sexpr.list().size() - 1;
        ಪ್ರತಿ(auto arg : sexpr.list()) {
            ಯದಿ(space < 0) ಭಙ್ಗ;
            space = check_fit(arg, space);
        }
        ಪ್ರತ್ಯಾಗಮ space;
    } ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ -1;
}

ಶೂನ್ಯ SExprWriter::print(SExpr ಧ್ರುವ &sexpr, ತಾರ್ಕಿಕ close, ತಾರ್ಕಿಕ indent_rest) {
    ಯದಿ(sexpr.is_atom())
        puts(sexpr.atom());
    ಅನ್ಯಥಾ ಯದಿ(sexpr.is_list()) {
        auto args = sexpr.list();
        puts("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        ತಾರ್ಕಿಕ vertical = args.size() > 1 && check_fit(sexpr, max_ಸಾಲು_width - _pos + 1) < 0;
        ಯದಿ(vertical) _indent++;
        ಪ್ರತಿ(size_t i = 0; i < args.size(); i++) {
            ಯದಿ(i > 0) puts(vertical ? "\n" : " ");
            print(args[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!close && indent_rest) - vertical;
        ಯದಿ(close)
            puts(")");
        ಅನ್ಯಥಾ {
            _unclosed.ತಳ್ಳು_back(indent_rest);
            _pending_nl = ಸತ್ಯ;
        }
    }ಅನ್ಯಥಾ
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.to_string());
}

ಶೂನ್ಯ SExprWriter::close(size_t n) {
    log_assert(_unclosed.size() - (unclosed_ರಾಶಿ.empty() ? 0 : unclosed_ರಾಶಿ.back()) >= n);
    ಯಾವತ್(n-- > 0) {
        ತಾರ್ಕಿಕ indented = _unclosed[_unclosed.size() - 1];
        _unclosed.ತೆಗೆ_back();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= max_ಸಾಲು_width;
        ಯದಿ(indented)
            _indent--;
        puts(")");
        _pending_nl = ಸತ್ಯ;
    }
}

ಶೂನ್ಯ SExprWriter::comment(std::string ಧ್ರುವ &str, ತಾರ್ಕಿಕ hanging) {
    ಯದಿ(hanging) {
        ಯದಿ(_pending_nl) {
            _pending_nl = ಅಸತ್ಯ;
            puts(" ");
        }
    }
    size_t i = 0, e;
    ಕುರು{
        e = str.find('\n', i);
        puts("; ");
        puts(std::string_view(str).substr(i, e - i));
        puts("\n");
        i = e + 1;
    }ಯಾವತ್(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    ಯಾವತ್(!unclosed_ರಾಶಿ.empty())
        ತೆಗೆ();
    close(_unclosed.size());
    nl_if_pending();
}

YOSYS_NAMESPACE_END
