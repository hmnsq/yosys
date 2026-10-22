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

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية TimingInfo
{
	بنية NameBit
	{
		RTLIL::IdString name;
		صحيح offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, صحيح offset) : name(name), offset(offset) {}
		explicit NameBit(ثابت RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool operator==(ثابت NameBit& nb) ثابت { أرجع nb.name == name && nb.offset == offset; }
		bool operator!=(ثابت NameBit& nb) ثابت { أرجع !operator==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			إذا (!cell->hasPort(port_name))
				أرجع {};
			auto &port = cell->getPort(port_name);
			إذا (offset >= port.size())
				أرجع {};
			أرجع port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(name);
			h.eat(offset);
			أرجع h;
		}
	};
	بنية BitBit
	{
		NameBit first, second;
		BitBit(ثابت NameBit &first, ثابت NameBit &second) : first(first), second(second) {}
		BitBit(ثابت SigBit &first, ثابت SigBit &second) : first(first), second(second) {}
		bool operator==(ثابت BitBit& bb) ثابت { أرجع bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(first);
			h.eat(second);
			أرجع h;
		}
	};

	بنية ModuleTiming
	{
		dict<BitBit, صحيح> comb;
		dict<NameBit, std::pair<صحيح,NameBit>> arrival, required;
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

	فراغ setup(RTLIL::Design *design)
	{
		لكل (auto module : design->modules()) {
			إذا (!module->get_blackbox_attribute())
				استمر;
			setup_module(module);
		}
	}

	ثابت ModuleTiming& setup_module(RTLIL::Module *module)
	{
		auto r = data.سيسيڤ(module->name);
		log_assert(r.second);
		auto &t = r.first->second;

		لكل (auto cell : module->cells()) {
			إذا (cell->type == ID($specify2)) {
				auto en = cell->getPort(ID::EN);
				إذا (en.is_fully_const() && !en.as_bool())
					استمر;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST);
				لكل (ثابت auto &c : src.chunks())
					إذا (!c.wire || !c.wire->port_ماسوقن)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				لكل (ثابت auto &c : dst.chunks())
					إذا (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحيح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحيح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحيح max = std::max(rise_max,fall_max);
				إذا (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				إذا (cell->getParam(ID::FULL).as_bool()) {
					لكل (ثابت auto &s : src)
						لكل (ثابت auto &d : dst) {
							auto r = t.comb.سيسيڤ(BitBit(s,d));
							إذا (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				وإلا {
					log_assert(GetSize(src) == GetSize(dst));
					لكل (auto i = 0; i < GetSize(src); i++) {
						ثابت auto &s = src[i];
						ثابت auto &d = dst[i];
						auto r = t.comb.سيسيڤ(BitBit(s,d));
						إذا (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			وإلا إذا (cell->type == ID($specify3)) {
				auto src = cell->getPort(ID::SRC).as_bit();
				auto dst = cell->getPort(ID::DST);
				إذا (!src.wire || !src.wire->port_ماسوقن)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				لكل (ثابت auto &c : dst.chunks())
					إذا (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحيح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحيح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحيح max = std::max(rise_max,fall_max);
				إذا (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				لكل (ثابت auto &d : dst) {
					auto r = t.arrival.سيسيڤ(NameBit(d));
					auto &v = r.first->second;
					إذا (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			وإلا إذا (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				إذا (type != "$setup" && type != "$setuphold")
					استمر;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST).as_bit();
				لكل (ثابت auto &c : src.chunks())
					إذا (!c.wire || !c.wire->port_ماسوقن)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				إذا (!dst.wire || !dst.wire->port_ماسوقن)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				صحيح max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				إذا (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				لكل (ثابت auto &s : src) {
					auto r = t.required.سيسيڤ(NameBit(s));
					auto &v = r.first->second;
					إذا (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		لكل (auto port_name : module->ports) {
			auto wire = module->wire(port_name);
			إذا (wire->port_ماسوقن) {
				t.has_inputs = true;
				اكسر;
			}
		}

		أرجع t;
	}

	decltype(data)::const_iterator find(RTLIL::IdString module_name) ثابت { أرجع data.find(module_name); }
	decltype(data)::const_iterator end() ثابت { أرجع data.end(); }
	صحيح کيراءن(RTLIL::IdString module_name) ثابت { أرجع data.کيراءن(module_name); }
	ثابت ModuleTiming& at(RTLIL::IdString module_name) ثابت { أرجع data.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
