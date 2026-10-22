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

#ifndef YW_H
#define YW_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

ಸಂರಚನಾ IdPath : ಸಾರ್ವಜನಿಕ std::vector<std::string>
{
	template<typename... T>
	IdPath(T&&... args) : std::vector<std::string>(std::forward<T>(args)...) { }
	IdPath prefix() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ {begin(), end() - !empty()}; }
	std::string str() ಧ್ರುವ;

	bool has_address() ಧ್ರುವ { ಪೂರ್ಣಾಙ್ಕ tmp; ಪ್ರತ್ಯಾಗಮ get_address(tmp); };
	bool get_address(ಪೂರ್ಣಾಙ್ಕ &addr) ಧ್ರುವ;

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
		h.eat(static_cast<ಧ್ರುವ std::vector<std::string>&&>(*this));
		ಪ್ರತ್ಯಾಗಮ h;
	}
};

ಸಂರಚನಾ WitnessHierarchyItem {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	WitnessHierarchyItem(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

template<typename D, typename T>
ಶೂನ್ಯ witness_hierarchy(RTLIL::Module *module, D data, T callback);

template<class T> ಸ್ಥಿರ std::vector<std::string> witness_path(T *obj) {
	std::vector<std::string> path;
	ಯದಿ (obj->name.isPublic()) {
		auto hdlname = obj->get_string_attribute(ID::hdlname);
		ಪ್ರತಿ (auto token : split_tokens(hdlname))
			path.ತಳ್ಳು_back("\\" + token);
	}
	ಯದಿ (path.empty())
		path.ತಳ್ಳು_back(obj->name.str());
	ಪ್ರತ್ಯಾಗಮ path;
}

ಸಂರಚನಾ ReadWitness
{
	ಸಂರಚನಾ Clock {
		IdPath path;
		ಪೂರ್ಣಾಙ್ಕ offset;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	ಸಂರಚನಾ Signal {
		IdPath path;
		ಪೂರ್ಣಾಙ್ಕ offset;
		ಪೂರ್ಣಾಙ್ಕ width;
		bool init_only;

		ಪೂರ್ಣಾಙ್ಕ bits_offset;
	};

	ಸಂರಚನಾ Step {
		std::string bits;
	};

	std::string filename;
	std::vector<Clock> clocks;
	std::vector<Signal> signals;
	std::vector<Step> steps;

	ReadWitness(ಧ್ರುವ std::string &filename);

	RTLIL::Const get_bits(ಪೂರ್ಣಾಙ್ಕ t, ಪೂರ್ಣಾಙ್ಕ bits_offset, ಪೂರ್ಣಾಙ್ಕ width) ಧ್ರುವ;
};

template<typename D, typename T>
ಶೂನ್ಯ witness_hierarchy_recursion(IdPath &path, ಪೂರ್ಣಾಙ್ಕ hdlname_mode, RTLIL::Module *module, D data, T &callback)
{
	auto ಧ್ರುವ &const_path = path;
	size_t path_size = path.size();
	ಪ್ರತಿ (auto wire : module->wires())
	{
		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : wire->get_hdlname_attribute();
		ಪ್ರತಿ (auto item : hdlname)
			path.ತಳ್ಳು_back("\\" + item);
		ಯದಿ (hdlname.size() == 1 && wire->name == path.back())
			hdlname.clear();
		ಯದಿ (!hdlname.empty())
			callback(const_path, WitnessHierarchyItem(module, wire), data);
		path.resize(path_size);
		ಯದಿ (hdlname.empty() || hdlname_mode <= 0) {
			path.ತಳ್ಳು_back(wire->name);
			callback(const_path, WitnessHierarchyItem(module, wire), data);
			path.ತೆಗೆ_back();
		}
	}

	ಪ್ರತಿ (auto cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		ಯದಿ (child == nullptr)
			ಅನುವರ್ತ;

		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : cell->get_hdlname_attribute();
		ಪ್ರತಿ (auto item : hdlname)
			path.ತಳ್ಳು_back("\\" + item);
		ಯದಿ (hdlname.size() == 1 && cell->name == path.back())
			hdlname.clear();
		ಯದಿ (!hdlname.empty()) {
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, 1, child, child_data, callback);
		}
		path.resize(path_size);
		ಯದಿ (hdlname.empty() || hdlname_mode <= 0) {
			path.ತಳ್ಳು_back(cell->name);
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, hdlname.empty() ? hdlname_mode : -1, child, child_data, callback);
			path.ತೆಗೆ_back();
		}
	}

	ಪ್ರತಿ (auto mem : Mem::get_all_memories(module)) {
		std::vector<std::string> hdlname;

		ಯದಿ (hdlname_mode >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		ಪ್ರತಿ (auto item : hdlname)
			path.ತಳ್ಳು_back("\\" + item);
		ಯದಿ (hdlname.size() == 1 && mem.cell->name == path.back())
			hdlname.clear();
		ಯದಿ (!hdlname.empty()) {
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
		}
		path.resize(path_size);

		ಯದಿ (hdlname.empty() || hdlname_mode <= 0) {
			path.ತಳ್ಳು_back(mem.memid.str());
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
			path.ತೆಗೆ_back();

			ಯದಿ (mem.cell != nullptr && mem.cell->name != mem.memid) {
				path.ತಳ್ಳು_back(mem.cell->name);
				callback(const_path, WitnessHierarchyItem(module, &mem), data);
				path.ತೆಗೆ_back();
			}
		}
	}
}

template<typename D, typename T>
ಶೂನ್ಯ witness_hierarchy(RTLIL::Module *module, D data, T callback)
{
	IdPath path;
	witness_hierarchy_recursion<D, T>(path, 0, module, data, callback);
}

YOSYS_NAMESPACE_END

#endif
