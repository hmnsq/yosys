#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#अन्तर्भाव "kernel/hashcons.h"

#अन्तर्भाव <algorithm>

YOSYS_NAMESPACE_BEGIN

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
bool HashConsPool<Derived, गाँठ_2, Ref>::is_live(Ref ref) ध्रुव {
	Ref idx = ref.untag();
	यदि (is_static(idx))
		प्रत्यागम true;
	माप_प्रकार slot = idx.raw() - Derived::STATIC_गिनती;
	प्रत्यागम slot < backing.size() && !backing[slot].is_dead();
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
शून्य HashConsPool<Derived, गाँठ_2, Ref>::check_ready() {}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
ध्रुव गाँठ_2& HashConsPool<Derived, गाँठ_2, Ref>::चालक[] (Ref ref) ध्रुव {
	Ref idx = ref.untag();
	यदि constexpr (Derived::STATIC_गिनती != 0) {
		यदि (is_static(idx))
			प्रत्यागम Derived::static_गाँठ(idx.raw());
	}
	प्रत्यागम backing[idx.raw() - Derived::STATIC_गिनती];
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
Derived* HashConsPool<Derived, गाँठ_2, Ref>::self() { प्रत्यागम static_cast<Derived*>(this); }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
ध्रुव Derived* HashConsPool<Derived, गाँठ_2, Ref>::self() ध्रुव { प्रत्यागम static_cast<ध्रुव Derived*>(this); }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
bool HashConsPool<Derived, गाँठ_2, Ref>::is_static(Ref ref) {
	यदि constexpr (Derived::STATIC_गिनती == 0)
		प्रत्यागम false;
	अन्यथा
		प्रत्यागम ref.raw() < Derived::STATIC_गिनती;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
शून्य HashConsPool<Derived, गाँठ_2, Ref>::RefIterator::settle() {
	यावत् (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
Ref HashConsPool<Derived, गाँठ_2, Ref>::RefIterator::चालक*() ध्रुव { प्रत्यागम Ref(idx); }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठ_2, Ref>::RefIterator::चालक++() -> RefIterator& { idx++; settle(); प्रत्यागम *this; }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
bool HashConsPool<Derived, गाँठ_2, Ref>::RefIterator::चालक!=(ध्रुव RefIterator& other) ध्रुव { प्रत्यागम idx != other.idx; }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठ_2, Ref>::RefRange::begin() ध्रुव -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	प्रत्यागम it;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठ_2, Ref>::RefRange::end() ध्रुव -> RefIterator { प्रत्यागम RefIterator{pool, stop, stop, skip_dead}; }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठ_2, Ref>::refs(bool include_statics) ध्रुव -> RefRange {
	प्रत्यागम RefRange{this, include_statics ? 0 : Derived::STATIC_गिनती,
			Derived::STATIC_गिनती + backing.size(), true};
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठ_2, Ref>::slots() ध्रुव -> RefRange {
	प्रत्यागम RefRange{this, Derived::STATIC_गिनती,
			Derived::STATIC_गिनती + backing.size(), false};
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
माप_प्रकार HashConsPool<Derived, गाँठ_2, Ref>::home_slot(uint64_t hash) ध्रुव {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	प्रत्यागम (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
माप_प्रकार HashConsPool<Derived, गाँठ_2, Ref>::next_slot(माप_प्रकार slot) ध्रुव { प्रत्यागम (slot + 1) & (table.size() - 1); }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
माप_प्रकार HashConsPool<Derived, गाँठ_2, Ref>::size() ध्रुव { प्रत्यागम backing.size() - free_list.size(); }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
bool HashConsPool<Derived, गाँठ_2, Ref>::slot_live(माप_प्रकार असल) ध्रुव {
	यदि (असल < Derived::STATIC_गिनती)
		प्रत्यागम true;
	प्रत्यागम !backing[असल - Derived::STATIC_गिनती].is_dead();
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
HashConsPool<Derived, गाँठ_2, Ref>::HashConsPool() { rebuild_index(); }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
HashConsPool<Derived, गाँठ_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
HashConsPool<Derived, गाँठ_2, Ref>& HashConsPool<Derived, गाँठ_2, Ref>::चालक=(HashConsPool&& other) {
	यदि (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	प्रत्यागम *this;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
शून्य HashConsPool<Derived, गाँठ_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
शून्य HashConsPool<Derived, गाँठ_2, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	प्रति (माप_प्रकार idx = 0; idx < backing.size(); ++idx)
		यदि (backing[idx].is_dead())
			free_list.धकेलो_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<माप_प्रकार>());
	table.assign(std::bit_ceil((Derived::STATIC_गिनती + size()) * 2 + 2), Ref());
	प्रति (Ref ref : refs())
		index_जोड़ो(ref);
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
ढाँचा<typename Eq>
Ref HashConsPool<Derived, गाँठ_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) ध्रुव {
	प्रति (माप_प्रकार slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		यदि (Derived::hash_गाँठ((*this)[ref]) == hash && eq(ref))
			प्रत्यागम ref;
	}
	प्रत्यागम Ref();
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
शून्य HashConsPool<Derived, गाँठ_2, Ref>::index_जोड़ो(Ref ref) {
	माप_प्रकार slot = home_slot(Derived::hash_गाँठ((*this)[ref]));
	यावत् (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
शून्य HashConsPool<Derived, गाँठ_2, Ref>::index_erase(Ref ref) {
	माप_प्रकार hole = home_slot(Derived::hash_गाँठ((*this)[ref]));
	यावत् (table[hole] != ref)
		hole = next_slot(hole);
	माप_प्रकार mask = table.size() - 1;
	प्रति (माप_प्रकार slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		माप_प्रकार home = home_slot(Derived::hash_गाँठ((*this)[table[slot]]));
		यदि (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
शून्य HashConsPool<Derived, गाँठ_2, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	प्रति (Ref ref : old)
		यदि (ref != Ref())
			index_जोड़ो(ref);
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
Ref HashConsPool<Derived, गाँठ_2, Ref>::add_inner(गाँठ_2 t) {
	Ref ref;
	यदि (!free_list.empty()) {
		माप_प्रकार idx = free_list.back();
		free_list.निकालो_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_गिनती + idx);
	} अन्यथा {
		ref = Ref(Derived::STATIC_गिनती + backing.size());
		backing.धकेलो_back(std::move(t));
	}
	यदि ((Derived::STATIC_गिनती + size()) * 3 > table.size() * 2)
		grow_index();
	index_जोड़ो(ref);
	यदि (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	प्रत्यागम ref;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
ढाँचा<typename Roots>
माप_प्रकार HashConsPool<Derived, गाँठ_2, Ref>::gc(ध्रुव Roots& roots) {
	pool<Ref> live;
	प्रति (Ref ref : roots)
		mark_live(ref, live);
	माप_प्रकार erased = 0;
	प्रति (माप_प्रकार idx = 0; idx < backing.size(); ++idx) {
		यदि (backing[idx].is_dead())
			अनुवर्त;
		यदि (!live.गिनती(Ref(Derived::STATIC_गिनती + idx))) {
			index_erase(Ref(Derived::STATIC_गिनती + idx));
			free_list.धकेलो_back(idx);
			backing[idx] = गाँठ_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<माप_प्रकार>());
	प्रत्यागम erased;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
शून्य HashConsPool<Derived, गाँठ_2, Ref>::mark_live(Ref ref, pool<Ref>& live) ध्रुव {
	ref = ref.untag();
	यदि (ref == Ref() || is_static(ref) || !live.जोड़ो(ref).second)
		प्रत्यागम;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
