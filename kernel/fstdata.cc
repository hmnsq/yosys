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

#અન્તર્ભાવ "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#જો defined(YOSYS_ENABLE_SPAWN)
સ્થિર std::string file_base_name(std::string અચલ & path)
{
	પાછા path.substr(path.find_last_of("/\\") + 1);
}
#endif

FstData::FstData(std::string filename) : ctx(nullptr)
{
	#જો defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_base_name(filename);
	જો (filename_trim.size() > 4 && filename_trim.compare(filename_trim.size()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.size()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_base_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		log("Exec: %s\n", cmd);
		જો (run_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#endif
	અચલ std::vector<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	જો (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (પૂર્ણાંક)fstReaderGetTimescale(ctx);
	timescale_str = "";
	પૂર્ણાંક unit = 0;
	પૂર્ણાંક zeros = 0;
	જો (scale > 0)  {
		zeros = scale;
	} નહીંતર {
		જો ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} નહીંતર {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	માટે (પૂર્ણાંક i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extractVarNames();
}

FstData::~FstData()
{
	જો (ctx)
		fstReaderClose(ctx);
	જો (!tmp_file.empty())
		remove(tmp_file.c_str());
}

uint64_t FstData::getStartTime() { પાછા fstReaderGetStartTime(ctx); }

uint64_t FstData::getEndTime() { પાછા fstReaderGetEndTime(ctx); }

સ્થિર શૂન્ય normalize_brackets(std::string &str)
{
	માટે (auto &c : str) {
		જો (c == '<')
			c = '[';
		નહીંતર જો (c == '>')
			c = ']';
	}
}

fstHandle FstData::getHandle(std::string name) {
	normalize_brackets(name);
	જો (name_to_handle.find(name) != name_to_handle.end())
		પાછા name_to_handle[name];
	નહીંતર
		પાછા 0;
};

dict<પૂર્ણાંક,fstHandle> FstData::getMemoryHandles(std::string name) {
	જો (memory_to_handle.find(name) != memory_to_handle.end())
		પાછા memory_to_handle[name];
	નહીંતર
		પાછા dict<પૂર્ણાંક,fstHandle>();
};

સ્થિર std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	પાછા str;
}

શૂન્ય FstData::extractVarNames()
{
	સંરચના fstHier *h;
	std::string fst_scope_name;

	જ્યારે ((h = fstReaderIterateHier(ctx))) {
		પસંદગી (h->htyp) {
			સ્થિતિ FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_ધકેલો_Scope(ctx, h->u.scope.name, ખાલી);
				તોડો;
			}
			સ્થિતિ FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_કાઢો_Scope(ctx);
				તોડો;
			}
			સ્થિતિ FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.width = h->u.var.length;
				vars.ધકેલો_back(var);
				જો (!var.is_alias)
					handle_to_var[h->u.var.handle] = var;
				std::string clean_name;
				તાર્કિક has_space = ખોટું;
				માટે(size_t i=0;i<strlen(h->u.var.name);i++)
				{
					અક્ષર c = h->u.var.name[i];
					જો(c==' ') { has_space = સાચું; તોડો; }
					clean_name += c;
				}
				જો (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				જો (!has_space) {
					size_t pos = clean_name.find_last_of("[");
					std::string index_or_range = clean_name.substr(pos+1);
					જો (index_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				size_t pos = clean_name.find_last_of("<");
				જો (pos != std::string::npos && clean_name.back() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.કાઢો_back(); // remove closing bracket
					અક્ષર *endptr;
					પૂર્ણાંક mem_addr = strtol(addr.c_str(), &endptr, 16);
					જો (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} નહીંતર {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_last_of("[");
				જો (pos != std::string::npos && clean_name.back() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.કાઢો_back(); // remove closing bracket
					અક્ષર *endptr;
					પૂર્ણાંક mem_addr = strtol(addr.c_str(), &endptr, 10);
					જો (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} નહીંતર {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_to_handle[var.scope+"."+clean_name] = h->u.var.handle;
				તોડો;
			}
		}
	}
}


સ્થિર શૂન્ય reconstruct_clb_varlen_attimes(શૂન્ય *user_data, uint64_t pnt_time, fstHandle pnt_facidx, અચલ unsigned અક્ષર *pnt_કિંમત, uint32_t plen)
{
	FstData *ptr = (FstData*)user_data;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_કિંમત, plen);
}

સ્થિર શૂન્ય reconstruct_clb_attimes(શૂન્ય *user_data, uint64_t pnt_time, fstHandle pnt_facidx, અચલ unsigned અક્ષર *pnt_કિંમત)
{
	FstData *ptr = (FstData*)user_data;
	uint32_t plen = (pnt_કિંમત) ?  strlen((અચલ અક્ષર *)pnt_કિંમત) : 0;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_કિંમત, plen);
}

શૂન્ય FstData::reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, અચલ unsigned અક્ષર *pnt_કિંમત, uint32_t /* plen */)
{
	જો (pnt_time > end_time || !pnt_કિંમત) પાછા;
	જો (curr_cycle > last_cycle) પાછા;
	// if we are past the timestamp
	તાર્કિક is_clock = ખોટું;
	જો (!all_samples) {
		માટે(auto &s : clk_signals) {
			જો (s==pnt_facidx)  {
				is_clock=સાચું;
				તોડો;
			}
		}
	}

	જો (pnt_time > past_time) {
		past_data = last_data;
		past_time = pnt_time;
	}

	જો (pnt_time > last_time) {
		જો (all_samples) {
			callback(last_time);
			curr_cycle++;
			last_time = pnt_time;
		} નહીંતર {
			જો (is_clock) {
				std::string val = std::string((અચલ અક્ષર *)pnt_કિંમત);
				std::string prev = past_data[pnt_facidx];
				જો ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(last_time);
					curr_cycle++;
					last_time = pnt_time;
				}
			}
		}
	}
	// always update last_data
	last_data[pnt_facidx] =  std::string((અચલ અક્ષર *)pnt_કિંમત);
}

શૂન્ય FstData::reconstructAllAtTimes(std::vector<fstHandle> &સંકેત, uint64_t start, uint64_t end, unsigned પૂર્ણાંક end_cycle, CallbackFunction cb)
{
	clk_signals = સંકેત;
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
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, આ, nullptr);
	જો (last_time!=end_time && curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(last_time);
		curr_cycle++;
	}
	જો (curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(end_time);
		curr_cycle++;
	}
}

std::string FstData::કિંમત_Of(fstHandle સંકેત)
{
	જો (past_data.find(સંકેત) == past_data.end()) {
		પાછા std::string(handle_to_var[સંકેત].width, 'x');
	}
	પાછા past_data[સંકેત];
}
