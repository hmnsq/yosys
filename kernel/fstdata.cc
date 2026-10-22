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
ساكن std::string file_پایه_name(std::string ثابت & مسیر_2)
{
	أرجع مسیر_2.substr(مسیر_2.پیدا_کن_آخرین_از("/\\") + 1);
}
#پایان_اگر

Fst_داده::Fst_داده(std::string filename) : ctx(اشاره_تهی)
{
	#إذا defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_پایه_name(filename);
	إذا (filename_trim.اندازه() > 4 && filename_trim.compare(filename_trim.اندازه()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.اندازه()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_پایه_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		لگاریتم("Exec: %s\n", cmd);
		إذا (پیاپی_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#پایان_اگر
	ثابت std::وکتور<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
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
	استخراج_Var_نامها();
}

Fst_داده::~Fst_داده()
{
	إذا (ctx)
		fstReaderClose(ctx);
	إذا (!tmp_file.خالی())
		remove(tmp_file.c_str());
}

uint64_t Fst_داده::get_شروع_زمان() { أرجع fst_Reader_Get_شروع_زمان(ctx); }

uint64_t Fst_داده::get_انتها_زمان() { أرجع fst_Reader_Get_انتها_زمان(ctx); }

ساكن فراغ normalize_brackets(std::string &str)
{
	لكل (خودکار &c : str) {
		إذا (c == '<')
			c = '[';
		وإلا إذا (c == '>')
			c = ']';
	}
}

fstHandle Fst_داده::getHandle(std::string name) {
	normalize_brackets(name);
	إذا (name_مقصد_handle.پیدا_کن(name) != name_مقصد_handle.انتها())
		أرجع name_مقصد_handle[name];
	وإلا
		أرجع 0;
};

dict<صحيح,fstHandle> Fst_داده::get_حافظه_Handles(std::string name) {
	إذا (حافظه_مقصد_handle.پیدا_کن(name) != حافظه_مقصد_handle.انتها())
		أرجع حافظه_مقصد_handle[name];
	وإلا
		أرجع dict<صحيح,fstHandle>();
};

ساكن std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.انتها(), ' '), str.انتها());
	أرجع str;
}

