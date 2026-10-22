#ifndef OPT_DFF_COMP_H
#define OPT_DFF_COMP_H

#شامل "kernel/rtlil.h"
#شامل <map>
#شامل <optional>

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
تعریفنوع std::map<RTLIL::SigBit, bool> pattern_t; // Set of control signals that must ALL match required vals
تعریفنوع std::set<pattern_t> patterns_t;          // Alternative patterns (OR)
تعریفنوع std::pair<RTLIL::SigBit, bool> ctrl_t;   // Control signal
تعریفنوع std::set<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
درونخطی std::optional<RTLIL::SigBit> find_complementary_pattern_var(
	ثابت pattern_t& چپ_2,
	ثابت pattern_t& راست_2
) {
	std::optional<RTLIL::SigBit> ret;
	برای (ثابت auto &pt : چپ_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		اگر (راست_2.شمار(pt.first) == 0)
			بازگشت std::nullopt;
		// Signal has same required value in both - not the complement variable
		اگر (راست_2.at(pt.first) == pt.second)
			ادامه;
		// Already found one differing signal, now found another - not simplifiable
		اگر (ret)
			بازگشت std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.first;
	}
	بازگشت ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
درونخطی تهی simplify_patterns(patterns_t& patterns) {
	auto new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	انجامبده {
		optimized = false;
		برای (auto i = patterns.begin(); i != patterns.end(); i++) {
			برای (auto j = std::next(i, 1); j != patterns.end(); j++) {
				ثابت auto& چپ_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
				auto راست_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
				ثابت auto complementary_var = find_complementary_pattern_var(چپ_2, راست_2);

				اگر (complementary_var && new_patterns.شمار(راست_2)) {
					new_patterns.erase(راست_2);
					راست_2.erase(complementary_var.مقدار());
					new_patterns.درج(راست_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} وقتی(optimized);

	// Remove redundant patterns
	برای (auto i = patterns.begin(); i != patterns.end(); ++i) {
		برای (auto j = std::next(i, 1); j != patterns.end(); ++j) {
			ثابت auto& چپ_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
			ثابت auto& راست_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
			bool redundant = true;

			برای (ثابت auto& pt : چپ_2)
				اگر (راست_2.شمار(pt.first) == 0 || راست_2.at(pt.first) != pt.second)
					redundant = false;
			اگر (redundant)
				new_patterns.erase(راست_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_END

#endif
