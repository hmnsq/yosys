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

#অন্তর্ভাৱ "kernel/yosys.h"
#অন্তর্ভাৱ "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

সংরচনা IdPath : সার্ৱজনিক std::vector<std::string>
{
	template<typename... T>
	IdPath(T&&... args) : std::vector<std::string>(std::forward<T>(args)...) { }
	IdPath prefix() ধ্রুৱ { ঘূৰাই {begin(), end() - !empty()}; }
	std::string str() ধ্রুৱ;

	bool has_address() ধ্রুৱ { পূর্ণাঙ্ক tmp; ঘূৰাই get_address(tmp); };
	bool get_address(পূর্ণাঙ্ক &addr) ধ্রুৱ;

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুৱ {
		h.eat(static_cast<ধ্রুৱ std::vector<std::string>&&>(*this));
		ঘূৰাই h;
	}
};

সংরচনা WitnessHierarchyItem {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	WitnessHierarchyItem(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

template<typename D, typename T>
শূন্য witness_hierarchy(RTLIL::Module *module, D data, T callback);

template<class T> স্থির std::vector<std::string> witness_path(T *obj) {
	std::vector<std::string> path;
	যদি (obj->name.isPublic()) {
		auto hdlname = obj->get_string_attribute(ID::hdlname);
		বাবে (auto token : split_tokens(hdlname))
			path.ঠেলো_back("\\" + token);
	}
	যদি (path.empty())
		path.ঠেলো_back(obj->name.str());
	ঘূৰাই path;
}

সংরচনা ReadWitness
{
	সংরচনা Clock {
		IdPath path;
		পূর্ণাঙ্ক offset;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	সংরচনা Signal {
		IdPath path;
		পূর্ণাঙ্ক offset;
		পূর্ণাঙ্ক width;
		bool init_only;

		পূর্ণাঙ্ক bits_offset;
	};

	সংরচনা Step {
		std::string bits;
	};

	std::string filename;
	std::vector<Clock> clocks;
	std::vector<Signal> signals;
	std::vector<Step> steps;

	ReadWitness(ধ্রুৱ std::string &filename);

	RTLIL::Const get_bits(পূর্ণাঙ্ক t, পূর্ণাঙ্ক bits_offset, পূর্ণাঙ্ক width) ধ্রুৱ;
};

template<typename D, typename T>
শূন্য witness_hierarchy_recursion(IdPath &path, পূর্ণাঙ্ক hdlname_mode, RTLIL::Module *module, D data, T &callback)
{
	auto ধ্রুৱ &const_path = path;
	size_t path_size = path.size();
	বাবে (auto wire : module->wires())
	{
		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : wire->get_hdlname_attribute();
		বাবে (auto item : hdlname)
			path.ঠেলো_back("\\" + item);
		যদি (hdlname.size() == 1 && wire->name == path.back())
			hdlname.clear();
		যদি (!hdlname.empty())
			callback(const_path, WitnessHierarchyItem(module, wire), data);
		path.resize(path_size);
		যদি (hdlname.empty() || hdlname_mode <= 0) {
			path.ঠেলো_back(wire->name);
			callback(const_path, WitnessHierarchyItem(module, wire), data);
			path.বেৰ_কৰো_back();
		}
	}

	বাবে (auto cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		যদি (child == nullptr)
			অনুৱর্ত;

		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : cell->get_hdlname_attribute();
		বাবে (auto item : hdlname)
			path.ঠেলো_back("\\" + item);
		যদি (hdlname.size() == 1 && cell->name == path.back())
			hdlname.clear();
		যদি (!hdlname.empty()) {
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, 1, child, child_data, callback);
		}
		path.resize(path_size);
		যদি (hdlname.empty() || hdlname_mode <= 0) {
			path.ঠেলো_back(cell->name);
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, hdlname.empty() ? hdlname_mode : -1, child, child_data, callback);
			path.বেৰ_কৰো_back();
		}
	}

	বাবে (auto mem : Mem::get_all_memories(module)) {
		std::vector<std::string> hdlname;

		যদি (hdlname_mode >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		বাবে (auto item : hdlname)
			path.ঠেলো_back("\\" + item);
		যদি (hdlname.size() == 1 && mem.cell->name == path.back())
			hdlname.clear();
		যদি (!hdlname.empty()) {
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
		}
		path.resize(path_size);

		যদি (hdlname.empty() || hdlname_mode <= 0) {
			path.ঠেলো_back(mem.memid.str());
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
			path.বেৰ_কৰো_back();

			যদি (mem.cell != nullptr && mem.cell->name != mem.memid) {
				path.ঠেলো_back(mem.cell->name);
				callback(const_path, WitnessHierarchyItem(module, &mem), data);
				path.বেৰ_কৰো_back();
			}
		}
	}
}

template<typename D, typename T>
শূন্য witness_hierarchy(RTLIL::Module *module, D data, T callback)
{
	IdPath path;
	witness_hierarchy_recursion<D, T>(path, 0, module, data, callback);
}

YOSYS_NAMESPACE_END

#endif
