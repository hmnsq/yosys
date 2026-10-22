#ifndef YOSYS_HASHCONS_H
#define YOSYS_HASHCONS_H

#અન્તર્ભાવ "kernel/yosys_common.h"

#અન્તર્ભાવ <bit>
#અન્તર્ભાવ <deque>
#અન્તર્ભાવ <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename ગાંઠ_2, typename Ref>
સંરચના HashConsPool {
જાહેર:
	bool is_live(Ref ref) અચલ;

સુરક્ષિત:
	std::deque<ગાંઠ_2> backing;
	std::vector<Ref> table;
	std::vector<size_t> free_list;

	HashConsPool();
	HashConsPool(અચલ HashConsPool& other) = મૂળભૂત;
	HashConsPool(HashConsPool&& other);
	HashConsPool& operator=(અચલ HashConsPool& other) = મૂળભૂત;
	HashConsPool& operator=(HashConsPool&& other);

	સ્થિર શૂન્ય check_ready();

	અચલ ગાંઠ_2& operator[] (Ref ref) અચલ;

	template<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) અચલ;
	Ref add_inner(ગાંઠ_2 t);

	template<typename Roots>
	size_t gc(અચલ Roots& roots);

ખાનગી:
	Derived* self();
	અચલ Derived* self() અચલ;

	શૂન્ય reset();

	સ્થિર bool is_static(Ref ref);

	સંરચના RefIterator {
		અચલ HashConsPool* pool;
		size_t idx;
		size_t stop;
		bool skip_dead;

		શૂન્ય settle();
		Ref operator*() અચલ;
		RefIterator& operator++();
		bool operator!=(અચલ RefIterator& other) અચલ;
	};

	સંરચના RefRange {
		અચલ HashConsPool* pool;
		size_t first;
		size_t stop;
		bool skip_dead;

		RefIterator begin() અચલ;
		RefIterator end() અચલ;
	};

	RefRange refs(bool include_statics = true) અચલ;
	// Includes the dead
	RefRange slots() અચલ;

	શૂન્ય rebuild_index();
	size_t home_slot(uint64_t hash) અચલ;
	size_t next_slot(size_t slot) અચલ;
	શૂન્ય index_ઉમેરો(Ref ref);
	શૂન્ય index_erase(Ref ref);
	શૂન્ય grow_index();

	size_t size() અચલ;
	bool slot_live(size_t abs) અચલ;
	શૂન્ય mark_live(Ref ref, pool<Ref>& live) અચલ;
};

YOSYS_NAMESPACE_END

#endif
