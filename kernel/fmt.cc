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

शून्य Fmt::append_literal(स्थिरांक std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.ढकला_back(part);
}

शून्य Fmt::parse_rtlil(स्थिरांक RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.clear();

	FmtPart part;
	साठी (माप_प्रकार i = 0; i < fmt.size(); i++) {
		जर (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} अन्यथा जर (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} अन्यथा जर (fmt[i] == '}') {
			log_assert(असत्य && "Unexpected '}' in format string");
		} अन्यथा जर (fmt[i] == '{') {
			जर (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.ढकला_back(part);
				part = {};
			}

			जर (++i == fmt.size())
				log_assert(असत्य && "Unexpected end in format substitution");

			माप_प्रकार arg_size = 0;
			साठी (; i < fmt.size(); i++) {
				जर (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} अन्यथा जर (fmt[i] == ':') {
					++i;
					थांबा;
				} अन्यथा {
					log_assert(असत्य && "Unexpected character in format substitution");
				}
			}
			जर (i == fmt.size())
				log_assert(असत्य && "Unexpected end in format substitution");

			जर ((माप_प्रकार)args.size() < arg_size)
				log_assert(असत्य && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.हटाओ(0, arg_size);

			जर (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				जा success;
			}

			जर (fmt[i] == '>')
				part.justify = FmtPart::उजवा;
			अन्यथा जर (fmt[i] == '<')
				part.justify = FmtPart::डावा;
			अन्यथा जर (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			अन्यथा
				log_assert(असत्य && "Unexpected justification in format substitution");
			जर (++i == fmt.size())
				log_assert(असत्य && "Unexpected end in format substitution");

			part.padding = fmt[i];
			जर (++i == fmt.size())
				log_assert(असत्य && "Unexpected end in format substitution");

			साठी (; i < fmt.size(); i++) {
				जर (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					सुरूठेवा;
				} अन्यथा जर (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} अन्यथा जर (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} अन्यथा जर (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} अन्यथा जर (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} अन्यथा जर (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = सत्य;
				} अन्यथा जर (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} अन्यथा जर (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} अन्यथा जर (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = सत्य;
				} अन्यथा {
					log_assert(असत्य && "Unexpected character in format substitution");
				}
				++i;
				थांबा;
			}
			जर (i == fmt.size())
				log_assert(असत्य && "Unexpected end in format substitution");

			जर (part.type == FmtPart::INTEGER) {
				जर (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					जर (++i == fmt.size())
						log_assert(असत्य && "Unexpected end in format substitution");
				} अन्यथा जर (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					जर (++i == fmt.size())
						log_assert(असत्य && "Unexpected end in format substitution");
				} अन्यथा जर (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					जर (++i == fmt.size())
						log_assert(असत्य && "Unexpected end in format substitution");
				} अन्यथा {
					// also accept no sign character and treat like MINUS for compatibility
				}

				जर (fmt[i] == '#') {
					part.show_base = सत्य;
					++i;
				}
				जर (fmt[i] == '_') {
					part.group = सत्य;
					++i;
				}

				जर (fmt[i] == 'u')
					part.signed_ = असत्य;
				अन्यथा जर (fmt[i] == 's')
					part.signed_ = सत्य;
				अन्यथा
					log_assert(असत्य && "Unexpected character in format substitution");
				जर (++i == fmt.size())
					log_assert(असत्य && "Unexpected end in format substitution");
			}

		success:
			जर (fmt[i] != '}')
				log_assert(असत्य && "Expected '}' after format substitution");

			parts.ढकला_back(part);
			part = {};
		} अन्यथा {
			part.str += fmt[i];
		}
	}
	जर (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.ढकला_back(part);
	}
}

शून्य Fmt::emit_rtlil(RTLIL::Cell *cell) स्थिरांक {
	std::string fmt;
	RTLIL::SigSpec args;

	साठी (स्वतः &part : parts) {
		निवड (part.type) {
			स्थिती FmtPart::LITERAL:
				साठी (अक्षर c : part.str) {
					जर (c == '{')
						fmt += "{{";
					अन्यथा जर (c == '}')
						fmt += "}}";
					अन्यथा
						fmt += c;
				}
				थांबा;

			स्थिती FmtPart::UNICHAR:
				log_assert(part.sig.size() <= 32);
				fmt += "{U}";
				थांबा;

			स्थिती FmtPart::VLOG_TIME:
				log_assert(part.sig.size() == 0);
				YS_FALLTHROUGH
			स्थिती FmtPart::STRING:
				log_assert(part.sig.size() % 8 == 0);
				YS_FALLTHROUGH
			स्थिती FmtPart::INTEGER:
				args.append(part.sig);
				fmt += '{';
				fmt += std::to_string(part.sig.size());
				fmt += ':';
				जर (part.justify == FmtPart::उजवा)
					fmt += '>';
				अन्यथा जर (part.justify == FmtPart::डावा)
					fmt += '<';
				अन्यथा जर (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				अन्यथा log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				जर (part.width > 0)
					fmt += std::to_string(part.width);
				जर (part.type == FmtPart::INTEGER) {
					निवड (part.base) {
						स्थिती  2: fmt += 'b'; थांबा;
						स्थिती  8: fmt += 'o'; थांबा;
						स्थिती 10: fmt += 'd'; थांबा;
						स्थिती 16: fmt += part.hex_upper ? 'H' : 'h'; थांबा;
						पूर्वनिर्धारित: log_abort();
					}
					निवड (part.sign) {
						स्थिती FmtPart::MINUS:       fmt += '-'; थांबा;
						स्थिती FmtPart::PLUS_MINUS:  fmt += '+'; थांबा;
						स्थिती FmtPart::SPACE_MINUS: fmt += ' '; थांबा;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} अन्यथा जर (part.type == FmtPart::STRING) {
					fmt += 'c';
				} अन्यथा जर (part.type == FmtPart::VLOG_TIME) {
					जर (part.realtime)
						fmt += 'r';
					अन्यथा
						fmt += 't';
				} अन्यथा log_abort();
				fmt += '}';
				थांबा;

			पूर्वनिर्धारित: log_abort();
		}
	}

	cell->setParam(ID(FORMAT), fmt);
	cell->setParam(ID(ARGS_WIDTH), args.size());
	cell->setPort(ID(ARGS), args);
}

स्थिर माप_प्रकार compute_required_decimal_places(माप_प्रकार size, तार्किक signed_)
{
	BigUnsigned max;
	जर (!signed_)
		max.setBit(size, सत्य);
	अन्यथा
		max.setBit(size - 1, सत्य);
	माप_प्रकार places = 0;
	पर्यंत (!max.isZero()) {
		places++;
		max /= 10;
	}
	जर (signed_)
		places++;
	परत places;
}

स्थिर माप_प्रकार compute_required_nondecimal_places(माप_प्रकार size, अचिन्हित base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, सत्य);
	माप_प्रकार places = 0;
	पर्यंत (!max.isZero()) {
		places++;
		max /= base;
	}
	परत places;
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
शून्य Fmt::apply_verilog_automatic_sizing_and_add(FmtPart &part)
{
	जर (part.base == 10) {
		माप_प्रकार places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.ढकला_back(part);
		परत;
	}

	part.padding = '0';

	माप_प्रकार places = compute_required_nondecimal_places(part.sig.size(), part.base);
	जर (part.width < places) {
		part.justify = FmtPart::उजवा;
		part.width = places;
		parts.ढकला_back(part);
	} अन्यथा जर (part.width == places) {
		parts.ढकला_back(part);
	} अन्यथा जर (part.width > places) {
		स्वतः gap = std::string(part.width - places, ' ');
		part.width = places;

		जर (part.justify == FmtPart::उजवा) {
			append_literal(gap);
			parts.ढकला_back(part);
		} अन्यथा {
			part.justify = FmtPart::उजवा;
			parts.ढकला_back(part);
			append_literal(gap);
		}
	}
}

शून्य Fmt::parse_verilog(स्थिरांक std::vector<VerilogFmtArg> &args, तार्किक sformat_like, पूर्णांक default_base, स्थिरांक std::string &task_name, स्थिरांक std::string &module_name)
{
	parts.clear();

	स्वतः arg = args.begin();
	साठी (; arg != args.end(); ++arg) {
		निवड (arg->type) {
			स्थिती VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				थांबा;
			}

			स्थिती VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.ढकला_back(part);
				थांबा;
			}

			स्थिती VerilogFmtArg::STRING: {
				जर (arg == args.begin() || !sformat_like) {
					स्थिरांक स्वतः fmtarg = arg;
					स्थिरांक std::string &fmt = fmtarg->str;
					FmtPart part = {};
					साठी (माप_प्रकार i = 0; i < fmt.size(); i++) {
						जर (fmt[i] != '%') {
							part.str += fmt[i];
						} अन्यथा जर (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} अन्यथा जर (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} अन्यथा जर (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} अन्यथा {
							जर (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.ढकला_back(part);
								part = {};
							}
							जर (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							जर (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							साठी (; i < fmt.size(); i++) {
								जर (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::डावा;
								} अन्यथा जर (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} अन्यथा थांबा;
							}
							जर (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							तार्किक has_leading_zero = असत्य, has_width = असत्य;
							साठी (; i < fmt.size(); i++) {
								जर (fmt[i] >= '0' && fmt[i] <= '9') {
									जर (fmt[i] == '0' && !has_width) {
										has_leading_zero = सत्य;
									} अन्यथा {
										has_width = सत्य;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									सुरूठेवा;
								} अन्यथा जर (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} अन्यथा जर (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} अन्यथा जर (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} अन्यथा जर (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} अन्यथा जर (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} अन्यथा जर (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									जर ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} अन्यथा जर (fmt[i] == 't' || fmt[i] == 'T') {
									जर (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										जर (!has_width && !has_leading_zero)
											part.width = 20;
									} अन्यथा {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} अन्यथा {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								थांबा;
							}
							जर (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							जर (part.padding == '\0') {
								जर (has_leading_zero && part.justify == FmtPart::उजवा) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} अन्यथा {
									part.padding = ' ';
								}
							}

							जर (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							जर (part.base != 10)
								part.signed_ = असत्य;
							जर (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							अन्यथा
								parts.ढकला_back(part);
							part = {};
						}
					}
					जर (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.ढकला_back(part);
					}
				} अन्यथा {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.ढकला_back(part);
				}
				थांबा;
			}

			पूर्वनिर्धारित: log_abort();
		}
	}
}

std::vector<VerilogFmtArg> Fmt::emit_verilog() स्थिरांक
{
	std::vector<VerilogFmtArg> args;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	साठी (स्वतः &part : parts) {
		निवड (part.type) {
			स्थिती FmtPart::LITERAL:
				साठी (अक्षर c : part.str) {
					जर (c == '%')
						fmt.str += "%%";
					अन्यथा
						fmt.str += c;
				}
				थांबा;

			स्थिती FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.ढकला_back(arg);

				fmt.str += '%';
				जर (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				जर (part.justify == FmtPart::डावा)
					fmt.str += '-';
				जर (part.width == 0) {
					fmt.str += '0';
				} अन्यथा जर (part.width > 0) {
					जर (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				निवड (part.base) {
					स्थिती  2: fmt.str += 'b'; थांबा;
					स्थिती  8: fmt.str += 'o'; थांबा;
					स्थिती 10: fmt.str += 'd'; थांबा;
					स्थिती 16: fmt.str += 'h'; थांबा; // treat uppercase hex as lowercase
					पूर्वनिर्धारित: log_abort();
				}
				थांबा;
			}

			स्थिती FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				args.ढकला_back(arg);

				fmt.str += '%';
				जर (part.justify == FmtPart::डावा)
					fmt.str += '-';
				जर (part.sig.size() == 8) {
					जर (part.width > 0) {
						जर (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} अन्यथा {
					log_assert(part.sig.size() % 8 == 0);
					जर (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				थांबा;
			}

			स्थिती FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.ढकला_back(arg);

				fmt.str += "%c";
				थांबा;
			}

			स्थिती FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				जर (part.realtime)
					arg.realtime = सत्य;
				args.ढकला_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				जर (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				जर (part.justify == FmtPart::डावा)
					fmt.str += '-';
				जर (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				थांबा;
			}

			पूर्वनिर्धारित: log_abort();
		}
	}

	args.घाला(args.begin(), fmt);
	परत args;
}

std::string escape_cxx_string(स्थिरांक std::string &आदान)
{
	std::string output = "\"";
	साठी (स्वतः c : आदान) {
		जर (::है_छाप(c)) {
			जर (c == '\\' || c == '"')
				output.ढकला_back('\\');
			output.ढकला_back(c);
		} अन्यथा {
			अक्षर l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.ढकला_back('\\');
			output.ढकला_back('0' + h);
			output.ढकला_back('0' + m);
			output.ढकला_back('0' + l);
		}
	}
	output.ढकला_back('"');
	जर (output.find('\0') != std::string::npos) {
		output.घाला(0, "std::string {");
		output.append(stringf(", %zu}", आदान.size()));
	}
	परत output;
}

शून्य Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<शून्य(स्थिरांक RTLIL::SigSpec &)> emit_sig, स्थिरांक std::string &context) स्थिरांक
{
	os << indent << "std::string buf;\n";
	साठी (स्वतः &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		निवड (part.type) {
			स्थिती FmtPart::LITERAL:   os << "LITERAL";   थांबा;
			स्थिती FmtPart::INTEGER:   os << "INTEGER";   थांबा;
			स्थिती FmtPart::STRING:    os << "STRING";    थांबा;
			स्थिती FmtPart::UNICHAR:   os << "UNICHAR";   थांबा;
			स्थिती FmtPart::VLOG_TIME: os << "VLOG_TIME"; थांबा;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		निवड (part.justify) {
			स्थिती FmtPart::डावा:    os << "LEFT";    थांबा;
			स्थिती FmtPart::उजवा:   os << "RIGHT";   थांबा;
			स्थिती FmtPart::NUMERIC: os << "NUMERIC"; थांबा;
		}
		os << ", ";
		os << "(char)" << (पूर्णांक)part.padding << ", ";
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		निवड (part.sign) {
			स्थिती FmtPart::MINUS:       os << "MINUS";       थांबा;
			स्थिती FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  थांबा;
			स्थिती FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; थांबा;
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

std::string Fmt::render() स्थिरांक
{
	std::string str;

	साठी (स्वतः &part : parts) {
		निवड (part.type) {
			स्थिती FmtPart::LITERAL:
				str += part.str;
				थांबा;

			स्थिती FmtPart::UNICHAR: {
				RTLIL::Const मूल्य = part.sig.as_const();
				uint32_t codepoint = मूल्य.as_int();
				जर (codepoint >= 0x10000)
					str += (अक्षर)(0xf0 |  (codepoint >> 18));
				अन्यथा जर (codepoint >= 0x800)
					str += (अक्षर)(0xe0 |  (codepoint >> 12));
				अन्यथा जर (codepoint >= 0x80)
					str += (अक्षर)(0xc0 |  (codepoint >>  6));
				अन्यथा
					str += (अक्षर)codepoint;
				जर (codepoint >= 0x10000)
					str += (अक्षर)(0x80 | ((codepoint >> 12) & 0x3f));
				जर (codepoint >= 0x800)
					str += (अक्षर)(0x80 | ((codepoint >>  6) & 0x3f));
				जर (codepoint >= 0x80)
					str += (अक्षर)(0x80 | ((codepoint >>  0) & 0x3f));
				थांबा;
			}

			स्थिती FmtPart::INTEGER:
			स्थिती FmtPart::STRING:
			स्थिती FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				जर (part.type == FmtPart::INTEGER) {
					RTLIL::Const मूल्य = part.sig.as_const();
					तार्किक has_x = असत्य, all_x = सत्य, has_z = असत्य, all_z = सत्य;
					साठी (State bit : मूल्य) {
						जर (bit == State::Sx)
							has_x = सत्य;
						अन्यथा
							all_x = असत्य;
						जर (bit == State::Sz)
							has_z = सत्य;
						अन्यथा
							all_z = असत्य;
					}

					जर (!has_z && !has_x && part.signed_ && मूल्य[मूल्य.size() - 1]) {
						prefix = "-";
						मूल्य = RTLIL::const_neg(मूल्य, {}, part.signed_, {}, मूल्य.size() + 1);
					} अन्यथा {
						निवड (part.sign) {
							स्थिती FmtPart::MINUS:       थांबा;
							स्थिती FmtPart::PLUS_MINUS:  prefix = "+"; थांबा;
							स्थिती FmtPart::SPACE_MINUS: prefix = " "; थांबा;
						}
					}

					जर (part.base != 10) {
						माप_प्रकार minimum_size = 1;
						साठी (माप_प्रकार index = 0; index < (माप_प्रकार)मूल्य.size(); index++)
							जर (मूल्य[index] != State::S0)
								minimum_size = index + 1;
						मूल्य = मूल्य.extract(0, minimum_size);
					}

					जर (part.base == 2) {
						जर (part.show_base)
							prefix += "0b";
						साठी (माप_प्रकार index = 0; index < (माप_प्रकार)मूल्य.size(); index++) {
							जर (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = मूल्य[index];
							जर (bit == State::Sx)
								buf += 'x';
							अन्यथा जर (bit == State::Sz)
								buf += 'z';
							अन्यथा जर (bit == State::S1)
								buf += '1';
							अन्यथा /* if (bit == State::S0) */
								buf += '0';
						}
					} अन्यथा जर (part.base == 8 || part.base == 16) {
						जर (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						माप_प्रकार step = (part.base == 16) ? 4 : 3;
						साठी (माप_प्रकार index = 0; index < (माप_प्रकार)मूल्य.size(); index += step) {
							जर (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = मूल्य.extract(index, min(step, मूल्य.size() - index));
							तार्किक has_x = असत्य, all_x = सत्य, has_z = असत्य, all_z = सत्य;
							साठी (State bit : subvalue) {
								जर (bit == State::Sx)
									has_x = सत्य;
								अन्यथा
									all_x = असत्य;
								जर (bit == State::Sz)
									has_z = सत्य;
								अन्यथा
									all_z = असत्य;
							}
							जर (all_x)
								buf += 'x';
							अन्यथा जर (all_z)
								buf += 'z';
							अन्यथा जर (has_x)
								buf += 'X';
							अन्यथा जर (has_z)
								buf += 'Z';
							अन्यथा {
								स्थिरांक अक्षर *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} अन्यथा जर (part.base == 10) {
						जर (part.show_base)
							prefix += "0d";
						जर (all_x)
							buf += 'x';
						अन्यथा जर (all_z)
							buf += 'z';
						अन्यथा जर (has_x)
							buf += 'X';
						अन्यथा जर (has_z)
							buf += 'Z';
						अन्यथा {
							log_assert(मूल्य.is_fully_def());
							जर (मूल्य.is_fully_zero())
								buf += '0';
							माप_प्रकार index = 0;
							पर्यंत (!मूल्य.is_fully_zero())	{
								जर (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(मूल्य, 10, असत्य, असत्य, 4).as_int();
								मूल्य = RTLIL::const_div(मूल्य, 10, असत्य, असत्य, मूल्य.size());
								index++;
							}
						}
					} अन्यथा log_abort();
					जर (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						माप_प्रकार group_size = part.base == 10 ? 3 : 4;
						पर्यंत (prefix.size() + buf.size() < part.width) {
							जर (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} अन्यथा जर (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} अन्यथा जर (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				जर (prefix.size() + buf.size() < part.width) {
					माप_प्रकार pad_width = part.width - prefix.size() - buf.size();
					निवड (part.justify) {
						स्थिती FmtPart::डावा:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							थांबा;
						स्थिती FmtPart::उजवा:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							थांबा;
						स्थिती FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							थांबा;
					}
				} अन्यथा {
					str += prefix;
					str += buf;
				}
				थांबा;
			}
		}
	}

	परत str;
}
