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

#यदि_अपरिभाषितम् TIMINGINFO_H
#परिभाषय TIMINGINFO_H

#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

संरचना TimingInfo
{
	संरचना NameBit
	{
		RTLIL::IdString name;
		पूर्णाङ्क विस्थापनम्;
		NameBit() : विस्थापनम्(0) {}
		NameBit(IdString name, पूर्णाङ्क विस्थापनम्) : name(name), विस्थापनम्(विस्थापनम्) {}
		explicit NameBit(ध्रुव RTLIL::SigBit &b) : name(b.wire->name), विस्थापनम्(b.विस्थापनम्) {}
		bool संक्रियकः==(ध्रुव NameBit& nb) ध्रुव { प्रत्यागम nb.name == name && nb.विस्थापनम् == विस्थापनम्; }
		bool संक्रियकः!=(ध्रुव NameBit& nb) ध्रुव { प्रत्यागम !संक्रियकः==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString पत्तनम्_name = name;
			यदि (!cell->has_पत्तनम्(पत्तनम्_name))
				प्रत्यागम {};
			स्वचालित &पत्तनम् = cell->get_पत्तनम्(पत्तनम्_name);
			यदि (विस्थापनम् >= पत्तनम्.आकार())
				प्रत्यागम {};
			प्रत्यागम पत्तनम्[विस्थापनम्];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(name);
			h.eat(विस्थापनम्);
			प्रत्यागम h;
		}
	};
	संरचना BitBit
	{
		NameBit first, second;
		BitBit(ध्रुव NameBit &first, ध्रुव NameBit &second) : first(first), second(second) {}
		BitBit(ध्रुव SigBit &first, ध्रुव SigBit &second) : first(first), second(second) {}
		bool संक्रियकः==(ध्रुव BitBit& bb) ध्रुव { प्रत्यागम bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(first);
			h.eat(second);
			प्रत्यागम h;
		}
	};

	संरचना ModuleTiming
	{
		dict<BitBit, पूर्णाङ्क> comb;
		dict<NameBit, std::pair<पूर्णाङ्क,NameBit>> आगमनम्, required;
		bool has_inputs;
	};

	dict<RTLIL::IdString, ModuleTiming> दत्त;

	TimingInfo()
	{
	}

	TimingInfo(RTLIL::Design *design)
	{
		सज्जता(design);
	}

	शून्य सज्जता(RTLIL::Design *design)
	{
		प्रति (स्वचालित module : design->modules()) {
			यदि (!module->get_blackbox_attribute())
				अनुवर्त;
			सज्जता_module(module);
		}
	}

	ध्रुव ModuleTiming& सज्जता_module(RTLIL::Module *module)
	{
		स्वचालित r = दत्त.निवेशय(module->name);
		log_assert(r.second);
		स्वचालित &t = r.first->second;

		प्रति (स्वचालित cell : module->cells()) {
			यदि (cell->type == ID($specify2)) {
				स्वचालित en = cell->get_पत्तनम्(ID::EN);
				यदि (en.is_fully_const() && !en.as_bool())
					अनुवर्त;
				स्वचालित src = cell->get_पत्तनम्(ID::SRC);
				स्वचालित dst = cell->get_पत्तनम्(ID::DST);
				प्रति (ध्रुव स्वचालित &c : src.chunks())
					यदि (!c.wire || !c.wire->पत्तनम्_निवेशः)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				प्रति (ध्रुव स्वचालित &c : dst.chunks())
					यदि (!c.wire || !c.wire->पत्तनम्_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				पूर्णाङ्क rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				पूर्णाङ्क fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				पूर्णाङ्क max = std::max(rise_max,fall_max);
				यदि (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				यदि (cell->getParam(ID::पूर्णम्).as_bool()) {
					प्रति (ध्रुव स्वचालित &s : src)
						प्रति (ध्रुव स्वचालित &d : dst) {
							स्वचालित r = t.comb.निवेशय(BitBit(s,d));
							यदि (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				अन्यथा {
					log_assert(Get_आकार(src) == Get_आकार(dst));
					प्रति (स्वचालित i = 0; i < Get_आकार(src); i++) {
						ध्रुव स्वचालित &s = src[i];
						ध्रुव स्वचालित &d = dst[i];
						स्वचालित r = t.comb.निवेशय(BitBit(s,d));
						यदि (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			अन्यथा यदि (cell->type == ID($specify3)) {
				स्वचालित src = cell->get_पत्तनम्(ID::SRC).as_bit();
				स्वचालित dst = cell->get_पत्तनम्(ID::DST);
				यदि (!src.wire || !src.wire->पत्तनम्_निवेशः)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				प्रति (ध्रुव स्वचालित &c : dst.chunks())
					यदि (!c.wire->पत्तनम्_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				पूर्णाङ्क rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				पूर्णाङ्क fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				पूर्णाङ्क max = std::max(rise_max,fall_max);
				यदि (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				प्रति (ध्रुव स्वचालित &d : dst) {
					स्वचालित r = t.आगमनम्.निवेशय(NameBit(d));
					स्वचालित &v = r.first->second;
					यदि (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			अन्यथा यदि (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				यदि (type != "$setup" && type != "$setuphold")
					अनुवर्त;
				स्वचालित src = cell->get_पत्तनम्(ID::SRC);
				स्वचालित dst = cell->get_पत्तनम्(ID::DST).as_bit();
				प्रति (ध्रुव स्वचालित &c : src.chunks())
					यदि (!c.wire || !c.wire->पत्तनम्_निवेशः)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				यदि (!dst.wire || !dst.wire->पत्तनम्_निवेशः)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				पूर्णाङ्क max = cell->getParam(ID::T_सीमा_MAX).as_int();
				यदि (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				प्रति (ध्रुव स्वचालित &s : src) {
					स्वचालित r = t.required.निवेशय(NameBit(s));
					स्वचालित &v = r.first->second;
					यदि (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		प्रति (स्वचालित पत्तनम्_name : module->ports) {
			स्वचालित wire = module->wire(पत्तनम्_name);
			यदि (wire->पत्तनम्_निवेशः) {
				t.has_inputs = true;
				भङ्ग;
			}
		}

		प्रत्यागम t;
	}

	decltype(दत्त)::const_iterator find(RTLIL::IdString module_name) ध्रुव { प्रत्यागम दत्त.find(module_name); }
	decltype(दत्त)::const_iterator अन्तः() ध्रुव { प्रत्यागम दत्त.अन्तः(); }
	पूर्णाङ्क गणनम्(RTLIL::IdString module_name) ध्रुव { प्रत्यागम दत्त.गणनम्(module_name); }
	ध्रुव ModuleTiming& at(RTLIL::IdString module_name) ध्रुव { प्रत्यागम दत्त.at(module_name); }
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
