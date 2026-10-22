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

#شامل "kernel/rtlil.h"
#شامل "kernel/yosys.h"
#شامل "kernel/macc.h"
#شامل "kernel/newcelltypes.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/threading.h"
#شامل "frontends/verilog/verilog_frontend.h"
#شامل "frontends/verilog/preproc.h"
#شامل "backends/rtlil/rtlil_backend.h"

#شامل <string.h>
#شامل <algorithm>
#شامل <charconv>
#شامل <optional>
#شامل <string_view>
#شامل <sstream>

YOSYS_NAMESPACE_BEGIN

dict<std::string, std::string> RTLIL::constpad;

ثابت pool<IdString> &RTLIL::builtin_ff_cell_types() {
	ساکن ثابت pool<IdString> res = []() {
		pool<IdString> r;
		برائے (سائز_t i = 0; i < StaticCellTypes::builder.گنتی; i++) {
			خودکار &cell = StaticCellTypes::builder.cells[i];
			اگر (cell.features.is_ff)
				r.داخل_کرو(cell.type);
		}
		واپس r;
	}();
	واپس res;
}

#تعریف_کرو check(condition) log_assert(condition && "malformed Const union")

ثابت Const::bitvectype& Const::get_بٹ() ثابت {
	check(is_بٹ());
	واپس *get_if_بٹ();
}

ثابت std::string& Const::get_str() ثابت {
	check(is_str());
	واپس *get_if_str();
}

Const::bitvectype& Const::get_بٹ() {
	check(is_بٹ());
	واپس *get_if_بٹ();
}

std::string& Const::get_str() {
	check(is_str());
	واپس *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	نیا ((خالی*)&str_) std::string(std::move(str));
	لیبل = backing_لیبل::string;
}

RTLIL::Const::Const(طویل طویل val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	حرف بائٹیں[] = {
		(حرف)(val >> 24), (حرف)(val >> 16), (حرف)(val >> 8), (حرف)val
	};
	نیا ((خالی*)&str_) std::string(بائٹیں, 4);
	لیبل = backing_لیبل::string;
}

