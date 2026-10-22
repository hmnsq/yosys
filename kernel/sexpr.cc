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

#અન્તર્ભાવ "sexpr.h"

YOSYS_NAMESPACE_BEGIN

std::ostream &operator<<(std::ostream &os, SExpr અચલ &sexpr) {
    જો(sexpr.is_atom())
        os << sexpr.atom();
    નહીંતર જો(sexpr.is_list()){
        os << "(";
        auto l = sexpr.list();
        માટે(size_t i = 0; i < l.size(); i++) {
            જો(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }નહીંતર
        os << "<invalid>";
    પાછા os;
}

 std::string SExpr::to_string() અચલ {
    std::stringstream ss;
    ss << *આ;
    પાછા ss.str();
}

શૂન્ય SExprWriter::nl_if_pending() {
    જો(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = ખોટું;
    }
}

શૂન્ય SExprWriter::puts(std::string_view s) {
    જો(s.empty()) પાછા;
    nl_if_pending();
    માટે(auto c : s) {
        જો(c == '\n') {
            os << c;
            _pos = 0;
        } નહીંતર {
            જો(_pos == 0) {
                માટે(પૂર્ણાંક i = 0; i < _indent; i++)
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
પૂર્ણાંક SExprWriter::check_fit(SExpr અચલ &sexpr, પૂર્ણાંક space) {
    જો(sexpr.is_atom())
        પાછા space - sexpr.atom().size();
    નહીંતર જો(sexpr.is_list()) {
        space -= 2;
        જો(sexpr.list().size() > 1)
            space -= sexpr.list().size() - 1;
        માટે(auto arg : sexpr.list()) {
            જો(space < 0) તોડો;
            space = check_fit(arg, space);
        }
        પાછા space;
    } નહીંતર
        પાછા -1;
}

શૂન્ય SExprWriter::print(SExpr અચલ &sexpr, તાર્કિક close, તાર્કિક indent_rest) {
    જો(sexpr.is_atom())
        puts(sexpr.atom());
    નહીંતર જો(sexpr.is_list()) {
        auto args = sexpr.list();
        puts("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        તાર્કિક vertical = args.size() > 1 && check_fit(sexpr, max_લીટી_width - _pos + 1) < 0;
        જો(vertical) _indent++;
        માટે(size_t i = 0; i < args.size(); i++) {
            જો(i > 0) puts(vertical ? "\n" : " ");
            print(args[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!close && indent_rest) - vertical;
        જો(close)
            puts(")");
        નહીંતર {
            _unclosed.ધકેલો_back(indent_rest);
            _pending_nl = સાચું;
        }
    }નહીંતર
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.to_string());
}

શૂન્ય SExprWriter::close(size_t n) {
    log_assert(_unclosed.size() - (unclosed_ઢગલો.empty() ? 0 : unclosed_ઢગલો.back()) >= n);
    જ્યારે(n-- > 0) {
        તાર્કિક indented = _unclosed[_unclosed.size() - 1];
        _unclosed.કાઢો_back();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= max_લીટી_width;
        જો(indented)
            _indent--;
        puts(")");
        _pending_nl = સાચું;
    }
}

શૂન્ય SExprWriter::comment(std::string અચલ &str, તાર્કિક hanging) {
    જો(hanging) {
        જો(_pending_nl) {
            _pending_nl = ખોટું;
            puts(" ");
        }
    }
    size_t i = 0, e;
    કરો{
        e = str.find('\n', i);
        puts("; ");
        puts(std::string_view(str).substr(i, e - i));
        puts("\n");
        i = e + 1;
    }જ્યારે(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    જ્યારે(!unclosed_ઢગલો.empty())
        કાઢો();
    close(_unclosed.size());
    nl_if_pending();
}

YOSYS_NAMESPACE_END
