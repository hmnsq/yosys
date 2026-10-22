#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ OPT_DFF_COMP_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OPT_DFF_COMP_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/rtlil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <map>
#ਅਨ੍ਤਰ੍ਭਾਵ <optional>

YOSYS_NAMESPACE_BEGIN

/**
 *  Pattern matching utilities for control signal analysis.
 *
 *  A pattern_t maps control signals to required values, representing a
 *  product term (conjunction): {A=1, B=0} means "A AND !B".
 *
 *  A patterns_t is a set of patterns representing a sum-of-products:
 *  {{A=1, B=0}, {A=0, C=1}} means "(A AND !B) OR (!A AND C)".
 *
 *  Used for analyzing MUX tree control paths in DFF optimization.
 */

// Pattern matching for clock enable
// A pattern maps control signals to their required values for a MUX path
ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::map<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, bool> ਨਮੂਨਾ_t; // Set of control signals that must ALL match required vals
ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::ਸੈੱਟ<ਨਮੂਨਾ_t> patterns_t;          // Alternative patterns (OR)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::pair<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, bool> ctrl_t;   // Control signal
ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::ਸੈੱਟ<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
ਸਤਰਅੰਦਰ std::optional<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਲੱਭੋ_complementary_ਨਮੂਨਾ_var(
	ਅਚਲ ਨਮੂਨਾ_t& ਖੱਬਾ_2,
	ਅਚਲ ਨਮੂਨਾ_t& ਸੱਜਾ_2
) {
	std::optional<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ret;
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &pt : ਖੱਬਾ_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		ਜੇ (ਸੱਜਾ_2.ਗਿਣਤੀ_2(pt.ਪਹਿਲਾ) == 0)
			ਵਾਪਸ std::nullopt;
		// Signal has same required value in both - not the complement variable
		ਜੇ (ਸੱਜਾ_2.at(pt.ਪਹਿਲਾ) == pt.ਦੂਜਾ)
			ਜਾਰੀ;
		// Already found one differing signal, now found another - not simplifiable
		ਜੇ (ret)
			ਵਾਪਸ std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.ਪਹਿਲਾ;
	}
	ਵਾਪਸ ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
ਸਤਰਅੰਦਰ ਖਾਲੀ simplify_patterns(patterns_t& patterns) {
	ਸਵੈਚਾਲਿਤ new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	ਕਰੋ {
		optimized = false;
		ਲਈ (ਸਵੈਚਾਲਿਤ i = patterns.begin(); i != patterns.ਸਿਰਾ(); i++) {
			ਲਈ (ਸਵੈਚਾਲਿਤ j = std::ਅਗਲਾ(i, 1); j != patterns.ਸਿਰਾ(); j++) {
				ਅਚਲ ਸਵੈਚਾਲਿਤ& ਖੱਬਾ_2 = (Get_ਮਾਪ(*j) <= Get_ਮਾਪ(*i)) ? *j : *i;
				ਸਵੈਚਾਲਿਤ ਸੱਜਾ_2 = (Get_ਮਾਪ(*i) < Get_ਮਾਪ(*j)) ? *j : *i;
				ਅਚਲ ਸਵੈਚਾਲਿਤ complementary_var = ਲੱਭੋ_complementary_ਨਮੂਨਾ_var(ਖੱਬਾ_2, ਸੱਜਾ_2);

				ਜੇ (complementary_var && new_patterns.ਗਿਣਤੀ_2(ਸੱਜਾ_2)) {
					new_patterns.erase(ਸੱਜਾ_2);
					ਸੱਜਾ_2.erase(complementary_var.ਮੁੱਲ());
					new_patterns.ਪਾਓ(ਸੱਜਾ_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} ਜਦੋਂ(optimized);

	// Remove redundant patterns
	ਲਈ (ਸਵੈਚਾਲਿਤ i = patterns.begin(); i != patterns.ਸਿਰਾ(); ++i) {
		ਲਈ (ਸਵੈਚਾਲਿਤ j = std::ਅਗਲਾ(i, 1); j != patterns.ਸਿਰਾ(); ++j) {
			ਅਚਲ ਸਵੈਚਾਲਿਤ& ਖੱਬਾ_2 = (Get_ਮਾਪ(*j) <= Get_ਮਾਪ(*i)) ? *j : *i;
			ਅਚਲ ਸਵੈਚਾਲਿਤ& ਸੱਜਾ_2 = (Get_ਮਾਪ(*i) < Get_ਮਾਪ(*j)) ? *j : *i;
			bool redundant = true;

			ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ& pt : ਖੱਬਾ_2)
				ਜੇ (ਸੱਜਾ_2.ਗਿਣਤੀ_2(pt.ਪਹਿਲਾ) == 0 || ਸੱਜਾ_2.at(pt.ਪਹਿਲਾ) != pt.ਦੂਜਾ)
					redundant = false;
			ਜੇ (redundant)
				new_patterns.erase(ਸੱਜਾ_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
