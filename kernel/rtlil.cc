/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ಅನ್ತರ್ಭಾವ "kernel/rtlil.h"
#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/macc.h"
#ಅನ್ತರ್ಭಾವ "kernel/newcelltypes.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/threading.h"
#ಅನ್ತರ್ಭಾವ "frontends/verilog/verilog_frontend.h"
#ಅನ್ತರ್ಭಾವ "frontends/verilog/preproc.h"
#ಅನ್ತರ್ಭಾವ "backends/rtlil/rtlil_backend.h"

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <algorithm>
#ಅನ್ತರ್ಭಾವ <charconv>
#ಅನ್ತರ್ಭಾವ <optional>
#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <sstream>

YOSYS_NAMESPACE_BEGIN

dict<std::string, std::string> RTLIL::constpad;

ಸ್ಥಿರಾಂಕ pool<IdString> &RTLIL::builtin_ff_cell_types() {
	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ pool<IdString> res = []() {
		pool<IdString> r;
		ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < StaticCellTypes::builder.ಎಣಿಕೆ_2; i++) {
			ಸ್ವಯಂಚಾಲಿತ &cell = StaticCellTypes::builder.cells[i];
			ಆದರೆ (cell.features.is_ff)
				r.ಸೇರಿಸು(cell.type);
		}
		ಹಿಂತಿರುಗಿ r;
	}();
	ಹಿಂತಿರುಗಿ res;
}

#ವ್ಯಾಖ್ಯಾನಿಸು check(condition) log_assert(condition && "malformed Const union")

ಸ್ಥಿರಾಂಕ Const::bitvectype& Const::get_ಬಿಟ್ಟುಗಳು() ಸ್ಥಿರಾಂಕ {
	check(is_ಬಿಟ್ಟುಗಳು());
	ಹಿಂತಿರುಗಿ *get_if_ಬಿಟ್ಟುಗಳು();
}

ಸ್ಥಿರಾಂಕ std::string& Const::get_str() ಸ್ಥಿರಾಂಕ {
	check(is_str());
	ಹಿಂತಿರುಗಿ *get_if_str();
}

Const::bitvectype& Const::get_ಬಿಟ್ಟುಗಳು() {
	check(is_ಬಿಟ್ಟುಗಳು());
	ಹಿಂತಿರುಗಿ *get_if_ಬಿಟ್ಟುಗಳು();
}

std::string& Const::get_str() {
	check(is_str());
	ಹಿಂತಿರುಗಿ *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	ಹೊಸ ((ಶೂನ್ಯ*)&str_) std::string(std::move(str));
	ಟ್ಯಾಗ್ = backing_ಟ್ಯಾಗ್::string;
}

RTLIL::Const::Const(ದೀರ್ಘ ದೀರ್ಘ val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	ಅಕ್ಷರ ಬೈಟುಗಳು[] = {
		(ಅಕ್ಷರ)(val >> 24), (ಅಕ್ಷರ)(val >> 16), (ಅಕ್ಷರ)(val >> 8), (ಅಕ್ಷರ)val
	};
	ಹೊಸ ((ಶೂನ್ಯ*)&str_) std::string(ಬೈಟುಗಳು, 4);
	ಟ್ಯಾಗ್ = backing_ಟ್ಯಾಗ್::string;
}

RTLIL::Const::Const(ದೀರ್ಘ ದೀರ್ಘ val, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
	log_assert(ಅಗಲ >= 0 && ಅಗಲ < RTLIL::ಅಗಲ_ಎಲ್ಲೆ);
	flags = RTLIL::CONST_FLAG_NONE;
	ಆದರೆ ((ಅಗಲ & 7) == 0) {
		ಹೊಸ ((ಶೂನ್ಯ*)&str_) std::string();
		ಟ್ಯಾಗ್ = backing_ಟ್ಯಾಗ್::string;
		std::string& str = get_str();
		ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು = ಅಗಲ >> 3;
		ಚಿಹ್ನೆಯುಳ್ಳ ಅಕ್ಷರ sign_byte = val < 0 ? -1 : 0;
		str.resize(ಬೈಟುಗಳು, sign_byte);
		ಬೈಟುಗಳು = std::min<ಪೂರ್ಣಾಂಕ>(ಬೈಟುಗಳು, ಗಾತ್ರ(val));
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಬೈಟುಗಳು; i++) {
			str[str.ಗಾತ್ರ_2() - 1 - i] = val;
			val = val >> 8;
		}
		ಹಿಂತಿರುಗಿ;
	}

	ಹೊಸ ((ಶೂನ್ಯ*)&ಬಿಟ್ಟುಗಳು_2) bitvectype();
	ಟ್ಯಾಗ್ = backing_ಟ್ಯಾಗ್::ಬಿಟ್ಟುಗಳು;
	bitvectype& bv = get_ಬಿಟ್ಟುಗಳು();
	bv.reserve(ಅಗಲ);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++) {
		bv.ತಳ್ಳು_ಹಿಂಭಾಗ((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State ಬಿಟ್, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
	log_assert(ಅಗಲ >= 0 && ಅಗಲ < RTLIL::ಅಗಲ_ಎಲ್ಲೆ);
	flags = RTLIL::CONST_FLAG_NONE;
	ಹೊಸ ((ಶೂನ್ಯ*)&ಬಿಟ್ಟುಗಳು_2) bitvectype();
	ಟ್ಯಾಗ್ = backing_ಟ್ಯಾಗ್::ಬಿಟ್ಟುಗಳು;
	bitvectype& bv = get_ಬಿಟ್ಟುಗಳು();
	bv.reserve(ಅಗಲ);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++)
		bv.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್);
}

RTLIL::Const::Const(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ತಾರ್ಕಿಕ> &ಬಿಟ್ಟುಗಳು)
{
	flags = RTLIL::CONST_FLAG_NONE;
	ಹೊಸ ((ಶೂನ್ಯ*)&ಬಿಟ್ಟುಗಳು_2) bitvectype();
	ಟ್ಯಾಗ್ = backing_ಟ್ಯಾಗ್::ಬಿಟ್ಟುಗಳು;
	bitvectype& bv = get_ಬಿಟ್ಟುಗಳು();
	bv.reserve(ಬಿಟ್ಟುಗಳು.ಗಾತ್ರ_2());
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &b : ಬಿಟ್ಟುಗಳು)
		bv.emplace_ಹಿಂಭಾಗ(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ಸ್ಥಿರಾಂಕ RTLIL::Const &other) {
	ಟ್ಯಾಗ್ = other.ಟ್ಯಾಗ್;
	flags = other.flags;
	ಆದರೆ (is_str())
		ಹೊಸ ((ಶೂನ್ಯ*)&str_) std::string(other.get_str());
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_ಬಿಟ್ಟುಗಳು())
		ಹೊಸ ((ಶೂನ್ಯ*)&ಬಿಟ್ಟುಗಳು_2) bitvectype(other.get_ಬಿಟ್ಟುಗಳು());
	ಇಲ್ಲದಿದ್ದರೆ
		check(ಸುಳ್ಳು);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	ಟ್ಯಾಗ್ = other.ಟ್ಯಾಗ್;
	flags = other.flags;
	ಆದರೆ (is_str())
		ಹೊಸ ((ಶೂನ್ಯ*)&str_) std::string(std::move(other.get_str()));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_ಬಿಟ್ಟುಗಳು())
		ಹೊಸ ((ಶೂನ್ಯ*)&ಬಿಟ್ಟುಗಳು_2) bitvectype(std::move(other.get_ಬಿಟ್ಟುಗಳು()));
	ಇಲ್ಲದಿದ್ದರೆ
		check(ಸುಳ್ಳು);
}

RTLIL::Const &RTLIL::Const::ಕ್ರಿಯಾಕಾರಕ =(ಸ್ಥಿರಾಂಕ RTLIL::Const &other) {
	flags = other.flags;
	ಆದರೆ (other.is_str()) {
		ಆದರೆ (!is_str()) {
			// sketchy zone
			check(is_ಬಿಟ್ಟುಗಳು());
			ಬಿಟ್ಟುಗಳು_2.~bitvectype();
			(ಶೂನ್ಯ)ಹೊಸ ((ಶೂನ್ಯ*)&str_) std::string();
		}
		ಟ್ಯಾಗ್ = other.ಟ್ಯಾಗ್;
		get_str() = other.get_str();
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (other.is_ಬಿಟ್ಟುಗಳು()) {
		ಆದರೆ (!is_ಬಿಟ್ಟುಗಳು()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(ಶೂನ್ಯ)ಹೊಸ ((ಶೂನ್ಯ*)&ಬಿಟ್ಟುಗಳು_2) bitvectype();
		}
		ಟ್ಯಾಗ್ = other.ಟ್ಯಾಗ್;
		get_ಬಿಟ್ಟುಗಳು() = other.get_ಬಿಟ್ಟುಗಳು();
	} ಇಲ್ಲದಿದ್ದರೆ {
		check(ಸುಳ್ಳು);
	}
	ಹಿಂತಿರುಗಿ *ಇದು;
}

RTLIL::Const::~Const() {
	ಆದರೆ (is_ಬಿಟ್ಟುಗಳು())
		ಬಿಟ್ಟುಗಳು_2.~bitvectype();
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_str())
		str_.~string();
	ಇಲ್ಲದಿದ್ದರೆ
		check(ಸುಳ್ಳು);
}

ತಾರ್ಕಿಕ RTLIL::Const::ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ RTLIL::Const &other) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಗಾತ್ರ_2() != other.ಗಾತ್ರ_2())
		ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2() < other.ಗಾತ್ರ_2();

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಗಾತ್ರ_2(); i++)
		ಆದರೆ ((*ಇದು)[i] != other[i])
			ಹಿಂತಿರುಗಿ (*ಇದು)[i] < other[i];

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ RTLIL::Const::ಕ್ರಿಯಾಕಾರಕ ==(ಸ್ಥಿರಾಂಕ RTLIL::Const &other) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (is_str() && other.is_str())
		ಹಿಂತಿರುಗಿ get_str() == other.get_str();
	ಆದರೆ (is_ಬಿಟ್ಟುಗಳು() && other.is_ಬಿಟ್ಟುಗಳು())
		ಹಿಂತಿರುಗಿ get_ಬಿಟ್ಟುಗಳು() == other.get_ಬಿಟ್ಟುಗಳು();

	ಆದರೆ (ಗಾತ್ರ_2() != other.ಗಾತ್ರ_2())
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಗಾತ್ರ_2(); i++)
	ಆದರೆ ((*ಇದು)[i] != other[i])
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::Const::ಕ್ರಿಯಾಕಾರಕ !=(ಸ್ಥಿರಾಂಕ RTLIL::Const &other) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ !(*ಇದು == other);
}

std::ಸದಿಶ<RTLIL::State>& RTLIL::Const::ಬಿಟ್ಟುಗಳು_ಆಂತರಿಕ()
{
	bitvectorize_ಆಂತರಿಕ();
	ಹಿಂತಿರುಗಿ get_ಬಿಟ್ಟುಗಳು();
}

std::ಸದಿಶ<RTLIL::State> RTLIL::Const::ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು() ಸ್ಥಿರಾಂಕ
{
	std::ಸದಿಶ<State> v;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : *ಇದು)
		v.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್);
	ಹಿಂತಿರುಗಿ v;
}

ತಾರ್ಕಿಕ RTLIL::Const::as_bool() ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (is_str()) {
		ಗಾಗಿ (ಅಕ್ಷರ ch : get_str())
			ಆದರೆ (ch != 0)
				ಹಿಂತಿರುಗಿ ನಿಜ;
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಸ್ಥಿರಾಂಕ bitvectype& bv = get_ಬಿಟ್ಟುಗಳು();
	ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < bv.ಗಾತ್ರ_2(); i++)
		ಆದರೆ (bv[i] == State::S1)
			ಹಿಂತಿರುಗಿ ನಿಜ;
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಪೂರ್ಣಾಂಕ RTLIL::Const::as_int(ತಾರ್ಕಿಕ is_signed) ಸ್ಥಿರಾಂಕ
{
	int32_t ret = 0;
	ಆದರೆ (ಸ್ಥಿರಾಂಕ std::string *s = get_if_str()) {
		ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = Get_ಗಾತ್ರ(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = std::min(4, ಗಾತ್ರ_2); i > 0; i--)
			ret |= static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>((*s)[ಗಾತ್ರ_2 - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		ಆದರೆ (is_signed && ಗಾತ್ರ_2 > 0 && ಗಾತ್ರ_2 < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << ಗಾತ್ರ_2*8;
		ಹಿಂತಿರುಗಿ ret;
	}

	ಸ್ಥಿರಾಂಕ bitvectype& bv = get_ಬಿಟ್ಟುಗಳು();
	ಪೂರ್ಣಾಂಕ significant_ಬಿಟ್ಟುಗಳು = std::min(Get_ಗಾತ್ರ(bv), 32);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < significant_ಬಿಟ್ಟುಗಳು; i++)
		ಆದರೆ (bv[i] == State::S1)
			ret |= 1 << i;
	ಆದರೆ (is_signed && significant_ಬಿಟ್ಟುಗಳು > 0 && significant_ಬಿಟ್ಟುಗಳು < 32 && bv.ಹಿಂಭಾಗ() == State::S1 )
		ret |= UINT32_MAX << significant_ಬಿಟ್ಟುಗಳು;
	ಹಿಂತಿರುಗಿ ret;
}

ತಾರ್ಕಿಕ RTLIL::Const::convertible_ಗಮ್ಯ_int(ತಾರ್ಕಿಕ is_signed) ಸ್ಥಿರಾಂಕ
{
	ಸ್ವಯಂಚಾಲಿತ ಗಾತ್ರ_2 = get_min_ಗಾತ್ರ(is_signed);

	ಆದರೆ (ಗಾತ್ರ_2 < 0)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	// If it fits in 31 bits it is definitely convertible
	ಆದರೆ (ಗಾತ್ರ_2 <= 31)
		ಹಿಂತಿರುಗಿ ನಿಜ;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	ಆದರೆ (ಗಾತ್ರ_2 == 32) {
		ಆದರೆ (is_signed)
			ಹಿಂತಿರುಗಿ ನಿಜ;
		ಹಿಂತಿರುಗಿ (*ಇದು)[ಗಾತ್ರ_2 - 1] != State::S1;
	}

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

std::optional<ಪೂರ್ಣಾಂಕ> RTLIL::Const::try_as_int(ತಾರ್ಕಿಕ is_signed) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (!convertible_ಗಮ್ಯ_int(is_signed))
		ಹಿಂತಿರುಗಿ std::nullopt;
	ಹಿಂತಿರುಗಿ as_int(is_signed);
}

ಪೂರ್ಣಾಂಕ RTLIL::Const::as_int_saturating(ತಾರ್ಕಿಕ is_signed) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (!convertible_ಗಮ್ಯ_int(is_signed)) {
		ಆದರೆ (!is_signed)
			ಹಿಂತಿರುಗಿ std::numeric_limits<ಪೂರ್ಣಾಂಕ>::max();

		ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ min_ಗಾತ್ರ = get_min_ಗಾತ್ರ(is_signed);
		log_assert(min_ಗಾತ್ರ > 0);
		ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ neg = (*ಇದು)[min_ಗಾತ್ರ - 1];
		ಹಿಂತಿರುಗಿ neg ? std::numeric_limits<ಪೂರ್ಣಾಂಕ>::min() : std::numeric_limits<ಪೂರ್ಣಾಂಕ>::max();
	}
	ಹಿಂತಿರುಗಿ as_int(is_signed);
}

ಶೂನ್ಯ RTLIL::Const::ಟ್ಯಾಗ್_bare_integer_const(ಸ್ಥಿರಾಂಕ std::string &ಮೌಲ್ಯ)
{
	ಆದರೆ (ಮೌಲ್ಯ.ಖಾಲಿಗಳು() || ಮೌಲ್ಯ.ಕಂಡುಹಿಡಿ('\'') != std::string::npos)
		ಹಿಂತಿರುಗಿ;
	ಗಾತ್ರ_t ಆರಂಭ = (ಮೌಲ್ಯ[0] == '-' || ಮೌಲ್ಯ[0] == '+') ? 1 : 0;
	ಆದರೆ (ಆರಂಭ < ಮೌಲ್ಯ.ಗಾತ್ರ_2() && std::all_ದ(ಮೌಲ್ಯ.begin() + ಆರಂಭ, ಮೌಲ್ಯ.ಕೊನೆ(), ::ಅಂಕಿಯೇ))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

ಪೂರ್ಣಾಂಕ RTLIL::Const::get_min_ಗಾತ್ರ(ತಾರ್ಕಿಕ is_signed) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಖಾಲಿಗಳು()) ಹಿಂತಿರುಗಿ 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_ಬಿಟ್;
	ಆದರೆ (is_signed)
		leading_ಬಿಟ್ = (ಹಿಂಭಾಗ() == RTLIL::State::Sx) ? RTLIL::State::S0 : ಹಿಂಭಾಗ();
	ಇಲ್ಲದಿದ್ದರೆ
		leading_ಬಿಟ್ = RTLIL::State::S0;

	ಸ್ವಯಂಚಾಲಿತ idx = ಗಾತ್ರ_2();
	ಆಗ (idx > 0 && (*ಇದು)[idx -1] == leading_ಬಿಟ್) {
		idx--;
	}

	// signed needs one leading bit
	ಆದರೆ (is_signed && idx < ಗಾತ್ರ_2()) {
		idx++;
	}
	// must be at least one bit
	ಹಿಂತಿರುಗಿ (idx == 0) ? 1 : idx;
}

ಶೂನ್ಯ RTLIL::Const::compress(ತಾರ್ಕಿಕ is_signed)
{
	ಸ್ವಯಂಚಾಲಿತ idx = get_min_ಗಾತ್ರ(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<ಪೂರ್ಣಾಂಕ> RTLIL::Const::as_int_compress(ತಾರ್ಕಿಕ is_signed) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* any) ಸ್ಥಿರಾಂಕ
{
	ಪೂರ್ಣಾಂಕ sz = ಗಾತ್ರ_2();
	std::string ret;
	ret.reserve(sz);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = sz - 1; i >= 0; --i)
		ಆಯ್ಕೆ ((*ಇದು)[i]) {
			ಸ್ಥಿತಿ S0: ret.ತಳ್ಳು_ಹಿಂಭಾಗ('0'); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ S1: ret.ತಳ್ಳು_ಹಿಂಭಾಗ('1'); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Sx: ret.ತಳ್ಳು_ಹಿಂಭಾಗ('x'); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Sz: ret.ತಳ್ಳು_ಹಿಂಭಾಗ('z'); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Sa: ret += any; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Sm: ret.ತಳ್ಳು_ಹಿಂಭಾಗ('m'); ನಿಲ್ಲಿಸು;
		}
	ಹಿಂತಿರುಗಿ ret;
}

RTLIL::Const RTLIL::Const::from_string(ಸ್ಥಿರಾಂಕ std::string &str)
{
	Const c;
	bitvectype& bv = c.get_ಬಿಟ್ಟುಗಳು();
	bv.reserve(str.ಗಾತ್ರ_2());
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = str.rbegin(); it != str.rend(); it++)
		ಆಯ್ಕೆ (*it) {
			ಸ್ಥಿತಿ '0': bv.ತಳ್ಳು_ಹಿಂಭಾಗ(State::S0); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ '1': bv.ತಳ್ಳು_ಹಿಂಭಾಗ(State::S1); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 'x': bv.ತಳ್ಳು_ಹಿಂಭಾಗ(State::Sx); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 'z': bv.ತಳ್ಳು_ಹಿಂಭಾಗ(State::Sz); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ 'm': bv.ತಳ್ಳು_ಹಿಂಭಾಗ(State::Sm); ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ: bv.ತಳ್ಳು_ಹಿಂಭಾಗ(State::Sa);
		}
	ಹಿಂತಿರುಗಿ c;
}

std::string RTLIL::Const::decode_string() ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಸ್ವಯಂಚಾಲಿತ str = get_if_str())
		ಹಿಂತಿರುಗಿ *str;

	ಸ್ಥಿರಾಂಕ bitvectype& bv = get_ಬಿಟ್ಟುಗಳು();
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ n = Get_ಗಾತ್ರ(bv);
	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	ಪೂರ್ಣಾಂಕ i = n_over_8 * 8;
	ಆದರೆ (i < n) {
		ಅಕ್ಷರ ch = 0;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < (n - i); j++) {
			ಆದರೆ (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		ಆದರೆ (ch != 0)
			s.append({ch});
	}
	i -= 8;
	ಗಾಗಿ (; i >= 0; i -= 8) {
		ಅಕ್ಷರ ch = 0;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < 8; j++) {
			ಆದರೆ (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		ಆದರೆ (ch != 0)
			s.append({ch});
	}
	ಹಿಂತಿರುಗಿ s;
}

ಪೂರ್ಣಾಂಕ RTLIL::Const::ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (is_str())
		ಹಿಂತಿರುಗಿ 8 * str_.ಗಾತ್ರ_2();
	ಇಲ್ಲದಿದ್ದರೆ {
		check(is_ಬಿಟ್ಟುಗಳು());
		ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2.ಗಾತ್ರ_2();
	}
}

ತಾರ್ಕಿಕ RTLIL::Const::ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (is_str())
		ಹಿಂತಿರುಗಿ str_.ಖಾಲಿಗಳು();
	ಇಲ್ಲದಿದ್ದರೆ {
		check(is_ಬಿಟ್ಟುಗಳು());
		ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2.ಖಾಲಿಗಳು();
	}
}

ಶೂನ್ಯ RTLIL::Const::bitvectorize_ಆಂತರಿಕ() {
	ಆದರೆ (ಟ್ಯಾಗ್ == backing_ಟ್ಯಾಗ್::ಬಿಟ್ಟುಗಳು)
		ಹಿಂತಿರುಗಿ;

	check(is_str());

	bitvectype new_ಬಿಟ್ಟುಗಳು;

	new_ಬಿಟ್ಟುಗಳು.reserve(str_.ಗಾತ್ರ_2() * 8);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = str_.ಗಾತ್ರ_2() - 1; i >= 0; i--) {
		ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ch = str_[i];
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < 8; j++) {
			new_ಬಿಟ್ಟುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(ಶೂನ್ಯ)ಹೊಸ ((ಶೂನ್ಯ*)&ಬಿಟ್ಟುಗಳು_2) bitvectype(std::move(new_ಬಿಟ್ಟುಗಳು));
		ಟ್ಯಾಗ್ = backing_ಟ್ಯಾಗ್::ಬಿಟ್ಟುಗಳು;
	}
}

ಶೂನ್ಯ RTLIL::Const::append(ಸ್ಥಿರಾಂಕ RTLIL::Const &other) {
	bitvectorize_ಆಂತರಿಕ();
	bitvectype& bv = get_ಬಿಟ್ಟುಗಳು();
	bv.ಸೇರಿಸು(bv.ಕೊನೆ(), other.begin(), other.ಕೊನೆ());
}

RTLIL::State RTLIL::Const::const_iterator::ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (ಸ್ವಯಂಚಾಲಿತ bv = ಪೋಷಕ->get_if_ಬಿಟ್ಟುಗಳು())
		ಹಿಂತಿರುಗಿ (*bv)[idx];

	ಪೂರ್ಣಾಂಕ char_idx = ಪೋಷಕ->get_str().ಗಾತ್ರ_2() - idx / 8 - 1;
	ತಾರ್ಕಿಕ ಬಿಟ್ = (ಪೋಷಕ->get_str()[char_idx] & (1 << (idx % 8)));
	ಹಿಂತಿರುಗಿ ಬಿಟ್ ? State::S1 : State::S0;
}

ತಾರ್ಕಿಕ RTLIL::Const::is_fully_zero() ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಸ್ವಯಂಚಾಲಿತ str = get_if_str()) {
		ಗಾಗಿ (ಅಕ್ಷರ ch : *str)
			ಆದರೆ (ch != 0)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಸ್ಥಿರಾಂಕ bitvectype& bv = get_ಬಿಟ್ಟುಗಳು();

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : bv)
		ಆದರೆ (ಬಿಟ್ != RTLIL::State::S0)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::Const::is_fully_ones() ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಸ್ವಯಂಚಾಲಿತ str = get_if_str()) {
		ಗಾಗಿ (ಅಕ್ಷರ ch : *str)
			ಆದರೆ (ch != (ಅಕ್ಷರ)0xff)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಸ್ಥಿರಾಂಕ bitvectype& bv = get_ಬಿಟ್ಟುಗಳು();
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : bv)
		ಆದರೆ (ಬಿಟ್ != RTLIL::State::S1)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::Const::is_fully_def() ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (is_str())
		ಹಿಂತಿರುಗಿ ನಿಜ;

	ಸ್ಥಿರಾಂಕ bitvectype& bv = get_ಬಿಟ್ಟುಗಳು();
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : bv)
		ಆದರೆ (ಬಿಟ್ != RTLIL::State::S0 && ಬಿಟ್ != RTLIL::State::S1)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::Const::is_fully_undef() ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಸ್ವಯಂಚಾಲಿತ str = get_if_str())
		ಹಿಂತಿರುಗಿ str->ಖಾಲಿಗಳು();

	ಸ್ಥಿರಾಂಕ bitvectype& bv = get_ಬಿಟ್ಟುಗಳು();
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : bv)
		ಆದರೆ (ಬಿಟ್ != RTLIL::State::Sx && ಬಿಟ್ != RTLIL::State::Sz)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::Const::is_fully_undef_x_only() ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಸ್ವಯಂಚಾಲಿತ str = get_if_str())
		ಹಿಂತಿರುಗಿ str->ಖಾಲಿಗಳು();

	ಸ್ಥಿರಾಂಕ bitvectype& bv = get_ಬಿಟ್ಟುಗಳು();
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : bv)
		ಆದರೆ (ಬಿಟ್ != RTLIL::State::Sx)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::Const::is_onehot(ಪೂರ್ಣಾಂಕ *pos) ಸ್ಥಿರಾಂಕ
{
	ತಾರ್ಕಿಕ ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;
	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = Get_ಗಾತ್ರ(*ಇದು);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಗಾತ್ರ_2; i++) {
		State ಬಿಟ್ = (*ಇದು)[i];
		ಆದರೆ (ಬಿಟ್ != RTLIL::State::S0 && ಬಿಟ್ != RTLIL::State::S1)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಆದರೆ (ಬಿಟ್ == RTLIL::State::S1) {
			ಆದರೆ (ಸಿಕ್ಕಿತು)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (pos)
				*pos = i;
			ಸಿಕ್ಕಿತು = ನಿಜ;
		}
	}
	ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}

Hasher RTLIL::Const::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಸ್ವಯಂಚಾಲಿತ str = get_if_str())
		ಹಿಂತಿರುಗಿ hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ಸ್ಥಿರಾಂಕ bitvectype &bv = get_ಬಿಟ್ಟುಗಳು();
	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = Get_ಗಾತ್ರ(bv);
	std::string packed;
	ಪೂರ್ಣಾಂಕ packed_ಗಾತ್ರ = (ಗಾತ್ರ_2 + 7) >> 3;
	packed.resize(packed_ಗಾತ್ರ, 0);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ bi = 0; bi < packed_ಗಾತ್ರ; ++bi) {
		ಅಕ್ಷರ ch = 0;
		ಪೂರ್ಣಾಂಕ ಕೊನೆ = std::min((bi + 1)*8, ಗಾತ್ರ_2);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = bi*8; i < ಕೊನೆ; ++i) {
			RTLIL::State b = bv[i];
			ಆದರೆ (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<ಅಕ್ಷರ>::hash_into(ch, h);
				ಗಾಗಿ (; i < ಗಾತ್ರ_2; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(ಗಾತ್ರ_2);
				ಹಿಂತಿರುಗಿ h;
			}
			ch |= static_cast<ಪೂರ್ಣಾಂಕ>(b) << (i & 7);
		}
		packed[packed_ಗಾತ್ರ - 1 - bi] = ch;
	}
	ಹಿಂತಿರುಗಿ hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::ಹೊರತೆಗೆ(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ len, RTLIL::State padding) ಸ್ಥಿರಾಂಕ {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = ಸ್ಥಾನಾಂತರ; i < ಸ್ಥಾನಾಂತರ + len; i++)
		ret_bv.ತಳ್ಳು_ಹಿಂಭಾಗ(i < Get_ಗಾತ್ರ(*ಇದು) ? (*ಇದು)[i] : padding);
	ಹಿಂತಿರುಗಿ RTLIL::Const(ret_bv);
}
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು check /* check(condition) for Const */

ತಾರ್ಕಿಕ RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ ಗುಣಲಕ್ಷಣಗಳು.ಎಣಿಕೆ_2(id);
}

ಶೂನ್ಯ RTLIL::AttrObject::ಗಣ_bool_attribute(RTLIL::IdString id, ತಾರ್ಕಿಕ ಮೌಲ್ಯ)
{
	ಆದರೆ (ಮೌಲ್ಯ)
		ಗುಣಲಕ್ಷಣಗಳು[id] = RTLIL::Const(1);
	ಇಲ್ಲದಿದ್ದರೆ
		ಗುಣಲಕ್ಷಣಗಳು.erase(id);
}

