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

#جے_بنا_تعریف MODTOOLS_H
#تعریف_کرو MODTOOLS_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/celltypes.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

بنية Mod_اشاریہ : عام RTLIL::Monitor
{
	بنية اشارہ_گر_Ordered_Sig_ثنائی_ہندسہ : عام RTLIL::Sig_ثنائی_ہندسہ {
		اشارہ_گر_Ordered_Sig_ثنائی_ہندسہ(Sig_ثنائی_ہندسہ s) {
			wire = s.wire;
			إذا (wire)
				وتھ = s.وتھ;
			وإلا
				انکڑے = s.انکڑے;
		}
		مضمن bool عامل<(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت {
			إذا (wire == other.wire)
				أرجع wire ? (وتھ < other.وتھ) : (انکڑے < other.انکڑے);
			إذا (wire != nullptr && other.wire != nullptr)
				أرجع wire < other.wire; // look here
			أرجع (wire != nullptr) < (other.wire != nullptr);
		}
	};
	بنية پورٹ_Info {
		RTLIL::Cell* cell;
		RTLIL::IdString پورٹ;
		صحيح وتھ;

		پورٹ_Info() : cell(), پورٹ(), وتھ() { }
		پورٹ_Info(RTLIL::Cell* _c, RTLIL::IdString _p, صحيح _o) : cell(_c), پورٹ(_p), وتھ(_o) { }

		bool عامل<(ثابت پورٹ_Info &other) ثابت {
			إذا (cell != other.cell)
				أرجع cell < other.cell;
			إذا (وتھ != other.وتھ)
				أرجع وتھ < other.وتھ;
			أرجع پورٹ < other.پورٹ;
		}

		bool عامل==(ثابت پورٹ_Info &other) ثابت {
			أرجع cell == other.cell && پورٹ == other.پورٹ && وتھ == other.وتھ;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(پورٹ);
			h.eat(وتھ);
			أرجع h;
		}
	};

	بنية Sig_ثنائی_ہندسہ_Info
	{
		bool is_داخلہ, is_output;
		pool<پورٹ_Info> ports;

		Sig_ثنائی_ہندسہ_Info() : is_داخلہ(false), is_output(false) { }

		bool عامل==(ثابت Sig_ثنائی_ہندسہ_Info &other) ثابت {
			أرجع is_داخلہ == other.is_داخلہ && is_output == other.is_output && ports == other.ports;
		}

		فراغ ملاؤ(ثابت Sig_ثنائی_ہندسہ_Info &other)
		{
			is_داخلہ = is_داخلہ || other.is_داخلہ;
			is_output = is_output || other.is_output;
			ports.داخل_کرو(other.ports.begin(), other.ports.سرا());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<اشارہ_گر_Ordered_Sig_ثنائی_ہندسہ, Sig_ثنائی_ہندسہ_Info> database;
	صحيح auto_reload_counter;
	bool auto_reload_module;

	فراغ پورٹ_جوڑو(RTLIL::Cell *cell, RTLIL::IdString پورٹ, ثابت RTLIL::SigSpec &sig)
	{
		لكل (صحيح i = 0; i < Get_ناپ(sig); i++) {
			RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ = sigmap(sig[i]);
			إذا (ثنائی_ہندسہ.wire)
				database[ثنائی_ہندسہ].ports.داخل_کرو(پورٹ_Info(cell, پورٹ, i));
		}
	}

	فراغ پورٹ_del(RTLIL::Cell *cell, RTLIL::IdString پورٹ, ثابت RTLIL::SigSpec &sig)
	{
		لكل (صحيح i = 0; i < Get_ناپ(sig); i++) {
			RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ = sigmap(sig[i]);
			إذا (ثنائی_ہندسہ.wire) {
				database[ثنائی_ہندسہ].ports.erase(پورٹ_Info(cell, پورٹ, i));
				إذا (!database[ثنائی_ہندسہ].is_داخلہ && !database[ثنائی_ہندسہ].is_output && database[ثنائی_ہندسہ].ports.خالی_تھاواں())
					database.erase(ثنائی_ہندسہ);
			}
		}
	}

	ثابت Sig_ثنائی_ہندسہ_Info &info(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
	{
		أرجع database[sigmap(ثنائی_ہندسہ)];
	}

	فراغ reload_module(bool reset_sigmap = true)
	{
		إذا (reset_sigmap) {
			sigmap.clear();
			sigmap.سیٹ(module);
		}

		database.clear();
		لكل (خودکار wire : module->wires())
			إذا (wire->پورٹ_داخلہ || wire->پورٹ_output)
				لكل (صحيح i = 0; i < Get_ناپ(wire); i++) {
					RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ = sigmap(RTLIL::Sig_ثنائی_ہندسہ(wire, i));
					إذا (ثنائی_ہندسہ.wire && wire->پورٹ_داخلہ)
						database[ثنائی_ہندسہ].is_داخلہ = true;
					إذا (ثنائی_ہندسہ.wire && wire->پورٹ_output)
						database[ثنائی_ہندسہ].is_output = true;
				}
		لكل (خودکار cell : module->cells())
			لكل (خودکار &conn : cell->connections())
				پورٹ_جوڑو(cell, conn.پہلا, conn.دوجا);

		إذا (auto_reload_module) {
			إذا (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#جے_بنا_تعریف NDEBUG
		إذا (auto_reload_module)
			أرجع true;

		لكل (خودکار it : database)
			log_assert(it.پہلا == sigmap(it.پہلا));

		خودکار database_bak = std::move(database);
		reload_module(false);

		إذا (!(database == database_bak))
		{
			لكل (خودکار &it : database_bak)
				إذا (!database.گنتی(it.پہلا))
					لوگارتھم("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.پہلا));

			لكل (خودکار &it : database)
				إذا (!database_bak.گنتی(it.پہلا))
					لوگارتھم("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.پہلا));
				وإلا إذا (!(it.دوجا == database_bak.at(it.پہلا)))
					لوگارتھم("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.پہلا));

			أرجع false;
		}
#جے_انت
		أرجع true;
	}

	فراغ check()
	{
		log_assert(ok());
	}

	فراغ notify_connect(RTLIL::Cell *cell, RTLIL::IdString پورٹ, ثابت RTLIL::SigSpec &old_sig, ثابت RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		إذا (auto_reload_module)
			أرجع;

		پورٹ_del(cell, پورٹ, old_sig);
		پورٹ_جوڑو(cell, پورٹ, sig);
	}

	فراغ notify_connect(RTLIL::Module *mod, ثابت RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		إذا (auto_reload_module)
			أرجع;

		لكل (صحيح i = 0; i < Get_ناپ(sigsig.پہلا); i++)
		{
			RTLIL::Sig_ثنائی_ہندسہ lhs = sigmap(sigsig.پہلا[i]);
			RTLIL::Sig_ثنائی_ہندسہ rhs = sigmap(sigsig.دوجا[i]);
			bool has_lhs = database.گنتی(lhs) != 0;
			bool has_rhs = database.گنتی(rhs) != 0;

			إذا (!has_lhs && !has_rhs) {
				sigmap.جوڑو(lhs, rhs);
			} وإلا
			إذا (!has_rhs) {
				Sig_ثنائی_ہندسہ_Info new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.جوڑو(lhs, rhs);
				lhs = sigmap(lhs);
				إذا (lhs.wire)
					database[lhs] = new_info;
			} وإلا
			إذا (!has_lhs) {
				Sig_ثنائی_ہندسہ_Info new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.جوڑو(lhs, rhs);
				rhs = sigmap(rhs);
				إذا (rhs.wire)
					database[rhs] = new_info;
			} وإلا {
				Sig_ثنائی_ہندسہ_Info new_info = database.at(lhs);
				new_info.ملاؤ(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.جوڑو(lhs, rhs);
				rhs = sigmap(rhs);
				إذا (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	فراغ notify_connect(RTLIL::Module *mod, ثابت std::سمتیہ<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	فراغ notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	Mod_اشاریہ(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.داخل_کرو(this);
	}

	~Mod_اشاریہ()
	{
		module->monitors.erase(this);
	}

	Sig_ثنائی_ہندسہ_Info *query(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
	{
		إذا (auto_reload_module)
			reload_module();

		خودکار it = database.لبھو(sigmap(ثنائی_ہندسہ));
		إذا (it == database.سرا())
			أرجع nullptr;
		وإلا
			أرجع &it->دوجا;
	}

	bool query_is_داخلہ(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
	{
		ثابت Sig_ثنائی_ہندسہ_Info *info = query(ثنائی_ہندسہ);
		إذا (info == nullptr)
			أرجع false;
		أرجع info->is_داخلہ;
	}

	bool query_is_output(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
	{
		ثابت Sig_ثنائی_ہندسہ_Info *info = query(ثنائی_ہندسہ);
		إذا (info == nullptr)
			أرجع false;
		أرجع info->is_output;
	}

	pool<پورٹ_Info> &query_ports(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
	{
		ساكن pool<پورٹ_Info> خالی_تھاواں_نتیجہ_سیٹ;
		Sig_ثنائی_ہندسہ_Info *info = query(ثنائی_ہندسہ);
		إذا (info == nullptr)
			أرجع خالی_تھاواں_نتیجہ_سیٹ;
		أرجع info->ports;
	}

	فراغ dump_db()
	{
		لوگارتھم("--- ModIndex Dump ---\n");

		إذا (auto_reload_module) {
			لوگارتھم("AUTO-RELOAD\n");
			reload_module();
		}

		لكل (خودکار &it : database) {
			لوگارتھم("BIT %s:\n", log_signal(it.پہلا));
			إذا (it.دوجا.is_داخلہ)
				لوگارتھم("  PRIMARY INPUT\n");
			إذا (it.دوجا.is_output)
				لوگارتھم("  PRIMARY OUTPUT\n");
			لكل (خودکار &پورٹ : it.دوجا.ports)
				لوگارتھم("  PORT: %s.%s[%d] (%s)\n", پورٹ.cell,
						PooledName(module, پورٹ.پورٹ).unescape(), پورٹ.وتھ, پورٹ.cell->type.unescape());
		}
	}
};

بنية ModWalker
{
	بنية پورٹ_ثنائی_ہندسہ_2
	{
		RTLIL::Cell *cell;
		RTLIL::IdString پورٹ;
		صحيح وتھ;
		پورٹ_ثنائی_ہندسہ_2(Cell* c, IdString p, صحيح o) : cell(c), پورٹ(p), وتھ(o) {}

		bool عامل<(ثابت پورٹ_ثنائی_ہندسہ_2 &other) ثابت {
			إذا (cell != other.cell)
				أرجع cell < other.cell;
			إذا (پورٹ != other.پورٹ)
				أرجع پورٹ < other.پورٹ;
			أرجع وتھ < other.وتھ;
		}

		bool عامل==(ثابت پورٹ_ثنائی_ہندسہ_2 &other) ثابت {
			أرجع cell == other.cell && پورٹ == other.پورٹ && وتھ == other.وتھ;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(پورٹ);
			h.eat(وتھ);
			أرجع h;
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

	فراغ جوڑو_wire(RTLIL::Wire *wire)
	{
		إذا (wire->پورٹ_داخلہ) {
			std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ = sigmap(wire);
			لكل (خودکار ثنائی_ہندسہ : بٹ)
				إذا (ثنائی_ہندسہ.wire != عدم)
					signal_inputs.داخل_کرو(ثنائی_ہندسہ);
		}

		إذا (wire->پورٹ_output) {
			std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ = sigmap(wire);
			لكل (خودکار ثنائی_ہندسہ : بٹ)
				إذا (ثنائی_ہندسہ.wire != عدم)
					signal_outputs.داخل_کرو(ثنائی_ہندسہ);
		}
	}

	فراغ جوڑو_cell_پورٹ(RTLIL::Cell *cell, RTLIL::IdString پورٹ, std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ, bool is_output, bool is_داخلہ)
	{
		لكل (صحيح i = 0; i < صحيح(بٹ.ناپ()); i++)
			إذا (بٹ[i].wire != عدم) {
				پورٹ_ثنائی_ہندسہ_2 pbit {cell, پورٹ, i};
				إذا (is_output) {
					signal_drivers[بٹ[i]].داخل_کرو(pbit);
					cell_outputs[cell].داخل_کرو(بٹ[i]);
				}
				إذا (is_داخلہ) {
					signal_consumers[بٹ[i]].داخل_کرو(pbit);
					cell_inputs[cell].داخل_کرو(بٹ[i]);
				}
			}
	}

	فراغ جوڑو_cell(RTLIL::Cell *cell)
	{
		إذا (ct.cell_معلوم(cell->type)) {
			لكل (خودکار &conn : cell->connections())
				جوڑو_cell_پورٹ(cell, conn.پہلا, sigmap(conn.دوجا),
						ct.cell_output(cell->type, conn.پہلا),
						ct.cell_داخلہ(cell->type, conn.پہلا));
		} وإلا {
			لكل (خودکار &conn : cell->connections())
				جوڑو_cell_پورٹ(cell, conn.پہلا, sigmap(conn.دوجا), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(عدم)
	{
		ct.تیاری(design);
		إذا (module)
			تیاری(module);
	}

	فراغ تیاری(RTLIL::Module *module, CellTypes *filter_ct = عدم)
	{
		this->module = module;

		sigmap.سیٹ(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		لكل (خودکار &it : module->wires_)
			جوڑو_wire(it.دوجا);
		لكل (خودکار &it : module->cells_)
			إذا (filter_ct == عدم || filter_ct->cell_معلوم(it.دوجا->type))
				جوڑو_cell(it.دوجا);
	}

	// get_* methods -- single RTLIL::SigBit

	مضمن bool get_drivers(pool<پورٹ_ثنائی_ہندسہ_2> &نتیجہ, RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		bool لبھیا = false;
		إذا (signal_drivers.گنتی(ثنائی_ہندسہ)) {
			ثابت pool<پورٹ_ثنائی_ہندسہ_2> &r = signal_drivers.at(ثنائی_ہندسہ);
			نتیجہ.داخل_کرو(r.begin(), r.سرا());
			لبھیا = true;
		}
		أرجع لبھیا;
	}

	مضمن bool get_consumers(pool<پورٹ_ثنائی_ہندسہ_2> &نتیجہ, RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		bool لبھیا = false;
		إذا (signal_consumers.گنتی(ثنائی_ہندسہ)) {
			ثابت pool<پورٹ_ثنائی_ہندسہ_2> &r = signal_consumers.at(ثنائی_ہندسہ);
			نتیجہ.داخل_کرو(r.begin(), r.سرا());
			لبھیا = true;
		}
		أرجع لبھیا;
	}

	مضمن bool get_inputs(pool<RTLIL::Sig_ثنائی_ہندسہ> &نتیجہ, RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		bool لبھیا = false;
		إذا (signal_inputs.گنتی(ثنائی_ہندسہ))
			نتیجہ.داخل_کرو(ثنائی_ہندسہ), لبھیا = true;
		أرجع لبھیا;
	}

	مضمن bool get_outputs(pool<RTLIL::Sig_ثنائی_ہندسہ> &نتیجہ, RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		bool لبھیا = false;
		إذا (signal_outputs.گنتی(ثنائی_ہندسہ))
			نتیجہ.داخل_کرو(ثنائی_ہندسہ), لبھیا = true;
		أرجع لبھیا;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	سانچا<typename T>
	مضمن bool get_drivers(pool<پورٹ_ثنائی_ہندسہ_2> &نتیجہ, ثابت T &بٹ) ثابت
	{
		bool لبھیا = false;
		لكل (RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ : بٹ)
			إذا (signal_drivers.گنتی(ثنائی_ہندسہ)) {
				ثابت pool<پورٹ_ثنائی_ہندسہ_2> &r = signal_drivers.at(ثنائی_ہندسہ);
				نتیجہ.داخل_کرو(r.begin(), r.سرا());
				لبھیا = true;
			}
		أرجع لبھیا;
	}

	سانچا<typename T>
	مضمن bool get_consumers(pool<پورٹ_ثنائی_ہندسہ_2> &نتیجہ, ثابت T &بٹ) ثابت
	{
		bool لبھیا = false;
		لكل (RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ : بٹ)
			إذا (signal_consumers.گنتی(ثنائی_ہندسہ)) {
				ثابت pool<پورٹ_ثنائی_ہندسہ_2> &r = signal_consumers.at(ثنائی_ہندسہ);
				نتیجہ.داخل_کرو(r.begin(), r.سرا());
				لبھیا = true;
			}
		أرجع لبھیا;
	}

	سانچا<typename T>
	مضمن bool get_inputs(pool<RTLIL::Sig_ثنائی_ہندسہ> &نتیجہ, ثابت T &بٹ) ثابت
	{
		bool لبھیا = false;
		لكل (RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ : بٹ)
			إذا (signal_inputs.گنتی(ثنائی_ہندسہ))
				نتیجہ.داخل_کرو(ثنائی_ہندسہ), لبھیا = true;
		أرجع لبھیا;
	}

	سانچا<typename T>
	مضمن bool get_outputs(pool<RTLIL::Sig_ثنائی_ہندسہ> &نتیجہ, ثابت T &بٹ) ثابت
	{
		bool لبھیا = false;
		لكل (RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ : بٹ)
			إذا (signal_outputs.گنتی(ثنائی_ہندسہ))
				نتیجہ.داخل_کرو(ثنائی_ہندسہ), لبھیا = true;
		أرجع لبھیا;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<پورٹ_ثنائی_ہندسہ_2> &نتیجہ, RTLIL::SigSpec إشارة) ثابت
	{
		std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ = sigmap(إشارة);
		أرجع get_drivers(نتیجہ, بٹ);
	}

	bool get_consumers(pool<پورٹ_ثنائی_ہندسہ_2> &نتیجہ, RTLIL::SigSpec إشارة) ثابت
	{
		std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ = sigmap(إشارة);
		أرجع get_consumers(نتیجہ, بٹ);
	}

	bool get_inputs(pool<RTLIL::Sig_ثنائی_ہندسہ> &نتیجہ, RTLIL::SigSpec إشارة) ثابت
	{
		std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ = sigmap(إشارة);
		أرجع get_inputs(نتیجہ, بٹ);
	}

	bool get_outputs(pool<RTLIL::Sig_ثنائی_ہندسہ> &نتیجہ, RTLIL::SigSpec إشارة) ثابت
	{
		std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ = sigmap(إشارة);
		أرجع get_outputs(نتیجہ, بٹ);
	}

	// has_* methods -- call by reference

	سانچا<typename T>
	مضمن bool has_drivers(ثابت T &sig) ثابت {
		pool<پورٹ_ثنائی_ہندسہ_2> نتیجہ;
		أرجع get_drivers(نتیجہ, sig);
	}

	سانچا<typename T>
	مضمن bool has_consumers(ثابت T &sig) ثابت {
		pool<پورٹ_ثنائی_ہندسہ_2> نتیجہ;
		أرجع get_consumers(نتیجہ, sig);
	}

	سانچا<typename T>
	مضمن bool has_inputs(ثابت T &sig) ثابت {
		pool<RTLIL::Sig_ثنائی_ہندسہ> نتیجہ;
		أرجع get_inputs(نتیجہ, sig);
	}

	سانچا<typename T>
	مضمن bool has_outputs(ثابت T &sig) ثابت {
		pool<RTLIL::Sig_ثنائی_ہندسہ> نتیجہ;
		أرجع get_outputs(نتیجہ, sig);
	}

	// has_* methods -- call by value

	مضمن bool has_drivers(RTLIL::SigSpec sig) ثابت {
		pool<پورٹ_ثنائی_ہندسہ_2> نتیجہ;
		أرجع get_drivers(نتیجہ, sig);
	}

	مضمن bool has_consumers(RTLIL::SigSpec sig) ثابت {
		pool<پورٹ_ثنائی_ہندسہ_2> نتیجہ;
		أرجع get_consumers(نتیجہ, sig);
	}

	مضمن bool has_inputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::Sig_ثنائی_ہندسہ> نتیجہ;
		أرجع get_inputs(نتیجہ, sig);
	}

	مضمن bool has_outputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::Sig_ثنائی_ہندسہ> نتیجہ;
		أرجع get_outputs(نتیجہ, sig);
	}
};

YOSYS_NAMESPACE_سرا

#جے_انت
