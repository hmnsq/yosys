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

#ଅନ୍ତର୍ଭାଵ "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#ଯଦି defined(YOSYS_ENABLE_SPAWN)
ସ୍ଥିର std::string file_base_name(std::string ଅଚଳ & path)
{
	ଫେରନ୍ତୁ path.substr(path.find_last_of("/\\") + 1);
}
#endif

FstData::FstData(std::string filename) : ctx(nullptr)
{
	#ଯଦି defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_base_name(filename);
	ଯଦି (filename_trim.size() > 4 && filename_trim.compare(filename_trim.size()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.size()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_base_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		log("Exec: %s\n", cmd);
		ଯଦି (run_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#endif
	ଅଚଳ std::vector<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	ଯଦି (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fstReaderGetTimescale(ctx);
	timescale_str = "";
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା zeros = 0;
	ଯଦି (scale > 0)  {
		zeros = scale;
	} ଅନ୍ୟଥା {
		ଯଦି ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} ଅନ୍ୟଥା {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extractVarNames();
}

FstData::~FstData()
{
	ଯଦି (ctx)
		fstReaderClose(ctx);
	ଯଦି (!tmp_file.empty())
		remove(tmp_file.c_str());
}

uint64_t FstData::getStartTime() { ଫେରନ୍ତୁ fstReaderGetStartTime(ctx); }

uint64_t FstData::getEndTime() { ଫେରନ୍ତୁ fstReaderGetEndTime(ctx); }

ସ୍ଥିର ଶୂନ୍ୟ normalize_brackets(std::string &str)
{
	ପାଇଁ (auto &c : str) {
		ଯଦି (c == '<')
			c = '[';
		ଅନ୍ୟଥା ଯଦି (c == '>')
			c = ']';
	}
}

fstHandle FstData::getHandle(std::string name) {
	normalize_brackets(name);
	ଯଦି (name_to_handle.find(name) != name_to_handle.end())
		ଫେରନ୍ତୁ name_to_handle[name];
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ 0;
};

dict<ପୂର୍ଣ୍ଣସଂଖ୍ୟା,fstHandle> FstData::getMemoryHandles(std::string name) {
	ଯଦି (memory_to_handle.find(name) != memory_to_handle.end())
		ଫେରନ୍ତୁ memory_to_handle[name];
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ dict<ପୂର୍ଣ୍ଣସଂଖ୍ୟା,fstHandle>();
};

ସ୍ଥିର std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	ଫେରନ୍ତୁ str;
}

ଶୂନ୍ୟ FstData::extractVarNames()
{
	ସଂରଚନା fstHier *h;
	std::string fst_scope_name;

	ଯେତେବେଳେ ((h = fstReaderIterateHier(ctx))) {
		ବାଛ (h->htyp) {
			ସ୍ଥିତି FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_ଠେଲ_Scope(ctx, h->u.scope.name, ଖାଲି);
				ଭାଙ୍ଗ;
			}
			ସ୍ଥିତି FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_କାଢ଼_Scope(ctx);
				ଭାଙ୍ଗ;
			}
			ସ୍ଥିତି FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.width = h->u.var.length;
				vars.ଠେଲ_back(var);
				ଯଦି (!var.is_alias)
					handle_to_var[h->u.var.handle] = var;
				std::string clean_name;
				ତାର୍କିକ has_space = ମିଥ୍ୟା;
				ପାଇଁ(size_t i=0;i<strlen(h->u.var.name);i++)
				{
					ଅକ୍ଷର c = h->u.var.name[i];
					ଯଦି(c==' ') { has_space = ସତ୍ୟ; ଭାଙ୍ଗ; }
					clean_name += c;
				}
				ଯଦି (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				ଯଦି (!has_space) {
					size_t pos = clean_name.find_last_of("[");
					std::string index_or_range = clean_name.substr(pos+1);
					ଯଦି (index_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				size_t pos = clean_name.find_last_of("<");
				ଯଦି (pos != std::string::npos && clean_name.back() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.କାଢ଼_back(); // remove closing bracket
					ଅକ୍ଷର *endptr;
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା mem_addr = strtol(addr.c_str(), &endptr, 16);
					ଯଦି (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} ଅନ୍ୟଥା {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_last_of("[");
				ଯଦି (pos != std::string::npos && clean_name.back() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.କାଢ଼_back(); // remove closing bracket
					ଅକ୍ଷର *endptr;
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା mem_addr = strtol(addr.c_str(), &endptr, 10);
					ଯଦି (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} ଅନ୍ୟଥା {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_to_handle[var.scope+"."+clean_name] = h->u.var.handle;
				ଭାଙ୍ଗ;
			}
		}
	}
}


ସ୍ଥିର ଶୂନ୍ୟ reconstruct_clb_varlen_attimes(ଶୂନ୍ୟ *user_data, uint64_t pnt_time, fstHandle pnt_facidx, ଅଚଳ unsigned ଅକ୍ଷର *pnt_ମୂଲ୍ୟ, uint32_t plen)
{
	FstData *ptr = (FstData*)user_data;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_ମୂଲ୍ୟ, plen);
}

ସ୍ଥିର ଶୂନ୍ୟ reconstruct_clb_attimes(ଶୂନ୍ୟ *user_data, uint64_t pnt_time, fstHandle pnt_facidx, ଅଚଳ unsigned ଅକ୍ଷର *pnt_ମୂଲ୍ୟ)
{
	FstData *ptr = (FstData*)user_data;
	uint32_t plen = (pnt_ମୂଲ୍ୟ) ?  strlen((ଅଚଳ ଅକ୍ଷର *)pnt_ମୂଲ୍ୟ) : 0;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_ମୂଲ୍ୟ, plen);
}

ଶୂନ୍ୟ FstData::reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, ଅଚଳ unsigned ଅକ୍ଷର *pnt_ମୂଲ୍ୟ, uint32_t /* plen */)
{
	ଯଦି (pnt_time > end_time || !pnt_ମୂଲ୍ୟ) ଫେରନ୍ତୁ;
	ଯଦି (curr_cycle > last_cycle) ଫେରନ୍ତୁ;
	// if we are past the timestamp
	ତାର୍କିକ is_clock = ମିଥ୍ୟା;
	ଯଦି (!all_samples) {
		ପାଇଁ(auto &s : clk_signals) {
			ଯଦି (s==pnt_facidx)  {
				is_clock=ସତ୍ୟ;
				ଭାଙ୍ଗ;
			}
		}
	}

	ଯଦି (pnt_time > past_time) {
		past_data = last_data;
		past_time = pnt_time;
	}

	ଯଦି (pnt_time > last_time) {
		ଯଦି (all_samples) {
			callback(last_time);
			curr_cycle++;
			last_time = pnt_time;
		} ଅନ୍ୟଥା {
			ଯଦି (is_clock) {
				std::string val = std::string((ଅଚଳ ଅକ୍ଷର *)pnt_ମୂଲ୍ୟ);
				std::string prev = past_data[pnt_facidx];
				ଯଦି ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(last_time);
					curr_cycle++;
					last_time = pnt_time;
				}
			}
		}
	}
	// always update last_data
	last_data[pnt_facidx] =  std::string((ଅଚଳ ଅକ୍ଷର *)pnt_ମୂଲ୍ୟ);
}

ଶୂନ୍ୟ FstData::reconstructAllAtTimes(std::vector<fstHandle> &ସଙ୍କେତ, uint64_t start, uint64_t end, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_cycle, CallbackFunction cb)
{
	clk_signals = ସଙ୍କେତ;
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
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, ଏହି, nullptr);
	ଯଦି (last_time!=end_time && curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(last_time);
		curr_cycle++;
	}
	ଯଦି (curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(end_time);
		curr_cycle++;
	}
}

std::string FstData::ମୂଲ୍ୟ_Of(fstHandle ସଙ୍କେତ)
{
	ଯଦି (past_data.find(ସଙ୍କେତ) == past_data.end()) {
		ଫେରନ୍ତୁ std::string(handle_to_var[ସଙ୍କେତ].width, 'x');
	}
	ଫେରନ୍ତୁ past_data[ସଙ୍କେତ];
}
