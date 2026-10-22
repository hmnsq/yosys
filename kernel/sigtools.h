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

#यदि_अपरिभाषितम् SIGTOOLS_H
#परिभाषय SIGTOOLS_H

#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

संरचना SigPool
{
	संरचना bitDef_t : सार्वजनिक std::pair<RTLIL::Wire*, पूर्णाङ्क> {
		bitDef_t() : std::pair<RTLIL::Wire*, पूर्णाङ्क>(रिक्त, 0) { }
		bitDef_t(ध्रुव RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, पूर्णाङ्क>(bit.wire, bit.विस्थापनम्) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(first->name);
			h.eat(second);
			प्रत्यागम h;
		}
	};

	pool<bitDef_t> द्वयङ्काः;

	शून्य clear()
	{
		द्वयङ्काः.clear();
	}

	शून्य सङ्कलय(ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त)
				द्वयङ्काः.निवेशय(bit);
	}

	शून्य सङ्कलय(ध्रुव SigPool &other)
	{
		प्रति (स्वचालित &bit : other.द्वयङ्काः)
			द्वयङ्काः.निवेशय(bit);
	}

	शून्य del(ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त)
				द्वयङ्काः.erase(bit);
	}

	शून्य del(ध्रुव SigPool &other)
	{
		प्रति (स्वचालित &bit : other.द्वयङ्काः)
			द्वयङ्काः.erase(bit);
	}

	शून्य expand(ध्रुव RTLIL::SigSpec &from, ध्रुव RTLIL::SigSpec &गन्तव्यम्)
	{
		log_assert(Get_आकार(from) == Get_आकार(गन्तव्यम्));
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(from); i++) {
			bitDef_t bit_from(from[i]), bit_गन्तव्यम्(गन्तव्यम्[i]);
			यदि (bit_from.first != रिक्त && bit_गन्तव्यम्.first != रिक्त && द्वयङ्काः.गणनम्(bit_from) > 0)
				द्वयङ्काः.निवेशय(bit_गन्तव्यम्);
		}
	}

	RTLIL::SigSpec extract(ध्रुव RTLIL::SigSpec &sig) ध्रुव
	{
		RTLIL::SigSpec परिणाम;
		प्रति (स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त && द्वयङ्काः.गणनम्(bit))
				परिणाम.append(bit);
		प्रत्यागम परिणाम;
	}

	RTLIL::SigSpec remove(ध्रुव RTLIL::SigSpec &sig) ध्रुव
	{
		RTLIL::SigSpec परिणाम;
		प्रति (स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त && द्वयङ्काः.गणनम्(bit) == 0)
				परिणाम.append(bit);
		प्रत्यागम परिणाम;
	}

	bool check(ध्रुव RTLIL::SigBit &bit) ध्रुव
	{
		प्रत्यागम bit.wire != रिक्त && द्वयङ्काः.गणनम्(bit);
	}

	bool check_any(ध्रुव RTLIL::SigSpec &sig) ध्रुव
	{
		प्रति (स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त && द्वयङ्काः.गणनम्(bit))
				प्रत्यागम true;
		प्रत्यागम false;
	}

	bool check_all(ध्रुव RTLIL::SigSpec &sig) ध्रुव
	{
		प्रति (स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त && द्वयङ्काः.गणनम्(bit) == 0)
				प्रत्यागम false;
		प्रत्यागम true;
	}

	RTLIL::SigSpec export_one() ध्रुव
	{
		प्रति (स्वचालित &bit : द्वयङ्काः)
			प्रत्यागम RTLIL::SigSpec(bit.first, bit.second);
		प्रत्यागम RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ध्रुव
	{
		pool<RTLIL::SigBit> sig;
		प्रति (स्वचालित &bit : द्वयङ्काः)
			sig.निवेशय(RTLIL::SigBit(bit.first, bit.second));
		प्रत्यागम sig;
	}

	आकार_t आकार() ध्रुव
	{
		प्रत्यागम द्वयङ्काः.आकार();
	}
};

