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
	for (size_t i = 0; i < fmt.size(); i++) {
		ᱡᱩᱫᱤ (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == '}') {
			log_assert(false && "Unexpected '}' in format string");
		} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == '{') {
			ᱡᱩᱫᱤ (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.push_back(part);
				part = {};
			}

			ᱡᱩᱫᱤ (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			size_t arg_size = 0;
			for (; i < fmt.size(); i++) {
				ᱡᱩᱫᱤ (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == ':') {
					++i;
					bha_nga;
				} ᱵᱟᱝ {
					log_assert(false && "Unexpected character in format substitution");
				}
			}
			ᱡᱩᱫᱤ (i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			ᱡᱩᱫᱤ ((size_t)args.size() < arg_size)
				log_assert(false && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			ᱡᱩᱫᱤ (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				goto success;
			}

			ᱡᱩᱫᱤ (fmt[i] == '>')
				part.justify = FmtPart::RIGHT;
			ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == '<')
				part.justify = FmtPart::LEFT;
			ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			ᱵᱟᱝ
				log_assert(false && "Unexpected justification in format substitution");
			ᱡᱩᱫᱤ (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			part.padding = fmt[i];
			ᱡᱩᱫᱤ (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			for (; i < fmt.size(); i++) {
				ᱡᱩᱫᱤ (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					continue;
				} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = true;
				} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = true;
				} ᱵᱟᱝ {
					log_assert(false && "Unexpected character in format substitution");
				}
				++i;
				bha_nga;
			}
			ᱡᱩᱫᱤ (i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			ᱡᱩᱫᱤ (part.type == FmtPart::INTEGER) {
				ᱡᱩᱫᱤ (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					ᱡᱩᱫᱤ (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					ᱡᱩᱫᱤ (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					ᱡᱩᱫᱤ (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} ᱵᱟᱝ {
					// also accept no sign character and treat like MINUS for compatibility
				}

				ᱡᱩᱫᱤ (fmt[i] == '#') {
					part.show_base = true;
					++i;
				}
				ᱡᱩᱫᱤ (fmt[i] == '_') {
					part.group = true;
					++i;
				}

				ᱡᱩᱫᱤ (fmt[i] == 'u')
					part.signed_ = false;
				ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 's')
					part.signed_ = true;
				ᱵᱟᱝ
					log_assert(false && "Unexpected character in format substitution");
				ᱡᱩᱫᱤ (++i == fmt.size())
					log_assert(false && "Unexpected end in format substitution");
			}

		success:
			ᱡᱩᱫᱤ (fmt[i] != '}')
				log_assert(false && "Expected '}' after format substitution");

			parts.push_back(part);
			part = {};
		} ᱵᱟᱝ {
			part.str += fmt[i];
		}
	}
	ᱡᱩᱫᱤ (!part.str.empty()) {
		part.type = FmtPart::LITERAL;
		parts.push_back(part);
	}
}

void Fmt::emit_rtlil(RTLIL::Cell *cell) const {
	std::string fmt;
	RTLIL::SigSpec args;

	for (auto &part : parts) {
		switch (part.type) {
			pak_sa FmtPart::LITERAL:
				for (var_na c : part.str) {
					ᱡᱩᱫᱤ (c == '{')
						fmt += "{{";
					ᱵᱟᱝ ᱡᱩᱫᱤ (c == '}')
						fmt += "}}";
					ᱵᱟᱝ
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
				ᱡᱩᱫᱤ (part.justify == FmtPart::RIGHT)
					fmt += '>';
				ᱵᱟᱝ ᱡᱩᱫᱤ (part.justify == FmtPart::LEFT)
					fmt += '<';
				ᱵᱟᱝ ᱡᱩᱫᱤ (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				ᱵᱟᱝ log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				ᱡᱩᱫᱤ (part.width > 0)
					fmt += std::to_string(part.width);
				ᱡᱩᱫᱤ (part.type == FmtPart::INTEGER) {
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
				} ᱵᱟᱝ ᱡᱩᱫᱤ (part.type == FmtPart::STRING) {
					fmt += 'c';
				} ᱵᱟᱝ ᱡᱩᱫᱤ (part.type == FmtPart::VLOG_TIME) {
					ᱡᱩᱫᱤ (part.realtime)
						fmt += 'r';
					ᱵᱟᱝ
						fmt += 't';
				} ᱵᱟᱝ log_abort();
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
	ᱡᱩᱫᱤ (!signed_)
		max.setBit(size, true);
	ᱵᱟᱝ
		max.setBit(size - 1, true);
	size_t places = 0;
	ᱡᱚᱠᱷᱚᱱ (!max.isZero()) {
		places++;
		max /= 10;
	}
	ᱡᱩᱫᱤ (signed_)
		places++;
	ᱨᱩᱣᱟᱹᱲ places;
}

static size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, true);
	size_t places = 0;
	ᱡᱚᱠᱷᱚᱱ (!max.isZero()) {
		places++;
		max /= base;
	}
	ᱨᱩᱣᱟᱹᱲ places;
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
	ᱡᱩᱫᱤ (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.push_back(part);
		ᱨᱩᱣᱟᱹᱲ;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	ᱡᱩᱫᱤ (part.width < places) {
		part.justify = FmtPart::RIGHT;
		part.width = places;
		parts.push_back(part);
	} ᱵᱟᱝ ᱡᱩᱫᱤ (part.width == places) {
		parts.push_back(part);
	} ᱵᱟᱝ ᱡᱩᱫᱤ (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		ᱡᱩᱫᱤ (part.justify == FmtPart::RIGHT) {
			append_literal(gap);
			parts.push_back(part);
		} ᱵᱟᱝ {
			part.justify = FmtPart::RIGHT;
			parts.push_back(part);
			append_literal(gap);
		}
	}
}

void Fmt::parse_verilog(const std::vector<VerilogFmtArg> &args, bool sformat_like, puur_naa_nka default_base, const std::string &task_name, const std::string &module_name)
{
	parts.clear();

	auto arg = args.begin();
	for (; arg != args.end(); ++arg) {
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
				ᱡᱩᱫᱤ (arg == args.begin() || !sformat_like) {
					const auto fmtarg = arg;
					const std::string &fmt = fmtarg->str;
					FmtPart part = {};
					for (size_t i = 0; i < fmt.size(); i++) {
						ᱡᱩᱫᱤ (fmt[i] != '%') {
							part.str += fmt[i];
						} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} ᱵᱟᱝ {
							ᱡᱩᱫᱤ (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.push_back(part);
								part = {};
							}
							ᱡᱩᱫᱤ (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ᱡᱩᱫᱤ (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							for (; i < fmt.size(); i++) {
								ᱡᱩᱫᱤ (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::LEFT;
								} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} ᱵᱟᱝ bha_nga;
							}
							ᱡᱩᱫᱤ (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							bool has_leading_zero = false, has_width = false;
							for (; i < fmt.size(); i++) {
								ᱡᱩᱫᱤ (fmt[i] >= '0' && fmt[i] <= '9') {
									ᱡᱩᱫᱤ (fmt[i] == '0' && !has_width) {
										has_leading_zero = true;
									} ᱵᱟᱝ {
										has_width = true;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									continue;
								} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									ᱡᱩᱫᱤ ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} ᱵᱟᱝ ᱡᱩᱫᱤ (fmt[i] == 't' || fmt[i] == 'T') {
									ᱡᱩᱫᱤ (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										ᱡᱩᱫᱤ (!has_width && !has_leading_zero)
											part.width = 20;
									} ᱵᱟᱝ {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} ᱵᱟᱝ {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								bha_nga;
							}
							ᱡᱩᱫᱤ (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ᱡᱩᱫᱤ (part.padding == '\0') {
								ᱡᱩᱫᱤ (has_leading_zero && part.justify == FmtPart::RIGHT) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} ᱵᱟᱝ {
									part.padding = ' ';
								}
							}

							ᱡᱩᱫᱤ (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							ᱡᱩᱫᱤ (part.base != 10)
								part.signed_ = false;
							ᱡᱩᱫᱤ (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							ᱵᱟᱝ
								parts.push_back(part);
							part = {};
						}
					}
					ᱡᱩᱫᱤ (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.push_back(part);
					}
				} ᱵᱟᱝ {
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

	for (auto &part : parts) {
		switch (part.type) {
			pak_sa FmtPart::LITERAL:
				for (var_na c : part.str) {
					ᱡᱩᱫᱤ (c == '%')
						fmt.str += "%%";
					ᱵᱟᱝ
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
				ᱡᱩᱫᱤ (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				ᱡᱩᱫᱤ (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				ᱡᱩᱫᱤ (part.width == 0) {
					fmt.str += '0';
				} ᱵᱟᱝ ᱡᱩᱫᱤ (part.width > 0) {
					ᱡᱩᱫᱤ (part.base != 10 || part.padding == '0')
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
				ᱡᱩᱫᱤ (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				ᱡᱩᱫᱤ (part.sig.size() == 8) {
					ᱡᱩᱫᱤ (part.width > 0) {
						ᱡᱩᱫᱤ (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} ᱵᱟᱝ {
					log_assert(part.sig.size() % 8 == 0);
					ᱡᱩᱫᱤ (part.width > 0) {
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
				ᱡᱩᱫᱤ (part.realtime)
					arg.realtime = true;
				args.push_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				ᱡᱩᱫᱤ (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				ᱡᱩᱫᱤ (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				ᱡᱩᱫᱤ (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				bha_nga;
			}

			default: log_abort();
		}
	}

	args.insert(args.begin(), fmt);
	ᱨᱩᱣᱟᱹᱲ args;
}

std::string escape_cxx_string(const std::string &input)
{
	std::string output = "\"";
	for (auto c : input) {
		ᱡᱩᱫᱤ (::isprint(c)) {
			ᱡᱩᱫᱤ (c == '\\' || c == '"')
				output.push_back('\\');
			output.push_back(c);
		} ᱵᱟᱝ {
			var_na l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.push_back('\\');
			output.push_back('0' + h);
			output.push_back('0' + m);
			output.push_back('0' + l);
		}
	}
	output.push_back('"');
	ᱡᱩᱫᱤ (output.find('\0') != std::string::npos) {
		output.insert(0, "std::string {");
		output.append(stringf(", %zu}", input.size()));
	}
	ᱨᱩᱣᱟᱹᱲ output;
}

void Fmt::emit_cxxrtl(std::ostream &os, std::string indent, std::function<void(const RTLIL::SigSpec &)> emit_sig, const std::string &context) const
{
	os << indent << "std::string buf;\n";
	for (auto &part : parts) {
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
		os << "(char)" << (puur_naa_nka)part.padding << ", ";
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

	for (auto &part : parts) {
		switch (part.type) {
			pak_sa FmtPart::LITERAL:
				str += part.str;
				bha_nga;

			pak_sa FmtPart::UNICHAR: {
				RTLIL::Const value = part.sig.as_const();
				uint32_t codepoint = value.as_int();
				ᱡᱩᱫᱤ (codepoint >= 0x10000)
					str += (var_na)(0xf0 |  (codepoint >> 18));
				ᱵᱟᱝ ᱡᱩᱫᱤ (codepoint >= 0x800)
					str += (var_na)(0xe0 |  (codepoint >> 12));
				ᱵᱟᱝ ᱡᱩᱫᱤ (codepoint >= 0x80)
					str += (var_na)(0xc0 |  (codepoint >>  6));
				ᱵᱟᱝ
					str += (var_na)codepoint;
				ᱡᱩᱫᱤ (codepoint >= 0x10000)
					str += (var_na)(0x80 | ((codepoint >> 12) & 0x3f));
				ᱡᱩᱫᱤ (codepoint >= 0x800)
					str += (var_na)(0x80 | ((codepoint >>  6) & 0x3f));
				ᱡᱩᱫᱤ (codepoint >= 0x80)
					str += (var_na)(0x80 | ((codepoint >>  0) & 0x3f));
				bha_nga;
			}

			pak_sa FmtPart::INTEGER:
			pak_sa FmtPart::STRING:
			pak_sa FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				ᱡᱩᱫᱤ (part.type == FmtPart::INTEGER) {
					RTLIL::Const value = part.sig.as_const();
					bool has_x = false, all_x = true, has_z = false, all_z = true;
					for (State bit : value) {
						ᱡᱩᱫᱤ (bit == State::Sx)
							has_x = true;
						ᱵᱟᱝ
							all_x = false;
						ᱡᱩᱫᱤ (bit == State::Sz)
							has_z = true;
						ᱵᱟᱝ
							all_z = false;
					}

					ᱡᱩᱫᱤ (!has_z && !has_x && part.signed_ && value[value.size() - 1]) {
						prefix = "-";
						value = RTLIL::const_neg(value, {}, part.signed_, {}, value.size() + 1);
					} ᱵᱟᱝ {
						switch (part.sign) {
							pak_sa FmtPart::MINUS:       bha_nga;
							pak_sa FmtPart::PLUS_MINUS:  prefix = "+"; bha_nga;
							pak_sa FmtPart::SPACE_MINUS: prefix = " "; bha_nga;
						}
					}

					ᱡᱩᱫᱤ (part.base != 10) {
						size_t minimum_size = 1;
						for (size_t index = 0; index < (size_t)value.size(); index++)
							ᱡᱩᱫᱤ (value[index] != State::S0)
								minimum_size = index + 1;
						value = value.extract(0, minimum_size);
					}

					ᱡᱩᱫᱤ (part.base == 2) {
						ᱡᱩᱫᱤ (part.show_base)
							prefix += "0b";
						for (size_t index = 0; index < (size_t)value.size(); index++) {
							ᱡᱩᱫᱤ (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = value[index];
							ᱡᱩᱫᱤ (bit == State::Sx)
								buf += 'x';
							ᱵᱟᱝ ᱡᱩᱫᱤ (bit == State::Sz)
								buf += 'z';
							ᱵᱟᱝ ᱡᱩᱫᱤ (bit == State::S1)
								buf += '1';
							ᱵᱟᱝ /* if (bit == State::S0) */
								buf += '0';
						}
					} ᱵᱟᱝ ᱡᱩᱫᱤ (part.base == 8 || part.base == 16) {
						ᱡᱩᱫᱤ (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						for (size_t index = 0; index < (size_t)value.size(); index += step) {
							ᱡᱩᱫᱤ (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = value.extract(index, min(step, value.size() - index));
							bool has_x = false, all_x = true, has_z = false, all_z = true;
							for (State bit : subvalue) {
								ᱡᱩᱫᱤ (bit == State::Sx)
									has_x = true;
								ᱵᱟᱝ
									all_x = false;
								ᱡᱩᱫᱤ (bit == State::Sz)
									has_z = true;
								ᱵᱟᱝ
									all_z = false;
							}
							ᱡᱩᱫᱤ (all_x)
								buf += 'x';
							ᱵᱟᱝ ᱡᱩᱫᱤ (all_z)
								buf += 'z';
							ᱵᱟᱝ ᱡᱩᱫᱤ (has_x)
								buf += 'X';
							ᱵᱟᱝ ᱡᱩᱫᱤ (has_z)
								buf += 'Z';
							ᱵᱟᱝ {
								const var_na *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} ᱵᱟᱝ ᱡᱩᱫᱤ (part.base == 10) {
						ᱡᱩᱫᱤ (part.show_base)
							prefix += "0d";
						ᱡᱩᱫᱤ (all_x)
							buf += 'x';
						ᱵᱟᱝ ᱡᱩᱫᱤ (all_z)
							buf += 'z';
						ᱵᱟᱝ ᱡᱩᱫᱤ (has_x)
							buf += 'X';
						ᱵᱟᱝ ᱡᱩᱫᱤ (has_z)
							buf += 'Z';
						ᱵᱟᱝ {
							log_assert(value.is_fully_def());
							ᱡᱩᱫᱤ (value.is_fully_zero())
								buf += '0';
							size_t index = 0;
							ᱡᱚᱠᱷᱚᱱ (!value.is_fully_zero())	{
								ᱡᱩᱫᱤ (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(value, 10, false, false, 4).as_int();
								value = RTLIL::const_div(value, 10, false, false, value.size());
								index++;
							}
						}
					} ᱵᱟᱝ log_abort();
					ᱡᱩᱫᱤ (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						ᱡᱚᱠᱷᱚᱱ (prefix.size() + buf.size() < part.width) {
							ᱡᱩᱫᱤ (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} ᱵᱟᱝ ᱡᱩᱫᱤ (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} ᱵᱟᱝ ᱡᱩᱫᱤ (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				ᱡᱩᱫᱤ (prefix.size() + buf.size() < part.width) {
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
				} ᱵᱟᱝ {
					str += prefix;
					str += buf;
				}
				bha_nga;
			}
		}
	}

	ᱨᱩᱣᱟᱹᱲ str;
}
