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

#شامل "kernel/yosys.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/celltypes.h"
#شامل "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ساختار ModIndex : عمومی RTLIL::Monitor
{
	ساختار PointerOrderedSigBit : عمومی RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			اگر (wire)
				offset = s.offset;
			وگرنه
				data = s.data;
		}
		درونخطی bool operator<(ثابت RTLIL::SigBit &other) ثابت {
			اگر (wire == other.wire)
				بازگشت wire ? (offset < other.offset) : (data < other.data);
			اگر (wire != nullptr && other.wire != nullptr)
				بازگشت wire < other.wire; // look here
			بازگشت (wire != nullptr) < (other.wire != nullptr);
		}
	};
	ساختار PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		صحیح offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, صحیح _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(ثابت PortInfo &other) ثابت {
			اگر (cell != other.cell)
				بازگشت cell < other.cell;
			اگر (offset != other.offset)
				بازگشت offset < other.offset;
			بازگشت port < other.port;
		}

		bool operator==(ثابت PortInfo &other) ثابت {
			بازگشت cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			بازگشت h;
		}
	};

	ساختار SigBitInfo
	{
		bool is_درون_داد, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_درون_داد(false), is_output(false) { }

		bool operator==(ثابت SigBitInfo &other) ثابت {
			بازگشت is_درون_داد == other.is_درون_داد && is_output == other.is_output && ports == other.ports;
		}

		تهی merge(ثابت SigBitInfo &other)
		{
			is_درون_داد = is_درون_داد || other.is_درون_داد;
			is_output = is_output || other.is_output;
			ports.درج(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	صحیح auto_reload_counter;
	bool auto_reload_module;

	تهی port_add(RTLIL::Cell *cell, RTLIL::IdString port, ثابت RTLIL::SigSpec &sig)
	{
		برای (صحیح i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			اگر (bit.wire)
				database[bit].ports.درج(PortInfo(cell, port, i));
		}
	}

	تهی port_del(RTLIL::Cell *cell, RTLIL::IdString port, ثابت RTLIL::SigSpec &sig)
	{
		برای (صحیح i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			اگر (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				اگر (!database[bit].is_درون_داد && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	ثابت SigBitInfo &info(RTLIL::SigBit bit)
	{
		بازگشت database[sigmap(bit)];
	}

	تهی reload_module(bool reset_sigmap = true)
	{
		اگر (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		برای (auto wire : module->wires())
			اگر (wire->port_درون_داد || wire->port_output)
				برای (صحیح i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					اگر (bit.wire && wire->port_درون_داد)
						database[bit].is_درون_داد = true;
					اگر (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		برای (auto cell : module->cells())
			برای (auto &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		اگر (auto_reload_module) {
			اگر (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef NDEBUG
		اگر (auto_reload_module)
			بازگشت true;

		برای (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		اگر (!(database == database_bak))
		{
			برای (auto &it : database_bak)
				اگر (!database.شمار(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			برای (auto &it : database)
				اگر (!database_bak.شمار(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				وگرنه اگر (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			بازگشت false;
		}
#endif
		بازگشت true;
	}

	تهی check()
	{
		log_assert(ok());
	}

	تهی notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, ثابت RTLIL::SigSpec &old_sig, ثابت RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		اگر (auto_reload_module)
			بازگشت;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	تهی notify_connect(RTLIL::Module *mod, ثابت RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		اگر (auto_reload_module)
			بازگشت;

		برای (صحیح i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.شمار(lhs) != 0;
			bool has_rhs = database.شمار(rhs) != 0;

			اگر (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} وگرنه
			اگر (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				اگر (lhs.wire)
					database[lhs] = new_info;
			} وگرنه
			اگر (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				اگر (rhs.wire)
					database[rhs] = new_info;
			} وگرنه {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				اگر (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	تهی notify_connect(RTLIL::Module *mod, ثابت std::vector<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	تهی notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ModIndex(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.درج(this);
	}

	~ModIndex()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		اگر (auto_reload_module)
			reload_module();

		auto it = database.find(sigmap(bit));
		اگر (it == database.end())
			بازگشت nullptr;
		وگرنه
			بازگشت &it->second;
	}

	bool query_is_درون_داد(RTLIL::SigBit bit)
	{
		ثابت SigBitInfo *info = query(bit);
		اگر (info == nullptr)
			بازگشت false;
		بازگشت info->is_درون_داد;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		ثابت SigBitInfo *info = query(bit);
		اگر (info == nullptr)
			بازگشت false;
		بازگشت info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		ایستا pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		اگر (info == nullptr)
			بازگشت empty_result_set;
		بازگشت info->ports;
	}

	تهی dump_db()
	{
		log("--- ModIndex Dump ---\n");

		اگر (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		برای (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			اگر (it.second.is_درون_داد)
				log("  PRIMARY INPUT\n");
			اگر (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			برای (auto &port : it.second.ports)
				log("  PORT: %s.%s[%d] (%s)\n", port.cell,
						PooledName(module, port.port).unescape(), port.offset, port.cell->type.unescape());
		}
	}
};

ساختار ModWalker
{
	ساختار PortBit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString port;
		صحیح offset;
		PortBit(Cell* c, IdString p, صحیح o) : cell(c), port(p), offset(o) {}

		bool operator<(ثابت PortBit &other) ثابت {
			اگر (cell != other.cell)
				بازگشت cell < other.cell;
			اگر (port != other.port)
				بازگشت port < other.port;
			بازگشت offset < other.offset;
		}

		bool operator==(ثابت PortBit &other) ثابت {
			بازگشت cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			بازگشت h;
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

	تهی add_wire(RTLIL::Wire *wire)
	{
		اگر (wire->port_درون_داد) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			برای (auto bit : bits)
				اگر (bit.wire != پوچ)
					signal_inputs.درج(bit);
		}

		اگر (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			برای (auto bit : bits)
				اگر (bit.wire != پوچ)
					signal_outputs.درج(bit);
		}
	}

	تهی add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_درون_داد)
	{
		برای (صحیح i = 0; i < صحیح(bits.size()); i++)
			اگر (bits[i].wire != پوچ) {
				PortBit pbit {cell, port, i};
				اگر (is_output) {
					signal_drivers[bits[i]].درج(pbit);
					cell_outputs[cell].درج(bits[i]);
				}
				اگر (is_درون_داد) {
					signal_consumers[bits[i]].درج(pbit);
					cell_inputs[cell].درج(bits[i]);
				}
			}
	}

	تهی add_cell(RTLIL::Cell *cell)
	{
		اگر (ct.cell_known(cell->type)) {
			برای (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_درون_داد(cell->type, conn.first));
		} وگرنه {
			برای (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(پوچ)
	{
		ct.setup(design);
		اگر (module)
			setup(module);
	}

	تهی setup(RTLIL::Module *module, CellTypes *filter_ct = پوچ)
	{
		this->module = module;

		sigmap.set(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		برای (auto &it : module->wires_)
			add_wire(it.second);
		برای (auto &it : module->cells_)
			اگر (filter_ct == پوچ || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	درونخطی bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) ثابت
	{
		bool found = false;
		اگر (signal_drivers.شمار(bit)) {
			ثابت pool<PortBit> &r = signal_drivers.at(bit);
			result.درج(r.begin(), r.end());
			found = true;
		}
		بازگشت found;
	}

	درونخطی bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) ثابت
	{
		bool found = false;
		اگر (signal_consumers.شمار(bit)) {
			ثابت pool<PortBit> &r = signal_consumers.at(bit);
			result.درج(r.begin(), r.end());
			found = true;
		}
		بازگشت found;
	}

	درونخطی bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ثابت
	{
		bool found = false;
		اگر (signal_inputs.شمار(bit))
			result.درج(bit), found = true;
		بازگشت found;
	}

	درونخطی bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ثابت
	{
		bool found = false;
		اگر (signal_outputs.شمار(bit))
			result.درج(bit), found = true;
		بازگشت found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	درونخطی bool get_drivers(pool<PortBit> &result, ثابت T &bits) ثابت
	{
		bool found = false;
		برای (RTLIL::SigBit bit : bits)
			اگر (signal_drivers.شمار(bit)) {
				ثابت pool<PortBit> &r = signal_drivers.at(bit);
				result.درج(r.begin(), r.end());
				found = true;
			}
		بازگشت found;
	}

	template<typename T>
	درونخطی bool get_consumers(pool<PortBit> &result, ثابت T &bits) ثابت
	{
		bool found = false;
		برای (RTLIL::SigBit bit : bits)
			اگر (signal_consumers.شمار(bit)) {
				ثابت pool<PortBit> &r = signal_consumers.at(bit);
				result.درج(r.begin(), r.end());
				found = true;
			}
		بازگشت found;
	}

	template<typename T>
	درونخطی bool get_inputs(pool<RTLIL::SigBit> &result, ثابت T &bits) ثابت
	{
		bool found = false;
		برای (RTLIL::SigBit bit : bits)
			اگر (signal_inputs.شمار(bit))
				result.درج(bit), found = true;
		بازگشت found;
	}

	template<typename T>
	درونخطی bool get_outputs(pool<RTLIL::SigBit> &result, ثابت T &bits) ثابت
	{
		bool found = false;
		برای (RTLIL::SigBit bit : bits)
			اگر (signal_outputs.شمار(bit))
				result.درج(bit), found = true;
		بازگشت found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec سیگنال) ثابت
	{
		std::vector<RTLIL::SigBit> bits = sigmap(سیگنال);
		بازگشت get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec سیگنال) ثابت
	{
		std::vector<RTLIL::SigBit> bits = sigmap(سیگنال);
		بازگشت get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec سیگنال) ثابت
	{
		std::vector<RTLIL::SigBit> bits = sigmap(سیگنال);
		بازگشت get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec سیگنال) ثابت
	{
		std::vector<RTLIL::SigBit> bits = sigmap(سیگنال);
		بازگشت get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	درونخطی bool has_drivers(ثابت T &sig) ثابت {
		pool<PortBit> result;
		بازگشت get_drivers(result, sig);
	}

	template<typename T>
	درونخطی bool has_consumers(ثابت T &sig) ثابت {
		pool<PortBit> result;
		بازگشت get_consumers(result, sig);
	}

	template<typename T>
	درونخطی bool has_inputs(ثابت T &sig) ثابت {
		pool<RTLIL::SigBit> result;
		بازگشت get_inputs(result, sig);
	}

	template<typename T>
	درونخطی bool has_outputs(ثابت T &sig) ثابت {
		pool<RTLIL::SigBit> result;
		بازگشت get_outputs(result, sig);
	}

	// has_* methods -- call by value

	درونخطی bool has_drivers(RTLIL::SigSpec sig) ثابت {
		pool<PortBit> result;
		بازگشت get_drivers(result, sig);
	}

	درونخطی bool has_consumers(RTLIL::SigSpec sig) ثابت {
		pool<PortBit> result;
		بازگشت get_consumers(result, sig);
	}

	درونخطی bool has_inputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::SigBit> result;
		بازگشت get_inputs(result, sig);
	}

	درونخطی bool has_outputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::SigBit> result;
		بازگشت get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
