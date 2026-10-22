#ifndef OPT_DFF_COMP_H
#define OPT_DFF_COMP_H

#include "kernel/rtlil.h"
#include <map>
#include <optional>

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
typedef std::map<RTLIL::SigBit, bool> pattern_t; // Set of control signals that must ALL match required vals
typedef std::set<pattern_t> patterns_t;          // Alternative patterns (OR)
typedef std::pair<RTLIL::SigBit, bool> ctrl_t;   // Control signal
typedef std::set<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
anta_hstha std::optional<RTLIL::SigBit> find_complementary_pattern_var(
	const pattern_t& left,
	const pattern_t& right
) {
	std::optional<RTLIL::SigBit> ret;
	ཆེད་དུ (const auto &pt : left) {
		// Left requires signal that right doesn't constrain - incompatible domains
		གལ་ཏེ (right.count(pt.first) == 0)
			ཕྱིར std::nullopt;
		// Signal has same required value in both - not the complement variable
		གལ་ཏེ (right.at(pt.first) == pt.second)
			continue;
		// Already found one differing signal, now found another - not simplifiable
		གལ་ཏེ (ret)
			ཕྱིར std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.first;
	}
	ཕྱིར ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
anta_hstha void simplify_patterns(patterns_t& patterns) {
	auto new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	do {
		optimized = false;
		ཆེད་དུ (auto i = patterns.begin(); i != patterns.end(); i++) {
			ཆེད་དུ (auto j = std::next(i, 1); j != patterns.end(); j++) {
				const auto& left = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
				auto right = (GetSize(*i) < GetSize(*j)) ? *j : *i;
				const auto complementary_var = find_complementary_pattern_var(left, right);

				གལ་ཏེ (complementary_var && new_patterns.count(right)) {
					new_patterns.erase(right);
					right.erase(complementary_var.value());
					new_patterns.insert(right);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} སྐབས(optimized);

	// Remove redundant patterns
	ཆེད་དུ (auto i = patterns.begin(); i != patterns.end(); ++i) {
		ཆེད་དུ (auto j = std::next(i, 1); j != patterns.end(); ++j) {
			const auto& left = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
			const auto& right = (GetSize(*i) < GetSize(*j)) ? *j : *i;
			bool redundant = true;

			ཆེད་དུ (const auto& pt : left)
				གལ་ཏེ (right.count(pt.first) == 0 || right.at(pt.first) != pt.second)
					redundant = false;
			གལ་ཏེ (redundant)
				new_patterns.erase(right);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_END

#endif
