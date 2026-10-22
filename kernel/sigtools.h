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

#اگر_غیر_تعریف_شدہ SIGTOOLS_H
#تعریف_کرو SIGTOOLS_H

#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ساخت SigPool
{
	ساخت ثنائی_ہندسہ_Def_t : عوامی std::pair<RTLIL::Wire*, صحیح> {
		ثنائی_ہندسہ_Def_t() : std::pair<RTLIL::Wire*, صحیح>(تہی, 0) { }
		ثنائی_ہندسہ_Def_t(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ) : std::pair<RTLIL::Wire*, صحیح>(ثنائی_ہندسہ.wire, ثنائی_ہندسہ.انحراف) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(پہلا->name);
			h.eat(دوسرا);
			واپس h;
		}
	};

	pool<ثنائی_ہندسہ_Def_t> بٹ;

	خالی clear()
	{
		بٹ.clear();
	}

	خالی جوڑو(ثابت RTLIL::SigSpec &sig)
	{
		برائے (خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی)
				بٹ.داخل_کرو(ثنائی_ہندسہ);
	}

	خالی جوڑو(ثابت SigPool &other)
	{
		برائے (خودکار &ثنائی_ہندسہ : other.بٹ)
			بٹ.داخل_کرو(ثنائی_ہندسہ);
	}

	خالی del(ثابت RTLIL::SigSpec &sig)
	{
		برائے (خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی)
				بٹ.erase(ثنائی_ہندسہ);
	}

	خالی del(ثابت SigPool &other)
	{
		برائے (خودکار &ثنائی_ہندسہ : other.بٹ)
			بٹ.erase(ثنائی_ہندسہ);
	}

	خالی expand(ثابت RTLIL::SigSpec &from, ثابت RTLIL::SigSpec &مقصد)
	{
		log_assert(Get_سائز(from) == Get_سائز(مقصد));
		برائے (صحیح i = 0; i < Get_سائز(from); i++) {
			ثنائی_ہندسہ_Def_t ثنائی_ہندسہ_from(from[i]), ثنائی_ہندسہ_مقصد(مقصد[i]);
			اگر (ثنائی_ہندسہ_from.پہلا != تہی && ثنائی_ہندسہ_مقصد.پہلا != تہی && بٹ.گنتی(ثنائی_ہندسہ_from) > 0)
				بٹ.داخل_کرو(ثنائی_ہندسہ_مقصد);
		}
	}

	RTLIL::SigSpec نکالو_2(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec نتیجہ;
		برائے (خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی && بٹ.گنتی(ثنائی_ہندسہ))
				نتیجہ.append(ثنائی_ہندسہ);
		واپس نتیجہ;
	}

	RTLIL::SigSpec remove(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec نتیجہ;
		برائے (خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی && بٹ.گنتی(ثنائی_ہندسہ) == 0)
				نتیجہ.append(ثنائی_ہندسہ);
		واپس نتیجہ;
	}

	bool check(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ) ثابت
	{
		واپس ثنائی_ہندسہ.wire != تہی && بٹ.گنتی(ثنائی_ہندسہ);
	}

	bool check_any(ثابت RTLIL::SigSpec &sig) ثابت
	{
		برائے (خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی && بٹ.گنتی(ثنائی_ہندسہ))
				واپس true;
		واپس false;
	}

	bool check_all(ثابت RTLIL::SigSpec &sig) ثابت
	{
		برائے (خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی && بٹ.گنتی(ثنائی_ہندسہ) == 0)
				واپس false;
		واپس true;
	}

	RTLIL::SigSpec export_one() ثابت
	{
		برائے (خودکار &ثنائی_ہندسہ : بٹ)
			واپس RTLIL::SigSpec(ثنائی_ہندسہ.پہلا, ثنائی_ہندسہ.دوسرا);
		واپس RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ثابت
	{
		pool<RTLIL::Sig_ثنائی_ہندسہ> sig;
		برائے (خودکار &ثنائی_ہندسہ : بٹ)
			sig.داخل_کرو(RTLIL::Sig_ثنائی_ہندسہ(ثنائی_ہندسہ.پہلا, ثنائی_ہندسہ.دوسرا));
		واپس sig;
	}

	سائز_t سائز() ثابت
	{
		واپس بٹ.سائز();
	}
};