ತಾರ್ಕಿಕ RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ it = ಗುಣಲಕ್ಷಣಗಳು.ಕಂಡುಹಿಡಿ(id);
	ಆದರೆ (it == ಗುಣಲಕ್ಷಣಗಳು.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಹಿಂತಿರುಗಿ it->ಎರಡನೇ.as_bool();
}

ಶೂನ್ಯ RTLIL::AttrObject::ಗಣ_string_attribute(RTLIL::IdString id, string ಮೌಲ್ಯ)
{
	ಆದರೆ (ಮೌಲ್ಯ.ಖಾಲಿಗಳು())
		ಗುಣಲಕ್ಷಣಗಳು.erase(id);
	ಇಲ್ಲದಿದ್ದರೆ
		ಗುಣಲಕ್ಷಣಗಳು[id] = ಮೌಲ್ಯ;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ಸ್ಥಿರಾಂಕ
{
	std::string ಮೌಲ್ಯ;
	ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ it = ಗುಣಲಕ್ಷಣಗಳು.ಕಂಡುಹಿಡಿ(id);
	ಆದರೆ (it != ಗುಣಲಕ್ಷಣಗಳು.ಕೊನೆ())
		ಮೌಲ್ಯ = it->ಎರಡನೇ.decode_string();
	ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ;
}

ಶೂನ್ಯ RTLIL::AttrObject::ಗಣ_strpool_attribute(RTLIL::IdString id, ಸ್ಥಿರಾಂಕ pool<string> &ದತ್ತಾಂಶ)
{
	string attrval;
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &s : ದತ್ತಾಂಶ) {
		ಆದರೆ (!attrval.ಖಾಲಿಗಳು())
			attrval += "|";
		attrval += s;
	}
	ಗಣ_string_attribute(id, attrval);
}

ಶೂನ್ಯ RTLIL::AttrObject::ಕೂಡಿಸು_strpool_attribute(RTLIL::IdString id, ಸ್ಥಿರಾಂಕ pool<string> &ದತ್ತಾಂಶ)
{
	pool<string> union_ದತ್ತಾಂಶ = get_strpool_attribute(id);
	union_ದತ್ತಾಂಶ.ಸೇರಿಸು(ದತ್ತಾಂಶ.begin(), ದತ್ತಾಂಶ.ಕೊನೆ());
	ಆದರೆ (!union_ದತ್ತಾಂಶ.ಖಾಲಿಗಳು())
		ಗಣ_strpool_attribute(id, union_ದತ್ತಾಂಶ);
}

ಶೂನ್ಯ RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->ಗುಣಲಕ್ಷಣಗಳು = std::move(buf);
}

ನಾಮಾಕಾಶ {
	ಮಾದರಿ<ಬಗೆ_ಹೆಸರು F>
	ಶೂನ್ಯ walk_process_attr_objects(RTLIL::Process *process, F ಭೇಟಿ) {
		ಭೇಟಿ(process);
		std::ಸದಿಶ<RTLIL::Case_ನಿಯಮ*> case_ರಾಶಿ{&process->ಬೇರು_case};
		ಆಗ (!case_ರಾಶಿ.ಖಾಲಿಗಳು()) {
			RTLIL::Case_ನಿಯಮ *cs = case_ರಾಶಿ.ಹಿಂಭಾಗ();
			case_ರಾಶಿ.ತೆಗೆ_ಹಿಂಭಾಗ();
			ಭೇಟಿ(cs);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ *sw : cs->switches) {
				ಭೇಟಿ(sw);
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ *case_ : sw->cases)
					case_ರಾಶಿ.ತಳ್ಳು_ಹಿಂಭಾಗ(case_);
			}
		}
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ *ಸಮಕಾಲೀಕರಣ : process->syncs)
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &mwa : ಸಮಕಾಲೀಕರಣ->mem_write_actions)
				ಭೇಟಿ(&mwa);
	}

	ಮಾದರಿ<ಬಗೆ_ಹೆಸರು F>
	ಶೂನ್ಯ walk_attr_objects(RTLIL::Design *design, F ಭೇಟಿ) {
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[_, module] : design->modules_) {
			ಭೇಟಿ(module);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[_, wire] : module->wires_)
				ಭೇಟಿ(wire);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[_, mem] : module->memories)
				ಭೇಟಿ(mem);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[_, cell] : module->cells_)
				ಭೇಟಿ(cell);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[_, process] : module->processes)
				walk_process_attr_objects(process, ಭೇಟಿ);
		}
	}
}


ಗಾತ್ರ_t RTLIL::Design::gc_twines()
{
	int64_t ಆರಂಭ = PerformanceTimer::query();
	pool<IdString> ಜೀವಂತ;
	ಸ್ವಯಂಚಾಲಿತ ಬೇರು = [&](IdString ref) {
		ಆದರೆ (ref != IdString::Null)
			ಜೀವಂತ.ಸೇರಿಸು(ref);
	};

	walk_attr_objects(ಇದು, [&](ಸ್ಥಿರಾಂಕ RTLIL::AttrObject *obj) {
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &attr : obj->ಗುಣಲಕ್ಷಣಗಳು)
			ಬೇರು(attr.ಮೊದಲ);
	});

	ಬೇರು(selected_ಸಕ್ರಿಯ_module);

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[name, module] : modules_) {
		ಬೇರು(name);
		ಗಾಗಿ (IdString ಪೋರ್ಟ್ : module->ports)
			ಬೇರು(ಪೋರ್ಟ್);
		ಗಾಗಿ (IdString param : module->avail_parameters)
			ಬೇರು(param);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[param, _] : module->parameter_default_ಮೌಲ್ಯಗಳು)
			ಬೇರು(param);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[name, _] : module->memories)
			ಬೇರು(name);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[name, _] : module->processes)
			ಬೇರು(name);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[name, wire] : module->wires_) {
			ಬೇರು(name);
			ಆದರೆ (wire->ತಿಳಿದ_driver())
				ಬೇರು(wire->driver_ಪೋರ್ಟ್());
		}
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[name, cell] : module->cells_) {
			ಬೇರು(name);
			ಬೇರು(cell->type);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &conn : cell->connections())
				ಬೇರು(conn.ಮೊದಲ);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &param : cell->parameters)
				ಬೇರು(param.ಮೊದಲ);
		}
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[_, process] : module->processes) {
			std::ಸದಿಶ<RTLIL::Case_ನಿಯಮ*> case_ರಾಶಿ{&process->ಬೇರು_case};
			ಆಗ (!case_ರಾಶಿ.ಖಾಲಿಗಳು()) {
				RTLIL::Case_ನಿಯಮ *cs = case_ರಾಶಿ.ಹಿಂಭಾಗ();
				case_ರಾಶಿ.ತೆಗೆ_ಹಿಂಭಾಗ();
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ *sw : cs->switches) {
					ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ *case_ : sw->cases)
						case_ರಾಶಿ.ತಳ್ಳು_ಹಿಂಭಾಗ(case_);
				}
			}
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ *ಸಮಕಾಲೀಕರಣ : process->syncs) {
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &mwa : ಸಮಕಾಲೀಕರಣ->mem_write_actions)
					ಬೇರು(mwa.memid);
			}
		}
	}

	ಸ್ವಯಂಚಾಲಿತ ಬೇರು_selection = [&](ಸ್ಥಿರಾಂಕ RTLIL::Selection &sel) {
		ಗಾಗಿ (IdString m : sel.selected_modules)
			ಬೇರು(m);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[m, members] : sel.selected_members) {
			ಬೇರು(m);
			ಗಾಗಿ (IdString member : members)
				ಬೇರು(member);
		}
	};

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &[name, sel] : selection_vars) {
		ಬೇರು(name);
		ಬೇರು_selection(sel);
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &sel : selection_ರಾಶಿ)
		ಬೇರು_selection(sel);

	ಗಾತ್ರ_t erased = twines_.gc(ಜೀವಂತ);

	int64_t ಸಮಯ_ns = PerformanceTimer::query() - ಆರಂಭ;
	Pass::ಕಳೆ_from_current_runtime_ns(ಸಮಯ_ns);
	twine_gc_ns += ಸಮಯ_ns;
	++twine_gc_ಎಣಿಕೆ;
	ಹಿಂತಿರುಗಿ erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ಸ್ಥಿರಾಂಕ
{
	pool<string> ದತ್ತಾಂಶ;
	ಆದರೆ (ಗುಣಲಕ್ಷಣಗಳು.ಎಣಿಕೆ_2(id) != 0)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ s : ವಿಭಜಿಸು_ಟೋಕನುಗಳು(get_string_attribute(id), "|"))
			ದತ್ತಾಂಶ.ಸೇರಿಸು(s);
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
}

ಶೂನ್ಯ RTLIL::AttrObject::ಗಣ_hdlname_attribute(ಸ್ಥಿರಾಂಕ ಸದಿಶ<string> &hierarchy)
{
	string attrval;
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ident : hierarchy) {
		ಆದರೆ (!attrval.ಖಾಲಿಗಳು())
			attrval += " ";
		attrval += ident;
	}
	ಗಣ_string_attribute(ID::hdlname, attrval);
}

ಸದಿಶ<string> RTLIL::AttrObject::get_hdlname_attribute() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ ವಿಭಜಿಸು_ಟೋಕನುಗಳು(get_string_attribute(ID::hdlname), " ");
}

ಶೂನ್ಯ RTLIL::AttrObject::ಗಣ_intvec_attribute(RTLIL::IdString id, ಸ್ಥಿರಾಂಕ ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &ದತ್ತಾಂಶ)
{
	std::stringstream attrval;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &i : ದತ್ತಾಂಶ) {
		ಆದರೆ (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	ಗುಣಲಕ್ಷಣಗಳು[id] = RTLIL::Const(attrval.str());
}

ಸದಿಶ<ಪೂರ್ಣಾಂಕ> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ಸ್ಥಿರಾಂಕ
{
	ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ದತ್ತಾಂಶ;
	ಸ್ವಯಂಚಾಲಿತ it = ಗುಣಲಕ್ಷಣಗಳು.ಕಂಡುಹಿಡಿ(id);
	ಆದರೆ (it != ಗುಣಲಕ್ಷಣಗಳು.ಕೊನೆ())
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &s : ವಿಭಜಿಸು_ಟೋಕನುಗಳು(ಗುಣಲಕ್ಷಣಗಳು.at(id).decode_string())) {
			ಅಕ್ಷರ *ಕೊನೆ = ಶೂನ್ಯಸೂಚಕ;
			errno = 0;
			ದೀರ್ಘ ಮೌಲ್ಯ = strtol(s.c_str(), &ಕೊನೆ, 10);
			ಆದರೆ (ಕೊನೆ != s.c_str() + s.ಗಾತ್ರ_2())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			ಆದರೆ (errno == ERANGE || ಮೌಲ್ಯ < INT_MIN || ಮೌಲ್ಯ > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			ದತ್ತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(ಮೌಲ್ಯ);
		}
	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
}

ತಾರ್ಕಿಕ RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (current_design != ಶೂನ್ಯಸೂಚಕ) {
		ಸ್ವಯಂಚಾಲಿತ module = current_design->module(mod_name);
		ಹಿಂತಿರುಗಿ module && module->get_blackbox_attribute();
	} ಇಲ್ಲದಿದ್ದರೆ {
		log_warning("Unable to check if module is boxed for null design.\n");
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}
}

ತಾರ್ಕಿಕ RTLIL::Selection::selected_module(RTLIL::IdString mod_name) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (complete_selection)
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಆದರೆ (!selects_boxes && boxed_module(mod_name))
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಆದರೆ (ತುಂಬಿದ_selection)
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಆದರೆ (selected_modules.ಎಣಿಕೆ_2(mod_name) > 0)
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಆದರೆ (selected_members.ಎಣಿಕೆ_2(mod_name) > 0)
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ RTLIL::Selection::selected_ಸಂಪೂರ್ಣ_module(RTLIL::IdString mod_name) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (complete_selection)
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಆದರೆ (!selects_boxes && boxed_module(mod_name))
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಆದರೆ (ತುಂಬಿದ_selection)
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಆದರೆ (selected_modules.ಎಣಿಕೆ_2(mod_name) > 0)
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (complete_selection)
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಆದರೆ (!selects_boxes && boxed_module(mod_name))
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಆದರೆ (ತುಂಬಿದ_selection)
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಆದರೆ (selected_modules.ಎಣಿಕೆ_2(mod_name) > 0)
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಆದರೆ (selected_members.ಎಣಿಕೆ_2(mod_name) > 0)
		ಆದರೆ (selected_members.at(mod_name).ಎಣಿಕೆ_2(memb_name) > 0)
			ಹಿಂತಿರುಗಿ ನಿಜ;
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ RTLIL::Selection::optimize(RTLIL::Design *design)
{
	ಆದರೆ (design != current_design) {
		current_design = design;
	}

	ಆದರೆ (selects_boxes && ತುಂಬಿದ_selection)
		complete_selection = ನಿಜ;
	ಆದರೆ (complete_selection) {
		ತುಂಬಿದ_selection = ಸುಳ್ಳು;
		selects_boxes = ನಿಜ;
	}
	ಆದರೆ (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		ಹಿಂತಿರುಗಿ;
	}

	std::ಸದಿಶ<RTLIL::IdString> del_list, ಕೂಡಿಸು_list;

	del_list.clear();
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mod_name : selected_modules) {
		ಆದರೆ (current_design->module(mod_name) == ಶೂನ್ಯಸೂಚಕ || (!selects_boxes && boxed_module(mod_name)))
			del_list.ತಳ್ಳು_ಹಿಂಭಾಗ(mod_name);
		selected_members.erase(mod_name);
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : selected_members)
		ಆದರೆ (current_design->module(it.ಮೊದಲ) == ಶೂನ್ಯಸೂಚಕ || (!selects_boxes && boxed_module(it.ಮೊದಲ)))
			del_list.ತಳ್ಳು_ಹಿಂಭಾಗ(it.ಮೊದಲ);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mod_name : del_list)
		selected_members.erase(mod_name);

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : selected_members) {
		del_list.clear();
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ memb_name : it.ಎರಡನೇ)
			ಆದರೆ (current_design->module(it.ಮೊದಲ)->ಎಣಿಕೆ_id(memb_name) == 0)
				del_list.ತಳ್ಳು_ಹಿಂಭಾಗ(memb_name);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ memb_name : del_list)
			it.ಎರಡನೇ.erase(memb_name);
	}

	del_list.clear();
	ಕೂಡಿಸು_list.clear();
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : selected_members)
		ಆದರೆ (it.ಎರಡನೇ.ಗಾತ್ರ_2() == 0)
			del_list.ತಳ್ಳು_ಹಿಂಭಾಗ(it.ಮೊದಲ);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (it.ಎರಡನೇ.ಗಾತ್ರ_2() == current_design->module(it.ಮೊದಲ)->wires_.ಗಾತ್ರ_2() + current_design->module(it.ಮೊದಲ)->memories.ಗಾತ್ರ_2() +
				current_design->module(it.ಮೊದಲ)->cells_.ಗಾತ್ರ_2() + current_design->module(it.ಮೊದಲ)->processes.ಗಾತ್ರ_2())
			ಕೂಡಿಸು_list.ತಳ್ಳು_ಹಿಂಭಾಗ(it.ಮೊದಲ);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mod_name : del_list)
		selected_members.erase(mod_name);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mod_name : ಕೂಡಿಸು_list) {
		selected_members.erase(mod_name);
		selected_modules.ಸೇರಿಸು(mod_name);
	}

	ಆದರೆ (selected_modules.ಗಾತ್ರ_2() == current_design->modules_.ಗಾತ್ರ_2()) {
		selected_modules.clear();
		selected_members.clear();
		ಆದರೆ (selects_boxes)
			complete_selection = ನಿಜ;
		ಇಲ್ಲದಿದ್ದರೆ
			ತುಂಬಿದ_selection = ನಿಜ;
	}
}

ಶೂನ್ಯ RTLIL::Selection::clear()
{
	ತುಂಬಿದ_selection = ಸುಳ್ಳು;
	complete_selection = ಸುಳ್ಳು;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_ನಿರೂಪಿಸುತ್ತದೆ (ಹೊಸ define_map_t)
{
	ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hashidx_ಎಣಿಕೆ = 123456789;
	hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
	hashidx_ = hashidx_ಎಣಿಕೆ;

	refcount_modules_ = 0;
	selected_ಸಕ್ರಿಯ_module.clear();
	ತಳ್ಳು_ತುಂಬಿದ_selection();

	RTLIL::Design::get_all_designs()->ಸೇರಿಸು(std::pair<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Design*>(hashidx_, ಇದು));
}

RTLIL::Design::~Design()
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &pr : modules_)
		ಅಳಿಸು pr.ಎರಡನೇ;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

ಸ್ಥಿರ std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Design*> all_designs;
std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Design*> *RTLIL::Design::get_all_designs(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	ಹಿಂತಿರುಗಿ RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ಸ್ಥಿರಾಂಕ RTLIL::Module *RTLIL::Design::module(IdString id) ಸ್ಥಿರಾಂಕ {
	twines_.check_owned(id);
	ಹಿಂತಿರುಗಿ modules_.ಎಣಿಕೆ_2(id) ? modules_.at(id) : ಖಾಲಿ;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	ಹಿಂತಿರುಗಿ modules_.ಎಣಿಕೆ_2(id) ? modules_.at(id) : ಖಾಲಿ;
}

RTLIL::Module *RTLIL::Design::ತುದಿ_module() ಸ್ಥಿರಾಂಕ
{
	RTLIL::Module *module = ಶೂನ್ಯಸೂಚಕ;
	ಪೂರ್ಣಾಂಕ module_ಎಣಿಕೆ = 0;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mod : selected_modules()) {
		ಆದರೆ (mod->get_bool_attribute(ID::ತುದಿ))
			ಹಿಂತಿರುಗಿ mod;
		module_ಎಣಿಕೆ++;
		module = mod;
	}

	ಹಿಂತಿರುಗಿ module_ಎಣಿಕೆ == 1 ? module : ಶೂನ್ಯಸೂಚಕ;
}

RTLIL::Module *RTLIL::Design::ಕೂಡಿಸು_Module(RTLIL::IdString name)
{
	ಆದರೆ (modules_.ಎಣಿಕೆ_2(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = ಹೊಸ RTLIL::Module;
	modules_[name] = module;
	module->design = ಇದು;
	module->name = name;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mon : monitors)
		mon->notify_module_ಕೂಡಿಸು(module);

	ಆದರೆ (yosys_xtrace) {
		ಲಘುಗಣಕ("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	ಹಿಂತಿರುಗಿ module;
}

ಶೂನ್ಯ RTLIL::Design::scratchpad_unset(ಸ್ಥಿರಾಂಕ std::string &varname)
{
	scratchpad.erase(varname);
}

ಶೂನ್ಯ RTLIL::Design::scratchpad_ಗಣ_int(ಸ್ಥಿರಾಂಕ std::string &varname, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ)
{
	scratchpad[varname] = stringf("%d", ಮೌಲ್ಯ);
}

ಶೂನ್ಯ RTLIL::Design::scratchpad_ಗಣ_bool(ಸ್ಥಿರಾಂಕ std::string &varname, ತಾರ್ಕಿಕ ಮೌಲ್ಯ)
{
	scratchpad[varname] = ಮೌಲ್ಯ ? "true" : "false";
}

ಶೂನ್ಯ RTLIL::Design::scratchpad_ಗಣ_string(ಸ್ಥಿರಾಂಕ std::string &varname, std::string ಮೌಲ್ಯ)
{
	scratchpad[varname] = std::move(ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ RTLIL::Design::scratchpad_get_int(ಸ್ಥಿರಾಂಕ std::string &varname, ಪೂರ್ಣಾಂಕ default_ಮೌಲ್ಯ_2) ಸ್ಥಿರಾಂಕ
{
	ಸ್ವಯಂಚಾಲಿತ it = scratchpad.ಕಂಡುಹಿಡಿ(varname);
	ಆದರೆ (it == scratchpad.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ default_ಮೌಲ್ಯ_2;

	ಸ್ಥಿರಾಂಕ std::string &str = it->ಎರಡನೇ;

	ಆದರೆ (str == "0" || str == "false")
		ಹಿಂತಿರುಗಿ 0;

	ಆದರೆ (str == "1" || str == "true")
		ಹಿಂತಿರುಗಿ 1;

	ಅಕ್ಷರ *endptr = ಶೂನ್ಯಸೂಚಕ;
	ದೀರ್ಘ ಪೂರ್ಣಾಂಕ parsed_ಮೌಲ್ಯ = strtol(str.c_str(), &endptr, 10);
	ಹಿಂತಿರುಗಿ *endptr ? default_ಮೌಲ್ಯ_2 : parsed_ಮೌಲ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::Design::scratchpad_get_bool(ಸ್ಥಿರಾಂಕ std::string &varname, ತಾರ್ಕಿಕ default_ಮೌಲ್ಯ_2) ಸ್ಥಿರಾಂಕ
{
	ಸ್ವಯಂಚಾಲಿತ it = scratchpad.ಕಂಡುಹಿಡಿ(varname);
	ಆದರೆ (it == scratchpad.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ default_ಮೌಲ್ಯ_2;

	ಸ್ಥಿರಾಂಕ std::string &str = it->ಎರಡನೇ;

	ಆದರೆ (str == "0" || str == "false")
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಆದರೆ (str == "1" || str == "true")
		ಹಿಂತಿರುಗಿ ನಿಜ;

	ಹಿಂತಿರುಗಿ default_ಮೌಲ್ಯ_2;
}

std::string RTLIL::Design::scratchpad_get_string(ಸ್ಥಿರಾಂಕ std::string &varname, ಸ್ಥಿರಾಂಕ std::string &default_ಮೌಲ್ಯ_2) ಸ್ಥಿರಾಂಕ
{
	ಸ್ವಯಂಚಾಲಿತ it = scratchpad.ಕಂಡುಹಿಡಿ(varname);
	ಆದರೆ (it == scratchpad.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ default_ಮೌಲ್ಯ_2;

	ಹಿಂತಿರುಗಿ it->ಎರಡನೇ;
}

ಶೂನ್ಯ RTLIL::Design::remove(RTLIL::Module *module)
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mon : monitors)
		mon->notify_module_del(module);

	ಆದರೆ (yosys_xtrace) {
		ಲಘುಗಣಕ("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	ಅಳಿಸು module;
}

ಶೂನ್ಯ RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	ಕೂಡಿಸು(module);
}

ಶೂನ್ಯ RTLIL::Design::ವಿಂಗಡಿಸು()
{
	scratchpad.ವಿಂಗಡಿಸು();
	modules_.ವಿಂಗಡಿಸು(ವಿಂಗಡಿಸು_by_id_str(twines_));
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : modules_)
		it.ಎರಡನೇ->ವಿಂಗಡಿಸು();
}

ಶೂನ್ಯ RTLIL::Design::ವಿಂಗಡಿಸು_modules()
{
	scratchpad.ವಿಂಗಡಿಸು();
	modules_.ವಿಂಗಡಿಸು(ವಿಂಗಡಿಸು_by_id_str(twines_));
}

ಶೂನ್ಯ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

ಶೂನ್ಯ RTLIL::Design::check()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NDEBUG
	log_assert(!selection_ರಾಶಿ.ಖಾಲಿಗಳು());
	ಪೂರ್ಣಾಂಕ pool_ಗಾತ್ರ = 0;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : modules_)
		pool_ಗಾತ್ರ = std::max(pool_ಗಾತ್ರ, ThreadPool::work_pool_ಗಾತ್ರ(0, it.ಎರಡನೇ->cells_ಗಾತ್ರ(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_ಗಾತ್ರ);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : modules_) {
		log_assert(ಇದು == it.ಎರಡನೇ->design);
		log_assert(it.ಮೊದಲ == it.ಎರಡನೇ->name);
		check_module(it.ಎರಡನೇ, thread_pool);
	}
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ RTLIL::Design::optimize()
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : modules_)
		it.ಎರಡನೇ->optimize();
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : selection_ರಾಶಿ)
		it.optimize(ಇದು);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : selection_vars)
		it.ಎರಡನೇ.optimize(ಇದು);
}

ಶೂನ್ಯ RTLIL::Design::clone_into(RTLIL::Design *dst) ಸ್ಥಿರಾಂಕ
{
	log_assert(dst->modules_.ಖಾಲಿಗಳು());
	dst->twines_ = twines_;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->ಎರಡನೇ->clone(dst);
}

ತಾರ್ಕಿಕ RTLIL::Design::selected_module(IdString mod_name) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (!selected_ಸಕ್ರಿಯ_module.ಖಾಲಿಗಳು() && mod_name != selected_ಸಕ್ರಿಯ_module)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಹಿಂತಿರುಗಿ selection().selected_module(mod_name);
}

ತಾರ್ಕಿಕ RTLIL::Design::selected_ಸಂಪೂರ್ಣ_module(RTLIL::IdString mod_name) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (!selected_ಸಕ್ರಿಯ_module.ಖಾಲಿಗಳು() && mod_name != selected_ಸಕ್ರಿಯ_module)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಹಿಂತಿರುಗಿ selection().selected_ಸಂಪೂರ್ಣ_module(mod_name);
}

ತಾರ್ಕಿಕ RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (!selected_ಸಕ್ರಿಯ_module.ಖಾಲಿಗಳು() && mod_name != selected_ಸಕ್ರಿಯ_module)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಹಿಂತಿರುಗಿ selection().selected_member(mod_name, memb_name);
}

ತಾರ್ಕಿಕ RTLIL::Design::selected_module(RTLIL::Module *mod) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ selected_module(mod->name);
}

ತಾರ್ಕಿಕ RTLIL::Design::selected_ಸಂಪೂರ್ಣ_module(RTLIL::Module *mod) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ selected_ಸಂಪೂರ್ಣ_module(mod->name);
}

ಶೂನ್ಯ RTLIL::Design::ತಳ್ಳು_selection(RTLIL::Selection sel)
{
	sel.current_design = ಇದು;
	selection_ರಾಶಿ.ತಳ್ಳು_ಹಿಂಭಾಗ(sel);
}

ಶೂನ್ಯ RTLIL::Design::ತಳ್ಳು_ಖಾಲಿಗಳು_selection()
{
	ತಳ್ಳು_selection(RTLIL::Selection::ಖಾಲಿಗಳು_Selection(ಇದು));
}

ಶೂನ್ಯ RTLIL::Design::ತಳ್ಳು_ತುಂಬಿದ_selection()
{
	ತಳ್ಳು_selection(RTLIL::Selection::ತುಂಬಿದ_Selection(ಇದು));
}

ಶೂನ್ಯ RTLIL::Design::ತಳ್ಳು_complete_selection()
{
	ತಳ್ಳು_selection(RTLIL::Selection::CompleteSelection(ಇದು));
}

ಶೂನ್ಯ RTLIL::Design::ತೆಗೆ_selection()
{
	selection_ರಾಶಿ.ತೆಗೆ_ಹಿಂಭಾಗ();
	// Default to a full_selection if we ran out of stack
	ಆದರೆ (selection_ರಾಶಿ.ಖಾಲಿಗಳು())
		ತಳ್ಳು_ತುಂಬಿದ_selection();
}

std::string RTLIL::Design::ಗಮ್ಯ_rtlil_str(ತಾರ್ಕಿಕ only_selected) ಸ್ಥಿರಾಂಕ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(ಇದು), only_selected, ನಿಜ, ಸುಳ್ಳು, RTLIL_BACKEND::Dump_ಬಹುಲಕ::Readable);
	ಹಿಂತಿರುಗಿ f.str();
}

