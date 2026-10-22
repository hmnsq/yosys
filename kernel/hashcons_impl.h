#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#अन्तर्भाव "kernel/hashcons.h"

#अन्तर्भाव <algorithm>

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
bool HashConsPool<Derived, ग्रन्थिः_2, Ref>::is_live(Ref ref) ध्रुव {
	Ref idx = ref.untag();
	जइ (is_static(idx))
		प्रत्यागम true;
	size_t slot = idx.raw() - Derived::STATIC_गणनम्;
	प्रत्यागम slot < backing.size() && !backing[slot].is_dead();
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::check_ready() {}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
ध्रुव ग्रन्थिः_2& HashConsPool<Derived, ग्रन्थिः_2, Ref>::operator[] (Ref ref) ध्रुव {
	Ref idx = ref.untag();
	जइ constexpr (Derived::STATIC_गणनम् != 0) {
		जइ (is_static(idx))
			प्रत्यागम Derived::static_ग्रन्थिः(idx.raw());
	}
	प्रत्यागम backing[idx.raw() - Derived::STATIC_गणनम्];
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
Derived* HashConsPool<Derived, ग्रन्थिः_2, Ref>::self() { प्रत्यागम static_cast<Derived*>(this); }

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
ध्रुव Derived* HashConsPool<Derived, ग्रन्थिः_2, Ref>::self() ध्रुव { प्रत्यागम static_cast<ध्रुव Derived*>(this); }

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
bool HashConsPool<Derived, ग्रन्थिः_2, Ref>::is_static(Ref ref) {
	जइ constexpr (Derived::STATIC_गणनम् == 0)
		प्रत्यागम false;
	अण्णहा
		प्रत्यागम ref.raw() < Derived::STATIC_गणनम्;
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::RefIterator::settle() {
	जाव (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
Ref HashConsPool<Derived, ग्रन्थिः_2, Ref>::RefIterator::operator*() ध्रुव { प्रत्यागम Ref(idx); }

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
auto HashConsPool<Derived, ग्रन्थिः_2, Ref>::RefIterator::operator++() -> RefIterator& { idx++; settle(); प्रत्यागम *this; }

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
bool HashConsPool<Derived, ग्रन्थिः_2, Ref>::RefIterator::operator!=(ध्रुव RefIterator& other) ध्रुव { प्रत्यागम idx != other.idx; }

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
auto HashConsPool<Derived, ग्रन्थिः_2, Ref>::RefRange::begin() ध्रुव -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	प्रत्यागम it;
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
auto HashConsPool<Derived, ग्रन्थिः_2, Ref>::RefRange::end() ध्रुव -> RefIterator { प्रत्यागम RefIterator{pool, stop, stop, skip_dead}; }

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
auto HashConsPool<Derived, ग्रन्थिः_2, Ref>::refs(bool include_statics) ध्रुव -> RefRange {
	प्रत्यागम RefRange{this, include_statics ? 0 : Derived::STATIC_गणनम्,
			Derived::STATIC_गणनम् + backing.size(), true};
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
auto HashConsPool<Derived, ग्रन्थिः_2, Ref>::slots() ध्रुव -> RefRange {
	प्रत्यागम RefRange{this, Derived::STATIC_गणनम्,
			Derived::STATIC_गणनम् + backing.size(), false};
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
size_t HashConsPool<Derived, ग्रन्थिः_2, Ref>::home_slot(uint64_t hash) ध्रुव {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	प्रत्यागम (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
size_t HashConsPool<Derived, ग्रन्थिः_2, Ref>::next_slot(size_t slot) ध्रुव { प्रत्यागम (slot + 1) & (table.size() - 1); }

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
size_t HashConsPool<Derived, ग्रन्थिः_2, Ref>::size() ध्रुव { प्रत्यागम backing.size() - free_list.size(); }

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
bool HashConsPool<Derived, ग्रन्थिः_2, Ref>::slot_live(size_t abs) ध्रुव {
	जइ (abs < Derived::STATIC_गणनम्)
		प्रत्यागम true;
	प्रत्यागम !backing[abs - Derived::STATIC_गणनम्].is_dead();
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
HashConsPool<Derived, ग्रन्थिः_2, Ref>::HashConsPool() { rebuild_index(); }

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
HashConsPool<Derived, ग्रन्थिः_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
HashConsPool<Derived, ग्रन्थिः_2, Ref>& HashConsPool<Derived, ग्रन्थिः_2, Ref>::operator=(HashConsPool&& other) {
	जइ (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	प्रत्यागम *this;
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	प्रति (size_t idx = 0; idx < backing.size(); ++idx)
		जइ (backing[idx].is_dead())
			free_list.प्रक्षिप_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	table.assign(std::bit_ceil((Derived::STATIC_गणनम् + size()) * 2 + 2), Ref());
	प्रति (Ref ref : refs())
		index_निवेशय(ref);
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
template<typename Eq>
Ref HashConsPool<Derived, ग्रन्थिः_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) ध्रुव {
	प्रति (size_t slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		जइ (Derived::hash_ग्रन्थिः((*this)[ref]) == hash && eq(ref))
			प्रत्यागम ref;
	}
	प्रत्यागम Ref();
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::index_निवेशय(Ref ref) {
	size_t slot = home_slot(Derived::hash_ग्रन्थिः((*this)[ref]));
	जाव (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::index_erase(Ref ref) {
	size_t hole = home_slot(Derived::hash_ग्रन्थिः((*this)[ref]));
	जाव (table[hole] != ref)
		hole = next_slot(hole);
	size_t mask = table.size() - 1;
	प्रति (size_t slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		size_t home = home_slot(Derived::hash_ग्रन्थिः((*this)[table[slot]]));
		जइ (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	प्रति (Ref ref : old)
		जइ (ref != Ref())
			index_निवेशय(ref);
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
Ref HashConsPool<Derived, ग्रन्थिः_2, Ref>::add_inner(ग्रन्थिः_2 t) {
	Ref ref;
	जइ (!free_list.empty()) {
		size_t idx = free_list.back();
		free_list.निष्कासय_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_गणनम् + idx);
	} अण्णहा {
		ref = Ref(Derived::STATIC_गणनम् + backing.size());
		backing.प्रक्षिप_back(std::move(t));
	}
	जइ ((Derived::STATIC_गणनम् + size()) * 3 > table.size() * 2)
		grow_index();
	index_निवेशय(ref);
	जइ (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	प्रत्यागम ref;
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
template<typename Roots>
size_t HashConsPool<Derived, ग्रन्थिः_2, Ref>::gc(ध्रुव Roots& roots) {
	pool<Ref> live;
	प्रति (Ref ref : roots)
		mark_live(ref, live);
	size_t erased = 0;
	प्रति (size_t idx = 0; idx < backing.size(); ++idx) {
		जइ (backing[idx].is_dead())
			अनुवर्त;
		जइ (!live.गणनम्(Ref(Derived::STATIC_गणनम् + idx))) {
			index_erase(Ref(Derived::STATIC_गणनम् + idx));
			free_list.प्रक्षिप_back(idx);
			backing[idx] = ग्रन्थिः_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	प्रत्यागम erased;
}

template<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::mark_live(Ref ref, pool<Ref>& live) ध्रुव {
	ref = ref.untag();
	जइ (ref == Ref() || is_static(ref) || !live.निवेशय(ref).second)
		प्रत्यागम;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
