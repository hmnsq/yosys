#جے_بنا_تعریف YOSYS_HASHCONS_H
#تعریف_کرو YOSYS_HASHCONS_H

#ضمن "kernel/yosys_common.h"

#ضمن <bit>
#ضمن <deque>
#ضمن <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

سانچا<typename Derived, typename گرہ_2, typename Ref>
بنية HashConsPool {
عام:
	bool is_جیوندا(Ref ref) ثابت;

محمي:
	std::deque<گرہ_2> backing;
	std::سمتیہ<Ref> جدول_2;
	std::سمتیہ<ناپ_t> free_list;

	HashConsPool();
	HashConsPool(ثابت HashConsPool& other) = افتراضي;
	HashConsPool(HashConsPool&& other);
	HashConsPool& عامل=(ثابت HashConsPool& other) = افتراضي;
	HashConsPool& عامل=(HashConsPool&& other);

	ساكن فراغ check_ready();

	ثابت گرہ_2& عامل[] (Ref ref) ثابت;

	سانچا<typename Eq>
	Ref لبھو_hashed(uint64_t hash, Eq&& eq) ثابت;
	Ref جوڑو_inner(گرہ_2 t);

	سانچا<typename Roots>
	ناپ_t gc(ثابت Roots& roots);

خاص:
	Derived* self();
	ثابت Derived* self() ثابت;

	فراغ reset();

	ساكن bool is_static(Ref ref);

	بنية RefIterator {
		ثابت HashConsPool* pool;
		ناپ_t idx;
		ناپ_t stop;
		bool skip_dead;

		فراغ settle();
		Ref عامل*() ثابت;
		RefIterator& عامل++();
		bool عامل!=(ثابت RefIterator& other) ثابت;
	};

	بنية RefRange {
		ثابت HashConsPool* pool;
		ناپ_t پہلا;
		ناپ_t stop;
		bool skip_dead;

		RefIterator begin() ثابت;
		RefIterator سرا() ثابت;
	};

	RefRange refs(bool include_statics = true) ثابت;
	// Includes the dead
	RefRange slots() ثابت;

	فراغ rebuild_اشاریہ();
	ناپ_t home_تھاں(uint64_t hash) ثابت;
	ناپ_t اگلا_تھاں(ناپ_t تھاں) ثابت;
	فراغ اشاریہ_داخل_کرو(Ref ref);
	فراغ اشاریہ_erase(Ref ref);
	فراغ grow_اشاریہ();

	ناپ_t ناپ() ثابت;
	bool تھاں_جیوندا(ناپ_t مطلق_قدر) ثابت;
	فراغ mark_جیوندا(Ref ref, pool<Ref>& جیوندا) ثابت;
};

YOSYS_NAMESPACE_سرا

#جے_انت
