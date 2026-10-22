#ifndef YOSYS_HASHCONS_H
#define YOSYS_HASHCONS_H

#अन्तर्भाव "kernel/yosys_common.h"

#अन्तर्भाव <bit>
#अन्तर्भाव <deque>
#अन्तर्भाव <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

ढाँचा<typename Derived, typename गाठ_2, typename Ref>
संरचना HashConsPool {
सार्वजनिक:
	bool is_live(Ref ref) स्थिरांक;

संरक्षित:
	std::deque<गाठ_2> backing;
	std::vector<Ref> table;
	std::vector<माप_प्रकार> free_list;

	HashConsPool();
	HashConsPool(स्थिरांक HashConsPool& other) = पूर्वनिर्धारित;
	HashConsPool(HashConsPool&& other);
	HashConsPool& चालक=(स्थिरांक HashConsPool& other) = पूर्वनिर्धारित;
	HashConsPool& चालक=(HashConsPool&& other);

	स्थिर शून्य check_ready();

	स्थिरांक गाठ_2& चालक[] (Ref ref) स्थिरांक;

	ढाँचा<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) स्थिरांक;
	Ref add_inner(गाठ_2 t);

	ढाँचा<typename Roots>
	माप_प्रकार gc(स्थिरांक Roots& roots);

खाजगी:
	Derived* self();
	स्थिरांक Derived* self() स्थिरांक;

	शून्य reset();

	स्थिर bool is_static(Ref ref);

	संरचना RefIterator {
		स्थिरांक HashConsPool* pool;
		माप_प्रकार idx;
		माप_प्रकार stop;
		bool skip_dead;

		शून्य settle();
		Ref चालक*() स्थिरांक;
		RefIterator& चालक++();
		bool चालक!=(स्थिरांक RefIterator& other) स्थिरांक;
	};

	संरचना RefRange {
		स्थिरांक HashConsPool* pool;
		माप_प्रकार first;
		माप_प्रकार stop;
		bool skip_dead;

		RefIterator begin() स्थिरांक;
		RefIterator end() स्थिरांक;
	};

	RefRange refs(bool include_statics = true) स्थिरांक;
	// Includes the dead
	RefRange slots() स्थिरांक;

	शून्य rebuild_index();
	माप_प्रकार home_slot(uint64_t hash) स्थिरांक;
	माप_प्रकार next_slot(माप_प्रकार slot) स्थिरांक;
	शून्य index_घाला(Ref ref);
	शून्य index_erase(Ref ref);
	शून्य grow_index();

	माप_प्रकार size() स्थिरांक;
	bool slot_live(माप_प्रकार असल) स्थिरांक;
	शून्य mark_live(Ref ref, pool<Ref>& live) स्थिरांक;
};

YOSYS_NAMESPACE_END

#endif
