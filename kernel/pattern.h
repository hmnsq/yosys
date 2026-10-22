#جيڪڏهن_وصف_نه_ٿيل OPT_DFF_COMP_H
#وصف_ڪريو OPT_DFF_COMP_H

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
تعريفنوع std::map<RTLIL::Sig_ٻٽو_انگ, bool> نمونو_t; // Set of control signals that must ALL match required vals
تعريفنوع std::سيٽ<نمونو_t> patterns_t;          // Alternative patterns (OR)
تعريفنوع std::pair<RTLIL::Sig_ٻٽو_انگ, bool> ctrl_t;   // Control signal
تعريفنوع std::سيٽ<ctrl_t> ctrls_t;                // Set of control signals that must ALL be active

/**
 * Find if two patterns differ in exactly one variable.
 * Example: {A=1,B=1} vs {A=1,B=0} returns B, allows simplification: (A&B) | (A&!B) => A
 */
مضمن std::optional<RTLIL::Sig_ٻٽو_انگ> ڳوليو_complementary_نمونو_var(
	ثابت نمونو_t& کاٻو_2,
	ثابت نمونو_t& ساڄو_2
) {
	std::optional<RTLIL::Sig_ٻٽو_انگ> ret;
	لكل (ثابت خودڪار &pt : کاٻو_2) {
		// Left requires signal that right doesn't constrain - incompatible domains
		إذا (ساڄو_2.ڳڻپ(pt.پهريون) == 0)
			أرجع std::nullopt;
		// Signal has same required value in both - not the complement variable
		إذا (ساڄو_2.at(pt.پهريون) == pt.ٻيو)
			استمر;
		// Already found one differing signal, now found another - not simplifiable
		إذا (ret)
			أرجع std::nullopt;
		// First differing signal - candidate complement variable
		ret = pt.پهريون;
	}
	أرجع ret;
}

/**
 * Simplify a sum-of-products by merging complementary patterns: (A&B) | (A&!B) => A,
 * and removing redundant patterns: A | (A&B) => A
 */
مضمن فراغ simplify_patterns(patterns_t& patterns) {
	خودڪار new_patterns = patterns;

	// Merge complementary patterns
	bool optimized;
	افعل {
		optimized = false;
		لكل (خودڪار i = patterns.begin(); i != patterns.پڇاڙي(); i++) {
			لكل (خودڪار j = std::ايندڙ(i, 1); j != patterns.پڇاڙي(); j++) {
				ثابت خودڪار& کاٻو_2 = (Get_سائيز(*j) <= Get_سائيز(*i)) ? *j : *i;
				خودڪار ساڄو_2 = (Get_سائيز(*i) < Get_سائيز(*j)) ? *j : *i;
				ثابت خودڪار complementary_var = ڳوليو_complementary_نمونو_var(کاٻو_2, ساڄو_2);

				إذا (complementary_var && new_patterns.ڳڻپ(ساڄو_2)) {
					new_patterns.erase(ساڄو_2);
					ساڄو_2.erase(complementary_var.قدر());
					new_patterns.وجهو(ساڄو_2);
					optimized = true;
				}
			}
		}
		patterns = new_patterns;
	} طالما(optimized);

	// Remove redundant patterns
	لكل (خودڪار i = patterns.begin(); i != patterns.پڇاڙي(); ++i) {
		لكل (خودڪار j = std::ايندڙ(i, 1); j != patterns.پڇاڙي(); ++j) {
			ثابت خودڪار& کاٻو_2 = (Get_سائيز(*j) <= Get_سائيز(*i)) ? *j : *i;
			ثابت خودڪار& ساڄو_2 = (Get_سائيز(*i) < Get_سائيز(*j)) ? *j : *i;
			bool redundant = true;

			لكل (ثابت خودڪار& pt : کاٻو_2)
				إذا (ساڄو_2.ڳڻپ(pt.پهريون) == 0 || ساڄو_2.at(pt.پهريون) != pt.ٻيو)
					redundant = false;
			إذا (redundant)
				new_patterns.erase(ساڄو_2);
		}
	}

	patterns = std::move(new_patterns);
}

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