std::ಸದಿಶ<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ಸ್ಥಿರಾಂಕ
{
	ತಾರ್ಕಿಕ include_partials = partials == RTLIL::SELECT_ALL;
	ತಾರ್ಕಿಕ exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	ತಾರ್ಕಿಕ ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::ಸದಿಶ<RTLIL::Module*> ಫಲಿತಾಂಶ;
	ಫಲಿತಾಂಶ.reserve(modules_.ಗಾತ್ರ_2());
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : modules_) {
		ಆದರೆ (selected_ಸಂಪೂರ್ಣ_module(it.ಮೊದಲ) || (include_partials && selected_module(it.ಮೊದಲ))) {
			ಆದರೆ (!(exclude_boxes && it.ಎರಡನೇ->get_blackbox_attribute(ignore_wb)))
				ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(it.ಎರಡನೇ);
			ಇಲ್ಲದಿದ್ದರೆ
				ಆಯ್ಕೆ (boxes)
				{
				ಸ್ಥಿತಿ RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.ಎರಡನೇ);
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.ಎರಡನೇ);
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.ಎರಡನೇ);
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.ಎರಡನೇ);
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.ಎರಡನೇ);
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.ಎರಡನೇ);
					ನಿಲ್ಲಿಸು;
				ಪೂರ್ವನಿಯೋಜಿತ:
					ನಿಲ್ಲಿಸು;
				}
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!include_partials && selected_module(it.ಮೊದಲ)) {
			ಆಯ್ಕೆ(partials)
			{
			ಸ್ಥಿತಿ RTLIL::SELECT_ಸಂಪೂರ್ಣ_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.ಎರಡನೇ);
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ RTLIL::SELECT_ಸಂಪೂರ್ಣ_ERR:
				log_error("Unsupported partially selected module %s.\n", it.ಎರಡನೇ);
				ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ RTLIL::SELECT_ಸಂಪೂರ್ಣ_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.ಎರಡನೇ);
				ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ:
				ನಿಲ್ಲಿಸು;
			}
		}
	}
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Module::Module()
{
	ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hashidx_ಎಣಿಕೆ = 123456789;
	hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
	hashidx_ = hashidx_ಎಣಿಕೆ;

	design = ಶೂನ್ಯಸೂಚಕ;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->ಸೇರಿಸು(std::pair<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Module*>(hashidx_, ಇದು));
#ಆದರೆ_ಅಂತ್ಯ
}

RTLIL::Module::~Module()
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &pr : wires_)
		ಅಳಿಸು pr.ಎರಡನೇ;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &pr : memories)
		ಅಳಿಸು pr.ಎರಡನೇ;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &pr : cells_)
		ಅಳಿಸು pr.ಎರಡನೇ;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &pr : processes)
		ಅಳಿಸು pr.ಎರಡನೇ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
ಸ್ಥಿರ std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Module*> all_modules;
std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Module*> *RTLIL::Module::get_all_modules(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ &all_modules;
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = wires_.begin(); it != wires_.ಕೊನೆ(); ++it)
		ಆದರೆ (!it->ಎರಡನೇ->ಪೋರ್ಟ್_ಒಳಹರಿವು && !it->ಎರಡನೇ->ಪೋರ್ಟ್_output)
			delwires.ಸೇರಿಸು(it->ಎರಡನೇ);

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = memories.begin(); it != memories.ಕೊನೆ(); ++it)
		ಅಳಿಸು it->ಎರಡನೇ;
	memories.clear();

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = cells_.begin(); it != cells_.ಕೊನೆ(); ++it)
		ಅಳಿಸು it->ಎರಡನೇ;
	cells_.clear();

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = processes.begin(); it != processes.ಕೊನೆ(); ++it)
		ಅಳಿಸು it->ಎರಡನೇ;
	processes.clear();

	connections_.clear();

	remove(delwires);
	ಗಣ_bool_attribute(ID::blackbox);
}

ಶೂನ್ಯ RTLIL::Module::expand_interfaces(RTLIL::Design *, ಸ್ಥಿರಾಂಕ dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

ತಾರ್ಕಿಕ RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ಸ್ಥಿರಾಂಕ dict<RTLIL::IdString, RTLIL::Const> &, ತಾರ್ಕಿಕ mayfail)
{
	ಆದರೆ (mayfail)
		ಹಿಂತಿರುಗಿ IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ಸ್ಥಿರಾಂಕ dict<RTLIL::IdString, RTLIL::Const> &, ಸ್ಥಿರಾಂಕ dict<RTLIL::IdString, RTLIL::Module*> &, ಸ್ಥಿರಾಂಕ dict<RTLIL::IdString, RTLIL::IdString> &, ತಾರ್ಕಿಕ mayfail)
{
	ಆದರೆ (mayfail)
		ಹಿಂತಿರುಗಿ IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

ಗಾತ್ರ_t RTLIL::Module::ಎಣಿಕೆ_id(RTLIL::IdString id)
{
	ಹಿಂತಿರುಗಿ wires_.ಎಣಿಕೆ_2(id) + cells_.ಎಣಿಕೆ_2(id) + memories.ಎಣಿಕೆ_2(id) + processes.ಎಣಿಕೆ_2(id);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NDEBUG
ನಾಮಾಕಾಶ {
	ರಚನೆ ಆಂತರಿಕ_Cell_Checker
	{
		ಸ್ಥಿರಾಂಕ RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		ಆಂತರಿಕ_Cell_Checker(ಸ್ಥಿರಾಂಕ RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		ಶೂನ್ಯ error(ಪೂರ್ಣಾಂಕ linenr, std::optional<std::string> error_ಸಂದೇಶ = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			ಆದರೆ (error_ಸಂದೇಶ) {
				log_error("%s in internal cell %s\n", *error_ಸಂದೇಶ, cell_loc);
			} ಇಲ್ಲದಿದ್ದರೆ {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		ಪೂರ್ಣಾಂಕ param(RTLIL::IdString name)
		{
			ಸ್ವಯಂಚಾಲಿತ it = cell->parameters.ಕಂಡುಹಿಡಿ(name);
			ಆದರೆ (it == cell->parameters.ಕೊನೆ()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.ಸೇರಿಸು(name);
			ಹಿಂತಿರುಗಿ it->ಎರಡನೇ.as_int();
		}

		ಪೂರ್ಣಾಂಕ param_bool(RTLIL::IdString name)
		{
			ಪೂರ್ಣಾಂಕ v = param(name);
			ಆದರೆ (Get_ಗಾತ್ರ(cell->parameters.at(name)) > 32)
				error(__LINE__);
			ಆದರೆ (v != 0 && v != 1)
				error(__LINE__);
			ಹಿಂತಿರುಗಿ v;
		}

		ಪೂರ್ಣಾಂಕ param_bool(RTLIL::IdString name, ತಾರ್ಕಿಕ expected)
		{
			ಪೂರ್ಣಾಂಕ v = param_bool(name);
			ಆದರೆ (v != expected)
				error(__LINE__);
			ಹಿಂತಿರುಗಿ v;
		}

		ಶೂನ್ಯ param_ಬಿಟ್ಟುಗಳು(RTLIL::IdString name, ಪೂರ್ಣಾಂಕ ಅಗಲ)
		{
			param(name);
			ಆದರೆ (Get_ಗಾತ್ರ(cell->parameters.at(name)) != ಅಗಲ)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			ಹಿಂತಿರುಗಿ cell->parameters.at(name).decode_string();
		}

		ಶೂನ್ಯ ಪೋರ್ಟ್(RTLIL::IdString name, ಪೂರ್ಣಾಂಕ ಅಗಲ)
		{
			ಸ್ವಯಂಚಾಲಿತ it = cell->connections_.ಕಂಡುಹಿಡಿ(name);
			ಆದರೆ (it == cell->connections_.ಕೊನೆ()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			ಆದರೆ (Get_ಗಾತ್ರ(it->ಎರಡನೇ) != ಅಗಲ) {
				std::string err = stringf("Expected port to have width %d", ಅಗಲ);
				error(__LINE__, err);
			}
			expected_ports.ಸೇರಿಸು(name);
		}

		ಶೂನ್ಯ check_expected(ತಾರ್ಕಿಕ check_matched_sign = ಸುಳ್ಳು)
		{
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &para : cell->parameters)
				ಆದರೆ (expected_params.ಎಣಿಕೆ_2(para.ಮೊದಲ) == 0)
					error(__LINE__);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &conn : cell->connections())
				ಆದರೆ (expected_ports.ಎಣಿಕೆ_2(conn.ಮೊದಲ) == 0)
					error(__LINE__);

			ಆದರೆ (check_matched_sign) {
				log_assert(expected_params.ಎಣಿಕೆ_2(ID::A_SIGNED) != 0 && expected_params.ಎಣಿಕೆ_2(ID::B_SIGNED) != 0);
				ತಾರ್ಕಿಕ a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				ತಾರ್ಕಿಕ b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				ಆದರೆ (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		ಶೂನ್ಯ check()
		{
			ಆದರೆ (cell->type.is_ಬಹಿರಂಗ() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				ಹಿಂತಿರುಗಿ;

			ಆದರೆ (cell->type.ಒಳ(ID($buf), ID($barrier))) {
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type.ಒಳ(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				ಪೋರ್ಟ್(ID::A, param(ID::A_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::Y_ಅಗಲ));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type.ಒಳ(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				ಪೋರ್ಟ್(ID::A, param(ID::A_ಅಗಲ));
				ಪೋರ್ಟ್(ID::B, param(ID::B_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::Y_ಅಗಲ));
				check_expected(ನಿಜ);
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type.ಒಳ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				ಪೋರ್ಟ್(ID::A, param(ID::A_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::Y_ಅಗಲ));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type.ಒಳ(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/ಸುಳ್ಳು);
				ಪೋರ್ಟ್(ID::A, param(ID::A_ಅಗಲ));
				ಪೋರ್ಟ್(ID::B, param(ID::B_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::Y_ಅಗಲ));
				check_expected(/*check_matched_sign=*/ಸುಳ್ಳು);
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type.ಒಳ(ID($ಸರಿಸು), ID($shiftx))) {
				ಆದರೆ (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/ಸುಳ್ಳು);
				} ಇಲ್ಲದಿದ್ದರೆ {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				ಪೋರ್ಟ್(ID::A, param(ID::A_ಅಗಲ));
				ಪೋರ್ಟ್(ID::B, param(ID::B_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::Y_ಅಗಲ));
				check_expected(/*check_matched_sign=*/ಸುಳ್ಳು);
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type.ಒಳ(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				ಪೋರ್ಟ್(ID::A, param(ID::A_ಅಗಲ));
				ಪೋರ್ಟ್(ID::B, param(ID::B_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::Y_ಅಗಲ));
				check_expected(ನಿಜ);
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type.ಒಳ(ID($ಕೂಡಿಸು), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ಘಾತ))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				ಪೋರ್ಟ್(ID::A, param(ID::A_ಅಗಲ));
				ಪೋರ್ಟ್(ID::B, param(ID::B_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::Y_ಅಗಲ));
				check_expected(cell->type != ID($ಘಾತ));
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($fa)) {
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::B, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::C, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::X, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($lcu)) {
				ಪೋರ್ಟ್(ID::P, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::G, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::CI, 1);
				ಪೋರ್ಟ್(ID::CO, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				ಪೋರ್ಟ್(ID::A, param(ID::A_ಅಗಲ));
				ಪೋರ್ಟ್(ID::B, param(ID::B_ಅಗಲ));
				ಪೋರ್ಟ್(ID::CI, 1);
				ಪೋರ್ಟ್(ID::BI, 1);
				ಪೋರ್ಟ್(ID::X, param(ID::Y_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::Y_ಅಗಲ));
				ಪೋರ್ಟ್(ID::CO, param(ID::Y_ಅಗಲ));
				check_expected(ನಿಜ);
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_ಅಗಲ);
				ಪೋರ್ಟ್(ID::A, param(ID::A_ಅಗಲ));
				ಪೋರ್ಟ್(ID::B, param(ID::B_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::Y_ಅಗಲ));
				check_expected();
				Macc().from_cell(cell);
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($macc_v2)) {
				ಆದರೆ (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				ಆದರೆ (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_ಬಿಟ್ಟುಗಳು(ID::ಗುಣಲಬ್ಧ_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_ಬಿಟ್ಟುಗಳು(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_ಬಿಟ್ಟುಗಳು(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_ಬಿಟ್ಟುಗಳು(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_ಬಿಟ್ಟುಗಳು(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				ಆದರೆ (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_ಬಿಟ್ಟುಗಳು(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_ಬಿಟ್ಟುಗಳು(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_ಬಿಟ್ಟುಗಳು(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ಸ್ಥಿರಾಂಕ Const &a_ಅಗಲ = cell->getParam(ID::A_WIDTHS);
				ಸ್ಥಿರಾಂಕ Const &b_ಅಗಲ = cell->getParam(ID::B_WIDTHS);
				ಸ್ಥಿರಾಂಕ Const &c_ಅಗಲ = cell->getParam(ID::C_WIDTHS);
				ಪೂರ್ಣಾಂಕ a_ಅಗಲ_ಮೊತ್ತ = 0, b_ಅಗಲ_ಮೊತ್ತ = 0, c_ಅಗಲ_ಮೊತ್ತ = 0;
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < param(ID::NPRODUCTS); i++) {
					a_ಅಗಲ_ಮೊತ್ತ += a_ಅಗಲ.ಹೊರತೆಗೆ(16 * i, 16).as_int(ಸುಳ್ಳು);
					b_ಅಗಲ_ಮೊತ್ತ += b_ಅಗಲ.ಹೊರತೆಗೆ(16 * i, 16).as_int(ಸುಳ್ಳು);
				}
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < param(ID::NADDENDS); i++) {
					c_ಅಗಲ_ಮೊತ್ತ += c_ಅಗಲ.ಹೊರತೆಗೆ(16 * i, 16).as_int(ಸುಳ್ಳು);
				}
				ಪೋರ್ಟ್(ID::A, a_ಅಗಲ_ಮೊತ್ತ);
				ಪೋರ್ಟ್(ID::B, b_ಅಗಲ_ಮೊತ್ತ);
				ಪೋರ್ಟ್(ID::C, c_ಅಗಲ_ಮೊತ್ತ);
				ಪೋರ್ಟ್(ID::Y, param(ID::Y_ಅಗಲ));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				ಪೋರ್ಟ್(ID::A, param(ID::A_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::Y_ಅಗಲ));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type.ಒಳ(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				ಪೋರ್ಟ್(ID::A, param(ID::A_ಅಗಲ));
				ಪೋರ್ಟ್(ID::B, param(ID::B_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::Y_ಅಗಲ));
				check_expected(/*check_matched_sign=*/ಸುಳ್ಳು);
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($ತುಂಡು)) {
				param(ID::ಸ್ಥಾನಾಂತರ_2);
				ಪೋರ್ಟ್(ID::A, param(ID::A_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::Y_ಅಗಲ));
				ಆದರೆ (param(ID::ಸ್ಥಾನಾಂತರ_2) + param(ID::Y_ಅಗಲ) > param(ID::A_ಅಗಲ))
					error(__LINE__);
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($concat)) {
				ಪೋರ್ಟ್(ID::A, param(ID::A_ಅಗಲ));
				ಪೋರ್ಟ್(ID::B, param(ID::B_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::A_ಅಗಲ) + param(ID::B_ಅಗಲ));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($mux)) {
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::B, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::S, 1);
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($pmux)) {
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::B, param(ID::ಅಗಲ_2) * param(ID::S_ಅಗಲ));
				ಪೋರ್ಟ್(ID::S, param(ID::S_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($bmux)) {
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2) << param(ID::S_ಅಗಲ));
				ಪೋರ್ಟ್(ID::S, param(ID::S_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($demux)) {
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::S, param(ID::S_ಅಗಲ));
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2) << param(ID::S_ಅಗಲ));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($lut)) {
				param(ID::LUT);
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Y, 1);
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($sop)) {
				param(ID::ಆಳ_2);
				param(ID::ಕೋಷ್ಟಕ);
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Y, 1);
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($sr)) {
				param_bool(ID::ಗಣ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				ಪೋರ್ಟ್(ID::ಗಣ_2, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::CLR, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q,   param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($ff)) {
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::EN, 1);
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ಗಣ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::ಗಣ_2, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::CLR, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($dffsre)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ಗಣ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				param_bool(ID::EN_POLARITY);
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::EN, 1);
				ಪೋರ್ಟ್(ID::ಗಣ_2, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::CLR, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_ಬಿಟ್ಟುಗಳು(ID::ARST_ಮೌಲ್ಯ, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::ARST, 1);
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_ಬಿಟ್ಟುಗಳು(ID::SRST_ಮೌಲ್ಯ, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::SRST, 1);
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type.ಒಳ(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_ಬಿಟ್ಟುಗಳು(ID::SRST_ಮೌಲ್ಯ, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::EN, 1);
				ಪೋರ್ಟ್(ID::SRST, 1);
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_ಬಿಟ್ಟುಗಳು(ID::ARST_ಮೌಲ್ಯ, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::EN, 1);
				ಪೋರ್ಟ್(ID::ARST, 1);
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::ALOAD, 1);
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::AD, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($aldffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::EN, 1);
				ಪೋರ್ಟ್(ID::ALOAD, 1);
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::AD, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				ಪೋರ್ಟ್(ID::EN, 1);
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_ಬಿಟ್ಟುಗಳು(ID::ARST_ಮೌಲ್ಯ, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::EN, 1);
				ಪೋರ್ಟ್(ID::ARST, 1);
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ಗಣ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				ಪೋರ್ಟ್(ID::EN, 1);
				ಪೋರ್ಟ್(ID::ಗಣ_2, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::CLR, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_ಬಿಟ್ಟುಗಳು);
				param(ID::STATE_ಅಂಕೆ);
				param(ID::STATE_ಅಂಕೆ_LOG2);
				param(ID::STATE_RST);
				param_ಬಿಟ್ಟುಗಳು(ID::STATE_ಕೋಷ್ಟಕ, param(ID::STATE_ಬಿಟ್ಟುಗಳು) * param(ID::STATE_ಅಂಕೆ));
				param(ID::TRANS_ಅಂಕೆ);
				param_ಬಿಟ್ಟುಗಳು(ID::TRANS_ಕೋಷ್ಟಕ, param(ID::TRANS_ಅಂಕೆ) * (2*param(ID::STATE_ಅಂಕೆ_LOG2) + param(ID::CTRL_ಒಳ_ಅಗಲ) + param(ID::CTRL_ಹೊರಹೋಗು_ಅಗಲ)));
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::ARST, 1);
				ಪೋರ್ಟ್(ID::CTRL_ಒಳ, param(ID::CTRL_ಒಳ_ಅಗಲ));
				ಪೋರ್ಟ್(ID::CTRL_ಹೊರಹೋಗು, param(ID::CTRL_ಹೊರಹೋಗು_ಅಗಲ));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::EN, 1);
				ಪೋರ್ಟ್(ID::ADDR, param(ID::ABITS));
				ಪೋರ್ಟ್(ID::ದತ್ತಾಂಶ_2, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_ಮುಖವಾಡ);
				param(ID::COLLISION_X_ಮುಖವಾಡ);
				param_bool(ID::CE_OVER_SRST);
				param_ಬಿಟ್ಟುಗಳು(ID::ARST_ಮೌಲ್ಯ, param(ID::ಅಗಲ_2));
				param_ಬಿಟ್ಟುಗಳು(ID::SRST_ಮೌಲ್ಯ, param(ID::ಅಗಲ_2));
				param_ಬಿಟ್ಟುಗಳು(ID::INIT_ಮೌಲ್ಯ, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::EN, 1);
				ಪೋರ್ಟ್(ID::ARST, 1);
				ಪೋರ್ಟ್(ID::SRST, 1);
				ಪೋರ್ಟ್(ID::ADDR, param(ID::ABITS));
				ಪೋರ್ಟ್(ID::ದತ್ತಾಂಶ_2, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::ಆದ್ಯತೆ);
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::EN, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::ADDR, param(ID::ABITS));
				ಪೋರ್ಟ್(ID::ದತ್ತಾಂಶ_2, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($memwr_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PORTID);
				param(ID::ಆದ್ಯತೆ_ಮುಖವಾಡ_2);
				ಪೋರ್ಟ್(ID::CLK, 1);
				ಪೋರ್ಟ್(ID::EN, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::ADDR, param(ID::ABITS));
				ಪೋರ್ಟ್(ID::ದತ್ತಾಂಶ_2, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::ಆದ್ಯತೆ);
				ಪೋರ್ಟ್(ID::ADDR, param(ID::ABITS));
				ಪೋರ್ಟ್(ID::ದತ್ತಾಂಶ_2, param(ID::ಅಗಲ_2) * param(ID::ಪದಗಳು));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::ಆದ್ಯತೆ);
				ಪೋರ್ಟ್(ID::ADDR, param(ID::ABITS));
				ಪೋರ್ಟ್(ID::ದತ್ತಾಂಶ_2, param(ID::ಅಗಲ_2) * param(ID::ಪದಗಳು));
				ಪೋರ್ಟ್(ID::EN, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($mem)) {
				param(ID::MEMID);
				param(ID::ಗಾತ್ರ_3);
				param(ID::ಸ್ಥಾನಾಂತರ_2);
				param(ID::INIT);
				param_ಬಿಟ್ಟುಗಳು(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_ಬಿಟ್ಟುಗಳು(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_ಬಿಟ್ಟುಗಳು(ID::RD_TRANSPARENT, max(1, param(ID::RD_PORTS)));
				param_ಬಿಟ್ಟುಗಳು(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_ಬಿಟ್ಟುಗಳು(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				ಪೋರ್ಟ್(ID::RD_CLK, param(ID::RD_PORTS));
				ಪೋರ್ಟ್(ID::RD_EN, param(ID::RD_PORTS));
				ಪೋರ್ಟ್(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				ಪೋರ್ಟ್(ID::RD_ದತ್ತಾಂಶ, param(ID::RD_PORTS) * param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::WR_CLK, param(ID::WR_PORTS));
				ಪೋರ್ಟ್(ID::WR_EN, param(ID::WR_PORTS) * param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				ಪೋರ್ಟ್(ID::WR_ದತ್ತಾಂಶ, param(ID::WR_PORTS) * param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($mem_v2)) {
				param(ID::MEMID);
				param(ID::ಗಾತ್ರ_3);
				param(ID::ಸ್ಥಾನಾಂತರ_2);
				param(ID::INIT);
				param_ಬಿಟ್ಟುಗಳು(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_ಬಿಟ್ಟುಗಳು(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_ಬಿಟ್ಟುಗಳು(ID::RD_TRANSPARENCY_ಮುಖವಾಡ, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_ಬಿಟ್ಟುಗಳು(ID::RD_COLLISION_X_ಮುಖವಾಡ, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_ಬಿಟ್ಟುಗಳು(ID::RD_WIDE_CONTINUATION, max(1, param(ID::RD_PORTS)));
				param_ಬಿಟ್ಟುಗಳು(ID::RD_CE_OVER_SRST, max(1, param(ID::RD_PORTS)));
				param_ಬಿಟ್ಟುಗಳು(ID::RD_ARST_ಮೌಲ್ಯ, max(1, param(ID::RD_PORTS) * param(ID::ಅಗಲ_2)));
				param_ಬಿಟ್ಟುಗಳು(ID::RD_SRST_ಮೌಲ್ಯ, max(1, param(ID::RD_PORTS) * param(ID::ಅಗಲ_2)));
				param_ಬಿಟ್ಟುಗಳು(ID::RD_INIT_ಮೌಲ್ಯ, max(1, param(ID::RD_PORTS) * param(ID::ಅಗಲ_2)));
				param_ಬಿಟ್ಟುಗಳು(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_ಬಿಟ್ಟುಗಳು(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				param_ಬಿಟ್ಟುಗಳು(ID::WR_WIDE_CONTINUATION, max(1, param(ID::WR_PORTS)));
				param_ಬಿಟ್ಟುಗಳು(ID::WR_ಆದ್ಯತೆ_ಮುಖವಾಡ, max(1, param(ID::WR_PORTS) * param(ID::WR_PORTS)));
				ಪೋರ್ಟ್(ID::RD_CLK, param(ID::RD_PORTS));
				ಪೋರ್ಟ್(ID::RD_EN, param(ID::RD_PORTS));
				ಪೋರ್ಟ್(ID::RD_ARST, param(ID::RD_PORTS));
				ಪೋರ್ಟ್(ID::RD_SRST, param(ID::RD_PORTS));
				ಪೋರ್ಟ್(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				ಪೋರ್ಟ್(ID::RD_ದತ್ತಾಂಶ, param(ID::RD_PORTS) * param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::WR_CLK, param(ID::WR_PORTS));
				ಪೋರ್ಟ್(ID::WR_EN, param(ID::WR_PORTS) * param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				ಪೋರ್ಟ್(ID::WR_ದತ್ತಾಂಶ, param(ID::WR_PORTS) * param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($tribuf)) {
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::EN, 1);
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($bweqx)) {
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::B, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($bwmux)) {
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::B, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::S, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type.ಒಳ(ID($ಪ್ರತಿಜ್ಞಾ), ID($assume), ID($ಜೀವಂತ), ID($fair), ID($cover))) {
				ಪೋರ್ಟ್(ID::A, 1);
				ಪೋರ್ಟ್(ID::EN, 1);
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($initstate)) {
				ಪೋರ್ಟ್(ID::Y, 1);
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type.ಒಳ(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type.ಒಳ(ID($anyinit))) {
				ಪೋರ್ಟ್(ID::D, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Q, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($equiv)) {
				ಪೋರ್ಟ್(ID::A, 1);
				ಪೋರ್ಟ್(ID::B, 1);
				ಪೋರ್ಟ್(ID::Y, 1);
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type.ಒಳ(ID($specify2), ID($specify3))) {
				param_bool(ID::ತುಂಬಿದ);
				param_bool(ID::SRC_DST_PEN);
				param_bool(ID::SRC_DST_POL);
				param(ID::T_RISE_MIN);
				param(ID::T_RISE_TYP);
				param(ID::T_RISE_MAX);
				param(ID::T_FALL_MIN);
				param(ID::T_FALL_TYP);
				param(ID::T_FALL_MAX);
				ಪೋರ್ಟ್(ID::EN, 1);
				ಪೋರ್ಟ್(ID::SRC, param(ID::SRC_ಅಗಲ));
				ಪೋರ್ಟ್(ID::DST, param(ID::DST_ಅಗಲ));
				ಆದರೆ (cell->type == ID($specify3)) {
					param_bool(ID::ಅಂಚು_EN);
					param_bool(ID::ಅಂಚು_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					ಪೋರ್ಟ್(ID::DAT, param(ID::DST_ಅಗಲ));
				}
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($specrule)) {
				param(ID::TYPE);
				param_bool(ID::SRC_PEN);
				param_bool(ID::SRC_POL);
				param_bool(ID::DST_PEN);
				param_bool(ID::DST_POL);
				param(ID::T_ಎಲ್ಲೆ_MIN);
				param(ID::T_ಎಲ್ಲೆ_TYP);
				param(ID::T_ಎಲ್ಲೆ_MAX);
				param(ID::T_LIMIT2_MIN);
				param(ID::T_LIMIT2_TYP);
				param(ID::T_LIMIT2_MAX);
				ಪೋರ್ಟ್(ID::SRC_EN, 1);
				ಪೋರ್ಟ್(ID::DST_EN, 1);
				ಪೋರ್ಟ್(ID::SRC, param(ID::SRC_ಅಗಲ));
				ಪೋರ್ಟ್(ID::DST, param(ID::DST_ಅಗಲ));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::ಆದ್ಯತೆ);
				ಪೋರ್ಟ್(ID::EN, 1);
				ಪೋರ್ಟ್(ID::TRG, param(ID::TRG_ಅಗಲ));
				ಪೋರ್ಟ್(ID::ವಾದಗಳು, param(ID::ವಾದಗಳು_ಅಗಲ));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				ಆದರೆ (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
					error(__LINE__);
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::ಆದ್ಯತೆ);
				ಪೋರ್ಟ್(ID::A, 1);
				ಪೋರ್ಟ್(ID::EN, 1);
				ಪೋರ್ಟ್(ID::TRG, param(ID::TRG_ಅಗಲ));
				ಪೋರ್ಟ್(ID::ವಾದಗಳು, param(ID::ವಾದಗಳು_ಅಗಲ));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				ಆದರೆ (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				ಹಿಂತಿರುಗಿ;
			}

			ಆದರೆ (cell->type == ID($_BUF_))    { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_NOT_))    { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_AND_))    { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_NAND_))   { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_OR_))     { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_NOR_))    { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_XOR_))    { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_XNOR_))   { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_ANDNOT_)) { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_ORNOT_))  { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_MUX_))    { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::S,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_NMUX_))   { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::S,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_AOI3_))   { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_OAI3_))   { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_AOI4_))   { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_OAI4_))   { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type == ID($_TBUF_))  { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::Y,1); ಪೋರ್ಟ್(ID::E,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type == ID($_MUX4_))  { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::S,1); ಪೋರ್ಟ್(ID::T,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_MUX8_))  { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::E,1); ಪೋರ್ಟ್(ID::F,1); ಪೋರ್ಟ್(ID::G,1); ಪೋರ್ಟ್(ID::H,1); ಪೋರ್ಟ್(ID::S,1); ಪೋರ್ಟ್(ID::T,1); ಪೋರ್ಟ್(ID::U,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }
			ಆದರೆ (cell->type == ID($_MUX16_)) { ಪೋರ್ಟ್(ID::A,1); ಪೋರ್ಟ್(ID::B,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::E,1); ಪೋರ್ಟ್(ID::F,1); ಪೋರ್ಟ್(ID::G,1); ಪೋರ್ಟ್(ID::H,1); ಪೋರ್ಟ್(ID::I,1); ಪೋರ್ಟ್(ID::J,1); ಪೋರ್ಟ್(ID::K,1); ಪೋರ್ಟ್(ID::L,1); ಪೋರ್ಟ್(ID::M,1); ಪೋರ್ಟ್(ID::N,1); ಪೋರ್ಟ್(ID::O,1); ಪೋರ್ಟ್(ID::P,1); ಪೋರ್ಟ್(ID::S,1); ಪೋರ್ಟ್(ID::T,1); ಪೋರ್ಟ್(ID::U,1); ಪೋರ್ಟ್(ID::V,1); ಪೋರ್ಟ್(ID::Y,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ ಪೋರ್ಟ್(ID::S,1); ಪೋರ್ಟ್(ID::R,1); ಪೋರ್ಟ್(ID::Q,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type == ID($_FF_)) { ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Q,1); check_expected();  ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(ID($_DFF_N_), ID($_DFF_P_)))
				{ ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Q,1); ಪೋರ್ಟ್(ID::C,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Q,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::E,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Q,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::R,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Q,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::R,1); ಪೋರ್ಟ್(ID::E,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Q,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::L,1); ಪೋರ್ಟ್(ID::AD,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Q,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::L,1); ಪೋರ್ಟ್(ID::AD,1); ಪೋರ್ಟ್(ID::E,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::S,1); ಪೋರ್ಟ್(ID::R,1); ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Q,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::S,1); ಪೋರ್ಟ್(ID::R,1); ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::E,1); ಪೋರ್ಟ್(ID::Q,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Q,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::R,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Q,1); ಪೋರ್ಟ್(ID::C,1); ಪೋರ್ಟ್(ID::R,1); ಪೋರ್ಟ್(ID::E,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ ಪೋರ್ಟ್(ID::E,1); ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Q,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ ಪೋರ್ಟ್(ID::E,1); ಪೋರ್ಟ್(ID::R,1); ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Q,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ ಪೋರ್ಟ್(ID::E,1); ಪೋರ್ಟ್(ID::S,1); ಪೋರ್ಟ್(ID::R,1); ಪೋರ್ಟ್(ID::D,1); ಪೋರ್ಟ್(ID::Q,1); check_expected(); ಹಿಂತಿರುಗಿ; }

			ಆದರೆ (cell->type.ಒಳ(ID($ಗಣ_ಟ್ಯಾಗ್))) {
				param(ID::ಅಗಲ_2);
				param(ID::ಟ್ಯಾಗ್_2);
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::ಗಣ_2, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::CLR, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}
			ಆದರೆ (cell->type.ಒಳ(ID($get_ಟ್ಯಾಗ್),ID($original_ಟ್ಯಾಗ್))) {
				param(ID::ಅಗಲ_2);
				param(ID::ಟ್ಯಾಗ್_2);
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}
			ಆದರೆ (cell->type.ಒಳ(ID($overwrite_ಟ್ಯಾಗ್))) {
				param(ID::ಅಗಲ_2);
				param(ID::ಟ್ಯಾಗ್_2);
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::ಗಣ_2, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::CLR, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}
			ಆದರೆ (cell->type.ಒಳ(ID($future_ff))) {
				param(ID::ಅಗಲ_2);
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}
			ಆದರೆ (cell->type.ಒಳ(ID($ಒಳಹರಿವು_ಪೋರ್ಟ್))) {
				param(ID::ಅಗಲ_2);
				ಪೋರ್ಟ್(ID::Y, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}
			ಆದರೆ (cell->type.ಒಳ(ID($connect))) {
				param(ID::ಅಗಲ_2);
				ಪೋರ್ಟ್(ID::A, param(ID::ಅಗಲ_2));
				ಪೋರ್ಟ್(ID::B, param(ID::ಅಗಲ_2));
				check_expected();
				ಹಿಂತಿರುಗಿ;
			}
			/*
			 * Checklist for adding internal cell types
			 * ========================================
			 * Things to do right away:
			 *    - Add to kernel/celltypes.h (incl. eval() handling for non-mem cells)
			 *    - Add to InternalCellChecker::check() in kernel/rtlil.cc
			 *    - Add to techlibs/common/simlib.v
			 *    - Add to techlibs/common/techmap.v
			 *
			 * Things to do after finalizing the cell interface:
			 *    - Add support to kernel/satgen.h for the new cell type
			 *    - Add to docs/source/CHAPTER_CellLib.rst (or just add a fixme to the bottom)
			 *    - Maybe add support to the Verilog backend for dumping such cells as expression
			 *
			 */
			error(__LINE__);
		}
	};
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ RTLIL::Module::ವಿಂಗಡಿಸು()
{
	wires_.ವಿಂಗಡಿಸು(ವಿಂಗಡಿಸು_by_id_str(design->twines()));
	cells_.ವಿಂಗಡಿಸು(ವಿಂಗಡಿಸು_by_id_str(design->twines()));
	parameter_default_ಮೌಲ್ಯಗಳು.ವಿಂಗಡಿಸು(ವಿಂಗಡಿಸು_by_id_str(design->twines()));
	memories.ವಿಂಗಡಿಸು(ವಿಂಗಡಿಸು_by_id_str(design->twines()));
	processes.ವಿಂಗಡಿಸು(ವಿಂಗಡಿಸು_by_id_str(design->twines()));
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : cells_)
		it.ಎರಡನೇ->ವಿಂಗಡಿಸು();
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : wires_)
		it.ಎರಡನೇ->ಗುಣಲಕ್ಷಣಗಳು.ವಿಂಗಡಿಸು(ವಿಂಗಡಿಸು_by_id_str(design->twines()));
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : memories)
		it.ಎರಡನೇ->ಗುಣಲಕ್ಷಣಗಳು.ವಿಂಗಡಿಸು(ವಿಂಗಡಿಸು_by_id_str(design->twines()));
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NDEBUG
ಸ್ಥಿರ ಶೂನ್ಯ check_id_ಕೀಲಿಗಳು(ಸ್ಥಿರಾಂಕ dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : keyed)
		log_assert(!it.ಮೊದಲ.ಖಾಲಿಗಳು());
}
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_ಗಾತ್ರ(0, module->cells_ಗಾತ್ರ(), 1000));
	ಸ್ಥಿರಾಂಕ RTLIL::Module *const_module = module;

	pool<std::string> ಸ್ಮರಣೆ_ಅಕ್ಷರಸರಣಿಗಳು;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : module->memories) {
		log_assert(it.ಮೊದಲ == it.ಎರಡನೇ->name);
		log_assert(it.ಮೊದಲ != IdString::Null);
		log_assert(it.ಎರಡನೇ->ಅಗಲ >= 0);
		log_assert(it.ಎರಡನೇ->ಗಾತ್ರ_2 >= 0);
		check_id_ಕೀಲಿಗಳು(it.ಎರಡನೇ->ಗುಣಲಕ್ಷಣಗಳು);
		ಸ್ಮರಣೆ_ಅಕ್ಷರಸರಣಿಗಳು.ಸೇರಿಸು(it.ಎರಡನೇ->name.str());
	}

	std::ಸದಿಶ<MonotonicFlag> ports_declared(Get_ಗಾತ್ರ(module->ports));
	Sharded_ಸದಿಶ<std::string> memids(subpool);
	subpool.ಸತತ([const_module, &ports_declared, &ಸ್ಮರಣೆ_ಅಕ್ಷರಸರಣಿಗಳು, &memids](ಸ್ಥಿರಾಂಕ ParallelDispatchThreadPool::ಸತತ_Ctx &ctx) {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i : ctx.ಅಂಶ_range(const_module->cells_ಗಾತ್ರ())) {
			ಸ್ವಯಂಚಾಲಿತ it = *const_module->cells_.element(i);
			log_assert(const_module == it.ಎರಡನೇ->module);
			log_assert(it.ಮೊದಲ == it.ಎರಡನೇ->name);
			log_assert(it.ಮೊದಲ != IdString::Null);
			log_assert(!it.ಎರಡನೇ->type.ಖಾಲಿಗಳು());
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it2 : it.ಎರಡನೇ->connections()) {
				log_assert(it2.ಮೊದಲ != IdString::Null);
				it2.ಎರಡನೇ.check(const_module);
			}
			check_id_ಕೀಲಿಗಳು(it.ಎರಡನೇ->ಗುಣಲಕ್ಷಣಗಳು);
			check_id_ಕೀಲಿಗಳು(it.ಎರಡನೇ->parameters);
			ಆಂತರಿಕ_Cell_Checker checker(const_module, it.ಎರಡನೇ);
			checker.check();
			ಆದರೆ (it.ಎರಡನೇ->has_memid()) {
				log_assert(ಸ್ಮರಣೆ_ಅಕ್ಷರಸರಣಿಗಳು.ಎಣಿಕೆ_2(it.ಎರಡನೇ->parameters.at(ID::MEMID).decode_string()));
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (it.ಎರಡನೇ->is_mem_cell()) {
				std::string memid = it.ಎರಡನೇ->parameters.at(ID::MEMID).decode_string();
				log_assert(!ಸ್ಮರಣೆ_ಅಕ್ಷರಸರಣಿಗಳು.ಎಣಿಕೆ_2(memid));
				memids.ಸೇರಿಸು(ctx, std::move(memid));
			}
			ಸ್ವಯಂಚಾಲಿತ cell_mod = const_module->design->module(it.ಎರಡನೇ->name);
			ಆದರೆ (cell_mod != ಶೂನ್ಯಸೂಚಕ) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GNUC__
				#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
				#ಸೂಚನೆ GCC diagnostic ignored "-Wdeprecated-declarations"
				#ಆದರೆ_ಅಂತ್ಯ
				log_assert(!it.ಎರಡನೇ->get_blackbox_attribute());
				#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GNUC__
				#ಸೂಚನೆ GCC diagnostic ತೆಗೆ_2
				#ಆದರೆ_ಅಂತ್ಯ
			}
		}

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i : ctx.ಅಂಶ_range(const_module->wires_ಗಾತ್ರ())) {
			ಸ್ವಯಂಚಾಲಿತ it = *const_module->wires_.element(i);
			log_assert(const_module == it.ಎರಡನೇ->module);
			log_assert(it.ಮೊದಲ == it.ಎರಡನೇ->name);
			log_assert(it.ಮೊದಲ != IdString::Null);
			log_assert(it.ಎರಡನೇ->ಅಗಲ >= 0);
			log_assert(it.ಎರಡನೇ->ಪೋರ್ಟ್_id >= 0);
			check_id_ಕೀಲಿಗಳು(it.ಎರಡನೇ->ಗುಣಲಕ್ಷಣಗಳು);
			ಆದರೆ (it.ಎರಡನೇ->ಪೋರ್ಟ್_id) {
				log_assert(Get_ಗಾತ್ರ(const_module->ports) >= it.ಎರಡನೇ->ಪೋರ್ಟ್_id);
				log_assert(const_module->ports.at(it.ಎರಡನೇ->ಪೋರ್ಟ್_id-1) == it.ಎರಡನೇ->name);
				log_assert(it.ಎರಡನೇ->ಪೋರ್ಟ್_ಒಳಹರಿವು || it.ಎರಡನೇ->ಪೋರ್ಟ್_output);
				log_assert(it.ಎರಡನೇ->ಪೋರ್ಟ್_id <= Get_ಗಾತ್ರ(ports_declared));
				ತಾರ್ಕಿಕ previously_declared = ports_declared[it.ಎರಡನೇ->ಪೋರ್ಟ್_id-1].ಗಣ_and_return_old();
				log_assert(previously_declared == ಸುಳ್ಳು);
			} ಇಲ್ಲದಿದ್ದರೆ
				log_assert(!it.ಎರಡನೇ->ಪೋರ್ಟ್_ಒಳಹರಿವು && !it.ಎರಡನೇ->ಪೋರ್ಟ್_output);
		}
	});
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ MonotonicFlag &ಪೋರ್ಟ್_declared : ports_declared)
		log_assert(ಪೋರ್ಟ್_declared.ತುಂಬು() == ನಿಜ);
	pool<std::string> memids_pool;
	ಗಾಗಿ (std::string &memid : memids)
		log_assert(memids_pool.ಸೇರಿಸು(memid).ಎರಡನೇ);

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : module->processes) {
		log_assert(it.ಮೊದಲ == it.ಎರಡನೇ->name);
		log_assert(it.ಮೊದಲ != IdString::Null);
		log_assert(it.ಎರಡನೇ->ಬೇರು_case.compare.ಖಾಲಿಗಳು());
		std::ಸದಿಶ<RTLIL::Case_ನಿಯಮ*> all_cases = {&it.ಎರಡನೇ->ಬೇರು_case};
		ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < all_cases.ಗಾತ್ರ_2(); i++) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &switch_it : all_cases[i]->switches) {
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &case_it : switch_it->cases) {
					ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &compare_it : case_it->compare) {
						log_assert(switch_it->ಸಂಕೇತ.ಗಾತ್ರ_2() == compare_it.ಗಾತ್ರ_2());
					}
					all_cases.ತಳ್ಳು_ಹಿಂಭಾಗ(case_it);
				}
			}
		}
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &sync_it : it.ಎರಡನೇ->syncs) {
			ಆಯ್ಕೆ (sync_it->type) {
				ಸ್ಥಿತಿ RTLIL::SyncType::ST0:
				ಸ್ಥಿತಿ RTLIL::SyncType::ST1:
				ಸ್ಥಿತಿ RTLIL::SyncType::STp:
				ಸ್ಥಿತಿ RTLIL::SyncType::STn:
				ಸ್ಥಿತಿ RTLIL::SyncType::STe:
					log_assert(!sync_it->ಸಂಕೇತ.ಖಾಲಿಗಳು());
					ನಿಲ್ಲಿಸು;
				ಸ್ಥಿತಿ RTLIL::SyncType::STa:
				ಸ್ಥಿತಿ RTLIL::SyncType::STg:
				ಸ್ಥಿತಿ RTLIL::SyncType::STi:
					log_assert(sync_it->ಸಂಕೇತ.ಖಾಲಿಗಳು());
					ನಿಲ್ಲಿಸು;
			}
		}
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : module->connections_) {
		log_assert(it.ಮೊದಲ.ಗಾತ್ರ_2() == it.ಎರಡನೇ.ಗಾತ್ರ_2());
		log_assert(!it.ಮೊದಲ.has_const());
		it.ಮೊದಲ.check(module);
		it.ಎರಡನೇ.check(module);
	}

	check_id_ಕೀಲಿಗಳು(module->ಗುಣಲಕ್ಷಣಗಳು);
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ RTLIL::Module::check()
{
	ಪೂರ್ಣಾಂಕ pool_ಗಾತ್ರ = ThreadPool::work_pool_ಗಾತ್ರ(0, cells_ಗಾತ್ರ(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_ಗಾತ್ರ);
	check_module(ಇದು, thread_pool);
}

ಶೂನ್ಯ RTLIL::Module::optimize()
{
}

ಶೂನ್ಯ RTLIL::Module::cloneInto(RTLIL::Module *new_mod) ಸ್ಥಿರಾಂಕ
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	ಗಾಗಿ (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_ಮೌಲ್ಯಗಳು.clear();
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : parameter_default_ಮೌಲ್ಯಗಳು)
		new_mod->parameter_default_ಮೌಲ್ಯಗಳು[dst_twines.copy_from(design->twines(), it.ಮೊದಲ)] = it.ಎರಡನೇ;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->ಗುಣಲಕ್ಷಣಗಳು, ಗುಣಲಕ್ಷಣಗಳು, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->ಮೊದಲ);
		wire_map[it->ಎರಡನೇ] = new_mod->ಕೂಡಿಸು_Wire(dst_id, it->ಎರಡನೇ);
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->ಮೊದಲ);
		new_mod->ಕೂಡಿಸು_ಸ್ಮರಣೆ(dst_id, it->ಎರಡನೇ);
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->ಮೊದಲ);
		new_mod->ಕೂಡಿಸು_Cell(dst_id, it->ಎರಡನೇ);
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->ಮೊದಲ);
		new_mod->ಕೂಡಿಸು_Process(dst_id, it->ಎರಡನೇ);
	}

	ರಚನೆ Rewrite_Sig_Spec_ಕೆಲಸಗಾರ
	{
		ಸ್ಥಿರಾಂಕ dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ಸ್ಥಿರಾಂಕ RTLIL::Module *new_mod;
		ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([ಇದು](RTLIL::Wire *&wire) {
				ಸ್ವಯಂಚಾಲಿತ it = wire_map.ಕಂಡುಹಿಡಿ(wire);
				ಆದರೆ (it != wire_map.ಕೊನೆ())
					wire = it->ಎರಡನೇ;
				ಇಲ್ಲದಿದ್ದರೆ
					log_assert(wire->module == new_mod);
			});
		}
	};

	Rewrite_Sig_Spec_ಕೆಲಸಗಾರ rewrite_Sig_Spec_ಕೆಲಸಗಾರ{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewrite_Sig_Spec_ಕೆಲಸಗಾರ);
	new_mod->fixup_ports();
}

