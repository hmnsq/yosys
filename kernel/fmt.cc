/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2020  whitequark <whitequark@whitequark.org>
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

#ਅਨ੍ਤਰ੍ਭਾਵ "libs/bigint/BigUnsigned.hh"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fmt.h"

USING_YOSYS_NAMESPACE

ਖਾਲੀ Fmt::append_literal(ਅਚਲ std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.ਧੱਕੋ_ਪਿੱਠ(part);
}

ਖਾਲੀ Fmt::parse_rtlil(ਅਚਲ RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec ਦਲੀਲਾਂ_2 = cell->get_ਪੋਰਟ(ID(ਦਲੀਲਾਂ));
	parts.clear();

	FmtPart part;
	ਲਈ (ਮਾਪ_t i = 0; i < fmt.ਮਾਪ(); i++) {
		ਜੇ (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} ਨਹੀਂਤਾਂ ਜੇ (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == '}') {
			log_assert(ਝੂਠ && "Unexpected '}' in format string");
		} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == '{') {
			ਜੇ (!part.str.ਖਾਲੀ_ਥਾਵਾਂ()) {
				part.type = FmtPart::LITERAL;
				parts.ਧੱਕੋ_ਪਿੱਠ(part);
				part = {};
			}

			ਜੇ (++i == fmt.ਮਾਪ())
				log_assert(ਝੂਠ && "Unexpected end in format substitution");

			ਮਾਪ_t arg_ਮਾਪ = 0;
			ਲਈ (; i < fmt.ਮਾਪ(); i++) {
				ਜੇ (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_ਮਾਪ *= 10;
					arg_ਮਾਪ += fmt[i] - '0';
				} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == ':') {
					++i;
					ਰੋਕੋ;
				} ਨਹੀਂਤਾਂ {
					log_assert(ਝੂਠ && "Unexpected character in format substitution");
				}
			}
			ਜੇ (i == fmt.ਮਾਪ())
				log_assert(ਝੂਠ && "Unexpected end in format substitution");

			ਜੇ ((ਮਾਪ_t)ਦਲੀਲਾਂ_2.ਮਾਪ() < arg_ਮਾਪ)
				log_assert(ਝੂਠ && "Format part overruns arguments");
			part.sig = ਦਲੀਲਾਂ_2.ਕੱਢੋ_2(0, arg_ਮਾਪ);
			ਦਲੀਲਾਂ_2.remove(0, arg_ਮਾਪ);

			ਜੇ (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				ਜਾਓ success;
			}

			ਜੇ (fmt[i] == '>')
				part.justify = FmtPart::ਸੱਜਾ;
			ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == '<')
				part.justify = FmtPart::ਖੱਬਾ;
			ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			ਨਹੀਂਤਾਂ
				log_assert(ਝੂਠ && "Unexpected justification in format substitution");
			ਜੇ (++i == fmt.ਮਾਪ())
				log_assert(ਝੂਠ && "Unexpected end in format substitution");

			part.padding = fmt[i];
			ਜੇ (++i == fmt.ਮਾਪ())
				log_assert(ਝੂਠ && "Unexpected end in format substitution");

			ਲਈ (; i < fmt.ਮਾਪ(); i++) {
				ਜੇ (fmt[i] >= '0' && fmt[i] <= '9') {
					part.ਚੌੜਾਈ *= 10;
					part.ਚੌੜਾਈ += fmt[i] - '0';
					ਜਾਰੀ;
				} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.ਅਧਾਰ = 2;
				} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.ਅਧਾਰ = 8;
				} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.ਅਧਾਰ = 10;
				} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.ਅਧਾਰ = 16;
				} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.ਅਧਾਰ = 16;
					part.hex_ਉੱਤਲਾ = ਸੱਚ;
				} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 't') {
					part.type = FmtPart::VLOG_ਸਮਾਂ;
				} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_ਸਮਾਂ;
					part.realtime = ਸੱਚ;
				} ਨਹੀਂਤਾਂ {
					log_assert(ਝੂਠ && "Unexpected character in format substitution");
				}
				++i;
				ਰੋਕੋ;
			}
			ਜੇ (i == fmt.ਮਾਪ())
				log_assert(ਝੂਠ && "Unexpected end in format substitution");

			ਜੇ (part.type == FmtPart::INTEGER) {
				ਜੇ (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					ਜੇ (++i == fmt.ਮਾਪ())
						log_assert(ਝੂਠ && "Unexpected end in format substitution");
				} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					ਜੇ (++i == fmt.ਮਾਪ())
						log_assert(ਝੂਠ && "Unexpected end in format substitution");
				} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					ਜੇ (++i == fmt.ਮਾਪ())
						log_assert(ਝੂਠ && "Unexpected end in format substitution");
				} ਨਹੀਂਤਾਂ {
					// also accept no sign character and treat like MINUS for compatibility
				}

				ਜੇ (fmt[i] == '#') {
					part.show_ਅਧਾਰ = ਸੱਚ;
					++i;
				}
				ਜੇ (fmt[i] == '_') {
					part.ਸਮੂਹ = ਸੱਚ;
					++i;
				}

				ਜੇ (fmt[i] == 'u')
					part.signed_ = ਝੂਠ;
				ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 's')
					part.signed_ = ਸੱਚ;
				ਨਹੀਂਤਾਂ
					log_assert(ਝੂਠ && "Unexpected character in format substitution");
				ਜੇ (++i == fmt.ਮਾਪ())
					log_assert(ਝੂਠ && "Unexpected end in format substitution");
			}

		success:
			ਜੇ (fmt[i] != '}')
				log_assert(ਝੂਠ && "Expected '}' after format substitution");

			parts.ਧੱਕੋ_ਪਿੱਠ(part);
			part = {};
		} ਨਹੀਂਤਾਂ {
			part.str += fmt[i];
		}
	}
	ਜੇ (!part.str.ਖਾਲੀ_ਥਾਵਾਂ()) {
		part.type = FmtPart::LITERAL;
		parts.ਧੱਕੋ_ਪਿੱਠ(part);
	}
}

