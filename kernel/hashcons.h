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

template<typename Derived, typename गाठ_2, typename Ref>
संरचना HashConsPool {
सार्वजनिक:
	bool is_live(Ref ref) ध्रुव;

रक्षित:
	std::deque<गाठ_2> backing;
	std::vector<Ref> table;
	std::vector<size_t> free_list;

	HashConsPool();
	HashConsPool(ध्रुव HashConsPool& other) = स्वभाव;
	HashConsPool(HashConsPool&& other);
	HashConsPool& operator=(ध्रुव HashConsPool& other) = स्वभाव;
	HashConsPool& operator=(HashConsPool&& other);

	स्थिर शून्य check_ready();

	ध्रुव गाठ_2& operator[] (Ref ref) ध्रुव;

	template<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) ध्रुव;
	Ref add_inner(गाठ_2 t);

	template<typename Roots>
	size_t gc(ध्रुव Roots& roots);

गुप्त:
	Derived* self();
	ध्रुव Derived* self() ध्रुव;

	शून्य reset();

	स्थिर bool is_static(Ref ref);

	संरचना RefIterator {
		ध्रुव HashConsPool* pool;
		size_t idx;
		size_t stop;
		bool skip_dead;

		शून्य settle();
		Ref operator*() ध्रुव;
		RefIterator& operator++();
		bool operator!=(ध्रुव RefIterator& other) ध्रुव;
	};

	संरचना RefRange {
		ध्रुव HashConsPool* pool;
		size_t first;
		size_t stop;
		bool skip_dead;

		RefIterator begin() ध्रुव;
		RefIterator end() ध्रुव;
	};

	RefRange refs(bool include_statics = true) ध्रुव;
	// Includes the dead
	RefRange slots() ध्रुव;

	शून्य rebuild_index();
	size_t home_slot(uint64_t hash) ध्रुव;
	size_t next_slot(size_t slot) ध्रुव;
	शून्य index_घाला(Ref ref);
	शून्य index_erase(Ref ref);
	शून्य grow_index();

	size_t size() ध्रुव;
	bool slot_live(size_t abs) ध्रुव;
	शून्य mark_live(Ref ref, pool<Ref>& live) ध्रुव;
};

YOSYS_NAMESPACE_END

#endif
