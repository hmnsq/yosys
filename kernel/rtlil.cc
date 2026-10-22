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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/rtlil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/macc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/newcelltypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/sigtools.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/threading.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "frontends/verilog/verilog_frontend.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "frontends/verilog/preproc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "backends/rtlil/rtlil_backend.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <algorithm>
#ਅਨ੍ਤਰ੍ਭਾਵ <charconv>
#ਅਨ੍ਤਰ੍ਭਾਵ <optional>
#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>
#ਅਨ੍ਤਰ੍ਭਾਵ <sstream>

YOSYS_NAMESPACE_BEGIN

dict<std::string, std::string> RTLIL::constpad;

ਅਚਲ pool<IdString> &RTLIL::builtin_ff_cell_types() {
	ਸਥਿਰ ਅਚਲ pool<IdString> res = []() {
		pool<IdString> r;
		ਲਈ (ਮਾਪ_t i = 0; i < StaticCellTypes::builder.ਗਿਣਤੀ_2; i++) {
			ਸਵੈਚਾਲਿਤ &cell = StaticCellTypes::builder.cells[i];
			ਜੇ (cell.features.is_ff)
				r.ਪਾਓ(cell.type);
		}
		ਵਾਪਸ r;
	}();
	ਵਾਪਸ res;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ check(condition) log_assert(condition && "malformed Const union")

ਅਚਲ Const::bitvectype& Const::get_ਬਿੱਟ() ਅਚਲ {
	check(is_ਬਿੱਟ());
	ਵਾਪਸ *get_if_ਬਿੱਟ();
}

ਅਚਲ std::string& Const::get_str() ਅਚਲ {
	check(is_str());
	ਵਾਪਸ *get_if_str();
}

Const::bitvectype& Const::get_ਬਿੱਟ() {
	check(is_ਬਿੱਟ());
	ਵਾਪਸ *get_if_ਬਿੱਟ();
}

std::string& Const::get_str() {
	check(is_str());
	ਵਾਪਸ *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	ਨਵਾਂ ((ਖਾਲੀ*)&str_) std::string(std::move(str));
	ਟੈਗ = backing_ਟੈਗ::string;
}

RTLIL::Const::Const(ਲੰਮਾ ਲੰਮਾ val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	ਅੱਖਰ ਬਾਈਟਾਂ[] = {
		(ਅੱਖਰ)(val >> 24), (ਅੱਖਰ)(val >> 16), (ਅੱਖਰ)(val >> 8), (ਅੱਖਰ)val
	};
	ਨਵਾਂ ((ਖਾਲੀ*)&str_) std::string(ਬਾਈਟਾਂ, 4);
	ਟੈਗ = backing_ਟੈਗ::string;
}

RTLIL::Const::Const(ਲੰਮਾ ਲੰਮਾ val, ਪੂਰਨ ਚੌੜਾਈ)
{
	log_assert(ਚੌੜਾਈ >= 0 && ਚੌੜਾਈ < RTLIL::ਚੌੜਾਈ_ਹੱਦਬੰਦੀ);
	flags = RTLIL::CONST_FLAG_NONE;
	ਜੇ ((ਚੌੜਾਈ & 7) == 0) {
		ਨਵਾਂ ((ਖਾਲੀ*)&str_) std::string();
		ਟੈਗ = backing_ਟੈਗ::string;
		std::string& str = get_str();
		ਪੂਰਨ ਬਾਈਟਾਂ = ਚੌੜਾਈ >> 3;
		ਨਿਸ਼ਾਨੀਵਾਲਾ ਅੱਖਰ sign_byte = val < 0 ? -1 : 0;
		str.resize(ਬਾਈਟਾਂ, sign_byte);
		ਬਾਈਟਾਂ = std::min<ਪੂਰਨ>(ਬਾਈਟਾਂ, ਆਕਾਰ(val));
		ਲਈ (ਪੂਰਨ i = 0; i < ਬਾਈਟਾਂ; i++) {
			str[str.ਮਾਪ() - 1 - i] = val;
			val = val >> 8;
		}
		ਵਾਪਸ;
	}

	ਨਵਾਂ ((ਖਾਲੀ*)&ਬਿੱਟ_2) bitvectype();
	ਟੈਗ = backing_ਟੈਗ::ਬਿੱਟ;
	bitvectype& bv = get_ਬਿੱਟ();
	bv.reserve(ਚੌੜਾਈ);
	ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++) {
		bv.ਧੱਕੋ_ਪਿੱਠ((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ ਚੌੜਾਈ)
{
	log_assert(ਚੌੜਾਈ >= 0 && ਚੌੜਾਈ < RTLIL::ਚੌੜਾਈ_ਹੱਦਬੰਦੀ);
	flags = RTLIL::CONST_FLAG_NONE;
	ਨਵਾਂ ((ਖਾਲੀ*)&ਬਿੱਟ_2) bitvectype();
	ਟੈਗ = backing_ਟੈਗ::ਬਿੱਟ;
	bitvectype& bv = get_ਬਿੱਟ();
	bv.reserve(ਚੌੜਾਈ);
	ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++)
		bv.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ);
}

RTLIL::Const::Const(ਅਚਲ std::ਸਦਿਸ਼<ਤਾਰਕਿਕ> &ਬਿੱਟ)
{
	flags = RTLIL::CONST_FLAG_NONE;
	ਨਵਾਂ ((ਖਾਲੀ*)&ਬਿੱਟ_2) bitvectype();
	ਟੈਗ = backing_ਟੈਗ::ਬਿੱਟ;
	bitvectype& bv = get_ਬਿੱਟ();
	bv.reserve(ਬਿੱਟ.ਮਾਪ());
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &b : ਬਿੱਟ)
		bv.emplace_ਪਿੱਠ(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ਅਚਲ RTLIL::Const &other) {
	ਟੈਗ = other.ਟੈਗ;
	flags = other.flags;
	ਜੇ (is_str())
		ਨਵਾਂ ((ਖਾਲੀ*)&str_) std::string(other.get_str());
	ਨਹੀਂਤਾਂ ਜੇ (is_ਬਿੱਟ())
		ਨਵਾਂ ((ਖਾਲੀ*)&ਬਿੱਟ_2) bitvectype(other.get_ਬਿੱਟ());
	ਨਹੀਂਤਾਂ
		check(ਝੂਠ);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	ਟੈਗ = other.ਟੈਗ;
	flags = other.flags;
	ਜੇ (is_str())
		ਨਵਾਂ ((ਖਾਲੀ*)&str_) std::string(std::move(other.get_str()));
	ਨਹੀਂਤਾਂ ਜੇ (is_ਬਿੱਟ())
		ਨਵਾਂ ((ਖਾਲੀ*)&ਬਿੱਟ_2) bitvectype(std::move(other.get_ਬਿੱਟ()));
	ਨਹੀਂਤਾਂ
		check(ਝੂਠ);
}

RTLIL::Const &RTLIL::Const::ਸੰਚਾਲਕ =(ਅਚਲ RTLIL::Const &other) {
	flags = other.flags;
	ਜੇ (other.is_str()) {
		ਜੇ (!is_str()) {
			// sketchy zone
			check(is_ਬਿੱਟ());
			ਬਿੱਟ_2.~bitvectype();
			(ਖਾਲੀ)ਨਵਾਂ ((ਖਾਲੀ*)&str_) std::string();
		}
		ਟੈਗ = other.ਟੈਗ;
		get_str() = other.get_str();
	} ਨਹੀਂਤਾਂ ਜੇ (other.is_ਬਿੱਟ()) {
		ਜੇ (!is_ਬਿੱਟ()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(ਖਾਲੀ)ਨਵਾਂ ((ਖਾਲੀ*)&ਬਿੱਟ_2) bitvectype();
		}
		ਟੈਗ = other.ਟੈਗ;
		get_ਬਿੱਟ() = other.get_ਬਿੱਟ();
	} ਨਹੀਂਤਾਂ {
		check(ਝੂਠ);
	}
	ਵਾਪਸ *ਇਹ;
}

RTLIL::Const::~Const() {
	ਜੇ (is_ਬਿੱਟ())
		ਬਿੱਟ_2.~bitvectype();
	ਨਹੀਂਤਾਂ ਜੇ (is_str())
		str_.~string();
	ਨਹੀਂਤਾਂ
		check(ਝੂਠ);
}

ਤਾਰਕਿਕ RTLIL::Const::ਸੰਚਾਲਕ<(ਅਚਲ RTLIL::Const &other) ਅਚਲ
{
	ਜੇ (ਮਾਪ() != other.ਮਾਪ())
		ਵਾਪਸ ਮਾਪ() < other.ਮਾਪ();

	ਲਈ (ਪੂਰਨ i = 0; i < ਮਾਪ(); i++)
		ਜੇ ((*ਇਹ)[i] != other[i])
			ਵਾਪਸ (*ਇਹ)[i] < other[i];

	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ RTLIL::Const::ਸੰਚਾਲਕ ==(ਅਚਲ RTLIL::Const &other) ਅਚਲ
{
	ਜੇ (is_str() && other.is_str())
		ਵਾਪਸ get_str() == other.get_str();
	ਜੇ (is_ਬਿੱਟ() && other.is_ਬਿੱਟ())
		ਵਾਪਸ get_ਬਿੱਟ() == other.get_ਬਿੱਟ();

	ਜੇ (ਮਾਪ() != other.ਮਾਪ())
		ਵਾਪਸ ਝੂਠ;

	ਲਈ (ਪੂਰਨ i = 0; i < ਮਾਪ(); i++)
	ਜੇ ((*ਇਹ)[i] != other[i])
		ਵਾਪਸ ਝੂਠ;

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::Const::ਸੰਚਾਲਕ !=(ਅਚਲ RTLIL::Const &other) ਅਚਲ
{
	ਵਾਪਸ !(*ਇਹ == other);
}

std::ਸਦਿਸ਼<RTLIL::State>& RTLIL::Const::ਬਿੱਟ_ਅੰਦਰੂਨੀ()
{
	bitvectorize_ਅੰਦਰੂਨੀ();
	ਵਾਪਸ get_ਬਿੱਟ();
}

std::ਸਦਿਸ਼<RTLIL::State> RTLIL::Const::ਮੰਜ਼ਿਲ_ਬਿੱਟ() ਅਚਲ
{
	std::ਸਦਿਸ਼<State> v;
	ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : *ਇਹ)
		v.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ);
	ਵਾਪਸ v;
}

ਤਾਰਕਿਕ RTLIL::Const::as_bool() ਅਚਲ
{
	ਜੇ (is_str()) {
		ਲਈ (ਅੱਖਰ ch : get_str())
			ਜੇ (ch != 0)
				ਵਾਪਸ ਸੱਚ;
		ਵਾਪਸ ਝੂਠ;
	}

	ਅਚਲ bitvectype& bv = get_ਬਿੱਟ();
	ਲਈ (ਮਾਪ_t i = 0; i < bv.ਮਾਪ(); i++)
		ਜੇ (bv[i] == State::S1)
			ਵਾਪਸ ਸੱਚ;
	ਵਾਪਸ ਝੂਠ;
}

ਪੂਰਨ RTLIL::Const::as_int(ਤਾਰਕਿਕ is_signed) ਅਚਲ
{
	int32_t ret = 0;
	ਜੇ (ਅਚਲ std::string *s = get_if_str()) {
		ਪੂਰਨ ਮਾਪ = Get_ਮਾਪ(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		ਲਈ (ਪੂਰਨ i = std::min(4, ਮਾਪ); i > 0; i--)
			ret |= static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>((*s)[ਮਾਪ - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		ਜੇ (is_signed && ਮਾਪ > 0 && ਮਾਪ < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << ਮਾਪ*8;
		ਵਾਪਸ ret;
	}

	ਅਚਲ bitvectype& bv = get_ਬਿੱਟ();
	ਪੂਰਨ significant_ਬਿੱਟ = std::min(Get_ਮਾਪ(bv), 32);
	ਲਈ (ਪੂਰਨ i = 0; i < significant_ਬਿੱਟ; i++)
		ਜੇ (bv[i] == State::S1)
			ret |= 1 << i;
	ਜੇ (is_signed && significant_ਬਿੱਟ > 0 && significant_ਬਿੱਟ < 32 && bv.ਪਿੱਠ() == State::S1 )
		ret |= UINT32_MAX << significant_ਬਿੱਟ;
	ਵਾਪਸ ret;
}

ਤਾਰਕਿਕ RTLIL::Const::convertible_ਮੰਜ਼ਿਲ_int(ਤਾਰਕਿਕ is_signed) ਅਚਲ
{
	ਸਵੈਚਾਲਿਤ ਮਾਪ = get_min_ਮਾਪ(is_signed);

	ਜੇ (ਮਾਪ < 0)
		ਵਾਪਸ ਝੂਠ;

	// If it fits in 31 bits it is definitely convertible
	ਜੇ (ਮਾਪ <= 31)
		ਵਾਪਸ ਸੱਚ;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	ਜੇ (ਮਾਪ == 32) {
		ਜੇ (is_signed)
			ਵਾਪਸ ਸੱਚ;
		ਵਾਪਸ (*ਇਹ)[ਮਾਪ - 1] != State::S1;
	}

	ਵਾਪਸ ਝੂਠ;
}

std::optional<ਪੂਰਨ> RTLIL::Const::try_as_int(ਤਾਰਕਿਕ is_signed) ਅਚਲ
{
	ਜੇ (!convertible_ਮੰਜ਼ਿਲ_int(is_signed))
		ਵਾਪਸ std::nullopt;
	ਵਾਪਸ as_int(is_signed);
}

ਪੂਰਨ RTLIL::Const::as_int_saturating(ਤਾਰਕਿਕ is_signed) ਅਚਲ
{
	ਜੇ (!convertible_ਮੰਜ਼ਿਲ_int(is_signed)) {
		ਜੇ (!is_signed)
			ਵਾਪਸ std::numeric_limits<ਪੂਰਨ>::max();

		ਅਚਲ ਸਵੈਚਾਲਿਤ min_ਮਾਪ = get_min_ਮਾਪ(is_signed);
		log_assert(min_ਮਾਪ > 0);
		ਅਚਲ ਸਵੈਚਾਲਿਤ neg = (*ਇਹ)[min_ਮਾਪ - 1];
		ਵਾਪਸ neg ? std::numeric_limits<ਪੂਰਨ>::min() : std::numeric_limits<ਪੂਰਨ>::max();
	}
	ਵਾਪਸ as_int(is_signed);
}

ਖਾਲੀ RTLIL::Const::ਟੈਗ_bare_integer_const(ਅਚਲ std::string &ਮੁੱਲ)
{
	ਜੇ (ਮੁੱਲ.ਖਾਲੀ_ਥਾਵਾਂ() || ਮੁੱਲ.ਲੱਭੋ('\'') != std::string::npos)
		ਵਾਪਸ;
	ਮਾਪ_t ਸ਼ੁਰੂ = (ਮੁੱਲ[0] == '-' || ਮੁੱਲ[0] == '+') ? 1 : 0;
	ਜੇ (ਸ਼ੁਰੂ < ਮੁੱਲ.ਮਾਪ() && std::all_ਦਾ(ਮੁੱਲ.begin() + ਸ਼ੁਰੂ, ਮੁੱਲ.ਸਿਰਾ(), ::ਅੰਕ_ਹੈ))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

ਪੂਰਨ RTLIL::Const::get_min_ਮਾਪ(ਤਾਰਕਿਕ is_signed) ਅਚਲ
{
	ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ()) ਵਾਪਸ 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_ਦੋਆਧਾਰੀ_ਅੰਕ;
	ਜੇ (is_signed)
		leading_ਦੋਆਧਾਰੀ_ਅੰਕ = (ਪਿੱਠ() == RTLIL::State::Sx) ? RTLIL::State::S0 : ਪਿੱਠ();
	ਨਹੀਂਤਾਂ
		leading_ਦੋਆਧਾਰੀ_ਅੰਕ = RTLIL::State::S0;

	ਸਵੈਚਾਲਿਤ idx = ਮਾਪ();
	ਜਦੋਂ (idx > 0 && (*ਇਹ)[idx -1] == leading_ਦੋਆਧਾਰੀ_ਅੰਕ) {
		idx--;
	}

	// signed needs one leading bit
	ਜੇ (is_signed && idx < ਮਾਪ()) {
		idx++;
	}
	// must be at least one bit
	ਵਾਪਸ (idx == 0) ? 1 : idx;
}

ਖਾਲੀ RTLIL::Const::compress(ਤਾਰਕਿਕ is_signed)
{
	ਸਵੈਚਾਲਿਤ idx = get_min_ਮਾਪ(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<ਪੂਰਨ> RTLIL::Const::as_int_compress(ਤਾਰਕਿਕ is_signed) ਅਚਲ
{
	ਵਾਪਸ try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ਅਚਲ ਅੱਖਰ* any) ਅਚਲ
{
	ਪੂਰਨ sz = ਮਾਪ();
	std::string ret;
	ret.reserve(sz);
	ਲਈ (ਪੂਰਨ i = sz - 1; i >= 0; --i)
		ਚੋਣ ((*ਇਹ)[i]) {
			ਹਾਲਤ S0: ret.ਧੱਕੋ_ਪਿੱਠ('0'); ਰੋਕੋ;
			ਹਾਲਤ S1: ret.ਧੱਕੋ_ਪਿੱਠ('1'); ਰੋਕੋ;
			ਹਾਲਤ Sx: ret.ਧੱਕੋ_ਪਿੱਠ('x'); ਰੋਕੋ;
			ਹਾਲਤ Sz: ret.ਧੱਕੋ_ਪਿੱਠ('z'); ਰੋਕੋ;
			ਹਾਲਤ Sa: ret += any; ਰੋਕੋ;
			ਹਾਲਤ Sm: ret.ਧੱਕੋ_ਪਿੱਠ('m'); ਰੋਕੋ;
		}
	ਵਾਪਸ ret;
}

RTLIL::Const RTLIL::Const::from_string(ਅਚਲ std::string &str)
{
	Const c;
	bitvectype& bv = c.get_ਬਿੱਟ();
	bv.reserve(str.ਮਾਪ());
	ਲਈ (ਸਵੈਚਾਲਿਤ it = str.rbegin(); it != str.rend(); it++)
		ਚੋਣ (*it) {
			ਹਾਲਤ '0': bv.ਧੱਕੋ_ਪਿੱਠ(State::S0); ਰੋਕੋ;
			ਹਾਲਤ '1': bv.ਧੱਕੋ_ਪਿੱਠ(State::S1); ਰੋਕੋ;
			ਹਾਲਤ 'x': bv.ਧੱਕੋ_ਪਿੱਠ(State::Sx); ਰੋਕੋ;
			ਹਾਲਤ 'z': bv.ਧੱਕੋ_ਪਿੱਠ(State::Sz); ਰੋਕੋ;
			ਹਾਲਤ 'm': bv.ਧੱਕੋ_ਪਿੱਠ(State::Sm); ਰੋਕੋ;
			ਮੂਲ: bv.ਧੱਕੋ_ਪਿੱਠ(State::Sa);
		}
	ਵਾਪਸ c;
}

std::string RTLIL::Const::decode_string() ਅਚਲ
{
	ਜੇ (ਸਵੈਚਾਲਿਤ str = get_if_str())
		ਵਾਪਸ *str;

	ਅਚਲ bitvectype& bv = get_ਬਿੱਟ();
	ਅਚਲ ਪੂਰਨ n = Get_ਮਾਪ(bv);
	ਅਚਲ ਪੂਰਨ n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	ਪੂਰਨ i = n_over_8 * 8;
	ਜੇ (i < n) {
		ਅੱਖਰ ch = 0;
		ਲਈ (ਪੂਰਨ j = 0; j < (n - i); j++) {
			ਜੇ (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		ਜੇ (ch != 0)
			s.append({ch});
	}
	i -= 8;
	ਲਈ (; i >= 0; i -= 8) {
		ਅੱਖਰ ch = 0;
		ਲਈ (ਪੂਰਨ j = 0; j < 8; j++) {
			ਜੇ (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		ਜੇ (ch != 0)
			s.append({ch});
	}
	ਵਾਪਸ s;
}

ਪੂਰਨ RTLIL::Const::ਮਾਪ() ਅਚਲ {
	ਜੇ (is_str())
		ਵਾਪਸ 8 * str_.ਮਾਪ();
	ਨਹੀਂਤਾਂ {
		check(is_ਬਿੱਟ());
		ਵਾਪਸ ਬਿੱਟ_2.ਮਾਪ();
	}
}

ਤਾਰਕਿਕ RTLIL::Const::ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ {
	ਜੇ (is_str())
		ਵਾਪਸ str_.ਖਾਲੀ_ਥਾਵਾਂ();
	ਨਹੀਂਤਾਂ {
		check(is_ਬਿੱਟ());
		ਵਾਪਸ ਬਿੱਟ_2.ਖਾਲੀ_ਥਾਵਾਂ();
	}
}

ਖਾਲੀ RTLIL::Const::bitvectorize_ਅੰਦਰੂਨੀ() {
	ਜੇ (ਟੈਗ == backing_ਟੈਗ::ਬਿੱਟ)
		ਵਾਪਸ;

	check(is_str());

	bitvectype new_ਬਿੱਟ;

	new_ਬਿੱਟ.reserve(str_.ਮਾਪ() * 8);
	ਲਈ (ਪੂਰਨ i = str_.ਮਾਪ() - 1; i >= 0; i--) {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ch = str_[i];
		ਲਈ (ਪੂਰਨ j = 0; j < 8; j++) {
			new_ਬਿੱਟ.ਧੱਕੋ_ਪਿੱਠ((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(ਖਾਲੀ)ਨਵਾਂ ((ਖਾਲੀ*)&ਬਿੱਟ_2) bitvectype(std::move(new_ਬਿੱਟ));
		ਟੈਗ = backing_ਟੈਗ::ਬਿੱਟ;
	}
}

ਖਾਲੀ RTLIL::Const::append(ਅਚਲ RTLIL::Const &other) {
	bitvectorize_ਅੰਦਰੂਨੀ();
	bitvectype& bv = get_ਬਿੱਟ();
	bv.ਪਾਓ(bv.ਸਿਰਾ(), other.begin(), other.ਸਿਰਾ());
}

RTLIL::State RTLIL::Const::const_iterator::ਸੰਚਾਲਕ*() ਅਚਲ {
	ਜੇ (ਸਵੈਚਾਲਿਤ bv = ਮਾਪਾ->get_if_ਬਿੱਟ())
		ਵਾਪਸ (*bv)[idx];

	ਪੂਰਨ char_idx = ਮਾਪਾ->get_str().ਮਾਪ() - idx / 8 - 1;
	ਤਾਰਕਿਕ ਦੋਆਧਾਰੀ_ਅੰਕ = (ਮਾਪਾ->get_str()[char_idx] & (1 << (idx % 8)));
	ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ ? State::S1 : State::S0;
}

ਤਾਰਕਿਕ RTLIL::Const::is_fully_zero() ਅਚਲ
{
	ਜੇ (ਸਵੈਚਾਲਿਤ str = get_if_str()) {
		ਲਈ (ਅੱਖਰ ch : *str)
			ਜੇ (ch != 0)
				ਵਾਪਸ ਝੂਠ;
		ਵਾਪਸ ਸੱਚ;
	}

	ਅਚਲ bitvectype& bv = get_ਬਿੱਟ();

	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : bv)
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ != RTLIL::State::S0)
			ਵਾਪਸ ਝੂਠ;

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::Const::is_fully_ones() ਅਚਲ
{
	ਜੇ (ਸਵੈਚਾਲਿਤ str = get_if_str()) {
		ਲਈ (ਅੱਖਰ ch : *str)
			ਜੇ (ch != (ਅੱਖਰ)0xff)
				ਵਾਪਸ ਝੂਠ;
		ਵਾਪਸ ਸੱਚ;
	}

	ਅਚਲ bitvectype& bv = get_ਬਿੱਟ();
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : bv)
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ != RTLIL::State::S1)
			ਵਾਪਸ ਝੂਠ;

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::Const::is_fully_def() ਅਚਲ
{
	ਜੇ (is_str())
		ਵਾਪਸ ਸੱਚ;

	ਅਚਲ bitvectype& bv = get_ਬਿੱਟ();
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : bv)
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ != RTLIL::State::S0 && ਦੋਆਧਾਰੀ_ਅੰਕ != RTLIL::State::S1)
			ਵਾਪਸ ਝੂਠ;

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::Const::is_fully_undef() ਅਚਲ
{
	ਜੇ (ਸਵੈਚਾਲਿਤ str = get_if_str())
		ਵਾਪਸ str->ਖਾਲੀ_ਥਾਵਾਂ();

	ਅਚਲ bitvectype& bv = get_ਬਿੱਟ();
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : bv)
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ != RTLIL::State::Sx && ਦੋਆਧਾਰੀ_ਅੰਕ != RTLIL::State::Sz)
			ਵਾਪਸ ਝੂਠ;

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::Const::is_fully_undef_x_only() ਅਚਲ
{
	ਜੇ (ਸਵੈਚਾਲਿਤ str = get_if_str())
		ਵਾਪਸ str->ਖਾਲੀ_ਥਾਵਾਂ();

	ਅਚਲ bitvectype& bv = get_ਬਿੱਟ();
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : bv)
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ != RTLIL::State::Sx)
			ਵਾਪਸ ਝੂਠ;

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::Const::is_onehot(ਪੂਰਨ *pos) ਅਚਲ
{
	ਤਾਰਕਿਕ ਮਿਲਿਆ = ਝੂਠ;
	ਪੂਰਨ ਮਾਪ = Get_ਮਾਪ(*ਇਹ);
	ਲਈ (ਪੂਰਨ i = 0; i < ਮਾਪ; i++) {
		State ਦੋਆਧਾਰੀ_ਅੰਕ = (*ਇਹ)[i];
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ != RTLIL::State::S0 && ਦੋਆਧਾਰੀ_ਅੰਕ != RTLIL::State::S1)
			ਵਾਪਸ ਝੂਠ;
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ == RTLIL::State::S1) {
			ਜੇ (ਮਿਲਿਆ)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (pos)
				*pos = i;
			ਮਿਲਿਆ = ਸੱਚ;
		}
	}
	ਵਾਪਸ ਮਿਲਿਆ;
}

Hasher RTLIL::Const::hash_into(Hasher h) ਅਚਲ
{
	ਜੇ (ਸਵੈਚਾਲਿਤ str = get_if_str())
		ਵਾਪਸ hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ਅਚਲ bitvectype &bv = get_ਬਿੱਟ();
	ਪੂਰਨ ਮਾਪ = Get_ਮਾਪ(bv);
	std::string packed;
	ਪੂਰਨ packed_ਮਾਪ = (ਮਾਪ + 7) >> 3;
	packed.resize(packed_ਮਾਪ, 0);
	ਲਈ (ਪੂਰਨ bi = 0; bi < packed_ਮਾਪ; ++bi) {
		ਅੱਖਰ ch = 0;
		ਪੂਰਨ ਸਿਰਾ = std::min((bi + 1)*8, ਮਾਪ);
		ਲਈ (ਪੂਰਨ i = bi*8; i < ਸਿਰਾ; ++i) {
			RTLIL::State b = bv[i];
			ਜੇ (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<ਅੱਖਰ>::hash_into(ch, h);
				ਲਈ (; i < ਮਾਪ; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(ਮਾਪ);
				ਵਾਪਸ h;
			}
			ch |= static_cast<ਪੂਰਨ>(b) << (i & 7);
		}
		packed[packed_ਮਾਪ - 1 - bi] = ch;
	}
	ਵਾਪਸ hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::ਕੱਢੋ_2(ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ len, RTLIL::State padding) ਅਚਲ {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	ਲਈ (ਪੂਰਨ i = ਵਿੱਥ; i < ਵਿੱਥ + len; i++)
		ret_bv.ਧੱਕੋ_ਪਿੱਠ(i < Get_ਮਾਪ(*ਇਹ) ? (*ਇਹ)[i] : padding);
	ਵਾਪਸ RTLIL::Const(ret_bv);
}
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ check /* check(condition) for Const */

ਤਾਰਕਿਕ RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ਅਚਲ
{
	ਵਾਪਸ ਗੁਣ.ਗਿਣਤੀ_2(id);
}

ਖਾਲੀ RTLIL::AttrObject::ਸੈੱਟ_bool_attribute(RTLIL::IdString id, ਤਾਰਕਿਕ ਮੁੱਲ)
{
	ਜੇ (ਮੁੱਲ)
		ਗੁਣ[id] = RTLIL::Const(1);
	ਨਹੀਂਤਾਂ
		ਗੁਣ.erase(id);
}

ਤਾਰਕਿਕ RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ਅਚਲ
{
	ਅਚਲ ਸਵੈਚਾਲਿਤ it = ਗੁਣ.ਲੱਭੋ(id);
	ਜੇ (it == ਗੁਣ.ਸਿਰਾ())
		ਵਾਪਸ ਝੂਠ;
	ਵਾਪਸ it->ਦੂਜਾ.as_bool();
}

ਖਾਲੀ RTLIL::AttrObject::ਸੈੱਟ_string_attribute(RTLIL::IdString id, string ਮੁੱਲ)
{
	ਜੇ (ਮੁੱਲ.ਖਾਲੀ_ਥਾਵਾਂ())
		ਗੁਣ.erase(id);
	ਨਹੀਂਤਾਂ
		ਗੁਣ[id] = ਮੁੱਲ;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ਅਚਲ
{
	std::string ਮੁੱਲ;
	ਅਚਲ ਸਵੈਚਾਲਿਤ it = ਗੁਣ.ਲੱਭੋ(id);
	ਜੇ (it != ਗੁਣ.ਸਿਰਾ())
		ਮੁੱਲ = it->ਦੂਜਾ.decode_string();
	ਵਾਪਸ ਮੁੱਲ;
}

ਖਾਲੀ RTLIL::AttrObject::ਸੈੱਟ_strpool_attribute(RTLIL::IdString id, ਅਚਲ pool<string> &ਅੰਕੜੇ)
{
	string attrval;
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &s : ਅੰਕੜੇ) {
		ਜੇ (!attrval.ਖਾਲੀ_ਥਾਵਾਂ())
			attrval += "|";
		attrval += s;
	}
	ਸੈੱਟ_string_attribute(id, attrval);
}

ਖਾਲੀ RTLIL::AttrObject::ਜੋੜੋ_strpool_attribute(RTLIL::IdString id, ਅਚਲ pool<string> &ਅੰਕੜੇ)
{
	pool<string> union_ਅੰਕੜੇ = get_strpool_attribute(id);
	union_ਅੰਕੜੇ.ਪਾਓ(ਅੰਕੜੇ.begin(), ਅੰਕੜੇ.ਸਿਰਾ());
	ਜੇ (!union_ਅੰਕੜੇ.ਖਾਲੀ_ਥਾਵਾਂ())
		ਸੈੱਟ_strpool_attribute(id, union_ਅੰਕੜੇ);
}

ਖਾਲੀ RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->ਗੁਣ = std::move(buf);
}

ਨਾਮਾਕਾਸ਼ {
	ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ F>
	ਖਾਲੀ walk_process_attr_objects(RTLIL::Process *process, F ਦੇਖੋ) {
		ਦੇਖੋ(process);
		std::ਸਦਿਸ਼<RTLIL::Case_ਨਿਯਮ*> case_ਢੇਰ{&process->ਜੜ੍ਹ_case};
		ਜਦੋਂ (!case_ਢੇਰ.ਖਾਲੀ_ਥਾਵਾਂ()) {
			RTLIL::Case_ਨਿਯਮ *cs = case_ਢੇਰ.ਪਿੱਠ();
			case_ਢੇਰ.ਕੱਢੋ_ਪਿੱਠ();
			ਦੇਖੋ(cs);
			ਲਈ (ਸਵੈਚਾਲਿਤ *sw : cs->switches) {
				ਦੇਖੋ(sw);
				ਲਈ (ਸਵੈਚਾਲਿਤ *case_ : sw->cases)
					case_ਢੇਰ.ਧੱਕੋ_ਪਿੱਠ(case_);
			}
		}
		ਲਈ (ਸਵੈਚਾਲਿਤ *ਸਮਕਾਲੀ : process->syncs)
			ਲਈ (ਸਵੈਚਾਲਿਤ &mwa : ਸਮਕਾਲੀ->mem_write_actions)
				ਦੇਖੋ(&mwa);
	}

	ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ F>
	ਖਾਲੀ walk_attr_objects(RTLIL::Design *design, F ਦੇਖੋ) {
		ਲਈ (ਸਵੈਚਾਲਿਤ &[_, module] : design->modules_) {
			ਦੇਖੋ(module);
			ਲਈ (ਸਵੈਚਾਲਿਤ &[_, wire] : module->wires_)
				ਦੇਖੋ(wire);
			ਲਈ (ਸਵੈਚਾਲਿਤ &[_, mem] : module->memories)
				ਦੇਖੋ(mem);
			ਲਈ (ਸਵੈਚਾਲਿਤ &[_, cell] : module->cells_)
				ਦੇਖੋ(cell);
			ਲਈ (ਸਵੈਚਾਲਿਤ &[_, process] : module->processes)
				walk_process_attr_objects(process, ਦੇਖੋ);
		}
	}
}


ਮਾਪ_t RTLIL::Design::gc_twines()
{
	int64_t ਸ਼ੁਰੂ = PerformanceTimer::query();
	pool<IdString> ਜੀਵਤ;
	ਸਵੈਚਾਲਿਤ ਜੜ੍ਹ = [&](IdString ref) {
		ਜੇ (ref != IdString::Null)
			ਜੀਵਤ.ਪਾਓ(ref);
	};

	walk_attr_objects(ਇਹ, [&](ਅਚਲ RTLIL::AttrObject *obj) {
		ਲਈ (ਸਵੈਚਾਲਿਤ &attr : obj->ਗੁਣ)
			ਜੜ੍ਹ(attr.ਪਹਿਲਾ);
	});

	ਜੜ੍ਹ(selected_ਸਰਗਰਮ_module);

	ਲਈ (ਸਵੈਚਾਲਿਤ &[name, module] : modules_) {
		ਜੜ੍ਹ(name);
		ਲਈ (IdString ਪੋਰਟ : module->ports)
			ਜੜ੍ਹ(ਪੋਰਟ);
		ਲਈ (IdString param : module->avail_parameters)
			ਜੜ੍ਹ(param);
		ਲਈ (ਸਵੈਚਾਲਿਤ &[param, _] : module->parameter_default_ਮੁੱਲਾਂ)
			ਜੜ੍ਹ(param);
		ਲਈ (ਸਵੈਚਾਲਿਤ &[name, _] : module->memories)
			ਜੜ੍ਹ(name);
		ਲਈ (ਸਵੈਚਾਲਿਤ &[name, _] : module->processes)
			ਜੜ੍ਹ(name);
		ਲਈ (ਸਵੈਚਾਲਿਤ &[name, wire] : module->wires_) {
			ਜੜ੍ਹ(name);
			ਜੇ (wire->ਗਿਆਤ_driver())
				ਜੜ੍ਹ(wire->driver_ਪੋਰਟ());
		}
		ਲਈ (ਸਵੈਚਾਲਿਤ &[name, cell] : module->cells_) {
			ਜੜ੍ਹ(name);
			ਜੜ੍ਹ(cell->type);
			ਲਈ (ਸਵੈਚਾਲਿਤ &conn : cell->connections())
				ਜੜ੍ਹ(conn.ਪਹਿਲਾ);
			ਲਈ (ਸਵੈਚਾਲਿਤ &param : cell->parameters)
				ਜੜ੍ਹ(param.ਪਹਿਲਾ);
		}
		ਲਈ (ਸਵੈਚਾਲਿਤ &[_, process] : module->processes) {
			std::ਸਦਿਸ਼<RTLIL::Case_ਨਿਯਮ*> case_ਢੇਰ{&process->ਜੜ੍ਹ_case};
			ਜਦੋਂ (!case_ਢੇਰ.ਖਾਲੀ_ਥਾਵਾਂ()) {
				RTLIL::Case_ਨਿਯਮ *cs = case_ਢੇਰ.ਪਿੱਠ();
				case_ਢੇਰ.ਕੱਢੋ_ਪਿੱਠ();
				ਲਈ (ਸਵੈਚਾਲਿਤ *sw : cs->switches) {
					ਲਈ (ਸਵੈਚਾਲਿਤ *case_ : sw->cases)
						case_ਢੇਰ.ਧੱਕੋ_ਪਿੱਠ(case_);
				}
			}
			ਲਈ (ਸਵੈਚਾਲਿਤ *ਸਮਕਾਲੀ : process->syncs) {
				ਲਈ (ਸਵੈਚਾਲਿਤ &mwa : ਸਮਕਾਲੀ->mem_write_actions)
					ਜੜ੍ਹ(mwa.memid);
			}
		}
	}

	ਸਵੈਚਾਲਿਤ ਜੜ੍ਹ_selection = [&](ਅਚਲ RTLIL::Selection &sel) {
		ਲਈ (IdString m : sel.selected_modules)
			ਜੜ੍ਹ(m);
		ਲਈ (ਸਵੈਚਾਲਿਤ &[m, members] : sel.selected_members) {
			ਜੜ੍ਹ(m);
			ਲਈ (IdString member : members)
				ਜੜ੍ਹ(member);
		}
	};

	ਲਈ (ਸਵੈਚਾਲਿਤ &[name, sel] : selection_vars) {
		ਜੜ੍ਹ(name);
		ਜੜ੍ਹ_selection(sel);
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ &sel : selection_ਢੇਰ)
		ਜੜ੍ਹ_selection(sel);

	ਮਾਪ_t erased = twines_.gc(ਜੀਵਤ);

	int64_t ਸਮਾਂ_ns = PerformanceTimer::query() - ਸ਼ੁਰੂ;
	Pass::ਘਟਾਓ_from_current_runtime_ns(ਸਮਾਂ_ns);
	twine_gc_ns += ਸਮਾਂ_ns;
	++twine_gc_ਗਿਣਤੀ;
	ਵਾਪਸ erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ਅਚਲ
{
	pool<string> ਅੰਕੜੇ;
	ਜੇ (ਗੁਣ.ਗਿਣਤੀ_2(id) != 0)
		ਲਈ (ਸਵੈਚਾਲਿਤ s : ਵੰਡੋ_ਟੋਕਨ(get_string_attribute(id), "|"))
			ਅੰਕੜੇ.ਪਾਓ(s);
	ਵਾਪਸ ਅੰਕੜੇ;
}

ਖਾਲੀ RTLIL::AttrObject::ਸੈੱਟ_hdlname_attribute(ਅਚਲ ਸਦਿਸ਼<string> &hierarchy)
{
	string attrval;
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ident : hierarchy) {
		ਜੇ (!attrval.ਖਾਲੀ_ਥਾਵਾਂ())
			attrval += " ";
		attrval += ident;
	}
	ਸੈੱਟ_string_attribute(ID::hdlname, attrval);
}

ਸਦਿਸ਼<string> RTLIL::AttrObject::get_hdlname_attribute() ਅਚਲ
{
	ਵਾਪਸ ਵੰਡੋ_ਟੋਕਨ(get_string_attribute(ID::hdlname), " ");
}

ਖਾਲੀ RTLIL::AttrObject::ਸੈੱਟ_intvec_attribute(RTLIL::IdString id, ਅਚਲ ਸਦਿਸ਼<ਪੂਰਨ> &ਅੰਕੜੇ)
{
	std::stringstream attrval;
	ਲਈ (ਸਵੈਚਾਲਿਤ &i : ਅੰਕੜੇ) {
		ਜੇ (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	ਗੁਣ[id] = RTLIL::Const(attrval.str());
}

ਸਦਿਸ਼<ਪੂਰਨ> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ਅਚਲ
{
	ਸਦਿਸ਼<ਪੂਰਨ> ਅੰਕੜੇ;
	ਸਵੈਚਾਲਿਤ it = ਗੁਣ.ਲੱਭੋ(id);
	ਜੇ (it != ਗੁਣ.ਸਿਰਾ())
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &s : ਵੰਡੋ_ਟੋਕਨ(ਗੁਣ.at(id).decode_string())) {
			ਅੱਖਰ *ਸਿਰਾ = ਖਾਲੀ_ਸੂਚਕ;
			errno = 0;
			ਲੰਮਾ ਮੁੱਲ = strtol(s.c_str(), &ਸਿਰਾ, 10);
			ਜੇ (ਸਿਰਾ != s.c_str() + s.ਮਾਪ())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			ਜੇ (errno == ERANGE || ਮੁੱਲ < INT_MIN || ਮੁੱਲ > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			ਅੰਕੜੇ.ਧੱਕੋ_ਪਿੱਠ(ਮੁੱਲ);
		}
	ਵਾਪਸ ਅੰਕੜੇ;
}

ਤਾਰਕਿਕ RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ਅਚਲ
{
	ਜੇ (current_design != ਖਾਲੀ_ਸੂਚਕ) {
		ਸਵੈਚਾਲਿਤ module = current_design->module(mod_name);
		ਵਾਪਸ module && module->get_blackbox_attribute();
	} ਨਹੀਂਤਾਂ {
		log_warning("Unable to check if module is boxed for null design.\n");
		ਵਾਪਸ ਝੂਠ;
	}
}

ਤਾਰਕਿਕ RTLIL::Selection::selected_module(RTLIL::IdString mod_name) ਅਚਲ
{
	ਜੇ (complete_selection)
		ਵਾਪਸ ਸੱਚ;
	ਜੇ (!selects_boxes && boxed_module(mod_name))
		ਵਾਪਸ ਝੂਠ;
	ਜੇ (ਭਰਿਆ_selection)
		ਵਾਪਸ ਸੱਚ;
	ਜੇ (selected_modules.ਗਿਣਤੀ_2(mod_name) > 0)
		ਵਾਪਸ ਸੱਚ;
	ਜੇ (selected_members.ਗਿਣਤੀ_2(mod_name) > 0)
		ਵਾਪਸ ਸੱਚ;
	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ RTLIL::Selection::selected_ਸਾਰਾ_module(RTLIL::IdString mod_name) ਅਚਲ
{
	ਜੇ (complete_selection)
		ਵਾਪਸ ਸੱਚ;
	ਜੇ (!selects_boxes && boxed_module(mod_name))
		ਵਾਪਸ ਝੂਠ;
	ਜੇ (ਭਰਿਆ_selection)
		ਵਾਪਸ ਸੱਚ;
	ਜੇ (selected_modules.ਗਿਣਤੀ_2(mod_name) > 0)
		ਵਾਪਸ ਸੱਚ;
	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ਅਚਲ
{
	ਜੇ (complete_selection)
		ਵਾਪਸ ਸੱਚ;
	ਜੇ (!selects_boxes && boxed_module(mod_name))
		ਵਾਪਸ ਝੂਠ;
	ਜੇ (ਭਰਿਆ_selection)
		ਵਾਪਸ ਸੱਚ;
	ਜੇ (selected_modules.ਗਿਣਤੀ_2(mod_name) > 0)
		ਵਾਪਸ ਸੱਚ;
	ਜੇ (selected_members.ਗਿਣਤੀ_2(mod_name) > 0)
		ਜੇ (selected_members.at(mod_name).ਗਿਣਤੀ_2(memb_name) > 0)
			ਵਾਪਸ ਸੱਚ;
	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ RTLIL::Selection::optimize(RTLIL::Design *design)
{
	ਜੇ (design != current_design) {
		current_design = design;
	}

	ਜੇ (selects_boxes && ਭਰਿਆ_selection)
		complete_selection = ਸੱਚ;
	ਜੇ (complete_selection) {
		ਭਰਿਆ_selection = ਝੂਠ;
		selects_boxes = ਸੱਚ;
	}
	ਜੇ (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		ਵਾਪਸ;
	}

	std::ਸਦਿਸ਼<RTLIL::IdString> del_list, ਜੋੜੋ_list;

	del_list.clear();
	ਲਈ (ਸਵੈਚਾਲਿਤ mod_name : selected_modules) {
		ਜੇ (current_design->module(mod_name) == ਖਾਲੀ_ਸੂਚਕ || (!selects_boxes && boxed_module(mod_name)))
			del_list.ਧੱਕੋ_ਪਿੱਠ(mod_name);
		selected_members.erase(mod_name);
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : selected_members)
		ਜੇ (current_design->module(it.ਪਹਿਲਾ) == ਖਾਲੀ_ਸੂਚਕ || (!selects_boxes && boxed_module(it.ਪਹਿਲਾ)))
			del_list.ਧੱਕੋ_ਪਿੱਠ(it.ਪਹਿਲਾ);
	ਲਈ (ਸਵੈਚਾਲਿਤ mod_name : del_list)
		selected_members.erase(mod_name);

	ਲਈ (ਸਵੈਚਾਲਿਤ &it : selected_members) {
		del_list.clear();
		ਲਈ (ਸਵੈਚਾਲਿਤ memb_name : it.ਦੂਜਾ)
			ਜੇ (current_design->module(it.ਪਹਿਲਾ)->ਗਿਣਤੀ_id(memb_name) == 0)
				del_list.ਧੱਕੋ_ਪਿੱਠ(memb_name);
		ਲਈ (ਸਵੈਚਾਲਿਤ memb_name : del_list)
			it.ਦੂਜਾ.erase(memb_name);
	}

	del_list.clear();
	ਜੋੜੋ_list.clear();
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : selected_members)
		ਜੇ (it.ਦੂਜਾ.ਮਾਪ() == 0)
			del_list.ਧੱਕੋ_ਪਿੱਠ(it.ਪਹਿਲਾ);
		ਨਹੀਂਤਾਂ ਜੇ (it.ਦੂਜਾ.ਮਾਪ() == current_design->module(it.ਪਹਿਲਾ)->wires_.ਮਾਪ() + current_design->module(it.ਪਹਿਲਾ)->memories.ਮਾਪ() +
				current_design->module(it.ਪਹਿਲਾ)->cells_.ਮਾਪ() + current_design->module(it.ਪਹਿਲਾ)->processes.ਮਾਪ())
			ਜੋੜੋ_list.ਧੱਕੋ_ਪਿੱਠ(it.ਪਹਿਲਾ);
	ਲਈ (ਸਵੈਚਾਲਿਤ mod_name : del_list)
		selected_members.erase(mod_name);
	ਲਈ (ਸਵੈਚਾਲਿਤ mod_name : ਜੋੜੋ_list) {
		selected_members.erase(mod_name);
		selected_modules.ਪਾਓ(mod_name);
	}

	ਜੇ (selected_modules.ਮਾਪ() == current_design->modules_.ਮਾਪ()) {
		selected_modules.clear();
		selected_members.clear();
		ਜੇ (selects_boxes)
			complete_selection = ਸੱਚ;
		ਨਹੀਂਤਾਂ
			ਭਰਿਆ_selection = ਸੱਚ;
	}
}

ਖਾਲੀ RTLIL::Selection::clear()
{
	ਭਰਿਆ_selection = ਝੂਠ;
	complete_selection = ਝੂਠ;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_ਨਿਰਧਾਰਤ_ਕਰਦਾ (ਨਵਾਂ define_map_t)
{
	ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hashidx_ਗਿਣਤੀ = 123456789;
	hashidx_ਗਿਣਤੀ = mkhash_xorshift(hashidx_ਗਿਣਤੀ);
	hashidx_ = hashidx_ਗਿਣਤੀ;

	refcount_modules_ = 0;
	selected_ਸਰਗਰਮ_module.clear();
	ਧੱਕੋ_ਭਰਿਆ_selection();

	RTLIL::Design::get_all_designs()->ਪਾਓ(std::pair<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Design*>(hashidx_, ਇਹ));
}

RTLIL::Design::~Design()
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &pr : modules_)
		ਮਿਟਾਓ pr.ਦੂਜਾ;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

ਸਥਿਰ std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Design*> all_designs;
std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Design*> *RTLIL::Design::get_all_designs(ਖਾਲੀ)
{
	ਵਾਪਸ &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	ਵਾਪਸ RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ਅਚਲ RTLIL::Module *RTLIL::Design::module(IdString id) ਅਚਲ {
	twines_.check_owned(id);
	ਵਾਪਸ modules_.ਗਿਣਤੀ_2(id) ? modules_.at(id) : ਨਿਰਾ;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	ਵਾਪਸ modules_.ਗਿਣਤੀ_2(id) ? modules_.at(id) : ਨਿਰਾ;
}

RTLIL::Module *RTLIL::Design::ਸਿਖਰ_module() ਅਚਲ
{
	RTLIL::Module *module = ਖਾਲੀ_ਸੂਚਕ;
	ਪੂਰਨ module_ਗਿਣਤੀ = 0;

	ਲਈ (ਸਵੈਚਾਲਿਤ mod : selected_modules()) {
		ਜੇ (mod->get_bool_attribute(ID::ਸਿਖਰ))
			ਵਾਪਸ mod;
		module_ਗਿਣਤੀ++;
		module = mod;
	}

	ਵਾਪਸ module_ਗਿਣਤੀ == 1 ? module : ਖਾਲੀ_ਸੂਚਕ;
}

RTLIL::Module *RTLIL::Design::ਜੋੜੋ_Module(RTLIL::IdString name)
{
	ਜੇ (modules_.ਗਿਣਤੀ_2(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = ਨਵਾਂ RTLIL::Module;
	modules_[name] = module;
	module->design = ਇਹ;
	module->name = name;

	ਲਈ (ਸਵੈਚਾਲਿਤ mon : monitors)
		mon->notify_module_ਜੋੜੋ(module);

	ਜੇ (yosys_xtrace) {
		ਲਘੂਗਣਕ("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	ਵਾਪਸ module;
}

ਖਾਲੀ RTLIL::Design::scratchpad_unset(ਅਚਲ std::string &varname)
{
	scratchpad.erase(varname);
}

ਖਾਲੀ RTLIL::Design::scratchpad_ਸੈੱਟ_int(ਅਚਲ std::string &varname, ਪੂਰਨ ਮੁੱਲ)
{
	scratchpad[varname] = stringf("%d", ਮੁੱਲ);
}

ਖਾਲੀ RTLIL::Design::scratchpad_ਸੈੱਟ_bool(ਅਚਲ std::string &varname, ਤਾਰਕਿਕ ਮੁੱਲ)
{
	scratchpad[varname] = ਮੁੱਲ ? "true" : "false";
}

ਖਾਲੀ RTLIL::Design::scratchpad_ਸੈੱਟ_string(ਅਚਲ std::string &varname, std::string ਮੁੱਲ)
{
	scratchpad[varname] = std::move(ਮੁੱਲ);
}

ਪੂਰਨ RTLIL::Design::scratchpad_get_int(ਅਚਲ std::string &varname, ਪੂਰਨ default_ਮੁੱਲ_2) ਅਚਲ
{
	ਸਵੈਚਾਲਿਤ it = scratchpad.ਲੱਭੋ(varname);
	ਜੇ (it == scratchpad.ਸਿਰਾ())
		ਵਾਪਸ default_ਮੁੱਲ_2;

	ਅਚਲ std::string &str = it->ਦੂਜਾ;

	ਜੇ (str == "0" || str == "false")
		ਵਾਪਸ 0;

	ਜੇ (str == "1" || str == "true")
		ਵਾਪਸ 1;

	ਅੱਖਰ *endptr = ਖਾਲੀ_ਸੂਚਕ;
	ਲੰਮਾ ਪੂਰਨ parsed_ਮੁੱਲ = strtol(str.c_str(), &endptr, 10);
	ਵਾਪਸ *endptr ? default_ਮੁੱਲ_2 : parsed_ਮੁੱਲ;
}

ਤਾਰਕਿਕ RTLIL::Design::scratchpad_get_bool(ਅਚਲ std::string &varname, ਤਾਰਕਿਕ default_ਮੁੱਲ_2) ਅਚਲ
{
	ਸਵੈਚਾਲਿਤ it = scratchpad.ਲੱਭੋ(varname);
	ਜੇ (it == scratchpad.ਸਿਰਾ())
		ਵਾਪਸ default_ਮੁੱਲ_2;

	ਅਚਲ std::string &str = it->ਦੂਜਾ;

	ਜੇ (str == "0" || str == "false")
		ਵਾਪਸ ਝੂਠ;

	ਜੇ (str == "1" || str == "true")
		ਵਾਪਸ ਸੱਚ;

	ਵਾਪਸ default_ਮੁੱਲ_2;
}

std::string RTLIL::Design::scratchpad_get_string(ਅਚਲ std::string &varname, ਅਚਲ std::string &default_ਮੁੱਲ_2) ਅਚਲ
{
	ਸਵੈਚਾਲਿਤ it = scratchpad.ਲੱਭੋ(varname);
	ਜੇ (it == scratchpad.ਸਿਰਾ())
		ਵਾਪਸ default_ਮੁੱਲ_2;

	ਵਾਪਸ it->ਦੂਜਾ;
}

ਖਾਲੀ RTLIL::Design::remove(RTLIL::Module *module)
{
	ਲਈ (ਸਵੈਚਾਲਿਤ mon : monitors)
		mon->notify_module_del(module);

	ਜੇ (yosys_xtrace) {
		ਲਘੂਗਣਕ("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	ਮਿਟਾਓ module;
}

ਖਾਲੀ RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	ਜੋੜੋ(module);
}

ਖਾਲੀ RTLIL::Design::ਛਾਂਟੋ()
{
	scratchpad.ਛਾਂਟੋ();
	modules_.ਛਾਂਟੋ(ਛਾਂਟੋ_by_id_str(twines_));
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : modules_)
		it.ਦੂਜਾ->ਛਾਂਟੋ();
}

ਖਾਲੀ RTLIL::Design::ਛਾਂਟੋ_modules()
{
	scratchpad.ਛਾਂਟੋ();
	modules_.ਛਾਂਟੋ(ਛਾਂਟੋ_by_id_str(twines_));
}

ਖਾਲੀ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

ਖਾਲੀ RTLIL::Design::check()
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NDEBUG
	log_assert(!selection_ਢੇਰ.ਖਾਲੀ_ਥਾਵਾਂ());
	ਪੂਰਨ pool_ਮਾਪ = 0;
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : modules_)
		pool_ਮਾਪ = std::max(pool_ਮਾਪ, ThreadPool::work_pool_ਮਾਪ(0, it.ਦੂਜਾ->cells_ਮਾਪ(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_ਮਾਪ);
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : modules_) {
		log_assert(ਇਹ == it.ਦੂਜਾ->design);
		log_assert(it.ਪਹਿਲਾ == it.ਦੂਜਾ->name);
		check_module(it.ਦੂਜਾ, thread_pool);
	}
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ RTLIL::Design::optimize()
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : modules_)
		it.ਦੂਜਾ->optimize();
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : selection_ਢੇਰ)
		it.optimize(ਇਹ);
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : selection_vars)
		it.ਦੂਜਾ.optimize(ਇਹ);
}

ਖਾਲੀ RTLIL::Design::clone_into(RTLIL::Design *dst) ਅਚਲ
{
	log_assert(dst->modules_.ਖਾਲੀ_ਥਾਵਾਂ());
	dst->twines_ = twines_;
	ਲਈ (ਸਵੈਚਾਲਿਤ it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->ਦੂਜਾ->clone(dst);
}

ਤਾਰਕਿਕ RTLIL::Design::selected_module(IdString mod_name) ਅਚਲ
{
	ਜੇ (!selected_ਸਰਗਰਮ_module.ਖਾਲੀ_ਥਾਵਾਂ() && mod_name != selected_ਸਰਗਰਮ_module)
		ਵਾਪਸ ਝੂਠ;
	ਵਾਪਸ selection().selected_module(mod_name);
}

ਤਾਰਕਿਕ RTLIL::Design::selected_ਸਾਰਾ_module(RTLIL::IdString mod_name) ਅਚਲ
{
	ਜੇ (!selected_ਸਰਗਰਮ_module.ਖਾਲੀ_ਥਾਵਾਂ() && mod_name != selected_ਸਰਗਰਮ_module)
		ਵਾਪਸ ਝੂਠ;
	ਵਾਪਸ selection().selected_ਸਾਰਾ_module(mod_name);
}

ਤਾਰਕਿਕ RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ਅਚਲ
{
	ਜੇ (!selected_ਸਰਗਰਮ_module.ਖਾਲੀ_ਥਾਵਾਂ() && mod_name != selected_ਸਰਗਰਮ_module)
		ਵਾਪਸ ਝੂਠ;
	ਵਾਪਸ selection().selected_member(mod_name, memb_name);
}

ਤਾਰਕਿਕ RTLIL::Design::selected_module(RTLIL::Module *mod) ਅਚਲ
{
	ਵਾਪਸ selected_module(mod->name);
}

ਤਾਰਕਿਕ RTLIL::Design::selected_ਸਾਰਾ_module(RTLIL::Module *mod) ਅਚਲ
{
	ਵਾਪਸ selected_ਸਾਰਾ_module(mod->name);
}

ਖਾਲੀ RTLIL::Design::ਧੱਕੋ_selection(RTLIL::Selection sel)
{
	sel.current_design = ਇਹ;
	selection_ਢੇਰ.ਧੱਕੋ_ਪਿੱਠ(sel);
}

ਖਾਲੀ RTLIL::Design::ਧੱਕੋ_ਖਾਲੀ_ਥਾਵਾਂ_selection()
{
	ਧੱਕੋ_selection(RTLIL::Selection::ਖਾਲੀ_ਥਾਵਾਂ_Selection(ਇਹ));
}

ਖਾਲੀ RTLIL::Design::ਧੱਕੋ_ਭਰਿਆ_selection()
{
	ਧੱਕੋ_selection(RTLIL::Selection::ਭਰਿਆ_Selection(ਇਹ));
}

ਖਾਲੀ RTLIL::Design::ਧੱਕੋ_complete_selection()
{
	ਧੱਕੋ_selection(RTLIL::Selection::CompleteSelection(ਇਹ));
}

ਖਾਲੀ RTLIL::Design::ਕੱਢੋ_selection()
{
	selection_ਢੇਰ.ਕੱਢੋ_ਪਿੱਠ();
	// Default to a full_selection if we ran out of stack
	ਜੇ (selection_ਢੇਰ.ਖਾਲੀ_ਥਾਵਾਂ())
		ਧੱਕੋ_ਭਰਿਆ_selection();
}

std::string RTLIL::Design::ਮੰਜ਼ਿਲ_rtlil_str(ਤਾਰਕਿਕ only_selected) ਅਚਲ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(ਇਹ), only_selected, ਸੱਚ, ਝੂਠ, RTLIL_BACKEND::Dump_ਬਹੁਲਕ::Readable);
	ਵਾਪਸ f.str();
}

std::ਸਦਿਸ਼<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ਅਚਲ
{
	ਤਾਰਕਿਕ include_partials = partials == RTLIL::SELECT_ALL;
	ਤਾਰਕਿਕ exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	ਤਾਰਕਿਕ ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::ਸਦਿਸ਼<RTLIL::Module*> ਨਤੀਜਾ;
	ਨਤੀਜਾ.reserve(modules_.ਮਾਪ());
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : modules_) {
		ਜੇ (selected_ਸਾਰਾ_module(it.ਪਹਿਲਾ) || (include_partials && selected_module(it.ਪਹਿਲਾ))) {
			ਜੇ (!(exclude_boxes && it.ਦੂਜਾ->get_blackbox_attribute(ignore_wb)))
				ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(it.ਦੂਜਾ);
			ਨਹੀਂਤਾਂ
				ਚੋਣ (boxes)
				{
				ਹਾਲਤ RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.ਦੂਜਾ);
					ਰੋਕੋ;
				ਹਾਲਤ RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.ਦੂਜਾ);
					ਰੋਕੋ;
				ਹਾਲਤ RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.ਦੂਜਾ);
					ਰੋਕੋ;
				ਹਾਲਤ RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.ਦੂਜਾ);
					ਰੋਕੋ;
				ਹਾਲਤ RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.ਦੂਜਾ);
					ਰੋਕੋ;
				ਹਾਲਤ RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.ਦੂਜਾ);
					ਰੋਕੋ;
				ਮੂਲ:
					ਰੋਕੋ;
				}
		} ਨਹੀਂਤਾਂ ਜੇ (!include_partials && selected_module(it.ਪਹਿਲਾ)) {
			ਚੋਣ(partials)
			{
			ਹਾਲਤ RTLIL::SELECT_ਸਾਰਾ_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.ਦੂਜਾ);
				ਰੋਕੋ;
			ਹਾਲਤ RTLIL::SELECT_ਸਾਰਾ_ERR:
				log_error("Unsupported partially selected module %s.\n", it.ਦੂਜਾ);
				ਰੋਕੋ;
			ਹਾਲਤ RTLIL::SELECT_ਸਾਰਾ_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.ਦੂਜਾ);
				ਰੋਕੋ;
			ਮੂਲ:
				ਰੋਕੋ;
			}
		}
	}
	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Module::Module()
{
	ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hashidx_ਗਿਣਤੀ = 123456789;
	hashidx_ਗਿਣਤੀ = mkhash_xorshift(hashidx_ਗਿਣਤੀ);
	hashidx_ = hashidx_ਗਿਣਤੀ;

	design = ਖਾਲੀ_ਸੂਚਕ;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->ਪਾਓ(std::pair<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Module*>(hashidx_, ਇਹ));
#ਜੇ_ਅੰਤ
}

RTLIL::Module::~Module()
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &pr : wires_)
		ਮਿਟਾਓ pr.ਦੂਜਾ;
	ਲਈ (ਸਵੈਚਾਲਿਤ &pr : memories)
		ਮਿਟਾਓ pr.ਦੂਜਾ;
	ਲਈ (ਸਵੈਚਾਲਿਤ &pr : cells_)
		ਮਿਟਾਓ pr.ਦੂਜਾ;
	ਲਈ (ਸਵੈਚਾਲਿਤ &pr : processes)
		ਮਿਟਾਓ pr.ਦੂਜਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
ਸਥਿਰ std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Module*> all_modules;
std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Module*> *RTLIL::Module::get_all_modules(ਖਾਲੀ)
{
	ਵਾਪਸ &all_modules;
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	ਲਈ (ਸਵੈਚਾਲਿਤ it = wires_.begin(); it != wires_.ਸਿਰਾ(); ++it)
		ਜੇ (!it->ਦੂਜਾ->ਪੋਰਟ_ਨਿਵੇਸ਼ && !it->ਦੂਜਾ->ਪੋਰਟ_output)
			delwires.ਪਾਓ(it->ਦੂਜਾ);

	ਲਈ (ਸਵੈਚਾਲਿਤ it = memories.begin(); it != memories.ਸਿਰਾ(); ++it)
		ਮਿਟਾਓ it->ਦੂਜਾ;
	memories.clear();

	ਲਈ (ਸਵੈਚਾਲਿਤ it = cells_.begin(); it != cells_.ਸਿਰਾ(); ++it)
		ਮਿਟਾਓ it->ਦੂਜਾ;
	cells_.clear();

	ਲਈ (ਸਵੈਚਾਲਿਤ it = processes.begin(); it != processes.ਸਿਰਾ(); ++it)
		ਮਿਟਾਓ it->ਦੂਜਾ;
	processes.clear();

	connections_.clear();

	remove(delwires);
	ਸੈੱਟ_bool_attribute(ID::blackbox);
}

ਖਾਲੀ RTLIL::Module::expand_interfaces(RTLIL::Design *, ਅਚਲ dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

ਤਾਰਕਿਕ RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	ਵਾਪਸ ਝੂਠ;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ਅਚਲ dict<RTLIL::IdString, RTLIL::Const> &, ਤਾਰਕਿਕ mayfail)
{
	ਜੇ (mayfail)
		ਵਾਪਸ IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ਅਚਲ dict<RTLIL::IdString, RTLIL::Const> &, ਅਚਲ dict<RTLIL::IdString, RTLIL::Module*> &, ਅਚਲ dict<RTLIL::IdString, RTLIL::IdString> &, ਤਾਰਕਿਕ mayfail)
{
	ਜੇ (mayfail)
		ਵਾਪਸ IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

ਮਾਪ_t RTLIL::Module::ਗਿਣਤੀ_id(RTLIL::IdString id)
{
	ਵਾਪਸ wires_.ਗਿਣਤੀ_2(id) + cells_.ਗਿਣਤੀ_2(id) + memories.ਗਿਣਤੀ_2(id) + processes.ਗਿਣਤੀ_2(id);
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NDEBUG
ਨਾਮਾਕਾਸ਼ {
	ਬਣਤਰ ਅੰਦਰੂਨੀ_Cell_Checker
	{
		ਅਚਲ RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		ਅੰਦਰੂਨੀ_Cell_Checker(ਅਚਲ RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		ਖਾਲੀ error(ਪੂਰਨ linenr, std::optional<std::string> error_ਸੁਨੇਹਾ = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			ਜੇ (error_ਸੁਨੇਹਾ) {
				log_error("%s in internal cell %s\n", *error_ਸੁਨੇਹਾ, cell_loc);
			} ਨਹੀਂਤਾਂ {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		ਪੂਰਨ param(RTLIL::IdString name)
		{
			ਸਵੈਚਾਲਿਤ it = cell->parameters.ਲੱਭੋ(name);
			ਜੇ (it == cell->parameters.ਸਿਰਾ()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.ਪਾਓ(name);
			ਵਾਪਸ it->ਦੂਜਾ.as_int();
		}

		ਪੂਰਨ param_bool(RTLIL::IdString name)
		{
			ਪੂਰਨ v = param(name);
			ਜੇ (Get_ਮਾਪ(cell->parameters.at(name)) > 32)
				error(__LINE__);
			ਜੇ (v != 0 && v != 1)
				error(__LINE__);
			ਵਾਪਸ v;
		}

		ਪੂਰਨ param_bool(RTLIL::IdString name, ਤਾਰਕਿਕ expected)
		{
			ਪੂਰਨ v = param_bool(name);
			ਜੇ (v != expected)
				error(__LINE__);
			ਵਾਪਸ v;
		}

		ਖਾਲੀ param_ਬਿੱਟ(RTLIL::IdString name, ਪੂਰਨ ਚੌੜਾਈ)
		{
			param(name);
			ਜੇ (Get_ਮਾਪ(cell->parameters.at(name)) != ਚੌੜਾਈ)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			ਵਾਪਸ cell->parameters.at(name).decode_string();
		}

		ਖਾਲੀ ਪੋਰਟ(RTLIL::IdString name, ਪੂਰਨ ਚੌੜਾਈ)
		{
			ਸਵੈਚਾਲਿਤ it = cell->connections_.ਲੱਭੋ(name);
			ਜੇ (it == cell->connections_.ਸਿਰਾ()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			ਜੇ (Get_ਮਾਪ(it->ਦੂਜਾ) != ਚੌੜਾਈ) {
				std::string err = stringf("Expected port to have width %d", ਚੌੜਾਈ);
				error(__LINE__, err);
			}
			expected_ports.ਪਾਓ(name);
		}

		ਖਾਲੀ check_expected(ਤਾਰਕਿਕ check_matched_sign = ਝੂਠ)
		{
			ਲਈ (ਸਵੈਚਾਲਿਤ &para : cell->parameters)
				ਜੇ (expected_params.ਗਿਣਤੀ_2(para.ਪਹਿਲਾ) == 0)
					error(__LINE__);
			ਲਈ (ਸਵੈਚਾਲਿਤ &conn : cell->connections())
				ਜੇ (expected_ports.ਗਿਣਤੀ_2(conn.ਪਹਿਲਾ) == 0)
					error(__LINE__);

			ਜੇ (check_matched_sign) {
				log_assert(expected_params.ਗਿਣਤੀ_2(ID::A_SIGNED) != 0 && expected_params.ਗਿਣਤੀ_2(ID::B_SIGNED) != 0);
				ਤਾਰਕਿਕ a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				ਤਾਰਕਿਕ b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				ਜੇ (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		ਖਾਲੀ check()
		{
			ਜੇ (cell->type.is_ਜਨਤਕ() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				ਵਾਪਸ;

			ਜੇ (cell->type.ਵਿੱਚ(ID($buf), ID($barrier))) {
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type.ਵਿੱਚ(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				ਪੋਰਟ(ID::A, param(ID::A_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::Y_ਚੌੜਾਈ));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type.ਵਿੱਚ(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				ਪੋਰਟ(ID::A, param(ID::A_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::B, param(ID::B_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::Y_ਚੌੜਾਈ));
				check_expected(ਸੱਚ);
				ਵਾਪਸ;
			}

			ਜੇ (cell->type.ਵਿੱਚ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				ਪੋਰਟ(ID::A, param(ID::A_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::Y_ਚੌੜਾਈ));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type.ਵਿੱਚ(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/ਝੂਠ);
				ਪੋਰਟ(ID::A, param(ID::A_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::B, param(ID::B_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::Y_ਚੌੜਾਈ));
				check_expected(/*check_matched_sign=*/ਝੂਠ);
				ਵਾਪਸ;
			}

			ਜੇ (cell->type.ਵਿੱਚ(ID($ਖਿਸਕਾਓ), ID($shiftx))) {
				ਜੇ (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/ਝੂਠ);
				} ਨਹੀਂਤਾਂ {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				ਪੋਰਟ(ID::A, param(ID::A_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::B, param(ID::B_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::Y_ਚੌੜਾਈ));
				check_expected(/*check_matched_sign=*/ਝੂਠ);
				ਵਾਪਸ;
			}

			ਜੇ (cell->type.ਵਿੱਚ(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				ਪੋਰਟ(ID::A, param(ID::A_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::B, param(ID::B_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::Y_ਚੌੜਾਈ));
				check_expected(ਸੱਚ);
				ਵਾਪਸ;
			}

			ਜੇ (cell->type.ਵਿੱਚ(ID($ਜੋੜੋ), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ਘਾਤ))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				ਪੋਰਟ(ID::A, param(ID::A_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::B, param(ID::B_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::Y_ਚੌੜਾਈ));
				check_expected(cell->type != ID($ਘਾਤ));
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($fa)) {
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::B, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::C, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::X, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($lcu)) {
				ਪੋਰਟ(ID::P, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::G, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::CI, 1);
				ਪੋਰਟ(ID::CO, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				ਪੋਰਟ(ID::A, param(ID::A_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::B, param(ID::B_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::CI, 1);
				ਪੋਰਟ(ID::BI, 1);
				ਪੋਰਟ(ID::X, param(ID::Y_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::Y_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::CO, param(ID::Y_ਚੌੜਾਈ));
				check_expected(ਸੱਚ);
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_ਚੌੜਾਈ);
				ਪੋਰਟ(ID::A, param(ID::A_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::B, param(ID::B_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::Y_ਚੌੜਾਈ));
				check_expected();
				Macc().from_cell(cell);
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($macc_v2)) {
				ਜੇ (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				ਜੇ (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_ਬਿੱਟ(ID::ਗੁਣਨਫਲ_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_ਬਿੱਟ(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_ਬਿੱਟ(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_ਬਿੱਟ(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_ਬਿੱਟ(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				ਜੇ (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_ਬਿੱਟ(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_ਬਿੱਟ(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_ਬਿੱਟ(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ਅਚਲ Const &a_ਚੌੜਾਈ = cell->getParam(ID::A_WIDTHS);
				ਅਚਲ Const &b_ਚੌੜਾਈ = cell->getParam(ID::B_WIDTHS);
				ਅਚਲ Const &c_ਚੌੜਾਈ = cell->getParam(ID::C_WIDTHS);
				ਪੂਰਨ a_ਚੌੜਾਈ_ਜੋੜ = 0, b_ਚੌੜਾਈ_ਜੋੜ = 0, c_ਚੌੜਾਈ_ਜੋੜ = 0;
				ਲਈ (ਪੂਰਨ i = 0; i < param(ID::NPRODUCTS); i++) {
					a_ਚੌੜਾਈ_ਜੋੜ += a_ਚੌੜਾਈ.ਕੱਢੋ_2(16 * i, 16).as_int(ਝੂਠ);
					b_ਚੌੜਾਈ_ਜੋੜ += b_ਚੌੜਾਈ.ਕੱਢੋ_2(16 * i, 16).as_int(ਝੂਠ);
				}
				ਲਈ (ਪੂਰਨ i = 0; i < param(ID::NADDENDS); i++) {
					c_ਚੌੜਾਈ_ਜੋੜ += c_ਚੌੜਾਈ.ਕੱਢੋ_2(16 * i, 16).as_int(ਝੂਠ);
				}
				ਪੋਰਟ(ID::A, a_ਚੌੜਾਈ_ਜੋੜ);
				ਪੋਰਟ(ID::B, b_ਚੌੜਾਈ_ਜੋੜ);
				ਪੋਰਟ(ID::C, c_ਚੌੜਾਈ_ਜੋੜ);
				ਪੋਰਟ(ID::Y, param(ID::Y_ਚੌੜਾਈ));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				ਪੋਰਟ(ID::A, param(ID::A_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::Y_ਚੌੜਾਈ));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type.ਵਿੱਚ(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				ਪੋਰਟ(ID::A, param(ID::A_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::B, param(ID::B_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::Y_ਚੌੜਾਈ));
				check_expected(/*check_matched_sign=*/ਝੂਠ);
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($ਟੁਕੜਾ)) {
				param(ID::ਵਿੱਥ_2);
				ਪੋਰਟ(ID::A, param(ID::A_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::Y_ਚੌੜਾਈ));
				ਜੇ (param(ID::ਵਿੱਥ_2) + param(ID::Y_ਚੌੜਾਈ) > param(ID::A_ਚੌੜਾਈ))
					error(__LINE__);
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($concat)) {
				ਪੋਰਟ(ID::A, param(ID::A_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::B, param(ID::B_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::A_ਚੌੜਾਈ) + param(ID::B_ਚੌੜਾਈ));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($mux)) {
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::B, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::S, 1);
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($pmux)) {
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::B, param(ID::ਚੌੜਾਈ_2) * param(ID::S_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::S, param(ID::S_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($bmux)) {
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2) << param(ID::S_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::S, param(ID::S_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($demux)) {
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::S, param(ID::S_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2) << param(ID::S_ਚੌੜਾਈ));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($lut)) {
				param(ID::LUT);
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Y, 1);
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($sop)) {
				param(ID::ਡੂੰਘਾਈ_2);
				param(ID::ਸਾਰਣੀ);
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Y, 1);
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($sr)) {
				param_bool(ID::ਸੈੱਟ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				ਪੋਰਟ(ID::ਸੈੱਟ_2, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::CLR, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q,   param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($ff)) {
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::EN, 1);
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ਸੈੱਟ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::ਸੈੱਟ_2, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::CLR, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($dffsre)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ਸੈੱਟ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				param_bool(ID::EN_POLARITY);
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::EN, 1);
				ਪੋਰਟ(ID::ਸੈੱਟ_2, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::CLR, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_ਬਿੱਟ(ID::ARST_ਮੁੱਲ, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::ARST, 1);
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_ਬਿੱਟ(ID::SRST_ਮੁੱਲ, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::SRST, 1);
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type.ਵਿੱਚ(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_ਬਿੱਟ(ID::SRST_ਮੁੱਲ, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::EN, 1);
				ਪੋਰਟ(ID::SRST, 1);
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_ਬਿੱਟ(ID::ARST_ਮੁੱਲ, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::EN, 1);
				ਪੋਰਟ(ID::ARST, 1);
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::ALOAD, 1);
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::AD, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($aldffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::EN, 1);
				ਪੋਰਟ(ID::ALOAD, 1);
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::AD, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				ਪੋਰਟ(ID::EN, 1);
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_ਬਿੱਟ(ID::ARST_ਮੁੱਲ, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::EN, 1);
				ਪੋਰਟ(ID::ARST, 1);
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ਸੈੱਟ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				ਪੋਰਟ(ID::EN, 1);
				ਪੋਰਟ(ID::ਸੈੱਟ_2, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::CLR, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_ਬਿੱਟ);
				param(ID::STATE_ਅੰਕ);
				param(ID::STATE_ਅੰਕ_LOG2);
				param(ID::STATE_RST);
				param_ਬਿੱਟ(ID::STATE_ਸਾਰਣੀ, param(ID::STATE_ਬਿੱਟ) * param(ID::STATE_ਅੰਕ));
				param(ID::TRANS_ਅੰਕ);
				param_ਬਿੱਟ(ID::TRANS_ਸਾਰਣੀ, param(ID::TRANS_ਅੰਕ) * (2*param(ID::STATE_ਅੰਕ_LOG2) + param(ID::CTRL_ਵਿੱਚ_ਚੌੜਾਈ) + param(ID::CTRL_ਬਾਹਰਲਾ_ਚੌੜਾਈ)));
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::ARST, 1);
				ਪੋਰਟ(ID::CTRL_ਵਿੱਚ, param(ID::CTRL_ਵਿੱਚ_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::CTRL_ਬਾਹਰਲਾ, param(ID::CTRL_ਬਾਹਰਲਾ_ਚੌੜਾਈ));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::EN, 1);
				ਪੋਰਟ(ID::ADDR, param(ID::ABITS));
				ਪੋਰਟ(ID::ਅੰਕੜੇ_2, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_ਨਕਾਬ);
				param(ID::COLLISION_X_ਨਕਾਬ);
				param_bool(ID::CE_OVER_SRST);
				param_ਬਿੱਟ(ID::ARST_ਮੁੱਲ, param(ID::ਚੌੜਾਈ_2));
				param_ਬਿੱਟ(ID::SRST_ਮੁੱਲ, param(ID::ਚੌੜਾਈ_2));
				param_ਬਿੱਟ(ID::INIT_ਮੁੱਲ, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::EN, 1);
				ਪੋਰਟ(ID::ARST, 1);
				ਪੋਰਟ(ID::SRST, 1);
				ਪੋਰਟ(ID::ADDR, param(ID::ABITS));
				ਪੋਰਟ(ID::ਅੰਕੜੇ_2, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::ਤਰਜੀਹ);
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::EN, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::ADDR, param(ID::ABITS));
				ਪੋਰਟ(ID::ਅੰਕੜੇ_2, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($memwr_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PORTID);
				param(ID::ਤਰਜੀਹ_ਨਕਾਬ_2);
				ਪੋਰਟ(ID::CLK, 1);
				ਪੋਰਟ(ID::EN, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::ADDR, param(ID::ABITS));
				ਪੋਰਟ(ID::ਅੰਕੜੇ_2, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::ਤਰਜੀਹ);
				ਪੋਰਟ(ID::ADDR, param(ID::ABITS));
				ਪੋਰਟ(ID::ਅੰਕੜੇ_2, param(ID::ਚੌੜਾਈ_2) * param(ID::ਸ਼ਬਦ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::ਤਰਜੀਹ);
				ਪੋਰਟ(ID::ADDR, param(ID::ABITS));
				ਪੋਰਟ(ID::ਅੰਕੜੇ_2, param(ID::ਚੌੜਾਈ_2) * param(ID::ਸ਼ਬਦ_2));
				ਪੋਰਟ(ID::EN, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($mem)) {
				param(ID::MEMID);
				param(ID::ਮਾਪ_2);
				param(ID::ਵਿੱਥ_2);
				param(ID::INIT);
				param_ਬਿੱਟ(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_ਬਿੱਟ(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_ਬਿੱਟ(ID::RD_TRANSPARENT, max(1, param(ID::RD_PORTS)));
				param_ਬਿੱਟ(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_ਬਿੱਟ(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				ਪੋਰਟ(ID::RD_CLK, param(ID::RD_PORTS));
				ਪੋਰਟ(ID::RD_EN, param(ID::RD_PORTS));
				ਪੋਰਟ(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				ਪੋਰਟ(ID::RD_ਅੰਕੜੇ, param(ID::RD_PORTS) * param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::WR_CLK, param(ID::WR_PORTS));
				ਪੋਰਟ(ID::WR_EN, param(ID::WR_PORTS) * param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				ਪੋਰਟ(ID::WR_ਅੰਕੜੇ, param(ID::WR_PORTS) * param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($mem_v2)) {
				param(ID::MEMID);
				param(ID::ਮਾਪ_2);
				param(ID::ਵਿੱਥ_2);
				param(ID::INIT);
				param_ਬਿੱਟ(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_ਬਿੱਟ(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_ਬਿੱਟ(ID::RD_TRANSPARENCY_ਨਕਾਬ, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_ਬਿੱਟ(ID::RD_COLLISION_X_ਨਕਾਬ, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_ਬਿੱਟ(ID::RD_WIDE_CONTINUATION, max(1, param(ID::RD_PORTS)));
				param_ਬਿੱਟ(ID::RD_CE_OVER_SRST, max(1, param(ID::RD_PORTS)));
				param_ਬਿੱਟ(ID::RD_ARST_ਮੁੱਲ, max(1, param(ID::RD_PORTS) * param(ID::ਚੌੜਾਈ_2)));
				param_ਬਿੱਟ(ID::RD_SRST_ਮੁੱਲ, max(1, param(ID::RD_PORTS) * param(ID::ਚੌੜਾਈ_2)));
				param_ਬਿੱਟ(ID::RD_INIT_ਮੁੱਲ, max(1, param(ID::RD_PORTS) * param(ID::ਚੌੜਾਈ_2)));
				param_ਬਿੱਟ(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_ਬਿੱਟ(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				param_ਬਿੱਟ(ID::WR_WIDE_CONTINUATION, max(1, param(ID::WR_PORTS)));
				param_ਬਿੱਟ(ID::WR_ਤਰਜੀਹ_ਨਕਾਬ, max(1, param(ID::WR_PORTS) * param(ID::WR_PORTS)));
				ਪੋਰਟ(ID::RD_CLK, param(ID::RD_PORTS));
				ਪੋਰਟ(ID::RD_EN, param(ID::RD_PORTS));
				ਪੋਰਟ(ID::RD_ARST, param(ID::RD_PORTS));
				ਪੋਰਟ(ID::RD_SRST, param(ID::RD_PORTS));
				ਪੋਰਟ(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				ਪੋਰਟ(ID::RD_ਅੰਕੜੇ, param(ID::RD_PORTS) * param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::WR_CLK, param(ID::WR_PORTS));
				ਪੋਰਟ(ID::WR_EN, param(ID::WR_PORTS) * param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				ਪੋਰਟ(ID::WR_ਅੰਕੜੇ, param(ID::WR_PORTS) * param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($tribuf)) {
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::EN, 1);
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($bweqx)) {
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::B, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($bwmux)) {
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::B, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::S, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type.ਵਿੱਚ(ID($ਪ੍ਰਤਿਜ੍ਞਾ), ID($assume), ID($ਜੀਵਤ), ID($fair), ID($cover))) {
				ਪੋਰਟ(ID::A, 1);
				ਪੋਰਟ(ID::EN, 1);
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($initstate)) {
				ਪੋਰਟ(ID::Y, 1);
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type.ਵਿੱਚ(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type.ਵਿੱਚ(ID($anyinit))) {
				ਪੋਰਟ(ID::D, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Q, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($equiv)) {
				ਪੋਰਟ(ID::A, 1);
				ਪੋਰਟ(ID::B, 1);
				ਪੋਰਟ(ID::Y, 1);
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type.ਵਿੱਚ(ID($specify2), ID($specify3))) {
				param_bool(ID::ਭਰਿਆ);
				param_bool(ID::SRC_DST_PEN);
				param_bool(ID::SRC_DST_POL);
				param(ID::T_RISE_MIN);
				param(ID::T_RISE_TYP);
				param(ID::T_RISE_MAX);
				param(ID::T_FALL_MIN);
				param(ID::T_FALL_TYP);
				param(ID::T_FALL_MAX);
				ਪੋਰਟ(ID::EN, 1);
				ਪੋਰਟ(ID::SRC, param(ID::SRC_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::DST, param(ID::DST_ਚੌੜਾਈ));
				ਜੇ (cell->type == ID($specify3)) {
					param_bool(ID::ਕਿਨਾਰਾ_EN);
					param_bool(ID::ਕਿਨਾਰਾ_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					ਪੋਰਟ(ID::DAT, param(ID::DST_ਚੌੜਾਈ));
				}
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($specrule)) {
				param(ID::TYPE);
				param_bool(ID::SRC_PEN);
				param_bool(ID::SRC_POL);
				param_bool(ID::DST_PEN);
				param_bool(ID::DST_POL);
				param(ID::T_ਹੱਦਬੰਦੀ_MIN);
				param(ID::T_ਹੱਦਬੰਦੀ_TYP);
				param(ID::T_ਹੱਦਬੰਦੀ_MAX);
				param(ID::T_LIMIT2_MIN);
				param(ID::T_LIMIT2_TYP);
				param(ID::T_LIMIT2_MAX);
				ਪੋਰਟ(ID::SRC_EN, 1);
				ਪੋਰਟ(ID::DST_EN, 1);
				ਪੋਰਟ(ID::SRC, param(ID::SRC_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::DST, param(ID::DST_ਚੌੜਾਈ));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::ਤਰਜੀਹ);
				ਪੋਰਟ(ID::EN, 1);
				ਪੋਰਟ(ID::TRG, param(ID::TRG_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::ਦਲੀਲਾਂ, param(ID::ਦਲੀਲਾਂ_ਚੌੜਾਈ));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				ਜੇ (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
					error(__LINE__);
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::ਤਰਜੀਹ);
				ਪੋਰਟ(ID::A, 1);
				ਪੋਰਟ(ID::EN, 1);
				ਪੋਰਟ(ID::TRG, param(ID::TRG_ਚੌੜਾਈ));
				ਪੋਰਟ(ID::ਦਲੀਲਾਂ, param(ID::ਦਲੀਲਾਂ_ਚੌੜਾਈ));
				check_expected();
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				ਜੇ (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				ਵਾਪਸ;
			}

			ਜੇ (cell->type == ID($_BUF_))    { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_NOT_))    { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_AND_))    { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_NAND_))   { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_OR_))     { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_NOR_))    { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_XOR_))    { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_XNOR_))   { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_ANDNOT_)) { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_ORNOT_))  { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_MUX_))    { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::S,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_NMUX_))   { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::S,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_AOI3_))   { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_OAI3_))   { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_AOI4_))   { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_OAI4_))   { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type == ID($_TBUF_))  { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::Y,1); ਪੋਰਟ(ID::E,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type == ID($_MUX4_))  { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::S,1); ਪੋਰਟ(ID::T,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_MUX8_))  { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::E,1); ਪੋਰਟ(ID::F,1); ਪੋਰਟ(ID::G,1); ਪੋਰਟ(ID::H,1); ਪੋਰਟ(ID::S,1); ਪੋਰਟ(ID::T,1); ਪੋਰਟ(ID::U,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }
			ਜੇ (cell->type == ID($_MUX16_)) { ਪੋਰਟ(ID::A,1); ਪੋਰਟ(ID::B,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::E,1); ਪੋਰਟ(ID::F,1); ਪੋਰਟ(ID::G,1); ਪੋਰਟ(ID::H,1); ਪੋਰਟ(ID::I,1); ਪੋਰਟ(ID::J,1); ਪੋਰਟ(ID::K,1); ਪੋਰਟ(ID::L,1); ਪੋਰਟ(ID::M,1); ਪੋਰਟ(ID::N,1); ਪੋਰਟ(ID::O,1); ਪੋਰਟ(ID::P,1); ਪੋਰਟ(ID::S,1); ਪੋਰਟ(ID::T,1); ਪੋਰਟ(ID::U,1); ਪੋਰਟ(ID::V,1); ਪੋਰਟ(ID::Y,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ ਪੋਰਟ(ID::S,1); ਪੋਰਟ(ID::R,1); ਪੋਰਟ(ID::Q,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type == ID($_FF_)) { ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Q,1); check_expected();  ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(ID($_DFF_N_), ID($_DFF_P_)))
				{ ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Q,1); ਪੋਰਟ(ID::C,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Q,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::E,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Q,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::R,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Q,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::R,1); ਪੋਰਟ(ID::E,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Q,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::L,1); ਪੋਰਟ(ID::AD,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Q,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::L,1); ਪੋਰਟ(ID::AD,1); ਪੋਰਟ(ID::E,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::S,1); ਪੋਰਟ(ID::R,1); ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Q,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::S,1); ਪੋਰਟ(ID::R,1); ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::E,1); ਪੋਰਟ(ID::Q,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Q,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::R,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Q,1); ਪੋਰਟ(ID::C,1); ਪੋਰਟ(ID::R,1); ਪੋਰਟ(ID::E,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ ਪੋਰਟ(ID::E,1); ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Q,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ ਪੋਰਟ(ID::E,1); ਪੋਰਟ(ID::R,1); ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Q,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ ਪੋਰਟ(ID::E,1); ਪੋਰਟ(ID::S,1); ਪੋਰਟ(ID::R,1); ਪੋਰਟ(ID::D,1); ਪੋਰਟ(ID::Q,1); check_expected(); ਵਾਪਸ; }

			ਜੇ (cell->type.ਵਿੱਚ(ID($ਸੈੱਟ_ਟੈਗ))) {
				param(ID::ਚੌੜਾਈ_2);
				param(ID::ਟੈਗ_2);
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::ਸੈੱਟ_2, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::CLR, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}
			ਜੇ (cell->type.ਵਿੱਚ(ID($get_ਟੈਗ),ID($original_ਟੈਗ))) {
				param(ID::ਚੌੜਾਈ_2);
				param(ID::ਟੈਗ_2);
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}
			ਜੇ (cell->type.ਵਿੱਚ(ID($overwrite_ਟੈਗ))) {
				param(ID::ਚੌੜਾਈ_2);
				param(ID::ਟੈਗ_2);
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::ਸੈੱਟ_2, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::CLR, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}
			ਜੇ (cell->type.ਵਿੱਚ(ID($future_ff))) {
				param(ID::ਚੌੜਾਈ_2);
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}
			ਜੇ (cell->type.ਵਿੱਚ(ID($ਨਿਵੇਸ਼_ਪੋਰਟ))) {
				param(ID::ਚੌੜਾਈ_2);
				ਪੋਰਟ(ID::Y, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
			}
			ਜੇ (cell->type.ਵਿੱਚ(ID($connect))) {
				param(ID::ਚੌੜਾਈ_2);
				ਪੋਰਟ(ID::A, param(ID::ਚੌੜਾਈ_2));
				ਪੋਰਟ(ID::B, param(ID::ਚੌੜਾਈ_2));
				check_expected();
				ਵਾਪਸ;
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
#ਜੇ_ਅੰਤ

ਖਾਲੀ RTLIL::Module::ਛਾਂਟੋ()
{
	wires_.ਛਾਂਟੋ(ਛਾਂਟੋ_by_id_str(design->twines()));
	cells_.ਛਾਂਟੋ(ਛਾਂਟੋ_by_id_str(design->twines()));
	parameter_default_ਮੁੱਲਾਂ.ਛਾਂਟੋ(ਛਾਂਟੋ_by_id_str(design->twines()));
	memories.ਛਾਂਟੋ(ਛਾਂਟੋ_by_id_str(design->twines()));
	processes.ਛਾਂਟੋ(ਛਾਂਟੋ_by_id_str(design->twines()));
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : cells_)
		it.ਦੂਜਾ->ਛਾਂਟੋ();
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : wires_)
		it.ਦੂਜਾ->ਗੁਣ.ਛਾਂਟੋ(ਛਾਂਟੋ_by_id_str(design->twines()));
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : memories)
		it.ਦੂਜਾ->ਗੁਣ.ਛਾਂਟੋ(ਛਾਂਟੋ_by_id_str(design->twines()));
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NDEBUG
ਸਥਿਰ ਖਾਲੀ check_id_ਕੁੰਜੀਆਂ(ਅਚਲ dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : keyed)
		log_assert(!it.ਪਹਿਲਾ.ਖਾਲੀ_ਥਾਵਾਂ());
}
#ਜੇ_ਅੰਤ

ਖਾਲੀ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_ਮਾਪ(0, module->cells_ਮਾਪ(), 1000));
	ਅਚਲ RTLIL::Module *const_module = module;

	pool<std::string> ਸਮ੍ਰਿਤੀ_ਲੜੀਆਂ;
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : module->memories) {
		log_assert(it.ਪਹਿਲਾ == it.ਦੂਜਾ->name);
		log_assert(it.ਪਹਿਲਾ != IdString::Null);
		log_assert(it.ਦੂਜਾ->ਚੌੜਾਈ >= 0);
		log_assert(it.ਦੂਜਾ->ਮਾਪ >= 0);
		check_id_ਕੁੰਜੀਆਂ(it.ਦੂਜਾ->ਗੁਣ);
		ਸਮ੍ਰਿਤੀ_ਲੜੀਆਂ.ਪਾਓ(it.ਦੂਜਾ->name.str());
	}

	std::ਸਦਿਸ਼<MonotonicFlag> ports_declared(Get_ਮਾਪ(module->ports));
	Sharded_ਸਦਿਸ਼<std::string> memids(subpool);
	subpool.ਲੜੀਵਾਰ([const_module, &ports_declared, &ਸਮ੍ਰਿਤੀ_ਲੜੀਆਂ, &memids](ਅਚਲ ParallelDispatchThreadPool::ਲੜੀਵਾਰ_Ctx &ctx) {
		ਲਈ (ਪੂਰਨ i : ctx.ਵਸਤੂ_range(const_module->cells_ਮਾਪ())) {
			ਸਵੈਚਾਲਿਤ it = *const_module->cells_.element(i);
			log_assert(const_module == it.ਦੂਜਾ->module);
			log_assert(it.ਪਹਿਲਾ == it.ਦੂਜਾ->name);
			log_assert(it.ਪਹਿਲਾ != IdString::Null);
			log_assert(!it.ਦੂਜਾ->type.ਖਾਲੀ_ਥਾਵਾਂ());
			ਲਈ (ਸਵੈਚਾਲਿਤ &it2 : it.ਦੂਜਾ->connections()) {
				log_assert(it2.ਪਹਿਲਾ != IdString::Null);
				it2.ਦੂਜਾ.check(const_module);
			}
			check_id_ਕੁੰਜੀਆਂ(it.ਦੂਜਾ->ਗੁਣ);
			check_id_ਕੁੰਜੀਆਂ(it.ਦੂਜਾ->parameters);
			ਅੰਦਰੂਨੀ_Cell_Checker checker(const_module, it.ਦੂਜਾ);
			checker.check();
			ਜੇ (it.ਦੂਜਾ->has_memid()) {
				log_assert(ਸਮ੍ਰਿਤੀ_ਲੜੀਆਂ.ਗਿਣਤੀ_2(it.ਦੂਜਾ->parameters.at(ID::MEMID).decode_string()));
			} ਨਹੀਂਤਾਂ ਜੇ (it.ਦੂਜਾ->is_mem_cell()) {
				std::string memid = it.ਦੂਜਾ->parameters.at(ID::MEMID).decode_string();
				log_assert(!ਸਮ੍ਰਿਤੀ_ਲੜੀਆਂ.ਗਿਣਤੀ_2(memid));
				memids.ਪਾਓ(ctx, std::move(memid));
			}
			ਸਵੈਚਾਲਿਤ cell_mod = const_module->design->module(it.ਦੂਜਾ->name);
			ਜੇ (cell_mod != ਖਾਲੀ_ਸੂਚਕ) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GNUC__
				#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
				#ਹਦਾਇਤ GCC diagnostic ignored "-Wdeprecated-declarations"
				#ਜੇ_ਅੰਤ
				log_assert(!it.ਦੂਜਾ->get_blackbox_attribute());
				#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GNUC__
				#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
				#ਜੇ_ਅੰਤ
			}
		}

		ਲਈ (ਪੂਰਨ i : ctx.ਵਸਤੂ_range(const_module->wires_ਮਾਪ())) {
			ਸਵੈਚਾਲਿਤ it = *const_module->wires_.element(i);
			log_assert(const_module == it.ਦੂਜਾ->module);
			log_assert(it.ਪਹਿਲਾ == it.ਦੂਜਾ->name);
			log_assert(it.ਪਹਿਲਾ != IdString::Null);
			log_assert(it.ਦੂਜਾ->ਚੌੜਾਈ >= 0);
			log_assert(it.ਦੂਜਾ->ਪੋਰਟ_id >= 0);
			check_id_ਕੁੰਜੀਆਂ(it.ਦੂਜਾ->ਗੁਣ);
			ਜੇ (it.ਦੂਜਾ->ਪੋਰਟ_id) {
				log_assert(Get_ਮਾਪ(const_module->ports) >= it.ਦੂਜਾ->ਪੋਰਟ_id);
				log_assert(const_module->ports.at(it.ਦੂਜਾ->ਪੋਰਟ_id-1) == it.ਦੂਜਾ->name);
				log_assert(it.ਦੂਜਾ->ਪੋਰਟ_ਨਿਵੇਸ਼ || it.ਦੂਜਾ->ਪੋਰਟ_output);
				log_assert(it.ਦੂਜਾ->ਪੋਰਟ_id <= Get_ਮਾਪ(ports_declared));
				ਤਾਰਕਿਕ previously_declared = ports_declared[it.ਦੂਜਾ->ਪੋਰਟ_id-1].ਸੈੱਟ_and_return_old();
				log_assert(previously_declared == ਝੂਠ);
			} ਨਹੀਂਤਾਂ
				log_assert(!it.ਦੂਜਾ->ਪੋਰਟ_ਨਿਵੇਸ਼ && !it.ਦੂਜਾ->ਪੋਰਟ_output);
		}
	});
	ਲਈ (ਅਚਲ MonotonicFlag &ਪੋਰਟ_declared : ports_declared)
		log_assert(ਪੋਰਟ_declared.ਲੱਦੋ() == ਸੱਚ);
	pool<std::string> memids_pool;
	ਲਈ (std::string &memid : memids)
		log_assert(memids_pool.ਪਾਓ(memid).ਦੂਜਾ);

	ਲਈ (ਸਵੈਚਾਲਿਤ &it : module->processes) {
		log_assert(it.ਪਹਿਲਾ == it.ਦੂਜਾ->name);
		log_assert(it.ਪਹਿਲਾ != IdString::Null);
		log_assert(it.ਦੂਜਾ->ਜੜ੍ਹ_case.compare.ਖਾਲੀ_ਥਾਵਾਂ());
		std::ਸਦਿਸ਼<RTLIL::Case_ਨਿਯਮ*> all_cases = {&it.ਦੂਜਾ->ਜੜ੍ਹ_case};
		ਲਈ (ਮਾਪ_t i = 0; i < all_cases.ਮਾਪ(); i++) {
			ਲਈ (ਸਵੈਚਾਲਿਤ &switch_it : all_cases[i]->switches) {
				ਲਈ (ਸਵੈਚਾਲਿਤ &case_it : switch_it->cases) {
					ਲਈ (ਸਵੈਚਾਲਿਤ &compare_it : case_it->compare) {
						log_assert(switch_it->ਸੰਕੇਤ.ਮਾਪ() == compare_it.ਮਾਪ());
					}
					all_cases.ਧੱਕੋ_ਪਿੱਠ(case_it);
				}
			}
		}
		ਲਈ (ਸਵੈਚਾਲਿਤ &sync_it : it.ਦੂਜਾ->syncs) {
			ਚੋਣ (sync_it->type) {
				ਹਾਲਤ RTLIL::SyncType::ST0:
				ਹਾਲਤ RTLIL::SyncType::ST1:
				ਹਾਲਤ RTLIL::SyncType::STp:
				ਹਾਲਤ RTLIL::SyncType::STn:
				ਹਾਲਤ RTLIL::SyncType::STe:
					log_assert(!sync_it->ਸੰਕੇਤ.ਖਾਲੀ_ਥਾਵਾਂ());
					ਰੋਕੋ;
				ਹਾਲਤ RTLIL::SyncType::STa:
				ਹਾਲਤ RTLIL::SyncType::STg:
				ਹਾਲਤ RTLIL::SyncType::STi:
					log_assert(sync_it->ਸੰਕੇਤ.ਖਾਲੀ_ਥਾਵਾਂ());
					ਰੋਕੋ;
			}
		}
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ &it : module->connections_) {
		log_assert(it.ਪਹਿਲਾ.ਮਾਪ() == it.ਦੂਜਾ.ਮਾਪ());
		log_assert(!it.ਪਹਿਲਾ.has_const());
		it.ਪਹਿਲਾ.check(module);
		it.ਦੂਜਾ.check(module);
	}

	check_id_ਕੁੰਜੀਆਂ(module->ਗੁਣ);
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ RTLIL::Module::check()
{
	ਪੂਰਨ pool_ਮਾਪ = ThreadPool::work_pool_ਮਾਪ(0, cells_ਮਾਪ(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_ਮਾਪ);
	check_module(ਇਹ, thread_pool);
}

ਖਾਲੀ RTLIL::Module::optimize()
{
}

ਖਾਲੀ RTLIL::Module::cloneInto(RTLIL::Module *new_mod) ਅਚਲ
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	ਲਈ (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_ਮੁੱਲਾਂ.clear();
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : parameter_default_ਮੁੱਲਾਂ)
		new_mod->parameter_default_ਮੁੱਲਾਂ[dst_twines.copy_from(design->twines(), it.ਪਹਿਲਾ)] = it.ਦੂਜਾ;

	ਲਈ (ਸਵੈਚਾਲਿਤ &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->ਗੁਣ, ਗੁਣ, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	ਲਈ (ਸਵੈਚਾਲਿਤ it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->ਪਹਿਲਾ);
		wire_map[it->ਦੂਜਾ] = new_mod->ਜੋੜੋ_Wire(dst_id, it->ਦੂਜਾ);
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->ਪਹਿਲਾ);
		new_mod->ਜੋੜੋ_ਸਮ੍ਰਿਤੀ(dst_id, it->ਦੂਜਾ);
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->ਪਹਿਲਾ);
		new_mod->ਜੋੜੋ_Cell(dst_id, it->ਦੂਜਾ);
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->ਪਹਿਲਾ);
		new_mod->ਜੋੜੋ_Process(dst_id, it->ਦੂਜਾ);
	}

	ਬਣਤਰ Rewrite_Sig_Spec_ਕਾਮਾ
	{
		ਅਚਲ dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ਅਚਲ RTLIL::Module *new_mod;
		ਖਾਲੀ ਸੰਚਾਲਕ()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([ਇਹ](RTLIL::Wire *&wire) {
				ਸਵੈਚਾਲਿਤ it = wire_map.ਲੱਭੋ(wire);
				ਜੇ (it != wire_map.ਸਿਰਾ())
					wire = it->ਦੂਜਾ;
				ਨਹੀਂਤਾਂ
					log_assert(wire->module == new_mod);
			});
		}
	};

	Rewrite_Sig_Spec_ਕਾਮਾ rewrite_Sig_Spec_ਕਾਮਾ{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewrite_Sig_Spec_ਕਾਮਾ);
	new_mod->fixup_ports();
}

RTLIL::Module *RTLIL::Module::clone() ਅਚਲ
{
	RTLIL::Module *new_mod = ਨਵਾਂ RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	ਵਾਪਸ new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) ਅਚਲ
{
	RTLIL::Module *new_mod = ਨਵਾਂ RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->ਜੋੜੋ(new_mod);
	ਵਾਪਸ new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString ਨਿਸ਼ਾਨਾ_name) ਅਚਲ
{
	RTLIL::Module *new_mod = ਨਵਾਂ RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = ਨਿਸ਼ਾਨਾ_name;
	cloneInto(new_mod);
	dst->ਜੋੜੋ(new_mod);
	ਵਾਪਸ new_mod;
}

ਤਾਰਕਿਕ RTLIL::Module::has_memories() ਅਚਲ
{
	ਵਾਪਸ !memories.ਖਾਲੀ_ਥਾਵਾਂ();
}

ਤਾਰਕਿਕ RTLIL::Module::has_processes() ਅਚਲ
{
	ਵਾਪਸ !processes.ਖਾਲੀ_ਥਾਵਾਂ();
}

ਤਾਰਕਿਕ RTLIL::Module::has_memories_warn() ਅਚਲ
{
	ਜੇ (!memories.ਖਾਲੀ_ਥਾਵਾਂ())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", ਇਹ);
	ਵਾਪਸ !memories.ਖਾਲੀ_ਥਾਵਾਂ();
}

ਤਾਰਕਿਕ RTLIL::Module::has_processes_warn() ਅਚਲ
{
	ਜੇ (!processes.ਖਾਲੀ_ਥਾਵਾਂ())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", ਇਹ);
	ਵਾਪਸ !processes.ਖਾਲੀ_ਥਾਵਾਂ();
}

ਤਾਰਕਿਕ RTLIL::Module::is_selected() ਅਚਲ
{
	ਵਾਪਸ design->selected_module(ਇਹ->name);
}

ਤਾਰਕਿਕ RTLIL::Module::is_selected_ਸਾਰਾ() ਅਚਲ
{
	ਵਾਪਸ design->selected_ਸਾਰਾ_module(ਇਹ->name);
}

std::ਸਦਿਸ਼<RTLIL::Wire*> RTLIL::Module::selected_wires() ਅਚਲ
{
	std::ਸਦਿਸ਼<RTLIL::Wire*> ਨਤੀਜਾ;
	ਨਤੀਜਾ.reserve(wires_.ਮਾਪ());
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : wires_)
		ਜੇ (design->selected(ਇਹ, it.ਦੂਜਾ))
			ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(it.ਦੂਜਾ);
	ਵਾਪਸ ਨਤੀਜਾ;
}

std::ਸਦਿਸ਼<RTLIL::Cell*> RTLIL::Module::selected_cells() ਅਚਲ
{
	std::ਸਦਿਸ਼<RTLIL::Cell*> ਨਤੀਜਾ;
	ਨਤੀਜਾ.reserve(cells_.ਮਾਪ());
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : cells_)
		ਜੇ (design->selected(ਇਹ, it.ਦੂਜਾ))
			ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(it.ਦੂਜਾ);
	ਵਾਪਸ ਨਤੀਜਾ;
}

std::ਸਦਿਸ਼<RTLIL::ਸਮ੍ਰਿਤੀ*> RTLIL::Module::selected_memories() ਅਚਲ
{
	std::ਸਦਿਸ਼<RTLIL::ਸਮ੍ਰਿਤੀ*> ਨਤੀਜਾ;
	ਨਤੀਜਾ.reserve(memories.ਮਾਪ());
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : memories)
		ਜੇ (design->selected(ਇਹ, it.ਦੂਜਾ))
			ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(it.ਦੂਜਾ);
	ਵਾਪਸ ਨਤੀਜਾ;
}

std::ਸਦਿਸ਼<RTLIL::Process*> RTLIL::Module::selected_processes() ਅਚਲ
{
	std::ਸਦਿਸ਼<RTLIL::Process*> ਨਤੀਜਾ;
	ਨਤੀਜਾ.reserve(processes.ਮਾਪ());
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : processes)
		ਜੇ (design->selected(ਇਹ, it.ਦੂਜਾ))
			ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(it.ਦੂਜਾ);
	ਵਾਪਸ ਨਤੀਜਾ;
}

std::ਸਦਿਸ਼<RTLIL::SelectedMember> RTLIL::Module::selected_members() ਅਚਲ
{
	std::ਸਦਿਸ਼<RTLIL::SelectedMember> ਨਤੀਜਾ;
	ਲਈ (ਸਵੈਚਾਲਿਤ cell : selected_cells())
		ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ({cell, cell->name});
	ਲਈ (ਸਵੈਚਾਲਿਤ ਸਮ੍ਰਿਤੀ_3 : selected_memories())
		ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ({ਸਮ੍ਰਿਤੀ_3, ਸਮ੍ਰਿਤੀ_3->name});
	ਲਈ (ਸਵੈਚਾਲਿਤ wire : selected_wires())
		ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ({wire, wire->name});
	ਲਈ (ਸਵੈਚਾਲਿਤ process : selected_processes())
		ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ({process, process->name});
	ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ RTLIL::Module::ਜੋੜੋ(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(ਗਿਣਤੀ_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = ਇਹ;
}

ਖਾਲੀ RTLIL::Module::ਜੋੜੋ(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(ਗਿਣਤੀ_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = ਇਹ;
}

ਖਾਲੀ RTLIL::Module::ਜੋੜੋ(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(ਗਿਣਤੀ_id(process->name) == 0);
	processes[process->name] = process;
	process->module = ਇਹ;
}

ਖਾਲੀ RTLIL::Module::remove(ਅਚਲ pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	ਜੇ (wires.ਖਾਲੀ_ਥਾਵਾਂ())
		ਵਾਪਸ;

	ਬਣਤਰ Delete_Wire_ਕਾਮਾ
	{
		RTLIL::Module *module;
		ਅਚਲ pool<RTLIL::Wire*> *wires_p;

		ਖਾਲੀ ਸੰਚਾਲਕ()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([ਇਹ](RTLIL::Wire *&wire) {
				ਜੇ (wires_p->ਗਿਣਤੀ_2(wire))
					wire = module->ਜੋੜੋ_Wire(stringf("$delete_wire$%d", (ਪੂਰਨ)autoidx++), wire->ਚੌੜਾਈ);
			});
		}

		ਖਾਲੀ ਸੰਚਾਲਕ()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	Delete_Wire_ਕਾਮਾ delete_wire_ਕਾਮਾ;
	delete_wire_ਕਾਮਾ.module = ਇਹ;
	delete_wire_ਕਾਮਾ.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_ਕਾਮਾ);

	ਜੇ (design->flagBufferedNormalized) {
		ਲਈ (ਸਵੈਚਾਲਿਤ wire : wires) {
			buf_norm_wire_ਕਤਾਰ.erase(wire);
			buf_norm_connect_ਸੂਚਕਾਂਕ.erase(wire);
		}
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.ਗਿਣਤੀ_2(id) != 0);
		wires_.erase(id);
		ਮਿਟਾਓ it;
	}
}

ਖਾਲੀ RTLIL::Module::remove(RTLIL::Cell *cell)
{
	ਜਦੋਂ (!cell->connections_.ਖਾਲੀ_ਥਾਵਾਂ())
		cell->unset_ਪੋਰਟ(cell->connections_.begin()->ਪਹਿਲਾ);

	log_assert(cells_.ਗਿਣਤੀ_2(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	ਜੇ (design && design->flagBufferedNormalized && buf_norm_cell_ਕਤਾਰ.ਗਿਣਤੀ_2(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.ਪਾਓ(cell);
	} ਨਹੀਂਤਾਂ {
		ਮਿਟਾਓ cell;
	}
}

ਖਾਲੀ RTLIL::Module::remove(RTLIL::ਸਮ੍ਰਿਤੀ *ਸਮ੍ਰਿਤੀ_3)
{
	log_assert(memories.ਗਿਣਤੀ_2(ਸਮ੍ਰਿਤੀ_3->name) != 0);
	memories.erase(ਸਮ੍ਰਿਤੀ_3->name);
	ਮਿਟਾਓ ਸਮ੍ਰਿਤੀ_3;
}

ਖਾਲੀ RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.ਗਿਣਤੀ_2(process->name) != 0);
	processes.erase(process->name);
	ਮਿਟਾਓ process;
}

ਖਾਲੀ RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	ਜੋੜੋ(wire);
}

ਖਾਲੀ RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	ਜੋੜੋ(cell);
}

ਖਾਲੀ RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	ਜੇ (old_id != IdString::Null && wires_.ਗਿਣਤੀ_2(old_id))
		rename(wires_.at(old_id), new_name);
	ਨਹੀਂਤਾਂ ਜੇ (old_id != IdString::Null && cells_.ਗਿਣਤੀ_2(old_id))
		rename(cells_.at(old_id), new_name);
	ਨਹੀਂਤਾਂ
		log_abort();
}

ਖਾਲੀ RTLIL::Module::ਅਦਲਾ_ਬਦਲੀ_ਨਾਂ(RTLIL::Wire *w1, RTLIL::Wire *w2)
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
	ਅਦਲਾ_ਬਦਲੀ(w1->name, w2->name);
}

ਖਾਲੀ RTLIL::Module::ਅਦਲਾ_ਬਦਲੀ_ਨਾਂ(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	ਅਦਲਾ_ਬਦਲੀ(c1->name, c2->name);
}

RTLIL::IdString RTLIL::Module::uniquify(RTLIL::IdString name)
{
	ਪੂਰਨ ਸੂਚਕਾਂਕ = 0;
	ਵਾਪਸ uniquify(name, ਸੂਚਕਾਂਕ);
}

IdString RTLIL::Module::uniquify(IdString name, ਪੂਰਨ &ਸੂਚਕਾਂਕ)
{
	ਜੇ (ਸੂਚਕਾਂਕ == 0) {
		ਜੇ (ਗਿਣਤੀ_id(name) == 0)
			ਵਾਪਸ name;
		ਸੂਚਕਾਂਕ++;
	}

	ਜਦੋਂ (1) {
		IdString new_name = design->twines().ਜੋੜੋ(TwineSpec::Suffix{name, stringf("_%d", ਸੂਚਕਾਂਕ)});
		ਜੇ (ਗਿਣਤੀ_id(new_name) == 0)
			ਵਾਪਸ new_name;
		ਸੂਚਕਾਂਕ++;
	}
}

ਸਥਿਰ ਤਾਰਕਿਕ fixup_ports_compare(ਅਚਲ RTLIL::Wire *a, ਅਚਲ RTLIL::Wire *b)
{
	ਜੇ (a->ਪੋਰਟ_id && !b->ਪੋਰਟ_id)
		ਵਾਪਸ ਸੱਚ;
	ਜੇ (!a->ਪੋਰਟ_id && b->ਪੋਰਟ_id)
		ਵਾਪਸ ਝੂਠ;

	ਜੇ (a->ਪੋਰਟ_id == b->ਪੋਰਟ_id)
		ਵਾਪਸ a->name < b->name;
	ਵਾਪਸ a->ਪੋਰਟ_id < b->ਪੋਰਟ_id;
}

ਖਾਲੀ RTLIL::Module::connect(ਅਚਲ RTLIL::SigSig &conn)
{
	ਲਈ (ਸਵੈਚਾਲਿਤ mon : monitors)
		mon->notify_connect(ਇਹ, conn);

	ਜੇ (design)
		ਲਈ (ਸਵੈਚਾਲਿਤ mon : design->monitors)
			mon->notify_connect(ਇਹ, conn);

	// ignore all attempts to assign constants to other constants
	ਜੇ (conn.ਪਹਿਲਾ.has_const()) {
		RTLIL::SigSig new_conn;
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(conn.ਪਹਿਲਾ); i++)
			ਜੇ (conn.ਪਹਿਲਾ[i].wire) {
				new_conn.ਪਹਿਲਾ.append(conn.ਪਹਿਲਾ[i]);
				new_conn.ਦੂਜਾ.append(conn.ਦੂਜਾ[i]);
			}
		ਜੇ (Get_ਮਾਪ(new_conn.ਪਹਿਲਾ))
			connect(new_conn);
		ਵਾਪਸ;
	}

	ਜੇ (yosys_xtrace) {
		ਲਘੂਗਣਕ("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", ਇਹ, log_signal(conn.ਪਹਿਲਾ), log_signal(conn.ਦੂਜਾ), Get_ਮਾਪ(conn.ਪਹਿਲਾ));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(Get_ਮਾਪ(conn.ਪਹਿਲਾ) == Get_ਮਾਪ(conn.ਦੂਜਾ));
	connections_.ਧੱਕੋ_ਪਿੱਠ(conn);
}

ਖਾਲੀ RTLIL::Module::connect(ਅਚਲ RTLIL::SigSpec &lhs, ਅਚਲ RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

ਖਾਲੀ RTLIL::Module::new_connections(ਅਚਲ std::ਸਦਿਸ਼<RTLIL::SigSig> &new_conn)
{
	ਲਈ (ਸਵੈਚਾਲਿਤ mon : monitors)
		mon->notify_connect(ਇਹ, new_conn);

	ਜੇ (design)
		ਲਈ (ਸਵੈਚਾਲਿਤ mon : design->monitors)
			mon->notify_connect(ਇਹ, new_conn);

	ਜੇ (yosys_xtrace) {
		ਲਘੂਗਣਕ("#X# New connections vector in %s:\n", ਇਹ);
		ਲਈ (ਸਵੈਚਾਲਿਤ &conn: new_conn)
			ਲਘੂਗਣਕ("#X#    %s = %s (%d bits)\n", log_signal(conn.ਪਹਿਲਾ), log_signal(conn.ਦੂਜਾ), Get_ਮਾਪ(conn.ਪਹਿਲਾ));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ਅਚਲ std::ਸਦਿਸ਼<RTLIL::SigSig> &RTLIL::Module::connections() ਅਚਲ
{
	ਵਾਪਸ connections_;
}

ਖਾਲੀ RTLIL::Module::fixup_ports()
{
	std::ਸਦਿਸ਼<RTLIL::Wire*> all_ports;

	ਲਈ (ਸਵੈਚਾਲਿਤ &w : wires_)
		ਜੇ (w.ਦੂਜਾ->ਪੋਰਟ_ਨਿਵੇਸ਼ || w.ਦੂਜਾ->ਪੋਰਟ_output)
			all_ports.ਧੱਕੋ_ਪਿੱਠ(w.ਦੂਜਾ);
		ਨਹੀਂਤਾਂ
			w.ਦੂਜਾ->ਪੋਰਟ_id = 0;

	std::ਛਾਂਟੋ(all_ports.begin(), all_ports.ਸਿਰਾ(), fixup_ports_compare);

	ਜੇ (design && design->flagBufferedNormalized) {
		ਲਈ (ਸਵੈਚਾਲਿਤ &w : wires_)
			ਜੇ (w.ਦੂਜਾ->driverCell_ && w.ਦੂਜਾ->driverCell_->type == ID($ਨਿਵੇਸ਼_ਪੋਰਟ))
				buf_norm_wire_ਕਤਾਰ.ਪਾਓ(w.ਦੂਜਾ);

		buf_norm_wire_ਕਤਾਰ.ਪਾਓ(all_ports.begin(), all_ports.ਸਿਰਾ());
	}

	ports.clear();
	ਲਈ (ਮਾਪ_t i = 0; i < all_ports.ਮਾਪ(); i++) {
		ports.ਧੱਕੋ_ਪਿੱਠ(all_ports[i]->name);
		all_ports[i]->ਪੋਰਟ_id = i+1;
	}
}

ਖਾਲੀ RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		ਅਚਲ dict<IdString, RTLIL::Const> &src,
		ਅਚਲ RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	ਜੇ (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		ਵਾਪਸ;
	}
	dst.clear();
	ਲਈ (ਪੂਰਨ i = Get_ਮਾਪ(src) - 1; i >= 0; i--) {
		ਸਵੈਚਾਲਿਤ &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.ਪਹਿਲਾ)] = it.ਦੂਜਾ;
	}
}

RTLIL::Wire *RTLIL::Module::ਜੋੜੋ_Wire(RTLIL::IdString name, ਪੂਰਨ ਚੌੜਾਈ)
{
	log_assert(design);
	log_assert(ਚੌੜਾਈ >= 0 && ਚੌੜਾਈ < RTLIL::ਚੌੜਾਈ_ਹੱਦਬੰਦੀ);
	RTLIL::Wire *wire = ਨਵਾਂ RTLIL::Wire;
	wire->ਚੌੜਾਈ = ਚੌੜਾਈ;
	wire->name = name;
	ਜੋੜੋ(wire);
	ਵਾਪਸ wire;
}

RTLIL::Wire *RTLIL::Module::ਜੋੜੋ_Wire(IdString name, ਅਚਲ RTLIL::Wire *other)
{
	RTLIL::Wire *wire = ਜੋੜੋ_Wire(name);
	wire->ਚੌੜਾਈ = other->ਚੌੜਾਈ;
	wire->ਸ਼ੁਰੂ_ਵਿੱਥ = other->ਸ਼ੁਰੂ_ਵਿੱਥ;
	wire->ਪੋਰਟ_id = other->ਪੋਰਟ_id;
	wire->ਪੋਰਟ_ਨਿਵੇਸ਼ = other->ਪੋਰਟ_ਨਿਵੇਸ਼;
	wire->ਪੋਰਟ_output = other->ਪੋਰਟ_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ਅਚਲ RTLIL::Design *src_design = other->module ? other->module->design : ਖਾਲੀ_ਸੂਚਕ;
		RTLIL::copy_attr_dict(wire->ਗੁਣ, other->ਗੁਣ, src_design, ਇਹ->design);
	}
	ਵਾਪਸ wire;
}

RTLIL::Cell *RTLIL::Module::ਜੋੜੋ_Cell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = ਨਵਾਂ RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	ਜੋੜੋ(cell);
	ਵਾਪਸ cell;
}

RTLIL::Cell *RTLIL::Module::ਜੋੜੋ_Cell(IdString name, ਅਚਲ RTLIL::Cell *other)
{
	ਅਚਲ RTLIL::Design *src_design = other->module ? other->module->design : ਖਾਲੀ_ਸੂਚਕ;
	ਤਾਰਕਿਕ cross_pool = src_design && ਇਹ->design && src_design != ਇਹ->design;

	IdString type = other->type.ref();
	ਜੇ (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, ਇਹ->design);
	RTLIL::copy_attr_dict(cell->ਗੁਣ, other->ਗੁਣ, src_design, ਇਹ->design);

	ਜੇ (!cross_pool)
		cell->connections_ = other->connections_;
	ਨਹੀਂਤਾਂ
		ਲਈ (ਪੂਰਨ i = Get_ਮਾਪ(other->connections_) - 1; i >= 0; i--) {
			ਸਵੈਚਾਲਿਤ &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.ਪਹਿਲਾ)] = c.ਦੂਜਾ;
		}
	ਵਾਪਸ cell;
}

RTLIL::ਸਮ੍ਰਿਤੀ *RTLIL::Module::ਜੋੜੋ_ਸਮ੍ਰਿਤੀ(IdString name)
{
	log_assert(design);
	RTLIL::ਸਮ੍ਰਿਤੀ *mem = ਨਵਾਂ RTLIL::ਸਮ੍ਰਿਤੀ;
	mem->module = ਇਹ;
	mem->name = name;
	memories[name] = mem;
	ਵਾਪਸ mem;
}

RTLIL::ਸਮ੍ਰਿਤੀ *RTLIL::Module::ਜੋੜੋ_ਸਮ੍ਰਿਤੀ(IdString name, ਅਚਲ RTLIL::ਸਮ੍ਰਿਤੀ *other)
{
	log_assert(design);
	RTLIL::ਸਮ੍ਰਿਤੀ *mem = ਨਵਾਂ RTLIL::ਸਮ੍ਰਿਤੀ;
	mem->module = ਇਹ;
	mem->name = name;
	mem->ਚੌੜਾਈ = other->ਚੌੜਾਈ;
	mem->ਸ਼ੁਰੂ_ਵਿੱਥ = other->ਸ਼ੁਰੂ_ਵਿੱਥ;
	mem->ਮਾਪ = other->ਮਾਪ;
	RTLIL::copy_attr_dict(mem->ਗੁਣ, other->ਗੁਣ,
			other->module ? other->module->design : ਖਾਲੀ_ਸੂਚਕ, design);
	memories[name] = mem;
	ਵਾਪਸ mem;
}

RTLIL::Process *RTLIL::Module::ਜੋੜੋ_Process(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = ਨਵਾਂ RTLIL::Process;
	proc->name = name;
	ਜੋੜੋ(proc);
	ਵਾਪਸ proc;
}

RTLIL::Process *RTLIL::Module::ਜੋੜੋ_Process(RTLIL::IdString name, ਅਚਲ RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ਅਚਲ RTLIL::Design *src_design = other->module ? other->module->design : ਖਾਲੀ_ਸੂਚਕ;
	ਜੇ (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> ਗੁਣ;
			RTLIL::copy_attr_dict(ਗੁਣ, obj->ਗੁਣ, src_design, design);
			obj->ਗੁਣ = std::move(ਗੁਣ);
		});
		ਲਈ (ਸਵੈਚਾਲਿਤ *ਸਮਕਾਲੀ : proc->syncs)
			ਲਈ (ਸਵੈਚਾਲਿਤ &mwa : ਸਮਕਾਲੀ->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	ਜੋੜੋ(proc);
	ਵਾਪਸ proc;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEF_ਵਿਧੀ(_func, y_ਮਾਪ, _type) \
	RTLIL::Cell* RTLIL::Module::ਜੋੜੋ ## _func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, ਤਾਰਕਿਕ is_signed, ਅਚਲ std::string &src) { \
		RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_ਚੌੜਾਈ] = sig_a.ਮਾਪ();       \
		cell->parameters[ID::Y_ਚੌੜਾਈ] = sig_y.ਮਾਪ();       \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);                        \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);                        \
		cell->ਸੈੱਟ_src_attribute(src);                       \
		ਵਾਪਸ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਤਾਰਕਿਕ is_signed, ਅਚਲ std::string &src) { \
		RTLIL::SigSpec sig_y = ਜੋੜੋ_Wire(NEW_ID, y_ਮਾਪ);    \
		ਜੋੜੋ ## _func(name, sig_a, sig_y, is_signed, src);   \
		ਵਾਪਸ sig_y;                                       \
	}
DEF_ਵਿਧੀ(Not,        sig_a.ਮਾਪ(), ID($not))
DEF_ਵਿਧੀ(Pos,        sig_a.ਮਾਪ(), ID($pos))
DEF_ਵਿਧੀ(Neg,        sig_a.ਮਾਪ(), ID($neg))
DEF_ਵਿਧੀ(ReduceAnd,  1, ID($reduce_and))
DEF_ਵਿਧੀ(ReduceOr,   1, ID($reduce_or))
DEF_ਵਿਧੀ(ReduceXor,  1, ID($reduce_xor))
DEF_ਵਿਧੀ(ReduceXnor, 1, ID($reduce_xnor))
DEF_ਵਿਧੀ(ReduceBool, 1, ID($reduce_bool))
DEF_ਵਿਧੀ(LogicNot,   1, ID($logic_not))
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEF_ਵਿਧੀ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEF_ਵਿਧੀ(_func, y_ਮਾਪ, _type) \
	RTLIL::Cell* RTLIL::Module::ਜੋੜੋ ## _func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, ਤਾਰਕਿਕ /* is_signed */, ਅਚਲ std::string &src) { \
		RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, _type);           \
		cell->parameters[ID::ਚੌੜਾਈ_2] = sig_a.ਮਾਪ();         \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);                        \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);                        \
		cell->ਸੈੱਟ_src_attribute(src);                       \
		ਵਾਪਸ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਤਾਰਕਿਕ is_signed, ਅਚਲ std::string &src) { \
		RTLIL::SigSpec sig_y = ਜੋੜੋ_Wire(NEW_ID, y_ਮਾਪ);    \
		ਜੋੜੋ ## _func(name, sig_a, sig_y, is_signed, src);   \
		ਵਾਪਸ sig_y;                                       \
	}
DEF_ਵਿਧੀ(Buf,     sig_a.ਮਾਪ(), ID($buf))
DEF_ਵਿਧੀ(Barrier, sig_a.ਮਾਪ(), ID($barrier))
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEF_ਵਿਧੀ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEF_ਵਿਧੀ(_func, y_ਮਾਪ, _type) \
	RTLIL::Cell* RTLIL::Module::ਜੋੜੋ ## _func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, ਤਾਰਕਿਕ is_signed, ਅਚਲ std::string &src) { \
		RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_ਚੌੜਾਈ] = sig_a.ਮਾਪ();       \
		cell->parameters[ID::B_ਚੌੜਾਈ] = sig_b.ਮਾਪ();       \
		cell->parameters[ID::Y_ਚੌੜਾਈ] = sig_y.ਮਾਪ();       \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);                        \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::B, sig_b);                        \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);                        \
		cell->ਸੈੱਟ_src_attribute(src);                       \
		ਵਾਪਸ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਤਾਰਕਿਕ is_signed, ਅਚਲ std::string &src) { \
		RTLIL::SigSpec sig_y = ਜੋੜੋ_Wire(NEW_ID, y_ਮਾਪ);         \
		ਜੋੜੋ ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ਵਾਪਸ sig_y;                                            \
	}
DEF_ਵਿਧੀ(And,      max(sig_a.ਮਾਪ(), sig_b.ਮਾਪ()), ID($and))
DEF_ਵਿਧੀ(Or,       max(sig_a.ਮਾਪ(), sig_b.ਮਾਪ()), ID($or))
DEF_ਵਿਧੀ(Xor,      max(sig_a.ਮਾਪ(), sig_b.ਮਾਪ()), ID($xor))
DEF_ਵਿਧੀ(Xnor,     max(sig_a.ਮਾਪ(), sig_b.ਮਾਪ()), ID($xnor))
DEF_ਵਿਧੀ(ਖਿਸਕਾਓ_2,    sig_a.ਮਾਪ(), ID($ਖਿਸਕਾਓ))
DEF_ਵਿਧੀ(Lt,       1, ID($lt))
DEF_ਵਿਧੀ(Le,       1, ID($le))
DEF_ਵਿਧੀ(Eq,       1, ID($eq))
DEF_ਵਿਧੀ(Ne,       1, ID($ne))
DEF_ਵਿਧੀ(Eqx,      1, ID($eqx))
DEF_ਵਿਧੀ(Nex,      1, ID($nex))
DEF_ਵਿਧੀ(Ge,       1, ID($ge))
DEF_ਵਿਧੀ(Gt,       1, ID($gt))
DEF_ਵਿਧੀ(ਜੋੜੋ_2,      max(sig_a.ਮਾਪ(), sig_b.ਮਾਪ()), ID($ਜੋੜੋ))
DEF_ਵਿਧੀ(Sub,      max(sig_a.ਮਾਪ(), sig_b.ਮਾਪ()), ID($sub))
DEF_ਵਿਧੀ(Mul,      max(sig_a.ਮਾਪ(), sig_b.ਮਾਪ()), ID($mul))
DEF_ਵਿਧੀ(Div,      max(sig_a.ਮਾਪ(), sig_b.ਮਾਪ()), ID($div))
DEF_ਵਿਧੀ(Mod,      max(sig_a.ਮਾਪ(), sig_b.ਮਾਪ()), ID($mod))
DEF_ਵਿਧੀ(DivFloor, max(sig_a.ਮਾਪ(), sig_b.ਮਾਪ()), ID($divfloor))
DEF_ਵਿਧੀ(ModFloor, max(sig_a.ਮਾਪ(), sig_b.ਮਾਪ()), ID($modfloor))
DEF_ਵਿਧੀ(LogicAnd, 1, ID($logic_and))
DEF_ਵਿਧੀ(LogicOr,  1, ID($logic_or))
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEF_ਵਿਧੀ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEF_ਵਿਧੀ(_func, y_ਮਾਪ, _type) \
	RTLIL::Cell* RTLIL::Module::ਜੋੜੋ ## _func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, ਤਾਰਕਿਕ is_signed, ਅਚਲ std::string &src) { \
		RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = ਝੂਠ;             \
		cell->parameters[ID::A_ਚੌੜਾਈ] = sig_a.ਮਾਪ();       \
		cell->parameters[ID::B_ਚੌੜਾਈ] = sig_b.ਮਾਪ();       \
		cell->parameters[ID::Y_ਚੌੜਾਈ] = sig_y.ਮਾਪ();       \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);                        \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::B, sig_b);                        \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);                        \
		cell->ਸੈੱਟ_src_attribute(src);                       \
		ਵਾਪਸ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਤਾਰਕਿਕ is_signed, ਅਚਲ std::string &src) { \
		RTLIL::SigSpec sig_y = ਜੋੜੋ_Wire(NEW_ID, y_ਮਾਪ);         \
		ਜੋੜੋ ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ਵਾਪਸ sig_y;                                            \
	}
DEF_ਵਿਧੀ(Shl,      sig_a.ਮਾਪ(), ID($shl))
DEF_ਵਿਧੀ(Shr,      sig_a.ਮਾਪ(), ID($shr))
DEF_ਵਿਧੀ(Sshl,     sig_a.ਮਾਪ(), ID($sshl))
DEF_ਵਿਧੀ(Sshr,     sig_a.ਮਾਪ(), ID($sshr))
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEF_ਵਿਧੀ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEF_ਵਿਧੀ(_func, y_ਮਾਪ, _type) \
	RTLIL::Cell* RTLIL::Module::ਜੋੜੋ ## _func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, ਤਾਰਕਿਕ is_signed, ਅਚਲ std::string &src) { \
		RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = ਝੂਠ;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_ਚੌੜਾਈ] = sig_a.ਮਾਪ();       \
		cell->parameters[ID::B_ਚੌੜਾਈ] = sig_b.ਮਾਪ();       \
		cell->parameters[ID::Y_ਚੌੜਾਈ] = sig_y.ਮਾਪ();       \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);                        \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::B, sig_b);                        \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);                        \
		cell->ਸੈੱਟ_src_attribute(src);                       \
		ਵਾਪਸ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਤਾਰਕਿਕ is_signed, ਅਚਲ std::string &src) { \
		RTLIL::SigSpec sig_y = ਜੋੜੋ_Wire(NEW_ID, y_ਮਾਪ);         \
		ਜੋੜੋ ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ਵਾਪਸ sig_y;                                            \
	}
DEF_ਵਿਧੀ(Shiftx,      sig_a.ਮਾਪ(), ID($shiftx))
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEF_ਵਿਧੀ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEF_ਵਿਧੀ(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::ਜੋੜੋ ## _func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src) { \
		RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, _type);                 \
		cell->parameters[ID::ਚੌੜਾਈ_2] = sig_a.ਮਾਪ();               \
		ਜੇ (_pmux) cell->parameters[ID::S_ਚੌੜਾਈ] = sig_s.ਮਾਪ();  \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);                              \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::B, sig_b);                              \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::S, sig_s);                              \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);                              \
		cell->ਸੈੱਟ_src_attribute(src);                             \
		ਵਾਪਸ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ std::string &src) { \
		RTLIL::SigSpec sig_y = ਜੋੜੋ_Wire(NEW_ID, sig_a.ਮਾਪ());     \
		ਜੋੜੋ ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		ਵਾਪਸ sig_y;                                             \
	}
DEF_ਵਿਧੀ(Mux,      ID($mux),        0)
DEF_ਵਿਧੀ(Bwmux,    ID($bwmux),      0)
DEF_ਵਿਧੀ(Pmux,     ID($pmux),       1)
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEF_ਵਿਧੀ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEF_ਵਿਧੀ(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::ਜੋੜੋ ## _func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src) { \
		RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, _type);                 \
		cell->parameters[ID::ਚੌੜਾਈ_2] = _demux ? sig_a.ਮਾਪ() : sig_y.ਮਾਪ(); \
		cell->parameters[ID::S_ਚੌੜਾਈ] = sig_s.ਮਾਪ();             \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);                              \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::S, sig_s);                              \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);                              \
		cell->ਸੈੱਟ_src_attribute(src);                             \
		ਵਾਪਸ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ std::string &src) { \
		RTLIL::SigSpec sig_y = ਜੋੜੋ_Wire(NEW_ID, _demux ? sig_a.ਮਾਪ() << sig_s.ਮਾਪ() : sig_a.ਮਾਪ() >> sig_s.ਮਾਪ()); \
		ਜੋੜੋ ## _func(name, sig_a, sig_s, sig_y, src);             \
		ਵਾਪਸ sig_y;                                             \
	}
