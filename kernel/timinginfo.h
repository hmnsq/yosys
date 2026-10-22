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

#اگر_غیر_تعریف_شدہ TIMINGINFO_H
#تعریف_کرو TIMINGINFO_H

#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ساخت TimingInfo
{
	ساخت Name_ثنائی_ہندسہ
	{
		RTLIL::IdString name;
		صحیح انحراف;
		Name_ثنائی_ہندسہ() : انحراف(0) {}
		Name_ثنائی_ہندسہ(IdString name, صحیح انحراف) : name(name), انحراف(انحراف) {}
		explicit Name_ثنائی_ہندسہ(ثابت RTLIL::Sig_ثنائی_ہندسہ &b) : name(b.wire->name), انحراف(b.انحراف) {}
		bool عامل==(ثابت Name_ثنائی_ہندسہ& nb) ثابت { واپس nb.name == name && nb.انحراف == انحراف; }
		bool عامل!=(ثابت Name_ثنائی_ہندسہ& nb) ثابت { واپس !عامل==(nb); }
		std::optional<Sig_ثنائی_ہندسہ> get_connection(RTLIL::Cell *cell) {
			IdString پورٹ_name = name;
			اگر (!cell->has_پورٹ(پورٹ_name))
				واپس {};
			خودکار &پورٹ = cell->get_پورٹ(پورٹ_name);
			اگر (انحراف >= پورٹ.سائز())
				واپس {};
			واپس پورٹ[انحراف];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(name);
			h.eat(انحراف);
			واپس h;
		}
	};
	ساخت ثنائی_ہندسہ_ثنائی_ہندسہ
	{
		Name_ثنائی_ہندسہ پہلا, دوسرا;
		ثنائی_ہندسہ_ثنائی_ہندسہ(ثابت Name_ثنائی_ہندسہ &پہلا, ثابت Name_ثنائی_ہندسہ &دوسرا) : پہلا(پہلا), دوسرا(دوسرا) {}
		ثنائی_ہندسہ_ثنائی_ہندسہ(ثابت Sig_ثنائی_ہندسہ &پہلا, ثابت Sig_ثنائی_ہندسہ &دوسرا) : پہلا(پہلا), دوسرا(دوسرا) {}
		bool عامل==(ثابت ثنائی_ہندسہ_ثنائی_ہندسہ& bb) ثابت { واپس bb.پہلا == پہلا && bb.دوسرا == دوسرا; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(پہلا);
			h.eat(دوسرا);
			واپس h;
		}
	};

	ساخت ModuleTiming
	{
		dict<ثنائی_ہندسہ_ثنائی_ہندسہ, صحیح> comb;
		dict<Name_ثنائی_ہندسہ, std::pair<صحیح,Name_ثنائی_ہندسہ>> آمد, required;
		bool has_inputs;
	};

	dict<RTLIL::IdString, ModuleTiming> ڈیٹا;

	TimingInfo()
	{
	}

	TimingInfo(RTLIL::Design *design)
	{
		تیاری(design);
	}

	خالی تیاری(RTLIL::Design *design)
	{
		برائے (خودکار module : design->modules()) {
			اگر (!module->get_blackbox_attribute())
				جاری;
			تیاری_module(module);
		}
	}

	ثابت ModuleTiming& تیاری_module(RTLIL::Module *module)
	{
		خودکار r = ڈیٹا.داخل_کرو(module->name);
		log_assert(r.دوسرا);
		خودکار &t = r.پہلا->دوسرا;

		برائے (خودکار cell : module->cells()) {
			اگر (cell->type == ID($specify2)) {
				خودکار en = cell->get_پورٹ(ID::EN);
				اگر (en.is_fully_const() && !en.as_bool())
					جاری;
				خودکار src = cell->get_پورٹ(ID::SRC);
				خودکار dst = cell->get_پورٹ(ID::DST);
				برائے (ثابت خودکار &c : src.chunks())
					اگر (!c.wire || !c.wire->پورٹ_داخلہ)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				برائے (ثابت خودکار &c : dst.chunks())
					اگر (!c.wire || !c.wire->پورٹ_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحیح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحیح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحیح max = std::max(rise_max,fall_max);
				اگر (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				اگر (cell->getParam(ID::بھرا).as_bool()) {
					برائے (ثابت خودکار &s : src)
						برائے (ثابت خودکار &d : dst) {
							خودکار r = t.comb.داخل_کرو(ثنائی_ہندسہ_ثنائی_ہندسہ(s,d));
							اگر (!r.دوسرا)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.پہلا->دوسرا = max;
						}
				}
				ورنہ {
					log_assert(Get_سائز(src) == Get_سائز(dst));
					برائے (خودکار i = 0; i < Get_سائز(src); i++) {
						ثابت خودکار &s = src[i];
						ثابت خودکار &d = dst[i];
						خودکار r = t.comb.داخل_کرو(ثنائی_ہندسہ_ثنائی_ہندسہ(s,d));
						اگر (!r.دوسرا)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.پہلا->دوسرا = max;
					}
				}
			}
			ورنہ اگر (cell->type == ID($specify3)) {
				خودکار src = cell->get_پورٹ(ID::SRC).as_ثنائی_ہندسہ();
				خودکار dst = cell->get_پورٹ(ID::DST);
				اگر (!src.wire || !src.wire->پورٹ_داخلہ)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				برائے (ثابت خودکار &c : dst.chunks())
					اگر (!c.wire->پورٹ_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحیح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحیح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحیح max = std::max(rise_max,fall_max);
				اگر (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				برائے (ثابت خودکار &d : dst) {
					خودکار r = t.آمد.داخل_کرو(Name_ثنائی_ہندسہ(d));
					خودکار &v = r.پہلا->دوسرا;
					اگر (r.دوسرا || v.پہلا < max) {
						v.پہلا = max;
						v.دوسرا = Name_ثنائی_ہندسہ(src);
					}
				}
			}
			ورنہ اگر (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				اگر (type != "$setup" && type != "$setuphold")
					جاری;
				خودکار src = cell->get_پورٹ(ID::SRC);
				خودکار dst = cell->get_پورٹ(ID::DST).as_ثنائی_ہندسہ();
				برائے (ثابت خودکار &c : src.chunks())
					اگر (!c.wire || !c.wire->پورٹ_داخلہ)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				اگر (!dst.wire || !dst.wire->پورٹ_داخلہ)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				صحیح max = cell->getParam(ID::T_حد_بندی_MAX).as_int();
				اگر (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				برائے (ثابت خودکار &s : src) {
					خودکار r = t.required.داخل_کرو(Name_ثنائی_ہندسہ(s));
					خودکار &v = r.پہلا->دوسرا;
					اگر (r.دوسرا || v.پہلا < max) {
						v.پہلا = max;
						v.دوسرا = Name_ثنائی_ہندسہ(dst);
					}
				}
			}
		}

		برائے (خودکار پورٹ_name : module->ports) {
			خودکار wire = module->wire(پورٹ_name);
			اگر (wire->پورٹ_داخلہ) {
				t.has_inputs = true;
				توڑو;
			}
		}

		واپس t;
	}

	decltype(ڈیٹا)::const_iterator ڈھونڈو(RTLIL::IdString module_name) ثابت { واپس ڈیٹا.ڈھونڈو(module_name); }
	decltype(ڈیٹا)::const_iterator انتہا() ثابت { واپس ڈیٹا.انتہا(); }
	صحیح گنتی(RTLIL::IdString module_name) ثابت { واپس ڈیٹا.گنتی(module_name); }
	ثابت ModuleTiming& at(RTLIL::IdString module_name) ثابت { واپس ڈیٹا.at(module_name); }
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
