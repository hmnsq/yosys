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

#اگر_تعریف_نشده UTILS_H
#تعریف_کن UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

قالب<typename کلید_2, typename T>
بنية stackmap
{
خاص:
	std::وکتور<dict<کلید_2, T*>> backup_state;
	dict<کلید_2, T> current_state;
	ساكن T خالی_tuple;

عام:
	stackmap() { }
	stackmap(ثابت dict<کلید_2, T> &other) : current_state(other) { }

	قالب<typename Other>
	فراغ عملگر=(ثابت Other &other)
	{
		لكل (خودکار &it : current_state)
			إذا (!backup_state.خالی() && backup_state.پشت().شمار(it.اولی) == 0)
				backup_state.پشت()[it.اولی] = new T(it.دومی);
		current_state.clear();

		لكل (خودکار &it : other)
			مجموعه(it.اولی, it.دومی);
	}

	bool has(ثابت کلید_2 &k)
	{
		أرجع current_state.شمار(k) != 0;
	}

	فراغ مجموعه(ثابت کلید_2 &k, ثابت T &v)
	{
		إذا (!backup_state.خالی() && backup_state.پشت().شمار(k) == 0)
			backup_state.پشت()[k] = current_state.شمار(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	فراغ unset(ثابت کلید_2 &k)
	{
		إذا (!backup_state.خالی() && backup_state.پشت().شمار(k) == 0)
			backup_state.پشت()[k] = current_state.شمار(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ثابت T &get(ثابت کلید_2 &k)
	{
		إذا (current_state.شمار(k) == 0)
			أرجع خالی_tuple;
		أرجع current_state.at(k);
	}

	فراغ reset(ثابت کلید_2 &k)
	{
		لكل (صحيح i = Get_اندازه(backup_state)-1; i >= 0; i--)
			إذا (backup_state[i].شمار(k) != 0) {
				إذا (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				وإلا
					current_state[k] = *backup_state[i].at(k);
				أرجع;
			}
		current_state.erase(k);
	}

	ثابت dict<کلید_2, T> &stdmap()
	{
		أرجع current_state;
	}

	فراغ save()
	{
		backup_state.resize(backup_state.اندازه()+1);
	}

	فراغ restore()
	{
		log_assert(!backup_state.خالی());
		لكل (خودکار &it : backup_state.پشت())
			إذا (it.دومی != nullptr) {
				current_state[it.اولی] = *it.دومی;
				delete it.دومی;
			} وإلا
				current_state.erase(it.اولی);
		backup_state.برداشت_پشت();
	}

	~stackmap()
	{
		طالما (!backup_state.خالی())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

قالب <typename T, typename C = std::less<T>> class Topo_مرتب_سازی
{
	static_assert(!(std::is_اشاره_گر<T>::مقدار && std::is_same<C, std::less<T>>::مقدار),
			"std::less is run-to-run unstable for pointers");
عام:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	بنية غیرمستقیم_Cmp {
		غیرمستقیم_Cmp(ثابت std::وکتور<T> &nodes, ثابت C &گره_cmp) : گره_cmp_2(گره_cmp), nodes_(nodes) {}
		bool عملگر()(صحيح a, صحيح b) ثابت
		{
			log_assert(static_cast<اندازه_t>(a) < nodes_.اندازه());
			log_assert(static_cast<اندازه_t>(b) < nodes_.اندازه());
			أرجع گره_cmp_2(nodes_[a], nodes_[b]);
		}
		ثابت C گره_cmp_2;
		ثابت std::وکتور<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, صحيح, C> گره_مقصد_شاخص;
	// edges[i] is the set of nodes with an edge into node i
	std::وکتور<std::مجموعه<صحيح, غیرمستقیم_Cmp>> یالها;
	std::وکتور<T> sorted;
	std::مجموعه<std::وکتور<T>> loops;

	Topo_مرتب_سازی() requires std::is_default_constructible_v<C> : Topo_مرتب_سازی(C()) {}
	explicit Topo_مرتب_سازی(ثابت C &cmp) : گره_مقصد_شاخص(cmp), غیرمستقیم_cmp(nodes, cmp)
	{
		analyze_loops = true;
		یافت_loops = false;
	}

	صحيح گره(T n)
	{
		خودکار rv = گره_مقصد_شاخص.emplace(n, static_cast<صحيح>(nodes.اندازه()));
		إذا (rv.دومی) {
			nodes.افزودن_پشت(n);
			یالها.افزودن_پشت(std::مجموعه<صحيح, غیرمستقیم_Cmp>(غیرمستقیم_cmp));
		}
		أرجع rv.اولی->دومی;
	}

	فراغ یال(صحيح l_شاخص, صحيح r_شاخص) { یالها[r_شاخص].درج(l_شاخص); }

	فراغ یال(T چپ_3, T راست_3) { یال(گره(چپ_3), گره(راست_3)); }

	bool has_گره(ثابت T &گره) { أرجع گره_مقصد_شاخص.پیدا_کن(گره) != گره_مقصد_شاخص.انتها(); }

	bool مرتب_سازی()
	{
		log_assert(Get_اندازه(گره_مقصد_شاخص) == Get_اندازه(یالها));
		log_assert(Get_اندازه(nodes) == Get_اندازه(یالها));

		loops.clear();
		sorted.clear();
		یافت_loops = false;

		std::وکتور<bool> گره_is_sorted(یالها.اندازه(), false);
		std::وکتور<bool> گره_is_on_پشته(یالها.اندازه(), false);
		// Only used with analyze_loops
		std::وکتور<صحيح> پشته;
		sorted.reserve(یالها.اندازه());

		لكل (ثابت خودکار &it : گره_مقصد_شاخص)
			مرتب_سازی_کارگر(it.دومی, گره_is_sorted, گره_is_on_پشته, پشته);

		log_assert(Get_اندازه(sorted) == Get_اندازه(nodes));

		أرجع !یافت_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::مجموعه<T, C>, C> get_database()
	{
		std::map<T, std::مجموعه<T, C>, C> database(گره_مقصد_شاخص.کلید_comp());
		لكل (اندازه_t i = 0; i < nodes.اندازه(); ++i) {
			std::مجموعه<T, C> converted_یال_مجموعه(گره_مقصد_شاخص.کلید_comp());
			لكل (صحيح other_گره : یالها[i]) {
				converted_یال_مجموعه.درج(nodes[other_گره]);
			}
			database.emplace(nodes[i], converted_یال_مجموعه);
		}
		أرجع database;
	}

خاص:
	bool یافت_loops;
	std::وکتور<T> nodes;
	ثابت غیرمستقیم_Cmp غیرمستقیم_cmp;

	فراغ مرتب_سازی_کارگر(ثابت صحيح ریشه_شاخص, std::وکتور<bool> &گره_is_sorted, std::وکتور<bool> &گره_is_on_پشته, std::وکتور<صحيح> &پشته)
	{
		إذا (گره_is_on_پشته[ریشه_شاخص]) {
			// We've been here before, meaning we have a loop
			یافت_loops = true;
			إذا (analyze_loops) {
				std::وکتور<T> loop;
				لكل (صحيح i = Get_اندازه(پشته) - 1; i >= 0; i--) {
					ثابت صحيح شاخص = پشته[i];
					loop.افزودن_پشت(nodes[شاخص]);
					إذا (شاخص == ریشه_شاخص)
						اكسر;
				}
				loops.درج(loop);
			}
			أرجع;
		}

		// We're done if we've already sorted this subgraph
		إذا (گره_is_sorted[ریشه_شاخص])
			أرجع;

		إذا (!یالها[ریشه_شاخص].خالی()) {
			إذا (analyze_loops)
				پشته.افزودن_پشت(ریشه_شاخص);
			گره_is_on_پشته[ریشه_شاخص] = true;

			لكل (صحيح چپ_n : یالها[ریشه_شاخص])
				مرتب_سازی_کارگر(چپ_n, گره_is_sorted, گره_is_on_پشته, پشته);

			إذا (analyze_loops)
				پشته.برداشت_پشت();
			گره_is_on_پشته[ریشه_شاخص] = false;
		}

		گره_is_sorted[ریشه_شاخص] = true;
		sorted.افزودن_پشت(nodes[ریشه_شاخص]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
قالب<class T>
بنية arrow_proxy {
	T v;
	explicit arrow_proxy(T ثابت & v) : v(v) {}
	T* عملگر->() { أرجع &v; }
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
#پایان_اگر
}

قالب <typename T>
خودکار reversed(T& container) {
	بنية معکوس_view {
		معکوس_view(T& container) : container(container) {}
		خودکار begin() ثابت { أرجع container.rbegin(); }
		خودکار انتها() ثابت { أرجع container.rend(); }
		T& container;
	};
	أرجع معکوس_view{container};
}

قالب <typename T>
خودکار reversed(ثابت T& container) {
	بنية معکوس_view {
		معکوس_view(ثابت T& container) : container(container) {}
		خودکار begin() ثابت { أرجع container.rbegin(); }
		خودکار انتها() ثابت { أرجع container.rend(); }
		ثابت T& container;
	};
	أرجع معکوس_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
بنية IntRange {
	صحيح شروع_2;
	صحيح انتها_4;
	بنية Int {
		صحيح v;
		صحيح عملگر*() ثابت { أرجع v; }
		Int &عملگر++() { ++v; أرجع *this; }
		bool عملگر!=(ثابت Int &other) ثابت { أرجع v != other.v; }
	};
	Int begin() ثابت { أرجع {شروع_2}; }
	Int انتها() ثابت { أرجع {انتها_4}; }

	bool عملگر==(ثابت IntRange &other) ثابت { أرجع شروع_2 == other.شروع_2 && انتها_4 == other.انتها_4; }
	bool عملگر!=(ثابت IntRange &other) ثابت { أرجع !(*this == other); }
};

YOSYS_NAMESPACE_انتها

#پایان_اگر
