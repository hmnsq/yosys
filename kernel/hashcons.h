#اگر_غیر_تعریف_شدہ YOSYS_HASHCONS_H
#تعریف_کرو YOSYS_HASHCONS_H

#شامل "kernel/yosys_common.h"

#شامل <bit>
#شامل <deque>
#شامل <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

سانچہ<typename Derived, typename گرہ_2, typename Ref>
ساخت HashConsPool {
عوامی:
	bool is_زندہ(Ref ref) ثابت;

محفوظ:
	std::deque<گرہ_2> backing;
	std::سمتیہ<Ref> جدول_2;
	std::سمتیہ<سائز_t> free_list;

	HashConsPool();
	HashConsPool(ثابت HashConsPool& other) = طےشدہ;
	HashConsPool(HashConsPool&& other);
	HashConsPool& عامل=(ثابت HashConsPool& other) = طےشدہ;
	HashConsPool& عامل=(HashConsPool&& other);

	ساکن خالی check_ready();

	ثابت گرہ_2& عامل[] (Ref ref) ثابت;

	سانچہ<typename Eq>
	Ref ڈھونڈو_hashed(uint64_t hash, Eq&& eq) ثابت;
	Ref جوڑو_inner(گرہ_2 t);

	سانچہ<typename Roots>
	سائز_t gc(ثابت Roots& roots);

نجی:
	Derived* self();
	ثابت Derived* self() ثابت;

	خالی reset();

	ساکن bool is_static(Ref ref);

	ساخت RefIterator {
		ثابت HashConsPool* pool;
		سائز_t idx;
		سائز_t stop;
		bool skip_dead;

		خالی settle();
		Ref عامل*() ثابت;
		RefIterator& عامل++();
		bool عامل!=(ثابت RefIterator& other) ثابت;
	};

	ساخت RefRange {
		ثابت HashConsPool* pool;
		سائز_t پہلا;
		سائز_t stop;
		bool skip_dead;

		RefIterator begin() ثابت;
		RefIterator انتہا() ثابت;
	};

	RefRange refs(bool include_statics = true) ثابت;
	// Includes the dead
	RefRange slots() ثابت;

	خالی rebuild_اشاریہ();
	سائز_t home_خانہ(uint64_t hash) ثابت;
	سائز_t اگلا_خانہ(سائز_t خانہ) ثابت;
	خالی اشاریہ_داخل_کرو(Ref ref);
	خالی اشاریہ_erase(Ref ref);
	خالی grow_اشاریہ();

	سائز_t سائز() ثابت;
	bool خانہ_زندہ(سائز_t مطلق_قدر) ثابت;
	خالی mark_زندہ(Ref ref, pool<Ref>& زندہ) ثابت;
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
