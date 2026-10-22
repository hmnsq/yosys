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

#அந்தர்பாவ "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#எனில் defined(YOSYS_ENABLE_SPAWN)
நிலையான std::string file_base_name(std::string மாறிலி & path)
{
	திரும்பு path.substr(path.find_last_of("/\\") + 1);
}
#endif

FstData::FstData(std::string filename) : ctx(nullptr)
{
	#எனில் defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_base_name(filename);
	எனில் (filename_trim.size() > 4 && filename_trim.compare(filename_trim.size()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.size()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_base_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		log("Exec: %s\n", cmd);
		எனில் (run_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#endif
	மாறிலி std::vector<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	எனில் (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (முழு)fstReaderGetTimescale(ctx);
	timescale_str = "";
	முழு unit = 0;
	முழு zeros = 0;
	எனில் (scale > 0)  {
		zeros = scale;
	} இல்லையெனில் {
		எனில் ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} இல்லையெனில் {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	க்கு (முழு i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extractVarNames();
}

FstData::~FstData()
{
	எனில் (ctx)
		fstReaderClose(ctx);
	எனில் (!tmp_file.empty())
		remove(tmp_file.c_str());
}

uint64_t FstData::getStartTime() { திரும்பு fstReaderGetStartTime(ctx); }

uint64_t FstData::getEndTime() { திரும்பு fstReaderGetEndTime(ctx); }

நிலையான வெறுமை normalize_brackets(std::string &str)
{
	க்கு (auto &c : str) {
		எனில் (c == '<')
			c = '[';
		இல்லையெனில் எனில் (c == '>')
			c = ']';
	}
}

fstHandle FstData::getHandle(std::string name) {
	normalize_brackets(name);
	எனில் (name_to_handle.find(name) != name_to_handle.end())
		திரும்பு name_to_handle[name];
	இல்லையெனில்
		திரும்பு 0;
};

dict<முழு,fstHandle> FstData::getMemoryHandles(std::string name) {
	எனில் (memory_to_handle.find(name) != memory_to_handle.end())
		திரும்பு memory_to_handle[name];
	இல்லையெனில்
		திரும்பு dict<முழு,fstHandle>();
};

நிலையான std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	திரும்பு str;
}

வெறுமை FstData::extractVarNames()
{
	கட்டமைப்பு fstHier *h;
	std::string fst_scope_name;

	போது ((h = fstReaderIterateHier(ctx))) {
		தேர்வு (h->htyp) {
			நிலை FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_தள்ளு_Scope(ctx, h->u.scope.name, வெற்று);
				நிறுத்து;
			}
			நிலை FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_எடு_Scope(ctx);
				நிறுத்து;
			}
			நிலை FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.width = h->u.var.length;
				vars.தள்ளு_back(var);
				எனில் (!var.is_alias)
					handle_to_var[h->u.var.handle] = var;
				std::string clean_name;
				தருக்க has_space = பொய்;
				க்கு(size_t i=0;i<strlen(h->u.var.name);i++)
				{
					எழுத்து c = h->u.var.name[i];
					எனில்(c==' ') { has_space = உண்மை; நிறுத்து; }
					clean_name += c;
				}
				எனில் (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				எனில் (!has_space) {
					size_t pos = clean_name.find_last_of("[");
					std::string index_or_range = clean_name.substr(pos+1);
					எனில் (index_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				size_t pos = clean_name.find_last_of("<");
				எனில் (pos != std::string::npos && clean_name.back() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.எடு_back(); // remove closing bracket
					எழுத்து *endptr;
					முழு mem_addr = strtol(addr.c_str(), &endptr, 16);
					எனில் (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} இல்லையெனில் {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_last_of("[");
				எனில் (pos != std::string::npos && clean_name.back() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.எடு_back(); // remove closing bracket
					எழுத்து *endptr;
					முழு mem_addr = strtol(addr.c_str(), &endptr, 10);
					எனில் (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} இல்லையெனில் {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_to_handle[var.scope+"."+clean_name] = h->u.var.handle;
				நிறுத்து;
			}
		}
	}
}


நிலையான வெறுமை reconstruct_clb_varlen_attimes(வெறுமை *user_data, uint64_t pnt_time, fstHandle pnt_facidx, மாறிலி unsigned எழுத்து *pnt_மதிப்பு, uint32_t plen)
{
	FstData *ptr = (FstData*)user_data;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_மதிப்பு, plen);
}

நிலையான வெறுமை reconstruct_clb_attimes(வெறுமை *user_data, uint64_t pnt_time, fstHandle pnt_facidx, மாறிலி unsigned எழுத்து *pnt_மதிப்பு)
{
	FstData *ptr = (FstData*)user_data;
	uint32_t plen = (pnt_மதிப்பு) ?  strlen((மாறிலி எழுத்து *)pnt_மதிப்பு) : 0;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_மதிப்பு, plen);
}

வெறுமை FstData::reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, மாறிலி unsigned எழுத்து *pnt_மதிப்பு, uint32_t /* plen */)
{
	எனில் (pnt_time > end_time || !pnt_மதிப்பு) திரும்பு;
	எனில் (curr_cycle > last_cycle) திரும்பு;
	// if we are past the timestamp
	தருக்க is_clock = பொய்;
	எனில் (!all_samples) {
		க்கு(auto &s : clk_signals) {
			எனில் (s==pnt_facidx)  {
				is_clock=உண்மை;
				நிறுத்து;
			}
		}
	}

	எனில் (pnt_time > past_time) {
		past_data = last_data;
		past_time = pnt_time;
	}

	எனில் (pnt_time > last_time) {
		எனில் (all_samples) {
			callback(last_time);
			curr_cycle++;
			last_time = pnt_time;
		} இல்லையெனில் {
			எனில் (is_clock) {
				std::string val = std::string((மாறிலி எழுத்து *)pnt_மதிப்பு);
				std::string prev = past_data[pnt_facidx];
				எனில் ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(last_time);
					curr_cycle++;
					last_time = pnt_time;
				}
			}
		}
	}
	// always update last_data
	last_data[pnt_facidx] =  std::string((மாறிலி எழுத்து *)pnt_மதிப்பு);
}

வெறுமை FstData::reconstructAllAtTimes(std::vector<fstHandle> &சமிக்ஞை, uint64_t start, uint64_t end, unsigned முழு end_cycle, CallbackFunction cb)
{
	clk_signals = சமிக்ஞை;
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
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, இது, nullptr);
	எனில் (last_time!=end_time && curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(last_time);
		curr_cycle++;
	}
	எனில் (curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(end_time);
		curr_cycle++;
	}
}

std::string FstData::மதிப்பு_Of(fstHandle சமிக்ஞை)
{
	எனில் (past_data.find(சமிக்ஞை) == past_data.end()) {
		திரும்பு std::string(handle_to_var[சமிக்ஞை].width, 'x');
	}
	திரும்பு past_data[சமிக்ஞை];
}
