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
ساكن std::string file_بنسټ_name(std::string ثابت & لار_2)
{
	أرجع لار_2.substr(لار_2.پیدا_کړه_وروستی_د("/\\") + 1);
}
#د_که_پای

Fst_معلومات::Fst_معلومات(std::string filename) : ctx(تش_اشاره)
{
	#إذا defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_بنسټ_name(filename);
	إذا (filename_trim.اندازه() > 4 && filename_trim.compare(filename_trim.اندازه()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.اندازه()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_بنسټ_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		لوګاریتم("Exec: %s\n", cmd);
		إذا (پرلپسې_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#د_که_پای
	ثابت std::ویکټور<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
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
	ایستل_Var_نومونه();
}

Fst_معلومات::~Fst_معلومات()
{
	إذا (ctx)
		fstReaderClose(ctx);
	إذا (!tmp_file.تشه())
		remove(tmp_file.c_str());
}

uint64_t Fst_معلومات::get_پیل_وخت() { أرجع fst_Reader_Get_پیل_وخت(ctx); }

uint64_t Fst_معلومات::get_پای_ټکی_وخت() { أرجع fst_Reader_Get_پای_ټکی_وخت(ctx); }

ساكن فراغ normalize_brackets(std::string &str)
{
	لكل (خپلکاری &c : str) {
		إذا (c == '<')
			c = '[';
		وإلا إذا (c == '>')
			c = ']';
	}
}

fstHandle Fst_معلومات::getHandle(std::string name) {
	normalize_brackets(name);
	إذا (name_منزل_handle.پیدا_کړه(name) != name_منزل_handle.پای_ټکی())
		أرجع name_منزل_handle[name];
	وإلا
		أرجع 0;
};

dict<صحيح,fstHandle> Fst_معلومات::get_حافظه_Handles(std::string name) {
	إذا (حافظه_منزل_handle.پیدا_کړه(name) != حافظه_منزل_handle.پای_ټکی())
		أرجع حافظه_منزل_handle[name];
	وإلا
		أرجع dict<صحيح,fstHandle>();
};

ساكن std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.پای_ټکی(), ' '), str.پای_ټکی());
	أرجع str;
}

