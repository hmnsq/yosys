/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
 *            (C) 2020  Eddie Hung    <eddie@fpgeh.com>
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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ TIMINGINFO_H
#ವ್ಯಾಖ್ಯಾನಿಸು TIMINGINFO_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ರಚನೆ TimingInfo
{
	ರಚನೆ Name_ಬಿಟ್
	{
		RTLIL::IdString name;
		ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;
		Name_ಬಿಟ್() : ಸ್ಥಾನಾಂತರ(0) {}
		Name_ಬಿಟ್(IdString name, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ) : name(name), ಸ್ಥಾನಾಂತರ(ಸ್ಥಾನಾಂತರ) {}
		explicit Name_ಬಿಟ್(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &b) : name(b.wire->name), ಸ್ಥಾನಾಂತರ(b.ಸ್ಥಾನಾಂತರ) {}
		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Name_ಬಿಟ್& nb) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ nb.name == name && nb.ಸ್ಥಾನಾಂತರ == ಸ್ಥಾನಾಂತರ; }
		bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Name_ಬಿಟ್& nb) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !ಕ್ರಿಯಾಕಾರಕ==(nb); }
		std::optional<Sig_ಬಿಟ್> get_connection(RTLIL::Cell *cell) {
			IdString ಪೋರ್ಟ್_name = name;
			ಆದರೆ (!cell->has_ಪೋರ್ಟ್(ಪೋರ್ಟ್_name))
				ಹಿಂತಿರುಗಿ {};
			ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ = cell->get_ಪೋರ್ಟ್(ಪೋರ್ಟ್_name);
			ಆದರೆ (ಸ್ಥಾನಾಂತರ >= ಪೋರ್ಟ್.ಗಾತ್ರ_2())
				ಹಿಂತಿರುಗಿ {};
			ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್[ಸ್ಥಾನಾಂತರ];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
			h.eat(name);
			h.eat(ಸ್ಥಾನಾಂತರ);
			ಹಿಂತಿರುಗಿ h;
		}
	};
	ರಚನೆ ಬಿಟ್_ಬಿಟ್
	{
		Name_ಬಿಟ್ ಮೊದಲ, ಎರಡನೇ;
		ಬಿಟ್_ಬಿಟ್(ಸ್ಥಿರಾಂಕ Name_ಬಿಟ್ &ಮೊದಲ, ಸ್ಥಿರಾಂಕ Name_ಬಿಟ್ &ಎರಡನೇ) : ಮೊದಲ(ಮೊದಲ), ಎರಡನೇ(ಎರಡನೇ) {}
		ಬಿಟ್_ಬಿಟ್(ಸ್ಥಿರಾಂಕ Sig_ಬಿಟ್ &ಮೊದಲ, ಸ್ಥಿರಾಂಕ Sig_ಬಿಟ್ &ಎರಡನೇ) : ಮೊದಲ(ಮೊದಲ), ಎರಡನೇ(ಎರಡನೇ) {}
		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಬಿಟ್_ಬಿಟ್& bb) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ bb.ಮೊದಲ == ಮೊದಲ && bb.ಎರಡನೇ == ಎರಡನೇ; }
		[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
			h.eat(ಮೊದಲ);
			h.eat(ಎರಡನೇ);
			ಹಿಂತಿರುಗಿ h;
		}
	};

	ರಚನೆ ModuleTiming
	{
		dict<ಬಿಟ್_ಬಿಟ್, ಪೂರ್ಣಾಂಕ> comb;
		dict<Name_ಬಿಟ್, std::pair<ಪೂರ್ಣಾಂಕ,Name_ಬಿಟ್>> ಆಗಮನ, required;
		bool has_inputs;
	};

	dict<RTLIL::IdString, ModuleTiming> ದತ್ತಾಂಶ;

	TimingInfo()
	{
	}

	TimingInfo(RTLIL::Design *design)
	{
		ಸಿದ್ಧತೆ(design);
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ(RTLIL::Design *design)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ module : design->modules()) {
			ಆದರೆ (!module->get_blackbox_attribute())
				ಮುಂದುವರಿ;
			ಸಿದ್ಧತೆ_module(module);
		}
	}

	ಸ್ಥಿರಾಂಕ ModuleTiming& ಸಿದ್ಧತೆ_module(RTLIL::Module *module)
	{
		ಸ್ವಯಂಚಾಲಿತ r = ದತ್ತಾಂಶ.ಸೇರಿಸು(module->name);
		log_assert(r.ಎರಡನೇ);
		ಸ್ವಯಂಚಾಲಿತ &t = r.ಮೊದಲ->ಎರಡನೇ;

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : module->cells()) {
			ಆದರೆ (cell->type == ID($specify2)) {
				ಸ್ವಯಂಚಾಲಿತ en = cell->get_ಪೋರ್ಟ್(ID::EN);
				ಆದರೆ (en.is_fully_const() && !en.as_bool())
					ಮುಂದುವರಿ;
				ಸ್ವಯಂಚಾಲಿತ src = cell->get_ಪೋರ್ಟ್(ID::SRC);
				ಸ್ವಯಂಚಾಲಿತ dst = cell->get_ಪೋರ್ಟ್(ID::DST);
				ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &c : src.chunks())
					ಆದರೆ (!c.wire || !c.wire->ಪೋರ್ಟ್_ಒಳಹರಿವು)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &c : dst.chunks())
					ಆದರೆ (!c.wire || !c.wire->ಪೋರ್ಟ್_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				ಪೂರ್ಣಾಂಕ rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				ಪೂರ್ಣಾಂಕ fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				ಪೂರ್ಣಾಂಕ max = std::max(rise_max,fall_max);
				ಆದರೆ (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				ಆದರೆ (cell->getParam(ID::ತುಂಬಿದ).as_bool()) {
					ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &s : src)
						ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &d : dst) {
							ಸ್ವಯಂಚಾಲಿತ r = t.comb.ಸೇರಿಸು(ಬಿಟ್_ಬಿಟ್(s,d));
							ಆದರೆ (!r.ಎರಡನೇ)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.ಮೊದಲ->ಎರಡನೇ = max;
						}
				}
				ಇಲ್ಲದಿದ್ದರೆ {
					log_assert(Get_ಗಾತ್ರ(src) == Get_ಗಾತ್ರ(dst));
					ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = 0; i < Get_ಗಾತ್ರ(src); i++) {
						ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &s = src[i];
						ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &d = dst[i];
						ಸ್ವಯಂಚಾಲಿತ r = t.comb.ಸೇರಿಸು(ಬಿಟ್_ಬಿಟ್(s,d));
						ಆದರೆ (!r.ಎರಡನೇ)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.ಮೊದಲ->ಎರಡನೇ = max;
					}
				}
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($specify3)) {
				ಸ್ವಯಂಚಾಲಿತ src = cell->get_ಪೋರ್ಟ್(ID::SRC).as_ಬಿಟ್();
				ಸ್ವಯಂಚಾಲಿತ dst = cell->get_ಪೋರ್ಟ್(ID::DST);
				ಆದರೆ (!src.wire || !src.wire->ಪೋರ್ಟ್_ಒಳಹರಿವು)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &c : dst.chunks())
					ಆದರೆ (!c.wire->ಪೋರ್ಟ್_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				ಪೂರ್ಣಾಂಕ rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				ಪೂರ್ಣಾಂಕ fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				ಪೂರ್ಣಾಂಕ max = std::max(rise_max,fall_max);
				ಆದರೆ (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &d : dst) {
					ಸ್ವಯಂಚಾಲಿತ r = t.ಆಗಮನ.ಸೇರಿಸು(Name_ಬಿಟ್(d));
					ಸ್ವಯಂಚಾಲಿತ &v = r.ಮೊದಲ->ಎರಡನೇ;
					ಆದರೆ (r.ಎರಡನೇ || v.ಮೊದಲ < max) {
						v.ಮೊದಲ = max;
						v.ಎರಡನೇ = Name_ಬಿಟ್(src);
					}
				}
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				ಆದರೆ (type != "$setup" && type != "$setuphold")
					ಮುಂದುವರಿ;
				ಸ್ವಯಂಚಾಲಿತ src = cell->get_ಪೋರ್ಟ್(ID::SRC);
				ಸ್ವಯಂಚಾಲಿತ dst = cell->get_ಪೋರ್ಟ್(ID::DST).as_ಬಿಟ್();
				ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &c : src.chunks())
					ಆದರೆ (!c.wire || !c.wire->ಪೋರ್ಟ್_ಒಳಹರಿವು)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				ಆದರೆ (!dst.wire || !dst.wire->ಪೋರ್ಟ್_ಒಳಹರಿವು)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				ಪೂರ್ಣಾಂಕ max = cell->getParam(ID::T_ಎಲ್ಲೆ_MAX).as_int();
				ಆದರೆ (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &s : src) {
					ಸ್ವಯಂಚಾಲಿತ r = t.required.ಸೇರಿಸು(Name_ಬಿಟ್(s));
					ಸ್ವಯಂಚಾಲಿತ &v = r.ಮೊದಲ->ಎರಡನೇ;
					ಆದರೆ (r.ಎರಡನೇ || v.ಮೊದಲ < max) {
						v.ಮೊದಲ = max;
						v.ಎರಡನೇ = Name_ಬಿಟ್(dst);
					}
				}
			}
		}

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಪೋರ್ಟ್_name : module->ports) {
			ಸ್ವಯಂಚಾಲಿತ wire = module->wire(ಪೋರ್ಟ್_name);
			ಆದರೆ (wire->ಪೋರ್ಟ್_ಒಳಹರಿವು) {
				t.has_inputs = true;
				ನಿಲ್ಲಿಸು;
			}
		}

		ಹಿಂತಿರುಗಿ t;
	}

	decltype(ದತ್ತಾಂಶ)::const_iterator ಕಂಡುಹಿಡಿ(RTLIL::IdString module_name) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ಕಂಡುಹಿಡಿ(module_name); }
	decltype(ದತ್ತಾಂಶ)::const_iterator ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ಕೊನೆ(); }
	ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2(RTLIL::IdString module_name) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ಎಣಿಕೆ_2(module_name); }
	ಸ್ಥಿರಾಂಕ ModuleTiming& at(RTLIL::IdString module_name) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.at(module_name); }
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
