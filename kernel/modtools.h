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

#که_نه_تعریف_شوی MODTOOLS_H
#تعریف_کړه MODTOOLS_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/celltypes.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

بنية Mod_شاخص : عام RTLIL::Monitor
{
	بنية اشاره_ګر_Ordered_Sig_بټ : عام RTLIL::Sig_بټ {
		اشاره_ګر_Ordered_Sig_بټ(Sig_بټ s) {
			wire = s.wire;
			إذا (wire)
				دننه_واټن = s.دننه_واټن;
			وإلا
				معلومات = s.معلومات;
		}
		مضمن bool چلوونکی<(ثابت RTLIL::Sig_بټ &other) ثابت {
			إذا (wire == other.wire)
				أرجع wire ? (دننه_واټن < other.دننه_واټن) : (معلومات < other.معلومات);
			إذا (wire != nullptr && other.wire != nullptr)
				أرجع wire < other.wire; // look here
			أرجع (wire != nullptr) < (other.wire != nullptr);
		}
	};
	بنية پورټ_Info {
		RTLIL::Cell* cell;
		RTLIL::IdString پورټ;
		صحيح دننه_واټن;

		پورټ_Info() : cell(), پورټ(), دننه_واټن() { }
		پورټ_Info(RTLIL::Cell* _c, RTLIL::IdString _p, صحيح _o) : cell(_c), پورټ(_p), دننه_واټن(_o) { }

		bool چلوونکی<(ثابت پورټ_Info &other) ثابت {
			إذا (cell != other.cell)
				أرجع cell < other.cell;
			إذا (دننه_واټن != other.دننه_واټن)
				أرجع دننه_واټن < other.دننه_واټن;
			أرجع پورټ < other.پورټ;
		}

		bool چلوونکی==(ثابت پورټ_Info &other) ثابت {
			أرجع cell == other.cell && پورټ == other.پورټ && دننه_واټن == other.دننه_واټن;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(پورټ);
			h.eat(دننه_واټن);
			أرجع h;
		}
	};

	بنية Sig_بټ_Info
	{
		bool is_ننوت, is_output;
		pool<پورټ_Info> ports;

		Sig_بټ_Info() : is_ننوت(false), is_output(false) { }

		bool چلوونکی==(ثابت Sig_بټ_Info &other) ثابت {
			أرجع is_ننوت == other.is_ننوت && is_output == other.is_output && ports == other.ports;
		}

		فراغ یوځای(ثابت Sig_بټ_Info &other)
		{
			is_ننوت = is_ننوت || other.is_ننوت;
			is_output = is_output || other.is_output;
			ports.ورننباسه(other.ports.begin(), other.ports.پای_ټکی());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<اشاره_ګر_Ordered_Sig_بټ, Sig_بټ_Info> database;
	صحيح auto_reload_counter;
	bool auto_reload_module;

	فراغ پورټ_جمع_کړه(RTLIL::Cell *cell, RTLIL::IdString پورټ, ثابت RTLIL::SigSpec &sig)
	{
		لكل (صحيح i = 0; i < Get_اندازه(sig); i++) {
			RTLIL::Sig_بټ بټ = sigmap(sig[i]);
			إذا (بټ.wire)
				database[بټ].ports.ورننباسه(پورټ_Info(cell, پورټ, i));
		}
	}

	فراغ پورټ_del(RTLIL::Cell *cell, RTLIL::IdString پورټ, ثابت RTLIL::SigSpec &sig)
	{
		لكل (صحيح i = 0; i < Get_اندازه(sig); i++) {
			RTLIL::Sig_بټ بټ = sigmap(sig[i]);
			إذا (بټ.wire) {
				database[بټ].ports.erase(پورټ_Info(cell, پورټ, i));
				إذا (!database[بټ].is_ننوت && !database[بټ].is_output && database[بټ].ports.تشه())
					database.erase(بټ);
			}
		}
	}

	ثابت Sig_بټ_Info &info(RTLIL::Sig_بټ بټ)
	{
		أرجع database[sigmap(بټ)];
	}

	فراغ reload_module(bool reset_sigmap = true)
	{
		إذا (reset_sigmap) {
			sigmap.clear();
			sigmap.سېټ(module);
		}

		database.clear();
		لكل (خپلکاری wire : module->wires())
			إذا (wire->پورټ_ننوت || wire->پورټ_output)
				لكل (صحيح i = 0; i < Get_اندازه(wire); i++) {
					RTLIL::Sig_بټ بټ = sigmap(RTLIL::Sig_بټ(wire, i));
					إذا (بټ.wire && wire->پورټ_ننوت)
						database[بټ].is_ننوت = true;
					إذا (بټ.wire && wire->پورټ_output)
						database[بټ].is_output = true;
				}
		لكل (خپلکاری cell : module->cells())
			لكل (خپلکاری &conn : cell->connections())
				پورټ_جمع_کړه(cell, conn.لومړی, conn.دوهم);

		إذا (auto_reload_module) {
			إذا (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#که_نه_تعریف_شوی NDEBUG
		إذا (auto_reload_module)
			أرجع true;

		لكل (خپلکاری it : database)
			log_assert(it.لومړی == sigmap(it.لومړی));

		خپلکاری database_bak = std::move(database);
		reload_module(false);

		إذا (!(database == database_bak))
		{
			لكل (خپلکاری &it : database_bak)
				إذا (!database.شمېر(it.لومړی))
					لوګاریتم("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.لومړی));

			لكل (خپلکاری &it : database)
				إذا (!database_bak.شمېر(it.لومړی))
					لوګاریتم("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.لومړی));
				وإلا إذا (!(it.دوهم == database_bak.at(it.لومړی)))
					لوګاریتم("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.لومړی));

			أرجع false;
		}
#د_که_پای
		أرجع true;
	}

	فراغ check()
	{
		log_assert(ok());
	}

	فراغ notify_connect(RTLIL::Cell *cell, RTLIL::IdString پورټ, ثابت RTLIL::SigSpec &old_sig, ثابت RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		إذا (auto_reload_module)
			أرجع;

		پورټ_del(cell, پورټ, old_sig);
		پورټ_جمع_کړه(cell, پورټ, sig);
	}

	فراغ notify_connect(RTLIL::Module *mod, ثابت RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		إذا (auto_reload_module)
			أرجع;

		لكل (صحيح i = 0; i < Get_اندازه(sigsig.لومړی); i++)
		{
			RTLIL::Sig_بټ lhs = sigmap(sigsig.لومړی[i]);
			RTLIL::Sig_بټ rhs = sigmap(sigsig.دوهم[i]);
			bool has_lhs = database.شمېر(lhs) != 0;
			bool has_rhs = database.شمېر(rhs) != 0;

			إذا (!has_lhs && !has_rhs) {
				sigmap.جمع_کړه(lhs, rhs);
			} وإلا
			إذا (!has_rhs) {
				Sig_بټ_Info new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.جمع_کړه(lhs, rhs);
				lhs = sigmap(lhs);
				إذا (lhs.wire)
					database[lhs] = new_info;
			} وإلا
			إذا (!has_lhs) {
				Sig_بټ_Info new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.جمع_کړه(lhs, rhs);
				rhs = sigmap(rhs);
				إذا (rhs.wire)
					database[rhs] = new_info;
			} وإلا {
				Sig_بټ_Info new_info = database.at(lhs);
				new_info.یوځای(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.جمع_کړه(lhs, rhs);
				rhs = sigmap(rhs);
				إذا (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	فراغ notify_connect(RTLIL::Module *mod, ثابت std::ویکټور<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	فراغ notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	Mod_شاخص(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.ورننباسه(this);
	}

	~Mod_شاخص()
	{
		module->monitors.erase(this);
	}

	Sig_بټ_Info *query(RTLIL::Sig_بټ بټ)
	{
		إذا (auto_reload_module)
			reload_module();

		خپلکاری it = database.پیدا_کړه(sigmap(بټ));
		إذا (it == database.پای_ټکی())
			أرجع nullptr;
		وإلا
			أرجع &it->دوهم;
	}

	bool query_is_ننوت(RTLIL::Sig_بټ بټ)
	{
		ثابت Sig_بټ_Info *info = query(بټ);
		إذا (info == nullptr)
			أرجع false;
		أرجع info->is_ننوت;
	}

	bool query_is_output(RTLIL::Sig_بټ بټ)
	{
		ثابت Sig_بټ_Info *info = query(بټ);
		إذا (info == nullptr)
			أرجع false;
		أرجع info->is_output;
	}

	pool<پورټ_Info> &query_ports(RTLIL::Sig_بټ بټ)
	{
		ساكن pool<پورټ_Info> تشه_پایله_سېټ;
		Sig_بټ_Info *info = query(بټ);
		إذا (info == nullptr)
			أرجع تشه_پایله_سېټ;
		أرجع info->ports;
	}

	فراغ dump_db()
	{
		لوګاریتم("--- ModIndex Dump ---\n");

		إذا (auto_reload_module) {
			لوګاریتم("AUTO-RELOAD\n");
			reload_module();
		}

		لكل (خپلکاری &it : database) {
			لوګاریتم("BIT %s:\n", log_signal(it.لومړی));
			إذا (it.دوهم.is_ننوت)
				لوګاریتم("  PRIMARY INPUT\n");
			إذا (it.دوهم.is_output)
				لوګاریتم("  PRIMARY OUTPUT\n");
			لكل (خپلکاری &پورټ : it.دوهم.ports)
				لوګاریتم("  PORT: %s.%s[%d] (%s)\n", پورټ.cell,
						PooledName(module, پورټ.پورټ).unescape(), پورټ.دننه_واټن, پورټ.cell->type.unescape());
		}
	}
};

بنية ModWalker
{
	بنية پورټ_بټ_2
	{
		RTLIL::Cell *cell;
		RTLIL::IdString پورټ;
		صحيح دننه_واټن;
		پورټ_بټ_2(Cell* c, IdString p, صحيح o) : cell(c), پورټ(p), دننه_واټن(o) {}

		bool چلوونکی<(ثابت پورټ_بټ_2 &other) ثابت {
			إذا (cell != other.cell)
				أرجع cell < other.cell;
			إذا (پورټ != other.پورټ)
				أرجع پورټ < other.پورټ;
			أرجع دننه_واټن < other.دننه_واټن;
		}

		bool چلوونکی==(ثابت پورټ_بټ_2 &other) ثابت {
			أرجع cell == other.cell && پورټ == other.پورټ && دننه_واټن == other.دننه_واټن;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(پورټ);
			h.eat(دننه_واټن);
			أرجع h;
		}
	};

	RTLIL::Design *design;
	RTLIL::Module *module;

	NewCellTypes ct;
	SigMap sigmap;

	dict<RTLIL::Sig_بټ, pool<پورټ_بټ_2>> signal_drivers;
	dict<RTLIL::Sig_بټ, pool<پورټ_بټ_2>> signal_consumers;
	pool<RTLIL::Sig_بټ> signal_inputs, signal_outputs;

	dict<RTLIL::Cell*, pool<RTLIL::Sig_بټ>> cell_outputs, cell_inputs;

	فراغ جمع_کړه_wire(RTLIL::Wire *wire)
	{
		إذا (wire->پورټ_ننوت) {
			std::ویکټور<RTLIL::Sig_بټ> بټونه = sigmap(wire);
			لكل (خپلکاری بټ : بټونه)
				إذا (بټ.wire != عدم)
					signal_inputs.ورننباسه(بټ);
		}

		إذا (wire->پورټ_output) {
			std::ویکټور<RTLIL::Sig_بټ> بټونه = sigmap(wire);
			لكل (خپلکاری بټ : بټونه)
				إذا (بټ.wire != عدم)
					signal_outputs.ورننباسه(بټ);
		}
	}

	فراغ جمع_کړه_cell_پورټ(RTLIL::Cell *cell, RTLIL::IdString پورټ, std::ویکټور<RTLIL::Sig_بټ> بټونه, bool is_output, bool is_ننوت)
	{
		لكل (صحيح i = 0; i < صحيح(بټونه.اندازه()); i++)
			إذا (بټونه[i].wire != عدم) {
				پورټ_بټ_2 pbit {cell, پورټ, i};
				إذا (is_output) {
					signal_drivers[بټونه[i]].ورننباسه(pbit);
					cell_outputs[cell].ورننباسه(بټونه[i]);
				}
				إذا (is_ننوت) {
					signal_consumers[بټونه[i]].ورننباسه(pbit);
					cell_inputs[cell].ورننباسه(بټونه[i]);
				}
			}
	}

	فراغ جمع_کړه_cell(RTLIL::Cell *cell)
	{
		إذا (ct.cell_معلوم(cell->type)) {
			لكل (خپلکاری &conn : cell->connections())
				جمع_کړه_cell_پورټ(cell, conn.لومړی, sigmap(conn.دوهم),
						ct.cell_output(cell->type, conn.لومړی),
						ct.cell_ننوت(cell->type, conn.لومړی));
		} وإلا {
			لكل (خپلکاری &conn : cell->connections())
				جمع_کړه_cell_پورټ(cell, conn.لومړی, sigmap(conn.دوهم), true, true);
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

		sigmap.سېټ(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		لكل (خپلکاری &it : module->wires_)
			جمع_کړه_wire(it.دوهم);
		لكل (خپلکاری &it : module->cells_)
			إذا (filter_ct == عدم || filter_ct->cell_معلوم(it.دوهم->type))
				جمع_کړه_cell(it.دوهم);
	}

	// get_* methods -- single RTLIL::SigBit

	مضمن bool get_drivers(pool<پورټ_بټ_2> &پایله, RTLIL::Sig_بټ بټ) ثابت
	{
		bool وموندل = false;
		إذا (signal_drivers.شمېر(بټ)) {
			ثابت pool<پورټ_بټ_2> &r = signal_drivers.at(بټ);
			پایله.ورننباسه(r.begin(), r.پای_ټکی());
			وموندل = true;
		}
		أرجع وموندل;
	}

	مضمن bool get_consumers(pool<پورټ_بټ_2> &پایله, RTLIL::Sig_بټ بټ) ثابت
	{
		bool وموندل = false;
		إذا (signal_consumers.شمېر(بټ)) {
			ثابت pool<پورټ_بټ_2> &r = signal_consumers.at(بټ);
			پایله.ورننباسه(r.begin(), r.پای_ټکی());
			وموندل = true;
		}
		أرجع وموندل;
	}

	مضمن bool get_inputs(pool<RTLIL::Sig_بټ> &پایله, RTLIL::Sig_بټ بټ) ثابت
	{
		bool وموندل = false;
		إذا (signal_inputs.شمېر(بټ))
			پایله.ورننباسه(بټ), وموندل = true;
		أرجع وموندل;
	}

	مضمن bool get_outputs(pool<RTLIL::Sig_بټ> &پایله, RTLIL::Sig_بټ بټ) ثابت
	{
		bool وموندل = false;
		إذا (signal_outputs.شمېر(بټ))
			پایله.ورننباسه(بټ), وموندل = true;
		أرجع وموندل;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	کينډۍ<typename T>
	مضمن bool get_drivers(pool<پورټ_بټ_2> &پایله, ثابت T &بټونه) ثابت
	{
		bool وموندل = false;
		لكل (RTLIL::Sig_بټ بټ : بټونه)
			إذا (signal_drivers.شمېر(بټ)) {
				ثابت pool<پورټ_بټ_2> &r = signal_drivers.at(بټ);
				پایله.ورننباسه(r.begin(), r.پای_ټکی());
				وموندل = true;
			}
		أرجع وموندل;
	}

	کينډۍ<typename T>
	مضمن bool get_consumers(pool<پورټ_بټ_2> &پایله, ثابت T &بټونه) ثابت
	{
		bool وموندل = false;
		لكل (RTLIL::Sig_بټ بټ : بټونه)
			إذا (signal_consumers.شمېر(بټ)) {
				ثابت pool<پورټ_بټ_2> &r = signal_consumers.at(بټ);
				پایله.ورننباسه(r.begin(), r.پای_ټکی());
				وموندل = true;
			}
		أرجع وموندل;
	}

	کينډۍ<typename T>
	مضمن bool get_inputs(pool<RTLIL::Sig_بټ> &پایله, ثابت T &بټونه) ثابت
	{
		bool وموندل = false;
		لكل (RTLIL::Sig_بټ بټ : بټونه)
			إذا (signal_inputs.شمېر(بټ))
				پایله.ورننباسه(بټ), وموندل = true;
		أرجع وموندل;
	}

	کينډۍ<typename T>
	مضمن bool get_outputs(pool<RTLIL::Sig_بټ> &پایله, ثابت T &بټونه) ثابت
	{
		bool وموندل = false;
		لكل (RTLIL::Sig_بټ بټ : بټونه)
			إذا (signal_outputs.شمېر(بټ))
				پایله.ورننباسه(بټ), وموندل = true;
		أرجع وموندل;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<پورټ_بټ_2> &پایله, RTLIL::SigSpec إشارة) ثابت
	{
		std::ویکټور<RTLIL::Sig_بټ> بټونه = sigmap(إشارة);
		أرجع get_drivers(پایله, بټونه);
	}

	bool get_consumers(pool<پورټ_بټ_2> &پایله, RTLIL::SigSpec إشارة) ثابت
	{
		std::ویکټور<RTLIL::Sig_بټ> بټونه = sigmap(إشارة);
		أرجع get_consumers(پایله, بټونه);
	}

	bool get_inputs(pool<RTLIL::Sig_بټ> &پایله, RTLIL::SigSpec إشارة) ثابت
	{
		std::ویکټور<RTLIL::Sig_بټ> بټونه = sigmap(إشارة);
		أرجع get_inputs(پایله, بټونه);
	}

	bool get_outputs(pool<RTLIL::Sig_بټ> &پایله, RTLIL::SigSpec إشارة) ثابت
	{
		std::ویکټور<RTLIL::Sig_بټ> بټونه = sigmap(إشارة);
		أرجع get_outputs(پایله, بټونه);
	}

	// has_* methods -- call by reference

	کينډۍ<typename T>
	مضمن bool has_drivers(ثابت T &sig) ثابت {
		pool<پورټ_بټ_2> پایله;
		أرجع get_drivers(پایله, sig);
	}

	کينډۍ<typename T>
	مضمن bool has_consumers(ثابت T &sig) ثابت {
		pool<پورټ_بټ_2> پایله;
		أرجع get_consumers(پایله, sig);
	}

	کينډۍ<typename T>
	مضمن bool has_inputs(ثابت T &sig) ثابت {
		pool<RTLIL::Sig_بټ> پایله;
		أرجع get_inputs(پایله, sig);
	}

	کينډۍ<typename T>
	مضمن bool has_outputs(ثابت T &sig) ثابت {
		pool<RTLIL::Sig_بټ> پایله;
		أرجع get_outputs(پایله, sig);
	}

	// has_* methods -- call by value

	مضمن bool has_drivers(RTLIL::SigSpec sig) ثابت {
		pool<پورټ_بټ_2> پایله;
		أرجع get_drivers(پایله, sig);
	}

	مضمن bool has_consumers(RTLIL::SigSpec sig) ثابت {
		pool<پورټ_بټ_2> پایله;
		أرجع get_consumers(پایله, sig);
	}

	مضمن bool has_inputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::Sig_بټ> پایله;
		أرجع get_inputs(پایله, sig);
	}

	مضمن bool has_outputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::Sig_بټ> پایله;
		أرجع get_outputs(پایله, sig);
	}
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
