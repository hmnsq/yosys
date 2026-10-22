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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SIGTOOLS_H
#ವ್ಯಾಖ್ಯಾನಿಸು SIGTOOLS_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ರಚನೆ SigPool
{
	ರಚನೆ ಬಿಟ್_Def_t : ಸಾರ್ವಜನಿಕ std::pair<RTLIL::Wire*, ಪೂರ್ಣಾಂಕ> {
		ಬಿಟ್_Def_t() : std::pair<RTLIL::Wire*, ಪೂರ್ಣಾಂಕ>(ಖಾಲಿ, 0) { }
		ಬಿಟ್_Def_t(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್) : std::pair<RTLIL::Wire*, ಪೂರ್ಣಾಂಕ>(ಬಿಟ್.wire, ಬಿಟ್.ಸ್ಥಾನಾಂತರ) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
			h.eat(ಮೊದಲ->name);
			h.eat(ಎರಡನೇ);
			ಹಿಂತಿರುಗಿ h;
		}
	};

	pool<ಬಿಟ್_Def_t> ಬಿಟ್ಟುಗಳು;

	ಶೂನ್ಯ clear()
	{
		ಬಿಟ್ಟುಗಳು.clear();
	}

	ಶೂನ್ಯ ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ)
				ಬಿಟ್ಟುಗಳು.ಸೇರಿಸು(ಬಿಟ್);
	}

	ಶೂನ್ಯ ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ SigPool &other)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : other.ಬಿಟ್ಟುಗಳು)
			ಬಿಟ್ಟುಗಳು.ಸೇರಿಸು(ಬಿಟ್);
	}

	ಶೂನ್ಯ del(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ)
				ಬಿಟ್ಟುಗಳು.erase(ಬಿಟ್);
	}

	ಶೂನ್ಯ del(ಸ್ಥಿರಾಂಕ SigPool &other)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : other.ಬಿಟ್ಟುಗಳು)
			ಬಿಟ್ಟುಗಳು.erase(ಬಿಟ್);
	}

	ಶೂನ್ಯ expand(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &from, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ಗಮ್ಯ)
	{
		log_assert(Get_ಗಾತ್ರ(from) == Get_ಗಾತ್ರ(ಗಮ್ಯ));
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(from); i++) {
			ಬಿಟ್_Def_t ಬಿಟ್_from(from[i]), ಬಿಟ್_ಗಮ್ಯ(ಗಮ್ಯ[i]);
			ಆದರೆ (ಬಿಟ್_from.ಮೊದಲ != ಖಾಲಿ && ಬಿಟ್_ಗಮ್ಯ.ಮೊದಲ != ಖಾಲಿ && ಬಿಟ್ಟುಗಳು.ಎಣಿಕೆ_2(ಬಿಟ್_from) > 0)
				ಬಿಟ್ಟುಗಳು.ಸೇರಿಸು(ಬಿಟ್_ಗಮ್ಯ);
		}
	}

	RTLIL::SigSpec ಹೊರತೆಗೆ(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig) ಸ್ಥಿರಾಂಕ
	{
		RTLIL::SigSpec ಫಲಿತಾಂಶ;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ && ಬಿಟ್ಟುಗಳು.ಎಣಿಕೆ_2(ಬಿಟ್))
				ಫಲಿತಾಂಶ.append(ಬಿಟ್);
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	RTLIL::SigSpec remove(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig) ಸ್ಥಿರಾಂಕ
	{
		RTLIL::SigSpec ಫಲಿತಾಂಶ;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ && ಬಿಟ್ಟುಗಳು.ಎಣಿಕೆ_2(ಬಿಟ್) == 0)
				ಫಲಿತಾಂಶ.append(ಬಿಟ್);
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	bool check(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ ಬಿಟ್.wire != ಖಾಲಿ && ಬಿಟ್ಟುಗಳು.ಎಣಿಕೆ_2(ಬಿಟ್);
	}

	bool check_any(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig) ಸ್ಥಿರಾಂಕ
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ && ಬಿಟ್ಟುಗಳು.ಎಣಿಕೆ_2(ಬಿಟ್))
				ಹಿಂತಿರುಗಿ true;
		ಹಿಂತಿರುಗಿ false;
	}

	bool check_all(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig) ಸ್ಥಿರಾಂಕ
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ && ಬಿಟ್ಟುಗಳು.ಎಣಿಕೆ_2(ಬಿಟ್) == 0)
				ಹಿಂತಿರುಗಿ false;
		ಹಿಂತಿರುಗಿ true;
	}

	RTLIL::SigSpec export_one() ಸ್ಥಿರಾಂಕ
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : ಬಿಟ್ಟುಗಳು)
			ಹಿಂತಿರುಗಿ RTLIL::SigSpec(ಬಿಟ್.ಮೊದಲ, ಬಿಟ್.ಎರಡನೇ);
		ಹಿಂತಿರುಗಿ RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ಸ್ಥಿರಾಂಕ
	{
		pool<RTLIL::Sig_ಬಿಟ್> sig;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : ಬಿಟ್ಟುಗಳು)
			sig.ಸೇರಿಸು(RTLIL::Sig_ಬಿಟ್(ಬಿಟ್.ಮೊದಲ, ಬಿಟ್.ಎರಡನೇ));
		ಹಿಂತಿರುಗಿ sig;
	}

	ಗಾತ್ರ_t ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು.ಗಾತ್ರ_2();
	}
};

