#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#অন্তর্ভাব "kernel/hashcons.h"

#অন্তর্ভাব <algorithm>

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename গ্রন্থি_2, typename Ref>
bool HashConsPool<Derived, গ্রন্থি_2, Ref>::is_live(Ref ref) ধ্রুবক {
	Ref idx = ref.untag();
	যদি (is_static(idx))
		ফেরত true;
	আকার_t slot = idx.raw() - Derived::STATIC_গুনতি;
	ফেরত slot < backing.আকার_2() && !backing[slot].is_dead();
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::check_ready() {}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
ধ্রুবক গ্রন্থি_2& HashConsPool<Derived, গ্রন্থি_2, Ref>::operator[] (Ref ref) ধ্রুবক {
	Ref idx = ref.untag();
	যদি constexpr (Derived::STATIC_গুনতি != 0) {
		যদি (is_static(idx))
			ফেরত Derived::static_গ্রন্থি(idx.raw());
	}
	ফেরত backing[idx.raw() - Derived::STATIC_গুনতি];
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
Derived* HashConsPool<Derived, গ্রন্থি_2, Ref>::self() { ফেরত static_cast<Derived*>(this); }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
ধ্রুবক Derived* HashConsPool<Derived, গ্রন্থি_2, Ref>::self() ধ্রুবক { ফেরত static_cast<ধ্রুবক Derived*>(this); }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
bool HashConsPool<Derived, গ্রন্থি_2, Ref>::is_static(Ref ref) {
	যদি constexpr (Derived::STATIC_গুনতি == 0)
		ফেরত false;
	অন্যথা
		ফেরত ref.raw() < Derived::STATIC_গুনতি;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::RefIterator::settle() {
	যখন (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
Ref HashConsPool<Derived, গ্রন্থি_2, Ref>::RefIterator::operator*() ধ্রুবক { ফেরত Ref(idx); }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্রন্থি_2, Ref>::RefIterator::operator++() -> RefIterator& { idx++; settle(); ফেরত *this; }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
bool HashConsPool<Derived, গ্রন্থি_2, Ref>::RefIterator::operator!=(ধ্রুবক RefIterator& other) ধ্রুবক { ফেরত idx != other.idx; }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্রন্থি_2, Ref>::RefRange::begin() ধ্রুবক -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	ফেরত it;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্রন্থি_2, Ref>::RefRange::end() ধ্রুবক -> RefIterator { ফেরত RefIterator{pool, stop, stop, skip_dead}; }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্রন্থি_2, Ref>::refs(bool include_statics) ধ্রুবক -> RefRange {
	ফেরত RefRange{this, include_statics ? 0 : Derived::STATIC_গুনতি,
			Derived::STATIC_গুনতি + backing.আকার_2(), true};
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্রন্থি_2, Ref>::slots() ধ্রুবক -> RefRange {
	ফেরত RefRange{this, Derived::STATIC_গুনতি,
			Derived::STATIC_গুনতি + backing.আকার_2(), false};
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
আকার_t HashConsPool<Derived, গ্রন্থি_2, Ref>::home_slot(uint64_t hash) ধ্রুবক {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	ফেরত (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.আকার_2()));
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
আকার_t HashConsPool<Derived, গ্রন্থি_2, Ref>::পরবর্তী_slot(আকার_t slot) ধ্রুবক { ফেরত (slot + 1) & (table.আকার_2() - 1); }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
আকার_t HashConsPool<Derived, গ্রন্থি_2, Ref>::আকার_2() ধ্রুবক { ফেরত backing.আকার_2() - free_list.আকার_2(); }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
bool HashConsPool<Derived, গ্রন্থি_2, Ref>::slot_live(আকার_t abs) ধ্রুবক {
	যদি (abs < Derived::STATIC_গুনতি)
		ফেরত true;
	ফেরত !backing[abs - Derived::STATIC_গুনতি].is_dead();
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
HashConsPool<Derived, গ্রন্থি_2, Ref>::HashConsPool() { rebuild_সূচক(); }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
HashConsPool<Derived, গ্রন্থি_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
HashConsPool<Derived, গ্রন্থি_2, Ref>& HashConsPool<Derived, গ্রন্থি_2, Ref>::operator=(HashConsPool&& other) {
	যদি (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	ফেরত *this;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_সূচক();
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::rebuild_সূচক() {
	Derived::check_ready();
	free_list.clear();
	জন্য (আকার_t idx = 0; idx < backing.আকার_2(); ++idx)
		যদি (backing[idx].is_dead())
			free_list.ঠেলো_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<আকার_t>());
	table.assign(std::bit_ceil((Derived::STATIC_গুনতি + আকার_2()) * 2 + 2), Ref());
	জন্য (Ref ref : refs())
		সূচক_ঢোকাও(ref);
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
template<typename Eq>
Ref HashConsPool<Derived, গ্রন্থি_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) ধ্রুবক {
	জন্য (আকার_t slot = home_slot(hash); table[slot] != Ref(); slot = পরবর্তী_slot(slot)) {
		Ref ref = table[slot];
		যদি (Derived::hash_গ্রন্থি((*this)[ref]) == hash && eq(ref))
			ফেরত ref;
	}
	ফেরত Ref();
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::সূচক_ঢোকাও(Ref ref) {
	আকার_t slot = home_slot(Derived::hash_গ্রন্থি((*this)[ref]));
	যখন (table[slot] != Ref())
		slot = পরবর্তী_slot(slot);
	table[slot] = ref;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::সূচক_erase(Ref ref) {
	আকার_t hole = home_slot(Derived::hash_গ্রন্থি((*this)[ref]));
	যখন (table[hole] != ref)
		hole = পরবর্তী_slot(hole);
	আকার_t mask = table.আকার_2() - 1;
	জন্য (আকার_t slot = পরবর্তী_slot(hole); table[slot] != Ref(); slot = পরবর্তী_slot(slot)) {
		আকার_t home = home_slot(Derived::hash_গ্রন্থি((*this)[table[slot]]));
		যদি (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::grow_সূচক() {
	std::vector<Ref> old(table.আকার_2() * 2, Ref());
	std::swap(old, table);
	জন্য (Ref ref : old)
		যদি (ref != Ref())
			সূচক_ঢোকাও(ref);
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
Ref HashConsPool<Derived, গ্রন্থি_2, Ref>::add_inner(গ্রন্থি_2 t) {
	Ref ref;
	যদি (!free_list.empty()) {
		আকার_t idx = free_list.back();
		free_list.বের_করো_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_গুনতি + idx);
	} অন্যথা {
		ref = Ref(Derived::STATIC_গুনতি + backing.আকার_2());
		backing.ঠেলো_back(std::move(t));
	}
	যদি ((Derived::STATIC_গুনতি + আকার_2()) * 3 > table.আকার_2() * 2)
		grow_সূচক();
	সূচক_ঢোকাও(ref);
	যদি (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	ফেরত ref;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
template<typename Roots>
আকার_t HashConsPool<Derived, গ্রন্থি_2, Ref>::gc(ধ্রুবক Roots& roots) {
	pool<Ref> live;
	জন্য (Ref ref : roots)
		mark_live(ref, live);
	আকার_t erased = 0;
	জন্য (আকার_t idx = 0; idx < backing.আকার_2(); ++idx) {
		যদি (backing[idx].is_dead())
			চালু;
		যদি (!live.গুনতি(Ref(Derived::STATIC_গুনতি + idx))) {
			সূচক_erase(Ref(Derived::STATIC_গুনতি + idx));
			free_list.ঠেলো_back(idx);
			backing[idx] = গ্রন্থি_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<আকার_t>());
	ফেরত erased;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::mark_live(Ref ref, pool<Ref>& live) ধ্রুবক {
	ref = ref.untag();
	যদি (ref == Ref() || is_static(ref) || !live.ঢোকাও(ref).second)
		ফেরত;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
