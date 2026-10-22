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

#ಅನ್ತರ್ಭಾವ "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#ಯದಿ defined(YOSYS_ENABLE_SPAWN)
ಸ್ಥಿರ std::string file_base_name(std::string ಧ್ರುವ & path)
{
	ಪ್ರತ್ಯಾಗಮ path.substr(path.find_last_of("/\\") + 1);
}
#endif

FstData::FstData(std::string filename) : ctx(nullptr)
{
	#ಯದಿ defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_base_name(filename);
	ಯದಿ (filename_trim.size() > 4 && filename_trim.compare(filename_trim.size()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.size()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_base_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		log("Exec: %s\n", cmd);
		ಯದಿ (run_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#endif
	ಧ್ರುವ std::vector<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	ಯದಿ (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (ಪೂರ್ಣಾಙ್ಕ)fstReaderGetTimescale(ctx);
	timescale_str = "";
	ಪೂರ್ಣಾಙ್ಕ unit = 0;
	ಪೂರ್ಣಾಙ್ಕ zeros = 0;
	ಯದಿ (scale > 0)  {
		zeros = scale;
	} ಅನ್ಯಥಾ {
		ಯದಿ ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} ಅನ್ಯಥಾ {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extractVarNames();
}

FstData::~FstData()
{
	ಯದಿ (ctx)
		fstReaderClose(ctx);
	ಯದಿ (!tmp_file.empty())
		remove(tmp_file.c_str());
}

uint64_t FstData::getStartTime() { ಪ್ರತ್ಯಾಗಮ fstReaderGetStartTime(ctx); }

uint64_t FstData::getEndTime() { ಪ್ರತ್ಯಾಗಮ fstReaderGetEndTime(ctx); }

ಸ್ಥಿರ ಶೂನ್ಯ normalize_brackets(std::string &str)
{
	ಪ್ರತಿ (auto &c : str) {
		ಯದಿ (c == '<')
			c = '[';
		ಅನ್ಯಥಾ ಯದಿ (c == '>')
			c = ']';
	}
}

fstHandle FstData::getHandle(std::string name) {
	normalize_brackets(name);
	ಯದಿ (name_to_handle.find(name) != name_to_handle.end())
		ಪ್ರತ್ಯಾಗಮ name_to_handle[name];
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ 0;
};

dict<ಪೂರ್ಣಾಙ್ಕ,fstHandle> FstData::getMemoryHandles(std::string name) {
	ಯದಿ (memory_to_handle.find(name) != memory_to_handle.end())
		ಪ್ರತ್ಯಾಗಮ memory_to_handle[name];
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ dict<ಪೂರ್ಣಾಙ್ಕ,fstHandle>();
};

ಸ್ಥಿರ std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	ಪ್ರತ್ಯಾಗಮ str;
}

ಶೂನ್ಯ FstData::extractVarNames()
{
	ಸಂರಚನಾ fstHier *h;
	std::string fst_scope_name;

	ಯಾವತ್ ((h = fstReaderIterateHier(ctx))) {
		ವಿಕಲ್ಪ (h->htyp) {
			ಪಕ್ಷ FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_ತಳ್ಳು_Scope(ctx, h->u.scope.name, ರಿಕ್ತ);
				ಭಙ್ಗ;
			}
			ಪಕ್ಷ FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_ತೆಗೆ_Scope(ctx);
				ಭಙ್ಗ;
			}
			ಪಕ್ಷ FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.width = h->u.var.length;
				vars.ತಳ್ಳು_back(var);
				ಯದಿ (!var.is_alias)
					handle_to_var[h->u.var.handle] = var;
				std::string clean_name;
				ತಾರ್ಕಿಕ has_space = ಅಸತ್ಯ;
				ಪ್ರತಿ(size_t i=0;i<strlen(h->u.var.name);i++)
				{
					ವರ್ಣ c = h->u.var.name[i];
					ಯದಿ(c==' ') { has_space = ಸತ್ಯ; ಭಙ್ಗ; }
					clean_name += c;
				}
				ಯದಿ (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				ಯದಿ (!has_space) {
					size_t pos = clean_name.find_last_of("[");
					std::string index_or_range = clean_name.substr(pos+1);
					ಯದಿ (index_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				size_t pos = clean_name.find_last_of("<");
				ಯದಿ (pos != std::string::npos && clean_name.back() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.ತೆಗೆ_back(); // remove closing bracket
					ವರ್ಣ *endptr;
					ಪೂರ್ಣಾಙ್ಕ mem_addr = strtol(addr.c_str(), &endptr, 16);
					ಯದಿ (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} ಅನ್ಯಥಾ {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_last_of("[");
				ಯದಿ (pos != std::string::npos && clean_name.back() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.ತೆಗೆ_back(); // remove closing bracket
					ವರ್ಣ *endptr;
					ಪೂರ್ಣಾಙ್ಕ mem_addr = strtol(addr.c_str(), &endptr, 10);
					ಯದಿ (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} ಅನ್ಯಥಾ {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_to_handle[var.scope+"."+clean_name] = h->u.var.handle;
				ಭಙ್ಗ;
			}
		}
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ reconstruct_clb_varlen_attimes(ಶೂನ್ಯ *user_data, uint64_t pnt_time, fstHandle pnt_facidx, ಧ್ರುವ unsigned ವರ್ಣ *pnt_ಮೌಲ್ಯ, uint32_t plen)
{
	FstData *ptr = (FstData*)user_data;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_ಮೌಲ್ಯ, plen);
}

ಸ್ಥಿರ ಶೂನ್ಯ reconstruct_clb_attimes(ಶೂನ್ಯ *user_data, uint64_t pnt_time, fstHandle pnt_facidx, ಧ್ರುವ unsigned ವರ್ಣ *pnt_ಮೌಲ್ಯ)
{
	FstData *ptr = (FstData*)user_data;
	uint32_t plen = (pnt_ಮೌಲ್ಯ) ?  strlen((ಧ್ರುವ ವರ್ಣ *)pnt_ಮೌಲ್ಯ) : 0;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_ಮೌಲ್ಯ, plen);
}

ಶೂನ್ಯ FstData::reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, ಧ್ರುವ unsigned ವರ್ಣ *pnt_ಮೌಲ್ಯ, uint32_t /* plen */)
{
	ಯದಿ (pnt_time > end_time || !pnt_ಮೌಲ್ಯ) ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (curr_cycle > last_cycle) ಪ್ರತ್ಯಾಗಮ;
	// if we are past the timestamp
	ತಾರ್ಕಿಕ is_clock = ಅಸತ್ಯ;
	ಯದಿ (!all_samples) {
		ಪ್ರತಿ(auto &s : clk_signals) {
			ಯದಿ (s==pnt_facidx)  {
				is_clock=ಸತ್ಯ;
				ಭಙ್ಗ;
			}
		}
	}

	ಯದಿ (pnt_time > past_time) {
		past_data = last_data;
		past_time = pnt_time;
	}

	ಯದಿ (pnt_time > last_time) {
		ಯದಿ (all_samples) {
			callback(last_time);
			curr_cycle++;
			last_time = pnt_time;
		} ಅನ್ಯಥಾ {
			ಯದಿ (is_clock) {
				std::string val = std::string((ಧ್ರುವ ವರ್ಣ *)pnt_ಮೌಲ್ಯ);
				std::string prev = past_data[pnt_facidx];
				ಯದಿ ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(last_time);
					curr_cycle++;
					last_time = pnt_time;
				}
			}
		}
	}
	// always update last_data
	last_data[pnt_facidx] =  std::string((ಧ್ರುವ ವರ್ಣ *)pnt_ಮೌಲ್ಯ);
}

ಶೂನ್ಯ FstData::reconstructAllAtTimes(std::vector<fstHandle> &ಸಙ್ಕೇತ, uint64_t start, uint64_t end, unsigned ಪೂರ್ಣಾಙ್ಕ end_cycle, CallbackFunction cb)
{
	clk_signals = ಸಙ್ಕೇತ;
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
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, ಏತತ್, nullptr);
	ಯದಿ (last_time!=end_time && curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(last_time);
		curr_cycle++;
	}
	ಯದಿ (curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(end_time);
		curr_cycle++;
	}
}

std::string FstData::ಮೌಲ್ಯ_Of(fstHandle ಸಙ್ಕೇತ)
{
	ಯದಿ (past_data.find(ಸಙ್ಕೇತ) == past_data.end()) {
		ಪ್ರತ್ಯಾಗಮ std::string(handle_to_var[ಸಙ್ಕೇತ].width, 'x');
	}
	ಪ್ರತ್ಯಾಗಮ past_data[ಸಙ್ಕೇತ];
}
