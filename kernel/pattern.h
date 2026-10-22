#اگر_غیر_تعریف_شدہ OPT_DFF_COMP_H
#تعریف_کرو OPT_DFF_COMP_H

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
قسمتعریف std::map<RTLIL::Sig_ثنائی_ہندسہ, bool> نمونہ_t; // Set of control signals that must ALL match required vals
قسمتعریف std::سیٹ<نمونہ_t> patterns_t;          // Alternative patterns (OR)
قسمتعریف std::pair<RTLIL::Sig_ثنائی_ہندسہ, bool> ctrl_t;   // Control signal
قسمتعریف std::سیٹ<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
اندرونی std::optional<RTLIL::Sig_ثنائی_ہندسہ> ڈھونڈو_complementary_نمونہ_var(
	ثابت نمونہ_t& بایاں_2,
	ثابت نمونہ_t& دایاں_2
) {
	std::optional<RTLIL::Sig_ثنائی_ہندسہ> ret;
	برائے (ثابت خودکار &pt : بایاں_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		اگر (دایاں_2.گنتی(pt.پہلا) == 0)
			واپس std::nullopt;
		// Signal has same required value in both - not the complement variable
		اگر (دایاں_2.at(pt.پہلا) == pt.دوسرا)
			جاری;
		// Already found one differing signal, now found another - not simplifiable
		اگر (ret)
			واپس std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.پہلا;
	}
	واپس ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
اندرونی خالی simplify_patterns(patterns_t& patterns) {
	خودکار new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	کرو {
		optimized = false;
		برائے (خودکار i = patterns.begin(); i != patterns.انتہا(); i++) {
			برائے (خودکار j = std::اگلا(i, 1); j != patterns.انتہا(); j++) {
				ثابت خودکار& بایاں_2 = (Get_سائز(*j) <= Get_سائز(*i)) ? *j : *i;
				خودکار دایاں_2 = (Get_سائز(*i) < Get_سائز(*j)) ? *j : *i;
				ثابت خودکار complementary_var = ڈھونڈو_complementary_نمونہ_var(بایاں_2, دایاں_2);

				اگر (complementary_var && new_patterns.گنتی(دایاں_2)) {
					new_patterns.erase(دایاں_2);
					دایاں_2.erase(complementary_var.قدر());
					new_patterns.داخل_کرو(دایاں_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} جب(optimized);

	// Remove redundant patterns
	برائے (خودکار i = patterns.begin(); i != patterns.انتہا(); ++i) {
		برائے (خودکار j = std::اگلا(i, 1); j != patterns.انتہا(); ++j) {
			ثابت خودکار& بایاں_2 = (Get_سائز(*j) <= Get_سائز(*i)) ? *j : *i;
			ثابت خودکار& دایاں_2 = (Get_سائز(*i) < Get_سائز(*j)) ? *j : *i;
			bool redundant = true;

			برائے (ثابت خودکار& pt : بایاں_2)
				اگر (دایاں_2.گنتی(pt.پہلا) == 0 || دایاں_2.at(pt.پہلا) != pt.دوسرا)
					redundant = false;
			اگر (redundant)
				new_patterns.erase(دایاں_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_انتہا

#اگر_ختم
