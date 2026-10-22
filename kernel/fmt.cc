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
	parts.प्रक्षिप_back(part);
}

शून्य Fmt::parse_rtlil(ध्रुव RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.clear();

	FmtPart part;
	प्रति (size_t i = 0; i < fmt.size(); i++) {
		जइ (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} अण्णहा जइ (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} अण्णहा जइ (fmt[i] == '}') {
			log_assert(मिच्छ && "Unexpected '}' in format string");
		} अण्णहा जइ (fmt[i] == '{') {
			जइ (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.प्रक्षिप_back(part);
				part = {};
			}

			जइ (++i == fmt.size())
				log_assert(मिच्छ && "Unexpected end in format substitution");

			size_t arg_size = 0;
			प्रति (; i < fmt.size(); i++) {
				जइ (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} अण्णहा जइ (fmt[i] == ':') {
					++i;
					भङ्ग;
				} अण्णहा {
					log_assert(मिच्छ && "Unexpected character in format substitution");
				}
			}
			जइ (i == fmt.size())
				log_assert(मिच्छ && "Unexpected end in format substitution");

			जइ ((size_t)args.size() < arg_size)
				log_assert(मिच्छ && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			जइ (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				गच्छ success;
			}

			जइ (fmt[i] == '>')
				part.justify = FmtPart::दक्षिणः;
			अण्णहा जइ (fmt[i] == '<')
				part.justify = FmtPart::वामः;
			अण्णहा जइ (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			अण्णहा
				log_assert(मिच्छ && "Unexpected justification in format substitution");
			जइ (++i == fmt.size())
				log_assert(मिच्छ && "Unexpected end in format substitution");

			part.padding = fmt[i];
			जइ (++i == fmt.size())
				log_assert(मिच्छ && "Unexpected end in format substitution");

			प्रति (; i < fmt.size(); i++) {
				जइ (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					अनुवर्त;
				} अण्णहा जइ (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} अण्णहा जइ (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} अण्णहा जइ (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} अण्णहा जइ (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} अण्णहा जइ (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = सच्च;
				} अण्णहा जइ (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} अण्णहा जइ (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} अण्णहा जइ (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = सच्च;
				} अण्णहा {
					log_assert(मिच्छ && "Unexpected character in format substitution");
				}
				++i;
				भङ्ग;
			}
			जइ (i == fmt.size())
				log_assert(मिच्छ && "Unexpected end in format substitution");

			जइ (part.type == FmtPart::INTEGER) {
				जइ (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					जइ (++i == fmt.size())
						log_assert(मिच्छ && "Unexpected end in format substitution");
				} अण्णहा जइ (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					जइ (++i == fmt.size())
						log_assert(मिच्छ && "Unexpected end in format substitution");
				} अण्णहा जइ (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					जइ (++i == fmt.size())
						log_assert(मिच्छ && "Unexpected end in format substitution");
				} अण्णहा {
					// also accept no sign character and treat like MINUS for compatibility
				}

				जइ (fmt[i] == '#') {
					part.show_base = सच्च;
					++i;
				}
				जइ (fmt[i] == '_') {
					part.group = सच्च;
					++i;
				}

				जइ (fmt[i] == 'u')
					part.signed_ = मिच्छ;
				अण्णहा जइ (fmt[i] == 's')
					part.signed_ = सच्च;
				अण्णहा
					log_assert(मिच्छ && "Unexpected character in format substitution");
				जइ (++i == fmt.size())
					log_assert(मिच्छ && "Unexpected end in format substitution");
			}

		success:
			जइ (fmt[i] != '}')
				log_assert(मिच्छ && "Expected '}' after format substitution");

			parts.प्रक्षिप_back(part);
			part = {};
		} अण्णहा {
			part.str += fmt[i];
		}
	}
	जइ (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.प्रक्षिप_back(part);
	}
}

शून्य Fmt::emit_rtlil(RTLIL::Cell *cell) ध्रुव {
	std::string fmt;
	RTLIL::SigSpec args;

	प्रति (auto &part : parts) {
		विकल्प (part.type) {
			पक्ष FmtPart::LITERAL:
				प्रति (वर्ण c : part.str) {
					जइ (c == '{')
						fmt += "{{";
					अण्णहा जइ (c == '}')
						fmt += "}}";
					अण्णहा
						fmt += c;
				}
				भङ्ग;

			पक्ष FmtPart::UNICHAR:
				log_assert(part.sig.size() <= 32);
				fmt += "{U}";
				भङ्ग;

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
				जइ (part.justify == FmtPart::दक्षिणः)
					fmt += '>';
				अण्णहा जइ (part.justify == FmtPart::वामः)
					fmt += '<';
				अण्णहा जइ (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				अण्णहा log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				जइ (part.width > 0)
					fmt += std::to_string(part.width);
				जइ (part.type == FmtPart::INTEGER) {
					विकल्प (part.base) {
						पक्ष  2: fmt += 'b'; भङ्ग;
						पक्ष  8: fmt += 'o'; भङ्ग;
						पक्ष 10: fmt += 'd'; भङ्ग;
						पक्ष 16: fmt += part.hex_upper ? 'H' : 'h'; भङ्ग;
						स्वभाव: log_abort();
					}
					विकल्प (part.sign) {
						पक्ष FmtPart::MINUS:       fmt += '-'; भङ्ग;
						पक्ष FmtPart::PLUS_MINUS:  fmt += '+'; भङ्ग;
						पक्ष FmtPart::SPACE_MINUS: fmt += ' '; भङ्ग;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} अण्णहा जइ (part.type == FmtPart::STRING) {
					fmt += 'c';
				} अण्णहा जइ (part.type == FmtPart::VLOG_TIME) {
					जइ (part.realtime)
						fmt += 'r';
					अण्णहा
						fmt += 't';
				} अण्णहा log_abort();
				fmt += '}';
				भङ्ग;

			स्वभाव: log_abort();
		}
	}

	cell->setParam(ID(FORMAT), fmt);
	cell->setParam(ID(ARGS_WIDTH), args.size());
	cell->setPort(ID(ARGS), args);
}

स्थिर size_t compute_required_decimal_places(size_t size, तार्किक signed_)
{
	BigUnsigned max;
	जइ (!signed_)
		max.setBit(size, सच्च);
	अण्णहा
		max.setBit(size - 1, सच्च);
	size_t places = 0;
	जाव (!max.isZero()) {
		places++;
		max /= 10;
	}
	जइ (signed_)
		places++;
	प्रत्यागम places;
}

स्थिर size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, सच्च);
	size_t places = 0;
	जाव (!max.isZero()) {
		places++;
		max /= base;
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
शून्य Fmt::apply_verilog_automatic_sizing_and_add(FmtPart &part)
{
	जइ (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.प्रक्षिप_back(part);
		प्रत्यागम;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	जइ (part.width < places) {
		part.justify = FmtPart::दक्षिणः;
		part.width = places;
		parts.प्रक्षिप_back(part);
	} अण्णहा जइ (part.width == places) {
		parts.प्रक्षिप_back(part);
	} अण्णहा जइ (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		जइ (part.justify == FmtPart::दक्षिणः) {
			append_literal(gap);
			parts.प्रक्षिप_back(part);
		} अण्णहा {
			part.justify = FmtPart::दक्षिणः;
			parts.प्रक्षिप_back(part);
			append_literal(gap);
		}
	}
}

शून्य Fmt::parse_verilog(ध्रुव std::vector<VerilogFmtArg> &args, तार्किक sformat_like, पूर्णाङ्क default_base, ध्रुव std::string &task_name, ध्रुव std::string &module_name)
{
	parts.clear();

	auto arg = args.begin();
	प्रति (; arg != args.end(); ++arg) {
		विकल्प (arg->type) {
			पक्ष VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				भङ्ग;
			}

			पक्ष VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.प्रक्षिप_back(part);
				भङ्ग;
			}

			पक्ष VerilogFmtArg::STRING: {
				जइ (arg == args.begin() || !sformat_like) {
					ध्रुव auto fmtarg = arg;
					ध्रुव std::string &fmt = fmtarg->str;
					FmtPart part = {};
					प्रति (size_t i = 0; i < fmt.size(); i++) {
						जइ (fmt[i] != '%') {
							part.str += fmt[i];
						} अण्णहा जइ (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} अण्णहा जइ (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} अण्णहा जइ (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} अण्णहा {
							जइ (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.प्रक्षिप_back(part);
								part = {};
							}
							जइ (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							जइ (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							प्रति (; i < fmt.size(); i++) {
								जइ (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::वामः;
								} अण्णहा जइ (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} अण्णहा भङ्ग;
							}
							जइ (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							तार्किक has_leading_zero = मिच्छ, has_width = मिच्छ;
							प्रति (; i < fmt.size(); i++) {
								जइ (fmt[i] >= '0' && fmt[i] <= '9') {
									जइ (fmt[i] == '0' && !has_width) {
										has_leading_zero = सच्च;
									} अण्णहा {
										has_width = सच्च;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									अनुवर्त;
								} अण्णहा जइ (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} अण्णहा जइ (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} अण्णहा जइ (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} अण्णहा जइ (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} अण्णहा जइ (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} अण्णहा जइ (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									जइ ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} अण्णहा जइ (fmt[i] == 't' || fmt[i] == 'T') {
									जइ (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										जइ (!has_width && !has_leading_zero)
											part.width = 20;
									} अण्णहा {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} अण्णहा {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								भङ्ग;
							}
							जइ (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							जइ (part.padding == '\0') {
								जइ (has_leading_zero && part.justify == FmtPart::दक्षिणः) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} अण्णहा {
									part.padding = ' ';
								}
							}

							जइ (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							जइ (part.base != 10)
								part.signed_ = मिच्छ;
							जइ (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							अण्णहा
								parts.प्रक्षिप_back(part);
							part = {};
						}
					}
					जइ (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.प्रक्षिप_back(part);
					}
				} अण्णहा {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.प्रक्षिप_back(part);
				}
				भङ्ग;
			}

			स्वभाव: log_abort();
		}
	}
}

std::vector<VerilogFmtArg> Fmt::emit_verilog() ध्रुव
{
	std::vector<VerilogFmtArg> args;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	प्रति (auto &part : parts) {
		विकल्प (part.type) {
			पक्ष FmtPart::LITERAL:
				प्रति (वर्ण c : part.str) {
					जइ (c == '%')
						fmt.str += "%%";
					अण्णहा
						fmt.str += c;
				}
				भङ्ग;

			पक्ष FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.प्रक्षिप_back(arg);

				fmt.str += '%';
				जइ (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				जइ (part.justify == FmtPart::वामः)
					fmt.str += '-';
				जइ (part.width == 0) {
					fmt.str += '0';
				} अण्णहा जइ (part.width > 0) {
					जइ (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				विकल्प (part.base) {
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
				args.प्रक्षिप_back(arg);

				fmt.str += '%';
				जइ (part.justify == FmtPart::वामः)
					fmt.str += '-';
				जइ (part.sig.size() == 8) {
					जइ (part.width > 0) {
						जइ (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} अण्णहा {
					log_assert(part.sig.size() % 8 == 0);
					जइ (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				भङ्ग;
			}

			पक्ष FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.प्रक्षिप_back(arg);

				fmt.str += "%c";
				भङ्ग;
			}

			पक्ष FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				जइ (part.realtime)
					arg.realtime = सच्च;
				args.प्रक्षिप_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				जइ (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				जइ (part.justify == FmtPart::वामः)
					fmt.str += '-';
				जइ (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				भङ्ग;
			}

			स्वभाव: log_abort();
		}
	}

	args.निवेशय(args.begin(), fmt);
	प्रत्यागम args;
}

std::string escape_cxx_string(ध्रुव std::string &निवेशः)
{
	std::string output = "\"";
	प्रति (auto c : निवेशः) {
		जइ (::isprint(c)) {
			जइ (c == '\\' || c == '"')
				output.प्रक्षिप_back('\\');
			output.प्रक्षिप_back(c);
		} अण्णहा {
			वर्ण l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.प्रक्षिप_back('\\');
			output.प्रक्षिप_back('0' + h);
			output.प्रक्षिप_back('0' + m);
			output.प्रक्षिप_back('0' + l);
		}
	}
	output.प्रक्षिप_back('"');
	जइ (output.find('\0') != std::string::npos) {
		output.निवेशय(0, "std::string {");
		output.append(stringf(", %zu}", निवेशः.size()));
	}
	प्रत्यागम output;
}

शून्य Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<शून्य(ध्रुव RTLIL::SigSpec &)> emit_sig, ध्रुव std::string &context) ध्रुव
{
	os << indent << "std::string buf;\n";
	प्रति (auto &part : parts) {
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
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		विकल्प (part.sign) {
			पक्ष FmtPart::MINUS:       os << "MINUS";       भङ्ग;
			पक्ष FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  भङ्ग;
			पक्ष FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; भङ्ग;
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

std::string Fmt::render() ध्रुव
{
	std::string str;

	प्रति (auto &part : parts) {
		विकल्प (part.type) {
			पक्ष FmtPart::LITERAL:
				str += part.str;
				भङ्ग;

			पक्ष FmtPart::UNICHAR: {
				RTLIL::Const मानम् = part.sig.as_const();
				uint32_t codepoint = मानम्.as_int();
				जइ (codepoint >= 0x10000)
					str += (वर्ण)(0xf0 |  (codepoint >> 18));
				अण्णहा जइ (codepoint >= 0x800)
					str += (वर्ण)(0xe0 |  (codepoint >> 12));
				अण्णहा जइ (codepoint >= 0x80)
					str += (वर्ण)(0xc0 |  (codepoint >>  6));
				अण्णहा
					str += (वर्ण)codepoint;
				जइ (codepoint >= 0x10000)
					str += (वर्ण)(0x80 | ((codepoint >> 12) & 0x3f));
				जइ (codepoint >= 0x800)
					str += (वर्ण)(0x80 | ((codepoint >>  6) & 0x3f));
				जइ (codepoint >= 0x80)
					str += (वर्ण)(0x80 | ((codepoint >>  0) & 0x3f));
				भङ्ग;
			}

			पक्ष FmtPart::INTEGER:
			पक्ष FmtPart::STRING:
			पक्ष FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				जइ (part.type == FmtPart::INTEGER) {
					RTLIL::Const मानम् = part.sig.as_const();
					तार्किक has_x = मिच्छ, all_x = सच्च, has_z = मिच्छ, all_z = सच्च;
					प्रति (State bit : मानम्) {
						जइ (bit == State::Sx)
							has_x = सच्च;
						अण्णहा
							all_x = मिच्छ;
						जइ (bit == State::Sz)
							has_z = सच्च;
						अण्णहा
							all_z = मिच्छ;
					}

					जइ (!has_z && !has_x && part.signed_ && मानम्[मानम्.size() - 1]) {
						prefix = "-";
						मानम् = RTLIL::const_neg(मानम्, {}, part.signed_, {}, मानम्.size() + 1);
					} अण्णहा {
						विकल्प (part.sign) {
							पक्ष FmtPart::MINUS:       भङ्ग;
							पक्ष FmtPart::PLUS_MINUS:  prefix = "+"; भङ्ग;
							पक्ष FmtPart::SPACE_MINUS: prefix = " "; भङ्ग;
						}
					}

					जइ (part.base != 10) {
						size_t minimum_size = 1;
						प्रति (size_t index = 0; index < (size_t)मानम्.size(); index++)
							जइ (मानम्[index] != State::S0)
								minimum_size = index + 1;
						मानम् = मानम्.extract(0, minimum_size);
					}

					जइ (part.base == 2) {
						जइ (part.show_base)
							prefix += "0b";
						प्रति (size_t index = 0; index < (size_t)मानम्.size(); index++) {
							जइ (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = मानम्[index];
							जइ (bit == State::Sx)
								buf += 'x';
							अण्णहा जइ (bit == State::Sz)
								buf += 'z';
							अण्णहा जइ (bit == State::S1)
								buf += '1';
							अण्णहा /* if (bit == State::S0) */
								buf += '0';
						}
					} अण्णहा जइ (part.base == 8 || part.base == 16) {
						जइ (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						प्रति (size_t index = 0; index < (size_t)मानम्.size(); index += step) {
							जइ (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = मानम्.extract(index, min(step, मानम्.size() - index));
							तार्किक has_x = मिच्छ, all_x = सच्च, has_z = मिच्छ, all_z = सच्च;
							प्रति (State bit : subvalue) {
								जइ (bit == State::Sx)
									has_x = सच्च;
								अण्णहा
									all_x = मिच्छ;
								जइ (bit == State::Sz)
									has_z = सच्च;
								अण्णहा
									all_z = मिच्छ;
							}
							जइ (all_x)
								buf += 'x';
							अण्णहा जइ (all_z)
								buf += 'z';
							अण्णहा जइ (has_x)
								buf += 'X';
							अण्णहा जइ (has_z)
								buf += 'Z';
							अण्णहा {
								ध्रुव वर्ण *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} अण्णहा जइ (part.base == 10) {
						जइ (part.show_base)
							prefix += "0d";
						जइ (all_x)
							buf += 'x';
						अण्णहा जइ (all_z)
							buf += 'z';
						अण्णहा जइ (has_x)
							buf += 'X';
						अण्णहा जइ (has_z)
							buf += 'Z';
						अण्णहा {
							log_assert(मानम्.is_fully_def());
							जइ (मानम्.is_fully_zero())
								buf += '0';
							size_t index = 0;
							जाव (!मानम्.is_fully_zero())	{
								जइ (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(मानम्, 10, मिच्छ, मिच्छ, 4).as_int();
								मानम् = RTLIL::const_div(मानम्, 10, मिच्छ, मिच्छ, मानम्.size());
								index++;
							}
						}
					} अण्णहा log_abort();
					जइ (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						जाव (prefix.size() + buf.size() < part.width) {
							जइ (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} अण्णहा जइ (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} अण्णहा जइ (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				जइ (prefix.size() + buf.size() < part.width) {
					size_t pad_width = part.width - prefix.size() - buf.size();
					विकल्प (part.justify) {
						पक्ष FmtPart::वामः:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							भङ्ग;
						पक्ष FmtPart::दक्षिणः:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							भङ्ग;
						पक्ष FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							भङ्ग;
					}
				} अण्णहा {
					str += prefix;
					str += buf;
				}
				भङ्ग;
			}
		}
	}

	प्रत्यागम str;
}
