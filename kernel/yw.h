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

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

ഘടന IdPath : പൊതു std::vector<std::string>
{
	template<typename... T>
	IdPath(T&&... args) : std::vector<std::string>(std::forward<T>(args)...) { }
	IdPath prefix() സ്ഥിരം { തിരികെ {begin(), end() - !empty()}; }
	std::string str() സ്ഥിരം;

	bool has_address() സ്ഥിരം { പൂർണ്ണസംഖ്യ tmp; തിരികെ get_address(tmp); };
	bool get_address(പൂർണ്ണസംഖ്യ &addr) സ്ഥിരം;

	[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
		h.eat(static_cast<സ്ഥിരം std::vector<std::string>&&>(*this));
		തിരികെ h;
	}
};

ഘടന WitnessHierarchyItem {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	WitnessHierarchyItem(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

template<typename D, typename T>
ശൂന്യം witness_hierarchy(RTLIL::Module *module, D data, T callback);

template<class T> സ്ഥിതം std::vector<std::string> witness_path(T *obj) {
	std::vector<std::string> path;
	എങ്കിൽ (obj->name.isPublic()) {
		auto hdlname = obj->get_string_attribute(ID::hdlname);
		വേണ്ടി (auto token : split_tokens(hdlname))
			path.തള്ളുക_back("\\" + token);
	}
	എങ്കിൽ (path.empty())
		path.തള്ളുക_back(obj->name.str());
	തിരികെ path;
}

ഘടന ReadWitness
{
	ഘടന Clock {
		IdPath path;
		പൂർണ്ണസംഖ്യ offset;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	ഘടന Signal {
		IdPath path;
		പൂർണ്ണസംഖ്യ offset;
		പൂർണ്ണസംഖ്യ width;
		bool init_only;

		പൂർണ്ണസംഖ്യ bits_offset;
	};

	ഘടന Step {
		std::string bits;
	};

	std::string filename;
	std::vector<Clock> clocks;
	std::vector<Signal> signals;
	std::vector<Step> steps;

	ReadWitness(സ്ഥിരം std::string &filename);

	RTLIL::Const get_bits(പൂർണ്ണസംഖ്യ t, പൂർണ്ണസംഖ്യ bits_offset, പൂർണ്ണസംഖ്യ width) സ്ഥിരം;
};

template<typename D, typename T>
ശൂന്യം witness_hierarchy_recursion(IdPath &path, പൂർണ്ണസംഖ്യ hdlname_mode, RTLIL::Module *module, D data, T &callback)
{
	auto സ്ഥിരം &const_path = path;
	size_t path_size = path.size();
	വേണ്ടി (auto wire : module->wires())
	{
		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : wire->get_hdlname_attribute();
		വേണ്ടി (auto item : hdlname)
			path.തള്ളുക_back("\\" + item);
		എങ്കിൽ (hdlname.size() == 1 && wire->name == path.back())
			hdlname.clear();
		എങ്കിൽ (!hdlname.empty())
			callback(const_path, WitnessHierarchyItem(module, wire), data);
		path.resize(path_size);
		എങ്കിൽ (hdlname.empty() || hdlname_mode <= 0) {
			path.തള്ളുക_back(wire->name);
			callback(const_path, WitnessHierarchyItem(module, wire), data);
			path.എടുക്കുക_back();
		}
	}

	വേണ്ടി (auto cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		എങ്കിൽ (child == nullptr)
			തുടരുക;

		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : cell->get_hdlname_attribute();
		വേണ്ടി (auto item : hdlname)
			path.തള്ളുക_back("\\" + item);
		എങ്കിൽ (hdlname.size() == 1 && cell->name == path.back())
			hdlname.clear();
		എങ്കിൽ (!hdlname.empty()) {
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, 1, child, child_data, callback);
		}
		path.resize(path_size);
		എങ്കിൽ (hdlname.empty() || hdlname_mode <= 0) {
			path.തള്ളുക_back(cell->name);
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, hdlname.empty() ? hdlname_mode : -1, child, child_data, callback);
			path.എടുക്കുക_back();
		}
	}

	വേണ്ടി (auto mem : Mem::get_all_memories(module)) {
		std::vector<std::string> hdlname;

		എങ്കിൽ (hdlname_mode >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		വേണ്ടി (auto item : hdlname)
			path.തള്ളുക_back("\\" + item);
		എങ്കിൽ (hdlname.size() == 1 && mem.cell->name == path.back())
			hdlname.clear();
		എങ്കിൽ (!hdlname.empty()) {
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
		}
		path.resize(path_size);

		എങ്കിൽ (hdlname.empty() || hdlname_mode <= 0) {
			path.തള്ളുക_back(mem.memid.str());
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
			path.എടുക്കുക_back();

			എങ്കിൽ (mem.cell != nullptr && mem.cell->name != mem.memid) {
				path.തള്ളുക_back(mem.cell->name);
				callback(const_path, WitnessHierarchyItem(module, &mem), data);
				path.എടുക്കുക_back();
			}
		}
	}
}

template<typename D, typename T>
ശൂന്യം witness_hierarchy(RTLIL::Module *module, D data, T callback)
{
	IdPath path;
	witness_hierarchy_recursion<D, T>(path, 0, module, data, callback);
}

YOSYS_NAMESPACE_END

#endif
