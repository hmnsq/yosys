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

#अन्तर्भाव "libs/bigint/BigUnsigned.hh"
#अन्तर्भाव "kernel/fmt.h"

USING_YOSYS_NAMESPACE

शून्य Fmt::append_literal(ध्रुव std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.प्रक्षिप_पृष्ठभागः(part);
}

शून्य Fmt::parse_rtlil(ध्रुव RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec तर्काः_2 = cell->get_पत्तनम्(ID(तर्काः));
	parts.clear();

	FmtPart part;
	प्रति (आकार_t i = 0; i < fmt.आकार(); i++) {
		यदि (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} अन्यथा यदि (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} अन्यथा यदि (fmt[i] == '}') {
			log_assert(असत्य && "Unexpected '}' in format string");
		} अन्यथा यदि (fmt[i] == '{') {
			यदि (!part.str.रिक्तानि()) {
				part.type = FmtPart::LITERAL;
				parts.प्रक्षिप_पृष्ठभागः(part);
				part = {};
			}

			यदि (++i == fmt.आकार())
				log_assert(असत्य && "Unexpected end in format substitution");

			आकार_t arg_आकार = 0;
			प्रति (; i < fmt.आकार(); i++) {
				यदि (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_आकार *= 10;
					arg_आकार += fmt[i] - '0';
				} अन्यथा यदि (fmt[i] == ':') {
					++i;
					भङ्ग;
				} अन्यथा {
					log_assert(असत्य && "Unexpected character in format substitution");
				}
			}
			यदि (i == fmt.आकार())
				log_assert(असत्य && "Unexpected end in format substitution");

			यदि ((आकार_t)तर्काः_2.आकार() < arg_आकार)
				log_assert(असत्य && "Format part overruns arguments");
			part.sig = तर्काः_2.extract(0, arg_आकार);
			तर्काः_2.remove(0, arg_आकार);

			यदि (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				गच्छ success;
			}

			यदि (fmt[i] == '>')
				part.justify = FmtPart::दक्षिणः;
			अन्यथा यदि (fmt[i] == '<')
				part.justify = FmtPart::वामः;
			अन्यथा यदि (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			अन्यथा
				log_assert(असत्य && "Unexpected justification in format substitution");
			यदि (++i == fmt.आकार())
				log_assert(असत्य && "Unexpected end in format substitution");

			part.padding = fmt[i];
			यदि (++i == fmt.आकार())
				log_assert(असत्य && "Unexpected end in format substitution");

			प्रति (; i < fmt.आकार(); i++) {
				यदि (fmt[i] >= '0' && fmt[i] <= '9') {
					part.विस्तारः *= 10;
					part.विस्तारः += fmt[i] - '0';
					अनुवर्त;
				} अन्यथा यदि (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.आधारः = 2;
				} अन्यथा यदि (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.आधारः = 8;
				} अन्यथा यदि (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.आधारः = 10;
				} अन्यथा यदि (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.आधारः = 16;
				} अन्यथा यदि (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.आधारः = 16;
					part.hex_ऊर्ध्वस्थम् = सत्य;
				} अन्यथा यदि (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} अन्यथा यदि (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} अन्यथा यदि (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = सत्य;
				} अन्यथा {
					log_assert(असत्य && "Unexpected character in format substitution");
				}
				++i;
				भङ्ग;
			}
			यदि (i == fmt.आकार())
				log_assert(असत्य && "Unexpected end in format substitution");

			यदि (part.type == FmtPart::INTEGER) {
				यदि (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					यदि (++i == fmt.आकार())
						log_assert(असत्य && "Unexpected end in format substitution");
				} अन्यथा यदि (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					यदि (++i == fmt.आकार())
						log_assert(असत्य && "Unexpected end in format substitution");
				} अन्यथा यदि (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					यदि (++i == fmt.आकार())
						log_assert(असत्य && "Unexpected end in format substitution");
				} अन्यथा {
					// also accept no sign character and treat like MINUS for compatibility
				}

				यदि (fmt[i] == '#') {
					part.show_आधारः = सत्य;
					++i;
				}
				यदि (fmt[i] == '_') {
					part.गणः = सत्य;
					++i;
				}

				यदि (fmt[i] == 'u')
					part.signed_ = असत्य;
				अन्यथा यदि (fmt[i] == 's')
					part.signed_ = सत्य;
				अन्यथा
					log_assert(असत्य && "Unexpected character in format substitution");
				यदि (++i == fmt.आकार())
					log_assert(असत्य && "Unexpected end in format substitution");
			}

		success:
			यदि (fmt[i] != '}')
				log_assert(असत्य && "Expected '}' after format substitution");

			parts.प्रक्षिप_पृष्ठभागः(part);
			part = {};
		} अन्यथा {
			part.str += fmt[i];
		}
	}
	यदि (!part.str.रिक्तानि()) {
		part.type = FmtPart::LITERAL;
		parts.प्रक्षिप_पृष्ठभागः(part);
	}
}

शून्य Fmt::emit_rtlil(RTLIL::Cell *cell) ध्रुव {
	std::string fmt;
	RTLIL::SigSpec तर्काः_2;

	प्रति (स्वचालित &part : parts) {
		विकल्प (part.type) {
			पक्ष FmtPart::LITERAL:
				प्रति (वर्ण c : part.str) {
					यदि (c == '{')
						fmt += "{{";
					अन्यथा यदि (c == '}')
						fmt += "}}";
					अन्यथा
						fmt += c;
				}
				भङ्ग;

			पक्ष FmtPart::UNICHAR:
				log_assert(part.sig.आकार() <= 32);
				fmt += "{U}";
				भङ्ग;

			पक्ष FmtPart::VLOG_TIME:
				log_assert(part.sig.आकार() == 0);
				YS_FALLTHROUGH
			पक्ष FmtPart::STRING:
				log_assert(part.sig.आकार() % 8 == 0);
				YS_FALLTHROUGH
			पक्ष FmtPart::INTEGER:
				तर्काः_2.append(part.sig);
				fmt += '{';
				fmt += std::गन्तव्यम्_string(part.sig.आकार());
				fmt += ':';
				यदि (part.justify == FmtPart::दक्षिणः)
					fmt += '>';
				अन्यथा यदि (part.justify == FmtPart::वामः)
					fmt += '<';
				अन्यथा यदि (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				अन्यथा log_abort();
				log_assert(part.विस्तारः == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				यदि (part.विस्तारः > 0)
					fmt += std::गन्तव्यम्_string(part.विस्तारः);
				यदि (part.type == FmtPart::INTEGER) {
					विकल्प (part.आधारः) {
						पक्ष  2: fmt += 'b'; भङ्ग;
						पक्ष  8: fmt += 'o'; भङ्ग;
						पक्ष 10: fmt += 'd'; भङ्ग;
						पक्ष 16: fmt += part.hex_ऊर्ध्वस्थम् ? 'H' : 'h'; भङ्ग;
						स्वभाव: log_abort();
					}
					विकल्प (part.sign) {
						पक्ष FmtPart::MINUS:       fmt += '-'; भङ्ग;
						पक्ष FmtPart::PLUS_MINUS:  fmt += '+'; भङ्ग;
						पक्ष FmtPart::SPACE_MINUS: fmt += ' '; भङ्ग;
					}
					fmt += part.show_आधारः ? "#" : "";
					fmt += part.गणः ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} अन्यथा यदि (part.type == FmtPart::STRING) {
					fmt += 'c';
				} अन्यथा यदि (part.type == FmtPart::VLOG_TIME) {
					यदि (part.realtime)
						fmt += 'r';
					अन्यथा
						fmt += 't';
				} अन्यथा log_abort();
				fmt += '}';
				भङ्ग;

			स्वभाव: log_abort();
		}
	}

	cell->समुच्चयः_Param(ID(FORMAT), fmt);
	cell->समुच्चयः_Param(ID(तर्काः_विस्तारः), तर्काः_2.आकार());
	cell->समुच्चयः_पत्तनम्(ID(तर्काः), तर्काः_2);
}

स्थिर आकार_t compute_required_decimal_places(आकार_t आकार, तार्किक signed_)
{
	BigUnsigned max;
	यदि (!signed_)
		max.समुच्चयः_Bit(आकार, सत्य);
	अन्यथा
		max.समुच्चयः_Bit(आकार - 1, सत्य);
	आकार_t places = 0;
	यावत् (!max.isZero()) {
		places++;
		max /= 10;
	}
	यदि (signed_)
		places++;
	प्रत्यागम places;
}

स्थिर आकार_t compute_required_nondecimal_places(आकार_t आकार, अचिह्नित आधारः)
{
	log_assert(आधारः != 10);
	BigUnsigned max;
	max.समुच्चयः_Bit(आकार - 1, सत्य);
	आकार_t places = 0;
	यावत् (!max.isZero()) {
		places++;
		max /= आधारः;
	}
	प्रत्यागम places;
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
शून्य Fmt::apply_verilog_automatic_sizing_and_सङ्कलय(FmtPart &part)
{
	यदि (part.आधारः == 10) {
		आकार_t places = compute_required_decimal_places(part.sig.आकार(), part.signed_);
		part.padding = ' ';
		part.विस्तारः = std::max(part.विस्तारः, places);
		parts.प्रक्षिप_पृष्ठभागः(part);
		प्रत्यागम;
	}

	part.padding = '0';

	आकार_t places = compute_required_nondecimal_places(part.sig.आकार(), part.आधारः);
	यदि (part.विस्तारः < places) {
		part.justify = FmtPart::दक्षिणः;
		part.विस्तारः = places;
		parts.प्रक्षिप_पृष्ठभागः(part);
	} अन्यथा यदि (part.विस्तारः == places) {
		parts.प्रक्षिप_पृष्ठभागः(part);
	} अन्यथा यदि (part.विस्तारः > places) {
		स्वचालित gap = std::string(part.विस्तारः - places, ' ');
		part.विस्तारः = places;

		यदि (part.justify == FmtPart::दक्षिणः) {
			append_literal(gap);
			parts.प्रक्षिप_पृष्ठभागः(part);
		} अन्यथा {
			part.justify = FmtPart::दक्षिणः;
			parts.प्रक्षिप_पृष्ठभागः(part);
			append_literal(gap);
		}
	}
}

शून्य Fmt::parse_verilog(ध्रुव std::सदिशम्<VerilogFmtArg> &तर्काः_2, तार्किक sformat_like, पूर्णाङ्क default_आधारः, ध्रुव std::string &task_name, ध्रुव std::string &module_name)
{
	parts.clear();

	स्वचालित arg = तर्काः_2.begin();
	प्रति (; arg != तर्काः_2.अन्तः(); ++arg) {
		विकल्प (arg->type) {
			पक्ष VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.आधारः = default_आधारः;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_सङ्कलय(part);
				भङ्ग;
			}

			पक्ष VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.विस्तारः = 20;
				parts.प्रक्षिप_पृष्ठभागः(part);
				भङ्ग;
			}

			पक्ष VerilogFmtArg::STRING: {
				यदि (arg == तर्काः_2.begin() || !sformat_like) {
					ध्रुव स्वचालित fmtarg = arg;
					ध्रुव std::string &fmt = fmtarg->str;
					FmtPart part = {};
					प्रति (आकार_t i = 0; i < fmt.आकार(); i++) {
						यदि (fmt[i] != '%') {
							part.str += fmt[i];
						} अन्यथा यदि (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} अन्यथा यदि (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} अन्यथा यदि (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} अन्यथा {
							यदि (!part.str.रिक्तानि()) {
								part.type = FmtPart::LITERAL;
								parts.प्रक्षिप_पृष्ठभागः(part);
								part = {};
							}
							यदि (++i == fmt.आकार()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - तर्काः_2.begin() + 1);
							}

							यदि (++arg == तर्काः_2.अन्तः()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - तर्काः_2.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							प्रति (; i < fmt.आकार(); i++) {
								यदि (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::वामः;
								} अन्यथा यदि (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} अन्यथा भङ्ग;
							}
							यदि (i == fmt.आकार()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - तर्काः_2.begin() + 1);
							}

							तार्किक has_leading_zero = असत्य, has_विस्तारः = असत्य;
							प्रति (; i < fmt.आकार(); i++) {
								यदि (fmt[i] >= '0' && fmt[i] <= '9') {
									यदि (fmt[i] == '0' && !has_विस्तारः) {
										has_leading_zero = सत्य;
									} अन्यथा {
										has_विस्तारः = सत्य;
										part.विस्तारः *= 10;
										part.विस्तारः += fmt[i] - '0';
									}
									अनुवर्त;
								} अन्यथा यदि (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.आधारः =  2;
								} अन्यथा यदि (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.आधारः =  8;
								} अन्यथा यदि (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.आधारः = 10;
								} अन्यथा यदि (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.आधारः = 16;
								} अन्यथा यदि (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} अन्यथा यदि (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									यदि ((part.sig.आकार() % 8) != 0)
										part.sig.extend_u0((part.sig.आकार() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} अन्यथा यदि (fmt[i] == 't' || fmt[i] == 'T') {
									यदि (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										यदि (!has_विस्तारः && !has_leading_zero)
											part.विस्तारः = 20;
									} अन्यथा {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - तर्काः_2.begin() + 1);
									}
								} अन्यथा {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - तर्काः_2.begin() + 1);
								}
								भङ्ग;
							}
							यदि (i == fmt.आकार()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - तर्काः_2.begin() + 1);
							}

							यदि (part.padding == '\0') {
								यदि (has_leading_zero && part.justify == FmtPart::दक्षिणः) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} अन्यथा {
									part.padding = ' ';
								}
							}

							यदि (part.type == FmtPart::INTEGER && part.आधारः != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - तर्काः_2.begin() + 1);

							यदि (part.आधारः != 10)
								part.signed_ = असत्य;
							यदि (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_सङ्कलय(part);
							अन्यथा
								parts.प्रक्षिप_पृष्ठभागः(part);
							part = {};
						}
					}
					यदि (!part.str.रिक्तानि()) {
						part.type = FmtPart::LITERAL;
						parts.प्रक्षिप_पृष्ठभागः(part);
					}
				} अन्यथा {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.प्रक्षिप_पृष्ठभागः(part);
				}
				भङ्ग;
			}

			स्वभाव: log_abort();
		}
	}
}

