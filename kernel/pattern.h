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
प्रकारनिर्देश std::map<RTLIL::SigBit, bool> pattern_t; // Set of control signals that must ALL match required vals
प्रकारनिर्देश std::set<pattern_t> patterns_t;          // Alternative patterns (OR)
प्रकारनिर्देश std::pair<RTLIL::SigBit, bool> ctrl_t;   // Control signal
प्रकारनिर्देश std::set<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
अन्तःस्थ std::optional<RTLIL::SigBit> find_complementary_pattern_var(
	ध्रुव pattern_t& वामः_2,
	ध्रुव pattern_t& दक्षिणः_2
) {
	std::optional<RTLIL::SigBit> ret;
	अत्थाय (ध्रुव auto &pt : वामः_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		सचे (दक्षिणः_2.गणनम्(pt.first) == 0)
			पटिनिवत्त std::nullopt;
		// Signal has same required value in both - not the complement variable
		सचे (दक्षिणः_2.at(pt.first) == pt.second)
			अनुवर्त;
		// Already found one differing signal, now found another - not simplifiable
		सचे (ret)
			पटिनिवत्त std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.first;
	}
	पटिनिवत्त ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
अन्तःस्थ शून्य simplify_patterns(patterns_t& patterns) {
	auto new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	करोहि {
		optimized = false;
		अत्थाय (auto i = patterns.begin(); i != patterns.end(); i++) {
			अत्थाय (auto j = std::next(i, 1); j != patterns.end(); j++) {
				ध्रुव auto& वामः_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
				auto दक्षिणः_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
				ध्रुव auto complementary_var = find_complementary_pattern_var(वामः_2, दक्षिणः_2);

				सचे (complementary_var && new_patterns.गणनम्(दक्षिणः_2)) {
					new_patterns.erase(दक्षिणः_2);
					दक्षिणः_2.erase(complementary_var.मानम्());
					new_patterns.निवेशय(दक्षिणः_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} याव(optimized);

	// Remove redundant patterns
	अत्थाय (auto i = patterns.begin(); i != patterns.end(); ++i) {
		अत्थाय (auto j = std::next(i, 1); j != patterns.end(); ++j) {
			ध्रुव auto& वामः_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
			ध्रुव auto& दक्षिणः_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
			bool redundant = true;

			अत्थाय (ध्रुव auto& pt : वामः_2)
				सचे (दक्षिणः_2.गणनम्(pt.first) == 0 || दक्षिणः_2.at(pt.first) != pt.second)
					redundant = false;
			सचे (redundant)
				new_patterns.erase(दक्षिणः_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_END

#endif
