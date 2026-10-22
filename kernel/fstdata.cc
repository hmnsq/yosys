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

#అన్తర్భావ "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#ఒకవేళ defined(YOSYS_ENABLE_SPAWN)
స్థిర std::string file_పునాది_name(std::string స్థిరరాశి & మార్గం_2)
{
	తిరిగి మార్గం_2.substr(మార్గం_2.కనుగొను_చివరి_యొక్క("/\\") + 1);
}
#అయితే_ముగింపు

Fst_దత్తాంశం::Fst_దత్తాంశం(std::string filename) : ctx(శూన్యసూచిక)
{
	#ఒకవేళ defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_పునాది_name(filename);
	ఒకవేళ (filename_trim.కొలత() > 4 && filename_trim.compare(filename_trim.కొలత()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.కొలత()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_పునాది_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		సంవర్గమానం("Exec: %s\n", cmd);
		ఒకవేళ (నిరంతరం_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#అయితే_ముగింపు
	స్థిరరాశి std::సదిశ<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	ఒకవేళ (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (పూర్ణాంకం)fstReaderGetTimescale(ctx);
	timescale_str = "";
	పూర్ణాంకం unit = 0;
	పూర్ణాంకం zeros = 0;
	ఒకవేళ (scale > 0)  {
		zeros = scale;
	} లేకపోతే {
		ఒకవేళ ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} లేకపోతే {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	కొరకు (పూర్ణాంకం i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	వెలికితీయి_Var_పేర్లు();
}

Fst_దత్తాంశం::~Fst_దత్తాంశం()
{
	ఒకవేళ (ctx)
		fstReaderClose(ctx);
	ఒకవేళ (!tmp_file.ఖాళీలు())
		remove(tmp_file.c_str());
}

uint64_t Fst_దత్తాంశం::get_ప్రారంభం_సమయం() { తిరిగి fst_Reader_Get_ప్రారంభం_సమయం(ctx); }

uint64_t Fst_దత్తాంశం::get_చివర_సమయం() { తిరిగి fst_Reader_Get_చివర_సమయం(ctx); }

స్థిర శూన్య normalize_brackets(std::string &str)
{
	కొరకు (స్వయంచాలక &c : str) {
		ఒకవేళ (c == '<')
			c = '[';
		లేకపోతే ఒకవేళ (c == '>')
			c = ']';
	}
}

fstHandle Fst_దత్తాంశం::getHandle(std::string name) {
	normalize_brackets(name);
	ఒకవేళ (name_గమ్యం_handle.కనుగొను(name) != name_గమ్యం_handle.చివర())
		తిరిగి name_గమ్యం_handle[name];
	లేకపోతే
		తిరిగి 0;
};

dict<పూర్ణాంకం,fstHandle> Fst_దత్తాంశం::get_జ్ఞాపకం_Handles(std::string name) {
	ఒకవేళ (జ్ఞాపకం_గమ్యం_handle.కనుగొను(name) != జ్ఞాపకం_గమ్యం_handle.చివర())
		తిరిగి జ్ఞాపకం_గమ్యం_handle[name];
	లేకపోతే
		తిరిగి dict<పూర్ణాంకం,fstHandle>();
};

స్థిర std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.చివర(), ' '), str.చివర());
	తిరిగి str;
}

శూన్య Fst_దత్తాంశం::వెలికితీయి_Var_పేర్లు()
{
	నిర్మాణం fstHier *h;
	std::string fst_scope_name;

	అయితే ((h = fstReaderIterateHier(ctx))) {
		ఎంపిక (h->htyp) {
			స్థితి FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_నెట్టు_Scope(ctx, h->u.scope.name, ఖాళీ);
				ఆపు;
			}
			స్థితి FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_తీయి_Scope(ctx);
				ఆపు;
			}
			స్థితి FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.వెడల్పు = h->u.var.నిడివి;
				vars.నెట్టు_వెనుక(var);
				ఒకవేళ (!var.is_alias)
					handle_గమ్యం_var[h->u.var.handle] = var;
				std::string clean_name;
				తార్కిక has_space = అబద్ధం;
				కొరకు(కొలత_t i=0;i<అక్షరశ్రేణి_పొడవు(h->u.var.name);i++)
				{
					అక్షరం c = h->u.var.name[i];
					ఒకవేళ(c==' ') { has_space = నిజం; ఆపు; }
					clean_name += c;
				}
				ఒకవేళ (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				ఒకవేళ (!has_space) {
					కొలత_t pos = clean_name.కనుగొను_చివరి_యొక్క("[");
					std::string సూచీ_or_range = clean_name.substr(pos+1);
					ఒకవేళ (సూచీ_or_range.కనుగొను(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				కొలత_t pos = clean_name.కనుగొను_చివరి_యొక్క("<");
				ఒకవేళ (pos != std::string::npos && clean_name.వెనుక() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.తీయి_వెనుక(); // remove closing bracket
					అక్షరం *endptr;
					పూర్ణాంకం mem_addr = strtol(addr.c_str(), &endptr, 16);
					ఒకవేళ (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} లేకపోతే {
						జ్ఞాపకం_గమ్యం_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.కనుగొను_చివరి_యొక్క("[");
				ఒకవేళ (pos != std::string::npos && clean_name.వెనుక() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.తీయి_వెనుక(); // remove closing bracket
					అక్షరం *endptr;
					పూర్ణాంకం mem_addr = strtol(addr.c_str(), &endptr, 10);
					ఒకవేళ (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} లేకపోతే {
						జ్ఞాపకం_గమ్యం_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_గమ్యం_handle[var.scope+"."+clean_name] = h->u.var.handle;
				ఆపు;
			}
		}
	}
}


స్థిర శూన్య reconstruct_clb_varlen_attimes(శూన్య *user_దత్తాంశం, uint64_t pnt_సమయం, fstHandle pnt_facidx, స్థిరరాశి గుర్తులేని అక్షరం *pnt_విలువ, uint32_t plen)
{
	Fst_దత్తాంశం *ptr = (Fst_దత్తాంశం*)user_దత్తాంశం;
	ptr->reconstruct_callback_attimes(pnt_సమయం, pnt_facidx, pnt_విలువ, plen);
}

స్థిర శూన్య reconstruct_clb_attimes(శూన్య *user_దత్తాంశం, uint64_t pnt_సమయం, fstHandle pnt_facidx, స్థిరరాశి గుర్తులేని అక్షరం *pnt_విలువ)
{
	Fst_దత్తాంశం *ptr = (Fst_దత్తాంశం*)user_దత్తాంశం;
	uint32_t plen = (pnt_విలువ) ?  అక్షరశ్రేణి_పొడవు((స్థిరరాశి అక్షరం *)pnt_విలువ) : 0;
	ptr->reconstruct_callback_attimes(pnt_సమయం, pnt_facidx, pnt_విలువ, plen);
}

శూన్య Fst_దత్తాంశం::reconstruct_callback_attimes(uint64_t pnt_సమయం, fstHandle pnt_facidx, స్థిరరాశి గుర్తులేని అక్షరం *pnt_విలువ, uint32_t /* plen */)
{
	ఒకవేళ (pnt_సమయం > చివర_సమయం || !pnt_విలువ) తిరిగి;
	ఒకవేళ (curr_చక్రం > చివరి_చక్రం) తిరిగి;
	// if we are past the timestamp
	తార్కిక is_clock = అబద్ధం;
	ఒకవేళ (!all_samples) {
		కొరకు(స్వయంచాలక &s : clk_సంకేతాలు) {
			ఒకవేళ (s==pnt_facidx)  {
				is_clock=నిజం;
				ఆపు;
			}
		}
	}

	ఒకవేళ (pnt_సమయం > past_సమయం) {
		past_దత్తాంశం = చివరి_దత్తాంశం;
		past_సమయం = pnt_సమయం;
	}

	ఒకవేళ (pnt_సమయం > చివరి_సమయం) {
		ఒకవేళ (all_samples) {
			callback(చివరి_సమయం);
			curr_చక్రం++;
			చివరి_సమయం = pnt_సమయం;
		} లేకపోతే {
			ఒకవేళ (is_clock) {
				std::string val = std::string((స్థిరరాశి అక్షరం *)pnt_విలువ);
				std::string prev = past_దత్తాంశం[pnt_facidx];
				ఒకవేళ ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(చివరి_సమయం);
					curr_చక్రం++;
					చివరి_సమయం = pnt_సమయం;
				}
			}
		}
	}
	// always update last_data
	చివరి_దత్తాంశం[pnt_facidx] =  std::string((స్థిరరాశి అక్షరం *)pnt_విలువ);
}

శూన్య Fst_దత్తాంశం::reconstructAllAtTimes(std::సదిశ<fstHandle> &సంకేతం, uint64_t ప్రారంభం, uint64_t చివర, గుర్తులేని పూర్ణాంకం చివర_చక్రం, CallbackFunction cb)
{
	clk_సంకేతాలు = సంకేతం;
	callback = cb;
	ప్రారంభం_సమయం = ప్రారంభం;
	చివర_సమయం = చివర;
	curr_చక్రం = 0;
	చివరి_చక్రం = చివర_చక్రం;
	చివరి_దత్తాంశం.clear();
	చివరి_సమయం = ప్రారంభం_సమయం;
	past_దత్తాంశం.clear();
	past_సమయం = ప్రారంభం_సమయం;
	all_samples = clk_సంకేతాలు.ఖాళీలు();

	fst_Reader_సమితి_Unlimited_సమయం_Range(ctx);
	fst_Reader_సమితి_Fac_Process_ముసుగు_All(ctx);
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, ఇది, శూన్యసూచిక);
	ఒకవేళ (చివరి_సమయం!=చివర_సమయం && curr_చక్రం <= చివరి_చక్రం) {
		past_దత్తాంశం = చివరి_దత్తాంశం;
		callback(చివరి_సమయం);
		curr_చక్రం++;
	}
	ఒకవేళ (curr_చక్రం <= చివరి_చక్రం) {
		past_దత్తాంశం = చివరి_దత్తాంశం;
		callback(చివర_సమయం);
		curr_చక్రం++;
	}
}

std::string Fst_దత్తాంశం::విలువ_యొక్క(fstHandle సంకేతం)
{
	ఒకవేళ (past_దత్తాంశం.కనుగొను(సంకేతం) == past_దత్తాంశం.చివర()) {
		తిరిగి std::string(handle_గమ్యం_var[సంకేతం].వెడల్పు, 'x');
	}
	తిరిగి past_దత్తాంశం[సంకేతం];
}
