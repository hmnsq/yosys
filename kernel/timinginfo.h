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

#include "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

sa_nracanaa TimingInfo
{
	sa_nracanaa NameBit
	{
		RTLIL::IdString name;
		ကိန်းပြည့် offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, ကိန်းပြည့် offset) : name(name), offset(offset) {}
		explicit NameBit(const RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool operator==(const NameBit& nb) const { ပြန်ပေး nb.name == name && nb.offset == offset; }
		bool operator!=(const NameBit& nb) const { ပြန်ပေး !operator==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			အကယ်၍ (!cell->hasPort(port_name))
				ပြန်ပေး {};
			auto &port = cell->getPort(port_name);
			အကယ်၍ (offset >= port.size())
				ပြန်ပေး {};
			ပြန်ပေး port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(name);
			h.eat(offset);
			ပြန်ပေး h;
		}
	};
	sa_nracanaa BitBit
	{
		NameBit first, second;
		BitBit(const NameBit &first, const NameBit &second) : first(first), second(second) {}
		BitBit(const SigBit &first, const SigBit &second) : first(first), second(second) {}
		bool operator==(const BitBit& bb) const { ပြန်ပေး bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(first);
			h.eat(second);
			ပြန်ပေး h;
		}
	};

	sa_nracanaa ModuleTiming
	{
		dict<BitBit, ကိန်းပြည့်> comb;
		dict<NameBit, std::pair<ကိန်းပြည့်,NameBit>> arrival, required;
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

	void setup(RTLIL::Design *design)
	{
		အတွက် (auto module : design->modules()) {
			အကယ်၍ (!module->get_blackbox_attribute())
				continue;
			setup_module(module);
		}
	}

	const ModuleTiming& setup_module(RTLIL::Module *module)
	{
		auto r = data.insert(module->name);
		log_assert(r.second);
		auto &t = r.first->second;

		အတွက် (auto cell : module->cells()) {
			အကယ်၍ (cell->type == ID($specify2)) {
				auto en = cell->getPort(ID::EN);
				အကယ်၍ (en.is_fully_const() && !en.as_bool())
					continue;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST);
				အတွက် (const auto &c : src.chunks())
					အကယ်၍ (!c.wire || !c.wire->port_input)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				အတွက် (const auto &c : dst.chunks())
					အကယ်၍ (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				ကိန်းပြည့် rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				ကိန်းပြည့် fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				ကိန်းပြည့် max = std::max(rise_max,fall_max);
				အကယ်၍ (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				အကယ်၍ (cell->getParam(ID::FULL).as_bool()) {
					အတွက် (const auto &s : src)
						အတွက် (const auto &d : dst) {
							auto r = t.comb.insert(BitBit(s,d));
							အကယ်၍ (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				မဟုတ်ပါက {
					log_assert(GetSize(src) == GetSize(dst));
					အတွက် (auto i = 0; i < GetSize(src); i++) {
						const auto &s = src[i];
						const auto &d = dst[i];
						auto r = t.comb.insert(BitBit(s,d));
						အကယ်၍ (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			မဟုတ်ပါက အကယ်၍ (cell->type == ID($specify3)) {
				auto src = cell->getPort(ID::SRC).as_bit();
				auto dst = cell->getPort(ID::DST);
				အကယ်၍ (!src.wire || !src.wire->port_input)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				အတွက် (const auto &c : dst.chunks())
					အကယ်၍ (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				ကိန်းပြည့် rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				ကိန်းပြည့် fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				ကိန်းပြည့် max = std::max(rise_max,fall_max);
				အကယ်၍ (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				အတွက် (const auto &d : dst) {
					auto r = t.arrival.insert(NameBit(d));
					auto &v = r.first->second;
					အကယ်၍ (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			မဟုတ်ပါက အကယ်၍ (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				အကယ်၍ (type != "$setup" && type != "$setuphold")
					continue;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST).as_bit();
				အတွက် (const auto &c : src.chunks())
					အကယ်၍ (!c.wire || !c.wire->port_input)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				အကယ်၍ (!dst.wire || !dst.wire->port_input)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				ကိန်းပြည့် max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				အကယ်၍ (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				အတွက် (const auto &s : src) {
					auto r = t.required.insert(NameBit(s));
					auto &v = r.first->second;
					အကယ်၍ (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		အတွက် (auto port_name : module->ports) {
			auto wire = module->wire(port_name);
			အကယ်၍ (wire->port_input) {
				t.has_inputs = true;
				bha_nga;
			}
		}

		ပြန်ပေး t;
	}

	decltype(data)::const_iterator find(RTLIL::IdString module_name) const { ပြန်ပေး data.find(module_name); }
	decltype(data)::const_iterator end() const { ပြန်ပေး data.end(); }
	ကိန်းပြည့် count(RTLIL::IdString module_name) const { ပြန်ပေး data.count(module_name); }
	const ModuleTiming& at(RTLIL::IdString module_name) const { ပြန်ပေး data.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
