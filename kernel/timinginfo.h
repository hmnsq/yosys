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
		पूर्णांक offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, पूर्णांक offset) : name(name), offset(offset) {}
		explicit NameBit(स्थिरांक RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool चालक==(स्थिरांक NameBit& nb) स्थिरांक { परत nb.name == name && nb.offset == offset; }
		bool चालक!=(स्थिरांक NameBit& nb) स्थिरांक { परत !चालक==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			जर (!cell->hasPort(port_name))
				परत {};
			स्वतः &port = cell->getPort(port_name);
			जर (offset >= port.size())
				परत {};
			परत port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक {
			h.eat(name);
			h.eat(offset);
			परत h;
		}
	};
	संरचना BitBit
	{
		NameBit first, second;
		BitBit(स्थिरांक NameBit &first, स्थिरांक NameBit &second) : first(first), second(second) {}
		BitBit(स्थिरांक SigBit &first, स्थिरांक SigBit &second) : first(first), second(second) {}
		bool चालक==(स्थिरांक BitBit& bb) स्थिरांक { परत bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक {
			h.eat(first);
			h.eat(second);
			परत h;
		}
	};

	संरचना ModuleTiming
	{
		dict<BitBit, पूर्णांक> comb;
		dict<NameBit, std::pair<पूर्णांक,NameBit>> arrival, required;
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
		साठी (स्वतः module : design->modules()) {
			जर (!module->get_blackbox_attribute())
				सुरूठेवा;
			setup_module(module);
		}
	}

	स्थिरांक ModuleTiming& setup_module(RTLIL::Module *module)
	{
		स्वतः r = data.घाला(module->name);
		log_assert(r.second);
		स्वतः &t = r.first->second;

		साठी (स्वतः cell : module->cells()) {
			जर (cell->type == ID($specify2)) {
				स्वतः en = cell->getPort(ID::EN);
				जर (en.is_fully_const() && !en.as_bool())
					सुरूठेवा;
				स्वतः src = cell->getPort(ID::SRC);
				स्वतः dst = cell->getPort(ID::DST);
				साठी (स्थिरांक स्वतः &c : src.chunks())
					जर (!c.wire || !c.wire->port_आदान)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				साठी (स्थिरांक स्वतः &c : dst.chunks())
					जर (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				पूर्णांक rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				पूर्णांक fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				पूर्णांक max = std::max(rise_max,fall_max);
				जर (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				जर (cell->getParam(ID::FULL).as_bool()) {
					साठी (स्थिरांक स्वतः &s : src)
						साठी (स्थिरांक स्वतः &d : dst) {
							स्वतः r = t.comb.घाला(BitBit(s,d));
							जर (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				अन्यथा {
					log_assert(GetSize(src) == GetSize(dst));
					साठी (स्वतः i = 0; i < GetSize(src); i++) {
						स्थिरांक स्वतः &s = src[i];
						स्थिरांक स्वतः &d = dst[i];
						स्वतः r = t.comb.घाला(BitBit(s,d));
						जर (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			अन्यथा जर (cell->type == ID($specify3)) {
				स्वतः src = cell->getPort(ID::SRC).as_bit();
				स्वतः dst = cell->getPort(ID::DST);
				जर (!src.wire || !src.wire->port_आदान)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				साठी (स्थिरांक स्वतः &c : dst.chunks())
					जर (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				पूर्णांक rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				पूर्णांक fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				पूर्णांक max = std::max(rise_max,fall_max);
				जर (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				साठी (स्थिरांक स्वतः &d : dst) {
					स्वतः r = t.arrival.घाला(NameBit(d));
					स्वतः &v = r.first->second;
					जर (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			अन्यथा जर (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				जर (type != "$setup" && type != "$setuphold")
					सुरूठेवा;
				स्वतः src = cell->getPort(ID::SRC);
				स्वतः dst = cell->getPort(ID::DST).as_bit();
				साठी (स्थिरांक स्वतः &c : src.chunks())
					जर (!c.wire || !c.wire->port_आदान)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				जर (!dst.wire || !dst.wire->port_आदान)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				पूर्णांक max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				जर (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				साठी (स्थिरांक स्वतः &s : src) {
					स्वतः r = t.required.घाला(NameBit(s));
					स्वतः &v = r.first->second;
					जर (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		साठी (स्वतः port_name : module->ports) {
			स्वतः wire = module->wire(port_name);
			जर (wire->port_आदान) {
				t.has_inputs = true;
				थांबा;
			}
		}

		परत t;
	}

	decltype(data)::const_iterator find(RTLIL::IdString module_name) स्थिरांक { परत data.find(module_name); }
	decltype(data)::const_iterator end() स्थिरांक { परत data.end(); }
	पूर्णांक मोजणी(RTLIL::IdString module_name) स्थिरांक { परत data.मोजणी(module_name); }
	स्थिरांक ModuleTiming& at(RTLIL::IdString module_name) स्थिरांक { परत data.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
