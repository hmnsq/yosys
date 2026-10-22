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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ସଂରଚନା TimingInfo
{
	ସଂରଚନା NameBit
	{
		RTLIL::IdString name;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;
		NameBit() : offset(0) {}
		NameBit(IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset) : name(name), offset(offset) {}
		explicit NameBit(ଅଚଳ RTLIL::SigBit &b) : name(b.wire->name), offset(b.offset) {}
		bool operator==(ଅଚଳ NameBit& nb) ଅଚଳ { ଫେରନ୍ତୁ nb.name == name && nb.offset == offset; }
		bool operator!=(ଅଚଳ NameBit& nb) ଅଚଳ { ଫେରନ୍ତୁ !operator==(nb); }
		std::optional<SigBit> get_connection(RTLIL::Cell *cell) {
			IdString port_name = name;
			ଯଦି (!cell->hasPort(port_name))
				ଫେରନ୍ତୁ {};
			auto &port = cell->getPort(port_name);
			ଯଦି (offset >= port.size())
				ଫେରନ୍ତୁ {};
			ଫେରନ୍ତୁ port[offset];
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
			h.eat(name);
			h.eat(offset);
			ଫେରନ୍ତୁ h;
		}
	};
	ସଂରଚନା BitBit
	{
		NameBit first, second;
		BitBit(ଅଚଳ NameBit &first, ଅଚଳ NameBit &second) : first(first), second(second) {}
		BitBit(ଅଚଳ SigBit &first, ଅଚଳ SigBit &second) : first(first), second(second) {}
		bool operator==(ଅଚଳ BitBit& bb) ଅଚଳ { ଫେରନ୍ତୁ bb.first == first && bb.second == second; }
		[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
			h.eat(first);
			h.eat(second);
			ଫେରନ୍ତୁ h;
		}
	};

	ସଂରଚନା ModuleTiming
	{
		dict<BitBit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା> comb;
		dict<NameBit, std::pair<ପୂର୍ଣ୍ଣସଂଖ୍ୟା,NameBit>> arrival, required;
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

	ଶୂନ୍ୟ setup(RTLIL::Design *design)
	{
		ପାଇଁ (auto module : design->modules()) {
			ଯଦି (!module->get_blackbox_attribute())
				ଜାରି;
			setup_module(module);
		}
	}

	ଅଚଳ ModuleTiming& setup_module(RTLIL::Module *module)
	{
		auto r = data.ଭର୍ତ୍ତି(module->name);
		log_assert(r.second);
		auto &t = r.first->second;

		ପାଇଁ (auto cell : module->cells()) {
			ଯଦି (cell->type == ID($specify2)) {
				auto en = cell->getPort(ID::EN);
				ଯଦି (en.is_fully_const() && !en.as_bool())
					ଜାରି;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST);
				ପାଇଁ (ଅଚଳ auto &c : src.chunks())
					ଯଦି (!c.wire || !c.wire->port_ନିବେଶ)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				ପାଇଁ (ଅଚଳ auto &c : dst.chunks())
					ଯଦି (!c.wire || !c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା max = std::max(rise_max,fall_max);
				ଯଦି (max < 0)
					log_error("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0.\n", module, cell);
				ଯଦି (cell->getParam(ID::FULL).as_bool()) {
					ପାଇଁ (ଅଚଳ auto &s : src)
						ପାଇଁ (ଅଚଳ auto &d : dst) {
							auto r = t.comb.ଭର୍ତ୍ତି(BitBit(s,d));
							ଯଦି (!r.second)
								log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
							r.first->second = max;
						}
				}
				ଅନ୍ୟଥା {
					log_assert(GetSize(src) == GetSize(dst));
					ପାଇଁ (auto i = 0; i < GetSize(src); i++) {
						ଅଚଳ auto &s = src[i];
						ଅଚଳ auto &d = dst[i];
						auto r = t.comb.ଭର୍ତ୍ତି(BitBit(s,d));
						ଯଦି (!r.second)
							log_error("Module '%s' contains multiple specify cells for SRC '%s' and DST '%s'.\n", module, log_signal(s), log_signal(d));
						r.first->second = max;
					}
				}
			}
			ଅନ୍ୟଥା ଯଦି (cell->type == ID($specify3)) {
				auto src = cell->getPort(ID::SRC).as_bit();
				auto dst = cell->getPort(ID::DST);
				ଯଦି (!src.wire || !src.wire->port_ନିବେଶ)
					log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				ପାଇଁ (ଅଚଳ auto &c : dst.chunks())
					ଯଦି (!c.wire->port_output)
						log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module output.\n", module, cell, log_signal(dst));
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା rise_max = cell->getParam(ID::T_RISE_MAX).as_int();
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା fall_max = cell->getParam(ID::T_FALL_MAX).as_int();
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା max = std::max(rise_max,fall_max);
				ଯଦି (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_{RISE,FALL}_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				ପାଇଁ (ଅଚଳ auto &d : dst) {
					auto r = t.arrival.ଭର୍ତ୍ତି(NameBit(d));
					auto &v = r.first->second;
					ଯଦି (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(src);
					}
				}
			}
			ଅନ୍ୟଥା ଯଦି (cell->type == ID($specrule)) {
				std::string type = cell->getParam(ID::TYPE).decode_string();
				ଯଦି (type != "$setup" && type != "$setuphold")
					ଜାରି;
				auto src = cell->getPort(ID::SRC);
				auto dst = cell->getPort(ID::DST).as_bit();
				ପାଇଁ (ଅଚଳ auto &c : src.chunks())
					ଯଦି (!c.wire || !c.wire->port_ନିବେଶ)
						log_error("Module '%s' contains specify cell '%s' where SRC '%s' is not a module input.\n", module, cell, log_signal(src));
				ଯଦି (!dst.wire || !dst.wire->port_ନିବେଶ)
					log_error("Module '%s' contains specify cell '%s' where DST '%s' is not a module input.\n", module, cell, log_signal(dst));
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା max = cell->getParam(ID::T_LIMIT_MAX).as_int();
				ଯଦି (max < 0) {
					log_warning("Module '%s' contains specify cell '%s' with T_LIMIT_MAX < 0 which is currently unsupported. Clamping to 0.\n", module, cell);
					max = 0;
				}
				ପାଇଁ (ଅଚଳ auto &s : src) {
					auto r = t.required.ଭର୍ତ୍ତି(NameBit(s));
					auto &v = r.first->second;
					ଯଦି (r.second || v.first < max) {
						v.first = max;
						v.second = NameBit(dst);
					}
				}
			}
		}

		ପାଇଁ (auto port_name : module->ports) {
			auto wire = module->wire(port_name);
			ଯଦି (wire->port_ନିବେଶ) {
				t.has_inputs = true;
				ଭାଙ୍ଗ;
			}
		}

		ଫେରନ୍ତୁ t;
	}

	decltype(data)::const_iterator find(RTLIL::IdString module_name) ଅଚଳ { ଫେରନ୍ତୁ data.find(module_name); }
	decltype(data)::const_iterator end() ଅଚଳ { ଫେରନ୍ତୁ data.end(); }
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2(RTLIL::IdString module_name) ଅଚଳ { ଫେରନ୍ତୁ data.ଗଣନା_2(module_name); }
	ଅଚଳ ModuleTiming& at(RTLIL::IdString module_name) ଅଚଳ { ଫେରନ୍ତୁ data.at(module_name); }
};

YOSYS_NAMESPACE_END

#endif
