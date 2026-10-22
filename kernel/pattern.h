#यदि_अपरिभाषितम् OPT_DFF_COMP_H
#परिभाषय OPT_DFF_COMP_H

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
प्रकारनिर्देश std::map<RTLIL::SigBit, bool> आदर्शः_t; // Set of control signals that must ALL match required vals
प्रकारनिर्देश std::समुच्चयः<आदर्शः_t> patterns_t;          // Alternative patterns (OR)
प्रकारनिर्देश std::pair<RTLIL::SigBit, bool> ctrl_t;   // Control signal
प्रकारनिर्देश std::समुच्चयः<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
अन्तःस्थ std::optional<RTLIL::SigBit> find_complementary_आदर्शः_var(
	ध्रुव आदर्शः_t& वामः_2,
	ध्रुव आदर्शः_t& दक्षिणः_2
) {
	std::optional<RTLIL::SigBit> ret;
	प्रति (ध्रुव स्वचालित &pt : वामः_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		यदि (दक्षिणः_2.गणनम्(pt.first) == 0)
			प्रत्यागम std::nullopt;
		// Signal has same required value in both - not the complement variable
		यदि (दक्षिणः_2.at(pt.first) == pt.second)
			अनुवर्त;
		// Already found one differing signal, now found another - not simplifiable
		यदि (ret)
			प्रत्यागम std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.first;
	}
	प्रत्यागम ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
अन्तःस्थ शून्य simplify_patterns(patterns_t& patterns) {
	स्वचालित new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	कुरु {
		optimized = false;
		प्रति (स्वचालित i = patterns.begin(); i != patterns.अन्तः(); i++) {
			प्रति (स्वचालित j = std::अग्र(i, 1); j != patterns.अन्तः(); j++) {
				ध्रुव स्वचालित& वामः_2 = (Get_आकार(*j) <= Get_आकार(*i)) ? *j : *i;
				स्वचालित दक्षिणः_2 = (Get_आकार(*i) < Get_आकार(*j)) ? *j : *i;
				ध्रुव स्वचालित complementary_var = find_complementary_आदर्शः_var(वामः_2, दक्षिणः_2);

				यदि (complementary_var && new_patterns.गणनम्(दक्षिणः_2)) {
					new_patterns.erase(दक्षिणः_2);
					दक्षिणः_2.erase(complementary_var.मानम्());
					new_patterns.निवेशय(दक्षिणः_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} यावत्(optimized);

	// Remove redundant patterns
	प्रति (स्वचालित i = patterns.begin(); i != patterns.अन्तः(); ++i) {
		प्रति (स्वचालित j = std::अग्र(i, 1); j != patterns.अन्तः(); ++j) {
			ध्रुव स्वचालित& वामः_2 = (Get_आकार(*j) <= Get_आकार(*i)) ? *j : *i;
			ध्रुव स्वचालित& दक्षिणः_2 = (Get_आकार(*i) < Get_आकार(*j)) ? *j : *i;
			bool redundant = true;

			प्रति (ध्रुव स्वचालित& pt : वामः_2)
				यदि (दक्षिणः_2.गणनम्(pt.first) == 0 || दक्षिणः_2.at(pt.first) != pt.second)
					redundant = false;
			यदि (redundant)
				new_patterns.erase(दक्षिणः_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
