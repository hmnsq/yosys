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
		整数 offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, 整数 offset) : name(name), offset(offset) {}
		explicit NameBit(const RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool operator==(const NameBit& nb) const { 返回 nb.name == name && nb.offset == offset; }
		bool operator!=(const NameBit& nb) const { 返回 !operator==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			如果 (!cell->hasPort(port_name))
				返回 {};
			auto &port = cell->getPort(port_name);
			如果 (offset >= port.size())
				返回 {};
			返回 port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(name);
			h.eat(offset);
			返回 h;
		}
	};
	struct BitBit
	{
		NameBit first, second;
		BitBit(const NameBit &first, const NameBit &second) : first(first), second(second) {}
		BitBit(const SigBit &first, const SigBit &second) : first(first), second(second) {}
		bool operator==(const BitBit& bb) const { 返回 bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(first);
			h.eat(second);
			返回 h;
		}
	};

	struct ModuleTiming
	{
		dict<BitBit, 整数> comb;
		dict<NameBit, std::pair<整数,NameBit>> arrival, required;
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
		为 (auto module : design->modules()) {
			如果 (!module->get_blackbox_attribute())
				continue;
			setup_module(module);
		}
	}

	const ModuleTiming& setup_module(RTLIL::Module *module)
	{
		auto r = data.insert(module->name);
		log_assert(r.second);
		auto &t = r.first->second;

		为 (auto cell : module->cells()) {
			如果 (cell->type == ID($specify2)) {
				auto en = cell->getPort(ID::EN);
				如果 (en.is_fully_const() && !en.as_bool())
					continue;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST);
				为 (const auto &c : src.chunks())
					如果 (!c.wire || !c.wire->port_input)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				为 (const auto &c : dst.chunks())
					如果 (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				整数 rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				整数 fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				整数 max = std::max(rise_max,fall_max);
				如果 (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				如果 (cell->getParam(ID::FULL).as_bool()) {
					为 (const auto &s : src)
						为 (const auto &d : dst) {
							auto r = t.comb.insert(BitBit(s,d));
							如果 (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				否则 {
					log_assert(GetSize(src) == GetSize(dst));
					为 (auto i = 0; i < GetSize(src); i++) {
						const auto &s = src[i];
						const auto &d = dst[i];
						auto r = t.comb.insert(BitBit(s,d));
						如果 (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			否则 如果 (cell->type == ID($specify3)) {
				auto src = cell->getPort(ID::SRC).as_bit();
				auto dst = cell->getPort(ID::DST);
				如果 (!src.wire || !src.wire->port_input)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				为 (const auto &c : dst.chunks())
					如果 (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				整数 rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				整数 fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				整数 max = std::max(rise_max,fall_max);
				如果 (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				为 (const auto &d : dst) {
					auto r = t.arrival.insert(NameBit(d));
					auto &v = r.first->second;
					如果 (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			否则 如果 (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				如果 (type != "$setup" && type != "$setuphold")
					continue;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST).as_bit();
				为 (const auto &c : src.chunks())
					如果 (!c.wire || !c.wire->port_input)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				如果 (!dst.wire || !dst.wire->port_input)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				整数 max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				如果 (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				为 (const auto &s : src) {
					auto r = t.required.insert(NameBit(s));
					auto &v = r.first->second;
					如果 (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		为 (auto port_name : module->ports) {
			auto wire = module->wire(port_name);
			如果 (wire->port_input) {
				t.has_inputs = true;
				break;
			}
		}

		返回 t;
	}

	decltype(data)::const_iterator find(RTLIL::IdString module_name) const { 返回 data.find(module_name); }
	decltype(data)::const_iterator end() const { 返回 data.end(); }
	整数 count(RTLIL::IdString module_name) const { 返回 data.count(module_name); }
	const ModuleTiming& at(RTLIL::IdString module_name) const { 返回 data.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
