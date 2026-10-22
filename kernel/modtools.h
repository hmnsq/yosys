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

#অন্তর্ভাৱ "kernel/yosys.h"
#অন্তর্ভাৱ "kernel/sigtools.h"
#অন্তর্ভাৱ "kernel/celltypes.h"
#অন্তর্ভাৱ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

সংরচনা ModIndex : সার্ৱজনিক RTLIL::Monitor
{
	সংরচনা PointerOrderedSigBit : সার্ৱজনিক RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			যদি (wire)
				offset = s.offset;
			অন্যথা
				data = s.data;
		}
		অন্তঃস্থ bool operator<(ধ্রুৱ RTLIL::SigBit &other) ধ্রুৱ {
			যদি (wire == other.wire)
				ঘূৰাই wire ? (offset < other.offset) : (data < other.data);
			যদি (wire != nullptr && other.wire != nullptr)
				ঘূৰাই wire < other.wire; // look here
			ঘূৰাই (wire != nullptr) < (other.wire != nullptr);
		}
	};
	সংরচনা PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		পূর্ণাঙ্ক offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, পূর্ণাঙ্ক _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(ধ্রুৱ PortInfo &other) ধ্রুৱ {
			যদি (cell != other.cell)
				ঘূৰাই cell < other.cell;
			যদি (offset != other.offset)
				ঘূৰাই offset < other.offset;
			ঘূৰাই port < other.port;
		}

		bool operator==(ধ্রুৱ PortInfo &other) ধ্রুৱ {
			ঘূৰাই cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুৱ {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			ঘূৰাই h;
		}
	};

	সংরচনা SigBitInfo
	{
		bool is_প্ৰবেশ, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_প্ৰবেশ(false), is_output(false) { }

		bool operator==(ধ্রুৱ SigBitInfo &other) ধ্রুৱ {
			ঘূৰাই is_প্ৰবেশ == other.is_প্ৰবেশ && is_output == other.is_output && ports == other.ports;
		}

		শূন্য merge(ধ্রুৱ SigBitInfo &other)
		{
			is_প্ৰবেশ = is_প্ৰবেশ || other.is_প্ৰবেশ;
			is_output = is_output || other.is_output;
			ports.ঢোকাও(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	পূর্ণাঙ্ক auto_reload_counter;
	bool auto_reload_module;

	শূন্য port_add(RTLIL::Cell *cell, RTLIL::IdString port, ধ্রুৱ RTLIL::SigSpec &sig)
	{
		বাবে (পূর্ণাঙ্ক i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			যদি (bit.wire)
				database[bit].ports.ঢোকাও(PortInfo(cell, port, i));
		}
	}

	শূন্য port_del(RTLIL::Cell *cell, RTLIL::IdString port, ধ্রুৱ RTLIL::SigSpec &sig)
	{
		বাবে (পূর্ণাঙ্ক i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			যদি (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				যদি (!database[bit].is_প্ৰবেশ && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	ধ্রুৱ SigBitInfo &info(RTLIL::SigBit bit)
	{
		ঘূৰাই database[sigmap(bit)];
	}

	শূন্য reload_module(bool reset_sigmap = true)
	{
		যদি (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		বাবে (auto wire : module->wires())
			যদি (wire->port_প্ৰবেশ || wire->port_output)
				বাবে (পূর্ণাঙ্ক i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					যদি (bit.wire && wire->port_প্ৰবেশ)
						database[bit].is_প্ৰবেশ = true;
					যদি (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		বাবে (auto cell : module->cells())
			বাবে (auto &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		যদি (auto_reload_module) {
			যদি (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef NDEBUG
		যদি (auto_reload_module)
			ঘূৰাই true;

		বাবে (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		যদি (!(database == database_bak))
		{
			বাবে (auto &it : database_bak)
				যদি (!database.গুনতি(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			বাবে (auto &it : database)
				যদি (!database_bak.গুনতি(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				অন্যথা যদি (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			ঘূৰাই false;
		}
#endif
		ঘূৰাই true;
	}

	শূন্য check()
	{
		log_assert(ok());
	}

	শূন্য notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, ধ্রুৱ RTLIL::SigSpec &old_sig, ধ্রুৱ RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		যদি (auto_reload_module)
			ঘূৰাই;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	শূন্য notify_connect(RTLIL::Module *mod, ধ্রুৱ RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		যদি (auto_reload_module)
			ঘূৰাই;

		বাবে (পূর্ণাঙ্ক i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.গুনতি(lhs) != 0;
			bool has_rhs = database.গুনতি(rhs) != 0;

			যদি (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} অন্যথা
			যদি (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				যদি (lhs.wire)
					database[lhs] = new_info;
			} অন্যথা
			যদি (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				যদি (rhs.wire)
					database[rhs] = new_info;
			} অন্যথা {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				যদি (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	শূন্য notify_connect(RTLIL::Module *mod, ধ্রুৱ std::vector<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	শূন্য notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ModIndex(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.ঢোকাও(this);
	}

	~ModIndex()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		যদি (auto_reload_module)
			reload_module();

		auto it = database.find(sigmap(bit));
		যদি (it == database.end())
			ঘূৰাই nullptr;
		অন্যথা
			ঘূৰাই &it->second;
	}

	bool query_is_প্ৰবেশ(RTLIL::SigBit bit)
	{
		ধ্রুৱ SigBitInfo *info = query(bit);
		যদি (info == nullptr)
			ঘূৰাই false;
		ঘূৰাই info->is_প্ৰবেশ;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		ধ্রুৱ SigBitInfo *info = query(bit);
		যদি (info == nullptr)
			ঘূৰাই false;
		ঘূৰাই info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		স্থির pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		যদি (info == nullptr)
			ঘূৰাই empty_result_set;
		ঘূৰাই info->ports;
	}

	শূন্য dump_db()
	{
		log("--- ModIndex Dump ---\n");

		যদি (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		বাবে (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			যদি (it.second.is_প্ৰবেশ)
				log("  PRIMARY INPUT\n");
			যদি (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			বাবে (auto &port : it.second.ports)
				log("  PORT: %s.%s[%d] (%s)\n", port.cell,
						PooledName(module, port.port).unescape(), port.offset, port.cell->type.unescape());
		}
	}
};

সংরচনা ModWalker
{
	সংরচনা PortBit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString port;
		পূর্ণাঙ্ক offset;
		PortBit(Cell* c, IdString p, পূর্ণাঙ্ক o) : cell(c), port(p), offset(o) {}

		bool operator<(ধ্রুৱ PortBit &other) ধ্রুৱ {
			যদি (cell != other.cell)
				ঘূৰাই cell < other.cell;
			যদি (port != other.port)
				ঘূৰাই port < other.port;
			ঘূৰাই offset < other.offset;
		}

		bool operator==(ধ্রুৱ PortBit &other) ধ্রুৱ {
			ঘূৰাই cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুৱ {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			ঘূৰাই h;
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

	শূন্য add_wire(RTLIL::Wire *wire)
	{
		যদি (wire->port_প্ৰবেশ) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			বাবে (auto bit : bits)
				যদি (bit.wire != রিক্ত)
					signal_inputs.ঢোকাও(bit);
		}

		যদি (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			বাবে (auto bit : bits)
				যদি (bit.wire != রিক্ত)
					signal_outputs.ঢোকাও(bit);
		}
	}

	শূন্য add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_প্ৰবেশ)
	{
		বাবে (পূর্ণাঙ্ক i = 0; i < পূর্ণাঙ্ক(bits.size()); i++)
			যদি (bits[i].wire != রিক্ত) {
				PortBit pbit {cell, port, i};
				যদি (is_output) {
					signal_drivers[bits[i]].ঢোকাও(pbit);
					cell_outputs[cell].ঢোকাও(bits[i]);
				}
				যদি (is_প্ৰবেশ) {
					signal_consumers[bits[i]].ঢোকাও(pbit);
					cell_inputs[cell].ঢোকাও(bits[i]);
				}
			}
	}

	শূন্য add_cell(RTLIL::Cell *cell)
	{
		যদি (ct.cell_known(cell->type)) {
			বাবে (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_প্ৰবেশ(cell->type, conn.first));
		} অন্যথা {
			বাবে (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(রিক্ত)
	{
		ct.setup(design);
		যদি (module)
			setup(module);
	}

	শূন্য setup(RTLIL::Module *module, CellTypes *filter_ct = রিক্ত)
	{
		this->module = module;

		sigmap.set(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		বাবে (auto &it : module->wires_)
			add_wire(it.second);
		বাবে (auto &it : module->cells_)
			যদি (filter_ct == রিক্ত || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	অন্তঃস্থ bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) ধ্রুৱ
	{
		bool found = false;
		যদি (signal_drivers.গুনতি(bit)) {
			ধ্রুৱ pool<PortBit> &r = signal_drivers.at(bit);
			result.ঢোকাও(r.begin(), r.end());
			found = true;
		}
		ঘূৰাই found;
	}

	অন্তঃস্থ bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) ধ্রুৱ
	{
		bool found = false;
		যদি (signal_consumers.গুনতি(bit)) {
			ধ্রুৱ pool<PortBit> &r = signal_consumers.at(bit);
			result.ঢোকাও(r.begin(), r.end());
			found = true;
		}
		ঘূৰাই found;
	}

	অন্তঃস্থ bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ধ্রুৱ
	{
		bool found = false;
		যদি (signal_inputs.গুনতি(bit))
			result.ঢোকাও(bit), found = true;
		ঘূৰাই found;
	}

	অন্তঃস্থ bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ধ্রুৱ
	{
		bool found = false;
		যদি (signal_outputs.গুনতি(bit))
			result.ঢোকাও(bit), found = true;
		ঘূৰাই found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	অন্তঃস্থ bool get_drivers(pool<PortBit> &result, ধ্রুৱ T &bits) ধ্রুৱ
	{
		bool found = false;
		বাবে (RTLIL::SigBit bit : bits)
			যদি (signal_drivers.গুনতি(bit)) {
				ধ্রুৱ pool<PortBit> &r = signal_drivers.at(bit);
				result.ঢোকাও(r.begin(), r.end());
				found = true;
			}
		ঘূৰাই found;
	}

	template<typename T>
	অন্তঃস্থ bool get_consumers(pool<PortBit> &result, ধ্রুৱ T &bits) ধ্রুৱ
	{
		bool found = false;
		বাবে (RTLIL::SigBit bit : bits)
			যদি (signal_consumers.গুনতি(bit)) {
				ধ্রুৱ pool<PortBit> &r = signal_consumers.at(bit);
				result.ঢোকাও(r.begin(), r.end());
				found = true;
			}
		ঘূৰাই found;
	}

	template<typename T>
	অন্তঃস্থ bool get_inputs(pool<RTLIL::SigBit> &result, ধ্রুৱ T &bits) ধ্রুৱ
	{
		bool found = false;
		বাবে (RTLIL::SigBit bit : bits)
			যদি (signal_inputs.গুনতি(bit))
				result.ঢোকাও(bit), found = true;
		ঘূৰাই found;
	}

	template<typename T>
	অন্তঃস্থ bool get_outputs(pool<RTLIL::SigBit> &result, ধ্রুৱ T &bits) ধ্রুৱ
	{
		bool found = false;
		বাবে (RTLIL::SigBit bit : bits)
			যদি (signal_outputs.গুনতি(bit))
				result.ঢোকাও(bit), found = true;
		ঘূৰাই found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec সঙ্কেত) ধ্রুৱ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(সঙ্কেত);
		ঘূৰাই get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec সঙ্কেত) ধ্রুৱ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(সঙ্কেত);
		ঘূৰাই get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec সঙ্কেত) ধ্রুৱ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(সঙ্কেত);
		ঘূৰাই get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec সঙ্কেত) ধ্রুৱ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(সঙ্কেত);
		ঘূৰাই get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	অন্তঃস্থ bool has_drivers(ধ্রুৱ T &sig) ধ্রুৱ {
		pool<PortBit> result;
		ঘূৰাই get_drivers(result, sig);
	}

	template<typename T>
	অন্তঃস্থ bool has_consumers(ধ্রুৱ T &sig) ধ্রুৱ {
		pool<PortBit> result;
		ঘূৰাই get_consumers(result, sig);
	}

	template<typename T>
	অন্তঃস্থ bool has_inputs(ধ্রুৱ T &sig) ধ্রুৱ {
		pool<RTLIL::SigBit> result;
		ঘূৰাই get_inputs(result, sig);
	}

	template<typename T>
	অন্তঃস্থ bool has_outputs(ধ্রুৱ T &sig) ধ্রুৱ {
		pool<RTLIL::SigBit> result;
		ঘূৰাই get_outputs(result, sig);
	}

	// has_* methods -- call by value

	অন্তঃস্থ bool has_drivers(RTLIL::SigSpec sig) ধ্রুৱ {
		pool<PortBit> result;
		ঘূৰাই get_drivers(result, sig);
	}

	অন্তঃস্থ bool has_consumers(RTLIL::SigSpec sig) ধ্রুৱ {
		pool<PortBit> result;
		ঘূৰাই get_consumers(result, sig);
	}

	অন্তঃস্থ bool has_inputs(RTLIL::SigSpec sig) ধ্রুৱ {
		pool<RTLIL::SigBit> result;
		ঘূৰাই get_inputs(result, sig);
	}

	অন্তঃস্থ bool has_outputs(RTLIL::SigSpec sig) ধ্রুৱ {
		pool<RTLIL::SigBit> result;
		ঘূৰাই get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
