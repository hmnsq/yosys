#ifndef YOSYS_HASHCONS_H
#define YOSYS_HASHCONS_H

#অন্তর্ভাব "kernel/yosys_common.h"

#অন্তর্ভাব <bit>
#অন্তর্ভাব <deque>
#অন্তর্ভাব <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename গ্রন্থি_2, typename Ref>
সংরচনা HashConsPool {
সার্বজনিক:
	bool is_live(Ref ref) ধ্রুব;

রক্ষিত:
	std::deque<গ্রন্থি_2> backing;
	std::vector<Ref> table;
	std::vector<size_t> free_list;

	HashConsPool();
	HashConsPool(ধ্রুব HashConsPool& other) = স্বভাব;
	HashConsPool(HashConsPool&& other);
	HashConsPool& operator=(ধ্রুব HashConsPool& other) = স্বভাব;
	HashConsPool& operator=(HashConsPool&& other);

	স্থির শূন্য check_ready();

	ধ্রুব গ্রন্থি_2& operator[] (Ref ref) ধ্রুব;

	template<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) ধ্রুব;
	Ref add_inner(গ্রন্থি_2 t);

	template<typename Roots>
	size_t gc(ধ্রুব Roots& roots);

গুপ্ত:
	Derived* self();
	ধ্রুব Derived* self() ধ্রুব;

	শূন্য reset();

	স্থির bool is_static(Ref ref);

	সংরচনা RefIterator {
		ধ্রুব HashConsPool* pool;
		size_t idx;
		size_t stop;
		bool skip_dead;

		শূন্য settle();
		Ref operator*() ধ্রুব;
		RefIterator& operator++();
		bool operator!=(ধ্রুব RefIterator& other) ধ্রুব;
	};

	সংরচনা RefRange {
		ধ্রুব HashConsPool* pool;
		size_t first;
		size_t stop;
		bool skip_dead;

		RefIterator begin() ধ্রুব;
		RefIterator end() ধ্রুব;
	};

	RefRange refs(bool include_statics = true) ধ্রুব;
	// Includes the dead
	RefRange slots() ধ্রুব;

	শূন্য rebuild_index();
	size_t home_slot(uint64_t hash) ধ্রুব;
	size_t next_slot(size_t slot) ধ্রুব;
	শূন্য index_ঢোকাও(Ref ref);
	শূন্য index_erase(Ref ref);
	শূন্য grow_index();

	size_t size() ধ্রুব;
	bool slot_live(size_t abs) ধ্রুব;
	শূন্য mark_live(Ref ref, pool<Ref>& live) ধ্রুব;
};

YOSYS_NAMESPACE_END

#endif
