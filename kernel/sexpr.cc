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

#அந்தர்பாவ "sexpr.h"

YOSYS_NAMESPACE_BEGIN

std::ostream &operator<<(std::ostream &os, SExpr மாறிலி &sexpr) {
    எனில்(sexpr.is_atom())
        os << sexpr.atom();
    இல்லையெனில் எனில்(sexpr.is_list()){
        os << "(";
        auto l = sexpr.list();
        க்கு(size_t i = 0; i < l.size(); i++) {
            எனில்(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }இல்லையெனில்
        os << "<invalid>";
    திரும்பு os;
}

 std::string SExpr::to_string() மாறிலி {
    std::stringstream ss;
    ss << *இது;
    திரும்பு ss.str();
}

வெறுமை SExprWriter::nl_if_pending() {
    எனில்(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = பொய்;
    }
}

வெறுமை SExprWriter::puts(std::string_view s) {
    எனில்(s.empty()) திரும்பு;
    nl_if_pending();
    க்கு(auto c : s) {
        எனில்(c == '\n') {
            os << c;
            _pos = 0;
        } இல்லையெனில் {
            எனில்(_pos == 0) {
                க்கு(முழு i = 0; i < _indent; i++)
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
முழு SExprWriter::check_fit(SExpr மாறிலி &sexpr, முழு space) {
    எனில்(sexpr.is_atom())
        திரும்பு space - sexpr.atom().size();
    இல்லையெனில் எனில்(sexpr.is_list()) {
        space -= 2;
        எனில்(sexpr.list().size() > 1)
            space -= sexpr.list().size() - 1;
        க்கு(auto arg : sexpr.list()) {
            எனில்(space < 0) நிறுத்து;
            space = check_fit(arg, space);
        }
        திரும்பு space;
    } இல்லையெனில்
        திரும்பு -1;
}

வெறுமை SExprWriter::print(SExpr மாறிலி &sexpr, தருக்க close, தருக்க indent_rest) {
    எனில்(sexpr.is_atom())
        puts(sexpr.atom());
    இல்லையெனில் எனில்(sexpr.is_list()) {
        auto args = sexpr.list();
        puts("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        தருக்க vertical = args.size() > 1 && check_fit(sexpr, max_வரி_width - _pos + 1) < 0;
        எனில்(vertical) _indent++;
        க்கு(size_t i = 0; i < args.size(); i++) {
            எனில்(i > 0) puts(vertical ? "\n" : " ");
            print(args[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!close && indent_rest) - vertical;
        எனில்(close)
            puts(")");
        இல்லையெனில் {
            _unclosed.தள்ளு_back(indent_rest);
            _pending_nl = உண்மை;
        }
    }இல்லையெனில்
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.to_string());
}

வெறுமை SExprWriter::close(size_t n) {
    log_assert(_unclosed.size() - (unclosed_அடுக்கு.empty() ? 0 : unclosed_அடுக்கு.back()) >= n);
    போது(n-- > 0) {
        தருக்க indented = _unclosed[_unclosed.size() - 1];
        _unclosed.எடு_back();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= max_வரி_width;
        எனில்(indented)
            _indent--;
        puts(")");
        _pending_nl = உண்மை;
    }
}

வெறுமை SExprWriter::comment(std::string மாறிலி &str, தருக்க hanging) {
    எனில்(hanging) {
        எனில்(_pending_nl) {
            _pending_nl = பொய்;
            puts(" ");
        }
    }
    size_t i = 0, e;
    செய்{
        e = str.find('\n', i);
        puts("; ");
        puts(std::string_view(str).substr(i, e - i));
        puts("\n");
        i = e + 1;
    }போது(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    போது(!unclosed_அடுக்கு.empty())
        எடு();
    close(_unclosed.size());
    nl_if_pending();
}

YOSYS_NAMESPACE_END
