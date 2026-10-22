#ifndef YOSYS_HASHCONS_H
#define YOSYS_HASHCONS_H

#अन्तर्भाव "kernel/yosys_common.h"

#अन्तर्भाव <bit>
#अन्तर्भाव <deque>
#अन्तर्भाव <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

ढाँचा<typename Derived, typename गाँठो_2, typename Ref>
संरचना HashConsPool {
सार्वजनिक:
	bool is_live(Ref ref) अचर;

रक्षित:
	std::deque<गाँठो_2> backing;
	std::vector<Ref> table;
	std::vector<माप_प्रकार> free_list;

	HashConsPool();
	HashConsPool(अचर HashConsPool& other) = स्वभाव;
	HashConsPool(HashConsPool&& other);
	HashConsPool& चालक=(अचर HashConsPool& other) = स्वभाव;
	HashConsPool& चालक=(HashConsPool&& other);

	स्थिर शून्य check_ready();

	अचर गाँठो_2& चालक[] (Ref ref) अचर;

	ढाँचा<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) अचर;
	Ref add_inner(गाँठो_2 t);

	ढाँचा<typename Roots>
	माप_प्रकार gc(अचर Roots& roots);

गुप्त:
	Derived* self();
	अचर Derived* self() अचर;

	शून्य reset();

	स्थिर bool is_static(Ref ref);

	संरचना RefIterator {
		अचर HashConsPool* pool;
		माप_प्रकार idx;
		माप_प्रकार stop;
		bool skip_dead;

		शून्य settle();
		Ref चालक*() अचर;
		RefIterator& चालक++();
		bool चालक!=(अचर RefIterator& other) अचर;
	};

	संरचना RefRange {
		अचर HashConsPool* pool;
		माप_प्रकार first;
		माप_प्रकार stop;
		bool skip_dead;

		RefIterator begin() अचर;
		RefIterator end() अचर;
	};

	RefRange refs(bool include_statics = true) अचर;
	// Includes the dead
	RefRange slots() अचर;

	शून्य rebuild_index();
	माप_प्रकार home_slot(uint64_t hash) अचर;
	माप_प्रकार next_slot(माप_प्रकार slot) अचर;
	शून्य index_घुसाउ(Ref ref);
	शून्य index_erase(Ref ref);
	शून्य grow_index();

	माप_प्रकार size() अचर;
	bool slot_live(माप_प्रकार असल) अचर;
	शून्य mark_live(Ref ref, pool<Ref>& live) अचर;
};

YOSYS_NAMESPACE_END

#endif