ಮಾದರಿ <typename T, class Compare = ಶೂನ್ಯ>
ರಚನೆ Sig_ಗಣ
{
	static_assert(!std::is_same<Compare,ಶೂನ್ಯ>::ಮೌಲ್ಯ, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	ರಚನೆ ಬಿಟ್_Def_t : ಸಾರ್ವಜನಿಕ std::pair<RTLIL::Wire*, ಪೂರ್ಣಾಂಕ> {
		ಬಿಟ್_Def_t() : std::pair<RTLIL::Wire*, ಪೂರ್ಣಾಂಕ>(ಖಾಲಿ, 0) { }
		ಬಿಟ್_Def_t(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್) : std::pair<RTLIL::Wire*, ಪೂರ್ಣಾಂಕ>(ಬಿಟ್.wire, ಬಿಟ್.ಸ್ಥಾನಾಂತರ) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
			h.eat(ಮೊದಲ->name);
			h.eat(ಎರಡನೇ);
			ಹಿಂತಿರುಗಿ h;
		}
	};

	dict<ಬಿಟ್_Def_t, std::ಗಣ<T, Compare>> ಬಿಟ್ಟುಗಳು;

	ಶೂನ್ಯ clear()
	{
		ಬಿಟ್ಟುಗಳು.clear();
	}

	ಶೂನ್ಯ ಸೇರಿಸು(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig, T ದತ್ತಾಂಶ)
	{
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ)
				ಬಿಟ್ಟುಗಳು[ಬಿಟ್].ಸೇರಿಸು(ದತ್ತಾಂಶ);
	}

	ಶೂನ್ಯ ಸೇರಿಸು(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec& sig, ಸ್ಥಿರಾಂಕ std::ಗಣ<T> &ದತ್ತಾಂಶ)
	{
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ)
				ಬಿಟ್ಟುಗಳು[ಬಿಟ್].ಸೇರಿಸು(ದತ್ತಾಂಶ.begin(), ದತ್ತಾಂಶ.ಕೊನೆ());
	}

	ಶೂನ್ಯ erase(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec& sig)
	{
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ)
				ಬಿಟ್ಟುಗಳು[ಬಿಟ್].clear();
	}

	ಶೂನ್ಯ erase(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig, T ದತ್ತಾಂಶ)
	{
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ)
				ಬಿಟ್ಟುಗಳು[ಬಿಟ್].erase(ದತ್ತಾಂಶ);
	}

	ಶೂನ್ಯ erase(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig, ಸ್ಥಿರಾಂಕ std::ಗಣ<T> &ದತ್ತಾಂಶ)
	{
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ)
				ಬಿಟ್ಟುಗಳು[ಬಿಟ್].erase(ದತ್ತಾಂಶ.begin(), ದತ್ತಾಂಶ.ಕೊನೆ());
	}

	ಶೂನ್ಯ ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig, std::ಗಣ<T> &ಫಲಿತಾಂಶ)
	{
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ) {
				ಸ್ವಯಂಚಾಲಿತ &ದತ್ತಾಂಶ = ಬಿಟ್ಟುಗಳು[ಬಿಟ್];
				ಫಲಿತಾಂಶ.ಸೇರಿಸು(ದತ್ತಾಂಶ.begin(), ದತ್ತಾಂಶ.ಕೊನೆ());
			}
	}

	ಶೂನ್ಯ ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig, pool<T> &ಫಲಿತಾಂಶ)
	{
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ) {
				ಸ್ವಯಂಚಾಲಿತ &ದತ್ತಾಂಶ = ಬಿಟ್ಟುಗಳು[ಬಿಟ್];
				ಫಲಿತಾಂಶ.ಸೇರಿಸು(ದತ್ತಾಂಶ.begin(), ದತ್ತಾಂಶ.ಕೊನೆ());
			}
	}

	std::ಗಣ<T> ಕಂಡುಹಿಡಿ(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig)
	{
		std::ಗಣ<T> ಫಲಿತಾಂಶ;
		ಕಂಡುಹಿಡಿ(sig, ಫಲಿತಾಂಶ);
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	bool has(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ && ಬಿಟ್ಟುಗಳು.ಎಣಿಕೆ_2(ಬಿಟ್))
				ಹಿಂತಿರುಗಿ true;
		ಹಿಂತಿರುಗಿ false;
	}
};

