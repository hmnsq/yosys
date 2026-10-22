#ifndef OPT_DFF_COMP_H
#define OPT_DFF_COMP_H

#অন্তর্ভাব "kernel/rtlil.h"
#অন্তর্ভাব <map>
#অন্তর্ভাব <optional>

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
প্রকারলেখ std::map<RTLIL::SigBit, bool> pattern_t; // Set of control signals that must ALL match required vals
প্রকারলেখ std::set<pattern_t> patterns_t;          // Alternative patterns (OR)
প্রকারলেখ std::pair<RTLIL::SigBit, bool> ctrl_t;   // Control signal
প্রকারলেখ std::set<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
অন্তঃরেখ std::optional<RTLIL::SigBit> find_complementary_pattern_var(
	ধ্রুবক pattern_t& বাম_2,
	ধ্রুবক pattern_t& ডান_2
) {
	std::optional<RTLIL::SigBit> ret;
	জন্য (ধ্রুবক auto &pt : বাম_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		যদি (ডান_2.গুনতি(pt.first) == 0)
			ফেরত std::nullopt;
		// Signal has same required value in both - not the complement variable
		যদি (ডান_2.at(pt.first) == pt.second)
			চালু;
		// Already found one differing signal, now found another - not simplifiable
		যদি (ret)
			ফেরত std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.first;
	}
	ফেরত ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
অন্তঃরেখ শূন্য simplify_patterns(patterns_t& patterns) {
	auto new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	করো {
		optimized = false;
		জন্য (auto i = patterns.begin(); i != patterns.end(); i++) {
			জন্য (auto j = std::পরবর্তী(i, 1); j != patterns.end(); j++) {
				ধ্রুবক auto& বাম_2 = (Get_আকার(*j) <= Get_আকার(*i)) ? *j : *i;
				auto ডান_2 = (Get_আকার(*i) < Get_আকার(*j)) ? *j : *i;
				ধ্রুবক auto complementary_var = find_complementary_pattern_var(বাম_2, ডান_2);

				যদি (complementary_var && new_patterns.গুনতি(ডান_2)) {
					new_patterns.erase(ডান_2);
					ডান_2.erase(complementary_var.মান());
					new_patterns.ঢোকাও(ডান_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} যখন(optimized);

	// Remove redundant patterns
	জন্য (auto i = patterns.begin(); i != patterns.end(); ++i) {
		জন্য (auto j = std::পরবর্তী(i, 1); j != patterns.end(); ++j) {
			ধ্রুবক auto& বাম_2 = (Get_আকার(*j) <= Get_আকার(*i)) ? *j : *i;
			ধ্রুবক auto& ডান_2 = (Get_আকার(*i) < Get_আকার(*j)) ? *j : *i;
			bool redundant = true;

			জন্য (ধ্রুবক auto& pt : বাম_2)
				যদি (ডান_2.গুনতি(pt.first) == 0 || ডান_2.at(pt.first) != pt.second)
					redundant = false;
			যদি (redundant)
				new_patterns.erase(ডান_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_END

#endif
