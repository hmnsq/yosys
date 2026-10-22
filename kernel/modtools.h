/* -*- c++ -*-
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
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

#ifndef MODTOOLS_H
#define MODTOOLS_H

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "kernel/sigtools.h"
#ଅନ୍ତର୍ଭାଵ "kernel/celltypes.h"
#ଅନ୍ତର୍ଭାଵ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ସଂରଚନା ModIndex : ସର୍ବସାଧାରଣ RTLIL::Monitor
{
	ସଂରଚନା PointerOrderedSigBit : ସର୍ବସାଧାରଣ RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			ଯଦି (wire)
				offset = s.offset;
			ଅନ୍ୟଥା
				data = s.data;
		}
		ଧାଡ଼ିଭିତରେ bool operator<(ଅଚଳ RTLIL::SigBit &other) ଅଚଳ {
			ଯଦି (wire == other.wire)
				ଫେରନ୍ତୁ wire ? (offset < other.offset) : (data < other.data);
			ଯଦି (wire != nullptr && other.wire != nullptr)
				ଫେରନ୍ତୁ wire < other.wire; // look here
			ଫେରନ୍ତୁ (wire != nullptr) < (other.wire != nullptr);
		}
	};
	ସଂରଚନା PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(ଅଚଳ PortInfo &other) ଅଚଳ {
			ଯଦି (cell != other.cell)
				ଫେରନ୍ତୁ cell < other.cell;
			ଯଦି (offset != other.offset)
				ଫେରନ୍ତୁ offset < other.offset;
			ଫେରନ୍ତୁ port < other.port;
		}

		bool operator==(ଅଚଳ PortInfo &other) ଅଚଳ {
			ଫେରନ୍ତୁ cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			ଫେରନ୍ତୁ h;
		}
	};

	ସଂରଚନା SigBitInfo
	{
		bool is_ନିବେଶ, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_ନିବେଶ(false), is_output(false) { }

		bool operator==(ଅଚଳ SigBitInfo &other) ଅଚଳ {
			ଫେରନ୍ତୁ is_ନିବେଶ == other.is_ନିବେଶ && is_output == other.is_output && ports == other.ports;
		}

		ଶୂନ୍ୟ merge(ଅଚଳ SigBitInfo &other)
		{
			is_ନିବେଶ = is_ନିବେଶ || other.is_ନିବେଶ;
			is_output = is_output || other.is_output;
			ports.ଭର୍ତ୍ତି(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା auto_reload_counter;
	bool auto_reload_module;

	ଶୂନ୍ୟ port_add(RTLIL::Cell *cell, RTLIL::IdString port, ଅଚଳ RTLIL::SigSpec &sig)
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			ଯଦି (bit.wire)
				database[bit].ports.ଭର୍ତ୍ତି(PortInfo(cell, port, i));
		}
	}

	ଶୂନ୍ୟ port_del(RTLIL::Cell *cell, RTLIL::IdString port, ଅଚଳ RTLIL::SigSpec &sig)
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			ଯଦି (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				ଯଦି (!database[bit].is_ନିବେଶ && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	ଅଚଳ SigBitInfo &info(RTLIL::SigBit bit)
	{
		ଫେରନ୍ତୁ database[sigmap(bit)];
	}

	ଶୂନ୍ୟ reload_module(bool reset_sigmap = true)
	{
		ଯଦି (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		ପାଇଁ (auto wire : module->wires())
			ଯଦି (wire->port_ନିବେଶ || wire->port_output)
				ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					ଯଦି (bit.wire && wire->port_ନିବେଶ)
						database[bit].is_ନିବେଶ = true;
					ଯଦି (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		ପାଇଁ (auto cell : module->cells())
			ପାଇଁ (auto &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		ଯଦି (auto_reload_module) {
			ଯଦି (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef NDEBUG
		ଯଦି (auto_reload_module)
			ଫେରନ୍ତୁ true;

		ପାଇଁ (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		ଯଦି (!(database == database_bak))
		{
			ପାଇଁ (auto &it : database_bak)
				ଯଦି (!database.ଗଣନା_2(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			ପାଇଁ (auto &it : database)
				ଯଦି (!database_bak.ଗଣନା_2(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				ଅନ୍ୟଥା ଯଦି (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			ଫେରନ୍ତୁ false;
		}
#endif
		ଫେରନ୍ତୁ true;
	}

	ଶୂନ୍ୟ check()
	{
		log_assert(ok());
	}

	ଶୂନ୍ୟ notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, ଅଚଳ RTLIL::SigSpec &old_sig, ଅଚଳ RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		ଯଦି (auto_reload_module)
			ଫେରନ୍ତୁ;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	ଶୂନ୍ୟ notify_connect(RTLIL::Module *mod, ଅଚଳ RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		ଯଦି (auto_reload_module)
			ଫେରନ୍ତୁ;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.ଗଣନା_2(lhs) != 0;
			bool has_rhs = database.ଗଣନା_2(rhs) != 0;

			ଯଦି (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} ଅନ୍ୟଥା
			ଯଦି (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				ଯଦି (lhs.wire)
					database[lhs] = new_info;
			} ଅନ୍ୟଥା
			ଯଦି (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				ଯଦି (rhs.wire)
					database[rhs] = new_info;
			} ଅନ୍ୟଥା {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				ଯଦି (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	ଶୂନ୍ୟ notify_connect(RTLIL::Module *mod, ଅଚଳ std::vector<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ଶୂନ୍ୟ notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ModIndex(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.ଭର୍ତ୍ତି(this);
	}

	~ModIndex()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		ଯଦି (auto_reload_module)
			reload_module();

		auto it = database.find(sigmap(bit));
		ଯଦି (it == database.end())
			ଫେରନ୍ତୁ nullptr;
		ଅନ୍ୟଥା
			ଫେରନ୍ତୁ &it->second;
	}

	bool query_is_ନିବେଶ(RTLIL::SigBit bit)
	{
		ଅଚଳ SigBitInfo *info = query(bit);
		ଯଦି (info == nullptr)
			ଫେରନ୍ତୁ false;
		ଫେରନ୍ତୁ info->is_ନିବେଶ;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		ଅଚଳ SigBitInfo *info = query(bit);
		ଯଦି (info == nullptr)
			ଫେରନ୍ତୁ false;
		ଫେରନ୍ତୁ info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		ସ୍ଥିର pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		ଯଦି (info == nullptr)
			ଫେରନ୍ତୁ empty_result_set;
		ଫେରନ୍ତୁ info->ports;
	}

	ଶୂନ୍ୟ dump_db()
	{
		log("--- ModIndex Dump ---\n");

		ଯଦି (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		ପାଇଁ (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			ଯଦି (it.second.is_ନିବେଶ)
				log("  PRIMARY INPUT\n");
			ଯଦି (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			ପାଇଁ (auto &port : it.second.ports)
				log("  PORT: %s.%s[%d] (%s)\n", port.cell,
						PooledName(module, port.port).unescape(), port.offset, port.cell->type.unescape());
		}
	}
};

ସଂରଚନା ModWalker
{
	ସଂରଚନା PortBit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString port;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;
		PortBit(Cell* c, IdString p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା o) : cell(c), port(p), offset(o) {}

		bool operator<(ଅଚଳ PortBit &other) ଅଚଳ {
			ଯଦି (cell != other.cell)
				ଫେରନ୍ତୁ cell < other.cell;
			ଯଦି (port != other.port)
				ଫେରନ୍ତୁ port < other.port;
			ଫେରନ୍ତୁ offset < other.offset;
		}

		bool operator==(ଅଚଳ PortBit &other) ଅଚଳ {
			ଫେରନ୍ତୁ cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			ଫେରନ୍ତୁ h;
		}
	};

	RTLIL::Design *design;
	RTLIL::Module *module;

	NewCellTypes ct;
	SigMap sigmap;

	dict<RTLIL::SigBit, pool<PortBit>> signal_drivers;
	dict<RTLIL::SigBit, pool<PortBit>> signal_consumers;
	pool<RTLIL::SigBit> signal_inputs, signal_outputs;

	dict<RTLIL::Cell*, pool<RTLIL::SigBit>> cell_outputs, cell_inputs;

	ଶୂନ୍ୟ add_wire(RTLIL::Wire *wire)
	{
		ଯଦି (wire->port_ନିବେଶ) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			ପାଇଁ (auto bit : bits)
				ଯଦି (bit.wire != ଖାଲି)
					signal_inputs.ଭର୍ତ୍ତି(bit);
		}

		ଯଦି (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			ପାଇଁ (auto bit : bits)
				ଯଦି (bit.wire != ଖାଲି)
					signal_outputs.ଭର୍ତ୍ତି(bit);
		}
	}

	ଶୂନ୍ୟ add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_ନିବେଶ)
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ପୂର୍ଣ୍ଣସଂଖ୍ୟା(bits.size()); i++)
			ଯଦି (bits[i].wire != ଖାଲି) {
				PortBit pbit {cell, port, i};
				ଯଦି (is_output) {
					signal_drivers[bits[i]].ଭର୍ତ୍ତି(pbit);
					cell_outputs[cell].ଭର୍ତ୍ତି(bits[i]);
				}
				ଯଦି (is_ନିବେଶ) {
					signal_consumers[bits[i]].ଭର୍ତ୍ତି(pbit);
					cell_inputs[cell].ଭର୍ତ୍ତି(bits[i]);
				}
			}
	}

	ଶୂନ୍ୟ add_cell(RTLIL::Cell *cell)
	{
		ଯଦି (ct.cell_known(cell->type)) {
			ପାଇଁ (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_ନିବେଶ(cell->type, conn.first));
		} ଅନ୍ୟଥା {
			ପାଇଁ (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(ଖାଲି)
	{
		ct.setup(design);
		ଯଦି (module)
			setup(module);
	}

	ଶୂନ୍ୟ setup(RTLIL::Module *module, CellTypes *filter_ct = ଖାଲି)
	{
		this->module = module;

		sigmap.set(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		ପାଇଁ (auto &it : module->wires_)
			add_wire(it.second);
		ପାଇଁ (auto &it : module->cells_)
			ଯଦି (filter_ct == ଖାଲି || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	ଧାଡ଼ିଭିତରେ bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) ଅଚଳ
	{
		bool found = false;
		ଯଦି (signal_drivers.ଗଣନା_2(bit)) {
			ଅଚଳ pool<PortBit> &r = signal_drivers.at(bit);
			result.ଭର୍ତ୍ତି(r.begin(), r.end());
			found = true;
		}
		ଫେରନ୍ତୁ found;
	}

	ଧାଡ଼ିଭିତରେ bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) ଅଚଳ
	{
		bool found = false;
		ଯଦି (signal_consumers.ଗଣନା_2(bit)) {
			ଅଚଳ pool<PortBit> &r = signal_consumers.at(bit);
			result.ଭର୍ତ୍ତି(r.begin(), r.end());
			found = true;
		}
		ଫେରନ୍ତୁ found;
	}

	ଧାଡ଼ିଭିତରେ bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ଅଚଳ
	{
		bool found = false;
		ଯଦି (signal_inputs.ଗଣନା_2(bit))
			result.ଭର୍ତ୍ତି(bit), found = true;
		ଫେରନ୍ତୁ found;
	}

	ଧାଡ଼ିଭିତରେ bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ଅଚଳ
	{
		bool found = false;
		ଯଦି (signal_outputs.ଗଣନା_2(bit))
			result.ଭର୍ତ୍ତି(bit), found = true;
		ଫେରନ୍ତୁ found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	ଧାଡ଼ିଭିତରେ bool get_drivers(pool<PortBit> &result, ଅଚଳ T &bits) ଅଚଳ
	{
		bool found = false;
		ପାଇଁ (RTLIL::SigBit bit : bits)
			ଯଦି (signal_drivers.ଗଣନା_2(bit)) {
				ଅଚଳ pool<PortBit> &r = signal_drivers.at(bit);
				result.ଭର୍ତ୍ତି(r.begin(), r.end());
				found = true;
			}
		ଫେରନ୍ତୁ found;
	}

	template<typename T>
	ଧାଡ଼ିଭିତରେ bool get_consumers(pool<PortBit> &result, ଅଚଳ T &bits) ଅଚଳ
	{
		bool found = false;
		ପାଇଁ (RTLIL::SigBit bit : bits)
			ଯଦି (signal_consumers.ଗଣନା_2(bit)) {
				ଅଚଳ pool<PortBit> &r = signal_consumers.at(bit);
				result.ଭର୍ତ୍ତି(r.begin(), r.end());
				found = true;
			}
		ଫେରନ୍ତୁ found;
	}

	template<typename T>
	ଧାଡ଼ିଭିତରେ bool get_inputs(pool<RTLIL::SigBit> &result, ଅଚଳ T &bits) ଅଚଳ
	{
		bool found = false;
		ପାଇଁ (RTLIL::SigBit bit : bits)
			ଯଦି (signal_inputs.ଗଣନା_2(bit))
				result.ଭର୍ତ୍ତି(bit), found = true;
		ଫେରନ୍ତୁ found;
	}

	template<typename T>
	ଧାଡ଼ିଭିତରେ bool get_outputs(pool<RTLIL::SigBit> &result, ଅଚଳ T &bits) ଅଚଳ
	{
		bool found = false;
		ପାଇଁ (RTLIL::SigBit bit : bits)
			ଯଦି (signal_outputs.ଗଣନା_2(bit))
				result.ଭର୍ତ୍ତି(bit), found = true;
		ଫେରନ୍ତୁ found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec ସଙ୍କେତ) ଅଚଳ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(ସଙ୍କେତ);
		ଫେରନ୍ତୁ get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec ସଙ୍କେତ) ଅଚଳ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(ସଙ୍କେତ);
		ଫେରନ୍ତୁ get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec ସଙ୍କେତ) ଅଚଳ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(ସଙ୍କେତ);
		ଫେରନ୍ତୁ get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec ସଙ୍କେତ) ଅଚଳ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(ସଙ୍କେତ);
		ଫେରନ୍ତୁ get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	ଧାଡ଼ିଭିତରେ bool has_drivers(ଅଚଳ T &sig) ଅଚଳ {
		pool<PortBit> result;
		ଫେରନ୍ତୁ get_drivers(result, sig);
	}

	template<typename T>
	ଧାଡ଼ିଭିତରେ bool has_consumers(ଅଚଳ T &sig) ଅଚଳ {
		pool<PortBit> result;
		ଫେରନ୍ତୁ get_consumers(result, sig);
	}

	template<typename T>
	ଧାଡ଼ିଭିତରେ bool has_inputs(ଅଚଳ T &sig) ଅଚଳ {
		pool<RTLIL::SigBit> result;
		ଫେରନ୍ତୁ get_inputs(result, sig);
	}

	template<typename T>
	ଧାଡ଼ିଭିତରେ bool has_outputs(ଅଚଳ T &sig) ଅଚଳ {
		pool<RTLIL::SigBit> result;
		ଫେରନ୍ତୁ get_outputs(result, sig);
	}

	// has_* methods -- call by value

	ଧାଡ଼ିଭିତରେ bool has_drivers(RTLIL::SigSpec sig) ଅଚଳ {
		pool<PortBit> result;
		ଫେରନ୍ତୁ get_drivers(result, sig);
	}

	ଧାଡ଼ିଭିତରେ bool has_consumers(RTLIL::SigSpec sig) ଅଚଳ {
		pool<PortBit> result;
		ଫେରନ୍ତୁ get_consumers(result, sig);
	}

	ଧାଡ଼ିଭିତରେ bool has_inputs(RTLIL::SigSpec sig) ଅଚଳ {
		pool<RTLIL::SigBit> result;
		ଫେରନ୍ତୁ get_inputs(result, sig);
	}

	ଧାଡ଼ିଭିତରେ bool has_outputs(RTLIL::SigSpec sig) ଅଚଳ {
		pool<RTLIL::SigBit> result;
		ଫେରନ୍ତୁ get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
