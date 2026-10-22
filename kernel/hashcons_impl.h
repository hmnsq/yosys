#اگر_تعریف_نشده YOSYS_HASHCONS_IMPL_H
#تعریف_کن YOSYS_HASHCONS_IMPL_H

#ضمن "kernel/hashcons.h"

#ضمن <algorithm>

YOSYS_NAMESPACE_BEGIN

قالب<typename Derived, typename گره_2, typename Ref>
bool HashConsPool<Derived, گره_2, Ref>::is_زنده(Ref ref) ثابت {
	Ref idx = ref.untag();
	إذا (is_static(idx))
		أرجع true;
	اندازه_t جایگاه = idx.raw() - Derived::STATIC_شمار;
	أرجع جایگاه < backing.اندازه() && !backing[جایگاه].is_dead();
}

قالب<typename Derived, typename گره_2, typename Ref>
فراغ HashConsPool<Derived, گره_2, Ref>::check_ready() {}

قالب<typename Derived, typename گره_2, typename Ref>
ثابت گره_2& HashConsPool<Derived, گره_2, Ref>::عملگر[] (Ref ref) ثابت {
	Ref idx = ref.untag();
	إذا constexpr (Derived::STATIC_شمار != 0) {
		إذا (is_static(idx))
			أرجع Derived::static_گره(idx.raw());
	}
	أرجع backing[idx.raw() - Derived::STATIC_شمار];
}

قالب<typename Derived, typename گره_2, typename Ref>
Derived* HashConsPool<Derived, گره_2, Ref>::self() { أرجع static_cast<Derived*>(this); }

قالب<typename Derived, typename گره_2, typename Ref>
ثابت Derived* HashConsPool<Derived, گره_2, Ref>::self() ثابت { أرجع static_cast<ثابت Derived*>(this); }

قالب<typename Derived, typename گره_2, typename Ref>
bool HashConsPool<Derived, گره_2, Ref>::is_static(Ref ref) {
	إذا constexpr (Derived::STATIC_شمار == 0)
		أرجع false;
	وإلا
		أرجع ref.raw() < Derived::STATIC_شمار;
}

قالب<typename Derived, typename گره_2, typename Ref>
فراغ HashConsPool<Derived, گره_2, Ref>::RefIterator::settle() {
	طالما (skip_dead && idx < stop && !pool->جایگاه_زنده(idx))
		idx++;
}

قالب<typename Derived, typename گره_2, typename Ref>
Ref HashConsPool<Derived, گره_2, Ref>::RefIterator::عملگر*() ثابت { أرجع Ref(idx); }

قالب<typename Derived, typename گره_2, typename Ref>
خودکار HashConsPool<Derived, گره_2, Ref>::RefIterator::عملگر++() -> RefIterator& { idx++; settle(); أرجع *this; }

قالب<typename Derived, typename گره_2, typename Ref>
bool HashConsPool<Derived, گره_2, Ref>::RefIterator::عملگر!=(ثابت RefIterator& other) ثابت { أرجع idx != other.idx; }

قالب<typename Derived, typename گره_2, typename Ref>
خودکار HashConsPool<Derived, گره_2, Ref>::RefRange::begin() ثابت -> RefIterator {
	RefIterator it{pool, اولی, stop, skip_dead};
	it.settle();
	أرجع it;
}

قالب<typename Derived, typename گره_2, typename Ref>
خودکار HashConsPool<Derived, گره_2, Ref>::RefRange::انتها() ثابت -> RefIterator { أرجع RefIterator{pool, stop, stop, skip_dead}; }

قالب<typename Derived, typename گره_2, typename Ref>
خودکار HashConsPool<Derived, گره_2, Ref>::refs(bool include_statics) ثابت -> RefRange {
	أرجع RefRange{this, include_statics ? 0 : Derived::STATIC_شمار,
			Derived::STATIC_شمار + backing.اندازه(), true};
}

قالب<typename Derived, typename گره_2, typename Ref>
خودکار HashConsPool<Derived, گره_2, Ref>::slots() ثابت -> RefRange {
	أرجع RefRange{this, Derived::STATIC_شمار,
			Derived::STATIC_شمار + backing.اندازه(), false};
}

قالب<typename Derived, typename گره_2, typename Ref>
اندازه_t HashConsPool<Derived, گره_2, Ref>::home_جایگاه(uint64_t hash) ثابت {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	أرجع (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(جدول_2.اندازه()));
}

قالب<typename Derived, typename گره_2, typename Ref>
اندازه_t HashConsPool<Derived, گره_2, Ref>::بعدی_جایگاه(اندازه_t جایگاه) ثابت { أرجع (جایگاه + 1) & (جدول_2.اندازه() - 1); }

قالب<typename Derived, typename گره_2, typename Ref>
اندازه_t HashConsPool<Derived, گره_2, Ref>::اندازه() ثابت { أرجع backing.اندازه() - free_list.اندازه(); }

قالب<typename Derived, typename گره_2, typename Ref>
bool HashConsPool<Derived, گره_2, Ref>::جایگاه_زنده(اندازه_t قدر_مطلق) ثابت {
	إذا (قدر_مطلق < Derived::STATIC_شمار)
		أرجع true;
	أرجع !backing[قدر_مطلق - Derived::STATIC_شمار].is_dead();
}

قالب<typename Derived, typename گره_2, typename Ref>
HashConsPool<Derived, گره_2, Ref>::HashConsPool() { rebuild_شاخص(); }

قالب<typename Derived, typename گره_2, typename Ref>
HashConsPool<Derived, گره_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), جدول_2(std::move(other.جدول_2)), free_list(std::move(other.free_list)) {
	other.reset();
}

