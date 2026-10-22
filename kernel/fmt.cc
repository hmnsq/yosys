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

#ಅನ್ತರ್ಭಾವ "libs/bigint/BigUnsigned.hh"
#ಅನ್ತರ್ಭಾವ "kernel/fmt.h"

USING_YOSYS_NAMESPACE

ಶೂನ್ಯ Fmt::append_literal(ಸ್ಥಿರಾಂಕ std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
}

ಶೂನ್ಯ Fmt::parse_rtlil(ಸ್ಥಿರಾಂಕ RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec ವಾದಗಳು_2 = cell->get_ಪೋರ್ಟ್(ID(ವಾದಗಳು));
	parts.clear();

	FmtPart part;
	ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < fmt.ಗಾತ್ರ_2(); i++) {
		ಆದರೆ (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == '}') {
			log_assert(ಸುಳ್ಳು && "Unexpected '}' in format string");
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == '{') {
			ಆದರೆ (!part.str.ಖಾಲಿಗಳು()) {
				part.type = FmtPart::LITERAL;
				parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
				part = {};
			}

			ಆದರೆ (++i == fmt.ಗಾತ್ರ_2())
				log_assert(ಸುಳ್ಳು && "Unexpected end in format substitution");

			ಗಾತ್ರ_t arg_ಗಾತ್ರ = 0;
			ಗಾಗಿ (; i < fmt.ಗಾತ್ರ_2(); i++) {
				ಆದರೆ (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_ಗಾತ್ರ *= 10;
					arg_ಗಾತ್ರ += fmt[i] - '0';
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == ':') {
					++i;
					ನಿಲ್ಲಿಸು;
				} ಇಲ್ಲದಿದ್ದರೆ {
					log_assert(ಸುಳ್ಳು && "Unexpected character in format substitution");
				}
			}
			ಆದರೆ (i == fmt.ಗಾತ್ರ_2())
				log_assert(ಸುಳ್ಳು && "Unexpected end in format substitution");

			ಆದರೆ ((ಗಾತ್ರ_t)ವಾದಗಳು_2.ಗಾತ್ರ_2() < arg_ಗಾತ್ರ)
				log_assert(ಸುಳ್ಳು && "Format part overruns arguments");
			part.sig = ವಾದಗಳು_2.ಹೊರತೆಗೆ(0, arg_ಗಾತ್ರ);
			ವಾದಗಳು_2.remove(0, arg_ಗಾತ್ರ);

			ಆದರೆ (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				ಹೋಗು success;
			}

			ಆದರೆ (fmt[i] == '>')
				part.justify = FmtPart::ಬಲ_2;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == '<')
				part.justify = FmtPart::ಎಡ_2;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			ಇಲ್ಲದಿದ್ದರೆ
				log_assert(ಸುಳ್ಳು && "Unexpected justification in format substitution");
			ಆದರೆ (++i == fmt.ಗಾತ್ರ_2())
				log_assert(ಸುಳ್ಳು && "Unexpected end in format substitution");

			part.padding = fmt[i];
			ಆದರೆ (++i == fmt.ಗಾತ್ರ_2())
				log_assert(ಸುಳ್ಳು && "Unexpected end in format substitution");

			ಗಾಗಿ (; i < fmt.ಗಾತ್ರ_2(); i++) {
				ಆದರೆ (fmt[i] >= '0' && fmt[i] <= '9') {
					part.ಅಗಲ *= 10;
					part.ಅಗಲ += fmt[i] - '0';
					ಮುಂದುವರಿ;
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.ತಳ = 2;
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.ತಳ = 8;
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.ತಳ = 10;
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.ತಳ = 16;
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.ತಳ = 16;
					part.hex_ಊರ್ಧ್ವ = ನಿಜ;
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 't') {
					part.type = FmtPart::VLOG_ಸಮಯ;
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_ಸಮಯ;
					part.realtime = ನಿಜ;
				} ಇಲ್ಲದಿದ್ದರೆ {
					log_assert(ಸುಳ್ಳು && "Unexpected character in format substitution");
				}
				++i;
				ನಿಲ್ಲಿಸು;
			}
			ಆದರೆ (i == fmt.ಗಾತ್ರ_2())
				log_assert(ಸುಳ್ಳು && "Unexpected end in format substitution");

			ಆದರೆ (part.type == FmtPart::INTEGER) {
				ಆದರೆ (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					ಆದರೆ (++i == fmt.ಗಾತ್ರ_2())
						log_assert(ಸುಳ್ಳು && "Unexpected end in format substitution");
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					ಆದರೆ (++i == fmt.ಗಾತ್ರ_2())
						log_assert(ಸುಳ್ಳು && "Unexpected end in format substitution");
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					ಆದರೆ (++i == fmt.ಗಾತ್ರ_2())
						log_assert(ಸುಳ್ಳು && "Unexpected end in format substitution");
				} ಇಲ್ಲದಿದ್ದರೆ {
					// also accept no sign character and treat like MINUS for compatibility
				}

				ಆದರೆ (fmt[i] == '#') {
					part.show_ತಳ = ನಿಜ;
					++i;
				}
				ಆದರೆ (fmt[i] == '_') {
					part.ಗುಂಪು = ನಿಜ;
					++i;
				}

				ಆದರೆ (fmt[i] == 'u')
					part.signed_ = ಸುಳ್ಳು;
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 's')
					part.signed_ = ನಿಜ;
				ಇಲ್ಲದಿದ್ದರೆ
					log_assert(ಸುಳ್ಳು && "Unexpected character in format substitution");
				ಆದರೆ (++i == fmt.ಗಾತ್ರ_2())
					log_assert(ಸುಳ್ಳು && "Unexpected end in format substitution");
			}

		success:
			ಆದರೆ (fmt[i] != '}')
				log_assert(ಸುಳ್ಳು && "Expected '}' after format substitution");

			parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
			part = {};
		} ಇಲ್ಲದಿದ್ದರೆ {
			part.str += fmt[i];
		}
	}
	ಆದರೆ (!part.str.ಖಾಲಿಗಳು()) {
		part.type = FmtPart::LITERAL;
		parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
	}
}

