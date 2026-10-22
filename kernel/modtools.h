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

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "kernel/sigtools.h"
#അന്തര്ഭാവ "kernel/celltypes.h"
#അന്തര്ഭാവ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ഘടന ModIndex : പൊതു RTLIL::Monitor
{
	ഘടന PointerOrderedSigBit : പൊതു RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			എങ്കിൽ (wire)
				offset = s.offset;
			അല്ലെങ്കിൽ
				data = s.data;
		}
		വരിയിൽ bool operator<(സ്ഥിരം RTLIL::SigBit &other) സ്ഥിരം {
			എങ്കിൽ (wire == other.wire)
				തിരികെ wire ? (offset < other.offset) : (data < other.data);
			എങ്കിൽ (wire != nullptr && other.wire != nullptr)
				തിരികെ wire < other.wire; // look here
			തിരികെ (wire != nullptr) < (other.wire != nullptr);
		}
	};
	ഘടന PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		പൂർണ്ണസംഖ്യ offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, പൂർണ്ണസംഖ്യ _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(സ്ഥിരം PortInfo &other) സ്ഥിരം {
			എങ്കിൽ (cell != other.cell)
				തിരികെ cell < other.cell;
			എങ്കിൽ (offset != other.offset)
				തിരികെ offset < other.offset;
			തിരികെ port < other.port;
		}

		bool operator==(സ്ഥിരം PortInfo &other) സ്ഥിരം {
			തിരികെ cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			തിരികെ h;
		}
	};

	ഘടന SigBitInfo
	{
		bool is_നിവേശം, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_നിവേശം(false), is_output(false) { }

		bool operator==(സ്ഥിരം SigBitInfo &other) സ്ഥിരം {
			തിരികെ is_നിവേശം == other.is_നിവേശം && is_output == other.is_output && ports == other.ports;
		}

		ശൂന്യം merge(സ്ഥിരം SigBitInfo &other)
		{
			is_നിവേശം = is_നിവേശം || other.is_നിവേശം;
			is_output = is_output || other.is_output;
			ports.തിരുകുക(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	പൂർണ്ണസംഖ്യ auto_reload_counter;
	bool auto_reload_module;

	ശൂന്യം port_add(RTLIL::Cell *cell, RTLIL::IdString port, സ്ഥിരം RTLIL::SigSpec &sig)
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			എങ്കിൽ (bit.wire)
				database[bit].ports.തിരുകുക(PortInfo(cell, port, i));
		}
	}

	ശൂന്യം port_del(RTLIL::Cell *cell, RTLIL::IdString port, സ്ഥിരം RTLIL::SigSpec &sig)
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			എങ്കിൽ (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				എങ്കിൽ (!database[bit].is_നിവേശം && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	സ്ഥിരം SigBitInfo &info(RTLIL::SigBit bit)
	{
		തിരികെ database[sigmap(bit)];
	}

	ശൂന്യം reload_module(bool reset_sigmap = true)
	{
		എങ്കിൽ (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		വേണ്ടി (auto wire : module->wires())
			എങ്കിൽ (wire->port_നിവേശം || wire->port_output)
				വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					എങ്കിൽ (bit.wire && wire->port_നിവേശം)
						database[bit].is_നിവേശം = true;
					എങ്കിൽ (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		വേണ്ടി (auto cell : module->cells())
			വേണ്ടി (auto &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		എങ്കിൽ (auto_reload_module) {
			എങ്കിൽ (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef NDEBUG
		എങ്കിൽ (auto_reload_module)
			തിരികെ true;

		വേണ്ടി (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		എങ്കിൽ (!(database == database_bak))
		{
			വേണ്ടി (auto &it : database_bak)
				എങ്കിൽ (!database.എണ്ണം(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			വേണ്ടി (auto &it : database)
				എങ്കിൽ (!database_bak.എണ്ണം(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				അല്ലെങ്കിൽ എങ്കിൽ (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			തിരികെ false;
		}
#endif
		തിരികെ true;
	}

	ശൂന്യം check()
	{
		log_assert(ok());
	}

	ശൂന്യം notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, സ്ഥിരം RTLIL::SigSpec &old_sig, സ്ഥിരം RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		എങ്കിൽ (auto_reload_module)
			തിരികെ;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	ശൂന്യം notify_connect(RTLIL::Module *mod, സ്ഥിരം RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		എങ്കിൽ (auto_reload_module)
			തിരികെ;

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.എണ്ണം(lhs) != 0;
			bool has_rhs = database.എണ്ണം(rhs) != 0;

			എങ്കിൽ (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} അല്ലെങ്കിൽ
			എങ്കിൽ (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				എങ്കിൽ (lhs.wire)
					database[lhs] = new_info;
			} അല്ലെങ്കിൽ
			എങ്കിൽ (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				എങ്കിൽ (rhs.wire)
					database[rhs] = new_info;
			} അല്ലെങ്കിൽ {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				എങ്കിൽ (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	ശൂന്യം notify_connect(RTLIL::Module *mod, സ്ഥിരം std::vector<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ശൂന്യം notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ModIndex(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.തിരുകുക(this);
	}

	~ModIndex()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		എങ്കിൽ (auto_reload_module)
			reload_module();

		auto it = database.find(sigmap(bit));
		എങ്കിൽ (it == database.end())
			തിരികെ nullptr;
		അല്ലെങ്കിൽ
			തിരികെ &it->second;
	}

	bool query_is_നിവേശം(RTLIL::SigBit bit)
	{
		സ്ഥിരം SigBitInfo *info = query(bit);
		എങ്കിൽ (info == nullptr)
			തിരികെ false;
		തിരികെ info->is_നിവേശം;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		സ്ഥിരം SigBitInfo *info = query(bit);
		എങ്കിൽ (info == nullptr)
			തിരികെ false;
		തിരികെ info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		സ്ഥിതം pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		എങ്കിൽ (info == nullptr)
			തിരികെ empty_result_set;
		തിരികെ info->ports;
	}

	ശൂന്യം dump_db()
	{
		log("--- ModIndex Dump ---\n");

		എങ്കിൽ (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		വേണ്ടി (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			എങ്കിൽ (it.second.is_നിവേശം)
				log("  PRIMARY INPUT\n");
			എങ്കിൽ (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			വേണ്ടി (auto &port : it.second.ports)
				log("  PORT: %s.%s[%d] (%s)\n", port.cell,
						PooledName(module, port.port).unescape(), port.offset, port.cell->type.unescape());
		}
	}
};

ഘടന ModWalker
{
	ഘടന PortBit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString port;
		പൂർണ്ണസംഖ്യ offset;
		PortBit(Cell* c, IdString p, പൂർണ്ണസംഖ്യ o) : cell(c), port(p), offset(o) {}

		bool operator<(സ്ഥിരം PortBit &other) സ്ഥിരം {
			എങ്കിൽ (cell != other.cell)
				തിരികെ cell < other.cell;
			എങ്കിൽ (port != other.port)
				തിരികെ port < other.port;
			തിരികെ offset < other.offset;
		}

		bool operator==(സ്ഥിരം PortBit &other) സ്ഥിരം {
			തിരികെ cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			തിരികെ h;
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

	ശൂന്യം add_wire(RTLIL::Wire *wire)
	{
		എങ്കിൽ (wire->port_നിവേശം) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			വേണ്ടി (auto bit : bits)
				എങ്കിൽ (bit.wire != ശൂന്യ)
					signal_inputs.തിരുകുക(bit);
		}

		എങ്കിൽ (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			വേണ്ടി (auto bit : bits)
				എങ്കിൽ (bit.wire != ശൂന്യ)
					signal_outputs.തിരുകുക(bit);
		}
	}

	ശൂന്യം add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_നിവേശം)
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < പൂർണ്ണസംഖ്യ(bits.size()); i++)
			എങ്കിൽ (bits[i].wire != ശൂന്യ) {
				PortBit pbit {cell, port, i};
				എങ്കിൽ (is_output) {
					signal_drivers[bits[i]].തിരുകുക(pbit);
					cell_outputs[cell].തിരുകുക(bits[i]);
				}
				എങ്കിൽ (is_നിവേശം) {
					signal_consumers[bits[i]].തിരുകുക(pbit);
					cell_inputs[cell].തിരുകുക(bits[i]);
				}
			}
	}

	ശൂന്യം add_cell(RTLIL::Cell *cell)
	{
		എങ്കിൽ (ct.cell_known(cell->type)) {
			വേണ്ടി (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_നിവേശം(cell->type, conn.first));
		} അല്ലെങ്കിൽ {
			വേണ്ടി (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(ശൂന്യ)
	{
		ct.setup(design);
		എങ്കിൽ (module)
			setup(module);
	}

	ശൂന്യം setup(RTLIL::Module *module, CellTypes *filter_ct = ശൂന്യ)
	{
		this->module = module;

		sigmap.set(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		വേണ്ടി (auto &it : module->wires_)
			add_wire(it.second);
		വേണ്ടി (auto &it : module->cells_)
			എങ്കിൽ (filter_ct == ശൂന്യ || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	വരിയിൽ bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) സ്ഥിരം
	{
		bool found = false;
		എങ്കിൽ (signal_drivers.എണ്ണം(bit)) {
			സ്ഥിരം pool<PortBit> &r = signal_drivers.at(bit);
			result.തിരുകുക(r.begin(), r.end());
			found = true;
		}
		തിരികെ found;
	}

	വരിയിൽ bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) സ്ഥിരം
	{
		bool found = false;
		എങ്കിൽ (signal_consumers.എണ്ണം(bit)) {
			സ്ഥിരം pool<PortBit> &r = signal_consumers.at(bit);
			result.തിരുകുക(r.begin(), r.end());
			found = true;
		}
		തിരികെ found;
	}

	വരിയിൽ bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) സ്ഥിരം
	{
		bool found = false;
		എങ്കിൽ (signal_inputs.എണ്ണം(bit))
			result.തിരുകുക(bit), found = true;
		തിരികെ found;
	}

	വരിയിൽ bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) സ്ഥിരം
	{
		bool found = false;
		എങ്കിൽ (signal_outputs.എണ്ണം(bit))
			result.തിരുകുക(bit), found = true;
		തിരികെ found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	വരിയിൽ bool get_drivers(pool<PortBit> &result, സ്ഥിരം T &bits) സ്ഥിരം
	{
		bool found = false;
		വേണ്ടി (RTLIL::SigBit bit : bits)
			എങ്കിൽ (signal_drivers.എണ്ണം(bit)) {
				സ്ഥിരം pool<PortBit> &r = signal_drivers.at(bit);
				result.തിരുകുക(r.begin(), r.end());
				found = true;
			}
		തിരികെ found;
	}

	template<typename T>
	വരിയിൽ bool get_consumers(pool<PortBit> &result, സ്ഥിരം T &bits) സ്ഥിരം
	{
		bool found = false;
		വേണ്ടി (RTLIL::SigBit bit : bits)
			എങ്കിൽ (signal_consumers.എണ്ണം(bit)) {
				സ്ഥിരം pool<PortBit> &r = signal_consumers.at(bit);
				result.തിരുകുക(r.begin(), r.end());
				found = true;
			}
		തിരികെ found;
	}

	template<typename T>
	വരിയിൽ bool get_inputs(pool<RTLIL::SigBit> &result, സ്ഥിരം T &bits) സ്ഥിരം
	{
		bool found = false;
		വേണ്ടി (RTLIL::SigBit bit : bits)
			എങ്കിൽ (signal_inputs.എണ്ണം(bit))
				result.തിരുകുക(bit), found = true;
		തിരികെ found;
	}

	template<typename T>
	വരിയിൽ bool get_outputs(pool<RTLIL::SigBit> &result, സ്ഥിരം T &bits) സ്ഥിരം
	{
		bool found = false;
		വേണ്ടി (RTLIL::SigBit bit : bits)
			എങ്കിൽ (signal_outputs.എണ്ണം(bit))
				result.തിരുകുക(bit), found = true;
		തിരികെ found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec സൂചന) സ്ഥിരം
	{
		std::vector<RTLIL::SigBit> bits = sigmap(സൂചന);
		തിരികെ get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec സൂചന) സ്ഥിരം
	{
		std::vector<RTLIL::SigBit> bits = sigmap(സൂചന);
		തിരികെ get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec സൂചന) സ്ഥിരം
	{
		std::vector<RTLIL::SigBit> bits = sigmap(സൂചന);
		തിരികെ get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec സൂചന) സ്ഥിരം
	{
		std::vector<RTLIL::SigBit> bits = sigmap(സൂചന);
		തിരികെ get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	വരിയിൽ bool has_drivers(സ്ഥിരം T &sig) സ്ഥിരം {
		pool<PortBit> result;
		തിരികെ get_drivers(result, sig);
	}

	template<typename T>
	വരിയിൽ bool has_consumers(സ്ഥിരം T &sig) സ്ഥിരം {
		pool<PortBit> result;
		തിരികെ get_consumers(result, sig);
	}

	template<typename T>
	വരിയിൽ bool has_inputs(സ്ഥിരം T &sig) സ്ഥിരം {
		pool<RTLIL::SigBit> result;
		തിരികെ get_inputs(result, sig);
	}

	template<typename T>
	വരിയിൽ bool has_outputs(സ്ഥിരം T &sig) സ്ഥിരം {
		pool<RTLIL::SigBit> result;
		തിരികെ get_outputs(result, sig);
	}

	// has_* methods -- call by value

	വരിയിൽ bool has_drivers(RTLIL::SigSpec sig) സ്ഥിരം {
		pool<PortBit> result;
		തിരികെ get_drivers(result, sig);
	}

	വരിയിൽ bool has_consumers(RTLIL::SigSpec sig) സ്ഥിരം {
		pool<PortBit> result;
		തിരികെ get_consumers(result, sig);
	}

	വരിയിൽ bool has_inputs(RTLIL::SigSpec sig) സ്ഥിരം {
		pool<RTLIL::SigBit> result;
		തിരികെ get_inputs(result, sig);
	}

	വരിയിൽ bool has_outputs(RTLIL::SigSpec sig) സ്ഥിരം {
		pool<RTLIL::SigBit> result;
		തിരികെ get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
