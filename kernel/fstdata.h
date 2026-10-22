/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2022  Miodrag Milanovic <micko@yosyshq.com>
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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FSTDATA_H
#ವ್ಯಾಖ್ಯಾನಿಸು FSTDATA_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::function<ಶೂನ್ಯ(uint64_t)> CallbackFunction;
ರಚನೆ fst_ಕೊನೆ_ದ_ದತ್ತಾಂಶ_exception { };

ರಚನೆ FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	ಪೂರ್ಣಾಂಕ ಅಗಲ;
};

class Fst_ದತ್ತಾಂಶ
{
	ಸಾರ್ವಜನಿಕ:
	Fst_ದತ್ತಾಂಶ(std::string filename);
	~Fst_ದತ್ತಾಂಶ();

	uint64_t get_ಆರಂಭ_ಸಮಯ();
	uint64_t get_ಕೊನೆ_ಸಮಯ();

	std::ಸದಿಶ<FstVar>& getVars() { ಹಿಂತಿರುಗಿ vars; };

	ಶೂನ್ಯ reconstruct_callback_attimes(uint64_t pnt_ಸಮಯ, fstHandle pnt_facidx, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pnt_ಮೌಲ್ಯ, uint32_t plen);
	ಶೂನ್ಯ reconstructAllAtTimes(std::ಸದಿಶ<fstHandle> &ಸಂಕೇತ, uint64_t ಆರಂಭ_ಸಮಯ, uint64_t ಕೊನೆ_ಸಮಯ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಚಕ್ರ, CallbackFunction cb);

	std::string ಮೌಲ್ಯ_ದ(fstHandle ಸಂಕೇತ);
	fstHandle getHandle(std::string name);
	dict<ಪೂರ್ಣಾಂಕ,fstHandle> get_ಸ್ಮರಣೆ_Handles(std::string name);
	ಪೂರ್ಣಾಂಕ getScale() { ಹಿಂತಿರುಗಿ scale; }
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *getTimescaleString() { ಹಿಂತಿರುಗಿ timescale_str.c_str(); }
ಖಾಸಗಿ:
	ಶೂನ್ಯ ಹೊರತೆಗೆ_Var_ಹೆಸರುಗಳು();

	ರಚನೆ fstReaderContext *ctx;
	std::ಸದಿಶ<FstVar> vars;
	std::map<fstHandle, FstVar> handle_ಗಮ್ಯ_var;
	std::map<std::string, fstHandle> name_ಗಮ್ಯ_handle;
	std::map<std::string, dict<ಪೂರ್ಣಾಂಕ, fstHandle>> ಸ್ಮರಣೆ_ಗಮ್ಯ_handle;
	std::map<fstHandle, std::string> ಕೊನೆಯ_ದತ್ತಾಂಶ;
	uint64_t ಕೊನೆಯ_ಸಮಯ;
	std::map<fstHandle, std::string> past_ದತ್ತಾಂಶ;
	uint64_t past_ಸಮಯ;
	ಪೂರ್ಣಾಂಕ scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t ಆರಂಭ_ಸಮಯ;
	uint64_t ಕೊನೆ_ಸಮಯ;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_ಚಕ್ರ;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ curr_ಚಕ್ರ;
	CallbackFunction callback;
	std::ಸದಿಶ<fstHandle> clk_ಸಂಕೇತಗಳು;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
