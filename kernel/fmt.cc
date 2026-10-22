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

#include "libs/bigint/BigUnsigned.hh"
#include "kernel/fmt.h"

USING_YOSYS_NAMESPACE

void Fmt::append_literal(const std::string &str) {
	FmtPart part = {};
	part.type = FmtPart::LITERAL;
	part.str = str;
	parts.push_back(part);
}

void Fmt::parse_rtlil(const RTLIL::Cell *cell) {
	std::string fmt = cell->getParam(ID(FORMAT)).decode_string();
	RTLIL::SigSpec args = cell->getPort(ID(ARGS));
	parts.clear();

	FmtPart part;
	cho (size_t i = 0; i < fmt.size(); i++) {
		nếu (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} không_thì nếu (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} không_thì nếu (fmt[i] == '}') {
			log_assert(false && "Unexpected '}' in format string");
		} không_thì nếu (fmt[i] == '{') {
			nếu (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.push_back(part);
				part = {};
			}

			nếu (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			size_t arg_size = 0;
			cho (; i < fmt.size(); i++) {
				nếu (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} không_thì nếu (fmt[i] == ':') {
					++i;
					break;
				} không_thì {
					log_assert(false && "Unexpected character in format substitution");
				}
			}
			nếu (i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			nếu ((size_t)args.size() < arg_size)
				log_assert(false && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			nếu (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				goto success;
			}

			nếu (fmt[i] == '>')
				part.justify = FmtPart::RIGHT;
			không_thì nếu (fmt[i] == '<')
				part.justify = FmtPart::LEFT;
			không_thì nếu (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			không_thì
				log_assert(false && "Unexpected justification in format substitution");
			nếu (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			part.padding = fmt[i];
			nếu (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			cho (; i < fmt.size(); i++) {
				nếu (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					continue;
				} không_thì nếu (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} không_thì nếu (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} không_thì nếu (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} không_thì nếu (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} không_thì nếu (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = true;
				} không_thì nếu (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} không_thì nếu (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} không_thì nếu (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = true;
				} không_thì {
					log_assert(false && "Unexpected character in format substitution");
				}
				++i;
				break;
			}
			nếu (i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			nếu (part.type == FmtPart::INTEGER) {
				nếu (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					nếu (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} không_thì nếu (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					nếu (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} không_thì nếu (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					nếu (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} không_thì {
					// also accept no sign character and treat like MINUS for compatibility
				}

				nếu (fmt[i] == '#') {
					part.show_base = true;
					++i;
				}
				nếu (fmt[i] == '_') {
					part.group = true;
					++i;
				}

				nếu (fmt[i] == 'u')
					part.signed_ = false;
				không_thì nếu (fmt[i] == 's')
					part.signed_ = true;
				không_thì
					log_assert(false && "Unexpected character in format substitution");
				nếu (++i == fmt.size())
					log_assert(false && "Unexpected end in format substitution");
			}

		success:
			nếu (fmt[i] != '}')
				log_assert(false && "Expected '}' after format substitution");

			parts.push_back(part);
			part = {};
		} không_thì {
			part.str += fmt[i];
		}
	}
	nếu (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.push_back(part);
	}
}

void Fmt::emit_rtlil(RTLIL::Cell *cell) const {
	std::string fmt;
	RTLIL::SigSpec args;

	cho (auto &part : parts) {
		switch (part.type) {
			case FmtPart::LITERAL:
				cho (char c : part.str) {
					nếu (c == '{')
						fmt += "{{";
					không_thì nếu (c == '}')
						fmt += "}}";
					không_thì
						fmt += c;
				}
				break;

			case FmtPart::UNICHAR:
				log_assert(part.sig.size() <= 32);
				fmt += "{U}";
				break;

			case FmtPart::VLOG_TIME:
				log_assert(part.sig.size() == 0);
				YS_FALLTHROUGH
			case FmtPart::STRING:
				log_assert(part.sig.size() % 8 == 0);
				YS_FALLTHROUGH
			case FmtPart::INTEGER:
				args.append(part.sig);
				fmt += '{';
				fmt += std::to_string(part.sig.size());
				fmt += ':';
				nếu (part.justify == FmtPart::RIGHT)
					fmt += '>';
				không_thì nếu (part.justify == FmtPart::LEFT)
					fmt += '<';
				không_thì nếu (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				không_thì log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				nếu (part.width > 0)
					fmt += std::to_string(part.width);
				nếu (part.type == FmtPart::INTEGER) {
					switch (part.base) {
						case  2: fmt += 'b'; break;
						case  8: fmt += 'o'; break;
						case 10: fmt += 'd'; break;
						case 16: fmt += part.hex_upper ? 'H' : 'h'; break;
						default: log_abort();
					}
					switch (part.sign) {
						case FmtPart::MINUS:       fmt += '-'; break;
						case FmtPart::PLUS_MINUS:  fmt += '+'; break;
						case FmtPart::SPACE_MINUS: fmt += ' '; break;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} không_thì nếu (part.type == FmtPart::STRING) {
					fmt += 'c';
				} không_thì nếu (part.type == FmtPart::VLOG_TIME) {
					nếu (part.realtime)
						fmt += 'r';
					không_thì
						fmt += 't';
				} không_thì log_abort();
				fmt += '}';
				break;

			default: log_abort();
		}
	}

	cell->setParam(ID(FORMAT), fmt);
	cell->setParam(ID(ARGS_WIDTH), args.size());
	cell->setPort(ID(ARGS), args);
}

static size_t compute_required_decimal_places(size_t size, bool signed_)
{
	BigUnsigned max;
	nếu (!signed_)
		max.setBit(size, true);
	không_thì
		max.setBit(size - 1, true);
	size_t places = 0;
	trong_khi (!max.isZero()) {
		places++;
		max /= 10;
	}
	nếu (signed_)
		places++;
	trả_về places;
}

static size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, true);
	size_t places = 0;
	trong_khi (!max.isZero()) {
		places++;
		max /= base;
	}
	trả_về places;
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
void Fmt::apply_verilog_automatic_sizing_and_add(FmtPart &part)
{
	nếu (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.push_back(part);
		trả_về;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	nếu (part.width < places) {
		part.justify = FmtPart::RIGHT;
		part.width = places;
		parts.push_back(part);
	} không_thì nếu (part.width == places) {
		parts.push_back(part);
	} không_thì nếu (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		nếu (part.justify == FmtPart::RIGHT) {
			append_literal(gap);
			parts.push_back(part);
		} không_thì {
			part.justify = FmtPart::RIGHT;
			parts.push_back(part);
			append_literal(gap);
		}
	}
}

void Fmt::parse_verilog(const std::vector<VerilogFmtArg> &args, bool sformat_like, số_nguyên default_base, const std::string &task_name, const std::string &module_name)
{
	parts.clear();

	auto arg = args.begin();
	cho (; arg != args.end(); ++arg) {
		switch (arg->type) {
			case VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				break;
			}

			case VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.push_back(part);
				break;
			}

			case VerilogFmtArg::STRING: {
				nếu (arg == args.begin() || !sformat_like) {
					const auto fmtarg = arg;
					const std::string &fmt = fmtarg->str;
					FmtPart part = {};
					cho (size_t i = 0; i < fmt.size(); i++) {
						nếu (fmt[i] != '%') {
							part.str += fmt[i];
						} không_thì nếu (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} không_thì nếu (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} không_thì nếu (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} không_thì {
							nếu (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.push_back(part);
								part = {};
							}
							nếu (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							nếu (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							cho (; i < fmt.size(); i++) {
								nếu (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::LEFT;
								} không_thì nếu (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} không_thì break;
							}
							nếu (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							bool has_leading_zero = false, has_width = false;
							cho (; i < fmt.size(); i++) {
								nếu (fmt[i] >= '0' && fmt[i] <= '9') {
									nếu (fmt[i] == '0' && !has_width) {
										has_leading_zero = true;
									} không_thì {
										has_width = true;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									continue;
								} không_thì nếu (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} không_thì nếu (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} không_thì nếu (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} không_thì nếu (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} không_thì nếu (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} không_thì nếu (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									nếu ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} không_thì nếu (fmt[i] == 't' || fmt[i] == 'T') {
									nếu (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										nếu (!has_width && !has_leading_zero)
											part.width = 20;
									} không_thì {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} không_thì {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								break;
							}
							nếu (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							nếu (part.padding == '\0') {
								nếu (has_leading_zero && part.justify == FmtPart::RIGHT) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} không_thì {
									part.padding = ' ';
								}
							}

							nếu (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							nếu (part.base != 10)
								part.signed_ = false;
							nếu (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							không_thì
								parts.push_back(part);
							part = {};
						}
					}
					nếu (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.push_back(part);
					}
				} không_thì {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.push_back(part);
				}
				break;
			}

			default: log_abort();
		}
	}
}

std::vector<VerilogFmtArg> Fmt::emit_verilog() const
{
	std::vector<VerilogFmtArg> args;
	VerilogFmtArg fmt = {};
	fmt.type = VerilogFmtArg::STRING;

	cho (auto &part : parts) {
		switch (part.type) {
			case FmtPart::LITERAL:
				cho (char c : part.str) {
					nếu (c == '%')
						fmt.str += "%%";
					không_thì
						fmt.str += c;
				}
				break;

			case FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.push_back(arg);

				fmt.str += '%';
				nếu (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				nếu (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				nếu (part.width == 0) {
					fmt.str += '0';
				} không_thì nếu (part.width > 0) {
					nếu (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				switch (part.base) {
					case  2: fmt.str += 'b'; break;
					case  8: fmt.str += 'o'; break;
					case 10: fmt.str += 'd'; break;
					case 16: fmt.str += 'h'; break; // treat uppercase hex as lowercase
					default: log_abort();
				}
				break;
			}

			case FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				args.push_back(arg);

				fmt.str += '%';
				nếu (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				nếu (part.sig.size() == 8) {
					nếu (part.width > 0) {
						nếu (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} không_thì {
					log_assert(part.sig.size() % 8 == 0);
					nếu (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				break;
			}

			case FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.push_back(arg);

				fmt.str += "%c";
				break;
			}

			case FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				nếu (part.realtime)
					arg.realtime = true;
				args.push_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				nếu (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				nếu (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				nếu (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				break;
			}

			default: log_abort();
		}
	}

	args.insert(args.begin(), fmt);
	trả_về args;
}

std::string escape_cxx_string(const std::string &input)
{
	std::string output = "\"";
	cho (auto c : input) {
		nếu (::isprint(c)) {
			nếu (c == '\\' || c == '"')
				output.push_back('\\');
			output.push_back(c);
		} không_thì {
			char l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.push_back('\\');
			output.push_back('0' + h);
			output.push_back('0' + m);
			output.push_back('0' + l);
		}
	}
	output.push_back('"');
	nếu (output.find('\0') != std::string::npos) {
		output.insert(0, "std::string {");
		output.append(stringf(", %zu}", input.size()));
	}
	trả_về output;
}

void Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<void(const RTLIL::SigSpec &)> emit_sig, const std::string &context) const
{
	os << indent << "std::string buf;\n";
	cho (auto &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		switch (part.type) {
			case FmtPart::LITERAL:   os << "LITERAL";   break;
			case FmtPart::INTEGER:   os << "INTEGER";   break;
			case FmtPart::STRING:    os << "STRING";    break;
			case FmtPart::UNICHAR:   os << "UNICHAR";   break;
			case FmtPart::VLOG_TIME: os << "VLOG_TIME"; break;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		switch (part.justify) {
			case FmtPart::LEFT:    os << "LEFT";    break;
			case FmtPart::RIGHT:   os << "RIGHT";   break;
			case FmtPart::NUMERIC: os << "NUMERIC"; break;
		}
		os << ", ";
		os << "(char)" << (số_nguyên)part.padding << ", ";
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		switch (part.sign) {
			case FmtPart::MINUS:       os << "MINUS";       break;
			case FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  break;
			case FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; break;
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

std::string Fmt::render() const
{
	std::string str;

	cho (auto &part : parts) {
		switch (part.type) {
			case FmtPart::LITERAL:
				str += part.str;
				break;

			case FmtPart::UNICHAR: {
				RTLIL::Const value = part.sig.as_const();
				uint32_t codepoint = value.as_int();
				nếu (codepoint >= 0x10000)
					str += (char)(0xf0 |  (codepoint >> 18));
				không_thì nếu (codepoint >= 0x800)
					str += (char)(0xe0 |  (codepoint >> 12));
				không_thì nếu (codepoint >= 0x80)
					str += (char)(0xc0 |  (codepoint >>  6));
				không_thì
					str += (char)codepoint;
				nếu (codepoint >= 0x10000)
					str += (char)(0x80 | ((codepoint >> 12) & 0x3f));
				nếu (codepoint >= 0x800)
					str += (char)(0x80 | ((codepoint >>  6) & 0x3f));
				nếu (codepoint >= 0x80)
					str += (char)(0x80 | ((codepoint >>  0) & 0x3f));
				break;
			}

			case FmtPart::INTEGER:
			case FmtPart::STRING:
			case FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				nếu (part.type == FmtPart::INTEGER) {
					RTLIL::Const value = part.sig.as_const();
					bool has_x = false, all_x = true, has_z = false, all_z = true;
					cho (State bit : value) {
						nếu (bit == State::Sx)
							has_x = true;
						không_thì
							all_x = false;
						nếu (bit == State::Sz)
							has_z = true;
						không_thì
							all_z = false;
					}

					nếu (!has_z && !has_x && part.signed_ && value[value.size() - 1]) {
						prefix = "-";
						value = RTLIL::const_neg(value, {}, part.signed_, {}, value.size() + 1);
					} không_thì {
						switch (part.sign) {
							case FmtPart::MINUS:       break;
							case FmtPart::PLUS_MINUS:  prefix = "+"; break;
							case FmtPart::SPACE_MINUS: prefix = " "; break;
						}
					}

					nếu (part.base != 10) {
						size_t minimum_size = 1;
						cho (size_t index = 0; index < (size_t)value.size(); index++)
							nếu (value[index] != State::S0)
								minimum_size = index + 1;
						value = value.extract(0, minimum_size);
					}

					nếu (part.base == 2) {
						nếu (part.show_base)
							prefix += "0b";
						cho (size_t index = 0; index < (size_t)value.size(); index++) {
							nếu (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = value[index];
							nếu (bit == State::Sx)
								buf += 'x';
							không_thì nếu (bit == State::Sz)
								buf += 'z';
							không_thì nếu (bit == State::S1)
								buf += '1';
							không_thì /* if (bit == State::S0) */
								buf += '0';
						}
					} không_thì nếu (part.base == 8 || part.base == 16) {
						nếu (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						cho (size_t index = 0; index < (size_t)value.size(); index += step) {
							nếu (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = value.extract(index, min(step, value.size() - index));
							bool has_x = false, all_x = true, has_z = false, all_z = true;
							cho (State bit : subvalue) {
								nếu (bit == State::Sx)
									has_x = true;
								không_thì
									all_x = false;
								nếu (bit == State::Sz)
									has_z = true;
								không_thì
									all_z = false;
							}
							nếu (all_x)
								buf += 'x';
							không_thì nếu (all_z)
								buf += 'z';
							không_thì nếu (has_x)
								buf += 'X';
							không_thì nếu (has_z)
								buf += 'Z';
							không_thì {
								const char *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} không_thì nếu (part.base == 10) {
						nếu (part.show_base)
							prefix += "0d";
						nếu (all_x)
							buf += 'x';
						không_thì nếu (all_z)
							buf += 'z';
						không_thì nếu (has_x)
							buf += 'X';
						không_thì nếu (has_z)
							buf += 'Z';
						không_thì {
							log_assert(value.is_fully_def());
							nếu (value.is_fully_zero())
								buf += '0';
							size_t index = 0;
							trong_khi (!value.is_fully_zero())	{
								nếu (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(value, 10, false, false, 4).as_int();
								value = RTLIL::const_div(value, 10, false, false, value.size());
								index++;
							}
						}
					} không_thì log_abort();
					nếu (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						trong_khi (prefix.size() + buf.size() < part.width) {
							nếu (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} không_thì nếu (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} không_thì nếu (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				nếu (prefix.size() + buf.size() < part.width) {
					size_t pad_width = part.width - prefix.size() - buf.size();
					switch (part.justify) {
						case FmtPart::LEFT:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							break;
						case FmtPart::RIGHT:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							break;
						case FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							break;
					}
				} không_thì {
					str += prefix;
					str += buf;
				}
				break;
			}
		}
	}

	trả_về str;
}
