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

#جے_بنا_تعریف YW_H
#تعریف_کرو YW_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

بنية Id_رستہ : عام std::سمتیہ<std::string>
{
	سانچا<typename... T>
	Id_رستہ(T&&... دلیلاں_2) : std::سمتیہ<std::string>(std::forward<T>(دلیلاں_2)...) { }
	Id_رستہ اگیتر_2() ثابت { أرجع {begin(), سرا() - !خالی_تھاواں()}; }
	std::string str() ثابت;

	bool has_پتہ() ثابت { صحيح tmp; أرجع get_پتہ(tmp); };
	bool get_پتہ(صحيح &addr) ثابت;

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(static_cast<ثابت std::سمتیہ<std::string>&&>(*this));
		أرجع h;
	}
};

بنية Witness_Hierarchy_شے {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	Witness_Hierarchy_شے(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	Witness_Hierarchy_شے(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	Witness_Hierarchy_شے(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

سانچا<typename D, typename T>
فراغ witness_hierarchy(RTLIL::Module *module, D انکڑے, T callback);

سانچا<class T> ساكن std::سمتیہ<std::string> witness_رستہ(T *obj) {
	std::سمتیہ<std::string> رستہ_2;
	إذا (obj->name.is_عوامی()) {
		خودکار hdlname = obj->get_string_attribute(ID::hdlname);
		لكل (خودکار token : ونڈو_ٹوکن(hdlname))
			رستہ_2.ڈالو_پٹھ("\\" + token);
	}
	إذا (رستہ_2.خالی_تھاواں())
		رستہ_2.ڈالو_پٹھ(obj->name.str());
	أرجع رستہ_2;
}

بنية ReadWitness
{
	بنية Clock {
		Id_رستہ رستہ_2;
		صحيح وتھ;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	بنية Signal {
		Id_رستہ رستہ_2;
		صحيح وتھ;
		صحيح چوڑائی;
		bool init_only;

		صحيح بٹ_وتھ;
	};

	بنية قدم_2 {
		std::string بٹ;
	};

	std::string filename;
	std::سمتیہ<Clock> clocks;
	std::سمتیہ<Signal> اشارے;
	std::سمتیہ<قدم_2> قدماں;

	ReadWitness(ثابت std::string &filename);

	RTLIL::Const get_بٹ(صحيح t, صحيح بٹ_وتھ, صحيح چوڑائی) ثابت;
};

سانچا<typename D, typename T>
فراغ witness_hierarchy_recursion(Id_رستہ &رستہ_2, صحيح hdlname_منوال, RTLIL::Module *module, D انکڑے, T &callback)
{
	خودکار ثابت &const_رستہ = رستہ_2;
	ناپ_t رستہ_ناپ = رستہ_2.ناپ();
	لكل (خودکار wire : module->wires())
	{
		خودکار hdlname = hdlname_منوال < 0 ? std::سمتیہ<std::string>() : wire->get_hdlname_attribute();
		لكل (خودکار شے : hdlname)
			رستہ_2.ڈالو_پٹھ("\\" + شے);
		إذا (hdlname.ناپ() == 1 && wire->name == رستہ_2.پٹھ())
			hdlname.clear();
		إذا (!hdlname.خالی_تھاواں())
			callback(const_رستہ, Witness_Hierarchy_شے(module, wire), انکڑے);
		رستہ_2.resize(رستہ_ناپ);
		إذا (hdlname.خالی_تھاواں() || hdlname_منوال <= 0) {
			رستہ_2.ڈالو_پٹھ(wire->name);
			callback(const_رستہ, Witness_Hierarchy_شے(module, wire), انکڑے);
			رستہ_2.نکالو_پٹھ();
		}
	}

	لكل (خودکار cell : module->cells())
	{
		Module *بچہ = module->design->module(cell->type);
		إذا (بچہ == nullptr)
			استمر;

		خودکار hdlname = hdlname_منوال < 0 ? std::سمتیہ<std::string>() : cell->get_hdlname_attribute();
		لكل (خودکار شے : hdlname)
			رستہ_2.ڈالو_پٹھ("\\" + شے);
		إذا (hdlname.ناپ() == 1 && cell->name == رستہ_2.پٹھ())
			hdlname.clear();
		إذا (!hdlname.خالی_تھاواں()) {
			D بچہ_انکڑے = callback(const_رستہ, Witness_Hierarchy_شے(module, cell), انکڑے);
			witness_hierarchy_recursion<D, T>(رستہ_2, 1, بچہ, بچہ_انکڑے, callback);
		}
		رستہ_2.resize(رستہ_ناپ);
		إذا (hdlname.خالی_تھاواں() || hdlname_منوال <= 0) {
			رستہ_2.ڈالو_پٹھ(cell->name);
			D بچہ_انکڑے = callback(const_رستہ, Witness_Hierarchy_شے(module, cell), انکڑے);
			witness_hierarchy_recursion<D, T>(رستہ_2, hdlname.خالی_تھاواں() ? hdlname_منوال : -1, بچہ, بچہ_انکڑے, callback);
			رستہ_2.نکالو_پٹھ();
		}
	}

	لكل (خودکار mem : Mem::get_all_memories(module)) {
		std::سمتیہ<std::string> hdlname;

		إذا (hdlname_منوال >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		لكل (خودکار شے : hdlname)
			رستہ_2.ڈالو_پٹھ("\\" + شے);
		إذا (hdlname.ناپ() == 1 && mem.cell->name == رستہ_2.پٹھ())
			hdlname.clear();
		إذا (!hdlname.خالی_تھاواں()) {
			callback(const_رستہ, Witness_Hierarchy_شے(module, &mem), انکڑے);
		}
		رستہ_2.resize(رستہ_ناپ);

		إذا (hdlname.خالی_تھاواں() || hdlname_منوال <= 0) {
			رستہ_2.ڈالو_پٹھ(mem.memid.str());
			callback(const_رستہ, Witness_Hierarchy_شے(module, &mem), انکڑے);
			رستہ_2.نکالو_پٹھ();

			إذا (mem.cell != nullptr && mem.cell->name != mem.memid) {
				رستہ_2.ڈالو_پٹھ(mem.cell->name);
				callback(const_رستہ, Witness_Hierarchy_شے(module, &mem), انکڑے);
				رستہ_2.نکالو_پٹھ();
			}
		}
	}
}

سانچا<typename D, typename T>
فراغ witness_hierarchy(RTLIL::Module *module, D انکڑے, T callback)
{
	Id_رستہ رستہ_2;
	witness_hierarchy_recursion<D, T>(رستہ_2, 0, module, انکڑے, callback);
}

YOSYS_NAMESPACE_سرا

#جے_انت