فراغ Fst_معلومات::ایستل_Var_نومونه()
{
	بنية fstHier *h;
	std::string fst_scope_name;

	طالما ((h = fstReaderIterateHier(ctx))) {
		بدل (h->htyp) {
			حالة FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_ټېل_Scope(ctx, h->u.scope.name, عدم);
				اكسر;
			}
			حالة FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_راوباسه_Scope(ctx);
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
				var.پلنوالی = h->u.var.اوږدتیا;
				vars.ټېل_شا(var);
				إذا (!var.is_alias)
					handle_منزل_var[h->u.var.handle] = var;
				std::string clean_name;
				منطقي has_space = خطأ;
				لكل(اندازه_t i=0;i<مزي_اوږدوالی(h->u.var.name);i++)
				{
					حرف c = h->u.var.name[i];
					إذا(c==' ') { has_space = ریښتیا; اكسر; }
					clean_name += c;
				}
				إذا (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				إذا (!has_space) {
					اندازه_t pos = clean_name.پیدا_کړه_وروستی_د("[");
					std::string شاخص_or_range = clean_name.substr(pos+1);
					إذا (شاخص_or_range.پیدا_کړه(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				اندازه_t pos = clean_name.پیدا_کړه_وروستی_د("<");
				إذا (pos != std::string::npos && clean_name.شا() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.راوباسه_شا(); // remove closing bracket
					حرف *endptr;
					صحيح mem_addr = strtol(addr.c_str(), &endptr, 16);
					إذا (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} وإلا {
						حافظه_منزل_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.پیدا_کړه_وروستی_د("[");
				إذا (pos != std::string::npos && clean_name.شا() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.راوباسه_شا(); // remove closing bracket
					حرف *endptr;
					صحيح mem_addr = strtol(addr.c_str(), &endptr, 10);
					إذا (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} وإلا {
						حافظه_منزل_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_منزل_handle[var.scope+"."+clean_name] = h->u.var.handle;
				اكسر;
			}
		}
	}
}


ساكن فراغ reconstruct_clb_varlen_attimes(فراغ *user_معلومات, uint64_t pnt_وخت, fstHandle pnt_facidx, ثابت بینښه حرف *pnt_ارزښت, uint32_t plen)
{
	Fst_معلومات *ptr = (Fst_معلومات*)user_معلومات;
	ptr->reconstruct_callback_attimes(pnt_وخت, pnt_facidx, pnt_ارزښت, plen);
}

ساكن فراغ reconstruct_clb_attimes(فراغ *user_معلومات, uint64_t pnt_وخت, fstHandle pnt_facidx, ثابت بینښه حرف *pnt_ارزښت)
{
	Fst_معلومات *ptr = (Fst_معلومات*)user_معلومات;
	uint32_t plen = (pnt_ارزښت) ?  مزي_اوږدوالی((ثابت حرف *)pnt_ارزښت) : 0;
	ptr->reconstruct_callback_attimes(pnt_وخت, pnt_facidx, pnt_ارزښت, plen);
}

فراغ Fst_معلومات::reconstruct_callback_attimes(uint64_t pnt_وخت, fstHandle pnt_facidx, ثابت بینښه حرف *pnt_ارزښت, uint32_t /* plen */)
{
	إذا (pnt_وخت > پای_ټکی_وخت || !pnt_ارزښت) أرجع;
	إذا (curr_دوره > وروستی_دوره) أرجع;
	// if we are past the timestamp
	منطقي is_clock = خطأ;
	إذا (!all_samples) {
		لكل(خپلکاری &s : clk_سیګنالونه) {
			إذا (s==pnt_facidx)  {
				is_clock=ریښتیا;
				اكسر;
			}
		}
	}

	إذا (pnt_وخت > past_وخت) {
		past_معلومات = وروستی_معلومات;
		past_وخت = pnt_وخت;
	}

	إذا (pnt_وخت > وروستی_وخت) {
		إذا (all_samples) {
			callback(وروستی_وخت);
			curr_دوره++;
			وروستی_وخت = pnt_وخت;
		} وإلا {
			إذا (is_clock) {
				std::string val = std::string((ثابت حرف *)pnt_ارزښت);
				std::string prev = past_معلومات[pnt_facidx];
				إذا ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(وروستی_وخت);
					curr_دوره++;
					وروستی_وخت = pnt_وخت;
				}
			}
		}
	}
	// always update last_data
	وروستی_معلومات[pnt_facidx] =  std::string((ثابت حرف *)pnt_ارزښت);
}

فراغ Fst_معلومات::reconstructAllAtTimes(std::ویکټور<fstHandle> &إشارة, uint64_t پیل, uint64_t پای_ټکی, بینښه صحيح پای_ټکی_دوره, CallbackFunction cb)
{
	clk_سیګنالونه = إشارة;
	callback = cb;
	پیل_وخت = پیل;
	پای_ټکی_وخت = پای_ټکی;
	curr_دوره = 0;
	وروستی_دوره = پای_ټکی_دوره;
	وروستی_معلومات.clear();
	وروستی_وخت = پیل_وخت;
	past_معلومات.clear();
	past_وخت = پیل_وخت;
	all_samples = clk_سیګنالونه.تشه();

	fst_Reader_سېټ_Unlimited_وخت_Range(ctx);
	fst_Reader_سېټ_Fac_Process_ماسک_All(ctx);
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, هذا, تش_اشاره);
	إذا (وروستی_وخت!=پای_ټکی_وخت && curr_دوره <= وروستی_دوره) {
		past_معلومات = وروستی_معلومات;
		callback(وروستی_وخت);
		curr_دوره++;
	}
	إذا (curr_دوره <= وروستی_دوره) {
		past_معلومات = وروستی_معلومات;
		callback(پای_ټکی_وخت);
		curr_دوره++;
	}
}

std::string Fst_معلومات::ارزښت_د(fstHandle إشارة)
{
	إذا (past_معلومات.پیدا_کړه(إشارة) == past_معلومات.پای_ټکی()) {
		أرجع std::string(handle_منزل_var[إشارة].پلنوالی, 'x');
	}
	أرجع past_معلومات[إشارة];
}
