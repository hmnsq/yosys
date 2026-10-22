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

#অন্তর্ভাব "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

কাঠামো TimingInfo
{
	কাঠামো NameBit
	{
		RTLIL::IdString name;
		পূর্ণসংখ্যা offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, পূর্ণসংখ্যা offset) : name(name), offset(offset) {}
		explicit NameBit(ধ্রুবক RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool operator==(ধ্রুবক NameBit& nb) ধ্রুবক { ফেরত nb.name == name && nb.offset == offset; }
		bool operator!=(ধ্রুবক NameBit& nb) ধ্রুবক { ফেরত !operator==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			যদি (!cell->hasPort(port_name))
				ফেরত {};
			auto &port = cell->getPort(port_name);
			যদি (offset >= port.আকার_2())
				ফেরত {};
			ফেরত port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক {
			h.eat(name);
			h.eat(offset);
			ফেরত h;
		}
	};
	কাঠামো BitBit
	{
		NameBit first, second;
		BitBit(ধ্রুবক NameBit &first, ধ্রুবক NameBit &second) : first(first), second(second) {}
		BitBit(ধ্রুবক SigBit &first, ধ্রুবক SigBit &second) : first(first), second(second) {}
		bool operator==(ধ্রুবক BitBit& bb) ধ্রুবক { ফেরত bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক {
			h.eat(first);
			h.eat(second);
			ফেরত h;
		}
	};

	কাঠামো ModuleTiming
	{
		dict<BitBit, পূর্ণসংখ্যা> comb;
		dict<NameBit, std::pair<পূর্ণসংখ্যা,NameBit>> arrival, required;
		bool has_inputs;
	};

	dict<RTLIL::IdString, ModuleTiming> তথ্য;

	TimingInfo()
	{
	}

	TimingInfo(RTLIL::Design *design)
	{
		setup(design);
	}

	শূন্য setup(RTLIL::Design *design)
	{
		জন্য (auto module : design->modules()) {
			যদি (!module->get_blackbox_attribute())
				চালু;
			setup_module(module);
		}
	}

	ধ্রুবক ModuleTiming& setup_module(RTLIL::Module *module)
	{
		auto r = তথ্য.ঢোকাও(module->name);
		log_assert(r.second);
		auto &t = r.first->second;

		জন্য (auto cell : module->cells()) {
			যদি (cell->type == ID($specify2)) {
				auto en = cell->getPort(ID::EN);
				যদি (en.is_fully_const() && !en.as_bool())
					চালু;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST);
				জন্য (ধ্রুবক auto &c : src.chunks())
					যদি (!c.wire || !c.wire->port_প্রবেশ)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				জন্য (ধ্রুবক auto &c : dst.chunks())
					যদি (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				পূর্ণসংখ্যা rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				পূর্ণসংখ্যা fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				পূর্ণসংখ্যা max = std::max(rise_max,fall_max);
				যদি (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				যদি (cell->getParam(ID::FULL).as_bool()) {
					জন্য (ধ্রুবক auto &s : src)
						জন্য (ধ্রুবক auto &d : dst) {
							auto r = t.comb.ঢোকাও(BitBit(s,d));
							যদি (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				অন্যথা {
					log_assert(Get_আকার(src) == Get_আকার(dst));
					জন্য (auto i = 0; i < Get_আকার(src); i++) {
						ধ্রুবক auto &s = src[i];
						ধ্রুবক auto &d = dst[i];
						auto r = t.comb.ঢোকাও(BitBit(s,d));
						যদি (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			অন্যথা যদি (cell->type == ID($specify3)) {
				auto src = cell->getPort(ID::SRC).as_bit();
				auto dst = cell->getPort(ID::DST);
				যদি (!src.wire || !src.wire->port_প্রবেশ)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				জন্য (ধ্রুবক auto &c : dst.chunks())
					যদি (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				পূর্ণসংখ্যা rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				পূর্ণসংখ্যা fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				পূর্ণসংখ্যা max = std::max(rise_max,fall_max);
				যদি (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				জন্য (ধ্রুবক auto &d : dst) {
					auto r = t.arrival.ঢোকাও(NameBit(d));
					auto &v = r.first->second;
					যদি (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			অন্যথা যদি (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				যদি (type != "$setup" && type != "$setuphold")
					চালু;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST).as_bit();
				জন্য (ধ্রুবক auto &c : src.chunks())
					যদি (!c.wire || !c.wire->port_প্রবেশ)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				যদি (!dst.wire || !dst.wire->port_প্রবেশ)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				পূর্ণসংখ্যা max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				যদি (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				জন্য (ধ্রুবক auto &s : src) {
					auto r = t.required.ঢোকাও(NameBit(s));
					auto &v = r.first->second;
					যদি (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		জন্য (auto port_name : module->ports) {
			auto wire = module->wire(port_name);
			যদি (wire->port_প্রবেশ) {
				t.has_inputs = true;
				বিরতি;
			}
		}

		ফেরত t;
	}

	decltype(তথ্য)::const_iterator find(RTLIL::IdString module_name) ধ্রুবক { ফেরত তথ্য.find(module_name); }
	decltype(তথ্য)::const_iterator end() ধ্রুবক { ফেরত তথ্য.end(); }
	পূর্ণসংখ্যা গুনতি(RTLIL::IdString module_name) ধ্রুবক { ফেরত তথ্য.গুনতি(module_name); }
	ধ্রুবক ModuleTiming& at(RTLIL::IdString module_name) ধ্রুবক { ফেরত তথ্য.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
