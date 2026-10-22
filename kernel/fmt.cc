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

#समावेश "libs/bigint/BigUnsigned.hh"
#समावेश "kernel/fmt.h"

USING_YOSYS_NAMESPACE

व्योम Fmt::append_literal(स्थिर std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.धकेलो_back(part);
}

व्योम Fmt::parse_rtlil(स्थिर RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.सफा();

	FmtPart part;
	क्रम (माप_प्रकार i = 0; i < fmt.आकार(); i++) {
		अगर (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} अन्यथा अगर (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} अन्यथा अगर (fmt[i] == '}') {
			log_assert(false && "Unexpected '}' in format string");
		} अन्यथा अगर (fmt[i] == '{') {
			अगर (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.धकेलो_back(part);
				part = {};
			}

			अगर (++i == fmt.आकार())
				log_assert(false && "Unexpected end in format substitution");

			माप_प्रकार arg_आकार = 0;
			क्रम (; i < fmt.आकार(); i++) {
				अगर (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_आकार *= 10;
					arg_आकार += fmt[i] - '0';
				} अन्यथा अगर (fmt[i] == ':') {
					++i;
					अवरोध;
				} अन्यथा {
					log_assert(false && "Unexpected character in format substitution");
				}
			}
			अगर (i == fmt.आकार())
				log_assert(false && "Unexpected end in format substitution");

			अगर ((माप_प्रकार)args.आकार() < arg_आकार)
				log_assert(false && "Format part overruns arguments");
			part.sig = args.extract(0, arg_आकार);
			args.हटाओ(0, arg_आकार);

			अगर (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				जाओ success;
			}

			अगर (fmt[i] == '>')
				part.justify = FmtPart::दायाँ;
			अन्यथा अगर (fmt[i] == '<')
				part.justify = FmtPart::बायाँ;
			अन्यथा अगर (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			अन्यथा
				log_assert(false && "Unexpected justification in format substitution");
			अगर (++i == fmt.आकार())
				log_assert(false && "Unexpected end in format substitution");

			part.padding = fmt[i];
			अगर (++i == fmt.आकार())
				log_assert(false && "Unexpected end in format substitution");

			क्रम (; i < fmt.आकार(); i++) {
				अगर (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					जारी;
				} अन्यथा अगर (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} अन्यथा अगर (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} अन्यथा अगर (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} अन्यथा अगर (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} अन्यथा अगर (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = true;
				} अन्यथा अगर (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} अन्यथा अगर (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} अन्यथा अगर (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = true;
				} अन्यथा {
					log_assert(false && "Unexpected character in format substitution");
				}
				++i;
				अवरोध;
			}
			अगर (i == fmt.आकार())
				log_assert(false && "Unexpected end in format substitution");

			अगर (part.type == FmtPart::INTEGER) {
				अगर (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					अगर (++i == fmt.आकार())
						log_assert(false && "Unexpected end in format substitution");
				} अन्यथा अगर (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					अगर (++i == fmt.आकार())
						log_assert(false && "Unexpected end in format substitution");
				} अन्यथा अगर (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					अगर (++i == fmt.आकार())
						log_assert(false && "Unexpected end in format substitution");
				} अन्यथा {
					// also accept no sign character and treat like MINUS for compatibility
				}

				अगर (fmt[i] == '#') {
					part.show_base = true;
					++i;
				}
				अगर (fmt[i] == '_') {
					part.group = true;
					++i;
				}

				अगर (fmt[i] == 'u')
					part.signed_ = false;
				अन्यथा अगर (fmt[i] == 's')
					part.signed_ = true;
				अन्यथा
					log_assert(false && "Unexpected character in format substitution");
				अगर (++i == fmt.आकार())
					log_assert(false && "Unexpected end in format substitution");
			}

		success:
			अगर (fmt[i] != '}')
				log_assert(false && "Expected '}' after format substitution");

			parts.धकेलो_back(part);
			part = {};
		} अन्यथा {
			part.str += fmt[i];
		}
	}
	अगर (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.धकेलो_back(part);
	}
}

व्योम Fmt::emit_rtlil(RTLIL::Cell *cell) स्थिर {
	std::string fmt;
	RTLIL::SigSpec args;

	क्रम (स्वतः &part : parts) {
		चयन (part.type) {
			हाल FmtPart::LITERAL:
				क्रम (अक्षर c : part.str) {
					अगर (c == '{')
						fmt += "{{";
					अन्यथा अगर (c == '}')
						fmt += "}}";
					अन्यथा
						fmt += c;
				}
				अवरोध;

			हाल FmtPart::UNICHAR:
				log_assert(part.sig.आकार() <= 32);
				fmt += "{U}";
				अवरोध;

			हाल FmtPart::VLOG_TIME:
				log_assert(part.sig.आकार() == 0);
				YS_FALLTHROUGH
			हाल FmtPart::STRING:
				log_assert(part.sig.आकार() % 8 == 0);
				YS_FALLTHROUGH
			हाल FmtPart::INTEGER:
				args.append(part.sig);
				fmt += '{';
				fmt += std::to_string(part.sig.आकार());
				fmt += ':';
				अगर (part.justify == FmtPart::दायाँ)
					fmt += '>';
				अन्यथा अगर (part.justify == FmtPart::बायाँ)
					fmt += '<';
				अन्यथा अगर (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				अन्यथा log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				अगर (part.width > 0)
					fmt += std::to_string(part.width);
				अगर (part.type == FmtPart::INTEGER) {
					चयन (part.base) {
						हाल  2: fmt += 'b'; अवरोध;
						हाल  8: fmt += 'o'; अवरोध;
						हाल 10: fmt += 'd'; अवरोध;
						हाल 16: fmt += part.hex_upper ? 'H' : 'h'; अवरोध;
						शेष: log_abort();
					}
					चयन (part.sign) {
						हाल FmtPart::MINUS:       fmt += '-'; अवरोध;
						हाल FmtPart::PLUS_MINUS:  fmt += '+'; अवरोध;
						हाल FmtPart::SPACE_MINUS: fmt += ' '; अवरोध;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} अन्यथा अगर (part.type == FmtPart::STRING) {
					fmt += 'c';
				} अन्यथा अगर (part.type == FmtPart::VLOG_TIME) {
					अगर (part.realtime)
						fmt += 'r';
					अन्यथा
						fmt += 't';
				} अन्यथा log_abort();
				fmt += '}';
				अवरोध;

			शेष: log_abort();
		}
	}

	cell->setParam(ID(FORMAT), fmt);
	cell->setParam(ID(ARGS_WIDTH), args.आकार());
	cell->setPort(ID(ARGS), args);
}

अटल माप_प्रकार compute_required_decimal_places(माप_प्रकार आकार, bool signed_)
{
	BigUnsigned max;
	अगर (!signed_)
		max.setBit(आकार, true);
	अन्यथा
		max.setBit(आकार - 1, true);
	माप_प्रकार places = 0;
	जबतक (!max.isZero()) {
		places++;
		max /= 10;
	}
	अगर (signed_)
		places++;
	वापस places;
}

अटल माप_प्रकार compute_required_nondecimal_places(माप_प्रकार आकार, अचिन्हित base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(आकार - 1, true);
	माप_प्रकार places = 0;
	जबतक (!max.isZero()) {
		places++;
		max /= base;
	}
	वापस places;
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
व्योम Fmt::apply_verilog_automatic_sizing_and_add(FmtPart &part)
{
	अगर (part.base == 10) {
		माप_प्रकार places = compute_required_decimal_places(part.sig.आकार(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.धकेलो_back(part);
		वापस;
	}

	part.padding = '0';

	माप_प्रकार places = compute_required_nondecimal_places(part.sig.आकार(), part.base);
	अगर (part.width < places) {
		part.justify = FmtPart::दायाँ;
		part.width = places;
		parts.धकेलो_back(part);
	} अन्यथा अगर (part.width == places) {
		parts.धकेलो_back(part);
	} अन्यथा अगर (part.width > places) {
		स्वतः gap = std::string(part.width - places, ' ');
		part.width = places;

		अगर (part.justify == FmtPart::दायाँ) {
			append_literal(gap);
			parts.धकेलो_back(part);
		} अन्यथा {
			part.justify = FmtPart::दायाँ;
			parts.धकेलो_back(part);
			append_literal(gap);
		}
	}
}

व्योम Fmt::parse_verilog(स्थिर std::vector<VerilogFmtArg> &args, bool sformat_like, पूर्णांक default_base, स्थिर std::string &task_name, स्थिर std::string &module_name)
{
	parts.सफा();

	स्वतः arg = args.begin();
	क्रम (; arg != args.end(); ++arg) {
		चयन (arg->type) {
			हाल VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				अवरोध;
			}

			हाल VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.धकेलो_back(part);
				अवरोध;
			}

			हाल VerilogFmtArg::STRING: {
				अगर (arg == args.begin() || !sformat_like) {
					स्थिर स्वतः fmtarg = arg;
					स्थिर std::string &fmt = fmtarg->str;
					FmtPart part = {};
					क्रम (माप_प्रकार i = 0; i < fmt.आकार(); i++) {
						अगर (fmt[i] != '%') {
							part.str += fmt[i];
						} अन्यथा अगर (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} अन्यथा अगर (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} अन्यथा अगर (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} अन्यथा {
							अगर (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.धकेलो_back(part);
								part = {};
							}
							अगर (++i == fmt.आकार()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							अगर (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							क्रम (; i < fmt.आकार(); i++) {
								अगर (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::बायाँ;
								} अन्यथा अगर (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} अन्यथा अवरोध;
							}
							अगर (i == fmt.आकार()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							bool has_leading_zero = false, has_width = false;
							क्रम (; i < fmt.आकार(); i++) {
								अगर (fmt[i] >= '0' && fmt[i] <= '9') {
									अगर (fmt[i] == '0' && !has_width) {
										has_leading_zero = true;
									} अन्यथा {
										has_width = true;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									जारी;
								} अन्यथा अगर (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} अन्यथा अगर (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} अन्यथा अगर (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} अन्यथा अगर (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} अन्यथा अगर (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} अन्यथा अगर (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									अगर ((part.sig.आकार() % 8) != 0)
										part.sig.extend_u0((part.sig.आकार() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} अन्यथा अगर (fmt[i] == 't' || fmt[i] == 'T') {
									अगर (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										अगर (!has_width && !has_leading_zero)
											part.width = 20;
									} अन्यथा {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} अन्यथा {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								अवरोध;
							}
							अगर (i == fmt.आकार()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							अगर (part.padding == '\0') {
								अगर (has_leading_zero && part.justify == FmtPart::दायाँ) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} अन्यथा {
									part.padding = ' ';
								}
							}

							अगर (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							अगर (part.base != 10)
								part.signed_ = false;
							अगर (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							अन्यथा
								parts.धकेलो_back(part);
							part = {};
						}
					}
					अगर (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.धकेलो_back(part);
					}
				} अन्यथा {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.धकेलो_back(part);
				}
				अवरोध;
			}

			शेष: log_abort();
		}
	}
}

std::vector<VerilogFmtArg> Fmt::emit_verilog() स्थिर
{
	std::vector<VerilogFmtArg> args;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	क्रम (स्वतः &part : parts) {
		चयन (part.type) {
			हाल FmtPart::LITERAL:
				क्रम (अक्षर c : part.str) {
					अगर (c == '%')
						fmt.str += "%%";
					अन्यथा
						fmt.str += c;
				}
				अवरोध;

			हाल FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.धकेलो_back(arg);

				fmt.str += '%';
				अगर (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				अगर (part.justify == FmtPart::बायाँ)
					fmt.str += '-';
				अगर (part.width == 0) {
					fmt.str += '0';
				} अन्यथा अगर (part.width > 0) {
					अगर (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				चयन (part.base) {
					हाल  2: fmt.str += 'b'; अवरोध;
					हाल  8: fmt.str += 'o'; अवरोध;
					हाल 10: fmt.str += 'd'; अवरोध;
					हाल 16: fmt.str += 'h'; अवरोध; // treat uppercase hex as lowercase
					शेष: log_abort();
				}
				अवरोध;
			}

			हाल FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				args.धकेलो_back(arg);

				fmt.str += '%';
				अगर (part.justify == FmtPart::बायाँ)
					fmt.str += '-';
				अगर (part.sig.आकार() == 8) {
					अगर (part.width > 0) {
						अगर (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} अन्यथा {
					log_assert(part.sig.आकार() % 8 == 0);
					अगर (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				अवरोध;
			}

			हाल FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.धकेलो_back(arg);

				fmt.str += "%c";
				अवरोध;
			}

			हाल FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				अगर (part.realtime)
					arg.realtime = true;
				args.धकेलो_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				अगर (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				अगर (part.justify == FmtPart::बायाँ)
					fmt.str += '-';
				अगर (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				अवरोध;
			}

			शेष: log_abort();
		}
	}

	args.जोड़ो(args.begin(), fmt);
	वापस args;
}

std::string escape_cxx_string(स्थिर std::string &निवेश)
{
	std::string output = "\"";
	क्रम (स्वतः c : निवेश) {
		अगर (::है_छाप(c)) {
			अगर (c == '\\' || c == '"')
				output.धकेलो_back('\\');
			output.धकेलो_back(c);
		} अन्यथा {
			अक्षर l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.धकेलो_back('\\');
			output.धकेलो_back('0' + h);
			output.धकेलो_back('0' + m);
			output.धकेलो_back('0' + l);
		}
	}
	output.धकेलो_back('"');
	अगर (output.find('\0') != std::string::npos) {
		output.जोड़ो(0, "std::string {");
		output.append(stringf(", %zu}", निवेश.आकार()));
	}
	वापस output;
}

व्योम Fmt::emit_cxxrtl(std::ब_प्रवाह &os, std::string indent, std::function<व्योम(स्थिर RTLIL::SigSpec &)> emit_sig, स्थिर std::string &context) स्थिर
{
	os << indent << "std::string buf;\n";
	क्रम (स्वतः &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		चयन (part.type) {
			हाल FmtPart::LITERAL:   os << "LITERAL";   अवरोध;
			हाल FmtPart::INTEGER:   os << "INTEGER";   अवरोध;
			हाल FmtPart::STRING:    os << "STRING";    अवरोध;
			हाल FmtPart::UNICHAR:   os << "UNICHAR";   अवरोध;
			हाल FmtPart::VLOG_TIME: os << "VLOG_TIME"; अवरोध;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		चयन (part.justify) {
			हाल FmtPart::बायाँ:    os << "LEFT";    अवरोध;
			हाल FmtPart::दायाँ:   os << "RIGHT";   अवरोध;
			हाल FmtPart::NUMERIC: os << "NUMERIC"; अवरोध;
		}
		os << ", ";
		os << "(char)" << (पूर्णांक)part.padding << ", ";
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		चयन (part.sign) {
			हाल FmtPart::MINUS:       os << "MINUS";       अवरोध;
			हाल FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  अवरोध;
			हाल FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; अवरोध;
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

std::string Fmt::render() स्थिर
{
	std::string str;

	क्रम (स्वतः &part : parts) {
		चयन (part.type) {
			हाल FmtPart::LITERAL:
				str += part.str;
				अवरोध;

			हाल FmtPart::UNICHAR: {
				RTLIL::Const मूल्य = part.sig.as_const();
				uint32_t codepoint = मूल्य.as_int();
				अगर (codepoint >= 0x10000)
					str += (अक्षर)(0xf0 |  (codepoint >> 18));
				अन्यथा अगर (codepoint >= 0x800)
					str += (अक्षर)(0xe0 |  (codepoint >> 12));
				अन्यथा अगर (codepoint >= 0x80)
					str += (अक्षर)(0xc0 |  (codepoint >>  6));
				अन्यथा
					str += (अक्षर)codepoint;
				अगर (codepoint >= 0x10000)
					str += (अक्षर)(0x80 | ((codepoint >> 12) & 0x3f));
				अगर (codepoint >= 0x800)
					str += (अक्षर)(0x80 | ((codepoint >>  6) & 0x3f));
				अगर (codepoint >= 0x80)
					str += (अक्षर)(0x80 | ((codepoint >>  0) & 0x3f));
				अवरोध;
			}

			हाल FmtPart::INTEGER:
			हाल FmtPart::STRING:
			हाल FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				अगर (part.type == FmtPart::INTEGER) {
					RTLIL::Const मूल्य = part.sig.as_const();
					bool has_x = false, all_x = true, has_z = false, all_z = true;
					क्रम (State bit : मूल्य) {
						अगर (bit == State::Sx)
							has_x = true;
						अन्यथा
							all_x = false;
						अगर (bit == State::Sz)
							has_z = true;
						अन्यथा
							all_z = false;
					}

					अगर (!has_z && !has_x && part.signed_ && मूल्य[मूल्य.आकार() - 1]) {
						prefix = "-";
						मूल्य = RTLIL::const_neg(मूल्य, {}, part.signed_, {}, मूल्य.आकार() + 1);
					} अन्यथा {
						चयन (part.sign) {
							हाल FmtPart::MINUS:       अवरोध;
							हाल FmtPart::PLUS_MINUS:  prefix = "+"; अवरोध;
							हाल FmtPart::SPACE_MINUS: prefix = " "; अवरोध;
						}
					}

					अगर (part.base != 10) {
						माप_प्रकार minimum_आकार = 1;
						क्रम (माप_प्रकार सूचकांक = 0; सूचकांक < (माप_प्रकार)मूल्य.आकार(); सूचकांक++)
							अगर (मूल्य[सूचकांक] != State::S0)
								minimum_आकार = सूचकांक + 1;
						मूल्य = मूल्य.extract(0, minimum_आकार);
					}

					अगर (part.base == 2) {
						अगर (part.show_base)
							prefix += "0b";
						क्रम (माप_प्रकार सूचकांक = 0; सूचकांक < (माप_प्रकार)मूल्य.आकार(); सूचकांक++) {
							अगर (part.group && सूचकांक > 0 && सूचकांक % 4 == 0)
								buf += '_';
							RTLIL::State bit = मूल्य[सूचकांक];
							अगर (bit == State::Sx)
								buf += 'x';
							अन्यथा अगर (bit == State::Sz)
								buf += 'z';
							अन्यथा अगर (bit == State::S1)
								buf += '1';
							अन्यथा /* if (bit == State::S0) */
								buf += '0';
						}
					} अन्यथा अगर (part.base == 8 || part.base == 16) {
						अगर (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						माप_प्रकार step = (part.base == 16) ? 4 : 3;
						क्रम (माप_प्रकार सूचकांक = 0; सूचकांक < (माप_प्रकार)मूल्य.आकार(); सूचकांक += step) {
							अगर (part.group && सूचकांक > 0 && सूचकांक % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = मूल्य.extract(सूचकांक, min(step, मूल्य.आकार() - सूचकांक));
							bool has_x = false, all_x = true, has_z = false, all_z = true;
							क्रम (State bit : subvalue) {
								अगर (bit == State::Sx)
									has_x = true;
								अन्यथा
									all_x = false;
								अगर (bit == State::Sz)
									has_z = true;
								अन्यथा
									all_z = false;
							}
							अगर (all_x)
								buf += 'x';
							अन्यथा अगर (all_z)
								buf += 'z';
							अन्यथा अगर (has_x)
								buf += 'X';
							अन्यथा अगर (has_z)
								buf += 'Z';
							अन्यथा {
								स्थिर अक्षर *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} अन्यथा अगर (part.base == 10) {
						अगर (part.show_base)
							prefix += "0d";
						अगर (all_x)
							buf += 'x';
						अन्यथा अगर (all_z)
							buf += 'z';
						अन्यथा अगर (has_x)
							buf += 'X';
						अन्यथा अगर (has_z)
							buf += 'Z';
						अन्यथा {
							log_assert(मूल्य.is_fully_def());
							अगर (मूल्य.is_fully_zero())
								buf += '0';
							माप_प्रकार सूचकांक = 0;
							जबतक (!मूल्य.is_fully_zero())	{
								अगर (part.group && सूचकांक > 0 && सूचकांक % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(मूल्य, 10, false, false, 4).as_int();
								मूल्य = RTLIL::const_div(मूल्य, 10, false, false, मूल्य.आकार());
								सूचकांक++;
							}
						}
					} अन्यथा log_abort();
					अगर (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						माप_प्रकार group_आकार = part.base == 10 ? 3 : 4;
						जबतक (prefix.आकार() + buf.आकार() < part.width) {
							अगर (buf.आकार() % (group_आकार + 1) == group_आकार)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} अन्यथा अगर (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} अन्यथा अगर (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				अगर (prefix.आकार() + buf.आकार() < part.width) {
					माप_प्रकार pad_width = part.width - prefix.आकार() - buf.आकार();
					चयन (part.justify) {
						हाल FmtPart::बायाँ:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							अवरोध;
						हाल FmtPart::दायाँ:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							अवरोध;
						हाल FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							अवरोध;
					}
				} अन्यथा {
					str += prefix;
					str += buf;
				}
				अवरोध;
			}
		}
	}

	वापस str;
}
