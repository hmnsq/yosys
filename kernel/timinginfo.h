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

#جے_بنا_تعریف TIMINGINFO_H
#تعریف_کرو TIMINGINFO_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية TimingInfo
{
	بنية Name_ثنائی_ہندسہ
	{
		RTLIL::IdString name;
		صحيح وتھ;
		Name_ثنائی_ہندسہ() : وتھ(0) {}
		Name_ثنائی_ہندسہ(IdString name, صحيح وتھ) : name(name), وتھ(وتھ) {}
		explicit Name_ثنائی_ہندسہ(ثابت RTLIL::Sig_ثنائی_ہندسہ &b) : name(b.wire->name), وتھ(b.وتھ) {}
		bool عامل==(ثابت Name_ثنائی_ہندسہ& nb) ثابت { أرجع nb.name == name && nb.وتھ == وتھ; }
		bool عامل!=(ثابت Name_ثنائی_ہندسہ& nb) ثابت { أرجع !عامل==(nb); }
		std::optional<Sig_ثنائی_ہندسہ> get_connection(RTLIL::Cell *cell) {
			IdString پورٹ_name = name;
			إذا (!cell->has_پورٹ(پورٹ_name))
				أرجع {};
			خودکار &پورٹ = cell->get_پورٹ(پورٹ_name);
			إذا (وتھ >= پورٹ.ناپ())
				أرجع {};
			أرجع پورٹ[وتھ];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(name);
			h.eat(وتھ);
			أرجع h;
		}
	};
	بنية ثنائی_ہندسہ_ثنائی_ہندسہ
	{
		Name_ثنائی_ہندسہ پہلا, دوجا;
		ثنائی_ہندسہ_ثنائی_ہندسہ(ثابت Name_ثنائی_ہندسہ &پہلا, ثابت Name_ثنائی_ہندسہ &دوجا) : پہلا(پہلا), دوجا(دوجا) {}
		ثنائی_ہندسہ_ثنائی_ہندسہ(ثابت Sig_ثنائی_ہندسہ &پہلا, ثابت Sig_ثنائی_ہندسہ &دوجا) : پہلا(پہلا), دوجا(دوجا) {}
		bool عامل==(ثابت ثنائی_ہندسہ_ثنائی_ہندسہ& bb) ثابت { أرجع bb.پہلا == پہلا && bb.دوجا == دوجا; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(پہلا);
			h.eat(دوجا);
			أرجع h;
		}
	};

	بنية ModuleTiming
	{
		dict<ثنائی_ہندسہ_ثنائی_ہندسہ, صحيح> comb;
		dict<Name_ثنائی_ہندسہ, std::pair<صحيح,Name_ثنائی_ہندسہ>> آمد, required;
		bool has_inputs;
	};

	dict<RTLIL::IdString, ModuleTiming> انکڑے;

	TimingInfo()
	{
	}

	TimingInfo(RTLIL::Design *design)
	{
		تیاری(design);
	}

	فراغ تیاری(RTLIL::Design *design)
	{
		لكل (خودکار module : design->modules()) {
			إذا (!module->get_blackbox_attribute())
				استمر;
			تیاری_module(module);
		}
	}

	ثابت ModuleTiming& تیاری_module(RTLIL::Module *module)
	{
		خودکار r = انکڑے.داخل_کرو(module->name);
		log_assert(r.دوجا);
		خودکار &t = r.پہلا->دوجا;

		لكل (خودکار cell : module->cells()) {
			إذا (cell->type == ID($specify2)) {
				خودکار en = cell->get_پورٹ(ID::EN);
				إذا (en.is_fully_const() && !en.as_bool())
					استمر;
				خودکار src = cell->get_پورٹ(ID::SRC);
				خودکار dst = cell->get_پورٹ(ID::DST);
				لكل (ثابت خودکار &c : src.chunks())
					إذا (!c.wire || !c.wire->پورٹ_داخلہ)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				لكل (ثابت خودکار &c : dst.chunks())
					إذا (!c.wire || !c.wire->پورٹ_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحيح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحيح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحيح max = std::max(rise_max,fall_max);
				إذا (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				إذا (cell->getParam(ID::بھریا).as_bool()) {
					لكل (ثابت خودکار &s : src)
						لكل (ثابت خودکار &d : dst) {
							خودکار r = t.comb.داخل_کرو(ثنائی_ہندسہ_ثنائی_ہندسہ(s,d));
							إذا (!r.دوجا)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.پہلا->دوجا = max;
						}
				}
				وإلا {
					log_assert(Get_ناپ(src) == Get_ناپ(dst));
					لكل (خودکار i = 0; i < Get_ناپ(src); i++) {
						ثابت خودکار &s = src[i];
						ثابت خودکار &d = dst[i];
						خودکار r = t.comb.داخل_کرو(ثنائی_ہندسہ_ثنائی_ہندسہ(s,d));
						إذا (!r.دوجا)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.پہلا->دوجا = max;
					}
				}
			}
			وإلا إذا (cell->type == ID($specify3)) {
				خودکار src = cell->get_پورٹ(ID::SRC).as_ثنائی_ہندسہ();
				خودکار dst = cell->get_پورٹ(ID::DST);
				إذا (!src.wire || !src.wire->پورٹ_داخلہ)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				لكل (ثابت خودکار &c : dst.chunks())
					إذا (!c.wire->پورٹ_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحيح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحيح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحيح max = std::max(rise_max,fall_max);
				إذا (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				لكل (ثابت خودکار &d : dst) {
					خودکار r = t.آمد.داخل_کرو(Name_ثنائی_ہندسہ(d));
					خودکار &v = r.پہلا->دوجا;
					إذا (r.دوجا || v.پہلا < max) {
						v.پہلا = max;
						v.دوجا = Name_ثنائی_ہندسہ(src);
					}
				}
			}
			وإلا إذا (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				إذا (type != "$setup" && type != "$setuphold")
					استمر;
				خودکار src = cell->get_پورٹ(ID::SRC);
				خودکار dst = cell->get_پورٹ(ID::DST).as_ثنائی_ہندسہ();
				لكل (ثابت خودکار &c : src.chunks())
					إذا (!c.wire || !c.wire->پورٹ_داخلہ)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				إذا (!dst.wire || !dst.wire->پورٹ_داخلہ)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				صحيح max = cell->getParam(ID::T_حد_بندی_MAX).as_int();
				إذا (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				لكل (ثابت خودکار &s : src) {
					خودکار r = t.required.داخل_کرو(Name_ثنائی_ہندسہ(s));
					خودکار &v = r.پہلا->دوجا;
					إذا (r.دوجا || v.پہلا < max) {
						v.پہلا = max;
						v.دوجا = Name_ثنائی_ہندسہ(dst);
					}
				}
			}
		}

		لكل (خودکار پورٹ_name : module->ports) {
			خودکار wire = module->wire(پورٹ_name);
			إذا (wire->پورٹ_داخلہ) {
				t.has_inputs = true;
				اكسر;
			}
		}

		أرجع t;
	}

	decltype(انکڑے)::const_iterator لبھو(RTLIL::IdString module_name) ثابت { أرجع انکڑے.لبھو(module_name); }
	decltype(انکڑے)::const_iterator سرا() ثابت { أرجع انکڑے.سرا(); }
	صحيح گنتی(RTLIL::IdString module_name) ثابت { أرجع انکڑے.گنتی(module_name); }
	ثابت ModuleTiming& at(RTLIL::IdString module_name) ثابت { أرجع انکڑے.at(module_name); }
};

YOSYS_NAMESPACE_سرا

#جے_انت
