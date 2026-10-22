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

#ضمن "sexpr.h"

YOSYS_NAMESPACE_BEGIN

std::ostream &operator<<(std::ostream &os, SExpr ثابت &sexpr) {
    ܐܢ(sexpr.is_atom())
        os << sexpr.atom();
    ܐܚܪܢܐ ܐܢ(sexpr.is_list()){
        os << "(";
        auto l = sexpr.list();
        لكل(size_t i = 0; i < l.size(); i++) {
            ܐܢ(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }ܐܚܪܢܐ
        os << "<invalid>";
    ܦܢܐ os;
}

 std::string SExpr::to_string() ثابت {
    std::stringstream ss;
    ss << *هذا;
    ܦܢܐ ss.str();
}

فراغ SExprWriter::nl_if_pending() {
    ܐܢ(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = ܕܓܠܐ;
    }
}

فراغ SExprWriter::puts(std::string_view s) {
    ܐܢ(s.empty()) ܦܢܐ;
    nl_if_pending();
    لكل(auto c : s) {
        ܐܢ(c == '\n') {
            os << c;
            _pos = 0;
        } ܐܚܪܢܐ {
            ܐܢ(_pos == 0) {
                لكل(صحيح i = 0; i < _indent; i++)
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
صحيح SExprWriter::check_fit(SExpr ثابت &sexpr, صحيح space) {
    ܐܢ(sexpr.is_atom())
        ܦܢܐ space - sexpr.atom().size();
    ܐܚܪܢܐ ܐܢ(sexpr.is_list()) {
        space -= 2;
        ܐܢ(sexpr.list().size() > 1)
            space -= sexpr.list().size() - 1;
        لكل(auto arg : sexpr.list()) {
            ܐܢ(space < 0) اكسر;
            space = check_fit(arg, space);
        }
        ܦܢܐ space;
    } ܐܚܪܢܐ
        ܦܢܐ -1;
}

فراغ SExprWriter::print(SExpr ثابت &sexpr, منطقي close, منطقي indent_rest) {
    ܐܢ(sexpr.is_atom())
        puts(sexpr.atom());
    ܐܚܪܢܐ ܐܢ(sexpr.is_list()) {
        auto args = sexpr.list();
        puts("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        منطقي vertical = args.size() > 1 && check_fit(sexpr, _max_line_width - _pos + 1) < 0;
        ܐܢ(vertical) _indent++;
        لكل(size_t i = 0; i < args.size(); i++) {
            ܐܢ(i > 0) puts(vertical ? "\n" : " ");
            print(args[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!close && indent_rest) - vertical;
        ܐܢ(close)
            puts(")");
        ܐܚܪܢܐ {
            _unclosed.push_back(indent_rest);
            _pending_nl = ܫܪܪܐ;
        }
    }ܐܚܪܢܐ
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.to_string());
}

فراغ SExprWriter::close(size_t n) {
    log_assert(_unclosed.size() - (_unclosed_stack.empty() ? 0 : _unclosed_stack.back()) >= n);
    ܟܡܐ(n-- > 0) {
        منطقي indented = _unclosed[_unclosed.size() - 1];
        _unclosed.pop_back();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= _max_line_width;
        ܐܢ(indented)
            _indent--;
        puts(")");
        _pending_nl = ܫܪܪܐ;
    }
}

فراغ SExprWriter::comment(std::string ثابت &str, منطقي hanging) {
    ܐܢ(hanging) {
        ܐܢ(_pending_nl) {
            _pending_nl = ܕܓܠܐ;
            puts(" ");
        }
    }
    size_t i = 0, e;
    ܥܒܕ{
        e = str.find('\n', i);
        puts("; ");
        puts(std::string_view(str).substr(i, e - i));
        puts("\n");
        i = e + 1;
    }ܟܡܐ(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    ܟܡܐ(!_unclosed_stack.empty())
        pop();
    close(_unclosed.size());
    nl_if_pending();
}

YOSYS_NAMESPACE_END
