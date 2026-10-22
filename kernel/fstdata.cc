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

#അന്തര്ഭാവ "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#എങ്കിൽ defined(YOSYS_ENABLE_SPAWN)
സ്ഥിതം std::string file_base_name(std::string സ്ഥിരം & path)
{
	തിരികെ path.substr(path.find_last_of("/\\") + 1);
}
#endif

FstData::FstData(std::string filename) : ctx(nullptr)
{
	#എങ്കിൽ defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_base_name(filename);
	എങ്കിൽ (filename_trim.size() > 4 && filename_trim.compare(filename_trim.size()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.size()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_base_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		log("Exec: %s\n", cmd);
		എങ്കിൽ (run_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#endif
	സ്ഥിരം std::vector<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	എങ്കിൽ (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (പൂർണ്ണസംഖ്യ)fstReaderGetTimescale(ctx);
	timescale_str = "";
	പൂർണ്ണസംഖ്യ unit = 0;
	പൂർണ്ണസംഖ്യ zeros = 0;
	എങ്കിൽ (scale > 0)  {
		zeros = scale;
	} അല്ലെങ്കിൽ {
		എങ്കിൽ ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} അല്ലെങ്കിൽ {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	വേണ്ടി (പൂർണ്ണസംഖ്യ i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extractVarNames();
}

FstData::~FstData()
{
	എങ്കിൽ (ctx)
		fstReaderClose(ctx);
	എങ്കിൽ (!tmp_file.empty())
		remove(tmp_file.c_str());
}

uint64_t FstData::getStartTime() { തിരികെ fstReaderGetStartTime(ctx); }

uint64_t FstData::getEndTime() { തിരികെ fstReaderGetEndTime(ctx); }

സ്ഥിതം ശൂന്യം normalize_brackets(std::string &str)
{
	വേണ്ടി (auto &c : str) {
		എങ്കിൽ (c == '<')
			c = '[';
		അല്ലെങ്കിൽ എങ്കിൽ (c == '>')
			c = ']';
	}
}

fstHandle FstData::getHandle(std::string name) {
	normalize_brackets(name);
	എങ്കിൽ (name_to_handle.find(name) != name_to_handle.end())
		തിരികെ name_to_handle[name];
	അല്ലെങ്കിൽ
		തിരികെ 0;
};

dict<പൂർണ്ണസംഖ്യ,fstHandle> FstData::getMemoryHandles(std::string name) {
	എങ്കിൽ (memory_to_handle.find(name) != memory_to_handle.end())
		തിരികെ memory_to_handle[name];
	അല്ലെങ്കിൽ
		തിരികെ dict<പൂർണ്ണസംഖ്യ,fstHandle>();
};

സ്ഥിതം std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	തിരികെ str;
}

ശൂന്യം FstData::extractVarNames()
{
	ഘടന fstHier *h;
	std::string fst_scope_name;

	ആകുമ്പോൾ ((h = fstReaderIterateHier(ctx))) {
		തിരഞ്ഞെടുപ്പ് (h->htyp) {
			അവസ്ഥ FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_തള്ളുക_Scope(ctx, h->u.scope.name, ശൂന്യ);
				നിർത്തുക;
			}
			അവസ്ഥ FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_എടുക്കുക_Scope(ctx);
				നിർത്തുക;
			}
			അവസ്ഥ FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.width = h->u.var.length;
				vars.തള്ളുക_back(var);
				എങ്കിൽ (!var.is_alias)
					handle_to_var[h->u.var.handle] = var;
				std::string clean_name;
				തർക്കം has_space = അസത്യം;
				വേണ്ടി(size_t i=0;i<strlen(h->u.var.name);i++)
				{
					അക്ഷരം c = h->u.var.name[i];
					എങ്കിൽ(c==' ') { has_space = സത്യം; നിർത്തുക; }
					clean_name += c;
				}
				എങ്കിൽ (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				എങ്കിൽ (!has_space) {
					size_t pos = clean_name.find_last_of("[");
					std::string index_or_range = clean_name.substr(pos+1);
					എങ്കിൽ (index_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				size_t pos = clean_name.find_last_of("<");
				എങ്കിൽ (pos != std::string::npos && clean_name.back() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.എടുക്കുക_back(); // remove closing bracket
					അക്ഷരം *endptr;
					പൂർണ്ണസംഖ്യ mem_addr = strtol(addr.c_str(), &endptr, 16);
					എങ്കിൽ (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} അല്ലെങ്കിൽ {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_last_of("[");
				എങ്കിൽ (pos != std::string::npos && clean_name.back() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.എടുക്കുക_back(); // remove closing bracket
					അക്ഷരം *endptr;
					പൂർണ്ണസംഖ്യ mem_addr = strtol(addr.c_str(), &endptr, 10);
					എങ്കിൽ (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} അല്ലെങ്കിൽ {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_to_handle[var.scope+"."+clean_name] = h->u.var.handle;
				നിർത്തുക;
			}
		}
	}
}


സ്ഥിതം ശൂന്യം reconstruct_clb_varlen_attimes(ശൂന്യം *user_data, uint64_t pnt_time, fstHandle pnt_facidx, സ്ഥിരം unsigned അക്ഷരം *pnt_മൂല്യം, uint32_t plen)
{
	FstData *ptr = (FstData*)user_data;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_മൂല്യം, plen);
}

സ്ഥിതം ശൂന്യം reconstruct_clb_attimes(ശൂന്യം *user_data, uint64_t pnt_time, fstHandle pnt_facidx, സ്ഥിരം unsigned അക്ഷരം *pnt_മൂല്യം)
{
	FstData *ptr = (FstData*)user_data;
	uint32_t plen = (pnt_മൂല്യം) ?  strlen((സ്ഥിരം അക്ഷരം *)pnt_മൂല്യം) : 0;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_മൂല്യം, plen);
}

ശൂന്യം FstData::reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, സ്ഥിരം unsigned അക്ഷരം *pnt_മൂല്യം, uint32_t /* plen */)
{
	എങ്കിൽ (pnt_time > end_time || !pnt_മൂല്യം) തിരികെ;
	എങ്കിൽ (curr_cycle > last_cycle) തിരികെ;
	// if we are past the timestamp
	തർക്കം is_clock = അസത്യം;
	എങ്കിൽ (!all_samples) {
		വേണ്ടി(auto &s : clk_signals) {
			എങ്കിൽ (s==pnt_facidx)  {
				is_clock=സത്യം;
				നിർത്തുക;
			}
		}
	}

	എങ്കിൽ (pnt_time > past_time) {
		past_data = last_data;
		past_time = pnt_time;
	}

	എങ്കിൽ (pnt_time > last_time) {
		എങ്കിൽ (all_samples) {
			callback(last_time);
			curr_cycle++;
			last_time = pnt_time;
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (is_clock) {
				std::string val = std::string((സ്ഥിരം അക്ഷരം *)pnt_മൂല്യം);
				std::string prev = past_data[pnt_facidx];
				എങ്കിൽ ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(last_time);
					curr_cycle++;
					last_time = pnt_time;
				}
			}
		}
	}
	// always update last_data
	last_data[pnt_facidx] =  std::string((സ്ഥിരം അക്ഷരം *)pnt_മൂല്യം);
}

ശൂന്യം FstData::reconstructAllAtTimes(std::vector<fstHandle> &സൂചന, uint64_t start, uint64_t end, unsigned പൂർണ്ണസംഖ്യ end_cycle, CallbackFunction cb)
{
	clk_signals = സൂചന;
	callback = cb;
	start_time = start;
	end_time = end;
	curr_cycle = 0;
	last_cycle = end_cycle;
	last_data.clear();
	last_time = start_time;
	past_data.clear();
	past_time = start_time;
	all_samples = clk_signals.empty();

	fstReaderSetUnlimitedTimeRange(ctx);
	fstReaderSetFacProcessMaskAll(ctx);
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, ഇത്, nullptr);
	എങ്കിൽ (last_time!=end_time && curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(last_time);
		curr_cycle++;
	}
	എങ്കിൽ (curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(end_time);
		curr_cycle++;
	}
}

std::string FstData::മൂല്യം_Of(fstHandle സൂചന)
{
	എങ്കിൽ (past_data.find(സൂചന) == past_data.end()) {
		തിരികെ std::string(handle_to_var[സൂചന].width, 'x');
	}
	തിരികെ past_data[സൂചന];
}
