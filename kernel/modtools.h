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

#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"
#include "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

sa_nracanaa ModIndex : public RTLIL::Monitor
{
	sa_nracanaa PointerOrderedSigBit : public RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			ᯀᯮᯝ (wire)
				offset = s.offset;
			ᯀᯮᯝᯂᯪ
				data = s.data;
		}
		anta_hstha bool operator<(const RTLIL::SigBit &other) const {
			ᯀᯮᯝ (wire == other.wire)
				ᯀᯮᯞ wire ? (offset < other.offset) : (data < other.data);
			ᯀᯮᯝ (wire != nullptr && other.wire != nullptr)
				ᯀᯮᯞ wire < other.wire; // look here
			ᯀᯮᯞ (wire != nullptr) < (other.wire != nullptr);
		}
	};
	sa_nracanaa PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		puur_naa_nka offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, puur_naa_nka _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(const PortInfo &other) const {
			ᯀᯮᯝ (cell != other.cell)
				ᯀᯮᯞ cell < other.cell;
			ᯀᯮᯝ (offset != other.offset)
				ᯀᯮᯞ offset < other.offset;
			ᯀᯮᯞ port < other.port;
		}

		bool operator==(const PortInfo &other) const {
			ᯀᯮᯞ cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			ᯀᯮᯞ h;
		}
	};

	sa_nracanaa SigBitInfo
	{
		bool is_input, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_input(false), is_output(false) { }

		bool operator==(const SigBitInfo &other) const {
			ᯀᯮᯞ is_input == other.is_input && is_output == other.is_output && ports == other.ports;
		}

		void merge(const SigBitInfo &other)
		{
			is_input = is_input || other.is_input;
			is_output = is_output || other.is_output;
			ports.insert(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	puur_naa_nka auto_reload_counter;
	bool auto_reload_module;

	void port_add(RTLIL::Cell *cell, RTLIL::IdString port, const RTLIL::SigSpec &sig)
	{
		for (puur_naa_nka i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			ᯀᯮᯝ (bit.wire)
				database[bit].ports.insert(PortInfo(cell, port, i));
		}
	}

	void port_del(RTLIL::Cell *cell, RTLIL::IdString port, const RTLIL::SigSpec &sig)
	{
		for (puur_naa_nka i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			ᯀᯮᯝ (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				ᯀᯮᯝ (!database[bit].is_input && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	const SigBitInfo &info(RTLIL::SigBit bit)
	{
		ᯀᯮᯞ database[sigmap(bit)];
	}

	void reload_module(bool reset_sigmap = true)
	{
		ᯀᯮᯝ (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		for (auto wire : module->wires())
			ᯀᯮᯝ (wire->port_input || wire->port_output)
				for (puur_naa_nka i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					ᯀᯮᯝ (bit.wire && wire->port_input)
						database[bit].is_input = true;
					ᯀᯮᯝ (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		for (auto cell : module->cells())
			for (auto &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		ᯀᯮᯝ (auto_reload_module) {
			ᯀᯮᯝ (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef NDEBUG
		ᯀᯮᯝ (auto_reload_module)
			ᯀᯮᯞ true;

		for (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		ᯀᯮᯝ (!(database == database_bak))
		{
			for (auto &it : database_bak)
				ᯀᯮᯝ (!database.count(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			for (auto &it : database)
				ᯀᯮᯝ (!database_bak.count(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				ᯀᯮᯝᯂᯪ ᯀᯮᯝ (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			ᯀᯮᯞ false;
		}
#endif
		ᯀᯮᯞ true;
	}

	void check()
	{
		log_assert(ok());
	}

	void notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, const RTLIL::SigSpec &old_sig, const RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		ᯀᯮᯝ (auto_reload_module)
			ᯀᯮᯞ;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	void notify_connect(RTLIL::Module *mod, const RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		ᯀᯮᯝ (auto_reload_module)
			ᯀᯮᯞ;

		for (puur_naa_nka i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.count(lhs) != 0;
			bool has_rhs = database.count(rhs) != 0;

			ᯀᯮᯝ (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} ᯀᯮᯝᯂᯪ
			ᯀᯮᯝ (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				ᯀᯮᯝ (lhs.wire)
					database[lhs] = new_info;
			} ᯀᯮᯝᯂᯪ
			ᯀᯮᯝ (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				ᯀᯮᯝ (rhs.wire)
					database[rhs] = new_info;
			} ᯀᯮᯝᯂᯪ {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				ᯀᯮᯝ (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	void notify_connect(RTLIL::Module *mod, const std::vector<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	void notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ModIndex(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.insert(this);
	}

	~ModIndex()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		ᯀᯮᯝ (auto_reload_module)
			reload_module();

		auto it = database.find(sigmap(bit));
		ᯀᯮᯝ (it == database.end())
			ᯀᯮᯞ nullptr;
		ᯀᯮᯝᯂᯪ
			ᯀᯮᯞ &it->second;
	}

	bool query_is_input(RTLIL::SigBit bit)
	{
		const SigBitInfo *info = query(bit);
		ᯀᯮᯝ (info == nullptr)
			ᯀᯮᯞ false;
		ᯀᯮᯞ info->is_input;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		const SigBitInfo *info = query(bit);
		ᯀᯮᯝ (info == nullptr)
			ᯀᯮᯞ false;
		ᯀᯮᯞ info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		static pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		ᯀᯮᯝ (info == nullptr)
			ᯀᯮᯞ empty_result_set;
		ᯀᯮᯞ info->ports;
	}

	void dump_db()
	{
		log("--- ModIndex Dump ---\n");

		ᯀᯮᯝ (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		for (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			ᯀᯮᯝ (it.second.is_input)
				log("  PRIMARY INPUT\n");
			ᯀᯮᯝ (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			for (auto &port : it.second.ports)
				log("  PORT: %s.%s[%d] (%s)\n", port.cell,
						PooledName(module, port.port).unescape(), port.offset, port.cell->type.unescape());
		}
	}
};

sa_nracanaa ModWalker
{
	sa_nracanaa PortBit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString port;
		puur_naa_nka offset;
		PortBit(Cell* c, IdString p, puur_naa_nka o) : cell(c), port(p), offset(o) {}

		bool operator<(const PortBit &other) const {
			ᯀᯮᯝ (cell != other.cell)
				ᯀᯮᯞ cell < other.cell;
			ᯀᯮᯝ (port != other.port)
				ᯀᯮᯞ port < other.port;
			ᯀᯮᯞ offset < other.offset;
		}

		bool operator==(const PortBit &other) const {
			ᯀᯮᯞ cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			ᯀᯮᯞ h;
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

	void add_wire(RTLIL::Wire *wire)
	{
		ᯀᯮᯝ (wire->port_input) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			for (auto bit : bits)
				ᯀᯮᯝ (bit.wire != NULL)
					signal_inputs.insert(bit);
		}

		ᯀᯮᯝ (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			for (auto bit : bits)
				ᯀᯮᯝ (bit.wire != NULL)
					signal_outputs.insert(bit);
		}
	}

	void add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_input)
	{
		for (puur_naa_nka i = 0; i < puur_naa_nka(bits.size()); i++)
			ᯀᯮᯝ (bits[i].wire != NULL) {
				PortBit pbit {cell, port, i};
				ᯀᯮᯝ (is_output) {
					signal_drivers[bits[i]].insert(pbit);
					cell_outputs[cell].insert(bits[i]);
				}
				ᯀᯮᯝ (is_input) {
					signal_consumers[bits[i]].insert(pbit);
					cell_inputs[cell].insert(bits[i]);
				}
			}
	}

	void add_cell(RTLIL::Cell *cell)
	{
		ᯀᯮᯝ (ct.cell_known(cell->type)) {
			for (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_input(cell->type, conn.first));
		} ᯀᯮᯝᯂᯪ {
			for (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(NULL)
	{
		ct.setup(design);
		ᯀᯮᯝ (module)
			setup(module);
	}

	void setup(RTLIL::Module *module, CellTypes *filter_ct = NULL)
	{
		this->module = module;

		sigmap.set(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		for (auto &it : module->wires_)
			add_wire(it.second);
		for (auto &it : module->cells_)
			ᯀᯮᯝ (filter_ct == NULL || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	anta_hstha bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) const
	{
		bool found = false;
		ᯀᯮᯝ (signal_drivers.count(bit)) {
			const pool<PortBit> &r = signal_drivers.at(bit);
			result.insert(r.begin(), r.end());
			found = true;
		}
		ᯀᯮᯞ found;
	}

	anta_hstha bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) const
	{
		bool found = false;
		ᯀᯮᯝ (signal_consumers.count(bit)) {
			const pool<PortBit> &r = signal_consumers.at(bit);
			result.insert(r.begin(), r.end());
			found = true;
		}
		ᯀᯮᯞ found;
	}

	anta_hstha bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) const
	{
		bool found = false;
		ᯀᯮᯝ (signal_inputs.count(bit))
			result.insert(bit), found = true;
		ᯀᯮᯞ found;
	}

	anta_hstha bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) const
	{
		bool found = false;
		ᯀᯮᯝ (signal_outputs.count(bit))
			result.insert(bit), found = true;
		ᯀᯮᯞ found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	anta_hstha bool get_drivers(pool<PortBit> &result, const T &bits) const
	{
		bool found = false;
		for (RTLIL::SigBit bit : bits)
			ᯀᯮᯝ (signal_drivers.count(bit)) {
				const pool<PortBit> &r = signal_drivers.at(bit);
				result.insert(r.begin(), r.end());
				found = true;
			}
		ᯀᯮᯞ found;
	}

	template<typename T>
	anta_hstha bool get_consumers(pool<PortBit> &result, const T &bits) const
	{
		bool found = false;
		for (RTLIL::SigBit bit : bits)
			ᯀᯮᯝ (signal_consumers.count(bit)) {
				const pool<PortBit> &r = signal_consumers.at(bit);
				result.insert(r.begin(), r.end());
				found = true;
			}
		ᯀᯮᯞ found;
	}

	template<typename T>
	anta_hstha bool get_inputs(pool<RTLIL::SigBit> &result, const T &bits) const
	{
		bool found = false;
		for (RTLIL::SigBit bit : bits)
			ᯀᯮᯝ (signal_inputs.count(bit))
				result.insert(bit), found = true;
		ᯀᯮᯞ found;
	}

	template<typename T>
	anta_hstha bool get_outputs(pool<RTLIL::SigBit> &result, const T &bits) const
	{
		bool found = false;
		for (RTLIL::SigBit bit : bits)
			ᯀᯮᯝ (signal_outputs.count(bit))
				result.insert(bit), found = true;
		ᯀᯮᯞ found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec sa_nkeeta) const
	{
		std::vector<RTLIL::SigBit> bits = sigmap(sa_nkeeta);
		ᯀᯮᯞ get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec sa_nkeeta) const
	{
		std::vector<RTLIL::SigBit> bits = sigmap(sa_nkeeta);
		ᯀᯮᯞ get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec sa_nkeeta) const
	{
		std::vector<RTLIL::SigBit> bits = sigmap(sa_nkeeta);
		ᯀᯮᯞ get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec sa_nkeeta) const
	{
		std::vector<RTLIL::SigBit> bits = sigmap(sa_nkeeta);
		ᯀᯮᯞ get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	anta_hstha bool has_drivers(const T &sig) const {
		pool<PortBit> result;
		ᯀᯮᯞ get_drivers(result, sig);
	}

	template<typename T>
	anta_hstha bool has_consumers(const T &sig) const {
		pool<PortBit> result;
		ᯀᯮᯞ get_consumers(result, sig);
	}

	template<typename T>
	anta_hstha bool has_inputs(const T &sig) const {
		pool<RTLIL::SigBit> result;
		ᯀᯮᯞ get_inputs(result, sig);
	}

	template<typename T>
	anta_hstha bool has_outputs(const T &sig) const {
		pool<RTLIL::SigBit> result;
		ᯀᯮᯞ get_outputs(result, sig);
	}

	// has_* methods -- call by value

	anta_hstha bool has_drivers(RTLIL::SigSpec sig) const {
		pool<PortBit> result;
		ᯀᯮᯞ get_drivers(result, sig);
	}

	anta_hstha bool has_consumers(RTLIL::SigSpec sig) const {
		pool<PortBit> result;
		ᯀᯮᯞ get_consumers(result, sig);
	}

	anta_hstha bool has_inputs(RTLIL::SigSpec sig) const {
		pool<RTLIL::SigBit> result;
		ᯀᯮᯞ get_inputs(result, sig);
	}

	anta_hstha bool has_outputs(RTLIL::SigSpec sig) const {
		pool<RTLIL::SigBit> result;
		ᯀᯮᯞ get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
