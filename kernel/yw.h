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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

ସଂରଚନା IdPath : ସର୍ବସାଧାରଣ std::vector<std::string>
{
	template<typename... T>
	IdPath(T&&... args) : std::vector<std::string>(std::forward<T>(args)...) { }
	IdPath prefix() ଅଚଳ { ଫେରନ୍ତୁ {begin(), end() - !empty()}; }
	std::string str() ଅଚଳ;

	bool has_address() ଅଚଳ { ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp; ଫେରନ୍ତୁ get_address(tmp); };
	bool get_address(ପୂର୍ଣ୍ଣସଂଖ୍ୟା &addr) ଅଚଳ;

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
		h.eat(static_cast<ଅଚଳ std::vector<std::string>&&>(*this));
		ଫେରନ୍ତୁ h;
	}
};

ସଂରଚନା WitnessHierarchyItem {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	WitnessHierarchyItem(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	WitnessHierarchyItem(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

template<typename D, typename T>
ଶୂନ୍ୟ witness_hierarchy(RTLIL::Module *module, D data, T callback);

template<class T> ସ୍ଥିର std::vector<std::string> witness_path(T *obj) {
	std::vector<std::string> path;
	ଯଦି (obj->name.isPublic()) {
		auto hdlname = obj->get_string_attribute(ID::hdlname);
		ପାଇଁ (auto token : split_tokens(hdlname))
			path.ଠେଲ_back("\\" + token);
	}
	ଯଦି (path.empty())
		path.ଠେଲ_back(obj->name.str());
	ଫେରନ୍ତୁ path;
}

ସଂରଚନା ReadWitness
{
	ସଂରଚନା Clock {
		IdPath path;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	ସଂରଚନା Signal {
		IdPath path;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା width;
		bool init_only;

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits_offset;
	};

	ସଂରଚନା Step {
		std::string bits;
	};

	std::string filename;
	std::vector<Clock> clocks;
	std::vector<Signal> signals;
	std::vector<Step> steps;

	ReadWitness(ଅଚଳ std::string &filename);

	RTLIL::Const get_bits(ପୂର୍ଣ୍ଣସଂଖ୍ୟା t, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width) ଅଚଳ;
};

template<typename D, typename T>
ଶୂନ୍ୟ witness_hierarchy_recursion(IdPath &path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hdlname_mode, RTLIL::Module *module, D data, T &callback)
{
	auto ଅଚଳ &const_path = path;
	size_t path_size = path.size();
	ପାଇଁ (auto wire : module->wires())
	{
		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : wire->get_hdlname_attribute();
		ପାଇଁ (auto item : hdlname)
			path.ଠେଲ_back("\\" + item);
		ଯଦି (hdlname.size() == 1 && wire->name == path.back())
			hdlname.clear();
		ଯଦି (!hdlname.empty())
			callback(const_path, WitnessHierarchyItem(module, wire), data);
		path.resize(path_size);
		ଯଦି (hdlname.empty() || hdlname_mode <= 0) {
			path.ଠେଲ_back(wire->name);
			callback(const_path, WitnessHierarchyItem(module, wire), data);
			path.କାଢ଼_back();
		}
	}

	ପାଇଁ (auto cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		ଯଦି (child == nullptr)
			ଜାରି;

		auto hdlname = hdlname_mode < 0 ? std::vector<std::string>() : cell->get_hdlname_attribute();
		ପାଇଁ (auto item : hdlname)
			path.ଠେଲ_back("\\" + item);
		ଯଦି (hdlname.size() == 1 && cell->name == path.back())
			hdlname.clear();
		ଯଦି (!hdlname.empty()) {
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, 1, child, child_data, callback);
		}
		path.resize(path_size);
		ଯଦି (hdlname.empty() || hdlname_mode <= 0) {
			path.ଠେଲ_back(cell->name);
			D child_data = callback(const_path, WitnessHierarchyItem(module, cell), data);
			witness_hierarchy_recursion<D, T>(path, hdlname.empty() ? hdlname_mode : -1, child, child_data, callback);
			path.କାଢ଼_back();
		}
	}

	ପାଇଁ (auto mem : Mem::get_all_memories(module)) {
		std::vector<std::string> hdlname;

		ଯଦି (hdlname_mode >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		ପାଇଁ (auto item : hdlname)
			path.ଠେଲ_back("\\" + item);
		ଯଦି (hdlname.size() == 1 && mem.cell->name == path.back())
			hdlname.clear();
		ଯଦି (!hdlname.empty()) {
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
		}
		path.resize(path_size);

		ଯଦି (hdlname.empty() || hdlname_mode <= 0) {
			path.ଠେଲ_back(mem.memid.str());
			callback(const_path, WitnessHierarchyItem(module, &mem), data);
			path.କାଢ଼_back();

			ଯଦି (mem.cell != nullptr && mem.cell->name != mem.memid) {
				path.ଠେଲ_back(mem.cell->name);
				callback(const_path, WitnessHierarchyItem(module, &mem), data);
				path.କାଢ଼_back();
			}
		}
	}
}

template<typename D, typename T>
ଶୂନ୍ୟ witness_hierarchy(RTLIL::Module *module, D data, T callback)
{
	IdPath path;
	witness_hierarchy_recursion<D, T>(path, 0, module, data, callback);
}

YOSYS_NAMESPACE_END

#endif
