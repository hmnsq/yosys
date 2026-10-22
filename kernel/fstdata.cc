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

#include "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#如果 defined(YOSYS_ENABLE_SPAWN)
static std::string file_base_name(std::string const & path)
{
	返回 path.substr(path.find_last_of("/\\") + 1);
}
#endif

FstData::FstData(std::string filename) : ctx(nullptr)
{
	#如果 defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_base_name(filename);
	如果 (filename_trim.size() > 4 && filename_trim.compare(filename_trim.size()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.size()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_base_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		log("Exec: %s\n", cmd);
		如果 (run_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#endif
	const std::vector<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	如果 (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (整数)fstReaderGetTimescale(ctx);
	timescale_str = "";
	整数 unit = 0;
	整数 zeros = 0;
	如果 (scale > 0)  {
		zeros = scale;
	} 否则 {
		如果 ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} 否则 {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	为 (整数 i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extractVarNames();
}

FstData::~FstData()
{
	如果 (ctx)
		fstReaderClose(ctx);
	如果 (!tmp_file.empty())
		remove(tmp_file.c_str());
}

uint64_t FstData::getStartTime() { 返回 fstReaderGetStartTime(ctx); }

uint64_t FstData::getEndTime() { 返回 fstReaderGetEndTime(ctx); }

static void normalize_brackets(std::string &str)
{
	为 (auto &c : str) {
		如果 (c == '<')
			c = '[';
		否则 如果 (c == '>')
			c = ']';
	}
}

fstHandle FstData::getHandle(std::string name) {
	normalize_brackets(name);
	如果 (name_to_handle.find(name) != name_to_handle.end())
		返回 name_to_handle[name];
	否则
		返回 0;
};

dict<整数,fstHandle> FstData::getMemoryHandles(std::string name) {
	如果 (memory_to_handle.find(name) != memory_to_handle.end())
		返回 memory_to_handle[name];
	否则
		返回 dict<整数,fstHandle>();
};

static std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	返回 str;
}

void FstData::extractVarNames()
{
	struct fstHier *h;
	std::string fst_scope_name;

	当 ((h = fstReaderIterateHier(ctx))) {
		switch (h->htyp) {
			case FST_HT_SCOPE: {
				fst_scope_name = fstReaderPushScope(ctx, h->u.scope.name, NULL);
				break;
			}
			case FST_HT_UPSCOPE: {
				fst_scope_name = fstReaderPopScope(ctx);
				break;
			}
			case FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.width = h->u.var.length;
				vars.push_back(var);
				如果 (!var.is_alias)
					handle_to_var[h->u.var.handle] = var;
				std::string clean_name;
				bool has_space = false;
				为(size_t i=0;i<strlen(h->u.var.name);i++)
				{
					char c = h->u.var.name[i];
					如果(c==' ') { has_space = true; break; }
					clean_name += c;
				}
				如果 (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				如果 (!has_space) {
					size_t pos = clean_name.find_last_of("[");
					std::string index_or_range = clean_name.substr(pos+1);
					如果 (index_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				size_t pos = clean_name.find_last_of("<");
				如果 (pos != std::string::npos && clean_name.back() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.pop_back(); // remove closing bracket
					char *endptr;
					整数 mem_addr = strtol(addr.c_str(), &endptr, 16);
					如果 (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} 否则 {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_last_of("[");
				如果 (pos != std::string::npos && clean_name.back() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.pop_back(); // remove closing bracket
					char *endptr;
					整数 mem_addr = strtol(addr.c_str(), &endptr, 10);
					如果 (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} 否则 {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_to_handle[var.scope+"."+clean_name] = h->u.var.handle;
				break;
			}
		}
	}
}


static void reconstruct_clb_varlen_attimes(void *user_data, uint64_t pnt_time, fstHandle pnt_facidx, const unsigned char *pnt_value, uint32_t plen)
{
	FstData *ptr = (FstData*)user_data;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_value, plen);
}

static void reconstruct_clb_attimes(void *user_data, uint64_t pnt_time, fstHandle pnt_facidx, const unsigned char *pnt_value)
{
	FstData *ptr = (FstData*)user_data;
	uint32_t plen = (pnt_value) ?  strlen((const char *)pnt_value) : 0;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_value, plen);
}

void FstData::reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, const unsigned char *pnt_value, uint32_t /* plen */)
{
	如果 (pnt_time > end_time || !pnt_value) 返回;
	如果 (curr_cycle > last_cycle) 返回;
	// if we are past the timestamp
	bool is_clock = false;
	如果 (!all_samples) {
		为(auto &s : clk_signals) {
			如果 (s==pnt_facidx)  {
				is_clock=true;
				break;
			}
		}
	}

	如果 (pnt_time > past_time) {
		past_data = last_data;
		past_time = pnt_time;
	}

	如果 (pnt_time > last_time) {
		如果 (all_samples) {
			callback(last_time);
			curr_cycle++;
			last_time = pnt_time;
		} 否则 {
			如果 (is_clock) {
				std::string val = std::string((const char *)pnt_value);
				std::string prev = past_data[pnt_facidx];
				如果 ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(last_time);
					curr_cycle++;
					last_time = pnt_time;
				}
			}
		}
	}
	// always update last_data
	last_data[pnt_facidx] =  std::string((const char *)pnt_value);
}

void FstData::reconstructAllAtTimes(std::vector<fstHandle> &signal, uint64_t start, uint64_t end, unsigned 整数 end_cycle, CallbackFunction cb)
{
	clk_signals = signal;
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
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, this, nullptr);
	如果 (last_time!=end_time && curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(last_time);
		curr_cycle++;
	}
	如果 (curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(end_time);
		curr_cycle++;
	}
}

std::string FstData::valueOf(fstHandle signal)
{
	如果 (past_data.find(signal) == past_data.end()) {
		返回 std::string(handle_to_var[signal].width, 'x');
	}
	返回 past_data[signal];
}
