#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#include "kernel/hashcons.h"

#include <algorithm>

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename Node, typename Ref>
bool HashConsPool<Derived, Node, Ref>::is_live(Ref ref) const {
	Ref idx = ref.untag();
	nếu (is_static(idx))
		trả_về true;
	size_t slot = idx.raw() - Derived::STATIC_COUNT;
	trả_về slot < backing.size() && !backing[slot].is_dead();
}

template<typename Derived, typename Node, typename Ref>
void HashConsPool<Derived, Node, Ref>::check_ready() {}

template<typename Derived, typename Node, typename Ref>
const Node& HashConsPool<Derived, Node, Ref>::operator[] (Ref ref) const {
	Ref idx = ref.untag();
	nếu constexpr (Derived::STATIC_COUNT != 0) {
		nếu (is_static(idx))
			trả_về Derived::static_node(idx.raw());
	}
	trả_về backing[idx.raw() - Derived::STATIC_COUNT];
}

template<typename Derived, typename Node, typename Ref>
Derived* HashConsPool<Derived, Node, Ref>::self() { trả_về static_cast<Derived*>(this); }

template<typename Derived, typename Node, typename Ref>
const Derived* HashConsPool<Derived, Node, Ref>::self() const { trả_về static_cast<const Derived*>(this); }

template<typename Derived, typename Node, typename Ref>
bool HashConsPool<Derived, Node, Ref>::is_static(Ref ref) {
	nếu constexpr (Derived::STATIC_COUNT == 0)
		trả_về false;
	không_thì
		trả_về ref.raw() < Derived::STATIC_COUNT;
}

template<typename Derived, typename Node, typename Ref>
void HashConsPool<Derived, Node, Ref>::RefIterator::settle() {
	trong_khi (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

template<typename Derived, typename Node, typename Ref>
Ref HashConsPool<Derived, Node, Ref>::RefIterator::operator*() const { trả_về Ref(idx); }

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::RefIterator::operator++() -> RefIterator& { idx++; settle(); trả_về *this; }

template<typename Derived, typename Node, typename Ref>
bool HashConsPool<Derived, Node, Ref>::RefIterator::operator!=(const RefIterator& other) const { trả_về idx != other.idx; }

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::RefRange::begin() const -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	trả_về it;
}

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::RefRange::end() const -> RefIterator { trả_về RefIterator{pool, stop, stop, skip_dead}; }

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::refs(bool include_statics) const -> RefRange {
	trả_về RefRange{this, include_statics ? 0 : Derived::STATIC_COUNT,
			Derived::STATIC_COUNT + backing.size(), true};
}

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::slots() const -> RefRange {
	trả_về RefRange{this, Derived::STATIC_COUNT,
			Derived::STATIC_COUNT + backing.size(), false};
}

template<typename Derived, typename Node, typename Ref>
size_t HashConsPool<Derived, Node, Ref>::home_slot(uint64_t hash) const {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	trả_về (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

template<typename Derived, typename Node, typename Ref>
size_t HashConsPool<Derived, Node, Ref>::next_slot(size_t slot) const { trả_về (slot + 1) & (table.size() - 1); }

template<typename Derived, typename Node, typename Ref>
size_t HashConsPool<Derived, Node, Ref>::size() const { trả_về backing.size() - free_list.size(); }

template<typename Derived, typename Node, typename Ref>
bool HashConsPool<Derived, Node, Ref>::slot_live(size_t abs) const {
	nếu (abs < Derived::STATIC_COUNT)
		trả_về true;
	trả_về !backing[abs - Derived::STATIC_COUNT].is_dead();
}

template<typename Derived, typename Node, typename Ref>
HashConsPool<Derived, Node, Ref>::HashConsPool() { rebuild_index(); }

template<typename Derived, typename Node, typename Ref>
HashConsPool<Derived, Node, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

template<typename Derived, typename Node, typename Ref>
HashConsPool<Derived, Node, Ref>& HashConsPool<Derived, Node, Ref>::operator=(HashConsPool&& other) {
	nếu (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	trả_về *this;
}

template<typename Derived, typename Node, typename Ref>
void HashConsPool<Derived, Node, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

template<typename Derived, typename Node, typename Ref>
void HashConsPool<Derived, Node, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	cho (size_t idx = 0; idx < backing.size(); ++idx)
		nếu (backing[idx].is_dead())
			free_list.push_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	table.assign(std::bit_ceil((Derived::STATIC_COUNT + size()) * 2 + 2), Ref());
	cho (Ref ref : refs())
		index_insert(ref);
}

template<typename Derived, typename Node, typename Ref>
template<typename Eq>
Ref HashConsPool<Derived, Node, Ref>::find_hashed(uint64_t hash, Eq&& eq) const {
	cho (size_t slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		nếu (Derived::hash_node((*this)[ref]) == hash && eq(ref))
			trả_về ref;
	}
	trả_về Ref();
}

template<typename Derived, typename Node, typename Ref>
void HashConsPool<Derived, Node, Ref>::index_insert(Ref ref) {
	size_t slot = home_slot(Derived::hash_node((*this)[ref]));
	trong_khi (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

template<typename Derived, typename Node, typename Ref>
void HashConsPool<Derived, Node, Ref>::index_erase(Ref ref) {
	size_t hole = home_slot(Derived::hash_node((*this)[ref]));
	trong_khi (table[hole] != ref)
		hole = next_slot(hole);
	size_t mask = table.size() - 1;
	cho (size_t slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		size_t home = home_slot(Derived::hash_node((*this)[table[slot]]));
		nếu (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

template<typename Derived, typename Node, typename Ref>
void HashConsPool<Derived, Node, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	cho (Ref ref : old)
		nếu (ref != Ref())
			index_insert(ref);
}

template<typename Derived, typename Node, typename Ref>
Ref HashConsPool<Derived, Node, Ref>::add_inner(Node t) {
	Ref ref;
	nếu (!free_list.empty()) {
		size_t idx = free_list.back();
		free_list.pop_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_COUNT + idx);
	} không_thì {
		ref = Ref(Derived::STATIC_COUNT + backing.size());
		backing.push_back(std::move(t));
	}
	nếu ((Derived::STATIC_COUNT + size()) * 3 > table.size() * 2)
		grow_index();
	index_insert(ref);
	nếu (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	trả_về ref;
}

template<typename Derived, typename Node, typename Ref>
template<typename Roots>
size_t HashConsPool<Derived, Node, Ref>::gc(const Roots& roots) {
	pool<Ref> live;
	cho (Ref ref : roots)
		mark_live(ref, live);
	size_t erased = 0;
	cho (size_t idx = 0; idx < backing.size(); ++idx) {
		nếu (backing[idx].is_dead())
			continue;
		nếu (!live.count(Ref(Derived::STATIC_COUNT + idx))) {
			index_erase(Ref(Derived::STATIC_COUNT + idx));
			free_list.push_back(idx);
			backing[idx] = Node{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	trả_về erased;
}

template<typename Derived, typename Node, typename Ref>
void HashConsPool<Derived, Node, Ref>::mark_live(Ref ref, pool<Ref>& live) const {
	ref = ref.untag();
	nếu (ref == Ref() || is_static(ref) || !live.insert(ref).second)
		trả_về;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
