#ifndef YOSYS_HASHCONS_H
#define YOSYS_HASHCONS_H

#شامل "kernel/yosys_common.h"

#شامل <bit>
#شامل <deque>
#شامل <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename گره_2, typename Ref>
ساختار HashConsPool {
عمومی:
	bool is_live(Ref ref) ثابت;

محافظتشده:
	std::deque<گره_2> backing;
	std::vector<Ref> table;
	std::vector<size_t> free_list;

	HashConsPool();
	HashConsPool(ثابت HashConsPool& other) = پیشفرض;
	HashConsPool(HashConsPool&& other);
	HashConsPool& operator=(ثابت HashConsPool& other) = پیشفرض;
	HashConsPool& operator=(HashConsPool&& other);

	ایستا تهی check_ready();

	ثابت گره_2& operator[] (Ref ref) ثابت;

	template<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) ثابت;
	Ref add_inner(گره_2 t);

	template<typename Roots>
	size_t gc(ثابت Roots& roots);

خصوصی:
	Derived* self();
	ثابت Derived* self() ثابت;

	تهی reset();

	ایستا bool is_static(Ref ref);

	ساختار RefIterator {
		ثابت HashConsPool* pool;
		size_t idx;
		size_t stop;
		bool skip_dead;

		تهی settle();
		Ref operator*() ثابت;
		RefIterator& operator++();
		bool operator!=(ثابت RefIterator& other) ثابت;
	};

	ساختار RefRange {
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

	تهی rebuild_index();
	size_t home_slot(uint64_t hash) ثابت;
	size_t next_slot(size_t slot) ثابت;
	تهی index_درج(Ref ref);
	تهی index_erase(Ref ref);
	تهی grow_index();

	size_t size() ثابت;
	bool slot_live(size_t abs) ثابت;
	تهی mark_live(Ref ref, pool<Ref>& live) ثابت;
};

YOSYS_NAMESPACE_END

#endif
