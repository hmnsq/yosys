#ifndef YOSYS_HASHCONS_IMPL_H
#define YOSYS_HASHCONS_IMPL_H

#ضمن "kernel/hashcons.h"

#ضمن <algorithm>

YOSYS_NAMESPACE_BEGIN

template<typename Derived, typename عقدة_2, typename Ref>
bool HashConsPool<Derived, عقدة_2, Ref>::is_live(Ref ref) ثابت {
	Ref idx = ref.untag();
	إذا (is_static(idx))
		إرجاع true;
	size_t slot = idx.raw() - Derived::STATIC_تعداد;
	إرجاع slot < backing.size() && !backing[slot].is_dead();
}

template<typename Derived, typename عقدة_2, typename Ref>
فراغ HashConsPool<Derived, عقدة_2, Ref>::check_ready() {}

template<typename Derived, typename عقدة_2, typename Ref>
ثابت عقدة_2& HashConsPool<Derived, عقدة_2, Ref>::operator[] (Ref ref) ثابت {
	Ref idx = ref.untag();
	إذا constexpr (Derived::STATIC_تعداد != 0) {
		إذا (is_static(idx))
			إرجاع Derived::static_عقدة(idx.raw());
	}
	إرجاع backing[idx.raw() - Derived::STATIC_تعداد];
}

template<typename Derived, typename عقدة_2, typename Ref>
Derived* HashConsPool<Derived, عقدة_2, Ref>::self() { إرجاع static_cast<Derived*>(this); }

template<typename Derived, typename عقدة_2, typename Ref>
ثابت Derived* HashConsPool<Derived, عقدة_2, Ref>::self() ثابت { إرجاع static_cast<ثابت Derived*>(this); }

template<typename Derived, typename عقدة_2, typename Ref>
bool HashConsPool<Derived, عقدة_2, Ref>::is_static(Ref ref) {
	إذا constexpr (Derived::STATIC_تعداد == 0)
		إرجاع false;
	وإلا
		إرجاع ref.raw() < Derived::STATIC_تعداد;
}

template<typename Derived, typename عقدة_2, typename Ref>
فراغ HashConsPool<Derived, عقدة_2, Ref>::RefIterator::settle() {
	بينما (skip_dead && idx < stop && !pool->slot_live(idx))
		idx++;
}

template<typename Derived, typename عقدة_2, typename Ref>
Ref HashConsPool<Derived, عقدة_2, Ref>::RefIterator::operator*() ثابت { إرجاع Ref(idx); }

template<typename Derived, typename عقدة_2, typename Ref>
auto HashConsPool<Derived, عقدة_2, Ref>::RefIterator::operator++() -> RefIterator& { idx++; settle(); إرجاع *this; }

template<typename Derived, typename عقدة_2, typename Ref>
bool HashConsPool<Derived, عقدة_2, Ref>::RefIterator::operator!=(ثابت RefIterator& other) ثابت { إرجاع idx != other.idx; }

template<typename Derived, typename عقدة_2, typename Ref>
auto HashConsPool<Derived, عقدة_2, Ref>::RefRange::begin() ثابت -> RefIterator {
	RefIterator it{pool, first, stop, skip_dead};
	it.settle();
	إرجاع it;
}

template<typename Derived, typename عقدة_2, typename Ref>
auto HashConsPool<Derived, عقدة_2, Ref>::RefRange::end() ثابت -> RefIterator { إرجاع RefIterator{pool, stop, stop, skip_dead}; }

template<typename Derived, typename عقدة_2, typename Ref>
auto HashConsPool<Derived, عقدة_2, Ref>::refs(bool include_statics) ثابت -> RefRange {
	إرجاع RefRange{this, include_statics ? 0 : Derived::STATIC_تعداد,
			Derived::STATIC_تعداد + backing.size(), true};
}

template<typename Derived, typename عقدة_2, typename Ref>
auto HashConsPool<Derived, عقدة_2, Ref>::slots() ثابت -> RefRange {
	إرجاع RefRange{this, Derived::STATIC_تعداد,
			Derived::STATIC_تعداد + backing.size(), false};
}

template<typename Derived, typename عقدة_2, typename Ref>
size_t HashConsPool<Derived, عقدة_2, Ref>::home_slot(uint64_t hash) ثابت {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	إرجاع (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(table.size()));
}

template<typename Derived, typename عقدة_2, typename Ref>
size_t HashConsPool<Derived, عقدة_2, Ref>::next_slot(size_t slot) ثابت { إرجاع (slot + 1) & (table.size() - 1); }

template<typename Derived, typename عقدة_2, typename Ref>
size_t HashConsPool<Derived, عقدة_2, Ref>::size() ثابت { إرجاع backing.size() - free_list.size(); }

template<typename Derived, typename عقدة_2, typename Ref>
bool HashConsPool<Derived, عقدة_2, Ref>::slot_live(size_t abs) ثابت {
	إذا (abs < Derived::STATIC_تعداد)
		إرجاع true;
	إرجاع !backing[abs - Derived::STATIC_تعداد].is_dead();
}

template<typename Derived, typename عقدة_2, typename Ref>
HashConsPool<Derived, عقدة_2, Ref>::HashConsPool() { rebuild_index(); }