प्रतिरूपम् <typename T, class Compare = शून्य>
संरचना Sig_समुच्चयः
{
	static_assert(!std::is_same<Compare,शून्य>::मानम्, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	संरचना bitDef_t : सार्वजनिक std::pair<RTLIL::Wire*, पूर्णाङ्क> {
		bitDef_t() : std::pair<RTLIL::Wire*, पूर्णाङ्क>(रिक्त, 0) { }
		bitDef_t(ध्रुव RTLIL::SigBit &bit) : std::pair<RTLIL::Wire*, पूर्णाङ्क>(bit.wire, bit.विस्थापनम्) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			h.eat(first->name);
			h.eat(second);
			प्रत्यागम h;
		}
	};

	dict<bitDef_t, std::समुच्चयः<T, Compare>> द्वयङ्काः;

	शून्य clear()
	{
		द्वयङ्काः.clear();
	}

	शून्य निवेशय(ध्रुव RTLIL::SigSpec &sig, T दत्त)
	{
		प्रति (ध्रुव स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त)
				द्वयङ्काः[bit].निवेशय(दत्त);
	}

	शून्य निवेशय(ध्रुव RTLIL::SigSpec& sig, ध्रुव std::समुच्चयः<T> &दत्त)
	{
		प्रति (ध्रुव स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त)
				द्वयङ्काः[bit].निवेशय(दत्त.begin(), दत्त.अन्तः());
	}

	शून्य erase(ध्रुव RTLIL::SigSpec& sig)
	{
		प्रति (ध्रुव स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त)
				द्वयङ्काः[bit].clear();
	}

	शून्य erase(ध्रुव RTLIL::SigSpec &sig, T दत्त)
	{
		प्रति (ध्रुव स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त)
				द्वयङ्काः[bit].erase(दत्त);
	}

	शून्य erase(ध्रुव RTLIL::SigSpec &sig, ध्रुव std::समुच्चयः<T> &दत्त)
	{
		प्रति (ध्रुव स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त)
				द्वयङ्काः[bit].erase(दत्त.begin(), दत्त.अन्तः());
	}

	शून्य find(ध्रुव RTLIL::SigSpec &sig, std::समुच्चयः<T> &परिणाम)
	{
		प्रति (ध्रुव स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त) {
				स्वचालित &दत्त = द्वयङ्काः[bit];
				परिणाम.निवेशय(दत्त.begin(), दत्त.अन्तः());
			}
	}

	शून्य find(ध्रुव RTLIL::SigSpec &sig, pool<T> &परिणाम)
	{
		प्रति (ध्रुव स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त) {
				स्वचालित &दत्त = द्वयङ्काः[bit];
				परिणाम.निवेशय(दत्त.begin(), दत्त.अन्तः());
			}
	}

	std::समुच्चयः<T> find(ध्रुव RTLIL::SigSpec &sig)
	{
		std::समुच्चयः<T> परिणाम;
		find(sig, परिणाम);
		प्रत्यागम परिणाम;
	}

	bool has(ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (स्वचालित &bit : sig)
			यदि (bit.wire != रिक्त && द्वयङ्काः.गणनम्(bit))
				प्रत्यागम true;
		प्रत्यागम false;
	}
};

प्रतिरूपम्<typename T>
class Sig_समुच्चयः<T, typename std::enable_if<!std::is_pointer<T>::मानम्>::type> : सार्वजनिक Sig_समुच्चयः<T, std::less<T>> {};
प्रतिरूपम्<typename T>
using क्रमय_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::मानम्>::type;
प्रतिरूपम्<typename T>
class Sig_समुच्चयः<T, क्रमय_by_name_id_guard<T>> : सार्वजनिक Sig_समुच्चयः<T, RTLIL::क्रमय_by_name_id<typename std::remove_pointer<T>::type>> {};

