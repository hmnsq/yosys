#ifndef OPT_DFF_COMP_H
#define OPT_DFF_COMP_H

#अन्तर्भाव "kernel/rtlil.h"
#अन्तर्भाव <map>
#अन्तर्भाव <optional>

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
प्रकारव्याख्या std::map<RTLIL::SigBit, bool> pattern_t; // Set of control signals that must ALL match required vals
प्रकारव्याख्या std::set<pattern_t> patterns_t;          // Alternative patterns (OR)
प्रकारव्याख्या std::pair<RTLIL::SigBit, bool> ctrl_t;   // Control signal
प्रकारव्याख्या std::set<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
ओळीत std::optional<RTLIL::SigBit> find_complementary_pattern_var(
	स्थिरांक pattern_t& डावा_2,
	स्थिरांक pattern_t& उजवा_2
) {
	std::optional<RTLIL::SigBit> ret;
	साठी (स्थिरांक स्वतः &pt : डावा_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		जर (उजवा_2.मोजणी(pt.first) == 0)
			परत std::nullopt;
		// Signal has same required value in both - not the complement variable
		जर (उजवा_2.at(pt.first) == pt.second)
			सुरूठेवा;
		// Already found one differing signal, now found another - not simplifiable
		जर (ret)
			परत std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.first;
	}
	परत ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
ओळीत शून्य simplify_patterns(patterns_t& patterns) {
	स्वतः new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	करा {
		optimized = false;
		साठी (स्वतः i = patterns.begin(); i != patterns.end(); i++) {
			साठी (स्वतः j = std::next(i, 1); j != patterns.end(); j++) {
				स्थिरांक स्वतः& डावा_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
				स्वतः उजवा_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
				स्थिरांक स्वतः complementary_var = find_complementary_pattern_var(डावा_2, उजवा_2);

				जर (complementary_var && new_patterns.मोजणी(उजवा_2)) {
					new_patterns.erase(उजवा_2);
					उजवा_2.erase(complementary_var.मूल्य());
					new_patterns.घाला(उजवा_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} पर्यंत(optimized);

	// Remove redundant patterns
	साठी (स्वतः i = patterns.begin(); i != patterns.end(); ++i) {
		साठी (स्वतः j = std::next(i, 1); j != patterns.end(); ++j) {
			स्थिरांक स्वतः& डावा_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
			स्थिरांक स्वतः& उजवा_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
			bool redundant = true;

			साठी (स्थिरांक स्वतः& pt : डावा_2)
				जर (उजवा_2.मोजणी(pt.first) == 0 || उजवा_2.at(pt.first) != pt.second)
					redundant = false;
			जर (redundant)
				new_patterns.erase(उजवा_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_END

#endif