RTLIL::Module *RTLIL::Module::clone() ಸ್ಥಿರಾಂಕ
{
	RTLIL::Module *new_mod = ಹೊಸ RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	ಹಿಂತಿರುಗಿ new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) ಸ್ಥಿರಾಂಕ
{
	RTLIL::Module *new_mod = ಹೊಸ RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->ಕೂಡಿಸು(new_mod);
	ಹಿಂತಿರುಗಿ new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString ಗುರಿ_name) ಸ್ಥಿರಾಂಕ
{
	RTLIL::Module *new_mod = ಹೊಸ RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = ಗುರಿ_name;
	cloneInto(new_mod);
	dst->ಕೂಡಿಸು(new_mod);
	ಹಿಂತಿರುಗಿ new_mod;
}

ತಾರ್ಕಿಕ RTLIL::Module::has_memories() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ !memories.ಖಾಲಿಗಳು();
}

ತಾರ್ಕಿಕ RTLIL::Module::has_processes() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ !processes.ಖಾಲಿಗಳು();
}

ತಾರ್ಕಿಕ RTLIL::Module::has_memories_warn() ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (!memories.ಖಾಲಿಗಳು())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", ಇದು);
	ಹಿಂತಿರುಗಿ !memories.ಖಾಲಿಗಳು();
}

ತಾರ್ಕಿಕ RTLIL::Module::has_processes_warn() ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (!processes.ಖಾಲಿಗಳು())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", ಇದು);
	ಹಿಂತಿರುಗಿ !processes.ಖಾಲಿಗಳು();
}

ತಾರ್ಕಿಕ RTLIL::Module::is_selected() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ design->selected_module(ಇದು->name);
}

ತಾರ್ಕಿಕ RTLIL::Module::is_selected_ಸಂಪೂರ್ಣ() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ design->selected_ಸಂಪೂರ್ಣ_module(ಇದು->name);
}

std::ಸದಿಶ<RTLIL::Wire*> RTLIL::Module::selected_wires() ಸ್ಥಿರಾಂಕ
{
	std::ಸದಿಶ<RTLIL::Wire*> ಫಲಿತಾಂಶ;
	ಫಲಿತಾಂಶ.reserve(wires_.ಗಾತ್ರ_2());
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : wires_)
		ಆದರೆ (design->selected(ಇದು, it.ಎರಡನೇ))
			ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(it.ಎರಡನೇ);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

std::ಸದಿಶ<RTLIL::Cell*> RTLIL::Module::selected_cells() ಸ್ಥಿರಾಂಕ
{
	std::ಸದಿಶ<RTLIL::Cell*> ಫಲಿತಾಂಶ;
	ಫಲಿತಾಂಶ.reserve(cells_.ಗಾತ್ರ_2());
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : cells_)
		ಆದರೆ (design->selected(ಇದು, it.ಎರಡನೇ))
			ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(it.ಎರಡನೇ);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

std::ಸದಿಶ<RTLIL::ಸ್ಮರಣೆ*> RTLIL::Module::selected_memories() ಸ್ಥಿರಾಂಕ
{
	std::ಸದಿಶ<RTLIL::ಸ್ಮರಣೆ*> ಫಲಿತಾಂಶ;
	ಫಲಿತಾಂಶ.reserve(memories.ಗಾತ್ರ_2());
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : memories)
		ಆದರೆ (design->selected(ಇದು, it.ಎರಡನೇ))
			ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(it.ಎರಡನೇ);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

std::ಸದಿಶ<RTLIL::Process*> RTLIL::Module::selected_processes() ಸ್ಥಿರಾಂಕ
{
	std::ಸದಿಶ<RTLIL::Process*> ಫಲಿತಾಂಶ;
	ಫಲಿತಾಂಶ.reserve(processes.ಗಾತ್ರ_2());
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : processes)
		ಆದರೆ (design->selected(ಇದು, it.ಎರಡನೇ))
			ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(it.ಎರಡನೇ);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

std::ಸದಿಶ<RTLIL::SelectedMember> RTLIL::Module::selected_members() ಸ್ಥಿರಾಂಕ
{
	std::ಸದಿಶ<RTLIL::SelectedMember> ಫಲಿತಾಂಶ;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : selected_cells())
		ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ({cell, cell->name});
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಸ್ಮರಣೆ_3 : selected_memories())
		ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ({ಸ್ಮರಣೆ_3, ಸ್ಮರಣೆ_3->name});
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : selected_wires())
		ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ({wire, wire->name});
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ process : selected_processes())
		ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ({process, process->name});
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ RTLIL::Module::ಕೂಡಿಸು(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(ಎಣಿಕೆ_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = ಇದು;
}

ಶೂನ್ಯ RTLIL::Module::ಕೂಡಿಸು(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(ಎಣಿಕೆ_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = ಇದು;
}

ಶೂನ್ಯ RTLIL::Module::ಕೂಡಿಸು(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(ಎಣಿಕೆ_id(process->name) == 0);
	processes[process->name] = process;
	process->module = ಇದು;
}

ಶೂನ್ಯ RTLIL::Module::remove(ಸ್ಥಿರಾಂಕ pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	ಆದರೆ (wires.ಖಾಲಿಗಳು())
		ಹಿಂತಿರುಗಿ;

	ರಚನೆ Delete_Wire_ಕೆಲಸಗಾರ
	{
		RTLIL::Module *module;
		ಸ್ಥಿರಾಂಕ pool<RTLIL::Wire*> *wires_p;

		ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([ಇದು](RTLIL::Wire *&wire) {
				ಆದರೆ (wires_p->ಎಣಿಕೆ_2(wire))
					wire = module->ಕೂಡಿಸು_Wire(stringf("$delete_wire$%d", (ಪೂರ್ಣಾಂಕ)autoidx++), wire->ಅಗಲ);
			});
		}

		ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	Delete_Wire_ಕೆಲಸಗಾರ delete_wire_ಕೆಲಸಗಾರ;
	delete_wire_ಕೆಲಸಗಾರ.module = ಇದು;
	delete_wire_ಕೆಲಸಗಾರ.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_ಕೆಲಸಗಾರ);

	ಆದರೆ (design->flagBufferedNormalized) {
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : wires) {
			buf_norm_wire_ಸರತಿ.erase(wire);
			buf_norm_connect_ಸೂಚ್ಯಂಕ.erase(wire);
		}
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.ಎಣಿಕೆ_2(id) != 0);
		wires_.erase(id);
		ಅಳಿಸು it;
	}
}

ಶೂನ್ಯ RTLIL::Module::remove(RTLIL::Cell *cell)
{
	ಆಗ (!cell->connections_.ಖಾಲಿಗಳು())
		cell->unset_ಪೋರ್ಟ್(cell->connections_.begin()->ಮೊದಲ);

	log_assert(cells_.ಎಣಿಕೆ_2(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	ಆದರೆ (design && design->flagBufferedNormalized && buf_norm_cell_ಸರತಿ.ಎಣಿಕೆ_2(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.ಸೇರಿಸು(cell);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಅಳಿಸು cell;
	}
}

ಶೂನ್ಯ RTLIL::Module::remove(RTLIL::ಸ್ಮರಣೆ *ಸ್ಮರಣೆ_3)
{
	log_assert(memories.ಎಣಿಕೆ_2(ಸ್ಮರಣೆ_3->name) != 0);
	memories.erase(ಸ್ಮರಣೆ_3->name);
	ಅಳಿಸು ಸ್ಮರಣೆ_3;
}

ಶೂನ್ಯ RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.ಎಣಿಕೆ_2(process->name) != 0);
	processes.erase(process->name);
	ಅಳಿಸು process;
}

ಶೂನ್ಯ RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	ಕೂಡಿಸು(wire);
}

ಶೂನ್ಯ RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	ಕೂಡಿಸು(cell);
}

ಶೂನ್ಯ RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	ಆದರೆ (old_id != IdString::Null && wires_.ಎಣಿಕೆ_2(old_id))
		rename(wires_.at(old_id), new_name);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (old_id != IdString::Null && cells_.ಎಣಿಕೆ_2(old_id))
		rename(cells_.at(old_id), new_name);
	ಇಲ್ಲದಿದ್ದರೆ
		log_abort();
}

ಶೂನ್ಯ RTLIL::Module::ಅದಲುಬದಲು_ಹೆಸರುಗಳು(RTLIL::Wire *w1, RTLIL::Wire *w2)
{
	log_assert(w1->name != IdString::Null);
	log_assert(w2->name != IdString::Null);
	IdString id1 = w1->name;
	IdString id2 = w2->name;
	log_assert(wires_[id1] == w1);
	log_assert(wires_[id2] == w2);
	log_assert(refcount_wires_ == 0);

	wires_[id1] = w2;
	wires_[id2] = w1;
	ಅದಲುಬದಲು(w1->name, w2->name);
}

ಶೂನ್ಯ RTLIL::Module::ಅದಲುಬದಲು_ಹೆಸರುಗಳು(RTLIL::Cell *c1, RTLIL::Cell *c2)
{
	log_assert(c1->name != IdString::Null);
	log_assert(c2->name != IdString::Null);
	IdString id1 = c1->name;
	IdString id2 = c2->name;
	log_assert(cells_[id1] == c1);
	log_assert(cells_[id2] == c2);
	log_assert(refcount_cells_ == 0);

	cells_[id1] = c2;
	cells_[id2] = c1;
	ಅದಲುಬದಲು(c1->name, c2->name);
}

RTLIL::IdString RTLIL::Module::uniquify(RTLIL::IdString name)
{
	ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = 0;
	ಹಿಂತಿರುಗಿ uniquify(name, ಸೂಚ್ಯಂಕ);
}

IdString RTLIL::Module::uniquify(IdString name, ಪೂರ್ಣಾಂಕ &ಸೂಚ್ಯಂಕ)
{
	ಆದರೆ (ಸೂಚ್ಯಂಕ == 0) {
		ಆದರೆ (ಎಣಿಕೆ_id(name) == 0)
			ಹಿಂತಿರುಗಿ name;
		ಸೂಚ್ಯಂಕ++;
	}

	ಆಗ (1) {
		IdString new_name = design->twines().ಕೂಡಿಸು(TwineSpec::Suffix{name, stringf("_%d", ಸೂಚ್ಯಂಕ)});
		ಆದರೆ (ಎಣಿಕೆ_id(new_name) == 0)
			ಹಿಂತಿರುಗಿ new_name;
		ಸೂಚ್ಯಂಕ++;
	}
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ fixup_ports_compare(ಸ್ಥಿರಾಂಕ RTLIL::Wire *a, ಸ್ಥಿರಾಂಕ RTLIL::Wire *b)
{
	ಆದರೆ (a->ಪೋರ್ಟ್_id && !b->ಪೋರ್ಟ್_id)
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಆದರೆ (!a->ಪೋರ್ಟ್_id && b->ಪೋರ್ಟ್_id)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಆದರೆ (a->ಪೋರ್ಟ್_id == b->ಪೋರ್ಟ್_id)
		ಹಿಂತಿರುಗಿ a->name < b->name;
	ಹಿಂತಿರುಗಿ a->ಪೋರ್ಟ್_id < b->ಪೋರ್ಟ್_id;
}

ಶೂನ್ಯ RTLIL::Module::connect(ಸ್ಥಿರಾಂಕ RTLIL::SigSig &conn)
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mon : monitors)
		mon->notify_connect(ಇದು, conn);

	ಆದರೆ (design)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mon : design->monitors)
			mon->notify_connect(ಇದು, conn);

	// ignore all attempts to assign constants to other constants
	ಆದರೆ (conn.ಮೊದಲ.has_const()) {
		RTLIL::SigSig new_conn;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(conn.ಮೊದಲ); i++)
			ಆದರೆ (conn.ಮೊದಲ[i].wire) {
				new_conn.ಮೊದಲ.append(conn.ಮೊದಲ[i]);
				new_conn.ಎರಡನೇ.append(conn.ಎರಡನೇ[i]);
			}
		ಆದರೆ (Get_ಗಾತ್ರ(new_conn.ಮೊದಲ))
			connect(new_conn);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (yosys_xtrace) {
		ಲಘುಗಣಕ("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", ಇದು, log_signal(conn.ಮೊದಲ), log_signal(conn.ಎರಡನೇ), Get_ಗಾತ್ರ(conn.ಮೊದಲ));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(Get_ಗಾತ್ರ(conn.ಮೊದಲ) == Get_ಗಾತ್ರ(conn.ಎರಡನೇ));
	connections_.ತಳ್ಳು_ಹಿಂಭಾಗ(conn);
}

