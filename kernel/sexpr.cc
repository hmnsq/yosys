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

#ଅନ୍ତର୍ଭାଵ "sexpr.h"

YOSYS_NAMESPACE_BEGIN

std::ostream &operator<<(std::ostream &os, SExpr ଅଚଳ &sexpr) {
    ଯଦି(sexpr.is_atom())
        os << sexpr.atom();
    ଅନ୍ୟଥା ଯଦି(sexpr.is_list()){
        os << "(";
        auto l = sexpr.list();
        ପାଇଁ(size_t i = 0; i < l.size(); i++) {
            ଯଦି(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }ଅନ୍ୟଥା
        os << "<invalid>";
    ଫେରନ୍ତୁ os;
}

 std::string SExpr::to_string() ଅଚଳ {
    std::stringstream ss;
    ss << *ଏହି;
    ଫେରନ୍ତୁ ss.str();
}

ଶୂନ୍ୟ SExprWriter::nl_if_pending() {
    ଯଦି(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = ମିଥ୍ୟା;
    }
}

ଶୂନ୍ୟ SExprWriter::puts(std::string_view s) {
    ଯଦି(s.empty()) ଫେରନ୍ତୁ;
    nl_if_pending();
    ପାଇଁ(auto c : s) {
        ଯଦି(c == '\n') {
            os << c;
            _pos = 0;
        } ଅନ୍ୟଥା {
            ଯଦି(_pos == 0) {
                ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < _indent; i++)
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା SExprWriter::check_fit(SExpr ଅଚଳ &sexpr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା space) {
    ଯଦି(sexpr.is_atom())
        ଫେରନ୍ତୁ space - sexpr.atom().size();
    ଅନ୍ୟଥା ଯଦି(sexpr.is_list()) {
        space -= 2;
        ଯଦି(sexpr.list().size() > 1)
            space -= sexpr.list().size() - 1;
        ପାଇଁ(auto arg : sexpr.list()) {
            ଯଦି(space < 0) ଭାଙ୍ଗ;
            space = check_fit(arg, space);
        }
        ଫେରନ୍ତୁ space;
    } ଅନ୍ୟଥା
        ଫେରନ୍ତୁ -1;
}

ଶୂନ୍ୟ SExprWriter::print(SExpr ଅଚଳ &sexpr, ତାର୍କିକ close, ତାର୍କିକ indent_rest) {
    ଯଦି(sexpr.is_atom())
        puts(sexpr.atom());
    ଅନ୍ୟଥା ଯଦି(sexpr.is_list()) {
        auto args = sexpr.list();
        puts("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        ତାର୍କିକ vertical = args.size() > 1 && check_fit(sexpr, max_ଧାଡ଼ି_width - _pos + 1) < 0;
        ଯଦି(vertical) _indent++;
        ପାଇଁ(size_t i = 0; i < args.size(); i++) {
            ଯଦି(i > 0) puts(vertical ? "\n" : " ");
            print(args[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!close && indent_rest) - vertical;
        ଯଦି(close)
            puts(")");
        ଅନ୍ୟଥା {
            _unclosed.ଠେଲ_back(indent_rest);
            _pending_nl = ସତ୍ୟ;
        }
    }ଅନ୍ୟଥା
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.to_string());
}

ଶୂନ୍ୟ SExprWriter::close(size_t n) {
    log_assert(_unclosed.size() - (unclosed_ଗଦା.empty() ? 0 : unclosed_ଗଦା.back()) >= n);
    ଯେତେବେଳେ(n-- > 0) {
        ତାର୍କିକ indented = _unclosed[_unclosed.size() - 1];
        _unclosed.କାଢ଼_back();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= max_ଧାଡ଼ି_width;
        ଯଦି(indented)
            _indent--;
        puts(")");
        _pending_nl = ସତ୍ୟ;
    }
}

ଶୂନ୍ୟ SExprWriter::comment(std::string ଅଚଳ &str, ତାର୍କିକ hanging) {
    ଯଦି(hanging) {
        ଯଦି(_pending_nl) {
            _pending_nl = ମିଥ୍ୟା;
            puts(" ");
        }
    }
    size_t i = 0, e;
    କର{
        e = str.find('\n', i);
        puts("; ");
        puts(std::string_view(str).substr(i, e - i));
        puts("\n");
        i = e + 1;
    }ଯେତେବେଳେ(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    ଯେତେବେଳେ(!unclosed_ଗଦା.empty())
        କାଢ଼();
    close(_unclosed.size());
    nl_if_pending();
}

YOSYS_NAMESPACE_END
