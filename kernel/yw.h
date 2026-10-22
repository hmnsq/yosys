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

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

संरचना IdPath : सार्वजनिक std::vector<std::string>
{
	template<typename... T>
	IdPath(T&&... args) : std::vector<std::string>(std::forward<T>(args)...) { }
	IdPath prefix() ध्रुव { प्रत्यागम {begin(), end() - !empty()}; }
	std::string str() ध्रुव;

	bool has_address() ध्रुव { पूर्णाङ्क tmp; प्रत्यागम get_address(tmp); };
	bool get_address(पूर्णाङ्क &addr) ध्रुव;

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
		h.eat(static_cast<ध्रुव std::vector<std::string>&&>(*this));
		प्रत्यागम h;
	}
};

संरचना WitnessHierarchyItem {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	WitnessHierarchyItem(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

template<typename D, typename T>
शून्य witness_hierarchy(RTLIL::Module *module, D data, T callback);

template<class T> स्थिर std::vector<std::string> witness_path(T *obj) {
	std::vector<std::string> path;
	यदि (obj->name.isPublic()) {
		auto hdlname = obj->get_string_attribute(ID::hdlname);
		प्रति (auto token : split_tokens(hdlname))
			path.ढकला_back("\\" + token);
	}
	यदि (path.empty())
		path.ढकला_back(obj->name.str());
	प्रत्यागम path;
}

संरचना ReadWitness
{
	संरचना Clock {
		IdPath path;
		पूर्णाङ्क offset;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	संरचना Signal {
		IdPath path;
		पूर्णाङ्क offset;
		पूर्णाङ्क width;
		bool init_only;

		पूर्णाङ्क bits_offset;
	};

	संरचना Step {
		std::string bits;
	};

	std::string filename;
	std::vector<Clock> clocks;
	std::vector<Signal> signals;
	std::vector<Step> steps;

	ReadWitness(ध्रुव std::string &filename);

	RTLIL::Const get_bits(पूर्णाङ्क t, पूर्णाङ्क bits_offset, पूर्णाङ्क width) ध्रुव;
};

template<typename D, typename T>
शून्य witness_hierarchy_recursion(IdPath &path, पूर्णाङ्क hdlname_mode, RTLIL::Module *module, D data, T &callback)
{
	auto ध्रुव &const_path = path;
	size_t path_size = path.size();
	प्रति (auto wire : module->wires())
	{
		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : wire->get_hdlname_attribute();
		प्रति (auto item : hdlname)
			path.ढकला_back("\\" + item);
		यदि (hdlname.size() == 1 && wire->name == path.back())
			hdlname.clear();
		यदि (!hdlname.empty())
			callback(const_path, WitnessHierarchyItem(module, wire), data);
		path.resize(path_size);
		यदि (hdlname.empty() || hdlname_mode <= 0) {
			path.ढकला_back(wire->name);
			callback(const_path, WitnessHierarchyItem(module, wire), data);
			path.काढा_back();
		}
	}

	प्रति (auto cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		यदि (child == nullptr)
			अनुवर्त;

		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : cell->get_hdlname_attribute();
		प्रति (auto item : hdlname)
			path.ढकला_back("\\" + item);
		यदि (hdlname.size() == 1 && cell->name == path.back())
			hdlname.clear();
		यदि (!hdlname.empty()) {
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, 1, child, child_data, callback);
		}
		path.resize(path_size);
		यदि (hdlname.empty() || hdlname_mode <= 0) {
			path.ढकला_back(cell->name);
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, hdlname.empty() ? hdlname_mode : -1, child, child_data, callback);
			path.काढा_back();
		}
	}

	प्रति (auto mem : Mem::get_all_memories(module)) {
		std::vector<std::string> hdlname;

		यदि (hdlname_mode >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		प्रति (auto item : hdlname)
			path.ढकला_back("\\" + item);
		यदि (hdlname.size() == 1 && mem.cell->name == path.back())
			hdlname.clear();
		यदि (!hdlname.empty()) {
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
		}
		path.resize(path_size);

		यदि (hdlname.empty() || hdlname_mode <= 0) {
			path.ढकला_back(mem.memid.str());
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
			path.काढा_back();

			यदि (mem.cell != nullptr && mem.cell->name != mem.memid) {
				path.ढकला_back(mem.cell->name);
				callback(const_path, WitnessHierarchyItem(module, &mem), data);
				path.काढा_back();
			}
		}
	}
}

template<typename D, typename T>
शून्य witness_hierarchy(RTLIL::Module *module, D data, T callback)
{
	IdPath path;
	witness_hierarchy_recursion<D, T>(path, 0, module, data, callback);
}

YOSYS_NAMESPACE_END

#endif