ಶೂನ್ಯ RTLIL::Module::connect(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &lhs, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

ಶೂನ್ಯ RTLIL::Module::new_connections(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<RTLIL::SigSig> &new_conn)
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mon : monitors)
		mon->notify_connect(ಇದು, new_conn);

	ಆದರೆ (design)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ mon : design->monitors)
			mon->notify_connect(ಇದು, new_conn);

	ಆದರೆ (yosys_xtrace) {
		ಲಘುಗಣಕ("#X# New connections vector in %s:\n", ಇದು);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &conn: new_conn)
			ಲಘುಗಣಕ("#X#    %s = %s (%d bits)\n", log_signal(conn.ಮೊದಲ), log_signal(conn.ಎರಡನೇ), Get_ಗಾತ್ರ(conn.ಮೊದಲ));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<RTLIL::SigSig> &RTLIL::Module::connections() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ connections_;
}

ಶೂನ್ಯ RTLIL::Module::fixup_ports()
{
	std::ಸದಿಶ<RTLIL::Wire*> all_ports;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &w : wires_)
		ಆದರೆ (w.ಎರಡನೇ->ಪೋರ್ಟ್_ಒಳಹರಿವು || w.ಎರಡನೇ->ಪೋರ್ಟ್_output)
			all_ports.ತಳ್ಳು_ಹಿಂಭಾಗ(w.ಎರಡನೇ);
		ಇಲ್ಲದಿದ್ದರೆ
			w.ಎರಡನೇ->ಪೋರ್ಟ್_id = 0;

	std::ವಿಂಗಡಿಸು(all_ports.begin(), all_ports.ಕೊನೆ(), fixup_ports_compare);

	ಆದರೆ (design && design->flagBufferedNormalized) {
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &w : wires_)
			ಆದರೆ (w.ಎರಡನೇ->driverCell_ && w.ಎರಡನೇ->driverCell_->type == ID($ಒಳಹರಿವು_ಪೋರ್ಟ್))
				buf_norm_wire_ಸರತಿ.ಸೇರಿಸು(w.ಎರಡನೇ);

		buf_norm_wire_ಸರತಿ.ಸೇರಿಸು(all_ports.begin(), all_ports.ಕೊನೆ());
	}

	ports.clear();
	ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < all_ports.ಗಾತ್ರ_2(); i++) {
		ports.ತಳ್ಳು_ಹಿಂಭಾಗ(all_ports[i]->name);
		all_ports[i]->ಪೋರ್ಟ್_id = i+1;
	}
}

ಶೂನ್ಯ RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		ಸ್ಥಿರಾಂಕ dict<IdString, RTLIL::Const> &src,
		ಸ್ಥಿರಾಂಕ RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	ಆದರೆ (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		ಹಿಂತಿರುಗಿ;
	}
	dst.clear();
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = Get_ಗಾತ್ರ(src) - 1; i >= 0; i--) {
		ಸ್ವಯಂಚಾಲಿತ &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.ಮೊದಲ)] = it.ಎರಡನೇ;
	}
}

RTLIL::Wire *RTLIL::Module::ಕೂಡಿಸು_Wire(RTLIL::IdString name, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
	log_assert(design);
	log_assert(ಅಗಲ >= 0 && ಅಗಲ < RTLIL::ಅಗಲ_ಎಲ್ಲೆ);
	RTLIL::Wire *wire = ಹೊಸ RTLIL::Wire;
	wire->ಅಗಲ = ಅಗಲ;
	wire->name = name;
	ಕೂಡಿಸು(wire);
	ಹಿಂತಿರುಗಿ wire;
}

RTLIL::Wire *RTLIL::Module::ಕೂಡಿಸು_Wire(IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Wire *other)
{
	RTLIL::Wire *wire = ಕೂಡಿಸು_Wire(name);
	wire->ಅಗಲ = other->ಅಗಲ;
	wire->ಆರಂಭ_ಸ್ಥಾನಾಂತರ = other->ಆರಂಭ_ಸ್ಥಾನಾಂತರ;
	wire->ಪೋರ್ಟ್_id = other->ಪೋರ್ಟ್_id;
	wire->ಪೋರ್ಟ್_ಒಳಹರಿವು = other->ಪೋರ್ಟ್_ಒಳಹರಿವು;
	wire->ಪೋರ್ಟ್_output = other->ಪೋರ್ಟ್_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ಸ್ಥಿರಾಂಕ RTLIL::Design *src_design = other->module ? other->module->design : ಶೂನ್ಯಸೂಚಕ;
		RTLIL::copy_attr_dict(wire->ಗುಣಲಕ್ಷಣಗಳು, other->ಗುಣಲಕ್ಷಣಗಳು, src_design, ಇದು->design);
	}
	ಹಿಂತಿರುಗಿ wire;
}

RTLIL::Cell *RTLIL::Module::ಕೂಡಿಸು_Cell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = ಹೊಸ RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	ಕೂಡಿಸು(cell);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell *RTLIL::Module::ಕೂಡಿಸು_Cell(IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Cell *other)
{
	ಸ್ಥಿರಾಂಕ RTLIL::Design *src_design = other->module ? other->module->design : ಶೂನ್ಯಸೂಚಕ;
	ತಾರ್ಕಿಕ cross_pool = src_design && ಇದು->design && src_design != ಇದು->design;

	IdString type = other->type.ref();
	ಆದರೆ (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, ಇದು->design);
	RTLIL::copy_attr_dict(cell->ಗುಣಲಕ್ಷಣಗಳು, other->ಗುಣಲಕ್ಷಣಗಳು, src_design, ಇದು->design);

	ಆದರೆ (!cross_pool)
		cell->connections_ = other->connections_;
	ಇಲ್ಲದಿದ್ದರೆ
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = Get_ಗಾತ್ರ(other->connections_) - 1; i >= 0; i--) {
			ಸ್ವಯಂಚಾಲಿತ &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.ಮೊದಲ)] = c.ಎರಡನೇ;
		}
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::ಸ್ಮರಣೆ *RTLIL::Module::ಕೂಡಿಸು_ಸ್ಮರಣೆ(IdString name)
{
	log_assert(design);
	RTLIL::ಸ್ಮರಣೆ *mem = ಹೊಸ RTLIL::ಸ್ಮರಣೆ;
	mem->module = ಇದು;
	mem->name = name;
	memories[name] = mem;
	ಹಿಂತಿರುಗಿ mem;
}

RTLIL::ಸ್ಮರಣೆ *RTLIL::Module::ಕೂಡಿಸು_ಸ್ಮರಣೆ(IdString name, ಸ್ಥಿರಾಂಕ RTLIL::ಸ್ಮರಣೆ *other)
{
	log_assert(design);
	RTLIL::ಸ್ಮರಣೆ *mem = ಹೊಸ RTLIL::ಸ್ಮರಣೆ;
	mem->module = ಇದು;
	mem->name = name;
	mem->ಅಗಲ = other->ಅಗಲ;
	mem->ಆರಂಭ_ಸ್ಥಾನಾಂತರ = other->ಆರಂಭ_ಸ್ಥಾನಾಂತರ;
	mem->ಗಾತ್ರ_2 = other->ಗಾತ್ರ_2;
	RTLIL::copy_attr_dict(mem->ಗುಣಲಕ್ಷಣಗಳು, other->ಗುಣಲಕ್ಷಣಗಳು,
			other->module ? other->module->design : ಶೂನ್ಯಸೂಚಕ, design);
	memories[name] = mem;
	ಹಿಂತಿರುಗಿ mem;
}

RTLIL::Process *RTLIL::Module::ಕೂಡಿಸು_Process(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = ಹೊಸ RTLIL::Process;
	proc->name = name;
	ಕೂಡಿಸು(proc);
	ಹಿಂತಿರುಗಿ proc;
}

RTLIL::Process *RTLIL::Module::ಕೂಡಿಸು_Process(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ಸ್ಥಿರಾಂಕ RTLIL::Design *src_design = other->module ? other->module->design : ಶೂನ್ಯಸೂಚಕ;
	ಆದರೆ (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> ಗುಣಲಕ್ಷಣಗಳು;
			RTLIL::copy_attr_dict(ಗುಣಲಕ್ಷಣಗಳು, obj->ಗುಣಲಕ್ಷಣಗಳು, src_design, design);
			obj->ಗುಣಲಕ್ಷಣಗಳು = std::move(ಗುಣಲಕ್ಷಣಗಳು);
		});
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ *ಸಮಕಾಲೀಕರಣ : proc->syncs)
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &mwa : ಸಮಕಾಲೀಕರಣ->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	ಕೂಡಿಸು(proc);
	ಹಿಂತಿರುಗಿ proc;
}

#ವ್ಯಾಖ್ಯಾನಿಸು DEF_ವಿಧಾನ(_func, y_ಗಾತ್ರ, _type) \
	RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು ## _func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ತಾರ್ಕಿಕ is_signed, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_ಅಗಲ] = sig_a.ಗಾತ್ರ_2();       \
		cell->parameters[ID::Y_ಅಗಲ] = sig_y.ಗಾತ್ರ_2();       \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);                        \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);                        \
		cell->ಗಣ_src_attribute(src);                       \
		ಹಿಂತಿರುಗಿ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ತಾರ್ಕಿಕ is_signed, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::SigSpec sig_y = ಕೂಡಿಸು_Wire(NEW_ID, y_ಗಾತ್ರ);    \
		ಕೂಡಿಸು ## _func(name, sig_a, sig_y, is_signed, src);   \
		ಹಿಂತಿರುಗಿ sig_y;                                       \
	}
DEF_ವಿಧಾನ(Not,        sig_a.ಗಾತ್ರ_2(), ID($not))
DEF_ವಿಧಾನ(Pos,        sig_a.ಗಾತ್ರ_2(), ID($pos))
DEF_ವಿಧಾನ(Neg,        sig_a.ಗಾತ್ರ_2(), ID($neg))
DEF_ವಿಧಾನ(ReduceAnd,  1, ID($reduce_and))
DEF_ವಿಧಾನ(ReduceOr,   1, ID($reduce_or))
DEF_ವಿಧಾನ(ReduceXor,  1, ID($reduce_xor))
DEF_ವಿಧಾನ(ReduceXnor, 1, ID($reduce_xnor))
DEF_ವಿಧಾನ(ReduceBool, 1, ID($reduce_bool))
DEF_ವಿಧಾನ(LogicNot,   1, ID($logic_not))
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEF_ವಿಧಾನ

#ವ್ಯಾಖ್ಯಾನಿಸು DEF_ವಿಧಾನ(_func, y_ಗಾತ್ರ, _type) \
	RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು ## _func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ತಾರ್ಕಿಕ /* is_signed */, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, _type);           \
		cell->parameters[ID::ಅಗಲ_2] = sig_a.ಗಾತ್ರ_2();         \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);                        \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);                        \
		cell->ಗಣ_src_attribute(src);                       \
		ಹಿಂತಿರುಗಿ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ತಾರ್ಕಿಕ is_signed, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::SigSpec sig_y = ಕೂಡಿಸು_Wire(NEW_ID, y_ಗಾತ್ರ);    \
		ಕೂಡಿಸು ## _func(name, sig_a, sig_y, is_signed, src);   \
		ಹಿಂತಿರುಗಿ sig_y;                                       \
	}
DEF_ವಿಧಾನ(Buf,     sig_a.ಗಾತ್ರ_2(), ID($buf))
DEF_ವಿಧಾನ(Barrier, sig_a.ಗಾತ್ರ_2(), ID($barrier))
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEF_ವಿಧಾನ

#ವ್ಯಾಖ್ಯಾನಿಸು DEF_ವಿಧಾನ(_func, y_ಗಾತ್ರ, _type) \
	RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು ## _func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ತಾರ್ಕಿಕ is_signed, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_ಅಗಲ] = sig_a.ಗಾತ್ರ_2();       \
		cell->parameters[ID::B_ಅಗಲ] = sig_b.ಗಾತ್ರ_2();       \
		cell->parameters[ID::Y_ಅಗಲ] = sig_y.ಗಾತ್ರ_2();       \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);                        \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::B, sig_b);                        \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);                        \
		cell->ಗಣ_src_attribute(src);                       \
		ಹಿಂತಿರುಗಿ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ತಾರ್ಕಿಕ is_signed, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::SigSpec sig_y = ಕೂಡಿಸು_Wire(NEW_ID, y_ಗಾತ್ರ);         \
		ಕೂಡಿಸು ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ಹಿಂತಿರುಗಿ sig_y;                                            \
	}
DEF_ವಿಧಾನ(And,      max(sig_a.ಗಾತ್ರ_2(), sig_b.ಗಾತ್ರ_2()), ID($and))
DEF_ವಿಧಾನ(Or,       max(sig_a.ಗಾತ್ರ_2(), sig_b.ಗಾತ್ರ_2()), ID($or))
DEF_ವಿಧಾನ(Xor,      max(sig_a.ಗಾತ್ರ_2(), sig_b.ಗಾತ್ರ_2()), ID($xor))
DEF_ವಿಧಾನ(Xnor,     max(sig_a.ಗಾತ್ರ_2(), sig_b.ಗಾತ್ರ_2()), ID($xnor))
DEF_ವಿಧಾನ(ಸರಿಸು_2,    sig_a.ಗಾತ್ರ_2(), ID($ಸರಿಸು))
DEF_ವಿಧಾನ(Lt,       1, ID($lt))
DEF_ವಿಧಾನ(Le,       1, ID($le))
DEF_ವಿಧಾನ(Eq,       1, ID($eq))
DEF_ವಿಧಾನ(Ne,       1, ID($ne))
DEF_ವಿಧಾನ(Eqx,      1, ID($eqx))
DEF_ವಿಧಾನ(Nex,      1, ID($nex))
DEF_ವಿಧಾನ(Ge,       1, ID($ge))
DEF_ವಿಧಾನ(Gt,       1, ID($gt))
DEF_ವಿಧಾನ(ಕೂಡಿಸು_2,      max(sig_a.ಗಾತ್ರ_2(), sig_b.ಗಾತ್ರ_2()), ID($ಕೂಡಿಸು))
DEF_ವಿಧಾನ(Sub,      max(sig_a.ಗಾತ್ರ_2(), sig_b.ಗಾತ್ರ_2()), ID($sub))
DEF_ವಿಧಾನ(Mul,      max(sig_a.ಗಾತ್ರ_2(), sig_b.ಗಾತ್ರ_2()), ID($mul))
DEF_ವಿಧಾನ(Div,      max(sig_a.ಗಾತ್ರ_2(), sig_b.ಗಾತ್ರ_2()), ID($div))
DEF_ವಿಧಾನ(Mod,      max(sig_a.ಗಾತ್ರ_2(), sig_b.ಗಾತ್ರ_2()), ID($mod))
DEF_ವಿಧಾನ(DivFloor, max(sig_a.ಗಾತ್ರ_2(), sig_b.ಗಾತ್ರ_2()), ID($divfloor))
DEF_ವಿಧಾನ(ModFloor, max(sig_a.ಗಾತ್ರ_2(), sig_b.ಗಾತ್ರ_2()), ID($modfloor))
DEF_ವಿಧಾನ(LogicAnd, 1, ID($logic_and))
DEF_ವಿಧಾನ(LogicOr,  1, ID($logic_or))
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEF_ವಿಧಾನ

#ವ್ಯಾಖ್ಯಾನಿಸು DEF_ವಿಧಾನ(_func, y_ಗಾತ್ರ, _type) \
	RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು ## _func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ತಾರ್ಕಿಕ is_signed, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = ಸುಳ್ಳು;             \
		cell->parameters[ID::A_ಅಗಲ] = sig_a.ಗಾತ್ರ_2();       \
		cell->parameters[ID::B_ಅಗಲ] = sig_b.ಗಾತ್ರ_2();       \
		cell->parameters[ID::Y_ಅಗಲ] = sig_y.ಗಾತ್ರ_2();       \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);                        \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::B, sig_b);                        \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);                        \
		cell->ಗಣ_src_attribute(src);                       \
		ಹಿಂತಿರುಗಿ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ತಾರ್ಕಿಕ is_signed, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::SigSpec sig_y = ಕೂಡಿಸು_Wire(NEW_ID, y_ಗಾತ್ರ);         \
		ಕೂಡಿಸು ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ಹಿಂತಿರುಗಿ sig_y;                                            \
	}
DEF_ವಿಧಾನ(Shl,      sig_a.ಗಾತ್ರ_2(), ID($shl))
DEF_ವಿಧಾನ(Shr,      sig_a.ಗಾತ್ರ_2(), ID($shr))
DEF_ವಿಧಾನ(Sshl,     sig_a.ಗಾತ್ರ_2(), ID($sshl))
DEF_ವಿಧಾನ(Sshr,     sig_a.ಗಾತ್ರ_2(), ID($sshr))
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEF_ವಿಧಾನ

#ವ್ಯಾಖ್ಯಾನಿಸು DEF_ವಿಧಾನ(_func, y_ಗಾತ್ರ, _type) \
	RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು ## _func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ತಾರ್ಕಿಕ is_signed, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = ಸುಳ್ಳು;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_ಅಗಲ] = sig_a.ಗಾತ್ರ_2();       \
		cell->parameters[ID::B_ಅಗಲ] = sig_b.ಗಾತ್ರ_2();       \
		cell->parameters[ID::Y_ಅಗಲ] = sig_y.ಗಾತ್ರ_2();       \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);                        \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::B, sig_b);                        \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);                        \
		cell->ಗಣ_src_attribute(src);                       \
		ಹಿಂತಿರುಗಿ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ತಾರ್ಕಿಕ is_signed, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::SigSpec sig_y = ಕೂಡಿಸು_Wire(NEW_ID, y_ಗಾತ್ರ);         \
		ಕೂಡಿಸು ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ಹಿಂತಿರುಗಿ sig_y;                                            \
	}
DEF_ವಿಧಾನ(Shiftx,      sig_a.ಗಾತ್ರ_2(), ID($shiftx))
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEF_ವಿಧಾನ

#ವ್ಯಾಖ್ಯಾನಿಸು DEF_ವಿಧಾನ(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು ## _func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, _type);                 \
		cell->parameters[ID::ಅಗಲ_2] = sig_a.ಗಾತ್ರ_2();               \
		ಆದರೆ (_pmux) cell->parameters[ID::S_ಅಗಲ] = sig_s.ಗಾತ್ರ_2();  \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);                              \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::B, sig_b);                              \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::S, sig_s);                              \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);                              \
		cell->ಗಣ_src_attribute(src);                             \
		ಹಿಂತಿರುಗಿ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::SigSpec sig_y = ಕೂಡಿಸು_Wire(NEW_ID, sig_a.ಗಾತ್ರ_2());     \
		ಕೂಡಿಸು ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		ಹಿಂತಿರುಗಿ sig_y;                                             \
	}
DEF_ವಿಧಾನ(Mux,      ID($mux),        0)
DEF_ವಿಧಾನ(Bwmux,    ID($bwmux),      0)
DEF_ವಿಧಾನ(Pmux,     ID($pmux),       1)
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEF_ವಿಧಾನ

#ವ್ಯಾಖ್ಯಾನಿಸು DEF_ವಿಧಾನ(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು ## _func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, _type);                 \
		cell->parameters[ID::ಅಗಲ_2] = _demux ? sig_a.ಗಾತ್ರ_2() : sig_y.ಗಾತ್ರ_2(); \
		cell->parameters[ID::S_ಅಗಲ] = sig_s.ಗಾತ್ರ_2();             \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);                              \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::S, sig_s);                              \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);                              \
		cell->ಗಣ_src_attribute(src);                             \
		ಹಿಂತಿರುಗಿ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::SigSpec sig_y = ಕೂಡಿಸು_Wire(NEW_ID, _demux ? sig_a.ಗಾತ್ರ_2() << sig_s.ಗಾತ್ರ_2() : sig_a.ಗಾತ್ರ_2() >> sig_s.ಗಾತ್ರ_2()); \
		ಕೂಡಿಸು ## _func(name, sig_a, sig_s, sig_y, src);             \
		ಹಿಂತಿರುಗಿ sig_y;                                             \
	}
DEF_ವಿಧಾನ(Bmux,     ID($bmux),       0)
DEF_ವಿಧಾನ(Demux,    ID($demux),      1)
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEF_ವಿಧಾನ

#ವ್ಯಾಖ್ಯಾನಿಸು DEF_ವಿಧಾನ(_func, _type) \
	RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು ## _func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, _type);                 \
		cell->parameters[ID::ಅಗಲ_2] = sig_a.ಗಾತ್ರ_2();               \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);                              \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::B, sig_b);                              \
		cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);                              \
		cell->ಗಣ_src_attribute(src);                             \
		ಹಿಂತಿರುಗಿ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::SigSpec sig_y = ಕೂಡಿಸು_Wire(NEW_ID, sig_a.ಗಾತ್ರ_2());     \
		ಕೂಡಿಸು ## _func(name, sig_a, sig_s, sig_y, src);             \
		ಹಿಂತಿರುಗಿ sig_y;                                             \
	}
DEF_ವಿಧಾನ(Bweqx,    ID($bweqx))
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEF_ವಿಧಾನ

#ವ್ಯಾಖ್ಯಾನಿಸು DEF_ವಿಧಾನ_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು ## _func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig1, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig2, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, _type);         \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P1, sig1);                   \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P2, sig2);                   \
		cell->ಗಣ_src_attribute(src);                     \
		ಹಿಂತಿರುಗಿ cell;                                      \
	} \
	RTLIL::Sig_ಬಿಟ್ RTLIL::Module::_func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig1, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Sig_ಬಿಟ್ sig2 = ಕೂಡಿಸು_Wire(NEW_ID);             \
		ಕೂಡಿಸು ## _func(name, sig1, sig2, src);              \
		ಹಿಂತಿರುಗಿ sig2;                                      \
	}
#ವ್ಯಾಖ್ಯಾನಿಸು DEF_ವಿಧಾನ_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು ## _func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig1, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig2, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig3, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, _type);         \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P1, sig1);                   \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P2, sig2);                   \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P3, sig3);                   \
		cell->ಗಣ_src_attribute(src);                     \
		ಹಿಂತಿರುಗಿ cell;                                      \
	} \
	RTLIL::Sig_ಬಿಟ್ RTLIL::Module::_func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig1, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig2, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Sig_ಬಿಟ್ sig3 = ಕೂಡಿಸು_Wire(NEW_ID);             \
		ಕೂಡಿಸು ## _func(name, sig1, sig2, sig3, src);        \
		ಹಿಂತಿರುಗಿ sig3;                                      \
	}
#ವ್ಯಾಖ್ಯಾನಿಸು DEF_ವಿಧಾನ_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು ## _func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig1, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig2, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig3, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig4, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, _type);         \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P1, sig1);                   \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P2, sig2);                   \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P3, sig3);                   \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P4, sig4);                   \
		cell->ಗಣ_src_attribute(src);                     \
		ಹಿಂತಿರುಗಿ cell;                                      \
	} \
	RTLIL::Sig_ಬಿಟ್ RTLIL::Module::_func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig1, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig2, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig3, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Sig_ಬಿಟ್ sig4 = ಕೂಡಿಸು_Wire(NEW_ID);             \
		ಕೂಡಿಸು ## _func(name, sig1, sig2, sig3, sig4, src);  \
		ಹಿಂತಿರುಗಿ sig4;                                      \
	}
#ವ್ಯಾಖ್ಯಾನಿಸು DEF_ವಿಧಾನ_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು ## _func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig1, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig2, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig3, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig4, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig5, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, _type);         \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P1, sig1);                   \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P2, sig2);                   \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P3, sig3);                   \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P4, sig4);                   \
		cell->ಗಣ_ಪೋರ್ಟ್("\\" #_P5, sig5);                   \
		cell->ಗಣ_src_attribute(src);                     \
		ಹಿಂತಿರುಗಿ cell;                                      \
	} \
	RTLIL::Sig_ಬಿಟ್ RTLIL::Module::_func(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig1, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig2, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig3, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig4, ಸ್ಥಿರಾಂಕ std::string &src) { \
		RTLIL::Sig_ಬಿಟ್ sig5 = ಕೂಡಿಸು_Wire(NEW_ID);                  \
		ಕೂಡಿಸು ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		ಹಿಂತಿರುಗಿ sig5;                                           \
	}
