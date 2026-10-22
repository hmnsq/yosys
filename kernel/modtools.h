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

#నిర్వచితం_కాకపోతే MODTOOLS_H
#నిర్వచించు MODTOOLS_H

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/sigtools.h"
#అన్తర్భావ "kernel/celltypes.h"
#అన్తర్భావ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

నిర్మాణం Mod_సూచీ : బహిరంగ RTLIL::Monitor
{
	నిర్మాణం సూచిక_Ordered_Sig_బిట్టు : బహిరంగ RTLIL::Sig_బిట్టు {
		సూచిక_Ordered_Sig_బిట్టు(Sig_బిట్టు s) {
			wire = s.wire;
			ఒకవేళ (wire)
				స్థానభ్రంశం = s.స్థానభ్రంశం;
			లేకపోతే
				దత్తాంశం = s.దత్తాంశం;
		}
		అంతఃవరుస bool క్రియాకారకం<(స్థిరరాశి RTLIL::Sig_బిట్టు &other) స్థిరరాశి {
			ఒకవేళ (wire == other.wire)
				తిరిగి wire ? (స్థానభ్రంశం < other.స్థానభ్రంశం) : (దత్తాంశం < other.దత్తాంశం);
			ఒకవేళ (wire != nullptr && other.wire != nullptr)
				తిరిగి wire < other.wire; // look here
			తిరిగి (wire != nullptr) < (other.wire != nullptr);
		}
	};
	నిర్మాణం పోర్టు_Info {
		RTLIL::Cell* cell;
		RTLIL::IdString పోర్టు;
		పూర్ణాంకం స్థానభ్రంశం;

		పోర్టు_Info() : cell(), పోర్టు(), స్థానభ్రంశం() { }
		పోర్టు_Info(RTLIL::Cell* _c, RTLIL::IdString _p, పూర్ణాంకం _o) : cell(_c), పోర్టు(_p), స్థానభ్రంశం(_o) { }

		bool క్రియాకారకం<(స్థిరరాశి పోర్టు_Info &other) స్థిరరాశి {
			ఒకవేళ (cell != other.cell)
				తిరిగి cell < other.cell;
			ఒకవేళ (స్థానభ్రంశం != other.స్థానభ్రంశం)
				తిరిగి స్థానభ్రంశం < other.స్థానభ్రంశం;
			తిరిగి పోర్టు < other.పోర్టు;
		}

		bool క్రియాకారకం==(స్థిరరాశి పోర్టు_Info &other) స్థిరరాశి {
			తిరిగి cell == other.cell && పోర్టు == other.పోర్టు && స్థానభ్రంశం == other.స్థానభ్రంశం;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
			h.eat(cell->name);
			h.eat(పోర్టు);
			h.eat(స్థానభ్రంశం);
			తిరిగి h;
		}
	};

	నిర్మాణం Sig_బిట్టు_Info
	{
		bool is_ఆదానం, is_output;
		pool<పోర్టు_Info> ports;

		Sig_బిట్టు_Info() : is_ఆదానం(false), is_output(false) { }

		bool క్రియాకారకం==(స్థిరరాశి Sig_బిట్టు_Info &other) స్థిరరాశి {
			తిరిగి is_ఆదానం == other.is_ఆదానం && is_output == other.is_output && ports == other.ports;
		}

		శూన్య విలీనం(స్థిరరాశి Sig_బిట్టు_Info &other)
		{
			is_ఆదానం = is_ఆదానం || other.is_ఆదానం;
			is_output = is_output || other.is_output;
			ports.చొప్పించు(other.ports.begin(), other.ports.చివర());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<సూచిక_Ordered_Sig_బిట్టు, Sig_బిట్టు_Info> database;
	పూర్ణాంకం auto_reload_counter;
	bool auto_reload_module;

	శూన్య పోర్టు_కూడు(RTLIL::Cell *cell, RTLIL::IdString పోర్టు, స్థిరరాశి RTLIL::SigSpec &sig)
	{
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(sig); i++) {
			RTLIL::Sig_బిట్టు బిట్టు = sigmap(sig[i]);
			ఒకవేళ (బిట్టు.wire)
				database[బిట్టు].ports.చొప్పించు(పోర్టు_Info(cell, పోర్టు, i));
		}
	}

	శూన్య పోర్టు_del(RTLIL::Cell *cell, RTLIL::IdString పోర్టు, స్థిరరాశి RTLIL::SigSpec &sig)
	{
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(sig); i++) {
			RTLIL::Sig_బిట్టు బిట్టు = sigmap(sig[i]);
			ఒకవేళ (బిట్టు.wire) {
				database[బిట్టు].ports.erase(పోర్టు_Info(cell, పోర్టు, i));
				ఒకవేళ (!database[బిట్టు].is_ఆదానం && !database[బిట్టు].is_output && database[బిట్టు].ports.ఖాళీలు())
					database.erase(బిట్టు);
			}
		}
	}

	స్థిరరాశి Sig_బిట్టు_Info &info(RTLIL::Sig_బిట్టు బిట్టు)
	{
		తిరిగి database[sigmap(బిట్టు)];
	}

	శూన్య reload_module(bool reset_sigmap = true)
	{
		ఒకవేళ (reset_sigmap) {
			sigmap.clear();
			sigmap.సమితి(module);
		}

		database.clear();
		కొరకు (స్వయంచాలక wire : module->wires())
			ఒకవేళ (wire->పోర్టు_ఆదానం || wire->పోర్టు_output)
				కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wire); i++) {
					RTLIL::Sig_బిట్టు బిట్టు = sigmap(RTLIL::Sig_బిట్టు(wire, i));
					ఒకవేళ (బిట్టు.wire && wire->పోర్టు_ఆదానం)
						database[బిట్టు].is_ఆదానం = true;
					ఒకవేళ (బిట్టు.wire && wire->పోర్టు_output)
						database[బిట్టు].is_output = true;
				}
		కొరకు (స్వయంచాలక cell : module->cells())
			కొరకు (స్వయంచాలక &conn : cell->connections())
				పోర్టు_కూడు(cell, conn.మొదటి, conn.రెండవ);

		ఒకవేళ (auto_reload_module) {
			ఒకవేళ (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#నిర్వచితం_కాకపోతే NDEBUG
		ఒకవేళ (auto_reload_module)
			తిరిగి true;

		కొరకు (స్వయంచాలక it : database)
			log_assert(it.మొదటి == sigmap(it.మొదటి));

		స్వయంచాలక database_bak = std::move(database);
		reload_module(false);

		ఒకవేళ (!(database == database_bak))
		{
			కొరకు (స్వయంచాలక &it : database_bak)
				ఒకవేళ (!database.లెక్క(it.మొదటి))
					సంవర్గమానం("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.మొదటి));

			కొరకు (స్వయంచాలక &it : database)
				ఒకవేళ (!database_bak.లెక్క(it.మొదటి))
					సంవర్గమానం("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.మొదటి));
				లేకపోతే ఒకవేళ (!(it.రెండవ == database_bak.at(it.మొదటి)))
					సంవర్గమానం("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.మొదటి));

			తిరిగి false;
		}
#అయితే_ముగింపు
		తిరిగి true;
	}

	శూన్య check()
	{
		log_assert(ok());
	}

	శూన్య notify_connect(RTLIL::Cell *cell, RTLIL::IdString పోర్టు, స్థిరరాశి RTLIL::SigSpec &old_sig, స్థిరరాశి RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		ఒకవేళ (auto_reload_module)
			తిరిగి;

		పోర్టు_del(cell, పోర్టు, old_sig);
		పోర్టు_కూడు(cell, పోర్టు, sig);
	}

	శూన్య notify_connect(RTLIL::Module *mod, స్థిరరాశి RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		ఒకవేళ (auto_reload_module)
			తిరిగి;

		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(sigsig.మొదటి); i++)
		{
			RTLIL::Sig_బిట్టు lhs = sigmap(sigsig.మొదటి[i]);
			RTLIL::Sig_బిట్టు rhs = sigmap(sigsig.రెండవ[i]);
			bool has_lhs = database.లెక్క(lhs) != 0;
			bool has_rhs = database.లెక్క(rhs) != 0;

			ఒకవేళ (!has_lhs && !has_rhs) {
				sigmap.కూడు(lhs, rhs);
			} లేకపోతే
			ఒకవేళ (!has_rhs) {
				Sig_బిట్టు_Info new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.కూడు(lhs, rhs);
				lhs = sigmap(lhs);
				ఒకవేళ (lhs.wire)
					database[lhs] = new_info;
			} లేకపోతే
			ఒకవేళ (!has_lhs) {
				Sig_బిట్టు_Info new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.కూడు(lhs, rhs);
				rhs = sigmap(rhs);
				ఒకవేళ (rhs.wire)
					database[rhs] = new_info;
			} లేకపోతే {
				Sig_బిట్టు_Info new_info = database.at(lhs);
				new_info.విలీనం(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.కూడు(lhs, rhs);
				rhs = sigmap(rhs);
				ఒకవేళ (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	శూన్య notify_connect(RTLIL::Module *mod, స్థిరరాశి std::సదిశ<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	శూన్య notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	Mod_సూచీ(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.చొప్పించు(this);
	}

	~Mod_సూచీ()
	{
		module->monitors.erase(this);
	}

	Sig_బిట్టు_Info *query(RTLIL::Sig_బిట్టు బిట్టు)
	{
		ఒకవేళ (auto_reload_module)
			reload_module();

		స్వయంచాలక it = database.కనుగొను(sigmap(బిట్టు));
		ఒకవేళ (it == database.చివర())
			తిరిగి nullptr;
		లేకపోతే
			తిరిగి &it->రెండవ;
	}

	bool query_is_ఆదానం(RTLIL::Sig_బిట్టు బిట్టు)
	{
		స్థిరరాశి Sig_బిట్టు_Info *info = query(బిట్టు);
		ఒకవేళ (info == nullptr)
			తిరిగి false;
		తిరిగి info->is_ఆదానం;
	}

	bool query_is_output(RTLIL::Sig_బిట్టు బిట్టు)
	{
		స్థిరరాశి Sig_బిట్టు_Info *info = query(బిట్టు);
		ఒకవేళ (info == nullptr)
			తిరిగి false;
		తిరిగి info->is_output;
	}

	pool<పోర్టు_Info> &query_ports(RTLIL::Sig_బిట్టు బిట్టు)
	{
		స్థిర pool<పోర్టు_Info> ఖాళీలు_ఫలితం_సమితి;
		Sig_బిట్టు_Info *info = query(బిట్టు);
		ఒకవేళ (info == nullptr)
			తిరిగి ఖాళీలు_ఫలితం_సమితి;
		తిరిగి info->ports;
	}

	శూన్య dump_db()
	{
		సంవర్గమానం("--- ModIndex Dump ---\n");

		ఒకవేళ (auto_reload_module) {
			సంవర్గమానం("AUTO-RELOAD\n");
			reload_module();
		}

		కొరకు (స్వయంచాలక &it : database) {
			సంవర్గమానం("BIT %s:\n", log_signal(it.మొదటి));
			ఒకవేళ (it.రెండవ.is_ఆదానం)
				సంవర్గమానం("  PRIMARY INPUT\n");
			ఒకవేళ (it.రెండవ.is_output)
				సంవర్గమానం("  PRIMARY OUTPUT\n");
			కొరకు (స్వయంచాలక &పోర్టు : it.రెండవ.ports)
				సంవర్గమానం("  PORT: %s.%s[%d] (%s)\n", పోర్టు.cell,
						PooledName(module, పోర్టు.పోర్టు).unescape(), పోర్టు.స్థానభ్రంశం, పోర్టు.cell->type.unescape());
		}
	}
};

నిర్మాణం ModWalker
{
	నిర్మాణం పోర్టు_బిట్టు_2
	{
		RTLIL::Cell *cell;
		RTLIL::IdString పోర్టు;
		పూర్ణాంకం స్థానభ్రంశం;
		పోర్టు_బిట్టు_2(Cell* c, IdString p, పూర్ణాంకం o) : cell(c), పోర్టు(p), స్థానభ్రంశం(o) {}

		bool క్రియాకారకం<(స్థిరరాశి పోర్టు_బిట్టు_2 &other) స్థిరరాశి {
			ఒకవేళ (cell != other.cell)
				తిరిగి cell < other.cell;
			ఒకవేళ (పోర్టు != other.పోర్టు)
				తిరిగి పోర్టు < other.పోర్టు;
			తిరిగి స్థానభ్రంశం < other.స్థానభ్రంశం;
		}

		bool క్రియాకారకం==(స్థిరరాశి పోర్టు_బిట్టు_2 &other) స్థిరరాశి {
			తిరిగి cell == other.cell && పోర్టు == other.పోర్టు && స్థానభ్రంశం == other.స్థానభ్రంశం;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
			h.eat(cell->name);
			h.eat(పోర్టు);
			h.eat(స్థానభ్రంశం);
			తిరిగి h;
		}
	};

	RTLIL::Design *design;
	RTLIL::Module *module;

	NewCellTypes ct;
	SigMap sigmap;

	dict<RTLIL::Sig_బిట్టు, pool<పోర్టు_బిట్టు_2>> signal_drivers;
	dict<RTLIL::Sig_బిట్టు, pool<పోర్టు_బిట్టు_2>> signal_consumers;
	pool<RTLIL::Sig_బిట్టు> signal_inputs, signal_outputs;

	dict<RTLIL::Cell*, pool<RTLIL::Sig_బిట్టు>> cell_outputs, cell_inputs;

	శూన్య కూడు_wire(RTLIL::Wire *wire)
	{
		ఒకవేళ (wire->పోర్టు_ఆదానం) {
			std::సదిశ<RTLIL::Sig_బిట్టు> బిట్లు = sigmap(wire);
			కొరకు (స్వయంచాలక బిట్టు : బిట్లు)
				ఒకవేళ (బిట్టు.wire != ఖాళీ)
					signal_inputs.చొప్పించు(బిట్టు);
		}

		ఒకవేళ (wire->పోర్టు_output) {
			std::సదిశ<RTLIL::Sig_బిట్టు> బిట్లు = sigmap(wire);
			కొరకు (స్వయంచాలక బిట్టు : బిట్లు)
				ఒకవేళ (బిట్టు.wire != ఖాళీ)
					signal_outputs.చొప్పించు(బిట్టు);
		}
	}

	శూన్య కూడు_cell_పోర్టు(RTLIL::Cell *cell, RTLIL::IdString పోర్టు, std::సదిశ<RTLIL::Sig_బిట్టు> బిట్లు, bool is_output, bool is_ఆదానం)
	{
		కొరకు (పూర్ణాంకం i = 0; i < పూర్ణాంకం(బిట్లు.కొలత()); i++)
			ఒకవేళ (బిట్లు[i].wire != ఖాళీ) {
				పోర్టు_బిట్టు_2 pbit {cell, పోర్టు, i};
				ఒకవేళ (is_output) {
					signal_drivers[బిట్లు[i]].చొప్పించు(pbit);
					cell_outputs[cell].చొప్పించు(బిట్లు[i]);
				}
				ఒకవేళ (is_ఆదానం) {
					signal_consumers[బిట్లు[i]].చొప్పించు(pbit);
					cell_inputs[cell].చొప్పించు(బిట్లు[i]);
				}
			}
	}

	శూన్య కూడు_cell(RTLIL::Cell *cell)
	{
		ఒకవేళ (ct.cell_తెలిసిన(cell->type)) {
			కొరకు (స్వయంచాలక &conn : cell->connections())
				కూడు_cell_పోర్టు(cell, conn.మొదటి, sigmap(conn.రెండవ),
						ct.cell_output(cell->type, conn.మొదటి),
						ct.cell_ఆదానం(cell->type, conn.మొదటి));
		} లేకపోతే {
			కొరకు (స్వయంచాలక &conn : cell->connections())
				కూడు_cell_పోర్టు(cell, conn.మొదటి, sigmap(conn.రెండవ), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(ఖాళీ)
	{
		ct.అమరిక(design);
		ఒకవేళ (module)
			అమరిక(module);
	}

	శూన్య అమరిక(RTLIL::Module *module, CellTypes *filter_ct = ఖాళీ)
	{
		this->module = module;

		sigmap.సమితి(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		కొరకు (స్వయంచాలక &it : module->wires_)
			కూడు_wire(it.రెండవ);
		కొరకు (స్వయంచాలక &it : module->cells_)
			ఒకవేళ (filter_ct == ఖాళీ || filter_ct->cell_తెలిసిన(it.రెండవ->type))
				కూడు_cell(it.రెండవ);
	}

	// get_* methods -- single RTLIL::SigBit

	అంతఃవరుస bool get_drivers(pool<పోర్టు_బిట్టు_2> &ఫలితం, RTLIL::Sig_బిట్టు బిట్టు) స్థిరరాశి
	{
		bool దొరికింది = false;
		ఒకవేళ (signal_drivers.లెక్క(బిట్టు)) {
			స్థిరరాశి pool<పోర్టు_బిట్టు_2> &r = signal_drivers.at(బిట్టు);
			ఫలితం.చొప్పించు(r.begin(), r.చివర());
			దొరికింది = true;
		}
		తిరిగి దొరికింది;
	}

	అంతఃవరుస bool get_consumers(pool<పోర్టు_బిట్టు_2> &ఫలితం, RTLIL::Sig_బిట్టు బిట్టు) స్థిరరాశి
	{
		bool దొరికింది = false;
		ఒకవేళ (signal_consumers.లెక్క(బిట్టు)) {
			స్థిరరాశి pool<పోర్టు_బిట్టు_2> &r = signal_consumers.at(బిట్టు);
			ఫలితం.చొప్పించు(r.begin(), r.చివర());
			దొరికింది = true;
		}
		తిరిగి దొరికింది;
	}

	అంతఃవరుస bool get_inputs(pool<RTLIL::Sig_బిట్టు> &ఫలితం, RTLIL::Sig_బిట్టు బిట్టు) స్థిరరాశి
	{
		bool దొరికింది = false;
		ఒకవేళ (signal_inputs.లెక్క(బిట్టు))
			ఫలితం.చొప్పించు(బిట్టు), దొరికింది = true;
		తిరిగి దొరికింది;
	}

	అంతఃవరుస bool get_outputs(pool<RTLIL::Sig_బిట్టు> &ఫలితం, RTLIL::Sig_బిట్టు బిట్టు) స్థిరరాశి
	{
		bool దొరికింది = false;
		ఒకవేళ (signal_outputs.లెక్క(బిట్టు))
			ఫలితం.చొప్పించు(బిట్టు), దొరికింది = true;
		తిరిగి దొరికింది;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	మూస<typename T>
	అంతఃవరుస bool get_drivers(pool<పోర్టు_బిట్టు_2> &ఫలితం, స్థిరరాశి T &బిట్లు) స్థిరరాశి
	{
		bool దొరికింది = false;
		కొరకు (RTLIL::Sig_బిట్టు బిట్టు : బిట్లు)
			ఒకవేళ (signal_drivers.లెక్క(బిట్టు)) {
				స్థిరరాశి pool<పోర్టు_బిట్టు_2> &r = signal_drivers.at(బిట్టు);
				ఫలితం.చొప్పించు(r.begin(), r.చివర());
				దొరికింది = true;
			}
		తిరిగి దొరికింది;
	}

	మూస<typename T>
	అంతఃవరుస bool get_consumers(pool<పోర్టు_బిట్టు_2> &ఫలితం, స్థిరరాశి T &బిట్లు) స్థిరరాశి
	{
		bool దొరికింది = false;
		కొరకు (RTLIL::Sig_బిట్టు బిట్టు : బిట్లు)
			ఒకవేళ (signal_consumers.లెక్క(బిట్టు)) {
				స్థిరరాశి pool<పోర్టు_బిట్టు_2> &r = signal_consumers.at(బిట్టు);
				ఫలితం.చొప్పించు(r.begin(), r.చివర());
				దొరికింది = true;
			}
		తిరిగి దొరికింది;
	}

	మూస<typename T>
	అంతఃవరుస bool get_inputs(pool<RTLIL::Sig_బిట్టు> &ఫలితం, స్థిరరాశి T &బిట్లు) స్థిరరాశి
	{
		bool దొరికింది = false;
		కొరకు (RTLIL::Sig_బిట్టు బిట్టు : బిట్లు)
			ఒకవేళ (signal_inputs.లెక్క(బిట్టు))
				ఫలితం.చొప్పించు(బిట్టు), దొరికింది = true;
		తిరిగి దొరికింది;
	}

	మూస<typename T>
	అంతఃవరుస bool get_outputs(pool<RTLIL::Sig_బిట్టు> &ఫలితం, స్థిరరాశి T &బిట్లు) స్థిరరాశి
	{
		bool దొరికింది = false;
		కొరకు (RTLIL::Sig_బిట్టు బిట్టు : బిట్లు)
			ఒకవేళ (signal_outputs.లెక్క(బిట్టు))
				ఫలితం.చొప్పించు(బిట్టు), దొరికింది = true;
		తిరిగి దొరికింది;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<పోర్టు_బిట్టు_2> &ఫలితం, RTLIL::SigSpec సంకేతం) స్థిరరాశి
	{
		std::సదిశ<RTLIL::Sig_బిట్టు> బిట్లు = sigmap(సంకేతం);
		తిరిగి get_drivers(ఫలితం, బిట్లు);
	}

	bool get_consumers(pool<పోర్టు_బిట్టు_2> &ఫలితం, RTLIL::SigSpec సంకేతం) స్థిరరాశి
	{
		std::సదిశ<RTLIL::Sig_బిట్టు> బిట్లు = sigmap(సంకేతం);
		తిరిగి get_consumers(ఫలితం, బిట్లు);
	}

	bool get_inputs(pool<RTLIL::Sig_బిట్టు> &ఫలితం, RTLIL::SigSpec సంకేతం) స్థిరరాశి
	{
		std::సదిశ<RTLIL::Sig_బిట్టు> బిట్లు = sigmap(సంకేతం);
		తిరిగి get_inputs(ఫలితం, బిట్లు);
	}

	bool get_outputs(pool<RTLIL::Sig_బిట్టు> &ఫలితం, RTLIL::SigSpec సంకేతం) స్థిరరాశి
	{
		std::సదిశ<RTLIL::Sig_బిట్టు> బిట్లు = sigmap(సంకేతం);
		తిరిగి get_outputs(ఫలితం, బిట్లు);
	}

	// has_* methods -- call by reference

	మూస<typename T>
	అంతఃవరుస bool has_drivers(స్థిరరాశి T &sig) స్థిరరాశి {
		pool<పోర్టు_బిట్టు_2> ఫలితం;
		తిరిగి get_drivers(ఫలితం, sig);
	}

	మూస<typename T>
	అంతఃవరుస bool has_consumers(స్థిరరాశి T &sig) స్థిరరాశి {
		pool<పోర్టు_బిట్టు_2> ఫలితం;
		తిరిగి get_consumers(ఫలితం, sig);
	}

	మూస<typename T>
	అంతఃవరుస bool has_inputs(స్థిరరాశి T &sig) స్థిరరాశి {
		pool<RTLIL::Sig_బిట్టు> ఫలితం;
		తిరిగి get_inputs(ఫలితం, sig);
	}

	మూస<typename T>
	అంతఃవరుస bool has_outputs(స్థిరరాశి T &sig) స్థిరరాశి {
		pool<RTLIL::Sig_బిట్టు> ఫలితం;
		తిరిగి get_outputs(ఫలితం, sig);
	}

	// has_* methods -- call by value

	అంతఃవరుస bool has_drivers(RTLIL::SigSpec sig) స్థిరరాశి {
		pool<పోర్టు_బిట్టు_2> ఫలితం;
		తిరిగి get_drivers(ఫలితం, sig);
	}

	అంతఃవరుస bool has_consumers(RTLIL::SigSpec sig) స్థిరరాశి {
		pool<పోర్టు_బిట్టు_2> ఫలితం;
		తిరిగి get_consumers(ఫలితం, sig);
	}

	అంతఃవరుస bool has_inputs(RTLIL::SigSpec sig) స్థిరరాశి {
		pool<RTLIL::Sig_బిట్టు> ఫలితం;
		తిరిగి get_inputs(ఫలితం, sig);
	}

	అంతఃవరుస bool has_outputs(RTLIL::SigSpec sig) స్థిరరాశి {
		pool<RTLIL::Sig_బిట్టు> ఫలితం;
		తిరిగి get_outputs(ఫలితం, sig);
	}
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
