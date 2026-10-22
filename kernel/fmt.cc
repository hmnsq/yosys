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

#అన్తర్భావ "libs/bigint/BigUnsigned.hh"
#అన్తర్భావ "kernel/fmt.h"

USING_YOSYS_NAMESPACE

శూన్య Fmt::append_literal(స్థిరరాశి std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.నెట్టు_వెనుక(part);
}

శూన్య Fmt::parse_rtlil(స్థిరరాశి RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec వాదనలు_2 = cell->get_పోర్టు(ID(వాదనలు));
	parts.clear();

	FmtPart part;
	కొరకు (కొలత_t i = 0; i < fmt.కొలత(); i++) {
		ఒకవేళ (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} లేకపోతే ఒకవేళ (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} లేకపోతే ఒకవేళ (fmt[i] == '}') {
			log_assert(అబద్ధం && "Unexpected '}' in format string");
		} లేకపోతే ఒకవేళ (fmt[i] == '{') {
			ఒకవేళ (!part.str.ఖాళీలు()) {
				part.type = FmtPart::LITERAL;
				parts.నెట్టు_వెనుక(part);
				part = {};
			}

			ఒకవేళ (++i == fmt.కొలత())
				log_assert(అబద్ధం && "Unexpected end in format substitution");

			కొలత_t arg_కొలత = 0;
			కొరకు (; i < fmt.కొలత(); i++) {
				ఒకవేళ (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_కొలత *= 10;
					arg_కొలత += fmt[i] - '0';
				} లేకపోతే ఒకవేళ (fmt[i] == ':') {
					++i;
					ఆపు;
				} లేకపోతే {
					log_assert(అబద్ధం && "Unexpected character in format substitution");
				}
			}
			ఒకవేళ (i == fmt.కొలత())
				log_assert(అబద్ధం && "Unexpected end in format substitution");

			ఒకవేళ ((కొలత_t)వాదనలు_2.కొలత() < arg_కొలత)
				log_assert(అబద్ధం && "Format part overruns arguments");
			part.sig = వాదనలు_2.వెలికితీయి(0, arg_కొలత);
			వాదనలు_2.remove(0, arg_కొలత);

			ఒకవేళ (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				వెళ్ళు success;
			}

			ఒకవేళ (fmt[i] == '>')
				part.justify = FmtPart::కుడి_2;
			లేకపోతే ఒకవేళ (fmt[i] == '<')
				part.justify = FmtPart::ఎడమ_2;
			లేకపోతే ఒకవేళ (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			లేకపోతే
				log_assert(అబద్ధం && "Unexpected justification in format substitution");
			ఒకవేళ (++i == fmt.కొలత())
				log_assert(అబద్ధం && "Unexpected end in format substitution");

			part.padding = fmt[i];
			ఒకవేళ (++i == fmt.కొలత())
				log_assert(అబద్ధం && "Unexpected end in format substitution");

			కొరకు (; i < fmt.కొలత(); i++) {
				ఒకవేళ (fmt[i] >= '0' && fmt[i] <= '9') {
					part.వెడల్పు *= 10;
					part.వెడల్పు += fmt[i] - '0';
					కొనసాగు;
				} లేకపోతే ఒకవేళ (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.పునాది = 2;
				} లేకపోతే ఒకవేళ (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.పునాది = 8;
				} లేకపోతే ఒకవేళ (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.పునాది = 10;
				} లేకపోతే ఒకవేళ (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.పునాది = 16;
				} లేకపోతే ఒకవేళ (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.పునాది = 16;
					part.hex_పై = నిజం;
				} లేకపోతే ఒకవేళ (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} లేకపోతే ఒకవేళ (fmt[i] == 't') {
					part.type = FmtPart::VLOG_సమయం;
				} లేకపోతే ఒకవేళ (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_సమయం;
					part.realtime = నిజం;
				} లేకపోతే {
					log_assert(అబద్ధం && "Unexpected character in format substitution");
				}
				++i;
				ఆపు;
			}
			ఒకవేళ (i == fmt.కొలత())
				log_assert(అబద్ధం && "Unexpected end in format substitution");

			ఒకవేళ (part.type == FmtPart::INTEGER) {
				ఒకవేళ (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					ఒకవేళ (++i == fmt.కొలత())
						log_assert(అబద్ధం && "Unexpected end in format substitution");
				} లేకపోతే ఒకవేళ (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					ఒకవేళ (++i == fmt.కొలత())
						log_assert(అబద్ధం && "Unexpected end in format substitution");
				} లేకపోతే ఒకవేళ (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					ఒకవేళ (++i == fmt.కొలత())
						log_assert(అబద్ధం && "Unexpected end in format substitution");
				} లేకపోతే {
					// also accept no sign character and treat like MINUS for compatibility
				}

				ఒకవేళ (fmt[i] == '#') {
					part.show_పునాది = నిజం;
					++i;
				}
				ఒకవేళ (fmt[i] == '_') {
					part.సమూహం = నిజం;
					++i;
				}

				ఒకవేళ (fmt[i] == 'u')
					part.signed_ = అబద్ధం;
				లేకపోతే ఒకవేళ (fmt[i] == 's')
					part.signed_ = నిజం;
				లేకపోతే
					log_assert(అబద్ధం && "Unexpected character in format substitution");
				ఒకవేళ (++i == fmt.కొలత())
					log_assert(అబద్ధం && "Unexpected end in format substitution");
			}

		success:
			ఒకవేళ (fmt[i] != '}')
				log_assert(అబద్ధం && "Expected '}' after format substitution");

			parts.నెట్టు_వెనుక(part);
			part = {};
		} లేకపోతే {
			part.str += fmt[i];
		}
	}
	ఒకవేళ (!part.str.ఖాళీలు()) {
		part.type = FmtPart::LITERAL;
		parts.నెట్టు_వెనుక(part);
	}
}

శూన్య Fmt::emit_rtlil(RTLIL::Cell *cell) స్థిరరాశి {
	std::string fmt;
	RTLIL::SigSpec వాదనలు_2;

	కొరకు (స్వయంచాలక &part : parts) {
		ఎంపిక (part.type) {
			స్థితి FmtPart::LITERAL:
				కొరకు (అక్షరం c : part.str) {
					ఒకవేళ (c == '{')
						fmt += "{{";
					లేకపోతే ఒకవేళ (c == '}')
						fmt += "}}";
					లేకపోతే
						fmt += c;
				}
				ఆపు;

			స్థితి FmtPart::UNICHAR:
				log_assert(part.sig.కొలత() <= 32);
				fmt += "{U}";
				ఆపు;

			స్థితి FmtPart::VLOG_సమయం:
				log_assert(part.sig.కొలత() == 0);
				YS_FALLTHROUGH
			స్థితి FmtPart::STRING:
				log_assert(part.sig.కొలత() % 8 == 0);
				YS_FALLTHROUGH
			స్థితి FmtPart::INTEGER:
				వాదనలు_2.append(part.sig);
				fmt += '{';
				fmt += std::గమ్యం_string(part.sig.కొలత());
				fmt += ':';
				ఒకవేళ (part.justify == FmtPart::కుడి_2)
					fmt += '>';
				లేకపోతే ఒకవేళ (part.justify == FmtPart::ఎడమ_2)
					fmt += '<';
				లేకపోతే ఒకవేళ (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				లేకపోతే log_abort();
				log_assert(part.వెడల్పు == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				ఒకవేళ (part.వెడల్పు > 0)
					fmt += std::గమ్యం_string(part.వెడల్పు);
				ఒకవేళ (part.type == FmtPart::INTEGER) {
					ఎంపిక (part.పునాది) {
						స్థితి  2: fmt += 'b'; ఆపు;
						స్థితి  8: fmt += 'o'; ఆపు;
						స్థితి 10: fmt += 'd'; ఆపు;
						స్థితి 16: fmt += part.hex_పై ? 'H' : 'h'; ఆపు;
						అప్రమేయ: log_abort();
					}
					ఎంపిక (part.sign) {
						స్థితి FmtPart::MINUS:       fmt += '-'; ఆపు;
						స్థితి FmtPart::PLUS_MINUS:  fmt += '+'; ఆపు;
						స్థితి FmtPart::SPACE_MINUS: fmt += ' '; ఆపు;
					}
					fmt += part.show_పునాది ? "#" : "";
					fmt += part.సమూహం ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} లేకపోతే ఒకవేళ (part.type == FmtPart::STRING) {
					fmt += 'c';
				} లేకపోతే ఒకవేళ (part.type == FmtPart::VLOG_సమయం) {
					ఒకవేళ (part.realtime)
						fmt += 'r';
					లేకపోతే
						fmt += 't';
				} లేకపోతే log_abort();
				fmt += '}';
				ఆపు;

			అప్రమేయ: log_abort();
		}
	}

	cell->సమితి_Param(ID(FORMAT), fmt);
	cell->సమితి_Param(ID(వాదనలు_వెడల్పు), వాదనలు_2.కొలత());
	cell->సమితి_పోర్టు(ID(వాదనలు), వాదనలు_2);
}

స్థిర కొలత_t compute_required_decimal_places(కొలత_t కొలత, తార్కిక signed_)
{
	BigUnsigned max;
	ఒకవేళ (!signed_)
		max.సమితి_బిట్టు(కొలత, నిజం);
	లేకపోతే
		max.సమితి_బిట్టు(కొలత - 1, నిజం);
	కొలత_t places = 0;
	అయితే (!max.isZero()) {
		places++;
		max /= 10;
	}
	ఒకవేళ (signed_)
		places++;
	తిరిగి places;
}

స్థిర కొలత_t compute_required_nondecimal_places(కొలత_t కొలత, గుర్తులేని పునాది)
{
	log_assert(పునాది != 10);
	BigUnsigned max;
	max.సమితి_బిట్టు(కొలత - 1, నిజం);
	కొలత_t places = 0;
	అయితే (!max.isZero()) {
		places++;
		max /= పునాది;
	}
	తిరిగి places;
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
శూన్య Fmt::apply_verilog_automatic_sizing_and_కూడు(FmtPart &part)
{
	ఒకవేళ (part.పునాది == 10) {
		కొలత_t places = compute_required_decimal_places(part.sig.కొలత(), part.signed_);
		part.padding = ' ';
		part.వెడల్పు = std::max(part.వెడల్పు, places);
		parts.నెట్టు_వెనుక(part);
		తిరిగి;
	}

	part.padding = '0';

	కొలత_t places = compute_required_nondecimal_places(part.sig.కొలత(), part.పునాది);
	ఒకవేళ (part.వెడల్పు < places) {
		part.justify = FmtPart::కుడి_2;
		part.వెడల్పు = places;
		parts.నెట్టు_వెనుక(part);
	} లేకపోతే ఒకవేళ (part.వెడల్పు == places) {
		parts.నెట్టు_వెనుక(part);
	} లేకపోతే ఒకవేళ (part.వెడల్పు > places) {
		స్వయంచాలక gap = std::string(part.వెడల్పు - places, ' ');
		part.వెడల్పు = places;

		ఒకవేళ (part.justify == FmtPart::కుడి_2) {
			append_literal(gap);
			parts.నెట్టు_వెనుక(part);
		} లేకపోతే {
			part.justify = FmtPart::కుడి_2;
			parts.నెట్టు_వెనుక(part);
			append_literal(gap);
		}
	}
}

శూన్య Fmt::parse_verilog(స్థిరరాశి std::సదిశ<VerilogFmtArg> &వాదనలు_2, తార్కిక sformat_like, పూర్ణాంకం default_పునాది, స్థిరరాశి std::string &task_name, స్థిరరాశి std::string &module_name)
{
	parts.clear();

	స్వయంచాలక arg = వాదనలు_2.begin();
	కొరకు (; arg != వాదనలు_2.చివర(); ++arg) {
		ఎంపిక (arg->type) {
			స్థితి VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.పునాది = default_పునాది;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_కూడు(part);
				ఆపు;
			}

			స్థితి VerilogFmtArg::సమయం: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_సమయం;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.వెడల్పు = 20;
				parts.నెట్టు_వెనుక(part);
				ఆపు;
			}

			స్థితి VerilogFmtArg::STRING: {
				ఒకవేళ (arg == వాదనలు_2.begin() || !sformat_like) {
					స్థిరరాశి స్వయంచాలక fmtarg = arg;
					స్థిరరాశి std::string &fmt = fmtarg->str;
					FmtPart part = {};
					కొరకు (కొలత_t i = 0; i < fmt.కొలత(); i++) {
						ఒకవేళ (fmt[i] != '%') {
							part.str += fmt[i];
						} లేకపోతే ఒకవేళ (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} లేకపోతే ఒకవేళ (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} లేకపోతే ఒకవేళ (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} లేకపోతే {
							ఒకవేళ (!part.str.ఖాళీలు()) {
								part.type = FmtPart::LITERAL;
								parts.నెట్టు_వెనుక(part);
								part = {};
							}
							ఒకవేళ (++i == fmt.కొలత()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - వాదనలు_2.begin() + 1);
							}

							ఒకవేళ (++arg == వాదనలు_2.చివర()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - వాదనలు_2.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							కొరకు (; i < fmt.కొలత(); i++) {
								ఒకవేళ (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::ఎడమ_2;
								} లేకపోతే ఒకవేళ (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} లేకపోతే ఆపు;
							}
							ఒకవేళ (i == fmt.కొలత()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - వాదనలు_2.begin() + 1);
							}

							తార్కిక has_leading_zero = అబద్ధం, has_వెడల్పు = అబద్ధం;
							కొరకు (; i < fmt.కొలత(); i++) {
								ఒకవేళ (fmt[i] >= '0' && fmt[i] <= '9') {
									ఒకవేళ (fmt[i] == '0' && !has_వెడల్పు) {
										has_leading_zero = నిజం;
									} లేకపోతే {
										has_వెడల్పు = నిజం;
										part.వెడల్పు *= 10;
										part.వెడల్పు += fmt[i] - '0';
									}
									కొనసాగు;
								} లేకపోతే ఒకవేళ (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.పునాది =  2;
								} లేకపోతే ఒకవేళ (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.పునాది =  8;
								} లేకపోతే ఒకవేళ (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.పునాది = 10;
								} లేకపోతే ఒకవేళ (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.పునాది = 16;
								} లేకపోతే ఒకవేళ (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} లేకపోతే ఒకవేళ (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									ఒకవేళ ((part.sig.కొలత() % 8) != 0)
										part.sig.extend_u0((part.sig.కొలత() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} లేకపోతే ఒకవేళ (fmt[i] == 't' || fmt[i] == 'T') {
									ఒకవేళ (arg->type == VerilogFmtArg::సమయం) {
										part.type = FmtPart::VLOG_సమయం;
										part.realtime = arg->realtime;
										ఒకవేళ (!has_వెడల్పు && !has_leading_zero)
											part.వెడల్పు = 20;
									} లేకపోతే {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - వాదనలు_2.begin() + 1);
									}
								} లేకపోతే {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - వాదనలు_2.begin() + 1);
								}
								ఆపు;
							}
							ఒకవేళ (i == fmt.కొలత()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - వాదనలు_2.begin() + 1);
							}

							ఒకవేళ (part.padding == '\0') {
								ఒకవేళ (has_leading_zero && part.justify == FmtPart::కుడి_2) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} లేకపోతే {
									part.padding = ' ';
								}
							}

							ఒకవేళ (part.type == FmtPart::INTEGER && part.పునాది != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - వాదనలు_2.begin() + 1);

							ఒకవేళ (part.పునాది != 10)
								part.signed_ = అబద్ధం;
							ఒకవేళ (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_కూడు(part);
							లేకపోతే
								parts.నెట్టు_వెనుక(part);
							part = {};
						}
					}
					ఒకవేళ (!part.str.ఖాళీలు()) {
						part.type = FmtPart::LITERAL;
						parts.నెట్టు_వెనుక(part);
					}
				} లేకపోతే {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.నెట్టు_వెనుక(part);
				}
				ఆపు;
			}

			అప్రమేయ: log_abort();
		}
	}
}

