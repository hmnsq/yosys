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

#ಅನ್ತರ್ಭಾವ "kernel/fstdata.h"

USING_YOSYS_NAMESPACE


#ಆದರೆ defined(YOSYS_ENABLE_SPAWN)
ಸ್ಥಿರ std::string file_ತಳ_name(std::string ಸ್ಥಿರಾಂಕ & ಮಾರ್ಗ_2)
{
	ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_2.substr(ಮಾರ್ಗ_2.ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ದ("/\\") + 1);
}
#ಆದರೆ_ಅಂತ್ಯ

Fst_ದತ್ತಾಂಶ::Fst_ದತ್ತಾಂಶ(std::string filename) : ctx(ಶೂನ್ಯಸೂಚಕ)
{
	#ಆದರೆ defined(YOSYS_ENABLE_SPAWN)
	std::string filename_trim = file_ತಳ_name(filename);
	ಆದರೆ (filename_trim.ಗಾತ್ರ_2() > 4 && filename_trim.compare(filename_trim.ಗಾತ್ರ_2()-4, std::string::npos, ".vcd") == 0) {
		filename_trim.erase(filename_trim.ಗಾತ್ರ_2()-4);
		tmp_file = stringf("%s/converted_%s.fst", get_ತಳ_tmpdir(), filename_trim);
		std::string cmd = stringf("vcd2fst %s %s", filename, tmp_file);
		ಲಘುಗಣಕ("Exec: %s\n", cmd);
		ಆದರೆ (ಸತತ_command(cmd) != 0)
			log_cmd_error("Shell command failed!\n");
		filename = tmp_file;
	}
	#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<std::string> g_units = { "s", "ms", "us", "ns", "ps", "fs", "as", "zs" };
	ctx = (fstReaderContext *)fstReaderOpen(filename.c_str());
	ಆದರೆ (!ctx)
		log_error("Error opening '%s' as FST file\n", filename);
	scale = (ಪೂರ್ಣಾಂಕ)fstReaderGetTimescale(ctx);
	timescale_str = "";
	ಪೂರ್ಣಾಂಕ unit = 0;
	ಪೂರ್ಣಾಂಕ zeros = 0;
	ಆದರೆ (scale > 0)  {
		zeros = scale;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ ((scale % 3) == 0) {
			zeros = (-scale % 3);
			unit = (-scale / 3);
		} ಇಲ್ಲದಿದ್ದರೆ {
			zeros = 3 - (-scale % 3);
			unit = (-scale / 3) + 1;
		}
	}
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i=0;i<zeros; i++) timescale_str += "0";
	timescale_str += g_units[unit];
	ಹೊರತೆಗೆ_Var_ಹೆಸರುಗಳು();
}

Fst_ದತ್ತಾಂಶ::~Fst_ದತ್ತಾಂಶ()
{
	ಆದರೆ (ctx)
		fstReaderClose(ctx);
	ಆದರೆ (!tmp_file.ಖಾಲಿಗಳು())
		remove(tmp_file.c_str());
}

uint64_t Fst_ದತ್ತಾಂಶ::get_ಆರಂಭ_ಸಮಯ() { ಹಿಂತಿರುಗಿ fst_Reader_Get_ಆರಂಭ_ಸಮಯ(ctx); }

uint64_t Fst_ದತ್ತಾಂಶ::get_ಕೊನೆ_ಸಮಯ() { ಹಿಂತಿರುಗಿ fst_Reader_Get_ಕೊನೆ_ಸಮಯ(ctx); }

ಸ್ಥಿರ ಶೂನ್ಯ normalize_brackets(std::string &str)
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &c : str) {
		ಆದರೆ (c == '<')
			c = '[';
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '>')
			c = ']';
	}
}

