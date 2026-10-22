#ifndef YOSYS_HASHCONS_H
#define YOSYS_HASHCONS_H

#അന്തര്ഭാവ "kernel/yosys_common.h"

#അന്തര്ഭാവ <bit>
#അന്തര്ഭാവ <deque>
#അന്തര്ഭാവ <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename കണ്ണി_2, typename Ref>
ഘടന HashConsPool {
പൊതു:
	bool is_live(Ref ref) സ്ഥിരം;

സംരക്ഷിത:
	std::deque<കണ്ണി_2> backing;
	std::vector<Ref> table;
	std::vector<size_t> free_list;

	HashConsPool();
	HashConsPool(സ്ഥിരം HashConsPool& other) = സ്ഥിരസ്ഥിതി;
	HashConsPool(HashConsPool&& other);
	HashConsPool& operator=(സ്ഥിരം HashConsPool& other) = സ്ഥിരസ്ഥിതി;
	HashConsPool& operator=(HashConsPool&& other);

	സ്ഥിതം ശൂന്യം check_ready();

	സ്ഥിരം കണ്ണി_2& operator[] (Ref ref) സ്ഥിരം;

	template<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) സ്ഥിരം;
	Ref add_inner(കണ്ണി_2 t);

	template<typename Roots>
	size_t gc(സ്ഥിരം Roots& roots);

സ്വകാര്യ:
	Derived* self();
	സ്ഥിരം Derived* self() സ്ഥിരം;

	ശൂന്യം reset();

	സ്ഥിതം bool is_static(Ref ref);

	ഘടന RefIterator {
		സ്ഥിരം HashConsPool* pool;
		size_t idx;
		size_t stop;
		bool skip_dead;

		ശൂന്യം settle();
		Ref operator*() സ്ഥിരം;
		RefIterator& operator++();
		bool operator!=(സ്ഥിരം RefIterator& other) സ്ഥിരം;
	};

	ഘടന RefRange {
		സ്ഥിരം HashConsPool* pool;
		size_t first;
		size_t stop;
		bool skip_dead;

		RefIterator begin() സ്ഥിരം;
		RefIterator end() സ്ഥിരം;
	};

	RefRange refs(bool include_statics = true) സ്ഥിരം;
	// Includes the dead
	RefRange slots() സ്ഥിരം;

	ശൂന്യം rebuild_index();
	size_t home_slot(uint64_t hash) സ്ഥിരം;
	size_t next_slot(size_t slot) സ്ഥിരം;
	ശൂന്യം index_തിരുകുക(Ref ref);
	ശൂന്യം index_erase(Ref ref);
	ശൂന്യം grow_index();

	size_t size() സ്ഥിരം;
	bool slot_live(size_t abs) സ്ഥിരം;
	ശൂന്യം mark_live(Ref ref, pool<Ref>& live) സ്ഥിരം;
};

YOSYS_NAMESPACE_END

#endif
