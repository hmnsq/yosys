#ifndef OPT_DFF_COMP_H
#define OPT_DFF_COMP_H

#ಅನ್ತರ್ಭಾವ "kernel/rtlil.h"
#ಅನ್ತರ್ಭಾವ <map>
#ಅನ್ತರ್ಭಾವ <optional>

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
ಪ್ರಕಾರನಿರ್ದೇಶ std::map<RTLIL::SigBit, bool> pattern_t; // Set of control signals that must ALL match required vals
ಪ್ರಕಾರನಿರ್ದೇಶ std::set<pattern_t> patterns_t;          // Alternative patterns (OR)
ಪ್ರಕಾರನಿರ್ದೇಶ std::pair<RTLIL::SigBit, bool> ctrl_t;   // Control signal
ಪ್ರಕಾರನಿರ್ದೇಶ std::set<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
ಅನ್ತಃಸ್ಥ std::optional<RTLIL::SigBit> find_complementary_pattern_var(
	ಧ್ರುವ pattern_t& ಎಡ_2,
	ಧ್ರುವ pattern_t& ಬಲ_2
) {
	std::optional<RTLIL::SigBit> ret;
	ಪ್ರತಿ (ಧ್ರುವ auto &pt : ಎಡ_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		ಯದಿ (ಬಲ_2.ಎಣಿಕೆ(pt.first) == 0)
			ಪ್ರತ್ಯಾಗಮ std::nullopt;
		// Signal has same required value in both - not the complement variable
		ಯದಿ (ಬಲ_2.at(pt.first) == pt.second)
			ಅನುವರ್ತ;
		// Already found one differing signal, now found another - not simplifiable
		ಯದಿ (ret)
			ಪ್ರತ್ಯಾಗಮ std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.first;
	}
	ಪ್ರತ್ಯಾಗಮ ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ simplify_patterns(patterns_t& patterns) {
	auto new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	ಕುರು {
		optimized = false;
		ಪ್ರತಿ (auto i = patterns.begin(); i != patterns.end(); i++) {
			ಪ್ರತಿ (auto j = std::next(i, 1); j != patterns.end(); j++) {
				ಧ್ರುವ auto& ಎಡ_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
				auto ಬಲ_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
				ಧ್ರುವ auto complementary_var = find_complementary_pattern_var(ಎಡ_2, ಬಲ_2);

				ಯದಿ (complementary_var && new_patterns.ಎಣಿಕೆ(ಬಲ_2)) {
					new_patterns.erase(ಬಲ_2);
					ಬಲ_2.erase(complementary_var.ಮೌಲ್ಯ());
					new_patterns.ಸೇರಿಸು(ಬಲ_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} ಯಾವತ್(optimized);

	// Remove redundant patterns
	ಪ್ರತಿ (auto i = patterns.begin(); i != patterns.end(); ++i) {
		ಪ್ರತಿ (auto j = std::next(i, 1); j != patterns.end(); ++j) {
			ಧ್ರುವ auto& ಎಡ_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
			ಧ್ರುವ auto& ಬಲ_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
			bool redundant = true;

			ಪ್ರತಿ (ಧ್ರುವ auto& pt : ಎಡ_2)
				ಯದಿ (ಬಲ_2.ಎಣಿಕೆ(pt.first) == 0 || ಬಲ_2.at(pt.first) != pt.second)
					redundant = false;
			ಯದಿ (redundant)
				new_patterns.erase(ಬಲ_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_END

#endif
