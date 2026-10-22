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

ಧ್ರುವ pool<IdString> &RTLIL::builtin_ff_cell_types() {
	ಸ್ಥಿರ ಧ್ರುವ pool<IdString> res = []() {
		pool<IdString> r;
		ಪ್ರತಿ (size_t i = 0; i < StaticCellTypes::builder.ಎಣಿಕೆ; i++) {
			auto &cell = StaticCellTypes::builder.cells[i];
			ಯದಿ (cell.features.is_ff)
				r.ಸೇರಿಸು(cell.type);
		}
		ಪ್ರತ್ಯಾಗಮ r;
	}();
	ಪ್ರತ್ಯಾಗಮ res;
}

#define check(condition) log_assert(condition && "malformed Const union")

ಧ್ರುವ Const::bitvectype& Const::get_bits() ಧ್ರುವ {
	check(is_bits());
	ಪ್ರತ್ಯಾಗಮ *get_if_bits();
}

ಧ್ರುವ std::string& Const::get_str() ಧ್ರುವ {
	check(is_str());
	ಪ್ರತ್ಯಾಗಮ *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	ಪ್ರತ್ಯಾಗಮ *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	ಪ್ರತ್ಯಾಗಮ *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	ನೂತನ ((ಶೂನ್ಯ*)&str_) std::string(std::move(str));
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	ವರ್ಣ bytes[] = {
		(ವರ್ಣ)(val >> 24), (ವರ್ಣ)(val >> 16), (ವರ್ಣ)(val >> 8), (ವರ್ಣ)val
	};
	ನೂತನ ((ಶೂನ್ಯ*)&str_) std::string(bytes, 4);
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val, ಪೂರ್ಣಾಙ್ಕ width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	ಯದಿ ((width & 7) == 0) {
		ನೂತನ ((ಶೂನ್ಯ*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		ಪೂರ್ಣಾಙ್ಕ bytes = width >> 3;
		signed ವರ್ಣ sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<ಪೂರ್ಣಾಙ್ಕ>(bytes, ಪರಿಮಾಣ(val));
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < bytes; i++) {
			str[str.size() - 1 - i] = val;
			val = val >> 8;
		}
		ಪ್ರತ್ಯಾಗಮ;
	}

	ನೂತನ ((ಶೂನ್ಯ*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++) {
		bv.ತಳ್ಳು_back((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, ಪೂರ್ಣಾಙ್ಕ width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	ನೂತನ ((ಶೂನ್ಯ*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++)
		bv.ತಳ್ಳು_back(bit);
}

RTLIL::Const::Const(ಧ್ರುವ std::vector<ತಾರ್ಕಿಕ> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	ನೂತನ ((ಶೂನ್ಯ*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.size());
	ಪ್ರತಿ (ಧ್ರುವ auto &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ಧ್ರುವ RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	ಯದಿ (is_str())
		ನೂತನ ((ಶೂನ್ಯ*)&str_) std::string(other.get_str());
	ಅನ್ಯಥಾ ಯದಿ (is_bits())
		ನೂತನ ((ಶೂನ್ಯ*)&bits_) bitvectype(other.get_bits());
	ಅನ್ಯಥಾ
		check(ಅಸತ್ಯ);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	ಯದಿ (is_str())
		ನೂತನ ((ಶೂನ್ಯ*)&str_) std::string(std::move(other.get_str()));
	ಅನ್ಯಥಾ ಯದಿ (is_bits())
		ನೂತನ ((ಶೂನ್ಯ*)&bits_) bitvectype(std::move(other.get_bits()));
	ಅನ್ಯಥಾ
		check(ಅಸತ್ಯ);
}

RTLIL::Const &RTLIL::Const::operator =(ಧ್ರುವ RTLIL::Const &other) {
	flags = other.flags;
	ಯದಿ (other.is_str()) {
		ಯದಿ (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(ಶೂನ್ಯ)ನೂತನ ((ಶೂನ್ಯ*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} ಅನ್ಯಥಾ ಯದಿ (other.is_bits()) {
		ಯದಿ (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(ಶೂನ್ಯ)ನೂತನ ((ಶೂನ್ಯ*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} ಅನ್ಯಥಾ {
		check(ಅಸತ್ಯ);
	}
	ಪ್ರತ್ಯಾಗಮ *ಏತತ್;
}

RTLIL::Const::~Const() {
	ಯದಿ (is_bits())
		bits_.~bitvectype();
	ಅನ್ಯಥಾ ಯದಿ (is_str())
		str_.~string();
	ಅನ್ಯಥಾ
		check(ಅಸತ್ಯ);
}

ತಾರ್ಕಿಕ RTLIL::Const::operator<(ಧ್ರುವ RTLIL::Const &other) ಧ್ರುವ
{
	ಯದಿ (size() != other.size())
		ಪ್ರತ್ಯಾಗಮ size() < other.size();

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < size(); i++)
		ಯದಿ ((*ಏತತ್)[i] != other[i])
			ಪ್ರತ್ಯಾಗಮ (*ಏತತ್)[i] < other[i];

	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::Const::operator ==(ಧ್ರುವ RTLIL::Const &other) ಧ್ರುವ
{
	ಯದಿ (is_str() && other.is_str())
		ಪ್ರತ್ಯಾಗಮ get_str() == other.get_str();
	ಯದಿ (is_bits() && other.is_bits())
		ಪ್ರತ್ಯಾಗಮ get_bits() == other.get_bits();

	ಯದಿ (size() != other.size())
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < size(); i++)
	ಯದಿ ((*ಏತತ್)[i] != other[i])
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::Const::operator !=(ಧ್ರುವ RTLIL::Const &other) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ !(*ಏತತ್ == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	ಪ್ರತ್ಯಾಗಮ get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() ಧ್ರುವ
{
	std::vector<State> v;
	ಪ್ರತಿ (auto bit : *ಏತತ್)
		v.ತಳ್ಳು_back(bit);
	ಪ್ರತ್ಯಾಗಮ v;
}

ತಾರ್ಕಿಕ RTLIL::Const::as_bool() ಧ್ರುವ
{
	ಯದಿ (is_str()) {
		ಪ್ರತಿ (ವರ್ಣ ch : get_str())
			ಯದಿ (ch != 0)
				ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಧ್ರುವ bitvectype& bv = get_bits();
	ಪ್ರತಿ (size_t i = 0; i < bv.size(); i++)
		ಯದಿ (bv[i] == State::S1)
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಪೂರ್ಣಾಙ್ಕ RTLIL::Const::as_int(ತಾರ್ಕಿಕ is_signed) ಧ್ರುವ
{
	int32_t ret = 0;
	ಯದಿ (ಧ್ರುವ std::string *s = get_if_str()) {
		ಪೂರ್ಣಾಙ್ಕ size = GetSize(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = std::min(4, size); i > 0; i--)
			ret |= static_cast<unsigned ವರ್ಣ>((*s)[size - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		ಯದಿ (is_signed && size > 0 && size < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << size*8;
		ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಧ್ರುವ bitvectype& bv = get_bits();
	ಪೂರ್ಣಾಙ್ಕ significant_bits = std::min(GetSize(bv), 32);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < significant_bits; i++)
		ಯದಿ (bv[i] == State::S1)
			ret |= 1 << i;
	ಯದಿ (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	ಪ್ರತ್ಯಾಗಮ ret;
}

ತಾರ್ಕಿಕ RTLIL::Const::convertible_to_int(ತಾರ್ಕಿಕ is_signed) ಧ್ರುವ
{
	auto size = get_min_size(is_signed);

	ಯದಿ (size < 0)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	// If it fits in 31 bits it is definitely convertible
	ಯದಿ (size <= 31)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	ಯದಿ (size == 32) {
		ಯದಿ (is_signed)
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		ಪ್ರತ್ಯಾಗಮ (*ಏತತ್)[size - 1] != State::S1;
	}

	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

std::optional<ಪೂರ್ಣಾಙ್ಕ> RTLIL::Const::try_as_int(ತಾರ್ಕಿಕ is_signed) ಧ್ರುವ
{
	ಯದಿ (!convertible_to_int(is_signed))
		ಪ್ರತ್ಯಾಗಮ std::nullopt;
	ಪ್ರತ್ಯಾಗಮ as_int(is_signed);
}

ಪೂರ್ಣಾಙ್ಕ RTLIL::Const::as_int_saturating(ತಾರ್ಕಿಕ is_signed) ಧ್ರುವ
{
	ಯದಿ (!convertible_to_int(is_signed)) {
		ಯದಿ (!is_signed)
			ಪ್ರತ್ಯಾಗಮ std::numeric_limits<ಪೂರ್ಣಾಙ್ಕ>::max();

		ಧ್ರುವ auto min_size = get_min_size(is_signed);
		log_assert(min_size > 0);
		ಧ್ರುವ auto neg = (*ಏತತ್)[min_size - 1];
		ಪ್ರತ್ಯಾಗಮ neg ? std::numeric_limits<ಪೂರ್ಣಾಙ್ಕ>::min() : std::numeric_limits<ಪೂರ್ಣಾಙ್ಕ>::max();
	}
	ಪ್ರತ್ಯಾಗಮ as_int(is_signed);
}

ಶೂನ್ಯ RTLIL::Const::tag_bare_integer_const(ಧ್ರುವ std::string &ಮೌಲ್ಯ)
{
	ಯದಿ (ಮೌಲ್ಯ.empty() || ಮೌಲ್ಯ.find('\'') != std::string::npos)
		ಪ್ರತ್ಯಾಗಮ;
	size_t start = (ಮೌಲ್ಯ[0] == '-' || ಮೌಲ್ಯ[0] == '+') ? 1 : 0;
	ಯದಿ (start < ಮೌಲ್ಯ.size() && std::all_of(ಮೌಲ್ಯ.begin() + start, ಮೌಲ್ಯ.end(), ::isdigit))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

ಪೂರ್ಣಾಙ್ಕ RTLIL::Const::get_min_size(ತಾರ್ಕಿಕ is_signed) ಧ್ರುವ
{
	ಯದಿ (empty()) ಪ್ರತ್ಯಾಗಮ 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	ಯದಿ (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	ಅನ್ಯಥಾ
		leading_bit = RTLIL::State::S0;

	auto idx = size();
	ಯಾವತ್ (idx > 0 && (*ಏತತ್)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	ಯದಿ (is_signed && idx < size()) {
		idx++;
	}
	// must be at least one bit
	ಪ್ರತ್ಯಾಗಮ (idx == 0) ? 1 : idx;
}

ಶೂನ್ಯ RTLIL::Const::compress(ತಾರ್ಕಿಕ is_signed)
{
	auto idx = get_min_size(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<ಪೂರ್ಣಾಙ್ಕ> RTLIL::Const::as_int_compress(ತಾರ್ಕಿಕ is_signed) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ಧ್ರುವ ವರ್ಣ* any) ಧ್ರುವ
{
	ಪೂರ್ಣಾಙ್ಕ sz = size();
	std::string ret;
	ret.reserve(sz);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = sz - 1; i >= 0; --i)
		ವಿಕಲ್ಪ ((*ಏತತ್)[i]) {
			ಪಕ್ಷ S0: ret.ತಳ್ಳು_back('0'); ಭಙ್ಗ;
			ಪಕ್ಷ S1: ret.ತಳ್ಳು_back('1'); ಭಙ್ಗ;
			ಪಕ್ಷ Sx: ret.ತಳ್ಳು_back('x'); ಭಙ್ಗ;
			ಪಕ್ಷ Sz: ret.ತಳ್ಳು_back('z'); ಭಙ್ಗ;
			ಪಕ್ಷ Sa: ret += any; ಭಙ್ಗ;
			ಪಕ್ಷ Sm: ret.ತಳ್ಳು_back('m'); ಭಙ್ಗ;
		}
	ಪ್ರತ್ಯಾಗಮ ret;
}

RTLIL::Const RTLIL::Const::from_string(ಧ್ರುವ std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.size());
	ಪ್ರತಿ (auto it = str.rbegin(); it != str.rend(); it++)
		ವಿಕಲ್ಪ (*it) {
			ಪಕ್ಷ '0': bv.ತಳ್ಳು_back(State::S0); ಭಙ್ಗ;
			ಪಕ್ಷ '1': bv.ತಳ್ಳು_back(State::S1); ಭಙ್ಗ;
			ಪಕ್ಷ 'x': bv.ತಳ್ಳು_back(State::Sx); ಭಙ್ಗ;
			ಪಕ್ಷ 'z': bv.ತಳ್ಳು_back(State::Sz); ಭಙ್ಗ;
			ಪಕ್ಷ 'm': bv.ತಳ್ಳು_back(State::Sm); ಭಙ್ಗ;
			ಸ್ವಭಾವ: bv.ತಳ್ಳು_back(State::Sa);
		}
	ಪ್ರತ್ಯಾಗಮ c;
}

std::string RTLIL::Const::decode_string() ಧ್ರುವ
{
	ಯದಿ (auto str = get_if_str())
		ಪ್ರತ್ಯಾಗಮ *str;

	ಧ್ರುವ bitvectype& bv = get_bits();
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ n = GetSize(bv);
	ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	ಪೂರ್ಣಾಙ್ಕ i = n_over_8 * 8;
	ಯದಿ (i < n) {
		ವರ್ಣ ch = 0;
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < (n - i); j++) {
			ಯದಿ (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		ಯದಿ (ch != 0)
			s.append({ch});
	}
	i -= 8;
	ಪ್ರತಿ (; i >= 0; i -= 8) {
		ವರ್ಣ ch = 0;
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < 8; j++) {
			ಯದಿ (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		ಯದಿ (ch != 0)
			s.append({ch});
	}
	ಪ್ರತ್ಯಾಗಮ s;
}

ಪೂರ್ಣಾಙ್ಕ RTLIL::Const::size() ಧ್ರುವ {
	ಯದಿ (is_str())
		ಪ್ರತ್ಯಾಗಮ 8 * str_.size();
	ಅನ್ಯಥಾ {
		check(is_bits());
		ಪ್ರತ್ಯಾಗಮ bits_.size();
	}
}

ತಾರ್ಕಿಕ RTLIL::Const::empty() ಧ್ರುವ {
	ಯದಿ (is_str())
		ಪ್ರತ್ಯಾಗಮ str_.empty();
	ಅನ್ಯಥಾ {
		check(is_bits());
		ಪ್ರತ್ಯಾಗಮ bits_.empty();
	}
}

ಶೂನ್ಯ RTLIL::Const::bitvectorize_internal() {
	ಯದಿ (tag == backing_tag::bits)
		ಪ್ರತ್ಯಾಗಮ;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.size() * 8);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = str_.size() - 1; i >= 0; i--) {
		unsigned ವರ್ಣ ch = str_[i];
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < 8; j++) {
			new_bits.ತಳ್ಳು_back((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(ಶೂನ್ಯ)ನೂತನ ((ಶೂನ್ಯ*)&bits_) bitvectype(std::move(new_bits));
		tag = backing_tag::bits;
	}
}

ಶೂನ್ಯ RTLIL::Const::append(ಧ್ರುವ RTLIL::Const &other) {
	bitvectorize_internal();
	bitvectype& bv = get_bits();
	bv.ಸೇರಿಸು(bv.end(), other.begin(), other.end());
}

RTLIL::State RTLIL::Const::const_iterator::operator*() ಧ್ರುವ {
	ಯದಿ (auto bv = parent->get_if_bits())
		ಪ್ರತ್ಯಾಗಮ (*bv)[idx];

	ಪೂರ್ಣಾಙ್ಕ char_idx = parent->get_str().size() - idx / 8 - 1;
	ತಾರ್ಕಿಕ bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	ಪ್ರತ್ಯಾಗಮ bit ? State::S1 : State::S0;
}

ತಾರ್ಕಿಕ RTLIL::Const::is_fully_zero() ಧ್ರುವ
{
	ಯದಿ (auto str = get_if_str()) {
		ಪ್ರತಿ (ವರ್ಣ ch : *str)
			ಯದಿ (ch != 0)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಧ್ರುವ bitvectype& bv = get_bits();

	ಪ್ರತಿ (ಧ್ರುವ auto &bit : bv)
		ಯದಿ (bit != RTLIL::State::S0)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::Const::is_fully_ones() ಧ್ರುವ
{
	ಯದಿ (auto str = get_if_str()) {
		ಪ್ರತಿ (ವರ್ಣ ch : *str)
			ಯದಿ (ch != (ವರ್ಣ)0xff)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಧ್ರುವ bitvectype& bv = get_bits();
	ಪ್ರತಿ (ಧ್ರುವ auto &bit : bv)
		ಯದಿ (bit != RTLIL::State::S1)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::Const::is_fully_def() ಧ್ರುವ
{
	ಯದಿ (is_str())
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;

	ಧ್ರುವ bitvectype& bv = get_bits();
	ಪ್ರತಿ (ಧ್ರುವ auto &bit : bv)
		ಯದಿ (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::Const::is_fully_undef() ಧ್ರುವ
{
	ಯದಿ (auto str = get_if_str())
		ಪ್ರತ್ಯಾಗಮ str->empty();

	ಧ್ರುವ bitvectype& bv = get_bits();
	ಪ್ರತಿ (ಧ್ರುವ auto &bit : bv)
		ಯದಿ (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::Const::is_fully_undef_x_only() ಧ್ರುವ
{
	ಯದಿ (auto str = get_if_str())
		ಪ್ರತ್ಯಾಗಮ str->empty();

	ಧ್ರುವ bitvectype& bv = get_bits();
	ಪ್ರತಿ (ಧ್ರುವ auto &bit : bv)
		ಯದಿ (bit != RTLIL::State::Sx)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::Const::is_onehot(ಪೂರ್ಣಾಙ್ಕ *pos) ಧ್ರುವ
{
	ತಾರ್ಕಿಕ found = ಅಸತ್ಯ;
	ಪೂರ್ಣಾಙ್ಕ size = GetSize(*ಏತತ್);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < size; i++) {
		State bit = (*ಏತತ್)[i];
		ಯದಿ (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		ಯದಿ (bit == RTLIL::State::S1) {
			ಯದಿ (found)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (pos)
				*pos = i;
			found = ಸತ್ಯ;
		}
	}
	ಪ್ರತ್ಯಾಗಮ found;
}

Hasher RTLIL::Const::hash_into(Hasher h) ಧ್ರುವ
{
	ಯದಿ (auto str = get_if_str())
		ಪ್ರತ್ಯಾಗಮ hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ಧ್ರುವ bitvectype &bv = get_bits();
	ಪೂರ್ಣಾಙ್ಕ size = GetSize(bv);
	std::string packed;
	ಪೂರ್ಣಾಙ್ಕ packed_size = (size + 7) >> 3;
	packed.resize(packed_size, 0);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ bi = 0; bi < packed_size; ++bi) {
		ವರ್ಣ ch = 0;
		ಪೂರ್ಣಾಙ್ಕ end = std::min((bi + 1)*8, size);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			ಯದಿ (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<ವರ್ಣ>::hash_into(ch, h);
				ಪ್ರತಿ (; i < size; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(size);
				ಪ್ರತ್ಯಾಗಮ h;
			}
			ch |= static_cast<ಪೂರ್ಣಾಙ್ಕ>(b) << (i & 7);
		}
		packed[packed_size - 1 - bi] = ch;
	}
	ಪ್ರತ್ಯಾಗಮ hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ len, RTLIL::State padding) ಧ್ರುವ {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = offset; i < offset + len; i++)
		ret_bv.ತಳ್ಳು_back(i < GetSize(*ಏತತ್) ? (*ಏತತ್)[i] : padding);
	ಪ್ರತ್ಯಾಗಮ RTLIL::Const(ret_bv);
}
#undef check /* check(condition) for Const */

ತಾರ್ಕಿಕ RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ attributes.ಎಣಿಕೆ(id);
}

ಶೂನ್ಯ RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, ತಾರ್ಕಿಕ ಮೌಲ್ಯ)
{
	ಯದಿ (ಮೌಲ್ಯ)
		attributes[id] = RTLIL::Const(1);
	ಅನ್ಯಥಾ
		attributes.erase(id);
}

ತಾರ್ಕಿಕ RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ಧ್ರುವ
{
	ಧ್ರುವ auto it = attributes.find(id);
	ಯದಿ (it == attributes.end())
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ it->second.as_bool();
}

ಶೂನ್ಯ RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string ಮೌಲ್ಯ)
{
	ಯದಿ (ಮೌಲ್ಯ.empty())
		attributes.erase(id);
	ಅನ್ಯಥಾ
		attributes[id] = ಮೌಲ್ಯ;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ಧ್ರುವ
{
	std::string ಮೌಲ್ಯ;
	ಧ್ರುವ auto it = attributes.find(id);
	ಯದಿ (it != attributes.end())
		ಮೌಲ್ಯ = it->second.decode_string();
	ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ;
}

ಶೂನ್ಯ RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, ಧ್ರುವ pool<string> &data)
{
	string attrval;
	ಪ್ರತಿ (ಧ್ರುವ auto &s : data) {
		ಯದಿ (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

ಶೂನ್ಯ RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, ಧ್ರುವ pool<string> &data)
{
	pool<string> union_data = get_strpool_attribute(id);
	union_data.ಸೇರಿಸು(data.begin(), data.end());
	ಯದಿ (!union_data.empty())
		set_strpool_attribute(id, union_data);
}

ಶೂನ್ಯ RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->attributes = std::move(buf);
}

ನಾಮಾಕಾಶ {
	template<typename F>
	ಶೂನ್ಯ walk_process_attr_objects(RTLIL::Process *process, F visit) {
		visit(process);
		std::vector<RTLIL::CaseRule*> case_ರಾಶಿ{&process->ಬೇರು_case};
		ಯಾವತ್ (!case_ರಾಶಿ.empty()) {
			RTLIL::CaseRule *cs = case_ರಾಶಿ.back();
			case_ರಾಶಿ.ತೆಗೆ_back();
			visit(cs);
			ಪ್ರತಿ (auto *sw : cs->switches) {
				visit(sw);
				ಪ್ರತಿ (auto *case_ : sw->cases)
					case_ರಾಶಿ.ತಳ್ಳು_back(case_);
			}
		}
		ಪ್ರತಿ (auto *ಸಮಕಾಲ : process->syncs)
			ಪ್ರತಿ (auto &mwa : ಸಮಕಾಲ->mem_write_actions)
				visit(&mwa);
	}

	template<typename F>
	ಶೂನ್ಯ walk_attr_objects(RTLIL::Design *design, F visit) {
		ಪ್ರತಿ (auto &[_, module] : design->modules_) {
			visit(module);
			ಪ್ರತಿ (auto &[_, wire] : module->wires_)
				visit(wire);
			ಪ್ರತಿ (auto &[_, mem] : module->memories)
				visit(mem);
			ಪ್ರತಿ (auto &[_, cell] : module->cells_)
				visit(cell);
			ಪ್ರತಿ (auto &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


size_t RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	auto ಬೇರು = [&](IdString ref) {
		ಯದಿ (ref != IdString::Null)
			live.ಸೇರಿಸು(ref);
	};

	walk_attr_objects(ಏತತ್, [&](ಧ್ರುವ RTLIL::AttrObject *obj) {
		ಪ್ರತಿ (auto &attr : obj->attributes)
			ಬೇರು(attr.first);
	});

	ಬೇರು(selected_active_module);

	ಪ್ರತಿ (auto &[name, module] : modules_) {
		ಬೇರು(name);
		ಪ್ರತಿ (IdString port : module->ports)
			ಬೇರು(port);
		ಪ್ರತಿ (IdString param : module->avail_parameters)
			ಬೇರು(param);
		ಪ್ರತಿ (auto &[param, _] : module->parameter_default_ಮೌಲ್ಯಗಳು)
			ಬೇರು(param);
		ಪ್ರತಿ (auto &[name, _] : module->memories)
			ಬೇರು(name);
		ಪ್ರತಿ (auto &[name, _] : module->processes)
			ಬೇರು(name);
		ಪ್ರತಿ (auto &[name, wire] : module->wires_) {
			ಬೇರು(name);
			ಯದಿ (wire->known_driver())
				ಬೇರು(wire->driverPort());
		}
		ಪ್ರತಿ (auto &[name, cell] : module->cells_) {
			ಬೇರು(name);
			ಬೇರು(cell->type);
			ಪ್ರತಿ (auto &conn : cell->connections())
				ಬೇರು(conn.first);
			ಪ್ರತಿ (auto &param : cell->parameters)
				ಬೇರು(param.first);
		}
		ಪ್ರತಿ (auto &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_ರಾಶಿ{&process->ಬೇರು_case};
			ಯಾವತ್ (!case_ರಾಶಿ.empty()) {
				RTLIL::CaseRule *cs = case_ರಾಶಿ.back();
				case_ರಾಶಿ.ತೆಗೆ_back();
				ಪ್ರತಿ (auto *sw : cs->switches) {
					ಪ್ರತಿ (auto *case_ : sw->cases)
						case_ರಾಶಿ.ತಳ್ಳು_back(case_);
				}
			}
			ಪ್ರತಿ (auto *ಸಮಕಾಲ : process->syncs) {
				ಪ್ರತಿ (auto &mwa : ಸಮಕಾಲ->mem_write_actions)
					ಬೇರು(mwa.memid);
			}
		}
	}

	auto ಬೇರು_selection = [&](ಧ್ರುವ RTLIL::Selection &sel) {
		ಪ್ರತಿ (IdString m : sel.selected_modules)
			ಬೇರು(m);
		ಪ್ರತಿ (auto &[m, members] : sel.selected_members) {
			ಬೇರು(m);
			ಪ್ರತಿ (IdString member : members)
				ಬೇರು(member);
		}
	};

	ಪ್ರತಿ (auto &[name, sel] : selection_vars) {
		ಬೇರು(name);
		ಬೇರು_selection(sel);
	}

	ಪ್ರತಿ (auto &sel : selection_ರಾಶಿ)
		ಬೇರು_selection(sel);

	size_t erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_ಎಣಿಕೆ;
	ಪ್ರತ್ಯಾಗಮ erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ಧ್ರುವ
{
	pool<string> data;
	ಯದಿ (attributes.ಎಣಿಕೆ(id) != 0)
		ಪ್ರತಿ (auto s : split_tokens(get_string_attribute(id), "|"))
			data.ಸೇರಿಸು(s);
	ಪ್ರತ್ಯಾಗಮ data;
}

ಶೂನ್ಯ RTLIL::AttrObject::set_hdlname_attribute(ಧ್ರುವ vector<string> &hierarchy)
{
	string attrval;
	ಪ್ರತಿ (ಧ್ರುವ auto &ident : hierarchy) {
		ಯದಿ (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ split_tokens(get_string_attribute(ID::hdlname), " ");
}

ಶೂನ್ಯ RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, ಧ್ರುವ vector<ಪೂರ್ಣಾಙ್ಕ> &data)
{
	std::stringstream attrval;
	ಪ್ರತಿ (auto &i : data) {
		ಯದಿ (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<ಪೂರ್ಣಾಙ್ಕ> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ಧ್ರುವ
{
	vector<ಪೂರ್ಣಾಙ್ಕ> data;
	auto it = attributes.find(id);
	ಯದಿ (it != attributes.end())
		ಪ್ರತಿ (ಧ್ರುವ auto &s : split_tokens(attributes.at(id).decode_string())) {
			ವರ್ಣ *end = nullptr;
			errno = 0;
			long ಮೌಲ್ಯ = strtol(s.c_str(), &end, 10);
			ಯದಿ (end != s.c_str() + s.size())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			ಯದಿ (errno == ERANGE || ಮೌಲ್ಯ < INT_MIN || ಮೌಲ್ಯ > INT_MAX)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			data.ತಳ್ಳು_back(ಮೌಲ್ಯ);
		}
	ಪ್ರತ್ಯಾಗಮ data;
}

ತಾರ್ಕಿಕ RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ಧ್ರುವ
{
	ಯದಿ (current_design != nullptr) {
		auto module = current_design->module(mod_name);
		ಪ್ರತ್ಯಾಗಮ module && module->get_blackbox_attribute();
	} ಅನ್ಯಥಾ {
		log_warning("Unable to check if module is boxed for null design.\n");
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}
}

ತಾರ್ಕಿಕ RTLIL::Selection::selected_module(RTLIL::IdString mod_name) ಧ್ರುವ
{
	ಯದಿ (complete_selection)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಯದಿ (!selects_boxes && boxed_module(mod_name))
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಯದಿ (full_selection)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಯದಿ (selected_modules.ಎಣಿಕೆ(mod_name) > 0)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಯದಿ (selected_members.ಎಣಿಕೆ(mod_name) > 0)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) ಧ್ರುವ
{
	ಯದಿ (complete_selection)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಯದಿ (!selects_boxes && boxed_module(mod_name))
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಯದಿ (full_selection)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಯದಿ (selected_modules.ಎಣಿಕೆ(mod_name) > 0)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ಧ್ರುವ
{
	ಯದಿ (complete_selection)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಯದಿ (!selects_boxes && boxed_module(mod_name))
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಯದಿ (full_selection)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಯದಿ (selected_modules.ಎಣಿಕೆ(mod_name) > 0)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಯದಿ (selected_members.ಎಣಿಕೆ(mod_name) > 0)
		ಯದಿ (selected_members.at(mod_name).ಎಣಿಕೆ(memb_name) > 0)
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ RTLIL::Selection::optimize(RTLIL::Design *design)
{
	ಯದಿ (design != current_design) {
		current_design = design;
	}

	ಯದಿ (selects_boxes && full_selection)
		complete_selection = ಸತ್ಯ;
	ಯದಿ (complete_selection) {
		full_selection = ಅಸತ್ಯ;
		selects_boxes = ಸತ್ಯ;
	}
	ಯದಿ (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		ಪ್ರತ್ಯಾಗಮ;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.clear();
	ಪ್ರತಿ (auto mod_name : selected_modules) {
		ಯದಿ (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.ತಳ್ಳು_back(mod_name);
		selected_members.erase(mod_name);
	}
	ಪ್ರತಿ (auto mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	ಪ್ರತಿ (auto &it : selected_members)
		ಯದಿ (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.ತಳ್ಳು_back(it.first);
	ಪ್ರತಿ (auto mod_name : del_list)
		selected_members.erase(mod_name);

	ಪ್ರತಿ (auto &it : selected_members) {
		del_list.clear();
		ಪ್ರತಿ (auto memb_name : it.second)
			ಯದಿ (current_design->module(it.first)->ಎಣಿಕೆ_id(memb_name) == 0)
				del_list.ತಳ್ಳು_back(memb_name);
		ಪ್ರತಿ (auto memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	add_list.clear();
	ಪ್ರತಿ (auto &it : selected_members)
		ಯದಿ (it.second.size() == 0)
			del_list.ತಳ್ಳು_back(it.first);
		ಅನ್ಯಥಾ ಯದಿ (it.second.size() == current_design->module(it.first)->wires_.size() + current_design->module(it.first)->memories.size() +
				current_design->module(it.first)->cells_.size() + current_design->module(it.first)->processes.size())
			add_list.ತಳ್ಳು_back(it.first);
	ಪ್ರತಿ (auto mod_name : del_list)
		selected_members.erase(mod_name);
	ಪ್ರತಿ (auto mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.ಸೇರಿಸು(mod_name);
	}

	ಯದಿ (selected_modules.size() == current_design->modules_.size()) {
		selected_modules.clear();
		selected_members.clear();
		ಯದಿ (selects_boxes)
			complete_selection = ಸತ್ಯ;
		ಅನ್ಯಥಾ
			full_selection = ಸತ್ಯ;
	}
}

ಶೂನ್ಯ RTLIL::Selection::clear()
{
	full_selection = ಅಸತ್ಯ;
	complete_selection = ಅಸತ್ಯ;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_defines (ನೂತನ define_map_t)
{
	ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ hashidx_ಎಣಿಕೆ = 123456789;
	hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
	hashidx_ = hashidx_ಎಣಿಕೆ;

	refcount_modules_ = 0;
	selected_active_module.clear();
	ತಳ್ಳು_full_selection();

	RTLIL::Design::get_all_designs()->ಸೇರಿಸು(std::pair<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Design*>(hashidx_, ಏತತ್));
}

RTLIL::Design::~Design()
{
	ಪ್ರತಿ (auto &pr : modules_)
		delete pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

ಸ್ಥಿರ std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Design*> all_designs;
std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Design*> *RTLIL::Design::get_all_designs(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	ಪ್ರತ್ಯಾಗಮ RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ಧ್ರುವ RTLIL::Module *RTLIL::Design::module(IdString id) ಧ್ರುವ {
	twines_.check_owned(id);
	ಪ್ರತ್ಯಾಗಮ modules_.ಎಣಿಕೆ(id) ? modules_.at(id) : ರಿಕ್ತ;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	ಪ್ರತ್ಯಾಗಮ modules_.ಎಣಿಕೆ(id) ? modules_.at(id) : ರಿಕ್ತ;
}

RTLIL::Module *RTLIL::Design::ತುದಿ_module() ಧ್ರುವ
{
	RTLIL::Module *module = nullptr;
	ಪೂರ್ಣಾಙ್ಕ module_ಎಣಿಕೆ = 0;

	ಪ್ರತಿ (auto mod : selected_modules()) {
		ಯದಿ (mod->get_bool_attribute(ID::ತುದಿ))
			ಪ್ರತ್ಯಾಗಮ mod;
		module_ಎಣಿಕೆ++;
		module = mod;
	}

	ಪ್ರತ್ಯಾಗಮ module_ಎಣಿಕೆ == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	ಯದಿ (modules_.ಎಣಿಕೆ(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = ನೂತನ RTLIL::Module;
	modules_[name] = module;
	module->design = ಏತತ್;
	module->name = name;

	ಪ್ರತಿ (auto mon : monitors)
		mon->notify_module_add(module);

	ಯದಿ (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	ಪ್ರತ್ಯಾಗಮ module;
}

ಶೂನ್ಯ RTLIL::Design::scratchpad_unset(ಧ್ರುವ std::string &varname)
{
	scratchpad.erase(varname);
}

ಶೂನ್ಯ RTLIL::Design::scratchpad_set_int(ಧ್ರುವ std::string &varname, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ)
{
	scratchpad[varname] = stringf("%d", ಮೌಲ್ಯ);
}

ಶೂನ್ಯ RTLIL::Design::scratchpad_set_bool(ಧ್ರುವ std::string &varname, ತಾರ್ಕಿಕ ಮೌಲ್ಯ)
{
	scratchpad[varname] = ಮೌಲ್ಯ ? "true" : "false";
}

ಶೂನ್ಯ RTLIL::Design::scratchpad_set_string(ಧ್ರುವ std::string &varname, std::string ಮೌಲ್ಯ)
{
	scratchpad[varname] = std::move(ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ RTLIL::Design::scratchpad_get_int(ಧ್ರುವ std::string &varname, ಪೂರ್ಣಾಙ್ಕ default_ಮೌಲ್ಯ_2) ಧ್ರುವ
{
	auto it = scratchpad.find(varname);
	ಯದಿ (it == scratchpad.end())
		ಪ್ರತ್ಯಾಗಮ default_ಮೌಲ್ಯ_2;

	ಧ್ರುವ std::string &str = it->second;

	ಯದಿ (str == "0" || str == "false")
		ಪ್ರತ್ಯಾಗಮ 0;

	ಯದಿ (str == "1" || str == "true")
		ಪ್ರತ್ಯಾಗಮ 1;

	ವರ್ಣ *endptr = nullptr;
	long ಪೂರ್ಣಾಙ್ಕ parsed_ಮೌಲ್ಯ = strtol(str.c_str(), &endptr, 10);
	ಪ್ರತ್ಯಾಗಮ *endptr ? default_ಮೌಲ್ಯ_2 : parsed_ಮೌಲ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::Design::scratchpad_get_bool(ಧ್ರುವ std::string &varname, ತಾರ್ಕಿಕ default_ಮೌಲ್ಯ_2) ಧ್ರುವ
{
	auto it = scratchpad.find(varname);
	ಯದಿ (it == scratchpad.end())
		ಪ್ರತ್ಯಾಗಮ default_ಮೌಲ್ಯ_2;

	ಧ್ರುವ std::string &str = it->second;

	ಯದಿ (str == "0" || str == "false")
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಯದಿ (str == "1" || str == "true")
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;

	ಪ್ರತ್ಯಾಗಮ default_ಮೌಲ್ಯ_2;
}

std::string RTLIL::Design::scratchpad_get_string(ಧ್ರುವ std::string &varname, ಧ್ರುವ std::string &default_ಮೌಲ್ಯ_2) ಧ್ರುವ
{
	auto it = scratchpad.find(varname);
	ಯದಿ (it == scratchpad.end())
		ಪ್ರತ್ಯಾಗಮ default_ಮೌಲ್ಯ_2;

	ಪ್ರತ್ಯಾಗಮ it->second;
}

ಶೂನ್ಯ RTLIL::Design::remove(RTLIL::Module *module)
{
	ಪ್ರತಿ (auto mon : monitors)
		mon->notify_module_del(module);

	ಯದಿ (yosys_xtrace) {
		log("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	delete module;
}

ಶೂನ್ಯ RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	add(module);
}

ಶೂನ್ಯ RTLIL::Design::sort()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
	ಪ್ರತಿ (auto &it : modules_)
		it.second->sort();
}

ಶೂನ್ಯ RTLIL::Design::sort_modules()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
}

ಶೂನ್ಯ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

ಶೂನ್ಯ RTLIL::Design::check()
{
#ifndef NDEBUG
	log_assert(!selection_ರಾಶಿ.empty());
	ಪೂರ್ಣಾಙ್ಕ pool_size = 0;
	ಪ್ರತಿ (auto &it : modules_)
		pool_size = std::max(pool_size, ThreadPool::work_pool_size(0, it.second->cells_size(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_size);
	ಪ್ರತಿ (auto &it : modules_) {
		log_assert(ಏತತ್ == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#endif
}

ಶೂನ್ಯ RTLIL::Design::optimize()
{
	ಪ್ರತಿ (auto &it : modules_)
		it.second->optimize();
	ಪ್ರತಿ (auto &it : selection_ರಾಶಿ)
		it.optimize(ಏತತ್);
	ಪ್ರತಿ (auto &it : selection_vars)
		it.second.optimize(ಏತತ್);
}

ಶೂನ್ಯ RTLIL::Design::clone_into(RTLIL::Design *dst) ಧ್ರುವ
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	ಪ್ರತಿ (auto it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

ತಾರ್ಕಿಕ RTLIL::Design::selected_module(IdString mod_name) ಧ್ರುವ
{
	ಯದಿ (!selected_active_module.empty() && mod_name != selected_active_module)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ selection().selected_module(mod_name);
}

ತಾರ್ಕಿಕ RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) ಧ್ರುವ
{
	ಯದಿ (!selected_active_module.empty() && mod_name != selected_active_module)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ selection().selected_whole_module(mod_name);
}

ತಾರ್ಕಿಕ RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ಧ್ರುವ
{
	ಯದಿ (!selected_active_module.empty() && mod_name != selected_active_module)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ selection().selected_member(mod_name, memb_name);
}

ತಾರ್ಕಿಕ RTLIL::Design::selected_module(RTLIL::Module *mod) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ selected_module(mod->name);
}

ತಾರ್ಕಿಕ RTLIL::Design::selected_whole_module(RTLIL::Module *mod) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ selected_whole_module(mod->name);
}

ಶೂನ್ಯ RTLIL::Design::ತಳ್ಳು_selection(RTLIL::Selection sel)
{
	sel.current_design = ಏತತ್;
	selection_ರಾಶಿ.ತಳ್ಳು_back(sel);
}

ಶೂನ್ಯ RTLIL::Design::ತಳ್ಳು_empty_selection()
{
	ತಳ್ಳು_selection(RTLIL::Selection::EmptySelection(ಏತತ್));
}

ಶೂನ್ಯ RTLIL::Design::ತಳ್ಳು_full_selection()
{
	ತಳ್ಳು_selection(RTLIL::Selection::FullSelection(ಏತತ್));
}

ಶೂನ್ಯ RTLIL::Design::ತಳ್ಳು_complete_selection()
{
	ತಳ್ಳು_selection(RTLIL::Selection::CompleteSelection(ಏತತ್));
}

ಶೂನ್ಯ RTLIL::Design::ತೆಗೆ_selection()
{
	selection_ರಾಶಿ.ತೆಗೆ_back();
	// Default to a full_selection if we ran out of stack
	ಯದಿ (selection_ರಾಶಿ.empty())
		ತಳ್ಳು_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(ತಾರ್ಕಿಕ only_selected) ಧ್ರುವ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(ಏತತ್), only_selected, ಸತ್ಯ, ಅಸತ್ಯ, RTLIL_BACKEND::DumpMode::Readable);
	ಪ್ರತ್ಯಾಗಮ f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ಧ್ರುವ
{
	ತಾರ್ಕಿಕ include_partials = partials == RTLIL::SELECT_ALL;
	ತಾರ್ಕಿಕ exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	ತಾರ್ಕಿಕ ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> result;
	result.reserve(modules_.size());
	ಪ್ರತಿ (auto &it : modules_) {
		ಯದಿ (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			ಯದಿ (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				result.ತಳ್ಳು_back(it.second);
			ಅನ್ಯಥಾ
				ವಿಕಲ್ಪ (boxes)
				{
				ಪಕ್ಷ RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.second);
					ಭಙ್ಗ;
				ಪಕ್ಷ RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.second);
					ಭಙ್ಗ;
				ಪಕ್ಷ RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.second);
					ಭಙ್ಗ;
				ಪಕ್ಷ RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.second);
					ಭಙ್ಗ;
				ಪಕ್ಷ RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.second);
					ಭಙ್ಗ;
				ಪಕ್ಷ RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.second);
					ಭಙ್ಗ;
				ಸ್ವಭಾವ:
					ಭಙ್ಗ;
				}
		} ಅನ್ಯಥಾ ಯದಿ (!include_partials && selected_module(it.first)) {
			ವಿಕಲ್ಪ(partials)
			{
			ಪಕ್ಷ RTLIL::SELECT_WHOLE_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.second);
				ಭಙ್ಗ;
			ಪಕ್ಷ RTLIL::SELECT_WHOLE_ERR:
				log_error("Unsupported partially selected module %s.\n", it.second);
				ಭಙ್ಗ;
			ಪಕ್ಷ RTLIL::SELECT_WHOLE_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.second);
				ಭಙ್ಗ;
			ಸ್ವಭಾವ:
				ಭಙ್ಗ;
			}
		}
	}
	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Module::Module()
{
	ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ hashidx_ಎಣಿಕೆ = 123456789;
	hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
	hashidx_ = hashidx_ಎಣಿಕೆ;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->ಸೇರಿಸು(std::pair<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Module*>(hashidx_, ಏತತ್));
#endif
}

RTLIL::Module::~Module()
{
	ಪ್ರತಿ (auto &pr : wires_)
		delete pr.second;
	ಪ್ರತಿ (auto &pr : memories)
		delete pr.second;
	ಪ್ರತಿ (auto &pr : cells_)
		delete pr.second;
	ಪ್ರತಿ (auto &pr : processes)
		delete pr.second;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
ಸ್ಥಿರ std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Module*> all_modules;
std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Module*> *RTLIL::Module::get_all_modules(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ &all_modules;
}
#endif

ಶೂನ್ಯ RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	ಪ್ರತಿ (auto it = wires_.begin(); it != wires_.end(); ++it)
		ಯದಿ (!it->second->port_ಒಳಹರಿವು && !it->second->port_output)
			delwires.ಸೇರಿಸು(it->second);

	ಪ್ರತಿ (auto it = memories.begin(); it != memories.end(); ++it)
		delete it->second;
	memories.clear();

	ಪ್ರತಿ (auto it = cells_.begin(); it != cells_.end(); ++it)
		delete it->second;
	cells_.clear();

	ಪ್ರತಿ (auto it = processes.begin(); it != processes.end(); ++it)
		delete it->second;
	processes.clear();

	connections_.clear();

	remove(delwires);
	set_bool_attribute(ID::blackbox);
}

ಶೂನ್ಯ RTLIL::Module::expand_interfaces(RTLIL::Design *, ಧ್ರುವ dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

ತಾರ್ಕಿಕ RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ಧ್ರುವ dict<RTLIL::IdString, RTLIL::Const> &, ತಾರ್ಕಿಕ mayfail)
{
	ಯದಿ (mayfail)
		ಪ್ರತ್ಯಾಗಮ IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ಧ್ರುವ dict<RTLIL::IdString, RTLIL::Const> &, ಧ್ರುವ dict<RTLIL::IdString, RTLIL::Module*> &, ಧ್ರುವ dict<RTLIL::IdString, RTLIL::IdString> &, ತಾರ್ಕಿಕ mayfail)
{
	ಯದಿ (mayfail)
		ಪ್ರತ್ಯಾಗಮ IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

size_t RTLIL::Module::ಎಣಿಕೆ_id(RTLIL::IdString id)
{
	ಪ್ರತ್ಯಾಗಮ wires_.ಎಣಿಕೆ(id) + cells_.ಎಣಿಕೆ(id) + memories.ಎಣಿಕೆ(id) + processes.ಎಣಿಕೆ(id);
}

#ifndef NDEBUG
ನಾಮಾಕಾಶ {
	ಸಂರಚನಾ InternalCellChecker
	{
		ಧ್ರುವ RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		InternalCellChecker(ಧ್ರುವ RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		ಶೂನ್ಯ error(ಪೂರ್ಣಾಙ್ಕ linenr, std::optional<std::string> error_ಸಂದೇಶ = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			ಯದಿ (error_ಸಂದೇಶ) {
				log_error("%s in internal cell %s\n", *error_ಸಂದೇಶ, cell_loc);
			} ಅನ್ಯಥಾ {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		ಪೂರ್ಣಾಙ್ಕ param(RTLIL::IdString name)
		{
			auto it = cell->parameters.find(name);
			ಯದಿ (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.ಸೇರಿಸು(name);
			ಪ್ರತ್ಯಾಗಮ it->second.as_int();
		}

		ಪೂರ್ಣಾಙ್ಕ param_bool(RTLIL::IdString name)
		{
			ಪೂರ್ಣಾಙ್ಕ v = param(name);
			ಯದಿ (GetSize(cell->parameters.at(name)) > 32)
				error(__LINE__);
			ಯದಿ (v != 0 && v != 1)
				error(__LINE__);
			ಪ್ರತ್ಯಾಗಮ v;
		}

		ಪೂರ್ಣಾಙ್ಕ param_bool(RTLIL::IdString name, ತಾರ್ಕಿಕ expected)
		{
			ಪೂರ್ಣಾಙ್ಕ v = param_bool(name);
			ಯದಿ (v != expected)
				error(__LINE__);
			ಪ್ರತ್ಯಾಗಮ v;
		}

		ಶೂನ್ಯ param_bits(RTLIL::IdString name, ಪೂರ್ಣಾಙ್ಕ width)
		{
			param(name);
			ಯದಿ (GetSize(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			ಪ್ರತ್ಯಾಗಮ cell->parameters.at(name).decode_string();
		}

		ಶೂನ್ಯ port(RTLIL::IdString name, ಪೂರ್ಣಾಙ್ಕ width)
		{
			auto it = cell->connections_.find(name);
			ಯದಿ (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			ಯದಿ (GetSize(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.ಸೇರಿಸು(name);
		}

		ಶೂನ್ಯ check_expected(ತಾರ್ಕಿಕ check_matched_sign = ಅಸತ್ಯ)
		{
			ಪ್ರತಿ (auto &para : cell->parameters)
				ಯದಿ (expected_params.ಎಣಿಕೆ(para.first) == 0)
					error(__LINE__);
			ಪ್ರತಿ (auto &conn : cell->connections())
				ಯದಿ (expected_ports.ಎಣಿಕೆ(conn.first) == 0)
					error(__LINE__);

			ಯದಿ (check_matched_sign) {
				log_assert(expected_params.ಎಣಿಕೆ(ID::A_SIGNED) != 0 && expected_params.ಎಣಿಕೆ(ID::B_SIGNED) != 0);
				ತಾರ್ಕಿಕ a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				ತಾರ್ಕಿಕ b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				ಯದಿ (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		ಶೂನ್ಯ check()
		{
			ಯದಿ (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				ಪ್ರತ್ಯಾಗಮ;

			ಯದಿ (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(ಸತ್ಯ);
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/ಅಸತ್ಯ);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/ಅಸತ್ಯ);
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type.in(ID($shift), ID($shiftx))) {
				ಯದಿ (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/ಅಸತ್ಯ);
				} ಅನ್ಯಥಾ {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/ಅಸತ್ಯ);
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(ಸತ್ಯ);
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($pow));
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::CI, 1);
				port(ID::BI, 1);
				port(ID::X, param(ID::Y_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				port(ID::CO, param(ID::Y_WIDTH));
				check_expected(ಸತ್ಯ);
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($macc_v2)) {
				ಯದಿ (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				ಯದಿ (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				ಯದಿ (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ಧ್ರುವ Const &a_width = cell->getParam(ID::A_WIDTHS);
				ಧ್ರುವ Const &b_width = cell->getParam(ID::B_WIDTHS);
				ಧ್ರುವ Const &c_width = cell->getParam(ID::C_WIDTHS);
				ಪೂರ್ಣಾಙ್ಕ a_width_sum = 0, b_width_sum = 0, c_width_sum = 0;
				ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_sum += a_width.extract(16 * i, 16).as_int(ಅಸತ್ಯ);
					b_width_sum += b_width.extract(16 * i, 16).as_int(ಅಸತ್ಯ);
				}
				ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < param(ID::NADDENDS); i++) {
					c_width_sum += c_width.extract(16 * i, 16).as_int(ಅಸತ್ಯ);
				}
				port(ID::A, a_width_sum);
				port(ID::B, b_width_sum);
				port(ID::C, c_width_sum);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/ಅಸತ್ಯ);
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				ಯದಿ (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($dffsre)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_ಮೌಲ್ಯ, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_ಮೌಲ್ಯ, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type.in(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_ಮೌಲ್ಯ, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_ಮೌಲ್ಯ, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($aldffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_ಮೌಲ್ಯ, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_BITS);
				param(ID::STATE_ಅಂಕೆ);
				param(ID::STATE_ಅಂಕೆ_LOG2);
				param(ID::STATE_RST);
				param_bits(ID::STATE_TABLE, param(ID::STATE_BITS) * param(ID::STATE_ಅಂಕೆ));
				param(ID::TRANS_ಅಂಕೆ);
				param_bits(ID::TRANS_TABLE, param(ID::TRANS_ಅಂಕೆ) * (2*param(ID::STATE_ಅಂಕೆ_LOG2) + param(ID::CTRL_IN_WIDTH) + param(ID::CTRL_OUT_WIDTH)));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::CTRL_IN, param(ID::CTRL_IN_WIDTH));
				port(ID::CTRL_OUT, param(ID::CTRL_OUT_WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_MASK);
				param(ID::COLLISION_X_MASK);
				param_bool(ID::CE_OVER_SRST);
				param_bits(ID::ARST_ಮೌಲ್ಯ, param(ID::WIDTH));
				param_bits(ID::SRST_ಮೌಲ್ಯ, param(ID::WIDTH));
				param_bits(ID::INIT_ಮೌಲ್ಯ, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::SRST, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($memwr_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PORTID);
				param(ID::PRIORITY_MASK);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::ಪದಗಳು));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::ಪದಗಳು));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($mem)) {
				param(ID::MEMID);
				param(ID::SIZE);
				param(ID::OFFSET);
				param(ID::INIT);
				param_bits(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_bits(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_bits(ID::RD_TRANSPARENT, max(1, param(ID::RD_PORTS)));
				param_bits(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_bits(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				port(ID::RD_CLK, param(ID::RD_PORTS));
				port(ID::RD_EN, param(ID::RD_PORTS));
				port(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				port(ID::RD_DATA, param(ID::RD_PORTS) * param(ID::WIDTH));
				port(ID::WR_CLK, param(ID::WR_PORTS));
				port(ID::WR_EN, param(ID::WR_PORTS) * param(ID::WIDTH));
				port(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				port(ID::WR_DATA, param(ID::WR_PORTS) * param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($mem_v2)) {
				param(ID::MEMID);
				param(ID::SIZE);
				param(ID::OFFSET);
				param(ID::INIT);
				param_bits(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_bits(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_bits(ID::RD_TRANSPARENCY_MASK, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_bits(ID::RD_COLLISION_X_MASK, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_bits(ID::RD_WIDE_CONTINUATION, max(1, param(ID::RD_PORTS)));
				param_bits(ID::RD_CE_OVER_SRST, max(1, param(ID::RD_PORTS)));
				param_bits(ID::RD_ARST_ಮೌಲ್ಯ, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_SRST_ಮೌಲ್ಯ, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_INIT_ಮೌಲ್ಯ, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_bits(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				param_bits(ID::WR_WIDE_CONTINUATION, max(1, param(ID::WR_PORTS)));
				param_bits(ID::WR_PRIORITY_MASK, max(1, param(ID::WR_PORTS) * param(ID::WR_PORTS)));
				port(ID::RD_CLK, param(ID::RD_PORTS));
				port(ID::RD_EN, param(ID::RD_PORTS));
				port(ID::RD_ARST, param(ID::RD_PORTS));
				port(ID::RD_SRST, param(ID::RD_PORTS));
				port(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				port(ID::RD_DATA, param(ID::RD_PORTS) * param(ID::WIDTH));
				port(ID::WR_CLK, param(ID::WR_PORTS));
				port(ID::WR_EN, param(ID::WR_PORTS) * param(ID::WIDTH));
				port(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				port(ID::WR_DATA, param(ID::WR_PORTS) * param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type.in(ID($ಪ್ರತಿಜ್ಞಾ), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type.in(ID($specify2), ID($specify3))) {
				param_bool(ID::FULL);
				param_bool(ID::SRC_DST_PEN);
				param_bool(ID::SRC_DST_POL);
				param(ID::T_RISE_MIN);
				param(ID::T_RISE_TYP);
				param(ID::T_RISE_MAX);
				param(ID::T_FALL_MIN);
				param(ID::T_FALL_TYP);
				param(ID::T_FALL_MAX);
				port(ID::EN, 1);
				port(ID::SRC, param(ID::SRC_WIDTH));
				port(ID::DST, param(ID::DST_WIDTH));
				ಯದಿ (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
				}
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($specrule)) {
				param(ID::TYPE);
				param_bool(ID::SRC_PEN);
				param_bool(ID::SRC_POL);
				param_bool(ID::DST_PEN);
				param_bool(ID::DST_POL);
				param(ID::T_LIMIT_MIN);
				param(ID::T_LIMIT_TYP);
				param(ID::T_LIMIT_MAX);
				param(ID::T_LIMIT2_MIN);
				param(ID::T_LIMIT2_TYP);
				param(ID::T_LIMIT2_MAX);
				port(ID::SRC_EN, 1);
				port(ID::DST_EN, 1);
				port(ID::SRC, param(ID::SRC_WIDTH));
				port(ID::DST, param(ID::DST_WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				ಯದಿ (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
					error(__LINE__);
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::A, 1);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				ಯದಿ (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				ಪ್ರತ್ಯಾಗಮ;
			}

			ಯದಿ (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }
			ಯದಿ (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); ಪ್ರತ್ಯಾಗಮ; }

			ಯದಿ (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}
			ಯದಿ (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}
			ಯದಿ (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}
			ಯದಿ (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}
			ಯದಿ (cell->type.in(ID($ಒಳಹರಿವು_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
			}
			ಯದಿ (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				check_expected();
				ಪ್ರತ್ಯಾಗಮ;
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
#endif

ಶೂನ್ಯ RTLIL::Module::sort()
{
	wires_.sort(sort_by_id_str(design->twines()));
	cells_.sort(sort_by_id_str(design->twines()));
	parameter_default_ಮೌಲ್ಯಗಳು.sort(sort_by_id_str(design->twines()));
	memories.sort(sort_by_id_str(design->twines()));
	processes.sort(sort_by_id_str(design->twines()));
	ಪ್ರತಿ (auto &it : cells_)
		it.second->sort();
	ಪ್ರತಿ (auto &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	ಪ್ರತಿ (auto &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#ifndef NDEBUG
ಸ್ಥಿರ ಶೂನ್ಯ check_id_ಕೀಲಿಗಳು(ಧ್ರುವ dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	ಪ್ರತಿ (auto &it : keyed)
		log_assert(!it.first.empty());
}
#endif

ಶೂನ್ಯ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ifndef NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->cells_size(), 1000));
	ಧ್ರುವ RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	ಪ್ರತಿ (auto &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->width >= 0);
		log_assert(it.second->size >= 0);
		check_id_ಕೀಲಿಗಳು(it.second->attributes);
		memory_strings.ಸೇರಿಸು(it.second->name.str());
	}

	std::vector<MonotonicFlag> ports_declared(GetSize(module->ports));
	ShardedVector<std::string> memids(subpool);
	subpool.run([const_module, &ports_declared, &memory_strings, &memids](ಧ್ರುವ ParallelDispatchThreadPool::RunCtx &ctx) {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i : ctx.item_range(const_module->cells_size())) {
			auto it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			ಪ್ರತಿ (auto &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_ಕೀಲಿಗಳು(it.second->attributes);
			check_id_ಕೀಲಿಗಳು(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			ಯದಿ (it.second->has_memid()) {
				log_assert(memory_strings.ಎಣಿಕೆ(it.second->parameters.at(ID::MEMID).decode_string()));
			} ಅನ್ಯಥಾ ಯದಿ (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.ಎಣಿಕೆ(memid));
				memids.ಸೇರಿಸು(ctx, std::move(memid));
			}
			auto cell_mod = const_module->design->module(it.second->name);
			ಯದಿ (cell_mod != nullptr) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#ifdef __GNUC__
				#pragma GCC diagnostic ತಳ್ಳು
				#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				#endif
				log_assert(!it.second->get_blackbox_attribute());
				#ifdef __GNUC__
				#pragma GCC diagnostic ತೆಗೆ
				#endif
			}
		}

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i : ctx.item_range(const_module->wires_size())) {
			auto it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_ಕೀಲಿಗಳು(it.second->attributes);
			ಯದಿ (it.second->port_id) {
				log_assert(GetSize(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_ಒಳಹರಿವು || it.second->port_output);
				log_assert(it.second->port_id <= GetSize(ports_declared));
				ತಾರ್ಕಿಕ previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == ಅಸತ್ಯ);
			} ಅನ್ಯಥಾ
				log_assert(!it.second->port_ಒಳಹರಿವು && !it.second->port_output);
		}
	});
	ಪ್ರತಿ (ಧ್ರುವ MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == ಸತ್ಯ);
	pool<std::string> memids_pool;
	ಪ್ರತಿ (std::string &memid : memids)
		log_assert(memids_pool.ಸೇರಿಸು(memid).second);

	ಪ್ರತಿ (auto &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->ಬೇರು_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->ಬೇರು_case};
		ಪ್ರತಿ (size_t i = 0; i < all_cases.size(); i++) {
			ಪ್ರತಿ (auto &switch_it : all_cases[i]->switches) {
				ಪ್ರತಿ (auto &case_it : switch_it->cases) {
					ಪ್ರತಿ (auto &compare_it : case_it->compare) {
						log_assert(switch_it->ಸಙ್ಕೇತ.size() == compare_it.size());
					}
					all_cases.ತಳ್ಳು_back(case_it);
				}
			}
		}
		ಪ್ರತಿ (auto &sync_it : it.second->syncs) {
			ವಿಕಲ್ಪ (sync_it->type) {
				ಪಕ್ಷ RTLIL::SyncType::ST0:
				ಪಕ್ಷ RTLIL::SyncType::ST1:
				ಪಕ್ಷ RTLIL::SyncType::STp:
				ಪಕ್ಷ RTLIL::SyncType::STn:
				ಪಕ್ಷ RTLIL::SyncType::STe:
					log_assert(!sync_it->ಸಙ್ಕೇತ.empty());
					ಭಙ್ಗ;
				ಪಕ್ಷ RTLIL::SyncType::STa:
				ಪಕ್ಷ RTLIL::SyncType::STg:
				ಪಕ್ಷ RTLIL::SyncType::STi:
					log_assert(sync_it->ಸಙ್ಕೇತ.empty());
					ಭಙ್ಗ;
			}
		}
	}

	ಪ್ರತಿ (auto &it : module->connections_) {
		log_assert(it.first.size() == it.second.size());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_ಕೀಲಿಗಳು(module->attributes);
#endif
}

ಶೂನ್ಯ RTLIL::Module::check()
{
	ಪೂರ್ಣಾಙ್ಕ pool_size = ThreadPool::work_pool_size(0, cells_size(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_size);
	check_module(ಏತತ್, thread_pool);
}

ಶೂನ್ಯ RTLIL::Module::optimize()
{
}

ಶೂನ್ಯ RTLIL::Module::cloneInto(RTLIL::Module *new_mod) ಧ್ರುವ
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	ಪ್ರತಿ (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_ಮೌಲ್ಯಗಳು.clear();
	ಪ್ರತಿ (auto &it : parameter_default_ಮೌಲ್ಯಗಳು)
		new_mod->parameter_default_ಮೌಲ್ಯಗಳು[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	ಪ್ರತಿ (auto &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	ಪ್ರತಿ (auto it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	ಪ್ರತಿ (auto it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	ಪ್ರತಿ (auto it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	ಪ್ರತಿ (auto it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addProcess(dst_id, it->second);
	}

	ಸಂರಚನಾ RewriteSigSpecWorker
	{
		ಧ್ರುವ dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ಧ್ರುವ RTLIL::Module *new_mod;
		ಶೂನ್ಯ operator()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([ಏತತ್](RTLIL::Wire *&wire) {
				auto it = wire_map.find(wire);
				ಯದಿ (it != wire_map.end())
					wire = it->second;
				ಅನ್ಯಥಾ
					log_assert(wire->module == new_mod);
			});
		}
	};

	RewriteSigSpecWorker rewriteSigSpecWorker{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewriteSigSpecWorker);
	new_mod->fixup_ports();
}

RTLIL::Module *RTLIL::Module::clone() ಧ್ರುವ
{
	RTLIL::Module *new_mod = ನೂತನ RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	ಪ್ರತ್ಯಾಗಮ new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) ಧ್ರುವ
{
	RTLIL::Module *new_mod = ನೂತನ RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->add(new_mod);
	ಪ್ರತ್ಯಾಗಮ new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) ಧ್ರುವ
{
	RTLIL::Module *new_mod = ನೂತನ RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	ಪ್ರತ್ಯಾಗಮ new_mod;
}

ತಾರ್ಕಿಕ RTLIL::Module::has_memories() ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ !memories.empty();
}

ತಾರ್ಕಿಕ RTLIL::Module::has_processes() ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ !processes.empty();
}

ತಾರ್ಕಿಕ RTLIL::Module::has_memories_warn() ಧ್ರುವ
{
	ಯದಿ (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", ಏತತ್);
	ಪ್ರತ್ಯಾಗಮ !memories.empty();
}

ತಾರ್ಕಿಕ RTLIL::Module::has_processes_warn() ಧ್ರುವ
{
	ಯದಿ (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", ಏತತ್);
	ಪ್ರತ್ಯಾಗಮ !processes.empty();
}

ತಾರ್ಕಿಕ RTLIL::Module::is_selected() ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ design->selected_module(ಏತತ್->name);
}

ತಾರ್ಕಿಕ RTLIL::Module::is_selected_whole() ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ design->selected_whole_module(ಏತತ್->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() ಧ್ರುವ
{
	std::vector<RTLIL::Wire*> result;
	result.reserve(wires_.size());
	ಪ್ರತಿ (auto &it : wires_)
		ಯದಿ (design->selected(ಏತತ್, it.second))
			result.ತಳ್ಳು_back(it.second);
	ಪ್ರತ್ಯಾಗಮ result;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() ಧ್ರುವ
{
	std::vector<RTLIL::Cell*> result;
	result.reserve(cells_.size());
	ಪ್ರತಿ (auto &it : cells_)
		ಯದಿ (design->selected(ಏತತ್, it.second))
			result.ತಳ್ಳು_back(it.second);
	ಪ್ರತ್ಯಾಗಮ result;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() ಧ್ರುವ
{
	std::vector<RTLIL::Memory*> result;
	result.reserve(memories.size());
	ಪ್ರತಿ (auto &it : memories)
		ಯದಿ (design->selected(ಏತತ್, it.second))
			result.ತಳ್ಳು_back(it.second);
	ಪ್ರತ್ಯಾಗಮ result;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() ಧ್ರುವ
{
	std::vector<RTLIL::Process*> result;
	result.reserve(processes.size());
	ಪ್ರತಿ (auto &it : processes)
		ಯದಿ (design->selected(ಏತತ್, it.second))
			result.ತಳ್ಳು_back(it.second);
	ಪ್ರತ್ಯಾಗಮ result;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() ಧ್ರುವ
{
	std::vector<RTLIL::SelectedMember> result;
	ಪ್ರತಿ (auto cell : selected_cells())
		result.ತಳ್ಳು_back({cell, cell->name});
	ಪ್ರತಿ (auto memory : selected_memories())
		result.ತಳ್ಳು_back({memory, memory->name});
	ಪ್ರತಿ (auto wire : selected_wires())
		result.ತಳ್ಳು_back({wire, wire->name});
	ಪ್ರತಿ (auto process : selected_processes())
		result.ತಳ್ಳು_back({process, process->name});
	ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ RTLIL::Module::add(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(ಎಣಿಕೆ_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = ಏತತ್;
}

ಶೂನ್ಯ RTLIL::Module::add(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(ಎಣಿಕೆ_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = ಏತತ್;
}

ಶೂನ್ಯ RTLIL::Module::add(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(ಎಣಿಕೆ_id(process->name) == 0);
	processes[process->name] = process;
	process->module = ಏತತ್;
}

ಶೂನ್ಯ RTLIL::Module::remove(ಧ್ರುವ pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	ಯದಿ (wires.empty())
		ಪ್ರತ್ಯಾಗಮ;

	ಸಂರಚನಾ DeleteWireWorker
	{
		RTLIL::Module *module;
		ಧ್ರುವ pool<RTLIL::Wire*> *wires_p;

		ಶೂನ್ಯ operator()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([ಏತತ್](RTLIL::Wire *&wire) {
				ಯದಿ (wires_p->ಎಣಿಕೆ(wire))
					wire = module->addWire(stringf("$delete_wire$%d", (ಪೂರ್ಣಾಙ್ಕ)autoidx++), wire->width);
			});
		}

		ಶೂನ್ಯ operator()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	DeleteWireWorker delete_wire_worker;
	delete_wire_worker.module = ಏತತ್;
	delete_wire_worker.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_worker);

	ಯದಿ (design->flagBufferedNormalized) {
		ಪ್ರತಿ (auto wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_index.erase(wire);
		}
	}

	ಪ್ರತಿ (auto &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.ಎಣಿಕೆ(id) != 0);
		wires_.erase(id);
		delete it;
	}
}

ಶೂನ್ಯ RTLIL::Module::remove(RTLIL::Cell *cell)
{
	ಯಾವತ್ (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.ಎಣಿಕೆ(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	ಯದಿ (design && design->flagBufferedNormalized && buf_norm_cell_queue.ಎಣಿಕೆ(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.ಸೇರಿಸು(cell);
	} ಅನ್ಯಥಾ {
		delete cell;
	}
}

ಶೂನ್ಯ RTLIL::Module::remove(RTLIL::Memory *memory)
{
	log_assert(memories.ಎಣಿಕೆ(memory->name) != 0);
	memories.erase(memory->name);
	delete memory;
}

ಶೂನ್ಯ RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.ಎಣಿಕೆ(process->name) != 0);
	processes.erase(process->name);
	delete process;
}

ಶೂನ್ಯ RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	add(wire);
}

ಶೂನ್ಯ RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	add(cell);
}

ಶೂನ್ಯ RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	ಯದಿ (old_id != IdString::Null && wires_.ಎಣಿಕೆ(old_id))
		rename(wires_.at(old_id), new_name);
	ಅನ್ಯಥಾ ಯದಿ (old_id != IdString::Null && cells_.ಎಣಿಕೆ(old_id))
		rename(cells_.at(old_id), new_name);
	ಅನ್ಯಥಾ
		log_abort();
}

ಶೂನ್ಯ RTLIL::Module::swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2)
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
	swap(w1->name, w2->name);
}

ಶೂನ್ಯ RTLIL::Module::swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	swap(c1->name, c2->name);
}

RTLIL::IdString RTLIL::Module::uniquify(RTLIL::IdString name)
{
	ಪೂರ್ಣಾಙ್ಕ index = 0;
	ಪ್ರತ್ಯಾಗಮ uniquify(name, index);
}

IdString RTLIL::Module::uniquify(IdString name, ಪೂರ್ಣಾಙ್ಕ &index)
{
	ಯದಿ (index == 0) {
		ಯದಿ (ಎಣಿಕೆ_id(name) == 0)
			ಪ್ರತ್ಯಾಗಮ name;
		index++;
	}

	ಯಾವತ್ (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", index)});
		ಯದಿ (ಎಣಿಕೆ_id(new_name) == 0)
			ಪ್ರತ್ಯಾಗಮ new_name;
		index++;
	}
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ fixup_ports_compare(ಧ್ರುವ RTLIL::Wire *a, ಧ್ರುವ RTLIL::Wire *b)
{
	ಯದಿ (a->port_id && !b->port_id)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಯದಿ (!a->port_id && b->port_id)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಯದಿ (a->port_id == b->port_id)
		ಪ್ರತ್ಯಾಗಮ a->name < b->name;
	ಪ್ರತ್ಯಾಗಮ a->port_id < b->port_id;
}

ಶೂನ್ಯ RTLIL::Module::connect(ಧ್ರುವ RTLIL::SigSig &conn)
{
	ಪ್ರತಿ (auto mon : monitors)
		mon->notify_connect(ಏತತ್, conn);

	ಯದಿ (design)
		ಪ್ರತಿ (auto mon : design->monitors)
			mon->notify_connect(ಏತತ್, conn);

	// ignore all attempts to assign constants to other constants
	ಯದಿ (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(conn.first); i++)
			ಯದಿ (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		ಯದಿ (GetSize(new_conn.first))
			connect(new_conn);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (yosys_xtrace) {
		log("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", ಏತತ್, log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(GetSize(conn.first) == GetSize(conn.second));
	connections_.ತಳ್ಳು_back(conn);
}

ಶೂನ್ಯ RTLIL::Module::connect(ಧ್ರುವ RTLIL::SigSpec &lhs, ಧ್ರುವ RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

ಶೂನ್ಯ RTLIL::Module::new_connections(ಧ್ರುವ std::vector<RTLIL::SigSig> &new_conn)
{
	ಪ್ರತಿ (auto mon : monitors)
		mon->notify_connect(ಏತತ್, new_conn);

	ಯದಿ (design)
		ಪ್ರತಿ (auto mon : design->monitors)
			mon->notify_connect(ಏತತ್, new_conn);

	ಯದಿ (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", ಏತತ್);
		ಪ್ರತಿ (auto &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ಧ್ರುವ std::vector<RTLIL::SigSig> &RTLIL::Module::connections() ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ connections_;
}

ಶೂನ್ಯ RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	ಪ್ರತಿ (auto &w : wires_)
		ಯದಿ (w.second->port_ಒಳಹರಿವು || w.second->port_output)
			all_ports.ತಳ್ಳು_back(w.second);
		ಅನ್ಯಥಾ
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	ಯದಿ (design && design->flagBufferedNormalized) {
		ಪ್ರತಿ (auto &w : wires_)
			ಯದಿ (w.second->driverCell_ && w.second->driverCell_->type == ID($ಒಳಹರಿವು_port))
				buf_norm_wire_queue.ಸೇರಿಸು(w.second);

		buf_norm_wire_queue.ಸೇರಿಸು(all_ports.begin(), all_ports.end());
	}

	ports.clear();
	ಪ್ರತಿ (size_t i = 0; i < all_ports.size(); i++) {
		ports.ತಳ್ಳು_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
	}
}

ಶೂನ್ಯ RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		ಧ್ರುವ dict<IdString, RTLIL::Const> &src,
		ಧ್ರುವ RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	ಯದಿ (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		ಪ್ರತ್ಯಾಗಮ;
	}
	dst.clear();
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = GetSize(src) - 1; i >= 0; i--) {
		auto &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::addWire(RTLIL::IdString name, ಪೂರ್ಣಾಙ್ಕ width)
{
	log_assert(design);
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	RTLIL::Wire *wire = ನೂತನ RTLIL::Wire;
	wire->width = width;
	wire->name = name;
	add(wire);
	ಪ್ರತ್ಯಾಗಮ wire;
}

RTLIL::Wire *RTLIL::Module::addWire(IdString name, ಧ್ರುವ RTLIL::Wire *other)
{
	RTLIL::Wire *wire = addWire(name);
	wire->width = other->width;
	wire->start_offset = other->start_offset;
	wire->port_id = other->port_id;
	wire->port_ಒಳಹರಿವು = other->port_ಒಳಹರಿವು;
	wire->port_output = other->port_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ಧ್ರುವ RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
		RTLIL::copy_attr_dict(wire->attributes, other->attributes, src_design, ಏತತ್->design);
	}
	ಪ್ರತ್ಯಾಗಮ wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = ನೂತನ RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, ಧ್ರುವ RTLIL::Cell *other)
{
	ಧ್ರುವ RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	ತಾರ್ಕಿಕ cross_pool = src_design && ಏತತ್->design && src_design != ಏತತ್->design;

	IdString type = other->type.ref();
	ಯದಿ (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, ಏತತ್->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, ಏತತ್->design);

	ಯದಿ (!cross_pool)
		cell->connections_ = other->connections_;
	ಅನ್ಯಥಾ
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = GetSize(other->connections_) - 1; i >= 0; i--) {
			auto &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = ನೂತನ RTLIL::Memory;
	mem->module = ಏತತ್;
	mem->name = name;
	memories[name] = mem;
	ಪ್ರತ್ಯಾಗಮ mem;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name, ಧ್ರುವ RTLIL::Memory *other)
{
	log_assert(design);
	RTLIL::Memory *mem = ನೂತನ RTLIL::Memory;
	mem->module = ಏತತ್;
	mem->name = name;
	mem->width = other->width;
	mem->start_offset = other->start_offset;
	mem->size = other->size;
	RTLIL::copy_attr_dict(mem->attributes, other->attributes,
			other->module ? other->module->design : nullptr, design);
	memories[name] = mem;
	ಪ್ರತ್ಯಾಗಮ mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = ನೂತನ RTLIL::Process;
	proc->name = name;
	add(proc);
	ಪ್ರತ್ಯಾಗಮ proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, ಧ್ರುವ RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ಧ್ರುವ RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	ಯದಿ (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		ಪ್ರತಿ (auto *ಸಮಕಾಲ : proc->syncs)
			ಪ್ರತಿ (auto &mwa : ಸಮಕಾಲ->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	ಪ್ರತ್ಯಾಗಮ proc;
}

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, ತಾರ್ಕಿಕ is_signed, ಧ್ರುವ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		ಪ್ರತ್ಯಾಗಮ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ತಾರ್ಕಿಕ is_signed, ಧ್ರುವ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		ಪ್ರತ್ಯಾಗಮ sig_y;                                       \
	}
DEF_METHOD(Not,        sig_a.size(), ID($not))
DEF_METHOD(Pos,        sig_a.size(), ID($pos))
DEF_METHOD(Neg,        sig_a.size(), ID($neg))
DEF_METHOD(ReduceAnd,  1, ID($reduce_and))
DEF_METHOD(ReduceOr,   1, ID($reduce_or))
DEF_METHOD(ReduceXor,  1, ID($reduce_xor))
DEF_METHOD(ReduceXnor, 1, ID($reduce_xnor))
DEF_METHOD(ReduceBool, 1, ID($reduce_bool))
DEF_METHOD(LogicNot,   1, ID($logic_not))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, ತಾರ್ಕಿಕ /* is_signed */, ಧ್ರುವ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::WIDTH] = sig_a.size();         \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		ಪ್ರತ್ಯಾಗಮ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ತಾರ್ಕಿಕ is_signed, ಧ್ರುವ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		ಪ್ರತ್ಯಾಗಮ sig_y;                                       \
	}
DEF_METHOD(Buf,     sig_a.size(), ID($buf))
DEF_METHOD(Barrier, sig_a.size(), ID($barrier))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, ತಾರ್ಕಿಕ is_signed, ಧ್ರುವ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		ಪ್ರತ್ಯಾಗಮ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ತಾರ್ಕಿಕ is_signed, ಧ್ರುವ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ಪ್ರತ್ಯಾಗಮ sig_y;                                            \
	}
DEF_METHOD(And,      max(sig_a.size(), sig_b.size()), ID($and))
DEF_METHOD(Or,       max(sig_a.size(), sig_b.size()), ID($or))
DEF_METHOD(Xor,      max(sig_a.size(), sig_b.size()), ID($xor))
DEF_METHOD(Xnor,     max(sig_a.size(), sig_b.size()), ID($xnor))
DEF_METHOD(Shift,    sig_a.size(), ID($shift))
DEF_METHOD(Lt,       1, ID($lt))
DEF_METHOD(Le,       1, ID($le))
DEF_METHOD(Eq,       1, ID($eq))
DEF_METHOD(Ne,       1, ID($ne))
DEF_METHOD(Eqx,      1, ID($eqx))
DEF_METHOD(Nex,      1, ID($nex))
DEF_METHOD(Ge,       1, ID($ge))
DEF_METHOD(Gt,       1, ID($gt))
DEF_METHOD(Add,      max(sig_a.size(), sig_b.size()), ID($add))
DEF_METHOD(Sub,      max(sig_a.size(), sig_b.size()), ID($sub))
DEF_METHOD(Mul,      max(sig_a.size(), sig_b.size()), ID($mul))
DEF_METHOD(Div,      max(sig_a.size(), sig_b.size()), ID($div))
DEF_METHOD(Mod,      max(sig_a.size(), sig_b.size()), ID($mod))
DEF_METHOD(DivFloor, max(sig_a.size(), sig_b.size()), ID($divfloor))
DEF_METHOD(ModFloor, max(sig_a.size(), sig_b.size()), ID($modfloor))
DEF_METHOD(LogicAnd, 1, ID($logic_and))
DEF_METHOD(LogicOr,  1, ID($logic_or))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, ತಾರ್ಕಿಕ is_signed, ಧ್ರುವ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = ಅಸತ್ಯ;             \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		ಪ್ರತ್ಯಾಗಮ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ತಾರ್ಕಿಕ is_signed, ಧ್ರುವ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ಪ್ರತ್ಯಾಗಮ sig_y;                                            \
	}
DEF_METHOD(Shl,      sig_a.size(), ID($shl))
DEF_METHOD(Shr,      sig_a.size(), ID($shr))
DEF_METHOD(Sshl,     sig_a.size(), ID($sshl))
DEF_METHOD(Sshr,     sig_a.size(), ID($sshr))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, ತಾರ್ಕಿಕ is_signed, ಧ್ರುವ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = ಅಸತ್ಯ;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		ಪ್ರತ್ಯಾಗಮ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ತಾರ್ಕಿಕ is_signed, ಧ್ರುವ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ಪ್ರತ್ಯಾಗಮ sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.size(), ID($shiftx))
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		ಯದಿ (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.size();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		ಪ್ರತ್ಯಾಗಮ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		ಪ್ರತ್ಯಾಗಮ sig_y;                                             \
	}
DEF_METHOD(Mux,      ID($mux),        0)
DEF_METHOD(Bwmux,    ID($bwmux),      0)
DEF_METHOD(Pmux,     ID($pmux),       1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = _demux ? sig_a.size() : sig_y.size(); \
		cell->parameters[ID::S_WIDTH] = sig_s.size();             \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		ಪ್ರತ್ಯಾಗಮ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.size() << sig_s.size() : sig_a.size() >> sig_s.size()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		ಪ್ರತ್ಯಾಗಮ sig_y;                                             \
	}
DEF_METHOD(Bmux,     ID($bmux),       0)
DEF_METHOD(Demux,    ID($demux),      1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		ಪ್ರತ್ಯಾಗಮ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		ಪ್ರತ್ಯಾಗಮ sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#undef DEF_METHOD

#define DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig1, ಧ್ರುವ RTLIL::SigBit &sig2, ಧ್ರುವ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		ಪ್ರತ್ಯಾಗಮ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig1, ಧ್ರುವ std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		ಪ್ರತ್ಯಾಗಮ sig2;                                      \
	}
#define DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig1, ಧ್ರುವ RTLIL::SigBit &sig2, ಧ್ರುವ RTLIL::SigBit &sig3, ಧ್ರುವ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		ಪ್ರತ್ಯಾಗಮ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig1, ಧ್ರುವ RTLIL::SigBit &sig2, ಧ್ರುವ std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		ಪ್ರತ್ಯಾಗಮ sig3;                                      \
	}
#define DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig1, ಧ್ರುವ RTLIL::SigBit &sig2, ಧ್ರುವ RTLIL::SigBit &sig3, ಧ್ರುವ RTLIL::SigBit &sig4, ಧ್ರುವ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		ಪ್ರತ್ಯಾಗಮ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig1, ಧ್ರುವ RTLIL::SigBit &sig2, ಧ್ರುವ RTLIL::SigBit &sig3, ಧ್ರುವ std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		ಪ್ರತ್ಯಾಗಮ sig4;                                      \
	}
#define DEF_METHOD_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig1, ಧ್ರುವ RTLIL::SigBit &sig2, ಧ್ರುವ RTLIL::SigBit &sig3, ಧ್ರುವ RTLIL::SigBit &sig4, ಧ್ರುವ RTLIL::SigBit &sig5, ಧ್ರುವ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->setPort("\\" #_P5, sig5);                   \
		cell->set_src_attribute(src);                     \
		ಪ್ರತ್ಯಾಗಮ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig1, ಧ್ರುವ RTLIL::SigBit &sig2, ಧ್ರುವ RTLIL::SigBit &sig3, ಧ್ರುವ RTLIL::SigBit &sig4, ಧ್ರುವ std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		ಪ್ರತ್ಯಾಗಮ sig5;                                           \
	}
DEF_METHOD_2(BufGate,    ID($_BUF_),    A, Y)
DEF_METHOD_2(NotGate,    ID($_NOT_),    A, Y)
DEF_METHOD_3(AndGate,    ID($_AND_),    A, B, Y)
DEF_METHOD_3(NandGate,   ID($_NAND_),   A, B, Y)
DEF_METHOD_3(OrGate,     ID($_OR_),     A, B, Y)
DEF_METHOD_3(NorGate,    ID($_NOR_),    A, B, Y)
DEF_METHOD_3(XorGate,    ID($_XOR_),    A, B, Y)
DEF_METHOD_3(XnorGate,   ID($_XNOR_),   A, B, Y)
DEF_METHOD_3(AndnotGate, ID($_ANDNOT_), A, B, Y)
DEF_METHOD_3(OrnotGate,  ID($_ORNOT_),  A, B, Y)
DEF_METHOD_4(MuxGate,    ID($_MUX_),    A, B, S, Y)
DEF_METHOD_4(NmuxGate,   ID($_NMUX_),   A, B, S, Y)
DEF_METHOD_4(Aoi3Gate,   ID($_AOI3_),   A, B, C, Y)
DEF_METHOD_4(Oai3Gate,   ID($_OAI3_),   A, B, C, Y)
DEF_METHOD_5(Aoi4Gate,   ID($_AOI4_),   A, B, C, D, Y)
DEF_METHOD_5(Oai4Gate,   ID($_OAI4_),   A, B, C, D, Y)
#undef DEF_METHOD_2
#undef DEF_METHOD_3
#undef DEF_METHOD_4
#undef DEF_METHOD_5

RTLIL::Cell* RTLIL::Module::addPow(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, ತಾರ್ಕಿಕ a_signed, ತಾರ್ಕಿಕ b_signed, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($pow));
	cell->parameters[ID::A_SIGNED] = a_signed;
	cell->parameters[ID::B_SIGNED] = b_signed;
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::B_WIDTH] = sig_b.size();
	cell->parameters[ID::Y_WIDTH] = sig_y.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addFa(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_c, ಧ್ರುವ RTLIL::SigSpec &sig_x, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fa));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::C, sig_c);
	cell->setPort(ID::X, sig_x);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addSlice(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, RTLIL::Const offset, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($slice));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::Y_WIDTH] = sig_y.size();
	cell->parameters[ID::OFFSET] = offset;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addConcat(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($concat));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::B_WIDTH] = sig_b.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, RTLIL::Const lut, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ಪ್ರತಿಜ್ಞಾ));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addSr(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr, ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ set_polarity, ತಾರ್ಕಿಕ clr_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sr));
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addDff(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addDffe(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addDffsr(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ set_polarity, ತಾರ್ಕಿಕ clr_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dffsr));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addDffsre(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ set_polarity, ತಾರ್ಕಿಕ clr_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dffsre));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addAdff(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_arst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ arst_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ಮೌಲ್ಯ] = arst_ಮೌಲ್ಯ;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addAdffe(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_arst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ arst_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ಮೌಲ್ಯ] = arst_ಮೌಲ್ಯ;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addAldff(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_aload, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		ಧ್ರುವ RTLIL::SigSpec &sig_ad, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ aload_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($aldff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::ALOAD, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addAldffe(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_aload, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		ಧ್ರುವ RTLIL::SigSpec &sig_ad, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ aload_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($aldffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ALOAD, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addSdff(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_srst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ srst_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ಮೌಲ್ಯ] = srst_ಮೌಲ್ಯ;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffe(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_srst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ srst_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ಮೌಲ್ಯ] = srst_ಮೌಲ್ಯ;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffce(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_srst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ srst_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ಮೌಲ್ಯ] = srst_ಮೌಲ್ಯ;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addDlatch(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ en_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatch(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_arst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ arst_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ಮೌಲ್ಯ] = arst_ಮೌಲ್ಯ;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsr(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ set_polarity, ತಾರ್ಕಿಕ clr_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatchsr));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr,
		ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ set_polarity, ತಾರ್ಕಿಕ clr_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addDffsrGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ set_polarity, ತಾರ್ಕಿಕ clr_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addDffsreGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ set_polarity, ತಾರ್ಕಿಕ clr_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addAdffGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_arst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		ತಾರ್ಕಿಕ arst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ arst_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ಮೌಲ್ಯ ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addAdffeGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_arst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		ತಾರ್ಕಿಕ arst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ arst_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ಮೌಲ್ಯ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addAldffGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_aload, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		ಧ್ರುವ RTLIL::SigSpec &sig_ad, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ aload_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addAldffeGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_aload, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		ಧ್ರುವ RTLIL::SigSpec &sig_ad, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ aload_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_srst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		ತಾರ್ಕಿಕ srst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ srst_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ಮೌಲ್ಯ ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffeGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_srst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		ತಾರ್ಕಿಕ srst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ srst_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ಮೌಲ್ಯ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffceGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_srst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		ತಾರ್ಕಿಕ srst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ clk_polarity, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ srst_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ಮೌಲ್ಯ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ en_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatchGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_arst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
		ತಾರ್ಕಿಕ arst_ಮೌಲ್ಯ, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ arst_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ಮೌಲ್ಯ ? '1' : '0'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsrGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ತಾರ್ಕಿಕ en_polarity, ತಾರ್ಕಿಕ set_polarity, ತಾರ್ಕಿಕ clr_polarity, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, ಪೂರ್ಣಾಙ್ಕ width, ಧ್ರುವ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, ಪೂರ್ಣಾಙ್ಕ width, ಧ್ರುವ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, ಪೂರ್ಣಾಙ್ಕ width, ಧ್ರುವ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, ಪೂರ್ಣಾಙ್ಕ width, ಧ್ರುವ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ಧ್ರುವ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ sig;
}

RTLIL::SigSpec RTLIL::Module::SetTag(RTLIL::IdString name, ಧ್ರುವ std::string &tag, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ RTLIL::SigSpec &sig_c, ಧ್ರುವ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($set_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ sig;
}

RTLIL::Cell* RTLIL::Module::addSetTag(RTLIL::IdString name, ಧ್ರುವ std::string &tag, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ RTLIL::SigSpec &sig_c, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src)
{
	Cell *cell = addCell(name, ID($set_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::SigSpec RTLIL::Module::GetTag(RTLIL::IdString name, ಧ್ರುವ std::string &tag, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($get_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ sig;
}

RTLIL::Cell* RTLIL::Module::addOverwriteTag(RTLIL::IdString name, ಧ್ರುವ std::string &tag, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ RTLIL::SigSpec &sig_c, ಧ್ರುವ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($overwrite_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ cell;
}

RTLIL::SigSpec RTLIL::Module::OriginalTag(RTLIL::IdString name, ಧ್ರುವ std::string &tag, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($original_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_e, ಧ್ರುವ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.size());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.size();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ಪ್ರತ್ಯಾಗಮ sig;
}

std::string RTLIL::Module::to_rtlil_str() ಧ್ರುವ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(ಏತತ್), design, ಅಸತ್ಯ, ಸತ್ಯ, ಅಸತ್ಯ, RTLIL_BACKEND::DumpMode::Readable);
	ಪ್ರತ್ಯಾಗಮ f.str();
}

RTLIL::Wire::Wire()
{
	ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ hashidx_ಎಣಿಕೆ = 123456789;
	hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
	hashidx_ = hashidx_ಎಣಿಕೆ;

	module = nullptr;
	width = 1;
	start_offset = 0;
	port_id = 0;
	port_ಒಳಹರಿವು = ಅಸತ್ಯ;
	port_output = ಅಸತ್ಯ;
	upto = ಅಸತ್ಯ;
	is_signed = ಅಸತ್ಯ;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->ಸೇರಿಸು(std::pair<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Wire*>(hashidx_, ಏತತ್));
#endif
}

RTLIL::Wire::~Wire()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#endif
}

std::string RTLIL::Wire::to_rtlil_str() ಧ್ರುವ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", ಏತತ್, design(), RTLIL_BACKEND::DumpMode::Readable);
	ಪ್ರತ್ಯಾಗಮ f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
ಸ್ಥಿರ std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Wire*> all_wires;
std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ &all_wires;
}
#endif

RTLIL::Memory::Memory()
{
	ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ hashidx_ಎಣಿಕೆ = 123456789;
	hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
	hashidx_ = hashidx_ಎಣಿಕೆ;

	width = 1;
	start_offset = 0;
	size = 0;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->ಸೇರಿಸು(std::pair<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Memory*>(hashidx_, ಏತತ್));
#endif
}

std::string RTLIL::Memory::to_rtlil_str() ಧ್ರುವ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", ಏತತ್, design(), RTLIL_BACKEND::DumpMode::Readable);
	ಪ್ರತ್ಯಾಗಮ f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ hashidx_ಎಣಿಕೆ = 123456789;
	hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
	hashidx_ = hashidx_ಎಣಿಕೆ;
}

std::string RTLIL::Process::to_rtlil_str() ಧ್ರುವ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", ಏತತ್, design(), RTLIL_BACKEND::DumpMode::Readable);
	ಪ್ರತ್ಯಾಗಮ f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ hashidx_ಎಣಿಕೆ = 123456789;
	hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
	hashidx_ = hashidx_ಎಣಿಕೆ;

	// log("#memtrace# %p\n", this);
	memhasher();

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->ಸೇರಿಸು(std::pair<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Cell*>(hashidx_, ಏತತ್));
#endif
}

RTLIL::Cell::~Cell()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#endif
}

std::string RTLIL::Cell::to_rtlil_str() ಧ್ರುವ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", ಏತತ್, design(), RTLIL_BACKEND::DumpMode::Readable);
	ಪ್ರತ್ಯಾಗಮ f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
ಸ್ಥಿರ std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Cell*> all_cells;
std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ &all_cells;
}
#endif

ತಾರ್ಕಿಕ RTLIL::Cell::hasPort(RTLIL::IdString portname) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ connections_.ಎಣಿಕೆ(portname) != 0;
}

// bufnorm

ಧ್ರುವ RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ connections_.at(portname);
}

ಧ್ರುವ dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ connections_;
}

ತಾರ್ಕಿಕ RTLIL::Cell::known() ಧ್ರುವ
{
	ಯದಿ (yosys_celltypes.cell_known(type))
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಯದಿ (module && module->design && module->design->module(type))
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::Cell::ಒಳಹರಿವು(IdString portname) ಧ್ರುವ
{
	ಯದಿ (yosys_celltypes.cell_known(type))
		ಪ್ರತ್ಯಾಗಮ yosys_celltypes.cell_ಒಳಹರಿವು(type.ref(), portname);
	ಯದಿ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		ಪ್ರತ್ಯಾಗಮ w && w->port_ಒಳಹರಿವು;
	}
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::Cell::output(RTLIL::IdString portname) ಧ್ರುವ
{
	ಯದಿ (yosys_celltypes.cell_known(type))
		ಪ್ರತ್ಯಾಗಮ yosys_celltypes.cell_output(type, portname);
	ಯದಿ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		ಪ್ರತ್ಯಾಗಮ w && w->port_output;
	}
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) ಧ್ರುವ
{
	ಯದಿ (yosys_celltypes.cell_known(type.ref()))
		ಪ್ರತ್ಯಾಗಮ yosys_celltypes.cell_port_dir(type, portname);
	ಯದಿ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		ಯದಿ (m == nullptr)
			ಪ್ರತ್ಯಾಗಮ PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		ಯದಿ (w == nullptr)
			ಪ್ರತ್ಯಾಗಮ PortDir::PD_UNKNOWN;
		ಪ್ರತ್ಯಾಗಮ PortDir(w->port_ಒಳಹರಿವು + w->port_output * 2);
	}
	ಪ್ರತ್ಯಾಗಮ PortDir::PD_UNKNOWN;
}

ತಾರ್ಕಿಕ RTLIL::Cell::hasParam(RTLIL::IdString paramname) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ parameters.ಎಣಿಕೆ(paramname) != 0;
}

ಶೂನ್ಯ RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

ಶೂನ್ಯ RTLIL::Cell::setParam(RTLIL::IdString paramname, RTLIL::Const ಮೌಲ್ಯ)
{
	parameters[paramname] = std::move(ಮೌಲ್ಯ);
}

ಧ್ರುವ RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ಧ್ರುವ
{
	ಧ್ರುವ auto &it = parameters.find(paramname);
	ಯದಿ (it != parameters.end())
		ಪ್ರತ್ಯಾಗಮ it->second;
	ಯದಿ (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		ಯದಿ (m)
			ಪ್ರತ್ಯಾಗಮ m->parameter_default_ಮೌಲ್ಯಗಳು.at(paramname);
	}
	ಕ್ಷಿಪ std::out_of_range("Cell::getParam()");
}

ಶೂನ್ಯ RTLIL::Cell::sort()
{
	connections_.sort(sort_by_id_str(module->twines()));
	parameters.sort(sort_by_id_str(module->twines()));
	attributes.sort(sort_by_id_str(module->twines()));
}

ಶೂನ್ಯ RTLIL::Cell::check()
{
#ifndef NDEBUG
	InternalCellChecker checker(ರಿಕ್ತ, ಏತತ್);
	checker.check();
#endif
}

ಶೂನ್ಯ RTLIL::Cell::fixup_parameters(ತಾರ್ಕಿಕ set_a_signed, ತಾರ್ಕಿಕ set_b_signed)
{
	ಯದಿ (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		ಯದಿ (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (type == ID($demux)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (type == ID($fa)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (type == ID($lcu)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::CO]);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ತಾರ್ಕಿಕ signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	ಯದಿ (connections_.ಎಣಿಕೆ(ID::A)) {
		ಯದಿ (signedness_ab) {
			ಯದಿ (set_a_signed)
				parameters[ID::A_SIGNED] = ಸತ್ಯ;
			ಅನ್ಯಥಾ ಯದಿ (parameters.ಎಣಿಕೆ(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = ಅಸತ್ಯ;
		}
		parameters[ID::A_WIDTH] = GetSize(connections_[ID::A]);
	}

	ಯದಿ (connections_.ಎಣಿಕೆ(ID::B)) {
		ಯದಿ (signedness_ab) {
			ಯದಿ (set_b_signed)
				parameters[ID::B_SIGNED] = ಸತ್ಯ;
			ಅನ್ಯಥಾ ಯದಿ (parameters.ಎಣಿಕೆ(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = ಅಸತ್ಯ;
		}
		parameters[ID::B_WIDTH] = GetSize(connections_[ID::B]);
	}

	ಯದಿ (connections_.ಎಣಿಕೆ(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);

	ಯದಿ (connections_.ಎಣಿಕೆ(ID::Q))
		parameters[ID::WIDTH] = GetSize(connections_[ID::Q]);

	check();
}

ತಾರ್ಕಿಕ RTLIL::Cell::has_keep_attr() ಧ್ರುವ {
	ಪ್ರತ್ಯಾಗಮ get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

ತಾರ್ಕಿಕ RTLIL::Cell::has_memid() ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

ತಾರ್ಕಿಕ RTLIL::Cell::is_mem_cell() ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ type.in(ID($mem), ID($mem_v2)) || has_memid();
}

ತಾರ್ಕಿಕ RTLIL::Cell::is_builtin_ff() ಧ್ರುವ {
	ಪ್ರತ್ಯಾಗಮ StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ಧ್ರುವ RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	ಯದಿ (wire == ರಿಕ್ತ)
		data = {bit.data};
	ಅನ್ಯಥಾ
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ length) ಧ್ರುವ
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= width);
	RTLIL::SigChunk ret;
	ಯದಿ (wire) {
		ret.wire = wire;
		ret.offset = ಏತತ್->offset + offset;
		ret.width = length;
	} ಅನ್ಯಥಾ {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < length; i++)
			ret.data.ತಳ್ಳು_back(data[offset+i]);
		ret.width = length;
	}
	ಪ್ರತ್ಯಾಗಮ ret;
}

RTLIL::SigBit RTLIL::SigChunk::operator[](ಪೂರ್ಣಾಙ್ಕ offset) ಧ್ರುವ
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	ಯದಿ (wire) {
		ret.wire = wire;
		ret.offset = ಏತತ್->offset + offset;
	} ಅನ್ಯಥಾ {
		ret.data = data[offset];
	}
	ಪ್ರತ್ಯಾಗಮ ret;
}

ತಾರ್ಕಿಕ RTLIL::SigChunk::operator <(ಧ್ರುವ RTLIL::SigChunk &other) ಧ್ರುವ
{
	ಯದಿ (wire && other.wire)
		ಯದಿ (wire->name != other.wire->name)
			ಪ್ರತ್ಯಾಗಮ wire->name < other.wire->name;

	ಯದಿ (wire != other.wire)
		ಪ್ರತ್ಯಾಗಮ wire < other.wire;

	ಯದಿ (offset != other.offset)
		ಪ್ರತ್ಯಾಗಮ offset < other.offset;

	ಯದಿ (width != other.width)
		ಪ್ರತ್ಯಾಗಮ width < other.width;

	ಪ್ರತ್ಯಾಗಮ data < other.data;
}

ತಾರ್ಕಿಕ RTLIL::SigChunk::operator ==(ಧ್ರುವ RTLIL::SigChunk &other) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ wire == other.wire && width == other.width && offset == other.offset && data == other.data;
}

ತಾರ್ಕಿಕ RTLIL::SigChunk::operator !=(ಧ್ರುವ RTLIL::SigChunk &other) ಧ್ರುವ
{
	ಯದಿ (*ಏತತ್ == other)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.size() > 0);
	auto ie = parts.begin();
	auto it = ie + parts.size() - 1;
	ಯಾವತ್ (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ಧ್ರುವ RTLIL::Const &ಮೌಲ್ಯ)
{
	ಯದಿ (GetSize(ಮೌಲ್ಯ) != 0) {
		rep_ = CHUNK;
		ನೂತನ (&chunk_) RTLIL::SigChunk(ಮೌಲ್ಯ);
	} ಅನ್ಯಥಾ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&ಮೌಲ್ಯ)
{
	ಯದಿ (GetSize(ಮೌಲ್ಯ) != 0) {
		rep_ = CHUNK;
		ನೂತನ (&chunk_) RTLIL::SigChunk(ಮೌಲ್ಯ);
	} ಅನ್ಯಥಾ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ಧ್ರುವ RTLIL::SigChunk &chunk)
{
	ಯದಿ (chunk.width != 0) {
		rep_ = CHUNK;
		ನೂತನ (&chunk_) RTLIL::SigChunk(chunk);
	} ಅನ್ಯಥಾ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	ಯದಿ (chunk.width != 0) {
		rep_ = CHUNK;
		ನೂತನ (&chunk_) RTLIL::SigChunk(chunk);
	} ಅನ್ಯಥಾ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	ಯದಿ (wire->width != 0) {
		rep_ = CHUNK;
		ನೂತನ (&chunk_) RTLIL::SigChunk(wire);
	} ಅನ್ಯಥಾ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ width)
{
	ಯದಿ (width != 0) {
		rep_ = CHUNK;
		ನೂತನ (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} ಅನ್ಯಥಾ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ಧ್ರುವ std::string &str)
{
	ಯದಿ (str.size() != 0) {
		rep_ = CHUNK;
		ನೂತನ (&chunk_) RTLIL::SigChunk(str);
	} ಅನ್ಯಥಾ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ಪೂರ್ಣಾಙ್ಕ val, ಪೂರ್ಣಾಙ್ಕ width)
{
	ಯದಿ (width != 0) {
		rep_ = CHUNK;
		ನೂತನ (&chunk_) RTLIL::SigChunk(val, width);
	} ಅನ್ಯಥಾ
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, ಪೂರ್ಣಾಙ್ಕ width)
{
	ಯದಿ (width != 0) {
		rep_ = CHUNK;
		ನೂತನ (&chunk_) RTLIL::SigChunk(bit, width);
	} ಅನ್ಯಥಾ
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ಧ್ರುವ RTLIL::SigBit &bit, ಪೂರ್ಣಾಙ್ಕ width)
{
	ಯದಿ (width != 0) {
		ಯದಿ (bit.wire == ರಿಕ್ತ) {
			rep_ = CHUNK;
			ನೂತನ (&chunk_) RTLIL::SigChunk(bit.data, width);
		} ಅನ್ಯಥಾ ಯದಿ (width == 1) {
			rep_ = CHUNK;
			ನೂತನ (&chunk_) RTLIL::SigChunk(bit);
		} ಅನ್ಯಥಾ {
			init_empty_bits();
			bits_.reserve(width);
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++)
				bits_.ತಳ್ಳು_back(bit);
		}
	} ಅನ್ಯಥಾ
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ಧ್ರುವ std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	ಪ್ರತಿ (ಧ್ರುವ auto &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ಧ್ರುವ std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	ಪ್ರತಿ (ಧ್ರುವ auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ಧ್ರುವ pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	ಪ್ರತಿ (ಧ್ರುವ auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ಧ್ರುವ std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	ಪ್ರತಿ (ಧ್ರುವ auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ತಾರ್ಕಿಕ bit)
{
	rep_ = CHUNK;
	ನೂತನ (&chunk_) RTLIL::SigChunk(bit ? RTLIL::S1 : RTLIL::S0);
	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::Chunks::const_iterator::next_chunk_bits()
{
	ಪೂರ್ಣಾಙ್ಕ bits_size = GetSize(spec.bits_);
	ಯದಿ (bit_index >= bits_size)
		ಪ್ರತ್ಯಾಗಮ;
	ಪೂರ್ಣಾಙ್ಕ i = bit_index;
	ಧ್ರುವ SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.data.clear();
	ಯದಿ (bit.is_wire()) {
		chunk.offset = bit.offset;
		ಯಾವತ್ (i < bits_size && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_index)
			++i;
	} ಅನ್ಯಥಾ {
		chunk.offset = 0;
		chunk.data.ತಳ್ಳು_back(bit.data);
		ಯಾವತ್ (i < bits_size && !spec.bits_[i].is_wire()) {
			chunk.data.ತಳ್ಳು_back(spec.bits_[i].data);
			++i;
		}
	}
	chunk.width = i - bit_index;
}

ಶೂನ್ಯ RTLIL::SigSpec::unpack()
{
	ಯದಿ (rep_ == BITS)
		ಪ್ರತ್ಯಾಗಮ;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	ನೂತನ (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

ಶೂನ್ಯ RTLIL::SigSpec::try_repack()
{
	ಯದಿ (rep_ != BITS)
		ಪ್ರತ್ಯಾಗಮ;

	ಪೂರ್ಣಾಙ್ಕ bits_size = GetSize(bits_);
	ಯದಿ (bits_size == 0)
		ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (bits_[0].is_wire()) {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < bits_size; i++)
			ಯದಿ (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				ಪ್ರತ್ಯಾಗಮ;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_size);
		bits_.~vector();
		rep_ = CHUNK;
		ನೂತನ (&chunk_) SigChunk(std::move(chunk));
		ಪ್ರತ್ಯಾಗಮ;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_size);
	bits.ತಳ್ಳು_back(bits_[0].data);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < bits_size; i++) {
		ಯದಿ (bits_[i].is_wire())
			ಪ್ರತ್ಯಾಗಮ;
		bits.ತಳ್ಳು_back(bits_[i].data);
	}
	bits_.~vector();
	rep_ = CHUNK;
	ನೂತನ (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ಧ್ರುವ
{
	Hasher h;
	ಪ್ರತಿ (auto &c : chunks())
		ಯದಿ (c.wire == ರಿಕ್ತ) {
			ಪ್ರತಿ (auto &v : c.data)
				h.eat(v);
		} ಅನ್ಯಥಾ {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t result = h.yield();
	ಯದಿ (result == 0)
		result = 1;
	hash_.set(result);
	ಪ್ರತ್ಯಾಗಮ result;
}

ಶೂನ್ಯ RTLIL::SigSpec::sort()
{
	unpack();
	std::sort(bits_.begin(), bits_.end());
	hash_.clear();
	try_repack();
}

ಶೂನ್ಯ RTLIL::SigSpec::sort_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::vector<SigBit> unique_bits = bits_;
	std::sort(unique_bits.begin(), unique_bits.end());
	auto last = std::unique(unique_bits.begin(), unique_bits.end());
	unique_bits.erase(last, unique_bits.end());

	*ಏತತ್ = unique_bits;
	hash_.clear();
	try_repack();
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಧ್ರುವ RTLIL::SigSpec &pattern, ಧ್ರುವ RTLIL::SigSpec &with)
{
	replace(pattern, with, ಏತತ್);
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಧ್ರುವ RTLIL::SigSpec &pattern, ಧ್ರುವ RTLIL::SigSpec &with, RTLIL::SigSpec *other) ಧ್ರುವ
{
	log_assert(other != ರಿಕ್ತ);
	log_assert(size() == other->size());
	log_assert(pattern.size() == with.size());

	dict<RTLIL::SigBit, ಪೂರ್ಣಾಙ್ಕ> pattern_to_with;
	ಪೂರ್ಣಾಙ್ಕ pattern_size = pattern.size();
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < pattern_size; i++) {
		SigBit pattern_bit = pattern[i];
		ಯದಿ (pattern_bit.wire != ರಿಕ್ತ) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	ಪೂರ್ಣಾಙ್ಕ this_size = size();
	ತಾರ್ಕಿಕ other_modified = ಅಸತ್ಯ;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < this_size; j++) {
		auto it = pattern_to_with.find((*ಏತತ್)[j]);
		ಯದಿ (it != pattern_to_with.end()) {
			other_modified = ಸತ್ಯ;
			(*other)[j] = with[it->second];
		}
	}

	ಯದಿ (other_modified)
		other->try_repack();
	other->check();
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಧ್ರುವ dict<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, ಏತತ್);
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಧ್ರುವ dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ಧ್ರುವ
{
	log_assert(other != ರಿಕ್ತ);
	log_assert(size() == other->size());

	ಯದಿ (rules.empty()) ಪ್ರತ್ಯಾಗಮ;

	ಪೂರ್ಣಾಙ್ಕ this_size = size();
	ತಾರ್ಕಿಕ other_modified = ಅಸತ್ಯ;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < this_size; i++) {
		auto it = rules.find((*ಏತತ್)[i]);
		ಯದಿ (it != rules.end()) {
			other_modified = ಸತ್ಯ;
			(*other)[i] = it->second;
		}
	}

	ಯದಿ (other_modified)
		other->try_repack();
	other->check();
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಧ್ರುವ std::map<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, ಏತತ್);
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಧ್ರುವ std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ಧ್ರುವ
{
	log_assert(other != ರಿಕ್ತ);
	log_assert(size() == other->size());

	ಯದಿ (rules.empty()) ಪ್ರತ್ಯಾಗಮ;

	ಪೂರ್ಣಾಙ್ಕ this_size = size();
	ತಾರ್ಕಿಕ other_modified = ಅಸತ್ಯ;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < this_size; i++) {
		auto it = rules.find((*ಏತತ್)[i]);
		ಯದಿ (it != rules.end()) {
			other_modified = ಸತ್ಯ;
			(*other)[i] = it->second;
		}
	}

	ಯದಿ (other_modified)
		other->try_repack();
	other->check();
}

ಶೂನ್ಯ RTLIL::SigSpec::remove(ಧ್ರುವ RTLIL::SigSpec &pattern)
{
	remove2(pattern, ರಿಕ್ತ);
}

ಶೂನ್ಯ RTLIL::SigSpec::remove(ಧ್ರುವ RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) ಧ್ರುವ
{
	RTLIL::SigSpec tmp = *ಏತತ್;
	tmp.remove2(pattern, other);
}

ಶೂನ್ಯ RTLIL::SigSpec::remove2(ಧ್ರುವ RTLIL::SigSpec &pattern, RTLIL::SigSpec *other)
{
	unpack();
	ಯದಿ (other != ರಿಕ್ತ) {
		log_assert(size() == other->size());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.size());
	ಪ್ರತಿ (auto &bit : pattern)
		ಯದಿ (bit.wire != ರಿಕ್ತ)
			pattern_bits.ಸೇರಿಸು(bit);

	// Compact in-place to avoid O(n^2) erase operations
	size_t write_idx = 0;
	ಪ್ರತಿ (size_t read_idx = 0; read_idx < bits_.size(); read_idx++)
	{
		ಯದಿ (!(bits_[read_idx].wire != ರಿಕ್ತ && pattern_bits.ಎಣಿಕೆ(bits_[read_idx]))) {
			ಯದಿ (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				ಯದಿ (other != ರಿಕ್ತ)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	ತಾರ್ಕಿಕ modified = (write_idx < bits_.size());
	ಯದಿ (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ಯದಿ (other != ರಿಕ್ತ && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::remove(ಧ್ರುವ pool<RTLIL::SigBit> &pattern)
{
	remove2(pattern, ರಿಕ್ತ);
}

ಶೂನ್ಯ RTLIL::SigSpec::remove(ಧ್ರುವ pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) ಧ್ರುವ
{
	RTLIL::SigSpec tmp = *ಏತತ್;
	tmp.remove2(pattern, other);
}

ಶೂನ್ಯ RTLIL::SigSpec::remove2(ಧ್ರುವ pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	ಯದಿ (other != ರಿಕ್ತ) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	ಪ್ರತಿ (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		ಯದಿ (!(bits_[read_idx].wire != ರಿಕ್ತ && pattern.ಎಣಿಕೆ(bits_[read_idx]))) {
			ಯದಿ (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				ಯದಿ (other != ರಿಕ್ತ)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	ತಾರ್ಕಿಕ modified = (write_idx < bits_.size());
	ಯದಿ (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ಯದಿ (other != ರಿಕ್ತ && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::remove2(ಧ್ರುವ std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	ಯದಿ (other != ರಿಕ್ತ) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	ಪ್ರತಿ (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		ಯದಿ (!(bits_[read_idx].wire != ರಿಕ್ತ && pattern.ಎಣಿಕೆ(bits_[read_idx]))) {
			ಯದಿ (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				ಯದಿ (other != ರಿಕ್ತ)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	ತಾರ್ಕಿಕ modified = (write_idx < bits_.size());
	ಯದಿ (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ಯದಿ (other != ರಿಕ್ತ && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::remove2(ಧ್ರುವ pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	ಯದಿ (other != ರಿಕ್ತ) {
		log_assert(size() == other->size());
		other->unpack();
	}

	ತಾರ್ಕಿಕ modified = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ other_modified = ಅಸತ್ಯ;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = GetSize(bits_) - 1; i >= 0; i--) {
		ಯದಿ (bits_[i].wire != ರಿಕ್ತ && pattern.ಎಣಿಕೆ(bits_[i].wire)) {
			modified = ಸತ್ಯ;
			bits_.erase(bits_.begin() + i);
			ಯದಿ (other != ರಿಕ್ತ) {
				other_modified = ಸತ್ಯ;
				other->bits_.erase(other->bits_.begin() + i);
			}
		}
	}

	ಯದಿ (modified) {
		hash_.clear();
		try_repack();
	}
	ಯದಿ (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ಧ್ರುವ RTLIL::SigSpec &pattern, ಧ್ರುವ RTLIL::SigSpec *other) ಧ್ರುವ
{
	log_assert(other == ರಿಕ್ತ || size() == other->size());

	RTLIL::SigSpec ret;
	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();

	ಪ್ರತಿ (auto& pattern_chunk : pattern.chunks()) {
		ಯದಿ (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			ಪೂರ್ಣಾಙ್ಕ i = 0;
			ಪ್ರತಿ (ಧ್ರುವ RTLIL::SigBit &bit : bits_match) {
				ಯದಿ (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} ಅನ್ಯಥಾ {
			ಪ್ರತಿ (ಧ್ರುವ RTLIL::SigBit &bit : bits_match)
				ಯದಿ (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	ಪ್ರತ್ಯಾಗಮ ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ಧ್ರುವ pool<RTLIL::SigBit> &pattern, ಧ್ರುವ RTLIL::SigSpec *other) ಧ್ರುವ
{
	log_assert(other == ರಿಕ್ತ || size() == other->size());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	ಯದಿ (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		ಪೂರ್ಣಾಙ್ಕ i = 0;
		ಪ್ರತಿ (ಧ್ರುವ RTLIL::SigBit &bit : bits_match) {
			ಯದಿ (bit.wire && pattern.ಎಣಿಕೆ(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} ಅನ್ಯಥಾ {
		ಪ್ರತಿ (ಧ್ರುವ RTLIL::SigBit &bit : bits_match)
			ಯದಿ (bit.wire && pattern.ಎಣಿಕೆ(bit))
				ret.append(bit);
	}

	ret.check();
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ RTLIL::SigSpec::replace(ಪೂರ್ಣಾಙ್ಕ offset, ಧ್ರುವ RTLIL::SigSpec &with)
{
	ಯದಿ (with.size() == 0)
		ಪ್ರತ್ಯಾಗಮ;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.size() >= 0);
	log_assert(offset+with.size() <= size());

	ಪೂರ್ಣಾಙ್ಕ i = 0;
	ಪ್ರತಿ (ಧ್ರುವ RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::remove_const()
{
	ಯದಿ (rep_ == CHUNK)
	{
		ಯದಿ (chunk_.wire == ರಿಕ್ತ) {
			chunk_.~SigChunk();
			init_empty_bits();
			hash_.clear();
		}
	}
	ಅನ್ಯಥಾ
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.size());
		ಪ್ರತಿ (auto &bit : bits_)
			ಯದಿ (bit.wire != ರಿಕ್ತ)
				new_bits.ತಳ್ಳು_back(bit);
		ಯದಿ (GetSize(new_bits) != GetSize(bits_)) {
			bits_.swap(new_bits);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::remove(ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ length)
{
	ಯದಿ (length == 0)
		ಪ್ರತ್ಯಾಗಮ;

	unpack();

	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	bits_.erase(bits_.begin() + offset, bits_.begin() + offset + length);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ length) ಧ್ರುವ
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	std::vector<SigBit> extracted;
	SigBit first;
	ತಾರ್ಕಿಕ is_packing = ಸತ್ಯ;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = offset; i < offset + length; i++) {
		ತಾರ್ಕಿಕ was_packing_before = is_packing;
		SigBit bit = (*ಏತತ್)[i];
		ಯದಿ (i == offset) {
			first = bit;
			ಯದಿ (!bit.wire)
				is_packing = ಅಸತ್ಯ;
		} ಅನ್ಯಥಾ {
			ಯದಿ (bit.wire != first.wire)
				is_packing = ಅಸತ್ಯ;
			ಯದಿ (bit.wire)
				ಯದಿ (bit.offset != first.offset + (i - offset))
					is_packing = ಅಸತ್ಯ;
		}
		ಯದಿ (was_packing_before && !is_packing)
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = offset; j < i; j++)
				extracted.ತಳ್ಳು_back((*ಏತತ್)[j]);
		ಯದಿ (!is_packing)
			extracted.ತಳ್ಳು_back((*ಏತತ್)[i]);
	}
	ಯದಿ (is_packing)
		ಪ್ರತ್ಯಾಗಮ SigChunk(first.wire, first.offset, length);

	ಪ್ರತ್ಯಾಗಮ extracted;
}

ಶೂನ್ಯ RTLIL::SigSpec::rewrite_wires(std::function<ಶೂನ್ಯ(RTLIL::Wire*& wire)> rewrite)
{
	ಯದಿ (rep_ == CHUNK) {
		ಯದಿ (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		ಪ್ರತ್ಯಾಗಮ;
	}

	std::vector<RTLIL::SigBit> new_bits;
	ಪ್ರತಿ (ಧ್ರುವ RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		ಯದಿ (c.wire != nullptr)
			rewrite(c.wire);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.clear();
}

ಶೂನ್ಯ RTLIL::SigSpec::append(ಧ್ರುವ RTLIL::SigSpec &ಸಙ್ಕೇತ)
{
	ಯದಿ (ಸಙ್ಕೇತ.size() == 0)
		ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (size() == 0) {
		*ಏತತ್ = ಸಙ್ಕೇತ;
		ಪ್ರತ್ಯಾಗಮ;
	}

	hash_.clear();
	ಯದಿ (rep_ == CHUNK && ಸಙ್ಕೇತ.rep_ == CHUNK && chunk_.wire == ಸಙ್ಕೇತ.chunk_.wire) {
		ಯದಿ (chunk_.wire == ರಿಕ್ತ) {
			chunk_.data.ಸೇರಿಸು(chunk_.data.end(), ಸಙ್ಕೇತ.chunk_.data.begin(), ಸಙ್ಕೇತ.chunk_.data.end());
			chunk_.width = GetSize(chunk_.data);
			ಪ್ರತ್ಯಾಗಮ;
		}
		ಯದಿ (chunk_.offset + chunk_.width == ಸಙ್ಕೇತ.chunk_.offset) {
			chunk_.width += ಸಙ್ಕೇತ.chunk_.width;
			ಪ್ರತ್ಯಾಗಮ;
		}
	}

	unpack();
	ಪ್ರತಿ (ಧ್ರುವ SigBit &bit : ಸಙ್ಕೇತ.bits())
		bits_.ತಳ್ಳು_back(bit);
	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::append(ಧ್ರುವ RTLIL::SigBit &bit)
{
	hash_.clear();

	ಯದಿ (size() == 0) {
		destroy();
		rep_ = CHUNK;
		ನೂತನ (&chunk_) RTLIL::SigChunk(bit);
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (rep_ == CHUNK && chunk_.wire == bit.wire) {
		ಯದಿ (chunk_.wire == ರಿಕ್ತ) {
			chunk_.data.ತಳ್ಳು_back(bit.data);
			chunk_.width++;
			ಪ್ರತ್ಯಾಗಮ;
		}
		ಯದಿ (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			ಪ್ರತ್ಯಾಗಮ;
		}
	}

	unpack();

	bits_.ತಳ್ಳು_back(bit);
	check();
}

ಶೂನ್ಯ RTLIL::SigSpec::extend_u0(ಪೂರ್ಣಾಙ್ಕ width, ತಾರ್ಕಿಕ is_signed)
{
	ಯದಿ (size() > width)
		remove(width, size() - width);

	ಯದಿ (size() < width) {
		RTLIL::SigBit padding = size() > 0 ? (*ಏತತ್)[size() - 1] : RTLIL::State::Sx;
		ಯದಿ (!is_signed)
			padding = RTLIL::State::S0;
		ಯಾವತ್ (size() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ) ಧ್ರುವ
{
	RTLIL::SigSpec sig;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ; i++)
		sig.append(*ಏತತ್);
	ಪ್ರತ್ಯಾಗಮ sig;
}

#ifndef NDEBUG
ಶೂನ್ಯ RTLIL::SigSpec::check(ಧ್ರುವ Module *mod) ಧ್ರುವ
{
	ಯದಿ (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		ಯದಿ (chunk_.wire == ರಿಕ್ತ) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.data.size() == (size_t)chunk_.width);
		} ಅನ್ಯಥಾ {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.data.size() == 0);
			ಯದಿ (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	ಅನ್ಯಥಾ ಯದಿ (size() <= 64)
	{
		ಯದಿ (mod != nullptr) {
			ಪ್ರತಿ (ಧ್ರುವ RTLIL::SigBit &bit : bits_)
				ಯದಿ (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#endif

ತಾರ್ಕಿಕ RTLIL::SigSpec::operator <(ಧ್ರುವ RTLIL::SigSpec &other) ಧ್ರುವ
{
	ಯದಿ (ಏತತ್ == &other)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	ಯದಿ (size() != other.size())
		ಪ್ರತ್ಯಾಗಮ size() < other.size();

	auto other_it = other.chunks().begin();
	ಪ್ರತಿ (ಧ್ರುವ SigChunk &c : chunks()) {
		ಯದಿ (c != *other_it)
			ಪ್ರತ್ಯಾಗಮ c < *other_it;
		++other_it;
	}

	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::operator ==(ಧ್ರುವ RTLIL::SigSpec &other) ಧ್ರುವ
{
	ಯದಿ (ಏತತ್ == &other)
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;

	ಯದಿ (size() != other.size())
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	auto other_it = other.chunks().begin();
	ಪ್ರತಿ (ಧ್ರುವ SigChunk &c : chunks()) {
		ಯದಿ (c != *other_it)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		++other_it;
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_wire() ಧ್ರುವ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	ಯದಿ (it == cs.end())
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಧ್ರುವ RTLIL::SigChunk &chunk = *it;
	ಪ್ರತ್ಯಾಗಮ chunk.wire && chunk.wire->width == size() && ++it == cs.end();
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_chunk() ಧ್ರುವ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	ಯದಿ (it == cs.end())
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ ++it == cs.end();
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::known_driver() ಧ್ರುವ
{
	ಪ್ರತಿ (auto &chunk : chunks())
		ಯದಿ (chunk.is_wire() && !chunk.wire->known_driver())
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_fully_const() ಧ್ರುವ
{
	ಪ್ರತಿ (auto &chunk : chunks())
		ಯದಿ (chunk.width > 0 && chunk.wire != ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_fully_zero() ಧ್ರುವ
{
	ಪ್ರತಿ (auto &chunk : chunks()) {
		ಯದಿ (chunk.width > 0 && chunk.wire != ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		ಪ್ರತಿ (RTLIL::State d : chunk.data)
			ಯದಿ (d != RTLIL::State::S0)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_fully_ones() ಧ್ರುವ
{
	ಪ್ರತಿ (auto &chunk : chunks()) {
		ಯದಿ (chunk.width > 0 && chunk.wire != ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		ಪ್ರತಿ (RTLIL::State d : chunk.data)
			ಯದಿ (d != RTLIL::State::S1)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_fully_def() ಧ್ರುವ
{
	ಪ್ರತಿ (auto &chunk : chunks()) {
		ಯದಿ (chunk.width > 0 && chunk.wire != ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		ಪ್ರತಿ (RTLIL::State d : chunk.data)
			ಯದಿ (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_fully_undef() ಧ್ರುವ
{
	ಪ್ರತಿ (auto &chunk : chunks()) {
		ಯದಿ (chunk.width > 0 && chunk.wire != ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		ಪ್ರತಿ (RTLIL::State d : chunk.data)
			ಯದಿ (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::has_const() ಧ್ರುವ
{
	ಪ್ರತಿ (auto &chunk : chunks())
		ಯದಿ (chunk.width > 0 && chunk.wire == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::has_const(State state) ಧ್ರುವ
{
	ಪ್ರತಿ (auto &chunk : chunks())
		ಯದಿ (chunk.width > 0 && chunk.wire == ರಿಕ್ತ && std::find(chunk.data.begin(), chunk.data.end(), state) != chunk.data.end())
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}


ತಾರ್ಕಿಕ RTLIL::SigSpec::has_marked_bits() ಧ್ರುವ
{
	ಪ್ರತಿ (auto &chunk : chunks())
		ಯದಿ (chunk.width > 0 && chunk.wire == ರಿಕ್ತ) {
			ಪ್ರತಿ (RTLIL::State d : chunk.data)
				ಯದಿ (d == RTLIL::State::Sm)
					ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		}
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::is_onehot(ಪೂರ್ಣಾಙ್ಕ *pos) ಧ್ರುವ
{
	ಯದಿ (std::optional<RTLIL::Const> c = try_as_const())
		ಪ್ರತ್ಯಾಗಮ c->is_onehot(pos);
	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::as_bool() ಧ್ರುವ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ಮೌಲ್ಯ());
	ಪ್ರತ್ಯಾಗಮ c->as_bool();
}

ಪೂರ್ಣಾಙ್ಕ RTLIL::SigSpec::as_int(ತಾರ್ಕಿಕ is_signed) ಧ್ರುವ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ಮೌಲ್ಯ());
	ಪ್ರತ್ಯಾಗಮ c->as_int(is_signed);
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::convertible_to_int(ತಾರ್ಕಿಕ is_signed) ಧ್ರುವ
{
	std::optional<RTLIL::Const> c = try_as_const();
	ಯದಿ (!c.has_ಮೌಲ್ಯ())
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ c->convertible_to_int(is_signed);
}

std::optional<ಪೂರ್ಣಾಙ್ಕ> RTLIL::SigSpec::try_as_int(ತಾರ್ಕಿಕ is_signed) ಧ್ರುವ
{
	std::optional<RTLIL::Const> c = try_as_const();
	ಯದಿ (!c.has_ಮೌಲ್ಯ())
		ಪ್ರತ್ಯಾಗಮ std::nullopt;
	ಪ್ರತ್ಯಾಗಮ c->try_as_int(is_signed);
}

ಪೂರ್ಣಾಙ್ಕ RTLIL::SigSpec::as_int_saturating(ತಾರ್ಕಿಕ is_signed) ಧ್ರುವ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ಮೌಲ್ಯ());
	ಪ್ರತ್ಯಾಗಮ c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ಧ್ರುವ
{
	std::string str;
	str.reserve(size());
	std::vector<RTLIL::SigChunk> chunks = *ಏತತ್;
	ಪ್ರತಿ (size_t i = chunks.size(); i > 0; i--) {
		ಧ್ರುವ RTLIL::SigChunk &chunk = chunks[i-1];
		ಯದಿ (chunk.wire != ರಿಕ್ತ)
			str.append(chunk.width, '?');
		ಅನ್ಯಥಾ
			str += RTLIL::Const(chunk.data).as_string();
	}
	ಪ್ರತ್ಯಾಗಮ str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ಧ್ರುವ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	ಯದಿ (it == cs.end())
		ಪ್ರತ್ಯಾಗಮ RTLIL::Const();
	SigChunk chunk = *it;
	ಯದಿ (chunk.wire != ರಿಕ್ತ || ++it != cs.end())
		ಪ್ರತ್ಯಾಗಮ std::nullopt;
	ಪ್ರತ್ಯಾಗಮ RTLIL::Const(std::move(chunk.data));
}

RTLIL::Const RTLIL::SigSpec::as_const() ಧ್ರುವ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ಮೌಲ್ಯ());
	ಪ್ರತ್ಯಾಗಮ *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ಧ್ರುವ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == size());
	ಪ್ರತ್ಯಾಗಮ chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ಧ್ರುವ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	ಪ್ರತ್ಯಾಗಮ chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ RTLIL::SigBit(*ಏತತ್);
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::match(ಧ್ರುವ ವರ್ಣ* pattern) ಧ್ರುವ
{
	ಪೂರ್ಣಾಙ್ಕ pattern_len = strlen(pattern);
	log_assert(pattern_len == size());

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < pattern_len; ++i) {
		ವರ್ಣ ch = pattern[i];
		ಯದಿ (ch == ' ')
			ಅನುವರ್ತ;
		RTLIL::SigBit bit = (*ಏತತ್)[pattern_len - 1 - i];
		ಯದಿ (ch == '*') {
			ಯದಿ (bit != State::Sz && bit != State::Sx)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಅನುವರ್ತ;
		}
		ಯದಿ (ch == '0') {
			ಯದಿ (bit != State::S0)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		} ಅನ್ಯಥಾ
		ಯದಿ (ch == '1') {
			ಯದಿ (bit != State::S1)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		} ಅನ್ಯಥಾ
			log_abort();
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() ಧ್ರುವ
{
	std::set<RTLIL::SigBit> sigbits;
	ಪ್ರತಿ (auto &c : chunks())
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < c.width; i++)
			sigbits.ಸೇರಿಸು(RTLIL::SigBit(c, i));
	ಪ್ರತ್ಯಾಗಮ sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() ಧ್ರುವ
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(size());
	ಪ್ರತಿ (auto &c : chunks())
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < c.width; i++)
			sigbits.ಸೇರಿಸು(RTLIL::SigBit(c, i));
	ಪ್ರತ್ಯಾಗಮ sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() ಧ್ರುವ
{
	std::vector<RTLIL::SigBit> result;
	result.reserve(size());
	ಪ್ರತಿ (SigBit bit : *ಏತತ್)
		result.ತಳ್ಳು_back(bit);
	ಪ್ರತ್ಯಾಗಮ result;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(ಧ್ರುವ RTLIL::SigSpec &other) ಧ್ರುವ
{
	ಪೂರ್ಣಾಙ್ಕ this_size = size();
	log_assert(this_size == other.size());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < this_size; i++)
		new_map[(*ಏತತ್)[i]] = other[i];

	ಪ್ರತ್ಯಾಗಮ new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(ಧ್ರುವ RTLIL::SigSpec &other) ಧ್ರುವ
{
	ಪೂರ್ಣಾಙ್ಕ this_size = size();
	log_assert(this_size == other.size());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_size);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < this_size; i++)
		new_map[(*ಏತತ್)[i]] = other[i];

	ಪ್ರತ್ಯಾಗಮ new_map;
}

ಸ್ಥಿರ ಶೂನ್ಯ sigspec_parse_split(std::vector<std::string> &tokens, ಧ್ರುವ std::string &text, ವರ್ಣ sep)
{
	size_t start = 0, end = 0;
	ಯಾವತ್ ((end = text.find(sep, start)) != std::string::npos) {
		tokens.ತಳ್ಳು_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.ತಳ್ಳು_back(text.substr(start));
}

ಸ್ಥಿರ RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ಧ್ರುವ std::string &netname)
{
	ಪ್ರತ್ಯಾಗಮ module->wire(netname);
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ tokidx = ಪೂರ್ಣಾಙ್ಕ(tokens.size())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		ಯದಿ (netname.size() == 0)
			ಅನುವರ್ತ;

		ಯದಿ (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			auto ast = p.const2ast(netname);
			ಯದಿ (ast == nullptr)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			sig.append(RTLIL::Const(ast->bits));
			ಅನುವರ್ತ;
		}

		ಯದಿ (module == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

		ಯದಿ (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		ಯದಿ (sigspec_parse_wire(module, netname) == nullptr) {
			size_t indices_pos = netname.size()-1;
			ಯದಿ (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				ಯಾವತ್ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				ಯದಿ (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					ಯಾವತ್ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				ಯದಿ (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		ಯದಿ (wire == nullptr)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

		ಯದಿ (!indices.empty()) {
			std::vector<std::string> index_tokens;
			sigspec_parse_split(index_tokens, indices.substr(1, indices.size()-2), ':');
			ಯದಿ (index_tokens.size() == 1) {
				ಪೂರ್ಣಾಙ್ಕ a = atoi(index_tokens.at(0).c_str());
				ಯದಿ (a < 0 || a >= wire->width)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				sig.append(RTLIL::SigSpec(wire, a));
			} ಅನ್ಯಥಾ {
				ಪೂರ್ಣಾಙ್ಕ a = atoi(index_tokens.at(0).c_str());
				ಪೂರ್ಣಾಙ್ಕ b = atoi(index_tokens.at(1).c_str());
				ಯದಿ (a > b) {
					ಪೂರ್ಣಾಙ್ಕ tmp = a;
					a = b, b = tmp;
				}
				ಯದಿ (a < 0 || a >= wire->width)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (b < 0 || b >= wire->width)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} ಅನ್ಯಥಾ
			sig.append(wire);
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	ಯದಿ (str.empty() || str[0] != '@')
		ಪ್ರತ್ಯಾಗಮ parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	ಯದಿ (sel_name == IdString::Null || design->selection_vars.ಎಣಿಕೆ(sel_name) == 0)
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	ಪ್ರತಿ (auto &it : module->wires_)
		ಯದಿ (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ RTLIL::SigSpec::parse_rhs(ಧ್ರುವ RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	ಯದಿ (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.size());
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.size());
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (lhs.is_chunk()) {
		ವರ್ಣ *p = (ವರ್ಣ*)str.c_str(), *endptr;
		long ಪೂರ್ಣಾಙ್ಕ val = strtol(p, &endptr, 10);
		ಯದಿ (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.size());
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		}
	}

	ಯದಿ (!parse(sig, module, str))
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಯದಿ (sig.size() > lhs.size())
		sig.remove(lhs.size(), sig.size() - lhs.size());
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

RTLIL::SigSpec::operator std::vector<RTLIL::SigChunk>() ಧ್ರುವ
{
	std::vector<RTLIL::SigChunk> result;
	ಪ್ರತಿ (ಧ್ರುವ RTLIL::SigChunk &c : chunks())
		result.ತಳ್ಳು_back(c);
	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::CaseRule::~CaseRule()
{
	ಪ್ರತಿ (auto it = switches.begin(); it != switches.end(); it++)
		delete *it;
}

ತಾರ್ಕಿಕ RTLIL::CaseRule::empty() ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() ಧ್ರುವ
{
	RTLIL::CaseRule *new_caserule = ನೂತನ RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	ಪ್ರತಿ (auto &it : switches)
		new_caserule->switches.ತಳ್ಳು_back(it->clone());
	ಪ್ರತ್ಯಾಗಮ new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	ಪ್ರತಿ (auto it = cases.begin(); it != cases.end(); it++)
		delete *it;
}

ತಾರ್ಕಿಕ RTLIL::SwitchRule::empty() ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() ಧ್ರುವ
{
	RTLIL::SwitchRule *new_switchrule = ನೂತನ RTLIL::SwitchRule;
	new_switchrule->ಸಙ್ಕೇತ = ಸಙ್ಕೇತ;
	new_switchrule->attributes = attributes;
	ಪ್ರತಿ (auto &it : cases)
		new_switchrule->cases.ತಳ್ಳು_back(it->clone());
	ಪ್ರತ್ಯಾಗಮ new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() ಧ್ರುವ
{
	RTLIL::SyncRule *new_syncrule = ನೂತನ RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->ಸಙ್ಕೇತ = ಸಙ್ಕೇತ;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	ಪ್ರತ್ಯಾಗಮ new_syncrule;
}

RTLIL::Process::~Process()
{
	ಪ್ರತಿ (auto it = syncs.begin(); it != syncs.end(); it++)
		delete *it;
}

RTLIL::Process *RTLIL::Process::clone() ಧ್ರುವ
{
	RTLIL::Process *new_proc = ನೂತನ RTLIL::Process;

	new_proc->attributes = attributes;

	RTLIL::CaseRule *rc_ptr = ಬೇರು_case.clone();
	new_proc->ಬೇರು_case = *rc_ptr;
	rc_ptr->switches.clear();
	delete rc_ptr;

	ಪ್ರತಿ (auto &it : syncs)
		new_proc->syncs.ತಳ್ಳು_back(it->clone());

	ಪ್ರತ್ಯಾಗಮ new_proc;
}

RTLIL::Memory::~Memory()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
ಸ್ಥಿರ std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Memory*> all_memorys;
std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Memory*> *RTLIL::Memory::get_all_memorys(ಶೂನ್ಯ)
{
	ಪ್ರತ್ಯಾಗಮ &all_memorys;
}
#endif
YOSYS_NAMESPACE_END
