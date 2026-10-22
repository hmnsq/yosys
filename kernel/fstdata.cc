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

#ضمن "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#אם defined(YOSYS_ENABLE_SPAWN)
ساكن std::string file_base_name(std::string ثابت & path)
{
	החזר path.substr(path.find_last_of("/\\") + 1);
}
#endif

FstData::FstData(std::string filename) : ctx(nullptr)
{
	#אם defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_base_name(filename);
	אם (filename_trim.size() > 4 && filename_trim.compare(filename_trim.size()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.size()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_base_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		log("Exec: %s\n", cmd);
		אם (run_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#endif
	ثابت std::vector<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	אם (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (صحيح)fstReaderGetTimescale(ctx);
	timescale_str = "";
	صحيح unit = 0;
	صحيح zeros = 0;
	אם (scale > 0)  {
		zeros = scale;
	} אחרת {
		אם ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} אחרת {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	עבור (صحيح i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extractVarNames();
}

FstData::~FstData()
{
	אם (ctx)
		fstReaderClose(ctx);
	אם (!tmp_file.empty())
		remove(tmp_file.c_str());
}

uint64_t FstData::getStartTime() { החזר fstReaderGetStartTime(ctx); }

uint64_t FstData::getEndTime() { החזר fstReaderGetEndTime(ctx); }

ساكن فراغ normalize_brackets(std::string &str)
{
	עבור (auto &c : str) {
		אם (c == '<')
			c = '[';
		אחרת אם (c == '>')
			c = ']';
	}
}

fstHandle FstData::getHandle(std::string name) {
	normalize_brackets(name);
	אם (name_to_handle.find(name) != name_to_handle.end())
		החזר name_to_handle[name];
	אחרת
		החזר 0;
};

dict<صحيح,fstHandle> FstData::getMemoryHandles(std::string name) {
	אם (memory_to_handle.find(name) != memory_to_handle.end())
		החזר memory_to_handle[name];
	אחרת
		החזר dict<صحيح,fstHandle>();
};

ساكن std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	החזר str;
}

فراغ FstData::extractVarNames()
{
	بنية fstHier *h;
	std::string fst_scope_name;

	כלעוד ((h = fstReaderIterateHier(ctx))) {
		بدل (h->htyp) {
			حالة FST_HT_SCOPE: {
				fst_scope_name = fstReaderPushScope(ctx, h->u.scope.name, عدم);
				اكسر;
			}
			حالة FST_HT_UPSCOPE: {
				fst_scope_name = fstReaderPopScope(ctx);
				اكسر;
			}
			حالة FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.width = h->u.var.length;
				vars.push_back(var);
				אם (!var.is_alias)
					handle_to_var[h->u.var.handle] = var;
				std::string clean_name;
				منطقي has_space = שקר;
				עבור(size_t i=0;i<strlen(h->u.var.name);i++)
				{
					حرف c = h->u.var.name[i];
					אם(c==' ') { has_space = אמת; اكسر; }
					clean_name += c;
				}
				אם (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				אם (!has_space) {
					size_t pos = clean_name.find_last_of("[");
					std::string index_or_range = clean_name.substr(pos+1);
					אם (index_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				size_t pos = clean_name.find_last_of("<");
				אם (pos != std::string::npos && clean_name.back() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.pop_back(); // remove closing bracket
					حرف *endptr;
					صحيح mem_addr = strtol(addr.c_str(), &endptr, 16);
					אם (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} אחרת {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_last_of("[");
				אם (pos != std::string::npos && clean_name.back() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.pop_back(); // remove closing bracket
					حرف *endptr;
					صحيح mem_addr = strtol(addr.c_str(), &endptr, 10);
					אם (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} אחרת {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_to_handle[var.scope+"."+clean_name] = h->u.var.handle;
				اكسر;
			}
		}
	}
}


ساكن فراغ reconstruct_clb_varlen_attimes(فراغ *user_data, uint64_t pnt_time, fstHandle pnt_facidx, ثابت unsigned حرف *pnt_value, uint32_t plen)
{
	FstData *ptr = (FstData*)user_data;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_value, plen);
}

ساكن فراغ reconstruct_clb_attimes(فراغ *user_data, uint64_t pnt_time, fstHandle pnt_facidx, ثابت unsigned حرف *pnt_value)
{
	FstData *ptr = (FstData*)user_data;
	uint32_t plen = (pnt_value) ?  strlen((ثابت حرف *)pnt_value) : 0;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_value, plen);
}

فراغ FstData::reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, ثابت unsigned حرف *pnt_value, uint32_t /* plen */)
{
	אם (pnt_time > end_time || !pnt_value) החזר;
	אם (curr_cycle > last_cycle) החזר;
	// if we are past the timestamp
	منطقي is_clock = שקר;
	אם (!all_samples) {
		עבור(auto &s : clk_signals) {
			אם (s==pnt_facidx)  {
				is_clock=אמת;
				اكسر;
			}
		}
	}

	אם (pnt_time > past_time) {
		past_data = last_data;
		past_time = pnt_time;
	}

	אם (pnt_time > last_time) {
		אם (all_samples) {
			callback(last_time);
			curr_cycle++;
			last_time = pnt_time;
		} אחרת {
			אם (is_clock) {
				std::string val = std::string((ثابت حرف *)pnt_value);
				std::string prev = past_data[pnt_facidx];
				אם ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(last_time);
					curr_cycle++;
					last_time = pnt_time;
				}
			}
		}
	}
	// always update last_data
	last_data[pnt_facidx] =  std::string((ثابت حرف *)pnt_value);
}

فراغ FstData::reconstructAllAtTimes(std::vector<fstHandle> &إشارة, uint64_t start, uint64_t end, unsigned صحيح end_cycle, CallbackFunction cb)
{
	clk_signals = إشارة;
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
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, هذا, nullptr);
	אם (last_time!=end_time && curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(last_time);
		curr_cycle++;
	}
	אם (curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(end_time);
		curr_cycle++;
	}
}

std::string FstData::valueOf(fstHandle إشارة)
{
	אם (past_data.find(إشارة) == past_data.end()) {
		החזר std::string(handle_to_var[إشارة].width, 'x');
	}
	החזר past_data[إشارة];
}