ಶೂನ್ಯ Fmt::emit_rtlil(RTLIL::Cell *cell) ಸ್ಥಿರಾಂಕ {
	std::string fmt;
	RTLIL::SigSpec ವಾದಗಳು_2;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &part : parts) {
		ಆಯ್ಕೆ (part.type) {
			ಸ್ಥಿತಿ FmtPart::LITERAL:
				ಗಾಗಿ (ಅಕ್ಷರ c : part.str) {
					ಆದರೆ (c == '{')
						fmt += "{{";
					ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '}')
						fmt += "}}";
					ಇಲ್ಲದಿದ್ದರೆ
						fmt += c;
				}
				ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ FmtPart::UNICHAR:
				log_assert(part.sig.ಗಾತ್ರ_2() <= 32);
				fmt += "{U}";
				ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ FmtPart::VLOG_ಸಮಯ:
				log_assert(part.sig.ಗಾತ್ರ_2() == 0);
				YS_FALLTHROUGH
			ಸ್ಥಿತಿ FmtPart::STRING:
				log_assert(part.sig.ಗಾತ್ರ_2() % 8 == 0);
				YS_FALLTHROUGH
			ಸ್ಥಿತಿ FmtPart::INTEGER:
				ವಾದಗಳು_2.append(part.sig);
				fmt += '{';
				fmt += std::ಗಮ್ಯ_string(part.sig.ಗಾತ್ರ_2());
				fmt += ':';
				ಆದರೆ (part.justify == FmtPart::ಬಲ_2)
					fmt += '>';
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (part.justify == FmtPart::ಎಡ_2)
					fmt += '<';
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				ಇಲ್ಲದಿದ್ದರೆ log_abort();
				log_assert(part.ಅಗಲ == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				ಆದರೆ (part.ಅಗಲ > 0)
					fmt += std::ಗಮ್ಯ_string(part.ಅಗಲ);
				ಆದರೆ (part.type == FmtPart::INTEGER) {
					ಆಯ್ಕೆ (part.ತಳ) {
						ಸ್ಥಿತಿ  2: fmt += 'b'; ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ  8: fmt += 'o'; ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 10: fmt += 'd'; ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ 16: fmt += part.hex_ಊರ್ಧ್ವ ? 'H' : 'h'; ನಿಲ್ಲಿಸು;
						ಪೂರ್ವನಿಯೋಜಿತ: log_abort();
					}
					ಆಯ್ಕೆ (part.sign) {
						ಸ್ಥಿತಿ FmtPart::MINUS:       fmt += '-'; ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ FmtPart::PLUS_MINUS:  fmt += '+'; ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ FmtPart::SPACE_MINUS: fmt += ' '; ನಿಲ್ಲಿಸು;
					}
					fmt += part.show_ತಳ ? "#" : "";
					fmt += part.ಗುಂಪು ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (part.type == FmtPart::STRING) {
					fmt += 'c';
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (part.type == FmtPart::VLOG_ಸಮಯ) {
					ಆದರೆ (part.realtime)
						fmt += 'r';
					ಇಲ್ಲದಿದ್ದರೆ
						fmt += 't';
				} ಇಲ್ಲದಿದ್ದರೆ log_abort();
				fmt += '}';
				ನಿಲ್ಲಿಸು;

			ಪೂರ್ವನಿಯೋಜಿತ: log_abort();
		}
	}

	cell->ಗಣ_Param(ID(FORMAT), fmt);
	cell->ಗಣ_Param(ID(ವಾದಗಳು_ಅಗಲ), ವಾದಗಳು_2.ಗಾತ್ರ_2());
	cell->ಗಣ_ಪೋರ್ಟ್(ID(ವಾದಗಳು), ವಾದಗಳು_2);
}

ಸ್ಥಿರ ಗಾತ್ರ_t compute_required_decimal_places(ಗಾತ್ರ_t ಗಾತ್ರ_2, ತಾರ್ಕಿಕ signed_)
{
	BigUnsigned max;
	ಆದರೆ (!signed_)
		max.ಗಣ_ಬಿಟ್(ಗಾತ್ರ_2, ನಿಜ);
	ಇಲ್ಲದಿದ್ದರೆ
		max.ಗಣ_ಬಿಟ್(ಗಾತ್ರ_2 - 1, ನಿಜ);
	ಗಾತ್ರ_t places = 0;
	ಆಗ (!max.isZero()) {
		places++;
		max /= 10;
	}
	ಆದರೆ (signed_)
		places++;
	ಹಿಂತಿರುಗಿ places;
}

ಸ್ಥಿರ ಗಾತ್ರ_t compute_required_nondecimal_places(ಗಾತ್ರ_t ಗಾತ್ರ_2, ಚಿಹ್ನೆರಹಿತ ತಳ)
{
	log_assert(ತಳ != 10);
	BigUnsigned max;
	max.ಗಣ_ಬಿಟ್(ಗಾತ್ರ_2 - 1, ನಿಜ);
	ಗಾತ್ರ_t places = 0;
	ಆಗ (!max.isZero()) {
		places++;
		max /= ತಳ;
	}
	ಹಿಂತಿರುಗಿ places;
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
ಶೂನ್ಯ Fmt::apply_verilog_automatic_sizing_and_ಕೂಡಿಸು(FmtPart &part)
{
	ಆದರೆ (part.ತಳ == 10) {
		ಗಾತ್ರ_t places = compute_required_decimal_places(part.sig.ಗಾತ್ರ_2(), part.signed_);
		part.padding = ' ';
		part.ಅಗಲ = std::max(part.ಅಗಲ, places);
		parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
		ಹಿಂತಿರುಗಿ;
	}

	part.padding = '0';

	ಗಾತ್ರ_t places = compute_required_nondecimal_places(part.sig.ಗಾತ್ರ_2(), part.ತಳ);
	ಆದರೆ (part.ಅಗಲ < places) {
		part.justify = FmtPart::ಬಲ_2;
		part.ಅಗಲ = places;
		parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (part.ಅಗಲ == places) {
		parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (part.ಅಗಲ > places) {
		ಸ್ವಯಂಚಾಲಿತ gap = std::string(part.ಅಗಲ - places, ' ');
		part.ಅಗಲ = places;

		ಆದರೆ (part.justify == FmtPart::ಬಲ_2) {
			append_literal(gap);
			parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
		} ಇಲ್ಲದಿದ್ದರೆ {
			part.justify = FmtPart::ಬಲ_2;
			parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
			append_literal(gap);
		}
	}
}

ಶೂನ್ಯ Fmt::parse_verilog(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<VerilogFmtArg> &ವಾದಗಳು_2, ತಾರ್ಕಿಕ sformat_like, ಪೂರ್ಣಾಂಕ default_ತಳ, ಸ್ಥಿರಾಂಕ std::string &task_name, ಸ್ಥಿರಾಂಕ std::string &module_name)
{
	parts.clear();

	ಸ್ವಯಂಚಾಲಿತ arg = ವಾದಗಳು_2.begin();
	ಗಾಗಿ (; arg != ವಾದಗಳು_2.ಕೊನೆ(); ++arg) {
		ಆಯ್ಕೆ (arg->type) {
			ಸ್ಥಿತಿ VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.ತಳ = default_ತಳ;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_ಕೂಡಿಸು(part);
				ನಿಲ್ಲಿಸು;
			}

			ಸ್ಥಿತಿ VerilogFmtArg::ಸಮಯ: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_ಸಮಯ;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.ಅಗಲ = 20;
				parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
				ನಿಲ್ಲಿಸು;
			}

			ಸ್ಥಿತಿ VerilogFmtArg::STRING: {
				ಆದರೆ (arg == ವಾದಗಳು_2.begin() || !sformat_like) {
					ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ fmtarg = arg;
					ಸ್ಥಿರಾಂಕ std::string &fmt = fmtarg->str;
					FmtPart part = {};
					ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < fmt.ಗಾತ್ರ_2(); i++) {
						ಆದರೆ (fmt[i] != '%') {
							part.str += fmt[i];
						} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} ಇಲ್ಲದಿದ್ದರೆ {
							ಆದರೆ (!part.str.ಖಾಲಿಗಳು()) {
								part.type = FmtPart::LITERAL;
								parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
								part = {};
							}
							ಆದರೆ (++i == fmt.ಗಾತ್ರ_2()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - ವಾದಗಳು_2.begin() + 1);
							}

							ಆದರೆ (++arg == ವಾದಗಳು_2.ಕೊನೆ()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - ವಾದಗಳು_2.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							ಗಾಗಿ (; i < fmt.ಗಾತ್ರ_2(); i++) {
								ಆದರೆ (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::ಎಡ_2;
								} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} ಇಲ್ಲದಿದ್ದರೆ ನಿಲ್ಲಿಸು;
							}
							ಆದರೆ (i == fmt.ಗಾತ್ರ_2()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - ವಾದಗಳು_2.begin() + 1);
							}

							ತಾರ್ಕಿಕ has_leading_zero = ಸುಳ್ಳು, has_ಅಗಲ = ಸುಳ್ಳು;
							ಗಾಗಿ (; i < fmt.ಗಾತ್ರ_2(); i++) {
								ಆದರೆ (fmt[i] >= '0' && fmt[i] <= '9') {
									ಆದರೆ (fmt[i] == '0' && !has_ಅಗಲ) {
										has_leading_zero = ನಿಜ;
									} ಇಲ್ಲದಿದ್ದರೆ {
										has_ಅಗಲ = ನಿಜ;
										part.ಅಗಲ *= 10;
										part.ಅಗಲ += fmt[i] - '0';
									}
									ಮುಂದುವರಿ;
								} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.ತಳ =  2;
								} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.ತಳ =  8;
								} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.ತಳ = 10;
								} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.ತಳ = 16;
								} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									ಆದರೆ ((part.sig.ಗಾತ್ರ_2() % 8) != 0)
										part.sig.extend_u0((part.sig.ಗಾತ್ರ_2() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fmt[i] == 't' || fmt[i] == 'T') {
									ಆದರೆ (arg->type == VerilogFmtArg::ಸಮಯ) {
										part.type = FmtPart::VLOG_ಸಮಯ;
										part.realtime = arg->realtime;
										ಆದರೆ (!has_ಅಗಲ && !has_leading_zero)
											part.ಅಗಲ = 20;
									} ಇಲ್ಲದಿದ್ದರೆ {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - ವಾದಗಳು_2.begin() + 1);
									}
								} ಇಲ್ಲದಿದ್ದರೆ {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - ವಾದಗಳು_2.begin() + 1);
								}
								ನಿಲ್ಲಿಸು;
							}
							ಆದರೆ (i == fmt.ಗಾತ್ರ_2()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - ವಾದಗಳು_2.begin() + 1);
							}

							ಆದರೆ (part.padding == '\0') {
								ಆದರೆ (has_leading_zero && part.justify == FmtPart::ಬಲ_2) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} ಇಲ್ಲದಿದ್ದರೆ {
									part.padding = ' ';
								}
							}

							ಆದರೆ (part.type == FmtPart::INTEGER && part.ತಳ != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - ವಾದಗಳು_2.begin() + 1);

							ಆದರೆ (part.ತಳ != 10)
								part.signed_ = ಸುಳ್ಳು;
							ಆದರೆ (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_ಕೂಡಿಸು(part);
							ಇಲ್ಲದಿದ್ದರೆ
								parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
							part = {};
						}
					}
					ಆದರೆ (!part.str.ಖಾಲಿಗಳು()) {
						part.type = FmtPart::LITERAL;
						parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
					}
				} ಇಲ್ಲದಿದ್ದರೆ {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.ತಳ್ಳು_ಹಿಂಭಾಗ(part);
				}
				ನಿಲ್ಲಿಸು;
			}

			ಪೂರ್ವನಿಯೋಜಿತ: log_abort();
		}
	}
}