ਖਾਲੀ Fmt::emit_rtlil(RTLIL::Cell *cell) ਅਚਲ {
	std::string fmt;
	RTLIL::SigSpec ਦਲੀਲਾਂ_2;

	ਲਈ (ਸਵੈਚਾਲਿਤ &part : parts) {
		ਚੋਣ (part.type) {
			ਹਾਲਤ FmtPart::LITERAL:
				ਲਈ (ਅੱਖਰ c : part.str) {
					ਜੇ (c == '{')
						fmt += "{{";
					ਨਹੀਂਤਾਂ ਜੇ (c == '}')
						fmt += "}}";
					ਨਹੀਂਤਾਂ
						fmt += c;
				}
				ਰੋਕੋ;

			ਹਾਲਤ FmtPart::UNICHAR:
				log_assert(part.sig.ਮਾਪ() <= 32);
				fmt += "{U}";
				ਰੋਕੋ;

			ਹਾਲਤ FmtPart::VLOG_ਸਮਾਂ:
				log_assert(part.sig.ਮਾਪ() == 0);
				YS_FALLTHROUGH
			ਹਾਲਤ FmtPart::STRING:
				log_assert(part.sig.ਮਾਪ() % 8 == 0);
				YS_FALLTHROUGH
			ਹਾਲਤ FmtPart::INTEGER:
				ਦਲੀਲਾਂ_2.append(part.sig);
				fmt += '{';
				fmt += std::ਮੰਜ਼ਿਲ_string(part.sig.ਮਾਪ());
				fmt += ':';
				ਜੇ (part.justify == FmtPart::ਸੱਜਾ)
					fmt += '>';
				ਨਹੀਂਤਾਂ ਜੇ (part.justify == FmtPart::ਖੱਬਾ)
					fmt += '<';
				ਨਹੀਂਤਾਂ ਜੇ (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				ਨਹੀਂਤਾਂ log_abort();
				log_assert(part.ਚੌੜਾਈ == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				ਜੇ (part.ਚੌੜਾਈ > 0)
					fmt += std::ਮੰਜ਼ਿਲ_string(part.ਚੌੜਾਈ);
				ਜੇ (part.type == FmtPart::INTEGER) {
					ਚੋਣ (part.ਅਧਾਰ) {
						ਹਾਲਤ  2: fmt += 'b'; ਰੋਕੋ;
						ਹਾਲਤ  8: fmt += 'o'; ਰੋਕੋ;
						ਹਾਲਤ 10: fmt += 'd'; ਰੋਕੋ;
						ਹਾਲਤ 16: fmt += part.hex_ਉੱਤਲਾ ? 'H' : 'h'; ਰੋਕੋ;
						ਮੂਲ: log_abort();
					}
					ਚੋਣ (part.sign) {
						ਹਾਲਤ FmtPart::MINUS:       fmt += '-'; ਰੋਕੋ;
						ਹਾਲਤ FmtPart::PLUS_MINUS:  fmt += '+'; ਰੋਕੋ;
						ਹਾਲਤ FmtPart::SPACE_MINUS: fmt += ' '; ਰੋਕੋ;
					}
					fmt += part.show_ਅਧਾਰ ? "#" : "";
					fmt += part.ਸਮੂਹ ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} ਨਹੀਂਤਾਂ ਜੇ (part.type == FmtPart::STRING) {
					fmt += 'c';
				} ਨਹੀਂਤਾਂ ਜੇ (part.type == FmtPart::VLOG_ਸਮਾਂ) {
					ਜੇ (part.realtime)
						fmt += 'r';
					ਨਹੀਂਤਾਂ
						fmt += 't';
				} ਨਹੀਂਤਾਂ log_abort();
				fmt += '}';
				ਰੋਕੋ;

			ਮੂਲ: log_abort();
		}
	}

	cell->ਸੈੱਟ_Param(ID(FORMAT), fmt);
	cell->ਸੈੱਟ_Param(ID(ਦਲੀਲਾਂ_ਚੌੜਾਈ), ਦਲੀਲਾਂ_2.ਮਾਪ());
	cell->ਸੈੱਟ_ਪੋਰਟ(ID(ਦਲੀਲਾਂ), ਦਲੀਲਾਂ_2);
}

ਸਥਿਰ ਮਾਪ_t compute_required_decimal_places(ਮਾਪ_t ਮਾਪ, ਤਾਰਕਿਕ signed_)
{
	BigUnsigned max;
	ਜੇ (!signed_)
		max.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਮਾਪ, ਸੱਚ);
	ਨਹੀਂਤਾਂ
		max.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਮਾਪ - 1, ਸੱਚ);
	ਮਾਪ_t places = 0;
	ਜਦੋਂ (!max.isZero()) {
		places++;
		max /= 10;
	}
	ਜੇ (signed_)
		places++;
	ਵਾਪਸ places;
}

