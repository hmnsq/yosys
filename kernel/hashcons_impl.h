#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#ಅನ್ತರ್ಭಾವ "kernel/hashcons.h"

#ಅನ್ತರ್ಭಾವ <algorithm>

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename ಗಂಟು_2, typename Ref>
bool HashConsPool<Derived, ಗಂಟು_2, Ref>::is_live(Ref ref) ಧ್ರುವ {
	Ref idx = ref.untag();
	ಯದಿ (is_static(idx))
		ಪ್ರತ್ಯಾಗಮ true;
	size_t slot = idx.raw() - Derived::STATIC_ಎಣಿಕೆ;
	ಪ್ರತ್ಯಾಗಮ slot < backing.size() && !backing[slot].is_dead();
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::check_ready() {}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
ಧ್ರುವ ಗಂಟು_2& HashConsPool<Derived, ಗಂಟು_2, Ref>::operator[] (Ref ref) ಧ್ರುವ {
	Ref idx = ref.untag();
	ಯದಿ constexpr (Derived::STATIC_ಎಣಿಕೆ != 0) {
		ಯದಿ (is_static(idx))
			ಪ್ರತ್ಯಾಗಮ Derived::static_ಗಂಟು(idx.raw());
	}
	ಪ್ರತ್ಯಾಗಮ backing[idx.raw() - Derived::STATIC_ಎಣಿಕೆ];
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
Derived* HashConsPool<Derived, ಗಂಟು_2, Ref>::self() { ಪ್ರತ್ಯಾಗಮ static_cast<Derived*>(this); }

template<typename Derived, typename ಗಂಟು_2, typename Ref>
ಧ್ರುವ Derived* HashConsPool<Derived, ಗಂಟು_2, Ref>::self() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ static_cast<ಧ್ರುವ Derived*>(this); }

template<typename Derived, typename ಗಂಟು_2, typename Ref>
bool HashConsPool<Derived, ಗಂಟು_2, Ref>::is_static(Ref ref) {
	ಯದಿ constexpr (Derived::STATIC_ಎಣಿಕೆ == 0)
		ಪ್ರತ್ಯಾಗಮ false;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ ref.raw() < Derived::STATIC_ಎಣಿಕೆ;
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::RefIterator::settle() {
	ಯಾವತ್ (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
Ref HashConsPool<Derived, ಗಂಟು_2, Ref>::RefIterator::operator*() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ Ref(idx); }

template<typename Derived, typename ಗಂಟು_2, typename Ref>
auto HashConsPool<Derived, ಗಂಟು_2, Ref>::RefIterator::operator++() -> RefIterator& { idx++; settle(); ಪ್ರತ್ಯಾಗಮ *this; }

template<typename Derived, typename ಗಂಟು_2, typename Ref>
bool HashConsPool<Derived, ಗಂಟು_2, Ref>::RefIterator::operator!=(ಧ್ರುವ RefIterator& other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ idx != other.idx; }

template<typename Derived, typename ಗಂಟು_2, typename Ref>
auto HashConsPool<Derived, ಗಂಟು_2, Ref>::RefRange::begin() ಧ್ರುವ -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	ಪ್ರತ್ಯಾಗಮ it;
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
auto HashConsPool<Derived, ಗಂಟು_2, Ref>::RefRange::end() ಧ್ರುವ -> RefIterator { ಪ್ರತ್ಯಾಗಮ RefIterator{pool, stop, stop, skip_dead}; }

template<typename Derived, typename ಗಂಟು_2, typename Ref>
auto HashConsPool<Derived, ಗಂಟು_2, Ref>::refs(bool include_statics) ಧ್ರುವ -> RefRange {
	ಪ್ರತ್ಯಾಗಮ RefRange{this, include_statics ? 0 : Derived::STATIC_ಎಣಿಕೆ,
			Derived::STATIC_ಎಣಿಕೆ + backing.size(), true};
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
auto HashConsPool<Derived, ಗಂಟು_2, Ref>::slots() ಧ್ರುವ -> RefRange {
	ಪ್ರತ್ಯಾಗಮ RefRange{this, Derived::STATIC_ಎಣಿಕೆ,
			Derived::STATIC_ಎಣಿಕೆ + backing.size(), false};
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
size_t HashConsPool<Derived, ಗಂಟು_2, Ref>::home_slot(uint64_t hash) ಧ್ರುವ {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	ಪ್ರತ್ಯಾಗಮ (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
size_t HashConsPool<Derived, ಗಂಟು_2, Ref>::next_slot(size_t slot) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (slot + 1) & (table.size() - 1); }

template<typename Derived, typename ಗಂಟು_2, typename Ref>
size_t HashConsPool<Derived, ಗಂಟು_2, Ref>::size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ backing.size() - free_list.size(); }

template<typename Derived, typename ಗಂಟು_2, typename Ref>
bool HashConsPool<Derived, ಗಂಟು_2, Ref>::slot_live(size_t abs) ಧ್ರುವ {
	ಯದಿ (abs < Derived::STATIC_ಎಣಿಕೆ)
		ಪ್ರತ್ಯಾಗಮ true;
	ಪ್ರತ್ಯಾಗಮ !backing[abs - Derived::STATIC_ಎಣಿಕೆ].is_dead();
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
HashConsPool<Derived, ಗಂಟು_2, Ref>::HashConsPool() { rebuild_index(); }

template<typename Derived, typename ಗಂಟು_2, typename Ref>
HashConsPool<Derived, ಗಂಟು_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
HashConsPool<Derived, ಗಂಟು_2, Ref>& HashConsPool<Derived, ಗಂಟು_2, Ref>::operator=(HashConsPool&& other) {
	ಯದಿ (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	ಪ್ರತ್ಯಾಗಮ *this;
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	ಪ್ರತಿ (size_t idx = 0; idx < backing.size(); ++idx)
		ಯದಿ (backing[idx].is_dead())
			free_list.ತಳ್ಳು_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	table.assign(std::bit_ceil((Derived::STATIC_ಎಣಿಕೆ + size()) * 2 + 2), Ref());
	ಪ್ರತಿ (Ref ref : refs())
		index_ಸೇರಿಸು(ref);
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
template<typename Eq>
Ref HashConsPool<Derived, ಗಂಟು_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) ಧ್ರುವ {
	ಪ್ರತಿ (size_t slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		ಯದಿ (Derived::hash_ಗಂಟು((*this)[ref]) == hash && eq(ref))
			ಪ್ರತ್ಯಾಗಮ ref;
	}
	ಪ್ರತ್ಯಾಗಮ Ref();
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::index_ಸೇರಿಸು(Ref ref) {
	size_t slot = home_slot(Derived::hash_ಗಂಟು((*this)[ref]));
	ಯಾವತ್ (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::index_erase(Ref ref) {
	size_t hole = home_slot(Derived::hash_ಗಂಟು((*this)[ref]));
	ಯಾವತ್ (table[hole] != ref)
		hole = next_slot(hole);
	size_t mask = table.size() - 1;
	ಪ್ರತಿ (size_t slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		size_t home = home_slot(Derived::hash_ಗಂಟು((*this)[table[slot]]));
		ಯದಿ (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	ಪ್ರತಿ (Ref ref : old)
		ಯದಿ (ref != Ref())
			index_ಸೇರಿಸು(ref);
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
Ref HashConsPool<Derived, ಗಂಟು_2, Ref>::add_inner(ಗಂಟು_2 t) {
	Ref ref;
	ಯದಿ (!free_list.empty()) {
		size_t idx = free_list.back();
		free_list.ತೆಗೆ_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_ಎಣಿಕೆ + idx);
	} ಅನ್ಯಥಾ {
		ref = Ref(Derived::STATIC_ಎಣಿಕೆ + backing.size());
		backing.ತಳ್ಳು_back(std::move(t));
	}
	ಯದಿ ((Derived::STATIC_ಎಣಿಕೆ + size()) * 3 > table.size() * 2)
		grow_index();
	index_ಸೇರಿಸು(ref);
	ಯದಿ (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	ಪ್ರತ್ಯಾಗಮ ref;
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
template<typename Roots>
size_t HashConsPool<Derived, ಗಂಟು_2, Ref>::gc(ಧ್ರುವ Roots& roots) {
	pool<Ref> live;
	ಪ್ರತಿ (Ref ref : roots)
		mark_live(ref, live);
	size_t erased = 0;
	ಪ್ರತಿ (size_t idx = 0; idx < backing.size(); ++idx) {
		ಯದಿ (backing[idx].is_dead())
			ಅನುವರ್ತ;
		ಯದಿ (!live.ಎಣಿಕೆ(Ref(Derived::STATIC_ಎಣಿಕೆ + idx))) {
			index_erase(Ref(Derived::STATIC_ಎಣಿಕೆ + idx));
			free_list.ತಳ್ಳು_back(idx);
			backing[idx] = ಗಂಟು_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	ಪ್ರತ್ಯಾಗಮ erased;
}

template<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::mark_live(Ref ref, pool<Ref>& live) ಧ್ರುವ {
	ref = ref.untag();
	ಯದಿ (ref == Ref() || is_static(ref) || !live.ಸೇರಿಸು(ref).second)
		ಪ್ರತ್ಯಾಗಮ;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
