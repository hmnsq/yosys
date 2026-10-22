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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FSTDATA_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FSTDATA_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::function<ਖਾਲੀ(uint64_t)> CallbackFunction;
ਬਣਤਰ fst_ਸਿਰਾ_ਦਾ_ਅੰਕੜੇ_exception { };

ਬਣਤਰ FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	ਪੂਰਨ ਚੌੜਾਈ;
};

class Fst_ਅੰਕੜੇ
{
	ਜਨਤਕ:
	Fst_ਅੰਕੜੇ(std::string filename);
	~Fst_ਅੰਕੜੇ();

	uint64_t get_ਸ਼ੁਰੂ_ਸਮਾਂ();
	uint64_t get_ਸਿਰਾ_ਸਮਾਂ();

	std::ਸਦਿਸ਼<FstVar>& getVars() { ਵਾਪਸ vars; };

	ਖਾਲੀ reconstruct_callback_attimes(uint64_t pnt_ਸਮਾਂ, fstHandle pnt_facidx, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pnt_ਮੁੱਲ, uint32_t plen);
	ਖਾਲੀ reconstructAllAtTimes(std::ਸਦਿਸ਼<fstHandle> &ਸੰਕੇਤ, uint64_t ਸ਼ੁਰੂ_ਸਮਾਂ, uint64_t ਸਿਰਾ_ਸਮਾਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸਿਰਾ_ਚੱਕਰ, CallbackFunction cb);

	std::string ਮੁੱਲ_ਦਾ(fstHandle ਸੰਕੇਤ);
	fstHandle getHandle(std::string name);
	dict<ਪੂਰਨ,fstHandle> get_ਸਮ੍ਰਿਤੀ_Handles(std::string name);
	ਪੂਰਨ getScale() { ਵਾਪਸ scale; }
	ਅਚਲ ਅੱਖਰ *getTimescaleString() { ਵਾਪਸ timescale_str.c_str(); }
ਨਿਜੀ:
	ਖਾਲੀ ਕੱਢੋ_Var_ਨਾਂ();

	ਬਣਤਰ fstReaderContext *ctx;
	std::ਸਦਿਸ਼<FstVar> vars;
	std::map<fstHandle, FstVar> handle_ਮੰਜ਼ਿਲ_var;
	std::map<std::string, fstHandle> name_ਮੰਜ਼ਿਲ_handle;
	std::map<std::string, dict<ਪੂਰਨ, fstHandle>> ਸਮ੍ਰਿਤੀ_ਮੰਜ਼ਿਲ_handle;
	std::map<fstHandle, std::string> ਆਖਰੀ_ਅੰਕੜੇ;
	uint64_t ਆਖਰੀ_ਸਮਾਂ;
	std::map<fstHandle, std::string> past_ਅੰਕੜੇ;
	uint64_t past_ਸਮਾਂ;
	ਪੂਰਨ scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t ਸ਼ੁਰੂ_ਸਮਾਂ;
	uint64_t ਸਿਰਾ_ਸਮਾਂ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਆਖਰੀ_ਚੱਕਰ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ curr_ਚੱਕਰ;
	CallbackFunction callback;
	std::ਸਦਿਸ਼<fstHandle> clk_ਸੰਕੇਤ;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
