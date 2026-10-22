#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#ଅନ୍ତର୍ଭାଵ "kernel/hashcons.h"

#ଅନ୍ତର୍ଭାଵ <algorithm>

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
bool HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::is_live(Ref ref) ଅଚଳ {
	Ref idx = ref.untag();
	ଯଦି (is_static(idx))
		ଫେରନ୍ତୁ true;
	size_t slot = idx.raw() - Derived::STATIC_ଗଣନା;
	ଫେରନ୍ତୁ slot < backing.size() && !backing[slot].is_dead();
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
ଶୂନ୍ୟ HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::check_ready() {}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
ଅଚଳ ଗଣ୍ଠି_2& HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::operator[] (Ref ref) ଅଚଳ {
	Ref idx = ref.untag();
	ଯଦି constexpr (Derived::STATIC_ଗଣନା != 0) {
		ଯଦି (is_static(idx))
			ଫେରନ୍ତୁ Derived::static_ଗଣ୍ଠି(idx.raw());
	}
	ଫେରନ୍ତୁ backing[idx.raw() - Derived::STATIC_ଗଣନା];
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
Derived* HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::self() { ଫେରନ୍ତୁ static_cast<Derived*>(this); }

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
ଅଚଳ Derived* HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::self() ଅଚଳ { ଫେରନ୍ତୁ static_cast<ଅଚଳ Derived*>(this); }

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
bool HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::is_static(Ref ref) {
	ଯଦି constexpr (Derived::STATIC_ଗଣନା == 0)
		ଫେରନ୍ତୁ false;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ ref.raw() < Derived::STATIC_ଗଣନା;
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
ଶୂନ୍ୟ HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::RefIterator::settle() {
	ଯେତେବେଳେ (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
Ref HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::RefIterator::operator*() ଅଚଳ { ଫେରନ୍ତୁ Ref(idx); }

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
auto HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::RefIterator::operator++() -> RefIterator& { idx++; settle(); ଫେରନ୍ତୁ *this; }

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
bool HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::RefIterator::operator!=(ଅଚଳ RefIterator& other) ଅଚଳ { ଫେରନ୍ତୁ idx != other.idx; }

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
auto HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::RefRange::begin() ଅଚଳ -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	ଫେରନ୍ତୁ it;
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
auto HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::RefRange::end() ଅଚଳ -> RefIterator { ଫେରନ୍ତୁ RefIterator{pool, stop, stop, skip_dead}; }

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
auto HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::refs(bool include_statics) ଅଚଳ -> RefRange {
	ଫେରନ୍ତୁ RefRange{this, include_statics ? 0 : Derived::STATIC_ଗଣନା,
			Derived::STATIC_ଗଣନା + backing.size(), true};
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
auto HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::slots() ଅଚଳ -> RefRange {
	ଫେରନ୍ତୁ RefRange{this, Derived::STATIC_ଗଣନା,
			Derived::STATIC_ଗଣନା + backing.size(), false};
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
size_t HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::home_slot(uint64_t hash) ଅଚଳ {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	ଫେରନ୍ତୁ (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
size_t HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::next_slot(size_t slot) ଅଚଳ { ଫେରନ୍ତୁ (slot + 1) & (table.size() - 1); }

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
size_t HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::size() ଅଚଳ { ଫେରନ୍ତୁ backing.size() - free_list.size(); }

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
bool HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::slot_live(size_t abs) ଅଚଳ {
	ଯଦି (abs < Derived::STATIC_ଗଣନା)
		ଫେରନ୍ତୁ true;
	ଫେରନ୍ତୁ !backing[abs - Derived::STATIC_ଗଣନା].is_dead();
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::HashConsPool() { rebuild_index(); }

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>& HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::operator=(HashConsPool&& other) {
	ଯଦି (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	ଫେରନ୍ତୁ *this;
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
ଶୂନ୍ୟ HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
ଶୂନ୍ୟ HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	ପାଇଁ (size_t idx = 0; idx < backing.size(); ++idx)
		ଯଦି (backing[idx].is_dead())
			free_list.ଠେଲ_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	table.assign(std::bit_ceil((Derived::STATIC_ଗଣନା + size()) * 2 + 2), Ref());
	ପାଇଁ (Ref ref : refs())
		index_ଭର୍ତ୍ତି(ref);
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
template<typename Eq>
Ref HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) ଅଚଳ {
	ପାଇଁ (size_t slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		ଯଦି (Derived::hash_ଗଣ୍ଠି((*this)[ref]) == hash && eq(ref))
			ଫେରନ୍ତୁ ref;
	}
	ଫେରନ୍ତୁ Ref();
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
ଶୂନ୍ୟ HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::index_ଭର୍ତ୍ତି(Ref ref) {
	size_t slot = home_slot(Derived::hash_ଗଣ୍ଠି((*this)[ref]));
	ଯେତେବେଳେ (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
ଶୂନ୍ୟ HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::index_erase(Ref ref) {
	size_t hole = home_slot(Derived::hash_ଗଣ୍ଠି((*this)[ref]));
	ଯେତେବେଳେ (table[hole] != ref)
		hole = next_slot(hole);
	size_t mask = table.size() - 1;
	ପାଇଁ (size_t slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		size_t home = home_slot(Derived::hash_ଗଣ୍ଠି((*this)[table[slot]]));
		ଯଦି (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
ଶୂନ୍ୟ HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	ପାଇଁ (Ref ref : old)
		ଯଦି (ref != Ref())
			index_ଭର୍ତ୍ତି(ref);
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
Ref HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::add_inner(ଗଣ୍ଠି_2 t) {
	Ref ref;
	ଯଦି (!free_list.empty()) {
		size_t idx = free_list.back();
		free_list.କାଢ଼_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_ଗଣନା + idx);
	} ଅନ୍ୟଥା {
		ref = Ref(Derived::STATIC_ଗଣନା + backing.size());
		backing.ଠେଲ_back(std::move(t));
	}
	ଯଦି ((Derived::STATIC_ଗଣନା + size()) * 3 > table.size() * 2)
		grow_index();
	index_ଭର୍ତ୍ତି(ref);
	ଯଦି (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	ଫେରନ୍ତୁ ref;
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
template<typename Roots>
size_t HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::gc(ଅଚଳ Roots& roots) {
	pool<Ref> live;
	ପାଇଁ (Ref ref : roots)
		mark_live(ref, live);
	size_t erased = 0;
	ପାଇଁ (size_t idx = 0; idx < backing.size(); ++idx) {
		ଯଦି (backing[idx].is_dead())
			ଜାରି;
		ଯଦି (!live.ଗଣନା_2(Ref(Derived::STATIC_ଗଣନା + idx))) {
			index_erase(Ref(Derived::STATIC_ଗଣନା + idx));
			free_list.ଠେଲ_back(idx);
			backing[idx] = ଗଣ୍ଠି_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	ଫେରନ୍ତୁ erased;
}

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
ଶୂନ୍ୟ HashConsPool<Derived, ଗଣ୍ଠି_2, Ref>::mark_live(Ref ref, pool<Ref>& live) ଅଚଳ {
	ref = ref.untag();
	ଯଦି (ref == Ref() || is_static(ref) || !live.ଭର୍ତ୍ତି(ref).second)
		ଫେରନ୍ତୁ;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
