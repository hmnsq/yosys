#اگر_تعریف_نشده OPT_DFF_COMP_H
#تعریف_کن OPT_DFF_COMP_H

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
تعريفنوع std::map<RTLIL::Sig_بیت, bool> الگو_t; // Set of control signals that must ALL match required vals
تعريفنوع std::مجموعه<الگو_t> patterns_t;          // Alternative patterns (OR)
تعريفنوع std::pair<RTLIL::Sig_بیت, bool> ctrl_t;   // Control signal
تعريفنوع std::مجموعه<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
مضمن std::optional<RTLIL::Sig_بیت> پیدا_کن_complementary_الگو_var(
	ثابت الگو_t& چپ_3,
	ثابت الگو_t& راست_3
) {
	std::optional<RTLIL::Sig_بیت> ret;
	لكل (ثابت خودکار &pt : چپ_3) {
		// Left requires signal that right doesn't constrain - incompatible domains
		إذا (راست_3.شمار(pt.اولی) == 0)
			أرجع std::nullopt;
		// Signal has same required value in both - not the complement variable
		إذا (راست_3.at(pt.اولی) == pt.دومی)
			استمر;
		// Already found one differing signal, now found another - not simplifiable
		إذا (ret)
			أرجع std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.اولی;
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
		لكل (خودکار i = patterns.begin(); i != patterns.انتها(); i++) {
			لكل (خودکار j = std::بعدی(i, 1); j != patterns.انتها(); j++) {
				ثابت خودکار& چپ_3 = (Get_اندازه(*j) <= Get_اندازه(*i)) ? *j : *i;
				خودکار راست_3 = (Get_اندازه(*i) < Get_اندازه(*j)) ? *j : *i;
				ثابت خودکار complementary_var = پیدا_کن_complementary_الگو_var(چپ_3, راست_3);

				إذا (complementary_var && new_patterns.شمار(راست_3)) {
					new_patterns.erase(راست_3);
					راست_3.erase(complementary_var.مقدار());
					new_patterns.درج(راست_3);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} طالما(optimized);

	// Remove redundant patterns
	لكل (خودکار i = patterns.begin(); i != patterns.انتها(); ++i) {
		لكل (خودکار j = std::بعدی(i, 1); j != patterns.انتها(); ++j) {
			ثابت خودکار& چپ_3 = (Get_اندازه(*j) <= Get_اندازه(*i)) ? *j : *i;
			ثابت خودکار& راست_3 = (Get_اندازه(*i) < Get_اندازه(*j)) ? *j : *i;
			bool redundant = true;

			لكل (ثابت خودکار& pt : چپ_3)
				إذا (راست_3.شمار(pt.اولی) == 0 || راست_3.at(pt.اولی) != pt.دومی)
					redundant = false;
			إذا (redundant)
				new_patterns.erase(راست_3);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_انتها

#پایان_اگر
