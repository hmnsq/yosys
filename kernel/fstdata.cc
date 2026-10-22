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
ساكن std::string file_بنياد_name(std::string ثابت & رستو_2)
{
	أرجع رستو_2.substr(رستو_2.ڳوليو_آخري_جو("/\\") + 1);
}
#جيڪڏهن_پورو

Fst_معلومات::Fst_معلومات(std::string filename) : ctx(خالي_اشارو)
{
	#إذا defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_بنياد_name(filename);
	إذا (filename_trim.سائيز() > 4 && filename_trim.compare(filename_trim.سائيز()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.سائيز()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_بنياد_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		لاگرٿم("Exec: %s\n", cmd);
		إذا (لاڳيتو_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#جيڪڏهن_پورو
	ثابت std::ويڪٽر<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
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
	ڪڍو_Var_نالا();
}

Fst_معلومات::~Fst_معلومات()
{
	إذا (ctx)
		fstReaderClose(ctx);
	إذا (!tmp_file.خالي_جايون())
		remove(tmp_file.c_str());
}

uint64_t Fst_معلومات::get_شروعات_وقت() { أرجع fst_Reader_Get_شروعات_وقت(ctx); }

uint64_t Fst_معلومات::get_پڇاڙي_وقت() { أرجع fst_Reader_Get_پڇاڙي_وقت(ctx); }

ساكن فراغ normalize_brackets(std::string &str)
{
	لكل (خودڪار &c : str) {
		إذا (c == '<')
			c = '[';
		وإلا إذا (c == '>')
			c = ']';
	}
}

fstHandle Fst_معلومات::getHandle(std::string name) {
	normalize_brackets(name);
	إذا (name_منزل_handle.ڳوليو(name) != name_منزل_handle.پڇاڙي())
		أرجع name_منزل_handle[name];
	وإلا
		أرجع 0;
};

dict<صحيح,fstHandle> Fst_معلومات::get_يادگيري_Handles(std::string name) {
	إذا (يادگيري_منزل_handle.ڳوليو(name) != يادگيري_منزل_handle.پڇاڙي())
		أرجع يادگيري_منزل_handle[name];
	وإلا
		أرجع dict<صحيح,fstHandle>();
};

ساكن std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.پڇاڙي(), ' '), str.پڇاڙي());
	أرجع str;
}

