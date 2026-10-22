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

#ضمن "kernel/rtlil.h"
#ضمن "kernel/yosys.h"
#ضمن "kernel/macc.h"
#ضمن "kernel/newcelltypes.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/threading.h"
#ضمن "frontends/verilog/verilog_frontend.h"
#ضمن "frontends/verilog/preproc.h"
#ضمن "backends/rtlil/rtlil_backend.h"

#ضمن <string.h>
#ضمن <algorithm>
#ضمن <charconv>
#ضمن <optional>
#ضمن <string_view>
#ضمن <sstream>

YOSYS_NAMESPACE_BEGIN

dict<std::string, std::string> RTLIL::constpad;

ثابت pool<IdString> &RTLIL::builtin_ff_cell_types() {
	ساكن ثابت pool<IdString> res = []() {
		pool<IdString> r;
		لكل (سائيز_t i = 0; i < StaticCellTypes::builder.ڳڻپ; i++) {
			خودڪار &cell = StaticCellTypes::builder.cells[i];
			إذا (cell.features.is_ff)
				r.وجهو(cell.type);
		}
		أرجع r;
	}();
	أرجع res;
}

#وصف_ڪريو check(condition) log_assert(condition && "malformed Const union")

ثابت Const::bitvectype& Const::get_بٽ() ثابت {
	check(is_بٽ());
	أرجع *get_if_بٽ();
}

ثابت std::string& Const::get_str() ثابت {
	check(is_str());
	أرجع *get_if_str();
}

Const::bitvectype& Const::get_بٽ() {
	check(is_بٽ());
	أرجع *get_if_بٽ();
}

std::string& Const::get_str() {
	check(is_str());
	أرجع *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	جديد ((فراغ*)&str_) std::string(std::move(str));
	ٽيگ = backing_ٽيگ::string;
}

RTLIL::Const::Const(ڊگهو ڊگهو val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	حرف بائيٽون[] = {
		(حرف)(val >> 24), (حرف)(val >> 16), (حرف)(val >> 8), (حرف)val
	};
	جديد ((فراغ*)&str_) std::string(بائيٽون, 4);
	ٽيگ = backing_ٽيگ::string;
}