DEF_ವಿಧಾನ_2(BufGate,    ID($_BUF_),    A, Y)
DEF_ವಿಧಾನ_2(NotGate,    ID($_NOT_),    A, Y)
DEF_ವಿಧಾನ_3(AndGate,    ID($_AND_),    A, B, Y)
DEF_ವಿಧಾನ_3(NandGate,   ID($_NAND_),   A, B, Y)
DEF_ವಿಧಾನ_3(OrGate,     ID($_OR_),     A, B, Y)
DEF_ವಿಧಾನ_3(NorGate,    ID($_NOR_),    A, B, Y)
DEF_ವಿಧಾನ_3(XorGate,    ID($_XOR_),    A, B, Y)
DEF_ವಿಧಾನ_3(XnorGate,   ID($_XNOR_),   A, B, Y)
DEF_ವಿಧಾನ_3(AndnotGate, ID($_ANDNOT_), A, B, Y)
DEF_ವಿಧಾನ_3(OrnotGate,  ID($_ORNOT_),  A, B, Y)
DEF_ವಿಧಾನ_4(MuxGate,    ID($_MUX_),    A, B, S, Y)
DEF_ವಿಧಾನ_4(NmuxGate,   ID($_NMUX_),   A, B, S, Y)
DEF_ವಿಧಾನ_4(Aoi3Gate,   ID($_AOI3_),   A, B, C, Y)
DEF_ವಿಧಾನ_4(Oai3Gate,   ID($_OAI3_),   A, B, C, Y)
DEF_ವಿಧಾನ_5(Aoi4Gate,   ID($_AOI4_),   A, B, C, D, Y)
DEF_ವಿಧಾನ_5(Oai4Gate,   ID($_OAI4_),   A, B, C, D, Y)
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEF_ವಿಧಾನ_2
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEF_ವಿಧಾನ_3
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEF_ವಿಧಾನ_4
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು DEF_ವಿಧಾನ_5

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Pow(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ತಾರ್ಕಿಕ a_signed, ತಾರ್ಕಿಕ b_signed, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($ಘಾತ));
	cell->parameters[ID::A_SIGNED] = a_signed;
	cell->parameters[ID::B_SIGNED] = b_signed;
	cell->parameters[ID::A_ಅಗಲ] = sig_a.ಗಾತ್ರ_2();
	cell->parameters[ID::B_ಅಗಲ] = sig_b.ಗಾತ್ರ_2();
	cell->parameters[ID::Y_ಅಗಲ] = sig_y.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::B, sig_b);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Fa(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_c, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_x, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($fa));
	cell->parameters[ID::ಅಗಲ_2] = sig_a.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::B, sig_b);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::C, sig_c);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::X, sig_x);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_ತುಂಡು(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, RTLIL::Const ಸ್ಥಾನಾಂತರ, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($ತುಂಡು));
	cell->parameters[ID::A_ಅಗಲ] = sig_a.ಗಾತ್ರ_2();
	cell->parameters[ID::Y_ಅಗಲ] = sig_y.ಗಾತ್ರ_2();
	cell->parameters[ID::ಸ್ಥಾನಾಂತರ_2] = ಸ್ಥಾನಾಂತರ;
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Concat(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($concat));
	cell->parameters[ID::A_ಅಗಲ] = sig_a.ಗಾತ್ರ_2();
	cell->parameters[ID::B_ಅಗಲ] = sig_b.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::B, sig_b);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Lut(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, RTLIL::Const lut, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::ಅಗಲ_2] = sig_a.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Tribuf(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($tribuf));
	cell->parameters[ID::ಅಗಲ_2] = sig_a.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Assert(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($ಪ್ರತಿಜ್ಞಾ));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Assume(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($assume));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_ಜೀವಂತ(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($ಜೀವಂತ));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Fair(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($fair));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Cover(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($cover));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Equiv(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($equiv));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::B, sig_b);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Sr(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ ಗಣ_polarity, ತಾರ್ಕಿಕ clr_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($sr));
	cell->parameters[ID::ಗಣ_POLARITY] = ಗಣ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::ಗಣ_2, sig_ಗಣ);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLR, sig_clr);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Ff(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($ff));
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Dff(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLK, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Dffe(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($dffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLK, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Dffsr(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ ಗಣ_polarity, ತಾರ್ಕಿಕ clr_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($dffsr));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ಗಣ_POLARITY] = ಗಣ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLK, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::ಗಣ_2, sig_ಗಣ);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLR, sig_clr);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Dffsre(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ ಗಣ_polarity, ತಾರ್ಕಿಕ clr_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($dffsre));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ಗಣ_POLARITY] = ಗಣ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLK, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::ಗಣ_2, sig_ಗಣ);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLR, sig_clr);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Adff(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_arst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ arst_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ಮೌಲ್ಯ] = arst_ಮೌಲ್ಯ;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLK, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::ARST, sig_arst);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Adffe(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_arst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ arst_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ಮೌಲ್ಯ] = arst_ಮೌಲ್ಯ;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLK, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::ARST, sig_arst);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Aldff(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_aload, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ad, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ aload_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($aldff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLK, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::ALOAD, sig_aload);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::AD, sig_ad);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Aldffe(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_aload, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ad, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ aload_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($aldffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLK, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::ALOAD, sig_aload);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::AD, sig_ad);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Sdff(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_srst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ srst_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ಮೌಲ್ಯ] = srst_ಮೌಲ್ಯ;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLK, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::SRST, sig_srst);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Sdffe(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_srst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ srst_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ಮೌಲ್ಯ] = srst_ಮೌಲ್ಯ;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLK, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::SRST, sig_srst);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Sdffce(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_srst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ srst_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ಮೌಲ್ಯ] = srst_ಮೌಲ್ಯ;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLK, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::SRST, sig_srst);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Dlatch(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ en_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Adlatch(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_arst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ arst_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ಮೌಲ್ಯ] = arst_ಮೌಲ್ಯ;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::ARST, sig_arst);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Dlatchsr(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ ಗಣ_polarity, ತಾರ್ಕಿಕ clr_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($dlatchsr));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ಗಣ_POLARITY] = ಗಣ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::EN, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::ಗಣ_2, sig_ಗಣ);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLR, sig_clr);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Sr_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr,
		ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ ಗಣ_polarity, ತಾರ್ಕಿಕ clr_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_SR_%c%c_", ಗಣ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::S, sig_ಗಣ);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::R, sig_clr);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Ff_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($_FF_));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Dff_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::C, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Dffe_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::C, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::E, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Dffsr_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ ಗಣ_polarity, ತಾರ್ಕಿಕ clr_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', ಗಣ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::C, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::S, sig_ಗಣ);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::R, sig_clr);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Dffsre_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ ಗಣ_polarity, ತಾರ್ಕಿಕ clr_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', ಗಣ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::C, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::S, sig_ಗಣ);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::R, sig_clr);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::E, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Adff_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_arst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		ತಾರ್ಕಿಕ arst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ arst_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ಮೌಲ್ಯ ? '1' : '0'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::C, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::R, sig_arst);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Adffe_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_arst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		ತಾರ್ಕಿಕ arst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ arst_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ಮೌಲ್ಯ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::C, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::R, sig_arst);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::E, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Aldff_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_aload, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ad, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ aload_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::C, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::L, sig_aload);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::AD, sig_ad);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Aldffe_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_aload, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ad, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ aload_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::C, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::L, sig_aload);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::E, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::AD, sig_ad);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Sdff_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_srst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		ತಾರ್ಕಿಕ srst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ srst_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ಮೌಲ್ಯ ? '1' : '0'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::C, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::R, sig_srst);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Sdffe_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_srst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		ತಾರ್ಕಿಕ srst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ srst_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ಮೌಲ್ಯ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::C, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::R, sig_srst);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::E, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Sdffce_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_srst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		ತಾರ್ಕಿಕ srst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ srst_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ಮೌಲ್ಯ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::C, sig_clk);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::R, sig_srst);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::E, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Dlatch_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ en_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::E, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Adlatch_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_arst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
		ತಾರ್ಕಿಕ arst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ arst_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ಮೌಲ್ಯ ? '1' : '0'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::E, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::R, sig_arst);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Dlatchsr_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ ಗಣ_polarity, ತಾರ್ಕಿಕ clr_polarity, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', ಗಣ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::E, sig_en);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::S, sig_ಗಣ);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::R, sig_clr);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Anyinit(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($anyinit));
	cell->parameters[ID::ಅಗಲ_2] = sig_q.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::D, sig_d);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, sig_q);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::SigSpec sig = ಕೂಡಿಸು_Wire(NEW_ID, ಅಗಲ);
	Cell *cell = ಕೂಡಿಸು_Cell(name, ID($anyconst));
	cell->ಗಣ_Param(ID::ಅಗಲ_2, ಅಗಲ);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::SigSpec sig = ಕೂಡಿಸು_Wire(NEW_ID, ಅಗಲ);
	Cell *cell = ಕೂಡಿಸು_Cell(name, ID($anyseq));
	cell->ಗಣ_Param(ID::ಅಗಲ_2, ಅಗಲ);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::SigSpec sig = ಕೂಡಿಸು_Wire(NEW_ID, ಅಗಲ);
	Cell *cell = ಕೂಡಿಸು_Cell(name, ID($allconst));
	cell->ಗಣ_Param(ID::ಅಗಲ_2, ಅಗಲ);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::SigSpec sig = ಕೂಡಿಸು_Wire(NEW_ID, ಅಗಲ);
	Cell *cell = ಕೂಡಿಸು_Cell(name, ID($allseq));
	cell->ಗಣ_Param(ID::ಅಗಲ_2, ಅಗಲ);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::SigSpec sig = ಕೂಡಿಸು_Wire(NEW_ID);
	Cell *cell = ಕೂಡಿಸು_Cell(name, ID($initstate));
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ sig;
}

RTLIL::SigSpec RTLIL::Module::ಗಣ_ಟ್ಯಾಗ್_2(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ std::string &ಟ್ಯಾಗ್, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_c, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::SigSpec sig = ಕೂಡಿಸು_Wire(NEW_ID, sig_a.ಗಾತ್ರ_2());
	Cell *cell = ಕೂಡಿಸು_Cell(name, ID($ಗಣ_ಟ್ಯಾಗ್));
	cell->parameters[ID::ಅಗಲ_2] = sig_a.ಗಾತ್ರ_2();
	cell->parameters[ID::ಟ್ಯಾಗ್_2] = ಟ್ಯಾಗ್;
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::ಗಣ_2, sig_s);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLR, sig_c);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ sig;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_ಗಣ_ಟ್ಯಾಗ್(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ std::string &ಟ್ಯಾಗ್, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_c, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src)
{
	Cell *cell = ಕೂಡಿಸು_Cell(name, ID($ಗಣ_ಟ್ಯಾಗ್));
	cell->parameters[ID::ಅಗಲ_2] = sig_a.ಗಾತ್ರ_2();
	cell->parameters[ID::ಟ್ಯಾಗ್_2] = ಟ್ಯಾಗ್;
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::ಗಣ_2, sig_s);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLR, sig_c);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig_y);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::SigSpec RTLIL::Module::Get_ಟ್ಯಾಗ್(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ std::string &ಟ್ಯಾಗ್, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::SigSpec sig = ಕೂಡಿಸು_Wire(NEW_ID, sig_a.ಗಾತ್ರ_2());
	Cell *cell = ಕೂಡಿಸು_Cell(name, ID($get_ಟ್ಯಾಗ್));
	cell->parameters[ID::ಅಗಲ_2] = sig_a.ಗಾತ್ರ_2();
	cell->parameters[ID::ಟ್ಯಾಗ್_2] = ಟ್ಯಾಗ್;
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ sig;
}

RTLIL::Cell* RTLIL::Module::ಕೂಡಿಸು_Overwrite_ಟ್ಯಾಗ್(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ std::string &ಟ್ಯಾಗ್, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_c, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::Cell *cell = ಕೂಡಿಸು_Cell(name, ID($overwrite_ಟ್ಯಾಗ್));
	cell->parameters[ID::ಅಗಲ_2] = sig_a.ಗಾತ್ರ_2();
	cell->parameters[ID::ಟ್ಯಾಗ್_2] = ಟ್ಯಾಗ್;
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::ಗಣ_2, sig_s);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::CLR, sig_c);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ cell;
}

RTLIL::SigSpec RTLIL::Module::Original_ಟ್ಯಾಗ್(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ std::string &ಟ್ಯಾಗ್, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::SigSpec sig = ಕೂಡಿಸು_Wire(NEW_ID, sig_a.ಗಾತ್ರ_2());
	Cell *cell = ಕೂಡಿಸು_Cell(name, ID($original_ಟ್ಯಾಗ್));
	cell->parameters[ID::ಅಗಲ_2] = sig_a.ಗಾತ್ರ_2();
	cell->parameters[ID::ಟ್ಯಾಗ್_2] = ಟ್ಯಾಗ್;
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_a);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_e, ಸ್ಥಿರಾಂಕ std::string &src)
{
	RTLIL::SigSpec sig = ಕೂಡಿಸು_Wire(NEW_ID, sig_e.ಗಾತ್ರ_2());
	Cell *cell = ಕೂಡಿಸು_Cell(name, ID($future_ff));
	cell->parameters[ID::ಅಗಲ_2] = sig_e.ಗಾತ್ರ_2();
	cell->ಗಣ_ಪೋರ್ಟ್(ID::A, sig_e);
	cell->ಗಣ_ಪೋರ್ಟ್(ID::Y, sig);
	cell->ಗಣ_src_attribute(src);
	ಹಿಂತಿರುಗಿ sig;
}

std::string RTLIL::Module::ಗಮ್ಯ_rtlil_str() ಸ್ಥಿರಾಂಕ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(ಇದು), design, ಸುಳ್ಳು, ನಿಜ, ಸುಳ್ಳು, RTLIL_BACKEND::Dump_ಬಹುಲಕ::Readable);
	ಹಿಂತಿರುಗಿ f.str();
}

RTLIL::Wire::Wire()
{
	ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hashidx_ಎಣಿಕೆ = 123456789;
	hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
	hashidx_ = hashidx_ಎಣಿಕೆ;

	module = ಶೂನ್ಯಸೂಚಕ;
	ಅಗಲ = 1;
	ಆರಂಭ_ಸ್ಥಾನಾಂತರ = 0;
	ಪೋರ್ಟ್_id = 0;
	ಪೋರ್ಟ್_ಒಳಹರಿವು = ಸುಳ್ಳು;
	ಪೋರ್ಟ್_output = ಸುಳ್ಳು;
	upto = ಸುಳ್ಳು;
	is_signed = ಸುಳ್ಳು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->ಸೇರಿಸು(std::pair<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Wire*>(hashidx_, ಇದು));
#ಆದರೆ_ಅಂತ್ಯ
}

RTLIL::Wire::~Wire()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#ಆದರೆ_ಅಂತ್ಯ
}

std::string RTLIL::Wire::ಗಮ್ಯ_rtlil_str() ಸ್ಥಿರಾಂಕ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", ಇದು, design(), RTLIL_BACKEND::Dump_ಬಹುಲಕ::Readable);
	ಹಿಂತಿರುಗಿ f.str();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
ಸ್ಥಿರ std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Wire*> all_wires;
std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ &all_wires;
}
#ಆದರೆ_ಅಂತ್ಯ

RTLIL::ಸ್ಮರಣೆ::ಸ್ಮರಣೆ()
{
	ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hashidx_ಎಣಿಕೆ = 123456789;
	hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
	hashidx_ = hashidx_ಎಣಿಕೆ;

	ಅಗಲ = 1;
	ಆರಂಭ_ಸ್ಥಾನಾಂತರ = 0;
	ಗಾತ್ರ_2 = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	RTLIL::ಸ್ಮರಣೆ::get_all_memorys()->ಸೇರಿಸು(std::pair<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::ಸ್ಮರಣೆ*>(hashidx_, ಇದು));
#ಆದರೆ_ಅಂತ್ಯ
}

std::string RTLIL::ಸ್ಮರಣೆ::ಗಮ್ಯ_rtlil_str() ಸ್ಥಿರಾಂಕ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_ಸ್ಮರಣೆ(f, "", ಇದು, design(), RTLIL_BACKEND::Dump_ಬಹುಲಕ::Readable);
	ಹಿಂತಿರುಗಿ f.str();
}

RTLIL::Process::Process() : module(ಶೂನ್ಯಸೂಚಕ)
{
	ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hashidx_ಎಣಿಕೆ = 123456789;
	hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
	hashidx_ = hashidx_ಎಣಿಕೆ;
}

std::string RTLIL::Process::ಗಮ್ಯ_rtlil_str() ಸ್ಥಿರಾಂಕ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", ಇದು, design(), RTLIL_BACKEND::Dump_ಬಹುಲಕ::Readable);
	ಹಿಂತಿರುಗಿ f.str();
}

RTLIL::Cell::Cell() : module(ಶೂನ್ಯಸೂಚಕ)
{
	ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hashidx_ಎಣಿಕೆ = 123456789;
	hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
	hashidx_ = hashidx_ಎಣಿಕೆ;

	// log("#memtrace# %p\n", this);
	memhasher();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->ಸೇರಿಸು(std::pair<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Cell*>(hashidx_, ಇದು));
#ಆದರೆ_ಅಂತ್ಯ
}

RTLIL::Cell::~Cell()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#ಆದರೆ_ಅಂತ್ಯ
}

std::string RTLIL::Cell::ಗಮ್ಯ_rtlil_str() ಸ್ಥಿರಾಂಕ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", ಇದು, design(), RTLIL_BACKEND::Dump_ಬಹುಲಕ::Readable);
	ಹಿಂತಿರುಗಿ f.str();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
ಸ್ಥಿರ std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Cell*> all_cells;
std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ &all_cells;
}
#ಆದರೆ_ಅಂತ್ಯ

ತಾರ್ಕಿಕ RTLIL::Cell::has_ಪೋರ್ಟ್(RTLIL::IdString portname) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ connections_.ಎಣಿಕೆ_2(portname) != 0;
}

// bufnorm

ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &RTLIL::Cell::get_ಪೋರ್ಟ್(RTLIL::IdString portname) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ connections_.at(portname);
}

ಸ್ಥಿರಾಂಕ dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ connections_;
}

ತಾರ್ಕಿಕ RTLIL::Cell::ತಿಳಿದ() ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (yosys_celltypes.cell_ತಿಳಿದ(type))
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಆದರೆ (module && module->design && module->design->module(type))
		ಹಿಂತಿರುಗಿ ನಿಜ;
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ RTLIL::Cell::ಒಳಹರಿವು(IdString portname) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (yosys_celltypes.cell_ತಿಳಿದ(type))
		ಹಿಂತಿರುಗಿ yosys_celltypes.cell_ಒಳಹರಿವು(type.ref(), portname);
	ಆದರೆ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : ಶೂನ್ಯಸೂಚಕ;
		ಹಿಂತಿರುಗಿ w && w->ಪೋರ್ಟ್_ಒಳಹರಿವು;
	}
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ RTLIL::Cell::output(RTLIL::IdString portname) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (yosys_celltypes.cell_ತಿಳಿದ(type))
		ಹಿಂತಿರುಗಿ yosys_celltypes.cell_output(type, portname);
	ಆದರೆ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : ಶೂನ್ಯಸೂಚಕ;
		ಹಿಂತಿರುಗಿ w && w->ಪೋರ್ಟ್_output;
	}
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

RTLIL::ಪೋರ್ಟ್_Dir RTLIL::Cell::ಪೋರ್ಟ್_dir(RTLIL::IdString portname) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (yosys_celltypes.cell_ತಿಳಿದ(type.ref()))
		ಹಿಂತಿರುಗಿ yosys_celltypes.cell_ಪೋರ್ಟ್_dir(type, portname);
	ಆದರೆ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		ಆದರೆ (m == ಶೂನ್ಯಸೂಚಕ)
			ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_Dir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		ಆದರೆ (w == ಶೂನ್ಯಸೂಚಕ)
			ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_Dir::PD_UNKNOWN;
		ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_Dir(w->ಪೋರ್ಟ್_ಒಳಹರಿವು + w->ಪೋರ್ಟ್_output * 2);
	}
	ಹಿಂತಿರುಗಿ ಪೋರ್ಟ್_Dir::PD_UNKNOWN;
}

ತಾರ್ಕಿಕ RTLIL::Cell::hasParam(RTLIL::IdString paramname) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ parameters.ಎಣಿಕೆ_2(paramname) != 0;
}

ಶೂನ್ಯ RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

ಶೂನ್ಯ RTLIL::Cell::ಗಣ_Param(RTLIL::IdString paramname, RTLIL::Const ಮೌಲ್ಯ)
{
	parameters[paramname] = std::move(ಮೌಲ್ಯ);
}

ಸ್ಥಿರಾಂಕ RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &it = parameters.ಕಂಡುಹಿಡಿ(paramname);
	ಆದರೆ (it != parameters.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ it->ಎರಡನೇ;
	ಆದರೆ (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		ಆದರೆ (m)
			ಹಿಂತಿರುಗಿ m->parameter_default_ಮೌಲ್ಯಗಳು.at(paramname);
	}
	ಎಸೆ std::ಹೊರಹೋಗು_ದ_range("Cell::getParam()");
}

ಶೂನ್ಯ RTLIL::Cell::ವಿಂಗಡಿಸು()
{
	connections_.ವಿಂಗಡಿಸು(ವಿಂಗಡಿಸು_by_id_str(module->twines()));
	parameters.ವಿಂಗಡಿಸು(ವಿಂಗಡಿಸು_by_id_str(module->twines()));
	ಗುಣಲಕ್ಷಣಗಳು.ವಿಂಗಡಿಸು(ವಿಂಗಡಿಸು_by_id_str(module->twines()));
}

ಶೂನ್ಯ RTLIL::Cell::check()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NDEBUG
	ಆಂತರಿಕ_Cell_Checker checker(ಖಾಲಿ, ಇದು);
	checker.check();
#ಆದರೆ_ಅಂತ್ಯ
}

