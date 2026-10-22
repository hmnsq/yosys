#अगर_अघोषित OPT_DFF_COMP_H
#घोषणा OPT_DFF_COMP_H

#समावेश "kernel/rtlil.h"
#समावेश <map>
#समावेश <optional>

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
प्रकार std::map<RTLIL::SigBit, bool> pattern_t; // Set of control signals that must ALL match required vals
प्रकार std::set<pattern_t> patterns_t;          // Alternative patterns (OR)
प्रकार std::pair<RTLIL::SigBit, bool> ctrl_t;   // Control signal
प्रकार std::set<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
अंतरभूत std::optional<RTLIL::SigBit> find_complementary_pattern_var(
	स्थिर pattern_t& बायाँ_2,
	स्थिर pattern_t& दायाँ_2
) {
	std::optional<RTLIL::SigBit> ret;
	क्रम (स्थिर स्वतः &pt : बायाँ_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		अगर (दायाँ_2.गिनती(pt.first) == 0)
			वापस std::nullopt;
		// Signal has same required value in both - not the complement variable
		अगर (दायाँ_2.at(pt.first) == pt.second)
			जारी;
		// Already found one differing signal, now found another - not simplifiable
		अगर (ret)
			वापस std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.first;
	}
	वापस ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
अंतरभूत व्योम simplify_patterns(patterns_t& patterns) {
	स्वतः new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	करो {
		optimized = false;
		क्रम (स्वतः i = patterns.begin(); i != patterns.end(); i++) {
			क्रम (स्वतः j = std::अग्र(i, 1); j != patterns.end(); j++) {
				स्थिर स्वतः& बायाँ_2 = (Get_आकार(*j) <= Get_आकार(*i)) ? *j : *i;
				स्वतः दायाँ_2 = (Get_आकार(*i) < Get_आकार(*j)) ? *j : *i;
				स्थिर स्वतः complementary_var = find_complementary_pattern_var(बायाँ_2, दायाँ_2);

				अगर (complementary_var && new_patterns.गिनती(दायाँ_2)) {
					new_patterns.erase(दायाँ_2);
					दायाँ_2.erase(complementary_var.मूल्य());
					new_patterns.जोड़ो(दायाँ_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} जबतक(optimized);

	// Remove redundant patterns
	क्रम (स्वतः i = patterns.begin(); i != patterns.end(); ++i) {
		क्रम (स्वतः j = std::अग्र(i, 1); j != patterns.end(); ++j) {
			स्थिर स्वतः& बायाँ_2 = (Get_आकार(*j) <= Get_आकार(*i)) ? *j : *i;
			स्थिर स्वतः& दायाँ_2 = (Get_आकार(*i) < Get_आकार(*j)) ? *j : *i;
			bool redundant = true;

			क्रम (स्थिर स्वतः& pt : बायाँ_2)
				अगर (दायाँ_2.गिनती(pt.first) == 0 || दायाँ_2.at(pt.first) != pt.second)
					redundant = false;
			अगर (redundant)
				new_patterns.erase(दायाँ_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_END

#पूर्ण_अगर