std::सदिशम्<VerilogFmtArg> Fmt::emit_verilog() ध्रुव
{
	std::सदिशम्<VerilogFmtArg> तर्काः_2;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	प्रति (स्वचालित &part : parts) {
		विकल्प (part.type) {
			पक्ष FmtPart::LITERAL:
				प्रति (वर्ण c : part.str) {
					यदि (c == '%')
						fmt.str += "%%";
					अन्यथा
						fmt.str += c;
				}
				भङ्ग;

			पक्ष FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				तर्काः_2.प्रक्षिप_पृष्ठभागः(arg);

				fmt.str += '%';
				यदि (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				यदि (part.justify == FmtPart::वामः)
					fmt.str += '-';
				यदि (part.विस्तारः == 0) {
					fmt.str += '0';
				} अन्यथा यदि (part.विस्तारः > 0) {
					यदि (part.आधारः != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::गन्तव्यम्_string(part.विस्तारः);
				}
				विकल्प (part.आधारः) {
					पक्ष  2: fmt.str += 'b'; भङ्ग;
					पक्ष  8: fmt.str += 'o'; भङ्ग;
					पक्ष 10: fmt.str += 'd'; भङ्ग;
					पक्ष 16: fmt.str += 'h'; भङ्ग; // treat uppercase hex as lowercase
					स्वभाव: log_abort();
				}
				भङ्ग;
			}

			पक्ष FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				तर्काः_2.प्रक्षिप_पृष्ठभागः(arg);

				fmt.str += '%';
				यदि (part.justify == FmtPart::वामः)
					fmt.str += '-';
				यदि (part.sig.आकार() == 8) {
					यदि (part.विस्तारः > 0) {
						यदि (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::गन्तव्यम्_string(part.विस्तारः);
					}
					fmt.str += 'c';
				} अन्यथा {
					log_assert(part.sig.आकार() % 8 == 0);
					यदि (part.विस्तारः > 0) {
						fmt.str += std::गन्तव्यम्_string(part.विस्तारः);
					}
					fmt.str += 's';
				}
				भङ्ग;
			}

			पक्ष FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				तर्काः_2.प्रक्षिप_पृष्ठभागः(arg);

				fmt.str += "%c";
				भङ्ग;
			}

			पक्ष FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				यदि (part.realtime)
					arg.realtime = सत्य;
				तर्काः_2.प्रक्षिप_पृष्ठभागः(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				यदि (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				यदि (part.justify == FmtPart::वामः)
					fmt.str += '-';
				यदि (part.padding == '0' && part.विस्तारः > 0)
					fmt.str += '0';
				fmt.str += std::गन्तव्यम्_string(part.विस्तारः);
				fmt.str += 't';
				भङ्ग;
			}

			स्वभाव: log_abort();
		}
	}

	तर्काः_2.निवेशय(तर्काः_2.begin(), fmt);
	प्रत्यागम तर्काः_2;
}

std::string escape_cxx_string(ध्रुव std::string &निवेशः)
{
	std::string output = "\"";
	प्रति (स्वचालित c : निवेशः) {
		यदि (::isprint(c)) {
			यदि (c == '\\' || c == '"')
				output.प्रक्षिप_पृष्ठभागः('\\');
			output.प्रक्षिप_पृष्ठभागः(c);
		} अन्यथा {
			वर्ण l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.प्रक्षिप_पृष्ठभागः('\\');
			output.प्रक्षिप_पृष्ठभागः('0' + h);
			output.प्रक्षिप_पृष्ठभागः('0' + m);
			output.प्रक्षिप_पृष्ठभागः('0' + l);
		}
	}
	output.प्रक्षिप_पृष्ठभागः('"');
	यदि (output.find('\0') != std::string::npos) {
		output.निवेशय(0, "std::string {");
		output.append(stringf(", %zu}", निवेशः.आकार()));
	}
	प्रत्यागम output;
}

शून्य Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<शून्य(ध्रुव RTLIL::SigSpec &)> emit_sig, ध्रुव std::string &context) ध्रुव
{
	os << indent << "std::string buf;\n";
	प्रति (स्वचालित &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		विकल्प (part.type) {
			पक्ष FmtPart::LITERAL:   os << "LITERAL";   भङ्ग;
			पक्ष FmtPart::INTEGER:   os << "INTEGER";   भङ्ग;
			पक्ष FmtPart::STRING:    os << "STRING";    भङ्ग;
			पक्ष FmtPart::UNICHAR:   os << "UNICHAR";   भङ्ग;
			पक्ष FmtPart::VLOG_TIME: os << "VLOG_TIME"; भङ्ग;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		विकल्प (part.justify) {
			पक्ष FmtPart::वामः:    os << "LEFT";    भङ्ग;
			पक्ष FmtPart::दक्षिणः:   os << "RIGHT";   भङ्ग;
			पक्ष FmtPart::NUMERIC: os << "NUMERIC"; भङ्ग;
		}
		os << ", ";
		os << "(char)" << (पूर्णाङ्क)part.padding << ", ";
		os << part.विस्तारः << ", ";
		os << part.आधारः << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		विकल्प (part.sign) {
			पक्ष FmtPart::MINUS:       os << "MINUS";       भङ्ग;
			पक्ष FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  भङ्ग;
			पक्ष FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; भङ्ग;
		}
		os << ", ";
		os << part.hex_ऊर्ध्वस्थम् << ", ";
		os << part.show_आधारः << ", ";
		os << part.गणः << ", ";
		os << part.realtime;
		os << " }.render(";
		emit_sig(part.sig);
		os << ", " << context << ");\n";
	}
	os << indent << "return buf;\n";
}

std::string Fmt::render() ध्रुव
{
	std::string str;

	प्रति (स्वचालित &part : parts) {
		विकल्प (part.type) {
			पक्ष FmtPart::LITERAL:
				str += part.str;
				भङ्ग;

			पक्ष FmtPart::UNICHAR: {
				RTLIL::Const मानम् = part.sig.as_const();
				uint32_t codepoint = मानम्.as_int();
				यदि (codepoint >= 0x10000)
					str += (वर्ण)(0xf0 |  (codepoint >> 18));
				अन्यथा यदि (codepoint >= 0x800)
					str += (वर्ण)(0xe0 |  (codepoint >> 12));
				अन्यथा यदि (codepoint >= 0x80)
					str += (वर्ण)(0xc0 |  (codepoint >>  6));
				अन्यथा
					str += (वर्ण)codepoint;
				यदि (codepoint >= 0x10000)
					str += (वर्ण)(0x80 | ((codepoint >> 12) & 0x3f));
				यदि (codepoint >= 0x800)
					str += (वर्ण)(0x80 | ((codepoint >>  6) & 0x3f));
				यदि (codepoint >= 0x80)
					str += (वर्ण)(0x80 | ((codepoint >>  0) & 0x3f));
				भङ्ग;
			}

			पक्ष FmtPart::INTEGER:
			पक्ष FmtPart::STRING:
			पक्ष FmtPart::VLOG_TIME: {
				std::string buf;
				std::string उपसर्गः_2;
				यदि (part.type == FmtPart::INTEGER) {
					RTLIL::Const मानम् = part.sig.as_const();
					तार्किक has_x = असत्य, all_x = सत्य, has_z = असत्य, all_z = सत्य;
					प्रति (State bit : मानम्) {
						यदि (bit == State::Sx)
							has_x = सत्य;
						अन्यथा
							all_x = असत्य;
						यदि (bit == State::Sz)
							has_z = सत्य;
						अन्यथा
							all_z = असत्य;
					}

					यदि (!has_z && !has_x && part.signed_ && मानम्[मानम्.आकार() - 1]) {
						उपसर्गः_2 = "-";
						मानम् = RTLIL::const_neg(मानम्, {}, part.signed_, {}, मानम्.आकार() + 1);
					} अन्यथा {
						विकल्प (part.sign) {
							पक्ष FmtPart::MINUS:       भङ्ग;
							पक्ष FmtPart::PLUS_MINUS:  उपसर्गः_2 = "+"; भङ्ग;
							पक्ष FmtPart::SPACE_MINUS: उपसर्गः_2 = " "; भङ्ग;
						}
					}

					यदि (part.आधारः != 10) {
						आकार_t minimum_आकार = 1;
						प्रति (आकार_t सूचकांक = 0; सूचकांक < (आकार_t)मानम्.आकार(); सूचकांक++)
							यदि (मानम्[सूचकांक] != State::S0)
								minimum_आकार = सूचकांक + 1;
						मानम् = मानम्.extract(0, minimum_आकार);
					}

					यदि (part.आधारः == 2) {
						यदि (part.show_आधारः)
							उपसर्गः_2 += "0b";
						प्रति (आकार_t सूचकांक = 0; सूचकांक < (आकार_t)मानम्.आकार(); सूचकांक++) {
							यदि (part.गणः && सूचकांक > 0 && सूचकांक % 4 == 0)
								buf += '_';
							RTLIL::State bit = मानम्[सूचकांक];
							यदि (bit == State::Sx)
								buf += 'x';
							अन्यथा यदि (bit == State::Sz)
								buf += 'z';
							अन्यथा यदि (bit == State::S1)
								buf += '1';
							अन्यथा /* if (bit == State::S0) */
								buf += '0';
						}
					} अन्यथा यदि (part.आधारः == 8 || part.आधारः == 16) {
						यदि (part.show_आधारः)
							उपसर्गः_2 += (part.आधारः == 16) ? (part.hex_ऊर्ध्वस्थम् ? "0X" : "0x") : "0o";
						आकार_t चरणः = (part.आधारः == 16) ? 4 : 3;
						प्रति (आकार_t सूचकांक = 0; सूचकांक < (आकार_t)मानम्.आकार(); सूचकांक += चरणः) {
							यदि (part.गणः && सूचकांक > 0 && सूचकांक % (4 * चरणः) == 0)
								buf += '_';
							RTLIL::Const subvalue = मानम्.extract(सूचकांक, min(चरणः, मानम्.आकार() - सूचकांक));
							तार्किक has_x = असत्य, all_x = सत्य, has_z = असत्य, all_z = सत्य;
							प्रति (State bit : subvalue) {
								यदि (bit == State::Sx)
									has_x = सत्य;
								अन्यथा
									all_x = असत्य;
								यदि (bit == State::Sz)
									has_z = सत्य;
								अन्यथा
									all_z = असत्य;
							}
							यदि (all_x)
								buf += 'x';
							अन्यथा यदि (all_z)
								buf += 'z';
							अन्यथा यदि (has_x)
								buf += 'X';
							अन्यथा यदि (has_z)
								buf += 'Z';
							अन्यथा {
								ध्रुव वर्ण *digits = part.hex_ऊर्ध्वस्थम् ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} अन्यथा यदि (part.आधारः == 10) {
						यदि (part.show_आधारः)
							उपसर्गः_2 += "0d";
						यदि (all_x)
							buf += 'x';
						अन्यथा यदि (all_z)
							buf += 'z';
						अन्यथा यदि (has_x)
							buf += 'X';
						अन्यथा यदि (has_z)
							buf += 'Z';
						अन्यथा {
							log_assert(मानम्.is_fully_def());
							यदि (मानम्.is_fully_zero())
								buf += '0';
							आकार_t सूचकांक = 0;
							यावत् (!मानम्.is_fully_zero())	{
								यदि (part.गणः && सूचकांक > 0 && सूचकांक % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(मानम्, 10, असत्य, असत्य, 4).as_int();
								मानम् = RTLIL::const_div(मानम्, 10, असत्य, असत्य, मानम्.आकार());
								सूचकांक++;
							}
						}
					} अन्यथा log_abort();
					यदि (part.justify == FmtPart::NUMERIC && part.गणः && part.padding == '0') {
						आकार_t गणः_आकार = part.आधारः == 10 ? 3 : 4;
						यावत् (उपसर्गः_2.आकार() + buf.आकार() < part.विस्तारः) {
							यदि (buf.आकार() % (गणः_आकार + 1) == गणः_आकार)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.अन्तः());
				} अन्यथा यदि (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} अन्यथा यदि (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.विस्तारः == 0 || part.padding != '\0');
				यदि (उपसर्गः_2.आकार() + buf.आकार() < part.विस्तारः) {
					आकार_t pad_विस्तारः = part.विस्तारः - उपसर्गः_2.आकार() - buf.आकार();
					विकल्प (part.justify) {
						पक्ष FmtPart::वामः:
							str += उपसर्गः_2;
							str += buf;
							str += std::string(pad_विस्तारः, part.padding);
							भङ्ग;
						पक्ष FmtPart::दक्षिणः:
							str += std::string(pad_विस्तारः, part.padding);
							str += उपसर्गः_2;
							str += buf;
							भङ्ग;
						पक्ष FmtPart::NUMERIC:
							str += उपसर्गः_2;
							str += std::string(pad_विस्तारः, part.padding);
							str += buf;
							भङ्ग;
					}
				} अन्यथा {
					str += उपसर्गः_2;
					str += buf;
				}
				भङ्ग;
			}
		}
	}

	प्रत्यागम str;
}
