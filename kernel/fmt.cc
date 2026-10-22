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

ಶೂನ್ಯ Fmt::append_literal(ಧ್ರುವ std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.ತಳ್ಳು_back(part);
}

ಶೂನ್ಯ Fmt::parse_rtlil(ಧ್ರುವ RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.clear();

	FmtPart part;
	ಪ್ರತಿ (size_t i = 0; i < fmt.size(); i++) {
		ಯದಿ (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} ಅನ್ಯಥಾ ಯದಿ (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == '}') {
			log_assert(ಅಸತ್ಯ && "Unexpected '}' in format string");
		} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == '{') {
			ಯದಿ (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.ತಳ್ಳು_back(part);
				part = {};
			}

			ಯದಿ (++i == fmt.size())
				log_assert(ಅಸತ್ಯ && "Unexpected end in format substitution");

			size_t arg_size = 0;
			ಪ್ರತಿ (; i < fmt.size(); i++) {
				ಯದಿ (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == ':') {
					++i;
					ಭಙ್ಗ;
				} ಅನ್ಯಥಾ {
					log_assert(ಅಸತ್ಯ && "Unexpected character in format substitution");
				}
			}
			ಯದಿ (i == fmt.size())
				log_assert(ಅಸತ್ಯ && "Unexpected end in format substitution");

			ಯದಿ ((size_t)args.size() < arg_size)
				log_assert(ಅಸತ್ಯ && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			ಯದಿ (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				ಗಚ್ಛ success;
			}

			ಯದಿ (fmt[i] == '>')
				part.justify = FmtPart::ಬಲ;
			ಅನ್ಯಥಾ ಯದಿ (fmt[i] == '<')
				part.justify = FmtPart::ಎಡ;
			ಅನ್ಯಥಾ ಯದಿ (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			ಅನ್ಯಥಾ
				log_assert(ಅಸತ್ಯ && "Unexpected justification in format substitution");
			ಯದಿ (++i == fmt.size())
				log_assert(ಅಸತ್ಯ && "Unexpected end in format substitution");

			part.padding = fmt[i];
			ಯದಿ (++i == fmt.size())
				log_assert(ಅಸತ್ಯ && "Unexpected end in format substitution");

			ಪ್ರತಿ (; i < fmt.size(); i++) {
				ಯದಿ (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					ಅನುವರ್ತ;
				} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = ಸತ್ಯ;
				} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = ಸತ್ಯ;
				} ಅನ್ಯಥಾ {
					log_assert(ಅಸತ್ಯ && "Unexpected character in format substitution");
				}
				++i;
				ಭಙ್ಗ;
			}
			ಯದಿ (i == fmt.size())
				log_assert(ಅಸತ್ಯ && "Unexpected end in format substitution");

			ಯದಿ (part.type == FmtPart::INTEGER) {
				ಯದಿ (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					ಯದಿ (++i == fmt.size())
						log_assert(ಅಸತ್ಯ && "Unexpected end in format substitution");
				} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					ಯದಿ (++i == fmt.size())
						log_assert(ಅಸತ್ಯ && "Unexpected end in format substitution");
				} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					ಯದಿ (++i == fmt.size())
						log_assert(ಅಸತ್ಯ && "Unexpected end in format substitution");
				} ಅನ್ಯಥಾ {
					// also accept no sign character and treat like MINUS for compatibility
				}

				ಯದಿ (fmt[i] == '#') {
					part.show_base = ಸತ್ಯ;
					++i;
				}
				ಯದಿ (fmt[i] == '_') {
					part.group = ಸತ್ಯ;
					++i;
				}

				ಯದಿ (fmt[i] == 'u')
					part.signed_ = ಅಸತ್ಯ;
				ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 's')
					part.signed_ = ಸತ್ಯ;
				ಅನ್ಯಥಾ
					log_assert(ಅಸತ್ಯ && "Unexpected character in format substitution");
				ಯದಿ (++i == fmt.size())
					log_assert(ಅಸತ್ಯ && "Unexpected end in format substitution");
			}

		success:
			ಯದಿ (fmt[i] != '}')
				log_assert(ಅಸತ್ಯ && "Expected '}' after format substitution");

			parts.ತಳ್ಳು_back(part);
			part = {};
		} ಅನ್ಯಥಾ {
			part.str += fmt[i];
		}
	}
	ಯದಿ (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.ತಳ್ಳು_back(part);
	}
}

