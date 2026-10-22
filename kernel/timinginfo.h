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

#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ساختار TimingInfo
{
	ساختار NameBit
	{
		RTLIL::IdString name;
		صحیح offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, صحیح offset) : name(name), offset(offset) {}
		explicit NameBit(ثابت RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool operator==(ثابت NameBit& nb) ثابت { بازگشت nb.name == name && nb.offset == offset; }
		bool operator!=(ثابت NameBit& nb) ثابت { بازگشت !operator==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			اگر (!cell->hasPort(port_name))
				بازگشت {};
			auto &port = cell->getPort(port_name);
			اگر (offset >= port.size())
				بازگشت {};
			بازگشت port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(name);
			h.eat(offset);
			بازگشت h;
		}
	};
	ساختار BitBit
	{
		NameBit first, second;
		BitBit(ثابت NameBit &first, ثابت NameBit &second) : first(first), second(second) {}
		BitBit(ثابت SigBit &first, ثابت SigBit &second) : first(first), second(second) {}
		bool operator==(ثابت BitBit& bb) ثابت { بازگشت bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(first);
			h.eat(second);
			بازگشت h;
		}
	};

	ساختار ModuleTiming
	{
		dict<BitBit, صحیح> comb;
		dict<NameBit, std::pair<صحیح,NameBit>> arrival, required;
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

	تهی setup(RTLIL::Design *design)
	{
		برای (auto module : design->modules()) {
			اگر (!module->get_blackbox_attribute())
				ادامه;
			setup_module(module);
		}
	}

	ثابت ModuleTiming& setup_module(RTLIL::Module *module)
	{
		auto r = data.درج(module->name);
		log_assert(r.second);
		auto &t = r.first->second;

		برای (auto cell : module->cells()) {
			اگر (cell->type == ID($specify2)) {
				auto en = cell->getPort(ID::EN);
				اگر (en.is_fully_const() && !en.as_bool())
					ادامه;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST);
				برای (ثابت auto &c : src.chunks())
					اگر (!c.wire || !c.wire->port_درون_داد)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				برای (ثابت auto &c : dst.chunks())
					اگر (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحیح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحیح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحیح max = std::max(rise_max,fall_max);
				اگر (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				اگر (cell->getParam(ID::FULL).as_bool()) {
					برای (ثابت auto &s : src)
						برای (ثابت auto &d : dst) {
							auto r = t.comb.درج(BitBit(s,d));
							اگر (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				وگرنه {
					log_assert(GetSize(src) == GetSize(dst));
					برای (auto i = 0; i < GetSize(src); i++) {
						ثابت auto &s = src[i];
						ثابت auto &d = dst[i];
						auto r = t.comb.درج(BitBit(s,d));
						اگر (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			وگرنه اگر (cell->type == ID($specify3)) {
				auto src = cell->getPort(ID::SRC).as_bit();
				auto dst = cell->getPort(ID::DST);
				اگر (!src.wire || !src.wire->port_درون_داد)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				برای (ثابت auto &c : dst.chunks())
					اگر (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				صحیح rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				صحیح fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				صحیح max = std::max(rise_max,fall_max);
				اگر (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				برای (ثابت auto &d : dst) {
					auto r = t.arrival.درج(NameBit(d));
					auto &v = r.first->second;
					اگر (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			وگرنه اگر (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				اگر (type != "$setup" && type != "$setuphold")
					ادامه;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST).as_bit();
				برای (ثابت auto &c : src.chunks())
					اگر (!c.wire || !c.wire->port_درون_داد)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				اگر (!dst.wire || !dst.wire->port_درون_داد)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				صحیح max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				اگر (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				برای (ثابت auto &s : src) {
					auto r = t.required.درج(NameBit(s));
					auto &v = r.first->second;
					اگر (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		برای (auto port_name : module->ports) {
			auto wire = module->wire(port_name);
			اگر (wire->port_درون_داد) {
				t.has_inputs = true;
				بشکن;
			}
		}

		بازگشت t;
	}

	decltype(data)::const_iterator find(RTLIL::IdString module_name) ثابت { بازگشت data.find(module_name); }
	decltype(data)::const_iterator end() ثابت { بازگشت data.end(); }
	صحیح شمار(RTLIL::IdString module_name) ثابت { بازگشت data.شمار(module_name); }
	ثابت ModuleTiming& at(RTLIL::IdString module_name) ثابت { بازگشت data.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
