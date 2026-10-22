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
		ᬬᬾᬦ᭄ (fmt.substr(i, 2) == "}}") {
			part.str += '}';
			++i;
		} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt.substr(i, 2) == "{{") {
			part.str += '{';
			++i;
		} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == '}') {
			log_assert(false && "Unexpected '}' in format string");
		} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == '{') {
			ᬬᬾᬦ᭄ (!part.str.empty()) {
				part.type = FmtPart::LITERAL;
				parts.push_back(part);
				part = {};
			}

			ᬬᬾᬦ᭄ (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			size_t arg_size = 0;
			for (; i < fmt.size(); i++) {
				ᬬᬾᬦ᭄ (fmt[i] >= '0' && fmt[i] <= '9') {
					arg_size *= 10;
					arg_size += fmt[i] - '0';
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == ':') {
					++i;
					bha_nga;
				} ᬮᬶᬬ {
					log_assert(false && "Unexpected character in format substitution");
				}
			}
			ᬬᬾᬦ᭄ (i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			ᬬᬾᬦ᭄ ((size_t)args.size() < arg_size)
				log_assert(false && "Format part overruns arguments");
			part.sig = args.extract(0, arg_size);
			args.remove(0, arg_size);

			ᬬᬾᬦ᭄ (fmt[i] == 'U') {
				part.type = FmtPart::UNICHAR;
				++i;
				goto success;
			}

			ᬬᬾᬦ᭄ (fmt[i] == '>')
				part.justify = FmtPart::RIGHT;
			ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == '<')
				part.justify = FmtPart::LEFT;
			ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == '=')
				part.justify = FmtPart::NUMERIC;
			ᬮᬶᬬ
				log_assert(false && "Unexpected justification in format substitution");
			ᬬᬾᬦ᭄ (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			part.padding = fmt[i];
			ᬬᬾᬦ᭄ (++i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			for (; i < fmt.size(); i++) {
				ᬬᬾᬦ᭄ (fmt[i] >= '0' && fmt[i] <= '9') {
					part.width *= 10;
					part.width += fmt[i] - '0';
					continue;
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 'b') {
					part.type = FmtPart::INTEGER;
					part.base = 2;
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 'o') {
					part.type = FmtPart::INTEGER;
					part.base = 8;
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 'd') {
					part.type = FmtPart::INTEGER;
					part.base = 10;
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 'h') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 'H') {
					part.type = FmtPart::INTEGER;
					part.base = 16;
					part.hex_upper = true;
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 'c') {
					part.type = FmtPart::STRING;
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 't') {
					part.type = FmtPart::VLOG_TIME;
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 'r') {
					part.type = FmtPart::VLOG_TIME;
					part.realtime = true;
				} ᬮᬶᬬ {
					log_assert(false && "Unexpected character in format substitution");
				}
				++i;
				bha_nga;
			}
			ᬬᬾᬦ᭄ (i == fmt.size())
				log_assert(false && "Unexpected end in format substitution");

			ᬬᬾᬦ᭄ (part.type == FmtPart::INTEGER) {
				ᬬᬾᬦ᭄ (fmt[i] == '-') {
					part.sign = FmtPart::MINUS;
					ᬬᬾᬦ᭄ (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == '+') {
					part.sign = FmtPart::PLUS_MINUS;
					ᬬᬾᬦ᭄ (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == ' ') {
					part.sign = FmtPart::SPACE_MINUS;
					ᬬᬾᬦ᭄ (++i == fmt.size())
						log_assert(false && "Unexpected end in format substitution");
				} ᬮᬶᬬ {
					// also accept no sign character and treat like MINUS for compatibility
				}

				ᬬᬾᬦ᭄ (fmt[i] == '#') {
					part.show_base = true;
					++i;
				}
				ᬬᬾᬦ᭄ (fmt[i] == '_') {
					part.group = true;
					++i;
				}

				ᬬᬾᬦ᭄ (fmt[i] == 'u')
					part.signed_ = false;
				ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 's')
					part.signed_ = true;
				ᬮᬶᬬ
					log_assert(false && "Unexpected character in format substitution");
				ᬬᬾᬦ᭄ (++i == fmt.size())
					log_assert(false && "Unexpected end in format substitution");
			}

		success:
			ᬬᬾᬦ᭄ (fmt[i] != '}')
				log_assert(false && "Expected '}' after format substitution");

			parts.push_back(part);
			part = {};
		} ᬮᬶᬬ {
			part.str += fmt[i];
		}
	}
	ᬬᬾᬦ᭄ (!part.str.empty()) {
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
					ᬬᬾᬦ᭄ (c == '{')
						fmt += "{{";
					ᬮᬶᬬ ᬬᬾᬦ᭄ (c == '}')
						fmt += "}}";
					ᬮᬶᬬ
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
				ᬬᬾᬦ᭄ (part.justify == FmtPart::RIGHT)
					fmt += '>';
				ᬮᬶᬬ ᬬᬾᬦ᭄ (part.justify == FmtPart::LEFT)
					fmt += '<';
				ᬮᬶᬬ ᬬᬾᬦ᭄ (part.justify == FmtPart::NUMERIC)
					fmt += '=';
				ᬮᬶᬬ log_abort();
				log_assert(part.width == 0 || part.padding != '\0');
				fmt += part.padding != '\0' ? part.padding : ' ';
				ᬬᬾᬦ᭄ (part.width > 0)
					fmt += std::to_string(part.width);
				ᬬᬾᬦ᭄ (part.type == FmtPart::INTEGER) {
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
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (part.type == FmtPart::STRING) {
					fmt += 'c';
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (part.type == FmtPart::VLOG_TIME) {
					ᬬᬾᬦ᭄ (part.realtime)
						fmt += 'r';
					ᬮᬶᬬ
						fmt += 't';
				} ᬮᬶᬬ log_abort();
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
	ᬬᬾᬦ᭄ (!signed_)
		max.setBit(size, true);
	ᬮᬶᬬ
		max.setBit(size - 1, true);
	size_t places = 0;
	ᬲᭂᬤᭂᬮ (!max.isZero()) {
		places++;
		max /= 10;
	}
	ᬬᬾᬦ᭄ (signed_)
		places++;
	ᬯᬮᬶ places;
}

static size_t compute_required_nondecimal_places(size_t size, unsigned base)
{
	log_assert(base != 10);
	BigUnsigned max;
	max.setBit(size - 1, true);
	size_t places = 0;
	ᬲᭂᬤᭂᬮ (!max.isZero()) {
		places++;
		max /= base;
	}
	ᬯᬮᬶ places;
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
	ᬬᬾᬦ᭄ (part.base == 10) {
		size_t places = compute_required_decimal_places(part.sig.size(), part.signed_);
		part.padding = ' ';
		part.width = std::max(part.width, places);
		parts.push_back(part);
		ᬯᬮᬶ;
	}

	part.padding = '0';

	size_t places = compute_required_nondecimal_places(part.sig.size(), part.base);
	ᬬᬾᬦ᭄ (part.width < places) {
		part.justify = FmtPart::RIGHT;
		part.width = places;
		parts.push_back(part);
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (part.width == places) {
		parts.push_back(part);
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (part.width > places) {
		auto gap = std::string(part.width - places, ' ');
		part.width = places;

		ᬬᬾᬦ᭄ (part.justify == FmtPart::RIGHT) {
			append_literal(gap);
			parts.push_back(part);
		} ᬮᬶᬬ {
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
				ᬬᬾᬦ᭄ (arg == args.begin() || !sformat_like) {
					const auto fmtarg = arg;
					const std::string &fmt = fmtarg->str;
					FmtPart part = {};
					for (size_t i = 0; i < fmt.size(); i++) {
						ᬬᬾᬦ᭄ (fmt[i] != '%') {
							part.str += fmt[i];
						} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt.substr(i, 2) == "%%") {
							i++;
							part.str += '%';
						} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt.substr(i, 2) == "%l" || fmt.substr(i, 2) == "%L") {
							i++;
							part.str += module_name;
						} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt.substr(i, 2) == "%m" || fmt.substr(i, 2) == "%M") {
							i++;
							part.str += module_name;
						} ᬮᬶᬬ {
							ᬬᬾᬦ᭄ (!part.str.empty()) {
								part.type = FmtPart::LITERAL;
								parts.push_back(part);
								part = {};
							}
							ᬬᬾᬦ᭄ (++i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ᬬᬾᬦ᭄ (++arg == args.end()) {
								log_file_error(fmtarg->src, "System task `%s' called with fewer arguments than the format specifiers in argument %zu require.\n", task_name, fmtarg - args.begin() + 1);
							}
							part.sig = arg->sig;
							part.signed_ = arg->signed_;

							for (; i < fmt.size(); i++) {
								ᬬᬾᬦ᭄ (fmt[i] == '-') {
									// left justify; not in IEEE 1800-2017 or verilator but iverilog has it
									part.justify = FmtPart::LEFT;
								} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == '+') {
									// always show sign; not in IEEE 1800-2017 or verilator but iverilog has it
									part.sign = FmtPart::PLUS_MINUS;
								} ᬮᬶᬬ bha_nga;
							}
							ᬬᬾᬦ᭄ (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							bool has_leading_zero = false, has_width = false;
							for (; i < fmt.size(); i++) {
								ᬬᬾᬦ᭄ (fmt[i] >= '0' && fmt[i] <= '9') {
									ᬬᬾᬦ᭄ (fmt[i] == '0' && !has_width) {
										has_leading_zero = true;
									} ᬮᬶᬬ {
										has_width = true;
										part.width *= 10;
										part.width += fmt[i] - '0';
									}
									continue;
								} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 'b' || fmt[i] == 'B') {
									part.type = FmtPart::INTEGER;
									part.base =  2;
								} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 'o' || fmt[i] == 'O') {
									part.type = FmtPart::INTEGER;
									part.base =  8;
								} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 'd' || fmt[i] == 'D') {
									part.type = FmtPart::INTEGER;
									part.base = 10;
								} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 'h' || fmt[i] == 'H' ||
								           fmt[i] == 'x' || fmt[i] == 'X') {
									// hex digits always printed in lowercase for %h%x as well as %H%X
									part.type = FmtPart::INTEGER;
									part.base = 16;
								} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 'c' || fmt[i] == 'C') {
									part.type = FmtPart::STRING;
									part.sig.extend_u0(8);
									// %10c and %010c not fully defined in IEEE 1800-2017 and do different things in iverilog
								} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 's' || fmt[i] == 'S') {
									part.type = FmtPart::STRING;
									ᬬᬾᬦ᭄ ((part.sig.size() % 8) != 0)
										part.sig.extend_u0((part.sig.size() + 7) / 8 * 8);
									// %10s and %010s not fully defined in IEEE 1800-2017 and do the same thing in iverilog
									part.padding = ' ';
								} ᬮᬶᬬ ᬬᬾᬦ᭄ (fmt[i] == 't' || fmt[i] == 'T') {
									ᬬᬾᬦ᭄ (arg->type == VerilogFmtArg::TIME) {
										part.type = FmtPart::VLOG_TIME;
										part.realtime = arg->realtime;
										ᬬᬾᬦ᭄ (!has_width && !has_leading_zero)
											part.width = 20;
									} ᬮᬶᬬ {
										log_file_error(fmtarg->src, "System task `%s' called with format character `%c' in argument %zu, but the argument is not $time or $realtime.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
									}
								} ᬮᬶᬬ {
									log_file_error(fmtarg->src, "System task `%s' called with unrecognized format character `%c' in argument %zu.\n", task_name, fmt[i], fmtarg - args.begin() + 1);
								}
								bha_nga;
							}
							ᬬᬾᬦ᭄ (i == fmt.size()) {
								log_file_error(fmtarg->src, "System task `%s' called with incomplete format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);
							}

							ᬬᬾᬦ᭄ (part.padding == '\0') {
								ᬬᬾᬦ᭄ (has_leading_zero && part.justify == FmtPart::RIGHT) {
									part.padding = '0';
									part.justify = FmtPart::NUMERIC;
								} ᬮᬶᬬ {
									part.padding = ' ';
								}
							}

							ᬬᬾᬦ᭄ (part.type == FmtPart::INTEGER && part.base != 10 && part.sign != FmtPart::MINUS)
								log_file_error(fmtarg->src, "System task `%s' called with invalid format specifier in argument %zu.\n", task_name, fmtarg - args.begin() + 1);

							ᬬᬾᬦ᭄ (part.base != 10)
								part.signed_ = false;
							ᬬᬾᬦ᭄ (part.type == FmtPart::INTEGER && !has_leading_zero)
								apply_verilog_automatic_sizing_and_add(part);
							ᬮᬶᬬ
								parts.push_back(part);
							part = {};
						}
					}
					ᬬᬾᬦ᭄ (!part.str.empty()) {
						part.type = FmtPart::LITERAL;
						parts.push_back(part);
					}
				} ᬮᬶᬬ {
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
					ᬬᬾᬦ᭄ (c == '%')
						fmt.str += "%%";
					ᬮᬶᬬ
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
				ᬬᬾᬦ᭄ (part.sign == FmtPart::PLUS_MINUS || part.sign == FmtPart::SPACE_MINUS)
					fmt.str += '+'; // treat space/minus as plus/minus
				ᬬᬾᬦ᭄ (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				ᬬᬾᬦ᭄ (part.width == 0) {
					fmt.str += '0';
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (part.width > 0) {
					ᬬᬾᬦ᭄ (part.base != 10 || part.padding == '0')
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
				ᬬᬾᬦ᭄ (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				ᬬᬾᬦ᭄ (part.sig.size() == 8) {
					ᬬᬾᬦ᭄ (part.width > 0) {
						ᬬᬾᬦ᭄ (part.padding == '0')
							fmt.str += part.padding;
						fmt.str += std::to_string(part.width);
					}
					fmt.str += 'c';
				} ᬮᬶᬬ {
					log_assert(part.sig.size() % 8 == 0);
					ᬬᬾᬦ᭄ (part.width > 0) {
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
				ᬬᬾᬦ᭄ (part.realtime)
					arg.realtime = true;
				args.push_back(arg);

				fmt.str += '%';
				log_assert(part.sign == FmtPart::MINUS || part.sign == FmtPart::PLUS_MINUS);
				ᬬᬾᬦ᭄ (part.sign == FmtPart::PLUS_MINUS)
					fmt.str += '+';
				ᬬᬾᬦ᭄ (part.justify == FmtPart::LEFT)
					fmt.str += '-';
				ᬬᬾᬦ᭄ (part.padding == '0' && part.width > 0)
					fmt.str += '0';
				fmt.str += std::to_string(part.width);
				fmt.str += 't';
				bha_nga;
			}

			default: log_abort();
		}
	}

	args.insert(args.begin(), fmt);
	ᬯᬮᬶ args;
}

std::string escape_cxx_string(const std::string &input)
{
	std::string output = "\"";
	for (auto c : input) {
		ᬬᬾᬦ᭄ (::isprint(c)) {
			ᬬᬾᬦ᭄ (c == '\\' || c == '"')
				output.push_back('\\');
			output.push_back(c);
		} ᬮᬶᬬ {
			var_na l = c & 0x7, m = (c >> 3) & 0x7, h = (c >> 6) & 0x3;
			output.push_back('\\');
			output.push_back('0' + h);
			output.push_back('0' + m);
			output.push_back('0' + l);
		}
	}
	output.push_back('"');
	ᬬᬾᬦ᭄ (output.find('\0') != std::string::npos) {
		output.insert(0, "std::string {");
		output.append(stringf(", %zu}", input.size()));
	}
	ᬯᬮᬶ output;
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
				ᬬᬾᬦ᭄ (codepoint >= 0x10000)
					str += (var_na)(0xf0 |  (codepoint >> 18));
				ᬮᬶᬬ ᬬᬾᬦ᭄ (codepoint >= 0x800)
					str += (var_na)(0xe0 |  (codepoint >> 12));
				ᬮᬶᬬ ᬬᬾᬦ᭄ (codepoint >= 0x80)
					str += (var_na)(0xc0 |  (codepoint >>  6));
				ᬮᬶᬬ
					str += (var_na)codepoint;
				ᬬᬾᬦ᭄ (codepoint >= 0x10000)
					str += (var_na)(0x80 | ((codepoint >> 12) & 0x3f));
				ᬬᬾᬦ᭄ (codepoint >= 0x800)
					str += (var_na)(0x80 | ((codepoint >>  6) & 0x3f));
				ᬬᬾᬦ᭄ (codepoint >= 0x80)
					str += (var_na)(0x80 | ((codepoint >>  0) & 0x3f));
				bha_nga;
			}

			pak_sa FmtPart::INTEGER:
			pak_sa FmtPart::STRING:
			pak_sa FmtPart::VLOG_TIME: {
				std::string buf;
				std::string prefix;
				ᬬᬾᬦ᭄ (part.type == FmtPart::INTEGER) {
					RTLIL::Const value = part.sig.as_const();
					bool has_x = false, all_x = true, has_z = false, all_z = true;
					for (State bit : value) {
						ᬬᬾᬦ᭄ (bit == State::Sx)
							has_x = true;
						ᬮᬶᬬ
							all_x = false;
						ᬬᬾᬦ᭄ (bit == State::Sz)
							has_z = true;
						ᬮᬶᬬ
							all_z = false;
					}

					ᬬᬾᬦ᭄ (!has_z && !has_x && part.signed_ && value[value.size() - 1]) {
						prefix = "-";
						value = RTLIL::const_neg(value, {}, part.signed_, {}, value.size() + 1);
					} ᬮᬶᬬ {
						switch (part.sign) {
							pak_sa FmtPart::MINUS:       bha_nga;
							pak_sa FmtPart::PLUS_MINUS:  prefix = "+"; bha_nga;
							pak_sa FmtPart::SPACE_MINUS: prefix = " "; bha_nga;
						}
					}

					ᬬᬾᬦ᭄ (part.base != 10) {
						size_t minimum_size = 1;
						for (size_t index = 0; index < (size_t)value.size(); index++)
							ᬬᬾᬦ᭄ (value[index] != State::S0)
								minimum_size = index + 1;
						value = value.extract(0, minimum_size);
					}

					ᬬᬾᬦ᭄ (part.base == 2) {
						ᬬᬾᬦ᭄ (part.show_base)
							prefix += "0b";
						for (size_t index = 0; index < (size_t)value.size(); index++) {
							ᬬᬾᬦ᭄ (part.group && index > 0 && index % 4 == 0)
								buf += '_';
							RTLIL::State bit = value[index];
							ᬬᬾᬦ᭄ (bit == State::Sx)
								buf += 'x';
							ᬮᬶᬬ ᬬᬾᬦ᭄ (bit == State::Sz)
								buf += 'z';
							ᬮᬶᬬ ᬬᬾᬦ᭄ (bit == State::S1)
								buf += '1';
							ᬮᬶᬬ /* if (bit == State::S0) */
								buf += '0';
						}
					} ᬮᬶᬬ ᬬᬾᬦ᭄ (part.base == 8 || part.base == 16) {
						ᬬᬾᬦ᭄ (part.show_base)
							prefix += (part.base == 16) ? (part.hex_upper ? "0X" : "0x") : "0o";
						size_t step = (part.base == 16) ? 4 : 3;
						for (size_t index = 0; index < (size_t)value.size(); index += step) {
							ᬬᬾᬦ᭄ (part.group && index > 0 && index % (4 * step) == 0)
								buf += '_';
							RTLIL::Const subvalue = value.extract(index, min(step, value.size() - index));
							bool has_x = false, all_x = true, has_z = false, all_z = true;
							for (State bit : subvalue) {
								ᬬᬾᬦ᭄ (bit == State::Sx)
									has_x = true;
								ᬮᬶᬬ
									all_x = false;
								ᬬᬾᬦ᭄ (bit == State::Sz)
									has_z = true;
								ᬮᬶᬬ
									all_z = false;
							}
							ᬬᬾᬦ᭄ (all_x)
								buf += 'x';
							ᬮᬶᬬ ᬬᬾᬦ᭄ (all_z)
								buf += 'z';
							ᬮᬶᬬ ᬬᬾᬦ᭄ (has_x)
								buf += 'X';
							ᬮᬶᬬ ᬬᬾᬦ᭄ (has_z)
								buf += 'Z';
							ᬮᬶᬬ {
								const var_na *digits = part.hex_upper ? "0123456789ABCDEF" : "0123456789abcdef";
								buf += digits[subvalue.as_int()];
							}
						}
					} ᬮᬶᬬ ᬬᬾᬦ᭄ (part.base == 10) {
						ᬬᬾᬦ᭄ (part.show_base)
							prefix += "0d";
						ᬬᬾᬦ᭄ (all_x)
							buf += 'x';
						ᬮᬶᬬ ᬬᬾᬦ᭄ (all_z)
							buf += 'z';
						ᬮᬶᬬ ᬬᬾᬦ᭄ (has_x)
							buf += 'X';
						ᬮᬶᬬ ᬬᬾᬦ᭄ (has_z)
							buf += 'Z';
						ᬮᬶᬬ {
							log_assert(value.is_fully_def());
							ᬬᬾᬦ᭄ (value.is_fully_zero())
								buf += '0';
							size_t index = 0;
							ᬲᭂᬤᭂᬮ (!value.is_fully_zero())	{
								ᬬᬾᬦ᭄ (part.group && index > 0 && index % 3 == 0)
									buf += '_';
								buf += '0' + RTLIL::const_mod(value, 10, false, false, 4).as_int();
								value = RTLIL::const_div(value, 10, false, false, value.size());
								index++;
							}
						}
					} ᬮᬶᬬ log_abort();
					ᬬᬾᬦ᭄ (part.justify == FmtPart::NUMERIC && part.group && part.padding == '0') {
						size_t group_size = part.base == 10 ? 3 : 4;
						ᬲᭂᬤᭂᬮ (prefix.size() + buf.size() < part.width) {
							ᬬᬾᬦ᭄ (buf.size() % (group_size + 1) == group_size)
								buf += '_';
							buf += '0';
						}
					}
					std::reverse(buf.begin(), buf.end());
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (part.type == FmtPart::STRING) {
					buf = part.sig.as_const().decode_string();
				} ᬮᬶᬬ ᬬᬾᬦ᭄ (part.type == FmtPart::VLOG_TIME) {
					// We only render() during initial, so time is always zero.
					buf = "0";
				}

				log_assert(part.width == 0 || part.padding != '\0');
				ᬬᬾᬦ᭄ (prefix.size() + buf.size() < part.width) {
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
				} ᬮᬶᬬ {
					str += prefix;
					str += buf;
				}
				bha_nga;
			}
		}
	}

	ᬯᬮᬶ str;
}
