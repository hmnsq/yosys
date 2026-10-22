#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#अन्तर्भाव "kernel/hashcons.h"

#अन्तर्भाव <algorithm>

YOSYS_NAMESPACE_BEGIN

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
bool HashConsPool<Derived, गाँठो_2, Ref>::is_live(Ref ref) अचर {
	Ref idx = ref.untag();
	यदि (is_static(idx))
		फर्काऊ true;
	माप_प्रकार slot = idx.raw() - Derived::STATIC_गन्ती;
	फर्काऊ slot < backing.size() && !backing[slot].is_dead();
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
शून्य HashConsPool<Derived, गाँठो_2, Ref>::check_ready() {}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
अचर गाँठो_2& HashConsPool<Derived, गाँठो_2, Ref>::चालक[] (Ref ref) अचर {
	Ref idx = ref.untag();
	यदि constexpr (Derived::STATIC_गन्ती != 0) {
		यदि (is_static(idx))
			फर्काऊ Derived::static_गाँठो(idx.raw());
	}
	फर्काऊ backing[idx.raw() - Derived::STATIC_गन्ती];
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
Derived* HashConsPool<Derived, गाँठो_2, Ref>::self() { फर्काऊ static_cast<Derived*>(this); }

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
अचर Derived* HashConsPool<Derived, गाँठो_2, Ref>::self() अचर { फर्काऊ static_cast<अचर Derived*>(this); }

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
bool HashConsPool<Derived, गाँठो_2, Ref>::is_static(Ref ref) {
	यदि constexpr (Derived::STATIC_गन्ती == 0)
		फर्काऊ false;
	अन्यथा
		फर्काऊ ref.raw() < Derived::STATIC_गन्ती;
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
शून्य HashConsPool<Derived, गाँठो_2, Ref>::RefIterator::settle() {
	जबसम्म (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
Ref HashConsPool<Derived, गाँठो_2, Ref>::RefIterator::चालक*() अचर { फर्काऊ Ref(idx); }

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठो_2, Ref>::RefIterator::चालक++() -> RefIterator& { idx++; settle(); फर्काऊ *this; }

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
bool HashConsPool<Derived, गाँठो_2, Ref>::RefIterator::चालक!=(अचर RefIterator& other) अचर { फर्काऊ idx != other.idx; }

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठो_2, Ref>::RefRange::begin() अचर -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	फर्काऊ it;
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठो_2, Ref>::RefRange::end() अचर -> RefIterator { फर्काऊ RefIterator{pool, stop, stop, skip_dead}; }

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठो_2, Ref>::refs(bool include_statics) अचर -> RefRange {
	फर्काऊ RefRange{this, include_statics ? 0 : Derived::STATIC_गन्ती,
			Derived::STATIC_गन्ती + backing.size(), true};
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठो_2, Ref>::slots() अचर -> RefRange {
	फर्काऊ RefRange{this, Derived::STATIC_गन्ती,
			Derived::STATIC_गन्ती + backing.size(), false};
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
माप_प्रकार HashConsPool<Derived, गाँठो_2, Ref>::home_slot(uint64_t hash) अचर {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	फर्काऊ (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
माप_प्रकार HashConsPool<Derived, गाँठो_2, Ref>::next_slot(माप_प्रकार slot) अचर { फर्काऊ (slot + 1) & (table.size() - 1); }

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
माप_प्रकार HashConsPool<Derived, गाँठो_2, Ref>::size() अचर { फर्काऊ backing.size() - free_list.size(); }

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
bool HashConsPool<Derived, गाँठो_2, Ref>::slot_live(माप_प्रकार असल) अचर {
	यदि (असल < Derived::STATIC_गन्ती)
		फर्काऊ true;
	फर्काऊ !backing[असल - Derived::STATIC_गन्ती].is_dead();
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
HashConsPool<Derived, गाँठो_2, Ref>::HashConsPool() { rebuild_index(); }

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
HashConsPool<Derived, गाँठो_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
HashConsPool<Derived, गाँठो_2, Ref>& HashConsPool<Derived, गाँठो_2, Ref>::चालक=(HashConsPool&& other) {
	यदि (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	फर्काऊ *this;
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
शून्य HashConsPool<Derived, गाँठो_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
शून्य HashConsPool<Derived, गाँठो_2, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	लागि (माप_प्रकार idx = 0; idx < backing.size(); ++idx)
		यदि (backing[idx].is_dead())
			free_list.थप_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<माप_प्रकार>());
	table.assign(std::bit_ceil((Derived::STATIC_गन्ती + size()) * 2 + 2), Ref());
	लागि (Ref ref : refs())
		index_घुसाउ(ref);
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
ढाँचा<typename Eq>
Ref HashConsPool<Derived, गाँठो_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) अचर {
	लागि (माप_प्रकार slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		यदि (Derived::hash_गाँठो((*this)[ref]) == hash && eq(ref))
			फर्काऊ ref;
	}
	फर्काऊ Ref();
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
शून्य HashConsPool<Derived, गाँठो_2, Ref>::index_घुसाउ(Ref ref) {
	माप_प्रकार slot = home_slot(Derived::hash_गाँठो((*this)[ref]));
	जबसम्म (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
शून्य HashConsPool<Derived, गाँठो_2, Ref>::index_erase(Ref ref) {
	माप_प्रकार hole = home_slot(Derived::hash_गाँठो((*this)[ref]));
	जबसम्म (table[hole] != ref)
		hole = next_slot(hole);
	माप_प्रकार mask = table.size() - 1;
	लागि (माप_प्रकार slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		माप_प्रकार home = home_slot(Derived::hash_गाँठो((*this)[table[slot]]));
		यदि (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
शून्य HashConsPool<Derived, गाँठो_2, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	लागि (Ref ref : old)
		यदि (ref != Ref())
			index_घुसाउ(ref);
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
Ref HashConsPool<Derived, गाँठो_2, Ref>::add_inner(गाँठो_2 t) {
	Ref ref;
	यदि (!free_list.empty()) {
		माप_प्रकार idx = free_list.back();
		free_list.झिक_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_गन्ती + idx);
	} अन्यथा {
		ref = Ref(Derived::STATIC_गन्ती + backing.size());
		backing.थप_back(std::move(t));
	}
	यदि ((Derived::STATIC_गन्ती + size()) * 3 > table.size() * 2)
		grow_index();
	index_घुसाउ(ref);
	यदि (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	फर्काऊ ref;
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
ढाँचा<typename Roots>
माप_प्रकार HashConsPool<Derived, गाँठो_2, Ref>::gc(अचर Roots& roots) {
	pool<Ref> live;
	लागि (Ref ref : roots)
		mark_live(ref, live);
	माप_प्रकार erased = 0;
	लागि (माप_प्रकार idx = 0; idx < backing.size(); ++idx) {
		यदि (backing[idx].is_dead())
			जारी;
		यदि (!live.गन्ती(Ref(Derived::STATIC_गन्ती + idx))) {
			index_erase(Ref(Derived::STATIC_गन्ती + idx));
			free_list.थप_back(idx);
			backing[idx] = गाँठो_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<माप_प्रकार>());
	फर्काऊ erased;
}

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
शून्य HashConsPool<Derived, गाँठो_2, Ref>::mark_live(Ref ref, pool<Ref>& live) अचर {
	ref = ref.untag();
	यदि (ref == Ref() || is_static(ref) || !live.घुसाउ(ref).second)
		फर्काऊ;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
