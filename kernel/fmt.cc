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

#অন্তর্ভাব "libs/bigint/BigUnsigned.hh"
#অন্তর্ভাব "kernel/fmt.h"

USING_YOSYS_NAMESPACE

শূন্য Fmt::append_literal(ধ্রুবক std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.ঠেলো_back(part);
}

শূন্য Fmt::parse_rtlil(ধ্রুবক RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.clear();

	FmtPart part;
	জন্য (আকার_t i = 0; i < fmt.আকার_2(); i++) {
		যদি (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} অন্যথা যদি (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} অন্যথা যদি (fmt[i] == '}') {
			log_assert(মিথ্যা && "Unexpected '}' in format string");
		} অন্যথা যদি (fmt[i] == '{') {
			যদি (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.ঠেলো_back(part);
				part = {};
			}

			যদি (++i == fmt.আকার_2())
				log_assert(মিথ্যা && "Unexpected end in format substitution");

			আকার_t arg_আকার = 0;
			জন্য (; i < fmt.আকার_2(); i++) {
				যদি (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_আকার *= 10;
					arg_আকার += fmt[i] - '0';
				} অন্যথা যদি (fmt[i] == ':') {
					++i;
					বিরতি;
				} অন্যথা {
					log_assert(মিথ্যা && "Unexpected character in format substitution");
				}
			}
			যদি (i == fmt.আকার_2())
				log_assert(মিথ্যা && "Unexpected end in format substitution");

			যদি ((আকার_t)args.আকার_2() < arg_আকার)
				log_assert(মিথ্যা && "Format part overruns arguments");
			part.sig = args.extract(0, arg_আকার);
			args.remove(0, arg_আকার);

			যদি (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				যাও success;
			}

			যদি (fmt[i] == '>')
				part.justify = FmtPart::ডান;
			অন্যথা যদি (fmt[i] == '<')
				part.justify = FmtPart::বাম;
			অন্যথা যদি (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			অন্যথা
				log_assert(মিথ্যা && "Unexpected justification in format substitution");
			যদি (++i == fmt.আকার_2())
				log_assert(মিথ্যা && "Unexpected end in format substitution");

			part.padding = fmt[i];
			যদি (++i == fmt.আকার_2())
				log_assert(মিথ্যা && "Unexpected end in format substitution");

			জন্য (; i < fmt.আকার_2(); i++) {
				যদি (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					চালু;
				} অন্যথা যদি (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} অন্যথা যদি (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} অন্যথা যদি (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} অন্যথা যদি (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} অন্যথা যদি (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = সত্য;
				} অন্যথা যদি (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} অন্যথা যদি (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} অন্যথা যদি (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = সত্য;
				} অন্যথা {
					log_assert(মিথ্যা && "Unexpected character in format substitution");
				}
				++i;
				বিরতি;
			}
			যদি (i == fmt.আকার_2())
				log_assert(মিথ্যা && "Unexpected end in format substitution");

			যদি (part.type == FmtPart::INTEGER) {
				যদি (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					যদি (++i == fmt.আকার_2())
						log_assert(মিথ্যা && "Unexpected end in format substitution");
				} অন্যথা যদি (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					যদি (++i == fmt.আকার_2())
						log_assert(মিথ্যা && "Unexpected end in format substitution");
				} অন্যথা যদি (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					যদি (++i == fmt.আকার_2())
						log_assert(মিথ্যা && "Unexpected end in format substitution");
				} অন্যথা {
					// also accept no sign character and treat like MINUS for compatibility
				}

				যদি (fmt[i] == '#') {
					part.show_base = সত্য;
					++i;
				}
				যদি (fmt[i] == '_') {
					part.group = সত্য;
					++i;
				}

				যদি (fmt[i] == 'u')
					part.signed_ = মিথ্যা;
				অন্যথা যদি (fmt[i] == 's')
					part.signed_ = সত্য;
				অন্যথা
					log_assert(মিথ্যা && "Unexpected character in format substitution");
				যদি (++i == fmt.আকার_2())
					log_assert(মিথ্যা && "Unexpected end in format substitution");
			}

		success:
			যদি (fmt[i] != '}')
				log_assert(মিথ্যা && "Expected '}' after format substitution");

			parts.ঠেলো_back(part);
			part = {};
		} অন্যথা {
			part.str += fmt[i];
		}
	}
	যদি (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.ঠেলো_back(part);
	}
}

শূন্য Fmt::emit_rtlil(RTLIL::Cell *cell) ধ্রুবক {
	std::string fmt;
	RTLIL::SigSpec args;

	জন্য (auto &part : parts) {
		নির্বাচন (part.type) {
			ক্ষেত্র FmtPart::LITERAL:
				জন্য (অক্ষর c : part.str) {
					যদি (c == '{')
						fmt += "{{";
					অন্যথা যদি (c == '}')
						fmt += "}}";
					অন্যথা
						fmt += c;
				}
				বিরতি;

			ক্ষেত্র FmtPart::UNICHAR:
				log_assert(part.sig.আকার_2() <= 32);
				fmt += "{U}";
				বিরতি;

			ক্ষেত্র FmtPart::VLOG_TIME:
				log_assert(part.sig.আকার_2() == 0);
				YS_FALLTHROUGH
			ক্ষেত্র FmtPart::STRING:
				log_assert(part.sig.আকার_2() % 8 == 0);
				YS_FALLTHROUGH
			ক্ষেত্র FmtPart::INTEGER:
				args.append(part.sig);
				fmt += '{';
				fmt += std::to_string(part.sig.আকার_2());
				fmt += ':';
				যদি (part.justify == FmtPart::ডান)
					fmt += '>';
				অন্যথা যদি (part.justify == FmtPart::বাম)
					fmt += '<';
				অন্যথা যদি (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				অন্যথা log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				যদি (part.width > 0)
					fmt += std::to_string(part.width);
				যদি (part.type == FmtPart::INTEGER) {
					নির্বাচন (part.base) {
						ক্ষেত্র  2: fmt += 'b'; বিরতি;
						ক্ষেত্র  8: fmt += 'o'; বিরতি;
						ক্ষেত্র 10: fmt += 'd'; বিরতি;
						ক্ষেত্র 16: fmt += part.hex_upper ? 'H' : 'h'; বিরতি;
						পূর্বনির্ধারিত: log_abort();
					}
					নির্বাচন (part.sign) {
						ক্ষেত্র FmtPart::MINUS:       fmt += '-'; বিরতি;
						ক্ষেত্র FmtPart::PLUS_MINUS:  fmt += '+'; বিরতি;
						ক্ষেত্র FmtPart::SPACE_MINUS: fmt += ' '; বিরতি;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} অন্যথা যদি (part.type == FmtPart::STRING) {
					fmt += 'c';
				} অন্যথা যদি (part.type == FmtPart::VLOG_TIME) {
					যদি (part.realtime)
						fmt += 'r';
					অন্যথা
						fmt += 't';
				} অন্যথা log_abort();
				fmt += '}';
				বিরতি;

			পূর্বনির্ধারিত: log_abort();
		}
	}

	cell->setParam(ID(FORMAT), fmt);
	cell->setParam(ID(ARGS_WIDTH), args.আকার_2());
	cell->setPort(ID(ARGS), args);
}

স্থির আকার_t compute_required_decimal_places(আকার_t আকার_2, যৌক্তিক signed_)
{
	BigUnsigned max;
	যদি (!signed_)
		max.setBit(আকার_2, সত্য);
	অন্যথা
		max.setBit(আকার_2 - 1, সত্য);
	আকার_t places = 0;
	যখন (!max.isZero()) {
		places++;
		max /= 10;
	}
	যদি (signed_)
		places++;
	ফেরত places;
}

স্থির আকার_t compute_required_nondecimal_places(আকার_t আকার_2, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(আকার_2 - 1, সত্য);
	আকার_t places = 0;
	যখন (!max.isZero()) {
		places++;
		max /= base;
	}
	ফেরত places;
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
শূন্য Fmt::apply_verilog_automatic_sizing_and_add(FmtPart &part)
{
	যদি (part.base == 10) {
		আকার_t places = compute_required_decimal_places(part.sig.আকার_2(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.ঠেলো_back(part);
		ফেরত;
	}

	part.padding = '0';

	আকার_t places = compute_required_nondecimal_places(part.sig.আকার_2(), part.base);
	যদি (part.width < places) {
		part.justify = FmtPart::ডান;
		part.width = places;
		parts.ঠেলো_back(part);
	} অন্যথা যদি (part.width == places) {
		parts.ঠেলো_back(part);
	} অন্যথা যদি (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		যদি (part.justify == FmtPart::ডান) {
			append_literal(gap);
			parts.ঠেলো_back(part);
		} অন্যথা {
			part.justify = FmtPart::ডান;
			parts.ঠেলো_back(part);
			append_literal(gap);
		}
	}
}

শূন্য Fmt::parse_verilog(ধ্রুবক std::vector<VerilogFmtArg> &args, যৌক্তিক sformat_like, পূর্ণসংখ্যা default_base, ধ্রুবক std::string &task_name, ধ্রুবক std::string &module_name)
{
	parts.clear();

	auto arg = args.begin();
	জন্য (; arg != args.end(); ++arg) {
		নির্বাচন (arg->type) {
			ক্ষেত্র VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				বিরতি;
			}

			ক্ষেত্র VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.ঠেলো_back(part);
				বিরতি;
			}

			ক্ষেত্র VerilogFmtArg::STRING: {
				যদি (arg == args.begin() || !sformat_like) {
					ধ্রুবক auto fmtarg = arg;
					ধ্রুবক std::string &fmt = fmtarg->str;
					FmtPart part = {};
					জন্য (আকার_t i = 0; i < fmt.আকার_2(); i++) {
						যদি (fmt[i] != '%') {
							part.str += fmt[i];
						} অন্যথা যদি (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} অন্যথা যদি (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} অন্যথা যদি (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} অন্যথা {
							যদি (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.ঠেলো_back(part);
								part = {};
							}
							যদি (++i == fmt.আকার_2()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							যদি (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							জন্য (; i < fmt.আকার_2(); i++) {
								যদি (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::বাম;
								} অন্যথা যদি (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} অন্যথা বিরতি;
							}
							যদি (i == fmt.আকার_2()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							যৌক্তিক has_leading_zero = মিথ্যা, has_width = মিথ্যা;
							জন্য (; i < fmt.আকার_2(); i++) {
								যদি (fmt[i] >= '0' && fmt[i] <= '9') {
									যদি (fmt[i] == '0' && !has_width) {
										has_leading_zero = সত্য;
									} অন্যথা {
										has_width = সত্য;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									চালু;
								} অন্যথা যদি (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} অন্যথা যদি (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} অন্যথা যদি (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} অন্যথা যদি (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} অন্যথা যদি (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} অন্যথা যদি (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									যদি ((part.sig.আকার_2() % 8) != 0)
										part.sig.extend_u0((part.sig.আকার_2() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} অন্যথা যদি (fmt[i] == 't' || fmt[i] == 'T') {
									যদি (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										যদি (!has_width && !has_leading_zero)
											part.width = 20;
									} অন্যথা {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} অন্যথা {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								বিরতি;
							}
							যদি (i == fmt.আকার_2()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							যদি (part.padding == '\0') {
								যদি (has_leading_zero && part.justify == FmtPart::ডান) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} অন্যথা {
									part.padding = ' ';
								}
							}

							যদি (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							যদি (part.base != 10)
								part.signed_ = মিথ্যা;
							যদি (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							অন্যথা
								parts.ঠেলো_back(part);
							part = {};
						}
					}
					যদি (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.ঠেলো_back(part);
					}
				} অন্যথা {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.ঠেলো_back(part);
				}
				বিরতি;
			}

			পূর্বনির্ধারিত: log_abort();
		}
	}
}

std::vector<VerilogFmtArg> Fmt::emit_verilog() ধ্রুবক
{
	std::vector<VerilogFmtArg> args;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	জন্য (auto &part : parts) {
		নির্বাচন (part.type) {
			ক্ষেত্র FmtPart::LITERAL:
				জন্য (অক্ষর c : part.str) {
					যদি (c == '%')
						fmt.str += "%%";
					অন্যথা
						fmt.str += c;
				}
				বিরতি;

			ক্ষেত্র FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.ঠেলো_back(arg);

				fmt.str += '%';
				যদি (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				যদি (part.justify == FmtPart::বাম)
					fmt.str += '-';
				যদি (part.width == 0) {
					fmt.str += '0';
				} অন্যথা যদি (part.width > 0) {
					যদি (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				নির্বাচন (part.base) {
					ক্ষেত্র  2: fmt.str += 'b'; বিরতি;
					ক্ষেত্র  8: fmt.str += 'o'; বিরতি;
					ক্ষেত্র 10: fmt.str += 'd'; বিরতি;
					ক্ষেত্র 16: fmt.str += 'h'; বিরতি; // treat uppercase hex as lowercase
					পূর্বনির্ধারিত: log_abort();
				}
				বিরতি;
			}

			ক্ষেত্র FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				args.ঠেলো_back(arg);

				fmt.str += '%';
				যদি (part.justify == FmtPart::বাম)
					fmt.str += '-';
				যদি (part.sig.আকার_2() == 8) {
					যদি (part.width > 0) {
						যদি (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} অন্যথা {
					log_assert(part.sig.আকার_2() % 8 == 0);
					যদি (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				বিরতি;
			}

			ক্ষেত্র FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.ঠেলো_back(arg);

				fmt.str += "%c";
				বিরতি;
			}

			ক্ষেত্র FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				যদি (part.realtime)
					arg.realtime = সত্য;
				args.ঠেলো_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				যদি (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				যদি (part.justify == FmtPart::বাম)
					fmt.str += '-';
				যদি (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				বিরতি;
			}

			পূর্বনির্ধারিত: log_abort();
		}
	}

	args.ঢোকাও(args.begin(), fmt);
	ফেরত args;
}

std::string escape_cxx_string(ধ্রুবক std::string &প্রবেশ)
{
	std::string output = "\"";
	জন্য (auto c : প্রবেশ) {
		যদি (::isprint(c)) {
			যদি (c == '\\' || c == '"')
				output.ঠেলো_back('\\');
			output.ঠেলো_back(c);
		} অন্যথা {
			অক্ষর l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.ঠেলো_back('\\');
			output.ঠেলো_back('0' + h);
			output.ঠেলো_back('0' + m);
			output.ঠেলো_back('0' + l);
		}
	}
	output.ঠেলো_back('"');
	যদি (output.find('\0') != std::string::npos) {
		output.ঢোকাও(0, "std::string {");
		output.append(stringf(", %zu}", প্রবেশ.আকার_2()));
	}
	ফেরত output;
}

শূন্য Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<শূন্য(ধ্রুবক RTLIL::SigSpec &)> emit_sig, ধ্রুবক std::string &context) ধ্রুবক
{
	os << indent << "std::string buf;\n";
	জন্য (auto &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		নির্বাচন (part.type) {
			ক্ষেত্র FmtPart::LITERAL:   os << "LITERAL";   বিরতি;
			ক্ষেত্র FmtPart::INTEGER:   os << "INTEGER";   বিরতি;
			ক্ষেত্র FmtPart::STRING:    os << "STRING";    বিরতি;
			ক্ষেত্র FmtPart::UNICHAR:   os << "UNICHAR";   বিরতি;
			ক্ষেত্র FmtPart::VLOG_TIME: os << "VLOG_TIME"; বিরতি;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		নির্বাচন (part.justify) {
			ক্ষেত্র FmtPart::বাম:    os << "LEFT";    বিরতি;
			ক্ষেত্র FmtPart::ডান:   os << "RIGHT";   বিরতি;
			ক্ষেত্র FmtPart::NUMERIC: os << "NUMERIC"; বিরতি;
		}
		os << ", ";
		os << "(char)" << (পূর্ণসংখ্যা)part.padding << ", ";
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		নির্বাচন (part.sign) {
			ক্ষেত্র FmtPart::MINUS:       os << "MINUS";       বিরতি;
			ক্ষেত্র FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  বিরতি;
			ক্ষেত্র FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; বিরতি;
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

std::string Fmt::render() ধ্রুবক
{
	std::string str;

	জন্য (auto &part : parts) {
		নির্বাচন (part.type) {
			ক্ষেত্র FmtPart::LITERAL:
				str += part.str;
				বিরতি;

			ক্ষেত্র FmtPart::UNICHAR: {
				RTLIL::Const মান = part.sig.as_const();
				uint32_t codepoint = মান.as_int();
				যদি (codepoint >= 0x10000)
					str += (অক্ষর)(0xf0 |  (codepoint >> 18));
				অন্যথা যদি (codepoint >= 0x800)
					str += (অক্ষর)(0xe0 |  (codepoint >> 12));
				অন্যথা যদি (codepoint >= 0x80)
					str += (অক্ষর)(0xc0 |  (codepoint >>  6));
				অন্যথা
					str += (অক্ষর)codepoint;
				যদি (codepoint >= 0x10000)
					str += (অক্ষর)(0x80 | ((codepoint >> 12) & 0x3f));
				যদি (codepoint >= 0x800)
					str += (অক্ষর)(0x80 | ((codepoint >>  6) & 0x3f));
				যদি (codepoint >= 0x80)
					str += (অক্ষর)(0x80 | ((codepoint >>  0) & 0x3f));
				বিরতি;
			}

			ক্ষেত্র FmtPart::INTEGER:
			ক্ষেত্র FmtPart::STRING:
			ক্ষেত্র FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				যদি (part.type == FmtPart::INTEGER) {
					RTLIL::Const মান = part.sig.as_const();
					যৌক্তিক has_x = মিথ্যা, all_x = সত্য, has_z = মিথ্যা, all_z = সত্য;
					জন্য (State bit : মান) {
						যদি (bit == State::Sx)
							has_x = সত্য;
						অন্যথা
							all_x = মিথ্যা;
						যদি (bit == State::Sz)
							has_z = সত্য;
						অন্যথা
							all_z = মিথ্যা;
					}

					যদি (!has_z && !has_x && part.signed_ && মান[মান.আকার_2() - 1]) {
						prefix = "-";
						মান = RTLIL::const_neg(মান, {}, part.signed_, {}, মান.আকার_2() + 1);
					} অন্যথা {
						নির্বাচন (part.sign) {
							ক্ষেত্র FmtPart::MINUS:       বিরতি;
							ক্ষেত্র FmtPart::PLUS_MINUS:  prefix = "+"; বিরতি;
							ক্ষেত্র FmtPart::SPACE_MINUS: prefix = " "; বিরতি;
						}
					}

					যদি (part.base != 10) {
						আকার_t minimum_আকার = 1;
						জন্য (আকার_t সূচক = 0; সূচক < (আকার_t)মান.আকার_2(); সূচক++)
							যদি (মান[সূচক] != State::S0)
								minimum_আকার = সূচক + 1;
						মান = মান.extract(0, minimum_আকার);
					}

					যদি (part.base == 2) {
						যদি (part.show_base)
							prefix += "0b";
						জন্য (আকার_t সূচক = 0; সূচক < (আকার_t)মান.আকার_2(); সূচক++) {
							যদি (part.group && সূচক > 0 && সূচক % 4 == 0)
								buf += '_';
							RTLIL::State bit = মান[সূচক];
							যদি (bit == State::Sx)
								buf += 'x';
							অন্যথা যদি (bit == State::Sz)
								buf += 'z';
							অন্যথা যদি (bit == State::S1)
								buf += '1';
							অন্যথা /* if (bit == State::S0) */
								buf += '0';
						}
					} অন্যথা যদি (part.base == 8 || part.base == 16) {
						যদি (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						আকার_t step = (part.base == 16) ? 4 : 3;
						জন্য (আকার_t সূচক = 0; সূচক < (আকার_t)মান.আকার_2(); সূচক += step) {
							যদি (part.group && সূচক > 0 && সূচক % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = মান.extract(সূচক, min(step, মান.আকার_2() - সূচক));
							যৌক্তিক has_x = মিথ্যা, all_x = সত্য, has_z = মিথ্যা, all_z = সত্য;
							জন্য (State bit : subvalue) {
								যদি (bit == State::Sx)
									has_x = সত্য;
								অন্যথা
									all_x = মিথ্যা;
								যদি (bit == State::Sz)
									has_z = সত্য;
								অন্যথা
									all_z = মিথ্যা;
							}
							যদি (all_x)
								buf += 'x';
							অন্যথা যদি (all_z)
								buf += 'z';
							অন্যথা যদি (has_x)
								buf += 'X';
							অন্যথা যদি (has_z)
								buf += 'Z';
							অন্যথা {
								ধ্রুবক অক্ষর *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} অন্যথা যদি (part.base == 10) {
						যদি (part.show_base)
							prefix += "0d";
						যদি (all_x)
							buf += 'x';
						অন্যথা যদি (all_z)
							buf += 'z';
						অন্যথা যদি (has_x)
							buf += 'X';
						অন্যথা যদি (has_z)
							buf += 'Z';
						অন্যথা {
							log_assert(মান.is_fully_def());
							যদি (মান.is_fully_zero())
								buf += '0';
							আকার_t সূচক = 0;
							যখন (!মান.is_fully_zero())	{
								যদি (part.group && সূচক > 0 && সূচক % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(মান, 10, মিথ্যা, মিথ্যা, 4).as_int();
								মান = RTLIL::const_div(মান, 10, মিথ্যা, মিথ্যা, মান.আকার_2());
								সূচক++;
							}
						}
					} অন্যথা log_abort();
					যদি (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						আকার_t group_আকার = part.base == 10 ? 3 : 4;
						যখন (prefix.আকার_2() + buf.আকার_2() < part.width) {
							যদি (buf.আকার_2() % (group_আকার + 1) == group_আকার)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} অন্যথা যদি (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} অন্যথা যদি (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				যদি (prefix.আকার_2() + buf.আকার_2() < part.width) {
					আকার_t pad_width = part.width - prefix.আকার_2() - buf.আকার_2();
					নির্বাচন (part.justify) {
						ক্ষেত্র FmtPart::বাম:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							বিরতি;
						ক্ষেত্র FmtPart::ডান:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							বিরতি;
						ক্ষেত্র FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							বিরতি;
					}
				} অন্যথা {
					str += prefix;
					str += buf;
				}
				বিরতি;
			}
		}
	}

	ফেরত str;
}
