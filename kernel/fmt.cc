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

#അന്തര്ഭാവ "libs/bigint/BigUnsigned.hh"
#അന്തര്ഭാവ "kernel/fmt.h"

USING_YOSYS_NAMESPACE

ശൂന്യം Fmt::append_literal(സ്ഥിരം std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.തള്ളുക_back(part);
}

ശൂന്യം Fmt::parse_rtlil(സ്ഥിരം RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.clear();

	FmtPart part;
	വേണ്ടി (size_t i = 0; i < fmt.size(); i++) {
		എങ്കിൽ (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} അല്ലെങ്കിൽ എങ്കിൽ (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == '}') {
			log_assert(അസത്യം && "Unexpected '}' in format string");
		} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == '{') {
			എങ്കിൽ (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.തള്ളുക_back(part);
				part = {};
			}

			എങ്കിൽ (++i == fmt.size())
				log_assert(അസത്യം && "Unexpected end in format substitution");

			size_t arg_size = 0;
			വേണ്ടി (; i < fmt.size(); i++) {
				എങ്കിൽ (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == ':') {
					++i;
					നിർത്തുക;
				} അല്ലെങ്കിൽ {
					log_assert(അസത്യം && "Unexpected character in format substitution");
				}
			}
			എങ്കിൽ (i == fmt.size())
				log_assert(അസത്യം && "Unexpected end in format substitution");

			എങ്കിൽ ((size_t)args.size() < arg_size)
				log_assert(അസത്യം && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			എങ്കിൽ (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				പോകുക success;
			}

			എങ്കിൽ (fmt[i] == '>')
				part.justify = FmtPart::വലത്;
			അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == '<')
				part.justify = FmtPart::ഇടത്;
			അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			അല്ലെങ്കിൽ
				log_assert(അസത്യം && "Unexpected justification in format substitution");
			എങ്കിൽ (++i == fmt.size())
				log_assert(അസത്യം && "Unexpected end in format substitution");

			part.padding = fmt[i];
			എങ്കിൽ (++i == fmt.size())
				log_assert(അസത്യം && "Unexpected end in format substitution");

			വേണ്ടി (; i < fmt.size(); i++) {
				എങ്കിൽ (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					തുടരുക;
				} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = സത്യം;
				} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = സത്യം;
				} അല്ലെങ്കിൽ {
					log_assert(അസത്യം && "Unexpected character in format substitution");
				}
				++i;
				നിർത്തുക;
			}
			എങ്കിൽ (i == fmt.size())
				log_assert(അസത്യം && "Unexpected end in format substitution");

			എങ്കിൽ (part.type == FmtPart::INTEGER) {
				എങ്കിൽ (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					എങ്കിൽ (++i == fmt.size())
						log_assert(അസത്യം && "Unexpected end in format substitution");
				} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					എങ്കിൽ (++i == fmt.size())
						log_assert(അസത്യം && "Unexpected end in format substitution");
				} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					എങ്കിൽ (++i == fmt.size())
						log_assert(അസത്യം && "Unexpected end in format substitution");
				} അല്ലെങ്കിൽ {
					// also accept no sign character and treat like MINUS for compatibility
				}

				എങ്കിൽ (fmt[i] == '#') {
					part.show_base = സത്യം;
					++i;
				}
				എങ്കിൽ (fmt[i] == '_') {
					part.group = സത്യം;
					++i;
				}

				എങ്കിൽ (fmt[i] == 'u')
					part.signed_ = അസത്യം;
				അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 's')
					part.signed_ = സത്യം;
				അല്ലെങ്കിൽ
					log_assert(അസത്യം && "Unexpected character in format substitution");
				എങ്കിൽ (++i == fmt.size())
					log_assert(അസത്യം && "Unexpected end in format substitution");
			}

		success:
			എങ്കിൽ (fmt[i] != '}')
				log_assert(അസത്യം && "Expected '}' after format substitution");

			parts.തള്ളുക_back(part);
			part = {};
		} അല്ലെങ്കിൽ {
			part.str += fmt[i];
		}
	}
	എങ്കിൽ (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.തള്ളുക_back(part);
	}
}

