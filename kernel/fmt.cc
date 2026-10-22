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

خالی Fmt::append_literal(ثابت std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.ڈالو_عقب(part);
}

خالی Fmt::parse_rtlil(ثابت RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec دلائل_2 = cell->get_پورٹ(ID(دلائل));
	parts.clear();

	FmtPart part;
	برائے (سائز_t i = 0; i < fmt.سائز(); i++) {
		اگر (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} ورنہ اگر (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} ورنہ اگر (fmt[i] == '}') {
			log_assert(جھوٹ && "Unexpected '}' in format string");
		} ورنہ اگر (fmt[i] == '{') {
			اگر (!part.str.خالی_خانے()) {
				part.type = FmtPart::LITERAL;
				parts.ڈالو_عقب(part);
				part = {};
			}

			اگر (++i == fmt.سائز())
				log_assert(جھوٹ && "Unexpected end in format substitution");

			سائز_t arg_سائز = 0;
			برائے (; i < fmt.سائز(); i++) {
				اگر (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_سائز *= 10;
					arg_سائز += fmt[i] - '0';
				} ورنہ اگر (fmt[i] == ':') {
					++i;
					توڑو;
				} ورنہ {
					log_assert(جھوٹ && "Unexpected character in format substitution");
				}
			}
			اگر (i == fmt.سائز())
				log_assert(جھوٹ && "Unexpected end in format substitution");

			اگر ((سائز_t)دلائل_2.سائز() < arg_سائز)
				log_assert(جھوٹ && "Format part overruns arguments");
			part.sig = دلائل_2.نکالو_2(0, arg_سائز);
			دلائل_2.remove(0, arg_سائز);

			اگر (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				جاؤ success;
			}

			اگر (fmt[i] == '>')
				part.justify = FmtPart::دایاں;
			ورنہ اگر (fmt[i] == '<')
				part.justify = FmtPart::بایاں;
			ورنہ اگر (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			ورنہ
				log_assert(جھوٹ && "Unexpected justification in format substitution");
			اگر (++i == fmt.سائز())
				log_assert(جھوٹ && "Unexpected end in format substitution");

			part.padding = fmt[i];
			اگر (++i == fmt.سائز())
				log_assert(جھوٹ && "Unexpected end in format substitution");

			برائے (; i < fmt.سائز(); i++) {
				اگر (fmt[i] >= '0' && fmt[i] <= '9') {
					part.چوڑائی *= 10;
					part.چوڑائی += fmt[i] - '0';
					جاری;
				} ورنہ اگر (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.بنیاد = 2;
				} ورنہ اگر (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.بنیاد = 8;
				} ورنہ اگر (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.بنیاد = 10;
				} ورنہ اگر (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.بنیاد = 16;
				} ورنہ اگر (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.بنیاد = 16;
					part.hex_بالائی = سچ;
				} ورنہ اگر (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} ورنہ اگر (fmt[i] == 't') {
					part.type = FmtPart::VLOG_وقت;
				} ورنہ اگر (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_وقت;
					part.realtime = سچ;
				} ورنہ {
					log_assert(جھوٹ && "Unexpected character in format substitution");
				}
				++i;
				توڑو;
			}
			اگر (i == fmt.سائز())
				log_assert(جھوٹ && "Unexpected end in format substitution");

			اگر (part.type == FmtPart::INTEGER) {
				اگر (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					اگر (++i == fmt.سائز())
						log_assert(جھوٹ && "Unexpected end in format substitution");
				} ورنہ اگر (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					اگر (++i == fmt.سائز())
						log_assert(جھوٹ && "Unexpected end in format substitution");
				} ورنہ اگر (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					اگر (++i == fmt.سائز())
						log_assert(جھوٹ && "Unexpected end in format substitution");
				} ورنہ {
					// also accept no sign character and treat like MINUS for compatibility
				}

				اگر (fmt[i] == '#') {
					part.show_بنیاد = سچ;
					++i;
				}
				اگر (fmt[i] == '_') {
					part.گروہ = سچ;
					++i;
				}

				اگر (fmt[i] == 'u')
					part.signed_ = جھوٹ;
				ورنہ اگر (fmt[i] == 's')
					part.signed_ = سچ;
				ورنہ
					log_assert(جھوٹ && "Unexpected character in format substitution");
				اگر (++i == fmt.سائز())
					log_assert(جھوٹ && "Unexpected end in format substitution");
			}

		success:
			اگر (fmt[i] != '}')
				log_assert(جھوٹ && "Expected '}' after format substitution");

			parts.ڈالو_عقب(part);
			part = {};
		} ورنہ {
			part.str += fmt[i];
		}
	}
	اگر (!part.str.خالی_خانے()) {
		part.type = FmtPart::LITERAL;
		parts.ڈالو_عقب(part);
	}
}

خالی Fmt::emit_rtlil(RTLIL::Cell *cell) ثابت {
	std::string fmt;
	RTLIL::SigSpec دلائل_2;

	برائے (خودکار &part : parts) {
		انتخاب (part.type) {
			حالت FmtPart::LITERAL:
				برائے (حرف c : part.str) {
					اگر (c == '{')
						fmt += "{{";
					ورنہ اگر (c == '}')
						fmt += "}}";
					ورنہ
						fmt += c;
				}
				توڑو;

			حالت FmtPart::UNICHAR:
				log_assert(part.sig.سائز() <= 32);
				fmt += "{U}";
				توڑو;

			حالت FmtPart::VLOG_وقت:
				log_assert(part.sig.سائز() == 0);
				YS_FALLTHROUGH
			حالت FmtPart::STRING:
				log_assert(part.sig.سائز() % 8 == 0);
				YS_FALLTHROUGH
			حالت FmtPart::INTEGER:
				دلائل_2.append(part.sig);
				fmt += '{';
				fmt += std::مقصد_string(part.sig.سائز());
				fmt += ':';
				اگر (part.justify == FmtPart::دایاں)
					fmt += '>';
				ورنہ اگر (part.justify == FmtPart::بایاں)
					fmt += '<';
				ورنہ اگر (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				ورنہ log_abort();
				log_assert(part.چوڑائی == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				اگر (part.چوڑائی > 0)
					fmt += std::مقصد_string(part.چوڑائی);
				اگر (part.type == FmtPart::INTEGER) {
					انتخاب (part.بنیاد) {
						حالت  2: fmt += 'b'; توڑو;
						حالت  8: fmt += 'o'; توڑو;
						حالت 10: fmt += 'd'; توڑو;
						حالت 16: fmt += part.hex_بالائی ? 'H' : 'h'; توڑو;
						طےشدہ: log_abort();
					}
					انتخاب (part.sign) {
						حالت FmtPart::MINUS:       fmt += '-'; توڑو;
						حالت FmtPart::PLUS_MINUS:  fmt += '+'; توڑو;
						حالت FmtPart::SPACE_MINUS: fmt += ' '; توڑو;
					}
					fmt += part.show_بنیاد ? "#" : "";
					fmt += part.گروہ ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} ورنہ اگر (part.type == FmtPart::STRING) {
					fmt += 'c';
				} ورنہ اگر (part.type == FmtPart::VLOG_وقت) {
					اگر (part.realtime)
						fmt += 'r';
					ورنہ
						fmt += 't';
				} ورنہ log_abort();
				fmt += '}';
				توڑو;

			طےشدہ: log_abort();
		}
	}

	cell->سیٹ_Param(ID(FORMAT), fmt);
	cell->سیٹ_Param(ID(دلائل_چوڑائی), دلائل_2.سائز());
	cell->سیٹ_پورٹ(ID(دلائل), دلائل_2);
}

ساکن سائز_t compute_required_decimal_places(سائز_t سائز, منطقی signed_)
{
	BigUnsigned max;
	اگر (!signed_)
		max.سیٹ_ثنائی_ہندسہ(سائز, سچ);
	ورنہ
		max.سیٹ_ثنائی_ہندسہ(سائز - 1, سچ);
	سائز_t places = 0;
	جب (!max.isZero()) {
		places++;
		max /= 10;
	}
	اگر (signed_)
		places++;
	واپس places;
}

ساکن سائز_t compute_required_nondecimal_places(سائز_t سائز, غیرعلامتی بنیاد)
{
	log_assert(بنیاد != 10);
	BigUnsigned max;
	max.سیٹ_ثنائی_ہندسہ(سائز - 1, سچ);
	سائز_t places = 0;
	جب (!max.isZero()) {
		places++;
		max /= بنیاد;
	}
	واپس places;
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
خالی Fmt::apply_verilog_automatic_sizing_and_جوڑو(FmtPart &part)
{
	اگر (part.بنیاد == 10) {
		سائز_t places = compute_required_decimal_places(part.sig.سائز(), part.signed_);
		part.padding = ' ';
		part.چوڑائی = std::max(part.چوڑائی, places);
		parts.ڈالو_عقب(part);
		واپس;
	}

	part.padding = '0';

	سائز_t places = compute_required_nondecimal_places(part.sig.سائز(), part.بنیاد);
	اگر (part.چوڑائی < places) {
		part.justify = FmtPart::دایاں;
		part.چوڑائی = places;
		parts.ڈالو_عقب(part);
	} ورنہ اگر (part.چوڑائی == places) {
		parts.ڈالو_عقب(part);
	} ورنہ اگر (part.چوڑائی > places) {
		خودکار gap = std::string(part.چوڑائی - places, ' ');
		part.چوڑائی = places;

		اگر (part.justify == FmtPart::دایاں) {
			append_literal(gap);
			parts.ڈالو_عقب(part);
		} ورنہ {
			part.justify = FmtPart::دایاں;
			parts.ڈالو_عقب(part);
			append_literal(gap);
		}
	}
}

خالی Fmt::parse_verilog(ثابت std::سمتیہ<VerilogFmtArg> &دلائل_2, منطقی sformat_like, صحیح default_بنیاد, ثابت std::string &task_name, ثابت std::string &module_name)
{
	parts.clear();

	خودکار arg = دلائل_2.begin();
	برائے (; arg != دلائل_2.انتہا(); ++arg) {
		انتخاب (arg->type) {
			حالت VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.بنیاد = default_بنیاد;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_جوڑو(part);
				توڑو;
			}

			حالت VerilogFmtArg::وقت: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_وقت;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.چوڑائی = 20;
				parts.ڈالو_عقب(part);
				توڑو;
			}

			حالت VerilogFmtArg::STRING: {
				اگر (arg == دلائل_2.begin() || !sformat_like) {
					ثابت خودکار fmtarg = arg;
					ثابت std::string &fmt = fmtarg->str;
					FmtPart part = {};
					برائے (سائز_t i = 0; i < fmt.سائز(); i++) {
						اگر (fmt[i] != '%') {
							part.str += fmt[i];
						} ورنہ اگر (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} ورنہ اگر (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} ورنہ اگر (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} ورنہ {
							اگر (!part.str.خالی_خانے()) {
								part.type = FmtPart::LITERAL;
								parts.ڈالو_عقب(part);
								part = {};
							}
							اگر (++i == fmt.سائز()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - دلائل_2.begin() + 1);
							}

							اگر (++arg == دلائل_2.انتہا()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - دلائل_2.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							برائے (; i < fmt.سائز(); i++) {
								اگر (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::بایاں;
								} ورنہ اگر (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} ورنہ توڑو;
							}
							اگر (i == fmt.سائز()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - دلائل_2.begin() + 1);
							}

							منطقی has_leading_zero = جھوٹ, has_چوڑائی = جھوٹ;
							برائے (; i < fmt.سائز(); i++) {
								اگر (fmt[i] >= '0' && fmt[i] <= '9') {
									اگر (fmt[i] == '0' && !has_چوڑائی) {
										has_leading_zero = سچ;
									} ورنہ {
										has_چوڑائی = سچ;
										part.چوڑائی *= 10;
										part.چوڑائی += fmt[i] - '0';
									}
									جاری;
								} ورنہ اگر (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.بنیاد =  2;
								} ورنہ اگر (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.بنیاد =  8;
								} ورنہ اگر (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.بنیاد = 10;
								} ورنہ اگر (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.بنیاد = 16;
								} ورنہ اگر (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} ورنہ اگر (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									اگر ((part.sig.سائز() % 8) != 0)
										part.sig.extend_u0((part.sig.سائز() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} ورنہ اگر (fmt[i] == 't' || fmt[i] == 'T') {
									اگر (arg->type == VerilogFmtArg::وقت) {
										part.type = FmtPart::VLOG_وقت;
										part.realtime = arg->realtime;
										اگر (!has_چوڑائی && !has_leading_zero)
											part.چوڑائی = 20;
									} ورنہ {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - دلائل_2.begin() + 1);
									}
								} ورنہ {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - دلائل_2.begin() + 1);
								}
								توڑو;
							}
							اگر (i == fmt.سائز()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - دلائل_2.begin() + 1);
							}

							اگر (part.padding == '\0') {
								اگر (has_leading_zero && part.justify == FmtPart::دایاں) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} ورنہ {
									part.padding = ' ';
								}
							}

							اگر (part.type == FmtPart::INTEGER && part.بنیاد != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - دلائل_2.begin() + 1);

							اگر (part.بنیاد != 10)
								part.signed_ = جھوٹ;
							اگر (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_جوڑو(part);
							ورنہ
								parts.ڈالو_عقب(part);
							part = {};
						}
					}
					اگر (!part.str.خالی_خانے()) {
						part.type = FmtPart::LITERAL;
						parts.ڈالو_عقب(part);
					}
				} ورنہ {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.ڈالو_عقب(part);
				}
				توڑو;
			}

			طےشدہ: log_abort();
		}
	}
}

