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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ YW_H
#ವ್ಯಾಖ್ಯಾನಿಸು YW_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

ರಚನೆ Id_ಮಾರ್ಗ : ಸಾರ್ವಜನಿಕ std::ಸದಿಶ<std::string>
{
	ಮಾದರಿ<typename... T>
	Id_ಮಾರ್ಗ(T&&... ವಾದಗಳು_2) : std::ಸದಿಶ<std::string>(std::forward<T>(ವಾದಗಳು_2)...) { }
	Id_ಮಾರ್ಗ ಪೂರ್ವಪ್ರತ್ಯಯ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ {begin(), ಕೊನೆ() - !ಖಾಲಿಗಳು()}; }
	std::string str() ಸ್ಥಿರಾಂಕ;

	bool has_ವಿಳಾಸ() ಸ್ಥಿರಾಂಕ { ಪೂರ್ಣಾಂಕ tmp; ಹಿಂತಿರುಗಿ get_ವಿಳಾಸ(tmp); };
	bool get_ವಿಳಾಸ(ಪೂರ್ಣಾಂಕ &addr) ಸ್ಥಿರಾಂಕ;

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
		h.eat(static_cast<ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<std::string>&&>(*this));
		ಹಿಂತಿರುಗಿ h;
	}
};

ರಚನೆ Witness_Hierarchy_ಅಂಶ {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	Witness_Hierarchy_ಅಂಶ(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	Witness_Hierarchy_ಅಂಶ(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	Witness_Hierarchy_ಅಂಶ(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

ಮಾದರಿ<typename D, typename T>
ಶೂನ್ಯ witness_hierarchy(RTLIL::Module *module, D ದತ್ತಾಂಶ, T callback);

ಮಾದರಿ<class T> ಸ್ಥಿರ std::ಸದಿಶ<std::string> witness_ಮಾರ್ಗ(T *obj) {
	std::ಸದಿಶ<std::string> ಮಾರ್ಗ_2;
	ಆದರೆ (obj->name.is_ಬಹಿರಂಗ()) {
		ಸ್ವಯಂಚಾಲಿತ hdlname = obj->get_string_attribute(ID::hdlname);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ token : ವಿಭಜಿಸು_ಟೋಕನುಗಳು(hdlname))
			ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ("\\" + token);
	}
	ಆದರೆ (ಮಾರ್ಗ_2.ಖಾಲಿಗಳು())
		ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ(obj->name.str());
	ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_2;
}

ರಚನೆ ReadWitness
{
	ರಚನೆ Clock {
		Id_ಮಾರ್ಗ ಮಾರ್ಗ_2;
		ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	ರಚನೆ Signal {
		Id_ಮಾರ್ಗ ಮಾರ್ಗ_2;
		ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;
		ಪೂರ್ಣಾಂಕ ಅಗಲ;
		bool init_only;

		ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು_ಸ್ಥಾನಾಂತರ;
	};

	ರಚನೆ ಹೆಜ್ಜೆ_2 {
		std::string ಬಿಟ್ಟುಗಳು;
	};

	std::string filename;
	std::ಸದಿಶ<Clock> clocks;
	std::ಸದಿಶ<Signal> ಸಂಕೇತಗಳು;
	std::ಸದಿಶ<ಹೆಜ್ಜೆ_2> ಹೆಜ್ಜೆಗಳು;

	ReadWitness(ಸ್ಥಿರಾಂಕ std::string &filename);

	RTLIL::Const get_ಬಿಟ್ಟುಗಳು(ಪೂರ್ಣಾಂಕ t, ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಅಗಲ) ಸ್ಥಿರಾಂಕ;
};

ಮಾದರಿ<typename D, typename T>
ಶೂನ್ಯ witness_hierarchy_recursion(Id_ಮಾರ್ಗ &ಮಾರ್ಗ_2, ಪೂರ್ಣಾಂಕ hdlname_ಬಹುಲಕ, RTLIL::Module *module, D ದತ್ತಾಂಶ, T &callback)
{
	ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &const_ಮಾರ್ಗ = ಮಾರ್ಗ_2;
	ಗಾತ್ರ_t ಮಾರ್ಗ_ಗಾತ್ರ = ಮಾರ್ಗ_2.ಗಾತ್ರ_2();
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : module->wires())
	{
		ಸ್ವಯಂಚಾಲಿತ hdlname = hdlname_ಬಹುಲಕ < 0 ? std::ಸದಿಶ<std::string>() : wire->get_hdlname_attribute();
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಅಂಶ : hdlname)
			ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ("\\" + ಅಂಶ);
		ಆದರೆ (hdlname.ಗಾತ್ರ_2() == 1 && wire->name == ಮಾರ್ಗ_2.ಹಿಂಭಾಗ())
			hdlname.clear();
		ಆದರೆ (!hdlname.ಖಾಲಿಗಳು())
			callback(const_ಮಾರ್ಗ, Witness_Hierarchy_ಅಂಶ(module, wire), ದತ್ತಾಂಶ);
		ಮಾರ್ಗ_2.resize(ಮಾರ್ಗ_ಗಾತ್ರ);
		ಆದರೆ (hdlname.ಖಾಲಿಗಳು() || hdlname_ಬಹುಲಕ <= 0) {
			ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ(wire->name);
			callback(const_ಮಾರ್ಗ, Witness_Hierarchy_ಅಂಶ(module, wire), ದತ್ತಾಂಶ);
			ಮಾರ್ಗ_2.ತೆಗೆ_ಹಿಂಭಾಗ();
		}
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : module->cells())
	{
		Module *ಮಗು = module->design->module(cell->type);
		ಆದರೆ (ಮಗು == nullptr)
			ಮುಂದುವರಿ;

		ಸ್ವಯಂಚಾಲಿತ hdlname = hdlname_ಬಹುಲಕ < 0 ? std::ಸದಿಶ<std::string>() : cell->get_hdlname_attribute();
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಅಂಶ : hdlname)
			ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ("\\" + ಅಂಶ);
		ಆದರೆ (hdlname.ಗಾತ್ರ_2() == 1 && cell->name == ಮಾರ್ಗ_2.ಹಿಂಭಾಗ())
			hdlname.clear();
		ಆದರೆ (!hdlname.ಖಾಲಿಗಳು()) {
			D ಮಗು_ದತ್ತಾಂಶ = callback(const_ಮಾರ್ಗ, Witness_Hierarchy_ಅಂಶ(module, cell), ದತ್ತಾಂಶ);
			witness_hierarchy_recursion<D, T>(ಮಾರ್ಗ_2, 1, ಮಗು, ಮಗು_ದತ್ತಾಂಶ, callback);
		}
		ಮಾರ್ಗ_2.resize(ಮಾರ್ಗ_ಗಾತ್ರ);
		ಆದರೆ (hdlname.ಖಾಲಿಗಳು() || hdlname_ಬಹುಲಕ <= 0) {
			ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ(cell->name);
			D ಮಗು_ದತ್ತಾಂಶ = callback(const_ಮಾರ್ಗ, Witness_Hierarchy_ಅಂಶ(module, cell), ದತ್ತಾಂಶ);
			witness_hierarchy_recursion<D, T>(ಮಾರ್ಗ_2, hdlname.ಖಾಲಿಗಳು() ? hdlname_ಬಹುಲಕ : -1, ಮಗು, ಮಗು_ದತ್ತಾಂಶ, callback);
			ಮಾರ್ಗ_2.ತೆಗೆ_ಹಿಂಭಾಗ();
		}
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mem : Mem::get_all_memories(module)) {
		std::ಸದಿಶ<std::string> hdlname;

		ಆದರೆ (hdlname_ಬಹುಲಕ >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಅಂಶ : hdlname)
			ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ("\\" + ಅಂಶ);
		ಆದರೆ (hdlname.ಗಾತ್ರ_2() == 1 && mem.cell->name == ಮಾರ್ಗ_2.ಹಿಂಭಾಗ())
			hdlname.clear();
		ಆದರೆ (!hdlname.ಖಾಲಿಗಳು()) {
			callback(const_ಮಾರ್ಗ, Witness_Hierarchy_ಅಂಶ(module, &mem), ದತ್ತಾಂಶ);
		}
		ಮಾರ್ಗ_2.resize(ಮಾರ್ಗ_ಗಾತ್ರ);

		ಆದರೆ (hdlname.ಖಾಲಿಗಳು() || hdlname_ಬಹುಲಕ <= 0) {
			ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ(mem.memid.str());
			callback(const_ಮಾರ್ಗ, Witness_Hierarchy_ಅಂಶ(module, &mem), ದತ್ತಾಂಶ);
			ಮಾರ್ಗ_2.ತೆಗೆ_ಹಿಂಭಾಗ();

			ಆದರೆ (mem.cell != nullptr && mem.cell->name != mem.memid) {
				ಮಾರ್ಗ_2.ತಳ್ಳು_ಹಿಂಭಾಗ(mem.cell->name);
				callback(const_ಮಾರ್ಗ, Witness_Hierarchy_ಅಂಶ(module, &mem), ದತ್ತಾಂಶ);
				ಮಾರ್ಗ_2.ತೆಗೆ_ಹಿಂಭಾಗ();
			}
		}
	}
}

ಮಾದರಿ<typename D, typename T>
ಶೂನ್ಯ witness_hierarchy(RTLIL::Module *module, D ದತ್ತಾಂಶ, T callback)
{
	Id_ಮಾರ್ಗ ಮಾರ್ಗ_2;
	witness_hierarchy_recursion<D, T>(ಮಾರ್ಗ_2, 0, module, ದತ್ತಾಂಶ, callback);
}

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
