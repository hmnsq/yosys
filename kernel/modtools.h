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

#اگر_تعریف_نشده MODTOOLS_H
#تعریف_کن MODTOOLS_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/celltypes.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

بنية Mod_شاخص : عام RTLIL::Monitor
{
	بنية اشاره_گر_Ordered_Sig_بیت : عام RTLIL::Sig_بیت {
		اشاره_گر_Ordered_Sig_بیت(Sig_بیت s) {
			wire = s.wire;
			إذا (wire)
				جابه_جایی = s.جابه_جایی;
			وإلا
				داده = s.داده;
		}
		مضمن bool عملگر<(ثابت RTLIL::Sig_بیت &other) ثابت {
			إذا (wire == other.wire)
				أرجع wire ? (جابه_جایی < other.جابه_جایی) : (داده < other.داده);
			إذا (wire != nullptr && other.wire != nullptr)
				أرجع wire < other.wire; // look here
			أرجع (wire != nullptr) < (other.wire != nullptr);
		}
	};
	بنية پورت_Info {
		RTLIL::Cell* cell;
		RTLIL::IdString پورت;
		صحيح جابه_جایی;

		پورت_Info() : cell(), پورت(), جابه_جایی() { }
		پورت_Info(RTLIL::Cell* _c, RTLIL::IdString _p, صحيح _o) : cell(_c), پورت(_p), جابه_جایی(_o) { }

		bool عملگر<(ثابت پورت_Info &other) ثابت {
			إذا (cell != other.cell)
				أرجع cell < other.cell;
			إذا (جابه_جایی != other.جابه_جایی)
				أرجع جابه_جایی < other.جابه_جایی;
			أرجع پورت < other.پورت;
		}

		bool عملگر==(ثابت پورت_Info &other) ثابت {
			أرجع cell == other.cell && پورت == other.پورت && جابه_جایی == other.جابه_جایی;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(پورت);
			h.eat(جابه_جایی);
			أرجع h;
		}
	};

	بنية Sig_بیت_Info
	{
		bool is_درون_داد, is_output;
		pool<پورت_Info> ports;

		Sig_بیت_Info() : is_درون_داد(false), is_output(false) { }

		bool عملگر==(ثابت Sig_بیت_Info &other) ثابت {
			أرجع is_درون_داد == other.is_درون_داد && is_output == other.is_output && ports == other.ports;
		}

		فراغ ادغام(ثابت Sig_بیت_Info &other)
		{
			is_درون_داد = is_درون_داد || other.is_درون_داد;
			is_output = is_output || other.is_output;
			ports.درج(other.ports.begin(), other.ports.انتها());
		}
	};

	SigMap sigmap;
	RTLIL::Module *module;
	std::map<اشاره_گر_Ordered_Sig_بیت, Sig_بیت_Info> database;
	صحيح auto_reload_counter;
	bool auto_reload_module;

	فراغ پورت_جمع_کن(RTLIL::Cell *cell, RTLIL::IdString پورت, ثابت RTLIL::SigSpec &sig)
	{
		لكل (صحيح i = 0; i < Get_اندازه(sig); i++) {
			RTLIL::Sig_بیت بیت = sigmap(sig[i]);
			إذا (بیت.wire)
				database[بیت].ports.درج(پورت_Info(cell, پورت, i));
		}
	}

	فراغ پورت_del(RTLIL::Cell *cell, RTLIL::IdString پورت, ثابت RTLIL::SigSpec &sig)
	{
		لكل (صحيح i = 0; i < Get_اندازه(sig); i++) {
			RTLIL::Sig_بیت بیت = sigmap(sig[i]);
			إذا (بیت.wire) {
				database[بیت].ports.erase(پورت_Info(cell, پورت, i));
				إذا (!database[بیت].is_درون_داد && !database[بیت].is_output && database[بیت].ports.خالی())
					database.erase(بیت);
			}
		}
	}

	ثابت Sig_بیت_Info &info(RTLIL::Sig_بیت بیت)
	{
		أرجع database[sigmap(بیت)];
	}

	فراغ reload_module(bool reset_sigmap = true)
	{
		إذا (reset_sigmap) {
			sigmap.clear();
			sigmap.مجموعه(module);
		}

		database.clear();
		لكل (خودکار wire : module->wires())
			إذا (wire->پورت_درون_داد || wire->پورت_output)
				لكل (صحيح i = 0; i < Get_اندازه(wire); i++) {
					RTLIL::Sig_بیت بیت = sigmap(RTLIL::Sig_بیت(wire, i));
					إذا (بیت.wire && wire->پورت_درون_داد)
						database[بیت].is_درون_داد = true;
					إذا (بیت.wire && wire->پورت_output)
						database[بیت].is_output = true;
				}
		لكل (خودکار cell : module->cells())
			لكل (خودکار &conn : cell->connections())
				پورت_جمع_کن(cell, conn.اولی, conn.دومی);

		إذا (auto_reload_module) {
			إذا (++auto_reload_counter > 2)
				log_warning("Auto-reload in ModIndex -- possible performance bug!\n");
			auto_reload_module = false;
		}
	}

	bool ok()
	{
#اگر_تعریف_نشده NDEBUG
		إذا (auto_reload_module)
			أرجع true;

		لكل (خودکار it : database)
			log_assert(it.اولی == sigmap(it.اولی));

		خودکار database_bak = std::move(database);
		reload_module(false);

		إذا (!(database == database_bak))
		{
			لكل (خودکار &it : database_bak)
				إذا (!database.شمار(it.اولی))
					لگاریتم("ModuleIndex::check(): Only in database_bak, not database: %s\n", log_signal(it.اولی));

			لكل (خودکار &it : database)
				إذا (!database_bak.شمار(it.اولی))
					لگاریتم("ModuleIndex::check(): Only in database, not database_bak: %s\n", log_signal(it.اولی));
				وإلا إذا (!(it.دومی == database_bak.at(it.اولی)))
					لگاریتم("ModuleIndex::check(): Different content for database[%s].\n", log_signal(it.اولی));

			أرجع false;
		}
#پایان_اگر
		أرجع true;
	}

	فراغ check()
	{
		log_assert(ok());
	}

	فراغ notify_connect(RTLIL::Cell *cell, RTLIL::IdString پورت, ثابت RTLIL::SigSpec &old_sig, ثابت RTLIL::SigSpec &sig) override
	{
		log_assert(module == cell->module);

		إذا (auto_reload_module)
			أرجع;

		پورت_del(cell, پورت, old_sig);
		پورت_جمع_کن(cell, پورت, sig);
	}

	فراغ notify_connect(RTLIL::Module *mod, ثابت RTLIL::SigSig &sigsig) override
	{
		log_assert(module == mod);

		إذا (auto_reload_module)
			أرجع;

		لكل (صحيح i = 0; i < Get_اندازه(sigsig.اولی); i++)
		{
			RTLIL::Sig_بیت lhs = sigmap(sigsig.اولی[i]);
			RTLIL::Sig_بیت rhs = sigmap(sigsig.دومی[i]);
			bool has_lhs = database.شمار(lhs) != 0;
			bool has_rhs = database.شمار(rhs) != 0;

			إذا (!has_lhs && !has_rhs) {
				sigmap.جمع_کن(lhs, rhs);
			} وإلا
			إذا (!has_rhs) {
				Sig_بیت_Info new_info = database.at(lhs);
				database.erase(lhs);
				sigmap.جمع_کن(lhs, rhs);
				lhs = sigmap(lhs);
				إذا (lhs.wire)
					database[lhs] = new_info;
			} وإلا
			إذا (!has_lhs) {
				Sig_بیت_Info new_info = database.at(rhs);
				database.erase(rhs);
				sigmap.جمع_کن(lhs, rhs);
				rhs = sigmap(rhs);
				إذا (rhs.wire)
					database[rhs] = new_info;
			} وإلا {
				Sig_بیت_Info new_info = database.at(lhs);
				new_info.ادغام(database.at(rhs));
				database.erase(lhs);
				database.erase(rhs);
				sigmap.جمع_کن(lhs, rhs);
				rhs = sigmap(rhs);
				إذا (rhs.wire)
					database[rhs] = new_info;
			}
		}
	}

	فراغ notify_connect(RTLIL::Module *mod, ثابت std::وکتور<RTLIL::SigSig>&) override
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
		module->monitors.درج(this);
	}

	~Mod_شاخص()
	{
		module->monitors.erase(this);
	}

	Sig_بیت_Info *query(RTLIL::Sig_بیت بیت)
	{
		إذا (auto_reload_module)
			reload_module();

		خودکار it = database.پیدا_کن(sigmap(بیت));
		إذا (it == database.انتها())
			أرجع nullptr;
		وإلا
			أرجع &it->دومی;
	}

	bool query_is_درون_داد(RTLIL::Sig_بیت بیت)
	{
		ثابت Sig_بیت_Info *info = query(بیت);
		إذا (info == nullptr)
			أرجع false;
		أرجع info->is_درون_داد;
	}

	bool query_is_output(RTLIL::Sig_بیت بیت)
	{
		ثابت Sig_بیت_Info *info = query(بیت);
		إذا (info == nullptr)
			أرجع false;
		أرجع info->is_output;
	}

	pool<پورت_Info> &query_ports(RTLIL::Sig_بیت بیت)
	{
		ساكن pool<پورت_Info> خالی_نتیجه_مجموعه;
		Sig_بیت_Info *info = query(بیت);
		إذا (info == nullptr)
			أرجع خالی_نتیجه_مجموعه;
		أرجع info->ports;
	}

	فراغ dump_db()
	{
		لگاریتم("--- ModIndex Dump ---\n");

		إذا (auto_reload_module) {
			لگاریتم("AUTO-RELOAD\n");
			reload_module();
		}

		لكل (خودکار &it : database) {
			لگاریتم("BIT %s:\n", log_signal(it.اولی));
			إذا (it.دومی.is_درون_داد)
				لگاریتم("  PRIMARY INPUT\n");
			إذا (it.دومی.is_output)
				لگاریتم("  PRIMARY OUTPUT\n");
			لكل (خودکار &پورت : it.دومی.ports)
				لگاریتم("  PORT: %s.%s[%d] (%s)\n", پورت.cell,
						PooledName(module, پورت.پورت).unescape(), پورت.جابه_جایی, پورت.cell->type.unescape());
		}
	}
};

