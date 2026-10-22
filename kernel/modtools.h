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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/sigtools.h"
#અન્તર્ભાવ "kernel/celltypes.h"
#અન્તર્ભાવ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

સંરચના ModIndex : જાહેર RTLIL::Monitor
{
	સંરચના PointerOrderedSigBit : જાહેર RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			જો (wire)
				offset = s.offset;
			નહીંતર
				data = s.data;
		}
		અંતઃરેખ bool operator<(અચલ RTLIL::SigBit &other) અચલ {
			જો (wire == other.wire)
				પાછા wire ? (offset < other.offset) : (data < other.data);
			જો (wire != nullptr && other.wire != nullptr)
				પાછા wire < other.wire; // look here
			પાછા (wire != nullptr) < (other.wire != nullptr);
		}
	};
	સંરચના PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		પૂર્ણાંક offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, પૂર્ણાંક _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(અચલ PortInfo &other) અચલ {
			જો (cell != other.cell)
				પાછા cell < other.cell;
			જો (offset != other.offset)
				પાછા offset < other.offset;
			પાછા port < other.port;
		}

		bool operator==(અચલ PortInfo &other) અચલ {
			પાછા cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			પાછા h;
		}
	};

	સંરચના SigBitInfo
	{
		bool is_આગત, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_આગત(false), is_output(false) { }

		bool operator==(અચલ SigBitInfo &other) અચલ {
			પાછા is_આગત == other.is_આગત && is_output == other.is_output && ports == other.ports;
		}

		શૂન્ય merge(અચલ SigBitInfo &other)
		{
			is_આગત = is_આગત || other.is_આગત;
			is_output = is_output || other.is_output;
			ports.ઉમેરો(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	પૂર્ણાંક auto_reload_counter;
	bool auto_reload_module;

	શૂન્ય port_add(RTLIL::Cell *cell, RTLIL::IdString port, અચલ RTLIL::SigSpec &sig)
	{
		માટે (પૂર્ણાંક i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			જો (bit.wire)
				database[bit].ports.ઉમેરો(PortInfo(cell, port, i));
		}
	}

	શૂન્ય port_del(RTLIL::Cell *cell, RTLIL::IdString port, અચલ RTLIL::SigSpec &sig)
	{
		માટે (પૂર્ણાંક i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			જો (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				જો (!database[bit].is_આગત && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	અચલ SigBitInfo &info(RTLIL::SigBit bit)
	{
		પાછા database[sigmap(bit)];
	}

	શૂન્ય reload_module(bool reset_sigmap = true)
	{
		જો (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		માટે (auto wire : module->wires())
			જો (wire->port_આગત || wire->port_output)
				માટે (પૂર્ણાંક i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					જો (bit.wire && wire->port_આગત)
						database[bit].is_આગત = true;
					જો (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		માટે (auto cell : module->cells())
			માટે (auto &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		જો (auto_reload_module) {
			જો (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef NDEBUG
		જો (auto_reload_module)
			પાછા true;

		માટે (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		જો (!(database == database_bak))
		{
			માટે (auto &it : database_bak)
				જો (!database.ગણતરી(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			માટે (auto &it : database)
				જો (!database_bak.ગણતરી(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				નહીંતર જો (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			પાછા false;
		}
#endif
		પાછા true;
	}

	શૂન્ય check()
	{
		log_assert(ok());
	}

	શૂન્ય notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, અચલ RTLIL::SigSpec &old_sig, અચલ RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		જો (auto_reload_module)
			પાછા;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	શૂન્ય notify_connect(RTLIL::Module *mod, અચલ RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		જો (auto_reload_module)
			પાછા;

		માટે (પૂર્ણાંક i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.ગણતરી(lhs) != 0;
			bool has_rhs = database.ગણતરી(rhs) != 0;

			જો (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} નહીંતર
			જો (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				જો (lhs.wire)
					database[lhs] = new_info;
			} નહીંતર
			જો (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				જો (rhs.wire)
					database[rhs] = new_info;
			} નહીંતર {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				જો (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	શૂન્ય notify_connect(RTLIL::Module *mod, અચલ std::vector<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	શૂન્ય notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ModIndex(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.ઉમેરો(this);
	}

	~ModIndex()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		જો (auto_reload_module)
			reload_module();

		auto it = database.find(sigmap(bit));
		જો (it == database.end())
			પાછા nullptr;
		નહીંતર
			પાછા &it->second;
	}

	bool query_is_આગત(RTLIL::SigBit bit)
	{
		અચલ SigBitInfo *info = query(bit);
		જો (info == nullptr)
			પાછા false;
		પાછા info->is_આગત;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		અચલ SigBitInfo *info = query(bit);
		જો (info == nullptr)
			પાછા false;
		પાછા info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		સ્થિર pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		જો (info == nullptr)
			પાછા empty_result_set;
		પાછા info->ports;
	}

	શૂન્ય dump_db()
	{
		log("--- ModIndex Dump ---\n");

		જો (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		માટે (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			જો (it.second.is_આગત)
				log("  PRIMARY INPUT\n");
			જો (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			માટે (auto &port : it.second.ports)
				log("  PORT: %s.%s[%d] (%s)\n", port.cell,
						PooledName(module, port.port).unescape(), port.offset, port.cell->type.unescape());
		}
	}
};

સંરચના ModWalker
{
	સંરચના PortBit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString port;
		પૂર્ણાંક offset;
		PortBit(Cell* c, IdString p, પૂર્ણાંક o) : cell(c), port(p), offset(o) {}

		bool operator<(અચલ PortBit &other) અચલ {
			જો (cell != other.cell)
				પાછા cell < other.cell;
			જો (port != other.port)
				પાછા port < other.port;
			પાછા offset < other.offset;
		}

		bool operator==(અચલ PortBit &other) અચલ {
			પાછા cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			પાછા h;
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

	શૂન્ય add_wire(RTLIL::Wire *wire)
	{
		જો (wire->port_આગત) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			માટે (auto bit : bits)
				જો (bit.wire != ખાલી)
					signal_inputs.ઉમેરો(bit);
		}

		જો (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			માટે (auto bit : bits)
				જો (bit.wire != ખાલી)
					signal_outputs.ઉમેરો(bit);
		}
	}

	શૂન્ય add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_આગત)
	{
		માટે (પૂર્ણાંક i = 0; i < પૂર્ણાંક(bits.size()); i++)
			જો (bits[i].wire != ખાલી) {
				PortBit pbit {cell, port, i};
				જો (is_output) {
					signal_drivers[bits[i]].ઉમેરો(pbit);
					cell_outputs[cell].ઉમેરો(bits[i]);
				}
				જો (is_આગત) {
					signal_consumers[bits[i]].ઉમેરો(pbit);
					cell_inputs[cell].ઉમેરો(bits[i]);
				}
			}
	}

	શૂન્ય add_cell(RTLIL::Cell *cell)
	{
		જો (ct.cell_known(cell->type)) {
			માટે (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_આગત(cell->type, conn.first));
		} નહીંતર {
			માટે (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(ખાલી)
	{
		ct.setup(design);
		જો (module)
			setup(module);
	}

	શૂન્ય setup(RTLIL::Module *module, CellTypes *filter_ct = ખાલી)
	{
		this->module = module;

		sigmap.set(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		માટે (auto &it : module->wires_)
			add_wire(it.second);
		માટે (auto &it : module->cells_)
			જો (filter_ct == ખાલી || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	અંતઃરેખ bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) અચલ
	{
		bool found = false;
		જો (signal_drivers.ગણતરી(bit)) {
			અચલ pool<PortBit> &r = signal_drivers.at(bit);
			result.ઉમેરો(r.begin(), r.end());
			found = true;
		}
		પાછા found;
	}

	અંતઃરેખ bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) અચલ
	{
		bool found = false;
		જો (signal_consumers.ગણતરી(bit)) {
			અચલ pool<PortBit> &r = signal_consumers.at(bit);
			result.ઉમેરો(r.begin(), r.end());
			found = true;
		}
		પાછા found;
	}

	અંતઃરેખ bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) અચલ
	{
		bool found = false;
		જો (signal_inputs.ગણતરી(bit))
			result.ઉમેરો(bit), found = true;
		પાછા found;
	}

	અંતઃરેખ bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) અચલ
	{
		bool found = false;
		જો (signal_outputs.ગણતરી(bit))
			result.ઉમેરો(bit), found = true;
		પાછા found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	અંતઃરેખ bool get_drivers(pool<PortBit> &result, અચલ T &bits) અચલ
	{
		bool found = false;
		માટે (RTLIL::SigBit bit : bits)
			જો (signal_drivers.ગણતરી(bit)) {
				અચલ pool<PortBit> &r = signal_drivers.at(bit);
				result.ઉમેરો(r.begin(), r.end());
				found = true;
			}
		પાછા found;
	}

	template<typename T>
	અંતઃરેખ bool get_consumers(pool<PortBit> &result, અચલ T &bits) અચલ
	{
		bool found = false;
		માટે (RTLIL::SigBit bit : bits)
			જો (signal_consumers.ગણતરી(bit)) {
				અચલ pool<PortBit> &r = signal_consumers.at(bit);
				result.ઉમેરો(r.begin(), r.end());
				found = true;
			}
		પાછા found;
	}

	template<typename T>
	અંતઃરેખ bool get_inputs(pool<RTLIL::SigBit> &result, અચલ T &bits) અચલ
	{
		bool found = false;
		માટે (RTLIL::SigBit bit : bits)
			જો (signal_inputs.ગણતરી(bit))
				result.ઉમેરો(bit), found = true;
		પાછા found;
	}

	template<typename T>
	અંતઃરેખ bool get_outputs(pool<RTLIL::SigBit> &result, અચલ T &bits) અચલ
	{
		bool found = false;
		માટે (RTLIL::SigBit bit : bits)
			જો (signal_outputs.ગણતરી(bit))
				result.ઉમેરો(bit), found = true;
		પાછા found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec સંકેત) અચલ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(સંકેત);
		પાછા get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec સંકેત) અચલ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(સંકેત);
		પાછા get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec સંકેત) અચલ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(સંકેત);
		પાછા get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec સંકેત) અચલ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(સંકેત);
		પાછા get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	અંતઃરેખ bool has_drivers(અચલ T &sig) અચલ {
		pool<PortBit> result;
		પાછા get_drivers(result, sig);
	}

	template<typename T>
	અંતઃરેખ bool has_consumers(અચલ T &sig) અચલ {
		pool<PortBit> result;
		પાછા get_consumers(result, sig);
	}

	template<typename T>
	અંતઃરેખ bool has_inputs(અચલ T &sig) અચલ {
		pool<RTLIL::SigBit> result;
		પાછા get_inputs(result, sig);
	}

	template<typename T>
	અંતઃરેખ bool has_outputs(અચલ T &sig) અચલ {
		pool<RTLIL::SigBit> result;
		પાછા get_outputs(result, sig);
	}

	// has_* methods -- call by value

	અંતઃરેખ bool has_drivers(RTLIL::SigSpec sig) અચલ {
		pool<PortBit> result;
		પાછા get_drivers(result, sig);
	}

	અંતઃરેખ bool has_consumers(RTLIL::SigSpec sig) અચલ {
		pool<PortBit> result;
		પાછા get_consumers(result, sig);
	}

	અંતઃરેખ bool has_inputs(RTLIL::SigSpec sig) અચલ {
		pool<RTLIL::SigBit> result;
		પાછા get_inputs(result, sig);
	}

	અંતઃરેખ bool has_outputs(RTLIL::SigSpec sig) અચલ {
		pool<RTLIL::SigBit> result;
		પાછા get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
