#నిర్వచితం_కాకపోతే OPT_DFF_COMP_H
#నిర్వచించు OPT_DFF_COMP_H

#అన్తర్భావ "kernel/rtlil.h"
#అన్తర్భావ <map>
#అన్తర్భావ <optional>

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
రకనిర్వచనం std::map<RTLIL::Sig_బిట్టు, bool> నమూనా_t; // Set of control signals that must ALL match required vals
రకనిర్వచనం std::సమితి<నమూనా_t> patterns_t;          // Alternative patterns (OR)
రకనిర్వచనం std::pair<RTLIL::Sig_బిట్టు, bool> ctrl_t;   // Control signal
రకనిర్వచనం std::సమితి<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
అంతఃవరుస std::optional<RTLIL::Sig_బిట్టు> కనుగొను_complementary_నమూనా_var(
	స్థిరరాశి నమూనా_t& ఎడమ_3,
	స్థిరరాశి నమూనా_t& కుడి_3
) {
	std::optional<RTLIL::Sig_బిట్టు> ret;
	కొరకు (స్థిరరాశి స్వయంచాలక &pt : ఎడమ_3) {
		// Left requires signal that right doesn't constrain - incompatible domains
		ఒకవేళ (కుడి_3.లెక్క(pt.మొదటి) == 0)
			తిరిగి std::nullopt;
		// Signal has same required value in both - not the complement variable
		ఒకవేళ (కుడి_3.at(pt.మొదటి) == pt.రెండవ)
			కొనసాగు;
		// Already found one differing signal, now found another - not simplifiable
		ఒకవేళ (ret)
			తిరిగి std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.మొదటి;
	}
	తిరిగి ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
అంతఃవరుస శూన్య simplify_patterns(patterns_t& patterns) {
	స్వయంచాలక new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	చేయి {
		optimized = false;
		కొరకు (స్వయంచాలక i = patterns.begin(); i != patterns.చివర(); i++) {
			కొరకు (స్వయంచాలక j = std::తదుపరి(i, 1); j != patterns.చివర(); j++) {
				స్థిరరాశి స్వయంచాలక& ఎడమ_3 = (Get_కొలత(*j) <= Get_కొలత(*i)) ? *j : *i;
				స్వయంచాలక కుడి_3 = (Get_కొలత(*i) < Get_కొలత(*j)) ? *j : *i;
				స్థిరరాశి స్వయంచాలక complementary_var = కనుగొను_complementary_నమూనా_var(ఎడమ_3, కుడి_3);

				ఒకవేళ (complementary_var && new_patterns.లెక్క(కుడి_3)) {
					new_patterns.erase(కుడి_3);
					కుడి_3.erase(complementary_var.విలువ());
					new_patterns.చొప్పించు(కుడి_3);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} అయితే(optimized);

	// Remove redundant patterns
	కొరకు (స్వయంచాలక i = patterns.begin(); i != patterns.చివర(); ++i) {
		కొరకు (స్వయంచాలక j = std::తదుపరి(i, 1); j != patterns.చివర(); ++j) {
			స్థిరరాశి స్వయంచాలక& ఎడమ_3 = (Get_కొలత(*j) <= Get_కొలత(*i)) ? *j : *i;
			స్థిరరాశి స్వయంచాలక& కుడి_3 = (Get_కొలత(*i) < Get_కొలత(*j)) ? *j : *i;
			bool redundant = true;

			కొరకు (స్థిరరాశి స్వయంచాలక& pt : ఎడమ_3)
				ఒకవేళ (కుడి_3.లెక్క(pt.మొదటి) == 0 || కుడి_3.at(pt.మొదటి) != pt.రెండవ)
					redundant = false;
			ఒకవేళ (redundant)
				new_patterns.erase(కుడి_3);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
