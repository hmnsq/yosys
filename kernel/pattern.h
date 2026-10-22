#ifndef OPT_DFF_COMP_H
#define OPT_DFF_COMP_H

#അന്തര്ഭാവ "kernel/rtlil.h"
#അന്തര്ഭാവ <map>
#അന്തര്ഭാവ <optional>

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
തരനിർവചനം std::map<RTLIL::SigBit, bool> pattern_t; // Set of control signals that must ALL match required vals
തരനിർവചനം std::set<pattern_t> patterns_t;          // Alternative patterns (OR)
തരനിർവചനം std::pair<RTLIL::SigBit, bool> ctrl_t;   // Control signal
തരനിർവചനം std::set<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
വരിയിൽ std::optional<RTLIL::SigBit> find_complementary_pattern_var(
	സ്ഥിരം pattern_t& ഇടത്_2,
	സ്ഥിരം pattern_t& വലത്_2
) {
	std::optional<RTLIL::SigBit> ret;
	വേണ്ടി (സ്ഥിരം auto &pt : ഇടത്_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		എങ്കിൽ (വലത്_2.എണ്ണം(pt.first) == 0)
			തിരികെ std::nullopt;
		// Signal has same required value in both - not the complement variable
		എങ്കിൽ (വലത്_2.at(pt.first) == pt.second)
			തുടരുക;
		// Already found one differing signal, now found another - not simplifiable
		എങ്കിൽ (ret)
			തിരികെ std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.first;
	}
	തിരികെ ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
വരിയിൽ ശൂന്യം simplify_patterns(patterns_t& patterns) {
	auto new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	ചെയ്യുക {
		optimized = false;
		വേണ്ടി (auto i = patterns.begin(); i != patterns.end(); i++) {
			വേണ്ടി (auto j = std::next(i, 1); j != patterns.end(); j++) {
				സ്ഥിരം auto& ഇടത്_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
				auto വലത്_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
				സ്ഥിരം auto complementary_var = find_complementary_pattern_var(ഇടത്_2, വലത്_2);

				എങ്കിൽ (complementary_var && new_patterns.എണ്ണം(വലത്_2)) {
					new_patterns.erase(വലത്_2);
					വലത്_2.erase(complementary_var.മൂല്യം());
					new_patterns.തിരുകുക(വലത്_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} ആകുമ്പോൾ(optimized);

	// Remove redundant patterns
	വേണ്ടി (auto i = patterns.begin(); i != patterns.end(); ++i) {
		വേണ്ടി (auto j = std::next(i, 1); j != patterns.end(); ++j) {
			സ്ഥിരം auto& ഇടത്_2 = (GetSize(*j) <= GetSize(*i)) ? *j : *i;
			സ്ഥിരം auto& വലത്_2 = (GetSize(*i) < GetSize(*j)) ? *j : *i;
			bool redundant = true;

			വേണ്ടി (സ്ഥിരം auto& pt : ഇടത്_2)
				എങ്കിൽ (വലത്_2.എണ്ണം(pt.first) == 0 || വലത്_2.at(pt.first) != pt.second)
					redundant = false;
			എങ്കിൽ (redundant)
				new_patterns.erase(വലത്_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_END

#endif
