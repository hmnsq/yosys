#اگر_غیر_تعریف_شدہ YOSYS_HASHCONS_IMPL_H
#تعریف_کرو YOSYS_HASHCONS_IMPL_H

#شامل "kernel/hashcons.h"

#شامل <algorithm>

YOSYS_NAMESPACE_BEGIN

سانچہ<typename Derived, typename گرہ_2, typename Ref>
bool HashConsPool<Derived, گرہ_2, Ref>::is_زندہ(Ref ref) ثابت {
	Ref idx = ref.untag();
	اگر (is_static(idx))
		واپس true;
	سائز_t خانہ = idx.raw() - Derived::STATIC_گنتی;
	واپس خانہ < backing.سائز() && !backing[خانہ].is_dead();
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
خالی HashConsPool<Derived, گرہ_2, Ref>::check_ready() {}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
ثابت گرہ_2& HashConsPool<Derived, گرہ_2, Ref>::عامل[] (Ref ref) ثابت {
	Ref idx = ref.untag();
	اگر constexpr (Derived::STATIC_گنتی != 0) {
		اگر (is_static(idx))
			واپس Derived::static_گرہ(idx.raw());
	}
	واپس backing[idx.raw() - Derived::STATIC_گنتی];
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
Derived* HashConsPool<Derived, گرہ_2, Ref>::self() { واپس static_cast<Derived*>(this); }

سانچہ<typename Derived, typename گرہ_2, typename Ref>
ثابت Derived* HashConsPool<Derived, گرہ_2, Ref>::self() ثابت { واپس static_cast<ثابت Derived*>(this); }

سانچہ<typename Derived, typename گرہ_2, typename Ref>
bool HashConsPool<Derived, گرہ_2, Ref>::is_static(Ref ref) {
	اگر constexpr (Derived::STATIC_گنتی == 0)
		واپس false;
	ورنہ
		واپس ref.raw() < Derived::STATIC_گنتی;
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
خالی HashConsPool<Derived, گرہ_2, Ref>::RefIterator::settle() {
	جب (skip_dead && idx < stop && !pool->خانہ_زندہ(idx))
		idx++;
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
Ref HashConsPool<Derived, گرہ_2, Ref>::RefIterator::عامل*() ثابت { واپس Ref(idx); }

سانچہ<typename Derived, typename گرہ_2, typename Ref>
خودکار HashConsPool<Derived, گرہ_2, Ref>::RefIterator::عامل++() -> RefIterator& { idx++; settle(); واپس *this; }

سانچہ<typename Derived, typename گرہ_2, typename Ref>
bool HashConsPool<Derived, گرہ_2, Ref>::RefIterator::عامل!=(ثابت RefIterator& other) ثابت { واپس idx != other.idx; }

سانچہ<typename Derived, typename گرہ_2, typename Ref>
خودکار HashConsPool<Derived, گرہ_2, Ref>::RefRange::begin() ثابت -> RefIterator {
	RefIterator it{pool, پہلا, stop, skip_dead};
	it.settle();
	واپس it;
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
خودکار HashConsPool<Derived, گرہ_2, Ref>::RefRange::انتہا() ثابت -> RefIterator { واپس RefIterator{pool, stop, stop, skip_dead}; }

سانچہ<typename Derived, typename گرہ_2, typename Ref>
خودکار HashConsPool<Derived, گرہ_2, Ref>::refs(bool include_statics) ثابت -> RefRange {
	واپس RefRange{this, include_statics ? 0 : Derived::STATIC_گنتی,
			Derived::STATIC_گنتی + backing.سائز(), true};
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
خودکار HashConsPool<Derived, گرہ_2, Ref>::slots() ثابت -> RefRange {
	واپس RefRange{this, Derived::STATIC_گنتی,
			Derived::STATIC_گنتی + backing.سائز(), false};
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
سائز_t HashConsPool<Derived, گرہ_2, Ref>::home_خانہ(uint64_t hash) ثابت {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	واپس (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(جدول_2.سائز()));
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
سائز_t HashConsPool<Derived, گرہ_2, Ref>::اگلا_خانہ(سائز_t خانہ) ثابت { واپس (خانہ + 1) & (جدول_2.سائز() - 1); }

سانچہ<typename Derived, typename گرہ_2, typename Ref>
سائز_t HashConsPool<Derived, گرہ_2, Ref>::سائز() ثابت { واپس backing.سائز() - free_list.سائز(); }

سانچہ<typename Derived, typename گرہ_2, typename Ref>
bool HashConsPool<Derived, گرہ_2, Ref>::خانہ_زندہ(سائز_t مطلق_قدر) ثابت {
	اگر (مطلق_قدر < Derived::STATIC_گنتی)
		واپس true;
	واپس !backing[مطلق_قدر - Derived::STATIC_گنتی].is_dead();
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
HashConsPool<Derived, گرہ_2, Ref>::HashConsPool() { rebuild_اشاریہ(); }

سانچہ<typename Derived, typename گرہ_2, typename Ref>
HashConsPool<Derived, گرہ_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), جدول_2(std::move(other.جدول_2)), free_list(std::move(other.free_list)) {
	other.reset();
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
HashConsPool<Derived, گرہ_2, Ref>& HashConsPool<Derived, گرہ_2, Ref>::عامل=(HashConsPool&& other) {
	اگر (this != &other) {
		backing = std::move(other.backing);
		جدول_2 = std::move(other.جدول_2);
		free_list = std::move(other.free_list);
		other.reset();
	}
	واپس *this;
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
خالی HashConsPool<Derived, گرہ_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_اشاریہ();
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
خالی HashConsPool<Derived, گرہ_2, Ref>::rebuild_اشاریہ() {
	Derived::check_ready();
	free_list.clear();
	برائے (سائز_t idx = 0; idx < backing.سائز(); ++idx)
		اگر (backing[idx].is_dead())
			free_list.ڈالو_عقب(idx);
	std::ترتیب_دو(free_list.begin(), free_list.انتہا(), std::greater<سائز_t>());
	جدول_2.تفویض(std::ثنائی_ہندسہ_ceil((Derived::STATIC_گنتی + سائز()) * 2 + 2), Ref());
	برائے (Ref ref : refs())
		اشاریہ_داخل_کرو(ref);
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
سانچہ<typename Eq>
Ref HashConsPool<Derived, گرہ_2, Ref>::ڈھونڈو_hashed(uint64_t hash, Eq&& eq) ثابت {
	برائے (سائز_t خانہ = home_خانہ(hash); جدول_2[خانہ] != Ref(); خانہ = اگلا_خانہ(خانہ)) {
		Ref ref = جدول_2[خانہ];
		اگر (Derived::hash_گرہ((*this)[ref]) == hash && eq(ref))
			واپس ref;
	}
	واپس Ref();
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
خالی HashConsPool<Derived, گرہ_2, Ref>::اشاریہ_داخل_کرو(Ref ref) {
	سائز_t خانہ = home_خانہ(Derived::hash_گرہ((*this)[ref]));
	جب (جدول_2[خانہ] != Ref())
		خانہ = اگلا_خانہ(خانہ);
	جدول_2[خانہ] = ref;
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
خالی HashConsPool<Derived, گرہ_2, Ref>::اشاریہ_erase(Ref ref) {
	سائز_t hole = home_خانہ(Derived::hash_گرہ((*this)[ref]));
	جب (جدول_2[hole] != ref)
		hole = اگلا_خانہ(hole);
	سائز_t نقاب = جدول_2.سائز() - 1;
	برائے (سائز_t خانہ = اگلا_خانہ(hole); جدول_2[خانہ] != Ref(); خانہ = اگلا_خانہ(خانہ)) {
		سائز_t home = home_خانہ(Derived::hash_گرہ((*this)[جدول_2[خانہ]]));
		اگر (((خانہ - home) & نقاب) >= ((خانہ - hole) & نقاب)) {
			جدول_2[hole] = جدول_2[خانہ];
			hole = خانہ;
		}
	}
	جدول_2[hole] = Ref();
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
خالی HashConsPool<Derived, گرہ_2, Ref>::grow_اشاریہ() {
	std::سمتیہ<Ref> old(جدول_2.سائز() * 2, Ref());
	std::تبادلہ(old, جدول_2);
	برائے (Ref ref : old)
		اگر (ref != Ref())
			اشاریہ_داخل_کرو(ref);
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
Ref HashConsPool<Derived, گرہ_2, Ref>::جوڑو_inner(گرہ_2 t) {
	Ref ref;
	اگر (!free_list.خالی_خانے()) {
		سائز_t idx = free_list.عقب();
		free_list.نکالو_عقب();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_گنتی + idx);
	} ورنہ {
		ref = Ref(Derived::STATIC_گنتی + backing.سائز());
		backing.ڈالو_عقب(std::move(t));
	}
	اگر ((Derived::STATIC_گنتی + سائز()) * 3 > جدول_2.سائز() * 2)
		grow_اشاریہ();
	اشاریہ_داخل_کرو(ref);
	اگر (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	واپس ref;
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
سانچہ<typename Roots>
سائز_t HashConsPool<Derived, گرہ_2, Ref>::gc(ثابت Roots& roots) {
	pool<Ref> زندہ;
	برائے (Ref ref : roots)
		mark_زندہ(ref, زندہ);
	سائز_t erased = 0;
	برائے (سائز_t idx = 0; idx < backing.سائز(); ++idx) {
		اگر (backing[idx].is_dead())
			جاری;
		اگر (!زندہ.گنتی(Ref(Derived::STATIC_گنتی + idx))) {
			اشاریہ_erase(Ref(Derived::STATIC_گنتی + idx));
			free_list.ڈالو_عقب(idx);
			backing[idx] = گرہ_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::ترتیب_دو(free_list.begin(), free_list.انتہا(), std::greater<سائز_t>());
	واپس erased;
}

سانچہ<typename Derived, typename گرہ_2, typename Ref>
خالی HashConsPool<Derived, گرہ_2, Ref>::mark_زندہ(Ref ref, pool<Ref>& زندہ) ثابت {
	ref = ref.untag();
	اگر (ref == Ref() || is_static(ref) || !زندہ.داخل_کرو(ref).دوسرا)
		واپس;
	Derived::for_each_بچہ((*this)[ref], [&](Ref بچہ) { mark_زندہ(بچہ, زندہ); });
}

YOSYS_NAMESPACE_انتہا

#اگر_ختم
