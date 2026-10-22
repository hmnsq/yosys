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

#অন্তর্ভাব "kernel/yosys.h"
#অন্তর্ভাব "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

প্রকারলেখ std::function<শূন্য(uint64_t)> CallbackFunction;
কাঠামো fst_end_of_তথ্য_exception { };

কাঠামো FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	পূর্ণসংখ্যা width;
};

class Fst_তথ্য
{
	সর্বজনীন:
	Fst_তথ্য(std::string filename);
	~Fst_তথ্য();

	uint64_t getStartTime();
	uint64_t getEndTime();

	std::vector<FstVar>& getVars() { ফেরত vars; };

	শূন্য reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, ধ্রুবক unsigned অক্ষর *pnt_মান, uint32_t plen);
	শূন্য reconstructAllAtTimes(std::vector<fstHandle> &সংকেত, uint64_t start_time, uint64_t end_time, unsigned পূর্ণসংখ্যা end_cycle, CallbackFunction cb);

	std::string মান_Of(fstHandle সংকেত);
	fstHandle getHandle(std::string name);
	dict<পূর্ণসংখ্যা,fstHandle> getMemoryHandles(std::string name);
	পূর্ণসংখ্যা getScale() { ফেরত scale; }
	ধ্রুবক অক্ষর *getTimescaleString() { ফেরত timescale_str.c_str(); }
ব্যক্তিগত:
	শূন্য extractVarNames();

	কাঠামো fstReaderContext *ctx;
	std::vector<FstVar> vars;
	std::map<fstHandle, FstVar> handle_to_var;
	std::map<std::string, fstHandle> name_to_handle;
	std::map<std::string, dict<পূর্ণসংখ্যা, fstHandle>> memory_to_handle;
	std::map<fstHandle, std::string> last_তথ্য;
	uint64_t last_time;
	std::map<fstHandle, std::string> past_তথ্য;
	uint64_t past_time;
	পূর্ণসংখ্যা scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t start_time;
	uint64_t end_time;
	unsigned পূর্ণসংখ্যা last_cycle;
	unsigned পূর্ণসংখ্যা curr_cycle;
	CallbackFunction callback;
	std::vector<fstHandle> clk_signals;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_END

#endif
