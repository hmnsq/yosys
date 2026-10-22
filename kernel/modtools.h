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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MODTOOLS_H
#ವ್ಯಾಖ್ಯಾನಿಸು MODTOOLS_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/celltypes.h"
#ಅನ್ತರ್ಭಾವ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ರಚನೆ Mod_ಸೂಚ್ಯಂಕ : ಸಾರ್ವಜನಿಕ RTLIL::Monitor
{
	ರಚನೆ ಸೂಚಕ_Ordered_Sig_ಬಿಟ್ : ಸಾರ್ವಜನಿಕ RTLIL::Sig_ಬಿಟ್ {
		ಸೂಚಕ_Ordered_Sig_ಬಿಟ್(Sig_ಬಿಟ್ s) {
			wire = s.wire;
			ಆದರೆ (wire)
				ಸ್ಥಾನಾಂತರ = s.ಸ್ಥಾನಾಂತರ;
			ಇಲ್ಲದಿದ್ದರೆ
				ದತ್ತಾಂಶ = s.ದತ್ತಾಂಶ;
		}
		ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &other) ಸ್ಥಿರಾಂಕ {
			ಆದರೆ (wire == other.wire)
				ಹಿಂತಿರುಗಿ wire ? (ಸ್ಥಾನಾಂತರ < other.ಸ್ಥಾನಾಂತರ) : (ದತ್ತಾಂಶ < other.ದತ್ತಾಂಶ);
			ಆದರೆ (wire != nullptr && other.wire != nullptr)
				ಹಿಂತಿರುಗಿ wire < other.wire; // look here
			ಹಿಂತಿರುಗಿ (wire != nullptr) < (other.wire != nullptr);
		}
	};
	ರಚನೆ ಪೋರ್ಟ್_Info {
		RTLIL::Cell* cell;
		RTLIL::IdString ಪೋರ್ಟ್;
		ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;

		ಪೋರ್ಟ್_Info() : cell(), ಪೋರ್ಟ್(), ಸ್ಥಾನಾಂತರ() { }
		ಪೋರ್ಟ್_Info(RTLIL::Cell* _c, RTLIL::IdString _p, ಪೂರ್ಣಾಂಕ _o) : cell(_c), ಪೋರ್ಟ್(_p), ಸ್ಥಾನಾಂತರ(_o) { }

		bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ ಪೋರ್ಟ್_Info &other) ಸ್ಥಿರಾಂಕ {
			ಆದರೆ (cell != other.cell)
				ಹಿಂತಿರುಗಿ cell < other.cell;
			ಆದರೆ (ಸ್ಥಾನಾಂತರ != other.ಸ್ಥಾನಾಂತರ)
				ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ < other.ಸ್ಥಾನಾಂತರ;
			ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್ < other.ಪೋರ್ಟ್;
		}

		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಪೋರ್ಟ್_Info &other) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ cell == other.cell && ಪೋರ್ಟ್ == other.ಪೋರ್ಟ್ && ಸ್ಥಾನಾಂತರ == other.ಸ್ಥಾನಾಂತರ;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
			h.eat(cell->name);
			h.eat(ಪೋರ್ಟ್);
			h.eat(ಸ್ಥಾನಾಂತರ);
			ಹಿಂತಿರುಗಿ h;
		}
	};

	ರಚನೆ Sig_ಬಿಟ್_Info
	{
		bool is_ಒಳಹರಿವು, is_output;
		pool<ಪೋರ್ಟ್_Info> ports;

		Sig_ಬಿಟ್_Info() : is_ಒಳಹರಿವು(false), is_output(false) { }

		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Sig_ಬಿಟ್_Info &other) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ is_ಒಳಹರಿವು == other.is_ಒಳಹರಿವು && is_output == other.is_output && ports == other.ports;
		}

		ಶೂನ್ಯ ವಿಲೀನ(ಸ್ಥಿರಾಂಕ Sig_ಬಿಟ್_Info &other)
		{
			is_ಒಳಹರಿವು = is_ಒಳಹರಿವು || other.is_ಒಳಹರಿವು;
			is_output = is_output || other.is_output;
			ports.ಸೇರಿಸು(other.ports.begin(), other.ports.ಕೊನೆ());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<ಸೂಚಕ_Ordered_Sig_ಬಿಟ್, Sig_ಬಿಟ್_Info> database;
	ಪೂರ್ಣಾಂಕ auto_reload_counter;
	bool auto_reload_module;

	ಶೂನ್ಯ ಪೋರ್ಟ್_ಕೂಡಿಸು(RTLIL::Cell *cell, RTLIL::IdString ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig)
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(sig); i++) {
			RTLIL::Sig_ಬಿಟ್ ಬಿಟ್ = sigmap(sig[i]);
			ಆದರೆ (ಬಿಟ್.wire)
				database[ಬಿಟ್].ports.ಸೇರಿಸು(ಪೋರ್ಟ್_Info(cell, ಪೋರ್ಟ್, i));
		}
	}

	ಶೂನ್ಯ ಪೋರ್ಟ್_del(RTLIL::Cell *cell, RTLIL::IdString ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig)
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(sig); i++) {
			RTLIL::Sig_ಬಿಟ್ ಬಿಟ್ = sigmap(sig[i]);
			ಆದರೆ (ಬಿಟ್.wire) {
				database[ಬಿಟ್].ports.erase(ಪೋರ್ಟ್_Info(cell, ಪೋರ್ಟ್, i));
				ಆದರೆ (!database[ಬಿಟ್].is_ಒಳಹರಿವು && !database[ಬಿಟ್].is_output && database[ಬಿಟ್].ports.ಖಾಲಿಗಳು())
					database.erase(ಬಿಟ್);
			}
		}
	}

	ಸ್ಥಿರಾಂಕ Sig_ಬಿಟ್_Info &info(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್)
	{
		ಹಿಂತಿರುಗಿ database[sigmap(ಬಿಟ್)];
	}

	ಶೂನ್ಯ reload_module(bool reset_sigmap = true)
	{
		ಆದರೆ (reset_sigmap) {
			sigmap.clear();
			sigmap.ಗಣ(module);
		}

		database.clear();
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : module->wires())
			ಆದರೆ (wire->ಪೋರ್ಟ್_ಒಳಹರಿವು || wire->ಪೋರ್ಟ್_output)
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wire); i++) {
					RTLIL::Sig_ಬಿಟ್ ಬಿಟ್ = sigmap(RTLIL::Sig_ಬಿಟ್(wire, i));
					ಆದರೆ (ಬಿಟ್.wire && wire->ಪೋರ್ಟ್_ಒಳಹರಿವು)
						database[ಬಿಟ್].is_ಒಳಹರಿವು = true;
					ಆದರೆ (ಬಿಟ್.wire && wire->ಪೋರ್ಟ್_output)
						database[ಬಿಟ್].is_output = true;
				}
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : module->cells())
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &conn : cell->connections())
				ಪೋರ್ಟ್_ಕೂಡಿಸು(cell, conn.ಮೊದಲ, conn.ಎರಡನೇ);

		ಆದರೆ (auto_reload_module) {
			ಆದರೆ (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NDEBUG
		ಆದರೆ (auto_reload_module)
			ಹಿಂತಿರುಗಿ true;

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it : database)
			log_assert(it.ಮೊದಲ == sigmap(it.ಮೊದಲ));

		ಸ್ವಯಂಚಾಲಿತ database_bak = std::move(database);
		reload_module(false);

		ಆದರೆ (!(database == database_bak))
		{
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : database_bak)
				ಆದರೆ (!database.ಎಣಿಕೆ_2(it.ಮೊದಲ))
					ಲಘುಗಣಕ("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.ಮೊದಲ));

			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : database)
				ಆದರೆ (!database_bak.ಎಣಿಕೆ_2(it.ಮೊದಲ))
					ಲಘುಗಣಕ("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.ಮೊದಲ));
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(it.ಎರಡನೇ == database_bak.at(it.ಮೊದಲ)))
					ಲಘುಗಣಕ("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.ಮೊದಲ));

			ಹಿಂತಿರುಗಿ false;
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಹಿಂತಿರುಗಿ true;
	}

	ಶೂನ್ಯ check()
	{
		log_assert(ok());
	}

	ಶೂನ್ಯ notify_connect(RTLIL::Cell *cell, RTLIL::IdString ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &old_sig, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		ಆದರೆ (auto_reload_module)
			ಹಿಂತಿರುಗಿ;

		ಪೋರ್ಟ್_del(cell, ಪೋರ್ಟ್, old_sig);
		ಪೋರ್ಟ್_ಕೂಡಿಸು(cell, ಪೋರ್ಟ್, sig);
	}

	ಶೂನ್ಯ notify_connect(RTLIL::Module *mod, ಸ್ಥಿರಾಂಕ RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		ಆದರೆ (auto_reload_module)
			ಹಿಂತಿರುಗಿ;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(sigsig.ಮೊದಲ); i++)
		{
			RTLIL::Sig_ಬಿಟ್ lhs = sigmap(sigsig.ಮೊದಲ[i]);
			RTLIL::Sig_ಬಿಟ್ rhs = sigmap(sigsig.ಎರಡನೇ[i]);
			bool has_lhs = database.ಎಣಿಕೆ_2(lhs) != 0;
			bool has_rhs = database.ಎಣಿಕೆ_2(rhs) != 0;

			ಆದರೆ (!has_lhs && !has_rhs) {
				sigmap.ಕೂಡಿಸು(lhs, rhs);
			} ಇಲ್ಲದಿದ್ದರೆ
			ಆದರೆ (!has_rhs) {
				Sig_ಬಿಟ್_Info new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.ಕೂಡಿಸು(lhs, rhs);
				lhs = sigmap(lhs);
				ಆದರೆ (lhs.wire)
					database[lhs] = new_info;
			} ಇಲ್ಲದಿದ್ದರೆ
			ಆದರೆ (!has_lhs) {
				Sig_ಬಿಟ್_Info new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.ಕೂಡಿಸು(lhs, rhs);
				rhs = sigmap(rhs);
				ಆದರೆ (rhs.wire)
					database[rhs] = new_info;
			} ಇಲ್ಲದಿದ್ದರೆ {
				Sig_ಬಿಟ್_Info new_info = database.at(lhs);
				new_info.ವಿಲೀನ(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.ಕೂಡಿಸು(lhs, rhs);
				rhs = sigmap(rhs);
				ಆದರೆ (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	ಶೂನ್ಯ notify_connect(RTLIL::Module *mod, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ಶೂನ್ಯ notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	Mod_ಸೂಚ್ಯಂಕ(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.ಸೇರಿಸು(this);
	}

	~Mod_ಸೂಚ್ಯಂಕ()
	{
		module->monitors.erase(this);
	}

	Sig_ಬಿಟ್_Info *query(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್)
	{
		ಆದರೆ (auto_reload_module)
			reload_module();

		ಸ್ವಯಂಚಾಲಿತ it = database.ಕಂಡುಹಿಡಿ(sigmap(ಬಿಟ್));
		ಆದರೆ (it == database.ಕೊನೆ())
			ಹಿಂತಿರುಗಿ nullptr;
		ಇಲ್ಲದಿದ್ದರೆ
			ಹಿಂತಿರುಗಿ &it->ಎರಡನೇ;
	}

	bool query_is_ಒಳಹರಿವು(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್)
	{
		ಸ್ಥಿರಾಂಕ Sig_ಬಿಟ್_Info *info = query(ಬಿಟ್);
		ಆದರೆ (info == nullptr)
			ಹಿಂತಿರುಗಿ false;
		ಹಿಂತಿರುಗಿ info->is_ಒಳಹರಿವು;
	}

	bool query_is_output(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್)
	{
		ಸ್ಥಿರಾಂಕ Sig_ಬಿಟ್_Info *info = query(ಬಿಟ್);
		ಆದರೆ (info == nullptr)
			ಹಿಂತಿರುಗಿ false;
		ಹಿಂತಿರುಗಿ info->is_output;
	}

	pool<ಪೋರ್ಟ್_Info> &query_ports(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್)
	{
		ಸ್ಥಿರ pool<ಪೋರ್ಟ್_Info> ಖಾಲಿಗಳು_ಫಲಿತಾಂಶ_ಗಣ;
		Sig_ಬಿಟ್_Info *info = query(ಬಿಟ್);
		ಆದರೆ (info == nullptr)
			ಹಿಂತಿರುಗಿ ಖಾಲಿಗಳು_ಫಲಿತಾಂಶ_ಗಣ;
		ಹಿಂತಿರುಗಿ info->ports;
	}

	ಶೂನ್ಯ dump_db()
	{
		ಲಘುಗಣಕ("--- ModIndex Dump ---\n");

		ಆದರೆ (auto_reload_module) {
			ಲಘುಗಣಕ("AUTO-RELOAD\n");
			reload_module();
		}

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : database) {
			ಲಘುಗಣಕ("BIT %s:\n", log_signal(it.ಮೊದಲ));
			ಆದರೆ (it.ಎರಡನೇ.is_ಒಳಹರಿವು)
				ಲಘುಗಣಕ("  PRIMARY INPUT\n");
			ಆದರೆ (it.ಎರಡನೇ.is_output)
				ಲಘುಗಣಕ("  PRIMARY OUTPUT\n");
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : it.ಎರಡನೇ.ports)
				ಲಘುಗಣಕ("  PORT: %s.%s[%d] (%s)\n", ಪೋರ್ಟ್.cell,
						PooledName(module, ಪೋರ್ಟ್.ಪೋರ್ಟ್).unescape(), ಪೋರ್ಟ್.ಸ್ಥಾನಾಂತರ, ಪೋರ್ಟ್.cell->type.unescape());
		}
	}
};

ರಚನೆ ModWalker
{
	ರಚನೆ ಪೋರ್ಟ್_ಬಿಟ್_2
	{
		RTLIL::Cell *cell;
		RTLIL::IdString ಪೋರ್ಟ್;
		ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;
		ಪೋರ್ಟ್_ಬಿಟ್_2(Cell* c, IdString p, ಪೂರ್ಣಾಂಕ o) : cell(c), ಪೋರ್ಟ್(p), ಸ್ಥಾನಾಂತರ(o) {}

		bool ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ ಪೋರ್ಟ್_ಬಿಟ್_2 &other) ಸ್ಥಿರಾಂಕ {
			ಆದರೆ (cell != other.cell)
				ಹಿಂತಿರುಗಿ cell < other.cell;
			ಆದರೆ (ಪೋರ್ಟ್ != other.ಪೋರ್ಟ್)
				ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್ < other.ಪೋರ್ಟ್;
			ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ < other.ಸ್ಥಾನಾಂತರ;
		}

		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಪೋರ್ಟ್_ಬಿಟ್_2 &other) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ cell == other.cell && ಪೋರ್ಟ್ == other.ಪೋರ್ಟ್ && ಸ್ಥಾನಾಂತರ == other.ಸ್ಥಾನಾಂತರ;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
			h.eat(cell->name);
			h.eat(ಪೋರ್ಟ್);
			h.eat(ಸ್ಥಾನಾಂತರ);
			ಹಿಂತಿರುಗಿ h;
		}
	};

	RTLIL::Design *design;
	RTLIL::Module *module;

	NewCellTypes ct;
	SigMap sigmap;

	dict<RTLIL::Sig_ಬಿಟ್, pool<ಪೋರ್ಟ್_ಬಿಟ್_2>> signal_drivers;
	dict<RTLIL::Sig_ಬಿಟ್, pool<ಪೋರ್ಟ್_ಬಿಟ್_2>> signal_consumers;
	pool<RTLIL::Sig_ಬಿಟ್> signal_inputs, signal_outputs;

	dict<RTLIL::Cell*, pool<RTLIL::Sig_ಬಿಟ್>> cell_outputs, cell_inputs;

	ಶೂನ್ಯ ಕೂಡಿಸು_wire(RTLIL::Wire *wire)
	{
		ಆದರೆ (wire->ಪೋರ್ಟ್_ಒಳಹರಿವು) {
			std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು = sigmap(wire);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : ಬಿಟ್ಟುಗಳು)
				ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ)
					signal_inputs.ಸೇರಿಸು(ಬಿಟ್);
		}

		ಆದರೆ (wire->ಪೋರ್ಟ್_output) {
			std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು = sigmap(wire);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : ಬಿಟ್ಟುಗಳು)
				ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ)
					signal_outputs.ಸೇರಿಸು(ಬಿಟ್);
		}
	}

	ಶೂನ್ಯ ಕೂಡಿಸು_cell_ಪೋರ್ಟ್(RTLIL::Cell *cell, RTLIL::IdString ಪೋರ್ಟ್, std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು, bool is_output, bool is_ಒಳಹರಿವು)
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಪೂರ್ಣಾಂಕ(ಬಿಟ್ಟುಗಳು.ಗಾತ್ರ_2()); i++)
			ಆದರೆ (ಬಿಟ್ಟುಗಳು[i].wire != ಖಾಲಿ) {
				ಪೋರ್ಟ್_ಬಿಟ್_2 pbit {cell, ಪೋರ್ಟ್, i};
				ಆದರೆ (is_output) {
					signal_drivers[ಬಿಟ್ಟುಗಳು[i]].ಸೇರಿಸು(pbit);
					cell_outputs[cell].ಸೇರಿಸು(ಬಿಟ್ಟುಗಳು[i]);
				}
				ಆದರೆ (is_ಒಳಹರಿವು) {
					signal_consumers[ಬಿಟ್ಟುಗಳು[i]].ಸೇರಿಸು(pbit);
					cell_inputs[cell].ಸೇರಿಸು(ಬಿಟ್ಟುಗಳು[i]);
				}
			}
	}

	ಶೂನ್ಯ ಕೂಡಿಸು_cell(RTLIL::Cell *cell)
	{
		ಆದರೆ (ct.cell_ತಿಳಿದ(cell->type)) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &conn : cell->connections())
				ಕೂಡಿಸು_cell_ಪೋರ್ಟ್(cell, conn.ಮೊದಲ, sigmap(conn.ಎರಡನೇ),
						ct.cell_output(cell->type, conn.ಮೊದಲ),
						ct.cell_ಒಳಹರಿವು(cell->type, conn.ಮೊದಲ));
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &conn : cell->connections())
				ಕೂಡಿಸು_cell_ಪೋರ್ಟ್(cell, conn.ಮೊದಲ, sigmap(conn.ಎರಡನೇ), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(ಖಾಲಿ)
	{
		ct.ಸಿದ್ಧತೆ(design);
		ಆದರೆ (module)
			ಸಿದ್ಧತೆ(module);
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ(RTLIL::Module *module, CellTypes *filter_ct = ಖಾಲಿ)
	{
		this->module = module;

		sigmap.ಗಣ(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : module->wires_)
			ಕೂಡಿಸು_wire(it.ಎರಡನೇ);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : module->cells_)
			ಆದರೆ (filter_ct == ಖಾಲಿ || filter_ct->cell_ತಿಳಿದ(it.ಎರಡನೇ->type))
				ಕೂಡಿಸು_cell(it.ಎರಡನೇ);
	}

	// get_* methods -- single RTLIL::SigBit

	ಸಾಲೊಳಗೆ bool get_drivers(pool<ಪೋರ್ಟ್_ಬಿಟ್_2> &ಫಲಿತಾಂಶ, RTLIL::Sig_ಬಿಟ್ ಬಿಟ್) ಸ್ಥಿರಾಂಕ
	{
		bool ಸಿಕ್ಕಿತು = false;
		ಆದರೆ (signal_drivers.ಎಣಿಕೆ_2(ಬಿಟ್)) {
			ಸ್ಥಿರಾಂಕ pool<ಪೋರ್ಟ್_ಬಿಟ್_2> &r = signal_drivers.at(ಬಿಟ್);
			ಫಲಿತಾಂಶ.ಸೇರಿಸು(r.begin(), r.ಕೊನೆ());
			ಸಿಕ್ಕಿತು = true;
		}
		ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
	}

	ಸಾಲೊಳಗೆ bool get_consumers(pool<ಪೋರ್ಟ್_ಬಿಟ್_2> &ಫಲಿತಾಂಶ, RTLIL::Sig_ಬಿಟ್ ಬಿಟ್) ಸ್ಥಿರಾಂಕ
	{
		bool ಸಿಕ್ಕಿತು = false;
		ಆದರೆ (signal_consumers.ಎಣಿಕೆ_2(ಬಿಟ್)) {
			ಸ್ಥಿರಾಂಕ pool<ಪೋರ್ಟ್_ಬಿಟ್_2> &r = signal_consumers.at(ಬಿಟ್);
			ಫಲಿತಾಂಶ.ಸೇರಿಸು(r.begin(), r.ಕೊನೆ());
			ಸಿಕ್ಕಿತು = true;
		}
		ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
	}

	ಸಾಲೊಳಗೆ bool get_inputs(pool<RTLIL::Sig_ಬಿಟ್> &ಫಲಿತಾಂಶ, RTLIL::Sig_ಬಿಟ್ ಬಿಟ್) ಸ್ಥಿರಾಂಕ
	{
		bool ಸಿಕ್ಕಿತು = false;
		ಆದರೆ (signal_inputs.ಎಣಿಕೆ_2(ಬಿಟ್))
			ಫಲಿತಾಂಶ.ಸೇರಿಸು(ಬಿಟ್), ಸಿಕ್ಕಿತು = true;
		ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
	}

	ಸಾಲೊಳಗೆ bool get_outputs(pool<RTLIL::Sig_ಬಿಟ್> &ಫಲಿತಾಂಶ, RTLIL::Sig_ಬಿಟ್ ಬಿಟ್) ಸ್ಥಿರಾಂಕ
	{
		bool ಸಿಕ್ಕಿತು = false;
		ಆದರೆ (signal_outputs.ಎಣಿಕೆ_2(ಬಿಟ್))
			ಫಲಿತಾಂಶ.ಸೇರಿಸು(ಬಿಟ್), ಸಿಕ್ಕಿತು = true;
		ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	ಮಾದರಿ<typename T>
	ಸಾಲೊಳಗೆ bool get_drivers(pool<ಪೋರ್ಟ್_ಬಿಟ್_2> &ಫಲಿತಾಂಶ, ಸ್ಥಿರಾಂಕ T &ಬಿಟ್ಟುಗಳು) ಸ್ಥಿರಾಂಕ
	{
		bool ಸಿಕ್ಕಿತು = false;
		ಗಾಗಿ (RTLIL::Sig_ಬಿಟ್ ಬಿಟ್ : ಬಿಟ್ಟುಗಳು)
			ಆದರೆ (signal_drivers.ಎಣಿಕೆ_2(ಬಿಟ್)) {
				ಸ್ಥಿರಾಂಕ pool<ಪೋರ್ಟ್_ಬಿಟ್_2> &r = signal_drivers.at(ಬಿಟ್);
				ಫಲಿತಾಂಶ.ಸೇರಿಸು(r.begin(), r.ಕೊನೆ());
				ಸಿಕ್ಕಿತು = true;
			}
		ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
	}

	ಮಾದರಿ<typename T>
	ಸಾಲೊಳಗೆ bool get_consumers(pool<ಪೋರ್ಟ್_ಬಿಟ್_2> &ಫಲಿತಾಂಶ, ಸ್ಥಿರಾಂಕ T &ಬಿಟ್ಟುಗಳು) ಸ್ಥಿರಾಂಕ
	{
		bool ಸಿಕ್ಕಿತು = false;
		ಗಾಗಿ (RTLIL::Sig_ಬಿಟ್ ಬಿಟ್ : ಬಿಟ್ಟುಗಳು)
			ಆದರೆ (signal_consumers.ಎಣಿಕೆ_2(ಬಿಟ್)) {
				ಸ್ಥಿರಾಂಕ pool<ಪೋರ್ಟ್_ಬಿಟ್_2> &r = signal_consumers.at(ಬಿಟ್);
				ಫಲಿತಾಂಶ.ಸೇರಿಸು(r.begin(), r.ಕೊನೆ());
				ಸಿಕ್ಕಿತು = true;
			}
		ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
	}

	ಮಾದರಿ<typename T>
	ಸಾಲೊಳಗೆ bool get_inputs(pool<RTLIL::Sig_ಬಿಟ್> &ಫಲಿತಾಂಶ, ಸ್ಥಿರಾಂಕ T &ಬಿಟ್ಟುಗಳು) ಸ್ಥಿರಾಂಕ
	{
		bool ಸಿಕ್ಕಿತು = false;
		ಗಾಗಿ (RTLIL::Sig_ಬಿಟ್ ಬಿಟ್ : ಬಿಟ್ಟುಗಳು)
			ಆದರೆ (signal_inputs.ಎಣಿಕೆ_2(ಬಿಟ್))
				ಫಲಿತಾಂಶ.ಸೇರಿಸು(ಬಿಟ್), ಸಿಕ್ಕಿತು = true;
		ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
	}

	ಮಾದರಿ<typename T>
	ಸಾಲೊಳಗೆ bool get_outputs(pool<RTLIL::Sig_ಬಿಟ್> &ಫಲಿತಾಂಶ, ಸ್ಥಿರಾಂಕ T &ಬಿಟ್ಟುಗಳು) ಸ್ಥಿರಾಂಕ
	{
		bool ಸಿಕ್ಕಿತು = false;
		ಗಾಗಿ (RTLIL::Sig_ಬಿಟ್ ಬಿಟ್ : ಬಿಟ್ಟುಗಳು)
			ಆದರೆ (signal_outputs.ಎಣಿಕೆ_2(ಬಿಟ್))
				ಫಲಿತಾಂಶ.ಸೇರಿಸು(ಬಿಟ್), ಸಿಕ್ಕಿತು = true;
		ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<ಪೋರ್ಟ್_ಬಿಟ್_2> &ಫಲಿತಾಂಶ, RTLIL::SigSpec ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ
	{
		std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು = sigmap(ಸಂಕೇತ);
		ಹಿಂತಿರುಗಿ get_drivers(ಫಲಿತಾಂಶ, ಬಿಟ್ಟುಗಳು);
	}

	bool get_consumers(pool<ಪೋರ್ಟ್_ಬಿಟ್_2> &ಫಲಿತಾಂಶ, RTLIL::SigSpec ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ
	{
		std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು = sigmap(ಸಂಕೇತ);
		ಹಿಂತಿರುಗಿ get_consumers(ಫಲಿತಾಂಶ, ಬಿಟ್ಟುಗಳು);
	}

	bool get_inputs(pool<RTLIL::Sig_ಬಿಟ್> &ಫಲಿತಾಂಶ, RTLIL::SigSpec ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ
	{
		std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು = sigmap(ಸಂಕೇತ);
		ಹಿಂತಿರುಗಿ get_inputs(ಫಲಿತಾಂಶ, ಬಿಟ್ಟುಗಳು);
	}

	bool get_outputs(pool<RTLIL::Sig_ಬಿಟ್> &ಫಲಿತಾಂಶ, RTLIL::SigSpec ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ
	{
		std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು = sigmap(ಸಂಕೇತ);
		ಹಿಂತಿರುಗಿ get_outputs(ಫಲಿತಾಂಶ, ಬಿಟ್ಟುಗಳು);
	}

	// has_* methods -- call by reference

	ಮಾದರಿ<typename T>
	ಸಾಲೊಳಗೆ bool has_drivers(ಸ್ಥಿರಾಂಕ T &sig) ಸ್ಥಿರಾಂಕ {
		pool<ಪೋರ್ಟ್_ಬಿಟ್_2> ಫಲಿತಾಂಶ;
		ಹಿಂತಿರುಗಿ get_drivers(ಫಲಿತಾಂಶ, sig);
	}

	ಮಾದರಿ<typename T>
	ಸಾಲೊಳಗೆ bool has_consumers(ಸ್ಥಿರಾಂಕ T &sig) ಸ್ಥಿರಾಂಕ {
		pool<ಪೋರ್ಟ್_ಬಿಟ್_2> ಫಲಿತಾಂಶ;
		ಹಿಂತಿರುಗಿ get_consumers(ಫಲಿತಾಂಶ, sig);
	}

	ಮಾದರಿ<typename T>
	ಸಾಲೊಳಗೆ bool has_inputs(ಸ್ಥಿರಾಂಕ T &sig) ಸ್ಥಿರಾಂಕ {
		pool<RTLIL::Sig_ಬಿಟ್> ಫಲಿತಾಂಶ;
		ಹಿಂತಿರುಗಿ get_inputs(ಫಲಿತಾಂಶ, sig);
	}

	ಮಾದರಿ<typename T>
	ಸಾಲೊಳಗೆ bool has_outputs(ಸ್ಥಿರಾಂಕ T &sig) ಸ್ಥಿರಾಂಕ {
		pool<RTLIL::Sig_ಬಿಟ್> ಫಲಿತಾಂಶ;
		ಹಿಂತಿರುಗಿ get_outputs(ಫಲಿತಾಂಶ, sig);
	}

	// has_* methods -- call by value

	ಸಾಲೊಳಗೆ bool has_drivers(RTLIL::SigSpec sig) ಸ್ಥಿರಾಂಕ {
		pool<ಪೋರ್ಟ್_ಬಿಟ್_2> ಫಲಿತಾಂಶ;
		ಹಿಂತಿರುಗಿ get_drivers(ಫಲಿತಾಂಶ, sig);
	}

	ಸಾಲೊಳಗೆ bool has_consumers(RTLIL::SigSpec sig) ಸ್ಥಿರಾಂಕ {
		pool<ಪೋರ್ಟ್_ಬಿಟ್_2> ಫಲಿತಾಂಶ;
		ಹಿಂತಿರುಗಿ get_consumers(ಫಲಿತಾಂಶ, sig);
	}

	ಸಾಲೊಳಗೆ bool has_inputs(RTLIL::SigSpec sig) ಸ್ಥಿರಾಂಕ {
		pool<RTLIL::Sig_ಬಿಟ್> ಫಲಿತಾಂಶ;
		ಹಿಂತಿರುಗಿ get_inputs(ಫಲಿತಾಂಶ, sig);
	}

	ಸಾಲೊಳಗೆ bool has_outputs(RTLIL::SigSpec sig) ಸ್ಥಿರಾಂಕ {
		pool<RTLIL::Sig_ಬಿಟ್> ಫಲಿತಾಂಶ;
		ಹಿಂತಿರುಗಿ get_outputs(ಫಲಿತಾಂಶ, sig);
	}
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
