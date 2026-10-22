#నిర్వచితం_కాకపోతే YOSYS_HASHCONS_IMPL_H
#నిర్వచించు YOSYS_HASHCONS_IMPL_H

#అన్తర్భావ "kernel/hashcons.h"

#అన్తర్భావ <algorithm>

YOSYS_NAMESPACE_BEGIN

మూస<typename Derived, typename కణుపు_2, typename Ref>
bool HashConsPool<Derived, కణుపు_2, Ref>::is_సజీవ(Ref ref) స్థిరరాశి {
	Ref idx = ref.untag();
	ఒకవేళ (is_static(idx))
		తిరిగి true;
	కొలత_t స్థానం = idx.raw() - Derived::STATIC_లెక్క;
	తిరిగి స్థానం < backing.కొలత() && !backing[స్థానం].is_dead();
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
శూన్య HashConsPool<Derived, కణుపు_2, Ref>::check_ready() {}

మూస<typename Derived, typename కణుపు_2, typename Ref>
స్థిరరాశి కణుపు_2& HashConsPool<Derived, కణుపు_2, Ref>::క్రియాకారకం[] (Ref ref) స్థిరరాశి {
	Ref idx = ref.untag();
	ఒకవేళ constexpr (Derived::STATIC_లెక్క != 0) {
		ఒకవేళ (is_static(idx))
			తిరిగి Derived::static_కణుపు(idx.raw());
	}
	తిరిగి backing[idx.raw() - Derived::STATIC_లెక్క];
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
Derived* HashConsPool<Derived, కణుపు_2, Ref>::self() { తిరిగి static_cast<Derived*>(this); }

మూస<typename Derived, typename కణుపు_2, typename Ref>
స్థిరరాశి Derived* HashConsPool<Derived, కణుపు_2, Ref>::self() స్థిరరాశి { తిరిగి static_cast<స్థిరరాశి Derived*>(this); }

మూస<typename Derived, typename కణుపు_2, typename Ref>
bool HashConsPool<Derived, కణుపు_2, Ref>::is_static(Ref ref) {
	ఒకవేళ constexpr (Derived::STATIC_లెక్క == 0)
		తిరిగి false;
	లేకపోతే
		తిరిగి ref.raw() < Derived::STATIC_లెక్క;
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
శూన్య HashConsPool<Derived, కణుపు_2, Ref>::RefIterator::settle() {
	అయితే (skip_dead && idx < stop && !pool->స్థానం_సజీవ(idx))
		idx++;
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
Ref HashConsPool<Derived, కణుపు_2, Ref>::RefIterator::క్రియాకారకం*() స్థిరరాశి { తిరిగి Ref(idx); }

మూస<typename Derived, typename కణుపు_2, typename Ref>
స్వయంచాలక HashConsPool<Derived, కణుపు_2, Ref>::RefIterator::క్రియాకారకం++() -> RefIterator& { idx++; settle(); తిరిగి *this; }

మూస<typename Derived, typename కణుపు_2, typename Ref>
bool HashConsPool<Derived, కణుపు_2, Ref>::RefIterator::క్రియాకారకం!=(స్థిరరాశి RefIterator& other) స్థిరరాశి { తిరిగి idx != other.idx; }

మూస<typename Derived, typename కణుపు_2, typename Ref>
స్వయంచాలక HashConsPool<Derived, కణుపు_2, Ref>::RefRange::begin() స్థిరరాశి -> RefIterator {
	RefIterator it{pool, మొదటి, stop, skip_dead};
	it.settle();
	తిరిగి it;
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
స్వయంచాలక HashConsPool<Derived, కణుపు_2, Ref>::RefRange::చివర() స్థిరరాశి -> RefIterator { తిరిగి RefIterator{pool, stop, stop, skip_dead}; }

మూస<typename Derived, typename కణుపు_2, typename Ref>
స్వయంచాలక HashConsPool<Derived, కణుపు_2, Ref>::refs(bool include_statics) స్థిరరాశి -> RefRange {
	తిరిగి RefRange{this, include_statics ? 0 : Derived::STATIC_లెక్క,
			Derived::STATIC_లెక్క + backing.కొలత(), true};
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
స్వయంచాలక HashConsPool<Derived, కణుపు_2, Ref>::slots() స్థిరరాశి -> RefRange {
	తిరిగి RefRange{this, Derived::STATIC_లెక్క,
			Derived::STATIC_లెక్క + backing.కొలత(), false};
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
కొలత_t HashConsPool<Derived, కణుపు_2, Ref>::home_స్థానం(uint64_t hash) స్థిరరాశి {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	తిరిగి (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(పట్టిక_2.కొలత()));
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
కొలత_t HashConsPool<Derived, కణుపు_2, Ref>::తదుపరి_స్థానం(కొలత_t స్థానం) స్థిరరాశి { తిరిగి (స్థానం + 1) & (పట్టిక_2.కొలత() - 1); }

మూస<typename Derived, typename కణుపు_2, typename Ref>
కొలత_t HashConsPool<Derived, కణుపు_2, Ref>::కొలత() స్థిరరాశి { తిరిగి backing.కొలత() - free_list.కొలత(); }

మూస<typename Derived, typename కణుపు_2, typename Ref>
bool HashConsPool<Derived, కణుపు_2, Ref>::స్థానం_సజీవ(కొలత_t నిరపేక్ష) స్థిరరాశి {
	ఒకవేళ (నిరపేక్ష < Derived::STATIC_లెక్క)
		తిరిగి true;
	తిరిగి !backing[నిరపేక్ష - Derived::STATIC_లెక్క].is_dead();
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
HashConsPool<Derived, కణుపు_2, Ref>::HashConsPool() { rebuild_సూచీ(); }

మూస<typename Derived, typename కణుపు_2, typename Ref>
HashConsPool<Derived, కణుపు_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), పట్టిక_2(std::move(other.పట్టిక_2)), free_list(std::move(other.free_list)) {
	other.reset();
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
HashConsPool<Derived, కణుపు_2, Ref>& HashConsPool<Derived, కణుపు_2, Ref>::క్రియాకారకం=(HashConsPool&& other) {
	ఒకవేళ (this != &other) {
		backing = std::move(other.backing);
		పట్టిక_2 = std::move(other.పట్టిక_2);
		free_list = std::move(other.free_list);
		other.reset();
	}
	తిరిగి *this;
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
శూన్య HashConsPool<Derived, కణుపు_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_సూచీ();
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
శూన్య HashConsPool<Derived, కణుపు_2, Ref>::rebuild_సూచీ() {
	Derived::check_ready();
	free_list.clear();
	కొరకు (కొలత_t idx = 0; idx < backing.కొలత(); ++idx)
		ఒకవేళ (backing[idx].is_dead())
			free_list.నెట్టు_వెనుక(idx);
	std::క్రమీకరించు(free_list.begin(), free_list.చివర(), std::greater<కొలత_t>());
	పట్టిక_2.కేటాయించు(std::బిట్టు_ceil((Derived::STATIC_లెక్క + కొలత()) * 2 + 2), Ref());
	కొరకు (Ref ref : refs())
		సూచీ_చొప్పించు(ref);
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
మూస<typename Eq>
Ref HashConsPool<Derived, కణుపు_2, Ref>::కనుగొను_hashed(uint64_t hash, Eq&& eq) స్థిరరాశి {
	కొరకు (కొలత_t స్థానం = home_స్థానం(hash); పట్టిక_2[స్థానం] != Ref(); స్థానం = తదుపరి_స్థానం(స్థానం)) {
		Ref ref = పట్టిక_2[స్థానం];
		ఒకవేళ (Derived::hash_కణుపు((*this)[ref]) == hash && eq(ref))
			తిరిగి ref;
	}
	తిరిగి Ref();
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
శూన్య HashConsPool<Derived, కణుపు_2, Ref>::సూచీ_చొప్పించు(Ref ref) {
	కొలత_t స్థానం = home_స్థానం(Derived::hash_కణుపు((*this)[ref]));
	అయితే (పట్టిక_2[స్థానం] != Ref())
		స్థానం = తదుపరి_స్థానం(స్థానం);
	పట్టిక_2[స్థానం] = ref;
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
శూన్య HashConsPool<Derived, కణుపు_2, Ref>::సూచీ_erase(Ref ref) {
	కొలత_t hole = home_స్థానం(Derived::hash_కణుపు((*this)[ref]));
	అయితే (పట్టిక_2[hole] != ref)
		hole = తదుపరి_స్థానం(hole);
	కొలత_t ముసుగు = పట్టిక_2.కొలత() - 1;
	కొరకు (కొలత_t స్థానం = తదుపరి_స్థానం(hole); పట్టిక_2[స్థానం] != Ref(); స్థానం = తదుపరి_స్థానం(స్థానం)) {
		కొలత_t home = home_స్థానం(Derived::hash_కణుపు((*this)[పట్టిక_2[స్థానం]]));
		ఒకవేళ (((స్థానం - home) & ముసుగు) >= ((స్థానం - hole) & ముసుగు)) {
			పట్టిక_2[hole] = పట్టిక_2[స్థానం];
			hole = స్థానం;
		}
	}
	పట్టిక_2[hole] = Ref();
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
శూన్య HashConsPool<Derived, కణుపు_2, Ref>::grow_సూచీ() {
	std::సదిశ<Ref> old(పట్టిక_2.కొలత() * 2, Ref());
	std::మార్పిడి(old, పట్టిక_2);
	కొరకు (Ref ref : old)
		ఒకవేళ (ref != Ref())
			సూచీ_చొప్పించు(ref);
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
Ref HashConsPool<Derived, కణుపు_2, Ref>::కూడు_inner(కణుపు_2 t) {
	Ref ref;
	ఒకవేళ (!free_list.ఖాళీలు()) {
		కొలత_t idx = free_list.వెనుక();
		free_list.తీయి_వెనుక();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_లెక్క + idx);
	} లేకపోతే {
		ref = Ref(Derived::STATIC_లెక్క + backing.కొలత());
		backing.నెట్టు_వెనుక(std::move(t));
	}
	ఒకవేళ ((Derived::STATIC_లెక్క + కొలత()) * 3 > పట్టిక_2.కొలత() * 2)
		grow_సూచీ();
	సూచీ_చొప్పించు(ref);
	ఒకవేళ (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	తిరిగి ref;
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
మూస<typename Roots>
కొలత_t HashConsPool<Derived, కణుపు_2, Ref>::gc(స్థిరరాశి Roots& roots) {
	pool<Ref> సజీవ;
	కొరకు (Ref ref : roots)
		mark_సజీవ(ref, సజీవ);
	కొలత_t erased = 0;
	కొరకు (కొలత_t idx = 0; idx < backing.కొలత(); ++idx) {
		ఒకవేళ (backing[idx].is_dead())
			కొనసాగు;
		ఒకవేళ (!సజీవ.లెక్క(Ref(Derived::STATIC_లెక్క + idx))) {
			సూచీ_erase(Ref(Derived::STATIC_లెక్క + idx));
			free_list.నెట్టు_వెనుక(idx);
			backing[idx] = కణుపు_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::క్రమీకరించు(free_list.begin(), free_list.చివర(), std::greater<కొలత_t>());
	తిరిగి erased;
}

మూస<typename Derived, typename కణుపు_2, typename Ref>
శూన్య HashConsPool<Derived, కణుపు_2, Ref>::mark_సజీవ(Ref ref, pool<Ref>& సజీవ) స్థిరరాశి {
	ref = ref.untag();
	ఒకవేళ (ref == Ref() || is_static(ref) || !సజీవ.చొప్పించు(ref).రెండవ)
		తిరిగి;
	Derived::for_each_బిడ్డ((*this)[ref], [&](Ref బిడ్డ) { mark_సజీవ(బిడ్డ, సజీవ); });
}

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
