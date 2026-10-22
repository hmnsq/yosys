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

#অন্তর্ভাব "kernel/yosys.h"
#অন্তর্ভাব "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

কাঠামো IdPath : সর্বজনীন std::vector<std::string>
{
	template<typename... T>
	IdPath(T&&... args) : std::vector<std::string>(std::forward<T>(args)...) { }
	IdPath prefix() ধ্রুবক { ফেরত {begin(), end() - !empty()}; }
	std::string str() ধ্রুবক;

	bool has_address() ধ্রুবক { পূর্ণসংখ্যা tmp; ফেরত get_address(tmp); };
	bool get_address(পূর্ণসংখ্যা &addr) ধ্রুবক;

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক {
		h.eat(static_cast<ধ্রুবক std::vector<std::string>&&>(*this));
		ফেরত h;
	}
};

কাঠামো WitnessHierarchyItem {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	WitnessHierarchyItem(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

template<typename D, typename T>
শূন্য witness_hierarchy(RTLIL::Module *module, D তথ্য, T callback);

template<class T> স্থির std::vector<std::string> witness_path(T *obj) {
	std::vector<std::string> path;
	যদি (obj->name.isPublic()) {
		auto hdlname = obj->get_string_attribute(ID::hdlname);
		জন্য (auto token : split_tokens(hdlname))
			path.ঠেলো_back("\\" + token);
	}
	যদি (path.empty())
		path.ঠেলো_back(obj->name.str());
	ফেরত path;
}

কাঠামো ReadWitness
{
	কাঠামো Clock {
		IdPath path;
		পূর্ণসংখ্যা offset;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	কাঠামো Signal {
		IdPath path;
		পূর্ণসংখ্যা offset;
		পূর্ণসংখ্যা width;
		bool init_only;

		পূর্ণসংখ্যা bits_offset;
	};

	কাঠামো Step {
		std::string bits;
	};

	std::string filename;
	std::vector<Clock> clocks;
	std::vector<Signal> signals;
	std::vector<Step> steps;

	ReadWitness(ধ্রুবক std::string &filename);

	RTLIL::Const get_bits(পূর্ণসংখ্যা t, পূর্ণসংখ্যা bits_offset, পূর্ণসংখ্যা width) ধ্রুবক;
};

template<typename D, typename T>
শূন্য witness_hierarchy_recursion(IdPath &path, পূর্ণসংখ্যা hdlname_mode, RTLIL::Module *module, D তথ্য, T &callback)
{
	auto ধ্রুবক &const_path = path;
	আকার_t path_আকার = path.আকার_2();
	জন্য (auto wire : module->wires())
	{
		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : wire->get_hdlname_attribute();
		জন্য (auto item : hdlname)
			path.ঠেলো_back("\\" + item);
		যদি (hdlname.আকার_2() == 1 && wire->name == path.back())
			hdlname.clear();
		যদি (!hdlname.empty())
			callback(const_path, WitnessHierarchyItem(module, wire), তথ্য);
		path.resize(path_আকার);
		যদি (hdlname.empty() || hdlname_mode <= 0) {
			path.ঠেলো_back(wire->name);
			callback(const_path, WitnessHierarchyItem(module, wire), তথ্য);
			path.বের_করো_back();
		}
	}

	জন্য (auto cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		যদি (child == nullptr)
			চালু;

		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : cell->get_hdlname_attribute();
		জন্য (auto item : hdlname)
			path.ঠেলো_back("\\" + item);
		যদি (hdlname.আকার_2() == 1 && cell->name == path.back())
			hdlname.clear();
		যদি (!hdlname.empty()) {
			D child_তথ্য = callback(const_path, WitnessHierarchyItem(module, cell), তথ্য);
			witness_hierarchy_recursion<D, T>(path, 1, child, child_তথ্য, callback);
		}
		path.resize(path_আকার);
		যদি (hdlname.empty() || hdlname_mode <= 0) {
			path.ঠেলো_back(cell->name);
			D child_তথ্য = callback(const_path, WitnessHierarchyItem(module, cell), তথ্য);
			witness_hierarchy_recursion<D, T>(path, hdlname.empty() ? hdlname_mode : -1, child, child_তথ্য, callback);
			path.বের_করো_back();
		}
	}

	জন্য (auto mem : Mem::get_all_memories(module)) {
		std::vector<std::string> hdlname;

		যদি (hdlname_mode >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		জন্য (auto item : hdlname)
			path.ঠেলো_back("\\" + item);
		যদি (hdlname.আকার_2() == 1 && mem.cell->name == path.back())
			hdlname.clear();
		যদি (!hdlname.empty()) {
			callback(const_path, WitnessHierarchyItem(module, &mem), তথ্য);
		}
		path.resize(path_আকার);

		যদি (hdlname.empty() || hdlname_mode <= 0) {
			path.ঠেলো_back(mem.memid.str());
			callback(const_path, WitnessHierarchyItem(module, &mem), তথ্য);
			path.বের_করো_back();

			যদি (mem.cell != nullptr && mem.cell->name != mem.memid) {
				path.ঠেলো_back(mem.cell->name);
				callback(const_path, WitnessHierarchyItem(module, &mem), তথ্য);
				path.বের_করো_back();
			}
		}
	}
}

template<typename D, typename T>
শূন্য witness_hierarchy(RTLIL::Module *module, D তথ্য, T callback)
{
	IdPath path;
	witness_hierarchy_recursion<D, T>(path, 0, module, তথ্য, callback);
}

YOSYS_NAMESPACE_END

#endif