std::ಸದಿಶ<VerilogFmtArg> Fmt::emit_verilog() ಸ್ಥಿರಾಂಕ
{
	std::ಸದಿಶ<VerilogFmtArg> ವಾದಗಳು_2;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &part : parts) {
		ಆಯ್ಕೆ (part.type) {
			ಸ್ಥಿತಿ FmtPart::LITERAL:
				ಗಾಗಿ (ಅಕ್ಷರ c : part.str) {
					ಆದರೆ (c == '%')
						fmt.str += "%%";
					ಇಲ್ಲದಿದ್ದರೆ
						fmt.str += c;
				}
				ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(arg);

				fmt.str += '%';
				ಆದರೆ (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				ಆದರೆ (part.justify == FmtPart::ಎಡ_2)
					fmt.str += '-';
				ಆದರೆ (part.ಅಗಲ == 0) {
					fmt.str += '0';
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (part.ಅಗಲ > 0) {
					ಆದರೆ (part.ತಳ != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::ಗಮ್ಯ_string(part.ಅಗಲ);
				}
				ಆಯ್ಕೆ (part.ತಳ) {
					ಸ್ಥಿತಿ  2: fmt.str += 'b'; ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ  8: fmt.str += 'o'; ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ 10: fmt.str += 'd'; ನಿಲ್ಲಿಸು;
					ಸ್ಥಿತಿ 16: fmt.str += 'h'; ನಿಲ್ಲಿಸು; // treat uppercase hex as lowercase
					ಪೂರ್ವನಿಯೋಜಿತ: log_abort();
				}
				ನಿಲ್ಲಿಸು;
			}

			ಸ್ಥಿತಿ FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(arg);

				fmt.str += '%';
				ಆದರೆ (part.justify == FmtPart::ಎಡ_2)
					fmt.str += '-';
				ಆದರೆ (part.sig.ಗಾತ್ರ_2() == 8) {
					ಆದರೆ (part.ಅಗಲ > 0) {
						ಆದರೆ (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::ಗಮ್ಯ_string(part.ಅಗಲ);
					}
					fmt.str += 'c';
				} ಇಲ್ಲದಿದ್ದರೆ {
					log_assert(part.sig.ಗಾತ್ರ_2() % 8 == 0);
					ಆದರೆ (part.ಅಗಲ > 0) {
						fmt.str += std::ಗಮ್ಯ_string(part.ಅಗಲ);
					}
					fmt.str += 's';
				}
				ನಿಲ್ಲಿಸು;
			}

			ಸ್ಥಿತಿ FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.ಹೊರತೆಗೆ(0, 7); // only ASCII
				ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(arg);

				fmt.str += "%c";
				ನಿಲ್ಲಿಸು;
			}

			ಸ್ಥಿತಿ FmtPart::VLOG_ಸಮಯ: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::ಸಮಯ;
				ಆದರೆ (part.realtime)
					arg.realtime = ನಿಜ;
				ವಾದಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				ಆದರೆ (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				ಆದರೆ (part.justify == FmtPart::ಎಡ_2)
					fmt.str += '-';
				ಆದರೆ (part.padding == '0' && part.ಅಗಲ > 0)
					fmt.str += '0';
				fmt.str += std::ಗಮ್ಯ_string(part.ಅಗಲ);
				fmt.str += 't';
				ನಿಲ್ಲಿಸು;
			}

			ಪೂರ್ವನಿಯೋಜಿತ: log_abort();
		}
	}

	ವಾದಗಳು_2.ಸೇರಿಸು(ವಾದಗಳು_2.begin(), fmt);
	ಹಿಂತಿರುಗಿ ವಾದಗಳು_2;
}

