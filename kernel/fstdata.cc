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


#නම් defined(YOSYS_ENABLE_SPAWN)
static std::string file_base_name(std::string const & path)
{
	ආපසු path.substr(path.find_last_of("/\\") + 1);
}
#endif

FstData::FstData(std::string filename) : ctx(nullptr)
{
	#නම් defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_base_name(filename);
	නම් (filename_trim.size() > 4 && filename_trim.compare(filename_trim.size()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.size()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_base_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		log("Exec: %s\n", cmd);
		නම් (run_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#endif
	const std::vector<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	නම් (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (පූර්ණ_සංඛ්‍යා)fstReaderGetTimescale(ctx);
	timescale_str = "";
	පූර්ණ_සංඛ්‍යා unit = 0;
	පූර්ණ_සංඛ්‍යා zeros = 0;
	නම් (scale > 0)  {
		zeros = scale;
	} නැතහොත් {
		නම් ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} නැතහොත් {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	සඳහා (පූර්ණ_සංඛ්‍යා i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extractVarNames();
}

FstData::~FstData()
{
	නම් (ctx)
		fstReaderClose(ctx);
	නම් (!tmp_file.empty())
		remove(tmp_file.c_str());
}

uint64_t FstData::getStartTime() { ආපසු fstReaderGetStartTime(ctx); }

uint64_t FstData::getEndTime() { ආපසු fstReaderGetEndTime(ctx); }

static void normalize_brackets(std::string &str)
{
	සඳහා (auto &c : str) {
		නම් (c == '<')
			c = '[';
		නැතහොත් නම් (c == '>')
			c = ']';
	}
}

fstHandle FstData::getHandle(std::string name) {
	normalize_brackets(name);
	නම් (name_to_handle.find(name) != name_to_handle.end())
		ආපසු name_to_handle[name];
	නැතහොත්
		ආපසු 0;
};

dict<පූර්ණ_සංඛ්‍යා,fstHandle> FstData::getMemoryHandles(std::string name) {
	නම් (memory_to_handle.find(name) != memory_to_handle.end())
		ආපසු memory_to_handle[name];
	නැතහොත්
		ආපසු dict<පූර්ණ_සංඛ්‍යා,fstHandle>();
};

static std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	ආපසු str;
}

void FstData::extractVarNames()
{
	sa_nracanaa fstHier *h;
	std::string fst_scope_name;

	අතරතුර ((h = fstReaderIterateHier(ctx))) {
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
				නම් (!var.is_alias)
					handle_to_var[h->u.var.handle] = var;
				std::string clean_name;
				bool has_space = false;
				සඳහා(size_t i=0;i<strlen(h->u.var.name);i++)
				{
					var_na c = h->u.var.name[i];
					නම්(c==' ') { has_space = true; bha_nga; }
					clean_name += c;
				}
				නම් (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				නම් (!has_space) {
					size_t pos = clean_name.find_last_of("[");
					std::string index_or_range = clean_name.substr(pos+1);
					නම් (index_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				size_t pos = clean_name.find_last_of("<");
				නම් (pos != std::string::npos && clean_name.back() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.pop_back(); // remove closing bracket
					var_na *endptr;
					පූර්ණ_සංඛ්‍යා mem_addr = strtol(addr.c_str(), &endptr, 16);
					නම් (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} නැතහොත් {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_last_of("[");
				නම් (pos != std::string::npos && clean_name.back() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.pop_back(); // remove closing bracket
					var_na *endptr;
					පූර්ණ_සංඛ්‍යා mem_addr = strtol(addr.c_str(), &endptr, 10);
					නම් (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} නැතහොත් {
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
	නම් (pnt_time > end_time || !pnt_value) ආපසු;
	නම් (curr_cycle > last_cycle) ආපසු;
	// if we are past the timestamp
	bool is_clock = false;
	නම් (!all_samples) {
		සඳහා(auto &s : clk_signals) {
			නම් (s==pnt_facidx)  {
				is_clock=true;
				bha_nga;
			}
		}
	}

	නම් (pnt_time > past_time) {
		past_data = last_data;
		past_time = pnt_time;
	}

	නම් (pnt_time > last_time) {
		නම් (all_samples) {
			callback(last_time);
			curr_cycle++;
			last_time = pnt_time;
		} නැතහොත් {
			නම් (is_clock) {
				std::string val = std::string((const var_na *)pnt_value);
				std::string prev = past_data[pnt_facidx];
				නම් ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
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

void FstData::reconstructAllAtTimes(std::vector<fstHandle> &sa_nkeeta, uint64_t start, uint64_t end, unsigned පූර්ණ_සංඛ්‍යා end_cycle, CallbackFunction cb)
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
	නම් (last_time!=end_time && curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(last_time);
		curr_cycle++;
	}
	නම් (curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(end_time);
		curr_cycle++;
	}
}

std::string FstData::valueOf(fstHandle sa_nkeeta)
{
	නම් (past_data.find(sa_nkeeta) == past_data.end()) {
		ආපසු std::string(handle_to_var[sa_nkeeta].width, 'x');
	}
	ආපසු past_data[sa_nkeeta];
}
