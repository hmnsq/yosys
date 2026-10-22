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

#ଅନ୍ତର୍ଭାଵ "libs/bigint/BigUnsigned.hh"
#ଅନ୍ତର୍ଭାଵ "kernel/fmt.h"

USING_YOSYS_NAMESPACE

ଶୂନ୍ୟ Fmt::append_literal(ଅଚଳ std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.ଠେଲ_back(part);
}

ଶୂନ୍ୟ Fmt::parse_rtlil(ଅଚଳ RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.clear();

	FmtPart part;
	ପାଇଁ (size_t i = 0; i < fmt.size(); i++) {
		ଯଦି (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} ଅନ୍ୟଥା ଯଦି (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} ଅନ୍ୟଥା ଯଦି (fmt[i] == '}') {
			log_assert(ମିଥ୍ୟା && "Unexpected '}' in format string");
		} ଅନ୍ୟଥା ଯଦି (fmt[i] == '{') {
			ଯଦି (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.ଠେଲ_back(part);
				part = {};
			}

			ଯଦି (++i == fmt.size())
				log_assert(ମିଥ୍ୟା && "Unexpected end in format substitution");

			size_t arg_size = 0;
			ପାଇଁ (; i < fmt.size(); i++) {
				ଯଦି (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} ଅନ୍ୟଥା ଯଦି (fmt[i] == ':') {
					++i;
					ଭାଙ୍ଗ;
				} ଅନ୍ୟଥା {
					log_assert(ମିଥ୍ୟା && "Unexpected character in format substitution");
				}
			}
			ଯଦି (i == fmt.size())
				log_assert(ମିଥ୍ୟା && "Unexpected end in format substitution");

			ଯଦି ((size_t)args.size() < arg_size)
				log_assert(ମିଥ୍ୟା && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			ଯଦି (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				ଯାଅ success;
			}

			ଯଦି (fmt[i] == '>')
				part.justify = FmtPart::ଡାହାଣ;
			ଅନ୍ୟଥା ଯଦି (fmt[i] == '<')
				part.justify = FmtPart::ବାମ;
			ଅନ୍ୟଥା ଯଦି (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			ଅନ୍ୟଥା
				log_assert(ମିଥ୍ୟା && "Unexpected justification in format substitution");
			ଯଦି (++i == fmt.size())
				log_assert(ମିଥ୍ୟା && "Unexpected end in format substitution");

			part.padding = fmt[i];
			ଯଦି (++i == fmt.size())
				log_assert(ମିଥ୍ୟା && "Unexpected end in format substitution");

			ପାଇଁ (; i < fmt.size(); i++) {
				ଯଦି (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					ଜାରି;
				} ଅନ୍ୟଥା ଯଦି (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} ଅନ୍ୟଥା ଯଦି (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} ଅନ୍ୟଥା ଯଦି (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} ଅନ୍ୟଥା ଯଦି (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} ଅନ୍ୟଥା ଯଦି (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = ସତ୍ୟ;
				} ଅନ୍ୟଥା ଯଦି (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} ଅନ୍ୟଥା ଯଦି (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} ଅନ୍ୟଥା ଯଦି (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = ସତ୍ୟ;
				} ଅନ୍ୟଥା {
					log_assert(ମିଥ୍ୟା && "Unexpected character in format substitution");
				}
				++i;
				ଭାଙ୍ଗ;
			}
			ଯଦି (i == fmt.size())
				log_assert(ମିଥ୍ୟା && "Unexpected end in format substitution");

			ଯଦି (part.type == FmtPart::INTEGER) {
				ଯଦି (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					ଯଦି (++i == fmt.size())
						log_assert(ମିଥ୍ୟା && "Unexpected end in format substitution");
				} ଅନ୍ୟଥା ଯଦି (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					ଯଦି (++i == fmt.size())
						log_assert(ମିଥ୍ୟା && "Unexpected end in format substitution");
				} ଅନ୍ୟଥା ଯଦି (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					ଯଦି (++i == fmt.size())
						log_assert(ମିଥ୍ୟା && "Unexpected end in format substitution");
				} ଅନ୍ୟଥା {
					// also accept no sign character and treat like MINUS for compatibility
				}

				ଯଦି (fmt[i] == '#') {
					part.show_base = ସତ୍ୟ;
					++i;
				}
				ଯଦି (fmt[i] == '_') {
					part.group = ସତ୍ୟ;
					++i;
				}

				ଯଦି (fmt[i] == 'u')
					part.signed_ = ମିଥ୍ୟା;
				ଅନ୍ୟଥା ଯଦି (fmt[i] == 's')
					part.signed_ = ସତ୍ୟ;
				ଅନ୍ୟଥା
					log_assert(ମିଥ୍ୟା && "Unexpected character in format substitution");
				ଯଦି (++i == fmt.size())
					log_assert(ମିଥ୍ୟା && "Unexpected end in format substitution");
			}

		success:
			ଯଦି (fmt[i] != '}')
				log_assert(ମିଥ୍ୟା && "Expected '}' after format substitution");

			parts.ଠେଲ_back(part);
			part = {};
		} ଅନ୍ୟଥା {
			part.str += fmt[i];
		}
	}
	ଯଦି (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.ଠେଲ_back(part);
	}
}

ଶୂନ୍ୟ Fmt::emit_rtlil(RTLIL::Cell *cell) ଅଚଳ {
	std::string fmt;
	RTLIL::SigSpec args;

	ପାଇଁ (auto &part : parts) {
		ବାଛ (part.type) {
			ସ୍ଥିତି FmtPart::LITERAL:
				ପାଇଁ (ଅକ୍ଷର c : part.str) {
					ଯଦି (c == '{')
						fmt += "{{";
					ଅନ୍ୟଥା ଯଦି (c == '}')
						fmt += "}}";
					ଅନ୍ୟଥା
						fmt += c;
				}
				ଭାଙ୍ଗ;

			ସ୍ଥିତି FmtPart::UNICHAR:
				log_assert(part.sig.size() <= 32);
				fmt += "{U}";
				ଭାଙ୍ଗ;

			ସ୍ଥିତି FmtPart::VLOG_TIME:
				log_assert(part.sig.size() == 0);
				YS_FALLTHROUGH
			ସ୍ଥିତି FmtPart::STRING:
				log_assert(part.sig.size() % 8 == 0);
				YS_FALLTHROUGH
			ସ୍ଥିତି FmtPart::INTEGER:
				args.append(part.sig);
				fmt += '{';
				fmt += std::to_string(part.sig.size());
				fmt += ':';
				ଯଦି (part.justify == FmtPart::ଡାହାଣ)
					fmt += '>';
				ଅନ୍ୟଥା ଯଦି (part.justify == FmtPart::ବାମ)
					fmt += '<';
				ଅନ୍ୟଥା ଯଦି (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				ଅନ୍ୟଥା log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				ଯଦି (part.width > 0)
					fmt += std::to_string(part.width);
				ଯଦି (part.type == FmtPart::INTEGER) {
					ବାଛ (part.base) {
						ସ୍ଥିତି  2: fmt += 'b'; ଭାଙ୍ଗ;
						ସ୍ଥିତି  8: fmt += 'o'; ଭାଙ୍ଗ;
						ସ୍ଥିତି 10: fmt += 'd'; ଭାଙ୍ଗ;
						ସ୍ଥିତି 16: fmt += part.hex_upper ? 'H' : 'h'; ଭାଙ୍ଗ;
						ପୂର୍ବନିର୍ଦ୍ଧାରିତ: log_abort();
					}
					ବାଛ (part.sign) {
						ସ୍ଥିତି FmtPart::MINUS:       fmt += '-'; ଭାଙ୍ଗ;
						ସ୍ଥିତି FmtPart::PLUS_MINUS:  fmt += '+'; ଭାଙ୍ଗ;
						ସ୍ଥିତି FmtPart::SPACE_MINUS: fmt += ' '; ଭାଙ୍ଗ;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} ଅନ୍ୟଥା ଯଦି (part.type == FmtPart::STRING) {
					fmt += 'c';
				} ଅନ୍ୟଥା ଯଦି (part.type == FmtPart::VLOG_TIME) {
					ଯଦି (part.realtime)
						fmt += 'r';
					ଅନ୍ୟଥା
						fmt += 't';
				} ଅନ୍ୟଥା log_abort();
				fmt += '}';
				ଭାଙ୍ଗ;

			ପୂର୍ବନିର୍ଦ୍ଧାରିତ: log_abort();
		}
	}

	cell->setParam(ID(FORMAT), fmt);
	cell->setParam(ID(ARGS_WIDTH), args.size());
	cell->setPort(ID(ARGS), args);
}

ସ୍ଥିର size_t compute_required_decimal_places(size_t size, ତାର୍କିକ signed_)
{
	BigUnsigned max;
	ଯଦି (!signed_)
		max.setBit(size, ସତ୍ୟ);
	ଅନ୍ୟଥା
		max.setBit(size - 1, ସତ୍ୟ);
	size_t places = 0;
	ଯେତେବେଳେ (!max.isZero()) {
		places++;
		max /= 10;
	}
	ଯଦି (signed_)
		places++;
	ଫେରନ୍ତୁ places;
}

ସ୍ଥିର size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, ସତ୍ୟ);
	size_t places = 0;
	ଯେତେବେଳେ (!max.isZero()) {
		places++;
		max /= base;
	}
	ଫେରନ୍ତୁ places;
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
ଶୂନ୍ୟ Fmt::apply_verilog_automatic_sizing_and_add(FmtPart &part)
{
	ଯଦି (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.ଠେଲ_back(part);
		ଫେରନ୍ତୁ;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	ଯଦି (part.width < places) {
		part.justify = FmtPart::ଡାହାଣ;
		part.width = places;
		parts.ଠେଲ_back(part);
	} ଅନ୍ୟଥା ଯଦି (part.width == places) {
		parts.ଠେଲ_back(part);
	} ଅନ୍ୟଥା ଯଦି (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		ଯଦି (part.justify == FmtPart::ଡାହାଣ) {
			append_literal(gap);
			parts.ଠେଲ_back(part);
		} ଅନ୍ୟଥା {
			part.justify = FmtPart::ଡାହାଣ;
			parts.ଠେଲ_back(part);
			append_literal(gap);
		}
	}
}

ଶୂନ୍ୟ Fmt::parse_verilog(ଅଚଳ std::vector<VerilogFmtArg> &args, ତାର୍କିକ sformat_like, ପୂର୍ଣ୍ଣସଂଖ୍ୟା default_base, ଅଚଳ std::string &task_name, ଅଚଳ std::string &module_name)
{
	parts.clear();

	auto arg = args.begin();
	ପାଇଁ (; arg != args.end(); ++arg) {
		ବାଛ (arg->type) {
			ସ୍ଥିତି VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				ଭାଙ୍ଗ;
			}

			ସ୍ଥିତି VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.ଠେଲ_back(part);
				ଭାଙ୍ଗ;
			}

			ସ୍ଥିତି VerilogFmtArg::STRING: {
				ଯଦି (arg == args.begin() || !sformat_like) {
					ଅଚଳ auto fmtarg = arg;
					ଅଚଳ std::string &fmt = fmtarg->str;
					FmtPart part = {};
					ପାଇଁ (size_t i = 0; i < fmt.size(); i++) {
						ଯଦି (fmt[i] != '%') {
							part.str += fmt[i];
						} ଅନ୍ୟଥା ଯଦି (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} ଅନ୍ୟଥା ଯଦି (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} ଅନ୍ୟଥା ଯଦି (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} ଅନ୍ୟଥା {
							ଯଦି (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.ଠେଲ_back(part);
								part = {};
							}
							ଯଦି (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ଯଦି (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							ପାଇଁ (; i < fmt.size(); i++) {
								ଯଦି (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::ବାମ;
								} ଅନ୍ୟଥା ଯଦି (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} ଅନ୍ୟଥା ଭାଙ୍ଗ;
							}
							ଯଦି (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ତାର୍କିକ has_leading_zero = ମିଥ୍ୟା, has_width = ମିଥ୍ୟା;
							ପାଇଁ (; i < fmt.size(); i++) {
								ଯଦି (fmt[i] >= '0' && fmt[i] <= '9') {
									ଯଦି (fmt[i] == '0' && !has_width) {
										has_leading_zero = ସତ୍ୟ;
									} ଅନ୍ୟଥା {
										has_width = ସତ୍ୟ;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									ଜାରି;
								} ଅନ୍ୟଥା ଯଦି (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} ଅନ୍ୟଥା ଯଦି (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} ଅନ୍ୟଥା ଯଦି (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} ଅନ୍ୟଥା ଯଦି (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} ଅନ୍ୟଥା ଯଦି (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} ଅନ୍ୟଥା ଯଦି (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									ଯଦି ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} ଅନ୍ୟଥା ଯଦି (fmt[i] == 't' || fmt[i] == 'T') {
									ଯଦି (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										ଯଦି (!has_width && !has_leading_zero)
											part.width = 20;
									} ଅନ୍ୟଥା {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} ଅନ୍ୟଥା {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								ଭାଙ୍ଗ;
							}
							ଯଦି (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ଯଦି (part.padding == '\0') {
								ଯଦି (has_leading_zero && part.justify == FmtPart::ଡାହାଣ) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} ଅନ୍ୟଥା {
									part.padding = ' ';
								}
							}

							ଯଦି (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							ଯଦି (part.base != 10)
								part.signed_ = ମିଥ୍ୟା;
							ଯଦି (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							ଅନ୍ୟଥା
								parts.ଠେଲ_back(part);
							part = {};
						}
					}
					ଯଦି (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.ଠେଲ_back(part);
					}
				} ଅନ୍ୟଥା {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.ଠେଲ_back(part);
				}
				ଭାଙ୍ଗ;
			}

			ପୂର୍ବନିର୍ଦ୍ଧାରିତ: log_abort();
		}
	}
}

std::vector<VerilogFmtArg> Fmt::emit_verilog() ଅଚଳ
{
	std::vector<VerilogFmtArg> args;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	ପାଇଁ (auto &part : parts) {
		ବାଛ (part.type) {
			ସ୍ଥିତି FmtPart::LITERAL:
				ପାଇଁ (ଅକ୍ଷର c : part.str) {
					ଯଦି (c == '%')
						fmt.str += "%%";
					ଅନ୍ୟଥା
						fmt.str += c;
				}
				ଭାଙ୍ଗ;

			ସ୍ଥିତି FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.ଠେଲ_back(arg);

				fmt.str += '%';
				ଯଦି (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				ଯଦି (part.justify == FmtPart::ବାମ)
					fmt.str += '-';
				ଯଦି (part.width == 0) {
					fmt.str += '0';
				} ଅନ୍ୟଥା ଯଦି (part.width > 0) {
					ଯଦି (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				ବାଛ (part.base) {
					ସ୍ଥିତି  2: fmt.str += 'b'; ଭାଙ୍ଗ;
					ସ୍ଥିତି  8: fmt.str += 'o'; ଭାଙ୍ଗ;
					ସ୍ଥିତି 10: fmt.str += 'd'; ଭାଙ୍ଗ;
					ସ୍ଥିତି 16: fmt.str += 'h'; ଭାଙ୍ଗ; // treat uppercase hex as lowercase
					ପୂର୍ବନିର୍ଦ୍ଧାରିତ: log_abort();
				}
				ଭାଙ୍ଗ;
			}

			ସ୍ଥିତି FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				args.ଠେଲ_back(arg);

				fmt.str += '%';
				ଯଦି (part.justify == FmtPart::ବାମ)
					fmt.str += '-';
				ଯଦି (part.sig.size() == 8) {
					ଯଦି (part.width > 0) {
						ଯଦି (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} ଅନ୍ୟଥା {
					log_assert(part.sig.size() % 8 == 0);
					ଯଦି (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				ଭାଙ୍ଗ;
			}

			ସ୍ଥିତି FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.ଠେଲ_back(arg);

				fmt.str += "%c";
				ଭାଙ୍ଗ;
			}

			ସ୍ଥିତି FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				ଯଦି (part.realtime)
					arg.realtime = ସତ୍ୟ;
				args.ଠେଲ_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				ଯଦି (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				ଯଦି (part.justify == FmtPart::ବାମ)
					fmt.str += '-';
				ଯଦି (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				ଭାଙ୍ଗ;
			}

			ପୂର୍ବନିର୍ଦ୍ଧାରିତ: log_abort();
		}
	}

	args.ଭର୍ତ୍ତି(args.begin(), fmt);
	ଫେରନ୍ତୁ args;
}

std::string escape_cxx_string(ଅଚଳ std::string &ନିବେଶ)
{
	std::string output = "\"";
	ପାଇଁ (auto c : ନିବେଶ) {
		ଯଦି (::isprint(c)) {
			ଯଦି (c == '\\' || c == '"')
				output.ଠେଲ_back('\\');
			output.ଠେଲ_back(c);
		} ଅନ୍ୟଥା {
			ଅକ୍ଷର l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.ଠେଲ_back('\\');
			output.ଠେଲ_back('0' + h);
			output.ଠେଲ_back('0' + m);
			output.ଠେଲ_back('0' + l);
		}
	}
	output.ଠେଲ_back('"');
	ଯଦି (output.find('\0') != std::string::npos) {
		output.ଭର୍ତ୍ତି(0, "std::string {");
		output.append(stringf(", %zu}", ନିବେଶ.size()));
	}
	ଫେରନ୍ତୁ output;
}

ଶୂନ୍ୟ Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<ଶୂନ୍ୟ(ଅଚଳ RTLIL::SigSpec &)> emit_sig, ଅଚଳ std::string &context) ଅଚଳ
{
	os << indent << "std::string buf;\n";
	ପାଇଁ (auto &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		ବାଛ (part.type) {
			ସ୍ଥିତି FmtPart::LITERAL:   os << "LITERAL";   ଭାଙ୍ଗ;
			ସ୍ଥିତି FmtPart::INTEGER:   os << "INTEGER";   ଭାଙ୍ଗ;
			ସ୍ଥିତି FmtPart::STRING:    os << "STRING";    ଭାଙ୍ଗ;
			ସ୍ଥିତି FmtPart::UNICHAR:   os << "UNICHAR";   ଭାଙ୍ଗ;
			ସ୍ଥିତି FmtPart::VLOG_TIME: os << "VLOG_TIME"; ଭାଙ୍ଗ;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		ବାଛ (part.justify) {
			ସ୍ଥିତି FmtPart::ବାମ:    os << "LEFT";    ଭାଙ୍ଗ;
			ସ୍ଥିତି FmtPart::ଡାହାଣ:   os << "RIGHT";   ଭାଙ୍ଗ;
			ସ୍ଥିତି FmtPart::NUMERIC: os << "NUMERIC"; ଭାଙ୍ଗ;
		}
		os << ", ";
		os << "(char)" << (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)part.padding << ", ";
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		ବାଛ (part.sign) {
			ସ୍ଥିତି FmtPart::MINUS:       os << "MINUS";       ଭାଙ୍ଗ;
			ସ୍ଥିତି FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  ଭାଙ୍ଗ;
			ସ୍ଥିତି FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; ଭାଙ୍ଗ;
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

std::string Fmt::render() ଅଚଳ
{
	std::string str;

	ପାଇଁ (auto &part : parts) {
		ବାଛ (part.type) {
			ସ୍ଥିତି FmtPart::LITERAL:
				str += part.str;
				ଭାଙ୍ଗ;

			ସ୍ଥିତି FmtPart::UNICHAR: {
				RTLIL::Const ମୂଲ୍ୟ = part.sig.as_const();
				uint32_t codepoint = ମୂଲ୍ୟ.as_int();
				ଯଦି (codepoint >= 0x10000)
					str += (ଅକ୍ଷର)(0xf0 |  (codepoint >> 18));
				ଅନ୍ୟଥା ଯଦି (codepoint >= 0x800)
					str += (ଅକ୍ଷର)(0xe0 |  (codepoint >> 12));
				ଅନ୍ୟଥା ଯଦି (codepoint >= 0x80)
					str += (ଅକ୍ଷର)(0xc0 |  (codepoint >>  6));
				ଅନ୍ୟଥା
					str += (ଅକ୍ଷର)codepoint;
				ଯଦି (codepoint >= 0x10000)
					str += (ଅକ୍ଷର)(0x80 | ((codepoint >> 12) & 0x3f));
				ଯଦି (codepoint >= 0x800)
					str += (ଅକ୍ଷର)(0x80 | ((codepoint >>  6) & 0x3f));
				ଯଦି (codepoint >= 0x80)
					str += (ଅକ୍ଷର)(0x80 | ((codepoint >>  0) & 0x3f));
				ଭାଙ୍ଗ;
			}

			ସ୍ଥିତି FmtPart::INTEGER:
			ସ୍ଥିତି FmtPart::STRING:
			ସ୍ଥିତି FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				ଯଦି (part.type == FmtPart::INTEGER) {
					RTLIL::Const ମୂଲ୍ୟ = part.sig.as_const();
					ତାର୍କିକ has_x = ମିଥ୍ୟା, all_x = ସତ୍ୟ, has_z = ମିଥ୍ୟା, all_z = ସତ୍ୟ;
					ପାଇଁ (State bit : ମୂଲ୍ୟ) {
						ଯଦି (bit == State::Sx)
							has_x = ସତ୍ୟ;
						ଅନ୍ୟଥା
							all_x = ମିଥ୍ୟା;
						ଯଦି (bit == State::Sz)
							has_z = ସତ୍ୟ;
						ଅନ୍ୟଥା
							all_z = ମିଥ୍ୟା;
					}

					ଯଦି (!has_z && !has_x && part.signed_ && ମୂଲ୍ୟ[ମୂଲ୍ୟ.size() - 1]) {
						prefix = "-";
						ମୂଲ୍ୟ = RTLIL::const_neg(ମୂଲ୍ୟ, {}, part.signed_, {}, ମୂଲ୍ୟ.size() + 1);
					} ଅନ୍ୟଥା {
						ବାଛ (part.sign) {
							ସ୍ଥିତି FmtPart::MINUS:       ଭାଙ୍ଗ;
							ସ୍ଥିତି FmtPart::PLUS_MINUS:  prefix = "+"; ଭାଙ୍ଗ;
							ସ୍ଥିତି FmtPart::SPACE_MINUS: prefix = " "; ଭାଙ୍ଗ;
						}
					}

					ଯଦି (part.base != 10) {
						size_t minimum_size = 1;
						ପାଇଁ (size_t index = 0; index < (size_t)ମୂଲ୍ୟ.size(); index++)
							ଯଦି (ମୂଲ୍ୟ[index] != State::S0)
								minimum_size = index + 1;
						ମୂଲ୍ୟ = ମୂଲ୍ୟ.extract(0, minimum_size);
					}

					ଯଦି (part.base == 2) {
						ଯଦି (part.show_base)
							prefix += "0b";
						ପାଇଁ (size_t index = 0; index < (size_t)ମୂଲ୍ୟ.size(); index++) {
							ଯଦି (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = ମୂଲ୍ୟ[index];
							ଯଦି (bit == State::Sx)
								buf += 'x';
							ଅନ୍ୟଥା ଯଦି (bit == State::Sz)
								buf += 'z';
							ଅନ୍ୟଥା ଯଦି (bit == State::S1)
								buf += '1';
							ଅନ୍ୟଥା /* if (bit == State::S0) */
								buf += '0';
						}
					} ଅନ୍ୟଥା ଯଦି (part.base == 8 || part.base == 16) {
						ଯଦି (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						ପାଇଁ (size_t index = 0; index < (size_t)ମୂଲ୍ୟ.size(); index += step) {
							ଯଦି (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = ମୂଲ୍ୟ.extract(index, min(step, ମୂଲ୍ୟ.size() - index));
							ତାର୍କିକ has_x = ମିଥ୍ୟା, all_x = ସତ୍ୟ, has_z = ମିଥ୍ୟା, all_z = ସତ୍ୟ;
							ପାଇଁ (State bit : subvalue) {
								ଯଦି (bit == State::Sx)
									has_x = ସତ୍ୟ;
								ଅନ୍ୟଥା
									all_x = ମିଥ୍ୟା;
								ଯଦି (bit == State::Sz)
									has_z = ସତ୍ୟ;
								ଅନ୍ୟଥା
									all_z = ମିଥ୍ୟା;
							}
							ଯଦି (all_x)
								buf += 'x';
							ଅନ୍ୟଥା ଯଦି (all_z)
								buf += 'z';
							ଅନ୍ୟଥା ଯଦି (has_x)
								buf += 'X';
							ଅନ୍ୟଥା ଯଦି (has_z)
								buf += 'Z';
							ଅନ୍ୟଥା {
								ଅଚଳ ଅକ୍ଷର *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} ଅନ୍ୟଥା ଯଦି (part.base == 10) {
						ଯଦି (part.show_base)
							prefix += "0d";
						ଯଦି (all_x)
							buf += 'x';
						ଅନ୍ୟଥା ଯଦି (all_z)
							buf += 'z';
						ଅନ୍ୟଥା ଯଦି (has_x)
							buf += 'X';
						ଅନ୍ୟଥା ଯଦି (has_z)
							buf += 'Z';
						ଅନ୍ୟଥା {
							log_assert(ମୂଲ୍ୟ.is_fully_def());
							ଯଦି (ମୂଲ୍ୟ.is_fully_zero())
								buf += '0';
							size_t index = 0;
							ଯେତେବେଳେ (!ମୂଲ୍ୟ.is_fully_zero())	{
								ଯଦି (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(ମୂଲ୍ୟ, 10, ମିଥ୍ୟା, ମିଥ୍ୟା, 4).as_int();
								ମୂଲ୍ୟ = RTLIL::const_div(ମୂଲ୍ୟ, 10, ମିଥ୍ୟା, ମିଥ୍ୟା, ମୂଲ୍ୟ.size());
								index++;
							}
						}
					} ଅନ୍ୟଥା log_abort();
					ଯଦି (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						ଯେତେବେଳେ (prefix.size() + buf.size() < part.width) {
							ଯଦି (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} ଅନ୍ୟଥା ଯଦି (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} ଅନ୍ୟଥା ଯଦି (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				ଯଦି (prefix.size() + buf.size() < part.width) {
					size_t pad_width = part.width - prefix.size() - buf.size();
					ବାଛ (part.justify) {
						ସ୍ଥିତି FmtPart::ବାମ:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							ଭାଙ୍ଗ;
						ସ୍ଥିତି FmtPart::ଡାହାଣ:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							ଭାଙ୍ଗ;
						ସ୍ଥିତି FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							ଭାଙ୍ଗ;
					}
				} ଅନ୍ୟଥା {
					str += prefix;
					str += buf;
				}
				ଭାଙ୍ଗ;
			}
		}
	}

	ଫେରନ୍ତୁ str;
}