DEF_ਵਿਧੀ(Bmux,     ID($bmux),       0)
DEF_ਵਿਧੀ(Demux,    ID($demux),      1)
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEF_ਵਿਧੀ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEF_ਵਿਧੀ(_func, _type) \
	RTLIL::Cell* RTLIL::Module::ਜੋੜੋ ## _func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src) { \
		RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, _type);                 \
		cell->parameters[ID::ਚੌੜਾਈ_2] = sig_a.ਮਾਪ();               \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);                              \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::B, sig_b);                              \
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);                              \
		cell->ਸੈੱਟ_src_attribute(src);                             \
		ਵਾਪਸ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ std::string &src) { \
		RTLIL::SigSpec sig_y = ਜੋੜੋ_Wire(NEW_ID, sig_a.ਮਾਪ());     \
		ਜੋੜੋ ## _func(name, sig_a, sig_s, sig_y, src);             \
		ਵਾਪਸ sig_y;                                             \
	}
DEF_ਵਿਧੀ(Bweqx,    ID($bweqx))
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEF_ਵਿਧੀ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEF_ਵਿਧੀ_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::ਜੋੜੋ ## _func(RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig1, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig2, ਅਚਲ std::string &src) { \
		RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, _type);         \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P1, sig1);                   \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P2, sig2);                   \
		cell->ਸੈੱਟ_src_attribute(src);                     \
		ਵਾਪਸ cell;                                      \
	} \
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ RTLIL::Module::_func(RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig1, ਅਚਲ std::string &src) { \
		RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ sig2 = ਜੋੜੋ_Wire(NEW_ID);             \
		ਜੋੜੋ ## _func(name, sig1, sig2, src);              \
		ਵਾਪਸ sig2;                                      \
	}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEF_ਵਿਧੀ_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::ਜੋੜੋ ## _func(RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig1, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig2, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig3, ਅਚਲ std::string &src) { \
		RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, _type);         \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P1, sig1);                   \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P2, sig2);                   \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P3, sig3);                   \
		cell->ਸੈੱਟ_src_attribute(src);                     \
		ਵਾਪਸ cell;                                      \
	} \
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ RTLIL::Module::_func(RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig1, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig2, ਅਚਲ std::string &src) { \
		RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ sig3 = ਜੋੜੋ_Wire(NEW_ID);             \
		ਜੋੜੋ ## _func(name, sig1, sig2, sig3, src);        \
		ਵਾਪਸ sig3;                                      \
	}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEF_ਵਿਧੀ_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::ਜੋੜੋ ## _func(RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig1, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig2, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig3, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig4, ਅਚਲ std::string &src) { \
		RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, _type);         \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P1, sig1);                   \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P2, sig2);                   \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P3, sig3);                   \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P4, sig4);                   \
		cell->ਸੈੱਟ_src_attribute(src);                     \
		ਵਾਪਸ cell;                                      \
	} \
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ RTLIL::Module::_func(RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig1, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig2, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig3, ਅਚਲ std::string &src) { \
		RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ sig4 = ਜੋੜੋ_Wire(NEW_ID);             \
		ਜੋੜੋ ## _func(name, sig1, sig2, sig3, sig4, src);  \
		ਵਾਪਸ sig4;                                      \
	}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEF_ਵਿਧੀ_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::ਜੋੜੋ ## _func(RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig1, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig2, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig3, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig4, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig5, ਅਚਲ std::string &src) { \
		RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, _type);         \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P1, sig1);                   \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P2, sig2);                   \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P3, sig3);                   \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P4, sig4);                   \
		cell->ਸੈੱਟ_ਪੋਰਟ("\\" #_P5, sig5);                   \
		cell->ਸੈੱਟ_src_attribute(src);                     \
		ਵਾਪਸ cell;                                      \
	} \
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ RTLIL::Module::_func(RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig1, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig2, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig3, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig4, ਅਚਲ std::string &src) { \
		RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ sig5 = ਜੋੜੋ_Wire(NEW_ID);                  \
		ਜੋੜੋ ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		ਵਾਪਸ sig5;                                           \
	}
