#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#ضمن "kernel/hashcons.h"

#ضمن <algorithm>

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename Node, typename Ref>
bool HashConsPool<Derived, Node, Ref>::is_live(Ref ref) ثابت {
	Ref idx = ref.untag();
	𐡄𐡍 (is_static(idx))
		أرجع true;
	size_t slot = idx.raw() - Derived::STATIC_COUNT;
	أرجع slot < backing.size() && !backing[slot].is_dead();
}

template<typename Derived, typename Node, typename Ref>
فراغ HashConsPool<Derived, Node, Ref>::check_ready() {}

template<typename Derived, typename Node, typename Ref>
ثابت Node& HashConsPool<Derived, Node, Ref>::operator[] (Ref ref) ثابت {
	Ref idx = ref.untag();
	𐡄𐡍 constexpr (Derived::STATIC_COUNT != 0) {
		𐡄𐡍 (is_static(idx))
			أرجع Derived::static_node(idx.raw());
	}
	أرجع backing[idx.raw() - Derived::STATIC_COUNT];
}

template<typename Derived, typename Node, typename Ref>
Derived* HashConsPool<Derived, Node, Ref>::self() { أرجع static_cast<Derived*>(this); }

template<typename Derived, typename Node, typename Ref>
ثابت Derived* HashConsPool<Derived, Node, Ref>::self() ثابت { أرجع static_cast<ثابت Derived*>(this); }

template<typename Derived, typename Node, typename Ref>
bool HashConsPool<Derived, Node, Ref>::is_static(Ref ref) {
	𐡄𐡍 constexpr (Derived::STATIC_COUNT == 0)
		أرجع false;
	وإلا
		أرجع ref.raw() < Derived::STATIC_COUNT;
}

template<typename Derived, typename Node, typename Ref>
فراغ HashConsPool<Derived, Node, Ref>::RefIterator::settle() {
	طالما (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

template<typename Derived, typename Node, typename Ref>
Ref HashConsPool<Derived, Node, Ref>::RefIterator::operator*() ثابت { أرجع Ref(idx); }

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::RefIterator::operator++() -> RefIterator& { idx++; settle(); أرجع *this; }

template<typename Derived, typename Node, typename Ref>
bool HashConsPool<Derived, Node, Ref>::RefIterator::operator!=(ثابت RefIterator& other) ثابت { أرجع idx != other.idx; }

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::RefRange::begin() ثابت -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	أرجع it;
}

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::RefRange::end() ثابت -> RefIterator { أرجع RefIterator{pool, stop, stop, skip_dead}; }

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::refs(bool include_statics) ثابت -> RefRange {
	أرجع RefRange{this, include_statics ? 0 : Derived::STATIC_COUNT,
			Derived::STATIC_COUNT + backing.size(), true};
}

template<typename Derived, typename Node, typename Ref>
auto HashConsPool<Derived, Node, Ref>::slots() ثابت -> RefRange {
	أرجع RefRange{this, Derived::STATIC_COUNT,
			Derived::STATIC_COUNT + backing.size(), false};
}

template<typename Derived, typename Node, typename Ref>
size_t HashConsPool<Derived, Node, Ref>::home_slot(uint64_t hash) ثابت {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	أرجع (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

template<typename Derived, typename Node, typename Ref>
size_t HashConsPool<Derived, Node, Ref>::next_slot(size_t slot) ثابت { أرجع (slot + 1) & (table.size() - 1); }

template<typename Derived, typename Node, typename Ref>
size_t HashConsPool<Derived, Node, Ref>::size() ثابت { أرجع backing.size() - free_list.size(); }

template<typename Derived, typename Node, typename Ref>
bool HashConsPool<Derived, Node, Ref>::slot_live(size_t abs) ثابت {
	𐡄𐡍 (abs < Derived::STATIC_COUNT)
		أرجع true;
	أرجع !backing[abs - Derived::STATIC_COUNT].is_dead();
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
	𐡄𐡍 (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	أرجع *this;
}

template<typename Derived, typename Node, typename Ref>
فراغ HashConsPool<Derived, Node, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

template<typename Derived, typename Node, typename Ref>
فراغ HashConsPool<Derived, Node, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	لكل (size_t idx = 0; idx < backing.size(); ++idx)
		𐡄𐡍 (backing[idx].is_dead())
			free_list.push_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	table.assign(std::bit_ceil((Derived::STATIC_COUNT + size()) * 2 + 2), Ref());
	لكل (Ref ref : refs())
		index_insert(ref);
}

template<typename Derived, typename Node, typename Ref>
template<typename Eq>
Ref HashConsPool<Derived, Node, Ref>::find_hashed(uint64_t hash, Eq&& eq) ثابت {
	لكل (size_t slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		𐡄𐡍 (Derived::hash_node((*this)[ref]) == hash && eq(ref))
			أرجع ref;
	}
	أرجع Ref();
}

template<typename Derived, typename Node, typename Ref>
فراغ HashConsPool<Derived, Node, Ref>::index_insert(Ref ref) {
	size_t slot = home_slot(Derived::hash_node((*this)[ref]));
	طالما (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

template<typename Derived, typename Node, typename Ref>
فراغ HashConsPool<Derived, Node, Ref>::index_erase(Ref ref) {
	size_t hole = home_slot(Derived::hash_node((*this)[ref]));
	طالما (table[hole] != ref)
		hole = next_slot(hole);
	size_t mask = table.size() - 1;
	لكل (size_t slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		size_t home = home_slot(Derived::hash_node((*this)[table[slot]]));
		𐡄𐡍 (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

template<typename Derived, typename Node, typename Ref>
فراغ HashConsPool<Derived, Node, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	لكل (Ref ref : old)
		𐡄𐡍 (ref != Ref())
			index_insert(ref);
}

template<typename Derived, typename Node, typename Ref>
Ref HashConsPool<Derived, Node, Ref>::add_inner(Node t) {
	Ref ref;
	𐡄𐡍 (!free_list.empty()) {
		size_t idx = free_list.back();
		free_list.pop_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_COUNT + idx);
	} وإلا {
		ref = Ref(Derived::STATIC_COUNT + backing.size());
		backing.push_back(std::move(t));
	}
	𐡄𐡍 ((Derived::STATIC_COUNT + size()) * 3 > table.size() * 2)
		grow_index();
	index_insert(ref);
	𐡄𐡍 (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	أرجع ref;
}

template<typename Derived, typename Node, typename Ref>
template<typename Roots>
size_t HashConsPool<Derived, Node, Ref>::gc(ثابت Roots& roots) {
	pool<Ref> live;
	لكل (Ref ref : roots)
		mark_live(ref, live);
	size_t erased = 0;
	لكل (size_t idx = 0; idx < backing.size(); ++idx) {
		𐡄𐡍 (backing[idx].is_dead())
			استمر;
		𐡄𐡍 (!live.count(Ref(Derived::STATIC_COUNT + idx))) {
			index_erase(Ref(Derived::STATIC_COUNT + idx));
			free_list.push_back(idx);
			backing[idx] = Node{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	أرجع erased;
}

template<typename Derived, typename Node, typename Ref>
فراغ HashConsPool<Derived, Node, Ref>::mark_live(Ref ref, pool<Ref>& live) ثابت {
	ref = ref.untag();
	𐡄𐡍 (ref == Ref() || is_static(ref) || !live.insert(ref).second)
		أرجع;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