ಮಾದರಿ<typename T>
class Sig_ಗಣ<T, typename std::enable_if<!std::is_ಸೂಚಕ<T>::ಮೌಲ್ಯ>::type> : ಸಾರ್ವಜನಿಕ Sig_ಗಣ<T, std::less<T>> {};
ಮಾದರಿ<typename T>
using ವಿಂಗಡಿಸು_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::ಮೌಲ್ಯ>::type;
ಮಾದರಿ<typename T>
class Sig_ಗಣ<T, ವಿಂಗಡಿಸು_by_name_id_guard<T>> : ಸಾರ್ವಜನಿಕ Sig_ಗಣ<T, RTLIL::ವಿಂಗಡಿಸು_by_name_id<typename std::remove_ಸೂಚಕ<T>::type>> {};

ರಚನೆ SigMapView
{
	mfp<Sig_ಬಿಟ್> database;

	// Modify bit to its representative
	ಶೂನ್ಯ apply(RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ
	{
		ಬಿಟ್ = database.ಕಂಡುಹಿಡಿ(ಬಿಟ್);
	}

	ಶೂನ್ಯ apply(RTLIL::SigSpec &sig) ಸ್ಥಿರಾಂಕ
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			apply(ಬಿಟ್);
	}

	RTLIL::Sig_ಬಿಟ್ ಕ್ರಿಯಾಕಾರಕ()(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್) ಸ್ಥಿರಾಂಕ
	{
		apply(ಬಿಟ್);
		ಹಿಂತಿರುಗಿ ಬಿಟ್;
	}

	RTLIL::SigSpec ಕ್ರಿಯಾಕಾರಕ()(RTLIL::SigSpec sig) ಸ್ಥಿರಾಂಕ
	{
		apply(sig);
		ಹಿಂತಿರುಗಿ sig;
	}

	RTLIL::SigSpec ಕ್ರಿಯಾಕಾರಕ()(RTLIL::Wire *wire) ಸ್ಥಿರಾಂಕ
	{
		SigSpec sig(wire);
		apply(sig);
		ಹಿಂತಿರುಗಿ sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
ರಚನೆ SigMap final : ಸಾರ್ವಜನಿಕ SigMapView
{
	SigMap(RTLIL::Module *module = ಖಾಲಿ)
	{
		ಆದರೆ (module != ಖಾಲಿ)
			ಗಣ(module);
	}

	ಶೂನ್ಯ ಅದಲುಬದಲು(SigMap &other)
	{
		database.ಅದಲುಬದಲು(other.database);
	}

	ಶೂನ್ಯ clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	ಶೂನ್ಯ ಗಣ(RTLIL::Module *module)
	{
		ಪೂರ್ಣಾಂಕ bitcount = 0;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : module->connections())
			bitcount += it.ಮೊದಲ.ಗಾತ್ರ_2();

		database.clear();
		database.reserve(bitcount);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : module->connections())
			ಕೂಡಿಸು(it.ಮೊದಲ, it.ಎರಡನೇ);
	}

	// Add connections from "from" to "to", bit-by-bit
	ಶೂನ್ಯ ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec& from, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec& ಗಮ್ಯ)
	{
		log_assert(Get_ಗಾತ್ರ(from) == Get_ಗಾತ್ರ(ಗಮ್ಯ));

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(from); i++)
		{
			ಪೂರ್ಣಾಂಕ bfi = database.lookup(from[i]);
			ಪೂರ್ಣಾಂಕ bti = database.lookup(ಗಮ್ಯ[i]);

			ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &bf = database[bfi];
			ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &bt = database[bti];

			ಆದರೆ (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				ಆದರೆ (bf.wire == nullptr)
					database.ipromote(bfi);

				ಆದರೆ (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	ಶೂನ್ಯ ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್)
	{
		ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &b = database.ಕಂಡುಹಿಡಿ(ಬಿಟ್);
		ಆದರೆ (b.wire != nullptr)
			database.promote(ಬಿಟ್);
	}

	ಶೂನ್ಯ ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig)
	{
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಕೂಡಿಸು(ಬಿಟ್);
	}

	ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಕೂಡಿಸು(Wire *wire) { ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ಸ್ಥಿರಾಂಕ
	{
		RTLIL::SigSpec sig;
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : database)
			ಆದರೆ (ಬಿಟ್.wire != nullptr)
				sig.append(ಬಿಟ್);
		ಹಿಂತಿರುಗಿ sig;
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
ಮಾದರಿ <class Val>
ರಚನೆ SigValMap final : ಸಾರ್ವಜನಿಕ SigMapView
{
	dict<Sig_ಬಿಟ್, Val> ಮೌಲ್ಯಗಳು;

	ಶೂನ್ಯ ಅದಲುಬದಲು(SigValMap<Val> &other)
	{
		database.ಅದಲುಬದಲು(other.database);
		ಮೌಲ್ಯಗಳು.ಅದಲುಬದಲು(other.ಮೌಲ್ಯಗಳು);
	}

	ಶೂನ್ಯ clear()
	{
		database.clear();
		ಮೌಲ್ಯಗಳು.clear();
	}

	// Rebuild SigMap for all connections in module
	ಶೂನ್ಯ ಗಣ(RTLIL::Module *module)
	{
		ಪೂರ್ಣಾಂಕ bitcount = 0;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : module->connections())
			bitcount += it.ಮೊದಲ.ಗಾತ್ರ_2();

		database.clear();
		ಮೌಲ್ಯಗಳು.clear();
		database.reserve(bitcount);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : module->connections())
			ಕೂಡಿಸು(it.ಮೊದಲ, it.ಎರಡನೇ);
	}

	// Add connections from "from" to "to", bit-by-bit.
	ಶೂನ್ಯ ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec& from, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec& ಗಮ್ಯ)
	{
		log_assert(Get_ಗಾತ್ರ(from) == Get_ಗಾತ್ರ(ಗಮ್ಯ));

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(from); i++)
		{
			ಪೂರ್ಣಾಂಕ bfi = database.lookup(from[i]);
			ಪೂರ್ಣಾಂಕ bti = database.lookup(ಗಮ್ಯ[i]);
			ಆದರೆ (bfi == bti) {
				ಮುಂದುವರಿ;
			}

			ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &bf = database[bfi];
			ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &bt = database[bti];
			ಆದರೆ (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				ವಿಲೀನ_ಮೌಲ್ಯ(bt, bf);
			} ಇಲ್ಲದಿದ್ದರೆ {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				ವಿಲೀನ_ಮೌಲ್ಯ(bf, bt);
			}
		}
	}

	ಶೂನ್ಯ ಕೂಡಿಸು_Val(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್, ಸ್ಥಿರಾಂಕ Val &val)
	{
		ಮೌಲ್ಯಗಳು[database.ಕಂಡುಹಿಡಿ(ಬಿಟ್)] |= val;
	}

	ಶೂನ್ಯ ಕೂಡಿಸು_Val(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig, ಸ್ಥಿರಾಂಕ Val &val)
	{
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಕೂಡಿಸು_Val(ಬಿಟ್, val);
	}

	Val apply_and_get_ಮೌಲ್ಯ(RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್) ಸ್ಥಿರಾಂಕ
	{
		ಬಿಟ್ = database.ಕಂಡುಹಿಡಿ(ಬಿಟ್);
		ಸ್ವಯಂಚಾಲಿತ it = ಮೌಲ್ಯಗಳು.ಕಂಡುಹಿಡಿ(ಬಿಟ್);
		ಹಿಂತಿರುಗಿ it == ಮೌಲ್ಯಗಳು.ಕೊನೆ() ? Val() : it->ಎರಡನೇ;
	}

ಖಾಸಗಿ:
	ಶೂನ್ಯ ವಿಲೀನ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &from, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಗಮ್ಯ)
	{
		ಸ್ವಯಂಚಾಲಿತ it = ಮೌಲ್ಯಗಳು.ಕಂಡುಹಿಡಿ(from);
		ಆದರೆ (it == ಮೌಲ್ಯಗಳು.ಕೊನೆ()) {
			ಹಿಂತಿರುಗಿ;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->ಎರಡನೇ;
		ಮೌಲ್ಯಗಳು.erase(it);
		ಮೌಲ್ಯಗಳು[ಗಮ್ಯ] |= v;
	}
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ /* SIGTOOLS_H */
