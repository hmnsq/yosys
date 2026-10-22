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

#నిర్వచితం_కాకపోతే YW_H
#నిర్వచించు YW_H

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/mem.h"

YOSYS_NAMESPACE_BEGIN

నిర్మాణం Id_మార్గం : బహిరంగ std::సదిశ<std::string>
{
	మూస<typename... T>
	Id_మార్గం(T&&... వాదనలు_2) : std::సదిశ<std::string>(std::forward<T>(వాదనలు_2)...) { }
	Id_మార్గం ఉపసర్గ_2() స్థిరరాశి { తిరిగి {begin(), చివర() - !ఖాళీలు()}; }
	std::string str() స్థిరరాశి;

	bool has_చిరునామా() స్థిరరాశి { పూర్ణాంకం tmp; తిరిగి get_చిరునామా(tmp); };
	bool get_చిరునామా(పూర్ణాంకం &addr) స్థిరరాశి;

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
		h.eat(static_cast<స్థిరరాశి std::సదిశ<std::string>&&>(*this));
		తిరిగి h;
	}
};

నిర్మాణం Witness_Hierarchy_అంశం {
	RTLIL::Module *module;
	RTLIL::Wire *wire = nullptr;
	RTLIL::Cell *cell = nullptr;
	Mem *mem = nullptr;

	Witness_Hierarchy_అంశం(RTLIL::Module *module, RTLIL::Wire *wire) : module(module), wire(wire) {}
	Witness_Hierarchy_అంశం(RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) {}
	Witness_Hierarchy_అంశం(RTLIL::Module *module, Mem *mem) : module(module), mem(mem) {}
};

మూస<typename D, typename T>
శూన్య witness_hierarchy(RTLIL::Module *module, D దత్తాంశం, T callback);

మూస<class T> స్థిర std::సదిశ<std::string> witness_మార్గం(T *obj) {
	std::సదిశ<std::string> మార్గం_2;
	ఒకవేళ (obj->name.is_ప్రజా()) {
		స్వయంచాలక hdlname = obj->get_string_attribute(ID::hdlname);
		కొరకు (స్వయంచాలక token : విభజించు_టోకెన్లు(hdlname))
			మార్గం_2.నెట్టు_వెనుక("\\" + token);
	}
	ఒకవేళ (మార్గం_2.ఖాళీలు())
		మార్గం_2.నెట్టు_వెనుక(obj->name.str());
	తిరిగి మార్గం_2;
}

నిర్మాణం ReadWitness
{
	నిర్మాణం Clock {
		Id_మార్గం మార్గం_2;
		పూర్ణాంకం స్థానభ్రంశం;
		bool is_posedge = false;
		bool is_negedge = false;
	};

	నిర్మాణం Signal {
		Id_మార్గం మార్గం_2;
		పూర్ణాంకం స్థానభ్రంశం;
		పూర్ణాంకం వెడల్పు;
		bool init_only;

		పూర్ణాంకం బిట్లు_స్థానభ్రంశం;
	};

	నిర్మాణం అడుగు_2 {
		std::string బిట్లు;
	};

	std::string filename;
	std::సదిశ<Clock> clocks;
	std::సదిశ<Signal> సంకేతాలు;
	std::సదిశ<అడుగు_2> అడుగులు;

	ReadWitness(స్థిరరాశి std::string &filename);

	RTLIL::Const get_బిట్లు(పూర్ణాంకం t, పూర్ణాంకం బిట్లు_స్థానభ్రంశం, పూర్ణాంకం వెడల్పు) స్థిరరాశి;
};