std::string escape_cxx_string(ಸ್ಥಿರಾಂಕ std::string &ಒಳಹರಿವು)
{
	std::string output = "\"";
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c : ಒಳಹರಿವು) {
		ಆದರೆ (::isprint(c)) {
			ಆದರೆ (c == '\\' || c == '"')
				output.ತಳ್ಳು_ಹಿಂಭಾಗ('\\');
			output.ತಳ್ಳು_ಹಿಂಭಾಗ(c);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಅಕ್ಷರ l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.ತಳ್ಳು_ಹಿಂಭಾಗ('\\');
			output.ತಳ್ಳು_ಹಿಂಭಾಗ('0' + h);
			output.ತಳ್ಳು_ಹಿಂಭಾಗ('0' + m);
			output.ತಳ್ಳು_ಹಿಂಭಾಗ('0' + l);
		}
	}
	output.ತಳ್ಳು_ಹಿಂಭಾಗ('"');
	ಆದರೆ (output.ಕಂಡುಹಿಡಿ('\0') != std::string::npos) {
		output.ಸೇರಿಸು(0, "std::string {");
		output.append(stringf(", %zu}", ಒಳಹರಿವು.ಗಾತ್ರ_2()));
	}
	ಹಿಂತಿರುಗಿ output;
}

