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
		अन्तःस्थ bool चालक<(अचर RTLIL::SigBit &other) अचर {
			यदि (wire == other.wire)
				फर्काऊ wire ? (offset < other.offset) : (data < other.data);
			यदि (wire != nullptr && other.wire != nullptr)
				फर्काऊ wire < other.wire; // look here
			फर्काऊ (wire != nullptr) < (other.wire != nullptr);
		}
	};
	संरचना PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		पूर्णाङ्क offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, पूर्णाङ्क _o) : cell(_c), port(_p), offset(_o) { }

		bool चालक<(अचर PortInfo &other) अचर {
			यदि (cell != other.cell)
				फर्काऊ cell < other.cell;
			यदि (offset != other.offset)
				फर्काऊ offset < other.offset;
			फर्काऊ port < other.port;
		}

		bool चालक==(अचर PortInfo &other) अचर {
			फर्काऊ cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) अचर {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			फर्काऊ h;
		}
	};

	संरचना SigBitInfo
	{
		bool is_आगत, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_आगत(false), is_output(false) { }

		bool चालक==(अचर SigBitInfo &other) अचर {
			फर्काऊ is_आगत == other.is_आगत && is_output == other.is_output && ports == other.ports;
		}

		शून्य merge(अचर SigBitInfo &other)
		{
			is_आगत = is_आगत || other.is_आगत;
			is_output = is_output || other.is_output;
			ports.घुसाउ(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	पूर्णाङ्क auto_reload_counter;
	bool auto_reload_module;

	शून्य port_add(RTLIL::Cell *cell, RTLIL::IdString port, अचर RTLIL::SigSpec &sig)
	{
		लागि (पूर्णाङ्क i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			यदि (bit.wire)
				database[bit].ports.घुसाउ(PortInfo(cell, port, i));
		}
	}

	शून्य port_del(RTLIL::Cell *cell, RTLIL::IdString port, अचर RTLIL::SigSpec &sig)
	{
		लागि (पूर्णाङ्क i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			यदि (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				यदि (!database[bit].is_आगत && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	अचर SigBitInfo &info(RTLIL::SigBit bit)
	{
		फर्काऊ database[sigmap(bit)];
	}

	शून्य reload_module(bool reset_sigmap = true)
	{
		यदि (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		लागि (स्वतः wire : module->wires())
			यदि (wire->port_आगत || wire->port_output)
				लागि (पूर्णाङ्क i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					यदि (bit.wire && wire->port_आगत)
						database[bit].is_आगत = true;
					यदि (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		लागि (स्वतः cell : module->cells())
			लागि (स्वतः &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		यदि (auto_reload_module) {
			यदि (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef न_संशोधन
		यदि (auto_reload_module)
			फर्काऊ true;

		लागि (स्वतः it : database)
			log_assert(it.first == sigmap(it.first));

		स्वतः database_bak = std::move(database);
		reload_module(false);

		यदि (!(database == database_bak))
		{
			लागि (स्वतः &it : database_bak)
				यदि (!database.गन्ती(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			लागि (स्वतः &it : database)
				यदि (!database_bak.गन्ती(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				अन्यथा यदि (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			फर्काऊ false;
		}
#endif
		फर्काऊ true;
	}

	शून्य check()
	{
		log_assert(ok());
	}

	शून्य notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, अचर RTLIL::SigSpec &old_sig, अचर RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		यदि (auto_reload_module)
			फर्काऊ;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	शून्य notify_connect(RTLIL::Module *mod, अचर RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		यदि (auto_reload_module)
			फर्काऊ;

		लागि (पूर्णाङ्क i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.गन्ती(lhs) != 0;
			bool has_rhs = database.गन्ती(rhs) != 0;

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

	शून्य notify_connect(RTLIL::Module *mod, अचर std::vector<RTLIL::SigSig>&) override
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
		module->monitors.घुसाउ(this);
	}

	~ModIndex()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		यदि (auto_reload_module)
			reload_module();

		स्वतः it = database.find(sigmap(bit));
		यदि (it == database.end())
			फर्काऊ nullptr;
		अन्यथा
			फर्काऊ &it->second;
	}

	bool query_is_आगत(RTLIL::SigBit bit)
	{
		अचर SigBitInfo *info = query(bit);
		यदि (info == nullptr)
			फर्काऊ false;
		फर्काऊ info->is_आगत;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		अचर SigBitInfo *info = query(bit);
		यदि (info == nullptr)
			फर्काऊ false;
		फर्काऊ info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		स्थिर pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		यदि (info == nullptr)
			फर्काऊ empty_result_set;
		फर्काऊ info->ports;
	}

	शून्य dump_db()
	{
		log("--- ModIndex Dump ---\n");

		यदि (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		लागि (स्वतः &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			यदि (it.second.is_आगत)
				log("  PRIMARY INPUT\n");
			यदि (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			लागि (स्वतः &port : it.second.ports)
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

		bool चालक<(अचर PortBit &other) अचर {
			यदि (cell != other.cell)
				फर्काऊ cell < other.cell;
			यदि (port != other.port)
				फर्काऊ port < other.port;
			फर्काऊ offset < other.offset;
		}

		bool चालक==(अचर PortBit &other) अचर {
			फर्काऊ cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) अचर {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			फर्काऊ h;
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
		यदि (wire->port_आगत) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			लागि (स्वतः bit : bits)
				यदि (bit.wire != रिक्त)
					signal_inputs.घुसाउ(bit);
		}

		यदि (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			लागि (स्वतः bit : bits)
				यदि (bit.wire != रिक्त)
					signal_outputs.घुसाउ(bit);
		}
	}

	शून्य add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_आगत)
	{
		लागि (पूर्णाङ्क i = 0; i < पूर्णाङ्क(bits.size()); i++)
			यदि (bits[i].wire != रिक्त) {
				PortBit pbit {cell, port, i};
				यदि (is_output) {
					signal_drivers[bits[i]].घुसाउ(pbit);
					cell_outputs[cell].घुसाउ(bits[i]);
				}
				यदि (is_आगत) {
					signal_consumers[bits[i]].घुसाउ(pbit);
					cell_inputs[cell].घुसाउ(bits[i]);
				}
			}
	}

	शून्य add_cell(RTLIL::Cell *cell)
	{
		यदि (ct.cell_known(cell->type)) {
			लागि (स्वतः &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_आगत(cell->type, conn.first));
		} अन्यथा {
			लागि (स्वतः &conn : cell->connections())
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

		लागि (स्वतः &it : module->wires_)
			add_wire(it.second);
		लागि (स्वतः &it : module->cells_)
			यदि (filter_ct == रिक्त || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	अन्तःस्थ bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) अचर
	{
		bool found = false;
		यदि (signal_drivers.गन्ती(bit)) {
			अचर pool<PortBit> &r = signal_drivers.at(bit);
			result.घुसाउ(r.begin(), r.end());
			found = true;
		}
		फर्काऊ found;
	}

	अन्तःस्थ bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) अचर
	{
		bool found = false;
		यदि (signal_consumers.गन्ती(bit)) {
			अचर pool<PortBit> &r = signal_consumers.at(bit);
			result.घुसाउ(r.begin(), r.end());
			found = true;
		}
		फर्काऊ found;
	}

	अन्तःस्थ bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) अचर
	{
		bool found = false;
		यदि (signal_inputs.गन्ती(bit))
			result.घुसाउ(bit), found = true;
		फर्काऊ found;
	}

	अन्तःस्थ bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) अचर
	{
		bool found = false;
		यदि (signal_outputs.गन्ती(bit))
			result.घुसाउ(bit), found = true;
		फर्काऊ found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	ढाँचा<typename T>
	अन्तःस्थ bool get_drivers(pool<PortBit> &result, अचर T &bits) अचर
	{
		bool found = false;
		लागि (RTLIL::SigBit bit : bits)
			यदि (signal_drivers.गन्ती(bit)) {
				अचर pool<PortBit> &r = signal_drivers.at(bit);
				result.घुसाउ(r.begin(), r.end());
				found = true;
			}
		फर्काऊ found;
	}

	ढाँचा<typename T>
	अन्तःस्थ bool get_consumers(pool<PortBit> &result, अचर T &bits) अचर
	{
		bool found = false;
		लागि (RTLIL::SigBit bit : bits)
			यदि (signal_consumers.गन्ती(bit)) {
				अचर pool<PortBit> &r = signal_consumers.at(bit);
				result.घुसाउ(r.begin(), r.end());
				found = true;
			}
		फर्काऊ found;
	}

	ढाँचा<typename T>
	अन्तःस्थ bool get_inputs(pool<RTLIL::SigBit> &result, अचर T &bits) अचर
	{
		bool found = false;
		लागि (RTLIL::SigBit bit : bits)
			यदि (signal_inputs.गन्ती(bit))
				result.घुसाउ(bit), found = true;
		फर्काऊ found;
	}

	ढाँचा<typename T>
	अन्तःस्थ bool get_outputs(pool<RTLIL::SigBit> &result, अचर T &bits) अचर
	{
		bool found = false;
		लागि (RTLIL::SigBit bit : bits)
			यदि (signal_outputs.गन्ती(bit))
				result.घुसाउ(bit), found = true;
		फर्काऊ found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec सङ्केत) अचर
	{
		std::vector<RTLIL::SigBit> bits = sigmap(सङ्केत);
		फर्काऊ get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec सङ्केत) अचर
	{
		std::vector<RTLIL::SigBit> bits = sigmap(सङ्केत);
		फर्काऊ get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec सङ्केत) अचर
	{
		std::vector<RTLIL::SigBit> bits = sigmap(सङ्केत);
		फर्काऊ get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec सङ्केत) अचर
	{
		std::vector<RTLIL::SigBit> bits = sigmap(सङ्केत);
		फर्काऊ get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	ढाँचा<typename T>
	अन्तःस्थ bool has_drivers(अचर T &sig) अचर {
		pool<PortBit> result;
		फर्काऊ get_drivers(result, sig);
	}

	ढाँचा<typename T>
	अन्तःस्थ bool has_consumers(अचर T &sig) अचर {
		pool<PortBit> result;
		फर्काऊ get_consumers(result, sig);
	}

	ढाँचा<typename T>
	अन्तःस्थ bool has_inputs(अचर T &sig) अचर {
		pool<RTLIL::SigBit> result;
		फर्काऊ get_inputs(result, sig);
	}

	ढाँचा<typename T>
	अन्तःस्थ bool has_outputs(अचर T &sig) अचर {
		pool<RTLIL::SigBit> result;
		फर्काऊ get_outputs(result, sig);
	}

	// has_* methods -- call by value

	अन्तःस्थ bool has_drivers(RTLIL::SigSpec sig) अचर {
		pool<PortBit> result;
		फर्काऊ get_drivers(result, sig);
	}

	अन्तःस्थ bool has_consumers(RTLIL::SigSpec sig) अचर {
		pool<PortBit> result;
		फर्काऊ get_consumers(result, sig);
	}

	अन्तःस्थ bool has_inputs(RTLIL::SigSpec sig) अचर {
		pool<RTLIL::SigBit> result;
		फर्काऊ get_inputs(result, sig);
	}

	अन्तःस्थ bool has_outputs(RTLIL::SigSpec sig) अचर {
		pool<RTLIL::SigBit> result;
		फर्काऊ get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
