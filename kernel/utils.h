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

#جے_بنا_تعریف UTILS_H
#تعریف_کرو UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

سانچا<typename کلید_2, typename T>
بنية stackmap
{
خاص:
	std::سمتیہ<dict<کلید_2, T*>> backup_state;
	dict<کلید_2, T> current_state;
	ساكن T خالی_تھاواں_tuple;

عام:
	stackmap() { }
	stackmap(ثابت dict<کلید_2, T> &other) : current_state(other) { }

	سانچا<typename Other>
	فراغ عامل=(ثابت Other &other)
	{
		لكل (خودکار &it : current_state)
			إذا (!backup_state.خالی_تھاواں() && backup_state.پٹھ().گنتی(it.پہلا) == 0)
				backup_state.پٹھ()[it.پہلا] = new T(it.دوجا);
		current_state.clear();

		لكل (خودکار &it : other)
			سیٹ(it.پہلا, it.دوجا);
	}

	bool has(ثابت کلید_2 &k)
	{
		أرجع current_state.گنتی(k) != 0;
	}

	فراغ سیٹ(ثابت کلید_2 &k, ثابت T &v)
	{
		إذا (!backup_state.خالی_تھاواں() && backup_state.پٹھ().گنتی(k) == 0)
			backup_state.پٹھ()[k] = current_state.گنتی(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	فراغ unset(ثابت کلید_2 &k)
	{
		إذا (!backup_state.خالی_تھاواں() && backup_state.پٹھ().گنتی(k) == 0)
			backup_state.پٹھ()[k] = current_state.گنتی(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ثابت T &get(ثابت کلید_2 &k)
	{
		إذا (current_state.گنتی(k) == 0)
			أرجع خالی_تھاواں_tuple;
		أرجع current_state.at(k);
	}

	فراغ reset(ثابت کلید_2 &k)
	{
		لكل (صحيح i = Get_ناپ(backup_state)-1; i >= 0; i--)
			إذا (backup_state[i].گنتی(k) != 0) {
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
		backup_state.resize(backup_state.ناپ()+1);
	}

	فراغ restore()
	{
		log_assert(!backup_state.خالی_تھاواں());
		لكل (خودکار &it : backup_state.پٹھ())
			إذا (it.دوجا != nullptr) {
				current_state[it.پہلا] = *it.دوجا;
				delete it.دوجا;
			} وإلا
				current_state.erase(it.پہلا);
		backup_state.نکالو_پٹھ();
	}

	~stackmap()
	{
		طالما (!backup_state.خالی_تھاواں())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

سانچا <typename T, typename C = std::less<T>> class Topo_ترتیب_دیو
{
	static_assert(!(std::is_اشارہ_گر<T>::قدر && std::is_same<C, std::less<T>>::قدر),
			"std::less is run-to-run unstable for pointers");
عام:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	بنية اسدھا_Cmp {
		اسدھا_Cmp(ثابت std::سمتیہ<T> &nodes, ثابت C &گرہ_cmp) : گرہ_cmp_2(گرہ_cmp), nodes_(nodes) {}
		bool عامل()(صحيح a, صحيح b) ثابت
		{
			log_assert(static_cast<ناپ_t>(a) < nodes_.ناپ());
			log_assert(static_cast<ناپ_t>(b) < nodes_.ناپ());
			أرجع گرہ_cmp_2(nodes_[a], nodes_[b]);
		}
		ثابت C گرہ_cmp_2;
		ثابت std::سمتیہ<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, صحيح, C> گرہ_منزل_اشاریہ;
	// edges[i] is the set of nodes with an edge into node i
	std::سمتیہ<std::سیٹ<صحيح, اسدھا_Cmp>> کنارے;
	std::سمتیہ<T> sorted;
	std::سیٹ<std::سمتیہ<T>> loops;

	Topo_ترتیب_دیو() requires std::is_default_constructible_v<C> : Topo_ترتیب_دیو(C()) {}
	explicit Topo_ترتیب_دیو(ثابت C &cmp) : گرہ_منزل_اشاریہ(cmp), اسدھا_cmp(nodes, cmp)
	{
		analyze_loops = true;
		لبھیا_loops = false;
	}

	صحيح گرہ(T n)
	{
		خودکار rv = گرہ_منزل_اشاریہ.emplace(n, static_cast<صحيح>(nodes.ناپ()));
		إذا (rv.دوجا) {
			nodes.ڈالو_پٹھ(n);
			کنارے.ڈالو_پٹھ(std::سیٹ<صحيح, اسدھا_Cmp>(اسدھا_cmp));
		}
		أرجع rv.پہلا->دوجا;
	}

	فراغ کنارا(صحيح l_اشاریہ, صحيح r_اشاریہ) { کنارے[r_اشاریہ].داخل_کرو(l_اشاریہ); }

	فراغ کنارا(T بایاں_2, T دایاں_2) { کنارا(گرہ(بایاں_2), گرہ(دایاں_2)); }

	bool has_گرہ(ثابت T &گرہ) { أرجع گرہ_منزل_اشاریہ.لبھو(گرہ) != گرہ_منزل_اشاریہ.سرا(); }

	bool ترتیب_دیو()
	{
		log_assert(Get_ناپ(گرہ_منزل_اشاریہ) == Get_ناپ(کنارے));
		log_assert(Get_ناپ(nodes) == Get_ناپ(کنارے));

		loops.clear();
		sorted.clear();
		لبھیا_loops = false;

		std::سمتیہ<bool> گرہ_is_sorted(کنارے.ناپ(), false);
		std::سمتیہ<bool> گرہ_is_on_ڈھیر(کنارے.ناپ(), false);
		// Only used with analyze_loops
		std::سمتیہ<صحيح> ڈھیر;
		sorted.reserve(کنارے.ناپ());

		لكل (ثابت خودکار &it : گرہ_منزل_اشاریہ)
			ترتیب_دیو_کامہ(it.دوجا, گرہ_is_sorted, گرہ_is_on_ڈھیر, ڈھیر);

		log_assert(Get_ناپ(sorted) == Get_ناپ(nodes));

		أرجع !لبھیا_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::سیٹ<T, C>, C> get_database()
	{
		std::map<T, std::سیٹ<T, C>, C> database(گرہ_منزل_اشاریہ.کلید_comp());
		لكل (ناپ_t i = 0; i < nodes.ناپ(); ++i) {
			std::سیٹ<T, C> converted_کنارا_سیٹ(گرہ_منزل_اشاریہ.کلید_comp());
			لكل (صحيح other_گرہ : کنارے[i]) {
				converted_کنارا_سیٹ.داخل_کرو(nodes[other_گرہ]);
			}
			database.emplace(nodes[i], converted_کنارا_سیٹ);
		}
		أرجع database;
	}

خاص:
	bool لبھیا_loops;
	std::سمتیہ<T> nodes;
	ثابت اسدھا_Cmp اسدھا_cmp;

	فراغ ترتیب_دیو_کامہ(ثابت صحيح جڑ_اشاریہ, std::سمتیہ<bool> &گرہ_is_sorted, std::سمتیہ<bool> &گرہ_is_on_ڈھیر, std::سمتیہ<صحيح> &ڈھیر)
	{
		إذا (گرہ_is_on_ڈھیر[جڑ_اشاریہ]) {
			// We've been here before, meaning we have a loop
			لبھیا_loops = true;
			إذا (analyze_loops) {
				std::سمتیہ<T> loop;
				لكل (صحيح i = Get_ناپ(ڈھیر) - 1; i >= 0; i--) {
					ثابت صحيح اشاریہ = ڈھیر[i];
					loop.ڈالو_پٹھ(nodes[اشاریہ]);
					إذا (اشاریہ == جڑ_اشاریہ)
						اكسر;
				}
				loops.داخل_کرو(loop);
			}
			أرجع;
		}

		// We're done if we've already sorted this subgraph
		إذا (گرہ_is_sorted[جڑ_اشاریہ])
			أرجع;

		إذا (!کنارے[جڑ_اشاریہ].خالی_تھاواں()) {
			إذا (analyze_loops)
				ڈھیر.ڈالو_پٹھ(جڑ_اشاریہ);
			گرہ_is_on_ڈھیر[جڑ_اشاریہ] = true;

			لكل (صحيح بایاں_n : کنارے[جڑ_اشاریہ])
				ترتیب_دیو_کامہ(بایاں_n, گرہ_is_sorted, گرہ_is_on_ڈھیر, ڈھیر);

			إذا (analyze_loops)
				ڈھیر.نکالو_پٹھ();
			گرہ_is_on_ڈھیر[جڑ_اشاریہ] = false;
		}

		گرہ_is_sorted[جڑ_اشاریہ] = true;
		sorted.ڈالو_پٹھ(nodes[جڑ_اشاریہ]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
سانچا<class T>
بنية arrow_proxy {
	T v;
	explicit arrow_proxy(T ثابت & v) : v(v) {}
	T* عامل->() { أرجع &v; }
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
#جے_انت
}

سانچا <typename T>
خودکار reversed(T& container) {
	بنية الٹ_view {
		الٹ_view(T& container) : container(container) {}
		خودکار begin() ثابت { أرجع container.rbegin(); }
		خودکار سرا() ثابت { أرجع container.rend(); }
		T& container;
	};
	أرجع الٹ_view{container};
}

سانچا <typename T>
خودکار reversed(ثابت T& container) {
	بنية الٹ_view {
		الٹ_view(ثابت T& container) : container(container) {}
		خودکار begin() ثابت { أرجع container.rbegin(); }
		خودکار سرا() ثابت { أرجع container.rend(); }
		ثابت T& container;
	};
	أرجع الٹ_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
بنية IntRange {
	صحيح شروع_2;
	صحيح سرا_4;
	بنية Int {
		صحيح v;
		صحيح عامل*() ثابت { أرجع v; }
		Int &عامل++() { ++v; أرجع *this; }
		bool عامل!=(ثابت Int &other) ثابت { أرجع v != other.v; }
	};
	Int begin() ثابت { أرجع {شروع_2}; }
	Int سرا() ثابت { أرجع {سرا_4}; }

	bool عامل==(ثابت IntRange &other) ثابت { أرجع شروع_2 == other.شروع_2 && سرا_4 == other.سرا_4; }
	bool عامل!=(ثابت IntRange &other) ثابت { أرجع !(*this == other); }
};

YOSYS_NAMESPACE_سرا

#جے_انت
