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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ YW_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YW_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

ਬਣਤਰ Id_ਰਸਤਾ : ਜਨਤਕ std::ਸਦਿਸ਼<std::string>
{
	ਸਾਂਚਾ<typename... T>
	Id_ਰਸਤਾ(T&&... ਦਲੀਲਾਂ_2) : std::ਸਦਿਸ਼<std::string>(std::forward<T>(ਦਲੀਲਾਂ_2)...) { }
	Id_ਰਸਤਾ ਅਗੇਤਰ_2() ਅਚਲ { ਵਾਪਸ {begin(), ਸਿਰਾ() - !ਖਾਲੀ_ਥਾਵਾਂ()}; }
	std::string str() ਅਚਲ;

	bool has_ਪਤਾ() ਅਚਲ { ਪੂਰਨ tmp; ਵਾਪਸ get_ਪਤਾ(tmp); };
	bool get_ਪਤਾ(ਪੂਰਨ &addr) ਅਚਲ;

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
		h.eat(static_cast<ਅਚਲ std::ਸਦਿਸ਼<std::string>&&>(*this));
		ਵਾਪਸ h;
	}
};

ਬਣਤਰ Witness_Hierarchy_ਵਸਤੂ {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	Witness_Hierarchy_ਵਸਤੂ(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	Witness_Hierarchy_ਵਸਤੂ(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	Witness_Hierarchy_ਵਸਤੂ(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

ਸਾਂਚਾ<typename D, typename T>
ਖਾਲੀ witness_hierarchy(RTLIL::Module *module, D ਅੰਕੜੇ, T callback);

ਸਾਂਚਾ<class T> ਸਥਿਰ std::ਸਦਿਸ਼<std::string> witness_ਰਸਤਾ(T *obj) {
	std::ਸਦਿਸ਼<std::string> ਰਸਤਾ_2;
	ਜੇ (obj->name.is_ਜਨਤਕ()) {
		ਸਵੈਚਾਲਿਤ hdlname = obj->get_string_attribute(ID::hdlname);
		ਲਈ (ਸਵੈਚਾਲਿਤ token : ਵੰਡੋ_ਟੋਕਨ(hdlname))
			ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ("\\" + token);
	}
	ਜੇ (ਰਸਤਾ_2.ਖਾਲੀ_ਥਾਵਾਂ())
		ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ(obj->name.str());
	ਵਾਪਸ ਰਸਤਾ_2;
}

ਬਣਤਰ ReadWitness
{
	ਬਣਤਰ Clock {
		Id_ਰਸਤਾ ਰਸਤਾ_2;
		ਪੂਰਨ ਵਿੱਥ;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	ਬਣਤਰ Signal {
		Id_ਰਸਤਾ ਰਸਤਾ_2;
		ਪੂਰਨ ਵਿੱਥ;
		ਪੂਰਨ ਚੌੜਾਈ;
		bool init_only;

		ਪੂਰਨ ਬਿੱਟ_ਵਿੱਥ;
	};

	ਬਣਤਰ ਕਦਮ_2 {
		std::string ਬਿੱਟ;
	};

	std::string filename;
	std::ਸਦਿਸ਼<Clock> clocks;
	std::ਸਦਿਸ਼<Signal> ਸੰਕੇਤ_2;
	std::ਸਦਿਸ਼<ਕਦਮ_2> ਕਦਮਾਂ;

	ReadWitness(ਅਚਲ std::string &filename);

	RTLIL::Const get_ਬਿੱਟ(ਪੂਰਨ t, ਪੂਰਨ ਬਿੱਟ_ਵਿੱਥ, ਪੂਰਨ ਚੌੜਾਈ) ਅਚਲ;
};

ਸਾਂਚਾ<typename D, typename T>
ਖਾਲੀ witness_hierarchy_recursion(Id_ਰਸਤਾ &ਰਸਤਾ_2, ਪੂਰਨ hdlname_ਬਹੁਲਕ, RTLIL::Module *module, D ਅੰਕੜੇ, T &callback)
{
	ਸਵੈਚਾਲਿਤ ਅਚਲ &const_ਰਸਤਾ = ਰਸਤਾ_2;
	ਮਾਪ_t ਰਸਤਾ_ਮਾਪ = ਰਸਤਾ_2.ਮਾਪ();
	ਲਈ (ਸਵੈਚਾਲਿਤ wire : module->wires())
	{
		ਸਵੈਚਾਲਿਤ hdlname = hdlname_ਬਹੁਲਕ < 0 ? std::ਸਦਿਸ਼<std::string>() : wire->get_hdlname_attribute();
		ਲਈ (ਸਵੈਚਾਲਿਤ ਵਸਤੂ_2 : hdlname)
			ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ("\\" + ਵਸਤੂ_2);
		ਜੇ (hdlname.ਮਾਪ() == 1 && wire->name == ਰਸਤਾ_2.ਪਿੱਠ())
			hdlname.clear();
		ਜੇ (!hdlname.ਖਾਲੀ_ਥਾਵਾਂ())
			callback(const_ਰਸਤਾ, Witness_Hierarchy_ਵਸਤੂ(module, wire), ਅੰਕੜੇ);
		ਰਸਤਾ_2.resize(ਰਸਤਾ_ਮਾਪ);
		ਜੇ (hdlname.ਖਾਲੀ_ਥਾਵਾਂ() || hdlname_ਬਹੁਲਕ <= 0) {
			ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ(wire->name);
			callback(const_ਰਸਤਾ, Witness_Hierarchy_ਵਸਤੂ(module, wire), ਅੰਕੜੇ);
			ਰਸਤਾ_2.ਕੱਢੋ_ਪਿੱਠ();
		}
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ cell : module->cells())
	{
		Module *ਬੱਚਾ = module->design->module(cell->type);
		ਜੇ (ਬੱਚਾ == nullptr)
			ਜਾਰੀ;

		ਸਵੈਚਾਲਿਤ hdlname = hdlname_ਬਹੁਲਕ < 0 ? std::ਸਦਿਸ਼<std::string>() : cell->get_hdlname_attribute();
		ਲਈ (ਸਵੈਚਾਲਿਤ ਵਸਤੂ_2 : hdlname)
			ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ("\\" + ਵਸਤੂ_2);
		ਜੇ (hdlname.ਮਾਪ() == 1 && cell->name == ਰਸਤਾ_2.ਪਿੱਠ())
			hdlname.clear();
		ਜੇ (!hdlname.ਖਾਲੀ_ਥਾਵਾਂ()) {
			D ਬੱਚਾ_ਅੰਕੜੇ = callback(const_ਰਸਤਾ, Witness_Hierarchy_ਵਸਤੂ(module, cell), ਅੰਕੜੇ);
			witness_hierarchy_recursion<D, T>(ਰਸਤਾ_2, 1, ਬੱਚਾ, ਬੱਚਾ_ਅੰਕੜੇ, callback);
		}
		ਰਸਤਾ_2.resize(ਰਸਤਾ_ਮਾਪ);
		ਜੇ (hdlname.ਖਾਲੀ_ਥਾਵਾਂ() || hdlname_ਬਹੁਲਕ <= 0) {
			ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ(cell->name);
			D ਬੱਚਾ_ਅੰਕੜੇ = callback(const_ਰਸਤਾ, Witness_Hierarchy_ਵਸਤੂ(module, cell), ਅੰਕੜੇ);
			witness_hierarchy_recursion<D, T>(ਰਸਤਾ_2, hdlname.ਖਾਲੀ_ਥਾਵਾਂ() ? hdlname_ਬਹੁਲਕ : -1, ਬੱਚਾ, ਬੱਚਾ_ਅੰਕੜੇ, callback);
			ਰਸਤਾ_2.ਕੱਢੋ_ਪਿੱਠ();
		}
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ mem : Mem::get_all_memories(module)) {
		std::ਸਦਿਸ਼<std::string> hdlname;

		ਜੇ (hdlname_ਬਹੁਲਕ >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		ਲਈ (ਸਵੈਚਾਲਿਤ ਵਸਤੂ_2 : hdlname)
			ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ("\\" + ਵਸਤੂ_2);
		ਜੇ (hdlname.ਮਾਪ() == 1 && mem.cell->name == ਰਸਤਾ_2.ਪਿੱਠ())
			hdlname.clear();
		ਜੇ (!hdlname.ਖਾਲੀ_ਥਾਵਾਂ()) {
			callback(const_ਰਸਤਾ, Witness_Hierarchy_ਵਸਤੂ(module, &mem), ਅੰਕੜੇ);
		}
		ਰਸਤਾ_2.resize(ਰਸਤਾ_ਮਾਪ);

		ਜੇ (hdlname.ਖਾਲੀ_ਥਾਵਾਂ() || hdlname_ਬਹੁਲਕ <= 0) {
			ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ(mem.memid.str());
			callback(const_ਰਸਤਾ, Witness_Hierarchy_ਵਸਤੂ(module, &mem), ਅੰਕੜੇ);
			ਰਸਤਾ_2.ਕੱਢੋ_ਪਿੱਠ();

			ਜੇ (mem.cell != nullptr && mem.cell->name != mem.memid) {
				ਰਸਤਾ_2.ਧੱਕੋ_ਪਿੱਠ(mem.cell->name);
				callback(const_ਰਸਤਾ, Witness_Hierarchy_ਵਸਤੂ(module, &mem), ਅੰਕੜੇ);
				ਰਸਤਾ_2.ਕੱਢੋ_ਪਿੱਠ();
			}
		}
	}
}

ਸਾਂਚਾ<typename D, typename T>
ਖਾਲੀ witness_hierarchy(RTLIL::Module *module, D ਅੰਕੜੇ, T callback)
{
	Id_ਰਸਤਾ ਰਸਤਾ_2;
	witness_hierarchy_recursion<D, T>(ਰਸਤਾ_2, 0, module, ਅੰਕੜੇ, callback);
}

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
