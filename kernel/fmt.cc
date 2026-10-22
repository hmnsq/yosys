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

#شامل "libs/bigint/BigUnsigned.hh"
#شامل "kernel/fmt.h"

USING_YOSYS_NAMESPACE

تهی Fmt::append_literal(ثابت std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.افزودن_back(part);
}

تهی Fmt::parse_rtlil(ثابت RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.clear();

	FmtPart part;
	برای (size_t i = 0; i < fmt.size(); i++) {
		اگر (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} وگرنه اگر (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} وگرنه اگر (fmt[i] == '}') {
			log_assert(نادرست && "Unexpected '}' in format string");
		} وگرنه اگر (fmt[i] == '{') {
			اگر (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.افزودن_back(part);
				part = {};
			}

			اگر (++i == fmt.size())
				log_assert(نادرست && "Unexpected end in format substitution");

			size_t arg_size = 0;
			برای (; i < fmt.size(); i++) {
				اگر (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} وگرنه اگر (fmt[i] == ':') {
					++i;
					بشکن;
				} وگرنه {
					log_assert(نادرست && "Unexpected character in format substitution");
				}
			}
			اگر (i == fmt.size())
				log_assert(نادرست && "Unexpected end in format substitution");

			اگر ((size_t)args.size() < arg_size)
				log_assert(نادرست && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			اگر (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				برو success;
			}

			اگر (fmt[i] == '>')
				part.justify = FmtPart::راست;
			وگرنه اگر (fmt[i] == '<')
				part.justify = FmtPart::چپ;
			وگرنه اگر (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			وگرنه
				log_assert(نادرست && "Unexpected justification in format substitution");
			اگر (++i == fmt.size())
				log_assert(نادرست && "Unexpected end in format substitution");

			part.padding = fmt[i];
			اگر (++i == fmt.size())
				log_assert(نادرست && "Unexpected end in format substitution");

			برای (; i < fmt.size(); i++) {
				اگر (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					ادامه;
				} وگرنه اگر (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} وگرنه اگر (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} وگرنه اگر (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} وگرنه اگر (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} وگرنه اگر (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = درست;
				} وگرنه اگر (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} وگرنه اگر (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} وگرنه اگر (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = درست;
				} وگرنه {
					log_assert(نادرست && "Unexpected character in format substitution");
				}
				++i;
				بشکن;
			}
			اگر (i == fmt.size())
				log_assert(نادرست && "Unexpected end in format substitution");

			اگر (part.type == FmtPart::INTEGER) {
				اگر (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					اگر (++i == fmt.size())
						log_assert(نادرست && "Unexpected end in format substitution");
				} وگرنه اگر (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					اگر (++i == fmt.size())
						log_assert(نادرست && "Unexpected end in format substitution");
				} وگرنه اگر (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					اگر (++i == fmt.size())
						log_assert(نادرست && "Unexpected end in format substitution");
				} وگرنه {
					// also accept no sign character and treat like MINUS for compatibility
				}

				اگر (fmt[i] == '#') {
					part.show_base = درست;
					++i;
				}
				اگر (fmt[i] == '_') {
					part.group = درست;
					++i;
				}

				اگر (fmt[i] == 'u')
					part.signed_ = نادرست;
				وگرنه اگر (fmt[i] == 's')
					part.signed_ = درست;
				وگرنه
					log_assert(نادرست && "Unexpected character in format substitution");
				اگر (++i == fmt.size())
					log_assert(نادرست && "Unexpected end in format substitution");
			}

		success:
			اگر (fmt[i] != '}')
				log_assert(نادرست && "Expected '}' after format substitution");

			parts.افزودن_back(part);
			part = {};
		} وگرنه {
			part.str += fmt[i];
		}
	}
	اگر (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.افزودن_back(part);
	}
}

تهی Fmt::emit_rtlil(RTLIL::Cell *cell) ثابت {
	std::string fmt;
	RTLIL::SigSpec args;

	برای (auto &part : parts) {
		انتخاب (part.type) {
			حالت FmtPart::LITERAL:
				برای (نویسه c : part.str) {
					اگر (c == '{')
						fmt += "{{";
					وگرنه اگر (c == '}')
						fmt += "}}";
					وگرنه
						fmt += c;
				}
				بشکن;

			حالت FmtPart::UNICHAR:
				log_assert(part.sig.size() <= 32);
				fmt += "{U}";
				بشکن;

			حالت FmtPart::VLOG_TIME:
				log_assert(part.sig.size() == 0);
				YS_FALLTHROUGH
			حالت FmtPart::STRING:
				log_assert(part.sig.size() % 8 == 0);
				YS_FALLTHROUGH
			حالت FmtPart::INTEGER:
				args.append(part.sig);
				fmt += '{';
				fmt += std::to_string(part.sig.size());
				fmt += ':';
				اگر (part.justify == FmtPart::راست)
					fmt += '>';
				وگرنه اگر (part.justify == FmtPart::چپ)
					fmt += '<';
				وگرنه اگر (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				وگرنه log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				اگر (part.width > 0)
					fmt += std::to_string(part.width);
				اگر (part.type == FmtPart::INTEGER) {
					انتخاب (part.base) {
						حالت  2: fmt += 'b'; بشکن;
						حالت  8: fmt += 'o'; بشکن;
						حالت 10: fmt += 'd'; بشکن;
						حالت 16: fmt += part.hex_upper ? 'H' : 'h'; بشکن;
						پیشفرض: log_abort();
					}
					انتخاب (part.sign) {
						حالت FmtPart::MINUS:       fmt += '-'; بشکن;
						حالت FmtPart::PLUS_MINUS:  fmt += '+'; بشکن;
						حالت FmtPart::SPACE_MINUS: fmt += ' '; بشکن;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} وگرنه اگر (part.type == FmtPart::STRING) {
					fmt += 'c';
				} وگرنه اگر (part.type == FmtPart::VLOG_TIME) {
					اگر (part.realtime)
						fmt += 'r';
					وگرنه
						fmt += 't';
				} وگرنه log_abort();
				fmt += '}';
				بشکن;

			پیشفرض: log_abort();
		}
	}

	cell->setParam(ID(FORMAT), fmt);
	cell->setParam(ID(ARGS_WIDTH), args.size());
	cell->setPort(ID(ARGS), args);
}

ایستا size_t compute_required_decimal_places(size_t size, منطقی signed_)
{
	BigUnsigned max;
	اگر (!signed_)
		max.setBit(size, درست);
	وگرنه
		max.setBit(size - 1, درست);
	size_t places = 0;
	وقتی (!max.isZero()) {
		places++;
		max /= 10;
	}
	اگر (signed_)
		places++;
	بازگشت places;
}

ایستا size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, درست);
	size_t places = 0;
	وقتی (!max.isZero()) {
		places++;
		max /= base;
	}
	بازگشت places;
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
تهی Fmt::apply_verilog_automatic_sizing_and_add(FmtPart &part)
{
	اگر (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.افزودن_back(part);
		بازگشت;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	اگر (part.width < places) {
		part.justify = FmtPart::راست;
		part.width = places;
		parts.افزودن_back(part);
	} وگرنه اگر (part.width == places) {
		parts.افزودن_back(part);
	} وگرنه اگر (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		اگر (part.justify == FmtPart::راست) {
			append_literal(gap);
			parts.افزودن_back(part);
		} وگرنه {
			part.justify = FmtPart::راست;
			parts.افزودن_back(part);
			append_literal(gap);
		}
	}
}

تهی Fmt::parse_verilog(ثابت std::vector<VerilogFmtArg> &args, منطقی sformat_like, صحیح default_base, ثابت std::string &task_name, ثابت std::string &module_name)
{
	parts.clear();

	auto arg = args.begin();
	برای (; arg != args.end(); ++arg) {
		انتخاب (arg->type) {
			حالت VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				بشکن;
			}

			حالت VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.افزودن_back(part);
				بشکن;
			}

			حالت VerilogFmtArg::STRING: {
				اگر (arg == args.begin() || !sformat_like) {
					ثابت auto fmtarg = arg;
					ثابت std::string &fmt = fmtarg->str;
					FmtPart part = {};
					برای (size_t i = 0; i < fmt.size(); i++) {
						اگر (fmt[i] != '%') {
							part.str += fmt[i];
						} وگرنه اگر (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} وگرنه اگر (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} وگرنه اگر (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} وگرنه {
							اگر (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.افزودن_back(part);
								part = {};
							}
							اگر (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							اگر (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							برای (; i < fmt.size(); i++) {
								اگر (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::چپ;
								} وگرنه اگر (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} وگرنه بشکن;
							}
							اگر (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							منطقی has_leading_zero = نادرست, has_width = نادرست;
							برای (; i < fmt.size(); i++) {
								اگر (fmt[i] >= '0' && fmt[i] <= '9') {
									اگر (fmt[i] == '0' && !has_width) {
										has_leading_zero = درست;
									} وگرنه {
										has_width = درست;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									ادامه;
								} وگرنه اگر (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} وگرنه اگر (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} وگرنه اگر (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} وگرنه اگر (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} وگرنه اگر (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} وگرنه اگر (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									اگر ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} وگرنه اگر (fmt[i] == 't' || fmt[i] == 'T') {
									اگر (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										اگر (!has_width && !has_leading_zero)
											part.width = 20;
									} وگرنه {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} وگرنه {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								بشکن;
							}
							اگر (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							اگر (part.padding == '\0') {
								اگر (has_leading_zero && part.justify == FmtPart::راست) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} وگرنه {
									part.padding = ' ';
								}
							}

							اگر (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							اگر (part.base != 10)
								part.signed_ = نادرست;
							اگر (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							وگرنه
								parts.افزودن_back(part);
							part = {};
						}
					}
					اگر (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.افزودن_back(part);
					}
				} وگرنه {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.افزودن_back(part);
				}
				بشکن;
			}

			پیشفرض: log_abort();
		}
	}
}

std::vector<VerilogFmtArg> Fmt::emit_verilog() ثابت
{
	std::vector<VerilogFmtArg> args;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	برای (auto &part : parts) {
		انتخاب (part.type) {
			حالت FmtPart::LITERAL:
				برای (نویسه c : part.str) {
					اگر (c == '%')
						fmt.str += "%%";
					وگرنه
						fmt.str += c;
				}
				بشکن;

			حالت FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.افزودن_back(arg);

				fmt.str += '%';
				اگر (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				اگر (part.justify == FmtPart::چپ)
					fmt.str += '-';
				اگر (part.width == 0) {
					fmt.str += '0';
				} وگرنه اگر (part.width > 0) {
					اگر (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				انتخاب (part.base) {
					حالت  2: fmt.str += 'b'; بشکن;
					حالت  8: fmt.str += 'o'; بشکن;
					حالت 10: fmt.str += 'd'; بشکن;
					حالت 16: fmt.str += 'h'; بشکن; // treat uppercase hex as lowercase
					پیشفرض: log_abort();
				}
				بشکن;
			}

			حالت FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				args.افزودن_back(arg);

				fmt.str += '%';
				اگر (part.justify == FmtPart::چپ)
					fmt.str += '-';
				اگر (part.sig.size() == 8) {
					اگر (part.width > 0) {
						اگر (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} وگرنه {
					log_assert(part.sig.size() % 8 == 0);
					اگر (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				بشکن;
			}

			حالت FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.افزودن_back(arg);

				fmt.str += "%c";
				بشکن;
			}

			حالت FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				اگر (part.realtime)
					arg.realtime = درست;
				args.افزودن_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				اگر (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				اگر (part.justify == FmtPart::چپ)
					fmt.str += '-';
				اگر (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				بشکن;
			}

			پیشفرض: log_abort();
		}
	}

	args.درج(args.begin(), fmt);
	بازگشت args;
}

std::string escape_cxx_string(ثابت std::string &درون_داد)
{
	std::string output = "\"";
	برای (auto c : درون_داد) {
		اگر (::isprint(c)) {
			اگر (c == '\\' || c == '"')
				output.افزودن_back('\\');
			output.افزودن_back(c);
		} وگرنه {
			نویسه l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.افزودن_back('\\');
			output.افزودن_back('0' + h);
			output.افزودن_back('0' + m);
			output.افزودن_back('0' + l);
		}
	}
	output.افزودن_back('"');
	اگر (output.find('\0') != std::string::npos) {
		output.درج(0, "std::string {");
		output.append(stringf(", %zu}", درون_داد.size()));
	}
	بازگشت output;
}

تهی Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<تهی(ثابت RTLIL::SigSpec &)> emit_sig, ثابت std::string &context) ثابت
{
	os << indent << "std::string buf;\n";
	برای (auto &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		انتخاب (part.type) {
			حالت FmtPart::LITERAL:   os << "LITERAL";   بشکن;
			حالت FmtPart::INTEGER:   os << "INTEGER";   بشکن;
			حالت FmtPart::STRING:    os << "STRING";    بشکن;
			حالت FmtPart::UNICHAR:   os << "UNICHAR";   بشکن;
			حالت FmtPart::VLOG_TIME: os << "VLOG_TIME"; بشکن;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		انتخاب (part.justify) {
			حالت FmtPart::چپ:    os << "LEFT";    بشکن;
			حالت FmtPart::راست:   os << "RIGHT";   بشکن;
			حالت FmtPart::NUMERIC: os << "NUMERIC"; بشکن;
		}
		os << ", ";
		os << "(char)" << (صحیح)part.padding << ", ";
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		انتخاب (part.sign) {
			حالت FmtPart::MINUS:       os << "MINUS";       بشکن;
			حالت FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  بشکن;
			حالت FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; بشکن;
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

	برای (auto &part : parts) {
		انتخاب (part.type) {
			حالت FmtPart::LITERAL:
				str += part.str;
				بشکن;

			حالت FmtPart::UNICHAR: {
				RTLIL::Const مقدار = part.sig.as_const();
				uint32_t codepoint = مقدار.as_int();
				اگر (codepoint >= 0x10000)
					str += (نویسه)(0xf0 |  (codepoint >> 18));
				وگرنه اگر (codepoint >= 0x800)
					str += (نویسه)(0xe0 |  (codepoint >> 12));
				وگرنه اگر (codepoint >= 0x80)
					str += (نویسه)(0xc0 |  (codepoint >>  6));
				وگرنه
					str += (نویسه)codepoint;
				اگر (codepoint >= 0x10000)
					str += (نویسه)(0x80 | ((codepoint >> 12) & 0x3f));
				اگر (codepoint >= 0x800)
					str += (نویسه)(0x80 | ((codepoint >>  6) & 0x3f));
				اگر (codepoint >= 0x80)
					str += (نویسه)(0x80 | ((codepoint >>  0) & 0x3f));
				بشکن;
			}

			حالت FmtPart::INTEGER:
			حالت FmtPart::STRING:
			حالت FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				اگر (part.type == FmtPart::INTEGER) {
					RTLIL::Const مقدار = part.sig.as_const();
					منطقی has_x = نادرست, all_x = درست, has_z = نادرست, all_z = درست;
					برای (State bit : مقدار) {
						اگر (bit == State::Sx)
							has_x = درست;
						وگرنه
							all_x = نادرست;
						اگر (bit == State::Sz)
							has_z = درست;
						وگرنه
							all_z = نادرست;
					}

					اگر (!has_z && !has_x && part.signed_ && مقدار[مقدار.size() - 1]) {
						prefix = "-";
						مقدار = RTLIL::const_neg(مقدار, {}, part.signed_, {}, مقدار.size() + 1);
					} وگرنه {
						انتخاب (part.sign) {
							حالت FmtPart::MINUS:       بشکن;
							حالت FmtPart::PLUS_MINUS:  prefix = "+"; بشکن;
							حالت FmtPart::SPACE_MINUS: prefix = " "; بشکن;
						}
					}

					اگر (part.base != 10) {
						size_t minimum_size = 1;
						برای (size_t index = 0; index < (size_t)مقدار.size(); index++)
							اگر (مقدار[index] != State::S0)
								minimum_size = index + 1;
						مقدار = مقدار.extract(0, minimum_size);
					}

					اگر (part.base == 2) {
						اگر (part.show_base)
							prefix += "0b";
						برای (size_t index = 0; index < (size_t)مقدار.size(); index++) {
							اگر (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = مقدار[index];
							اگر (bit == State::Sx)
								buf += 'x';
							وگرنه اگر (bit == State::Sz)
								buf += 'z';
							وگرنه اگر (bit == State::S1)
								buf += '1';
							وگرنه /* if (bit == State::S0) */
								buf += '0';
						}
					} وگرنه اگر (part.base == 8 || part.base == 16) {
						اگر (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						برای (size_t index = 0; index < (size_t)مقدار.size(); index += step) {
							اگر (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = مقدار.extract(index, min(step, مقدار.size() - index));
							منطقی has_x = نادرست, all_x = درست, has_z = نادرست, all_z = درست;
							برای (State bit : subvalue) {
								اگر (bit == State::Sx)
									has_x = درست;
								وگرنه
									all_x = نادرست;
								اگر (bit == State::Sz)
									has_z = درست;
								وگرنه
									all_z = نادرست;
							}
							اگر (all_x)
								buf += 'x';
							وگرنه اگر (all_z)
								buf += 'z';
							وگرنه اگر (has_x)
								buf += 'X';
							وگرنه اگر (has_z)
								buf += 'Z';
							وگرنه {
								ثابت نویسه *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} وگرنه اگر (part.base == 10) {
						اگر (part.show_base)
							prefix += "0d";
						اگر (all_x)
							buf += 'x';
						وگرنه اگر (all_z)
							buf += 'z';
						وگرنه اگر (has_x)
							buf += 'X';
						وگرنه اگر (has_z)
							buf += 'Z';
						وگرنه {
							log_assert(مقدار.is_fully_def());
							اگر (مقدار.is_fully_zero())
								buf += '0';
							size_t index = 0;
							وقتی (!مقدار.is_fully_zero())	{
								اگر (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(مقدار, 10, نادرست, نادرست, 4).as_int();
								مقدار = RTLIL::const_div(مقدار, 10, نادرست, نادرست, مقدار.size());
								index++;
							}
						}
					} وگرنه log_abort();
					اگر (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						وقتی (prefix.size() + buf.size() < part.width) {
							اگر (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} وگرنه اگر (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} وگرنه اگر (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				اگر (prefix.size() + buf.size() < part.width) {
					size_t pad_width = part.width - prefix.size() - buf.size();
					انتخاب (part.justify) {
						حالت FmtPart::چپ:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							بشکن;
						حالت FmtPart::راست:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							بشکن;
						حالت FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							بشکن;
					}
				} وگرنه {
					str += prefix;
					str += buf;
				}
				بشکن;
			}
		}
	}

	بازگشت str;
}
