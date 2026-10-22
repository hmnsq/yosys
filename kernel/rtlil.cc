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
		لكل (ناپ_t i = 0; i < StaticCellTypes::builder.گنتی; i++) {
			خودکار &cell = StaticCellTypes::builder.cells[i];
			إذا (cell.features.is_ff)
				r.داخل_کرو(cell.type);
		}
		أرجع r;
	}();
	أرجع res;
}

#تعریف_کرو check(condition) log_assert(condition && "malformed Const union")

ثابت Const::bitvectype& Const::get_بٹ() ثابت {
	check(is_بٹ());
	أرجع *get_if_بٹ();
}

ثابت std::string& Const::get_str() ثابت {
	check(is_str());
	أرجع *get_if_str();
}

Const::bitvectype& Const::get_بٹ() {
	check(is_بٹ());
	أرجع *get_if_بٹ();
}

std::string& Const::get_str() {
	check(is_str());
	أرجع *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	جديد ((فراغ*)&str_) std::string(std::move(str));
	ٹیگ = backing_ٹیگ::string;
}

RTLIL::Const::Const(لما لما val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	حرف بائٹاں[] = {
		(حرف)(val >> 24), (حرف)(val >> 16), (حرف)(val >> 8), (حرف)val
	};
	جديد ((فراغ*)&str_) std::string(بائٹاں, 4);
	ٹیگ = backing_ٹیگ::string;
}

