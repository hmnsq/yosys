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

struct TimingInfo
{
	struct NameBit
	{
		RTLIL::IdString name;
		정수 offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, 정수 offset) : name(name), offset(offset) {}
		explicit NameBit(const RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool operator==(const NameBit& nb) const { 반환 nb.name == name && nb.offset == offset; }
		bool operator!=(const NameBit& nb) const { 반환 !operator==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			만약 (!cell->hasPort(port_name))
				반환 {};
			auto &port = cell->getPort(port_name);
			만약 (offset >= port.size())
				반환 {};
			반환 port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(name);
			h.eat(offset);
			반환 h;
		}
	};
	struct BitBit
	{
		NameBit first, second;
		BitBit(const NameBit &first, const NameBit &second) : first(first), second(second) {}
		BitBit(const SigBit &first, const SigBit &second) : first(first), second(second) {}
		bool operator==(const BitBit& bb) const { 반환 bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(first);
			h.eat(second);
			반환 h;
		}
	};

	struct ModuleTiming
	{
		dict<BitBit, 정수> comb;
		dict<NameBit, std::pair<정수,NameBit>> arrival, required;
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
		위해 (auto module : design->modules()) {
			만약 (!module->get_blackbox_attribute())
				continue;
			setup_module(module);
		}
	}

	const ModuleTiming& setup_module(RTLIL::Module *module)
	{
		auto r = data.insert(module->name);
		log_assert(r.second);
		auto &t = r.first->second;

		위해 (auto cell : module->cells()) {
			만약 (cell->type == ID($specify2)) {
				auto en = cell->getPort(ID::EN);
				만약 (en.is_fully_const() && !en.as_bool())
					continue;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST);
				위해 (const auto &c : src.chunks())
					만약 (!c.wire || !c.wire->port_input)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				위해 (const auto &c : dst.chunks())
					만약 (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				정수 rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				정수 fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				정수 max = std::max(rise_max,fall_max);
				만약 (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				만약 (cell->getParam(ID::FULL).as_bool()) {
					위해 (const auto &s : src)
						위해 (const auto &d : dst) {
							auto r = t.comb.insert(BitBit(s,d));
							만약 (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				아니면 {
					log_assert(GetSize(src) == GetSize(dst));
					위해 (auto i = 0; i < GetSize(src); i++) {
						const auto &s = src[i];
						const auto &d = dst[i];
						auto r = t.comb.insert(BitBit(s,d));
						만약 (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			아니면 만약 (cell->type == ID($specify3)) {
				auto src = cell->getPort(ID::SRC).as_bit();
				auto dst = cell->getPort(ID::DST);
				만약 (!src.wire || !src.wire->port_input)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				위해 (const auto &c : dst.chunks())
					만약 (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				정수 rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				정수 fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				정수 max = std::max(rise_max,fall_max);
				만약 (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				위해 (const auto &d : dst) {
					auto r = t.arrival.insert(NameBit(d));
					auto &v = r.first->second;
					만약 (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			아니면 만약 (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				만약 (type != "$setup" && type != "$setuphold")
					continue;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST).as_bit();
				위해 (const auto &c : src.chunks())
					만약 (!c.wire || !c.wire->port_input)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				만약 (!dst.wire || !dst.wire->port_input)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				정수 max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				만약 (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				위해 (const auto &s : src) {
					auto r = t.required.insert(NameBit(s));
					auto &v = r.first->second;
					만약 (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		위해 (auto port_name : module->ports) {
			auto wire = module->wire(port_name);
			만약 (wire->port_input) {
				t.has_inputs = true;
				break;
			}
		}

		반환 t;
	}

	decltype(data)::const_iterator find(RTLIL::IdString module_name) const { 반환 data.find(module_name); }
	decltype(data)::const_iterator end() const { 반환 data.end(); }
	정수 count(RTLIL::IdString module_name) const { 반환 data.count(module_name); }
	const ModuleTiming& at(RTLIL::IdString module_name) const { 반환 data.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
