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

#यदि_अपरिभाषितम् YW_H
#परिभाषय YW_H

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

संरचना Id_मार्गः : सार्वजनिक std::सदिशम्<std::string>
{
	प्रतिरूपम्<typename... T>
	Id_मार्गः(T&&... तर्काः_2) : std::सदिशम्<std::string>(std::forward<T>(तर्काः_2)...) { }
	Id_मार्गः उपसर्गः_2() ध्रुव { प्रत्यागम {begin(), अन्तः() - !रिक्तानि()}; }
	std::string str() ध्रुव;

	bool has_स्थानाङ्कः() ध्रुव { पूर्णाङ्क tmp; प्रत्यागम get_स्थानाङ्कः(tmp); };
	bool get_स्थानाङ्कः(पूर्णाङ्क &addr) ध्रुव;

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
		h.eat(static_cast<ध्रुव std::सदिशम्<std::string>&&>(*this));
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

प्रतिरूपम्<typename D, typename T>
शून्य witness_hierarchy(RTLIL::Module *module, D दत्त, T callback);

प्रतिरूपम्<class T> स्थिर std::सदिशम्<std::string> witness_मार्गः(T *obj) {
	std::सदिशम्<std::string> मार्गः_2;
	यदि (obj->name.is_लौकिकम्()) {
		स्वचालित hdlname = obj->get_string_attribute(ID::hdlname);
		प्रति (स्वचालित token : विभज_tokens(hdlname))
			मार्गः_2.प्रक्षिप_पृष्ठभागः("\\" + token);
	}
	यदि (मार्गः_2.रिक्तानि())
		मार्गः_2.प्रक्षिप_पृष्ठभागः(obj->name.str());
	प्रत्यागम मार्गः_2;
}

संरचना ReadWitness
{
	संरचना Clock {
		Id_मार्गः मार्गः_2;
		पूर्णाङ्क विस्थापनम्;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	संरचना Signal {
		Id_मार्गः मार्गः_2;
		पूर्णाङ्क विस्थापनम्;
		पूर्णाङ्क विस्तारः;
		bool init_only;

		पूर्णाङ्क द्वयङ्काः_विस्थापनम्;
	};

	संरचना चरणः_2 {
		std::string द्वयङ्काः;
	};

	std::string filename;
	std::सदिशम्<Clock> clocks;
	std::सदिशम्<Signal> सङ्केताः;
	std::सदिशम्<चरणः_2> चरणाः;

	ReadWitness(ध्रुव std::string &filename);

	RTLIL::Const get_द्वयङ्काः(पूर्णाङ्क t, पूर्णाङ्क द्वयङ्काः_विस्थापनम्, पूर्णाङ्क विस्तारः) ध्रुव;
};

प्रतिरूपम्<typename D, typename T>
शून्य witness_hierarchy_recursion(Id_मार्गः &मार्गः_2, पूर्णाङ्क hdlname_बहुलकम्, RTLIL::Module *module, D दत्त, T &callback)
{
	स्वचालित ध्रुव &const_मार्गः = मार्गः_2;
	आकार_t मार्गः_आकार = मार्गः_2.आकार();
	प्रति (स्वचालित wire : module->wires())
	{
		स्वचालित hdlname = hdlname_बहुलकम् < 0 ? std::सदिशम्<std::string>() : wire->get_hdlname_attribute();
		प्रति (स्वचालित item : hdlname)
			मार्गः_2.प्रक्षिप_पृष्ठभागः("\\" + item);
		यदि (hdlname.आकार() == 1 && wire->name == मार्गः_2.पृष्ठभागः())
			hdlname.clear();
		यदि (!hdlname.रिक्तानि())
			callback(const_मार्गः, WitnessHierarchyItem(module, wire), दत्त);
		मार्गः_2.resize(मार्गः_आकार);
		यदि (hdlname.रिक्तानि() || hdlname_बहुलकम् <= 0) {
			मार्गः_2.प्रक्षिप_पृष्ठभागः(wire->name);
			callback(const_मार्गः, WitnessHierarchyItem(module, wire), दत्त);
			मार्गः_2.निष्कासय_पृष्ठभागः();
		}
	}

	प्रति (स्वचालित cell : module->cells())
	{
		Module *child = module->design->module(cell->type);
		यदि (child == nullptr)
			अनुवर्त;

		स्वचालित hdlname = hdlname_बहुलकम् < 0 ? std::सदिशम्<std::string>() : cell->get_hdlname_attribute();
		प्रति (स्वचालित item : hdlname)
			मार्गः_2.प्रक्षिप_पृष्ठभागः("\\" + item);
		यदि (hdlname.आकार() == 1 && cell->name == मार्गः_2.पृष्ठभागः())
			hdlname.clear();
		यदि (!hdlname.रिक्तानि()) {
			D child_दत्त = callback(const_मार्गः, WitnessHierarchyItem(module, cell), दत्त);
			witness_hierarchy_recursion<D, T>(मार्गः_2, 1, child, child_दत्त, callback);
		}
		मार्गः_2.resize(मार्गः_आकार);
		यदि (hdlname.रिक्तानि() || hdlname_बहुलकम् <= 0) {
			मार्गः_2.प्रक्षिप_पृष्ठभागः(cell->name);
			D child_दत्त = callback(const_मार्गः, WitnessHierarchyItem(module, cell), दत्त);
			witness_hierarchy_recursion<D, T>(मार्गः_2, hdlname.रिक्तानि() ? hdlname_बहुलकम् : -1, child, child_दत्त, callback);
			मार्गः_2.निष्कासय_पृष्ठभागः();
		}
	}

	प्रति (स्वचालित mem : Mem::get_all_memories(module)) {
		std::सदिशम्<std::string> hdlname;

		यदि (hdlname_बहुलकम् >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		प्रति (स्वचालित item : hdlname)
			मार्गः_2.प्रक्षिप_पृष्ठभागः("\\" + item);
		यदि (hdlname.आकार() == 1 && mem.cell->name == मार्गः_2.पृष्ठभागः())
			hdlname.clear();
		यदि (!hdlname.रिक्तानि()) {
			callback(const_मार्गः, WitnessHierarchyItem(module, &mem), दत्त);
		}
		मार्गः_2.resize(मार्गः_आकार);

		यदि (hdlname.रिक्तानि() || hdlname_बहुलकम् <= 0) {
			मार्गः_2.प्रक्षिप_पृष्ठभागः(mem.memid.str());
			callback(const_मार्गः, WitnessHierarchyItem(module, &mem), दत्त);
			मार्गः_2.निष्कासय_पृष्ठभागः();

			यदि (mem.cell != nullptr && mem.cell->name != mem.memid) {
				मार्गः_2.प्रक्षिप_पृष्ठभागः(mem.cell->name);
				callback(const_मार्गः, WitnessHierarchyItem(module, &mem), दत्त);
				मार्गः_2.निष्कासय_पृष्ठभागः();
			}
		}
	}
}

प्रतिरूपम्<typename D, typename T>
शून्य witness_hierarchy(RTLIL::Module *module, D दत्त, T callback)
{
	Id_मार्गः मार्गः_2;
	witness_hierarchy_recursion<D, T>(मार्गः_2, 0, module, दत्त, callback);
}

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
