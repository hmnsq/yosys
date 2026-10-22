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

शून्य Fmt::append_literal(अचर std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.थप_back(part);
}

शून्य Fmt::parse_rtlil(अचर RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.clear();

	FmtPart part;
	लागि (माप_प्रकार i = 0; i < fmt.size(); i++) {
		यदि (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} अन्यथा यदि (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} अन्यथा यदि (fmt[i] == '}') {
			log_assert(असत्य && "Unexpected '}' in format string");
		} अन्यथा यदि (fmt[i] == '{') {
			यदि (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.थप_back(part);
				part = {};
			}

			यदि (++i == fmt.size())
				log_assert(असत्य && "Unexpected end in format substitution");

			माप_प्रकार arg_size = 0;
			लागि (; i < fmt.size(); i++) {
				यदि (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} अन्यथा यदि (fmt[i] == ':') {
					++i;
					रोक;
				} अन्यथा {
					log_assert(असत्य && "Unexpected character in format substitution");
				}
			}
			यदि (i == fmt.size())
				log_assert(असत्य && "Unexpected end in format substitution");

			यदि ((माप_प्रकार)args.size() < arg_size)
				log_assert(असत्य && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.हटाओ(0, arg_size);

			यदि (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				गच्छ success;
			}

			यदि (fmt[i] == '>')
				part.justify = FmtPart::दायाँ;
			अन्यथा यदि (fmt[i] == '<')
				part.justify = FmtPart::बायाँ;
			अन्यथा यदि (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			अन्यथा
				log_assert(असत्य && "Unexpected justification in format substitution");
			यदि (++i == fmt.size())
				log_assert(असत्य && "Unexpected end in format substitution");

			part.padding = fmt[i];
			यदि (++i == fmt.size())
				log_assert(असत्य && "Unexpected end in format substitution");

			लागि (; i < fmt.size(); i++) {
				यदि (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					जारी;
				} अन्यथा यदि (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} अन्यथा यदि (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} अन्यथा यदि (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} अन्यथा यदि (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} अन्यथा यदि (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = सत्य;
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
				रोक;
			}
			यदि (i == fmt.size())
				log_assert(असत्य && "Unexpected end in format substitution");

			यदि (part.type == FmtPart::INTEGER) {
				यदि (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					यदि (++i == fmt.size())
						log_assert(असत्य && "Unexpected end in format substitution");
				} अन्यथा यदि (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					यदि (++i == fmt.size())
						log_assert(असत्य && "Unexpected end in format substitution");
				} अन्यथा यदि (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					यदि (++i == fmt.size())
						log_assert(असत्य && "Unexpected end in format substitution");
				} अन्यथा {
					// also accept no sign character and treat like MINUS for compatibility
				}

				यदि (fmt[i] == '#') {
					part.show_base = सत्य;
					++i;
				}
				यदि (fmt[i] == '_') {
					part.group = सत्य;
					++i;
				}

				यदि (fmt[i] == 'u')
					part.signed_ = असत्य;
				अन्यथा यदि (fmt[i] == 's')
					part.signed_ = सत्य;
				अन्यथा
					log_assert(असत्य && "Unexpected character in format substitution");
				यदि (++i == fmt.size())
					log_assert(असत्य && "Unexpected end in format substitution");
			}

		success:
			यदि (fmt[i] != '}')
				log_assert(असत्य && "Expected '}' after format substitution");

			parts.थप_back(part);
			part = {};
		} अन्यथा {
			part.str += fmt[i];
		}
	}
	यदि (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.थप_back(part);
	}
}

शून्य Fmt::emit_rtlil(RTLIL::Cell *cell) अचर {
	std::string fmt;
	RTLIL::SigSpec args;

	लागि (स्वतः &part : parts) {
		विकल्प (part.type) {
			पक्ष FmtPart::LITERAL:
				लागि (वर्ण c : part.str) {
					यदि (c == '{')
						fmt += "{{";
					अन्यथा यदि (c == '}')
						fmt += "}}";
					अन्यथा
						fmt += c;
				}
				रोक;

			पक्ष FmtPart::UNICHAR:
				log_assert(part.sig.size() <= 32);
				fmt += "{U}";
				रोक;

			पक्ष FmtPart::VLOG_TIME:
				log_assert(part.sig.size() == 0);
				YS_FALLTHROUGH
			पक्ष FmtPart::STRING:
				log_assert(part.sig.size() % 8 == 0);
				YS_FALLTHROUGH
			पक्ष FmtPart::INTEGER:
				args.append(part.sig);
				fmt += '{';
				fmt += std::to_string(part.sig.size());
				fmt += ':';
				यदि (part.justify == FmtPart::दायाँ)
					fmt += '>';
				अन्यथा यदि (part.justify == FmtPart::बायाँ)
					fmt += '<';
				अन्यथा यदि (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				अन्यथा log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				यदि (part.width > 0)
					fmt += std::to_string(part.width);
				यदि (part.type == FmtPart::INTEGER) {
					विकल्प (part.base) {
						पक्ष  2: fmt += 'b'; रोक;
						पक्ष  8: fmt += 'o'; रोक;
						पक्ष 10: fmt += 'd'; रोक;
						पक्ष 16: fmt += part.hex_upper ? 'H' : 'h'; रोक;
						स्वभाव: log_abort();
					}
					विकल्प (part.sign) {
						पक्ष FmtPart::MINUS:       fmt += '-'; रोक;
						पक्ष FmtPart::PLUS_MINUS:  fmt += '+'; रोक;
						पक्ष FmtPart::SPACE_MINUS: fmt += ' '; रोक;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
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
				रोक;

			स्वभाव: log_abort();
		}
	}

	cell->setParam(ID(FORMAT), fmt);
	cell->setParam(ID(ARGS_WIDTH), args.size());
	cell->setPort(ID(ARGS), args);
}

स्थिर माप_प्रकार compute_required_decimal_places(माप_प्रकार size, तार्किक signed_)
{
	BigUnsigned max;
	यदि (!signed_)
		max.setBit(size, सत्य);
	अन्यथा
		max.setBit(size - 1, सत्य);
	माप_प्रकार places = 0;
	जबसम्म (!max.isZero()) {
		places++;
		max /= 10;
	}
	यदि (signed_)
		places++;
	फर्काऊ places;
}

स्थिर माप_प्रकार compute_required_nondecimal_places(माप_प्रकार size, अचिन्हित base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, सत्य);
	माप_प्रकार places = 0;
	जबसम्म (!max.isZero()) {
		places++;
		max /= base;
	}
	फर्काऊ places;
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
	यदि (part.base == 10) {
		माप_प्रकार places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.थप_back(part);
		फर्काऊ;
	}

	part.padding = '0';

	माप_प्रकार places = compute_required_nondecimal_places(part.sig.size(), part.base);
	यदि (part.width < places) {
		part.justify = FmtPart::दायाँ;
		part.width = places;
		parts.थप_back(part);
	} अन्यथा यदि (part.width == places) {
		parts.थप_back(part);
	} अन्यथा यदि (part.width > places) {
		स्वतः gap = std::string(part.width - places, ' ');
		part.width = places;

		यदि (part.justify == FmtPart::दायाँ) {
			append_literal(gap);
			parts.थप_back(part);
		} अन्यथा {
			part.justify = FmtPart::दायाँ;
			parts.थप_back(part);
			append_literal(gap);
		}
	}
}

शून्य Fmt::parse_verilog(अचर std::vector<VerilogFmtArg> &args, तार्किक sformat_like, पूर्णाङ्क default_base, अचर std::string &task_name, अचर std::string &module_name)
{
	parts.clear();

	स्वतः arg = args.begin();
	लागि (; arg != args.end(); ++arg) {
		विकल्प (arg->type) {
			पक्ष VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				रोक;
			}

			पक्ष VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.थप_back(part);
				रोक;
			}

			पक्ष VerilogFmtArg::STRING: {
				यदि (arg == args.begin() || !sformat_like) {
					अचर स्वतः fmtarg = arg;
					अचर std::string &fmt = fmtarg->str;
					FmtPart part = {};
					लागि (माप_प्रकार i = 0; i < fmt.size(); i++) {
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
							यदि (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.थप_back(part);
								part = {};
							}
							यदि (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							यदि (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							लागि (; i < fmt.size(); i++) {
								यदि (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::बायाँ;
								} अन्यथा यदि (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} अन्यथा रोक;
							}
							यदि (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							तार्किक has_leading_zero = असत्य, has_width = असत्य;
							लागि (; i < fmt.size(); i++) {
								यदि (fmt[i] >= '0' && fmt[i] <= '9') {
									यदि (fmt[i] == '0' && !has_width) {
										has_leading_zero = सत्य;
									} अन्यथा {
										has_width = सत्य;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									जारी;
								} अन्यथा यदि (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} अन्यथा यदि (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} अन्यथा यदि (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} अन्यथा यदि (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} अन्यथा यदि (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} अन्यथा यदि (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									यदि ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} अन्यथा यदि (fmt[i] == 't' || fmt[i] == 'T') {
									यदि (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										यदि (!has_width && !has_leading_zero)
											part.width = 20;
									} अन्यथा {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} अन्यथा {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								रोक;
							}
							यदि (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							यदि (part.padding == '\0') {
								यदि (has_leading_zero && part.justify == FmtPart::दायाँ) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} अन्यथा {
									part.padding = ' ';
								}
							}

							यदि (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							यदि (part.base != 10)
								part.signed_ = असत्य;
							यदि (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							अन्यथा
								parts.थप_back(part);
							part = {};
						}
					}
					यदि (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.थप_back(part);
					}
				} अन्यथा {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.थप_back(part);
				}
				रोक;
			}

			स्वभाव: log_abort();
		}
	}
}

std::vector<VerilogFmtArg> Fmt::emit_verilog() अचर
{
	std::vector<VerilogFmtArg> args;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	लागि (स्वतः &part : parts) {
		विकल्प (part.type) {
			पक्ष FmtPart::LITERAL:
				लागि (वर्ण c : part.str) {
					यदि (c == '%')
						fmt.str += "%%";
					अन्यथा
						fmt.str += c;
				}
				रोक;

			पक्ष FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.थप_back(arg);

				fmt.str += '%';
				यदि (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				यदि (part.justify == FmtPart::बायाँ)
					fmt.str += '-';
				यदि (part.width == 0) {
					fmt.str += '0';
				} अन्यथा यदि (part.width > 0) {
					यदि (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				विकल्प (part.base) {
					पक्ष  2: fmt.str += 'b'; रोक;
					पक्ष  8: fmt.str += 'o'; रोक;
					पक्ष 10: fmt.str += 'd'; रोक;
					पक्ष 16: fmt.str += 'h'; रोक; // treat uppercase hex as lowercase
					स्वभाव: log_abort();
				}
				रोक;
			}

			पक्ष FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				args.थप_back(arg);

				fmt.str += '%';
				यदि (part.justify == FmtPart::बायाँ)
					fmt.str += '-';
				यदि (part.sig.size() == 8) {
					यदि (part.width > 0) {
						यदि (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} अन्यथा {
					log_assert(part.sig.size() % 8 == 0);
					यदि (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				रोक;
			}

			पक्ष FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.थप_back(arg);

				fmt.str += "%c";
				रोक;
			}

			पक्ष FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				यदि (part.realtime)
					arg.realtime = सत्य;
				args.थप_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				यदि (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				यदि (part.justify == FmtPart::बायाँ)
					fmt.str += '-';
				यदि (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				रोक;
			}

			स्वभाव: log_abort();
		}
	}

	args.घुसाउ(args.begin(), fmt);
	फर्काऊ args;
}

std::string escape_cxx_string(अचर std::string &आगत)
{
	std::string output = "\"";
	लागि (स्वतः c : आगत) {
		यदि (::है_छाप(c)) {
			यदि (c == '\\' || c == '"')
				output.थप_back('\\');
			output.थप_back(c);
		} अन्यथा {
			वर्ण l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.थप_back('\\');
			output.थप_back('0' + h);
			output.थप_back('0' + m);
			output.थप_back('0' + l);
		}
	}
	output.थप_back('"');
	यदि (output.find('\0') != std::string::npos) {
		output.घुसाउ(0, "std::string {");
		output.append(stringf(", %zu}", आगत.size()));
	}
	फर्काऊ output;
}

शून्य Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<शून्य(अचर RTLIL::SigSpec &)> emit_sig, अचर std::string &context) अचर
{
	os << indent << "std::string buf;\n";
	लागि (स्वतः &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		विकल्प (part.type) {
			पक्ष FmtPart::LITERAL:   os << "LITERAL";   रोक;
			पक्ष FmtPart::INTEGER:   os << "INTEGER";   रोक;
			पक्ष FmtPart::STRING:    os << "STRING";    रोक;
			पक्ष FmtPart::UNICHAR:   os << "UNICHAR";   रोक;
			पक्ष FmtPart::VLOG_TIME: os << "VLOG_TIME"; रोक;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		विकल्प (part.justify) {
			पक्ष FmtPart::बायाँ:    os << "LEFT";    रोक;
			पक्ष FmtPart::दायाँ:   os << "RIGHT";   रोक;
			पक्ष FmtPart::NUMERIC: os << "NUMERIC"; रोक;
		}
		os << ", ";
		os << "(char)" << (पूर्णाङ्क)part.padding << ", ";
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		विकल्प (part.sign) {
			पक्ष FmtPart::MINUS:       os << "MINUS";       रोक;
			पक्ष FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  रोक;
			पक्ष FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; रोक;
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

std::string Fmt::render() अचर
{
	std::string str;

	लागि (स्वतः &part : parts) {
		विकल्प (part.type) {
			पक्ष FmtPart::LITERAL:
				str += part.str;
				रोक;

			पक्ष FmtPart::UNICHAR: {
				RTLIL::Const मान = part.sig.as_const();
				uint32_t codepoint = मान.as_int();
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
				रोक;
			}

			पक्ष FmtPart::INTEGER:
			पक्ष FmtPart::STRING:
			पक्ष FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				यदि (part.type == FmtPart::INTEGER) {
					RTLIL::Const मान = part.sig.as_const();
					तार्किक has_x = असत्य, all_x = सत्य, has_z = असत्य, all_z = सत्य;
					लागि (State bit : मान) {
						यदि (bit == State::Sx)
							has_x = सत्य;
						अन्यथा
							all_x = असत्य;
						यदि (bit == State::Sz)
							has_z = सत्य;
						अन्यथा
							all_z = असत्य;
					}

					यदि (!has_z && !has_x && part.signed_ && मान[मान.size() - 1]) {
						prefix = "-";
						मान = RTLIL::const_neg(मान, {}, part.signed_, {}, मान.size() + 1);
					} अन्यथा {
						विकल्प (part.sign) {
							पक्ष FmtPart::MINUS:       रोक;
							पक्ष FmtPart::PLUS_MINUS:  prefix = "+"; रोक;
							पक्ष FmtPart::SPACE_MINUS: prefix = " "; रोक;
						}
					}

					यदि (part.base != 10) {
						माप_प्रकार minimum_size = 1;
						लागि (माप_प्रकार index = 0; index < (माप_प्रकार)मान.size(); index++)
							यदि (मान[index] != State::S0)
								minimum_size = index + 1;
						मान = मान.extract(0, minimum_size);
					}

					यदि (part.base == 2) {
						यदि (part.show_base)
							prefix += "0b";
						लागि (माप_प्रकार index = 0; index < (माप_प्रकार)मान.size(); index++) {
							यदि (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = मान[index];
							यदि (bit == State::Sx)
								buf += 'x';
							अन्यथा यदि (bit == State::Sz)
								buf += 'z';
							अन्यथा यदि (bit == State::S1)
								buf += '1';
							अन्यथा /* if (bit == State::S0) */
								buf += '0';
						}
					} अन्यथा यदि (part.base == 8 || part.base == 16) {
						यदि (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						माप_प्रकार step = (part.base == 16) ? 4 : 3;
						लागि (माप_प्रकार index = 0; index < (माप_प्रकार)मान.size(); index += step) {
							यदि (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = मान.extract(index, min(step, मान.size() - index));
							तार्किक has_x = असत्य, all_x = सत्य, has_z = असत्य, all_z = सत्य;
							लागि (State bit : subvalue) {
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
								अचर वर्ण *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} अन्यथा यदि (part.base == 10) {
						यदि (part.show_base)
							prefix += "0d";
						यदि (all_x)
							buf += 'x';
						अन्यथा यदि (all_z)
							buf += 'z';
						अन्यथा यदि (has_x)
							buf += 'X';
						अन्यथा यदि (has_z)
							buf += 'Z';
						अन्यथा {
							log_assert(मान.is_fully_def());
							यदि (मान.is_fully_zero())
								buf += '0';
							माप_प्रकार index = 0;
							जबसम्म (!मान.is_fully_zero())	{
								यदि (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(मान, 10, असत्य, असत्य, 4).as_int();
								मान = RTLIL::const_div(मान, 10, असत्य, असत्य, मान.size());
								index++;
							}
						}
					} अन्यथा log_abort();
					यदि (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						माप_प्रकार group_size = part.base == 10 ? 3 : 4;
						जबसम्म (prefix.size() + buf.size() < part.width) {
							यदि (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} अन्यथा यदि (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} अन्यथा यदि (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				यदि (prefix.size() + buf.size() < part.width) {
					माप_प्रकार pad_width = part.width - prefix.size() - buf.size();
					विकल्प (part.justify) {
						पक्ष FmtPart::बायाँ:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							रोक;
						पक्ष FmtPart::दायाँ:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							रोक;
						पक्ष FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							रोक;
					}
				} अन्यथा {
					str += prefix;
					str += buf;
				}
				रोक;
			}
		}
	}

	फर्काऊ str;
}