మూస<typename D, typename T>
శూన్య witness_hierarchy_recursion(Id_మార్గం &మార్గం_2, పూర్ణాంకం hdlname_బహుళకం, RTLIL::Module *module, D దత్తాంశం, T &callback)
{
	స్వయంచాలక స్థిరరాశి &const_మార్గం = మార్గం_2;
	కొలత_t మార్గం_కొలత = మార్గం_2.కొలత();
	కొరకు (స్వయంచాలక wire : module->wires())
	{
		స్వయంచాలక hdlname = hdlname_బహుళకం < 0 ? std::సదిశ<std::string>() : wire->get_hdlname_attribute();
		కొరకు (స్వయంచాలక అంశం : hdlname)
			మార్గం_2.నెట్టు_వెనుక("\\" + అంశం);
		ఒకవేళ (hdlname.కొలత() == 1 && wire->name == మార్గం_2.వెనుక())
			hdlname.clear();
		ఒకవేళ (!hdlname.ఖాళీలు())
			callback(const_మార్గం, Witness_Hierarchy_అంశం(module, wire), దత్తాంశం);
		మార్గం_2.resize(మార్గం_కొలత);
		ఒకవేళ (hdlname.ఖాళీలు() || hdlname_బహుళకం <= 0) {
			మార్గం_2.నెట్టు_వెనుక(wire->name);
			callback(const_మార్గం, Witness_Hierarchy_అంశం(module, wire), దత్తాంశం);
			మార్గం_2.తీయి_వెనుక();
		}
	}

	కొరకు (స్వయంచాలక cell : module->cells())
	{
		Module *బిడ్డ = module->design->module(cell->type);
		ఒకవేళ (బిడ్డ == nullptr)
			కొనసాగు;

		స్వయంచాలక hdlname = hdlname_బహుళకం < 0 ? std::సదిశ<std::string>() : cell->get_hdlname_attribute();
		కొరకు (స్వయంచాలక అంశం : hdlname)
			మార్గం_2.నెట్టు_వెనుక("\\" + అంశం);
		ఒకవేళ (hdlname.కొలత() == 1 && cell->name == మార్గం_2.వెనుక())
			hdlname.clear();
		ఒకవేళ (!hdlname.ఖాళీలు()) {
			D బిడ్డ_దత్తాంశం = callback(const_మార్గం, Witness_Hierarchy_అంశం(module, cell), దత్తాంశం);
			witness_hierarchy_recursion<D, T>(మార్గం_2, 1, బిడ్డ, బిడ్డ_దత్తాంశం, callback);
		}
		మార్గం_2.resize(మార్గం_కొలత);
		ఒకవేళ (hdlname.ఖాళీలు() || hdlname_బహుళకం <= 0) {
			మార్గం_2.నెట్టు_వెనుక(cell->name);
			D బిడ్డ_దత్తాంశం = callback(const_మార్గం, Witness_Hierarchy_అంశం(module, cell), దత్తాంశం);
			witness_hierarchy_recursion<D, T>(మార్గం_2, hdlname.ఖాళీలు() ? hdlname_బహుళకం : -1, బిడ్డ, బిడ్డ_దత్తాంశం, callback);
			మార్గం_2.తీయి_వెనుక();
		}
	}

	కొరకు (స్వయంచాలక mem : Mem::get_all_memories(module)) {
		std::సదిశ<std::string> hdlname;

		ఒకవేళ (hdlname_బహుళకం >= 0 && mem.cell != nullptr)
			hdlname = mem.cell->get_hdlname_attribute();
		కొరకు (స్వయంచాలక అంశం : hdlname)
			మార్గం_2.నెట్టు_వెనుక("\\" + అంశం);
		ఒకవేళ (hdlname.కొలత() == 1 && mem.cell->name == మార్గం_2.వెనుక())
			hdlname.clear();
		ఒకవేళ (!hdlname.ఖాళీలు()) {
			callback(const_మార్గం, Witness_Hierarchy_అంశం(module, &mem), దత్తాంశం);
		}
		మార్గం_2.resize(మార్గం_కొలత);

		ఒకవేళ (hdlname.ఖాళీలు() || hdlname_బహుళకం <= 0) {
			మార్గం_2.నెట్టు_వెనుక(mem.memid.str());
			callback(const_మార్గం, Witness_Hierarchy_అంశం(module, &mem), దత్తాంశం);
			మార్గం_2.తీయి_వెనుక();

			ఒకవేళ (mem.cell != nullptr && mem.cell->name != mem.memid) {
				మార్గం_2.నెట్టు_వెనుక(mem.cell->name);
				callback(const_మార్గం, Witness_Hierarchy_అంశం(module, &mem), దత్తాంశం);
				మార్గం_2.తీయి_వెనుక();
			}
		}
	}
}

మూస<typename D, typename T>
శూన్య witness_hierarchy(RTLIL::Module *module, D దత్తాంశం, T callback)
{
	Id_మార్గం మార్గం_2;
	witness_hierarchy_recursion<D, T>(మార్గం_2, 0, module, దత్తాంశం, callback);
}

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
