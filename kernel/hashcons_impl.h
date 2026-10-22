#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#অন্তর্ভাৱ "kernel/hashcons.h"

#অন্তর্ভাৱ <algorithm>

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
bool HashConsPool<Derived, গ্ৰন্থি_2, Ref>::is_live(Ref ref) ধ্রুৱ {
	Ref idx = ref.untag();
	যদি (is_static(idx))
		ঘূৰাই true;
	size_t slot = idx.raw() - Derived::STATIC_গুনতি;
	ঘূৰাই slot < backing.size() && !backing[slot].is_dead();
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্ৰন্থি_2, Ref>::check_ready() {}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
ধ্রুৱ গ্ৰন্থি_2& HashConsPool<Derived, গ্ৰন্থি_2, Ref>::operator[] (Ref ref) ধ্রুৱ {
	Ref idx = ref.untag();
	যদি constexpr (Derived::STATIC_গুনতি != 0) {
		যদি (is_static(idx))
			ঘূৰাই Derived::static_গ্ৰন্থি(idx.raw());
	}
	ঘূৰাই backing[idx.raw() - Derived::STATIC_গুনতি];
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
Derived* HashConsPool<Derived, গ্ৰন্থি_2, Ref>::self() { ঘূৰাই static_cast<Derived*>(this); }

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
ধ্রুৱ Derived* HashConsPool<Derived, গ্ৰন্থি_2, Ref>::self() ধ্রুৱ { ঘূৰাই static_cast<ধ্রুৱ Derived*>(this); }

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
bool HashConsPool<Derived, গ্ৰন্থি_2, Ref>::is_static(Ref ref) {
	যদি constexpr (Derived::STATIC_গুনতি == 0)
		ঘূৰাই false;
	অন্যথা
		ঘূৰাই ref.raw() < Derived::STATIC_গুনতি;
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্ৰন্থি_2, Ref>::RefIterator::settle() {
	যেতিয়ালৈ (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
Ref HashConsPool<Derived, গ্ৰন্থি_2, Ref>::RefIterator::operator*() ধ্রুৱ { ঘূৰাই Ref(idx); }

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্ৰন্থি_2, Ref>::RefIterator::operator++() -> RefIterator& { idx++; settle(); ঘূৰাই *this; }

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
bool HashConsPool<Derived, গ্ৰন্থি_2, Ref>::RefIterator::operator!=(ধ্রুৱ RefIterator& other) ধ্রুৱ { ঘূৰাই idx != other.idx; }

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্ৰন্থি_2, Ref>::RefRange::begin() ধ্রুৱ -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	ঘূৰাই it;
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্ৰন্থি_2, Ref>::RefRange::end() ধ্রুৱ -> RefIterator { ঘূৰাই RefIterator{pool, stop, stop, skip_dead}; }

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্ৰন্থি_2, Ref>::refs(bool include_statics) ধ্রুৱ -> RefRange {
	ঘূৰাই RefRange{this, include_statics ? 0 : Derived::STATIC_গুনতি,
			Derived::STATIC_গুনতি + backing.size(), true};
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
auto HashConsPool<Derived, গ্ৰন্থি_2, Ref>::slots() ধ্রুৱ -> RefRange {
	ঘূৰাই RefRange{this, Derived::STATIC_গুনতি,
			Derived::STATIC_গুনতি + backing.size(), false};
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
size_t HashConsPool<Derived, গ্ৰন্থি_2, Ref>::home_slot(uint64_t hash) ধ্রুৱ {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	ঘূৰাই (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
size_t HashConsPool<Derived, গ্ৰন্থি_2, Ref>::next_slot(size_t slot) ধ্রুৱ { ঘূৰাই (slot + 1) & (table.size() - 1); }

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
size_t HashConsPool<Derived, গ্ৰন্থি_2, Ref>::size() ধ্রুৱ { ঘূৰাই backing.size() - free_list.size(); }

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
bool HashConsPool<Derived, গ্ৰন্থি_2, Ref>::slot_live(size_t abs) ধ্রুৱ {
	যদি (abs < Derived::STATIC_গুনতি)
		ঘূৰাই true;
	ঘূৰাই !backing[abs - Derived::STATIC_গুনতি].is_dead();
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
HashConsPool<Derived, গ্ৰন্থি_2, Ref>::HashConsPool() { rebuild_index(); }

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
HashConsPool<Derived, গ্ৰন্থি_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
HashConsPool<Derived, গ্ৰন্থি_2, Ref>& HashConsPool<Derived, গ্ৰন্থি_2, Ref>::operator=(HashConsPool&& other) {
	যদি (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	ঘূৰাই *this;
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্ৰন্থি_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্ৰন্থি_2, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	বাবে (size_t idx = 0; idx < backing.size(); ++idx)
		যদি (backing[idx].is_dead())
			free_list.ঠেলো_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	table.assign(std::bit_ceil((Derived::STATIC_গুনতি + size()) * 2 + 2), Ref());
	বাবে (Ref ref : refs())
		index_ঢোকাও(ref);
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
template<typename Eq>
Ref HashConsPool<Derived, গ্ৰন্থি_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) ধ্রুৱ {
	বাবে (size_t slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		যদি (Derived::hash_গ্ৰন্থি((*this)[ref]) == hash && eq(ref))
			ঘূৰাই ref;
	}
	ঘূৰাই Ref();
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্ৰন্থি_2, Ref>::index_ঢোকাও(Ref ref) {
	size_t slot = home_slot(Derived::hash_গ্ৰন্থি((*this)[ref]));
	যেতিয়ালৈ (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্ৰন্থি_2, Ref>::index_erase(Ref ref) {
	size_t hole = home_slot(Derived::hash_গ্ৰন্থি((*this)[ref]));
	যেতিয়ালৈ (table[hole] != ref)
		hole = next_slot(hole);
	size_t mask = table.size() - 1;
	বাবে (size_t slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		size_t home = home_slot(Derived::hash_গ্ৰন্থি((*this)[table[slot]]));
		যদি (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্ৰন্থি_2, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	বাবে (Ref ref : old)
		যদি (ref != Ref())
			index_ঢোকাও(ref);
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
Ref HashConsPool<Derived, গ্ৰন্থি_2, Ref>::add_inner(গ্ৰন্থি_2 t) {
	Ref ref;
	যদি (!free_list.empty()) {
		size_t idx = free_list.back();
		free_list.বেৰ_কৰো_back();
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
	ঘূৰাই ref;
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
template<typename Roots>
size_t HashConsPool<Derived, গ্ৰন্থি_2, Ref>::gc(ধ্রুৱ Roots& roots) {
	pool<Ref> live;
	বাবে (Ref ref : roots)
		mark_live(ref, live);
	size_t erased = 0;
	বাবে (size_t idx = 0; idx < backing.size(); ++idx) {
		যদি (backing[idx].is_dead())
			অনুৱর্ত;
		যদি (!live.গুনতি(Ref(Derived::STATIC_গুনতি + idx))) {
			index_erase(Ref(Derived::STATIC_গুনতি + idx));
			free_list.ঠেলো_back(idx);
			backing[idx] = গ্ৰন্থি_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	ঘূৰাই erased;
}

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
শূন্য HashConsPool<Derived, গ্ৰন্থি_2, Ref>::mark_live(Ref ref, pool<Ref>& live) ধ্রুৱ {
	ref = ref.untag();
	যদি (ref == Ref() || is_static(ref) || !live.ঢোকাও(ref).second)
		ঘূৰাই;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
