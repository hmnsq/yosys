#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#شامل "kernel/hashcons.h"

#شامل <algorithm>

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename گره_2, typename Ref>
bool HashConsPool<Derived, گره_2, Ref>::is_live(Ref ref) ثابت {
	Ref idx = ref.untag();
	اگر (is_static(idx))
		بازگشت true;
	size_t slot = idx.raw() - Derived::STATIC_شمار;
	بازگشت slot < backing.size() && !backing[slot].is_dead();
}

template<typename Derived, typename گره_2, typename Ref>
تهی HashConsPool<Derived, گره_2, Ref>::check_ready() {}

template<typename Derived, typename گره_2, typename Ref>
ثابت گره_2& HashConsPool<Derived, گره_2, Ref>::operator[] (Ref ref) ثابت {
	Ref idx = ref.untag();
	اگر constexpr (Derived::STATIC_شمار != 0) {
		اگر (is_static(idx))
			بازگشت Derived::static_گره(idx.raw());
	}
	بازگشت backing[idx.raw() - Derived::STATIC_شمار];
}

template<typename Derived, typename گره_2, typename Ref>
Derived* HashConsPool<Derived, گره_2, Ref>::self() { بازگشت static_cast<Derived*>(this); }

template<typename Derived, typename گره_2, typename Ref>
ثابت Derived* HashConsPool<Derived, گره_2, Ref>::self() ثابت { بازگشت static_cast<ثابت Derived*>(this); }

template<typename Derived, typename گره_2, typename Ref>
bool HashConsPool<Derived, گره_2, Ref>::is_static(Ref ref) {
	اگر constexpr (Derived::STATIC_شمار == 0)
		بازگشت false;
	وگرنه
		بازگشت ref.raw() < Derived::STATIC_شمار;
}

