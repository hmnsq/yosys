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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Verilog format argument, such as the arguments in:
//   $display("foo %d bar %01x", 4'b0, $signed(2'b11))
ସଂରଚନା VerilogFmtArg {
	ଗଣନା {
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
ସଂରଚନା FmtPart {
	ଗଣନା {
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
	ଗଣନା {
		ଡାହାଣ	= 0,
		ବାମ	= 1,
		NUMERIC	= 2,
	} justify = ଡାହାଣ;
	ଅକ୍ଷର padding = '\0';
	size_t width = 0;

	// INTEGER type
	unsigned base = 10;
	bool signed_ = false;
	ଗଣନା {
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

ସଂରଚନା Fmt {
ସର୍ବସାଧାରଣ:
	std::vector<FmtPart> parts;

	ଶୂନ୍ୟ append_literal(ଅଚଳ std::string &str);

	ଶୂନ୍ୟ parse_rtlil(ଅଚଳ RTLIL::Cell *cell);
	ଶୂନ୍ୟ emit_rtlil(RTLIL::Cell *cell) ଅଚଳ;

	ଶୂନ୍ୟ parse_verilog(ଅଚଳ std::vector<VerilogFmtArg> &args, bool sformat_like, ପୂର୍ଣ୍ଣସଂଖ୍ୟା default_base, ଅଚଳ std::string &task_name, ଅଚଳ std::string &module_name);
	std::vector<VerilogFmtArg> emit_verilog() ଅଚଳ;

	ଶୂନ୍ୟ emit_cxxrtl(std::ostream &os, std::string indent, std::function<ଶୂନ୍ୟ(ଅଚଳ RTLIL::SigSpec &)> emit_sig, ଅଚଳ std::string &context) ଅଚଳ;

	std::string render() ଅଚଳ;

ବ୍ୟକ୍ତିଗତ:
	ଶୂନ୍ୟ apply_verilog_automatic_sizing_and_add(FmtPart &part);
};

YOSYS_NAMESPACE_END

#endif
