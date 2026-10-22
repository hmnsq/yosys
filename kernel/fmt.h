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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FMT_H
#ವ್ಯಾಖ್ಯಾನಿಸು FMT_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

// Verilog format argument, such as the arguments in:
//   $display("foo %d bar %01x", 4'b0, $signed(2'b11))
ರಚನೆ VerilogFmtArg {
	ಎಣಿಕೆ {
		STRING  = 0,
		INTEGER = 1,
		ಸಮಯ    = 2,
	} type;

	// All types
	Log_ಮೂಲ_Location src;

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
ರಚನೆ FmtPart {
	ಎಣಿಕೆ {
		LITERAL  	= 0,
		INTEGER 	= 1,
		STRING    = 2,
		UNICHAR   = 3,
		VLOG_ಸಮಯ = 4,
	} type;

	// LITERAL type
	std::string str;

	// INTEGER/STRING/UNICHAR types
	RTLIL::SigSpec sig;

	// INTEGER/STRING/VLOG_TIME types
	ಎಣಿಕೆ {
		ಬಲ_2	= 0,
		ಎಡ_2	= 1,
		NUMERIC	= 2,
	} justify = ಬಲ_2;
	ಅಕ್ಷರ padding = '\0';
	ಗಾತ್ರ_t ಅಗಲ = 0;

	// INTEGER type
	ಚಿಹ್ನೆರಹಿತ ತಳ = 10;
	bool signed_ = false;
	ಎಣಿಕೆ {
		MINUS		= 0,
		PLUS_MINUS	= 1,
		SPACE_MINUS	= 2,
	} sign = MINUS;
	bool hex_ಊರ್ಧ್ವ = false;
	bool show_ತಳ = false;
	bool ಗುಂಪು = false;

	// VLOG_TIME type
	bool realtime = false;
};

ರಚನೆ Fmt {
ಸಾರ್ವಜನಿಕ:
	std::ಸದಿಶ<FmtPart> parts;

	ಶೂನ್ಯ append_literal(ಸ್ಥಿರಾಂಕ std::string &str);

	ಶೂನ್ಯ parse_rtlil(ಸ್ಥಿರಾಂಕ RTLIL::Cell *cell);
	ಶೂನ್ಯ emit_rtlil(RTLIL::Cell *cell) ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ parse_verilog(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<VerilogFmtArg> &ವಾದಗಳು_2, bool sformat_like, ಪೂರ್ಣಾಂಕ default_ತಳ, ಸ್ಥಿರಾಂಕ std::string &task_name, ಸ್ಥಿರಾಂಕ std::string &module_name);
	std::ಸದಿಶ<VerilogFmtArg> emit_verilog() ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ emit_cxxrtl(std::ostream &os, std::string indent, std::function<ಶೂನ್ಯ(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &)> emit_sig, ಸ್ಥಿರಾಂಕ std::string &context) ಸ್ಥಿರಾಂಕ;

	std::string render() ಸ್ಥಿರಾಂಕ;

ಖಾಸಗಿ:
	ಶೂನ್ಯ apply_verilog_automatic_sizing_and_ಕೂಡಿಸು(FmtPart &part);
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
