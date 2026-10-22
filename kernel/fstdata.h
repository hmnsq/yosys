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

#నిర్వచితం_కాకపోతే FSTDATA_H
#నిర్వచించు FSTDATA_H

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "libs/fst/fstapi.h"

YOSYS_NAMESPACE_BEGIN

రకనిర్వచనం std::function<శూన్య(uint64_t)> CallbackFunction;
నిర్మాణం fst_చివర_యొక్క_దత్తాంశం_exception { };

నిర్మాణం FstVar
{
	fstHandle id;
	std::string name;
	bool is_alias;
	bool is_reg;
	std::string scope;
	పూర్ణాంకం వెడల్పు;
};

class Fst_దత్తాంశం
{
	బహిరంగ:
	Fst_దత్తాంశం(std::string filename);
	~Fst_దత్తాంశం();

	uint64_t get_ప్రారంభం_సమయం();
	uint64_t get_చివర_సమయం();

	std::సదిశ<FstVar>& getVars() { తిరిగి vars; };

	శూన్య reconstruct_callback_attimes(uint64_t pnt_సమయం, fstHandle pnt_facidx, స్థిరరాశి గుర్తులేని అక్షరం *pnt_విలువ, uint32_t plen);
	శూన్య reconstructAllAtTimes(std::సదిశ<fstHandle> &సంకేతం, uint64_t ప్రారంభం_సమయం, uint64_t చివర_సమయం, గుర్తులేని పూర్ణాంకం చివర_చక్రం, CallbackFunction cb);

	std::string విలువ_యొక్క(fstHandle సంకేతం);
	fstHandle getHandle(std::string name);
	dict<పూర్ణాంకం,fstHandle> get_జ్ఞాపకం_Handles(std::string name);
	పూర్ణాంకం getScale() { తిరిగి scale; }
	స్థిరరాశి అక్షరం *getTimescaleString() { తిరిగి timescale_str.c_str(); }
అంతరంగ:
	శూన్య వెలికితీయి_Var_పేర్లు();

	నిర్మాణం fstReaderContext *ctx;
	std::సదిశ<FstVar> vars;
	std::map<fstHandle, FstVar> handle_గమ్యం_var;
	std::map<std::string, fstHandle> name_గమ్యం_handle;
	std::map<std::string, dict<పూర్ణాంకం, fstHandle>> జ్ఞాపకం_గమ్యం_handle;
	std::map<fstHandle, std::string> చివరి_దత్తాంశం;
	uint64_t చివరి_సమయం;
	std::map<fstHandle, std::string> past_దత్తాంశం;
	uint64_t past_సమయం;
	పూర్ణాంకం scale; // exponent of 10, e.g. -6 = us, -9 = ns
	std::string timescale_str;
	uint64_t ప్రారంభం_సమయం;
	uint64_t చివర_సమయం;
	గుర్తులేని పూర్ణాంకం చివరి_చక్రం;
	గుర్తులేని పూర్ణాంకం curr_చక్రం;
	CallbackFunction callback;
	std::సదిశ<fstHandle> clk_సంకేతాలు;
	bool all_samples;
	std::string tmp_file;
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
