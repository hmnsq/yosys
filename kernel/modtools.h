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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/celltypes.h"
#ಅನ್ತರ್ಭಾವ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ಸಂರಚನಾ ModIndex : ಸಾರ್ವಜನಿಕ RTLIL::Monitor
{
	ಸಂರಚನಾ PointerOrderedSigBit : ಸಾರ್ವಜನಿಕ RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			ಯದಿ (wire)
				offset = s.offset;
			ಅನ್ಯಥಾ
				data = s.data;
		}
		ಅನ್ತಃಸ್ಥ bool operator<(ಧ್ರುವ RTLIL::SigBit &other) ಧ್ರುವ {
			ಯದಿ (wire == other.wire)
				ಪ್ರತ್ಯಾಗಮ wire ? (offset < other.offset) : (data < other.data);
			ಯದಿ (wire != nullptr && other.wire != nullptr)
				ಪ್ರತ್ಯಾಗಮ wire < other.wire; // look here
			ಪ್ರತ್ಯಾಗಮ (wire != nullptr) < (other.wire != nullptr);
		}
	};
	ಸಂರಚನಾ PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		ಪೂರ್ಣಾಙ್ಕ offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, ಪೂರ್ಣಾಙ್ಕ _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(ಧ್ರುವ PortInfo &other) ಧ್ರುವ {
			ಯದಿ (cell != other.cell)
				ಪ್ರತ್ಯಾಗಮ cell < other.cell;
			ಯದಿ (offset != other.offset)
				ಪ್ರತ್ಯಾಗಮ offset < other.offset;
			ಪ್ರತ್ಯಾಗಮ port < other.port;
		}

		bool operator==(ಧ್ರುವ PortInfo &other) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			ಪ್ರತ್ಯಾಗಮ h;
		}
	};

	ಸಂರಚನಾ SigBitInfo
	{
		bool is_ಒಳಹರಿವು, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_ಒಳಹರಿವು(false), is_output(false) { }

		bool operator==(ಧ್ರುವ SigBitInfo &other) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ is_ಒಳಹರಿವು == other.is_ಒಳಹರಿವು && is_output == other.is_output && ports == other.ports;
		}

		ಶೂನ್ಯ merge(ಧ್ರುವ SigBitInfo &other)
		{
			is_ಒಳಹರಿವು = is_ಒಳಹರಿವು || other.is_ಒಳಹರಿವು;
			is_output = is_output || other.is_output;
			ports.ಸೇರಿಸು(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	ಪೂರ್ಣಾಙ್ಕ auto_reload_counter;
	bool auto_reload_module;

	ಶೂನ್ಯ port_add(RTLIL::Cell *cell, RTLIL::IdString port, ಧ್ರುವ RTLIL::SigSpec &sig)
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			ಯದಿ (bit.wire)
				database[bit].ports.ಸೇರಿಸು(PortInfo(cell, port, i));
		}
	}

	ಶೂನ್ಯ port_del(RTLIL::Cell *cell, RTLIL::IdString port, ಧ್ರುವ RTLIL::SigSpec &sig)
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			ಯದಿ (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				ಯದಿ (!database[bit].is_ಒಳಹರಿವು && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	ಧ್ರುವ SigBitInfo &info(RTLIL::SigBit bit)
	{
		ಪ್ರತ್ಯಾಗಮ database[sigmap(bit)];
	}

	ಶೂನ್ಯ reload_module(bool reset_sigmap = true)
	{
		ಯದಿ (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		ಪ್ರತಿ (auto wire : module->wires())
			ಯದಿ (wire->port_ಒಳಹರಿವು || wire->port_output)
				ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					ಯದಿ (bit.wire && wire->port_ಒಳಹರಿವು)
						database[bit].is_ಒಳಹರಿವು = true;
					ಯದಿ (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		ಪ್ರತಿ (auto cell : module->cells())
			ಪ್ರತಿ (auto &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		ಯದಿ (auto_reload_module) {
			ಯದಿ (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef NDEBUG
		ಯದಿ (auto_reload_module)
			ಪ್ರತ್ಯಾಗಮ true;

		ಪ್ರತಿ (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		ಯದಿ (!(database == database_bak))
		{
			ಪ್ರತಿ (auto &it : database_bak)
				ಯದಿ (!database.ಎಣಿಕೆ(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			ಪ್ರತಿ (auto &it : database)
				ಯದಿ (!database_bak.ಎಣಿಕೆ(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				ಅನ್ಯಥಾ ಯದಿ (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			ಪ್ರತ್ಯಾಗಮ false;
		}
#endif
		ಪ್ರತ್ಯಾಗಮ true;
	}

	ಶೂನ್ಯ check()
	{
		log_assert(ok());
	}

	ಶೂನ್ಯ notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, ಧ್ರುವ RTLIL::SigSpec &old_sig, ಧ್ರುವ RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		ಯದಿ (auto_reload_module)
			ಪ್ರತ್ಯಾಗಮ;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	ಶೂನ್ಯ notify_connect(RTLIL::Module *mod, ಧ್ರುವ RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		ಯದಿ (auto_reload_module)
			ಪ್ರತ್ಯಾಗಮ;

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.ಎಣಿಕೆ(lhs) != 0;
			bool has_rhs = database.ಎಣಿಕೆ(rhs) != 0;

			ಯದಿ (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} ಅನ್ಯಥಾ
			ಯದಿ (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				ಯದಿ (lhs.wire)
					database[lhs] = new_info;
			} ಅನ್ಯಥಾ
			ಯದಿ (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				ಯದಿ (rhs.wire)
					database[rhs] = new_info;
			} ಅನ್ಯಥಾ {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				ಯದಿ (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	ಶೂನ್ಯ notify_connect(RTLIL::Module *mod, ಧ್ರುವ std::vector<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ಶೂನ್ಯ notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ModIndex(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.ಸೇರಿಸು(this);
	}

	~ModIndex()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		ಯದಿ (auto_reload_module)
			reload_module();

		auto it = database.find(sigmap(bit));
		ಯದಿ (it == database.end())
			ಪ್ರತ್ಯಾಗಮ nullptr;
		ಅನ್ಯಥಾ
			ಪ್ರತ್ಯಾಗಮ &it->second;
	}

	bool query_is_ಒಳಹರಿವು(RTLIL::SigBit bit)
	{
		ಧ್ರುವ SigBitInfo *info = query(bit);
		ಯದಿ (info == nullptr)
			ಪ್ರತ್ಯಾಗಮ false;
		ಪ್ರತ್ಯಾಗಮ info->is_ಒಳಹರಿವು;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		ಧ್ರುವ SigBitInfo *info = query(bit);
		ಯದಿ (info == nullptr)
			ಪ್ರತ್ಯಾಗಮ false;
		ಪ್ರತ್ಯಾಗಮ info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		ಸ್ಥಿರ pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		ಯದಿ (info == nullptr)
			ಪ್ರತ್ಯಾಗಮ empty_result_set;
		ಪ್ರತ್ಯಾಗಮ info->ports;
	}

	ಶೂನ್ಯ dump_db()
	{
		log("--- ModIndex Dump ---\n");

		ಯದಿ (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		ಪ್ರತಿ (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			ಯದಿ (it.second.is_ಒಳಹರಿವು)
				log("  PRIMARY INPUT\n");
			ಯದಿ (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			ಪ್ರತಿ (auto &port : it.second.ports)
				log("  PORT: %s.%s[%d] (%s)\n", port.cell,
						PooledName(module, port.port).unescape(), port.offset, port.cell->type.unescape());
		}
	}
};

ಸಂರಚನಾ ModWalker
{
	ಸಂರಚನಾ PortBit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString port;
		ಪೂರ್ಣಾಙ್ಕ offset;
		PortBit(Cell* c, IdString p, ಪೂರ್ಣಾಙ್ಕ o) : cell(c), port(p), offset(o) {}

		bool operator<(ಧ್ರುವ PortBit &other) ಧ್ರುವ {
			ಯದಿ (cell != other.cell)
				ಪ್ರತ್ಯಾಗಮ cell < other.cell;
			ಯದಿ (port != other.port)
				ಪ್ರತ್ಯಾಗಮ port < other.port;
			ಪ್ರತ್ಯಾಗಮ offset < other.offset;
		}

		bool operator==(ಧ್ರುವ PortBit &other) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			ಪ್ರತ್ಯಾಗಮ h;
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

	ಶೂನ್ಯ add_wire(RTLIL::Wire *wire)
	{
		ಯದಿ (wire->port_ಒಳಹರಿವು) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			ಪ್ರತಿ (auto bit : bits)
				ಯದಿ (bit.wire != ರಿಕ್ತ)
					signal_inputs.ಸೇರಿಸು(bit);
		}

		ಯದಿ (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			ಪ್ರತಿ (auto bit : bits)
				ಯದಿ (bit.wire != ರಿಕ್ತ)
					signal_outputs.ಸೇರಿಸು(bit);
		}
	}

	ಶೂನ್ಯ add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_ಒಳಹರಿವು)
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಪೂರ್ಣಾಙ್ಕ(bits.size()); i++)
			ಯದಿ (bits[i].wire != ರಿಕ್ತ) {
				PortBit pbit {cell, port, i};
				ಯದಿ (is_output) {
					signal_drivers[bits[i]].ಸೇರಿಸು(pbit);
					cell_outputs[cell].ಸೇರಿಸು(bits[i]);
				}
				ಯದಿ (is_ಒಳಹರಿವು) {
					signal_consumers[bits[i]].ಸೇರಿಸು(pbit);
					cell_inputs[cell].ಸೇರಿಸು(bits[i]);
				}
			}
	}

	ಶೂನ್ಯ add_cell(RTLIL::Cell *cell)
	{
		ಯದಿ (ct.cell_known(cell->type)) {
			ಪ್ರತಿ (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_ಒಳಹರಿವು(cell->type, conn.first));
		} ಅನ್ಯಥಾ {
			ಪ್ರತಿ (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(ರಿಕ್ತ)
	{
		ct.setup(design);
		ಯದಿ (module)
			setup(module);
	}

	ಶೂನ್ಯ setup(RTLIL::Module *module, CellTypes *filter_ct = ರಿಕ್ತ)
	{
		this->module = module;

		sigmap.set(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		ಪ್ರತಿ (auto &it : module->wires_)
			add_wire(it.second);
		ಪ್ರತಿ (auto &it : module->cells_)
			ಯದಿ (filter_ct == ರಿಕ್ತ || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	ಅನ್ತಃಸ್ಥ bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) ಧ್ರುವ
	{
		bool found = false;
		ಯದಿ (signal_drivers.ಎಣಿಕೆ(bit)) {
			ಧ್ರುವ pool<PortBit> &r = signal_drivers.at(bit);
			result.ಸೇರಿಸು(r.begin(), r.end());
			found = true;
		}
		ಪ್ರತ್ಯಾಗಮ found;
	}

	ಅನ್ತಃಸ್ಥ bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) ಧ್ರುವ
	{
		bool found = false;
		ಯದಿ (signal_consumers.ಎಣಿಕೆ(bit)) {
			ಧ್ರುವ pool<PortBit> &r = signal_consumers.at(bit);
			result.ಸೇರಿಸು(r.begin(), r.end());
			found = true;
		}
		ಪ್ರತ್ಯಾಗಮ found;
	}

	ಅನ್ತಃಸ್ಥ bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ಧ್ರುವ
	{
		bool found = false;
		ಯದಿ (signal_inputs.ಎಣಿಕೆ(bit))
			result.ಸೇರಿಸು(bit), found = true;
		ಪ್ರತ್ಯಾಗಮ found;
	}

	ಅನ್ತಃಸ್ಥ bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ಧ್ರುವ
	{
		bool found = false;
		ಯದಿ (signal_outputs.ಎಣಿಕೆ(bit))
			result.ಸೇರಿಸು(bit), found = true;
		ಪ್ರತ್ಯಾಗಮ found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	ಅನ್ತಃಸ್ಥ bool get_drivers(pool<PortBit> &result, ಧ್ರುವ T &bits) ಧ್ರುವ
	{
		bool found = false;
		ಪ್ರತಿ (RTLIL::SigBit bit : bits)
			ಯದಿ (signal_drivers.ಎಣಿಕೆ(bit)) {
				ಧ್ರುವ pool<PortBit> &r = signal_drivers.at(bit);
				result.ಸೇರಿಸು(r.begin(), r.end());
				found = true;
			}
		ಪ್ರತ್ಯಾಗಮ found;
	}

	template<typename T>
	ಅನ್ತಃಸ್ಥ bool get_consumers(pool<PortBit> &result, ಧ್ರುವ T &bits) ಧ್ರುವ
	{
		bool found = false;
		ಪ್ರತಿ (RTLIL::SigBit bit : bits)
			ಯದಿ (signal_consumers.ಎಣಿಕೆ(bit)) {
				ಧ್ರುವ pool<PortBit> &r = signal_consumers.at(bit);
				result.ಸೇರಿಸು(r.begin(), r.end());
				found = true;
			}
		ಪ್ರತ್ಯಾಗಮ found;
	}

	template<typename T>
	ಅನ್ತಃಸ್ಥ bool get_inputs(pool<RTLIL::SigBit> &result, ಧ್ರುವ T &bits) ಧ್ರುವ
	{
		bool found = false;
		ಪ್ರತಿ (RTLIL::SigBit bit : bits)
			ಯದಿ (signal_inputs.ಎಣಿಕೆ(bit))
				result.ಸೇರಿಸು(bit), found = true;
		ಪ್ರತ್ಯಾಗಮ found;
	}

	template<typename T>
	ಅನ್ತಃಸ್ಥ bool get_outputs(pool<RTLIL::SigBit> &result, ಧ್ರುವ T &bits) ಧ್ರುವ
	{
		bool found = false;
		ಪ್ರತಿ (RTLIL::SigBit bit : bits)
			ಯದಿ (signal_outputs.ಎಣಿಕೆ(bit))
				result.ಸೇರಿಸು(bit), found = true;
		ಪ್ರತ್ಯಾಗಮ found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec ಸಙ್ಕೇತ) ಧ್ರುವ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(ಸಙ್ಕೇತ);
		ಪ್ರತ್ಯಾಗಮ get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec ಸಙ್ಕೇತ) ಧ್ರುವ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(ಸಙ್ಕೇತ);
		ಪ್ರತ್ಯಾಗಮ get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec ಸಙ್ಕೇತ) ಧ್ರುವ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(ಸಙ್ಕೇತ);
		ಪ್ರತ್ಯಾಗಮ get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec ಸಙ್ಕೇತ) ಧ್ರುವ
	{
		std::vector<RTLIL::SigBit> bits = sigmap(ಸಙ್ಕೇತ);
		ಪ್ರತ್ಯಾಗಮ get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	ಅನ್ತಃಸ್ಥ bool has_drivers(ಧ್ರುವ T &sig) ಧ್ರುವ {
		pool<PortBit> result;
		ಪ್ರತ್ಯಾಗಮ get_drivers(result, sig);
	}

	template<typename T>
	ಅನ್ತಃಸ್ಥ bool has_consumers(ಧ್ರುವ T &sig) ಧ್ರುವ {
		pool<PortBit> result;
		ಪ್ರತ್ಯಾಗಮ get_consumers(result, sig);
	}

	template<typename T>
	ಅನ್ತಃಸ್ಥ bool has_inputs(ಧ್ರುವ T &sig) ಧ್ರುವ {
		pool<RTLIL::SigBit> result;
		ಪ್ರತ್ಯಾಗಮ get_inputs(result, sig);
	}

	template<typename T>
	ಅನ್ತಃಸ್ಥ bool has_outputs(ಧ್ರುವ T &sig) ಧ್ರುವ {
		pool<RTLIL::SigBit> result;
		ಪ್ರತ್ಯಾಗಮ get_outputs(result, sig);
	}

	// has_* methods -- call by value

	ಅನ್ತಃಸ್ಥ bool has_drivers(RTLIL::SigSpec sig) ಧ್ರುವ {
		pool<PortBit> result;
		ಪ್ರತ್ಯಾಗಮ get_drivers(result, sig);
	}

	ಅನ್ತಃಸ್ಥ bool has_consumers(RTLIL::SigSpec sig) ಧ್ರುವ {
		pool<PortBit> result;
		ಪ್ರತ್ಯಾಗಮ get_consumers(result, sig);
	}

	ಅನ್ತಃಸ್ಥ bool has_inputs(RTLIL::SigSpec sig) ಧ್ರುವ {
		pool<RTLIL::SigBit> result;
		ಪ್ರತ್ಯಾಗಮ get_inputs(result, sig);
	}

	ಅನ್ತಃಸ್ಥ bool has_outputs(RTLIL::SigSpec sig) ಧ್ರುವ {
		pool<RTLIL::SigBit> result;
		ಪ್ರತ್ಯಾಗಮ get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