RTLIL::Const::Const(طویل طویل val, صحیح چوڑائی)
{
	log_assert(چوڑائی >= 0 && چوڑائی < RTLIL::چوڑائی_حد_بندی);
	flags = RTLIL::CONST_FLAG_NONE;
	اگر ((چوڑائی & 7) == 0) {
		نیا ((خالی*)&str_) std::string();
		لیبل = backing_لیبل::string;
		std::string& str = get_str();
		صحیح بائٹیں = چوڑائی >> 3;
		علامتی حرف sign_byte = val < 0 ? -1 : 0;
		str.resize(بائٹیں, sign_byte);
		بائٹیں = std::min<صحیح>(بائٹیں, حجم(val));
		برائے (صحیح i = 0; i < بائٹیں; i++) {
			str[str.سائز() - 1 - i] = val;
			val = val >> 8;
		}
		واپس;
	}

	نیا ((خالی*)&بٹ_2) bitvectype();
	لیبل = backing_لیبل::بٹ;
	bitvectype& bv = get_بٹ();
	bv.reserve(چوڑائی);
	برائے (صحیح i = 0; i < چوڑائی; i++) {
		bv.ڈالو_عقب((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State ثنائی_ہندسہ, صحیح چوڑائی)
{
	log_assert(چوڑائی >= 0 && چوڑائی < RTLIL::چوڑائی_حد_بندی);
	flags = RTLIL::CONST_FLAG_NONE;
	نیا ((خالی*)&بٹ_2) bitvectype();
	لیبل = backing_لیبل::بٹ;
	bitvectype& bv = get_بٹ();
	bv.reserve(چوڑائی);
	برائے (صحیح i = 0; i < چوڑائی; i++)
		bv.ڈالو_عقب(ثنائی_ہندسہ);
}

RTLIL::Const::Const(ثابت std::سمتیہ<منطقی> &بٹ)
{
	flags = RTLIL::CONST_FLAG_NONE;
	نیا ((خالی*)&بٹ_2) bitvectype();
	لیبل = backing_لیبل::بٹ;
	bitvectype& bv = get_بٹ();
	bv.reserve(بٹ.سائز());
	برائے (ثابت خودکار &b : بٹ)
		bv.emplace_عقب(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ثابت RTLIL::Const &other) {
	لیبل = other.لیبل;
	flags = other.flags;
	اگر (is_str())
		نیا ((خالی*)&str_) std::string(other.get_str());
	ورنہ اگر (is_بٹ())
		نیا ((خالی*)&بٹ_2) bitvectype(other.get_بٹ());
	ورنہ
		check(جھوٹ);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	لیبل = other.لیبل;
	flags = other.flags;
	اگر (is_str())
		نیا ((خالی*)&str_) std::string(std::move(other.get_str()));
	ورنہ اگر (is_بٹ())
		نیا ((خالی*)&بٹ_2) bitvectype(std::move(other.get_بٹ()));
	ورنہ
		check(جھوٹ);
}

RTLIL::Const &RTLIL::Const::عامل =(ثابت RTLIL::Const &other) {
	flags = other.flags;
	اگر (other.is_str()) {
		اگر (!is_str()) {
			// sketchy zone
			check(is_بٹ());
			بٹ_2.~bitvectype();
			(خالی)نیا ((خالی*)&str_) std::string();
		}
		لیبل = other.لیبل;
		get_str() = other.get_str();
	} ورنہ اگر (other.is_بٹ()) {
		اگر (!is_بٹ()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(خالی)نیا ((خالی*)&بٹ_2) bitvectype();
		}
		لیبل = other.لیبل;
		get_بٹ() = other.get_بٹ();
	} ورنہ {
		check(جھوٹ);
	}
	واپس *یہ;
}

RTLIL::Const::~Const() {
	اگر (is_بٹ())
		بٹ_2.~bitvectype();
	ورنہ اگر (is_str())
		str_.~string();
	ورنہ
		check(جھوٹ);
}

منطقی RTLIL::Const::عامل<(ثابت RTLIL::Const &other) ثابت
{
	اگر (سائز() != other.سائز())
		واپس سائز() < other.سائز();

	برائے (صحیح i = 0; i < سائز(); i++)
		اگر ((*یہ)[i] != other[i])
			واپس (*یہ)[i] < other[i];

	واپس جھوٹ;
}

منطقی RTLIL::Const::عامل ==(ثابت RTLIL::Const &other) ثابت
{
	اگر (is_str() && other.is_str())
		واپس get_str() == other.get_str();
	اگر (is_بٹ() && other.is_بٹ())
		واپس get_بٹ() == other.get_بٹ();

	اگر (سائز() != other.سائز())
		واپس جھوٹ;

	برائے (صحیح i = 0; i < سائز(); i++)
	اگر ((*یہ)[i] != other[i])
		واپس جھوٹ;

	واپس سچ;
}

منطقی RTLIL::Const::عامل !=(ثابت RTLIL::Const &other) ثابت
{
	واپس !(*یہ == other);
}

std::سمتیہ<RTLIL::State>& RTLIL::Const::بٹ_اندرونی()
{
	bitvectorize_اندرونی();
	واپس get_بٹ();
}

std::سمتیہ<RTLIL::State> RTLIL::Const::مقصد_بٹ() ثابت
{
	std::سمتیہ<State> v;
	برائے (خودکار ثنائی_ہندسہ : *یہ)
		v.ڈالو_عقب(ثنائی_ہندسہ);
	واپس v;
}

منطقی RTLIL::Const::as_bool() ثابت
{
	اگر (is_str()) {
		برائے (حرف ch : get_str())
			اگر (ch != 0)
				واپس سچ;
		واپس جھوٹ;
	}

	ثابت bitvectype& bv = get_بٹ();
	برائے (سائز_t i = 0; i < bv.سائز(); i++)
		اگر (bv[i] == State::S1)
			واپس سچ;
	واپس جھوٹ;
}

صحیح RTLIL::Const::as_int(منطقی is_signed) ثابت
{
	int32_t ret = 0;
	اگر (ثابت std::string *s = get_if_str()) {
		صحیح سائز = Get_سائز(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		برائے (صحیح i = std::min(4, سائز); i > 0; i--)
			ret |= static_cast<غیرعلامتی حرف>((*s)[سائز - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		اگر (is_signed && سائز > 0 && سائز < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << سائز*8;
		واپس ret;
	}

	ثابت bitvectype& bv = get_بٹ();
	صحیح significant_بٹ = std::min(Get_سائز(bv), 32);
	برائے (صحیح i = 0; i < significant_بٹ; i++)
		اگر (bv[i] == State::S1)
			ret |= 1 << i;
	اگر (is_signed && significant_بٹ > 0 && significant_بٹ < 32 && bv.عقب() == State::S1 )
		ret |= UINT32_MAX << significant_بٹ;
	واپس ret;
}

منطقی RTLIL::Const::convertible_مقصد_int(منطقی is_signed) ثابت
{
	خودکار سائز = get_min_سائز(is_signed);

	اگر (سائز < 0)
		واپس جھوٹ;

	// If it fits in 31 bits it is definitely convertible
	اگر (سائز <= 31)
		واپس سچ;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	اگر (سائز == 32) {
		اگر (is_signed)
			واپس سچ;
		واپس (*یہ)[سائز - 1] != State::S1;
	}

	واپس جھوٹ;
}

std::optional<صحیح> RTLIL::Const::try_as_int(منطقی is_signed) ثابت
{
	اگر (!convertible_مقصد_int(is_signed))
		واپس std::nullopt;
	واپس as_int(is_signed);
}

صحیح RTLIL::Const::as_int_saturating(منطقی is_signed) ثابت
{
	اگر (!convertible_مقصد_int(is_signed)) {
		اگر (!is_signed)
			واپس std::numeric_limits<صحیح>::max();

		ثابت خودکار min_سائز = get_min_سائز(is_signed);
		log_assert(min_سائز > 0);
		ثابت خودکار neg = (*یہ)[min_سائز - 1];
		واپس neg ? std::numeric_limits<صحیح>::min() : std::numeric_limits<صحیح>::max();
	}
	واپس as_int(is_signed);
}

خالی RTLIL::Const::لیبل_bare_integer_const(ثابت std::string &قدر)
{
	اگر (قدر.خالی_خانے() || قدر.ڈھونڈو('\'') != std::string::npos)
		واپس;
	سائز_t آغاز = (قدر[0] == '-' || قدر[0] == '+') ? 1 : 0;
	اگر (آغاز < قدر.سائز() && std::all_کا(قدر.begin() + آغاز, قدر.انتہا(), ::ہندسہ_ہے))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

صحیح RTLIL::Const::get_min_سائز(منطقی is_signed) ثابت
{
	اگر (خالی_خانے()) واپس 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_ثنائی_ہندسہ;
	اگر (is_signed)
		leading_ثنائی_ہندسہ = (عقب() == RTLIL::State::Sx) ? RTLIL::State::S0 : عقب();
	ورنہ
		leading_ثنائی_ہندسہ = RTLIL::State::S0;

	خودکار idx = سائز();
	جب (idx > 0 && (*یہ)[idx -1] == leading_ثنائی_ہندسہ) {
		idx--;
	}

	// signed needs one leading bit
	اگر (is_signed && idx < سائز()) {
		idx++;
	}
	// must be at least one bit
	واپس (idx == 0) ? 1 : idx;
}

خالی RTLIL::Const::compress(منطقی is_signed)
{
	خودکار idx = get_min_سائز(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<صحیح> RTLIL::Const::as_int_compress(منطقی is_signed) ثابت
{
	واپس try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ثابت حرف* any) ثابت
{
	صحیح sz = سائز();
	std::string ret;
	ret.reserve(sz);
	برائے (صحیح i = sz - 1; i >= 0; --i)
		انتخاب ((*یہ)[i]) {
			حالت S0: ret.ڈالو_عقب('0'); توڑو;
			حالت S1: ret.ڈالو_عقب('1'); توڑو;
			حالت Sx: ret.ڈالو_عقب('x'); توڑو;
			حالت Sz: ret.ڈالو_عقب('z'); توڑو;
			حالت Sa: ret += any; توڑو;
			حالت Sm: ret.ڈالو_عقب('m'); توڑو;
		}
	واپس ret;
}

RTLIL::Const RTLIL::Const::from_string(ثابت std::string &str)
{
	Const c;
	bitvectype& bv = c.get_بٹ();
	bv.reserve(str.سائز());
	برائے (خودکار it = str.rbegin(); it != str.rend(); it++)
		انتخاب (*it) {
			حالت '0': bv.ڈالو_عقب(State::S0); توڑو;
			حالت '1': bv.ڈالو_عقب(State::S1); توڑو;
			حالت 'x': bv.ڈالو_عقب(State::Sx); توڑو;
			حالت 'z': bv.ڈالو_عقب(State::Sz); توڑو;
			حالت 'm': bv.ڈالو_عقب(State::Sm); توڑو;
			طےشدہ: bv.ڈالو_عقب(State::Sa);
		}
	واپس c;
}

std::string RTLIL::Const::decode_string() ثابت
{
	اگر (خودکار str = get_if_str())
		واپس *str;

	ثابت bitvectype& bv = get_بٹ();
	ثابت صحیح n = Get_سائز(bv);
	ثابت صحیح n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	صحیح i = n_over_8 * 8;
	اگر (i < n) {
		حرف ch = 0;
		برائے (صحیح j = 0; j < (n - i); j++) {
			اگر (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		اگر (ch != 0)
			s.append({ch});
	}
	i -= 8;
	برائے (; i >= 0; i -= 8) {
		حرف ch = 0;
		برائے (صحیح j = 0; j < 8; j++) {
			اگر (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		اگر (ch != 0)
			s.append({ch});
	}
	واپس s;
}

صحیح RTLIL::Const::سائز() ثابت {
	اگر (is_str())
		واپس 8 * str_.سائز();
	ورنہ {
		check(is_بٹ());
		واپس بٹ_2.سائز();
	}
}

منطقی RTLIL::Const::خالی_خانے() ثابت {
	اگر (is_str())
		واپس str_.خالی_خانے();
	ورنہ {
		check(is_بٹ());
		واپس بٹ_2.خالی_خانے();
	}
}

خالی RTLIL::Const::bitvectorize_اندرونی() {
	اگر (لیبل == backing_لیبل::بٹ)
		واپس;

	check(is_str());

	bitvectype new_بٹ;

	new_بٹ.reserve(str_.سائز() * 8);
	برائے (صحیح i = str_.سائز() - 1; i >= 0; i--) {
		غیرعلامتی حرف ch = str_[i];
		برائے (صحیح j = 0; j < 8; j++) {
			new_بٹ.ڈالو_عقب((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(خالی)نیا ((خالی*)&بٹ_2) bitvectype(std::move(new_بٹ));
		لیبل = backing_لیبل::بٹ;
	}
}

خالی RTLIL::Const::append(ثابت RTLIL::Const &other) {
	bitvectorize_اندرونی();
	bitvectype& bv = get_بٹ();
	bv.داخل_کرو(bv.انتہا(), other.begin(), other.انتہا());
}

RTLIL::State RTLIL::Const::const_iterator::عامل*() ثابت {
	اگر (خودکار bv = والد->get_if_بٹ())
		واپس (*bv)[idx];

	صحیح char_idx = والد->get_str().سائز() - idx / 8 - 1;
	منطقی ثنائی_ہندسہ = (والد->get_str()[char_idx] & (1 << (idx % 8)));
	واپس ثنائی_ہندسہ ? State::S1 : State::S0;
}

منطقی RTLIL::Const::is_fully_zero() ثابت
{
	اگر (خودکار str = get_if_str()) {
		برائے (حرف ch : *str)
			اگر (ch != 0)
				واپس جھوٹ;
		واپس سچ;
	}

	ثابت bitvectype& bv = get_بٹ();

	برائے (ثابت خودکار &ثنائی_ہندسہ : bv)
		اگر (ثنائی_ہندسہ != RTLIL::State::S0)
			واپس جھوٹ;

	واپس سچ;
}

منطقی RTLIL::Const::is_fully_ones() ثابت
{
	اگر (خودکار str = get_if_str()) {
		برائے (حرف ch : *str)
			اگر (ch != (حرف)0xff)
				واپس جھوٹ;
		واپس سچ;
	}

	ثابت bitvectype& bv = get_بٹ();
	برائے (ثابت خودکار &ثنائی_ہندسہ : bv)
		اگر (ثنائی_ہندسہ != RTLIL::State::S1)
			واپس جھوٹ;

	واپس سچ;
}

منطقی RTLIL::Const::is_fully_def() ثابت
{
	اگر (is_str())
		واپس سچ;

	ثابت bitvectype& bv = get_بٹ();
	برائے (ثابت خودکار &ثنائی_ہندسہ : bv)
		اگر (ثنائی_ہندسہ != RTLIL::State::S0 && ثنائی_ہندسہ != RTLIL::State::S1)
			واپس جھوٹ;

	واپس سچ;
}

منطقی RTLIL::Const::is_fully_undef() ثابت
{
	اگر (خودکار str = get_if_str())
		واپس str->خالی_خانے();

	ثابت bitvectype& bv = get_بٹ();
	برائے (ثابت خودکار &ثنائی_ہندسہ : bv)
		اگر (ثنائی_ہندسہ != RTLIL::State::Sx && ثنائی_ہندسہ != RTLIL::State::Sz)
			واپس جھوٹ;

	واپس سچ;
}

منطقی RTLIL::Const::is_fully_undef_x_only() ثابت
{
	اگر (خودکار str = get_if_str())
		واپس str->خالی_خانے();

	ثابت bitvectype& bv = get_بٹ();
	برائے (ثابت خودکار &ثنائی_ہندسہ : bv)
		اگر (ثنائی_ہندسہ != RTLIL::State::Sx)
			واپس جھوٹ;

	واپس سچ;
}

منطقی RTLIL::Const::is_onehot(صحیح *pos) ثابت
{
	منطقی ملا = جھوٹ;
	صحیح سائز = Get_سائز(*یہ);
	برائے (صحیح i = 0; i < سائز; i++) {
		State ثنائی_ہندسہ = (*یہ)[i];
		اگر (ثنائی_ہندسہ != RTLIL::State::S0 && ثنائی_ہندسہ != RTLIL::State::S1)
			واپس جھوٹ;
		اگر (ثنائی_ہندسہ == RTLIL::State::S1) {
			اگر (ملا)
				واپس جھوٹ;
			اگر (pos)
				*pos = i;
			ملا = سچ;
		}
	}
	واپس ملا;
}

Hasher RTLIL::Const::hash_into(Hasher h) ثابت
{
	اگر (خودکار str = get_if_str())
		واپس hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ثابت bitvectype &bv = get_بٹ();
	صحیح سائز = Get_سائز(bv);
	std::string packed;
	صحیح packed_سائز = (سائز + 7) >> 3;
	packed.resize(packed_سائز, 0);
	برائے (صحیح bi = 0; bi < packed_سائز; ++bi) {
		حرف ch = 0;
		صحیح انتہا = std::min((bi + 1)*8, سائز);
		برائے (صحیح i = bi*8; i < انتہا; ++i) {
			RTLIL::State b = bv[i];
			اگر (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<حرف>::hash_into(ch, h);
				برائے (; i < سائز; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(سائز);
				واپس h;
			}
			ch |= static_cast<صحیح>(b) << (i & 7);
		}
		packed[packed_سائز - 1 - bi] = ch;
	}
	واپس hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::نکالو_2(صحیح انحراف, صحیح len, RTLIL::State padding) ثابت {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	برائے (صحیح i = انحراف; i < انحراف + len; i++)
		ret_bv.ڈالو_عقب(i < Get_سائز(*یہ) ? (*یہ)[i] : padding);
	واپس RTLIL::Const(ret_bv);
}
#تعریف_منسوخ check /* check(condition) for Const */

منطقی RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ثابت
{
	واپس اوصاف.گنتی(id);
}

خالی RTLIL::AttrObject::سیٹ_bool_attribute(RTLIL::IdString id, منطقی قدر)
{
	اگر (قدر)
		اوصاف[id] = RTLIL::Const(1);
	ورنہ
		اوصاف.erase(id);
}

منطقی RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ثابت
{
	ثابت خودکار it = اوصاف.ڈھونڈو(id);
	اگر (it == اوصاف.انتہا())
		واپس جھوٹ;
	واپس it->دوسرا.as_bool();
}

خالی RTLIL::AttrObject::سیٹ_string_attribute(RTLIL::IdString id, string قدر)
{
	اگر (قدر.خالی_خانے())
		اوصاف.erase(id);
	ورنہ
		اوصاف[id] = قدر;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ثابت
{
	std::string قدر;
	ثابت خودکار it = اوصاف.ڈھونڈو(id);
	اگر (it != اوصاف.انتہا())
		قدر = it->دوسرا.decode_string();
	واپس قدر;
}

خالی RTLIL::AttrObject::سیٹ_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &ڈیٹا)
{
	string attrval;
	برائے (ثابت خودکار &s : ڈیٹا) {
		اگر (!attrval.خالی_خانے())
			attrval += "|";
		attrval += s;
	}
	سیٹ_string_attribute(id, attrval);
}

خالی RTLIL::AttrObject::جوڑو_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &ڈیٹا)
{
	pool<string> union_ڈیٹا = get_strpool_attribute(id);
	union_ڈیٹا.داخل_کرو(ڈیٹا.begin(), ڈیٹا.انتہا());
	اگر (!union_ڈیٹا.خالی_خانے())
		سیٹ_strpool_attribute(id, union_ڈیٹا);
}

خالی RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->اوصاف = std::move(buf);
}

نامخلا {
	سانچہ<قسم_نام F>
	خالی walk_process_attr_objects(RTLIL::Process *process, F دورہ) {
		دورہ(process);
		std::سمتیہ<RTLIL::Case_قاعدہ*> case_ڈھیر{&process->جڑ_case};
		جب (!case_ڈھیر.خالی_خانے()) {
			RTLIL::Case_قاعدہ *cs = case_ڈھیر.عقب();
			case_ڈھیر.نکالو_عقب();
			دورہ(cs);
			برائے (خودکار *sw : cs->switches) {
				دورہ(sw);
				برائے (خودکار *case_ : sw->cases)
					case_ڈھیر.ڈالو_عقب(case_);
			}
		}
		برائے (خودکار *ہمآہنگ : process->syncs)
			برائے (خودکار &mwa : ہمآہنگ->mem_write_actions)
				دورہ(&mwa);
	}

	سانچہ<قسم_نام F>
	خالی walk_attr_objects(RTLIL::Design *design, F دورہ) {
		برائے (خودکار &[_, module] : design->modules_) {
			دورہ(module);
			برائے (خودکار &[_, wire] : module->wires_)
				دورہ(wire);
			برائے (خودکار &[_, mem] : module->memories)
				دورہ(mem);
			برائے (خودکار &[_, cell] : module->cells_)
				دورہ(cell);
			برائے (خودکار &[_, process] : module->processes)
				walk_process_attr_objects(process, دورہ);
		}
	}
}


سائز_t RTLIL::Design::gc_twines()
{
	int64_t آغاز = PerformanceTimer::query();
	pool<IdString> زندہ;
	خودکار جڑ = [&](IdString ref) {
		اگر (ref != IdString::Null)
			زندہ.داخل_کرو(ref);
	};

	walk_attr_objects(یہ, [&](ثابت RTLIL::AttrObject *obj) {
		برائے (خودکار &attr : obj->اوصاف)
			جڑ(attr.پہلا);
	});

	جڑ(selected_فعال_module);

	برائے (خودکار &[name, module] : modules_) {
		جڑ(name);
		برائے (IdString پورٹ : module->ports)
			جڑ(پورٹ);
		برائے (IdString param : module->avail_parameters)
			جڑ(param);
		برائے (خودکار &[param, _] : module->parameter_default_اقدار)
			جڑ(param);
		برائے (خودکار &[name, _] : module->memories)
			جڑ(name);
		برائے (خودکار &[name, _] : module->processes)
			جڑ(name);
		برائے (خودکار &[name, wire] : module->wires_) {
			جڑ(name);
			اگر (wire->معلوم_driver())
				جڑ(wire->driver_پورٹ());
		}
		برائے (خودکار &[name, cell] : module->cells_) {
			جڑ(name);
			جڑ(cell->type);
			برائے (خودکار &conn : cell->connections())
				جڑ(conn.پہلا);
			برائے (خودکار &param : cell->parameters)
				جڑ(param.پہلا);
		}
		برائے (خودکار &[_, process] : module->processes) {
			std::سمتیہ<RTLIL::Case_قاعدہ*> case_ڈھیر{&process->جڑ_case};
			جب (!case_ڈھیر.خالی_خانے()) {
				RTLIL::Case_قاعدہ *cs = case_ڈھیر.عقب();
				case_ڈھیر.نکالو_عقب();
				برائے (خودکار *sw : cs->switches) {
					برائے (خودکار *case_ : sw->cases)
						case_ڈھیر.ڈالو_عقب(case_);
				}
			}
			برائے (خودکار *ہمآہنگ : process->syncs) {
				برائے (خودکار &mwa : ہمآہنگ->mem_write_actions)
					جڑ(mwa.memid);
			}
		}
	}

	خودکار جڑ_selection = [&](ثابت RTLIL::Selection &sel) {
		برائے (IdString m : sel.selected_modules)
			جڑ(m);
		برائے (خودکار &[m, members] : sel.selected_members) {
			جڑ(m);
			برائے (IdString member : members)
				جڑ(member);
		}
	};

	برائے (خودکار &[name, sel] : selection_vars) {
		جڑ(name);
		جڑ_selection(sel);
	}

	برائے (خودکار &sel : selection_ڈھیر)
		جڑ_selection(sel);

	سائز_t erased = twines_.gc(زندہ);

	int64_t وقت_ns = PerformanceTimer::query() - آغاز;
	Pass::تفریق_from_current_runtime_ns(وقت_ns);
	twine_gc_ns += وقت_ns;
	++twine_gc_گنتی;
	واپس erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ثابت
{
	pool<string> ڈیٹا;
	اگر (اوصاف.گنتی(id) != 0)
		برائے (خودکار s : بانٹو_علامات(get_string_attribute(id), "|"))
			ڈیٹا.داخل_کرو(s);
	واپس ڈیٹا;
}

خالی RTLIL::AttrObject::سیٹ_hdlname_attribute(ثابت سمتیہ<string> &hierarchy)
{
	string attrval;
	برائے (ثابت خودکار &ident : hierarchy) {
		اگر (!attrval.خالی_خانے())
			attrval += " ";
		attrval += ident;
	}
	سیٹ_string_attribute(ID::hdlname, attrval);
}

سمتیہ<string> RTLIL::AttrObject::get_hdlname_attribute() ثابت
{
	واپس بانٹو_علامات(get_string_attribute(ID::hdlname), " ");
}

خالی RTLIL::AttrObject::سیٹ_intvec_attribute(RTLIL::IdString id, ثابت سمتیہ<صحیح> &ڈیٹا)
{
	std::stringstream attrval;
	برائے (خودکار &i : ڈیٹا) {
		اگر (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	اوصاف[id] = RTLIL::Const(attrval.str());
}

سمتیہ<صحیح> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ثابت
{
	سمتیہ<صحیح> ڈیٹا;
	خودکار it = اوصاف.ڈھونڈو(id);
	اگر (it != اوصاف.انتہا())
		برائے (ثابت خودکار &s : بانٹو_علامات(اوصاف.at(id).decode_string())) {
			حرف *انتہا = خالی_اشاریہ;
			errno = 0;
			طویل قدر = strtol(s.c_str(), &انتہا, 10);
			اگر (انتہا != s.c_str() + s.سائز())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			اگر (errno == ERANGE || قدر < INT_MIN || قدر > زیادہ_سے_زیادہ_عدد)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			ڈیٹا.ڈالو_عقب(قدر);
		}
	واپس ڈیٹا;
}

منطقی RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ثابت
{
	اگر (current_design != خالی_اشاریہ) {
		خودکار module = current_design->module(mod_name);
		واپس module && module->get_blackbox_attribute();
	} ورنہ {
		log_warning("Unable to check if module is boxed for null design.\n");
		واپس جھوٹ;
	}
}

منطقی RTLIL::Selection::selected_module(RTLIL::IdString mod_name) ثابت
{
	اگر (complete_selection)
		واپس سچ;
	اگر (!selects_boxes && boxed_module(mod_name))
		واپس جھوٹ;
	اگر (بھرا_selection)
		واپس سچ;
	اگر (selected_modules.گنتی(mod_name) > 0)
		واپس سچ;
	اگر (selected_members.گنتی(mod_name) > 0)
		واپس سچ;
	واپس جھوٹ;
}

منطقی RTLIL::Selection::selected_پورا_module(RTLIL::IdString mod_name) ثابت
{
	اگر (complete_selection)
		واپس سچ;
	اگر (!selects_boxes && boxed_module(mod_name))
		واپس جھوٹ;
	اگر (بھرا_selection)
		واپس سچ;
	اگر (selected_modules.گنتی(mod_name) > 0)
		واپس سچ;
	واپس جھوٹ;
}

منطقی RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	اگر (complete_selection)
		واپس سچ;
	اگر (!selects_boxes && boxed_module(mod_name))
		واپس جھوٹ;
	اگر (بھرا_selection)
		واپس سچ;
	اگر (selected_modules.گنتی(mod_name) > 0)
		واپس سچ;
	اگر (selected_members.گنتی(mod_name) > 0)
		اگر (selected_members.at(mod_name).گنتی(memb_name) > 0)
			واپس سچ;
	واپس جھوٹ;
}

خالی RTLIL::Selection::optimize(RTLIL::Design *design)
{
	اگر (design != current_design) {
		current_design = design;
	}

	اگر (selects_boxes && بھرا_selection)
		complete_selection = سچ;
	اگر (complete_selection) {
		بھرا_selection = جھوٹ;
		selects_boxes = سچ;
	}
	اگر (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		واپس;
	}

	std::سمتیہ<RTLIL::IdString> del_list, جوڑو_list;

	del_list.clear();
	برائے (خودکار mod_name : selected_modules) {
		اگر (current_design->module(mod_name) == خالی_اشاریہ || (!selects_boxes && boxed_module(mod_name)))
			del_list.ڈالو_عقب(mod_name);
		selected_members.erase(mod_name);
	}
	برائے (خودکار mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	برائے (خودکار &it : selected_members)
		اگر (current_design->module(it.پہلا) == خالی_اشاریہ || (!selects_boxes && boxed_module(it.پہلا)))
			del_list.ڈالو_عقب(it.پہلا);
	برائے (خودکار mod_name : del_list)
		selected_members.erase(mod_name);

	برائے (خودکار &it : selected_members) {
		del_list.clear();
		برائے (خودکار memb_name : it.دوسرا)
			اگر (current_design->module(it.پہلا)->گنتی_id(memb_name) == 0)
				del_list.ڈالو_عقب(memb_name);
		برائے (خودکار memb_name : del_list)
			it.دوسرا.erase(memb_name);
	}

	del_list.clear();
	جوڑو_list.clear();
	برائے (خودکار &it : selected_members)
		اگر (it.دوسرا.سائز() == 0)
			del_list.ڈالو_عقب(it.پہلا);
		ورنہ اگر (it.دوسرا.سائز() == current_design->module(it.پہلا)->wires_.سائز() + current_design->module(it.پہلا)->memories.سائز() +
				current_design->module(it.پہلا)->cells_.سائز() + current_design->module(it.پہلا)->processes.سائز())
			جوڑو_list.ڈالو_عقب(it.پہلا);
	برائے (خودکار mod_name : del_list)
		selected_members.erase(mod_name);
	برائے (خودکار mod_name : جوڑو_list) {
		selected_members.erase(mod_name);
		selected_modules.داخل_کرو(mod_name);
	}

	اگر (selected_modules.سائز() == current_design->modules_.سائز()) {
		selected_modules.clear();
		selected_members.clear();
		اگر (selects_boxes)
			complete_selection = سچ;
		ورنہ
			بھرا_selection = سچ;
	}
}

خالی RTLIL::Selection::clear()
{
	بھرا_selection = جھوٹ;
	complete_selection = جھوٹ;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_متعین_کرتا (نیا define_map_t)
{
	ساکن غیرعلامتی صحیح hashidx_گنتی = 123456789;
	hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
	hashidx_ = hashidx_گنتی;

	refcount_modules_ = 0;
	selected_فعال_module.clear();
	ڈالو_بھرا_selection();

	RTLIL::Design::get_all_designs()->داخل_کرو(std::pair<غیرعلامتی صحیح, RTLIL::Design*>(hashidx_, یہ));
}

RTLIL::Design::~Design()
{
	برائے (خودکار &pr : modules_)
		حذف pr.دوسرا;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

ساکن std::map<غیرعلامتی صحیح, RTLIL::Design*> all_designs;
std::map<غیرعلامتی صحیح, RTLIL::Design*> *RTLIL::Design::get_all_designs(خالی)
{
	واپس &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	واپس RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ثابت RTLIL::Module *RTLIL::Design::module(IdString id) ثابت {
	twines_.check_owned(id);
	واپس modules_.گنتی(id) ? modules_.at(id) : تہی;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	واپس modules_.گنتی(id) ? modules_.at(id) : تہی;
}

RTLIL::Module *RTLIL::Design::چوٹی_module() ثابت
{
	RTLIL::Module *module = خالی_اشاریہ;
	صحیح module_گنتی = 0;

	برائے (خودکار mod : selected_modules()) {
		اگر (mod->get_bool_attribute(ID::چوٹی))
			واپس mod;
		module_گنتی++;
		module = mod;
	}

	واپس module_گنتی == 1 ? module : خالی_اشاریہ;
}

RTLIL::Module *RTLIL::Design::جوڑو_Module(RTLIL::IdString name)
{
	اگر (modules_.گنتی(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = نیا RTLIL::Module;
	modules_[name] = module;
	module->design = یہ;
	module->name = name;

	برائے (خودکار mon : monitors)
		mon->notify_module_جوڑو(module);

	اگر (yosys_xtrace) {
		لوگارتھم("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	واپس module;
}

خالی RTLIL::Design::scratchpad_unset(ثابت std::string &varname)
{
	scratchpad.erase(varname);
}

خالی RTLIL::Design::scratchpad_سیٹ_int(ثابت std::string &varname, صحیح قدر)
{
	scratchpad[varname] = stringf("%d", قدر);
}

خالی RTLIL::Design::scratchpad_سیٹ_bool(ثابت std::string &varname, منطقی قدر)
{
	scratchpad[varname] = قدر ? "true" : "false";
}

خالی RTLIL::Design::scratchpad_سیٹ_string(ثابت std::string &varname, std::string قدر)
{
	scratchpad[varname] = std::move(قدر);
}

صحیح RTLIL::Design::scratchpad_get_int(ثابت std::string &varname, صحیح default_قدر_2) ثابت
{
	خودکار it = scratchpad.ڈھونڈو(varname);
	اگر (it == scratchpad.انتہا())
		واپس default_قدر_2;

	ثابت std::string &str = it->دوسرا;

	اگر (str == "0" || str == "false")
		واپس 0;

	اگر (str == "1" || str == "true")
		واپس 1;

	حرف *endptr = خالی_اشاریہ;
	طویل صحیح parsed_قدر = strtol(str.c_str(), &endptr, 10);
	واپس *endptr ? default_قدر_2 : parsed_قدر;
}

منطقی RTLIL::Design::scratchpad_get_bool(ثابت std::string &varname, منطقی default_قدر_2) ثابت
{
	خودکار it = scratchpad.ڈھونڈو(varname);
	اگر (it == scratchpad.انتہا())
		واپس default_قدر_2;

	ثابت std::string &str = it->دوسرا;

	اگر (str == "0" || str == "false")
		واپس جھوٹ;

	اگر (str == "1" || str == "true")
		واپس سچ;

	واپس default_قدر_2;
}

std::string RTLIL::Design::scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_قدر_2) ثابت
{
	خودکار it = scratchpad.ڈھونڈو(varname);
	اگر (it == scratchpad.انتہا())
		واپس default_قدر_2;

	واپس it->دوسرا;
}

خالی RTLIL::Design::remove(RTLIL::Module *module)
{
	برائے (خودکار mon : monitors)
		mon->notify_module_del(module);

	اگر (yosys_xtrace) {
		لوگارتھم("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	حذف module;
}

خالی RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	جوڑو(module);
}

خالی RTLIL::Design::ترتیب_دو()
{
	scratchpad.ترتیب_دو();
	modules_.ترتیب_دو(ترتیب_دو_by_id_str(twines_));
	برائے (خودکار &it : modules_)
		it.دوسرا->ترتیب_دو();
}

خالی RTLIL::Design::ترتیب_دو_modules()
{
	scratchpad.ترتیب_دو();
	modules_.ترتیب_دو(ترتیب_دو_by_id_str(twines_));
}

خالی check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

خالی RTLIL::Design::check()
{
#اگر_غیر_تعریف_شدہ NDEBUG
	log_assert(!selection_ڈھیر.خالی_خانے());
	صحیح pool_سائز = 0;
	برائے (خودکار &it : modules_)
		pool_سائز = std::max(pool_سائز, ThreadPool::work_pool_سائز(0, it.دوسرا->cells_سائز(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_سائز);
	برائے (خودکار &it : modules_) {
		log_assert(یہ == it.دوسرا->design);
		log_assert(it.پہلا == it.دوسرا->name);
		check_module(it.دوسرا, thread_pool);
	}
#اگر_ختم
}

خالی RTLIL::Design::optimize()
{
	برائے (خودکار &it : modules_)
		it.دوسرا->optimize();
	برائے (خودکار &it : selection_ڈھیر)
		it.optimize(یہ);
	برائے (خودکار &it : selection_vars)
		it.دوسرا.optimize(یہ);
}

خالی RTLIL::Design::clone_into(RTLIL::Design *dst) ثابت
{
	log_assert(dst->modules_.خالی_خانے());
	dst->twines_ = twines_;
	برائے (خودکار it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->دوسرا->clone(dst);
}

منطقی RTLIL::Design::selected_module(IdString mod_name) ثابت
{
	اگر (!selected_فعال_module.خالی_خانے() && mod_name != selected_فعال_module)
		واپس جھوٹ;
	واپس selection().selected_module(mod_name);
}

منطقی RTLIL::Design::selected_پورا_module(RTLIL::IdString mod_name) ثابت
{
	اگر (!selected_فعال_module.خالی_خانے() && mod_name != selected_فعال_module)
		واپس جھوٹ;
	واپس selection().selected_پورا_module(mod_name);
}

منطقی RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	اگر (!selected_فعال_module.خالی_خانے() && mod_name != selected_فعال_module)
		واپس جھوٹ;
	واپس selection().selected_member(mod_name, memb_name);
}

منطقی RTLIL::Design::selected_module(RTLIL::Module *mod) ثابت
{
	واپس selected_module(mod->name);
}

منطقی RTLIL::Design::selected_پورا_module(RTLIL::Module *mod) ثابت
{
	واپس selected_پورا_module(mod->name);
}

خالی RTLIL::Design::ڈالو_selection(RTLIL::Selection sel)
{
	sel.current_design = یہ;
	selection_ڈھیر.ڈالو_عقب(sel);
}

خالی RTLIL::Design::ڈالو_خالی_خانے_selection()
{
	ڈالو_selection(RTLIL::Selection::خالی_خانے_Selection(یہ));
}

خالی RTLIL::Design::ڈالو_بھرا_selection()
{
	ڈالو_selection(RTLIL::Selection::بھرا_Selection(یہ));
}

خالی RTLIL::Design::ڈالو_complete_selection()
{
	ڈالو_selection(RTLIL::Selection::CompleteSelection(یہ));
}

خالی RTLIL::Design::نکالو_selection()
{
	selection_ڈھیر.نکالو_عقب();
	// Default to a full_selection if we ran out of stack
	اگر (selection_ڈھیر.خالی_خانے())
		ڈالو_بھرا_selection();
}

std::string RTLIL::Design::مقصد_rtlil_str(منطقی only_selected) ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(یہ), only_selected, سچ, جھوٹ, RTLIL_BACKEND::Dump_منوال::Readable);
	واپس f.str();
}

std::سمتیہ<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ثابت
{
	منطقی include_partials = partials == RTLIL::SELECT_ALL;
	منطقی exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	منطقی ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::سمتیہ<RTLIL::Module*> نتیجہ;
	نتیجہ.reserve(modules_.سائز());
	برائے (خودکار &it : modules_) {
		اگر (selected_پورا_module(it.پہلا) || (include_partials && selected_module(it.پہلا))) {
			اگر (!(exclude_boxes && it.دوسرا->get_blackbox_attribute(ignore_wb)))
				نتیجہ.ڈالو_عقب(it.دوسرا);
			ورنہ
				انتخاب (boxes)
				{
				حالت RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.دوسرا);
					توڑو;
				حالت RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.دوسرا);
					توڑو;
				حالت RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.دوسرا);
					توڑو;
				حالت RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.دوسرا);
					توڑو;
				حالت RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.دوسرا);
					توڑو;
				حالت RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.دوسرا);
					توڑو;
				طےشدہ:
					توڑو;
				}
		} ورنہ اگر (!include_partials && selected_module(it.پہلا)) {
			انتخاب(partials)
			{
			حالت RTLIL::SELECT_پورا_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.دوسرا);
				توڑو;
			حالت RTLIL::SELECT_پورا_ERR:
				log_error("Unsupported partially selected module %s.\n", it.دوسرا);
				توڑو;
			حالت RTLIL::SELECT_پورا_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.دوسرا);
				توڑو;
			طےشدہ:
				توڑو;
			}
		}
	}
	واپس نتیجہ;
}

RTLIL::Module::Module()
{
	ساکن غیرعلامتی صحیح hashidx_گنتی = 123456789;
	hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
	hashidx_ = hashidx_گنتی;

	design = خالی_اشاریہ;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->داخل_کرو(std::pair<غیرعلامتی صحیح, RTLIL::Module*>(hashidx_, یہ));
#اگر_ختم
}

RTLIL::Module::~Module()
{
	برائے (خودکار &pr : wires_)
		حذف pr.دوسرا;
	برائے (خودکار &pr : memories)
		حذف pr.دوسرا;
	برائے (خودکار &pr : cells_)
		حذف pr.دوسرا;
	برائے (خودکار &pr : processes)
		حذف pr.دوسرا;
#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#اگر_ختم
}

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
ساکن std::map<غیرعلامتی صحیح, RTLIL::Module*> all_modules;
std::map<غیرعلامتی صحیح, RTLIL::Module*> *RTLIL::Module::get_all_modules(خالی)
{
	واپس &all_modules;
}
#اگر_ختم

خالی RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	برائے (خودکار it = wires_.begin(); it != wires_.انتہا(); ++it)
		اگر (!it->دوسرا->پورٹ_داخلہ && !it->دوسرا->پورٹ_output)
			delwires.داخل_کرو(it->دوسرا);

	برائے (خودکار it = memories.begin(); it != memories.انتہا(); ++it)
		حذف it->دوسرا;
	memories.clear();

	برائے (خودکار it = cells_.begin(); it != cells_.انتہا(); ++it)
		حذف it->دوسرا;
	cells_.clear();

	برائے (خودکار it = processes.begin(); it != processes.انتہا(); ++it)
		حذف it->دوسرا;
	processes.clear();

	connections_.clear();

	remove(delwires);
	سیٹ_bool_attribute(ID::blackbox);
}

خالی RTLIL::Module::expand_interfaces(RTLIL::Design *, ثابت dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

منطقی RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	واپس جھوٹ;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ثابت dict<RTLIL::IdString, RTLIL::Const> &, منطقی mayfail)
{
	اگر (mayfail)
		واپس IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ثابت dict<RTLIL::IdString, RTLIL::Const> &, ثابت dict<RTLIL::IdString, RTLIL::Module*> &, ثابت dict<RTLIL::IdString, RTLIL::IdString> &, منطقی mayfail)
{
	اگر (mayfail)
		واپس IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

سائز_t RTLIL::Module::گنتی_id(RTLIL::IdString id)
{
	واپس wires_.گنتی(id) + cells_.گنتی(id) + memories.گنتی(id) + processes.گنتی(id);
}

#اگر_غیر_تعریف_شدہ NDEBUG
نامخلا {
	ساخت اندرونی_Cell_Checker
	{
		ثابت RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		اندرونی_Cell_Checker(ثابت RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		خالی error(صحیح linenr, std::optional<std::string> error_پیغام = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			اگر (error_پیغام) {
				log_error("%s in internal cell %s\n", *error_پیغام, cell_loc);
			} ورنہ {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		صحیح param(RTLIL::IdString name)
		{
			خودکار it = cell->parameters.ڈھونڈو(name);
			اگر (it == cell->parameters.انتہا()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.داخل_کرو(name);
			واپس it->دوسرا.as_int();
		}

		صحیح param_bool(RTLIL::IdString name)
		{
			صحیح v = param(name);
			اگر (Get_سائز(cell->parameters.at(name)) > 32)
				error(__LINE__);
			اگر (v != 0 && v != 1)
				error(__LINE__);
			واپس v;
		}

		صحیح param_bool(RTLIL::IdString name, منطقی expected)
		{
			صحیح v = param_bool(name);
			اگر (v != expected)
				error(__LINE__);
			واپس v;
		}

		خالی param_بٹ(RTLIL::IdString name, صحیح چوڑائی)
		{
			param(name);
			اگر (Get_سائز(cell->parameters.at(name)) != چوڑائی)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			واپس cell->parameters.at(name).decode_string();
		}

		خالی پورٹ(RTLIL::IdString name, صحیح چوڑائی)
		{
			خودکار it = cell->connections_.ڈھونڈو(name);
			اگر (it == cell->connections_.انتہا()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			اگر (Get_سائز(it->دوسرا) != چوڑائی) {
				std::string err = stringf("Expected port to have width %d", چوڑائی);
				error(__LINE__, err);
			}
			expected_ports.داخل_کرو(name);
		}

		خالی check_expected(منطقی check_matched_sign = جھوٹ)
		{
			برائے (خودکار &para : cell->parameters)
				اگر (expected_params.گنتی(para.پہلا) == 0)
					error(__LINE__);
			برائے (خودکار &conn : cell->connections())
				اگر (expected_ports.گنتی(conn.پہلا) == 0)
					error(__LINE__);

			اگر (check_matched_sign) {
				log_assert(expected_params.گنتی(ID::A_SIGNED) != 0 && expected_params.گنتی(ID::B_SIGNED) != 0);
				منطقی a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				منطقی b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				اگر (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		خالی check()
		{
			اگر (cell->type.is_عوامی() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				واپس;

			اگر (cell->type.اندر(ID($buf), ID($barrier))) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type.اندر(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected();
				واپس;
			}

			اگر (cell->type.اندر(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected(سچ);
				واپس;
			}

			اگر (cell->type.اندر(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected();
				واپس;
			}

			اگر (cell->type.اندر(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/جھوٹ);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected(/*check_matched_sign=*/جھوٹ);
				واپس;
			}

			اگر (cell->type.اندر(ID($سرکاؤ), ID($shiftx))) {
				اگر (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/جھوٹ);
				} ورنہ {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected(/*check_matched_sign=*/جھوٹ);
				واپس;
			}

			اگر (cell->type.اندر(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected(سچ);
				واپس;
			}

			اگر (cell->type.اندر(ID($جوڑو), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($قوت))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected(cell->type != ID($قوت));
				واپس;
			}

			اگر (cell->type == ID($fa)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::B, param(ID::چوڑائی_2));
				پورٹ(ID::C, param(ID::چوڑائی_2));
				پورٹ(ID::X, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($lcu)) {
				پورٹ(ID::P, param(ID::چوڑائی_2));
				پورٹ(ID::G, param(ID::چوڑائی_2));
				پورٹ(ID::CI, 1);
				پورٹ(ID::CO, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($alu)) {
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
				واپس;
			}

			اگر (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_چوڑائی);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected();
				Macc().from_cell(cell);
				واپس;
			}

			اگر (cell->type == ID($macc_v2)) {
				اگر (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				اگر (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_بٹ(ID::حاصل_ضرب_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_بٹ(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_بٹ(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_بٹ(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_بٹ(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				اگر (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_بٹ(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_بٹ(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_بٹ(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ثابت Const &a_چوڑائی = cell->getParam(ID::A_WIDTHS);
				ثابت Const &b_چوڑائی = cell->getParam(ID::B_WIDTHS);
				ثابت Const &c_چوڑائی = cell->getParam(ID::C_WIDTHS);
				صحیح a_چوڑائی_مجموع = 0, b_چوڑائی_مجموع = 0, c_چوڑائی_مجموع = 0;
				برائے (صحیح i = 0; i < param(ID::NPRODUCTS); i++) {
					a_چوڑائی_مجموع += a_چوڑائی.نکالو_2(16 * i, 16).as_int(جھوٹ);
					b_چوڑائی_مجموع += b_چوڑائی.نکالو_2(16 * i, 16).as_int(جھوٹ);
				}
				برائے (صحیح i = 0; i < param(ID::NADDENDS); i++) {
					c_چوڑائی_مجموع += c_چوڑائی.نکالو_2(16 * i, 16).as_int(جھوٹ);
				}
				پورٹ(ID::A, a_چوڑائی_مجموع);
				پورٹ(ID::B, b_چوڑائی_مجموع);
				پورٹ(ID::C, c_چوڑائی_مجموع);
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected();
				واپس;
			}

			اگر (cell->type.اندر(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				check_expected(/*check_matched_sign=*/جھوٹ);
				واپس;
			}

			اگر (cell->type == ID($قاش)) {
				param(ID::انحراف_2);
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::Y, param(ID::Y_چوڑائی));
				اگر (param(ID::انحراف_2) + param(ID::Y_چوڑائی) > param(ID::A_چوڑائی))
					error(__LINE__);
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($concat)) {
				پورٹ(ID::A, param(ID::A_چوڑائی));
				پورٹ(ID::B, param(ID::B_چوڑائی));
				پورٹ(ID::Y, param(ID::A_چوڑائی) + param(ID::B_چوڑائی));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($mux)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::B, param(ID::چوڑائی_2));
				پورٹ(ID::S, 1);
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($pmux)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::B, param(ID::چوڑائی_2) * param(ID::S_چوڑائی));
				پورٹ(ID::S, param(ID::S_چوڑائی));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($bmux)) {
				پورٹ(ID::A, param(ID::چوڑائی_2) << param(ID::S_چوڑائی));
				پورٹ(ID::S, param(ID::S_چوڑائی));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($demux)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::S, param(ID::S_چوڑائی));
				پورٹ(ID::Y, param(ID::چوڑائی_2) << param(ID::S_چوڑائی));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($lut)) {
				param(ID::LUT);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::Y, 1);
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($sop)) {
				param(ID::گہرائی_2);
				param(ID::جدول);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::Y, 1);
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($sr)) {
				param_bool(ID::سیٹ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورٹ(ID::سیٹ_2, param(ID::چوڑائی_2));
				پورٹ(ID::CLR, param(ID::چوڑائی_2));
				پورٹ(ID::Q,   param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($ff)) {
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::EN, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::سیٹ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::سیٹ_2, param(ID::چوڑائی_2));
				پورٹ(ID::CLR, param(ID::چوڑائی_2));
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($dffsre)) {
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
				واپس;
			}

			اگر (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بٹ(ID::ARST_قدر, param(ID::چوڑائی_2));
				پورٹ(ID::CLK, 1);
				پورٹ(ID::ARST, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_بٹ(ID::SRST_قدر, param(ID::چوڑائی_2));
				پورٹ(ID::CLK, 1);
				پورٹ(ID::SRST, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type.اندر(ID($sdffe), ID($sdffce))) {
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
				واپس;
			}

			اگر (cell->type == ID($adffe)) {
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
				واپس;
			}

			اگر (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::ALOAD, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::AD, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($aldffe)) {
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
				واپس;
			}

			اگر (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				پورٹ(ID::EN, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بٹ(ID::ARST_قدر, param(ID::چوڑائی_2));
				پورٹ(ID::EN, 1);
				پورٹ(ID::ARST, 1);
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::سیٹ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورٹ(ID::EN, 1);
				پورٹ(ID::سیٹ_2, param(ID::چوڑائی_2));
				پورٹ(ID::CLR, param(ID::چوڑائی_2));
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_بٹ);
				param(ID::STATE_عدد);
				param(ID::STATE_عدد_LOG2);
				param(ID::STATE_RST);
				param_بٹ(ID::STATE_جدول, param(ID::STATE_بٹ) * param(ID::STATE_عدد));
				param(ID::TRANS_عدد);
				param_بٹ(ID::TRANS_جدول, param(ID::TRANS_عدد) * (2*param(ID::STATE_عدد_LOG2) + param(ID::CTRL_اندر_چوڑائی) + param(ID::CTRL_برآمد_چوڑائی)));
				پورٹ(ID::CLK, 1);
				پورٹ(ID::ARST, 1);
				پورٹ(ID::CTRL_اندر, param(ID::CTRL_اندر_چوڑائی));
				پورٹ(ID::CTRL_برآمد, param(ID::CTRL_برآمد_چوڑائی));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::EN, 1);
				پورٹ(ID::ADDR, param(ID::ABITS));
				پورٹ(ID::ڈیٹا_2, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($memrd_v2)) {
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
				پورٹ(ID::ڈیٹا_2, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::ترجیح);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::EN, param(ID::چوڑائی_2));
				پورٹ(ID::ADDR, param(ID::ABITS));
				پورٹ(ID::ڈیٹا_2, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($memwr_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PORTID);
				param(ID::ترجیح_نقاب_2);
				پورٹ(ID::CLK, 1);
				پورٹ(ID::EN, param(ID::چوڑائی_2));
				پورٹ(ID::ADDR, param(ID::ABITS));
				پورٹ(ID::ڈیٹا_2, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::ترجیح);
				پورٹ(ID::ADDR, param(ID::ABITS));
				پورٹ(ID::ڈیٹا_2, param(ID::چوڑائی_2) * param(ID::الفاظ));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::ترجیح);
				پورٹ(ID::ADDR, param(ID::ABITS));
				پورٹ(ID::ڈیٹا_2, param(ID::چوڑائی_2) * param(ID::الفاظ));
				پورٹ(ID::EN, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($mem)) {
				param(ID::MEMID);
				param(ID::سائز_2);
				param(ID::انحراف_2);
				param(ID::INIT);
				param_بٹ(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_بٹ(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_بٹ(ID::RD_TRANSPARENT, max(1, param(ID::RD_PORTS)));
				param_بٹ(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_بٹ(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				پورٹ(ID::RD_CLK, param(ID::RD_PORTS));
				پورٹ(ID::RD_EN, param(ID::RD_PORTS));
				پورٹ(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				پورٹ(ID::RD_ڈیٹا, param(ID::RD_PORTS) * param(ID::چوڑائی_2));
				پورٹ(ID::WR_CLK, param(ID::WR_PORTS));
				پورٹ(ID::WR_EN, param(ID::WR_PORTS) * param(ID::چوڑائی_2));
				پورٹ(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				پورٹ(ID::WR_ڈیٹا, param(ID::WR_PORTS) * param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($mem_v2)) {
				param(ID::MEMID);
				param(ID::سائز_2);
				param(ID::انحراف_2);
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
				پورٹ(ID::RD_ڈیٹا, param(ID::RD_PORTS) * param(ID::چوڑائی_2));
				پورٹ(ID::WR_CLK, param(ID::WR_PORTS));
				پورٹ(ID::WR_EN, param(ID::WR_PORTS) * param(ID::چوڑائی_2));
				پورٹ(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				پورٹ(ID::WR_ڈیٹا, param(ID::WR_PORTS) * param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($tribuf)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				پورٹ(ID::EN, 1);
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($bweqx)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::B, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($bwmux)) {
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::B, param(ID::چوڑائی_2));
				پورٹ(ID::S, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type.اندر(ID($أكد), ID($assume), ID($زندہ), ID($fair), ID($cover))) {
				پورٹ(ID::A, 1);
				پورٹ(ID::EN, 1);
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($initstate)) {
				پورٹ(ID::Y, 1);
				check_expected();
				واپس;
			}

			اگر (cell->type.اندر(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type.اندر(ID($anyinit))) {
				پورٹ(ID::D, param(ID::چوڑائی_2));
				پورٹ(ID::Q, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($equiv)) {
				پورٹ(ID::A, 1);
				پورٹ(ID::B, 1);
				پورٹ(ID::Y, 1);
				check_expected();
				واپس;
			}

			اگر (cell->type.اندر(ID($specify2), ID($specify3))) {
				param_bool(ID::بھرا);
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
				اگر (cell->type == ID($specify3)) {
					param_bool(ID::کنارہ_EN);
					param_bool(ID::کنارہ_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					پورٹ(ID::DAT, param(ID::DST_چوڑائی));
				}
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($specrule)) {
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
				واپس;
			}

			اگر (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::ترجیح);
				پورٹ(ID::EN, 1);
				پورٹ(ID::TRG, param(ID::TRG_چوڑائی));
				پورٹ(ID::دلائل, param(ID::دلائل_چوڑائی));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				اگر (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
					error(__LINE__);
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::ترجیح);
				پورٹ(ID::A, 1);
				پورٹ(ID::EN, 1);
				پورٹ(ID::TRG, param(ID::TRG_چوڑائی));
				پورٹ(ID::دلائل, param(ID::دلائل_چوڑائی));
				check_expected();
				واپس;
			}

			اگر (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				اگر (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				واپس;
			}

			اگر (cell->type == ID($_BUF_))    { پورٹ(ID::A,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_NOT_))    { پورٹ(ID::A,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_AND_))    { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_NAND_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_OR_))     { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_NOR_))    { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_XOR_))    { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_XNOR_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_ANDNOT_)) { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_ORNOT_))  { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_MUX_))    { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::S,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_NMUX_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::S,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_AOI3_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_OAI3_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_AOI4_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::D,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_OAI4_))   { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::D,1); پورٹ(ID::Y,1); check_expected(); واپس; }

			اگر (cell->type == ID($_TBUF_))  { پورٹ(ID::A,1); پورٹ(ID::Y,1); پورٹ(ID::E,1); check_expected(); واپس; }

			اگر (cell->type == ID($_MUX4_))  { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::D,1); پورٹ(ID::S,1); پورٹ(ID::T,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_MUX8_))  { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::D,1); پورٹ(ID::E,1); پورٹ(ID::F,1); پورٹ(ID::G,1); پورٹ(ID::H,1); پورٹ(ID::S,1); پورٹ(ID::T,1); پورٹ(ID::U,1); پورٹ(ID::Y,1); check_expected(); واپس; }
			اگر (cell->type == ID($_MUX16_)) { پورٹ(ID::A,1); پورٹ(ID::B,1); پورٹ(ID::C,1); پورٹ(ID::D,1); پورٹ(ID::E,1); پورٹ(ID::F,1); پورٹ(ID::G,1); پورٹ(ID::H,1); پورٹ(ID::I,1); پورٹ(ID::J,1); پورٹ(ID::K,1); پورٹ(ID::L,1); پورٹ(ID::M,1); پورٹ(ID::N,1); پورٹ(ID::O,1); پورٹ(ID::P,1); پورٹ(ID::S,1); پورٹ(ID::T,1); پورٹ(ID::U,1); پورٹ(ID::V,1); پورٹ(ID::Y,1); check_expected(); واپس; }

			اگر (cell->type.اندر(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ پورٹ(ID::S,1); پورٹ(ID::R,1); پورٹ(ID::Q,1); check_expected(); واپس; }

			اگر (cell->type == ID($_FF_)) { پورٹ(ID::D,1); پورٹ(ID::Q,1); check_expected();  واپس; }

			اگر (cell->type.اندر(ID($_DFF_N_), ID($_DFF_P_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); check_expected(); واپس; }

			اگر (cell->type.اندر(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::E,1); check_expected(); واپس; }

			اگر (cell->type.اندر(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::R,1); check_expected(); واپس; }

			اگر (cell->type.اندر(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::R,1); پورٹ(ID::E,1); check_expected(); واپس; }

			اگر (cell->type.اندر(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::L,1); پورٹ(ID::AD,1); check_expected(); واپس; }

			اگر (cell->type.اندر(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::L,1); پورٹ(ID::AD,1); پورٹ(ID::E,1); check_expected(); واپس; }

			اگر (cell->type.اندر(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ پورٹ(ID::C,1); پورٹ(ID::S,1); پورٹ(ID::R,1); پورٹ(ID::D,1); پورٹ(ID::Q,1); check_expected(); واپس; }

			اگر (cell->type.اندر(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ پورٹ(ID::C,1); پورٹ(ID::S,1); پورٹ(ID::R,1); پورٹ(ID::D,1); پورٹ(ID::E,1); پورٹ(ID::Q,1); check_expected(); واپس; }

			اگر (cell->type.اندر(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::R,1); check_expected(); واپس; }

			اگر (cell->type.اندر(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ پورٹ(ID::D,1); پورٹ(ID::Q,1); پورٹ(ID::C,1); پورٹ(ID::R,1); پورٹ(ID::E,1); check_expected(); واپس; }

			اگر (cell->type.اندر(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ پورٹ(ID::E,1); پورٹ(ID::D,1); پورٹ(ID::Q,1); check_expected(); واپس; }

			اگر (cell->type.اندر(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ پورٹ(ID::E,1); پورٹ(ID::R,1); پورٹ(ID::D,1); پورٹ(ID::Q,1); check_expected(); واپس; }

			اگر (cell->type.اندر(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ پورٹ(ID::E,1); پورٹ(ID::S,1); پورٹ(ID::R,1); پورٹ(ID::D,1); پورٹ(ID::Q,1); check_expected(); واپس; }

			اگر (cell->type.اندر(ID($سیٹ_لیبل))) {
				param(ID::چوڑائی_2);
				param(ID::لیبل_2);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::سیٹ_2, param(ID::چوڑائی_2));
				پورٹ(ID::CLR, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}
			اگر (cell->type.اندر(ID($get_لیبل),ID($original_لیبل))) {
				param(ID::چوڑائی_2);
				param(ID::لیبل_2);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}
			اگر (cell->type.اندر(ID($overwrite_لیبل))) {
				param(ID::چوڑائی_2);
				param(ID::لیبل_2);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::سیٹ_2, param(ID::چوڑائی_2));
				پورٹ(ID::CLR, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}
			اگر (cell->type.اندر(ID($future_ff))) {
				param(ID::چوڑائی_2);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}
			اگر (cell->type.اندر(ID($داخلہ_پورٹ))) {
				param(ID::چوڑائی_2);
				پورٹ(ID::Y, param(ID::چوڑائی_2));
				check_expected();
				واپس;
			}
			اگر (cell->type.اندر(ID($connect))) {
				param(ID::چوڑائی_2);
				پورٹ(ID::A, param(ID::چوڑائی_2));
				پورٹ(ID::B, param(ID::چوڑائی_2));
				check_expected();
				واپس;
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
#اگر_ختم

خالی RTLIL::Module::ترتیب_دو()
{
	wires_.ترتیب_دو(ترتیب_دو_by_id_str(design->twines()));
	cells_.ترتیب_دو(ترتیب_دو_by_id_str(design->twines()));
	parameter_default_اقدار.ترتیب_دو(ترتیب_دو_by_id_str(design->twines()));
	memories.ترتیب_دو(ترتیب_دو_by_id_str(design->twines()));
	processes.ترتیب_دو(ترتیب_دو_by_id_str(design->twines()));
	برائے (خودکار &it : cells_)
		it.دوسرا->ترتیب_دو();
	برائے (خودکار &it : wires_)
		it.دوسرا->اوصاف.ترتیب_دو(ترتیب_دو_by_id_str(design->twines()));
	برائے (خودکار &it : memories)
		it.دوسرا->اوصاف.ترتیب_دو(ترتیب_دو_by_id_str(design->twines()));
}

#اگر_غیر_تعریف_شدہ NDEBUG
ساکن خالی check_id_کلیدیں(ثابت dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	برائے (خودکار &it : keyed)
		log_assert(!it.پہلا.خالی_خانے());
}
#اگر_ختم

خالی check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#اگر_غیر_تعریف_شدہ NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_سائز(0, module->cells_سائز(), 1000));
	ثابت RTLIL::Module *const_module = module;

	pool<std::string> حافظہ_لڑیاں;
	برائے (خودکار &it : module->memories) {
		log_assert(it.پہلا == it.دوسرا->name);
		log_assert(it.پہلا != IdString::Null);
		log_assert(it.دوسرا->چوڑائی >= 0);
		log_assert(it.دوسرا->سائز >= 0);
		check_id_کلیدیں(it.دوسرا->اوصاف);
		حافظہ_لڑیاں.داخل_کرو(it.دوسرا->name.str());
	}

	std::سمتیہ<MonotonicFlag> ports_declared(Get_سائز(module->ports));
	Sharded_سمتیہ<std::string> memids(subpool);
	subpool.تسلسل([const_module, &ports_declared, &حافظہ_لڑیاں, &memids](ثابت ParallelDispatchThreadPool::تسلسل_Ctx &ctx) {
		برائے (صحیح i : ctx.شے_range(const_module->cells_سائز())) {
			خودکار it = *const_module->cells_.element(i);
			log_assert(const_module == it.دوسرا->module);
			log_assert(it.پہلا == it.دوسرا->name);
			log_assert(it.پہلا != IdString::Null);
			log_assert(!it.دوسرا->type.خالی_خانے());
			برائے (خودکار &it2 : it.دوسرا->connections()) {
				log_assert(it2.پہلا != IdString::Null);
				it2.دوسرا.check(const_module);
			}
			check_id_کلیدیں(it.دوسرا->اوصاف);
			check_id_کلیدیں(it.دوسرا->parameters);
			اندرونی_Cell_Checker checker(const_module, it.دوسرا);
			checker.check();
			اگر (it.دوسرا->has_memid()) {
				log_assert(حافظہ_لڑیاں.گنتی(it.دوسرا->parameters.at(ID::MEMID).decode_string()));
			} ورنہ اگر (it.دوسرا->is_mem_cell()) {
				std::string memid = it.دوسرا->parameters.at(ID::MEMID).decode_string();
				log_assert(!حافظہ_لڑیاں.گنتی(memid));
				memids.داخل_کرو(ctx, std::move(memid));
			}
			خودکار cell_mod = const_module->design->module(it.دوسرا->name);
			اگر (cell_mod != خالی_اشاریہ) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#اگر_تعریف_شدہ __GNUC__
				#ہدایت GCC diagnostic ڈالو
				#ہدایت GCC diagnostic ignored "-Wdeprecated-declarations"
				#اگر_ختم
				log_assert(!it.دوسرا->get_blackbox_attribute());
				#اگر_تعریف_شدہ __GNUC__
				#ہدایت GCC diagnostic نکالو
				#اگر_ختم
			}
		}

		برائے (صحیح i : ctx.شے_range(const_module->wires_سائز())) {
			خودکار it = *const_module->wires_.element(i);
			log_assert(const_module == it.دوسرا->module);
			log_assert(it.پہلا == it.دوسرا->name);
			log_assert(it.پہلا != IdString::Null);
			log_assert(it.دوسرا->چوڑائی >= 0);
			log_assert(it.دوسرا->پورٹ_id >= 0);
			check_id_کلیدیں(it.دوسرا->اوصاف);
			اگر (it.دوسرا->پورٹ_id) {
				log_assert(Get_سائز(const_module->ports) >= it.دوسرا->پورٹ_id);
				log_assert(const_module->ports.at(it.دوسرا->پورٹ_id-1) == it.دوسرا->name);
				log_assert(it.دوسرا->پورٹ_داخلہ || it.دوسرا->پورٹ_output);
				log_assert(it.دوسرا->پورٹ_id <= Get_سائز(ports_declared));
				منطقی previously_declared = ports_declared[it.دوسرا->پورٹ_id-1].سیٹ_and_return_old();
				log_assert(previously_declared == جھوٹ);
			} ورنہ
				log_assert(!it.دوسرا->پورٹ_داخلہ && !it.دوسرا->پورٹ_output);
		}
	});
	برائے (ثابت MonotonicFlag &پورٹ_declared : ports_declared)
		log_assert(پورٹ_declared.بھرو() == سچ);
	pool<std::string> memids_pool;
	برائے (std::string &memid : memids)
		log_assert(memids_pool.داخل_کرو(memid).دوسرا);

	برائے (خودکار &it : module->processes) {
		log_assert(it.پہلا == it.دوسرا->name);
		log_assert(it.پہلا != IdString::Null);
		log_assert(it.دوسرا->جڑ_case.compare.خالی_خانے());
		std::سمتیہ<RTLIL::Case_قاعدہ*> all_cases = {&it.دوسرا->جڑ_case};
		برائے (سائز_t i = 0; i < all_cases.سائز(); i++) {
			برائے (خودکار &switch_it : all_cases[i]->switches) {
				برائے (خودکار &case_it : switch_it->cases) {
					برائے (خودکار &compare_it : case_it->compare) {
						log_assert(switch_it->اشارہ.سائز() == compare_it.سائز());
					}
					all_cases.ڈالو_عقب(case_it);
				}
			}
		}
		برائے (خودکار &sync_it : it.دوسرا->syncs) {
			انتخاب (sync_it->type) {
				حالت RTLIL::SyncType::ST0:
				حالت RTLIL::SyncType::ST1:
				حالت RTLIL::SyncType::STp:
				حالت RTLIL::SyncType::STn:
				حالت RTLIL::SyncType::STe:
					log_assert(!sync_it->اشارہ.خالی_خانے());
					توڑو;
				حالت RTLIL::SyncType::STa:
				حالت RTLIL::SyncType::STg:
				حالت RTLIL::SyncType::STi:
					log_assert(sync_it->اشارہ.خالی_خانے());
					توڑو;
			}
		}
	}

	برائے (خودکار &it : module->connections_) {
		log_assert(it.پہلا.سائز() == it.دوسرا.سائز());
		log_assert(!it.پہلا.has_const());
		it.پہلا.check(module);
		it.دوسرا.check(module);
	}

	check_id_کلیدیں(module->اوصاف);
#اگر_ختم
}

خالی RTLIL::Module::check()
{
	صحیح pool_سائز = ThreadPool::work_pool_سائز(0, cells_سائز(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_سائز);
	check_module(یہ, thread_pool);
}

خالی RTLIL::Module::optimize()
{
}

خالی RTLIL::Module::cloneInto(RTLIL::Module *new_mod) ثابت
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	برائے (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_اقدار.clear();
	برائے (خودکار &it : parameter_default_اقدار)
		new_mod->parameter_default_اقدار[dst_twines.copy_from(design->twines(), it.پہلا)] = it.دوسرا;

	برائے (خودکار &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->اوصاف, اوصاف, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	برائے (خودکار it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->پہلا);
		wire_map[it->دوسرا] = new_mod->جوڑو_Wire(dst_id, it->دوسرا);
	}

	برائے (خودکار it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->پہلا);
		new_mod->جوڑو_حافظہ(dst_id, it->دوسرا);
	}

	برائے (خودکار it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->پہلا);
		new_mod->جوڑو_Cell(dst_id, it->دوسرا);
	}

	برائے (خودکار it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->پہلا);
		new_mod->جوڑو_Process(dst_id, it->دوسرا);
	}

	ساخت Rewrite_Sig_Spec_کارکن
	{
		ثابت dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ثابت RTLIL::Module *new_mod;
		خالی عامل()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([یہ](RTLIL::Wire *&wire) {
				خودکار it = wire_map.ڈھونڈو(wire);
				اگر (it != wire_map.انتہا())
					wire = it->دوسرا;
				ورنہ
					log_assert(wire->module == new_mod);
			});
		}
	};

	Rewrite_Sig_Spec_کارکن rewrite_Sig_Spec_کارکن{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewrite_Sig_Spec_کارکن);
	new_mod->fixup_ports();
}

RTLIL::Module *RTLIL::Module::clone() ثابت
{
	RTLIL::Module *new_mod = نیا RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	واپس new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) ثابت
{
	RTLIL::Module *new_mod = نیا RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->جوڑو(new_mod);
	واپس new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString ہدف_name) ثابت
{
	RTLIL::Module *new_mod = نیا RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = ہدف_name;
	cloneInto(new_mod);
	dst->جوڑو(new_mod);
	واپس new_mod;
}

منطقی RTLIL::Module::has_memories() ثابت
{
	واپس !memories.خالی_خانے();
}

منطقی RTLIL::Module::has_processes() ثابت
{
	واپس !processes.خالی_خانے();
}

منطقی RTLIL::Module::has_memories_warn() ثابت
{
	اگر (!memories.خالی_خانے())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", یہ);
	واپس !memories.خالی_خانے();
}

منطقی RTLIL::Module::has_processes_warn() ثابت
{
	اگر (!processes.خالی_خانے())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", یہ);
	واپس !processes.خالی_خانے();
}

منطقی RTLIL::Module::is_selected() ثابت
{
	واپس design->selected_module(یہ->name);
}

منطقی RTLIL::Module::is_selected_پورا() ثابت
{
	واپس design->selected_پورا_module(یہ->name);
}

std::سمتیہ<RTLIL::Wire*> RTLIL::Module::selected_wires() ثابت
{
	std::سمتیہ<RTLIL::Wire*> نتیجہ;
	نتیجہ.reserve(wires_.سائز());
	برائے (خودکار &it : wires_)
		اگر (design->selected(یہ, it.دوسرا))
			نتیجہ.ڈالو_عقب(it.دوسرا);
	واپس نتیجہ;
}

std::سمتیہ<RTLIL::Cell*> RTLIL::Module::selected_cells() ثابت
{
	std::سمتیہ<RTLIL::Cell*> نتیجہ;
	نتیجہ.reserve(cells_.سائز());
	برائے (خودکار &it : cells_)
		اگر (design->selected(یہ, it.دوسرا))
			نتیجہ.ڈالو_عقب(it.دوسرا);
	واپس نتیجہ;
}

std::سمتیہ<RTLIL::حافظہ*> RTLIL::Module::selected_memories() ثابت
{
	std::سمتیہ<RTLIL::حافظہ*> نتیجہ;
	نتیجہ.reserve(memories.سائز());
	برائے (خودکار &it : memories)
		اگر (design->selected(یہ, it.دوسرا))
			نتیجہ.ڈالو_عقب(it.دوسرا);
	واپس نتیجہ;
}

std::سمتیہ<RTLIL::Process*> RTLIL::Module::selected_processes() ثابت
{
	std::سمتیہ<RTLIL::Process*> نتیجہ;
	نتیجہ.reserve(processes.سائز());
	برائے (خودکار &it : processes)
		اگر (design->selected(یہ, it.دوسرا))
			نتیجہ.ڈالو_عقب(it.دوسرا);
	واپس نتیجہ;
}

std::سمتیہ<RTLIL::SelectedMember> RTLIL::Module::selected_members() ثابت
{
	std::سمتیہ<RTLIL::SelectedMember> نتیجہ;
	برائے (خودکار cell : selected_cells())
		نتیجہ.ڈالو_عقب({cell, cell->name});
	برائے (خودکار حافظہ_3 : selected_memories())
		نتیجہ.ڈالو_عقب({حافظہ_3, حافظہ_3->name});
	برائے (خودکار wire : selected_wires())
		نتیجہ.ڈالو_عقب({wire, wire->name});
	برائے (خودکار process : selected_processes())
		نتیجہ.ڈالو_عقب({process, process->name});
	واپس نتیجہ;
}

خالی RTLIL::Module::جوڑو(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(گنتی_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = یہ;
}

خالی RTLIL::Module::جوڑو(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(گنتی_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = یہ;
}

خالی RTLIL::Module::جوڑو(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(گنتی_id(process->name) == 0);
	processes[process->name] = process;
	process->module = یہ;
}

خالی RTLIL::Module::remove(ثابت pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	اگر (wires.خالی_خانے())
		واپس;

	ساخت Delete_Wire_کارکن
	{
		RTLIL::Module *module;
		ثابت pool<RTLIL::Wire*> *wires_p;

		خالی عامل()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([یہ](RTLIL::Wire *&wire) {
				اگر (wires_p->گنتی(wire))
					wire = module->جوڑو_Wire(stringf("$delete_wire$%d", (صحیح)autoidx++), wire->چوڑائی);
			});
		}

		خالی عامل()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	Delete_Wire_کارکن delete_wire_کارکن;
	delete_wire_کارکن.module = یہ;
	delete_wire_کارکن.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_کارکن);

	اگر (design->flagBufferedNormalized) {
		برائے (خودکار wire : wires) {
			buf_norm_wire_قطار.erase(wire);
			buf_norm_connect_اشاریہ.erase(wire);
		}
	}

	برائے (خودکار &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.گنتی(id) != 0);
		wires_.erase(id);
		حذف it;
	}
}

خالی RTLIL::Module::remove(RTLIL::Cell *cell)
{
	جب (!cell->connections_.خالی_خانے())
		cell->unset_پورٹ(cell->connections_.begin()->پہلا);

	log_assert(cells_.گنتی(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	اگر (design && design->flagBufferedNormalized && buf_norm_cell_قطار.گنتی(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.داخل_کرو(cell);
	} ورنہ {
		حذف cell;
	}
}

خالی RTLIL::Module::remove(RTLIL::حافظہ *حافظہ_3)
{
	log_assert(memories.گنتی(حافظہ_3->name) != 0);
	memories.erase(حافظہ_3->name);
	حذف حافظہ_3;
}

خالی RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.گنتی(process->name) != 0);
	processes.erase(process->name);
	حذف process;
}

خالی RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	جوڑو(wire);
}

خالی RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	جوڑو(cell);
}

خالی RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	اگر (old_id != IdString::Null && wires_.گنتی(old_id))
		rename(wires_.at(old_id), new_name);
	ورنہ اگر (old_id != IdString::Null && cells_.گنتی(old_id))
		rename(cells_.at(old_id), new_name);
	ورنہ
		log_abort();
}

خالی RTLIL::Module::تبادلہ_نام(RTLIL::Wire *w1, RTLIL::Wire *w2)
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
	تبادلہ(w1->name, w2->name);
}

خالی RTLIL::Module::تبادلہ_نام(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	تبادلہ(c1->name, c2->name);
}

RTLIL::IdString RTLIL::Module::uniquify(RTLIL::IdString name)
{
	صحیح اشاریہ = 0;
	واپس uniquify(name, اشاریہ);
}

IdString RTLIL::Module::uniquify(IdString name, صحیح &اشاریہ)
{
	اگر (اشاریہ == 0) {
		اگر (گنتی_id(name) == 0)
			واپس name;
		اشاریہ++;
	}

	جب (1) {
		IdString new_name = design->twines().جوڑو(TwineSpec::Suffix{name, stringf("_%d", اشاریہ)});
		اگر (گنتی_id(new_name) == 0)
			واپس new_name;
		اشاریہ++;
	}
}

ساکن منطقی fixup_ports_compare(ثابت RTLIL::Wire *a, ثابت RTLIL::Wire *b)
{
	اگر (a->پورٹ_id && !b->پورٹ_id)
		واپس سچ;
	اگر (!a->پورٹ_id && b->پورٹ_id)
		واپس جھوٹ;

	اگر (a->پورٹ_id == b->پورٹ_id)
		واپس a->name < b->name;
	واپس a->پورٹ_id < b->پورٹ_id;
}

خالی RTLIL::Module::connect(ثابت RTLIL::SigSig &conn)
{
	برائے (خودکار mon : monitors)
		mon->notify_connect(یہ, conn);

	اگر (design)
		برائے (خودکار mon : design->monitors)
			mon->notify_connect(یہ, conn);

	// ignore all attempts to assign constants to other constants
	اگر (conn.پہلا.has_const()) {
		RTLIL::SigSig new_conn;
		برائے (صحیح i = 0; i < Get_سائز(conn.پہلا); i++)
			اگر (conn.پہلا[i].wire) {
				new_conn.پہلا.append(conn.پہلا[i]);
				new_conn.دوسرا.append(conn.دوسرا[i]);
			}
		اگر (Get_سائز(new_conn.پہلا))
			connect(new_conn);
		واپس;
	}

	اگر (yosys_xtrace) {
		لوگارتھم("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", یہ, log_signal(conn.پہلا), log_signal(conn.دوسرا), Get_سائز(conn.پہلا));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(Get_سائز(conn.پہلا) == Get_سائز(conn.دوسرا));
	connections_.ڈالو_عقب(conn);
}

خالی RTLIL::Module::connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

خالی RTLIL::Module::new_connections(ثابت std::سمتیہ<RTLIL::SigSig> &new_conn)
{
	برائے (خودکار mon : monitors)
		mon->notify_connect(یہ, new_conn);

	اگر (design)
		برائے (خودکار mon : design->monitors)
			mon->notify_connect(یہ, new_conn);

	اگر (yosys_xtrace) {
		لوگارتھم("#X# New connections vector in %s:\n", یہ);
		برائے (خودکار &conn: new_conn)
			لوگارتھم("#X#    %s = %s (%d bits)\n", log_signal(conn.پہلا), log_signal(conn.دوسرا), Get_سائز(conn.پہلا));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ثابت std::سمتیہ<RTLIL::SigSig> &RTLIL::Module::connections() ثابت
{
	واپس connections_;
}

خالی RTLIL::Module::fixup_ports()
{
	std::سمتیہ<RTLIL::Wire*> all_ports;

	برائے (خودکار &w : wires_)
		اگر (w.دوسرا->پورٹ_داخلہ || w.دوسرا->پورٹ_output)
			all_ports.ڈالو_عقب(w.دوسرا);
		ورنہ
			w.دوسرا->پورٹ_id = 0;

	std::ترتیب_دو(all_ports.begin(), all_ports.انتہا(), fixup_ports_compare);

	اگر (design && design->flagBufferedNormalized) {
		برائے (خودکار &w : wires_)
			اگر (w.دوسرا->driverCell_ && w.دوسرا->driverCell_->type == ID($داخلہ_پورٹ))
				buf_norm_wire_قطار.داخل_کرو(w.دوسرا);

		buf_norm_wire_قطار.داخل_کرو(all_ports.begin(), all_ports.انتہا());
	}

	ports.clear();
	برائے (سائز_t i = 0; i < all_ports.سائز(); i++) {
		ports.ڈالو_عقب(all_ports[i]->name);
		all_ports[i]->پورٹ_id = i+1;
	}
}

خالی RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		ثابت dict<IdString, RTLIL::Const> &src,
		ثابت RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	اگر (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		واپس;
	}
	dst.clear();
	برائے (صحیح i = Get_سائز(src) - 1; i >= 0; i--) {
		خودکار &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.پہلا)] = it.دوسرا;
	}
}

RTLIL::Wire *RTLIL::Module::جوڑو_Wire(RTLIL::IdString name, صحیح چوڑائی)
{
	log_assert(design);
	log_assert(چوڑائی >= 0 && چوڑائی < RTLIL::چوڑائی_حد_بندی);
	RTLIL::Wire *wire = نیا RTLIL::Wire;
	wire->چوڑائی = چوڑائی;
	wire->name = name;
	جوڑو(wire);
	واپس wire;
}

RTLIL::Wire *RTLIL::Module::جوڑو_Wire(IdString name, ثابت RTLIL::Wire *other)
{
	RTLIL::Wire *wire = جوڑو_Wire(name);
	wire->چوڑائی = other->چوڑائی;
	wire->آغاز_انحراف = other->آغاز_انحراف;
	wire->پورٹ_id = other->پورٹ_id;
	wire->پورٹ_داخلہ = other->پورٹ_داخلہ;
	wire->پورٹ_output = other->پورٹ_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ثابت RTLIL::Design *src_design = other->module ? other->module->design : خالی_اشاریہ;
		RTLIL::copy_attr_dict(wire->اوصاف, other->اوصاف, src_design, یہ->design);
	}
	واپس wire;
}

RTLIL::Cell *RTLIL::Module::جوڑو_Cell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = نیا RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	جوڑو(cell);
	واپس cell;
}

RTLIL::Cell *RTLIL::Module::جوڑو_Cell(IdString name, ثابت RTLIL::Cell *other)
{
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : خالی_اشاریہ;
	منطقی cross_pool = src_design && یہ->design && src_design != یہ->design;

	IdString type = other->type.ref();
	اگر (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = جوڑو_Cell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, یہ->design);
	RTLIL::copy_attr_dict(cell->اوصاف, other->اوصاف, src_design, یہ->design);

	اگر (!cross_pool)
		cell->connections_ = other->connections_;
	ورنہ
		برائے (صحیح i = Get_سائز(other->connections_) - 1; i >= 0; i--) {
			خودکار &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.پہلا)] = c.دوسرا;
		}
	واپس cell;
}

RTLIL::حافظہ *RTLIL::Module::جوڑو_حافظہ(IdString name)
{
	log_assert(design);
	RTLIL::حافظہ *mem = نیا RTLIL::حافظہ;
	mem->module = یہ;
	mem->name = name;
	memories[name] = mem;
	واپس mem;
}

RTLIL::حافظہ *RTLIL::Module::جوڑو_حافظہ(IdString name, ثابت RTLIL::حافظہ *other)
{
	log_assert(design);
	RTLIL::حافظہ *mem = نیا RTLIL::حافظہ;
	mem->module = یہ;
	mem->name = name;
	mem->چوڑائی = other->چوڑائی;
	mem->آغاز_انحراف = other->آغاز_انحراف;
	mem->سائز = other->سائز;
	RTLIL::copy_attr_dict(mem->اوصاف, other->اوصاف,
			other->module ? other->module->design : خالی_اشاریہ, design);
	memories[name] = mem;
	واپس mem;
}

RTLIL::Process *RTLIL::Module::جوڑو_Process(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = نیا RTLIL::Process;
	proc->name = name;
	جوڑو(proc);
	واپس proc;
}

RTLIL::Process *RTLIL::Module::جوڑو_Process(RTLIL::IdString name, ثابت RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : خالی_اشاریہ;
	اگر (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> اوصاف;
			RTLIL::copy_attr_dict(اوصاف, obj->اوصاف, src_design, design);
			obj->اوصاف = std::move(اوصاف);
		});
		برائے (خودکار *ہمآہنگ : proc->syncs)
			برائے (خودکار &mwa : ہمآہنگ->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	جوڑو(proc);
	واپس proc;
}

#تعریف_کرو DEF_طریقہ(_func, y_سائز, _type) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_چوڑائی] = sig_a.سائز();       \
		cell->parameters[ID::Y_چوڑائی] = sig_y.سائز();       \
		cell->سیٹ_پورٹ(ID::A, sig_a);                        \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                        \
		cell->سیٹ_src_attribute(src);                       \
		واپس cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, y_سائز);    \
		جوڑو ## _func(name, sig_a, sig_y, is_signed, src);   \
		واپس sig_y;                                       \
	}
DEF_طریقہ(Not,        sig_a.سائز(), ID($not))
DEF_طریقہ(Pos,        sig_a.سائز(), ID($pos))
DEF_طریقہ(Neg,        sig_a.سائز(), ID($neg))
DEF_طریقہ(ReduceAnd,  1, ID($reduce_and))
DEF_طریقہ(ReduceOr,   1, ID($reduce_or))
DEF_طریقہ(ReduceXor,  1, ID($reduce_xor))
DEF_طریقہ(ReduceXnor, 1, ID($reduce_xnor))
DEF_طریقہ(ReduceBool, 1, ID($reduce_bool))
DEF_طریقہ(LogicNot,   1, ID($logic_not))
#تعریف_منسوخ DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, y_سائز, _type) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقی /* is_signed */, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);           \
		cell->parameters[ID::چوڑائی_2] = sig_a.سائز();         \
		cell->سیٹ_پورٹ(ID::A, sig_a);                        \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                        \
		cell->سیٹ_src_attribute(src);                       \
		واپس cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, y_سائز);    \
		جوڑو ## _func(name, sig_a, sig_y, is_signed, src);   \
		واپس sig_y;                                       \
	}
DEF_طریقہ(Buf,     sig_a.سائز(), ID($buf))
DEF_طریقہ(Barrier, sig_a.سائز(), ID($barrier))
#تعریف_منسوخ DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, y_سائز, _type) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_چوڑائی] = sig_a.سائز();       \
		cell->parameters[ID::B_چوڑائی] = sig_b.سائز();       \
		cell->parameters[ID::Y_چوڑائی] = sig_y.سائز();       \
		cell->سیٹ_پورٹ(ID::A, sig_a);                        \
		cell->سیٹ_پورٹ(ID::B, sig_b);                        \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                        \
		cell->سیٹ_src_attribute(src);                       \
		واپس cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, y_سائز);         \
		جوڑو ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		واپس sig_y;                                            \
	}
DEF_طریقہ(And,      max(sig_a.سائز(), sig_b.سائز()), ID($and))
DEF_طریقہ(Or,       max(sig_a.سائز(), sig_b.سائز()), ID($or))
DEF_طریقہ(Xor,      max(sig_a.سائز(), sig_b.سائز()), ID($xor))
DEF_طریقہ(Xnor,     max(sig_a.سائز(), sig_b.سائز()), ID($xnor))
DEF_طریقہ(سرکاؤ_2,    sig_a.سائز(), ID($سرکاؤ))
DEF_طریقہ(Lt,       1, ID($lt))
DEF_طریقہ(Le,       1, ID($le))
DEF_طریقہ(Eq,       1, ID($eq))
DEF_طریقہ(Ne,       1, ID($ne))
DEF_طریقہ(Eqx,      1, ID($eqx))
DEF_طریقہ(Nex,      1, ID($nex))
DEF_طریقہ(Ge,       1, ID($ge))
DEF_طریقہ(Gt,       1, ID($gt))
DEF_طریقہ(جوڑو_2,      max(sig_a.سائز(), sig_b.سائز()), ID($جوڑو))
DEF_طریقہ(Sub,      max(sig_a.سائز(), sig_b.سائز()), ID($sub))
DEF_طریقہ(Mul,      max(sig_a.سائز(), sig_b.سائز()), ID($mul))
DEF_طریقہ(Div,      max(sig_a.سائز(), sig_b.سائز()), ID($div))
DEF_طریقہ(Mod,      max(sig_a.سائز(), sig_b.سائز()), ID($mod))
DEF_طریقہ(DivFloor, max(sig_a.سائز(), sig_b.سائز()), ID($divfloor))
DEF_طریقہ(ModFloor, max(sig_a.سائز(), sig_b.سائز()), ID($modfloor))
DEF_طریقہ(LogicAnd, 1, ID($logic_and))
DEF_طریقہ(LogicOr,  1, ID($logic_or))
#تعریف_منسوخ DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, y_سائز, _type) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = جھوٹ;             \
		cell->parameters[ID::A_چوڑائی] = sig_a.سائز();       \
		cell->parameters[ID::B_چوڑائی] = sig_b.سائز();       \
		cell->parameters[ID::Y_چوڑائی] = sig_y.سائز();       \
		cell->سیٹ_پورٹ(ID::A, sig_a);                        \
		cell->سیٹ_پورٹ(ID::B, sig_b);                        \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                        \
		cell->سیٹ_src_attribute(src);                       \
		واپس cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, y_سائز);         \
		جوڑو ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		واپس sig_y;                                            \
	}
DEF_طریقہ(Shl,      sig_a.سائز(), ID($shl))
DEF_طریقہ(Shr,      sig_a.سائز(), ID($shr))
DEF_طریقہ(Sshl,     sig_a.سائز(), ID($sshl))
DEF_طریقہ(Sshr,     sig_a.سائز(), ID($sshr))
#تعریف_منسوخ DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, y_سائز, _type) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = جھوٹ;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_چوڑائی] = sig_a.سائز();       \
		cell->parameters[ID::B_چوڑائی] = sig_b.سائز();       \
		cell->parameters[ID::Y_چوڑائی] = sig_y.سائز();       \
		cell->سیٹ_پورٹ(ID::A, sig_a);                        \
		cell->سیٹ_پورٹ(ID::B, sig_b);                        \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                        \
		cell->سیٹ_src_attribute(src);                       \
		واپس cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, y_سائز);         \
		جوڑو ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		واپس sig_y;                                            \
	}
DEF_طریقہ(Shiftx,      sig_a.سائز(), ID($shiftx))
#تعریف_منسوخ DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);                 \
		cell->parameters[ID::چوڑائی_2] = sig_a.سائز();               \
		اگر (_pmux) cell->parameters[ID::S_چوڑائی] = sig_s.سائز();  \
		cell->سیٹ_پورٹ(ID::A, sig_a);                              \
		cell->سیٹ_پورٹ(ID::B, sig_b);                              \
		cell->سیٹ_پورٹ(ID::S, sig_s);                              \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                              \
		cell->سیٹ_src_attribute(src);                             \
		واپس cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, sig_a.سائز());     \
		جوڑو ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		واپس sig_y;                                             \
	}
DEF_طریقہ(Mux,      ID($mux),        0)
DEF_طریقہ(Bwmux,    ID($bwmux),      0)
DEF_طریقہ(Pmux,     ID($pmux),       1)
#تعریف_منسوخ DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);                 \
		cell->parameters[ID::چوڑائی_2] = _demux ? sig_a.سائز() : sig_y.سائز(); \
		cell->parameters[ID::S_چوڑائی] = sig_s.سائز();             \
		cell->سیٹ_پورٹ(ID::A, sig_a);                              \
		cell->سیٹ_پورٹ(ID::S, sig_s);                              \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                              \
		cell->سیٹ_src_attribute(src);                             \
		واپس cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, _demux ? sig_a.سائز() << sig_s.سائز() : sig_a.سائز() >> sig_s.سائز()); \
		جوڑو ## _func(name, sig_a, sig_s, sig_y, src);             \
		واپس sig_y;                                             \
	}
DEF_طریقہ(Bmux,     ID($bmux),       0)
DEF_طریقہ(Demux,    ID($demux),      1)
#تعریف_منسوخ DEF_طریقہ

#تعریف_کرو DEF_طریقہ(_func, _type) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);                 \
		cell->parameters[ID::چوڑائی_2] = sig_a.سائز();               \
		cell->سیٹ_پورٹ(ID::A, sig_a);                              \
		cell->سیٹ_پورٹ(ID::B, sig_b);                              \
		cell->سیٹ_پورٹ(ID::Y, sig_y);                              \
		cell->سیٹ_src_attribute(src);                             \
		واپس cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جوڑو_Wire(NEW_ID, sig_a.سائز());     \
		جوڑو ## _func(name, sig_a, sig_s, sig_y, src);             \
		واپس sig_y;                                             \
	}
DEF_طریقہ(Bweqx,    ID($bweqx))
#تعریف_منسوخ DEF_طریقہ

#تعریف_کرو DEF_طریقہ_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig2, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);         \
		cell->سیٹ_پورٹ("\\" #_P1, sig1);                   \
		cell->سیٹ_پورٹ("\\" #_P2, sig2);                   \
		cell->سیٹ_src_attribute(src);                     \
		واپس cell;                                      \
	} \
	RTLIL::Sig_ثنائی_ہندسہ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت std::string &src) { \
		RTLIL::Sig_ثنائی_ہندسہ sig2 = جوڑو_Wire(NEW_ID);             \
		جوڑو ## _func(name, sig1, sig2, src);              \
		واپس sig2;                                      \
	}
#تعریف_کرو DEF_طریقہ_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig2, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig3, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);         \
		cell->سیٹ_پورٹ("\\" #_P1, sig1);                   \
		cell->سیٹ_پورٹ("\\" #_P2, sig2);                   \
		cell->سیٹ_پورٹ("\\" #_P3, sig3);                   \
		cell->سیٹ_src_attribute(src);                     \
		واپس cell;                                      \
	} \
	RTLIL::Sig_ثنائی_ہندسہ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig2, ثابت std::string &src) { \
		RTLIL::Sig_ثنائی_ہندسہ sig3 = جوڑو_Wire(NEW_ID);             \
		جوڑو ## _func(name, sig1, sig2, sig3, src);        \
		واپس sig3;                                      \
	}
#تعریف_کرو DEF_طریقہ_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::جوڑو ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig2, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig3, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig4, ثابت std::string &src) { \
		RTLIL::Cell *cell = جوڑو_Cell(name, _type);         \
		cell->سیٹ_پورٹ("\\" #_P1, sig1);                   \
		cell->سیٹ_پورٹ("\\" #_P2, sig2);                   \
		cell->سیٹ_پورٹ("\\" #_P3, sig3);                   \
		cell->سیٹ_پورٹ("\\" #_P4, sig4);                   \
		cell->سیٹ_src_attribute(src);                     \
		واپس cell;                                      \
	} \
	RTLIL::Sig_ثنائی_ہندسہ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig2, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig3, ثابت std::string &src) { \
		RTLIL::Sig_ثنائی_ہندسہ sig4 = جوڑو_Wire(NEW_ID);             \
		جوڑو ## _func(name, sig1, sig2, sig3, sig4, src);  \
		واپس sig4;                                      \
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
		واپس cell;                                      \
	} \
	RTLIL::Sig_ثنائی_ہندسہ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig1, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig2, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig3, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig4, ثابت std::string &src) { \
		RTLIL::Sig_ثنائی_ہندسہ sig5 = جوڑو_Wire(NEW_ID);                  \
		جوڑو ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		واپس sig5;                                           \
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
#تعریف_منسوخ DEF_طریقہ_2
#تعریف_منسوخ DEF_طریقہ_3
#تعریف_منسوخ DEF_طریقہ_4
#تعریف_منسوخ DEF_طریقہ_5

RTLIL::Cell* RTLIL::Module::جوڑو_Pow(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقی a_signed, منطقی b_signed, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($قوت));
	cell->parameters[ID::A_SIGNED] = a_signed;
	cell->parameters[ID::B_SIGNED] = b_signed;
	cell->parameters[ID::A_چوڑائی] = sig_a.سائز();
	cell->parameters[ID::B_چوڑائی] = sig_b.سائز();
	cell->parameters[ID::Y_چوڑائی] = sig_y.سائز();
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::B, sig_b);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Fa(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_x, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($fa));
	cell->parameters[ID::چوڑائی_2] = sig_a.سائز();
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::B, sig_b);
	cell->سیٹ_پورٹ(ID::C, sig_c);
	cell->سیٹ_پورٹ(ID::X, sig_x);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_قاش(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const انحراف, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($قاش));
	cell->parameters[ID::A_چوڑائی] = sig_a.سائز();
	cell->parameters[ID::Y_چوڑائی] = sig_y.سائز();
	cell->parameters[ID::انحراف_2] = انحراف;
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Concat(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($concat));
	cell->parameters[ID::A_چوڑائی] = sig_a.سائز();
	cell->parameters[ID::B_چوڑائی] = sig_b.سائز();
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::B, sig_b);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Lut(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::چوڑائی_2] = sig_a.سائز();
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Tribuf(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($tribuf));
	cell->parameters[ID::چوڑائی_2] = sig_a.سائز();
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Assert(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($أكد));
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Assume(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($assume));
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_زندہ(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($زندہ));
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Fair(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($fair));
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Cover(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($cover));
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Equiv(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($equiv));
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::B, sig_b);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr, ثابت RTLIL::SigSpec &sig_q, منطقی سیٹ_polarity, منطقی clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($sr));
	cell->parameters[ID::سیٹ_POLARITY] = سیٹ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::CLR, sig_clr);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Ff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($ff));
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, منطقی en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($dffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dffsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, منطقی سیٹ_polarity, منطقی clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($dffsr));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::سیٹ_POLARITY] = سیٹ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::CLR, sig_clr);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dffsre(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, منطقی en_polarity, منطقی سیٹ_polarity, منطقی clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($dffsre));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::سیٹ_POLARITY] = سیٹ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::CLR, sig_clr);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Adff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_قدر, منطقی clk_polarity, منطقی arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_قدر] = arst_قدر;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::ARST, sig_arst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Adffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_قدر, منطقی clk_polarity, منطقی en_polarity, منطقی arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_قدر] = arst_قدر;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::ARST, sig_arst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Aldff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقی clk_polarity, منطقی aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($aldff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::ALOAD, sig_aload);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::AD, sig_ad);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Aldffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقی clk_polarity, منطقی en_polarity, منطقی aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($aldffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::ALOAD, sig_aload);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::AD, sig_ad);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sdff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_قدر, منطقی clk_polarity, منطقی srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_قدر] = srst_قدر;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::SRST, sig_srst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sdffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_قدر, منطقی clk_polarity, منطقی en_polarity, منطقی srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_قدر] = srst_قدر;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::SRST, sig_srst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sdffce(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_قدر, منطقی clk_polarity, منطقی en_polarity, منطقی srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_قدر] = srst_قدر;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::CLK, sig_clk);
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::SRST, sig_srst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Adlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_قدر, منطقی en_polarity, منطقی arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_قدر] = arst_قدر;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::ARST, sig_arst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dlatchsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی en_polarity, منطقی سیٹ_polarity, منطقی clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($dlatchsr));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::سیٹ_POLARITY] = سیٹ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::EN, sig_en);
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::CLR, sig_clr);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		ثابت RTLIL::SigSpec &sig_q, منطقی سیٹ_polarity, منطقی clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_SR_%c%c_", سیٹ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::S, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::R, sig_clr);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Ff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($_FF_));
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, منطقی en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dffsr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, منطقی سیٹ_polarity, منطقی clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', سیٹ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::S, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::R, sig_clr);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dffsre_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, منطقی en_polarity, منطقی سیٹ_polarity, منطقی clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', سیٹ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::S, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::R, sig_clr);
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Adff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقی arst_قدر, منطقی clk_polarity, منطقی arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_قدر ? '1' : '0'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::R, sig_arst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Adffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقی arst_قدر, منطقی clk_polarity, منطقی en_polarity, منطقی arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_قدر ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::R, sig_arst);
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Aldff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقی clk_polarity, منطقی aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::L, sig_aload);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::AD, sig_ad);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Aldffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقی clk_polarity, منطقی en_polarity, منطقی aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::L, sig_aload);
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::AD, sig_ad);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sdff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقی srst_قدر, منطقی clk_polarity, منطقی srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_قدر ? '1' : '0'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::R, sig_srst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sdffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقی srst_قدر, منطقی clk_polarity, منطقی en_polarity, منطقی srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_قدر ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::R, sig_srst);
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Sdffce_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقی srst_قدر, منطقی clk_polarity, منطقی en_polarity, منطقی srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_قدر ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::C, sig_clk);
	cell->سیٹ_پورٹ(ID::R, sig_srst);
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Adlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقی arst_قدر, منطقی en_polarity, منطقی arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_قدر ? '1' : '0'));
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::R, sig_arst);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Dlatchsr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی en_polarity, منطقی سیٹ_polarity, منطقی clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', سیٹ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->سیٹ_پورٹ(ID::E, sig_en);
	cell->سیٹ_پورٹ(ID::S, sig_سیٹ);
	cell->سیٹ_پورٹ(ID::R, sig_clr);
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Anyinit(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($anyinit));
	cell->parameters[ID::چوڑائی_2] = sig_q.سائز();
	cell->سیٹ_پورٹ(ID::D, sig_d);
	cell->سیٹ_پورٹ(ID::Q, sig_q);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, صحیح چوڑائی, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, چوڑائی);
	Cell *cell = جوڑو_Cell(name, ID($anyconst));
	cell->سیٹ_Param(ID::چوڑائی_2, چوڑائی);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	واپس sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, صحیح چوڑائی, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, چوڑائی);
	Cell *cell = جوڑو_Cell(name, ID($anyseq));
	cell->سیٹ_Param(ID::چوڑائی_2, چوڑائی);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	واپس sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, صحیح چوڑائی, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, چوڑائی);
	Cell *cell = جوڑو_Cell(name, ID($allconst));
	cell->سیٹ_Param(ID::چوڑائی_2, چوڑائی);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	واپس sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, صحیح چوڑائی, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, چوڑائی);
	Cell *cell = جوڑو_Cell(name, ID($allseq));
	cell->سیٹ_Param(ID::چوڑائی_2, چوڑائی);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	واپس sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID);
	Cell *cell = جوڑو_Cell(name, ID($initstate));
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	واپس sig;
}

