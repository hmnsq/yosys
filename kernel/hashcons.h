#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ YOSYS_HASHCONS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YOSYS_HASHCONS_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <bit>
#ਅਨ੍ਤਰ੍ਭਾਵ <deque>
#ਅਨ੍ਤਰ੍ਭਾਵ <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

ਸਾਂਚਾ<typename Derived, typename ਗੰਢ_2, typename Ref>
ਬਣਤਰ HashConsPool {
ਜਨਤਕ:
	bool is_ਜੀਵਤ(Ref ref) ਅਚਲ;

ਸੁਰੱਖਿਅਤ:
	std::deque<ਗੰਢ_2> backing;
	std::ਸਦਿਸ਼<Ref> ਸਾਰਣੀ_2;
	std::ਸਦਿਸ਼<ਮਾਪ_t> free_list;

	HashConsPool();
	HashConsPool(ਅਚਲ HashConsPool& other) = ਮੂਲ;
	HashConsPool(HashConsPool&& other);
	HashConsPool& ਸੰਚਾਲਕ=(ਅਚਲ HashConsPool& other) = ਮੂਲ;
	HashConsPool& ਸੰਚਾਲਕ=(HashConsPool&& other);

	ਸਥਿਰ ਖਾਲੀ check_ready();

	ਅਚਲ ਗੰਢ_2& ਸੰਚਾਲਕ[] (Ref ref) ਅਚਲ;

	ਸਾਂਚਾ<typename Eq>
	Ref ਲੱਭੋ_hashed(uint64_t hash, Eq&& eq) ਅਚਲ;
	Ref ਜੋੜੋ_inner(ਗੰਢ_2 t);

	ਸਾਂਚਾ<typename Roots>
	ਮਾਪ_t gc(ਅਚਲ Roots& roots);

ਨਿਜੀ:
	Derived* self();
	ਅਚਲ Derived* self() ਅਚਲ;

	ਖਾਲੀ reset();

	ਸਥਿਰ bool is_static(Ref ref);

	ਬਣਤਰ RefIterator {
		ਅਚਲ HashConsPool* pool;
		ਮਾਪ_t idx;
		ਮਾਪ_t stop;
		bool skip_dead;

		ਖਾਲੀ settle();
		Ref ਸੰਚਾਲਕ*() ਅਚਲ;
		RefIterator& ਸੰਚਾਲਕ++();
		bool ਸੰਚਾਲਕ!=(ਅਚਲ RefIterator& other) ਅਚਲ;
	};

	ਬਣਤਰ RefRange {
		ਅਚਲ HashConsPool* pool;
		ਮਾਪ_t ਪਹਿਲਾ;
		ਮਾਪ_t stop;
		bool skip_dead;

		RefIterator begin() ਅਚਲ;
		RefIterator ਸਿਰਾ() ਅਚਲ;
	};

	RefRange refs(bool include_statics = true) ਅਚਲ;
	// Includes the dead
	RefRange slots() ਅਚਲ;

	ਖਾਲੀ rebuild_ਸੂਚਕਾਂਕ();
	ਮਾਪ_t home_ਥਾਂ(uint64_t hash) ਅਚਲ;
	ਮਾਪ_t ਅਗਲਾ_ਥਾਂ(ਮਾਪ_t ਥਾਂ) ਅਚਲ;
	ਖਾਲੀ ਸੂਚਕਾਂਕ_ਪਾਓ(Ref ref);
	ਖਾਲੀ ਸੂਚਕਾਂਕ_erase(Ref ref);
	ਖਾਲੀ grow_ਸੂਚਕਾਂਕ();

	ਮਾਪ_t ਮਾਪ() ਅਚਲ;
	bool ਥਾਂ_ਜੀਵਤ(ਮਾਪ_t ਨਿਰਪੇਖ) ਅਚਲ;
	ਖਾਲੀ mark_ਜੀਵਤ(Ref ref, pool<Ref>& ਜੀਵਤ) ਅਚਲ;
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
