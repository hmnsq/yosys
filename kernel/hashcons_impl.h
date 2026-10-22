#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#include "kernel/hashcons.h"

#include <algorithm>

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename Node, typename Ref>
bool HashConsPool<Derived, Node, Ref>::is_live(Ref ref) const {
	Ref idx = ref.untag();
	නම් (is_static(idx))
		ආපසු true;
	size_t slot = idx.raw() - Derived::STATIC_COUNT;
	ආපසු slot < backing.size() && !backing[slot].is_dead();
}

template<typename Derived, typename Node, typename Ref>
void HashConsPool<Derived, Node, Ref>::check_ready() {}

template<typename Derived, typename Node, typename Ref>
const Node& HashConsPool<Derived, Node, Ref>::operator[] (Ref ref) const {
	Ref idx = ref.untag();
	නම් constexpr (Derived::STATIC_COUNT != 0) {
		නම් (is_static(idx))
			ආපසු Derived::static_node(idx.raw());
	}
	ආපසු backing[idx.raw() - Derived::STATIC_COUNT];
}

template<typename Derived, typename Node, typename Ref>
Derived* HashConsPool<Derived, Node, Ref>::self() { ආපසු static_cast<Derived*>(this); }

template<typename Derived, typename Node, typename Ref>
const Derived* HashConsPool<Derived, Node, Ref>::self() const { ආපසු static_cast<const Derived*>(this); }

template<typename Derived, typename Node, typename Ref>
bool HashConsPool<Derived, Node, Ref>::is_static(Ref ref) {
	නම් constexpr (Derived::STATIC_COUNT == 0)
		ආපසු false;
	නැතහොත්
		ආපසු ref.raw() < Derived::STATIC_COUNT;
}

template<typename Derived, typename Node, typename Ref>
void HashConsPool<Derived, Node, Ref>::RefIterator::settle() {
	අතරතුර (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

template<typename Derived, typename Node, typename Ref>
Ref HashConsPool<Derived, Node, Ref>::RefIterator::operator*() const { ආපසු Ref(idx); }

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::RefIterator::operator++() -> RefIterator& { idx++; settle(); ආපසු *this; }

template<typename Derived, typename Node, typename Ref>
bool HashConsPool<Derived, Node, Ref>::RefIterator::operator!=(const RefIterator& other) const { ආපසු idx != other.idx; }

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::RefRange::begin() const -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	ආපසු it;
}

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::RefRange::end() const -> RefIterator { ආපසු RefIterator{pool, stop, stop, skip_dead}; }

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::refs(bool include_statics) const -> RefRange {
	ආපසු RefRange{this, include_statics ? 0 : Derived::STATIC_COUNT,
			Derived::STATIC_COUNT + backing.size(), true};
}

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::slots() const -> RefRange {
	ආපසු RefRange{this, Derived::STATIC_COUNT,
			Derived::STATIC_COUNT + backing.size(), false};
}

template<typename Derived, typename Node, typename Ref>
size_t HashConsPool<Derived, Node, Ref>::home_slot(uint64_t hash) const {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	ආපසු (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

template<typename Derived, typename Node, typename Ref>
size_t HashConsPool<Derived, Node, Ref>::next_slot(size_t slot) const { ආපසු (slot + 1) & (table.size() - 1); }

template<typename Derived, typename Node, typename Ref>
size_t HashConsPool<Derived, Node, Ref>::size() const { ආපසු backing.size() - free_list.size(); }

template<typename Derived, typename Node, typename Ref>
bool HashConsPool<Derived, Node, Ref>::slot_live(size_t abs) const {
	නම් (abs < Derived::STATIC_COUNT)
		ආපසු true;
	ආපසු !backing[abs - Derived::STATIC_COUNT].is_dead();
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
	නම් (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	ආපසු *this;
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
	සඳහා (size_t idx = 0; idx < backing.size(); ++idx)
		නම් (backing[idx].is_dead())
			free_list.push_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	table.assign(std::bit_ceil((Derived::STATIC_COUNT + size()) * 2 + 2), Ref());
	සඳහා (Ref ref : refs())
		index_insert(ref);
}

template<typename Derived, typename Node, typename Ref>
template<typename Eq>
Ref HashConsPool<Derived, Node, Ref>::find_hashed(uint64_t hash, Eq&& eq) const {
	සඳහා (size_t slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		නම් (Derived::hash_node((*this)[ref]) == hash && eq(ref))
			ආපසු ref;
	}
	ආපසු Ref();
}

template<typename Derived, typename Node, typename Ref>
void HashConsPool<Derived, Node, Ref>::index_insert(Ref ref) {
	size_t slot = home_slot(Derived::hash_node((*this)[ref]));
	අතරතුර (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

template<typename Derived, typename Node, typename Ref>
void HashConsPool<Derived, Node, Ref>::index_erase(Ref ref) {
	size_t hole = home_slot(Derived::hash_node((*this)[ref]));
	අතරතුර (table[hole] != ref)
		hole = next_slot(hole);
	size_t mask = table.size() - 1;
	සඳහා (size_t slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		size_t home = home_slot(Derived::hash_node((*this)[table[slot]]));
		නම් (((slot - home) & mask) >= ((slot - hole) & mask)) {
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
	සඳහා (Ref ref : old)
		නම් (ref != Ref())
			index_insert(ref);
}

template<typename Derived, typename Node, typename Ref>
Ref HashConsPool<Derived, Node, Ref>::add_inner(Node t) {
	Ref ref;
	නම් (!free_list.empty()) {
		size_t idx = free_list.back();
		free_list.pop_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_COUNT + idx);
	} නැතහොත් {
		ref = Ref(Derived::STATIC_COUNT + backing.size());
		backing.push_back(std::move(t));
	}
	නම් ((Derived::STATIC_COUNT + size()) * 3 > table.size() * 2)
		grow_index();
	index_insert(ref);
	නම් (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	ආපසු ref;
}

template<typename Derived, typename Node, typename Ref>
template<typename Roots>
size_t HashConsPool<Derived, Node, Ref>::gc(const Roots& roots) {
	pool<Ref> live;
	සඳහා (Ref ref : roots)
		mark_live(ref, live);
	size_t erased = 0;
	සඳහා (size_t idx = 0; idx < backing.size(); ++idx) {
		නම් (backing[idx].is_dead())
			continue;
		නම් (!live.count(Ref(Derived::STATIC_COUNT + idx))) {
			index_erase(Ref(Derived::STATIC_COUNT + idx));
			free_list.push_back(idx);
			backing[idx] = Node{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	ආපසු erased;
}

template<typename Derived, typename Node, typename Ref>
void HashConsPool<Derived, Node, Ref>::mark_live(Ref ref, pool<Ref>& live) const {
	ref = ref.untag();
	නම් (ref == Ref() || is_static(ref) || !live.insert(ref).second)
		ආපසු;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
