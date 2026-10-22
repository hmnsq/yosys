#यदि_अपरिभाषितम् YOSYS_HASHCONS_H
#परिभाषय YOSYS_HASHCONS_H

#अन्तर्भाव "kernel/yosys_common.h"

#अन्तर्भाव <bit>
#अन्तर्भाव <deque>
#अन्तर्भाव <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

प्रतिरूपम्<typename Derived, typename ग्रन्थिः_2, typename Ref>
संरचना HashConsPool {
सार्वजनिक:
	bool is_जीवितम्(Ref ref) ध्रुव;

रक्षित:
	std::deque<ग्रन्थिः_2> backing;
	std::सदिशम्<Ref> सारणी_2;
	std::सदिशम्<आकार_t> free_list;

	HashConsPool();
	HashConsPool(ध्रुव HashConsPool& other) = स्वभाव;
	HashConsPool(HashConsPool&& other);
	HashConsPool& संक्रियकः=(ध्रुव HashConsPool& other) = स्वभाव;
	HashConsPool& संक्रियकः=(HashConsPool&& other);

	स्थिर शून्य check_ready();

	ध्रुव ग्रन्थिः_2& संक्रियकः[] (Ref ref) ध्रुव;

	प्रतिरूपम्<typename Eq>
	Ref find_hashed(uint64_t hash, Eq&& eq) ध्रुव;
	Ref सङ्कलय_inner(ग्रन्थिः_2 t);

	प्रतिरूपम्<typename Roots>
	आकार_t gc(ध्रुव Roots& roots);

गुप्त:
	Derived* self();
	ध्रुव Derived* self() ध्रुव;

	शून्य reset();

	स्थिर bool is_static(Ref ref);

	संरचना RefIterator {
		ध्रुव HashConsPool* pool;
		आकार_t idx;
		आकार_t stop;
		bool skip_dead;

		शून्य settle();
		Ref संक्रियकः*() ध्रुव;
		RefIterator& संक्रियकः++();
		bool संक्रियकः!=(ध्रुव RefIterator& other) ध्रुव;
	};

	संरचना RefRange {
		ध्रुव HashConsPool* pool;
		आकार_t first;
		आकार_t stop;
		bool skip_dead;

		RefIterator begin() ध्रुव;
		RefIterator अन्तः() ध्रुव;
	};

	RefRange refs(bool include_statics = true) ध्रुव;
	// Includes the dead
	RefRange slots() ध्रुव;

	शून्य rebuild_सूचकांक();
	आकार_t home_slot(uint64_t hash) ध्रुव;
	आकार_t अग्र_slot(आकार_t slot) ध्रुव;
	शून्य सूचकांक_निवेशय(Ref ref);
	शून्य सूचकांक_erase(Ref ref);
	शून्य grow_सूचकांक();

	आकार_t आकार() ध्रुव;
	bool slot_जीवितम्(आकार_t निरपेक्षम्) ध्रुव;
	शून्य mark_जीवितम्(Ref ref, pool<Ref>& जीवितम्) ध्रुव;
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
