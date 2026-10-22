#ifndef OPT_DFF_COMP_H
#define OPT_DFF_COMP_H

#ଅନ୍ତର୍ଭାଵ "kernel/rtlil.h"
#ଅନ୍ତର୍ଭାଵ <map>
#ଅନ୍ତର୍ଭାଵ <optional>

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
ପ୍ରକାରବ୍ୟାଖ୍ୟା std::map<RTLIL::SigBit, bool> pattern_t; // Set of control signals that must ALL match required vals
ପ୍ରକାରବ୍ୟାଖ୍ୟା std::set<pattern_t> patterns_t;          // Alternative patterns (OR)
ପ୍ରକାରବ୍ୟାଖ୍ୟା std::pair<RTLIL::SigBit, bool> ctrl_t;   // Control signal
ପ୍ରକାରବ୍ୟାଖ୍ୟା std::set<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
ଧାଡ଼ିଭିତରେ std::optional<RTLIL::SigBit> find_complementary_pattern_var(
	ଅଚଳ pattern_t& ବାମ_2,
	ଅଚଳ pattern_t& ଡାହାଣ_2
) {
	std::optional<RTLIL::SigBit> ret;
	ପାଇଁ (ଅଚଳ auto &pt : ବାମ_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		ଯଦି (ଡାହାଣ_2.ଗଣନା_2(pt.first) == 0)
			ଫେରନ୍ତୁ std::nullopt;
		// Signal has same required value in both - not the complement variable
		ଯଦି (ଡାହାଣ_2.at(pt.first) == pt.second)
			ଜାରି;
		// Already found one differing signal, now found another - not simplifiable
		ଯଦି (ret)
			ଫେରନ୍ତୁ std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.first;
	}
	ଫେରନ୍ତୁ ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ simplify_patterns(patterns_t& patterns) {
	auto new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	କର {
		optimized = false;
		ପାଇଁ (auto i = patterns.begin(); i != patterns.end(); i++) {
			ପାଇଁ (auto j = std::next(i, 1); j != patterns.end(); j++) {
				ଅଚଳ auto& ବାମ_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
				auto ଡାହାଣ_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
				ଅଚଳ auto complementary_var = find_complementary_pattern_var(ବାମ_2, ଡାହାଣ_2);

				ଯଦି (complementary_var && new_patterns.ଗଣନା_2(ଡାହାଣ_2)) {
					new_patterns.erase(ଡାହାଣ_2);
					ଡାହାଣ_2.erase(complementary_var.ମୂଲ୍ୟ());
					new_patterns.ଭର୍ତ୍ତି(ଡାହାଣ_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} ଯେତେବେଳେ(optimized);

	// Remove redundant patterns
	ପାଇଁ (auto i = patterns.begin(); i != patterns.end(); ++i) {
		ପାଇଁ (auto j = std::next(i, 1); j != patterns.end(); ++j) {
			ଅଚଳ auto& ବାମ_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
			ଅଚଳ auto& ଡାହାଣ_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
			bool redundant = true;

			ପାଇଁ (ଅଚଳ auto& pt : ବାମ_2)
				ଯଦି (ଡାହାଣ_2.ଗଣନା_2(pt.first) == 0 || ଡାହାଣ_2.at(pt.first) != pt.second)
					redundant = false;
			ଯଦି (redundant)
				new_patterns.erase(ଡାହାଣ_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_END

#endif