fstHandle Fst_ದತ್ತಾಂಶ::getHandle(std::string name) {
	normalize_brackets(name);
	ಆದರೆ (name_ಗಮ್ಯ_handle.ಕಂಡುಹಿಡಿ(name) != name_ಗಮ್ಯ_handle.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ name_ಗಮ್ಯ_handle[name];
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ 0;
};

dict<ಪೂರ್ಣಾಂಕ,fstHandle> Fst_ದತ್ತಾಂಶ::get_ಸ್ಮರಣೆ_Handles(std::string name) {
	ಆದರೆ (ಸ್ಮರಣೆ_ಗಮ್ಯ_handle.ಕಂಡುಹಿಡಿ(name) != ಸ್ಮರಣೆ_ಗಮ್ಯ_handle.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ ಸ್ಮರಣೆ_ಗಮ್ಯ_handle[name];
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ dict<ಪೂರ್ಣಾಂಕ,fstHandle>();
};

ಸ್ಥಿರ std::string remove_spaces(std::string str)
{
	str.erase(std::remove(str.begin(), str.ಕೊನೆ(), ' '), str.ಕೊನೆ());
	ಹಿಂತಿರುಗಿ str;
}

ಶೂನ್ಯ Fst_ದತ್ತಾಂಶ::ಹೊರತೆಗೆ_Var_ಹೆಸರುಗಳು()
{
	ರಚನೆ fstHier *h;
	std::string fst_scope_name;

	ಆಗ ((h = fstReaderIterateHier(ctx))) {
		ಆಯ್ಕೆ (h->htyp) {
			ಸ್ಥಿತಿ FST_HT_SCOPE: {
				fst_scope_name = fst_Reader_ತಳ್ಳು_Scope(ctx, h->u.scope.name, ಖಾಲಿ);
				ನಿಲ್ಲಿಸು;
			}
			ಸ್ಥಿತಿ FST_HT_UPSCOPE: {
				fst_scope_name = fst_Reader_ತೆಗೆ_Scope(ctx);
				ನಿಲ್ಲಿಸು;
			}
			ಸ್ಥಿತಿ FST_HT_VAR: {
				FstVar var;
				var.id = h->u.var.handle;
				var.is_alias = h->u.var.is_alias;
				var.is_reg = (fstVarType)h->u.var.typ == FST_VT_VCD_REG;
				var.name = remove_spaces(h->u.var.name);
				var.scope = fst_scope_name;
				normalize_brackets(var.scope);
				var.ಅಗಲ = h->u.var.ಉದ್ದಳತೆ;
				vars.ತಳ್ಳು_ಹಿಂಭಾಗ(var);
				ಆದರೆ (!var.is_alias)
					handle_ಗಮ್ಯ_var[h->u.var.handle] = var;
				std::string clean_name;
				ತಾರ್ಕಿಕ has_space = ಸುಳ್ಳು;
				ಗಾಗಿ(ಗಾತ್ರ_t i=0;i<ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(h->u.var.name);i++)
				{
					ಅಕ್ಷರ c = h->u.var.name[i];
					ಆದರೆ(c==' ') { has_space = ನಿಜ; ನಿಲ್ಲಿಸು; }
					clean_name += c;
				}
				ಆದರೆ (clean_name[0]=='\\')
					clean_name = clean_name.substr(1);
				ಆದರೆ (!has_space) {
					ಗಾತ್ರ_t pos = clean_name.ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ದ("[");
					std::string ಸೂಚ್ಯಂಕ_or_range = clean_name.substr(pos+1);
					ಆದರೆ (ಸೂಚ್ಯಂಕ_or_range.ಕಂಡುಹಿಡಿ(":") != std::string::npos) {
						clean_name = clean_name.substr(0,pos);
					}
				}
				ಗಾತ್ರ_t pos = clean_name.ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ದ("<");
				ಆದರೆ (pos != std::string::npos && clean_name.ಹಿಂಭಾಗ() == '>') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.ತೆಗೆ_ಹಿಂಭಾಗ(); // remove closing bracket
					ಅಕ್ಷರ *endptr;
					ಪೂರ್ಣಾಂಕ mem_addr = strtol(addr.c_str(), &endptr, 16);
					ಆದರೆ (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} ಇಲ್ಲದಿದ್ದರೆ {
						ಸ್ಮರಣೆ_ಗಮ್ಯ_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				pos = clean_name.ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_ದ("[");
				ಆದರೆ (pos != std::string::npos && clean_name.ಹಿಂಭಾಗ() == ']') {
					std::string mem_cell = clean_name.substr(0, pos);
					normalize_brackets(mem_cell);
					std::string addr = clean_name.substr(pos+1);
					addr.ತೆಗೆ_ಹಿಂಭಾಗ(); // remove closing bracket
					ಅಕ್ಷರ *endptr;
					ಪೂರ್ಣಾಂಕ mem_addr = strtol(addr.c_str(), &endptr, 10);
					ಆದರೆ (*endptr) {
						log_debug("Error parsing memory address in : %s\n", clean_name);
					} ಇಲ್ಲದಿದ್ದರೆ {
						ಸ್ಮರಣೆ_ಗಮ್ಯ_handle[var.scope+"."+mem_cell][mem_addr] = var.id;
					}
				}
				normalize_brackets(clean_name);
				name_ಗಮ್ಯ_handle[var.scope+"."+clean_name] = h->u.var.handle;
				ನಿಲ್ಲಿಸು;
			}
		}
	}
}


ಸ್ಥಿರ ಶೂನ್ಯ reconstruct_clb_varlen_attimes(ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, uint64_t pnt_ಸಮಯ, fstHandle pnt_facidx, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pnt_ಮೌಲ್ಯ, uint32_t plen)
{
	Fst_ದತ್ತಾಂಶ *ptr = (Fst_ದತ್ತಾಂಶ*)user_ದತ್ತಾಂಶ;
	ptr->reconstruct_callback_attimes(pnt_ಸಮಯ, pnt_facidx, pnt_ಮೌಲ್ಯ, plen);
}

ಸ್ಥಿರ ಶೂನ್ಯ reconstruct_clb_attimes(ಶೂನ್ಯ *user_ದತ್ತಾಂಶ, uint64_t pnt_ಸಮಯ, fstHandle pnt_facidx, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pnt_ಮೌಲ್ಯ)
{
	Fst_ದತ್ತಾಂಶ *ptr = (Fst_ದತ್ತಾಂಶ*)user_ದತ್ತಾಂಶ;
	uint32_t plen = (pnt_ಮೌಲ್ಯ) ?  ಅಕ್ಷರಸರಣಿ_ಉದ್ದ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)pnt_ಮೌಲ್ಯ) : 0;
	ptr->reconstruct_callback_attimes(pnt_ಸಮಯ, pnt_facidx, pnt_ಮೌಲ್ಯ, plen);
}

ಶೂನ್ಯ Fst_ದತ್ತಾಂಶ::reconstruct_callback_attimes(uint64_t pnt_ಸಮಯ, fstHandle pnt_facidx, ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *pnt_ಮೌಲ್ಯ, uint32_t /* plen */)
{
	ಆದರೆ (pnt_ಸಮಯ > ಕೊನೆ_ಸಮಯ || !pnt_ಮೌಲ್ಯ) ಹಿಂತಿರುಗಿ;
	ಆದರೆ (curr_ಚಕ್ರ > ಕೊನೆಯ_ಚಕ್ರ) ಹಿಂತಿರುಗಿ;
	// if we are past the timestamp
	ತಾರ್ಕಿಕ is_clock = ಸುಳ್ಳು;
	ಆದರೆ (!all_samples) {
		ಗಾಗಿ(ಸ್ವಯಂಚಾಲಿತ &s : clk_ಸಂಕೇತಗಳು) {
			ಆದರೆ (s==pnt_facidx)  {
				is_clock=ನಿಜ;
				ನಿಲ್ಲಿಸು;
			}
		}
	}

	ಆದರೆ (pnt_ಸಮಯ > past_ಸಮಯ) {
		past_ದತ್ತಾಂಶ = ಕೊನೆಯ_ದತ್ತಾಂಶ;
		past_ಸಮಯ = pnt_ಸಮಯ;
	}

	ಆದರೆ (pnt_ಸಮಯ > ಕೊನೆಯ_ಸಮಯ) {
		ಆದರೆ (all_samples) {
			callback(ಕೊನೆಯ_ಸಮಯ);
			curr_ಚಕ್ರ++;
			ಕೊನೆಯ_ಸಮಯ = pnt_ಸಮಯ;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (is_clock) {
				std::string val = std::string((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)pnt_ಮೌಲ್ಯ);
				std::string prev = past_ದತ್ತಾಂಶ[pnt_facidx];
				ಆದರೆ ((prev!="1" && val=="1") || (prev!="0" && val=="0")) {
					callback(ಕೊನೆಯ_ಸಮಯ);
					curr_ಚಕ್ರ++;
					ಕೊನೆಯ_ಸಮಯ = pnt_ಸಮಯ;
				}
			}
		}
	}
	// always update last_data
	ಕೊನೆಯ_ದತ್ತಾಂಶ[pnt_facidx] =  std::string((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)pnt_ಮೌಲ್ಯ);
}

ಶೂನ್ಯ Fst_ದತ್ತಾಂಶ::reconstructAllAtTimes(std::ಸದಿಶ<fstHandle> &ಸಂಕೇತ, uint64_t ಆರಂಭ, uint64_t ಕೊನೆ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಚಕ್ರ, CallbackFunction cb)
{
	clk_ಸಂಕೇತಗಳು = ಸಂಕೇತ;
	callback = cb;
	ಆರಂಭ_ಸಮಯ = ಆರಂಭ;
	ಕೊನೆ_ಸಮಯ = ಕೊನೆ;
	curr_ಚಕ್ರ = 0;
	ಕೊನೆಯ_ಚಕ್ರ = ಕೊನೆ_ಚಕ್ರ;
	ಕೊನೆಯ_ದತ್ತಾಂಶ.clear();
	ಕೊನೆಯ_ಸಮಯ = ಆರಂಭ_ಸಮಯ;
	past_ದತ್ತಾಂಶ.clear();
	past_ಸಮಯ = ಆರಂಭ_ಸಮಯ;
	all_samples = clk_ಸಂಕೇತಗಳು.ಖಾಲಿಗಳು();

	fst_Reader_ಗಣ_Unlimited_ಸಮಯ_Range(ctx);
	fst_Reader_ಗಣ_Fac_Process_ಮುಖವಾಡ_All(ctx);
	fstReaderIterBlocks2(ctx, reconstruct_clb_attimes, reconstruct_clb_varlen_attimes, ಇದು, ಶೂನ್ಯಸೂಚಕ);
	ಆದರೆ (ಕೊನೆಯ_ಸಮಯ!=ಕೊನೆ_ಸಮಯ && curr_ಚಕ್ರ <= ಕೊನೆಯ_ಚಕ್ರ) {
		past_ದತ್ತಾಂಶ = ಕೊನೆಯ_ದತ್ತಾಂಶ;
		callback(ಕೊನೆಯ_ಸಮಯ);
		curr_ಚಕ್ರ++;
	}
	ಆದರೆ (curr_ಚಕ್ರ <= ಕೊನೆಯ_ಚಕ್ರ) {
		past_ದತ್ತಾಂಶ = ಕೊನೆಯ_ದತ್ತಾಂಶ;
		callback(ಕೊನೆ_ಸಮಯ);
		curr_ಚಕ್ರ++;
	}
}

std::string Fst_ದತ್ತಾಂಶ::ಮೌಲ್ಯ_ದ(fstHandle ಸಂಕೇತ)
{
	ಆದರೆ (past_ದತ್ತಾಂಶ.ಕಂಡುಹಿಡಿ(ಸಂಕೇತ) == past_ದತ್ತಾಂಶ.ಕೊನೆ()) {
		ಹಿಂತಿರುಗಿ std::string(handle_ಗಮ್ಯ_var[ಸಂಕೇತ].ಅಗಲ, 'x');
	}
	ಹಿಂತಿರುಗಿ past_ದತ್ತಾಂಶ[ಸಂಕೇತ];
}
