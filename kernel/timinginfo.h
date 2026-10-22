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
		පූර්ණ_සංඛ්‍යා offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, පූර්ණ_සංඛ්‍යා offset) : name(name), offset(offset) {}
		explicit NameBit(const RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool operator==(const NameBit& nb) const { ආපසු nb.name == name && nb.offset == offset; }
		bool operator!=(const NameBit& nb) const { ආපසු !operator==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			නම් (!cell->hasPort(port_name))
				ආපසු {};
			auto &port = cell->getPort(port_name);
			නම් (offset >= port.size())
				ආපසු {};
			ආපසු port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(name);
			h.eat(offset);
			ආපසු h;
		}
	};
	sa_nracanaa BitBit
	{
		NameBit first, second;
		BitBit(const NameBit &first, const NameBit &second) : first(first), second(second) {}
		BitBit(const SigBit &first, const SigBit &second) : first(first), second(second) {}
		bool operator==(const BitBit& bb) const { ආපසු bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(first);
			h.eat(second);
			ආපසු h;
		}
	};

	sa_nracanaa ModuleTiming
	{
		dict<BitBit, පූර්ණ_සංඛ්‍යා> comb;
		dict<NameBit, std::pair<පූර්ණ_සංඛ්‍යා,NameBit>> arrival, required;
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
		සඳහා (auto module : design->modules()) {
			නම් (!module->get_blackbox_attribute())
				continue;
			setup_module(module);
		}
	}

	const ModuleTiming& setup_module(RTLIL::Module *module)
	{
		auto r = data.insert(module->name);
		log_assert(r.second);
		auto &t = r.first->second;

		සඳහා (auto cell : module->cells()) {
			නම් (cell->type == ID($specify2)) {
				auto en = cell->getPort(ID::EN);
				නම් (en.is_fully_const() && !en.as_bool())
					continue;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST);
				සඳහා (const auto &c : src.chunks())
					නම් (!c.wire || !c.wire->port_input)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				සඳහා (const auto &c : dst.chunks())
					නම් (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				පූර්ණ_සංඛ්‍යා rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				පූර්ණ_සංඛ්‍යා fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				පූර්ණ_සංඛ්‍යා max = std::max(rise_max,fall_max);
				නම් (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				නම් (cell->getParam(ID::FULL).as_bool()) {
					සඳහා (const auto &s : src)
						සඳහා (const auto &d : dst) {
							auto r = t.comb.insert(BitBit(s,d));
							නම් (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				නැතහොත් {
					log_assert(GetSize(src) == GetSize(dst));
					සඳහා (auto i = 0; i < GetSize(src); i++) {
						const auto &s = src[i];
						const auto &d = dst[i];
						auto r = t.comb.insert(BitBit(s,d));
						නම් (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			නැතහොත් නම් (cell->type == ID($specify3)) {
				auto src = cell->getPort(ID::SRC).as_bit();
				auto dst = cell->getPort(ID::DST);
				නම් (!src.wire || !src.wire->port_input)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				සඳහා (const auto &c : dst.chunks())
					නම් (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				පූර්ණ_සංඛ්‍යා rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				පූර්ණ_සංඛ්‍යා fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				පූර්ණ_සංඛ්‍යා max = std::max(rise_max,fall_max);
				නම් (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				සඳහා (const auto &d : dst) {
					auto r = t.arrival.insert(NameBit(d));
					auto &v = r.first->second;
					නම් (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			නැතහොත් නම් (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				නම් (type != "$setup" && type != "$setuphold")
					continue;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST).as_bit();
				සඳහා (const auto &c : src.chunks())
					නම් (!c.wire || !c.wire->port_input)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				නම් (!dst.wire || !dst.wire->port_input)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				පූර්ණ_සංඛ්‍යා max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				නම් (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				සඳහා (const auto &s : src) {
					auto r = t.required.insert(NameBit(s));
					auto &v = r.first->second;
					නම් (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		සඳහා (auto port_name : module->ports) {
			auto wire = module->wire(port_name);
			නම් (wire->port_input) {
				t.has_inputs = true;
				bha_nga;
			}
		}

		ආපසු t;
	}

	decltype(data)::const_iterator find(RTLIL::IdString module_name) const { ආපසු data.find(module_name); }
	decltype(data)::const_iterator end() const { ආපසු data.end(); }
	පූර්ණ_සංඛ්‍යා count(RTLIL::IdString module_name) const { ආපසු data.count(module_name); }
	const ModuleTiming& at(RTLIL::IdString module_name) const { ආපසු data.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
