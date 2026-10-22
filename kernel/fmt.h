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

#اگر_غیر_تعریف_شدہ FMT_H
#تعریف_کرو FMT_H

#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Verilog format argument, such as the arguments in:
//   $display("foo %d bar %01x", 4'b0, $signed(2'b11))
ساخت VerilogFmtArg {
	شمار {
		STRING  = 0,
		INTEGER = 1,
		وقت    = 2,
	} type;

	// All types
	Log_ماخذ_Location src;

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
ساخت FmtPart {
	شمار {
		LITERAL  	= 0,
		INTEGER 	= 1,
		STRING    = 2,
		UNICHAR   = 3,
		VLOG_وقت = 4,
	} type;

	// LITERAL type
	std::string str;

	// INTEGER/STRING/UNICHAR types
	RTLIL::SigSpec sig;

	// INTEGER/STRING/VLOG_TIME types
	شمار {
		دایاں	= 0,
		بایاں	= 1,
		NUMERIC	= 2,
	} justify = دایاں;
	حرف padding = '\0';
	سائز_t چوڑائی = 0;

	// INTEGER type
	غیرعلامتی بنیاد = 10;
	bool signed_ = false;
	شمار {
		MINUS		= 0,
		PLUS_MINUS	= 1,
		SPACE_MINUS	= 2,
	} sign = MINUS;
	bool hex_بالائی = false;
	bool show_بنیاد = false;
	bool گروہ = false;

	// VLOG_TIME type
	bool realtime = false;
};

ساخت Fmt {
عوامی:
	std::سمتیہ<FmtPart> parts;

	خالی append_literal(ثابت std::string &str);

	خالی parse_rtlil(ثابت RTLIL::Cell *cell);
	خالی emit_rtlil(RTLIL::Cell *cell) ثابت;

	خالی parse_verilog(ثابت std::سمتیہ<VerilogFmtArg> &دلائل_2, bool sformat_like, صحیح default_بنیاد, ثابت std::string &task_name, ثابت std::string &module_name);
	std::سمتیہ<VerilogFmtArg> emit_verilog() ثابت;

	خالی emit_cxxrtl(std::ostream &os, std::string indent, std::function<خالی(ثابت RTLIL::SigSpec &)> emit_sig, ثابت std::string &context) ثابت;

	std::string render() ثابت;

نجی:
	خالی apply_verilog_automatic_sizing_and_جوڑو(FmtPart &part);
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
