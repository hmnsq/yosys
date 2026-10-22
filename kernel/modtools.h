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

struct ModIndex : public RTLIL::Monitor
{
	struct PointerOrderedSigBit : public RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			如果 (wire)
				offset = s.offset;
			否则
				data = s.data;
		}
		inline bool operator<(const RTLIL::SigBit &other) const {
			如果 (wire == other.wire)
				返回 wire ? (offset < other.offset) : (data < other.data);
			如果 (wire != nullptr && other.wire != nullptr)
				返回 wire < other.wire; // look here
			返回 (wire != nullptr) < (other.wire != nullptr);
		}
	};
	struct PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		整数 offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, 整数 _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(const PortInfo &other) const {
			如果 (cell != other.cell)
				返回 cell < other.cell;
			如果 (offset != other.offset)
				返回 offset < other.offset;
			返回 port < other.port;
		}

		bool operator==(const PortInfo &other) const {
			返回 cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			返回 h;
		}
	};

	struct SigBitInfo
	{
		bool is_input, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_input(false), is_output(false) { }

		bool operator==(const SigBitInfo &other) const {
			返回 is_input == other.is_input && is_output == other.is_output && ports == other.ports;
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
	整数 auto_reload_counter;
	bool auto_reload_module;

	void port_add(RTLIL::Cell *cell, RTLIL::IdString port, const RTLIL::SigSpec &sig)
	{
		为 (整数 i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			如果 (bit.wire)
				database[bit].ports.insert(PortInfo(cell, port, i));
		}
	}

	void port_del(RTLIL::Cell *cell, RTLIL::IdString port, const RTLIL::SigSpec &sig)
	{
		为 (整数 i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			如果 (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				如果 (!database[bit].is_input && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	const SigBitInfo &info(RTLIL::SigBit bit)
	{
		返回 database[sigmap(bit)];
	}

	void reload_module(bool reset_sigmap = true)
	{
		如果 (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		为 (auto wire : module->wires())
			如果 (wire->port_input || wire->port_output)
				为 (整数 i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					如果 (bit.wire && wire->port_input)
						database[bit].is_input = true;
					如果 (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		为 (auto cell : module->cells())
			为 (auto &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		如果 (auto_reload_module) {
			如果 (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef NDEBUG
		如果 (auto_reload_module)
			返回 true;

		为 (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		如果 (!(database == database_bak))
		{
			为 (auto &it : database_bak)
				如果 (!database.count(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			为 (auto &it : database)
				如果 (!database_bak.count(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				否则 如果 (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			返回 false;
		}
#endif
		返回 true;
	}

	void check()
	{
		log_assert(ok());
	}

	void notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, const RTLIL::SigSpec &old_sig, const RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		如果 (auto_reload_module)
			返回;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	void notify_connect(RTLIL::Module *mod, const RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		如果 (auto_reload_module)
			返回;

		为 (整数 i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.count(lhs) != 0;
			bool has_rhs = database.count(rhs) != 0;

			如果 (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} 否则
			如果 (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				如果 (lhs.wire)
					database[lhs] = new_info;
			} 否则
			如果 (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				如果 (rhs.wire)
					database[rhs] = new_info;
			} 否则 {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				如果 (rhs.wire)
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
		如果 (auto_reload_module)
			reload_module();

		auto it = database.find(sigmap(bit));
		如果 (it == database.end())
			返回 nullptr;
		否则
			返回 &it->second;
	}

	bool query_is_input(RTLIL::SigBit bit)
	{
		const SigBitInfo *info = query(bit);
		如果 (info == nullptr)
			返回 false;
		返回 info->is_input;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		const SigBitInfo *info = query(bit);
		如果 (info == nullptr)
			返回 false;
		返回 info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		static pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		如果 (info == nullptr)
			返回 empty_result_set;
		返回 info->ports;
	}

	void dump_db()
	{
		log("--- ModIndex Dump ---\n");

		如果 (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		为 (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			如果 (it.second.is_input)
				log("  PRIMARY INPUT\n");
			如果 (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			为 (auto &port : it.second.ports)
				log("  PORT: %s.%s[%d] (%s)\n", port.cell,
						PooledName(module, port.port).unescape(), port.offset, port.cell->type.unescape());
		}
	}
};

struct ModWalker
{
	struct PortBit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString port;
		整数 offset;
		PortBit(Cell* c, IdString p, 整数 o) : cell(c), port(p), offset(o) {}

		bool operator<(const PortBit &other) const {
			如果 (cell != other.cell)
				返回 cell < other.cell;
			如果 (port != other.port)
				返回 port < other.port;
			返回 offset < other.offset;
		}

		bool operator==(const PortBit &other) const {
			返回 cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) const {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			返回 h;
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
		如果 (wire->port_input) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			为 (auto bit : bits)
				如果 (bit.wire != NULL)
					signal_inputs.insert(bit);
		}

		如果 (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			为 (auto bit : bits)
				如果 (bit.wire != NULL)
					signal_outputs.insert(bit);
		}
	}

	void add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_input)
	{
		为 (整数 i = 0; i < 整数(bits.size()); i++)
			如果 (bits[i].wire != NULL) {
				PortBit pbit {cell, port, i};
				如果 (is_output) {
					signal_drivers[bits[i]].insert(pbit);
					cell_outputs[cell].insert(bits[i]);
				}
				如果 (is_input) {
					signal_consumers[bits[i]].insert(pbit);
					cell_inputs[cell].insert(bits[i]);
				}
			}
	}

	void add_cell(RTLIL::Cell *cell)
	{
		如果 (ct.cell_known(cell->type)) {
			为 (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_input(cell->type, conn.first));
		} 否则 {
			为 (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(NULL)
	{
		ct.setup(design);
		如果 (module)
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

		为 (auto &it : module->wires_)
			add_wire(it.second);
		为 (auto &it : module->cells_)
			如果 (filter_ct == NULL || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	inline bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) const
	{
		bool found = false;
		如果 (signal_drivers.count(bit)) {
			const pool<PortBit> &r = signal_drivers.at(bit);
			result.insert(r.begin(), r.end());
			found = true;
		}
		返回 found;
	}

	inline bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) const
	{
		bool found = false;
		如果 (signal_consumers.count(bit)) {
			const pool<PortBit> &r = signal_consumers.at(bit);
			result.insert(r.begin(), r.end());
			found = true;
		}
		返回 found;
	}

	inline bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) const
	{
		bool found = false;
		如果 (signal_inputs.count(bit))
			result.insert(bit), found = true;
		返回 found;
	}

	inline bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) const
	{
		bool found = false;
		如果 (signal_outputs.count(bit))
			result.insert(bit), found = true;
		返回 found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	inline bool get_drivers(pool<PortBit> &result, const T &bits) const
	{
		bool found = false;
		为 (RTLIL::SigBit bit : bits)
			如果 (signal_drivers.count(bit)) {
				const pool<PortBit> &r = signal_drivers.at(bit);
				result.insert(r.begin(), r.end());
				found = true;
			}
		返回 found;
	}

	template<typename T>
	inline bool get_consumers(pool<PortBit> &result, const T &bits) const
	{
		bool found = false;
		为 (RTLIL::SigBit bit : bits)
			如果 (signal_consumers.count(bit)) {
				const pool<PortBit> &r = signal_consumers.at(bit);
				result.insert(r.begin(), r.end());
				found = true;
			}
		返回 found;
	}

	template<typename T>
	inline bool get_inputs(pool<RTLIL::SigBit> &result, const T &bits) const
	{
		bool found = false;
		为 (RTLIL::SigBit bit : bits)
			如果 (signal_inputs.count(bit))
				result.insert(bit), found = true;
		返回 found;
	}

	template<typename T>
	inline bool get_outputs(pool<RTLIL::SigBit> &result, const T &bits) const
	{
		bool found = false;
		为 (RTLIL::SigBit bit : bits)
			如果 (signal_outputs.count(bit))
				result.insert(bit), found = true;
		返回 found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec signal) const
	{
		std::vector<RTLIL::SigBit> bits = sigmap(signal);
		返回 get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec signal) const
	{
		std::vector<RTLIL::SigBit> bits = sigmap(signal);
		返回 get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec signal) const
	{
		std::vector<RTLIL::SigBit> bits = sigmap(signal);
		返回 get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec signal) const
	{
		std::vector<RTLIL::SigBit> bits = sigmap(signal);
		返回 get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	inline bool has_drivers(const T &sig) const {
		pool<PortBit> result;
		返回 get_drivers(result, sig);
	}

	template<typename T>
	inline bool has_consumers(const T &sig) const {
		pool<PortBit> result;
		返回 get_consumers(result, sig);
	}

	template<typename T>
	inline bool has_inputs(const T &sig) const {
		pool<RTLIL::SigBit> result;
		返回 get_inputs(result, sig);
	}

	template<typename T>
	inline bool has_outputs(const T &sig) const {
		pool<RTLIL::SigBit> result;
		返回 get_outputs(result, sig);
	}

	// has_* methods -- call by value

	inline bool has_drivers(RTLIL::SigSpec sig) const {
		pool<PortBit> result;
		返回 get_drivers(result, sig);
	}

	inline bool has_consumers(RTLIL::SigSpec sig) const {
		pool<PortBit> result;
		返回 get_consumers(result, sig);
	}

	inline bool has_inputs(RTLIL::SigSpec sig) const {
		pool<RTLIL::SigBit> result;
		返回 get_inputs(result, sig);
	}

	inline bool has_outputs(RTLIL::SigSpec sig) const {
		pool<RTLIL::SigBit> result;
		返回 get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
