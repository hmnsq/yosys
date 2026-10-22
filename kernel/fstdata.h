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

#অন্তর্ভাৱ "kernel/yosys.h"
#অন্তর্ভাৱ "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

প্রকারনির্দেশ std::function<শূন্য(uint64_t)> CallbackFunction;
সংরচনা fst_end_of_data_exception { };

সংরচনা FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	পূর্ণাঙ্ক width;
};

class FstData
{
	সার্ৱজনিক:
	FstData(std::string filename);
	~FstData();

	uint64_t getStartTime();
	uint64_t getEndTime();

	std::vector<FstVar>& getVars() { ঘূৰাই vars; };

	শূন্য reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, ধ্রুৱ unsigned ৱর্ণ *pnt_মান, uint32_t plen);
	শূন্য reconstructAllAtTimes(std::vector<fstHandle> &সঙ্কেত, uint64_t start_time, uint64_t end_time, unsigned পূর্ণাঙ্ক end_cycle, CallbackFunction cb);

	std::string মান_Of(fstHandle সঙ্কেত);
	fstHandle getHandle(std::string name);
	dict<পূর্ণাঙ্ক,fstHandle> getMemoryHandles(std::string name);
	পূর্ণাঙ্ক getScale() { ঘূৰাই scale; }
	ধ্রুৱ ৱর্ণ *getTimescaleString() { ঘূৰাই timescale_str.c_str(); }
গুপ্ত:
	শূন্য extractVarNames();

	সংরচনা fstReaderContext *ctx;
	std::vector<FstVar> vars;
	std::map<fstHandle, FstVar> handle_to_var;
	std::map<std::string, fstHandle> name_to_handle;
	std::map<std::string, dict<পূর্ণাঙ্ক, fstHandle>> memory_to_handle;
	std::map<fstHandle, std::string> last_data;
	uint64_t last_time;
	std::map<fstHandle, std::string> past_data;
	uint64_t past_time;
	পূর্ণাঙ্ক scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t start_time;
	uint64_t end_time;
	unsigned পূর্ণাঙ্ক last_cycle;
	unsigned পূর্ণাঙ্ক curr_cycle;
	CallbackFunction callback;
	std::vector<fstHandle> clk_signals;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_END

#endif
