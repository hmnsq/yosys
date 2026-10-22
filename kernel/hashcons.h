#ifndef YOSYS_HASHCONS_H
#define YOSYS_HASHCONS_H

#অন্তর্ভাৱ "kernel/yosys_common.h"

#অন্তর্ভাৱ <bit>
#অন্তর্ভাৱ <deque>
#অন্তর্ভাৱ <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename গ্ৰন্থি_2, typename Ref>
সংরচনা HashConsPool {
সার্ৱজনিক:
	bool is_live(Ref ref) ধ্রুৱ;

রক্ষিত:
	std::deque<গ্ৰন্থি_2> backing;
	std::vector<Ref> table;
	std::vector<size_t> free_list;

	HashConsPool();
	HashConsPool(ধ্রুৱ HashConsPool& other) = স্ৱভাৱ;
	HashConsPool(HashConsPool&& other);
	HashConsPool& operator=(ধ্রুৱ HashConsPool& other) = স্ৱভাৱ;
	HashConsPool& operator=(HashConsPool&& other);

	স্থির শূন্য check_ready();

	ধ্রুৱ গ্ৰন্থি_2& operator[] (Ref ref) ধ্রুৱ;

	template<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) ধ্রুৱ;
	Ref add_inner(গ্ৰন্থি_2 t);

	template<typename Roots>
	size_t gc(ধ্রুৱ Roots& roots);

গুপ্ত:
	Derived* self();
	ধ্রুৱ Derived* self() ধ্রুৱ;

	শূন্য reset();

	স্থির bool is_static(Ref ref);

	সংরচনা RefIterator {
		ধ্রুৱ HashConsPool* pool;
		size_t idx;
		size_t stop;
		bool skip_dead;

		শূন্য settle();
		Ref operator*() ধ্রুৱ;
		RefIterator& operator++();
		bool operator!=(ধ্রুৱ RefIterator& other) ধ্রুৱ;
	};

	সংরচনা RefRange {
		ধ্রুৱ HashConsPool* pool;
		size_t first;
		size_t stop;
		bool skip_dead;

		RefIterator begin() ধ্রুৱ;
		RefIterator end() ধ্রুৱ;
	};

	RefRange refs(bool include_statics = true) ধ্রুৱ;
	// Includes the dead
	RefRange slots() ধ্রুৱ;

	শূন্য rebuild_index();
	size_t home_slot(uint64_t hash) ধ্রুৱ;
	size_t next_slot(size_t slot) ধ্রুৱ;
	শূন্য index_ঢোকাও(Ref ref);
	শূন্য index_erase(Ref ref);
	শূন্য grow_index();

	size_t size() ধ্রুৱ;
	bool slot_live(size_t abs) ধ্রুৱ;
	শূন্য mark_live(Ref ref, pool<Ref>& live) ধ্রুৱ;
};

YOSYS_NAMESPACE_END

#endif
