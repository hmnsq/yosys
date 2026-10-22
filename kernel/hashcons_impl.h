#جيڪڏهن_وصف_نه_ٿيل YOSYS_HASHCONS_IMPL_H
#وصف_ڪريو YOSYS_HASHCONS_IMPL_H

#ضمن "kernel/hashcons.h"

#ضمن <algorithm>

YOSYS_NAMESPACE_BEGIN

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
bool HashConsPool<Derived, ڳنڍ_2, Ref>::is_جيئرو(Ref ref) ثابت {
	Ref idx = ref.untag();
	إذا (is_static(idx))
		أرجع true;
	سائيز_t جڳهه = idx.raw() - Derived::STATIC_ڳڻپ;
	أرجع جڳهه < backing.سائيز() && !backing[جڳهه].is_dead();
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
فراغ HashConsPool<Derived, ڳنڍ_2, Ref>::check_ready() {}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
ثابت ڳنڍ_2& HashConsPool<Derived, ڳنڍ_2, Ref>::عامل[] (Ref ref) ثابت {
	Ref idx = ref.untag();
	إذا constexpr (Derived::STATIC_ڳڻپ != 0) {
		إذا (is_static(idx))
			أرجع Derived::static_ڳنڍ(idx.raw());
	}
	أرجع backing[idx.raw() - Derived::STATIC_ڳڻپ];
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
Derived* HashConsPool<Derived, ڳنڍ_2, Ref>::self() { أرجع static_cast<Derived*>(this); }

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
ثابت Derived* HashConsPool<Derived, ڳنڍ_2, Ref>::self() ثابت { أرجع static_cast<ثابت Derived*>(this); }

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
bool HashConsPool<Derived, ڳنڍ_2, Ref>::is_static(Ref ref) {
	إذا constexpr (Derived::STATIC_ڳڻپ == 0)
		أرجع false;
	وإلا
		أرجع ref.raw() < Derived::STATIC_ڳڻپ;
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
فراغ HashConsPool<Derived, ڳنڍ_2, Ref>::RefIterator::settle() {
	طالما (skip_dead && idx < stop && !pool->جڳهه_جيئرو(idx))
		idx++;
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
Ref HashConsPool<Derived, ڳنڍ_2, Ref>::RefIterator::عامل*() ثابت { أرجع Ref(idx); }

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
خودڪار HashConsPool<Derived, ڳنڍ_2, Ref>::RefIterator::عامل++() -> RefIterator& { idx++; settle(); أرجع *this; }

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
bool HashConsPool<Derived, ڳنڍ_2, Ref>::RefIterator::عامل!=(ثابت RefIterator& other) ثابت { أرجع idx != other.idx; }

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
خودڪار HashConsPool<Derived, ڳنڍ_2, Ref>::RefRange::begin() ثابت -> RefIterator {
	RefIterator it{pool, پهريون, stop, skip_dead};
	it.settle();
	أرجع it;
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
خودڪار HashConsPool<Derived, ڳنڍ_2, Ref>::RefRange::پڇاڙي() ثابت -> RefIterator { أرجع RefIterator{pool, stop, stop, skip_dead}; }

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
خودڪار HashConsPool<Derived, ڳنڍ_2, Ref>::refs(bool include_statics) ثابت -> RefRange {
	أرجع RefRange{this, include_statics ? 0 : Derived::STATIC_ڳڻپ,
			Derived::STATIC_ڳڻپ + backing.سائيز(), true};
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
خودڪار HashConsPool<Derived, ڳنڍ_2, Ref>::slots() ثابت -> RefRange {
	أرجع RefRange{this, Derived::STATIC_ڳڻپ,
			Derived::STATIC_ڳڻپ + backing.سائيز(), false};
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
سائيز_t HashConsPool<Derived, ڳنڍ_2, Ref>::home_جڳهه(uint64_t hash) ثابت {
	// Knuth's Fibonacci hashing saves us from how bad DJB2 is
	أرجع (hash * 0x9e3779b97f4a7c15ull) >> (64 - std::countr_zero(جدول_2.سائيز()));
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
سائيز_t HashConsPool<Derived, ڳنڍ_2, Ref>::ايندڙ_جڳهه(سائيز_t جڳهه) ثابت { أرجع (جڳهه + 1) & (جدول_2.سائيز() - 1); }

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
سائيز_t HashConsPool<Derived, ڳنڍ_2, Ref>::سائيز() ثابت { أرجع backing.سائيز() - free_list.سائيز(); }

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
bool HashConsPool<Derived, ڳنڍ_2, Ref>::جڳهه_جيئرو(سائيز_t مطلق_قدر) ثابت {
	إذا (مطلق_قدر < Derived::STATIC_ڳڻپ)
		أرجع true;
	أرجع !backing[مطلق_قدر - Derived::STATIC_ڳڻپ].is_dead();
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
HashConsPool<Derived, ڳنڍ_2, Ref>::HashConsPool() { rebuild_اشاريو(); }

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
HashConsPool<Derived, ڳنڍ_2, Ref>::HashConsPool(HashConsPool&& other)
	: backing(std::move(other.backing)), جدول_2(std::move(other.جدول_2)), free_list(std::move(other.free_list)) {
	other.reset();
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
HashConsPool<Derived, ڳنڍ_2, Ref>& HashConsPool<Derived, ڳنڍ_2, Ref>::عامل=(HashConsPool&& other) {
	إذا (this != &other) {
		backing = std::move(other.backing);
		جدول_2 = std::move(other.جدول_2);
		free_list = std::move(other.free_list);
		other.reset();
	}
	أرجع *this;
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
فراغ HashConsPool<Derived, ڳنڍ_2, Ref>::reset() {
	backing.clear();
	free_list.clear();
	rebuild_اشاريو();
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
فراغ HashConsPool<Derived, ڳنڍ_2, Ref>::rebuild_اشاريو() {
	Derived::check_ready();
	free_list.clear();
	لكل (سائيز_t idx = 0; idx < backing.سائيز(); ++idx)
		إذا (backing[idx].is_dead())
			free_list.ڌڪو_پٺ(idx);
	std::ترتيب_ڏيو(free_list.begin(), free_list.پڇاڙي(), std::greater<سائيز_t>());
	جدول_2.سونپيو(std::ٻٽو_انگ_ceil((Derived::STATIC_ڳڻپ + سائيز()) * 2 + 2), Ref());
	لكل (Ref ref : refs())
		اشاريو_وجهو(ref);
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
سانچو<typename Eq>
Ref HashConsPool<Derived, ڳنڍ_2, Ref>::ڳوليو_hashed(uint64_t hash, Eq&& eq) ثابت {
	لكل (سائيز_t جڳهه = home_جڳهه(hash); جدول_2[جڳهه] != Ref(); جڳهه = ايندڙ_جڳهه(جڳهه)) {
		Ref ref = جدول_2[جڳهه];
		إذا (Derived::hash_ڳنڍ((*this)[ref]) == hash && eq(ref))
			أرجع ref;
	}
	أرجع Ref();
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
فراغ HashConsPool<Derived, ڳنڍ_2, Ref>::اشاريو_وجهو(Ref ref) {
	سائيز_t جڳهه = home_جڳهه(Derived::hash_ڳنڍ((*this)[ref]));
	طالما (جدول_2[جڳهه] != Ref())
		جڳهه = ايندڙ_جڳهه(جڳهه);
	جدول_2[جڳهه] = ref;
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
فراغ HashConsPool<Derived, ڳنڍ_2, Ref>::اشاريو_erase(Ref ref) {
	سائيز_t hole = home_جڳهه(Derived::hash_ڳنڍ((*this)[ref]));
	طالما (جدول_2[hole] != ref)
		hole = ايندڙ_جڳهه(hole);
	سائيز_t نقاب = جدول_2.سائيز() - 1;
	لكل (سائيز_t جڳهه = ايندڙ_جڳهه(hole); جدول_2[جڳهه] != Ref(); جڳهه = ايندڙ_جڳهه(جڳهه)) {
		سائيز_t home = home_جڳهه(Derived::hash_ڳنڍ((*this)[جدول_2[جڳهه]]));
		إذا (((جڳهه - home) & نقاب) >= ((جڳهه - hole) & نقاب)) {
			جدول_2[hole] = جدول_2[جڳهه];
			hole = جڳهه;
		}
	}
	جدول_2[hole] = Ref();
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
فراغ HashConsPool<Derived, ڳنڍ_2, Ref>::grow_اشاريو() {
	std::ويڪٽر<Ref> old(جدول_2.سائيز() * 2, Ref());
	std::مٽا_سٽا(old, جدول_2);
	لكل (Ref ref : old)
		إذا (ref != Ref())
			اشاريو_وجهو(ref);
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
Ref HashConsPool<Derived, ڳنڍ_2, Ref>::جوڙيو_inner(ڳنڍ_2 t) {
	Ref ref;
	إذا (!free_list.خالي_جايون()) {
		سائيز_t idx = free_list.پٺ();
		free_list.ڪڍو_پٺ();
		backing[idx] = std::move(t);
		ref = Ref(Derived::STATIC_ڳڻپ + idx);
	} وإلا {
		ref = Ref(Derived::STATIC_ڳڻپ + backing.سائيز());
		backing.ڌڪو_پٺ(std::move(t));
	}
	إذا ((Derived::STATIC_ڳڻپ + سائيز()) * 3 > جدول_2.سائيز() * 2)
		grow_اشاريو();
	اشاريو_وجهو(ref);
	إذا (yosys_xtrace) {
		std::cout << "#X# add_inner added ";
		self()->dump(ref);
		std::cout << "\n";
		std::cout << "#X# as integer " << ref.raw() << "\n";
	}
	أرجع ref;
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
سانچو<typename Roots>
سائيز_t HashConsPool<Derived, ڳنڍ_2, Ref>::gc(ثابت Roots& roots) {
	pool<Ref> جيئرو;
	لكل (Ref ref : roots)
		mark_جيئرو(ref, جيئرو);
	سائيز_t erased = 0;
	لكل (سائيز_t idx = 0; idx < backing.سائيز(); ++idx) {
		إذا (backing[idx].is_dead())
			استمر;
		إذا (!جيئرو.ڳڻپ(Ref(Derived::STATIC_ڳڻپ + idx))) {
			اشاريو_erase(Ref(Derived::STATIC_ڳڻپ + idx));
			free_list.ڌڪو_پٺ(idx);
			backing[idx] = ڳنڍ_2{};
			erased++;
		}
	}
	// TODO something like YOSYS_SORT_ID_FREE_LIST to make it optional?
	std::ترتيب_ڏيو(free_list.begin(), free_list.پڇاڙي(), std::greater<سائيز_t>());
	أرجع erased;
}

سانچو<typename Derived, typename ڳنڍ_2, typename Ref>
فراغ HashConsPool<Derived, ڳنڍ_2, Ref>::mark_جيئرو(Ref ref, pool<Ref>& جيئرو) ثابت {
	ref = ref.untag();
	إذا (ref == Ref() || is_static(ref) || !جيئرو.وجهو(ref).ٻيو)
		أرجع;
	Derived::for_each_ٻار((*this)[ref], [&](Ref ٻار) { mark_جيئرو(ٻار, جيئرو); });
}

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
