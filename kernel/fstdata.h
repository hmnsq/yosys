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

#यदि_अपरिभाषितम् FSTDATA_H
#परिभाषय FSTDATA_H

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

प्रकारनिर्देश std::function<शून्य(uint64_t)> CallbackFunction;
संरचना fst_अन्तः_सम्बन्धि_दत्त_exception { };

संरचना FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	पूर्णाङ्क विस्तारः;
};

class Fst_दत्त
{
	सार्वजनिक:
	Fst_दत्त(std::string filename);
	~Fst_दत्त();

	uint64_t getStartTime();
	uint64_t get_अन्तः_Time();

	std::सदिशम्<FstVar>& getVars() { प्रत्यागम vars; };

	शून्य reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, ध्रुव अचिह्नित वर्ण *pnt_मानम्, uint32_t plen);
	शून्य reconstructAllAtTimes(std::सदिशम्<fstHandle> &सङ्केत, uint64_t start_time, uint64_t अन्तः_time, अचिह्नित पूर्णाङ्क अन्तः_आवृत्तिः, CallbackFunction cb);

	std::string मानम्_सम्बन्धि(fstHandle सङ्केत);
	fstHandle getHandle(std::string name);
	dict<पूर्णाङ्क,fstHandle> get_स्मृतिः_Handles(std::string name);
	पूर्णाङ्क getScale() { प्रत्यागम scale; }
	ध्रुव वर्ण *getTimescaleString() { प्रत्यागम timescale_str.c_str(); }
गुप्त:
	शून्य extract_Var_नामानि();

	संरचना fstReaderContext *ctx;
	std::सदिशम्<FstVar> vars;
	std::map<fstHandle, FstVar> handle_गन्तव्यम्_var;
	std::map<std::string, fstHandle> name_गन्तव्यम्_handle;
	std::map<std::string, dict<पूर्णाङ्क, fstHandle>> स्मृतिः_गन्तव्यम्_handle;
	std::map<fstHandle, std::string> चरमः_दत्त;
	uint64_t चरमः_time;
	std::map<fstHandle, std::string> past_दत्त;
	uint64_t past_time;
	पूर्णाङ्क scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t start_time;
	uint64_t अन्तः_time;
	अचिह्नित पूर्णाङ्क चरमः_आवृत्तिः;
	अचिह्नित पूर्णाङ्क curr_आवृत्तिः;
	CallbackFunction callback;
	std::सदिशम्<fstHandle> clk_सङ्केताः;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