DEF_ਵਿਧੀ_2(BufGate,    ID($_BUF_),    A, Y)
DEF_ਵਿਧੀ_2(NotGate,    ID($_NOT_),    A, Y)
DEF_ਵਿਧੀ_3(AndGate,    ID($_AND_),    A, B, Y)
DEF_ਵਿਧੀ_3(NandGate,   ID($_NAND_),   A, B, Y)
DEF_ਵਿਧੀ_3(OrGate,     ID($_OR_),     A, B, Y)
DEF_ਵਿਧੀ_3(NorGate,    ID($_NOR_),    A, B, Y)
DEF_ਵਿਧੀ_3(XorGate,    ID($_XOR_),    A, B, Y)
DEF_ਵਿਧੀ_3(XnorGate,   ID($_XNOR_),   A, B, Y)
DEF_ਵਿਧੀ_3(AndnotGate, ID($_ANDNOT_), A, B, Y)
DEF_ਵਿਧੀ_3(OrnotGate,  ID($_ORNOT_),  A, B, Y)
DEF_ਵਿਧੀ_4(MuxGate,    ID($_MUX_),    A, B, S, Y)
DEF_ਵਿਧੀ_4(NmuxGate,   ID($_NMUX_),   A, B, S, Y)
DEF_ਵਿਧੀ_4(Aoi3Gate,   ID($_AOI3_),   A, B, C, Y)
DEF_ਵਿਧੀ_4(Oai3Gate,   ID($_OAI3_),   A, B, C, Y)
DEF_ਵਿਧੀ_5(Aoi4Gate,   ID($_AOI4_),   A, B, C, D, Y)
DEF_ਵਿਧੀ_5(Oai4Gate,   ID($_OAI4_),   A, B, C, D, Y)
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEF_ਵਿਧੀ_2
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEF_ਵਿਧੀ_3
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEF_ਵਿਧੀ_4
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ DEF_ਵਿਧੀ_5

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Pow(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, ਤਾਰਕਿਕ a_signed, ਤਾਰਕਿਕ b_signed, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($ਘਾਤ));
	cell->parameters[ID::A_SIGNED] = a_signed;
	cell->parameters[ID::B_SIGNED] = b_signed;
	cell->parameters[ID::A_ਚੌੜਾਈ] = sig_a.ਮਾਪ();
	cell->parameters[ID::B_ਚੌੜਾਈ] = sig_b.ਮਾਪ();
	cell->parameters[ID::Y_ਚੌੜਾਈ] = sig_y.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::B, sig_b);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Fa(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_c, ਅਚਲ RTLIL::SigSpec &sig_x, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($fa));
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_a.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::B, sig_b);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::C, sig_c);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::X, sig_x);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_ਟੁਕੜਾ(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, RTLIL::Const ਵਿੱਥ, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($ਟੁਕੜਾ));
	cell->parameters[ID::A_ਚੌੜਾਈ] = sig_a.ਮਾਪ();
	cell->parameters[ID::Y_ਚੌੜਾਈ] = sig_y.ਮਾਪ();
	cell->parameters[ID::ਵਿੱਥ_2] = ਵਿੱਥ;
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Concat(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($concat));
	cell->parameters[ID::A_ਚੌੜਾਈ] = sig_a.ਮਾਪ();
	cell->parameters[ID::B_ਚੌੜਾਈ] = sig_b.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::B, sig_b);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Lut(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, RTLIL::Const lut, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_a.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Tribuf(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($tribuf));
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_a.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Assert(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($ਪ੍ਰਤਿਜ੍ਞਾ));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Assume(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($assume));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_ਜੀਵਤ(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($ਜੀਵਤ));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Fair(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($fair));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Cover(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($cover));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Equiv(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($equiv));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::B, sig_b);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Sr(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr, ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ ਸੈੱਟ_polarity, ਤਾਰਕਿਕ clr_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($sr));
	cell->parameters[ID::ਸੈੱਟ_POLARITY] = ਸੈੱਟ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::ਸੈੱਟ_2, sig_ਸੈੱਟ);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLR, sig_clr);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Ff(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($ff));
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Dff(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ clk_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLK, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Dffe(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ en_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($dffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLK, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Dffsr(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ ਸੈੱਟ_polarity, ਤਾਰਕਿਕ clr_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($dffsr));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ਸੈੱਟ_POLARITY] = ਸੈੱਟ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLK, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::ਸੈੱਟ_2, sig_ਸੈੱਟ);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLR, sig_clr);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Dffsre(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ ਸੈੱਟ_polarity, ਤਾਰਕਿਕ clr_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($dffsre));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ਸੈੱਟ_POLARITY] = ਸੈੱਟ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLK, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::ਸੈੱਟ_2, sig_ਸੈੱਟ);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLR, sig_clr);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Adff(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_arst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ਮੁੱਲ, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ arst_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ਮੁੱਲ] = arst_ਮੁੱਲ;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLK, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::ARST, sig_arst);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Adffe(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_arst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ਮੁੱਲ, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ arst_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ਮੁੱਲ] = arst_ਮੁੱਲ;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLK, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::ARST, sig_arst);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Aldff(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_aload, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		ਅਚਲ RTLIL::SigSpec &sig_ad, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ aload_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($aldff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLK, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::ALOAD, sig_aload);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::AD, sig_ad);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Aldffe(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_aload, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		ਅਚਲ RTLIL::SigSpec &sig_ad, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ aload_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($aldffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLK, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::ALOAD, sig_aload);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::AD, sig_ad);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Sdff(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_srst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ਮੁੱਲ, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ srst_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ਮੁੱਲ] = srst_ਮੁੱਲ;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLK, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::SRST, sig_srst);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Sdffe(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_srst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ਮੁੱਲ, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ srst_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ਮੁੱਲ] = srst_ਮੁੱਲ;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLK, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::SRST, sig_srst);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Sdffce(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_srst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ਮੁੱਲ, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ srst_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ਮੁੱਲ] = srst_ਮੁੱਲ;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLK, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::SRST, sig_srst);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Dlatch(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ en_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Adlatch(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_arst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ਮੁੱਲ, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ arst_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ਮੁੱਲ] = arst_ਮੁੱਲ;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::ARST, sig_arst);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Dlatchsr(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ ਸੈੱਟ_polarity, ਤਾਰਕਿਕ clr_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($dlatchsr));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ਸੈੱਟ_POLARITY] = ਸੈੱਟ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::EN, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::ਸੈੱਟ_2, sig_ਸੈੱਟ);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLR, sig_clr);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Sr_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr,
		ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ ਸੈੱਟ_polarity, ਤਾਰਕਿਕ clr_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_SR_%c%c_", ਸੈੱਟ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::S, sig_ਸੈੱਟ);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::R, sig_clr);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Ff_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($_FF_));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Dff_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ clk_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::C, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Dffe_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ en_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::C, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::E, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Dffsr_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ ਸੈੱਟ_polarity, ਤਾਰਕਿਕ clr_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', ਸੈੱਟ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::C, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::S, sig_ਸੈੱਟ);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::R, sig_clr);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Dffsre_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ ਸੈੱਟ_polarity, ਤਾਰਕਿਕ clr_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', ਸੈੱਟ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::C, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::S, sig_ਸੈੱਟ);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::R, sig_clr);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::E, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Adff_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_arst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		ਤਾਰਕਿਕ arst_ਮੁੱਲ, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ arst_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ਮੁੱਲ ? '1' : '0'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::C, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::R, sig_arst);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Adffe_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_arst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		ਤਾਰਕਿਕ arst_ਮੁੱਲ, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ arst_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ਮੁੱਲ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::C, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::R, sig_arst);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::E, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Aldff_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_aload, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		ਅਚਲ RTLIL::SigSpec &sig_ad, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ aload_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::C, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::L, sig_aload);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::AD, sig_ad);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Aldffe_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_aload, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		ਅਚਲ RTLIL::SigSpec &sig_ad, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ aload_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::C, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::L, sig_aload);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::E, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::AD, sig_ad);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Sdff_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_srst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		ਤਾਰਕਿਕ srst_ਮੁੱਲ, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ srst_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ਮੁੱਲ ? '1' : '0'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::C, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::R, sig_srst);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Sdffe_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_srst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		ਤਾਰਕਿਕ srst_ਮੁੱਲ, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ srst_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ਮੁੱਲ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::C, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::R, sig_srst);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::E, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Sdffce_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_srst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		ਤਾਰਕਿਕ srst_ਮੁੱਲ, ਤਾਰਕਿਕ clk_polarity, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ srst_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ਮੁੱਲ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::C, sig_clk);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::R, sig_srst);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::E, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Dlatch_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ en_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::E, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Adlatch_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_arst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
		ਤਾਰਕਿਕ arst_ਮੁੱਲ, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ arst_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ਮੁੱਲ ? '1' : '0'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::E, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::R, sig_arst);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Dlatchsr_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਤਾਰਕਿਕ en_polarity, ਤਾਰਕਿਕ ਸੈੱਟ_polarity, ਤਾਰਕਿਕ clr_polarity, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', ਸੈੱਟ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::E, sig_en);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::S, sig_ਸੈੱਟ);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::R, sig_clr);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Anyinit(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($anyinit));
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_q.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::D, sig_d);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, sig_q);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, ਪੂਰਨ ਚੌੜਾਈ, ਅਚਲ std::string &src)
{
	RTLIL::SigSpec sig = ਜੋੜੋ_Wire(NEW_ID, ਚੌੜਾਈ);
	Cell *cell = ਜੋੜੋ_Cell(name, ID($anyconst));
	cell->ਸੈੱਟ_Param(ID::ਚੌੜਾਈ_2, ਚੌੜਾਈ);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, ਪੂਰਨ ਚੌੜਾਈ, ਅਚਲ std::string &src)
{
	RTLIL::SigSpec sig = ਜੋੜੋ_Wire(NEW_ID, ਚੌੜਾਈ);
	Cell *cell = ਜੋੜੋ_Cell(name, ID($anyseq));
	cell->ਸੈੱਟ_Param(ID::ਚੌੜਾਈ_2, ਚੌੜਾਈ);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, ਪੂਰਨ ਚੌੜਾਈ, ਅਚਲ std::string &src)
{
	RTLIL::SigSpec sig = ਜੋੜੋ_Wire(NEW_ID, ਚੌੜਾਈ);
	Cell *cell = ਜੋੜੋ_Cell(name, ID($allconst));
	cell->ਸੈੱਟ_Param(ID::ਚੌੜਾਈ_2, ਚੌੜਾਈ);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, ਪੂਰਨ ਚੌੜਾਈ, ਅਚਲ std::string &src)
{
	RTLIL::SigSpec sig = ਜੋੜੋ_Wire(NEW_ID, ਚੌੜਾਈ);
	Cell *cell = ਜੋੜੋ_Cell(name, ID($allseq));
	cell->ਸੈੱਟ_Param(ID::ਚੌੜਾਈ_2, ਚੌੜਾਈ);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ਅਚਲ std::string &src)
{
	RTLIL::SigSpec sig = ਜੋੜੋ_Wire(NEW_ID);
	Cell *cell = ਜੋੜੋ_Cell(name, ID($initstate));
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ sig;
}

