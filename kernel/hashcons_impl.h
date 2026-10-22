#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#अन्तर्भाव "kernel/hashcons.h"

#अन्तर्भाव <algorithm>

YOSYS_NAMESPACE_BEGIN

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
bool HashConsPool<Derived, गाठ_2, Ref>::is_live(Ref ref) स्थिरांक {
	Ref idx = ref.untag();
	जर (is_static(idx))
		परत true;
	माप_प्रकार slot = idx.raw() - Derived::STATIC_मोजणी;
	परत slot < backing.size() && !backing[slot].is_dead();
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
शून्य HashConsPool<Derived, गाठ_2, Ref>::check_ready() {}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
स्थिरांक गाठ_2& HashConsPool<Derived, गाठ_2, Ref>::चालक[] (Ref ref) स्थिरांक {
	Ref idx = ref.untag();
	जर constexpr (Derived::STATIC_मोजणी != 0) {
		जर (is_static(idx))
			परत Derived::static_गाठ(idx.raw());
	}
	परत backing[idx.raw() - Derived::STATIC_मोजणी];
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
Derived* HashConsPool<Derived, गाठ_2, Ref>::self() { परत static_cast<Derived*>(this); }

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
स्थिरांक Derived* HashConsPool<Derived, गाठ_2, Ref>::self() स्थिरांक { परत static_cast<स्थिरांक Derived*>(this); }

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
bool HashConsPool<Derived, गाठ_2, Ref>::is_static(Ref ref) {
	जर constexpr (Derived::STATIC_मोजणी == 0)
		परत false;
	अन्यथा
		परत ref.raw() < Derived::STATIC_मोजणी;
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
शून्य HashConsPool<Derived, गाठ_2, Ref>::RefIterator::settle() {
	पर्यंत (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
Ref HashConsPool<Derived, गाठ_2, Ref>::RefIterator::चालक*() स्थिरांक { परत Ref(idx); }

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाठ_2, Ref>::RefIterator::चालक++() -> RefIterator& { idx++; settle(); परत *this; }

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
bool HashConsPool<Derived, गाठ_2, Ref>::RefIterator::चालक!=(स्थिरांक RefIterator& other) स्थिरांक { परत idx != other.idx; }

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाठ_2, Ref>::RefRange::begin() स्थिरांक -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	परत it;
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाठ_2, Ref>::RefRange::end() स्थिरांक -> RefIterator { परत RefIterator{pool, stop, stop, skip_dead}; }

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाठ_2, Ref>::refs(bool include_statics) स्थिरांक -> RefRange {
	परत RefRange{this, include_statics ? 0 : Derived::STATIC_मोजणी,
			Derived::STATIC_मोजणी + backing.size(), true};
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
स्वतः HashConsPool<Derived, गाठ_2, Ref>::slots() स्थिरांक -> RefRange {
	परत RefRange{this, Derived::STATIC_मोजणी,
			Derived::STATIC_मोजणी + backing.size(), false};
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
माप_प्रकार HashConsPool<Derived, गाठ_2, Ref>::home_slot(uint64_t hash) स्थिरांक {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	परत (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
माप_प्रकार HashConsPool<Derived, गाठ_2, Ref>::next_slot(माप_प्रकार slot) स्थिरांक { परत (slot + 1) & (table.size() - 1); }

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
माप_प्रकार HashConsPool<Derived, गाठ_2, Ref>::size() स्थिरांक { परत backing.size() - free_list.size(); }

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
bool HashConsPool<Derived, गाठ_2, Ref>::slot_live(माप_प्रकार असल) स्थिरांक {
	जर (असल < Derived::STATIC_मोजणी)
		परत true;
	परत !backing[असल - Derived::STATIC_मोजणी].is_dead();
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
HashConsPool<Derived, गाठ_2, Ref>::HashConsPool() { rebuild_index(); }

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
HashConsPool<Derived, गाठ_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
HashConsPool<Derived, गाठ_2, Ref>& HashConsPool<Derived, गाठ_2, Ref>::चालक=(HashConsPool&& other) {
	जर (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	परत *this;
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
शून्य HashConsPool<Derived, गाठ_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
शून्य HashConsPool<Derived, गाठ_2, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	साठी (माप_प्रकार idx = 0; idx < backing.size(); ++idx)
		जर (backing[idx].is_dead())
			free_list.ढकला_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<माप_प्रकार>());
	table.assign(std::bit_ceil((Derived::STATIC_मोजणी + size()) * 2 + 2), Ref());
	साठी (Ref ref : refs())
		index_घाला(ref);
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
ढाँचा<typename Eq>
Ref HashConsPool<Derived, गाठ_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) स्थिरांक {
	साठी (माप_प्रकार slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		जर (Derived::hash_गाठ((*this)[ref]) == hash && eq(ref))
			परत ref;
	}
	परत Ref();
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
शून्य HashConsPool<Derived, गाठ_2, Ref>::index_घाला(Ref ref) {
	माप_प्रकार slot = home_slot(Derived::hash_गाठ((*this)[ref]));
	पर्यंत (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
शून्य HashConsPool<Derived, गाठ_2, Ref>::index_erase(Ref ref) {
	माप_प्रकार hole = home_slot(Derived::hash_गाठ((*this)[ref]));
	पर्यंत (table[hole] != ref)
		hole = next_slot(hole);
	माप_प्रकार mask = table.size() - 1;
	साठी (माप_प्रकार slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		माप_प्रकार home = home_slot(Derived::hash_गाठ((*this)[table[slot]]));
		जर (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
शून्य HashConsPool<Derived, गाठ_2, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	साठी (Ref ref : old)
		जर (ref != Ref())
			index_घाला(ref);
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
Ref HashConsPool<Derived, गाठ_2, Ref>::add_inner(गाठ_2 t) {
	Ref ref;
	जर (!free_list.empty()) {
		माप_प्रकार idx = free_list.back();
		free_list.काढा_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_मोजणी + idx);
	} अन्यथा {
		ref = Ref(Derived::STATIC_मोजणी + backing.size());
		backing.ढकला_back(std::move(t));
	}
	जर ((Derived::STATIC_मोजणी + size()) * 3 > table.size() * 2)
		grow_index();
	index_घाला(ref);
	जर (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	परत ref;
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
ढाँचा<typename Roots>
माप_प्रकार HashConsPool<Derived, गाठ_2, Ref>::gc(स्थिरांक Roots& roots) {
	pool<Ref> live;
	साठी (Ref ref : roots)
		mark_live(ref, live);
	माप_प्रकार erased = 0;
	साठी (माप_प्रकार idx = 0; idx < backing.size(); ++idx) {
		जर (backing[idx].is_dead())
			सुरूठेवा;
		जर (!live.मोजणी(Ref(Derived::STATIC_मोजणी + idx))) {
			index_erase(Ref(Derived::STATIC_मोजणी + idx));
			free_list.ढकला_back(idx);
			backing[idx] = गाठ_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<माप_प्रकार>());
	परत erased;
}

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
शून्य HashConsPool<Derived, गाठ_2, Ref>::mark_live(Ref ref, pool<Ref>& live) स्थिरांक {
	ref = ref.untag();
	जर (ref == Ref() || is_static(ref) || !live.घाला(ref).second)
		परत;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
