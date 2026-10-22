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

#جيڪڏهن_وصف_نه_ٿيل FSTDATA_H
#وصف_ڪريو FSTDATA_H

#ضمن "kernel/yosys.h"
#ضمن "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

تعريفنوع std::function<فراغ(uint64_t)> CallbackFunction;
بنية fst_پڇاڙي_جو_معلومات_exception { };

بنية FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	صحيح ويڪر;
};

class Fst_معلومات
{
	عام:
	Fst_معلومات(std::string filename);
	~Fst_معلومات();

	uint64_t get_شروعات_وقت();
	uint64_t get_پڇاڙي_وقت();

	std::ويڪٽر<FstVar>& getVars() { أرجع vars; };

	فراغ reconstruct_callback_attimes(uint64_t pnt_وقت, fstHandle pnt_facidx, ثابت بنانشان حرف *pnt_قدر, uint32_t plen);
	فراغ reconstructAllAtTimes(std::ويڪٽر<fstHandle> &إشارة, uint64_t شروعات_وقت, uint64_t پڇاڙي_وقت, بنانشان صحيح پڇاڙي_چڪر, CallbackFunction cb);

	std::string قدر_جو(fstHandle إشارة);
	fstHandle getHandle(std::string name);
	dict<صحيح,fstHandle> get_يادگيري_Handles(std::string name);
	صحيح getScale() { أرجع scale; }
	ثابت حرف *getTimescaleString() { أرجع timescale_str.c_str(); }
خاص:
	فراغ ڪڍو_Var_نالا();

	بنية fstReaderContext *ctx;
	std::ويڪٽر<FstVar> vars;
	std::map<fstHandle, FstVar> handle_منزل_var;
	std::map<std::string, fstHandle> name_منزل_handle;
	std::map<std::string, dict<صحيح, fstHandle>> يادگيري_منزل_handle;
	std::map<fstHandle, std::string> آخري_معلومات;
	uint64_t آخري_وقت;
	std::map<fstHandle, std::string> past_معلومات;
	uint64_t past_وقت;
	صحيح scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t شروعات_وقت;
	uint64_t پڇاڙي_وقت;
	بنانشان صحيح آخري_چڪر;
	بنانشان صحيح curr_چڪر;
	CallbackFunction callback;
	std::ويڪٽر<fstHandle> clk_سگنل;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