template<typename Derived, typename عقدة_2, typename Ref>
HashConsPool<Derived, عقدة_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), table(std::move(other.table)), free_list(std::move(other.free_list)) {
	other.reset();
}

template<typename Derived, typename عقدة_2, typename Ref>
HashConsPool<Derived, عقدة_2, Ref>& HashConsPool<Derived, عقدة_2, Ref>::operator=(HashConsPool&& other) {
	إذا (this != &other) {
		backing = std::move(other.backing);
		table = std::move(other.table);
		free_list = std::move(other.free_list);
		other.reset();
	}
	إرجاع *this;
}

template<typename Derived, typename عقدة_2, typename Ref>
فراغ HashConsPool<Derived, عقدة_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_index();
}

template<typename Derived, typename عقدة_2, typename Ref>
فراغ HashConsPool<Derived, عقدة_2, Ref>::rebuild_index() {
	Derived::check_ready();
	free_list.clear();
	لكل (size_t idx = 0; idx < backing.size(); ++idx)
		إذا (backing[idx].is_dead())
			free_list.ادفع_back(idx);
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	table.assign(std::bit_ceil((Derived::STATIC_تعداد + size()) * 2 + 2), Ref());
	لكل (Ref ref : refs())
		index_أدرج(ref);
}

template<typename Derived, typename عقدة_2, typename Ref>
template<typename Eq>
Ref HashConsPool<Derived, عقدة_2, Ref>::find_hashed(uint64_t hash, Eq&& eq) ثابت {
	لكل (size_t slot = home_slot(hash); table[slot] != Ref(); slot = next_slot(slot)) {
		Ref ref = table[slot];
		إذا (Derived::hash_عقدة((*this)[ref]) == hash && eq(ref))
			إرجاع ref;
	}
	إرجاع Ref();
}

template<typename Derived, typename عقدة_2, typename Ref>
فراغ HashConsPool<Derived, عقدة_2, Ref>::index_أدرج(Ref ref) {
	size_t slot = home_slot(Derived::hash_عقدة((*this)[ref]));
	بينما (table[slot] != Ref())
		slot = next_slot(slot);
	table[slot] = ref;
}

template<typename Derived, typename عقدة_2, typename Ref>
فراغ HashConsPool<Derived, عقدة_2, Ref>::index_erase(Ref ref) {
	size_t hole = home_slot(Derived::hash_عقدة((*this)[ref]));
	بينما (table[hole] != ref)
		hole = next_slot(hole);
	size_t mask = table.size() - 1;
	لكل (size_t slot = next_slot(hole); table[slot] != Ref(); slot = next_slot(slot)) {
		size_t home = home_slot(Derived::hash_عقدة((*this)[table[slot]]));
		إذا (((slot - home) & mask) >= ((slot - hole) & mask)) {
			table[hole] = table[slot];
			hole = slot;
		}
	}
	table[hole] = Ref();
}

template<typename Derived, typename عقدة_2, typename Ref>
فراغ HashConsPool<Derived, عقدة_2, Ref>::grow_index() {
	std::vector<Ref> old(table.size() * 2, Ref());
	std::swap(old, table);
	لكل (Ref ref : old)
		إذا (ref != Ref())
			index_أدرج(ref);
}

template<typename Derived, typename عقدة_2, typename Ref>
Ref HashConsPool<Derived, عقدة_2, Ref>::add_inner(عقدة_2 t) {
	Ref ref;
	إذا (!free_list.empty()) {
		size_t idx = free_list.back();
		free_list.اسحب_back();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_تعداد + idx);
	} وإلا {
		ref = Ref(Derived::STATIC_تعداد + backing.size());
		backing.ادفع_back(std::move(t));
	}
	إذا ((Derived::STATIC_تعداد + size()) * 3 > table.size() * 2)
		grow_index();
	index_أدرج(ref);
	إذا (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	إرجاع ref;
}

template<typename Derived, typename عقدة_2, typename Ref>
template<typename Roots>
size_t HashConsPool<Derived, عقدة_2, Ref>::gc(ثابت Roots& roots) {
	pool<Ref> live;
	لكل (Ref ref : roots)
		mark_live(ref, live);
	size_t erased = 0;
	لكل (size_t idx = 0; idx < backing.size(); ++idx) {
		إذا (backing[idx].is_dead())
			استمر;
		إذا (!live.تعداد_2(Ref(Derived::STATIC_تعداد + idx))) {
			index_erase(Ref(Derived::STATIC_تعداد + idx));
			free_list.ادفع_back(idx);
			backing[idx] = عقدة_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::sort(free_list.begin(), free_list.end(), std::greater<size_t>());
	إرجاع erased;
}

template<typename Derived, typename عقدة_2, typename Ref>
فراغ HashConsPool<Derived, عقدة_2, Ref>::mark_live(Ref ref, pool<Ref>& live) ثابت {
	ref = ref.untag();
	إذا (ref == Ref() || is_static(ref) || !live.أدرج(ref).second)
		إرجاع;
	Derived::for_each_child((*this)[ref], [&](Ref child) { mark_live(child, live); });
}

YOSYS_NAMESPACE_END

#endif