RTLIL::SigSpec RTLIL::Module::سیٹ_لیبل_2(RTLIL::IdString name, ثابت std::string &لیبل, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, sig_a.سائز());
	Cell *cell = جوڑو_Cell(name, ID($سیٹ_لیبل));
	cell->parameters[ID::چوڑائی_2] = sig_a.سائز();
	cell->parameters[ID::لیبل_2] = لیبل;
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_s);
	cell->سیٹ_پورٹ(ID::CLR, sig_c);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	واپس sig;
}

RTLIL::Cell* RTLIL::Module::جوڑو_سیٹ_لیبل(RTLIL::IdString name, ثابت std::string &لیبل, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	Cell *cell = جوڑو_Cell(name, ID($سیٹ_لیبل));
	cell->parameters[ID::چوڑائی_2] = sig_a.سائز();
	cell->parameters[ID::لیبل_2] = لیبل;
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_s);
	cell->سیٹ_پورٹ(ID::CLR, sig_c);
	cell->سیٹ_پورٹ(ID::Y, sig_y);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::SigSpec RTLIL::Module::Get_لیبل(RTLIL::IdString name, ثابت std::string &لیبل, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, sig_a.سائز());
	Cell *cell = جوڑو_Cell(name, ID($get_لیبل));
	cell->parameters[ID::چوڑائی_2] = sig_a.سائز();
	cell->parameters[ID::لیبل_2] = لیبل;
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	واپس sig;
}

