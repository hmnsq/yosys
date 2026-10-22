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

#include "kernel/yosys.h"
#include "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

typedef std::function<void(uint64_t)> CallbackFunction;
sa_nracanaa fst_end_of_data_exception { };

sa_nracanaa FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	ཧྲིལ width;
};

class FstData
{
	public:
	FstData(std::string filename);
	~FstData();

	uint64_t getStartTime();
	uint64_t getEndTime();

	std::vector<FstVar>& getVars() { ལོག vars; };

	void reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, const unsigned var_na *pnt_value, uint32_t plen);
	void reconstructAllAtTimes(std::vector<fstHandle> &sa_nkeeta, uint64_t start_time, uint64_t end_time, unsigned ཧྲིལ end_cycle, CallbackFunction cb);

	std::string valueOf(fstHandle sa_nkeeta);
	fstHandle getHandle(std::string name);
	dict<ཧྲིལ,fstHandle> getMemoryHandles(std::string name);
	ཧྲིལ getScale() { ལོག scale; }
	const var_na *getTimescaleString() { ལོག timescale_str.c_str(); }
private:
	void extractVarNames();

	sa_nracanaa fstReaderContext *ctx;
	std::vector<FstVar> vars;
	std::map<fstHandle, FstVar> handle_to_var;
	std::map<std::string, fstHandle> name_to_handle;
	std::map<std::string, dict<ཧྲིལ, fstHandle>> memory_to_handle;
	std::map<fstHandle, std::string> last_data;
	uint64_t last_time;
	std::map<fstHandle, std::string> past_data;
	uint64_t past_time;
	ཧྲིལ scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t start_time;
	uint64_t end_time;
	unsigned ཧྲིལ last_cycle;
	unsigned ཧྲིལ curr_cycle;
	CallbackFunction callback;
	std::vector<fstHandle> clk_signals;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_END

#endif
