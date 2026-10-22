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

#اگر_غیر_تعریف_شدہ MODTOOLS_H
#تعریف_کرو MODTOOLS_H

#شامل "kernel/yosys.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/celltypes.h"
#شامل "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ساخت Mod_اشاریہ : عوامی RTLIL::Monitor
{
	ساخت اشارہ_گر_Ordered_Sig_ثنائی_ہندسہ : عوامی RTLIL::Sig_ثنائی_ہندسہ {
		اشارہ_گر_Ordered_Sig_ثنائی_ہندسہ(Sig_ثنائی_ہندسہ s) {
			wire = s.wire;
			اگر (wire)
				انحراف = s.انحراف;
			ورنہ
				ڈیٹا = s.ڈیٹا;
		}
		اندرونی bool عامل<(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت {
			اگر (wire == other.wire)
				واپس wire ? (انحراف < other.انحراف) : (ڈیٹا < other.ڈیٹا);
			اگر (wire != nullptr && other.wire != nullptr)
				واپس wire < other.wire; // look here
			واپس (wire != nullptr) < (other.wire != nullptr);
		}
	};
	ساخت پورٹ_Info {
		RTLIL::Cell* cell;
		RTLIL::IdString پورٹ;
		صحیح انحراف;

		پورٹ_Info() : cell(), پورٹ(), انحراف() { }
		پورٹ_Info(RTLIL::Cell* _c, RTLIL::IdString _p, صحیح _o) : cell(_c), پورٹ(_p), انحراف(_o) { }

		bool عامل<(ثابت پورٹ_Info &other) ثابت {
			اگر (cell != other.cell)
				واپس cell < other.cell;
			اگر (انحراف != other.انحراف)
				واپس انحراف < other.انحراف;
			واپس پورٹ < other.پورٹ;
		}

		bool عامل==(ثابت پورٹ_Info &other) ثابت {
			واپس cell == other.cell && پورٹ == other.پورٹ && انحراف == other.انحراف;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(پورٹ);
			h.eat(انحراف);
			واپس h;
		}
	};

	ساخت Sig_ثنائی_ہندسہ_Info
	{
		bool is_داخلہ, is_output;
		pool<پورٹ_Info> ports;

		Sig_ثنائی_ہندسہ_Info() : is_داخلہ(false), is_output(false) { }

		bool عامل==(ثابت Sig_ثنائی_ہندسہ_Info &other) ثابت {
			واپس is_داخلہ == other.is_داخلہ && is_output == other.is_output && ports == other.ports;
		}

		خالی ملاؤ(ثابت Sig_ثنائی_ہندسہ_Info &other)
		{
			is_داخلہ = is_داخلہ || other.is_داخلہ;
			is_output = is_output || other.is_output;
			ports.داخل_کرو(other.ports.begin(), other.ports.انتہا());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<اشارہ_گر_Ordered_Sig_ثنائی_ہندسہ, Sig_ثنائی_ہندسہ_Info> database;
	صحیح auto_reload_شمار;
	bool auto_reload_module;

	خالی پورٹ_جوڑو(RTLIL::Cell *cell, RTLIL::IdString پورٹ, ثابت RTLIL::SigSpec &sig)
	{
		برائے (صحیح i = 0; i < Get_سائز(sig); i++) {
			RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ = sigmap(sig[i]);
			اگر (ثنائی_ہندسہ.wire)
				database[ثنائی_ہندسہ].ports.داخل_کرو(پورٹ_Info(cell, پورٹ, i));
		}
	}

	خالی پورٹ_del(RTLIL::Cell *cell, RTLIL::IdString پورٹ, ثابت RTLIL::SigSpec &sig)
	{
		برائے (صحیح i = 0; i < Get_سائز(sig); i++) {
			RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ = sigmap(sig[i]);
			اگر (ثنائی_ہندسہ.wire) {
				database[ثنائی_ہندسہ].ports.erase(پورٹ_Info(cell, پورٹ, i));
				اگر (!database[ثنائی_ہندسہ].is_داخلہ && !database[ثنائی_ہندسہ].is_output && database[ثنائی_ہندسہ].ports.خالی_خانے())
					database.erase(ثنائی_ہندسہ);
			}
		}
	}

	ثابت Sig_ثنائی_ہندسہ_Info &info(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
	{
		واپس database[sigmap(ثنائی_ہندسہ)];
	}

	خالی reload_module(bool reset_sigmap = true)
	{
		اگر (reset_sigmap) {
			sigmap.clear();
			sigmap.سیٹ(module);
		}

		database.clear();
		برائے (خودکار wire : module->wires())
			اگر (wire->پورٹ_داخلہ || wire->پورٹ_output)
				برائے (صحیح i = 0; i < Get_سائز(wire); i++) {
					RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ = sigmap(RTLIL::Sig_ثنائی_ہندسہ(wire, i));
					اگر (ثنائی_ہندسہ.wire && wire->پورٹ_داخلہ)
						database[ثنائی_ہندسہ].is_داخلہ = true;
					اگر (ثنائی_ہندسہ.wire && wire->پورٹ_output)
						database[ثنائی_ہندسہ].is_output = true;
				}
		برائے (خودکار cell : module->cells())
			برائے (خودکار &conn : cell->connections())
				پورٹ_جوڑو(cell, conn.پہلا, conn.دوسرا);

		اگر (auto_reload_module) {
			اگر (++auto_reload_شمار > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#اگر_غیر_تعریف_شدہ NDEBUG
		اگر (auto_reload_module)
			واپس true;

		برائے (خودکار it : database)
			log_assert(it.پہلا == sigmap(it.پہلا));

		خودکار database_bak = std::move(database);
		reload_module(false);

		اگر (!(database == database_bak))
		{
			برائے (خودکار &it : database_bak)
				اگر (!database.گنتی(it.پہلا))
					لوگارتھم("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.پہلا));

			برائے (خودکار &it : database)
				اگر (!database_bak.گنتی(it.پہلا))
					لوگارتھم("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.پہلا));
				ورنہ اگر (!(it.دوسرا == database_bak.at(it.پہلا)))
					لوگارتھم("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.پہلا));

			واپس false;
		}
#اگر_ختم
		واپس true;
	}

	خالی check()
	{
		log_assert(ok());
	}

	خالی notify_connect(RTLIL::Cell *cell, RTLIL::IdString پورٹ, ثابت RTLIL::SigSpec &old_sig, ثابت RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		اگر (auto_reload_module)
			واپس;

		پورٹ_del(cell, پورٹ, old_sig);
		پورٹ_جوڑو(cell, پورٹ, sig);
	}

	خالی notify_connect(RTLIL::Module *mod, ثابت RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		اگر (auto_reload_module)
			واپس;

		برائے (صحیح i = 0; i < Get_سائز(sigsig.پہلا); i++)
		{
			RTLIL::Sig_ثنائی_ہندسہ lhs = sigmap(sigsig.پہلا[i]);
			RTLIL::Sig_ثنائی_ہندسہ rhs = sigmap(sigsig.دوسرا[i]);
			bool has_lhs = database.گنتی(lhs) != 0;
			bool has_rhs = database.گنتی(rhs) != 0;

			اگر (!has_lhs && !has_rhs) {
				sigmap.جوڑو(lhs, rhs);
			} ورنہ
			اگر (!has_rhs) {
				Sig_ثنائی_ہندسہ_Info new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.جوڑو(lhs, rhs);
				lhs = sigmap(lhs);
				اگر (lhs.wire)
					database[lhs] = new_info;
			} ورنہ
			اگر (!has_lhs) {
				Sig_ثنائی_ہندسہ_Info new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.جوڑو(lhs, rhs);
				rhs = sigmap(rhs);
				اگر (rhs.wire)
					database[rhs] = new_info;
			} ورنہ {
				Sig_ثنائی_ہندسہ_Info new_info = database.at(lhs);
				new_info.ملاؤ(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.جوڑو(lhs, rhs);
				rhs = sigmap(rhs);
				اگر (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	خالی notify_connect(RTLIL::Module *mod, ثابت std::سمتیہ<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	خالی notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	Mod_اشاریہ(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_شمار = 0;
		auto_reload_module = true;
		module->monitors.داخل_کرو(this);
	}

	~Mod_اشاریہ()
	{
		module->monitors.erase(this);
	}

	Sig_ثنائی_ہندسہ_Info *query(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
	{
		اگر (auto_reload_module)
			reload_module();

		خودکار it = database.ڈھونڈو(sigmap(ثنائی_ہندسہ));
		اگر (it == database.انتہا())
			واپس nullptr;
		ورنہ
			واپس &it->دوسرا;
	}

	bool query_is_داخلہ(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
	{
		ثابت Sig_ثنائی_ہندسہ_Info *info = query(ثنائی_ہندسہ);
		اگر (info == nullptr)
			واپس false;
		واپس info->is_داخلہ;
	}

	bool query_is_output(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
	{
		ثابت Sig_ثنائی_ہندسہ_Info *info = query(ثنائی_ہندسہ);
		اگر (info == nullptr)
			واپس false;
		واپس info->is_output;
	}

	pool<پورٹ_Info> &query_ports(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
	{
		ساکن pool<پورٹ_Info> خالی_خانے_نتیجہ_سیٹ;
		Sig_ثنائی_ہندسہ_Info *info = query(ثنائی_ہندسہ);
		اگر (info == nullptr)
			واپس خالی_خانے_نتیجہ_سیٹ;
		واپس info->ports;
	}

	خالی dump_db()
	{
		لوگارتھم("--- ModIndex Dump ---\n");

		اگر (auto_reload_module) {
			لوگارتھم("AUTO-RELOAD\n");
			reload_module();
		}

		برائے (خودکار &it : database) {
			لوگارتھم("BIT %s:\n", log_signal(it.پہلا));
			اگر (it.دوسرا.is_داخلہ)
				لوگارتھم("  PRIMARY INPUT\n");
			اگر (it.دوسرا.is_output)
				لوگارتھم("  PRIMARY OUTPUT\n");
			برائے (خودکار &پورٹ : it.دوسرا.ports)
				لوگارتھم("  PORT: %s.%s[%d] (%s)\n", پورٹ.cell,
						PooledName(module, پورٹ.پورٹ).unescape(), پورٹ.انحراف, پورٹ.cell->type.unescape());
		}
	}
};

ساخت ModWalker
{
	ساخت پورٹ_ثنائی_ہندسہ_2
	{
		RTLIL::Cell *cell;
		RTLIL::IdString پورٹ;
		صحیح انحراف;
		پورٹ_ثنائی_ہندسہ_2(Cell* c, IdString p, صحیح o) : cell(c), پورٹ(p), انحراف(o) {}

		bool عامل<(ثابت پورٹ_ثنائی_ہندسہ_2 &other) ثابت {
			اگر (cell != other.cell)
				واپس cell < other.cell;
			اگر (پورٹ != other.پورٹ)
				واپس پورٹ < other.پورٹ;
			واپس انحراف < other.انحراف;
		}

		bool عامل==(ثابت پورٹ_ثنائی_ہندسہ_2 &other) ثابت {
			واپس cell == other.cell && پورٹ == other.پورٹ && انحراف == other.انحراف;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(پورٹ);
			h.eat(انحراف);
			واپس h;
		}
	};

	RTLIL::Design *design;
	RTLIL::Module *module;

	NewCellTypes ct;
	SigMap sigmap;

	dict<RTLIL::Sig_ثنائی_ہندسہ, pool<پورٹ_ثنائی_ہندسہ_2>> signal_drivers;
	dict<RTLIL::Sig_ثنائی_ہندسہ, pool<پورٹ_ثنائی_ہندسہ_2>> signal_consumers;
	pool<RTLIL::Sig_ثنائی_ہندسہ> signal_inputs, signal_outputs;

	dict<RTLIL::Cell*, pool<RTLIL::Sig_ثنائی_ہندسہ>> cell_outputs, cell_inputs;

	خالی جوڑو_wire(RTLIL::Wire *wire)
	{
		اگر (wire->پورٹ_داخلہ) {
			std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ = sigmap(wire);
			برائے (خودکار ثنائی_ہندسہ : بٹ)
				اگر (ثنائی_ہندسہ.wire != تہی)
					signal_inputs.داخل_کرو(ثنائی_ہندسہ);
		}

		اگر (wire->پورٹ_output) {
			std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ = sigmap(wire);
			برائے (خودکار ثنائی_ہندسہ : بٹ)
				اگر (ثنائی_ہندسہ.wire != تہی)
					signal_outputs.داخل_کرو(ثنائی_ہندسہ);
		}
	}

	خالی جوڑو_cell_پورٹ(RTLIL::Cell *cell, RTLIL::IdString پورٹ, std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ, bool is_output, bool is_داخلہ)
	{
		برائے (صحیح i = 0; i < صحیح(بٹ.سائز()); i++)
			اگر (بٹ[i].wire != تہی) {
				پورٹ_ثنائی_ہندسہ_2 pbit {cell, پورٹ, i};
				اگر (is_output) {
					signal_drivers[بٹ[i]].داخل_کرو(pbit);
					cell_outputs[cell].داخل_کرو(بٹ[i]);
				}
				اگر (is_داخلہ) {
					signal_consumers[بٹ[i]].داخل_کرو(pbit);
					cell_inputs[cell].داخل_کرو(بٹ[i]);
				}
			}
	}

	خالی جوڑو_cell(RTLIL::Cell *cell)
	{
		اگر (ct.cell_معلوم(cell->type)) {
			برائے (خودکار &conn : cell->connections())
				جوڑو_cell_پورٹ(cell, conn.پہلا, sigmap(conn.دوسرا),
						ct.cell_output(cell->type, conn.پہلا),
						ct.cell_داخلہ(cell->type, conn.پہلا));
		} ورنہ {
			برائے (خودکار &conn : cell->connections())
				جوڑو_cell_پورٹ(cell, conn.پہلا, sigmap(conn.دوسرا), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(تہی)
	{
		ct.تیاری(design);
		اگر (module)
			تیاری(module);
	}

	خالی تیاری(RTLIL::Module *module, CellTypes *filter_ct = تہی)
	{
		this->module = module;

		sigmap.سیٹ(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		برائے (خودکار &it : module->wires_)
			جوڑو_wire(it.دوسرا);
		برائے (خودکار &it : module->cells_)
			اگر (filter_ct == تہی || filter_ct->cell_معلوم(it.دوسرا->type))
				جوڑو_cell(it.دوسرا);
	}

	// get_* methods -- single RTLIL::SigBit

	اندرونی bool get_drivers(pool<پورٹ_ثنائی_ہندسہ_2> &نتیجہ, RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		bool ملا = false;
		اگر (signal_drivers.گنتی(ثنائی_ہندسہ)) {
			ثابت pool<پورٹ_ثنائی_ہندسہ_2> &r = signal_drivers.at(ثنائی_ہندسہ);
			نتیجہ.داخل_کرو(r.begin(), r.انتہا());
			ملا = true;
		}
		واپس ملا;
	}

	اندرونی bool get_consumers(pool<پورٹ_ثنائی_ہندسہ_2> &نتیجہ, RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		bool ملا = false;
		اگر (signal_consumers.گنتی(ثنائی_ہندسہ)) {
			ثابت pool<پورٹ_ثنائی_ہندسہ_2> &r = signal_consumers.at(ثنائی_ہندسہ);
			نتیجہ.داخل_کرو(r.begin(), r.انتہا());
			ملا = true;
		}
		واپس ملا;
	}

	اندرونی bool get_inputs(pool<RTLIL::Sig_ثنائی_ہندسہ> &نتیجہ, RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		bool ملا = false;
		اگر (signal_inputs.گنتی(ثنائی_ہندسہ))
			نتیجہ.داخل_کرو(ثنائی_ہندسہ), ملا = true;
		واپس ملا;
	}

	اندرونی bool get_outputs(pool<RTLIL::Sig_ثنائی_ہندسہ> &نتیجہ, RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		bool ملا = false;
		اگر (signal_outputs.گنتی(ثنائی_ہندسہ))
			نتیجہ.داخل_کرو(ثنائی_ہندسہ), ملا = true;
		واپس ملا;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	سانچہ<typename T>
	اندرونی bool get_drivers(pool<پورٹ_ثنائی_ہندسہ_2> &نتیجہ, ثابت T &بٹ) ثابت
	{
		bool ملا = false;
		برائے (RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ : بٹ)
			اگر (signal_drivers.گنتی(ثنائی_ہندسہ)) {
				ثابت pool<پورٹ_ثنائی_ہندسہ_2> &r = signal_drivers.at(ثنائی_ہندسہ);
				نتیجہ.داخل_کرو(r.begin(), r.انتہا());
				ملا = true;
			}
		واپس ملا;
	}

	سانچہ<typename T>
	اندرونی bool get_consumers(pool<پورٹ_ثنائی_ہندسہ_2> &نتیجہ, ثابت T &بٹ) ثابت
	{
		bool ملا = false;
		برائے (RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ : بٹ)
			اگر (signal_consumers.گنتی(ثنائی_ہندسہ)) {
				ثابت pool<پورٹ_ثنائی_ہندسہ_2> &r = signal_consumers.at(ثنائی_ہندسہ);
				نتیجہ.داخل_کرو(r.begin(), r.انتہا());
				ملا = true;
			}
		واپس ملا;
	}

	سانچہ<typename T>
	اندرونی bool get_inputs(pool<RTLIL::Sig_ثنائی_ہندسہ> &نتیجہ, ثابت T &بٹ) ثابت
	{
		bool ملا = false;
		برائے (RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ : بٹ)
			اگر (signal_inputs.گنتی(ثنائی_ہندسہ))
				نتیجہ.داخل_کرو(ثنائی_ہندسہ), ملا = true;
		واپس ملا;
	}

	سانچہ<typename T>
	اندرونی bool get_outputs(pool<RTLIL::Sig_ثنائی_ہندسہ> &نتیجہ, ثابت T &بٹ) ثابت
	{
		bool ملا = false;
		برائے (RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ : بٹ)
			اگر (signal_outputs.گنتی(ثنائی_ہندسہ))
				نتیجہ.داخل_کرو(ثنائی_ہندسہ), ملا = true;
		واپس ملا;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<پورٹ_ثنائی_ہندسہ_2> &نتیجہ, RTLIL::SigSpec اشارہ) ثابت
	{
		std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ = sigmap(اشارہ);
		واپس get_drivers(نتیجہ, بٹ);
	}

	bool get_consumers(pool<پورٹ_ثنائی_ہندسہ_2> &نتیجہ, RTLIL::SigSpec اشارہ) ثابت
	{
		std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ = sigmap(اشارہ);
		واپس get_consumers(نتیجہ, بٹ);
	}

	bool get_inputs(pool<RTLIL::Sig_ثنائی_ہندسہ> &نتیجہ, RTLIL::SigSpec اشارہ) ثابت
	{
		std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ = sigmap(اشارہ);
		واپس get_inputs(نتیجہ, بٹ);
	}

	bool get_outputs(pool<RTLIL::Sig_ثنائی_ہندسہ> &نتیجہ, RTLIL::SigSpec اشارہ) ثابت
	{
		std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ = sigmap(اشارہ);
		واپس get_outputs(نتیجہ, بٹ);
	}

	// has_* methods -- call by reference

	سانچہ<typename T>
	اندرونی bool has_drivers(ثابت T &sig) ثابت {
		pool<پورٹ_ثنائی_ہندسہ_2> نتیجہ;
		واپس get_drivers(نتیجہ, sig);
	}

	سانچہ<typename T>
	اندرونی bool has_consumers(ثابت T &sig) ثابت {
		pool<پورٹ_ثنائی_ہندسہ_2> نتیجہ;
		واپس get_consumers(نتیجہ, sig);
	}

	سانچہ<typename T>
	اندرونی bool has_inputs(ثابت T &sig) ثابت {
		pool<RTLIL::Sig_ثنائی_ہندسہ> نتیجہ;
		واپس get_inputs(نتیجہ, sig);
	}

	سانچہ<typename T>
	اندرونی bool has_outputs(ثابت T &sig) ثابت {
		pool<RTLIL::Sig_ثنائی_ہندسہ> نتیجہ;
		واپس get_outputs(نتیجہ, sig);
	}

	// has_* methods -- call by value

	اندرونی bool has_drivers(RTLIL::SigSpec sig) ثابت {
		pool<پورٹ_ثنائی_ہندسہ_2> نتیجہ;
		واپس get_drivers(نتیجہ, sig);
	}

	اندرونی bool has_consumers(RTLIL::SigSpec sig) ثابت {
		pool<پورٹ_ثنائی_ہندسہ_2> نتیجہ;
		واپس get_consumers(نتیجہ, sig);
	}

	اندرونی bool has_inputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::Sig_ثنائی_ہندسہ> نتیجہ;
		واپس get_inputs(نتیجہ, sig);
	}

	اندرونی bool has_outputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::Sig_ثنائی_ہندسہ> نتیجہ;
		واپس get_outputs(نتیجہ, sig);
	}
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