RTLIL::SigSpec RTLIL::Module::ਸੈੱਟ_ਟੈਗ_2(RTLIL::IdString name, ਅਚਲ std::string &ਟੈਗ, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ RTLIL::SigSpec &sig_c, ਅਚਲ std::string &src)
{
	RTLIL::SigSpec sig = ਜੋੜੋ_Wire(NEW_ID, sig_a.ਮਾਪ());
	Cell *cell = ਜੋੜੋ_Cell(name, ID($ਸੈੱਟ_ਟੈਗ));
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_a.ਮਾਪ();
	cell->parameters[ID::ਟੈਗ_2] = ਟੈਗ;
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::ਸੈੱਟ_2, sig_s);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLR, sig_c);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ sig;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_ਸੈੱਟ_ਟੈਗ(RTLIL::IdString name, ਅਚਲ std::string &ਟੈਗ, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ RTLIL::SigSpec &sig_c, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src)
{
	Cell *cell = ਜੋੜੋ_Cell(name, ID($ਸੈੱਟ_ਟੈਗ));
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_a.ਮਾਪ();
	cell->parameters[ID::ਟੈਗ_2] = ਟੈਗ;
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::ਸੈੱਟ_2, sig_s);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLR, sig_c);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig_y);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::SigSpec RTLIL::Module::Get_ਟੈਗ(RTLIL::IdString name, ਅਚਲ std::string &ਟੈਗ, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ std::string &src)
{
	RTLIL::SigSpec sig = ਜੋੜੋ_Wire(NEW_ID, sig_a.ਮਾਪ());
	Cell *cell = ਜੋੜੋ_Cell(name, ID($get_ਟੈਗ));
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_a.ਮਾਪ();
	cell->parameters[ID::ਟੈਗ_2] = ਟੈਗ;
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ sig;
}