بنية ModWalker
{
	بنية پورت_بیت_2
	{
		RTLIL::Cell *cell;
		RTLIL::IdString پورت;
		صحيح جابه_جایی;
		پورت_بیت_2(Cell* c, IdString p, صحيح o) : cell(c), پورت(p), جابه_جایی(o) {}

		bool عملگر<(ثابت پورت_بیت_2 &other) ثابت {
			إذا (cell != other.cell)
				أرجع cell < other.cell;
			إذا (پورت != other.پورت)
				أرجع پورت < other.پورت;
			أرجع جابه_جایی < other.جابه_جایی;
		}

		bool عملگر==(ثابت پورت_بیت_2 &other) ثابت {
			أرجع cell == other.cell && پورت == other.پورت && جابه_جایی == other.جابه_جایی;
		}

		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(cell->name);
			h.eat(پورت);
			h.eat(جابه_جایی);
			أرجع h;
		}
	};

	RTLIL::Design *design;
	RTLIL::Module *module;

	NewCellTypes ct;
	SigMap sigmap;

	dict<RTLIL::Sig_بیت, pool<پورت_بیت_2>> signal_drivers;
	dict<RTLIL::Sig_بیت, pool<پورت_بیت_2>> signal_consumers;
	pool<RTLIL::Sig_بیت> signal_inputs, signal_outputs;

	dict<RTLIL::Cell*, pool<RTLIL::Sig_بیت>> cell_outputs, cell_inputs;

	فراغ جمع_کن_wire(RTLIL::Wire *wire)
	{
		إذا (wire->پورت_درون_داد) {
			std::وکتور<RTLIL::Sig_بیت> بیتها = sigmap(wire);
			لكل (خودکار بیت : بیتها)
				إذا (بیت.wire != عدم)
					signal_inputs.درج(بیت);
		}

		إذا (wire->پورت_output) {
			std::وکتور<RTLIL::Sig_بیت> بیتها = sigmap(wire);
			لكل (خودکار بیت : بیتها)
				إذا (بیت.wire != عدم)
					signal_outputs.درج(بیت);
		}
	}

	فراغ جمع_کن_cell_پورت(RTLIL::Cell *cell, RTLIL::IdString پورت, std::وکتور<RTLIL::Sig_بیت> بیتها, bool is_output, bool is_درون_داد)
	{
		لكل (صحيح i = 0; i < صحيح(بیتها.اندازه()); i++)
			إذا (بیتها[i].wire != عدم) {
				پورت_بیت_2 pbit {cell, پورت, i};
				إذا (is_output) {
					signal_drivers[بیتها[i]].درج(pbit);
					cell_outputs[cell].درج(بیتها[i]);
				}
				إذا (is_درون_داد) {
					signal_consumers[بیتها[i]].درج(pbit);
					cell_inputs[cell].درج(بیتها[i]);
				}
			}
	}

	فراغ جمع_کن_cell(RTLIL::Cell *cell)
	{
		إذا (ct.cell_معلوم(cell->type)) {
			لكل (خودکار &conn : cell->connections())
				جمع_کن_cell_پورت(cell, conn.اولی, sigmap(conn.دومی),
						ct.cell_output(cell->type, conn.اولی),
						ct.cell_درون_داد(cell->type, conn.اولی));
		} وإلا {
			لكل (خودکار &conn : cell->connections())
				جمع_کن_cell_پورت(cell, conn.اولی, sigmap(conn.دومی), true, true);
		}
	}

	ModWalker(RTLIL::Design *design, RTLIL::Module *module = nullptr) : design(design), module(عدم)
	{
		ct.راهاندازی(design);
		إذا (module)
			راهاندازی(module);
	}

	فراغ راهاندازی(RTLIL::Module *module, CellTypes *filter_ct = عدم)
	{
		this->module = module;

		sigmap.مجموعه(module);

		signal_drivers.clear();
		signal_consumers.clear();
		signal_inputs.clear();
		signal_outputs.clear();
		cell_inputs.clear();
		cell_outputs.clear();

		لكل (خودکار &it : module->wires_)
			جمع_کن_wire(it.دومی);
		لكل (خودکار &it : module->cells_)
			إذا (filter_ct == عدم || filter_ct->cell_معلوم(it.دومی->type))
				جمع_کن_cell(it.دومی);
	}

	// get_* methods -- single RTLIL::SigBit

	مضمن bool get_drivers(pool<پورت_بیت_2> &نتیجه, RTLIL::Sig_بیت بیت) ثابت
	{
		bool یافت = false;
		إذا (signal_drivers.شمار(بیت)) {
			ثابت pool<پورت_بیت_2> &r = signal_drivers.at(بیت);
			نتیجه.درج(r.begin(), r.انتها());
			یافت = true;
		}
		أرجع یافت;
	}

	مضمن bool get_consumers(pool<پورت_بیت_2> &نتیجه, RTLIL::Sig_بیت بیت) ثابت
	{
		bool یافت = false;
		إذا (signal_consumers.شمار(بیت)) {
			ثابت pool<پورت_بیت_2> &r = signal_consumers.at(بیت);
			نتیجه.درج(r.begin(), r.انتها());
			یافت = true;
		}
		أرجع یافت;
	}

	مضمن bool get_inputs(pool<RTLIL::Sig_بیت> &نتیجه, RTLIL::Sig_بیت بیت) ثابت
	{
		bool یافت = false;
		إذا (signal_inputs.شمار(بیت))
			نتیجه.درج(بیت), یافت = true;
		أرجع یافت;
	}

	مضمن bool get_outputs(pool<RTLIL::Sig_بیت> &نتیجه, RTLIL::Sig_بیت بیت) ثابت
	{
		bool یافت = false;
		إذا (signal_outputs.شمار(بیت))
			نتیجه.درج(بیت), یافت = true;
		أرجع یافت;
	}

	// get_* methods -- container of RTLIL::SigBit's (always by reference)

	قالب<typename T>
	مضمن bool get_drivers(pool<پورت_بیت_2> &نتیجه, ثابت T &بیتها) ثابت
	{
		bool یافت = false;
		لكل (RTLIL::Sig_بیت بیت : بیتها)
			إذا (signal_drivers.شمار(بیت)) {
				ثابت pool<پورت_بیت_2> &r = signal_drivers.at(بیت);
				نتیجه.درج(r.begin(), r.انتها());
				یافت = true;
			}
		أرجع یافت;
	}

	قالب<typename T>
	مضمن bool get_consumers(pool<پورت_بیت_2> &نتیجه, ثابت T &بیتها) ثابت
	{
		bool یافت = false;
		لكل (RTLIL::Sig_بیت بیت : بیتها)
			إذا (signal_consumers.شمار(بیت)) {
				ثابت pool<پورت_بیت_2> &r = signal_consumers.at(بیت);
				نتیجه.درج(r.begin(), r.انتها());
				یافت = true;
			}
		أرجع یافت;
	}

	قالب<typename T>
	مضمن bool get_inputs(pool<RTLIL::Sig_بیت> &نتیجه, ثابت T &بیتها) ثابت
	{
		bool یافت = false;
		لكل (RTLIL::Sig_بیت بیت : بیتها)
			إذا (signal_inputs.شمار(بیت))
				نتیجه.درج(بیت), یافت = true;
		أرجع یافت;
	}

	قالب<typename T>
	مضمن bool get_outputs(pool<RTLIL::Sig_بیت> &نتیجه, ثابت T &بیتها) ثابت
	{
		bool یافت = false;
		لكل (RTLIL::Sig_بیت بیت : بیتها)
			إذا (signal_outputs.شمار(بیت))
				نتیجه.درج(بیت), یافت = true;
		أرجع یافت;
	}

	// get_* methods -- call by RTLIL::SigSpec (always by value)

	bool get_drivers(pool<پورت_بیت_2> &نتیجه, RTLIL::SigSpec إشارة) ثابت
	{
		std::وکتور<RTLIL::Sig_بیت> بیتها = sigmap(إشارة);
		أرجع get_drivers(نتیجه, بیتها);
	}

	bool get_consumers(pool<پورت_بیت_2> &نتیجه, RTLIL::SigSpec إشارة) ثابت
	{
		std::وکتور<RTLIL::Sig_بیت> بیتها = sigmap(إشارة);
		أرجع get_consumers(نتیجه, بیتها);
	}

	bool get_inputs(pool<RTLIL::Sig_بیت> &نتیجه, RTLIL::SigSpec إشارة) ثابت
	{
		std::وکتور<RTLIL::Sig_بیت> بیتها = sigmap(إشارة);
		أرجع get_inputs(نتیجه, بیتها);
	}

	bool get_outputs(pool<RTLIL::Sig_بیت> &نتیجه, RTLIL::SigSpec إشارة) ثابت
	{
		std::وکتور<RTLIL::Sig_بیت> بیتها = sigmap(إشارة);
		أرجع get_outputs(نتیجه, بیتها);
	}

	// has_* methods -- call by reference

	قالب<typename T>
	مضمن bool has_drivers(ثابت T &sig) ثابت {
		pool<پورت_بیت_2> نتیجه;
		أرجع get_drivers(نتیجه, sig);
	}

	قالب<typename T>
	مضمن bool has_consumers(ثابت T &sig) ثابت {
		pool<پورت_بیت_2> نتیجه;
		أرجع get_consumers(نتیجه, sig);
	}

	قالب<typename T>
	مضمن bool has_inputs(ثابت T &sig) ثابت {
		pool<RTLIL::Sig_بیت> نتیجه;
		أرجع get_inputs(نتیجه, sig);
	}

	قالب<typename T>
	مضمن bool has_outputs(ثابت T &sig) ثابت {
		pool<RTLIL::Sig_بیت> نتیجه;
		أرجع get_outputs(نتیجه, sig);
	}

	// has_* methods -- call by value

	مضمن bool has_drivers(RTLIL::SigSpec sig) ثابت {
		pool<پورت_بیت_2> نتیجه;
		أرجع get_drivers(نتیجه, sig);
	}

	مضمن bool has_consumers(RTLIL::SigSpec sig) ثابت {
		pool<پورت_بیت_2> نتیجه;
		أرجع get_consumers(نتیجه, sig);
	}

	مضمن bool has_inputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::Sig_بیت> نتیجه;
		أرجع get_inputs(نتیجه, sig);
	}

	مضمن bool has_outputs(RTLIL::SigSpec sig) ثابت {
		pool<RTLIL::Sig_بیت> نتیجه;
		أرجع get_outputs(نتیجه, sig);
	}
};

YOSYS_NAMESPACE_انتها

#پایان_اگر