RTLIL::Const::Const(لما لما val, صحيح چوڑائی)
{
	log_assert(چوڑائی >= 0 && چوڑائی < RTLIL::چوڑائی_حد_بندی);
	flags = RTLIL::CONST_FLAG_NONE;
	إذا ((چوڑائی & 7) == 0) {
		جديد ((فراغ*)&str_) std::string();
		ٹیگ = backing_ٹیگ::string;
		std::string& str = get_str();
		صحيح بائٹاں = چوڑائی >> 3;
		نشانیوالا حرف sign_byte = val < 0 ? -1 : 0;
		str.resize(بائٹاں, sign_byte);
		بائٹاں = std::min<صحيح>(بائٹاں, حجم(val));
		لكل (صحيح i = 0; i < بائٹاں; i++) {
			str[str.ناپ() - 1 - i] = val;
			val = val >> 8;
		}
		أرجع;
	}

	جديد ((فراغ*)&بٹ_2) bitvectype();
	ٹیگ = backing_ٹیگ::بٹ;
	bitvectype& bv = get_بٹ();
	bv.reserve(چوڑائی);
	لكل (صحيح i = 0; i < چوڑائی; i++) {
		bv.ڈالو_پٹھ((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State ثنائی_ہندسہ, صحيح چوڑائی)
{
	log_assert(چوڑائی >= 0 && چوڑائی < RTLIL::چوڑائی_حد_بندی);
	flags = RTLIL::CONST_FLAG_NONE;
	جديد ((فراغ*)&بٹ_2) bitvectype();
	ٹیگ = backing_ٹیگ::بٹ;
	bitvectype& bv = get_بٹ();
	bv.reserve(چوڑائی);
	لكل (صحيح i = 0; i < چوڑائی; i++)
		bv.ڈالو_پٹھ(ثنائی_ہندسہ);
}

RTLIL::Const::Const(ثابت std::سمتیہ<منطقي> &بٹ)
{
	flags = RTLIL::CONST_FLAG_NONE;
	جديد ((فراغ*)&بٹ_2) bitvectype();
	ٹیگ = backing_ٹیگ::بٹ;
	bitvectype& bv = get_بٹ();
	bv.reserve(بٹ.ناپ());
	لكل (ثابت خودکار &b : بٹ)
		bv.emplace_پٹھ(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ثابت RTLIL::Const &other) {
	ٹیگ = other.ٹیگ;
	flags = other.flags;
	إذا (is_str())
		جديد ((فراغ*)&str_) std::string(other.get_str());
	وإلا إذا (is_بٹ())
		جديد ((فراغ*)&بٹ_2) bitvectype(other.get_بٹ());
	وإلا
		check(خطأ);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	ٹیگ = other.ٹیگ;
	flags = other.flags;
	إذا (is_str())
		جديد ((فراغ*)&str_) std::string(std::move(other.get_str()));
	وإلا إذا (is_بٹ())
		جديد ((فراغ*)&بٹ_2) bitvectype(std::move(other.get_بٹ()));
	وإلا
		check(خطأ);
}

RTLIL::Const &RTLIL::Const::عامل =(ثابت RTLIL::Const &other) {
	flags = other.flags;
	إذا (other.is_str()) {
		إذا (!is_str()) {
			// sketchy zone
			check(is_بٹ());
			بٹ_2.~bitvectype();
			(فراغ)جديد ((فراغ*)&str_) std::string();
		}
		ٹیگ = other.ٹیگ;
		get_str() = other.get_str();
	} وإلا إذا (other.is_بٹ()) {
		إذا (!is_بٹ()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(فراغ)جديد ((فراغ*)&بٹ_2) bitvectype();
		}
		ٹیگ = other.ٹیگ;
		get_بٹ() = other.get_بٹ();
	} وإلا {
		check(خطأ);
	}
	أرجع *هذا;
}

RTLIL::Const::~Const() {
	إذا (is_بٹ())
		بٹ_2.~bitvectype();
	وإلا إذا (is_str())
		str_.~string();
	وإلا
		check(خطأ);
}

منطقي RTLIL::Const::عامل<(ثابت RTLIL::Const &other) ثابت
{
	إذا (ناپ() != other.ناپ())
		أرجع ناپ() < other.ناپ();

	لكل (صحيح i = 0; i < ناپ(); i++)
		إذا ((*هذا)[i] != other[i])
			أرجع (*هذا)[i] < other[i];

	أرجع خطأ;
}

منطقي RTLIL::Const::عامل ==(ثابت RTLIL::Const &other) ثابت
{
	إذا (is_str() && other.is_str())
		أرجع get_str() == other.get_str();
	إذا (is_بٹ() && other.is_بٹ())
		أرجع get_بٹ() == other.get_بٹ();

	إذا (ناپ() != other.ناپ())
		أرجع خطأ;

	لكل (صحيح i = 0; i < ناپ(); i++)
	إذا ((*هذا)[i] != other[i])
		أرجع خطأ;

	أرجع سچ;
}

منطقي RTLIL::Const::عامل !=(ثابت RTLIL::Const &other) ثابت
{
	أرجع !(*هذا == other);
}

std::سمتیہ<RTLIL::State>& RTLIL::Const::بٹ_اندرونی()
{
	bitvectorize_اندرونی();
	أرجع get_بٹ();
}

std::سمتیہ<RTLIL::State> RTLIL::Const::منزل_بٹ() ثابت
{
	std::سمتیہ<State> v;
	لكل (خودکار ثنائی_ہندسہ : *هذا)
		v.ڈالو_پٹھ(ثنائی_ہندسہ);
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

	ثابت bitvectype& bv = get_بٹ();
	لكل (ناپ_t i = 0; i < bv.ناپ(); i++)
		إذا (bv[i] == State::S1)
			أرجع سچ;
	أرجع خطأ;
}

صحيح RTLIL::Const::as_int(منطقي is_signed) ثابت
{
	int32_t ret = 0;
	إذا (ثابت std::string *s = get_if_str()) {
		صحيح ناپ = Get_ناپ(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		لكل (صحيح i = std::min(4, ناپ); i > 0; i--)
			ret |= static_cast<بنانشانی حرف>((*s)[ناپ - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		إذا (is_signed && ناپ > 0 && ناپ < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << ناپ*8;
		أرجع ret;
	}

	ثابت bitvectype& bv = get_بٹ();
	صحيح significant_بٹ = std::min(Get_ناپ(bv), 32);
	لكل (صحيح i = 0; i < significant_بٹ; i++)
		إذا (bv[i] == State::S1)
			ret |= 1 << i;
	إذا (is_signed && significant_بٹ > 0 && significant_بٹ < 32 && bv.پٹھ() == State::S1 )
		ret |= UINT32_MAX << significant_بٹ;
	أرجع ret;
}

منطقي RTLIL::Const::convertible_منزل_int(منطقي is_signed) ثابت
{
	خودکار ناپ = get_min_ناپ(is_signed);

	إذا (ناپ < 0)
		أرجع خطأ;

	// If it fits in 31 bits it is definitely convertible
	إذا (ناپ <= 31)
		أرجع سچ;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	إذا (ناپ == 32) {
		إذا (is_signed)
			أرجع سچ;
		أرجع (*هذا)[ناپ - 1] != State::S1;
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

		ثابت خودکار min_ناپ = get_min_ناپ(is_signed);
		log_assert(min_ناپ > 0);
		ثابت خودکار neg = (*هذا)[min_ناپ - 1];
		أرجع neg ? std::numeric_limits<صحيح>::min() : std::numeric_limits<صحيح>::max();
	}
	أرجع as_int(is_signed);
}

فراغ RTLIL::Const::ٹیگ_bare_integer_const(ثابت std::string &قدر)
{
	إذا (قدر.خالی_تھاواں() || قدر.لبھو('\'') != std::string::npos)
		أرجع;
	ناپ_t شروع = (قدر[0] == '-' || قدر[0] == '+') ? 1 : 0;
	إذا (شروع < قدر.ناپ() && std::all_دا(قدر.begin() + شروع, قدر.سرا(), ::ہندسہ_اے))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

صحيح RTLIL::Const::get_min_ناپ(منطقي is_signed) ثابت
{
	إذا (خالی_تھاواں()) أرجع 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_ثنائی_ہندسہ;
	إذا (is_signed)
		leading_ثنائی_ہندسہ = (پٹھ() == RTLIL::State::Sx) ? RTLIL::State::S0 : پٹھ();
	وإلا
		leading_ثنائی_ہندسہ = RTLIL::State::S0;

	خودکار idx = ناپ();
	طالما (idx > 0 && (*هذا)[idx -1] == leading_ثنائی_ہندسہ) {
		idx--;
	}

	// signed needs one leading bit
	إذا (is_signed && idx < ناپ()) {
		idx++;
	}
	// must be at least one bit
	أرجع (idx == 0) ? 1 : idx;
}

فراغ RTLIL::Const::compress(منطقي is_signed)
{
	خودکار idx = get_min_ناپ(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<صحيح> RTLIL::Const::as_int_compress(منطقي is_signed) ثابت
{
	أرجع try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ثابت حرف* any) ثابت
{
	صحيح sz = ناپ();
	std::string ret;
	ret.reserve(sz);
	لكل (صحيح i = sz - 1; i >= 0; --i)
		بدل ((*هذا)[i]) {
			حالة S0: ret.ڈالو_پٹھ('0'); اكسر;
			حالة S1: ret.ڈالو_پٹھ('1'); اكسر;
			حالة Sx: ret.ڈالو_پٹھ('x'); اكسر;
			حالة Sz: ret.ڈالو_پٹھ('z'); اكسر;
			حالة Sa: ret += any; اكسر;
			حالة Sm: ret.ڈالو_پٹھ('m'); اكسر;
		}
	أرجع ret;
}

RTLIL::Const RTLIL::Const::from_string(ثابت std::string &str)
{
	Const c;
	bitvectype& bv = c.get_بٹ();
	bv.reserve(str.ناپ());
	لكل (خودکار it = str.rbegin(); it != str.rend(); it++)
		بدل (*it) {
			حالة '0': bv.ڈالو_پٹھ(State::S0); اكسر;
			حالة '1': bv.ڈالو_پٹھ(State::S1); اكسر;
			حالة 'x': bv.ڈالو_پٹھ(State::Sx); اكسر;
			حالة 'z': bv.ڈالو_پٹھ(State::Sz); اكسر;
			حالة 'm': bv.ڈالو_پٹھ(State::Sm); اكسر;
			افتراضي: bv.ڈالو_پٹھ(State::Sa);
		}
	أرجع c;
}

std::string RTLIL::Const::decode_string() ثابت
{
	إذا (خودکار str = get_if_str())
		أرجع *str;

	ثابت bitvectype& bv = get_بٹ();
	ثابت صحيح n = Get_ناپ(bv);
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

صحيح RTLIL::Const::ناپ() ثابت {
	إذا (is_str())
		أرجع 8 * str_.ناپ();
	وإلا {
		check(is_بٹ());
		أرجع بٹ_2.ناپ();
	}
}

منطقي RTLIL::Const::خالی_تھاواں() ثابت {
	إذا (is_str())
		أرجع str_.خالی_تھاواں();
	وإلا {
		check(is_بٹ());
		أرجع بٹ_2.خالی_تھاواں();
	}
}

فراغ RTLIL::Const::bitvectorize_اندرونی() {
	إذا (ٹیگ == backing_ٹیگ::بٹ)
		أرجع;

	check(is_str());

	bitvectype new_بٹ;

	new_بٹ.reserve(str_.ناپ() * 8);
	لكل (صحيح i = str_.ناپ() - 1; i >= 0; i--) {
		بنانشانی حرف ch = str_[i];
		لكل (صحيح j = 0; j < 8; j++) {
			new_بٹ.ڈالو_پٹھ((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(فراغ)جديد ((فراغ*)&بٹ_2) bitvectype(std::move(new_بٹ));
		ٹیگ = backing_ٹیگ::بٹ;
	}
}

فراغ RTLIL::Const::append(ثابت RTLIL::Const &other) {
	bitvectorize_اندرونی();
	bitvectype& bv = get_بٹ();
	bv.داخل_کرو(bv.سرا(), other.begin(), other.سرا());
}

RTLIL::State RTLIL::Const::const_iterator::عامل*() ثابت {
	إذا (خودکار bv = ماپا->get_if_بٹ())
		أرجع (*bv)[idx];

	صحيح char_idx = ماپا->get_str().ناپ() - idx / 8 - 1;
	منطقي ثنائی_ہندسہ = (ماپا->get_str()[char_idx] & (1 << (idx % 8)));
	أرجع ثنائی_ہندسہ ? State::S1 : State::S0;
}

منطقي RTLIL::Const::is_fully_zero() ثابت
{
	إذا (خودکار str = get_if_str()) {
		لكل (حرف ch : *str)
			إذا (ch != 0)
				أرجع خطأ;
		أرجع سچ;
	}

	ثابت bitvectype& bv = get_بٹ();

	لكل (ثابت خودکار &ثنائی_ہندسہ : bv)
		إذا (ثنائی_ہندسہ != RTLIL::State::S0)
			أرجع خطأ;

	أرجع سچ;
}

منطقي RTLIL::Const::is_fully_ones() ثابت
{
	إذا (خودکار str = get_if_str()) {
		لكل (حرف ch : *str)
			إذا (ch != (حرف)0xff)
				أرجع خطأ;
		أرجع سچ;
	}

	ثابت bitvectype& bv = get_بٹ();
	لكل (ثابت خودکار &ثنائی_ہندسہ : bv)
		إذا (ثنائی_ہندسہ != RTLIL::State::S1)
			أرجع خطأ;

	أرجع سچ;
}

منطقي RTLIL::Const::is_fully_def() ثابت
{
	إذا (is_str())
		أرجع سچ;

	ثابت bitvectype& bv = get_بٹ();
	لكل (ثابت خودکار &ثنائی_ہندسہ : bv)
		إذا (ثنائی_ہندسہ != RTLIL::State::S0 && ثنائی_ہندسہ != RTLIL::State::S1)
			أرجع خطأ;

	أرجع سچ;
}

منطقي RTLIL::Const::is_fully_undef() ثابت
{
	إذا (خودکار str = get_if_str())
		أرجع str->خالی_تھاواں();

	ثابت bitvectype& bv = get_بٹ();
	لكل (ثابت خودکار &ثنائی_ہندسہ : bv)
		إذا (ثنائی_ہندسہ != RTLIL::State::Sx && ثنائی_ہندسہ != RTLIL::State::Sz)
			أرجع خطأ;

	أرجع سچ;
}

منطقي RTLIL::Const::is_fully_undef_x_only() ثابت
{
	إذا (خودکار str = get_if_str())
		أرجع str->خالی_تھاواں();

	ثابت bitvectype& bv = get_بٹ();
	لكل (ثابت خودکار &ثنائی_ہندسہ : bv)
		إذا (ثنائی_ہندسہ != RTLIL::State::Sx)
			أرجع خطأ;

	أرجع سچ;
}

منطقي RTLIL::Const::is_onehot(صحيح *pos) ثابت
{
	منطقي لبھیا = خطأ;
	صحيح ناپ = Get_ناپ(*هذا);
	لكل (صحيح i = 0; i < ناپ; i++) {
		State ثنائی_ہندسہ = (*هذا)[i];
		إذا (ثنائی_ہندسہ != RTLIL::State::S0 && ثنائی_ہندسہ != RTLIL::State::S1)
			أرجع خطأ;
		إذا (ثنائی_ہندسہ == RTLIL::State::S1) {
			إذا (لبھیا)
				أرجع خطأ;
			إذا (pos)
				*pos = i;
			لبھیا = سچ;
		}
	}
	أرجع لبھیا;
}

Hasher RTLIL::Const::hash_into(Hasher h) ثابت
{
	إذا (خودکار str = get_if_str())
		أرجع hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ثابت bitvectype &bv = get_بٹ();
	صحيح ناپ = Get_ناپ(bv);
	std::string packed;
	صحيح packed_ناپ = (ناپ + 7) >> 3;
	packed.resize(packed_ناپ, 0);
	لكل (صحيح bi = 0; bi < packed_ناپ; ++bi) {
		حرف ch = 0;
		صحيح سرا = std::min((bi + 1)*8, ناپ);
		لكل (صحيح i = bi*8; i < سرا; ++i) {
			RTLIL::State b = bv[i];
			إذا (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<حرف>::hash_into(ch, h);
				لكل (; i < ناپ; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(ناپ);
				أرجع h;
			}
			ch |= static_cast<صحيح>(b) << (i & 7);
		}
		packed[packed_ناپ - 1 - bi] = ch;
	}
	أرجع hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::کڈھو(صحيح وتھ, صحيح len, RTLIL::State padding) ثابت {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	لكل (صحيح i = وتھ; i < وتھ + len; i++)
		ret_bv.ڈالو_پٹھ(i < Get_ناپ(*هذا) ? (*هذا)[i] : padding);
	أرجع RTLIL::Const(ret_bv);
}
#تعریف_رد check /* check(condition) for Const */

منطقي RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ثابت
{
	أرجع گن.گنتی(id);
}

فراغ RTLIL::AttrObject::سیٹ_bool_attribute(RTLIL::IdString id, منطقي قدر)
{
	إذا (قدر)
		گن[id] = RTLIL::Const(1);
	وإلا
		گن.erase(id);
}

منطقي RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ثابت
{
	ثابت خودکار it = گن.لبھو(id);
	إذا (it == گن.سرا())
		أرجع خطأ;
	أرجع it->دوجا.as_bool();
}

فراغ RTLIL::AttrObject::سیٹ_string_attribute(RTLIL::IdString id, string قدر)
{
	إذا (قدر.خالی_تھاواں())
		گن.erase(id);
	وإلا
		گن[id] = قدر;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ثابت
{
	std::string قدر;
	ثابت خودکار it = گن.لبھو(id);
	إذا (it != گن.سرا())
		قدر = it->دوجا.decode_string();
	أرجع قدر;
}

فراغ RTLIL::AttrObject::سیٹ_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &انکڑے)
{
	string attrval;
	لكل (ثابت خودکار &s : انکڑے) {
		إذا (!attrval.خالی_تھاواں())
			attrval += "|";
		attrval += s;
	}
	سیٹ_string_attribute(id, attrval);
}

فراغ RTLIL::AttrObject::جوڑو_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &انکڑے)
{
	pool<string> union_انکڑے = get_strpool_attribute(id);
	union_انکڑے.داخل_کرو(انکڑے.begin(), انکڑے.سرا());
	إذا (!union_انکڑے.خالی_تھاواں())
		سیٹ_strpool_attribute(id, union_انکڑے);
}

فراغ RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->گن = std::move(buf);
}

نطاق {
	سانچا<قسم_ناں F>
	فراغ walk_process_attr_objects(RTLIL::Process *process, F ویکھو) {
		ویکھو(process);
		std::سمتیہ<RTLIL::Case_قاعدہ*> case_ڈھیر{&process->جڑ_case};
		طالما (!case_ڈھیر.خالی_تھاواں()) {
			RTLIL::Case_قاعدہ *cs = case_ڈھیر.پٹھ();
			case_ڈھیر.نکالو_پٹھ();
			ویکھو(cs);
			لكل (خودکار *sw : cs->switches) {
				ویکھو(sw);
				لكل (خودکار *case_ : sw->cases)
					case_ڈھیر.ڈالو_پٹھ(case_);
			}
		}
		لكل (خودکار *متزامن : process->syncs)
			لكل (خودکار &mwa : متزامن->mem_write_actions)
				ویکھو(&mwa);
	}

	سانچا<قسم_ناں F>
	فراغ walk_attr_objects(RTLIL::Design *design, F ویکھو) {
		لكل (خودکار &[_, module] : design->modules_) {
			ویکھو(module);
			لكل (خودکار &[_, wire] : module->wires_)
				ویکھو(wire);
			لكل (خودکار &[_, mem] : module->memories)
				ویکھو(mem);
			لكل (خودکار &[_, cell] : module->cells_)
				ویکھو(cell);
			لكل (خودکار &[_, process] : module->processes)
				walk_process_attr_objects(process, ویکھو);
		}
	}
}


ناپ_t RTLIL::Design::gc_twines()
{
	int64_t شروع = PerformanceTimer::query();
	pool<IdString> جیوندا;
	خودکار جڑ = [&](IdString ref) {
		إذا (ref != IdString::Null)
			جیوندا.داخل_کرو(ref);
	};

	walk_attr_objects(هذا, [&](ثابت RTLIL::AttrObject *obj) {
		لكل (خودکار &attr : obj->گن)
			جڑ(attr.پہلا);
	});

	جڑ(selected_سرگرم_module);

	لكل (خودکار &[name, module] : modules_) {
		جڑ(name);
		لكل (IdString پورٹ : module->ports)
			جڑ(پورٹ);
		لكل (IdString param : module->avail_parameters)
			جڑ(param);
		لكل (خودکار &[param, _] : module->parameter_default_اقدار)
			جڑ(param);
		لكل (خودکار &[name, _] : module->memories)
			جڑ(name);
		لكل (خودکار &[name, _] : module->processes)
			جڑ(name);
		لكل (خودکار &[name, wire] : module->wires_) {
			جڑ(name);
			إذا (wire->معلوم_driver())
				جڑ(wire->driver_پورٹ());
		}
		لكل (خودکار &[name, cell] : module->cells_) {
			جڑ(name);
			جڑ(cell->type);
			لكل (خودکار &conn : cell->connections())
				جڑ(conn.پہلا);
			لكل (خودکار &param : cell->parameters)
				جڑ(param.پہلا);
		}
		لكل (خودکار &[_, process] : module->processes) {
			std::سمتیہ<RTLIL::Case_قاعدہ*> case_ڈھیر{&process->جڑ_case};
			طالما (!case_ڈھیر.خالی_تھاواں()) {
				RTLIL::Case_قاعدہ *cs = case_ڈھیر.پٹھ();
				case_ڈھیر.نکالو_پٹھ();
				لكل (خودکار *sw : cs->switches) {
					لكل (خودکار *case_ : sw->cases)
						case_ڈھیر.ڈالو_پٹھ(case_);
				}
			}
			لكل (خودکار *متزامن : process->syncs) {
				لكل (خودکار &mwa : متزامن->mem_write_actions)
					جڑ(mwa.memid);
			}
		}
	}

	خودکار جڑ_selection = [&](ثابت RTLIL::Selection &sel) {
		لكل (IdString m : sel.selected_modules)
			جڑ(m);
		لكل (خودکار &[m, members] : sel.selected_members) {
			جڑ(m);
			لكل (IdString member : members)
				جڑ(member);
		}
	};

	لكل (خودکار &[name, sel] : selection_vars) {
		جڑ(name);
		جڑ_selection(sel);
	}

	لكل (خودکار &sel : selection_ڈھیر)
		جڑ_selection(sel);

	ناپ_t erased = twines_.gc(جیوندا);

	int64_t ویلا_ns = PerformanceTimer::query() - شروع;
	Pass::گھٹاؤ_from_current_runtime_ns(ویلا_ns);
	twine_gc_ns += ویلا_ns;
	++twine_gc_گنتی;
	أرجع erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ثابت
{
	pool<string> انکڑے;
	إذا (گن.گنتی(id) != 0)
		لكل (خودکار s : ونڈو_ٹوکن(get_string_attribute(id), "|"))
			انکڑے.داخل_کرو(s);
	أرجع انکڑے;
}

فراغ RTLIL::AttrObject::سیٹ_hdlname_attribute(ثابت سمتیہ<string> &hierarchy)
{
	string attrval;
	لكل (ثابت خودکار &ident : hierarchy) {
		إذا (!attrval.خالی_تھاواں())
			attrval += " ";
		attrval += ident;
	}
	سیٹ_string_attribute(ID::hdlname, attrval);
}

سمتیہ<string> RTLIL::AttrObject::get_hdlname_attribute() ثابت
{
	أرجع ونڈو_ٹوکن(get_string_attribute(ID::hdlname), " ");
}

فراغ RTLIL::AttrObject::سیٹ_intvec_attribute(RTLIL::IdString id, ثابت سمتیہ<صحيح> &انکڑے)
{
	std::stringstream attrval;
	لكل (خودکار &i : انکڑے) {
		إذا (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	گن[id] = RTLIL::Const(attrval.str());
}

سمتیہ<صحيح> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ثابت
{
	سمتیہ<صحيح> انکڑے;
	خودکار it = گن.لبھو(id);
	إذا (it != گن.سرا())
		لكل (ثابت خودکار &s : ونڈو_ٹوکن(گن.at(id).decode_string())) {
			حرف *سرا = خالی_سوچک;
			errno = 0;
			لما قدر = strtol(s.c_str(), &سرا, 10);
			إذا (سرا != s.c_str() + s.ناپ())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			إذا (errno == ERANGE || قدر < INT_MIN || قدر > ودھ_توں_ودھ_عدد)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			انکڑے.ڈالو_پٹھ(قدر);
		}
	أرجع انکڑے;
}

منطقي RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ثابت
{
	إذا (current_design != خالی_سوچک) {
		خودکار module = current_design->module(mod_name);
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
	إذا (بھریا_selection)
		أرجع سچ;
	إذا (selected_modules.گنتی(mod_name) > 0)
		أرجع سچ;
	إذا (selected_members.گنتی(mod_name) > 0)
		أرجع سچ;
	أرجع خطأ;
}

منطقي RTLIL::Selection::selected_سارا_module(RTLIL::IdString mod_name) ثابت
{
	إذا (complete_selection)
		أرجع سچ;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (بھریا_selection)
		أرجع سچ;
	إذا (selected_modules.گنتی(mod_name) > 0)
		أرجع سچ;
	أرجع خطأ;
}

منطقي RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	إذا (complete_selection)
		أرجع سچ;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (بھریا_selection)
		أرجع سچ;
	إذا (selected_modules.گنتی(mod_name) > 0)
		أرجع سچ;
	إذا (selected_members.گنتی(mod_name) > 0)
		إذا (selected_members.at(mod_name).گنتی(memb_name) > 0)
			أرجع سچ;
	أرجع خطأ;
}

فراغ RTLIL::Selection::optimize(RTLIL::Design *design)
{
	إذا (design != current_design) {
		current_design = design;
	}

	إذا (selects_boxes && بھریا_selection)
		complete_selection = سچ;
	إذا (complete_selection) {
		بھریا_selection = خطأ;
		selects_boxes = سچ;
	}
	إذا (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		أرجع;
	}

	std::سمتیہ<RTLIL::IdString> del_list, جوڑو_list;

	del_list.clear();
	لكل (خودکار mod_name : selected_modules) {
		إذا (current_design->module(mod_name) == خالی_سوچک || (!selects_boxes && boxed_module(mod_name)))
			del_list.ڈالو_پٹھ(mod_name);
		selected_members.erase(mod_name);
	}
	لكل (خودکار mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	لكل (خودکار &it : selected_members)
		إذا (current_design->module(it.پہلا) == خالی_سوچک || (!selects_boxes && boxed_module(it.پہلا)))
			del_list.ڈالو_پٹھ(it.پہلا);
	لكل (خودکار mod_name : del_list)
		selected_members.erase(mod_name);

	لكل (خودکار &it : selected_members) {
		del_list.clear();
		لكل (خودکار memb_name : it.دوجا)
			إذا (current_design->module(it.پہلا)->گنتی_id(memb_name) == 0)
				del_list.ڈالو_پٹھ(memb_name);
		لكل (خودکار memb_name : del_list)
			it.دوجا.erase(memb_name);
	}

	del_list.clear();
	جوڑو_list.clear();
	لكل (خودکار &it : selected_members)
		إذا (it.دوجا.ناپ() == 0)
			del_list.ڈالو_پٹھ(it.پہلا);
		وإلا إذا (it.دوجا.ناپ() == current_design->module(it.پہلا)->wires_.ناپ() + current_design->module(it.پہلا)->memories.ناپ() +
				current_design->module(it.پہلا)->cells_.ناپ() + current_design->module(it.پہلا)->processes.ناپ())
			جوڑو_list.ڈالو_پٹھ(it.پہلا);
	لكل (خودکار mod_name : del_list)
		selected_members.erase(mod_name);
	لكل (خودکار mod_name : جوڑو_list) {
		selected_members.erase(mod_name);
		selected_modules.داخل_کرو(mod_name);
	}

	إذا (selected_modules.ناپ() == current_design->modules_.ناپ()) {
		selected_modules.clear();
		selected_members.clear();
		إذا (selects_boxes)
			complete_selection = سچ;
		وإلا
			بھریا_selection = سچ;
	}
}

فراغ RTLIL::Selection::clear()
{
	بھریا_selection = خطأ;
	complete_selection = خطأ;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_مقرر_کردا (جديد define_map_t)
{
	ساكن بنانشانی صحيح hashidx_گنتی = 123456789;
	hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
	hashidx_ = hashidx_گنتی;

	refcount_modules_ = 0;
	selected_سرگرم_module.clear();
	ڈالو_بھریا_selection();

	RTLIL::Design::get_all_designs()->داخل_کرو(std::pair<بنانشانی صحيح, RTLIL::Design*>(hashidx_, هذا));
}

RTLIL::Design::~Design()
{
	لكل (خودکار &pr : modules_)
		مٹاؤ pr.دوجا;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

ساكن std::map<بنانشانی صحيح, RTLIL::Design*> all_designs;
std::map<بنانشانی صحيح, RTLIL::Design*> *RTLIL::Design::get_all_designs(فراغ)
{
	أرجع &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	أرجع RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ثابت RTLIL::Module *RTLIL::Design::module(IdString id) ثابت {
	twines_.check_owned(id);
	أرجع modules_.گنتی(id) ? modules_.at(id) : عدم;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	أرجع modules_.گنتی(id) ? modules_.at(id) : عدم;
}

RTLIL::Module *RTLIL::Design::چوٹی_module() ثابت
{
	RTLIL::Module *module = خالی_سوچک;
	صحيح module_گنتی = 0;

	لكل (خودکار mod : selected_modules()) {
		إذا (mod->get_bool_attribute(ID::چوٹی))
			أرجع mod;
		module_گنتی++;
		module = mod;
	}

	أرجع module_گنتی == 1 ? module : خالی_سوچک;
}

RTLIL::Module *RTLIL::Design::جوڑو_Module(RTLIL::IdString name)
{
	إذا (modules_.گنتی(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = جديد RTLIL::Module;
	modules_[name] = module;
	module->design = هذا;
	module->name = name;

	لكل (خودکار mon : monitors)
		mon->notify_module_جوڑو(module);

	إذا (yosys_xtrace) {
		لوگارتھم("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	أرجع module;
}

فراغ RTLIL::Design::scratchpad_unset(ثابت std::string &varname)
{
	scratchpad.erase(varname);
}

فراغ RTLIL::Design::scratchpad_سیٹ_int(ثابت std::string &varname, صحيح قدر)
{
	scratchpad[varname] = stringf("%d", قدر);
}

فراغ RTLIL::Design::scratchpad_سیٹ_bool(ثابت std::string &varname, منطقي قدر)
{
	scratchpad[varname] = قدر ? "true" : "false";
}

فراغ RTLIL::Design::scratchpad_سیٹ_string(ثابت std::string &varname, std::string قدر)
{
	scratchpad[varname] = std::move(قدر);
}

صحيح RTLIL::Design::scratchpad_get_int(ثابت std::string &varname, صحيح default_قدر_2) ثابت
{
	خودکار it = scratchpad.لبھو(varname);
	إذا (it == scratchpad.سرا())
		أرجع default_قدر_2;

	ثابت std::string &str = it->دوجا;

	إذا (str == "0" || str == "false")
		أرجع 0;

	إذا (str == "1" || str == "true")
		أرجع 1;

	حرف *endptr = خالی_سوچک;
	لما صحيح parsed_قدر = strtol(str.c_str(), &endptr, 10);
	أرجع *endptr ? default_قدر_2 : parsed_قدر;
}

منطقي RTLIL::Design::scratchpad_get_bool(ثابت std::string &varname, منطقي default_قدر_2) ثابت
{
	خودکار it = scratchpad.لبھو(varname);
	إذا (it == scratchpad.سرا())
		أرجع default_قدر_2;

	ثابت std::string &str = it->دوجا;

	إذا (str == "0" || str == "false")
		أرجع خطأ;

	إذا (str == "1" || str == "true")
		أرجع سچ;

	أرجع default_قدر_2;
}

std::string RTLIL::Design::scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_قدر_2) ثابت
{
	خودکار it = scratchpad.لبھو(varname);
	إذا (it == scratchpad.سرا())
		أرجع default_قدر_2;

	أرجع it->دوجا;
}

فراغ RTLIL::Design::remove(RTLIL::Module *module)
{
	لكل (خودکار mon : monitors)
		mon->notify_module_del(module);

	إذا (yosys_xtrace) {
		لوگارتھم("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	مٹاؤ module;
}

فراغ RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	جوڑو(module);
}

فراغ RTLIL::Design::ترتیب_دیو()
{
	scratchpad.ترتیب_دیو();
	modules_.ترتیب_دیو(ترتیب_دیو_by_id_str(twines_));
	لكل (خودکار &it : modules_)
		it.دوجا->ترتیب_دیو();
}

فراغ RTLIL::Design::ترتیب_دیو_modules()
{
	scratchpad.ترتیب_دیو();
	modules_.ترتیب_دیو(ترتیب_دیو_by_id_str(twines_));
}

فراغ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

فراغ RTLIL::Design::check()
{
#جے_بنا_تعریف NDEBUG
	log_assert(!selection_ڈھیر.خالی_تھاواں());
	صحيح pool_ناپ = 0;
	لكل (خودکار &it : modules_)
		pool_ناپ = std::max(pool_ناپ, ThreadPool::work_pool_ناپ(0, it.دوجا->cells_ناپ(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_ناپ);
	لكل (خودکار &it : modules_) {
		log_assert(هذا == it.دوجا->design);
		log_assert(it.پہلا == it.دوجا->name);
		check_module(it.دوجا, thread_pool);
	}
#جے_انت
}

فراغ RTLIL::Design::optimize()
{
	لكل (خودکار &it : modules_)
		it.دوجا->optimize();
	لكل (خودکار &it : selection_ڈھیر)
		it.optimize(هذا);
	لكل (خودکار &it : selection_vars)
		it.دوجا.optimize(هذا);
}

فراغ RTLIL::Design::clone_into(RTLIL::Design *dst) ثابت
{
	log_assert(dst->modules_.خالی_تھاواں());
	dst->twines_ = twines_;
	لكل (خودکار it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->دوجا->clone(dst);
}

منطقي RTLIL::Design::selected_module(IdString mod_name) ثابت
{
	إذا (!selected_سرگرم_module.خالی_تھاواں() && mod_name != selected_سرگرم_module)
		أرجع خطأ;
	أرجع selection().selected_module(mod_name);
}

منطقي RTLIL::Design::selected_سارا_module(RTLIL::IdString mod_name) ثابت
{
	إذا (!selected_سرگرم_module.خالی_تھاواں() && mod_name != selected_سرگرم_module)
		أرجع خطأ;
	أرجع selection().selected_سارا_module(mod_name);
}

منطقي RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	إذا (!selected_سرگرم_module.خالی_تھاواں() && mod_name != selected_سرگرم_module)
		أرجع خطأ;
	أرجع selection().selected_member(mod_name, memb_name);
}

منطقي RTLIL::Design::selected_module(RTLIL::Module *mod) ثابت
{
	أرجع selected_module(mod->name);
}

منطقي RTLIL::Design::selected_سارا_module(RTLIL::Module *mod) ثابت
{
	أرجع selected_سارا_module(mod->name);
}

فراغ RTLIL::Design::ڈالو_selection(RTLIL::Selection sel)
{
	sel.current_design = هذا;
	selection_ڈھیر.ڈالو_پٹھ(sel);
}

فراغ RTLIL::Design::ڈالو_خالی_تھاواں_selection()
{
	ڈالو_selection(RTLIL::Selection::خالی_تھاواں_Selection(هذا));
}

فراغ RTLIL::Design::ڈالو_بھریا_selection()
{
	ڈالو_selection(RTLIL::Selection::بھریا_Selection(هذا));
}

فراغ RTLIL::Design::ڈالو_complete_selection()
{
	ڈالو_selection(RTLIL::Selection::CompleteSelection(هذا));
}

فراغ RTLIL::Design::نکالو_selection()
{
	selection_ڈھیر.نکالو_پٹھ();
	// Default to a full_selection if we ran out of stack
	إذا (selection_ڈھیر.خالی_تھاواں())
		ڈالو_بھریا_selection();
}

std::string RTLIL::Design::منزل_rtlil_str(منطقي only_selected) ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(هذا), only_selected, سچ, خطأ, RTLIL_BACKEND::Dump_منوال::Readable);
	أرجع f.str();
}

std::سمتیہ<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ثابت
{
	منطقي include_partials = partials == RTLIL::SELECT_ALL;
	منطقي exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	منطقي ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::سمتیہ<RTLIL::Module*> نتیجہ;
	نتیجہ.reserve(modules_.ناپ());
	لكل (خودکار &it : modules_) {
		إذا (selected_سارا_module(it.پہلا) || (include_partials && selected_module(it.پہلا))) {
			إذا (!(exclude_boxes && it.دوجا->get_blackbox_attribute(ignore_wb)))
				نتیجہ.ڈالو_پٹھ(it.دوجا);
			وإلا
				بدل (boxes)
				{
				حالة RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.دوجا);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.دوجا);
					اكسر;
				حالة RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.دوجا);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.دوجا);
					اكسر;
				حالة RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.دوجا);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.دوجا);
					اكسر;
				افتراضي:
					اكسر;
				}
		} وإلا إذا (!include_partials && selected_module(it.پہلا)) {
			بدل(partials)
			{
			حالة RTLIL::SELECT_سارا_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.دوجا);
				اكسر;
			حالة RTLIL::SELECT_سارا_ERR:
				log_error("Unsupported partially selected module %s.\n", it.دوجا);
				اكسر;
			حالة RTLIL::SELECT_سارا_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.دوجا);
				اكسر;
			افتراضي:
				اكسر;
			}
		}
	}
	أرجع نتیجہ;
}

RTLIL::Module::Module()
{
	ساكن بنانشانی صحيح hashidx_گنتی = 123456789;
	hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
	hashidx_ = hashidx_گنتی;

	design = خالی_سوچک;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->داخل_کرو(std::pair<بنانشانی صحيح, RTLIL::Module*>(hashidx_, هذا));
#جے_انت
}

RTLIL::Module::~Module()
{
	لكل (خودکار &pr : wires_)
		مٹاؤ pr.دوجا;
	لكل (خودکار &pr : memories)
		مٹاؤ pr.دوجا;
	لكل (خودکار &pr : cells_)
		مٹاؤ pr.دوجا;
	لكل (خودکار &pr : processes)
		مٹاؤ pr.دوجا;
#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#جے_انت
}

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
ساكن std::map<بنانشانی صحيح, RTLIL::Module*> all_modules;
std::map<بنانشانی صحيح, RTLIL::Module*> *RTLIL::Module::get_all_modules(فراغ)
{
	أرجع &all_modules;
}
#جے_انت

فراغ RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	لكل (خودکار it = wires_.begin(); it != wires_.سرا(); ++it)
		إذا (!it->دوجا->پورٹ_داخلہ && !it->دوجا->پورٹ_output)
			delwires.داخل_کرو(it->دوجا);

	لكل (خودکار it = memories.begin(); it != memories.سرا(); ++it)
		مٹاؤ it->دوجا;
	memories.clear();

	لكل (خودکار it = cells_.begin(); it != cells_.سرا(); ++it)
		مٹاؤ it->دوجا;
	cells_.clear();

	لكل (خودکار it = processes.begin(); it != processes.سرا(); ++it)
		مٹاؤ it->دوجا;
	processes.clear();

	connections_.clear();

	remove(delwires);
	سیٹ_bool_attribute(ID::blackbox);
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

ناپ_t RTLIL::Module::گنتی_id(RTLIL::IdString id)
{
	أرجع wires_.گنتی(id) + cells_.گنتی(id) + memories.گنتی(id) + processes.گنتی(id);
}

#جے_بنا_تعریف NDEBUG
نطاق {
	بنية اندرونی_Cell_Checker
	{
		ثابت RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		اندرونی_Cell_Checker(ثابت RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		فراغ error(صحيح linenr, std::optional<std::string> error_پیغام = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			إذا (error_پیغام) {
				log_error("%s in internal cell %s\n", *error_پیغام, cell_loc);
			} وإلا {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		صحيح param(RTLIL::IdString name)
		{
			خودکار it = cell->parameters.لبھو(name);
			إذا (it == cell->parameters.سرا()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.داخل_کرو(name);
			أرجع it->دوجا.as_int();
		}

		صحيح param_bool(RTLIL::IdString name)
		{
			صحيح v = param(name);
			إذا (Get_ناپ(cell->parameters.at(name)) > 32)
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

		فراغ param_بٹ(RTLIL::IdString name, صحيح چوڑائی)
		{
			param(name);
			إذا (Get_ناپ(cell->parameters.at(name)) != چوڑائی)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			أرجع cell->parameters.at(name).decode_string();
		}

		فراغ پورٹ(RTLIL::IdString name, صحيح چوڑائی)
		{
			خودکار it = cell->connections_.لبھو(name);
			إذا (it == cell->connections_.سرا()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			إذا (Get_ناپ(it->دوجا) != چوڑائی) {
				std::string err = stringf("Expected port to have width %d", چوڑائی);
				error(__LINE__, err);
			}
			expected_ports.داخل_کرو(name);
		}

		فراغ check_expected(منطقي check_matched_sign = خطأ)
		{
			لكل (خودکار &para : cell->parameters)
				إذا (expected_params.گنتی(para.پہلا) == 0)
					error(__LINE__);
			لكل (خودکار &conn : cell->connections())
				إذا (expected_ports.گنتی(conn.پہلا) == 0)
					error(__LINE__);

			إذا (check_matched_sign) {
				log_assert(expected_params.گنتی(ID::A_SIGNED) != 0 && expected_params.گنتی(ID::B_SIGNED) != 0);
				منطقي a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				منطقي b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				إذا (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		فراغ check()
		{
			إذا (cell->type.is_عوامی() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				أرجع;

			إذا (cell->type.وچ(ID($buf), ID($barrier))) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.وچ(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected();
				أرجع;
			}

			إذا (cell->type.وچ(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected(سچ);
				أرجع;
			}

			إذا (cell->type.وچ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected();
				أرجع;
			}

			إذا (cell->type.وچ(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/خطأ);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type.وچ(ID($کھسکاؤ), ID($shiftx))) {
				إذا (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/خطأ);
				} وإلا {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type.وچ(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected(سچ);
				أرجع;
			}

			إذا (cell->type.وچ(ID($جوڑو), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($قوت))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected(cell->type != ID($قوت));
				أرجع;
			}

			إذا (cell->type == ID($fa)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::B, param(ID::چوڑائی_2));
				پورٹ(ID::C, param(ID::چوڑائی_2));
				پورٹ(ID::X, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($lcu)) {
				پورٹ(ID::P, param(ID::چوڑائی_2));
				پورٹ(ID::G, param(ID::چوڑائی_2));
				پورٹ(ID::CI, 1);
				پورٹ(ID::CO, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::CI, 1);
				پورٹ(ID::BI, 1);
				پورٹ(ID::X, param(ID::Y_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				پورٹ(ID::CO, param(ID::Y_چوڑائی));
				check_expected(سچ);
				أرجع;
			}

			إذا (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_چوڑائی);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected();
				Macc().from_cell(cell);
				أرجع;
			}

			إذا (cell->type == ID($macc_v2)) {
				إذا (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				إذا (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_بٹ(ID::ضرب_حاصل_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_بٹ(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_بٹ(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_بٹ(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_بٹ(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				إذا (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_بٹ(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_بٹ(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_بٹ(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ثابت Const &a_چوڑائی = cell->getParam(ID::A_WIDTHS);
				ثابت Const &b_چوڑائی = cell->getParam(ID::B_WIDTHS);
				ثابت Const &c_چوڑائی = cell->getParam(ID::C_WIDTHS);
				صحيح a_چوڑائی_جوڑ = 0, b_چوڑائی_جوڑ = 0, c_چوڑائی_جوڑ = 0;
				لكل (صحيح i = 0; i < param(ID::NPRODUCTS); i++) {
					a_چوڑائی_جوڑ += a_چوڑائی.کڈھو(16 * i, 16).as_int(خطأ);
					b_چوڑائی_جوڑ += b_چوڑائی.کڈھو(16 * i, 16).as_int(خطأ);
				}
				لكل (صحيح i = 0; i < param(ID::NADDENDS); i++) {
					c_چوڑائی_جوڑ += c_چوڑائی.کڈھو(16 * i, 16).as_int(خطأ);
				}
				پورٹ(ID::A, a_چوڑائی_جوڑ);
				پورٹ(ID::B, b_چوڑائی_جوڑ);
				پورٹ(ID::C, c_چوڑائی_جوڑ);
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected();
				أرجع;
			}

			إذا (cell->type.وچ(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type == ID($ٹکڑا)) {
				param(ID::وتھ_2);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				إذا (param(ID::وتھ_2) + param(ID::Y_چوڑائی) > param(ID::A_چوڑائی))
					error(__LINE__);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($concat)) {
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::A_چوڑائی) + param(ID::B_چوڑائی));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mux)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::B, param(ID::چوڑائی_2));
				پورٹ(ID::S, 1);
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($pmux)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::B, param(ID::چوڑائی_2) * param(ID::S_چوڑائی));
				پورٹ(ID::S, param(ID::S_چوڑائی));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bmux)) {
				پورٹ(ID::A, param(ID::چوڑائی_2) << param(ID::S_چوڑائی));
				پورٹ(ID::S, param(ID::S_چوڑائی));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($demux)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::S, param(ID::S_چوڑائی));
				پورٹ(ID::Y, param(ID::چوڑائی_2) << param(ID::S_چوڑائی));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($lut)) {
				param(ID::LUT);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sop)) {
				param(ID::ڈونگھائی_2);
				param(ID::جدول);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sr)) {
				param_bool(ID::سیٹ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورٹ(ID::سیٹ_2, param(ID::چوڑائی_2));
				پورٹ(ID::CLR, param(ID::چوڑائی_2));
				پورٹ(ID::Q,   param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($ff)) {
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::EN, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::سیٹ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::سیٹ_2, param(ID::چوڑائی_2));
				پورٹ(ID::CLR, param(ID::چوڑائی_2));
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffsre)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::سیٹ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				param_bool(ID::EN_POLARITY);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::EN, 1);
				پورٹ(ID::سیٹ_2, param(ID::چوڑائی_2));
				پورٹ(ID::CLR, param(ID::چوڑائی_2));
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بٹ(ID::ARST_قدر, param(ID::چوڑائی_2));
				پورٹ(ID::CLK, 1);
				پورٹ(ID::ARST, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_بٹ(ID::SRST_قدر, param(ID::چوڑائی_2));
				پورٹ(ID::CLK, 1);
				پورٹ(ID::SRST, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.وچ(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_بٹ(ID::SRST_قدر, param(ID::چوڑائی_2));
				پورٹ(ID::CLK, 1);
				پورٹ(ID::EN, 1);
				پورٹ(ID::SRST, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بٹ(ID::ARST_قدر, param(ID::چوڑائی_2));
				پورٹ(ID::CLK, 1);
				پورٹ(ID::EN, 1);
				پورٹ(ID::ARST, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::ALOAD, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::AD, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($aldffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::EN, 1);
				پورٹ(ID::ALOAD, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::AD, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				پورٹ(ID::EN, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بٹ(ID::ARST_قدر, param(ID::چوڑائی_2));
				پورٹ(ID::EN, 1);
				پورٹ(ID::ARST, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::سیٹ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورٹ(ID::EN, 1);
				پورٹ(ID::سیٹ_2, param(ID::چوڑائی_2));
				پورٹ(ID::CLR, param(ID::چوڑائی_2));
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_بٹ);
				param(ID::STATE_عدد);
				param(ID::STATE_عدد_LOG2);
				param(ID::STATE_RST);
				param_بٹ(ID::STATE_جدول, param(ID::STATE_بٹ) * param(ID::STATE_عدد));
				param(ID::TRANS_عدد);
				param_بٹ(ID::TRANS_جدول, param(ID::TRANS_عدد) * (2*param(ID::STATE_عدد_LOG2) + param(ID::CTRL_وچ_چوڑائی) + param(ID::CTRL_باہرلا_چوڑائی)));
				پورٹ(ID::CLK, 1);
				پورٹ(ID::ARST, 1);
				پورٹ(ID::CTRL_وچ, param(ID::CTRL_وچ_چوڑائی));
				پورٹ(ID::CTRL_باہرلا, param(ID::CTRL_باہرلا_چوڑائی));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::EN, 1);
				پورٹ(ID::ADDR, param(ID::ABITS));
				پورٹ(ID::انکڑے_2, param(ID::چوڑائی_2));
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
				param_بٹ(ID::ARST_قدر, param(ID::چوڑائی_2));
				param_بٹ(ID::SRST_قدر, param(ID::چوڑائی_2));
				param_بٹ(ID::INIT_قدر, param(ID::چوڑائی_2));
				پورٹ(ID::CLK, 1);
				پورٹ(ID::EN, 1);
				پورٹ(ID::ARST, 1);
				پورٹ(ID::SRST, 1);
				پورٹ(ID::ADDR, param(ID::ABITS));
				پورٹ(ID::انکڑے_2, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::ترجیح);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::EN, param(ID::چوڑائی_2));
				پورٹ(ID::ADDR, param(ID::ABITS));
				پورٹ(ID::انکڑے_2, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memwr_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PORTID);
				param(ID::ترجیح_نقاب_2);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::EN, param(ID::چوڑائی_2));
				پورٹ(ID::ADDR, param(ID::ABITS));
				پورٹ(ID::انکڑے_2, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::ترجیح);
				پورٹ(ID::ADDR, param(ID::ABITS));
				پورٹ(ID::انکڑے_2, param(ID::چوڑائی_2) * param(ID::الفاظ));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::ترجیح);
				پورٹ(ID::ADDR, param(ID::ABITS));
				پورٹ(ID::انکڑے_2, param(ID::چوڑائی_2) * param(ID::الفاظ));
				پورٹ(ID::EN, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mem)) {
				param(ID::MEMID);
				param(ID::ناپ_2);
				param(ID::وتھ_2);
				param(ID::INIT);
				param_بٹ(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_بٹ(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_بٹ(ID::RD_TRANSPARENT, max(1, param(ID::RD_PORTS)));
				param_بٹ(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_بٹ(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				پورٹ(ID::RD_CLK, param(ID::RD_PORTS));
				پورٹ(ID::RD_EN, param(ID::RD_PORTS));
				پورٹ(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				پورٹ(ID::RD_انکڑے, param(ID::RD_PORTS) * param(ID::چوڑائی_2));
				پورٹ(ID::WR_CLK, param(ID::WR_PORTS));
				پورٹ(ID::WR_EN, param(ID::WR_PORTS) * param(ID::چوڑائی_2));
				پورٹ(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				پورٹ(ID::WR_انکڑے, param(ID::WR_PORTS) * param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mem_v2)) {
				param(ID::MEMID);
				param(ID::ناپ_2);
				param(ID::وتھ_2);
				param(ID::INIT);
				param_بٹ(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_بٹ(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_بٹ(ID::RD_TRANSPARENCY_نقاب, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_بٹ(ID::RD_COLLISION_X_نقاب, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_بٹ(ID::RD_WIDE_CONTINUATION, max(1, param(ID::RD_PORTS)));
				param_بٹ(ID::RD_CE_OVER_SRST, max(1, param(ID::RD_PORTS)));
				param_بٹ(ID::RD_ARST_قدر, max(1, param(ID::RD_PORTS) * param(ID::چوڑائی_2)));
				param_بٹ(ID::RD_SRST_قدر, max(1, param(ID::RD_PORTS) * param(ID::چوڑائی_2)));
				param_بٹ(ID::RD_INIT_قدر, max(1, param(ID::RD_PORTS) * param(ID::چوڑائی_2)));
				param_بٹ(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_بٹ(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				param_بٹ(ID::WR_WIDE_CONTINUATION, max(1, param(ID::WR_PORTS)));
				param_بٹ(ID::WR_ترجیح_نقاب, max(1, param(ID::WR_PORTS) * param(ID::WR_PORTS)));
				پورٹ(ID::RD_CLK, param(ID::RD_PORTS));
				پورٹ(ID::RD_EN, param(ID::RD_PORTS));
				پورٹ(ID::RD_ARST, param(ID::RD_PORTS));
				پورٹ(ID::RD_SRST, param(ID::RD_PORTS));
				پورٹ(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				پورٹ(ID::RD_انکڑے, param(ID::RD_PORTS) * param(ID::چوڑائی_2));
				پورٹ(ID::WR_CLK, param(ID::WR_PORTS));
				پورٹ(ID::WR_EN, param(ID::WR_PORTS) * param(ID::چوڑائی_2));
				پورٹ(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				پورٹ(ID::WR_انکڑے, param(ID::WR_PORTS) * param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($tribuf)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				پورٹ(ID::EN, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bweqx)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::B, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bwmux)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::B, param(ID::چوڑائی_2));
				پورٹ(ID::S, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.وچ(ID($أكد), ID($assume), ID($جیوندا), ID($fair), ID($cover))) {
				پورٹ(ID::A, 1);
				پورٹ(ID::EN, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($initstate)) {
				پورٹ(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type.وچ(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.وچ(ID($anyinit))) {
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($equiv)) {
				پورٹ(ID::A, 1);
				پورٹ(ID::B, 1);
				پورٹ(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type.وچ(ID($specify2), ID($specify3))) {
				param_bool(ID::بھریا);
				param_bool(ID::SRC_DST_PEN);
				param_bool(ID::SRC_DST_POL);
				param(ID::T_RISE_MIN);
				param(ID::T_RISE_TYP);
				param(ID::T_RISE_MAX);
				param(ID::T_FALL_MIN);
				param(ID::T_FALL_TYP);
				param(ID::T_FALL_MAX);
				پورٹ(ID::EN, 1);
				پورٹ(ID::SRC, param(ID::SRC_چوڑائی));
				پورٹ(ID::DST, param(ID::DST_چوڑائی));
				إذا (cell->type == ID($specify3)) {
					param_bool(ID::کنارا_EN);
					param_bool(ID::کنارا_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					پورٹ(ID::DAT, param(ID::DST_چوڑائی));
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
				param(ID::T_حد_بندی_MIN);
				param(ID::T_حد_بندی_TYP);
				param(ID::T_حد_بندی_MAX);
				param(ID::T_LIMIT2_MIN);
				param(ID::T_LIMIT2_TYP);
				param(ID::T_LIMIT2_MAX);
				پورٹ(ID::SRC_EN, 1);
				پورٹ(ID::DST_EN, 1);
				پورٹ(ID::SRC, param(ID::SRC_چوڑائی));
				پورٹ(ID::DST, param(ID::DST_چوڑائی));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::ترجیح);
				پورٹ(ID::EN, 1);
				پورٹ(ID::TRG, param(ID::TRG_چوڑائی));
				پورٹ(ID::دلیلاں, param(ID::دلیلاں_چوڑائی));
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
				param(ID::ترجیح);
				پورٹ(ID::A, 1);
				پورٹ(ID::EN, 1);
				پورٹ(ID::TRG, param(ID::TRG_چوڑائی));
				پورٹ(ID::دلیلاں, param(ID::دلیلاں_چوڑائی));
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

			إذا (cell->type == ID($_BUF_))    { پورٹ(ID::A,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NOT_))    { پورٹ(ID::A,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AND_))    { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NAND_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OR_))     { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NOR_))    { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_XOR_))    { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_XNOR_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_ANDNOT_)) { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_ORNOT_))  { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX_))    { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::S,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NMUX_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::S,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AOI3_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OAI3_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AOI4_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::D,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OAI4_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::D,1); پورٹ(ID::Y,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_TBUF_))  { پورٹ(ID::A,1); پورٹ(ID::Y,1); پورٹ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_MUX4_))  { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::D,1); پورٹ(ID::S,1); پورٹ(ID::T,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX8_))  { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::D,1); پورٹ(ID::E,1); پورٹ(ID::F,1); پورٹ(ID::G,1); پورٹ(ID::H,1); پورٹ(ID::S,1); پورٹ(ID::T,1); پورٹ(ID::U,1); پورٹ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX16_)) { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::D,1); پورٹ(ID::E,1); پورٹ(ID::F,1); پورٹ(ID::G,1); پورٹ(ID::H,1); پورٹ(ID::I,1); پورٹ(ID::J,1); پورٹ(ID::K,1); پورٹ(ID::L,1); پورٹ(ID::M,1); پورٹ(ID::N,1); پورٹ(ID::O,1); پورٹ(ID::P,1); پورٹ(ID::S,1); پورٹ(ID::T,1); پورٹ(ID::U,1); پورٹ(ID::V,1); پورٹ(ID::Y,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ پورٹ(ID::S,1); پورٹ(ID::R,1); پورٹ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_FF_)) { پورٹ(ID::D,1); پورٹ(ID::Q,1); check_expected();  أرجع; }

			إذا (cell->type.وچ(ID($_DFF_N_), ID($_DFF_P_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::R,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::R,1); پورٹ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::L,1); پورٹ(ID::AD,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::L,1); پورٹ(ID::AD,1); پورٹ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ پورٹ(ID::C,1); پورٹ(ID::S,1); پورٹ(ID::R,1); پورٹ(ID::D,1); پورٹ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ پورٹ(ID::C,1); پورٹ(ID::S,1); پورٹ(ID::R,1); پورٹ(ID::D,1); پورٹ(ID::E,1); پورٹ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::R,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::R,1); پورٹ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ پورٹ(ID::E,1); پورٹ(ID::D,1); پورٹ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ پورٹ(ID::E,1); پورٹ(ID::R,1); پورٹ(ID::D,1); پورٹ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ پورٹ(ID::E,1); پورٹ(ID::S,1); پورٹ(ID::R,1); پورٹ(ID::D,1); پورٹ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.وچ(ID($سیٹ_ٹیگ))) {
				param(ID::چوڑائی_2);
				param(ID::ٹیگ_2);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::سیٹ_2, param(ID::چوڑائی_2));
				پورٹ(ID::CLR, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.وچ(ID($get_ٹیگ),ID($original_ٹیگ))) {
				param(ID::چوڑائی_2);
				param(ID::ٹیگ_2);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.وچ(ID($overwrite_ٹیگ))) {
				param(ID::چوڑائی_2);
				param(ID::ٹیگ_2);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::سیٹ_2, param(ID::چوڑائی_2));
				پورٹ(ID::CLR, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.وچ(ID($future_ff))) {
				param(ID::چوڑائی_2);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.وچ(ID($داخلہ_پورٹ))) {
				param(ID::چوڑائی_2);
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.وچ(ID($connect))) {
				param(ID::چوڑائی_2);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::B, param(ID::چوڑائی_2));
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
#جے_انت

فراغ RTLIL::Module::ترتیب_دیو()
{
	wires_.ترتیب_دیو(ترتیب_دیو_by_id_str(design->twines()));
	cells_.ترتیب_دیو(ترتیب_دیو_by_id_str(design->twines()));
	parameter_default_اقدار.ترتیب_دیو(ترتیب_دیو_by_id_str(design->twines()));
	memories.ترتیب_دیو(ترتیب_دیو_by_id_str(design->twines()));
	processes.ترتیب_دیو(ترتیب_دیو_by_id_str(design->twines()));
	لكل (خودکار &it : cells_)
		it.دوجا->ترتیب_دیو();
	لكل (خودکار &it : wires_)
		it.دوجا->گن.ترتیب_دیو(ترتیب_دیو_by_id_str(design->twines()));
	لكل (خودکار &it : memories)
		it.دوجا->گن.ترتیب_دیو(ترتیب_دیو_by_id_str(design->twines()));
}

#جے_بنا_تعریف NDEBUG
ساكن فراغ check_id_کلیدیں(ثابت dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	لكل (خودکار &it : keyed)
		log_assert(!it.پہلا.خالی_تھاواں());
}
#جے_انت

فراغ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#جے_بنا_تعریف NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_ناپ(0, module->cells_ناپ(), 1000));
	ثابت RTLIL::Module *const_module = module;

	pool<std::string> حافظہ_لڑیاں;
	لكل (خودکار &it : module->memories) {
		log_assert(it.پہلا == it.دوجا->name);
		log_assert(it.پہلا != IdString::Null);
		log_assert(it.دوجا->چوڑائی >= 0);
		log_assert(it.دوجا->ناپ >= 0);
		check_id_کلیدیں(it.دوجا->گن);
		حافظہ_لڑیاں.داخل_کرو(it.دوجا->name.str());
	}

	std::سمتیہ<MonotonicFlag> ports_declared(Get_ناپ(module->ports));
	Sharded_سمتیہ<std::string> memids(subpool);
	subpool.لگاتار([const_module, &ports_declared, &حافظہ_لڑیاں, &memids](ثابت ParallelDispatchThreadPool::لگاتار_Ctx &ctx) {
		لكل (صحيح i : ctx.شے_range(const_module->cells_ناپ())) {
			خودکار it = *const_module->cells_.element(i);
			log_assert(const_module == it.دوجا->module);
			log_assert(it.پہلا == it.دوجا->name);
			log_assert(it.پہلا != IdString::Null);
			log_assert(!it.دوجا->type.خالی_تھاواں());
			لكل (خودکار &it2 : it.دوجا->connections()) {
				log_assert(it2.پہلا != IdString::Null);
				it2.دوجا.check(const_module);
			}
			check_id_کلیدیں(it.دوجا->گن);
			check_id_کلیدیں(it.دوجا->parameters);
			اندرونی_Cell_Checker checker(const_module, it.دوجا);
			checker.check();
			إذا (it.دوجا->has_memid()) {
				log_assert(حافظہ_لڑیاں.گنتی(it.دوجا->parameters.at(ID::MEMID).decode_string()));
			} وإلا إذا (it.دوجا->is_mem_cell()) {
				std::string memid = it.دوجا->parameters.at(ID::MEMID).decode_string();
				log_assert(!حافظہ_لڑیاں.گنتی(memid));
				memids.داخل_کرو(ctx, std::move(memid));
			}
			خودکار cell_mod = const_module->design->module(it.دوجا->name);
			إذا (cell_mod != خالی_سوچک) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#جے_تعریف_شدہ __GNUC__
				#ہدایت GCC diagnostic ڈالو
				#ہدایت GCC diagnostic ignored "-Wdeprecated-declarations"
				#جے_انت
				log_assert(!it.دوجا->get_blackbox_attribute());
				#جے_تعریف_شدہ __GNUC__
				#ہدایت GCC diagnostic نکالو
				#جے_انت
			}
		}

		لكل (صحيح i : ctx.شے_range(const_module->wires_ناپ())) {
			خودکار it = *const_module->wires_.element(i);
			log_assert(const_module == it.دوجا->module);
			log_assert(it.پہلا == it.دوجا->name);
			log_assert(it.پہلا != IdString::Null);
			log_assert(it.دوجا->چوڑائی >= 0);
			log_assert(it.دوجا->پورٹ_id >= 0);
			check_id_کلیدیں(it.دوجا->گن);
			إذا (it.دوجا->پورٹ_id) {
				log_assert(Get_ناپ(const_module->ports) >= it.دوجا->پورٹ_id);
				log_assert(const_module->ports.at(it.دوجا->پورٹ_id-1) == it.دوجا->name);
				log_assert(it.دوجا->پورٹ_داخلہ || it.دوجا->پورٹ_output);
				log_assert(it.دوجا->پورٹ_id <= Get_ناپ(ports_declared));
				منطقي previously_declared = ports_declared[it.دوجا->پورٹ_id-1].سیٹ_and_return_old();
				log_assert(previously_declared == خطأ);
			} وإلا
				log_assert(!it.دوجا->پورٹ_داخلہ && !it.دوجا->پورٹ_output);
		}
	});
	لكل (ثابت MonotonicFlag &پورٹ_declared : ports_declared)
		log_assert(پورٹ_declared.لدو() == سچ);
	pool<std::string> memids_pool;
	لكل (std::string &memid : memids)
		log_assert(memids_pool.داخل_کرو(memid).دوجا);

	لكل (خودکار &it : module->processes) {
		log_assert(it.پہلا == it.دوجا->name);
		log_assert(it.پہلا != IdString::Null);
		log_assert(it.دوجا->جڑ_case.compare.خالی_تھاواں());
		std::سمتیہ<RTLIL::Case_قاعدہ*> all_cases = {&it.دوجا->جڑ_case};
		لكل (ناپ_t i = 0; i < all_cases.ناپ(); i++) {
			لكل (خودکار &switch_it : all_cases[i]->switches) {
				لكل (خودکار &case_it : switch_it->cases) {
					لكل (خودکار &compare_it : case_it->compare) {
						log_assert(switch_it->إشارة.ناپ() == compare_it.ناپ());
					}
					all_cases.ڈالو_پٹھ(case_it);
				}
			}
		}
		لكل (خودکار &sync_it : it.دوجا->syncs) {
			بدل (sync_it->type) {
				حالة RTLIL::SyncType::ST0:
				حالة RTLIL::SyncType::ST1:
				حالة RTLIL::SyncType::STp:
				حالة RTLIL::SyncType::STn:
				حالة RTLIL::SyncType::STe:
					log_assert(!sync_it->إشارة.خالی_تھاواں());
					اكسر;
				حالة RTLIL::SyncType::STa:
				حالة RTLIL::SyncType::STg:
				حالة RTLIL::SyncType::STi:
					log_assert(sync_it->إشارة.خالی_تھاواں());
					اكسر;
			}
		}
	}

	لكل (خودکار &it : module->connections_) {
		log_assert(it.پہلا.ناپ() == it.دوجا.ناپ());
		log_assert(!it.پہلا.has_const());
		it.پہلا.check(module);
		it.دوجا.check(module);
	}

	check_id_کلیدیں(module->گن);
#جے_انت
}

فراغ RTLIL::Module::check()
{
	صحيح pool_ناپ = ThreadPool::work_pool_ناپ(0, cells_ناپ(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_ناپ);
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
	new_mod->parameter_default_اقدار.clear();
	لكل (خودکار &it : parameter_default_اقدار)
		new_mod->parameter_default_اقدار[dst_twines.copy_from(design->twines(), it.پہلا)] = it.دوجا;

	لكل (خودکار &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->گن, گن, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	لكل (خودکار it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->پہلا);
		wire_map[it->دوجا] = new_mod->جوڑو_Wire(dst_id, it->دوجا);
	}

	لكل (خودکار it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->پہلا);
		new_mod->جوڑو_حافظہ(dst_id, it->دوجا);
	}

	لكل (خودکار it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->پہلا);
		new_mod->جوڑو_Cell(dst_id, it->دوجا);
	}

	لكل (خودکار it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->پہلا);
		new_mod->جوڑو_Process(dst_id, it->دوجا);
	}

	بنية Rewrite_Sig_Spec_کامہ
	{
		ثابت dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ثابت RTLIL::Module *new_mod;
		فراغ عامل()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([هذا](RTLIL::Wire *&wire) {
				خودکار it = wire_map.لبھو(wire);
				إذا (it != wire_map.سرا())
					wire = it->دوجا;
				وإلا
					log_assert(wire->module == new_mod);
			});
		}
	};

	Rewrite_Sig_Spec_کامہ rewrite_Sig_Spec_کامہ{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewrite_Sig_Spec_کامہ);
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
	dst->جوڑو(new_mod);
	أرجع new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString نشانہ_name) ثابت
{
	RTLIL::Module *new_mod = جديد RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = نشانہ_name;
	cloneInto(new_mod);
	dst->جوڑو(new_mod);
	أرجع new_mod;
}

منطقي RTLIL::Module::has_memories() ثابت
{
	أرجع !memories.خالی_تھاواں();
}

منطقي RTLIL::Module::has_processes() ثابت
{
	أرجع !processes.خالی_تھاواں();
}

منطقي RTLIL::Module::has_memories_warn() ثابت
{
	إذا (!memories.خالی_تھاواں())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", هذا);
	أرجع !memories.خالی_تھاواں();
}

منطقي RTLIL::Module::has_processes_warn() ثابت
{
	إذا (!processes.خالی_تھاواں())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", هذا);
	أرجع !processes.خالی_تھاواں();
}

منطقي RTLIL::Module::is_selected() ثابت
{
	أرجع design->selected_module(هذا->name);
}

منطقي RTLIL::Module::is_selected_سارا() ثابت
{
	أرجع design->selected_سارا_module(هذا->name);
}

std::سمتیہ<RTLIL::Wire*> RTLIL::Module::selected_wires() ثابت
{
	std::سمتیہ<RTLIL::Wire*> نتیجہ;
	نتیجہ.reserve(wires_.ناپ());
	لكل (خودکار &it : wires_)
		إذا (design->selected(هذا, it.دوجا))
			نتیجہ.ڈالو_پٹھ(it.دوجا);
	أرجع نتیجہ;
}

std::سمتیہ<RTLIL::Cell*> RTLIL::Module::selected_cells() ثابت
{
	std::سمتیہ<RTLIL::Cell*> نتیجہ;
	نتیجہ.reserve(cells_.ناپ());
	لكل (خودکار &it : cells_)
		إذا (design->selected(هذا, it.دوجا))
			نتیجہ.ڈالو_پٹھ(it.دوجا);
	أرجع نتیجہ;
}

std::سمتیہ<RTLIL::حافظہ*> RTLIL::Module::selected_memories() ثابت
{
	std::سمتیہ<RTLIL::حافظہ*> نتیجہ;
	نتیجہ.reserve(memories.ناپ());
	لكل (خودکار &it : memories)
		إذا (design->selected(هذا, it.دوجا))
			نتیجہ.ڈالو_پٹھ(it.دوجا);
	أرجع نتیجہ;
}

std::سمتیہ<RTLIL::Process*> RTLIL::Module::selected_processes() ثابت
{
	std::سمتیہ<RTLIL::Process*> نتیجہ;
	نتیجہ.reserve(processes.ناپ());
	لكل (خودکار &it : processes)
		إذا (design->selected(هذا, it.دوجا))
			نتیجہ.ڈالو_پٹھ(it.دوجا);
	أرجع نتیجہ;
}

std::سمتیہ<RTLIL::SelectedMember> RTLIL::Module::selected_members() ثابت
{
	std::سمتیہ<RTLIL::SelectedMember> نتیجہ;
	لكل (خودکار cell : selected_cells())
		نتیجہ.ڈالو_پٹھ({cell, cell->name});
	لكل (خودکار حافظہ_3 : selected_memories())
		نتیجہ.ڈالو_پٹھ({حافظہ_3, حافظہ_3->name});
	لكل (خودکار wire : selected_wires())
		نتیجہ.ڈالو_پٹھ({wire, wire->name});
	لكل (خودکار process : selected_processes())
		نتیجہ.ڈالو_پٹھ({process, process->name});
	أرجع نتیجہ;
}

فراغ RTLIL::Module::جوڑو(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(گنتی_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = هذا;
}

فراغ RTLIL::Module::جوڑو(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(گنتی_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = هذا;
}

فراغ RTLIL::Module::جوڑو(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(گنتی_id(process->name) == 0);
	processes[process->name] = process;
	process->module = هذا;
}

فراغ RTLIL::Module::remove(ثابت pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	إذا (wires.خالی_تھاواں())
		أرجع;

	بنية Delete_Wire_کامہ
	{
		RTLIL::Module *module;
		ثابت pool<RTLIL::Wire*> *wires_p;

		فراغ عامل()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([هذا](RTLIL::Wire *&wire) {
				إذا (wires_p->گنتی(wire))
					wire = module->جوڑو_Wire(stringf("$delete_wire$%d", (صحيح)autoidx++), wire->چوڑائی);
			});
		}

		فراغ عامل()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	Delete_Wire_کامہ delete_wire_کامہ;
	delete_wire_کامہ.module = هذا;
	delete_wire_کامہ.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_کامہ);

	إذا (design->flagBufferedNormalized) {
		لكل (خودکار wire : wires) {
			buf_norm_wire_قطار.erase(wire);
			buf_norm_connect_اشاریہ.erase(wire);
		}
	}

	لكل (خودکار &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.گنتی(id) != 0);
		wires_.erase(id);
		مٹاؤ it;
	}
}

فراغ RTLIL::Module::remove(RTLIL::Cell *cell)
{
	طالما (!cell->connections_.خالی_تھاواں())
		cell->unset_پورٹ(cell->connections_.begin()->پہلا);

	log_assert(cells_.گنتی(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	إذا (design && design->flagBufferedNormalized && buf_norm_cell_قطار.گنتی(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.داخل_کرو(cell);
	} وإلا {
		مٹاؤ cell;
	}
}

فراغ RTLIL::Module::remove(RTLIL::حافظہ *حافظہ_3)
{
	log_assert(memories.گنتی(حافظہ_3->name) != 0);
	memories.erase(حافظہ_3->name);
	مٹاؤ حافظہ_3;
}

فراغ RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.گنتی(process->name) != 0);
	processes.erase(process->name);
	مٹاؤ process;
}

فراغ RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	جوڑو(wire);
}

فراغ RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	جوڑو(cell);
}

فراغ RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	إذا (old_id != IdString::Null && wires_.گنتی(old_id))
		rename(wires_.at(old_id), new_name);
	وإلا إذا (old_id != IdString::Null && cells_.گنتی(old_id))
		rename(cells_.at(old_id), new_name);
	وإلا
		log_abort();
}

فراغ RTLIL::Module::وٹاندرا_ناں(RTLIL::Wire *w1, RTLIL::Wire *w2)
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
	وٹاندرا(w1->name, w2->name);
}

فراغ RTLIL::Module::وٹاندرا_ناں(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	وٹاندرا(c1->name, c2->name);
}

RTLIL::IdString RTLIL::Module::uniquify(RTLIL::IdString name)
{
	صحيح اشاریہ = 0;
	أرجع uniquify(name, اشاریہ);
}

IdString RTLIL::Module::uniquify(IdString name, صحيح &اشاریہ)
{
	إذا (اشاریہ == 0) {
		إذا (گنتی_id(name) == 0)
			أرجع name;
		اشاریہ++;
	}

	طالما (1) {
		IdString new_name = design->twines().جوڑو(TwineSpec::Suffix{name, stringf("_%d", اشاریہ)});
		إذا (گنتی_id(new_name) == 0)
			أرجع new_name;
		اشاریہ++;
	}
}

ساكن منطقي fixup_ports_compare(ثابت RTLIL::Wire *a, ثابت RTLIL::Wire *b)
{
	إذا (a->پورٹ_id && !b->پورٹ_id)
		أرجع سچ;
	إذا (!a->پورٹ_id && b->پورٹ_id)
		أرجع خطأ;

	إذا (a->پورٹ_id == b->پورٹ_id)
		أرجع a->name < b->name;
	أرجع a->پورٹ_id < b->پورٹ_id;
}

فراغ RTLIL::Module::connect(ثابت RTLIL::SigSig &conn)
{
	لكل (خودکار mon : monitors)
		mon->notify_connect(هذا, conn);

	إذا (design)
		لكل (خودکار mon : design->monitors)
			mon->notify_connect(هذا, conn);

	// ignore all attempts to assign constants to other constants
	إذا (conn.پہلا.has_const()) {
		RTLIL::SigSig new_conn;
		لكل (صحيح i = 0; i < Get_ناپ(conn.پہلا); i++)
			إذا (conn.پہلا[i].wire) {
				new_conn.پہلا.append(conn.پہلا[i]);
				new_conn.دوجا.append(conn.دوجا[i]);
			}
		إذا (Get_ناپ(new_conn.پہلا))
			connect(new_conn);
		أرجع;
	}

	إذا (yosys_xtrace) {
		لوگارتھم("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", هذا, log_signal(conn.پہلا), log_signal(conn.دوجا), Get_ناپ(conn.پہلا));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(Get_ناپ(conn.پہلا) == Get_ناپ(conn.دوجا));
	connections_.ڈالو_پٹھ(conn);
}

فراغ RTLIL::Module::connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

فراغ RTLIL::Module::new_connections(ثابت std::سمتیہ<RTLIL::SigSig> &new_conn)
{
	لكل (خودکار mon : monitors)
		mon->notify_connect(هذا, new_conn);

	إذا (design)
		لكل (خودکار mon : design->monitors)
			mon->notify_connect(هذا, new_conn);

	إذا (yosys_xtrace) {
		لوگارتھم("#X# New connections vector in %s:\n", هذا);
		لكل (خودکار &conn: new_conn)
			لوگارتھم("#X#    %s = %s (%d bits)\n", log_signal(conn.پہلا), log_signal(conn.دوجا), Get_ناپ(conn.پہلا));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ثابت std::سمتیہ<RTLIL::SigSig> &RTLIL::Module::connections() ثابت
{
	أرجع connections_;
}

فراغ RTLIL::Module::fixup_ports()
{
	std::سمتیہ<RTLIL::Wire*> all_ports;

	لكل (خودکار &w : wires_)
		إذا (w.دوجا->پورٹ_داخلہ || w.دوجا->پورٹ_output)
			all_ports.ڈالو_پٹھ(w.دوجا);
		وإلا
			w.دوجا->پورٹ_id = 0;

	std::ترتیب_دیو(all_ports.begin(), all_ports.سرا(), fixup_ports_compare);

	إذا (design && design->flagBufferedNormalized) {
		لكل (خودکار &w : wires_)
			إذا (w.دوجا->driverCell_ && w.دوجا->driverCell_->type == ID($داخلہ_پورٹ))
				buf_norm_wire_قطار.داخل_کرو(w.دوجا);

		buf_norm_wire_قطار.داخل_کرو(all_ports.begin(), all_ports.سرا());
	}

	ports.clear();
	لكل (ناپ_t i = 0; i < all_ports.ناپ(); i++) {
		ports.ڈالو_پٹھ(all_ports[i]->name);
		all_ports[i]->پورٹ_id = i+1;
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
	لكل (صحيح i = Get_ناپ(src) - 1; i >= 0; i--) {
		خودکار &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.پہلا)] = it.دوجا;
	}
}

RTLIL::Wire *RTLIL::Module::جوڑو_Wire(RTLIL::IdString name, صحيح چوڑائی)
{
	log_assert(design);
	log_assert(چوڑائی >= 0 && چوڑائی < RTLIL::چوڑائی_حد_بندی);
	RTLIL::Wire *wire = جديد RTLIL::Wire;
	wire->چوڑائی = چوڑائی;
	wire->name = name;
	جوڑو(wire);
	أرجع wire;
}

RTLIL::Wire *RTLIL::Module::جوڑو_Wire(IdString name, ثابت RTLIL::Wire *other)
{
	RTLIL::Wire *wire = جوڑو_Wire(name);
	wire->چوڑائی = other->چوڑائی;
	wire->شروع_وتھ = other->شروع_وتھ;
	wire->پورٹ_id = other->پورٹ_id;
	wire->پورٹ_داخلہ = other->پورٹ_داخلہ;
	wire->پورٹ_output = other->پورٹ_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ثابت RTLIL::Design *src_design = other->module ? other->module->design : خالی_سوچک;
		RTLIL::copy_attr_dict(wire->گن, other->گن, src_design, هذا->design);
	}
	أرجع wire;
}

RTLIL::Cell *RTLIL::Module::جوڑو_Cell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = جديد RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	جوڑو(cell);
	أرجع cell;
}

RTLIL::Cell *RTLIL::Module::جوڑو_Cell(IdString name, ثابت RTLIL::Cell *other)
{
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : خالی_سوچک;
	منطقي cross_pool = src_design && هذا->design && src_design != هذا->design;

	IdString type = other->type.ref();
	إذا (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = جوڑو_Cell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, هذا->design);
	RTLIL::copy_attr_dict(cell->گن, other->گن, src_design, هذا->design);

	إذا (!cross_pool)
		cell->connections_ = other->connections_;
	وإلا
		لكل (صحيح i = Get_ناپ(other->connections_) - 1; i >= 0; i--) {
			خودکار &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.پہلا)] = c.دوجا;
		}
	أرجع cell;
}

RTLIL::حافظہ *RTLIL::Module::جوڑو_حافظہ(IdString name)
{
	log_assert(design);
	RTLIL::حافظہ *mem = جديد RTLIL::حافظہ;
	mem->module = هذا;
	mem->name = name;
	memories[name] = mem;
	أرجع mem;
}

RTLIL::حافظہ *RTLIL::Module::جوڑو_حافظہ(IdString name, ثابت RTLIL::حافظہ *other)
{
	log_assert(design);
	RTLIL::حافظہ *mem = جديد RTLIL::حافظہ;
	mem->module = هذا;
	mem->name = name;
	mem->چوڑائی = other->چوڑائی;
	mem->شروع_وتھ = other->شروع_وتھ;
	mem->ناپ = other->ناپ;
	RTLIL::copy_attr_dict(mem->گن, other->گن,
			other->module ? other->module->design : خالی_سوچک, design);
	memories[name] = mem;
	أرجع mem;
}

RTLIL::Process *RTLIL::Module::جوڑو_Process(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = جديد RTLIL::Process;
	proc->name = name;
	جوڑو(proc);
	أرجع proc;
}

RTLIL::Process *RTLIL::Module::جوڑو_Process(RTLIL::IdString name, ثابت RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : خالی_سوچک;
	إذا (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> گن;
			RTLIL::copy_attr_dict(گن, obj->گن, src_design, design);
			obj->گن = std::move(گن);
		});
		لكل (خودکار *متزامن : proc->syncs)
			لكل (خودکار &mwa : متزامن->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	جوڑو(proc);
	أرجع proc;
}

#تعریف_کرو DEF_طریقہ(_func, y_ناپ, _type) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_چوڑائی] = sig_a.ناپ();       \
		cell->parameters[ID::Y_چوڑائی] = sig_y.ناپ();       \
		cell->سیٹ_پورٹ(ID::A, sig_a);                        \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                        \
		cell->سیٹ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, y_ناپ);    \
		جوڑو ## _func(name, sig_a, sig_y, is_signed, src);   \
		أرجع sig_y;                                       \
	}
DEF_طریقہ(Not,        sig_a.ناپ(), ID($not))
DEF_طریقہ(Pos,        sig_a.ناپ(), ID($pos))
DEF_طریقہ(Neg,        sig_a.ناپ(), ID($neg))
DEF_طریقہ(ReduceAnd,  1, ID($reduce_and))
DEF_طریقہ(ReduceOr,   1, ID($reduce_or))
DEF_طریقہ(ReduceXor,  1, ID($reduce_xor))
DEF_طریقہ(ReduceXnor, 1, ID($reduce_xnor))
DEF_طریقہ(ReduceBool, 1, ID($reduce_bool))
DEF_طریقہ(LogicNot,   1, ID($logic_not))
#تعریف_رد DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, y_ناپ, _type) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقي /* is_signed */, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);           \
		cell->parameters[ID::چوڑائی_2] = sig_a.ناپ();         \
		cell->سیٹ_پورٹ(ID::A, sig_a);                        \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                        \
		cell->سیٹ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, y_ناپ);    \
		جوڑو ## _func(name, sig_a, sig_y, is_signed, src);   \
		أرجع sig_y;                                       \
	}
DEF_طریقہ(Buf,     sig_a.ناپ(), ID($buf))
DEF_طریقہ(Barrier, sig_a.ناپ(), ID($barrier))
#تعریف_رد DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, y_ناپ, _type) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_چوڑائی] = sig_a.ناپ();       \
		cell->parameters[ID::B_چوڑائی] = sig_b.ناپ();       \
		cell->parameters[ID::Y_چوڑائی] = sig_y.ناپ();       \
		cell->سیٹ_پورٹ(ID::A, sig_a);                        \
		cell->سیٹ_پورٹ(ID::B, sig_b);                        \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                        \
		cell->سیٹ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, y_ناپ);         \
		جوڑو ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_طریقہ(And,      max(sig_a.ناپ(), sig_b.ناپ()), ID($and))
DEF_طریقہ(Or,       max(sig_a.ناپ(), sig_b.ناپ()), ID($or))
DEF_طریقہ(Xor,      max(sig_a.ناپ(), sig_b.ناپ()), ID($xor))
DEF_طریقہ(Xnor,     max(sig_a.ناپ(), sig_b.ناپ()), ID($xnor))
DEF_طریقہ(کھسکاؤ_2,    sig_a.ناپ(), ID($کھسکاؤ))
DEF_طریقہ(Lt,       1, ID($lt))
DEF_طریقہ(Le,       1, ID($le))
DEF_طریقہ(Eq,       1, ID($eq))
DEF_طریقہ(Ne,       1, ID($ne))
DEF_طریقہ(Eqx,      1, ID($eqx))
DEF_طریقہ(Nex,      1, ID($nex))
DEF_طریقہ(Ge,       1, ID($ge))
DEF_طریقہ(Gt,       1, ID($gt))
DEF_طریقہ(جوڑو_2,      max(sig_a.ناپ(), sig_b.ناپ()), ID($جوڑو))
DEF_طریقہ(Sub,      max(sig_a.ناپ(), sig_b.ناپ()), ID($sub))
DEF_طریقہ(Mul,      max(sig_a.ناپ(), sig_b.ناپ()), ID($mul))
DEF_طریقہ(Div,      max(sig_a.ناپ(), sig_b.ناپ()), ID($div))
DEF_طریقہ(Mod,      max(sig_a.ناپ(), sig_b.ناپ()), ID($mod))
DEF_طریقہ(DivFloor, max(sig_a.ناپ(), sig_b.ناپ()), ID($divfloor))
DEF_طریقہ(ModFloor, max(sig_a.ناپ(), sig_b.ناپ()), ID($modfloor))
DEF_طریقہ(LogicAnd, 1, ID($logic_and))
DEF_طریقہ(LogicOr,  1, ID($logic_or))
#تعریف_رد DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, y_ناپ, _type) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = خطأ;             \
		cell->parameters[ID::A_چوڑائی] = sig_a.ناپ();       \
		cell->parameters[ID::B_چوڑائی] = sig_b.ناپ();       \
		cell->parameters[ID::Y_چوڑائی] = sig_y.ناپ();       \
		cell->سیٹ_پورٹ(ID::A, sig_a);                        \
		cell->سیٹ_پورٹ(ID::B, sig_b);                        \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                        \
		cell->سیٹ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, y_ناپ);         \
		جوڑو ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_طریقہ(Shl,      sig_a.ناپ(), ID($shl))
DEF_طریقہ(Shr,      sig_a.ناپ(), ID($shr))
DEF_طریقہ(Sshl,     sig_a.ناپ(), ID($sshl))
DEF_طریقہ(Sshr,     sig_a.ناپ(), ID($sshr))
#تعریف_رد DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, y_ناپ, _type) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = خطأ;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_چوڑائی] = sig_a.ناپ();       \
		cell->parameters[ID::B_چوڑائی] = sig_b.ناپ();       \
		cell->parameters[ID::Y_چوڑائی] = sig_y.ناپ();       \
		cell->سیٹ_پورٹ(ID::A, sig_a);                        \
		cell->سیٹ_پورٹ(ID::B, sig_b);                        \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                        \
		cell->سیٹ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, y_ناپ);         \
		جوڑو ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_طریقہ(Shiftx,      sig_a.ناپ(), ID($shiftx))
#تعریف_رد DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);                 \
		cell->parameters[ID::چوڑائی_2] = sig_a.ناپ();               \
		إذا (_pmux) cell->parameters[ID::S_چوڑائی] = sig_s.ناپ();  \
		cell->سیٹ_پورٹ(ID::A, sig_a);                              \
		cell->سیٹ_پورٹ(ID::B, sig_b);                              \
		cell->سیٹ_پورٹ(ID::S, sig_s);                              \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                              \
		cell->سیٹ_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, sig_a.ناپ());     \
		جوڑو ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		أرجع sig_y;                                             \
	}
DEF_طریقہ(Mux,      ID($mux),        0)
DEF_طریقہ(Bwmux,    ID($bwmux),      0)
DEF_طریقہ(Pmux,     ID($pmux),       1)
#تعریف_رد DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);                 \
		cell->parameters[ID::چوڑائی_2] = _demux ? sig_a.ناپ() : sig_y.ناپ(); \
		cell->parameters[ID::S_چوڑائی] = sig_s.ناپ();             \
		cell->سیٹ_پورٹ(ID::A, sig_a);                              \
		cell->سیٹ_پورٹ(ID::S, sig_s);                              \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                              \
		cell->سیٹ_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, _demux ? sig_a.ناپ() << sig_s.ناپ() : sig_a.ناپ() >> sig_s.ناپ()); \
		جوڑو ## _func(name, sig_a, sig_s, sig_y, src);             \
		أرجع sig_y;                                             \
	}
DEF_طریقہ(Bmux,     ID($bmux),       0)
DEF_طریقہ(Demux,    ID($demux),      1)
#تعریف_رد DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, _type) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);                 \
		cell->parameters[ID::چوڑائی_2] = sig_a.ناپ();               \
		cell->سیٹ_پورٹ(ID::A, sig_a);                              \
		cell->سیٹ_پورٹ(ID::B, sig_b);                              \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                              \
		cell->سیٹ_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, sig_a.ناپ());     \
		جوڑو ## _func(name, sig_a, sig_s, sig_y, src);             \
		أرجع sig_y;                                             \
	}
DEF_طریقہ(Bweqx,    ID($bweqx))
#تعریف_رد DEF_طریقہ

#تعریف_کرو DEF_طریقہ_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig2, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);         \
		cell->سیٹ_پورٹ("\\" #_P1, sig1);                   \
		cell->سیٹ_پورٹ("\\" #_P2, sig2);                   \
		cell->سیٹ_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_ثنائی_ہندسہ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت std::string &src) { \
		RTLIL::Sig_ثنائی_ہندسہ sig2 = جوڑو_Wire(NEW_ID);             \
		جوڑو ## _func(name, sig1, sig2, src);              \
		أرجع sig2;                                      \
	}
#تعریف_کرو DEF_طریقہ_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig2, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig3, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);         \
		cell->سیٹ_پورٹ("\\" #_P1, sig1);                   \
		cell->سیٹ_پورٹ("\\" #_P2, sig2);                   \
		cell->سیٹ_پورٹ("\\" #_P3, sig3);                   \
		cell->سیٹ_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_ثنائی_ہندسہ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig2, ثابت std::string &src) { \
		RTLIL::Sig_ثنائی_ہندسہ sig3 = جوڑو_Wire(NEW_ID);             \
		جوڑو ## _func(name, sig1, sig2, sig3, src);        \
		أرجع sig3;                                      \
	}
#تعریف_کرو DEF_طریقہ_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig2, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig3, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig4, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);         \
		cell->سیٹ_پورٹ("\\" #_P1, sig1);                   \
		cell->سیٹ_پورٹ("\\" #_P2, sig2);                   \
		cell->سیٹ_پورٹ("\\" #_P3, sig3);                   \
		cell->سیٹ_پورٹ("\\" #_P4, sig4);                   \
		cell->سیٹ_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_ثنائی_ہندسہ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig2, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig3, ثابت std::string &src) { \
		RTLIL::Sig_ثنائی_ہندسہ sig4 = جوڑو_Wire(NEW_ID);             \
		جوڑو ## _func(name, sig1, sig2, sig3, sig4, src);  \
		أرجع sig4;                                      \
	}
#تعریف_کرو DEF_طریقہ_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig2, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig3, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig4, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig5, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);         \
		cell->سیٹ_پورٹ("\\" #_P1, sig1);                   \
		cell->سیٹ_پورٹ("\\" #_P2, sig2);                   \
		cell->سیٹ_پورٹ("\\" #_P3, sig3);                   \
		cell->سیٹ_پورٹ("\\" #_P4, sig4);                   \
		cell->سیٹ_پورٹ("\\" #_P5, sig5);                   \
		cell->سیٹ_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_ثنائی_ہندسہ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig2, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig3, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig4, ثابت std::string &src) { \
		RTLIL::Sig_ثنائی_ہندسہ sig5 = جوڑو_Wire(NEW_ID);                  \
		جوڑو ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		أرجع sig5;                                           \
	}
DEF_طریقہ_2(BufGate,    ID($_BUF_),    A, Y)
DEF_طریقہ_2(NotGate,    ID($_NOT_),    A, Y)
DEF_طریقہ_3(AndGate,    ID($_AND_),    A, B, Y)
DEF_طریقہ_3(NandGate,   ID($_NAND_),   A, B, Y)
DEF_طریقہ_3(OrGate,     ID($_OR_),     A, B, Y)
DEF_طریقہ_3(NorGate,    ID($_NOR_),    A, B, Y)
DEF_طریقہ_3(XorGate,    ID($_XOR_),    A, B, Y)
DEF_طریقہ_3(XnorGate,   ID($_XNOR_),   A, B, Y)
DEF_طریقہ_3(AndnotGate, ID($_ANDNOT_), A, B, Y)
DEF_طریقہ_3(OrnotGate,  ID($_ORNOT_),  A, B, Y)
DEF_طریقہ_4(MuxGate,    ID($_MUX_),    A, B, S, Y)
DEF_طریقہ_4(NmuxGate,   ID($_NMUX_),   A, B, S, Y)
DEF_طریقہ_4(Aoi3Gate,   ID($_AOI3_),   A, B, C, Y)
DEF_طریقہ_4(Oai3Gate,   ID($_OAI3_),   A, B, C, Y)
DEF_طریقہ_5(Aoi4Gate,   ID($_AOI4_),   A, B, C, D, Y)
DEF_طریقہ_5(Oai4Gate,   ID($_OAI4_),   A, B, C, D, Y)
#تعریف_رد DEF_طریقہ_2
#تعریف_رد DEF_طریقہ_3
#تعریف_رد DEF_طریقہ_4
#تعریف_رد DEF_طریقہ_5

RTLIL::Cell* RTLIL::Module::جوڑو_Pow(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي a_signed, منطقي b_signed, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($قوت));
	cell->parameters[ID::A_SIGNED] = a_signed;
	cell->parameters[ID::B_SIGNED] = b_signed;
	cell->parameters[ID::A_چوڑائی] = sig_a.ناپ();
	cell->parameters[ID::B_چوڑائی] = sig_b.ناپ();
	cell->parameters[ID::Y_چوڑائی] = sig_y.ناپ();
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::B, sig_b);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Fa(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_x, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($fa));
	cell->parameters[ID::چوڑائی_2] = sig_a.ناپ();
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::B, sig_b);
	cell->سیٹ_پورٹ(ID::C, sig_c);
	cell->سیٹ_پورٹ(ID::X, sig_x);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_ٹکڑا(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const وتھ, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($ٹکڑا));
	cell->parameters[ID::A_چوڑائی] = sig_a.ناپ();
	cell->parameters[ID::Y_چوڑائی] = sig_y.ناپ();
	cell->parameters[ID::وتھ_2] = وتھ;
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Concat(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($concat));
	cell->parameters[ID::A_چوڑائی] = sig_a.ناپ();
	cell->parameters[ID::B_چوڑائی] = sig_b.ناپ();
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::B, sig_b);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Lut(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::چوڑائی_2] = sig_a.ناپ();
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Tribuf(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($tribuf));
	cell->parameters[ID::چوڑائی_2] = sig_a.ناپ();
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Assert(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($أكد));
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Assume(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($assume));
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_جیوندا(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($جیوندا));
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Fair(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($fair));
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Cover(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($cover));
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Equiv(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($equiv));
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::B, sig_b);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr, ثابت RTLIL::SigSpec &sig_q, منطقي سیٹ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($sr));
	cell->parameters[ID::سیٹ_POLARITY] = سیٹ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::CLR, sig_clr);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Ff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($ff));
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($dffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dffsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي سیٹ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($dffsr));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::سیٹ_POLARITY] = سیٹ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::CLR, sig_clr);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dffsre(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, منطقي سیٹ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($dffsre));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::سیٹ_POLARITY] = سیٹ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::CLR, sig_clr);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Adff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_قدر, منطقي clk_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_قدر] = arst_قدر;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::ARST, sig_arst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Adffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_قدر, منطقي clk_polarity, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_قدر] = arst_قدر;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::ARST, sig_arst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Aldff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($aldff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::ALOAD, sig_aload);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::AD, sig_ad);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Aldffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي en_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($aldffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::ALOAD, sig_aload);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::AD, sig_ad);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sdff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_قدر, منطقي clk_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_قدر] = srst_قدر;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::SRST, sig_srst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sdffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_قدر, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_قدر] = srst_قدر;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::SRST, sig_srst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sdffce(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_قدر, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_قدر] = srst_قدر;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::SRST, sig_srst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Adlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_قدر, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_قدر] = arst_قدر;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::ARST, sig_arst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dlatchsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, منطقي سیٹ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($dlatchsr));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::سیٹ_POLARITY] = سیٹ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::CLR, sig_clr);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		ثابت RTLIL::SigSpec &sig_q, منطقي سیٹ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_SR_%c%c_", سیٹ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::S, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::R, sig_clr);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Ff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($_FF_));
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dffsr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي سیٹ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', سیٹ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::S, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::R, sig_clr);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dffsre_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, منطقي سیٹ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', سیٹ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::S, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::R, sig_clr);
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Adff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_قدر, منطقي clk_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_قدر ? '1' : '0'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::R, sig_arst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Adffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_قدر, منطقي clk_polarity, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_قدر ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::R, sig_arst);
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Aldff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::L, sig_aload);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::AD, sig_ad);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Aldffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي en_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::L, sig_aload);
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::AD, sig_ad);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sdff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_قدر, منطقي clk_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_قدر ? '1' : '0'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::R, sig_srst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sdffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_قدر, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_قدر ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::R, sig_srst);
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sdffce_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_قدر, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_قدر ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::R, sig_srst);
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Adlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_قدر, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_قدر ? '1' : '0'));
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::R, sig_arst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dlatchsr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, منطقي سیٹ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', سیٹ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::S, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::R, sig_clr);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Anyinit(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($anyinit));
	cell->parameters[ID::چوڑائی_2] = sig_q.ناپ();
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, صحيح چوڑائی, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, چوڑائی);
	Cell *cell = جوڑو_Cell(name, ID($anyconst));
	cell->سیٹ_Param(ID::چوڑائی_2, چوڑائی);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, صحيح چوڑائی, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, چوڑائی);
	Cell *cell = جوڑو_Cell(name, ID($anyseq));
	cell->سیٹ_Param(ID::چوڑائی_2, چوڑائی);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, صحيح چوڑائی, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, چوڑائی);
	Cell *cell = جوڑو_Cell(name, ID($allconst));
	cell->سیٹ_Param(ID::چوڑائی_2, چوڑائی);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, صحيح چوڑائی, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, چوڑائی);
	Cell *cell = جوڑو_Cell(name, ID($allseq));
	cell->سیٹ_Param(ID::چوڑائی_2, چوڑائی);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID);
	Cell *cell = جوڑو_Cell(name, ID($initstate));
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::سیٹ_ٹیگ_2(RTLIL::IdString name, ثابت std::string &ٹیگ, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, sig_a.ناپ());
	Cell *cell = جوڑو_Cell(name, ID($سیٹ_ٹیگ));
	cell->parameters[ID::چوڑائی_2] = sig_a.ناپ();
	cell->parameters[ID::ٹیگ_2] = ٹیگ;
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_s);
	cell->سیٹ_پورٹ(ID::CLR, sig_c);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	أرجع sig;
}

