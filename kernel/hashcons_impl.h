#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ YOSYS_HASHCONS_IMPL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_HASHCONS_IMPL_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/hashcons.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <algorithm>

YOSYS_NAMESPACE_BEGIN

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
bool HashConsPool<Derived, ਗੰਢ_2, Ref>::is_ਜੀਵਤ(Ref ref) ਅਚਲ {
	Ref idx = ref.untag();
	ਜੇ (is_static(idx))
		ਵਾਪਸ true;
	ਮਾਪ_t ਥਾਂ = idx.raw() - Derived::STATIC_ਗਿਣਤੀ;
	ਵਾਪਸ ਥਾਂ < backing.ਮਾਪ() && !backing[ਥਾਂ].is_dead();
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਖਾਲੀ HashConsPool<Derived, ਗੰਢ_2, Ref>::check_ready() {}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਅਚਲ ਗੰਢ_2& HashConsPool<Derived, ਗੰਢ_2, Ref>::ਸੰਚਾਲਕ[] (Ref ref) ਅਚਲ {
	Ref idx = ref.untag();
	ਜੇ constexpr (Derived::STATIC_ਗਿਣਤੀ != 0) {
		ਜੇ (is_static(idx))
			ਵਾਪਸ Derived::static_ਗੰਢ(idx.raw());
	}
	ਵਾਪਸ backing[idx.raw() - Derived::STATIC_ਗਿਣਤੀ];
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
Derived* HashConsPool<Derived, ਗੰਢ_2, Ref>::self() { ਵਾਪਸ static_cast<Derived*>(this); }

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਅਚਲ Derived* HashConsPool<Derived, ਗੰਢ_2, Ref>::self() ਅਚਲ { ਵਾਪਸ static_cast<ਅਚਲ Derived*>(this); }

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
bool HashConsPool<Derived, ਗੰਢ_2, Ref>::is_static(Ref ref) {
	ਜੇ constexpr (Derived::STATIC_ਗਿਣਤੀ == 0)
		ਵਾਪਸ false;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ ref.raw() < Derived::STATIC_ਗਿਣਤੀ;
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਖਾਲੀ HashConsPool<Derived, ਗੰਢ_2, Ref>::RefIterator::settle() {
	ਜਦੋਂ (skip_dead && idx < stop && !pool->ਥਾਂ_ਜੀਵਤ(idx))
		idx++;
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
Ref HashConsPool<Derived, ਗੰਢ_2, Ref>::RefIterator::ਸੰਚਾਲਕ*() ਅਚਲ { ਵਾਪਸ Ref(idx); }

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਸਵੈਚਾਲਿਤ HashConsPool<Derived, ਗੰਢ_2, Ref>::RefIterator::ਸੰਚਾਲਕ++() -> RefIterator& { idx++; settle(); ਵਾਪਸ *this; }

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
bool HashConsPool<Derived, ਗੰਢ_2, Ref>::RefIterator::ਸੰਚਾਲਕ!=(ਅਚਲ RefIterator& other) ਅਚਲ { ਵਾਪਸ idx != other.idx; }

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਸਵੈਚਾਲਿਤ HashConsPool<Derived, ਗੰਢ_2, Ref>::RefRange::begin() ਅਚਲ -> RefIterator {
	RefIterator it{pool, ਪਹਿਲਾ, stop, skip_dead};
	it.settle();
	ਵਾਪਸ it;
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਸਵੈਚਾਲਿਤ HashConsPool<Derived, ਗੰਢ_2, Ref>::RefRange::ਸਿਰਾ() ਅਚਲ -> RefIterator { ਵਾਪਸ RefIterator{pool, stop, stop, skip_dead}; }

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਸਵੈਚਾਲਿਤ HashConsPool<Derived, ਗੰਢ_2, Ref>::refs(bool include_statics) ਅਚਲ -> RefRange {
	ਵਾਪਸ RefRange{this, include_statics ? 0 : Derived::STATIC_ਗਿਣਤੀ,
			Derived::STATIC_ਗਿਣਤੀ + backing.ਮਾਪ(), true};
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਸਵੈਚਾਲਿਤ HashConsPool<Derived, ਗੰਢ_2, Ref>::slots() ਅਚਲ -> RefRange {
	ਵਾਪਸ RefRange{this, Derived::STATIC_ਗਿਣਤੀ,
			Derived::STATIC_ਗਿਣਤੀ + backing.ਮਾਪ(), false};
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਮਾਪ_t HashConsPool<Derived, ਗੰਢ_2, Ref>::home_ਥਾਂ(uint64_t hash) ਅਚਲ {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	ਵਾਪਸ (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(ਸਾਰਣੀ_2.ਮਾਪ()));
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਮਾਪ_t HashConsPool<Derived, ਗੰਢ_2, Ref>::ਅਗਲਾ_ਥਾਂ(ਮਾਪ_t ਥਾਂ) ਅਚਲ { ਵਾਪਸ (ਥਾਂ + 1) & (ਸਾਰਣੀ_2.ਮਾਪ() - 1); }

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਮਾਪ_t HashConsPool<Derived, ਗੰਢ_2, Ref>::ਮਾਪ() ਅਚਲ { ਵਾਪਸ backing.ਮਾਪ() - free_list.ਮਾਪ(); }

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
bool HashConsPool<Derived, ਗੰਢ_2, Ref>::ਥਾਂ_ਜੀਵਤ(ਮਾਪ_t ਨਿਰਪੇਖ) ਅਚਲ {
	ਜੇ (ਨਿਰਪੇਖ < Derived::STATIC_ਗਿਣਤੀ)
		ਵਾਪਸ true;
	ਵਾਪਸ !backing[ਨਿਰਪੇਖ - Derived::STATIC_ਗਿਣਤੀ].is_dead();
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
HashConsPool<Derived, ਗੰਢ_2, Ref>::HashConsPool() { rebuild_ਸੂਚਕਾਂਕ(); }

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
HashConsPool<Derived, ਗੰਢ_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), ਸਾਰਣੀ_2(std::move(other.ਸਾਰਣੀ_2)), free_list(std::move(other.free_list)) {
	other.reset();
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
HashConsPool<Derived, ਗੰਢ_2, Ref>& HashConsPool<Derived, ਗੰਢ_2, Ref>::ਸੰਚਾਲਕ=(HashConsPool&& other) {
	ਜੇ (this != &other) {
		backing = std::move(other.backing);
		ਸਾਰਣੀ_2 = std::move(other.ਸਾਰਣੀ_2);
		free_list = std::move(other.free_list);
		other.reset();
	}
	ਵਾਪਸ *this;
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਖਾਲੀ HashConsPool<Derived, ਗੰਢ_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_ਸੂਚਕਾਂਕ();
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਖਾਲੀ HashConsPool<Derived, ਗੰਢ_2, Ref>::rebuild_ਸੂਚਕਾਂਕ() {
	Derived::check_ready();
	free_list.clear();
	ਲਈ (ਮਾਪ_t idx = 0; idx < backing.ਮਾਪ(); ++idx)
		ਜੇ (backing[idx].is_dead())
			free_list.ਧੱਕੋ_ਪਿੱਠ(idx);
	std::ਛਾਂਟੋ(free_list.begin(), free_list.ਸਿਰਾ(), std::greater<ਮਾਪ_t>());
	ਸਾਰਣੀ_2.ਸੌਂਪੋ(std::ਦੋਆਧਾਰੀ_ਅੰਕ_ceil((Derived::STATIC_ਗਿਣਤੀ + ਮਾਪ()) * 2 + 2), Ref());
	ਲਈ (Ref ref : refs())
		ਸੂਚਕਾਂਕ_ਪਾਓ(ref);
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਸਾਂਚਾ<typename Eq>
Ref HashConsPool<Derived, ਗੰਢ_2, Ref>::ਲੱਭੋ_hashed(uint64_t hash, Eq&& eq) ਅਚਲ {
	ਲਈ (ਮਾਪ_t ਥਾਂ = home_ਥਾਂ(hash); ਸਾਰਣੀ_2[ਥਾਂ] != Ref(); ਥਾਂ = ਅਗਲਾ_ਥਾਂ(ਥਾਂ)) {
		Ref ref = ਸਾਰਣੀ_2[ਥਾਂ];
		ਜੇ (Derived::hash_ਗੰਢ((*this)[ref]) == hash && eq(ref))
			ਵਾਪਸ ref;
	}
	ਵਾਪਸ Ref();
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਖਾਲੀ HashConsPool<Derived, ਗੰਢ_2, Ref>::ਸੂਚਕਾਂਕ_ਪਾਓ(Ref ref) {
	ਮਾਪ_t ਥਾਂ = home_ਥਾਂ(Derived::hash_ਗੰਢ((*this)[ref]));
	ਜਦੋਂ (ਸਾਰਣੀ_2[ਥਾਂ] != Ref())
		ਥਾਂ = ਅਗਲਾ_ਥਾਂ(ਥਾਂ);
	ਸਾਰਣੀ_2[ਥਾਂ] = ref;
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਖਾਲੀ HashConsPool<Derived, ਗੰਢ_2, Ref>::ਸੂਚਕਾਂਕ_erase(Ref ref) {
	ਮਾਪ_t hole = home_ਥਾਂ(Derived::hash_ਗੰਢ((*this)[ref]));
	ਜਦੋਂ (ਸਾਰਣੀ_2[hole] != ref)
		hole = ਅਗਲਾ_ਥਾਂ(hole);
	ਮਾਪ_t ਨਕਾਬ = ਸਾਰਣੀ_2.ਮਾਪ() - 1;
	ਲਈ (ਮਾਪ_t ਥਾਂ = ਅਗਲਾ_ਥਾਂ(hole); ਸਾਰਣੀ_2[ਥਾਂ] != Ref(); ਥਾਂ = ਅਗਲਾ_ਥਾਂ(ਥਾਂ)) {
		ਮਾਪ_t home = home_ਥਾਂ(Derived::hash_ਗੰਢ((*this)[ਸਾਰਣੀ_2[ਥਾਂ]]));
		ਜੇ (((ਥਾਂ - home) & ਨਕਾਬ) >= ((ਥਾਂ - hole) & ਨਕਾਬ)) {
			ਸਾਰਣੀ_2[hole] = ਸਾਰਣੀ_2[ਥਾਂ];
			hole = ਥਾਂ;
		}
	}
	ਸਾਰਣੀ_2[hole] = Ref();
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਖਾਲੀ HashConsPool<Derived, ਗੰਢ_2, Ref>::grow_ਸੂਚਕਾਂਕ() {
	std::ਸਦਿਸ਼<Ref> old(ਸਾਰਣੀ_2.ਮਾਪ() * 2, Ref());
	std::ਅਦਲਾ_ਬਦਲੀ(old, ਸਾਰਣੀ_2);
	ਲਈ (Ref ref : old)
		ਜੇ (ref != Ref())
			ਸੂਚਕਾਂਕ_ਪਾਓ(ref);
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
Ref HashConsPool<Derived, ਗੰਢ_2, Ref>::ਜੋੜੋ_inner(ਗੰਢ_2 t) {
	Ref ref;
	ਜੇ (!free_list.ਖਾਲੀ_ਥਾਵਾਂ()) {
		ਮਾਪ_t idx = free_list.ਪਿੱਠ();
		free_list.ਕੱਢੋ_ਪਿੱਠ();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_ਗਿਣਤੀ + idx);
	} ਨਹੀਂਤਾਂ {
		ref = Ref(Derived::STATIC_ਗਿਣਤੀ + backing.ਮਾਪ());
		backing.ਧੱਕੋ_ਪਿੱਠ(std::move(t));
	}
	ਜੇ ((Derived::STATIC_ਗਿਣਤੀ + ਮਾਪ()) * 3 > ਸਾਰਣੀ_2.ਮਾਪ() * 2)
		grow_ਸੂਚਕਾਂਕ();
	ਸੂਚਕਾਂਕ_ਪਾਓ(ref);
	ਜੇ (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	ਵਾਪਸ ref;
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਸਾਂਚਾ<typename Roots>
ਮਾਪ_t HashConsPool<Derived, ਗੰਢ_2, Ref>::gc(ਅਚਲ Roots& roots) {
	pool<Ref> ਜੀਵਤ;
	ਲਈ (Ref ref : roots)
		mark_ਜੀਵਤ(ref, ਜੀਵਤ);
	ਮਾਪ_t erased = 0;
	ਲਈ (ਮਾਪ_t idx = 0; idx < backing.ਮਾਪ(); ++idx) {
		ਜੇ (backing[idx].is_dead())
			ਜਾਰੀ;
		ਜੇ (!ਜੀਵਤ.ਗਿਣਤੀ_2(Ref(Derived::STATIC_ਗਿਣਤੀ + idx))) {
			ਸੂਚਕਾਂਕ_erase(Ref(Derived::STATIC_ਗਿਣਤੀ + idx));
			free_list.ਧੱਕੋ_ਪਿੱਠ(idx);
			backing[idx] = ਗੰਢ_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::ਛਾਂਟੋ(free_list.begin(), free_list.ਸਿਰਾ(), std::greater<ਮਾਪ_t>());
	ਵਾਪਸ erased;
}

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਖਾਲੀ HashConsPool<Derived, ਗੰਢ_2, Ref>::mark_ਜੀਵਤ(Ref ref, pool<Ref>& ਜੀਵਤ) ਅਚਲ {
	ref = ref.untag();
	ਜੇ (ref == Ref() || is_static(ref) || !ਜੀਵਤ.ਪਾਓ(ref).ਦੂਜਾ)
		ਵਾਪਸ;
	Derived::for_each_ਬੱਚਾ((*this)[ref], [&](Ref ਬੱਚਾ) { mark_ਜੀਵਤ(ਬੱਚਾ, ਜੀਵਤ); });
}

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
