#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ OPT_DFF_COMP_H
#ವ್ಯಾಖ್ಯಾನಿಸು OPT_DFF_COMP_H

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::map<RTLIL::Sig_ಬಿಟ್, bool> ನಮೂನೆ_t; // Set of control signals that must ALL match required vals
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::ಗಣ<ನಮೂನೆ_t> patterns_t;          // Alternative patterns (OR)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::pair<RTLIL::Sig_ಬಿಟ್, bool> ctrl_t;   // Control signal
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::ಗಣ<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
ಸಾಲೊಳಗೆ std::optional<RTLIL::Sig_ಬಿಟ್> ಕಂಡುಹಿಡಿ_complementary_ನಮೂನೆ_var(
	ಸ್ಥಿರಾಂಕ ನಮೂನೆ_t& ಎಡ_3,
	ಸ್ಥಿರಾಂಕ ನಮೂನೆ_t& ಬಲ_3
) {
	std::optional<RTLIL::Sig_ಬಿಟ್> ret;
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &pt : ಎಡ_3) {
		// Left requires signal that right doesn't constrain - incompatible domains
		ಆದರೆ (ಬಲ_3.ಎಣಿಕೆ_2(pt.ಮೊದಲ) == 0)
			ಹಿಂತಿರುಗಿ std::nullopt;
		// Signal has same required value in both - not the complement variable
		ಆದರೆ (ಬಲ_3.at(pt.ಮೊದಲ) == pt.ಎರಡನೇ)
			ಮುಂದುವರಿ;
		// Already found one differing signal, now found another - not simplifiable
		ಆದರೆ (ret)
			ಹಿಂತಿರುಗಿ std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.ಮೊದಲ;
	}
	ಹಿಂತಿರುಗಿ ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
ಸಾಲೊಳಗೆ ಶೂನ್ಯ simplify_patterns(patterns_t& patterns) {
	ಸ್ವಯಂಚಾಲಿತ new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	ಮಾಡು {
		optimized = false;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = patterns.begin(); i != patterns.ಕೊನೆ(); i++) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ j = std::ಮುಂದಿನ(i, 1); j != patterns.ಕೊನೆ(); j++) {
				ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ& ಎಡ_3 = (Get_ಗಾತ್ರ(*j) <= Get_ಗಾತ್ರ(*i)) ? *j : *i;
				ಸ್ವಯಂಚಾಲಿತ ಬಲ_3 = (Get_ಗಾತ್ರ(*i) < Get_ಗಾತ್ರ(*j)) ? *j : *i;
				ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ complementary_var = ಕಂಡುಹಿಡಿ_complementary_ನಮೂನೆ_var(ಎಡ_3, ಬಲ_3);

				ಆದರೆ (complementary_var && new_patterns.ಎಣಿಕೆ_2(ಬಲ_3)) {
					new_patterns.erase(ಬಲ_3);
					ಬಲ_3.erase(complementary_var.ಮೌಲ್ಯ());
					new_patterns.ಸೇರಿಸು(ಬಲ_3);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} ಆಗ(optimized);

	// Remove redundant patterns
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = patterns.begin(); i != patterns.ಕೊನೆ(); ++i) {
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ j = std::ಮುಂದಿನ(i, 1); j != patterns.ಕೊನೆ(); ++j) {
			ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ& ಎಡ_3 = (Get_ಗಾತ್ರ(*j) <= Get_ಗಾತ್ರ(*i)) ? *j : *i;
			ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ& ಬಲ_3 = (Get_ಗಾತ್ರ(*i) < Get_ಗಾತ್ರ(*j)) ? *j : *i;
			bool redundant = true;

			ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ& pt : ಎಡ_3)
				ಆದರೆ (ಬಲ_3.ಎಣಿಕೆ_2(pt.ಮೊದಲ) == 0 || ಬಲ_3.at(pt.ಮೊದಲ) != pt.ಎರಡನೇ)
					redundant = false;
			ಆದರೆ (redundant)
				new_patterns.erase(ಬಲ_3);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
