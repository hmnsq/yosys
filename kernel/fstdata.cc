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

#شامل "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#اگر defined(YOSYS_ENABLE_SPAWN)
ایستا std::string file_base_name(std::string ثابت & path)
{
	بازگشت path.substr(path.find_last_of("/\\") + 1);
}
#endif

FstData::FstData(std::string filename) : ctx(nullptr)
{
	#اگر defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_base_name(filename);
	اگر (filename_trim.size() > 4 && filename_trim.compare(filename_trim.size()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.size()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_base_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		log("Exec: %s\n", cmd);
		اگر (run_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#endif
	ثابت std::vector<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	اگر (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (صحیح)fstReaderGetTimescale(ctx);
	timescale_str = "";
	صحیح unit = 0;
	صحیح zeros = 0;
	اگر (scale > 0)  {
		zeros = scale;
	} وگرنه {
		اگر ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} وگرنه {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	برای (صحیح i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extractVarNames();
}

FstData::~FstData()
{
	اگر (ctx)
		fstReaderClose(ctx);
	اگر (!tmp_file.empty())
		remove(tmp_file.c_str());
}

uint64_t FstData::getStartTime() { بازگشت fstReaderGetStartTime(ctx); }

uint64_t FstData::getEndTime() { بازگشت fstReaderGetEndTime(ctx); }

ایستا تهی normalize_brackets(std::string &str)
{
	برای (auto &c : str) {
		اگر (c == '<')
			c = '[';
		وگرنه اگر (c == '>')
			c = ']';
	}
}

fstHandle FstData::getHandle(std::string name) {
	normalize_brackets(name);
	اگر (name_to_handle.find(name) != name_to_handle.end())
		بازگشت name_to_handle[name];
	وگرنه
		بازگشت 0;
};

dict<صحیح,fstHandle> FstData::getMemoryHandles(std::string name) {
	اگر (memory_to_handle.find(name) != memory_to_handle.end())
		بازگشت memory_to_handle[name];
	وگرنه
		بازگشت dict<صحیح,fstHandle>();
};

ایستا std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	بازگشت str;
}

تهی FstData::extractVarNames()
{
	ساختار fstHier *h;
	std::string fst_scope_name;

	وقتی ((h = fstReaderIterateHier(ctx))) {
		انتخاب (h->htyp) {
			حالت FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_افزودن_Scope(ctx, h->u.scope.name, پوچ);
				بشکن;
			}
			حالت FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_برداشت_Scope(ctx);
				بشکن;
			}
			حالت FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.width = h->u.var.length;
				vars.افزودن_back(var);
				اگر (!var.is_alias)
					handle_to_var[h->u.var.handle] = var;
				std::string clean_name;
				منطقی has_space = نادرست;
				برای(size_t i=0;i<strlen(h->u.var.name);i++)
				{
					نویسه c = h->u.var.name[i];
					اگر(c==' ') { has_space = درست; بشکن; }
					clean_name += c;
				}
				اگر (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				اگر (!has_space) {
					size_t pos = clean_name.find_last_of("[");
					std::string index_or_range = clean_name.substr(pos+1);
					اگر (index_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				size_t pos = clean_name.find_last_of("<");
				اگر (pos != std::string::npos && clean_name.back() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.برداشت_back(); // remove closing bracket
					نویسه *endptr;
					صحیح mem_addr = strtol(addr.c_str(), &endptr, 16);
					اگر (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} وگرنه {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_last_of("[");
				اگر (pos != std::string::npos && clean_name.back() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.برداشت_back(); // remove closing bracket
					نویسه *endptr;
					صحیح mem_addr = strtol(addr.c_str(), &endptr, 10);
					اگر (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} وگرنه {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_to_handle[var.scope+"."+clean_name] = h->u.var.handle;
				بشکن;
			}
		}
	}
}


ایستا تهی reconstruct_clb_varlen_attimes(تهی *user_data, uint64_t pnt_time, fstHandle pnt_facidx, ثابت unsigned نویسه *pnt_مقدار, uint32_t plen)
{
	FstData *ptr = (FstData*)user_data;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_مقدار, plen);
}

ایستا تهی reconstruct_clb_attimes(تهی *user_data, uint64_t pnt_time, fstHandle pnt_facidx, ثابت unsigned نویسه *pnt_مقدار)
{
	FstData *ptr = (FstData*)user_data;
	uint32_t plen = (pnt_مقدار) ?  strlen((ثابت نویسه *)pnt_مقدار) : 0;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_مقدار, plen);
}

تهی FstData::reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, ثابت unsigned نویسه *pnt_مقدار, uint32_t /* plen */)
{
	اگر (pnt_time > end_time || !pnt_مقدار) بازگشت;
	اگر (curr_cycle > last_cycle) بازگشت;
	// if we are past the timestamp
	منطقی is_clock = نادرست;
	اگر (!all_samples) {
		برای(auto &s : clk_signals) {
			اگر (s==pnt_facidx)  {
				is_clock=درست;
				بشکن;
			}
		}
	}

	اگر (pnt_time > past_time) {
		past_data = last_data;
		past_time = pnt_time;
	}

	اگر (pnt_time > last_time) {
		اگر (all_samples) {
			callback(last_time);
			curr_cycle++;
			last_time = pnt_time;
		} وگرنه {
			اگر (is_clock) {
				std::string val = std::string((ثابت نویسه *)pnt_مقدار);
				std::string prev = past_data[pnt_facidx];
				اگر ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(last_time);
					curr_cycle++;
					last_time = pnt_time;
				}
			}
		}
	}
	// always update last_data
	last_data[pnt_facidx] =  std::string((ثابت نویسه *)pnt_مقدار);
}

تهی FstData::reconstructAllAtTimes(std::vector<fstHandle> &سیگنال, uint64_t start, uint64_t end, unsigned صحیح end_cycle, CallbackFunction cb)
{
	clk_signals = سیگنال;
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
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, این, nullptr);
	اگر (last_time!=end_time && curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(last_time);
		curr_cycle++;
	}
	اگر (curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(end_time);
		curr_cycle++;
	}
}

std::string FstData::مقدار_Of(fstHandle سیگنال)
{
	اگر (past_data.find(سیگنال) == past_data.end()) {
		بازگشت std::string(handle_to_var[سیگنال].width, 'x');
	}
	بازگشت past_data[سیگنال];
}