ਸਥਿਰ ਮਾਪ_t compute_required_nondecimal_places(ਮਾਪ_t ਮਾਪ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ)
{
	log_assert(ਅਧਾਰ != 10);
	BigUnsigned max;
	max.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਮਾਪ - 1, ਸੱਚ);
	ਮਾਪ_t places = 0;
	ਜਦੋਂ (!max.isZero()) {
		places++;
		max /= ਅਧਾਰ;
	}
	ਵਾਪਸ places;
}

// Only called for integers, either when:
//
// (a) passed without a format string (e.g. "$display(a);"), or
//
// (b) the corresponding format specifier has no leading zero, e.g. "%b",
// "%20h", "%-10d".
//
// In these cases, for binary/octal/hex, we always zero-pad to the size of the
// signal; i.e. whether "%h" or "%10h" or "%-20h" is used, if the corresponding
// signal is 32'h1234, "00001234" will always be a substring of the output.
//
// For case (a), we have no specified width, so there is nothing more to do.
//
// For case (b), because we are only called with no leading zero on the
// specifier, any specified width beyond the signal size is therefore space
// padding, whatever the justification.
//
// For decimal, we do no zero-padding, instead space-padding to the size
// required for the signal's largest value.  This is per other Verilog
// implementations, and intuitively makes sense as decimal representations lack
// a discrete mapping of digits to bit groups.  Decimals may also show sign and
// must accommodate this, whereas other representations do not.
ਖਾਲੀ Fmt::apply_verilog_automatic_sizing_and_ਜੋੜੋ(FmtPart &part)
{
	ਜੇ (part.ਅਧਾਰ == 10) {
		ਮਾਪ_t places = compute_required_decimal_places(part.sig.ਮਾਪ(), part.signed_);
		part.padding = ' ';
		part.ਚੌੜਾਈ = std::max(part.ਚੌੜਾਈ, places);
		parts.ਧੱਕੋ_ਪਿੱਠ(part);
		ਵਾਪਸ;
	}

	part.padding = '0';

	ਮਾਪ_t places = compute_required_nondecimal_places(part.sig.ਮਾਪ(), part.ਅਧਾਰ);
	ਜੇ (part.ਚੌੜਾਈ < places) {
		part.justify = FmtPart::ਸੱਜਾ;
		part.ਚੌੜਾਈ = places;
		parts.ਧੱਕੋ_ਪਿੱਠ(part);
	} ਨਹੀਂਤਾਂ ਜੇ (part.ਚੌੜਾਈ == places) {
		parts.ਧੱਕੋ_ਪਿੱਠ(part);
	} ਨਹੀਂਤਾਂ ਜੇ (part.ਚੌੜਾਈ > places) {
		ਸਵੈਚਾਲਿਤ gap = std::string(part.ਚੌੜਾਈ - places, ' ');
		part.ਚੌੜਾਈ = places;

		ਜੇ (part.justify == FmtPart::ਸੱਜਾ) {
			append_literal(gap);
			parts.ਧੱਕੋ_ਪਿੱਠ(part);
		} ਨਹੀਂਤਾਂ {
			part.justify = FmtPart::ਸੱਜਾ;
			parts.ਧੱਕੋ_ਪਿੱਠ(part);
			append_literal(gap);
		}
	}
}