RTLIL::Const::Const(ڊگهو ڊگهو val, صحيح ويڪر)
{
	log_assert(ويڪر >= 0 && ويڪر < RTLIL::ويڪر_حد_بندي);
	flags = RTLIL::CONST_FLAG_NONE;
	إذا ((ويڪر & 7) == 0) {
		جديد ((فراغ*)&str_) std::string();
		ٽيگ = backing_ٽيگ::string;
		std::string& str = get_str();
		صحيح بائيٽون = ويڪر >> 3;
		نشانوارو حرف sign_byte = val < 0 ? -1 : 0;
		str.resize(بائيٽون, sign_byte);
		بائيٽون = std::min<صحيح>(بائيٽون, حجم(val));
		لكل (صحيح i = 0; i < بائيٽون; i++) {
			str[str.سائيز() - 1 - i] = val;
			val = val >> 8;
		}
		أرجع;
	}

	جديد ((فراغ*)&بٽ_2) bitvectype();
	ٽيگ = backing_ٽيگ::بٽ;
	bitvectype& bv = get_بٽ();
	bv.reserve(ويڪر);
	لكل (صحيح i = 0; i < ويڪر; i++) {
		bv.ڌڪو_پٺ((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State ٻٽو_انگ, صحيح ويڪر)
{
	log_assert(ويڪر >= 0 && ويڪر < RTLIL::ويڪر_حد_بندي);
	flags = RTLIL::CONST_FLAG_NONE;
	جديد ((فراغ*)&بٽ_2) bitvectype();
	ٽيگ = backing_ٽيگ::بٽ;
	bitvectype& bv = get_بٽ();
	bv.reserve(ويڪر);
	لكل (صحيح i = 0; i < ويڪر; i++)
		bv.ڌڪو_پٺ(ٻٽو_انگ);
}

RTLIL::Const::Const(ثابت std::ويڪٽر<منطقي> &بٽ)
{
	flags = RTLIL::CONST_FLAG_NONE;
	جديد ((فراغ*)&بٽ_2) bitvectype();
	ٽيگ = backing_ٽيگ::بٽ;
	bitvectype& bv = get_بٽ();
	bv.reserve(بٽ.سائيز());
	لكل (ثابت خودڪار &b : بٽ)
		bv.emplace_پٺ(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ثابت RTLIL::Const &other) {
	ٽيگ = other.ٽيگ;
	flags = other.flags;
	إذا (is_str())
		جديد ((فراغ*)&str_) std::string(other.get_str());
	وإلا إذا (is_بٽ())
		جديد ((فراغ*)&بٽ_2) bitvectype(other.get_بٽ());
	وإلا
		check(خطأ);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	ٽيگ = other.ٽيگ;
	flags = other.flags;
	إذا (is_str())
		جديد ((فراغ*)&str_) std::string(std::move(other.get_str()));
	وإلا إذا (is_بٽ())
		جديد ((فراغ*)&بٽ_2) bitvectype(std::move(other.get_بٽ()));
	وإلا
		check(خطأ);
}

RTLIL::Const &RTLIL::Const::عامل =(ثابت RTLIL::Const &other) {
	flags = other.flags;
	إذا (other.is_str()) {
		إذا (!is_str()) {
			// sketchy zone
			check(is_بٽ());
			بٽ_2.~bitvectype();
			(فراغ)جديد ((فراغ*)&str_) std::string();
		}
		ٽيگ = other.ٽيگ;
		get_str() = other.get_str();
	} وإلا إذا (other.is_بٽ()) {
		إذا (!is_بٽ()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(فراغ)جديد ((فراغ*)&بٽ_2) bitvectype();
		}
		ٽيگ = other.ٽيگ;
		get_بٽ() = other.get_بٽ();
	} وإلا {
		check(خطأ);
	}
	أرجع *هذا;
}

RTLIL::Const::~Const() {
	إذا (is_بٽ())
		بٽ_2.~bitvectype();
	وإلا إذا (is_str())
		str_.~string();
	وإلا
		check(خطأ);
}

منطقي RTLIL::Const::عامل<(ثابت RTLIL::Const &other) ثابت
{
	إذا (سائيز() != other.سائيز())
		أرجع سائيز() < other.سائيز();

	لكل (صحيح i = 0; i < سائيز(); i++)
		إذا ((*هذا)[i] != other[i])
			أرجع (*هذا)[i] < other[i];

	أرجع خطأ;
}

منطقي RTLIL::Const::عامل ==(ثابت RTLIL::Const &other) ثابت
{
	إذا (is_str() && other.is_str())
		أرجع get_str() == other.get_str();
	إذا (is_بٽ() && other.is_بٽ())
		أرجع get_بٽ() == other.get_بٽ();

	إذا (سائيز() != other.سائيز())
		أرجع خطأ;

	لكل (صحيح i = 0; i < سائيز(); i++)
	إذا ((*هذا)[i] != other[i])
		أرجع خطأ;

	أرجع سچ;
}

منطقي RTLIL::Const::عامل !=(ثابت RTLIL::Const &other) ثابت
{
	أرجع !(*هذا == other);
}

std::ويڪٽر<RTLIL::State>& RTLIL::Const::بٽ_اندروني()
{
	bitvectorize_اندروني();
	أرجع get_بٽ();
}

std::ويڪٽر<RTLIL::State> RTLIL::Const::منزل_بٽ() ثابت
{
	std::ويڪٽر<State> v;
	لكل (خودڪار ٻٽو_انگ : *هذا)
		v.ڌڪو_پٺ(ٻٽو_انگ);
	أرجع v;
}

منطقي RTLIL::Const::as_bool() ثابت
{
	إذا (is_str()) {
		لكل (حرف ch : get_str())
			إذا (ch != 0)
				أرجع سچ;
		أرجع خطأ;
	}

	ثابت bitvectype& bv = get_بٽ();
	لكل (سائيز_t i = 0; i < bv.سائيز(); i++)
		إذا (bv[i] == State::S1)
			أرجع سچ;
	أرجع خطأ;
}

صحيح RTLIL::Const::as_int(منطقي is_signed) ثابت
{
	int32_t ret = 0;
	إذا (ثابت std::string *s = get_if_str()) {
		صحيح سائيز = Get_سائيز(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		لكل (صحيح i = std::min(4, سائيز); i > 0; i--)
			ret |= static_cast<بنانشان حرف>((*s)[سائيز - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		إذا (is_signed && سائيز > 0 && سائيز < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << سائيز*8;
		أرجع ret;
	}

	ثابت bitvectype& bv = get_بٽ();
	صحيح significant_بٽ = std::min(Get_سائيز(bv), 32);
	لكل (صحيح i = 0; i < significant_بٽ; i++)
		إذا (bv[i] == State::S1)
			ret |= 1 << i;
	إذا (is_signed && significant_بٽ > 0 && significant_بٽ < 32 && bv.پٺ() == State::S1 )
		ret |= UINT32_MAX << significant_بٽ;
	أرجع ret;
}

منطقي RTLIL::Const::convertible_منزل_int(منطقي is_signed) ثابت
{
	خودڪار سائيز = get_min_سائيز(is_signed);

	إذا (سائيز < 0)
		أرجع خطأ;

	// If it fits in 31 bits it is definitely convertible
	إذا (سائيز <= 31)
		أرجع سچ;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	إذا (سائيز == 32) {
		إذا (is_signed)
			أرجع سچ;
		أرجع (*هذا)[سائيز - 1] != State::S1;
	}

	أرجع خطأ;
}

std::optional<صحيح> RTLIL::Const::try_as_int(منطقي is_signed) ثابت
{
	إذا (!convertible_منزل_int(is_signed))
		أرجع std::nullopt;
	أرجع as_int(is_signed);
}

صحيح RTLIL::Const::as_int_saturating(منطقي is_signed) ثابت
{
	إذا (!convertible_منزل_int(is_signed)) {
		إذا (!is_signed)
			أرجع std::numeric_limits<صحيح>::max();

		ثابت خودڪار min_سائيز = get_min_سائيز(is_signed);
		log_assert(min_سائيز > 0);
		ثابت خودڪار neg = (*هذا)[min_سائيز - 1];
		أرجع neg ? std::numeric_limits<صحيح>::min() : std::numeric_limits<صحيح>::max();
	}
	أرجع as_int(is_signed);
}

فراغ RTLIL::Const::ٽيگ_bare_integer_const(ثابت std::string &قدر)
{
	إذا (قدر.خالي_جايون() || قدر.ڳوليو('\'') != std::string::npos)
		أرجع;
	سائيز_t شروعات = (قدر[0] == '-' || قدر[0] == '+') ? 1 : 0;
	إذا (شروعات < قدر.سائيز() && std::all_جو(قدر.begin() + شروعات, قدر.پڇاڙي(), ::انگ_آهي))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

صحيح RTLIL::Const::get_min_سائيز(منطقي is_signed) ثابت
{
	إذا (خالي_جايون()) أرجع 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_ٻٽو_انگ;
	إذا (is_signed)
		leading_ٻٽو_انگ = (پٺ() == RTLIL::State::Sx) ? RTLIL::State::S0 : پٺ();
	وإلا
		leading_ٻٽو_انگ = RTLIL::State::S0;

	خودڪار idx = سائيز();
	طالما (idx > 0 && (*هذا)[idx -1] == leading_ٻٽو_انگ) {
		idx--;
	}

	// signed needs one leading bit
	إذا (is_signed && idx < سائيز()) {
		idx++;
	}
	// must be at least one bit
	أرجع (idx == 0) ? 1 : idx;
}

فراغ RTLIL::Const::compress(منطقي is_signed)
{
	خودڪار idx = get_min_سائيز(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<صحيح> RTLIL::Const::as_int_compress(منطقي is_signed) ثابت
{
	أرجع try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ثابت حرف* any) ثابت
{
	صحيح sz = سائيز();
	std::string ret;
	ret.reserve(sz);
	لكل (صحيح i = sz - 1; i >= 0; --i)
		بدل ((*هذا)[i]) {
			حالة S0: ret.ڌڪو_پٺ('0'); اكسر;
			حالة S1: ret.ڌڪو_پٺ('1'); اكسر;
			حالة Sx: ret.ڌڪو_پٺ('x'); اكسر;
			حالة Sz: ret.ڌڪو_پٺ('z'); اكسر;
			حالة Sa: ret += any; اكسر;
			حالة Sm: ret.ڌڪو_پٺ('m'); اكسر;
		}
	أرجع ret;
}

RTLIL::Const RTLIL::Const::from_string(ثابت std::string &str)
{
	Const c;
	bitvectype& bv = c.get_بٽ();
	bv.reserve(str.سائيز());
	لكل (خودڪار it = str.rbegin(); it != str.rend(); it++)
		بدل (*it) {
			حالة '0': bv.ڌڪو_پٺ(State::S0); اكسر;
			حالة '1': bv.ڌڪو_پٺ(State::S1); اكسر;
			حالة 'x': bv.ڌڪو_پٺ(State::Sx); اكسر;
			حالة 'z': bv.ڌڪو_پٺ(State::Sz); اكسر;
			حالة 'm': bv.ڌڪو_پٺ(State::Sm); اكسر;
			افتراضي: bv.ڌڪو_پٺ(State::Sa);
		}
	أرجع c;
}

std::string RTLIL::Const::decode_string() ثابت
{
	إذا (خودڪار str = get_if_str())
		أرجع *str;

	ثابت bitvectype& bv = get_بٽ();
	ثابت صحيح n = Get_سائيز(bv);
	ثابت صحيح n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	صحيح i = n_over_8 * 8;
	إذا (i < n) {
		حرف ch = 0;
		لكل (صحيح j = 0; j < (n - i); j++) {
			إذا (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		إذا (ch != 0)
			s.append({ch});
	}
	i -= 8;
	لكل (; i >= 0; i -= 8) {
		حرف ch = 0;
		لكل (صحيح j = 0; j < 8; j++) {
			إذا (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		إذا (ch != 0)
			s.append({ch});
	}
	أرجع s;
}

صحيح RTLIL::Const::سائيز() ثابت {
	إذا (is_str())
		أرجع 8 * str_.سائيز();
	وإلا {
		check(is_بٽ());
		أرجع بٽ_2.سائيز();
	}
}

منطقي RTLIL::Const::خالي_جايون() ثابت {
	إذا (is_str())
		أرجع str_.خالي_جايون();
	وإلا {
		check(is_بٽ());
		أرجع بٽ_2.خالي_جايون();
	}
}

فراغ RTLIL::Const::bitvectorize_اندروني() {
	إذا (ٽيگ == backing_ٽيگ::بٽ)
		أرجع;

	check(is_str());

	bitvectype new_بٽ;

	new_بٽ.reserve(str_.سائيز() * 8);
	لكل (صحيح i = str_.سائيز() - 1; i >= 0; i--) {
		بنانشان حرف ch = str_[i];
		لكل (صحيح j = 0; j < 8; j++) {
			new_بٽ.ڌڪو_پٺ((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(فراغ)جديد ((فراغ*)&بٽ_2) bitvectype(std::move(new_بٽ));
		ٽيگ = backing_ٽيگ::بٽ;
	}
}

فراغ RTLIL::Const::append(ثابت RTLIL::Const &other) {
	bitvectorize_اندروني();
	bitvectype& bv = get_بٽ();
	bv.وجهو(bv.پڇاڙي(), other.begin(), other.پڇاڙي());
}

RTLIL::State RTLIL::Const::const_iterator::عامل*() ثابت {
	إذا (خودڪار bv = والد->get_if_بٽ())
		أرجع (*bv)[idx];

	صحيح char_idx = والد->get_str().سائيز() - idx / 8 - 1;
	منطقي ٻٽو_انگ = (والد->get_str()[char_idx] & (1 << (idx % 8)));
	أرجع ٻٽو_انگ ? State::S1 : State::S0;
}

منطقي RTLIL::Const::is_fully_zero() ثابت
{
	إذا (خودڪار str = get_if_str()) {
		لكل (حرف ch : *str)
			إذا (ch != 0)
				أرجع خطأ;
		أرجع سچ;
	}

	ثابت bitvectype& bv = get_بٽ();

	لكل (ثابت خودڪار &ٻٽو_انگ : bv)
		إذا (ٻٽو_انگ != RTLIL::State::S0)
			أرجع خطأ;

	أرجع سچ;
}

منطقي RTLIL::Const::is_fully_ones() ثابت
{
	إذا (خودڪار str = get_if_str()) {
		لكل (حرف ch : *str)
			إذا (ch != (حرف)0xff)
				أرجع خطأ;
		أرجع سچ;
	}

	ثابت bitvectype& bv = get_بٽ();
	لكل (ثابت خودڪار &ٻٽو_انگ : bv)
		إذا (ٻٽو_انگ != RTLIL::State::S1)
			أرجع خطأ;

	أرجع سچ;
}

منطقي RTLIL::Const::is_fully_def() ثابت
{
	إذا (is_str())
		أرجع سچ;

	ثابت bitvectype& bv = get_بٽ();
	لكل (ثابت خودڪار &ٻٽو_انگ : bv)
		إذا (ٻٽو_انگ != RTLIL::State::S0 && ٻٽو_انگ != RTLIL::State::S1)
			أرجع خطأ;

	أرجع سچ;
}

منطقي RTLIL::Const::is_fully_undef() ثابت
{
	إذا (خودڪار str = get_if_str())
		أرجع str->خالي_جايون();

	ثابت bitvectype& bv = get_بٽ();
	لكل (ثابت خودڪار &ٻٽو_انگ : bv)
		إذا (ٻٽو_انگ != RTLIL::State::Sx && ٻٽو_انگ != RTLIL::State::Sz)
			أرجع خطأ;

	أرجع سچ;
}

منطقي RTLIL::Const::is_fully_undef_x_only() ثابت
{
	إذا (خودڪار str = get_if_str())
		أرجع str->خالي_جايون();

	ثابت bitvectype& bv = get_بٽ();
	لكل (ثابت خودڪار &ٻٽو_انگ : bv)
		إذا (ٻٽو_انگ != RTLIL::State::Sx)
			أرجع خطأ;

	أرجع سچ;
}

منطقي RTLIL::Const::is_onehot(صحيح *pos) ثابت
{
	منطقي مليو = خطأ;
	صحيح سائيز = Get_سائيز(*هذا);
	لكل (صحيح i = 0; i < سائيز; i++) {
		State ٻٽو_انگ = (*هذا)[i];
		إذا (ٻٽو_انگ != RTLIL::State::S0 && ٻٽو_انگ != RTLIL::State::S1)
			أرجع خطأ;
		إذا (ٻٽو_انگ == RTLIL::State::S1) {
			إذا (مليو)
				أرجع خطأ;
			إذا (pos)
				*pos = i;
			مليو = سچ;
		}
	}
	أرجع مليو;
}

Hasher RTLIL::Const::hash_into(Hasher h) ثابت
{
	إذا (خودڪار str = get_if_str())
		أرجع hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ثابت bitvectype &bv = get_بٽ();
	صحيح سائيز = Get_سائيز(bv);
	std::string packed;
	صحيح packed_سائيز = (سائيز + 7) >> 3;
	packed.resize(packed_سائيز, 0);
	لكل (صحيح bi = 0; bi < packed_سائيز; ++bi) {
		حرف ch = 0;
		صحيح پڇاڙي = std::min((bi + 1)*8, سائيز);
		لكل (صحيح i = bi*8; i < پڇاڙي; ++i) {
			RTLIL::State b = bv[i];
			إذا (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<حرف>::hash_into(ch, h);
				لكل (; i < سائيز; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(سائيز);
				أرجع h;
			}
			ch |= static_cast<صحيح>(b) << (i & 7);
		}
		packed[packed_سائيز - 1 - bi] = ch;
	}
	أرجع hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::ڪڍو_2(صحيح وٿي, صحيح len, RTLIL::State padding) ثابت {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	لكل (صحيح i = وٿي; i < وٿي + len; i++)
		ret_bv.ڌڪو_پٺ(i < Get_سائيز(*هذا) ? (*هذا)[i] : padding);
	أرجع RTLIL::Const(ret_bv);
}
#وصف_رد check /* check(condition) for Const */

منطقي RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ثابت
{
	أرجع خاصيتون.ڳڻپ(id);
}

فراغ RTLIL::AttrObject::سيٽ_bool_attribute(RTLIL::IdString id, منطقي قدر)
{
	إذا (قدر)
		خاصيتون[id] = RTLIL::Const(1);
	وإلا
		خاصيتون.erase(id);
}

منطقي RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ثابت
{
	ثابت خودڪار it = خاصيتون.ڳوليو(id);
	إذا (it == خاصيتون.پڇاڙي())
		أرجع خطأ;
	أرجع it->ٻيو.as_bool();
}

فراغ RTLIL::AttrObject::سيٽ_string_attribute(RTLIL::IdString id, string قدر)
{
	إذا (قدر.خالي_جايون())
		خاصيتون.erase(id);
	وإلا
		خاصيتون[id] = قدر;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ثابت
{
	std::string قدر;
	ثابت خودڪار it = خاصيتون.ڳوليو(id);
	إذا (it != خاصيتون.پڇاڙي())
		قدر = it->ٻيو.decode_string();
	أرجع قدر;
}

فراغ RTLIL::AttrObject::سيٽ_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &معلومات)
{
	string attrval;
	لكل (ثابت خودڪار &s : معلومات) {
		إذا (!attrval.خالي_جايون())
			attrval += "|";
		attrval += s;
	}
	سيٽ_string_attribute(id, attrval);
}

فراغ RTLIL::AttrObject::جوڙيو_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &معلومات)
{
	pool<string> union_معلومات = get_strpool_attribute(id);
	union_معلومات.وجهو(معلومات.begin(), معلومات.پڇاڙي());
	إذا (!union_معلومات.خالي_جايون())
		سيٽ_strpool_attribute(id, union_معلومات);
}

فراغ RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->خاصيتون = std::move(buf);
}

نطاق {
	سانچو<قسم_نالو F>
	فراغ walk_process_attr_objects(RTLIL::Process *process, F گهمو) {
		گهمو(process);
		std::ويڪٽر<RTLIL::Case_قاعدو*> case_ڍير{&process->پاڙ_case};
		طالما (!case_ڍير.خالي_جايون()) {
			RTLIL::Case_قاعدو *cs = case_ڍير.پٺ();
			case_ڍير.ڪڍو_پٺ();
			گهمو(cs);
			لكل (خودڪار *sw : cs->switches) {
				گهمو(sw);
				لكل (خودڪار *case_ : sw->cases)
					case_ڍير.ڌڪو_پٺ(case_);
			}
		}
		لكل (خودڪار *متزامن : process->syncs)
			لكل (خودڪار &mwa : متزامن->mem_write_actions)
				گهمو(&mwa);
	}

	سانچو<قسم_نالو F>
	فراغ walk_attr_objects(RTLIL::Design *design, F گهمو) {
		لكل (خودڪار &[_, module] : design->modules_) {
			گهمو(module);
			لكل (خودڪار &[_, wire] : module->wires_)
				گهمو(wire);
			لكل (خودڪار &[_, mem] : module->memories)
				گهمو(mem);
			لكل (خودڪار &[_, cell] : module->cells_)
				گهمو(cell);
			لكل (خودڪار &[_, process] : module->processes)
				walk_process_attr_objects(process, گهمو);
		}
	}
}


سائيز_t RTLIL::Design::gc_twines()
{
	int64_t شروعات = PerformanceTimer::query();
	pool<IdString> جيئرو;
	خودڪار پاڙ = [&](IdString ref) {
		إذا (ref != IdString::Null)
			جيئرو.وجهو(ref);
	};

	walk_attr_objects(هذا, [&](ثابت RTLIL::AttrObject *obj) {
		لكل (خودڪار &attr : obj->خاصيتون)
			پاڙ(attr.پهريون);
	});

	پاڙ(selected_سرگرم_module);

	لكل (خودڪار &[name, module] : modules_) {
		پاڙ(name);
		لكل (IdString پورٽ : module->ports)
			پاڙ(پورٽ);
		لكل (IdString param : module->avail_parameters)
			پاڙ(param);
		لكل (خودڪار &[param, _] : module->parameter_default_قدرون)
			پاڙ(param);
		لكل (خودڪار &[name, _] : module->memories)
			پاڙ(name);
		لكل (خودڪار &[name, _] : module->processes)
			پاڙ(name);
		لكل (خودڪار &[name, wire] : module->wires_) {
			پاڙ(name);
			إذا (wire->معلوم_driver())
				پاڙ(wire->driver_پورٽ());
		}
		لكل (خودڪار &[name, cell] : module->cells_) {
			پاڙ(name);
			پاڙ(cell->type);
			لكل (خودڪار &conn : cell->connections())
				پاڙ(conn.پهريون);
			لكل (خودڪار &param : cell->parameters)
				پاڙ(param.پهريون);
		}
		لكل (خودڪار &[_, process] : module->processes) {
			std::ويڪٽر<RTLIL::Case_قاعدو*> case_ڍير{&process->پاڙ_case};
			طالما (!case_ڍير.خالي_جايون()) {
				RTLIL::Case_قاعدو *cs = case_ڍير.پٺ();
				case_ڍير.ڪڍو_پٺ();
				لكل (خودڪار *sw : cs->switches) {
					لكل (خودڪار *case_ : sw->cases)
						case_ڍير.ڌڪو_پٺ(case_);
				}
			}
			لكل (خودڪار *متزامن : process->syncs) {
				لكل (خودڪار &mwa : متزامن->mem_write_actions)
					پاڙ(mwa.memid);
			}
		}
	}

	خودڪار پاڙ_selection = [&](ثابت RTLIL::Selection &sel) {
		لكل (IdString m : sel.selected_modules)
			پاڙ(m);
		لكل (خودڪار &[m, members] : sel.selected_members) {
			پاڙ(m);
			لكل (IdString member : members)
				پاڙ(member);
		}
	};

	لكل (خودڪار &[name, sel] : selection_vars) {
		پاڙ(name);
		پاڙ_selection(sel);
	}

	لكل (خودڪار &sel : selection_ڍير)
		پاڙ_selection(sel);

	سائيز_t erased = twines_.gc(جيئرو);

	int64_t وقت_ns = PerformanceTimer::query() - شروعات;
	Pass::ڪٽيو_from_current_runtime_ns(وقت_ns);
	twine_gc_ns += وقت_ns;
	++twine_gc_ڳڻپ;
	أرجع erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ثابت
{
	pool<string> معلومات;
	إذا (خاصيتون.ڳڻپ(id) != 0)
		لكل (خودڪار s : ورهايو_ٽوڪن(get_string_attribute(id), "|"))
			معلومات.وجهو(s);
	أرجع معلومات;
}

فراغ RTLIL::AttrObject::سيٽ_hdlname_attribute(ثابت ويڪٽر<string> &hierarchy)
{
	string attrval;
	لكل (ثابت خودڪار &ident : hierarchy) {
		إذا (!attrval.خالي_جايون())
			attrval += " ";
		attrval += ident;
	}
	سيٽ_string_attribute(ID::hdlname, attrval);
}

ويڪٽر<string> RTLIL::AttrObject::get_hdlname_attribute() ثابت
{
	أرجع ورهايو_ٽوڪن(get_string_attribute(ID::hdlname), " ");
}

فراغ RTLIL::AttrObject::سيٽ_intvec_attribute(RTLIL::IdString id, ثابت ويڪٽر<صحيح> &معلومات)
{
	std::stringstream attrval;
	لكل (خودڪار &i : معلومات) {
		إذا (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	خاصيتون[id] = RTLIL::Const(attrval.str());
}

ويڪٽر<صحيح> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ثابت
{
	ويڪٽر<صحيح> معلومات;
	خودڪار it = خاصيتون.ڳوليو(id);
	إذا (it != خاصيتون.پڇاڙي())
		لكل (ثابت خودڪار &s : ورهايو_ٽوڪن(خاصيتون.at(id).decode_string())) {
			حرف *پڇاڙي = خالي_اشارو;
			errno = 0;
			ڊگهو قدر = strtol(s.c_str(), &پڇاڙي, 10);
			إذا (پڇاڙي != s.c_str() + s.سائيز())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			إذا (errno == ERANGE || قدر < INT_MIN || قدر > وڌ_کان_وڌ_عدد)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			معلومات.ڌڪو_پٺ(قدر);
		}
	أرجع معلومات;
}

منطقي RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ثابت
{
	إذا (current_design != خالي_اشارو) {
		خودڪار module = current_design->module(mod_name);
		أرجع module && module->get_blackbox_attribute();
	} وإلا {
		log_warning("Unable to check if module is boxed for null design.\n");
		أرجع خطأ;
	}
}

منطقي RTLIL::Selection::selected_module(RTLIL::IdString mod_name) ثابت
{
	إذا (complete_selection)
		أرجع سچ;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (ڀريل_selection)
		أرجع سچ;
	إذا (selected_modules.ڳڻپ(mod_name) > 0)
		أرجع سچ;
	إذا (selected_members.ڳڻپ(mod_name) > 0)
		أرجع سچ;
	أرجع خطأ;
}

منطقي RTLIL::Selection::selected_سڄو_module(RTLIL::IdString mod_name) ثابت
{
	إذا (complete_selection)
		أرجع سچ;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (ڀريل_selection)
		أرجع سچ;
	إذا (selected_modules.ڳڻپ(mod_name) > 0)
		أرجع سچ;
	أرجع خطأ;
}

منطقي RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	إذا (complete_selection)
		أرجع سچ;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (ڀريل_selection)
		أرجع سچ;
	إذا (selected_modules.ڳڻپ(mod_name) > 0)
		أرجع سچ;
	إذا (selected_members.ڳڻپ(mod_name) > 0)
		إذا (selected_members.at(mod_name).ڳڻپ(memb_name) > 0)
			أرجع سچ;
	أرجع خطأ;
}

فراغ RTLIL::Selection::optimize(RTLIL::Design *design)
{
	إذا (design != current_design) {
		current_design = design;
	}

	إذا (selects_boxes && ڀريل_selection)
		complete_selection = سچ;
	إذا (complete_selection) {
		ڀريل_selection = خطأ;
		selects_boxes = سچ;
	}
	إذا (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		أرجع;
	}

	std::ويڪٽر<RTLIL::IdString> del_list, جوڙيو_list;

	del_list.clear();
	لكل (خودڪار mod_name : selected_modules) {
		إذا (current_design->module(mod_name) == خالي_اشارو || (!selects_boxes && boxed_module(mod_name)))
			del_list.ڌڪو_پٺ(mod_name);
		selected_members.erase(mod_name);
	}
	لكل (خودڪار mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	لكل (خودڪار &it : selected_members)
		إذا (current_design->module(it.پهريون) == خالي_اشارو || (!selects_boxes && boxed_module(it.پهريون)))
			del_list.ڌڪو_پٺ(it.پهريون);
	لكل (خودڪار mod_name : del_list)
		selected_members.erase(mod_name);

	لكل (خودڪار &it : selected_members) {
		del_list.clear();
		لكل (خودڪار memb_name : it.ٻيو)
			إذا (current_design->module(it.پهريون)->ڳڻپ_id(memb_name) == 0)
				del_list.ڌڪو_پٺ(memb_name);
		لكل (خودڪار memb_name : del_list)
			it.ٻيو.erase(memb_name);
	}

	del_list.clear();
	جوڙيو_list.clear();
	لكل (خودڪار &it : selected_members)
		إذا (it.ٻيو.سائيز() == 0)
			del_list.ڌڪو_پٺ(it.پهريون);
		وإلا إذا (it.ٻيو.سائيز() == current_design->module(it.پهريون)->wires_.سائيز() + current_design->module(it.پهريون)->memories.سائيز() +
				current_design->module(it.پهريون)->cells_.سائيز() + current_design->module(it.پهريون)->processes.سائيز())
			جوڙيو_list.ڌڪو_پٺ(it.پهريون);
	لكل (خودڪار mod_name : del_list)
		selected_members.erase(mod_name);
	لكل (خودڪار mod_name : جوڙيو_list) {
		selected_members.erase(mod_name);
		selected_modules.وجهو(mod_name);
	}

	إذا (selected_modules.سائيز() == current_design->modules_.سائيز()) {
		selected_modules.clear();
		selected_members.clear();
		إذا (selects_boxes)
			complete_selection = سچ;
		وإلا
			ڀريل_selection = سچ;
	}
}

فراغ RTLIL::Selection::clear()
{
	ڀريل_selection = خطأ;
	complete_selection = خطأ;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_مقرر_ڪري (جديد define_map_t)
{
	ساكن بنانشان صحيح hashidx_ڳڻپ = 123456789;
	hashidx_ڳڻپ = mkhash_xorshift(hashidx_ڳڻپ);
	hashidx_ = hashidx_ڳڻپ;

	refcount_modules_ = 0;
	selected_سرگرم_module.clear();
	ڌڪو_ڀريل_selection();

	RTLIL::Design::get_all_designs()->وجهو(std::pair<بنانشان صحيح, RTLIL::Design*>(hashidx_, هذا));
}

RTLIL::Design::~Design()
{
	لكل (خودڪار &pr : modules_)
		ميٽيو pr.ٻيو;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

ساكن std::map<بنانشان صحيح, RTLIL::Design*> all_designs;
std::map<بنانشان صحيح, RTLIL::Design*> *RTLIL::Design::get_all_designs(فراغ)
{
	أرجع &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	أرجع RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ثابت RTLIL::Module *RTLIL::Design::module(IdString id) ثابت {
	twines_.check_owned(id);
	أرجع modules_.ڳڻپ(id) ? modules_.at(id) : عدم;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	أرجع modules_.ڳڻپ(id) ? modules_.at(id) : عدم;
}

RTLIL::Module *RTLIL::Design::چوٽي_module() ثابت
{
	RTLIL::Module *module = خالي_اشارو;
	صحيح module_ڳڻپ = 0;

	لكل (خودڪار mod : selected_modules()) {
		إذا (mod->get_bool_attribute(ID::چوٽي))
			أرجع mod;
		module_ڳڻپ++;
		module = mod;
	}

	أرجع module_ڳڻپ == 1 ? module : خالي_اشارو;
}

RTLIL::Module *RTLIL::Design::جوڙيو_Module(RTLIL::IdString name)
{
	إذا (modules_.ڳڻپ(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = جديد RTLIL::Module;
	modules_[name] = module;
	module->design = هذا;
	module->name = name;

	لكل (خودڪار mon : monitors)
		mon->notify_module_جوڙيو(module);

	إذا (yosys_xtrace) {
		لاگرٿم("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	أرجع module;
}

فراغ RTLIL::Design::scratchpad_unset(ثابت std::string &varname)
{
	scratchpad.erase(varname);
}

فراغ RTLIL::Design::scratchpad_سيٽ_int(ثابت std::string &varname, صحيح قدر)
{
	scratchpad[varname] = stringf("%d", قدر);
}

فراغ RTLIL::Design::scratchpad_سيٽ_bool(ثابت std::string &varname, منطقي قدر)
{
	scratchpad[varname] = قدر ? "true" : "false";
}

فراغ RTLIL::Design::scratchpad_سيٽ_string(ثابت std::string &varname, std::string قدر)
{
	scratchpad[varname] = std::move(قدر);
}

صحيح RTLIL::Design::scratchpad_get_int(ثابت std::string &varname, صحيح default_قدر_2) ثابت
{
	خودڪار it = scratchpad.ڳوليو(varname);
	إذا (it == scratchpad.پڇاڙي())
		أرجع default_قدر_2;

	ثابت std::string &str = it->ٻيو;

	إذا (str == "0" || str == "false")
		أرجع 0;

	إذا (str == "1" || str == "true")
		أرجع 1;

	حرف *endptr = خالي_اشارو;
	ڊگهو صحيح parsed_قدر = strtol(str.c_str(), &endptr, 10);
	أرجع *endptr ? default_قدر_2 : parsed_قدر;
}

منطقي RTLIL::Design::scratchpad_get_bool(ثابت std::string &varname, منطقي default_قدر_2) ثابت
{
	خودڪار it = scratchpad.ڳوليو(varname);
	إذا (it == scratchpad.پڇاڙي())
		أرجع default_قدر_2;

	ثابت std::string &str = it->ٻيو;

	إذا (str == "0" || str == "false")
		أرجع خطأ;

	إذا (str == "1" || str == "true")
		أرجع سچ;

	أرجع default_قدر_2;
}

std::string RTLIL::Design::scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_قدر_2) ثابت
{
	خودڪار it = scratchpad.ڳوليو(varname);
	إذا (it == scratchpad.پڇاڙي())
		أرجع default_قدر_2;

	أرجع it->ٻيو;
}

فراغ RTLIL::Design::remove(RTLIL::Module *module)
{
	لكل (خودڪار mon : monitors)
		mon->notify_module_del(module);

	إذا (yosys_xtrace) {
		لاگرٿم("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	ميٽيو module;
}

فراغ RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	جوڙيو(module);
}

فراغ RTLIL::Design::ترتيب_ڏيو()
{
	scratchpad.ترتيب_ڏيو();
	modules_.ترتيب_ڏيو(ترتيب_ڏيو_by_id_str(twines_));
	لكل (خودڪار &it : modules_)
		it.ٻيو->ترتيب_ڏيو();
}

فراغ RTLIL::Design::ترتيب_ڏيو_modules()
{
	scratchpad.ترتيب_ڏيو();
	modules_.ترتيب_ڏيو(ترتيب_ڏيو_by_id_str(twines_));
}

فراغ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

فراغ RTLIL::Design::check()
{
#جيڪڏهن_وصف_نه_ٿيل NDEBUG
	log_assert(!selection_ڍير.خالي_جايون());
	صحيح pool_سائيز = 0;
	لكل (خودڪار &it : modules_)
		pool_سائيز = std::max(pool_سائيز, ThreadPool::work_pool_سائيز(0, it.ٻيو->cells_سائيز(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_سائيز);
	لكل (خودڪار &it : modules_) {
		log_assert(هذا == it.ٻيو->design);
		log_assert(it.پهريون == it.ٻيو->name);
		check_module(it.ٻيو, thread_pool);
	}
#جيڪڏهن_پورو
}

فراغ RTLIL::Design::optimize()
{
	لكل (خودڪار &it : modules_)
		it.ٻيو->optimize();
	لكل (خودڪار &it : selection_ڍير)
		it.optimize(هذا);
	لكل (خودڪار &it : selection_vars)
		it.ٻيو.optimize(هذا);
}

فراغ RTLIL::Design::clone_into(RTLIL::Design *dst) ثابت
{
	log_assert(dst->modules_.خالي_جايون());
	dst->twines_ = twines_;
	لكل (خودڪار it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->ٻيو->clone(dst);
}

منطقي RTLIL::Design::selected_module(IdString mod_name) ثابت
{
	إذا (!selected_سرگرم_module.خالي_جايون() && mod_name != selected_سرگرم_module)
		أرجع خطأ;
	أرجع selection().selected_module(mod_name);
}

منطقي RTLIL::Design::selected_سڄو_module(RTLIL::IdString mod_name) ثابت
{
	إذا (!selected_سرگرم_module.خالي_جايون() && mod_name != selected_سرگرم_module)
		أرجع خطأ;
	أرجع selection().selected_سڄو_module(mod_name);
}

منطقي RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	إذا (!selected_سرگرم_module.خالي_جايون() && mod_name != selected_سرگرم_module)
		أرجع خطأ;
	أرجع selection().selected_member(mod_name, memb_name);
}

منطقي RTLIL::Design::selected_module(RTLIL::Module *mod) ثابت
{
	أرجع selected_module(mod->name);
}

منطقي RTLIL::Design::selected_سڄو_module(RTLIL::Module *mod) ثابت
{
	أرجع selected_سڄو_module(mod->name);
}

فراغ RTLIL::Design::ڌڪو_selection(RTLIL::Selection sel)
{
	sel.current_design = هذا;
	selection_ڍير.ڌڪو_پٺ(sel);
}

فراغ RTLIL::Design::ڌڪو_خالي_جايون_selection()
{
	ڌڪو_selection(RTLIL::Selection::خالي_جايون_Selection(هذا));
}

فراغ RTLIL::Design::ڌڪو_ڀريل_selection()
{
	ڌڪو_selection(RTLIL::Selection::ڀريل_Selection(هذا));
}

فراغ RTLIL::Design::ڌڪو_complete_selection()
{
	ڌڪو_selection(RTLIL::Selection::CompleteSelection(هذا));
}

فراغ RTLIL::Design::ڪڍو_selection()
{
	selection_ڍير.ڪڍو_پٺ();
	// Default to a full_selection if we ran out of stack
	إذا (selection_ڍير.خالي_جايون())
		ڌڪو_ڀريل_selection();
}

std::string RTLIL::Design::منزل_rtlil_str(منطقي only_selected) ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(هذا), only_selected, سچ, خطأ, RTLIL_BACKEND::Dump_گهڻائي::Readable);
	أرجع f.str();
}

std::ويڪٽر<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ثابت
{
	منطقي include_partials = partials == RTLIL::SELECT_ALL;
	منطقي exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	منطقي ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::ويڪٽر<RTLIL::Module*> نتيجو;
	نتيجو.reserve(modules_.سائيز());
	لكل (خودڪار &it : modules_) {
		إذا (selected_سڄو_module(it.پهريون) || (include_partials && selected_module(it.پهريون))) {
			إذا (!(exclude_boxes && it.ٻيو->get_blackbox_attribute(ignore_wb)))
				نتيجو.ڌڪو_پٺ(it.ٻيو);
			وإلا
				بدل (boxes)
				{
				حالة RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.ٻيو);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.ٻيو);
					اكسر;
				حالة RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.ٻيو);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.ٻيو);
					اكسر;
				حالة RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.ٻيو);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.ٻيو);
					اكسر;
				افتراضي:
					اكسر;
				}
		} وإلا إذا (!include_partials && selected_module(it.پهريون)) {
			بدل(partials)
			{
			حالة RTLIL::SELECT_سڄو_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.ٻيو);
				اكسر;
			حالة RTLIL::SELECT_سڄو_ERR:
				log_error("Unsupported partially selected module %s.\n", it.ٻيو);
				اكسر;
			حالة RTLIL::SELECT_سڄو_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.ٻيو);
				اكسر;
			افتراضي:
				اكسر;
			}
		}
	}
	أرجع نتيجو;
}

RTLIL::Module::Module()
{
	ساكن بنانشان صحيح hashidx_ڳڻپ = 123456789;
	hashidx_ڳڻپ = mkhash_xorshift(hashidx_ڳڻپ);
	hashidx_ = hashidx_ڳڻپ;

	design = خالي_اشارو;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->وجهو(std::pair<بنانشان صحيح, RTLIL::Module*>(hashidx_, هذا));
#جيڪڏهن_پورو
}

RTLIL::Module::~Module()
{
	لكل (خودڪار &pr : wires_)
		ميٽيو pr.ٻيو;
	لكل (خودڪار &pr : memories)
		ميٽيو pr.ٻيو;
	لكل (خودڪار &pr : cells_)
		ميٽيو pr.ٻيو;
	لكل (خودڪار &pr : processes)
		ميٽيو pr.ٻيو;
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
ساكن std::map<بنانشان صحيح, RTLIL::Module*> all_modules;
std::map<بنانشان صحيح, RTLIL::Module*> *RTLIL::Module::get_all_modules(فراغ)
{
	أرجع &all_modules;
}
#جيڪڏهن_پورو

فراغ RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	لكل (خودڪار it = wires_.begin(); it != wires_.پڇاڙي(); ++it)
		إذا (!it->ٻيو->پورٽ_داخلا && !it->ٻيو->پورٽ_output)
			delwires.وجهو(it->ٻيو);

	لكل (خودڪار it = memories.begin(); it != memories.پڇاڙي(); ++it)
		ميٽيو it->ٻيو;
	memories.clear();

	لكل (خودڪار it = cells_.begin(); it != cells_.پڇاڙي(); ++it)
		ميٽيو it->ٻيو;
	cells_.clear();

	لكل (خودڪار it = processes.begin(); it != processes.پڇاڙي(); ++it)
		ميٽيو it->ٻيو;
	processes.clear();

	connections_.clear();

	remove(delwires);
	سيٽ_bool_attribute(ID::blackbox);
}

فراغ RTLIL::Module::expand_interfaces(RTLIL::Design *, ثابت dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

منطقي RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	أرجع خطأ;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ثابت dict<RTLIL::IdString, RTLIL::Const> &, منطقي mayfail)
{
	إذا (mayfail)
		أرجع IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ثابت dict<RTLIL::IdString, RTLIL::Const> &, ثابت dict<RTLIL::IdString, RTLIL::Module*> &, ثابت dict<RTLIL::IdString, RTLIL::IdString> &, منطقي mayfail)
{
	إذا (mayfail)
		أرجع IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

سائيز_t RTLIL::Module::ڳڻپ_id(RTLIL::IdString id)
{
	أرجع wires_.ڳڻپ(id) + cells_.ڳڻپ(id) + memories.ڳڻپ(id) + processes.ڳڻپ(id);
}

#جيڪڏهن_وصف_نه_ٿيل NDEBUG
نطاق {
	بنية اندروني_Cell_Checker
	{
		ثابت RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		اندروني_Cell_Checker(ثابت RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		فراغ error(صحيح linenr, std::optional<std::string> error_نياپو = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			إذا (error_نياپو) {
				log_error("%s in internal cell %s\n", *error_نياپو, cell_loc);
			} وإلا {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		صحيح param(RTLIL::IdString name)
		{
			خودڪار it = cell->parameters.ڳوليو(name);
			إذا (it == cell->parameters.پڇاڙي()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.وجهو(name);
			أرجع it->ٻيو.as_int();
		}

		صحيح param_bool(RTLIL::IdString name)
		{
			صحيح v = param(name);
			إذا (Get_سائيز(cell->parameters.at(name)) > 32)
				error(__LINE__);
			إذا (v != 0 && v != 1)
				error(__LINE__);
			أرجع v;
		}

		صحيح param_bool(RTLIL::IdString name, منطقي expected)
		{
			صحيح v = param_bool(name);
			إذا (v != expected)
				error(__LINE__);
			أرجع v;
		}

		فراغ param_بٽ(RTLIL::IdString name, صحيح ويڪر)
		{
			param(name);
			إذا (Get_سائيز(cell->parameters.at(name)) != ويڪر)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			أرجع cell->parameters.at(name).decode_string();
		}

		فراغ پورٽ(RTLIL::IdString name, صحيح ويڪر)
		{
			خودڪار it = cell->connections_.ڳوليو(name);
			إذا (it == cell->connections_.پڇاڙي()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			إذا (Get_سائيز(it->ٻيو) != ويڪر) {
				std::string err = stringf("Expected port to have width %d", ويڪر);
				error(__LINE__, err);
			}
			expected_ports.وجهو(name);
		}

		فراغ check_expected(منطقي check_matched_sign = خطأ)
		{
			لكل (خودڪار &para : cell->parameters)
				إذا (expected_params.ڳڻپ(para.پهريون) == 0)
					error(__LINE__);
			لكل (خودڪار &conn : cell->connections())
				إذا (expected_ports.ڳڻپ(conn.پهريون) == 0)
					error(__LINE__);

			إذا (check_matched_sign) {
				log_assert(expected_params.ڳڻپ(ID::A_SIGNED) != 0 && expected_params.ڳڻپ(ID::B_SIGNED) != 0);
				منطقي a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				منطقي b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				إذا (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		فراغ check()
		{
			إذا (cell->type.is_عوامي() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				أرجع;

			إذا (cell->type.اندريون(ID($buf), ID($barrier))) {
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::Y, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.اندريون(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				پورٽ(ID::A, param(ID::A_ويڪر));
				پورٽ(ID::Y, param(ID::Y_ويڪر));
				check_expected();
				أرجع;
			}

			إذا (cell->type.اندريون(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٽ(ID::A, param(ID::A_ويڪر));
				پورٽ(ID::B, param(ID::B_ويڪر));
				پورٽ(ID::Y, param(ID::Y_ويڪر));
				check_expected(سچ);
				أرجع;
			}

			إذا (cell->type.اندريون(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				پورٽ(ID::A, param(ID::A_ويڪر));
				پورٽ(ID::Y, param(ID::Y_ويڪر));
				check_expected();
				أرجع;
			}

			إذا (cell->type.اندريون(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/خطأ);
				پورٽ(ID::A, param(ID::A_ويڪر));
				پورٽ(ID::B, param(ID::B_ويڪر));
				پورٽ(ID::Y, param(ID::Y_ويڪر));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type.اندريون(ID($سرڪايو), ID($shiftx))) {
				إذا (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/خطأ);
				} وإلا {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				پورٽ(ID::A, param(ID::A_ويڪر));
				پورٽ(ID::B, param(ID::B_ويڪر));
				پورٽ(ID::Y, param(ID::Y_ويڪر));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type.اندريون(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٽ(ID::A, param(ID::A_ويڪر));
				پورٽ(ID::B, param(ID::B_ويڪر));
				پورٽ(ID::Y, param(ID::Y_ويڪر));
				check_expected(سچ);
				أرجع;
			}

			إذا (cell->type.اندريون(ID($جوڙيو), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($قوت))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٽ(ID::A, param(ID::A_ويڪر));
				پورٽ(ID::B, param(ID::B_ويڪر));
				پورٽ(ID::Y, param(ID::Y_ويڪر));
				check_expected(cell->type != ID($قوت));
				أرجع;
			}

			إذا (cell->type == ID($fa)) {
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::B, param(ID::ويڪر_2));
				پورٽ(ID::C, param(ID::ويڪر_2));
				پورٽ(ID::X, param(ID::ويڪر_2));
				پورٽ(ID::Y, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($lcu)) {
				پورٽ(ID::P, param(ID::ويڪر_2));
				پورٽ(ID::G, param(ID::ويڪر_2));
				پورٽ(ID::CI, 1);
				پورٽ(ID::CO, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٽ(ID::A, param(ID::A_ويڪر));
				پورٽ(ID::B, param(ID::B_ويڪر));
				پورٽ(ID::CI, 1);
				پورٽ(ID::BI, 1);
				پورٽ(ID::X, param(ID::Y_ويڪر));
				پورٽ(ID::Y, param(ID::Y_ويڪر));
				پورٽ(ID::CO, param(ID::Y_ويڪر));
				check_expected(سچ);
				أرجع;
			}

			إذا (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_ويڪر);
				پورٽ(ID::A, param(ID::A_ويڪر));
				پورٽ(ID::B, param(ID::B_ويڪر));
				پورٽ(ID::Y, param(ID::Y_ويڪر));
				check_expected();
				Macc().from_cell(cell);
				أرجع;
			}

			إذا (cell->type == ID($macc_v2)) {
				إذا (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				إذا (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_بٽ(ID::حاصل_ضرب_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_بٽ(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_بٽ(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_بٽ(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_بٽ(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				إذا (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_بٽ(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_بٽ(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_بٽ(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ثابت Const &a_ويڪر = cell->getParam(ID::A_WIDTHS);
				ثابت Const &b_ويڪر = cell->getParam(ID::B_WIDTHS);
				ثابت Const &c_ويڪر = cell->getParam(ID::C_WIDTHS);
				صحيح a_ويڪر_جوڙ = 0, b_ويڪر_جوڙ = 0, c_ويڪر_جوڙ = 0;
				لكل (صحيح i = 0; i < param(ID::NPRODUCTS); i++) {
					a_ويڪر_جوڙ += a_ويڪر.ڪڍو_2(16 * i, 16).as_int(خطأ);
					b_ويڪر_جوڙ += b_ويڪر.ڪڍو_2(16 * i, 16).as_int(خطأ);
				}
				لكل (صحيح i = 0; i < param(ID::NADDENDS); i++) {
					c_ويڪر_جوڙ += c_ويڪر.ڪڍو_2(16 * i, 16).as_int(خطأ);
				}
				پورٽ(ID::A, a_ويڪر_جوڙ);
				پورٽ(ID::B, b_ويڪر_جوڙ);
				پورٽ(ID::C, c_ويڪر_جوڙ);
				پورٽ(ID::Y, param(ID::Y_ويڪر));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				پورٽ(ID::A, param(ID::A_ويڪر));
				پورٽ(ID::Y, param(ID::Y_ويڪر));
				check_expected();
				أرجع;
			}

			إذا (cell->type.اندريون(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٽ(ID::A, param(ID::A_ويڪر));
				پورٽ(ID::B, param(ID::B_ويڪر));
				پورٽ(ID::Y, param(ID::Y_ويڪر));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type == ID($ٽڪرو)) {
				param(ID::وٿي_2);
				پورٽ(ID::A, param(ID::A_ويڪر));
				پورٽ(ID::Y, param(ID::Y_ويڪر));
				إذا (param(ID::وٿي_2) + param(ID::Y_ويڪر) > param(ID::A_ويڪر))
					error(__LINE__);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($concat)) {
				پورٽ(ID::A, param(ID::A_ويڪر));
				پورٽ(ID::B, param(ID::B_ويڪر));
				پورٽ(ID::Y, param(ID::A_ويڪر) + param(ID::B_ويڪر));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mux)) {
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::B, param(ID::ويڪر_2));
				پورٽ(ID::S, 1);
				پورٽ(ID::Y, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($pmux)) {
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::B, param(ID::ويڪر_2) * param(ID::S_ويڪر));
				پورٽ(ID::S, param(ID::S_ويڪر));
				پورٽ(ID::Y, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bmux)) {
				پورٽ(ID::A, param(ID::ويڪر_2) << param(ID::S_ويڪر));
				پورٽ(ID::S, param(ID::S_ويڪر));
				پورٽ(ID::Y, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($demux)) {
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::S, param(ID::S_ويڪر));
				پورٽ(ID::Y, param(ID::ويڪر_2) << param(ID::S_ويڪر));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($lut)) {
				param(ID::LUT);
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sop)) {
				param(ID::اونهائي_2);
				param(ID::جدول);
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sr)) {
				param_bool(ID::سيٽ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورٽ(ID::سيٽ_2, param(ID::ويڪر_2));
				پورٽ(ID::CLR, param(ID::ويڪر_2));
				پورٽ(ID::Q,   param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($ff)) {
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				پورٽ(ID::CLK, 1);
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				پورٽ(ID::CLK, 1);
				پورٽ(ID::EN, 1);
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::سيٽ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورٽ(ID::CLK, 1);
				پورٽ(ID::سيٽ_2, param(ID::ويڪر_2));
				پورٽ(ID::CLR, param(ID::ويڪر_2));
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffsre)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::سيٽ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				param_bool(ID::EN_POLARITY);
				پورٽ(ID::CLK, 1);
				پورٽ(ID::EN, 1);
				پورٽ(ID::سيٽ_2, param(ID::ويڪر_2));
				پورٽ(ID::CLR, param(ID::ويڪر_2));
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بٽ(ID::ARST_قدر, param(ID::ويڪر_2));
				پورٽ(ID::CLK, 1);
				پورٽ(ID::ARST, 1);
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_بٽ(ID::SRST_قدر, param(ID::ويڪر_2));
				پورٽ(ID::CLK, 1);
				پورٽ(ID::SRST, 1);
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.اندريون(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_بٽ(ID::SRST_قدر, param(ID::ويڪر_2));
				پورٽ(ID::CLK, 1);
				پورٽ(ID::EN, 1);
				پورٽ(ID::SRST, 1);
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بٽ(ID::ARST_قدر, param(ID::ويڪر_2));
				پورٽ(ID::CLK, 1);
				پورٽ(ID::EN, 1);
				پورٽ(ID::ARST, 1);
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				پورٽ(ID::CLK, 1);
				پورٽ(ID::ALOAD, 1);
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::AD, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($aldffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				پورٽ(ID::CLK, 1);
				پورٽ(ID::EN, 1);
				پورٽ(ID::ALOAD, 1);
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::AD, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				پورٽ(ID::EN, 1);
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بٽ(ID::ARST_قدر, param(ID::ويڪر_2));
				پورٽ(ID::EN, 1);
				پورٽ(ID::ARST, 1);
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::سيٽ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورٽ(ID::EN, 1);
				پورٽ(ID::سيٽ_2, param(ID::ويڪر_2));
				پورٽ(ID::CLR, param(ID::ويڪر_2));
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_بٽ);
				param(ID::STATE_انگ);
				param(ID::STATE_انگ_LOG2);
				param(ID::STATE_RST);
				param_بٽ(ID::STATE_جدول, param(ID::STATE_بٽ) * param(ID::STATE_انگ));
				param(ID::TRANS_انگ);
				param_بٽ(ID::TRANS_جدول, param(ID::TRANS_انگ) * (2*param(ID::STATE_انگ_LOG2) + param(ID::CTRL_اندريون_ويڪر) + param(ID::CTRL_ٻاهريون_ويڪر)));
				پورٽ(ID::CLK, 1);
				پورٽ(ID::ARST, 1);
				پورٽ(ID::CTRL_اندريون, param(ID::CTRL_اندريون_ويڪر));
				پورٽ(ID::CTRL_ٻاهريون, param(ID::CTRL_ٻاهريون_ويڪر));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				پورٽ(ID::CLK, 1);
				پورٽ(ID::EN, 1);
				پورٽ(ID::ADDR, param(ID::ABITS));
				پورٽ(ID::معلومات_2, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_نقاب);
				param(ID::COLLISION_X_نقاب);
				param_bool(ID::CE_OVER_SRST);
				param_بٽ(ID::ARST_قدر, param(ID::ويڪر_2));
				param_بٽ(ID::SRST_قدر, param(ID::ويڪر_2));
				param_بٽ(ID::INIT_قدر, param(ID::ويڪر_2));
				پورٽ(ID::CLK, 1);
				پورٽ(ID::EN, 1);
				پورٽ(ID::ARST, 1);
				پورٽ(ID::SRST, 1);
				پورٽ(ID::ADDR, param(ID::ABITS));
				پورٽ(ID::معلومات_2, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::ترجيح);
				پورٽ(ID::CLK, 1);
				پورٽ(ID::EN, param(ID::ويڪر_2));
				پورٽ(ID::ADDR, param(ID::ABITS));
				پورٽ(ID::معلومات_2, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memwr_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PORTID);
				param(ID::ترجيح_نقاب_2);
				پورٽ(ID::CLK, 1);
				پورٽ(ID::EN, param(ID::ويڪر_2));
				پورٽ(ID::ADDR, param(ID::ABITS));
				پورٽ(ID::معلومات_2, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::ترجيح);
				پورٽ(ID::ADDR, param(ID::ABITS));
				پورٽ(ID::معلومات_2, param(ID::ويڪر_2) * param(ID::لفظ_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::ترجيح);
				پورٽ(ID::ADDR, param(ID::ABITS));
				پورٽ(ID::معلومات_2, param(ID::ويڪر_2) * param(ID::لفظ_2));
				پورٽ(ID::EN, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mem)) {
				param(ID::MEMID);
				param(ID::سائيز_2);
				param(ID::وٿي_2);
				param(ID::INIT);
				param_بٽ(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_بٽ(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_بٽ(ID::RD_TRANSPARENT, max(1, param(ID::RD_PORTS)));
				param_بٽ(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_بٽ(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				پورٽ(ID::RD_CLK, param(ID::RD_PORTS));
				پورٽ(ID::RD_EN, param(ID::RD_PORTS));
				پورٽ(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				پورٽ(ID::RD_معلومات, param(ID::RD_PORTS) * param(ID::ويڪر_2));
				پورٽ(ID::WR_CLK, param(ID::WR_PORTS));
				پورٽ(ID::WR_EN, param(ID::WR_PORTS) * param(ID::ويڪر_2));
				پورٽ(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				پورٽ(ID::WR_معلومات, param(ID::WR_PORTS) * param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mem_v2)) {
				param(ID::MEMID);
				param(ID::سائيز_2);
				param(ID::وٿي_2);
				param(ID::INIT);
				param_بٽ(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_بٽ(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_بٽ(ID::RD_TRANSPARENCY_نقاب, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_بٽ(ID::RD_COLLISION_X_نقاب, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_بٽ(ID::RD_WIDE_CONTINUATION, max(1, param(ID::RD_PORTS)));
				param_بٽ(ID::RD_CE_OVER_SRST, max(1, param(ID::RD_PORTS)));
				param_بٽ(ID::RD_ARST_قدر, max(1, param(ID::RD_PORTS) * param(ID::ويڪر_2)));
				param_بٽ(ID::RD_SRST_قدر, max(1, param(ID::RD_PORTS) * param(ID::ويڪر_2)));
				param_بٽ(ID::RD_INIT_قدر, max(1, param(ID::RD_PORTS) * param(ID::ويڪر_2)));
				param_بٽ(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_بٽ(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				param_بٽ(ID::WR_WIDE_CONTINUATION, max(1, param(ID::WR_PORTS)));
				param_بٽ(ID::WR_ترجيح_نقاب, max(1, param(ID::WR_PORTS) * param(ID::WR_PORTS)));
				پورٽ(ID::RD_CLK, param(ID::RD_PORTS));
				پورٽ(ID::RD_EN, param(ID::RD_PORTS));
				پورٽ(ID::RD_ARST, param(ID::RD_PORTS));
				پورٽ(ID::RD_SRST, param(ID::RD_PORTS));
				پورٽ(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				پورٽ(ID::RD_معلومات, param(ID::RD_PORTS) * param(ID::ويڪر_2));
				پورٽ(ID::WR_CLK, param(ID::WR_PORTS));
				پورٽ(ID::WR_EN, param(ID::WR_PORTS) * param(ID::ويڪر_2));
				پورٽ(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				پورٽ(ID::WR_معلومات, param(ID::WR_PORTS) * param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($tribuf)) {
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::Y, param(ID::ويڪر_2));
				پورٽ(ID::EN, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bweqx)) {
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::B, param(ID::ويڪر_2));
				پورٽ(ID::Y, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bwmux)) {
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::B, param(ID::ويڪر_2));
				پورٽ(ID::S, param(ID::ويڪر_2));
				پورٽ(ID::Y, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.اندريون(ID($أكد), ID($assume), ID($جيئرو), ID($fair), ID($cover))) {
				پورٽ(ID::A, 1);
				پورٽ(ID::EN, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($initstate)) {
				پورٽ(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type.اندريون(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				پورٽ(ID::Y, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.اندريون(ID($anyinit))) {
				پورٽ(ID::D, param(ID::ويڪر_2));
				پورٽ(ID::Q, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($equiv)) {
				پورٽ(ID::A, 1);
				پورٽ(ID::B, 1);
				پورٽ(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type.اندريون(ID($specify2), ID($specify3))) {
				param_bool(ID::ڀريل);
				param_bool(ID::SRC_DST_PEN);
				param_bool(ID::SRC_DST_POL);
				param(ID::T_RISE_MIN);
				param(ID::T_RISE_TYP);
				param(ID::T_RISE_MAX);
				param(ID::T_FALL_MIN);
				param(ID::T_FALL_TYP);
				param(ID::T_FALL_MAX);
				پورٽ(ID::EN, 1);
				پورٽ(ID::SRC, param(ID::SRC_ويڪر));
				پورٽ(ID::DST, param(ID::DST_ويڪر));
				إذا (cell->type == ID($specify3)) {
					param_bool(ID::ڪنارو_EN);
					param_bool(ID::ڪنارو_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					پورٽ(ID::DAT, param(ID::DST_ويڪر));
				}
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($specrule)) {
				param(ID::TYPE);
				param_bool(ID::SRC_PEN);
				param_bool(ID::SRC_POL);
				param_bool(ID::DST_PEN);
				param_bool(ID::DST_POL);
				param(ID::T_حد_بندي_MIN);
				param(ID::T_حد_بندي_TYP);
				param(ID::T_حد_بندي_MAX);
				param(ID::T_LIMIT2_MIN);
				param(ID::T_LIMIT2_TYP);
				param(ID::T_LIMIT2_MAX);
				پورٽ(ID::SRC_EN, 1);
				پورٽ(ID::DST_EN, 1);
				پورٽ(ID::SRC, param(ID::SRC_ويڪر));
				پورٽ(ID::DST, param(ID::DST_ويڪر));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::ترجيح);
				پورٽ(ID::EN, 1);
				پورٽ(ID::TRG, param(ID::TRG_ويڪر));
				پورٽ(ID::دليلون, param(ID::دليلون_ويڪر));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				إذا (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
					error(__LINE__);
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::ترجيح);
				پورٽ(ID::A, 1);
				پورٽ(ID::EN, 1);
				پورٽ(ID::TRG, param(ID::TRG_ويڪر));
				پورٽ(ID::دليلون, param(ID::دليلون_ويڪر));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				إذا (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				أرجع;
			}

			إذا (cell->type == ID($_BUF_))    { پورٽ(ID::A,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NOT_))    { پورٽ(ID::A,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AND_))    { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NAND_))   { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OR_))     { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NOR_))    { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_XOR_))    { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_XNOR_))   { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_ANDNOT_)) { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_ORNOT_))  { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX_))    { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::S,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NMUX_))   { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::S,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AOI3_))   { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::C,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OAI3_))   { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::C,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AOI4_))   { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::C,1); پورٽ(ID::D,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OAI4_))   { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::C,1); پورٽ(ID::D,1); پورٽ(ID::Y,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_TBUF_))  { پورٽ(ID::A,1); پورٽ(ID::Y,1); پورٽ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_MUX4_))  { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::C,1); پورٽ(ID::D,1); پورٽ(ID::S,1); پورٽ(ID::T,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX8_))  { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::C,1); پورٽ(ID::D,1); پورٽ(ID::E,1); پورٽ(ID::F,1); پورٽ(ID::G,1); پورٽ(ID::H,1); پورٽ(ID::S,1); پورٽ(ID::T,1); پورٽ(ID::U,1); پورٽ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX16_)) { پورٽ(ID::A,1); پورٽ(ID::B,1); پورٽ(ID::C,1); پورٽ(ID::D,1); پورٽ(ID::E,1); پورٽ(ID::F,1); پورٽ(ID::G,1); پورٽ(ID::H,1); پورٽ(ID::I,1); پورٽ(ID::J,1); پورٽ(ID::K,1); پورٽ(ID::L,1); پورٽ(ID::M,1); پورٽ(ID::N,1); پورٽ(ID::O,1); پورٽ(ID::P,1); پورٽ(ID::S,1); پورٽ(ID::T,1); پورٽ(ID::U,1); پورٽ(ID::V,1); پورٽ(ID::Y,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ پورٽ(ID::S,1); پورٽ(ID::R,1); پورٽ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_FF_)) { پورٽ(ID::D,1); پورٽ(ID::Q,1); check_expected();  أرجع; }

			إذا (cell->type.اندريون(ID($_DFF_N_), ID($_DFF_P_)))
				{ پورٽ(ID::D,1); پورٽ(ID::Q,1); پورٽ(ID::C,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ پورٽ(ID::D,1); پورٽ(ID::Q,1); پورٽ(ID::C,1); پورٽ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ پورٽ(ID::D,1); پورٽ(ID::Q,1); پورٽ(ID::C,1); پورٽ(ID::R,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ پورٽ(ID::D,1); پورٽ(ID::Q,1); پورٽ(ID::C,1); پورٽ(ID::R,1); پورٽ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ پورٽ(ID::D,1); پورٽ(ID::Q,1); پورٽ(ID::C,1); پورٽ(ID::L,1); پورٽ(ID::AD,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ پورٽ(ID::D,1); پورٽ(ID::Q,1); پورٽ(ID::C,1); پورٽ(ID::L,1); پورٽ(ID::AD,1); پورٽ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ پورٽ(ID::C,1); پورٽ(ID::S,1); پورٽ(ID::R,1); پورٽ(ID::D,1); پورٽ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ پورٽ(ID::C,1); پورٽ(ID::S,1); پورٽ(ID::R,1); پورٽ(ID::D,1); پورٽ(ID::E,1); پورٽ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ پورٽ(ID::D,1); پورٽ(ID::Q,1); پورٽ(ID::C,1); پورٽ(ID::R,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ پورٽ(ID::D,1); پورٽ(ID::Q,1); پورٽ(ID::C,1); پورٽ(ID::R,1); پورٽ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ پورٽ(ID::E,1); پورٽ(ID::D,1); پورٽ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ پورٽ(ID::E,1); پورٽ(ID::R,1); پورٽ(ID::D,1); پورٽ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ پورٽ(ID::E,1); پورٽ(ID::S,1); پورٽ(ID::R,1); پورٽ(ID::D,1); پورٽ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.اندريون(ID($سيٽ_ٽيگ))) {
				param(ID::ويڪر_2);
				param(ID::ٽيگ_2);
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::سيٽ_2, param(ID::ويڪر_2));
				پورٽ(ID::CLR, param(ID::ويڪر_2));
				پورٽ(ID::Y, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.اندريون(ID($get_ٽيگ),ID($original_ٽيگ))) {
				param(ID::ويڪر_2);
				param(ID::ٽيگ_2);
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::Y, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.اندريون(ID($overwrite_ٽيگ))) {
				param(ID::ويڪر_2);
				param(ID::ٽيگ_2);
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::سيٽ_2, param(ID::ويڪر_2));
				پورٽ(ID::CLR, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.اندريون(ID($future_ff))) {
				param(ID::ويڪر_2);
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::Y, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.اندريون(ID($داخلا_پورٽ))) {
				param(ID::ويڪر_2);
				پورٽ(ID::Y, param(ID::ويڪر_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.اندريون(ID($connect))) {
				param(ID::ويڪر_2);
				پورٽ(ID::A, param(ID::ويڪر_2));
				پورٽ(ID::B, param(ID::ويڪر_2));
				check_expected();
				أرجع;
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
#جيڪڏهن_پورو

فراغ RTLIL::Module::ترتيب_ڏيو()
{
	wires_.ترتيب_ڏيو(ترتيب_ڏيو_by_id_str(design->twines()));
	cells_.ترتيب_ڏيو(ترتيب_ڏيو_by_id_str(design->twines()));
	parameter_default_قدرون.ترتيب_ڏيو(ترتيب_ڏيو_by_id_str(design->twines()));
	memories.ترتيب_ڏيو(ترتيب_ڏيو_by_id_str(design->twines()));
	processes.ترتيب_ڏيو(ترتيب_ڏيو_by_id_str(design->twines()));
	لكل (خودڪار &it : cells_)
		it.ٻيو->ترتيب_ڏيو();
	لكل (خودڪار &it : wires_)
		it.ٻيو->خاصيتون.ترتيب_ڏيو(ترتيب_ڏيو_by_id_str(design->twines()));
	لكل (خودڪار &it : memories)
		it.ٻيو->خاصيتون.ترتيب_ڏيو(ترتيب_ڏيو_by_id_str(design->twines()));
}

#جيڪڏهن_وصف_نه_ٿيل NDEBUG
ساكن فراغ check_id_ڪنجيون(ثابت dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	لكل (خودڪار &it : keyed)
		log_assert(!it.پهريون.خالي_جايون());
}
#جيڪڏهن_پورو

فراغ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#جيڪڏهن_وصف_نه_ٿيل NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_سائيز(0, module->cells_سائيز(), 1000));
	ثابت RTLIL::Module *const_module = module;

	pool<std::string> يادگيري_لڙيون;
	لكل (خودڪار &it : module->memories) {
		log_assert(it.پهريون == it.ٻيو->name);
		log_assert(it.پهريون != IdString::Null);
		log_assert(it.ٻيو->ويڪر >= 0);
		log_assert(it.ٻيو->سائيز >= 0);
		check_id_ڪنجيون(it.ٻيو->خاصيتون);
		يادگيري_لڙيون.وجهو(it.ٻيو->name.str());
	}

	std::ويڪٽر<MonotonicFlag> ports_declared(Get_سائيز(module->ports));
	Sharded_ويڪٽر<std::string> memids(subpool);
	subpool.لاڳيتو([const_module, &ports_declared, &يادگيري_لڙيون, &memids](ثابت ParallelDispatchThreadPool::لاڳيتو_Ctx &ctx) {
		لكل (صحيح i : ctx.شي_range(const_module->cells_سائيز())) {
			خودڪار it = *const_module->cells_.element(i);
			log_assert(const_module == it.ٻيو->module);
			log_assert(it.پهريون == it.ٻيو->name);
			log_assert(it.پهريون != IdString::Null);
			log_assert(!it.ٻيو->type.خالي_جايون());
			لكل (خودڪار &it2 : it.ٻيو->connections()) {
				log_assert(it2.پهريون != IdString::Null);
				it2.ٻيو.check(const_module);
			}
			check_id_ڪنجيون(it.ٻيو->خاصيتون);
			check_id_ڪنجيون(it.ٻيو->parameters);
			اندروني_Cell_Checker checker(const_module, it.ٻيو);
			checker.check();
			إذا (it.ٻيو->has_memid()) {
				log_assert(يادگيري_لڙيون.ڳڻپ(it.ٻيو->parameters.at(ID::MEMID).decode_string()));
			} وإلا إذا (it.ٻيو->is_mem_cell()) {
				std::string memid = it.ٻيو->parameters.at(ID::MEMID).decode_string();
				log_assert(!يادگيري_لڙيون.ڳڻپ(memid));
				memids.وجهو(ctx, std::move(memid));
			}
			خودڪار cell_mod = const_module->design->module(it.ٻيو->name);
			إذا (cell_mod != خالي_اشارو) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#جيڪڏهن_وصف_ٿيل __GNUC__
				#هدايت GCC diagnostic ڌڪو
				#هدايت GCC diagnostic ignored "-Wdeprecated-declarations"
				#جيڪڏهن_پورو
				log_assert(!it.ٻيو->get_blackbox_attribute());
				#جيڪڏهن_وصف_ٿيل __GNUC__
				#هدايت GCC diagnostic ڪڍو
				#جيڪڏهن_پورو
			}
		}

		لكل (صحيح i : ctx.شي_range(const_module->wires_سائيز())) {
			خودڪار it = *const_module->wires_.element(i);
			log_assert(const_module == it.ٻيو->module);
			log_assert(it.پهريون == it.ٻيو->name);
			log_assert(it.پهريون != IdString::Null);
			log_assert(it.ٻيو->ويڪر >= 0);
			log_assert(it.ٻيو->پورٽ_id >= 0);
			check_id_ڪنجيون(it.ٻيو->خاصيتون);
			إذا (it.ٻيو->پورٽ_id) {
				log_assert(Get_سائيز(const_module->ports) >= it.ٻيو->پورٽ_id);
				log_assert(const_module->ports.at(it.ٻيو->پورٽ_id-1) == it.ٻيو->name);
				log_assert(it.ٻيو->پورٽ_داخلا || it.ٻيو->پورٽ_output);
				log_assert(it.ٻيو->پورٽ_id <= Get_سائيز(ports_declared));
				منطقي previously_declared = ports_declared[it.ٻيو->پورٽ_id-1].سيٽ_and_return_old();
				log_assert(previously_declared == خطأ);
			} وإلا
				log_assert(!it.ٻيو->پورٽ_داخلا && !it.ٻيو->پورٽ_output);
		}
	});
	لكل (ثابت MonotonicFlag &پورٽ_declared : ports_declared)
		log_assert(پورٽ_declared.ڀريو() == سچ);
	pool<std::string> memids_pool;
	لكل (std::string &memid : memids)
		log_assert(memids_pool.وجهو(memid).ٻيو);

	لكل (خودڪار &it : module->processes) {
		log_assert(it.پهريون == it.ٻيو->name);
		log_assert(it.پهريون != IdString::Null);
		log_assert(it.ٻيو->پاڙ_case.compare.خالي_جايون());
		std::ويڪٽر<RTLIL::Case_قاعدو*> all_cases = {&it.ٻيو->پاڙ_case};
		لكل (سائيز_t i = 0; i < all_cases.سائيز(); i++) {
			لكل (خودڪار &switch_it : all_cases[i]->switches) {
				لكل (خودڪار &case_it : switch_it->cases) {
					لكل (خودڪار &compare_it : case_it->compare) {
						log_assert(switch_it->إشارة.سائيز() == compare_it.سائيز());
					}
					all_cases.ڌڪو_پٺ(case_it);
				}
			}
		}
		لكل (خودڪار &sync_it : it.ٻيو->syncs) {
			بدل (sync_it->type) {
				حالة RTLIL::SyncType::ST0:
				حالة RTLIL::SyncType::ST1:
				حالة RTLIL::SyncType::STp:
				حالة RTLIL::SyncType::STn:
				حالة RTLIL::SyncType::STe:
					log_assert(!sync_it->إشارة.خالي_جايون());
					اكسر;
				حالة RTLIL::SyncType::STa:
				حالة RTLIL::SyncType::STg:
				حالة RTLIL::SyncType::STi:
					log_assert(sync_it->إشارة.خالي_جايون());
					اكسر;
			}
		}
	}

	لكل (خودڪار &it : module->connections_) {
		log_assert(it.پهريون.سائيز() == it.ٻيو.سائيز());
		log_assert(!it.پهريون.has_const());
		it.پهريون.check(module);
		it.ٻيو.check(module);
	}

	check_id_ڪنجيون(module->خاصيتون);
#جيڪڏهن_پورو
}

فراغ RTLIL::Module::check()
{
	صحيح pool_سائيز = ThreadPool::work_pool_سائيز(0, cells_سائيز(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_سائيز);
	check_module(هذا, thread_pool);
}

فراغ RTLIL::Module::optimize()
{
}

فراغ RTLIL::Module::cloneInto(RTLIL::Module *new_mod) ثابت
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	لكل (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_قدرون.clear();
	لكل (خودڪار &it : parameter_default_قدرون)
		new_mod->parameter_default_قدرون[dst_twines.copy_from(design->twines(), it.پهريون)] = it.ٻيو;

	لكل (خودڪار &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->خاصيتون, خاصيتون, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	لكل (خودڪار it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->پهريون);
		wire_map[it->ٻيو] = new_mod->جوڙيو_Wire(dst_id, it->ٻيو);
	}

	لكل (خودڪار it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->پهريون);
		new_mod->جوڙيو_يادگيري(dst_id, it->ٻيو);
	}

	لكل (خودڪار it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->پهريون);
		new_mod->جوڙيو_Cell(dst_id, it->ٻيو);
	}

	لكل (خودڪار it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->پهريون);
		new_mod->جوڙيو_Process(dst_id, it->ٻيو);
	}

	بنية Rewrite_Sig_Spec_ڪم_ڪندڙ
	{
		ثابت dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ثابت RTLIL::Module *new_mod;
		فراغ عامل()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([هذا](RTLIL::Wire *&wire) {
				خودڪار it = wire_map.ڳوليو(wire);
				إذا (it != wire_map.پڇاڙي())
					wire = it->ٻيو;
				وإلا
					log_assert(wire->module == new_mod);
			});
		}
	};

	Rewrite_Sig_Spec_ڪم_ڪندڙ rewrite_Sig_Spec_ڪم_ڪندڙ{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewrite_Sig_Spec_ڪم_ڪندڙ);
	new_mod->fixup_ports();
}

RTLIL::Module *RTLIL::Module::clone() ثابت
{
	RTLIL::Module *new_mod = جديد RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	أرجع new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) ثابت
{
	RTLIL::Module *new_mod = جديد RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->جوڙيو(new_mod);
	أرجع new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString هدف_name) ثابت
{
	RTLIL::Module *new_mod = جديد RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = هدف_name;
	cloneInto(new_mod);
	dst->جوڙيو(new_mod);
	أرجع new_mod;
}

منطقي RTLIL::Module::has_memories() ثابت
{
	أرجع !memories.خالي_جايون();
}

منطقي RTLIL::Module::has_processes() ثابت
{
	أرجع !processes.خالي_جايون();
}

منطقي RTLIL::Module::has_memories_warn() ثابت
{
	إذا (!memories.خالي_جايون())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", هذا);
	أرجع !memories.خالي_جايون();
}

منطقي RTLIL::Module::has_processes_warn() ثابت
{
	إذا (!processes.خالي_جايون())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", هذا);
	أرجع !processes.خالي_جايون();
}

منطقي RTLIL::Module::is_selected() ثابت
{
	أرجع design->selected_module(هذا->name);
}

منطقي RTLIL::Module::is_selected_سڄو() ثابت
{
	أرجع design->selected_سڄو_module(هذا->name);
}

std::ويڪٽر<RTLIL::Wire*> RTLIL::Module::selected_wires() ثابت
{
	std::ويڪٽر<RTLIL::Wire*> نتيجو;
	نتيجو.reserve(wires_.سائيز());
	لكل (خودڪار &it : wires_)
		إذا (design->selected(هذا, it.ٻيو))
			نتيجو.ڌڪو_پٺ(it.ٻيو);
	أرجع نتيجو;
}

std::ويڪٽر<RTLIL::Cell*> RTLIL::Module::selected_cells() ثابت
{
	std::ويڪٽر<RTLIL::Cell*> نتيجو;
	نتيجو.reserve(cells_.سائيز());
	لكل (خودڪار &it : cells_)
		إذا (design->selected(هذا, it.ٻيو))
			نتيجو.ڌڪو_پٺ(it.ٻيو);
	أرجع نتيجو;
}

std::ويڪٽر<RTLIL::يادگيري*> RTLIL::Module::selected_memories() ثابت
{
	std::ويڪٽر<RTLIL::يادگيري*> نتيجو;
	نتيجو.reserve(memories.سائيز());
	لكل (خودڪار &it : memories)
		إذا (design->selected(هذا, it.ٻيو))
			نتيجو.ڌڪو_پٺ(it.ٻيو);
	أرجع نتيجو;
}

std::ويڪٽر<RTLIL::Process*> RTLIL::Module::selected_processes() ثابت
{
	std::ويڪٽر<RTLIL::Process*> نتيجو;
	نتيجو.reserve(processes.سائيز());
	لكل (خودڪار &it : processes)
		إذا (design->selected(هذا, it.ٻيو))
			نتيجو.ڌڪو_پٺ(it.ٻيو);
	أرجع نتيجو;
}

std::ويڪٽر<RTLIL::SelectedMember> RTLIL::Module::selected_members() ثابت
{
	std::ويڪٽر<RTLIL::SelectedMember> نتيجو;
	لكل (خودڪار cell : selected_cells())
		نتيجو.ڌڪو_پٺ({cell, cell->name});
	لكل (خودڪار يادگيري_3 : selected_memories())
		نتيجو.ڌڪو_پٺ({يادگيري_3, يادگيري_3->name});
	لكل (خودڪار wire : selected_wires())
		نتيجو.ڌڪو_پٺ({wire, wire->name});
	لكل (خودڪار process : selected_processes())
		نتيجو.ڌڪو_پٺ({process, process->name});
	أرجع نتيجو;
}

فراغ RTLIL::Module::جوڙيو(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(ڳڻپ_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = هذا;
}

فراغ RTLIL::Module::جوڙيو(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(ڳڻپ_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = هذا;
}

فراغ RTLIL::Module::جوڙيو(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(ڳڻپ_id(process->name) == 0);
	processes[process->name] = process;
	process->module = هذا;
}

فراغ RTLIL::Module::remove(ثابت pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	إذا (wires.خالي_جايون())
		أرجع;

	بنية Delete_Wire_ڪم_ڪندڙ
	{
		RTLIL::Module *module;
		ثابت pool<RTLIL::Wire*> *wires_p;

		فراغ عامل()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([هذا](RTLIL::Wire *&wire) {
				إذا (wires_p->ڳڻپ(wire))
					wire = module->جوڙيو_Wire(stringf("$delete_wire$%d", (صحيح)autoidx++), wire->ويڪر);
			});
		}

		فراغ عامل()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	Delete_Wire_ڪم_ڪندڙ delete_wire_ڪم_ڪندڙ;
	delete_wire_ڪم_ڪندڙ.module = هذا;
	delete_wire_ڪم_ڪندڙ.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_ڪم_ڪندڙ);

	إذا (design->flagBufferedNormalized) {
		لكل (خودڪار wire : wires) {
			buf_norm_wire_قطار.erase(wire);
			buf_norm_connect_اشاريو.erase(wire);
		}
	}

	لكل (خودڪار &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.ڳڻپ(id) != 0);
		wires_.erase(id);
		ميٽيو it;
	}
}

فراغ RTLIL::Module::remove(RTLIL::Cell *cell)
{
	طالما (!cell->connections_.خالي_جايون())
		cell->unset_پورٽ(cell->connections_.begin()->پهريون);

	log_assert(cells_.ڳڻپ(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	إذا (design && design->flagBufferedNormalized && buf_norm_cell_قطار.ڳڻپ(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.وجهو(cell);
	} وإلا {
		ميٽيو cell;
	}
}

فراغ RTLIL::Module::remove(RTLIL::يادگيري *يادگيري_3)
{
	log_assert(memories.ڳڻپ(يادگيري_3->name) != 0);
	memories.erase(يادگيري_3->name);
	ميٽيو يادگيري_3;
}

فراغ RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.ڳڻپ(process->name) != 0);
	processes.erase(process->name);
	ميٽيو process;
}

فراغ RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	جوڙيو(wire);
}

فراغ RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	جوڙيو(cell);
}

فراغ RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	إذا (old_id != IdString::Null && wires_.ڳڻپ(old_id))
		rename(wires_.at(old_id), new_name);
	وإلا إذا (old_id != IdString::Null && cells_.ڳڻپ(old_id))
		rename(cells_.at(old_id), new_name);
	وإلا
		log_abort();
}

فراغ RTLIL::Module::مٽا_سٽا_نالا(RTLIL::Wire *w1, RTLIL::Wire *w2)
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
	مٽا_سٽا(w1->name, w2->name);
}

فراغ RTLIL::Module::مٽا_سٽا_نالا(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	مٽا_سٽا(c1->name, c2->name);
}

RTLIL::IdString RTLIL::Module::uniquify(RTLIL::IdString name)
{
	صحيح اشاريو = 0;
	أرجع uniquify(name, اشاريو);
}

IdString RTLIL::Module::uniquify(IdString name, صحيح &اشاريو)
{
	إذا (اشاريو == 0) {
		إذا (ڳڻپ_id(name) == 0)
			أرجع name;
		اشاريو++;
	}

	طالما (1) {
		IdString new_name = design->twines().جوڙيو(TwineSpec::Suffix{name, stringf("_%d", اشاريو)});
		إذا (ڳڻپ_id(new_name) == 0)
			أرجع new_name;
		اشاريو++;
	}
}

ساكن منطقي fixup_ports_compare(ثابت RTLIL::Wire *a, ثابت RTLIL::Wire *b)
{
	إذا (a->پورٽ_id && !b->پورٽ_id)
		أرجع سچ;
	إذا (!a->پورٽ_id && b->پورٽ_id)
		أرجع خطأ;

	إذا (a->پورٽ_id == b->پورٽ_id)
		أرجع a->name < b->name;
	أرجع a->پورٽ_id < b->پورٽ_id;
}

فراغ RTLIL::Module::connect(ثابت RTLIL::SigSig &conn)
{
	لكل (خودڪار mon : monitors)
		mon->notify_connect(هذا, conn);

	إذا (design)
		لكل (خودڪار mon : design->monitors)
			mon->notify_connect(هذا, conn);

	// ignore all attempts to assign constants to other constants
	إذا (conn.پهريون.has_const()) {
		RTLIL::SigSig new_conn;
		لكل (صحيح i = 0; i < Get_سائيز(conn.پهريون); i++)
			إذا (conn.پهريون[i].wire) {
				new_conn.پهريون.append(conn.پهريون[i]);
				new_conn.ٻيو.append(conn.ٻيو[i]);
			}
		إذا (Get_سائيز(new_conn.پهريون))
			connect(new_conn);
		أرجع;
	}

	إذا (yosys_xtrace) {
		لاگرٿم("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", هذا, log_signal(conn.پهريون), log_signal(conn.ٻيو), Get_سائيز(conn.پهريون));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(Get_سائيز(conn.پهريون) == Get_سائيز(conn.ٻيو));
	connections_.ڌڪو_پٺ(conn);
}

فراغ RTLIL::Module::connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

فراغ RTLIL::Module::new_connections(ثابت std::ويڪٽر<RTLIL::SigSig> &new_conn)
{
	لكل (خودڪار mon : monitors)
		mon->notify_connect(هذا, new_conn);

	إذا (design)
		لكل (خودڪار mon : design->monitors)
			mon->notify_connect(هذا, new_conn);

	إذا (yosys_xtrace) {
		لاگرٿم("#X# New connections vector in %s:\n", هذا);
		لكل (خودڪار &conn: new_conn)
			لاگرٿم("#X#    %s = %s (%d bits)\n", log_signal(conn.پهريون), log_signal(conn.ٻيو), Get_سائيز(conn.پهريون));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ثابت std::ويڪٽر<RTLIL::SigSig> &RTLIL::Module::connections() ثابت
{
	أرجع connections_;
}

فراغ RTLIL::Module::fixup_ports()
{
	std::ويڪٽر<RTLIL::Wire*> all_ports;

	لكل (خودڪار &w : wires_)
		إذا (w.ٻيو->پورٽ_داخلا || w.ٻيو->پورٽ_output)
			all_ports.ڌڪو_پٺ(w.ٻيو);
		وإلا
			w.ٻيو->پورٽ_id = 0;

	std::ترتيب_ڏيو(all_ports.begin(), all_ports.پڇاڙي(), fixup_ports_compare);

	إذا (design && design->flagBufferedNormalized) {
		لكل (خودڪار &w : wires_)
			إذا (w.ٻيو->driverCell_ && w.ٻيو->driverCell_->type == ID($داخلا_پورٽ))
				buf_norm_wire_قطار.وجهو(w.ٻيو);

		buf_norm_wire_قطار.وجهو(all_ports.begin(), all_ports.پڇاڙي());
	}

	ports.clear();
	لكل (سائيز_t i = 0; i < all_ports.سائيز(); i++) {
		ports.ڌڪو_پٺ(all_ports[i]->name);
		all_ports[i]->پورٽ_id = i+1;
	}
}

فراغ RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		ثابت dict<IdString, RTLIL::Const> &src,
		ثابت RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	إذا (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		أرجع;
	}
	dst.clear();
	لكل (صحيح i = Get_سائيز(src) - 1; i >= 0; i--) {
		خودڪار &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.پهريون)] = it.ٻيو;
	}
}

RTLIL::Wire *RTLIL::Module::جوڙيو_Wire(RTLIL::IdString name, صحيح ويڪر)
{
	log_assert(design);
	log_assert(ويڪر >= 0 && ويڪر < RTLIL::ويڪر_حد_بندي);
	RTLIL::Wire *wire = جديد RTLIL::Wire;
	wire->ويڪر = ويڪر;
	wire->name = name;
	جوڙيو(wire);
	أرجع wire;
}

RTLIL::Wire *RTLIL::Module::جوڙيو_Wire(IdString name, ثابت RTLIL::Wire *other)
{
	RTLIL::Wire *wire = جوڙيو_Wire(name);
	wire->ويڪر = other->ويڪر;
	wire->شروعات_وٿي = other->شروعات_وٿي;
	wire->پورٽ_id = other->پورٽ_id;
	wire->پورٽ_داخلا = other->پورٽ_داخلا;
	wire->پورٽ_output = other->پورٽ_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ثابت RTLIL::Design *src_design = other->module ? other->module->design : خالي_اشارو;
		RTLIL::copy_attr_dict(wire->خاصيتون, other->خاصيتون, src_design, هذا->design);
	}
	أرجع wire;
}

RTLIL::Cell *RTLIL::Module::جوڙيو_Cell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = جديد RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	جوڙيو(cell);
	أرجع cell;
}

RTLIL::Cell *RTLIL::Module::جوڙيو_Cell(IdString name, ثابت RTLIL::Cell *other)
{
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : خالي_اشارو;
	منطقي cross_pool = src_design && هذا->design && src_design != هذا->design;

	IdString type = other->type.ref();
	إذا (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = جوڙيو_Cell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, هذا->design);
	RTLIL::copy_attr_dict(cell->خاصيتون, other->خاصيتون, src_design, هذا->design);

	إذا (!cross_pool)
		cell->connections_ = other->connections_;
	وإلا
		لكل (صحيح i = Get_سائيز(other->connections_) - 1; i >= 0; i--) {
			خودڪار &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.پهريون)] = c.ٻيو;
		}
	أرجع cell;
}

RTLIL::يادگيري *RTLIL::Module::جوڙيو_يادگيري(IdString name)
{
	log_assert(design);
	RTLIL::يادگيري *mem = جديد RTLIL::يادگيري;
	mem->module = هذا;
	mem->name = name;
	memories[name] = mem;
	أرجع mem;
}

RTLIL::يادگيري *RTLIL::Module::جوڙيو_يادگيري(IdString name, ثابت RTLIL::يادگيري *other)
{
	log_assert(design);
	RTLIL::يادگيري *mem = جديد RTLIL::يادگيري;
	mem->module = هذا;
	mem->name = name;
	mem->ويڪر = other->ويڪر;
	mem->شروعات_وٿي = other->شروعات_وٿي;
	mem->سائيز = other->سائيز;
	RTLIL::copy_attr_dict(mem->خاصيتون, other->خاصيتون,
			other->module ? other->module->design : خالي_اشارو, design);
	memories[name] = mem;
	أرجع mem;
}

RTLIL::Process *RTLIL::Module::جوڙيو_Process(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = جديد RTLIL::Process;
	proc->name = name;
	جوڙيو(proc);
	أرجع proc;
}

RTLIL::Process *RTLIL::Module::جوڙيو_Process(RTLIL::IdString name, ثابت RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : خالي_اشارو;
	إذا (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> خاصيتون;
			RTLIL::copy_attr_dict(خاصيتون, obj->خاصيتون, src_design, design);
			obj->خاصيتون = std::move(خاصيتون);
		});
		لكل (خودڪار *متزامن : proc->syncs)
			لكل (خودڪار &mwa : متزامن->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	جوڙيو(proc);
	أرجع proc;
}

#وصف_ڪريو DEF_طريقو(_func, y_سائيز, _type) \
	RTLIL::Cell* RTLIL::Module::جوڙيو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڙيو_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_ويڪر] = sig_a.سائيز();       \
		cell->parameters[ID::Y_ويڪر] = sig_y.سائيز();       \
		cell->سيٽ_پورٽ(ID::A, sig_a);                        \
		cell->سيٽ_پورٽ(ID::Y, sig_y);                        \
		cell->سيٽ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڙيو_Wire(NEW_ID, y_سائيز);    \
		جوڙيو ## _func(name, sig_a, sig_y, is_signed, src);   \
		أرجع sig_y;                                       \
	}
DEF_طريقو(Not,        sig_a.سائيز(), ID($not))
DEF_طريقو(Pos,        sig_a.سائيز(), ID($pos))
DEF_طريقو(Neg,        sig_a.سائيز(), ID($neg))
DEF_طريقو(ReduceAnd,  1, ID($reduce_and))
DEF_طريقو(ReduceOr,   1, ID($reduce_or))
DEF_طريقو(ReduceXor,  1, ID($reduce_xor))
DEF_طريقو(ReduceXnor, 1, ID($reduce_xnor))
DEF_طريقو(ReduceBool, 1, ID($reduce_bool))
DEF_طريقو(LogicNot,   1, ID($logic_not))
#وصف_رد DEF_طريقو

#وصف_ڪريو DEF_طريقو(_func, y_سائيز, _type) \
	RTLIL::Cell* RTLIL::Module::جوڙيو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقي /* is_signed */, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڙيو_Cell(name, _type);           \
		cell->parameters[ID::ويڪر_2] = sig_a.سائيز();         \
		cell->سيٽ_پورٽ(ID::A, sig_a);                        \
		cell->سيٽ_پورٽ(ID::Y, sig_y);                        \
		cell->سيٽ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڙيو_Wire(NEW_ID, y_سائيز);    \
		جوڙيو ## _func(name, sig_a, sig_y, is_signed, src);   \
		أرجع sig_y;                                       \
	}
DEF_طريقو(Buf,     sig_a.سائيز(), ID($buf))
DEF_طريقو(Barrier, sig_a.سائيز(), ID($barrier))
#وصف_رد DEF_طريقو

#وصف_ڪريو DEF_طريقو(_func, y_سائيز, _type) \
	RTLIL::Cell* RTLIL::Module::جوڙيو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڙيو_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_ويڪر] = sig_a.سائيز();       \
		cell->parameters[ID::B_ويڪر] = sig_b.سائيز();       \
		cell->parameters[ID::Y_ويڪر] = sig_y.سائيز();       \
		cell->سيٽ_پورٽ(ID::A, sig_a);                        \
		cell->سيٽ_پورٽ(ID::B, sig_b);                        \
		cell->سيٽ_پورٽ(ID::Y, sig_y);                        \
		cell->سيٽ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڙيو_Wire(NEW_ID, y_سائيز);         \
		جوڙيو ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_طريقو(And,      max(sig_a.سائيز(), sig_b.سائيز()), ID($and))
DEF_طريقو(Or,       max(sig_a.سائيز(), sig_b.سائيز()), ID($or))
DEF_طريقو(Xor,      max(sig_a.سائيز(), sig_b.سائيز()), ID($xor))
DEF_طريقو(Xnor,     max(sig_a.سائيز(), sig_b.سائيز()), ID($xnor))
DEF_طريقو(سرڪايو_2,    sig_a.سائيز(), ID($سرڪايو))
DEF_طريقو(Lt,       1, ID($lt))
DEF_طريقو(Le,       1, ID($le))
DEF_طريقو(Eq,       1, ID($eq))
DEF_طريقو(Ne,       1, ID($ne))
DEF_طريقو(Eqx,      1, ID($eqx))
DEF_طريقو(Nex,      1, ID($nex))
DEF_طريقو(Ge,       1, ID($ge))
DEF_طريقو(Gt,       1, ID($gt))
DEF_طريقو(جوڙيو_2,      max(sig_a.سائيز(), sig_b.سائيز()), ID($جوڙيو))
DEF_طريقو(Sub,      max(sig_a.سائيز(), sig_b.سائيز()), ID($sub))
DEF_طريقو(Mul,      max(sig_a.سائيز(), sig_b.سائيز()), ID($mul))
DEF_طريقو(Div,      max(sig_a.سائيز(), sig_b.سائيز()), ID($div))
DEF_طريقو(Mod,      max(sig_a.سائيز(), sig_b.سائيز()), ID($mod))
DEF_طريقو(DivFloor, max(sig_a.سائيز(), sig_b.سائيز()), ID($divfloor))
DEF_طريقو(ModFloor, max(sig_a.سائيز(), sig_b.سائيز()), ID($modfloor))
DEF_طريقو(LogicAnd, 1, ID($logic_and))
DEF_طريقو(LogicOr,  1, ID($logic_or))
#وصف_رد DEF_طريقو

#وصف_ڪريو DEF_طريقو(_func, y_سائيز, _type) \
	RTLIL::Cell* RTLIL::Module::جوڙيو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڙيو_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = خطأ;             \
		cell->parameters[ID::A_ويڪر] = sig_a.سائيز();       \
		cell->parameters[ID::B_ويڪر] = sig_b.سائيز();       \
		cell->parameters[ID::Y_ويڪر] = sig_y.سائيز();       \
		cell->سيٽ_پورٽ(ID::A, sig_a);                        \
		cell->سيٽ_پورٽ(ID::B, sig_b);                        \
		cell->سيٽ_پورٽ(ID::Y, sig_y);                        \
		cell->سيٽ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڙيو_Wire(NEW_ID, y_سائيز);         \
		جوڙيو ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_طريقو(Shl,      sig_a.سائيز(), ID($shl))
DEF_طريقو(Shr,      sig_a.سائيز(), ID($shr))
DEF_طريقو(Sshl,     sig_a.سائيز(), ID($sshl))
DEF_طريقو(Sshr,     sig_a.سائيز(), ID($sshr))
#وصف_رد DEF_طريقو

#وصف_ڪريو DEF_طريقو(_func, y_سائيز, _type) \
	RTLIL::Cell* RTLIL::Module::جوڙيو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڙيو_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = خطأ;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_ويڪر] = sig_a.سائيز();       \
		cell->parameters[ID::B_ويڪر] = sig_b.سائيز();       \
		cell->parameters[ID::Y_ويڪر] = sig_y.سائيز();       \
		cell->سيٽ_پورٽ(ID::A, sig_a);                        \
		cell->سيٽ_پورٽ(ID::B, sig_b);                        \
		cell->سيٽ_پورٽ(ID::Y, sig_y);                        \
		cell->سيٽ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڙيو_Wire(NEW_ID, y_سائيز);         \
		جوڙيو ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_طريقو(Shiftx,      sig_a.سائيز(), ID($shiftx))
#وصف_رد DEF_طريقو

#وصف_ڪريو DEF_طريقو(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::جوڙيو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڙيو_Cell(name, _type);                 \
		cell->parameters[ID::ويڪر_2] = sig_a.سائيز();               \
		إذا (_pmux) cell->parameters[ID::S_ويڪر] = sig_s.سائيز();  \
		cell->سيٽ_پورٽ(ID::A, sig_a);                              \
		cell->سيٽ_پورٽ(ID::B, sig_b);                              \
		cell->سيٽ_پورٽ(ID::S, sig_s);                              \
		cell->سيٽ_پورٽ(ID::Y, sig_y);                              \
		cell->سيٽ_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڙيو_Wire(NEW_ID, sig_a.سائيز());     \
		جوڙيو ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		أرجع sig_y;                                             \
	}
DEF_طريقو(Mux,      ID($mux),        0)
DEF_طريقو(Bwmux,    ID($bwmux),      0)
DEF_طريقو(Pmux,     ID($pmux),       1)
#وصف_رد DEF_طريقو

#وصف_ڪريو DEF_طريقو(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::جوڙيو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڙيو_Cell(name, _type);                 \
		cell->parameters[ID::ويڪر_2] = _demux ? sig_a.سائيز() : sig_y.سائيز(); \
		cell->parameters[ID::S_ويڪر] = sig_s.سائيز();             \
		cell->سيٽ_پورٽ(ID::A, sig_a);                              \
		cell->سيٽ_پورٽ(ID::S, sig_s);                              \
		cell->سيٽ_پورٽ(ID::Y, sig_y);                              \
		cell->سيٽ_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڙيو_Wire(NEW_ID, _demux ? sig_a.سائيز() << sig_s.سائيز() : sig_a.سائيز() >> sig_s.سائيز()); \
		جوڙيو ## _func(name, sig_a, sig_s, sig_y, src);             \
		أرجع sig_y;                                             \
	}
DEF_طريقو(Bmux,     ID($bmux),       0)
DEF_طريقو(Demux,    ID($demux),      1)
#وصف_رد DEF_طريقو

#وصف_ڪريو DEF_طريقو(_func, _type) \
	RTLIL::Cell* RTLIL::Module::جوڙيو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڙيو_Cell(name, _type);                 \
		cell->parameters[ID::ويڪر_2] = sig_a.سائيز();               \
		cell->سيٽ_پورٽ(ID::A, sig_a);                              \
		cell->سيٽ_پورٽ(ID::B, sig_b);                              \
		cell->سيٽ_پورٽ(ID::Y, sig_y);                              \
		cell->سيٽ_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڙيو_Wire(NEW_ID, sig_a.سائيز());     \
		جوڙيو ## _func(name, sig_a, sig_s, sig_y, src);             \
		أرجع sig_y;                                             \
	}
DEF_طريقو(Bweqx,    ID($bweqx))
#وصف_رد DEF_طريقو

#وصف_ڪريو DEF_طريقو_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::جوڙيو ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig1, ثابت RTLIL::Sig_ٻٽو_انگ &sig2, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڙيو_Cell(name, _type);         \
		cell->سيٽ_پورٽ("\\" #_P1, sig1);                   \
		cell->سيٽ_پورٽ("\\" #_P2, sig2);                   \
		cell->سيٽ_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_ٻٽو_انگ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig1, ثابت std::string &src) { \
		RTLIL::Sig_ٻٽو_انگ sig2 = جوڙيو_Wire(NEW_ID);             \
		جوڙيو ## _func(name, sig1, sig2, src);              \
		أرجع sig2;                                      \
	}
#وصف_ڪريو DEF_طريقو_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::جوڙيو ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig1, ثابت RTLIL::Sig_ٻٽو_انگ &sig2, ثابت RTLIL::Sig_ٻٽو_انگ &sig3, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڙيو_Cell(name, _type);         \
		cell->سيٽ_پورٽ("\\" #_P1, sig1);                   \
		cell->سيٽ_پورٽ("\\" #_P2, sig2);                   \
		cell->سيٽ_پورٽ("\\" #_P3, sig3);                   \
		cell->سيٽ_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_ٻٽو_انگ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig1, ثابت RTLIL::Sig_ٻٽو_انگ &sig2, ثابت std::string &src) { \
		RTLIL::Sig_ٻٽو_انگ sig3 = جوڙيو_Wire(NEW_ID);             \
		جوڙيو ## _func(name, sig1, sig2, sig3, src);        \
		أرجع sig3;                                      \
	}
#وصف_ڪريو DEF_طريقو_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::جوڙيو ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig1, ثابت RTLIL::Sig_ٻٽو_انگ &sig2, ثابت RTLIL::Sig_ٻٽو_انگ &sig3, ثابت RTLIL::Sig_ٻٽو_انگ &sig4, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڙيو_Cell(name, _type);         \
		cell->سيٽ_پورٽ("\\" #_P1, sig1);                   \
		cell->سيٽ_پورٽ("\\" #_P2, sig2);                   \
		cell->سيٽ_پورٽ("\\" #_P3, sig3);                   \
		cell->سيٽ_پورٽ("\\" #_P4, sig4);                   \
		cell->سيٽ_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_ٻٽو_انگ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig1, ثابت RTLIL::Sig_ٻٽو_انگ &sig2, ثابت RTLIL::Sig_ٻٽو_انگ &sig3, ثابت std::string &src) { \
		RTLIL::Sig_ٻٽو_انگ sig4 = جوڙيو_Wire(NEW_ID);             \
		جوڙيو ## _func(name, sig1, sig2, sig3, sig4, src);  \
		أرجع sig4;                                      \
	}
#وصف_ڪريو DEF_طريقو_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::جوڙيو ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig1, ثابت RTLIL::Sig_ٻٽو_انگ &sig2, ثابت RTLIL::Sig_ٻٽو_انگ &sig3, ثابت RTLIL::Sig_ٻٽو_انگ &sig4, ثابت RTLIL::Sig_ٻٽو_انگ &sig5, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڙيو_Cell(name, _type);         \
		cell->سيٽ_پورٽ("\\" #_P1, sig1);                   \
		cell->سيٽ_پورٽ("\\" #_P2, sig2);                   \
		cell->سيٽ_پورٽ("\\" #_P3, sig3);                   \
		cell->سيٽ_پورٽ("\\" #_P4, sig4);                   \
		cell->سيٽ_پورٽ("\\" #_P5, sig5);                   \
		cell->سيٽ_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_ٻٽو_انگ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig1, ثابت RTLIL::Sig_ٻٽو_انگ &sig2, ثابت RTLIL::Sig_ٻٽو_انگ &sig3, ثابت RTLIL::Sig_ٻٽو_انگ &sig4, ثابت std::string &src) { \
		RTLIL::Sig_ٻٽو_انگ sig5 = جوڙيو_Wire(NEW_ID);                  \
		جوڙيو ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		أرجع sig5;                                           \
	}
DEF_طريقو_2(BufGate,    ID($_BUF_),    A, Y)
DEF_طريقو_2(NotGate,    ID($_NOT_),    A, Y)
DEF_طريقو_3(AndGate,    ID($_AND_),    A, B, Y)
DEF_طريقو_3(NandGate,   ID($_NAND_),   A, B, Y)
DEF_طريقو_3(OrGate,     ID($_OR_),     A, B, Y)
DEF_طريقو_3(NorGate,    ID($_NOR_),    A, B, Y)
DEF_طريقو_3(XorGate,    ID($_XOR_),    A, B, Y)
DEF_طريقو_3(XnorGate,   ID($_XNOR_),   A, B, Y)
DEF_طريقو_3(AndnotGate, ID($_ANDNOT_), A, B, Y)
DEF_طريقو_3(OrnotGate,  ID($_ORNOT_),  A, B, Y)
DEF_طريقو_4(MuxGate,    ID($_MUX_),    A, B, S, Y)
DEF_طريقو_4(NmuxGate,   ID($_NMUX_),   A, B, S, Y)
DEF_طريقو_4(Aoi3Gate,   ID($_AOI3_),   A, B, C, Y)
DEF_طريقو_4(Oai3Gate,   ID($_OAI3_),   A, B, C, Y)
DEF_طريقو_5(Aoi4Gate,   ID($_AOI4_),   A, B, C, D, Y)
DEF_طريقو_5(Oai4Gate,   ID($_OAI4_),   A, B, C, D, Y)
#وصف_رد DEF_طريقو_2
#وصف_رد DEF_طريقو_3
#وصف_رد DEF_طريقو_4
#وصف_رد DEF_طريقو_5

RTLIL::Cell* RTLIL::Module::جوڙيو_Pow(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي a_signed, منطقي b_signed, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($قوت));
	cell->parameters[ID::A_SIGNED] = a_signed;
	cell->parameters[ID::B_SIGNED] = b_signed;
	cell->parameters[ID::A_ويڪر] = sig_a.سائيز();
	cell->parameters[ID::B_ويڪر] = sig_b.سائيز();
	cell->parameters[ID::Y_ويڪر] = sig_y.سائيز();
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::B, sig_b);
	cell->سيٽ_پورٽ(ID::Y, sig_y);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Fa(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_x, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($fa));
	cell->parameters[ID::ويڪر_2] = sig_a.سائيز();
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::B, sig_b);
	cell->سيٽ_پورٽ(ID::C, sig_c);
	cell->سيٽ_پورٽ(ID::X, sig_x);
	cell->سيٽ_پورٽ(ID::Y, sig_y);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_ٽڪرو(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const وٿي, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($ٽڪرو));
	cell->parameters[ID::A_ويڪر] = sig_a.سائيز();
	cell->parameters[ID::Y_ويڪر] = sig_y.سائيز();
	cell->parameters[ID::وٿي_2] = وٿي;
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::Y, sig_y);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Concat(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($concat));
	cell->parameters[ID::A_ويڪر] = sig_a.سائيز();
	cell->parameters[ID::B_ويڪر] = sig_b.سائيز();
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::B, sig_b);
	cell->سيٽ_پورٽ(ID::Y, sig_y);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Lut(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::ويڪر_2] = sig_a.سائيز();
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::Y, sig_y);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Tribuf(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($tribuf));
	cell->parameters[ID::ويڪر_2] = sig_a.سائيز();
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_پورٽ(ID::Y, sig_y);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Assert(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($أكد));
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Assume(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($assume));
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_جيئرو(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($جيئرو));
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Fair(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($fair));
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Cover(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($cover));
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Equiv(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($equiv));
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::B, sig_b);
	cell->سيٽ_پورٽ(ID::Y, sig_y);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Sr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr, ثابت RTLIL::SigSpec &sig_q, منطقي سيٽ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($sr));
	cell->parameters[ID::سيٽ_POLARITY] = سيٽ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::سيٽ_2, sig_سيٽ);
	cell->سيٽ_پورٽ(ID::CLR, sig_clr);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Ff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($ff));
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Dff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::CLK, sig_clk);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Dffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($dffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::CLK, sig_clk);
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Dffsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي سيٽ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($dffsr));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::سيٽ_POLARITY] = سيٽ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::CLK, sig_clk);
	cell->سيٽ_پورٽ(ID::سيٽ_2, sig_سيٽ);
	cell->سيٽ_پورٽ(ID::CLR, sig_clr);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Dffsre(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, منطقي سيٽ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($dffsre));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::سيٽ_POLARITY] = سيٽ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::CLK, sig_clk);
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_پورٽ(ID::سيٽ_2, sig_سيٽ);
	cell->سيٽ_پورٽ(ID::CLR, sig_clr);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Adff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_قدر, منطقي clk_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_قدر] = arst_قدر;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::CLK, sig_clk);
	cell->سيٽ_پورٽ(ID::ARST, sig_arst);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Adffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_قدر, منطقي clk_polarity, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_قدر] = arst_قدر;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::CLK, sig_clk);
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_پورٽ(ID::ARST, sig_arst);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Aldff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($aldff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::CLK, sig_clk);
	cell->سيٽ_پورٽ(ID::ALOAD, sig_aload);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::AD, sig_ad);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Aldffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي en_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($aldffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::CLK, sig_clk);
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_پورٽ(ID::ALOAD, sig_aload);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::AD, sig_ad);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Sdff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_قدر, منطقي clk_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_قدر] = srst_قدر;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::CLK, sig_clk);
	cell->سيٽ_پورٽ(ID::SRST, sig_srst);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Sdffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_قدر, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_قدر] = srst_قدر;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::CLK, sig_clk);
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_پورٽ(ID::SRST, sig_srst);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Sdffce(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_قدر, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_قدر] = srst_قدر;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::CLK, sig_clk);
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_پورٽ(ID::SRST, sig_srst);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Dlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Adlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_قدر, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_قدر] = arst_قدر;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_پورٽ(ID::ARST, sig_arst);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Dlatchsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, منطقي سيٽ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($dlatchsr));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::سيٽ_POLARITY] = سيٽ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::EN, sig_en);
	cell->سيٽ_پورٽ(ID::سيٽ_2, sig_سيٽ);
	cell->سيٽ_پورٽ(ID::CLR, sig_clr);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Sr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr,
		ثابت RTLIL::SigSpec &sig_q, منطقي سيٽ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_SR_%c%c_", سيٽ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->سيٽ_پورٽ(ID::S, sig_سيٽ);
	cell->سيٽ_پورٽ(ID::R, sig_clr);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Ff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($_FF_));
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Dff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->سيٽ_پورٽ(ID::C, sig_clk);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Dffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->سيٽ_پورٽ(ID::C, sig_clk);
	cell->سيٽ_پورٽ(ID::E, sig_en);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Dffsr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي سيٽ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', سيٽ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->سيٽ_پورٽ(ID::C, sig_clk);
	cell->سيٽ_پورٽ(ID::S, sig_سيٽ);
	cell->سيٽ_پورٽ(ID::R, sig_clr);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Dffsre_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, منطقي سيٽ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', سيٽ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->سيٽ_پورٽ(ID::C, sig_clk);
	cell->سيٽ_پورٽ(ID::S, sig_سيٽ);
	cell->سيٽ_پورٽ(ID::R, sig_clr);
	cell->سيٽ_پورٽ(ID::E, sig_en);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Adff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_قدر, منطقي clk_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_قدر ? '1' : '0'));
	cell->سيٽ_پورٽ(ID::C, sig_clk);
	cell->سيٽ_پورٽ(ID::R, sig_arst);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Adffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_قدر, منطقي clk_polarity, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_قدر ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->سيٽ_پورٽ(ID::C, sig_clk);
	cell->سيٽ_پورٽ(ID::R, sig_arst);
	cell->سيٽ_پورٽ(ID::E, sig_en);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Aldff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->سيٽ_پورٽ(ID::C, sig_clk);
	cell->سيٽ_پورٽ(ID::L, sig_aload);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::AD, sig_ad);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Aldffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي en_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->سيٽ_پورٽ(ID::C, sig_clk);
	cell->سيٽ_پورٽ(ID::L, sig_aload);
	cell->سيٽ_پورٽ(ID::E, sig_en);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::AD, sig_ad);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Sdff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_قدر, منطقي clk_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_قدر ? '1' : '0'));
	cell->سيٽ_پورٽ(ID::C, sig_clk);
	cell->سيٽ_پورٽ(ID::R, sig_srst);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Sdffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_قدر, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_قدر ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->سيٽ_پورٽ(ID::C, sig_clk);
	cell->سيٽ_پورٽ(ID::R, sig_srst);
	cell->سيٽ_پورٽ(ID::E, sig_en);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Sdffce_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_قدر, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_قدر ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->سيٽ_پورٽ(ID::C, sig_clk);
	cell->سيٽ_پورٽ(ID::R, sig_srst);
	cell->سيٽ_پورٽ(ID::E, sig_en);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Dlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->سيٽ_پورٽ(ID::E, sig_en);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Adlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_قدر, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_قدر ? '1' : '0'));
	cell->سيٽ_پورٽ(ID::E, sig_en);
	cell->سيٽ_پورٽ(ID::R, sig_arst);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Dlatchsr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, منطقي سيٽ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', سيٽ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->سيٽ_پورٽ(ID::E, sig_en);
	cell->سيٽ_پورٽ(ID::S, sig_سيٽ);
	cell->سيٽ_پورٽ(ID::R, sig_clr);
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Anyinit(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($anyinit));
	cell->parameters[ID::ويڪر_2] = sig_q.سائيز();
	cell->سيٽ_پورٽ(ID::D, sig_d);
	cell->سيٽ_پورٽ(ID::Q, sig_q);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, صحيح ويڪر, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڙيو_Wire(NEW_ID, ويڪر);
	Cell *cell = جوڙيو_Cell(name, ID($anyconst));
	cell->سيٽ_Param(ID::ويڪر_2, ويڪر);
	cell->سيٽ_پورٽ(ID::Y, sig);
	cell->سيٽ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, صحيح ويڪر, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڙيو_Wire(NEW_ID, ويڪر);
	Cell *cell = جوڙيو_Cell(name, ID($anyseq));
	cell->سيٽ_Param(ID::ويڪر_2, ويڪر);
	cell->سيٽ_پورٽ(ID::Y, sig);
	cell->سيٽ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, صحيح ويڪر, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڙيو_Wire(NEW_ID, ويڪر);
	Cell *cell = جوڙيو_Cell(name, ID($allconst));
	cell->سيٽ_Param(ID::ويڪر_2, ويڪر);
	cell->سيٽ_پورٽ(ID::Y, sig);
	cell->سيٽ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, صحيح ويڪر, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڙيو_Wire(NEW_ID, ويڪر);
	Cell *cell = جوڙيو_Cell(name, ID($allseq));
	cell->سيٽ_Param(ID::ويڪر_2, ويڪر);
	cell->سيٽ_پورٽ(ID::Y, sig);
	cell->سيٽ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڙيو_Wire(NEW_ID);
	Cell *cell = جوڙيو_Cell(name, ID($initstate));
	cell->سيٽ_پورٽ(ID::Y, sig);
	cell->سيٽ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::سيٽ_ٽيگ_2(RTLIL::IdString name, ثابت std::string &ٽيگ, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڙيو_Wire(NEW_ID, sig_a.سائيز());
	Cell *cell = جوڙيو_Cell(name, ID($سيٽ_ٽيگ));
	cell->parameters[ID::ويڪر_2] = sig_a.سائيز();
	cell->parameters[ID::ٽيگ_2] = ٽيگ;
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::سيٽ_2, sig_s);
	cell->سيٽ_پورٽ(ID::CLR, sig_c);
	cell->سيٽ_پورٽ(ID::Y, sig);
	cell->سيٽ_src_attribute(src);
	أرجع sig;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_سيٽ_ٽيگ(RTLIL::IdString name, ثابت std::string &ٽيگ, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	Cell *cell = جوڙيو_Cell(name, ID($سيٽ_ٽيگ));
	cell->parameters[ID::ويڪر_2] = sig_a.سائيز();
	cell->parameters[ID::ٽيگ_2] = ٽيگ;
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::سيٽ_2, sig_s);
	cell->سيٽ_پورٽ(ID::CLR, sig_c);
	cell->سيٽ_پورٽ(ID::Y, sig_y);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::Get_ٽيگ(RTLIL::IdString name, ثابت std::string &ٽيگ, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڙيو_Wire(NEW_ID, sig_a.سائيز());
	Cell *cell = جوڙيو_Cell(name, ID($get_ٽيگ));
	cell->parameters[ID::ويڪر_2] = sig_a.سائيز();
	cell->parameters[ID::ٽيگ_2] = ٽيگ;
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::Y, sig);
	cell->سيٽ_src_attribute(src);
	أرجع sig;
}

RTLIL::Cell* RTLIL::Module::جوڙيو_Overwrite_ٽيگ(RTLIL::IdString name, ثابت std::string &ٽيگ, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڙيو_Cell(name, ID($overwrite_ٽيگ));
	cell->parameters[ID::ويڪر_2] = sig_a.سائيز();
	cell->parameters[ID::ٽيگ_2] = ٽيگ;
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::سيٽ_2, sig_s);
	cell->سيٽ_پورٽ(ID::CLR, sig_c);
	cell->سيٽ_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::Original_ٽيگ(RTLIL::IdString name, ثابت std::string &ٽيگ, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڙيو_Wire(NEW_ID, sig_a.سائيز());
	Cell *cell = جوڙيو_Cell(name, ID($original_ٽيگ));
	cell->parameters[ID::ويڪر_2] = sig_a.سائيز();
	cell->parameters[ID::ٽيگ_2] = ٽيگ;
	cell->سيٽ_پورٽ(ID::A, sig_a);
	cell->سيٽ_پورٽ(ID::Y, sig);
	cell->سيٽ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڙيو_Wire(NEW_ID, sig_e.سائيز());
	Cell *cell = جوڙيو_Cell(name, ID($future_ff));
	cell->parameters[ID::ويڪر_2] = sig_e.سائيز();
	cell->سيٽ_پورٽ(ID::A, sig_e);
	cell->سيٽ_پورٽ(ID::Y, sig);
	cell->سيٽ_src_attribute(src);
	أرجع sig;
}

std::string RTLIL::Module::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(هذا), design, خطأ, سچ, خطأ, RTLIL_BACKEND::Dump_گهڻائي::Readable);
	أرجع f.str();
}

RTLIL::Wire::Wire()
{
	ساكن بنانشان صحيح hashidx_ڳڻپ = 123456789;
	hashidx_ڳڻپ = mkhash_xorshift(hashidx_ڳڻپ);
	hashidx_ = hashidx_ڳڻپ;

	module = خالي_اشارو;
	ويڪر = 1;
	شروعات_وٿي = 0;
	پورٽ_id = 0;
	پورٽ_داخلا = خطأ;
	پورٽ_output = خطأ;
	upto = خطأ;
	is_signed = خطأ;

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->وجهو(std::pair<بنانشان صحيح, RTLIL::Wire*>(hashidx_, هذا));
#جيڪڏهن_پورو
}

RTLIL::Wire::~Wire()
{
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#جيڪڏهن_پورو
}

std::string RTLIL::Wire::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", هذا, design(), RTLIL_BACKEND::Dump_گهڻائي::Readable);
	أرجع f.str();
}

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
ساكن std::map<بنانشان صحيح, RTLIL::Wire*> all_wires;
std::map<بنانشان صحيح, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(فراغ)
{
	أرجع &all_wires;
}
#جيڪڏهن_پورو

RTLIL::يادگيري::يادگيري()
{
	ساكن بنانشان صحيح hashidx_ڳڻپ = 123456789;
	hashidx_ڳڻپ = mkhash_xorshift(hashidx_ڳڻپ);
	hashidx_ = hashidx_ڳڻپ;

	ويڪر = 1;
	شروعات_وٿي = 0;
	سائيز = 0;
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	RTLIL::يادگيري::get_all_memorys()->وجهو(std::pair<بنانشان صحيح, RTLIL::يادگيري*>(hashidx_, هذا));
#جيڪڏهن_پورو
}

std::string RTLIL::يادگيري::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_يادگيري(f, "", هذا, design(), RTLIL_BACKEND::Dump_گهڻائي::Readable);
	أرجع f.str();
}

RTLIL::Process::Process() : module(خالي_اشارو)
{
	ساكن بنانشان صحيح hashidx_ڳڻپ = 123456789;
	hashidx_ڳڻپ = mkhash_xorshift(hashidx_ڳڻپ);
	hashidx_ = hashidx_ڳڻپ;
}

std::string RTLIL::Process::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", هذا, design(), RTLIL_BACKEND::Dump_گهڻائي::Readable);
	أرجع f.str();
}

RTLIL::Cell::Cell() : module(خالي_اشارو)
{
	ساكن بنانشان صحيح hashidx_ڳڻپ = 123456789;
	hashidx_ڳڻپ = mkhash_xorshift(hashidx_ڳڻپ);
	hashidx_ = hashidx_ڳڻپ;

	// log("#memtrace# %p\n", this);
	memhasher();

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->وجهو(std::pair<بنانشان صحيح, RTLIL::Cell*>(hashidx_, هذا));
#جيڪڏهن_پورو
}

RTLIL::Cell::~Cell()
{
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#جيڪڏهن_پورو
}

std::string RTLIL::Cell::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", هذا, design(), RTLIL_BACKEND::Dump_گهڻائي::Readable);
	أرجع f.str();
}

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
ساكن std::map<بنانشان صحيح, RTLIL::Cell*> all_cells;
std::map<بنانشان صحيح, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(فراغ)
{
	أرجع &all_cells;
}
#جيڪڏهن_پورو

منطقي RTLIL::Cell::has_پورٽ(RTLIL::IdString portname) ثابت
{
	أرجع connections_.ڳڻپ(portname) != 0;
}

// bufnorm

ثابت RTLIL::SigSpec &RTLIL::Cell::get_پورٽ(RTLIL::IdString portname) ثابت
{
	أرجع connections_.at(portname);
}

ثابت dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() ثابت
{
	أرجع connections_;
}

منطقي RTLIL::Cell::معلوم() ثابت
{
	إذا (yosys_celltypes.cell_معلوم(type))
		أرجع سچ;
	إذا (module && module->design && module->design->module(type))
		أرجع سچ;
	أرجع خطأ;
}

منطقي RTLIL::Cell::داخلا(IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_معلوم(type))
		أرجع yosys_celltypes.cell_داخلا(type.ref(), portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : خالي_اشارو;
		أرجع w && w->پورٽ_داخلا;
	}
	أرجع خطأ;
}

