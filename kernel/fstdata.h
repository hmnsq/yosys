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

#ifndef FSTDATA_H
#define FSTDATA_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

ಪ್ರಕಾರನಿರ್ದೇಶ std::function<ಶೂನ್ಯ(uint64_t)> CallbackFunction;
ಸಂರಚನಾ fst_end_of_data_exception { };

ಸಂರಚನಾ FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	ಪೂರ್ಣಾಙ್ಕ width;
};

class FstData
{
	ಸಾರ್ವಜನಿಕ:
	FstData(std::string filename);
	~FstData();

	uint64_t getStartTime();
	uint64_t getEndTime();

	std::vector<FstVar>& getVars() { ಪ್ರತ್ಯಾಗಮ vars; };

	ಶೂನ್ಯ reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, ಧ್ರುವ unsigned ವರ್ಣ *pnt_ಮೌಲ್ಯ, uint32_t plen);
	ಶೂನ್ಯ reconstructAllAtTimes(std::vector<fstHandle> &ಸಙ್ಕೇತ, uint64_t start_time, uint64_t end_time, unsigned ಪೂರ್ಣಾಙ್ಕ end_cycle, CallbackFunction cb);

	std::string ಮೌಲ್ಯ_Of(fstHandle ಸಙ್ಕೇತ);
	fstHandle getHandle(std::string name);
	dict<ಪೂರ್ಣಾಙ್ಕ,fstHandle> getMemoryHandles(std::string name);
	ಪೂರ್ಣಾಙ್ಕ getScale() { ಪ್ರತ್ಯಾಗಮ scale; }
	ಧ್ರುವ ವರ್ಣ *getTimescaleString() { ಪ್ರತ್ಯಾಗಮ timescale_str.c_str(); }
ಗುಪ್ತ:
	ಶೂನ್ಯ extractVarNames();

	ಸಂರಚನಾ fstReaderContext *ctx;
	std::vector<FstVar> vars;
	std::map<fstHandle, FstVar> handle_to_var;
	std::map<std::string, fstHandle> name_to_handle;
	std::map<std::string, dict<ಪೂರ್ಣಾಙ್ಕ, fstHandle>> memory_to_handle;
	std::map<fstHandle, std::string> last_data;
	uint64_t last_time;
	std::map<fstHandle, std::string> past_data;
	uint64_t past_time;
	ಪೂರ್ಣಾಙ್ಕ scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t start_time;
	uint64_t end_time;
	unsigned ಪೂರ್ಣಾಙ್ಕ last_cycle;
	unsigned ಪೂರ್ಣಾಙ್ಕ curr_cycle;
	CallbackFunction callback;
	std::vector<fstHandle> clk_signals;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_END

#endif
