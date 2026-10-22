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

#અન્તર્ભાવ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

સંરચના TimingInfo
{
	સંરચના NameBit
	{
		RTLIL::IdString name;
		પૂર્ણાંક offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, પૂર્ણાંક offset) : name(name), offset(offset) {}
		explicit NameBit(અચલ RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool operator==(અચલ NameBit& nb) અચલ { પાછા nb.name == name && nb.offset == offset; }
		bool operator!=(અચલ NameBit& nb) અચલ { પાછા !operator==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			જો (!cell->hasPort(port_name))
				પાછા {};
			auto &port = cell->getPort(port_name);
			જો (offset >= port.size())
				પાછા {};
			પાછા port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
			h.eat(name);
			h.eat(offset);
			પાછા h;
		}
	};
	સંરચના BitBit
	{
		NameBit first, second;
		BitBit(અચલ NameBit &first, અચલ NameBit &second) : first(first), second(second) {}
		BitBit(અચલ SigBit &first, અચલ SigBit &second) : first(first), second(second) {}
		bool operator==(અચલ BitBit& bb) અચલ { પાછા bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
			h.eat(first);
			h.eat(second);
			પાછા h;
		}
	};

	સંરચના ModuleTiming
	{
		dict<BitBit, પૂર્ણાંક> comb;
		dict<NameBit, std::pair<પૂર્ણાંક,NameBit>> arrival, required;
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

	શૂન્ય setup(RTLIL::Design *design)
	{
		માટે (auto module : design->modules()) {
			જો (!module->get_blackbox_attribute())
				ચાલુ;
			setup_module(module);
		}
	}

	અચલ ModuleTiming& setup_module(RTLIL::Module *module)
	{
		auto r = data.ઉમેરો(module->name);
		log_assert(r.second);
		auto &t = r.first->second;

		માટે (auto cell : module->cells()) {
			જો (cell->type == ID($specify2)) {
				auto en = cell->getPort(ID::EN);
				જો (en.is_fully_const() && !en.as_bool())
					ચાલુ;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST);
				માટે (અચલ auto &c : src.chunks())
					જો (!c.wire || !c.wire->port_આગત)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				માટે (અચલ auto &c : dst.chunks())
					જો (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				પૂર્ણાંક rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				પૂર્ણાંક fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				પૂર્ણાંક max = std::max(rise_max,fall_max);
				જો (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				જો (cell->getParam(ID::FULL).as_bool()) {
					માટે (અચલ auto &s : src)
						માટે (અચલ auto &d : dst) {
							auto r = t.comb.ઉમેરો(BitBit(s,d));
							જો (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				નહીંતર {
					log_assert(GetSize(src) == GetSize(dst));
					માટે (auto i = 0; i < GetSize(src); i++) {
						અચલ auto &s = src[i];
						અચલ auto &d = dst[i];
						auto r = t.comb.ઉમેરો(BitBit(s,d));
						જો (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			નહીંતર જો (cell->type == ID($specify3)) {
				auto src = cell->getPort(ID::SRC).as_bit();
				auto dst = cell->getPort(ID::DST);
				જો (!src.wire || !src.wire->port_આગત)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				માટે (અચલ auto &c : dst.chunks())
					જો (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				પૂર્ણાંક rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				પૂર્ણાંક fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				પૂર્ણાંક max = std::max(rise_max,fall_max);
				જો (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				માટે (અચલ auto &d : dst) {
					auto r = t.arrival.ઉમેરો(NameBit(d));
					auto &v = r.first->second;
					જો (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			નહીંતર જો (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				જો (type != "$setup" && type != "$setuphold")
					ચાલુ;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST).as_bit();
				માટે (અચલ auto &c : src.chunks())
					જો (!c.wire || !c.wire->port_આગત)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				જો (!dst.wire || !dst.wire->port_આગત)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				પૂર્ણાંક max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				જો (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				માટે (અચલ auto &s : src) {
					auto r = t.required.ઉમેરો(NameBit(s));
					auto &v = r.first->second;
					જો (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		માટે (auto port_name : module->ports) {
			auto wire = module->wire(port_name);
			જો (wire->port_આગત) {
				t.has_inputs = true;
				તોડો;
			}
		}

		પાછા t;
	}

	decltype(data)::const_iterator find(RTLIL::IdString module_name) અચલ { પાછા data.find(module_name); }
	decltype(data)::const_iterator end() અચલ { પાછા data.end(); }
	પૂર્ણાંક ગણતરી(RTLIL::IdString module_name) અચલ { પાછા data.ગણતરી(module_name); }
	અચલ ModuleTiming& at(RTLIL::IdString module_name) અચલ { પાછા data.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
