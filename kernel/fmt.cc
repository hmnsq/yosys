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
	parts.افزودن_پشت(part);
}

فراغ Fmt::parse_rtlil(ثابت RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec آرگومانها_2 = cell->get_پورت(ID(آرگومانها));
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
			إذا (!part.str.خالی()) {
				part.type = FmtPart::LITERAL;
				parts.افزودن_پشت(part);
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

			إذا ((اندازه_t)آرگومانها_2.اندازه() < arg_اندازه)
				log_assert(خطأ && "Format part overruns arguments");
			part.sig = آرگومانها_2.استخراج(0, arg_اندازه);
			آرگومانها_2.remove(0, arg_اندازه);

			إذا (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				اذهب success;
			}

			إذا (fmt[i] == '>')
				part.justify = FmtPart::راست_2;
			وإلا إذا (fmt[i] == '<')
				part.justify = FmtPart::چپ_2;
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
					part.عرض *= 10;
					part.عرض += fmt[i] - '0';
					استمر;
				} وإلا إذا (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.پایه = 2;
				} وإلا إذا (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.پایه = 8;
				} وإلا إذا (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.پایه = 10;
				} وإلا إذا (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.پایه = 16;
				} وإلا إذا (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.پایه = 16;
					part.hex_فوقانی = درست;
				} وإلا إذا (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} وإلا إذا (fmt[i] == 't') {
					part.type = FmtPart::VLOG_زمان;
				} وإلا إذا (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_زمان;
					part.realtime = درست;
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
					part.show_پایه = درست;
					++i;
				}
				إذا (fmt[i] == '_') {
					part.گروه = درست;
					++i;
				}

				إذا (fmt[i] == 'u')
					part.signed_ = خطأ;
				وإلا إذا (fmt[i] == 's')
					part.signed_ = درست;
				وإلا
					log_assert(خطأ && "Unexpected character in format substitution");
				إذا (++i == fmt.اندازه())
					log_assert(خطأ && "Unexpected end in format substitution");
			}

		success:
			إذا (fmt[i] != '}')
				log_assert(خطأ && "Expected '}' after format substitution");

			parts.افزودن_پشت(part);
			part = {};
		} وإلا {
			part.str += fmt[i];
		}
	}
	إذا (!part.str.خالی()) {
		part.type = FmtPart::LITERAL;
		parts.افزودن_پشت(part);
	}
}