RTLIL::Cell* RTLIL::Module::جوڑو_سیٹ_ٹیگ(RTLIL::IdString name, ثابت std::string &ٹیگ, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	Cell *cell = جوڑو_Cell(name, ID($سیٹ_ٹیگ));
	cell->parameters[ID::چوڑائی_2] = sig_a.ناپ();
	cell->parameters[ID::ٹیگ_2] = ٹیگ;
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_s);
	cell->سیٹ_پورٹ(ID::CLR, sig_c);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::Get_ٹیگ(RTLIL::IdString name, ثابت std::string &ٹیگ, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, sig_a.ناپ());
	Cell *cell = جوڑو_Cell(name, ID($get_ٹیگ));
	cell->parameters[ID::چوڑائی_2] = sig_a.ناپ();
	cell->parameters[ID::ٹیگ_2] = ٹیگ;
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	أرجع sig;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Overwrite_ٹیگ(RTLIL::IdString name, ثابت std::string &ٹیگ, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($overwrite_ٹیگ));
	cell->parameters[ID::چوڑائی_2] = sig_a.ناپ();
	cell->parameters[ID::ٹیگ_2] = ٹیگ;
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_s);
	cell->سیٹ_پورٹ(ID::CLR, sig_c);
	cell->سیٹ_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::Original_ٹیگ(RTLIL::IdString name, ثابت std::string &ٹیگ, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, sig_a.ناپ());
	Cell *cell = جوڑو_Cell(name, ID($original_ٹیگ));
	cell->parameters[ID::چوڑائی_2] = sig_a.ناپ();
	cell->parameters[ID::ٹیگ_2] = ٹیگ;
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, sig_e.ناپ());
	Cell *cell = جوڑو_Cell(name, ID($future_ff));
	cell->parameters[ID::چوڑائی_2] = sig_e.ناپ();
	cell->سیٹ_پورٹ(ID::A, sig_e);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	أرجع sig;
}

