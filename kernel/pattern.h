#ifndef OPT_DFF_COMP_H
#define OPT_DFF_COMP_H

#ضمن "kernel/rtlil.h"
#ضمن <map>
#ضمن <optional>

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
تعريفنوع std::map<RTLIL::SigBit, bool> pattern_t; // Set of control signals that must ALL match required vals
تعريفنوع std::set<pattern_t> patterns_t;          // Alternative patterns (OR)
تعريفنوع std::pair<RTLIL::SigBit, bool> ctrl_t;   // Control signal
تعريفنوع std::set<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
مضمن std::optional<RTLIL::SigBit> find_complementary_pattern_var(
	ثابت pattern_t& چەپ_2,
	ثابت pattern_t& ڕاست_2
) {
	std::optional<RTLIL::SigBit> ret;
	لكل (ثابت auto &pt : چەپ_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		إذا (ڕاست_2.ژماردن(pt.first) == 0)
			أرجع std::nullopt;
		// Signal has same required value in both - not the complement variable
		إذا (ڕاست_2.at(pt.first) == pt.second)
			استمر;
		// Already found one differing signal, now found another - not simplifiable
		إذا (ret)
			أرجع std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.first;
	}
	أرجع ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
مضمن فراغ simplify_patterns(patterns_t& patterns) {
	auto new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	افعل {
		optimized = false;
		لكل (auto i = patterns.begin(); i != patterns.end(); i++) {
			لكل (auto j = std::next(i, 1); j != patterns.end(); j++) {
				ثابت auto& چەپ_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
				auto ڕاست_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
				ثابت auto complementary_var = find_complementary_pattern_var(چەپ_2, ڕاست_2);

				إذا (complementary_var && new_patterns.ژماردن(ڕاست_2)) {
					new_patterns.erase(ڕاست_2);
					ڕاست_2.erase(complementary_var.بەها());
					new_patterns.زیادکردن(ڕاست_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} طالما(optimized);

	// Remove redundant patterns
	لكل (auto i = patterns.begin(); i != patterns.end(); ++i) {
		لكل (auto j = std::next(i, 1); j != patterns.end(); ++j) {
			ثابت auto& چەپ_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
			ثابت auto& ڕاست_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
			bool redundant = true;

			لكل (ثابت auto& pt : چەپ_2)
				إذا (ڕاست_2.ژماردن(pt.first) == 0 || ڕاست_2.at(pt.first) != pt.second)
					redundant = false;
			إذا (redundant)
				new_patterns.erase(ڕاست_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_END

#endif
