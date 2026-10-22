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

#అన్తర్భావ "sexpr.h"

YOSYS_NAMESPACE_BEGIN

std::ostream &క్రియాకారకం<<(std::ostream &os, SExpr స్థిరరాశి &sexpr) {
    ఒకవేళ(sexpr.is_atom())
        os << sexpr.atom();
    లేకపోతే ఒకవేళ(sexpr.is_list()){
        os << "(";
        స్వయంచాలక l = sexpr.list();
        కొరకు(కొలత_t i = 0; i < l.కొలత(); i++) {
            ఒకవేళ(i > 0) os << " ";
            os << l[i];
        }
        os << ")";
    }లేకపోతే
        os << "<invalid>";
    తిరిగి os;
}

 std::string SExpr::గమ్యం_string() స్థిరరాశి {
    std::stringstream ss;
    ss << *ఇది;
    తిరిగి ss.str();
}

శూన్య SExprWriter::nl_if_pending() {
    ఒకవేళ(_pending_nl) {
        os << '\n';
        _pos = 0;
        _pending_nl = అబద్ధం;
    }
}

శూన్య SExprWriter::puts(std::string_view s) {
    ఒకవేళ(s.ఖాళీలు()) తిరిగి;
    nl_if_pending();
    కొరకు(స్వయంచాలక c : s) {
        ఒకవేళ(c == '\n') {
            os << c;
            _pos = 0;
        } లేకపోతే {
            ఒకవేళ(_pos == 0) {
                కొరకు(పూర్ణాంకం i = 0; i < _indent; i++)
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
పూర్ణాంకం SExprWriter::check_fit(SExpr స్థిరరాశి &sexpr, పూర్ణాంకం space) {
    ఒకవేళ(sexpr.is_atom())
        తిరిగి space - sexpr.atom().కొలత();
    లేకపోతే ఒకవేళ(sexpr.is_list()) {
        space -= 2;
        ఒకవేళ(sexpr.list().కొలత() > 1)
            space -= sexpr.list().కొలత() - 1;
        కొరకు(స్వయంచాలక arg : sexpr.list()) {
            ఒకవేళ(space < 0) ఆపు;
            space = check_fit(arg, space);
        }
        తిరిగి space;
    } లేకపోతే
        తిరిగి -1;
}

శూన్య SExprWriter::print(SExpr స్థిరరాశి &sexpr, తార్కిక close, తార్కిక indent_rest) {
    ఒకవేళ(sexpr.is_atom())
        puts(sexpr.atom());
    లేకపోతే ఒకవేళ(sexpr.is_list()) {
        స్వయంచాలక వాదనలు_2 = sexpr.list();
        puts("(");
        // Expressions are printed horizontally if they fit on the line.
        // We do the check *after* puts("(") to make sure that _pos is accurate.
        // (Otherwise there could be a pending newline + indentation)
        తార్కిక vertical = వాదనలు_2.కొలత() > 1 && check_fit(sexpr, max_పంక్తి_వెడల్పు - _pos + 1) < 0;
        ఒకవేళ(vertical) _indent++;
        కొరకు(కొలత_t i = 0; i < వాదనలు_2.కొలత(); i++) {
            ఒకవేళ(i > 0) puts(vertical ? "\n" : " ");
            print(వాదనలు_2[i]);
        }
        // Any remaining arguments are currently always printed vertically,
        // but are not indented if indent_rest = false.
        _indent += (!close && indent_rest) - vertical;
        ఒకవేళ(close)
            puts(")");
        లేకపోతే {
            _unclosed.నెట్టు_వెనుక(indent_rest);
            _pending_nl = నిజం;
        }
    }లేకపోతే
        log_error("shouldn't happen: SExpr '%s' is neither an atom nor a list\n", sexpr.గమ్యం_string());
}

శూన్య SExprWriter::close(కొలత_t n) {
    log_assert(_unclosed.కొలత() - (unclosed_దొంతర.ఖాళీలు() ? 0 : unclosed_దొంతర.వెనుక()) >= n);
    అయితే(n-- > 0) {
        తార్కిక indented = _unclosed[_unclosed.కొలత() - 1];
        _unclosed.తీయి_వెనుక();
        // Only print ) on the same line if it fits.
        _pending_nl = _pos >= max_పంక్తి_వెడల్పు;
        ఒకవేళ(indented)
            _indent--;
        puts(")");
        _pending_nl = నిజం;
    }
}

శూన్య SExprWriter::comment(std::string స్థిరరాశి &str, తార్కిక hanging) {
    ఒకవేళ(hanging) {
        ఒకవేళ(_pending_nl) {
            _pending_nl = అబద్ధం;
            puts(" ");
        }
    }
    కొలత_t i = 0, e;
    చేయి{
        e = str.కనుగొను('\n', i);
        puts("; ");
        puts(std::string_view(str).substr(i, e - i));
        puts("\n");
        i = e + 1;
    }అయితే(e != std::string::npos);
}

SExprWriter::~SExprWriter() {
    అయితే(!unclosed_దొంతర.ఖాళీలు())
        తీయి();
    close(_unclosed.కొలత());
    nl_if_pending();
}

YOSYS_NAMESPACE_చివర
