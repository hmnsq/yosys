/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ifndef SIGTOOLS_H
#define SIGTOOLS_H

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ସଂରଚନା SigPool
{
	ସଂରଚନା bitDef_t : ସର୍ବସାଧାରଣ std::pair<RTLIL::Wire*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା> {
		bitDef_t() : std::pair<RTLIL::Wire*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(ଖାଲି, 0) { }
		bitDef_t(ଅଚଳ RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
			h.eat(first->name);
			h.eat(second);
			ଫେରନ୍ତୁ h;
		}
	};

	pool<bitDef_t> bits;

	ଶୂନ୍ୟ clear()
	{
		bits.clear();
	}

	ଶୂନ୍ୟ add(ଅଚଳ RTLIL::SigSpec &sig)
	{
		ପାଇଁ (auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି)
				bits.ଭର୍ତ୍ତି(bit);
	}

	ଶୂନ୍ୟ add(ଅଚଳ SigPool &other)
	{
		ପାଇଁ (auto &bit : other.bits)
			bits.ଭର୍ତ୍ତି(bit);
	}

	ଶୂନ୍ୟ del(ଅଚଳ RTLIL::SigSpec &sig)
	{
		ପାଇଁ (auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି)
				bits.erase(bit);
	}

	ଶୂନ୍ୟ del(ଅଚଳ SigPool &other)
	{
		ପାଇଁ (auto &bit : other.bits)
			bits.erase(bit);
	}

	ଶୂନ୍ୟ expand(ଅଚଳ RTLIL::SigSpec &from, ଅଚଳ RTLIL::SigSpec &to)
	{
		log_assert(GetSize(from) == GetSize(to));
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(from); i++) {
			bitDef_t bit_from(from[i]), bit_to(to[i]);
			ଯଦି (bit_from.first != ଖାଲି && bit_to.first != ଖାଲି && bits.ଗଣନା_2(bit_from) > 0)
				bits.ଭର୍ତ୍ତି(bit_to);
		}
	}

	RTLIL::SigSpec extract(ଅଚଳ RTLIL::SigSpec &sig) ଅଚଳ
	{
		RTLIL::SigSpec result;
		ପାଇଁ (auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି && bits.ଗଣନା_2(bit))
				result.append(bit);
		ଫେରନ୍ତୁ result;
	}

	RTLIL::SigSpec remove(ଅଚଳ RTLIL::SigSpec &sig) ଅଚଳ
	{
		RTLIL::SigSpec result;
		ପାଇଁ (auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି && bits.ଗଣନା_2(bit) == 0)
				result.append(bit);
		ଫେରନ୍ତୁ result;
	}

	bool check(ଅଚଳ RTLIL::SigBit &bit) ଅଚଳ
	{
		ଫେରନ୍ତୁ bit.wire != ଖାଲି && bits.ଗଣନା_2(bit);
	}

	bool check_any(ଅଚଳ RTLIL::SigSpec &sig) ଅଚଳ
	{
		ପାଇଁ (auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି && bits.ଗଣନା_2(bit))
				ଫେରନ୍ତୁ true;
		ଫେରନ୍ତୁ false;
	}

	bool check_all(ଅଚଳ RTLIL::SigSpec &sig) ଅଚଳ
	{
		ପାଇଁ (auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି && bits.ଗଣନା_2(bit) == 0)
				ଫେରନ୍ତୁ false;
		ଫେରନ୍ତୁ true;
	}

	RTLIL::SigSpec export_one() ଅଚଳ
	{
		ପାଇଁ (auto &bit : bits)
			ଫେରନ୍ତୁ RTLIL::SigSpec(bit.first, bit.second);
		ଫେରନ୍ତୁ RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ଅଚଳ
	{
		pool<RTLIL::SigBit> sig;
		ପାଇଁ (auto &bit : bits)
			sig.ଭର୍ତ୍ତି(RTLIL::SigBit(bit.first, bit.second));
		ଫେରନ୍ତୁ sig;
	}

	size_t size() ଅଚଳ
	{
		ଫେରନ୍ତୁ bits.size();
	}
};

