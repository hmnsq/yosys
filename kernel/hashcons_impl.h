#अगर_अघोषित YOSYS_HASHCONS_IMPL_H
#घोषणा YOSYS_HASHCONS_IMPL_H

#समावेश "kernel/hashcons.h"

#समावेश <algorithm>

YOSYS_NAMESPACE_BEGIN

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
bool HashConsPool<Derived, गाँठ_2, Ref>::is_live(Ref ref) स्थिर {
	Ref idx = ref.untag();
	अगर (is_static(idx))
		वापस true;
	माप_प्रकार slot = idx.raw() - Derived::STATIC_गिनती;
	वापस slot < backing.आकार() && !backing[slot].is_dead();
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
व्योम HashConsPool<Derived, गाँठ_2, Ref>::check_ready() {}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
स्थिर गाँठ_2& HashConsPool<Derived, गाँठ_2, Ref>::चालक[] (Ref ref) स्थिर {
	Ref idx = ref.untag();
	अगर constexpr (Derived::STATIC_गिनती != 0) {
		अगर (is_static(idx))
			वापस Derived::static_गाँठ(idx.raw());
	}
	वापस backing[idx.raw() - Derived::STATIC_गिनती];
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
Derived* HashConsPool<Derived, गाँठ_2, Ref>::self() { वापस static_cast<Derived*>(this); }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
स्थिर Derived* HashConsPool<Derived, गाँठ_2, Ref>::self() स्थिर { वापस static_cast<स्थिर Derived*>(this); }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
bool HashConsPool<Derived, गाँठ_2, Ref>::is_static(Ref ref) {
	अगर constexpr (Derived::STATIC_गिनती == 0)
		वापस false;
	अन्यथा
		वापस ref.raw() < Derived::STATIC_गिनती;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
व्योम HashConsPool<Derived, गाँठ_2, Ref>::RefIterator::settle() {
	जबतक (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
Ref HashConsPool<Derived, गाँठ_2, Ref>::RefIterator::चालक*() स्थिर { वापस Ref(idx); }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठ_2, Ref>::RefIterator::चालक++() -> RefIterator& { idx++; settle(); वापस *this; }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
bool HashConsPool<Derived, गाँठ_2, Ref>::RefIterator::चालक!=(स्थिर RefIterator& other) स्थिर { वापस idx != other.idx; }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठ_2, Ref>::RefRange::begin() स्थिर -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	वापस it;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठ_2, Ref>::RefRange::end() स्थिर -> RefIterator { वापस RefIterator{pool, stop, stop, skip_dead}; }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठ_2, Ref>::refs(bool include_statics) स्थिर -> RefRange {
	वापस RefRange{this, include_statics ? 0 : Derived::STATIC_गिनती,
			Derived::STATIC_गिनती + backing.आकार(), true};
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाँठ_2, Ref>::slots() स्थिर -> RefRange {
	वापस RefRange{this, Derived::STATIC_गिनती,
			Derived::STATIC_गिनती + backing.आकार(), false};
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
माप_प्रकार HashConsPool<Derived, गाँठ_2, Ref>::home_slot(uint64_t hash) स्थिर {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	वापस (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.आकार()));
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
माप_प्रकार HashConsPool<Derived, गाँठ_2, Ref>::अग्र_slot(माप_प्रकार slot) स्थिर { वापस (slot + 1) & (table.आकार() - 1); }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
माप_प्रकार HashConsPool<Derived, गाँठ_2, Ref>::आकार() स्थिर { वापस backing.आकार() - free_list.आकार(); }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
bool HashConsPool<Derived, गाँठ_2, Ref>::slot_live(माप_प्रकार असल) स्थिर {
	अगर (असल < Derived::STATIC_गिनती)
		वापस true;
	वापस !backing[असल - Derived::STATIC_गिनती].is_dead();
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
HashConsPool<Derived, गाँठ_2, Ref>::HashConsPool() { rebuild_सूचकांक(); }

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
HashConsPool<Derived, गाँठ_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
HashConsPool<Derived, गाँठ_2, Ref>& HashConsPool<Derived, गाँठ_2, Ref>::चालक=(HashConsPool&& other) {
	अगर (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	वापस *this;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
व्योम HashConsPool<Derived, गाँठ_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_सूचकांक();
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
व्योम HashConsPool<Derived, गाँठ_2, Ref>::rebuild_सूचकांक() {
	Derived::check_ready();
	free_list.clear();
	क्रम (माप_प्रकार idx = 0; idx < backing.आकार(); ++idx)
		अगर (backing[idx].is_dead())
			free_list.धकेलो_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<माप_प्रकार>());
	table.assign(std::bit_ceil((Derived::STATIC_गिनती + आकार()) * 2 + 2), Ref());
	क्रम (Ref ref : refs())
		सूचकांक_जोड़ो(ref);
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
ढाँचा<typename Eq>
Ref HashConsPool<Derived, गाँठ_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) स्थिर {
	क्रम (माप_प्रकार slot = home_slot(hash); table[slot] != Ref(); slot = अग्र_slot(slot)) {
		Ref ref = table[slot];
		अगर (Derived::hash_गाँठ((*this)[ref]) == hash && eq(ref))
			वापस ref;
	}
	वापस Ref();
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
व्योम HashConsPool<Derived, गाँठ_2, Ref>::सूचकांक_जोड़ो(Ref ref) {
	माप_प्रकार slot = home_slot(Derived::hash_गाँठ((*this)[ref]));
	जबतक (table[slot] != Ref())
		slot = अग्र_slot(slot);
	table[slot] = ref;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
व्योम HashConsPool<Derived, गाँठ_2, Ref>::सूचकांक_erase(Ref ref) {
	माप_प्रकार hole = home_slot(Derived::hash_गाँठ((*this)[ref]));
	जबतक (table[hole] != ref)
		hole = अग्र_slot(hole);
	माप_प्रकार mask = table.आकार() - 1;
	क्रम (माप_प्रकार slot = अग्र_slot(hole); table[slot] != Ref(); slot = अग्र_slot(slot)) {
		माप_प्रकार home = home_slot(Derived::hash_गाँठ((*this)[table[slot]]));
		अगर (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
व्योम HashConsPool<Derived, गाँठ_2, Ref>::grow_सूचकांक() {
	std::vector<Ref> old(table.आकार() * 2, Ref());
	std::swap(old, table);
	क्रम (Ref ref : old)
		अगर (ref != Ref())
			सूचकांक_जोड़ो(ref);
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
Ref HashConsPool<Derived, गाँठ_2, Ref>::add_inner(गाँठ_2 t) {
	Ref ref;
	अगर (!free_list.empty()) {
		माप_प्रकार idx = free_list.back();
		free_list.निकालो_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_गिनती + idx);
	} अन्यथा {
		ref = Ref(Derived::STATIC_गिनती + backing.आकार());
		backing.धकेलो_back(std::move(t));
	}
	अगर ((Derived::STATIC_गिनती + आकार()) * 3 > table.आकार() * 2)
		grow_सूचकांक();
	सूचकांक_जोड़ो(ref);
	अगर (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	वापस ref;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
ढाँचा<typename Roots>
माप_प्रकार HashConsPool<Derived, गाँठ_2, Ref>::gc(स्थिर Roots& roots) {
	pool<Ref> live;
	क्रम (Ref ref : roots)
		mark_live(ref, live);
	माप_प्रकार erased = 0;
	क्रम (माप_प्रकार idx = 0; idx < backing.आकार(); ++idx) {
		अगर (backing[idx].is_dead())
			जारी;
		अगर (!live.गिनती(Ref(Derived::STATIC_गिनती + idx))) {
			सूचकांक_erase(Ref(Derived::STATIC_गिनती + idx));
			free_list.धकेलो_back(idx);
			backing[idx] = गाँठ_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<माप_प्रकार>());
	वापस erased;
}

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
व्योम HashConsPool<Derived, गाँठ_2, Ref>::mark_live(Ref ref, pool<Ref>& live) स्थिर {
	ref = ref.untag();
	अगर (ref == Ref() || is_static(ref) || !live.जोड़ो(ref).second)
		वापस;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#पूर्ण_अगर