منطقي RTLIL::Cell::output(RTLIL::IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_معلوم(type))
		أرجع yosys_celltypes.cell_output(type, portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : خالي_اشارو;
		أرجع w && w->پورٽ_output;
	}
	أرجع خطأ;
}

RTLIL::پورٽ_Dir RTLIL::Cell::پورٽ_dir(RTLIL::IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_معلوم(type.ref()))
		أرجع yosys_celltypes.cell_پورٽ_dir(type, portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		إذا (m == خالي_اشارو)
			أرجع پورٽ_Dir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		إذا (w == خالي_اشارو)
			أرجع پورٽ_Dir::PD_UNKNOWN;
		أرجع پورٽ_Dir(w->پورٽ_داخلا + w->پورٽ_output * 2);
	}
	أرجع پورٽ_Dir::PD_UNKNOWN;
}

منطقي RTLIL::Cell::hasParam(RTLIL::IdString paramname) ثابت
{
	أرجع parameters.ڳڻپ(paramname) != 0;
}

فراغ RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

فراغ RTLIL::Cell::سيٽ_Param(RTLIL::IdString paramname, RTLIL::Const قدر)
{
	parameters[paramname] = std::move(قدر);
}

ثابت RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ثابت
{
	ثابت خودڪار &it = parameters.ڳوليو(paramname);
	إذا (it != parameters.پڇاڙي())
		أرجع it->ٻيو;
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		إذا (m)
			أرجع m->parameter_default_قدرون.at(paramname);
	}
	ارم std::ٻاهريون_جو_range("Cell::getParam()");
}

