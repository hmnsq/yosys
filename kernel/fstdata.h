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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

ପ୍ରକାରବ୍ୟାଖ୍ୟା std::function<ଶୂନ୍ୟ(uint64_t)> CallbackFunction;
ସଂରଚନା fst_end_of_data_exception { };

ସଂରଚନା FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width;
};

class FstData
{
	ସର୍ବସାଧାରଣ:
	FstData(std::string filename);
	~FstData();

	uint64_t getStartTime();
	uint64_t getEndTime();

	std::vector<FstVar>& getVars() { ଫେରନ୍ତୁ vars; };

	ଶୂନ୍ୟ reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, ଅଚଳ unsigned ଅକ୍ଷର *pnt_ମୂଲ୍ୟ, uint32_t plen);
	ଶୂନ୍ୟ reconstructAllAtTimes(std::vector<fstHandle> &ସଙ୍କେତ, uint64_t start_time, uint64_t end_time, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_cycle, CallbackFunction cb);

	std::string ମୂଲ୍ୟ_Of(fstHandle ସଙ୍କେତ);
	fstHandle getHandle(std::string name);
	dict<ପୂର୍ଣ୍ଣସଂଖ୍ୟା,fstHandle> getMemoryHandles(std::string name);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା getScale() { ଫେରନ୍ତୁ scale; }
	ଅଚଳ ଅକ୍ଷର *getTimescaleString() { ଫେରନ୍ତୁ timescale_str.c_str(); }
ବ୍ୟକ୍ତିଗତ:
	ଶୂନ୍ୟ extractVarNames();

	ସଂରଚନା fstReaderContext *ctx;
	std::vector<FstVar> vars;
	std::map<fstHandle, FstVar> handle_to_var;
	std::map<std::string, fstHandle> name_to_handle;
	std::map<std::string, dict<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, fstHandle>> memory_to_handle;
	std::map<fstHandle, std::string> last_data;
	uint64_t last_time;
	std::map<fstHandle, std::string> past_data;
	uint64_t past_time;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t start_time;
	uint64_t end_time;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_cycle;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା curr_cycle;
	CallbackFunction callback;
	std::vector<fstHandle> clk_signals;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_END

#endif
