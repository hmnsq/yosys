#ifndef YOSYS_HASHCONS_H
#define YOSYS_HASHCONS_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"

#ಅನ್ತರ್ಭಾವ <bit>
#ಅನ್ತರ್ಭಾವ <deque>
#ಅನ್ತರ್ಭಾವ <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename ಗಂಟು_2, typename Ref>
ಸಂರಚನಾ HashConsPool {
ಸಾರ್ವಜನಿಕ:
	bool is_live(Ref ref) ಧ್ರುವ;

ರಕ್ಷಿತ:
	std::deque<ಗಂಟು_2> backing;
	std::vector<Ref> table;
	std::vector<size_t> free_list;

	HashConsPool();
	HashConsPool(ಧ್ರುವ HashConsPool& other) = ಸ್ವಭಾವ;
	HashConsPool(HashConsPool&& other);
	HashConsPool& operator=(ಧ್ರುವ HashConsPool& other) = ಸ್ವಭಾವ;
	HashConsPool& operator=(HashConsPool&& other);

	ಸ್ಥಿರ ಶೂನ್ಯ check_ready();

	ಧ್ರುವ ಗಂಟು_2& operator[] (Ref ref) ಧ್ರುವ;

	template<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) ಧ್ರುವ;
	Ref add_inner(ಗಂಟು_2 t);

	template<typename Roots>
	size_t gc(ಧ್ರುವ Roots& roots);

ಗುಪ್ತ:
	Derived* self();
	ಧ್ರುವ Derived* self() ಧ್ರುವ;

	ಶೂನ್ಯ reset();

	ಸ್ಥಿರ bool is_static(Ref ref);

	ಸಂರಚನಾ RefIterator {
		ಧ್ರುವ HashConsPool* pool;
		size_t idx;
		size_t stop;
		bool skip_dead;

		ಶೂನ್ಯ settle();
		Ref operator*() ಧ್ರುವ;
		RefIterator& operator++();
		bool operator!=(ಧ್ರುವ RefIterator& other) ಧ್ರುವ;
	};

	ಸಂರಚನಾ RefRange {
		ಧ್ರುವ HashConsPool* pool;
		size_t first;
		size_t stop;
		bool skip_dead;

		RefIterator begin() ಧ್ರುವ;
		RefIterator end() ಧ್ರುವ;
	};

	RefRange refs(bool include_statics = true) ಧ್ರುವ;
	// Includes the dead
	RefRange slots() ಧ್ರುವ;

	ಶೂನ್ಯ rebuild_index();
	size_t home_slot(uint64_t hash) ಧ್ರುವ;
	size_t next_slot(size_t slot) ಧ್ರುವ;
	ಶೂನ್ಯ index_ಸೇರಿಸು(Ref ref);
	ಶೂನ್ಯ index_erase(Ref ref);
	ಶೂನ್ಯ grow_index();

	size_t size() ಧ್ರುವ;
	bool slot_live(size_t abs) ಧ್ರುವ;
	ಶೂನ್ಯ mark_live(Ref ref, pool<Ref>& live) ಧ್ರುವ;
};

YOSYS_NAMESPACE_END

#endif
