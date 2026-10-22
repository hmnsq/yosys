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

#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Verilog format argument, such as the arguments in:
//   $display("foo %d bar %01x", 4'b0, $signed(2'b11))
संरचना VerilogFmtArg {
	गणना {
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
संरचना FmtPart {
	गणना {
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
	गणना {
		दायाँ	= 0,
		बायाँ	= 1,
		NUMERIC	= 2,
	} justify = दायाँ;
	वर्ण padding = '\0';
	माप_प्रकार width = 0;

	// INTEGER type
	अचिन्हित base = 10;
	bool signed_ = false;
	गणना {
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

संरचना Fmt {
सार्वजनिक:
	std::vector<FmtPart> parts;

	शून्य append_literal(ध्रुव std::string &str);

	शून्य parse_rtlil(ध्रुव RTLIL::Cell *cell);
	शून्य emit_rtlil(RTLIL::Cell *cell) ध्रुव;

	शून्य parse_verilog(ध्रुव std::vector<VerilogFmtArg> &args, bool sformat_like, पूर्णाङ्क default_base, ध्रुव std::string &task_name, ध्रुव std::string &module_name);
	std::vector<VerilogFmtArg> emit_verilog() ध्रुव;

	शून्य emit_cxxrtl(std::ostream &os, std::string indent, std::function<शून्य(ध्रुव RTLIL::SigSpec &)> emit_sig, ध्रुव std::string &context) ध्रुव;

	std::string render() ध्रुव;

गुप्त:
	शून्य apply_verilog_automatic_sizing_and_add(FmtPart &part);
};

YOSYS_NAMESPACE_END

#endif