ശൂന്യം Fmt::emit_rtlil(RTLIL::Cell *cell) സ്ഥിരം {
	std::string fmt;
	RTLIL::SigSpec args;

	വേണ്ടി (auto &part : parts) {
		തിരഞ്ഞെടുപ്പ് (part.type) {
			അവസ്ഥ FmtPart::LITERAL:
				വേണ്ടി (അക്ഷരം c : part.str) {
					എങ്കിൽ (c == '{')
						fmt += "{{";
					അല്ലെങ്കിൽ എങ്കിൽ (c == '}')
						fmt += "}}";
					അല്ലെങ്കിൽ
						fmt += c;
				}
				നിർത്തുക;

			അവസ്ഥ FmtPart::UNICHAR:
				log_assert(part.sig.size() <= 32);
				fmt += "{U}";
				നിർത്തുക;

			അവസ്ഥ FmtPart::VLOG_TIME:
				log_assert(part.sig.size() == 0);
				YS_FALLTHROUGH
			അവസ്ഥ FmtPart::STRING:
				log_assert(part.sig.size() % 8 == 0);
				YS_FALLTHROUGH
			അവസ്ഥ FmtPart::INTEGER:
				args.append(part.sig);
				fmt += '{';
				fmt += std::to_string(part.sig.size());
				fmt += ':';
				എങ്കിൽ (part.justify == FmtPart::വലത്)
					fmt += '>';
				അല്ലെങ്കിൽ എങ്കിൽ (part.justify == FmtPart::ഇടത്)
					fmt += '<';
				അല്ലെങ്കിൽ എങ്കിൽ (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				അല്ലെങ്കിൽ log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				എങ്കിൽ (part.width > 0)
					fmt += std::to_string(part.width);
				എങ്കിൽ (part.type == FmtPart::INTEGER) {
					തിരഞ്ഞെടുപ്പ് (part.base) {
						അവസ്ഥ  2: fmt += 'b'; നിർത്തുക;
						അവസ്ഥ  8: fmt += 'o'; നിർത്തുക;
						അവസ്ഥ 10: fmt += 'd'; നിർത്തുക;
						അവസ്ഥ 16: fmt += part.hex_upper ? 'H' : 'h'; നിർത്തുക;
						സ്ഥിരസ്ഥിതി: log_abort();
					}
					തിരഞ്ഞെടുപ്പ് (part.sign) {
						അവസ്ഥ FmtPart::MINUS:       fmt += '-'; നിർത്തുക;
						അവസ്ഥ FmtPart::PLUS_MINUS:  fmt += '+'; നിർത്തുക;
						അവസ്ഥ FmtPart::SPACE_MINUS: fmt += ' '; നിർത്തുക;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} അല്ലെങ്കിൽ എങ്കിൽ (part.type == FmtPart::STRING) {
					fmt += 'c';
				} അല്ലെങ്കിൽ എങ്കിൽ (part.type == FmtPart::VLOG_TIME) {
					എങ്കിൽ (part.realtime)
						fmt += 'r';
					അല്ലെങ്കിൽ
						fmt += 't';
				} അല്ലെങ്കിൽ log_abort();
				fmt += '}';
				നിർത്തുക;

			സ്ഥിരസ്ഥിതി: log_abort();
		}
	}

	cell->setParam(ID(FORMAT), fmt);
	cell->setParam(ID(ARGS_WIDTH), args.size());
	cell->setPort(ID(ARGS), args);
}

സ്ഥിതം size_t compute_required_decimal_places(size_t size, തർക്കം signed_)
{
	BigUnsigned max;
	എങ്കിൽ (!signed_)
		max.setBit(size, സത്യം);
	അല്ലെങ്കിൽ
		max.setBit(size - 1, സത്യം);
	size_t places = 0;
	ആകുമ്പോൾ (!max.isZero()) {
		places++;
		max /= 10;
	}
	എങ്കിൽ (signed_)
		places++;
	തിരികെ places;
}

സ്ഥിതം size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, സത്യം);
	size_t places = 0;
	ആകുമ്പോൾ (!max.isZero()) {
		places++;
		max /= base;
	}
	തിരികെ places;
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
ശൂന്യം Fmt::apply_verilog_automatic_sizing_and_add(FmtPart &part)
{
	എങ്കിൽ (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.തള്ളുക_back(part);
		തിരികെ;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	എങ്കിൽ (part.width < places) {
		part.justify = FmtPart::വലത്;
		part.width = places;
		parts.തള്ളുക_back(part);
	} അല്ലെങ്കിൽ എങ്കിൽ (part.width == places) {
		parts.തള്ളുക_back(part);
	} അല്ലെങ്കിൽ എങ്കിൽ (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		എങ്കിൽ (part.justify == FmtPart::വലത്) {
			append_literal(gap);
			parts.തള്ളുക_back(part);
		} അല്ലെങ്കിൽ {
			part.justify = FmtPart::വലത്;
			parts.തള്ളുക_back(part);
			append_literal(gap);
		}
	}
}

ശൂന്യം Fmt::parse_verilog(സ്ഥിരം std::vector<VerilogFmtArg> &args, തർക്കം sformat_like, പൂർണ്ണസംഖ്യ default_base, സ്ഥിരം std::string &task_name, സ്ഥിരം std::string &module_name)
{
	parts.clear();

	auto arg = args.begin();
	വേണ്ടി (; arg != args.end(); ++arg) {
		തിരഞ്ഞെടുപ്പ് (arg->type) {
			അവസ്ഥ VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				നിർത്തുക;
			}

			അവസ്ഥ VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.തള്ളുക_back(part);
				നിർത്തുക;
			}

			അവസ്ഥ VerilogFmtArg::STRING: {
				എങ്കിൽ (arg == args.begin() || !sformat_like) {
					സ്ഥിരം auto fmtarg = arg;
					സ്ഥിരം std::string &fmt = fmtarg->str;
					FmtPart part = {};
					വേണ്ടി (size_t i = 0; i < fmt.size(); i++) {
						എങ്കിൽ (fmt[i] != '%') {
							part.str += fmt[i];
						} അല്ലെങ്കിൽ എങ്കിൽ (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} അല്ലെങ്കിൽ എങ്കിൽ (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} അല്ലെങ്കിൽ എങ്കിൽ (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} അല്ലെങ്കിൽ {
							എങ്കിൽ (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.തള്ളുക_back(part);
								part = {};
							}
							എങ്കിൽ (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							എങ്കിൽ (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							വേണ്ടി (; i < fmt.size(); i++) {
								എങ്കിൽ (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::ഇടത്;
								} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} അല്ലെങ്കിൽ നിർത്തുക;
							}
							എങ്കിൽ (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							തർക്കം has_leading_zero = അസത്യം, has_width = അസത്യം;
							വേണ്ടി (; i < fmt.size(); i++) {
								എങ്കിൽ (fmt[i] >= '0' && fmt[i] <= '9') {
									എങ്കിൽ (fmt[i] == '0' && !has_width) {
										has_leading_zero = സത്യം;
									} അല്ലെങ്കിൽ {
										has_width = സത്യം;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									തുടരുക;
								} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									എങ്കിൽ ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} അല്ലെങ്കിൽ എങ്കിൽ (fmt[i] == 't' || fmt[i] == 'T') {
									എങ്കിൽ (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										എങ്കിൽ (!has_width && !has_leading_zero)
											part.width = 20;
									} അല്ലെങ്കിൽ {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} അല്ലെങ്കിൽ {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								നിർത്തുക;
							}
							എങ്കിൽ (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							എങ്കിൽ (part.padding == '\0') {
								എങ്കിൽ (has_leading_zero && part.justify == FmtPart::വലത്) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} അല്ലെങ്കിൽ {
									part.padding = ' ';
								}
							}

							എങ്കിൽ (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							എങ്കിൽ (part.base != 10)
								part.signed_ = അസത്യം;
							എങ്കിൽ (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							അല്ലെങ്കിൽ
								parts.തള്ളുക_back(part);
							part = {};
						}
					}
					എങ്കിൽ (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.തള്ളുക_back(part);
					}
				} അല്ലെങ്കിൽ {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.തള്ളുക_back(part);
				}
				നിർത്തുക;
			}

			സ്ഥിരസ്ഥിതി: log_abort();
		}
	}
}

std::vector<VerilogFmtArg> Fmt::emit_verilog() സ്ഥിരം
{
	std::vector<VerilogFmtArg> args;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	വേണ്ടി (auto &part : parts) {
		തിരഞ്ഞെടുപ്പ് (part.type) {
			അവസ്ഥ FmtPart::LITERAL:
				വേണ്ടി (അക്ഷരം c : part.str) {
					എങ്കിൽ (c == '%')
						fmt.str += "%%";
					അല്ലെങ്കിൽ
						fmt.str += c;
				}
				നിർത്തുക;

			അവസ്ഥ FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.തള്ളുക_back(arg);

				fmt.str += '%';
				എങ്കിൽ (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				എങ്കിൽ (part.justify == FmtPart::ഇടത്)
					fmt.str += '-';
				എങ്കിൽ (part.width == 0) {
					fmt.str += '0';
				} അല്ലെങ്കിൽ എങ്കിൽ (part.width > 0) {
					എങ്കിൽ (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				തിരഞ്ഞെടുപ്പ് (part.base) {
					അവസ്ഥ  2: fmt.str += 'b'; നിർത്തുക;
					അവസ്ഥ  8: fmt.str += 'o'; നിർത്തുക;
					അവസ്ഥ 10: fmt.str += 'd'; നിർത്തുക;
					അവസ്ഥ 16: fmt.str += 'h'; നിർത്തുക; // treat uppercase hex as lowercase
					സ്ഥിരസ്ഥിതി: log_abort();
				}
				നിർത്തുക;
			}

			അവസ്ഥ FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				args.തള്ളുക_back(arg);

				fmt.str += '%';
				എങ്കിൽ (part.justify == FmtPart::ഇടത്)
					fmt.str += '-';
				എങ്കിൽ (part.sig.size() == 8) {
					എങ്കിൽ (part.width > 0) {
						എങ്കിൽ (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} അല്ലെങ്കിൽ {
					log_assert(part.sig.size() % 8 == 0);
					എങ്കിൽ (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				നിർത്തുക;
			}

			അവസ്ഥ FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.തള്ളുക_back(arg);

				fmt.str += "%c";
				നിർത്തുക;
			}

			അവസ്ഥ FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				എങ്കിൽ (part.realtime)
					arg.realtime = സത്യം;
				args.തള്ളുക_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				എങ്കിൽ (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				എങ്കിൽ (part.justify == FmtPart::ഇടത്)
					fmt.str += '-';
				എങ്കിൽ (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				നിർത്തുക;
			}

			സ്ഥിരസ്ഥിതി: log_abort();
		}
	}

	args.തിരുകുക(args.begin(), fmt);
	തിരികെ args;
}

std::string escape_cxx_string(സ്ഥിരം std::string &നിവേശം)
{
	std::string output = "\"";
	വേണ്ടി (auto c : നിവേശം) {
		എങ്കിൽ (::isprint(c)) {
			എങ്കിൽ (c == '\\' || c == '"')
				output.തള്ളുക_back('\\');
			output.തള്ളുക_back(c);
		} അല്ലെങ്കിൽ {
			അക്ഷരം l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.തള്ളുക_back('\\');
			output.തള്ളുക_back('0' + h);
			output.തള്ളുക_back('0' + m);
			output.തള്ളുക_back('0' + l);
		}
	}
	output.തള്ളുക_back('"');
	എങ്കിൽ (output.find('\0') != std::string::npos) {
		output.തിരുകുക(0, "std::string {");
		output.append(stringf(", %zu}", നിവേശം.size()));
	}
	തിരികെ output;
}

ശൂന്യം Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<ശൂന്യം(സ്ഥിരം RTLIL::SigSpec &)> emit_sig, സ്ഥിരം std::string &context) സ്ഥിരം
{
	os << indent << "std::string buf;\n";
	വേണ്ടി (auto &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		തിരഞ്ഞെടുപ്പ് (part.type) {
			അവസ്ഥ FmtPart::LITERAL:   os << "LITERAL";   നിർത്തുക;
			അവസ്ഥ FmtPart::INTEGER:   os << "INTEGER";   നിർത്തുക;
			അവസ്ഥ FmtPart::STRING:    os << "STRING";    നിർത്തുക;
			അവസ്ഥ FmtPart::UNICHAR:   os << "UNICHAR";   നിർത്തുക;
			അവസ്ഥ FmtPart::VLOG_TIME: os << "VLOG_TIME"; നിർത്തുക;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		തിരഞ്ഞെടുപ്പ് (part.justify) {
			അവസ്ഥ FmtPart::ഇടത്:    os << "LEFT";    നിർത്തുക;
			അവസ്ഥ FmtPart::വലത്:   os << "RIGHT";   നിർത്തുക;
			അവസ്ഥ FmtPart::NUMERIC: os << "NUMERIC"; നിർത്തുക;
		}
		os << ", ";
		os << "(char)" << (പൂർണ്ണസംഖ്യ)part.padding << ", ";
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		തിരഞ്ഞെടുപ്പ് (part.sign) {
			അവസ്ഥ FmtPart::MINUS:       os << "MINUS";       നിർത്തുക;
			അവസ്ഥ FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  നിർത്തുക;
			അവസ്ഥ FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; നിർത്തുക;
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

std::string Fmt::render() സ്ഥിരം
{
	std::string str;

	വേണ്ടി (auto &part : parts) {
		തിരഞ്ഞെടുപ്പ് (part.type) {
			അവസ്ഥ FmtPart::LITERAL:
				str += part.str;
				നിർത്തുക;

			അവസ്ഥ FmtPart::UNICHAR: {
				RTLIL::Const മൂല്യം = part.sig.as_const();
				uint32_t codepoint = മൂല്യം.as_int();
				എങ്കിൽ (codepoint >= 0x10000)
					str += (അക്ഷരം)(0xf0 |  (codepoint >> 18));
				അല്ലെങ്കിൽ എങ്കിൽ (codepoint >= 0x800)
					str += (അക്ഷരം)(0xe0 |  (codepoint >> 12));
				അല്ലെങ്കിൽ എങ്കിൽ (codepoint >= 0x80)
					str += (അക്ഷരം)(0xc0 |  (codepoint >>  6));
				അല്ലെങ്കിൽ
					str += (അക്ഷരം)codepoint;
				എങ്കിൽ (codepoint >= 0x10000)
					str += (അക്ഷരം)(0x80 | ((codepoint >> 12) & 0x3f));
				എങ്കിൽ (codepoint >= 0x800)
					str += (അക്ഷരം)(0x80 | ((codepoint >>  6) & 0x3f));
				എങ്കിൽ (codepoint >= 0x80)
					str += (അക്ഷരം)(0x80 | ((codepoint >>  0) & 0x3f));
				നിർത്തുക;
			}

			അവസ്ഥ FmtPart::INTEGER:
			അവസ്ഥ FmtPart::STRING:
			അവസ്ഥ FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				എങ്കിൽ (part.type == FmtPart::INTEGER) {
					RTLIL::Const മൂല്യം = part.sig.as_const();
					തർക്കം has_x = അസത്യം, all_x = സത്യം, has_z = അസത്യം, all_z = സത്യം;
					വേണ്ടി (State bit : മൂല്യം) {
						എങ്കിൽ (bit == State::Sx)
							has_x = സത്യം;
						അല്ലെങ്കിൽ
							all_x = അസത്യം;
						എങ്കിൽ (bit == State::Sz)
							has_z = സത്യം;
						അല്ലെങ്കിൽ
							all_z = അസത്യം;
					}

					എങ്കിൽ (!has_z && !has_x && part.signed_ && മൂല്യം[മൂല്യം.size() - 1]) {
						prefix = "-";
						മൂല്യം = RTLIL::const_neg(മൂല്യം, {}, part.signed_, {}, മൂല്യം.size() + 1);
					} അല്ലെങ്കിൽ {
						തിരഞ്ഞെടുപ്പ് (part.sign) {
							അവസ്ഥ FmtPart::MINUS:       നിർത്തുക;
							അവസ്ഥ FmtPart::PLUS_MINUS:  prefix = "+"; നിർത്തുക;
							അവസ്ഥ FmtPart::SPACE_MINUS: prefix = " "; നിർത്തുക;
						}
					}

					എങ്കിൽ (part.base != 10) {
						size_t minimum_size = 1;
						വേണ്ടി (size_t index = 0; index < (size_t)മൂല്യം.size(); index++)
							എങ്കിൽ (മൂല്യം[index] != State::S0)
								minimum_size = index + 1;
						മൂല്യം = മൂല്യം.extract(0, minimum_size);
					}

					എങ്കിൽ (part.base == 2) {
						എങ്കിൽ (part.show_base)
							prefix += "0b";
						വേണ്ടി (size_t index = 0; index < (size_t)മൂല്യം.size(); index++) {
							എങ്കിൽ (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = മൂല്യം[index];
							എങ്കിൽ (bit == State::Sx)
								buf += 'x';
							അല്ലെങ്കിൽ എങ്കിൽ (bit == State::Sz)
								buf += 'z';
							അല്ലെങ്കിൽ എങ്കിൽ (bit == State::S1)
								buf += '1';
							അല്ലെങ്കിൽ /* if (bit == State::S0) */
								buf += '0';
						}
					} അല്ലെങ്കിൽ എങ്കിൽ (part.base == 8 || part.base == 16) {
						എങ്കിൽ (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						വേണ്ടി (size_t index = 0; index < (size_t)മൂല്യം.size(); index += step) {
							എങ്കിൽ (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = മൂല്യം.extract(index, min(step, മൂല്യം.size() - index));
							തർക്കം has_x = അസത്യം, all_x = സത്യം, has_z = അസത്യം, all_z = സത്യം;
							വേണ്ടി (State bit : subvalue) {
								എങ്കിൽ (bit == State::Sx)
									has_x = സത്യം;
								അല്ലെങ്കിൽ
									all_x = അസത്യം;
								എങ്കിൽ (bit == State::Sz)
									has_z = സത്യം;
								അല്ലെങ്കിൽ
									all_z = അസത്യം;
							}
							എങ്കിൽ (all_x)
								buf += 'x';
							അല്ലെങ്കിൽ എങ്കിൽ (all_z)
								buf += 'z';
							അല്ലെങ്കിൽ എങ്കിൽ (has_x)
								buf += 'X';
							അല്ലെങ്കിൽ എങ്കിൽ (has_z)
								buf += 'Z';
							അല്ലെങ്കിൽ {
								സ്ഥിരം അക്ഷരം *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} അല്ലെങ്കിൽ എങ്കിൽ (part.base == 10) {
						എങ്കിൽ (part.show_base)
							prefix += "0d";
						എങ്കിൽ (all_x)
							buf += 'x';
						അല്ലെങ്കിൽ എങ്കിൽ (all_z)
							buf += 'z';
						അല്ലെങ്കിൽ എങ്കിൽ (has_x)
							buf += 'X';
						അല്ലെങ്കിൽ എങ്കിൽ (has_z)
							buf += 'Z';
						അല്ലെങ്കിൽ {
							log_assert(മൂല്യം.is_fully_def());
							എങ്കിൽ (മൂല്യം.is_fully_zero())
								buf += '0';
							size_t index = 0;
							ആകുമ്പോൾ (!മൂല്യം.is_fully_zero())	{
								എങ്കിൽ (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(മൂല്യം, 10, അസത്യം, അസത്യം, 4).as_int();
								മൂല്യം = RTLIL::const_div(മൂല്യം, 10, അസത്യം, അസത്യം, മൂല്യം.size());
								index++;
							}
						}
					} അല്ലെങ്കിൽ log_abort();
					എങ്കിൽ (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						ആകുമ്പോൾ (prefix.size() + buf.size() < part.width) {
							എങ്കിൽ (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} അല്ലെങ്കിൽ എങ്കിൽ (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} അല്ലെങ്കിൽ എങ്കിൽ (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				എങ്കിൽ (prefix.size() + buf.size() < part.width) {
					size_t pad_width = part.width - prefix.size() - buf.size();
					തിരഞ്ഞെടുപ്പ് (part.justify) {
						അവസ്ഥ FmtPart::ഇടത്:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							നിർത്തുക;
						അവസ്ഥ FmtPart::വലത്:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							നിർത്തുക;
						അവസ്ഥ FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							നിർത്തുക;
					}
				} അല്ലെങ്കിൽ {
					str += prefix;
					str += buf;
				}
				നിർത്തുക;
			}
		}
	}

	തിരികെ str;
}
