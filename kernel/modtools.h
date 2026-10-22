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

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/celltypes.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

بنية ModIndex : عام RTLIL::Monitor
{
	بنية PointerOrderedSigBit : عام RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			إذا (wire)
				offset = s.offset;
			وإلا
				data = s.data;
		}
		مضمن bool operator<(ثابت RTLIL::SigBit &other) ثابت {
			إذا (wire == other.wire)
				أرجع wire ? (offset < other.offset) : (data < other.data);
			إذا (wire != nullptr && other.wire != nullptr)
				أرجع wire < other.wire; // look here
			أرجع (wire != nullptr) < (other.wire != nullptr);
		}
	};
	بنية PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		صحيح offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, صحيح _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(ثابت PortInfo &other) ثابت {
			إذا (cell != other.cell)
				أرجع cell < other.cell;
			إذا (offset != other.offset)
				أرجع offset < other.offset;
			أرجع port < other.port;
		}

		bool operator==(ثابت PortInfo &other) ثابت {
			أرجع cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			أرجع h;
		}
	};

	بنية SigBitInfo
	{
		bool is_ماسوقن, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_ماسوقن(false), is_output(false) { }

		bool operator==(ثابت SigBitInfo &other) ثابت {
			أرجع is_ماسوقن == other.is_ماسوقن && is_output == other.is_output && ports == other.ports;
		}

		فراغ merge(ثابت SigBitInfo &other)
		{
			is_ماسوقن = is_ماسوقن || other.is_ماسوقن;
			is_output = is_output || other.is_output;
			ports.سيسيڤ(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	صحيح auto_reload_counter;
	bool auto_reload_module;

	فراغ port_add(RTLIL::Cell *cell, RTLIL::IdString port, ثابت RTLIL::SigSpec &sig)
	{
		لكل (صحيح i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			إذا (bit.wire)
				database[bit].ports.سيسيڤ(PortInfo(cell, port, i));
		}
	}

	فراغ port_del(RTLIL::Cell *cell, RTLIL::IdString port, ثابت RTLIL::SigSpec &sig)
	{
		لكل (صحيح i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			إذا (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				إذا (!database[bit].is_ماسوقن && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	ثابت SigBitInfo &info(RTLIL::SigBit bit)
	{
		أرجع database[sigmap(bit)];
	}

	فراغ reload_module(bool reset_sigmap = true)
	{
		إذا (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		لكل (auto wire : module->wires())
			إذا (wire->port_ماسوقن || wire->port_output)
				لكل (صحيح i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					إذا (bit.wire && wire->port_ماسوقن)
						database[bit].is_ماسوقن = true;
					إذا (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		لكل (auto cell : module->cells())
			لكل (auto &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		إذا (auto_reload_module) {
			إذا (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef NDEBUG
		إذا (auto_reload_module)
			أرجع true;

		لكل (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		إذا (!(database == database_bak))
		{
			لكل (auto &it : database_bak)
				إذا (!database.کيراءن(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			لكل (auto &it : database)
				إذا (!database_bak.کيراءن(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				وإلا إذا (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			أرجع false;
		}
#endif
		أرجع true;
	}

	فراغ check()
	{
		log_assert(ok());
	}

	فراغ notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, ثابت RTLIL::SigSpec &old_sig, ثابت RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		إذا (auto_reload_module)
			أرجع;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	فراغ notify_connect(RTLIL::Module *mod, ثابت RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		إذا (auto_reload_module)
			أرجع;

		لكل (صحيح i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.کيراءن(lhs) != 0;
			bool has_rhs = database.کيراءن(rhs) != 0;

			إذا (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} وإلا
			إذا (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				إذا (lhs.wire)
					database[lhs] = new_info;
			} وإلا
			إذا (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				إذا (rhs.wire)
					database[rhs] = new_info;
			} وإلا {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				إذا (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	فراغ notify_connect(RTLIL::Module *mod, ثابت std::vector<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	فراغ notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ModIndex(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.سيسيڤ(this);
	}

	~ModIndex()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		إذا (auto_reload_module)
			reload_module();

		auto it = database.find(sigmap(bit));
		إذا (it == database.end())
			أرجع nullptr;
		وإلا
			أرجع &it->second;
	}

	bool query_is_ماسوقن(RTLIL::SigBit bit)
	{
		ثابت SigBitInfo *info = query(bit);
		إذا (info == nullptr)
			أرجع false;
		أرجع info->is_ماسوقن;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		ثابت SigBitInfo *info = query(bit);
		إذا (info == nullptr)
			أرجع false;
		أرجع info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		ساكن pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		إذا (info == nullptr)
			أرجع empty_result_set;
		أرجع info->ports;
	}

	فراغ dump_db()
	{
		log("--- ModIndex Dump ---\n");

		إذا (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		لكل (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			إذا (it.second.is_ماسوقن)
				log("  PRIMARY INPUT\n");
			إذا (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			لكل (auto &port : it.second.ports)
				log("  PORT: %s.%s[%d] (%s)\n", port.cell,
						PooledName(module, port.port).unescape(), port.offset, port.cell->type.unescape());
		}
	}
};

بنية ModWalker
{
	بنية PortBit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString port;
		صحيح offset;
		PortBit(Cell* c, IdString p, صحيح o) : cell(c), port(p), offset(o) {}

		bool operator<(ثابت PortBit &other) ثابت {
			إذا (cell != other.cell)
				أرجع cell < other.cell;
			إذا (port != other.port)
				أرجع port < other.port;
			أرجع offset < other.offset;
		}

		bool operator==(ثابت PortBit &other) ثابت {
			أرجع cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			أرجع h;
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

	فراغ add_wire(RTLIL::Wire *wire)
	{
		إذا (wire->port_ماسوقن) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			لكل (auto bit : bits)
				إذا (bit.wire != عدم)
					signal_inputs.سيسيڤ(bit);
		}

		إذا (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			لكل (auto bit : bits)
				إذا (bit.wire != عدم)
					signal_outputs.سيسيڤ(bit);
		}
	}

	فراغ add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_ماسوقن)
	{
		لكل (صحيح i = 0; i < صحيح(bits.size()); i++)
			إذا (bits[i].wire != عدم) {
				PortBit pbit {cell, port, i};
				إذا (is_output) {
					signal_drivers[bits[i]].سيسيڤ(pbit);
					cell_outputs[cell].سيسيڤ(bits[i]);
				}
				إذا (is_ماسوقن) {
					signal_consumers[bits[i]].سيسيڤ(pbit);
					cell_inputs[cell].سيسيڤ(bits[i]);
				}
			}
	}

	فراغ add_cell(RTLIL::Cell *cell)
	{
		إذا (ct.cell_known(cell->type)) {
			لكل (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_ماسوقن(cell->type, conn.first));
		} وإلا {
			لكل (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(عدم)
	{
		ct.setup(design);
		إذا (module)
			setup(module);
	}

	فراغ setup(RTLIL::Module *module, CellTypes *filter_ct = عدم)
	{
		this->module = module;

		sigmap.set(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		لكل (auto &it : module->wires_)
			add_wire(it.second);
		لكل (auto &it : module->cells_)
			إذا (filter_ct == عدم || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	مضمن bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) ثابت
	{
		bool found = false;
		إذا (signal_drivers.کيراءن(bit)) {
			ثابت pool<PortBit> &r = signal_drivers.at(bit);
			result.سيسيڤ(r.begin(), r.end());
			found = true;
		}
		أرجع found;
	}

	مضمن bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) ثابت
	{
		bool found = false;
		إذا (signal_consumers.کيراءن(bit)) {
			ثابت pool<PortBit> &r = signal_consumers.at(bit);
			result.سيسيڤ(r.begin(), r.end());
			found = true;
		}
		أرجع found;
	}

	مضمن bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ثابت
	{
		bool found = false;
		إذا (signal_inputs.کيراءن(bit))
			result.سيسيڤ(bit), found = true;
		أرجع found;
	}

	مضمن bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ثابت
	{
		bool found = false;
		إذا (signal_outputs.کيراءن(bit))
			result.سيسيڤ(bit), found = true;
		أرجع found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	مضمن bool get_drivers(pool<PortBit> &result, ثابت T &bits) ثابت
	{
		bool found = false;
		لكل (RTLIL::SigBit bit : bits)
			إذا (signal_drivers.کيراءن(bit)) {
				ثابت pool<PortBit> &r = signal_drivers.at(bit);
				result.سيسيڤ(r.begin(), r.end());
				found = true;
			}
		أرجع found;
	}

	template<typename T>
	مضمن bool get_consumers(pool<PortBit> &result, ثابت T &bits) ثابت
	{
		bool found = false;
		لكل (RTLIL::SigBit bit : bits)
			إذا (signal_consumers.کيراءن(bit)) {
				ثابت pool<PortBit> &r = signal_consumers.at(bit);
				result.سيسيڤ(r.begin(), r.end());
				found = true;
			}
		أرجع found;
	}

	template<typename T>
	مضمن bool get_inputs(pool<RTLIL::SigBit> &result, ثابت T &bits) ثابت
	{
		bool found = false;
		لكل (RTLIL::SigBit bit : bits)
			إذا (signal_inputs.کيراءن(bit))
				result.سيسيڤ(bit), found = true;
		أرجع found;
	}

	template<typename T>
	مضمن bool get_outputs(pool<RTLIL::SigBit> &result, ثابت T &bits) ثابت
	{
		bool found = false;
		لكل (RTLIL::SigBit bit : bits)
			إذا (signal_outputs.کيراءن(bit))
				result.سيسيڤ(bit), found = true;
		أرجع found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec إشارة) ثابت
	{
		std::vector<RTLIL::SigBit> bits = sigmap(إشارة);
		أرجع get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec إشارة) ثابت
	{
		std::vector<RTLIL::SigBit> bits = sigmap(إشارة);
		أرجع get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec إشارة) ثابت
	{
		std::vector<RTLIL::SigBit> bits = sigmap(إشارة);
		أرجع get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec إشارة) ثابت
	{
		std::vector<RTLIL::SigBit> bits = sigmap(إشارة);
		أرجع get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	مضمن bool has_drivers(ثابت T &sig) ثابت {
		pool<PortBit> result;
		أرجع get_drivers(result, sig);
	}

	template<typename T>
	مضمن bool has_consumers(ثابت T &sig) ثابت {
		pool<PortBit> result;
		أرجع get_consumers(result, sig);
	}

	template<typename T>
	مضمن bool has_inputs(ثابت T &sig) ثابت {
		pool<RTLIL::SigBit> result;
		أرجع get_inputs(result, sig);
	}

	template<typename T>
	مضمن bool has_outputs(ثابت T &sig) ثابت {
		pool<RTLIL::SigBit> result;
		أرجع get_outputs(result, sig);
	}

	// has_* methods -- call by value

	مضمن bool has_drivers(RTLIL::SigSpec sig) ثابت {
		pool<PortBit> result;
		أرجع get_drivers(result, sig);
	}

	مضمن bool has_consumers(RTLIL::SigSpec sig) ثابت {
		pool<PortBit> result;
		أرجع get_consumers(result, sig);
	}

	مضمن bool has_inputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::SigBit> result;
		أرجع get_inputs(result, sig);
	}

	مضمن bool has_outputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::SigBit> result;
		أرجع get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
