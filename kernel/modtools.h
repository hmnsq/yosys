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

কাঠামো Mod_সূচক : সর্বজনীন RTLIL::Monitor
{
	কাঠামো PointerOrderedSigBit : সর্বজনীন RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			যদি (wire)
				offset = s.offset;
			অন্যথা
				তথ্য = s.তথ্য;
		}
		অন্তঃরেখ bool operator<(ধ্রুবক RTLIL::SigBit &other) ধ্রুবক {
			যদি (wire == other.wire)
				ফেরত wire ? (offset < other.offset) : (তথ্য < other.তথ্য);
			যদি (wire != nullptr && other.wire != nullptr)
				ফেরত wire < other.wire; // look here
			ফেরত (wire != nullptr) < (other.wire != nullptr);
		}
	};
	কাঠামো PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		পূর্ণসংখ্যা offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, পূর্ণসংখ্যা _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(ধ্রুবক PortInfo &other) ধ্রুবক {
			যদি (cell != other.cell)
				ফেরত cell < other.cell;
			যদি (offset != other.offset)
				ফেরত offset < other.offset;
			ফেরত port < other.port;
		}

		bool operator==(ধ্রুবক PortInfo &other) ধ্রুবক {
			ফেরত cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			ফেরত h;
		}
	};

	কাঠামো SigBitInfo
	{
		bool is_প্রবেশ, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_প্রবেশ(false), is_output(false) { }

		bool operator==(ধ্রুবক SigBitInfo &other) ধ্রুবক {
			ফেরত is_প্রবেশ == other.is_প্রবেশ && is_output == other.is_output && ports == other.ports;
		}

		শূন্য merge(ধ্রুবক SigBitInfo &other)
		{
			is_প্রবেশ = is_প্রবেশ || other.is_প্রবেশ;
			is_output = is_output || other.is_output;
			ports.ঢোকাও(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	পূর্ণসংখ্যা auto_reload_গণক;
	bool auto_reload_module;

	শূন্য port_add(RTLIL::Cell *cell, RTLIL::IdString port, ধ্রুবক RTLIL::SigSpec &sig)
	{
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			যদি (bit.wire)
				database[bit].ports.ঢোকাও(PortInfo(cell, port, i));
		}
	}

	শূন্য port_del(RTLIL::Cell *cell, RTLIL::IdString port, ধ্রুবক RTLIL::SigSpec &sig)
	{
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			যদি (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				যদি (!database[bit].is_প্রবেশ && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	ধ্রুবক SigBitInfo &info(RTLIL::SigBit bit)
	{
		ফেরত database[sigmap(bit)];
	}

	শূন্য reload_module(bool reset_sigmap = true)
	{
		যদি (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		জন্য (auto wire : module->wires())
			যদি (wire->port_প্রবেশ || wire->port_output)
				জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					যদি (bit.wire && wire->port_প্রবেশ)
						database[bit].is_প্রবেশ = true;
					যদি (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		জন্য (auto cell : module->cells())
			জন্য (auto &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		যদি (auto_reload_module) {
			যদি (++auto_reload_গণক > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef NDEBUG
		যদি (auto_reload_module)
			ফেরত true;

		জন্য (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		যদি (!(database == database_bak))
		{
			জন্য (auto &it : database_bak)
				যদি (!database.গুনতি(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			জন্য (auto &it : database)
				যদি (!database_bak.গুনতি(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				অন্যথা যদি (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			ফেরত false;
		}
#endif
		ফেরত true;
	}

	শূন্য check()
	{
		log_assert(ok());
	}

	শূন্য notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, ধ্রুবক RTLIL::SigSpec &old_sig, ধ্রুবক RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		যদি (auto_reload_module)
			ফেরত;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	শূন্য notify_connect(RTLIL::Module *mod, ধ্রুবক RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		যদি (auto_reload_module)
			ফেরত;

		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(sigsig.first); i++)
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

	শূন্য notify_connect(RTLIL::Module *mod, ধ্রুবক std::vector<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	শূন্য notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	Mod_সূচক(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_গণক = 0;
		auto_reload_module = true;
		module->monitors.ঢোকাও(this);
	}

	~Mod_সূচক()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		যদি (auto_reload_module)
			reload_module();

		auto it = database.find(sigmap(bit));
		যদি (it == database.end())
			ফেরত nullptr;
		অন্যথা
			ফেরত &it->second;
	}

	bool query_is_প্রবেশ(RTLIL::SigBit bit)
	{
		ধ্রুবক SigBitInfo *info = query(bit);
		যদি (info == nullptr)
			ফেরত false;
		ফেরত info->is_প্রবেশ;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		ধ্রুবক SigBitInfo *info = query(bit);
		যদি (info == nullptr)
			ফেরত false;
		ফেরত info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		স্থির pool<PortInfo> empty_ফলাফল_set;
		SigBitInfo *info = query(bit);
		যদি (info == nullptr)
			ফেরত empty_ফলাফল_set;
		ফেরত info->ports;
	}

	শূন্য dump_db()
	{
		log("--- ModIndex Dump ---\n");

		যদি (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		জন্য (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			যদি (it.second.is_প্রবেশ)
				log("  PRIMARY INPUT\n");
			যদি (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			জন্য (auto &port : it.second.ports)
				log("  PORT: %s.%s[%d] (%s)\n", port.cell,
						PooledName(module, port.port).unescape(), port.offset, port.cell->type.unescape());
		}
	}
};

কাঠামো ModWalker
{
	কাঠামো PortBit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString port;
		পূর্ণসংখ্যা offset;
		PortBit(Cell* c, IdString p, পূর্ণসংখ্যা o) : cell(c), port(p), offset(o) {}

		bool operator<(ধ্রুবক PortBit &other) ধ্রুবক {
			যদি (cell != other.cell)
				ফেরত cell < other.cell;
			যদি (port != other.port)
				ফেরত port < other.port;
			ফেরত offset < other.offset;
		}

		bool operator==(ধ্রুবক PortBit &other) ধ্রুবক {
			ফেরত cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			ফেরত h;
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
			জন্য (auto bit : bits)
				যদি (bit.wire != ফাঁকা)
					signal_inputs.ঢোকাও(bit);
		}

		যদি (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			জন্য (auto bit : bits)
				যদি (bit.wire != ফাঁকা)
					signal_outputs.ঢোকাও(bit);
		}
	}

	শূন্য add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_প্রবেশ)
	{
		জন্য (পূর্ণসংখ্যা i = 0; i < পূর্ণসংখ্যা(bits.আকার_2()); i++)
			যদি (bits[i].wire != ফাঁকা) {
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
			জন্য (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_প্রবেশ(cell->type, conn.first));
		} অন্যথা {
			জন্য (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(ফাঁকা)
	{
		ct.setup(design);
		যদি (module)
			setup(module);
	}

	শূন্য setup(RTLIL::Module *module, CellTypes *filter_ct = ফাঁকা)
	{
		this->module = module;

		sigmap.set(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		জন্য (auto &it : module->wires_)
			add_wire(it.second);
		জন্য (auto &it : module->cells_)
			যদি (filter_ct == ফাঁকা || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	অন্তঃরেখ bool get_drivers(pool<PortBit> &ফলাফল, RTLIL::SigBit bit) ধ্রুবক
	{
		bool found = false;
		যদি (signal_drivers.গুনতি(bit)) {
			ধ্রুবক pool<PortBit> &r = signal_drivers.at(bit);
			ফলাফল.ঢোকাও(r.begin(), r.end());
			found = true;
		}
		ফেরত found;
	}

	অন্তঃরেখ bool get_consumers(pool<PortBit> &ফলাফল, RTLIL::SigBit bit) ধ্রুবক
	{
		bool found = false;
		যদি (signal_consumers.গুনতি(bit)) {
			ধ্রুবক pool<PortBit> &r = signal_consumers.at(bit);
			ফলাফল.ঢোকাও(r.begin(), r.end());
			found = true;
		}
		ফেরত found;
	}

	অন্তঃরেখ bool get_inputs(pool<RTLIL::SigBit> &ফলাফল, RTLIL::SigBit bit) ধ্রুবক
	{
		bool found = false;
		যদি (signal_inputs.গুনতি(bit))
			ফলাফল.ঢোকাও(bit), found = true;
		ফেরত found;
	}

	অন্তঃরেখ bool get_outputs(pool<RTLIL::SigBit> &ফলাফল, RTLIL::SigBit bit) ধ্রুবক
	{
		bool found = false;
		যদি (signal_outputs.গুনতি(bit))
			ফলাফল.ঢোকাও(bit), found = true;
		ফেরত found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	অন্তঃরেখ bool get_drivers(pool<PortBit> &ফলাফল, ধ্রুবক T &bits) ধ্রুবক
	{
		bool found = false;
		জন্য (RTLIL::SigBit bit : bits)
			যদি (signal_drivers.গুনতি(bit)) {
				ধ্রুবক pool<PortBit> &r = signal_drivers.at(bit);
				ফলাফল.ঢোকাও(r.begin(), r.end());
				found = true;
			}
		ফেরত found;
	}

	template<typename T>
	অন্তঃরেখ bool get_consumers(pool<PortBit> &ফলাফল, ধ্রুবক T &bits) ধ্রুবক
	{
		bool found = false;
		জন্য (RTLIL::SigBit bit : bits)
			যদি (signal_consumers.গুনতি(bit)) {
				ধ্রুবক pool<PortBit> &r = signal_consumers.at(bit);
				ফলাফল.ঢোকাও(r.begin(), r.end());
				found = true;
			}
		ফেরত found;
	}

	template<typename T>
	অন্তঃরেখ bool get_inputs(pool<RTLIL::SigBit> &ফলাফল, ধ্রুবক T &bits) ধ্রুবক
	{
		bool found = false;
		জন্য (RTLIL::SigBit bit : bits)
			যদি (signal_inputs.গুনতি(bit))
				ফলাফল.ঢোকাও(bit), found = true;
		ফেরত found;
	}

	template<typename T>
	অন্তঃরেখ bool get_outputs(pool<RTLIL::SigBit> &ফলাফল, ধ্রুবক T &bits) ধ্রুবক
	{
		bool found = false;
		জন্য (RTLIL::SigBit bit : bits)
			যদি (signal_outputs.গুনতি(bit))
				ফলাফল.ঢোকাও(bit), found = true;
		ফেরত found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &ফলাফল, RTLIL::SigSpec সংকেত) ধ্রুবক
	{
		std::vector<RTLIL::SigBit> bits = sigmap(সংকেত);
		ফেরত get_drivers(ফলাফল, bits);
	}

	bool get_consumers(pool<PortBit> &ফলাফল, RTLIL::SigSpec সংকেত) ধ্রুবক
	{
		std::vector<RTLIL::SigBit> bits = sigmap(সংকেত);
		ফেরত get_consumers(ফলাফল, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &ফলাফল, RTLIL::SigSpec সংকেত) ধ্রুবক
	{
		std::vector<RTLIL::SigBit> bits = sigmap(সংকেত);
		ফেরত get_inputs(ফলাফল, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &ফলাফল, RTLIL::SigSpec সংকেত) ধ্রুবক
	{
		std::vector<RTLIL::SigBit> bits = sigmap(সংকেত);
		ফেরত get_outputs(ফলাফল, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	অন্তঃরেখ bool has_drivers(ধ্রুবক T &sig) ধ্রুবক {
		pool<PortBit> ফলাফল;
		ফেরত get_drivers(ফলাফল, sig);
	}

	template<typename T>
	অন্তঃরেখ bool has_consumers(ধ্রুবক T &sig) ধ্রুবক {
		pool<PortBit> ফলাফল;
		ফেরত get_consumers(ফলাফল, sig);
	}

	template<typename T>
	অন্তঃরেখ bool has_inputs(ধ্রুবক T &sig) ধ্রুবক {
		pool<RTLIL::SigBit> ফলাফল;
		ফেরত get_inputs(ফলাফল, sig);
	}

	template<typename T>
	অন্তঃরেখ bool has_outputs(ধ্রুবক T &sig) ধ্রুবক {
		pool<RTLIL::SigBit> ফলাফল;
		ফেরত get_outputs(ফলাফল, sig);
	}

	// has_* methods -- call by value

	অন্তঃরেখ bool has_drivers(RTLIL::SigSpec sig) ধ্রুবক {
		pool<PortBit> ফলাফল;
		ফেরত get_drivers(ফলাফল, sig);
	}

	অন্তঃরেখ bool has_consumers(RTLIL::SigSpec sig) ধ্রুবক {
		pool<PortBit> ফলাফল;
		ফেরত get_consumers(ফলাফল, sig);
	}

	অন্তঃরেখ bool has_inputs(RTLIL::SigSpec sig) ধ্রুবক {
		pool<RTLIL::SigBit> ফলাফল;
		ফেরত get_inputs(ফলাফল, sig);
	}

	অন্তঃরেখ bool has_outputs(RTLIL::SigSpec sig) ধ্রুবক {
		pool<RTLIL::SigBit> ফলাফল;
		ফেরত get_outputs(ফলাফল, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
