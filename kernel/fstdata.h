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

#که_نه_تعریف_شوی FSTDATA_H
#تعریف_کړه FSTDATA_H

#ضمن "kernel/yosys.h"
#ضمن "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

تعريفنوع std::function<فراغ(uint64_t)> CallbackFunction;
بنية fst_پای_ټکی_د_معلومات_exception { };

بنية FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	صحيح پلنوالی;
};

class Fst_معلومات
{
	عام:
	Fst_معلومات(std::string filename);
	~Fst_معلومات();

	uint64_t get_پیل_وخت();
	uint64_t get_پای_ټکی_وخت();

	std::ویکټور<FstVar>& getVars() { أرجع vars; };

	فراغ reconstruct_callback_attimes(uint64_t pnt_وخت, fstHandle pnt_facidx, ثابت بینښه حرف *pnt_ارزښت, uint32_t plen);
	فراغ reconstructAllAtTimes(std::ویکټور<fstHandle> &إشارة, uint64_t پیل_وخت, uint64_t پای_ټکی_وخت, بینښه صحيح پای_ټکی_دوره, CallbackFunction cb);

	std::string ارزښت_د(fstHandle إشارة);
	fstHandle getHandle(std::string name);
	dict<صحيح,fstHandle> get_حافظه_Handles(std::string name);
	صحيح getScale() { أرجع scale; }
	ثابت حرف *getTimescaleString() { أرجع timescale_str.c_str(); }
خاص:
	فراغ ایستل_Var_نومونه();

	بنية fstReaderContext *ctx;
	std::ویکټور<FstVar> vars;
	std::map<fstHandle, FstVar> handle_منزل_var;
	std::map<std::string, fstHandle> name_منزل_handle;
	std::map<std::string, dict<صحيح, fstHandle>> حافظه_منزل_handle;
	std::map<fstHandle, std::string> وروستی_معلومات;
	uint64_t وروستی_وخت;
	std::map<fstHandle, std::string> past_معلومات;
	uint64_t past_وخت;
	صحيح scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t پیل_وخت;
	uint64_t پای_ټکی_وخت;
	بینښه صحيح وروستی_دوره;
	بینښه صحيح curr_دوره;
	CallbackFunction callback;
	std::ویکټور<fstHandle> clk_سیګنالونه;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