فراغ Fmt::emit_rtlil(RTLIL::Cell *cell) ثابت {
	std::string fmt;
	RTLIL::SigSpec آرگومانها_2;

	لكل (خودکار &part : parts) {
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

			حالة FmtPart::VLOG_زمان:
				log_assert(part.sig.اندازه() == 0);
				YS_FALLTHROUGH
			حالة FmtPart::STRING:
				log_assert(part.sig.اندازه() % 8 == 0);
				YS_FALLTHROUGH
			حالة FmtPart::INTEGER:
				آرگومانها_2.append(part.sig);
				fmt += '{';
				fmt += std::مقصد_string(part.sig.اندازه());
				fmt += ':';
				إذا (part.justify == FmtPart::راست_2)
					fmt += '>';
				وإلا إذا (part.justify == FmtPart::چپ_2)
					fmt += '<';
				وإلا إذا (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				وإلا log_abort();
				log_assert(part.عرض == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				إذا (part.عرض > 0)
					fmt += std::مقصد_string(part.عرض);
				إذا (part.type == FmtPart::INTEGER) {
					بدل (part.پایه) {
						حالة  2: fmt += 'b'; اكسر;
						حالة  8: fmt += 'o'; اكسر;
						حالة 10: fmt += 'd'; اكسر;
						حالة 16: fmt += part.hex_فوقانی ? 'H' : 'h'; اكسر;
						افتراضي: log_abort();
					}
					بدل (part.sign) {
						حالة FmtPart::MINUS:       fmt += '-'; اكسر;
						حالة FmtPart::PLUS_MINUS:  fmt += '+'; اكسر;
						حالة FmtPart::SPACE_MINUS: fmt += ' '; اكسر;
					}
					fmt += part.show_پایه ? "#" : "";
					fmt += part.گروه ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} وإلا إذا (part.type == FmtPart::STRING) {
					fmt += 'c';
				} وإلا إذا (part.type == FmtPart::VLOG_زمان) {
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

	cell->مجموعه_Param(ID(FORMAT), fmt);
	cell->مجموعه_Param(ID(آرگومانها_عرض), آرگومانها_2.اندازه());
	cell->مجموعه_پورت(ID(آرگومانها), آرگومانها_2);
}

ساكن اندازه_t compute_required_decimal_places(اندازه_t اندازه, منطقي signed_)
{
	BigUnsigned max;
	إذا (!signed_)
		max.مجموعه_بیت(اندازه, درست);
	وإلا
		max.مجموعه_بیت(اندازه - 1, درست);
	اندازه_t places = 0;
	طالما (!max.isZero()) {
		places++;
		max /= 10;
	}
	إذا (signed_)
		places++;
	أرجع places;
}

ساكن اندازه_t compute_required_nondecimal_places(اندازه_t اندازه, بیعلامت پایه)
{
	log_assert(پایه != 10);
	BigUnsigned max;
	max.مجموعه_بیت(اندازه - 1, درست);
	اندازه_t places = 0;
	طالما (!max.isZero()) {
		places++;
		max /= پایه;
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
فراغ Fmt::apply_verilog_automatic_sizing_and_جمع_کن(FmtPart &part)
{
	إذا (part.پایه == 10) {
		اندازه_t places = compute_required_decimal_places(part.sig.اندازه(), part.signed_);
		part.padding = ' ';
		part.عرض = std::max(part.عرض, places);
		parts.افزودن_پشت(part);
		أرجع;
	}

	part.padding = '0';

	اندازه_t places = compute_required_nondecimal_places(part.sig.اندازه(), part.پایه);
	إذا (part.عرض < places) {
		part.justify = FmtPart::راست_2;
		part.عرض = places;
		parts.افزودن_پشت(part);
	} وإلا إذا (part.عرض == places) {
		parts.افزودن_پشت(part);
	} وإلا إذا (part.عرض > places) {
		خودکار gap = std::string(part.عرض - places, ' ');
		part.عرض = places;

		إذا (part.justify == FmtPart::راست_2) {
			append_literal(gap);
			parts.افزودن_پشت(part);
		} وإلا {
			part.justify = FmtPart::راست_2;
			parts.افزودن_پشت(part);
			append_literal(gap);
		}
	}
}

فراغ Fmt::parse_verilog(ثابت std::وکتور<VerilogFmtArg> &آرگومانها_2, منطقي sformat_like, صحيح default_پایه, ثابت std::string &task_name, ثابت std::string &module_name)
{
	parts.clear();

	خودکار arg = آرگومانها_2.begin();
	لكل (; arg != آرگومانها_2.انتها(); ++arg) {
		بدل (arg->type) {
			حالة VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.پایه = default_پایه;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_جمع_کن(part);
				اكسر;
			}

			حالة VerilogFmtArg::زمان: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_زمان;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.عرض = 20;
				parts.افزودن_پشت(part);
				اكسر;
			}

			حالة VerilogFmtArg::STRING: {
				إذا (arg == آرگومانها_2.begin() || !sformat_like) {
					ثابت خودکار fmtarg = arg;
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
							إذا (!part.str.خالی()) {
								part.type = FmtPart::LITERAL;
								parts.افزودن_پشت(part);
								part = {};
							}
							إذا (++i == fmt.اندازه()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - آرگومانها_2.begin() + 1);
							}

							إذا (++arg == آرگومانها_2.انتها()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - آرگومانها_2.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							لكل (; i < fmt.اندازه(); i++) {
								إذا (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::چپ_2;
								} وإلا إذا (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} وإلا اكسر;
							}
							إذا (i == fmt.اندازه()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - آرگومانها_2.begin() + 1);
							}

							منطقي has_leading_zero = خطأ, has_عرض = خطأ;
							لكل (; i < fmt.اندازه(); i++) {
								إذا (fmt[i] >= '0' && fmt[i] <= '9') {
									إذا (fmt[i] == '0' && !has_عرض) {
										has_leading_zero = درست;
									} وإلا {
										has_عرض = درست;
										part.عرض *= 10;
										part.عرض += fmt[i] - '0';
									}
									استمر;
								} وإلا إذا (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.پایه =  2;
								} وإلا إذا (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.پایه =  8;
								} وإلا إذا (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.پایه = 10;
								} وإلا إذا (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.پایه = 16;
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
									إذا (arg->type == VerilogFmtArg::زمان) {
										part.type = FmtPart::VLOG_زمان;
										part.realtime = arg->realtime;
										إذا (!has_عرض && !has_leading_zero)
											part.عرض = 20;
									} وإلا {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - آرگومانها_2.begin() + 1);
									}
								} وإلا {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - آرگومانها_2.begin() + 1);
								}
								اكسر;
							}
							إذا (i == fmt.اندازه()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - آرگومانها_2.begin() + 1);
							}

							إذا (part.padding == '\0') {
								إذا (has_leading_zero && part.justify == FmtPart::راست_2) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} وإلا {
									part.padding = ' ';
								}
							}

							إذا (part.type == FmtPart::INTEGER && part.پایه != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - آرگومانها_2.begin() + 1);

							إذا (part.پایه != 10)
								part.signed_ = خطأ;
							إذا (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_جمع_کن(part);
							وإلا
								parts.افزودن_پشت(part);
							part = {};
						}
					}
					إذا (!part.str.خالی()) {
						part.type = FmtPart::LITERAL;
						parts.افزودن_پشت(part);
					}
				} وإلا {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.افزودن_پشت(part);
				}
				اكسر;
			}

			افتراضي: log_abort();
		}
	}
}