template<typename Derived, typename گره_2, typename Ref>
تهی HashConsPool<Derived, گره_2, Ref>::RefIterator::settle() {
	وقتی (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

template<typename Derived, typename گره_2, typename Ref>
Ref HashConsPool<Derived, گره_2, Ref>::RefIterator::operator*() ثابت { بازگشت Ref(idx); }

template<typename Derived, typename گره_2, typename Ref>
auto HashConsPool<Derived, گره_2, Ref>::RefIterator::operator++() -> RefIterator& { idx++; settle(); بازگشت *this; }

template<typename Derived, typename گره_2, typename Ref>
bool HashConsPool<Derived, گره_2, Ref>::RefIterator::operator!=(ثابت RefIterator& other) ثابت { بازگشت idx != other.idx; }

template<typename Derived, typename گره_2, typename Ref>
auto HashConsPool<Derived, گره_2, Ref>::RefRange::begin() ثابت -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	بازگشت it;
}

template<typename Derived, typename گره_2, typename Ref>
auto HashConsPool<Derived, گره_2, Ref>::RefRange::end() ثابت -> RefIterator { بازگشت RefIterator{pool, stop, stop, skip_dead}; }

template<typename Derived, typename گره_2, typename Ref>
auto HashConsPool<Derived, گره_2, Ref>::refs(bool include_statics) ثابت -> RefRange {
	بازگشت RefRange{this, include_statics ? 0 : Derived::STATIC_شمار,
			Derived::STATIC_شمار + backing.size(), true};
}

template<typename Derived, typename گره_2, typename Ref>
auto HashConsPool<Derived, گره_2, Ref>::slots() ثابت -> RefRange {
	بازگشت RefRange{this, Derived::STATIC_شمار,
			Derived::STATIC_شمار + backing.size(), false};
}

template<typename Derived, typename گره_2, typename Ref>
size_t HashConsPool<Derived, گره_2, Ref>::home_slot(uint64_t hash) ثابت {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	بازگشت (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

template<typename Derived, typename گره_2, typename Ref>
size_t HashConsPool<Derived, گره_2, Ref>::next_slot(size_t slot) ثابت { بازگشت (slot + 1) & (table.size() - 1); }

template<typename Derived, typename گره_2, typename Ref>
size_t HashConsPool<Derived, گره_2, Ref>::size() ثابت { بازگشت backing.size() - free_list.size(); }

template<typename Derived, typename گره_2, typename Ref>
bool HashConsPool<Derived, گره_2, Ref>::slot_live(size_t abs) ثابت {
	اگر (abs < Derived::STATIC_شمار)
		بازگشت true;
	بازگشت !backing[abs - Derived::STATIC_شمار].is_dead();
}

template<typename Derived, typename گره_2, typename Ref>
HashConsPool<Derived, گره_2, Ref>::HashConsPool() { rebuild_index(); }

template<typename Derived, typename گره_2, typename Ref>
HashConsPool<Derived, گره_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

template<typename Derived, typename گره_2, typename Ref>
HashConsPool<Derived, گره_2, Ref>& HashConsPool<Derived, گره_2, Ref>::operator=(HashConsPool&& other) {
	اگر (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	بازگشت *this;
}

template<typename Derived, typename گره_2, typename Ref>
تهی HashConsPool<Derived, گره_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

template<typename Derived, typename گره_2, typename Ref>
تهی HashConsPool<Derived, گره_2, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	برای (size_t idx = 0; idx < backing.size(); ++idx)
		اگر (backing[idx].is_dead())
			free_list.افزودن_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	table.assign(std::bit_ceil((Derived::STATIC_شمار + size()) * 2 + 2), Ref());
	برای (Ref ref : refs())
		index_درج(ref);
}

template<typename Derived, typename گره_2, typename Ref>
template<typename Eq>
Ref HashConsPool<Derived, گره_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) ثابت {
	برای (size_t slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		اگر (Derived::hash_گره((*this)[ref]) == hash && eq(ref))
			بازگشت ref;
	}
	بازگشت Ref();
}

template<typename Derived, typename گره_2, typename Ref>
تهی HashConsPool<Derived, گره_2, Ref>::index_درج(Ref ref) {
	size_t slot = home_slot(Derived::hash_گره((*this)[ref]));
	وقتی (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

template<typename Derived, typename گره_2, typename Ref>
تهی HashConsPool<Derived, گره_2, Ref>::index_erase(Ref ref) {
	size_t hole = home_slot(Derived::hash_گره((*this)[ref]));
	وقتی (table[hole] != ref)
		hole = next_slot(hole);
	size_t mask = table.size() - 1;
	برای (size_t slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		size_t home = home_slot(Derived::hash_گره((*this)[table[slot]]));
		اگر (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

template<typename Derived, typename گره_2, typename Ref>
تهی HashConsPool<Derived, گره_2, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	برای (Ref ref : old)
		اگر (ref != Ref())
			index_درج(ref);
}

template<typename Derived, typename گره_2, typename Ref>
Ref HashConsPool<Derived, گره_2, Ref>::add_inner(گره_2 t) {
	Ref ref;
	اگر (!free_list.empty()) {
		size_t idx = free_list.back();
		free_list.برداشت_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_شمار + idx);
	} وگرنه {
		ref = Ref(Derived::STATIC_شمار + backing.size());
		backing.افزودن_back(std::move(t));
	}
	اگر ((Derived::STATIC_شمار + size()) * 3 > table.size() * 2)
		grow_index();
	index_درج(ref);
	اگر (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	بازگشت ref;
}

template<typename Derived, typename گره_2, typename Ref>
template<typename Roots>
size_t HashConsPool<Derived, گره_2, Ref>::gc(ثابت Roots& roots) {
	pool<Ref> live;
	برای (Ref ref : roots)
		mark_live(ref, live);
	size_t erased = 0;
	برای (size_t idx = 0; idx < backing.size(); ++idx) {
		اگر (backing[idx].is_dead())
			ادامه;
		اگر (!live.شمار(Ref(Derived::STATIC_شمار + idx))) {
			index_erase(Ref(Derived::STATIC_شمار + idx));
			free_list.افزودن_back(idx);
			backing[idx] = گره_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	بازگشت erased;
}

template<typename Derived, typename گره_2, typename Ref>
تهی HashConsPool<Derived, گره_2, Ref>::mark_live(Ref ref, pool<Ref>& live) ثابت {
	ref = ref.untag();
	اگر (ref == Ref() || is_static(ref) || !live.درج(ref).second)
		بازگشت;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
