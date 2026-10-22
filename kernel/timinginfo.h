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

#جيڪڏهن_وصف_نه_ٿيل TIMINGINFO_H
#وصف_ڪريو TIMINGINFO_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية TimingInfo
{
	بنية Name_ٻٽو_انگ
	{
		RTLIL::IdString name;
		صحيح وٿي;
		Name_ٻٽو_انگ() : وٿي(0) {}
		Name_ٻٽو_انگ(IdString name, صحيح وٿي) : name(name), وٿي(وٿي) {}
		explicit Name_ٻٽو_انگ(ثابت RTLIL::Sig_ٻٽو_انگ &b) : name(b.wire->name), وٿي(b.وٿي) {}
		bool عامل==(ثابت Name_ٻٽو_انگ& nb) ثابت { أرجع nb.name == name && nb.وٿي == وٿي; }
		bool عامل!=(ثابت Name_ٻٽو_انگ& nb) ثابت { أرجع !عامل==(nb); }
		std::optional<Sig_ٻٽو_انگ> get_connection(RTLIL::Cell *cell) {
			IdString پورٽ_name = name;
			إذا (!cell->has_پورٽ(پورٽ_name))
				أرجع {};
			خودڪار &پورٽ = cell->get_پورٽ(پورٽ_name);
			إذا (وٿي >= پورٽ.سائيز())
				أرجع {};
			أرجع پورٽ[وٿي];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(name);
			h.eat(وٿي);
			أرجع h;
		}
	};
	بنية ٻٽو_انگ_ٻٽو_انگ
	{
		Name_ٻٽو_انگ پهريون, ٻيو;
		ٻٽو_انگ_ٻٽو_انگ(ثابت Name_ٻٽو_انگ &پهريون, ثابت Name_ٻٽو_انگ &ٻيو) : پهريون(پهريون), ٻيو(ٻيو) {}
		ٻٽو_انگ_ٻٽو_انگ(ثابت Sig_ٻٽو_انگ &پهريون, ثابت Sig_ٻٽو_انگ &ٻيو) : پهريون(پهريون), ٻيو(ٻيو) {}
		bool عامل==(ثابت ٻٽو_انگ_ٻٽو_انگ& bb) ثابت { أرجع bb.پهريون == پهريون && bb.ٻيو == ٻيو; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(پهريون);
			h.eat(ٻيو);
			أرجع h;
		}
	};

	بنية ModuleTiming
	{
		dict<ٻٽو_انگ_ٻٽو_انگ, صحيح> comb;
		dict<Name_ٻٽو_انگ, std::pair<صحيح,Name_ٻٽو_انگ>> آمد, required;
		bool has_inputs;
	};

	dict<RTLIL::IdString, ModuleTiming> معلومات;

	TimingInfo()
	{
	}

	TimingInfo(RTLIL::Design *design)
	{
		تياري(design);
	}

	فراغ تياري(RTLIL::Design *design)
	{
		لكل (خودڪار module : design->modules()) {
			إذا (!module->get_blackbox_attribute())
				استمر;
			تياري_module(module);
		}
	}

	ثابت ModuleTiming& تياري_module(RTLIL::Module *module)
	{
		خودڪار r = معلومات.وجهو(module->name);
		log_assert(r.ٻيو);
		خودڪار &t = r.پهريون->ٻيو;

		لكل (خودڪار cell : module->cells()) {
			إذا (cell->type == ID($specify2)) {
				خودڪار en = cell->get_پورٽ(ID::EN);
				إذا (en.is_fully_const() && !en.as_bool())
					استمر;
				خودڪار src = cell->get_پورٽ(ID::SRC);
				خودڪار dst = cell->get_پورٽ(ID::DST);
				لكل (ثابت خودڪار &c : src.chunks())
					إذا (!c.wire || !c.wire->پورٽ_داخلا)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				لكل (ثابت خودڪار &c : dst.chunks())
					إذا (!c.wire || !c.wire->پورٽ_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحيح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحيح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحيح max = std::max(rise_max,fall_max);
				إذا (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				إذا (cell->getParam(ID::ڀريل).as_bool()) {
					لكل (ثابت خودڪار &s : src)
						لكل (ثابت خودڪار &d : dst) {
							خودڪار r = t.comb.وجهو(ٻٽو_انگ_ٻٽو_انگ(s,d));
							إذا (!r.ٻيو)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.پهريون->ٻيو = max;
						}
				}
				وإلا {
					log_assert(Get_سائيز(src) == Get_سائيز(dst));
					لكل (خودڪار i = 0; i < Get_سائيز(src); i++) {
						ثابت خودڪار &s = src[i];
						ثابت خودڪار &d = dst[i];
						خودڪار r = t.comb.وجهو(ٻٽو_انگ_ٻٽو_انگ(s,d));
						إذا (!r.ٻيو)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.پهريون->ٻيو = max;
					}
				}
			}
			وإلا إذا (cell->type == ID($specify3)) {
				خودڪار src = cell->get_پورٽ(ID::SRC).as_ٻٽو_انگ();
				خودڪار dst = cell->get_پورٽ(ID::DST);
				إذا (!src.wire || !src.wire->پورٽ_داخلا)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				لكل (ثابت خودڪار &c : dst.chunks())
					إذا (!c.wire->پورٽ_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحيح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحيح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحيح max = std::max(rise_max,fall_max);
				إذا (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				لكل (ثابت خودڪار &d : dst) {
					خودڪار r = t.آمد.وجهو(Name_ٻٽو_انگ(d));
					خودڪار &v = r.پهريون->ٻيو;
					إذا (r.ٻيو || v.پهريون < max) {
						v.پهريون = max;
						v.ٻيو = Name_ٻٽو_انگ(src);
					}
				}
			}
			وإلا إذا (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				إذا (type != "$setup" && type != "$setuphold")
					استمر;
				خودڪار src = cell->get_پورٽ(ID::SRC);
				خودڪار dst = cell->get_پورٽ(ID::DST).as_ٻٽو_انگ();
				لكل (ثابت خودڪار &c : src.chunks())
					إذا (!c.wire || !c.wire->پورٽ_داخلا)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				إذا (!dst.wire || !dst.wire->پورٽ_داخلا)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				صحيح max = cell->getParam(ID::T_حد_بندي_MAX).as_int();
				إذا (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				لكل (ثابت خودڪار &s : src) {
					خودڪار r = t.required.وجهو(Name_ٻٽو_انگ(s));
					خودڪار &v = r.پهريون->ٻيو;
					إذا (r.ٻيو || v.پهريون < max) {
						v.پهريون = max;
						v.ٻيو = Name_ٻٽو_انگ(dst);
					}
				}
			}
		}

		لكل (خودڪار پورٽ_name : module->ports) {
			خودڪار wire = module->wire(پورٽ_name);
			إذا (wire->پورٽ_داخلا) {
				t.has_inputs = true;
				اكسر;
			}
		}

		أرجع t;
	}

	decltype(معلومات)::const_iterator ڳوليو(RTLIL::IdString module_name) ثابت { أرجع معلومات.ڳوليو(module_name); }
	decltype(معلومات)::const_iterator پڇاڙي() ثابت { أرجع معلومات.پڇاڙي(); }
	صحيح ڳڻپ(RTLIL::IdString module_name) ثابت { أرجع معلومات.ڳڻپ(module_name); }
	ثابت ModuleTiming& at(RTLIL::IdString module_name) ثابت { أرجع معلومات.at(module_name); }
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
