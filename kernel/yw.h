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

#اگر_تعریف_نشده YW_H
#تعریف_کن YW_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

بنية Id_مسیر : عام std::وکتور<std::string>
{
	قالب<typename... T>
	Id_مسیر(T&&... آرگومانها_2) : std::وکتور<std::string>(std::forward<T>(آرگومانها_2)...) { }
	Id_مسیر پیشوند_2() ثابت { أرجع {begin(), انتها() - !خالی()}; }
	std::string str() ثابت;

	bool has_آدرس() ثابت { صحيح tmp; أرجع get_آدرس(tmp); };
	bool get_آدرس(صحيح &addr) ثابت;

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(static_cast<ثابت std::وکتور<std::string>&&>(*this));
		أرجع h;
	}
};

بنية Witness_Hierarchy_قلم {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	Witness_Hierarchy_قلم(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	Witness_Hierarchy_قلم(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	Witness_Hierarchy_قلم(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

قالب<typename D, typename T>
فراغ witness_hierarchy(RTLIL::Module *module, D داده, T callback);

قالب<class T> ساكن std::وکتور<std::string> witness_مسیر(T *obj) {
	std::وکتور<std::string> مسیر_2;
	إذا (obj->name.is_عمومی()) {
		خودکار hdlname = obj->get_string_attribute(ID::hdlname);
		لكل (خودکار token : تقسیم_نشانهها(hdlname))
			مسیر_2.افزودن_پشت("\\" + token);
	}
	إذا (مسیر_2.خالی())
		مسیر_2.افزودن_پشت(obj->name.str());
	أرجع مسیر_2;
}

بنية ReadWitness
{
	بنية Clock {
		Id_مسیر مسیر_2;
		صحيح جابه_جایی;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	بنية Signal {
		Id_مسیر مسیر_2;
		صحيح جابه_جایی;
		صحيح عرض;
		bool init_only;

		صحيح بیتها_جابه_جایی;
	};

	بنية گام_2 {
		std::string بیتها;
	};

	std::string filename;
	std::وکتور<Clock> clocks;
	std::وکتور<Signal> سیگنالها;
	std::وکتور<گام_2> گامها;

	ReadWitness(ثابت std::string &filename);

	RTLIL::Const get_بیتها(صحيح t, صحيح بیتها_جابه_جایی, صحيح عرض) ثابت;
};

قالب<typename D, typename T>
فراغ witness_hierarchy_recursion(Id_مسیر &مسیر_2, صحيح hdlname_نما, RTLIL::Module *module, D داده, T &callback)
{
	خودکار ثابت &const_مسیر = مسیر_2;
	اندازه_t مسیر_اندازه = مسیر_2.اندازه();
	لكل (خودکار wire : module->wires())
	{
		خودکار hdlname = hdlname_نما < 0 ? std::وکتور<std::string>() : wire->get_hdlname_attribute();
		لكل (خودکار قلم : hdlname)
			مسیر_2.افزودن_پشت("\\" + قلم);
		إذا (hdlname.اندازه() == 1 && wire->name == مسیر_2.پشت())
			hdlname.clear();
		إذا (!hdlname.خالی())
			callback(const_مسیر, Witness_Hierarchy_قلم(module, wire), داده);
		مسیر_2.resize(مسیر_اندازه);
		إذا (hdlname.خالی() || hdlname_نما <= 0) {
			مسیر_2.افزودن_پشت(wire->name);
			callback(const_مسیر, Witness_Hierarchy_قلم(module, wire), داده);
			مسیر_2.برداشت_پشت();
		}
	}

	لكل (خودکار cell : module->cells())
	{
		Module *فرزند = module->design->module(cell->type);
		إذا (فرزند == nullptr)
			استمر;

		خودکار hdlname = hdlname_نما < 0 ? std::وکتور<std::string>() : cell->get_hdlname_attribute();
		لكل (خودکار قلم : hdlname)
			مسیر_2.افزودن_پشت("\\" + قلم);
		إذا (hdlname.اندازه() == 1 && cell->name == مسیر_2.پشت())
			hdlname.clear();
		إذا (!hdlname.خالی()) {
			D فرزند_داده = callback(const_مسیر, Witness_Hierarchy_قلم(module, cell), داده);
			witness_hierarchy_recursion<D, T>(مسیر_2, 1, فرزند, فرزند_داده, callback);
		}
		مسیر_2.resize(مسیر_اندازه);
		إذا (hdlname.خالی() || hdlname_نما <= 0) {
			مسیر_2.افزودن_پشت(cell->name);
			D فرزند_داده = callback(const_مسیر, Witness_Hierarchy_قلم(module, cell), داده);
			witness_hierarchy_recursion<D, T>(مسیر_2, hdlname.خالی() ? hdlname_نما : -1, فرزند, فرزند_داده, callback);
			مسیر_2.برداشت_پشت();
		}
	}

	لكل (خودکار mem : Mem::get_all_memories(module)) {
		std::وکتور<std::string> hdlname;

		إذا (hdlname_نما >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		لكل (خودکار قلم : hdlname)
			مسیر_2.افزودن_پشت("\\" + قلم);
		إذا (hdlname.اندازه() == 1 && mem.cell->name == مسیر_2.پشت())
			hdlname.clear();
		إذا (!hdlname.خالی()) {
			callback(const_مسیر, Witness_Hierarchy_قلم(module, &mem), داده);
		}
		مسیر_2.resize(مسیر_اندازه);

		إذا (hdlname.خالی() || hdlname_نما <= 0) {
			مسیر_2.افزودن_پشت(mem.memid.str());
			callback(const_مسیر, Witness_Hierarchy_قلم(module, &mem), داده);
			مسیر_2.برداشت_پشت();

			إذا (mem.cell != nullptr && mem.cell->name != mem.memid) {
				مسیر_2.افزودن_پشت(mem.cell->name);
				callback(const_مسیر, Witness_Hierarchy_قلم(module, &mem), داده);
				مسیر_2.برداشت_پشت();
			}
		}
	}
}

قالب<typename D, typename T>
فراغ witness_hierarchy(RTLIL::Module *module, D داده, T callback)
{
	Id_مسیر مسیر_2;
	witness_hierarchy_recursion<D, T>(مسیر_2, 0, module, داده, callback);
}

YOSYS_NAMESPACE_انتها

#پایان_اگر
