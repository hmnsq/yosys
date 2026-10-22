#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#அந்தர்பாவ "kernel/hashcons.h"

#அந்தர்பாவ <algorithm>

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename முனை_2, typename Ref>
bool HashConsPool<Derived, முனை_2, Ref>::is_live(Ref ref) மாறிலி {
	Ref idx = ref.untag();
	எனில் (is_static(idx))
		திரும்பு true;
	size_t slot = idx.raw() - Derived::STATIC_மொத்தம்;
	திரும்பு slot < backing.size() && !backing[slot].is_dead();
}

template<typename Derived, typename முனை_2, typename Ref>
வெறுமை HashConsPool<Derived, முனை_2, Ref>::check_ready() {}

template<typename Derived, typename முனை_2, typename Ref>
மாறிலி முனை_2& HashConsPool<Derived, முனை_2, Ref>::operator[] (Ref ref) மாறிலி {
	Ref idx = ref.untag();
	எனில் constexpr (Derived::STATIC_மொத்தம் != 0) {
		எனில் (is_static(idx))
			திரும்பு Derived::static_முனை(idx.raw());
	}
	திரும்பு backing[idx.raw() - Derived::STATIC_மொத்தம்];
}

template<typename Derived, typename முனை_2, typename Ref>
Derived* HashConsPool<Derived, முனை_2, Ref>::self() { திரும்பு static_cast<Derived*>(this); }

template<typename Derived, typename முனை_2, typename Ref>
மாறிலி Derived* HashConsPool<Derived, முனை_2, Ref>::self() மாறிலி { திரும்பு static_cast<மாறிலி Derived*>(this); }

template<typename Derived, typename முனை_2, typename Ref>
bool HashConsPool<Derived, முனை_2, Ref>::is_static(Ref ref) {
	எனில் constexpr (Derived::STATIC_மொத்தம் == 0)
		திரும்பு false;
	இல்லையெனில்
		திரும்பு ref.raw() < Derived::STATIC_மொத்தம்;
}

