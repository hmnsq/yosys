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
	parts.ټېل_شا(part);
}

فراغ Fmt::parse_rtlil(ثابت RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec دلیلونه_2 = cell->get_پورټ(ID(دلیلونه));
	parts.clear();

	FmtPart part;
	لكل (اندازه_t i = 0; i < fmt.اندازه(); i++) {
		إذا (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} وإلا إذا (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} وإلا إذا (fmt[i] == '}') {
			log_assert(خطأ && "Unexpected '}' in format string");
		} وإلا إذا (fmt[i] == '{') {
			إذا (!part.str.تشه()) {
				part.type = FmtPart::LITERAL;
				parts.ټېل_شا(part);
				part = {};
			}

			إذا (++i == fmt.اندازه())
				log_assert(خطأ && "Unexpected end in format substitution");

			اندازه_t arg_اندازه = 0;
			لكل (; i < fmt.اندازه(); i++) {
				إذا (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_اندازه *= 10;
					arg_اندازه += fmt[i] - '0';
				} وإلا إذا (fmt[i] == ':') {
					++i;
					اكسر;
				} وإلا {
					log_assert(خطأ && "Unexpected character in format substitution");
				}
			}
			إذا (i == fmt.اندازه())
				log_assert(خطأ && "Unexpected end in format substitution");

			إذا ((اندازه_t)دلیلونه_2.اندازه() < arg_اندازه)
				log_assert(خطأ && "Format part overruns arguments");
			part.sig = دلیلونه_2.ایستل(0, arg_اندازه);
			دلیلونه_2.remove(0, arg_اندازه);

			إذا (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				اذهب success;
			}

			إذا (fmt[i] == '>')
				part.justify = FmtPart::ښی_2;
			وإلا إذا (fmt[i] == '<')
				part.justify = FmtPart::کیڼ_2;
			وإلا إذا (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			وإلا
				log_assert(خطأ && "Unexpected justification in format substitution");
			إذا (++i == fmt.اندازه())
				log_assert(خطأ && "Unexpected end in format substitution");

			part.padding = fmt[i];
			إذا (++i == fmt.اندازه())
				log_assert(خطأ && "Unexpected end in format substitution");

			لكل (; i < fmt.اندازه(); i++) {
				إذا (fmt[i] >= '0' && fmt[i] <= '9') {
					part.پلنوالی *= 10;
					part.پلنوالی += fmt[i] - '0';
					استمر;
				} وإلا إذا (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.بنسټ = 2;
				} وإلا إذا (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.بنسټ = 8;
				} وإلا إذا (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.بنسټ = 10;
				} وإلا إذا (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.بنسټ = 16;
				} وإلا إذا (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.بنسټ = 16;
					part.hex_پاسنی = ریښتیا;
				} وإلا إذا (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} وإلا إذا (fmt[i] == 't') {
					part.type = FmtPart::VLOG_وخت;
				} وإلا إذا (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_وخت;
					part.realtime = ریښتیا;
				} وإلا {
					log_assert(خطأ && "Unexpected character in format substitution");
				}
				++i;
				اكسر;
			}
			إذا (i == fmt.اندازه())
				log_assert(خطأ && "Unexpected end in format substitution");

			إذا (part.type == FmtPart::INTEGER) {
				إذا (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					إذا (++i == fmt.اندازه())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا إذا (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					إذا (++i == fmt.اندازه())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا إذا (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					إذا (++i == fmt.اندازه())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا {
					// also accept no sign character and treat like MINUS for compatibility
				}

				إذا (fmt[i] == '#') {
					part.show_بنسټ = ریښتیا;
					++i;
				}
				إذا (fmt[i] == '_') {
					part.ډله = ریښتیا;
					++i;
				}

				إذا (fmt[i] == 'u')
					part.signed_ = خطأ;
				وإلا إذا (fmt[i] == 's')
					part.signed_ = ریښتیا;
				وإلا
					log_assert(خطأ && "Unexpected character in format substitution");
				إذا (++i == fmt.اندازه())
					log_assert(خطأ && "Unexpected end in format substitution");
			}

		success:
			إذا (fmt[i] != '}')
				log_assert(خطأ && "Expected '}' after format substitution");

			parts.ټېل_شا(part);
			part = {};
		} وإلا {
			part.str += fmt[i];
		}
	}
	إذا (!part.str.تشه()) {
		part.type = FmtPart::LITERAL;
		parts.ټېل_شا(part);
	}
}

فراغ Fmt::emit_rtlil(RTLIL::Cell *cell) ثابت {
	std::string fmt;
	RTLIL::SigSpec دلیلونه_2;

	لكل (خپلکاری &part : parts) {
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
				log_assert(part.sig.اندازه() <= 32);
				fmt += "{U}";
				اكسر;

			حالة FmtPart::VLOG_وخت:
				log_assert(part.sig.اندازه() == 0);
				YS_FALLTHROUGH
			حالة FmtPart::STRING:
				log_assert(part.sig.اندازه() % 8 == 0);
				YS_FALLTHROUGH
			حالة FmtPart::INTEGER:
				دلیلونه_2.append(part.sig);
				fmt += '{';
				fmt += std::منزل_string(part.sig.اندازه());
				fmt += ':';
				إذا (part.justify == FmtPart::ښی_2)
					fmt += '>';
				وإلا إذا (part.justify == FmtPart::کیڼ_2)
					fmt += '<';
				وإلا إذا (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				وإلا log_abort();
				log_assert(part.پلنوالی == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				إذا (part.پلنوالی > 0)
					fmt += std::منزل_string(part.پلنوالی);
				إذا (part.type == FmtPart::INTEGER) {
					بدل (part.بنسټ) {
						حالة  2: fmt += 'b'; اكسر;
						حالة  8: fmt += 'o'; اكسر;
						حالة 10: fmt += 'd'; اكسر;
						حالة 16: fmt += part.hex_پاسنی ? 'H' : 'h'; اكسر;
						افتراضي: log_abort();
					}
					بدل (part.sign) {
						حالة FmtPart::MINUS:       fmt += '-'; اكسر;
						حالة FmtPart::PLUS_MINUS:  fmt += '+'; اكسر;
						حالة FmtPart::SPACE_MINUS: fmt += ' '; اكسر;
					}
					fmt += part.show_بنسټ ? "#" : "";
					fmt += part.ډله ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} وإلا إذا (part.type == FmtPart::STRING) {
					fmt += 'c';
				} وإلا إذا (part.type == FmtPart::VLOG_وخت) {
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

	cell->سېټ_Param(ID(FORMAT), fmt);
	cell->سېټ_Param(ID(دلیلونه_پلنوالی), دلیلونه_2.اندازه());
	cell->سېټ_پورټ(ID(دلیلونه), دلیلونه_2);
}

ساكن اندازه_t compute_required_decimal_places(اندازه_t اندازه, منطقي signed_)
{
	BigUnsigned max;
	إذا (!signed_)
		max.سېټ_بټ(اندازه, ریښتیا);
	وإلا
		max.سېټ_بټ(اندازه - 1, ریښتیا);
	اندازه_t places = 0;
	طالما (!max.isZero()) {
		places++;
		max /= 10;
	}
	إذا (signed_)
		places++;
	أرجع places;
}

ساكن اندازه_t compute_required_nondecimal_places(اندازه_t اندازه, بینښه بنسټ)
{
	log_assert(بنسټ != 10);
	BigUnsigned max;
	max.سېټ_بټ(اندازه - 1, ریښتیا);
	اندازه_t places = 0;
	طالما (!max.isZero()) {
		places++;
		max /= بنسټ;
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
فراغ Fmt::apply_verilog_automatic_sizing_and_جمع_کړه(FmtPart &part)
{
	إذا (part.بنسټ == 10) {
		اندازه_t places = compute_required_decimal_places(part.sig.اندازه(), part.signed_);
		part.padding = ' ';
		part.پلنوالی = std::max(part.پلنوالی, places);
		parts.ټېل_شا(part);
		أرجع;
	}

	part.padding = '0';

	اندازه_t places = compute_required_nondecimal_places(part.sig.اندازه(), part.بنسټ);
	إذا (part.پلنوالی < places) {
		part.justify = FmtPart::ښی_2;
		part.پلنوالی = places;
		parts.ټېل_شا(part);
	} وإلا إذا (part.پلنوالی == places) {
		parts.ټېل_شا(part);
	} وإلا إذا (part.پلنوالی > places) {
		خپلکاری gap = std::string(part.پلنوالی - places, ' ');
		part.پلنوالی = places;

		إذا (part.justify == FmtPart::ښی_2) {
			append_literal(gap);
			parts.ټېل_شا(part);
		} وإلا {
			part.justify = FmtPart::ښی_2;
			parts.ټېل_شا(part);
			append_literal(gap);
		}
	}
}

فراغ Fmt::parse_verilog(ثابت std::ویکټور<VerilogFmtArg> &دلیلونه_2, منطقي sformat_like, صحيح default_بنسټ, ثابت std::string &task_name, ثابت std::string &module_name)
{
	parts.clear();

	خپلکاری arg = دلیلونه_2.begin();
	لكل (; arg != دلیلونه_2.پای_ټکی(); ++arg) {
		بدل (arg->type) {
			حالة VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.بنسټ = default_بنسټ;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_جمع_کړه(part);
				اكسر;
			}

			حالة VerilogFmtArg::وخت: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_وخت;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.پلنوالی = 20;
				parts.ټېل_شا(part);
				اكسر;
			}

			حالة VerilogFmtArg::STRING: {
				إذا (arg == دلیلونه_2.begin() || !sformat_like) {
					ثابت خپلکاری fmtarg = arg;
					ثابت std::string &fmt = fmtarg->str;
					FmtPart part = {};
					لكل (اندازه_t i = 0; i < fmt.اندازه(); i++) {
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
							إذا (!part.str.تشه()) {
								part.type = FmtPart::LITERAL;
								parts.ټېل_شا(part);
								part = {};
							}
							إذا (++i == fmt.اندازه()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - دلیلونه_2.begin() + 1);
							}

							إذا (++arg == دلیلونه_2.پای_ټکی()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - دلیلونه_2.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							لكل (; i < fmt.اندازه(); i++) {
								إذا (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::کیڼ_2;
								} وإلا إذا (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} وإلا اكسر;
							}
							إذا (i == fmt.اندازه()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - دلیلونه_2.begin() + 1);
							}

							منطقي has_leading_zero = خطأ, has_پلنوالی = خطأ;
							لكل (; i < fmt.اندازه(); i++) {
								إذا (fmt[i] >= '0' && fmt[i] <= '9') {
									إذا (fmt[i] == '0' && !has_پلنوالی) {
										has_leading_zero = ریښتیا;
									} وإلا {
										has_پلنوالی = ریښتیا;
										part.پلنوالی *= 10;
										part.پلنوالی += fmt[i] - '0';
									}
									استمر;
								} وإلا إذا (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.بنسټ =  2;
								} وإلا إذا (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.بنسټ =  8;
								} وإلا إذا (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.بنسټ = 10;
								} وإلا إذا (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.بنسټ = 16;
								} وإلا إذا (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} وإلا إذا (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									إذا ((part.sig.اندازه() % 8) != 0)
										part.sig.extend_u0((part.sig.اندازه() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} وإلا إذا (fmt[i] == 't' || fmt[i] == 'T') {
									إذا (arg->type == VerilogFmtArg::وخت) {
										part.type = FmtPart::VLOG_وخت;
										part.realtime = arg->realtime;
										إذا (!has_پلنوالی && !has_leading_zero)
											part.پلنوالی = 20;
									} وإلا {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - دلیلونه_2.begin() + 1);
									}
								} وإلا {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - دلیلونه_2.begin() + 1);
								}
								اكسر;
							}
							إذا (i == fmt.اندازه()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - دلیلونه_2.begin() + 1);
							}

							إذا (part.padding == '\0') {
								إذا (has_leading_zero && part.justify == FmtPart::ښی_2) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} وإلا {
									part.padding = ' ';
								}
							}

							إذا (part.type == FmtPart::INTEGER && part.بنسټ != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - دلیلونه_2.begin() + 1);

							إذا (part.بنسټ != 10)
								part.signed_ = خطأ;
							إذا (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_جمع_کړه(part);
							وإلا
								parts.ټېل_شا(part);
							part = {};
						}
					}
					إذا (!part.str.تشه()) {
						part.type = FmtPart::LITERAL;
						parts.ټېل_شا(part);
					}
				} وإلا {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.ټېل_شا(part);
				}
				اكسر;
			}

			افتراضي: log_abort();
		}
	}
}

std::ویکټور<VerilogFmtArg> Fmt::emit_verilog() ثابت
{
	std::ویکټور<VerilogFmtArg> دلیلونه_2;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	لكل (خپلکاری &part : parts) {
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
				دلیلونه_2.ټېل_شا(arg);

				fmt.str += '%';
				إذا (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				إذا (part.justify == FmtPart::کیڼ_2)
					fmt.str += '-';
				إذا (part.پلنوالی == 0) {
					fmt.str += '0';
				} وإلا إذا (part.پلنوالی > 0) {
					إذا (part.بنسټ != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::منزل_string(part.پلنوالی);
				}
				بدل (part.بنسټ) {
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
				دلیلونه_2.ټېل_شا(arg);

				fmt.str += '%';
				إذا (part.justify == FmtPart::کیڼ_2)
					fmt.str += '-';
				إذا (part.sig.اندازه() == 8) {
					إذا (part.پلنوالی > 0) {
						إذا (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::منزل_string(part.پلنوالی);
					}
					fmt.str += 'c';
				} وإلا {
					log_assert(part.sig.اندازه() % 8 == 0);
					إذا (part.پلنوالی > 0) {
						fmt.str += std::منزل_string(part.پلنوالی);
					}
					fmt.str += 's';
				}
				اكسر;
			}

			حالة FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.ایستل(0, 7); // only ASCII
				دلیلونه_2.ټېل_شا(arg);

				fmt.str += "%c";
				اكسر;
			}

			حالة FmtPart::VLOG_وخت: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::وخت;
				إذا (part.realtime)
					arg.realtime = ریښتیا;
				دلیلونه_2.ټېل_شا(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				إذا (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				إذا (part.justify == FmtPart::کیڼ_2)
					fmt.str += '-';
				إذا (part.padding == '0' && part.پلنوالی > 0)
					fmt.str += '0';
				fmt.str += std::منزل_string(part.پلنوالی);
				fmt.str += 't';
				اكسر;
			}

			افتراضي: log_abort();
		}
	}

	دلیلونه_2.ورننباسه(دلیلونه_2.begin(), fmt);
	أرجع دلیلونه_2;
}

std::string escape_cxx_string(ثابت std::string &ننوت)
{
	std::string output = "\"";
	لكل (خپلکاری c : ننوت) {
		إذا (::isprint(c)) {
			إذا (c == '\\' || c == '"')
				output.ټېل_شا('\\');
			output.ټېل_شا(c);
		} وإلا {
			حرف l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.ټېل_شا('\\');
			output.ټېل_شا('0' + h);
			output.ټېل_شا('0' + m);
			output.ټېل_شا('0' + l);
		}
	}
	output.ټېل_شا('"');
	إذا (output.پیدا_کړه('\0') != std::string::npos) {
		output.ورننباسه(0, "std::string {");
		output.append(stringf(", %zu}", ننوت.اندازه()));
	}
	أرجع output;
}

فراغ Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<فراغ(ثابت RTLIL::SigSpec &)> emit_sig, ثابت std::string &context) ثابت
{
	os << indent << "std::string buf;\n";
	لكل (خپلکاری &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		بدل (part.type) {
			حالة FmtPart::LITERAL:   os << "LITERAL";   اكسر;
			حالة FmtPart::INTEGER:   os << "INTEGER";   اكسر;
			حالة FmtPart::STRING:    os << "STRING";    اكسر;
			حالة FmtPart::UNICHAR:   os << "UNICHAR";   اكسر;
			حالة FmtPart::VLOG_وخت: os << "VLOG_TIME"; اكسر;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		بدل (part.justify) {
			حالة FmtPart::کیڼ_2:    os << "LEFT";    اكسر;
			حالة FmtPart::ښی_2:   os << "RIGHT";   اكسر;
			حالة FmtPart::NUMERIC: os << "NUMERIC"; اكسر;
		}
		os << ", ";
		os << "(char)" << (صحيح)part.padding << ", ";
		os << part.پلنوالی << ", ";
		os << part.بنسټ << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		بدل (part.sign) {
			حالة FmtPart::MINUS:       os << "MINUS";       اكسر;
			حالة FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  اكسر;
			حالة FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; اكسر;
		}
		os << ", ";
		os << part.hex_پاسنی << ", ";
		os << part.show_بنسټ << ", ";
		os << part.ډله << ", ";
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

	لكل (خپلکاری &part : parts) {
		بدل (part.type) {
			حالة FmtPart::LITERAL:
				str += part.str;
				اكسر;

			حالة FmtPart::UNICHAR: {
				RTLIL::Const ارزښت = part.sig.as_const();
				uint32_t codepoint = ارزښت.as_int();
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
			حالة FmtPart::VLOG_وخت: {
				std::string buf;
				std::string مختاړی_2;
				إذا (part.type == FmtPart::INTEGER) {
					RTLIL::Const ارزښت = part.sig.as_const();
					منطقي has_x = خطأ, all_x = ریښتیا, has_z = خطأ, all_z = ریښتیا;
					لكل (State بټ : ارزښت) {
						إذا (بټ == State::Sx)
							has_x = ریښتیا;
						وإلا
							all_x = خطأ;
						إذا (بټ == State::Sz)
							has_z = ریښتیا;
						وإلا
							all_z = خطأ;
					}

					إذا (!has_z && !has_x && part.signed_ && ارزښت[ارزښت.اندازه() - 1]) {
						مختاړی_2 = "-";
						ارزښت = RTLIL::const_neg(ارزښت, {}, part.signed_, {}, ارزښت.اندازه() + 1);
					} وإلا {
						بدل (part.sign) {
							حالة FmtPart::MINUS:       اكسر;
							حالة FmtPart::PLUS_MINUS:  مختاړی_2 = "+"; اكسر;
							حالة FmtPart::SPACE_MINUS: مختاړی_2 = " "; اكسر;
						}
					}

					إذا (part.بنسټ != 10) {
						اندازه_t minimum_اندازه = 1;
						لكل (اندازه_t شاخص = 0; شاخص < (اندازه_t)ارزښت.اندازه(); شاخص++)
							إذا (ارزښت[شاخص] != State::S0)
								minimum_اندازه = شاخص + 1;
						ارزښت = ارزښت.ایستل(0, minimum_اندازه);
					}

					إذا (part.بنسټ == 2) {
						إذا (part.show_بنسټ)
							مختاړی_2 += "0b";
						لكل (اندازه_t شاخص = 0; شاخص < (اندازه_t)ارزښت.اندازه(); شاخص++) {
							إذا (part.ډله && شاخص > 0 && شاخص % 4 == 0)
								buf += '_';
							RTLIL::State بټ = ارزښت[شاخص];
							إذا (بټ == State::Sx)
								buf += 'x';
							وإلا إذا (بټ == State::Sz)
								buf += 'z';
							وإلا إذا (بټ == State::S1)
								buf += '1';
							وإلا /* if (bit == State::S0) */
								buf += '0';
						}
					} وإلا إذا (part.بنسټ == 8 || part.بنسټ == 16) {
						إذا (part.show_بنسټ)
							مختاړی_2 += (part.بنسټ == 16) ? (part.hex_پاسنی ? "0X" : "0x") : "0o";
						اندازه_t ګام = (part.بنسټ == 16) ? 4 : 3;
						لكل (اندازه_t شاخص = 0; شاخص < (اندازه_t)ارزښت.اندازه(); شاخص += ګام) {
							إذا (part.ډله && شاخص > 0 && شاخص % (4 * ګام) == 0)
								buf += '_';
							RTLIL::Const subvalue = ارزښت.ایستل(شاخص, min(ګام, ارزښت.اندازه() - شاخص));
							منطقي has_x = خطأ, all_x = ریښتیا, has_z = خطأ, all_z = ریښتیا;
							لكل (State بټ : subvalue) {
								إذا (بټ == State::Sx)
									has_x = ریښتیا;
								وإلا
									all_x = خطأ;
								إذا (بټ == State::Sz)
									has_z = ریښتیا;
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
								ثابت حرف *digits = part.hex_پاسنی ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} وإلا إذا (part.بنسټ == 10) {
						إذا (part.show_بنسټ)
							مختاړی_2 += "0d";
						إذا (all_x)
							buf += 'x';
						وإلا إذا (all_z)
							buf += 'z';
						وإلا إذا (has_x)
							buf += 'X';
						وإلا إذا (has_z)
							buf += 'Z';
						وإلا {
							log_assert(ارزښت.is_fully_def());
							إذا (ارزښت.is_fully_zero())
								buf += '0';
							اندازه_t شاخص = 0;
							طالما (!ارزښت.is_fully_zero())	{
								إذا (part.ډله && شاخص > 0 && شاخص % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(ارزښت, 10, خطأ, خطأ, 4).as_int();
								ارزښت = RTLIL::const_div(ارزښت, 10, خطأ, خطأ, ارزښت.اندازه());
								شاخص++;
							}
						}
					} وإلا log_abort();
					إذا (part.justify == FmtPart::NUMERIC && part.ډله && part.padding == '0') {
						اندازه_t ډله_اندازه = part.بنسټ == 10 ? 3 : 4;
						طالما (مختاړی_2.اندازه() + buf.اندازه() < part.پلنوالی) {
							إذا (buf.اندازه() % (ډله_اندازه + 1) == ډله_اندازه)
								buf += '_';
							buf += '0';
						}
					}
					std::سرچپه(buf.begin(), buf.پای_ټکی());
				} وإلا إذا (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} وإلا إذا (part.type == FmtPart::VLOG_وخت) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.پلنوالی == 0 || part.padding != '\0');
				إذا (مختاړی_2.اندازه() + buf.اندازه() < part.پلنوالی) {
					اندازه_t pad_پلنوالی = part.پلنوالی - مختاړی_2.اندازه() - buf.اندازه();
					بدل (part.justify) {
						حالة FmtPart::کیڼ_2:
							str += مختاړی_2;
							str += buf;
							str += std::string(pad_پلنوالی, part.padding);
							اكسر;
						حالة FmtPart::ښی_2:
							str += std::string(pad_پلنوالی, part.padding);
							str += مختاړی_2;
							str += buf;
							اكسر;
						حالة FmtPart::NUMERIC:
							str += مختاړی_2;
							str += std::string(pad_پلنوالی, part.padding);
							str += buf;
							اكسر;
					}
				} وإلا {
					str += مختاړی_2;
					str += buf;
				}
				اكسر;
			}
		}
	}

	أرجع str;
}
