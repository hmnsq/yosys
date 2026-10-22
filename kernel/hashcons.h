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
কাঠামো HashConsPool {
সর্বজনীন:
	bool is_live(Ref ref) ধ্রুবক;

সুরক্ষিত:
	std::deque<গ্রন্থি_2> backing;
	std::vector<Ref> table;
	std::vector<আকার_t> free_list;

	HashConsPool();
	HashConsPool(ধ্রুবক HashConsPool& other) = পূর্বনির্ধারিত;
	HashConsPool(HashConsPool&& other);
	HashConsPool& operator=(ধ্রুবক HashConsPool& other) = পূর্বনির্ধারিত;
	HashConsPool& operator=(HashConsPool&& other);

	স্থির শূন্য check_ready();

	ধ্রুবক গ্রন্থি_2& operator[] (Ref ref) ধ্রুবক;

	template<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) ধ্রুবক;
	Ref add_inner(গ্রন্থি_2 t);

	template<typename Roots>
	আকার_t gc(ধ্রুবক Roots& roots);

ব্যক্তিগত:
	Derived* self();
	ধ্রুবক Derived* self() ধ্রুবক;

	শূন্য reset();

	স্থির bool is_static(Ref ref);

	কাঠামো RefIterator {
		ধ্রুবক HashConsPool* pool;
		আকার_t idx;
		আকার_t stop;
		bool skip_dead;

		শূন্য settle();
		Ref operator*() ধ্রুবক;
		RefIterator& operator++();
		bool operator!=(ধ্রুবক RefIterator& other) ধ্রুবক;
	};

	কাঠামো RefRange {
		ধ্রুবক HashConsPool* pool;
		আকার_t first;
		আকার_t stop;
		bool skip_dead;

		RefIterator begin() ধ্রুবক;
		RefIterator end() ধ্রুবক;
	};

	RefRange refs(bool include_statics = true) ধ্রুবক;
	// Includes the dead
	RefRange slots() ধ্রুবক;

	শূন্য rebuild_সূচক();
	আকার_t home_slot(uint64_t hash) ধ্রুবক;
	আকার_t পরবর্তী_slot(আকার_t slot) ধ্রুবক;
	শূন্য সূচক_ঢোকাও(Ref ref);
	শূন্য সূচক_erase(Ref ref);
	শূন্য grow_সূচক();

	আকার_t আকার_2() ধ্রুবক;
	bool slot_live(আকার_t abs) ধ্রুবক;
	শূন্য mark_live(Ref ref, pool<Ref>& live) ধ্রুবক;
};

YOSYS_NAMESPACE_END

#endif
