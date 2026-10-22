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

#شامل "kernel/yosys.h"
#شامل <iterator>

#اگر_غیر_تعریف_شدہ UTILS_H
#تعریف_کرو UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

سانچہ<typename کلید_2, typename T>
ساخت stackmap
{
نجی:
	std::سمتیہ<dict<کلید_2, T*>> backup_state;
	dict<کلید_2, T> current_state;
	ساکن T خالی_خانے_tuple;

عوامی:
	stackmap() { }
	stackmap(ثابت dict<کلید_2, T> &other) : current_state(other) { }

	سانچہ<typename Other>
	خالی عامل=(ثابت Other &other)
	{
		برائے (خودکار &it : current_state)
			اگر (!backup_state.خالی_خانے() && backup_state.عقب().گنتی(it.پہلا) == 0)
				backup_state.عقب()[it.پہلا] = new T(it.دوسرا);
		current_state.clear();

		برائے (خودکار &it : other)
			سیٹ(it.پہلا, it.دوسرا);
	}

	bool has(ثابت کلید_2 &k)
	{
		واپس current_state.گنتی(k) != 0;
	}

	خالی سیٹ(ثابت کلید_2 &k, ثابت T &v)
	{
		اگر (!backup_state.خالی_خانے() && backup_state.عقب().گنتی(k) == 0)
			backup_state.عقب()[k] = current_state.گنتی(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	خالی unset(ثابت کلید_2 &k)
	{
		اگر (!backup_state.خالی_خانے() && backup_state.عقب().گنتی(k) == 0)
			backup_state.عقب()[k] = current_state.گنتی(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ثابت T &get(ثابت کلید_2 &k)
	{
		اگر (current_state.گنتی(k) == 0)
			واپس خالی_خانے_tuple;
		واپس current_state.at(k);
	}

	خالی reset(ثابت کلید_2 &k)
	{
		برائے (صحیح i = Get_سائز(backup_state)-1; i >= 0; i--)
			اگر (backup_state[i].گنتی(k) != 0) {
				اگر (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				ورنہ
					current_state[k] = *backup_state[i].at(k);
				واپس;
			}
		current_state.erase(k);
	}

	ثابت dict<کلید_2, T> &stdmap()
	{
		واپس current_state;
	}

	خالی save()
	{
		backup_state.resize(backup_state.سائز()+1);
	}

	خالی restore()
	{
		log_assert(!backup_state.خالی_خانے());
		برائے (خودکار &it : backup_state.عقب())
			اگر (it.دوسرا != nullptr) {
				current_state[it.پہلا] = *it.دوسرا;
				delete it.دوسرا;
			} ورنہ
				current_state.erase(it.پہلا);
		backup_state.نکالو_عقب();
	}

	~stackmap()
	{
		جب (!backup_state.خالی_خانے())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

سانچہ <typename T, typename C = std::less<T>> class Topo_ترتیب_دو
{
	static_assert(!(std::is_اشارہ_گر<T>::قدر && std::is_same<C, std::less<T>>::قدر),
			"std::less is run-to-run unstable for pointers");
عوامی:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	ساخت بالواسطہ_Cmp {
		بالواسطہ_Cmp(ثابت std::سمتیہ<T> &nodes, ثابت C &گرہ_cmp) : گرہ_cmp_2(گرہ_cmp), nodes_(nodes) {}
		bool عامل()(صحیح a, صحیح b) ثابت
		{
			log_assert(static_cast<سائز_t>(a) < nodes_.سائز());
			log_assert(static_cast<سائز_t>(b) < nodes_.سائز());
			واپس گرہ_cmp_2(nodes_[a], nodes_[b]);
		}
		ثابت C گرہ_cmp_2;
		ثابت std::سمتیہ<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, صحیح, C> گرہ_مقصد_اشاریہ;
	// edges[i] is the set of nodes with an edge into node i
	std::سمتیہ<std::سیٹ<صحیح, بالواسطہ_Cmp>> کنارے;
	std::سمتیہ<T> sorted;
	std::سیٹ<std::سمتیہ<T>> loops;

	Topo_ترتیب_دو() requires std::is_default_constructible_v<C> : Topo_ترتیب_دو(C()) {}
	explicit Topo_ترتیب_دو(ثابت C &cmp) : گرہ_مقصد_اشاریہ(cmp), بالواسطہ_cmp(nodes, cmp)
	{
		analyze_loops = true;
		ملا_loops = false;
	}

	صحیح گرہ(T n)
	{
		خودکار rv = گرہ_مقصد_اشاریہ.emplace(n, static_cast<صحیح>(nodes.سائز()));
		اگر (rv.دوسرا) {
			nodes.ڈالو_عقب(n);
			کنارے.ڈالو_عقب(std::سیٹ<صحیح, بالواسطہ_Cmp>(بالواسطہ_cmp));
		}
		واپس rv.پہلا->دوسرا;
	}

	خالی کنارہ(صحیح l_اشاریہ, صحیح r_اشاریہ) { کنارے[r_اشاریہ].داخل_کرو(l_اشاریہ); }

	خالی کنارہ(T بایاں_2, T دایاں_2) { کنارہ(گرہ(بایاں_2), گرہ(دایاں_2)); }

	bool has_گرہ(ثابت T &گرہ) { واپس گرہ_مقصد_اشاریہ.ڈھونڈو(گرہ) != گرہ_مقصد_اشاریہ.انتہا(); }

	bool ترتیب_دو()
	{
		log_assert(Get_سائز(گرہ_مقصد_اشاریہ) == Get_سائز(کنارے));
		log_assert(Get_سائز(nodes) == Get_سائز(کنارے));

		loops.clear();
		sorted.clear();
		ملا_loops = false;

		std::سمتیہ<bool> گرہ_is_sorted(کنارے.سائز(), false);
		std::سمتیہ<bool> گرہ_is_on_ڈھیر(کنارے.سائز(), false);
		// Only used with analyze_loops
		std::سمتیہ<صحیح> ڈھیر;
		sorted.reserve(کنارے.سائز());

		برائے (ثابت خودکار &it : گرہ_مقصد_اشاریہ)
			ترتیب_دو_کارکن(it.دوسرا, گرہ_is_sorted, گرہ_is_on_ڈھیر, ڈھیر);

		log_assert(Get_سائز(sorted) == Get_سائز(nodes));

		واپس !ملا_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::سیٹ<T, C>, C> get_database()
	{
		std::map<T, std::سیٹ<T, C>, C> database(گرہ_مقصد_اشاریہ.کلید_comp());
		برائے (سائز_t i = 0; i < nodes.سائز(); ++i) {
			std::سیٹ<T, C> converted_کنارہ_سیٹ(گرہ_مقصد_اشاریہ.کلید_comp());
			برائے (صحیح other_گرہ : کنارے[i]) {
				converted_کنارہ_سیٹ.داخل_کرو(nodes[other_گرہ]);
			}
			database.emplace(nodes[i], converted_کنارہ_سیٹ);
		}
		واپس database;
	}

نجی:
	bool ملا_loops;
	std::سمتیہ<T> nodes;
	ثابت بالواسطہ_Cmp بالواسطہ_cmp;

	خالی ترتیب_دو_کارکن(ثابت صحیح جڑ_اشاریہ, std::سمتیہ<bool> &گرہ_is_sorted, std::سمتیہ<bool> &گرہ_is_on_ڈھیر, std::سمتیہ<صحیح> &ڈھیر)
	{
		اگر (گرہ_is_on_ڈھیر[جڑ_اشاریہ]) {
			// We've been here before, meaning we have a loop
			ملا_loops = true;
			اگر (analyze_loops) {
				std::سمتیہ<T> loop;
				برائے (صحیح i = Get_سائز(ڈھیر) - 1; i >= 0; i--) {
					ثابت صحیح اشاریہ = ڈھیر[i];
					loop.ڈالو_عقب(nodes[اشاریہ]);
					اگر (اشاریہ == جڑ_اشاریہ)
						توڑو;
				}
				loops.داخل_کرو(loop);
			}
			واپس;
		}

		// We're done if we've already sorted this subgraph
		اگر (گرہ_is_sorted[جڑ_اشاریہ])
			واپس;

		اگر (!کنارے[جڑ_اشاریہ].خالی_خانے()) {
			اگر (analyze_loops)
				ڈھیر.ڈالو_عقب(جڑ_اشاریہ);
			گرہ_is_on_ڈھیر[جڑ_اشاریہ] = true;

			برائے (صحیح بایاں_n : کنارے[جڑ_اشاریہ])
				ترتیب_دو_کارکن(بایاں_n, گرہ_is_sorted, گرہ_is_on_ڈھیر, ڈھیر);

			اگر (analyze_loops)
				ڈھیر.نکالو_عقب();
			گرہ_is_on_ڈھیر[جڑ_اشاریہ] = false;
		}

		گرہ_is_sorted[جڑ_اشاریہ] = true;
		sorted.ڈالو_عقب(nodes[جڑ_اشاریہ]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
سانچہ<class T>
ساخت arrow_proxy {
	T v;
	explicit arrow_proxy(T ثابت & v) : v(v) {}
	T* عامل->() { واپس &v; }
};

اندرونی صحیح ceil_log2(صحیح x)
{
#اگر defined(__GNUC__)
        واپس x > 1 ? (8*حجم(صحیح)) - __builtin_clz(x-1) : 0;
#ورنہ
	اگر (x <= 0)
		واپس 0;
	برائے (صحیح i = 0; i < 32; i++)
		اگر (((x-1) >> i) == 0)
			واپس i;
	log_abort();
#اگر_ختم
}

سانچہ <typename T>
خودکار reversed(T& container) {
	ساخت الٹ_view {
		الٹ_view(T& container) : container(container) {}
		خودکار begin() ثابت { واپس container.rbegin(); }
		خودکار انتہا() ثابت { واپس container.rend(); }
		T& container;
	};
	واپس الٹ_view{container};
}

سانچہ <typename T>
خودکار reversed(ثابت T& container) {
	ساخت الٹ_view {
		الٹ_view(ثابت T& container) : container(container) {}
		خودکار begin() ثابت { واپس container.rbegin(); }
		خودکار انتہا() ثابت { واپس container.rend(); }
		ثابت T& container;
	};
	واپس الٹ_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
ساخت IntRange {
	صحیح آغاز_2;
	صحیح انتہا_4;
	ساخت Int {
		صحیح v;
		صحیح عامل*() ثابت { واپس v; }
		Int &عامل++() { ++v; واپس *this; }
		bool عامل!=(ثابت Int &other) ثابت { واپس v != other.v; }
	};
	Int begin() ثابت { واپس {آغاز_2}; }
	Int انتہا() ثابت { واپس {انتہا_4}; }

	bool عامل==(ثابت IntRange &other) ثابت { واپس آغاز_2 == other.آغاز_2 && انتہا_4 == other.انتہا_4; }
	bool عامل!=(ثابت IntRange &other) ثابت { واپس !(*this == other); }
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
