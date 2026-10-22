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

#شامل "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#اگر defined(YOSYS_ENABLE_SPAWN)
ساکن std::string file_بنیاد_name(std::string ثابت & راستہ_2)
{
	واپس راستہ_2.substr(راستہ_2.ڈھونڈو_آخری_کا("/\\") + 1);
}
#اگر_ختم

Fst_ڈیٹا::Fst_ڈیٹا(std::string filename) : ctx(خالی_اشاریہ)
{
	#اگر defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_بنیاد_name(filename);
	اگر (filename_trim.سائز() > 4 && filename_trim.compare(filename_trim.سائز()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.سائز()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_بنیاد_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		لوگارتھم("Exec: %s\n", cmd);
		اگر (تسلسل_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#اگر_ختم
	ثابت std::سمتیہ<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	اگر (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (صحیح)fstReaderGetTimescale(ctx);
	timescale_str = "";
	صحیح unit = 0;
	صحیح zeros = 0;
	اگر (scale > 0)  {
		zeros = scale;
	} ورنہ {
		اگر ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} ورنہ {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	برائے (صحیح i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	نکالو_Var_نام();
}

Fst_ڈیٹا::~Fst_ڈیٹا()
{
	اگر (ctx)
		fstReaderClose(ctx);
	اگر (!tmp_file.خالی_خانے())
		remove(tmp_file.c_str());
}

uint64_t Fst_ڈیٹا::get_آغاز_وقت() { واپس fst_Reader_Get_آغاز_وقت(ctx); }

uint64_t Fst_ڈیٹا::get_انتہا_وقت() { واپس fst_Reader_Get_انتہا_وقت(ctx); }

ساکن خالی normalize_brackets(std::string &str)
{
	برائے (خودکار &c : str) {
		اگر (c == '<')
			c = '[';
		ورنہ اگر (c == '>')
			c = ']';
	}
}

fstHandle Fst_ڈیٹا::getHandle(std::string name) {
	normalize_brackets(name);
	اگر (name_مقصد_handle.ڈھونڈو(name) != name_مقصد_handle.انتہا())
		واپس name_مقصد_handle[name];
	ورنہ
		واپس 0;
};

dict<صحیح,fstHandle> Fst_ڈیٹا::get_حافظہ_Handles(std::string name) {
	اگر (حافظہ_مقصد_handle.ڈھونڈو(name) != حافظہ_مقصد_handle.انتہا())
		واپس حافظہ_مقصد_handle[name];
	ورنہ
		واپس dict<صحیح,fstHandle>();
};

ساکن std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.انتہا(), ' '), str.انتہا());
	واپس str;
}

خالی Fst_ڈیٹا::نکالو_Var_نام()
{
	ساخت fstHier *h;
	std::string fst_scope_name;

	جب ((h = fstReaderIterateHier(ctx))) {
		انتخاب (h->htyp) {
			حالت FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_ڈالو_Scope(ctx, h->u.scope.name, تہی);
				توڑو;
			}
			حالت FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_نکالو_Scope(ctx);
				توڑو;
			}
			حالت FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.چوڑائی = h->u.var.لمبائی_2;
				vars.ڈالو_عقب(var);
				اگر (!var.is_alias)
					handle_مقصد_var[h->u.var.handle] = var;
				std::string clean_name;
				منطقی has_space = جھوٹ;
				برائے(سائز_t i=0;i<لڑی_لمبائی(h->u.var.name);i++)
				{
					حرف c = h->u.var.name[i];
					اگر(c==' ') { has_space = سچ; توڑو; }
					clean_name += c;
				}
				اگر (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				اگر (!has_space) {
					سائز_t pos = clean_name.ڈھونڈو_آخری_کا("[");
					std::string اشاریہ_or_range = clean_name.substr(pos+1);
					اگر (اشاریہ_or_range.ڈھونڈو(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				سائز_t pos = clean_name.ڈھونڈو_آخری_کا("<");
				اگر (pos != std::string::npos && clean_name.عقب() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.نکالو_عقب(); // remove closing bracket
					حرف *endptr;
					صحیح mem_addr = strtol(addr.c_str(), &endptr, 16);
					اگر (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} ورنہ {
						حافظہ_مقصد_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.ڈھونڈو_آخری_کا("[");
				اگر (pos != std::string::npos && clean_name.عقب() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.نکالو_عقب(); // remove closing bracket
					حرف *endptr;
					صحیح mem_addr = strtol(addr.c_str(), &endptr, 10);
					اگر (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} ورنہ {
						حافظہ_مقصد_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_مقصد_handle[var.scope+"."+clean_name] = h->u.var.handle;
				توڑو;
			}
		}
	}
}


ساکن خالی reconstruct_clb_varlen_attimes(خالی *user_ڈیٹا, uint64_t pnt_وقت, fstHandle pnt_facidx, ثابت غیرعلامتی حرف *pnt_قدر, uint32_t plen)
{
	Fst_ڈیٹا *ptr = (Fst_ڈیٹا*)user_ڈیٹا;
	ptr->reconstruct_callback_attimes(pnt_وقت, pnt_facidx, pnt_قدر, plen);
}

ساکن خالی reconstruct_clb_attimes(خالی *user_ڈیٹا, uint64_t pnt_وقت, fstHandle pnt_facidx, ثابت غیرعلامتی حرف *pnt_قدر)
{
	Fst_ڈیٹا *ptr = (Fst_ڈیٹا*)user_ڈیٹا;
	uint32_t plen = (pnt_قدر) ?  لڑی_لمبائی((ثابت حرف *)pnt_قدر) : 0;
	ptr->reconstruct_callback_attimes(pnt_وقت, pnt_facidx, pnt_قدر, plen);
}

خالی Fst_ڈیٹا::reconstruct_callback_attimes(uint64_t pnt_وقت, fstHandle pnt_facidx, ثابت غیرعلامتی حرف *pnt_قدر, uint32_t /* plen */)
{
	اگر (pnt_وقت > انتہا_وقت || !pnt_قدر) واپس;
	اگر (curr_چکر > آخری_چکر) واپس;
	// if we are past the timestamp
	منطقی is_clock = جھوٹ;
	اگر (!all_samples) {
		برائے(خودکار &s : clk_اشارات) {
			اگر (s==pnt_facidx)  {
				is_clock=سچ;
				توڑو;
			}
		}
	}

	اگر (pnt_وقت > past_وقت) {
		past_ڈیٹا = آخری_ڈیٹا;
		past_وقت = pnt_وقت;
	}

	اگر (pnt_وقت > آخری_وقت) {
		اگر (all_samples) {
			callback(آخری_وقت);
			curr_چکر++;
			آخری_وقت = pnt_وقت;
		} ورنہ {
			اگر (is_clock) {
				std::string val = std::string((ثابت حرف *)pnt_قدر);
				std::string پچھلا = past_ڈیٹا[pnt_facidx];
				اگر ((پچھلا!="1" && val=="1") || (پچھلا!="0" && val=="0")) {
					callback(آخری_وقت);
					curr_چکر++;
					آخری_وقت = pnt_وقت;
				}
			}
		}
	}
	// always update last_data
	آخری_ڈیٹا[pnt_facidx] =  std::string((ثابت حرف *)pnt_قدر);
}

خالی Fst_ڈیٹا::reconstructAllAtTimes(std::سمتیہ<fstHandle> &اشارہ, uint64_t آغاز, uint64_t انتہا, غیرعلامتی صحیح انتہا_چکر, CallbackFunction cb)
{
	clk_اشارات = اشارہ;
	callback = cb;
	آغاز_وقت = آغاز;
	انتہا_وقت = انتہا;
	curr_چکر = 0;
	آخری_چکر = انتہا_چکر;
	آخری_ڈیٹا.clear();
	آخری_وقت = آغاز_وقت;
	past_ڈیٹا.clear();
	past_وقت = آغاز_وقت;
	all_samples = clk_اشارات.خالی_خانے();

	fst_Reader_سیٹ_Unlimited_وقت_Range(ctx);
	fst_Reader_سیٹ_Fac_Process_نقاب_All(ctx);
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, یہ, خالی_اشاریہ);
	اگر (آخری_وقت!=انتہا_وقت && curr_چکر <= آخری_چکر) {
		past_ڈیٹا = آخری_ڈیٹا;
		callback(آخری_وقت);
		curr_چکر++;
	}
	اگر (curr_چکر <= آخری_چکر) {
		past_ڈیٹا = آخری_ڈیٹا;
		callback(انتہا_وقت);
		curr_چکر++;
	}
}

std::string Fst_ڈیٹا::قدر_کا(fstHandle اشارہ)
{
	اگر (past_ڈیٹا.ڈھونڈو(اشارہ) == past_ڈیٹا.انتہا()) {
		واپس std::string(handle_مقصد_var[اشارہ].چوڑائی, 'x');
	}
	واپس past_ڈیٹا[اشارہ];
}