RTLIL::Cell* RTLIL::Module::ਜੋੜੋ_Overwrite_ਟੈਗ(RTLIL::IdString name, ਅਚਲ std::string &ਟੈਗ, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ RTLIL::SigSpec &sig_c, ਅਚਲ std::string &src)
{
	RTLIL::Cell *cell = ਜੋੜੋ_Cell(name, ID($overwrite_ਟੈਗ));
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_a.ਮਾਪ();
	cell->parameters[ID::ਟੈਗ_2] = ਟੈਗ;
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::ਸੈੱਟ_2, sig_s);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::CLR, sig_c);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ cell;
}

RTLIL::SigSpec RTLIL::Module::Original_ਟੈਗ(RTLIL::IdString name, ਅਚਲ std::string &ਟੈਗ, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ std::string &src)
{
	RTLIL::SigSpec sig = ਜੋੜੋ_Wire(NEW_ID, sig_a.ਮਾਪ());
	Cell *cell = ਜੋੜੋ_Cell(name, ID($original_ਟੈਗ));
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_a.ਮਾਪ();
	cell->parameters[ID::ਟੈਗ_2] = ਟੈਗ;
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_a);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_e, ਅਚਲ std::string &src)
{
	RTLIL::SigSpec sig = ਜੋੜੋ_Wire(NEW_ID, sig_e.ਮਾਪ());
	Cell *cell = ਜੋੜੋ_Cell(name, ID($future_ff));
	cell->parameters[ID::ਚੌੜਾਈ_2] = sig_e.ਮਾਪ();
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, sig_e);
	cell->ਸੈੱਟ_ਪੋਰਟ(ID::Y, sig);
	cell->ਸੈੱਟ_src_attribute(src);
	ਵਾਪਸ sig;
}

