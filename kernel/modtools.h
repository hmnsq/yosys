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

#অন্তর্ভাব "kernel/yosys.h"
#অন্তর্ভাব "kernel/sigtools.h"
#অন্তর্ভাব "kernel/celltypes.h"
#অন্তর্ভাব "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

সংরচনা ModIndex : সার্বজনিক RTLIL::Monitor
{
	সংরচনা PointerOrderedSigBit : সার্বজনিক RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			যদি (wire)
				offset = s.offset;
			অন্যথা
				data = s.data;
		}
		অন্তঃস্থ bool operator<(ধ্রুব RTLIL::SigBit &other) ধ্রুব {
			যদি (wire == other.wire)
				প্রত্যাগম wire ? (offset < other.offset) : (data < other.data);
			যদি (wire != nullptr && other.wire != nullptr)
				প্রত্যাগম wire < other.wire; // look here
			প্রত্যাগম (wire != nullptr) < (other.wire != nullptr);
		}
	};
	সংরচনা PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		পূর্ণাঙ্ক offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, পূর্ণাঙ্ক _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(ধ্রুব PortInfo &other) ধ্রুব {
			যদি (cell != other.cell)
				প্রত্যাগম cell < other.cell;
			যদি (offset != other.offset)
				প্রত্যাগম offset < other.offset;
			প্রত্যাগম port < other.port;
		}

		bool operator==(ধ্রুব PortInfo &other) ধ্রুব {
			প্রত্যাগম cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			প্রত্যাগম h;
		}
	};

	সংরচনা SigBitInfo
	{
		bool is_প্রবেশ, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_প্রবেশ(false), is_output(false) { }

		bool operator==(ধ্রুব SigBitInfo &other) ধ্রুব {
			প্রত্যাগম is_প্রবেশ == other.is_প্রবেশ && is_output == other.is_output && ports == other.ports;
		}

		শূন্য merge(ধ্রুব SigBitInfo &other)
		{
			is_প্রবেশ = is_প্রবেশ || other.is_প্রবেশ;
			is_output = is_output || other.is_output;
			ports.ঢোকাও(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	পূর্ণাঙ্ক auto_reload_counter;
	bool auto_reload_module;

	শূন্য port_add(RTLIL::Cell *cell, RTLIL::IdString port, ধ্রুব RTLIL::SigSpec &sig)
	{
		প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			যদি (bit.wire)
				database[bit].ports.ঢোকাও(PortInfo(cell, port, i));
		}
	}

	শূন্য port_del(RTLIL::Cell *cell, RTLIL::IdString port, ধ্রুব RTLIL::SigSpec &sig)
	{
		প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			যদি (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				যদি (!database[bit].is_প্রবেশ && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	ধ্রুব SigBitInfo &info(RTLIL::SigBit bit)
	{
		প্রত্যাগম database[sigmap(bit)];
	}

	শূন্য reload_module(bool reset_sigmap = true)
	{
		যদি (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		প্রতি (auto wire : module->wires())
			যদি (wire->port_প্রবেশ || wire->port_output)
				প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					যদি (bit.wire && wire->port_প্রবেশ)
						database[bit].is_প্রবেশ = true;
					যদি (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		প্রতি (auto cell : module->cells())
			প্রতি (auto &conn : cell->connections())
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
			প্রত্যাগম true;

		প্রতি (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		যদি (!(database == database_bak))
		{
			প্রতি (auto &it : database_bak)
				যদি (!database.গুনতি(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			প্রতি (auto &it : database)
				যদি (!database_bak.গুনতি(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				অন্যথা যদি (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			প্রত্যাগম false;
		}
#endif
		প্রত্যাগম true;
	}

	শূন্য check()
	{
		log_assert(ok());
	}

	শূন্য notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, ধ্রুব RTLIL::SigSpec &old_sig, ধ্রুব RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		যদি (auto_reload_module)
			প্রত্যাগম;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	শূন্য notify_connect(RTLIL::Module *mod, ধ্রুব RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		যদি (auto_reload_module)
			প্রত্যাগম;

		প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(sigsig.first); i++)
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

	শূন্য notify_connect(RTLIL::Module *mod, ধ্রুব std::vector<RTLIL::SigSig>&) override
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
			প্রত্যাগম nullptr;
		অন্যথা
			প্রত্যাগম &it->second;
	}

	bool query_is_প্রবেশ(RTLIL::SigBit bit)
	{
		ধ্রুব SigBitInfo *info = query(bit);
		যদি (info == nullptr)
			প্রত্যাগম false;
		প্রত্যাগম info->is_প্রবেশ;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		ধ্রুব SigBitInfo *info = query(bit);
		যদি (info == nullptr)
			প্রত্যাগম false;
		প্রত্যাগম info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		স্থির pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		যদি (info == nullptr)
			প্রত্যাগম empty_result_set;
		প্রত্যাগম info->ports;
	}

	শূন্য dump_db()
	{
		log("--- ModIndex Dump ---\n");

		যদি (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		প্রতি (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			যদি (it.second.is_প্রবেশ)
				log("  PRIMARY INPUT\n");
			যদি (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			প্রতি (auto &port : it.second.ports)
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

		bool operator<(ধ্রুব PortBit &other) ধ্রুব {
			যদি (cell != other.cell)
				প্রত্যাগম cell < other.cell;
			যদি (port != other.port)
				প্রত্যাগম port < other.port;
			প্রত্যাগম offset < other.offset;
		}

		bool operator==(ধ্রুব PortBit &other) ধ্রুব {
			প্রত্যাগম cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			প্রত্যাগম h;
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
		যদি (wire->port_প্রবেশ) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			প্রতি (auto bit : bits)
				যদি (bit.wire != রিক্ত)
					signal_inputs.ঢোকাও(bit);
		}

		যদি (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			প্রতি (auto bit : bits)
				যদি (bit.wire != রিক্ত)
					signal_outputs.ঢোকাও(bit);
		}
	}

	শূন্য add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_প্রবেশ)
	{
		প্রতি (পূর্ণাঙ্ক i = 0; i < পূর্ণাঙ্ক(bits.size()); i++)
			যদি (bits[i].wire != রিক্ত) {
				PortBit pbit {cell, port, i};
				যদি (is_output) {
					signal_drivers[bits[i]].ঢোকাও(pbit);
					cell_outputs[cell].ঢোকাও(bits[i]);
				}
				যদি (is_প্রবেশ) {
					signal_consumers[bits[i]].ঢোকাও(pbit);
					cell_inputs[cell].ঢোকাও(bits[i]);
				}
			}
	}

	শূন্য add_cell(RTLIL::Cell *cell)
	{
		যদি (ct.cell_known(cell->type)) {
			প্রতি (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_প্রবেশ(cell->type, conn.first));
		} অন্যথা {
			প্রতি (auto &conn : cell->connections())
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

		প্রতি (auto &it : module->wires_)
			add_wire(it.second);
		প্রতি (auto &it : module->cells_)
			যদি (filter_ct == রিক্ত || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	অন্তঃস্থ bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) ধ্রুব
	{
		bool found = false;
		যদি (signal_drivers.গুনতি(bit)) {
			ধ্রুব pool<PortBit> &r = signal_drivers.at(bit);
			result.ঢোকাও(r.begin(), r.end());
			found = true;
		}
		প্রত্যাগম found;
	}

	অন্তঃস্থ bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) ধ্রুব
	{
		bool found = false;
		যদি (signal_consumers.গুনতি(bit)) {
			ধ্রুব pool<PortBit> &r = signal_consumers.at(bit);
			result.ঢোকাও(r.begin(), r.end());
			found = true;
		}
		প্রত্যাগম found;
	}

	অন্তঃস্থ bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ধ্রুব
	{
		bool found = false;
		যদি (signal_inputs.গুনতি(bit))
			result.ঢোকাও(bit), found = true;
		প্রত্যাগম found;
	}

	অন্তঃস্থ bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ধ্রুব
	{
		bool found = false;
		যদি (signal_outputs.গুনতি(bit))
			result.ঢোকাও(bit), found = true;
		প্রত্যাগম found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	অন্তঃস্থ bool get_drivers(pool<PortBit> &result, ধ্রুব T &bits) ধ্রুব
	{
		bool found = false;
		প্রতি (RTLIL::SigBit bit : bits)
			যদি (signal_drivers.গুনতি(bit)) {
				ধ্রুব pool<PortBit> &r = signal_drivers.at(bit);
				result.ঢোকাও(r.begin(), r.end());
				found = true;
			}
		প্রত্যাগম found;
	}

	template<typename T>
	অন্তঃস্থ bool get_consumers(pool<PortBit> &result, ধ্রুব T &bits) ধ্রুব
	{
		bool found = false;
		প্রতি (RTLIL::SigBit bit : bits)
			যদি (signal_consumers.গুনতি(bit)) {
				ধ্রুব pool<PortBit> &r = signal_consumers.at(bit);
				result.ঢোকাও(r.begin(), r.end());
				found = true;
			}
		প্রত্যাগম found;
	}

	template<typename T>
	অন্তঃস্থ bool get_inputs(pool<RTLIL::SigBit> &result, ধ্রুব T &bits) ধ্রুব
	{
		bool found = false;
		প্রতি (RTLIL::SigBit bit : bits)
			যদি (signal_inputs.গুনতি(bit))
				result.ঢোকাও(bit), found = true;
		প্রত্যাগম found;
	}

	template<typename T>
	অন্তঃস্থ bool get_outputs(pool<RTLIL::SigBit> &result, ধ্রুব T &bits) ধ্রুব
	{
		bool found = false;
		প্রতি (RTLIL::SigBit bit : bits)
			যদি (signal_outputs.গুনতি(bit))
				result.ঢোকাও(bit), found = true;
		প্রত্যাগম found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec সঙ্কেত) ধ্রুব
	{
		std::vector<RTLIL::SigBit> bits = sigmap(সঙ্কেত);
		প্রত্যাগম get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec সঙ্কেত) ধ্রুব
	{
		std::vector<RTLIL::SigBit> bits = sigmap(সঙ্কেত);
		প্রত্যাগম get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec সঙ্কেত) ধ্রুব
	{
		std::vector<RTLIL::SigBit> bits = sigmap(সঙ্কেত);
		প্রত্যাগম get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec সঙ্কেত) ধ্রুব
	{
		std::vector<RTLIL::SigBit> bits = sigmap(সঙ্কেত);
		প্রত্যাগম get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	অন্তঃস্থ bool has_drivers(ধ্রুব T &sig) ধ্রুব {
		pool<PortBit> result;
		প্রত্যাগম get_drivers(result, sig);
	}

	template<typename T>
	অন্তঃস্থ bool has_consumers(ধ্রুব T &sig) ধ্রুব {
		pool<PortBit> result;
		প্রত্যাগম get_consumers(result, sig);
	}

	template<typename T>
	অন্তঃস্থ bool has_inputs(ধ্রুব T &sig) ধ্রুব {
		pool<RTLIL::SigBit> result;
		প্রত্যাগম get_inputs(result, sig);
	}

	template<typename T>
	অন্তঃস্থ bool has_outputs(ধ্রুব T &sig) ধ্রুব {
		pool<RTLIL::SigBit> result;
		প্রত্যাগম get_outputs(result, sig);
	}

	// has_* methods -- call by value

	অন্তঃস্থ bool has_drivers(RTLIL::SigSpec sig) ধ্রুব {
		pool<PortBit> result;
		প্রত্যাগম get_drivers(result, sig);
	}

	অন্তঃস্থ bool has_consumers(RTLIL::SigSpec sig) ধ্রুব {
		pool<PortBit> result;
		প্রত্যাগম get_consumers(result, sig);
	}

	অন্তঃস্থ bool has_inputs(RTLIL::SigSpec sig) ধ্রুব {
		pool<RTLIL::SigBit> result;
		প্রত্যাগম get_inputs(result, sig);
	}

	অন্তঃস্থ bool has_outputs(RTLIL::SigSpec sig) ধ্রুব {
		pool<RTLIL::SigBit> result;
		প্রত্যাগম get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
