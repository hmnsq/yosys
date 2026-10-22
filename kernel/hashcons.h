#अगर_अघोषित YOSYS_HASHCONS_H
#घोषणा YOSYS_HASHCONS_H

#समावेश "kernel/yosys_common.h"

#समावेश <bit>
#समावेश <deque>
#समावेश <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

ढाँचा<typename Derived, typename गाँठ_2, typename Ref>
काष्ठा HashConsPool {
खुला:
	bool is_live(Ref ref) स्थिर;

रक्षित:
	std::deque<गाँठ_2> backing;
	std::vector<Ref> table;
	std::vector<माप_प्रकार> free_list;

	HashConsPool();
	HashConsPool(स्थिर HashConsPool& other) = शेष;
	HashConsPool(HashConsPool&& other);
	HashConsPool& चालक=(स्थिर HashConsPool& other) = शेष;
	HashConsPool& चालक=(HashConsPool&& other);

	अटल व्योम check_ready();

	स्थिर गाँठ_2& चालक[] (Ref ref) स्थिर;

	ढाँचा<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) स्थिर;
	Ref add_inner(गाँठ_2 t);

	ढाँचा<typename Roots>
	माप_प्रकार gc(स्थिर Roots& roots);

निजी:
	Derived* self();
	स्थिर Derived* self() स्थिर;

	व्योम reset();

	अटल bool is_static(Ref ref);

	काष्ठा RefIterator {
		स्थिर HashConsPool* pool;
		माप_प्रकार idx;
		माप_प्रकार stop;
		bool skip_dead;

		व्योम settle();
		Ref चालक*() स्थिर;
		RefIterator& चालक++();
		bool चालक!=(स्थिर RefIterator& other) स्थिर;
	};

	काष्ठा RefRange {
		स्थिर HashConsPool* pool;
		माप_प्रकार first;
		माप_प्रकार stop;
		bool skip_dead;

		RefIterator begin() स्थिर;
		RefIterator end() स्थिर;
	};

	RefRange refs(bool include_statics = true) स्थिर;
	// Includes the dead
	RefRange slots() स्थिर;

	व्योम rebuild_सूचकांक();
	माप_प्रकार home_slot(uint64_t hash) स्थिर;
	माप_प्रकार अग्र_slot(माप_प्रकार slot) स्थिर;
	व्योम सूचकांक_जोड़ो(Ref ref);
	व्योम सूचकांक_erase(Ref ref);
	व्योम grow_सूचकांक();

	माप_प्रकार आकार() स्थिर;
	bool slot_live(माप_प्रकार असल) स्थिर;
	व्योम mark_live(Ref ref, pool<Ref>& live) स्थिर;
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
