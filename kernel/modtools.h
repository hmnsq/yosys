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
			सचे (wire)
				offset = s.offset;
			अञ्ञथा
				data = s.data;
		}
		अन्तःस्थ bool operator<(ध्रुव RTLIL::SigBit &other) ध्रुव {
			सचे (wire == other.wire)
				पटिनिवत्त wire ? (offset < other.offset) : (data < other.data);
			सचे (wire != nullptr && other.wire != nullptr)
				पटिनिवत्त wire < other.wire; // look here
			पटिनिवत्त (wire != nullptr) < (other.wire != nullptr);
		}
	};
	संरचना PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		पूर्णाङ्क offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, पूर्णाङ्क _o) : cell(_c), port(_p), offset(_o) { }

		bool operator<(ध्रुव PortInfo &other) ध्रुव {
			सचे (cell != other.cell)
				पटिनिवत्त cell < other.cell;
			सचे (offset != other.offset)
				पटिनिवत्त offset < other.offset;
			पटिनिवत्त port < other.port;
		}

		bool operator==(ध्रुव PortInfo &other) ध्रुव {
			पटिनिवत्त cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			पटिनिवत्त h;
		}
	};

	संरचना SigBitInfo
	{
		bool is_निवेशः, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_निवेशः(false), is_output(false) { }

		bool operator==(ध्रुव SigBitInfo &other) ध्रुव {
			पटिनिवत्त is_निवेशः == other.is_निवेशः && is_output == other.is_output && ports == other.ports;
		}

		शून्य merge(ध्रुव SigBitInfo &other)
		{
			is_निवेशः = is_निवेशः || other.is_निवेशः;
			is_output = is_output || other.is_output;
			ports.निवेशय(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	पूर्णाङ्क auto_reload_counter;
	bool auto_reload_module;

	शून्य port_add(RTLIL::Cell *cell, RTLIL::IdString port, ध्रुव RTLIL::SigSpec &sig)
	{
		अत्थाय (पूर्णाङ्क i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			सचे (bit.wire)
				database[bit].ports.निवेशय(PortInfo(cell, port, i));
		}
	}

	शून्य port_del(RTLIL::Cell *cell, RTLIL::IdString port, ध्रुव RTLIL::SigSpec &sig)
	{
		अत्थाय (पूर्णाङ्क i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			सचे (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				सचे (!database[bit].is_निवेशः && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	ध्रुव SigBitInfo &info(RTLIL::SigBit bit)
	{
		पटिनिवत्त database[sigmap(bit)];
	}

	शून्य reload_module(bool reset_sigmap = true)
	{
		सचे (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		अत्थाय (auto wire : module->wires())
			सचे (wire->port_निवेशः || wire->port_output)
				अत्थाय (पूर्णाङ्क i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					सचे (bit.wire && wire->port_निवेशः)
						database[bit].is_निवेशः = true;
					सचे (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		अत्थाय (auto cell : module->cells())
			अत्थाय (auto &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		सचे (auto_reload_module) {
			सचे (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef NDEBUG
		सचे (auto_reload_module)
			पटिनिवत्त true;

		अत्थाय (auto it : database)
			log_assert(it.first == sigmap(it.first));

		auto database_bak = std::move(database);
		reload_module(false);

		सचे (!(database == database_bak))
		{
			अत्थाय (auto &it : database_bak)
				सचे (!database.गणनम्(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			अत्थाय (auto &it : database)
				सचे (!database_bak.गणनम्(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				अञ्ञथा सचे (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			पटिनिवत्त false;
		}
#endif
		पटिनिवत्त true;
	}

	शून्य check()
	{
		log_assert(ok());
	}

	शून्य notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, ध्रुव RTLIL::SigSpec &old_sig, ध्रुव RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		सचे (auto_reload_module)
			पटिनिवत्त;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	शून्य notify_connect(RTLIL::Module *mod, ध्रुव RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		सचे (auto_reload_module)
			पटिनिवत्त;

		अत्थाय (पूर्णाङ्क i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.गणनम्(lhs) != 0;
			bool has_rhs = database.गणनम्(rhs) != 0;

			सचे (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} अञ्ञथा
			सचे (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				सचे (lhs.wire)
					database[lhs] = new_info;
			} अञ्ञथा
			सचे (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				सचे (rhs.wire)
					database[rhs] = new_info;
			} अञ्ञथा {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				सचे (rhs.wire)
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
		module->monitors.निवेशय(this);
	}

	~ModIndex()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		सचे (auto_reload_module)
			reload_module();

		auto it = database.find(sigmap(bit));
		सचे (it == database.end())
			पटिनिवत्त nullptr;
		अञ्ञथा
			पटिनिवत्त &it->second;
	}

	bool query_is_निवेशः(RTLIL::SigBit bit)
	{
		ध्रुव SigBitInfo *info = query(bit);
		सचे (info == nullptr)
			पटिनिवत्त false;
		पटिनिवत्त info->is_निवेशः;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		ध्रुव SigBitInfo *info = query(bit);
		सचे (info == nullptr)
			पटिनिवत्त false;
		पटिनिवत्त info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		स्थिर pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		सचे (info == nullptr)
			पटिनिवत्त empty_result_set;
		पटिनिवत्त info->ports;
	}

	शून्य dump_db()
	{
		log("--- ModIndex Dump ---\n");

		सचे (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		अत्थाय (auto &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			सचे (it.second.is_निवेशः)
				log("  PRIMARY INPUT\n");
			सचे (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			अत्थाय (auto &port : it.second.ports)
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
			सचे (cell != other.cell)
				पटिनिवत्त cell < other.cell;
			सचे (port != other.port)
				पटिनिवत्त port < other.port;
			पटिनिवत्त offset < other.offset;
		}

		bool operator==(ध्रुव PortBit &other) ध्रुव {
			पटिनिवत्त cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			पटिनिवत्त h;
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
		सचे (wire->port_निवेशः) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			अत्थाय (auto bit : bits)
				सचे (bit.wire != रिक्त)
					signal_inputs.निवेशय(bit);
		}

		सचे (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			अत्थाय (auto bit : bits)
				सचे (bit.wire != रिक्त)
					signal_outputs.निवेशय(bit);
		}
	}

	शून्य add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_निवेशः)
	{
		अत्थाय (पूर्णाङ्क i = 0; i < पूर्णाङ्क(bits.size()); i++)
			सचे (bits[i].wire != रिक्त) {
				PortBit pbit {cell, port, i};
				सचे (is_output) {
					signal_drivers[bits[i]].निवेशय(pbit);
					cell_outputs[cell].निवेशय(bits[i]);
				}
				सचे (is_निवेशः) {
					signal_consumers[bits[i]].निवेशय(pbit);
					cell_inputs[cell].निवेशय(bits[i]);
				}
			}
	}

	शून्य add_cell(RTLIL::Cell *cell)
	{
		सचे (ct.cell_known(cell->type)) {
			अत्थाय (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_निवेशः(cell->type, conn.first));
		} अञ्ञथा {
			अत्थाय (auto &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(रिक्त)
	{
		ct.setup(design);
		सचे (module)
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

		अत्थाय (auto &it : module->wires_)
			add_wire(it.second);
		अत्थाय (auto &it : module->cells_)
			सचे (filter_ct == रिक्त || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	अन्तःस्थ bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) ध्रुव
	{
		bool found = false;
		सचे (signal_drivers.गणनम्(bit)) {
			ध्रुव pool<PortBit> &r = signal_drivers.at(bit);
			result.निवेशय(r.begin(), r.end());
			found = true;
		}
		पटिनिवत्त found;
	}

	अन्तःस्थ bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) ध्रुव
	{
		bool found = false;
		सचे (signal_consumers.गणनम्(bit)) {
			ध्रुव pool<PortBit> &r = signal_consumers.at(bit);
			result.निवेशय(r.begin(), r.end());
			found = true;
		}
		पटिनिवत्त found;
	}

	अन्तःस्थ bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ध्रुव
	{
		bool found = false;
		सचे (signal_inputs.गणनम्(bit))
			result.निवेशय(bit), found = true;
		पटिनिवत्त found;
	}

	अन्तःस्थ bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) ध्रुव
	{
		bool found = false;
		सचे (signal_outputs.गणनम्(bit))
			result.निवेशय(bit), found = true;
		पटिनिवत्त found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	template<typename T>
	अन्तःस्थ bool get_drivers(pool<PortBit> &result, ध्रुव T &bits) ध्रुव
	{
		bool found = false;
		अत्थाय (RTLIL::SigBit bit : bits)
			सचे (signal_drivers.गणनम्(bit)) {
				ध्रुव pool<PortBit> &r = signal_drivers.at(bit);
				result.निवेशय(r.begin(), r.end());
				found = true;
			}
		पटिनिवत्त found;
	}

	template<typename T>
	अन्तःस्थ bool get_consumers(pool<PortBit> &result, ध्रुव T &bits) ध्रुव
	{
		bool found = false;
		अत्थाय (RTLIL::SigBit bit : bits)
			सचे (signal_consumers.गणनम्(bit)) {
				ध्रुव pool<PortBit> &r = signal_consumers.at(bit);
				result.निवेशय(r.begin(), r.end());
				found = true;
			}
		पटिनिवत्त found;
	}

	template<typename T>
	अन्तःस्थ bool get_inputs(pool<RTLIL::SigBit> &result, ध्रुव T &bits) ध्रुव
	{
		bool found = false;
		अत्थाय (RTLIL::SigBit bit : bits)
			सचे (signal_inputs.गणनम्(bit))
				result.निवेशय(bit), found = true;
		पटिनिवत्त found;
	}

	template<typename T>
	अन्तःस्थ bool get_outputs(pool<RTLIL::SigBit> &result, ध्रुव T &bits) ध्रुव
	{
		bool found = false;
		अत्थाय (RTLIL::SigBit bit : bits)
			सचे (signal_outputs.गणनम्(bit))
				result.निवेशय(bit), found = true;
		पटिनिवत्त found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec सङ्केत) ध्रुव
	{
		std::vector<RTLIL::SigBit> bits = sigmap(सङ्केत);
		पटिनिवत्त get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec सङ्केत) ध्रुव
	{
		std::vector<RTLIL::SigBit> bits = sigmap(सङ्केत);
		पटिनिवत्त get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec सङ्केत) ध्रुव
	{
		std::vector<RTLIL::SigBit> bits = sigmap(सङ्केत);
		पटिनिवत्त get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec सङ्केत) ध्रुव
	{
		std::vector<RTLIL::SigBit> bits = sigmap(सङ्केत);
		पटिनिवत्त get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	template<typename T>
	अन्तःस्थ bool has_drivers(ध्रुव T &sig) ध्रुव {
		pool<PortBit> result;
		पटिनिवत्त get_drivers(result, sig);
	}

	template<typename T>
	अन्तःस्थ bool has_consumers(ध्रुव T &sig) ध्रुव {
		pool<PortBit> result;
		पटिनिवत्त get_consumers(result, sig);
	}

	template<typename T>
	अन्तःस्थ bool has_inputs(ध्रुव T &sig) ध्रुव {
		pool<RTLIL::SigBit> result;
		पटिनिवत्त get_inputs(result, sig);
	}

	template<typename T>
	अन्तःस्थ bool has_outputs(ध्रुव T &sig) ध्रुव {
		pool<RTLIL::SigBit> result;
		पटिनिवत्त get_outputs(result, sig);
	}

	// has_* methods -- call by value

	अन्तःस्थ bool has_drivers(RTLIL::SigSpec sig) ध्रुव {
		pool<PortBit> result;
		पटिनिवत्त get_drivers(result, sig);
	}

	अन्तःस्थ bool has_consumers(RTLIL::SigSpec sig) ध्रुव {
		pool<PortBit> result;
		पटिनिवत्त get_consumers(result, sig);
	}

	अन्तःस्थ bool has_inputs(RTLIL::SigSpec sig) ध्रुव {
		pool<RTLIL::SigBit> result;
		पटिनिवत्त get_inputs(result, sig);
	}

	अन्तःस्थ bool has_outputs(RTLIL::SigSpec sig) ध्रुव {
		pool<RTLIL::SigBit> result;
		पटिनिवत्त get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
