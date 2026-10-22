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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SIGTOOLS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIGTOOLS_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ਬਣਤਰ SigPool
{
	ਬਣਤਰ ਦੋਆਧਾਰੀ_ਅੰਕ_Def_t : ਜਨਤਕ std::pair<RTLIL::Wire*, ਪੂਰਨ> {
		ਦੋਆਧਾਰੀ_ਅੰਕ_Def_t() : std::pair<RTLIL::Wire*, ਪੂਰਨ>(ਨਿਰਾ, 0) { }
		ਦੋਆਧਾਰੀ_ਅੰਕ_Def_t(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ) : std::pair<RTLIL::Wire*, ਪੂਰਨ>(ਦੋਆਧਾਰੀ_ਅੰਕ.wire, ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
			h.eat(ਪਹਿਲਾ->name);
			h.eat(ਦੂਜਾ);
			ਵਾਪਸ h;
		}
	};

	pool<ਦੋਆਧਾਰੀ_ਅੰਕ_Def_t> ਬਿੱਟ;

	ਖਾਲੀ clear()
	{
		ਬਿੱਟ.clear();
	}

	ਖਾਲੀ ਜੋੜੋ(ਅਚਲ RTLIL::SigSpec &sig)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ)
				ਬਿੱਟ.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	ਖਾਲੀ ਜੋੜੋ(ਅਚਲ SigPool &other)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : other.ਬਿੱਟ)
			ਬਿੱਟ.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	ਖਾਲੀ del(ਅਚਲ RTLIL::SigSpec &sig)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ)
				ਬਿੱਟ.erase(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	ਖਾਲੀ del(ਅਚਲ SigPool &other)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : other.ਬਿੱਟ)
			ਬਿੱਟ.erase(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	ਖਾਲੀ expand(ਅਚਲ RTLIL::SigSpec &from, ਅਚਲ RTLIL::SigSpec &ਮੰਜ਼ਿਲ)
	{
		log_assert(Get_ਮਾਪ(from) == Get_ਮਾਪ(ਮੰਜ਼ਿਲ));
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(from); i++) {
			ਦੋਆਧਾਰੀ_ਅੰਕ_Def_t ਦੋਆਧਾਰੀ_ਅੰਕ_from(from[i]), ਦੋਆਧਾਰੀ_ਅੰਕ_ਮੰਜ਼ਿਲ(ਮੰਜ਼ਿਲ[i]);
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ_from.ਪਹਿਲਾ != ਨਿਰਾ && ਦੋਆਧਾਰੀ_ਅੰਕ_ਮੰਜ਼ਿਲ.ਪਹਿਲਾ != ਨਿਰਾ && ਬਿੱਟ.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ_from) > 0)
				ਬਿੱਟ.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ_ਮੰਜ਼ਿਲ);
		}
	}

	RTLIL::SigSpec ਕੱਢੋ_2(ਅਚਲ RTLIL::SigSpec &sig) ਅਚਲ
	{
		RTLIL::SigSpec ਨਤੀਜਾ;
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ && ਬਿੱਟ.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ))
				ਨਤੀਜਾ.append(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	RTLIL::SigSpec remove(ਅਚਲ RTLIL::SigSpec &sig) ਅਚਲ
	{
		RTLIL::SigSpec ਨਤੀਜਾ;
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ && ਬਿੱਟ.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ) == 0)
				ਨਤੀਜਾ.append(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	bool check(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
	{
		ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ && ਬਿੱਟ.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	bool check_any(ਅਚਲ RTLIL::SigSpec &sig) ਅਚਲ
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ && ਬਿੱਟ.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ))
				ਵਾਪਸ true;
		ਵਾਪਸ false;
	}

	bool check_all(ਅਚਲ RTLIL::SigSpec &sig) ਅਚਲ
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ && ਬਿੱਟ.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ) == 0)
				ਵਾਪਸ false;
		ਵਾਪਸ true;
	}

	RTLIL::SigSpec export_one() ਅਚਲ
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ)
			ਵਾਪਸ RTLIL::SigSpec(ਦੋਆਧਾਰੀ_ਅੰਕ.ਪਹਿਲਾ, ਦੋਆਧਾਰੀ_ਅੰਕ.ਦੂਜਾ);
		ਵਾਪਸ RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ਅਚਲ
	{
		pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> sig;
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ)
			sig.ਪਾਓ(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(ਦੋਆਧਾਰੀ_ਅੰਕ.ਪਹਿਲਾ, ਦੋਆਧਾਰੀ_ਅੰਕ.ਦੂਜਾ));
		ਵਾਪਸ sig;
	}

	ਮਾਪ_t ਮਾਪ() ਅਚਲ
	{
		ਵਾਪਸ ਬਿੱਟ.ਮਾਪ();
	}
};

