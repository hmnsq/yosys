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
	parts.ڈالو_پٹھ(part);
}

فراغ Fmt::parse_rtlil(ثابت RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec دلیلاں_2 = cell->get_پورٹ(ID(دلیلاں));
	parts.clear();

	FmtPart part;
	لكل (ناپ_t i = 0; i < fmt.ناپ(); i++) {
		إذا (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} وإلا إذا (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} وإلا إذا (fmt[i] == '}') {
			log_assert(خطأ && "Unexpected '}' in format string");
		} وإلا إذا (fmt[i] == '{') {
			إذا (!part.str.خالی_تھاواں()) {
				part.type = FmtPart::LITERAL;
				parts.ڈالو_پٹھ(part);
				part = {};
			}

			إذا (++i == fmt.ناپ())
				log_assert(خطأ && "Unexpected end in format substitution");

			ناپ_t arg_ناپ = 0;
			لكل (; i < fmt.ناپ(); i++) {
				إذا (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_ناپ *= 10;
					arg_ناپ += fmt[i] - '0';
				} وإلا إذا (fmt[i] == ':') {
					++i;
					اكسر;
				} وإلا {
					log_assert(خطأ && "Unexpected character in format substitution");
				}
			}
			إذا (i == fmt.ناپ())
				log_assert(خطأ && "Unexpected end in format substitution");

			إذا ((ناپ_t)دلیلاں_2.ناپ() < arg_ناپ)
				log_assert(خطأ && "Format part overruns arguments");
			part.sig = دلیلاں_2.کڈھو(0, arg_ناپ);
			دلیلاں_2.remove(0, arg_ناپ);

			إذا (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				اذهب success;
			}

			إذا (fmt[i] == '>')
				part.justify = FmtPart::دایاں;
			وإلا إذا (fmt[i] == '<')
				part.justify = FmtPart::بایاں;
			وإلا إذا (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			وإلا
				log_assert(خطأ && "Unexpected justification in format substitution");
			إذا (++i == fmt.ناپ())
				log_assert(خطأ && "Unexpected end in format substitution");

			part.padding = fmt[i];
			إذا (++i == fmt.ناپ())
				log_assert(خطأ && "Unexpected end in format substitution");

			لكل (; i < fmt.ناپ(); i++) {
				إذا (fmt[i] >= '0' && fmt[i] <= '9') {
					part.چوڑائی *= 10;
					part.چوڑائی += fmt[i] - '0';
					استمر;
				} وإلا إذا (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.ادھار = 2;
				} وإلا إذا (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.ادھار = 8;
				} وإلا إذا (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.ادھار = 10;
				} وإلا إذا (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.ادھار = 16;
				} وإلا إذا (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.ادھار = 16;
					part.hex_اتلا = سچ;
				} وإلا إذا (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} وإلا إذا (fmt[i] == 't') {
					part.type = FmtPart::VLOG_ویلا;
				} وإلا إذا (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_ویلا;
					part.realtime = سچ;
				} وإلا {
					log_assert(خطأ && "Unexpected character in format substitution");
				}
				++i;
				اكسر;
			}
			إذا (i == fmt.ناپ())
				log_assert(خطأ && "Unexpected end in format substitution");

			إذا (part.type == FmtPart::INTEGER) {
				إذا (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					إذا (++i == fmt.ناپ())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا إذا (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					إذا (++i == fmt.ناپ())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا إذا (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					إذا (++i == fmt.ناپ())
						log_assert(خطأ && "Unexpected end in format substitution");
				} وإلا {
					// also accept no sign character and treat like MINUS for compatibility
				}

				إذا (fmt[i] == '#') {
					part.show_ادھار = سچ;
					++i;
				}
				إذا (fmt[i] == '_') {
					part.گروہ = سچ;
					++i;
				}

				إذا (fmt[i] == 'u')
					part.signed_ = خطأ;
				وإلا إذا (fmt[i] == 's')
					part.signed_ = سچ;
				وإلا
					log_assert(خطأ && "Unexpected character in format substitution");
				إذا (++i == fmt.ناپ())
					log_assert(خطأ && "Unexpected end in format substitution");
			}

		success:
			إذا (fmt[i] != '}')
				log_assert(خطأ && "Expected '}' after format substitution");

			parts.ڈالو_پٹھ(part);
			part = {};
		} وإلا {
			part.str += fmt[i];
		}
	}
	إذا (!part.str.خالی_تھاواں()) {
		part.type = FmtPart::LITERAL;
		parts.ڈالو_پٹھ(part);
	}
}

فراغ Fmt::emit_rtlil(RTLIL::Cell *cell) ثابت {
	std::string fmt;
	RTLIL::SigSpec دلیلاں_2;

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
				log_assert(part.sig.ناپ() <= 32);
				fmt += "{U}";
				اكسر;

			حالة FmtPart::VLOG_ویلا:
				log_assert(part.sig.ناپ() == 0);
				YS_FALLTHROUGH
			حالة FmtPart::STRING:
				log_assert(part.sig.ناپ() % 8 == 0);
				YS_FALLTHROUGH
			حالة FmtPart::INTEGER:
				دلیلاں_2.append(part.sig);
				fmt += '{';
				fmt += std::منزل_string(part.sig.ناپ());
				fmt += ':';
				إذا (part.justify == FmtPart::دایاں)
					fmt += '>';
				وإلا إذا (part.justify == FmtPart::بایاں)
					fmt += '<';
				وإلا إذا (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				وإلا log_abort();
				log_assert(part.چوڑائی == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				إذا (part.چوڑائی > 0)
					fmt += std::منزل_string(part.چوڑائی);
				إذا (part.type == FmtPart::INTEGER) {
					بدل (part.ادھار) {
						حالة  2: fmt += 'b'; اكسر;
						حالة  8: fmt += 'o'; اكسر;
						حالة 10: fmt += 'd'; اكسر;
						حالة 16: fmt += part.hex_اتلا ? 'H' : 'h'; اكسر;
						افتراضي: log_abort();
					}
					بدل (part.sign) {
						حالة FmtPart::MINUS:       fmt += '-'; اكسر;
						حالة FmtPart::PLUS_MINUS:  fmt += '+'; اكسر;
						حالة FmtPart::SPACE_MINUS: fmt += ' '; اكسر;
					}
					fmt += part.show_ادھار ? "#" : "";
					fmt += part.گروہ ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} وإلا إذا (part.type == FmtPart::STRING) {
					fmt += 'c';
				} وإلا إذا (part.type == FmtPart::VLOG_ویلا) {
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

	cell->سیٹ_Param(ID(FORMAT), fmt);
	cell->سیٹ_Param(ID(دلیلاں_چوڑائی), دلیلاں_2.ناپ());
	cell->سیٹ_پورٹ(ID(دلیلاں), دلیلاں_2);
}

ساكن ناپ_t compute_required_decimal_places(ناپ_t ناپ, منطقي signed_)
{
	BigUnsigned max;
	إذا (!signed_)
		max.سیٹ_ثنائی_ہندسہ(ناپ, سچ);
	وإلا
		max.سیٹ_ثنائی_ہندسہ(ناپ - 1, سچ);
	ناپ_t places = 0;
	طالما (!max.isZero()) {
		places++;
		max /= 10;
	}
	إذا (signed_)
		places++;
	أرجع places;
}

ساكن ناپ_t compute_required_nondecimal_places(ناپ_t ناپ, بنانشانی ادھار)
{
	log_assert(ادھار != 10);
	BigUnsigned max;
	max.سیٹ_ثنائی_ہندسہ(ناپ - 1, سچ);
	ناپ_t places = 0;
	طالما (!max.isZero()) {
		places++;
		max /= ادھار;
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
فراغ Fmt::apply_verilog_automatic_sizing_and_جوڑو(FmtPart &part)
{
	إذا (part.ادھار == 10) {
		ناپ_t places = compute_required_decimal_places(part.sig.ناپ(), part.signed_);
		part.padding = ' ';
		part.چوڑائی = std::max(part.چوڑائی, places);
		parts.ڈالو_پٹھ(part);
		أرجع;
	}

	part.padding = '0';

	ناپ_t places = compute_required_nondecimal_places(part.sig.ناپ(), part.ادھار);
	إذا (part.چوڑائی < places) {
		part.justify = FmtPart::دایاں;
		part.چوڑائی = places;
		parts.ڈالو_پٹھ(part);
	} وإلا إذا (part.چوڑائی == places) {
		parts.ڈالو_پٹھ(part);
	} وإلا إذا (part.چوڑائی > places) {
		خودکار gap = std::string(part.چوڑائی - places, ' ');
		part.چوڑائی = places;

		إذا (part.justify == FmtPart::دایاں) {
			append_literal(gap);
			parts.ڈالو_پٹھ(part);
		} وإلا {
			part.justify = FmtPart::دایاں;
			parts.ڈالو_پٹھ(part);
			append_literal(gap);
		}
	}
}

فراغ Fmt::parse_verilog(ثابت std::سمتیہ<VerilogFmtArg> &دلیلاں_2, منطقي sformat_like, صحيح default_ادھار, ثابت std::string &task_name, ثابت std::string &module_name)
{
	parts.clear();

	خودکار arg = دلیلاں_2.begin();
	لكل (; arg != دلیلاں_2.سرا(); ++arg) {
		بدل (arg->type) {
			حالة VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.ادھار = default_ادھار;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_جوڑو(part);
				اكسر;
			}

			حالة VerilogFmtArg::ویلا: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_ویلا;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.چوڑائی = 20;
				parts.ڈالو_پٹھ(part);
				اكسر;
			}

			حالة VerilogFmtArg::STRING: {
				إذا (arg == دلیلاں_2.begin() || !sformat_like) {
					ثابت خودکار fmtarg = arg;
					ثابت std::string &fmt = fmtarg->str;
					FmtPart part = {};
					لكل (ناپ_t i = 0; i < fmt.ناپ(); i++) {
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
							إذا (!part.str.خالی_تھاواں()) {
								part.type = FmtPart::LITERAL;
								parts.ڈالو_پٹھ(part);
								part = {};
							}
							إذا (++i == fmt.ناپ()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - دلیلاں_2.begin() + 1);
							}

							إذا (++arg == دلیلاں_2.سرا()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - دلیلاں_2.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							لكل (; i < fmt.ناپ(); i++) {
								إذا (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::بایاں;
								} وإلا إذا (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} وإلا اكسر;
							}
							إذا (i == fmt.ناپ()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - دلیلاں_2.begin() + 1);
							}

							منطقي has_leading_zero = خطأ, has_چوڑائی = خطأ;
							لكل (; i < fmt.ناپ(); i++) {
								إذا (fmt[i] >= '0' && fmt[i] <= '9') {
									إذا (fmt[i] == '0' && !has_چوڑائی) {
										has_leading_zero = سچ;
									} وإلا {
										has_چوڑائی = سچ;
										part.چوڑائی *= 10;
										part.چوڑائی += fmt[i] - '0';
									}
									استمر;
								} وإلا إذا (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.ادھار =  2;
								} وإلا إذا (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.ادھار =  8;
								} وإلا إذا (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.ادھار = 10;
								} وإلا إذا (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.ادھار = 16;
								} وإلا إذا (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} وإلا إذا (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									إذا ((part.sig.ناپ() % 8) != 0)
										part.sig.extend_u0((part.sig.ناپ() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} وإلا إذا (fmt[i] == 't' || fmt[i] == 'T') {
									إذا (arg->type == VerilogFmtArg::ویلا) {
										part.type = FmtPart::VLOG_ویلا;
										part.realtime = arg->realtime;
										إذا (!has_چوڑائی && !has_leading_zero)
											part.چوڑائی = 20;
									} وإلا {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - دلیلاں_2.begin() + 1);
									}
								} وإلا {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - دلیلاں_2.begin() + 1);
								}
								اكسر;
							}
							إذا (i == fmt.ناپ()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - دلیلاں_2.begin() + 1);
							}

							إذا (part.padding == '\0') {
								إذا (has_leading_zero && part.justify == FmtPart::دایاں) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} وإلا {
									part.padding = ' ';
								}
							}

							إذا (part.type == FmtPart::INTEGER && part.ادھار != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - دلیلاں_2.begin() + 1);

							إذا (part.ادھار != 10)
								part.signed_ = خطأ;
							إذا (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_جوڑو(part);
							وإلا
								parts.ڈالو_پٹھ(part);
							part = {};
						}
					}
					إذا (!part.str.خالی_تھاواں()) {
						part.type = FmtPart::LITERAL;
						parts.ڈالو_پٹھ(part);
					}
				} وإلا {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.ڈالو_پٹھ(part);
				}
				اكسر;
			}

			افتراضي: log_abort();
		}
	}
}

std::سمتیہ<VerilogFmtArg> Fmt::emit_verilog() ثابت
{
	std::سمتیہ<VerilogFmtArg> دلیلاں_2;
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
				دلیلاں_2.ڈالو_پٹھ(arg);

				fmt.str += '%';
				إذا (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				إذا (part.justify == FmtPart::بایاں)
					fmt.str += '-';
				إذا (part.چوڑائی == 0) {
					fmt.str += '0';
				} وإلا إذا (part.چوڑائی > 0) {
					إذا (part.ادھار != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::منزل_string(part.چوڑائی);
				}
				بدل (part.ادھار) {
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
				دلیلاں_2.ڈالو_پٹھ(arg);

				fmt.str += '%';
				إذا (part.justify == FmtPart::بایاں)
					fmt.str += '-';
				إذا (part.sig.ناپ() == 8) {
					إذا (part.چوڑائی > 0) {
						إذا (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::منزل_string(part.چوڑائی);
					}
					fmt.str += 'c';
				} وإلا {
					log_assert(part.sig.ناپ() % 8 == 0);
					إذا (part.چوڑائی > 0) {
						fmt.str += std::منزل_string(part.چوڑائی);
					}
					fmt.str += 's';
				}
				اكسر;
			}

			حالة FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.کڈھو(0, 7); // only ASCII
				دلیلاں_2.ڈالو_پٹھ(arg);

				fmt.str += "%c";
				اكسر;
			}

			حالة FmtPart::VLOG_ویلا: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::ویلا;
				إذا (part.realtime)
					arg.realtime = سچ;
				دلیلاں_2.ڈالو_پٹھ(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				إذا (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				إذا (part.justify == FmtPart::بایاں)
					fmt.str += '-';
				إذا (part.padding == '0' && part.چوڑائی > 0)
					fmt.str += '0';
				fmt.str += std::منزل_string(part.چوڑائی);
				fmt.str += 't';
				اكسر;
			}

			افتراضي: log_abort();
		}
	}

	دلیلاں_2.داخل_کرو(دلیلاں_2.begin(), fmt);
	أرجع دلیلاں_2;
}

std::string escape_cxx_string(ثابت std::string &داخلہ)
{
	std::string output = "\"";
	لكل (خودکار c : داخلہ) {
		إذا (::isprint(c)) {
			إذا (c == '\\' || c == '"')
				output.ڈالو_پٹھ('\\');
			output.ڈالو_پٹھ(c);
		} وإلا {
			حرف l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.ڈالو_پٹھ('\\');
			output.ڈالو_پٹھ('0' + h);
			output.ڈالو_پٹھ('0' + m);
			output.ڈالو_پٹھ('0' + l);
		}
	}
	output.ڈالو_پٹھ('"');
	إذا (output.لبھو('\0') != std::string::npos) {
		output.داخل_کرو(0, "std::string {");
		output.append(stringf(", %zu}", داخلہ.ناپ()));
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
			حالة FmtPart::VLOG_ویلا: os << "VLOG_TIME"; اكسر;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		بدل (part.justify) {
			حالة FmtPart::بایاں:    os << "LEFT";    اكسر;
			حالة FmtPart::دایاں:   os << "RIGHT";   اكسر;
			حالة FmtPart::NUMERIC: os << "NUMERIC"; اكسر;
		}
		os << ", ";
		os << "(char)" << (صحيح)part.padding << ", ";
		os << part.چوڑائی << ", ";
		os << part.ادھار << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		بدل (part.sign) {
			حالة FmtPart::MINUS:       os << "MINUS";       اكسر;
			حالة FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  اكسر;
			حالة FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; اكسر;
		}
		os << ", ";
		os << part.hex_اتلا << ", ";
		os << part.show_ادھار << ", ";
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

	لكل (خودکار &part : parts) {
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
			حالة FmtPart::VLOG_ویلا: {
				std::string buf;
				std::string اگیتر_2;
				إذا (part.type == FmtPart::INTEGER) {
					RTLIL::Const قدر = part.sig.as_const();
					منطقي has_x = خطأ, all_x = سچ, has_z = خطأ, all_z = سچ;
					لكل (State ثنائی_ہندسہ : قدر) {
						إذا (ثنائی_ہندسہ == State::Sx)
							has_x = سچ;
						وإلا
							all_x = خطأ;
						إذا (ثنائی_ہندسہ == State::Sz)
							has_z = سچ;
						وإلا
							all_z = خطأ;
					}

					إذا (!has_z && !has_x && part.signed_ && قدر[قدر.ناپ() - 1]) {
						اگیتر_2 = "-";
						قدر = RTLIL::const_neg(قدر, {}, part.signed_, {}, قدر.ناپ() + 1);
					} وإلا {
						بدل (part.sign) {
							حالة FmtPart::MINUS:       اكسر;
							حالة FmtPart::PLUS_MINUS:  اگیتر_2 = "+"; اكسر;
							حالة FmtPart::SPACE_MINUS: اگیتر_2 = " "; اكسر;
						}
					}

					إذا (part.ادھار != 10) {
						ناپ_t minimum_ناپ = 1;
						لكل (ناپ_t اشاریہ = 0; اشاریہ < (ناپ_t)قدر.ناپ(); اشاریہ++)
							إذا (قدر[اشاریہ] != State::S0)
								minimum_ناپ = اشاریہ + 1;
						قدر = قدر.کڈھو(0, minimum_ناپ);
					}

					إذا (part.ادھار == 2) {
						إذا (part.show_ادھار)
							اگیتر_2 += "0b";
						لكل (ناپ_t اشاریہ = 0; اشاریہ < (ناپ_t)قدر.ناپ(); اشاریہ++) {
							إذا (part.گروہ && اشاریہ > 0 && اشاریہ % 4 == 0)
								buf += '_';
							RTLIL::State ثنائی_ہندسہ = قدر[اشاریہ];
							إذا (ثنائی_ہندسہ == State::Sx)
								buf += 'x';
							وإلا إذا (ثنائی_ہندسہ == State::Sz)
								buf += 'z';
							وإلا إذا (ثنائی_ہندسہ == State::S1)
								buf += '1';
							وإلا /* if (bit == State::S0) */
								buf += '0';
						}
					} وإلا إذا (part.ادھار == 8 || part.ادھار == 16) {
						إذا (part.show_ادھار)
							اگیتر_2 += (part.ادھار == 16) ? (part.hex_اتلا ? "0X" : "0x") : "0o";
						ناپ_t قدم = (part.ادھار == 16) ? 4 : 3;
						لكل (ناپ_t اشاریہ = 0; اشاریہ < (ناپ_t)قدر.ناپ(); اشاریہ += قدم) {
							إذا (part.گروہ && اشاریہ > 0 && اشاریہ % (4 * قدم) == 0)
								buf += '_';
							RTLIL::Const subvalue = قدر.کڈھو(اشاریہ, min(قدم, قدر.ناپ() - اشاریہ));
							منطقي has_x = خطأ, all_x = سچ, has_z = خطأ, all_z = سچ;
							لكل (State ثنائی_ہندسہ : subvalue) {
								إذا (ثنائی_ہندسہ == State::Sx)
									has_x = سچ;
								وإلا
									all_x = خطأ;
								إذا (ثنائی_ہندسہ == State::Sz)
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
								ثابت حرف *digits = part.hex_اتلا ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} وإلا إذا (part.ادھار == 10) {
						إذا (part.show_ادھار)
							اگیتر_2 += "0d";
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
							ناپ_t اشاریہ = 0;
							طالما (!قدر.is_fully_zero())	{
								إذا (part.گروہ && اشاریہ > 0 && اشاریہ % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(قدر, 10, خطأ, خطأ, 4).as_int();
								قدر = RTLIL::const_div(قدر, 10, خطأ, خطأ, قدر.ناپ());
								اشاریہ++;
							}
						}
					} وإلا log_abort();
					إذا (part.justify == FmtPart::NUMERIC && part.گروہ && part.padding == '0') {
						ناپ_t گروہ_ناپ = part.ادھار == 10 ? 3 : 4;
						طالما (اگیتر_2.ناپ() + buf.ناپ() < part.چوڑائی) {
							إذا (buf.ناپ() % (گروہ_ناپ + 1) == گروہ_ناپ)
								buf += '_';
							buf += '0';
						}
					}
					std::الٹ(buf.begin(), buf.سرا());
				} وإلا إذا (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} وإلا إذا (part.type == FmtPart::VLOG_ویلا) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.چوڑائی == 0 || part.padding != '\0');
				إذا (اگیتر_2.ناپ() + buf.ناپ() < part.چوڑائی) {
					ناپ_t pad_چوڑائی = part.چوڑائی - اگیتر_2.ناپ() - buf.ناپ();
					بدل (part.justify) {
						حالة FmtPart::بایاں:
							str += اگیتر_2;
							str += buf;
							str += std::string(pad_چوڑائی, part.padding);
							اكسر;
						حالة FmtPart::دایاں:
							str += std::string(pad_چوڑائی, part.padding);
							str += اگیتر_2;
							str += buf;
							اكسر;
						حالة FmtPart::NUMERIC:
							str += اگیتر_2;
							str += std::string(pad_چوڑائی, part.padding);
							str += buf;
							اكسر;
					}
				} وإلا {
					str += اگیتر_2;
					str += buf;
				}
				اكسر;
			}
		}
	}

	أرجع str;
}
