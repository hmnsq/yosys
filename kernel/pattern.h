#ifndef OPT_DFF_COMP_H
#define OPT_DFF_COMP_H

#அந்தர்பாவ "kernel/rtlil.h"
#அந்தர்பாவ <map>
#அந்தர்பாவ <optional>

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
வகைவரை std::map<RTLIL::SigBit, bool> pattern_t; // Set of control signals that must ALL match required vals
வகைவரை std::set<pattern_t> patterns_t;          // Alternative patterns (OR)
வகைவரை std::pair<RTLIL::SigBit, bool> ctrl_t;   // Control signal
வகைவரை std::set<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
உள்வரி std::optional<RTLIL::SigBit> find_complementary_pattern_var(
	மாறிலி pattern_t& இடது_2,
	மாறிலி pattern_t& வலது_2
) {
	std::optional<RTLIL::SigBit> ret;
	க்கு (மாறிலி auto &pt : இடது_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		எனில் (வலது_2.மொத்தம்(pt.first) == 0)
			திரும்பு std::nullopt;
		// Signal has same required value in both - not the complement variable
		எனில் (வலது_2.at(pt.first) == pt.second)
			தொடர்;
		// Already found one differing signal, now found another - not simplifiable
		எனில் (ret)
			திரும்பு std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.first;
	}
	திரும்பு ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
உள்வரி வெறுமை simplify_patterns(patterns_t& patterns) {
	auto new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	செய் {
		optimized = false;
		க்கு (auto i = patterns.begin(); i != patterns.end(); i++) {
			க்கு (auto j = std::next(i, 1); j != patterns.end(); j++) {
				மாறிலி auto& இடது_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
				auto வலது_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
				மாறிலி auto complementary_var = find_complementary_pattern_var(இடது_2, வலது_2);

				எனில் (complementary_var && new_patterns.மொத்தம்(வலது_2)) {
					new_patterns.erase(வலது_2);
					வலது_2.erase(complementary_var.மதிப்பு());
					new_patterns.செருகு(வலது_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} போது(optimized);

	// Remove redundant patterns
	க்கு (auto i = patterns.begin(); i != patterns.end(); ++i) {
		க்கு (auto j = std::next(i, 1); j != patterns.end(); ++j) {
			மாறிலி auto& இடது_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
			மாறிலி auto& வலது_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
			bool redundant = true;

			க்கு (மாறிலி auto& pt : இடது_2)
				எனில் (வலது_2.மொத்தம்(pt.first) == 0 || வலது_2.at(pt.first) != pt.second)
					redundant = false;
			எனில் (redundant)
				new_patterns.erase(வலது_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_END

#endif
