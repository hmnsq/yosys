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
	parts.ڌڪو_پٺ(part);
}

فراغ Fmt::parse_rtlil(ثابت RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec دليلون_2 = cell->get_پورٽ(ID(دليلون));
	parts.clear();

	FmtPart part;
	لكل (سائيز_t i = 0; i < fmt.سائيز(); i++) {
		إذا (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} وإلا إذا (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} وإلا إذا (fmt[i] == '}') {
			log_assert(خطأ && "Unexpected '}' in format string");
		} وإلا إذا (fmt[i] == '{') {
			إذا (!part.str.خالي_جايون()) {
				part.type = FmtPart::LITERAL;
				parts.ڌڪو_پٺ(part);
				part = {};
			}

			إذا (++i == fmt.سائيز())
				log_assert(خطأ && "Unexpected end in format substitution");

			سائيز_t arg_سائيز = 0;
			لكل (; i < fmt.سائيز(); i++) {
				إذا (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_سائيز *= 10;
					arg_سائيز += fmt[i] - '0';
				} وإلا إذا (fmt[i] == ':') {
					++i;
					اكسر;
				} وإلا {
					log_assert(خطأ && "Unexpected character in format substitution");
				}
			}
			إذا (i == fmt.سائيز())
				log_assert(خطأ && "Unexpected end in format substitution");

			إذا ((سائيز_t)دليلون_2.سائيز() < arg_سائيز)
				log_assert(خطأ && "Format part overruns arguments");
			part.sig = دليلون_2.ڪڍو_2(0, arg_سائيز);
			دليلون_2.remove(0, arg_سائيز);

			إذا (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				اذهب success;
			}

			إذا (fmt[i] == '>')
				part.justify = FmtPart::ساڄو;
			وإلا إذا (fmt[i] == '<')
				part.justify = FmtPart::کاٻو;
			وإلا إذا (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			وإلا
				log_assert(خطأ && "Unexpected justification in format substitution");
			إذا (++i == fmt.سائيز())
				log_assert(خطأ && "Unexpected end in format substitution");

			part.padding = fmt[i];
			إذا (++i == fmt.سائيز())
				log_assert(خطأ && "Unexpected end in format substitution");

			لكل (; i < fmt.سائيز(); i++) {
				إذا (fmt[i] >= '0' && fmt[i] <= '9') {
					part.ويڪر *= 10;
					part.ويڪر += fmt[i] - '0';
					استمر;
				} وإلا إذا (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.بنياد = 2;
				} وإلا إذا (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.بنياد = 8;
				} وإلا إذا (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.بنياد = 10;
				} وإلا إذا (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.بنياد = 16;
				} وإلا إذا (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.بنياد = 16;
					part.hex_مٿانهون = سچ;
				} وإلا إذا (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} وإلا إذا (fmt[i] == 't') {
					part.type = FmtPart::VLOG_وقت;
				} وإلا إذا (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_وقت;
					part.realtime = سچ;
				} وإلا {
					log_assert(خطأ && "Unexpected character in format substitution");
				}
				++i;
				اكسر;
			}
			إذا (i == fmt.سائيز())
				log_assert(خطأ && "Unexpected end in format substitution");

			إذا (part.type == FmtPart::INTEGER) {
				إذا (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					إذا (++i == fmt.سائيز())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا إذا (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					إذا (++i == fmt.سائيز())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا إذا (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					إذا (++i == fmt.سائيز())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا {
					// also accept no sign character and treat like MINUS for compatibility
				}

				إذا (fmt[i] == '#') {
					part.show_بنياد = سچ;
					++i;
				}
				إذا (fmt[i] == '_') {
					part.گروهه = سچ;
					++i;
				}

				إذا (fmt[i] == 'u')
					part.signed_ = خطأ;
				وإلا إذا (fmt[i] == 's')
					part.signed_ = سچ;
				وإلا
					log_assert(خطأ && "Unexpected character in format substitution");
				إذا (++i == fmt.سائيز())
					log_assert(خطأ && "Unexpected end in format substitution");
			}

		success:
			إذا (fmt[i] != '}')
				log_assert(خطأ && "Expected '}' after format substitution");

			parts.ڌڪو_پٺ(part);
			part = {};
		} وإلا {
			part.str += fmt[i];
		}
	}
	إذا (!part.str.خالي_جايون()) {
		part.type = FmtPart::LITERAL;
		parts.ڌڪو_پٺ(part);
	}
}

فراغ Fmt::emit_rtlil(RTLIL::Cell *cell) ثابت {
	std::string fmt;
	RTLIL::SigSpec دليلون_2;

	لكل (خودڪار &part : parts) {
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
				log_assert(part.sig.سائيز() <= 32);
				fmt += "{U}";
				اكسر;

			حالة FmtPart::VLOG_وقت:
				log_assert(part.sig.سائيز() == 0);
				YS_FALLTHROUGH
			حالة FmtPart::STRING:
				log_assert(part.sig.سائيز() % 8 == 0);
				YS_FALLTHROUGH
			حالة FmtPart::INTEGER:
				دليلون_2.append(part.sig);
				fmt += '{';
				fmt += std::منزل_string(part.sig.سائيز());
				fmt += ':';
				إذا (part.justify == FmtPart::ساڄو)
					fmt += '>';
				وإلا إذا (part.justify == FmtPart::کاٻو)
					fmt += '<';
				وإلا إذا (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				وإلا log_abort();
				log_assert(part.ويڪر == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				إذا (part.ويڪر > 0)
					fmt += std::منزل_string(part.ويڪر);
				إذا (part.type == FmtPart::INTEGER) {
					بدل (part.بنياد) {
						حالة  2: fmt += 'b'; اكسر;
						حالة  8: fmt += 'o'; اكسر;
						حالة 10: fmt += 'd'; اكسر;
						حالة 16: fmt += part.hex_مٿانهون ? 'H' : 'h'; اكسر;
						افتراضي: log_abort();
					}
					بدل (part.sign) {
						حالة FmtPart::MINUS:       fmt += '-'; اكسر;
						حالة FmtPart::PLUS_MINUS:  fmt += '+'; اكسر;
						حالة FmtPart::SPACE_MINUS: fmt += ' '; اكسر;
					}
					fmt += part.show_بنياد ? "#" : "";
					fmt += part.گروهه ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} وإلا إذا (part.type == FmtPart::STRING) {
					fmt += 'c';
				} وإلا إذا (part.type == FmtPart::VLOG_وقت) {
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

	cell->سيٽ_Param(ID(FORMAT), fmt);
	cell->سيٽ_Param(ID(دليلون_ويڪر), دليلون_2.سائيز());
	cell->سيٽ_پورٽ(ID(دليلون), دليلون_2);
}

ساكن سائيز_t compute_required_decimal_places(سائيز_t سائيز, منطقي signed_)
{
	BigUnsigned max;
	إذا (!signed_)
		max.سيٽ_ٻٽو_انگ(سائيز, سچ);
	وإلا
		max.سيٽ_ٻٽو_انگ(سائيز - 1, سچ);
	سائيز_t places = 0;
	طالما (!max.isZero()) {
		places++;
		max /= 10;
	}
	إذا (signed_)
		places++;
	أرجع places;
}

ساكن سائيز_t compute_required_nondecimal_places(سائيز_t سائيز, بنانشان بنياد)
{
	log_assert(بنياد != 10);
	BigUnsigned max;
	max.سيٽ_ٻٽو_انگ(سائيز - 1, سچ);
	سائيز_t places = 0;
	طالما (!max.isZero()) {
		places++;
		max /= بنياد;
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
فراغ Fmt::apply_verilog_automatic_sizing_and_جوڙيو(FmtPart &part)
{
	إذا (part.بنياد == 10) {
		سائيز_t places = compute_required_decimal_places(part.sig.سائيز(), part.signed_);
		part.padding = ' ';
		part.ويڪر = std::max(part.ويڪر, places);
		parts.ڌڪو_پٺ(part);
		أرجع;
	}

	part.padding = '0';

	سائيز_t places = compute_required_nondecimal_places(part.sig.سائيز(), part.بنياد);
	إذا (part.ويڪر < places) {
		part.justify = FmtPart::ساڄو;
		part.ويڪر = places;
		parts.ڌڪو_پٺ(part);
	} وإلا إذا (part.ويڪر == places) {
		parts.ڌڪو_پٺ(part);
	} وإلا إذا (part.ويڪر > places) {
		خودڪار gap = std::string(part.ويڪر - places, ' ');
		part.ويڪر = places;

		إذا (part.justify == FmtPart::ساڄو) {
			append_literal(gap);
			parts.ڌڪو_پٺ(part);
		} وإلا {
			part.justify = FmtPart::ساڄو;
			parts.ڌڪو_پٺ(part);
			append_literal(gap);
		}
	}
}

فراغ Fmt::parse_verilog(ثابت std::ويڪٽر<VerilogFmtArg> &دليلون_2, منطقي sformat_like, صحيح default_بنياد, ثابت std::string &task_name, ثابت std::string &module_name)
{
	parts.clear();

	خودڪار arg = دليلون_2.begin();
	لكل (; arg != دليلون_2.پڇاڙي(); ++arg) {
		بدل (arg->type) {
			حالة VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.بنياد = default_بنياد;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_جوڙيو(part);
				اكسر;
			}

			حالة VerilogFmtArg::وقت: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_وقت;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.ويڪر = 20;
				parts.ڌڪو_پٺ(part);
				اكسر;
			}

			حالة VerilogFmtArg::STRING: {
				إذا (arg == دليلون_2.begin() || !sformat_like) {
					ثابت خودڪار fmtarg = arg;
					ثابت std::string &fmt = fmtarg->str;
					FmtPart part = {};
					لكل (سائيز_t i = 0; i < fmt.سائيز(); i++) {
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
							إذا (!part.str.خالي_جايون()) {
								part.type = FmtPart::LITERAL;
								parts.ڌڪو_پٺ(part);
								part = {};
							}
							إذا (++i == fmt.سائيز()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - دليلون_2.begin() + 1);
							}

							إذا (++arg == دليلون_2.پڇاڙي()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - دليلون_2.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							لكل (; i < fmt.سائيز(); i++) {
								إذا (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::کاٻو;
								} وإلا إذا (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} وإلا اكسر;
							}
							إذا (i == fmt.سائيز()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - دليلون_2.begin() + 1);
							}

							منطقي has_leading_zero = خطأ, has_ويڪر = خطأ;
							لكل (; i < fmt.سائيز(); i++) {
								إذا (fmt[i] >= '0' && fmt[i] <= '9') {
									إذا (fmt[i] == '0' && !has_ويڪر) {
										has_leading_zero = سچ;
									} وإلا {
										has_ويڪر = سچ;
										part.ويڪر *= 10;
										part.ويڪر += fmt[i] - '0';
									}
									استمر;
								} وإلا إذا (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.بنياد =  2;
								} وإلا إذا (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.بنياد =  8;
								} وإلا إذا (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.بنياد = 10;
								} وإلا إذا (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.بنياد = 16;
								} وإلا إذا (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} وإلا إذا (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									إذا ((part.sig.سائيز() % 8) != 0)
										part.sig.extend_u0((part.sig.سائيز() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} وإلا إذا (fmt[i] == 't' || fmt[i] == 'T') {
									إذا (arg->type == VerilogFmtArg::وقت) {
										part.type = FmtPart::VLOG_وقت;
										part.realtime = arg->realtime;
										إذا (!has_ويڪر && !has_leading_zero)
											part.ويڪر = 20;
									} وإلا {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - دليلون_2.begin() + 1);
									}
								} وإلا {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - دليلون_2.begin() + 1);
								}
								اكسر;
							}
							إذا (i == fmt.سائيز()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - دليلون_2.begin() + 1);
							}

							إذا (part.padding == '\0') {
								إذا (has_leading_zero && part.justify == FmtPart::ساڄو) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} وإلا {
									part.padding = ' ';
								}
							}

							إذا (part.type == FmtPart::INTEGER && part.بنياد != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - دليلون_2.begin() + 1);

							إذا (part.بنياد != 10)
								part.signed_ = خطأ;
							إذا (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_جوڙيو(part);
							وإلا
								parts.ڌڪو_پٺ(part);
							part = {};
						}
					}
					إذا (!part.str.خالي_جايون()) {
						part.type = FmtPart::LITERAL;
						parts.ڌڪو_پٺ(part);
					}
				} وإلا {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.ڌڪو_پٺ(part);
				}
				اكسر;
			}

			افتراضي: log_abort();
		}
	}
}

std::ويڪٽر<VerilogFmtArg> Fmt::emit_verilog() ثابت
{
	std::ويڪٽر<VerilogFmtArg> دليلون_2;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	لكل (خودڪار &part : parts) {
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
				دليلون_2.ڌڪو_پٺ(arg);

				fmt.str += '%';
				إذا (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				إذا (part.justify == FmtPart::کاٻو)
					fmt.str += '-';
				إذا (part.ويڪر == 0) {
					fmt.str += '0';
				} وإلا إذا (part.ويڪر > 0) {
					إذا (part.بنياد != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::منزل_string(part.ويڪر);
				}
				بدل (part.بنياد) {
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
				دليلون_2.ڌڪو_پٺ(arg);

				fmt.str += '%';
				إذا (part.justify == FmtPart::کاٻو)
					fmt.str += '-';
				إذا (part.sig.سائيز() == 8) {
					إذا (part.ويڪر > 0) {
						إذا (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::منزل_string(part.ويڪر);
					}
					fmt.str += 'c';
				} وإلا {
					log_assert(part.sig.سائيز() % 8 == 0);
					إذا (part.ويڪر > 0) {
						fmt.str += std::منزل_string(part.ويڪر);
					}
					fmt.str += 's';
				}
				اكسر;
			}

			حالة FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.ڪڍو_2(0, 7); // only ASCII
				دليلون_2.ڌڪو_پٺ(arg);

				fmt.str += "%c";
				اكسر;
			}

			حالة FmtPart::VLOG_وقت: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::وقت;
				إذا (part.realtime)
					arg.realtime = سچ;
				دليلون_2.ڌڪو_پٺ(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				إذا (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				إذا (part.justify == FmtPart::کاٻو)
					fmt.str += '-';
				إذا (part.padding == '0' && part.ويڪر > 0)
					fmt.str += '0';
				fmt.str += std::منزل_string(part.ويڪر);
				fmt.str += 't';
				اكسر;
			}

			افتراضي: log_abort();
		}
	}

	دليلون_2.وجهو(دليلون_2.begin(), fmt);
	أرجع دليلون_2;
}

std::string escape_cxx_string(ثابت std::string &داخلا)
{
	std::string output = "\"";
	لكل (خودڪار c : داخلا) {
		إذا (::isprint(c)) {
			إذا (c == '\\' || c == '"')
				output.ڌڪو_پٺ('\\');
			output.ڌڪو_پٺ(c);
		} وإلا {
			حرف l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.ڌڪو_پٺ('\\');
			output.ڌڪو_پٺ('0' + h);
			output.ڌڪو_پٺ('0' + m);
			output.ڌڪو_پٺ('0' + l);
		}
	}
	output.ڌڪو_پٺ('"');
	إذا (output.ڳوليو('\0') != std::string::npos) {
		output.وجهو(0, "std::string {");
		output.append(stringf(", %zu}", داخلا.سائيز()));
	}
	أرجع output;
}

فراغ Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<فراغ(ثابت RTLIL::SigSpec &)> emit_sig, ثابت std::string &context) ثابت
{
	os << indent << "std::string buf;\n";
	لكل (خودڪار &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		بدل (part.type) {
			حالة FmtPart::LITERAL:   os << "LITERAL";   اكسر;
			حالة FmtPart::INTEGER:   os << "INTEGER";   اكسر;
			حالة FmtPart::STRING:    os << "STRING";    اكسر;
			حالة FmtPart::UNICHAR:   os << "UNICHAR";   اكسر;
			حالة FmtPart::VLOG_وقت: os << "VLOG_TIME"; اكسر;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		بدل (part.justify) {
			حالة FmtPart::کاٻو:    os << "LEFT";    اكسر;
			حالة FmtPart::ساڄو:   os << "RIGHT";   اكسر;
			حالة FmtPart::NUMERIC: os << "NUMERIC"; اكسر;
		}
		os << ", ";
		os << "(char)" << (صحيح)part.padding << ", ";
		os << part.ويڪر << ", ";
		os << part.بنياد << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		بدل (part.sign) {
			حالة FmtPart::MINUS:       os << "MINUS";       اكسر;
			حالة FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  اكسر;
			حالة FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; اكسر;
		}
		os << ", ";
		os << part.hex_مٿانهون << ", ";
		os << part.show_بنياد << ", ";
		os << part.گروهه << ", ";
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

	لكل (خودڪار &part : parts) {
		بدل (part.type) {
			حالة FmtPart::LITERAL:
				str += part.str;
				اكسر;

			حالة FmtPart::UNICHAR: {
				RTLIL::Const قدر = part.sig.as_const();
				uint32_t codepoint = قدر.as_int();
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
			حالة FmtPart::VLOG_وقت: {
				std::string buf;
				std::string اڳياڙي_2;
				إذا (part.type == FmtPart::INTEGER) {
					RTLIL::Const قدر = part.sig.as_const();
					منطقي has_x = خطأ, all_x = سچ, has_z = خطأ, all_z = سچ;
					لكل (State ٻٽو_انگ : قدر) {
						إذا (ٻٽو_انگ == State::Sx)
							has_x = سچ;
						وإلا
							all_x = خطأ;
						إذا (ٻٽو_انگ == State::Sz)
							has_z = سچ;
						وإلا
							all_z = خطأ;
					}

					إذا (!has_z && !has_x && part.signed_ && قدر[قدر.سائيز() - 1]) {
						اڳياڙي_2 = "-";
						قدر = RTLIL::const_neg(قدر, {}, part.signed_, {}, قدر.سائيز() + 1);
					} وإلا {
						بدل (part.sign) {
							حالة FmtPart::MINUS:       اكسر;
							حالة FmtPart::PLUS_MINUS:  اڳياڙي_2 = "+"; اكسر;
							حالة FmtPart::SPACE_MINUS: اڳياڙي_2 = " "; اكسر;
						}
					}

					إذا (part.بنياد != 10) {
						سائيز_t minimum_سائيز = 1;
						لكل (سائيز_t اشاريو = 0; اشاريو < (سائيز_t)قدر.سائيز(); اشاريو++)
							إذا (قدر[اشاريو] != State::S0)
								minimum_سائيز = اشاريو + 1;
						قدر = قدر.ڪڍو_2(0, minimum_سائيز);
					}

					إذا (part.بنياد == 2) {
						إذا (part.show_بنياد)
							اڳياڙي_2 += "0b";
						لكل (سائيز_t اشاريو = 0; اشاريو < (سائيز_t)قدر.سائيز(); اشاريو++) {
							إذا (part.گروهه && اشاريو > 0 && اشاريو % 4 == 0)
								buf += '_';
							RTLIL::State ٻٽو_انگ = قدر[اشاريو];
							إذا (ٻٽو_انگ == State::Sx)
								buf += 'x';
							وإلا إذا (ٻٽو_انگ == State::Sz)
								buf += 'z';
							وإلا إذا (ٻٽو_انگ == State::S1)
								buf += '1';
							وإلا /* if (bit == State::S0) */
								buf += '0';
						}
					} وإلا إذا (part.بنياد == 8 || part.بنياد == 16) {
						إذا (part.show_بنياد)
							اڳياڙي_2 += (part.بنياد == 16) ? (part.hex_مٿانهون ? "0X" : "0x") : "0o";
						سائيز_t قدم = (part.بنياد == 16) ? 4 : 3;
						لكل (سائيز_t اشاريو = 0; اشاريو < (سائيز_t)قدر.سائيز(); اشاريو += قدم) {
							إذا (part.گروهه && اشاريو > 0 && اشاريو % (4 * قدم) == 0)
								buf += '_';
							RTLIL::Const subvalue = قدر.ڪڍو_2(اشاريو, min(قدم, قدر.سائيز() - اشاريو));
							منطقي has_x = خطأ, all_x = سچ, has_z = خطأ, all_z = سچ;
							لكل (State ٻٽو_انگ : subvalue) {
								إذا (ٻٽو_انگ == State::Sx)
									has_x = سچ;
								وإلا
									all_x = خطأ;
								إذا (ٻٽو_انگ == State::Sz)
									has_z = سچ;
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
								ثابت حرف *digits = part.hex_مٿانهون ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} وإلا إذا (part.بنياد == 10) {
						إذا (part.show_بنياد)
							اڳياڙي_2 += "0d";
						إذا (all_x)
							buf += 'x';
						وإلا إذا (all_z)
							buf += 'z';
						وإلا إذا (has_x)
							buf += 'X';
						وإلا إذا (has_z)
							buf += 'Z';
						وإلا {
							log_assert(قدر.is_fully_def());
							إذا (قدر.is_fully_zero())
								buf += '0';
							سائيز_t اشاريو = 0;
							طالما (!قدر.is_fully_zero())	{
								إذا (part.گروهه && اشاريو > 0 && اشاريو % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(قدر, 10, خطأ, خطأ, 4).as_int();
								قدر = RTLIL::const_div(قدر, 10, خطأ, خطأ, قدر.سائيز());
								اشاريو++;
							}
						}
					} وإلا log_abort();
					إذا (part.justify == FmtPart::NUMERIC && part.گروهه && part.padding == '0') {
						سائيز_t گروهه_سائيز = part.بنياد == 10 ? 3 : 4;
						طالما (اڳياڙي_2.سائيز() + buf.سائيز() < part.ويڪر) {
							إذا (buf.سائيز() % (گروهه_سائيز + 1) == گروهه_سائيز)
								buf += '_';
							buf += '0';
						}
					}
					std::ابتو(buf.begin(), buf.پڇاڙي());
				} وإلا إذا (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} وإلا إذا (part.type == FmtPart::VLOG_وقت) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.ويڪر == 0 || part.padding != '\0');
				إذا (اڳياڙي_2.سائيز() + buf.سائيز() < part.ويڪر) {
					سائيز_t pad_ويڪر = part.ويڪر - اڳياڙي_2.سائيز() - buf.سائيز();
					بدل (part.justify) {
						حالة FmtPart::کاٻو:
							str += اڳياڙي_2;
							str += buf;
							str += std::string(pad_ويڪر, part.padding);
							اكسر;
						حالة FmtPart::ساڄو:
							str += std::string(pad_ويڪر, part.padding);
							str += اڳياڙي_2;
							str += buf;
							اكسر;
						حالة FmtPart::NUMERIC:
							str += اڳياڙي_2;
							str += std::string(pad_ويڪر, part.padding);
							str += buf;
							اكسر;
					}
				} وإلا {
					str += اڳياڙي_2;
					str += buf;
				}
				اكسر;
			}
		}
	}

	أرجع str;
}
