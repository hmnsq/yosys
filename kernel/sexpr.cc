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

#അന്തര്ഭാവ "sexpr.h"

YOSYS_NAMESPACE_BEGIN

std::ostream &operator<<(std::ostream &os, SExpr സ്ഥിരം &sexpr) {
    എങ്കിൽ(sexpr.is_atom())
        os << sexpr.atom();
    അല്ലെങ്കിൽ എങ്കിൽ(sexpr.is_list()){
        os << "(";
        auto l = sexpr.list();
        വേണ്ടി(size_t i = 0; i < l.size(); i++) {
            എങ്കിൽ(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }അല്ലെങ്കിൽ
        os << "<invalid>";
    തിരികെ os;
}

 std::string SExpr::to_string() സ്ഥിരം {
    std::stringstream ss;
    ss << *ഇത്;
    തിരികെ ss.str();
}

ശൂന്യം SExprWriter::nl_if_pending() {
    എങ്കിൽ(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = അസത്യം;
    }
}

ശൂന്യം SExprWriter::puts(std::string_view s) {
    എങ്കിൽ(s.empty()) തിരികെ;
    nl_if_pending();
    വേണ്ടി(auto c : s) {
        എങ്കിൽ(c == '\n') {
            os << c;
            _pos = 0;
        } അല്ലെങ്കിൽ {
            എങ്കിൽ(_pos == 0) {
                വേണ്ടി(പൂർണ്ണസംഖ്യ i = 0; i < _indent; i++)
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
പൂർണ്ണസംഖ്യ SExprWriter::check_fit(SExpr സ്ഥിരം &sexpr, പൂർണ്ണസംഖ്യ space) {
    എങ്കിൽ(sexpr.is_atom())
        തിരികെ space - sexpr.atom().size();
    അല്ലെങ്കിൽ എങ്കിൽ(sexpr.is_list()) {
        space -= 2;
        എങ്കിൽ(sexpr.list().size() > 1)
            space -= sexpr.list().size() - 1;
        വേണ്ടി(auto arg : sexpr.list()) {
            എങ്കിൽ(space < 0) നിർത്തുക;
            space = check_fit(arg, space);
        }
        തിരികെ space;
    } അല്ലെങ്കിൽ
        തിരികെ -1;
}

ശൂന്യം SExprWriter::print(SExpr സ്ഥിരം &sexpr, തർക്കം close, തർക്കം indent_rest) {
    എങ്കിൽ(sexpr.is_atom())
        puts(sexpr.atom());
    അല്ലെങ്കിൽ എങ്കിൽ(sexpr.is_list()) {
        auto args = sexpr.list();
        puts("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        തർക്കം vertical = args.size() > 1 && check_fit(sexpr, max_വരി_width - _pos + 1) < 0;
        എങ്കിൽ(vertical) _indent++;
        വേണ്ടി(size_t i = 0; i < args.size(); i++) {
            എങ്കിൽ(i > 0) puts(vertical ? "\n" : " ");
            print(args[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!close && indent_rest) - vertical;
        എങ്കിൽ(close)
            puts(")");
        അല്ലെങ്കിൽ {
            _unclosed.തള്ളുക_back(indent_rest);
            _pending_nl = സത്യം;
        }
    }അല്ലെങ്കിൽ
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.to_string());
}

ശൂന്യം SExprWriter::close(size_t n) {
    log_assert(_unclosed.size() - (unclosed_അട്ടി.empty() ? 0 : unclosed_അട്ടി.back()) >= n);
    ആകുമ്പോൾ(n-- > 0) {
        തർക്കം indented = _unclosed[_unclosed.size() - 1];
        _unclosed.എടുക്കുക_back();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= max_വരി_width;
        എങ്കിൽ(indented)
            _indent--;
        puts(")");
        _pending_nl = സത്യം;
    }
}

ശൂന്യം SExprWriter::comment(std::string സ്ഥിരം &str, തർക്കം hanging) {
    എങ്കിൽ(hanging) {
        എങ്കിൽ(_pending_nl) {
            _pending_nl = അസത്യം;
            puts(" ");
        }
    }
    size_t i = 0, e;
    ചെയ്യുക{
        e = str.find('\n', i);
        puts("; ");
        puts(std::string_view(str).substr(i, e - i));
        puts("\n");
        i = e + 1;
    }ആകുമ്പോൾ(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    ആകുമ്പോൾ(!unclosed_അട്ടി.empty())
        എടുക്കുക();
    close(_unclosed.size());
    nl_if_pending();
}

YOSYS_NAMESPACE_END
