#నిర్వచితం_కాకపోతే YOSYS_HASHCONS_H
#నిర్వచించు YOSYS_HASHCONS_H

#అన్తర్భావ "kernel/yosys_common.h"

#అన్తర్భావ <bit>
#అన్తర్భావ <deque>
#అన్తర్భావ <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

మూస<typename Derived, typename కణుపు_2, typename Ref>
నిర్మాణం HashConsPool {
బహిరంగ:
	bool is_సజీవ(Ref ref) స్థిరరాశి;

రక్షిత:
	std::deque<కణుపు_2> backing;
	std::సదిశ<Ref> పట్టిక_2;
	std::సదిశ<కొలత_t> free_list;

	HashConsPool();
	HashConsPool(స్థిరరాశి HashConsPool& other) = అప్రమేయ;
	HashConsPool(HashConsPool&& other);
	HashConsPool& క్రియాకారకం=(స్థిరరాశి HashConsPool& other) = అప్రమేయ;
	HashConsPool& క్రియాకారకం=(HashConsPool&& other);

	స్థిర శూన్య check_ready();

	స్థిరరాశి కణుపు_2& క్రియాకారకం[] (Ref ref) స్థిరరాశి;

	మూస<typename Eq>
	Ref కనుగొను_hashed(uint64_t hash, Eq&& eq) స్థిరరాశి;
	Ref కూడు_inner(కణుపు_2 t);

	మూస<typename Roots>
	కొలత_t gc(స్థిరరాశి Roots& roots);

అంతరంగ:
	Derived* self();
	స్థిరరాశి Derived* self() స్థిరరాశి;

	శూన్య reset();

	స్థిర bool is_static(Ref ref);

	నిర్మాణం RefIterator {
		స్థిరరాశి HashConsPool* pool;
		కొలత_t idx;
		కొలత_t stop;
		bool skip_dead;

		శూన్య settle();
		Ref క్రియాకారకం*() స్థిరరాశి;
		RefIterator& క్రియాకారకం++();
		bool క్రియాకారకం!=(స్థిరరాశి RefIterator& other) స్థిరరాశి;
	};

	నిర్మాణం RefRange {
		స్థిరరాశి HashConsPool* pool;
		కొలత_t మొదటి;
		కొలత_t stop;
		bool skip_dead;

		RefIterator begin() స్థిరరాశి;
		RefIterator చివర() స్థిరరాశి;
	};

	RefRange refs(bool include_statics = true) స్థిరరాశి;
	// Includes the dead
	RefRange slots() స్థిరరాశి;

	శూన్య rebuild_సూచీ();
	కొలత_t home_స్థానం(uint64_t hash) స్థిరరాశి;
	కొలత_t తదుపరి_స్థానం(కొలత_t స్థానం) స్థిరరాశి;
	శూన్య సూచీ_చొప్పించు(Ref ref);
	శూన్య సూచీ_erase(Ref ref);
	శూన్య grow_సూచీ();

	కొలత_t కొలత() స్థిరరాశి;
	bool స్థానం_సజీవ(కొలత_t నిరపేక్ష) స్థిరరాశి;
	శూన్య mark_సజీవ(Ref ref, pool<Ref>& సజీవ) స్థిరరాశి;
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
