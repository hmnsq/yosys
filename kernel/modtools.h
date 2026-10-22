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

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/sigtools.h"
#अन्तर्भाव "kernel/celltypes.h"
#अन्तर्भाव "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

संरचना ModIndex : सार्वजनिक RTLIL::Monitor
{
	संरचना PointerOrderedSigBit : सार्वजनिक RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			यदि (wire)
				offset = s.offset;
			अन्यथा
				data = s.data;
		}
		अन्तःस्थ bool operator<(ध्रुव RTLIL::SigBit &other) ध्रुव {
			यदि (wire == other.wire)
				प्रत्यागम wire ? (offset < other.offset) : (data < other.data);
			यदि (wire != nullptr && other.wire != nullptr)
				प्रत्यागम wire < other.wire; // look here
			प्रत्यागम (wire != nullptr) < (other.wire != nullptr);
		}
	};
	संरचना PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		पूर्णाङ्क offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, पूर्णाङ्क _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(ध्रुव PortInfo &other) ध्रुव {
			यदि (cell != other.cell)
				प्रत्यागम cell < other.cell;
			यदि (offset != other.offset)
				प्रत्यागम offset < other.offset;
			प्रत्यागम port < other.port;
		}

		bool operator==(ध्रुव PortInfo &other) ध्रुव {
			प्रत्यागम cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			प्रत्यागम h;
		}
	};

	संरचना SigBitInfo
	{
		bool is_निवेश, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_निवेश(false), is_output(false) { }

		bool operator==(ध्रुव SigBitInfo &other) ध्रुव {
			प्रत्यागम is_निवेश == other.is_निवेश && is_output == other.is_output && ports == other.ports;
		}

		शून्य merge(ध्रुव SigBitInfo &other)
		{
			is_निवेश = is_निवेश || other.is_निवेश;
			is_output = is_output || other.is_output;
			ports.जोड़ो(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	पूर्णाङ्क auto_reload_counter;
	bool auto_reload_module;

	शून्य port_add(RTLIL::Cell *cell, RTLIL::IdString port, ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (पूर्णाङ्क i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			यदि (bit.wire)
				database[bit].ports.जोड़ो(PortInfo(cell, port, i));
		}
	}

	शून्य port_del(RTLIL::Cell *cell, RTLIL::IdString port, ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (पूर्णाङ्क i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			यदि (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				यदि (!database[bit].is_निवेश && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	ध्रुव SigBitInfo &info(RTLIL::SigBit bit)
	{
		प्रत्यागम database[sigmap(bit)];
	}

	शून्य reload_module(bool reset_sigmap = true)
	{
		यदि (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		प्रति (auto wire : module->wires())
			यदि (wire->port_निवेश || wire->port_output)
				प्रति (पूर्णाङ्क i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					यदि (bit.wire && wire->port_निवेश)
						database[bit].is_निवेश = true;
					यदि (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		प्रति (auto cell : module->cells())
			प्रति (auto &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		यदि (auto_reload_module) {
			यदि (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef NDEBUG
		यदि (auto_reload_module)
			प्रत्यागम true;

		प्रति (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		यदि (!(database == database_bak))
		{
			प्रति (auto &it : database_bak)
				यदि (!database.गिनती(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			प्रति (auto &it : database)
				यदि (!database_bak.गिनती(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				अन्यथा यदि (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			प्रत्यागम false;
		}
#endif
		प्रत्यागम true;
	}

	शून्य check()
	{
		log_assert(ok());
	}

	शून्य notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, ध्रुव RTLIL::SigSpec &old_sig, ध्रुव RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		यदि (auto_reload_module)
			प्रत्यागम;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	शून्य notify_connect(RTLIL::Module *mod, ध्रुव RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		यदि (auto_reload_module)
			प्रत्यागम;

		प्रति (पूर्णाङ्क i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.गिनती(lhs) != 0;
			bool has_rhs = database.गिनती(rhs) != 0;

			यदि (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} अन्यथा
			यदि (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				यदि (lhs.wire)
					database[lhs] = new_info;
			} अन्यथा
			यदि (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				यदि (rhs.wire)
					database[rhs] = new_info;
			} अन्यथा {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				यदि (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	शून्य notify_connect(RTLIL::Module *mod, ध्रुव std::vector<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	शून्य notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ModIndex(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.जोड़ो(this);
	}

	~ModIndex()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		यदि (auto_reload_module)
			reload_module();

		auto it = database.find(sigmap(bit));
		यदि (it == database.end())
			प्रत्यागम nullptr;
		अन्यथा
			प्रत्यागम &it->second;
	}

	bool query_is_निवेश(RTLIL::SigBit bit)
	{
		ध्रुव SigBitInfo *info = query(bit);
		यदि (info == nullptr)
			प्रत्यागम false;
		प्रत्यागम info->is_निवेश;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		ध्रुव SigBitInfo *info = query(bit);
		यदि (info == nullptr)
			प्रत्यागम false;
		प्रत्यागम info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		स्थिर pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		यदि (info == nullptr)
			प्रत्यागम empty_result_set;
		प्रत्यागम info->ports;
	}

	शून्य dump_db()
	{
		log("--- ModIndex Dump ---\n");

		यदि (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		प्रति (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			यदि (it.second.is_निवेश)
				log("  PRIMARY INPUT\n");
			यदि (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			प्रति (auto &port : it.second.ports)
				log("  PORT: %s.%s[%d] (%s)\n", port.cell,
						PooledName(module, port.port).unescape(), port.offset, port.cell->type.unescape());
		}
	}
};

संरचना ModWalker
{
	संरचना PortBit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString port;
		पूर्णाङ्क offset;
		PortBit(Cell* c, IdString p, पूर्णाङ्क o) : cell(c), port(p), offset(o) {}

		bool operator<(ध्रुव PortBit &other) ध्रुव {
			यदि (cell != other.cell)
				प्रत्यागम cell < other.cell;
			यदि (port != other.port)
				प्रत्यागम port < other.port;
			प्रत्यागम offset < other.offset;
		}

		bool operator==(ध्रुव PortBit &other) ध्रुव {
			प्रत्यागम cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			प्रत्यागम h;
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

	शून्य add_wire(RTLIL::Wire *wire)
	{
		यदि (wire->port_निवेश) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			प्रति (auto bit : bits)
				यदि (bit.wire != रिक्त)
					signal_inputs.जोड़ो(bit);
		}

		यदि (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			प्रति (auto bit : bits)
				यदि (bit.wire != रिक्त)
					signal_outputs.जोड़ो(bit);
		}
	}

	शून्य add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_निवेश)
	{
		प्रति (पूर्णाङ्क i = 0; i < पूर्णाङ्क(bits.size()); i++)
			यदि (bits[i].wire != रिक्त) {
				PortBit pbit {cell, port, i};
				यदि (is_output) {
					signal_drivers[bits[i]].जोड़ो(pbit);
					cell_outputs[cell].जोड़ो(bits[i]);
				}
				यदि (is_निवेश) {
					signal_consumers[bits[i]].जोड़ो(pbit);
					cell_inputs[cell].जोड़ो(bits[i]);
				}
			}
	}

	शून्य add_cell(RTLIL::Cell *cell)
	{
		यदि (ct.cell_known(cell->type)) {
			प्रति (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_निवेश(cell->type, conn.first));
		} अन्यथा {
			प्रति (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(रिक्त)
	{
		ct.setup(design);
		यदि (module)
			setup(module);
	}

	शून्य setup(RTLIL::Module *module, CellTypes *filter_ct = रिक्त)
	{
		this->module = module;

		sigmap.set(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		प्रति (auto &it : module->wires_)
			add_wire(it.second);
		प्रति (auto &it : module->cells_)
			यदि (filter_ct == रिक्त || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	अन्तःस्थ bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) ध्रुव
	{
		bool found = false;
		यदि (signal_drivers.गिनती(bit)) {
			ध्रुव pool<PortBit> &r = signal_drivers.at(bit);
			result.जोड़ो(r.begin(), r.end());
			found = true;
		}
		प्रत्यागम found;
	}

	अन्तःस्थ bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) ध्रुव
	{
		bool found = false;
		यदि (signal_consumers.गिनती(bit)) {
			ध्रुव pool<PortBit> &r = signal_consumers.at(bit);
			result.जोड़ो(r.begin(), r.end());
			found = true;
		}
		प्रत्यागम found;
	}

	अन्तःस्थ bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ध्रुव
	{
		bool found = false;
		यदि (signal_inputs.गिनती(bit))
			result.जोड़ो(bit), found = true;
		प्रत्यागम found;
	}

	अन्तःस्थ bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ध्रुव
	{
		bool found = false;
		यदि (signal_outputs.गिनती(bit))
			result.जोड़ो(bit), found = true;
		प्रत्यागम found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	अन्तःस्थ bool get_drivers(pool<PortBit> &result, ध्रुव T &bits) ध्रुव
	{
		bool found = false;
		प्रति (RTLIL::SigBit bit : bits)
			यदि (signal_drivers.गिनती(bit)) {
				ध्रुव pool<PortBit> &r = signal_drivers.at(bit);
				result.जोड़ो(r.begin(), r.end());
				found = true;
			}
		प्रत्यागम found;
	}

	template<typename T>
	अन्तःस्थ bool get_consumers(pool<PortBit> &result, ध्रुव T &bits) ध्रुव
	{
		bool found = false;
		प्रति (RTLIL::SigBit bit : bits)
			यदि (signal_consumers.गिनती(bit)) {
				ध्रुव pool<PortBit> &r = signal_consumers.at(bit);
				result.जोड़ो(r.begin(), r.end());
				found = true;
			}
		प्रत्यागम found;
	}

	template<typename T>
	अन्तःस्थ bool get_inputs(pool<RTLIL::SigBit> &result, ध्रुव T &bits) ध्रुव
	{
		bool found = false;
		प्रति (RTLIL::SigBit bit : bits)
			यदि (signal_inputs.गिनती(bit))
				result.जोड़ो(bit), found = true;
		प्रत्यागम found;
	}

	template<typename T>
	अन्तःस्थ bool get_outputs(pool<RTLIL::SigBit> &result, ध्रुव T &bits) ध्रुव
	{
		bool found = false;
		प्रति (RTLIL::SigBit bit : bits)
			यदि (signal_outputs.गिनती(bit))
				result.जोड़ो(bit), found = true;
		प्रत्यागम found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec सङ्केत) ध्रुव
	{
		std::vector<RTLIL::SigBit> bits = sigmap(सङ्केत);
		प्रत्यागम get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec सङ्केत) ध्रुव
	{
		std::vector<RTLIL::SigBit> bits = sigmap(सङ्केत);
		प्रत्यागम get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec सङ्केत) ध्रुव
	{
		std::vector<RTLIL::SigBit> bits = sigmap(सङ्केत);
		प्रत्यागम get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec सङ्केत) ध्रुव
	{
		std::vector<RTLIL::SigBit> bits = sigmap(सङ्केत);
		प्रत्यागम get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	अन्तःस्थ bool has_drivers(ध्रुव T &sig) ध्रुव {
		pool<PortBit> result;
		प्रत्यागम get_drivers(result, sig);
	}

	template<typename T>
	अन्तःस्थ bool has_consumers(ध्रुव T &sig) ध्रुव {
		pool<PortBit> result;
		प्रत्यागम get_consumers(result, sig);
	}

	template<typename T>
	अन्तःस्थ bool has_inputs(ध्रुव T &sig) ध्रुव {
		pool<RTLIL::SigBit> result;
		प्रत्यागम get_inputs(result, sig);
	}

	template<typename T>
	अन्तःस्थ bool has_outputs(ध्रुव T &sig) ध्रुव {
		pool<RTLIL::SigBit> result;
		प्रत्यागम get_outputs(result, sig);
	}

	// has_* methods -- call by value

	अन्तःस्थ bool has_drivers(RTLIL::SigSpec sig) ध्रुव {
		pool<PortBit> result;
		प्रत्यागम get_drivers(result, sig);
	}

	अन्तःस्थ bool has_consumers(RTLIL::SigSpec sig) ध्रुव {
		pool<PortBit> result;
		प्रत्यागम get_consumers(result, sig);
	}

	अन्तःस्थ bool has_inputs(RTLIL::SigSpec sig) ध्रुव {
		pool<RTLIL::SigBit> result;
		प्रत्यागम get_inputs(result, sig);
	}

	अन्तःस्थ bool has_outputs(RTLIL::SigSpec sig) ध्रुव {
		pool<RTLIL::SigBit> result;
		प्रत्यागम get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
