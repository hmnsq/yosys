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

#اگر_غیر_تعریف_شدہ YW_H
#تعریف_کرو YW_H

#شامل "kernel/yosys.h"
#شامل "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

ساخت Id_راستہ : عوامی std::سمتیہ<std::string>
{
	سانچہ<typename... T>
	Id_راستہ(T&&... دلائل_2) : std::سمتیہ<std::string>(std::forward<T>(دلائل_2)...) { }
	Id_راستہ سابقہ_2() ثابت { واپس {begin(), انتہا() - !خالی_خانے()}; }
	std::string str() ثابت;

	bool has_پتہ() ثابت { صحیح tmp; واپس get_پتہ(tmp); };
	bool get_پتہ(صحیح &addr) ثابت;

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(static_cast<ثابت std::سمتیہ<std::string>&&>(*this));
		واپس h;
	}
};

ساخت Witness_Hierarchy_شے {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	Witness_Hierarchy_شے(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	Witness_Hierarchy_شے(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	Witness_Hierarchy_شے(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

سانچہ<typename D, typename T>
خالی witness_hierarchy(RTLIL::Module *module, D ڈیٹا, T callback);

سانچہ<class T> ساکن std::سمتیہ<std::string> witness_راستہ(T *obj) {
	std::سمتیہ<std::string> راستہ_2;
	اگر (obj->name.is_عوامی()) {
		خودکار hdlname = obj->get_string_attribute(ID::hdlname);
		برائے (خودکار token : بانٹو_علامات(hdlname))
			راستہ_2.ڈالو_عقب("\\" + token);
	}
	اگر (راستہ_2.خالی_خانے())
		راستہ_2.ڈالو_عقب(obj->name.str());
	واپس راستہ_2;
}

ساخت ReadWitness
{
	ساخت Clock {
		Id_راستہ راستہ_2;
		صحیح انحراف;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	ساخت Signal {
		Id_راستہ راستہ_2;
		صحیح انحراف;
		صحیح چوڑائی;
		bool init_only;

		صحیح بٹ_انحراف;
	};

	ساخت قدم_2 {
		std::string بٹ;
	};

	std::string filename;
	std::سمتیہ<Clock> clocks;
	std::سمتیہ<Signal> اشارات;
	std::سمتیہ<قدم_2> اقدام;

	ReadWitness(ثابت std::string &filename);

	RTLIL::Const get_بٹ(صحیح t, صحیح بٹ_انحراف, صحیح چوڑائی) ثابت;
};

سانچہ<typename D, typename T>
خالی witness_hierarchy_recursion(Id_راستہ &راستہ_2, صحیح hdlname_منوال, RTLIL::Module *module, D ڈیٹا, T &callback)
{
	خودکار ثابت &const_راستہ = راستہ_2;
	سائز_t راستہ_سائز = راستہ_2.سائز();
	برائے (خودکار wire : module->wires())
	{
		خودکار hdlname = hdlname_منوال < 0 ? std::سمتیہ<std::string>() : wire->get_hdlname_attribute();
		برائے (خودکار شے_2 : hdlname)
			راستہ_2.ڈالو_عقب("\\" + شے_2);
		اگر (hdlname.سائز() == 1 && wire->name == راستہ_2.عقب())
			hdlname.clear();
		اگر (!hdlname.خالی_خانے())
			callback(const_راستہ, Witness_Hierarchy_شے(module, wire), ڈیٹا);
		راستہ_2.resize(راستہ_سائز);
		اگر (hdlname.خالی_خانے() || hdlname_منوال <= 0) {
			راستہ_2.ڈالو_عقب(wire->name);
			callback(const_راستہ, Witness_Hierarchy_شے(module, wire), ڈیٹا);
			راستہ_2.نکالو_عقب();
		}
	}

	برائے (خودکار cell : module->cells())
	{
		Module *بچہ = module->design->module(cell->type);
		اگر (بچہ == nullptr)
			جاری;

		خودکار hdlname = hdlname_منوال < 0 ? std::سمتیہ<std::string>() : cell->get_hdlname_attribute();
		برائے (خودکار شے_2 : hdlname)
			راستہ_2.ڈالو_عقب("\\" + شے_2);
		اگر (hdlname.سائز() == 1 && cell->name == راستہ_2.عقب())
			hdlname.clear();
		اگر (!hdlname.خالی_خانے()) {
			D بچہ_ڈیٹا = callback(const_راستہ, Witness_Hierarchy_شے(module, cell), ڈیٹا);
			witness_hierarchy_recursion<D, T>(راستہ_2, 1, بچہ, بچہ_ڈیٹا, callback);
		}
		راستہ_2.resize(راستہ_سائز);
		اگر (hdlname.خالی_خانے() || hdlname_منوال <= 0) {
			راستہ_2.ڈالو_عقب(cell->name);
			D بچہ_ڈیٹا = callback(const_راستہ, Witness_Hierarchy_شے(module, cell), ڈیٹا);
			witness_hierarchy_recursion<D, T>(راستہ_2, hdlname.خالی_خانے() ? hdlname_منوال : -1, بچہ, بچہ_ڈیٹا, callback);
			راستہ_2.نکالو_عقب();
		}
	}

	برائے (خودکار mem : Mem::get_all_memories(module)) {
		std::سمتیہ<std::string> hdlname;

		اگر (hdlname_منوال >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		برائے (خودکار شے_2 : hdlname)
			راستہ_2.ڈالو_عقب("\\" + شے_2);
		اگر (hdlname.سائز() == 1 && mem.cell->name == راستہ_2.عقب())
			hdlname.clear();
		اگر (!hdlname.خالی_خانے()) {
			callback(const_راستہ, Witness_Hierarchy_شے(module, &mem), ڈیٹا);
		}
		راستہ_2.resize(راستہ_سائز);

		اگر (hdlname.خالی_خانے() || hdlname_منوال <= 0) {
			راستہ_2.ڈالو_عقب(mem.memid.str());
			callback(const_راستہ, Witness_Hierarchy_شے(module, &mem), ڈیٹا);
			راستہ_2.نکالو_عقب();

			اگر (mem.cell != nullptr && mem.cell->name != mem.memid) {
				راستہ_2.ڈالو_عقب(mem.cell->name);
				callback(const_راستہ, Witness_Hierarchy_شے(module, &mem), ڈیٹا);
				راستہ_2.نکالو_عقب();
			}
		}
	}
}

سانچہ<typename D, typename T>
خالی witness_hierarchy(RTLIL::Module *module, D ڈیٹا, T callback)
{
	Id_راستہ راستہ_2;
	witness_hierarchy_recursion<D, T>(راستہ_2, 0, module, ڈیٹا, callback);
}

YOSYS_NAMESPACE_انتہا

#اگر_ختم
