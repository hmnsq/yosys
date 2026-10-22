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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

પ્રકારવ્યાખ્યા std::function<શૂન્ય(uint64_t)> CallbackFunction;
સંરચના fst_end_of_data_exception { };

સંરચના FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	પૂર્ણાંક width;
};

class FstData
{
	જાહેર:
	FstData(std::string filename);
	~FstData();

	uint64_t getStartTime();
	uint64_t getEndTime();

	std::vector<FstVar>& getVars() { પાછા vars; };

	શૂન્ય reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, અચલ unsigned અક્ષર *pnt_કિંમત, uint32_t plen);
	શૂન્ય reconstructAllAtTimes(std::vector<fstHandle> &સંકેત, uint64_t start_time, uint64_t end_time, unsigned પૂર્ણાંક end_cycle, CallbackFunction cb);

	std::string કિંમત_Of(fstHandle સંકેત);
	fstHandle getHandle(std::string name);
	dict<પૂર્ણાંક,fstHandle> getMemoryHandles(std::string name);
	પૂર્ણાંક getScale() { પાછા scale; }
	અચલ અક્ષર *getTimescaleString() { પાછા timescale_str.c_str(); }
ખાનગી:
	શૂન્ય extractVarNames();

	સંરચના fstReaderContext *ctx;
	std::vector<FstVar> vars;
	std::map<fstHandle, FstVar> handle_to_var;
	std::map<std::string, fstHandle> name_to_handle;
	std::map<std::string, dict<પૂર્ણાંક, fstHandle>> memory_to_handle;
	std::map<fstHandle, std::string> last_data;
	uint64_t last_time;
	std::map<fstHandle, std::string> past_data;
	uint64_t past_time;
	પૂર્ણાંક scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t start_time;
	uint64_t end_time;
	unsigned પૂર્ણાંક last_cycle;
	unsigned પૂર્ણાંક curr_cycle;
	CallbackFunction callback;
	std::vector<fstHandle> clk_signals;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_END

#endif