ਸਾਂਚਾ <typename T, class Compare = ਖਾਲੀ>
ਬਣਤਰ Sig_ਸੈੱਟ
{
	static_assert(!std::is_same<Compare,ਖਾਲੀ>::ਮੁੱਲ, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	ਬਣਤਰ ਦੋਆਧਾਰੀ_ਅੰਕ_Def_t : ਜਨਤਕ std::pair<RTLIL::Wire*, ਪੂਰਨ> {
		ਦੋਆਧਾਰੀ_ਅੰਕ_Def_t() : std::pair<RTLIL::Wire*, ਪੂਰਨ>(ਨਿਰਾ, 0) { }
		ਦੋਆਧਾਰੀ_ਅੰਕ_Def_t(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ) : std::pair<RTLIL::Wire*, ਪੂਰਨ>(ਦੋਆਧਾਰੀ_ਅੰਕ.wire, ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
			h.eat(ਪਹਿਲਾ->name);
			h.eat(ਦੂਜਾ);
			ਵਾਪਸ h;
		}
	};

	dict<ਦੋਆਧਾਰੀ_ਅੰਕ_Def_t, std::ਸੈੱਟ<T, Compare>> ਬਿੱਟ;

	ਖਾਲੀ clear()
	{
		ਬਿੱਟ.clear();
	}

	ਖਾਲੀ ਪਾਓ(ਅਚਲ RTLIL::SigSpec &sig, T ਅੰਕੜੇ)
	{
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ)
				ਬਿੱਟ[ਦੋਆਧਾਰੀ_ਅੰਕ].ਪਾਓ(ਅੰਕੜੇ);
	}

	ਖਾਲੀ ਪਾਓ(ਅਚਲ RTLIL::SigSpec& sig, ਅਚਲ std::ਸੈੱਟ<T> &ਅੰਕੜੇ)
	{
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ)
				ਬਿੱਟ[ਦੋਆਧਾਰੀ_ਅੰਕ].ਪਾਓ(ਅੰਕੜੇ.begin(), ਅੰਕੜੇ.ਸਿਰਾ());
	}

	ਖਾਲੀ erase(ਅਚਲ RTLIL::SigSpec& sig)
	{
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ)
				ਬਿੱਟ[ਦੋਆਧਾਰੀ_ਅੰਕ].clear();
	}

	ਖਾਲੀ erase(ਅਚਲ RTLIL::SigSpec &sig, T ਅੰਕੜੇ)
	{
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ)
				ਬਿੱਟ[ਦੋਆਧਾਰੀ_ਅੰਕ].erase(ਅੰਕੜੇ);
	}

	ਖਾਲੀ erase(ਅਚਲ RTLIL::SigSpec &sig, ਅਚਲ std::ਸੈੱਟ<T> &ਅੰਕੜੇ)
	{
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ)
				ਬਿੱਟ[ਦੋਆਧਾਰੀ_ਅੰਕ].erase(ਅੰਕੜੇ.begin(), ਅੰਕੜੇ.ਸਿਰਾ());
	}

	ਖਾਲੀ ਲੱਭੋ(ਅਚਲ RTLIL::SigSpec &sig, std::ਸੈੱਟ<T> &ਨਤੀਜਾ)
	{
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ) {
				ਸਵੈਚਾਲਿਤ &ਅੰਕੜੇ = ਬਿੱਟ[ਦੋਆਧਾਰੀ_ਅੰਕ];
				ਨਤੀਜਾ.ਪਾਓ(ਅੰਕੜੇ.begin(), ਅੰਕੜੇ.ਸਿਰਾ());
			}
	}

	ਖਾਲੀ ਲੱਭੋ(ਅਚਲ RTLIL::SigSpec &sig, pool<T> &ਨਤੀਜਾ)
	{
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ) {
				ਸਵੈਚਾਲਿਤ &ਅੰਕੜੇ = ਬਿੱਟ[ਦੋਆਧਾਰੀ_ਅੰਕ];
				ਨਤੀਜਾ.ਪਾਓ(ਅੰਕੜੇ.begin(), ਅੰਕੜੇ.ਸਿਰਾ());
			}
	}

	std::ਸੈੱਟ<T> ਲੱਭੋ(ਅਚਲ RTLIL::SigSpec &sig)
	{
		std::ਸੈੱਟ<T> ਨਤੀਜਾ;
		ਲੱਭੋ(sig, ਨਤੀਜਾ);
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	bool has(ਅਚਲ RTLIL::SigSpec &sig)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ && ਬਿੱਟ.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ))
				ਵਾਪਸ true;
		ਵਾਪਸ false;
	}
};

