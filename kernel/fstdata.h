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

#اگر_غیر_تعریف_شدہ FSTDATA_H
#تعریف_کرو FSTDATA_H

#شامل "kernel/yosys.h"
#شامل "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

قسمتعریف std::function<خالی(uint64_t)> CallbackFunction;
ساخت fst_انتہا_کا_ڈیٹا_exception { };

ساخت FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	صحیح چوڑائی;
};

class Fst_ڈیٹا
{
	عوامی:
	Fst_ڈیٹا(std::string filename);
	~Fst_ڈیٹا();

	uint64_t get_آغاز_وقت();
	uint64_t get_انتہا_وقت();

	std::سمتیہ<FstVar>& getVars() { واپس vars; };

	خالی reconstruct_callback_attimes(uint64_t pnt_وقت, fstHandle pnt_facidx, ثابت غیرعلامتی حرف *pnt_قدر, uint32_t plen);
	خالی reconstructAllAtTimes(std::سمتیہ<fstHandle> &اشارہ, uint64_t آغاز_وقت, uint64_t انتہا_وقت, غیرعلامتی صحیح انتہا_چکر, CallbackFunction cb);

	std::string قدر_کا(fstHandle اشارہ);
	fstHandle getHandle(std::string name);
	dict<صحیح,fstHandle> get_حافظہ_Handles(std::string name);
	صحیح getScale() { واپس scale; }
	ثابت حرف *getTimescaleString() { واپس timescale_str.c_str(); }
نجی:
	خالی نکالو_Var_نام();

	ساخت fstReaderContext *ctx;
	std::سمتیہ<FstVar> vars;
	std::map<fstHandle, FstVar> handle_مقصد_var;
	std::map<std::string, fstHandle> name_مقصد_handle;
	std::map<std::string, dict<صحیح, fstHandle>> حافظہ_مقصد_handle;
	std::map<fstHandle, std::string> آخری_ڈیٹا;
	uint64_t آخری_وقت;
	std::map<fstHandle, std::string> past_ڈیٹا;
	uint64_t past_وقت;
	صحیح scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t آغاز_وقت;
	uint64_t انتہا_وقت;
	غیرعلامتی صحیح آخری_چکر;
	غیرعلامتی صحیح curr_چکر;
	CallbackFunction callback;
	std::سمتیہ<fstHandle> clk_اشارات;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