ಶೂನ್ಯ Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<ಶೂನ್ಯ(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &)> emit_sig, ಸ್ಥಿರಾಂಕ std::string &context) ಸ್ಥಿರಾಂಕ
{
	os << indent << "std::string buf;\n";
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		ಆಯ್ಕೆ (part.type) {
			ಸ್ಥಿತಿ FmtPart::LITERAL:   os << "LITERAL";   ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ FmtPart::INTEGER:   os << "INTEGER";   ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ FmtPart::STRING:    os << "STRING";    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ FmtPart::UNICHAR:   os << "UNICHAR";   ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ FmtPart::VLOG_ಸಮಯ: os << "VLOG_TIME"; ನಿಲ್ಲಿಸು;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		ಆಯ್ಕೆ (part.justify) {
			ಸ್ಥಿತಿ FmtPart::ಎಡ_2:    os << "LEFT";    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ FmtPart::ಬಲ_2:   os << "RIGHT";   ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ FmtPart::NUMERIC: os << "NUMERIC"; ನಿಲ್ಲಿಸು;
		}
		os << ", ";
		os << "(char)" << (ಪೂರ್ಣಾಂಕ)part.padding << ", ";
		os << part.ಅಗಲ << ", ";
		os << part.ತಳ << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		ಆಯ್ಕೆ (part.sign) {
			ಸ್ಥಿತಿ FmtPart::MINUS:       os << "MINUS";       ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; ನಿಲ್ಲಿಸು;
		}
		os << ", ";
		os << part.hex_ಊರ್ಧ್ವ << ", ";
		os << part.show_ತಳ << ", ";
		os << part.ಗುಂಪು << ", ";
		os << part.realtime;
		os << " }.render(";
		emit_sig(part.sig);
		os << ", " << context << ");\n";
	}
	os << indent << "return buf;\n";
}