std::string RTLIL::Module::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(هذا), design, خطأ, سچ, خطأ, RTLIL_BACKEND::Dump_منوال::Readable);
	أرجع f.str();
}

RTLIL::Wire::Wire()
{
	ساكن بنانشانی صحيح hashidx_گنتی = 123456789;
	hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
	hashidx_ = hashidx_گنتی;

	module = خالی_سوچک;
	چوڑائی = 1;
	شروع_وتھ = 0;
	پورٹ_id = 0;
	پورٹ_داخلہ = خطأ;
	پورٹ_output = خطأ;
	upto = خطأ;
	is_signed = خطأ;

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->داخل_کرو(std::pair<بنانشانی صحيح, RTLIL::Wire*>(hashidx_, هذا));
#جے_انت
}

RTLIL::Wire::~Wire()
{
#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#جے_انت
}

std::string RTLIL::Wire::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", هذا, design(), RTLIL_BACKEND::Dump_منوال::Readable);
	أرجع f.str();
}

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
ساكن std::map<بنانشانی صحيح, RTLIL::Wire*> all_wires;
std::map<بنانشانی صحيح, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(فراغ)
{
	أرجع &all_wires;
}
#جے_انت

RTLIL::حافظہ::حافظہ()
{
	ساكن بنانشانی صحيح hashidx_گنتی = 123456789;
	hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
	hashidx_ = hashidx_گنتی;

	چوڑائی = 1;
	شروع_وتھ = 0;
	ناپ = 0;
#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::حافظہ::get_all_memorys()->داخل_کرو(std::pair<بنانشانی صحيح, RTLIL::حافظہ*>(hashidx_, هذا));
#جے_انت
}

