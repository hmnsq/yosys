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

#شامل "kernel/yosys.h"
#شامل "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

ساختار IdPath : عمومی std::vector<std::string>
{
	template<typename... T>
	IdPath(T&&... args) : std::vector<std::string>(std::forward<T>(args)...) { }
	IdPath prefix() ثابت { بازگشت {begin(), end() - !empty()}; }
	std::string str() ثابت;

	bool has_address() ثابت { صحیح tmp; بازگشت get_address(tmp); };
	bool get_address(صحیح &addr) ثابت;

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(static_cast<ثابت std::vector<std::string>&&>(*this));
		بازگشت h;
	}
};

ساختار WitnessHierarchyItem {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	WitnessHierarchyItem(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

template<typename D, typename T>
تهی witness_hierarchy(RTLIL::Module *module, D data, T callback);

template<class T> ایستا std::vector<std::string> witness_path(T *obj) {
	std::vector<std::string> path;
	اگر (obj->name.isPublic()) {
		auto hdlname = obj->get_string_attribute(ID::hdlname);
		برای (auto token : split_tokens(hdlname))
			path.افزودن_back("\\" + token);
	}
	اگر (path.empty())
		path.افزودن_back(obj->name.str());
	بازگشت path;
}

ساختار ReadWitness
{
	ساختار Clock {
		IdPath path;
		صحیح offset;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	ساختار Signal {
		IdPath path;
		صحیح offset;
		صحیح width;
		bool init_only;

		صحیح bits_offset;
	};

	ساختار Step {
		std::string bits;
	};

	std::string filename;
	std::vector<Clock> clocks;
	std::vector<Signal> signals;
	std::vector<Step> steps;

	ReadWitness(ثابت std::string &filename);

	RTLIL::Const get_bits(صحیح t, صحیح bits_offset, صحیح width) ثابت;
};

template<typename D, typename T>
تهی witness_hierarchy_recursion(IdPath &path, صحیح hdlname_mode, RTLIL::Module *module, D data, T &callback)
{
	auto ثابت &const_path = path;
	size_t path_size = path.size();
	برای (auto wire : module->wires())
	{
		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : wire->get_hdlname_attribute();
		برای (auto item : hdlname)
			path.افزودن_back("\\" + item);
		اگر (hdlname.size() == 1 && wire->name == path.back())
			hdlname.clear();
		اگر (!hdlname.empty())
			callback(const_path, WitnessHierarchyItem(module, wire), data);
		path.resize(path_size);
		اگر (hdlname.empty() || hdlname_mode <= 0) {
			path.افزودن_back(wire->name);
			callback(const_path, WitnessHierarchyItem(module, wire), data);
			path.برداشت_back();
		}
	}

	برای (auto cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		اگر (child == nullptr)
			ادامه;

		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : cell->get_hdlname_attribute();
		برای (auto item : hdlname)
			path.افزودن_back("\\" + item);
		اگر (hdlname.size() == 1 && cell->name == path.back())
			hdlname.clear();
		اگر (!hdlname.empty()) {
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, 1, child, child_data, callback);
		}
		path.resize(path_size);
		اگر (hdlname.empty() || hdlname_mode <= 0) {
			path.افزودن_back(cell->name);
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, hdlname.empty() ? hdlname_mode : -1, child, child_data, callback);
			path.برداشت_back();
		}
	}

	برای (auto mem : Mem::get_all_memories(module)) {
		std::vector<std::string> hdlname;

		اگر (hdlname_mode >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		برای (auto item : hdlname)
			path.افزودن_back("\\" + item);
		اگر (hdlname.size() == 1 && mem.cell->name == path.back())
			hdlname.clear();
		اگر (!hdlname.empty()) {
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
		}
		path.resize(path_size);

		اگر (hdlname.empty() || hdlname_mode <= 0) {
			path.افزودن_back(mem.memid.str());
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
			path.برداشت_back();

			اگر (mem.cell != nullptr && mem.cell->name != mem.memid) {
				path.افزودن_back(mem.cell->name);
				callback(const_path, WitnessHierarchyItem(module, &mem), data);
				path.برداشت_back();
			}
		}
	}
}

template<typename D, typename T>
تهی witness_hierarchy(RTLIL::Module *module, D data, T callback)
{
	IdPath path;
	witness_hierarchy_recursion<D, T>(path, 0, module, data, callback);
}

YOSYS_NAMESPACE_END

#endif