template<typename Derived, typename முனை_2, typename Ref>
வெறுமை HashConsPool<Derived, முனை_2, Ref>::RefIterator::settle() {
	போது (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

template<typename Derived, typename முனை_2, typename Ref>
Ref HashConsPool<Derived, முனை_2, Ref>::RefIterator::operator*() மாறிலி { திரும்பு Ref(idx); }

template<typename Derived, typename முனை_2, typename Ref>
auto HashConsPool<Derived, முனை_2, Ref>::RefIterator::operator++() -> RefIterator& { idx++; settle(); திரும்பு *this; }

template<typename Derived, typename முனை_2, typename Ref>
bool HashConsPool<Derived, முனை_2, Ref>::RefIterator::operator!=(மாறிலி RefIterator& other) மாறிலி { திரும்பு idx != other.idx; }

template<typename Derived, typename முனை_2, typename Ref>
auto HashConsPool<Derived, முனை_2, Ref>::RefRange::begin() மாறிலி -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	திரும்பு it;
}

template<typename Derived, typename முனை_2, typename Ref>
auto HashConsPool<Derived, முனை_2, Ref>::RefRange::end() மாறிலி -> RefIterator { திரும்பு RefIterator{pool, stop, stop, skip_dead}; }

template<typename Derived, typename முனை_2, typename Ref>
auto HashConsPool<Derived, முனை_2, Ref>::refs(bool include_statics) மாறிலி -> RefRange {
	திரும்பு RefRange{this, include_statics ? 0 : Derived::STATIC_மொத்தம்,
			Derived::STATIC_மொத்தம் + backing.size(), true};
}

template<typename Derived, typename முனை_2, typename Ref>
auto HashConsPool<Derived, முனை_2, Ref>::slots() மாறிலி -> RefRange {
	திரும்பு RefRange{this, Derived::STATIC_மொத்தம்,
			Derived::STATIC_மொத்தம் + backing.size(), false};
}

template<typename Derived, typename முனை_2, typename Ref>
size_t HashConsPool<Derived, முனை_2, Ref>::home_slot(uint64_t hash) மாறிலி {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	திரும்பு (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

template<typename Derived, typename முனை_2, typename Ref>
size_t HashConsPool<Derived, முனை_2, Ref>::next_slot(size_t slot) மாறிலி { திரும்பு (slot + 1) & (table.size() - 1); }

template<typename Derived, typename முனை_2, typename Ref>
size_t HashConsPool<Derived, முனை_2, Ref>::size() மாறிலி { திரும்பு backing.size() - free_list.size(); }

template<typename Derived, typename முனை_2, typename Ref>
bool HashConsPool<Derived, முனை_2, Ref>::slot_live(size_t abs) மாறிலி {
	எனில் (abs < Derived::STATIC_மொத்தம்)
		திரும்பு true;
	திரும்பு !backing[abs - Derived::STATIC_மொத்தம்].is_dead();
}

template<typename Derived, typename முனை_2, typename Ref>
HashConsPool<Derived, முனை_2, Ref>::HashConsPool() { rebuild_index(); }

template<typename Derived, typename முனை_2, typename Ref>
HashConsPool<Derived, முனை_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

template<typename Derived, typename முனை_2, typename Ref>
HashConsPool<Derived, முனை_2, Ref>& HashConsPool<Derived, முனை_2, Ref>::operator=(HashConsPool&& other) {
	எனில் (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	திரும்பு *this;
}

template<typename Derived, typename முனை_2, typename Ref>
வெறுமை HashConsPool<Derived, முனை_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

template<typename Derived, typename முனை_2, typename Ref>
வெறுமை HashConsPool<Derived, முனை_2, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	க்கு (size_t idx = 0; idx < backing.size(); ++idx)
		எனில் (backing[idx].is_dead())
			free_list.தள்ளு_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	table.assign(std::bit_ceil((Derived::STATIC_மொத்தம் + size()) * 2 + 2), Ref());
	க்கு (Ref ref : refs())
		index_செருகு(ref);
}

template<typename Derived, typename முனை_2, typename Ref>
template<typename Eq>
Ref HashConsPool<Derived, முனை_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) மாறிலி {
	க்கு (size_t slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		எனில் (Derived::hash_முனை((*this)[ref]) == hash && eq(ref))
			திரும்பு ref;
	}
	திரும்பு Ref();
}

template<typename Derived, typename முனை_2, typename Ref>
வெறுமை HashConsPool<Derived, முனை_2, Ref>::index_செருகு(Ref ref) {
	size_t slot = home_slot(Derived::hash_முனை((*this)[ref]));
	போது (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

template<typename Derived, typename முனை_2, typename Ref>
வெறுமை HashConsPool<Derived, முனை_2, Ref>::index_erase(Ref ref) {
	size_t hole = home_slot(Derived::hash_முனை((*this)[ref]));
	போது (table[hole] != ref)
		hole = next_slot(hole);
	size_t mask = table.size() - 1;
	க்கு (size_t slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		size_t home = home_slot(Derived::hash_முனை((*this)[table[slot]]));
		எனில் (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

template<typename Derived, typename முனை_2, typename Ref>
வெறுமை HashConsPool<Derived, முனை_2, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	க்கு (Ref ref : old)
		எனில் (ref != Ref())
			index_செருகு(ref);
}

template<typename Derived, typename முனை_2, typename Ref>
Ref HashConsPool<Derived, முனை_2, Ref>::add_inner(முனை_2 t) {
	Ref ref;
	எனில் (!free_list.empty()) {
		size_t idx = free_list.back();
		free_list.எடு_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_மொத்தம் + idx);
	} இல்லையெனில் {
		ref = Ref(Derived::STATIC_மொத்தம் + backing.size());
		backing.தள்ளு_back(std::move(t));
	}
	எனில் ((Derived::STATIC_மொத்தம் + size()) * 3 > table.size() * 2)
		grow_index();
	index_செருகு(ref);
	எனில் (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	திரும்பு ref;
}

template<typename Derived, typename முனை_2, typename Ref>
template<typename Roots>
size_t HashConsPool<Derived, முனை_2, Ref>::gc(மாறிலி Roots& roots) {
	pool<Ref> live;
	க்கு (Ref ref : roots)
		mark_live(ref, live);
	size_t erased = 0;
	க்கு (size_t idx = 0; idx < backing.size(); ++idx) {
		எனில் (backing[idx].is_dead())
			தொடர்;
		எனில் (!live.மொத்தம்(Ref(Derived::STATIC_மொத்தம் + idx))) {
			index_erase(Ref(Derived::STATIC_மொத்தம் + idx));
			free_list.தள்ளு_back(idx);
			backing[idx] = முனை_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	திரும்பு erased;
}

template<typename Derived, typename முனை_2, typename Ref>
வெறுமை HashConsPool<Derived, முனை_2, Ref>::mark_live(Ref ref, pool<Ref>& live) மாறிலி {
	ref = ref.untag();
	எனில் (ref == Ref() || is_static(ref) || !live.செருகு(ref).second)
		திரும்பு;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
