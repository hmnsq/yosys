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

#اگر_تعریف_نشده TIMINGINFO_H
#تعریف_کن TIMINGINFO_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية TimingInfo
{
	بنية Name_بیت
	{
		RTLIL::IdString name;
		صحيح جابه_جایی;
		Name_بیت() : جابه_جایی(0) {}
		Name_بیت(IdString name, صحيح جابه_جایی) : name(name), جابه_جایی(جابه_جایی) {}
		explicit Name_بیت(ثابت RTLIL::Sig_بیت &b) : name(b.wire->name), جابه_جایی(b.جابه_جایی) {}
		bool عملگر==(ثابت Name_بیت& nb) ثابت { أرجع nb.name == name && nb.جابه_جایی == جابه_جایی; }
		bool عملگر!=(ثابت Name_بیت& nb) ثابت { أرجع !عملگر==(nb); }
		std::optional<Sig_بیت> get_connection(RTLIL::Cell *cell) {
			IdString پورت_name = name;
			إذا (!cell->has_پورت(پورت_name))
				أرجع {};
			خودکار &پورت = cell->get_پورت(پورت_name);
			إذا (جابه_جایی >= پورت.اندازه())
				أرجع {};
			أرجع پورت[جابه_جایی];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(name);
			h.eat(جابه_جایی);
			أرجع h;
		}
	};
	بنية بیت_بیت
	{
		Name_بیت اولی, دومی;
		بیت_بیت(ثابت Name_بیت &اولی, ثابت Name_بیت &دومی) : اولی(اولی), دومی(دومی) {}
		بیت_بیت(ثابت Sig_بیت &اولی, ثابت Sig_بیت &دومی) : اولی(اولی), دومی(دومی) {}
		bool عملگر==(ثابت بیت_بیت& bb) ثابت { أرجع bb.اولی == اولی && bb.دومی == دومی; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(اولی);
			h.eat(دومی);
			أرجع h;
		}
	};

	بنية ModuleTiming
	{
		dict<بیت_بیت, صحيح> comb;
		dict<Name_بیت, std::pair<صحيح,Name_بیت>> رسیدن, required;
		bool has_inputs;
	};

	dict<RTLIL::IdString, ModuleTiming> داده;

	TimingInfo()
	{
	}

	TimingInfo(RTLIL::Design *design)
	{
		راهاندازی(design);
	}

	فراغ راهاندازی(RTLIL::Design *design)
	{
		لكل (خودکار module : design->modules()) {
			إذا (!module->get_blackbox_attribute())
				استمر;
			راهاندازی_module(module);
		}
	}

	ثابت ModuleTiming& راهاندازی_module(RTLIL::Module *module)
	{
		خودکار r = داده.درج(module->name);
		log_assert(r.دومی);
		خودکار &t = r.اولی->دومی;

		لكل (خودکار cell : module->cells()) {
			إذا (cell->type == ID($specify2)) {
				خودکار en = cell->get_پورت(ID::EN);
				إذا (en.is_fully_const() && !en.as_bool())
					استمر;
				خودکار src = cell->get_پورت(ID::SRC);
				خودکار dst = cell->get_پورت(ID::DST);
				لكل (ثابت خودکار &c : src.chunks())
					إذا (!c.wire || !c.wire->پورت_درون_داد)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				لكل (ثابت خودکار &c : dst.chunks())
					إذا (!c.wire || !c.wire->پورت_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحيح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحيح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحيح max = std::max(rise_max,fall_max);
				إذا (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				إذا (cell->getParam(ID::پر).as_bool()) {
					لكل (ثابت خودکار &s : src)
						لكل (ثابت خودکار &d : dst) {
							خودکار r = t.comb.درج(بیت_بیت(s,d));
							إذا (!r.دومی)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.اولی->دومی = max;
						}
				}
				وإلا {
					log_assert(Get_اندازه(src) == Get_اندازه(dst));
					لكل (خودکار i = 0; i < Get_اندازه(src); i++) {
						ثابت خودکار &s = src[i];
						ثابت خودکار &d = dst[i];
						خودکار r = t.comb.درج(بیت_بیت(s,d));
						إذا (!r.دومی)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.اولی->دومی = max;
					}
				}
			}
			وإلا إذا (cell->type == ID($specify3)) {
				خودکار src = cell->get_پورت(ID::SRC).as_بیت();
				خودکار dst = cell->get_پورت(ID::DST);
				إذا (!src.wire || !src.wire->پورت_درون_داد)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				لكل (ثابت خودکار &c : dst.chunks())
					إذا (!c.wire->پورت_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحيح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحيح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحيح max = std::max(rise_max,fall_max);
				إذا (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				لكل (ثابت خودکار &d : dst) {
					خودکار r = t.رسیدن.درج(Name_بیت(d));
					خودکار &v = r.اولی->دومی;
					إذا (r.دومی || v.اولی < max) {
						v.اولی = max;
						v.دومی = Name_بیت(src);
					}
				}
			}
			وإلا إذا (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				إذا (type != "$setup" && type != "$setuphold")
					استمر;
				خودکار src = cell->get_پورت(ID::SRC);
				خودکار dst = cell->get_پورت(ID::DST).as_بیت();
				لكل (ثابت خودکار &c : src.chunks())
					إذا (!c.wire || !c.wire->پورت_درون_داد)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				إذا (!dst.wire || !dst.wire->پورت_درون_داد)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				صحيح max = cell->getParam(ID::T_حد_MAX).as_int();
				إذا (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				لكل (ثابت خودکار &s : src) {
					خودکار r = t.required.درج(Name_بیت(s));
					خودکار &v = r.اولی->دومی;
					إذا (r.دومی || v.اولی < max) {
						v.اولی = max;
						v.دومی = Name_بیت(dst);
					}
				}
			}
		}

		لكل (خودکار پورت_name : module->ports) {
			خودکار wire = module->wire(پورت_name);
			إذا (wire->پورت_درون_داد) {
				t.has_inputs = true;
				اكسر;
			}
		}

		أرجع t;
	}

	decltype(داده)::const_iterator پیدا_کن(RTLIL::IdString module_name) ثابت { أرجع داده.پیدا_کن(module_name); }
	decltype(داده)::const_iterator انتها() ثابت { أرجع داده.انتها(); }
	صحيح شمار(RTLIL::IdString module_name) ثابت { أرجع داده.شمار(module_name); }
	ثابت ModuleTiming& at(RTLIL::IdString module_name) ثابت { أرجع داده.at(module_name); }
};

YOSYS_NAMESPACE_انتها

#پایان_اگر