قالب<typename Derived, typename گره_2, typename Ref>
HashConsPool<Derived, گره_2, Ref>& HashConsPool<Derived, گره_2, Ref>::عملگر=(HashConsPool&& other) {
	إذا (this != &other) {
		backing = std::move(other.backing);
		جدول_2 = std::move(other.جدول_2);
		free_list = std::move(other.free_list);
		other.reset();
	}
	أرجع *this;
}

قالب<typename Derived, typename گره_2, typename Ref>
فراغ HashConsPool<Derived, گره_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_شاخص();
}

قالب<typename Derived, typename گره_2, typename Ref>
فراغ HashConsPool<Derived, گره_2, Ref>::rebuild_شاخص() {
	Derived::check_ready();
	free_list.clear();
	لكل (اندازه_t idx = 0; idx < backing.اندازه(); ++idx)
		إذا (backing[idx].is_dead())
			free_list.افزودن_پشت(idx);
	std::مرتب_سازی(free_list.begin(), free_list.انتها(), std::greater<اندازه_t>());
	جدول_2.تخصیص_بده(std::بیت_ceil((Derived::STATIC_شمار + اندازه()) * 2 + 2), Ref());
	لكل (Ref ref : refs())
		شاخص_درج(ref);
}

قالب<typename Derived, typename گره_2, typename Ref>
قالب<typename Eq>
Ref HashConsPool<Derived, گره_2, Ref>::پیدا_کن_hashed(uint64_t hash, Eq&& eq) ثابت {
	لكل (اندازه_t جایگاه = home_جایگاه(hash); جدول_2[جایگاه] != Ref(); جایگاه = بعدی_جایگاه(جایگاه)) {
		Ref ref = جدول_2[جایگاه];
		إذا (Derived::hash_گره((*this)[ref]) == hash && eq(ref))
			أرجع ref;
	}
	أرجع Ref();
}

قالب<typename Derived, typename گره_2, typename Ref>
فراغ HashConsPool<Derived, گره_2, Ref>::شاخص_درج(Ref ref) {
	اندازه_t جایگاه = home_جایگاه(Derived::hash_گره((*this)[ref]));
	طالما (جدول_2[جایگاه] != Ref())
		جایگاه = بعدی_جایگاه(جایگاه);
	جدول_2[جایگاه] = ref;
}

قالب<typename Derived, typename گره_2, typename Ref>
فراغ HashConsPool<Derived, گره_2, Ref>::شاخص_erase(Ref ref) {
	اندازه_t hole = home_جایگاه(Derived::hash_گره((*this)[ref]));
	طالما (جدول_2[hole] != ref)
		hole = بعدی_جایگاه(hole);
	اندازه_t ماسک = جدول_2.اندازه() - 1;
	لكل (اندازه_t جایگاه = بعدی_جایگاه(hole); جدول_2[جایگاه] != Ref(); جایگاه = بعدی_جایگاه(جایگاه)) {
		اندازه_t home = home_جایگاه(Derived::hash_گره((*this)[جدول_2[جایگاه]]));
		إذا (((جایگاه - home) & ماسک) >= ((جایگاه - hole) & ماسک)) {
			جدول_2[hole] = جدول_2[جایگاه];
			hole = جایگاه;
		}
	}
	جدول_2[hole] = Ref();
}

قالب<typename Derived, typename گره_2, typename Ref>
فراغ HashConsPool<Derived, گره_2, Ref>::grow_شاخص() {
	std::وکتور<Ref> old(جدول_2.اندازه() * 2, Ref());
	std::جابجا(old, جدول_2);
	لكل (Ref ref : old)
		إذا (ref != Ref())
			شاخص_درج(ref);
}

قالب<typename Derived, typename گره_2, typename Ref>
Ref HashConsPool<Derived, گره_2, Ref>::جمع_کن_inner(گره_2 t) {
	Ref ref;
	إذا (!free_list.خالی()) {
		اندازه_t idx = free_list.پشت();
		free_list.برداشت_پشت();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_شمار + idx);
	} وإلا {
		ref = Ref(Derived::STATIC_شمار + backing.اندازه());
		backing.افزودن_پشت(std::move(t));
	}
	إذا ((Derived::STATIC_شمار + اندازه()) * 3 > جدول_2.اندازه() * 2)
		grow_شاخص();
	شاخص_درج(ref);
	إذا (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	أرجع ref;
}

قالب<typename Derived, typename گره_2, typename Ref>
قالب<typename Roots>
اندازه_t HashConsPool<Derived, گره_2, Ref>::gc(ثابت Roots& roots) {
	pool<Ref> زنده;
	لكل (Ref ref : roots)
		mark_زنده(ref, زنده);
	اندازه_t erased = 0;
	لكل (اندازه_t idx = 0; idx < backing.اندازه(); ++idx) {
		إذا (backing[idx].is_dead())
			استمر;
		إذا (!زنده.شمار(Ref(Derived::STATIC_شمار + idx))) {
			شاخص_erase(Ref(Derived::STATIC_شمار + idx));
			free_list.افزودن_پشت(idx);
			backing[idx] = گره_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::مرتب_سازی(free_list.begin(), free_list.انتها(), std::greater<اندازه_t>());
	أرجع erased;
}

قالب<typename Derived, typename گره_2, typename Ref>
فراغ HashConsPool<Derived, گره_2, Ref>::mark_زنده(Ref ref, pool<Ref>& زنده) ثابت {
	ref = ref.untag();
	إذا (ref == Ref() || is_static(ref) || !زنده.درج(ref).دومی)
		أرجع;
	Derived::for_each_فرزند((*this)[ref], [&](Ref فرزند) { mark_زنده(فرزند, زنده); });
}

YOSYS_NAMESPACE_انتها

#پایان_اگر
