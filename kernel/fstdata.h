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

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

प्रकारनिर्देश std::function<शून्य(uint64_t)> CallbackFunction;
संरचना fst_end_of_data_exception { };

संरचना FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	पूर्णाङ्क width;
};

class FstData
{
	सार्वजनिक:
	FstData(std::string filename);
	~FstData();

	uint64_t getStartTime();
	uint64_t getEndTime();

	std::vector<FstVar>& getVars() { फर्काऊ vars; };

	शून्य reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, अचर अचिन्हित वर्ण *pnt_मान, uint32_t plen);
	शून्य reconstructAllAtTimes(std::vector<fstHandle> &सङ्केत, uint64_t start_time, uint64_t end_time, अचिन्हित पूर्णाङ्क end_cycle, CallbackFunction cb);

	std::string मान_Of(fstHandle सङ्केत);
	fstHandle getHandle(std::string name);
	dict<पूर्णाङ्क,fstHandle> getMemoryHandles(std::string name);
	पूर्णाङ्क getScale() { फर्काऊ scale; }
	अचर वर्ण *getTimescaleString() { फर्काऊ timescale_str.c_str(); }
गुप्त:
	शून्य extractVarNames();

	संरचना fstReaderContext *ctx;
	std::vector<FstVar> vars;
	std::map<fstHandle, FstVar> handle_to_var;
	std::map<std::string, fstHandle> name_to_handle;
	std::map<std::string, dict<पूर्णाङ्क, fstHandle>> memory_to_handle;
	std::map<fstHandle, std::string> last_data;
	uint64_t last_time;
	std::map<fstHandle, std::string> past_data;
	uint64_t past_time;
	पूर्णाङ्क scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t start_time;
	uint64_t end_time;
	अचिन्हित पूर्णाङ्क last_cycle;
	अचिन्हित पूर्णाङ्क curr_cycle;
	CallbackFunction callback;
	std::vector<fstHandle> clk_signals;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_END

#endif
