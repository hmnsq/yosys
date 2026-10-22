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

#جيڪڏهن_وصف_نه_ٿيل UTILS_H
#وصف_ڪريو UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

سانچو<typename ڪنجي_2, typename T>
بنية stackmap
{
خاص:
	std::ويڪٽر<dict<ڪنجي_2, T*>> backup_state;
	dict<ڪنجي_2, T> current_state;
	ساكن T خالي_جايون_tuple;

عام:
	stackmap() { }
	stackmap(ثابت dict<ڪنجي_2, T> &other) : current_state(other) { }

	سانچو<typename Other>
	فراغ عامل=(ثابت Other &other)
	{
		لكل (خودڪار &it : current_state)
			إذا (!backup_state.خالي_جايون() && backup_state.پٺ().ڳڻپ(it.پهريون) == 0)
				backup_state.پٺ()[it.پهريون] = new T(it.ٻيو);
		current_state.clear();

		لكل (خودڪار &it : other)
			سيٽ(it.پهريون, it.ٻيو);
	}

	bool has(ثابت ڪنجي_2 &k)
	{
		أرجع current_state.ڳڻپ(k) != 0;
	}

	فراغ سيٽ(ثابت ڪنجي_2 &k, ثابت T &v)
	{
		إذا (!backup_state.خالي_جايون() && backup_state.پٺ().ڳڻپ(k) == 0)
			backup_state.پٺ()[k] = current_state.ڳڻپ(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	فراغ unset(ثابت ڪنجي_2 &k)
	{
		إذا (!backup_state.خالي_جايون() && backup_state.پٺ().ڳڻپ(k) == 0)
			backup_state.پٺ()[k] = current_state.ڳڻپ(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ثابت T &get(ثابت ڪنجي_2 &k)
	{
		إذا (current_state.ڳڻپ(k) == 0)
			أرجع خالي_جايون_tuple;
		أرجع current_state.at(k);
	}

	فراغ reset(ثابت ڪنجي_2 &k)
	{
		لكل (صحيح i = Get_سائيز(backup_state)-1; i >= 0; i--)
			إذا (backup_state[i].ڳڻپ(k) != 0) {
				إذا (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				وإلا
					current_state[k] = *backup_state[i].at(k);
				أرجع;
			}
		current_state.erase(k);
	}

	ثابت dict<ڪنجي_2, T> &stdmap()
	{
		أرجع current_state;
	}

	فراغ save()
	{
		backup_state.resize(backup_state.سائيز()+1);
	}

	فراغ restore()
	{
		log_assert(!backup_state.خالي_جايون());
		لكل (خودڪار &it : backup_state.پٺ())
			إذا (it.ٻيو != nullptr) {
				current_state[it.پهريون] = *it.ٻيو;
				delete it.ٻيو;
			} وإلا
				current_state.erase(it.پهريون);
		backup_state.ڪڍو_پٺ();
	}

	~stackmap()
	{
		طالما (!backup_state.خالي_جايون())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

سانچو <typename T, typename C = std::less<T>> class Topo_ترتيب_ڏيو
{
	static_assert(!(std::is_اشارو<T>::قدر && std::is_same<C, std::less<T>>::قدر),
			"std::less is run-to-run unstable for pointers");
عام:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	بنية اڻسڌو_Cmp {
		اڻسڌو_Cmp(ثابت std::ويڪٽر<T> &nodes, ثابت C &ڳنڍ_cmp) : ڳنڍ_cmp_2(ڳنڍ_cmp), nodes_(nodes) {}
		bool عامل()(صحيح a, صحيح b) ثابت
		{
			log_assert(static_cast<سائيز_t>(a) < nodes_.سائيز());
			log_assert(static_cast<سائيز_t>(b) < nodes_.سائيز());
			أرجع ڳنڍ_cmp_2(nodes_[a], nodes_[b]);
		}
		ثابت C ڳنڍ_cmp_2;
		ثابت std::ويڪٽر<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, صحيح, C> ڳنڍ_منزل_اشاريو;
	// edges[i] is the set of nodes with an edge into node i
	std::ويڪٽر<std::سيٽ<صحيح, اڻسڌو_Cmp>> ڪنارا;
	std::ويڪٽر<T> sorted;
	std::سيٽ<std::ويڪٽر<T>> loops;

	Topo_ترتيب_ڏيو() requires std::is_default_constructible_v<C> : Topo_ترتيب_ڏيو(C()) {}
	explicit Topo_ترتيب_ڏيو(ثابت C &cmp) : ڳنڍ_منزل_اشاريو(cmp), اڻسڌو_cmp(nodes, cmp)
	{
		analyze_loops = true;
		مليو_loops = false;
	}

	صحيح ڳنڍ(T n)
	{
		خودڪار rv = ڳنڍ_منزل_اشاريو.emplace(n, static_cast<صحيح>(nodes.سائيز()));
		إذا (rv.ٻيو) {
			nodes.ڌڪو_پٺ(n);
			ڪنارا.ڌڪو_پٺ(std::سيٽ<صحيح, اڻسڌو_Cmp>(اڻسڌو_cmp));
		}
		أرجع rv.پهريون->ٻيو;
	}

	فراغ ڪنارو(صحيح l_اشاريو, صحيح r_اشاريو) { ڪنارا[r_اشاريو].وجهو(l_اشاريو); }

	فراغ ڪنارو(T کاٻو_2, T ساڄو_2) { ڪنارو(ڳنڍ(کاٻو_2), ڳنڍ(ساڄو_2)); }

	bool has_ڳنڍ(ثابت T &ڳنڍ) { أرجع ڳنڍ_منزل_اشاريو.ڳوليو(ڳنڍ) != ڳنڍ_منزل_اشاريو.پڇاڙي(); }

	bool ترتيب_ڏيو()
	{
		log_assert(Get_سائيز(ڳنڍ_منزل_اشاريو) == Get_سائيز(ڪنارا));
		log_assert(Get_سائيز(nodes) == Get_سائيز(ڪنارا));

		loops.clear();
		sorted.clear();
		مليو_loops = false;

		std::ويڪٽر<bool> ڳنڍ_is_sorted(ڪنارا.سائيز(), false);
		std::ويڪٽر<bool> ڳنڍ_is_on_ڍير(ڪنارا.سائيز(), false);
		// Only used with analyze_loops
		std::ويڪٽر<صحيح> ڍير;
		sorted.reserve(ڪنارا.سائيز());

		لكل (ثابت خودڪار &it : ڳنڍ_منزل_اشاريو)
			ترتيب_ڏيو_ڪم_ڪندڙ(it.ٻيو, ڳنڍ_is_sorted, ڳنڍ_is_on_ڍير, ڍير);

		log_assert(Get_سائيز(sorted) == Get_سائيز(nodes));

		أرجع !مليو_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::سيٽ<T, C>, C> get_database()
	{
		std::map<T, std::سيٽ<T, C>, C> database(ڳنڍ_منزل_اشاريو.ڪنجي_comp());
		لكل (سائيز_t i = 0; i < nodes.سائيز(); ++i) {
			std::سيٽ<T, C> converted_ڪنارو_سيٽ(ڳنڍ_منزل_اشاريو.ڪنجي_comp());
			لكل (صحيح other_ڳنڍ : ڪنارا[i]) {
				converted_ڪنارو_سيٽ.وجهو(nodes[other_ڳنڍ]);
			}
			database.emplace(nodes[i], converted_ڪنارو_سيٽ);
		}
		أرجع database;
	}

خاص:
	bool مليو_loops;
	std::ويڪٽر<T> nodes;
	ثابت اڻسڌو_Cmp اڻسڌو_cmp;

	فراغ ترتيب_ڏيو_ڪم_ڪندڙ(ثابت صحيح پاڙ_اشاريو, std::ويڪٽر<bool> &ڳنڍ_is_sorted, std::ويڪٽر<bool> &ڳنڍ_is_on_ڍير, std::ويڪٽر<صحيح> &ڍير)
	{
		إذا (ڳنڍ_is_on_ڍير[پاڙ_اشاريو]) {
			// We've been here before, meaning we have a loop
			مليو_loops = true;
			إذا (analyze_loops) {
				std::ويڪٽر<T> loop;
				لكل (صحيح i = Get_سائيز(ڍير) - 1; i >= 0; i--) {
					ثابت صحيح اشاريو = ڍير[i];
					loop.ڌڪو_پٺ(nodes[اشاريو]);
					إذا (اشاريو == پاڙ_اشاريو)
						اكسر;
				}
				loops.وجهو(loop);
			}
			أرجع;
		}

		// We're done if we've already sorted this subgraph
		إذا (ڳنڍ_is_sorted[پاڙ_اشاريو])
			أرجع;

		إذا (!ڪنارا[پاڙ_اشاريو].خالي_جايون()) {
			إذا (analyze_loops)
				ڍير.ڌڪو_پٺ(پاڙ_اشاريو);
			ڳنڍ_is_on_ڍير[پاڙ_اشاريو] = true;

			لكل (صحيح کاٻو_n : ڪنارا[پاڙ_اشاريو])
				ترتيب_ڏيو_ڪم_ڪندڙ(کاٻو_n, ڳنڍ_is_sorted, ڳنڍ_is_on_ڍير, ڍير);

			إذا (analyze_loops)
				ڍير.ڪڍو_پٺ();
			ڳنڍ_is_on_ڍير[پاڙ_اشاريو] = false;
		}

		ڳنڍ_is_sorted[پاڙ_اشاريو] = true;
		sorted.ڌڪو_پٺ(nodes[پاڙ_اشاريو]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
سانچو<class T>
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
#جيڪڏهن_پورو
}

سانچو <typename T>
خودڪار reversed(T& container) {
	بنية ابتو_view {
		ابتو_view(T& container) : container(container) {}
		خودڪار begin() ثابت { أرجع container.rbegin(); }
		خودڪار پڇاڙي() ثابت { أرجع container.rend(); }
		T& container;
	};
	أرجع ابتو_view{container};
}

سانچو <typename T>
خودڪار reversed(ثابت T& container) {
	بنية ابتو_view {
		ابتو_view(ثابت T& container) : container(container) {}
		خودڪار begin() ثابت { أرجع container.rbegin(); }
		خودڪار پڇاڙي() ثابت { أرجع container.rend(); }
		ثابت T& container;
	};
	أرجع ابتو_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
بنية IntRange {
	صحيح شروعات_2;
	صحيح پڇاڙي_4;
	بنية Int {
		صحيح v;
		صحيح عامل*() ثابت { أرجع v; }
		Int &عامل++() { ++v; أرجع *this; }
		bool عامل!=(ثابت Int &other) ثابت { أرجع v != other.v; }
	};
	Int begin() ثابت { أرجع {شروعات_2}; }
	Int پڇاڙي() ثابت { أرجع {پڇاڙي_4}; }

	bool عامل==(ثابت IntRange &other) ثابت { أرجع شروعات_2 == other.شروعات_2 && پڇاڙي_4 == other.پڇاڙي_4; }
	bool عامل!=(ثابت IntRange &other) ثابت { أرجع !(*this == other); }
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
