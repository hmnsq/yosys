#यदि_अपरिभाषितम् YOSYS_HASHCONS_IMPL_H
#परिभाषय YOSYS_HASHCONS_IMPL_H

#अन्तर्भाव "kernel/hashcons.h"

#अन्तर्भाव <algorithm>

YOSYS_NAMESPACE_BEGIN

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
bool HashConsPool<Derived, ग्रन्थिः_2, Ref>::is_जीवितम्(Ref ref) ध्रुव {
	Ref idx = ref.untag();
	यदि (is_static(idx))
		प्रत्यागम true;
	आकार_t slot = idx.raw() - Derived::STATIC_गणनम्;
	प्रत्यागम slot < backing.आकार() && !backing[slot].is_dead();
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::check_ready() {}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
ध्रुव ग्रन्थिः_2& HashConsPool<Derived, ग्रन्थिः_2, Ref>::संक्रियकः[] (Ref ref) ध्रुव {
	Ref idx = ref.untag();
	यदि constexpr (Derived::STATIC_गणनम् != 0) {
		यदि (is_static(idx))
			प्रत्यागम Derived::static_ग्रन्थिः(idx.raw());
	}
	प्रत्यागम backing[idx.raw() - Derived::STATIC_गणनम्];
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
Derived* HashConsPool<Derived, ग्रन्थिः_2, Ref>::self() { प्रत्यागम static_cast<Derived*>(this); }

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
ध्रुव Derived* HashConsPool<Derived, ग्रन्थिः_2, Ref>::self() ध्रुव { प्रत्यागम static_cast<ध्रुव Derived*>(this); }

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
bool HashConsPool<Derived, ग्रन्थिः_2, Ref>::is_static(Ref ref) {
	यदि constexpr (Derived::STATIC_गणनम् == 0)
		प्रत्यागम false;
	अन्यथा
		प्रत्यागम ref.raw() < Derived::STATIC_गणनम्;
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::RefIterator::settle() {
	यावत् (skip_dead && idx < stop && !pool->slot_जीवितम्(idx))
		idx++;
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
Ref HashConsPool<Derived, ग्रन्थिः_2, Ref>::RefIterator::संक्रियकः*() ध्रुव { प्रत्यागम Ref(idx); }

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
स्वचालित HashConsPool<Derived, ग्रन्थिः_2, Ref>::RefIterator::संक्रियकः++() -> RefIterator& { idx++; settle(); प्रत्यागम *this; }

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
bool HashConsPool<Derived, ग्रन्थिः_2, Ref>::RefIterator::संक्रियकः!=(ध्रुव RefIterator& other) ध्रुव { प्रत्यागम idx != other.idx; }

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
स्वचालित HashConsPool<Derived, ग्रन्थिः_2, Ref>::RefRange::begin() ध्रुव -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	प्रत्यागम it;
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
स्वचालित HashConsPool<Derived, ग्रन्थिः_2, Ref>::RefRange::अन्तः() ध्रुव -> RefIterator { प्रत्यागम RefIterator{pool, stop, stop, skip_dead}; }

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
स्वचालित HashConsPool<Derived, ग्रन्थिः_2, Ref>::refs(bool include_statics) ध्रुव -> RefRange {
	प्रत्यागम RefRange{this, include_statics ? 0 : Derived::STATIC_गणनम्,
			Derived::STATIC_गणनम् + backing.आकार(), true};
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
स्वचालित HashConsPool<Derived, ग्रन्थिः_2, Ref>::slots() ध्रुव -> RefRange {
	प्रत्यागम RefRange{this, Derived::STATIC_गणनम्,
			Derived::STATIC_गणनम् + backing.आकार(), false};
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
आकार_t HashConsPool<Derived, ग्रन्थिः_2, Ref>::home_slot(uint64_t hash) ध्रुव {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	प्रत्यागम (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(सारणी_2.आकार()));
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
आकार_t HashConsPool<Derived, ग्रन्थिः_2, Ref>::अग्र_slot(आकार_t slot) ध्रुव { प्रत्यागम (slot + 1) & (सारणी_2.आकार() - 1); }

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
आकार_t HashConsPool<Derived, ग्रन्थिः_2, Ref>::आकार() ध्रुव { प्रत्यागम backing.आकार() - free_list.आकार(); }

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
bool HashConsPool<Derived, ग्रन्थिः_2, Ref>::slot_जीवितम्(आकार_t निरपेक्षम्) ध्रुव {
	यदि (निरपेक्षम् < Derived::STATIC_गणनम्)
		प्रत्यागम true;
	प्रत्यागम !backing[निरपेक्षम् - Derived::STATIC_गणनम्].is_dead();
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
HashConsPool<Derived, ग्रन्थिः_2, Ref>::HashConsPool() { rebuild_सूचकांक(); }

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
HashConsPool<Derived, ग्रन्थिः_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), सारणी_2(std::move(other.सारणी_2)), free_list(std::move(other.free_list)) {
	other.reset();
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
HashConsPool<Derived, ग्रन्थिः_2, Ref>& HashConsPool<Derived, ग्रन्थिः_2, Ref>::संक्रियकः=(HashConsPool&& other) {
	यदि (this != &other) {
		backing = std::move(other.backing);
		सारणी_2 = std::move(other.सारणी_2);
		free_list = std::move(other.free_list);
		other.reset();
	}
	प्रत्यागम *this;
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_सूचकांक();
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::rebuild_सूचकांक() {
	Derived::check_ready();
	free_list.clear();
	प्रति (आकार_t idx = 0; idx < backing.आकार(); ++idx)
		यदि (backing[idx].is_dead())
			free_list.प्रक्षिप_पृष्ठभागः(idx);
	std::क्रमय(free_list.begin(), free_list.अन्तः(), std::greater<आकार_t>());
	सारणी_2.नियोजय(std::bit_ceil((Derived::STATIC_गणनम् + आकार()) * 2 + 2), Ref());
	प्रति (Ref ref : refs())
		सूचकांक_निवेशय(ref);
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
प्रतिरूपम्<typename Eq>
Ref HashConsPool<Derived, ग्रन्थिः_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) ध्रुव {
	प्रति (आकार_t slot = home_slot(hash); सारणी_2[slot] != Ref(); slot = अग्र_slot(slot)) {
		Ref ref = सारणी_2[slot];
		यदि (Derived::hash_ग्रन्थिः((*this)[ref]) == hash && eq(ref))
			प्रत्यागम ref;
	}
	प्रत्यागम Ref();
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::सूचकांक_निवेशय(Ref ref) {
	आकार_t slot = home_slot(Derived::hash_ग्रन्थिः((*this)[ref]));
	यावत् (सारणी_2[slot] != Ref())
		slot = अग्र_slot(slot);
	सारणी_2[slot] = ref;
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::सूचकांक_erase(Ref ref) {
	आकार_t hole = home_slot(Derived::hash_ग्रन्थिः((*this)[ref]));
	यावत् (सारणी_2[hole] != ref)
		hole = अग्र_slot(hole);
	आकार_t आवरणम् = सारणी_2.आकार() - 1;
	प्रति (आकार_t slot = अग्र_slot(hole); सारणी_2[slot] != Ref(); slot = अग्र_slot(slot)) {
		आकार_t home = home_slot(Derived::hash_ग्रन्थिः((*this)[सारणी_2[slot]]));
		यदि (((slot - home) & आवरणम्) >= ((slot - hole) & आवरणम्)) {
			सारणी_2[hole] = सारणी_2[slot];
			hole = slot;
		}
	}
	सारणी_2[hole] = Ref();
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::grow_सूचकांक() {
	std::सदिशम्<Ref> old(सारणी_2.आकार() * 2, Ref());
	std::swap(old, सारणी_2);
	प्रति (Ref ref : old)
		यदि (ref != Ref())
			सूचकांक_निवेशय(ref);
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
Ref HashConsPool<Derived, ग्रन्थिः_2, Ref>::सङ्कलय_inner(ग्रन्थिः_2 t) {
	Ref ref;
	यदि (!free_list.रिक्तानि()) {
		आकार_t idx = free_list.पृष्ठभागः();
		free_list.निष्कासय_पृष्ठभागः();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_गणनम् + idx);
	} अन्यथा {
		ref = Ref(Derived::STATIC_गणनम् + backing.आकार());
		backing.प्रक्षिप_पृष्ठभागः(std::move(t));
	}
	यदि ((Derived::STATIC_गणनम् + आकार()) * 3 > सारणी_2.आकार() * 2)
		grow_सूचकांक();
	सूचकांक_निवेशय(ref);
	यदि (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	प्रत्यागम ref;
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
प्रतिरूपम्<typename Roots>
आकार_t HashConsPool<Derived, ग्रन्थिः_2, Ref>::gc(ध्रुव Roots& roots) {
	pool<Ref> जीवितम्;
	प्रति (Ref ref : roots)
		mark_जीवितम्(ref, जीवितम्);
	आकार_t erased = 0;
	प्रति (आकार_t idx = 0; idx < backing.आकार(); ++idx) {
		यदि (backing[idx].is_dead())
			अनुवर्त;
		यदि (!जीवितम्.गणनम्(Ref(Derived::STATIC_गणनम् + idx))) {
			सूचकांक_erase(Ref(Derived::STATIC_गणनम् + idx));
			free_list.प्रक्षिप_पृष्ठभागः(idx);
			backing[idx] = ग्रन्थिः_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::क्रमय(free_list.begin(), free_list.अन्तः(), std::greater<आकार_t>());
	प्रत्यागम erased;
}

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
शून्य HashConsPool<Derived, ग्रन्थिः_2, Ref>::mark_जीवितम्(Ref ref, pool<Ref>& जीवितम्) ध्रुव {
	ref = ref.untag();
	यदि (ref == Ref() || is_static(ref) || !जीवितम्.निवेशय(ref).second)
		प्रत्यागम;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_जीवितम्(child, जीवितम्); });
}

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
