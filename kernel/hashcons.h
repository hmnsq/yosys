#ifndef YOSYS_HASHCONS_H
#define YOSYS_HASHCONS_H

#ضمن "kernel/yosys_common.h"

#ضمن <bit>
#ضمن <deque>
#ضمن <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename گرێ_2, typename Ref>
بنية HashConsPool {
عام:
	bool is_live(Ref ref) ثابت;

محمي:
	std::deque<گرێ_2> backing;
	std::vector<Ref> table;
	std::vector<size_t> free_list;

	HashConsPool();
	HashConsPool(ثابت HashConsPool& other) = افتراضي;
	HashConsPool(HashConsPool&& other);
	HashConsPool& operator=(ثابت HashConsPool& other) = افتراضي;
	HashConsPool& operator=(HashConsPool&& other);

	ساكن فراغ check_ready();

	ثابت گرێ_2& operator[] (Ref ref) ثابت;

	template<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) ثابت;
	Ref add_inner(گرێ_2 t);

	template<typename Roots>
	size_t gc(ثابت Roots& roots);

خاص:
	Derived* self();
	ثابت Derived* self() ثابت;

	فراغ reset();

	ساكن bool is_static(Ref ref);

	بنية RefIterator {
		ثابت HashConsPool* pool;
		size_t idx;
		size_t stop;
		bool skip_dead;

		فراغ settle();
		Ref operator*() ثابت;
		RefIterator& operator++();
		bool operator!=(ثابت RefIterator& other) ثابت;
	};

	بنية RefRange {
		ثابت HashConsPool* pool;
		size_t first;
		size_t stop;
		bool skip_dead;

		RefIterator begin() ثابت;
		RefIterator end() ثابت;
	};

	RefRange refs(bool include_statics = true) ثابت;
	// Includes the dead
	RefRange slots() ثابت;

	فراغ rebuild_index();
	size_t home_slot(uint64_t hash) ثابت;
	size_t next_slot(size_t slot) ثابت;
	فراغ index_زیادکردن(Ref ref);
	فراغ index_erase(Ref ref);
	فراغ grow_index();

	size_t size() ثابت;
	bool slot_live(size_t abs) ثابت;
	فراغ mark_live(Ref ref, pool<Ref>& live) ثابت;
};

YOSYS_NAMESPACE_END

#endif
