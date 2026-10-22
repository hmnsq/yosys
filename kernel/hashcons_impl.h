#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#অন্তর্ভাব "kernel/hashcons.h"

#অন্তর্ভাব <algorithm>

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename গ্রন্থি_2, typename Ref>
bool HashConsPool<Derived, গ্রন্থি_2, Ref>::is_live(Ref ref) ধ্রুব {
	Ref idx = ref.untag();
	যদি (is_static(idx))
		প্রত্যাগম true;
	size_t slot = idx.raw() - Derived::STATIC_গুনতি;
	প্রত্যাগম slot < backing.size() && !backing[slot].is_dead();
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::check_ready() {}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
ধ্রুব গ্রন্থি_2& HashConsPool<Derived, গ্রন্থি_2, Ref>::operator[] (Ref ref) ধ্রুব {
	Ref idx = ref.untag();
	যদি constexpr (Derived::STATIC_গুনতি != 0) {
		যদি (is_static(idx))
			প্রত্যাগম Derived::static_গ্রন্থি(idx.raw());
	}
	প্রত্যাগম backing[idx.raw() - Derived::STATIC_গুনতি];
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
Derived* HashConsPool<Derived, গ্রন্থি_2, Ref>::self() { প্রত্যাগম static_cast<Derived*>(this); }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
ধ্রুব Derived* HashConsPool<Derived, গ্রন্থি_2, Ref>::self() ধ্রুব { প্রত্যাগম static_cast<ধ্রুব Derived*>(this); }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
bool HashConsPool<Derived, গ্রন্থি_2, Ref>::is_static(Ref ref) {
	যদি constexpr (Derived::STATIC_গুনতি == 0)
		প্রত্যাগম false;
	অন্যথা
		প্রত্যাগম ref.raw() < Derived::STATIC_গুনতি;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::RefIterator::settle() {
	যাবত্ (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
Ref HashConsPool<Derived, গ্রন্থি_2, Ref>::RefIterator::operator*() ধ্রুব { প্রত্যাগম Ref(idx); }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্রন্থি_2, Ref>::RefIterator::operator++() -> RefIterator& { idx++; settle(); প্রত্যাগম *this; }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
bool HashConsPool<Derived, গ্রন্থি_2, Ref>::RefIterator::operator!=(ধ্রুব RefIterator& other) ধ্রুব { প্রত্যাগম idx != other.idx; }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্রন্থি_2, Ref>::RefRange::begin() ধ্রুব -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	প্রত্যাগম it;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্রন্থি_2, Ref>::RefRange::end() ধ্রুব -> RefIterator { প্রত্যাগম RefIterator{pool, stop, stop, skip_dead}; }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্রন্থি_2, Ref>::refs(bool include_statics) ধ্রুব -> RefRange {
	প্রত্যাগম RefRange{this, include_statics ? 0 : Derived::STATIC_গুনতি,
			Derived::STATIC_গুনতি + backing.size(), true};
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্রন্থি_2, Ref>::slots() ধ্রুব -> RefRange {
	প্রত্যাগম RefRange{this, Derived::STATIC_গুনতি,
			Derived::STATIC_গুনতি + backing.size(), false};
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
size_t HashConsPool<Derived, গ্রন্থি_2, Ref>::home_slot(uint64_t hash) ধ্রুব {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	প্রত্যাগম (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
size_t HashConsPool<Derived, গ্রন্থি_2, Ref>::next_slot(size_t slot) ধ্রুব { প্রত্যাগম (slot + 1) & (table.size() - 1); }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
size_t HashConsPool<Derived, গ্রন্থি_2, Ref>::size() ধ্রুব { প্রত্যাগম backing.size() - free_list.size(); }

template<typename Derived, typename গ্রন্থি_2, typename Ref>
bool HashConsPool<Derived, গ্রন্থি_2, Ref>::slot_live(size_t abs) ধ্রুব {
	যদি (abs < Derived::STATIC_গুনতি)
		প্রত্যাগম true;
	প্রত্যাগম !backing[abs - Derived::STATIC_গুনতি].is_dead();
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
HashConsPool<Derived, গ্রন্থি_2, Ref>::HashConsPool() { rebuild_index(); }

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
	প্রত্যাগম *this;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	প্রতি (size_t idx = 0; idx < backing.size(); ++idx)
		যদি (backing[idx].is_dead())
			free_list.ঠেলো_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	table.assign(std::bit_ceil((Derived::STATIC_গুনতি + size()) * 2 + 2), Ref());
	প্রতি (Ref ref : refs())
		index_ঢোকাও(ref);
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
template<typename Eq>
Ref HashConsPool<Derived, গ্রন্থি_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) ধ্রুব {
	প্রতি (size_t slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		যদি (Derived::hash_গ্রন্থি((*this)[ref]) == hash && eq(ref))
			প্রত্যাগম ref;
	}
	প্রত্যাগম Ref();
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::index_ঢোকাও(Ref ref) {
	size_t slot = home_slot(Derived::hash_গ্রন্থি((*this)[ref]));
	যাবত্ (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::index_erase(Ref ref) {
	size_t hole = home_slot(Derived::hash_গ্রন্থি((*this)[ref]));
	যাবত্ (table[hole] != ref)
		hole = next_slot(hole);
	size_t mask = table.size() - 1;
	প্রতি (size_t slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		size_t home = home_slot(Derived::hash_গ্রন্থি((*this)[table[slot]]));
		যদি (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	প্রতি (Ref ref : old)
		যদি (ref != Ref())
			index_ঢোকাও(ref);
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
Ref HashConsPool<Derived, গ্রন্থি_2, Ref>::add_inner(গ্রন্থি_2 t) {
	Ref ref;
	যদি (!free_list.empty()) {
		size_t idx = free_list.back();
		free_list.বের_করো_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_গুনতি + idx);
	} অন্যথা {
		ref = Ref(Derived::STATIC_গুনতি + backing.size());
		backing.ঠেলো_back(std::move(t));
	}
	যদি ((Derived::STATIC_গুনতি + size()) * 3 > table.size() * 2)
		grow_index();
	index_ঢোকাও(ref);
	যদি (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	প্রত্যাগম ref;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
template<typename Roots>
size_t HashConsPool<Derived, গ্রন্থি_2, Ref>::gc(ধ্রুব Roots& roots) {
	pool<Ref> live;
	প্রতি (Ref ref : roots)
		mark_live(ref, live);
	size_t erased = 0;
	প্রতি (size_t idx = 0; idx < backing.size(); ++idx) {
		যদি (backing[idx].is_dead())
			অনুবর্ত;
		যদি (!live.গুনতি(Ref(Derived::STATIC_গুনতি + idx))) {
			index_erase(Ref(Derived::STATIC_গুনতি + idx));
			free_list.ঠেলো_back(idx);
			backing[idx] = গ্রন্থি_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	প্রত্যাগম erased;
}

template<typename Derived, typename গ্রন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্রন্থি_2, Ref>::mark_live(Ref ref, pool<Ref>& live) ধ্রুব {
	ref = ref.untag();
	যদি (ref == Ref() || is_static(ref) || !live.ঢোকাও(ref).second)
		প্রত্যাগম;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