संरचना SigMapView
{
	mfp<SigBit> database;

	// Modify bit to its representative
	शून्य apply(RTLIL::SigBit &bit) ध्रुव
	{
		bit = database.find(bit);
	}

	शून्य apply(RTLIL::SigSpec &sig) ध्रुव
	{
		प्रति (स्वचालित &bit : sig)
			apply(bit);
	}

	RTLIL::SigBit संक्रियकः()(RTLIL::SigBit bit) ध्रुव
	{
		apply(bit);
		प्रत्यागम bit;
	}

	RTLIL::SigSpec संक्रियकः()(RTLIL::SigSpec sig) ध्रुव
	{
		apply(sig);
		प्रत्यागम sig;
	}

	RTLIL::SigSpec संक्रियकः()(RTLIL::Wire *wire) ध्रुव
	{
		SigSpec sig(wire);
		apply(sig);
		प्रत्यागम sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
संरचना SigMap final : सार्वजनिक SigMapView
{
	SigMap(RTLIL::Module *module = रिक्त)
	{
		यदि (module != रिक्त)
			समुच्चयः(module);
	}

	शून्य swap(SigMap &other)
	{
		database.swap(other.database);
	}

	शून्य clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	शून्य समुच्चयः(RTLIL::Module *module)
	{
		पूर्णाङ्क bitcount = 0;
		प्रति (स्वचालित &it : module->connections())
			bitcount += it.first.आकार();

		database.clear();
		database.reserve(bitcount);

		प्रति (स्वचालित &it : module->connections())
			सङ्कलय(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit
	शून्य सङ्कलय(ध्रुव RTLIL::SigSpec& from, ध्रुव RTLIL::SigSpec& गन्तव्यम्)
	{
		log_assert(Get_आकार(from) == Get_आकार(गन्तव्यम्));

		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(from); i++)
		{
			पूर्णाङ्क bfi = database.lookup(from[i]);
			पूर्णाङ्क bti = database.lookup(गन्तव्यम्[i]);

			ध्रुव RTLIL::SigBit &bf = database[bfi];
			ध्रुव RTLIL::SigBit &bt = database[bti];

			यदि (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				यदि (bf.wire == nullptr)
					database.ipromote(bfi);

				यदि (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	शून्य सङ्कलय(ध्रुव RTLIL::SigBit &bit)
	{
		ध्रुव स्वचालित &b = database.find(bit);
		यदि (b.wire != nullptr)
			database.promote(bit);
	}

	शून्य सङ्कलय(ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (ध्रुव स्वचालित &bit : sig)
			सङ्कलय(bit);
	}

	अन्तःस्थ शून्य सङ्कलय(Wire *wire) { प्रत्यागम सङ्कलय(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ध्रुव
	{
		RTLIL::SigSpec sig;
		प्रति (ध्रुव स्वचालित &bit : database)
			यदि (bit.wire != nullptr)
				sig.append(bit);
		प्रत्यागम sig;
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
प्रतिरूपम् <class Val>
संरचना SigValMap final : सार्वजनिक SigMapView
{
	dict<SigBit, Val> मानानि;

	शून्य swap(SigValMap<Val> &other)
	{
		database.swap(other.database);
		मानानि.swap(other.मानानि);
	}

	शून्य clear()
	{
		database.clear();
		मानानि.clear();
	}

	// Rebuild SigMap for all connections in module
	शून्य समुच्चयः(RTLIL::Module *module)
	{
		पूर्णाङ्क bitcount = 0;
		प्रति (स्वचालित &it : module->connections())
			bitcount += it.first.आकार();

		database.clear();
		मानानि.clear();
		database.reserve(bitcount);

		प्रति (स्वचालित &it : module->connections())
			सङ्कलय(it.first, it.second);
	}

	// Add connections from "from" to "to", bit-by-bit.
	शून्य सङ्कलय(ध्रुव RTLIL::SigSpec& from, ध्रुव RTLIL::SigSpec& गन्तव्यम्)
	{
		log_assert(Get_आकार(from) == Get_आकार(गन्तव्यम्));

		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(from); i++)
		{
			पूर्णाङ्क bfi = database.lookup(from[i]);
			पूर्णाङ्क bti = database.lookup(गन्तव्यम्[i]);
			यदि (bfi == bti) {
				अनुवर्त;
			}

			ध्रुव RTLIL::SigBit &bf = database[bfi];
			ध्रुव RTLIL::SigBit &bt = database[bti];
			यदि (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				merge_मानम्(bt, bf);
			} अन्यथा {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				merge_मानम्(bf, bt);
			}
		}
	}

	शून्य सङ्कलय_Val(ध्रुव RTLIL::SigBit &bit, ध्रुव Val &val)
	{
		मानानि[database.find(bit)] |= val;
	}

	शून्य सङ्कलय_Val(ध्रुव RTLIL::SigSpec &sig, ध्रुव Val &val)
	{
		प्रति (ध्रुव स्वचालित &bit : sig)
			सङ्कलय_Val(bit, val);
	}

	Val apply_and_get_मानम्(RTLIL::SigBit &bit) ध्रुव
	{
		bit = database.find(bit);
		स्वचालित it = मानानि.find(bit);
		प्रत्यागम it == मानानि.अन्तः() ? Val() : it->second;
	}

गुप्त:
	शून्य merge_मानम्(ध्रुव RTLIL::SigBit &from, ध्रुव RTLIL::SigBit &गन्तव्यम्)
	{
		स्वचालित it = मानानि.find(from);
		यदि (it == मानानि.अन्तः()) {
			प्रत्यागम;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->second;
		मानानि.erase(it);
		मानानि[गन्तव्यम्] |= v;
	}
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः /* SIGTOOLS_H */
