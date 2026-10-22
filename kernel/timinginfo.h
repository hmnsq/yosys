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

#که_نه_تعریف_شوی TIMINGINFO_H
#تعریف_کړه TIMINGINFO_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية TimingInfo
{
	بنية Name_بټ
	{
		RTLIL::IdString name;
		صحيح دننه_واټن;
		Name_بټ() : دننه_واټن(0) {}
		Name_بټ(IdString name, صحيح دننه_واټن) : name(name), دننه_واټن(دننه_واټن) {}
		explicit Name_بټ(ثابت RTLIL::Sig_بټ &b) : name(b.wire->name), دننه_واټن(b.دننه_واټن) {}
		bool چلوونکی==(ثابت Name_بټ& nb) ثابت { أرجع nb.name == name && nb.دننه_واټن == دننه_واټن; }
		bool چلوونکی!=(ثابت Name_بټ& nb) ثابت { أرجع !چلوونکی==(nb); }
		std::optional<Sig_بټ> get_connection(RTLIL::Cell *cell) {
			IdString پورټ_name = name;
			إذا (!cell->has_پورټ(پورټ_name))
				أرجع {};
			خپلکاری &پورټ = cell->get_پورټ(پورټ_name);
			إذا (دننه_واټن >= پورټ.اندازه())
				أرجع {};
			أرجع پورټ[دننه_واټن];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(name);
			h.eat(دننه_واټن);
			أرجع h;
		}
	};
	بنية بټ_بټ
	{
		Name_بټ لومړی, دوهم;
		بټ_بټ(ثابت Name_بټ &لومړی, ثابت Name_بټ &دوهم) : لومړی(لومړی), دوهم(دوهم) {}
		بټ_بټ(ثابت Sig_بټ &لومړی, ثابت Sig_بټ &دوهم) : لومړی(لومړی), دوهم(دوهم) {}
		bool چلوونکی==(ثابت بټ_بټ& bb) ثابت { أرجع bb.لومړی == لومړی && bb.دوهم == دوهم; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(لومړی);
			h.eat(دوهم);
			أرجع h;
		}
	};

	بنية ModuleTiming
	{
		dict<بټ_بټ, صحيح> comb;
		dict<Name_بټ, std::pair<صحيح,Name_بټ>> رارسېدنه, required;
		bool has_inputs;
	};

	dict<RTLIL::IdString, ModuleTiming> معلومات;

	TimingInfo()
	{
	}

	TimingInfo(RTLIL::Design *design)
	{
		تیاری(design);
	}

	فراغ تیاری(RTLIL::Design *design)
	{
		لكل (خپلکاری module : design->modules()) {
			إذا (!module->get_blackbox_attribute())
				استمر;
			تیاری_module(module);
		}
	}

	ثابت ModuleTiming& تیاری_module(RTLIL::Module *module)
	{
		خپلکاری r = معلومات.ورننباسه(module->name);
		log_assert(r.دوهم);
		خپلکاری &t = r.لومړی->دوهم;

		لكل (خپلکاری cell : module->cells()) {
			إذا (cell->type == ID($specify2)) {
				خپلکاری en = cell->get_پورټ(ID::EN);
				إذا (en.is_fully_const() && !en.as_bool())
					استمر;
				خپلکاری src = cell->get_پورټ(ID::SRC);
				خپلکاری dst = cell->get_پورټ(ID::DST);
				لكل (ثابت خپلکاری &c : src.chunks())
					إذا (!c.wire || !c.wire->پورټ_ننوت)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				لكل (ثابت خپلکاری &c : dst.chunks())
					إذا (!c.wire || !c.wire->پورټ_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحيح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحيح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحيح max = std::max(rise_max,fall_max);
				إذا (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				إذا (cell->getParam(ID::ډک).as_bool()) {
					لكل (ثابت خپلکاری &s : src)
						لكل (ثابت خپلکاری &d : dst) {
							خپلکاری r = t.comb.ورننباسه(بټ_بټ(s,d));
							إذا (!r.دوهم)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.لومړی->دوهم = max;
						}
				}
				وإلا {
					log_assert(Get_اندازه(src) == Get_اندازه(dst));
					لكل (خپلکاری i = 0; i < Get_اندازه(src); i++) {
						ثابت خپلکاری &s = src[i];
						ثابت خپلکاری &d = dst[i];
						خپلکاری r = t.comb.ورننباسه(بټ_بټ(s,d));
						إذا (!r.دوهم)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.لومړی->دوهم = max;
					}
				}
			}
			وإلا إذا (cell->type == ID($specify3)) {
				خپلکاری src = cell->get_پورټ(ID::SRC).as_بټ();
				خپلکاری dst = cell->get_پورټ(ID::DST);
				إذا (!src.wire || !src.wire->پورټ_ننوت)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				لكل (ثابت خپلکاری &c : dst.chunks())
					إذا (!c.wire->پورټ_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحيح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحيح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحيح max = std::max(rise_max,fall_max);
				إذا (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				لكل (ثابت خپلکاری &d : dst) {
					خپلکاری r = t.رارسېدنه.ورننباسه(Name_بټ(d));
					خپلکاری &v = r.لومړی->دوهم;
					إذا (r.دوهم || v.لومړی < max) {
						v.لومړی = max;
						v.دوهم = Name_بټ(src);
					}
				}
			}
			وإلا إذا (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				إذا (type != "$setup" && type != "$setuphold")
					استمر;
				خپلکاری src = cell->get_پورټ(ID::SRC);
				خپلکاری dst = cell->get_پورټ(ID::DST).as_بټ();
				لكل (ثابت خپلکاری &c : src.chunks())
					إذا (!c.wire || !c.wire->پورټ_ننوت)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				إذا (!dst.wire || !dst.wire->پورټ_ننوت)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				صحيح max = cell->getParam(ID::T_حد_MAX).as_int();
				إذا (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				لكل (ثابت خپلکاری &s : src) {
					خپلکاری r = t.required.ورننباسه(Name_بټ(s));
					خپلکاری &v = r.لومړی->دوهم;
					إذا (r.دوهم || v.لومړی < max) {
						v.لومړی = max;
						v.دوهم = Name_بټ(dst);
					}
				}
			}
		}

		لكل (خپلکاری پورټ_name : module->ports) {
			خپلکاری wire = module->wire(پورټ_name);
			إذا (wire->پورټ_ننوت) {
				t.has_inputs = true;
				اكسر;
			}
		}

		أرجع t;
	}

	decltype(معلومات)::const_iterator پیدا_کړه(RTLIL::IdString module_name) ثابت { أرجع معلومات.پیدا_کړه(module_name); }
	decltype(معلومات)::const_iterator پای_ټکی() ثابت { أرجع معلومات.پای_ټکی(); }
	صحيح شمېر(RTLIL::IdString module_name) ثابت { أرجع معلومات.شمېر(module_name); }
	ثابت ModuleTiming& at(RTLIL::IdString module_name) ثابت { أرجع معلومات.at(module_name); }
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
