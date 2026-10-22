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

#अगर_अघोषित MODTOOLS_H
#घोषणा MODTOOLS_H

#समावेश "kernel/yosys.h"
#समावेश "kernel/sigtools.h"
#समावेश "kernel/celltypes.h"
#समावेश "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

काष्ठा Mod_सूचकांक : खुला RTLIL::Monitor
{
	काष्ठा PointerOrderedSigBit : खुला RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			अगर (wire)
				offset = s.offset;
			अन्यथा
				तथ्य = s.तथ्य;
		}
		अंतरभूत bool चालक<(स्थिर RTLIL::SigBit &other) स्थिर {
			अगर (wire == other.wire)
				वापस wire ? (offset < other.offset) : (तथ्य < other.तथ्य);
			अगर (wire != nullptr && other.wire != nullptr)
				वापस wire < other.wire; // look here
			वापस (wire != nullptr) < (other.wire != nullptr);
		}
	};
	काष्ठा PortInfo {
		RTLIL::Cell* cell;
		RTLIL::IdString port;
		पूर्णांक offset;

		PortInfo() : cell(), port(), offset() { }
		PortInfo(RTLIL::Cell* _c, RTLIL::IdString _p, पूर्णांक _o) : cell(_c), port(_p), offset(_o) { }

		bool चालक<(स्थिर PortInfo &other) स्थिर {
			अगर (cell != other.cell)
				वापस cell < other.cell;
			अगर (offset != other.offset)
				वापस offset < other.offset;
			वापस port < other.port;
		}

		bool चालक==(स्थिर PortInfo &other) स्थिर {
			वापस cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			वापस h;
		}
	};

	काष्ठा SigBitInfo
	{
		bool is_निवेश, is_output;
		pool<PortInfo> ports;

		SigBitInfo() : is_निवेश(false), is_output(false) { }

		bool चालक==(स्थिर SigBitInfo &other) स्थिर {
			वापस is_निवेश == other.is_निवेश && is_output == other.is_output && ports == other.ports;
		}

		व्योम merge(स्थिर SigBitInfo &other)
		{
			is_निवेश = is_निवेश || other.is_निवेश;
			is_output = is_output || other.is_output;
			ports.जोड़ो(other.ports.begin(), other.ports.end());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	पूर्णांक auto_reload_गणक;
	bool auto_reload_module;

	व्योम port_add(RTLIL::Cell *cell, RTLIL::IdString port, स्थिर RTLIL::SigSpec &sig)
	{
		क्रम (पूर्णांक i = 0; i < Get_आकार(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			अगर (bit.wire)
				database[bit].ports.जोड़ो(PortInfo(cell, port, i));
		}
	}

	व्योम port_del(RTLIL::Cell *cell, RTLIL::IdString port, स्थिर RTLIL::SigSpec &sig)
	{
		क्रम (पूर्णांक i = 0; i < Get_आकार(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			अगर (bit.wire) {
				database[bit].ports.erase(PortInfo(cell, port, i));
				अगर (!database[bit].is_निवेश && !database[bit].is_output && database[bit].ports.empty())
					database.erase(bit);
			}
		}
	}

	स्थिर SigBitInfo &info(RTLIL::SigBit bit)
	{
		वापस database[sigmap(bit)];
	}

	व्योम reload_module(bool reset_sigmap = true)
	{
		अगर (reset_sigmap) {
			sigmap.clear();
			sigmap.set(module);
		}

		database.clear();
		क्रम (स्वतः wire : module->wires())
			अगर (wire->port_निवेश || wire->port_output)
				क्रम (पूर्णांक i = 0; i < Get_आकार(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					अगर (bit.wire && wire->port_निवेश)
						database[bit].is_निवेश = true;
					अगर (bit.wire && wire->port_output)
						database[bit].is_output = true;
				}
		क्रम (स्वतः cell : module->cells())
			क्रम (स्वतः &conn : cell->connections())
				port_add(cell, conn.first, conn.second);

		अगर (auto_reload_module) {
			अगर (++auto_reload_गणक > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#अगर_अघोषित न_संशोधन
		अगर (auto_reload_module)
			वापस true;

		क्रम (स्वतः it : database)
			log_assert(it.first == sigmap(it.first));

		स्वतः database_bak = std::move(database);
		reload_module(false);

		अगर (!(database == database_bak))
		{
			क्रम (स्वतः &it : database_bak)
				अगर (!database.गिनती(it.first))
					log("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			क्रम (स्वतः &it : database)
				अगर (!database_bak.गिनती(it.first))
					log("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				अन्यथा अगर (!(it.second == database_bak.at(it.first)))
					log("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			वापस false;
		}
#पूर्ण_अगर
		वापस true;
	}

	व्योम check()
	{
		log_assert(ok());
	}

	व्योम notify_connect(RTLIL::Cell *cell, RTLIL::IdString port, स्थिर RTLIL::SigSpec &old_sig, स्थिर RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		अगर (auto_reload_module)
			वापस;

		port_del(cell, port, old_sig);
		port_add(cell, port, sig);
	}

	व्योम notify_connect(RTLIL::Module *mod, स्थिर RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		अगर (auto_reload_module)
			वापस;

		क्रम (पूर्णांक i = 0; i < Get_आकार(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.गिनती(lhs) != 0;
			bool has_rhs = database.गिनती(rhs) != 0;

			अगर (!has_lhs && !has_rhs) {
				sigmap.add(lhs, rhs);
			} अन्यथा
			अगर (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.add(lhs, rhs);
				lhs = sigmap(lhs);
				अगर (lhs.wire)
					database[lhs] = new_info;
			} अन्यथा
			अगर (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				अगर (rhs.wire)
					database[rhs] = new_info;
			} अन्यथा {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.add(lhs, rhs);
				rhs = sigmap(rhs);
				अगर (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	व्योम notify_connect(RTLIL::Module *mod, स्थिर std::vector<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	व्योम notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	Mod_सूचकांक(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_गणक = 0;
		auto_reload_module = true;
		module->monitors.जोड़ो(this);
	}

	~Mod_सूचकांक()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		अगर (auto_reload_module)
			reload_module();

		स्वतः it = database.find(sigmap(bit));
		अगर (it == database.end())
			वापस nullptr;
		अन्यथा
			वापस &it->second;
	}

	bool query_is_निवेश(RTLIL::SigBit bit)
	{
		स्थिर SigBitInfo *info = query(bit);
		अगर (info == nullptr)
			वापस false;
		वापस info->is_निवेश;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		स्थिर SigBitInfo *info = query(bit);
		अगर (info == nullptr)
			वापस false;
		वापस info->is_output;
	}

	pool<PortInfo> &query_ports(RTLIL::SigBit bit)
	{
		अटल pool<PortInfo> empty_परिणाम_set;
		SigBitInfo *info = query(bit);
		अगर (info == nullptr)
			वापस empty_परिणाम_set;
		वापस info->ports;
	}

	व्योम dump_db()
	{
		log("--- ModIndex Dump ---\n");

		अगर (auto_reload_module) {
			log("AUTO-RELOAD\n");
			reload_module();
		}

		क्रम (स्वतः &it : database) {
			log("BIT %s:\n", log_signal(it.first));
			अगर (it.second.is_निवेश)
				log("  PRIMARY INPUT\n");
			अगर (it.second.is_output)
				log("  PRIMARY OUTPUT\n");
			क्रम (स्वतः &port : it.second.ports)
				log("  PORT: %s.%s[%d] (%s)\n", port.cell,
						PooledName(module, port.port).unescape(), port.offset, port.cell->type.unescape());
		}
	}
};

काष्ठा ModWalker
{
	काष्ठा PortBit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString port;
		पूर्णांक offset;
		PortBit(Cell* c, IdString p, पूर्णांक o) : cell(c), port(p), offset(o) {}

		bool चालक<(स्थिर PortBit &other) स्थिर {
			अगर (cell != other.cell)
				वापस cell < other.cell;
			अगर (port != other.port)
				वापस port < other.port;
			वापस offset < other.offset;
		}

		bool चालक==(स्थिर PortBit &other) स्थिर {
			वापस cell == other.cell && port == other.port && offset == other.offset;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
			h.eat(cell->name);
			h.eat(port);
			h.eat(offset);
			वापस h;
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

	व्योम add_wire(RTLIL::Wire *wire)
	{
		अगर (wire->port_निवेश) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			क्रम (स्वतः bit : bits)
				अगर (bit.wire != शून्य)
					signal_inputs.जोड़ो(bit);
		}

		अगर (wire->port_output) {
			std::vector<RTLIL::SigBit> bits = sigmap(wire);
			क्रम (स्वतः bit : bits)
				अगर (bit.wire != शून्य)
					signal_outputs.जोड़ो(bit);
		}
	}

	व्योम add_cell_port(RTLIL::Cell *cell, RTLIL::IdString port, std::vector<RTLIL::SigBit> bits, bool is_output, bool is_निवेश)
	{
		क्रम (पूर्णांक i = 0; i < पूर्णांक(bits.आकार()); i++)
			अगर (bits[i].wire != शून्य) {
				PortBit pbit {cell, port, i};
				अगर (is_output) {
					signal_drivers[bits[i]].जोड़ो(pbit);
					cell_outputs[cell].जोड़ो(bits[i]);
				}
				अगर (is_निवेश) {
					signal_consumers[bits[i]].जोड़ो(pbit);
					cell_inputs[cell].जोड़ो(bits[i]);
				}
			}
	}

	व्योम add_cell(RTLIL::Cell *cell)
	{
		अगर (ct.cell_known(cell->type)) {
			क्रम (स्वतः &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_निवेश(cell->type, conn.first));
		} अन्यथा {
			क्रम (स्वतः &conn : cell->connections())
				add_cell_port(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(शून्य)
	{
		ct.setup(design);
		अगर (module)
			setup(module);
	}

	व्योम setup(RTLIL::Module *module, CellTypes *filter_ct = शून्य)
	{
		this->module = module;

		sigmap.set(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		क्रम (स्वतः &it : module->wires_)
			add_wire(it.second);
		क्रम (स्वतः &it : module->cells_)
			अगर (filter_ct == शून्य || filter_ct->cell_known(it.second->type))
				add_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	अंतरभूत bool get_drivers(pool<PortBit> &परिणाम, RTLIL::SigBit bit) स्थिर
	{
		bool found = false;
		अगर (signal_drivers.गिनती(bit)) {
			स्थिर pool<PortBit> &r = signal_drivers.at(bit);
			परिणाम.जोड़ो(r.begin(), r.end());
			found = true;
		}
		वापस found;
	}

	अंतरभूत bool get_consumers(pool<PortBit> &परिणाम, RTLIL::SigBit bit) स्थिर
	{
		bool found = false;
		अगर (signal_consumers.गिनती(bit)) {
			स्थिर pool<PortBit> &r = signal_consumers.at(bit);
			परिणाम.जोड़ो(r.begin(), r.end());
			found = true;
		}
		वापस found;
	}

	अंतरभूत bool get_inputs(pool<RTLIL::SigBit> &परिणाम, RTLIL::SigBit bit) स्थिर
	{
		bool found = false;
		अगर (signal_inputs.गिनती(bit))
			परिणाम.जोड़ो(bit), found = true;
		वापस found;
	}

	अंतरभूत bool get_outputs(pool<RTLIL::SigBit> &परिणाम, RTLIL::SigBit bit) स्थिर
	{
		bool found = false;
		अगर (signal_outputs.गिनती(bit))
			परिणाम.जोड़ो(bit), found = true;
		वापस found;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	ढाँचा<typename T>
	अंतरभूत bool get_drivers(pool<PortBit> &परिणाम, स्थिर T &bits) स्थिर
	{
		bool found = false;
		क्रम (RTLIL::SigBit bit : bits)
			अगर (signal_drivers.गिनती(bit)) {
				स्थिर pool<PortBit> &r = signal_drivers.at(bit);
				परिणाम.जोड़ो(r.begin(), r.end());
				found = true;
			}
		वापस found;
	}

	ढाँचा<typename T>
	अंतरभूत bool get_consumers(pool<PortBit> &परिणाम, स्थिर T &bits) स्थिर
	{
		bool found = false;
		क्रम (RTLIL::SigBit bit : bits)
			अगर (signal_consumers.गिनती(bit)) {
				स्थिर pool<PortBit> &r = signal_consumers.at(bit);
				परिणाम.जोड़ो(r.begin(), r.end());
				found = true;
			}
		वापस found;
	}

	ढाँचा<typename T>
	अंतरभूत bool get_inputs(pool<RTLIL::SigBit> &परिणाम, स्थिर T &bits) स्थिर
	{
		bool found = false;
		क्रम (RTLIL::SigBit bit : bits)
			अगर (signal_inputs.गिनती(bit))
				परिणाम.जोड़ो(bit), found = true;
		वापस found;
	}

	ढाँचा<typename T>
	अंतरभूत bool get_outputs(pool<RTLIL::SigBit> &परिणाम, स्थिर T &bits) स्थिर
	{
		bool found = false;
		क्रम (RTLIL::SigBit bit : bits)
			अगर (signal_outputs.गिनती(bit))
				परिणाम.जोड़ो(bit), found = true;
		वापस found;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<PortBit> &परिणाम, RTLIL::SigSpec संकेत) स्थिर
	{
		std::vector<RTLIL::SigBit> bits = sigmap(संकेत);
		वापस get_drivers(परिणाम, bits);
	}

	bool get_consumers(pool<PortBit> &परिणाम, RTLIL::SigSpec संकेत) स्थिर
	{
		std::vector<RTLIL::SigBit> bits = sigmap(संकेत);
		वापस get_consumers(परिणाम, bits);
	}

	bool get_inputs(pool<RTLIL::SigBit> &परिणाम, RTLIL::SigSpec संकेत) स्थिर
	{
		std::vector<RTLIL::SigBit> bits = sigmap(संकेत);
		वापस get_inputs(परिणाम, bits);
	}

	bool get_outputs(pool<RTLIL::SigBit> &परिणाम, RTLIL::SigSpec संकेत) स्थिर
	{
		std::vector<RTLIL::SigBit> bits = sigmap(संकेत);
		वापस get_outputs(परिणाम, bits);
	}

	// has_* methods -- call by reference

	ढाँचा<typename T>
	अंतरभूत bool has_drivers(स्थिर T &sig) स्थिर {
		pool<PortBit> परिणाम;
		वापस get_drivers(परिणाम, sig);
	}

	ढाँचा<typename T>
	अंतरभूत bool has_consumers(स्थिर T &sig) स्थिर {
		pool<PortBit> परिणाम;
		वापस get_consumers(परिणाम, sig);
	}

	ढाँचा<typename T>
	अंतरभूत bool has_inputs(स्थिर T &sig) स्थिर {
		pool<RTLIL::SigBit> परिणाम;
		वापस get_inputs(परिणाम, sig);
	}

	ढाँचा<typename T>
	अंतरभूत bool has_outputs(स्थिर T &sig) स्थिर {
		pool<RTLIL::SigBit> परिणाम;
		वापस get_outputs(परिणाम, sig);
	}

	// has_* methods -- call by value

	अंतरभूत bool has_drivers(RTLIL::SigSpec sig) स्थिर {
		pool<PortBit> परिणाम;
		वापस get_drivers(परिणाम, sig);
	}

	अंतरभूत bool has_consumers(RTLIL::SigSpec sig) स्थिर {
		pool<PortBit> परिणाम;
		वापस get_consumers(परिणाम, sig);
	}

	अंतरभूत bool has_inputs(RTLIL::SigSpec sig) स्थिर {
		pool<RTLIL::SigBit> परिणाम;
		वापस get_inputs(परिणाम, sig);
	}

	अंतरभूत bool has_outputs(RTLIL::SigSpec sig) स्थिर {
		pool<RTLIL::SigBit> परिणाम;
		वापस get_outputs(परिणाम, sig);
	}
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
