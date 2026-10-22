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

#جے_بنا_تعریف FSTDATA_H
#تعریف_کرو FSTDATA_H

#ضمن "kernel/yosys.h"
#ضمن "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

تعريفنوع std::function<فراغ(uint64_t)> CallbackFunction;
بنية fst_سرا_دا_انکڑے_exception { };

بنية FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	صحيح چوڑائی;
};

class Fst_انکڑے
{
	عام:
	Fst_انکڑے(std::string filename);
	~Fst_انکڑے();

	uint64_t get_شروع_ویلا();
	uint64_t get_سرا_ویلا();

	std::سمتیہ<FstVar>& getVars() { أرجع vars; };

	فراغ reconstruct_callback_attimes(uint64_t pnt_ویلا, fstHandle pnt_facidx, ثابت بنانشانی حرف *pnt_قدر, uint32_t plen);
	فراغ reconstructAllAtTimes(std::سمتیہ<fstHandle> &إشارة, uint64_t شروع_ویلا, uint64_t سرا_ویلا, بنانشانی صحيح سرا_چکر, CallbackFunction cb);

	std::string قدر_دا(fstHandle إشارة);
	fstHandle getHandle(std::string name);
	dict<صحيح,fstHandle> get_حافظہ_Handles(std::string name);
	صحيح getScale() { أرجع scale; }
	ثابت حرف *getTimescaleString() { أرجع timescale_str.c_str(); }
خاص:
	فراغ کڈھو_Var_ناں();

	بنية fstReaderContext *ctx;
	std::سمتیہ<FstVar> vars;
	std::map<fstHandle, FstVar> handle_منزل_var;
	std::map<std::string, fstHandle> name_منزل_handle;
	std::map<std::string, dict<صحيح, fstHandle>> حافظہ_منزل_handle;
	std::map<fstHandle, std::string> آخری_انکڑے;
	uint64_t آخری_ویلا;
	std::map<fstHandle, std::string> past_انکڑے;
	uint64_t past_ویلا;
	صحيح scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t شروع_ویلا;
	uint64_t سرا_ویلا;
	بنانشانی صحيح آخری_چکر;
	بنانشانی صحيح curr_چکر;
	CallbackFunction callback;
	std::سمتیہ<fstHandle> clk_اشارے;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_سرا

#جے_انت