فراغ Fst_داده::استخراج_Var_نامها()
{
	بنية fstHier *h;
	std::string fst_scope_name;

	طالما ((h = fstReaderIterateHier(ctx))) {
		بدل (h->htyp) {
			حالة FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_افزودن_Scope(ctx, h->u.scope.name, عدم);
				اكسر;
			}
			حالة FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_برداشت_Scope(ctx);
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
				var.عرض = h->u.var.درازی;
				vars.افزودن_پشت(var);
				إذا (!var.is_alias)
					handle_مقصد_var[h->u.var.handle] = var;
				std::string clean_name;
				منطقي has_space = خطأ;
				لكل(اندازه_t i=0;i<طول_رشته(h->u.var.name);i++)
				{
					حرف c = h->u.var.name[i];
					إذا(c==' ') { has_space = درست; اكسر; }
					clean_name += c;
				}
				إذا (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				إذا (!has_space) {
					اندازه_t pos = clean_name.پیدا_کن_آخرین_از("[");
					std::string شاخص_or_range = clean_name.substr(pos+1);
					إذا (شاخص_or_range.پیدا_کن(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				اندازه_t pos = clean_name.پیدا_کن_آخرین_از("<");
				إذا (pos != std::string::npos && clean_name.پشت() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.برداشت_پشت(); // remove closing bracket
					حرف *endptr;
					صحيح mem_addr = strtol(addr.c_str(), &endptr, 16);
					إذا (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} وإلا {
						حافظه_مقصد_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.پیدا_کن_آخرین_از("[");
				إذا (pos != std::string::npos && clean_name.پشت() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.برداشت_پشت(); // remove closing bracket
					حرف *endptr;
					صحيح mem_addr = strtol(addr.c_str(), &endptr, 10);
					إذا (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} وإلا {
						حافظه_مقصد_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_مقصد_handle[var.scope+"."+clean_name] = h->u.var.handle;
				اكسر;
			}
		}
	}
}


ساكن فراغ reconstruct_clb_varlen_attimes(فراغ *user_داده, uint64_t pnt_زمان, fstHandle pnt_facidx, ثابت بیعلامت حرف *pnt_مقدار, uint32_t plen)
{
	Fst_داده *ptr = (Fst_داده*)user_داده;
	ptr->reconstruct_callback_attimes(pnt_زمان, pnt_facidx, pnt_مقدار, plen);
}

ساكن فراغ reconstruct_clb_attimes(فراغ *user_داده, uint64_t pnt_زمان, fstHandle pnt_facidx, ثابت بیعلامت حرف *pnt_مقدار)
{
	Fst_داده *ptr = (Fst_داده*)user_داده;
	uint32_t plen = (pnt_مقدار) ?  طول_رشته((ثابت حرف *)pnt_مقدار) : 0;
	ptr->reconstruct_callback_attimes(pnt_زمان, pnt_facidx, pnt_مقدار, plen);
}

فراغ Fst_داده::reconstruct_callback_attimes(uint64_t pnt_زمان, fstHandle pnt_facidx, ثابت بیعلامت حرف *pnt_مقدار, uint32_t /* plen */)
{
	إذا (pnt_زمان > انتها_زمان || !pnt_مقدار) أرجع;
	إذا (curr_چرخه > آخرین_چرخه) أرجع;
	// if we are past the timestamp
	منطقي is_clock = خطأ;
	إذا (!all_samples) {
		لكل(خودکار &s : clk_سیگنالها) {
			إذا (s==pnt_facidx)  {
				is_clock=درست;
				اكسر;
			}
		}
	}

	إذا (pnt_زمان > past_زمان) {
		past_داده = آخرین_داده;
		past_زمان = pnt_زمان;
	}

	إذا (pnt_زمان > آخرین_زمان) {
		إذا (all_samples) {
			callback(آخرین_زمان);
			curr_چرخه++;
			آخرین_زمان = pnt_زمان;
		} وإلا {
			إذا (is_clock) {
				std::string val = std::string((ثابت حرف *)pnt_مقدار);
				std::string prev = past_داده[pnt_facidx];
				إذا ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(آخرین_زمان);
					curr_چرخه++;
					آخرین_زمان = pnt_زمان;
				}
			}
		}
	}
	// always update last_data
	آخرین_داده[pnt_facidx] =  std::string((ثابت حرف *)pnt_مقدار);
}

فراغ Fst_داده::reconstructAllAtTimes(std::وکتور<fstHandle> &إشارة, uint64_t شروع, uint64_t انتها, بیعلامت صحيح انتها_چرخه, CallbackFunction cb)
{
	clk_سیگنالها = إشارة;
	callback = cb;
	شروع_زمان = شروع;
	انتها_زمان = انتها;
	curr_چرخه = 0;
	آخرین_چرخه = انتها_چرخه;
	آخرین_داده.clear();
	آخرین_زمان = شروع_زمان;
	past_داده.clear();
	past_زمان = شروع_زمان;
	all_samples = clk_سیگنالها.خالی();

	fst_Reader_مجموعه_Unlimited_زمان_Range(ctx);
	fst_Reader_مجموعه_Fac_Process_ماسک_All(ctx);
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, هذا, اشاره_تهی);
	إذا (آخرین_زمان!=انتها_زمان && curr_چرخه <= آخرین_چرخه) {
		past_داده = آخرین_داده;
		callback(آخرین_زمان);
		curr_چرخه++;
	}
	إذا (curr_چرخه <= آخرین_چرخه) {
		past_داده = آخرین_داده;
		callback(انتها_زمان);
		curr_چرخه++;
	}
}

std::string Fst_داده::مقدار_از(fstHandle إشارة)
{
	إذا (past_داده.پیدا_کن(إشارة) == past_داده.انتها()) {
		أرجع std::string(handle_مقصد_var[إشارة].عرض, 'x');
	}
	أرجع past_داده[إشارة];
}