سانچہ <typename T, class Compare = خالی>
ساخت Sig_سیٹ
{
	static_assert(!std::is_same<Compare,خالی>::قدر, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	ساخت ثنائی_ہندسہ_Def_t : عوامی std::pair<RTLIL::Wire*, صحیح> {
		ثنائی_ہندسہ_Def_t() : std::pair<RTLIL::Wire*, صحیح>(تہی, 0) { }
		ثنائی_ہندسہ_Def_t(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ) : std::pair<RTLIL::Wire*, صحیح>(ثنائی_ہندسہ.wire, ثنائی_ہندسہ.انحراف) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(پہلا->name);
			h.eat(دوسرا);
			واپس h;
		}
	};

	dict<ثنائی_ہندسہ_Def_t, std::سیٹ<T, Compare>> بٹ;

	خالی clear()
	{
		بٹ.clear();
	}

	خالی داخل_کرو(ثابت RTLIL::SigSpec &sig, T ڈیٹا)
	{
		برائے (ثابت خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی)
				بٹ[ثنائی_ہندسہ].داخل_کرو(ڈیٹا);
	}

	خالی داخل_کرو(ثابت RTLIL::SigSpec& sig, ثابت std::سیٹ<T> &ڈیٹا)
	{
		برائے (ثابت خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی)
				بٹ[ثنائی_ہندسہ].داخل_کرو(ڈیٹا.begin(), ڈیٹا.انتہا());
	}

	خالی erase(ثابت RTLIL::SigSpec& sig)
	{
		برائے (ثابت خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی)
				بٹ[ثنائی_ہندسہ].clear();
	}

	خالی erase(ثابت RTLIL::SigSpec &sig, T ڈیٹا)
	{
		برائے (ثابت خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی)
				بٹ[ثنائی_ہندسہ].erase(ڈیٹا);
	}

	خالی erase(ثابت RTLIL::SigSpec &sig, ثابت std::سیٹ<T> &ڈیٹا)
	{
		برائے (ثابت خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی)
				بٹ[ثنائی_ہندسہ].erase(ڈیٹا.begin(), ڈیٹا.انتہا());
	}

	خالی ڈھونڈو(ثابت RTLIL::SigSpec &sig, std::سیٹ<T> &نتیجہ)
	{
		برائے (ثابت خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی) {
				خودکار &ڈیٹا = بٹ[ثنائی_ہندسہ];
				نتیجہ.داخل_کرو(ڈیٹا.begin(), ڈیٹا.انتہا());
			}
	}

	خالی ڈھونڈو(ثابت RTLIL::SigSpec &sig, pool<T> &نتیجہ)
	{
		برائے (ثابت خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی) {
				خودکار &ڈیٹا = بٹ[ثنائی_ہندسہ];
				نتیجہ.داخل_کرو(ڈیٹا.begin(), ڈیٹا.انتہا());
			}
	}

	std::سیٹ<T> ڈھونڈو(ثابت RTLIL::SigSpec &sig)
	{
		std::سیٹ<T> نتیجہ;
		ڈھونڈو(sig, نتیجہ);
		واپس نتیجہ;
	}

	bool has(ثابت RTLIL::SigSpec &sig)
	{
		برائے (خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire != تہی && بٹ.گنتی(ثنائی_ہندسہ))
				واپس true;
		واپس false;
	}
};

سانچہ<typename T>
class Sig_سیٹ<T, typename std::enable_if<!std::is_اشارہ_گر<T>::قدر>::type> : عوامی Sig_سیٹ<T, std::less<T>> {};
سانچہ<typename T>
using ترتیب_دو_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::قدر>::type;
سانچہ<typename T>
class Sig_سیٹ<T, ترتیب_دو_by_name_id_guard<T>> : عوامی Sig_سیٹ<T, RTLIL::ترتیب_دو_by_name_id<typename std::remove_اشارہ_گر<T>::type>> {};