template <typename T, class Compare = ଶୂନ୍ୟ>
ସଂରଚନା SigSet
{
	static_assert(!std::is_same<Compare,ଶୂନ୍ୟ>::ମୂଲ୍ୟ, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	ସଂରଚନା bitDef_t : ସର୍ବସାଧାରଣ std::pair<RTLIL::Wire*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା> {
		bitDef_t() : std::pair<RTLIL::Wire*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(ଖାଲି, 0) { }
		bitDef_t(ଅଚଳ RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(bit.wire, bit.offset) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
			h.eat(first->name);
			h.eat(second);
			ଫେରନ୍ତୁ h;
		}
	};

	dict<bitDef_t, std::set<T, Compare>> bits;

	ଶୂନ୍ୟ clear()
	{
		bits.clear();
	}

	ଶୂନ୍ୟ ଭର୍ତ୍ତି(ଅଚଳ RTLIL::SigSpec &sig, T data)
	{
		ପାଇଁ (ଅଚଳ auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି)
				bits[bit].ଭର୍ତ୍ତି(data);
	}

	ଶୂନ୍ୟ ଭର୍ତ୍ତି(ଅଚଳ RTLIL::SigSpec& sig, ଅଚଳ std::set<T> &data)
	{
		ପାଇଁ (ଅଚଳ auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି)
				bits[bit].ଭର୍ତ୍ତି(data.begin(), data.end());
	}

	ଶୂନ୍ୟ erase(ଅଚଳ RTLIL::SigSpec& sig)
	{
		ପାଇଁ (ଅଚଳ auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି)
				bits[bit].clear();
	}

	ଶୂନ୍ୟ erase(ଅଚଳ RTLIL::SigSpec &sig, T data)
	{
		ପାଇଁ (ଅଚଳ auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି)
				bits[bit].erase(data);
	}

	ଶୂନ୍ୟ erase(ଅଚଳ RTLIL::SigSpec &sig, ଅଚଳ std::set<T> &data)
	{
		ପାଇଁ (ଅଚଳ auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି)
				bits[bit].erase(data.begin(), data.end());
	}

	ଶୂନ୍ୟ find(ଅଚଳ RTLIL::SigSpec &sig, std::set<T> &result)
	{
		ପାଇଁ (ଅଚଳ auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି) {
				auto &data = bits[bit];
				result.ଭର୍ତ୍ତି(data.begin(), data.end());
			}
	}

	ଶୂନ୍ୟ find(ଅଚଳ RTLIL::SigSpec &sig, pool<T> &result)
	{
		ପାଇଁ (ଅଚଳ auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି) {
				auto &data = bits[bit];
				result.ଭର୍ତ୍ତି(data.begin(), data.end());
			}
	}

	std::set<T> find(ଅଚଳ RTLIL::SigSpec &sig)
	{
		std::set<T> result;
		find(sig, result);
		ଫେରନ୍ତୁ result;
	}

	bool has(ଅଚଳ RTLIL::SigSpec &sig)
	{
		ପାଇଁ (auto &bit : sig)
			ଯଦି (bit.wire != ଖାଲି && bits.ଗଣନା_2(bit))
				ଫେରନ୍ତୁ true;
		ଫେରନ୍ତୁ false;
	}
};

template<typename T>
class SigSet<T, typename std::enable_if<!std::is_pointer<T>::ମୂଲ୍ୟ>::type> : ସର୍ବସାଧାରଣ SigSet<T, std::less<T>> {};
template<typename T>
using sort_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::ମୂଲ୍ୟ>::type;
template<typename T>
class SigSet<T, sort_by_name_id_guard<T>> : ସର୍ବସାଧାରଣ SigSet<T, RTLIL::sort_by_name_id<typename std::remove_pointer<T>::type>> {};

ସଂରଚନା SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	ଶୂନ୍ୟ apply(RTLIL::SigBit &bit) ଅଚଳ
	{
		bit = database.find(bit);
	}

	ଶୂନ୍ୟ apply(RTLIL::SigSpec &sig) ଅଚଳ
	{
		ପାଇଁ (auto &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit operator()(RTLIL::SigBit bit) ଅଚଳ
	{
		apply(bit);
		ଫେରନ୍ତୁ bit;
	}

	RTLIL::SigSpec operator()(RTLIL::SigSpec sig) ଅଚଳ
	{
		apply(sig);
		ଫେରନ୍ତୁ sig;
	}

	RTLIL::SigSpec operator()(RTLIL::Wire *wire) ଅଚଳ
	{
		SigSpec sig(wire);
		apply(sig);
		ଫେରନ୍ତୁ sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
ସଂରଚନା SigMap final : ସର୍ବସାଧାରଣ SigMapView
{
	SigMap(RTLIL::Module *module = ଖାଲି)
	{
		ଯଦି (module != ଖାଲି)
			set(module);
	}

	ଶୂନ୍ୟ swap(SigMap &other)
	{
		database.swap(other.database);
	}

	ଶୂନ୍ୟ clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	ଶୂନ୍ୟ set(RTLIL::Module *module)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା bitcount = 0;
		ପାଇଁ (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		database.reserve(bitcount);

		ପାଇଁ (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	ଶୂନ୍ୟ add(ଅଚଳ RTLIL::SigSpec& from, ଅଚଳ RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(from); i++)
		{
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା bfi = database.lookup(from[i]);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା bti = database.lookup(to[i]);

			ଅଚଳ RTLIL::SigBit &bf = database[bfi];
			ଅଚଳ RTLIL::SigBit &bt = database[bti];

			ଯଦି (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				ଯଦି (bf.wire == nullptr)
					database.ipromote(bfi);

				ଯଦି (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	ଶୂନ୍ୟ add(ଅଚଳ RTLIL::SigBit &bit)
	{
		ଅଚଳ auto &b = database.find(bit);
		ଯଦି (b.wire != nullptr)
			database.promote(bit);
	}

	ଶୂନ୍ୟ add(ଅଚଳ RTLIL::SigSpec &sig)
	{
		ପାଇଁ (ଅଚଳ auto &bit : sig)
			add(bit);
	}

	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ add(Wire *wire) { ଫେରନ୍ତୁ add(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ଅଚଳ
	{
		RTLIL::SigSpec sig;
		ପାଇଁ (ଅଚଳ auto &bit : database)
			ଯଦି (bit.wire != nullptr)
				sig.append(bit);
		ଫେରନ୍ତୁ sig;
	}
};

/**
 * SiValgMap wraps a union-find "database" to map SigBits of a module to
 * canonical representative SigBits plus some optional Val value associated with the bits.
 * Val has a commutative, associative, idempotent operator|=, a default constructor
 * which constructs an identity element, and a copy constructor.
 * SigBits that are connected share a set in the underlying database;
 * the associated value is the "sum" of all the values associated with the contributing bits.
 * If any of the SigBits in a set are a constant, the canonical SigBit is a constant.
 */
template <class Val>
ସଂରଚନା SigValMap final : ସର୍ବସାଧାରଣ SigMapView
{
	dict<SigBit, Val> ମୂଲ୍ୟଗୁଡ଼ିକ;

	ଶୂନ୍ୟ swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		ମୂଲ୍ୟଗୁଡ଼ିକ.swap(other.ମୂଲ୍ୟଗୁଡ଼ିକ);
	}

	ଶୂନ୍ୟ clear()
	{
		database.clear();
		ମୂଲ୍ୟଗୁଡ଼ିକ.clear();
	}

	// Rebuild SigMap for all connections in module
	ଶୂନ୍ୟ set(RTLIL::Module *module)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା bitcount = 0;
		ପାଇଁ (auto &it : module->connections())
			bitcount += it.first.size();

		database.clear();
		ମୂଲ୍ୟଗୁଡ଼ିକ.clear();
		database.reserve(bitcount);

		ପାଇଁ (auto &it : module->connections())
			add(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	ଶୂନ୍ୟ add(ଅଚଳ RTLIL::SigSpec& from, ଅଚଳ RTLIL::SigSpec& to)
	{
		log_assert(GetSize(from) == GetSize(to));

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(from); i++)
		{
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା bfi = database.lookup(from[i]);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା bti = database.lookup(to[i]);
			ଯଦି (bfi == bti) {
				ଜାରି;
			}

			ଅଚଳ RTLIL::SigBit &bf = database[bfi];
			ଅଚଳ RTLIL::SigBit &bt = database[bti];
			ଯଦି (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				merge_ମୂଲ୍ୟ(bt, bf);
			} ଅନ୍ୟଥା {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				merge_ମୂଲ୍ୟ(bf, bt);
			}
		}
	}

	ଶୂନ୍ୟ addVal(ଅଚଳ RTLIL::SigBit &bit, ଅଚଳ Val &val)
	{
		ମୂଲ୍ୟଗୁଡ଼ିକ[database.find(bit)] |= val;
	}

	ଶୂନ୍ୟ addVal(ଅଚଳ RTLIL::SigSpec &sig, ଅଚଳ Val &val)
	{
		ପାଇଁ (ଅଚଳ auto &bit : sig)
			addVal(bit, val);
	}

	Val apply_and_get_ମୂଲ୍ୟ(RTLIL::SigBit &bit) ଅଚଳ
	{
		bit = database.find(bit);
		auto it = ମୂଲ୍ୟଗୁଡ଼ିକ.find(bit);
		ଫେରନ୍ତୁ it == ମୂଲ୍ୟଗୁଡ଼ିକ.end() ? Val() : it->second;
	}

ବ୍ୟକ୍ତିଗତ:
	ଶୂନ୍ୟ merge_ମୂଲ୍ୟ(ଅଚଳ RTLIL::SigBit &from, ଅଚଳ RTLIL::SigBit &to)
	{
		auto it = ମୂଲ୍ୟଗୁଡ଼ିକ.find(from);
		ଯଦି (it == ମୂଲ୍ୟଗୁଡ଼ିକ.end()) {
			ଫେରନ୍ତୁ;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		ମୂଲ୍ୟଗୁଡ଼ିକ.erase(it);
		ମୂଲ୍ୟଗୁଡ଼ିକ[to] |= v;
	}
};

YOSYS_NAMESPACE_END

#endif /* SIGTOOLS_H */