std::string Fmt::render() ಸ್ಥಿರಾಂಕ
{
	std::string str;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &part : parts) {
		ಆಯ್ಕೆ (part.type) {
			ಸ್ಥಿತಿ FmtPart::LITERAL:
				str += part.str;
				ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ FmtPart::UNICHAR: {
				RTLIL::Const ಮೌಲ್ಯ = part.sig.as_const();
				uint32_t codepoint = ಮೌಲ್ಯ.as_int();
				ಆದರೆ (codepoint >= 0x10000)
					str += (ಅಕ್ಷರ)(0xf0 |  (codepoint >> 18));
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (codepoint >= 0x800)
					str += (ಅಕ್ಷರ)(0xe0 |  (codepoint >> 12));
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (codepoint >= 0x80)
					str += (ಅಕ್ಷರ)(0xc0 |  (codepoint >>  6));
				ಇಲ್ಲದಿದ್ದರೆ
					str += (ಅಕ್ಷರ)codepoint;
				ಆದರೆ (codepoint >= 0x10000)
					str += (ಅಕ್ಷರ)(0x80 | ((codepoint >> 12) & 0x3f));
				ಆದರೆ (codepoint >= 0x800)
					str += (ಅಕ್ಷರ)(0x80 | ((codepoint >>  6) & 0x3f));
				ಆದರೆ (codepoint >= 0x80)
					str += (ಅಕ್ಷರ)(0x80 | ((codepoint >>  0) & 0x3f));
				ನಿಲ್ಲಿಸು;
			}

			ಸ್ಥಿತಿ FmtPart::INTEGER:
			ಸ್ಥಿತಿ FmtPart::STRING:
			ಸ್ಥಿತಿ FmtPart::VLOG_ಸಮಯ: {
				std::string buf;
				std::string ಪೂರ್ವಪ್ರತ್ಯಯ_2;
				ಆದರೆ (part.type == FmtPart::INTEGER) {
					RTLIL::Const ಮೌಲ್ಯ = part.sig.as_const();
					ತಾರ್ಕಿಕ has_x = ಸುಳ್ಳು, all_x = ನಿಜ, has_z = ಸುಳ್ಳು, all_z = ನಿಜ;
					ಗಾಗಿ (State ಬಿಟ್ : ಮೌಲ್ಯ) {
						ಆದರೆ (ಬಿಟ್ == State::Sx)
							has_x = ನಿಜ;
						ಇಲ್ಲದಿದ್ದರೆ
							all_x = ಸುಳ್ಳು;
						ಆದರೆ (ಬಿಟ್ == State::Sz)
							has_z = ನಿಜ;
						ಇಲ್ಲದಿದ್ದರೆ
							all_z = ಸುಳ್ಳು;
					}

					ಆದರೆ (!has_z && !has_x && part.signed_ && ಮೌಲ್ಯ[ಮೌಲ್ಯ.ಗಾತ್ರ_2() - 1]) {
						ಪೂರ್ವಪ್ರತ್ಯಯ_2 = "-";
						ಮೌಲ್ಯ = RTLIL::const_neg(ಮೌಲ್ಯ, {}, part.signed_, {}, ಮೌಲ್ಯ.ಗಾತ್ರ_2() + 1);
					} ಇಲ್ಲದಿದ್ದರೆ {
						ಆಯ್ಕೆ (part.sign) {
							ಸ್ಥಿತಿ FmtPart::MINUS:       ನಿಲ್ಲಿಸು;
							ಸ್ಥಿತಿ FmtPart::PLUS_MINUS:  ಪೂರ್ವಪ್ರತ್ಯಯ_2 = "+"; ನಿಲ್ಲಿಸು;
							ಸ್ಥಿತಿ FmtPart::SPACE_MINUS: ಪೂರ್ವಪ್ರತ್ಯಯ_2 = " "; ನಿಲ್ಲಿಸು;
						}
					}

					ಆದರೆ (part.ತಳ != 10) {
						ಗಾತ್ರ_t minimum_ಗಾತ್ರ = 1;
						ಗಾಗಿ (ಗಾತ್ರ_t ಸೂಚ್ಯಂಕ = 0; ಸೂಚ್ಯಂಕ < (ಗಾತ್ರ_t)ಮೌಲ್ಯ.ಗಾತ್ರ_2(); ಸೂಚ್ಯಂಕ++)
							ಆದರೆ (ಮೌಲ್ಯ[ಸೂಚ್ಯಂಕ] != State::S0)
								minimum_ಗಾತ್ರ = ಸೂಚ್ಯಂಕ + 1;
						ಮೌಲ್ಯ = ಮೌಲ್ಯ.ಹೊರತೆಗೆ(0, minimum_ಗಾತ್ರ);
					}

					ಆದರೆ (part.ತಳ == 2) {
						ಆದರೆ (part.show_ತಳ)
							ಪೂರ್ವಪ್ರತ್ಯಯ_2 += "0b";
						ಗಾಗಿ (ಗಾತ್ರ_t ಸೂಚ್ಯಂಕ = 0; ಸೂಚ್ಯಂಕ < (ಗಾತ್ರ_t)ಮೌಲ್ಯ.ಗಾತ್ರ_2(); ಸೂಚ್ಯಂಕ++) {
							ಆದರೆ (part.ಗುಂಪು && ಸೂಚ್ಯಂಕ > 0 && ಸೂಚ್ಯಂಕ % 4 == 0)
								buf += '_';
							RTLIL::State ಬಿಟ್ = ಮೌಲ್ಯ[ಸೂಚ್ಯಂಕ];
							ಆದರೆ (ಬಿಟ್ == State::Sx)
								buf += 'x';
							ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಿಟ್ == State::Sz)
								buf += 'z';
							ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಿಟ್ == State::S1)
								buf += '1';
							ಇಲ್ಲದಿದ್ದರೆ /* if (bit == State::S0) */
								buf += '0';
						}
					} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (part.ತಳ == 8 || part.ತಳ == 16) {
						ಆದರೆ (part.show_ತಳ)
							ಪೂರ್ವಪ್ರತ್ಯಯ_2 += (part.ತಳ == 16) ? (part.hex_ಊರ್ಧ್ವ ? "0X" : "0x") : "0o";
						ಗಾತ್ರ_t ಹೆಜ್ಜೆ = (part.ತಳ == 16) ? 4 : 3;
						ಗಾಗಿ (ಗಾತ್ರ_t ಸೂಚ್ಯಂಕ = 0; ಸೂಚ್ಯಂಕ < (ಗಾತ್ರ_t)ಮೌಲ್ಯ.ಗಾತ್ರ_2(); ಸೂಚ್ಯಂಕ += ಹೆಜ್ಜೆ) {
							ಆದರೆ (part.ಗುಂಪು && ಸೂಚ್ಯಂಕ > 0 && ಸೂಚ್ಯಂಕ % (4 * ಹೆಜ್ಜೆ) == 0)
								buf += '_';
							RTLIL::Const subvalue = ಮೌಲ್ಯ.ಹೊರತೆಗೆ(ಸೂಚ್ಯಂಕ, min(ಹೆಜ್ಜೆ, ಮೌಲ್ಯ.ಗಾತ್ರ_2() - ಸೂಚ್ಯಂಕ));
							ತಾರ್ಕಿಕ has_x = ಸುಳ್ಳು, all_x = ನಿಜ, has_z = ಸುಳ್ಳು, all_z = ನಿಜ;
							ಗಾಗಿ (State ಬಿಟ್ : subvalue) {
								ಆದರೆ (ಬಿಟ್ == State::Sx)
									has_x = ನಿಜ;
								ಇಲ್ಲದಿದ್ದರೆ
									all_x = ಸುಳ್ಳು;
								ಆದರೆ (ಬಿಟ್ == State::Sz)
									has_z = ನಿಜ;
								ಇಲ್ಲದಿದ್ದರೆ
									all_z = ಸುಳ್ಳು;
							}
							ಆದರೆ (all_x)
								buf += 'x';
							ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (all_z)
								buf += 'z';
							ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_x)
								buf += 'X';
							ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_z)
								buf += 'Z';
							ಇಲ್ಲದಿದ್ದರೆ {
								ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *digits = part.hex_ಊರ್ಧ್ವ ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (part.ತಳ == 10) {
						ಆದರೆ (part.show_ತಳ)
							ಪೂರ್ವಪ್ರತ್ಯಯ_2 += "0d";
						ಆದರೆ (all_x)
							buf += 'x';
						ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (all_z)
							buf += 'z';
						ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_x)
							buf += 'X';
						ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_z)
							buf += 'Z';
						ಇಲ್ಲದಿದ್ದರೆ {
							log_assert(ಮೌಲ್ಯ.is_fully_def());
							ಆದರೆ (ಮೌಲ್ಯ.is_fully_zero())
								buf += '0';
							ಗಾತ್ರ_t ಸೂಚ್ಯಂಕ = 0;
							ಆಗ (!ಮೌಲ್ಯ.is_fully_zero())	{
								ಆದರೆ (part.ಗುಂಪು && ಸೂಚ್ಯಂಕ > 0 && ಸೂಚ್ಯಂಕ % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(ಮೌಲ್ಯ, 10, ಸುಳ್ಳು, ಸುಳ್ಳು, 4).as_int();
								ಮೌಲ್ಯ = RTLIL::const_div(ಮೌಲ್ಯ, 10, ಸುಳ್ಳು, ಸುಳ್ಳು, ಮೌಲ್ಯ.ಗಾತ್ರ_2());
								ಸೂಚ್ಯಂಕ++;
							}
						}
					} ಇಲ್ಲದಿದ್ದರೆ log_abort();
					ಆದರೆ (part.justify == FmtPart::NUMERIC && part.ಗುಂಪು && part.padding == '0') {
						ಗಾತ್ರ_t ಗುಂಪು_ಗಾತ್ರ = part.ತಳ == 10 ? 3 : 4;
						ಆಗ (ಪೂರ್ವಪ್ರತ್ಯಯ_2.ಗಾತ್ರ_2() + buf.ಗಾತ್ರ_2() < part.ಅಗಲ) {
							ಆದರೆ (buf.ಗಾತ್ರ_2() % (ಗುಂಪು_ಗಾತ್ರ + 1) == ಗುಂಪು_ಗಾತ್ರ)
								buf += '_';
							buf += '0';
						}
					}
					std::ಹಿಮ್ಮುಖ(buf.begin(), buf.ಕೊನೆ());
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (part.type == FmtPart::VLOG_ಸಮಯ) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.ಅಗಲ == 0 || part.padding != '\0');
				ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ_2.ಗಾತ್ರ_2() + buf.ಗಾತ್ರ_2() < part.ಅಗಲ) {
					ಗಾತ್ರ_t pad_ಅಗಲ = part.ಅಗಲ - ಪೂರ್ವಪ್ರತ್ಯಯ_2.ಗಾತ್ರ_2() - buf.ಗಾತ್ರ_2();
					ಆಯ್ಕೆ (part.justify) {
						ಸ್ಥಿತಿ FmtPart::ಎಡ_2:
							str += ಪೂರ್ವಪ್ರತ್ಯಯ_2;
							str += buf;
							str += std::string(pad_ಅಗಲ, part.padding);
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ FmtPart::ಬಲ_2:
							str += std::string(pad_ಅಗಲ, part.padding);
							str += ಪೂರ್ವಪ್ರತ್ಯಯ_2;
							str += buf;
							ನಿಲ್ಲಿಸು;
						ಸ್ಥಿತಿ FmtPart::NUMERIC:
							str += ಪೂರ್ವಪ್ರತ್ಯಯ_2;
							str += std::string(pad_ಅಗಲ, part.padding);
							str += buf;
							ನಿಲ್ಲಿಸು;
					}
				} ಇಲ್ಲದಿದ್ದರೆ {
					str += ಪೂರ್ವಪ್ರತ್ಯಯ_2;
					str += buf;
				}
				ನಿಲ್ಲಿಸು;
			}
		}
	}

	ಹಿಂತಿರುಗಿ str;
}
