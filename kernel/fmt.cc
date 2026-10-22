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
	parts.پاڵنان_back(part);
}

فراغ Fmt::parse_rtlil(ثابت RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.clear();

	FmtPart part;
	لكل (size_t i = 0; i < fmt.size(); i++) {
		إذا (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} وإلا إذا (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} وإلا إذا (fmt[i] == '}') {
			log_assert(خطأ && "Unexpected '}' in format string");
		} وإلا إذا (fmt[i] == '{') {
			إذا (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.پاڵنان_back(part);
				part = {};
			}

			إذا (++i == fmt.size())
				log_assert(خطأ && "Unexpected end in format substitution");

			size_t arg_size = 0;
			لكل (; i < fmt.size(); i++) {
				إذا (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} وإلا إذا (fmt[i] == ':') {
					++i;
					اكسر;
				} وإلا {
					log_assert(خطأ && "Unexpected character in format substitution");
				}
			}
			إذا (i == fmt.size())
				log_assert(خطأ && "Unexpected end in format substitution");

			إذا ((size_t)args.size() < arg_size)
				log_assert(خطأ && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			إذا (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				اذهب success;
			}

			إذا (fmt[i] == '>')
				part.justify = FmtPart::ڕاست;
			وإلا إذا (fmt[i] == '<')
				part.justify = FmtPart::چەپ;
			وإلا إذا (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			وإلا
				log_assert(خطأ && "Unexpected justification in format substitution");
			إذا (++i == fmt.size())
				log_assert(خطأ && "Unexpected end in format substitution");

			part.padding = fmt[i];
			إذا (++i == fmt.size())
				log_assert(خطأ && "Unexpected end in format substitution");

			لكل (; i < fmt.size(); i++) {
				إذا (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					استمر;
				} وإلا إذا (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} وإلا إذا (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} وإلا إذا (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} وإلا إذا (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} وإلا إذا (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = true;
				} وإلا إذا (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} وإلا إذا (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} وإلا إذا (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = true;
				} وإلا {
					log_assert(خطأ && "Unexpected character in format substitution");
				}
				++i;
				اكسر;
			}
			إذا (i == fmt.size())
				log_assert(خطأ && "Unexpected end in format substitution");

			إذا (part.type == FmtPart::INTEGER) {
				إذا (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					إذا (++i == fmt.size())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا إذا (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					إذا (++i == fmt.size())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا إذا (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					إذا (++i == fmt.size())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا {
					// also accept no sign character and treat like MINUS for compatibility
				}

				إذا (fmt[i] == '#') {
					part.show_base = true;
					++i;
				}
				إذا (fmt[i] == '_') {
					part.group = true;
					++i;
				}

				إذا (fmt[i] == 'u')
					part.signed_ = خطأ;
				وإلا إذا (fmt[i] == 's')
					part.signed_ = true;
				وإلا
					log_assert(خطأ && "Unexpected character in format substitution");
				إذا (++i == fmt.size())
					log_assert(خطأ && "Unexpected end in format substitution");
			}

		success:
			إذا (fmt[i] != '}')
				log_assert(خطأ && "Expected '}' after format substitution");

			parts.پاڵنان_back(part);
			part = {};
		} وإلا {
			part.str += fmt[i];
		}
	}
	إذا (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.پاڵنان_back(part);
	}
}

فراغ Fmt::emit_rtlil(RTLIL::Cell *cell) ثابت {
	std::string fmt;
	RTLIL::SigSpec args;

	لكل (auto &part : parts) {
		بدل (part.type) {
			حالة FmtPart::LITERAL:
				لكل (حرف c : part.str) {
					إذا (c == '{')
						fmt += "{{";
					وإلا إذا (c == '}')
						fmt += "}}";
					وإلا
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
				إذا (part.justify == FmtPart::ڕاست)
					fmt += '>';
				وإلا إذا (part.justify == FmtPart::چەپ)
					fmt += '<';
				وإلا إذا (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				وإلا log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				إذا (part.width > 0)
					fmt += std::to_string(part.width);
				إذا (part.type == FmtPart::INTEGER) {
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
				} وإلا إذا (part.type == FmtPart::STRING) {
					fmt += 'c';
				} وإلا إذا (part.type == FmtPart::VLOG_TIME) {
					إذا (part.realtime)
						fmt += 'r';
					وإلا
						fmt += 't';
				} وإلا log_abort();
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
	إذا (!signed_)
		max.setBit(size, true);
	وإلا
		max.setBit(size - 1, true);
	size_t places = 0;
	طالما (!max.isZero()) {
		places++;
		max /= 10;
	}
	إذا (signed_)
		places++;
	أرجع places;
}

ساكن size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, true);
	size_t places = 0;
	طالما (!max.isZero()) {
		places++;
		max /= base;
	}
	أرجع places;
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
	إذا (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.پاڵنان_back(part);
		أرجع;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	إذا (part.width < places) {
		part.justify = FmtPart::ڕاست;
		part.width = places;
		parts.پاڵنان_back(part);
	} وإلا إذا (part.width == places) {
		parts.پاڵنان_back(part);
	} وإلا إذا (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		إذا (part.justify == FmtPart::ڕاست) {
			append_literal(gap);
			parts.پاڵنان_back(part);
		} وإلا {
			part.justify = FmtPart::ڕاست;
			parts.پاڵنان_back(part);
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
				parts.پاڵنان_back(part);
				اكسر;
			}

			حالة VerilogFmtArg::STRING: {
				إذا (arg == args.begin() || !sformat_like) {
					ثابت auto fmtarg = arg;
					ثابت std::string &fmt = fmtarg->str;
					FmtPart part = {};
					لكل (size_t i = 0; i < fmt.size(); i++) {
						إذا (fmt[i] != '%') {
							part.str += fmt[i];
						} وإلا إذا (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} وإلا إذا (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} وإلا إذا (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} وإلا {
							إذا (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.پاڵنان_back(part);
								part = {};
							}
							إذا (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							إذا (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							لكل (; i < fmt.size(); i++) {
								إذا (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::چەپ;
								} وإلا إذا (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} وإلا اكسر;
							}
							إذا (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							منطقي has_leading_zero = خطأ, has_width = خطأ;
							لكل (; i < fmt.size(); i++) {
								إذا (fmt[i] >= '0' && fmt[i] <= '9') {
									إذا (fmt[i] == '0' && !has_width) {
										has_leading_zero = true;
									} وإلا {
										has_width = true;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									استمر;
								} وإلا إذا (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} وإلا إذا (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} وإلا إذا (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} وإلا إذا (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} وإلا إذا (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} وإلا إذا (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									إذا ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} وإلا إذا (fmt[i] == 't' || fmt[i] == 'T') {
									إذا (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										إذا (!has_width && !has_leading_zero)
											part.width = 20;
									} وإلا {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} وإلا {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								اكسر;
							}
							إذا (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							إذا (part.padding == '\0') {
								إذا (has_leading_zero && part.justify == FmtPart::ڕاست) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} وإلا {
									part.padding = ' ';
								}
							}

							إذا (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							إذا (part.base != 10)
								part.signed_ = خطأ;
							إذا (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							وإلا
								parts.پاڵنان_back(part);
							part = {};
						}
					}
					إذا (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.پاڵنان_back(part);
					}
				} وإلا {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.پاڵنان_back(part);
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
					إذا (c == '%')
						fmt.str += "%%";
					وإلا
						fmt.str += c;
				}
				اكسر;

			حالة FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.پاڵنان_back(arg);

				fmt.str += '%';
				إذا (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				إذا (part.justify == FmtPart::چەپ)
					fmt.str += '-';
				إذا (part.width == 0) {
					fmt.str += '0';
				} وإلا إذا (part.width > 0) {
					إذا (part.base != 10 || part.padding == '0')
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
				args.پاڵنان_back(arg);

				fmt.str += '%';
				إذا (part.justify == FmtPart::چەپ)
					fmt.str += '-';
				إذا (part.sig.size() == 8) {
					إذا (part.width > 0) {
						إذا (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} وإلا {
					log_assert(part.sig.size() % 8 == 0);
					إذا (part.width > 0) {
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
				args.پاڵنان_back(arg);

				fmt.str += "%c";
				اكسر;
			}

			حالة FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				إذا (part.realtime)
					arg.realtime = true;
				args.پاڵنان_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				إذا (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				إذا (part.justify == FmtPart::چەپ)
					fmt.str += '-';
				إذا (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				اكسر;
			}

			افتراضي: log_abort();
		}
	}

	args.زیادکردن(args.begin(), fmt);
	أرجع args;
}

std::string escape_cxx_string(ثابت std::string &تێخستن)
{
	std::string output = "\"";
	لكل (auto c : تێخستن) {
		إذا (::isprint(c)) {
			إذا (c == '\\' || c == '"')
				output.پاڵنان_back('\\');
			output.پاڵنان_back(c);
		} وإلا {
			حرف l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.پاڵنان_back('\\');
			output.پاڵنان_back('0' + h);
			output.پاڵنان_back('0' + m);
			output.پاڵنان_back('0' + l);
		}
	}
	output.پاڵنان_back('"');
	إذا (output.find('\0') != std::string::npos) {
		output.زیادکردن(0, "std::string {");
		output.append(stringf(", %zu}", تێخستن.size()));
	}
	أرجع output;
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
			حالة FmtPart::چەپ:    os << "LEFT";    اكسر;
			حالة FmtPart::ڕاست:   os << "RIGHT";   اكسر;
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
				RTLIL::Const بەها = part.sig.as_const();
				uint32_t codepoint = بەها.as_int();
				إذا (codepoint >= 0x10000)
					str += (حرف)(0xf0 |  (codepoint >> 18));
				وإلا إذا (codepoint >= 0x800)
					str += (حرف)(0xe0 |  (codepoint >> 12));
				وإلا إذا (codepoint >= 0x80)
					str += (حرف)(0xc0 |  (codepoint >>  6));
				وإلا
					str += (حرف)codepoint;
				إذا (codepoint >= 0x10000)
					str += (حرف)(0x80 | ((codepoint >> 12) & 0x3f));
				إذا (codepoint >= 0x800)
					str += (حرف)(0x80 | ((codepoint >>  6) & 0x3f));
				إذا (codepoint >= 0x80)
					str += (حرف)(0x80 | ((codepoint >>  0) & 0x3f));
				اكسر;
			}

			حالة FmtPart::INTEGER:
			حالة FmtPart::STRING:
			حالة FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				إذا (part.type == FmtPart::INTEGER) {
					RTLIL::Const بەها = part.sig.as_const();
					منطقي has_x = خطأ, all_x = true, has_z = خطأ, all_z = true;
					لكل (State bit : بەها) {
						إذا (bit == State::Sx)
							has_x = true;
						وإلا
							all_x = خطأ;
						إذا (bit == State::Sz)
							has_z = true;
						وإلا
							all_z = خطأ;
					}

					إذا (!has_z && !has_x && part.signed_ && بەها[بەها.size() - 1]) {
						prefix = "-";
						بەها = RTLIL::const_neg(بەها, {}, part.signed_, {}, بەها.size() + 1);
					} وإلا {
						بدل (part.sign) {
							حالة FmtPart::MINUS:       اكسر;
							حالة FmtPart::PLUS_MINUS:  prefix = "+"; اكسر;
							حالة FmtPart::SPACE_MINUS: prefix = " "; اكسر;
						}
					}

					إذا (part.base != 10) {
						size_t minimum_size = 1;
						لكل (size_t index = 0; index < (size_t)بەها.size(); index++)
							إذا (بەها[index] != State::S0)
								minimum_size = index + 1;
						بەها = بەها.extract(0, minimum_size);
					}

					إذا (part.base == 2) {
						إذا (part.show_base)
							prefix += "0b";
						لكل (size_t index = 0; index < (size_t)بەها.size(); index++) {
							إذا (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = بەها[index];
							إذا (bit == State::Sx)
								buf += 'x';
							وإلا إذا (bit == State::Sz)
								buf += 'z';
							وإلا إذا (bit == State::S1)
								buf += '1';
							وإلا /* if (bit == State::S0) */
								buf += '0';
						}
					} وإلا إذا (part.base == 8 || part.base == 16) {
						إذا (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						لكل (size_t index = 0; index < (size_t)بەها.size(); index += step) {
							إذا (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = بەها.extract(index, min(step, بەها.size() - index));
							منطقي has_x = خطأ, all_x = true, has_z = خطأ, all_z = true;
							لكل (State bit : subvalue) {
								إذا (bit == State::Sx)
									has_x = true;
								وإلا
									all_x = خطأ;
								إذا (bit == State::Sz)
									has_z = true;
								وإلا
									all_z = خطأ;
							}
							إذا (all_x)
								buf += 'x';
							وإلا إذا (all_z)
								buf += 'z';
							وإلا إذا (has_x)
								buf += 'X';
							وإلا إذا (has_z)
								buf += 'Z';
							وإلا {
								ثابت حرف *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} وإلا إذا (part.base == 10) {
						إذا (part.show_base)
							prefix += "0d";
						إذا (all_x)
							buf += 'x';
						وإلا إذا (all_z)
							buf += 'z';
						وإلا إذا (has_x)
							buf += 'X';
						وإلا إذا (has_z)
							buf += 'Z';
						وإلا {
							log_assert(بەها.is_fully_def());
							إذا (بەها.is_fully_zero())
								buf += '0';
							size_t index = 0;
							طالما (!بەها.is_fully_zero())	{
								إذا (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(بەها, 10, خطأ, خطأ, 4).as_int();
								بەها = RTLIL::const_div(بەها, 10, خطأ, خطأ, بەها.size());
								index++;
							}
						}
					} وإلا log_abort();
					إذا (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						طالما (prefix.size() + buf.size() < part.width) {
							إذا (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} وإلا إذا (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} وإلا إذا (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				إذا (prefix.size() + buf.size() < part.width) {
					size_t pad_width = part.width - prefix.size() - buf.size();
					بدل (part.justify) {
						حالة FmtPart::چەپ:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							اكسر;
						حالة FmtPart::ڕاست:
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
				} وإلا {
					str += prefix;
					str += buf;
				}
				اكسر;
			}
		}
	}

	أرجع str;
}