ಶೂನ್ಯ RTLIL::Cell::fixup_parameters(ತಾರ್ಕಿಕ ಗಣ_a_signed, ತಾರ್ಕಿಕ ಗಣ_b_signed)
{
	ಆದರೆ (type.is_ಬಹಿರಂಗ() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::ಅಗಲ_2] = Get_ಗಾತ್ರ(connections_[ID::Y]);
		ಆದರೆ (type.ಒಳ(ID($pmux), ID($bmux)))
			parameters[ID::S_ಅಗಲ] = Get_ಗಾತ್ರ(connections_[ID::S]);
		check();
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (type == ID($demux)) {
		parameters[ID::ಅಗಲ_2] = Get_ಗಾತ್ರ(connections_[ID::A]);
		parameters[ID::S_ಅಗಲ] = Get_ಗಾತ್ರ(connections_[ID::S]);
		check();
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (type == ID($lut) || type == ID($sop)) {
		parameters[ID::ಅಗಲ_2] = Get_ಗಾತ್ರ(connections_[ID::A]);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (type == ID($fa)) {
		parameters[ID::ಅಗಲ_2] = Get_ಗಾತ್ರ(connections_[ID::Y]);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (type == ID($lcu)) {
		parameters[ID::ಅಗಲ_2] = Get_ಗಾತ್ರ(connections_[ID::CO]);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (type == ID($macc_v2)) {
		parameters[ID::Y_ಅಗಲ] = Get_ಗಾತ್ರ(connections_[ID::Y]);
		ಹಿಂತಿರುಗಿ;
	}

	ತಾರ್ಕಿಕ signedness_ab = !type.ಒಳ(ID($ತುಂಡು), ID($concat), ID($macc));

	ಆದರೆ (connections_.ಎಣಿಕೆ_2(ID::A)) {
		ಆದರೆ (signedness_ab) {
			ಆದರೆ (ಗಣ_a_signed)
				parameters[ID::A_SIGNED] = ನಿಜ;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (parameters.ಎಣಿಕೆ_2(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = ಸುಳ್ಳು;
		}
		parameters[ID::A_ಅಗಲ] = Get_ಗಾತ್ರ(connections_[ID::A]);
	}

	ಆದರೆ (connections_.ಎಣಿಕೆ_2(ID::B)) {
		ಆದರೆ (signedness_ab) {
			ಆದರೆ (ಗಣ_b_signed)
				parameters[ID::B_SIGNED] = ನಿಜ;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (parameters.ಎಣಿಕೆ_2(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = ಸುಳ್ಳು;
		}
		parameters[ID::B_ಅಗಲ] = Get_ಗಾತ್ರ(connections_[ID::B]);
	}

	ಆದರೆ (connections_.ಎಣಿಕೆ_2(ID::Y) && type != ID($concat))
		parameters[ID::Y_ಅಗಲ] = Get_ಗಾತ್ರ(connections_[ID::Y]);

	ಆದರೆ (connections_.ಎಣಿಕೆ_2(ID::Q))
		parameters[ID::ಅಗಲ_2] = Get_ಗಾತ್ರ(connections_[ID::Q]);

	check();
}

ತಾರ್ಕಿಕ RTLIL::Cell::has_keep_attr() ಸ್ಥಿರಾಂಕ {
	ಹಿಂತಿರುಗಿ get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

ತಾರ್ಕಿಕ RTLIL::Cell::has_memid() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ type.ಒಳ(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

ತಾರ್ಕಿಕ RTLIL::Cell::is_mem_cell() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ type.ಒಳ(ID($mem), ID($mem_v2)) || has_memid();
}

ತಾರ್ಕಿಕ RTLIL::Cell::is_builtin_ff() ಸ್ಥಿರಾಂಕ {
	ಹಿಂತಿರುಗಿ StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್)
{
	wire = ಬಿಟ್.wire;
	ಸ್ಥಾನಾಂತರ = 0;
	ಆದರೆ (wire == ಖಾಲಿ)
		ದತ್ತಾಂಶ = {ಬಿಟ್.ದತ್ತಾಂಶ};
	ಇಲ್ಲದಿದ್ದರೆ
		ಸ್ಥಾನಾಂತರ = ಬಿಟ್.ಸ್ಥಾನಾಂತರ;
	ಅಗಲ = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::ಹೊರತೆಗೆ(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ) ಸ್ಥಿರಾಂಕ
{
	log_assert(ಸ್ಥಾನಾಂತರ >= 0);
	log_assert(ಉದ್ದಳತೆ >= 0);
	log_assert(ಸ್ಥಾನಾಂತರ + ಉದ್ದಳತೆ <= ಅಗಲ);
	RTLIL::SigChunk ret;
	ಆದರೆ (wire) {
		ret.wire = wire;
		ret.ಸ್ಥಾನಾಂತರ = ಇದು->ಸ್ಥಾನಾಂತರ + ಸ್ಥಾನಾಂತರ;
		ret.ಅಗಲ = ಉದ್ದಳತೆ;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಉದ್ದಳತೆ; i++)
			ret.ದತ್ತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(ದತ್ತಾಂಶ[ಸ್ಥಾನಾಂತರ+i]);
		ret.ಅಗಲ = ಉದ್ದಳತೆ;
	}
	ಹಿಂತಿರುಗಿ ret;
}

RTLIL::Sig_ಬಿಟ್ RTLIL::SigChunk::ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ) ಸ್ಥಿರಾಂಕ
{
	log_assert(ಸ್ಥಾನಾಂತರ >= 0);
	log_assert(ಸ್ಥಾನಾಂತರ < ಅಗಲ);
	RTLIL::Sig_ಬಿಟ್ ret;
	ಆದರೆ (wire) {
		ret.wire = wire;
		ret.ಸ್ಥಾನಾಂತರ = ಇದು->ಸ್ಥಾನಾಂತರ + ಸ್ಥಾನಾಂತರ;
	} ಇಲ್ಲದಿದ್ದರೆ {
		ret.ದತ್ತಾಂಶ = ದತ್ತಾಂಶ[ಸ್ಥಾನಾಂತರ];
	}
	ಹಿಂತಿರುಗಿ ret;
}

ತಾರ್ಕಿಕ RTLIL::SigChunk::ಕ್ರಿಯಾಕಾರಕ <(ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &other) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (wire && other.wire)
		ಆದರೆ (wire->name != other.wire->name)
			ಹಿಂತಿರುಗಿ wire->name < other.wire->name;

	ಆದರೆ (wire != other.wire)
		ಹಿಂತಿರುಗಿ wire < other.wire;

	ಆದರೆ (ಸ್ಥಾನಾಂತರ != other.ಸ್ಥಾನಾಂತರ)
		ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ < other.ಸ್ಥಾನಾಂತರ;

	ಆದರೆ (ಅಗಲ != other.ಅಗಲ)
		ಹಿಂತಿರುಗಿ ಅಗಲ < other.ಅಗಲ;

	ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ < other.ದತ್ತಾಂಶ;
}

ತಾರ್ಕಿಕ RTLIL::SigChunk::ಕ್ರಿಯಾಕಾರಕ ==(ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &other) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ wire == other.wire && ಅಗಲ == other.ಅಗಲ && ಸ್ಥಾನಾಂತರ == other.ಸ್ಥಾನಾಂತರ && ದತ್ತಾಂಶ == other.ದತ್ತಾಂಶ;
}

ತಾರ್ಕಿಕ RTLIL::SigChunk::ಕ್ರಿಯಾಕಾರಕ !=(ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &other) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (*ಇದು == other)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	log_assert(parts.ಗಾತ್ರ_2() > 0);
	ಸ್ವಯಂಚಾಲಿತ ie = parts.begin();
	ಸ್ವಯಂಚಾಲಿತ it = ie + parts.ಗಾತ್ರ_2() - 1;
	ಆಗ (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ಸ್ಥಿರಾಂಕ RTLIL::Const &ಮೌಲ್ಯ)
{
	ಆದರೆ (Get_ಗಾತ್ರ(ಮೌಲ್ಯ) != 0) {
		rep_ = CHUNK;
		ಹೊಸ (&chunk_) RTLIL::SigChunk(ಮೌಲ್ಯ);
	} ಇಲ್ಲದಿದ್ದರೆ {
		init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&ಮೌಲ್ಯ)
{
	ಆದರೆ (Get_ಗಾತ್ರ(ಮೌಲ್ಯ) != 0) {
		rep_ = CHUNK;
		ಹೊಸ (&chunk_) RTLIL::SigChunk(ಮೌಲ್ಯ);
	} ಇಲ್ಲದಿದ್ದರೆ {
		init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &chunk)
{
	ಆದರೆ (chunk.ಅಗಲ != 0) {
		rep_ = CHUNK;
		ಹೊಸ (&chunk_) RTLIL::SigChunk(chunk);
	} ಇಲ್ಲದಿದ್ದರೆ {
		init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	ಆದರೆ (chunk.ಅಗಲ != 0) {
		rep_ = CHUNK;
		ಹೊಸ (&chunk_) RTLIL::SigChunk(chunk);
	} ಇಲ್ಲದಿದ್ದರೆ {
		init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	ಆದರೆ (wire->ಅಗಲ != 0) {
		rep_ = CHUNK;
		ಹೊಸ (&chunk_) RTLIL::SigChunk(wire);
	} ಇಲ್ಲದಿದ್ದರೆ {
		init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
	ಆದರೆ (ಅಗಲ != 0) {
		rep_ = CHUNK;
		ಹೊಸ (&chunk_) RTLIL::SigChunk(wire, ಸ್ಥಾನಾಂತರ, ಅಗಲ);
	} ಇಲ್ಲದಿದ್ದರೆ {
		init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ಸ್ಥಿರಾಂಕ std::string &str)
{
	ಆದರೆ (str.ಗಾತ್ರ_2() != 0) {
		rep_ = CHUNK;
		ಹೊಸ (&chunk_) RTLIL::SigChunk(str);
	} ಇಲ್ಲದಿದ್ದರೆ {
		init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ಪೂರ್ಣಾಂಕ val, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
	ಆದರೆ (ಅಗಲ != 0) {
		rep_ = CHUNK;
		ಹೊಸ (&chunk_) RTLIL::SigChunk(val, ಅಗಲ);
	} ಇಲ್ಲದಿದ್ದರೆ
		init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State ಬಿಟ್, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
	ಆದರೆ (ಅಗಲ != 0) {
		rep_ = CHUNK;
		ಹೊಸ (&chunk_) RTLIL::SigChunk(ಬಿಟ್, ಅಗಲ);
	} ಇಲ್ಲದಿದ್ದರೆ
		init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	check();
}

RTLIL::SigSpec::SigSpec(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್, ಪೂರ್ಣಾಂಕ ಅಗಲ)
{
	ಆದರೆ (ಅಗಲ != 0) {
		ಆದರೆ (ಬಿಟ್.wire == ಖಾಲಿ) {
			rep_ = CHUNK;
			ಹೊಸ (&chunk_) RTLIL::SigChunk(ಬಿಟ್.ದತ್ತಾಂಶ, ಅಗಲ);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಗಲ == 1) {
			rep_ = CHUNK;
			ಹೊಸ (&chunk_) RTLIL::SigChunk(ಬಿಟ್);
		} ಇಲ್ಲದಿದ್ದರೆ {
			init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
			ಬಿಟ್ಟುಗಳು_2.reserve(ಅಗಲ);
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++)
				ಬಿಟ್ಟುಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್);
		}
	} ಇಲ್ಲದಿದ್ದರೆ
		init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	check();
}

RTLIL::SigSpec::SigSpec(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<RTLIL::SigChunk> &chunks)
{
	init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> &ಬಿಟ್ಟುಗಳು)
{
	init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : ಬಿಟ್ಟುಗಳು)
		append(ಬಿಟ್);
	check();
}

RTLIL::SigSpec::SigSpec(ಸ್ಥಿರಾಂಕ pool<RTLIL::Sig_ಬಿಟ್> &ಬಿಟ್ಟುಗಳು)
{
	init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : ಬಿಟ್ಟುಗಳು)
		append(ಬಿಟ್);
	check();
}

RTLIL::SigSpec::SigSpec(ಸ್ಥಿರಾಂಕ std::ಗಣ<RTLIL::Sig_ಬಿಟ್> &ಬಿಟ್ಟುಗಳು)
{
	init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : ಬಿಟ್ಟುಗಳು)
		append(ಬಿಟ್);
	check();
}

RTLIL::SigSpec::SigSpec(ತಾರ್ಕಿಕ ಬಿಟ್)
{
	rep_ = CHUNK;
	ಹೊಸ (&chunk_) RTLIL::SigChunk(ಬಿಟ್ ? RTLIL::S1 : RTLIL::S0);
	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::Chunks::const_iterator::ಮುಂದಿನ_chunk_ಬಿಟ್ಟುಗಳು()
{
	ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು_ಗಾತ್ರ = Get_ಗಾತ್ರ(spec.ಬಿಟ್ಟುಗಳು_2);
	ಆದರೆ (ಬಿಟ್_ಸೂಚ್ಯಂಕ >= ಬಿಟ್ಟುಗಳು_ಗಾತ್ರ)
		ಹಿಂತಿರುಗಿ;
	ಪೂರ್ಣಾಂಕ i = ಬಿಟ್_ಸೂಚ್ಯಂಕ;
	ಸ್ಥಿರಾಂಕ Sig_ಬಿಟ್ &ಬಿಟ್ = spec.ಬಿಟ್ಟುಗಳು_2[i++];
	chunk.wire = ಬಿಟ್.wire;
	chunk.ದತ್ತಾಂಶ.clear();
	ಆದರೆ (ಬಿಟ್.is_wire()) {
		chunk.ಸ್ಥಾನಾಂತರ = ಬಿಟ್.ಸ್ಥಾನಾಂತರ;
		ಆಗ (i < ಬಿಟ್ಟುಗಳು_ಗಾತ್ರ && spec.ಬಿಟ್ಟುಗಳು_2[i].wire == ಬಿಟ್.wire &&
			spec.ಬಿಟ್ಟುಗಳು_2[i].ಸ್ಥಾನಾಂತರ == ಬಿಟ್.ಸ್ಥಾನಾಂತರ + i - ಬಿಟ್_ಸೂಚ್ಯಂಕ)
			++i;
	} ಇಲ್ಲದಿದ್ದರೆ {
		chunk.ಸ್ಥಾನಾಂತರ = 0;
		chunk.ದತ್ತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್.ದತ್ತಾಂಶ);
		ಆಗ (i < ಬಿಟ್ಟುಗಳು_ಗಾತ್ರ && !spec.ಬಿಟ್ಟುಗಳು_2[i].is_wire()) {
			chunk.ದತ್ತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(spec.ಬಿಟ್ಟುಗಳು_2[i].ದತ್ತಾಂಶ);
			++i;
		}
	}
	chunk.ಅಗಲ = i - ಬಿಟ್_ಸೂಚ್ಯಂಕ;
}

ಶೂನ್ಯ RTLIL::SigSpec::unpack()
{
	ಆದರೆ (rep_ == ಬಿಟ್ಟುಗಳು_3)
		ಹಿಂತಿರುಗಿ;

	std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು;
	ಬಿಟ್ಟುಗಳು.reserve(chunk_.ಅಗಲ);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < chunk_.ಅಗಲ; i++)
		ಬಿಟ್ಟುಗಳು.emplace_ಹಿಂಭಾಗ(chunk_, i);

	chunk_.~SigChunk();
	rep_ = ಬಿಟ್ಟುಗಳು_3;
	ಹೊಸ (&ಬಿಟ್ಟುಗಳು_2) std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್>(std::move(ಬಿಟ್ಟುಗಳು));
}

ಶೂನ್ಯ RTLIL::SigSpec::try_repack()
{
	ಆದರೆ (rep_ != ಬಿಟ್ಟುಗಳು_3)
		ಹಿಂತಿರುಗಿ;

	ಪೂರ್ಣಾಂಕ ಬಿಟ್ಟುಗಳು_ಗಾತ್ರ = Get_ಗಾತ್ರ(ಬಿಟ್ಟುಗಳು_2);
	ಆದರೆ (ಬಿಟ್ಟುಗಳು_ಗಾತ್ರ == 0)
		ಹಿಂತಿರುಗಿ;
	ಆದರೆ (ಬಿಟ್ಟುಗಳು_2[0].is_wire()) {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < ಬಿಟ್ಟುಗಳು_ಗಾತ್ರ; i++)
			ಆದರೆ (ಬಿಟ್ಟುಗಳು_2[0].wire != ಬಿಟ್ಟುಗಳು_2[i].wire || ಬಿಟ್ಟುಗಳು_2[0].ಸ್ಥಾನಾಂತರ + i != ಬಿಟ್ಟುಗಳು_2[i].ಸ್ಥಾನಾಂತರ)
				ಹಿಂತಿರುಗಿ;
		SigChunk chunk(ಬಿಟ್ಟುಗಳು_2[0].wire, ಬಿಟ್ಟುಗಳು_2[0].ಸ್ಥಾನಾಂತರ, ಬಿಟ್ಟುಗಳು_ಗಾತ್ರ);
		ಬಿಟ್ಟುಗಳು_2.~ಸದಿಶ();
		rep_ = CHUNK;
		ಹೊಸ (&chunk_) SigChunk(std::move(chunk));
		ಹಿಂತಿರುಗಿ;
	}
	std::ಸದಿಶ<RTLIL::State> ಬಿಟ್ಟುಗಳು;
	ಬಿಟ್ಟುಗಳು.reserve(ಬಿಟ್ಟುಗಳು_ಗಾತ್ರ);
	ಬಿಟ್ಟುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್ಟುಗಳು_2[0].ದತ್ತಾಂಶ);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < ಬಿಟ್ಟುಗಳು_ಗಾತ್ರ; i++) {
		ಆದರೆ (ಬಿಟ್ಟುಗಳು_2[i].is_wire())
			ಹಿಂತಿರುಗಿ;
		ಬಿಟ್ಟುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್ಟುಗಳು_2[i].ದತ್ತಾಂಶ);
	}
	ಬಿಟ್ಟುಗಳು_2.~ಸದಿಶ();
	rep_ = CHUNK;
	ಹೊಸ (&chunk_) SigChunk(std::move(ಬಿಟ್ಟುಗಳು));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ಸ್ಥಿರಾಂಕ
{
	Hasher h;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &c : chunks())
		ಆದರೆ (c.wire == ಖಾಲಿ) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &v : c.ದತ್ತಾಂಶ)
				h.eat(v);
		} ಇಲ್ಲದಿದ್ದರೆ {
			h.eat(c.wire->name.ref());
			h.eat(c.ಸ್ಥಾನಾಂತರ);
			h.eat(c.ಅಗಲ);
		}
	Hasher::hash_t ಫಲಿತಾಂಶ = h.yield();
	ಆದರೆ (ಫಲಿತಾಂಶ == 0)
		ಫಲಿತಾಂಶ = 1;
	hash_.ಗಣ(ಫಲಿತಾಂಶ);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಶೂನ್ಯ RTLIL::SigSpec::ವಿಂಗಡಿಸು()
{
	unpack();
	std::ವಿಂಗಡಿಸು(ಬಿಟ್ಟುಗಳು_2.begin(), ಬಿಟ್ಟುಗಳು_2.ಕೊನೆ());
	hash_.clear();
	try_repack();
}

ಶೂನ್ಯ RTLIL::SigSpec::ವಿಂಗಡಿಸು_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::ಸದಿಶ<Sig_ಬಿಟ್> unique_ಬಿಟ್ಟುಗಳು = ಬಿಟ್ಟುಗಳು_2;
	std::ವಿಂಗಡಿಸು(unique_ಬಿಟ್ಟುಗಳು.begin(), unique_ಬಿಟ್ಟುಗಳು.ಕೊನೆ());
	ಸ್ವಯಂಚಾಲಿತ ಕೊನೆಯ = std::unique(unique_ಬಿಟ್ಟುಗಳು.begin(), unique_ಬಿಟ್ಟುಗಳು.ಕೊನೆ());
	unique_ಬಿಟ್ಟುಗಳು.erase(ಕೊನೆಯ, unique_ಬಿಟ್ಟುಗಳು.ಕೊನೆ());

	*ಇದು = unique_ಬಿಟ್ಟುಗಳು;
	hash_.clear();
	try_repack();
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ನಮೂನೆ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &with)
{
	replace(ನಮೂನೆ, with, ಇದು);
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ನಮೂನೆ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &with, RTLIL::SigSpec *other) ಸ್ಥಿರಾಂಕ
{
	log_assert(other != ಖಾಲಿ);
	log_assert(ಗಾತ್ರ_2() == other->ಗಾತ್ರ_2());
	log_assert(ನಮೂನೆ.ಗಾತ್ರ_2() == with.ಗಾತ್ರ_2());

	dict<RTLIL::Sig_ಬಿಟ್, ಪೂರ್ಣಾಂಕ> ನಮೂನೆ_ಗಮ್ಯ_with;
	ಪೂರ್ಣಾಂಕ ನಮೂನೆ_ಗಾತ್ರ = ನಮೂನೆ.ಗಾತ್ರ_2();
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ನಮೂನೆ_ಗಾತ್ರ; i++) {
		Sig_ಬಿಟ್ ನಮೂನೆ_ಬಿಟ್ = ನಮೂನೆ[i];
		ಆದರೆ (ನಮೂನೆ_ಬಿಟ್.wire != ಖಾಲಿ) {
			ನಮೂನೆ_ಗಮ್ಯ_with.emplace(ನಮೂನೆ_ಬಿಟ್, i);
		}
	}

	ಪೂರ್ಣಾಂಕ this_ಗಾತ್ರ = ಗಾತ್ರ_2();
	ತಾರ್ಕಿಕ other_modified = ಸುಳ್ಳು;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < this_ಗಾತ್ರ; j++) {
		ಸ್ವಯಂಚಾಲಿತ it = ನಮೂನೆ_ಗಮ್ಯ_with.ಕಂಡುಹಿಡಿ((*ಇದು)[j]);
		ಆದರೆ (it != ನಮೂನೆ_ಗಮ್ಯ_with.ಕೊನೆ()) {
			other_modified = ನಿಜ;
			(*other)[j] = with[it->ಎರಡನೇ];
		}
	}

	ಆದರೆ (other_modified)
		other->try_repack();
	other->check();
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಸ್ಥಿರಾಂಕ dict<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> &ನಿಯಮಗಳು)
{
	replace(ನಿಯಮಗಳು, ಇದು);
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಸ್ಥಿರಾಂಕ dict<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> &ನಿಯಮಗಳು, RTLIL::SigSpec *other) ಸ್ಥಿರಾಂಕ
{
	log_assert(other != ಖಾಲಿ);
	log_assert(ಗಾತ್ರ_2() == other->ಗಾತ್ರ_2());

	ಆದರೆ (ನಿಯಮಗಳು.ಖಾಲಿಗಳು()) ಹಿಂತಿರುಗಿ;

	ಪೂರ್ಣಾಂಕ this_ಗಾತ್ರ = ಗಾತ್ರ_2();
	ತಾರ್ಕಿಕ other_modified = ಸುಳ್ಳು;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < this_ಗಾತ್ರ; i++) {
		ಸ್ವಯಂಚಾಲಿತ it = ನಿಯಮಗಳು.ಕಂಡುಹಿಡಿ((*ಇದು)[i]);
		ಆದರೆ (it != ನಿಯಮಗಳು.ಕೊನೆ()) {
			other_modified = ನಿಜ;
			(*other)[i] = it->ಎರಡನೇ;
		}
	}

	ಆದರೆ (other_modified)
		other->try_repack();
	other->check();
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಸ್ಥಿರಾಂಕ std::map<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> &ನಿಯಮಗಳು)
{
	replace(ನಿಯಮಗಳು, ಇದು);
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಸ್ಥಿರಾಂಕ std::map<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> &ನಿಯಮಗಳು, RTLIL::SigSpec *other) ಸ್ಥಿರಾಂಕ
{
	log_assert(other != ಖಾಲಿ);
	log_assert(ಗಾತ್ರ_2() == other->ಗಾತ್ರ_2());

	ಆದರೆ (ನಿಯಮಗಳು.ಖಾಲಿಗಳು()) ಹಿಂತಿರುಗಿ;

	ಪೂರ್ಣಾಂಕ this_ಗಾತ್ರ = ಗಾತ್ರ_2();
	ತಾರ್ಕಿಕ other_modified = ಸುಳ್ಳು;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < this_ಗಾತ್ರ; i++) {
		ಸ್ವಯಂಚಾಲಿತ it = ನಿಯಮಗಳು.ಕಂಡುಹಿಡಿ((*ಇದು)[i]);
		ಆದರೆ (it != ನಿಯಮಗಳು.ಕೊನೆ()) {
			other_modified = ನಿಜ;
			(*other)[i] = it->ಎರಡನೇ;
		}
	}

	ಆದರೆ (other_modified)
		other->try_repack();
	other->check();
}

ಶೂನ್ಯ RTLIL::SigSpec::remove(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ನಮೂನೆ)
{
	remove2(ನಮೂನೆ, ಖಾಲಿ);
}

ಶೂನ್ಯ RTLIL::SigSpec::remove(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ನಮೂನೆ, RTLIL::SigSpec *other) ಸ್ಥಿರಾಂಕ
{
	RTLIL::SigSpec tmp = *ಇದು;
	tmp.remove2(ನಮೂನೆ, other);
}

