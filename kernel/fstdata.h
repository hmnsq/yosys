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

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

തരനിർവചനം std::function<ശൂന്യം(uint64_t)> CallbackFunction;
ഘടന fst_end_of_data_exception { };

ഘടന FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	പൂർണ്ണസംഖ്യ width;
};

class FstData
{
	പൊതു:
	FstData(std::string filename);
	~FstData();

	uint64_t getStartTime();
	uint64_t getEndTime();

	std::vector<FstVar>& getVars() { തിരികെ vars; };

	ശൂന്യം reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, സ്ഥിരം unsigned അക്ഷരം *pnt_മൂല്യം, uint32_t plen);
	ശൂന്യം reconstructAllAtTimes(std::vector<fstHandle> &സൂചന, uint64_t start_time, uint64_t end_time, unsigned പൂർണ്ണസംഖ്യ end_cycle, CallbackFunction cb);

	std::string മൂല്യം_Of(fstHandle സൂചന);
	fstHandle getHandle(std::string name);
	dict<പൂർണ്ണസംഖ്യ,fstHandle> getMemoryHandles(std::string name);
	പൂർണ്ണസംഖ്യ getScale() { തിരികെ scale; }
	സ്ഥിരം അക്ഷരം *getTimescaleString() { തിരികെ timescale_str.c_str(); }
സ്വകാര്യ:
	ശൂന്യം extractVarNames();

	ഘടന fstReaderContext *ctx;
	std::vector<FstVar> vars;
	std::map<fstHandle, FstVar> handle_to_var;
	std::map<std::string, fstHandle> name_to_handle;
	std::map<std::string, dict<പൂർണ്ണസംഖ്യ, fstHandle>> memory_to_handle;
	std::map<fstHandle, std::string> last_data;
	uint64_t last_time;
	std::map<fstHandle, std::string> past_data;
	uint64_t past_time;
	പൂർണ്ണസംഖ്യ scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t start_time;
	uint64_t end_time;
	unsigned പൂർണ്ണസംഖ്യ last_cycle;
	unsigned പൂർണ്ണസംഖ്യ curr_cycle;
	CallbackFunction callback;
	std::vector<fstHandle> clk_signals;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_END

#endif
