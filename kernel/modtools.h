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

#यदि_अपरिभाषितम् MODTOOLS_H
#परिभाषय MODTOOLS_H

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/sigtools.h"
#अन्तर्भाव "kernel/celltypes.h"
#अन्तर्भाव "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

संरचना Mod_सूचकांक : सार्वजनिक RTLIL::Monitor
{
	संरचना PointerOrderedSigBit : सार्वजनिक RTLIL::SigBit {
		PointerOrderedSigBit(SigBit s) {
			wire = s.wire;
			यदि (wire)
				विस्थापनम् = s.विस्थापनम्;
			अन्यथा
				दत्त = s.दत्त;
		}
		अन्तःस्थ bool संक्रियकः<(ध्रुव RTLIL::SigBit &other) ध्रुव {
			यदि (wire == other.wire)
				प्रत्यागम wire ? (विस्थापनम् < other.विस्थापनम्) : (दत्त < other.दत्त);
			यदि (wire != nullptr && other.wire != nullptr)
				प्रत्यागम wire < other.wire; // look here
			प्रत्यागम (wire != nullptr) < (other.wire != nullptr);
		}
	};
	संरचना पत्तनम्_Info {
		RTLIL::Cell* cell;
		RTLIL::IdString पत्तनम्;
		पूर्णाङ्क विस्थापनम्;

		पत्तनम्_Info() : cell(), पत्तनम्(), विस्थापनम्() { }
		पत्तनम्_Info(RTLIL::Cell* _c, RTLIL::IdString _p, पूर्णाङ्क _o) : cell(_c), पत्तनम्(_p), विस्थापनम्(_o) { }

		bool संक्रियकः<(ध्रुव पत्तनम्_Info &other) ध्रुव {
			यदि (cell != other.cell)
				प्रत्यागम cell < other.cell;
			यदि (विस्थापनम् != other.विस्थापनम्)
				प्रत्यागम विस्थापनम् < other.विस्थापनम्;
			प्रत्यागम पत्तनम् < other.पत्तनम्;
		}

		bool संक्रियकः==(ध्रुव पत्तनम्_Info &other) ध्रुव {
			प्रत्यागम cell == other.cell && पत्तनम् == other.पत्तनम् && विस्थापनम् == other.विस्थापनम्;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(cell->name);
			h.eat(पत्तनम्);
			h.eat(विस्थापनम्);
			प्रत्यागम h;
		}
	};

	संरचना SigBitInfo
	{
		bool is_निवेशः, is_output;
		pool<पत्तनम्_Info> ports;

		SigBitInfo() : is_निवेशः(false), is_output(false) { }

		bool संक्रियकः==(ध्रुव SigBitInfo &other) ध्रुव {
			प्रत्यागम is_निवेशः == other.is_निवेशः && is_output == other.is_output && ports == other.ports;
		}

		शून्य merge(ध्रुव SigBitInfo &other)
		{
			is_निवेशः = is_निवेशः || other.is_निवेशः;
			is_output = is_output || other.is_output;
			ports.निवेशय(other.ports.begin(), other.ports.अन्तः());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<PointerOrderedSigBit, SigBitInfo> database;
	पूर्णाङ्क auto_reload_गणक;
	bool auto_reload_module;

	शून्य पत्तनम्_सङ्कलय(RTLIL::Cell *cell, RTLIL::IdString पत्तनम्, ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			यदि (bit.wire)
				database[bit].ports.निवेशय(पत्तनम्_Info(cell, पत्तनम्, i));
		}
	}

	शून्य पत्तनम्_del(RTLIL::Cell *cell, RTLIL::IdString पत्तनम्, ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(sig); i++) {
			RTLIL::SigBit bit = sigmap(sig[i]);
			यदि (bit.wire) {
				database[bit].ports.erase(पत्तनम्_Info(cell, पत्तनम्, i));
				यदि (!database[bit].is_निवेशः && !database[bit].is_output && database[bit].ports.रिक्तानि())
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
			sigmap.समुच्चयः(module);
		}

		database.clear();
		प्रति (स्वचालित wire : module->wires())
			यदि (wire->पत्तनम्_निवेशः || wire->पत्तनम्_output)
				प्रति (पूर्णाङ्क i = 0; i < Get_आकार(wire); i++) {
					RTLIL::SigBit bit = sigmap(RTLIL::SigBit(wire, i));
					यदि (bit.wire && wire->पत्तनम्_निवेशः)
						database[bit].is_निवेशः = true;
					यदि (bit.wire && wire->पत्तनम्_output)
						database[bit].is_output = true;
				}
		प्रति (स्वचालित cell : module->cells())
			प्रति (स्वचालित &conn : cell->connections())
				पत्तनम्_सङ्कलय(cell, conn.first, conn.second);

		यदि (auto_reload_module) {
			यदि (++auto_reload_गणक > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#यदि_अपरिभाषितम् NDEBUG
		यदि (auto_reload_module)
			प्रत्यागम true;

		प्रति (स्वचालित it : database)
			log_assert(it.first == sigmap(it.first));

		स्वचालित database_bak = std::move(database);
		reload_module(false);

		यदि (!(database == database_bak))
		{
			प्रति (स्वचालित &it : database_bak)
				यदि (!database.गणनम्(it.first))
					लघुगणकः("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.first));

			प्रति (स्वचालित &it : database)
				यदि (!database_bak.गणनम्(it.first))
					लघुगणकः("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.first));
				अन्यथा यदि (!(it.second == database_bak.at(it.first)))
					लघुगणकः("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.first));

			प्रत्यागम false;
		}
#यदि_अन्तः
		प्रत्यागम true;
	}

	शून्य check()
	{
		log_assert(ok());
	}

	शून्य notify_connect(RTLIL::Cell *cell, RTLIL::IdString पत्तनम्, ध्रुव RTLIL::SigSpec &old_sig, ध्रुव RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		यदि (auto_reload_module)
			प्रत्यागम;

		पत्तनम्_del(cell, पत्तनम्, old_sig);
		पत्तनम्_सङ्कलय(cell, पत्तनम्, sig);
	}

	शून्य notify_connect(RTLIL::Module *mod, ध्रुव RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		यदि (auto_reload_module)
			प्रत्यागम;

		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(sigsig.first); i++)
		{
			RTLIL::SigBit lhs = sigmap(sigsig.first[i]);
			RTLIL::SigBit rhs = sigmap(sigsig.second[i]);
			bool has_lhs = database.गणनम्(lhs) != 0;
			bool has_rhs = database.गणनम्(rhs) != 0;

			यदि (!has_lhs && !has_rhs) {
				sigmap.सङ्कलय(lhs, rhs);
			} अन्यथा
			यदि (!has_rhs) {
				SigBitInfo new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.सङ्कलय(lhs, rhs);
				lhs = sigmap(lhs);
				यदि (lhs.wire)
					database[lhs] = new_info;
			} अन्यथा
			यदि (!has_lhs) {
				SigBitInfo new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.सङ्कलय(lhs, rhs);
				rhs = sigmap(rhs);
				यदि (rhs.wire)
					database[rhs] = new_info;
			} अन्यथा {
				SigBitInfo new_info = database.at(lhs);
				new_info.merge(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.सङ्कलय(lhs, rhs);
				rhs = sigmap(rhs);
				यदि (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	शून्य notify_connect(RTLIL::Module *mod, ध्रुव std::सदिशम्<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	शून्य notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	Mod_सूचकांक(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_गणक = 0;
		auto_reload_module = true;
		module->monitors.निवेशय(this);
	}

	~Mod_सूचकांक()
	{
		module->monitors.erase(this);
	}

	SigBitInfo *query(RTLIL::SigBit bit)
	{
		यदि (auto_reload_module)
			reload_module();

		स्वचालित it = database.find(sigmap(bit));
		यदि (it == database.अन्तः())
			प्रत्यागम nullptr;
		अन्यथा
			प्रत्यागम &it->second;
	}

	bool query_is_निवेशः(RTLIL::SigBit bit)
	{
		ध्रुव SigBitInfo *info = query(bit);
		यदि (info == nullptr)
			प्रत्यागम false;
		प्रत्यागम info->is_निवेशः;
	}

	bool query_is_output(RTLIL::SigBit bit)
	{
		ध्रुव SigBitInfo *info = query(bit);
		यदि (info == nullptr)
			प्रत्यागम false;
		प्रत्यागम info->is_output;
	}

	pool<पत्तनम्_Info> &query_ports(RTLIL::SigBit bit)
	{
		स्थिर pool<पत्तनम्_Info> रिक्तानि_परिणाम_समुच्चयः;
		SigBitInfo *info = query(bit);
		यदि (info == nullptr)
			प्रत्यागम रिक्तानि_परिणाम_समुच्चयः;
		प्रत्यागम info->ports;
	}

	शून्य dump_db()
	{
		लघुगणकः("--- ModIndex Dump ---\n");

		यदि (auto_reload_module) {
			लघुगणकः("AUTO-RELOAD\n");
			reload_module();
		}

		प्रति (स्वचालित &it : database) {
			लघुगणकः("BIT %s:\n", log_signal(it.first));
			यदि (it.second.is_निवेशः)
				लघुगणकः("  PRIMARY INPUT\n");
			यदि (it.second.is_output)
				लघुगणकः("  PRIMARY OUTPUT\n");
			प्रति (स्वचालित &पत्तनम् : it.second.ports)
				लघुगणकः("  PORT: %s.%s[%d] (%s)\n", पत्तनम्.cell,
						PooledName(module, पत्तनम्.पत्तनम्).unescape(), पत्तनम्.विस्थापनम्, पत्तनम्.cell->type.unescape());
		}
	}
};

संरचना ModWalker
{
	संरचना पत्तनम्_Bit
	{
		RTLIL::Cell *cell;
		RTLIL::IdString पत्तनम्;
		पूर्णाङ्क विस्थापनम्;
		पत्तनम्_Bit(Cell* c, IdString p, पूर्णाङ्क o) : cell(c), पत्तनम्(p), विस्थापनम्(o) {}

		bool संक्रियकः<(ध्रुव पत्तनम्_Bit &other) ध्रुव {
			यदि (cell != other.cell)
				प्रत्यागम cell < other.cell;
			यदि (पत्तनम् != other.पत्तनम्)
				प्रत्यागम पत्तनम् < other.पत्तनम्;
			प्रत्यागम विस्थापनम् < other.विस्थापनम्;
		}

		bool संक्रियकः==(ध्रुव पत्तनम्_Bit &other) ध्रुव {
			प्रत्यागम cell == other.cell && पत्तनम् == other.पत्तनम् && विस्थापनम् == other.विस्थापनम्;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(cell->name);
			h.eat(पत्तनम्);
			h.eat(विस्थापनम्);
			प्रत्यागम h;
		}
	};

	RTLIL::Design *design;
	RTLIL::Module *module;

	NewCellTypes ct;
	SigMap sigmap;

	dict<RTLIL::SigBit, pool<पत्तनम्_Bit>> signal_drivers;
	dict<RTLIL::SigBit, pool<पत्तनम्_Bit>> signal_consumers;
	pool<RTLIL::SigBit> signal_inputs, signal_outputs;

	dict<RTLIL::Cell*, pool<RTLIL::SigBit>> cell_outputs, cell_inputs;

	शून्य सङ्कलय_wire(RTLIL::Wire *wire)
	{
		यदि (wire->पत्तनम्_निवेशः) {
			std::सदिशम्<RTLIL::SigBit> द्वयङ्काः = sigmap(wire);
			प्रति (स्वचालित bit : द्वयङ्काः)
				यदि (bit.wire != रिक्त)
					signal_inputs.निवेशय(bit);
		}

		यदि (wire->पत्तनम्_output) {
			std::सदिशम्<RTLIL::SigBit> द्वयङ्काः = sigmap(wire);
			प्रति (स्वचालित bit : द्वयङ्काः)
				यदि (bit.wire != रिक्त)
					signal_outputs.निवेशय(bit);
		}
	}

	शून्य सङ्कलय_cell_पत्तनम्(RTLIL::Cell *cell, RTLIL::IdString पत्तनम्, std::सदिशम्<RTLIL::SigBit> द्वयङ्काः, bool is_output, bool is_निवेशः)
	{
		प्रति (पूर्णाङ्क i = 0; i < पूर्णाङ्क(द्वयङ्काः.आकार()); i++)
			यदि (द्वयङ्काः[i].wire != रिक्त) {
				पत्तनम्_Bit pbit {cell, पत्तनम्, i};
				यदि (is_output) {
					signal_drivers[द्वयङ्काः[i]].निवेशय(pbit);
					cell_outputs[cell].निवेशय(द्वयङ्काः[i]);
				}
				यदि (is_निवेशः) {
					signal_consumers[द्वयङ्काः[i]].निवेशय(pbit);
					cell_inputs[cell].निवेशय(द्वयङ्काः[i]);
				}
			}
	}

	शून्य सङ्कलय_cell(RTLIL::Cell *cell)
	{
		यदि (ct.cell_ज्ञातम्(cell->type)) {
			प्रति (स्वचालित &conn : cell->connections())
				सङ्कलय_cell_पत्तनम्(cell, conn.first, sigmap(conn.second),
						ct.cell_output(cell->type, conn.first),
						ct.cell_निवेशः(cell->type, conn.first));
		} अन्यथा {
			प्रति (स्वचालित &conn : cell->connections())
				सङ्कलय_cell_पत्तनम्(cell, conn.first, sigmap(conn.second), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(रिक्त)
	{
		ct.सज्जता(design);
		यदि (module)
			सज्जता(module);
	}

	शून्य सज्जता(RTLIL::Module *module, CellTypes *filter_ct = रिक्त)
	{
		this->module = module;

		sigmap.समुच्चयः(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		प्रति (स्वचालित &it : module->wires_)
			सङ्कलय_wire(it.second);
		प्रति (स्वचालित &it : module->cells_)
			यदि (filter_ct == रिक्त || filter_ct->cell_ज्ञातम्(it.second->type))
				सङ्कलय_cell(it.second);
	}

	// get_* methods -- single RTLIL::SigBit

	अन्तःस्थ bool get_drivers(pool<पत्तनम्_Bit> &परिणाम, RTLIL::SigBit bit) ध्रुव
	{
		bool प्राप्तम् = false;
		यदि (signal_drivers.गणनम्(bit)) {
			ध्रुव pool<पत्तनम्_Bit> &r = signal_drivers.at(bit);
			परिणाम.निवेशय(r.begin(), r.अन्तः());
			प्राप्तम् = true;
		}
		प्रत्यागम प्राप्तम्;
	}

	अन्तःस्थ bool get_consumers(pool<पत्तनम्_Bit> &परिणाम, RTLIL::SigBit bit) ध्रुव
	{
		bool प्राप्तम् = false;
		यदि (signal_consumers.गणनम्(bit)) {
			ध्रुव pool<पत्तनम्_Bit> &r = signal_consumers.at(bit);
			परिणाम.निवेशय(r.begin(), r.अन्तः());
			प्राप्तम् = true;
		}
		प्रत्यागम प्राप्तम्;
	}

	अन्तःस्थ bool get_inputs(pool<RTLIL::SigBit> &परिणाम, RTLIL::SigBit bit) ध्रुव
	{
		bool प्राप्तम् = false;
		यदि (signal_inputs.गणनम्(bit))
			परिणाम.निवेशय(bit), प्राप्तम् = true;
		प्रत्यागम प्राप्तम्;
	}

	अन्तःस्थ bool get_outputs(pool<RTLIL::SigBit> &परिणाम, RTLIL::SigBit bit) ध्रुव
	{
		bool प्राप्तम् = false;
		यदि (signal_outputs.गणनम्(bit))
			परिणाम.निवेशय(bit), प्राप्तम् = true;
		प्रत्यागम प्राप्तम्;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	प्रतिरूपम्<typename T>
	अन्तःस्थ bool get_drivers(pool<पत्तनम्_Bit> &परिणाम, ध्रुव T &द्वयङ्काः) ध्रुव
	{
		bool प्राप्तम् = false;
		प्रति (RTLIL::SigBit bit : द्वयङ्काः)
			यदि (signal_drivers.गणनम्(bit)) {
				ध्रुव pool<पत्तनम्_Bit> &r = signal_drivers.at(bit);
				परिणाम.निवेशय(r.begin(), r.अन्तः());
				प्राप्तम् = true;
			}
		प्रत्यागम प्राप्तम्;
	}

	प्रतिरूपम्<typename T>
	अन्तःस्थ bool get_consumers(pool<पत्तनम्_Bit> &परिणाम, ध्रुव T &द्वयङ्काः) ध्रुव
	{
		bool प्राप्तम् = false;
		प्रति (RTLIL::SigBit bit : द्वयङ्काः)
			यदि (signal_consumers.गणनम्(bit)) {
				ध्रुव pool<पत्तनम्_Bit> &r = signal_consumers.at(bit);
				परिणाम.निवेशय(r.begin(), r.अन्तः());
				प्राप्तम् = true;
			}
		प्रत्यागम प्राप्तम्;
	}

	प्रतिरूपम्<typename T>
	अन्तःस्थ bool get_inputs(pool<RTLIL::SigBit> &परिणाम, ध्रुव T &द्वयङ्काः) ध्रुव
	{
		bool प्राप्तम् = false;
		प्रति (RTLIL::SigBit bit : द्वयङ्काः)
			यदि (signal_inputs.गणनम्(bit))
				परिणाम.निवेशय(bit), प्राप्तम् = true;
		प्रत्यागम प्राप्तम्;
	}

	प्रतिरूपम्<typename T>
	अन्तःस्थ bool get_outputs(pool<RTLIL::SigBit> &परिणाम, ध्रुव T &द्वयङ्काः) ध्रुव
	{
		bool प्राप्तम् = false;
		प्रति (RTLIL::SigBit bit : द्वयङ्काः)
			यदि (signal_outputs.गणनम्(bit))
				परिणाम.निवेशय(bit), प्राप्तम् = true;
		प्रत्यागम प्राप्तम्;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<पत्तनम्_Bit> &परिणाम, RTLIL::SigSpec सङ्केत) ध्रुव
	{
		std::सदिशम्<RTLIL::SigBit> द्वयङ्काः = sigmap(सङ्केत);
		प्रत्यागम get_drivers(परिणाम, द्वयङ्काः);
	}

	bool get_consumers(pool<पत्तनम्_Bit> &परिणाम, RTLIL::SigSpec सङ्केत) ध्रुव
	{
		std::सदिशम्<RTLIL::SigBit> द्वयङ्काः = sigmap(सङ्केत);
		प्रत्यागम get_consumers(परिणाम, द्वयङ्काः);
	}

	bool get_inputs(pool<RTLIL::SigBit> &परिणाम, RTLIL::SigSpec सङ्केत) ध्रुव
	{
		std::सदिशम्<RTLIL::SigBit> द्वयङ्काः = sigmap(सङ्केत);
		प्रत्यागम get_inputs(परिणाम, द्वयङ्काः);
	}

	bool get_outputs(pool<RTLIL::SigBit> &परिणाम, RTLIL::SigSpec सङ्केत) ध्रुव
	{
		std::सदिशम्<RTLIL::SigBit> द्वयङ्काः = sigmap(सङ्केत);
		प्रत्यागम get_outputs(परिणाम, द्वयङ्काः);
	}

	// has_* methods -- call by reference

	प्रतिरूपम्<typename T>
	अन्तःस्थ bool has_drivers(ध्रुव T &sig) ध्रुव {
		pool<पत्तनम्_Bit> परिणाम;
		प्रत्यागम get_drivers(परिणाम, sig);
	}

	प्रतिरूपम्<typename T>
	अन्तःस्थ bool has_consumers(ध्रुव T &sig) ध्रुव {
		pool<पत्तनम्_Bit> परिणाम;
		प्रत्यागम get_consumers(परिणाम, sig);
	}

	प्रतिरूपम्<typename T>
	अन्तःस्थ bool has_inputs(ध्रुव T &sig) ध्रुव {
		pool<RTLIL::SigBit> परिणाम;
		प्रत्यागम get_inputs(परिणाम, sig);
	}

	प्रतिरूपम्<typename T>
	अन्तःस्थ bool has_outputs(ध्रुव T &sig) ध्रुव {
		pool<RTLIL::SigBit> परिणाम;
		प्रत्यागम get_outputs(परिणाम, sig);
	}

	// has_* methods -- call by value

	अन्तःस्थ bool has_drivers(RTLIL::SigSpec sig) ध्रुव {
		pool<पत्तनम्_Bit> परिणाम;
		प्रत्यागम get_drivers(परिणाम, sig);
	}

	अन्तःस्थ bool has_consumers(RTLIL::SigSpec sig) ध्रुव {
		pool<पत्तनम्_Bit> परिणाम;
		प्रत्यागम get_consumers(परिणाम, sig);
	}

	अन्तःस्थ bool has_inputs(RTLIL::SigSpec sig) ध्रुव {
		pool<RTLIL::SigBit> परिणाम;
		प्रत्यागम get_inputs(परिणाम, sig);
	}

	अन्तःस्थ bool has_outputs(RTLIL::SigSpec sig) ध्रुव {
		pool<RTLIL::SigBit> परिणाम;
		प्रत्यागम get_outputs(परिणाम, sig);
	}
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