فراغ RTLIL::Cell::ترتيب_ڏيو()
{
	connections_.ترتيب_ڏيو(ترتيب_ڏيو_by_id_str(module->twines()));
	parameters.ترتيب_ڏيو(ترتيب_ڏيو_by_id_str(module->twines()));
	خاصيتون.ترتيب_ڏيو(ترتيب_ڏيو_by_id_str(module->twines()));
}

فراغ RTLIL::Cell::check()
{
#جيڪڏهن_وصف_نه_ٿيل NDEBUG
	اندروني_Cell_Checker checker(عدم, هذا);
	checker.check();
#جيڪڏهن_پورو
}

فراغ RTLIL::Cell::fixup_parameters(منطقي سيٽ_a_signed, منطقي سيٽ_b_signed)
{
	إذا (type.is_عوامي() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		أرجع;

	إذا (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::ويڪر_2] = Get_سائيز(connections_[ID::Y]);
		إذا (type.اندريون(ID($pmux), ID($bmux)))
			parameters[ID::S_ويڪر] = Get_سائيز(connections_[ID::S]);
		check();
		أرجع;
	}

	إذا (type == ID($demux)) {
		parameters[ID::ويڪر_2] = Get_سائيز(connections_[ID::A]);
		parameters[ID::S_ويڪر] = Get_سائيز(connections_[ID::S]);
		check();
		أرجع;
	}

	إذا (type == ID($lut) || type == ID($sop)) {
		parameters[ID::ويڪر_2] = Get_سائيز(connections_[ID::A]);
		أرجع;
	}

	إذا (type == ID($fa)) {
		parameters[ID::ويڪر_2] = Get_سائيز(connections_[ID::Y]);
		أرجع;
	}

	إذا (type == ID($lcu)) {
		parameters[ID::ويڪر_2] = Get_سائيز(connections_[ID::CO]);
		أرجع;
	}

	إذا (type == ID($macc_v2)) {
		parameters[ID::Y_ويڪر] = Get_سائيز(connections_[ID::Y]);
		أرجع;
	}

	منطقي signedness_ab = !type.اندريون(ID($ٽڪرو), ID($concat), ID($macc));

	إذا (connections_.ڳڻپ(ID::A)) {
		إذا (signedness_ab) {
			إذا (سيٽ_a_signed)
				parameters[ID::A_SIGNED] = سچ;
			وإلا إذا (parameters.ڳڻپ(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = خطأ;
		}
		parameters[ID::A_ويڪر] = Get_سائيز(connections_[ID::A]);
	}

	إذا (connections_.ڳڻپ(ID::B)) {
		إذا (signedness_ab) {
			إذا (سيٽ_b_signed)
				parameters[ID::B_SIGNED] = سچ;
			وإلا إذا (parameters.ڳڻپ(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = خطأ;
		}
		parameters[ID::B_ويڪر] = Get_سائيز(connections_[ID::B]);
	}

	إذا (connections_.ڳڻپ(ID::Y) && type != ID($concat))
		parameters[ID::Y_ويڪر] = Get_سائيز(connections_[ID::Y]);

	إذا (connections_.ڳڻپ(ID::Q))
		parameters[ID::ويڪر_2] = Get_سائيز(connections_[ID::Q]);

	check();
}

منطقي RTLIL::Cell::has_keep_attr() ثابت {
	أرجع get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

منطقي RTLIL::Cell::has_memid() ثابت
{
	أرجع type.اندريون(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

منطقي RTLIL::Cell::is_mem_cell() ثابت
{
	أرجع type.اندريون(ID($mem), ID($mem_v2)) || has_memid();
}

منطقي RTLIL::Cell::is_builtin_ff() ثابت {
	أرجع StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ)
{
	wire = ٻٽو_انگ.wire;
	وٿي = 0;
	إذا (wire == عدم)
		معلومات = {ٻٽو_انگ.معلومات};
	وإلا
		وٿي = ٻٽو_انگ.وٿي;
	ويڪر = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::ڪڍو_2(صحيح وٿي, صحيح ڊيگهائي) ثابت
{
	log_assert(وٿي >= 0);
	log_assert(ڊيگهائي >= 0);
	log_assert(وٿي + ڊيگهائي <= ويڪر);
	RTLIL::SigChunk ret;
	إذا (wire) {
		ret.wire = wire;
		ret.وٿي = هذا->وٿي + وٿي;
		ret.ويڪر = ڊيگهائي;
	} وإلا {
		لكل (صحيح i = 0; i < ڊيگهائي; i++)
			ret.معلومات.ڌڪو_پٺ(معلومات[وٿي+i]);
		ret.ويڪر = ڊيگهائي;
	}
	أرجع ret;
}

RTLIL::Sig_ٻٽو_انگ RTLIL::SigChunk::عامل[](صحيح وٿي) ثابت
{
	log_assert(وٿي >= 0);
	log_assert(وٿي < ويڪر);
	RTLIL::Sig_ٻٽو_انگ ret;
	إذا (wire) {
		ret.wire = wire;
		ret.وٿي = هذا->وٿي + وٿي;
	} وإلا {
		ret.معلومات = معلومات[وٿي];
	}
	أرجع ret;
}

منطقي RTLIL::SigChunk::عامل <(ثابت RTLIL::SigChunk &other) ثابت
{
	إذا (wire && other.wire)
		إذا (wire->name != other.wire->name)
			أرجع wire->name < other.wire->name;

	إذا (wire != other.wire)
		أرجع wire < other.wire;

	إذا (وٿي != other.وٿي)
		أرجع وٿي < other.وٿي;

	إذا (ويڪر != other.ويڪر)
		أرجع ويڪر < other.ويڪر;

	أرجع معلومات < other.معلومات;
}

منطقي RTLIL::SigChunk::عامل ==(ثابت RTLIL::SigChunk &other) ثابت
{
	أرجع wire == other.wire && ويڪر == other.ويڪر && وٿي == other.وٿي && معلومات == other.معلومات;
}

منطقي RTLIL::SigChunk::عامل !=(ثابت RTLIL::SigChunk &other) ثابت
{
	إذا (*هذا == other)
		أرجع خطأ;
	أرجع سچ;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_خالي_جايون_بٽ();
	log_assert(parts.سائيز() > 0);
	خودڪار ie = parts.begin();
	خودڪار it = ie + parts.سائيز() - 1;
	طالما (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::Const &قدر)
{
	إذا (Get_سائيز(قدر) != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(قدر);
	} وإلا {
		init_خالي_جايون_بٽ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&قدر)
{
	إذا (Get_سائيز(قدر) != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(قدر);
	} وإلا {
		init_خالي_جايون_بٽ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::SigChunk &chunk)
{
	إذا (chunk.ويڪر != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(chunk);
	} وإلا {
		init_خالي_جايون_بٽ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	إذا (chunk.ويڪر != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(chunk);
	} وإلا {
		init_خالي_جايون_بٽ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	إذا (wire->ويڪر != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(wire);
	} وإلا {
		init_خالي_جايون_بٽ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, صحيح وٿي, صحيح ويڪر)
{
	إذا (ويڪر != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(wire, وٿي, ويڪر);
	} وإلا {
		init_خالي_جايون_بٽ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::string &str)
{
	إذا (str.سائيز() != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(str);
	} وإلا {
		init_خالي_جايون_بٽ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(صحيح val, صحيح ويڪر)
{
	إذا (ويڪر != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(val, ويڪر);
	} وإلا
		init_خالي_جايون_بٽ();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State ٻٽو_انگ, صحيح ويڪر)
{
	إذا (ويڪر != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(ٻٽو_انگ, ويڪر);
	} وإلا
		init_خالي_جايون_بٽ();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ, صحيح ويڪر)
{
	إذا (ويڪر != 0) {
		إذا (ٻٽو_انگ.wire == عدم) {
			rep_ = CHUNK;
			جديد (&chunk_) RTLIL::SigChunk(ٻٽو_انگ.معلومات, ويڪر);
		} وإلا إذا (ويڪر == 1) {
			rep_ = CHUNK;
			جديد (&chunk_) RTLIL::SigChunk(ٻٽو_انگ);
		} وإلا {
			init_خالي_جايون_بٽ();
			بٽ_2.reserve(ويڪر);
			لكل (صحيح i = 0; i < ويڪر; i++)
				بٽ_2.ڌڪو_پٺ(ٻٽو_انگ);
		}
	} وإلا
		init_خالي_جايون_بٽ();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::ويڪٽر<RTLIL::SigChunk> &chunks)
{
	init_خالي_جايون_بٽ();
	لكل (ثابت خودڪار &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> &بٽ)
{
	init_خالي_جايون_بٽ();
	لكل (ثابت خودڪار &ٻٽو_انگ : بٽ)
		append(ٻٽو_انگ);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت pool<RTLIL::Sig_ٻٽو_انگ> &بٽ)
{
	init_خالي_جايون_بٽ();
	لكل (ثابت خودڪار &ٻٽو_انگ : بٽ)
		append(ٻٽو_انگ);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::سيٽ<RTLIL::Sig_ٻٽو_انگ> &بٽ)
{
	init_خالي_جايون_بٽ();
	لكل (ثابت خودڪار &ٻٽو_انگ : بٽ)
		append(ٻٽو_انگ);
	check();
}

RTLIL::SigSpec::SigSpec(منطقي ٻٽو_انگ)
{
	rep_ = CHUNK;
	جديد (&chunk_) RTLIL::SigChunk(ٻٽو_انگ ? RTLIL::S1 : RTLIL::S0);
	check();
}

فراغ RTLIL::SigSpec::Chunks::const_iterator::ايندڙ_chunk_بٽ()
{
	صحيح بٽ_سائيز = Get_سائيز(spec.بٽ_2);
	إذا (ٻٽو_انگ_اشاريو >= بٽ_سائيز)
		أرجع;
	صحيح i = ٻٽو_انگ_اشاريو;
	ثابت Sig_ٻٽو_انگ &ٻٽو_انگ = spec.بٽ_2[i++];
	chunk.wire = ٻٽو_انگ.wire;
	chunk.معلومات.clear();
	إذا (ٻٽو_انگ.is_wire()) {
		chunk.وٿي = ٻٽو_انگ.وٿي;
		طالما (i < بٽ_سائيز && spec.بٽ_2[i].wire == ٻٽو_انگ.wire &&
			spec.بٽ_2[i].وٿي == ٻٽو_انگ.وٿي + i - ٻٽو_انگ_اشاريو)
			++i;
	} وإلا {
		chunk.وٿي = 0;
		chunk.معلومات.ڌڪو_پٺ(ٻٽو_انگ.معلومات);
		طالما (i < بٽ_سائيز && !spec.بٽ_2[i].is_wire()) {
			chunk.معلومات.ڌڪو_پٺ(spec.بٽ_2[i].معلومات);
			++i;
		}
	}
	chunk.ويڪر = i - ٻٽو_انگ_اشاريو;
}

فراغ RTLIL::SigSpec::unpack()
{
	إذا (rep_ == بٽ_3)
		أرجع;

	std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> بٽ;
	بٽ.reserve(chunk_.ويڪر);
	لكل (صحيح i = 0; i < chunk_.ويڪر; i++)
		بٽ.emplace_پٺ(chunk_, i);

	chunk_.~SigChunk();
	rep_ = بٽ_3;
	جديد (&بٽ_2) std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ>(std::move(بٽ));
}

فراغ RTLIL::SigSpec::try_repack()
{
	إذا (rep_ != بٽ_3)
		أرجع;

	صحيح بٽ_سائيز = Get_سائيز(بٽ_2);
	إذا (بٽ_سائيز == 0)
		أرجع;
	إذا (بٽ_2[0].is_wire()) {
		لكل (صحيح i = 1; i < بٽ_سائيز; i++)
			إذا (بٽ_2[0].wire != بٽ_2[i].wire || بٽ_2[0].وٿي + i != بٽ_2[i].وٿي)
				أرجع;
		SigChunk chunk(بٽ_2[0].wire, بٽ_2[0].وٿي, بٽ_سائيز);
		بٽ_2.~ويڪٽر();
		rep_ = CHUNK;
		جديد (&chunk_) SigChunk(std::move(chunk));
		أرجع;
	}
	std::ويڪٽر<RTLIL::State> بٽ;
	بٽ.reserve(بٽ_سائيز);
	بٽ.ڌڪو_پٺ(بٽ_2[0].معلومات);
	لكل (صحيح i = 1; i < بٽ_سائيز; i++) {
		إذا (بٽ_2[i].is_wire())
			أرجع;
		بٽ.ڌڪو_پٺ(بٽ_2[i].معلومات);
	}
	بٽ_2.~ويڪٽر();
	rep_ = CHUNK;
	جديد (&chunk_) SigChunk(std::move(بٽ));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ثابت
{
	Hasher h;
	لكل (خودڪار &c : chunks())
		إذا (c.wire == عدم) {
			لكل (خودڪار &v : c.معلومات)
				h.eat(v);
		} وإلا {
			h.eat(c.wire->name.ref());
			h.eat(c.وٿي);
			h.eat(c.ويڪر);
		}
	Hasher::hash_t نتيجو = h.yield();
	إذا (نتيجو == 0)
		نتيجو = 1;
	hash_.سيٽ(نتيجو);
	أرجع نتيجو;
}

فراغ RTLIL::SigSpec::ترتيب_ڏيو()
{
	unpack();
	std::ترتيب_ڏيو(بٽ_2.begin(), بٽ_2.پڇاڙي());
	hash_.clear();
	try_repack();
}

فراغ RTLIL::SigSpec::ترتيب_ڏيو_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::ويڪٽر<Sig_ٻٽو_انگ> unique_بٽ = بٽ_2;
	std::ترتيب_ڏيو(unique_بٽ.begin(), unique_بٽ.پڇاڙي());
	خودڪار آخري = std::unique(unique_بٽ.begin(), unique_بٽ.پڇاڙي());
	unique_بٽ.erase(آخري, unique_بٽ.پڇاڙي());

	*هذا = unique_بٽ;
	hash_.clear();
	try_repack();
}

فراغ RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &نمونو, ثابت RTLIL::SigSpec &with)
{
	replace(نمونو, with, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &نمونو, ثابت RTLIL::SigSpec &with, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(سائيز() == other->سائيز());
	log_assert(نمونو.سائيز() == with.سائيز());

	dict<RTLIL::Sig_ٻٽو_انگ, صحيح> نمونو_منزل_with;
	صحيح نمونو_سائيز = نمونو.سائيز();
	لكل (صحيح i = 0; i < نمونو_سائيز; i++) {
		Sig_ٻٽو_انگ نمونو_ٻٽو_انگ = نمونو[i];
		إذا (نمونو_ٻٽو_انگ.wire != عدم) {
			نمونو_منزل_with.emplace(نمونو_ٻٽو_انگ, i);
		}
	}

	صحيح this_سائيز = سائيز();
	منطقي other_modified = خطأ;
	لكل (صحيح j = 0; j < this_سائيز; j++) {
		خودڪار it = نمونو_منزل_with.ڳوليو((*هذا)[j]);
		إذا (it != نمونو_منزل_with.پڇاڙي()) {
			other_modified = سچ;
			(*other)[j] = with[it->ٻيو];
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::replace(ثابت dict<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> &قاعدا)
{
	replace(قاعدا, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت dict<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> &قاعدا, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(سائيز() == other->سائيز());

	إذا (قاعدا.خالي_جايون()) أرجع;

	صحيح this_سائيز = سائيز();
	منطقي other_modified = خطأ;
	لكل (صحيح i = 0; i < this_سائيز; i++) {
		خودڪار it = قاعدا.ڳوليو((*هذا)[i]);
		إذا (it != قاعدا.پڇاڙي()) {
			other_modified = سچ;
			(*other)[i] = it->ٻيو;
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::replace(ثابت std::map<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> &قاعدا)
{
	replace(قاعدا, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت std::map<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> &قاعدا, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(سائيز() == other->سائيز());

	إذا (قاعدا.خالي_جايون()) أرجع;

	صحيح this_سائيز = سائيز();
	منطقي other_modified = خطأ;
	لكل (صحيح i = 0; i < this_سائيز; i++) {
		خودڪار it = قاعدا.ڳوليو((*هذا)[i]);
		إذا (it != قاعدا.پڇاڙي()) {
			other_modified = سچ;
			(*other)[i] = it->ٻيو;
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &نمونو)
{
	remove2(نمونو, عدم);
}

فراغ RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &نمونو, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *هذا;
	tmp.remove2(نمونو, other);
}

فراغ RTLIL::SigSpec::remove2(ثابت RTLIL::SigSpec &نمونو, RTLIL::SigSpec *other)
{
	unpack();
	إذا (other != عدم) {
		log_assert(سائيز() == other->سائيز());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<Sig_ٻٽو_انگ> نمونو_بٽ;
	نمونو_بٽ.reserve(نمونو.سائيز());
	لكل (خودڪار &ٻٽو_انگ : نمونو)
		إذا (ٻٽو_انگ.wire != عدم)
			نمونو_بٽ.وجهو(ٻٽو_انگ);

	// Compact in-place to avoid O(n^2) erase operations
	سائيز_t write_idx = 0;
	لكل (سائيز_t read_idx = 0; read_idx < بٽ_2.سائيز(); read_idx++)
	{
		إذا (!(بٽ_2[read_idx].wire != عدم && نمونو_بٽ.ڳڻپ(بٽ_2[read_idx]))) {
			إذا (write_idx != read_idx) {
				بٽ_2[write_idx] = بٽ_2[read_idx];
				إذا (other != عدم)
					other->بٽ_2[write_idx] = other->بٽ_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < بٽ_2.سائيز());
	إذا (modified) {
		بٽ_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->بٽ_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove(ثابت pool<RTLIL::Sig_ٻٽو_انگ> &نمونو)
{
	remove2(نمونو, عدم);
}

فراغ RTLIL::SigSpec::remove(ثابت pool<RTLIL::Sig_ٻٽو_انگ> &نمونو, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *هذا;
	tmp.remove2(نمونو, other);
}

فراغ RTLIL::SigSpec::remove2(ثابت pool<RTLIL::Sig_ٻٽو_انگ> &نمونو, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(سائيز() == other->سائيز());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	سائيز_t write_idx = 0;
	لكل (سائيز_t read_idx = 0; read_idx < بٽ_2.سائيز(); read_idx++) {
		إذا (!(بٽ_2[read_idx].wire != عدم && نمونو.ڳڻپ(بٽ_2[read_idx]))) {
			إذا (write_idx != read_idx) {
				بٽ_2[write_idx] = بٽ_2[read_idx];
				إذا (other != عدم)
					other->بٽ_2[write_idx] = other->بٽ_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < بٽ_2.سائيز());
	إذا (modified) {
		بٽ_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->بٽ_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove2(ثابت std::سيٽ<RTLIL::Sig_ٻٽو_انگ> &نمونو, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(سائيز() == other->سائيز());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	سائيز_t write_idx = 0;
	لكل (سائيز_t read_idx = 0; read_idx < بٽ_2.سائيز(); read_idx++) {
		إذا (!(بٽ_2[read_idx].wire != عدم && نمونو.ڳڻپ(بٽ_2[read_idx]))) {
			إذا (write_idx != read_idx) {
				بٽ_2[write_idx] = بٽ_2[read_idx];
				إذا (other != عدم)
					other->بٽ_2[write_idx] = other->بٽ_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < بٽ_2.سائيز());
	إذا (modified) {
		بٽ_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->بٽ_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove2(ثابت pool<RTLIL::Wire*> &نمونو, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(سائيز() == other->سائيز());
		other->unpack();
	}

	منطقي modified = خطأ;
	منطقي other_modified = خطأ;
	لكل (صحيح i = Get_سائيز(بٽ_2) - 1; i >= 0; i--) {
		إذا (بٽ_2[i].wire != عدم && نمونو.ڳڻپ(بٽ_2[i].wire)) {
			modified = سچ;
			بٽ_2.erase(بٽ_2.begin() + i);
			إذا (other != عدم) {
				other_modified = سچ;
				other->بٽ_2.erase(other->بٽ_2.begin() + i);
			}
		}
	}

	إذا (modified) {
		hash_.clear();
		try_repack();
	}
	إذا (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::ڪڍو_2(ثابت RTLIL::SigSpec &نمونو, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == عدم || سائيز() == other->سائيز());

	RTLIL::SigSpec ret;
	std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> بٽ_ميلاپ = منزل_sigbit_ويڪٽر();

	لكل (خودڪار& نمونو_chunk : نمونو.chunks()) {
		إذا (other) {
			std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> بٽ_other = other->منزل_sigbit_ويڪٽر();
			صحيح i = 0;
			لكل (ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ : بٽ_ميلاپ) {
				إذا (ٻٽو_انگ.wire &&
					ٻٽو_انگ.wire == نمونو_chunk.wire &&
					ٻٽو_انگ.وٿي >= نمونو_chunk.وٿي &&
					ٻٽو_انگ.وٿي < نمونو_chunk.وٿي + نمونو_chunk.ويڪر)
					ret.append(بٽ_other[i]);
				++i;
			}
		} وإلا {
			لكل (ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ : بٽ_ميلاپ)
				إذا (ٻٽو_انگ.wire &&
					ٻٽو_انگ.wire == نمونو_chunk.wire &&
					ٻٽو_انگ.وٿي >= نمونو_chunk.وٿي &&
					ٻٽو_انگ.وٿي < نمونو_chunk.وٿي + نمونو_chunk.ويڪر)
					ret.append(ٻٽو_انگ);
		}
	}

	ret.check();
	أرجع ret;
}

RTLIL::SigSpec RTLIL::SigSpec::ڪڍو_2(ثابت pool<RTLIL::Sig_ٻٽو_انگ> &نمونو, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == عدم || سائيز() == other->سائيز());

	std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> بٽ_ميلاپ = منزل_sigbit_ويڪٽر();
	RTLIL::SigSpec ret;

	إذا (other) {
		std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> بٽ_other = other->منزل_sigbit_ويڪٽر();
		صحيح i = 0;
		لكل (ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ : بٽ_ميلاپ) {
			إذا (ٻٽو_انگ.wire && نمونو.ڳڻپ(ٻٽو_انگ))
				ret.append(بٽ_other[i]);
			++i;
		}
	} وإلا {
		لكل (ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ : بٽ_ميلاپ)
			إذا (ٻٽو_انگ.wire && نمونو.ڳڻپ(ٻٽو_انگ))
				ret.append(ٻٽو_انگ);
	}

	ret.check();
	أرجع ret;
}

فراغ RTLIL::SigSpec::replace(صحيح وٿي, ثابت RTLIL::SigSpec &with)
{
	إذا (with.سائيز() == 0)
		أرجع;

	unpack();

	log_assert(وٿي >= 0);
	log_assert(with.سائيز() >= 0);
	log_assert(وٿي+with.سائيز() <= سائيز());

	صحيح i = 0;
	لكل (ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ : with.بٽ()) {
		بٽ_2.at(وٿي + i) = ٻٽو_انگ;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

فراغ RTLIL::SigSpec::remove_const()
{
	إذا (rep_ == CHUNK)
	{
		إذا (chunk_.wire == عدم) {
			chunk_.~SigChunk();
			init_خالي_جايون_بٽ();
			hash_.clear();
		}
	}
	وإلا
	{
		std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> new_بٽ;
		new_بٽ.reserve(بٽ_2.سائيز());
		لكل (خودڪار &ٻٽو_انگ : بٽ_2)
			إذا (ٻٽو_انگ.wire != عدم)
				new_بٽ.ڌڪو_پٺ(ٻٽو_انگ);
		إذا (Get_سائيز(new_بٽ) != Get_سائيز(بٽ_2)) {
			بٽ_2.مٽا_سٽا(new_بٽ);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

فراغ RTLIL::SigSpec::remove(صحيح وٿي, صحيح ڊيگهائي)
{
	إذا (ڊيگهائي == 0)
		أرجع;

	unpack();

	log_assert(وٿي >= 0);
	log_assert(ڊيگهائي >= 0);
	log_assert(وٿي + ڊيگهائي <= سائيز());

	بٽ_2.erase(بٽ_2.begin() + وٿي, بٽ_2.begin() + وٿي + ڊيگهائي);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::ڪڍو_2(صحيح وٿي, صحيح ڊيگهائي) ثابت
{
	log_assert(وٿي >= 0);
	log_assert(ڊيگهائي >= 0);
	log_assert(وٿي + ڊيگهائي <= سائيز());

	std::ويڪٽر<Sig_ٻٽو_انگ> extracted;
	Sig_ٻٽو_انگ پهريون;
	منطقي is_packing = سچ;
	لكل (صحيح i = وٿي; i < وٿي + ڊيگهائي; i++) {
		منطقي was_packing_before = is_packing;
		Sig_ٻٽو_انگ ٻٽو_انگ = (*هذا)[i];
		إذا (i == وٿي) {
			پهريون = ٻٽو_انگ;
			إذا (!ٻٽو_انگ.wire)
				is_packing = خطأ;
		} وإلا {
			إذا (ٻٽو_انگ.wire != پهريون.wire)
				is_packing = خطأ;
			إذا (ٻٽو_انگ.wire)
				إذا (ٻٽو_انگ.وٿي != پهريون.وٿي + (i - وٿي))
					is_packing = خطأ;
		}
		إذا (was_packing_before && !is_packing)
			لكل (صحيح j = وٿي; j < i; j++)
				extracted.ڌڪو_پٺ((*هذا)[j]);
		إذا (!is_packing)
			extracted.ڌڪو_پٺ((*هذا)[i]);
	}
	إذا (is_packing)
		أرجع SigChunk(پهريون.wire, پهريون.وٿي, ڊيگهائي);

	أرجع extracted;
}

فراغ RTLIL::SigSpec::rewrite_wires(std::function<فراغ(RTLIL::Wire*& wire)> rewrite)
{
	إذا (rep_ == CHUNK) {
		إذا (chunk_.wire != خالي_اشارو)
			rewrite(chunk_.wire);
		أرجع;
	}

	std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> new_بٽ;
	لكل (ثابت RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		إذا (c.wire != خالي_اشارو)
			rewrite(c.wire);
		لكل (صحيح i = 0; i < c.ويڪر; i++)
			new_بٽ.emplace_پٺ(c, i);
	}
	بٽ_2 = std::move(new_بٽ);
	hash_.clear();
}

فراغ RTLIL::SigSpec::append(ثابت RTLIL::SigSpec &إشارة)
{
	إذا (إشارة.سائيز() == 0)
		أرجع;

	إذا (سائيز() == 0) {
		*هذا = إشارة;
		أرجع;
	}

	hash_.clear();
	إذا (rep_ == CHUNK && إشارة.rep_ == CHUNK && chunk_.wire == إشارة.chunk_.wire) {
		إذا (chunk_.wire == عدم) {
			chunk_.معلومات.وجهو(chunk_.معلومات.پڇاڙي(), إشارة.chunk_.معلومات.begin(), إشارة.chunk_.معلومات.پڇاڙي());
			chunk_.ويڪر = Get_سائيز(chunk_.معلومات);
			أرجع;
		}
		إذا (chunk_.وٿي + chunk_.ويڪر == إشارة.chunk_.وٿي) {
			chunk_.ويڪر += إشارة.chunk_.ويڪر;
			أرجع;
		}
	}

	unpack();
	لكل (ثابت Sig_ٻٽو_انگ &ٻٽو_انگ : إشارة.بٽ())
		بٽ_2.ڌڪو_پٺ(ٻٽو_انگ);
	check();
}

فراغ RTLIL::SigSpec::append(ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ)
{
	hash_.clear();

	إذا (سائيز() == 0) {
		destroy();
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(ٻٽو_انگ);
		أرجع;
	}

	إذا (rep_ == CHUNK && chunk_.wire == ٻٽو_انگ.wire) {
		إذا (chunk_.wire == عدم) {
			chunk_.معلومات.ڌڪو_پٺ(ٻٽو_انگ.معلومات);
			chunk_.ويڪر++;
			أرجع;
		}
		إذا (chunk_.وٿي + chunk_.ويڪر == ٻٽو_انگ.وٿي) {
			chunk_.ويڪر++;
			أرجع;
		}
	}

	unpack();

	بٽ_2.ڌڪو_پٺ(ٻٽو_انگ);
	check();
}

فراغ RTLIL::SigSpec::extend_u0(صحيح ويڪر, منطقي is_signed)
{
	إذا (سائيز() > ويڪر)
		remove(ويڪر, سائيز() - ويڪر);

	إذا (سائيز() < ويڪر) {
		RTLIL::Sig_ٻٽو_انگ padding = سائيز() > 0 ? (*هذا)[سائيز() - 1] : RTLIL::State::Sx;
		إذا (!is_signed)
			padding = RTLIL::State::S0;
		طالما (سائيز() < ويڪر)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(صحيح انگ) ثابت
{
	RTLIL::SigSpec sig;
	لكل (صحيح i = 0; i < انگ; i++)
		sig.append(*هذا);
	أرجع sig;
}

#جيڪڏهن_وصف_نه_ٿيل NDEBUG
فراغ RTLIL::SigSpec::check(ثابت Module *mod) ثابت
{
	إذا (rep_ == CHUNK)
	{
		log_assert(chunk_.ويڪر != 0);
		إذا (chunk_.wire == عدم) {
			log_assert(chunk_.وٿي == 0);
			log_assert(chunk_.معلومات.سائيز() == (سائيز_t)chunk_.ويڪر);
		} وإلا {
			log_assert(chunk_.وٿي >= 0);
			log_assert(chunk_.ويڪر >= 0);
			log_assert(chunk_.وٿي + chunk_.ويڪر <= chunk_.wire->ويڪر);
			log_assert(chunk_.معلومات.سائيز() == 0);
			إذا (mod != خالي_اشارو)
				log_assert(chunk_.wire->module == mod);
		}
	}
	وإلا إذا (سائيز() <= 64)
	{
		إذا (mod != خالي_اشارو) {
			لكل (ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ : بٽ_2)
				إذا (ٻٽو_انگ.wire != خالي_اشارو)
					log_assert(ٻٽو_انگ.wire->module == mod);
		}
	}
}
#جيڪڏهن_پورو

منطقي RTLIL::SigSpec::عامل <(ثابت RTLIL::SigSpec &other) ثابت
{
	إذا (هذا == &other)
		أرجع خطأ;

	إذا (سائيز() != other.سائيز())
		أرجع سائيز() < other.سائيز();

	خودڪار other_it = other.chunks().begin();
	لكل (ثابت SigChunk &c : chunks()) {
		إذا (c != *other_it)
			أرجع c < *other_it;
		++other_it;
	}

	أرجع خطأ;
}

منطقي RTLIL::SigSpec::عامل ==(ثابت RTLIL::SigSpec &other) ثابت
{
	إذا (هذا == &other)
		أرجع سچ;

	إذا (سائيز() != other.سائيز())
		أرجع خطأ;

	خودڪار other_it = other.chunks().begin();
	لكل (ثابت SigChunk &c : chunks()) {
		إذا (c != *other_it)
			أرجع خطأ;
		++other_it;
	}

	أرجع سچ;
}

منطقي RTLIL::SigSpec::is_wire() ثابت
{
	Chunks cs = chunks();
	خودڪار it = cs.begin();
	إذا (it == cs.پڇاڙي())
		أرجع خطأ;
	ثابت RTLIL::SigChunk &chunk = *it;
	أرجع chunk.wire && chunk.wire->ويڪر == سائيز() && ++it == cs.پڇاڙي();
}

منطقي RTLIL::SigSpec::is_chunk() ثابت
{
	Chunks cs = chunks();
	خودڪار it = cs.begin();
	إذا (it == cs.پڇاڙي())
		أرجع خطأ;
	أرجع ++it == cs.پڇاڙي();
}

منطقي RTLIL::SigSpec::معلوم_driver() ثابت
{
	لكل (خودڪار &chunk : chunks())
		إذا (chunk.is_wire() && !chunk.wire->معلوم_driver())
			أرجع خطأ;
	أرجع سچ;
}

منطقي RTLIL::SigSpec::is_fully_const() ثابت
{
	لكل (خودڪار &chunk : chunks())
		إذا (chunk.ويڪر > 0 && chunk.wire != عدم)
			أرجع خطأ;
	أرجع سچ;
}

منطقي RTLIL::SigSpec::is_fully_zero() ثابت
{
	لكل (خودڪار &chunk : chunks()) {
		إذا (chunk.ويڪر > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.معلومات)
			إذا (d != RTLIL::State::S0)
				أرجع خطأ;
	}
	أرجع سچ;
}

منطقي RTLIL::SigSpec::is_fully_ones() ثابت
{
	لكل (خودڪار &chunk : chunks()) {
		إذا (chunk.ويڪر > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.معلومات)
			إذا (d != RTLIL::State::S1)
				أرجع خطأ;
	}
	أرجع سچ;
}

منطقي RTLIL::SigSpec::is_fully_def() ثابت
{
	لكل (خودڪار &chunk : chunks()) {
		إذا (chunk.ويڪر > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.معلومات)
			إذا (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				أرجع خطأ;
	}
	أرجع سچ;
}

منطقي RTLIL::SigSpec::is_fully_undef() ثابت
{
	لكل (خودڪار &chunk : chunks()) {
		إذا (chunk.ويڪر > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.معلومات)
			إذا (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				أرجع خطأ;
	}
	أرجع سچ;
}

منطقي RTLIL::SigSpec::has_const() ثابت
{
	لكل (خودڪار &chunk : chunks())
		إذا (chunk.ويڪر > 0 && chunk.wire == عدم)
			أرجع سچ;
	أرجع خطأ;
}

منطقي RTLIL::SigSpec::has_const(State state) ثابت
{
	لكل (خودڪار &chunk : chunks())
		إذا (chunk.ويڪر > 0 && chunk.wire == عدم && std::ڳوليو(chunk.معلومات.begin(), chunk.معلومات.پڇاڙي(), state) != chunk.معلومات.پڇاڙي())
			أرجع سچ;
	أرجع خطأ;
}


منطقي RTLIL::SigSpec::has_marked_بٽ() ثابت
{
	لكل (خودڪار &chunk : chunks())
		إذا (chunk.ويڪر > 0 && chunk.wire == عدم) {
			لكل (RTLIL::State d : chunk.معلومات)
				إذا (d == RTLIL::State::Sm)
					أرجع سچ;
		}
	أرجع خطأ;
}

منطقي RTLIL::SigSpec::is_onehot(صحيح *pos) ثابت
{
	إذا (std::optional<RTLIL::Const> c = try_as_const())
		أرجع c->is_onehot(pos);
	أرجع خطأ;
}

منطقي RTLIL::SigSpec::as_bool() ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_قدر());
	أرجع c->as_bool();
}

صحيح RTLIL::SigSpec::as_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_قدر());
	أرجع c->as_int(is_signed);
}

منطقي RTLIL::SigSpec::convertible_منزل_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	إذا (!c.has_قدر())
		أرجع خطأ;
	أرجع c->convertible_منزل_int(is_signed);
}

std::optional<صحيح> RTLIL::SigSpec::try_as_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	إذا (!c.has_قدر())
		أرجع std::nullopt;
	أرجع c->try_as_int(is_signed);
}

صحيح RTLIL::SigSpec::as_int_saturating(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_قدر());
	أرجع c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ثابت
{
	std::string str;
	str.reserve(سائيز());
	std::ويڪٽر<RTLIL::SigChunk> chunks = *هذا;
	لكل (سائيز_t i = chunks.سائيز(); i > 0; i--) {
		ثابت RTLIL::SigChunk &chunk = chunks[i-1];
		إذا (chunk.wire != عدم)
			str.append(chunk.ويڪر, '?');
		وإلا
			str += RTLIL::Const(chunk.معلومات).as_string();
	}
	أرجع str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ثابت
{
	Chunks cs = chunks();
	خودڪار it = cs.begin();
	إذا (it == cs.پڇاڙي())
		أرجع RTLIL::Const();
	SigChunk chunk = *it;
	إذا (chunk.wire != عدم || ++it != cs.پڇاڙي())
		أرجع std::nullopt;
	أرجع RTLIL::Const(std::move(chunk.معلومات));
}

RTLIL::Const RTLIL::SigSpec::as_const() ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_قدر());
	أرجع *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ثابت
{
	Chunks cs = chunks();
	خودڪار it = cs.begin();
	log_assert(it != cs.پڇاڙي());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.پڇاڙي() && chunk.wire && chunk.wire->ويڪر == سائيز());
	أرجع chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ثابت
{
	Chunks cs = chunks();
	خودڪار it = cs.begin();
	log_assert(it != cs.پڇاڙي());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.پڇاڙي());
	أرجع chunk;
}

RTLIL::Sig_ٻٽو_انگ RTLIL::SigSpec::as_ٻٽو_انگ() ثابت
{
	أرجع RTLIL::Sig_ٻٽو_انگ(*هذا);
}

منطقي RTLIL::SigSpec::ميلاپ(ثابت حرف* نمونو) ثابت
{
	صحيح نمونو_len = لڙي_ڊيگهه(نمونو);
	log_assert(نمونو_len == سائيز());

	لكل (صحيح i = 0; i < نمونو_len; ++i) {
		حرف ch = نمونو[i];
		إذا (ch == ' ')
			استمر;
		RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ = (*هذا)[نمونو_len - 1 - i];
		إذا (ch == '*') {
			إذا (ٻٽو_انگ != State::Sz && ٻٽو_انگ != State::Sx)
				أرجع خطأ;
			استمر;
		}
		إذا (ch == '0') {
			إذا (ٻٽو_انگ != State::S0)
				أرجع خطأ;
		} وإلا
		إذا (ch == '1') {
			إذا (ٻٽو_انگ != State::S1)
				أرجع خطأ;
		} وإلا
			log_abort();
	}

	أرجع سچ;
}

std::سيٽ<RTLIL::Sig_ٻٽو_انگ> RTLIL::SigSpec::منزل_sigbit_سيٽ() ثابت
{
	std::سيٽ<RTLIL::Sig_ٻٽو_انگ> sigbits;
	لكل (خودڪار &c : chunks())
		لكل (صحيح i = 0; i < c.ويڪر; i++)
			sigbits.وجهو(RTLIL::Sig_ٻٽو_انگ(c, i));
	أرجع sigbits;
}

pool<RTLIL::Sig_ٻٽو_انگ> RTLIL::SigSpec::منزل_sigbit_pool() ثابت
{
	pool<RTLIL::Sig_ٻٽو_انگ> sigbits;
	sigbits.reserve(سائيز());
	لكل (خودڪار &c : chunks())
		لكل (صحيح i = 0; i < c.ويڪر; i++)
			sigbits.وجهو(RTLIL::Sig_ٻٽو_انگ(c, i));
	أرجع sigbits;
}

std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> RTLIL::SigSpec::منزل_sigbit_ويڪٽر() ثابت
{
	std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> نتيجو;
	نتيجو.reserve(سائيز());
	لكل (Sig_ٻٽو_انگ ٻٽو_انگ : *هذا)
		نتيجو.ڌڪو_پٺ(ٻٽو_انگ);
	أرجع نتيجو;
}

std::map<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> RTLIL::SigSpec::منزل_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت
{
	صحيح this_سائيز = سائيز();
	log_assert(this_سائيز == other.سائيز());

	std::map<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> new_map;
	لكل (صحيح i = 0; i < this_سائيز; i++)
		new_map[(*هذا)[i]] = other[i];

	أرجع new_map;
}

dict<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> RTLIL::SigSpec::منزل_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت
{
	صحيح this_سائيز = سائيز();
	log_assert(this_سائيز == other.سائيز());

	dict<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> new_map;
	new_map.reserve(this_سائيز);
	لكل (صحيح i = 0; i < this_سائيز; i++)
		new_map[(*هذا)[i]] = other[i];

	أرجع new_map;
}

ساكن فراغ sigspec_parse_ورهايو(std::ويڪٽر<std::string> &ٽوڪن, ثابت std::string &متن, حرف sep)
{
	سائيز_t شروعات = 0, پڇاڙي = 0;
	طالما ((پڇاڙي = متن.ڳوليو(sep, شروعات)) != std::string::npos) {
		ٽوڪن.ڌڪو_پٺ(متن.substr(شروعات, پڇاڙي - شروعات));
		شروعات = پڇاڙي + 1;
	}
	ٽوڪن.ڌڪو_پٺ(متن.substr(شروعات));
}

ساكن RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ثابت std::string &netname)
{
	أرجع module->wire(netname);
}

منطقي RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::ويڪٽر<std::string> ٽوڪن;
	sigspec_parse_ورهايو(ٽوڪن, str, ',');

	sig = RTLIL::SigSpec();
	لكل (صحيح tokidx = صحيح(ٽوڪن.سائيز())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = ٽوڪن[tokidx];
		std::string indices;

		إذا (netname.سائيز() == 0)
			استمر;

		إذا (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			خودڪار ast = p.const2ast(netname);
			إذا (ast == خالي_اشارو)
				أرجع خطأ;
			sig.append(RTLIL::Const(ast->بٽ));
			استمر;
		}

		إذا (module == عدم)
			أرجع خطأ;

		إذا (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		إذا (sigspec_parse_wire(module, netname) == خالي_اشارو) {
			سائيز_t indices_pos = netname.سائيز()-1;
			إذا (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				طالما (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				إذا (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					طالما (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				إذا (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		إذا (wire == خالي_اشارو)
			أرجع خطأ;

		إذا (!indices.خالي_جايون()) {
			std::ويڪٽر<std::string> اشاريو_ٽوڪن;
			sigspec_parse_ورهايو(اشاريو_ٽوڪن, indices.substr(1, indices.سائيز()-2), ':');
			إذا (اشاريو_ٽوڪن.سائيز() == 1) {
				صحيح a = atoi(اشاريو_ٽوڪن.at(0).c_str());
				إذا (a < 0 || a >= wire->ويڪر)
					أرجع خطأ;
				sig.append(RTLIL::SigSpec(wire, a));
			} وإلا {
				صحيح a = atoi(اشاريو_ٽوڪن.at(0).c_str());
				صحيح b = atoi(اشاريو_ٽوڪن.at(1).c_str());
				إذا (a > b) {
					صحيح tmp = a;
					a = b, b = tmp;
				}
				إذا (a < 0 || a >= wire->ويڪر)
					أرجع خطأ;
				إذا (b < 0 || b >= wire->ويڪر)
					أرجع خطأ;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} وإلا
			sig.append(wire);
	}

	أرجع سچ;
}

منطقي RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	إذا (str.خالي_جايون() || str[0] != '@')
		أرجع parse(sig, module, str);

	IdString sel_name = design->twines().ڳوليو(RTLIL::escape_id(str.substr(1)));
	إذا (sel_name == IdString::Null || design->selection_vars.ڳڻپ(sel_name) == 0)
		أرجع خطأ;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	لكل (خودڪار &it : module->wires_)
		إذا (sel.selected_member(module->name, it.ٻيو->name))
			sig.append(it.ٻيو);

	أرجع سچ;
}

منطقي RTLIL::SigSpec::parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	إذا (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.سائيز());
		أرجع سچ;
	}

	إذا (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.سائيز());
		أرجع سچ;
	}

	إذا (lhs.is_chunk()) {
		حرف *p = (حرف*)str.c_str(), *endptr;
		ڊگهو صحيح val = strtol(p, &endptr, 10);
		إذا (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.سائيز());
			أرجع سچ;
		}
	}

	إذا (!parse(sig, module, str))
		أرجع خطأ;
	إذا (sig.سائيز() > lhs.سائيز())
		sig.remove(lhs.سائيز(), sig.سائيز() - lhs.سائيز());
	أرجع سچ;
}

RTLIL::SigSpec::عامل std::ويڪٽر<RTLIL::SigChunk>() ثابت
{
	std::ويڪٽر<RTLIL::SigChunk> نتيجو;
	لكل (ثابت RTLIL::SigChunk &c : chunks())
		نتيجو.ڌڪو_پٺ(c);
	أرجع نتيجو;
}

RTLIL::Case_قاعدو::~Case_قاعدو()
{
	لكل (خودڪار it = switches.begin(); it != switches.پڇاڙي(); it++)
		ميٽيو *it;
}

منطقي RTLIL::Case_قاعدو::خالي_جايون() ثابت
{
	أرجع actions.خالي_جايون() && switches.خالي_جايون();
}

RTLIL::Case_قاعدو *RTLIL::Case_قاعدو::clone() ثابت
{
	RTLIL::Case_قاعدو *new_caserule = جديد RTLIL::Case_قاعدو;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->خاصيتون = خاصيتون;
	لكل (خودڪار &it : switches)
		new_caserule->switches.ڌڪو_پٺ(it->clone());
	أرجع new_caserule;
}

RTLIL::Switch_قاعدو::~Switch_قاعدو()
{
	لكل (خودڪار it = cases.begin(); it != cases.پڇاڙي(); it++)
		ميٽيو *it;
}

منطقي RTLIL::Switch_قاعدو::خالي_جايون() ثابت
{
	أرجع cases.خالي_جايون();
}

RTLIL::Switch_قاعدو *RTLIL::Switch_قاعدو::clone() ثابت
{
	RTLIL::Switch_قاعدو *new_switchrule = جديد RTLIL::Switch_قاعدو;
	new_switchrule->إشارة = إشارة;
	new_switchrule->خاصيتون = خاصيتون;
	لكل (خودڪار &it : cases)
		new_switchrule->cases.ڌڪو_پٺ(it->clone());
	أرجع new_switchrule;

}

RTLIL::Sync_قاعدو *RTLIL::Sync_قاعدو::clone() ثابت
{
	RTLIL::Sync_قاعدو *new_syncrule = جديد RTLIL::Sync_قاعدو;
	new_syncrule->type = type;
	new_syncrule->إشارة = إشارة;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	أرجع new_syncrule;
}

RTLIL::Process::~Process()
{
	لكل (خودڪار it = syncs.begin(); it != syncs.پڇاڙي(); it++)
		ميٽيو *it;
}

RTLIL::Process *RTLIL::Process::clone() ثابت
{
	RTLIL::Process *new_proc = جديد RTLIL::Process;

	new_proc->خاصيتون = خاصيتون;

	RTLIL::Case_قاعدو *rc_ptr = پاڙ_case.clone();
	new_proc->پاڙ_case = *rc_ptr;
	rc_ptr->switches.clear();
	ميٽيو rc_ptr;

	لكل (خودڪار &it : syncs)
		new_proc->syncs.ڌڪو_پٺ(it->clone());

	أرجع new_proc;
}

RTLIL::يادگيري::~يادگيري()
{
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	RTLIL::يادگيري::get_all_memorys()->erase(hashidx_);
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
ساكن std::map<بنانشان صحيح, RTLIL::يادگيري*> all_memorys;
std::map<بنانشان صحيح, RTLIL::يادگيري*> *RTLIL::يادگيري::get_all_memorys(فراغ)
{
	أرجع &all_memorys;
}
#جيڪڏهن_پورو
YOSYS_NAMESPACE_پڇاڙي
