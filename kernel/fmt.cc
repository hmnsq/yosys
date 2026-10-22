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
		𐤀𐤌 (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} وإلا 𐤀𐤌 (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} وإلا 𐤀𐤌 (fmt[i] == '}') {
			log_assert(خطأ && "Unexpected '}' in format string");
		} وإلا 𐤀𐤌 (fmt[i] == '{') {
			𐤀𐤌 (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.push_back(part);
				part = {};
			}

			𐤀𐤌 (++i == fmt.size())
				log_assert(خطأ && "Unexpected end in format substitution");

			size_t arg_size = 0;
			لكل (; i < fmt.size(); i++) {
				𐤀𐤌 (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} وإلا 𐤀𐤌 (fmt[i] == ':') {
					++i;
					اكسر;
				} وإلا {
					log_assert(خطأ && "Unexpected character in format substitution");
				}
			}
			𐤀𐤌 (i == fmt.size())
				log_assert(خطأ && "Unexpected end in format substitution");

			𐤀𐤌 ((size_t)args.size() < arg_size)
				log_assert(خطأ && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			𐤀𐤌 (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				اذهب success;
			}

			𐤀𐤌 (fmt[i] == '>')
				part.justify = FmtPart::RIGHT;
			وإلا 𐤀𐤌 (fmt[i] == '<')
				part.justify = FmtPart::LEFT;
			وإلا 𐤀𐤌 (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			وإلا
				log_assert(خطأ && "Unexpected justification in format substitution");
			𐤀𐤌 (++i == fmt.size())
				log_assert(خطأ && "Unexpected end in format substitution");

			part.padding = fmt[i];
			𐤀𐤌 (++i == fmt.size())
				log_assert(خطأ && "Unexpected end in format substitution");

			لكل (; i < fmt.size(); i++) {
				𐤀𐤌 (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					استمر;
				} وإلا 𐤀𐤌 (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} وإلا 𐤀𐤌 (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} وإلا 𐤀𐤌 (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} وإلا 𐤀𐤌 (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} وإلا 𐤀𐤌 (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = true;
				} وإلا 𐤀𐤌 (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} وإلا 𐤀𐤌 (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} وإلا 𐤀𐤌 (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = true;
				} وإلا {
					log_assert(خطأ && "Unexpected character in format substitution");
				}
				++i;
				اكسر;
			}
			𐤀𐤌 (i == fmt.size())
				log_assert(خطأ && "Unexpected end in format substitution");

			𐤀𐤌 (part.type == FmtPart::INTEGER) {
				𐤀𐤌 (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					𐤀𐤌 (++i == fmt.size())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا 𐤀𐤌 (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					𐤀𐤌 (++i == fmt.size())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا 𐤀𐤌 (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					𐤀𐤌 (++i == fmt.size())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا {
					// also accept no sign character and treat like MINUS for compatibility
				}

				𐤀𐤌 (fmt[i] == '#') {
					part.show_base = true;
					++i;
				}
				𐤀𐤌 (fmt[i] == '_') {
					part.group = true;
					++i;
				}

				𐤀𐤌 (fmt[i] == 'u')
					part.signed_ = خطأ;
				وإلا 𐤀𐤌 (fmt[i] == 's')
					part.signed_ = true;
				وإلا
					log_assert(خطأ && "Unexpected character in format substitution");
				𐤀𐤌 (++i == fmt.size())
					log_assert(خطأ && "Unexpected end in format substitution");
			}

		success:
			𐤀𐤌 (fmt[i] != '}')
				log_assert(خطأ && "Expected '}' after format substitution");

			parts.push_back(part);
			part = {};
		} وإلا {
			part.str += fmt[i];
		}
	}
	𐤀𐤌 (!part.str.empty()) {
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
					𐤀𐤌 (c == '{')
						fmt += "{{";
					وإلا 𐤀𐤌 (c == '}')
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
				𐤀𐤌 (part.justify == FmtPart::RIGHT)
					fmt += '>';
				وإلا 𐤀𐤌 (part.justify == FmtPart::LEFT)
					fmt += '<';
				وإلا 𐤀𐤌 (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				وإلا log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				𐤀𐤌 (part.width > 0)
					fmt += std::to_string(part.width);
				𐤀𐤌 (part.type == FmtPart::INTEGER) {
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
				} وإلا 𐤀𐤌 (part.type == FmtPart::STRING) {
					fmt += 'c';
				} وإلا 𐤀𐤌 (part.type == FmtPart::VLOG_TIME) {
					𐤀𐤌 (part.realtime)
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
	𐤀𐤌 (!signed_)
		max.setBit(size, true);
	وإلا
		max.setBit(size - 1, true);
	size_t places = 0;
	طالما (!max.isZero()) {
		places++;
		max /= 10;
	}
	𐤀𐤌 (signed_)
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
	𐤀𐤌 (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.push_back(part);
		أرجع;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	𐤀𐤌 (part.width < places) {
		part.justify = FmtPart::RIGHT;
		part.width = places;
		parts.push_back(part);
	} وإلا 𐤀𐤌 (part.width == places) {
		parts.push_back(part);
	} وإلا 𐤀𐤌 (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		𐤀𐤌 (part.justify == FmtPart::RIGHT) {
			append_literal(gap);
			parts.push_back(part);
		} وإلا {
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
				𐤀𐤌 (arg == args.begin() || !sformat_like) {
					ثابت auto fmtarg = arg;
					ثابت std::string &fmt = fmtarg->str;
					FmtPart part = {};
					لكل (size_t i = 0; i < fmt.size(); i++) {
						𐤀𐤌 (fmt[i] != '%') {
							part.str += fmt[i];
						} وإلا 𐤀𐤌 (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} وإلا 𐤀𐤌 (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} وإلا 𐤀𐤌 (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} وإلا {
							𐤀𐤌 (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.push_back(part);
								part = {};
							}
							𐤀𐤌 (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							𐤀𐤌 (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							لكل (; i < fmt.size(); i++) {
								𐤀𐤌 (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::LEFT;
								} وإلا 𐤀𐤌 (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} وإلا اكسر;
							}
							𐤀𐤌 (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							منطقي has_leading_zero = خطأ, has_width = خطأ;
							لكل (; i < fmt.size(); i++) {
								𐤀𐤌 (fmt[i] >= '0' && fmt[i] <= '9') {
									𐤀𐤌 (fmt[i] == '0' && !has_width) {
										has_leading_zero = true;
									} وإلا {
										has_width = true;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									استمر;
								} وإلا 𐤀𐤌 (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} وإلا 𐤀𐤌 (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} وإلا 𐤀𐤌 (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} وإلا 𐤀𐤌 (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} وإلا 𐤀𐤌 (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} وإلا 𐤀𐤌 (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									𐤀𐤌 ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} وإلا 𐤀𐤌 (fmt[i] == 't' || fmt[i] == 'T') {
									𐤀𐤌 (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										𐤀𐤌 (!has_width && !has_leading_zero)
											part.width = 20;
									} وإلا {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} وإلا {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								اكسر;
							}
							𐤀𐤌 (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							𐤀𐤌 (part.padding == '\0') {
								𐤀𐤌 (has_leading_zero && part.justify == FmtPart::RIGHT) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} وإلا {
									part.padding = ' ';
								}
							}

							𐤀𐤌 (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							𐤀𐤌 (part.base != 10)
								part.signed_ = خطأ;
							𐤀𐤌 (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							وإلا
								parts.push_back(part);
							part = {};
						}
					}
					𐤀𐤌 (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.push_back(part);
					}
				} وإلا {
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
					𐤀𐤌 (c == '%')
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
				args.push_back(arg);

				fmt.str += '%';
				𐤀𐤌 (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				𐤀𐤌 (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				𐤀𐤌 (part.width == 0) {
					fmt.str += '0';
				} وإلا 𐤀𐤌 (part.width > 0) {
					𐤀𐤌 (part.base != 10 || part.padding == '0')
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
				𐤀𐤌 (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				𐤀𐤌 (part.sig.size() == 8) {
					𐤀𐤌 (part.width > 0) {
						𐤀𐤌 (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} وإلا {
					log_assert(part.sig.size() % 8 == 0);
					𐤀𐤌 (part.width > 0) {
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
				𐤀𐤌 (part.realtime)
					arg.realtime = true;
				args.push_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				𐤀𐤌 (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				𐤀𐤌 (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				𐤀𐤌 (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				اكسر;
			}

			افتراضي: log_abort();
		}
	}

	args.insert(args.begin(), fmt);
	أرجع args;
}

std::string escape_cxx_string(ثابت std::string &input)
{
	std::string output = "\"";
	لكل (auto c : input) {
		𐤀𐤌 (::isprint(c)) {
			𐤀𐤌 (c == '\\' || c == '"')
				output.push_back('\\');
			output.push_back(c);
		} وإلا {
			حرف l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.push_back('\\');
			output.push_back('0' + h);
			output.push_back('0' + m);
			output.push_back('0' + l);
		}
	}
	output.push_back('"');
	𐤀𐤌 (output.find('\0') != std::string::npos) {
		output.insert(0, "std::string {");
		output.append(stringf(", %zu}", input.size()));
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
				𐤀𐤌 (codepoint >= 0x10000)
					str += (حرف)(0xf0 |  (codepoint >> 18));
				وإلا 𐤀𐤌 (codepoint >= 0x800)
					str += (حرف)(0xe0 |  (codepoint >> 12));
				وإلا 𐤀𐤌 (codepoint >= 0x80)
					str += (حرف)(0xc0 |  (codepoint >>  6));
				وإلا
					str += (حرف)codepoint;
				𐤀𐤌 (codepoint >= 0x10000)
					str += (حرف)(0x80 | ((codepoint >> 12) & 0x3f));
				𐤀𐤌 (codepoint >= 0x800)
					str += (حرف)(0x80 | ((codepoint >>  6) & 0x3f));
				𐤀𐤌 (codepoint >= 0x80)
					str += (حرف)(0x80 | ((codepoint >>  0) & 0x3f));
				اكسر;
			}

			حالة FmtPart::INTEGER:
			حالة FmtPart::STRING:
			حالة FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				𐤀𐤌 (part.type == FmtPart::INTEGER) {
					RTLIL::Const value = part.sig.as_const();
					منطقي has_x = خطأ, all_x = true, has_z = خطأ, all_z = true;
					لكل (State bit : value) {
						𐤀𐤌 (bit == State::Sx)
							has_x = true;
						وإلا
							all_x = خطأ;
						𐤀𐤌 (bit == State::Sz)
							has_z = true;
						وإلا
							all_z = خطأ;
					}

					𐤀𐤌 (!has_z && !has_x && part.signed_ && value[value.size() - 1]) {
						prefix = "-";
						value = RTLIL::const_neg(value, {}, part.signed_, {}, value.size() + 1);
					} وإلا {
						بدل (part.sign) {
							حالة FmtPart::MINUS:       اكسر;
							حالة FmtPart::PLUS_MINUS:  prefix = "+"; اكسر;
							حالة FmtPart::SPACE_MINUS: prefix = " "; اكسر;
						}
					}

					𐤀𐤌 (part.base != 10) {
						size_t minimum_size = 1;
						لكل (size_t index = 0; index < (size_t)value.size(); index++)
							𐤀𐤌 (value[index] != State::S0)
								minimum_size = index + 1;
						value = value.extract(0, minimum_size);
					}

					𐤀𐤌 (part.base == 2) {
						𐤀𐤌 (part.show_base)
							prefix += "0b";
						لكل (size_t index = 0; index < (size_t)value.size(); index++) {
							𐤀𐤌 (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = value[index];
							𐤀𐤌 (bit == State::Sx)
								buf += 'x';
							وإلا 𐤀𐤌 (bit == State::Sz)
								buf += 'z';
							وإلا 𐤀𐤌 (bit == State::S1)
								buf += '1';
							وإلا /* if (bit == State::S0) */
								buf += '0';
						}
					} وإلا 𐤀𐤌 (part.base == 8 || part.base == 16) {
						𐤀𐤌 (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						لكل (size_t index = 0; index < (size_t)value.size(); index += step) {
							𐤀𐤌 (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = value.extract(index, min(step, value.size() - index));
							منطقي has_x = خطأ, all_x = true, has_z = خطأ, all_z = true;
							لكل (State bit : subvalue) {
								𐤀𐤌 (bit == State::Sx)
									has_x = true;
								وإلا
									all_x = خطأ;
								𐤀𐤌 (bit == State::Sz)
									has_z = true;
								وإلا
									all_z = خطأ;
							}
							𐤀𐤌 (all_x)
								buf += 'x';
							وإلا 𐤀𐤌 (all_z)
								buf += 'z';
							وإلا 𐤀𐤌 (has_x)
								buf += 'X';
							وإلا 𐤀𐤌 (has_z)
								buf += 'Z';
							وإلا {
								ثابت حرف *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} وإلا 𐤀𐤌 (part.base == 10) {
						𐤀𐤌 (part.show_base)
							prefix += "0d";
						𐤀𐤌 (all_x)
							buf += 'x';
						وإلا 𐤀𐤌 (all_z)
							buf += 'z';
						وإلا 𐤀𐤌 (has_x)
							buf += 'X';
						وإلا 𐤀𐤌 (has_z)
							buf += 'Z';
						وإلا {
							log_assert(value.is_fully_def());
							𐤀𐤌 (value.is_fully_zero())
								buf += '0';
							size_t index = 0;
							طالما (!value.is_fully_zero())	{
								𐤀𐤌 (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(value, 10, خطأ, خطأ, 4).as_int();
								value = RTLIL::const_div(value, 10, خطأ, خطأ, value.size());
								index++;
							}
						}
					} وإلا log_abort();
					𐤀𐤌 (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						طالما (prefix.size() + buf.size() < part.width) {
							𐤀𐤌 (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} وإلا 𐤀𐤌 (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} وإلا 𐤀𐤌 (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				𐤀𐤌 (prefix.size() + buf.size() < part.width) {
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