RTLIL::Cell* RTLIL::Module::جوڑو_Overwrite_لیبل(RTLIL::IdString name, ثابت std::string &لیبل, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src)
{
	RTLIL::Cell *cell = جوڑو_Cell(name, ID($overwrite_لیبل));
	cell->parameters[ID::چوڑائی_2] = sig_a.سائز();
	cell->parameters[ID::لیبل_2] = لیبل;
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::سیٹ_2, sig_s);
	cell->سیٹ_پورٹ(ID::CLR, sig_c);
	cell->سیٹ_src_attribute(src);
	واپس cell;
}

RTLIL::SigSpec RTLIL::Module::Original_لیبل(RTLIL::IdString name, ثابت std::string &لیبل, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, sig_a.سائز());
	Cell *cell = جوڑو_Cell(name, ID($original_لیبل));
	cell->parameters[ID::چوڑائی_2] = sig_a.سائز();
	cell->parameters[ID::لیبل_2] = لیبل;
	cell->سیٹ_پورٹ(ID::A, sig_a);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	واپس sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جوڑو_Wire(NEW_ID, sig_e.سائز());
	Cell *cell = جوڑو_Cell(name, ID($future_ff));
	cell->parameters[ID::چوڑائی_2] = sig_e.سائز();
	cell->سیٹ_پورٹ(ID::A, sig_e);
	cell->سیٹ_پورٹ(ID::Y, sig);
	cell->سیٹ_src_attribute(src);
	واپس sig;
}

