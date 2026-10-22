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

#ضمن "kernel/yosys.h"
#ضمن "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

بنية IdPath : عام std::vector<std::string>
{
	template<typename... T>
	IdPath(T&&... args) : std::vector<std::string>(std::forward<T>(args)...) { }
	IdPath prefix() ثابت { إرجاع {begin(), end() - !empty()}; }
	std::string str() ثابت;

	bool has_address() ثابت { صحيح tmp; إرجاع get_address(tmp); };
	bool get_address(صحيح &addr) ثابت;

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		h.eat(static_cast<ثابت std::vector<std::string>&&>(*this));
		إرجاع h;
	}
};

بنية WitnessHierarchyItem {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	WitnessHierarchyItem(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

template<typename D, typename T>
فراغ witness_hierarchy(RTLIL::Module *module, D data, T callback);

template<class T> ساكن std::vector<std::string> witness_path(T *obj) {
	std::vector<std::string> path;
	إذا (obj->name.isPublic()) {
		auto hdlname = obj->get_string_attribute(ID::hdlname);
		لكل (auto token : split_tokens(hdlname))
			path.ادفع_back("\\" + token);
	}
	إذا (path.empty())
		path.ادفع_back(obj->name.str());
	إرجاع path;
}

بنية ReadWitness
{
	بنية Clock {
		IdPath path;
		صحيح offset;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	بنية Signal {
		IdPath path;
		صحيح offset;
		صحيح width;
		bool init_only;

		صحيح bits_offset;
	};

	بنية Step {
		std::string bits;
	};

	std::string filename;
	std::vector<Clock> clocks;
	std::vector<Signal> signals;
	std::vector<Step> steps;

	ReadWitness(ثابت std::string &filename);

	RTLIL::Const get_bits(صحيح t, صحيح bits_offset, صحيح width) ثابت;
};

template<typename D, typename T>
فراغ witness_hierarchy_recursion(IdPath &path, صحيح hdlname_mode, RTLIL::Module *module, D data, T &callback)
{
	auto ثابت &const_path = path;
	size_t path_size = path.size();
	لكل (auto wire : module->wires())
	{
		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : wire->get_hdlname_attribute();
		لكل (auto item : hdlname)
			path.ادفع_back("\\" + item);
		إذا (hdlname.size() == 1 && wire->name == path.back())
			hdlname.clear();
		إذا (!hdlname.empty())
			callback(const_path, WitnessHierarchyItem(module, wire), data);
		path.resize(path_size);
		إذا (hdlname.empty() || hdlname_mode <= 0) {
			path.ادفع_back(wire->name);
			callback(const_path, WitnessHierarchyItem(module, wire), data);
			path.اسحب_back();
		}
	}

	لكل (auto cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		إذا (child == nullptr)
			استمر;

		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : cell->get_hdlname_attribute();
		لكل (auto item : hdlname)
			path.ادفع_back("\\" + item);
		إذا (hdlname.size() == 1 && cell->name == path.back())
			hdlname.clear();
		إذا (!hdlname.empty()) {
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, 1, child, child_data, callback);
		}
		path.resize(path_size);
		إذا (hdlname.empty() || hdlname_mode <= 0) {
			path.ادفع_back(cell->name);
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, hdlname.empty() ? hdlname_mode : -1, child, child_data, callback);
			path.اسحب_back();
		}
	}

	لكل (auto mem : Mem::get_all_memories(module)) {
		std::vector<std::string> hdlname;

		إذا (hdlname_mode >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		لكل (auto item : hdlname)
			path.ادفع_back("\\" + item);
		إذا (hdlname.size() == 1 && mem.cell->name == path.back())
			hdlname.clear();
		إذا (!hdlname.empty()) {
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
		}
		path.resize(path_size);

		إذا (hdlname.empty() || hdlname_mode <= 0) {
			path.ادفع_back(mem.memid.str());
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
			path.اسحب_back();

			إذا (mem.cell != nullptr && mem.cell->name != mem.memid) {
				path.ادفع_back(mem.cell->name);
				callback(const_path, WitnessHierarchyItem(module, &mem), data);
				path.اسحب_back();
			}
		}
	}
}

template<typename D, typename T>
فراغ witness_hierarchy(RTLIL::Module *module, D data, T callback)
{
	IdPath path;
	witness_hierarchy_recursion<D, T>(path, 0, module, data, callback);
}

YOSYS_NAMESPACE_END

#endif