std::string RTLIL::Module::ਮੰਜ਼ਿਲ_rtlil_str() ਅਚਲ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(ਇਹ), design, ਝੂਠ, ਸੱਚ, ਝੂਠ, RTLIL_BACKEND::Dump_ਬਹੁਲਕ::Readable);
	ਵਾਪਸ f.str();
}

RTLIL::Wire::Wire()
{
	ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hashidx_ਗਿਣਤੀ = 123456789;
	hashidx_ਗਿਣਤੀ = mkhash_xorshift(hashidx_ਗਿਣਤੀ);
	hashidx_ = hashidx_ਗਿਣਤੀ;

	module = ਖਾਲੀ_ਸੂਚਕ;
	ਚੌੜਾਈ = 1;
	ਸ਼ੁਰੂ_ਵਿੱਥ = 0;
	ਪੋਰਟ_id = 0;
	ਪੋਰਟ_ਨਿਵੇਸ਼ = ਝੂਠ;
	ਪੋਰਟ_output = ਝੂਠ;
	upto = ਝੂਠ;
	is_signed = ਝੂਠ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->ਪਾਓ(std::pair<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Wire*>(hashidx_, ਇਹ));
#ਜੇ_ਅੰਤ
}

RTLIL::Wire::~Wire()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#ਜੇ_ਅੰਤ
}

std::string RTLIL::Wire::ਮੰਜ਼ਿਲ_rtlil_str() ਅਚਲ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", ਇਹ, design(), RTLIL_BACKEND::Dump_ਬਹੁਲਕ::Readable);
	ਵਾਪਸ f.str();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
ਸਥਿਰ std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Wire*> all_wires;
std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(ਖਾਲੀ)
{
	ਵਾਪਸ &all_wires;
}
#ਜੇ_ਅੰਤ

RTLIL::ਸਮ੍ਰਿਤੀ::ਸਮ੍ਰਿਤੀ()
{
	ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hashidx_ਗਿਣਤੀ = 123456789;
	hashidx_ਗਿਣਤੀ = mkhash_xorshift(hashidx_ਗਿਣਤੀ);
	hashidx_ = hashidx_ਗਿਣਤੀ;

	ਚੌੜਾਈ = 1;
	ਸ਼ੁਰੂ_ਵਿੱਥ = 0;
	ਮਾਪ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	RTLIL::ਸਮ੍ਰਿਤੀ::get_all_memorys()->ਪਾਓ(std::pair<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::ਸਮ੍ਰਿਤੀ*>(hashidx_, ਇਹ));
#ਜੇ_ਅੰਤ
}

std::string RTLIL::ਸਮ੍ਰਿਤੀ::ਮੰਜ਼ਿਲ_rtlil_str() ਅਚਲ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_ਸਮ੍ਰਿਤੀ(f, "", ਇਹ, design(), RTLIL_BACKEND::Dump_ਬਹੁਲਕ::Readable);
	ਵਾਪਸ f.str();
}

RTLIL::Process::Process() : module(ਖਾਲੀ_ਸੂਚਕ)
{
	ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hashidx_ਗਿਣਤੀ = 123456789;
	hashidx_ਗਿਣਤੀ = mkhash_xorshift(hashidx_ਗਿਣਤੀ);
	hashidx_ = hashidx_ਗਿਣਤੀ;
}

std::string RTLIL::Process::ਮੰਜ਼ਿਲ_rtlil_str() ਅਚਲ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", ਇਹ, design(), RTLIL_BACKEND::Dump_ਬਹੁਲਕ::Readable);
	ਵਾਪਸ f.str();
}

RTLIL::Cell::Cell() : module(ਖਾਲੀ_ਸੂਚਕ)
{
	ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hashidx_ਗਿਣਤੀ = 123456789;
	hashidx_ਗਿਣਤੀ = mkhash_xorshift(hashidx_ਗਿਣਤੀ);
	hashidx_ = hashidx_ਗਿਣਤੀ;

	// log("#memtrace# %p\n", this);
	memhasher();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->ਪਾਓ(std::pair<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Cell*>(hashidx_, ਇਹ));
#ਜੇ_ਅੰਤ
}

RTLIL::Cell::~Cell()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#ਜੇ_ਅੰਤ
}

std::string RTLIL::Cell::ਮੰਜ਼ਿਲ_rtlil_str() ਅਚਲ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", ਇਹ, design(), RTLIL_BACKEND::Dump_ਬਹੁਲਕ::Readable);
	ਵਾਪਸ f.str();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
ਸਥਿਰ std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Cell*> all_cells;
std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(ਖਾਲੀ)
{
	ਵਾਪਸ &all_cells;
}
#ਜੇ_ਅੰਤ

ਤਾਰਕਿਕ RTLIL::Cell::has_ਪੋਰਟ(RTLIL::IdString portname) ਅਚਲ
{
	ਵਾਪਸ connections_.ਗਿਣਤੀ_2(portname) != 0;
}

// bufnorm

ਅਚਲ RTLIL::SigSpec &RTLIL::Cell::get_ਪੋਰਟ(RTLIL::IdString portname) ਅਚਲ
{
	ਵਾਪਸ connections_.at(portname);
}

ਅਚਲ dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() ਅਚਲ
{
	ਵਾਪਸ connections_;
}

ਤਾਰਕਿਕ RTLIL::Cell::ਗਿਆਤ() ਅਚਲ
{
	ਜੇ (yosys_celltypes.cell_ਗਿਆਤ(type))
		ਵਾਪਸ ਸੱਚ;
	ਜੇ (module && module->design && module->design->module(type))
		ਵਾਪਸ ਸੱਚ;
	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ RTLIL::Cell::ਨਿਵੇਸ਼(IdString portname) ਅਚਲ
{
	ਜੇ (yosys_celltypes.cell_ਗਿਆਤ(type))
		ਵਾਪਸ yosys_celltypes.cell_ਨਿਵੇਸ਼(type.ref(), portname);
	ਜੇ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : ਖਾਲੀ_ਸੂਚਕ;
		ਵਾਪਸ w && w->ਪੋਰਟ_ਨਿਵੇਸ਼;
	}
	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ RTLIL::Cell::output(RTLIL::IdString portname) ਅਚਲ
{
	ਜੇ (yosys_celltypes.cell_ਗਿਆਤ(type))
		ਵਾਪਸ yosys_celltypes.cell_output(type, portname);
	ਜੇ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : ਖਾਲੀ_ਸੂਚਕ;
		ਵਾਪਸ w && w->ਪੋਰਟ_output;
	}
	ਵਾਪਸ ਝੂਠ;
}

RTLIL::ਪੋਰਟ_Dir RTLIL::Cell::ਪੋਰਟ_dir(RTLIL::IdString portname) ਅਚਲ
{
	ਜੇ (yosys_celltypes.cell_ਗਿਆਤ(type.ref()))
		ਵਾਪਸ yosys_celltypes.cell_ਪੋਰਟ_dir(type, portname);
	ਜੇ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		ਜੇ (m == ਖਾਲੀ_ਸੂਚਕ)
			ਵਾਪਸ ਪੋਰਟ_Dir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		ਜੇ (w == ਖਾਲੀ_ਸੂਚਕ)
			ਵਾਪਸ ਪੋਰਟ_Dir::PD_UNKNOWN;
		ਵਾਪਸ ਪੋਰਟ_Dir(w->ਪੋਰਟ_ਨਿਵੇਸ਼ + w->ਪੋਰਟ_output * 2);
	}
	ਵਾਪਸ ਪੋਰਟ_Dir::PD_UNKNOWN;
}

ਤਾਰਕਿਕ RTLIL::Cell::hasParam(RTLIL::IdString paramname) ਅਚਲ
{
	ਵਾਪਸ parameters.ਗਿਣਤੀ_2(paramname) != 0;
}

ਖਾਲੀ RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

ਖਾਲੀ RTLIL::Cell::ਸੈੱਟ_Param(RTLIL::IdString paramname, RTLIL::Const ਮੁੱਲ)
{
	parameters[paramname] = std::move(ਮੁੱਲ);
}

ਅਚਲ RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ਅਚਲ
{
	ਅਚਲ ਸਵੈਚਾਲਿਤ &it = parameters.ਲੱਭੋ(paramname);
	ਜੇ (it != parameters.ਸਿਰਾ())
		ਵਾਪਸ it->ਦੂਜਾ;
	ਜੇ (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		ਜੇ (m)
			ਵਾਪਸ m->parameter_default_ਮੁੱਲਾਂ.at(paramname);
	}
	ਸੁੱਟੋ std::ਬਾਹਰਲਾ_ਦਾ_range("Cell::getParam()");
}

ਖਾਲੀ RTLIL::Cell::ਛਾਂਟੋ()
{
	connections_.ਛਾਂਟੋ(ਛਾਂਟੋ_by_id_str(module->twines()));
	parameters.ਛਾਂਟੋ(ਛਾਂਟੋ_by_id_str(module->twines()));
	ਗੁਣ.ਛਾਂਟੋ(ਛਾਂਟੋ_by_id_str(module->twines()));
}

ਖਾਲੀ RTLIL::Cell::check()
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NDEBUG
	ਅੰਦਰੂਨੀ_Cell_Checker checker(ਨਿਰਾ, ਇਹ);
	checker.check();
#ਜੇ_ਅੰਤ
}