ಶೂನ್ಯ RTLIL::SigSpec::remove2(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ನಮೂನೆ, RTLIL::SigSpec *other)
{
	unpack();
	ಆದರೆ (other != ಖಾಲಿ) {
		log_assert(ಗಾತ್ರ_2() == other->ಗಾತ್ರ_2());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<Sig_ಬಿಟ್> ನಮೂನೆ_ಬಿಟ್ಟುಗಳು;
	ನಮೂನೆ_ಬಿಟ್ಟುಗಳು.reserve(ನಮೂನೆ.ಗಾತ್ರ_2());
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : ನಮೂನೆ)
		ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ)
			ನಮೂನೆ_ಬಿಟ್ಟುಗಳು.ಸೇರಿಸು(ಬಿಟ್);

	// Compact in-place to avoid O(n^2) erase operations
	ಗಾತ್ರ_t write_idx = 0;
	ಗಾಗಿ (ಗಾತ್ರ_t read_idx = 0; read_idx < ಬಿಟ್ಟುಗಳು_2.ಗಾತ್ರ_2(); read_idx++)
	{
		ಆದರೆ (!(ಬಿಟ್ಟುಗಳು_2[read_idx].wire != ಖಾಲಿ && ನಮೂನೆ_ಬಿಟ್ಟುಗಳು.ಎಣಿಕೆ_2(ಬಿಟ್ಟುಗಳು_2[read_idx]))) {
			ಆದರೆ (write_idx != read_idx) {
				ಬಿಟ್ಟುಗಳು_2[write_idx] = ಬಿಟ್ಟುಗಳು_2[read_idx];
				ಆದರೆ (other != ಖಾಲಿ)
					other->ಬಿಟ್ಟುಗಳು_2[write_idx] = other->ಬಿಟ್ಟುಗಳು_2[read_idx];
			}
			write_idx++;
		}
	}

	ತಾರ್ಕಿಕ modified = (write_idx < ಬಿಟ್ಟುಗಳು_2.ಗಾತ್ರ_2());
	ಆದರೆ (modified) {
		ಬಿಟ್ಟುಗಳು_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ಆದರೆ (other != ಖಾಲಿ && modified) {
		other->ಬಿಟ್ಟುಗಳು_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::remove(ಸ್ಥಿರಾಂಕ pool<RTLIL::Sig_ಬಿಟ್> &ನಮೂನೆ)
{
	remove2(ನಮೂನೆ, ಖಾಲಿ);
}

ಶೂನ್ಯ RTLIL::SigSpec::remove(ಸ್ಥಿರಾಂಕ pool<RTLIL::Sig_ಬಿಟ್> &ನಮೂನೆ, RTLIL::SigSpec *other) ಸ್ಥಿರಾಂಕ
{
	RTLIL::SigSpec tmp = *ಇದು;
	tmp.remove2(ನಮೂನೆ, other);
}

ಶೂನ್ಯ RTLIL::SigSpec::remove2(ಸ್ಥಿರಾಂಕ pool<RTLIL::Sig_ಬಿಟ್> &ನಮೂನೆ, RTLIL::SigSpec *other)
{
	unpack();

	ಆದರೆ (other != ಖಾಲಿ) {
		log_assert(ಗಾತ್ರ_2() == other->ಗಾತ್ರ_2());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	ಗಾತ್ರ_t write_idx = 0;
	ಗಾಗಿ (ಗಾತ್ರ_t read_idx = 0; read_idx < ಬಿಟ್ಟುಗಳು_2.ಗಾತ್ರ_2(); read_idx++) {
		ಆದರೆ (!(ಬಿಟ್ಟುಗಳು_2[read_idx].wire != ಖಾಲಿ && ನಮೂನೆ.ಎಣಿಕೆ_2(ಬಿಟ್ಟುಗಳು_2[read_idx]))) {
			ಆದರೆ (write_idx != read_idx) {
				ಬಿಟ್ಟುಗಳು_2[write_idx] = ಬಿಟ್ಟುಗಳು_2[read_idx];
				ಆದರೆ (other != ಖಾಲಿ)
					other->ಬಿಟ್ಟುಗಳು_2[write_idx] = other->ಬಿಟ್ಟುಗಳು_2[read_idx];
			}
			write_idx++;
		}
	}

	ತಾರ್ಕಿಕ modified = (write_idx < ಬಿಟ್ಟುಗಳು_2.ಗಾತ್ರ_2());
	ಆದರೆ (modified) {
		ಬಿಟ್ಟುಗಳು_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ಆದರೆ (other != ಖಾಲಿ && modified) {
		other->ಬಿಟ್ಟುಗಳು_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::remove2(ಸ್ಥಿರಾಂಕ std::ಗಣ<RTLIL::Sig_ಬಿಟ್> &ನಮೂನೆ, RTLIL::SigSpec *other)
{
	unpack();

	ಆದರೆ (other != ಖಾಲಿ) {
		log_assert(ಗಾತ್ರ_2() == other->ಗಾತ್ರ_2());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	ಗಾತ್ರ_t write_idx = 0;
	ಗಾಗಿ (ಗಾತ್ರ_t read_idx = 0; read_idx < ಬಿಟ್ಟುಗಳು_2.ಗಾತ್ರ_2(); read_idx++) {
		ಆದರೆ (!(ಬಿಟ್ಟುಗಳು_2[read_idx].wire != ಖಾಲಿ && ನಮೂನೆ.ಎಣಿಕೆ_2(ಬಿಟ್ಟುಗಳು_2[read_idx]))) {
			ಆದರೆ (write_idx != read_idx) {
				ಬಿಟ್ಟುಗಳು_2[write_idx] = ಬಿಟ್ಟುಗಳು_2[read_idx];
				ಆದರೆ (other != ಖಾಲಿ)
					other->ಬಿಟ್ಟುಗಳು_2[write_idx] = other->ಬಿಟ್ಟುಗಳು_2[read_idx];
			}
			write_idx++;
		}
	}

	ತಾರ್ಕಿಕ modified = (write_idx < ಬಿಟ್ಟುಗಳು_2.ಗಾತ್ರ_2());
	ಆದರೆ (modified) {
		ಬಿಟ್ಟುಗಳು_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ಆದರೆ (other != ಖಾಲಿ && modified) {
		other->ಬಿಟ್ಟುಗಳು_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::remove2(ಸ್ಥಿರಾಂಕ pool<RTLIL::Wire*> &ನಮೂನೆ, RTLIL::SigSpec *other)
{
	unpack();

	ಆದರೆ (other != ಖಾಲಿ) {
		log_assert(ಗಾತ್ರ_2() == other->ಗಾತ್ರ_2());
		other->unpack();
	}

	ತಾರ್ಕಿಕ modified = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ other_modified = ಸುಳ್ಳು;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = Get_ಗಾತ್ರ(ಬಿಟ್ಟುಗಳು_2) - 1; i >= 0; i--) {
		ಆದರೆ (ಬಿಟ್ಟುಗಳು_2[i].wire != ಖಾಲಿ && ನಮೂನೆ.ಎಣಿಕೆ_2(ಬಿಟ್ಟುಗಳು_2[i].wire)) {
			modified = ನಿಜ;
			ಬಿಟ್ಟುಗಳು_2.erase(ಬಿಟ್ಟುಗಳು_2.begin() + i);
			ಆದರೆ (other != ಖಾಲಿ) {
				other_modified = ನಿಜ;
				other->ಬಿಟ್ಟುಗಳು_2.erase(other->ಬಿಟ್ಟುಗಳು_2.begin() + i);
			}
		}
	}

	ಆದರೆ (modified) {
		hash_.clear();
		try_repack();
	}
	ಆದರೆ (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::ಹೊರತೆಗೆ(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ನಮೂನೆ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec *other) ಸ್ಥಿರಾಂಕ
{
	log_assert(other == ಖಾಲಿ || ಗಾತ್ರ_2() == other->ಗಾತ್ರ_2());

	RTLIL::SigSpec ret;
	std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು_ಹೊಂದಿಕೆ = ಗಮ್ಯ_sigbit_ಸದಿಶ();

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ& ನಮೂನೆ_chunk : ನಮೂನೆ.chunks()) {
		ಆದರೆ (other) {
			std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು_other = other->ಗಮ್ಯ_sigbit_ಸದಿಶ();
			ಪೂರ್ಣಾಂಕ i = 0;
			ಗಾಗಿ (ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್ : ಬಿಟ್ಟುಗಳು_ಹೊಂದಿಕೆ) {
				ಆದರೆ (ಬಿಟ್.wire &&
					ಬಿಟ್.wire == ನಮೂನೆ_chunk.wire &&
					ಬಿಟ್.ಸ್ಥಾನಾಂತರ >= ನಮೂನೆ_chunk.ಸ್ಥಾನಾಂತರ &&
					ಬಿಟ್.ಸ್ಥಾನಾಂತರ < ನಮೂನೆ_chunk.ಸ್ಥಾನಾಂತರ + ನಮೂನೆ_chunk.ಅಗಲ)
					ret.append(ಬಿಟ್ಟುಗಳು_other[i]);
				++i;
			}
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಗಾಗಿ (ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್ : ಬಿಟ್ಟುಗಳು_ಹೊಂದಿಕೆ)
				ಆದರೆ (ಬಿಟ್.wire &&
					ಬಿಟ್.wire == ನಮೂನೆ_chunk.wire &&
					ಬಿಟ್.ಸ್ಥಾನಾಂತರ >= ನಮೂನೆ_chunk.ಸ್ಥಾನಾಂತರ &&
					ಬಿಟ್.ಸ್ಥಾನಾಂತರ < ನಮೂನೆ_chunk.ಸ್ಥಾನಾಂತರ + ನಮೂನೆ_chunk.ಅಗಲ)
					ret.append(ಬಿಟ್);
		}
	}

	ret.check();
	ಹಿಂತಿರುಗಿ ret;
}

RTLIL::SigSpec RTLIL::SigSpec::ಹೊರತೆಗೆ(ಸ್ಥಿರಾಂಕ pool<RTLIL::Sig_ಬಿಟ್> &ನಮೂನೆ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec *other) ಸ್ಥಿರಾಂಕ
{
	log_assert(other == ಖಾಲಿ || ಗಾತ್ರ_2() == other->ಗಾತ್ರ_2());

	std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು_ಹೊಂದಿಕೆ = ಗಮ್ಯ_sigbit_ಸದಿಶ();
	RTLIL::SigSpec ret;

	ಆದರೆ (other) {
		std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು_other = other->ಗಮ್ಯ_sigbit_ಸದಿಶ();
		ಪೂರ್ಣಾಂಕ i = 0;
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್ : ಬಿಟ್ಟುಗಳು_ಹೊಂದಿಕೆ) {
			ಆದರೆ (ಬಿಟ್.wire && ನಮೂನೆ.ಎಣಿಕೆ_2(ಬಿಟ್))
				ret.append(ಬಿಟ್ಟುಗಳು_other[i]);
			++i;
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್ : ಬಿಟ್ಟುಗಳು_ಹೊಂದಿಕೆ)
			ಆದರೆ (ಬಿಟ್.wire && ನಮೂನೆ.ಎಣಿಕೆ_2(ಬಿಟ್))
				ret.append(ಬಿಟ್);
	}

	ret.check();
	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &with)
{
	ಆದರೆ (with.ಗಾತ್ರ_2() == 0)
		ಹಿಂತಿರುಗಿ;

	unpack();

	log_assert(ಸ್ಥಾನಾಂತರ >= 0);
	log_assert(with.ಗಾತ್ರ_2() >= 0);
	log_assert(ಸ್ಥಾನಾಂತರ+with.ಗಾತ್ರ_2() <= ಗಾತ್ರ_2());

	ಪೂರ್ಣಾಂಕ i = 0;
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್ : with.ಬಿಟ್ಟುಗಳು()) {
		ಬಿಟ್ಟುಗಳು_2.at(ಸ್ಥಾನಾಂತರ + i) = ಬಿಟ್;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::remove_const()
{
	ಆದರೆ (rep_ == CHUNK)
	{
		ಆದರೆ (chunk_.wire == ಖಾಲಿ) {
			chunk_.~SigChunk();
			init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು();
			hash_.clear();
		}
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
		std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> new_ಬಿಟ್ಟುಗಳು;
		new_ಬಿಟ್ಟುಗಳು.reserve(ಬಿಟ್ಟುಗಳು_2.ಗಾತ್ರ_2());
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : ಬಿಟ್ಟುಗಳು_2)
			ಆದರೆ (ಬಿಟ್.wire != ಖಾಲಿ)
				new_ಬಿಟ್ಟುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್);
		ಆದರೆ (Get_ಗಾತ್ರ(new_ಬಿಟ್ಟುಗಳು) != Get_ಗಾತ್ರ(ಬಿಟ್ಟುಗಳು_2)) {
			ಬಿಟ್ಟುಗಳು_2.ಅದಲುಬದಲು(new_ಬಿಟ್ಟುಗಳು);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::remove(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ)
{
	ಆದರೆ (ಉದ್ದಳತೆ == 0)
		ಹಿಂತಿರುಗಿ;

	unpack();

	log_assert(ಸ್ಥಾನಾಂತರ >= 0);
	log_assert(ಉದ್ದಳತೆ >= 0);
	log_assert(ಸ್ಥಾನಾಂತರ + ಉದ್ದಳತೆ <= ಗಾತ್ರ_2());

	ಬಿಟ್ಟುಗಳು_2.erase(ಬಿಟ್ಟುಗಳು_2.begin() + ಸ್ಥಾನಾಂತರ, ಬಿಟ್ಟುಗಳು_2.begin() + ಸ್ಥಾನಾಂತರ + ಉದ್ದಳತೆ);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::ಹೊರತೆಗೆ(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ) ಸ್ಥಿರಾಂಕ
{
	log_assert(ಸ್ಥಾನಾಂತರ >= 0);
	log_assert(ಉದ್ದಳತೆ >= 0);
	log_assert(ಸ್ಥಾನಾಂತರ + ಉದ್ದಳತೆ <= ಗಾತ್ರ_2());

	std::ಸದಿಶ<Sig_ಬಿಟ್> extracted;
	Sig_ಬಿಟ್ ಮೊದಲ;
	ತಾರ್ಕಿಕ is_packing = ನಿಜ;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = ಸ್ಥಾನಾಂತರ; i < ಸ್ಥಾನಾಂತರ + ಉದ್ದಳತೆ; i++) {
		ತಾರ್ಕಿಕ was_packing_before = is_packing;
		Sig_ಬಿಟ್ ಬಿಟ್ = (*ಇದು)[i];
		ಆದರೆ (i == ಸ್ಥಾನಾಂತರ) {
			ಮೊದಲ = ಬಿಟ್;
			ಆದರೆ (!ಬಿಟ್.wire)
				is_packing = ಸುಳ್ಳು;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (ಬಿಟ್.wire != ಮೊದಲ.wire)
				is_packing = ಸುಳ್ಳು;
			ಆದರೆ (ಬಿಟ್.wire)
				ಆದರೆ (ಬಿಟ್.ಸ್ಥಾನಾಂತರ != ಮೊದಲ.ಸ್ಥಾನಾಂತರ + (i - ಸ್ಥಾನಾಂತರ))
					is_packing = ಸುಳ್ಳು;
		}
		ಆದರೆ (was_packing_before && !is_packing)
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = ಸ್ಥಾನಾಂತರ; j < i; j++)
				extracted.ತಳ್ಳು_ಹಿಂಭಾಗ((*ಇದು)[j]);
		ಆದರೆ (!is_packing)
			extracted.ತಳ್ಳು_ಹಿಂಭಾಗ((*ಇದು)[i]);
	}
	ಆದರೆ (is_packing)
		ಹಿಂತಿರುಗಿ SigChunk(ಮೊದಲ.wire, ಮೊದಲ.ಸ್ಥಾನಾಂತರ, ಉದ್ದಳತೆ);

	ಹಿಂತಿರುಗಿ extracted;
}

ಶೂನ್ಯ RTLIL::SigSpec::rewrite_wires(std::function<ಶೂನ್ಯ(RTLIL::Wire*& wire)> rewrite)
{
	ಆದರೆ (rep_ == CHUNK) {
		ಆದರೆ (chunk_.wire != ಶೂನ್ಯಸೂಚಕ)
			rewrite(chunk_.wire);
		ಹಿಂತಿರುಗಿ;
	}

	std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> new_ಬಿಟ್ಟುಗಳು;
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		ಆದರೆ (c.wire != ಶೂನ್ಯಸೂಚಕ)
			rewrite(c.wire);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < c.ಅಗಲ; i++)
			new_ಬಿಟ್ಟುಗಳು.emplace_ಹಿಂಭಾಗ(c, i);
	}
	ಬಿಟ್ಟುಗಳು_2 = std::move(new_ಬಿಟ್ಟುಗಳು);
	hash_.clear();
}

ಶೂನ್ಯ RTLIL::SigSpec::append(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ಸಂಕೇತ)
{
	ಆದರೆ (ಸಂಕೇತ.ಗಾತ್ರ_2() == 0)
		ಹಿಂತಿರುಗಿ;

	ಆದರೆ (ಗಾತ್ರ_2() == 0) {
		*ಇದು = ಸಂಕೇತ;
		ಹಿಂತಿರುಗಿ;
	}

	hash_.clear();
	ಆದರೆ (rep_ == CHUNK && ಸಂಕೇತ.rep_ == CHUNK && chunk_.wire == ಸಂಕೇತ.chunk_.wire) {
		ಆದರೆ (chunk_.wire == ಖಾಲಿ) {
			chunk_.ದತ್ತಾಂಶ.ಸೇರಿಸು(chunk_.ದತ್ತಾಂಶ.ಕೊನೆ(), ಸಂಕೇತ.chunk_.ದತ್ತಾಂಶ.begin(), ಸಂಕೇತ.chunk_.ದತ್ತಾಂಶ.ಕೊನೆ());
			chunk_.ಅಗಲ = Get_ಗಾತ್ರ(chunk_.ದತ್ತಾಂಶ);
			ಹಿಂತಿರುಗಿ;
		}
		ಆದರೆ (chunk_.ಸ್ಥಾನಾಂತರ + chunk_.ಅಗಲ == ಸಂಕೇತ.chunk_.ಸ್ಥಾನಾಂತರ) {
			chunk_.ಅಗಲ += ಸಂಕೇತ.chunk_.ಅಗಲ;
			ಹಿಂತಿರುಗಿ;
		}
	}

	unpack();
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ Sig_ಬಿಟ್ &ಬಿಟ್ : ಸಂಕೇತ.ಬಿಟ್ಟುಗಳು())
		ಬಿಟ್ಟುಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್);
	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::append(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್)
{
	hash_.clear();

	ಆದರೆ (ಗಾತ್ರ_2() == 0) {
		destroy();
		rep_ = CHUNK;
		ಹೊಸ (&chunk_) RTLIL::SigChunk(ಬಿಟ್);
		ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (rep_ == CHUNK && chunk_.wire == ಬಿಟ್.wire) {
		ಆದರೆ (chunk_.wire == ಖಾಲಿ) {
			chunk_.ದತ್ತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್.ದತ್ತಾಂಶ);
			chunk_.ಅಗಲ++;
			ಹಿಂತಿರುಗಿ;
		}
		ಆದರೆ (chunk_.ಸ್ಥಾನಾಂತರ + chunk_.ಅಗಲ == ಬಿಟ್.ಸ್ಥಾನಾಂತರ) {
			chunk_.ಅಗಲ++;
			ಹಿಂತಿರುಗಿ;
		}
	}

	unpack();

	ಬಿಟ್ಟುಗಳು_2.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್);
	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::extend_u0(ಪೂರ್ಣಾಂಕ ಅಗಲ, ತಾರ್ಕಿಕ is_signed)
{
	ಆದರೆ (ಗಾತ್ರ_2() > ಅಗಲ)
		remove(ಅಗಲ, ಗಾತ್ರ_2() - ಅಗಲ);

	ಆದರೆ (ಗಾತ್ರ_2() < ಅಗಲ) {
		RTLIL::Sig_ಬಿಟ್ padding = ಗಾತ್ರ_2() > 0 ? (*ಇದು)[ಗಾತ್ರ_2() - 1] : RTLIL::State::Sx;
		ಆದರೆ (!is_signed)
			padding = RTLIL::State::S0;
		ಆಗ (ಗಾತ್ರ_2() < ಅಗಲ)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(ಪೂರ್ಣಾಂಕ ಅಂಕೆ) ಸ್ಥಿರಾಂಕ
{
	RTLIL::SigSpec sig;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ; i++)
		sig.append(*ಇದು);
	ಹಿಂತಿರುಗಿ sig;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NDEBUG
ಶೂನ್ಯ RTLIL::SigSpec::check(ಸ್ಥಿರಾಂಕ Module *mod) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (rep_ == CHUNK)
	{
		log_assert(chunk_.ಅಗಲ != 0);
		ಆದರೆ (chunk_.wire == ಖಾಲಿ) {
			log_assert(chunk_.ಸ್ಥಾನಾಂತರ == 0);
			log_assert(chunk_.ದತ್ತಾಂಶ.ಗಾತ್ರ_2() == (ಗಾತ್ರ_t)chunk_.ಅಗಲ);
		} ಇಲ್ಲದಿದ್ದರೆ {
			log_assert(chunk_.ಸ್ಥಾನಾಂತರ >= 0);
			log_assert(chunk_.ಅಗಲ >= 0);
			log_assert(chunk_.ಸ್ಥಾನಾಂತರ + chunk_.ಅಗಲ <= chunk_.wire->ಅಗಲ);
			log_assert(chunk_.ದತ್ತಾಂಶ.ಗಾತ್ರ_2() == 0);
			ಆದರೆ (mod != ಶೂನ್ಯಸೂಚಕ)
				log_assert(chunk_.wire->module == mod);
		}
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಗಾತ್ರ_2() <= 64)
	{
		ಆದರೆ (mod != ಶೂನ್ಯಸೂಚಕ) {
			ಗಾಗಿ (ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್ : ಬಿಟ್ಟುಗಳು_2)
				ಆದರೆ (ಬಿಟ್.wire != ಶೂನ್ಯಸೂಚಕ)
					log_assert(ಬಿಟ್.wire->module == mod);
		}
	}
}
#ಆದರೆ_ಅಂತ್ಯ

ತಾರ್ಕಿಕ RTLIL::SigSpec::ಕ್ರಿಯಾಕಾರಕ <(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &other) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಇದು == &other)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಆದರೆ (ಗಾತ್ರ_2() != other.ಗಾತ್ರ_2())
		ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2() < other.ಗಾತ್ರ_2();

	ಸ್ವಯಂಚಾಲಿತ other_it = other.chunks().begin();
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ SigChunk &c : chunks()) {
		ಆದರೆ (c != *other_it)
			ಹಿಂತಿರುಗಿ c < *other_it;
		++other_it;
	}

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::ಕ್ರಿಯಾಕಾರಕ ==(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &other) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ಇದು == &other)
		ಹಿಂತಿರುಗಿ ನಿಜ;

	ಆದರೆ (ಗಾತ್ರ_2() != other.ಗಾತ್ರ_2())
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಸ್ವಯಂಚಾಲಿತ other_it = other.chunks().begin();
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ SigChunk &c : chunks()) {
		ಆದರೆ (c != *other_it)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		++other_it;
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_wire() ಸ್ಥಿರಾಂಕ
{
	Chunks cs = chunks();
	ಸ್ವಯಂಚಾಲಿತ it = cs.begin();
	ಆದರೆ (it == cs.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &chunk = *it;
	ಹಿಂತಿರುಗಿ chunk.wire && chunk.wire->ಅಗಲ == ಗಾತ್ರ_2() && ++it == cs.ಕೊನೆ();
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_chunk() ಸ್ಥಿರಾಂಕ
{
	Chunks cs = chunks();
	ಸ್ವಯಂಚಾಲಿತ it = cs.begin();
	ಆದರೆ (it == cs.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಹಿಂತಿರುಗಿ ++it == cs.ಕೊನೆ();
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::ತಿಳಿದ_driver() ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : chunks())
		ಆದರೆ (chunk.is_wire() && !chunk.wire->ತಿಳಿದ_driver())
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_fully_const() ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : chunks())
		ಆದರೆ (chunk.ಅಗಲ > 0 && chunk.wire != ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_fully_zero() ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : chunks()) {
		ಆದರೆ (chunk.ಅಗಲ > 0 && chunk.wire != ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಗಾಗಿ (RTLIL::State d : chunk.ದತ್ತಾಂಶ)
			ಆದರೆ (d != RTLIL::State::S0)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_fully_ones() ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : chunks()) {
		ಆದರೆ (chunk.ಅಗಲ > 0 && chunk.wire != ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಗಾಗಿ (RTLIL::State d : chunk.ದತ್ತಾಂಶ)
			ಆದರೆ (d != RTLIL::State::S1)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_fully_def() ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : chunks()) {
		ಆದರೆ (chunk.ಅಗಲ > 0 && chunk.wire != ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಗಾಗಿ (RTLIL::State d : chunk.ದತ್ತಾಂಶ)
			ಆದರೆ (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_fully_undef() ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : chunks()) {
		ಆದರೆ (chunk.ಅಗಲ > 0 && chunk.wire != ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		ಗಾಗಿ (RTLIL::State d : chunk.ದತ್ತಾಂಶ)
			ಆದರೆ (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::has_const() ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : chunks())
		ಆದರೆ (chunk.ಅಗಲ > 0 && chunk.wire == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ ನಿಜ;
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::has_const(State state) ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : chunks())
		ಆದರೆ (chunk.ಅಗಲ > 0 && chunk.wire == ಖಾಲಿ && std::ಕಂಡುಹಿಡಿ(chunk.ದತ್ತಾಂಶ.begin(), chunk.ದತ್ತಾಂಶ.ಕೊನೆ(), state) != chunk.ದತ್ತಾಂಶ.ಕೊನೆ())
			ಹಿಂತಿರುಗಿ ನಿಜ;
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}


ತಾರ್ಕಿಕ RTLIL::SigSpec::has_marked_ಬಿಟ್ಟುಗಳು() ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &chunk : chunks())
		ಆದರೆ (chunk.ಅಗಲ > 0 && chunk.wire == ಖಾಲಿ) {
			ಗಾಗಿ (RTLIL::State d : chunk.ದತ್ತಾಂಶ)
				ಆದರೆ (d == RTLIL::State::Sm)
					ಹಿಂತಿರುಗಿ ನಿಜ;
		}
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_onehot(ಪೂರ್ಣಾಂಕ *pos) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (std::optional<RTLIL::Const> c = try_as_const())
		ಹಿಂತಿರುಗಿ c->is_onehot(pos);
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::as_bool() ಸ್ಥಿರಾಂಕ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ಮೌಲ್ಯ());
	ಹಿಂತಿರುಗಿ c->as_bool();
}

ಪೂರ್ಣಾಂಕ RTLIL::SigSpec::as_int(ತಾರ್ಕಿಕ is_signed) ಸ್ಥಿರಾಂಕ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ಮೌಲ್ಯ());
	ಹಿಂತಿರುಗಿ c->as_int(is_signed);
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::convertible_ಗಮ್ಯ_int(ತಾರ್ಕಿಕ is_signed) ಸ್ಥಿರಾಂಕ
{
	std::optional<RTLIL::Const> c = try_as_const();
	ಆದರೆ (!c.has_ಮೌಲ್ಯ())
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಹಿಂತಿರುಗಿ c->convertible_ಗಮ್ಯ_int(is_signed);
}

std::optional<ಪೂರ್ಣಾಂಕ> RTLIL::SigSpec::try_as_int(ತಾರ್ಕಿಕ is_signed) ಸ್ಥಿರಾಂಕ
{
	std::optional<RTLIL::Const> c = try_as_const();
	ಆದರೆ (!c.has_ಮೌಲ್ಯ())
		ಹಿಂತಿರುಗಿ std::nullopt;
	ಹಿಂತಿರುಗಿ c->try_as_int(is_signed);
}

ಪೂರ್ಣಾಂಕ RTLIL::SigSpec::as_int_saturating(ತಾರ್ಕಿಕ is_signed) ಸ್ಥಿರಾಂಕ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ಮೌಲ್ಯ());
	ಹಿಂತಿರುಗಿ c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ಸ್ಥಿರಾಂಕ
{
	std::string str;
	str.reserve(ಗಾತ್ರ_2());
	std::ಸದಿಶ<RTLIL::SigChunk> chunks = *ಇದು;
	ಗಾಗಿ (ಗಾತ್ರ_t i = chunks.ಗಾತ್ರ_2(); i > 0; i--) {
		ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &chunk = chunks[i-1];
		ಆದರೆ (chunk.wire != ಖಾಲಿ)
			str.append(chunk.ಅಗಲ, '?');
		ಇಲ್ಲದಿದ್ದರೆ
			str += RTLIL::Const(chunk.ದತ್ತಾಂಶ).as_string();
	}
	ಹಿಂತಿರುಗಿ str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ಸ್ಥಿರಾಂಕ
{
	Chunks cs = chunks();
	ಸ್ವಯಂಚಾಲಿತ it = cs.begin();
	ಆದರೆ (it == cs.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ RTLIL::Const();
	SigChunk chunk = *it;
	ಆದರೆ (chunk.wire != ಖಾಲಿ || ++it != cs.ಕೊನೆ())
		ಹಿಂತಿರುಗಿ std::nullopt;
	ಹಿಂತಿರುಗಿ RTLIL::Const(std::move(chunk.ದತ್ತಾಂಶ));
}

RTLIL::Const RTLIL::SigSpec::as_const() ಸ್ಥಿರಾಂಕ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ಮೌಲ್ಯ());
	ಹಿಂತಿರುಗಿ *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ಸ್ಥಿರಾಂಕ
{
	Chunks cs = chunks();
	ಸ್ವಯಂಚಾಲಿತ it = cs.begin();
	log_assert(it != cs.ಕೊನೆ());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.ಕೊನೆ() && chunk.wire && chunk.wire->ಅಗಲ == ಗಾತ್ರ_2());
	ಹಿಂತಿರುಗಿ chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ಸ್ಥಿರಾಂಕ
{
	Chunks cs = chunks();
	ಸ್ವಯಂಚಾಲಿತ it = cs.begin();
	log_assert(it != cs.ಕೊನೆ());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.ಕೊನೆ());
	ಹಿಂತಿರುಗಿ chunk;
}

RTLIL::Sig_ಬಿಟ್ RTLIL::SigSpec::as_ಬಿಟ್() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ RTLIL::Sig_ಬಿಟ್(*ಇದು);
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ನಮೂನೆ) ಸ್ಥಿರಾಂಕ
{
	ಪೂರ್ಣಾಂಕ ನಮೂನೆ_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ನಮೂನೆ);
	log_assert(ನಮೂನೆ_len == ಗಾತ್ರ_2());

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ನಮೂನೆ_len; ++i) {
		ಅಕ್ಷರ ch = ನಮೂನೆ[i];
		ಆದರೆ (ch == ' ')
			ಮುಂದುವರಿ;
		RTLIL::Sig_ಬಿಟ್ ಬಿಟ್ = (*ಇದು)[ನಮೂನೆ_len - 1 - i];
		ಆದರೆ (ch == '*') {
			ಆದರೆ (ಬಿಟ್ != State::Sz && ಬಿಟ್ != State::Sx)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಮುಂದುವರಿ;
		}
		ಆದರೆ (ch == '0') {
			ಆದರೆ (ಬಿಟ್ != State::S0)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		} ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ (ch == '1') {
			ಆದರೆ (ಬಿಟ್ != State::S1)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		} ಇಲ್ಲದಿದ್ದರೆ
			log_abort();
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

std::ಗಣ<RTLIL::Sig_ಬಿಟ್> RTLIL::SigSpec::ಗಮ್ಯ_sigbit_ಗಣ() ಸ್ಥಿರಾಂಕ
{
	std::ಗಣ<RTLIL::Sig_ಬಿಟ್> sigbits;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &c : chunks())
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < c.ಅಗಲ; i++)
			sigbits.ಸೇರಿಸು(RTLIL::Sig_ಬಿಟ್(c, i));
	ಹಿಂತಿರುಗಿ sigbits;
}

pool<RTLIL::Sig_ಬಿಟ್> RTLIL::SigSpec::ಗಮ್ಯ_sigbit_pool() ಸ್ಥಿರಾಂಕ
{
	pool<RTLIL::Sig_ಬಿಟ್> sigbits;
	sigbits.reserve(ಗಾತ್ರ_2());
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &c : chunks())
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < c.ಅಗಲ; i++)
			sigbits.ಸೇರಿಸು(RTLIL::Sig_ಬಿಟ್(c, i));
	ಹಿಂತಿರುಗಿ sigbits;
}

std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> RTLIL::SigSpec::ಗಮ್ಯ_sigbit_ಸದಿಶ() ಸ್ಥಿರಾಂಕ
{
	std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಫಲಿತಾಂಶ;
	ಫಲಿತಾಂಶ.reserve(ಗಾತ್ರ_2());
	ಗಾಗಿ (Sig_ಬಿಟ್ ಬಿಟ್ : *ಇದು)
		ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

std::map<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> RTLIL::SigSpec::ಗಮ್ಯ_sigbit_map(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &other) ಸ್ಥಿರಾಂಕ
{
	ಪೂರ್ಣಾಂಕ this_ಗಾತ್ರ = ಗಾತ್ರ_2();
	log_assert(this_ಗಾತ್ರ == other.ಗಾತ್ರ_2());

	std::map<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> new_map;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < this_ಗಾತ್ರ; i++)
		new_map[(*ಇದು)[i]] = other[i];

	ಹಿಂತಿರುಗಿ new_map;
}

dict<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> RTLIL::SigSpec::ಗಮ್ಯ_sigbit_dict(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &other) ಸ್ಥಿರಾಂಕ
{
	ಪೂರ್ಣಾಂಕ this_ಗಾತ್ರ = ಗಾತ್ರ_2();
	log_assert(this_ಗಾತ್ರ == other.ಗಾತ್ರ_2());

	dict<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> new_map;
	new_map.reserve(this_ಗಾತ್ರ);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < this_ಗಾತ್ರ; i++)
		new_map[(*ಇದು)[i]] = other[i];

	ಹಿಂತಿರುಗಿ new_map;
}

ಸ್ಥಿರ ಶೂನ್ಯ sigspec_parse_ವಿಭಜಿಸು(std::ಸದಿಶ<std::string> &ಟೋಕನುಗಳು, ಸ್ಥಿರಾಂಕ std::string &ಪಠ್ಯ, ಅಕ್ಷರ sep)
{
	ಗಾತ್ರ_t ಆರಂಭ = 0, ಕೊನೆ = 0;
	ಆಗ ((ಕೊನೆ = ಪಠ್ಯ.ಕಂಡುಹಿಡಿ(sep, ಆರಂಭ)) != std::string::npos) {
		ಟೋಕನುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪಠ್ಯ.substr(ಆರಂಭ, ಕೊನೆ - ಆರಂಭ));
		ಆರಂಭ = ಕೊನೆ + 1;
	}
	ಟೋಕನುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪಠ್ಯ.substr(ಆರಂಭ));
}

ಸ್ಥಿರ RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ಸ್ಥಿರಾಂಕ std::string &netname)
{
	ಹಿಂತಿರುಗಿ module->wire(netname);
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::ಸದಿಶ<std::string> ಟೋಕನುಗಳು;
	sigspec_parse_ವಿಭಜಿಸು(ಟೋಕನುಗಳು, str, ',');

	sig = RTLIL::SigSpec();
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ tokidx = ಪೂರ್ಣಾಂಕ(ಟೋಕನುಗಳು.ಗಾತ್ರ_2())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = ಟೋಕನುಗಳು[tokidx];
		std::string indices;

		ಆದರೆ (netname.ಗಾತ್ರ_2() == 0)
			ಮುಂದುವರಿ;

		ಆದರೆ (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			ಸ್ವಯಂಚಾಲಿತ ast = p.const2ast(netname);
			ಆದರೆ (ast == ಶೂನ್ಯಸೂಚಕ)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			sig.append(RTLIL::Const(ast->ಬಿಟ್ಟುಗಳು));
			ಮುಂದುವರಿ;
		}

		ಆದರೆ (module == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

		ಆದರೆ (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		ಆದರೆ (sigspec_parse_wire(module, netname) == ಶೂನ್ಯಸೂಚಕ) {
			ಗಾತ್ರ_t indices_pos = netname.ಗಾತ್ರ_2()-1;
			ಆದರೆ (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				ಆಗ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				ಆದರೆ (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					ಆಗ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				ಆದರೆ (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		ಆದರೆ (wire == ಶೂನ್ಯಸೂಚಕ)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

		ಆದರೆ (!indices.ಖಾಲಿಗಳು()) {
			std::ಸದಿಶ<std::string> ಸೂಚ್ಯಂಕ_ಟೋಕನುಗಳು;
			sigspec_parse_ವಿಭಜಿಸು(ಸೂಚ್ಯಂಕ_ಟೋಕನುಗಳು, indices.substr(1, indices.ಗಾತ್ರ_2()-2), ':');
			ಆದರೆ (ಸೂಚ್ಯಂಕ_ಟೋಕನುಗಳು.ಗಾತ್ರ_2() == 1) {
				ಪೂರ್ಣಾಂಕ a = atoi(ಸೂಚ್ಯಂಕ_ಟೋಕನುಗಳು.at(0).c_str());
				ಆದರೆ (a < 0 || a >= wire->ಅಗಲ)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				sig.append(RTLIL::SigSpec(wire, a));
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಪೂರ್ಣಾಂಕ a = atoi(ಸೂಚ್ಯಂಕ_ಟೋಕನುಗಳು.at(0).c_str());
				ಪೂರ್ಣಾಂಕ b = atoi(ಸೂಚ್ಯಂಕ_ಟೋಕನುಗಳು.at(1).c_str());
				ಆದರೆ (a > b) {
					ಪೂರ್ಣಾಂಕ tmp = a;
					a = b, b = tmp;
				}
				ಆದರೆ (a < 0 || a >= wire->ಅಗಲ)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (b < 0 || b >= wire->ಅಗಲ)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} ಇಲ್ಲದಿದ್ದರೆ
			sig.append(wire);
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	ಆದರೆ (str.ಖಾಲಿಗಳು() || str[0] != '@')
		ಹಿಂತಿರುಗಿ parse(sig, module, str);

	IdString sel_name = design->twines().ಕಂಡುಹಿಡಿ(RTLIL::escape_id(str.substr(1)));
	ಆದರೆ (sel_name == IdString::Null || design->selection_vars.ಎಣಿಕೆ_2(sel_name) == 0)
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : module->wires_)
		ಆದರೆ (sel.selected_member(module->name, it.ಎರಡನೇ->name))
			sig.append(it.ಎರಡನೇ);

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::parse_rhs(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	ಆದರೆ (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.ಗಾತ್ರ_2());
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.ಗಾತ್ರ_2());
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (lhs.is_chunk()) {
		ಅಕ್ಷರ *p = (ಅಕ್ಷರ*)str.c_str(), *endptr;
		ದೀರ್ಘ ಪೂರ್ಣಾಂಕ val = strtol(p, &endptr, 10);
		ಆದರೆ (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.ಗಾತ್ರ_2());
			ಹಿಂತಿರುಗಿ ನಿಜ;
		}
	}

	ಆದರೆ (!parse(sig, module, str))
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಆದರೆ (sig.ಗಾತ್ರ_2() > lhs.ಗಾತ್ರ_2())
		sig.remove(lhs.ಗಾತ್ರ_2(), sig.ಗಾತ್ರ_2() - lhs.ಗಾತ್ರ_2());
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

RTLIL::SigSpec::ಕ್ರಿಯಾಕಾರಕ std::ಸದಿಶ<RTLIL::SigChunk>() ಸ್ಥಿರಾಂಕ
{
	std::ಸದಿಶ<RTLIL::SigChunk> ಫಲಿತಾಂಶ;
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &c : chunks())
		ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(c);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Case_ನಿಯಮ::~Case_ನಿಯಮ()
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = switches.begin(); it != switches.ಕೊನೆ(); it++)
		ಅಳಿಸು *it;
}

ತಾರ್ಕಿಕ RTLIL::Case_ನಿಯಮ::ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ actions.ಖಾಲಿಗಳು() && switches.ಖಾಲಿಗಳು();
}

RTLIL::Case_ನಿಯಮ *RTLIL::Case_ನಿಯಮ::clone() ಸ್ಥಿರಾಂಕ
{
	RTLIL::Case_ನಿಯಮ *new_caserule = ಹೊಸ RTLIL::Case_ನಿಯಮ;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->ಗುಣಲಕ್ಷಣಗಳು = ಗುಣಲಕ್ಷಣಗಳು;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : switches)
		new_caserule->switches.ತಳ್ಳು_ಹಿಂಭಾಗ(it->clone());
	ಹಿಂತಿರುಗಿ new_caserule;
}

RTLIL::Switch_ನಿಯಮ::~Switch_ನಿಯಮ()
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = cases.begin(); it != cases.ಕೊನೆ(); it++)
		ಅಳಿಸು *it;
}

ತಾರ್ಕಿಕ RTLIL::Switch_ನಿಯಮ::ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ cases.ಖಾಲಿಗಳು();
}

RTLIL::Switch_ನಿಯಮ *RTLIL::Switch_ನಿಯಮ::clone() ಸ್ಥಿರಾಂಕ
{
	RTLIL::Switch_ನಿಯಮ *new_switchrule = ಹೊಸ RTLIL::Switch_ನಿಯಮ;
	new_switchrule->ಸಂಕೇತ = ಸಂಕೇತ;
	new_switchrule->ಗುಣಲಕ್ಷಣಗಳು = ಗುಣಲಕ್ಷಣಗಳು;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : cases)
		new_switchrule->cases.ತಳ್ಳು_ಹಿಂಭಾಗ(it->clone());
	ಹಿಂತಿರುಗಿ new_switchrule;

}

RTLIL::Sync_ನಿಯಮ *RTLIL::Sync_ನಿಯಮ::clone() ಸ್ಥಿರಾಂಕ
{
	RTLIL::Sync_ನಿಯಮ *new_syncrule = ಹೊಸ RTLIL::Sync_ನಿಯಮ;
	new_syncrule->type = type;
	new_syncrule->ಸಂಕೇತ = ಸಂಕೇತ;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	ಹಿಂತಿರುಗಿ new_syncrule;
}

RTLIL::Process::~Process()
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = syncs.begin(); it != syncs.ಕೊನೆ(); it++)
		ಅಳಿಸು *it;
}

RTLIL::Process *RTLIL::Process::clone() ಸ್ಥಿರಾಂಕ
{
	RTLIL::Process *new_proc = ಹೊಸ RTLIL::Process;

	new_proc->ಗುಣಲಕ್ಷಣಗಳು = ಗುಣಲಕ್ಷಣಗಳು;

	RTLIL::Case_ನಿಯಮ *rc_ptr = ಬೇರು_case.clone();
	new_proc->ಬೇರು_case = *rc_ptr;
	rc_ptr->switches.clear();
	ಅಳಿಸು rc_ptr;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : syncs)
		new_proc->syncs.ತಳ್ಳು_ಹಿಂಭಾಗ(it->clone());

	ಹಿಂತಿರುಗಿ new_proc;
}

RTLIL::ಸ್ಮರಣೆ::~ಸ್ಮರಣೆ()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	RTLIL::ಸ್ಮರಣೆ::get_all_memorys()->erase(hashidx_);
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
ಸ್ಥಿರ std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::ಸ್ಮರಣೆ*> all_memorys;
std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::ಸ್ಮರಣೆ*> *RTLIL::ಸ್ಮರಣೆ::get_all_memorys(ಶೂನ್ಯ)
{
	ಹಿಂತಿರುಗಿ &all_memorys;
}
#ಆದರೆ_ಅಂತ್ಯ
YOSYS_NAMESPACE_ಕೊನೆ
