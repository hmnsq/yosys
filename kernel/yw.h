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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

સંરચના IdPath : જાહેર std::vector<std::string>
{
	template<typename... T>
	IdPath(T&&... args) : std::vector<std::string>(std::forward<T>(args)...) { }
	IdPath prefix() અચલ { પાછા {begin(), end() - !empty()}; }
	std::string str() અચલ;

	bool has_address() અચલ { પૂર્ણાંક tmp; પાછા get_address(tmp); };
	bool get_address(પૂર્ણાંક &addr) અચલ;

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
		h.eat(static_cast<અચલ std::vector<std::string>&&>(*this));
		પાછા h;
	}
};

સંરચના WitnessHierarchyItem {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	WitnessHierarchyItem(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

template<typename D, typename T>
શૂન્ય witness_hierarchy(RTLIL::Module *module, D data, T callback);

template<class T> સ્થિર std::vector<std::string> witness_path(T *obj) {
	std::vector<std::string> path;
	જો (obj->name.isPublic()) {
		auto hdlname = obj->get_string_attribute(ID::hdlname);
		માટે (auto token : split_tokens(hdlname))
			path.ધકેલો_back("\\" + token);
	}
	જો (path.empty())
		path.ધકેલો_back(obj->name.str());
	પાછા path;
}

સંરચના ReadWitness
{
	સંરચના Clock {
		IdPath path;
		પૂર્ણાંક offset;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	સંરચના Signal {
		IdPath path;
		પૂર્ણાંક offset;
		પૂર્ણાંક width;
		bool init_only;

		પૂર્ણાંક bits_offset;
	};

	સંરચના Step {
		std::string bits;
	};

	std::string filename;
	std::vector<Clock> clocks;
	std::vector<Signal> signals;
	std::vector<Step> steps;

	ReadWitness(અચલ std::string &filename);

	RTLIL::Const get_bits(પૂર્ણાંક t, પૂર્ણાંક bits_offset, પૂર્ણાંક width) અચલ;
};

template<typename D, typename T>
શૂન્ય witness_hierarchy_recursion(IdPath &path, પૂર્ણાંક hdlname_mode, RTLIL::Module *module, D data, T &callback)
{
	auto અચલ &const_path = path;
	size_t path_size = path.size();
	માટે (auto wire : module->wires())
	{
		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : wire->get_hdlname_attribute();
		માટે (auto item : hdlname)
			path.ધકેલો_back("\\" + item);
		જો (hdlname.size() == 1 && wire->name == path.back())
			hdlname.clear();
		જો (!hdlname.empty())
			callback(const_path, WitnessHierarchyItem(module, wire), data);
		path.resize(path_size);
		જો (hdlname.empty() || hdlname_mode <= 0) {
			path.ધકેલો_back(wire->name);
			callback(const_path, WitnessHierarchyItem(module, wire), data);
			path.કાઢો_back();
		}
	}

	માટે (auto cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		જો (child == nullptr)
			ચાલુ;

		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : cell->get_hdlname_attribute();
		માટે (auto item : hdlname)
			path.ધકેલો_back("\\" + item);
		જો (hdlname.size() == 1 && cell->name == path.back())
			hdlname.clear();
		જો (!hdlname.empty()) {
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, 1, child, child_data, callback);
		}
		path.resize(path_size);
		જો (hdlname.empty() || hdlname_mode <= 0) {
			path.ધકેલો_back(cell->name);
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, hdlname.empty() ? hdlname_mode : -1, child, child_data, callback);
			path.કાઢો_back();
		}
	}

	માટે (auto mem : Mem::get_all_memories(module)) {
		std::vector<std::string> hdlname;

		જો (hdlname_mode >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		માટે (auto item : hdlname)
			path.ધકેલો_back("\\" + item);
		જો (hdlname.size() == 1 && mem.cell->name == path.back())
			hdlname.clear();
		જો (!hdlname.empty()) {
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
		}
		path.resize(path_size);

		જો (hdlname.empty() || hdlname_mode <= 0) {
			path.ધકેલો_back(mem.memid.str());
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
			path.કાઢો_back();

			જો (mem.cell != nullptr && mem.cell->name != mem.memid) {
				path.ધકેલો_back(mem.cell->name);
				callback(const_path, WitnessHierarchyItem(module, &mem), data);
				path.કાઢો_back();
			}
		}
	}
}

template<typename D, typename T>
શૂન્ય witness_hierarchy(RTLIL::Module *module, D data, T callback)
{
	IdPath path;
	witness_hierarchy_recursion<D, T>(path, 0, module, data, callback);
}

YOSYS_NAMESPACE_END

#endif
