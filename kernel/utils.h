/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
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

// This file contains various c++ utility routines and helper classes that
// do not depend on any other components of yosys (except stuff like log_*).

#ضمن "kernel/yosys.h"
#ضمن <iterator>

#که_نه_تعریف_شوی UTILS_H
#تعریف_کړه UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

کينډۍ<typename کلی_2, typename T>
بنية stackmap
{
خاص:
	std::ویکټور<dict<کلی_2, T*>> backup_state;
	dict<کلی_2, T> current_state;
	ساكن T تشه_tuple;

عام:
	stackmap() { }
	stackmap(ثابت dict<کلی_2, T> &other) : current_state(other) { }

	کينډۍ<typename Other>
	فراغ چلوونکی=(ثابت Other &other)
	{
		لكل (خپلکاری &it : current_state)
			إذا (!backup_state.تشه() && backup_state.شا().شمېر(it.لومړی) == 0)
				backup_state.شا()[it.لومړی] = new T(it.دوهم);
		current_state.clear();

		لكل (خپلکاری &it : other)
			سېټ(it.لومړی, it.دوهم);
	}

	bool has(ثابت کلی_2 &k)
	{
		أرجع current_state.شمېر(k) != 0;
	}

	فراغ سېټ(ثابت کلی_2 &k, ثابت T &v)
	{
		إذا (!backup_state.تشه() && backup_state.شا().شمېر(k) == 0)
			backup_state.شا()[k] = current_state.شمېر(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	فراغ unset(ثابت کلی_2 &k)
	{
		إذا (!backup_state.تشه() && backup_state.شا().شمېر(k) == 0)
			backup_state.شا()[k] = current_state.شمېر(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ثابت T &get(ثابت کلی_2 &k)
	{
		إذا (current_state.شمېر(k) == 0)
			أرجع تشه_tuple;
		أرجع current_state.at(k);
	}

	فراغ reset(ثابت کلی_2 &k)
	{
		لكل (صحيح i = Get_اندازه(backup_state)-1; i >= 0; i--)
			إذا (backup_state[i].شمېر(k) != 0) {
				إذا (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				وإلا
					current_state[k] = *backup_state[i].at(k);
				أرجع;
			}
		current_state.erase(k);
	}

	ثابت dict<کلی_2, T> &stdmap()
	{
		أرجع current_state;
	}

	فراغ save()
	{
		backup_state.resize(backup_state.اندازه()+1);
	}

	فراغ restore()
	{
		log_assert(!backup_state.تشه());
		لكل (خپلکاری &it : backup_state.شا())
			إذا (it.دوهم != nullptr) {
				current_state[it.لومړی] = *it.دوهم;
				delete it.دوهم;
			} وإلا
				current_state.erase(it.لومړی);
		backup_state.راوباسه_شا();
	}

	~stackmap()
	{
		طالما (!backup_state.تشه())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

کينډۍ <typename T, typename C = std::less<T>> class Topo_ترتیبول
{
	static_assert(!(std::is_اشاره_ګر<T>::ارزښت && std::is_same<C, std::less<T>>::ارزښت),
			"std::less is run-to-run unstable for pointers");
عام:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	بنية غیرمستقیم_Cmp {
		غیرمستقیم_Cmp(ثابت std::ویکټور<T> &nodes, ثابت C &غوټه_cmp) : غوټه_cmp_2(غوټه_cmp), nodes_(nodes) {}
		bool چلوونکی()(صحيح a, صحيح b) ثابت
		{
			log_assert(static_cast<اندازه_t>(a) < nodes_.اندازه());
			log_assert(static_cast<اندازه_t>(b) < nodes_.اندازه());
			أرجع غوټه_cmp_2(nodes_[a], nodes_[b]);
		}
		ثابت C غوټه_cmp_2;
		ثابت std::ویکټور<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, صحيح, C> غوټه_منزل_شاخص;
	// edges[i] is the set of nodes with an edge into node i
	std::ویکټور<std::سېټ<صحيح, غیرمستقیم_Cmp>> څنډې;
	std::ویکټور<T> sorted;
	std::سېټ<std::ویکټور<T>> loops;

	Topo_ترتیبول() requires std::is_default_constructible_v<C> : Topo_ترتیبول(C()) {}
	explicit Topo_ترتیبول(ثابت C &cmp) : غوټه_منزل_شاخص(cmp), غیرمستقیم_cmp(nodes, cmp)
	{
		analyze_loops = true;
		وموندل_loops = false;
	}

	صحيح غوټه(T n)
	{
		خپلکاری rv = غوټه_منزل_شاخص.emplace(n, static_cast<صحيح>(nodes.اندازه()));
		إذا (rv.دوهم) {
			nodes.ټېل_شا(n);
			څنډې.ټېل_شا(std::سېټ<صحيح, غیرمستقیم_Cmp>(غیرمستقیم_cmp));
		}
		أرجع rv.لومړی->دوهم;
	}

	فراغ څنډه(صحيح l_شاخص, صحيح r_شاخص) { څنډې[r_شاخص].ورننباسه(l_شاخص); }

	فراغ څنډه(T کیڼ_3, T ښی_3) { څنډه(غوټه(کیڼ_3), غوټه(ښی_3)); }

	bool has_غوټه(ثابت T &غوټه) { أرجع غوټه_منزل_شاخص.پیدا_کړه(غوټه) != غوټه_منزل_شاخص.پای_ټکی(); }

	bool ترتیبول()
	{
		log_assert(Get_اندازه(غوټه_منزل_شاخص) == Get_اندازه(څنډې));
		log_assert(Get_اندازه(nodes) == Get_اندازه(څنډې));

		loops.clear();
		sorted.clear();
		وموندل_loops = false;

		std::ویکټور<bool> غوټه_is_sorted(څنډې.اندازه(), false);
		std::ویکټور<bool> غوټه_is_on_ډېری(څنډې.اندازه(), false);
		// Only used with analyze_loops
		std::ویکټور<صحيح> ډېری;
		sorted.reserve(څنډې.اندازه());

		لكل (ثابت خپلکاری &it : غوټه_منزل_شاخص)
			ترتیبول_کارګر(it.دوهم, غوټه_is_sorted, غوټه_is_on_ډېری, ډېری);

		log_assert(Get_اندازه(sorted) == Get_اندازه(nodes));

		أرجع !وموندل_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::سېټ<T, C>, C> get_database()
	{
		std::map<T, std::سېټ<T, C>, C> database(غوټه_منزل_شاخص.کلی_comp());
		لكل (اندازه_t i = 0; i < nodes.اندازه(); ++i) {
			std::سېټ<T, C> converted_څنډه_سېټ(غوټه_منزل_شاخص.کلی_comp());
			لكل (صحيح other_غوټه : څنډې[i]) {
				converted_څنډه_سېټ.ورننباسه(nodes[other_غوټه]);
			}
			database.emplace(nodes[i], converted_څنډه_سېټ);
		}
		أرجع database;
	}

خاص:
	bool وموندل_loops;
	std::ویکټور<T> nodes;
	ثابت غیرمستقیم_Cmp غیرمستقیم_cmp;

	فراغ ترتیبول_کارګر(ثابت صحيح ریښه_شاخص, std::ویکټور<bool> &غوټه_is_sorted, std::ویکټور<bool> &غوټه_is_on_ډېری, std::ویکټور<صحيح> &ډېری)
	{
		إذا (غوټه_is_on_ډېری[ریښه_شاخص]) {
			// We've been here before, meaning we have a loop
			وموندل_loops = true;
			إذا (analyze_loops) {
				std::ویکټور<T> loop;
				لكل (صحيح i = Get_اندازه(ډېری) - 1; i >= 0; i--) {
					ثابت صحيح شاخص = ډېری[i];
					loop.ټېل_شا(nodes[شاخص]);
					إذا (شاخص == ریښه_شاخص)
						اكسر;
				}
				loops.ورننباسه(loop);
			}
			أرجع;
		}

		// We're done if we've already sorted this subgraph
		إذا (غوټه_is_sorted[ریښه_شاخص])
			أرجع;

		إذا (!څنډې[ریښه_شاخص].تشه()) {
			إذا (analyze_loops)
				ډېری.ټېل_شا(ریښه_شاخص);
			غوټه_is_on_ډېری[ریښه_شاخص] = true;

			لكل (صحيح کیڼ_n : څنډې[ریښه_شاخص])
				ترتیبول_کارګر(کیڼ_n, غوټه_is_sorted, غوټه_is_on_ډېری, ډېری);

			إذا (analyze_loops)
				ډېری.راوباسه_شا();
			غوټه_is_on_ډېری[ریښه_شاخص] = false;
		}

		غوټه_is_sorted[ریښه_شاخص] = true;
		sorted.ټېل_شا(nodes[ریښه_شاخص]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
کينډۍ<class T>
بنية arrow_proxy {
	T v;
	explicit arrow_proxy(T ثابت & v) : v(v) {}
	T* چلوونکی->() { أرجع &v; }
};

مضمن صحيح ceil_log2(صحيح x)
{
#إذا defined(__GNUC__)
        أرجع x > 1 ? (8*حجم(صحيح)) - __builtin_clz(x-1) : 0;
#وإلا
	إذا (x <= 0)
		أرجع 0;
	لكل (صحيح i = 0; i < 32; i++)
		إذا (((x-1) >> i) == 0)
			أرجع i;
	log_abort();
#د_که_پای
}

کينډۍ <typename T>
خپلکاری reversed(T& container) {
	بنية سرچپه_view {
		سرچپه_view(T& container) : container(container) {}
		خپلکاری begin() ثابت { أرجع container.rbegin(); }
		خپلکاری پای_ټکی() ثابت { أرجع container.rend(); }
		T& container;
	};
	أرجع سرچپه_view{container};
}

کينډۍ <typename T>
خپلکاری reversed(ثابت T& container) {
	بنية سرچپه_view {
		سرچپه_view(ثابت T& container) : container(container) {}
		خپلکاری begin() ثابت { أرجع container.rbegin(); }
		خپلکاری پای_ټکی() ثابت { أرجع container.rend(); }
		ثابت T& container;
	};
	أرجع سرچپه_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
بنية IntRange {
	صحيح پیل_2;
	صحيح پای_ټکی_4;
	بنية Int {
		صحيح v;
		صحيح چلوونکی*() ثابت { أرجع v; }
		Int &چلوونکی++() { ++v; أرجع *this; }
		bool چلوونکی!=(ثابت Int &other) ثابت { أرجع v != other.v; }
	};
	Int begin() ثابت { أرجع {پیل_2}; }
	Int پای_ټکی() ثابت { أرجع {پای_ټکی_4}; }

	bool چلوونکی==(ثابت IntRange &other) ثابت { أرجع پیل_2 == other.پیل_2 && پای_ټکی_4 == other.پای_ټکی_4; }
	bool چلوونکی!=(ثابت IntRange &other) ثابت { أرجع !(*this == other); }
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
