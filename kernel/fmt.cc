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

#અન્તર્ભાવ "libs/bigint/BigUnsigned.hh"
#અન્તર્ભાવ "kernel/fmt.h"

USING_YOSYS_NAMESPACE

શૂન્ય Fmt::append_literal(અચલ std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.ધકેલો_back(part);
}

શૂન્ય Fmt::parse_rtlil(અચલ RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.clear();

	FmtPart part;
	માટે (size_t i = 0; i < fmt.size(); i++) {
		જો (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} નહીંતર જો (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} નહીંતર જો (fmt[i] == '}') {
			log_assert(ખોટું && "Unexpected '}' in format string");
		} નહીંતર જો (fmt[i] == '{') {
			જો (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.ધકેલો_back(part);
				part = {};
			}

			જો (++i == fmt.size())
				log_assert(ખોટું && "Unexpected end in format substitution");

			size_t arg_size = 0;
			માટે (; i < fmt.size(); i++) {
				જો (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} નહીંતર જો (fmt[i] == ':') {
					++i;
					તોડો;
				} નહીંતર {
					log_assert(ખોટું && "Unexpected character in format substitution");
				}
			}
			જો (i == fmt.size())
				log_assert(ખોટું && "Unexpected end in format substitution");

			જો ((size_t)args.size() < arg_size)
				log_assert(ખોટું && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			જો (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				જાઓ success;
			}

			જો (fmt[i] == '>')
				part.justify = FmtPart::જમણું;
			નહીંતર જો (fmt[i] == '<')
				part.justify = FmtPart::ડાબું;
			નહીંતર જો (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			નહીંતર
				log_assert(ખોટું && "Unexpected justification in format substitution");
			જો (++i == fmt.size())
				log_assert(ખોટું && "Unexpected end in format substitution");

			part.padding = fmt[i];
			જો (++i == fmt.size())
				log_assert(ખોટું && "Unexpected end in format substitution");

			માટે (; i < fmt.size(); i++) {
				જો (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					ચાલુ;
				} નહીંતર જો (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} નહીંતર જો (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} નહીંતર જો (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} નહીંતર જો (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} નહીંતર જો (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = સાચું;
				} નહીંતર જો (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} નહીંતર જો (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} નહીંતર જો (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = સાચું;
				} નહીંતર {
					log_assert(ખોટું && "Unexpected character in format substitution");
				}
				++i;
				તોડો;
			}
			જો (i == fmt.size())
				log_assert(ખોટું && "Unexpected end in format substitution");

			જો (part.type == FmtPart::INTEGER) {
				જો (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					જો (++i == fmt.size())
						log_assert(ખોટું && "Unexpected end in format substitution");
				} નહીંતર જો (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					જો (++i == fmt.size())
						log_assert(ખોટું && "Unexpected end in format substitution");
				} નહીંતર જો (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					જો (++i == fmt.size())
						log_assert(ખોટું && "Unexpected end in format substitution");
				} નહીંતર {
					// also accept no sign character and treat like MINUS for compatibility
				}

				જો (fmt[i] == '#') {
					part.show_base = સાચું;
					++i;
				}
				જો (fmt[i] == '_') {
					part.group = સાચું;
					++i;
				}

				જો (fmt[i] == 'u')
					part.signed_ = ખોટું;
				નહીંતર જો (fmt[i] == 's')
					part.signed_ = સાચું;
				નહીંતર
					log_assert(ખોટું && "Unexpected character in format substitution");
				જો (++i == fmt.size())
					log_assert(ખોટું && "Unexpected end in format substitution");
			}

		success:
			જો (fmt[i] != '}')
				log_assert(ખોટું && "Expected '}' after format substitution");

			parts.ધકેલો_back(part);
			part = {};
		} નહીંતર {
			part.str += fmt[i];
		}
	}
	જો (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.ધકેલો_back(part);
	}
}

શૂન્ય Fmt::emit_rtlil(RTLIL::Cell *cell) અચલ {
	std::string fmt;
	RTLIL::SigSpec args;

	માટે (auto &part : parts) {
		પસંદગી (part.type) {
			સ્થિતિ FmtPart::LITERAL:
				માટે (અક્ષર c : part.str) {
					જો (c == '{')
						fmt += "{{";
					નહીંતર જો (c == '}')
						fmt += "}}";
					નહીંતર
						fmt += c;
				}
				તોડો;

			સ્થિતિ FmtPart::UNICHAR:
				log_assert(part.sig.size() <= 32);
				fmt += "{U}";
				તોડો;

			સ્થિતિ FmtPart::VLOG_TIME:
				log_assert(part.sig.size() == 0);
				YS_FALLTHROUGH
			સ્થિતિ FmtPart::STRING:
				log_assert(part.sig.size() % 8 == 0);
				YS_FALLTHROUGH
			સ્થિતિ FmtPart::INTEGER:
				args.append(part.sig);
				fmt += '{';
				fmt += std::to_string(part.sig.size());
				fmt += ':';
				જો (part.justify == FmtPart::જમણું)
					fmt += '>';
				નહીંતર જો (part.justify == FmtPart::ડાબું)
					fmt += '<';
				નહીંતર જો (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				નહીંતર log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				જો (part.width > 0)
					fmt += std::to_string(part.width);
				જો (part.type == FmtPart::INTEGER) {
					પસંદગી (part.base) {
						સ્થિતિ  2: fmt += 'b'; તોડો;
						સ્થિતિ  8: fmt += 'o'; તોડો;
						સ્થિતિ 10: fmt += 'd'; તોડો;
						સ્થિતિ 16: fmt += part.hex_upper ? 'H' : 'h'; તોડો;
						મૂળભૂત: log_abort();
					}
					પસંદગી (part.sign) {
						સ્થિતિ FmtPart::MINUS:       fmt += '-'; તોડો;
						સ્થિતિ FmtPart::PLUS_MINUS:  fmt += '+'; તોડો;
						સ્થિતિ FmtPart::SPACE_MINUS: fmt += ' '; તોડો;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} નહીંતર જો (part.type == FmtPart::STRING) {
					fmt += 'c';
				} નહીંતર જો (part.type == FmtPart::VLOG_TIME) {
					જો (part.realtime)
						fmt += 'r';
					નહીંતર
						fmt += 't';
				} નહીંતર log_abort();
				fmt += '}';
				તોડો;

			મૂળભૂત: log_abort();
		}
	}

	cell->setParam(ID(FORMAT), fmt);
	cell->setParam(ID(ARGS_WIDTH), args.size());
	cell->setPort(ID(ARGS), args);
}

સ્થિર size_t compute_required_decimal_places(size_t size, તાર્કિક signed_)
{
	BigUnsigned max;
	જો (!signed_)
		max.setBit(size, સાચું);
	નહીંતર
		max.setBit(size - 1, સાચું);
	size_t places = 0;
	જ્યારે (!max.isZero()) {
		places++;
		max /= 10;
	}
	જો (signed_)
		places++;
	પાછા places;
}

સ્થિર size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, સાચું);
	size_t places = 0;
	જ્યારે (!max.isZero()) {
		places++;
		max /= base;
	}
	પાછા places;
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
શૂન્ય Fmt::apply_verilog_automatic_sizing_and_add(FmtPart &part)
{
	જો (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.ધકેલો_back(part);
		પાછા;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	જો (part.width < places) {
		part.justify = FmtPart::જમણું;
		part.width = places;
		parts.ધકેલો_back(part);
	} નહીંતર જો (part.width == places) {
		parts.ધકેલો_back(part);
	} નહીંતર જો (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		જો (part.justify == FmtPart::જમણું) {
			append_literal(gap);
			parts.ધકેલો_back(part);
		} નહીંતર {
			part.justify = FmtPart::જમણું;
			parts.ધકેલો_back(part);
			append_literal(gap);
		}
	}
}

શૂન્ય Fmt::parse_verilog(અચલ std::vector<VerilogFmtArg> &args, તાર્કિક sformat_like, પૂર્ણાંક default_base, અચલ std::string &task_name, અચલ std::string &module_name)
{
	parts.clear();

	auto arg = args.begin();
	માટે (; arg != args.end(); ++arg) {
		પસંદગી (arg->type) {
			સ્થિતિ VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				તોડો;
			}

			સ્થિતિ VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.ધકેલો_back(part);
				તોડો;
			}

			સ્થિતિ VerilogFmtArg::STRING: {
				જો (arg == args.begin() || !sformat_like) {
					અચલ auto fmtarg = arg;
					અચલ std::string &fmt = fmtarg->str;
					FmtPart part = {};
					માટે (size_t i = 0; i < fmt.size(); i++) {
						જો (fmt[i] != '%') {
							part.str += fmt[i];
						} નહીંતર જો (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} નહીંતર જો (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} નહીંતર જો (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} નહીંતર {
							જો (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.ધકેલો_back(part);
								part = {};
							}
							જો (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							જો (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							માટે (; i < fmt.size(); i++) {
								જો (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::ડાબું;
								} નહીંતર જો (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} નહીંતર તોડો;
							}
							જો (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							તાર્કિક has_leading_zero = ખોટું, has_width = ખોટું;
							માટે (; i < fmt.size(); i++) {
								જો (fmt[i] >= '0' && fmt[i] <= '9') {
									જો (fmt[i] == '0' && !has_width) {
										has_leading_zero = સાચું;
									} નહીંતર {
										has_width = સાચું;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									ચાલુ;
								} નહીંતર જો (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} નહીંતર જો (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} નહીંતર જો (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} નહીંતર જો (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} નહીંતર જો (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} નહીંતર જો (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									જો ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} નહીંતર જો (fmt[i] == 't' || fmt[i] == 'T') {
									જો (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										જો (!has_width && !has_leading_zero)
											part.width = 20;
									} નહીંતર {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} નહીંતર {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								તોડો;
							}
							જો (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							જો (part.padding == '\0') {
								જો (has_leading_zero && part.justify == FmtPart::જમણું) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} નહીંતર {
									part.padding = ' ';
								}
							}

							જો (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							જો (part.base != 10)
								part.signed_ = ખોટું;
							જો (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							નહીંતર
								parts.ધકેલો_back(part);
							part = {};
						}
					}
					જો (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.ધકેલો_back(part);
					}
				} નહીંતર {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.ધકેલો_back(part);
				}
				તોડો;
			}

			મૂળભૂત: log_abort();
		}
	}
}

std::vector<VerilogFmtArg> Fmt::emit_verilog() અચલ
{
	std::vector<VerilogFmtArg> args;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	માટે (auto &part : parts) {
		પસંદગી (part.type) {
			સ્થિતિ FmtPart::LITERAL:
				માટે (અક્ષર c : part.str) {
					જો (c == '%')
						fmt.str += "%%";
					નહીંતર
						fmt.str += c;
				}
				તોડો;

			સ્થિતિ FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.ધકેલો_back(arg);

				fmt.str += '%';
				જો (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				જો (part.justify == FmtPart::ડાબું)
					fmt.str += '-';
				જો (part.width == 0) {
					fmt.str += '0';
				} નહીંતર જો (part.width > 0) {
					જો (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				પસંદગી (part.base) {
					સ્થિતિ  2: fmt.str += 'b'; તોડો;
					સ્થિતિ  8: fmt.str += 'o'; તોડો;
					સ્થિતિ 10: fmt.str += 'd'; તોડો;
					સ્થિતિ 16: fmt.str += 'h'; તોડો; // treat uppercase hex as lowercase
					મૂળભૂત: log_abort();
				}
				તોડો;
			}

			સ્થિતિ FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				args.ધકેલો_back(arg);

				fmt.str += '%';
				જો (part.justify == FmtPart::ડાબું)
					fmt.str += '-';
				જો (part.sig.size() == 8) {
					જો (part.width > 0) {
						જો (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} નહીંતર {
					log_assert(part.sig.size() % 8 == 0);
					જો (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				તોડો;
			}

			સ્થિતિ FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.ધકેલો_back(arg);

				fmt.str += "%c";
				તોડો;
			}

			સ્થિતિ FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				જો (part.realtime)
					arg.realtime = સાચું;
				args.ધકેલો_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				જો (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				જો (part.justify == FmtPart::ડાબું)
					fmt.str += '-';
				જો (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				તોડો;
			}

			મૂળભૂત: log_abort();
		}
	}

	args.ઉમેરો(args.begin(), fmt);
	પાછા args;
}

std::string escape_cxx_string(અચલ std::string &આગત)
{
	std::string output = "\"";
	માટે (auto c : આગત) {
		જો (::isprint(c)) {
			જો (c == '\\' || c == '"')
				output.ધકેલો_back('\\');
			output.ધકેલો_back(c);
		} નહીંતર {
			અક્ષર l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.ધકેલો_back('\\');
			output.ધકેલો_back('0' + h);
			output.ધકેલો_back('0' + m);
			output.ધકેલો_back('0' + l);
		}
	}
	output.ધકેલો_back('"');
	જો (output.find('\0') != std::string::npos) {
		output.ઉમેરો(0, "std::string {");
		output.append(stringf(", %zu}", આગત.size()));
	}
	પાછા output;
}

શૂન્ય Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<શૂન્ય(અચલ RTLIL::SigSpec &)> emit_sig, અચલ std::string &context) અચલ
{
	os << indent << "std::string buf;\n";
	માટે (auto &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		પસંદગી (part.type) {
			સ્થિતિ FmtPart::LITERAL:   os << "LITERAL";   તોડો;
			સ્થિતિ FmtPart::INTEGER:   os << "INTEGER";   તોડો;
			સ્થિતિ FmtPart::STRING:    os << "STRING";    તોડો;
			સ્થિતિ FmtPart::UNICHAR:   os << "UNICHAR";   તોડો;
			સ્થિતિ FmtPart::VLOG_TIME: os << "VLOG_TIME"; તોડો;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		પસંદગી (part.justify) {
			સ્થિતિ FmtPart::ડાબું:    os << "LEFT";    તોડો;
			સ્થિતિ FmtPart::જમણું:   os << "RIGHT";   તોડો;
			સ્થિતિ FmtPart::NUMERIC: os << "NUMERIC"; તોડો;
		}
		os << ", ";
		os << "(char)" << (પૂર્ણાંક)part.padding << ", ";
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		પસંદગી (part.sign) {
			સ્થિતિ FmtPart::MINUS:       os << "MINUS";       તોડો;
			સ્થિતિ FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  તોડો;
			સ્થિતિ FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; તોડો;
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

std::string Fmt::render() અચલ
{
	std::string str;

	માટે (auto &part : parts) {
		પસંદગી (part.type) {
			સ્થિતિ FmtPart::LITERAL:
				str += part.str;
				તોડો;

			સ્થિતિ FmtPart::UNICHAR: {
				RTLIL::Const કિંમત = part.sig.as_const();
				uint32_t codepoint = કિંમત.as_int();
				જો (codepoint >= 0x10000)
					str += (અક્ષર)(0xf0 |  (codepoint >> 18));
				નહીંતર જો (codepoint >= 0x800)
					str += (અક્ષર)(0xe0 |  (codepoint >> 12));
				નહીંતર જો (codepoint >= 0x80)
					str += (અક્ષર)(0xc0 |  (codepoint >>  6));
				નહીંતર
					str += (અક્ષર)codepoint;
				જો (codepoint >= 0x10000)
					str += (અક્ષર)(0x80 | ((codepoint >> 12) & 0x3f));
				જો (codepoint >= 0x800)
					str += (અક્ષર)(0x80 | ((codepoint >>  6) & 0x3f));
				જો (codepoint >= 0x80)
					str += (અક્ષર)(0x80 | ((codepoint >>  0) & 0x3f));
				તોડો;
			}

			સ્થિતિ FmtPart::INTEGER:
			સ્થિતિ FmtPart::STRING:
			સ્થિતિ FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				જો (part.type == FmtPart::INTEGER) {
					RTLIL::Const કિંમત = part.sig.as_const();
					તાર્કિક has_x = ખોટું, all_x = સાચું, has_z = ખોટું, all_z = સાચું;
					માટે (State bit : કિંમત) {
						જો (bit == State::Sx)
							has_x = સાચું;
						નહીંતર
							all_x = ખોટું;
						જો (bit == State::Sz)
							has_z = સાચું;
						નહીંતર
							all_z = ખોટું;
					}

					જો (!has_z && !has_x && part.signed_ && કિંમત[કિંમત.size() - 1]) {
						prefix = "-";
						કિંમત = RTLIL::const_neg(કિંમત, {}, part.signed_, {}, કિંમત.size() + 1);
					} નહીંતર {
						પસંદગી (part.sign) {
							સ્થિતિ FmtPart::MINUS:       તોડો;
							સ્થિતિ FmtPart::PLUS_MINUS:  prefix = "+"; તોડો;
							સ્થિતિ FmtPart::SPACE_MINUS: prefix = " "; તોડો;
						}
					}

					જો (part.base != 10) {
						size_t minimum_size = 1;
						માટે (size_t index = 0; index < (size_t)કિંમત.size(); index++)
							જો (કિંમત[index] != State::S0)
								minimum_size = index + 1;
						કિંમત = કિંમત.extract(0, minimum_size);
					}

					જો (part.base == 2) {
						જો (part.show_base)
							prefix += "0b";
						માટે (size_t index = 0; index < (size_t)કિંમત.size(); index++) {
							જો (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = કિંમત[index];
							જો (bit == State::Sx)
								buf += 'x';
							નહીંતર જો (bit == State::Sz)
								buf += 'z';
							નહીંતર જો (bit == State::S1)
								buf += '1';
							નહીંતર /* if (bit == State::S0) */
								buf += '0';
						}
					} નહીંતર જો (part.base == 8 || part.base == 16) {
						જો (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						માટે (size_t index = 0; index < (size_t)કિંમત.size(); index += step) {
							જો (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = કિંમત.extract(index, min(step, કિંમત.size() - index));
							તાર્કિક has_x = ખોટું, all_x = સાચું, has_z = ખોટું, all_z = સાચું;
							માટે (State bit : subvalue) {
								જો (bit == State::Sx)
									has_x = સાચું;
								નહીંતર
									all_x = ખોટું;
								જો (bit == State::Sz)
									has_z = સાચું;
								નહીંતર
									all_z = ખોટું;
							}
							જો (all_x)
								buf += 'x';
							નહીંતર જો (all_z)
								buf += 'z';
							નહીંતર જો (has_x)
								buf += 'X';
							નહીંતર જો (has_z)
								buf += 'Z';
							નહીંતર {
								અચલ અક્ષર *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} નહીંતર જો (part.base == 10) {
						જો (part.show_base)
							prefix += "0d";
						જો (all_x)
							buf += 'x';
						નહીંતર જો (all_z)
							buf += 'z';
						નહીંતર જો (has_x)
							buf += 'X';
						નહીંતર જો (has_z)
							buf += 'Z';
						નહીંતર {
							log_assert(કિંમત.is_fully_def());
							જો (કિંમત.is_fully_zero())
								buf += '0';
							size_t index = 0;
							જ્યારે (!કિંમત.is_fully_zero())	{
								જો (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(કિંમત, 10, ખોટું, ખોટું, 4).as_int();
								કિંમત = RTLIL::const_div(કિંમત, 10, ખોટું, ખોટું, કિંમત.size());
								index++;
							}
						}
					} નહીંતર log_abort();
					જો (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						જ્યારે (prefix.size() + buf.size() < part.width) {
							જો (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} નહીંતર જો (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} નહીંતર જો (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				જો (prefix.size() + buf.size() < part.width) {
					size_t pad_width = part.width - prefix.size() - buf.size();
					પસંદગી (part.justify) {
						સ્થિતિ FmtPart::ડાબું:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							તોડો;
						સ્થિતિ FmtPart::જમણું:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							તોડો;
						સ્થિતિ FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							તોડો;
					}
				} નહીંતર {
					str += prefix;
					str += buf;
				}
				તોડો;
			}
		}
	}

	પાછા str;
}
