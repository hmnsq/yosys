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

#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "kernel/sigtools.h"
#அந்தர்பாவ "kernel/celltypes.h"
#அந்தர்பாவ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

கட்டமைப்பு ModIndex : பொது RTLIL::Monitor
{
	கட்டமைப்பு PointerOrderedSigBit : பொது RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			எனில் (wire)
				offset = s.offset;
			இல்லையெனில்
				data = s.data;
		}
		உள்வரி bool operator<(மாறிலி RTLIL::SigBit &other) மாறிலி {
			எனில் (wire == other.wire)
				திரும்பு wire ? (offset < other.offset) : (data < other.data);
			எனில் (wire != nullptr && other.wire != nullptr)
				திரும்பு wire < other.wire; // look here
			திரும்பு (wire != nullptr) < (other.wire != nullptr);
		}
	};
	கட்டமைப்பு PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		முழு offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, முழு _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(மாறிலி PortInfo &other) மாறிலி {
			எனில் (cell != other.cell)
				திரும்பு cell < other.cell;
			எனில் (offset != other.offset)
				திரும்பு offset < other.offset;
			திரும்பு port < other.port;
		}

		bool operator==(மாறிலி PortInfo &other) மாறிலி {
			திரும்பு cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			திரும்பு h;
		}
	};

	கட்டமைப்பு SigBitInfo
	{
		bool is_உள்ளீடு, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_உள்ளீடு(false), is_output(false) { }

		bool operator==(மாறிலி SigBitInfo &other) மாறிலி {
			திரும்பு is_உள்ளீடு == other.is_உள்ளீடு && is_output == other.is_output && ports == other.ports;
		}

		வெறுமை merge(மாறிலி SigBitInfo &other)
		{
			is_உள்ளீடு = is_உள்ளீடு || other.is_உள்ளீடு;
			is_output = is_output || other.is_output;
			ports.செருகு(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	முழு auto_reload_counter;
	bool auto_reload_module;

	வெறுமை port_add(RTLIL::Cell *cell, RTLIL::IdString port, மாறிலி RTLIL::SigSpec &sig)
	{
		க்கு (முழு i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			எனில் (bit.wire)
				database[bit].ports.செருகு(PortInfo(cell, port, i));
		}
	}

	வெறுமை port_del(RTLIL::Cell *cell, RTLIL::IdString port, மாறிலி RTLIL::SigSpec &sig)
	{
		க்கு (முழு i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			எனில் (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				எனில் (!database[bit].is_உள்ளீடு && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	மாறிலி SigBitInfo &info(RTLIL::SigBit bit)
	{
		திரும்பு database[sigmap(bit)];
	}

	வெறுமை reload_module(bool reset_sigmap = true)
	{
		எனில் (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		க்கு (auto wire : module->wires())
			எனில் (wire->port_உள்ளீடு || wire->port_output)
				க்கு (முழு i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					எனில் (bit.wire && wire->port_உள்ளீடு)
						database[bit].is_உள்ளீடு = true;
					எனில் (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		க்கு (auto cell : module->cells())
			க்கு (auto &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		எனில் (auto_reload_module) {
			எனில் (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef NDEBUG
		எனில் (auto_reload_module)
			திரும்பு true;

		க்கு (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		எனில் (!(database == database_bak))
		{
			க்கு (auto &it : database_bak)
				எனில் (!database.மொத்தம்(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			க்கு (auto &it : database)
				எனில் (!database_bak.மொத்தம்(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				இல்லையெனில் எனில் (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			திரும்பு false;
		}
#endif
		திரும்பு true;
	}

	வெறுமை check()
	{
		log_assert(ok());
	}

	வெறுமை notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, மாறிலி RTLIL::SigSpec &old_sig, மாறிலி RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		எனில் (auto_reload_module)
			திரும்பு;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	வெறுமை notify_connect(RTLIL::Module *mod, மாறிலி RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		எனில் (auto_reload_module)
			திரும்பு;

		க்கு (முழு i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.மொத்தம்(lhs) != 0;
			bool has_rhs = database.மொத்தம்(rhs) != 0;

			எனில் (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} இல்லையெனில்
			எனில் (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				எனில் (lhs.wire)
					database[lhs] = new_info;
			} இல்லையெனில்
			எனில் (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				எனில் (rhs.wire)
					database[rhs] = new_info;
			} இல்லையெனில் {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				எனில் (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	வெறுமை notify_connect(RTLIL::Module *mod, மாறிலி std::vector<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	வெறுமை notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ModIndex(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.செருகு(this);
	}

	~ModIndex()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		எனில் (auto_reload_module)
			reload_module();

		auto it = database.find(sigmap(bit));
		எனில் (it == database.end())
			திரும்பு nullptr;
		இல்லையெனில்
			திரும்பு &it->second;
	}

	bool query_is_உள்ளீடு(RTLIL::SigBit bit)
	{
		மாறிலி SigBitInfo *info = query(bit);
		எனில் (info == nullptr)
			திரும்பு false;
		திரும்பு info->is_உள்ளீடு;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		மாறிலி SigBitInfo *info = query(bit);
		எனில் (info == nullptr)
			திரும்பு false;
		திரும்பு info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		நிலையான pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		எனில் (info == nullptr)
			திரும்பு empty_result_set;
		திரும்பு info->ports;
	}

	வெறுமை dump_db()
	{
		log("--- ModIndex Dump ---\n");

		எனில் (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		க்கு (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			எனில் (it.second.is_உள்ளீடு)
				log("  PRIMARY INPUT\n");
			எனில் (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			க்கு (auto &port : it.second.ports)
				log("  PORT: %s.%s[%d] (%s)\n", port.cell,
						PooledName(module, port.port).unescape(), port.offset, port.cell->type.unescape());
		}
	}
};

கட்டமைப்பு ModWalker
{
	கட்டமைப்பு PortBit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString port;
		முழு offset;
		PortBit(Cell* c, IdString p, முழு o) : cell(c), port(p), offset(o) {}

		bool operator<(மாறிலி PortBit &other) மாறிலி {
			எனில் (cell != other.cell)
				திரும்பு cell < other.cell;
			எனில் (port != other.port)
				திரும்பு port < other.port;
			திரும்பு offset < other.offset;
		}

		bool operator==(மாறிலி PortBit &other) மாறிலி {
			திரும்பு cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			திரும்பு h;
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

	வெறுமை add_wire(RTLIL::Wire *wire)
	{
		எனில் (wire->port_உள்ளீடு) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			க்கு (auto bit : bits)
				எனில் (bit.wire != வெற்று)
					signal_inputs.செருகு(bit);
		}

		எனில் (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			க்கு (auto bit : bits)
				எனில் (bit.wire != வெற்று)
					signal_outputs.செருகு(bit);
		}
	}

	வெறுமை add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_உள்ளீடு)
	{
		க்கு (முழு i = 0; i < முழு(bits.size()); i++)
			எனில் (bits[i].wire != வெற்று) {
				PortBit pbit {cell, port, i};
				எனில் (is_output) {
					signal_drivers[bits[i]].செருகு(pbit);
					cell_outputs[cell].செருகு(bits[i]);
				}
				எனில் (is_உள்ளீடு) {
					signal_consumers[bits[i]].செருகு(pbit);
					cell_inputs[cell].செருகு(bits[i]);
				}
			}
	}

	வெறுமை add_cell(RTLIL::Cell *cell)
	{
		எனில் (ct.cell_known(cell->type)) {
			க்கு (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_உள்ளீடு(cell->type, conn.first));
		} இல்லையெனில் {
			க்கு (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(வெற்று)
	{
		ct.setup(design);
		எனில் (module)
			setup(module);
	}

	வெறுமை setup(RTLIL::Module *module, CellTypes *filter_ct = வெற்று)
	{
		this->module = module;

		sigmap.set(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		க்கு (auto &it : module->wires_)
			add_wire(it.second);
		க்கு (auto &it : module->cells_)
			எனில் (filter_ct == வெற்று || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	உள்வரி bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) மாறிலி
	{
		bool found = false;
		எனில் (signal_drivers.மொத்தம்(bit)) {
			மாறிலி pool<PortBit> &r = signal_drivers.at(bit);
			result.செருகு(r.begin(), r.end());
			found = true;
		}
		திரும்பு found;
	}

	உள்வரி bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) மாறிலி
	{
		bool found = false;
		எனில் (signal_consumers.மொத்தம்(bit)) {
			மாறிலி pool<PortBit> &r = signal_consumers.at(bit);
			result.செருகு(r.begin(), r.end());
			found = true;
		}
		திரும்பு found;
	}

	உள்வரி bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) மாறிலி
	{
		bool found = false;
		எனில் (signal_inputs.மொத்தம்(bit))
			result.செருகு(bit), found = true;
		திரும்பு found;
	}

	உள்வரி bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) மாறிலி
	{
		bool found = false;
		எனில் (signal_outputs.மொத்தம்(bit))
			result.செருகு(bit), found = true;
		திரும்பு found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	உள்வரி bool get_drivers(pool<PortBit> &result, மாறிலி T &bits) மாறிலி
	{
		bool found = false;
		க்கு (RTLIL::SigBit bit : bits)
			எனில் (signal_drivers.மொத்தம்(bit)) {
				மாறிலி pool<PortBit> &r = signal_drivers.at(bit);
				result.செருகு(r.begin(), r.end());
				found = true;
			}
		திரும்பு found;
	}

	template<typename T>
	உள்வரி bool get_consumers(pool<PortBit> &result, மாறிலி T &bits) மாறிலி
	{
		bool found = false;
		க்கு (RTLIL::SigBit bit : bits)
			எனில் (signal_consumers.மொத்தம்(bit)) {
				மாறிலி pool<PortBit> &r = signal_consumers.at(bit);
				result.செருகு(r.begin(), r.end());
				found = true;
			}
		திரும்பு found;
	}

	template<typename T>
	உள்வரி bool get_inputs(pool<RTLIL::SigBit> &result, மாறிலி T &bits) மாறிலி
	{
		bool found = false;
		க்கு (RTLIL::SigBit bit : bits)
			எனில் (signal_inputs.மொத்தம்(bit))
				result.செருகு(bit), found = true;
		திரும்பு found;
	}

	template<typename T>
	உள்வரி bool get_outputs(pool<RTLIL::SigBit> &result, மாறிலி T &bits) மாறிலி
	{
		bool found = false;
		க்கு (RTLIL::SigBit bit : bits)
			எனில் (signal_outputs.மொத்தம்(bit))
				result.செருகு(bit), found = true;
		திரும்பு found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec சமிக்ஞை) மாறிலி
	{
		std::vector<RTLIL::SigBit> bits = sigmap(சமிக்ஞை);
		திரும்பு get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec சமிக்ஞை) மாறிலி
	{
		std::vector<RTLIL::SigBit> bits = sigmap(சமிக்ஞை);
		திரும்பு get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec சமிக்ஞை) மாறிலி
	{
		std::vector<RTLIL::SigBit> bits = sigmap(சமிக்ஞை);
		திரும்பு get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec சமிக்ஞை) மாறிலி
	{
		std::vector<RTLIL::SigBit> bits = sigmap(சமிக்ஞை);
		திரும்பு get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	உள்வரி bool has_drivers(மாறிலி T &sig) மாறிலி {
		pool<PortBit> result;
		திரும்பு get_drivers(result, sig);
	}

	template<typename T>
	உள்வரி bool has_consumers(மாறிலி T &sig) மாறிலி {
		pool<PortBit> result;
		திரும்பு get_consumers(result, sig);
	}

	template<typename T>
	உள்வரி bool has_inputs(மாறிலி T &sig) மாறிலி {
		pool<RTLIL::SigBit> result;
		திரும்பு get_inputs(result, sig);
	}

	template<typename T>
	உள்வரி bool has_outputs(மாறிலி T &sig) மாறிலி {
		pool<RTLIL::SigBit> result;
		திரும்பு get_outputs(result, sig);
	}

	// has_* methods -- call by value

	உள்வரி bool has_drivers(RTLIL::SigSpec sig) மாறிலி {
		pool<PortBit> result;
		திரும்பு get_drivers(result, sig);
	}

	உள்வரி bool has_consumers(RTLIL::SigSpec sig) மாறிலி {
		pool<PortBit> result;
		திரும்பு get_consumers(result, sig);
	}

	உள்வரி bool has_inputs(RTLIL::SigSpec sig) மாறிலி {
		pool<RTLIL::SigBit> result;
		திரும்பு get_inputs(result, sig);
	}

	உள்வரி bool has_outputs(RTLIL::SigSpec sig) மாறிலி {
		pool<RTLIL::SigBit> result;
		திரும்பு get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
