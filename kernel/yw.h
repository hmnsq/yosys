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
	ढाँचा<typename... T>
	IdPath(T&&... args) : std::vector<std::string>(std::forward<T>(args)...) { }
	IdPath prefix() स्थिरांक { परत {begin(), end() - !empty()}; }
	std::string str() स्थिरांक;

	bool has_address() स्थिरांक { पूर्णांक tmp; परत get_address(tmp); };
	bool get_address(पूर्णांक &addr) स्थिरांक;

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक {
		h.eat(static_cast<स्थिरांक std::vector<std::string>&&>(*this));
		परत h;
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

ढाँचा<typename D, typename T>
शून्य witness_hierarchy(RTLIL::Module *module, D data, T callback);

ढाँचा<class T> स्थिर std::vector<std::string> witness_path(T *obj) {
	std::vector<std::string> path;
	जर (obj->name.isPublic()) {
		स्वतः hdlname = obj->get_string_attribute(ID::hdlname);
		साठी (स्वतः token : split_tokens(hdlname))
			path.ढकला_back("\\" + token);
	}
	जर (path.empty())
		path.ढकला_back(obj->name.str());
	परत path;
}

संरचना ReadWitness
{
	संरचना Clock {
		IdPath path;
		पूर्णांक offset;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	संरचना Signal {
		IdPath path;
		पूर्णांक offset;
		पूर्णांक width;
		bool init_only;

		पूर्णांक bits_offset;
	};

	संरचना Step {
		std::string bits;
	};

	std::string filename;
	std::vector<Clock> clocks;
	std::vector<Signal> signals;
	std::vector<Step> steps;

	ReadWitness(स्थिरांक std::string &filename);

	RTLIL::Const get_bits(पूर्णांक t, पूर्णांक bits_offset, पूर्णांक width) स्थिरांक;
};

ढाँचा<typename D, typename T>
शून्य witness_hierarchy_recursion(IdPath &path, पूर्णांक hdlname_mode, RTLIL::Module *module, D data, T &callback)
{
	स्वतः स्थिरांक &const_path = path;
	माप_प्रकार path_size = path.size();
	साठी (स्वतः wire : module->wires())
	{
		स्वतः hdlname = hdlname_mode < 0 ? std::vector<std::string>() : wire->get_hdlname_attribute();
		साठी (स्वतः item : hdlname)
			path.ढकला_back("\\" + item);
		जर (hdlname.size() == 1 && wire->name == path.back())
			hdlname.clear();
		जर (!hdlname.empty())
			callback(const_path, WitnessHierarchyItem(module, wire), data);
		path.resize(path_size);
		जर (hdlname.empty() || hdlname_mode <= 0) {
			path.ढकला_back(wire->name);
			callback(const_path, WitnessHierarchyItem(module, wire), data);
			path.काढा_back();
		}
	}

	साठी (स्वतः cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		जर (child == nullptr)
			सुरूठेवा;

		स्वतः hdlname = hdlname_mode < 0 ? std::vector<std::string>() : cell->get_hdlname_attribute();
		साठी (स्वतः item : hdlname)
			path.ढकला_back("\\" + item);
		जर (hdlname.size() == 1 && cell->name == path.back())
			hdlname.clear();
		जर (!hdlname.empty()) {
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, 1, child, child_data, callback);
		}
		path.resize(path_size);
		जर (hdlname.empty() || hdlname_mode <= 0) {
			path.ढकला_back(cell->name);
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, hdlname.empty() ? hdlname_mode : -1, child, child_data, callback);
			path.काढा_back();
		}
	}

	साठी (स्वतः mem : Mem::get_all_memories(module)) {
		std::vector<std::string> hdlname;

		जर (hdlname_mode >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		साठी (स्वतः item : hdlname)
			path.ढकला_back("\\" + item);
		जर (hdlname.size() == 1 && mem.cell->name == path.back())
			hdlname.clear();
		जर (!hdlname.empty()) {
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
		}
		path.resize(path_size);

		जर (hdlname.empty() || hdlname_mode <= 0) {
			path.ढकला_back(mem.memid.str());
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
			path.काढा_back();

			जर (mem.cell != nullptr && mem.cell->name != mem.memid) {
				path.ढकला_back(mem.cell->name);
				callback(const_path, WitnessHierarchyItem(module, &mem), data);
				path.काढा_back();
			}
		}
	}
}

ढाँचा<typename D, typename T>
शून्य witness_hierarchy(RTLIL::Module *module, D data, T callback)
{
	IdPath path;
	witness_hierarchy_recursion<D, T>(path, 0, module, data, callback);
}

YOSYS_NAMESPACE_END

#endif
