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
	עבור (size_t i = 0; i < fmt.size(); i++) {
		אם (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} אחרת אם (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} אחרת אם (fmt[i] == '}') {
			log_assert(שקר && "Unexpected '}' in format string");
		} אחרת אם (fmt[i] == '{') {
			אם (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.push_back(part);
				part = {};
			}

			אם (++i == fmt.size())
				log_assert(שקר && "Unexpected end in format substitution");

			size_t arg_size = 0;
			עבור (; i < fmt.size(); i++) {
				אם (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} אחרת אם (fmt[i] == ':') {
					++i;
					اكسر;
				} אחרת {
					log_assert(שקר && "Unexpected character in format substitution");
				}
			}
			אם (i == fmt.size())
				log_assert(שקר && "Unexpected end in format substitution");

			אם ((size_t)args.size() < arg_size)
				log_assert(שקר && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			אם (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				اذهب success;
			}

			אם (fmt[i] == '>')
				part.justify = FmtPart::RIGHT;
			אחרת אם (fmt[i] == '<')
				part.justify = FmtPart::LEFT;
			אחרת אם (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			אחרת
				log_assert(שקר && "Unexpected justification in format substitution");
			אם (++i == fmt.size())
				log_assert(שקר && "Unexpected end in format substitution");

			part.padding = fmt[i];
			אם (++i == fmt.size())
				log_assert(שקר && "Unexpected end in format substitution");

			עבור (; i < fmt.size(); i++) {
				אם (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					استمر;
				} אחרת אם (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} אחרת אם (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} אחרת אם (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} אחרת אם (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} אחרת אם (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = אמת;
				} אחרת אם (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} אחרת אם (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} אחרת אם (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = אמת;
				} אחרת {
					log_assert(שקר && "Unexpected character in format substitution");
				}
				++i;
				اكسر;
			}
			אם (i == fmt.size())
				log_assert(שקר && "Unexpected end in format substitution");

			אם (part.type == FmtPart::INTEGER) {
				אם (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					אם (++i == fmt.size())
						log_assert(שקר && "Unexpected end in format substitution");
				} אחרת אם (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					אם (++i == fmt.size())
						log_assert(שקר && "Unexpected end in format substitution");
				} אחרת אם (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					אם (++i == fmt.size())
						log_assert(שקר && "Unexpected end in format substitution");
				} אחרת {
					// also accept no sign character and treat like MINUS for compatibility
				}

				אם (fmt[i] == '#') {
					part.show_base = אמת;
					++i;
				}
				אם (fmt[i] == '_') {
					part.group = אמת;
					++i;
				}

				אם (fmt[i] == 'u')
					part.signed_ = שקר;
				אחרת אם (fmt[i] == 's')
					part.signed_ = אמת;
				אחרת
					log_assert(שקר && "Unexpected character in format substitution");
				אם (++i == fmt.size())
					log_assert(שקר && "Unexpected end in format substitution");
			}

		success:
			אם (fmt[i] != '}')
				log_assert(שקר && "Expected '}' after format substitution");

			parts.push_back(part);
			part = {};
		} אחרת {
			part.str += fmt[i];
		}
	}
	אם (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.push_back(part);
	}
}

فراغ Fmt::emit_rtlil(RTLIL::Cell *cell) ثابت {
	std::string fmt;
	RTLIL::SigSpec args;

	עבור (auto &part : parts) {
		بدل (part.type) {
			حالة FmtPart::LITERAL:
				עבור (حرف c : part.str) {
					אם (c == '{')
						fmt += "{{";
					אחרת אם (c == '}')
						fmt += "}}";
					אחרת
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
				אם (part.justify == FmtPart::RIGHT)
					fmt += '>';
				אחרת אם (part.justify == FmtPart::LEFT)
					fmt += '<';
				אחרת אם (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				אחרת log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				אם (part.width > 0)
					fmt += std::to_string(part.width);
				אם (part.type == FmtPart::INTEGER) {
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
				} אחרת אם (part.type == FmtPart::STRING) {
					fmt += 'c';
				} אחרת אם (part.type == FmtPart::VLOG_TIME) {
					אם (part.realtime)
						fmt += 'r';
					אחרת
						fmt += 't';
				} אחרת log_abort();
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
	אם (!signed_)
		max.setBit(size, אמת);
	אחרת
		max.setBit(size - 1, אמת);
	size_t places = 0;
	כלעוד (!max.isZero()) {
		places++;
		max /= 10;
	}
	אם (signed_)
		places++;
	החזר places;
}

ساكن size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, אמת);
	size_t places = 0;
	כלעוד (!max.isZero()) {
		places++;
		max /= base;
	}
	החזר places;
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
	אם (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.push_back(part);
		החזר;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	אם (part.width < places) {
		part.justify = FmtPart::RIGHT;
		part.width = places;
		parts.push_back(part);
	} אחרת אם (part.width == places) {
		parts.push_back(part);
	} אחרת אם (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		אם (part.justify == FmtPart::RIGHT) {
			append_literal(gap);
			parts.push_back(part);
		} אחרת {
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
	עבור (; arg != args.end(); ++arg) {
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
				אם (arg == args.begin() || !sformat_like) {
					ثابت auto fmtarg = arg;
					ثابت std::string &fmt = fmtarg->str;
					FmtPart part = {};
					עבור (size_t i = 0; i < fmt.size(); i++) {
						אם (fmt[i] != '%') {
							part.str += fmt[i];
						} אחרת אם (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} אחרת אם (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} אחרת אם (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} אחרת {
							אם (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.push_back(part);
								part = {};
							}
							אם (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							אם (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							עבור (; i < fmt.size(); i++) {
								אם (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::LEFT;
								} אחרת אם (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} אחרת اكسر;
							}
							אם (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							منطقي has_leading_zero = שקר, has_width = שקר;
							עבור (; i < fmt.size(); i++) {
								אם (fmt[i] >= '0' && fmt[i] <= '9') {
									אם (fmt[i] == '0' && !has_width) {
										has_leading_zero = אמת;
									} אחרת {
										has_width = אמת;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									استمر;
								} אחרת אם (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} אחרת אם (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} אחרת אם (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} אחרת אם (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} אחרת אם (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} אחרת אם (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									אם ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} אחרת אם (fmt[i] == 't' || fmt[i] == 'T') {
									אם (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										אם (!has_width && !has_leading_zero)
											part.width = 20;
									} אחרת {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} אחרת {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								اكسر;
							}
							אם (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							אם (part.padding == '\0') {
								אם (has_leading_zero && part.justify == FmtPart::RIGHT) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} אחרת {
									part.padding = ' ';
								}
							}

							אם (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							אם (part.base != 10)
								part.signed_ = שקר;
							אם (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							אחרת
								parts.push_back(part);
							part = {};
						}
					}
					אם (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.push_back(part);
					}
				} אחרת {
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

	עבור (auto &part : parts) {
		بدل (part.type) {
			حالة FmtPart::LITERAL:
				עבור (حرف c : part.str) {
					אם (c == '%')
						fmt.str += "%%";
					אחרת
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
				אם (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				אם (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				אם (part.width == 0) {
					fmt.str += '0';
				} אחרת אם (part.width > 0) {
					אם (part.base != 10 || part.padding == '0')
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
				אם (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				אם (part.sig.size() == 8) {
					אם (part.width > 0) {
						אם (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} אחרת {
					log_assert(part.sig.size() % 8 == 0);
					אם (part.width > 0) {
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
				אם (part.realtime)
					arg.realtime = אמת;
				args.push_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				אם (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				אם (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				אם (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				اكسر;
			}

			افتراضي: log_abort();
		}
	}

	args.insert(args.begin(), fmt);
	החזר args;
}

std::string escape_cxx_string(ثابت std::string &input)
{
	std::string output = "\"";
	עבור (auto c : input) {
		אם (::isprint(c)) {
			אם (c == '\\' || c == '"')
				output.push_back('\\');
			output.push_back(c);
		} אחרת {
			حرف l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.push_back('\\');
			output.push_back('0' + h);
			output.push_back('0' + m);
			output.push_back('0' + l);
		}
	}
	output.push_back('"');
	אם (output.find('\0') != std::string::npos) {
		output.insert(0, "std::string {");
		output.append(stringf(", %zu}", input.size()));
	}
	החזר output;
}

فراغ Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<فراغ(ثابت RTLIL::SigSpec &)> emit_sig, ثابت std::string &context) ثابت
{
	os << indent << "std::string buf;\n";
	עבור (auto &part : parts) {
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

	עבור (auto &part : parts) {
		بدل (part.type) {
			حالة FmtPart::LITERAL:
				str += part.str;
				اكسر;

			حالة FmtPart::UNICHAR: {
				RTLIL::Const value = part.sig.as_const();
				uint32_t codepoint = value.as_int();
				אם (codepoint >= 0x10000)
					str += (حرف)(0xf0 |  (codepoint >> 18));
				אחרת אם (codepoint >= 0x800)
					str += (حرف)(0xe0 |  (codepoint >> 12));
				אחרת אם (codepoint >= 0x80)
					str += (حرف)(0xc0 |  (codepoint >>  6));
				אחרת
					str += (حرف)codepoint;
				אם (codepoint >= 0x10000)
					str += (حرف)(0x80 | ((codepoint >> 12) & 0x3f));
				אם (codepoint >= 0x800)
					str += (حرف)(0x80 | ((codepoint >>  6) & 0x3f));
				אם (codepoint >= 0x80)
					str += (حرف)(0x80 | ((codepoint >>  0) & 0x3f));
				اكسر;
			}

			حالة FmtPart::INTEGER:
			حالة FmtPart::STRING:
			حالة FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				אם (part.type == FmtPart::INTEGER) {
					RTLIL::Const value = part.sig.as_const();
					منطقي has_x = שקר, all_x = אמת, has_z = שקר, all_z = אמת;
					עבור (State bit : value) {
						אם (bit == State::Sx)
							has_x = אמת;
						אחרת
							all_x = שקר;
						אם (bit == State::Sz)
							has_z = אמת;
						אחרת
							all_z = שקר;
					}

					אם (!has_z && !has_x && part.signed_ && value[value.size() - 1]) {
						prefix = "-";
						value = RTLIL::const_neg(value, {}, part.signed_, {}, value.size() + 1);
					} אחרת {
						بدل (part.sign) {
							حالة FmtPart::MINUS:       اكسر;
							حالة FmtPart::PLUS_MINUS:  prefix = "+"; اكسر;
							حالة FmtPart::SPACE_MINUS: prefix = " "; اكسر;
						}
					}

					אם (part.base != 10) {
						size_t minimum_size = 1;
						עבור (size_t index = 0; index < (size_t)value.size(); index++)
							אם (value[index] != State::S0)
								minimum_size = index + 1;
						value = value.extract(0, minimum_size);
					}

					אם (part.base == 2) {
						אם (part.show_base)
							prefix += "0b";
						עבור (size_t index = 0; index < (size_t)value.size(); index++) {
							אם (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = value[index];
							אם (bit == State::Sx)
								buf += 'x';
							אחרת אם (bit == State::Sz)
								buf += 'z';
							אחרת אם (bit == State::S1)
								buf += '1';
							אחרת /* if (bit == State::S0) */
								buf += '0';
						}
					} אחרת אם (part.base == 8 || part.base == 16) {
						אם (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						עבור (size_t index = 0; index < (size_t)value.size(); index += step) {
							אם (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = value.extract(index, min(step, value.size() - index));
							منطقي has_x = שקר, all_x = אמת, has_z = שקר, all_z = אמת;
							עבור (State bit : subvalue) {
								אם (bit == State::Sx)
									has_x = אמת;
								אחרת
									all_x = שקר;
								אם (bit == State::Sz)
									has_z = אמת;
								אחרת
									all_z = שקר;
							}
							אם (all_x)
								buf += 'x';
							אחרת אם (all_z)
								buf += 'z';
							אחרת אם (has_x)
								buf += 'X';
							אחרת אם (has_z)
								buf += 'Z';
							אחרת {
								ثابت حرف *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} אחרת אם (part.base == 10) {
						אם (part.show_base)
							prefix += "0d";
						אם (all_x)
							buf += 'x';
						אחרת אם (all_z)
							buf += 'z';
						אחרת אם (has_x)
							buf += 'X';
						אחרת אם (has_z)
							buf += 'Z';
						אחרת {
							log_assert(value.is_fully_def());
							אם (value.is_fully_zero())
								buf += '0';
							size_t index = 0;
							כלעוד (!value.is_fully_zero())	{
								אם (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(value, 10, שקר, שקר, 4).as_int();
								value = RTLIL::const_div(value, 10, שקר, שקר, value.size());
								index++;
							}
						}
					} אחרת log_abort();
					אם (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						כלעוד (prefix.size() + buf.size() < part.width) {
							אם (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} אחרת אם (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} אחרת אם (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				אם (prefix.size() + buf.size() < part.width) {
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
				} אחרת {
					str += prefix;
					str += buf;
				}
				اكسر;
			}
		}
	}

	החזר str;
}