ਖਾਲੀ Fmt::parse_verilog(ਅਚਲ std::ਸਦਿਸ਼<VerilogFmtArg> &ਦਲੀਲਾਂ_2, ਤਾਰਕਿਕ sformat_like, ਪੂਰਨ default_ਅਧਾਰ, ਅਚਲ std::string &task_name, ਅਚਲ std::string &module_name)
{
	parts.clear();

	ਸਵੈਚਾਲਿਤ arg = ਦਲੀਲਾਂ_2.begin();
	ਲਈ (; arg != ਦਲੀਲਾਂ_2.ਸਿਰਾ(); ++arg) {
		ਚੋਣ (arg->type) {
			ਹਾਲਤ VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.ਅਧਾਰ = default_ਅਧਾਰ;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_ਜੋੜੋ(part);
				ਰੋਕੋ;
			}

			ਹਾਲਤ VerilogFmtArg::ਸਮਾਂ: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_ਸਮਾਂ;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.ਚੌੜਾਈ = 20;
				parts.ਧੱਕੋ_ਪਿੱਠ(part);
				ਰੋਕੋ;
			}

			ਹਾਲਤ VerilogFmtArg::STRING: {
				ਜੇ (arg == ਦਲੀਲਾਂ_2.begin() || !sformat_like) {
					ਅਚਲ ਸਵੈਚਾਲਿਤ fmtarg = arg;
					ਅਚਲ std::string &fmt = fmtarg->str;
					FmtPart part = {};
					ਲਈ (ਮਾਪ_t i = 0; i < fmt.ਮਾਪ(); i++) {
						ਜੇ (fmt[i] != '%') {
							part.str += fmt[i];
						} ਨਹੀਂਤਾਂ ਜੇ (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} ਨਹੀਂਤਾਂ ਜੇ (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} ਨਹੀਂਤਾਂ ਜੇ (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} ਨਹੀਂਤਾਂ {
							ਜੇ (!part.str.ਖਾਲੀ_ਥਾਵਾਂ()) {
								part.type = FmtPart::LITERAL;
								parts.ਧੱਕੋ_ਪਿੱਠ(part);
								part = {};
							}
							ਜੇ (++i == fmt.ਮਾਪ()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - ਦਲੀਲਾਂ_2.begin() + 1);
							}

							ਜੇ (++arg == ਦਲੀਲਾਂ_2.ਸਿਰਾ()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - ਦਲੀਲਾਂ_2.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							ਲਈ (; i < fmt.ਮਾਪ(); i++) {
								ਜੇ (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::ਖੱਬਾ;
								} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} ਨਹੀਂਤਾਂ ਰੋਕੋ;
							}
							ਜੇ (i == fmt.ਮਾਪ()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - ਦਲੀਲਾਂ_2.begin() + 1);
							}

							ਤਾਰਕਿਕ has_leading_zero = ਝੂਠ, has_ਚੌੜਾਈ = ਝੂਠ;
							ਲਈ (; i < fmt.ਮਾਪ(); i++) {
								ਜੇ (fmt[i] >= '0' && fmt[i] <= '9') {
									ਜੇ (fmt[i] == '0' && !has_ਚੌੜਾਈ) {
										has_leading_zero = ਸੱਚ;
									} ਨਹੀਂਤਾਂ {
										has_ਚੌੜਾਈ = ਸੱਚ;
										part.ਚੌੜਾਈ *= 10;
										part.ਚੌੜਾਈ += fmt[i] - '0';
									}
									ਜਾਰੀ;
								} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.ਅਧਾਰ =  2;
								} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.ਅਧਾਰ =  8;
								} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.ਅਧਾਰ = 10;
								} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.ਅਧਾਰ = 16;
								} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									ਜੇ ((part.sig.ਮਾਪ() % 8) != 0)
										part.sig.extend_u0((part.sig.ਮਾਪ() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} ਨਹੀਂਤਾਂ ਜੇ (fmt[i] == 't' || fmt[i] == 'T') {
									ਜੇ (arg->type == VerilogFmtArg::ਸਮਾਂ) {
										part.type = FmtPart::VLOG_ਸਮਾਂ;
										part.realtime = arg->realtime;
										ਜੇ (!has_ਚੌੜਾਈ && !has_leading_zero)
											part.ਚੌੜਾਈ = 20;
									} ਨਹੀਂਤਾਂ {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - ਦਲੀਲਾਂ_2.begin() + 1);
									}
								} ਨਹੀਂਤਾਂ {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - ਦਲੀਲਾਂ_2.begin() + 1);
								}
								ਰੋਕੋ;
							}
							ਜੇ (i == fmt.ਮਾਪ()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - ਦਲੀਲਾਂ_2.begin() + 1);
							}

							ਜੇ (part.padding == '\0') {
								ਜੇ (has_leading_zero && part.justify == FmtPart::ਸੱਜਾ) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} ਨਹੀਂਤਾਂ {
									part.padding = ' ';
								}
							}

							ਜੇ (part.type == FmtPart::INTEGER && part.ਅਧਾਰ != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - ਦਲੀਲਾਂ_2.begin() + 1);

							ਜੇ (part.ਅਧਾਰ != 10)
								part.signed_ = ਝੂਠ;
							ਜੇ (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_ਜੋੜੋ(part);
							ਨਹੀਂਤਾਂ
								parts.ਧੱਕੋ_ਪਿੱਠ(part);
							part = {};
						}
					}
					ਜੇ (!part.str.ਖਾਲੀ_ਥਾਵਾਂ()) {
						part.type = FmtPart::LITERAL;
						parts.ਧੱਕੋ_ਪਿੱਠ(part);
					}
				} ਨਹੀਂਤਾਂ {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.ਧੱਕੋ_ਪਿੱਠ(part);
				}
				ਰੋਕੋ;
			}

			ਮੂਲ: log_abort();
		}
	}
}