ਖਾਲੀ RTLIL::Cell::fixup_parameters(ਤਾਰਕਿਕ ਸੈੱਟ_a_signed, ਤਾਰਕਿਕ ਸੈੱਟ_b_signed)
{
	ਜੇ (type.is_ਜਨਤਕ() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		ਵਾਪਸ;

	ਜੇ (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::ਚੌੜਾਈ_2] = Get_ਮਾਪ(connections_[ID::Y]);
		ਜੇ (type.ਵਿੱਚ(ID($pmux), ID($bmux)))
			parameters[ID::S_ਚੌੜਾਈ] = Get_ਮਾਪ(connections_[ID::S]);
		check();
		ਵਾਪਸ;
	}

	ਜੇ (type == ID($demux)) {
		parameters[ID::ਚੌੜਾਈ_2] = Get_ਮਾਪ(connections_[ID::A]);
		parameters[ID::S_ਚੌੜਾਈ] = Get_ਮਾਪ(connections_[ID::S]);
		check();
		ਵਾਪਸ;
	}

	ਜੇ (type == ID($lut) || type == ID($sop)) {
		parameters[ID::ਚੌੜਾਈ_2] = Get_ਮਾਪ(connections_[ID::A]);
		ਵਾਪਸ;
	}

	ਜੇ (type == ID($fa)) {
		parameters[ID::ਚੌੜਾਈ_2] = Get_ਮਾਪ(connections_[ID::Y]);
		ਵਾਪਸ;
	}

	ਜੇ (type == ID($lcu)) {
		parameters[ID::ਚੌੜਾਈ_2] = Get_ਮਾਪ(connections_[ID::CO]);
		ਵਾਪਸ;
	}

	ਜੇ (type == ID($macc_v2)) {
		parameters[ID::Y_ਚੌੜਾਈ] = Get_ਮਾਪ(connections_[ID::Y]);
		ਵਾਪਸ;
	}

	ਤਾਰਕਿਕ signedness_ab = !type.ਵਿੱਚ(ID($ਟੁਕੜਾ), ID($concat), ID($macc));

	ਜੇ (connections_.ਗਿਣਤੀ_2(ID::A)) {
		ਜੇ (signedness_ab) {
			ਜੇ (ਸੈੱਟ_a_signed)
				parameters[ID::A_SIGNED] = ਸੱਚ;
			ਨਹੀਂਤਾਂ ਜੇ (parameters.ਗਿਣਤੀ_2(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = ਝੂਠ;
		}
		parameters[ID::A_ਚੌੜਾਈ] = Get_ਮਾਪ(connections_[ID::A]);
	}

	ਜੇ (connections_.ਗਿਣਤੀ_2(ID::B)) {
		ਜੇ (signedness_ab) {
			ਜੇ (ਸੈੱਟ_b_signed)
				parameters[ID::B_SIGNED] = ਸੱਚ;
			ਨਹੀਂਤਾਂ ਜੇ (parameters.ਗਿਣਤੀ_2(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = ਝੂਠ;
		}
		parameters[ID::B_ਚੌੜਾਈ] = Get_ਮਾਪ(connections_[ID::B]);
	}

	ਜੇ (connections_.ਗਿਣਤੀ_2(ID::Y) && type != ID($concat))
		parameters[ID::Y_ਚੌੜਾਈ] = Get_ਮਾਪ(connections_[ID::Y]);

	ਜੇ (connections_.ਗਿਣਤੀ_2(ID::Q))
		parameters[ID::ਚੌੜਾਈ_2] = Get_ਮਾਪ(connections_[ID::Q]);

	check();
}

ਤਾਰਕਿਕ RTLIL::Cell::has_keep_attr() ਅਚਲ {
	ਵਾਪਸ get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

ਤਾਰਕਿਕ RTLIL::Cell::has_memid() ਅਚਲ
{
	ਵਾਪਸ type.ਵਿੱਚ(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

ਤਾਰਕਿਕ RTLIL::Cell::is_mem_cell() ਅਚਲ
{
	ਵਾਪਸ type.ਵਿੱਚ(ID($mem), ID($mem_v2)) || has_memid();
}

ਤਾਰਕਿਕ RTLIL::Cell::is_builtin_ff() ਅਚਲ {
	ਵਾਪਸ StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	wire = ਦੋਆਧਾਰੀ_ਅੰਕ.wire;
	ਵਿੱਥ = 0;
	ਜੇ (wire == ਨਿਰਾ)
		ਅੰਕੜੇ = {ਦੋਆਧਾਰੀ_ਅੰਕ.ਅੰਕੜੇ};
	ਨਹੀਂਤਾਂ
		ਵਿੱਥ = ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ;
	ਚੌੜਾਈ = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::ਕੱਢੋ_2(ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਲੰਮਾਈ) ਅਚਲ
{
	log_assert(ਵਿੱਥ >= 0);
	log_assert(ਲੰਮਾਈ >= 0);
	log_assert(ਵਿੱਥ + ਲੰਮਾਈ <= ਚੌੜਾਈ);
	RTLIL::SigChunk ret;
	ਜੇ (wire) {
		ret.wire = wire;
		ret.ਵਿੱਥ = ਇਹ->ਵਿੱਥ + ਵਿੱਥ;
		ret.ਚੌੜਾਈ = ਲੰਮਾਈ;
	} ਨਹੀਂਤਾਂ {
		ਲਈ (ਪੂਰਨ i = 0; i < ਲੰਮਾਈ; i++)
			ret.ਅੰਕੜੇ.ਧੱਕੋ_ਪਿੱਠ(ਅੰਕੜੇ[ਵਿੱਥ+i]);
		ret.ਚੌੜਾਈ = ਲੰਮਾਈ;
	}
	ਵਾਪਸ ret;
}

RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ RTLIL::SigChunk::ਸੰਚਾਲਕ[](ਪੂਰਨ ਵਿੱਥ) ਅਚਲ
{
	log_assert(ਵਿੱਥ >= 0);
	log_assert(ਵਿੱਥ < ਚੌੜਾਈ);
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ret;
	ਜੇ (wire) {
		ret.wire = wire;
		ret.ਵਿੱਥ = ਇਹ->ਵਿੱਥ + ਵਿੱਥ;
	} ਨਹੀਂਤਾਂ {
		ret.ਅੰਕੜੇ = ਅੰਕੜੇ[ਵਿੱਥ];
	}
	ਵਾਪਸ ret;
}

ਤਾਰਕਿਕ RTLIL::SigChunk::ਸੰਚਾਲਕ <(ਅਚਲ RTLIL::SigChunk &other) ਅਚਲ
{
	ਜੇ (wire && other.wire)
		ਜੇ (wire->name != other.wire->name)
			ਵਾਪਸ wire->name < other.wire->name;

	ਜੇ (wire != other.wire)
		ਵਾਪਸ wire < other.wire;

	ਜੇ (ਵਿੱਥ != other.ਵਿੱਥ)
		ਵਾਪਸ ਵਿੱਥ < other.ਵਿੱਥ;

	ਜੇ (ਚੌੜਾਈ != other.ਚੌੜਾਈ)
		ਵਾਪਸ ਚੌੜਾਈ < other.ਚੌੜਾਈ;

	ਵਾਪਸ ਅੰਕੜੇ < other.ਅੰਕੜੇ;
}

ਤਾਰਕਿਕ RTLIL::SigChunk::ਸੰਚਾਲਕ ==(ਅਚਲ RTLIL::SigChunk &other) ਅਚਲ
{
	ਵਾਪਸ wire == other.wire && ਚੌੜਾਈ == other.ਚੌੜਾਈ && ਵਿੱਥ == other.ਵਿੱਥ && ਅੰਕੜੇ == other.ਅੰਕੜੇ;
}

ਤਾਰਕਿਕ RTLIL::SigChunk::ਸੰਚਾਲਕ !=(ਅਚਲ RTLIL::SigChunk &other) ਅਚਲ
{
	ਜੇ (*ਇਹ == other)
		ਵਾਪਸ ਝੂਠ;
	ਵਾਪਸ ਸੱਚ;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	log_assert(parts.ਮਾਪ() > 0);
	ਸਵੈਚਾਲਿਤ ie = parts.begin();
	ਸਵੈਚਾਲਿਤ it = ie + parts.ਮਾਪ() - 1;
	ਜਦੋਂ (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ਅਚਲ RTLIL::Const &ਮੁੱਲ)
{
	ਜੇ (Get_ਮਾਪ(ਮੁੱਲ) != 0) {
		rep_ = CHUNK;
		ਨਵਾਂ (&chunk_) RTLIL::SigChunk(ਮੁੱਲ);
	} ਨਹੀਂਤਾਂ {
		init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&ਮੁੱਲ)
{
	ਜੇ (Get_ਮਾਪ(ਮੁੱਲ) != 0) {
		rep_ = CHUNK;
		ਨਵਾਂ (&chunk_) RTLIL::SigChunk(ਮੁੱਲ);
	} ਨਹੀਂਤਾਂ {
		init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ਅਚਲ RTLIL::SigChunk &chunk)
{
	ਜੇ (chunk.ਚੌੜਾਈ != 0) {
		rep_ = CHUNK;
		ਨਵਾਂ (&chunk_) RTLIL::SigChunk(chunk);
	} ਨਹੀਂਤਾਂ {
		init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	ਜੇ (chunk.ਚੌੜਾਈ != 0) {
		rep_ = CHUNK;
		ਨਵਾਂ (&chunk_) RTLIL::SigChunk(chunk);
	} ਨਹੀਂਤਾਂ {
		init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	ਜੇ (wire->ਚੌੜਾਈ != 0) {
		rep_ = CHUNK;
		ਨਵਾਂ (&chunk_) RTLIL::SigChunk(wire);
	} ਨਹੀਂਤਾਂ {
		init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਚੌੜਾਈ)
{
	ਜੇ (ਚੌੜਾਈ != 0) {
		rep_ = CHUNK;
		ਨਵਾਂ (&chunk_) RTLIL::SigChunk(wire, ਵਿੱਥ, ਚੌੜਾਈ);
	} ਨਹੀਂਤਾਂ {
		init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ਅਚਲ std::string &str)
{
	ਜੇ (str.ਮਾਪ() != 0) {
		rep_ = CHUNK;
		ਨਵਾਂ (&chunk_) RTLIL::SigChunk(str);
	} ਨਹੀਂਤਾਂ {
		init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ਪੂਰਨ val, ਪੂਰਨ ਚੌੜਾਈ)
{
	ਜੇ (ਚੌੜਾਈ != 0) {
		rep_ = CHUNK;
		ਨਵਾਂ (&chunk_) RTLIL::SigChunk(val, ਚੌੜਾਈ);
	} ਨਹੀਂਤਾਂ
		init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ ਚੌੜਾਈ)
{
	ਜੇ (ਚੌੜਾਈ != 0) {
		rep_ = CHUNK;
		ਨਵਾਂ (&chunk_) RTLIL::SigChunk(ਦੋਆਧਾਰੀ_ਅੰਕ, ਚੌੜਾਈ);
	} ਨਹੀਂਤਾਂ
		init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	check();
}

RTLIL::SigSpec::SigSpec(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ ਚੌੜਾਈ)
{
	ਜੇ (ਚੌੜਾਈ != 0) {
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire == ਨਿਰਾ) {
			rep_ = CHUNK;
			ਨਵਾਂ (&chunk_) RTLIL::SigChunk(ਦੋਆਧਾਰੀ_ਅੰਕ.ਅੰਕੜੇ, ਚੌੜਾਈ);
		} ਨਹੀਂਤਾਂ ਜੇ (ਚੌੜਾਈ == 1) {
			rep_ = CHUNK;
			ਨਵਾਂ (&chunk_) RTLIL::SigChunk(ਦੋਆਧਾਰੀ_ਅੰਕ);
		} ਨਹੀਂਤਾਂ {
			init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
			ਬਿੱਟ_2.reserve(ਚੌੜਾਈ);
			ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++)
				ਬਿੱਟ_2.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ);
		}
	} ਨਹੀਂਤਾਂ
		init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	check();
}

RTLIL::SigSpec::SigSpec(ਅਚਲ std::ਸਦਿਸ਼<RTLIL::SigChunk> &chunks)
{
	init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ਅਚਲ std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਬਿੱਟ)
{
	init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ)
		append(ਦੋਆਧਾਰੀ_ਅੰਕ);
	check();
}

RTLIL::SigSpec::SigSpec(ਅਚਲ pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਬਿੱਟ)
{
	init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ)
		append(ਦੋਆਧਾਰੀ_ਅੰਕ);
	check();
}

RTLIL::SigSpec::SigSpec(ਅਚਲ std::ਸੈੱਟ<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਬਿੱਟ)
{
	init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ)
		append(ਦੋਆਧਾਰੀ_ਅੰਕ);
	check();
}

RTLIL::SigSpec::SigSpec(ਤਾਰਕਿਕ ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	rep_ = CHUNK;
	ਨਵਾਂ (&chunk_) RTLIL::SigChunk(ਦੋਆਧਾਰੀ_ਅੰਕ ? RTLIL::S1 : RTLIL::S0);
	check();
}

ਖਾਲੀ RTLIL::SigSpec::Chunks::const_iterator::ਅਗਲਾ_chunk_ਬਿੱਟ()
{
	ਪੂਰਨ ਬਿੱਟ_ਮਾਪ = Get_ਮਾਪ(spec.ਬਿੱਟ_2);
	ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ >= ਬਿੱਟ_ਮਾਪ)
		ਵਾਪਸ;
	ਪੂਰਨ i = ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ;
	ਅਚਲ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ = spec.ਬਿੱਟ_2[i++];
	chunk.wire = ਦੋਆਧਾਰੀ_ਅੰਕ.wire;
	chunk.ਅੰਕੜੇ.clear();
	ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.is_wire()) {
		chunk.ਵਿੱਥ = ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ;
		ਜਦੋਂ (i < ਬਿੱਟ_ਮਾਪ && spec.ਬਿੱਟ_2[i].wire == ਦੋਆਧਾਰੀ_ਅੰਕ.wire &&
			spec.ਬਿੱਟ_2[i].ਵਿੱਥ == ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ + i - ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ)
			++i;
	} ਨਹੀਂਤਾਂ {
		chunk.ਵਿੱਥ = 0;
		chunk.ਅੰਕੜੇ.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ.ਅੰਕੜੇ);
		ਜਦੋਂ (i < ਬਿੱਟ_ਮਾਪ && !spec.ਬਿੱਟ_2[i].is_wire()) {
			chunk.ਅੰਕੜੇ.ਧੱਕੋ_ਪਿੱਠ(spec.ਬਿੱਟ_2[i].ਅੰਕੜੇ);
			++i;
		}
	}
	chunk.ਚੌੜਾਈ = i - ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ;
}

ਖਾਲੀ RTLIL::SigSpec::unpack()
{
	ਜੇ (rep_ == ਬਿੱਟ_3)
		ਵਾਪਸ;

	std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ;
	ਬਿੱਟ.reserve(chunk_.ਚੌੜਾਈ);
	ਲਈ (ਪੂਰਨ i = 0; i < chunk_.ਚੌੜਾਈ; i++)
		ਬਿੱਟ.emplace_ਪਿੱਠ(chunk_, i);

	chunk_.~SigChunk();
	rep_ = ਬਿੱਟ_3;
	ਨਵਾਂ (&ਬਿੱਟ_2) std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ>(std::move(ਬਿੱਟ));
}

ਖਾਲੀ RTLIL::SigSpec::try_repack()
{
	ਜੇ (rep_ != ਬਿੱਟ_3)
		ਵਾਪਸ;

	ਪੂਰਨ ਬਿੱਟ_ਮਾਪ = Get_ਮਾਪ(ਬਿੱਟ_2);
	ਜੇ (ਬਿੱਟ_ਮਾਪ == 0)
		ਵਾਪਸ;
	ਜੇ (ਬਿੱਟ_2[0].is_wire()) {
		ਲਈ (ਪੂਰਨ i = 1; i < ਬਿੱਟ_ਮਾਪ; i++)
			ਜੇ (ਬਿੱਟ_2[0].wire != ਬਿੱਟ_2[i].wire || ਬਿੱਟ_2[0].ਵਿੱਥ + i != ਬਿੱਟ_2[i].ਵਿੱਥ)
				ਵਾਪਸ;
		SigChunk chunk(ਬਿੱਟ_2[0].wire, ਬਿੱਟ_2[0].ਵਿੱਥ, ਬਿੱਟ_ਮਾਪ);
		ਬਿੱਟ_2.~ਸਦਿਸ਼();
		rep_ = CHUNK;
		ਨਵਾਂ (&chunk_) SigChunk(std::move(chunk));
		ਵਾਪਸ;
	}
	std::ਸਦਿਸ਼<RTLIL::State> ਬਿੱਟ;
	ਬਿੱਟ.reserve(ਬਿੱਟ_ਮਾਪ);
	ਬਿੱਟ.ਧੱਕੋ_ਪਿੱਠ(ਬਿੱਟ_2[0].ਅੰਕੜੇ);
	ਲਈ (ਪੂਰਨ i = 1; i < ਬਿੱਟ_ਮਾਪ; i++) {
		ਜੇ (ਬਿੱਟ_2[i].is_wire())
			ਵਾਪਸ;
		ਬਿੱਟ.ਧੱਕੋ_ਪਿੱਠ(ਬਿੱਟ_2[i].ਅੰਕੜੇ);
	}
	ਬਿੱਟ_2.~ਸਦਿਸ਼();
	rep_ = CHUNK;
	ਨਵਾਂ (&chunk_) SigChunk(std::move(ਬਿੱਟ));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ਅਚਲ
{
	Hasher h;
	ਲਈ (ਸਵੈਚਾਲਿਤ &c : chunks())
		ਜੇ (c.wire == ਨਿਰਾ) {
			ਲਈ (ਸਵੈਚਾਲਿਤ &v : c.ਅੰਕੜੇ)
				h.eat(v);
		} ਨਹੀਂਤਾਂ {
			h.eat(c.wire->name.ref());
			h.eat(c.ਵਿੱਥ);
			h.eat(c.ਚੌੜਾਈ);
		}
	Hasher::hash_t ਨਤੀਜਾ = h.yield();
	ਜੇ (ਨਤੀਜਾ == 0)
		ਨਤੀਜਾ = 1;
	hash_.ਸੈੱਟ(ਨਤੀਜਾ);
	ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ RTLIL::SigSpec::ਛਾਂਟੋ()
{
	unpack();
	std::ਛਾਂਟੋ(ਬਿੱਟ_2.begin(), ਬਿੱਟ_2.ਸਿਰਾ());
	hash_.clear();
	try_repack();
}

ਖਾਲੀ RTLIL::SigSpec::ਛਾਂਟੋ_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::ਸਦਿਸ਼<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> unique_ਬਿੱਟ = ਬਿੱਟ_2;
	std::ਛਾਂਟੋ(unique_ਬਿੱਟ.begin(), unique_ਬਿੱਟ.ਸਿਰਾ());
	ਸਵੈਚਾਲਿਤ ਆਖਰੀ = std::unique(unique_ਬਿੱਟ.begin(), unique_ਬਿੱਟ.ਸਿਰਾ());
	unique_ਬਿੱਟ.erase(ਆਖਰੀ, unique_ਬਿੱਟ.ਸਿਰਾ());

	*ਇਹ = unique_ਬਿੱਟ;
	hash_.clear();
	try_repack();
}

ਖਾਲੀ RTLIL::SigSpec::replace(ਅਚਲ RTLIL::SigSpec &ਨਮੂਨਾ, ਅਚਲ RTLIL::SigSpec &with)
{
	replace(ਨਮੂਨਾ, with, ਇਹ);
}

ਖਾਲੀ RTLIL::SigSpec::replace(ਅਚਲ RTLIL::SigSpec &ਨਮੂਨਾ, ਅਚਲ RTLIL::SigSpec &with, RTLIL::SigSpec *other) ਅਚਲ
{
	log_assert(other != ਨਿਰਾ);
	log_assert(ਮਾਪ() == other->ਮਾਪ());
	log_assert(ਨਮੂਨਾ.ਮਾਪ() == with.ਮਾਪ());

	dict<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ> ਨਮੂਨਾ_ਮੰਜ਼ਿਲ_with;
	ਪੂਰਨ ਨਮੂਨਾ_ਮਾਪ = ਨਮੂਨਾ.ਮਾਪ();
	ਲਈ (ਪੂਰਨ i = 0; i < ਨਮੂਨਾ_ਮਾਪ; i++) {
		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਨਮੂਨਾ_ਦੋਆਧਾਰੀ_ਅੰਕ = ਨਮੂਨਾ[i];
		ਜੇ (ਨਮੂਨਾ_ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ) {
			ਨਮੂਨਾ_ਮੰਜ਼ਿਲ_with.emplace(ਨਮੂਨਾ_ਦੋਆਧਾਰੀ_ਅੰਕ, i);
		}
	}

	ਪੂਰਨ this_ਮਾਪ = ਮਾਪ();
	ਤਾਰਕਿਕ other_modified = ਝੂਠ;
	ਲਈ (ਪੂਰਨ j = 0; j < this_ਮਾਪ; j++) {
		ਸਵੈਚਾਲਿਤ it = ਨਮੂਨਾ_ਮੰਜ਼ਿਲ_with.ਲੱਭੋ((*ਇਹ)[j]);
		ਜੇ (it != ਨਮੂਨਾ_ਮੰਜ਼ਿਲ_with.ਸਿਰਾ()) {
			other_modified = ਸੱਚ;
			(*other)[j] = with[it->ਦੂਜਾ];
		}
	}

	ਜੇ (other_modified)
		other->try_repack();
	other->check();
}

ਖਾਲੀ RTLIL::SigSpec::replace(ਅਚਲ dict<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਿਯਮਾਂ)
{
	replace(ਨਿਯਮਾਂ, ਇਹ);
}

ਖਾਲੀ RTLIL::SigSpec::replace(ਅਚਲ dict<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਿਯਮਾਂ, RTLIL::SigSpec *other) ਅਚਲ
{
	log_assert(other != ਨਿਰਾ);
	log_assert(ਮਾਪ() == other->ਮਾਪ());

	ਜੇ (ਨਿਯਮਾਂ.ਖਾਲੀ_ਥਾਵਾਂ()) ਵਾਪਸ;

	ਪੂਰਨ this_ਮਾਪ = ਮਾਪ();
	ਤਾਰਕਿਕ other_modified = ਝੂਠ;
	ਲਈ (ਪੂਰਨ i = 0; i < this_ਮਾਪ; i++) {
		ਸਵੈਚਾਲਿਤ it = ਨਿਯਮਾਂ.ਲੱਭੋ((*ਇਹ)[i]);
		ਜੇ (it != ਨਿਯਮਾਂ.ਸਿਰਾ()) {
			other_modified = ਸੱਚ;
			(*other)[i] = it->ਦੂਜਾ;
		}
	}

	ਜੇ (other_modified)
		other->try_repack();
	other->check();
}

ਖਾਲੀ RTLIL::SigSpec::replace(ਅਚਲ std::map<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਿਯਮਾਂ)
{
	replace(ਨਿਯਮਾਂ, ਇਹ);
}

ਖਾਲੀ RTLIL::SigSpec::replace(ਅਚਲ std::map<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਿਯਮਾਂ, RTLIL::SigSpec *other) ਅਚਲ
{
	log_assert(other != ਨਿਰਾ);
	log_assert(ਮਾਪ() == other->ਮਾਪ());

	ਜੇ (ਨਿਯਮਾਂ.ਖਾਲੀ_ਥਾਵਾਂ()) ਵਾਪਸ;

	ਪੂਰਨ this_ਮਾਪ = ਮਾਪ();
	ਤਾਰਕਿਕ other_modified = ਝੂਠ;
	ਲਈ (ਪੂਰਨ i = 0; i < this_ਮਾਪ; i++) {
		ਸਵੈਚਾਲਿਤ it = ਨਿਯਮਾਂ.ਲੱਭੋ((*ਇਹ)[i]);
		ਜੇ (it != ਨਿਯਮਾਂ.ਸਿਰਾ()) {
			other_modified = ਸੱਚ;
			(*other)[i] = it->ਦੂਜਾ;
		}
	}

	ਜੇ (other_modified)
		other->try_repack();
	other->check();
}

ਖਾਲੀ RTLIL::SigSpec::remove(ਅਚਲ RTLIL::SigSpec &ਨਮੂਨਾ)
{
	remove2(ਨਮੂਨਾ, ਨਿਰਾ);
}

ਖਾਲੀ RTLIL::SigSpec::remove(ਅਚਲ RTLIL::SigSpec &ਨਮੂਨਾ, RTLIL::SigSpec *other) ਅਚਲ
{
	RTLIL::SigSpec tmp = *ਇਹ;
	tmp.remove2(ਨਮੂਨਾ, other);
}

ਖਾਲੀ RTLIL::SigSpec::remove2(ਅਚਲ RTLIL::SigSpec &ਨਮੂਨਾ, RTLIL::SigSpec *other)
{
	unpack();
	ਜੇ (other != ਨਿਰਾ) {
		log_assert(ਮਾਪ() == other->ਮਾਪ());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਨਮੂਨਾ_ਬਿੱਟ;
	ਨਮੂਨਾ_ਬਿੱਟ.reserve(ਨਮੂਨਾ.ਮਾਪ());
	ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਨਮੂਨਾ)
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ)
			ਨਮੂਨਾ_ਬਿੱਟ.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ);

	// Compact in-place to avoid O(n^2) erase operations
	ਮਾਪ_t write_idx = 0;
	ਲਈ (ਮਾਪ_t read_idx = 0; read_idx < ਬਿੱਟ_2.ਮਾਪ(); read_idx++)
	{
		ਜੇ (!(ਬਿੱਟ_2[read_idx].wire != ਨਿਰਾ && ਨਮੂਨਾ_ਬਿੱਟ.ਗਿਣਤੀ_2(ਬਿੱਟ_2[read_idx]))) {
			ਜੇ (write_idx != read_idx) {
				ਬਿੱਟ_2[write_idx] = ਬਿੱਟ_2[read_idx];
				ਜੇ (other != ਨਿਰਾ)
					other->ਬਿੱਟ_2[write_idx] = other->ਬਿੱਟ_2[read_idx];
			}
			write_idx++;
		}
	}

	ਤਾਰਕਿਕ modified = (write_idx < ਬਿੱਟ_2.ਮਾਪ());
	ਜੇ (modified) {
		ਬਿੱਟ_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ਜੇ (other != ਨਿਰਾ && modified) {
		other->ਬਿੱਟ_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ਖਾਲੀ RTLIL::SigSpec::remove(ਅਚਲ pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਮੂਨਾ)
{
	remove2(ਨਮੂਨਾ, ਨਿਰਾ);
}

ਖਾਲੀ RTLIL::SigSpec::remove(ਅਚਲ pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਮੂਨਾ, RTLIL::SigSpec *other) ਅਚਲ
{
	RTLIL::SigSpec tmp = *ਇਹ;
	tmp.remove2(ਨਮੂਨਾ, other);
}

ਖਾਲੀ RTLIL::SigSpec::remove2(ਅਚਲ pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਮੂਨਾ, RTLIL::SigSpec *other)
{
	unpack();

	ਜੇ (other != ਨਿਰਾ) {
		log_assert(ਮਾਪ() == other->ਮਾਪ());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	ਮਾਪ_t write_idx = 0;
	ਲਈ (ਮਾਪ_t read_idx = 0; read_idx < ਬਿੱਟ_2.ਮਾਪ(); read_idx++) {
		ਜੇ (!(ਬਿੱਟ_2[read_idx].wire != ਨਿਰਾ && ਨਮੂਨਾ.ਗਿਣਤੀ_2(ਬਿੱਟ_2[read_idx]))) {
			ਜੇ (write_idx != read_idx) {
				ਬਿੱਟ_2[write_idx] = ਬਿੱਟ_2[read_idx];
				ਜੇ (other != ਨਿਰਾ)
					other->ਬਿੱਟ_2[write_idx] = other->ਬਿੱਟ_2[read_idx];
			}
			write_idx++;
		}
	}

	ਤਾਰਕਿਕ modified = (write_idx < ਬਿੱਟ_2.ਮਾਪ());
	ਜੇ (modified) {
		ਬਿੱਟ_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ਜੇ (other != ਨਿਰਾ && modified) {
		other->ਬਿੱਟ_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ਖਾਲੀ RTLIL::SigSpec::remove2(ਅਚਲ std::ਸੈੱਟ<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਮੂਨਾ, RTLIL::SigSpec *other)
{
	unpack();

	ਜੇ (other != ਨਿਰਾ) {
		log_assert(ਮਾਪ() == other->ਮਾਪ());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	ਮਾਪ_t write_idx = 0;
	ਲਈ (ਮਾਪ_t read_idx = 0; read_idx < ਬਿੱਟ_2.ਮਾਪ(); read_idx++) {
		ਜੇ (!(ਬਿੱਟ_2[read_idx].wire != ਨਿਰਾ && ਨਮੂਨਾ.ਗਿਣਤੀ_2(ਬਿੱਟ_2[read_idx]))) {
			ਜੇ (write_idx != read_idx) {
				ਬਿੱਟ_2[write_idx] = ਬਿੱਟ_2[read_idx];
				ਜੇ (other != ਨਿਰਾ)
					other->ਬਿੱਟ_2[write_idx] = other->ਬਿੱਟ_2[read_idx];
			}
			write_idx++;
		}
	}

	ਤਾਰਕਿਕ modified = (write_idx < ਬਿੱਟ_2.ਮਾਪ());
	ਜੇ (modified) {
		ਬਿੱਟ_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ਜੇ (other != ਨਿਰਾ && modified) {
		other->ਬਿੱਟ_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ਖਾਲੀ RTLIL::SigSpec::remove2(ਅਚਲ pool<RTLIL::Wire*> &ਨਮੂਨਾ, RTLIL::SigSpec *other)
{
	unpack();

	ਜੇ (other != ਨਿਰਾ) {
		log_assert(ਮਾਪ() == other->ਮਾਪ());
		other->unpack();
	}

	ਤਾਰਕਿਕ modified = ਝੂਠ;
	ਤਾਰਕਿਕ other_modified = ਝੂਠ;
	ਲਈ (ਪੂਰਨ i = Get_ਮਾਪ(ਬਿੱਟ_2) - 1; i >= 0; i--) {
		ਜੇ (ਬਿੱਟ_2[i].wire != ਨਿਰਾ && ਨਮੂਨਾ.ਗਿਣਤੀ_2(ਬਿੱਟ_2[i].wire)) {
			modified = ਸੱਚ;
			ਬਿੱਟ_2.erase(ਬਿੱਟ_2.begin() + i);
			ਜੇ (other != ਨਿਰਾ) {
				other_modified = ਸੱਚ;
				other->ਬਿੱਟ_2.erase(other->ਬਿੱਟ_2.begin() + i);
			}
		}
	}

	ਜੇ (modified) {
		hash_.clear();
		try_repack();
	}
	ਜੇ (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::ਕੱਢੋ_2(ਅਚਲ RTLIL::SigSpec &ਨਮੂਨਾ, ਅਚਲ RTLIL::SigSpec *other) ਅਚਲ
{
	log_assert(other == ਨਿਰਾ || ਮਾਪ() == other->ਮਾਪ());

	RTLIL::SigSpec ret;
	std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ_ਮੇਲ = ਮੰਜ਼ਿਲ_sigbit_ਸਦਿਸ਼();

	ਲਈ (ਸਵੈਚਾਲਿਤ& ਨਮੂਨਾ_chunk : ਨਮੂਨਾ.chunks()) {
		ਜੇ (other) {
			std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ_other = other->ਮੰਜ਼ਿਲ_sigbit_ਸਦਿਸ਼();
			ਪੂਰਨ i = 0;
			ਲਈ (ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ_ਮੇਲ) {
				ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire &&
					ਦੋਆਧਾਰੀ_ਅੰਕ.wire == ਨਮੂਨਾ_chunk.wire &&
					ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ >= ਨਮੂਨਾ_chunk.ਵਿੱਥ &&
					ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ < ਨਮੂਨਾ_chunk.ਵਿੱਥ + ਨਮੂਨਾ_chunk.ਚੌੜਾਈ)
					ret.append(ਬਿੱਟ_other[i]);
				++i;
			}
		} ਨਹੀਂਤਾਂ {
			ਲਈ (ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ_ਮੇਲ)
				ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire &&
					ਦੋਆਧਾਰੀ_ਅੰਕ.wire == ਨਮੂਨਾ_chunk.wire &&
					ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ >= ਨਮੂਨਾ_chunk.ਵਿੱਥ &&
					ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ < ਨਮੂਨਾ_chunk.ਵਿੱਥ + ਨਮੂਨਾ_chunk.ਚੌੜਾਈ)
					ret.append(ਦੋਆਧਾਰੀ_ਅੰਕ);
		}
	}

	ret.check();
	ਵਾਪਸ ret;
}

RTLIL::SigSpec RTLIL::SigSpec::ਕੱਢੋ_2(ਅਚਲ pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਮੂਨਾ, ਅਚਲ RTLIL::SigSpec *other) ਅਚਲ
{
	log_assert(other == ਨਿਰਾ || ਮਾਪ() == other->ਮਾਪ());

	std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ_ਮੇਲ = ਮੰਜ਼ਿਲ_sigbit_ਸਦਿਸ਼();
	RTLIL::SigSpec ret;

	ਜੇ (other) {
		std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ_other = other->ਮੰਜ਼ਿਲ_sigbit_ਸਦਿਸ਼();
		ਪੂਰਨ i = 0;
		ਲਈ (ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ_ਮੇਲ) {
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire && ਨਮੂਨਾ.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ))
				ret.append(ਬਿੱਟ_other[i]);
			++i;
		}
	} ਨਹੀਂਤਾਂ {
		ਲਈ (ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ_ਮੇਲ)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire && ਨਮੂਨਾ.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ))
				ret.append(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	ret.check();
	ਵਾਪਸ ret;
}

ਖਾਲੀ RTLIL::SigSpec::replace(ਪੂਰਨ ਵਿੱਥ, ਅਚਲ RTLIL::SigSpec &with)
{
	ਜੇ (with.ਮਾਪ() == 0)
		ਵਾਪਸ;

	unpack();

	log_assert(ਵਿੱਥ >= 0);
	log_assert(with.ਮਾਪ() >= 0);
	log_assert(ਵਿੱਥ+with.ਮਾਪ() <= ਮਾਪ());

	ਪੂਰਨ i = 0;
	ਲਈ (ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ : with.ਬਿੱਟ()) {
		ਬਿੱਟ_2.at(ਵਿੱਥ + i) = ਦੋਆਧਾਰੀ_ਅੰਕ;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

ਖਾਲੀ RTLIL::SigSpec::remove_const()
{
	ਜੇ (rep_ == CHUNK)
	{
		ਜੇ (chunk_.wire == ਨਿਰਾ) {
			chunk_.~SigChunk();
			init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ();
			hash_.clear();
		}
	}
	ਨਹੀਂਤਾਂ
	{
		std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> new_ਬਿੱਟ;
		new_ਬਿੱਟ.reserve(ਬਿੱਟ_2.ਮਾਪ());
		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ_2)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਨਿਰਾ)
				new_ਬਿੱਟ.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਜੇ (Get_ਮਾਪ(new_ਬਿੱਟ) != Get_ਮਾਪ(ਬਿੱਟ_2)) {
			ਬਿੱਟ_2.ਅਦਲਾ_ਬਦਲੀ(new_ਬਿੱਟ);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

ਖਾਲੀ RTLIL::SigSpec::remove(ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਲੰਮਾਈ)
{
	ਜੇ (ਲੰਮਾਈ == 0)
		ਵਾਪਸ;

	unpack();

	log_assert(ਵਿੱਥ >= 0);
	log_assert(ਲੰਮਾਈ >= 0);
	log_assert(ਵਿੱਥ + ਲੰਮਾਈ <= ਮਾਪ());

	ਬਿੱਟ_2.erase(ਬਿੱਟ_2.begin() + ਵਿੱਥ, ਬਿੱਟ_2.begin() + ਵਿੱਥ + ਲੰਮਾਈ);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::ਕੱਢੋ_2(ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਲੰਮਾਈ) ਅਚਲ
{
	log_assert(ਵਿੱਥ >= 0);
	log_assert(ਲੰਮਾਈ >= 0);
	log_assert(ਵਿੱਥ + ਲੰਮਾਈ <= ਮਾਪ());

	std::ਸਦਿਸ਼<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> extracted;
	Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਪਹਿਲਾ;
	ਤਾਰਕਿਕ is_packing = ਸੱਚ;
	ਲਈ (ਪੂਰਨ i = ਵਿੱਥ; i < ਵਿੱਥ + ਲੰਮਾਈ; i++) {
		ਤਾਰਕਿਕ was_packing_before = is_packing;
		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ = (*ਇਹ)[i];
		ਜੇ (i == ਵਿੱਥ) {
			ਪਹਿਲਾ = ਦੋਆਧਾਰੀ_ਅੰਕ;
			ਜੇ (!ਦੋਆਧਾਰੀ_ਅੰਕ.wire)
				is_packing = ਝੂਠ;
		} ਨਹੀਂਤਾਂ {
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਪਹਿਲਾ.wire)
				is_packing = ਝੂਠ;
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire)
				ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ != ਪਹਿਲਾ.ਵਿੱਥ + (i - ਵਿੱਥ))
					is_packing = ਝੂਠ;
		}
		ਜੇ (was_packing_before && !is_packing)
			ਲਈ (ਪੂਰਨ j = ਵਿੱਥ; j < i; j++)
				extracted.ਧੱਕੋ_ਪਿੱਠ((*ਇਹ)[j]);
		ਜੇ (!is_packing)
			extracted.ਧੱਕੋ_ਪਿੱਠ((*ਇਹ)[i]);
	}
	ਜੇ (is_packing)
		ਵਾਪਸ SigChunk(ਪਹਿਲਾ.wire, ਪਹਿਲਾ.ਵਿੱਥ, ਲੰਮਾਈ);

	ਵਾਪਸ extracted;
}

ਖਾਲੀ RTLIL::SigSpec::rewrite_wires(std::function<ਖਾਲੀ(RTLIL::Wire*& wire)> rewrite)
{
	ਜੇ (rep_ == CHUNK) {
		ਜੇ (chunk_.wire != ਖਾਲੀ_ਸੂਚਕ)
			rewrite(chunk_.wire);
		ਵਾਪਸ;
	}

	std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> new_ਬਿੱਟ;
	ਲਈ (ਅਚਲ RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		ਜੇ (c.wire != ਖਾਲੀ_ਸੂਚਕ)
			rewrite(c.wire);
		ਲਈ (ਪੂਰਨ i = 0; i < c.ਚੌੜਾਈ; i++)
			new_ਬਿੱਟ.emplace_ਪਿੱਠ(c, i);
	}
	ਬਿੱਟ_2 = std::move(new_ਬਿੱਟ);
	hash_.clear();
}

ਖਾਲੀ RTLIL::SigSpec::append(ਅਚਲ RTLIL::SigSpec &ਸੰਕੇਤ)
{
	ਜੇ (ਸੰਕੇਤ.ਮਾਪ() == 0)
		ਵਾਪਸ;

	ਜੇ (ਮਾਪ() == 0) {
		*ਇਹ = ਸੰਕੇਤ;
		ਵਾਪਸ;
	}

	hash_.clear();
	ਜੇ (rep_ == CHUNK && ਸੰਕੇਤ.rep_ == CHUNK && chunk_.wire == ਸੰਕੇਤ.chunk_.wire) {
		ਜੇ (chunk_.wire == ਨਿਰਾ) {
			chunk_.ਅੰਕੜੇ.ਪਾਓ(chunk_.ਅੰਕੜੇ.ਸਿਰਾ(), ਸੰਕੇਤ.chunk_.ਅੰਕੜੇ.begin(), ਸੰਕੇਤ.chunk_.ਅੰਕੜੇ.ਸਿਰਾ());
			chunk_.ਚੌੜਾਈ = Get_ਮਾਪ(chunk_.ਅੰਕੜੇ);
			ਵਾਪਸ;
		}
		ਜੇ (chunk_.ਵਿੱਥ + chunk_.ਚੌੜਾਈ == ਸੰਕੇਤ.chunk_.ਵਿੱਥ) {
			chunk_.ਚੌੜਾਈ += ਸੰਕੇਤ.chunk_.ਚੌੜਾਈ;
			ਵਾਪਸ;
		}
	}

	unpack();
	ਲਈ (ਅਚਲ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਸੰਕੇਤ.ਬਿੱਟ())
		ਬਿੱਟ_2.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ);
	check();
}

ਖਾਲੀ RTLIL::SigSpec::append(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	hash_.clear();

	ਜੇ (ਮਾਪ() == 0) {
		destroy();
		rep_ = CHUNK;
		ਨਵਾਂ (&chunk_) RTLIL::SigChunk(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਵਾਪਸ;
	}

	ਜੇ (rep_ == CHUNK && chunk_.wire == ਦੋਆਧਾਰੀ_ਅੰਕ.wire) {
		ਜੇ (chunk_.wire == ਨਿਰਾ) {
			chunk_.ਅੰਕੜੇ.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ.ਅੰਕੜੇ);
			chunk_.ਚੌੜਾਈ++;
			ਵਾਪਸ;
		}
		ਜੇ (chunk_.ਵਿੱਥ + chunk_.ਚੌੜਾਈ == ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ) {
			chunk_.ਚੌੜਾਈ++;
			ਵਾਪਸ;
		}
	}

	unpack();

	ਬਿੱਟ_2.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ);
	check();
}

ਖਾਲੀ RTLIL::SigSpec::extend_u0(ਪੂਰਨ ਚੌੜਾਈ, ਤਾਰਕਿਕ is_signed)
{
	ਜੇ (ਮਾਪ() > ਚੌੜਾਈ)
		remove(ਚੌੜਾਈ, ਮਾਪ() - ਚੌੜਾਈ);

	ਜੇ (ਮਾਪ() < ਚੌੜਾਈ) {
		RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ padding = ਮਾਪ() > 0 ? (*ਇਹ)[ਮਾਪ() - 1] : RTLIL::State::Sx;
		ਜੇ (!is_signed)
			padding = RTLIL::State::S0;
		ਜਦੋਂ (ਮਾਪ() < ਚੌੜਾਈ)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(ਪੂਰਨ ਅੰਕ) ਅਚਲ
{
	RTLIL::SigSpec sig;
	ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ; i++)
		sig.append(*ਇਹ);
	ਵਾਪਸ sig;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NDEBUG
ਖਾਲੀ RTLIL::SigSpec::check(ਅਚਲ Module *mod) ਅਚਲ
{
	ਜੇ (rep_ == CHUNK)
	{
		log_assert(chunk_.ਚੌੜਾਈ != 0);
		ਜੇ (chunk_.wire == ਨਿਰਾ) {
			log_assert(chunk_.ਵਿੱਥ == 0);
			log_assert(chunk_.ਅੰਕੜੇ.ਮਾਪ() == (ਮਾਪ_t)chunk_.ਚੌੜਾਈ);
		} ਨਹੀਂਤਾਂ {
			log_assert(chunk_.ਵਿੱਥ >= 0);
			log_assert(chunk_.ਚੌੜਾਈ >= 0);
			log_assert(chunk_.ਵਿੱਥ + chunk_.ਚੌੜਾਈ <= chunk_.wire->ਚੌੜਾਈ);
			log_assert(chunk_.ਅੰਕੜੇ.ਮਾਪ() == 0);
			ਜੇ (mod != ਖਾਲੀ_ਸੂਚਕ)
				log_assert(chunk_.wire->module == mod);
		}
	}
	ਨਹੀਂਤਾਂ ਜੇ (ਮਾਪ() <= 64)
	{
		ਜੇ (mod != ਖਾਲੀ_ਸੂਚਕ) {
			ਲਈ (ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ_2)
				ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire != ਖਾਲੀ_ਸੂਚਕ)
					log_assert(ਦੋਆਧਾਰੀ_ਅੰਕ.wire->module == mod);
		}
	}
}
#ਜੇ_ਅੰਤ

ਤਾਰਕਿਕ RTLIL::SigSpec::ਸੰਚਾਲਕ <(ਅਚਲ RTLIL::SigSpec &other) ਅਚਲ
{
	ਜੇ (ਇਹ == &other)
		ਵਾਪਸ ਝੂਠ;

	ਜੇ (ਮਾਪ() != other.ਮਾਪ())
		ਵਾਪਸ ਮਾਪ() < other.ਮਾਪ();

	ਸਵੈਚਾਲਿਤ other_it = other.chunks().begin();
	ਲਈ (ਅਚਲ SigChunk &c : chunks()) {
		ਜੇ (c != *other_it)
			ਵਾਪਸ c < *other_it;
		++other_it;
	}

	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ RTLIL::SigSpec::ਸੰਚਾਲਕ ==(ਅਚਲ RTLIL::SigSpec &other) ਅਚਲ
{
	ਜੇ (ਇਹ == &other)
		ਵਾਪਸ ਸੱਚ;

	ਜੇ (ਮਾਪ() != other.ਮਾਪ())
		ਵਾਪਸ ਝੂਠ;

	ਸਵੈਚਾਲਿਤ other_it = other.chunks().begin();
	ਲਈ (ਅਚਲ SigChunk &c : chunks()) {
		ਜੇ (c != *other_it)
			ਵਾਪਸ ਝੂਠ;
		++other_it;
	}

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::SigSpec::is_wire() ਅਚਲ
{
	Chunks cs = chunks();
	ਸਵੈਚਾਲਿਤ it = cs.begin();
	ਜੇ (it == cs.ਸਿਰਾ())
		ਵਾਪਸ ਝੂਠ;
	ਅਚਲ RTLIL::SigChunk &chunk = *it;
	ਵਾਪਸ chunk.wire && chunk.wire->ਚੌੜਾਈ == ਮਾਪ() && ++it == cs.ਸਿਰਾ();
}

ਤਾਰਕਿਕ RTLIL::SigSpec::is_chunk() ਅਚਲ
{
	Chunks cs = chunks();
	ਸਵੈਚਾਲਿਤ it = cs.begin();
	ਜੇ (it == cs.ਸਿਰਾ())
		ਵਾਪਸ ਝੂਠ;
	ਵਾਪਸ ++it == cs.ਸਿਰਾ();
}

ਤਾਰਕਿਕ RTLIL::SigSpec::ਗਿਆਤ_driver() ਅਚਲ
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : chunks())
		ਜੇ (chunk.is_wire() && !chunk.wire->ਗਿਆਤ_driver())
			ਵਾਪਸ ਝੂਠ;
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::SigSpec::is_fully_const() ਅਚਲ
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : chunks())
		ਜੇ (chunk.ਚੌੜਾਈ > 0 && chunk.wire != ਨਿਰਾ)
			ਵਾਪਸ ਝੂਠ;
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::SigSpec::is_fully_zero() ਅਚਲ
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : chunks()) {
		ਜੇ (chunk.ਚੌੜਾਈ > 0 && chunk.wire != ਨਿਰਾ)
			ਵਾਪਸ ਝੂਠ;
		ਲਈ (RTLIL::State d : chunk.ਅੰਕੜੇ)
			ਜੇ (d != RTLIL::State::S0)
				ਵਾਪਸ ਝੂਠ;
	}
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::SigSpec::is_fully_ones() ਅਚਲ
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : chunks()) {
		ਜੇ (chunk.ਚੌੜਾਈ > 0 && chunk.wire != ਨਿਰਾ)
			ਵਾਪਸ ਝੂਠ;
		ਲਈ (RTLIL::State d : chunk.ਅੰਕੜੇ)
			ਜੇ (d != RTLIL::State::S1)
				ਵਾਪਸ ਝੂਠ;
	}
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::SigSpec::is_fully_def() ਅਚਲ
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : chunks()) {
		ਜੇ (chunk.ਚੌੜਾਈ > 0 && chunk.wire != ਨਿਰਾ)
			ਵਾਪਸ ਝੂਠ;
		ਲਈ (RTLIL::State d : chunk.ਅੰਕੜੇ)
			ਜੇ (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				ਵਾਪਸ ਝੂਠ;
	}
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::SigSpec::is_fully_undef() ਅਚਲ
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : chunks()) {
		ਜੇ (chunk.ਚੌੜਾਈ > 0 && chunk.wire != ਨਿਰਾ)
			ਵਾਪਸ ਝੂਠ;
		ਲਈ (RTLIL::State d : chunk.ਅੰਕੜੇ)
			ਜੇ (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				ਵਾਪਸ ਝੂਠ;
	}
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::SigSpec::has_const() ਅਚਲ
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : chunks())
		ਜੇ (chunk.ਚੌੜਾਈ > 0 && chunk.wire == ਨਿਰਾ)
			ਵਾਪਸ ਸੱਚ;
	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ RTLIL::SigSpec::has_const(State state) ਅਚਲ
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : chunks())
		ਜੇ (chunk.ਚੌੜਾਈ > 0 && chunk.wire == ਨਿਰਾ && std::ਲੱਭੋ(chunk.ਅੰਕੜੇ.begin(), chunk.ਅੰਕੜੇ.ਸਿਰਾ(), state) != chunk.ਅੰਕੜੇ.ਸਿਰਾ())
			ਵਾਪਸ ਸੱਚ;
	ਵਾਪਸ ਝੂਠ;
}


ਤਾਰਕਿਕ RTLIL::SigSpec::has_marked_ਬਿੱਟ() ਅਚਲ
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &chunk : chunks())
		ਜੇ (chunk.ਚੌੜਾਈ > 0 && chunk.wire == ਨਿਰਾ) {
			ਲਈ (RTLIL::State d : chunk.ਅੰਕੜੇ)
				ਜੇ (d == RTLIL::State::Sm)
					ਵਾਪਸ ਸੱਚ;
		}
	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ RTLIL::SigSpec::is_onehot(ਪੂਰਨ *pos) ਅਚਲ
{
	ਜੇ (std::optional<RTLIL::Const> c = try_as_const())
		ਵਾਪਸ c->is_onehot(pos);
	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ RTLIL::SigSpec::as_bool() ਅਚਲ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ਮੁੱਲ());
	ਵਾਪਸ c->as_bool();
}

ਪੂਰਨ RTLIL::SigSpec::as_int(ਤਾਰਕਿਕ is_signed) ਅਚਲ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ਮੁੱਲ());
	ਵਾਪਸ c->as_int(is_signed);
}

ਤਾਰਕਿਕ RTLIL::SigSpec::convertible_ਮੰਜ਼ਿਲ_int(ਤਾਰਕਿਕ is_signed) ਅਚਲ
{
	std::optional<RTLIL::Const> c = try_as_const();
	ਜੇ (!c.has_ਮੁੱਲ())
		ਵਾਪਸ ਝੂਠ;
	ਵਾਪਸ c->convertible_ਮੰਜ਼ਿਲ_int(is_signed);
}

std::optional<ਪੂਰਨ> RTLIL::SigSpec::try_as_int(ਤਾਰਕਿਕ is_signed) ਅਚਲ
{
	std::optional<RTLIL::Const> c = try_as_const();
	ਜੇ (!c.has_ਮੁੱਲ())
		ਵਾਪਸ std::nullopt;
	ਵਾਪਸ c->try_as_int(is_signed);
}

ਪੂਰਨ RTLIL::SigSpec::as_int_saturating(ਤਾਰਕਿਕ is_signed) ਅਚਲ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ਮੁੱਲ());
	ਵਾਪਸ c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ਅਚਲ
{
	std::string str;
	str.reserve(ਮਾਪ());
	std::ਸਦਿਸ਼<RTLIL::SigChunk> chunks = *ਇਹ;
	ਲਈ (ਮਾਪ_t i = chunks.ਮਾਪ(); i > 0; i--) {
		ਅਚਲ RTLIL::SigChunk &chunk = chunks[i-1];
		ਜੇ (chunk.wire != ਨਿਰਾ)
			str.append(chunk.ਚੌੜਾਈ, '?');
		ਨਹੀਂਤਾਂ
			str += RTLIL::Const(chunk.ਅੰਕੜੇ).as_string();
	}
	ਵਾਪਸ str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ਅਚਲ
{
	Chunks cs = chunks();
	ਸਵੈਚਾਲਿਤ it = cs.begin();
	ਜੇ (it == cs.ਸਿਰਾ())
		ਵਾਪਸ RTLIL::Const();
	SigChunk chunk = *it;
	ਜੇ (chunk.wire != ਨਿਰਾ || ++it != cs.ਸਿਰਾ())
		ਵਾਪਸ std::nullopt;
	ਵਾਪਸ RTLIL::Const(std::move(chunk.ਅੰਕੜੇ));
}

RTLIL::Const RTLIL::SigSpec::as_const() ਅਚਲ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ਮੁੱਲ());
	ਵਾਪਸ *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ਅਚਲ
{
	Chunks cs = chunks();
	ਸਵੈਚਾਲਿਤ it = cs.begin();
	log_assert(it != cs.ਸਿਰਾ());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.ਸਿਰਾ() && chunk.wire && chunk.wire->ਚੌੜਾਈ == ਮਾਪ());
	ਵਾਪਸ chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ਅਚਲ
{
	Chunks cs = chunks();
	ਸਵੈਚਾਲਿਤ it = cs.begin();
	log_assert(it != cs.ਸਿਰਾ());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.ਸਿਰਾ());
	ਵਾਪਸ chunk;
}

RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ RTLIL::SigSpec::as_ਦੋਆਧਾਰੀ_ਅੰਕ() ਅਚਲ
{
	ਵਾਪਸ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(*ਇਹ);
}

ਤਾਰਕਿਕ RTLIL::SigSpec::ਮੇਲ(ਅਚਲ ਅੱਖਰ* ਨਮੂਨਾ) ਅਚਲ
{
	ਪੂਰਨ ਨਮੂਨਾ_len = ਲੜੀ_ਲੰਬਾਈ(ਨਮੂਨਾ);
	log_assert(ਨਮੂਨਾ_len == ਮਾਪ());

	ਲਈ (ਪੂਰਨ i = 0; i < ਨਮੂਨਾ_len; ++i) {
		ਅੱਖਰ ch = ਨਮੂਨਾ[i];
		ਜੇ (ch == ' ')
			ਜਾਰੀ;
		RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ = (*ਇਹ)[ਨਮੂਨਾ_len - 1 - i];
		ਜੇ (ch == '*') {
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ != State::Sz && ਦੋਆਧਾਰੀ_ਅੰਕ != State::Sx)
				ਵਾਪਸ ਝੂਠ;
			ਜਾਰੀ;
		}
		ਜੇ (ch == '0') {
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ != State::S0)
				ਵਾਪਸ ਝੂਠ;
		} ਨਹੀਂਤਾਂ
		ਜੇ (ch == '1') {
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ != State::S1)
				ਵਾਪਸ ਝੂਠ;
		} ਨਹੀਂਤਾਂ
			log_abort();
	}

	ਵਾਪਸ ਸੱਚ;
}

std::ਸੈੱਟ<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> RTLIL::SigSpec::ਮੰਜ਼ਿਲ_sigbit_ਸੈੱਟ() ਅਚਲ
{
	std::ਸੈੱਟ<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> sigbits;
	ਲਈ (ਸਵੈਚਾਲਿਤ &c : chunks())
		ਲਈ (ਪੂਰਨ i = 0; i < c.ਚੌੜਾਈ; i++)
			sigbits.ਪਾਓ(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(c, i));
	ਵਾਪਸ sigbits;
}

pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> RTLIL::SigSpec::ਮੰਜ਼ਿਲ_sigbit_pool() ਅਚਲ
{
	pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> sigbits;
	sigbits.reserve(ਮਾਪ());
	ਲਈ (ਸਵੈਚਾਲਿਤ &c : chunks())
		ਲਈ (ਪੂਰਨ i = 0; i < c.ਚੌੜਾਈ; i++)
			sigbits.ਪਾਓ(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(c, i));
	ਵਾਪਸ sigbits;
}

std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> RTLIL::SigSpec::ਮੰਜ਼ਿਲ_sigbit_ਸਦਿਸ਼() ਅਚਲ
{
	std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਨਤੀਜਾ;
	ਨਤੀਜਾ.reserve(ਮਾਪ());
	ਲਈ (Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ : *ਇਹ)
		ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ);
	ਵਾਪਸ ਨਤੀਜਾ;
}