std::string RTLIL::Module::مقصد_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(یہ), design, جھوٹ, سچ, جھوٹ, RTLIL_BACKEND::Dump_منوال::Readable);
	واپس f.str();
}

RTLIL::Wire::Wire()
{
	ساکن غیرعلامتی صحیح hashidx_گنتی = 123456789;
	hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
	hashidx_ = hashidx_گنتی;

	module = خالی_اشاریہ;
	چوڑائی = 1;
	آغاز_انحراف = 0;
	پورٹ_id = 0;
	پورٹ_داخلہ = جھوٹ;
	پورٹ_output = جھوٹ;
	upto = جھوٹ;
	is_signed = جھوٹ;

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->داخل_کرو(std::pair<غیرعلامتی صحیح, RTLIL::Wire*>(hashidx_, یہ));
#اگر_ختم
}

RTLIL::Wire::~Wire()
{
#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#اگر_ختم
}

std::string RTLIL::Wire::مقصد_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", یہ, design(), RTLIL_BACKEND::Dump_منوال::Readable);
	واپس f.str();
}

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
ساکن std::map<غیرعلامتی صحیح, RTLIL::Wire*> all_wires;
std::map<غیرعلامتی صحیح, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(خالی)
{
	واپس &all_wires;
}
#اگر_ختم

