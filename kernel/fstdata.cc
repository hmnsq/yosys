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

#ضمن "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#إذا defined(YOSYS_ENABLE_SPAWN)
ساكن std::string file_ادھار_name(std::string ثابت & رستہ_2)
{
	أرجع رستہ_2.substr(رستہ_2.لبھو_آخری_دا("/\\") + 1);
}
#جے_انت

Fst_انکڑے::Fst_انکڑے(std::string filename) : ctx(خالی_سوچک)
{
	#إذا defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_ادھار_name(filename);
	إذا (filename_trim.ناپ() > 4 && filename_trim.compare(filename_trim.ناپ()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.ناپ()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_ادھار_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		لوگارتھم("Exec: %s\n", cmd);
		إذا (لگاتار_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#جے_انت
	ثابت std::سمتیہ<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	إذا (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (صحيح)fstReaderGetTimescale(ctx);
	timescale_str = "";
	صحيح unit = 0;
	صحيح zeros = 0;
	إذا (scale > 0)  {
		zeros = scale;
	} وإلا {
		إذا ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} وإلا {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	لكل (صحيح i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	کڈھو_Var_ناں();
}

Fst_انکڑے::~Fst_انکڑے()
{
	إذا (ctx)
		fstReaderClose(ctx);
	إذا (!tmp_file.خالی_تھاواں())
		remove(tmp_file.c_str());
}

uint64_t Fst_انکڑے::get_شروع_ویلا() { أرجع fst_Reader_Get_شروع_ویلا(ctx); }

uint64_t Fst_انکڑے::get_سرا_ویلا() { أرجع fst_Reader_Get_سرا_ویلا(ctx); }

ساكن فراغ normalize_brackets(std::string &str)
{
	لكل (خودکار &c : str) {
		إذا (c == '<')
			c = '[';
		وإلا إذا (c == '>')
			c = ']';
	}
}

fstHandle Fst_انکڑے::getHandle(std::string name) {
	normalize_brackets(name);
	إذا (name_منزل_handle.لبھو(name) != name_منزل_handle.سرا())
		أرجع name_منزل_handle[name];
	وإلا
		أرجع 0;
};

dict<صحيح,fstHandle> Fst_انکڑے::get_حافظہ_Handles(std::string name) {
	إذا (حافظہ_منزل_handle.لبھو(name) != حافظہ_منزل_handle.سرا())
		أرجع حافظہ_منزل_handle[name];
	وإلا
		أرجع dict<صحيح,fstHandle>();
};

ساكن std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.سرا(), ' '), str.سرا());
	أرجع str;
}

فراغ Fst_انکڑے::کڈھو_Var_ناں()
{
	بنية fstHier *h;
	std::string fst_scope_name;

	طالما ((h = fstReaderIterateHier(ctx))) {
		بدل (h->htyp) {
			حالة FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_ڈالو_Scope(ctx, h->u.scope.name, عدم);
				اكسر;
			}
			حالة FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_نکالو_Scope(ctx);
				اكسر;
			}
			حالة FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.چوڑائی = h->u.var.لمائی;
				vars.ڈالو_پٹھ(var);
				إذا (!var.is_alias)
					handle_منزل_var[h->u.var.handle] = var;
				std::string clean_name;
				منطقي has_space = خطأ;
				لكل(ناپ_t i=0;i<لڑی_لمبائی(h->u.var.name);i++)
				{
					حرف c = h->u.var.name[i];
					إذا(c==' ') { has_space = سچ; اكسر; }
					clean_name += c;
				}
				إذا (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				إذا (!has_space) {
					ناپ_t pos = clean_name.لبھو_آخری_دا("[");
					std::string اشاریہ_or_range = clean_name.substr(pos+1);
					إذا (اشاریہ_or_range.لبھو(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				ناپ_t pos = clean_name.لبھو_آخری_دا("<");
				إذا (pos != std::string::npos && clean_name.پٹھ() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.نکالو_پٹھ(); // remove closing bracket
					حرف *endptr;
					صحيح mem_addr = strtol(addr.c_str(), &endptr, 16);
					إذا (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} وإلا {
						حافظہ_منزل_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.لبھو_آخری_دا("[");
				إذا (pos != std::string::npos && clean_name.پٹھ() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.نکالو_پٹھ(); // remove closing bracket
					حرف *endptr;
					صحيح mem_addr = strtol(addr.c_str(), &endptr, 10);
					إذا (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} وإلا {
						حافظہ_منزل_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_منزل_handle[var.scope+"."+clean_name] = h->u.var.handle;
				اكسر;
			}
		}
	}
}


ساكن فراغ reconstruct_clb_varlen_attimes(فراغ *user_انکڑے, uint64_t pnt_ویلا, fstHandle pnt_facidx, ثابت بنانشانی حرف *pnt_قدر, uint32_t plen)
{
	Fst_انکڑے *ptr = (Fst_انکڑے*)user_انکڑے;
	ptr->reconstruct_callback_attimes(pnt_ویلا, pnt_facidx, pnt_قدر, plen);
}

ساكن فراغ reconstruct_clb_attimes(فراغ *user_انکڑے, uint64_t pnt_ویلا, fstHandle pnt_facidx, ثابت بنانشانی حرف *pnt_قدر)
{
	Fst_انکڑے *ptr = (Fst_انکڑے*)user_انکڑے;
	uint32_t plen = (pnt_قدر) ?  لڑی_لمبائی((ثابت حرف *)pnt_قدر) : 0;
	ptr->reconstruct_callback_attimes(pnt_ویلا, pnt_facidx, pnt_قدر, plen);
}

فراغ Fst_انکڑے::reconstruct_callback_attimes(uint64_t pnt_ویلا, fstHandle pnt_facidx, ثابت بنانشانی حرف *pnt_قدر, uint32_t /* plen */)
{
	إذا (pnt_ویلا > سرا_ویلا || !pnt_قدر) أرجع;
	إذا (curr_چکر > آخری_چکر) أرجع;
	// if we are past the timestamp
	منطقي is_clock = خطأ;
	إذا (!all_samples) {
		لكل(خودکار &s : clk_اشارے) {
			إذا (s==pnt_facidx)  {
				is_clock=سچ;
				اكسر;
			}
		}
	}

	إذا (pnt_ویلا > past_ویلا) {
		past_انکڑے = آخری_انکڑے;
		past_ویلا = pnt_ویلا;
	}

	إذا (pnt_ویلا > آخری_ویلا) {
		إذا (all_samples) {
			callback(آخری_ویلا);
			curr_چکر++;
			آخری_ویلا = pnt_ویلا;
		} وإلا {
			إذا (is_clock) {
				std::string val = std::string((ثابت حرف *)pnt_قدر);
				std::string prev = past_انکڑے[pnt_facidx];
				إذا ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(آخری_ویلا);
					curr_چکر++;
					آخری_ویلا = pnt_ویلا;
				}
			}
		}
	}
	// always update last_data
	آخری_انکڑے[pnt_facidx] =  std::string((ثابت حرف *)pnt_قدر);
}

فراغ Fst_انکڑے::reconstructAllAtTimes(std::سمتیہ<fstHandle> &إشارة, uint64_t شروع, uint64_t سرا, بنانشانی صحيح سرا_چکر, CallbackFunction cb)
{
	clk_اشارے = إشارة;
	callback = cb;
	شروع_ویلا = شروع;
	سرا_ویلا = سرا;
	curr_چکر = 0;
	آخری_چکر = سرا_چکر;
	آخری_انکڑے.clear();
	آخری_ویلا = شروع_ویلا;
	past_انکڑے.clear();
	past_ویلا = شروع_ویلا;
	all_samples = clk_اشارے.خالی_تھاواں();

	fst_Reader_سیٹ_Unlimited_ویلا_Range(ctx);
	fst_Reader_سیٹ_Fac_Process_نقاب_All(ctx);
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, هذا, خالی_سوچک);
	إذا (آخری_ویلا!=سرا_ویلا && curr_چکر <= آخری_چکر) {
		past_انکڑے = آخری_انکڑے;
		callback(آخری_ویلا);
		curr_چکر++;
	}
	إذا (curr_چکر <= آخری_چکر) {
		past_انکڑے = آخری_انکڑے;
		callback(سرا_ویلا);
		curr_چکر++;
	}
}

std::string Fst_انکڑے::قدر_دا(fstHandle إشارة)
{
	إذا (past_انکڑے.لبھو(إشارة) == past_انکڑے.سرا()) {
		أرجع std::string(handle_منزل_var[إشارة].چوڑائی, 'x');
	}
	أرجع past_انکڑے[إشارة];
}
