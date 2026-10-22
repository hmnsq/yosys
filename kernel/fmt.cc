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

#அந்தர்பாவ "libs/bigint/BigUnsigned.hh"
#அந்தர்பாவ "kernel/fmt.h"

USING_YOSYS_NAMESPACE

வெறுமை Fmt::append_literal(மாறிலி std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.தள்ளு_back(part);
}

வெறுமை Fmt::parse_rtlil(மாறிலி RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.clear();

	FmtPart part;
	க்கு (size_t i = 0; i < fmt.size(); i++) {
		எனில் (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} இல்லையெனில் எனில் (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} இல்லையெனில் எனில் (fmt[i] == '}') {
			log_assert(பொய் && "Unexpected '}' in format string");
		} இல்லையெனில் எனில் (fmt[i] == '{') {
			எனில் (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.தள்ளு_back(part);
				part = {};
			}

			எனில் (++i == fmt.size())
				log_assert(பொய் && "Unexpected end in format substitution");

			size_t arg_size = 0;
			க்கு (; i < fmt.size(); i++) {
				எனில் (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} இல்லையெனில் எனில் (fmt[i] == ':') {
					++i;
					நிறுத்து;
				} இல்லையெனில் {
					log_assert(பொய் && "Unexpected character in format substitution");
				}
			}
			எனில் (i == fmt.size())
				log_assert(பொய் && "Unexpected end in format substitution");

			எனில் ((size_t)args.size() < arg_size)
				log_assert(பொய் && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			எனில் (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				செல் success;
			}

			எனில் (fmt[i] == '>')
				part.justify = FmtPart::வலது;
			இல்லையெனில் எனில் (fmt[i] == '<')
				part.justify = FmtPart::இடது;
			இல்லையெனில் எனில் (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			இல்லையெனில்
				log_assert(பொய் && "Unexpected justification in format substitution");
			எனில் (++i == fmt.size())
				log_assert(பொய் && "Unexpected end in format substitution");

			part.padding = fmt[i];
			எனில் (++i == fmt.size())
				log_assert(பொய் && "Unexpected end in format substitution");

			க்கு (; i < fmt.size(); i++) {
				எனில் (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					தொடர்;
				} இல்லையெனில் எனில் (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} இல்லையெனில் எனில் (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} இல்லையெனில் எனில் (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} இல்லையெனில் எனில் (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} இல்லையெனில் எனில் (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = உண்மை;
				} இல்லையெனில் எனில் (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} இல்லையெனில் எனில் (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} இல்லையெனில் எனில் (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = உண்மை;
				} இல்லையெனில் {
					log_assert(பொய் && "Unexpected character in format substitution");
				}
				++i;
				நிறுத்து;
			}
			எனில் (i == fmt.size())
				log_assert(பொய் && "Unexpected end in format substitution");

			எனில் (part.type == FmtPart::INTEGER) {
				எனில் (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					எனில் (++i == fmt.size())
						log_assert(பொய் && "Unexpected end in format substitution");
				} இல்லையெனில் எனில் (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					எனில் (++i == fmt.size())
						log_assert(பொய் && "Unexpected end in format substitution");
				} இல்லையெனில் எனில் (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					எனில் (++i == fmt.size())
						log_assert(பொய் && "Unexpected end in format substitution");
				} இல்லையெனில் {
					// also accept no sign character and treat like MINUS for compatibility
				}

				எனில் (fmt[i] == '#') {
					part.show_base = உண்மை;
					++i;
				}
				எனில் (fmt[i] == '_') {
					part.group = உண்மை;
					++i;
				}

				எனில் (fmt[i] == 'u')
					part.signed_ = பொய்;
				இல்லையெனில் எனில் (fmt[i] == 's')
					part.signed_ = உண்மை;
				இல்லையெனில்
					log_assert(பொய் && "Unexpected character in format substitution");
				எனில் (++i == fmt.size())
					log_assert(பொய் && "Unexpected end in format substitution");
			}

		success:
			எனில் (fmt[i] != '}')
				log_assert(பொய் && "Expected '}' after format substitution");

			parts.தள்ளு_back(part);
			part = {};
		} இல்லையெனில் {
			part.str += fmt[i];
		}
	}
	எனில் (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.தள்ளு_back(part);
	}
}

வெறுமை Fmt::emit_rtlil(RTLIL::Cell *cell) மாறிலி {
	std::string fmt;
	RTLIL::SigSpec args;

	க்கு (auto &part : parts) {
		தேர்வு (part.type) {
			நிலை FmtPart::LITERAL:
				க்கு (எழுத்து c : part.str) {
					எனில் (c == '{')
						fmt += "{{";
					இல்லையெனில் எனில் (c == '}')
						fmt += "}}";
					இல்லையெனில்
						fmt += c;
				}
				நிறுத்து;

			நிலை FmtPart::UNICHAR:
				log_assert(part.sig.size() <= 32);
				fmt += "{U}";
				நிறுத்து;

			நிலை FmtPart::VLOG_TIME:
				log_assert(part.sig.size() == 0);
				YS_FALLTHROUGH
			நிலை FmtPart::STRING:
				log_assert(part.sig.size() % 8 == 0);
				YS_FALLTHROUGH
			நிலை FmtPart::INTEGER:
				args.append(part.sig);
				fmt += '{';
				fmt += std::to_string(part.sig.size());
				fmt += ':';
				எனில் (part.justify == FmtPart::வலது)
					fmt += '>';
				இல்லையெனில் எனில் (part.justify == FmtPart::இடது)
					fmt += '<';
				இல்லையெனில் எனில் (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				இல்லையெனில் log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				எனில் (part.width > 0)
					fmt += std::to_string(part.width);
				எனில் (part.type == FmtPart::INTEGER) {
					தேர்வு (part.base) {
						நிலை  2: fmt += 'b'; நிறுத்து;
						நிலை  8: fmt += 'o'; நிறுத்து;
						நிலை 10: fmt += 'd'; நிறுத்து;
						நிலை 16: fmt += part.hex_upper ? 'H' : 'h'; நிறுத்து;
						இயல்பு: log_abort();
					}
					தேர்வு (part.sign) {
						நிலை FmtPart::MINUS:       fmt += '-'; நிறுத்து;
						நிலை FmtPart::PLUS_MINUS:  fmt += '+'; நிறுத்து;
						நிலை FmtPart::SPACE_MINUS: fmt += ' '; நிறுத்து;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} இல்லையெனில் எனில் (part.type == FmtPart::STRING) {
					fmt += 'c';
				} இல்லையெனில் எனில் (part.type == FmtPart::VLOG_TIME) {
					எனில் (part.realtime)
						fmt += 'r';
					இல்லையெனில்
						fmt += 't';
				} இல்லையெனில் log_abort();
				fmt += '}';
				நிறுத்து;

			இயல்பு: log_abort();
		}
	}

	cell->setParam(ID(FORMAT), fmt);
	cell->setParam(ID(ARGS_WIDTH), args.size());
	cell->setPort(ID(ARGS), args);
}

நிலையான size_t compute_required_decimal_places(size_t size, தருக்க signed_)
{
	BigUnsigned max;
	எனில் (!signed_)
		max.setBit(size, உண்மை);
	இல்லையெனில்
		max.setBit(size - 1, உண்மை);
	size_t places = 0;
	போது (!max.isZero()) {
		places++;
		max /= 10;
	}
	எனில் (signed_)
		places++;
	திரும்பு places;
}

நிலையான size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, உண்மை);
	size_t places = 0;
	போது (!max.isZero()) {
		places++;
		max /= base;
	}
	திரும்பு places;
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
வெறுமை Fmt::apply_verilog_automatic_sizing_and_add(FmtPart &part)
{
	எனில் (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.தள்ளு_back(part);
		திரும்பு;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	எனில் (part.width < places) {
		part.justify = FmtPart::வலது;
		part.width = places;
		parts.தள்ளு_back(part);
	} இல்லையெனில் எனில் (part.width == places) {
		parts.தள்ளு_back(part);
	} இல்லையெனில் எனில் (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		எனில் (part.justify == FmtPart::வலது) {
			append_literal(gap);
			parts.தள்ளு_back(part);
		} இல்லையெனில் {
			part.justify = FmtPart::வலது;
			parts.தள்ளு_back(part);
			append_literal(gap);
		}
	}
}

வெறுமை Fmt::parse_verilog(மாறிலி std::vector<VerilogFmtArg> &args, தருக்க sformat_like, முழு default_base, மாறிலி std::string &task_name, மாறிலி std::string &module_name)
{
	parts.clear();

	auto arg = args.begin();
	க்கு (; arg != args.end(); ++arg) {
		தேர்வு (arg->type) {
			நிலை VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				நிறுத்து;
			}

			நிலை VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.தள்ளு_back(part);
				நிறுத்து;
			}

			நிலை VerilogFmtArg::STRING: {
				எனில் (arg == args.begin() || !sformat_like) {
					மாறிலி auto fmtarg = arg;
					மாறிலி std::string &fmt = fmtarg->str;
					FmtPart part = {};
					க்கு (size_t i = 0; i < fmt.size(); i++) {
						எனில் (fmt[i] != '%') {
							part.str += fmt[i];
						} இல்லையெனில் எனில் (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} இல்லையெனில் எனில் (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} இல்லையெனில் எனில் (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} இல்லையெனில் {
							எனில் (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.தள்ளு_back(part);
								part = {};
							}
							எனில் (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							எனில் (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							க்கு (; i < fmt.size(); i++) {
								எனில் (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::இடது;
								} இல்லையெனில் எனில் (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} இல்லையெனில் நிறுத்து;
							}
							எனில் (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							தருக்க has_leading_zero = பொய், has_width = பொய்;
							க்கு (; i < fmt.size(); i++) {
								எனில் (fmt[i] >= '0' && fmt[i] <= '9') {
									எனில் (fmt[i] == '0' && !has_width) {
										has_leading_zero = உண்மை;
									} இல்லையெனில் {
										has_width = உண்மை;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									தொடர்;
								} இல்லையெனில் எனில் (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} இல்லையெனில் எனில் (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} இல்லையெனில் எனில் (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} இல்லையெனில் எனில் (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} இல்லையெனில் எனில் (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} இல்லையெனில் எனில் (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									எனில் ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} இல்லையெனில் எனில் (fmt[i] == 't' || fmt[i] == 'T') {
									எனில் (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										எனில் (!has_width && !has_leading_zero)
											part.width = 20;
									} இல்லையெனில் {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} இல்லையெனில் {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								நிறுத்து;
							}
							எனில் (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							எனில் (part.padding == '\0') {
								எனில் (has_leading_zero && part.justify == FmtPart::வலது) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} இல்லையெனில் {
									part.padding = ' ';
								}
							}

							எனில் (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							எனில் (part.base != 10)
								part.signed_ = பொய்;
							எனில் (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							இல்லையெனில்
								parts.தள்ளு_back(part);
							part = {};
						}
					}
					எனில் (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.தள்ளு_back(part);
					}
				} இல்லையெனில் {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.தள்ளு_back(part);
				}
				நிறுத்து;
			}

			இயல்பு: log_abort();
		}
	}
}

std::vector<VerilogFmtArg> Fmt::emit_verilog() மாறிலி
{
	std::vector<VerilogFmtArg> args;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	க்கு (auto &part : parts) {
		தேர்வு (part.type) {
			நிலை FmtPart::LITERAL:
				க்கு (எழுத்து c : part.str) {
					எனில் (c == '%')
						fmt.str += "%%";
					இல்லையெனில்
						fmt.str += c;
				}
				நிறுத்து;

			நிலை FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.தள்ளு_back(arg);

				fmt.str += '%';
				எனில் (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				எனில் (part.justify == FmtPart::இடது)
					fmt.str += '-';
				எனில் (part.width == 0) {
					fmt.str += '0';
				} இல்லையெனில் எனில் (part.width > 0) {
					எனில் (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				தேர்வு (part.base) {
					நிலை  2: fmt.str += 'b'; நிறுத்து;
					நிலை  8: fmt.str += 'o'; நிறுத்து;
					நிலை 10: fmt.str += 'd'; நிறுத்து;
					நிலை 16: fmt.str += 'h'; நிறுத்து; // treat uppercase hex as lowercase
					இயல்பு: log_abort();
				}
				நிறுத்து;
			}

			நிலை FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				args.தள்ளு_back(arg);

				fmt.str += '%';
				எனில் (part.justify == FmtPart::இடது)
					fmt.str += '-';
				எனில் (part.sig.size() == 8) {
					எனில் (part.width > 0) {
						எனில் (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} இல்லையெனில் {
					log_assert(part.sig.size() % 8 == 0);
					எனில் (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				நிறுத்து;
			}

			நிலை FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.தள்ளு_back(arg);

				fmt.str += "%c";
				நிறுத்து;
			}

			நிலை FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				எனில் (part.realtime)
					arg.realtime = உண்மை;
				args.தள்ளு_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				எனில் (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				எனில் (part.justify == FmtPart::இடது)
					fmt.str += '-';
				எனில் (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				நிறுத்து;
			}

			இயல்பு: log_abort();
		}
	}

	args.செருகு(args.begin(), fmt);
	திரும்பு args;
}

std::string escape_cxx_string(மாறிலி std::string &உள்ளீடு)
{
	std::string output = "\"";
	க்கு (auto c : உள்ளீடு) {
		எனில் (::isprint(c)) {
			எனில் (c == '\\' || c == '"')
				output.தள்ளு_back('\\');
			output.தள்ளு_back(c);
		} இல்லையெனில் {
			எழுத்து l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.தள்ளு_back('\\');
			output.தள்ளு_back('0' + h);
			output.தள்ளு_back('0' + m);
			output.தள்ளு_back('0' + l);
		}
	}
	output.தள்ளு_back('"');
	எனில் (output.find('\0') != std::string::npos) {
		output.செருகு(0, "std::string {");
		output.append(stringf(", %zu}", உள்ளீடு.size()));
	}
	திரும்பு output;
}

வெறுமை Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<வெறுமை(மாறிலி RTLIL::SigSpec &)> emit_sig, மாறிலி std::string &context) மாறிலி
{
	os << indent << "std::string buf;\n";
	க்கு (auto &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		தேர்வு (part.type) {
			நிலை FmtPart::LITERAL:   os << "LITERAL";   நிறுத்து;
			நிலை FmtPart::INTEGER:   os << "INTEGER";   நிறுத்து;
			நிலை FmtPart::STRING:    os << "STRING";    நிறுத்து;
			நிலை FmtPart::UNICHAR:   os << "UNICHAR";   நிறுத்து;
			நிலை FmtPart::VLOG_TIME: os << "VLOG_TIME"; நிறுத்து;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		தேர்வு (part.justify) {
			நிலை FmtPart::இடது:    os << "LEFT";    நிறுத்து;
			நிலை FmtPart::வலது:   os << "RIGHT";   நிறுத்து;
			நிலை FmtPart::NUMERIC: os << "NUMERIC"; நிறுத்து;
		}
		os << ", ";
		os << "(char)" << (முழு)part.padding << ", ";
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		தேர்வு (part.sign) {
			நிலை FmtPart::MINUS:       os << "MINUS";       நிறுத்து;
			நிலை FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  நிறுத்து;
			நிலை FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; நிறுத்து;
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

std::string Fmt::render() மாறிலி
{
	std::string str;

	க்கு (auto &part : parts) {
		தேர்வு (part.type) {
			நிலை FmtPart::LITERAL:
				str += part.str;
				நிறுத்து;

			நிலை FmtPart::UNICHAR: {
				RTLIL::Const மதிப்பு = part.sig.as_const();
				uint32_t codepoint = மதிப்பு.as_int();
				எனில் (codepoint >= 0x10000)
					str += (எழுத்து)(0xf0 |  (codepoint >> 18));
				இல்லையெனில் எனில் (codepoint >= 0x800)
					str += (எழுத்து)(0xe0 |  (codepoint >> 12));
				இல்லையெனில் எனில் (codepoint >= 0x80)
					str += (எழுத்து)(0xc0 |  (codepoint >>  6));
				இல்லையெனில்
					str += (எழுத்து)codepoint;
				எனில் (codepoint >= 0x10000)
					str += (எழுத்து)(0x80 | ((codepoint >> 12) & 0x3f));
				எனில் (codepoint >= 0x800)
					str += (எழுத்து)(0x80 | ((codepoint >>  6) & 0x3f));
				எனில் (codepoint >= 0x80)
					str += (எழுத்து)(0x80 | ((codepoint >>  0) & 0x3f));
				நிறுத்து;
			}

			நிலை FmtPart::INTEGER:
			நிலை FmtPart::STRING:
			நிலை FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				எனில் (part.type == FmtPart::INTEGER) {
					RTLIL::Const மதிப்பு = part.sig.as_const();
					தருக்க has_x = பொய், all_x = உண்மை, has_z = பொய், all_z = உண்மை;
					க்கு (State bit : மதிப்பு) {
						எனில் (bit == State::Sx)
							has_x = உண்மை;
						இல்லையெனில்
							all_x = பொய்;
						எனில் (bit == State::Sz)
							has_z = உண்மை;
						இல்லையெனில்
							all_z = பொய்;
					}

					எனில் (!has_z && !has_x && part.signed_ && மதிப்பு[மதிப்பு.size() - 1]) {
						prefix = "-";
						மதிப்பு = RTLIL::const_neg(மதிப்பு, {}, part.signed_, {}, மதிப்பு.size() + 1);
					} இல்லையெனில் {
						தேர்வு (part.sign) {
							நிலை FmtPart::MINUS:       நிறுத்து;
							நிலை FmtPart::PLUS_MINUS:  prefix = "+"; நிறுத்து;
							நிலை FmtPart::SPACE_MINUS: prefix = " "; நிறுத்து;
						}
					}

					எனில் (part.base != 10) {
						size_t minimum_size = 1;
						க்கு (size_t index = 0; index < (size_t)மதிப்பு.size(); index++)
							எனில் (மதிப்பு[index] != State::S0)
								minimum_size = index + 1;
						மதிப்பு = மதிப்பு.extract(0, minimum_size);
					}

					எனில் (part.base == 2) {
						எனில் (part.show_base)
							prefix += "0b";
						க்கு (size_t index = 0; index < (size_t)மதிப்பு.size(); index++) {
							எனில் (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = மதிப்பு[index];
							எனில் (bit == State::Sx)
								buf += 'x';
							இல்லையெனில் எனில் (bit == State::Sz)
								buf += 'z';
							இல்லையெனில் எனில் (bit == State::S1)
								buf += '1';
							இல்லையெனில் /* if (bit == State::S0) */
								buf += '0';
						}
					} இல்லையெனில் எனில் (part.base == 8 || part.base == 16) {
						எனில் (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						க்கு (size_t index = 0; index < (size_t)மதிப்பு.size(); index += step) {
							எனில் (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = மதிப்பு.extract(index, min(step, மதிப்பு.size() - index));
							தருக்க has_x = பொய், all_x = உண்மை, has_z = பொய், all_z = உண்மை;
							க்கு (State bit : subvalue) {
								எனில் (bit == State::Sx)
									has_x = உண்மை;
								இல்லையெனில்
									all_x = பொய்;
								எனில் (bit == State::Sz)
									has_z = உண்மை;
								இல்லையெனில்
									all_z = பொய்;
							}
							எனில் (all_x)
								buf += 'x';
							இல்லையெனில் எனில் (all_z)
								buf += 'z';
							இல்லையெனில் எனில் (has_x)
								buf += 'X';
							இல்லையெனில் எனில் (has_z)
								buf += 'Z';
							இல்லையெனில் {
								மாறிலி எழுத்து *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} இல்லையெனில் எனில் (part.base == 10) {
						எனில் (part.show_base)
							prefix += "0d";
						எனில் (all_x)
							buf += 'x';
						இல்லையெனில் எனில் (all_z)
							buf += 'z';
						இல்லையெனில் எனில் (has_x)
							buf += 'X';
						இல்லையெனில் எனில் (has_z)
							buf += 'Z';
						இல்லையெனில் {
							log_assert(மதிப்பு.is_fully_def());
							எனில் (மதிப்பு.is_fully_zero())
								buf += '0';
							size_t index = 0;
							போது (!மதிப்பு.is_fully_zero())	{
								எனில் (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(மதிப்பு, 10, பொய், பொய், 4).as_int();
								மதிப்பு = RTLIL::const_div(மதிப்பு, 10, பொய், பொய், மதிப்பு.size());
								index++;
							}
						}
					} இல்லையெனில் log_abort();
					எனில் (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						போது (prefix.size() + buf.size() < part.width) {
							எனில் (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} இல்லையெனில் எனில் (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} இல்லையெனில் எனில் (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				எனில் (prefix.size() + buf.size() < part.width) {
					size_t pad_width = part.width - prefix.size() - buf.size();
					தேர்வு (part.justify) {
						நிலை FmtPart::இடது:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							நிறுத்து;
						நிலை FmtPart::வலது:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							நிறுத்து;
						நிலை FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							நிறுத்து;
					}
				} இல்லையெனில் {
					str += prefix;
					str += buf;
				}
				நிறுத்து;
			}
		}
	}

	திரும்பு str;
}
