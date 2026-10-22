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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FMT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FMT_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Verilog format argument, such as the arguments in:
//   $display("foo %d bar %01x", 4'b0, $signed(2'b11))
ਬਣਤਰ VerilogFmtArg {
	ਗਿਣਤੀ {
		STRING  = 0,
		INTEGER = 1,
		ਸਮਾਂ    = 2,
	} type;

	// All types
	Log_ਸਰੋਤ_Location src;

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
ਬਣਤਰ FmtPart {
	ਗਿਣਤੀ {
		LITERAL  	= 0,
		INTEGER 	= 1,
		STRING    = 2,
		UNICHAR   = 3,
		VLOG_ਸਮਾਂ = 4,
	} type;

	// LITERAL type
	std::string str;

	// INTEGER/STRING/UNICHAR types
	RTLIL::SigSpec sig;

	// INTEGER/STRING/VLOG_TIME types
	ਗਿਣਤੀ {
		ਸੱਜਾ	= 0,
		ਖੱਬਾ	= 1,
		NUMERIC	= 2,
	} justify = ਸੱਜਾ;
	ਅੱਖਰ padding = '\0';
	ਮਾਪ_t ਚੌੜਾਈ = 0;

	// INTEGER type
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ = 10;
	bool signed_ = false;
	ਗਿਣਤੀ {
		MINUS		= 0,
		PLUS_MINUS	= 1,
		SPACE_MINUS	= 2,
	} sign = MINUS;
	bool hex_ਉੱਤਲਾ = false;
	bool show_ਅਧਾਰ = false;
	bool ਸਮੂਹ = false;

	// VLOG_TIME type
	bool realtime = false;
};

ਬਣਤਰ Fmt {
ਜਨਤਕ:
	std::ਸਦਿਸ਼<FmtPart> parts;

	ਖਾਲੀ append_literal(ਅਚਲ std::string &str);

	ਖਾਲੀ parse_rtlil(ਅਚਲ RTLIL::Cell *cell);
	ਖਾਲੀ emit_rtlil(RTLIL::Cell *cell) ਅਚਲ;

	ਖਾਲੀ parse_verilog(ਅਚਲ std::ਸਦਿਸ਼<VerilogFmtArg> &ਦਲੀਲਾਂ_2, bool sformat_like, ਪੂਰਨ default_ਅਧਾਰ, ਅਚਲ std::string &task_name, ਅਚਲ std::string &module_name);
	std::ਸਦਿਸ਼<VerilogFmtArg> emit_verilog() ਅਚਲ;

	ਖਾਲੀ emit_cxxrtl(std::ostream &os, std::string indent, std::function<ਖਾਲੀ(ਅਚਲ RTLIL::SigSpec &)> emit_sig, ਅਚਲ std::string &context) ਅਚਲ;

	std::string render() ਅਚਲ;

ਨਿਜੀ:
	ਖਾਲੀ apply_verilog_automatic_sizing_and_ਜੋੜੋ(FmtPart &part);
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
