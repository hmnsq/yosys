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

#अगर_अघोषित TIMINGINFO_H
#घोषणा TIMINGINFO_H

#समावेश "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

काष्ठा TimingInfo
{
	काष्ठा NameBit
	{
		RTLIL::IdString name;
		पूर्णांक offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, पूर्णांक offset) : name(name), offset(offset) {}
		explicit NameBit(स्थिर RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool चालक==(स्थिर NameBit& nb) स्थिर { वापस nb.name == name && nb.offset == offset; }
		bool चालक!=(स्थिर NameBit& nb) स्थिर { वापस !चालक==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			अगर (!cell->hasPort(port_name))
				वापस {};
			स्वतः &port = cell->getPort(port_name);
			अगर (offset >= port.आकार())
				वापस {};
			वापस port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
			h.eat(name);
			h.eat(offset);
			वापस h;
		}
	};
	काष्ठा BitBit
	{
		NameBit first, second;
		BitBit(स्थिर NameBit &first, स्थिर NameBit &second) : first(first), second(second) {}
		BitBit(स्थिर SigBit &first, स्थिर SigBit &second) : first(first), second(second) {}
		bool चालक==(स्थिर BitBit& bb) स्थिर { वापस bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
			h.eat(first);
			h.eat(second);
			वापस h;
		}
	};

	काष्ठा ModuleTiming
	{
		dict<BitBit, पूर्णांक> comb;
		dict<NameBit, std::pair<पूर्णांक,NameBit>> arrival, required;
		bool has_inputs;
	};

	dict<RTLIL::IdString, ModuleTiming> तथ्य;

	TimingInfo()
	{
	}

	TimingInfo(RTLIL::Design *design)
	{
		setup(design);
	}

	व्योम setup(RTLIL::Design *design)
	{
		क्रम (स्वतः module : design->modules()) {
			अगर (!module->get_blackbox_attribute())
				जारी;
			setup_module(module);
		}
	}

	स्थिर ModuleTiming& setup_module(RTLIL::Module *module)
	{
		स्वतः r = तथ्य.जोड़ो(module->name);
		log_assert(r.second);
		स्वतः &t = r.first->second;

		क्रम (स्वतः cell : module->cells()) {
			अगर (cell->type == ID($specify2)) {
				स्वतः en = cell->getPort(ID::EN);
				अगर (en.is_fully_const() && !en.as_bool())
					जारी;
				स्वतः src = cell->getPort(ID::SRC);
				स्वतः dst = cell->getPort(ID::DST);
				क्रम (स्थिर स्वतः &c : src.chunks())
					अगर (!c.wire || !c.wire->port_निवेश)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				क्रम (स्थिर स्वतः &c : dst.chunks())
					अगर (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				पूर्णांक rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				पूर्णांक fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				पूर्णांक max = std::max(rise_max,fall_max);
				अगर (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				अगर (cell->getParam(ID::FULL).as_bool()) {
					क्रम (स्थिर स्वतः &s : src)
						क्रम (स्थिर स्वतः &d : dst) {
							स्वतः r = t.comb.जोड़ो(BitBit(s,d));
							अगर (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				अन्यथा {
					log_assert(Get_आकार(src) == Get_आकार(dst));
					क्रम (स्वतः i = 0; i < Get_आकार(src); i++) {
						स्थिर स्वतः &s = src[i];
						स्थिर स्वतः &d = dst[i];
						स्वतः r = t.comb.जोड़ो(BitBit(s,d));
						अगर (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			अन्यथा अगर (cell->type == ID($specify3)) {
				स्वतः src = cell->getPort(ID::SRC).as_bit();
				स्वतः dst = cell->getPort(ID::DST);
				अगर (!src.wire || !src.wire->port_निवेश)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				क्रम (स्थिर स्वतः &c : dst.chunks())
					अगर (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				पूर्णांक rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				पूर्णांक fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				पूर्णांक max = std::max(rise_max,fall_max);
				अगर (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				क्रम (स्थिर स्वतः &d : dst) {
					स्वतः r = t.arrival.जोड़ो(NameBit(d));
					स्वतः &v = r.first->second;
					अगर (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			अन्यथा अगर (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				अगर (type != "$setup" && type != "$setuphold")
					जारी;
				स्वतः src = cell->getPort(ID::SRC);
				स्वतः dst = cell->getPort(ID::DST).as_bit();
				क्रम (स्थिर स्वतः &c : src.chunks())
					अगर (!c.wire || !c.wire->port_निवेश)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				अगर (!dst.wire || !dst.wire->port_निवेश)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				पूर्णांक max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				अगर (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				क्रम (स्थिर स्वतः &s : src) {
					स्वतः r = t.required.जोड़ो(NameBit(s));
					स्वतः &v = r.first->second;
					अगर (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		क्रम (स्वतः port_name : module->ports) {
			स्वतः wire = module->wire(port_name);
			अगर (wire->port_निवेश) {
				t.has_inputs = true;
				अवरोध;
			}
		}

		वापस t;
	}

	decltype(तथ्य)::const_iterator find(RTLIL::IdString module_name) स्थिर { वापस तथ्य.find(module_name); }
	decltype(तथ्य)::const_iterator end() स्थिर { वापस तथ्य.end(); }
	पूर्णांक गिनती(RTLIL::IdString module_name) स्थिर { वापस तथ्य.गिनती(module_name); }
	स्थिर ModuleTiming& at(RTLIL::IdString module_name) स्थिर { वापस तथ्य.at(module_name); }
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
