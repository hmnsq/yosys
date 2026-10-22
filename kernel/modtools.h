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
			जर (wire)
				offset = s.offset;
			अन्यथा
				data = s.data;
		}
		ओळीत bool चालक<(स्थिरांक RTLIL::SigBit &other) स्थिरांक {
			जर (wire == other.wire)
				परत wire ? (offset < other.offset) : (data < other.data);
			जर (wire != nullptr && other.wire != nullptr)
				परत wire < other.wire; // look here
			परत (wire != nullptr) < (other.wire != nullptr);
		}
	};
	संरचना PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		पूर्णांक offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, पूर्णांक _o) : cell(_c), port(_p), offset(_o) { }

		bool चालक<(स्थिरांक PortInfo &other) स्थिरांक {
			जर (cell != other.cell)
				परत cell < other.cell;
			जर (offset != other.offset)
				परत offset < other.offset;
			परत port < other.port;
		}

		bool चालक==(स्थिरांक PortInfo &other) स्थिरांक {
			परत cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			परत h;
		}
	};

	संरचना SigBitInfo
	{
		bool is_आदान, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_आदान(false), is_output(false) { }

		bool चालक==(स्थिरांक SigBitInfo &other) स्थिरांक {
			परत is_आदान == other.is_आदान && is_output == other.is_output && ports == other.ports;
		}

		शून्य merge(स्थिरांक SigBitInfo &other)
		{
			is_आदान = is_आदान || other.is_आदान;
			is_output = is_output || other.is_output;
			ports.घाला(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	पूर्णांक auto_reload_counter;
	bool auto_reload_module;

	शून्य port_add(RTLIL::Cell *cell, RTLIL::IdString port, स्थिरांक RTLIL::SigSpec &sig)
	{
		साठी (पूर्णांक i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			जर (bit.wire)
				database[bit].ports.घाला(PortInfo(cell, port, i));
		}
	}

	शून्य port_del(RTLIL::Cell *cell, RTLIL::IdString port, स्थिरांक RTLIL::SigSpec &sig)
	{
		साठी (पूर्णांक i = 0; i < GetSize(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			जर (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				जर (!database[bit].is_आदान && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	स्थिरांक SigBitInfo &info(RTLIL::SigBit bit)
	{
		परत database[sigmap(bit)];
	}

	शून्य reload_module(bool reset_sigmap = true)
	{
		जर (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		साठी (स्वतः wire : module->wires())
			जर (wire->port_आदान || wire->port_output)
				साठी (पूर्णांक i = 0; i < GetSize(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					जर (bit.wire && wire->port_आदान)
						database[bit].is_आदान = true;
					जर (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		साठी (स्वतः cell : module->cells())
			साठी (स्वतः &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		जर (auto_reload_module) {
			जर (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ifndef न_संशोधन
		जर (auto_reload_module)
			परत true;

		साठी (स्वतः it : database)
			log_assert(it.first == sigmap(it.first));

		स्वतः database_bak = std::move(database);
		reload_module(false);

		जर (!(database == database_bak))
		{
			साठी (स्वतः &it : database_bak)
				जर (!database.मोजणी(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			साठी (स्वतः &it : database)
				जर (!database_bak.मोजणी(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				अन्यथा जर (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			परत false;
		}
#endif
		परत true;
	}

	शून्य check()
	{
		log_assert(ok());
	}

	शून्य notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, स्थिरांक RTLIL::SigSpec &old_sig, स्थिरांक RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		जर (auto_reload_module)
			परत;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	शून्य notify_connect(RTLIL::Module *mod, स्थिरांक RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		जर (auto_reload_module)
			परत;

		साठी (पूर्णांक i = 0; i < GetSize(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.मोजणी(lhs) != 0;
			bool has_rhs = database.मोजणी(rhs) != 0;

			जर (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} अन्यथा
			जर (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				जर (lhs.wire)
					database[lhs] = new_info;
			} अन्यथा
			जर (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				जर (rhs.wire)
					database[rhs] = new_info;
			} अन्यथा {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				जर (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	शून्य notify_connect(RTLIL::Module *mod, स्थिरांक std::vector<RTLIL::SigSig>&) override
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
		module->monitors.घाला(this);
	}

	~ModIndex()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		जर (auto_reload_module)
			reload_module();

		स्वतः it = database.find(sigmap(bit));
		जर (it == database.end())
			परत nullptr;
		अन्यथा
			परत &it->second;
	}

	bool query_is_आदान(RTLIL::SigBit bit)
	{
		स्थिरांक SigBitInfo *info = query(bit);
		जर (info == nullptr)
			परत false;
		परत info->is_आदान;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		स्थिरांक SigBitInfo *info = query(bit);
		जर (info == nullptr)
			परत false;
		परत info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		स्थिर pool<PortInfo> empty_result_set;
		SigBitInfo *info = query(bit);
		जर (info == nullptr)
			परत empty_result_set;
		परत info->ports;
	}

	शून्य dump_db()
	{
		log("--- ModIndex Dump ---\n");

		जर (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		साठी (स्वतः &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			जर (it.second.is_आदान)
				log("  PRIMARY INPUT\n");
			जर (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			साठी (स्वतः &port : it.second.ports)
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
		पूर्णांक offset;
		PortBit(Cell* c, IdString p, पूर्णांक o) : cell(c), port(p), offset(o) {}

		bool चालक<(स्थिरांक PortBit &other) स्थिरांक {
			जर (cell != other.cell)
				परत cell < other.cell;
			जर (port != other.port)
				परत port < other.port;
			परत offset < other.offset;
		}

		bool चालक==(स्थिरांक PortBit &other) स्थिरांक {
			परत cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			परत h;
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
		जर (wire->port_आदान) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			साठी (स्वतः bit : bits)
				जर (bit.wire != रिक्त)
					signal_inputs.घाला(bit);
		}

		जर (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			साठी (स्वतः bit : bits)
				जर (bit.wire != रिक्त)
					signal_outputs.घाला(bit);
		}
	}

	शून्य add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_आदान)
	{
		साठी (पूर्णांक i = 0; i < पूर्णांक(bits.size()); i++)
			जर (bits[i].wire != रिक्त) {
				PortBit pbit {cell, port, i};
				जर (is_output) {
					signal_drivers[bits[i]].घाला(pbit);
					cell_outputs[cell].घाला(bits[i]);
				}
				जर (is_आदान) {
					signal_consumers[bits[i]].घाला(pbit);
					cell_inputs[cell].घाला(bits[i]);
				}
			}
	}

	शून्य add_cell(RTLIL::Cell *cell)
	{
		जर (ct.cell_known(cell->type)) {
			साठी (स्वतः &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_आदान(cell->type, conn.first));
		} अन्यथा {
			साठी (स्वतः &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(रिक्त)
	{
		ct.setup(design);
		जर (module)
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

		साठी (स्वतः &it : module->wires_)
			add_wire(it.second);
		साठी (स्वतः &it : module->cells_)
			जर (filter_ct == रिक्त || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	ओळीत bool get_drivers(pool<PortBit> &result, RTLIL::SigBit bit) स्थिरांक
	{
		bool found = false;
		जर (signal_drivers.मोजणी(bit)) {
			स्थिरांक pool<PortBit> &r = signal_drivers.at(bit);
			result.घाला(r.begin(), r.end());
			found = true;
		}
		परत found;
	}

	ओळीत bool get_consumers(pool<PortBit> &result, RTLIL::SigBit bit) स्थिरांक
	{
		bool found = false;
		जर (signal_consumers.मोजणी(bit)) {
			स्थिरांक pool<PortBit> &r = signal_consumers.at(bit);
			result.घाला(r.begin(), r.end());
			found = true;
		}
		परत found;
	}

	ओळीत bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) स्थिरांक
	{
		bool found = false;
		जर (signal_inputs.मोजणी(bit))
			result.घाला(bit), found = true;
		परत found;
	}

	ओळीत bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigBit bit) स्थिरांक
	{
		bool found = false;
		जर (signal_outputs.मोजणी(bit))
			result.घाला(bit), found = true;
		परत found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	ढाँचा<typename T>
	ओळीत bool get_drivers(pool<PortBit> &result, स्थिरांक T &bits) स्थिरांक
	{
		bool found = false;
		साठी (RTLIL::SigBit bit : bits)
			जर (signal_drivers.मोजणी(bit)) {
				स्थिरांक pool<PortBit> &r = signal_drivers.at(bit);
				result.घाला(r.begin(), r.end());
				found = true;
			}
		परत found;
	}

	ढाँचा<typename T>
	ओळीत bool get_consumers(pool<PortBit> &result, स्थिरांक T &bits) स्थिरांक
	{
		bool found = false;
		साठी (RTLIL::SigBit bit : bits)
			जर (signal_consumers.मोजणी(bit)) {
				स्थिरांक pool<PortBit> &r = signal_consumers.at(bit);
				result.घाला(r.begin(), r.end());
				found = true;
			}
		परत found;
	}

	ढाँचा<typename T>
	ओळीत bool get_inputs(pool<RTLIL::SigBit> &result, स्थिरांक T &bits) स्थिरांक
	{
		bool found = false;
		साठी (RTLIL::SigBit bit : bits)
			जर (signal_inputs.मोजणी(bit))
				result.घाला(bit), found = true;
		परत found;
	}

	ढाँचा<typename T>
	ओळीत bool get_outputs(pool<RTLIL::SigBit> &result, स्थिरांक T &bits) स्थिरांक
	{
		bool found = false;
		साठी (RTLIL::SigBit bit : bits)
			जर (signal_outputs.मोजणी(bit))
				result.घाला(bit), found = true;
		परत found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &result, RTLIL::SigSpec संकेत) स्थिरांक
	{
		std::vector<RTLIL::SigBit> bits = sigmap(संकेत);
		परत get_drivers(result, bits);
	}

	bool get_consumers(pool<PortBit> &result, RTLIL::SigSpec संकेत) स्थिरांक
	{
		std::vector<RTLIL::SigBit> bits = sigmap(संकेत);
		परत get_consumers(result, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec संकेत) स्थिरांक
	{
		std::vector<RTLIL::SigBit> bits = sigmap(संकेत);
		परत get_inputs(result, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &result, RTLIL::SigSpec संकेत) स्थिरांक
	{
		std::vector<RTLIL::SigBit> bits = sigmap(संकेत);
		परत get_outputs(result, bits);
	}

	// has_* methods -- call by reference

	ढाँचा<typename T>
	ओळीत bool has_drivers(स्थिरांक T &sig) स्थिरांक {
		pool<PortBit> result;
		परत get_drivers(result, sig);
	}

	ढाँचा<typename T>
	ओळीत bool has_consumers(स्थिरांक T &sig) स्थिरांक {
		pool<PortBit> result;
		परत get_consumers(result, sig);
	}

	ढाँचा<typename T>
	ओळीत bool has_inputs(स्थिरांक T &sig) स्थिरांक {
		pool<RTLIL::SigBit> result;
		परत get_inputs(result, sig);
	}

	ढाँचा<typename T>
	ओळीत bool has_outputs(स्थिरांक T &sig) स्थिरांक {
		pool<RTLIL::SigBit> result;
		परत get_outputs(result, sig);
	}

	// has_* methods -- call by value

	ओळीत bool has_drivers(RTLIL::SigSpec sig) स्थिरांक {
		pool<PortBit> result;
		परत get_drivers(result, sig);
	}

	ओळीत bool has_consumers(RTLIL::SigSpec sig) स्थिरांक {
		pool<PortBit> result;
		परत get_consumers(result, sig);
	}

	ओळीत bool has_inputs(RTLIL::SigSpec sig) स्थिरांक {
		pool<RTLIL::SigBit> result;
		परत get_inputs(result, sig);
	}

	ओळीत bool has_outputs(RTLIL::SigSpec sig) स्थिरांक {
		pool<RTLIL::SigBit> result;
		परत get_outputs(result, sig);
	}
};

YOSYS_NAMESPACE_END

#endif
