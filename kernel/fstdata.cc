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
स्थिर std::string file_आधारः_name(std::string ध्रुव & मार्गः_2)
{
	प्रत्यागम मार्गः_2.substr(मार्गः_2.find_चरमः_सम्बन्धि("/\\") + 1);
}
#यदि_अन्तः

Fst_दत्त::Fst_दत्त(std::string filename) : ctx(शून्यसूचकः)
{
	#यदि defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_आधारः_name(filename);
	यदि (filename_trim.आकार() > 4 && filename_trim.compare(filename_trim.आकार()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.आकार()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_आधारः_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		लघुगणकः("Exec: %s\n", cmd);
		यदि (सततम्_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#यदि_अन्तः
	ध्रुव std::सदिशम्<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
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
	प्रति (पूर्णाङ्क i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	extract_Var_नामानि();
}

Fst_दत्त::~Fst_दत्त()
{
	यदि (ctx)
		fstReaderClose(ctx);
	यदि (!tmp_file.रिक्तानि())
		remove(tmp_file.c_str());
}

uint64_t Fst_दत्त::getStartTime() { प्रत्यागम fstReaderGetStartTime(ctx); }

uint64_t Fst_दत्त::get_अन्तः_Time() { प्रत्यागम fst_Reader_Get_अन्तः_Time(ctx); }

स्थिर शून्य normalize_brackets(std::string &str)
{
	प्रति (स्वचालित &c : str) {
		यदि (c == '<')
			c = '[';
		अन्यथा यदि (c == '>')
			c = ']';
	}
}

fstHandle Fst_दत्त::getHandle(std::string name) {
	normalize_brackets(name);
	यदि (name_गन्तव्यम्_handle.find(name) != name_गन्तव्यम्_handle.अन्तः())
		प्रत्यागम name_गन्तव्यम्_handle[name];
	अन्यथा
		प्रत्यागम 0;
};

dict<पूर्णाङ्क,fstHandle> Fst_दत्त::get_स्मृतिः_Handles(std::string name) {
	यदि (स्मृतिः_गन्तव्यम्_handle.find(name) != स्मृतिः_गन्तव्यम्_handle.अन्तः())
		प्रत्यागम स्मृतिः_गन्तव्यम्_handle[name];
	अन्यथा
		प्रत्यागम dict<पूर्णाङ्क,fstHandle>();
};

स्थिर std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.अन्तः(), ' '), str.अन्तः());
	प्रत्यागम str;
}

शून्य Fst_दत्त::extract_Var_नामानि()
{
	संरचना fstHier *h;
	std::string fst_scope_name;

	यावत् ((h = fstReaderIterateHier(ctx))) {
		विकल्प (h->htyp) {
			पक्ष FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_प्रक्षिप_Scope(ctx, h->u.scope.name, रिक्त);
				भङ्ग;
			}
			पक्ष FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_निष्कासय_Scope(ctx);
				भङ्ग;
			}
			पक्ष FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.विस्तारः = h->u.var.दीर्घता_2;
				vars.प्रक्षिप_पृष्ठभागः(var);
				यदि (!var.is_alias)
					handle_गन्तव्यम्_var[h->u.var.handle] = var;
				std::string clean_name;
				तार्किक has_space = असत्य;
				प्रति(आकार_t i=0;i<वाक्यदैर्घ्यम्(h->u.var.name);i++)
				{
					वर्ण c = h->u.var.name[i];
					यदि(c==' ') { has_space = सत्य; भङ्ग; }
					clean_name += c;
				}
				यदि (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				यदि (!has_space) {
					आकार_t pos = clean_name.find_चरमः_सम्बन्धि("[");
					std::string सूचकांक_or_range = clean_name.substr(pos+1);
					यदि (सूचकांक_or_range.find(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				आकार_t pos = clean_name.find_चरमः_सम्बन्धि("<");
				यदि (pos != std::string::npos && clean_name.पृष्ठभागः() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.निष्कासय_पृष्ठभागः(); // remove closing bracket
					वर्ण *endptr;
					पूर्णाङ्क mem_addr = strtol(addr.c_str(), &endptr, 16);
					यदि (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} अन्यथा {
						स्मृतिः_गन्तव्यम्_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.find_चरमः_सम्बन्धि("[");
				यदि (pos != std::string::npos && clean_name.पृष्ठभागः() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.निष्कासय_पृष्ठभागः(); // remove closing bracket
					वर्ण *endptr;
					पूर्णाङ्क mem_addr = strtol(addr.c_str(), &endptr, 10);
					यदि (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} अन्यथा {
						स्मृतिः_गन्तव्यम्_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_गन्तव्यम्_handle[var.scope+"."+clean_name] = h->u.var.handle;
				भङ्ग;
			}
		}
	}
}


स्थिर शून्य reconstruct_clb_varlen_attimes(शून्य *user_दत्त, uint64_t pnt_time, fstHandle pnt_facidx, ध्रुव अचिह्नित वर्ण *pnt_मानम्, uint32_t plen)
{
	Fst_दत्त *ptr = (Fst_दत्त*)user_दत्त;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_मानम्, plen);
}

स्थिर शून्य reconstruct_clb_attimes(शून्य *user_दत्त, uint64_t pnt_time, fstHandle pnt_facidx, ध्रुव अचिह्नित वर्ण *pnt_मानम्)
{
	Fst_दत्त *ptr = (Fst_दत्त*)user_दत्त;
	uint32_t plen = (pnt_मानम्) ?  वाक्यदैर्घ्यम्((ध्रुव वर्ण *)pnt_मानम्) : 0;
	ptr->reconstruct_callback_attimes(pnt_time, pnt_facidx, pnt_मानम्, plen);
}

शून्य Fst_दत्त::reconstruct_callback_attimes(uint64_t pnt_time, fstHandle pnt_facidx, ध्रुव अचिह्नित वर्ण *pnt_मानम्, uint32_t /* plen */)
{
	यदि (pnt_time > अन्तः_time || !pnt_मानम्) प्रत्यागम;
	यदि (curr_आवृत्तिः > चरमः_आवृत्तिः) प्रत्यागम;
	// if we are past the timestamp
	तार्किक is_clock = असत्य;
	यदि (!all_samples) {
		प्रति(स्वचालित &s : clk_सङ्केताः) {
			यदि (s==pnt_facidx)  {
				is_clock=सत्य;
				भङ्ग;
			}
		}
	}

	यदि (pnt_time > past_time) {
		past_दत्त = चरमः_दत्त;
		past_time = pnt_time;
	}

	यदि (pnt_time > चरमः_time) {
		यदि (all_samples) {
			callback(चरमः_time);
			curr_आवृत्तिः++;
			चरमः_time = pnt_time;
		} अन्यथा {
			यदि (is_clock) {
				std::string val = std::string((ध्रुव वर्ण *)pnt_मानम्);
				std::string पूर्व = past_दत्त[pnt_facidx];
				यदि ((पूर्व!="1" && val=="1") || (पूर्व!="0" && val=="0")) {
					callback(चरमः_time);
					curr_आवृत्तिः++;
					चरमः_time = pnt_time;
				}
			}
		}
	}
	// always update last_data
	चरमः_दत्त[pnt_facidx] =  std::string((ध्रुव वर्ण *)pnt_मानम्);
}

शून्य Fst_दत्त::reconstructAllAtTimes(std::सदिशम्<fstHandle> &सङ्केत, uint64_t start, uint64_t अन्तः, अचिह्नित पूर्णाङ्क अन्तः_आवृत्तिः, CallbackFunction cb)
{
	clk_सङ्केताः = सङ्केत;
	callback = cb;
	start_time = start;
	अन्तः_time = अन्तः;
	curr_आवृत्तिः = 0;
	चरमः_आवृत्तिः = अन्तः_आवृत्तिः;
	चरमः_दत्त.clear();
	चरमः_time = start_time;
	past_दत्त.clear();
	past_time = start_time;
	all_samples = clk_सङ्केताः.रिक्तानि();

	fst_Reader_समुच्चयः_Unlimited_Time_Range(ctx);
	fst_Reader_समुच्चयः_Fac_Process_आवरणम्_All(ctx);
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, एतत्, शून्यसूचकः);
	यदि (चरमः_time!=अन्तः_time && curr_आवृत्तिः <= चरमः_आवृत्तिः) {
		past_दत्त = चरमः_दत्त;
		callback(चरमः_time);
		curr_आवृत्तिः++;
	}
	यदि (curr_आवृत्तिः <= चरमः_आवृत्तिः) {
		past_दत्त = चरमः_दत्त;
		callback(अन्तः_time);
		curr_आवृत्तिः++;
	}
}

std::string Fst_दत्त::मानम्_सम्बन्धि(fstHandle सङ्केत)
{
	यदि (past_दत्त.find(सङ्केत) == past_दत्त.अन्तः()) {
		प्रत्यागम std::string(handle_गन्तव्यम्_var[सङ्केत].विस्तारः, 'x');
	}
	प्रत्यागम past_दत्त[सङ्केत];
}
