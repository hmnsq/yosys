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

#समावेश "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#अगर defined(YOSYS_ENABLE_SPAWN)
अटल std::string file_base_name(std::string स्थिर & path)
{
	वापस path.substr(path.find_last_of("/\\") + 1);
}
#पूर्ण_अगर

Fst_तथ्य::Fst_तथ्य(std::string filename) : ctx(nullptr)
{
	#अगर defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_base_name(filename);
	अगर (filename_trim.आकार() > 4 && filename_trim.compare(filename_trim.आकार()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.आकार()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_base_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		log("Exec: %s\n", cmd);
		अगर (run_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#पूर्ण_अगर
	स्थिर std::vector<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	अगर (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (पूर्णांक)fstReaderGetTimescale(ctx);
	timescale_str = "";
	पूर्णांक unit = 0;
	पूर्णांक zeros = 0;
	अगर (scale > 0)  {
		zeros = scale;
	} अन्यथा {
		अगर ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} अन्यथा {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	क्रम (पूर्णांक i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extractVarNames();
}

Fst_तथ्य::~Fst_तथ्य()
{
	अगर (ctx)
		fstReaderClose(ctx);
	अगर (!tmp_file.empty())
		हटाओ(tmp_file.c_str());
}

uint64_t Fst_तथ्य::getStartTime() { वापस fstReaderGetStartTime(ctx); }

uint64_t Fst_तथ्य::getEndTime() { वापस fstReaderGetEndTime(ctx); }

अटल व्योम normalize_brackets(std::string &str)
{
	क्रम (स्वतः &c : str) {
		अगर (c == '<')
			c = '[';
		अन्यथा अगर (c == '>')
			c = ']';
	}
}

fstHandle Fst_तथ्य::getHandle(std::string name) {
	normalize_brackets(name);
	अगर (name_to_handle.find(name) != name_to_handle.end())
		वापस name_to_handle[name];
	अन्यथा
		वापस 0;
};

dict<पूर्णांक,fstHandle> Fst_तथ्य::getMemoryHandles(std::string name) {
	अगर (memory_to_handle.find(name) != memory_to_handle.end())
		वापस memory_to_handle[name];
	अन्यथा
		वापस dict<पूर्णांक,fstHandle>();
};

अटल std::string remove_spaces(std::string str)
{
	str.erase(std::हटाओ(str.begin(), str.end(), ' '), str.end());
	वापस str;
}

व्योम Fst_तथ्य::extractVarNames()
{
	काष्ठा fstHier *h;
	std::string fst_scope_name;

	जबतक ((h = fstReaderIterateHier(ctx))) {
		चयन (h->htyp) {
			हाल FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_धकेलो_Scope(ctx, h->u.scope.name, शून्य);
				अवरोध;
			}
			हाल FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_निकालो_Scope(ctx);
				अवरोध;
			}
			हाल FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.width = h->u.var.लंबाई;
				vars.धकेलो_back(var);
				अगर (!var.is_alias)
					handle_to_var[h->u.var.handle] = var;
				std::string clean_name;
				bool has_space = false;
				क्रम(माप_प्रकार i=0;i<म_माप(h->u.var.name);i++)
				{
					अक्षर c = h->u.var.name[i];
					अगर(c==' ') { has_space = true; अवरोध; }
					clean_name += c;
				}
				अगर (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				अगर (!has_space) {
					माप_प्रकार pos = clean_name.find_last_of("[");
					std::string सूचकांक_or_range = clean_name.substr(pos+1);
					अगर (सूचकांक_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				माप_प्रकार pos = clean_name.find_last_of("<");
				अगर (pos != std::string::npos && clean_name.back() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.निकालो_back(); // remove closing bracket
					अक्षर *endptr;
					पूर्णांक mem_addr = म_से_दीर्घ(addr.c_str(), &endptr, 16);
					अगर (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} अन्यथा {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_last_of("[");
				अगर (pos != std::string::npos && clean_name.back() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.निकालो_back(); // remove closing bracket
					अक्षर *endptr;
					पूर्णांक mem_addr = म_से_दीर्घ(addr.c_str(), &endptr, 10);
					अगर (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} अन्यथा {
						memory_to_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_to_handle[var.scope+"."+clean_name] = h->u.var.handle;
				अवरोध;
			}
		}
	}
}


अटल व्योम reconstruct_clb_varlen_attimes(व्योम *user_तथ्य, uint64_t pnt_time, fstHandle pnt_facidx, स्थिर अचिन्हित अक्षर *pnt_मूल्य, uint32_t plen)
{
	Fst_तथ्य *ptr = (Fst_तथ्य*)user_तथ्य;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_मूल्य, plen);
}

अटल व्योम reconstruct_clb_attimes(व्योम *user_तथ्य, uint64_t pnt_time, fstHandle pnt_facidx, स्थिर अचिन्हित अक्षर *pnt_मूल्य)
{
	Fst_तथ्य *ptr = (Fst_तथ्य*)user_तथ्य;
	uint32_t plen = (pnt_मूल्य) ?  म_माप((स्थिर अक्षर *)pnt_मूल्य) : 0;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_मूल्य, plen);
}

व्योम Fst_तथ्य::reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, स्थिर अचिन्हित अक्षर *pnt_मूल्य, uint32_t /* plen */)
{
	अगर (pnt_time > end_time || !pnt_मूल्य) वापस;
	अगर (curr_cycle > last_cycle) वापस;
	// if we are past the timestamp
	bool is_clock = false;
	अगर (!all_samples) {
		क्रम(स्वतः &s : clk_signals) {
			अगर (s==pnt_facidx)  {
				is_clock=true;
				अवरोध;
			}
		}
	}

	अगर (pnt_time > past_time) {
		past_तथ्य = last_तथ्य;
		past_time = pnt_time;
	}

	अगर (pnt_time > last_time) {
		अगर (all_samples) {
			callback(last_time);
			curr_cycle++;
			last_time = pnt_time;
		} अन्यथा {
			अगर (is_clock) {
				std::string val = std::string((स्थिर अक्षर *)pnt_मूल्य);
				std::string पूर्व = past_तथ्य[pnt_facidx];
				अगर ((पूर्व!="1" && val=="1") || (पूर्व!="0" && val=="0")) {
					callback(last_time);
					curr_cycle++;
					last_time = pnt_time;
				}
			}
		}
	}
	// always update last_data
	last_तथ्य[pnt_facidx] =  std::string((स्थिर अक्षर *)pnt_मूल्य);
}

व्योम Fst_तथ्य::reconstructAllAtTimes(std::vector<fstHandle> &संकेत, uint64_t start, uint64_t end, अचिन्हित पूर्णांक end_cycle, CallbackFunction cb)
{
	clk_signals = संकेत;
	callback = cb;
	start_time = start;
	end_time = end;
	curr_cycle = 0;
	last_cycle = end_cycle;
	last_तथ्य.सफा();
	last_time = start_time;
	past_तथ्य.सफा();
	past_time = start_time;
	all_samples = clk_signals.empty();

	fstReaderSetUnlimitedTimeRange(ctx);
	fstReaderSetFacProcessMaskAll(ctx);
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, यह, nullptr);
	अगर (last_time!=end_time && curr_cycle <= last_cycle) {
		past_तथ्य = last_तथ्य;
		callback(last_time);
		curr_cycle++;
	}
	अगर (curr_cycle <= last_cycle) {
		past_तथ्य = last_तथ्य;
		callback(end_time);
		curr_cycle++;
	}
}

std::string Fst_तथ्य::मूल्य_Of(fstHandle संकेत)
{
	अगर (past_तथ्य.find(संकेत) == past_तथ्य.end()) {
		वापस std::string(handle_to_var[संकेत].width, 'x');
	}
	वापस past_तथ्य[संकेत];
}
