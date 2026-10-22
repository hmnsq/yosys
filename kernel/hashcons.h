#که_نه_تعریف_شوی YOSYS_HASHCONS_H
#تعریف_کړه YOSYS_HASHCONS_H

#ضمن "kernel/yosys_common.h"

#ضمن <bit>
#ضمن <deque>
#ضمن <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
بنية HashConsPool {
عام:
	bool is_ژوندی(Ref ref) ثابت;

محمي:
	std::deque<غوټه_2> backing;
	std::ویکټور<Ref> جدول_2;
	std::ویکټور<اندازه_t> free_list;

	HashConsPool();
	HashConsPool(ثابت HashConsPool& other) = افتراضي;
	HashConsPool(HashConsPool&& other);
	HashConsPool& چلوونکی=(ثابت HashConsPool& other) = افتراضي;
	HashConsPool& چلوونکی=(HashConsPool&& other);

	ساكن فراغ check_ready();

	ثابت غوټه_2& چلوونکی[] (Ref ref) ثابت;

	کينډۍ<typename Eq>
	Ref پیدا_کړه_hashed(uint64_t hash, Eq&& eq) ثابت;
	Ref جمع_کړه_inner(غوټه_2 t);

	کينډۍ<typename Roots>
	اندازه_t gc(ثابت Roots& roots);

خاص:
	Derived* self();
	ثابت Derived* self() ثابت;

	فراغ reset();

	ساكن bool is_static(Ref ref);

	بنية RefIterator {
		ثابت HashConsPool* pool;
		اندازه_t idx;
		اندازه_t stop;
		bool skip_dead;

		فراغ settle();
		Ref چلوونکی*() ثابت;
		RefIterator& چلوونکی++();
		bool چلوونکی!=(ثابت RefIterator& other) ثابت;
	};

	بنية RefRange {
		ثابت HashConsPool* pool;
		اندازه_t لومړی;
		اندازه_t stop;
		bool skip_dead;

		RefIterator begin() ثابت;
		RefIterator پای_ټکی() ثابت;
	};

	RefRange refs(bool include_statics = true) ثابت;
	// Includes the dead
	RefRange slots() ثابت;

	فراغ rebuild_شاخص();
	اندازه_t home_خونه(uint64_t hash) ثابت;
	اندازه_t بل_خونه(اندازه_t خونه) ثابت;
	فراغ شاخص_ورننباسه(Ref ref);
	فراغ شاخص_erase(Ref ref);
	فراغ grow_شاخص();

	اندازه_t اندازه() ثابت;
	bool خونه_ژوندی(اندازه_t مطلق_قیمت) ثابت;
	فراغ mark_ژوندی(Ref ref, pool<Ref>& ژوندی) ثابت;
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
