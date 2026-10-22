#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ YOSYS_HASHCONS_H
#ವ್ಯಾಖ್ಯಾನಿಸು YOSYS_HASHCONS_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"

#ಅನ್ತರ್ಭಾವ <bit>
#ಅನ್ತರ್ಭಾವ <deque>
#ಅನ್ತರ್ಭಾವ <vector>

/**
 * Implements deduplicating backing storage indexed by content hash
 */

YOSYS_NAMESPACE_BEGIN

ಮಾದರಿ<typename Derived, typename ಗಂಟು_2, typename Ref>
ರಚನೆ HashConsPool {
ಸಾರ್ವಜನಿಕ:
	bool is_ಜೀವಂತ(Ref ref) ಸ್ಥಿರಾಂಕ;

ಸಂರಕ್ಷಿತ:
	std::deque<ಗಂಟು_2> backing;
	std::ಸದಿಶ<Ref> ಕೋಷ್ಟಕ_2;
	std::ಸದಿಶ<ಗಾತ್ರ_t> free_list;

	HashConsPool();
	HashConsPool(ಸ್ಥಿರಾಂಕ HashConsPool& other) = ಪೂರ್ವನಿಯೋಜಿತ;
	HashConsPool(HashConsPool&& other);
	HashConsPool& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ HashConsPool& other) = ಪೂರ್ವನಿಯೋಜಿತ;
	HashConsPool& ಕ್ರಿಯಾಕಾರಕ=(HashConsPool&& other);

	ಸ್ಥಿರ ಶೂನ್ಯ check_ready();

	ಸ್ಥಿರಾಂಕ ಗಂಟು_2& ಕ್ರಿಯಾಕಾರಕ[] (Ref ref) ಸ್ಥಿರಾಂಕ;

	ಮಾದರಿ<typename Eq>
	Ref ಕಂಡುಹಿಡಿ_hashed(uint64_t hash, Eq&& eq) ಸ್ಥಿರಾಂಕ;
	Ref ಕೂಡಿಸು_inner(ಗಂಟು_2 t);

	ಮಾದರಿ<typename Roots>
	ಗಾತ್ರ_t gc(ಸ್ಥಿರಾಂಕ Roots& roots);

ಖಾಸಗಿ:
	Derived* self();
	ಸ್ಥಿರಾಂಕ Derived* self() ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ reset();

	ಸ್ಥಿರ bool is_static(Ref ref);

	ರಚನೆ RefIterator {
		ಸ್ಥಿರಾಂಕ HashConsPool* pool;
		ಗಾತ್ರ_t idx;
		ಗಾತ್ರ_t stop;
		bool skip_dead;

		ಶೂನ್ಯ settle();
		Ref ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ;
		RefIterator& ಕ್ರಿಯಾಕಾರಕ++();
		bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ RefIterator& other) ಸ್ಥಿರಾಂಕ;
	};

	ರಚನೆ RefRange {
		ಸ್ಥಿರಾಂಕ HashConsPool* pool;
		ಗಾತ್ರ_t ಮೊದಲ;
		ಗಾತ್ರ_t stop;
		bool skip_dead;

		RefIterator begin() ಸ್ಥಿರಾಂಕ;
		RefIterator ಕೊನೆ() ಸ್ಥಿರಾಂಕ;
	};

	RefRange refs(bool include_statics = true) ಸ್ಥಿರಾಂಕ;
	// Includes the dead
	RefRange slots() ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ rebuild_ಸೂಚ್ಯಂಕ();
	ಗಾತ್ರ_t home_ಸ್ಥಾನ(uint64_t hash) ಸ್ಥಿರಾಂಕ;
	ಗಾತ್ರ_t ಮುಂದಿನ_ಸ್ಥಾನ(ಗಾತ್ರ_t ಸ್ಥಾನ) ಸ್ಥಿರಾಂಕ;
	ಶೂನ್ಯ ಸೂಚ್ಯಂಕ_ಸೇರಿಸು(Ref ref);
	ಶೂನ್ಯ ಸೂಚ್ಯಂಕ_erase(Ref ref);
	ಶೂನ್ಯ grow_ಸೂಚ್ಯಂಕ();

	ಗಾತ್ರ_t ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ;
	bool ಸ್ಥಾನ_ಜೀವಂತ(ಗಾತ್ರ_t ನಿರಪೇಕ್ಷ) ಸ್ಥಿರಾಂಕ;
	ಶೂನ್ಯ mark_ಜೀವಂತ(Ref ref, pool<Ref>& ಜೀವಂತ) ಸ್ಥಿರಾಂಕ;
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
