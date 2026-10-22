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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#ਜੇ defined(YOSYS_ENABLE_SPAWN)
ਸਥਿਰ std::string file_ਅਧਾਰ_name(std::string ਅਚਲ & ਰਸਤਾ_2)
{
	ਵਾਪਸ ਰਸਤਾ_2.substr(ਰਸਤਾ_2.ਲੱਭੋ_ਆਖਰੀ_ਦਾ("/\\") + 1);
}
#ਜੇ_ਅੰਤ

Fst_ਅੰਕੜੇ::Fst_ਅੰਕੜੇ(std::string filename) : ctx(ਖਾਲੀ_ਸੂਚਕ)
{
	#ਜੇ defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_ਅਧਾਰ_name(filename);
	ਜੇ (filename_trim.ਮਾਪ() > 4 && filename_trim.compare(filename_trim.ਮਾਪ()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.ਮਾਪ()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_ਅਧਾਰ_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		ਲਘੂਗਣਕ("Exec: %s\n", cmd);
		ਜੇ (ਲੜੀਵਾਰ_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#ਜੇ_ਅੰਤ
	ਅਚਲ std::ਸਦਿਸ਼<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	ਜੇ (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (ਪੂਰਨ)fstReaderGetTimescale(ctx);
	timescale_str = "";
	ਪੂਰਨ unit = 0;
	ਪੂਰਨ zeros = 0;
	ਜੇ (scale > 0)  {
		zeros = scale;
	} ਨਹੀਂਤਾਂ {
		ਜੇ ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} ਨਹੀਂਤਾਂ {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	ਲਈ (ਪੂਰਨ i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	ਕੱਢੋ_Var_ਨਾਂ();
}

Fst_ਅੰਕੜੇ::~Fst_ਅੰਕੜੇ()
{
	ਜੇ (ctx)
		fstReaderClose(ctx);
	ਜੇ (!tmp_file.ਖਾਲੀ_ਥਾਵਾਂ())
		remove(tmp_file.c_str());
}

uint64_t Fst_ਅੰਕੜੇ::get_ਸ਼ੁਰੂ_ਸਮਾਂ() { ਵਾਪਸ fst_Reader_Get_ਸ਼ੁਰੂ_ਸਮਾਂ(ctx); }

uint64_t Fst_ਅੰਕੜੇ::get_ਸਿਰਾ_ਸਮਾਂ() { ਵਾਪਸ fst_Reader_Get_ਸਿਰਾ_ਸਮਾਂ(ctx); }

ਸਥਿਰ ਖਾਲੀ normalize_brackets(std::string &str)
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &c : str) {
		ਜੇ (c == '<')
			c = '[';
		ਨਹੀਂਤਾਂ ਜੇ (c == '>')
			c = ']';
	}
}

fstHandle Fst_ਅੰਕੜੇ::getHandle(std::string name) {
	normalize_brackets(name);
	ਜੇ (name_ਮੰਜ਼ਿਲ_handle.ਲੱਭੋ(name) != name_ਮੰਜ਼ਿਲ_handle.ਸਿਰਾ())
		ਵਾਪਸ name_ਮੰਜ਼ਿਲ_handle[name];
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ 0;
};

dict<ਪੂਰਨ,fstHandle> Fst_ਅੰਕੜੇ::get_ਸਮ੍ਰਿਤੀ_Handles(std::string name) {
	ਜੇ (ਸਮ੍ਰਿਤੀ_ਮੰਜ਼ਿਲ_handle.ਲੱਭੋ(name) != ਸਮ੍ਰਿਤੀ_ਮੰਜ਼ਿਲ_handle.ਸਿਰਾ())
		ਵਾਪਸ ਸਮ੍ਰਿਤੀ_ਮੰਜ਼ਿਲ_handle[name];
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ dict<ਪੂਰਨ,fstHandle>();
};

ਸਥਿਰ std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.ਸਿਰਾ(), ' '), str.ਸਿਰਾ());
	ਵਾਪਸ str;
}

ਖਾਲੀ Fst_ਅੰਕੜੇ::ਕੱਢੋ_Var_ਨਾਂ()
{
	ਬਣਤਰ fstHier *h;
	std::string fst_scope_name;

	ਜਦੋਂ ((h = fstReaderIterateHier(ctx))) {
		ਚੋਣ (h->htyp) {
			ਹਾਲਤ FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_ਧੱਕੋ_Scope(ctx, h->u.scope.name, ਨਿਰਾ);
				ਰੋਕੋ;
			}
			ਹਾਲਤ FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_ਕੱਢੋ_Scope(ctx);
				ਰੋਕੋ;
			}
			ਹਾਲਤ FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.ਚੌੜਾਈ = h->u.var.ਲੰਮਾਈ;
				vars.ਧੱਕੋ_ਪਿੱਠ(var);
				ਜੇ (!var.is_alias)
					handle_ਮੰਜ਼ਿਲ_var[h->u.var.handle] = var;
				std::string clean_name;
				ਤਾਰਕਿਕ has_space = ਝੂਠ;
				ਲਈ(ਮਾਪ_t i=0;i<ਲੜੀ_ਲੰਬਾਈ(h->u.var.name);i++)
				{
					ਅੱਖਰ c = h->u.var.name[i];
					ਜੇ(c==' ') { has_space = ਸੱਚ; ਰੋਕੋ; }
					clean_name += c;
				}
				ਜੇ (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				ਜੇ (!has_space) {
					ਮਾਪ_t pos = clean_name.ਲੱਭੋ_ਆਖਰੀ_ਦਾ("[");
					std::string ਸੂਚਕਾਂਕ_or_range = clean_name.substr(pos+1);
					ਜੇ (ਸੂਚਕਾਂਕ_or_range.ਲੱਭੋ(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				ਮਾਪ_t pos = clean_name.ਲੱਭੋ_ਆਖਰੀ_ਦਾ("<");
				ਜੇ (pos != std::string::npos && clean_name.ਪਿੱਠ() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.ਕੱਢੋ_ਪਿੱਠ(); // remove closing bracket
					ਅੱਖਰ *endptr;
					ਪੂਰਨ mem_addr = strtol(addr.c_str(), &endptr, 16);
					ਜੇ (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} ਨਹੀਂਤਾਂ {
						ਸਮ੍ਰਿਤੀ_ਮੰਜ਼ਿਲ_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.ਲੱਭੋ_ਆਖਰੀ_ਦਾ("[");
				ਜੇ (pos != std::string::npos && clean_name.ਪਿੱਠ() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.ਕੱਢੋ_ਪਿੱਠ(); // remove closing bracket
					ਅੱਖਰ *endptr;
					ਪੂਰਨ mem_addr = strtol(addr.c_str(), &endptr, 10);
					ਜੇ (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} ਨਹੀਂਤਾਂ {
						ਸਮ੍ਰਿਤੀ_ਮੰਜ਼ਿਲ_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_ਮੰਜ਼ਿਲ_handle[var.scope+"."+clean_name] = h->u.var.handle;
				ਰੋਕੋ;
			}
		}
	}
}


ਸਥਿਰ ਖਾਲੀ reconstruct_clb_varlen_attimes(ਖਾਲੀ *user_ਅੰਕੜੇ, uint64_t pnt_ਸਮਾਂ, fstHandle pnt_facidx, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pnt_ਮੁੱਲ, uint32_t plen)
{
	Fst_ਅੰਕੜੇ *ptr = (Fst_ਅੰਕੜੇ*)user_ਅੰਕੜੇ;
	ptr->reconstruct_callback_attimes(pnt_ਸਮਾਂ, pnt_facidx, pnt_ਮੁੱਲ, plen);
}

ਸਥਿਰ ਖਾਲੀ reconstruct_clb_attimes(ਖਾਲੀ *user_ਅੰਕੜੇ, uint64_t pnt_ਸਮਾਂ, fstHandle pnt_facidx, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pnt_ਮੁੱਲ)
{
	Fst_ਅੰਕੜੇ *ptr = (Fst_ਅੰਕੜੇ*)user_ਅੰਕੜੇ;
	uint32_t plen = (pnt_ਮੁੱਲ) ?  ਲੜੀ_ਲੰਬਾਈ((ਅਚਲ ਅੱਖਰ *)pnt_ਮੁੱਲ) : 0;
	ptr->reconstruct_callback_attimes(pnt_ਸਮਾਂ, pnt_facidx, pnt_ਮੁੱਲ, plen);
}

ਖਾਲੀ Fst_ਅੰਕੜੇ::reconstruct_callback_attimes(uint64_t pnt_ਸਮਾਂ, fstHandle pnt_facidx, ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *pnt_ਮੁੱਲ, uint32_t /* plen */)
{
	ਜੇ (pnt_ਸਮਾਂ > ਸਿਰਾ_ਸਮਾਂ || !pnt_ਮੁੱਲ) ਵਾਪਸ;
	ਜੇ (curr_ਚੱਕਰ > ਆਖਰੀ_ਚੱਕਰ) ਵਾਪਸ;
	// if we are past the timestamp
	ਤਾਰਕਿਕ is_clock = ਝੂਠ;
	ਜੇ (!all_samples) {
		ਲਈ(ਸਵੈਚਾਲਿਤ &s : clk_ਸੰਕੇਤ) {
			ਜੇ (s==pnt_facidx)  {
				is_clock=ਸੱਚ;
				ਰੋਕੋ;
			}
		}
	}

	ਜੇ (pnt_ਸਮਾਂ > past_ਸਮਾਂ) {
		past_ਅੰਕੜੇ = ਆਖਰੀ_ਅੰਕੜੇ;
		past_ਸਮਾਂ = pnt_ਸਮਾਂ;
	}

	ਜੇ (pnt_ਸਮਾਂ > ਆਖਰੀ_ਸਮਾਂ) {
		ਜੇ (all_samples) {
			callback(ਆਖਰੀ_ਸਮਾਂ);
			curr_ਚੱਕਰ++;
			ਆਖਰੀ_ਸਮਾਂ = pnt_ਸਮਾਂ;
		} ਨਹੀਂਤਾਂ {
			ਜੇ (is_clock) {
				std::string val = std::string((ਅਚਲ ਅੱਖਰ *)pnt_ਮੁੱਲ);
				std::string prev = past_ਅੰਕੜੇ[pnt_facidx];
				ਜੇ ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(ਆਖਰੀ_ਸਮਾਂ);
					curr_ਚੱਕਰ++;
					ਆਖਰੀ_ਸਮਾਂ = pnt_ਸਮਾਂ;
				}
			}
		}
	}
	// always update last_data
	ਆਖਰੀ_ਅੰਕੜੇ[pnt_facidx] =  std::string((ਅਚਲ ਅੱਖਰ *)pnt_ਮੁੱਲ);
}

ਖਾਲੀ Fst_ਅੰਕੜੇ::reconstructAllAtTimes(std::ਸਦਿਸ਼<fstHandle> &ਸੰਕੇਤ, uint64_t ਸ਼ੁਰੂ, uint64_t ਸਿਰਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸਿਰਾ_ਚੱਕਰ, CallbackFunction cb)
{
	clk_ਸੰਕੇਤ = ਸੰਕੇਤ;
	callback = cb;
	ਸ਼ੁਰੂ_ਸਮਾਂ = ਸ਼ੁਰੂ;
	ਸਿਰਾ_ਸਮਾਂ = ਸਿਰਾ;
	curr_ਚੱਕਰ = 0;
	ਆਖਰੀ_ਚੱਕਰ = ਸਿਰਾ_ਚੱਕਰ;
	ਆਖਰੀ_ਅੰਕੜੇ.clear();
	ਆਖਰੀ_ਸਮਾਂ = ਸ਼ੁਰੂ_ਸਮਾਂ;
	past_ਅੰਕੜੇ.clear();
	past_ਸਮਾਂ = ਸ਼ੁਰੂ_ਸਮਾਂ;
	all_samples = clk_ਸੰਕੇਤ.ਖਾਲੀ_ਥਾਵਾਂ();

	fst_Reader_ਸੈੱਟ_Unlimited_ਸਮਾਂ_Range(ctx);
	fst_Reader_ਸੈੱਟ_Fac_Process_ਨਕਾਬ_All(ctx);
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, ਇਹ, ਖਾਲੀ_ਸੂਚਕ);
	ਜੇ (ਆਖਰੀ_ਸਮਾਂ!=ਸਿਰਾ_ਸਮਾਂ && curr_ਚੱਕਰ <= ਆਖਰੀ_ਚੱਕਰ) {
		past_ਅੰਕੜੇ = ਆਖਰੀ_ਅੰਕੜੇ;
		callback(ਆਖਰੀ_ਸਮਾਂ);
		curr_ਚੱਕਰ++;
	}
	ਜੇ (curr_ਚੱਕਰ <= ਆਖਰੀ_ਚੱਕਰ) {
		past_ਅੰਕੜੇ = ਆਖਰੀ_ਅੰਕੜੇ;
		callback(ਸਿਰਾ_ਸਮਾਂ);
		curr_ਚੱਕਰ++;
	}
}

std::string Fst_ਅੰਕੜੇ::ਮੁੱਲ_ਦਾ(fstHandle ਸੰਕੇਤ)
{
	ਜੇ (past_ਅੰਕੜੇ.ਲੱਭੋ(ਸੰਕੇਤ) == past_ਅੰਕੜੇ.ਸਿਰਾ()) {
		ਵਾਪਸ std::string(handle_ਮੰਜ਼ਿਲ_var[ਸੰਕੇਤ].ਚੌੜਾਈ, 'x');
	}
	ਵਾਪਸ past_ਅੰਕੜੇ[ਸੰਕੇਤ];
}
