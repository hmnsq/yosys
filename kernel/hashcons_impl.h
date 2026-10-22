#جے_بنا_تعریف YOSYS_HASHCONS_IMPL_H
#تعریف_کرو YOSYS_HASHCONS_IMPL_H

#ضمن "kernel/hashcons.h"

#ضمن <algorithm>

YOSYS_NAMESPACE_BEGIN

سانچا<typename Derived, typename گرہ_2, typename Ref>
bool HashConsPool<Derived, گرہ_2, Ref>::is_جیوندا(Ref ref) ثابت {
	Ref idx = ref.untag();
	إذا (is_static(idx))
		أرجع true;
	ناپ_t تھاں = idx.raw() - Derived::STATIC_گنتی;
	أرجع تھاں < backing.ناپ() && !backing[تھاں].is_dead();
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
فراغ HashConsPool<Derived, گرہ_2, Ref>::check_ready() {}

سانچا<typename Derived, typename گرہ_2, typename Ref>
ثابت گرہ_2& HashConsPool<Derived, گرہ_2, Ref>::عامل[] (Ref ref) ثابت {
	Ref idx = ref.untag();
	إذا constexpr (Derived::STATIC_گنتی != 0) {
		إذا (is_static(idx))
			أرجع Derived::static_گرہ(idx.raw());
	}
	أرجع backing[idx.raw() - Derived::STATIC_گنتی];
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
Derived* HashConsPool<Derived, گرہ_2, Ref>::self() { أرجع static_cast<Derived*>(this); }

سانچا<typename Derived, typename گرہ_2, typename Ref>
ثابت Derived* HashConsPool<Derived, گرہ_2, Ref>::self() ثابت { أرجع static_cast<ثابت Derived*>(this); }

سانچا<typename Derived, typename گرہ_2, typename Ref>
bool HashConsPool<Derived, گرہ_2, Ref>::is_static(Ref ref) {
	إذا constexpr (Derived::STATIC_گنتی == 0)
		أرجع false;
	وإلا
		أرجع ref.raw() < Derived::STATIC_گنتی;
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
فراغ HashConsPool<Derived, گرہ_2, Ref>::RefIterator::settle() {
	طالما (skip_dead && idx < stop && !pool->تھاں_جیوندا(idx))
		idx++;
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
Ref HashConsPool<Derived, گرہ_2, Ref>::RefIterator::عامل*() ثابت { أرجع Ref(idx); }

سانچا<typename Derived, typename گرہ_2, typename Ref>
خودکار HashConsPool<Derived, گرہ_2, Ref>::RefIterator::عامل++() -> RefIterator& { idx++; settle(); أرجع *this; }

سانچا<typename Derived, typename گرہ_2, typename Ref>
bool HashConsPool<Derived, گرہ_2, Ref>::RefIterator::عامل!=(ثابت RefIterator& other) ثابت { أرجع idx != other.idx; }

سانچا<typename Derived, typename گرہ_2, typename Ref>
خودکار HashConsPool<Derived, گرہ_2, Ref>::RefRange::begin() ثابت -> RefIterator {
	RefIterator it{pool, پہلا, stop, skip_dead};
	it.settle();
	أرجع it;
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
خودکار HashConsPool<Derived, گرہ_2, Ref>::RefRange::سرا() ثابت -> RefIterator { أرجع RefIterator{pool, stop, stop, skip_dead}; }

سانچا<typename Derived, typename گرہ_2, typename Ref>
خودکار HashConsPool<Derived, گرہ_2, Ref>::refs(bool include_statics) ثابت -> RefRange {
	أرجع RefRange{this, include_statics ? 0 : Derived::STATIC_گنتی,
			Derived::STATIC_گنتی + backing.ناپ(), true};
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
خودکار HashConsPool<Derived, گرہ_2, Ref>::slots() ثابت -> RefRange {
	أرجع RefRange{this, Derived::STATIC_گنتی,
			Derived::STATIC_گنتی + backing.ناپ(), false};
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
ناپ_t HashConsPool<Derived, گرہ_2, Ref>::home_تھاں(uint64_t hash) ثابت {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	أرجع (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(جدول_2.ناپ()));
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
ناپ_t HashConsPool<Derived, گرہ_2, Ref>::اگلا_تھاں(ناپ_t تھاں) ثابت { أرجع (تھاں + 1) & (جدول_2.ناپ() - 1); }

سانچا<typename Derived, typename گرہ_2, typename Ref>
ناپ_t HashConsPool<Derived, گرہ_2, Ref>::ناپ() ثابت { أرجع backing.ناپ() - free_list.ناپ(); }

سانچا<typename Derived, typename گرہ_2, typename Ref>
bool HashConsPool<Derived, گرہ_2, Ref>::تھاں_جیوندا(ناپ_t مطلق_قدر) ثابت {
	إذا (مطلق_قدر < Derived::STATIC_گنتی)
		أرجع true;
	أرجع !backing[مطلق_قدر - Derived::STATIC_گنتی].is_dead();
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
HashConsPool<Derived, گرہ_2, Ref>::HashConsPool() { rebuild_اشاریہ(); }

سانچا<typename Derived, typename گرہ_2, typename Ref>
HashConsPool<Derived, گرہ_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), جدول_2(std::move(other.جدول_2)), free_list(std::move(other.free_list)) {
	other.reset();
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
HashConsPool<Derived, گرہ_2, Ref>& HashConsPool<Derived, گرہ_2, Ref>::عامل=(HashConsPool&& other) {
	إذا (this != &other) {
		backing = std::move(other.backing);
		جدول_2 = std::move(other.جدول_2);
		free_list = std::move(other.free_list);
		other.reset();
	}
	أرجع *this;
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
فراغ HashConsPool<Derived, گرہ_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_اشاریہ();
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
فراغ HashConsPool<Derived, گرہ_2, Ref>::rebuild_اشاریہ() {
	Derived::check_ready();
	free_list.clear();
	لكل (ناپ_t idx = 0; idx < backing.ناپ(); ++idx)
		إذا (backing[idx].is_dead())
			free_list.ڈالو_پٹھ(idx);
	std::ترتیب_دیو(free_list.begin(), free_list.سرا(), std::greater<ناپ_t>());
	جدول_2.سونپو(std::ثنائی_ہندسہ_ceil((Derived::STATIC_گنتی + ناپ()) * 2 + 2), Ref());
	لكل (Ref ref : refs())
		اشاریہ_داخل_کرو(ref);
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
سانچا<typename Eq>
Ref HashConsPool<Derived, گرہ_2, Ref>::لبھو_hashed(uint64_t hash, Eq&& eq) ثابت {
	لكل (ناپ_t تھاں = home_تھاں(hash); جدول_2[تھاں] != Ref(); تھاں = اگلا_تھاں(تھاں)) {
		Ref ref = جدول_2[تھاں];
		إذا (Derived::hash_گرہ((*this)[ref]) == hash && eq(ref))
			أرجع ref;
	}
	أرجع Ref();
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
فراغ HashConsPool<Derived, گرہ_2, Ref>::اشاریہ_داخل_کرو(Ref ref) {
	ناپ_t تھاں = home_تھاں(Derived::hash_گرہ((*this)[ref]));
	طالما (جدول_2[تھاں] != Ref())
		تھاں = اگلا_تھاں(تھاں);
	جدول_2[تھاں] = ref;
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
فراغ HashConsPool<Derived, گرہ_2, Ref>::اشاریہ_erase(Ref ref) {
	ناپ_t hole = home_تھاں(Derived::hash_گرہ((*this)[ref]));
	طالما (جدول_2[hole] != ref)
		hole = اگلا_تھاں(hole);
	ناپ_t نقاب = جدول_2.ناپ() - 1;
	لكل (ناپ_t تھاں = اگلا_تھاں(hole); جدول_2[تھاں] != Ref(); تھاں = اگلا_تھاں(تھاں)) {
		ناپ_t home = home_تھاں(Derived::hash_گرہ((*this)[جدول_2[تھاں]]));
		إذا (((تھاں - home) & نقاب) >= ((تھاں - hole) & نقاب)) {
			جدول_2[hole] = جدول_2[تھاں];
			hole = تھاں;
		}
	}
	جدول_2[hole] = Ref();
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
فراغ HashConsPool<Derived, گرہ_2, Ref>::grow_اشاریہ() {
	std::سمتیہ<Ref> old(جدول_2.ناپ() * 2, Ref());
	std::وٹاندرا(old, جدول_2);
	لكل (Ref ref : old)
		إذا (ref != Ref())
			اشاریہ_داخل_کرو(ref);
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
Ref HashConsPool<Derived, گرہ_2, Ref>::جوڑو_inner(گرہ_2 t) {
	Ref ref;
	إذا (!free_list.خالی_تھاواں()) {
		ناپ_t idx = free_list.پٹھ();
		free_list.نکالو_پٹھ();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_گنتی + idx);
	} وإلا {
		ref = Ref(Derived::STATIC_گنتی + backing.ناپ());
		backing.ڈالو_پٹھ(std::move(t));
	}
	إذا ((Derived::STATIC_گنتی + ناپ()) * 3 > جدول_2.ناپ() * 2)
		grow_اشاریہ();
	اشاریہ_داخل_کرو(ref);
	إذا (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	أرجع ref;
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
سانچا<typename Roots>
ناپ_t HashConsPool<Derived, گرہ_2, Ref>::gc(ثابت Roots& roots) {
	pool<Ref> جیوندا;
	لكل (Ref ref : roots)
		mark_جیوندا(ref, جیوندا);
	ناپ_t erased = 0;
	لكل (ناپ_t idx = 0; idx < backing.ناپ(); ++idx) {
		إذا (backing[idx].is_dead())
			استمر;
		إذا (!جیوندا.گنتی(Ref(Derived::STATIC_گنتی + idx))) {
			اشاریہ_erase(Ref(Derived::STATIC_گنتی + idx));
			free_list.ڈالو_پٹھ(idx);
			backing[idx] = گرہ_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::ترتیب_دیو(free_list.begin(), free_list.سرا(), std::greater<ناپ_t>());
	أرجع erased;
}

سانچا<typename Derived, typename گرہ_2, typename Ref>
فراغ HashConsPool<Derived, گرہ_2, Ref>::mark_جیوندا(Ref ref, pool<Ref>& جیوندا) ثابت {
	ref = ref.untag();
	إذا (ref == Ref() || is_static(ref) || !جیوندا.داخل_کرو(ref).دوجا)
		أرجع;
	Derived::for_each_بچہ((*this)[ref], [&](Ref بچہ) { mark_جیوندا(بچہ, جیوندا); });
}

YOSYS_NAMESPACE_سرا

#جے_انت