std::سمتیہ<VerilogFmtArg> Fmt::emit_verilog() ثابت
{
	std::سمتیہ<VerilogFmtArg> دلائل_2;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	برائے (خودکار &part : parts) {
		انتخاب (part.type) {
			حالت FmtPart::LITERAL:
				برائے (حرف c : part.str) {
					اگر (c == '%')
						fmt.str += "%%";
					ورنہ
						fmt.str += c;
				}
				توڑو;

			حالت FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				دلائل_2.ڈالو_عقب(arg);

				fmt.str += '%';
				اگر (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				اگر (part.justify == FmtPart::بایاں)
					fmt.str += '-';
				اگر (part.چوڑائی == 0) {
					fmt.str += '0';
				} ورنہ اگر (part.چوڑائی > 0) {
					اگر (part.بنیاد != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::مقصد_string(part.چوڑائی);
				}
				انتخاب (part.بنیاد) {
					حالت  2: fmt.str += 'b'; توڑو;
					حالت  8: fmt.str += 'o'; توڑو;
					حالت 10: fmt.str += 'd'; توڑو;
					حالت 16: fmt.str += 'h'; توڑو; // treat uppercase hex as lowercase
					طےشدہ: log_abort();
				}
				توڑو;
			}

			حالت FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				دلائل_2.ڈالو_عقب(arg);

				fmt.str += '%';
				اگر (part.justify == FmtPart::بایاں)
					fmt.str += '-';
				اگر (part.sig.سائز() == 8) {
					اگر (part.چوڑائی > 0) {
						اگر (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::مقصد_string(part.چوڑائی);
					}
					fmt.str += 'c';
				} ورنہ {
					log_assert(part.sig.سائز() % 8 == 0);
					اگر (part.چوڑائی > 0) {
						fmt.str += std::مقصد_string(part.چوڑائی);
					}
					fmt.str += 's';
				}
				توڑو;
			}

			حالت FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.نکالو_2(0, 7); // only ASCII
				دلائل_2.ڈالو_عقب(arg);

				fmt.str += "%c";
				توڑو;
			}

			حالت FmtPart::VLOG_وقت: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::وقت;
				اگر (part.realtime)
					arg.realtime = سچ;
				دلائل_2.ڈالو_عقب(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				اگر (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				اگر (part.justify == FmtPart::بایاں)
					fmt.str += '-';
				اگر (part.padding == '0' && part.چوڑائی > 0)
					fmt.str += '0';
				fmt.str += std::مقصد_string(part.چوڑائی);
				fmt.str += 't';
				توڑو;
			}

			طےشدہ: log_abort();
		}
	}

	دلائل_2.داخل_کرو(دلائل_2.begin(), fmt);
	واپس دلائل_2;
}

std::string escape_cxx_string(ثابت std::string &داخلہ)
{
	std::string output = "\"";
	برائے (خودکار c : داخلہ) {
		اگر (::isprint(c)) {
			اگر (c == '\\' || c == '"')
				output.ڈالو_عقب('\\');
			output.ڈالو_عقب(c);
		} ورنہ {
			حرف l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.ڈالو_عقب('\\');
			output.ڈالو_عقب('0' + h);
			output.ڈالو_عقب('0' + m);
			output.ڈالو_عقب('0' + l);
		}
	}
	output.ڈالو_عقب('"');
	اگر (output.ڈھونڈو('\0') != std::string::npos) {
		output.داخل_کرو(0, "std::string {");
		output.append(stringf(", %zu}", داخلہ.سائز()));
	}
	واپس output;
}

خالی Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<خالی(ثابت RTLIL::SigSpec &)> emit_sig, ثابت std::string &context) ثابت
{
	os << indent << "std::string buf;\n";
	برائے (خودکار &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		انتخاب (part.type) {
			حالت FmtPart::LITERAL:   os << "LITERAL";   توڑو;
			حالت FmtPart::INTEGER:   os << "INTEGER";   توڑو;
			حالت FmtPart::STRING:    os << "STRING";    توڑو;
			حالت FmtPart::UNICHAR:   os << "UNICHAR";   توڑو;
			حالت FmtPart::VLOG_وقت: os << "VLOG_TIME"; توڑو;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		انتخاب (part.justify) {
			حالت FmtPart::بایاں:    os << "LEFT";    توڑو;
			حالت FmtPart::دایاں:   os << "RIGHT";   توڑو;
			حالت FmtPart::NUMERIC: os << "NUMERIC"; توڑو;
		}
		os << ", ";
		os << "(char)" << (صحیح)part.padding << ", ";
		os << part.چوڑائی << ", ";
		os << part.بنیاد << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		انتخاب (part.sign) {
			حالت FmtPart::MINUS:       os << "MINUS";       توڑو;
			حالت FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  توڑو;
			حالت FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; توڑو;
		}
		os << ", ";
		os << part.hex_بالائی << ", ";
		os << part.show_بنیاد << ", ";
		os << part.گروہ << ", ";
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

	برائے (خودکار &part : parts) {
		انتخاب (part.type) {
			حالت FmtPart::LITERAL:
				str += part.str;
				توڑو;

			حالت FmtPart::UNICHAR: {
				RTLIL::Const قدر = part.sig.as_const();
				uint32_t codepoint = قدر.as_int();
				اگر (codepoint >= 0x10000)
					str += (حرف)(0xf0 |  (codepoint >> 18));
				ورنہ اگر (codepoint >= 0x800)
					str += (حرف)(0xe0 |  (codepoint >> 12));
				ورنہ اگر (codepoint >= 0x80)
					str += (حرف)(0xc0 |  (codepoint >>  6));
				ورنہ
					str += (حرف)codepoint;
				اگر (codepoint >= 0x10000)
					str += (حرف)(0x80 | ((codepoint >> 12) & 0x3f));
				اگر (codepoint >= 0x800)
					str += (حرف)(0x80 | ((codepoint >>  6) & 0x3f));
				اگر (codepoint >= 0x80)
					str += (حرف)(0x80 | ((codepoint >>  0) & 0x3f));
				توڑو;
			}

			حالت FmtPart::INTEGER:
			حالت FmtPart::STRING:
			حالت FmtPart::VLOG_وقت: {
				std::string buf;
				std::string سابقہ_2;
				اگر (part.type == FmtPart::INTEGER) {
					RTLIL::Const قدر = part.sig.as_const();
					منطقی has_x = جھوٹ, all_x = سچ, has_z = جھوٹ, all_z = سچ;
					برائے (State ثنائی_ہندسہ : قدر) {
						اگر (ثنائی_ہندسہ == State::Sx)
							has_x = سچ;
						ورنہ
							all_x = جھوٹ;
						اگر (ثنائی_ہندسہ == State::Sz)
							has_z = سچ;
						ورنہ
							all_z = جھوٹ;
					}

					اگر (!has_z && !has_x && part.signed_ && قدر[قدر.سائز() - 1]) {
						سابقہ_2 = "-";
						قدر = RTLIL::const_neg(قدر, {}, part.signed_, {}, قدر.سائز() + 1);
					} ورنہ {
						انتخاب (part.sign) {
							حالت FmtPart::MINUS:       توڑو;
							حالت FmtPart::PLUS_MINUS:  سابقہ_2 = "+"; توڑو;
							حالت FmtPart::SPACE_MINUS: سابقہ_2 = " "; توڑو;
						}
					}

					اگر (part.بنیاد != 10) {
						سائز_t minimum_سائز = 1;
						برائے (سائز_t اشاریہ = 0; اشاریہ < (سائز_t)قدر.سائز(); اشاریہ++)
							اگر (قدر[اشاریہ] != State::S0)
								minimum_سائز = اشاریہ + 1;
						قدر = قدر.نکالو_2(0, minimum_سائز);
					}

					اگر (part.بنیاد == 2) {
						اگر (part.show_بنیاد)
							سابقہ_2 += "0b";
						برائے (سائز_t اشاریہ = 0; اشاریہ < (سائز_t)قدر.سائز(); اشاریہ++) {
							اگر (part.گروہ && اشاریہ > 0 && اشاریہ % 4 == 0)
								buf += '_';
							RTLIL::State ثنائی_ہندسہ = قدر[اشاریہ];
							اگر (ثنائی_ہندسہ == State::Sx)
								buf += 'x';
							ورنہ اگر (ثنائی_ہندسہ == State::Sz)
								buf += 'z';
							ورنہ اگر (ثنائی_ہندسہ == State::S1)
								buf += '1';
							ورنہ /* if (bit == State::S0) */
								buf += '0';
						}
					} ورنہ اگر (part.بنیاد == 8 || part.بنیاد == 16) {
						اگر (part.show_بنیاد)
							سابقہ_2 += (part.بنیاد == 16) ? (part.hex_بالائی ? "0X" : "0x") : "0o";
						سائز_t قدم = (part.بنیاد == 16) ? 4 : 3;
						برائے (سائز_t اشاریہ = 0; اشاریہ < (سائز_t)قدر.سائز(); اشاریہ += قدم) {
							اگر (part.گروہ && اشاریہ > 0 && اشاریہ % (4 * قدم) == 0)
								buf += '_';
							RTLIL::Const subvalue = قدر.نکالو_2(اشاریہ, min(قدم, قدر.سائز() - اشاریہ));
							منطقی has_x = جھوٹ, all_x = سچ, has_z = جھوٹ, all_z = سچ;
							برائے (State ثنائی_ہندسہ : subvalue) {
								اگر (ثنائی_ہندسہ == State::Sx)
									has_x = سچ;
								ورنہ
									all_x = جھوٹ;
								اگر (ثنائی_ہندسہ == State::Sz)
									has_z = سچ;
								ورنہ
									all_z = جھوٹ;
							}
							اگر (all_x)
								buf += 'x';
							ورنہ اگر (all_z)
								buf += 'z';
							ورنہ اگر (has_x)
								buf += 'X';
							ورنہ اگر (has_z)
								buf += 'Z';
							ورنہ {
								ثابت حرف *digits = part.hex_بالائی ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} ورنہ اگر (part.بنیاد == 10) {
						اگر (part.show_بنیاد)
							سابقہ_2 += "0d";
						اگر (all_x)
							buf += 'x';
						ورنہ اگر (all_z)
							buf += 'z';
						ورنہ اگر (has_x)
							buf += 'X';
						ورنہ اگر (has_z)
							buf += 'Z';
						ورنہ {
							log_assert(قدر.is_fully_def());
							اگر (قدر.is_fully_zero())
								buf += '0';
							سائز_t اشاریہ = 0;
							جب (!قدر.is_fully_zero())	{
								اگر (part.گروہ && اشاریہ > 0 && اشاریہ % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(قدر, 10, جھوٹ, جھوٹ, 4).as_int();
								قدر = RTLIL::const_div(قدر, 10, جھوٹ, جھوٹ, قدر.سائز());
								اشاریہ++;
							}
						}
					} ورنہ log_abort();
					اگر (part.justify == FmtPart::NUMERIC && part.گروہ && part.padding == '0') {
						سائز_t گروہ_سائز = part.بنیاد == 10 ? 3 : 4;
						جب (سابقہ_2.سائز() + buf.سائز() < part.چوڑائی) {
							اگر (buf.سائز() % (گروہ_سائز + 1) == گروہ_سائز)
								buf += '_';
							buf += '0';
						}
					}
					std::الٹ(buf.begin(), buf.انتہا());
				} ورنہ اگر (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} ورنہ اگر (part.type == FmtPart::VLOG_وقت) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.چوڑائی == 0 || part.padding != '\0');
				اگر (سابقہ_2.سائز() + buf.سائز() < part.چوڑائی) {
					سائز_t pad_چوڑائی = part.چوڑائی - سابقہ_2.سائز() - buf.سائز();
					انتخاب (part.justify) {
						حالت FmtPart::بایاں:
							str += سابقہ_2;
							str += buf;
							str += std::string(pad_چوڑائی, part.padding);
							توڑو;
						حالت FmtPart::دایاں:
							str += std::string(pad_چوڑائی, part.padding);
							str += سابقہ_2;
							str += buf;
							توڑو;
						حالت FmtPart::NUMERIC:
							str += سابقہ_2;
							str += std::string(pad_چوڑائی, part.padding);
							str += buf;
							توڑو;
					}
				} ورنہ {
					str += سابقہ_2;
					str += buf;
				}
				توڑو;
			}
		}
	}

	واپس str;
}