ساخت SigMapView
{
	mfp<Sig_ثنائی_ہندسہ> database;

	// Modify bit to its representative
	خالی apply(RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ) ثابت
	{
		ثنائی_ہندسہ = database.ڈھونڈو(ثنائی_ہندسہ);
	}

	خالی apply(RTLIL::SigSpec &sig) ثابت
	{
		برائے (خودکار &ثنائی_ہندسہ : sig)
			apply(ثنائی_ہندسہ);
	}

	RTLIL::Sig_ثنائی_ہندسہ عامل()(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		apply(ثنائی_ہندسہ);
		واپس ثنائی_ہندسہ;
	}

	RTLIL::SigSpec عامل()(RTLIL::SigSpec sig) ثابت
	{
		apply(sig);
		واپس sig;
	}

	RTLIL::SigSpec عامل()(RTLIL::Wire *wire) ثابت
	{
		SigSpec sig(wire);
		apply(sig);
		واپس sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
ساخت SigMap final : عوامی SigMapView
{
	SigMap(RTLIL::Module *module = تہی)
	{
		اگر (module != تہی)
			سیٹ(module);
	}

	خالی تبادلہ(SigMap &other)
	{
		database.تبادلہ(other.database);
	}

	خالی clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	خالی سیٹ(RTLIL::Module *module)
	{
		صحیح bitcount = 0;
		برائے (خودکار &it : module->connections())
			bitcount += it.پہلا.سائز();

		database.clear();
		database.reserve(bitcount);

		برائے (خودکار &it : module->connections())
			جوڑو(it.پہلا, it.دوسرا);
	}

	// Add connections from "from" to "to", bit-by-bit
	خالی جوڑو(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& مقصد)
	{
		log_assert(Get_سائز(from) == Get_سائز(مقصد));

		برائے (صحیح i = 0; i < Get_سائز(from); i++)
		{
			صحیح bfi = database.lookup(from[i]);
			صحیح bti = database.lookup(مقصد[i]);

			ثابت RTLIL::Sig_ثنائی_ہندسہ &bf = database[bfi];
			ثابت RTLIL::Sig_ثنائی_ہندسہ &bt = database[bti];

			اگر (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				اگر (bf.wire == nullptr)
					database.ipromote(bfi);

				اگر (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	خالی جوڑو(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ)
	{
		ثابت خودکار &b = database.ڈھونڈو(ثنائی_ہندسہ);
		اگر (b.wire != nullptr)
			database.promote(ثنائی_ہندسہ);
	}

	خالی جوڑو(ثابت RTLIL::SigSpec &sig)
	{
		برائے (ثابت خودکار &ثنائی_ہندسہ : sig)
			جوڑو(ثنائی_ہندسہ);
	}

	اندرونی خالی جوڑو(Wire *wire) { واپس جوڑو(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ثابت
	{
		RTLIL::SigSpec sig;
		برائے (ثابت خودکار &ثنائی_ہندسہ : database)
			اگر (ثنائی_ہندسہ.wire != nullptr)
				sig.append(ثنائی_ہندسہ);
		واپس sig;
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
سانچہ <class Val>
ساخت SigValMap final : عوامی SigMapView
{
	dict<Sig_ثنائی_ہندسہ, Val> اقدار;

	خالی تبادلہ(SigValMap<Val> &other)
	{
		database.تبادلہ(other.database);
		اقدار.تبادلہ(other.اقدار);
	}

	خالی clear()
	{
		database.clear();
		اقدار.clear();
	}

	// Rebuild SigMap for all connections in module
	خالی سیٹ(RTLIL::Module *module)
	{
		صحیح bitcount = 0;
		برائے (خودکار &it : module->connections())
			bitcount += it.پہلا.سائز();

		database.clear();
		اقدار.clear();
		database.reserve(bitcount);

		برائے (خودکار &it : module->connections())
			جوڑو(it.پہلا, it.دوسرا);
	}

	// Add connections from "from" to "to", bit-by-bit.
	خالی جوڑو(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& مقصد)
	{
		log_assert(Get_سائز(from) == Get_سائز(مقصد));

		برائے (صحیح i = 0; i < Get_سائز(from); i++)
		{
			صحیح bfi = database.lookup(from[i]);
			صحیح bti = database.lookup(مقصد[i]);
			اگر (bfi == bti) {
				جاری;
			}

			ثابت RTLIL::Sig_ثنائی_ہندسہ &bf = database[bfi];
			ثابت RTLIL::Sig_ثنائی_ہندسہ &bt = database[bti];
			اگر (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				ملاؤ_قدر(bt, bf);
			} ورنہ {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				ملاؤ_قدر(bf, bt);
			}
		}
	}

	خالی جوڑو_Val(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ, ثابت Val &val)
	{
		اقدار[database.ڈھونڈو(ثنائی_ہندسہ)] |= val;
	}

	خالی جوڑو_Val(ثابت RTLIL::SigSpec &sig, ثابت Val &val)
	{
		برائے (ثابت خودکار &ثنائی_ہندسہ : sig)
			جوڑو_Val(ثنائی_ہندسہ, val);
	}

	Val apply_and_get_قدر(RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ) ثابت
	{
		ثنائی_ہندسہ = database.ڈھونڈو(ثنائی_ہندسہ);
		خودکار it = اقدار.ڈھونڈو(ثنائی_ہندسہ);
		واپس it == اقدار.انتہا() ? Val() : it->دوسرا;
	}

نجی:
	خالی ملاؤ_قدر(ثابت RTLIL::Sig_ثنائی_ہندسہ &from, ثابت RTLIL::Sig_ثنائی_ہندسہ &مقصد)
	{
		خودکار it = اقدار.ڈھونڈو(from);
		اگر (it == اقدار.انتہا()) {
			واپس;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->دوسرا;
		اقدار.erase(it);
		اقدار[مقصد] |= v;
	}
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم /* SIGTOOLS_H */
