#جے_بنا_تعریف OPT_DFF_COMP_H
#تعریف_کرو OPT_DFF_COMP_H

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
تعريفنوع std::map<RTLIL::Sig_ثنائی_ہندسہ, bool> نمونہ_t; // Set of control signals that must ALL match required vals
تعريفنوع std::سیٹ<نمونہ_t> patterns_t;          // Alternative patterns (OR)
تعريفنوع std::pair<RTLIL::Sig_ثنائی_ہندسہ, bool> ctrl_t;   // Control signal
تعريفنوع std::سیٹ<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
مضمن std::optional<RTLIL::Sig_ثنائی_ہندسہ> لبھو_complementary_نمونہ_var(
	ثابت نمونہ_t& بایاں_2,
	ثابت نمونہ_t& دایاں_2
) {
	std::optional<RTLIL::Sig_ثنائی_ہندسہ> ret;
	لكل (ثابت خودکار &pt : بایاں_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		إذا (دایاں_2.گنتی(pt.پہلا) == 0)
			أرجع std::nullopt;
		// Signal has same required value in both - not the complement variable
		إذا (دایاں_2.at(pt.پہلا) == pt.دوجا)
			استمر;
		// Already found one differing signal, now found another - not simplifiable
		إذا (ret)
			أرجع std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.پہلا;
	}
	أرجع ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
مضمن فراغ simplify_patterns(patterns_t& patterns) {
	خودکار new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	افعل {
		optimized = false;
		لكل (خودکار i = patterns.begin(); i != patterns.سرا(); i++) {
			لكل (خودکار j = std::اگلا(i, 1); j != patterns.سرا(); j++) {
				ثابت خودکار& بایاں_2 = (Get_ناپ(*j) <= Get_ناپ(*i)) ? *j : *i;
				خودکار دایاں_2 = (Get_ناپ(*i) < Get_ناپ(*j)) ? *j : *i;
				ثابت خودکار complementary_var = لبھو_complementary_نمونہ_var(بایاں_2, دایاں_2);

				إذا (complementary_var && new_patterns.گنتی(دایاں_2)) {
					new_patterns.erase(دایاں_2);
					دایاں_2.erase(complementary_var.قدر());
					new_patterns.داخل_کرو(دایاں_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} طالما(optimized);

	// Remove redundant patterns
	لكل (خودکار i = patterns.begin(); i != patterns.سرا(); ++i) {
		لكل (خودکار j = std::اگلا(i, 1); j != patterns.سرا(); ++j) {
			ثابت خودکار& بایاں_2 = (Get_ناپ(*j) <= Get_ناپ(*i)) ? *j : *i;
			ثابت خودکار& دایاں_2 = (Get_ناپ(*i) < Get_ناپ(*j)) ? *j : *i;
			bool redundant = true;

			لكل (ثابت خودکار& pt : بایاں_2)
				إذا (دایاں_2.گنتی(pt.پہلا) == 0 || دایاں_2.at(pt.پہلا) != pt.دوجا)
					redundant = false;
			إذا (redundant)
				new_patterns.erase(دایاں_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_سرا

#جے_انت