ಶೂನ್ಯ Fmt::emit_rtlil(RTLIL::Cell *cell) ಧ್ರುವ {
	std::string fmt;
	RTLIL::SigSpec args;

	ಪ್ರತಿ (auto &part : parts) {
		ವಿಕಲ್ಪ (part.type) {
			ಪಕ್ಷ FmtPart::LITERAL:
				ಪ್ರತಿ (ವರ್ಣ c : part.str) {
					ಯದಿ (c == '{')
						fmt += "{{";
					ಅನ್ಯಥಾ ಯದಿ (c == '}')
						fmt += "}}";
					ಅನ್ಯಥಾ
						fmt += c;
				}
				ಭಙ್ಗ;

			ಪಕ್ಷ FmtPart::UNICHAR:
				log_assert(part.sig.size() <= 32);
				fmt += "{U}";
				ಭಙ್ಗ;

			ಪಕ್ಷ FmtPart::VLOG_TIME:
				log_assert(part.sig.size() == 0);
				YS_FALLTHROUGH
			ಪಕ್ಷ FmtPart::STRING:
				log_assert(part.sig.size() % 8 == 0);
				YS_FALLTHROUGH
			ಪಕ್ಷ FmtPart::INTEGER:
				args.append(part.sig);
				fmt += '{';
				fmt += std::to_string(part.sig.size());
				fmt += ':';
				ಯದಿ (part.justify == FmtPart::ಬಲ)
					fmt += '>';
				ಅನ್ಯಥಾ ಯದಿ (part.justify == FmtPart::ಎಡ)
					fmt += '<';
				ಅನ್ಯಥಾ ಯದಿ (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				ಅನ್ಯಥಾ log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				ಯದಿ (part.width > 0)
					fmt += std::to_string(part.width);
				ಯದಿ (part.type == FmtPart::INTEGER) {
					ವಿಕಲ್ಪ (part.base) {
						ಪಕ್ಷ  2: fmt += 'b'; ಭಙ್ಗ;
						ಪಕ್ಷ  8: fmt += 'o'; ಭಙ್ಗ;
						ಪಕ್ಷ 10: fmt += 'd'; ಭಙ್ಗ;
						ಪಕ್ಷ 16: fmt += part.hex_upper ? 'H' : 'h'; ಭಙ್ಗ;
						ಸ್ವಭಾವ: log_abort();
					}
					ವಿಕಲ್ಪ (part.sign) {
						ಪಕ್ಷ FmtPart::MINUS:       fmt += '-'; ಭಙ್ಗ;
						ಪಕ್ಷ FmtPart::PLUS_MINUS:  fmt += '+'; ಭಙ್ಗ;
						ಪಕ್ಷ FmtPart::SPACE_MINUS: fmt += ' '; ಭಙ್ಗ;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} ಅನ್ಯಥಾ ಯದಿ (part.type == FmtPart::STRING) {
					fmt += 'c';
				} ಅನ್ಯಥಾ ಯದಿ (part.type == FmtPart::VLOG_TIME) {
					ಯದಿ (part.realtime)
						fmt += 'r';
					ಅನ್ಯಥಾ
						fmt += 't';
				} ಅನ್ಯಥಾ log_abort();
				fmt += '}';
				ಭಙ್ಗ;

			ಸ್ವಭಾವ: log_abort();
		}
	}

	cell->setParam(ID(FORMAT), fmt);
	cell->setParam(ID(ARGS_WIDTH), args.size());
	cell->setPort(ID(ARGS), args);
}

ಸ್ಥಿರ size_t compute_required_decimal_places(size_t size, ತಾರ್ಕಿಕ signed_)
{
	BigUnsigned max;
	ಯದಿ (!signed_)
		max.setBit(size, ಸತ್ಯ);
	ಅನ್ಯಥಾ
		max.setBit(size - 1, ಸತ್ಯ);
	size_t places = 0;
	ಯಾವತ್ (!max.isZero()) {
		places++;
		max /= 10;
	}
	ಯದಿ (signed_)
		places++;
	ಪ್ರತ್ಯಾಗಮ places;
}

ಸ್ಥಿರ size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, ಸತ್ಯ);
	size_t places = 0;
	ಯಾವತ್ (!max.isZero()) {
		places++;
		max /= base;
	}
	ಪ್ರತ್ಯಾಗಮ places;
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
ಶೂನ್ಯ Fmt::apply_verilog_automatic_sizing_and_add(FmtPart &part)
{
	ಯದಿ (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.ತಳ್ಳು_back(part);
		ಪ್ರತ್ಯಾಗಮ;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	ಯದಿ (part.width < places) {
		part.justify = FmtPart::ಬಲ;
		part.width = places;
		parts.ತಳ್ಳು_back(part);
	} ಅನ್ಯಥಾ ಯದಿ (part.width == places) {
		parts.ತಳ್ಳು_back(part);
	} ಅನ್ಯಥಾ ಯದಿ (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		ಯದಿ (part.justify == FmtPart::ಬಲ) {
			append_literal(gap);
			parts.ತಳ್ಳು_back(part);
		} ಅನ್ಯಥಾ {
			part.justify = FmtPart::ಬಲ;
			parts.ತಳ್ಳು_back(part);
			append_literal(gap);
		}
	}
}

ಶೂನ್ಯ Fmt::parse_verilog(ಧ್ರುವ std::vector<VerilogFmtArg> &args, ತಾರ್ಕಿಕ sformat_like, ಪೂರ್ಣಾಙ್ಕ default_base, ಧ್ರುವ std::string &task_name, ಧ್ರುವ std::string &module_name)
{
	parts.clear();

	auto arg = args.begin();
	ಪ್ರತಿ (; arg != args.end(); ++arg) {
		ವಿಕಲ್ಪ (arg->type) {
			ಪಕ್ಷ VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				ಭಙ್ಗ;
			}

			ಪಕ್ಷ VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.ತಳ್ಳು_back(part);
				ಭಙ್ಗ;
			}

			ಪಕ್ಷ VerilogFmtArg::STRING: {
				ಯದಿ (arg == args.begin() || !sformat_like) {
					ಧ್ರುವ auto fmtarg = arg;
					ಧ್ರುವ std::string &fmt = fmtarg->str;
					FmtPart part = {};
					ಪ್ರತಿ (size_t i = 0; i < fmt.size(); i++) {
						ಯದಿ (fmt[i] != '%') {
							part.str += fmt[i];
						} ಅನ್ಯಥಾ ಯದಿ (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} ಅನ್ಯಥಾ ಯದಿ (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} ಅನ್ಯಥಾ ಯದಿ (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} ಅನ್ಯಥಾ {
							ಯದಿ (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.ತಳ್ಳು_back(part);
								part = {};
							}
							ಯದಿ (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ಯದಿ (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							ಪ್ರತಿ (; i < fmt.size(); i++) {
								ಯದಿ (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::ಎಡ;
								} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} ಅನ್ಯಥಾ ಭಙ್ಗ;
							}
							ಯದಿ (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ತಾರ್ಕಿಕ has_leading_zero = ಅಸತ್ಯ, has_width = ಅಸತ್ಯ;
							ಪ್ರತಿ (; i < fmt.size(); i++) {
								ಯದಿ (fmt[i] >= '0' && fmt[i] <= '9') {
									ಯದಿ (fmt[i] == '0' && !has_width) {
										has_leading_zero = ಸತ್ಯ;
									} ಅನ್ಯಥಾ {
										has_width = ಸತ್ಯ;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									ಅನುವರ್ತ;
								} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									ಯದಿ ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} ಅನ್ಯಥಾ ಯದಿ (fmt[i] == 't' || fmt[i] == 'T') {
									ಯದಿ (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										ಯದಿ (!has_width && !has_leading_zero)
											part.width = 20;
									} ಅನ್ಯಥಾ {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} ಅನ್ಯಥಾ {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								ಭಙ್ಗ;
							}
							ಯದಿ (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ಯದಿ (part.padding == '\0') {
								ಯದಿ (has_leading_zero && part.justify == FmtPart::ಬಲ) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} ಅನ್ಯಥಾ {
									part.padding = ' ';
								}
							}

							ಯದಿ (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							ಯದಿ (part.base != 10)
								part.signed_ = ಅಸತ್ಯ;
							ಯದಿ (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							ಅನ್ಯಥಾ
								parts.ತಳ್ಳು_back(part);
							part = {};
						}
					}
					ಯದಿ (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.ತಳ್ಳು_back(part);
					}
				} ಅನ್ಯಥಾ {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.ತಳ್ಳು_back(part);
				}
				ಭಙ್ಗ;
			}

			ಸ್ವಭಾವ: log_abort();
		}
	}
}

std::vector<VerilogFmtArg> Fmt::emit_verilog() ಧ್ರುವ
{
	std::vector<VerilogFmtArg> args;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	ಪ್ರತಿ (auto &part : parts) {
		ವಿಕಲ್ಪ (part.type) {
			ಪಕ್ಷ FmtPart::LITERAL:
				ಪ್ರತಿ (ವರ್ಣ c : part.str) {
					ಯದಿ (c == '%')
						fmt.str += "%%";
					ಅನ್ಯಥಾ
						fmt.str += c;
				}
				ಭಙ್ಗ;

			ಪಕ್ಷ FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.ತಳ್ಳು_back(arg);

				fmt.str += '%';
				ಯದಿ (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				ಯದಿ (part.justify == FmtPart::ಎಡ)
					fmt.str += '-';
				ಯದಿ (part.width == 0) {
					fmt.str += '0';
				} ಅನ್ಯಥಾ ಯದಿ (part.width > 0) {
					ಯದಿ (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				ವಿಕಲ್ಪ (part.base) {
					ಪಕ್ಷ  2: fmt.str += 'b'; ಭಙ್ಗ;
					ಪಕ್ಷ  8: fmt.str += 'o'; ಭಙ್ಗ;
					ಪಕ್ಷ 10: fmt.str += 'd'; ಭಙ್ಗ;
					ಪಕ್ಷ 16: fmt.str += 'h'; ಭಙ್ಗ; // treat uppercase hex as lowercase
					ಸ್ವಭಾವ: log_abort();
				}
				ಭಙ್ಗ;
			}

			ಪಕ್ಷ FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				args.ತಳ್ಳು_back(arg);

				fmt.str += '%';
				ಯದಿ (part.justify == FmtPart::ಎಡ)
					fmt.str += '-';
				ಯದಿ (part.sig.size() == 8) {
					ಯದಿ (part.width > 0) {
						ಯದಿ (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} ಅನ್ಯಥಾ {
					log_assert(part.sig.size() % 8 == 0);
					ಯದಿ (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				ಭಙ್ಗ;
			}

			ಪಕ್ಷ FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.ತಳ್ಳು_back(arg);

				fmt.str += "%c";
				ಭಙ್ಗ;
			}

			ಪಕ್ಷ FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				ಯದಿ (part.realtime)
					arg.realtime = ಸತ್ಯ;
				args.ತಳ್ಳು_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				ಯದಿ (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				ಯದಿ (part.justify == FmtPart::ಎಡ)
					fmt.str += '-';
				ಯದಿ (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				ಭಙ್ಗ;
			}

			ಸ್ವಭಾವ: log_abort();
		}
	}

	args.ಸೇರಿಸು(args.begin(), fmt);
	ಪ್ರತ್ಯಾಗಮ args;
}

std::string escape_cxx_string(ಧ್ರುವ std::string &ಒಳಹರಿವು)
{
	std::string output = "\"";
	ಪ್ರತಿ (auto c : ಒಳಹರಿವು) {
		ಯದಿ (::isprint(c)) {
			ಯದಿ (c == '\\' || c == '"')
				output.ತಳ್ಳು_back('\\');
			output.ತಳ್ಳು_back(c);
		} ಅನ್ಯಥಾ {
			ವರ್ಣ l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.ತಳ್ಳು_back('\\');
			output.ತಳ್ಳು_back('0' + h);
			output.ತಳ್ಳು_back('0' + m);
			output.ತಳ್ಳು_back('0' + l);
		}
	}
	output.ತಳ್ಳು_back('"');
	ಯದಿ (output.find('\0') != std::string::npos) {
		output.ಸೇರಿಸು(0, "std::string {");
		output.append(stringf(", %zu}", ಒಳಹರಿವು.size()));
	}
	ಪ್ರತ್ಯಾಗಮ output;
}

ಶೂನ್ಯ Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<ಶೂನ್ಯ(ಧ್ರುವ RTLIL::SigSpec &)> emit_sig, ಧ್ರುವ std::string &context) ಧ್ರುವ
{
	os << indent << "std::string buf;\n";
	ಪ್ರತಿ (auto &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		ವಿಕಲ್ಪ (part.type) {
			ಪಕ್ಷ FmtPart::LITERAL:   os << "LITERAL";   ಭಙ್ಗ;
			ಪಕ್ಷ FmtPart::INTEGER:   os << "INTEGER";   ಭಙ್ಗ;
			ಪಕ್ಷ FmtPart::STRING:    os << "STRING";    ಭಙ್ಗ;
			ಪಕ್ಷ FmtPart::UNICHAR:   os << "UNICHAR";   ಭಙ್ಗ;
			ಪಕ್ಷ FmtPart::VLOG_TIME: os << "VLOG_TIME"; ಭಙ್ಗ;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		ವಿಕಲ್ಪ (part.justify) {
			ಪಕ್ಷ FmtPart::ಎಡ:    os << "LEFT";    ಭಙ್ಗ;
			ಪಕ್ಷ FmtPart::ಬಲ:   os << "RIGHT";   ಭಙ್ಗ;
			ಪಕ್ಷ FmtPart::NUMERIC: os << "NUMERIC"; ಭಙ್ಗ;
		}
		os << ", ";
		os << "(char)" << (ಪೂರ್ಣಾಙ್ಕ)part.padding << ", ";
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		ವಿಕಲ್ಪ (part.sign) {
			ಪಕ್ಷ FmtPart::MINUS:       os << "MINUS";       ಭಙ್ಗ;
			ಪಕ್ಷ FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  ಭಙ್ಗ;
			ಪಕ್ಷ FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; ಭಙ್ಗ;
		}
		os << ", ";
		os << part.hex_upper << ", ";
		os << part.show_base << ", ";
		os << part.group << ", ";
		os << part.realtime;
		os << " }.render(";
		emit_sig(part.sig);
		os << ", " << context << ");\n";
	}
	os << indent << "return buf;\n";
}

std::string Fmt::render() ಧ್ರುವ
{
	std::string str;

	ಪ್ರತಿ (auto &part : parts) {
		ವಿಕಲ್ಪ (part.type) {
			ಪಕ್ಷ FmtPart::LITERAL:
				str += part.str;
				ಭಙ್ಗ;

			ಪಕ್ಷ FmtPart::UNICHAR: {
				RTLIL::Const ಮೌಲ್ಯ = part.sig.as_const();
				uint32_t codepoint = ಮೌಲ್ಯ.as_int();
				ಯದಿ (codepoint >= 0x10000)
					str += (ವರ್ಣ)(0xf0 |  (codepoint >> 18));
				ಅನ್ಯಥಾ ಯದಿ (codepoint >= 0x800)
					str += (ವರ್ಣ)(0xe0 |  (codepoint >> 12));
				ಅನ್ಯಥಾ ಯದಿ (codepoint >= 0x80)
					str += (ವರ್ಣ)(0xc0 |  (codepoint >>  6));
				ಅನ್ಯಥಾ
					str += (ವರ್ಣ)codepoint;
				ಯದಿ (codepoint >= 0x10000)
					str += (ವರ್ಣ)(0x80 | ((codepoint >> 12) & 0x3f));
				ಯದಿ (codepoint >= 0x800)
					str += (ವರ್ಣ)(0x80 | ((codepoint >>  6) & 0x3f));
				ಯದಿ (codepoint >= 0x80)
					str += (ವರ್ಣ)(0x80 | ((codepoint >>  0) & 0x3f));
				ಭಙ್ಗ;
			}

			ಪಕ್ಷ FmtPart::INTEGER:
			ಪಕ್ಷ FmtPart::STRING:
			ಪಕ್ಷ FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				ಯದಿ (part.type == FmtPart::INTEGER) {
					RTLIL::Const ಮೌಲ್ಯ = part.sig.as_const();
					ತಾರ್ಕಿಕ has_x = ಅಸತ್ಯ, all_x = ಸತ್ಯ, has_z = ಅಸತ್ಯ, all_z = ಸತ್ಯ;
					ಪ್ರತಿ (State bit : ಮೌಲ್ಯ) {
						ಯದಿ (bit == State::Sx)
							has_x = ಸತ್ಯ;
						ಅನ್ಯಥಾ
							all_x = ಅಸತ್ಯ;
						ಯದಿ (bit == State::Sz)
							has_z = ಸತ್ಯ;
						ಅನ್ಯಥಾ
							all_z = ಅಸತ್ಯ;
					}

					ಯದಿ (!has_z && !has_x && part.signed_ && ಮೌಲ್ಯ[ಮೌಲ್ಯ.size() - 1]) {
						prefix = "-";
						ಮೌಲ್ಯ = RTLIL::const_neg(ಮೌಲ್ಯ, {}, part.signed_, {}, ಮೌಲ್ಯ.size() + 1);
					} ಅನ್ಯಥಾ {
						ವಿಕಲ್ಪ (part.sign) {
							ಪಕ್ಷ FmtPart::MINUS:       ಭಙ್ಗ;
							ಪಕ್ಷ FmtPart::PLUS_MINUS:  prefix = "+"; ಭಙ್ಗ;
							ಪಕ್ಷ FmtPart::SPACE_MINUS: prefix = " "; ಭಙ್ಗ;
						}
					}

					ಯದಿ (part.base != 10) {
						size_t minimum_size = 1;
						ಪ್ರತಿ (size_t index = 0; index < (size_t)ಮೌಲ್ಯ.size(); index++)
							ಯದಿ (ಮೌಲ್ಯ[index] != State::S0)
								minimum_size = index + 1;
						ಮೌಲ್ಯ = ಮೌಲ್ಯ.extract(0, minimum_size);
					}

					ಯದಿ (part.base == 2) {
						ಯದಿ (part.show_base)
							prefix += "0b";
						ಪ್ರತಿ (size_t index = 0; index < (size_t)ಮೌಲ್ಯ.size(); index++) {
							ಯದಿ (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = ಮೌಲ್ಯ[index];
							ಯದಿ (bit == State::Sx)
								buf += 'x';
							ಅನ್ಯಥಾ ಯದಿ (bit == State::Sz)
								buf += 'z';
							ಅನ್ಯಥಾ ಯದಿ (bit == State::S1)
								buf += '1';
							ಅನ್ಯಥಾ /* if (bit == State::S0) */
								buf += '0';
						}
					} ಅನ್ಯಥಾ ಯದಿ (part.base == 8 || part.base == 16) {
						ಯದಿ (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						ಪ್ರತಿ (size_t index = 0; index < (size_t)ಮೌಲ್ಯ.size(); index += step) {
							ಯದಿ (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = ಮೌಲ್ಯ.extract(index, min(step, ಮೌಲ್ಯ.size() - index));
							ತಾರ್ಕಿಕ has_x = ಅಸತ್ಯ, all_x = ಸತ್ಯ, has_z = ಅಸತ್ಯ, all_z = ಸತ್ಯ;
							ಪ್ರತಿ (State bit : subvalue) {
								ಯದಿ (bit == State::Sx)
									has_x = ಸತ್ಯ;
								ಅನ್ಯಥಾ
									all_x = ಅಸತ್ಯ;
								ಯದಿ (bit == State::Sz)
									has_z = ಸತ್ಯ;
								ಅನ್ಯಥಾ
									all_z = ಅಸತ್ಯ;
							}
							ಯದಿ (all_x)
								buf += 'x';
							ಅನ್ಯಥಾ ಯದಿ (all_z)
								buf += 'z';
							ಅನ್ಯಥಾ ಯದಿ (has_x)
								buf += 'X';
							ಅನ್ಯಥಾ ಯದಿ (has_z)
								buf += 'Z';
							ಅನ್ಯಥಾ {
								ಧ್ರುವ ವರ್ಣ *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} ಅನ್ಯಥಾ ಯದಿ (part.base == 10) {
						ಯದಿ (part.show_base)
							prefix += "0d";
						ಯದಿ (all_x)
							buf += 'x';
						ಅನ್ಯಥಾ ಯದಿ (all_z)
							buf += 'z';
						ಅನ್ಯಥಾ ಯದಿ (has_x)
							buf += 'X';
						ಅನ್ಯಥಾ ಯದಿ (has_z)
							buf += 'Z';
						ಅನ್ಯಥಾ {
							log_assert(ಮೌಲ್ಯ.is_fully_def());
							ಯದಿ (ಮೌಲ್ಯ.is_fully_zero())
								buf += '0';
							size_t index = 0;
							ಯಾವತ್ (!ಮೌಲ್ಯ.is_fully_zero())	{
								ಯದಿ (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(ಮೌಲ್ಯ, 10, ಅಸತ್ಯ, ಅಸತ್ಯ, 4).as_int();
								ಮೌಲ್ಯ = RTLIL::const_div(ಮೌಲ್ಯ, 10, ಅಸತ್ಯ, ಅಸತ್ಯ, ಮೌಲ್ಯ.size());
								index++;
							}
						}
					} ಅನ್ಯಥಾ log_abort();
					ಯದಿ (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						ಯಾವತ್ (prefix.size() + buf.size() < part.width) {
							ಯದಿ (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} ಅನ್ಯಥಾ ಯದಿ (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} ಅನ್ಯಥಾ ಯದಿ (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				ಯದಿ (prefix.size() + buf.size() < part.width) {
					size_t pad_width = part.width - prefix.size() - buf.size();
					ವಿಕಲ್ಪ (part.justify) {
						ಪಕ್ಷ FmtPart::ಎಡ:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							ಭಙ್ಗ;
						ಪಕ್ಷ FmtPart::ಬಲ:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							ಭಙ್ಗ;
						ಪಕ್ಷ FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							ಭಙ್ಗ;
					}
				} ಅನ್ಯಥಾ {
					str += prefix;
					str += buf;
				}
				ಭಙ್ಗ;
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ str;
}
