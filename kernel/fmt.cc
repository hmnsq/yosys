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
	для (size_t i = 0; i < fmt.size(); i++) {
		якщо (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} інакше якщо (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} інакше якщо (fmt[i] == '}') {
			log_assert(false && "Unexpected '}' in format string");
		} інакше якщо (fmt[i] == '{') {
			якщо (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.push_back(part);
				part = {};
			}

			якщо (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			size_t arg_size = 0;
			для (; i < fmt.size(); i++) {
				якщо (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} інакше якщо (fmt[i] == ':') {
					++i;
					break;
				} інакше {
					log_assert(false && "Unexpected character in format substitution");
				}
			}
			якщо (i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			якщо ((size_t)args.size() < arg_size)
				log_assert(false && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			якщо (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				goto success;
			}

			якщо (fmt[i] == '>')
				part.justify = FmtPart::RIGHT;
			інакше якщо (fmt[i] == '<')
				part.justify = FmtPart::LEFT;
			інакше якщо (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			інакше
				log_assert(false && "Unexpected justification in format substitution");
			якщо (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			part.padding = fmt[i];
			якщо (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			для (; i < fmt.size(); i++) {
				якщо (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					continue;
				} інакше якщо (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} інакше якщо (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} інакше якщо (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} інакше якщо (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} інакше якщо (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = true;
				} інакше якщо (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} інакше якщо (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} інакше якщо (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = true;
				} інакше {
					log_assert(false && "Unexpected character in format substitution");
				}
				++i;
				break;
			}
			якщо (i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			якщо (part.type == FmtPart::INTEGER) {
				якщо (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					якщо (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} інакше якщо (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					якщо (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} інакше якщо (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					якщо (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} інакше {
					// also accept no sign character and treat like MINUS for compatibility
				}

				якщо (fmt[i] == '#') {
					part.show_base = true;
					++i;
				}
				якщо (fmt[i] == '_') {
					part.group = true;
					++i;
				}

				якщо (fmt[i] == 'u')
					part.signed_ = false;
				інакше якщо (fmt[i] == 's')
					part.signed_ = true;
				інакше
					log_assert(false && "Unexpected character in format substitution");
				якщо (++i == fmt.size())
					log_assert(false && "Unexpected end in format substitution");
			}

		success:
			якщо (fmt[i] != '}')
				log_assert(false && "Expected '}' after format substitution");

			parts.push_back(part);
			part = {};
		} інакше {
			part.str += fmt[i];
		}
	}
	якщо (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.push_back(part);
	}
}

void Fmt::emit_rtlil(RTLIL::Cell *cell) const {
	std::string fmt;
	RTLIL::SigSpec args;

	для (auto &part : parts) {
		switch (part.type) {
			case FmtPart::LITERAL:
				для (char c : part.str) {
					якщо (c == '{')
						fmt += "{{";
					інакше якщо (c == '}')
						fmt += "}}";
					інакше
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
				якщо (part.justify == FmtPart::RIGHT)
					fmt += '>';
				інакше якщо (part.justify == FmtPart::LEFT)
					fmt += '<';
				інакше якщо (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				інакше log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				якщо (part.width > 0)
					fmt += std::to_string(part.width);
				якщо (part.type == FmtPart::INTEGER) {
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
				} інакше якщо (part.type == FmtPart::STRING) {
					fmt += 'c';
				} інакше якщо (part.type == FmtPart::VLOG_TIME) {
					якщо (part.realtime)
						fmt += 'r';
					інакше
						fmt += 't';
				} інакше log_abort();
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
	якщо (!signed_)
		max.setBit(size, true);
	інакше
		max.setBit(size - 1, true);
	size_t places = 0;
	поки (!max.isZero()) {
		places++;
		max /= 10;
	}
	якщо (signed_)
		places++;
	повернути places;
}

static size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, true);
	size_t places = 0;
	поки (!max.isZero()) {
		places++;
		max /= base;
	}
	повернути places;
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
	якщо (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.push_back(part);
		повернути;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	якщо (part.width < places) {
		part.justify = FmtPart::RIGHT;
		part.width = places;
		parts.push_back(part);
	} інакше якщо (part.width == places) {
		parts.push_back(part);
	} інакше якщо (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		якщо (part.justify == FmtPart::RIGHT) {
			append_literal(gap);
			parts.push_back(part);
		} інакше {
			part.justify = FmtPart::RIGHT;
			parts.push_back(part);
			append_literal(gap);
		}
	}
}

void Fmt::parse_verilog(const std::vector<VerilogFmtArg> &args, bool sformat_like, ціле default_base, const std::string &task_name, const std::string &module_name)
{
	parts.clear();

	auto arg = args.begin();
	для (; arg != args.end(); ++arg) {
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
				якщо (arg == args.begin() || !sformat_like) {
					const auto fmtarg = arg;
					const std::string &fmt = fmtarg->str;
					FmtPart part = {};
					для (size_t i = 0; i < fmt.size(); i++) {
						якщо (fmt[i] != '%') {
							part.str += fmt[i];
						} інакше якщо (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} інакше якщо (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} інакше якщо (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} інакше {
							якщо (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.push_back(part);
								part = {};
							}
							якщо (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							якщо (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							для (; i < fmt.size(); i++) {
								якщо (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::LEFT;
								} інакше якщо (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} інакше break;
							}
							якщо (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							bool has_leading_zero = false, has_width = false;
							для (; i < fmt.size(); i++) {
								якщо (fmt[i] >= '0' && fmt[i] <= '9') {
									якщо (fmt[i] == '0' && !has_width) {
										has_leading_zero = true;
									} інакше {
										has_width = true;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									continue;
								} інакше якщо (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} інакше якщо (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} інакше якщо (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} інакше якщо (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} інакше якщо (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} інакше якщо (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									якщо ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} інакше якщо (fmt[i] == 't' || fmt[i] == 'T') {
									якщо (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										якщо (!has_width && !has_leading_zero)
											part.width = 20;
									} інакше {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} інакше {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								break;
							}
							якщо (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							якщо (part.padding == '\0') {
								якщо (has_leading_zero && part.justify == FmtPart::RIGHT) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} інакше {
									part.padding = ' ';
								}
							}

							якщо (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							якщо (part.base != 10)
								part.signed_ = false;
							якщо (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							інакше
								parts.push_back(part);
							part = {};
						}
					}
					якщо (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.push_back(part);
					}
				} інакше {
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

	для (auto &part : parts) {
		switch (part.type) {
			case FmtPart::LITERAL:
				для (char c : part.str) {
					якщо (c == '%')
						fmt.str += "%%";
					інакше
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
				якщо (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				якщо (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				якщо (part.width == 0) {
					fmt.str += '0';
				} інакше якщо (part.width > 0) {
					якщо (part.base != 10 || part.padding == '0')
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
				якщо (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				якщо (part.sig.size() == 8) {
					якщо (part.width > 0) {
						якщо (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} інакше {
					log_assert(part.sig.size() % 8 == 0);
					якщо (part.width > 0) {
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
				якщо (part.realtime)
					arg.realtime = true;
				args.push_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				якщо (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				якщо (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				якщо (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				break;
			}

			default: log_abort();
		}
	}

	args.insert(args.begin(), fmt);
	повернути args;
}

std::string escape_cxx_string(const std::string &input)
{
	std::string output = "\"";
	для (auto c : input) {
		якщо (::isprint(c)) {
			якщо (c == '\\' || c == '"')
				output.push_back('\\');
			output.push_back(c);
		} інакше {
			char l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.push_back('\\');
			output.push_back('0' + h);
			output.push_back('0' + m);
			output.push_back('0' + l);
		}
	}
	output.push_back('"');
	якщо (output.find('\0') != std::string::npos) {
		output.insert(0, "std::string {");
		output.append(stringf(", %zu}", input.size()));
	}
	повернути output;
}

void Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<void(const RTLIL::SigSpec &)> emit_sig, const std::string &context) const
{
	os << indent << "std::string buf;\n";
	для (auto &part : parts) {
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
		os << "(char)" << (ціле)part.padding << ", ";
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

	для (auto &part : parts) {
		switch (part.type) {
			case FmtPart::LITERAL:
				str += part.str;
				break;

			case FmtPart::UNICHAR: {
				RTLIL::Const value = part.sig.as_const();
				uint32_t codepoint = value.as_int();
				якщо (codepoint >= 0x10000)
					str += (char)(0xf0 |  (codepoint >> 18));
				інакше якщо (codepoint >= 0x800)
					str += (char)(0xe0 |  (codepoint >> 12));
				інакше якщо (codepoint >= 0x80)
					str += (char)(0xc0 |  (codepoint >>  6));
				інакше
					str += (char)codepoint;
				якщо (codepoint >= 0x10000)
					str += (char)(0x80 | ((codepoint >> 12) & 0x3f));
				якщо (codepoint >= 0x800)
					str += (char)(0x80 | ((codepoint >>  6) & 0x3f));
				якщо (codepoint >= 0x80)
					str += (char)(0x80 | ((codepoint >>  0) & 0x3f));
				break;
			}

			case FmtPart::INTEGER:
			case FmtPart::STRING:
			case FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				якщо (part.type == FmtPart::INTEGER) {
					RTLIL::Const value = part.sig.as_const();
					bool has_x = false, all_x = true, has_z = false, all_z = true;
					для (State bit : value) {
						якщо (bit == State::Sx)
							has_x = true;
						інакше
							all_x = false;
						якщо (bit == State::Sz)
							has_z = true;
						інакше
							all_z = false;
					}

					якщо (!has_z && !has_x && part.signed_ && value[value.size() - 1]) {
						prefix = "-";
						value = RTLIL::const_neg(value, {}, part.signed_, {}, value.size() + 1);
					} інакше {
						switch (part.sign) {
							case FmtPart::MINUS:       break;
							case FmtPart::PLUS_MINUS:  prefix = "+"; break;
							case FmtPart::SPACE_MINUS: prefix = " "; break;
						}
					}

					якщо (part.base != 10) {
						size_t minimum_size = 1;
						для (size_t index = 0; index < (size_t)value.size(); index++)
							якщо (value[index] != State::S0)
								minimum_size = index + 1;
						value = value.extract(0, minimum_size);
					}

					якщо (part.base == 2) {
						якщо (part.show_base)
							prefix += "0b";
						для (size_t index = 0; index < (size_t)value.size(); index++) {
							якщо (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = value[index];
							якщо (bit == State::Sx)
								buf += 'x';
							інакше якщо (bit == State::Sz)
								buf += 'z';
							інакше якщо (bit == State::S1)
								buf += '1';
							інакше /* if (bit == State::S0) */
								buf += '0';
						}
					} інакше якщо (part.base == 8 || part.base == 16) {
						якщо (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						для (size_t index = 0; index < (size_t)value.size(); index += step) {
							якщо (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = value.extract(index, min(step, value.size() - index));
							bool has_x = false, all_x = true, has_z = false, all_z = true;
							для (State bit : subvalue) {
								якщо (bit == State::Sx)
									has_x = true;
								інакше
									all_x = false;
								якщо (bit == State::Sz)
									has_z = true;
								інакше
									all_z = false;
							}
							якщо (all_x)
								buf += 'x';
							інакше якщо (all_z)
								buf += 'z';
							інакше якщо (has_x)
								buf += 'X';
							інакше якщо (has_z)
								buf += 'Z';
							інакше {
								const char *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} інакше якщо (part.base == 10) {
						якщо (part.show_base)
							prefix += "0d";
						якщо (all_x)
							buf += 'x';
						інакше якщо (all_z)
							buf += 'z';
						інакше якщо (has_x)
							buf += 'X';
						інакше якщо (has_z)
							buf += 'Z';
						інакше {
							log_assert(value.is_fully_def());
							якщо (value.is_fully_zero())
								buf += '0';
							size_t index = 0;
							поки (!value.is_fully_zero())	{
								якщо (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(value, 10, false, false, 4).as_int();
								value = RTLIL::const_div(value, 10, false, false, value.size());
								index++;
							}
						}
					} інакше log_abort();
					якщо (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						поки (prefix.size() + buf.size() < part.width) {
							якщо (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} інакше якщо (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} інакше якщо (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				якщо (prefix.size() + buf.size() < part.width) {
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
				} інакше {
					str += prefix;
					str += buf;
				}
				break;
			}
		}
	}

	повернути str;
}
