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

#அந்தர்பாவ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

கட்டமைப்பு TimingInfo
{
	கட்டமைப்பு NameBit
	{
		RTLIL::IdString name;
		முழு offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, முழு offset) : name(name), offset(offset) {}
		explicit NameBit(மாறிலி RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool operator==(மாறிலி NameBit& nb) மாறிலி { திரும்பு nb.name == name && nb.offset == offset; }
		bool operator!=(மாறிலி NameBit& nb) மாறிலி { திரும்பு !operator==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			எனில் (!cell->hasPort(port_name))
				திரும்பு {};
			auto &port = cell->getPort(port_name);
			எனில் (offset >= port.size())
				திரும்பு {};
			திரும்பு port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
			h.eat(name);
			h.eat(offset);
			திரும்பு h;
		}
	};
	கட்டமைப்பு BitBit
	{
		NameBit first, second;
		BitBit(மாறிலி NameBit &first, மாறிலி NameBit &second) : first(first), second(second) {}
		BitBit(மாறிலி SigBit &first, மாறிலி SigBit &second) : first(first), second(second) {}
		bool operator==(மாறிலி BitBit& bb) மாறிலி { திரும்பு bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
			h.eat(first);
			h.eat(second);
			திரும்பு h;
		}
	};

	கட்டமைப்பு ModuleTiming
	{
		dict<BitBit, முழு> comb;
		dict<NameBit, std::pair<முழு,NameBit>> arrival, required;
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

	வெறுமை setup(RTLIL::Design *design)
	{
		க்கு (auto module : design->modules()) {
			எனில் (!module->get_blackbox_attribute())
				தொடர்;
			setup_module(module);
		}
	}

	மாறிலி ModuleTiming& setup_module(RTLIL::Module *module)
	{
		auto r = data.செருகு(module->name);
		log_assert(r.second);
		auto &t = r.first->second;

		க்கு (auto cell : module->cells()) {
			எனில் (cell->type == ID($specify2)) {
				auto en = cell->getPort(ID::EN);
				எனில் (en.is_fully_const() && !en.as_bool())
					தொடர்;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST);
				க்கு (மாறிலி auto &c : src.chunks())
					எனில் (!c.wire || !c.wire->port_உள்ளீடு)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				க்கு (மாறிலி auto &c : dst.chunks())
					எனில் (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				முழு rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				முழு fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				முழு max = std::max(rise_max,fall_max);
				எனில் (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				எனில் (cell->getParam(ID::FULL).as_bool()) {
					க்கு (மாறிலி auto &s : src)
						க்கு (மாறிலி auto &d : dst) {
							auto r = t.comb.செருகு(BitBit(s,d));
							எனில் (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				இல்லையெனில் {
					log_assert(GetSize(src) == GetSize(dst));
					க்கு (auto i = 0; i < GetSize(src); i++) {
						மாறிலி auto &s = src[i];
						மாறிலி auto &d = dst[i];
						auto r = t.comb.செருகு(BitBit(s,d));
						எனில் (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			இல்லையெனில் எனில் (cell->type == ID($specify3)) {
				auto src = cell->getPort(ID::SRC).as_bit();
				auto dst = cell->getPort(ID::DST);
				எனில் (!src.wire || !src.wire->port_உள்ளீடு)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				க்கு (மாறிலி auto &c : dst.chunks())
					எனில் (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				முழு rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				முழு fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				முழு max = std::max(rise_max,fall_max);
				எனில் (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				க்கு (மாறிலி auto &d : dst) {
					auto r = t.arrival.செருகு(NameBit(d));
					auto &v = r.first->second;
					எனில் (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			இல்லையெனில் எனில் (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				எனில் (type != "$setup" && type != "$setuphold")
					தொடர்;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST).as_bit();
				க்கு (மாறிலி auto &c : src.chunks())
					எனில் (!c.wire || !c.wire->port_உள்ளீடு)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				எனில் (!dst.wire || !dst.wire->port_உள்ளீடு)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				முழு max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				எனில் (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				க்கு (மாறிலி auto &s : src) {
					auto r = t.required.செருகு(NameBit(s));
					auto &v = r.first->second;
					எனில் (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		க்கு (auto port_name : module->ports) {
			auto wire = module->wire(port_name);
			எனில் (wire->port_உள்ளீடு) {
				t.has_inputs = true;
				நிறுத்து;
			}
		}

		திரும்பு t;
	}

	decltype(data)::const_iterator find(RTLIL::IdString module_name) மாறிலி { திரும்பு data.find(module_name); }
	decltype(data)::const_iterator end() மாறிலி { திரும்பு data.end(); }
	முழு மொத்தம்(RTLIL::IdString module_name) மாறிலி { திரும்பு data.மொத்தம்(module_name); }
	மாறிலி ModuleTiming& at(RTLIL::IdString module_name) மாறிலி { திரும்பு data.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