std::وکتور<VerilogFmtArg> Fmt::emit_verilog() ثابت
{
	std::وکتور<VerilogFmtArg> آرگومانها_2;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	لكل (خودکار &part : parts) {
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
				آرگومانها_2.افزودن_پشت(arg);

				fmt.str += '%';
				إذا (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				إذا (part.justify == FmtPart::چپ_2)
					fmt.str += '-';
				إذا (part.عرض == 0) {
					fmt.str += '0';
				} وإلا إذا (part.عرض > 0) {
					إذا (part.پایه != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::مقصد_string(part.عرض);
				}
				بدل (part.پایه) {
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
				آرگومانها_2.افزودن_پشت(arg);

				fmt.str += '%';
				إذا (part.justify == FmtPart::چپ_2)
					fmt.str += '-';
				إذا (part.sig.اندازه() == 8) {
					إذا (part.عرض > 0) {
						إذا (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::مقصد_string(part.عرض);
					}
					fmt.str += 'c';
				} وإلا {
					log_assert(part.sig.اندازه() % 8 == 0);
					إذا (part.عرض > 0) {
						fmt.str += std::مقصد_string(part.عرض);
					}
					fmt.str += 's';
				}
				اكسر;
			}

			حالة FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.استخراج(0, 7); // only ASCII
				آرگومانها_2.افزودن_پشت(arg);

				fmt.str += "%c";
				اكسر;
			}

			حالة FmtPart::VLOG_زمان: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::زمان;
				إذا (part.realtime)
					arg.realtime = درست;
				آرگومانها_2.افزودن_پشت(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				إذا (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				إذا (part.justify == FmtPart::چپ_2)
					fmt.str += '-';
				إذا (part.padding == '0' && part.عرض > 0)
					fmt.str += '0';
				fmt.str += std::مقصد_string(part.عرض);
				fmt.str += 't';
				اكسر;
			}

			افتراضي: log_abort();
		}
	}

	آرگومانها_2.درج(آرگومانها_2.begin(), fmt);
	أرجع آرگومانها_2;
}

std::string escape_cxx_string(ثابت std::string &درون_داد)
{
	std::string output = "\"";
	لكل (خودکار c : درون_داد) {
		إذا (::isprint(c)) {
			إذا (c == '\\' || c == '"')
				output.افزودن_پشت('\\');
			output.افزودن_پشت(c);
		} وإلا {
			حرف l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.افزودن_پشت('\\');
			output.افزودن_پشت('0' + h);
			output.افزودن_پشت('0' + m);
			output.افزودن_پشت('0' + l);
		}
	}
	output.افزودن_پشت('"');
	إذا (output.پیدا_کن('\0') != std::string::npos) {
		output.درج(0, "std::string {");
		output.append(stringf(", %zu}", درون_داد.اندازه()));
	}
	أرجع output;
}

فراغ Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<فراغ(ثابت RTLIL::SigSpec &)> emit_sig, ثابت std::string &context) ثابت
{
	os << indent << "std::string buf;\n";
	لكل (خودکار &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		بدل (part.type) {
			حالة FmtPart::LITERAL:   os << "LITERAL";   اكسر;
			حالة FmtPart::INTEGER:   os << "INTEGER";   اكسر;
			حالة FmtPart::STRING:    os << "STRING";    اكسر;
			حالة FmtPart::UNICHAR:   os << "UNICHAR";   اكسر;
			حالة FmtPart::VLOG_زمان: os << "VLOG_TIME"; اكسر;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		بدل (part.justify) {
			حالة FmtPart::چپ_2:    os << "LEFT";    اكسر;
			حالة FmtPart::راست_2:   os << "RIGHT";   اكسر;
			حالة FmtPart::NUMERIC: os << "NUMERIC"; اكسر;
		}
		os << ", ";
		os << "(char)" << (صحيح)part.padding << ", ";
		os << part.عرض << ", ";
		os << part.پایه << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		بدل (part.sign) {
			حالة FmtPart::MINUS:       os << "MINUS";       اكسر;
			حالة FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  اكسر;
			حالة FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; اكسر;
		}
		os << ", ";
		os << part.hex_فوقانی << ", ";
		os << part.show_پایه << ", ";
		os << part.گروه << ", ";
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

	لكل (خودکار &part : parts) {
		بدل (part.type) {
			حالة FmtPart::LITERAL:
				str += part.str;
				اكسر;

			حالة FmtPart::UNICHAR: {
				RTLIL::Const مقدار = part.sig.as_const();
				uint32_t codepoint = مقدار.as_int();
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
			حالة FmtPart::VLOG_زمان: {
				std::string buf;
				std::string پیشوند_2;
				إذا (part.type == FmtPart::INTEGER) {
					RTLIL::Const مقدار = part.sig.as_const();
					منطقي has_x = خطأ, all_x = درست, has_z = خطأ, all_z = درست;
					لكل (State بیت : مقدار) {
						إذا (بیت == State::Sx)
							has_x = درست;
						وإلا
							all_x = خطأ;
						إذا (بیت == State::Sz)
							has_z = درست;
						وإلا
							all_z = خطأ;
					}

					إذا (!has_z && !has_x && part.signed_ && مقدار[مقدار.اندازه() - 1]) {
						پیشوند_2 = "-";
						مقدار = RTLIL::const_neg(مقدار, {}, part.signed_, {}, مقدار.اندازه() + 1);
					} وإلا {
						بدل (part.sign) {
							حالة FmtPart::MINUS:       اكسر;
							حالة FmtPart::PLUS_MINUS:  پیشوند_2 = "+"; اكسر;
							حالة FmtPart::SPACE_MINUS: پیشوند_2 = " "; اكسر;
						}
					}

					إذا (part.پایه != 10) {
						اندازه_t minimum_اندازه = 1;
						لكل (اندازه_t شاخص = 0; شاخص < (اندازه_t)مقدار.اندازه(); شاخص++)
							إذا (مقدار[شاخص] != State::S0)
								minimum_اندازه = شاخص + 1;
						مقدار = مقدار.استخراج(0, minimum_اندازه);
					}

					إذا (part.پایه == 2) {
						إذا (part.show_پایه)
							پیشوند_2 += "0b";
						لكل (اندازه_t شاخص = 0; شاخص < (اندازه_t)مقدار.اندازه(); شاخص++) {
							إذا (part.گروه && شاخص > 0 && شاخص % 4 == 0)
								buf += '_';
							RTLIL::State بیت = مقدار[شاخص];
							إذا (بیت == State::Sx)
								buf += 'x';
							وإلا إذا (بیت == State::Sz)
								buf += 'z';
							وإلا إذا (بیت == State::S1)
								buf += '1';
							وإلا /* if (bit == State::S0) */
								buf += '0';
						}
					} وإلا إذا (part.پایه == 8 || part.پایه == 16) {
						إذا (part.show_پایه)
							پیشوند_2 += (part.پایه == 16) ? (part.hex_فوقانی ? "0X" : "0x") : "0o";
						اندازه_t گام = (part.پایه == 16) ? 4 : 3;
						لكل (اندازه_t شاخص = 0; شاخص < (اندازه_t)مقدار.اندازه(); شاخص += گام) {
							إذا (part.گروه && شاخص > 0 && شاخص % (4 * گام) == 0)
								buf += '_';
							RTLIL::Const subvalue = مقدار.استخراج(شاخص, min(گام, مقدار.اندازه() - شاخص));
							منطقي has_x = خطأ, all_x = درست, has_z = خطأ, all_z = درست;
							لكل (State بیت : subvalue) {
								إذا (بیت == State::Sx)
									has_x = درست;
								وإلا
									all_x = خطأ;
								إذا (بیت == State::Sz)
									has_z = درست;
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
								ثابت حرف *digits = part.hex_فوقانی ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} وإلا إذا (part.پایه == 10) {
						إذا (part.show_پایه)
							پیشوند_2 += "0d";
						إذا (all_x)
							buf += 'x';
						وإلا إذا (all_z)
							buf += 'z';
						وإلا إذا (has_x)
							buf += 'X';
						وإلا إذا (has_z)
							buf += 'Z';
						وإلا {
							log_assert(مقدار.is_fully_def());
							إذا (مقدار.is_fully_zero())
								buf += '0';
							اندازه_t شاخص = 0;
							طالما (!مقدار.is_fully_zero())	{
								إذا (part.گروه && شاخص > 0 && شاخص % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(مقدار, 10, خطأ, خطأ, 4).as_int();
								مقدار = RTLIL::const_div(مقدار, 10, خطأ, خطأ, مقدار.اندازه());
								شاخص++;
							}
						}
					} وإلا log_abort();
					إذا (part.justify == FmtPart::NUMERIC && part.گروه && part.padding == '0') {
						اندازه_t گروه_اندازه = part.پایه == 10 ? 3 : 4;
						طالما (پیشوند_2.اندازه() + buf.اندازه() < part.عرض) {
							إذا (buf.اندازه() % (گروه_اندازه + 1) == گروه_اندازه)
								buf += '_';
							buf += '0';
						}
					}
					std::معکوس(buf.begin(), buf.انتها());
				} وإلا إذا (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} وإلا إذا (part.type == FmtPart::VLOG_زمان) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.عرض == 0 || part.padding != '\0');
				إذا (پیشوند_2.اندازه() + buf.اندازه() < part.عرض) {
					اندازه_t pad_عرض = part.عرض - پیشوند_2.اندازه() - buf.اندازه();
					بدل (part.justify) {
						حالة FmtPart::چپ_2:
							str += پیشوند_2;
							str += buf;
							str += std::string(pad_عرض, part.padding);
							اكسر;
						حالة FmtPart::راست_2:
							str += std::string(pad_عرض, part.padding);
							str += پیشوند_2;
							str += buf;
							اكسر;
						حالة FmtPart::NUMERIC:
							str += پیشوند_2;
							str += std::string(pad_عرض, part.padding);
							str += buf;
							اكسر;
					}
				} وإلا {
					str += پیشوند_2;
					str += buf;
				}
				اكسر;
			}
		}
	}

	أرجع str;
}
