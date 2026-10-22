#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#અન્તર્ભાવ "kernel/hashcons.h"

#અન્તર્ભાવ <algorithm>

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename ગાંઠ_2, typename Ref>
bool HashConsPool<Derived, ગાંઠ_2, Ref>::is_live(Ref ref) અચલ {
	Ref idx = ref.untag();
	જો (is_static(idx))
		પાછા true;
	size_t slot = idx.raw() - Derived::STATIC_ગણતરી;
	પાછા slot < backing.size() && !backing[slot].is_dead();
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
શૂન્ય HashConsPool<Derived, ગાંઠ_2, Ref>::check_ready() {}

template<typename Derived, typename ગાંઠ_2, typename Ref>
અચલ ગાંઠ_2& HashConsPool<Derived, ગાંઠ_2, Ref>::operator[] (Ref ref) અચલ {
	Ref idx = ref.untag();
	જો constexpr (Derived::STATIC_ગણતરી != 0) {
		જો (is_static(idx))
			પાછા Derived::static_ગાંઠ(idx.raw());
	}
	પાછા backing[idx.raw() - Derived::STATIC_ગણતરી];
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
Derived* HashConsPool<Derived, ગાંઠ_2, Ref>::self() { પાછા static_cast<Derived*>(this); }

template<typename Derived, typename ગાંઠ_2, typename Ref>
અચલ Derived* HashConsPool<Derived, ગાંઠ_2, Ref>::self() અચલ { પાછા static_cast<અચલ Derived*>(this); }

template<typename Derived, typename ગાંઠ_2, typename Ref>
bool HashConsPool<Derived, ગાંઠ_2, Ref>::is_static(Ref ref) {
	જો constexpr (Derived::STATIC_ગણતરી == 0)
		પાછા false;
	નહીંતર
		પાછા ref.raw() < Derived::STATIC_ગણતરી;
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
શૂન્ય HashConsPool<Derived, ગાંઠ_2, Ref>::RefIterator::settle() {
	જ્યારે (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
Ref HashConsPool<Derived, ગાંઠ_2, Ref>::RefIterator::operator*() અચલ { પાછા Ref(idx); }

template<typename Derived, typename ગાંઠ_2, typename Ref>
auto HashConsPool<Derived, ગાંઠ_2, Ref>::RefIterator::operator++() -> RefIterator& { idx++; settle(); પાછા *this; }

template<typename Derived, typename ગાંઠ_2, typename Ref>
bool HashConsPool<Derived, ગાંઠ_2, Ref>::RefIterator::operator!=(અચલ RefIterator& other) અચલ { પાછા idx != other.idx; }

template<typename Derived, typename ગાંઠ_2, typename Ref>
auto HashConsPool<Derived, ગાંઠ_2, Ref>::RefRange::begin() અચલ -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	પાછા it;
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
auto HashConsPool<Derived, ગાંઠ_2, Ref>::RefRange::end() અચલ -> RefIterator { પાછા RefIterator{pool, stop, stop, skip_dead}; }

template<typename Derived, typename ગાંઠ_2, typename Ref>
auto HashConsPool<Derived, ગાંઠ_2, Ref>::refs(bool include_statics) અચલ -> RefRange {
	પાછા RefRange{this, include_statics ? 0 : Derived::STATIC_ગણતરી,
			Derived::STATIC_ગણતરી + backing.size(), true};
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
auto HashConsPool<Derived, ગાંઠ_2, Ref>::slots() અચલ -> RefRange {
	પાછા RefRange{this, Derived::STATIC_ગણતરી,
			Derived::STATIC_ગણતરી + backing.size(), false};
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
size_t HashConsPool<Derived, ગાંઠ_2, Ref>::home_slot(uint64_t hash) અચલ {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	પાછા (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
size_t HashConsPool<Derived, ગાંઠ_2, Ref>::next_slot(size_t slot) અચલ { પાછા (slot + 1) & (table.size() - 1); }

template<typename Derived, typename ગાંઠ_2, typename Ref>
size_t HashConsPool<Derived, ગાંઠ_2, Ref>::size() અચલ { પાછા backing.size() - free_list.size(); }

template<typename Derived, typename ગાંઠ_2, typename Ref>
bool HashConsPool<Derived, ગાંઠ_2, Ref>::slot_live(size_t abs) અચલ {
	જો (abs < Derived::STATIC_ગણતરી)
		પાછા true;
	પાછા !backing[abs - Derived::STATIC_ગણતરી].is_dead();
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
HashConsPool<Derived, ગાંઠ_2, Ref>::HashConsPool() { rebuild_index(); }

template<typename Derived, typename ગાંઠ_2, typename Ref>
HashConsPool<Derived, ગાંઠ_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
HashConsPool<Derived, ગાંઠ_2, Ref>& HashConsPool<Derived, ગાંઠ_2, Ref>::operator=(HashConsPool&& other) {
	જો (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	પાછા *this;
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
શૂન્ય HashConsPool<Derived, ગાંઠ_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
શૂન્ય HashConsPool<Derived, ગાંઠ_2, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	માટે (size_t idx = 0; idx < backing.size(); ++idx)
		જો (backing[idx].is_dead())
			free_list.ધકેલો_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	table.assign(std::bit_ceil((Derived::STATIC_ગણતરી + size()) * 2 + 2), Ref());
	માટે (Ref ref : refs())
		index_ઉમેરો(ref);
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
template<typename Eq>
Ref HashConsPool<Derived, ગાંઠ_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) અચલ {
	માટે (size_t slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		જો (Derived::hash_ગાંઠ((*this)[ref]) == hash && eq(ref))
			પાછા ref;
	}
	પાછા Ref();
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
શૂન્ય HashConsPool<Derived, ગાંઠ_2, Ref>::index_ઉમેરો(Ref ref) {
	size_t slot = home_slot(Derived::hash_ગાંઠ((*this)[ref]));
	જ્યારે (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
શૂન્ય HashConsPool<Derived, ગાંઠ_2, Ref>::index_erase(Ref ref) {
	size_t hole = home_slot(Derived::hash_ગાંઠ((*this)[ref]));
	જ્યારે (table[hole] != ref)
		hole = next_slot(hole);
	size_t mask = table.size() - 1;
	માટે (size_t slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		size_t home = home_slot(Derived::hash_ગાંઠ((*this)[table[slot]]));
		જો (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
શૂન્ય HashConsPool<Derived, ગાંઠ_2, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	માટે (Ref ref : old)
		જો (ref != Ref())
			index_ઉમેરો(ref);
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
Ref HashConsPool<Derived, ગાંઠ_2, Ref>::add_inner(ગાંઠ_2 t) {
	Ref ref;
	જો (!free_list.empty()) {
		size_t idx = free_list.back();
		free_list.કાઢો_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_ગણતરી + idx);
	} નહીંતર {
		ref = Ref(Derived::STATIC_ગણતરી + backing.size());
		backing.ધકેલો_back(std::move(t));
	}
	જો ((Derived::STATIC_ગણતરી + size()) * 3 > table.size() * 2)
		grow_index();
	index_ઉમેરો(ref);
	જો (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	પાછા ref;
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
template<typename Roots>
size_t HashConsPool<Derived, ગાંઠ_2, Ref>::gc(અચલ Roots& roots) {
	pool<Ref> live;
	માટે (Ref ref : roots)
		mark_live(ref, live);
	size_t erased = 0;
	માટે (size_t idx = 0; idx < backing.size(); ++idx) {
		જો (backing[idx].is_dead())
			ચાલુ;
		જો (!live.ગણતરી(Ref(Derived::STATIC_ગણતરી + idx))) {
			index_erase(Ref(Derived::STATIC_ગણતરી + idx));
			free_list.ધકેલો_back(idx);
			backing[idx] = ગાંઠ_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	પાછા erased;
}

template<typename Derived, typename ગાંઠ_2, typename Ref>
શૂન્ય HashConsPool<Derived, ગાંઠ_2, Ref>::mark_live(Ref ref, pool<Ref>& live) અચલ {
	ref = ref.untag();
	જો (ref == Ref() || is_static(ref) || !live.ઉમેરો(ref).second)
		પાછા;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
