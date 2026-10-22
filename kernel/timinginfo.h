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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ TIMINGINFO_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TIMINGINFO_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ਬਣਤਰ TimingInfo
{
	ਬਣਤਰ Name_ਦੋਆਧਾਰੀ_ਅੰਕ
	{
		RTLIL::IdString name;
		ਪੂਰਨ ਵਿੱਥ;
		Name_ਦੋਆਧਾਰੀ_ਅੰਕ() : ਵਿੱਥ(0) {}
		Name_ਦੋਆਧਾਰੀ_ਅੰਕ(IdString name, ਪੂਰਨ ਵਿੱਥ) : name(name), ਵਿੱਥ(ਵਿੱਥ) {}
		explicit Name_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &b) : name(b.wire->name), ਵਿੱਥ(b.ਵਿੱਥ) {}
		bool ਸੰਚਾਲਕ==(ਅਚਲ Name_ਦੋਆਧਾਰੀ_ਅੰਕ& nb) ਅਚਲ { ਵਾਪਸ nb.name == name && nb.ਵਿੱਥ == ਵਿੱਥ; }
		bool ਸੰਚਾਲਕ!=(ਅਚਲ Name_ਦੋਆਧਾਰੀ_ਅੰਕ& nb) ਅਚਲ { ਵਾਪਸ !ਸੰਚਾਲਕ==(nb); }
		std::optional<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> get_connection(RTLIL::Cell *cell) {
			IdString ਪੋਰਟ_name = name;
			ਜੇ (!cell->has_ਪੋਰਟ(ਪੋਰਟ_name))
				ਵਾਪਸ {};
			ਸਵੈਚਾਲਿਤ &ਪੋਰਟ = cell->get_ਪੋਰਟ(ਪੋਰਟ_name);
			ਜੇ (ਵਿੱਥ >= ਪੋਰਟ.ਮਾਪ())
				ਵਾਪਸ {};
			ਵਾਪਸ ਪੋਰਟ[ਵਿੱਥ];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
			h.eat(name);
			h.eat(ਵਿੱਥ);
			ਵਾਪਸ h;
		}
	};
	ਬਣਤਰ ਦੋਆਧਾਰੀ_ਅੰਕ_ਦੋਆਧਾਰੀ_ਅੰਕ
	{
		Name_ਦੋਆਧਾਰੀ_ਅੰਕ ਪਹਿਲਾ, ਦੂਜਾ;
		ਦੋਆਧਾਰੀ_ਅੰਕ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ Name_ਦੋਆਧਾਰੀ_ਅੰਕ &ਪਹਿਲਾ, ਅਚਲ Name_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੂਜਾ) : ਪਹਿਲਾ(ਪਹਿਲਾ), ਦੂਜਾ(ਦੂਜਾ) {}
		ਦੋਆਧਾਰੀ_ਅੰਕ_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਪਹਿਲਾ, ਅਚਲ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੂਜਾ) : ਪਹਿਲਾ(ਪਹਿਲਾ), ਦੂਜਾ(ਦੂਜਾ) {}
		bool ਸੰਚਾਲਕ==(ਅਚਲ ਦੋਆਧਾਰੀ_ਅੰਕ_ਦੋਆਧਾਰੀ_ਅੰਕ& bb) ਅਚਲ { ਵਾਪਸ bb.ਪਹਿਲਾ == ਪਹਿਲਾ && bb.ਦੂਜਾ == ਦੂਜਾ; }
		[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
			h.eat(ਪਹਿਲਾ);
			h.eat(ਦੂਜਾ);
			ਵਾਪਸ h;
		}
	};

	ਬਣਤਰ ModuleTiming
	{
		dict<ਦੋਆਧਾਰੀ_ਅੰਕ_ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ> comb;
		dict<Name_ਦੋਆਧਾਰੀ_ਅੰਕ, std::pair<ਪੂਰਨ,Name_ਦੋਆਧਾਰੀ_ਅੰਕ>> ਆਮਦ, required;
		bool has_inputs;
	};

	dict<RTLIL::IdString, ModuleTiming> ਅੰਕੜੇ;

	TimingInfo()
	{
	}

	TimingInfo(RTLIL::Design *design)
	{
		ਤਿਆਰੀ(design);
	}

	ਖਾਲੀ ਤਿਆਰੀ(RTLIL::Design *design)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ module : design->modules()) {
			ਜੇ (!module->get_blackbox_attribute())
				ਜਾਰੀ;
			ਤਿਆਰੀ_module(module);
		}
	}

	ਅਚਲ ModuleTiming& ਤਿਆਰੀ_module(RTLIL::Module *module)
	{
		ਸਵੈਚਾਲਿਤ r = ਅੰਕੜੇ.ਪਾਓ(module->name);
		log_assert(r.ਦੂਜਾ);
		ਸਵੈਚਾਲਿਤ &t = r.ਪਹਿਲਾ->ਦੂਜਾ;

		ਲਈ (ਸਵੈਚਾਲਿਤ cell : module->cells()) {
			ਜੇ (cell->type == ID($specify2)) {
				ਸਵੈਚਾਲਿਤ en = cell->get_ਪੋਰਟ(ID::EN);
				ਜੇ (en.is_fully_const() && !en.as_bool())
					ਜਾਰੀ;
				ਸਵੈਚਾਲਿਤ src = cell->get_ਪੋਰਟ(ID::SRC);
				ਸਵੈਚਾਲਿਤ dst = cell->get_ਪੋਰਟ(ID::DST);
				ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &c : src.chunks())
					ਜੇ (!c.wire || !c.wire->ਪੋਰਟ_ਨਿਵੇਸ਼)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &c : dst.chunks())
					ਜੇ (!c.wire || !c.wire->ਪੋਰਟ_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				ਪੂਰਨ rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				ਪੂਰਨ fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				ਪੂਰਨ max = std::max(rise_max,fall_max);
				ਜੇ (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				ਜੇ (cell->getParam(ID::ਭਰਿਆ).as_bool()) {
					ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &s : src)
						ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &d : dst) {
							ਸਵੈਚਾਲਿਤ r = t.comb.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ_ਦੋਆਧਾਰੀ_ਅੰਕ(s,d));
							ਜੇ (!r.ਦੂਜਾ)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.ਪਹਿਲਾ->ਦੂਜਾ = max;
						}
				}
				ਨਹੀਂਤਾਂ {
					log_assert(Get_ਮਾਪ(src) == Get_ਮਾਪ(dst));
					ਲਈ (ਸਵੈਚਾਲਿਤ i = 0; i < Get_ਮਾਪ(src); i++) {
						ਅਚਲ ਸਵੈਚਾਲਿਤ &s = src[i];
						ਅਚਲ ਸਵੈਚਾਲਿਤ &d = dst[i];
						ਸਵੈਚਾਲਿਤ r = t.comb.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ_ਦੋਆਧਾਰੀ_ਅੰਕ(s,d));
						ਜੇ (!r.ਦੂਜਾ)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.ਪਹਿਲਾ->ਦੂਜਾ = max;
					}
				}
			}
			ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($specify3)) {
				ਸਵੈਚਾਲਿਤ src = cell->get_ਪੋਰਟ(ID::SRC).as_ਦੋਆਧਾਰੀ_ਅੰਕ();
				ਸਵੈਚਾਲਿਤ dst = cell->get_ਪੋਰਟ(ID::DST);
				ਜੇ (!src.wire || !src.wire->ਪੋਰਟ_ਨਿਵੇਸ਼)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &c : dst.chunks())
					ਜੇ (!c.wire->ਪੋਰਟ_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				ਪੂਰਨ rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				ਪੂਰਨ fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				ਪੂਰਨ max = std::max(rise_max,fall_max);
				ਜੇ (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &d : dst) {
					ਸਵੈਚਾਲਿਤ r = t.ਆਮਦ.ਪਾਓ(Name_ਦੋਆਧਾਰੀ_ਅੰਕ(d));
					ਸਵੈਚਾਲਿਤ &v = r.ਪਹਿਲਾ->ਦੂਜਾ;
					ਜੇ (r.ਦੂਜਾ || v.ਪਹਿਲਾ < max) {
						v.ਪਹਿਲਾ = max;
						v.ਦੂਜਾ = Name_ਦੋਆਧਾਰੀ_ਅੰਕ(src);
					}
				}
			}
			ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				ਜੇ (type != "$setup" && type != "$setuphold")
					ਜਾਰੀ;
				ਸਵੈਚਾਲਿਤ src = cell->get_ਪੋਰਟ(ID::SRC);
				ਸਵੈਚਾਲਿਤ dst = cell->get_ਪੋਰਟ(ID::DST).as_ਦੋਆਧਾਰੀ_ਅੰਕ();
				ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &c : src.chunks())
					ਜੇ (!c.wire || !c.wire->ਪੋਰਟ_ਨਿਵੇਸ਼)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				ਜੇ (!dst.wire || !dst.wire->ਪੋਰਟ_ਨਿਵੇਸ਼)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				ਪੂਰਨ max = cell->getParam(ID::T_ਹੱਦਬੰਦੀ_MAX).as_int();
				ਜੇ (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &s : src) {
					ਸਵੈਚਾਲਿਤ r = t.required.ਪਾਓ(Name_ਦੋਆਧਾਰੀ_ਅੰਕ(s));
					ਸਵੈਚਾਲਿਤ &v = r.ਪਹਿਲਾ->ਦੂਜਾ;
					ਜੇ (r.ਦੂਜਾ || v.ਪਹਿਲਾ < max) {
						v.ਪਹਿਲਾ = max;
						v.ਦੂਜਾ = Name_ਦੋਆਧਾਰੀ_ਅੰਕ(dst);
					}
				}
			}
		}

		ਲਈ (ਸਵੈਚਾਲਿਤ ਪੋਰਟ_name : module->ports) {
			ਸਵੈਚਾਲਿਤ wire = module->wire(ਪੋਰਟ_name);
			ਜੇ (wire->ਪੋਰਟ_ਨਿਵੇਸ਼) {
				t.has_inputs = true;
				ਰੋਕੋ;
			}
		}

		ਵਾਪਸ t;
	}

	decltype(ਅੰਕੜੇ)::const_iterator ਲੱਭੋ(RTLIL::IdString module_name) ਅਚਲ { ਵਾਪਸ ਅੰਕੜੇ.ਲੱਭੋ(module_name); }
	decltype(ਅੰਕੜੇ)::const_iterator ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ ਅੰਕੜੇ.ਸਿਰਾ(); }
	ਪੂਰਨ ਗਿਣਤੀ_2(RTLIL::IdString module_name) ਅਚਲ { ਵਾਪਸ ਅੰਕੜੇ.ਗਿਣਤੀ_2(module_name); }
	ਅਚਲ ModuleTiming& at(RTLIL::IdString module_name) ਅਚਲ { ਵਾਪਸ ਅੰਕੜੇ.at(module_name); }
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
