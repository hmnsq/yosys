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


#ပေႃး defined(YOSYS_ENABLE_SPAWN)
static std::string file_base_name(std::string const & path)
{
	ပွၵ်ႈ path.substr(path.find_last_of("/\\") + 1);
}
#endif

FstData::FstData(std::string filename) : ctx(nullptr)
{
	#ပေႃး defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_base_name(filename);
	ပေႃး (filename_trim.size() > 4 && filename_trim.compare(filename_trim.size()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.size()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_base_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		log("Exec: %s\n", cmd);
		ပေႃး (run_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#endif
	const std::vector<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	ပေႃး (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (puur_naa_nka)fstReaderGetTimescale(ctx);
	timescale_str = "";
	puur_naa_nka unit = 0;
	puur_naa_nka zeros = 0;
	ပေႃး (scale > 0)  {
		zeros = scale;
	} ပေႃးဢမ်ႇ {
		ပေႃး ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} ပေႃးဢမ်ႇ {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	for (puur_naa_nka i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extractVarNames();
}

FstData::~FstData()
{
	ပေႃး (ctx)
		fstReaderClose(ctx);
	ပေႃး (!tmp_file.empty())
		remove(tmp_file.c_str());
}

uint64_t FstData::getStartTime() { ပွၵ်ႈ fstReaderGetStartTime(ctx); }

uint64_t FstData::getEndTime() { ပွၵ်ႈ fstReaderGetEndTime(ctx); }

static void normalize_brackets(std::string &str)
{
	for (auto &c : str) {
		ပေႃး (c == '<')
			c = '[';
		ပေႃးဢမ်ႇ ပေႃး (c == '>')
			c = ']';
	}
}

fstHandle FstData::getHandle(std::string name) {
	normalize_brackets(name);
	ပေႃး (name_to_handle.find(name) != name_to_handle.end())
		ပွၵ်ႈ name_to_handle[name];
	ပေႃးဢမ်ႇ
		ပွၵ်ႈ 0;
};

dict<puur_naa_nka,fstHandle> FstData::getMemoryHandles(std::string name) {
	ပေႃး (memory_to_handle.find(name) != memory_to_handle.end())
		ပွၵ်ႈ memory_to_handle[name];
	ပေႃးဢမ်ႇ
		ပွၵ်ႈ dict<puur_naa_nka,fstHandle>();
};

static std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	ပွၵ်ႈ str;
}

void FstData::extractVarNames()
{
	sa_nracanaa fstHier *h;
	std::string fst_scope_name;

	ၶၢဝ်း ((h = fstReaderIterateHier(ctx))) {
		switch (h->htyp) {
			pak_sa FST_HT_SCOPE: {
				fst_scope_name = fstReaderPushScope(ctx, h->u.scope.name, NULL);
				bha_nga;
			}
			pak_sa FST_HT_UPSCOPE: {
				fst_scope_name = fstReaderPopScope(ctx);
				bha_nga;
			}
			pak_sa FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.width = h->u.var.length;
				vars.push_back(var);
				ပေႃး (!var.is_alias)
					handle_to_var[h->u.var.handle] = var;
				std::string clean_name;
				bool has_space = false;
				for(size_t i=0;i<strlen(h->u.var.name);i++)
				{
					var_na c = h->u.var.name[i];
					ပေႃး(c==' ') { has_space = true; bha_nga; }
					clean_name += c;
				}
				ပေႃး (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				ပေႃး (!has_space) {
					size_t pos = clean_name.find_last_of("[");
					std::string index_or_range = clean_name.substr(pos+1);
					ပေႃး (index_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				size_t pos = clean_name.find_last_of("<");
				ပေႃး (pos != std::string::npos && clean_name.back() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.pop_back(); // remove closing bracket
					var_na *endptr;
					puur_naa_nka mem_addr = strtol(addr.c_str(), &endptr, 16);
					ပေႃး (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} ပေႃးဢမ်ႇ {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_last_of("[");
				ပေႃး (pos != std::string::npos && clean_name.back() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.pop_back(); // remove closing bracket
					var_na *endptr;
					puur_naa_nka mem_addr = strtol(addr.c_str(), &endptr, 10);
					ပေႃး (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} ပေႃးဢမ်ႇ {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_to_handle[var.scope+"."+clean_name] = h->u.var.handle;
				bha_nga;
			}
		}
	}
}


static void reconstruct_clb_varlen_attimes(void *user_data, uint64_t pnt_time, fstHandle pnt_facidx, const unsigned var_na *pnt_value, uint32_t plen)
{
	FstData *ptr = (FstData*)user_data;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_value, plen);
}

static void reconstruct_clb_attimes(void *user_data, uint64_t pnt_time, fstHandle pnt_facidx, const unsigned var_na *pnt_value)
{
	FstData *ptr = (FstData*)user_data;
	uint32_t plen = (pnt_value) ?  strlen((const var_na *)pnt_value) : 0;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_value, plen);
}

void FstData::reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, const unsigned var_na *pnt_value, uint32_t /* plen */)
{
	ပေႃး (pnt_time > end_time || !pnt_value) ပွၵ်ႈ;
	ပေႃး (curr_cycle > last_cycle) ပွၵ်ႈ;
	// if we are past the timestamp
	bool is_clock = false;
	ပေႃး (!all_samples) {
		for(auto &s : clk_signals) {
			ပေႃး (s==pnt_facidx)  {
				is_clock=true;
				bha_nga;
			}
		}
	}

	ပေႃး (pnt_time > past_time) {
		past_data = last_data;
		past_time = pnt_time;
	}

	ပေႃး (pnt_time > last_time) {
		ပေႃး (all_samples) {
			callback(last_time);
			curr_cycle++;
			last_time = pnt_time;
		} ပေႃးဢမ်ႇ {
			ပေႃး (is_clock) {
				std::string val = std::string((const var_na *)pnt_value);
				std::string prev = past_data[pnt_facidx];
				ပေႃး ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(last_time);
					curr_cycle++;
					last_time = pnt_time;
				}
			}
		}
	}
	// always update last_data
	last_data[pnt_facidx] =  std::string((const var_na *)pnt_value);
}

void FstData::reconstructAllAtTimes(std::vector<fstHandle> &sa_nkeeta, uint64_t start, uint64_t end, unsigned puur_naa_nka end_cycle, CallbackFunction cb)
{
	clk_signals = sa_nkeeta;
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
	ပေႃး (last_time!=end_time && curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(last_time);
		curr_cycle++;
	}
	ပေႃး (curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(end_time);
		curr_cycle++;
	}
}

std::string FstData::valueOf(fstHandle sa_nkeeta)
{
	ပေႃး (past_data.find(sa_nkeeta) == past_data.end()) {
		ပွၵ်ႈ std::string(handle_to_var[sa_nkeeta].width, 'x');
	}
	ပွၵ်ႈ past_data[sa_nkeeta];
}