std::ਸਦਿਸ਼<VerilogFmtArg> Fmt::emit_verilog() ਅਚਲ
{
	std::ਸਦਿਸ਼<VerilogFmtArg> ਦਲੀਲਾਂ_2;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	ਲਈ (ਸਵੈਚਾਲਿਤ &part : parts) {
		ਚੋਣ (part.type) {
			ਹਾਲਤ FmtPart::LITERAL:
				ਲਈ (ਅੱਖਰ c : part.str) {
					ਜੇ (c == '%')
						fmt.str += "%%";
					ਨਹੀਂਤਾਂ
						fmt.str += c;
				}
				ਰੋਕੋ;

			ਹਾਲਤ FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(arg);

				fmt.str += '%';
				ਜੇ (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				ਜੇ (part.justify == FmtPart::ਖੱਬਾ)
					fmt.str += '-';
				ਜੇ (part.ਚੌੜਾਈ == 0) {
					fmt.str += '0';
				} ਨਹੀਂਤਾਂ ਜੇ (part.ਚੌੜਾਈ > 0) {
					ਜੇ (part.ਅਧਾਰ != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::ਮੰਜ਼ਿਲ_string(part.ਚੌੜਾਈ);
				}
				ਚੋਣ (part.ਅਧਾਰ) {
					ਹਾਲਤ  2: fmt.str += 'b'; ਰੋਕੋ;
					ਹਾਲਤ  8: fmt.str += 'o'; ਰੋਕੋ;
					ਹਾਲਤ 10: fmt.str += 'd'; ਰੋਕੋ;
					ਹਾਲਤ 16: fmt.str += 'h'; ਰੋਕੋ; // treat uppercase hex as lowercase
					ਮੂਲ: log_abort();
				}
				ਰੋਕੋ;
			}

			ਹਾਲਤ FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(arg);

				fmt.str += '%';
				ਜੇ (part.justify == FmtPart::ਖੱਬਾ)
					fmt.str += '-';
				ਜੇ (part.sig.ਮਾਪ() == 8) {
					ਜੇ (part.ਚੌੜਾਈ > 0) {
						ਜੇ (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::ਮੰਜ਼ਿਲ_string(part.ਚੌੜਾਈ);
					}
					fmt.str += 'c';
				} ਨਹੀਂਤਾਂ {
					log_assert(part.sig.ਮਾਪ() % 8 == 0);
					ਜੇ (part.ਚੌੜਾਈ > 0) {
						fmt.str += std::ਮੰਜ਼ਿਲ_string(part.ਚੌੜਾਈ);
					}
					fmt.str += 's';
				}
				ਰੋਕੋ;
			}

			ਹਾਲਤ FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.ਕੱਢੋ_2(0, 7); // only ASCII
				ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(arg);

				fmt.str += "%c";
				ਰੋਕੋ;
			}

			ਹਾਲਤ FmtPart::VLOG_ਸਮਾਂ: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::ਸਮਾਂ;
				ਜੇ (part.realtime)
					arg.realtime = ਸੱਚ;
				ਦਲੀਲਾਂ_2.ਧੱਕੋ_ਪਿੱਠ(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				ਜੇ (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				ਜੇ (part.justify == FmtPart::ਖੱਬਾ)
					fmt.str += '-';
				ਜੇ (part.padding == '0' && part.ਚੌੜਾਈ > 0)
					fmt.str += '0';
				fmt.str += std::ਮੰਜ਼ਿਲ_string(part.ਚੌੜਾਈ);
				fmt.str += 't';
				ਰੋਕੋ;
			}

			ਮੂਲ: log_abort();
		}
	}

	ਦਲੀਲਾਂ_2.ਪਾਓ(ਦਲੀਲਾਂ_2.begin(), fmt);
	ਵਾਪਸ ਦਲੀਲਾਂ_2;
}

std::string escape_cxx_string(ਅਚਲ std::string &ਨਿਵੇਸ਼)
{
	std::string output = "\"";
	ਲਈ (ਸਵੈਚਾਲਿਤ c : ਨਿਵੇਸ਼) {
		ਜੇ (::isprint(c)) {
			ਜੇ (c == '\\' || c == '"')
				output.ਧੱਕੋ_ਪਿੱਠ('\\');
			output.ਧੱਕੋ_ਪਿੱਠ(c);
		} ਨਹੀਂਤਾਂ {
			ਅੱਖਰ l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.ਧੱਕੋ_ਪਿੱਠ('\\');
			output.ਧੱਕੋ_ਪਿੱਠ('0' + h);
			output.ਧੱਕੋ_ਪਿੱਠ('0' + m);
			output.ਧੱਕੋ_ਪਿੱਠ('0' + l);
		}
	}
	output.ਧੱਕੋ_ਪਿੱਠ('"');
	ਜੇ (output.ਲੱਭੋ('\0') != std::string::npos) {
		output.ਪਾਓ(0, "std::string {");
		output.append(stringf(", %zu}", ਨਿਵੇਸ਼.ਮਾਪ()));
	}
	ਵਾਪਸ output;
}

ਖਾਲੀ Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<ਖਾਲੀ(ਅਚਲ RTLIL::SigSpec &)> emit_sig, ਅਚਲ std::string &context) ਅਚਲ
{
	os << indent << "std::string buf;\n";
	ਲਈ (ਸਵੈਚਾਲਿਤ &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		ਚੋਣ (part.type) {
			ਹਾਲਤ FmtPart::LITERAL:   os << "LITERAL";   ਰੋਕੋ;
			ਹਾਲਤ FmtPart::INTEGER:   os << "INTEGER";   ਰੋਕੋ;
			ਹਾਲਤ FmtPart::STRING:    os << "STRING";    ਰੋਕੋ;
			ਹਾਲਤ FmtPart::UNICHAR:   os << "UNICHAR";   ਰੋਕੋ;
			ਹਾਲਤ FmtPart::VLOG_ਸਮਾਂ: os << "VLOG_TIME"; ਰੋਕੋ;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		ਚੋਣ (part.justify) {
			ਹਾਲਤ FmtPart::ਖੱਬਾ:    os << "LEFT";    ਰੋਕੋ;
			ਹਾਲਤ FmtPart::ਸੱਜਾ:   os << "RIGHT";   ਰੋਕੋ;
			ਹਾਲਤ FmtPart::NUMERIC: os << "NUMERIC"; ਰੋਕੋ;
		}
		os << ", ";
		os << "(char)" << (ਪੂਰਨ)part.padding << ", ";
		os << part.ਚੌੜਾਈ << ", ";
		os << part.ਅਧਾਰ << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		ਚੋਣ (part.sign) {
			ਹਾਲਤ FmtPart::MINUS:       os << "MINUS";       ਰੋਕੋ;
			ਹਾਲਤ FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  ਰੋਕੋ;
			ਹਾਲਤ FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; ਰੋਕੋ;
		}
		os << ", ";
		os << part.hex_ਉੱਤਲਾ << ", ";
		os << part.show_ਅਧਾਰ << ", ";
		os << part.ਸਮੂਹ << ", ";
		os << part.realtime;
		os << " }.render(";
		emit_sig(part.sig);
		os << ", " << context << ");\n";
	}
	os << indent << "return buf;\n";
}

std::string Fmt::render() ਅਚਲ
{
	std::string str;

	ਲਈ (ਸਵੈਚਾਲਿਤ &part : parts) {
		ਚੋਣ (part.type) {
			ਹਾਲਤ FmtPart::LITERAL:
				str += part.str;
				ਰੋਕੋ;

			ਹਾਲਤ FmtPart::UNICHAR: {
				RTLIL::Const ਮੁੱਲ = part.sig.as_const();
				uint32_t codepoint = ਮੁੱਲ.as_int();
				ਜੇ (codepoint >= 0x10000)
					str += (ਅੱਖਰ)(0xf0 |  (codepoint >> 18));
				ਨਹੀਂਤਾਂ ਜੇ (codepoint >= 0x800)
					str += (ਅੱਖਰ)(0xe0 |  (codepoint >> 12));
				ਨਹੀਂਤਾਂ ਜੇ (codepoint >= 0x80)
					str += (ਅੱਖਰ)(0xc0 |  (codepoint >>  6));
				ਨਹੀਂਤਾਂ
					str += (ਅੱਖਰ)codepoint;
				ਜੇ (codepoint >= 0x10000)
					str += (ਅੱਖਰ)(0x80 | ((codepoint >> 12) & 0x3f));
				ਜੇ (codepoint >= 0x800)
					str += (ਅੱਖਰ)(0x80 | ((codepoint >>  6) & 0x3f));
				ਜੇ (codepoint >= 0x80)
					str += (ਅੱਖਰ)(0x80 | ((codepoint >>  0) & 0x3f));
				ਰੋਕੋ;
			}

			ਹਾਲਤ FmtPart::INTEGER:
			ਹਾਲਤ FmtPart::STRING:
			ਹਾਲਤ FmtPart::VLOG_ਸਮਾਂ: {
				std::string buf;
				std::string ਅਗੇਤਰ_2;
				ਜੇ (part.type == FmtPart::INTEGER) {
					RTLIL::Const ਮੁੱਲ = part.sig.as_const();
					ਤਾਰਕਿਕ has_x = ਝੂਠ, all_x = ਸੱਚ, has_z = ਝੂਠ, all_z = ਸੱਚ;
					ਲਈ (State ਦੋਆਧਾਰੀ_ਅੰਕ : ਮੁੱਲ) {
						ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ == State::Sx)
							has_x = ਸੱਚ;
						ਨਹੀਂਤਾਂ
							all_x = ਝੂਠ;
						ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ == State::Sz)
							has_z = ਸੱਚ;
						ਨਹੀਂਤਾਂ
							all_z = ਝੂਠ;
					}

					ਜੇ (!has_z && !has_x && part.signed_ && ਮੁੱਲ[ਮੁੱਲ.ਮਾਪ() - 1]) {
						ਅਗੇਤਰ_2 = "-";
						ਮੁੱਲ = RTLIL::const_neg(ਮੁੱਲ, {}, part.signed_, {}, ਮੁੱਲ.ਮਾਪ() + 1);
					} ਨਹੀਂਤਾਂ {
						ਚੋਣ (part.sign) {
							ਹਾਲਤ FmtPart::MINUS:       ਰੋਕੋ;
							ਹਾਲਤ FmtPart::PLUS_MINUS:  ਅਗੇਤਰ_2 = "+"; ਰੋਕੋ;
							ਹਾਲਤ FmtPart::SPACE_MINUS: ਅਗੇਤਰ_2 = " "; ਰੋਕੋ;
						}
					}

					ਜੇ (part.ਅਧਾਰ != 10) {
						ਮਾਪ_t minimum_ਮਾਪ = 1;
						ਲਈ (ਮਾਪ_t ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < (ਮਾਪ_t)ਮੁੱਲ.ਮਾਪ(); ਸੂਚਕਾਂਕ++)
							ਜੇ (ਮੁੱਲ[ਸੂਚਕਾਂਕ] != State::S0)
								minimum_ਮਾਪ = ਸੂਚਕਾਂਕ + 1;
						ਮੁੱਲ = ਮੁੱਲ.ਕੱਢੋ_2(0, minimum_ਮਾਪ);
					}

					ਜੇ (part.ਅਧਾਰ == 2) {
						ਜੇ (part.show_ਅਧਾਰ)
							ਅਗੇਤਰ_2 += "0b";
						ਲਈ (ਮਾਪ_t ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < (ਮਾਪ_t)ਮੁੱਲ.ਮਾਪ(); ਸੂਚਕਾਂਕ++) {
							ਜੇ (part.ਸਮੂਹ && ਸੂਚਕਾਂਕ > 0 && ਸੂਚਕਾਂਕ % 4 == 0)
								buf += '_';
							RTLIL::State ਦੋਆਧਾਰੀ_ਅੰਕ = ਮੁੱਲ[ਸੂਚਕਾਂਕ];
							ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ == State::Sx)
								buf += 'x';
							ਨਹੀਂਤਾਂ ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ == State::Sz)
								buf += 'z';
							ਨਹੀਂਤਾਂ ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ == State::S1)
								buf += '1';
							ਨਹੀਂਤਾਂ /* if (bit == State::S0) */
								buf += '0';
						}
					} ਨਹੀਂਤਾਂ ਜੇ (part.ਅਧਾਰ == 8 || part.ਅਧਾਰ == 16) {
						ਜੇ (part.show_ਅਧਾਰ)
							ਅਗੇਤਰ_2 += (part.ਅਧਾਰ == 16) ? (part.hex_ਉੱਤਲਾ ? "0X" : "0x") : "0o";
						ਮਾਪ_t ਕਦਮ = (part.ਅਧਾਰ == 16) ? 4 : 3;
						ਲਈ (ਮਾਪ_t ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < (ਮਾਪ_t)ਮੁੱਲ.ਮਾਪ(); ਸੂਚਕਾਂਕ += ਕਦਮ) {
							ਜੇ (part.ਸਮੂਹ && ਸੂਚਕਾਂਕ > 0 && ਸੂਚਕਾਂਕ % (4 * ਕਦਮ) == 0)
								buf += '_';
							RTLIL::Const subvalue = ਮੁੱਲ.ਕੱਢੋ_2(ਸੂਚਕਾਂਕ, min(ਕਦਮ, ਮੁੱਲ.ਮਾਪ() - ਸੂਚਕਾਂਕ));
							ਤਾਰਕਿਕ has_x = ਝੂਠ, all_x = ਸੱਚ, has_z = ਝੂਠ, all_z = ਸੱਚ;
							ਲਈ (State ਦੋਆਧਾਰੀ_ਅੰਕ : subvalue) {
								ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ == State::Sx)
									has_x = ਸੱਚ;
								ਨਹੀਂਤਾਂ
									all_x = ਝੂਠ;
								ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ == State::Sz)
									has_z = ਸੱਚ;
								ਨਹੀਂਤਾਂ
									all_z = ਝੂਠ;
							}
							ਜੇ (all_x)
								buf += 'x';
							ਨਹੀਂਤਾਂ ਜੇ (all_z)
								buf += 'z';
							ਨਹੀਂਤਾਂ ਜੇ (has_x)
								buf += 'X';
							ਨਹੀਂਤਾਂ ਜੇ (has_z)
								buf += 'Z';
							ਨਹੀਂਤਾਂ {
								ਅਚਲ ਅੱਖਰ *digits = part.hex_ਉੱਤਲਾ ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} ਨਹੀਂਤਾਂ ਜੇ (part.ਅਧਾਰ == 10) {
						ਜੇ (part.show_ਅਧਾਰ)
							ਅਗੇਤਰ_2 += "0d";
						ਜੇ (all_x)
							buf += 'x';
						ਨਹੀਂਤਾਂ ਜੇ (all_z)
							buf += 'z';
						ਨਹੀਂਤਾਂ ਜੇ (has_x)
							buf += 'X';
						ਨਹੀਂਤਾਂ ਜੇ (has_z)
							buf += 'Z';
						ਨਹੀਂਤਾਂ {
							log_assert(ਮੁੱਲ.is_fully_def());
							ਜੇ (ਮੁੱਲ.is_fully_zero())
								buf += '0';
							ਮਾਪ_t ਸੂਚਕਾਂਕ = 0;
							ਜਦੋਂ (!ਮੁੱਲ.is_fully_zero())	{
								ਜੇ (part.ਸਮੂਹ && ਸੂਚਕਾਂਕ > 0 && ਸੂਚਕਾਂਕ % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(ਮੁੱਲ, 10, ਝੂਠ, ਝੂਠ, 4).as_int();
								ਮੁੱਲ = RTLIL::const_div(ਮੁੱਲ, 10, ਝੂਠ, ਝੂਠ, ਮੁੱਲ.ਮਾਪ());
								ਸੂਚਕਾਂਕ++;
							}
						}
					} ਨਹੀਂਤਾਂ log_abort();
					ਜੇ (part.justify == FmtPart::NUMERIC && part.ਸਮੂਹ && part.padding == '0') {
						ਮਾਪ_t ਸਮੂਹ_ਮਾਪ = part.ਅਧਾਰ == 10 ? 3 : 4;
						ਜਦੋਂ (ਅਗੇਤਰ_2.ਮਾਪ() + buf.ਮਾਪ() < part.ਚੌੜਾਈ) {
							ਜੇ (buf.ਮਾਪ() % (ਸਮੂਹ_ਮਾਪ + 1) == ਸਮੂਹ_ਮਾਪ)
								buf += '_';
							buf += '0';
						}
					}
					std::ਉਲਟ(buf.begin(), buf.ਸਿਰਾ());
				} ਨਹੀਂਤਾਂ ਜੇ (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} ਨਹੀਂਤਾਂ ਜੇ (part.type == FmtPart::VLOG_ਸਮਾਂ) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.ਚੌੜਾਈ == 0 || part.padding != '\0');
				ਜੇ (ਅਗੇਤਰ_2.ਮਾਪ() + buf.ਮਾਪ() < part.ਚੌੜਾਈ) {
					ਮਾਪ_t pad_ਚੌੜਾਈ = part.ਚੌੜਾਈ - ਅਗੇਤਰ_2.ਮਾਪ() - buf.ਮਾਪ();
					ਚੋਣ (part.justify) {
						ਹਾਲਤ FmtPart::ਖੱਬਾ:
							str += ਅਗੇਤਰ_2;
							str += buf;
							str += std::string(pad_ਚੌੜਾਈ, part.padding);
							ਰੋਕੋ;
						ਹਾਲਤ FmtPart::ਸੱਜਾ:
							str += std::string(pad_ਚੌੜਾਈ, part.padding);
							str += ਅਗੇਤਰ_2;
							str += buf;
							ਰੋਕੋ;
						ਹਾਲਤ FmtPart::NUMERIC:
							str += ਅਗੇਤਰ_2;
							str += std::string(pad_ਚੌੜਾਈ, part.padding);
							str += buf;
							ਰੋਕੋ;
					}
				} ਨਹੀਂਤਾਂ {
					str += ਅਗੇਤਰ_2;
					str += buf;
				}
				ਰੋਕੋ;
			}
		}
	}

	ਵਾਪਸ str;
}