RTLIL::حافظہ::حافظہ()
{
	ساکن غیرعلامتی صحیح hashidx_گنتی = 123456789;
	hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
	hashidx_ = hashidx_گنتی;

	چوڑائی = 1;
	آغاز_انحراف = 0;
	سائز = 0;
#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::حافظہ::get_all_memorys()->داخل_کرو(std::pair<غیرعلامتی صحیح, RTLIL::حافظہ*>(hashidx_, یہ));
#اگر_ختم
}

std::string RTLIL::حافظہ::مقصد_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_حافظہ(f, "", یہ, design(), RTLIL_BACKEND::Dump_منوال::Readable);
	واپس f.str();
}

RTLIL::Process::Process() : module(خالی_اشاریہ)
{
	ساکن غیرعلامتی صحیح hashidx_گنتی = 123456789;
	hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
	hashidx_ = hashidx_گنتی;
}

std::string RTLIL::Process::مقصد_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", یہ, design(), RTLIL_BACKEND::Dump_منوال::Readable);
	واپس f.str();
}

RTLIL::Cell::Cell() : module(خالی_اشاریہ)
{
	ساکن غیرعلامتی صحیح hashidx_گنتی = 123456789;
	hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
	hashidx_ = hashidx_گنتی;

	// log("#memtrace# %p\n", this);
	memhasher();

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->داخل_کرو(std::pair<غیرعلامتی صحیح, RTLIL::Cell*>(hashidx_, یہ));
#اگر_ختم
}

RTLIL::Cell::~Cell()
{
#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#اگر_ختم
}

std::string RTLIL::Cell::مقصد_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", یہ, design(), RTLIL_BACKEND::Dump_منوال::Readable);
	واپس f.str();
}

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
ساکن std::map<غیرعلامتی صحیح, RTLIL::Cell*> all_cells;
std::map<غیرعلامتی صحیح, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(خالی)
{
	واپس &all_cells;
}
#اگر_ختم

منطقی RTLIL::Cell::has_پورٹ(RTLIL::IdString portname) ثابت
{
	واپس connections_.گنتی(portname) != 0;
}

// bufnorm

ثابت RTLIL::SigSpec &RTLIL::Cell::get_پورٹ(RTLIL::IdString portname) ثابت
{
	واپس connections_.at(portname);
}

ثابت dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() ثابت
{
	واپس connections_;
}

منطقی RTLIL::Cell::معلوم() ثابت
{
	اگر (yosys_celltypes.cell_معلوم(type))
		واپس سچ;
	اگر (module && module->design && module->design->module(type))
		واپس سچ;
	واپس جھوٹ;
}

منطقی RTLIL::Cell::داخلہ(IdString portname) ثابت
{
	اگر (yosys_celltypes.cell_معلوم(type))
		واپس yosys_celltypes.cell_داخلہ(type.ref(), portname);
	اگر (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : خالی_اشاریہ;
		واپس w && w->پورٹ_داخلہ;
	}
	واپس جھوٹ;
}

منطقی RTLIL::Cell::output(RTLIL::IdString portname) ثابت
{
	اگر (yosys_celltypes.cell_معلوم(type))
		واپس yosys_celltypes.cell_output(type, portname);
	اگر (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : خالی_اشاریہ;
		واپس w && w->پورٹ_output;
	}
	واپس جھوٹ;
}

RTLIL::پورٹ_Dir RTLIL::Cell::پورٹ_dir(RTLIL::IdString portname) ثابت
{
	اگر (yosys_celltypes.cell_معلوم(type.ref()))
		واپس yosys_celltypes.cell_پورٹ_dir(type, portname);
	اگر (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		اگر (m == خالی_اشاریہ)
			واپس پورٹ_Dir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		اگر (w == خالی_اشاریہ)
			واپس پورٹ_Dir::PD_UNKNOWN;
		واپس پورٹ_Dir(w->پورٹ_داخلہ + w->پورٹ_output * 2);
	}
	واپس پورٹ_Dir::PD_UNKNOWN;
}

منطقی RTLIL::Cell::hasParam(RTLIL::IdString paramname) ثابت
{
	واپس parameters.گنتی(paramname) != 0;
}

خالی RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

خالی RTLIL::Cell::سیٹ_Param(RTLIL::IdString paramname, RTLIL::Const قدر)
{
	parameters[paramname] = std::move(قدر);
}

ثابت RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ثابت
{
	ثابت خودکار &it = parameters.ڈھونڈو(paramname);
	اگر (it != parameters.انتہا())
		واپس it->دوسرا;
	اگر (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		اگر (m)
			واپس m->parameter_default_اقدار.at(paramname);
	}
	پھینکو std::برآمد_کا_range("Cell::getParam()");
}

خالی RTLIL::Cell::ترتیب_دو()
{
	connections_.ترتیب_دو(ترتیب_دو_by_id_str(module->twines()));
	parameters.ترتیب_دو(ترتیب_دو_by_id_str(module->twines()));
	اوصاف.ترتیب_دو(ترتیب_دو_by_id_str(module->twines()));
}

خالی RTLIL::Cell::check()
{
#اگر_غیر_تعریف_شدہ NDEBUG
	اندرونی_Cell_Checker checker(تہی, یہ);
	checker.check();
#اگر_ختم
}