std::map<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> RTLIL::SigSpec::ਮੰਜ਼ਿਲ_sigbit_map(ਅਚਲ RTLIL::SigSpec &other) ਅਚਲ
{
	ਪੂਰਨ this_ਮਾਪ = ਮਾਪ();
	log_assert(this_ਮਾਪ == other.ਮਾਪ());

	std::map<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> new_map;
	ਲਈ (ਪੂਰਨ i = 0; i < this_ਮਾਪ; i++)
		new_map[(*ਇਹ)[i]] = other[i];

	ਵਾਪਸ new_map;
}

dict<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> RTLIL::SigSpec::ਮੰਜ਼ਿਲ_sigbit_dict(ਅਚਲ RTLIL::SigSpec &other) ਅਚਲ
{
	ਪੂਰਨ this_ਮਾਪ = ਮਾਪ();
	log_assert(this_ਮਾਪ == other.ਮਾਪ());

	dict<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> new_map;
	new_map.reserve(this_ਮਾਪ);
	ਲਈ (ਪੂਰਨ i = 0; i < this_ਮਾਪ; i++)
		new_map[(*ਇਹ)[i]] = other[i];

	ਵਾਪਸ new_map;
}

ਸਥਿਰ ਖਾਲੀ sigspec_parse_ਵੰਡੋ(std::ਸਦਿਸ਼<std::string> &ਟੋਕਨ, ਅਚਲ std::string &ਪਾਠ, ਅੱਖਰ sep)
{
	ਮਾਪ_t ਸ਼ੁਰੂ = 0, ਸਿਰਾ = 0;
	ਜਦੋਂ ((ਸਿਰਾ = ਪਾਠ.ਲੱਭੋ(sep, ਸ਼ੁਰੂ)) != std::string::npos) {
		ਟੋਕਨ.ਧੱਕੋ_ਪਿੱਠ(ਪਾਠ.substr(ਸ਼ੁਰੂ, ਸਿਰਾ - ਸ਼ੁਰੂ));
		ਸ਼ੁਰੂ = ਸਿਰਾ + 1;
	}
	ਟੋਕਨ.ਧੱਕੋ_ਪਿੱਠ(ਪਾਠ.substr(ਸ਼ੁਰੂ));
}

ਸਥਿਰ RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ਅਚਲ std::string &netname)
{
	ਵਾਪਸ module->wire(netname);
}

ਤਾਰਕਿਕ RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::ਸਦਿਸ਼<std::string> ਟੋਕਨ;
	sigspec_parse_ਵੰਡੋ(ਟੋਕਨ, str, ',');

	sig = RTLIL::SigSpec();
	ਲਈ (ਪੂਰਨ tokidx = ਪੂਰਨ(ਟੋਕਨ.ਮਾਪ())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = ਟੋਕਨ[tokidx];
		std::string indices;

		ਜੇ (netname.ਮਾਪ() == 0)
			ਜਾਰੀ;

		ਜੇ (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			ਸਵੈਚਾਲਿਤ ast = p.const2ast(netname);
			ਜੇ (ast == ਖਾਲੀ_ਸੂਚਕ)
				ਵਾਪਸ ਝੂਠ;
			sig.append(RTLIL::Const(ast->ਬਿੱਟ));
			ਜਾਰੀ;
		}

		ਜੇ (module == ਨਿਰਾ)
			ਵਾਪਸ ਝੂਠ;

		ਜੇ (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		ਜੇ (sigspec_parse_wire(module, netname) == ਖਾਲੀ_ਸੂਚਕ) {
			ਮਾਪ_t indices_pos = netname.ਮਾਪ()-1;
			ਜੇ (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				ਜਦੋਂ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				ਜੇ (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					ਜਦੋਂ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				ਜੇ (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		ਜੇ (wire == ਖਾਲੀ_ਸੂਚਕ)
			ਵਾਪਸ ਝੂਠ;

		ਜੇ (!indices.ਖਾਲੀ_ਥਾਵਾਂ()) {
			std::ਸਦਿਸ਼<std::string> ਸੂਚਕਾਂਕ_ਟੋਕਨ;
			sigspec_parse_ਵੰਡੋ(ਸੂਚਕਾਂਕ_ਟੋਕਨ, indices.substr(1, indices.ਮਾਪ()-2), ':');
			ਜੇ (ਸੂਚਕਾਂਕ_ਟੋਕਨ.ਮਾਪ() == 1) {
				ਪੂਰਨ a = atoi(ਸੂਚਕਾਂਕ_ਟੋਕਨ.at(0).c_str());
				ਜੇ (a < 0 || a >= wire->ਚੌੜਾਈ)
					ਵਾਪਸ ਝੂਠ;
				sig.append(RTLIL::SigSpec(wire, a));
			} ਨਹੀਂਤਾਂ {
				ਪੂਰਨ a = atoi(ਸੂਚਕਾਂਕ_ਟੋਕਨ.at(0).c_str());
				ਪੂਰਨ b = atoi(ਸੂਚਕਾਂਕ_ਟੋਕਨ.at(1).c_str());
				ਜੇ (a > b) {
					ਪੂਰਨ tmp = a;
					a = b, b = tmp;
				}
				ਜੇ (a < 0 || a >= wire->ਚੌੜਾਈ)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (b < 0 || b >= wire->ਚੌੜਾਈ)
					ਵਾਪਸ ਝੂਠ;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} ਨਹੀਂਤਾਂ
			sig.append(wire);
	}

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	ਜੇ (str.ਖਾਲੀ_ਥਾਵਾਂ() || str[0] != '@')
		ਵਾਪਸ parse(sig, module, str);

	IdString sel_name = design->twines().ਲੱਭੋ(RTLIL::escape_id(str.substr(1)));
	ਜੇ (sel_name == IdString::Null || design->selection_vars.ਗਿਣਤੀ_2(sel_name) == 0)
		ਵਾਪਸ ਝੂਠ;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : module->wires_)
		ਜੇ (sel.selected_member(module->name, it.ਦੂਜਾ->name))
			sig.append(it.ਦੂਜਾ);

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RTLIL::SigSpec::parse_rhs(ਅਚਲ RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	ਜੇ (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.ਮਾਪ());
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.ਮਾਪ());
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (lhs.is_chunk()) {
		ਅੱਖਰ *p = (ਅੱਖਰ*)str.c_str(), *endptr;
		ਲੰਮਾ ਪੂਰਨ val = strtol(p, &endptr, 10);
		ਜੇ (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.ਮਾਪ());
			ਵਾਪਸ ਸੱਚ;
		}
	}

	ਜੇ (!parse(sig, module, str))
		ਵਾਪਸ ਝੂਠ;
	ਜੇ (sig.ਮਾਪ() > lhs.ਮਾਪ())
		sig.remove(lhs.ਮਾਪ(), sig.ਮਾਪ() - lhs.ਮਾਪ());
	ਵਾਪਸ ਸੱਚ;
}

RTLIL::SigSpec::ਸੰਚਾਲਕ std::ਸਦਿਸ਼<RTLIL::SigChunk>() ਅਚਲ
{
	std::ਸਦਿਸ਼<RTLIL::SigChunk> ਨਤੀਜਾ;
	ਲਈ (ਅਚਲ RTLIL::SigChunk &c : chunks())
		ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(c);
	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Case_ਨਿਯਮ::~Case_ਨਿਯਮ()
{
	ਲਈ (ਸਵੈਚਾਲਿਤ it = switches.begin(); it != switches.ਸਿਰਾ(); it++)
		ਮਿਟਾਓ *it;
}

ਤਾਰਕਿਕ RTLIL::Case_ਨਿਯਮ::ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ
{
	ਵਾਪਸ actions.ਖਾਲੀ_ਥਾਵਾਂ() && switches.ਖਾਲੀ_ਥਾਵਾਂ();
}

RTLIL::Case_ਨਿਯਮ *RTLIL::Case_ਨਿਯਮ::clone() ਅਚਲ
{
	RTLIL::Case_ਨਿਯਮ *new_caserule = ਨਵਾਂ RTLIL::Case_ਨਿਯਮ;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->ਗੁਣ = ਗੁਣ;
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : switches)
		new_caserule->switches.ਧੱਕੋ_ਪਿੱਠ(it->clone());
	ਵਾਪਸ new_caserule;
}

RTLIL::Switch_ਨਿਯਮ::~Switch_ਨਿਯਮ()
{
	ਲਈ (ਸਵੈਚਾਲਿਤ it = cases.begin(); it != cases.ਸਿਰਾ(); it++)
		ਮਿਟਾਓ *it;
}

ਤਾਰਕਿਕ RTLIL::Switch_ਨਿਯਮ::ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ
{
	ਵਾਪਸ cases.ਖਾਲੀ_ਥਾਵਾਂ();
}

RTLIL::Switch_ਨਿਯਮ *RTLIL::Switch_ਨਿਯਮ::clone() ਅਚਲ
{
	RTLIL::Switch_ਨਿਯਮ *new_switchrule = ਨਵਾਂ RTLIL::Switch_ਨਿਯਮ;
	new_switchrule->ਸੰਕੇਤ = ਸੰਕੇਤ;
	new_switchrule->ਗੁਣ = ਗੁਣ;
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : cases)
		new_switchrule->cases.ਧੱਕੋ_ਪਿੱਠ(it->clone());
	ਵਾਪਸ new_switchrule;

}

RTLIL::Sync_ਨਿਯਮ *RTLIL::Sync_ਨਿਯਮ::clone() ਅਚਲ
{
	RTLIL::Sync_ਨਿਯਮ *new_syncrule = ਨਵਾਂ RTLIL::Sync_ਨਿਯਮ;
	new_syncrule->type = type;
	new_syncrule->ਸੰਕੇਤ = ਸੰਕੇਤ;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	ਵਾਪਸ new_syncrule;
}

RTLIL::Process::~Process()
{
	ਲਈ (ਸਵੈਚਾਲਿਤ it = syncs.begin(); it != syncs.ਸਿਰਾ(); it++)
		ਮਿਟਾਓ *it;
}

RTLIL::Process *RTLIL::Process::clone() ਅਚਲ
{
	RTLIL::Process *new_proc = ਨਵਾਂ RTLIL::Process;

	new_proc->ਗੁਣ = ਗੁਣ;

	RTLIL::Case_ਨਿਯਮ *rc_ptr = ਜੜ੍ਹ_case.clone();
	new_proc->ਜੜ੍ਹ_case = *rc_ptr;
	rc_ptr->switches.clear();
	ਮਿਟਾਓ rc_ptr;

	ਲਈ (ਸਵੈਚਾਲਿਤ &it : syncs)
		new_proc->syncs.ਧੱਕੋ_ਪਿੱਠ(it->clone());

	ਵਾਪਸ new_proc;
}

RTLIL::ਸਮ੍ਰਿਤੀ::~ਸਮ੍ਰਿਤੀ()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	RTLIL::ਸਮ੍ਰਿਤੀ::get_all_memorys()->erase(hashidx_);
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
ਸਥਿਰ std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::ਸਮ੍ਰਿਤੀ*> all_memorys;
std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::ਸਮ੍ਰਿਤੀ*> *RTLIL::ਸਮ੍ਰਿਤੀ::get_all_memorys(ਖਾਲੀ)
{
	ਵਾਪਸ &all_memorys;
}
#ਜੇ_ਅੰਤ
YOSYS_NAMESPACE_ਸਿਰਾ
