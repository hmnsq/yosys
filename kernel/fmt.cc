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
	สำหรับ (size_t i = 0; i < fmt.size(); i++) {
		ถ้า (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} มิฉะนั้น ถ้า (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} มิฉะนั้น ถ้า (fmt[i] == '}') {
			log_assert(false && "Unexpected '}' in format string");
		} มิฉะนั้น ถ้า (fmt[i] == '{') {
			ถ้า (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.push_back(part);
				part = {};
			}

			ถ้า (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			size_t arg_size = 0;
			สำหรับ (; i < fmt.size(); i++) {
				ถ้า (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} มิฉะนั้น ถ้า (fmt[i] == ':') {
					++i;
					bha_nga;
				} มิฉะนั้น {
					log_assert(false && "Unexpected character in format substitution");
				}
			}
			ถ้า (i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			ถ้า ((size_t)args.size() < arg_size)
				log_assert(false && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			ถ้า (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				goto success;
			}

			ถ้า (fmt[i] == '>')
				part.justify = FmtPart::RIGHT;
			มิฉะนั้น ถ้า (fmt[i] == '<')
				part.justify = FmtPart::LEFT;
			มิฉะนั้น ถ้า (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			มิฉะนั้น
				log_assert(false && "Unexpected justification in format substitution");
			ถ้า (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			part.padding = fmt[i];
			ถ้า (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			สำหรับ (; i < fmt.size(); i++) {
				ถ้า (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					continue;
				} มิฉะนั้น ถ้า (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} มิฉะนั้น ถ้า (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} มิฉะนั้น ถ้า (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} มิฉะนั้น ถ้า (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} มิฉะนั้น ถ้า (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = true;
				} มิฉะนั้น ถ้า (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} มิฉะนั้น ถ้า (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} มิฉะนั้น ถ้า (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = true;
				} มิฉะนั้น {
					log_assert(false && "Unexpected character in format substitution");
				}
				++i;
				bha_nga;
			}
			ถ้า (i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			ถ้า (part.type == FmtPart::INTEGER) {
				ถ้า (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					ถ้า (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} มิฉะนั้น ถ้า (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					ถ้า (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} มิฉะนั้น ถ้า (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					ถ้า (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} มิฉะนั้น {
					// also accept no sign character and treat like MINUS for compatibility
				}

				ถ้า (fmt[i] == '#') {
					part.show_base = true;
					++i;
				}
				ถ้า (fmt[i] == '_') {
					part.group = true;
					++i;
				}

				ถ้า (fmt[i] == 'u')
					part.signed_ = false;
				มิฉะนั้น ถ้า (fmt[i] == 's')
					part.signed_ = true;
				มิฉะนั้น
					log_assert(false && "Unexpected character in format substitution");
				ถ้า (++i == fmt.size())
					log_assert(false && "Unexpected end in format substitution");
			}

		success:
			ถ้า (fmt[i] != '}')
				log_assert(false && "Expected '}' after format substitution");

			parts.push_back(part);
			part = {};
		} มิฉะนั้น {
			part.str += fmt[i];
		}
	}
	ถ้า (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.push_back(part);
	}
}

void Fmt::emit_rtlil(RTLIL::Cell *cell) const {
	std::string fmt;
	RTLIL::SigSpec args;

	สำหรับ (auto &part : parts) {
		switch (part.type) {
			pak_sa FmtPart::LITERAL:
				สำหรับ (var_na c : part.str) {
					ถ้า (c == '{')
						fmt += "{{";
					มิฉะนั้น ถ้า (c == '}')
						fmt += "}}";
					มิฉะนั้น
						fmt += c;
				}
				bha_nga;

			pak_sa FmtPart::UNICHAR:
				log_assert(part.sig.size() <= 32);
				fmt += "{U}";
				bha_nga;

			pak_sa FmtPart::VLOG_TIME:
				log_assert(part.sig.size() == 0);
				YS_FALLTHROUGH
			pak_sa FmtPart::STRING:
				log_assert(part.sig.size() % 8 == 0);
				YS_FALLTHROUGH
			pak_sa FmtPart::INTEGER:
				args.append(part.sig);
				fmt += '{';
				fmt += std::to_string(part.sig.size());
				fmt += ':';
				ถ้า (part.justify == FmtPart::RIGHT)
					fmt += '>';
				มิฉะนั้น ถ้า (part.justify == FmtPart::LEFT)
					fmt += '<';
				มิฉะนั้น ถ้า (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				มิฉะนั้น log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				ถ้า (part.width > 0)
					fmt += std::to_string(part.width);
				ถ้า (part.type == FmtPart::INTEGER) {
					switch (part.base) {
						pak_sa  2: fmt += 'b'; bha_nga;
						pak_sa  8: fmt += 'o'; bha_nga;
						pak_sa 10: fmt += 'd'; bha_nga;
						pak_sa 16: fmt += part.hex_upper ? 'H' : 'h'; bha_nga;
						default: log_abort();
					}
					switch (part.sign) {
						pak_sa FmtPart::MINUS:       fmt += '-'; bha_nga;
						pak_sa FmtPart::PLUS_MINUS:  fmt += '+'; bha_nga;
						pak_sa FmtPart::SPACE_MINUS: fmt += ' '; bha_nga;
					}
					fmt += part.show_base ? "#" : "";
					fmt += part.group ? "_" : "";
					fmt += part.signed_ ? 's' : 'u';
				} มิฉะนั้น ถ้า (part.type == FmtPart::STRING) {
					fmt += 'c';
				} มิฉะนั้น ถ้า (part.type == FmtPart::VLOG_TIME) {
					ถ้า (part.realtime)
						fmt += 'r';
					มิฉะนั้น
						fmt += 't';
				} มิฉะนั้น log_abort();
				fmt += '}';
				bha_nga;

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
	ถ้า (!signed_)
		max.setBit(size, true);
	มิฉะนั้น
		max.setBit(size - 1, true);
	size_t places = 0;
	ขณะ (!max.isZero()) {
		places++;
		max /= 10;
	}
	ถ้า (signed_)
		places++;
	คืน places;
}

static size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, true);
	size_t places = 0;
	ขณะ (!max.isZero()) {
		places++;
		max /= base;
	}
	คืน places;
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
	ถ้า (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.push_back(part);
		คืน;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	ถ้า (part.width < places) {
		part.justify = FmtPart::RIGHT;
		part.width = places;
		parts.push_back(part);
	} มิฉะนั้น ถ้า (part.width == places) {
		parts.push_back(part);
	} มิฉะนั้น ถ้า (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		ถ้า (part.justify == FmtPart::RIGHT) {
			append_literal(gap);
			parts.push_back(part);
		} มิฉะนั้น {
			part.justify = FmtPart::RIGHT;
			parts.push_back(part);
			append_literal(gap);
		}
	}
}

void Fmt::parse_verilog(const std::vector<VerilogFmtArg> &args, bool sformat_like, จำนวนเต็ม default_base, const std::string &task_name, const std::string &module_name)
{
	parts.clear();

	auto arg = args.begin();
	สำหรับ (; arg != args.end(); ++arg) {
		switch (arg->type) {
			pak_sa VerilogFmtArg::INTEGER: {
				FmtPart part = {};
				part.type = FmtPart::INTEGER;
				part.sig = arg->sig;
				part.base = default_base;
				part.signed_ = arg->signed_;
				apply_verilog_automatic_sizing_and_add(part);
				bha_nga;
			}

			pak_sa VerilogFmtArg::TIME: {
				FmtPart part = {};
				part.type = FmtPart::VLOG_TIME;
				part.realtime = arg->realtime;
				part.padding = ' ';
				part.width = 20;
				parts.push_back(part);
				bha_nga;
			}

			pak_sa VerilogFmtArg::STRING: {
				ถ้า (arg == args.begin() || !sformat_like) {
					const auto fmtarg = arg;
					const std::string &fmt = fmtarg->str;
					FmtPart part = {};
					สำหรับ (size_t i = 0; i < fmt.size(); i++) {
						ถ้า (fmt[i] != '%') {
							part.str += fmt[i];
						} มิฉะนั้น ถ้า (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} มิฉะนั้น ถ้า (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} มิฉะนั้น ถ้า (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} มิฉะนั้น {
							ถ้า (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.push_back(part);
								part = {};
							}
							ถ้า (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ถ้า (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							สำหรับ (; i < fmt.size(); i++) {
								ถ้า (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::LEFT;
								} มิฉะนั้น ถ้า (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} มิฉะนั้น bha_nga;
							}
							ถ้า (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							bool has_leading_zero = false, has_width = false;
							สำหรับ (; i < fmt.size(); i++) {
								ถ้า (fmt[i] >= '0' && fmt[i] <= '9') {
									ถ้า (fmt[i] == '0' && !has_width) {
										has_leading_zero = true;
									} มิฉะนั้น {
										has_width = true;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									continue;
								} มิฉะนั้น ถ้า (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} มิฉะนั้น ถ้า (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} มิฉะนั้น ถ้า (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} มิฉะนั้น ถ้า (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} มิฉะนั้น ถ้า (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} มิฉะนั้น ถ้า (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									ถ้า ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} มิฉะนั้น ถ้า (fmt[i] == 't' || fmt[i] == 'T') {
									ถ้า (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										ถ้า (!has_width && !has_leading_zero)
											part.width = 20;
									} มิฉะนั้น {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} มิฉะนั้น {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								bha_nga;
							}
							ถ้า (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ถ้า (part.padding == '\0') {
								ถ้า (has_leading_zero && part.justify == FmtPart::RIGHT) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} มิฉะนั้น {
									part.padding = ' ';
								}
							}

							ถ้า (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							ถ้า (part.base != 10)
								part.signed_ = false;
							ถ้า (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							มิฉะนั้น
								parts.push_back(part);
							part = {};
						}
					}
					ถ้า (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.push_back(part);
					}
				} มิฉะนั้น {
					FmtPart part = {};
					part.type = FmtPart::LITERAL;
					part.str = arg->str;
					parts.push_back(part);
				}
				bha_nga;
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

	สำหรับ (auto &part : parts) {
		switch (part.type) {
			pak_sa FmtPart::LITERAL:
				สำหรับ (var_na c : part.str) {
					ถ้า (c == '%')
						fmt.str += "%%";
					มิฉะนั้น
						fmt.str += c;
				}
				bha_nga;

			pak_sa FmtPart::INTEGER: {
				VerilogFmtArg arg = {};
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				arg.signed_ = part.signed_;
				args.push_back(arg);

				fmt.str += '%';
				ถ้า (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				ถ้า (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				ถ้า (part.width == 0) {
					fmt.str += '0';
				} มิฉะนั้น ถ้า (part.width > 0) {
					ถ้า (part.base != 10 || part.padding == '0')
						fmt.str += '0';
					fmt.str += std::to_string(part.width);
				}
				switch (part.base) {
					pak_sa  2: fmt.str += 'b'; bha_nga;
					pak_sa  8: fmt.str += 'o'; bha_nga;
					pak_sa 10: fmt.str += 'd'; bha_nga;
					pak_sa 16: fmt.str += 'h'; bha_nga; // treat uppercase hex as lowercase
					default: log_abort();
				}
				bha_nga;
			}

			pak_sa FmtPart::STRING: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig;
				args.push_back(arg);

				fmt.str += '%';
				ถ้า (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				ถ้า (part.sig.size() == 8) {
					ถ้า (part.width > 0) {
						ถ้า (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} มิฉะนั้น {
					log_assert(part.sig.size() % 8 == 0);
					ถ้า (part.width > 0) {
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 's';
				}
				bha_nga;
			}

			pak_sa FmtPart::UNICHAR: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::INTEGER;
				arg.sig = part.sig.extract(0, 7); // only ASCII
				args.push_back(arg);

				fmt.str += "%c";
				bha_nga;
			}

			pak_sa FmtPart::VLOG_TIME: {
				VerilogFmtArg arg;
				arg.type = VerilogFmtArg::TIME;
				ถ้า (part.realtime)
					arg.realtime = true;
				args.push_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				ถ้า (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				ถ้า (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				ถ้า (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				bha_nga;
			}

			default: log_abort();
		}
	}

	args.insert(args.begin(), fmt);
	คืน args;
}

std::string escape_cxx_string(const std::string &input)
{
	std::string output = "\"";
	สำหรับ (auto c : input) {
		ถ้า (::isprint(c)) {
			ถ้า (c == '\\' || c == '"')
				output.push_back('\\');
			output.push_back(c);
		} มิฉะนั้น {
			var_na l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.push_back('\\');
			output.push_back('0' + h);
			output.push_back('0' + m);
			output.push_back('0' + l);
		}
	}
	output.push_back('"');
	ถ้า (output.find('\0') != std::string::npos) {
		output.insert(0, "std::string {");
		output.append(stringf(", %zu}", input.size()));
	}
	คืน output;
}

void Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<void(const RTLIL::SigSpec &)> emit_sig, const std::string &context) const
{
	os << indent << "std::string buf;\n";
	สำหรับ (auto &part : parts) {
		os << indent << "buf += fmt_part { ";
		os << "fmt_part::";
		switch (part.type) {
			pak_sa FmtPart::LITERAL:   os << "LITERAL";   bha_nga;
			pak_sa FmtPart::INTEGER:   os << "INTEGER";   bha_nga;
			pak_sa FmtPart::STRING:    os << "STRING";    bha_nga;
			pak_sa FmtPart::UNICHAR:   os << "UNICHAR";   bha_nga;
			pak_sa FmtPart::VLOG_TIME: os << "VLOG_TIME"; bha_nga;
		}
		os << ", ";
		os << escape_cxx_string(part.str) << ", ";
		os << "fmt_part::";
		switch (part.justify) {
			pak_sa FmtPart::LEFT:    os << "LEFT";    bha_nga;
			pak_sa FmtPart::RIGHT:   os << "RIGHT";   bha_nga;
			pak_sa FmtPart::NUMERIC: os << "NUMERIC"; bha_nga;
		}
		os << ", ";
		os << "(char)" << (จำนวนเต็ม)part.padding << ", ";
		os << part.width << ", ";
		os << part.base << ", ";
		os << part.signed_ << ", ";
		os << "fmt_part::";
		switch (part.sign) {
			pak_sa FmtPart::MINUS:       os << "MINUS";       bha_nga;
			pak_sa FmtPart::PLUS_MINUS:  os << "PLUS_MINUS";  bha_nga;
			pak_sa FmtPart::SPACE_MINUS: os << "SPACE_MINUS"; bha_nga;
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

	สำหรับ (auto &part : parts) {
		switch (part.type) {
			pak_sa FmtPart::LITERAL:
				str += part.str;
				bha_nga;

			pak_sa FmtPart::UNICHAR: {
				RTLIL::Const value = part.sig.as_const();
				uint32_t codepoint = value.as_int();
				ถ้า (codepoint >= 0x10000)
					str += (var_na)(0xf0 |  (codepoint >> 18));
				มิฉะนั้น ถ้า (codepoint >= 0x800)
					str += (var_na)(0xe0 |  (codepoint >> 12));
				มิฉะนั้น ถ้า (codepoint >= 0x80)
					str += (var_na)(0xc0 |  (codepoint >>  6));
				มิฉะนั้น
					str += (var_na)codepoint;
				ถ้า (codepoint >= 0x10000)
					str += (var_na)(0x80 | ((codepoint >> 12) & 0x3f));
				ถ้า (codepoint >= 0x800)
					str += (var_na)(0x80 | ((codepoint >>  6) & 0x3f));
				ถ้า (codepoint >= 0x80)
					str += (var_na)(0x80 | ((codepoint >>  0) & 0x3f));
				bha_nga;
			}

			pak_sa FmtPart::INTEGER:
			pak_sa FmtPart::STRING:
			pak_sa FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				ถ้า (part.type == FmtPart::INTEGER) {
					RTLIL::Const value = part.sig.as_const();
					bool has_x = false, all_x = true, has_z = false, all_z = true;
					สำหรับ (State bit : value) {
						ถ้า (bit == State::Sx)
							has_x = true;
						มิฉะนั้น
							all_x = false;
						ถ้า (bit == State::Sz)
							has_z = true;
						มิฉะนั้น
							all_z = false;
					}

					ถ้า (!has_z && !has_x && part.signed_ && value[value.size() - 1]) {
						prefix = "-";
						value = RTLIL::const_neg(value, {}, part.signed_, {}, value.size() + 1);
					} มิฉะนั้น {
						switch (part.sign) {
							pak_sa FmtPart::MINUS:       bha_nga;
							pak_sa FmtPart::PLUS_MINUS:  prefix = "+"; bha_nga;
							pak_sa FmtPart::SPACE_MINUS: prefix = " "; bha_nga;
						}
					}

					ถ้า (part.base != 10) {
						size_t minimum_size = 1;
						สำหรับ (size_t index = 0; index < (size_t)value.size(); index++)
							ถ้า (value[index] != State::S0)
								minimum_size = index + 1;
						value = value.extract(0, minimum_size);
					}

					ถ้า (part.base == 2) {
						ถ้า (part.show_base)
							prefix += "0b";
						สำหรับ (size_t index = 0; index < (size_t)value.size(); index++) {
							ถ้า (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = value[index];
							ถ้า (bit == State::Sx)
								buf += 'x';
							มิฉะนั้น ถ้า (bit == State::Sz)
								buf += 'z';
							มิฉะนั้น ถ้า (bit == State::S1)
								buf += '1';
							มิฉะนั้น /* if (bit == State::S0) */
								buf += '0';
						}
					} มิฉะนั้น ถ้า (part.base == 8 || part.base == 16) {
						ถ้า (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						สำหรับ (size_t index = 0; index < (size_t)value.size(); index += step) {
							ถ้า (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = value.extract(index, min(step, value.size() - index));
							bool has_x = false, all_x = true, has_z = false, all_z = true;
							สำหรับ (State bit : subvalue) {
								ถ้า (bit == State::Sx)
									has_x = true;
								มิฉะนั้น
									all_x = false;
								ถ้า (bit == State::Sz)
									has_z = true;
								มิฉะนั้น
									all_z = false;
							}
							ถ้า (all_x)
								buf += 'x';
							มิฉะนั้น ถ้า (all_z)
								buf += 'z';
							มิฉะนั้น ถ้า (has_x)
								buf += 'X';
							มิฉะนั้น ถ้า (has_z)
								buf += 'Z';
							มิฉะนั้น {
								const var_na *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} มิฉะนั้น ถ้า (part.base == 10) {
						ถ้า (part.show_base)
							prefix += "0d";
						ถ้า (all_x)
							buf += 'x';
						มิฉะนั้น ถ้า (all_z)
							buf += 'z';
						มิฉะนั้น ถ้า (has_x)
							buf += 'X';
						มิฉะนั้น ถ้า (has_z)
							buf += 'Z';
						มิฉะนั้น {
							log_assert(value.is_fully_def());
							ถ้า (value.is_fully_zero())
								buf += '0';
							size_t index = 0;
							ขณะ (!value.is_fully_zero())	{
								ถ้า (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(value, 10, false, false, 4).as_int();
								value = RTLIL::const_div(value, 10, false, false, value.size());
								index++;
							}
						}
					} มิฉะนั้น log_abort();
					ถ้า (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						ขณะ (prefix.size() + buf.size() < part.width) {
							ถ้า (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} มิฉะนั้น ถ้า (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} มิฉะนั้น ถ้า (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				ถ้า (prefix.size() + buf.size() < part.width) {
					size_t pad_width = part.width - prefix.size() - buf.size();
					switch (part.justify) {
						pak_sa FmtPart::LEFT:
							str += prefix;
							str += buf;
							str += std::string(pad_width, part.padding);
							bha_nga;
						pak_sa FmtPart::RIGHT:
							str += std::string(pad_width, part.padding);
							str += prefix;
							str += buf;
							bha_nga;
						pak_sa FmtPart::NUMERIC:
							str += prefix;
							str += std::string(pad_width, part.padding);
							str += buf;
							bha_nga;
					}
				} มิฉะนั้น {
					str += prefix;
					str += buf;
				}
				bha_nga;
			}
		}
	}

	คืน str;
}
