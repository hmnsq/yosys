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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ಸಂರಚನಾ TimingInfo
{
	ಸಂರಚನಾ NameBit
	{
		RTLIL::IdString name;
		ಪೂರ್ಣಾಙ್ಕ offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, ಪೂರ್ಣಾಙ್ಕ offset) : name(name), offset(offset) {}
		explicit NameBit(ಧ್ರುವ RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool operator==(ಧ್ರುವ NameBit& nb) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ nb.name == name && nb.offset == offset; }
		bool operator!=(ಧ್ರುವ NameBit& nb) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ !operator==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			ಯದಿ (!cell->hasPort(port_name))
				ಪ್ರತ್ಯಾಗಮ {};
			auto &port = cell->getPort(port_name);
			ಯದಿ (offset >= port.size())
				ಪ್ರತ್ಯಾಗಮ {};
			ಪ್ರತ್ಯಾಗಮ port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
			h.eat(name);
			h.eat(offset);
			ಪ್ರತ್ಯಾಗಮ h;
		}
	};
	ಸಂರಚನಾ BitBit
	{
		NameBit first, second;
		BitBit(ಧ್ರುವ NameBit &first, ಧ್ರುವ NameBit &second) : first(first), second(second) {}
		BitBit(ಧ್ರುವ SigBit &first, ಧ್ರುವ SigBit &second) : first(first), second(second) {}
		bool operator==(ಧ್ರುವ BitBit& bb) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
			h.eat(first);
			h.eat(second);
			ಪ್ರತ್ಯಾಗಮ h;
		}
	};

	ಸಂರಚನಾ ModuleTiming
	{
		dict<BitBit, ಪೂರ್ಣಾಙ್ಕ> comb;
		dict<NameBit, std::pair<ಪೂರ್ಣಾಙ್ಕ,NameBit>> arrival, required;
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

	ಶೂನ್ಯ setup(RTLIL::Design *design)
	{
		ಪ್ರತಿ (auto module : design->modules()) {
			ಯದಿ (!module->get_blackbox_attribute())
				ಅನುವರ್ತ;
			setup_module(module);
		}
	}

	ಧ್ರುವ ModuleTiming& setup_module(RTLIL::Module *module)
	{
		auto r = data.ಸೇರಿಸು(module->name);
		log_assert(r.second);
		auto &t = r.first->second;

		ಪ್ರತಿ (auto cell : module->cells()) {
			ಯದಿ (cell->type == ID($specify2)) {
				auto en = cell->getPort(ID::EN);
				ಯದಿ (en.is_fully_const() && !en.as_bool())
					ಅನುವರ್ತ;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST);
				ಪ್ರತಿ (ಧ್ರುವ auto &c : src.chunks())
					ಯದಿ (!c.wire || !c.wire->port_ಒಳಹರಿವು)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				ಪ್ರತಿ (ಧ್ರುವ auto &c : dst.chunks())
					ಯದಿ (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				ಪೂರ್ಣಾಙ್ಕ rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				ಪೂರ್ಣಾಙ್ಕ fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				ಪೂರ್ಣಾಙ್ಕ max = std::max(rise_max,fall_max);
				ಯದಿ (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				ಯದಿ (cell->getParam(ID::FULL).as_bool()) {
					ಪ್ರತಿ (ಧ್ರುವ auto &s : src)
						ಪ್ರತಿ (ಧ್ರುವ auto &d : dst) {
							auto r = t.comb.ಸೇರಿಸು(BitBit(s,d));
							ಯದಿ (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				ಅನ್ಯಥಾ {
					log_assert(GetSize(src) == GetSize(dst));
					ಪ್ರತಿ (auto i = 0; i < GetSize(src); i++) {
						ಧ್ರುವ auto &s = src[i];
						ಧ್ರುವ auto &d = dst[i];
						auto r = t.comb.ಸೇರಿಸು(BitBit(s,d));
						ಯದಿ (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			ಅನ್ಯಥಾ ಯದಿ (cell->type == ID($specify3)) {
				auto src = cell->getPort(ID::SRC).as_bit();
				auto dst = cell->getPort(ID::DST);
				ಯದಿ (!src.wire || !src.wire->port_ಒಳಹರಿವು)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				ಪ್ರತಿ (ಧ್ರುವ auto &c : dst.chunks())
					ಯದಿ (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				ಪೂರ್ಣಾಙ್ಕ rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				ಪೂರ್ಣಾಙ್ಕ fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				ಪೂರ್ಣಾಙ್ಕ max = std::max(rise_max,fall_max);
				ಯದಿ (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				ಪ್ರತಿ (ಧ್ರುವ auto &d : dst) {
					auto r = t.arrival.ಸೇರಿಸು(NameBit(d));
					auto &v = r.first->second;
					ಯದಿ (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			ಅನ್ಯಥಾ ಯದಿ (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				ಯದಿ (type != "$setup" && type != "$setuphold")
					ಅನುವರ್ತ;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST).as_bit();
				ಪ್ರತಿ (ಧ್ರುವ auto &c : src.chunks())
					ಯದಿ (!c.wire || !c.wire->port_ಒಳಹರಿವು)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				ಯದಿ (!dst.wire || !dst.wire->port_ಒಳಹರಿವು)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				ಪೂರ್ಣಾಙ್ಕ max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				ಯದಿ (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				ಪ್ರತಿ (ಧ್ರುವ auto &s : src) {
					auto r = t.required.ಸೇರಿಸು(NameBit(s));
					auto &v = r.first->second;
					ಯದಿ (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		ಪ್ರತಿ (auto port_name : module->ports) {
			auto wire = module->wire(port_name);
			ಯದಿ (wire->port_ಒಳಹರಿವು) {
				t.has_inputs = true;
				ಭಙ್ಗ;
			}
		}

		ಪ್ರತ್ಯಾಗಮ t;
	}

	decltype(data)::const_iterator find(RTLIL::IdString module_name) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ data.find(module_name); }
	decltype(data)::const_iterator end() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ data.end(); }
	ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ(RTLIL::IdString module_name) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ data.ಎಣಿಕೆ(module_name); }
	ಧ್ರುವ ModuleTiming& at(RTLIL::IdString module_name) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ data.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
