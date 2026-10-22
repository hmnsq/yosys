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

#اگر_تعریف_نشده FSTDATA_H
#تعریف_کن FSTDATA_H

#ضمن "kernel/yosys.h"
#ضمن "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

تعريفنوع std::function<فراغ(uint64_t)> CallbackFunction;
بنية fst_انتها_از_داده_exception { };

بنية FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	صحيح عرض;
};

class Fst_داده
{
	عام:
	Fst_داده(std::string filename);
	~Fst_داده();

	uint64_t get_شروع_زمان();
	uint64_t get_انتها_زمان();

	std::وکتور<FstVar>& getVars() { أرجع vars; };

	فراغ reconstruct_callback_attimes(uint64_t pnt_زمان, fstHandle pnt_facidx, ثابت بیعلامت حرف *pnt_مقدار, uint32_t plen);
	فراغ reconstructAllAtTimes(std::وکتور<fstHandle> &إشارة, uint64_t شروع_زمان, uint64_t انتها_زمان, بیعلامت صحيح انتها_چرخه, CallbackFunction cb);

	std::string مقدار_از(fstHandle إشارة);
	fstHandle getHandle(std::string name);
	dict<صحيح,fstHandle> get_حافظه_Handles(std::string name);
	صحيح getScale() { أرجع scale; }
	ثابت حرف *getTimescaleString() { أرجع timescale_str.c_str(); }
خاص:
	فراغ استخراج_Var_نامها();

	بنية fstReaderContext *ctx;
	std::وکتور<FstVar> vars;
	std::map<fstHandle, FstVar> handle_مقصد_var;
	std::map<std::string, fstHandle> name_مقصد_handle;
	std::map<std::string, dict<صحيح, fstHandle>> حافظه_مقصد_handle;
	std::map<fstHandle, std::string> آخرین_داده;
	uint64_t آخرین_زمان;
	std::map<fstHandle, std::string> past_داده;
	uint64_t past_زمان;
	صحيح scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t شروع_زمان;
	uint64_t انتها_زمان;
	بیعلامت صحيح آخرین_چرخه;
	بیعلامت صحيح curr_چرخه;
	CallbackFunction callback;
	std::وکتور<fstHandle> clk_سیگنالها;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_انتها

#پایان_اگر