std::string RTLIL::حافظہ::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_حافظہ(f, "", هذا, design(), RTLIL_BACKEND::Dump_منوال::Readable);
	أرجع f.str();
}

RTLIL::Process::Process() : module(خالی_سوچک)
{
	ساكن بنانشانی صحيح hashidx_گنتی = 123456789;
	hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
	hashidx_ = hashidx_گنتی;
}

std::string RTLIL::Process::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", هذا, design(), RTLIL_BACKEND::Dump_منوال::Readable);
	أرجع f.str();
}

RTLIL::Cell::Cell() : module(خالی_سوچک)
{
	ساكن بنانشانی صحيح hashidx_گنتی = 123456789;
	hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
	hashidx_ = hashidx_گنتی;

	// log("#memtrace# %p\n", this);
	memhasher();

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->داخل_کرو(std::pair<بنانشانی صحيح, RTLIL::Cell*>(hashidx_, هذا));
#جے_انت
}

RTLIL::Cell::~Cell()
{
#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#جے_انت
}

std::string RTLIL::Cell::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", هذا, design(), RTLIL_BACKEND::Dump_منوال::Readable);
	أرجع f.str();
}

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
ساكن std::map<بنانشانی صحيح, RTLIL::Cell*> all_cells;
std::map<بنانشانی صحيح, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(فراغ)
{
	أرجع &all_cells;
}
#جے_انت

