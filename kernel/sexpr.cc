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

#ਅਨ੍ਤਰ੍ਭਾਵ "sexpr.h"

YOSYS_NAMESPACE_BEGIN

std::ostream &ਸੰਚਾਲਕ<<(std::ostream &os, SExpr ਅਚਲ &sexpr) {
    ਜੇ(sexpr.is_atom())
        os << sexpr.atom();
    ਨਹੀਂਤਾਂ ਜੇ(sexpr.is_list()){
        os << "(";
        ਸਵੈਚਾਲਿਤ l = sexpr.list();
        ਲਈ(ਮਾਪ_t i = 0; i < l.ਮਾਪ(); i++) {
            ਜੇ(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }ਨਹੀਂਤਾਂ
        os << "<invalid>";
    ਵਾਪਸ os;
}

 std::string SExpr::ਮੰਜ਼ਿਲ_string() ਅਚਲ {
    std::stringstream ss;
    ss << *ਇਹ;
    ਵਾਪਸ ss.str();
}

ਖਾਲੀ SExprWriter::nl_if_pending() {
    ਜੇ(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = ਝੂਠ;
    }
}

ਖਾਲੀ SExprWriter::puts(std::string_view s) {
    ਜੇ(s.ਖਾਲੀ_ਥਾਵਾਂ()) ਵਾਪਸ;
    nl_if_pending();
    ਲਈ(ਸਵੈਚਾਲਿਤ c : s) {
        ਜੇ(c == '\n') {
            os << c;
            _pos = 0;
        } ਨਹੀਂਤਾਂ {
            ਜੇ(_pos == 0) {
                ਲਈ(ਪੂਰਨ i = 0; i < _indent; i++)
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
ਪੂਰਨ SExprWriter::check_fit(SExpr ਅਚਲ &sexpr, ਪੂਰਨ space) {
    ਜੇ(sexpr.is_atom())
        ਵਾਪਸ space - sexpr.atom().ਮਾਪ();
    ਨਹੀਂਤਾਂ ਜੇ(sexpr.is_list()) {
        space -= 2;
        ਜੇ(sexpr.list().ਮਾਪ() > 1)
            space -= sexpr.list().ਮਾਪ() - 1;
        ਲਈ(ਸਵੈਚਾਲਿਤ arg : sexpr.list()) {
            ਜੇ(space < 0) ਰੋਕੋ;
            space = check_fit(arg, space);
        }
        ਵਾਪਸ space;
    } ਨਹੀਂਤਾਂ
        ਵਾਪਸ -1;
}

ਖਾਲੀ SExprWriter::print(SExpr ਅਚਲ &sexpr, ਤਾਰਕਿਕ close, ਤਾਰਕਿਕ indent_rest) {
    ਜੇ(sexpr.is_atom())
        puts(sexpr.atom());
    ਨਹੀਂਤਾਂ ਜੇ(sexpr.is_list()) {
        ਸਵੈਚਾਲਿਤ ਦਲੀਲਾਂ_2 = sexpr.list();
        puts("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        ਤਾਰਕਿਕ vertical = ਦਲੀਲਾਂ_2.ਮਾਪ() > 1 && check_fit(sexpr, max_ਸਤਰ_ਚੌੜਾਈ - _pos + 1) < 0;
        ਜੇ(vertical) _indent++;
        ਲਈ(ਮਾਪ_t i = 0; i < ਦਲੀਲਾਂ_2.ਮਾਪ(); i++) {
            ਜੇ(i > 0) puts(vertical ? "\n" : " ");
            print(ਦਲੀਲਾਂ_2[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!close && indent_rest) - vertical;
        ਜੇ(close)
            puts(")");
        ਨਹੀਂਤਾਂ {
            _unclosed.ਧੱਕੋ_ਪਿੱਠ(indent_rest);
            _pending_nl = ਸੱਚ;
        }
    }ਨਹੀਂਤਾਂ
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.ਮੰਜ਼ਿਲ_string());
}

ਖਾਲੀ SExprWriter::close(ਮਾਪ_t n) {
    log_assert(_unclosed.ਮਾਪ() - (unclosed_ਢੇਰ.ਖਾਲੀ_ਥਾਵਾਂ() ? 0 : unclosed_ਢੇਰ.ਪਿੱਠ()) >= n);
    ਜਦੋਂ(n-- > 0) {
        ਤਾਰਕਿਕ indented = _unclosed[_unclosed.ਮਾਪ() - 1];
        _unclosed.ਕੱਢੋ_ਪਿੱਠ();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= max_ਸਤਰ_ਚੌੜਾਈ;
        ਜੇ(indented)
            _indent--;
        puts(")");
        _pending_nl = ਸੱਚ;
    }
}

ਖਾਲੀ SExprWriter::comment(std::string ਅਚਲ &str, ਤਾਰਕਿਕ hanging) {
    ਜੇ(hanging) {
        ਜੇ(_pending_nl) {
            _pending_nl = ਝੂਠ;
            puts(" ");
        }
    }
    ਮਾਪ_t i = 0, e;
    ਕਰੋ{
        e = str.ਲੱਭੋ('\n', i);
        puts("; ");
        puts(std::string_view(str).substr(i, e - i));
        puts("\n");
        i = e + 1;
    }ਜਦੋਂ(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    ਜਦੋਂ(!unclosed_ਢੇਰ.ਖਾਲੀ_ਥਾਵਾਂ())
        ਕੱਢੋ();
    close(_unclosed.ਮਾਪ());
    nl_if_pending();
}

YOSYS_NAMESPACE_ਸਿਰਾ
