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

#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

கட்டமைப்பு IdPath : பொது std::vector<std::string>
{
	template<typename... T>
	IdPath(T&&... args) : std::vector<std::string>(std::forward<T>(args)...) { }
	IdPath prefix() மாறிலி { திரும்பு {begin(), end() - !empty()}; }
	std::string str() மாறிலி;

	bool has_address() மாறிலி { முழு tmp; திரும்பு get_address(tmp); };
	bool get_address(முழு &addr) மாறிலி;

	[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
		h.eat(static_cast<மாறிலி std::vector<std::string>&&>(*this));
		திரும்பு h;
	}
};

கட்டமைப்பு WitnessHierarchyItem {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	WitnessHierarchyItem(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

template<typename D, typename T>
வெறுமை witness_hierarchy(RTLIL::Module *module, D data, T callback);

template<class T> நிலையான std::vector<std::string> witness_path(T *obj) {
	std::vector<std::string> path;
	எனில் (obj->name.isPublic()) {
		auto hdlname = obj->get_string_attribute(ID::hdlname);
		க்கு (auto token : split_tokens(hdlname))
			path.தள்ளு_back("\\" + token);
	}
	எனில் (path.empty())
		path.தள்ளு_back(obj->name.str());
	திரும்பு path;
}

கட்டமைப்பு ReadWitness
{
	கட்டமைப்பு Clock {
		IdPath path;
		முழு offset;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	கட்டமைப்பு Signal {
		IdPath path;
		முழு offset;
		முழு width;
		bool init_only;

		முழு bits_offset;
	};

	கட்டமைப்பு Step {
		std::string bits;
	};

	std::string filename;
	std::vector<Clock> clocks;
	std::vector<Signal> signals;
	std::vector<Step> steps;

	ReadWitness(மாறிலி std::string &filename);

	RTLIL::Const get_bits(முழு t, முழு bits_offset, முழு width) மாறிலி;
};

template<typename D, typename T>
வெறுமை witness_hierarchy_recursion(IdPath &path, முழு hdlname_mode, RTLIL::Module *module, D data, T &callback)
{
	auto மாறிலி &const_path = path;
	size_t path_size = path.size();
	க்கு (auto wire : module->wires())
	{
		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : wire->get_hdlname_attribute();
		க்கு (auto item : hdlname)
			path.தள்ளு_back("\\" + item);
		எனில் (hdlname.size() == 1 && wire->name == path.back())
			hdlname.clear();
		எனில் (!hdlname.empty())
			callback(const_path, WitnessHierarchyItem(module, wire), data);
		path.resize(path_size);
		எனில் (hdlname.empty() || hdlname_mode <= 0) {
			path.தள்ளு_back(wire->name);
			callback(const_path, WitnessHierarchyItem(module, wire), data);
			path.எடு_back();
		}
	}

	க்கு (auto cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		எனில் (child == nullptr)
			தொடர்;

		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : cell->get_hdlname_attribute();
		க்கு (auto item : hdlname)
			path.தள்ளு_back("\\" + item);
		எனில் (hdlname.size() == 1 && cell->name == path.back())
			hdlname.clear();
		எனில் (!hdlname.empty()) {
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, 1, child, child_data, callback);
		}
		path.resize(path_size);
		எனில் (hdlname.empty() || hdlname_mode <= 0) {
			path.தள்ளு_back(cell->name);
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, hdlname.empty() ? hdlname_mode : -1, child, child_data, callback);
			path.எடு_back();
		}
	}

	க்கு (auto mem : Mem::get_all_memories(module)) {
		std::vector<std::string> hdlname;

		எனில் (hdlname_mode >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		க்கு (auto item : hdlname)
			path.தள்ளு_back("\\" + item);
		எனில் (hdlname.size() == 1 && mem.cell->name == path.back())
			hdlname.clear();
		எனில் (!hdlname.empty()) {
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
		}
		path.resize(path_size);

		எனில் (hdlname.empty() || hdlname_mode <= 0) {
			path.தள்ளு_back(mem.memid.str());
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
			path.எடு_back();

			எனில் (mem.cell != nullptr && mem.cell->name != mem.memid) {
				path.தள்ளு_back(mem.cell->name);
				callback(const_path, WitnessHierarchyItem(module, &mem), data);
				path.எடு_back();
			}
		}
	}
}

template<typename D, typename T>
வெறுமை witness_hierarchy(RTLIL::Module *module, D data, T callback)
{
	IdPath path;
	witness_hierarchy_recursion<D, T>(path, 0, module, data, callback);
}

YOSYS_NAMESPACE_END

#endif
