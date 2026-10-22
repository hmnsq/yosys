/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2022  Jannis Harder <jix@yosyshq.com> <me@jix.one>
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

#جيڪڏهن_وصف_نه_ٿيل YW_H
#وصف_ڪريو YW_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

بنية Id_رستو : عام std::ويڪٽر<std::string>
{
	سانچو<typename... T>
	Id_رستو(T&&... دليلون_2) : std::ويڪٽر<std::string>(std::forward<T>(دليلون_2)...) { }
	Id_رستو اڳياڙي_2() ثابت { أرجع {begin(), پڇاڙي() - !خالي_جايون()}; }
	std::string str() ثابت;

	bool has_پتو() ثابت { صحيح tmp; أرجع get_پتو(tmp); };
	bool get_پتو(صحيح &addr) ثابت;

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(static_cast<ثابت std::ويڪٽر<std::string>&&>(*this));
		أرجع h;
	}
};

بنية Witness_Hierarchy_شي {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	Witness_Hierarchy_شي(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	Witness_Hierarchy_شي(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	Witness_Hierarchy_شي(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

سانچو<typename D, typename T>
فراغ witness_hierarchy(RTLIL::Module *module, D معلومات, T callback);

سانچو<class T> ساكن std::ويڪٽر<std::string> witness_رستو(T *obj) {
	std::ويڪٽر<std::string> رستو_2;
	إذا (obj->name.is_عوامي()) {
		خودڪار hdlname = obj->get_string_attribute(ID::hdlname);
		لكل (خودڪار token : ورهايو_ٽوڪن(hdlname))
			رستو_2.ڌڪو_پٺ("\\" + token);
	}
	إذا (رستو_2.خالي_جايون())
		رستو_2.ڌڪو_پٺ(obj->name.str());
	أرجع رستو_2;
}

بنية ReadWitness
{
	بنية Clock {
		Id_رستو رستو_2;
		صحيح وٿي;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	بنية Signal {
		Id_رستو رستو_2;
		صحيح وٿي;
		صحيح ويڪر;
		bool init_only;

		صحيح بٽ_وٿي;
	};

	بنية قدم_2 {
		std::string بٽ;
	};

	std::string filename;
	std::ويڪٽر<Clock> clocks;
	std::ويڪٽر<Signal> سگنل;
	std::ويڪٽر<قدم_2> قدمن;

	ReadWitness(ثابت std::string &filename);

	RTLIL::Const get_بٽ(صحيح t, صحيح بٽ_وٿي, صحيح ويڪر) ثابت;
};

سانچو<typename D, typename T>
فراغ witness_hierarchy_recursion(Id_رستو &رستو_2, صحيح hdlname_گهڻائي, RTLIL::Module *module, D معلومات, T &callback)
{
	خودڪار ثابت &const_رستو = رستو_2;
	سائيز_t رستو_سائيز = رستو_2.سائيز();
	لكل (خودڪار wire : module->wires())
	{
		خودڪار hdlname = hdlname_گهڻائي < 0 ? std::ويڪٽر<std::string>() : wire->get_hdlname_attribute();
		لكل (خودڪار شي : hdlname)
			رستو_2.ڌڪو_پٺ("\\" + شي);
		إذا (hdlname.سائيز() == 1 && wire->name == رستو_2.پٺ())
			hdlname.clear();
		إذا (!hdlname.خالي_جايون())
			callback(const_رستو, Witness_Hierarchy_شي(module, wire), معلومات);
		رستو_2.resize(رستو_سائيز);
		إذا (hdlname.خالي_جايون() || hdlname_گهڻائي <= 0) {
			رستو_2.ڌڪو_پٺ(wire->name);
			callback(const_رستو, Witness_Hierarchy_شي(module, wire), معلومات);
			رستو_2.ڪڍو_پٺ();
		}
	}

	لكل (خودڪار cell : module->cells())
	{
		Module *ٻار = module->design->module(cell->type);
		إذا (ٻار == nullptr)
			استمر;

		خودڪار hdlname = hdlname_گهڻائي < 0 ? std::ويڪٽر<std::string>() : cell->get_hdlname_attribute();
		لكل (خودڪار شي : hdlname)
			رستو_2.ڌڪو_پٺ("\\" + شي);
		إذا (hdlname.سائيز() == 1 && cell->name == رستو_2.پٺ())
			hdlname.clear();
		إذا (!hdlname.خالي_جايون()) {
			D ٻار_معلومات = callback(const_رستو, Witness_Hierarchy_شي(module, cell), معلومات);
			witness_hierarchy_recursion<D, T>(رستو_2, 1, ٻار, ٻار_معلومات, callback);
		}
		رستو_2.resize(رستو_سائيز);
		إذا (hdlname.خالي_جايون() || hdlname_گهڻائي <= 0) {
			رستو_2.ڌڪو_پٺ(cell->name);
			D ٻار_معلومات = callback(const_رستو, Witness_Hierarchy_شي(module, cell), معلومات);
			witness_hierarchy_recursion<D, T>(رستو_2, hdlname.خالي_جايون() ? hdlname_گهڻائي : -1, ٻار, ٻار_معلومات, callback);
			رستو_2.ڪڍو_پٺ();
		}
	}

	لكل (خودڪار mem : Mem::get_all_memories(module)) {
		std::ويڪٽر<std::string> hdlname;

		إذا (hdlname_گهڻائي >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		لكل (خودڪار شي : hdlname)
			رستو_2.ڌڪو_پٺ("\\" + شي);
		إذا (hdlname.سائيز() == 1 && mem.cell->name == رستو_2.پٺ())
			hdlname.clear();
		إذا (!hdlname.خالي_جايون()) {
			callback(const_رستو, Witness_Hierarchy_شي(module, &mem), معلومات);
		}
		رستو_2.resize(رستو_سائيز);

		إذا (hdlname.خالي_جايون() || hdlname_گهڻائي <= 0) {
			رستو_2.ڌڪو_پٺ(mem.memid.str());
			callback(const_رستو, Witness_Hierarchy_شي(module, &mem), معلومات);
			رستو_2.ڪڍو_پٺ();

			إذا (mem.cell != nullptr && mem.cell->name != mem.memid) {
				رستو_2.ڌڪو_پٺ(mem.cell->name);
				callback(const_رستو, Witness_Hierarchy_شي(module, &mem), معلومات);
				رستو_2.ڪڍو_پٺ();
			}
		}
	}
}

سانچو<typename D, typename T>
فراغ witness_hierarchy(RTLIL::Module *module, D معلومات, T callback)
{
	Id_رستو رستو_2;
	witness_hierarchy_recursion<D, T>(رستو_2, 0, module, معلومات, callback);
}

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
