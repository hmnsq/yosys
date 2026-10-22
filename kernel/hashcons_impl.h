#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ YOSYS_HASHCONS_IMPL_H
#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_HASHCONS_IMPL_H

#ಅನ್ತರ್ಭಾವ "kernel/hashcons.h"

#ಅನ್ತರ್ಭಾವ <algorithm>

YOSYS_NAMESPACE_BEGIN

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
bool HashConsPool<Derived, ಗಂಟು_2, Ref>::is_ಜೀವಂತ(Ref ref) ಸ್ಥಿರಾಂಕ {
	Ref idx = ref.untag();
	ಆದರೆ (is_static(idx))
		ಹಿಂತಿರುಗಿ true;
	ಗಾತ್ರ_t ಸ್ಥಾನ = idx.raw() - Derived::STATIC_ಎಣಿಕೆ;
	ಹಿಂತಿರುಗಿ ಸ್ಥಾನ < backing.ಗಾತ್ರ_2() && !backing[ಸ್ಥಾನ].is_dead();
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::check_ready() {}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಸ್ಥಿರಾಂಕ ಗಂಟು_2& HashConsPool<Derived, ಗಂಟು_2, Ref>::ಕ್ರಿಯಾಕಾರಕ[] (Ref ref) ಸ್ಥಿರಾಂಕ {
	Ref idx = ref.untag();
	ಆದರೆ constexpr (Derived::STATIC_ಎಣಿಕೆ != 0) {
		ಆದರೆ (is_static(idx))
			ಹಿಂತಿರುಗಿ Derived::static_ಗಂಟು(idx.raw());
	}
	ಹಿಂತಿರುಗಿ backing[idx.raw() - Derived::STATIC_ಎಣಿಕೆ];
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
Derived* HashConsPool<Derived, ಗಂಟು_2, Ref>::self() { ಹಿಂತಿರುಗಿ static_cast<Derived*>(this); }

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಸ್ಥಿರಾಂಕ Derived* HashConsPool<Derived, ಗಂಟು_2, Ref>::self() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ static_cast<ಸ್ಥಿರಾಂಕ Derived*>(this); }

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
bool HashConsPool<Derived, ಗಂಟು_2, Ref>::is_static(Ref ref) {
	ಆದರೆ constexpr (Derived::STATIC_ಎಣಿಕೆ == 0)
		ಹಿಂತಿರುಗಿ false;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ ref.raw() < Derived::STATIC_ಎಣಿಕೆ;
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::RefIterator::settle() {
	ಆಗ (skip_dead && idx < stop && !pool->ಸ್ಥಾನ_ಜೀವಂತ(idx))
		idx++;
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
Ref HashConsPool<Derived, ಗಂಟು_2, Ref>::RefIterator::ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ Ref(idx); }

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಸ್ವಯಂಚಾಲಿತ HashConsPool<Derived, ಗಂಟು_2, Ref>::RefIterator::ಕ್ರಿಯಾಕಾರಕ++() -> RefIterator& { idx++; settle(); ಹಿಂತಿರುಗಿ *this; }

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
bool HashConsPool<Derived, ಗಂಟು_2, Ref>::RefIterator::ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ RefIterator& other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ idx != other.idx; }

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಸ್ವಯಂಚಾಲಿತ HashConsPool<Derived, ಗಂಟು_2, Ref>::RefRange::begin() ಸ್ಥಿರಾಂಕ -> RefIterator {
	RefIterator it{pool, ಮೊದಲ, stop, skip_dead};
	it.settle();
	ಹಿಂತಿರುಗಿ it;
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಸ್ವಯಂಚಾಲಿತ HashConsPool<Derived, ಗಂಟು_2, Ref>::RefRange::ಕೊನೆ() ಸ್ಥಿರಾಂಕ -> RefIterator { ಹಿಂತಿರುಗಿ RefIterator{pool, stop, stop, skip_dead}; }

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಸ್ವಯಂಚಾಲಿತ HashConsPool<Derived, ಗಂಟು_2, Ref>::refs(bool include_statics) ಸ್ಥಿರಾಂಕ -> RefRange {
	ಹಿಂತಿರುಗಿ RefRange{this, include_statics ? 0 : Derived::STATIC_ಎಣಿಕೆ,
			Derived::STATIC_ಎಣಿಕೆ + backing.ಗಾತ್ರ_2(), true};
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಸ್ವಯಂಚಾಲಿತ HashConsPool<Derived, ಗಂಟು_2, Ref>::slots() ಸ್ಥಿರಾಂಕ -> RefRange {
	ಹಿಂತಿರುಗಿ RefRange{this, Derived::STATIC_ಎಣಿಕೆ,
			Derived::STATIC_ಎಣಿಕೆ + backing.ಗಾತ್ರ_2(), false};
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಗಾತ್ರ_t HashConsPool<Derived, ಗಂಟು_2, Ref>::home_ಸ್ಥಾನ(uint64_t hash) ಸ್ಥಿರಾಂಕ {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	ಹಿಂತಿರುಗಿ (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(ಕೋಷ್ಟಕ_2.ಗಾತ್ರ_2()));
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಗಾತ್ರ_t HashConsPool<Derived, ಗಂಟು_2, Ref>::ಮುಂದಿನ_ಸ್ಥಾನ(ಗಾತ್ರ_t ಸ್ಥಾನ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (ಸ್ಥಾನ + 1) & (ಕೋಷ್ಟಕ_2.ಗಾತ್ರ_2() - 1); }

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಗಾತ್ರ_t HashConsPool<Derived, ಗಂಟು_2, Ref>::ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ backing.ಗಾತ್ರ_2() - free_list.ಗಾತ್ರ_2(); }

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
bool HashConsPool<Derived, ಗಂಟು_2, Ref>::ಸ್ಥಾನ_ಜೀವಂತ(ಗಾತ್ರ_t ನಿರಪೇಕ್ಷ) ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (ನಿರಪೇಕ್ಷ < Derived::STATIC_ಎಣಿಕೆ)
		ಹಿಂತಿರುಗಿ true;
	ಹಿಂತಿರುಗಿ !backing[ನಿರಪೇಕ್ಷ - Derived::STATIC_ಎಣಿಕೆ].is_dead();
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
HashConsPool<Derived, ಗಂಟು_2, Ref>::HashConsPool() { rebuild_ಸೂಚ್ಯಂಕ(); }

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
HashConsPool<Derived, ಗಂಟು_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), ಕೋಷ್ಟಕ_2(std::move(other.ಕೋಷ್ಟಕ_2)), free_list(std::move(other.free_list)) {
	other.reset();
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
HashConsPool<Derived, ಗಂಟು_2, Ref>& HashConsPool<Derived, ಗಂಟು_2, Ref>::ಕ್ರಿಯಾಕಾರಕ=(HashConsPool&& other) {
	ಆದರೆ (this != &other) {
		backing = std::move(other.backing);
		ಕೋಷ್ಟಕ_2 = std::move(other.ಕೋಷ್ಟಕ_2);
		free_list = std::move(other.free_list);
		other.reset();
	}
	ಹಿಂತಿರುಗಿ *this;
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_ಸೂಚ್ಯಂಕ();
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::rebuild_ಸೂಚ್ಯಂಕ() {
	Derived::check_ready();
	free_list.clear();
	ಗಾಗಿ (ಗಾತ್ರ_t idx = 0; idx < backing.ಗಾತ್ರ_2(); ++idx)
		ಆದರೆ (backing[idx].is_dead())
			free_list.ತಳ್ಳು_ಹಿಂಭಾಗ(idx);
	std::ವಿಂಗಡಿಸು(free_list.begin(), free_list.ಕೊನೆ(), std::greater<ಗಾತ್ರ_t>());
	ಕೋಷ್ಟಕ_2.ನಿಯೋಜಿಸು(std::ಬಿಟ್_ceil((Derived::STATIC_ಎಣಿಕೆ + ಗಾತ್ರ_2()) * 2 + 2), Ref());
	ಗಾಗಿ (Ref ref : refs())
		ಸೂಚ್ಯಂಕ_ಸೇರಿಸು(ref);
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಮಾದರಿ<typename Eq>
Ref HashConsPool<Derived, ಗಂಟು_2, Ref>::ಕಂಡುಹಿಡಿ_hashed(uint64_t hash, Eq&& eq) ಸ್ಥಿರಾಂಕ {
	ಗಾಗಿ (ಗಾತ್ರ_t ಸ್ಥಾನ = home_ಸ್ಥಾನ(hash); ಕೋಷ್ಟಕ_2[ಸ್ಥಾನ] != Ref(); ಸ್ಥಾನ = ಮುಂದಿನ_ಸ್ಥಾನ(ಸ್ಥಾನ)) {
		Ref ref = ಕೋಷ್ಟಕ_2[ಸ್ಥಾನ];
		ಆದರೆ (Derived::hash_ಗಂಟು((*this)[ref]) == hash && eq(ref))
			ಹಿಂತಿರುಗಿ ref;
	}
	ಹಿಂತಿರುಗಿ Ref();
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::ಸೂಚ್ಯಂಕ_ಸೇರಿಸು(Ref ref) {
	ಗಾತ್ರ_t ಸ್ಥಾನ = home_ಸ್ಥಾನ(Derived::hash_ಗಂಟು((*this)[ref]));
	ಆಗ (ಕೋಷ್ಟಕ_2[ಸ್ಥಾನ] != Ref())
		ಸ್ಥಾನ = ಮುಂದಿನ_ಸ್ಥಾನ(ಸ್ಥಾನ);
	ಕೋಷ್ಟಕ_2[ಸ್ಥಾನ] = ref;
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::ಸೂಚ್ಯಂಕ_erase(Ref ref) {
	ಗಾತ್ರ_t hole = home_ಸ್ಥಾನ(Derived::hash_ಗಂಟು((*this)[ref]));
	ಆಗ (ಕೋಷ್ಟಕ_2[hole] != ref)
		hole = ಮುಂದಿನ_ಸ್ಥಾನ(hole);
	ಗಾತ್ರ_t ಮುಖವಾಡ = ಕೋಷ್ಟಕ_2.ಗಾತ್ರ_2() - 1;
	ಗಾಗಿ (ಗಾತ್ರ_t ಸ್ಥಾನ = ಮುಂದಿನ_ಸ್ಥಾನ(hole); ಕೋಷ್ಟಕ_2[ಸ್ಥಾನ] != Ref(); ಸ್ಥಾನ = ಮುಂದಿನ_ಸ್ಥಾನ(ಸ್ಥಾನ)) {
		ಗಾತ್ರ_t home = home_ಸ್ಥಾನ(Derived::hash_ಗಂಟು((*this)[ಕೋಷ್ಟಕ_2[ಸ್ಥಾನ]]));
		ಆದರೆ (((ಸ್ಥಾನ - home) & ಮುಖವಾಡ) >= ((ಸ್ಥಾನ - hole) & ಮುಖವಾಡ)) {
			ಕೋಷ್ಟಕ_2[hole] = ಕೋಷ್ಟಕ_2[ಸ್ಥಾನ];
			hole = ಸ್ಥಾನ;
		}
	}
	ಕೋಷ್ಟಕ_2[hole] = Ref();
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::grow_ಸೂಚ್ಯಂಕ() {
	std::ಸದಿಶ<Ref> old(ಕೋಷ್ಟಕ_2.ಗಾತ್ರ_2() * 2, Ref());
	std::ಅದಲುಬದಲು(old, ಕೋಷ್ಟಕ_2);
	ಗಾಗಿ (Ref ref : old)
		ಆದರೆ (ref != Ref())
			ಸೂಚ್ಯಂಕ_ಸೇರಿಸು(ref);
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
Ref HashConsPool<Derived, ಗಂಟು_2, Ref>::ಕೂಡಿಸು_inner(ಗಂಟು_2 t) {
	Ref ref;
	ಆದರೆ (!free_list.ಖಾಲಿಗಳು()) {
		ಗಾತ್ರ_t idx = free_list.ಹಿಂಭಾಗ();
		free_list.ತೆಗೆ_ಹಿಂಭಾಗ();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_ಎಣಿಕೆ + idx);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ref = Ref(Derived::STATIC_ಎಣಿಕೆ + backing.ಗಾತ್ರ_2());
		backing.ತಳ್ಳು_ಹಿಂಭಾಗ(std::move(t));
	}
	ಆದರೆ ((Derived::STATIC_ಎಣಿಕೆ + ಗಾತ್ರ_2()) * 3 > ಕೋಷ್ಟಕ_2.ಗಾತ್ರ_2() * 2)
		grow_ಸೂಚ್ಯಂಕ();
	ಸೂಚ್ಯಂಕ_ಸೇರಿಸು(ref);
	ಆದರೆ (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	ಹಿಂತಿರುಗಿ ref;
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಮಾದರಿ<typename Roots>
ಗಾತ್ರ_t HashConsPool<Derived, ಗಂಟು_2, Ref>::gc(ಸ್ಥಿರಾಂಕ Roots& roots) {
	pool<Ref> ಜೀವಂತ;
	ಗಾಗಿ (Ref ref : roots)
		mark_ಜೀವಂತ(ref, ಜೀವಂತ);
	ಗಾತ್ರ_t erased = 0;
	ಗಾಗಿ (ಗಾತ್ರ_t idx = 0; idx < backing.ಗಾತ್ರ_2(); ++idx) {
		ಆದರೆ (backing[idx].is_dead())
			ಮುಂದುವರಿ;
		ಆದರೆ (!ಜೀವಂತ.ಎಣಿಕೆ_2(Ref(Derived::STATIC_ಎಣಿಕೆ + idx))) {
			ಸೂಚ್ಯಂಕ_erase(Ref(Derived::STATIC_ಎಣಿಕೆ + idx));
			free_list.ತಳ್ಳು_ಹಿಂಭಾಗ(idx);
			backing[idx] = ಗಂಟು_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::ವಿಂಗಡಿಸು(free_list.begin(), free_list.ಕೊನೆ(), std::greater<ಗಾತ್ರ_t>());
	ಹಿಂತಿರುಗಿ erased;
}

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ಶೂನ್ಯ HashConsPool<Derived, ಗಂಟು_2, Ref>::mark_ಜೀವಂತ(Ref ref, pool<Ref>& ಜೀವಂತ) ಸ್ಥಿರಾಂಕ {
	ref = ref.untag();
	ಆದರೆ (ref == Ref() || is_static(ref) || !ಜೀವಂತ.ಸೇರಿಸು(ref).ಎರಡನೇ)
		ಹಿಂತಿರುಗಿ;
	Derived::for_each_ಮಗು((*this)[ref], [&](Ref ಮಗು) { mark_ಜೀವಂತ(ಮಗು, ಜೀವಂತ); });
}

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
