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

#నిర్వచితం_కాకపోతే TIMINGINFO_H
#నిర్వచించు TIMINGINFO_H

#అన్తర్భావ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

నిర్మాణం TimingInfo
{
	నిర్మాణం Name_బిట్టు
	{
		RTLIL::IdString name;
		పూర్ణాంకం స్థానభ్రంశం;
		Name_బిట్టు() : స్థానభ్రంశం(0) {}
		Name_బిట్టు(IdString name, పూర్ణాంకం స్థానభ్రంశం) : name(name), స్థానభ్రంశం(స్థానభ్రంశం) {}
		explicit Name_బిట్టు(స్థిరరాశి RTLIL::Sig_బిట్టు &b) : name(b.wire->name), స్థానభ్రంశం(b.స్థానభ్రంశం) {}
		bool క్రియాకారకం==(స్థిరరాశి Name_బిట్టు& nb) స్థిరరాశి { తిరిగి nb.name == name && nb.స్థానభ్రంశం == స్థానభ్రంశం; }
		bool క్రియాకారకం!=(స్థిరరాశి Name_బిట్టు& nb) స్థిరరాశి { తిరిగి !క్రియాకారకం==(nb); }
		std::optional<Sig_బిట్టు> get_connection(RTLIL::Cell *cell) {
			IdString పోర్టు_name = name;
			ఒకవేళ (!cell->has_పోర్టు(పోర్టు_name))
				తిరిగి {};
			స్వయంచాలక &పోర్టు = cell->get_పోర్టు(పోర్టు_name);
			ఒకవేళ (స్థానభ్రంశం >= పోర్టు.కొలత())
				తిరిగి {};
			తిరిగి పోర్టు[స్థానభ్రంశం];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
			h.eat(name);
			h.eat(స్థానభ్రంశం);
			తిరిగి h;
		}
	};
	నిర్మాణం బిట్టు_బిట్టు
	{
		Name_బిట్టు మొదటి, రెండవ;
		బిట్టు_బిట్టు(స్థిరరాశి Name_బిట్టు &మొదటి, స్థిరరాశి Name_బిట్టు &రెండవ) : మొదటి(మొదటి), రెండవ(రెండవ) {}
		బిట్టు_బిట్టు(స్థిరరాశి Sig_బిట్టు &మొదటి, స్థిరరాశి Sig_బిట్టు &రెండవ) : మొదటి(మొదటి), రెండవ(రెండవ) {}
		bool క్రియాకారకం==(స్థిరరాశి బిట్టు_బిట్టు& bb) స్థిరరాశి { తిరిగి bb.మొదటి == మొదటి && bb.రెండవ == రెండవ; }
		[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
			h.eat(మొదటి);
			h.eat(రెండవ);
			తిరిగి h;
		}
	};

	నిర్మాణం ModuleTiming
	{
		dict<బిట్టు_బిట్టు, పూర్ణాంకం> comb;
		dict<Name_బిట్టు, std::pair<పూర్ణాంకం,Name_బిట్టు>> రాక, required;
		bool has_inputs;
	};

	dict<RTLIL::IdString, ModuleTiming> దత్తాంశం;

	TimingInfo()
	{
	}

	TimingInfo(RTLIL::Design *design)
	{
		అమరిక(design);
	}

	శూన్య అమరిక(RTLIL::Design *design)
	{
		కొరకు (స్వయంచాలక module : design->modules()) {
			ఒకవేళ (!module->get_blackbox_attribute())
				కొనసాగు;
			అమరిక_module(module);
		}
	}

	స్థిరరాశి ModuleTiming& అమరిక_module(RTLIL::Module *module)
	{
		స్వయంచాలక r = దత్తాంశం.చొప్పించు(module->name);
		log_assert(r.రెండవ);
		స్వయంచాలక &t = r.మొదటి->రెండవ;

		కొరకు (స్వయంచాలక cell : module->cells()) {
			ఒకవేళ (cell->type == ID($specify2)) {
				స్వయంచాలక en = cell->get_పోర్టు(ID::EN);
				ఒకవేళ (en.is_fully_const() && !en.as_bool())
					కొనసాగు;
				స్వయంచాలక src = cell->get_పోర్టు(ID::SRC);
				స్వయంచాలక dst = cell->get_పోర్టు(ID::DST);
				కొరకు (స్థిరరాశి స్వయంచాలక &c : src.chunks())
					ఒకవేళ (!c.wire || !c.wire->పోర్టు_ఆదానం)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				కొరకు (స్థిరరాశి స్వయంచాలక &c : dst.chunks())
					ఒకవేళ (!c.wire || !c.wire->పోర్టు_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				పూర్ణాంకం rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				పూర్ణాంకం fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				పూర్ణాంకం max = std::max(rise_max,fall_max);
				ఒకవేళ (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				ఒకవేళ (cell->getParam(ID::నిండిన).as_bool()) {
					కొరకు (స్థిరరాశి స్వయంచాలక &s : src)
						కొరకు (స్థిరరాశి స్వయంచాలక &d : dst) {
							స్వయంచాలక r = t.comb.చొప్పించు(బిట్టు_బిట్టు(s,d));
							ఒకవేళ (!r.రెండవ)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.మొదటి->రెండవ = max;
						}
				}
				లేకపోతే {
					log_assert(Get_కొలత(src) == Get_కొలత(dst));
					కొరకు (స్వయంచాలక i = 0; i < Get_కొలత(src); i++) {
						స్థిరరాశి స్వయంచాలక &s = src[i];
						స్థిరరాశి స్వయంచాలక &d = dst[i];
						స్వయంచాలక r = t.comb.చొప్పించు(బిట్టు_బిట్టు(s,d));
						ఒకవేళ (!r.రెండవ)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.మొదటి->రెండవ = max;
					}
				}
			}
			లేకపోతే ఒకవేళ (cell->type == ID($specify3)) {
				స్వయంచాలక src = cell->get_పోర్టు(ID::SRC).as_బిట్టు();
				స్వయంచాలక dst = cell->get_పోర్టు(ID::DST);
				ఒకవేళ (!src.wire || !src.wire->పోర్టు_ఆదానం)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				కొరకు (స్థిరరాశి స్వయంచాలక &c : dst.chunks())
					ఒకవేళ (!c.wire->పోర్టు_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				పూర్ణాంకం rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				పూర్ణాంకం fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				పూర్ణాంకం max = std::max(rise_max,fall_max);
				ఒకవేళ (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				కొరకు (స్థిరరాశి స్వయంచాలక &d : dst) {
					స్వయంచాలక r = t.రాక.చొప్పించు(Name_బిట్టు(d));
					స్వయంచాలక &v = r.మొదటి->రెండవ;
					ఒకవేళ (r.రెండవ || v.మొదటి < max) {
						v.మొదటి = max;
						v.రెండవ = Name_బిట్టు(src);
					}
				}
			}
			లేకపోతే ఒకవేళ (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				ఒకవేళ (type != "$setup" && type != "$setuphold")
					కొనసాగు;
				స్వయంచాలక src = cell->get_పోర్టు(ID::SRC);
				స్వయంచాలక dst = cell->get_పోర్టు(ID::DST).as_బిట్టు();
				కొరకు (స్థిరరాశి స్వయంచాలక &c : src.chunks())
					ఒకవేళ (!c.wire || !c.wire->పోర్టు_ఆదానం)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				ఒకవేళ (!dst.wire || !dst.wire->పోర్టు_ఆదానం)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				పూర్ణాంకం max = cell->getParam(ID::T_హద్దు_MAX).as_int();
				ఒకవేళ (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				కొరకు (స్థిరరాశి స్వయంచాలక &s : src) {
					స్వయంచాలక r = t.required.చొప్పించు(Name_బిట్టు(s));
					స్వయంచాలక &v = r.మొదటి->రెండవ;
					ఒకవేళ (r.రెండవ || v.మొదటి < max) {
						v.మొదటి = max;
						v.రెండవ = Name_బిట్టు(dst);
					}
				}
			}
		}

		కొరకు (స్వయంచాలక పోర్టు_name : module->ports) {
			స్వయంచాలక wire = module->wire(పోర్టు_name);
			ఒకవేళ (wire->పోర్టు_ఆదానం) {
				t.has_inputs = true;
				ఆపు;
			}
		}

		తిరిగి t;
	}

	decltype(దత్తాంశం)::const_iterator కనుగొను(RTLIL::IdString module_name) స్థిరరాశి { తిరిగి దత్తాంశం.కనుగొను(module_name); }
	decltype(దత్తాంశం)::const_iterator చివర() స్థిరరాశి { తిరిగి దత్తాంశం.చివర(); }
	పూర్ణాంకం లెక్క(RTLIL::IdString module_name) స్థిరరాశి { తిరిగి దత్తాంశం.లెక్క(module_name); }
	స్థిరరాశి ModuleTiming& at(RTLIL::IdString module_name) స్థిరరాశి { తిరిగి దత్తాంశం.at(module_name); }
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
