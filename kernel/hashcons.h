#ifndef YOSYS_HASHCONS_H
#define YOSYS_HASHCONS_H

#ଅନ୍ତର୍ଭାଵ "kernel/yosys_common.h"

#ଅନ୍ତର୍ଭାଵ <bit>
#ଅନ୍ତର୍ଭାଵ <deque>
#ଅନ୍ତର୍ଭାଵ <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename ଗଣ୍ଠି_2, typename Ref>
ସଂରଚନା HashConsPool {
ସର୍ବସାଧାରଣ:
	bool is_live(Ref ref) ଅଚଳ;

ସୁରକ୍ଷିତ:
	std::deque<ଗଣ୍ଠି_2> backing;
	std::vector<Ref> table;
	std::vector<size_t> free_list;

	HashConsPool();
	HashConsPool(ଅଚଳ HashConsPool& other) = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	HashConsPool(HashConsPool&& other);
	HashConsPool& operator=(ଅଚଳ HashConsPool& other) = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	HashConsPool& operator=(HashConsPool&& other);

	ସ୍ଥିର ଶୂନ୍ୟ check_ready();

	ଅଚଳ ଗଣ୍ଠି_2& operator[] (Ref ref) ଅଚଳ;

	template<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) ଅଚଳ;
	Ref add_inner(ଗଣ୍ଠି_2 t);

	template<typename Roots>
	size_t gc(ଅଚଳ Roots& roots);

ବ୍ୟକ୍ତିଗତ:
	Derived* self();
	ଅଚଳ Derived* self() ଅଚଳ;

	ଶୂନ୍ୟ reset();

	ସ୍ଥିର bool is_static(Ref ref);

	ସଂରଚନା RefIterator {
		ଅଚଳ HashConsPool* pool;
		size_t idx;
		size_t stop;
		bool skip_dead;

		ଶୂନ୍ୟ settle();
		Ref operator*() ଅଚଳ;
		RefIterator& operator++();
		bool operator!=(ଅଚଳ RefIterator& other) ଅଚଳ;
	};

	ସଂରଚନା RefRange {
		ଅଚଳ HashConsPool* pool;
		size_t first;
		size_t stop;
		bool skip_dead;

		RefIterator begin() ଅଚଳ;
		RefIterator end() ଅଚଳ;
	};

	RefRange refs(bool include_statics = true) ଅଚଳ;
	// Includes the dead
	RefRange slots() ଅଚଳ;

	ଶୂନ୍ୟ rebuild_index();
	size_t home_slot(uint64_t hash) ଅଚଳ;
	size_t next_slot(size_t slot) ଅଚଳ;
	ଶୂନ୍ୟ index_ଭର୍ତ୍ତି(Ref ref);
	ଶୂନ୍ୟ index_erase(Ref ref);
	ଶୂନ୍ୟ grow_index();

	size_t size() ଅଚଳ;
	bool slot_live(size_t abs) ଅଚଳ;
	ଶୂନ୍ୟ mark_live(Ref ref, pool<Ref>& live) ଅଚଳ;
};

YOSYS_NAMESPACE_END

#endif
