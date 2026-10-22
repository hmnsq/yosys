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

#अन्तर्भाव "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#यदि defined(YOSYS_ENABLE_SPAWN)
स्थिर std::string file_base_name(std::string अचर & path)
{
	फर्काऊ path.substr(path.find_last_of("/\\") + 1);
}
#endif

FstData::FstData(std::string filename) : ctx(nullptr)
{
	#यदि defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_base_name(filename);
	यदि (filename_trim.size() > 4 && filename_trim.compare(filename_trim.size()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.size()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_base_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		log("Exec: %s\n", cmd);
		यदि (run_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#endif
	अचर std::vector<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	यदि (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (पूर्णाङ्क)fstReaderGetTimescale(ctx);
	timescale_str = "";
	पूर्णाङ्क unit = 0;
	पूर्णाङ्क zeros = 0;
	यदि (scale > 0)  {
		zeros = scale;
	} अन्यथा {
		यदि ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} अन्यथा {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	लागि (पूर्णाङ्क i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extractVarNames();
}

FstData::~FstData()
{
	यदि (ctx)
		fstReaderClose(ctx);
	यदि (!tmp_file.empty())
		हटाओ(tmp_file.c_str());
}

uint64_t FstData::getStartTime() { फर्काऊ fstReaderGetStartTime(ctx); }

uint64_t FstData::getEndTime() { फर्काऊ fstReaderGetEndTime(ctx); }

स्थिर शून्य normalize_brackets(std::string &str)
{
	लागि (स्वतः &c : str) {
		यदि (c == '<')
			c = '[';
		अन्यथा यदि (c == '>')
			c = ']';
	}
}

fstHandle FstData::getHandle(std::string name) {
	normalize_brackets(name);
	यदि (name_to_handle.find(name) != name_to_handle.end())
		फर्काऊ name_to_handle[name];
	अन्यथा
		फर्काऊ 0;
};

dict<पूर्णाङ्क,fstHandle> FstData::getMemoryHandles(std::string name) {
	यदि (memory_to_handle.find(name) != memory_to_handle.end())
		फर्काऊ memory_to_handle[name];
	अन्यथा
		फर्काऊ dict<पूर्णाङ्क,fstHandle>();
};

स्थिर std::string remove_spaces(std::string str)
{
	str.erase(std::हटाओ(str.begin(), str.end(), ' '), str.end());
	फर्काऊ str;
}

शून्य FstData::extractVarNames()
{
	संरचना fstHier *h;
	std::string fst_scope_name;

	जबसम्म ((h = fstReaderIterateHier(ctx))) {
		विकल्प (h->htyp) {
			पक्ष FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_थप_Scope(ctx, h->u.scope.name, रिक्त);
				रोक;
			}
			पक्ष FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_झिक_Scope(ctx);
				रोक;
			}
			पक्ष FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.width = h->u.var.length;
				vars.थप_back(var);
				यदि (!var.is_alias)
					handle_to_var[h->u.var.handle] = var;
				std::string clean_name;
				तार्किक has_space = असत्य;
				लागि(माप_प्रकार i=0;i<म_माप(h->u.var.name);i++)
				{
					वर्ण c = h->u.var.name[i];
					यदि(c==' ') { has_space = सत्य; रोक; }
					clean_name += c;
				}
				यदि (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				यदि (!has_space) {
					माप_प्रकार pos = clean_name.find_last_of("[");
					std::string index_or_range = clean_name.substr(pos+1);
					यदि (index_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				माप_प्रकार pos = clean_name.find_last_of("<");
				यदि (pos != std::string::npos && clean_name.back() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.झिक_back(); // remove closing bracket
					वर्ण *endptr;
					पूर्णाङ्क mem_addr = म_से_दीर्घ(addr.c_str(), &endptr, 16);
					यदि (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} अन्यथा {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_last_of("[");
				यदि (pos != std::string::npos && clean_name.back() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.झिक_back(); // remove closing bracket
					वर्ण *endptr;
					पूर्णाङ्क mem_addr = म_से_दीर्घ(addr.c_str(), &endptr, 10);
					यदि (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} अन्यथा {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_to_handle[var.scope+"."+clean_name] = h->u.var.handle;
				रोक;
			}
		}
	}
}


स्थिर शून्य reconstruct_clb_varlen_attimes(शून्य *user_data, uint64_t pnt_time, fstHandle pnt_facidx, अचर अचिन्हित वर्ण *pnt_मान, uint32_t plen)
{
	FstData *ptr = (FstData*)user_data;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_मान, plen);
}

स्थिर शून्य reconstruct_clb_attimes(शून्य *user_data, uint64_t pnt_time, fstHandle pnt_facidx, अचर अचिन्हित वर्ण *pnt_मान)
{
	FstData *ptr = (FstData*)user_data;
	uint32_t plen = (pnt_मान) ?  म_माप((अचर वर्ण *)pnt_मान) : 0;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_मान, plen);
}

शून्य FstData::reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, अचर अचिन्हित वर्ण *pnt_मान, uint32_t /* plen */)
{
	यदि (pnt_time > end_time || !pnt_मान) फर्काऊ;
	यदि (curr_cycle > last_cycle) फर्काऊ;
	// if we are past the timestamp
	तार्किक is_clock = असत्य;
	यदि (!all_samples) {
		लागि(स्वतः &s : clk_signals) {
			यदि (s==pnt_facidx)  {
				is_clock=सत्य;
				रोक;
			}
		}
	}

	यदि (pnt_time > past_time) {
		past_data = last_data;
		past_time = pnt_time;
	}

	यदि (pnt_time > last_time) {
		यदि (all_samples) {
			callback(last_time);
			curr_cycle++;
			last_time = pnt_time;
		} अन्यथा {
			यदि (is_clock) {
				std::string val = std::string((अचर वर्ण *)pnt_मान);
				std::string prev = past_data[pnt_facidx];
				यदि ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(last_time);
					curr_cycle++;
					last_time = pnt_time;
				}
			}
		}
	}
	// always update last_data
	last_data[pnt_facidx] =  std::string((अचर वर्ण *)pnt_मान);
}

शून्य FstData::reconstructAllAtTimes(std::vector<fstHandle> &सङ्केत, uint64_t start, uint64_t end, अचिन्हित पूर्णाङ्क end_cycle, CallbackFunction cb)
{
	clk_signals = सङ्केत;
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
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, एतत्, nullptr);
	यदि (last_time!=end_time && curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(last_time);
		curr_cycle++;
	}
	यदि (curr_cycle <= last_cycle) {
		past_data = last_data;
		callback(end_time);
		curr_cycle++;
	}
}

std::string FstData::मान_Of(fstHandle सङ्केत)
{
	यदि (past_data.find(सङ्केत) == past_data.end()) {
		फर्काऊ std::string(handle_to_var[सङ्केत].width, 'x');
	}
	फर्काऊ past_data[सङ्केत];
}
