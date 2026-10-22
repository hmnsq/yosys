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

#அந்தர்பாவ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Verilog format argument, such as the arguments in:
//   $display("foo %d bar %01x", 4'b0, $signed(2'b11))
கட்டமைப்பு VerilogFmtArg {
	எண்ணிக்கை {
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
கட்டமைப்பு FmtPart {
	எண்ணிக்கை {
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
	எண்ணிக்கை {
		வலது	= 0,
		இடது	= 1,
		NUMERIC	= 2,
	} justify = வலது;
	எழுத்து padding = '\0';
	size_t width = 0;

	// INTEGER type
	unsigned base = 10;
	bool signed_ = false;
	எண்ணிக்கை {
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

கட்டமைப்பு Fmt {
பொது:
	std::vector<FmtPart> parts;

	வெறுமை append_literal(மாறிலி std::string &str);

	வெறுமை parse_rtlil(மாறிலி RTLIL::Cell *cell);
	வெறுமை emit_rtlil(RTLIL::Cell *cell) மாறிலி;

	வெறுமை parse_verilog(மாறிலி std::vector<VerilogFmtArg> &args, bool sformat_like, முழு default_base, மாறிலி std::string &task_name, மாறிலி std::string &module_name);
	std::vector<VerilogFmtArg> emit_verilog() மாறிலி;

	வெறுமை emit_cxxrtl(std::ostream &os, std::string indent, std::function<வெறுமை(மாறிலி RTLIL::SigSpec &)> emit_sig, மாறிலி std::string &context) மாறிலி;

	std::string render() மாறிலி;

தனிப்பட்ட:
	வெறுமை apply_verilog_automatic_sizing_and_add(FmtPart &part);
};

YOSYS_NAMESPACE_END

#endif
