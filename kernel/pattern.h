#که_نه_تعریف_شوی OPT_DFF_COMP_H
#تعریف_کړه OPT_DFF_COMP_H

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
تعريفنوع std::map<RTLIL::Sig_بټ, bool> نمونه_t; // Set of control signals that must ALL match required vals
تعريفنوع std::سېټ<نمونه_t> patterns_t;          // Alternative patterns (OR)
تعريفنوع std::pair<RTLIL::Sig_بټ, bool> ctrl_t;   // Control signal
تعريفنوع std::سېټ<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
مضمن std::optional<RTLIL::Sig_بټ> پیدا_کړه_complementary_نمونه_var(
	ثابت نمونه_t& کیڼ_3,
	ثابت نمونه_t& ښی_3
) {
	std::optional<RTLIL::Sig_بټ> ret;
	لكل (ثابت خپلکاری &pt : کیڼ_3) {
		// Left requires signal that right doesn't constrain - incompatible domains
		إذا (ښی_3.شمېر(pt.لومړی) == 0)
			أرجع std::nullopt;
		// Signal has same required value in both - not the complement variable
		إذا (ښی_3.at(pt.لومړی) == pt.دوهم)
			استمر;
		// Already found one differing signal, now found another - not simplifiable
		إذا (ret)
			أرجع std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.لومړی;
	}
	أرجع ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
مضمن فراغ simplify_patterns(patterns_t& patterns) {
	خپلکاری new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	افعل {
		optimized = false;
		لكل (خپلکاری i = patterns.begin(); i != patterns.پای_ټکی(); i++) {
			لكل (خپلکاری j = std::بل(i, 1); j != patterns.پای_ټکی(); j++) {
				ثابت خپلکاری& کیڼ_3 = (Get_اندازه(*j) <= Get_اندازه(*i)) ? *j : *i;
				خپلکاری ښی_3 = (Get_اندازه(*i) < Get_اندازه(*j)) ? *j : *i;
				ثابت خپلکاری complementary_var = پیدا_کړه_complementary_نمونه_var(کیڼ_3, ښی_3);

				إذا (complementary_var && new_patterns.شمېر(ښی_3)) {
					new_patterns.erase(ښی_3);
					ښی_3.erase(complementary_var.ارزښت());
					new_patterns.ورننباسه(ښی_3);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} طالما(optimized);

	// Remove redundant patterns
	لكل (خپلکاری i = patterns.begin(); i != patterns.پای_ټکی(); ++i) {
		لكل (خپلکاری j = std::بل(i, 1); j != patterns.پای_ټکی(); ++j) {
			ثابت خپلکاری& کیڼ_3 = (Get_اندازه(*j) <= Get_اندازه(*i)) ? *j : *i;
			ثابت خپلکاری& ښی_3 = (Get_اندازه(*i) < Get_اندازه(*j)) ? *j : *i;
			bool redundant = true;

			لكل (ثابت خپلکاری& pt : کیڼ_3)
				إذا (ښی_3.شمېر(pt.لومړی) == 0 || ښی_3.at(pt.لومړی) != pt.دوهم)
					redundant = false;
			إذا (redundant)
				new_patterns.erase(ښی_3);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
