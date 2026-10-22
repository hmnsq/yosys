#که_نه_تعریف_شوی YOSYS_HASHCONS_IMPL_H
#تعریف_کړه YOSYS_HASHCONS_IMPL_H

#ضمن "kernel/hashcons.h"

#ضمن <algorithm>

YOSYS_NAMESPACE_BEGIN

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
bool HashConsPool<Derived, غوټه_2, Ref>::is_ژوندی(Ref ref) ثابت {
	Ref idx = ref.untag();
	إذا (is_static(idx))
		أرجع true;
	اندازه_t خونه = idx.raw() - Derived::STATIC_شمېر;
	أرجع خونه < backing.اندازه() && !backing[خونه].is_dead();
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
فراغ HashConsPool<Derived, غوټه_2, Ref>::check_ready() {}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
ثابت غوټه_2& HashConsPool<Derived, غوټه_2, Ref>::چلوونکی[] (Ref ref) ثابت {
	Ref idx = ref.untag();
	إذا constexpr (Derived::STATIC_شمېر != 0) {
		إذا (is_static(idx))
			أرجع Derived::static_غوټه(idx.raw());
	}
	أرجع backing[idx.raw() - Derived::STATIC_شمېر];
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
Derived* HashConsPool<Derived, غوټه_2, Ref>::self() { أرجع static_cast<Derived*>(this); }

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
ثابت Derived* HashConsPool<Derived, غوټه_2, Ref>::self() ثابت { أرجع static_cast<ثابت Derived*>(this); }

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
bool HashConsPool<Derived, غوټه_2, Ref>::is_static(Ref ref) {
	إذا constexpr (Derived::STATIC_شمېر == 0)
		أرجع false;
	وإلا
		أرجع ref.raw() < Derived::STATIC_شمېر;
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
فراغ HashConsPool<Derived, غوټه_2, Ref>::RefIterator::settle() {
	طالما (skip_dead && idx < stop && !pool->خونه_ژوندی(idx))
		idx++;
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
Ref HashConsPool<Derived, غوټه_2, Ref>::RefIterator::چلوونکی*() ثابت { أرجع Ref(idx); }

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
خپلکاری HashConsPool<Derived, غوټه_2, Ref>::RefIterator::چلوونکی++() -> RefIterator& { idx++; settle(); أرجع *this; }

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
bool HashConsPool<Derived, غوټه_2, Ref>::RefIterator::چلوونکی!=(ثابت RefIterator& other) ثابت { أرجع idx != other.idx; }

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
خپلکاری HashConsPool<Derived, غوټه_2, Ref>::RefRange::begin() ثابت -> RefIterator {
	RefIterator it{pool, لومړی, stop, skip_dead};
	it.settle();
	أرجع it;
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
خپلکاری HashConsPool<Derived, غوټه_2, Ref>::RefRange::پای_ټکی() ثابت -> RefIterator { أرجع RefIterator{pool, stop, stop, skip_dead}; }

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
خپلکاری HashConsPool<Derived, غوټه_2, Ref>::refs(bool include_statics) ثابت -> RefRange {
	أرجع RefRange{this, include_statics ? 0 : Derived::STATIC_شمېر,
			Derived::STATIC_شمېر + backing.اندازه(), true};
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
خپلکاری HashConsPool<Derived, غوټه_2, Ref>::slots() ثابت -> RefRange {
	أرجع RefRange{this, Derived::STATIC_شمېر,
			Derived::STATIC_شمېر + backing.اندازه(), false};
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
اندازه_t HashConsPool<Derived, غوټه_2, Ref>::home_خونه(uint64_t hash) ثابت {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	أرجع (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(جدول_2.اندازه()));
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
اندازه_t HashConsPool<Derived, غوټه_2, Ref>::بل_خونه(اندازه_t خونه) ثابت { أرجع (خونه + 1) & (جدول_2.اندازه() - 1); }

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
اندازه_t HashConsPool<Derived, غوټه_2, Ref>::اندازه() ثابت { أرجع backing.اندازه() - free_list.اندازه(); }

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
bool HashConsPool<Derived, غوټه_2, Ref>::خونه_ژوندی(اندازه_t مطلق_قیمت) ثابت {
	إذا (مطلق_قیمت < Derived::STATIC_شمېر)
		أرجع true;
	أرجع !backing[مطلق_قیمت - Derived::STATIC_شمېر].is_dead();
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
HashConsPool<Derived, غوټه_2, Ref>::HashConsPool() { rebuild_شاخص(); }

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
HashConsPool<Derived, غوټه_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), جدول_2(std::move(other.جدول_2)), free_list(std::move(other.free_list)) {
	other.reset();
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
HashConsPool<Derived, غوټه_2, Ref>& HashConsPool<Derived, غوټه_2, Ref>::چلوونکی=(HashConsPool&& other) {
	إذا (this != &other) {
		backing = std::move(other.backing);
		جدول_2 = std::move(other.جدول_2);
		free_list = std::move(other.free_list);
		other.reset();
	}
	أرجع *this;
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
فراغ HashConsPool<Derived, غوټه_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_شاخص();
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
فراغ HashConsPool<Derived, غوټه_2, Ref>::rebuild_شاخص() {
	Derived::check_ready();
	free_list.clear();
	لكل (اندازه_t idx = 0; idx < backing.اندازه(); ++idx)
		إذا (backing[idx].is_dead())
			free_list.ټېل_شا(idx);
	std::ترتیبول(free_list.begin(), free_list.پای_ټکی(), std::greater<اندازه_t>());
	جدول_2.ټاکل(std::بټ_ceil((Derived::STATIC_شمېر + اندازه()) * 2 + 2), Ref());
	لكل (Ref ref : refs())
		شاخص_ورننباسه(ref);
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
کينډۍ<typename Eq>
Ref HashConsPool<Derived, غوټه_2, Ref>::پیدا_کړه_hashed(uint64_t hash, Eq&& eq) ثابت {
	لكل (اندازه_t خونه = home_خونه(hash); جدول_2[خونه] != Ref(); خونه = بل_خونه(خونه)) {
		Ref ref = جدول_2[خونه];
		إذا (Derived::hash_غوټه((*this)[ref]) == hash && eq(ref))
			أرجع ref;
	}
	أرجع Ref();
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
فراغ HashConsPool<Derived, غوټه_2, Ref>::شاخص_ورننباسه(Ref ref) {
	اندازه_t خونه = home_خونه(Derived::hash_غوټه((*this)[ref]));
	طالما (جدول_2[خونه] != Ref())
		خونه = بل_خونه(خونه);
	جدول_2[خونه] = ref;
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
فراغ HashConsPool<Derived, غوټه_2, Ref>::شاخص_erase(Ref ref) {
	اندازه_t hole = home_خونه(Derived::hash_غوټه((*this)[ref]));
	طالما (جدول_2[hole] != ref)
		hole = بل_خونه(hole);
	اندازه_t ماسک = جدول_2.اندازه() - 1;
	لكل (اندازه_t خونه = بل_خونه(hole); جدول_2[خونه] != Ref(); خونه = بل_خونه(خونه)) {
		اندازه_t home = home_خونه(Derived::hash_غوټه((*this)[جدول_2[خونه]]));
		إذا (((خونه - home) & ماسک) >= ((خونه - hole) & ماسک)) {
			جدول_2[hole] = جدول_2[خونه];
			hole = خونه;
		}
	}
	جدول_2[hole] = Ref();
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
فراغ HashConsPool<Derived, غوټه_2, Ref>::grow_شاخص() {
	std::ویکټور<Ref> old(جدول_2.اندازه() * 2, Ref());
	std::بدلون(old, جدول_2);
	لكل (Ref ref : old)
		إذا (ref != Ref())
			شاخص_ورننباسه(ref);
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
Ref HashConsPool<Derived, غوټه_2, Ref>::جمع_کړه_inner(غوټه_2 t) {
	Ref ref;
	إذا (!free_list.تشه()) {
		اندازه_t idx = free_list.شا();
		free_list.راوباسه_شا();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_شمېر + idx);
	} وإلا {
		ref = Ref(Derived::STATIC_شمېر + backing.اندازه());
		backing.ټېل_شا(std::move(t));
	}
	إذا ((Derived::STATIC_شمېر + اندازه()) * 3 > جدول_2.اندازه() * 2)
		grow_شاخص();
	شاخص_ورننباسه(ref);
	إذا (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	أرجع ref;
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
کينډۍ<typename Roots>
اندازه_t HashConsPool<Derived, غوټه_2, Ref>::gc(ثابت Roots& roots) {
	pool<Ref> ژوندی;
	لكل (Ref ref : roots)
		mark_ژوندی(ref, ژوندی);
	اندازه_t erased = 0;
	لكل (اندازه_t idx = 0; idx < backing.اندازه(); ++idx) {
		إذا (backing[idx].is_dead())
			استمر;
		إذا (!ژوندی.شمېر(Ref(Derived::STATIC_شمېر + idx))) {
			شاخص_erase(Ref(Derived::STATIC_شمېر + idx));
			free_list.ټېل_شا(idx);
			backing[idx] = غوټه_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::ترتیبول(free_list.begin(), free_list.پای_ټکی(), std::greater<اندازه_t>());
	أرجع erased;
}

کينډۍ<typename Derived, typename غوټه_2, typename Ref>
فراغ HashConsPool<Derived, غوټه_2, Ref>::mark_ژوندی(Ref ref, pool<Ref>& ژوندی) ثابت {
	ref = ref.untag();
	إذا (ref == Ref() || is_static(ref) || !ژوندی.ورننباسه(ref).دوهم)
		أرجع;
	Derived::for_each_ماشوم((*this)[ref], [&](Ref ماشوم) { mark_ژوندی(ماشوم, ژوندی); });
}

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
