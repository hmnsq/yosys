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

#ضمن "libs/bigint/BigUnsigned.hh"
#ضمن "kernel/fmt.h"

USING_YOSYS_NAMESPACE

فراغ Fmt::append_literal(ثابت std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.push_back(part);
}

فراغ Fmt::parse_rtlil(ثابت RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.clear();

	FmtPart part;
	لكل (size_t i = 0; i < fmt.size(); i++) {
		ܐܢ (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} ܐܚܪܢܐ ܐܢ (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} ܐܚܪܢܐ ܐܢ (fmt[i] == '}') {
			log_assert(ܕܓܠܐ && "Unexpected '}' in format string");
		} ܐܚܪܢܐ ܐܢ (fmt[i] == '{') {
			ܐܢ (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.push_back(part);
				part = {};
			}

			ܐܢ (++i == fmt.size())
				log_assert(ܕܓܠܐ && "Unexpected end in format substitution");

			size_t arg_size = 0;
			لكل (; i < fmt.size(); i++) {
				ܐܢ (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} ܐܚܪܢܐ ܐܢ (fmt[i] == ':') {
					++i;
					اكسر;
				} ܐܚܪܢܐ {
					log_assert(ܕܓܠܐ && "Unexpected character in format substitution");
				}
			}
			ܐܢ (i == fmt.size())
				log_assert(ܕܓܠܐ && "Unexpected end in format substitution");

			ܐܢ ((size_t)args.size() < arg_size)
				log_assert(ܕܓܠܐ && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			ܐܢ (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				اذهب success;
			}

			ܐܢ (fmt[i] == '>')
				part.justify = FmtPart::RIGHT;
			ܐܚܪܢܐ ܐܢ (fmt[i] == '<')
				part.justify = FmtPart::LEFT;
			ܐܚܪܢܐ ܐܢ (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			ܐܚܪܢܐ
				log_assert(ܕܓܠܐ && "Unexpected justification in format substitution");
			ܐܢ (++i == fmt.size())
				log_assert(ܕܓܠܐ && "Unexpected end in format substitution");

			part.padding = fmt[i];
			ܐܢ (++i == fmt.size())
				log_assert(ܕܓܠܐ && "Unexpected end in format substitution");

			لكل (; i < fmt.size(); i++) {
				ܐܢ (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					استمر;
				} ܐܚܪܢܐ ܐܢ (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} ܐܚܪܢܐ ܐܢ (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} ܐܚܪܢܐ ܐܢ (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} ܐܚܪܢܐ ܐܢ (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} ܐܚܪܢܐ ܐܢ (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = ܫܪܪܐ;
				} ܐܚܪܢܐ ܐܢ (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} ܐܚܪܢܐ ܐܢ (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} ܐܚܪܢܐ ܐܢ (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = ܫܪܪܐ;
				} ܐܚܪܢܐ {
					log_assert(ܕܓܠܐ && "Unexpected character in format substitution");
				}
				++i;
				اكسر;
			}
			ܐܢ (i == fmt.size())
				log_assert(ܕܓܠܐ && "Unexpected end in format substitution");

			ܐܢ (part.type == FmtPart::INTEGER) {
				ܐܢ (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					ܐܢ (++i == fmt.size())
						log_assert(ܕܓܠܐ && "Unexpected end in format substitution");
				} ܐܚܪܢܐ ܐܢ (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					ܐܢ (++i == fmt.size())
						log_assert(ܕܓܠܐ && "Unexpected end in format substitution");
				} ܐܚܪܢܐ ܐܢ (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					ܐܢ (++i == fmt.size())
						log_assert(ܕܓܠܐ && "Unexpected end in format substitution");
				} ܐܚܪܢܐ {
					// also accept no sign character and treat like MINUS for compatibility
				}

				ܐܢ (fmt[i] == '#') {
					part.show_base = ܫܪܪܐ;
					++i;
				}
				ܐܢ (fmt[i] == '_') {
					part.group = ܫܪܪܐ;
					++i;
				}

				ܐܢ (fmt[i] == 'u')
					part.signed_ = ܕܓܠܐ;
				ܐܚܪܢܐ ܐܢ (fmt[i] == 's')
					part.signed_ = ܫܪܪܐ;
				ܐܚܪܢܐ
					log_assert(ܕܓܠܐ && "Unexpected character in format substitution");
				ܐܢ (++i == fmt.size())
					log_assert(ܕܓܠܐ && "Unexpected end in format substitution");
			}

		success:
			ܐܢ (fmt[i] != '}')
				log_assert(ܕܓܠܐ && "Expected '}' after format substitution");

			parts.push_back(part);
			part = {};
		} ܐܚܪܢܐ {
			part.str += fmt[i];
		}
	}
	ܐܢ (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.push_back(part);
	}
}

فراغ Fmt::emit_rtlil(RTLIL::Cell *cell) ثابت {
	std::string fmt;
	RTLIL::SigSpec args;

	لكل (auto &part : parts) {
		بدل (part.type) {
			حالة FmtPart::LITERAL:
				لكل (حرف c : part.str) {
					ܐܢ (c == '{')
						fmt += "{{";
					ܐܚܪܢܐ ܐܢ (c == '}')
						fmt += "}}";
					ܐܚܪܢܐ
						fmt += c;
				}
				اكسر;

			حالة FmtPart::UNICHAR:
				log_assert(part.sig.size() <= 32);
				fmt += "{U}";
				اكسر;

			حالة FmtPart::VLOG_TIME:
				log_assert(part.sig.size() == 0);
				YS_FALLTHROUGH
			حالة FmtPart::STRING:
				log_assert(part.sig.size() % 8 == 0);
				YS_FALLTHROUGH
			حالة FmtPart::INTEGER:
				args.append(part.sig);
				fmt += '{';
				fmt += std::to_string(part.sig.size());
				fmt += ':';
				ܐܢ (part.justify == FmtPart::RIGHT)
					fmt += '>';
				ܐܚܪܢܐ ܐܢ (part.justify == FmtPart::LEFT)
					fmt += '<';
				ܐܚܪܢܐ ܐܢ (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				ܐܚܪܢܐ log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				ܐܢ (part.width > 0)
					fmt += std::to_string(part.width);
				ܐܢ (part.type == FmtPart::INTEGER) {
					بدل (part.base) {
						حالة  2: fmt += 'b'; اكسر;
						حالة  8: fmt += 'o'; اكسر;
						حالة 10: fmt += 'd'; اكسر;
						حالة 16: fmt += part.hex_upper ? 'H' : 'h'; اكسر;
						افتراضي: log_abort();
					}
					بدل (part.sign) {
						حالة FmtPart::MINUS:       fmt += '-'; اكسر;
						حالة FmtPart::PLUS_MINUS:  fmt += '+'; اكسر;
						حالة FmtPart::SPACE_MINUS: fmt += ' '; اكسر;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} ܐܚܪܢܐ ܐܢ (part.type == FmtPart::STRING) {
					fmt += 'c';
				} ܐܚܪܢܐ ܐܢ (part.type == FmtPart::VLOG_TIME) {
					ܐܢ (part.realtime)
						fmt += 'r';
					ܐܚܪܢܐ
						fmt += 't';
				} ܐܚܪܢܐ log_abort();
				fmt += '}';
				اكسر;

			افتراضي: log_abort();
		}
	}

	cell->setParam(ID(FORMAT), fmt);
	cell->setParam(ID(ARGS_WIDTH), args.size());
	cell->setPort(ID(ARGS), args);
}

ساكن size_t compute_required_decimal_places(size_t size, منطقي signed_)
{
	BigUnsigned max;
	ܐܢ (!signed_)
		max.setBit(size, ܫܪܪܐ);
	ܐܚܪܢܐ
		max.setBit(size - 1, ܫܪܪܐ);
	size_t places = 0;
	ܟܡܐ (!max.isZero()) {
		places++;
		max /= 10;
	}
	ܐܢ (signed_)
		places++;
	ܦܢܐ places;
}

ساكن size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, ܫܪܪܐ);
	size_t places = 0;
	ܟܡܐ (!max.isZero()) {
		places++;
		max /= base;
	}
	ܦܢܐ places;
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
فراغ Fmt::apply_verilog_automatic_sizing_and_add(FmtPart &part)
{
	ܐܢ (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.push_back(part);
		ܦܢܐ;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	ܐܢ (part.width < places) {
		part.justify = FmtPart::RIGHT;
		part.width = places;
		parts.push_back(part);
	} ܐܚܪܢܐ ܐܢ (part.width == places) {
		parts.push_back(part);
	} ܐܚܪܢܐ ܐܢ (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		ܐܢ (part.justify == FmtPart::RIGHT) {
			append_literal(gap);
			parts.push_back(part);
		} ܐܚܪܢܐ {
			part.justify = FmtPart::RIGHT;
			parts.push_back(part);
			append_literal(gap);
		}
	}
}

فراغ Fmt::parse_verilog(ثابت std::vector<VerilogFmtArg> &args, منطقي sformat_like, صحيح default_base, ثابت std::string &task_name, ثابت std::string &module_name)
{
	parts.clear();

	auto arg = args.begin();
	لكل (; arg != args.end(); ++arg) {
		بدل (arg->type) {
			حالة VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				اكسر;
			}

			حالة VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.push_back(part);
				اكسر;
			}

			حالة VerilogFmtArg::STRING: {
				ܐܢ (arg == args.begin() || !sformat_like) {
					ثابت auto fmtarg = arg;
					ثابت std::string &fmt = fmtarg->str;
					FmtPart part = {};
					لكل (size_t i = 0; i < fmt.size(); i++) {
						ܐܢ (fmt[i] != '%') {
							part.str += fmt[i];
						} ܐܚܪܢܐ ܐܢ (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} ܐܚܪܢܐ ܐܢ (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} ܐܚܪܢܐ ܐܢ (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} ܐܚܪܢܐ {
							ܐܢ (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.push_back(part);
								part = {};
							}
							ܐܢ (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ܐܢ (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							لكل (; i < fmt.size(); i++) {
								ܐܢ (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::LEFT;
								} ܐܚܪܢܐ ܐܢ (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} ܐܚܪܢܐ اكسر;
							}
							ܐܢ (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							منطقي has_leading_zero = ܕܓܠܐ, has_width = ܕܓܠܐ;
							لكل (; i < fmt.size(); i++) {
								ܐܢ (fmt[i] >= '0' && fmt[i] <= '9') {
									ܐܢ (fmt[i] == '0' && !has_width) {
										has_leading_zero = ܫܪܪܐ;
									} ܐܚܪܢܐ {
										has_width = ܫܪܪܐ;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									استمر;
								} ܐܚܪܢܐ ܐܢ (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} ܐܚܪܢܐ ܐܢ (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} ܐܚܪܢܐ ܐܢ (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} ܐܚܪܢܐ ܐܢ (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} ܐܚܪܢܐ ܐܢ (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} ܐܚܪܢܐ ܐܢ (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									ܐܢ ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} ܐܚܪܢܐ ܐܢ (fmt[i] == 't' || fmt[i] == 'T') {
									ܐܢ (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										ܐܢ (!has_width && !has_leading_zero)
											part.width = 20;
									} ܐܚܪܢܐ {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} ܐܚܪܢܐ {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								اكسر;
							}
							ܐܢ (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ܐܢ (part.padding == '\0') {
								ܐܢ (has_leading_zero && part.justify == FmtPart::RIGHT) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} ܐܚܪܢܐ {
									part.padding = ' ';
								}
							}

							ܐܢ (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							ܐܢ (part.base != 10)
								part.signed_ = ܕܓܠܐ;
							ܐܢ (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							ܐܚܪܢܐ
								parts.push_back(part);
							part = {};
						}
					}
					ܐܢ (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.push_back(part);
					}
				} ܐܚܪܢܐ {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.push_back(part);
				}
				اكسر;
			}

			افتراضي: log_abort();
		}
	}
}

std::vector<VerilogFmtArg> Fmt::emit_verilog() ثابت
{
	std::vector<VerilogFmtArg> args;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	لكل (auto &part : parts) {
		بدل (part.type) {
			حالة FmtPart::LITERAL:
				لكل (حرف c : part.str) {
					ܐܢ (c == '%')
						fmt.str += "%%";
					ܐܚܪܢܐ
						fmt.str += c;
				}
				اكسر;

			حالة FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.push_back(arg);

				fmt.str += '%';
				ܐܢ (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				ܐܢ (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				ܐܢ (part.width == 0) {
					fmt.str += '0';
				} ܐܚܪܢܐ ܐܢ (part.width > 0) {
					ܐܢ (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				بدل (part.base) {
					حالة  2: fmt.str += 'b'; اكسر;
					حالة  8: fmt.str += 'o'; اكسر;
					حالة 10: fmt.str += 'd'; اكسر;
					حالة 16: fmt.str += 'h'; اكسر; // treat uppercase hex as lowercase
					افتراضي: log_abort();
				}
				اكسر;
			}

			حالة FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				args.push_back(arg);

				fmt.str += '%';
				ܐܢ (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				ܐܢ (part.sig.size() == 8) {
					ܐܢ (part.width > 0) {
						ܐܢ (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} ܐܚܪܢܐ {
					log_assert(part.sig.size() % 8 == 0);
					ܐܢ (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				اكسر;
			}

			حالة FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.push_back(arg);

				fmt.str += "%c";
				اكسر;
			}

			حالة FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				ܐܢ (part.realtime)
					arg.realtime = ܫܪܪܐ;
				args.push_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				ܐܢ (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				ܐܢ (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				ܐܢ (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				اكسر;
			}

			افتراضي: log_abort();
		}
	}

	args.insert(args.begin(), fmt);
	ܦܢܐ args;
}

std::string escape_cxx_string(ثابت std::string &input)
{
	std::string output = "\"";
	لكل (auto c : input) {
		ܐܢ (::isprint(c)) {
			ܐܢ (c == '\\' || c == '"')
				output.push_back('\\');
			output.push_back(c);
		} ܐܚܪܢܐ {
			حرف l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.push_back('\\');
			output.push_back('0' + h);
			output.push_back('0' + m);
			output.push_back('0' + l);
		}
	}
	output.push_back('"');
	ܐܢ (output.find('\0') != std::string::npos) {
		output.insert(0, "std::string {");
		output.append(stringf(", %zu}", input.size()));
	}
	ܦܢܐ output;
}

فراغ Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<فراغ(ثابت RTLIL::SigSpec &)> emit_sig, ثابت std::string &context) ثابت
{
	os << indent << "std::string buf;\n";
	لكل (auto &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		بدل (part.type) {
			حالة FmtPart::LITERAL:   os << "LITERAL";   اكسر;
			حالة FmtPart::INTEGER:   os << "INTEGER";   اكسر;
			حالة FmtPart::STRING:    os << "STRING";    اكسر;
			حالة FmtPart::UNICHAR:   os << "UNICHAR";   اكسر;
			حالة FmtPart::VLOG_TIME: os << "VLOG_TIME"; اكسر;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		بدل (part.justify) {
			حالة FmtPart::LEFT:    os << "LEFT";    اكسر;
			حالة FmtPart::RIGHT:   os << "RIGHT";   اكسر;
			حالة FmtPart::NUMERIC: os << "NUMERIC"; اكسر;
		}
		os << ", ";
		os << "(char)" << (صحيح)part.padding << ", ";
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		بدل (part.sign) {
			حالة FmtPart::MINUS:       os << "MINUS";       اكسر;
			حالة FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  اكسر;
			حالة FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; اكسر;
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

std::string Fmt::render() ثابت
{
	std::string str;

	لكل (auto &part : parts) {
		بدل (part.type) {
			حالة FmtPart::LITERAL:
				str += part.str;
				اكسر;

			حالة FmtPart::UNICHAR: {
				RTLIL::Const value = part.sig.as_const();
				uint32_t codepoint = value.as_int();
				ܐܢ (codepoint >= 0x10000)
					str += (حرف)(0xf0 |  (codepoint >> 18));
				ܐܚܪܢܐ ܐܢ (codepoint >= 0x800)
					str += (حرف)(0xe0 |  (codepoint >> 12));
				ܐܚܪܢܐ ܐܢ (codepoint >= 0x80)
					str += (حرف)(0xc0 |  (codepoint >>  6));
				ܐܚܪܢܐ
					str += (حرف)codepoint;
				ܐܢ (codepoint >= 0x10000)
					str += (حرف)(0x80 | ((codepoint >> 12) & 0x3f));
				ܐܢ (codepoint >= 0x800)
					str += (حرف)(0x80 | ((codepoint >>  6) & 0x3f));
				ܐܢ (codepoint >= 0x80)
					str += (حرف)(0x80 | ((codepoint >>  0) & 0x3f));
				اكسر;
			}

			حالة FmtPart::INTEGER:
			حالة FmtPart::STRING:
			حالة FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				ܐܢ (part.type == FmtPart::INTEGER) {
					RTLIL::Const value = part.sig.as_const();
					منطقي has_x = ܕܓܠܐ, all_x = ܫܪܪܐ, has_z = ܕܓܠܐ, all_z = ܫܪܪܐ;
					لكل (State bit : value) {
						ܐܢ (bit == State::Sx)
							has_x = ܫܪܪܐ;
						ܐܚܪܢܐ
							all_x = ܕܓܠܐ;
						ܐܢ (bit == State::Sz)
							has_z = ܫܪܪܐ;
						ܐܚܪܢܐ
							all_z = ܕܓܠܐ;
					}

					ܐܢ (!has_z && !has_x && part.signed_ && value[value.size() - 1]) {
						prefix = "-";
						value = RTLIL::const_neg(value, {}, part.signed_, {}, value.size() + 1);
					} ܐܚܪܢܐ {
						بدل (part.sign) {
							حالة FmtPart::MINUS:       اكسر;
							حالة FmtPart::PLUS_MINUS:  prefix = "+"; اكسر;
							حالة FmtPart::SPACE_MINUS: prefix = " "; اكسر;
						}
					}

					ܐܢ (part.base != 10) {
						size_t minimum_size = 1;
						لكل (size_t index = 0; index < (size_t)value.size(); index++)
							ܐܢ (value[index] != State::S0)
								minimum_size = index + 1;
						value = value.extract(0, minimum_size);
					}

					ܐܢ (part.base == 2) {
						ܐܢ (part.show_base)
							prefix += "0b";
						لكل (size_t index = 0; index < (size_t)value.size(); index++) {
							ܐܢ (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = value[index];
							ܐܢ (bit == State::Sx)
								buf += 'x';
							ܐܚܪܢܐ ܐܢ (bit == State::Sz)
								buf += 'z';
							ܐܚܪܢܐ ܐܢ (bit == State::S1)
								buf += '1';
							ܐܚܪܢܐ /* if (bit == State::S0) */
								buf += '0';
						}
					} ܐܚܪܢܐ ܐܢ (part.base == 8 || part.base == 16) {
						ܐܢ (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						لكل (size_t index = 0; index < (size_t)value.size(); index += step) {
							ܐܢ (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = value.extract(index, min(step, value.size() - index));
							منطقي has_x = ܕܓܠܐ, all_x = ܫܪܪܐ, has_z = ܕܓܠܐ, all_z = ܫܪܪܐ;
							لكل (State bit : subvalue) {
								ܐܢ (bit == State::Sx)
									has_x = ܫܪܪܐ;
								ܐܚܪܢܐ
									all_x = ܕܓܠܐ;
								ܐܢ (bit == State::Sz)
									has_z = ܫܪܪܐ;
								ܐܚܪܢܐ
									all_z = ܕܓܠܐ;
							}
							ܐܢ (all_x)
								buf += 'x';
							ܐܚܪܢܐ ܐܢ (all_z)
								buf += 'z';
							ܐܚܪܢܐ ܐܢ (has_x)
								buf += 'X';
							ܐܚܪܢܐ ܐܢ (has_z)
								buf += 'Z';
							ܐܚܪܢܐ {
								ثابت حرف *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} ܐܚܪܢܐ ܐܢ (part.base == 10) {
						ܐܢ (part.show_base)
							prefix += "0d";
						ܐܢ (all_x)
							buf += 'x';
						ܐܚܪܢܐ ܐܢ (all_z)
							buf += 'z';
						ܐܚܪܢܐ ܐܢ (has_x)
							buf += 'X';
						ܐܚܪܢܐ ܐܢ (has_z)
							buf += 'Z';
						ܐܚܪܢܐ {
							log_assert(value.is_fully_def());
							ܐܢ (value.is_fully_zero())
								buf += '0';
							size_t index = 0;
							ܟܡܐ (!value.is_fully_zero())	{
								ܐܢ (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(value, 10, ܕܓܠܐ, ܕܓܠܐ, 4).as_int();
								value = RTLIL::const_div(value, 10, ܕܓܠܐ, ܕܓܠܐ, value.size());
								index++;
							}
						}
					} ܐܚܪܢܐ log_abort();
					ܐܢ (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						ܟܡܐ (prefix.size() + buf.size() < part.width) {
							ܐܢ (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} ܐܚܪܢܐ ܐܢ (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} ܐܚܪܢܐ ܐܢ (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				ܐܢ (prefix.size() + buf.size() < part.width) {
					size_t pad_width = part.width - prefix.size() - buf.size();
					بدل (part.justify) {
						حالة FmtPart::LEFT:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							اكسر;
						حالة FmtPart::RIGHT:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							اكسر;
						حالة FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							اكسر;
					}
				} ܐܚܪܢܐ {
					str += prefix;
					str += buf;
				}
				اكسر;
			}
		}
	}

	ܦܢܐ str;
}
