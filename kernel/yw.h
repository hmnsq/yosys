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

#include "kernel/yosys.h"
#include "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

struct IdPath : public std::vector<std::string>
{
	template<typename... T>
	IdPath(T&&... args) : std::vector<std::string>(std::forward<T>(args)...) { }
	IdPath prefix() const { 返回 {begin(), end() - !empty()}; }
	std::string str() const;

	bool has_address() const { 整数 tmp; 返回 get_address(tmp); };
	bool get_address(整数 &addr) const;

	[[nodiscard]] Hasher hash_into(Hasher h) const {
		h.eat(static_cast<const std::vector<std::string>&&>(*this));
		返回 h;
	}
};

struct WitnessHierarchyItem {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	WitnessHierarchyItem(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

template<typename D, typename T>
void witness_hierarchy(RTLIL::Module *module, D data, T callback);

template<class T> static std::vector<std::string> witness_path(T *obj) {
	std::vector<std::string> path;
	如果 (obj->name.isPublic()) {
		auto hdlname = obj->get_string_attribute(ID::hdlname);
		为 (auto token : split_tokens(hdlname))
			path.push_back("\\" + token);
	}
	如果 (path.empty())
		path.push_back(obj->name.str());
	返回 path;
}

struct ReadWitness
{
	struct Clock {
		IdPath path;
		整数 offset;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	struct Signal {
		IdPath path;
		整数 offset;
		整数 width;
		bool init_only;

		整数 bits_offset;
	};

	struct Step {
		std::string bits;
	};

	std::string filename;
	std::vector<Clock> clocks;
	std::vector<Signal> signals;
	std::vector<Step> steps;

	ReadWitness(const std::string &filename);

	RTLIL::Const get_bits(整数 t, 整数 bits_offset, 整数 width) const;
};

template<typename D, typename T>
void witness_hierarchy_recursion(IdPath &path, 整数 hdlname_mode, RTLIL::Module *module, D data, T &callback)
{
	auto const &const_path = path;
	size_t path_size = path.size();
	为 (auto wire : module->wires())
	{
		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : wire->get_hdlname_attribute();
		为 (auto item : hdlname)
			path.push_back("\\" + item);
		如果 (hdlname.size() == 1 && wire->name == path.back())
			hdlname.clear();
		如果 (!hdlname.empty())
			callback(const_path, WitnessHierarchyItem(module, wire), data);
		path.resize(path_size);
		如果 (hdlname.empty() || hdlname_mode <= 0) {
			path.push_back(wire->name);
			callback(const_path, WitnessHierarchyItem(module, wire), data);
			path.pop_back();
		}
	}

	为 (auto cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		如果 (child == nullptr)
			continue;

		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : cell->get_hdlname_attribute();
		为 (auto item : hdlname)
			path.push_back("\\" + item);
		如果 (hdlname.size() == 1 && cell->name == path.back())
			hdlname.clear();
		如果 (!hdlname.empty()) {
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, 1, child, child_data, callback);
		}
		path.resize(path_size);
		如果 (hdlname.empty() || hdlname_mode <= 0) {
			path.push_back(cell->name);
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, hdlname.empty() ? hdlname_mode : -1, child, child_data, callback);
			path.pop_back();
		}
	}

	为 (auto mem : Mem::get_all_memories(module)) {
		std::vector<std::string> hdlname;

		如果 (hdlname_mode >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		为 (auto item : hdlname)
			path.push_back("\\" + item);
		如果 (hdlname.size() == 1 && mem.cell->name == path.back())
			hdlname.clear();
		如果 (!hdlname.empty()) {
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
		}
		path.resize(path_size);

		如果 (hdlname.empty() || hdlname_mode <= 0) {
			path.push_back(mem.memid.str());
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
			path.pop_back();

			如果 (mem.cell != nullptr && mem.cell->name != mem.memid) {
				path.push_back(mem.cell->name);
				callback(const_path, WitnessHierarchyItem(module, &mem), data);
				path.pop_back();
			}
		}
	}
}

template<typename D, typename T>
void witness_hierarchy(RTLIL::Module *module, D data, T callback)
{
	IdPath path;
	witness_hierarchy_recursion<D, T>(path, 0, module, data, callback);
}

YOSYS_NAMESPACE_END

#endif