فراغ Fst_معلومات::ڪڍو_Var_نالا()
{
	بنية fstHier *h;
	std::string fst_scope_name;

	طالما ((h = fstReaderIterateHier(ctx))) {
		بدل (h->htyp) {
			حالة FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_ڌڪو_Scope(ctx, h->u.scope.name, عدم);
				اكسر;
			}
			حالة FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_ڪڍو_Scope(ctx);
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
				var.ويڪر = h->u.var.ڊيگهائي;
				vars.ڌڪو_پٺ(var);
				إذا (!var.is_alias)
					handle_منزل_var[h->u.var.handle] = var;
				std::string clean_name;
				منطقي has_space = خطأ;
				لكل(سائيز_t i=0;i<لڙي_ڊيگهه(h->u.var.name);i++)
				{
					حرف c = h->u.var.name[i];
					إذا(c==' ') { has_space = سچ; اكسر; }
					clean_name += c;
				}
				إذا (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				إذا (!has_space) {
					سائيز_t pos = clean_name.ڳوليو_آخري_جو("[");
					std::string اشاريو_or_range = clean_name.substr(pos+1);
					إذا (اشاريو_or_range.ڳوليو(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				سائيز_t pos = clean_name.ڳوليو_آخري_جو("<");
				إذا (pos != std::string::npos && clean_name.پٺ() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.ڪڍو_پٺ(); // remove closing bracket
					حرف *endptr;
					صحيح mem_addr = strtol(addr.c_str(), &endptr, 16);
					إذا (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} وإلا {
						يادگيري_منزل_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.ڳوليو_آخري_جو("[");
				إذا (pos != std::string::npos && clean_name.پٺ() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.ڪڍو_پٺ(); // remove closing bracket
					حرف *endptr;
					صحيح mem_addr = strtol(addr.c_str(), &endptr, 10);
					إذا (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} وإلا {
						يادگيري_منزل_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_منزل_handle[var.scope+"."+clean_name] = h->u.var.handle;
				اكسر;
			}
		}
	}
}


ساكن فراغ reconstruct_clb_varlen_attimes(فراغ *user_معلومات, uint64_t pnt_وقت, fstHandle pnt_facidx, ثابت بنانشان حرف *pnt_قدر, uint32_t plen)
{
	Fst_معلومات *ptr = (Fst_معلومات*)user_معلومات;
	ptr->reconstruct_callback_attimes(pnt_وقت, pnt_facidx, pnt_قدر, plen);
}

ساكن فراغ reconstruct_clb_attimes(فراغ *user_معلومات, uint64_t pnt_وقت, fstHandle pnt_facidx, ثابت بنانشان حرف *pnt_قدر)
{
	Fst_معلومات *ptr = (Fst_معلومات*)user_معلومات;
	uint32_t plen = (pnt_قدر) ?  لڙي_ڊيگهه((ثابت حرف *)pnt_قدر) : 0;
	ptr->reconstruct_callback_attimes(pnt_وقت, pnt_facidx, pnt_قدر, plen);
}

فراغ Fst_معلومات::reconstruct_callback_attimes(uint64_t pnt_وقت, fstHandle pnt_facidx, ثابت بنانشان حرف *pnt_قدر, uint32_t /* plen */)
{
	إذا (pnt_وقت > پڇاڙي_وقت || !pnt_قدر) أرجع;
	إذا (curr_چڪر > آخري_چڪر) أرجع;
	// if we are past the timestamp
	منطقي is_clock = خطأ;
	إذا (!all_samples) {
		لكل(خودڪار &s : clk_سگنل) {
			إذا (s==pnt_facidx)  {
				is_clock=سچ;
				اكسر;
			}
		}
	}

	إذا (pnt_وقت > past_وقت) {
		past_معلومات = آخري_معلومات;
		past_وقت = pnt_وقت;
	}

	إذا (pnt_وقت > آخري_وقت) {
		إذا (all_samples) {
			callback(آخري_وقت);
			curr_چڪر++;
			آخري_وقت = pnt_وقت;
		} وإلا {
			إذا (is_clock) {
				std::string val = std::string((ثابت حرف *)pnt_قدر);
				std::string prev = past_معلومات[pnt_facidx];
				إذا ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(آخري_وقت);
					curr_چڪر++;
					آخري_وقت = pnt_وقت;
				}
			}
		}
	}
	// always update last_data
	آخري_معلومات[pnt_facidx] =  std::string((ثابت حرف *)pnt_قدر);
}

فراغ Fst_معلومات::reconstructAllAtTimes(std::ويڪٽر<fstHandle> &إشارة, uint64_t شروعات, uint64_t پڇاڙي, بنانشان صحيح پڇاڙي_چڪر, CallbackFunction cb)
{
	clk_سگنل = إشارة;
	callback = cb;
	شروعات_وقت = شروعات;
	پڇاڙي_وقت = پڇاڙي;
	curr_چڪر = 0;
	آخري_چڪر = پڇاڙي_چڪر;
	آخري_معلومات.clear();
	آخري_وقت = شروعات_وقت;
	past_معلومات.clear();
	past_وقت = شروعات_وقت;
	all_samples = clk_سگنل.خالي_جايون();

	fst_Reader_سيٽ_Unlimited_وقت_Range(ctx);
	fst_Reader_سيٽ_Fac_Process_نقاب_All(ctx);
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, هذا, خالي_اشارو);
	إذا (آخري_وقت!=پڇاڙي_وقت && curr_چڪر <= آخري_چڪر) {
		past_معلومات = آخري_معلومات;
		callback(آخري_وقت);
		curr_چڪر++;
	}
	إذا (curr_چڪر <= آخري_چڪر) {
		past_معلومات = آخري_معلومات;
		callback(پڇاڙي_وقت);
		curr_چڪر++;
	}
}

std::string Fst_معلومات::قدر_جو(fstHandle إشارة)
{
	إذا (past_معلومات.ڳوليو(إشارة) == past_معلومات.پڇاڙي()) {
		أرجع std::string(handle_منزل_var[إشارة].ويڪر, 'x');
	}
	أرجع past_معلومات[إشارة];
}
