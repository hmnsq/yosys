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

#অন্তর্ভাব "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#যদি defined(YOSYS_ENABLE_SPAWN)
স্থির std::string file_base_name(std::string ধ্রুবক & path)
{
	ফেরত path.substr(path.find_last_of("/\\") + 1);
}
#endif

Fst_তথ্য::Fst_তথ্য(std::string filename) : ctx(nullptr)
{
	#যদি defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_base_name(filename);
	যদি (filename_trim.আকার_2() > 4 && filename_trim.compare(filename_trim.আকার_2()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.আকার_2()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_base_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		log("Exec: %s\n", cmd);
		যদি (run_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#endif
	ধ্রুবক std::vector<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	যদি (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (পূর্ণসংখ্যা)fstReaderGetTimescale(ctx);
	timescale_str = "";
	পূর্ণসংখ্যা unit = 0;
	পূর্ণসংখ্যা zeros = 0;
	যদি (scale > 0)  {
		zeros = scale;
	} অন্যথা {
		যদি ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} অন্যথা {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	জন্য (পূর্ণসংখ্যা i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extractVarNames();
}

Fst_তথ্য::~Fst_তথ্য()
{
	যদি (ctx)
		fstReaderClose(ctx);
	যদি (!tmp_file.empty())
		remove(tmp_file.c_str());
}

uint64_t Fst_তথ্য::getStartTime() { ফেরত fstReaderGetStartTime(ctx); }

uint64_t Fst_তথ্য::getEndTime() { ফেরত fstReaderGetEndTime(ctx); }

স্থির শূন্য normalize_brackets(std::string &str)
{
	জন্য (auto &c : str) {
		যদি (c == '<')
			c = '[';
		অন্যথা যদি (c == '>')
			c = ']';
	}
}

fstHandle Fst_তথ্য::getHandle(std::string name) {
	normalize_brackets(name);
	যদি (name_to_handle.find(name) != name_to_handle.end())
		ফেরত name_to_handle[name];
	অন্যথা
		ফেরত 0;
};

dict<পূর্ণসংখ্যা,fstHandle> Fst_তথ্য::getMemoryHandles(std::string name) {
	যদি (memory_to_handle.find(name) != memory_to_handle.end())
		ফেরত memory_to_handle[name];
	অন্যথা
		ফেরত dict<পূর্ণসংখ্যা,fstHandle>();
};

স্থির std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	ফেরত str;
}

শূন্য Fst_তথ্য::extractVarNames()
{
	কাঠামো fstHier *h;
	std::string fst_scope_name;

	যখন ((h = fstReaderIterateHier(ctx))) {
		নির্বাচন (h->htyp) {
			ক্ষেত্র FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_ঠেলো_Scope(ctx, h->u.scope.name, ফাঁকা);
				বিরতি;
			}
			ক্ষেত্র FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_বের_করো_Scope(ctx);
				বিরতি;
			}
			ক্ষেত্র FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.width = h->u.var.দৈর্ঘ্য;
				vars.ঠেলো_back(var);
				যদি (!var.is_alias)
					handle_to_var[h->u.var.handle] = var;
				std::string clean_name;
				যৌক্তিক has_space = মিথ্যা;
				জন্য(আকার_t i=0;i<strlen(h->u.var.name);i++)
				{
					অক্ষর c = h->u.var.name[i];
					যদি(c==' ') { has_space = সত্য; বিরতি; }
					clean_name += c;
				}
				যদি (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				যদি (!has_space) {
					আকার_t pos = clean_name.find_last_of("[");
					std::string সূচক_or_range = clean_name.substr(pos+1);
					যদি (সূচক_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				আকার_t pos = clean_name.find_last_of("<");
				যদি (pos != std::string::npos && clean_name.back() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.বের_করো_back(); // remove closing bracket
					অক্ষর *endptr;
					পূর্ণসংখ্যা mem_addr = strtol(addr.c_str(), &endptr, 16);
					যদি (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} অন্যথা {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_last_of("[");
				যদি (pos != std::string::npos && clean_name.back() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.বের_করো_back(); // remove closing bracket
					অক্ষর *endptr;
					পূর্ণসংখ্যা mem_addr = strtol(addr.c_str(), &endptr, 10);
					যদি (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} অন্যথা {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_to_handle[var.scope+"."+clean_name] = h->u.var.handle;
				বিরতি;
			}
		}
	}
}


স্থির শূন্য reconstruct_clb_varlen_attimes(শূন্য *user_তথ্য, uint64_t pnt_time, fstHandle pnt_facidx, ধ্রুবক unsigned অক্ষর *pnt_মান, uint32_t plen)
{
	Fst_তথ্য *ptr = (Fst_তথ্য*)user_তথ্য;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_মান, plen);
}

স্থির শূন্য reconstruct_clb_attimes(শূন্য *user_তথ্য, uint64_t pnt_time, fstHandle pnt_facidx, ধ্রুবক unsigned অক্ষর *pnt_মান)
{
	Fst_তথ্য *ptr = (Fst_তথ্য*)user_তথ্য;
	uint32_t plen = (pnt_মান) ?  strlen((ধ্রুবক অক্ষর *)pnt_মান) : 0;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_মান, plen);
}

শূন্য Fst_তথ্য::reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, ধ্রুবক unsigned অক্ষর *pnt_মান, uint32_t /* plen */)
{
	যদি (pnt_time > end_time || !pnt_মান) ফেরত;
	যদি (curr_cycle > last_cycle) ফেরত;
	// if we are past the timestamp
	যৌক্তিক is_clock = মিথ্যা;
	যদি (!all_samples) {
		জন্য(auto &s : clk_signals) {
			যদি (s==pnt_facidx)  {
				is_clock=সত্য;
				বিরতি;
			}
		}
	}

	যদি (pnt_time > past_time) {
		past_তথ্য = last_তথ্য;
		past_time = pnt_time;
	}

	যদি (pnt_time > last_time) {
		যদি (all_samples) {
			callback(last_time);
			curr_cycle++;
			last_time = pnt_time;
		} অন্যথা {
			যদি (is_clock) {
				std::string val = std::string((ধ্রুবক অক্ষর *)pnt_মান);
				std::string পূর্ব = past_তথ্য[pnt_facidx];
				যদি ((পূর্ব!="1" && val=="1") || (পূর্ব!="0" && val=="0")) {
					callback(last_time);
					curr_cycle++;
					last_time = pnt_time;
				}
			}
		}
	}
	// always update last_data
	last_তথ্য[pnt_facidx] =  std::string((ধ্রুবক অক্ষর *)pnt_মান);
}

শূন্য Fst_তথ্য::reconstructAllAtTimes(std::vector<fstHandle> &সংকেত, uint64_t start, uint64_t end, unsigned পূর্ণসংখ্যা end_cycle, CallbackFunction cb)
{
	clk_signals = সংকেত;
	callback = cb;
	start_time = start;
	end_time = end;
	curr_cycle = 0;
	last_cycle = end_cycle;
	last_তথ্য.clear();
	last_time = start_time;
	past_তথ্য.clear();
	past_time = start_time;
	all_samples = clk_signals.empty();

	fstReaderSetUnlimitedTimeRange(ctx);
	fstReaderSetFacProcessMaskAll(ctx);
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, এই, nullptr);
	যদি (last_time!=end_time && curr_cycle <= last_cycle) {
		past_তথ্য = last_তথ্য;
		callback(last_time);
		curr_cycle++;
	}
	যদি (curr_cycle <= last_cycle) {
		past_তথ্য = last_তথ্য;
		callback(end_time);
		curr_cycle++;
	}
}

std::string Fst_তথ্য::মান_Of(fstHandle সংকেত)
{
	যদি (past_তথ্য.find(সংকেত) == past_তথ্য.end()) {
		ফেরত std::string(handle_to_var[সংকেত].width, 'x');
	}
	ফেরত past_তথ্য[সংকেত];
}