منطقي RTLIL::Cell::has_پورٹ(RTLIL::IdString portname) ثابت
{
	أرجع connections_.گنتی(portname) != 0;
}

// bufnorm

ثابت RTLIL::SigSpec &RTLIL::Cell::get_پورٹ(RTLIL::IdString portname) ثابت
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

منطقي RTLIL::Cell::داخلہ(IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_معلوم(type))
		أرجع yosys_celltypes.cell_داخلہ(type.ref(), portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : خالی_سوچک;
		أرجع w && w->پورٹ_داخلہ;
	}
	أرجع خطأ;
}

منطقي RTLIL::Cell::output(RTLIL::IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_معلوم(type))
		أرجع yosys_celltypes.cell_output(type, portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : خالی_سوچک;
		أرجع w && w->پورٹ_output;
	}
	أرجع خطأ;
}

RTLIL::پورٹ_Dir RTLIL::Cell::پورٹ_dir(RTLIL::IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_معلوم(type.ref()))
		أرجع yosys_celltypes.cell_پورٹ_dir(type, portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		إذا (m == خالی_سوچک)
			أرجع پورٹ_Dir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		إذا (w == خالی_سوچک)
			أرجع پورٹ_Dir::PD_UNKNOWN;
		أرجع پورٹ_Dir(w->پورٹ_داخلہ + w->پورٹ_output * 2);
	}
	أرجع پورٹ_Dir::PD_UNKNOWN;
}

منطقي RTLIL::Cell::hasParam(RTLIL::IdString paramname) ثابت
{
	أرجع parameters.گنتی(paramname) != 0;
}

فراغ RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

فراغ RTLIL::Cell::سیٹ_Param(RTLIL::IdString paramname, RTLIL::Const قدر)
{
	parameters[paramname] = std::move(قدر);
}

ثابت RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ثابت
{
	ثابت خودکار &it = parameters.لبھو(paramname);
	إذا (it != parameters.سرا())
		أرجع it->دوجا;
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		إذا (m)
			أرجع m->parameter_default_اقدار.at(paramname);
	}
	ارم std::باہرلا_دا_range("Cell::getParam()");
}

فراغ RTLIL::Cell::ترتیب_دیو()
{
	connections_.ترتیب_دیو(ترتیب_دیو_by_id_str(module->twines()));
	parameters.ترتیب_دیو(ترتیب_دیو_by_id_str(module->twines()));
	گن.ترتیب_دیو(ترتیب_دیو_by_id_str(module->twines()));
}

فراغ RTLIL::Cell::check()
{
#جے_بنا_تعریف NDEBUG
	اندرونی_Cell_Checker checker(عدم, هذا);
	checker.check();
#جے_انت
}

