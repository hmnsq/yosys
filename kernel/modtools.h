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

#جيڪڏهن_وصف_نه_ٿيل MODTOOLS_H
#وصف_ڪريو MODTOOLS_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/celltypes.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

بنية Mod_اشاريو : عام RTLIL::Monitor
{
	بنية اشارو_Ordered_Sig_ٻٽو_انگ : عام RTLIL::Sig_ٻٽو_انگ {
		اشارو_Ordered_Sig_ٻٽو_انگ(Sig_ٻٽو_انگ s) {
			wire = s.wire;
			إذا (wire)
				وٿي = s.وٿي;
			وإلا
				معلومات = s.معلومات;
		}
		مضمن bool عامل<(ثابت RTLIL::Sig_ٻٽو_انگ &other) ثابت {
			إذا (wire == other.wire)
				أرجع wire ? (وٿي < other.وٿي) : (معلومات < other.معلومات);
			إذا (wire != nullptr && other.wire != nullptr)
				أرجع wire < other.wire; // look here
			أرجع (wire != nullptr) < (other.wire != nullptr);
		}
	};
	بنية پورٽ_Info {
		RTLIL::Cell* cell;
		RTLIL::IdString پورٽ;
		صحيح وٿي;

		پورٽ_Info() : cell(), پورٽ(), وٿي() { }
		پورٽ_Info(RTLIL::Cell* _c, RTLIL::IdString _p, صحيح _o) : cell(_c), پورٽ(_p), وٿي(_o) { }

		bool عامل<(ثابت پورٽ_Info &other) ثابت {
			إذا (cell != other.cell)
				أرجع cell < other.cell;
			إذا (وٿي != other.وٿي)
				أرجع وٿي < other.وٿي;
			أرجع پورٽ < other.پورٽ;
		}

		bool عامل==(ثابت پورٽ_Info &other) ثابت {
			أرجع cell == other.cell && پورٽ == other.پورٽ && وٿي == other.وٿي;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(پورٽ);
			h.eat(وٿي);
			أرجع h;
		}
	};

	بنية Sig_ٻٽو_انگ_Info
	{
		bool is_داخلا, is_output;
		pool<پورٽ_Info> ports;

		Sig_ٻٽو_انگ_Info() : is_داخلا(false), is_output(false) { }

		bool عامل==(ثابت Sig_ٻٽو_انگ_Info &other) ثابت {
			أرجع is_داخلا == other.is_داخلا && is_output == other.is_output && ports == other.ports;
		}

		فراغ ملايو(ثابت Sig_ٻٽو_انگ_Info &other)
		{
			is_داخلا = is_داخلا || other.is_داخلا;
			is_output = is_output || other.is_output;
			ports.وجهو(other.ports.begin(), other.ports.پڇاڙي());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<اشارو_Ordered_Sig_ٻٽو_انگ, Sig_ٻٽو_انگ_Info> database;
	صحيح auto_reload_counter;
	bool auto_reload_module;

	فراغ پورٽ_جوڙيو(RTLIL::Cell *cell, RTLIL::IdString پورٽ, ثابت RTLIL::SigSpec &sig)
	{
		لكل (صحيح i = 0; i < Get_سائيز(sig); i++) {
			RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ = sigmap(sig[i]);
			إذا (ٻٽو_انگ.wire)
				database[ٻٽو_انگ].ports.وجهو(پورٽ_Info(cell, پورٽ, i));
		}
	}

	فراغ پورٽ_del(RTLIL::Cell *cell, RTLIL::IdString پورٽ, ثابت RTLIL::SigSpec &sig)
	{
		لكل (صحيح i = 0; i < Get_سائيز(sig); i++) {
			RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ = sigmap(sig[i]);
			إذا (ٻٽو_انگ.wire) {
				database[ٻٽو_انگ].ports.erase(پورٽ_Info(cell, پورٽ, i));
				إذا (!database[ٻٽو_انگ].is_داخلا && !database[ٻٽو_انگ].is_output && database[ٻٽو_انگ].ports.خالي_جايون())
					database.erase(ٻٽو_انگ);
			}
		}
	}

	ثابت Sig_ٻٽو_انگ_Info &info(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ)
	{
		أرجع database[sigmap(ٻٽو_انگ)];
	}

	فراغ reload_module(bool reset_sigmap = true)
	{
		إذا (reset_sigmap) {
			sigmap.clear();
			sigmap.سيٽ(module);
		}

		database.clear();
		لكل (خودڪار wire : module->wires())
			إذا (wire->پورٽ_داخلا || wire->پورٽ_output)
				لكل (صحيح i = 0; i < Get_سائيز(wire); i++) {
					RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ = sigmap(RTLIL::Sig_ٻٽو_انگ(wire, i));
					إذا (ٻٽو_انگ.wire && wire->پورٽ_داخلا)
						database[ٻٽو_انگ].is_داخلا = true;
					إذا (ٻٽو_انگ.wire && wire->پورٽ_output)
						database[ٻٽو_انگ].is_output = true;
				}
		لكل (خودڪار cell : module->cells())
			لكل (خودڪار &conn : cell->connections())
				پورٽ_جوڙيو(cell, conn.پهريون, conn.ٻيو);

		إذا (auto_reload_module) {
			إذا (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#جيڪڏهن_وصف_نه_ٿيل NDEBUG
		إذا (auto_reload_module)
			أرجع true;

		لكل (خودڪار it : database)
			log_assert(it.پهريون == sigmap(it.پهريون));

		خودڪار database_bak = std::move(database);
		reload_module(false);

		إذا (!(database == database_bak))
		{
			لكل (خودڪار &it : database_bak)
				إذا (!database.ڳڻپ(it.پهريون))
					لاگرٿم("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.پهريون));

			لكل (خودڪار &it : database)
				إذا (!database_bak.ڳڻپ(it.پهريون))
					لاگرٿم("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.پهريون));
				وإلا إذا (!(it.ٻيو == database_bak.at(it.پهريون)))
					لاگرٿم("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.پهريون));

			أرجع false;
		}
#جيڪڏهن_پورو
		أرجع true;
	}

	فراغ check()
	{
		log_assert(ok());
	}

	فراغ notify_connect(RTLIL::Cell *cell, RTLIL::IdString پورٽ, ثابت RTLIL::SigSpec &old_sig, ثابت RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		إذا (auto_reload_module)
			أرجع;

		پورٽ_del(cell, پورٽ, old_sig);
		پورٽ_جوڙيو(cell, پورٽ, sig);
	}

	فراغ notify_connect(RTLIL::Module *mod, ثابت RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		إذا (auto_reload_module)
			أرجع;

		لكل (صحيح i = 0; i < Get_سائيز(sigsig.پهريون); i++)
		{
			RTLIL::Sig_ٻٽو_انگ lhs = sigmap(sigsig.پهريون[i]);
			RTLIL::Sig_ٻٽو_انگ rhs = sigmap(sigsig.ٻيو[i]);
			bool has_lhs = database.ڳڻپ(lhs) != 0;
			bool has_rhs = database.ڳڻپ(rhs) != 0;

			إذا (!has_lhs && !has_rhs) {
				sigmap.جوڙيو(lhs, rhs);
			} وإلا
			إذا (!has_rhs) {
				Sig_ٻٽو_انگ_Info new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.جوڙيو(lhs, rhs);
				lhs = sigmap(lhs);
				إذا (lhs.wire)
					database[lhs] = new_info;
			} وإلا
			إذا (!has_lhs) {
				Sig_ٻٽو_انگ_Info new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.جوڙيو(lhs, rhs);
				rhs = sigmap(rhs);
				إذا (rhs.wire)
					database[rhs] = new_info;
			} وإلا {
				Sig_ٻٽو_انگ_Info new_info = database.at(lhs);
				new_info.ملايو(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.جوڙيو(lhs, rhs);
				rhs = sigmap(rhs);
				إذا (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	فراغ notify_connect(RTLIL::Module *mod, ثابت std::ويڪٽر<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	فراغ notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	Mod_اشاريو(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.وجهو(this);
	}

	~Mod_اشاريو()
	{
		module->monitors.erase(this);
	}

	Sig_ٻٽو_انگ_Info *query(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ)
	{
		إذا (auto_reload_module)
			reload_module();

		خودڪار it = database.ڳوليو(sigmap(ٻٽو_انگ));
		إذا (it == database.پڇاڙي())
			أرجع nullptr;
		وإلا
			أرجع &it->ٻيو;
	}

	bool query_is_داخلا(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ)
	{
		ثابت Sig_ٻٽو_انگ_Info *info = query(ٻٽو_انگ);
		إذا (info == nullptr)
			أرجع false;
		أرجع info->is_داخلا;
	}

	bool query_is_output(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ)
	{
		ثابت Sig_ٻٽو_انگ_Info *info = query(ٻٽو_انگ);
		إذا (info == nullptr)
			أرجع false;
		أرجع info->is_output;
	}

	pool<پورٽ_Info> &query_ports(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ)
	{
		ساكن pool<پورٽ_Info> خالي_جايون_نتيجو_سيٽ;
		Sig_ٻٽو_انگ_Info *info = query(ٻٽو_انگ);
		إذا (info == nullptr)
			أرجع خالي_جايون_نتيجو_سيٽ;
		أرجع info->ports;
	}

	فراغ dump_db()
	{
		لاگرٿم("--- ModIndex Dump ---\n");

		إذا (auto_reload_module) {
			لاگرٿم("AUTO-RELOAD\n");
			reload_module();
		}

		لكل (خودڪار &it : database) {
			لاگرٿم("BIT %s:\n", log_signal(it.پهريون));
			إذا (it.ٻيو.is_داخلا)
				لاگرٿم("  PRIMARY INPUT\n");
			إذا (it.ٻيو.is_output)
				لاگرٿم("  PRIMARY OUTPUT\n");
			لكل (خودڪار &پورٽ : it.ٻيو.ports)
				لاگرٿم("  PORT: %s.%s[%d] (%s)\n", پورٽ.cell,
						PooledName(module, پورٽ.پورٽ).unescape(), پورٽ.وٿي, پورٽ.cell->type.unescape());
		}
	}
};

بنية ModWalker
{
	بنية پورٽ_ٻٽو_انگ_2
	{
		RTLIL::Cell *cell;
		RTLIL::IdString پورٽ;
		صحيح وٿي;
		پورٽ_ٻٽو_انگ_2(Cell* c, IdString p, صحيح o) : cell(c), پورٽ(p), وٿي(o) {}

		bool عامل<(ثابت پورٽ_ٻٽو_انگ_2 &other) ثابت {
			إذا (cell != other.cell)
				أرجع cell < other.cell;
			إذا (پورٽ != other.پورٽ)
				أرجع پورٽ < other.پورٽ;
			أرجع وٿي < other.وٿي;
		}

		bool عامل==(ثابت پورٽ_ٻٽو_انگ_2 &other) ثابت {
			أرجع cell == other.cell && پورٽ == other.پورٽ && وٿي == other.وٿي;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(پورٽ);
			h.eat(وٿي);
			أرجع h;
		}
	};

	RTLIL::Design *design;
	RTLIL::Module *module;

	NewCellTypes ct;
	SigMap sigmap;

	dict<RTLIL::Sig_ٻٽو_انگ, pool<پورٽ_ٻٽو_انگ_2>> signal_drivers;
	dict<RTLIL::Sig_ٻٽو_انگ, pool<پورٽ_ٻٽو_انگ_2>> signal_consumers;
	pool<RTLIL::Sig_ٻٽو_انگ> signal_inputs, signal_outputs;

	dict<RTLIL::Cell*, pool<RTLIL::Sig_ٻٽو_انگ>> cell_outputs, cell_inputs;

	فراغ جوڙيو_wire(RTLIL::Wire *wire)
	{
		إذا (wire->پورٽ_داخلا) {
			std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> بٽ = sigmap(wire);
			لكل (خودڪار ٻٽو_انگ : بٽ)
				إذا (ٻٽو_انگ.wire != عدم)
					signal_inputs.وجهو(ٻٽو_انگ);
		}

		إذا (wire->پورٽ_output) {
			std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> بٽ = sigmap(wire);
			لكل (خودڪار ٻٽو_انگ : بٽ)
				إذا (ٻٽو_انگ.wire != عدم)
					signal_outputs.وجهو(ٻٽو_انگ);
		}
	}

	فراغ جوڙيو_cell_پورٽ(RTLIL::Cell *cell, RTLIL::IdString پورٽ, std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> بٽ, bool is_output, bool is_داخلا)
	{
		لكل (صحيح i = 0; i < صحيح(بٽ.سائيز()); i++)
			إذا (بٽ[i].wire != عدم) {
				پورٽ_ٻٽو_انگ_2 pbit {cell, پورٽ, i};
				إذا (is_output) {
					signal_drivers[بٽ[i]].وجهو(pbit);
					cell_outputs[cell].وجهو(بٽ[i]);
				}
				إذا (is_داخلا) {
					signal_consumers[بٽ[i]].وجهو(pbit);
					cell_inputs[cell].وجهو(بٽ[i]);
				}
			}
	}

	فراغ جوڙيو_cell(RTLIL::Cell *cell)
	{
		إذا (ct.cell_معلوم(cell->type)) {
			لكل (خودڪار &conn : cell->connections())
				جوڙيو_cell_پورٽ(cell, conn.پهريون, sigmap(conn.ٻيو),
						ct.cell_output(cell->type, conn.پهريون),
						ct.cell_داخلا(cell->type, conn.پهريون));
		} وإلا {
			لكل (خودڪار &conn : cell->connections())
				جوڙيو_cell_پورٽ(cell, conn.پهريون, sigmap(conn.ٻيو), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(عدم)
	{
		ct.تياري(design);
		إذا (module)
			تياري(module);
	}

	فراغ تياري(RTLIL::Module *module, CellTypes *filter_ct = عدم)
	{
		this->module = module;

		sigmap.سيٽ(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		لكل (خودڪار &it : module->wires_)
			جوڙيو_wire(it.ٻيو);
		لكل (خودڪار &it : module->cells_)
			إذا (filter_ct == عدم || filter_ct->cell_معلوم(it.ٻيو->type))
				جوڙيو_cell(it.ٻيو);
	}

	// get_* methods -- single RTLIL::SigBit

	مضمن bool get_drivers(pool<پورٽ_ٻٽو_انگ_2> &نتيجو, RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ) ثابت
	{
		bool مليو = false;
		إذا (signal_drivers.ڳڻپ(ٻٽو_انگ)) {
			ثابت pool<پورٽ_ٻٽو_انگ_2> &r = signal_drivers.at(ٻٽو_انگ);
			نتيجو.وجهو(r.begin(), r.پڇاڙي());
			مليو = true;
		}
		أرجع مليو;
	}

	مضمن bool get_consumers(pool<پورٽ_ٻٽو_انگ_2> &نتيجو, RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ) ثابت
	{
		bool مليو = false;
		إذا (signal_consumers.ڳڻپ(ٻٽو_انگ)) {
			ثابت pool<پورٽ_ٻٽو_انگ_2> &r = signal_consumers.at(ٻٽو_انگ);
			نتيجو.وجهو(r.begin(), r.پڇاڙي());
			مليو = true;
		}
		أرجع مليو;
	}

	مضمن bool get_inputs(pool<RTLIL::Sig_ٻٽو_انگ> &نتيجو, RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ) ثابت
	{
		bool مليو = false;
		إذا (signal_inputs.ڳڻپ(ٻٽو_انگ))
			نتيجو.وجهو(ٻٽو_انگ), مليو = true;
		أرجع مليو;
	}

	مضمن bool get_outputs(pool<RTLIL::Sig_ٻٽو_انگ> &نتيجو, RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ) ثابت
	{
		bool مليو = false;
		إذا (signal_outputs.ڳڻپ(ٻٽو_انگ))
			نتيجو.وجهو(ٻٽو_انگ), مليو = true;
		أرجع مليو;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	سانچو<typename T>
	مضمن bool get_drivers(pool<پورٽ_ٻٽو_انگ_2> &نتيجو, ثابت T &بٽ) ثابت
	{
		bool مليو = false;
		لكل (RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ : بٽ)
			إذا (signal_drivers.ڳڻپ(ٻٽو_انگ)) {
				ثابت pool<پورٽ_ٻٽو_انگ_2> &r = signal_drivers.at(ٻٽو_انگ);
				نتيجو.وجهو(r.begin(), r.پڇاڙي());
				مليو = true;
			}
		أرجع مليو;
	}

	سانچو<typename T>
	مضمن bool get_consumers(pool<پورٽ_ٻٽو_انگ_2> &نتيجو, ثابت T &بٽ) ثابت
	{
		bool مليو = false;
		لكل (RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ : بٽ)
			إذا (signal_consumers.ڳڻپ(ٻٽو_انگ)) {
				ثابت pool<پورٽ_ٻٽو_انگ_2> &r = signal_consumers.at(ٻٽو_انگ);
				نتيجو.وجهو(r.begin(), r.پڇاڙي());
				مليو = true;
			}
		أرجع مليو;
	}

	سانچو<typename T>
	مضمن bool get_inputs(pool<RTLIL::Sig_ٻٽو_انگ> &نتيجو, ثابت T &بٽ) ثابت
	{
		bool مليو = false;
		لكل (RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ : بٽ)
			إذا (signal_inputs.ڳڻپ(ٻٽو_انگ))
				نتيجو.وجهو(ٻٽو_انگ), مليو = true;
		أرجع مليو;
	}

	سانچو<typename T>
	مضمن bool get_outputs(pool<RTLIL::Sig_ٻٽو_انگ> &نتيجو, ثابت T &بٽ) ثابت
	{
		bool مليو = false;
		لكل (RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ : بٽ)
			إذا (signal_outputs.ڳڻپ(ٻٽو_انگ))
				نتيجو.وجهو(ٻٽو_انگ), مليو = true;
		أرجع مليو;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<پورٽ_ٻٽو_انگ_2> &نتيجو, RTLIL::SigSpec إشارة) ثابت
	{
		std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> بٽ = sigmap(إشارة);
		أرجع get_drivers(نتيجو, بٽ);
	}

	bool get_consumers(pool<پورٽ_ٻٽو_انگ_2> &نتيجو, RTLIL::SigSpec إشارة) ثابت
	{
		std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> بٽ = sigmap(إشارة);
		أرجع get_consumers(نتيجو, بٽ);
	}

	bool get_inputs(pool<RTLIL::Sig_ٻٽو_انگ> &نتيجو, RTLIL::SigSpec إشارة) ثابت
	{
		std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> بٽ = sigmap(إشارة);
		أرجع get_inputs(نتيجو, بٽ);
	}

	bool get_outputs(pool<RTLIL::Sig_ٻٽو_انگ> &نتيجو, RTLIL::SigSpec إشارة) ثابت
	{
		std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> بٽ = sigmap(إشارة);
		أرجع get_outputs(نتيجو, بٽ);
	}

	// has_* methods -- call by reference

	سانچو<typename T>
	مضمن bool has_drivers(ثابت T &sig) ثابت {
		pool<پورٽ_ٻٽو_انگ_2> نتيجو;
		أرجع get_drivers(نتيجو, sig);
	}

	سانچو<typename T>
	مضمن bool has_consumers(ثابت T &sig) ثابت {
		pool<پورٽ_ٻٽو_انگ_2> نتيجو;
		أرجع get_consumers(نتيجو, sig);
	}

	سانچو<typename T>
	مضمن bool has_inputs(ثابت T &sig) ثابت {
		pool<RTLIL::Sig_ٻٽو_انگ> نتيجو;
		أرجع get_inputs(نتيجو, sig);
	}

	سانچو<typename T>
	مضمن bool has_outputs(ثابت T &sig) ثابت {
		pool<RTLIL::Sig_ٻٽو_انگ> نتيجو;
		أرجع get_outputs(نتيجو, sig);
	}

	// has_* methods -- call by value

	مضمن bool has_drivers(RTLIL::SigSpec sig) ثابت {
		pool<پورٽ_ٻٽو_انگ_2> نتيجو;
		أرجع get_drivers(نتيجو, sig);
	}

	مضمن bool has_consumers(RTLIL::SigSpec sig) ثابت {
		pool<پورٽ_ٻٽو_انگ_2> نتيجو;
		أرجع get_consumers(نتيجو, sig);
	}

	مضمن bool has_inputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::Sig_ٻٽو_انگ> نتيجو;
		أرجع get_inputs(نتيجو, sig);
	}

	مضمن bool has_outputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::Sig_ٻٽو_انگ> نتيجو;
		أرجع get_outputs(نتيجو, sig);
	}
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