std::సదిశ<VerilogFmtArg> Fmt::emit_verilog() స్థిరరాశి
{
	std::సదిశ<VerilogFmtArg> వాదనలు_2;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	కొరకు (స్వయంచాలక &part : parts) {
		ఎంపిక (part.type) {
			స్థితి FmtPart::LITERAL:
				కొరకు (అక్షరం c : part.str) {
					ఒకవేళ (c == '%')
						fmt.str += "%%";
					లేకపోతే
						fmt.str += c;
				}
				ఆపు;

			స్థితి FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				వాదనలు_2.నెట్టు_వెనుక(arg);

				fmt.str += '%';
				ఒకవేళ (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				ఒకవేళ (part.justify == FmtPart::ఎడమ_2)
					fmt.str += '-';
				ఒకవేళ (part.వెడల్పు == 0) {
					fmt.str += '0';
				} లేకపోతే ఒకవేళ (part.వెడల్పు > 0) {
					ఒకవేళ (part.పునాది != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::గమ్యం_string(part.వెడల్పు);
				}
				ఎంపిక (part.పునాది) {
					స్థితి  2: fmt.str += 'b'; ఆపు;
					స్థితి  8: fmt.str += 'o'; ఆపు;
					స్థితి 10: fmt.str += 'd'; ఆపు;
					స్థితి 16: fmt.str += 'h'; ఆపు; // treat uppercase hex as lowercase
					అప్రమేయ: log_abort();
				}
				ఆపు;
			}

			స్థితి FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				వాదనలు_2.నెట్టు_వెనుక(arg);

				fmt.str += '%';
				ఒకవేళ (part.justify == FmtPart::ఎడమ_2)
					fmt.str += '-';
				ఒకవేళ (part.sig.కొలత() == 8) {
					ఒకవేళ (part.వెడల్పు > 0) {
						ఒకవేళ (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::గమ్యం_string(part.వెడల్పు);
					}
					fmt.str += 'c';
				} లేకపోతే {
					log_assert(part.sig.కొలత() % 8 == 0);
					ఒకవేళ (part.వెడల్పు > 0) {
						fmt.str += std::గమ్యం_string(part.వెడల్పు);
					}
					fmt.str += 's';
				}
				ఆపు;
			}

			స్థితి FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.వెలికితీయి(0, 7); // only ASCII
				వాదనలు_2.నెట్టు_వెనుక(arg);

				fmt.str += "%c";
				ఆపు;
			}

			స్థితి FmtPart::VLOG_సమయం: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::సమయం;
				ఒకవేళ (part.realtime)
					arg.realtime = నిజం;
				వాదనలు_2.నెట్టు_వెనుక(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				ఒకవేళ (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				ఒకవేళ (part.justify == FmtPart::ఎడమ_2)
					fmt.str += '-';
				ఒకవేళ (part.padding == '0' && part.వెడల్పు > 0)
					fmt.str += '0';
				fmt.str += std::గమ్యం_string(part.వెడల్పు);
				fmt.str += 't';
				ఆపు;
			}

			అప్రమేయ: log_abort();
		}
	}

	వాదనలు_2.చొప్పించు(వాదనలు_2.begin(), fmt);
	తిరిగి వాదనలు_2;
}

std::string escape_cxx_string(స్థిరరాశి std::string &ఆదానం)
{
	std::string output = "\"";
	కొరకు (స్వయంచాలక c : ఆదానం) {
		ఒకవేళ (::isprint(c)) {
			ఒకవేళ (c == '\\' || c == '"')
				output.నెట్టు_వెనుక('\\');
			output.నెట్టు_వెనుక(c);
		} లేకపోతే {
			అక్షరం l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.నెట్టు_వెనుక('\\');
			output.నెట్టు_వెనుక('0' + h);
			output.నెట్టు_వెనుక('0' + m);
			output.నెట్టు_వెనుక('0' + l);
		}
	}
	output.నెట్టు_వెనుక('"');
	ఒకవేళ (output.కనుగొను('\0') != std::string::npos) {
		output.చొప్పించు(0, "std::string {");
		output.append(stringf(", %zu}", ఆదానం.కొలత()));
	}
	తిరిగి output;
}

శూన్య Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<శూన్య(స్థిరరాశి RTLIL::SigSpec &)> emit_sig, స్థిరరాశి std::string &context) స్థిరరాశి
{
	os << indent << "std::string buf;\n";
	కొరకు (స్వయంచాలక &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		ఎంపిక (part.type) {
			స్థితి FmtPart::LITERAL:   os << "LITERAL";   ఆపు;
			స్థితి FmtPart::INTEGER:   os << "INTEGER";   ఆపు;
			స్థితి FmtPart::STRING:    os << "STRING";    ఆపు;
			స్థితి FmtPart::UNICHAR:   os << "UNICHAR";   ఆపు;
			స్థితి FmtPart::VLOG_సమయం: os << "VLOG_TIME"; ఆపు;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		ఎంపిక (part.justify) {
			స్థితి FmtPart::ఎడమ_2:    os << "LEFT";    ఆపు;
			స్థితి FmtPart::కుడి_2:   os << "RIGHT";   ఆపు;
			స్థితి FmtPart::NUMERIC: os << "NUMERIC"; ఆపు;
		}
		os << ", ";
		os << "(char)" << (పూర్ణాంకం)part.padding << ", ";
		os << part.వెడల్పు << ", ";
		os << part.పునాది << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		ఎంపిక (part.sign) {
			స్థితి FmtPart::MINUS:       os << "MINUS";       ఆపు;
			స్థితి FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  ఆపు;
			స్థితి FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; ఆపు;
		}
		os << ", ";
		os << part.hex_పై << ", ";
		os << part.show_పునాది << ", ";
		os << part.సమూహం << ", ";
		os << part.realtime;
		os << " }.render(";
		emit_sig(part.sig);
		os << ", " << context << ");\n";
	}
	os << indent << "return buf;\n";
}

std::string Fmt::render() స్థిరరాశి
{
	std::string str;

	కొరకు (స్వయంచాలక &part : parts) {
		ఎంపిక (part.type) {
			స్థితి FmtPart::LITERAL:
				str += part.str;
				ఆపు;

			స్థితి FmtPart::UNICHAR: {
				RTLIL::Const విలువ = part.sig.as_const();
				uint32_t codepoint = విలువ.as_int();
				ఒకవేళ (codepoint >= 0x10000)
					str += (అక్షరం)(0xf0 |  (codepoint >> 18));
				లేకపోతే ఒకవేళ (codepoint >= 0x800)
					str += (అక్షరం)(0xe0 |  (codepoint >> 12));
				లేకపోతే ఒకవేళ (codepoint >= 0x80)
					str += (అక్షరం)(0xc0 |  (codepoint >>  6));
				లేకపోతే
					str += (అక్షరం)codepoint;
				ఒకవేళ (codepoint >= 0x10000)
					str += (అక్షరం)(0x80 | ((codepoint >> 12) & 0x3f));
				ఒకవేళ (codepoint >= 0x800)
					str += (అక్షరం)(0x80 | ((codepoint >>  6) & 0x3f));
				ఒకవేళ (codepoint >= 0x80)
					str += (అక్షరం)(0x80 | ((codepoint >>  0) & 0x3f));
				ఆపు;
			}

			స్థితి FmtPart::INTEGER:
			స్థితి FmtPart::STRING:
			స్థితి FmtPart::VLOG_సమయం: {
				std::string buf;
				std::string ఉపసర్గ_2;
				ఒకవేళ (part.type == FmtPart::INTEGER) {
					RTLIL::Const విలువ = part.sig.as_const();
					తార్కిక has_x = అబద్ధం, all_x = నిజం, has_z = అబద్ధం, all_z = నిజం;
					కొరకు (State బిట్టు : విలువ) {
						ఒకవేళ (బిట్టు == State::Sx)
							has_x = నిజం;
						లేకపోతే
							all_x = అబద్ధం;
						ఒకవేళ (బిట్టు == State::Sz)
							has_z = నిజం;
						లేకపోతే
							all_z = అబద్ధం;
					}

					ఒకవేళ (!has_z && !has_x && part.signed_ && విలువ[విలువ.కొలత() - 1]) {
						ఉపసర్గ_2 = "-";
						విలువ = RTLIL::const_neg(విలువ, {}, part.signed_, {}, విలువ.కొలత() + 1);
					} లేకపోతే {
						ఎంపిక (part.sign) {
							స్థితి FmtPart::MINUS:       ఆపు;
							స్థితి FmtPart::PLUS_MINUS:  ఉపసర్గ_2 = "+"; ఆపు;
							స్థితి FmtPart::SPACE_MINUS: ఉపసర్గ_2 = " "; ఆపు;
						}
					}

					ఒకవేళ (part.పునాది != 10) {
						కొలత_t minimum_కొలత = 1;
						కొరకు (కొలత_t సూచీ = 0; సూచీ < (కొలత_t)విలువ.కొలత(); సూచీ++)
							ఒకవేళ (విలువ[సూచీ] != State::S0)
								minimum_కొలత = సూచీ + 1;
						విలువ = విలువ.వెలికితీయి(0, minimum_కొలత);
					}

					ఒకవేళ (part.పునాది == 2) {
						ఒకవేళ (part.show_పునాది)
							ఉపసర్గ_2 += "0b";
						కొరకు (కొలత_t సూచీ = 0; సూచీ < (కొలత_t)విలువ.కొలత(); సూచీ++) {
							ఒకవేళ (part.సమూహం && సూచీ > 0 && సూచీ % 4 == 0)
								buf += '_';
							RTLIL::State బిట్టు = విలువ[సూచీ];
							ఒకవేళ (బిట్టు == State::Sx)
								buf += 'x';
							లేకపోతే ఒకవేళ (బిట్టు == State::Sz)
								buf += 'z';
							లేకపోతే ఒకవేళ (బిట్టు == State::S1)
								buf += '1';
							లేకపోతే /* if (bit == State::S0) */
								buf += '0';
						}
					} లేకపోతే ఒకవేళ (part.పునాది == 8 || part.పునాది == 16) {
						ఒకవేళ (part.show_పునాది)
							ఉపసర్గ_2 += (part.పునాది == 16) ? (part.hex_పై ? "0X" : "0x") : "0o";
						కొలత_t అడుగు = (part.పునాది == 16) ? 4 : 3;
						కొరకు (కొలత_t సూచీ = 0; సూచీ < (కొలత_t)విలువ.కొలత(); సూచీ += అడుగు) {
							ఒకవేళ (part.సమూహం && సూచీ > 0 && సూచీ % (4 * అడుగు) == 0)
								buf += '_';
							RTLIL::Const subvalue = విలువ.వెలికితీయి(సూచీ, min(అడుగు, విలువ.కొలత() - సూచీ));
							తార్కిక has_x = అబద్ధం, all_x = నిజం, has_z = అబద్ధం, all_z = నిజం;
							కొరకు (State బిట్టు : subvalue) {
								ఒకవేళ (బిట్టు == State::Sx)
									has_x = నిజం;
								లేకపోతే
									all_x = అబద్ధం;
								ఒకవేళ (బిట్టు == State::Sz)
									has_z = నిజం;
								లేకపోతే
									all_z = అబద్ధం;
							}
							ఒకవేళ (all_x)
								buf += 'x';
							లేకపోతే ఒకవేళ (all_z)
								buf += 'z';
							లేకపోతే ఒకవేళ (has_x)
								buf += 'X';
							లేకపోతే ఒకవేళ (has_z)
								buf += 'Z';
							లేకపోతే {
								స్థిరరాశి అక్షరం *digits = part.hex_పై ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} లేకపోతే ఒకవేళ (part.పునాది == 10) {
						ఒకవేళ (part.show_పునాది)
							ఉపసర్గ_2 += "0d";
						ఒకవేళ (all_x)
							buf += 'x';
						లేకపోతే ఒకవేళ (all_z)
							buf += 'z';
						లేకపోతే ఒకవేళ (has_x)
							buf += 'X';
						లేకపోతే ఒకవేళ (has_z)
							buf += 'Z';
						లేకపోతే {
							log_assert(విలువ.is_fully_def());
							ఒకవేళ (విలువ.is_fully_zero())
								buf += '0';
							కొలత_t సూచీ = 0;
							అయితే (!విలువ.is_fully_zero())	{
								ఒకవేళ (part.సమూహం && సూచీ > 0 && సూచీ % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(విలువ, 10, అబద్ధం, అబద్ధం, 4).as_int();
								విలువ = RTLIL::const_div(విలువ, 10, అబద్ధం, అబద్ధం, విలువ.కొలత());
								సూచీ++;
							}
						}
					} లేకపోతే log_abort();
					ఒకవేళ (part.justify == FmtPart::NUMERIC && part.సమూహం && part.padding == '0') {
						కొలత_t సమూహం_కొలత = part.పునాది == 10 ? 3 : 4;
						అయితే (ఉపసర్గ_2.కొలత() + buf.కొలత() < part.వెడల్పు) {
							ఒకవేళ (buf.కొలత() % (సమూహం_కొలత + 1) == సమూహం_కొలత)
								buf += '_';
							buf += '0';
						}
					}
					std::తిరగేయి(buf.begin(), buf.చివర());
				} లేకపోతే ఒకవేళ (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} లేకపోతే ఒకవేళ (part.type == FmtPart::VLOG_సమయం) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.వెడల్పు == 0 || part.padding != '\0');
				ఒకవేళ (ఉపసర్గ_2.కొలత() + buf.కొలత() < part.వెడల్పు) {
					కొలత_t pad_వెడల్పు = part.వెడల్పు - ఉపసర్గ_2.కొలత() - buf.కొలత();
					ఎంపిక (part.justify) {
						స్థితి FmtPart::ఎడమ_2:
							str += ఉపసర్గ_2;
							str += buf;
							str += std::string(pad_వెడల్పు, part.padding);
							ఆపు;
						స్థితి FmtPart::కుడి_2:
							str += std::string(pad_వెడల్పు, part.padding);
							str += ఉపసర్గ_2;
							str += buf;
							ఆపు;
						స్థితి FmtPart::NUMERIC:
							str += ఉపసర్గ_2;
							str += std::string(pad_వెడల్పు, part.padding);
							str += buf;
							ఆపు;
					}
				} లేకపోతే {
					str += ఉపసర్గ_2;
					str += buf;
				}
				ఆపు;
			}
		}
	}

	తిరిగి str;
}