ਸਾਂਚਾ<typename T>
class Sig_ਸੈੱਟ<T, typename std::enable_if<!std::is_ਸੂਚਕ<T>::ਮੁੱਲ>::type> : ਜਨਤਕ Sig_ਸੈੱਟ<T, std::less<T>> {};
ਸਾਂਚਾ<typename T>
using ਛਾਂਟੋ_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::ਮੁੱਲ>::type;
ਸਾਂਚਾ<typename T>
class Sig_ਸੈੱਟ<T, ਛਾਂਟੋ_by_name_id_guard<T>> : ਜਨਤਕ Sig_ਸੈੱਟ<T, RTLIL::ਛਾਂਟੋ_by_name_id<typename std::remove_ਸੂਚਕ<T>::type>> {};

ਬਣਤਰ SigMapView
{
	mfp<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> database;

	// Modify bit to its representative
	ਖਾਲੀ apply(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
	{
		ਦੋਆਧਾਰੀ_ਅੰਕ = database.ਲੱਭੋ(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	ਖਾਲੀ apply(RTLIL::SigSpec &sig) ਅਚਲ
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			apply(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਸੰਚਾਲਕ()(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
	{
		apply(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ;
	}

	RTLIL::SigSpec ਸੰਚਾਲਕ()(RTLIL::SigSpec sig) ਅਚਲ
	{
		apply(sig);
		ਵਾਪਸ sig;
	}

	RTLIL::SigSpec ਸੰਚਾਲਕ()(RTLIL::Wire *wire) ਅਚਲ
	{
		SigSpec sig(wire);
		apply(sig);
		ਵਾਪਸ sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
ਬਣਤਰ SigMap final : ਜਨਤਕ SigMapView
{
	SigMap(RTLIL::Module *module = ਨਿਰਾ)
	{
		ਜੇ (module != ਨਿਰਾ)
			ਸੈੱਟ(module);
	}

	ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ(SigMap &other)
	{
		database.ਅਦਲਾ_ਬਦਲੀ(other.database);
	}

	ਖਾਲੀ clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	ਖਾਲੀ ਸੈੱਟ(RTLIL::Module *module)
	{
		ਪੂਰਨ bitcount = 0;
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : module->connections())
			bitcount += it.ਪਹਿਲਾ.ਮਾਪ();

		database.clear();
		database.reserve(bitcount);

		ਲਈ (ਸਵੈਚਾਲਿਤ &it : module->connections())
			ਜੋੜੋ(it.ਪਹਿਲਾ, it.ਦੂਜਾ);
	}

	// Add connections from "from" to "to", bit-by-bit
	ਖਾਲੀ ਜੋੜੋ(ਅਚਲ RTLIL::SigSpec& from, ਅਚਲ RTLIL::SigSpec& ਮੰਜ਼ਿਲ)
	{
		log_assert(Get_ਮਾਪ(from) == Get_ਮਾਪ(ਮੰਜ਼ਿਲ));

		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(from); i++)
		{
			ਪੂਰਨ bfi = database.lookup(from[i]);
			ਪੂਰਨ bti = database.lookup(ਮੰਜ਼ਿਲ[i]);

			ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &bf = database[bfi];
			ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &bt = database[bti];

			ਜੇ (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				ਜੇ (bf.wire == nullptr)
					database.ipromote(bfi);

				ਜੇ (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	ਖਾਲੀ ਜੋੜੋ(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ)
	{
		ਅਚਲ ਸਵੈਚਾਲਿਤ &b = database.ਲੱਭੋ(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਜੇ (b.wire != nullptr)
			database.promote(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	ਖਾਲੀ ਜੋੜੋ(ਅਚਲ RTLIL::SigSpec &sig)
	{
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੋੜੋ(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	ਸਤਰਅੰਦਰ ਖਾਲੀ ਜੋੜੋ(Wire *wire) { ਵਾਪਸ ਜੋੜੋ(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ਅਚਲ
	{
		RTLIL::SigSpec sig;
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : database)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != nullptr)
				sig.append(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਵਾਪਸ sig;
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
ਸਾਂਚਾ <class Val>
ਬਣਤਰ SigValMap final : ਜਨਤਕ SigMapView
{
	dict<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, Val> ਮੁੱਲਾਂ;

	ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ(SigValMap<Val> &other)
	{
		database.ਅਦਲਾ_ਬਦਲੀ(other.database);
		ਮੁੱਲਾਂ.ਅਦਲਾ_ਬਦਲੀ(other.ਮੁੱਲਾਂ);
	}

	ਖਾਲੀ clear()
	{
		database.clear();
		ਮੁੱਲਾਂ.clear();
	}

	// Rebuild SigMap for all connections in module
	ਖਾਲੀ ਸੈੱਟ(RTLIL::Module *module)
	{
		ਪੂਰਨ bitcount = 0;
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : module->connections())
			bitcount += it.ਪਹਿਲਾ.ਮਾਪ();

		database.clear();
		ਮੁੱਲਾਂ.clear();
		database.reserve(bitcount);

		ਲਈ (ਸਵੈਚਾਲਿਤ &it : module->connections())
			ਜੋੜੋ(it.ਪਹਿਲਾ, it.ਦੂਜਾ);
	}

	// Add connections from "from" to "to", bit-by-bit.
	ਖਾਲੀ ਜੋੜੋ(ਅਚਲ RTLIL::SigSpec& from, ਅਚਲ RTLIL::SigSpec& ਮੰਜ਼ਿਲ)
	{
		log_assert(Get_ਮਾਪ(from) == Get_ਮਾਪ(ਮੰਜ਼ਿਲ));

		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(from); i++)
		{
			ਪੂਰਨ bfi = database.lookup(from[i]);
			ਪੂਰਨ bti = database.lookup(ਮੰਜ਼ਿਲ[i]);
			ਜੇ (bfi == bti) {
				ਜਾਰੀ;
			}

			ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &bf = database[bfi];
			ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &bt = database[bti];
			ਜੇ (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				ਮਿਲਾਓ_ਮੁੱਲ(bt, bf);
			} ਨਹੀਂਤਾਂ {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				ਮਿਲਾਓ_ਮੁੱਲ(bf, bt);
			}
		}
	}

	ਖਾਲੀ ਜੋੜੋ_Val(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ, ਅਚਲ Val &val)
	{
		ਮੁੱਲਾਂ[database.ਲੱਭੋ(ਦੋਆਧਾਰੀ_ਅੰਕ)] |= val;
	}

	ਖਾਲੀ ਜੋੜੋ_Val(ਅਚਲ RTLIL::SigSpec &sig, ਅਚਲ Val &val)
	{
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੋੜੋ_Val(ਦੋਆਧਾਰੀ_ਅੰਕ, val);
	}

	Val apply_and_get_ਮੁੱਲ(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
	{
		ਦੋਆਧਾਰੀ_ਅੰਕ = database.ਲੱਭੋ(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਸਵੈਚਾਲਿਤ it = ਮੁੱਲਾਂ.ਲੱਭੋ(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਵਾਪਸ it == ਮੁੱਲਾਂ.ਸਿਰਾ() ? Val() : it->ਦੂਜਾ;
	}

ਨਿਜੀ:
	ਖਾਲੀ ਮਿਲਾਓ_ਮੁੱਲ(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &from, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਮੰਜ਼ਿਲ)
	{
		ਸਵੈਚਾਲਿਤ it = ਮੁੱਲਾਂ.ਲੱਭੋ(from);
		ਜੇ (it == ਮੁੱਲਾਂ.ਸਿਰਾ()) {
			ਵਾਪਸ;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->ਦੂਜਾ;
		ਮੁੱਲਾਂ.erase(it);
		ਮੁੱਲਾਂ[ਮੰਜ਼ਿਲ] |= v;
	}
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ /* SIGTOOLS_H */
