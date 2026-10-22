#ifndef OPT_DFF_COMP_H
#define OPT_DFF_COMP_H

#અન્તર્ભાવ "kernel/rtlil.h"
#અન્તર્ભાવ <map>
#અન્તર્ભાવ <optional>

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
પ્રકારવ્યાખ્યા std::map<RTLIL::SigBit, bool> pattern_t; // Set of control signals that must ALL match required vals
પ્રકારવ્યાખ્યા std::set<pattern_t> patterns_t;          // Alternative patterns (OR)
પ્રકારવ્યાખ્યા std::pair<RTLIL::SigBit, bool> ctrl_t;   // Control signal
પ્રકારવ્યાખ્યા std::set<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
અંતઃરેખ std::optional<RTLIL::SigBit> find_complementary_pattern_var(
	અચલ pattern_t& ડાબું_2,
	અચલ pattern_t& જમણું_2
) {
	std::optional<RTLIL::SigBit> ret;
	માટે (અચલ auto &pt : ડાબું_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		જો (જમણું_2.ગણતરી(pt.first) == 0)
			પાછા std::nullopt;
		// Signal has same required value in both - not the complement variable
		જો (જમણું_2.at(pt.first) == pt.second)
			ચાલુ;
		// Already found one differing signal, now found another - not simplifiable
		જો (ret)
			પાછા std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.first;
	}
	પાછા ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
અંતઃરેખ શૂન્ય simplify_patterns(patterns_t& patterns) {
	auto new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	કરો {
		optimized = false;
		માટે (auto i = patterns.begin(); i != patterns.end(); i++) {
			માટે (auto j = std::next(i, 1); j != patterns.end(); j++) {
				અચલ auto& ડાબું_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
				auto જમણું_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
				અચલ auto complementary_var = find_complementary_pattern_var(ડાબું_2, જમણું_2);

				જો (complementary_var && new_patterns.ગણતરી(જમણું_2)) {
					new_patterns.erase(જમણું_2);
					જમણું_2.erase(complementary_var.કિંમત());
					new_patterns.ઉમેરો(જમણું_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} જ્યારે(optimized);

	// Remove redundant patterns
	માટે (auto i = patterns.begin(); i != patterns.end(); ++i) {
		માટે (auto j = std::next(i, 1); j != patterns.end(); ++j) {
			અચલ auto& ડાબું_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
			અચલ auto& જમણું_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
			bool redundant = true;

			માટે (અચલ auto& pt : ડાબું_2)
				જો (જમણું_2.ગણતરી(pt.first) == 0 || જમણું_2.at(pt.first) != pt.second)
					redundant = false;
			જો (redundant)
				new_patterns.erase(જમણું_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_END

#endif
