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

#ifndef TIMINGINFO_H
#define TIMINGINFO_H

#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

संरचना TimingInfo
{
	संरचना NameBit
	{
		RTLIL::IdString name;
		पूर्णाङ्क offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, पूर्णाङ्क offset) : name(name), offset(offset) {}
		explicit NameBit(ध्रुव RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool चालक==(ध्रुव NameBit& nb) ध्रुव { प्रत्यागम nb.name == name && nb.offset == offset; }
		bool चालक!=(ध्रुव NameBit& nb) ध्रुव { प्रत्यागम !चालक==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			यदि (!cell->hasPort(port_name))
				प्रत्यागम {};
			स्वतः &port = cell->getPort(port_name);
			यदि (offset >= port.size())
				प्रत्यागम {};
			प्रत्यागम port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(name);
			h.eat(offset);
			प्रत्यागम h;
		}
	};
	संरचना BitBit
	{
		NameBit first, second;
		BitBit(ध्रुव NameBit &first, ध्रुव NameBit &second) : first(first), second(second) {}
		BitBit(ध्रुव SigBit &first, ध्रुव SigBit &second) : first(first), second(second) {}
		bool चालक==(ध्रुव BitBit& bb) ध्रुव { प्रत्यागम bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(first);
			h.eat(second);
			प्रत्यागम h;
		}
	};

	संरचना ModuleTiming
	{
		dict<BitBit, पूर्णाङ्क> comb;
		dict<NameBit, std::pair<पूर्णाङ्क,NameBit>> arrival, required;
		bool has_inputs;
	};

	dict<RTLIL::IdString, ModuleTiming> data;

	TimingInfo()
	{
	}

	TimingInfo(RTLIL::Design *design)
	{
		setup(design);
	}

	शून्य setup(RTLIL::Design *design)
	{
		प्रति (स्वतः module : design->modules()) {
			यदि (!module->get_blackbox_attribute())
				अनुवर्त;
			setup_module(module);
		}
	}

	ध्रुव ModuleTiming& setup_module(RTLIL::Module *module)
	{
		स्वतः r = data.जोड़ो(module->name);
		log_assert(r.second);
		स्वतः &t = r.first->second;

		प्रति (स्वतः cell : module->cells()) {
			यदि (cell->type == ID($specify2)) {
				स्वतः en = cell->getPort(ID::EN);
				यदि (en.is_fully_const() && !en.as_bool())
					अनुवर्त;
				स्वतः src = cell->getPort(ID::SRC);
				स्वतः dst = cell->getPort(ID::DST);
				प्रति (ध्रुव स्वतः &c : src.chunks())
					यदि (!c.wire || !c.wire->port_निवेश)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				प्रति (ध्रुव स्वतः &c : dst.chunks())
					यदि (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				पूर्णाङ्क rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				पूर्णाङ्क fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				पूर्णाङ्क max = std::max(rise_max,fall_max);
				यदि (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				यदि (cell->getParam(ID::FULL).as_bool()) {
					प्रति (ध्रुव स्वतः &s : src)
						प्रति (ध्रुव स्वतः &d : dst) {
							स्वतः r = t.comb.जोड़ो(BitBit(s,d));
							यदि (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				अन्यथा {
					log_assert(GetSize(src) == GetSize(dst));
					प्रति (स्वतः i = 0; i < GetSize(src); i++) {
						ध्रुव स्वतः &s = src[i];
						ध्रुव स्वतः &d = dst[i];
						स्वतः r = t.comb.जोड़ो(BitBit(s,d));
						यदि (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			अन्यथा यदि (cell->type == ID($specify3)) {
				स्वतः src = cell->getPort(ID::SRC).as_bit();
				स्वतः dst = cell->getPort(ID::DST);
				यदि (!src.wire || !src.wire->port_निवेश)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				प्रति (ध्रुव स्वतः &c : dst.chunks())
					यदि (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				पूर्णाङ्क rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				पूर्णाङ्क fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				पूर्णाङ्क max = std::max(rise_max,fall_max);
				यदि (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				प्रति (ध्रुव स्वतः &d : dst) {
					स्वतः r = t.arrival.जोड़ो(NameBit(d));
					स्वतः &v = r.first->second;
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
				स्वतः src = cell->getPort(ID::SRC);
				स्वतः dst = cell->getPort(ID::DST).as_bit();
				प्रति (ध्रुव स्वतः &c : src.chunks())
					यदि (!c.wire || !c.wire->port_निवेश)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				यदि (!dst.wire || !dst.wire->port_निवेश)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				पूर्णाङ्क max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				यदि (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				प्रति (ध्रुव स्वतः &s : src) {
					स्वतः r = t.required.जोड़ो(NameBit(s));
					स्वतः &v = r.first->second;
					यदि (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		प्रति (स्वतः port_name : module->ports) {
			स्वतः wire = module->wire(port_name);
			यदि (wire->port_निवेश) {
				t.has_inputs = true;
				भङ्ग;
			}
		}

		प्रत्यागम t;
	}

	decltype(data)::const_iterator find(RTLIL::IdString module_name) ध्रुव { प्रत्यागम data.find(module_name); }
	decltype(data)::const_iterator end() ध्रुव { प्रत्यागम data.end(); }
	पूर्णाङ्क गिनती(RTLIL::IdString module_name) ध्रुव { प्रत्यागम data.गिनती(module_name); }
	ध्रुव ModuleTiming& at(RTLIL::IdString module_name) ध्रुव { प्रत्यागम data.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
