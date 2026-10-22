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

#که_نه_تعریف_شوی YW_H
#تعریف_کړه YW_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

بنية Id_لار : عام std::ویکټور<std::string>
{
	کينډۍ<typename... T>
	Id_لار(T&&... دلیلونه_2) : std::ویکټور<std::string>(std::forward<T>(دلیلونه_2)...) { }
	Id_لار مختاړی_2() ثابت { أرجع {begin(), پای_ټکی() - !تشه()}; }
	std::string str() ثابت;

	bool has_پته() ثابت { صحيح tmp; أرجع get_پته(tmp); };
	bool get_پته(صحيح &addr) ثابت;

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(static_cast<ثابت std::ویکټور<std::string>&&>(*this));
		أرجع h;
	}
};

بنية Witness_Hierarchy_توکی {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	Witness_Hierarchy_توکی(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	Witness_Hierarchy_توکی(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	Witness_Hierarchy_توکی(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

کينډۍ<typename D, typename T>
فراغ witness_hierarchy(RTLIL::Module *module, D معلومات, T callback);

کينډۍ<class T> ساكن std::ویکټور<std::string> witness_لار(T *obj) {
	std::ویکټور<std::string> لار_2;
	إذا (obj->name.is_عامه()) {
		خپلکاری hdlname = obj->get_string_attribute(ID::hdlname);
		لكل (خپلکاری token : وېشل_نښې(hdlname))
			لار_2.ټېل_شا("\\" + token);
	}
	إذا (لار_2.تشه())
		لار_2.ټېل_شا(obj->name.str());
	أرجع لار_2;
}

بنية ReadWitness
{
	بنية Clock {
		Id_لار لار_2;
		صحيح دننه_واټن;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	بنية Signal {
		Id_لار لار_2;
		صحيح دننه_واټن;
		صحيح پلنوالی;
		bool init_only;

		صحيح بټونه_دننه_واټن;
	};

	بنية ګام_2 {
		std::string بټونه;
	};

	std::string filename;
	std::ویکټور<Clock> clocks;
	std::ویکټور<Signal> سیګنالونه;
	std::ویکټور<ګام_2> ګامونه;

	ReadWitness(ثابت std::string &filename);

	RTLIL::Const get_بټونه(صحيح t, صحيح بټونه_دننه_واټن, صحيح پلنوالی) ثابت;
};

کينډۍ<typename D, typename T>
فراغ witness_hierarchy_recursion(Id_لار &لار_2, صحيح hdlname_ډېرپېښ, RTLIL::Module *module, D معلومات, T &callback)
{
	خپلکاری ثابت &const_لار = لار_2;
	اندازه_t لار_اندازه = لار_2.اندازه();
	لكل (خپلکاری wire : module->wires())
	{
		خپلکاری hdlname = hdlname_ډېرپېښ < 0 ? std::ویکټور<std::string>() : wire->get_hdlname_attribute();
		لكل (خپلکاری توکی : hdlname)
			لار_2.ټېل_شا("\\" + توکی);
		إذا (hdlname.اندازه() == 1 && wire->name == لار_2.شا())
			hdlname.clear();
		إذا (!hdlname.تشه())
			callback(const_لار, Witness_Hierarchy_توکی(module, wire), معلومات);
		لار_2.resize(لار_اندازه);
		إذا (hdlname.تشه() || hdlname_ډېرپېښ <= 0) {
			لار_2.ټېل_شا(wire->name);
			callback(const_لار, Witness_Hierarchy_توکی(module, wire), معلومات);
			لار_2.راوباسه_شا();
		}
	}

	لكل (خپلکاری cell : module->cells())
	{
		Module *ماشوم = module->design->module(cell->type);
		إذا (ماشوم == nullptr)
			استمر;

		خپلکاری hdlname = hdlname_ډېرپېښ < 0 ? std::ویکټور<std::string>() : cell->get_hdlname_attribute();
		لكل (خپلکاری توکی : hdlname)
			لار_2.ټېل_شا("\\" + توکی);
		إذا (hdlname.اندازه() == 1 && cell->name == لار_2.شا())
			hdlname.clear();
		إذا (!hdlname.تشه()) {
			D ماشوم_معلومات = callback(const_لار, Witness_Hierarchy_توکی(module, cell), معلومات);
			witness_hierarchy_recursion<D, T>(لار_2, 1, ماشوم, ماشوم_معلومات, callback);
		}
		لار_2.resize(لار_اندازه);
		إذا (hdlname.تشه() || hdlname_ډېرپېښ <= 0) {
			لار_2.ټېل_شا(cell->name);
			D ماشوم_معلومات = callback(const_لار, Witness_Hierarchy_توکی(module, cell), معلومات);
			witness_hierarchy_recursion<D, T>(لار_2, hdlname.تشه() ? hdlname_ډېرپېښ : -1, ماشوم, ماشوم_معلومات, callback);
			لار_2.راوباسه_شا();
		}
	}

	لكل (خپلکاری mem : Mem::get_all_memories(module)) {
		std::ویکټور<std::string> hdlname;

		إذا (hdlname_ډېرپېښ >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		لكل (خپلکاری توکی : hdlname)
			لار_2.ټېل_شا("\\" + توکی);
		إذا (hdlname.اندازه() == 1 && mem.cell->name == لار_2.شا())
			hdlname.clear();
		إذا (!hdlname.تشه()) {
			callback(const_لار, Witness_Hierarchy_توکی(module, &mem), معلومات);
		}
		لار_2.resize(لار_اندازه);

		إذا (hdlname.تشه() || hdlname_ډېرپېښ <= 0) {
			لار_2.ټېل_شا(mem.memid.str());
			callback(const_لار, Witness_Hierarchy_توکی(module, &mem), معلومات);
			لار_2.راوباسه_شا();

			إذا (mem.cell != nullptr && mem.cell->name != mem.memid) {
				لار_2.ټېل_شا(mem.cell->name);
				callback(const_لار, Witness_Hierarchy_توکی(module, &mem), معلومات);
				لار_2.راوباسه_شا();
			}
		}
	}
}

کينډۍ<typename D, typename T>
فراغ witness_hierarchy(RTLIL::Module *module, D معلومات, T callback)
{
	Id_لار لار_2;
	witness_hierarchy_recursion<D, T>(لار_2, 0, module, معلومات, callback);
}

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
