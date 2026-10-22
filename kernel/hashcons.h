#جيڪڏهن_وصف_نه_ٿيل YOSYS_HASHCONS_H
#وصف_ڪريو YOSYS_HASHCONS_H

#ضمن "kernel/yosys_common.h"

#ضمن <bit>
#ضمن <deque>
#ضمن <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
بنية HashConsPool {
عام:
	bool is_جيئرو(Ref ref) ثابت;

محمي:
	std::deque<ڳنڍ_2> backing;
	std::ويڪٽر<Ref> جدول_2;
	std::ويڪٽر<سائيز_t> free_list;

	HashConsPool();
	HashConsPool(ثابت HashConsPool& other) = افتراضي;
	HashConsPool(HashConsPool&& other);
	HashConsPool& عامل=(ثابت HashConsPool& other) = افتراضي;
	HashConsPool& عامل=(HashConsPool&& other);

	ساكن فراغ check_ready();

	ثابت ڳنڍ_2& عامل[] (Ref ref) ثابت;

	سانچو<typename Eq>
	Ref ڳوليو_hashed(uint64_t hash, Eq&& eq) ثابت;
	Ref جوڙيو_inner(ڳنڍ_2 t);

	سانچو<typename Roots>
	سائيز_t gc(ثابت Roots& roots);

خاص:
	Derived* self();
	ثابت Derived* self() ثابت;

	فراغ reset();

	ساكن bool is_static(Ref ref);

	بنية RefIterator {
		ثابت HashConsPool* pool;
		سائيز_t idx;
		سائيز_t stop;
		bool skip_dead;

		فراغ settle();
		Ref عامل*() ثابت;
		RefIterator& عامل++();
		bool عامل!=(ثابت RefIterator& other) ثابت;
	};

	بنية RefRange {
		ثابت HashConsPool* pool;
		سائيز_t پهريون;
		سائيز_t stop;
		bool skip_dead;

		RefIterator begin() ثابت;
		RefIterator پڇاڙي() ثابت;
	};

	RefRange refs(bool include_statics = true) ثابت;
	// Includes the dead
	RefRange slots() ثابت;

	فراغ rebuild_اشاريو();
	سائيز_t home_جڳهه(uint64_t hash) ثابت;
	سائيز_t ايندڙ_جڳهه(سائيز_t جڳهه) ثابت;
	فراغ اشاريو_وجهو(Ref ref);
	فراغ اشاريو_erase(Ref ref);
	فراغ grow_اشاريو();

	سائيز_t سائيز() ثابت;
	bool جڳهه_جيئرو(سائيز_t مطلق_قدر) ثابت;
	فراغ mark_جيئرو(Ref ref, pool<Ref>& جيئرو) ثابت;
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