فراغ RTLIL::Cell::fixup_parameters(منطقي سیٹ_a_signed, منطقي سیٹ_b_signed)
{
	إذا (type.is_عوامی() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		أرجع;

	إذا (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::چوڑائی_2] = Get_ناپ(connections_[ID::Y]);
		إذا (type.وچ(ID($pmux), ID($bmux)))
			parameters[ID::S_چوڑائی] = Get_ناپ(connections_[ID::S]);
		check();
		أرجع;
	}

	إذا (type == ID($demux)) {
		parameters[ID::چوڑائی_2] = Get_ناپ(connections_[ID::A]);
		parameters[ID::S_چوڑائی] = Get_ناپ(connections_[ID::S]);
		check();
		أرجع;
	}

	إذا (type == ID($lut) || type == ID($sop)) {
		parameters[ID::چوڑائی_2] = Get_ناپ(connections_[ID::A]);
		أرجع;
	}

	إذا (type == ID($fa)) {
		parameters[ID::چوڑائی_2] = Get_ناپ(connections_[ID::Y]);
		أرجع;
	}

	إذا (type == ID($lcu)) {
		parameters[ID::چوڑائی_2] = Get_ناپ(connections_[ID::CO]);
		أرجع;
	}

	إذا (type == ID($macc_v2)) {
		parameters[ID::Y_چوڑائی] = Get_ناپ(connections_[ID::Y]);
		أرجع;
	}

	منطقي signedness_ab = !type.وچ(ID($ٹکڑا), ID($concat), ID($macc));

	إذا (connections_.گنتی(ID::A)) {
		إذا (signedness_ab) {
			إذا (سیٹ_a_signed)
				parameters[ID::A_SIGNED] = سچ;
			وإلا إذا (parameters.گنتی(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = خطأ;
		}
		parameters[ID::A_چوڑائی] = Get_ناپ(connections_[ID::A]);
	}

	إذا (connections_.گنتی(ID::B)) {
		إذا (signedness_ab) {
			إذا (سیٹ_b_signed)
				parameters[ID::B_SIGNED] = سچ;
			وإلا إذا (parameters.گنتی(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = خطأ;
		}
		parameters[ID::B_چوڑائی] = Get_ناپ(connections_[ID::B]);
	}

	إذا (connections_.گنتی(ID::Y) && type != ID($concat))
		parameters[ID::Y_چوڑائی] = Get_ناپ(connections_[ID::Y]);

	إذا (connections_.گنتی(ID::Q))
		parameters[ID::چوڑائی_2] = Get_ناپ(connections_[ID::Q]);

	check();
}

منطقي RTLIL::Cell::has_keep_attr() ثابت {
	أرجع get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

منطقي RTLIL::Cell::has_memid() ثابت
{
	أرجع type.وچ(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

منطقي RTLIL::Cell::is_mem_cell() ثابت
{
	أرجع type.وچ(ID($mem), ID($mem_v2)) || has_memid();
}

منطقي RTLIL::Cell::is_builtin_ff() ثابت {
	أرجع StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ)
{
	wire = ثنائی_ہندسہ.wire;
	وتھ = 0;
	إذا (wire == عدم)
		انکڑے = {ثنائی_ہندسہ.انکڑے};
	وإلا
		وتھ = ثنائی_ہندسہ.وتھ;
	چوڑائی = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::کڈھو(صحيح وتھ, صحيح لمائی) ثابت
{
	log_assert(وتھ >= 0);
	log_assert(لمائی >= 0);
	log_assert(وتھ + لمائی <= چوڑائی);
	RTLIL::SigChunk ret;
	إذا (wire) {
		ret.wire = wire;
		ret.وتھ = هذا->وتھ + وتھ;
		ret.چوڑائی = لمائی;
	} وإلا {
		لكل (صحيح i = 0; i < لمائی; i++)
			ret.انکڑے.ڈالو_پٹھ(انکڑے[وتھ+i]);
		ret.چوڑائی = لمائی;
	}
	أرجع ret;
}

RTLIL::Sig_ثنائی_ہندسہ RTLIL::SigChunk::عامل[](صحيح وتھ) ثابت
{
	log_assert(وتھ >= 0);
	log_assert(وتھ < چوڑائی);
	RTLIL::Sig_ثنائی_ہندسہ ret;
	إذا (wire) {
		ret.wire = wire;
		ret.وتھ = هذا->وتھ + وتھ;
	} وإلا {
		ret.انکڑے = انکڑے[وتھ];
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

	إذا (وتھ != other.وتھ)
		أرجع وتھ < other.وتھ;

	إذا (چوڑائی != other.چوڑائی)
		أرجع چوڑائی < other.چوڑائی;

	أرجع انکڑے < other.انکڑے;
}

منطقي RTLIL::SigChunk::عامل ==(ثابت RTLIL::SigChunk &other) ثابت
{
	أرجع wire == other.wire && چوڑائی == other.چوڑائی && وتھ == other.وتھ && انکڑے == other.انکڑے;
}

منطقي RTLIL::SigChunk::عامل !=(ثابت RTLIL::SigChunk &other) ثابت
{
	إذا (*هذا == other)
		أرجع خطأ;
	أرجع سچ;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_خالی_تھاواں_بٹ();
	log_assert(parts.ناپ() > 0);
	خودکار ie = parts.begin();
	خودکار it = ie + parts.ناپ() - 1;
	طالما (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::Const &قدر)
{
	إذا (Get_ناپ(قدر) != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(قدر);
	} وإلا {
		init_خالی_تھاواں_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&قدر)
{
	إذا (Get_ناپ(قدر) != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(قدر);
	} وإلا {
		init_خالی_تھاواں_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::SigChunk &chunk)
{
	إذا (chunk.چوڑائی != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(chunk);
	} وإلا {
		init_خالی_تھاواں_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	إذا (chunk.چوڑائی != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(chunk);
	} وإلا {
		init_خالی_تھاواں_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	إذا (wire->چوڑائی != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(wire);
	} وإلا {
		init_خالی_تھاواں_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, صحيح وتھ, صحيح چوڑائی)
{
	إذا (چوڑائی != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(wire, وتھ, چوڑائی);
	} وإلا {
		init_خالی_تھاواں_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::string &str)
{
	إذا (str.ناپ() != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(str);
	} وإلا {
		init_خالی_تھاواں_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(صحيح val, صحيح چوڑائی)
{
	إذا (چوڑائی != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(val, چوڑائی);
	} وإلا
		init_خالی_تھاواں_بٹ();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State ثنائی_ہندسہ, صحيح چوڑائی)
{
	إذا (چوڑائی != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(ثنائی_ہندسہ, چوڑائی);
	} وإلا
		init_خالی_تھاواں_بٹ();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ, صحيح چوڑائی)
{
	إذا (چوڑائی != 0) {
		إذا (ثنائی_ہندسہ.wire == عدم) {
			rep_ = CHUNK;
			جديد (&chunk_) RTLIL::SigChunk(ثنائی_ہندسہ.انکڑے, چوڑائی);
		} وإلا إذا (چوڑائی == 1) {
			rep_ = CHUNK;
			جديد (&chunk_) RTLIL::SigChunk(ثنائی_ہندسہ);
		} وإلا {
			init_خالی_تھاواں_بٹ();
			بٹ_2.reserve(چوڑائی);
			لكل (صحيح i = 0; i < چوڑائی; i++)
				بٹ_2.ڈالو_پٹھ(ثنائی_ہندسہ);
		}
	} وإلا
		init_خالی_تھاواں_بٹ();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::سمتیہ<RTLIL::SigChunk> &chunks)
{
	init_خالی_تھاواں_بٹ();
	لكل (ثابت خودکار &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> &بٹ)
{
	init_خالی_تھاواں_بٹ();
	لكل (ثابت خودکار &ثنائی_ہندسہ : بٹ)
		append(ثنائی_ہندسہ);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &بٹ)
{
	init_خالی_تھاواں_بٹ();
	لكل (ثابت خودکار &ثنائی_ہندسہ : بٹ)
		append(ثنائی_ہندسہ);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::سیٹ<RTLIL::Sig_ثنائی_ہندسہ> &بٹ)
{
	init_خالی_تھاواں_بٹ();
	لكل (ثابت خودکار &ثنائی_ہندسہ : بٹ)
		append(ثنائی_ہندسہ);
	check();
}

RTLIL::SigSpec::SigSpec(منطقي ثنائی_ہندسہ)
{
	rep_ = CHUNK;
	جديد (&chunk_) RTLIL::SigChunk(ثنائی_ہندسہ ? RTLIL::S1 : RTLIL::S0);
	check();
}

فراغ RTLIL::SigSpec::Chunks::const_iterator::اگلا_chunk_بٹ()
{
	صحيح بٹ_ناپ = Get_ناپ(spec.بٹ_2);
	إذا (ثنائی_ہندسہ_اشاریہ >= بٹ_ناپ)
		أرجع;
	صحيح i = ثنائی_ہندسہ_اشاریہ;
	ثابت Sig_ثنائی_ہندسہ &ثنائی_ہندسہ = spec.بٹ_2[i++];
	chunk.wire = ثنائی_ہندسہ.wire;
	chunk.انکڑے.clear();
	إذا (ثنائی_ہندسہ.is_wire()) {
		chunk.وتھ = ثنائی_ہندسہ.وتھ;
		طالما (i < بٹ_ناپ && spec.بٹ_2[i].wire == ثنائی_ہندسہ.wire &&
			spec.بٹ_2[i].وتھ == ثنائی_ہندسہ.وتھ + i - ثنائی_ہندسہ_اشاریہ)
			++i;
	} وإلا {
		chunk.وتھ = 0;
		chunk.انکڑے.ڈالو_پٹھ(ثنائی_ہندسہ.انکڑے);
		طالما (i < بٹ_ناپ && !spec.بٹ_2[i].is_wire()) {
			chunk.انکڑے.ڈالو_پٹھ(spec.بٹ_2[i].انکڑے);
			++i;
		}
	}
	chunk.چوڑائی = i - ثنائی_ہندسہ_اشاریہ;
}

فراغ RTLIL::SigSpec::unpack()
{
	إذا (rep_ == بٹ_3)
		أرجع;

	std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ;
	بٹ.reserve(chunk_.چوڑائی);
	لكل (صحيح i = 0; i < chunk_.چوڑائی; i++)
		بٹ.emplace_پٹھ(chunk_, i);

	chunk_.~SigChunk();
	rep_ = بٹ_3;
	جديد (&بٹ_2) std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>(std::move(بٹ));
}

فراغ RTLIL::SigSpec::try_repack()
{
	إذا (rep_ != بٹ_3)
		أرجع;

	صحيح بٹ_ناپ = Get_ناپ(بٹ_2);
	إذا (بٹ_ناپ == 0)
		أرجع;
	إذا (بٹ_2[0].is_wire()) {
		لكل (صحيح i = 1; i < بٹ_ناپ; i++)
			إذا (بٹ_2[0].wire != بٹ_2[i].wire || بٹ_2[0].وتھ + i != بٹ_2[i].وتھ)
				أرجع;
		SigChunk chunk(بٹ_2[0].wire, بٹ_2[0].وتھ, بٹ_ناپ);
		بٹ_2.~سمتیہ();
		rep_ = CHUNK;
		جديد (&chunk_) SigChunk(std::move(chunk));
		أرجع;
	}
	std::سمتیہ<RTLIL::State> بٹ;
	بٹ.reserve(بٹ_ناپ);
	بٹ.ڈالو_پٹھ(بٹ_2[0].انکڑے);
	لكل (صحيح i = 1; i < بٹ_ناپ; i++) {
		إذا (بٹ_2[i].is_wire())
			أرجع;
		بٹ.ڈالو_پٹھ(بٹ_2[i].انکڑے);
	}
	بٹ_2.~سمتیہ();
	rep_ = CHUNK;
	جديد (&chunk_) SigChunk(std::move(بٹ));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ثابت
{
	Hasher h;
	لكل (خودکار &c : chunks())
		إذا (c.wire == عدم) {
			لكل (خودکار &v : c.انکڑے)
				h.eat(v);
		} وإلا {
			h.eat(c.wire->name.ref());
			h.eat(c.وتھ);
			h.eat(c.چوڑائی);
		}
	Hasher::hash_t نتیجہ = h.yield();
	إذا (نتیجہ == 0)
		نتیجہ = 1;
	hash_.سیٹ(نتیجہ);
	أرجع نتیجہ;
}

فراغ RTLIL::SigSpec::ترتیب_دیو()
{
	unpack();
	std::ترتیب_دیو(بٹ_2.begin(), بٹ_2.سرا());
	hash_.clear();
	try_repack();
}

فراغ RTLIL::SigSpec::ترتیب_دیو_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::سمتیہ<Sig_ثنائی_ہندسہ> unique_بٹ = بٹ_2;
	std::ترتیب_دیو(unique_بٹ.begin(), unique_بٹ.سرا());
	خودکار آخری = std::unique(unique_بٹ.begin(), unique_بٹ.سرا());
	unique_بٹ.erase(آخری, unique_بٹ.سرا());

	*هذا = unique_بٹ;
	hash_.clear();
	try_repack();
}

فراغ RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &نمونہ, ثابت RTLIL::SigSpec &with)
{
	replace(نمونہ, with, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &نمونہ, ثابت RTLIL::SigSpec &with, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(ناپ() == other->ناپ());
	log_assert(نمونہ.ناپ() == with.ناپ());

	dict<RTLIL::Sig_ثنائی_ہندسہ, صحيح> نمونہ_منزل_with;
	صحيح نمونہ_ناپ = نمونہ.ناپ();
	لكل (صحيح i = 0; i < نمونہ_ناپ; i++) {
		Sig_ثنائی_ہندسہ نمونہ_ثنائی_ہندسہ = نمونہ[i];
		إذا (نمونہ_ثنائی_ہندسہ.wire != عدم) {
			نمونہ_منزل_with.emplace(نمونہ_ثنائی_ہندسہ, i);
		}
	}

	صحيح this_ناپ = ناپ();
	منطقي other_modified = خطأ;
	لكل (صحيح j = 0; j < this_ناپ; j++) {
		خودکار it = نمونہ_منزل_with.لبھو((*هذا)[j]);
		إذا (it != نمونہ_منزل_with.سرا()) {
			other_modified = سچ;
			(*other)[j] = with[it->دوجا];
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::replace(ثابت dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قاعدے)
{
	replace(قاعدے, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قاعدے, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(ناپ() == other->ناپ());

	إذا (قاعدے.خالی_تھاواں()) أرجع;

	صحيح this_ناپ = ناپ();
	منطقي other_modified = خطأ;
	لكل (صحيح i = 0; i < this_ناپ; i++) {
		خودکار it = قاعدے.لبھو((*هذا)[i]);
		إذا (it != قاعدے.سرا()) {
			other_modified = سچ;
			(*other)[i] = it->دوجا;
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::replace(ثابت std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قاعدے)
{
	replace(قاعدے, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قاعدے, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(ناپ() == other->ناپ());

	إذا (قاعدے.خالی_تھاواں()) أرجع;

	صحيح this_ناپ = ناپ();
	منطقي other_modified = خطأ;
	لكل (صحيح i = 0; i < this_ناپ; i++) {
		خودکار it = قاعدے.لبھو((*هذا)[i]);
		إذا (it != قاعدے.سرا()) {
			other_modified = سچ;
			(*other)[i] = it->دوجا;
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &نمونہ)
{
	remove2(نمونہ, عدم);
}

فراغ RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &نمونہ, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *هذا;
	tmp.remove2(نمونہ, other);
}

فراغ RTLIL::SigSpec::remove2(ثابت RTLIL::SigSpec &نمونہ, RTLIL::SigSpec *other)
{
	unpack();
	إذا (other != عدم) {
		log_assert(ناپ() == other->ناپ());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<Sig_ثنائی_ہندسہ> نمونہ_بٹ;
	نمونہ_بٹ.reserve(نمونہ.ناپ());
	لكل (خودکار &ثنائی_ہندسہ : نمونہ)
		إذا (ثنائی_ہندسہ.wire != عدم)
			نمونہ_بٹ.داخل_کرو(ثنائی_ہندسہ);

	// Compact in-place to avoid O(n^2) erase operations
	ناپ_t write_idx = 0;
	لكل (ناپ_t read_idx = 0; read_idx < بٹ_2.ناپ(); read_idx++)
	{
		إذا (!(بٹ_2[read_idx].wire != عدم && نمونہ_بٹ.گنتی(بٹ_2[read_idx]))) {
			إذا (write_idx != read_idx) {
				بٹ_2[write_idx] = بٹ_2[read_idx];
				إذا (other != عدم)
					other->بٹ_2[write_idx] = other->بٹ_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < بٹ_2.ناپ());
	إذا (modified) {
		بٹ_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->بٹ_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ)
{
	remove2(نمونہ, عدم);
}

فراغ RTLIL::SigSpec::remove(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *هذا;
	tmp.remove2(نمونہ, other);
}

فراغ RTLIL::SigSpec::remove2(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(ناپ() == other->ناپ());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	ناپ_t write_idx = 0;
	لكل (ناپ_t read_idx = 0; read_idx < بٹ_2.ناپ(); read_idx++) {
		إذا (!(بٹ_2[read_idx].wire != عدم && نمونہ.گنتی(بٹ_2[read_idx]))) {
			إذا (write_idx != read_idx) {
				بٹ_2[write_idx] = بٹ_2[read_idx];
				إذا (other != عدم)
					other->بٹ_2[write_idx] = other->بٹ_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < بٹ_2.ناپ());
	إذا (modified) {
		بٹ_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->بٹ_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove2(ثابت std::سیٹ<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(ناپ() == other->ناپ());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	ناپ_t write_idx = 0;
	لكل (ناپ_t read_idx = 0; read_idx < بٹ_2.ناپ(); read_idx++) {
		إذا (!(بٹ_2[read_idx].wire != عدم && نمونہ.گنتی(بٹ_2[read_idx]))) {
			إذا (write_idx != read_idx) {
				بٹ_2[write_idx] = بٹ_2[read_idx];
				إذا (other != عدم)
					other->بٹ_2[write_idx] = other->بٹ_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < بٹ_2.ناپ());
	إذا (modified) {
		بٹ_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->بٹ_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove2(ثابت pool<RTLIL::Wire*> &نمونہ, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(ناپ() == other->ناپ());
		other->unpack();
	}

	منطقي modified = خطأ;
	منطقي other_modified = خطأ;
	لكل (صحيح i = Get_ناپ(بٹ_2) - 1; i >= 0; i--) {
		إذا (بٹ_2[i].wire != عدم && نمونہ.گنتی(بٹ_2[i].wire)) {
			modified = سچ;
			بٹ_2.erase(بٹ_2.begin() + i);
			إذا (other != عدم) {
				other_modified = سچ;
				other->بٹ_2.erase(other->بٹ_2.begin() + i);
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

RTLIL::SigSpec RTLIL::SigSpec::کڈھو(ثابت RTLIL::SigSpec &نمونہ, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == عدم || ناپ() == other->ناپ());

	RTLIL::SigSpec ret;
	std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ_میل = منزل_sigbit_سمتیہ();

	لكل (خودکار& نمونہ_chunk : نمونہ.chunks()) {
		إذا (other) {
			std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ_other = other->منزل_sigbit_سمتیہ();
			صحيح i = 0;
			لكل (ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : بٹ_میل) {
				إذا (ثنائی_ہندسہ.wire &&
					ثنائی_ہندسہ.wire == نمونہ_chunk.wire &&
					ثنائی_ہندسہ.وتھ >= نمونہ_chunk.وتھ &&
					ثنائی_ہندسہ.وتھ < نمونہ_chunk.وتھ + نمونہ_chunk.چوڑائی)
					ret.append(بٹ_other[i]);
				++i;
			}
		} وإلا {
			لكل (ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : بٹ_میل)
				إذا (ثنائی_ہندسہ.wire &&
					ثنائی_ہندسہ.wire == نمونہ_chunk.wire &&
					ثنائی_ہندسہ.وتھ >= نمونہ_chunk.وتھ &&
					ثنائی_ہندسہ.وتھ < نمونہ_chunk.وتھ + نمونہ_chunk.چوڑائی)
					ret.append(ثنائی_ہندسہ);
		}
	}

	ret.check();
	أرجع ret;
}

RTLIL::SigSpec RTLIL::SigSpec::کڈھو(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == عدم || ناپ() == other->ناپ());

	std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ_میل = منزل_sigbit_سمتیہ();
	RTLIL::SigSpec ret;

	إذا (other) {
		std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ_other = other->منزل_sigbit_سمتیہ();
		صحيح i = 0;
		لكل (ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : بٹ_میل) {
			إذا (ثنائی_ہندسہ.wire && نمونہ.گنتی(ثنائی_ہندسہ))
				ret.append(بٹ_other[i]);
			++i;
		}
	} وإلا {
		لكل (ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : بٹ_میل)
			إذا (ثنائی_ہندسہ.wire && نمونہ.گنتی(ثنائی_ہندسہ))
				ret.append(ثنائی_ہندسہ);
	}

	ret.check();
	أرجع ret;
}

فراغ RTLIL::SigSpec::replace(صحيح وتھ, ثابت RTLIL::SigSpec &with)
{
	إذا (with.ناپ() == 0)
		أرجع;

	unpack();

	log_assert(وتھ >= 0);
	log_assert(with.ناپ() >= 0);
	log_assert(وتھ+with.ناپ() <= ناپ());

	صحيح i = 0;
	لكل (ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : with.بٹ()) {
		بٹ_2.at(وتھ + i) = ثنائی_ہندسہ;
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
			init_خالی_تھاواں_بٹ();
			hash_.clear();
		}
	}
	وإلا
	{
		std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> new_بٹ;
		new_بٹ.reserve(بٹ_2.ناپ());
		لكل (خودکار &ثنائی_ہندسہ : بٹ_2)
			إذا (ثنائی_ہندسہ.wire != عدم)
				new_بٹ.ڈالو_پٹھ(ثنائی_ہندسہ);
		إذا (Get_ناپ(new_بٹ) != Get_ناپ(بٹ_2)) {
			بٹ_2.وٹاندرا(new_بٹ);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

فراغ RTLIL::SigSpec::remove(صحيح وتھ, صحيح لمائی)
{
	إذا (لمائی == 0)
		أرجع;

	unpack();

	log_assert(وتھ >= 0);
	log_assert(لمائی >= 0);
	log_assert(وتھ + لمائی <= ناپ());

	بٹ_2.erase(بٹ_2.begin() + وتھ, بٹ_2.begin() + وتھ + لمائی);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::کڈھو(صحيح وتھ, صحيح لمائی) ثابت
{
	log_assert(وتھ >= 0);
	log_assert(لمائی >= 0);
	log_assert(وتھ + لمائی <= ناپ());

	std::سمتیہ<Sig_ثنائی_ہندسہ> extracted;
	Sig_ثنائی_ہندسہ پہلا;
	منطقي is_packing = سچ;
	لكل (صحيح i = وتھ; i < وتھ + لمائی; i++) {
		منطقي was_packing_before = is_packing;
		Sig_ثنائی_ہندسہ ثنائی_ہندسہ = (*هذا)[i];
		إذا (i == وتھ) {
			پہلا = ثنائی_ہندسہ;
			إذا (!ثنائی_ہندسہ.wire)
				is_packing = خطأ;
		} وإلا {
			إذا (ثنائی_ہندسہ.wire != پہلا.wire)
				is_packing = خطأ;
			إذا (ثنائی_ہندسہ.wire)
				إذا (ثنائی_ہندسہ.وتھ != پہلا.وتھ + (i - وتھ))
					is_packing = خطأ;
		}
		إذا (was_packing_before && !is_packing)
			لكل (صحيح j = وتھ; j < i; j++)
				extracted.ڈالو_پٹھ((*هذا)[j]);
		إذا (!is_packing)
			extracted.ڈالو_پٹھ((*هذا)[i]);
	}
	إذا (is_packing)
		أرجع SigChunk(پہلا.wire, پہلا.وتھ, لمائی);

	أرجع extracted;
}

فراغ RTLIL::SigSpec::rewrite_wires(std::function<فراغ(RTLIL::Wire*& wire)> rewrite)
{
	إذا (rep_ == CHUNK) {
		إذا (chunk_.wire != خالی_سوچک)
			rewrite(chunk_.wire);
		أرجع;
	}

	std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> new_بٹ;
	لكل (ثابت RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		إذا (c.wire != خالی_سوچک)
			rewrite(c.wire);
		لكل (صحيح i = 0; i < c.چوڑائی; i++)
			new_بٹ.emplace_پٹھ(c, i);
	}
	بٹ_2 = std::move(new_بٹ);
	hash_.clear();
}

فراغ RTLIL::SigSpec::append(ثابت RTLIL::SigSpec &إشارة)
{
	إذا (إشارة.ناپ() == 0)
		أرجع;

	إذا (ناپ() == 0) {
		*هذا = إشارة;
		أرجع;
	}

	hash_.clear();
	إذا (rep_ == CHUNK && إشارة.rep_ == CHUNK && chunk_.wire == إشارة.chunk_.wire) {
		إذا (chunk_.wire == عدم) {
			chunk_.انکڑے.داخل_کرو(chunk_.انکڑے.سرا(), إشارة.chunk_.انکڑے.begin(), إشارة.chunk_.انکڑے.سرا());
			chunk_.چوڑائی = Get_ناپ(chunk_.انکڑے);
			أرجع;
		}
		إذا (chunk_.وتھ + chunk_.چوڑائی == إشارة.chunk_.وتھ) {
			chunk_.چوڑائی += إشارة.chunk_.چوڑائی;
			أرجع;
		}
	}

	unpack();
	لكل (ثابت Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : إشارة.بٹ())
		بٹ_2.ڈالو_پٹھ(ثنائی_ہندسہ);
	check();
}

فراغ RTLIL::SigSpec::append(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ)
{
	hash_.clear();

	إذا (ناپ() == 0) {
		destroy();
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(ثنائی_ہندسہ);
		أرجع;
	}

	إذا (rep_ == CHUNK && chunk_.wire == ثنائی_ہندسہ.wire) {
		إذا (chunk_.wire == عدم) {
			chunk_.انکڑے.ڈالو_پٹھ(ثنائی_ہندسہ.انکڑے);
			chunk_.چوڑائی++;
			أرجع;
		}
		إذا (chunk_.وتھ + chunk_.چوڑائی == ثنائی_ہندسہ.وتھ) {
			chunk_.چوڑائی++;
			أرجع;
		}
	}

	unpack();

	بٹ_2.ڈالو_پٹھ(ثنائی_ہندسہ);
	check();
}

فراغ RTLIL::SigSpec::extend_u0(صحيح چوڑائی, منطقي is_signed)
{
	إذا (ناپ() > چوڑائی)
		remove(چوڑائی, ناپ() - چوڑائی);

	إذا (ناپ() < چوڑائی) {
		RTLIL::Sig_ثنائی_ہندسہ padding = ناپ() > 0 ? (*هذا)[ناپ() - 1] : RTLIL::State::Sx;
		إذا (!is_signed)
			padding = RTLIL::State::S0;
		طالما (ناپ() < چوڑائی)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(صحيح عدد) ثابت
{
	RTLIL::SigSpec sig;
	لكل (صحيح i = 0; i < عدد; i++)
		sig.append(*هذا);
	أرجع sig;
}

#جے_بنا_تعریف NDEBUG
فراغ RTLIL::SigSpec::check(ثابت Module *mod) ثابت
{
	إذا (rep_ == CHUNK)
	{
		log_assert(chunk_.چوڑائی != 0);
		إذا (chunk_.wire == عدم) {
			log_assert(chunk_.وتھ == 0);
			log_assert(chunk_.انکڑے.ناپ() == (ناپ_t)chunk_.چوڑائی);
		} وإلا {
			log_assert(chunk_.وتھ >= 0);
			log_assert(chunk_.چوڑائی >= 0);
			log_assert(chunk_.وتھ + chunk_.چوڑائی <= chunk_.wire->چوڑائی);
			log_assert(chunk_.انکڑے.ناپ() == 0);
			إذا (mod != خالی_سوچک)
				log_assert(chunk_.wire->module == mod);
		}
	}
	وإلا إذا (ناپ() <= 64)
	{
		إذا (mod != خالی_سوچک) {
			لكل (ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : بٹ_2)
				إذا (ثنائی_ہندسہ.wire != خالی_سوچک)
					log_assert(ثنائی_ہندسہ.wire->module == mod);
		}
	}
}
#جے_انت

منطقي RTLIL::SigSpec::عامل <(ثابت RTLIL::SigSpec &other) ثابت
{
	إذا (هذا == &other)
		أرجع خطأ;

	إذا (ناپ() != other.ناپ())
		أرجع ناپ() < other.ناپ();

	خودکار other_it = other.chunks().begin();
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

	إذا (ناپ() != other.ناپ())
		أرجع خطأ;

	خودکار other_it = other.chunks().begin();
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
	خودکار it = cs.begin();
	إذا (it == cs.سرا())
		أرجع خطأ;
	ثابت RTLIL::SigChunk &chunk = *it;
	أرجع chunk.wire && chunk.wire->چوڑائی == ناپ() && ++it == cs.سرا();
}

منطقي RTLIL::SigSpec::is_chunk() ثابت
{
	Chunks cs = chunks();
	خودکار it = cs.begin();
	إذا (it == cs.سرا())
		أرجع خطأ;
	أرجع ++it == cs.سرا();
}

منطقي RTLIL::SigSpec::معلوم_driver() ثابت
{
	لكل (خودکار &chunk : chunks())
		إذا (chunk.is_wire() && !chunk.wire->معلوم_driver())
			أرجع خطأ;
	أرجع سچ;
}

منطقي RTLIL::SigSpec::is_fully_const() ثابت
{
	لكل (خودکار &chunk : chunks())
		إذا (chunk.چوڑائی > 0 && chunk.wire != عدم)
			أرجع خطأ;
	أرجع سچ;
}

منطقي RTLIL::SigSpec::is_fully_zero() ثابت
{
	لكل (خودکار &chunk : chunks()) {
		إذا (chunk.چوڑائی > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.انکڑے)
			إذا (d != RTLIL::State::S0)
				أرجع خطأ;
	}
	أرجع سچ;
}

منطقي RTLIL::SigSpec::is_fully_ones() ثابت
{
	لكل (خودکار &chunk : chunks()) {
		إذا (chunk.چوڑائی > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.انکڑے)
			إذا (d != RTLIL::State::S1)
				أرجع خطأ;
	}
	أرجع سچ;
}

منطقي RTLIL::SigSpec::is_fully_def() ثابت
{
	لكل (خودکار &chunk : chunks()) {
		إذا (chunk.چوڑائی > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.انکڑے)
			إذا (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				أرجع خطأ;
	}
	أرجع سچ;
}

منطقي RTLIL::SigSpec::is_fully_undef() ثابت
{
	لكل (خودکار &chunk : chunks()) {
		إذا (chunk.چوڑائی > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.انکڑے)
			إذا (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				أرجع خطأ;
	}
	أرجع سچ;
}

منطقي RTLIL::SigSpec::has_const() ثابت
{
	لكل (خودکار &chunk : chunks())
		إذا (chunk.چوڑائی > 0 && chunk.wire == عدم)
			أرجع سچ;
	أرجع خطأ;
}

منطقي RTLIL::SigSpec::has_const(State state) ثابت
{
	لكل (خودکار &chunk : chunks())
		إذا (chunk.چوڑائی > 0 && chunk.wire == عدم && std::لبھو(chunk.انکڑے.begin(), chunk.انکڑے.سرا(), state) != chunk.انکڑے.سرا())
			أرجع سچ;
	أرجع خطأ;
}


منطقي RTLIL::SigSpec::has_marked_بٹ() ثابت
{
	لكل (خودکار &chunk : chunks())
		إذا (chunk.چوڑائی > 0 && chunk.wire == عدم) {
			لكل (RTLIL::State d : chunk.انکڑے)
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
	str.reserve(ناپ());
	std::سمتیہ<RTLIL::SigChunk> chunks = *هذا;
	لكل (ناپ_t i = chunks.ناپ(); i > 0; i--) {
		ثابت RTLIL::SigChunk &chunk = chunks[i-1];
		إذا (chunk.wire != عدم)
			str.append(chunk.چوڑائی, '?');
		وإلا
			str += RTLIL::Const(chunk.انکڑے).as_string();
	}
	أرجع str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ثابت
{
	Chunks cs = chunks();
	خودکار it = cs.begin();
	إذا (it == cs.سرا())
		أرجع RTLIL::Const();
	SigChunk chunk = *it;
	إذا (chunk.wire != عدم || ++it != cs.سرا())
		أرجع std::nullopt;
	أرجع RTLIL::Const(std::move(chunk.انکڑے));
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
	خودکار it = cs.begin();
	log_assert(it != cs.سرا());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.سرا() && chunk.wire && chunk.wire->چوڑائی == ناپ());
	أرجع chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ثابت
{
	Chunks cs = chunks();
	خودکار it = cs.begin();
	log_assert(it != cs.سرا());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.سرا());
	أرجع chunk;
}

RTLIL::Sig_ثنائی_ہندسہ RTLIL::SigSpec::as_ثنائی_ہندسہ() ثابت
{
	أرجع RTLIL::Sig_ثنائی_ہندسہ(*هذا);
}

منطقي RTLIL::SigSpec::میل(ثابت حرف* نمونہ) ثابت
{
	صحيح نمونہ_len = لڑی_لمبائی(نمونہ);
	log_assert(نمونہ_len == ناپ());

	لكل (صحيح i = 0; i < نمونہ_len; ++i) {
		حرف ch = نمونہ[i];
		إذا (ch == ' ')
			استمر;
		RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ = (*هذا)[نمونہ_len - 1 - i];
		إذا (ch == '*') {
			إذا (ثنائی_ہندسہ != State::Sz && ثنائی_ہندسہ != State::Sx)
				أرجع خطأ;
			استمر;
		}
		إذا (ch == '0') {
			إذا (ثنائی_ہندسہ != State::S0)
				أرجع خطأ;
		} وإلا
		إذا (ch == '1') {
			إذا (ثنائی_ہندسہ != State::S1)
				أرجع خطأ;
		} وإلا
			log_abort();
	}

	أرجع سچ;
}

std::سیٹ<RTLIL::Sig_ثنائی_ہندسہ> RTLIL::SigSpec::منزل_sigbit_سیٹ() ثابت
{
	std::سیٹ<RTLIL::Sig_ثنائی_ہندسہ> sigbits;
	لكل (خودکار &c : chunks())
		لكل (صحيح i = 0; i < c.چوڑائی; i++)
			sigbits.داخل_کرو(RTLIL::Sig_ثنائی_ہندسہ(c, i));
	أرجع sigbits;
}

pool<RTLIL::Sig_ثنائی_ہندسہ> RTLIL::SigSpec::منزل_sigbit_pool() ثابت
{
	pool<RTLIL::Sig_ثنائی_ہندسہ> sigbits;
	sigbits.reserve(ناپ());
	لكل (خودکار &c : chunks())
		لكل (صحيح i = 0; i < c.چوڑائی; i++)
			sigbits.داخل_کرو(RTLIL::Sig_ثنائی_ہندسہ(c, i));
	أرجع sigbits;
}

std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> RTLIL::SigSpec::منزل_sigbit_سمتیہ() ثابت
{
	std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> نتیجہ;
	نتیجہ.reserve(ناپ());
	لكل (Sig_ثنائی_ہندسہ ثنائی_ہندسہ : *هذا)
		نتیجہ.ڈالو_پٹھ(ثنائی_ہندسہ);
	أرجع نتیجہ;
}

std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> RTLIL::SigSpec::منزل_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت
{
	صحيح this_ناپ = ناپ();
	log_assert(this_ناپ == other.ناپ());

	std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> new_map;
	لكل (صحيح i = 0; i < this_ناپ; i++)
		new_map[(*هذا)[i]] = other[i];

	أرجع new_map;
}

dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> RTLIL::SigSpec::منزل_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت
{
	صحيح this_ناپ = ناپ();
	log_assert(this_ناپ == other.ناپ());

	dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> new_map;
	new_map.reserve(this_ناپ);
	لكل (صحيح i = 0; i < this_ناپ; i++)
		new_map[(*هذا)[i]] = other[i];

	أرجع new_map;
}

ساكن فراغ sigspec_parse_ونڈو(std::سمتیہ<std::string> &ٹوکن, ثابت std::string &متن, حرف sep)
{
	ناپ_t شروع = 0, سرا = 0;
	طالما ((سرا = متن.لبھو(sep, شروع)) != std::string::npos) {
		ٹوکن.ڈالو_پٹھ(متن.substr(شروع, سرا - شروع));
		شروع = سرا + 1;
	}
	ٹوکن.ڈالو_پٹھ(متن.substr(شروع));
}

ساكن RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ثابت std::string &netname)
{
	أرجع module->wire(netname);
}

منطقي RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::سمتیہ<std::string> ٹوکن;
	sigspec_parse_ونڈو(ٹوکن, str, ',');

	sig = RTLIL::SigSpec();
	لكل (صحيح tokidx = صحيح(ٹوکن.ناپ())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = ٹوکن[tokidx];
		std::string indices;

		إذا (netname.ناپ() == 0)
			استمر;

		إذا (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			خودکار ast = p.const2ast(netname);
			إذا (ast == خالی_سوچک)
				أرجع خطأ;
			sig.append(RTLIL::Const(ast->بٹ));
			استمر;
		}

		إذا (module == عدم)
			أرجع خطأ;

		إذا (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		إذا (sigspec_parse_wire(module, netname) == خالی_سوچک) {
			ناپ_t indices_pos = netname.ناپ()-1;
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
		إذا (wire == خالی_سوچک)
			أرجع خطأ;

		إذا (!indices.خالی_تھاواں()) {
			std::سمتیہ<std::string> اشاریہ_ٹوکن;
			sigspec_parse_ونڈو(اشاریہ_ٹوکن, indices.substr(1, indices.ناپ()-2), ':');
			إذا (اشاریہ_ٹوکن.ناپ() == 1) {
				صحيح a = atoi(اشاریہ_ٹوکن.at(0).c_str());
				إذا (a < 0 || a >= wire->چوڑائی)
					أرجع خطأ;
				sig.append(RTLIL::SigSpec(wire, a));
			} وإلا {
				صحيح a = atoi(اشاریہ_ٹوکن.at(0).c_str());
				صحيح b = atoi(اشاریہ_ٹوکن.at(1).c_str());
				إذا (a > b) {
					صحيح tmp = a;
					a = b, b = tmp;
				}
				إذا (a < 0 || a >= wire->چوڑائی)
					أرجع خطأ;
				إذا (b < 0 || b >= wire->چوڑائی)
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
	إذا (str.خالی_تھاواں() || str[0] != '@')
		أرجع parse(sig, module, str);

	IdString sel_name = design->twines().لبھو(RTLIL::escape_id(str.substr(1)));
	إذا (sel_name == IdString::Null || design->selection_vars.گنتی(sel_name) == 0)
		أرجع خطأ;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	لكل (خودکار &it : module->wires_)
		إذا (sel.selected_member(module->name, it.دوجا->name))
			sig.append(it.دوجا);

	أرجع سچ;
}

منطقي RTLIL::SigSpec::parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	إذا (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.ناپ());
		أرجع سچ;
	}

	إذا (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.ناپ());
		أرجع سچ;
	}

	إذا (lhs.is_chunk()) {
		حرف *p = (حرف*)str.c_str(), *endptr;
		لما صحيح val = strtol(p, &endptr, 10);
		إذا (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.ناپ());
			أرجع سچ;
		}
	}

	إذا (!parse(sig, module, str))
		أرجع خطأ;
	إذا (sig.ناپ() > lhs.ناپ())
		sig.remove(lhs.ناپ(), sig.ناپ() - lhs.ناپ());
	أرجع سچ;
}

RTLIL::SigSpec::عامل std::سمتیہ<RTLIL::SigChunk>() ثابت
{
	std::سمتیہ<RTLIL::SigChunk> نتیجہ;
	لكل (ثابت RTLIL::SigChunk &c : chunks())
		نتیجہ.ڈالو_پٹھ(c);
	أرجع نتیجہ;
}

RTLIL::Case_قاعدہ::~Case_قاعدہ()
{
	لكل (خودکار it = switches.begin(); it != switches.سرا(); it++)
		مٹاؤ *it;
}

منطقي RTLIL::Case_قاعدہ::خالی_تھاواں() ثابت
{
	أرجع actions.خالی_تھاواں() && switches.خالی_تھاواں();
}

RTLIL::Case_قاعدہ *RTLIL::Case_قاعدہ::clone() ثابت
{
	RTLIL::Case_قاعدہ *new_caserule = جديد RTLIL::Case_قاعدہ;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->گن = گن;
	لكل (خودکار &it : switches)
		new_caserule->switches.ڈالو_پٹھ(it->clone());
	أرجع new_caserule;
}

RTLIL::Switch_قاعدہ::~Switch_قاعدہ()
{
	لكل (خودکار it = cases.begin(); it != cases.سرا(); it++)
		مٹاؤ *it;
}

منطقي RTLIL::Switch_قاعدہ::خالی_تھاواں() ثابت
{
	أرجع cases.خالی_تھاواں();
}

RTLIL::Switch_قاعدہ *RTLIL::Switch_قاعدہ::clone() ثابت
{
	RTLIL::Switch_قاعدہ *new_switchrule = جديد RTLIL::Switch_قاعدہ;
	new_switchrule->إشارة = إشارة;
	new_switchrule->گن = گن;
	لكل (خودکار &it : cases)
		new_switchrule->cases.ڈالو_پٹھ(it->clone());
	أرجع new_switchrule;

}

RTLIL::Sync_قاعدہ *RTLIL::Sync_قاعدہ::clone() ثابت
{
	RTLIL::Sync_قاعدہ *new_syncrule = جديد RTLIL::Sync_قاعدہ;
	new_syncrule->type = type;
	new_syncrule->إشارة = إشارة;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	أرجع new_syncrule;
}

RTLIL::Process::~Process()
{
	لكل (خودکار it = syncs.begin(); it != syncs.سرا(); it++)
		مٹاؤ *it;
}

RTLIL::Process *RTLIL::Process::clone() ثابت
{
	RTLIL::Process *new_proc = جديد RTLIL::Process;

	new_proc->گن = گن;

	RTLIL::Case_قاعدہ *rc_ptr = جڑ_case.clone();
	new_proc->جڑ_case = *rc_ptr;
	rc_ptr->switches.clear();
	مٹاؤ rc_ptr;

	لكل (خودکار &it : syncs)
		new_proc->syncs.ڈالو_پٹھ(it->clone());

	أرجع new_proc;
}

RTLIL::حافظہ::~حافظہ()
{
#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::حافظہ::get_all_memorys()->erase(hashidx_);
#جے_انت
}

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
ساكن std::map<بنانشانی صحيح, RTLIL::حافظہ*> all_memorys;
std::map<بنانشانی صحيح, RTLIL::حافظہ*> *RTLIL::حافظہ::get_all_memorys(فراغ)
{
	أرجع &all_memorys;
}
#جے_انت
YOSYS_NAMESPACE_سرا