خالی RTLIL::Cell::fixup_parameters(منطقی سیٹ_a_signed, منطقی سیٹ_b_signed)
{
	اگر (type.is_عوامی() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		واپس;

	اگر (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::چوڑائی_2] = Get_سائز(connections_[ID::Y]);
		اگر (type.اندر(ID($pmux), ID($bmux)))
			parameters[ID::S_چوڑائی] = Get_سائز(connections_[ID::S]);
		check();
		واپس;
	}

	اگر (type == ID($demux)) {
		parameters[ID::چوڑائی_2] = Get_سائز(connections_[ID::A]);
		parameters[ID::S_چوڑائی] = Get_سائز(connections_[ID::S]);
		check();
		واپس;
	}

	اگر (type == ID($lut) || type == ID($sop)) {
		parameters[ID::چوڑائی_2] = Get_سائز(connections_[ID::A]);
		واپس;
	}

	اگر (type == ID($fa)) {
		parameters[ID::چوڑائی_2] = Get_سائز(connections_[ID::Y]);
		واپس;
	}

	اگر (type == ID($lcu)) {
		parameters[ID::چوڑائی_2] = Get_سائز(connections_[ID::CO]);
		واپس;
	}

	اگر (type == ID($macc_v2)) {
		parameters[ID::Y_چوڑائی] = Get_سائز(connections_[ID::Y]);
		واپس;
	}

	منطقی signedness_ab = !type.اندر(ID($قاش), ID($concat), ID($macc));

	اگر (connections_.گنتی(ID::A)) {
		اگر (signedness_ab) {
			اگر (سیٹ_a_signed)
				parameters[ID::A_SIGNED] = سچ;
			ورنہ اگر (parameters.گنتی(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = جھوٹ;
		}
		parameters[ID::A_چوڑائی] = Get_سائز(connections_[ID::A]);
	}

	اگر (connections_.گنتی(ID::B)) {
		اگر (signedness_ab) {
			اگر (سیٹ_b_signed)
				parameters[ID::B_SIGNED] = سچ;
			ورنہ اگر (parameters.گنتی(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = جھوٹ;
		}
		parameters[ID::B_چوڑائی] = Get_سائز(connections_[ID::B]);
	}

	اگر (connections_.گنتی(ID::Y) && type != ID($concat))
		parameters[ID::Y_چوڑائی] = Get_سائز(connections_[ID::Y]);

	اگر (connections_.گنتی(ID::Q))
		parameters[ID::چوڑائی_2] = Get_سائز(connections_[ID::Q]);

	check();
}

منطقی RTLIL::Cell::has_keep_attr() ثابت {
	واپس get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

منطقی RTLIL::Cell::has_memid() ثابت
{
	واپس type.اندر(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

منطقی RTLIL::Cell::is_mem_cell() ثابت
{
	واپس type.اندر(ID($mem), ID($mem_v2)) || has_memid();
}

منطقی RTLIL::Cell::is_builtin_ff() ثابت {
	واپس StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ)
{
	wire = ثنائی_ہندسہ.wire;
	انحراف = 0;
	اگر (wire == تہی)
		ڈیٹا = {ثنائی_ہندسہ.ڈیٹا};
	ورنہ
		انحراف = ثنائی_ہندسہ.انحراف;
	چوڑائی = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::نکالو_2(صحیح انحراف, صحیح لمبائی_2) ثابت
{
	log_assert(انحراف >= 0);
	log_assert(لمبائی_2 >= 0);
	log_assert(انحراف + لمبائی_2 <= چوڑائی);
	RTLIL::SigChunk ret;
	اگر (wire) {
		ret.wire = wire;
		ret.انحراف = یہ->انحراف + انحراف;
		ret.چوڑائی = لمبائی_2;
	} ورنہ {
		برائے (صحیح i = 0; i < لمبائی_2; i++)
			ret.ڈیٹا.ڈالو_عقب(ڈیٹا[انحراف+i]);
		ret.چوڑائی = لمبائی_2;
	}
	واپس ret;
}

RTLIL::Sig_ثنائی_ہندسہ RTLIL::SigChunk::عامل[](صحیح انحراف) ثابت
{
	log_assert(انحراف >= 0);
	log_assert(انحراف < چوڑائی);
	RTLIL::Sig_ثنائی_ہندسہ ret;
	اگر (wire) {
		ret.wire = wire;
		ret.انحراف = یہ->انحراف + انحراف;
	} ورنہ {
		ret.ڈیٹا = ڈیٹا[انحراف];
	}
	واپس ret;
}

منطقی RTLIL::SigChunk::عامل <(ثابت RTLIL::SigChunk &other) ثابت
{
	اگر (wire && other.wire)
		اگر (wire->name != other.wire->name)
			واپس wire->name < other.wire->name;

	اگر (wire != other.wire)
		واپس wire < other.wire;

	اگر (انحراف != other.انحراف)
		واپس انحراف < other.انحراف;

	اگر (چوڑائی != other.چوڑائی)
		واپس چوڑائی < other.چوڑائی;

	واپس ڈیٹا < other.ڈیٹا;
}

منطقی RTLIL::SigChunk::عامل ==(ثابت RTLIL::SigChunk &other) ثابت
{
	واپس wire == other.wire && چوڑائی == other.چوڑائی && انحراف == other.انحراف && ڈیٹا == other.ڈیٹا;
}

منطقی RTLIL::SigChunk::عامل !=(ثابت RTLIL::SigChunk &other) ثابت
{
	اگر (*یہ == other)
		واپس جھوٹ;
	واپس سچ;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_خالی_خانے_بٹ();
	log_assert(parts.سائز() > 0);
	خودکار ie = parts.begin();
	خودکار it = ie + parts.سائز() - 1;
	جب (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::Const &قدر)
{
	اگر (Get_سائز(قدر) != 0) {
		rep_ = CHUNK;
		نیا (&chunk_) RTLIL::SigChunk(قدر);
	} ورنہ {
		init_خالی_خانے_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&قدر)
{
	اگر (Get_سائز(قدر) != 0) {
		rep_ = CHUNK;
		نیا (&chunk_) RTLIL::SigChunk(قدر);
	} ورنہ {
		init_خالی_خانے_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::SigChunk &chunk)
{
	اگر (chunk.چوڑائی != 0) {
		rep_ = CHUNK;
		نیا (&chunk_) RTLIL::SigChunk(chunk);
	} ورنہ {
		init_خالی_خانے_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	اگر (chunk.چوڑائی != 0) {
		rep_ = CHUNK;
		نیا (&chunk_) RTLIL::SigChunk(chunk);
	} ورنہ {
		init_خالی_خانے_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	اگر (wire->چوڑائی != 0) {
		rep_ = CHUNK;
		نیا (&chunk_) RTLIL::SigChunk(wire);
	} ورنہ {
		init_خالی_خانے_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, صحیح انحراف, صحیح چوڑائی)
{
	اگر (چوڑائی != 0) {
		rep_ = CHUNK;
		نیا (&chunk_) RTLIL::SigChunk(wire, انحراف, چوڑائی);
	} ورنہ {
		init_خالی_خانے_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::string &str)
{
	اگر (str.سائز() != 0) {
		rep_ = CHUNK;
		نیا (&chunk_) RTLIL::SigChunk(str);
	} ورنہ {
		init_خالی_خانے_بٹ();
	}
	check();
}

RTLIL::SigSpec::SigSpec(صحیح val, صحیح چوڑائی)
{
	اگر (چوڑائی != 0) {
		rep_ = CHUNK;
		نیا (&chunk_) RTLIL::SigChunk(val, چوڑائی);
	} ورنہ
		init_خالی_خانے_بٹ();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State ثنائی_ہندسہ, صحیح چوڑائی)
{
	اگر (چوڑائی != 0) {
		rep_ = CHUNK;
		نیا (&chunk_) RTLIL::SigChunk(ثنائی_ہندسہ, چوڑائی);
	} ورنہ
		init_خالی_خانے_بٹ();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ, صحیح چوڑائی)
{
	اگر (چوڑائی != 0) {
		اگر (ثنائی_ہندسہ.wire == تہی) {
			rep_ = CHUNK;
			نیا (&chunk_) RTLIL::SigChunk(ثنائی_ہندسہ.ڈیٹا, چوڑائی);
		} ورنہ اگر (چوڑائی == 1) {
			rep_ = CHUNK;
			نیا (&chunk_) RTLIL::SigChunk(ثنائی_ہندسہ);
		} ورنہ {
			init_خالی_خانے_بٹ();
			بٹ_2.reserve(چوڑائی);
			برائے (صحیح i = 0; i < چوڑائی; i++)
				بٹ_2.ڈالو_عقب(ثنائی_ہندسہ);
		}
	} ورنہ
		init_خالی_خانے_بٹ();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::سمتیہ<RTLIL::SigChunk> &chunks)
{
	init_خالی_خانے_بٹ();
	برائے (ثابت خودکار &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> &بٹ)
{
	init_خالی_خانے_بٹ();
	برائے (ثابت خودکار &ثنائی_ہندسہ : بٹ)
		append(ثنائی_ہندسہ);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &بٹ)
{
	init_خالی_خانے_بٹ();
	برائے (ثابت خودکار &ثنائی_ہندسہ : بٹ)
		append(ثنائی_ہندسہ);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::سیٹ<RTLIL::Sig_ثنائی_ہندسہ> &بٹ)
{
	init_خالی_خانے_بٹ();
	برائے (ثابت خودکار &ثنائی_ہندسہ : بٹ)
		append(ثنائی_ہندسہ);
	check();
}

RTLIL::SigSpec::SigSpec(منطقی ثنائی_ہندسہ)
{
	rep_ = CHUNK;
	نیا (&chunk_) RTLIL::SigChunk(ثنائی_ہندسہ ? RTLIL::S1 : RTLIL::S0);
	check();
}

خالی RTLIL::SigSpec::Chunks::const_iterator::اگلا_chunk_بٹ()
{
	صحیح بٹ_سائز = Get_سائز(spec.بٹ_2);
	اگر (ثنائی_ہندسہ_اشاریہ >= بٹ_سائز)
		واپس;
	صحیح i = ثنائی_ہندسہ_اشاریہ;
	ثابت Sig_ثنائی_ہندسہ &ثنائی_ہندسہ = spec.بٹ_2[i++];
	chunk.wire = ثنائی_ہندسہ.wire;
	chunk.ڈیٹا.clear();
	اگر (ثنائی_ہندسہ.is_wire()) {
		chunk.انحراف = ثنائی_ہندسہ.انحراف;
		جب (i < بٹ_سائز && spec.بٹ_2[i].wire == ثنائی_ہندسہ.wire &&
			spec.بٹ_2[i].انحراف == ثنائی_ہندسہ.انحراف + i - ثنائی_ہندسہ_اشاریہ)
			++i;
	} ورنہ {
		chunk.انحراف = 0;
		chunk.ڈیٹا.ڈالو_عقب(ثنائی_ہندسہ.ڈیٹا);
		جب (i < بٹ_سائز && !spec.بٹ_2[i].is_wire()) {
			chunk.ڈیٹا.ڈالو_عقب(spec.بٹ_2[i].ڈیٹا);
			++i;
		}
	}
	chunk.چوڑائی = i - ثنائی_ہندسہ_اشاریہ;
}

خالی RTLIL::SigSpec::unpack()
{
	اگر (rep_ == بٹ_3)
		واپس;

	std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ;
	بٹ.reserve(chunk_.چوڑائی);
	برائے (صحیح i = 0; i < chunk_.چوڑائی; i++)
		بٹ.emplace_عقب(chunk_, i);

	chunk_.~SigChunk();
	rep_ = بٹ_3;
	نیا (&بٹ_2) std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>(std::move(بٹ));
}

خالی RTLIL::SigSpec::try_repack()
{
	اگر (rep_ != بٹ_3)
		واپس;

	صحیح بٹ_سائز = Get_سائز(بٹ_2);
	اگر (بٹ_سائز == 0)
		واپس;
	اگر (بٹ_2[0].is_wire()) {
		برائے (صحیح i = 1; i < بٹ_سائز; i++)
			اگر (بٹ_2[0].wire != بٹ_2[i].wire || بٹ_2[0].انحراف + i != بٹ_2[i].انحراف)
				واپس;
		SigChunk chunk(بٹ_2[0].wire, بٹ_2[0].انحراف, بٹ_سائز);
		بٹ_2.~سمتیہ();
		rep_ = CHUNK;
		نیا (&chunk_) SigChunk(std::move(chunk));
		واپس;
	}
	std::سمتیہ<RTLIL::State> بٹ;
	بٹ.reserve(بٹ_سائز);
	بٹ.ڈالو_عقب(بٹ_2[0].ڈیٹا);
	برائے (صحیح i = 1; i < بٹ_سائز; i++) {
		اگر (بٹ_2[i].is_wire())
			واپس;
		بٹ.ڈالو_عقب(بٹ_2[i].ڈیٹا);
	}
	بٹ_2.~سمتیہ();
	rep_ = CHUNK;
	نیا (&chunk_) SigChunk(std::move(بٹ));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ثابت
{
	Hasher h;
	برائے (خودکار &c : chunks())
		اگر (c.wire == تہی) {
			برائے (خودکار &v : c.ڈیٹا)
				h.eat(v);
		} ورنہ {
			h.eat(c.wire->name.ref());
			h.eat(c.انحراف);
			h.eat(c.چوڑائی);
		}
	Hasher::hash_t نتیجہ = h.yield();
	اگر (نتیجہ == 0)
		نتیجہ = 1;
	hash_.سیٹ(نتیجہ);
	واپس نتیجہ;
}

خالی RTLIL::SigSpec::ترتیب_دو()
{
	unpack();
	std::ترتیب_دو(بٹ_2.begin(), بٹ_2.انتہا());
	hash_.clear();
	try_repack();
}

خالی RTLIL::SigSpec::ترتیب_دو_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::سمتیہ<Sig_ثنائی_ہندسہ> unique_بٹ = بٹ_2;
	std::ترتیب_دو(unique_بٹ.begin(), unique_بٹ.انتہا());
	خودکار آخری = std::unique(unique_بٹ.begin(), unique_بٹ.انتہا());
	unique_بٹ.erase(آخری, unique_بٹ.انتہا());

	*یہ = unique_بٹ;
	hash_.clear();
	try_repack();
}

خالی RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &نمونہ, ثابت RTLIL::SigSpec &with)
{
	replace(نمونہ, with, یہ);
}

خالی RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &نمونہ, ثابت RTLIL::SigSpec &with, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != تہی);
	log_assert(سائز() == other->سائز());
	log_assert(نمونہ.سائز() == with.سائز());

	dict<RTLIL::Sig_ثنائی_ہندسہ, صحیح> نمونہ_مقصد_with;
	صحیح نمونہ_سائز = نمونہ.سائز();
	برائے (صحیح i = 0; i < نمونہ_سائز; i++) {
		Sig_ثنائی_ہندسہ نمونہ_ثنائی_ہندسہ = نمونہ[i];
		اگر (نمونہ_ثنائی_ہندسہ.wire != تہی) {
			نمونہ_مقصد_with.emplace(نمونہ_ثنائی_ہندسہ, i);
		}
	}

	صحیح this_سائز = سائز();
	منطقی other_modified = جھوٹ;
	برائے (صحیح j = 0; j < this_سائز; j++) {
		خودکار it = نمونہ_مقصد_with.ڈھونڈو((*یہ)[j]);
		اگر (it != نمونہ_مقصد_with.انتہا()) {
			other_modified = سچ;
			(*other)[j] = with[it->دوسرا];
		}
	}

	اگر (other_modified)
		other->try_repack();
	other->check();
}

خالی RTLIL::SigSpec::replace(ثابت dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قواعد)
{
	replace(قواعد, یہ);
}

خالی RTLIL::SigSpec::replace(ثابت dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قواعد, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != تہی);
	log_assert(سائز() == other->سائز());

	اگر (قواعد.خالی_خانے()) واپس;

	صحیح this_سائز = سائز();
	منطقی other_modified = جھوٹ;
	برائے (صحیح i = 0; i < this_سائز; i++) {
		خودکار it = قواعد.ڈھونڈو((*یہ)[i]);
		اگر (it != قواعد.انتہا()) {
			other_modified = سچ;
			(*other)[i] = it->دوسرا;
		}
	}

	اگر (other_modified)
		other->try_repack();
	other->check();
}

خالی RTLIL::SigSpec::replace(ثابت std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قواعد)
{
	replace(قواعد, یہ);
}

خالی RTLIL::SigSpec::replace(ثابت std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قواعد, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != تہی);
	log_assert(سائز() == other->سائز());

	اگر (قواعد.خالی_خانے()) واپس;

	صحیح this_سائز = سائز();
	منطقی other_modified = جھوٹ;
	برائے (صحیح i = 0; i < this_سائز; i++) {
		خودکار it = قواعد.ڈھونڈو((*یہ)[i]);
		اگر (it != قواعد.انتہا()) {
			other_modified = سچ;
			(*other)[i] = it->دوسرا;
		}
	}

	اگر (other_modified)
		other->try_repack();
	other->check();
}

خالی RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &نمونہ)
{
	remove2(نمونہ, تہی);
}

خالی RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &نمونہ, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *یہ;
	tmp.remove2(نمونہ, other);
}

خالی RTLIL::SigSpec::remove2(ثابت RTLIL::SigSpec &نمونہ, RTLIL::SigSpec *other)
{
	unpack();
	اگر (other != تہی) {
		log_assert(سائز() == other->سائز());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<Sig_ثنائی_ہندسہ> نمونہ_بٹ;
	نمونہ_بٹ.reserve(نمونہ.سائز());
	برائے (خودکار &ثنائی_ہندسہ : نمونہ)
		اگر (ثنائی_ہندسہ.wire != تہی)
			نمونہ_بٹ.داخل_کرو(ثنائی_ہندسہ);

	// Compact in-place to avoid O(n^2) erase operations
	سائز_t write_idx = 0;
	برائے (سائز_t read_idx = 0; read_idx < بٹ_2.سائز(); read_idx++)
	{
		اگر (!(بٹ_2[read_idx].wire != تہی && نمونہ_بٹ.گنتی(بٹ_2[read_idx]))) {
			اگر (write_idx != read_idx) {
				بٹ_2[write_idx] = بٹ_2[read_idx];
				اگر (other != تہی)
					other->بٹ_2[write_idx] = other->بٹ_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقی modified = (write_idx < بٹ_2.سائز());
	اگر (modified) {
		بٹ_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	اگر (other != تہی && modified) {
		other->بٹ_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

خالی RTLIL::SigSpec::remove(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ)
{
	remove2(نمونہ, تہی);
}

خالی RTLIL::SigSpec::remove(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *یہ;
	tmp.remove2(نمونہ, other);
}

خالی RTLIL::SigSpec::remove2(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, RTLIL::SigSpec *other)
{
	unpack();

	اگر (other != تہی) {
		log_assert(سائز() == other->سائز());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	سائز_t write_idx = 0;
	برائے (سائز_t read_idx = 0; read_idx < بٹ_2.سائز(); read_idx++) {
		اگر (!(بٹ_2[read_idx].wire != تہی && نمونہ.گنتی(بٹ_2[read_idx]))) {
			اگر (write_idx != read_idx) {
				بٹ_2[write_idx] = بٹ_2[read_idx];
				اگر (other != تہی)
					other->بٹ_2[write_idx] = other->بٹ_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقی modified = (write_idx < بٹ_2.سائز());
	اگر (modified) {
		بٹ_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	اگر (other != تہی && modified) {
		other->بٹ_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

خالی RTLIL::SigSpec::remove2(ثابت std::سیٹ<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, RTLIL::SigSpec *other)
{
	unpack();

	اگر (other != تہی) {
		log_assert(سائز() == other->سائز());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	سائز_t write_idx = 0;
	برائے (سائز_t read_idx = 0; read_idx < بٹ_2.سائز(); read_idx++) {
		اگر (!(بٹ_2[read_idx].wire != تہی && نمونہ.گنتی(بٹ_2[read_idx]))) {
			اگر (write_idx != read_idx) {
				بٹ_2[write_idx] = بٹ_2[read_idx];
				اگر (other != تہی)
					other->بٹ_2[write_idx] = other->بٹ_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقی modified = (write_idx < بٹ_2.سائز());
	اگر (modified) {
		بٹ_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	اگر (other != تہی && modified) {
		other->بٹ_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

خالی RTLIL::SigSpec::remove2(ثابت pool<RTLIL::Wire*> &نمونہ, RTLIL::SigSpec *other)
{
	unpack();

	اگر (other != تہی) {
		log_assert(سائز() == other->سائز());
		other->unpack();
	}

	منطقی modified = جھوٹ;
	منطقی other_modified = جھوٹ;
	برائے (صحیح i = Get_سائز(بٹ_2) - 1; i >= 0; i--) {
		اگر (بٹ_2[i].wire != تہی && نمونہ.گنتی(بٹ_2[i].wire)) {
			modified = سچ;
			بٹ_2.erase(بٹ_2.begin() + i);
			اگر (other != تہی) {
				other_modified = سچ;
				other->بٹ_2.erase(other->بٹ_2.begin() + i);
			}
		}
	}

	اگر (modified) {
		hash_.clear();
		try_repack();
	}
	اگر (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::نکالو_2(ثابت RTLIL::SigSpec &نمونہ, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == تہی || سائز() == other->سائز());

	RTLIL::SigSpec ret;
	std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ_مطابقت = مقصد_sigbit_سمتیہ();

	برائے (خودکار& نمونہ_chunk : نمونہ.chunks()) {
		اگر (other) {
			std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ_other = other->مقصد_sigbit_سمتیہ();
			صحیح i = 0;
			برائے (ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : بٹ_مطابقت) {
				اگر (ثنائی_ہندسہ.wire &&
					ثنائی_ہندسہ.wire == نمونہ_chunk.wire &&
					ثنائی_ہندسہ.انحراف >= نمونہ_chunk.انحراف &&
					ثنائی_ہندسہ.انحراف < نمونہ_chunk.انحراف + نمونہ_chunk.چوڑائی)
					ret.append(بٹ_other[i]);
				++i;
			}
		} ورنہ {
			برائے (ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : بٹ_مطابقت)
				اگر (ثنائی_ہندسہ.wire &&
					ثنائی_ہندسہ.wire == نمونہ_chunk.wire &&
					ثنائی_ہندسہ.انحراف >= نمونہ_chunk.انحراف &&
					ثنائی_ہندسہ.انحراف < نمونہ_chunk.انحراف + نمونہ_chunk.چوڑائی)
					ret.append(ثنائی_ہندسہ);
		}
	}

	ret.check();
	واپس ret;
}

RTLIL::SigSpec RTLIL::SigSpec::نکالو_2(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == تہی || سائز() == other->سائز());

	std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ_مطابقت = مقصد_sigbit_سمتیہ();
	RTLIL::SigSpec ret;

	اگر (other) {
		std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ_other = other->مقصد_sigbit_سمتیہ();
		صحیح i = 0;
		برائے (ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : بٹ_مطابقت) {
			اگر (ثنائی_ہندسہ.wire && نمونہ.گنتی(ثنائی_ہندسہ))
				ret.append(بٹ_other[i]);
			++i;
		}
	} ورنہ {
		برائے (ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : بٹ_مطابقت)
			اگر (ثنائی_ہندسہ.wire && نمونہ.گنتی(ثنائی_ہندسہ))
				ret.append(ثنائی_ہندسہ);
	}

	ret.check();
	واپس ret;
}

خالی RTLIL::SigSpec::replace(صحیح انحراف, ثابت RTLIL::SigSpec &with)
{
	اگر (with.سائز() == 0)
		واپس;

	unpack();

	log_assert(انحراف >= 0);
	log_assert(with.سائز() >= 0);
	log_assert(انحراف+with.سائز() <= سائز());

	صحیح i = 0;
	برائے (ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : with.بٹ()) {
		بٹ_2.at(انحراف + i) = ثنائی_ہندسہ;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

خالی RTLIL::SigSpec::remove_const()
{
	اگر (rep_ == CHUNK)
	{
		اگر (chunk_.wire == تہی) {
			chunk_.~SigChunk();
			init_خالی_خانے_بٹ();
			hash_.clear();
		}
	}
	ورنہ
	{
		std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> new_بٹ;
		new_بٹ.reserve(بٹ_2.سائز());
		برائے (خودکار &ثنائی_ہندسہ : بٹ_2)
			اگر (ثنائی_ہندسہ.wire != تہی)
				new_بٹ.ڈالو_عقب(ثنائی_ہندسہ);
		اگر (Get_سائز(new_بٹ) != Get_سائز(بٹ_2)) {
			بٹ_2.تبادلہ(new_بٹ);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

خالی RTLIL::SigSpec::remove(صحیح انحراف, صحیح لمبائی_2)
{
	اگر (لمبائی_2 == 0)
		واپس;

	unpack();

	log_assert(انحراف >= 0);
	log_assert(لمبائی_2 >= 0);
	log_assert(انحراف + لمبائی_2 <= سائز());

	بٹ_2.erase(بٹ_2.begin() + انحراف, بٹ_2.begin() + انحراف + لمبائی_2);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::نکالو_2(صحیح انحراف, صحیح لمبائی_2) ثابت
{
	log_assert(انحراف >= 0);
	log_assert(لمبائی_2 >= 0);
	log_assert(انحراف + لمبائی_2 <= سائز());

	std::سمتیہ<Sig_ثنائی_ہندسہ> extracted;
	Sig_ثنائی_ہندسہ پہلا;
	منطقی is_packing = سچ;
	برائے (صحیح i = انحراف; i < انحراف + لمبائی_2; i++) {
		منطقی was_packing_before = is_packing;
		Sig_ثنائی_ہندسہ ثنائی_ہندسہ = (*یہ)[i];
		اگر (i == انحراف) {
			پہلا = ثنائی_ہندسہ;
			اگر (!ثنائی_ہندسہ.wire)
				is_packing = جھوٹ;
		} ورنہ {
			اگر (ثنائی_ہندسہ.wire != پہلا.wire)
				is_packing = جھوٹ;
			اگر (ثنائی_ہندسہ.wire)
				اگر (ثنائی_ہندسہ.انحراف != پہلا.انحراف + (i - انحراف))
					is_packing = جھوٹ;
		}
		اگر (was_packing_before && !is_packing)
			برائے (صحیح j = انحراف; j < i; j++)
				extracted.ڈالو_عقب((*یہ)[j]);
		اگر (!is_packing)
			extracted.ڈالو_عقب((*یہ)[i]);
	}
	اگر (is_packing)
		واپس SigChunk(پہلا.wire, پہلا.انحراف, لمبائی_2);

	واپس extracted;
}

خالی RTLIL::SigSpec::rewrite_wires(std::function<خالی(RTLIL::Wire*& wire)> rewrite)
{
	اگر (rep_ == CHUNK) {
		اگر (chunk_.wire != خالی_اشاریہ)
			rewrite(chunk_.wire);
		واپس;
	}

	std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> new_بٹ;
	برائے (ثابت RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		اگر (c.wire != خالی_اشاریہ)
			rewrite(c.wire);
		برائے (صحیح i = 0; i < c.چوڑائی; i++)
			new_بٹ.emplace_عقب(c, i);
	}
	بٹ_2 = std::move(new_بٹ);
	hash_.clear();
}

خالی RTLIL::SigSpec::append(ثابت RTLIL::SigSpec &اشارہ)
{
	اگر (اشارہ.سائز() == 0)
		واپس;

	اگر (سائز() == 0) {
		*یہ = اشارہ;
		واپس;
	}

	hash_.clear();
	اگر (rep_ == CHUNK && اشارہ.rep_ == CHUNK && chunk_.wire == اشارہ.chunk_.wire) {
		اگر (chunk_.wire == تہی) {
			chunk_.ڈیٹا.داخل_کرو(chunk_.ڈیٹا.انتہا(), اشارہ.chunk_.ڈیٹا.begin(), اشارہ.chunk_.ڈیٹا.انتہا());
			chunk_.چوڑائی = Get_سائز(chunk_.ڈیٹا);
			واپس;
		}
		اگر (chunk_.انحراف + chunk_.چوڑائی == اشارہ.chunk_.انحراف) {
			chunk_.چوڑائی += اشارہ.chunk_.چوڑائی;
			واپس;
		}
	}

	unpack();
	برائے (ثابت Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : اشارہ.بٹ())
		بٹ_2.ڈالو_عقب(ثنائی_ہندسہ);
	check();
}

خالی RTLIL::SigSpec::append(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ)
{
	hash_.clear();

	اگر (سائز() == 0) {
		destroy();
		rep_ = CHUNK;
		نیا (&chunk_) RTLIL::SigChunk(ثنائی_ہندسہ);
		واپس;
	}

	اگر (rep_ == CHUNK && chunk_.wire == ثنائی_ہندسہ.wire) {
		اگر (chunk_.wire == تہی) {
			chunk_.ڈیٹا.ڈالو_عقب(ثنائی_ہندسہ.ڈیٹا);
			chunk_.چوڑائی++;
			واپس;
		}
		اگر (chunk_.انحراف + chunk_.چوڑائی == ثنائی_ہندسہ.انحراف) {
			chunk_.چوڑائی++;
			واپس;
		}
	}

	unpack();

	بٹ_2.ڈالو_عقب(ثنائی_ہندسہ);
	check();
}

خالی RTLIL::SigSpec::extend_u0(صحیح چوڑائی, منطقی is_signed)
{
	اگر (سائز() > چوڑائی)
		remove(چوڑائی, سائز() - چوڑائی);

	اگر (سائز() < چوڑائی) {
		RTLIL::Sig_ثنائی_ہندسہ padding = سائز() > 0 ? (*یہ)[سائز() - 1] : RTLIL::State::Sx;
		اگر (!is_signed)
			padding = RTLIL::State::S0;
		جب (سائز() < چوڑائی)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(صحیح عدد) ثابت
{
	RTLIL::SigSpec sig;
	برائے (صحیح i = 0; i < عدد; i++)
		sig.append(*یہ);
	واپس sig;
}

#اگر_غیر_تعریف_شدہ NDEBUG
خالی RTLIL::SigSpec::check(ثابت Module *mod) ثابت
{
	اگر (rep_ == CHUNK)
	{
		log_assert(chunk_.چوڑائی != 0);
		اگر (chunk_.wire == تہی) {
			log_assert(chunk_.انحراف == 0);
			log_assert(chunk_.ڈیٹا.سائز() == (سائز_t)chunk_.چوڑائی);
		} ورنہ {
			log_assert(chunk_.انحراف >= 0);
			log_assert(chunk_.چوڑائی >= 0);
			log_assert(chunk_.انحراف + chunk_.چوڑائی <= chunk_.wire->چوڑائی);
			log_assert(chunk_.ڈیٹا.سائز() == 0);
			اگر (mod != خالی_اشاریہ)
				log_assert(chunk_.wire->module == mod);
		}
	}
	ورنہ اگر (سائز() <= 64)
	{
		اگر (mod != خالی_اشاریہ) {
			برائے (ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ : بٹ_2)
				اگر (ثنائی_ہندسہ.wire != خالی_اشاریہ)
					log_assert(ثنائی_ہندسہ.wire->module == mod);
		}
	}
}
#اگر_ختم

منطقی RTLIL::SigSpec::عامل <(ثابت RTLIL::SigSpec &other) ثابت
{
	اگر (یہ == &other)
		واپس جھوٹ;

	اگر (سائز() != other.سائز())
		واپس سائز() < other.سائز();

	خودکار other_it = other.chunks().begin();
	برائے (ثابت SigChunk &c : chunks()) {
		اگر (c != *other_it)
			واپس c < *other_it;
		++other_it;
	}

	واپس جھوٹ;
}

منطقی RTLIL::SigSpec::عامل ==(ثابت RTLIL::SigSpec &other) ثابت
{
	اگر (یہ == &other)
		واپس سچ;

	اگر (سائز() != other.سائز())
		واپس جھوٹ;

	خودکار other_it = other.chunks().begin();
	برائے (ثابت SigChunk &c : chunks()) {
		اگر (c != *other_it)
			واپس جھوٹ;
		++other_it;
	}

	واپس سچ;
}

منطقی RTLIL::SigSpec::is_wire() ثابت
{
	Chunks cs = chunks();
	خودکار it = cs.begin();
	اگر (it == cs.انتہا())
		واپس جھوٹ;
	ثابت RTLIL::SigChunk &chunk = *it;
	واپس chunk.wire && chunk.wire->چوڑائی == سائز() && ++it == cs.انتہا();
}

منطقی RTLIL::SigSpec::is_chunk() ثابت
{
	Chunks cs = chunks();
	خودکار it = cs.begin();
	اگر (it == cs.انتہا())
		واپس جھوٹ;
	واپس ++it == cs.انتہا();
}

منطقی RTLIL::SigSpec::معلوم_driver() ثابت
{
	برائے (خودکار &chunk : chunks())
		اگر (chunk.is_wire() && !chunk.wire->معلوم_driver())
			واپس جھوٹ;
	واپس سچ;
}

منطقی RTLIL::SigSpec::is_fully_const() ثابت
{
	برائے (خودکار &chunk : chunks())
		اگر (chunk.چوڑائی > 0 && chunk.wire != تہی)
			واپس جھوٹ;
	واپس سچ;
}

منطقی RTLIL::SigSpec::is_fully_zero() ثابت
{
	برائے (خودکار &chunk : chunks()) {
		اگر (chunk.چوڑائی > 0 && chunk.wire != تہی)
			واپس جھوٹ;
		برائے (RTLIL::State d : chunk.ڈیٹا)
			اگر (d != RTLIL::State::S0)
				واپس جھوٹ;
	}
	واپس سچ;
}

منطقی RTLIL::SigSpec::is_fully_ones() ثابت
{
	برائے (خودکار &chunk : chunks()) {
		اگر (chunk.چوڑائی > 0 && chunk.wire != تہی)
			واپس جھوٹ;
		برائے (RTLIL::State d : chunk.ڈیٹا)
			اگر (d != RTLIL::State::S1)
				واپس جھوٹ;
	}
	واپس سچ;
}

منطقی RTLIL::SigSpec::is_fully_def() ثابت
{
	برائے (خودکار &chunk : chunks()) {
		اگر (chunk.چوڑائی > 0 && chunk.wire != تہی)
			واپس جھوٹ;
		برائے (RTLIL::State d : chunk.ڈیٹا)
			اگر (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				واپس جھوٹ;
	}
	واپس سچ;
}

منطقی RTLIL::SigSpec::is_fully_undef() ثابت
{
	برائے (خودکار &chunk : chunks()) {
		اگر (chunk.چوڑائی > 0 && chunk.wire != تہی)
			واپس جھوٹ;
		برائے (RTLIL::State d : chunk.ڈیٹا)
			اگر (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				واپس جھوٹ;
	}
	واپس سچ;
}

منطقی RTLIL::SigSpec::has_const() ثابت
{
	برائے (خودکار &chunk : chunks())
		اگر (chunk.چوڑائی > 0 && chunk.wire == تہی)
			واپس سچ;
	واپس جھوٹ;
}

منطقی RTLIL::SigSpec::has_const(State state) ثابت
{
	برائے (خودکار &chunk : chunks())
		اگر (chunk.چوڑائی > 0 && chunk.wire == تہی && std::ڈھونڈو(chunk.ڈیٹا.begin(), chunk.ڈیٹا.انتہا(), state) != chunk.ڈیٹا.انتہا())
			واپس سچ;
	واپس جھوٹ;
}


منطقی RTLIL::SigSpec::has_marked_بٹ() ثابت
{
	برائے (خودکار &chunk : chunks())
		اگر (chunk.چوڑائی > 0 && chunk.wire == تہی) {
			برائے (RTLIL::State d : chunk.ڈیٹا)
				اگر (d == RTLIL::State::Sm)
					واپس سچ;
		}
	واپس جھوٹ;
}

منطقی RTLIL::SigSpec::is_onehot(صحیح *pos) ثابت
{
	اگر (std::optional<RTLIL::Const> c = try_as_const())
		واپس c->is_onehot(pos);
	واپس جھوٹ;
}

منطقی RTLIL::SigSpec::as_bool() ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_قدر());
	واپس c->as_bool();
}

صحیح RTLIL::SigSpec::as_int(منطقی is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_قدر());
	واپس c->as_int(is_signed);
}

منطقی RTLIL::SigSpec::convertible_مقصد_int(منطقی is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	اگر (!c.has_قدر())
		واپس جھوٹ;
	واپس c->convertible_مقصد_int(is_signed);
}

std::optional<صحیح> RTLIL::SigSpec::try_as_int(منطقی is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	اگر (!c.has_قدر())
		واپس std::nullopt;
	واپس c->try_as_int(is_signed);
}

صحیح RTLIL::SigSpec::as_int_saturating(منطقی is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_قدر());
	واپس c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ثابت
{
	std::string str;
	str.reserve(سائز());
	std::سمتیہ<RTLIL::SigChunk> chunks = *یہ;
	برائے (سائز_t i = chunks.سائز(); i > 0; i--) {
		ثابت RTLIL::SigChunk &chunk = chunks[i-1];
		اگر (chunk.wire != تہی)
			str.append(chunk.چوڑائی, '?');
		ورنہ
			str += RTLIL::Const(chunk.ڈیٹا).as_string();
	}
	واپس str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ثابت
{
	Chunks cs = chunks();
	خودکار it = cs.begin();
	اگر (it == cs.انتہا())
		واپس RTLIL::Const();
	SigChunk chunk = *it;
	اگر (chunk.wire != تہی || ++it != cs.انتہا())
		واپس std::nullopt;
	واپس RTLIL::Const(std::move(chunk.ڈیٹا));
}

RTLIL::Const RTLIL::SigSpec::as_const() ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_قدر());
	واپس *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ثابت
{
	Chunks cs = chunks();
	خودکار it = cs.begin();
	log_assert(it != cs.انتہا());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.انتہا() && chunk.wire && chunk.wire->چوڑائی == سائز());
	واپس chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ثابت
{
	Chunks cs = chunks();
	خودکار it = cs.begin();
	log_assert(it != cs.انتہا());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.انتہا());
	واپس chunk;
}

RTLIL::Sig_ثنائی_ہندسہ RTLIL::SigSpec::as_ثنائی_ہندسہ() ثابت
{
	واپس RTLIL::Sig_ثنائی_ہندسہ(*یہ);
}

منطقی RTLIL::SigSpec::مطابقت(ثابت حرف* نمونہ) ثابت
{
	صحیح نمونہ_len = لڑی_لمبائی(نمونہ);
	log_assert(نمونہ_len == سائز());

	برائے (صحیح i = 0; i < نمونہ_len; ++i) {
		حرف ch = نمونہ[i];
		اگر (ch == ' ')
			جاری;
		RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ = (*یہ)[نمونہ_len - 1 - i];
		اگر (ch == '*') {
			اگر (ثنائی_ہندسہ != State::Sz && ثنائی_ہندسہ != State::Sx)
				واپس جھوٹ;
			جاری;
		}
		اگر (ch == '0') {
			اگر (ثنائی_ہندسہ != State::S0)
				واپس جھوٹ;
		} ورنہ
		اگر (ch == '1') {
			اگر (ثنائی_ہندسہ != State::S1)
				واپس جھوٹ;
		} ورنہ
			log_abort();
	}

	واپس سچ;
}

std::سیٹ<RTLIL::Sig_ثنائی_ہندسہ> RTLIL::SigSpec::مقصد_sigbit_سیٹ() ثابت
{
	std::سیٹ<RTLIL::Sig_ثنائی_ہندسہ> sigbits;
	برائے (خودکار &c : chunks())
		برائے (صحیح i = 0; i < c.چوڑائی; i++)
			sigbits.داخل_کرو(RTLIL::Sig_ثنائی_ہندسہ(c, i));
	واپس sigbits;
}

pool<RTLIL::Sig_ثنائی_ہندسہ> RTLIL::SigSpec::مقصد_sigbit_pool() ثابت
{
	pool<RTLIL::Sig_ثنائی_ہندسہ> sigbits;
	sigbits.reserve(سائز());
	برائے (خودکار &c : chunks())
		برائے (صحیح i = 0; i < c.چوڑائی; i++)
			sigbits.داخل_کرو(RTLIL::Sig_ثنائی_ہندسہ(c, i));
	واپس sigbits;
}

std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> RTLIL::SigSpec::مقصد_sigbit_سمتیہ() ثابت
{
	std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> نتیجہ;
	نتیجہ.reserve(سائز());
	برائے (Sig_ثنائی_ہندسہ ثنائی_ہندسہ : *یہ)
		نتیجہ.ڈالو_عقب(ثنائی_ہندسہ);
	واپس نتیجہ;
}

std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> RTLIL::SigSpec::مقصد_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت
{
	صحیح this_سائز = سائز();
	log_assert(this_سائز == other.سائز());

	std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> new_map;
	برائے (صحیح i = 0; i < this_سائز; i++)
		new_map[(*یہ)[i]] = other[i];

	واپس new_map;
}

dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> RTLIL::SigSpec::مقصد_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت
{
	صحیح this_سائز = سائز();
	log_assert(this_سائز == other.سائز());

	dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> new_map;
	new_map.reserve(this_سائز);
	برائے (صحیح i = 0; i < this_سائز; i++)
		new_map[(*یہ)[i]] = other[i];

	واپس new_map;
}

ساکن خالی sigspec_parse_بانٹو(std::سمتیہ<std::string> &علامات, ثابت std::string &متن, حرف sep)
{
	سائز_t آغاز = 0, انتہا = 0;
	جب ((انتہا = متن.ڈھونڈو(sep, آغاز)) != std::string::npos) {
		علامات.ڈالو_عقب(متن.substr(آغاز, انتہا - آغاز));
		آغاز = انتہا + 1;
	}
	علامات.ڈالو_عقب(متن.substr(آغاز));
}

ساکن RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ثابت std::string &netname)
{
	واپس module->wire(netname);
}

منطقی RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::سمتیہ<std::string> علامات;
	sigspec_parse_بانٹو(علامات, str, ',');

	sig = RTLIL::SigSpec();
	برائے (صحیح tokidx = صحیح(علامات.سائز())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = علامات[tokidx];
		std::string indices;

		اگر (netname.سائز() == 0)
			جاری;

		اگر (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			خودکار ast = p.const2ast(netname);
			اگر (ast == خالی_اشاریہ)
				واپس جھوٹ;
			sig.append(RTLIL::Const(ast->بٹ));
			جاری;
		}

		اگر (module == تہی)
			واپس جھوٹ;

		اگر (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		اگر (sigspec_parse_wire(module, netname) == خالی_اشاریہ) {
			سائز_t indices_pos = netname.سائز()-1;
			اگر (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				جب (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				اگر (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					جب (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				اگر (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		اگر (wire == خالی_اشاریہ)
			واپس جھوٹ;

		اگر (!indices.خالی_خانے()) {
			std::سمتیہ<std::string> اشاریہ_علامات;
			sigspec_parse_بانٹو(اشاریہ_علامات, indices.substr(1, indices.سائز()-2), ':');
			اگر (اشاریہ_علامات.سائز() == 1) {
				صحیح a = atoi(اشاریہ_علامات.at(0).c_str());
				اگر (a < 0 || a >= wire->چوڑائی)
					واپس جھوٹ;
				sig.append(RTLIL::SigSpec(wire, a));
			} ورنہ {
				صحیح a = atoi(اشاریہ_علامات.at(0).c_str());
				صحیح b = atoi(اشاریہ_علامات.at(1).c_str());
				اگر (a > b) {
					صحیح tmp = a;
					a = b, b = tmp;
				}
				اگر (a < 0 || a >= wire->چوڑائی)
					واپس جھوٹ;
				اگر (b < 0 || b >= wire->چوڑائی)
					واپس جھوٹ;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} ورنہ
			sig.append(wire);
	}

	واپس سچ;
}

منطقی RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	اگر (str.خالی_خانے() || str[0] != '@')
		واپس parse(sig, module, str);

	IdString sel_name = design->twines().ڈھونڈو(RTLIL::escape_id(str.substr(1)));
	اگر (sel_name == IdString::Null || design->selection_vars.گنتی(sel_name) == 0)
		واپس جھوٹ;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	برائے (خودکار &it : module->wires_)
		اگر (sel.selected_member(module->name, it.دوسرا->name))
			sig.append(it.دوسرا);

	واپس سچ;
}

منطقی RTLIL::SigSpec::parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	اگر (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.سائز());
		واپس سچ;
	}

	اگر (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.سائز());
		واپس سچ;
	}

	اگر (lhs.is_chunk()) {
		حرف *p = (حرف*)str.c_str(), *endptr;
		طویل صحیح val = strtol(p, &endptr, 10);
		اگر (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.سائز());
			واپس سچ;
		}
	}

	اگر (!parse(sig, module, str))
		واپس جھوٹ;
	اگر (sig.سائز() > lhs.سائز())
		sig.remove(lhs.سائز(), sig.سائز() - lhs.سائز());
	واپس سچ;
}

RTLIL::SigSpec::عامل std::سمتیہ<RTLIL::SigChunk>() ثابت
{
	std::سمتیہ<RTLIL::SigChunk> نتیجہ;
	برائے (ثابت RTLIL::SigChunk &c : chunks())
		نتیجہ.ڈالو_عقب(c);
	واپس نتیجہ;
}

RTLIL::Case_قاعدہ::~Case_قاعدہ()
{
	برائے (خودکار it = switches.begin(); it != switches.انتہا(); it++)
		حذف *it;
}

منطقی RTLIL::Case_قاعدہ::خالی_خانے() ثابت
{
	واپس actions.خالی_خانے() && switches.خالی_خانے();
}

RTLIL::Case_قاعدہ *RTLIL::Case_قاعدہ::clone() ثابت
{
	RTLIL::Case_قاعدہ *new_caserule = نیا RTLIL::Case_قاعدہ;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->اوصاف = اوصاف;
	برائے (خودکار &it : switches)
		new_caserule->switches.ڈالو_عقب(it->clone());
	واپس new_caserule;
}

RTLIL::Switch_قاعدہ::~Switch_قاعدہ()
{
	برائے (خودکار it = cases.begin(); it != cases.انتہا(); it++)
		حذف *it;
}

منطقی RTLIL::Switch_قاعدہ::خالی_خانے() ثابت
{
	واپس cases.خالی_خانے();
}

RTLIL::Switch_قاعدہ *RTLIL::Switch_قاعدہ::clone() ثابت
{
	RTLIL::Switch_قاعدہ *new_switchrule = نیا RTLIL::Switch_قاعدہ;
	new_switchrule->اشارہ = اشارہ;
	new_switchrule->اوصاف = اوصاف;
	برائے (خودکار &it : cases)
		new_switchrule->cases.ڈالو_عقب(it->clone());
	واپس new_switchrule;

}

RTLIL::Sync_قاعدہ *RTLIL::Sync_قاعدہ::clone() ثابت
{
	RTLIL::Sync_قاعدہ *new_syncrule = نیا RTLIL::Sync_قاعدہ;
	new_syncrule->type = type;
	new_syncrule->اشارہ = اشارہ;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	واپس new_syncrule;
}

RTLIL::Process::~Process()
{
	برائے (خودکار it = syncs.begin(); it != syncs.انتہا(); it++)
		حذف *it;
}

RTLIL::Process *RTLIL::Process::clone() ثابت
{
	RTLIL::Process *new_proc = نیا RTLIL::Process;

	new_proc->اوصاف = اوصاف;

	RTLIL::Case_قاعدہ *rc_ptr = جڑ_case.clone();
	new_proc->جڑ_case = *rc_ptr;
	rc_ptr->switches.clear();
	حذف rc_ptr;

	برائے (خودکار &it : syncs)
		new_proc->syncs.ڈالو_عقب(it->clone());

	واپس new_proc;
}

RTLIL::حافظہ::~حافظہ()
{
#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	RTLIL::حافظہ::get_all_memorys()->erase(hashidx_);
#اگر_ختم
}

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
ساکن std::map<غیرعلامتی صحیح, RTLIL::حافظہ*> all_memorys;
std::map<غیرعلامتی صحیح, RTLIL::حافظہ*> *RTLIL::حافظہ::get_all_memorys(خالی)
{
	واپس &all_memorys;
}
#اگر_ختم
YOSYS_NAMESPACE_انتہا
