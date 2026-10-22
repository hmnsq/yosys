#ifndef YOSYS_HASHCONS_H
#define YOSYS_HASHCONS_H

#அந்தர்பாவ "kernel/yosys_common.h"

#அந்தர்பாவ <bit>
#அந்தர்பாவ <deque>
#அந்தர்பாவ <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename முனை_2, typename Ref>
கட்டமைப்பு HashConsPool {
பொது:
	bool is_live(Ref ref) மாறிலி;

பாதுகாக்கப்பட்ட:
	std::deque<முனை_2> backing;
	std::vector<Ref> table;
	std::vector<size_t> free_list;

	HashConsPool();
	HashConsPool(மாறிலி HashConsPool& other) = இயல்பு;
	HashConsPool(HashConsPool&& other);
	HashConsPool& operator=(மாறிலி HashConsPool& other) = இயல்பு;
	HashConsPool& operator=(HashConsPool&& other);

	நிலையான வெறுமை check_ready();

	மாறிலி முனை_2& operator[] (Ref ref) மாறிலி;

	template<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) மாறிலி;
	Ref add_inner(முனை_2 t);

	template<typename Roots>
	size_t gc(மாறிலி Roots& roots);

தனிப்பட்ட:
	Derived* self();
	மாறிலி Derived* self() மாறிலி;

	வெறுமை reset();

	நிலையான bool is_static(Ref ref);

	கட்டமைப்பு RefIterator {
		மாறிலி HashConsPool* pool;
		size_t idx;
		size_t stop;
		bool skip_dead;

		வெறுமை settle();
		Ref operator*() மாறிலி;
		RefIterator& operator++();
		bool operator!=(மாறிலி RefIterator& other) மாறிலி;
	};

	கட்டமைப்பு RefRange {
		மாறிலி HashConsPool* pool;
		size_t first;
		size_t stop;
		bool skip_dead;

		RefIterator begin() மாறிலி;
		RefIterator end() மாறிலி;
	};

	RefRange refs(bool include_statics = true) மாறிலி;
	// Includes the dead
	RefRange slots() மாறிலி;

	வெறுமை rebuild_index();
	size_t home_slot(uint64_t hash) மாறிலி;
	size_t next_slot(size_t slot) மாறிலி;
	வெறுமை index_செருகு(Ref ref);
	வெறுமை index_erase(Ref ref);
	வெறுமை grow_index();

	size_t size() மாறிலி;
	bool slot_live(size_t abs) மாறிலி;
	வெறுமை mark_live(Ref ref, pool<Ref>& live) மாறிலி;
};

YOSYS_NAMESPACE_END

#endif
