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

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
संरचना HashConsPool {
सार्वजनिक:
	bool is_live(Ref ref) ध्रुव;

रक्षित:
	std::deque<गाँठ_2> backing;
	std::vector<Ref> table;
	std::vector<माप_प्रकार> free_list;

	HashConsPool();
	HashConsPool(ध्रुव HashConsPool& other) = स्वभाव;
	HashConsPool(HashConsPool&& other);
	HashConsPool& चालक=(ध्रुव HashConsPool& other) = स्वभाव;
	HashConsPool& चालक=(HashConsPool&& other);

	स्थिर शून्य check_ready();

	ध्रुव गाँठ_2& चालक[] (Ref ref) ध्रुव;

	ढाँचा<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) ध्रुव;
	Ref add_inner(गाँठ_2 t);

	ढाँचा<typename Roots>
	माप_प्रकार gc(ध्रुव Roots& roots);

गुप्त:
	Derived* self();
	ध्रुव Derived* self() ध्रुव;

	शून्य reset();

	स्थिर bool is_static(Ref ref);

	संरचना RefIterator {
		ध्रुव HashConsPool* pool;
		माप_प्रकार idx;
		माप_प्रकार stop;
		bool skip_dead;

		शून्य settle();
		Ref चालक*() ध्रुव;
		RefIterator& चालक++();
		bool चालक!=(ध्रुव RefIterator& other) ध्रुव;
	};

	संरचना RefRange {
		ध्रुव HashConsPool* pool;
		माप_प्रकार first;
		माप_प्रकार stop;
		bool skip_dead;

		RefIterator begin() ध्रुव;
		RefIterator end() ध्रुव;
	};

	RefRange refs(bool include_statics = true) ध्रुव;
	// Includes the dead
	RefRange slots() ध्रुव;

	शून्य rebuild_index();
	माप_प्रकार home_slot(uint64_t hash) ध्रुव;
	माप_प्रकार next_slot(माप_प्रकार slot) ध्रुव;
	शून्य index_जोड़ो(Ref ref);
	शून्य index_erase(Ref ref);
	शून्य grow_index();

	माप_प्रकार size() ध्रुव;
	bool slot_live(माप_प्रकार असल) ध्रुव;
	शून्य mark_live(Ref ref, pool<Ref>& live) ध्रुव;
};

YOSYS_NAMESPACE_END

#endif
