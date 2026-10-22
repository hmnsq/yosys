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

#അന്തര്ഭാവ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ഘടന TimingInfo
{
	ഘടന NameBit
	{
		RTLIL::IdString name;
		പൂർണ്ണസംഖ്യ offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, പൂർണ്ണസംഖ്യ offset) : name(name), offset(offset) {}
		explicit NameBit(സ്ഥിരം RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool operator==(സ്ഥിരം NameBit& nb) സ്ഥിരം { തിരികെ nb.name == name && nb.offset == offset; }
		bool operator!=(സ്ഥിരം NameBit& nb) സ്ഥിരം { തിരികെ !operator==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			എങ്കിൽ (!cell->hasPort(port_name))
				തിരികെ {};
			auto &port = cell->getPort(port_name);
			എങ്കിൽ (offset >= port.size())
				തിരികെ {};
			തിരികെ port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
			h.eat(name);
			h.eat(offset);
			തിരികെ h;
		}
	};
	ഘടന BitBit
	{
		NameBit first, second;
		BitBit(സ്ഥിരം NameBit &first, സ്ഥിരം NameBit &second) : first(first), second(second) {}
		BitBit(സ്ഥിരം SigBit &first, സ്ഥിരം SigBit &second) : first(first), second(second) {}
		bool operator==(സ്ഥിരം BitBit& bb) സ്ഥിരം { തിരികെ bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
			h.eat(first);
			h.eat(second);
			തിരികെ h;
		}
	};

	ഘടന ModuleTiming
	{
		dict<BitBit, പൂർണ്ണസംഖ്യ> comb;
		dict<NameBit, std::pair<പൂർണ്ണസംഖ്യ,NameBit>> arrival, required;
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

	ശൂന്യം setup(RTLIL::Design *design)
	{
		വേണ്ടി (auto module : design->modules()) {
			എങ്കിൽ (!module->get_blackbox_attribute())
				തുടരുക;
			setup_module(module);
		}
	}

	സ്ഥിരം ModuleTiming& setup_module(RTLIL::Module *module)
	{
		auto r = data.തിരുകുക(module->name);
		log_assert(r.second);
		auto &t = r.first->second;

		വേണ്ടി (auto cell : module->cells()) {
			എങ്കിൽ (cell->type == ID($specify2)) {
				auto en = cell->getPort(ID::EN);
				എങ്കിൽ (en.is_fully_const() && !en.as_bool())
					തുടരുക;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST);
				വേണ്ടി (സ്ഥിരം auto &c : src.chunks())
					എങ്കിൽ (!c.wire || !c.wire->port_നിവേശം)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				വേണ്ടി (സ്ഥിരം auto &c : dst.chunks())
					എങ്കിൽ (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				പൂർണ്ണസംഖ്യ rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				പൂർണ്ണസംഖ്യ fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				പൂർണ്ണസംഖ്യ max = std::max(rise_max,fall_max);
				എങ്കിൽ (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				എങ്കിൽ (cell->getParam(ID::FULL).as_bool()) {
					വേണ്ടി (സ്ഥിരം auto &s : src)
						വേണ്ടി (സ്ഥിരം auto &d : dst) {
							auto r = t.comb.തിരുകുക(BitBit(s,d));
							എങ്കിൽ (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				അല്ലെങ്കിൽ {
					log_assert(GetSize(src) == GetSize(dst));
					വേണ്ടി (auto i = 0; i < GetSize(src); i++) {
						സ്ഥിരം auto &s = src[i];
						സ്ഥിരം auto &d = dst[i];
						auto r = t.comb.തിരുകുക(BitBit(s,d));
						എങ്കിൽ (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($specify3)) {
				auto src = cell->getPort(ID::SRC).as_bit();
				auto dst = cell->getPort(ID::DST);
				എങ്കിൽ (!src.wire || !src.wire->port_നിവേശം)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				വേണ്ടി (സ്ഥിരം auto &c : dst.chunks())
					എങ്കിൽ (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				പൂർണ്ണസംഖ്യ rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				പൂർണ്ണസംഖ്യ fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				പൂർണ്ണസംഖ്യ max = std::max(rise_max,fall_max);
				എങ്കിൽ (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				വേണ്ടി (സ്ഥിരം auto &d : dst) {
					auto r = t.arrival.തിരുകുക(NameBit(d));
					auto &v = r.first->second;
					എങ്കിൽ (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				എങ്കിൽ (type != "$setup" && type != "$setuphold")
					തുടരുക;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST).as_bit();
				വേണ്ടി (സ്ഥിരം auto &c : src.chunks())
					എങ്കിൽ (!c.wire || !c.wire->port_നിവേശം)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				എങ്കിൽ (!dst.wire || !dst.wire->port_നിവേശം)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				പൂർണ്ണസംഖ്യ max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				എങ്കിൽ (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				വേണ്ടി (സ്ഥിരം auto &s : src) {
					auto r = t.required.തിരുകുക(NameBit(s));
					auto &v = r.first->second;
					എങ്കിൽ (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		വേണ്ടി (auto port_name : module->ports) {
			auto wire = module->wire(port_name);
			എങ്കിൽ (wire->port_നിവേശം) {
				t.has_inputs = true;
				നിർത്തുക;
			}
		}

		തിരികെ t;
	}

	decltype(data)::const_iterator find(RTLIL::IdString module_name) സ്ഥിരം { തിരികെ data.find(module_name); }
	decltype(data)::const_iterator end() സ്ഥിരം { തിരികെ data.end(); }
	പൂർണ്ണസംഖ്യ എണ്ണം(RTLIL::IdString module_name) സ്ഥിരം { തിരികെ data.എണ്ണം(module_name); }
	സ്ഥിരം ModuleTiming& at(RTLIL::IdString module_name) സ്ഥിരം { തിരികെ data.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
