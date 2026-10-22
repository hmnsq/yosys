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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MODTOOLS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MODTOOLS_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/sigtools.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/celltypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ਬਣਤਰ Mod_ਸੂਚਕਾਂਕ : ਜਨਤਕ RTLIL::Monitor
{
	ਬਣਤਰ ਸੂਚਕ_Ordered_Sig_ਦੋਆਧਾਰੀ_ਅੰਕ : ਜਨਤਕ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ {
		ਸੂਚਕ_Ordered_Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(Sig_ਦੋਆਧਾਰੀ_ਅੰਕ s) {
			wire = s.wire;
			ਜੇ (wire)
				ਵਿੱਥ = s.ਵਿੱਥ;
			ਨਹੀਂਤਾਂ
				ਅੰਕੜੇ = s.ਅੰਕੜੇ;
		}
		ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ<(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &other) ਅਚਲ {
			ਜੇ (wire == other.wire)
				ਵਾਪਸ wire ? (ਵਿੱਥ < other.ਵਿੱਥ) : (ਅੰਕੜੇ < other.ਅੰਕੜੇ);
			ਜੇ (wire != nullptr && other.wire != nullptr)
				ਵਾਪਸ wire < other.wire; // look here
			ਵਾਪਸ (wire != nullptr) < (other.wire != nullptr);
		}
	};
	ਬਣਤਰ ਪੋਰਟ_Info {
		RTLIL::Cell* cell;
		RTLIL::IdString ਪੋਰਟ;
		ਪੂਰਨ ਵਿੱਥ;

		ਪੋਰਟ_Info() : cell(), ਪੋਰਟ(), ਵਿੱਥ() { }
		ਪੋਰਟ_Info(RTLIL::Cell* _c, RTLIL::IdString _p, ਪੂਰਨ _o) : cell(_c), ਪੋਰਟ(_p), ਵਿੱਥ(_o) { }

		bool ਸੰਚਾਲਕ<(ਅਚਲ ਪੋਰਟ_Info &other) ਅਚਲ {
			ਜੇ (cell != other.cell)
				ਵਾਪਸ cell < other.cell;
			ਜੇ (ਵਿੱਥ != other.ਵਿੱਥ)
				ਵਾਪਸ ਵਿੱਥ < other.ਵਿੱਥ;
			ਵਾਪਸ ਪੋਰਟ < other.ਪੋਰਟ;
		}

		bool ਸੰਚਾਲਕ==(ਅਚਲ ਪੋਰਟ_Info &other) ਅਚਲ {
			ਵਾਪਸ cell == other.cell && ਪੋਰਟ == other.ਪੋਰਟ && ਵਿੱਥ == other.ਵਿੱਥ;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
			h.eat(cell->name);
			h.eat(ਪੋਰਟ);
			h.eat(ਵਿੱਥ);
			ਵਾਪਸ h;
		}
	};

	ਬਣਤਰ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ_Info
	{
		bool is_ਨਿਵੇਸ਼, is_output;
		pool<ਪੋਰਟ_Info> ports;

		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ_Info() : is_ਨਿਵੇਸ਼(false), is_output(false) { }

		bool ਸੰਚਾਲਕ==(ਅਚਲ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ_Info &other) ਅਚਲ {
			ਵਾਪਸ is_ਨਿਵੇਸ਼ == other.is_ਨਿਵੇਸ਼ && is_output == other.is_output && ports == other.ports;
		}

		ਖਾਲੀ ਮਿਲਾਓ(ਅਚਲ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ_Info &other)
		{
			is_ਨਿਵੇਸ਼ = is_ਨਿਵੇਸ਼ || other.is_ਨਿਵੇਸ਼;
			is_output = is_output || other.is_output;
			ports.ਪਾਓ(other.ports.begin(), other.ports.ਸਿਰਾ());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<ਸੂਚਕ_Ordered_Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, Sig_ਦੋਆਧਾਰੀ_ਅੰਕ_Info> database;
	ਪੂਰਨ auto_reload_counter;
	bool auto_reload_module;

	ਖਾਲੀ ਪੋਰਟ_ਜੋੜੋ(RTLIL::Cell *cell, RTLIL::IdString ਪੋਰਟ, ਅਚਲ RTLIL::SigSpec &sig)
	{
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(sig); i++) {
			RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ = sigmap(sig[i]);
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire)
				database[ਦੋਆਧਾਰੀ_ਅੰਕ].ports.ਪਾਓ(ਪੋਰਟ_Info(cell, ਪੋਰਟ, i));
		}
	}

	ਖਾਲੀ ਪੋਰਟ_del(RTLIL::Cell *cell, RTLIL::IdString ਪੋਰਟ, ਅਚਲ RTLIL::SigSpec &sig)
	{
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(sig); i++) {
			RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ = sigmap(sig[i]);
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire) {
				database[ਦੋਆਧਾਰੀ_ਅੰਕ].ports.erase(ਪੋਰਟ_Info(cell, ਪੋਰਟ, i));
				ਜੇ (!database[ਦੋਆਧਾਰੀ_ਅੰਕ].is_ਨਿਵੇਸ਼ && !database[ਦੋਆਧਾਰੀ_ਅੰਕ].is_output && database[ਦੋਆਧਾਰੀ_ਅੰਕ].ports.ਖਾਲੀ_ਥਾਵਾਂ())
					database.erase(ਦੋਆਧਾਰੀ_ਅੰਕ);
			}
		}
	}

	ਅਚਲ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ_Info &info(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ)
	{
		ਵਾਪਸ database[sigmap(ਦੋਆਧਾਰੀ_ਅੰਕ)];
	}

	ਖਾਲੀ reload_module(bool reset_sigmap = true)
	{
		ਜੇ (reset_sigmap) {
			sigmap.clear();
			sigmap.ਸੈੱਟ(module);
		}

		database.clear();
		ਲਈ (ਸਵੈਚਾਲਿਤ wire : module->wires())
			ਜੇ (wire->ਪੋਰਟ_ਨਿਵੇਸ਼ || wire->ਪੋਰਟ_output)
				ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wire); i++) {
					RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ = sigmap(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(wire, i));
					ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire && wire->ਪੋਰਟ_ਨਿਵੇਸ਼)
						database[ਦੋਆਧਾਰੀ_ਅੰਕ].is_ਨਿਵੇਸ਼ = true;
					ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire && wire->ਪੋਰਟ_output)
						database[ਦੋਆਧਾਰੀ_ਅੰਕ].is_output = true;
				}
		ਲਈ (ਸਵੈਚਾਲਿਤ cell : module->cells())
			ਲਈ (ਸਵੈਚਾਲਿਤ &conn : cell->connections())
				ਪੋਰਟ_ਜੋੜੋ(cell, conn.ਪਹਿਲਾ, conn.ਦੂਜਾ);

		ਜੇ (auto_reload_module) {
			ਜੇ (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NDEBUG
		ਜੇ (auto_reload_module)
			ਵਾਪਸ true;

		ਲਈ (ਸਵੈਚਾਲਿਤ it : database)
			log_assert(it.ਪਹਿਲਾ == sigmap(it.ਪਹਿਲਾ));

		ਸਵੈਚਾਲਿਤ database_bak = std::move(database);
		reload_module(false);

		ਜੇ (!(database == database_bak))
		{
			ਲਈ (ਸਵੈਚਾਲਿਤ &it : database_bak)
				ਜੇ (!database.ਗਿਣਤੀ_2(it.ਪਹਿਲਾ))
					ਲਘੂਗਣਕ("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.ਪਹਿਲਾ));

			ਲਈ (ਸਵੈਚਾਲਿਤ &it : database)
				ਜੇ (!database_bak.ਗਿਣਤੀ_2(it.ਪਹਿਲਾ))
					ਲਘੂਗਣਕ("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.ਪਹਿਲਾ));
				ਨਹੀਂਤਾਂ ਜੇ (!(it.ਦੂਜਾ == database_bak.at(it.ਪਹਿਲਾ)))
					ਲਘੂਗਣਕ("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.ਪਹਿਲਾ));

			ਵਾਪਸ false;
		}
#ਜੇ_ਅੰਤ
		ਵਾਪਸ true;
	}

	ਖਾਲੀ check()
	{
		log_assert(ok());
	}

	ਖਾਲੀ notify_connect(RTLIL::Cell *cell, RTLIL::IdString ਪੋਰਟ, ਅਚਲ RTLIL::SigSpec &old_sig, ਅਚਲ RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		ਜੇ (auto_reload_module)
			ਵਾਪਸ;

		ਪੋਰਟ_del(cell, ਪੋਰਟ, old_sig);
		ਪੋਰਟ_ਜੋੜੋ(cell, ਪੋਰਟ, sig);
	}

	ਖਾਲੀ notify_connect(RTLIL::Module *mod, ਅਚਲ RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		ਜੇ (auto_reload_module)
			ਵਾਪਸ;

		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(sigsig.ਪਹਿਲਾ); i++)
		{
			RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ lhs = sigmap(sigsig.ਪਹਿਲਾ[i]);
			RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ rhs = sigmap(sigsig.ਦੂਜਾ[i]);
			bool has_lhs = database.ਗਿਣਤੀ_2(lhs) != 0;
			bool has_rhs = database.ਗਿਣਤੀ_2(rhs) != 0;

			ਜੇ (!has_lhs && !has_rhs) {
				sigmap.ਜੋੜੋ(lhs, rhs);
			} ਨਹੀਂਤਾਂ
			ਜੇ (!has_rhs) {
				Sig_ਦੋਆਧਾਰੀ_ਅੰਕ_Info new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.ਜੋੜੋ(lhs, rhs);
				lhs = sigmap(lhs);
				ਜੇ (lhs.wire)
					database[lhs] = new_info;
			} ਨਹੀਂਤਾਂ
			ਜੇ (!has_lhs) {
				Sig_ਦੋਆਧਾਰੀ_ਅੰਕ_Info new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.ਜੋੜੋ(lhs, rhs);
				rhs = sigmap(rhs);
				ਜੇ (rhs.wire)
					database[rhs] = new_info;
			} ਨਹੀਂਤਾਂ {
				Sig_ਦੋਆਧਾਰੀ_ਅੰਕ_Info new_info = database.at(lhs);
				new_info.ਮਿਲਾਓ(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.ਜੋੜੋ(lhs, rhs);
				rhs = sigmap(rhs);
				ਜੇ (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	ਖਾਲੀ notify_connect(RTLIL::Module *mod, ਅਚਲ std::ਸਦਿਸ਼<RTLIL::SigSig>&) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	ਖਾਲੀ notify_blackout(RTLIL::Module *mod) override
	{
		log_assert(module == mod);
		auto_reload_module = true;
	}

	Mod_ਸੂਚਕਾਂਕ(RTLIL::Module *_m) : sigmap(_m), module(_m)
	{
		auto_reload_counter = 0;
		auto_reload_module = true;
		module->monitors.ਪਾਓ(this);
	}

	~Mod_ਸੂਚਕਾਂਕ()
	{
		module->monitors.erase(this);
	}

	Sig_ਦੋਆਧਾਰੀ_ਅੰਕ_Info *query(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ)
	{
		ਜੇ (auto_reload_module)
			reload_module();

		ਸਵੈਚਾਲਿਤ it = database.ਲੱਭੋ(sigmap(ਦੋਆਧਾਰੀ_ਅੰਕ));
		ਜੇ (it == database.ਸਿਰਾ())
			ਵਾਪਸ nullptr;
		ਨਹੀਂਤਾਂ
			ਵਾਪਸ &it->ਦੂਜਾ;
	}

	bool query_is_ਨਿਵੇਸ਼(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ)
	{
		ਅਚਲ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ_Info *info = query(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਜੇ (info == nullptr)
			ਵਾਪਸ false;
		ਵਾਪਸ info->is_ਨਿਵੇਸ਼;
	}

	bool query_is_output(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ)
	{
		ਅਚਲ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ_Info *info = query(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਜੇ (info == nullptr)
			ਵਾਪਸ false;
		ਵਾਪਸ info->is_output;
	}

	pool<ਪੋਰਟ_Info> &query_ports(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ)
	{
		ਸਥਿਰ pool<ਪੋਰਟ_Info> ਖਾਲੀ_ਥਾਵਾਂ_ਨਤੀਜਾ_ਸੈੱਟ;
		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ_Info *info = query(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਜੇ (info == nullptr)
			ਵਾਪਸ ਖਾਲੀ_ਥਾਵਾਂ_ਨਤੀਜਾ_ਸੈੱਟ;
		ਵਾਪਸ info->ports;
	}

	ਖਾਲੀ dump_db()
	{
		ਲਘੂਗਣਕ("--- ModIndex Dump ---\n");

		ਜੇ (auto_reload_module) {
			ਲਘੂਗਣਕ("AUTO-RELOAD\n");
			reload_module();
		}

		ਲਈ (ਸਵੈਚਾਲਿਤ &it : database) {
			ਲਘੂਗਣਕ("BIT %s:\n", log_signal(it.ਪਹਿਲਾ));
			ਜੇ (it.ਦੂਜਾ.is_ਨਿਵੇਸ਼)
				ਲਘੂਗਣਕ("  PRIMARY INPUT\n");
			ਜੇ (it.ਦੂਜਾ.is_output)
				ਲਘੂਗਣਕ("  PRIMARY OUTPUT\n");
			ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : it.ਦੂਜਾ.ports)
				ਲਘੂਗਣਕ("  PORT: %s.%s[%d] (%s)\n", ਪੋਰਟ.cell,
						PooledName(module, ਪੋਰਟ.ਪੋਰਟ).unescape(), ਪੋਰਟ.ਵਿੱਥ, ਪੋਰਟ.cell->type.unescape());
		}
	}
};

ਬਣਤਰ ModWalker
{
	ਬਣਤਰ ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2
	{
		RTLIL::Cell *cell;
		RTLIL::IdString ਪੋਰਟ;
		ਪੂਰਨ ਵਿੱਥ;
		ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2(Cell* c, IdString p, ਪੂਰਨ o) : cell(c), ਪੋਰਟ(p), ਵਿੱਥ(o) {}

		bool ਸੰਚਾਲਕ<(ਅਚਲ ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2 &other) ਅਚਲ {
			ਜੇ (cell != other.cell)
				ਵਾਪਸ cell < other.cell;
			ਜੇ (ਪੋਰਟ != other.ਪੋਰਟ)
				ਵਾਪਸ ਪੋਰਟ < other.ਪੋਰਟ;
			ਵਾਪਸ ਵਿੱਥ < other.ਵਿੱਥ;
		}

		bool ਸੰਚਾਲਕ==(ਅਚਲ ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2 &other) ਅਚਲ {
			ਵਾਪਸ cell == other.cell && ਪੋਰਟ == other.ਪੋਰਟ && ਵਿੱਥ == other.ਵਿੱਥ;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
			h.eat(cell->name);
			h.eat(ਪੋਰਟ);
			h.eat(ਵਿੱਥ);
			ਵਾਪਸ h;
		}
	};

	RTLIL::Design *design;
	RTLIL::Module *module;

	NewCellTypes ct;
	SigMap sigmap;

	dict<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2>> signal_drivers;
	dict<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2>> signal_consumers;
	pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> signal_inputs, signal_outputs;

	dict<RTLIL::Cell*, pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ>> cell_outputs, cell_inputs;

	ਖਾਲੀ ਜੋੜੋ_wire(RTLIL::Wire *wire)
	{
		ਜੇ (wire->ਪੋਰਟ_ਨਿਵੇਸ਼) {
			std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ = sigmap(wire);
			ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ)
				ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ)
					signal_inputs.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ);
		}

		ਜੇ (wire->ਪੋਰਟ_output) {
			std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ = sigmap(wire);
			ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ)
				ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ)
					signal_outputs.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ);
		}
	}

	ਖਾਲੀ ਜੋੜੋ_cell_ਪੋਰਟ(RTLIL::Cell *cell, RTLIL::IdString ਪੋਰਟ, std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ, bool is_output, bool is_ਨਿਵੇਸ਼)
	{
		ਲਈ (ਪੂਰਨ i = 0; i < ਪੂਰਨ(ਬਿੱਟ.ਮਾਪ()); i++)
			ਜੇ (ਬਿੱਟ[i].wire != ਨਿਰਾ) {
				ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2 pbit {cell, ਪੋਰਟ, i};
				ਜੇ (is_output) {
					signal_drivers[ਬਿੱਟ[i]].ਪਾਓ(pbit);
					cell_outputs[cell].ਪਾਓ(ਬਿੱਟ[i]);
				}
				ਜੇ (is_ਨਿਵੇਸ਼) {
					signal_consumers[ਬਿੱਟ[i]].ਪਾਓ(pbit);
					cell_inputs[cell].ਪਾਓ(ਬਿੱਟ[i]);
				}
			}
	}

	ਖਾਲੀ ਜੋੜੋ_cell(RTLIL::Cell *cell)
	{
		ਜੇ (ct.cell_ਗਿਆਤ(cell->type)) {
			ਲਈ (ਸਵੈਚਾਲਿਤ &conn : cell->connections())
				ਜੋੜੋ_cell_ਪੋਰਟ(cell, conn.ਪਹਿਲਾ, sigmap(conn.ਦੂਜਾ),
						ct.cell_output(cell->type, conn.ਪਹਿਲਾ),
						ct.cell_ਨਿਵੇਸ਼(cell->type, conn.ਪਹਿਲਾ));
		} ਨਹੀਂਤਾਂ {
			ਲਈ (ਸਵੈਚਾਲਿਤ &conn : cell->connections())
				ਜੋੜੋ_cell_ਪੋਰਟ(cell, conn.ਪਹਿਲਾ, sigmap(conn.ਦੂਜਾ), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(ਨਿਰਾ)
	{
		ct.ਤਿਆਰੀ(design);
		ਜੇ (module)
			ਤਿਆਰੀ(module);
	}

	ਖਾਲੀ ਤਿਆਰੀ(RTLIL::Module *module, CellTypes *filter_ct = ਨਿਰਾ)
	{
		this->module = module;

		sigmap.ਸੈੱਟ(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		ਲਈ (ਸਵੈਚਾਲਿਤ &it : module->wires_)
			ਜੋੜੋ_wire(it.ਦੂਜਾ);
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : module->cells_)
			ਜੇ (filter_ct == ਨਿਰਾ || filter_ct->cell_ਗਿਆਤ(it.ਦੂਜਾ->type))
				ਜੋੜੋ_cell(it.ਦੂਜਾ);
	}

	// get_* methods -- single RTLIL::SigBit

	ਸਤਰਅੰਦਰ bool get_drivers(pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> &ਨਤੀਜਾ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
	{
		bool ਮਿਲਿਆ = false;
		ਜੇ (signal_drivers.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ)) {
			ਅਚਲ pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> &r = signal_drivers.at(ਦੋਆਧਾਰੀ_ਅੰਕ);
			ਨਤੀਜਾ.ਪਾਓ(r.begin(), r.ਸਿਰਾ());
			ਮਿਲਿਆ = true;
		}
		ਵਾਪਸ ਮਿਲਿਆ;
	}

	ਸਤਰਅੰਦਰ bool get_consumers(pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> &ਨਤੀਜਾ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
	{
		bool ਮਿਲਿਆ = false;
		ਜੇ (signal_consumers.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ)) {
			ਅਚਲ pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> &r = signal_consumers.at(ਦੋਆਧਾਰੀ_ਅੰਕ);
			ਨਤੀਜਾ.ਪਾਓ(r.begin(), r.ਸਿਰਾ());
			ਮਿਲਿਆ = true;
		}
		ਵਾਪਸ ਮਿਲਿਆ;
	}

	ਸਤਰਅੰਦਰ bool get_inputs(pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਤੀਜਾ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
	{
		bool ਮਿਲਿਆ = false;
		ਜੇ (signal_inputs.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ))
			ਨਤੀਜਾ.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ), ਮਿਲਿਆ = true;
		ਵਾਪਸ ਮਿਲਿਆ;
	}

	ਸਤਰਅੰਦਰ bool get_outputs(pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਤੀਜਾ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
	{
		bool ਮਿਲਿਆ = false;
		ਜੇ (signal_outputs.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ))
			ਨਤੀਜਾ.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ), ਮਿਲਿਆ = true;
		ਵਾਪਸ ਮਿਲਿਆ;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	ਸਾਂਚਾ<typename T>
	ਸਤਰਅੰਦਰ bool get_drivers(pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> &ਨਤੀਜਾ, ਅਚਲ T &ਬਿੱਟ) ਅਚਲ
	{
		bool ਮਿਲਿਆ = false;
		ਲਈ (RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ)
			ਜੇ (signal_drivers.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ)) {
				ਅਚਲ pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> &r = signal_drivers.at(ਦੋਆਧਾਰੀ_ਅੰਕ);
				ਨਤੀਜਾ.ਪਾਓ(r.begin(), r.ਸਿਰਾ());
				ਮਿਲਿਆ = true;
			}
		ਵਾਪਸ ਮਿਲਿਆ;
	}

	ਸਾਂਚਾ<typename T>
	ਸਤਰਅੰਦਰ bool get_consumers(pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> &ਨਤੀਜਾ, ਅਚਲ T &ਬਿੱਟ) ਅਚਲ
	{
		bool ਮਿਲਿਆ = false;
		ਲਈ (RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ)
			ਜੇ (signal_consumers.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ)) {
				ਅਚਲ pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> &r = signal_consumers.at(ਦੋਆਧਾਰੀ_ਅੰਕ);
				ਨਤੀਜਾ.ਪਾਓ(r.begin(), r.ਸਿਰਾ());
				ਮਿਲਿਆ = true;
			}
		ਵਾਪਸ ਮਿਲਿਆ;
	}

	ਸਾਂਚਾ<typename T>
	ਸਤਰਅੰਦਰ bool get_inputs(pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਤੀਜਾ, ਅਚਲ T &ਬਿੱਟ) ਅਚਲ
	{
		bool ਮਿਲਿਆ = false;
		ਲਈ (RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ)
			ਜੇ (signal_inputs.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ))
				ਨਤੀਜਾ.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ), ਮਿਲਿਆ = true;
		ਵਾਪਸ ਮਿਲਿਆ;
	}

	ਸਾਂਚਾ<typename T>
	ਸਤਰਅੰਦਰ bool get_outputs(pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਤੀਜਾ, ਅਚਲ T &ਬਿੱਟ) ਅਚਲ
	{
		bool ਮਿਲਿਆ = false;
		ਲਈ (RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ)
			ਜੇ (signal_outputs.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ))
				ਨਤੀਜਾ.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ), ਮਿਲਿਆ = true;
		ਵਾਪਸ ਮਿਲਿਆ;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> &ਨਤੀਜਾ, RTLIL::SigSpec ਸੰਕੇਤ) ਅਚਲ
	{
		std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ = sigmap(ਸੰਕੇਤ);
		ਵਾਪਸ get_drivers(ਨਤੀਜਾ, ਬਿੱਟ);
	}

	bool get_consumers(pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> &ਨਤੀਜਾ, RTLIL::SigSpec ਸੰਕੇਤ) ਅਚਲ
	{
		std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ = sigmap(ਸੰਕੇਤ);
		ਵਾਪਸ get_consumers(ਨਤੀਜਾ, ਬਿੱਟ);
	}

	bool get_inputs(pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਤੀਜਾ, RTLIL::SigSpec ਸੰਕੇਤ) ਅਚਲ
	{
		std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ = sigmap(ਸੰਕੇਤ);
		ਵਾਪਸ get_inputs(ਨਤੀਜਾ, ਬਿੱਟ);
	}

	bool get_outputs(pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਤੀਜਾ, RTLIL::SigSpec ਸੰਕੇਤ) ਅਚਲ
	{
		std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ = sigmap(ਸੰਕੇਤ);
		ਵਾਪਸ get_outputs(ਨਤੀਜਾ, ਬਿੱਟ);
	}

	// has_* methods -- call by reference

	ਸਾਂਚਾ<typename T>
	ਸਤਰਅੰਦਰ bool has_drivers(ਅਚਲ T &sig) ਅਚਲ {
		pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> ਨਤੀਜਾ;
		ਵਾਪਸ get_drivers(ਨਤੀਜਾ, sig);
	}

	ਸਾਂਚਾ<typename T>
	ਸਤਰਅੰਦਰ bool has_consumers(ਅਚਲ T &sig) ਅਚਲ {
		pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> ਨਤੀਜਾ;
		ਵਾਪਸ get_consumers(ਨਤੀਜਾ, sig);
	}

	ਸਾਂਚਾ<typename T>
	ਸਤਰਅੰਦਰ bool has_inputs(ਅਚਲ T &sig) ਅਚਲ {
		pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਨਤੀਜਾ;
		ਵਾਪਸ get_inputs(ਨਤੀਜਾ, sig);
	}

	ਸਾਂਚਾ<typename T>
	ਸਤਰਅੰਦਰ bool has_outputs(ਅਚਲ T &sig) ਅਚਲ {
		pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਨਤੀਜਾ;
		ਵਾਪਸ get_outputs(ਨਤੀਜਾ, sig);
	}

	// has_* methods -- call by value

	ਸਤਰਅੰਦਰ bool has_drivers(RTLIL::SigSpec sig) ਅਚਲ {
		pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> ਨਤੀਜਾ;
		ਵਾਪਸ get_drivers(ਨਤੀਜਾ, sig);
	}

	ਸਤਰਅੰਦਰ bool has_consumers(RTLIL::SigSpec sig) ਅਚਲ {
		pool<ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> ਨਤੀਜਾ;
		ਵਾਪਸ get_consumers(ਨਤੀਜਾ, sig);
	}

	ਸਤਰਅੰਦਰ bool has_inputs(RTLIL::SigSpec sig) ਅਚਲ {
		pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਨਤੀਜਾ;
		ਵਾਪਸ get_inputs(ਨਤੀਜਾ, sig);
	}

	ਸਤਰਅੰਦਰ bool has_outputs(RTLIL::SigSpec sig) ਅਚਲ {
		pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਨਤੀਜਾ;
		ਵਾਪਸ get_outputs(ਨਤੀਜਾ, sig);
	}
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
