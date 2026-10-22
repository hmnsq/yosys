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

#अगर_अघोषित YW_H
#घोषणा YW_H

#समावेश "kernel/yosys.h"
#समावेश "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

काष्ठा IdPath : खुला std::vector<std::string>
{
	ढाँचा<typename... T>
	IdPath(T&&... args) : std::vector<std::string>(std::forward<T>(args)...) { }
	IdPath prefix() स्थिर { वापस {begin(), end() - !empty()}; }
	std::string str() स्थिर;

	bool has_address() स्थिर { पूर्णांक tmp; वापस get_address(tmp); };
	bool get_address(पूर्णांक &addr) स्थिर;

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
		h.eat(static_cast<स्थिर std::vector<std::string>&&>(*this));
		वापस h;
	}
};

काष्ठा WitnessHierarchyItem {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	WitnessHierarchyItem(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

ढाँचा<typename D, typename T>
व्योम witness_hierarchy(RTLIL::Module *module, D तथ्य, T callback);

ढाँचा<class T> अटल std::vector<std::string> witness_path(T *obj) {
	std::vector<std::string> path;
	अगर (obj->name.isPublic()) {
		स्वतः hdlname = obj->get_string_attribute(ID::hdlname);
		क्रम (स्वतः token : split_tokens(hdlname))
			path.धकेलो_back("\\" + token);
	}
	अगर (path.empty())
		path.धकेलो_back(obj->name.str());
	वापस path;
}

काष्ठा ReadWitness
{
	काष्ठा Clock {
		IdPath path;
		पूर्णांक offset;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	काष्ठा Signal {
		IdPath path;
		पूर्णांक offset;
		पूर्णांक width;
		bool init_only;

		पूर्णांक bits_offset;
	};

	काष्ठा Step {
		std::string bits;
	};

	std::string filename;
	std::vector<Clock> clocks;
	std::vector<Signal> signals;
	std::vector<Step> steps;

	ReadWitness(स्थिर std::string &filename);

	RTLIL::Const get_bits(पूर्णांक t, पूर्णांक bits_offset, पूर्णांक width) स्थिर;
};

ढाँचा<typename D, typename T>
व्योम witness_hierarchy_recursion(IdPath &path, पूर्णांक hdlname_mode, RTLIL::Module *module, D तथ्य, T &callback)
{
	स्वतः स्थिर &const_path = path;
	माप_प्रकार path_आकार = path.आकार();
	क्रम (स्वतः wire : module->wires())
	{
		स्वतः hdlname = hdlname_mode < 0 ? std::vector<std::string>() : wire->get_hdlname_attribute();
		क्रम (स्वतः item : hdlname)
			path.धकेलो_back("\\" + item);
		अगर (hdlname.आकार() == 1 && wire->name == path.back())
			hdlname.clear();
		अगर (!hdlname.empty())
			callback(const_path, WitnessHierarchyItem(module, wire), तथ्य);
		path.resize(path_आकार);
		अगर (hdlname.empty() || hdlname_mode <= 0) {
			path.धकेलो_back(wire->name);
			callback(const_path, WitnessHierarchyItem(module, wire), तथ्य);
			path.निकालो_back();
		}
	}

	क्रम (स्वतः cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		अगर (child == nullptr)
			जारी;

		स्वतः hdlname = hdlname_mode < 0 ? std::vector<std::string>() : cell->get_hdlname_attribute();
		क्रम (स्वतः item : hdlname)
			path.धकेलो_back("\\" + item);
		अगर (hdlname.आकार() == 1 && cell->name == path.back())
			hdlname.clear();
		अगर (!hdlname.empty()) {
			D child_तथ्य = callback(const_path, WitnessHierarchyItem(module, cell), तथ्य);
			witness_hierarchy_recursion<D, T>(path, 1, child, child_तथ्य, callback);
		}
		path.resize(path_आकार);
		अगर (hdlname.empty() || hdlname_mode <= 0) {
			path.धकेलो_back(cell->name);
			D child_तथ्य = callback(const_path, WitnessHierarchyItem(module, cell), तथ्य);
			witness_hierarchy_recursion<D, T>(path, hdlname.empty() ? hdlname_mode : -1, child, child_तथ्य, callback);
			path.निकालो_back();
		}
	}

	क्रम (स्वतः mem : Mem::get_all_memories(module)) {
		std::vector<std::string> hdlname;

		अगर (hdlname_mode >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		क्रम (स्वतः item : hdlname)
			path.धकेलो_back("\\" + item);
		अगर (hdlname.आकार() == 1 && mem.cell->name == path.back())
			hdlname.clear();
		अगर (!hdlname.empty()) {
			callback(const_path, WitnessHierarchyItem(module, &mem), तथ्य);
		}
		path.resize(path_आकार);

		अगर (hdlname.empty() || hdlname_mode <= 0) {
			path.धकेलो_back(mem.memid.str());
			callback(const_path, WitnessHierarchyItem(module, &mem), तथ्य);
			path.निकालो_back();

			अगर (mem.cell != nullptr && mem.cell->name != mem.memid) {
				path.धकेलो_back(mem.cell->name);
				callback(const_path, WitnessHierarchyItem(module, &mem), तथ्य);
				path.निकालो_back();
			}
		}
	}
}

ढाँचा<typename D, typename T>
व्योम witness_hierarchy(RTLIL::Module *module, D तथ्य, T callback)
{
	IdPath path;
	witness_hierarchy_recursion<D, T>(path, 0, module, तथ्य, callback);
}

YOSYS_NAMESPACE_END

#पूर्ण_अगर
