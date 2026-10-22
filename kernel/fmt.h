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

#ifndef FMT_H
#define FMT_H

#അന്തര്ഭാവ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Verilog format argument, such as the arguments in:
//   $display("foo %d bar %01x", 4'b0, $signed(2'b11))
ഘടന VerilogFmtArg {
	എണ്ണൽ {
		STRING  = 0,
		INTEGER = 1,
		TIME    = 2,
	} type;

	// All types
	LogSourceLocation src;

	// STRING type
	std::string str;

	// INTEGER type
	RTLIL::SigSpec sig;
	bool signed_ = false;

	// TIME type
	bool realtime = false;
};

// RTLIL format part, such as the substitutions in:
//   "foo {4:> 4du} bar {2:<01hs}"
// Must be kept in sync with `struct fmt_part` in backends/cxxrtl/runtime/cxxrtl/cxxrtl.h!
ഘടന FmtPart {
	എണ്ണൽ {
		LITERAL  	= 0,
		INTEGER 	= 1,
		STRING    = 2,
		UNICHAR   = 3,
		VLOG_TIME = 4,
	} type;

	// LITERAL type
	std::string str;

	// INTEGER/STRING/UNICHAR types
	RTLIL::SigSpec sig;

	// INTEGER/STRING/VLOG_TIME types
	എണ്ണൽ {
		വലത്	= 0,
		ഇടത്	= 1,
		NUMERIC	= 2,
	} justify = വലത്;
	അക്ഷരം padding = '\0';
	size_t width = 0;

	// INTEGER type
	unsigned base = 10;
	bool signed_ = false;
	എണ്ണൽ {
		MINUS		= 0,
		PLUS_MINUS	= 1,
		SPACE_MINUS	= 2,
	} sign = MINUS;
	bool hex_upper = false;
	bool show_base = false;
	bool group = false;

	// VLOG_TIME type
	bool realtime = false;
};

ഘടന Fmt {
പൊതു:
	std::vector<FmtPart> parts;

	ശൂന്യം append_literal(സ്ഥിരം std::string &str);

	ശൂന്യം parse_rtlil(സ്ഥിരം RTLIL::Cell *cell);
	ശൂന്യം emit_rtlil(RTLIL::Cell *cell) സ്ഥിരം;

	ശൂന്യം parse_verilog(സ്ഥിരം std::vector<VerilogFmtArg> &args, bool sformat_like, പൂർണ്ണസംഖ്യ default_base, സ്ഥിരം std::string &task_name, സ്ഥിരം std::string &module_name);
	std::vector<VerilogFmtArg> emit_verilog() സ്ഥിരം;

	ശൂന്യം emit_cxxrtl(std::ostream &os, std::string indent, std::function<ശൂന്യം(സ്ഥിരം RTLIL::SigSpec &)> emit_sig, സ്ഥിരം std::string &context) സ്ഥിരം;

	std::string render() സ്ഥിരം;

സ്വകാര്യ:
	ശൂന്യം apply_verilog_automatic_sizing_and_add(FmtPart &part);
};

YOSYS_NAMESPACE_END

#endif
