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
		لكل (اندازه_t i = 0; i < StaticCellTypes::builder.شمېر; i++) {
			خپلکاری &cell = StaticCellTypes::builder.cells[i];
			إذا (cell.features.is_ff)
				r.ورننباسه(cell.type);
		}
		أرجع r;
	}();
	أرجع res;
}

#تعریف_کړه check(condition) log_assert(condition && "malformed Const union")

ثابت Const::bitvectype& Const::get_بټونه() ثابت {
	check(is_بټونه());
	أرجع *get_if_بټونه();
}

ثابت std::string& Const::get_str() ثابت {
	check(is_str());
	أرجع *get_if_str();
}

Const::bitvectype& Const::get_بټونه() {
	check(is_بټونه());
	أرجع *get_if_بټونه();
}

std::string& Const::get_str() {
	check(is_str());
	أرجع *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	جديد ((فراغ*)&str_) std::string(std::move(str));
	نښان = backing_نښان::string;
}

RTLIL::Const::Const(اوږد اوږد val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	حرف بایټونه[] = {
		(حرف)(val >> 24), (حرف)(val >> 16), (حرف)(val >> 8), (حرف)val
	};
	جديد ((فراغ*)&str_) std::string(بایټونه, 4);
	نښان = backing_نښان::string;
}

RTLIL::Const::Const(اوږد اوږد val, صحيح پلنوالی)
{
	log_assert(پلنوالی >= 0 && پلنوالی < RTLIL::پلنوالی_حد);
	flags = RTLIL::CONST_FLAG_NONE;
	إذا ((پلنوالی & 7) == 0) {
		جديد ((فراغ*)&str_) std::string();
		نښان = backing_نښان::string;
		std::string& str = get_str();
		صحيح بایټونه = پلنوالی >> 3;
		نښهدار حرف sign_byte = val < 0 ? -1 : 0;
		str.resize(بایټونه, sign_byte);
		بایټونه = std::min<صحيح>(بایټونه, حجم(val));
		لكل (صحيح i = 0; i < بایټونه; i++) {
			str[str.اندازه() - 1 - i] = val;
			val = val >> 8;
		}
		أرجع;
	}

	جديد ((فراغ*)&بټونه_2) bitvectype();
	نښان = backing_نښان::بټونه;
	bitvectype& bv = get_بټونه();
	bv.reserve(پلنوالی);
	لكل (صحيح i = 0; i < پلنوالی; i++) {
		bv.ټېل_شا((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State بټ, صحيح پلنوالی)
{
	log_assert(پلنوالی >= 0 && پلنوالی < RTLIL::پلنوالی_حد);
	flags = RTLIL::CONST_FLAG_NONE;
	جديد ((فراغ*)&بټونه_2) bitvectype();
	نښان = backing_نښان::بټونه;
	bitvectype& bv = get_بټونه();
	bv.reserve(پلنوالی);
	لكل (صحيح i = 0; i < پلنوالی; i++)
		bv.ټېل_شا(بټ);
}

RTLIL::Const::Const(ثابت std::ویکټور<منطقي> &بټونه)
{
	flags = RTLIL::CONST_FLAG_NONE;
	جديد ((فراغ*)&بټونه_2) bitvectype();
	نښان = backing_نښان::بټونه;
	bitvectype& bv = get_بټونه();
	bv.reserve(بټونه.اندازه());
	لكل (ثابت خپلکاری &b : بټونه)
		bv.emplace_شا(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ثابت RTLIL::Const &other) {
	نښان = other.نښان;
	flags = other.flags;
	إذا (is_str())
		جديد ((فراغ*)&str_) std::string(other.get_str());
	وإلا إذا (is_بټونه())
		جديد ((فراغ*)&بټونه_2) bitvectype(other.get_بټونه());
	وإلا
		check(خطأ);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	نښان = other.نښان;
	flags = other.flags;
	إذا (is_str())
		جديد ((فراغ*)&str_) std::string(std::move(other.get_str()));
	وإلا إذا (is_بټونه())
		جديد ((فراغ*)&بټونه_2) bitvectype(std::move(other.get_بټونه()));
	وإلا
		check(خطأ);
}

RTLIL::Const &RTLIL::Const::چلوونکی =(ثابت RTLIL::Const &other) {
	flags = other.flags;
	إذا (other.is_str()) {
		إذا (!is_str()) {
			// sketchy zone
			check(is_بټونه());
			بټونه_2.~bitvectype();
			(فراغ)جديد ((فراغ*)&str_) std::string();
		}
		نښان = other.نښان;
		get_str() = other.get_str();
	} وإلا إذا (other.is_بټونه()) {
		إذا (!is_بټونه()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(فراغ)جديد ((فراغ*)&بټونه_2) bitvectype();
		}
		نښان = other.نښان;
		get_بټونه() = other.get_بټونه();
	} وإلا {
		check(خطأ);
	}
	أرجع *هذا;
}

RTLIL::Const::~Const() {
	إذا (is_بټونه())
		بټونه_2.~bitvectype();
	وإلا إذا (is_str())
		str_.~string();
	وإلا
		check(خطأ);
}

منطقي RTLIL::Const::چلوونکی<(ثابت RTLIL::Const &other) ثابت
{
	إذا (اندازه() != other.اندازه())
		أرجع اندازه() < other.اندازه();

	لكل (صحيح i = 0; i < اندازه(); i++)
		إذا ((*هذا)[i] != other[i])
			أرجع (*هذا)[i] < other[i];

	أرجع خطأ;
}

منطقي RTLIL::Const::چلوونکی ==(ثابت RTLIL::Const &other) ثابت
{
	إذا (is_str() && other.is_str())
		أرجع get_str() == other.get_str();
	إذا (is_بټونه() && other.is_بټونه())
		أرجع get_بټونه() == other.get_بټونه();

	إذا (اندازه() != other.اندازه())
		أرجع خطأ;

	لكل (صحيح i = 0; i < اندازه(); i++)
	إذا ((*هذا)[i] != other[i])
		أرجع خطأ;

	أرجع ریښتیا;
}

منطقي RTLIL::Const::چلوونکی !=(ثابت RTLIL::Const &other) ثابت
{
	أرجع !(*هذا == other);
}

std::ویکټور<RTLIL::State>& RTLIL::Const::بټونه_دننی()
{
	bitvectorize_دننی();
	أرجع get_بټونه();
}

std::ویکټور<RTLIL::State> RTLIL::Const::منزل_بټونه() ثابت
{
	std::ویکټور<State> v;
	لكل (خپلکاری بټ : *هذا)
		v.ټېل_شا(بټ);
	أرجع v;
}

منطقي RTLIL::Const::as_bool() ثابت
{
	إذا (is_str()) {
		لكل (حرف ch : get_str())
			إذا (ch != 0)
				أرجع ریښتیا;
		أرجع خطأ;
	}

	ثابت bitvectype& bv = get_بټونه();
	لكل (اندازه_t i = 0; i < bv.اندازه(); i++)
		إذا (bv[i] == State::S1)
			أرجع ریښتیا;
	أرجع خطأ;
}

صحيح RTLIL::Const::as_int(منطقي is_signed) ثابت
{
	int32_t ret = 0;
	إذا (ثابت std::string *s = get_if_str()) {
		صحيح اندازه = Get_اندازه(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		لكل (صحيح i = std::min(4, اندازه); i > 0; i--)
			ret |= static_cast<بینښه حرف>((*s)[اندازه - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		إذا (is_signed && اندازه > 0 && اندازه < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << اندازه*8;
		أرجع ret;
	}

	ثابت bitvectype& bv = get_بټونه();
	صحيح significant_بټونه = std::min(Get_اندازه(bv), 32);
	لكل (صحيح i = 0; i < significant_بټونه; i++)
		إذا (bv[i] == State::S1)
			ret |= 1 << i;
	إذا (is_signed && significant_بټونه > 0 && significant_بټونه < 32 && bv.شا() == State::S1 )
		ret |= UINT32_MAX << significant_بټونه;
	أرجع ret;
}

منطقي RTLIL::Const::convertible_منزل_int(منطقي is_signed) ثابت
{
	خپلکاری اندازه = get_min_اندازه(is_signed);

	إذا (اندازه < 0)
		أرجع خطأ;

	// If it fits in 31 bits it is definitely convertible
	إذا (اندازه <= 31)
		أرجع ریښتیا;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	إذا (اندازه == 32) {
		إذا (is_signed)
			أرجع ریښتیا;
		أرجع (*هذا)[اندازه - 1] != State::S1;
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

		ثابت خپلکاری min_اندازه = get_min_اندازه(is_signed);
		log_assert(min_اندازه > 0);
		ثابت خپلکاری neg = (*هذا)[min_اندازه - 1];
		أرجع neg ? std::numeric_limits<صحيح>::min() : std::numeric_limits<صحيح>::max();
	}
	أرجع as_int(is_signed);
}

فراغ RTLIL::Const::نښان_bare_integer_const(ثابت std::string &ارزښت)
{
	إذا (ارزښت.تشه() || ارزښت.پیدا_کړه('\'') != std::string::npos)
		أرجع;
	اندازه_t پیل = (ارزښت[0] == '-' || ارزښت[0] == '+') ? 1 : 0;
	إذا (پیل < ارزښت.اندازه() && std::all_د(ارزښت.begin() + پیل, ارزښت.پای_ټکی(), ::که_عدد))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

صحيح RTLIL::Const::get_min_اندازه(منطقي is_signed) ثابت
{
	إذا (تشه()) أرجع 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_بټ;
	إذا (is_signed)
		leading_بټ = (شا() == RTLIL::State::Sx) ? RTLIL::State::S0 : شا();
	وإلا
		leading_بټ = RTLIL::State::S0;

	خپلکاری idx = اندازه();
	طالما (idx > 0 && (*هذا)[idx -1] == leading_بټ) {
		idx--;
	}

	// signed needs one leading bit
	إذا (is_signed && idx < اندازه()) {
		idx++;
	}
	// must be at least one bit
	أرجع (idx == 0) ? 1 : idx;
}

فراغ RTLIL::Const::compress(منطقي is_signed)
{
	خپلکاری idx = get_min_اندازه(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<صحيح> RTLIL::Const::as_int_compress(منطقي is_signed) ثابت
{
	أرجع try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ثابت حرف* any) ثابت
{
	صحيح sz = اندازه();
	std::string ret;
	ret.reserve(sz);
	لكل (صحيح i = sz - 1; i >= 0; --i)
		بدل ((*هذا)[i]) {
			حالة S0: ret.ټېل_شا('0'); اكسر;
			حالة S1: ret.ټېل_شا('1'); اكسر;
			حالة Sx: ret.ټېل_شا('x'); اكسر;
			حالة Sz: ret.ټېل_شا('z'); اكسر;
			حالة Sa: ret += any; اكسر;
			حالة Sm: ret.ټېل_شا('m'); اكسر;
		}
	أرجع ret;
}

RTLIL::Const RTLIL::Const::from_string(ثابت std::string &str)
{
	Const c;
	bitvectype& bv = c.get_بټونه();
	bv.reserve(str.اندازه());
	لكل (خپلکاری it = str.rbegin(); it != str.rend(); it++)
		بدل (*it) {
			حالة '0': bv.ټېل_شا(State::S0); اكسر;
			حالة '1': bv.ټېل_شا(State::S1); اكسر;
			حالة 'x': bv.ټېل_شا(State::Sx); اكسر;
			حالة 'z': bv.ټېل_شا(State::Sz); اكسر;
			حالة 'm': bv.ټېل_شا(State::Sm); اكسر;
			افتراضي: bv.ټېل_شا(State::Sa);
		}
	أرجع c;
}

std::string RTLIL::Const::decode_string() ثابت
{
	إذا (خپلکاری str = get_if_str())
		أرجع *str;

	ثابت bitvectype& bv = get_بټونه();
	ثابت صحيح n = Get_اندازه(bv);
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

صحيح RTLIL::Const::اندازه() ثابت {
	إذا (is_str())
		أرجع 8 * str_.اندازه();
	وإلا {
		check(is_بټونه());
		أرجع بټونه_2.اندازه();
	}
}

منطقي RTLIL::Const::تشه() ثابت {
	إذا (is_str())
		أرجع str_.تشه();
	وإلا {
		check(is_بټونه());
		أرجع بټونه_2.تشه();
	}
}

فراغ RTLIL::Const::bitvectorize_دننی() {
	إذا (نښان == backing_نښان::بټونه)
		أرجع;

	check(is_str());

	bitvectype new_بټونه;

	new_بټونه.reserve(str_.اندازه() * 8);
	لكل (صحيح i = str_.اندازه() - 1; i >= 0; i--) {
		بینښه حرف ch = str_[i];
		لكل (صحيح j = 0; j < 8; j++) {
			new_بټونه.ټېل_شا((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(فراغ)جديد ((فراغ*)&بټونه_2) bitvectype(std::move(new_بټونه));
		نښان = backing_نښان::بټونه;
	}
}

فراغ RTLIL::Const::append(ثابت RTLIL::Const &other) {
	bitvectorize_دننی();
	bitvectype& bv = get_بټونه();
	bv.ورننباسه(bv.پای_ټکی(), other.begin(), other.پای_ټکی());
}

RTLIL::State RTLIL::Const::const_iterator::چلوونکی*() ثابت {
	إذا (خپلکاری bv = پلار->get_if_بټونه())
		أرجع (*bv)[idx];

	صحيح char_idx = پلار->get_str().اندازه() - idx / 8 - 1;
	منطقي بټ = (پلار->get_str()[char_idx] & (1 << (idx % 8)));
	أرجع بټ ? State::S1 : State::S0;
}

منطقي RTLIL::Const::is_fully_zero() ثابت
{
	إذا (خپلکاری str = get_if_str()) {
		لكل (حرف ch : *str)
			إذا (ch != 0)
				أرجع خطأ;
		أرجع ریښتیا;
	}

	ثابت bitvectype& bv = get_بټونه();

	لكل (ثابت خپلکاری &بټ : bv)
		إذا (بټ != RTLIL::State::S0)
			أرجع خطأ;

	أرجع ریښتیا;
}

منطقي RTLIL::Const::is_fully_ones() ثابت
{
	إذا (خپلکاری str = get_if_str()) {
		لكل (حرف ch : *str)
			إذا (ch != (حرف)0xff)
				أرجع خطأ;
		أرجع ریښتیا;
	}

	ثابت bitvectype& bv = get_بټونه();
	لكل (ثابت خپلکاری &بټ : bv)
		إذا (بټ != RTLIL::State::S1)
			أرجع خطأ;

	أرجع ریښتیا;
}

منطقي RTLIL::Const::is_fully_def() ثابت
{
	إذا (is_str())
		أرجع ریښتیا;

	ثابت bitvectype& bv = get_بټونه();
	لكل (ثابت خپلکاری &بټ : bv)
		إذا (بټ != RTLIL::State::S0 && بټ != RTLIL::State::S1)
			أرجع خطأ;

	أرجع ریښتیا;
}

منطقي RTLIL::Const::is_fully_undef() ثابت
{
	إذا (خپلکاری str = get_if_str())
		أرجع str->تشه();

	ثابت bitvectype& bv = get_بټونه();
	لكل (ثابت خپلکاری &بټ : bv)
		إذا (بټ != RTLIL::State::Sx && بټ != RTLIL::State::Sz)
			أرجع خطأ;

	أرجع ریښتیا;
}

منطقي RTLIL::Const::is_fully_undef_x_only() ثابت
{
	إذا (خپلکاری str = get_if_str())
		أرجع str->تشه();

	ثابت bitvectype& bv = get_بټونه();
	لكل (ثابت خپلکاری &بټ : bv)
		إذا (بټ != RTLIL::State::Sx)
			أرجع خطأ;

	أرجع ریښتیا;
}

منطقي RTLIL::Const::is_onehot(صحيح *pos) ثابت
{
	منطقي وموندل = خطأ;
	صحيح اندازه = Get_اندازه(*هذا);
	لكل (صحيح i = 0; i < اندازه; i++) {
		State بټ = (*هذا)[i];
		إذا (بټ != RTLIL::State::S0 && بټ != RTLIL::State::S1)
			أرجع خطأ;
		إذا (بټ == RTLIL::State::S1) {
			إذا (وموندل)
				أرجع خطأ;
			إذا (pos)
				*pos = i;
			وموندل = ریښتیا;
		}
	}
	أرجع وموندل;
}

Hasher RTLIL::Const::hash_into(Hasher h) ثابت
{
	إذا (خپلکاری str = get_if_str())
		أرجع hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ثابت bitvectype &bv = get_بټونه();
	صحيح اندازه = Get_اندازه(bv);
	std::string packed;
	صحيح packed_اندازه = (اندازه + 7) >> 3;
	packed.resize(packed_اندازه, 0);
	لكل (صحيح bi = 0; bi < packed_اندازه; ++bi) {
		حرف ch = 0;
		صحيح پای_ټکی = std::min((bi + 1)*8, اندازه);
		لكل (صحيح i = bi*8; i < پای_ټکی; ++i) {
			RTLIL::State b = bv[i];
			إذا (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<حرف>::hash_into(ch, h);
				لكل (; i < اندازه; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(اندازه);
				أرجع h;
			}
			ch |= static_cast<صحيح>(b) << (i & 7);
		}
		packed[packed_اندازه - 1 - bi] = ch;
	}
	أرجع hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::ایستل(صحيح دننه_واټن, صحيح len, RTLIL::State padding) ثابت {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	لكل (صحيح i = دننه_واټن; i < دننه_واټن + len; i++)
		ret_bv.ټېل_شا(i < Get_اندازه(*هذا) ? (*هذا)[i] : padding);
	أرجع RTLIL::Const(ret_bv);
}
#تعریف_لغوه check /* check(condition) for Const */

منطقي RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ثابت
{
	أرجع ځانګړتیاوې.شمېر(id);
}

فراغ RTLIL::AttrObject::سېټ_bool_attribute(RTLIL::IdString id, منطقي ارزښت)
{
	إذا (ارزښت)
		ځانګړتیاوې[id] = RTLIL::Const(1);
	وإلا
		ځانګړتیاوې.erase(id);
}

منطقي RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ثابت
{
	ثابت خپلکاری it = ځانګړتیاوې.پیدا_کړه(id);
	إذا (it == ځانګړتیاوې.پای_ټکی())
		أرجع خطأ;
	أرجع it->دوهم.as_bool();
}

فراغ RTLIL::AttrObject::سېټ_string_attribute(RTLIL::IdString id, string ارزښت)
{
	إذا (ارزښت.تشه())
		ځانګړتیاوې.erase(id);
	وإلا
		ځانګړتیاوې[id] = ارزښت;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ثابت
{
	std::string ارزښت;
	ثابت خپلکاری it = ځانګړتیاوې.پیدا_کړه(id);
	إذا (it != ځانګړتیاوې.پای_ټکی())
		ارزښت = it->دوهم.decode_string();
	أرجع ارزښت;
}

فراغ RTLIL::AttrObject::سېټ_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &معلومات)
{
	string attrval;
	لكل (ثابت خپلکاری &s : معلومات) {
		إذا (!attrval.تشه())
			attrval += "|";
		attrval += s;
	}
	سېټ_string_attribute(id, attrval);
}

فراغ RTLIL::AttrObject::جمع_کړه_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &معلومات)
{
	pool<string> union_معلومات = get_strpool_attribute(id);
	union_معلومات.ورننباسه(معلومات.begin(), معلومات.پای_ټکی());
	إذا (!union_معلومات.تشه())
		سېټ_strpool_attribute(id, union_معلومات);
}

فراغ RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->ځانګړتیاوې = std::move(buf);
}

نطاق {
	کينډۍ<ډول_نوم F>
	فراغ walk_process_attr_objects(RTLIL::Process *process, F لیدنه) {
		لیدنه(process);
		std::ویکټور<RTLIL::Case_قاعده*> case_ډېری{&process->ریښه_case};
		طالما (!case_ډېری.تشه()) {
			RTLIL::Case_قاعده *cs = case_ډېری.شا();
			case_ډېری.راوباسه_شا();
			لیدنه(cs);
			لكل (خپلکاری *sw : cs->switches) {
				لیدنه(sw);
				لكل (خپلکاری *case_ : sw->cases)
					case_ډېری.ټېل_شا(case_);
			}
		}
		لكل (خپلکاری *متزامن : process->syncs)
			لكل (خپلکاری &mwa : متزامن->mem_write_actions)
				لیدنه(&mwa);
	}

	کينډۍ<ډول_نوم F>
	فراغ walk_attr_objects(RTLIL::Design *design, F لیدنه) {
		لكل (خپلکاری &[_, module] : design->modules_) {
			لیدنه(module);
			لكل (خپلکاری &[_, wire] : module->wires_)
				لیدنه(wire);
			لكل (خپلکاری &[_, mem] : module->memories)
				لیدنه(mem);
			لكل (خپلکاری &[_, cell] : module->cells_)
				لیدنه(cell);
			لكل (خپلکاری &[_, process] : module->processes)
				walk_process_attr_objects(process, لیدنه);
		}
	}
}


اندازه_t RTLIL::Design::gc_twines()
{
	int64_t پیل = PerformanceTimer::query();
	pool<IdString> ژوندی;
	خپلکاری ریښه = [&](IdString ref) {
		إذا (ref != IdString::Null)
			ژوندی.ورننباسه(ref);
	};

	walk_attr_objects(هذا, [&](ثابت RTLIL::AttrObject *obj) {
		لكل (خپلکاری &attr : obj->ځانګړتیاوې)
			ریښه(attr.لومړی);
	});

	ریښه(selected_فعال_module);

	لكل (خپلکاری &[name, module] : modules_) {
		ریښه(name);
		لكل (IdString پورټ : module->ports)
			ریښه(پورټ);
		لكل (IdString param : module->avail_parameters)
			ریښه(param);
		لكل (خپلکاری &[param, _] : module->parameter_default_ارزښتونه)
			ریښه(param);
		لكل (خپلکاری &[name, _] : module->memories)
			ریښه(name);
		لكل (خپلکاری &[name, _] : module->processes)
			ریښه(name);
		لكل (خپلکاری &[name, wire] : module->wires_) {
			ریښه(name);
			إذا (wire->معلوم_driver())
				ریښه(wire->driver_پورټ());
		}
		لكل (خپلکاری &[name, cell] : module->cells_) {
			ریښه(name);
			ریښه(cell->type);
			لكل (خپلکاری &conn : cell->connections())
				ریښه(conn.لومړی);
			لكل (خپلکاری &param : cell->parameters)
				ریښه(param.لومړی);
		}
		لكل (خپلکاری &[_, process] : module->processes) {
			std::ویکټور<RTLIL::Case_قاعده*> case_ډېری{&process->ریښه_case};
			طالما (!case_ډېری.تشه()) {
				RTLIL::Case_قاعده *cs = case_ډېری.شا();
				case_ډېری.راوباسه_شا();
				لكل (خپلکاری *sw : cs->switches) {
					لكل (خپلکاری *case_ : sw->cases)
						case_ډېری.ټېل_شا(case_);
				}
			}
			لكل (خپلکاری *متزامن : process->syncs) {
				لكل (خپلکاری &mwa : متزامن->mem_write_actions)
					ریښه(mwa.memid);
			}
		}
	}

	خپلکاری ریښه_selection = [&](ثابت RTLIL::Selection &sel) {
		لكل (IdString m : sel.selected_modules)
			ریښه(m);
		لكل (خپلکاری &[m, members] : sel.selected_members) {
			ریښه(m);
			لكل (IdString member : members)
				ریښه(member);
		}
	};

	لكل (خپلکاری &[name, sel] : selection_vars) {
		ریښه(name);
		ریښه_selection(sel);
	}

	لكل (خپلکاری &sel : selection_ډېری)
		ریښه_selection(sel);

	اندازه_t erased = twines_.gc(ژوندی);

	int64_t وخت_ns = PerformanceTimer::query() - پیل;
	Pass::کمول_from_current_runtime_ns(وخت_ns);
	twine_gc_ns += وخت_ns;
	++twine_gc_شمېر;
	أرجع erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ثابت
{
	pool<string> معلومات;
	إذا (ځانګړتیاوې.شمېر(id) != 0)
		لكل (خپلکاری s : وېشل_نښې(get_string_attribute(id), "|"))
			معلومات.ورننباسه(s);
	أرجع معلومات;
}

فراغ RTLIL::AttrObject::سېټ_hdlname_attribute(ثابت ویکټور<string> &hierarchy)
{
	string attrval;
	لكل (ثابت خپلکاری &ident : hierarchy) {
		إذا (!attrval.تشه())
			attrval += " ";
		attrval += ident;
	}
	سېټ_string_attribute(ID::hdlname, attrval);
}

ویکټور<string> RTLIL::AttrObject::get_hdlname_attribute() ثابت
{
	أرجع وېشل_نښې(get_string_attribute(ID::hdlname), " ");
}

فراغ RTLIL::AttrObject::سېټ_intvec_attribute(RTLIL::IdString id, ثابت ویکټور<صحيح> &معلومات)
{
	std::stringstream attrval;
	لكل (خپلکاری &i : معلومات) {
		إذا (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	ځانګړتیاوې[id] = RTLIL::Const(attrval.str());
}

ویکټور<صحيح> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ثابت
{
	ویکټور<صحيح> معلومات;
	خپلکاری it = ځانګړتیاوې.پیدا_کړه(id);
	إذا (it != ځانګړتیاوې.پای_ټکی())
		لكل (ثابت خپلکاری &s : وېشل_نښې(ځانګړتیاوې.at(id).decode_string())) {
			حرف *پای_ټکی = تش_اشاره;
			errno = 0;
			اوږد ارزښت = strtol(s.c_str(), &پای_ټکی, 10);
			إذا (پای_ټکی != s.c_str() + s.اندازه())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			إذا (errno == ERANGE || ارزښت < INT_MIN || ارزښت > لوی_عدد)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			معلومات.ټېل_شا(ارزښت);
		}
	أرجع معلومات;
}

منطقي RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ثابت
{
	إذا (current_design != تش_اشاره) {
		خپلکاری module = current_design->module(mod_name);
		أرجع module && module->get_blackbox_attribute();
	} وإلا {
		log_warning("Unable to check if module is boxed for null design.\n");
		أرجع خطأ;
	}
}

منطقي RTLIL::Selection::selected_module(RTLIL::IdString mod_name) ثابت
{
	إذا (complete_selection)
		أرجع ریښتیا;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (ډک_selection)
		أرجع ریښتیا;
	إذا (selected_modules.شمېر(mod_name) > 0)
		أرجع ریښتیا;
	إذا (selected_members.شمېر(mod_name) > 0)
		أرجع ریښتیا;
	أرجع خطأ;
}

منطقي RTLIL::Selection::selected_ټولیز_module(RTLIL::IdString mod_name) ثابت
{
	إذا (complete_selection)
		أرجع ریښتیا;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (ډک_selection)
		أرجع ریښتیا;
	إذا (selected_modules.شمېر(mod_name) > 0)
		أرجع ریښتیا;
	أرجع خطأ;
}

منطقي RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	إذا (complete_selection)
		أرجع ریښتیا;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (ډک_selection)
		أرجع ریښتیا;
	إذا (selected_modules.شمېر(mod_name) > 0)
		أرجع ریښتیا;
	إذا (selected_members.شمېر(mod_name) > 0)
		إذا (selected_members.at(mod_name).شمېر(memb_name) > 0)
			أرجع ریښتیا;
	أرجع خطأ;
}

فراغ RTLIL::Selection::optimize(RTLIL::Design *design)
{
	إذا (design != current_design) {
		current_design = design;
	}

	إذا (selects_boxes && ډک_selection)
		complete_selection = ریښتیا;
	إذا (complete_selection) {
		ډک_selection = خطأ;
		selects_boxes = ریښتیا;
	}
	إذا (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		أرجع;
	}

	std::ویکټور<RTLIL::IdString> del_list, جمع_کړه_list;

	del_list.clear();
	لكل (خپلکاری mod_name : selected_modules) {
		إذا (current_design->module(mod_name) == تش_اشاره || (!selects_boxes && boxed_module(mod_name)))
			del_list.ټېل_شا(mod_name);
		selected_members.erase(mod_name);
	}
	لكل (خپلکاری mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	لكل (خپلکاری &it : selected_members)
		إذا (current_design->module(it.لومړی) == تش_اشاره || (!selects_boxes && boxed_module(it.لومړی)))
			del_list.ټېل_شا(it.لومړی);
	لكل (خپلکاری mod_name : del_list)
		selected_members.erase(mod_name);

	لكل (خپلکاری &it : selected_members) {
		del_list.clear();
		لكل (خپلکاری memb_name : it.دوهم)
			إذا (current_design->module(it.لومړی)->شمېر_id(memb_name) == 0)
				del_list.ټېل_شا(memb_name);
		لكل (خپلکاری memb_name : del_list)
			it.دوهم.erase(memb_name);
	}

	del_list.clear();
	جمع_کړه_list.clear();
	لكل (خپلکاری &it : selected_members)
		إذا (it.دوهم.اندازه() == 0)
			del_list.ټېل_شا(it.لومړی);
		وإلا إذا (it.دوهم.اندازه() == current_design->module(it.لومړی)->wires_.اندازه() + current_design->module(it.لومړی)->memories.اندازه() +
				current_design->module(it.لومړی)->cells_.اندازه() + current_design->module(it.لومړی)->processes.اندازه())
			جمع_کړه_list.ټېل_شا(it.لومړی);
	لكل (خپلکاری mod_name : del_list)
		selected_members.erase(mod_name);
	لكل (خپلکاری mod_name : جمع_کړه_list) {
		selected_members.erase(mod_name);
		selected_modules.ورننباسه(mod_name);
	}

	إذا (selected_modules.اندازه() == current_design->modules_.اندازه()) {
		selected_modules.clear();
		selected_members.clear();
		إذا (selects_boxes)
			complete_selection = ریښتیا;
		وإلا
			ډک_selection = ریښتیا;
	}
}

فراغ RTLIL::Selection::clear()
{
	ډک_selection = خطأ;
	complete_selection = خطأ;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_تعریفوي (جديد define_map_t)
{
	ساكن بینښه صحيح hashidx_شمېر = 123456789;
	hashidx_شمېر = mkhash_xorshift(hashidx_شمېر);
	hashidx_ = hashidx_شمېر;

	refcount_modules_ = 0;
	selected_فعال_module.clear();
	ټېل_ډک_selection();

	RTLIL::Design::get_all_designs()->ورننباسه(std::pair<بینښه صحيح, RTLIL::Design*>(hashidx_, هذا));
}

RTLIL::Design::~Design()
{
	لكل (خپلکاری &pr : modules_)
		ړنګول pr.دوهم;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

ساكن std::map<بینښه صحيح, RTLIL::Design*> all_designs;
std::map<بینښه صحيح, RTLIL::Design*> *RTLIL::Design::get_all_designs(فراغ)
{
	أرجع &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	أرجع RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ثابت RTLIL::Module *RTLIL::Design::module(IdString id) ثابت {
	twines_.check_owned(id);
	أرجع modules_.شمېر(id) ? modules_.at(id) : عدم;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	أرجع modules_.شمېر(id) ? modules_.at(id) : عدم;
}

RTLIL::Module *RTLIL::Design::سر_module() ثابت
{
	RTLIL::Module *module = تش_اشاره;
	صحيح module_شمېر = 0;

	لكل (خپلکاری mod : selected_modules()) {
		إذا (mod->get_bool_attribute(ID::سر))
			أرجع mod;
		module_شمېر++;
		module = mod;
	}

	أرجع module_شمېر == 1 ? module : تش_اشاره;
}

RTLIL::Module *RTLIL::Design::جمع_کړه_Module(RTLIL::IdString name)
{
	إذا (modules_.شمېر(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = جديد RTLIL::Module;
	modules_[name] = module;
	module->design = هذا;
	module->name = name;

	لكل (خپلکاری mon : monitors)
		mon->notify_module_جمع_کړه(module);

	إذا (yosys_xtrace) {
		لوګاریتم("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	أرجع module;
}

فراغ RTLIL::Design::scratchpad_unset(ثابت std::string &varname)
{
	scratchpad.erase(varname);
}

فراغ RTLIL::Design::scratchpad_سېټ_int(ثابت std::string &varname, صحيح ارزښت)
{
	scratchpad[varname] = stringf("%d", ارزښت);
}

فراغ RTLIL::Design::scratchpad_سېټ_bool(ثابت std::string &varname, منطقي ارزښت)
{
	scratchpad[varname] = ارزښت ? "true" : "false";
}

فراغ RTLIL::Design::scratchpad_سېټ_string(ثابت std::string &varname, std::string ارزښت)
{
	scratchpad[varname] = std::move(ارزښت);
}

صحيح RTLIL::Design::scratchpad_get_int(ثابت std::string &varname, صحيح default_ارزښت_2) ثابت
{
	خپلکاری it = scratchpad.پیدا_کړه(varname);
	إذا (it == scratchpad.پای_ټکی())
		أرجع default_ارزښت_2;

	ثابت std::string &str = it->دوهم;

	إذا (str == "0" || str == "false")
		أرجع 0;

	إذا (str == "1" || str == "true")
		أرجع 1;

	حرف *endptr = تش_اشاره;
	اوږد صحيح parsed_ارزښت = strtol(str.c_str(), &endptr, 10);
	أرجع *endptr ? default_ارزښت_2 : parsed_ارزښت;
}

منطقي RTLIL::Design::scratchpad_get_bool(ثابت std::string &varname, منطقي default_ارزښت_2) ثابت
{
	خپلکاری it = scratchpad.پیدا_کړه(varname);
	إذا (it == scratchpad.پای_ټکی())
		أرجع default_ارزښت_2;

	ثابت std::string &str = it->دوهم;

	إذا (str == "0" || str == "false")
		أرجع خطأ;

	إذا (str == "1" || str == "true")
		أرجع ریښتیا;

	أرجع default_ارزښت_2;
}

std::string RTLIL::Design::scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_ارزښت_2) ثابت
{
	خپلکاری it = scratchpad.پیدا_کړه(varname);
	إذا (it == scratchpad.پای_ټکی())
		أرجع default_ارزښت_2;

	أرجع it->دوهم;
}

فراغ RTLIL::Design::remove(RTLIL::Module *module)
{
	لكل (خپلکاری mon : monitors)
		mon->notify_module_del(module);

	إذا (yosys_xtrace) {
		لوګاریتم("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	ړنګول module;
}

فراغ RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	جمع_کړه(module);
}

فراغ RTLIL::Design::ترتیبول()
{
	scratchpad.ترتیبول();
	modules_.ترتیبول(ترتیبول_by_id_str(twines_));
	لكل (خپلکاری &it : modules_)
		it.دوهم->ترتیبول();
}

فراغ RTLIL::Design::ترتیبول_modules()
{
	scratchpad.ترتیبول();
	modules_.ترتیبول(ترتیبول_by_id_str(twines_));
}

فراغ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

فراغ RTLIL::Design::check()
{
#که_نه_تعریف_شوی NDEBUG
	log_assert(!selection_ډېری.تشه());
	صحيح pool_اندازه = 0;
	لكل (خپلکاری &it : modules_)
		pool_اندازه = std::max(pool_اندازه, ThreadPool::work_pool_اندازه(0, it.دوهم->cells_اندازه(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_اندازه);
	لكل (خپلکاری &it : modules_) {
		log_assert(هذا == it.دوهم->design);
		log_assert(it.لومړی == it.دوهم->name);
		check_module(it.دوهم, thread_pool);
	}
#د_که_پای
}

فراغ RTLIL::Design::optimize()
{
	لكل (خپلکاری &it : modules_)
		it.دوهم->optimize();
	لكل (خپلکاری &it : selection_ډېری)
		it.optimize(هذا);
	لكل (خپلکاری &it : selection_vars)
		it.دوهم.optimize(هذا);
}

فراغ RTLIL::Design::clone_into(RTLIL::Design *dst) ثابت
{
	log_assert(dst->modules_.تشه());
	dst->twines_ = twines_;
	لكل (خپلکاری it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->دوهم->clone(dst);
}

منطقي RTLIL::Design::selected_module(IdString mod_name) ثابت
{
	إذا (!selected_فعال_module.تشه() && mod_name != selected_فعال_module)
		أرجع خطأ;
	أرجع selection().selected_module(mod_name);
}

منطقي RTLIL::Design::selected_ټولیز_module(RTLIL::IdString mod_name) ثابت
{
	إذا (!selected_فعال_module.تشه() && mod_name != selected_فعال_module)
		أرجع خطأ;
	أرجع selection().selected_ټولیز_module(mod_name);
}

منطقي RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	إذا (!selected_فعال_module.تشه() && mod_name != selected_فعال_module)
		أرجع خطأ;
	أرجع selection().selected_member(mod_name, memb_name);
}

منطقي RTLIL::Design::selected_module(RTLIL::Module *mod) ثابت
{
	أرجع selected_module(mod->name);
}

منطقي RTLIL::Design::selected_ټولیز_module(RTLIL::Module *mod) ثابت
{
	أرجع selected_ټولیز_module(mod->name);
}

فراغ RTLIL::Design::ټېل_selection(RTLIL::Selection sel)
{
	sel.current_design = هذا;
	selection_ډېری.ټېل_شا(sel);
}

فراغ RTLIL::Design::ټېل_تشه_selection()
{
	ټېل_selection(RTLIL::Selection::تشه_Selection(هذا));
}

فراغ RTLIL::Design::ټېل_ډک_selection()
{
	ټېل_selection(RTLIL::Selection::ډک_Selection(هذا));
}

فراغ RTLIL::Design::ټېل_complete_selection()
{
	ټېل_selection(RTLIL::Selection::CompleteSelection(هذا));
}

فراغ RTLIL::Design::راوباسه_selection()
{
	selection_ډېری.راوباسه_شا();
	// Default to a full_selection if we ran out of stack
	إذا (selection_ډېری.تشه())
		ټېل_ډک_selection();
}

std::string RTLIL::Design::منزل_rtlil_str(منطقي only_selected) ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(هذا), only_selected, ریښتیا, خطأ, RTLIL_BACKEND::Dump_ډېرپېښ::Readable);
	أرجع f.str();
}

std::ویکټور<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ثابت
{
	منطقي include_partials = partials == RTLIL::SELECT_ALL;
	منطقي exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	منطقي ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::ویکټور<RTLIL::Module*> پایله;
	پایله.reserve(modules_.اندازه());
	لكل (خپلکاری &it : modules_) {
		إذا (selected_ټولیز_module(it.لومړی) || (include_partials && selected_module(it.لومړی))) {
			إذا (!(exclude_boxes && it.دوهم->get_blackbox_attribute(ignore_wb)))
				پایله.ټېل_شا(it.دوهم);
			وإلا
				بدل (boxes)
				{
				حالة RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.دوهم);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.دوهم);
					اكسر;
				حالة RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.دوهم);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.دوهم);
					اكسر;
				حالة RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.دوهم);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.دوهم);
					اكسر;
				افتراضي:
					اكسر;
				}
		} وإلا إذا (!include_partials && selected_module(it.لومړی)) {
			بدل(partials)
			{
			حالة RTLIL::SELECT_ټولیز_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.دوهم);
				اكسر;
			حالة RTLIL::SELECT_ټولیز_ERR:
				log_error("Unsupported partially selected module %s.\n", it.دوهم);
				اكسر;
			حالة RTLIL::SELECT_ټولیز_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.دوهم);
				اكسر;
			افتراضي:
				اكسر;
			}
		}
	}
	أرجع پایله;
}

RTLIL::Module::Module()
{
	ساكن بینښه صحيح hashidx_شمېر = 123456789;
	hashidx_شمېر = mkhash_xorshift(hashidx_شمېر);
	hashidx_ = hashidx_شمېر;

	design = تش_اشاره;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->ورننباسه(std::pair<بینښه صحيح, RTLIL::Module*>(hashidx_, هذا));
#د_که_پای
}

RTLIL::Module::~Module()
{
	لكل (خپلکاری &pr : wires_)
		ړنګول pr.دوهم;
	لكل (خپلکاری &pr : memories)
		ړنګول pr.دوهم;
	لكل (خپلکاری &pr : cells_)
		ړنګول pr.دوهم;
	لكل (خپلکاری &pr : processes)
		ړنګول pr.دوهم;
#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#د_که_پای
}

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
ساكن std::map<بینښه صحيح, RTLIL::Module*> all_modules;
std::map<بینښه صحيح, RTLIL::Module*> *RTLIL::Module::get_all_modules(فراغ)
{
	أرجع &all_modules;
}
#د_که_پای

فراغ RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	لكل (خپلکاری it = wires_.begin(); it != wires_.پای_ټکی(); ++it)
		إذا (!it->دوهم->پورټ_ننوت && !it->دوهم->پورټ_output)
			delwires.ورننباسه(it->دوهم);

	لكل (خپلکاری it = memories.begin(); it != memories.پای_ټکی(); ++it)
		ړنګول it->دوهم;
	memories.clear();

	لكل (خپلکاری it = cells_.begin(); it != cells_.پای_ټکی(); ++it)
		ړنګول it->دوهم;
	cells_.clear();

	لكل (خپلکاری it = processes.begin(); it != processes.پای_ټکی(); ++it)
		ړنګول it->دوهم;
	processes.clear();

	connections_.clear();

	remove(delwires);
	سېټ_bool_attribute(ID::blackbox);
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

اندازه_t RTLIL::Module::شمېر_id(RTLIL::IdString id)
{
	أرجع wires_.شمېر(id) + cells_.شمېر(id) + memories.شمېر(id) + processes.شمېر(id);
}

#که_نه_تعریف_شوی NDEBUG
نطاق {
	بنية دننی_Cell_Checker
	{
		ثابت RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		دننی_Cell_Checker(ثابت RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

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
			خپلکاری it = cell->parameters.پیدا_کړه(name);
			إذا (it == cell->parameters.پای_ټکی()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.ورننباسه(name);
			أرجع it->دوهم.as_int();
		}

		صحيح param_bool(RTLIL::IdString name)
		{
			صحيح v = param(name);
			إذا (Get_اندازه(cell->parameters.at(name)) > 32)
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

		فراغ param_بټونه(RTLIL::IdString name, صحيح پلنوالی)
		{
			param(name);
			إذا (Get_اندازه(cell->parameters.at(name)) != پلنوالی)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			أرجع cell->parameters.at(name).decode_string();
		}

		فراغ پورټ(RTLIL::IdString name, صحيح پلنوالی)
		{
			خپلکاری it = cell->connections_.پیدا_کړه(name);
			إذا (it == cell->connections_.پای_ټکی()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			إذا (Get_اندازه(it->دوهم) != پلنوالی) {
				std::string err = stringf("Expected port to have width %d", پلنوالی);
				error(__LINE__, err);
			}
			expected_ports.ورننباسه(name);
		}

		فراغ check_expected(منطقي check_matched_sign = خطأ)
		{
			لكل (خپلکاری &para : cell->parameters)
				إذا (expected_params.شمېر(para.لومړی) == 0)
					error(__LINE__);
			لكل (خپلکاری &conn : cell->connections())
				إذا (expected_ports.شمېر(conn.لومړی) == 0)
					error(__LINE__);

			إذا (check_matched_sign) {
				log_assert(expected_params.شمېر(ID::A_SIGNED) != 0 && expected_params.شمېر(ID::B_SIGNED) != 0);
				منطقي a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				منطقي b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				إذا (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		فراغ check()
		{
			إذا (cell->type.is_عامه() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				أرجع;

			إذا (cell->type.دننه(ID($buf), ID($barrier))) {
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::Y, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.دننه(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				پورټ(ID::A, param(ID::A_پلنوالی));
				پورټ(ID::Y, param(ID::Y_پلنوالی));
				check_expected();
				أرجع;
			}

			إذا (cell->type.دننه(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورټ(ID::A, param(ID::A_پلنوالی));
				پورټ(ID::B, param(ID::B_پلنوالی));
				پورټ(ID::Y, param(ID::Y_پلنوالی));
				check_expected(ریښتیا);
				أرجع;
			}

			إذا (cell->type.دننه(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				پورټ(ID::A, param(ID::A_پلنوالی));
				پورټ(ID::Y, param(ID::Y_پلنوالی));
				check_expected();
				أرجع;
			}

			إذا (cell->type.دننه(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/خطأ);
				پورټ(ID::A, param(ID::A_پلنوالی));
				پورټ(ID::B, param(ID::B_پلنوالی));
				پورټ(ID::Y, param(ID::Y_پلنوالی));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type.دننه(ID($ښويدنه), ID($shiftx))) {
				إذا (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/خطأ);
				} وإلا {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				پورټ(ID::A, param(ID::A_پلنوالی));
				پورټ(ID::B, param(ID::B_پلنوالی));
				پورټ(ID::Y, param(ID::Y_پلنوالی));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type.دننه(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورټ(ID::A, param(ID::A_پلنوالی));
				پورټ(ID::B, param(ID::B_پلنوالی));
				پورټ(ID::Y, param(ID::Y_پلنوالی));
				check_expected(ریښتیا);
				أرجع;
			}

			إذا (cell->type.دننه(ID($جمع_کړه), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($توان))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورټ(ID::A, param(ID::A_پلنوالی));
				پورټ(ID::B, param(ID::B_پلنوالی));
				پورټ(ID::Y, param(ID::Y_پلنوالی));
				check_expected(cell->type != ID($توان));
				أرجع;
			}

			إذا (cell->type == ID($fa)) {
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::B, param(ID::پلنوالی_2));
				پورټ(ID::C, param(ID::پلنوالی_2));
				پورټ(ID::X, param(ID::پلنوالی_2));
				پورټ(ID::Y, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($lcu)) {
				پورټ(ID::P, param(ID::پلنوالی_2));
				پورټ(ID::G, param(ID::پلنوالی_2));
				پورټ(ID::CI, 1);
				پورټ(ID::CO, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورټ(ID::A, param(ID::A_پلنوالی));
				پورټ(ID::B, param(ID::B_پلنوالی));
				پورټ(ID::CI, 1);
				پورټ(ID::BI, 1);
				پورټ(ID::X, param(ID::Y_پلنوالی));
				پورټ(ID::Y, param(ID::Y_پلنوالی));
				پورټ(ID::CO, param(ID::Y_پلنوالی));
				check_expected(ریښتیا);
				أرجع;
			}

			إذا (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_پلنوالی);
				پورټ(ID::A, param(ID::A_پلنوالی));
				پورټ(ID::B, param(ID::B_پلنوالی));
				پورټ(ID::Y, param(ID::Y_پلنوالی));
				check_expected();
				Macc().from_cell(cell);
				أرجع;
			}

			إذا (cell->type == ID($macc_v2)) {
				إذا (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				إذا (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_بټونه(ID::حاصل_ضرب_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_بټونه(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_بټونه(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_بټونه(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_بټونه(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				إذا (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_بټونه(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_بټونه(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_بټونه(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ثابت Const &a_پلنوالی = cell->getParam(ID::A_WIDTHS);
				ثابت Const &b_پلنوالی = cell->getParam(ID::B_WIDTHS);
				ثابت Const &c_پلنوالی = cell->getParam(ID::C_WIDTHS);
				صحيح a_پلنوالی_مجموعه = 0, b_پلنوالی_مجموعه = 0, c_پلنوالی_مجموعه = 0;
				لكل (صحيح i = 0; i < param(ID::NPRODUCTS); i++) {
					a_پلنوالی_مجموعه += a_پلنوالی.ایستل(16 * i, 16).as_int(خطأ);
					b_پلنوالی_مجموعه += b_پلنوالی.ایستل(16 * i, 16).as_int(خطأ);
				}
				لكل (صحيح i = 0; i < param(ID::NADDENDS); i++) {
					c_پلنوالی_مجموعه += c_پلنوالی.ایستل(16 * i, 16).as_int(خطأ);
				}
				پورټ(ID::A, a_پلنوالی_مجموعه);
				پورټ(ID::B, b_پلنوالی_مجموعه);
				پورټ(ID::C, c_پلنوالی_مجموعه);
				پورټ(ID::Y, param(ID::Y_پلنوالی));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				پورټ(ID::A, param(ID::A_پلنوالی));
				پورټ(ID::Y, param(ID::Y_پلنوالی));
				check_expected();
				أرجع;
			}

			إذا (cell->type.دننه(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورټ(ID::A, param(ID::A_پلنوالی));
				پورټ(ID::B, param(ID::B_پلنوالی));
				پورټ(ID::Y, param(ID::Y_پلنوالی));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type == ID($ټوټه)) {
				param(ID::دننه_واټن_2);
				پورټ(ID::A, param(ID::A_پلنوالی));
				پورټ(ID::Y, param(ID::Y_پلنوالی));
				إذا (param(ID::دننه_واټن_2) + param(ID::Y_پلنوالی) > param(ID::A_پلنوالی))
					error(__LINE__);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($concat)) {
				پورټ(ID::A, param(ID::A_پلنوالی));
				پورټ(ID::B, param(ID::B_پلنوالی));
				پورټ(ID::Y, param(ID::A_پلنوالی) + param(ID::B_پلنوالی));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mux)) {
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::B, param(ID::پلنوالی_2));
				پورټ(ID::S, 1);
				پورټ(ID::Y, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($pmux)) {
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::B, param(ID::پلنوالی_2) * param(ID::S_پلنوالی));
				پورټ(ID::S, param(ID::S_پلنوالی));
				پورټ(ID::Y, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bmux)) {
				پورټ(ID::A, param(ID::پلنوالی_2) << param(ID::S_پلنوالی));
				پورټ(ID::S, param(ID::S_پلنوالی));
				پورټ(ID::Y, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($demux)) {
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::S, param(ID::S_پلنوالی));
				پورټ(ID::Y, param(ID::پلنوالی_2) << param(ID::S_پلنوالی));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($lut)) {
				param(ID::LUT);
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sop)) {
				param(ID::ژوروالی_2);
				param(ID::جدول);
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sr)) {
				param_bool(ID::سېټ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورټ(ID::سېټ_2, param(ID::پلنوالی_2));
				پورټ(ID::CLR, param(ID::پلنوالی_2));
				پورټ(ID::Q,   param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($ff)) {
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				پورټ(ID::CLK, 1);
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				پورټ(ID::CLK, 1);
				پورټ(ID::EN, 1);
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::سېټ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورټ(ID::CLK, 1);
				پورټ(ID::سېټ_2, param(ID::پلنوالی_2));
				پورټ(ID::CLR, param(ID::پلنوالی_2));
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffsre)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::سېټ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				param_bool(ID::EN_POLARITY);
				پورټ(ID::CLK, 1);
				پورټ(ID::EN, 1);
				پورټ(ID::سېټ_2, param(ID::پلنوالی_2));
				پورټ(ID::CLR, param(ID::پلنوالی_2));
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بټونه(ID::ARST_ارزښت, param(ID::پلنوالی_2));
				پورټ(ID::CLK, 1);
				پورټ(ID::ARST, 1);
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_بټونه(ID::SRST_ارزښت, param(ID::پلنوالی_2));
				پورټ(ID::CLK, 1);
				پورټ(ID::SRST, 1);
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.دننه(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_بټونه(ID::SRST_ارزښت, param(ID::پلنوالی_2));
				پورټ(ID::CLK, 1);
				پورټ(ID::EN, 1);
				پورټ(ID::SRST, 1);
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بټونه(ID::ARST_ارزښت, param(ID::پلنوالی_2));
				پورټ(ID::CLK, 1);
				پورټ(ID::EN, 1);
				پورټ(ID::ARST, 1);
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				پورټ(ID::CLK, 1);
				پورټ(ID::ALOAD, 1);
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::AD, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($aldffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				پورټ(ID::CLK, 1);
				پورټ(ID::EN, 1);
				پورټ(ID::ALOAD, 1);
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::AD, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				پورټ(ID::EN, 1);
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بټونه(ID::ARST_ارزښت, param(ID::پلنوالی_2));
				پورټ(ID::EN, 1);
				پورټ(ID::ARST, 1);
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::سېټ_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورټ(ID::EN, 1);
				پورټ(ID::سېټ_2, param(ID::پلنوالی_2));
				پورټ(ID::CLR, param(ID::پلنوالی_2));
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_بټونه);
				param(ID::STATE_عدد);
				param(ID::STATE_عدد_LOG2);
				param(ID::STATE_RST);
				param_بټونه(ID::STATE_جدول, param(ID::STATE_بټونه) * param(ID::STATE_عدد));
				param(ID::TRANS_عدد);
				param_بټونه(ID::TRANS_جدول, param(ID::TRANS_عدد) * (2*param(ID::STATE_عدد_LOG2) + param(ID::CTRL_دننه_پلنوالی) + param(ID::CTRL_بهر_پلنوالی)));
				پورټ(ID::CLK, 1);
				پورټ(ID::ARST, 1);
				پورټ(ID::CTRL_دننه, param(ID::CTRL_دننه_پلنوالی));
				پورټ(ID::CTRL_بهر, param(ID::CTRL_بهر_پلنوالی));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				پورټ(ID::CLK, 1);
				پورټ(ID::EN, 1);
				پورټ(ID::ADDR, param(ID::ABITS));
				پورټ(ID::معلومات_2, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_ماسک);
				param(ID::COLLISION_X_ماسک);
				param_bool(ID::CE_OVER_SRST);
				param_بټونه(ID::ARST_ارزښت, param(ID::پلنوالی_2));
				param_بټونه(ID::SRST_ارزښت, param(ID::پلنوالی_2));
				param_بټونه(ID::INIT_ارزښت, param(ID::پلنوالی_2));
				پورټ(ID::CLK, 1);
				پورټ(ID::EN, 1);
				پورټ(ID::ARST, 1);
				پورټ(ID::SRST, 1);
				پورټ(ID::ADDR, param(ID::ABITS));
				پورټ(ID::معلومات_2, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::لومړیتوب);
				پورټ(ID::CLK, 1);
				پورټ(ID::EN, param(ID::پلنوالی_2));
				پورټ(ID::ADDR, param(ID::ABITS));
				پورټ(ID::معلومات_2, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memwr_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PORTID);
				param(ID::لومړیتوب_ماسک_2);
				پورټ(ID::CLK, 1);
				پورټ(ID::EN, param(ID::پلنوالی_2));
				پورټ(ID::ADDR, param(ID::ABITS));
				پورټ(ID::معلومات_2, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::لومړیتوب);
				پورټ(ID::ADDR, param(ID::ABITS));
				پورټ(ID::معلومات_2, param(ID::پلنوالی_2) * param(ID::ټکي));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::لومړیتوب);
				پورټ(ID::ADDR, param(ID::ABITS));
				پورټ(ID::معلومات_2, param(ID::پلنوالی_2) * param(ID::ټکي));
				پورټ(ID::EN, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mem)) {
				param(ID::MEMID);
				param(ID::اندازه_2);
				param(ID::دننه_واټن_2);
				param(ID::INIT);
				param_بټونه(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_بټونه(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_بټونه(ID::RD_TRANSPARENT, max(1, param(ID::RD_PORTS)));
				param_بټونه(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_بټونه(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				پورټ(ID::RD_CLK, param(ID::RD_PORTS));
				پورټ(ID::RD_EN, param(ID::RD_PORTS));
				پورټ(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				پورټ(ID::RD_معلومات, param(ID::RD_PORTS) * param(ID::پلنوالی_2));
				پورټ(ID::WR_CLK, param(ID::WR_PORTS));
				پورټ(ID::WR_EN, param(ID::WR_PORTS) * param(ID::پلنوالی_2));
				پورټ(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				پورټ(ID::WR_معلومات, param(ID::WR_PORTS) * param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mem_v2)) {
				param(ID::MEMID);
				param(ID::اندازه_2);
				param(ID::دننه_واټن_2);
				param(ID::INIT);
				param_بټونه(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_بټونه(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_بټونه(ID::RD_TRANSPARENCY_ماسک, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_بټونه(ID::RD_COLLISION_X_ماسک, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_بټونه(ID::RD_WIDE_CONTINUATION, max(1, param(ID::RD_PORTS)));
				param_بټونه(ID::RD_CE_OVER_SRST, max(1, param(ID::RD_PORTS)));
				param_بټونه(ID::RD_ARST_ارزښت, max(1, param(ID::RD_PORTS) * param(ID::پلنوالی_2)));
				param_بټونه(ID::RD_SRST_ارزښت, max(1, param(ID::RD_PORTS) * param(ID::پلنوالی_2)));
				param_بټونه(ID::RD_INIT_ارزښت, max(1, param(ID::RD_PORTS) * param(ID::پلنوالی_2)));
				param_بټونه(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_بټونه(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				param_بټونه(ID::WR_WIDE_CONTINUATION, max(1, param(ID::WR_PORTS)));
				param_بټونه(ID::WR_لومړیتوب_ماسک, max(1, param(ID::WR_PORTS) * param(ID::WR_PORTS)));
				پورټ(ID::RD_CLK, param(ID::RD_PORTS));
				پورټ(ID::RD_EN, param(ID::RD_PORTS));
				پورټ(ID::RD_ARST, param(ID::RD_PORTS));
				پورټ(ID::RD_SRST, param(ID::RD_PORTS));
				پورټ(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				پورټ(ID::RD_معلومات, param(ID::RD_PORTS) * param(ID::پلنوالی_2));
				پورټ(ID::WR_CLK, param(ID::WR_PORTS));
				پورټ(ID::WR_EN, param(ID::WR_PORTS) * param(ID::پلنوالی_2));
				پورټ(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				پورټ(ID::WR_معلومات, param(ID::WR_PORTS) * param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($tribuf)) {
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::Y, param(ID::پلنوالی_2));
				پورټ(ID::EN, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bweqx)) {
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::B, param(ID::پلنوالی_2));
				پورټ(ID::Y, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bwmux)) {
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::B, param(ID::پلنوالی_2));
				پورټ(ID::S, param(ID::پلنوالی_2));
				پورټ(ID::Y, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.دننه(ID($أكد), ID($assume), ID($ژوندی), ID($fair), ID($cover))) {
				پورټ(ID::A, 1);
				پورټ(ID::EN, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($initstate)) {
				پورټ(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type.دننه(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				پورټ(ID::Y, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.دننه(ID($anyinit))) {
				پورټ(ID::D, param(ID::پلنوالی_2));
				پورټ(ID::Q, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($equiv)) {
				پورټ(ID::A, 1);
				پورټ(ID::B, 1);
				پورټ(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type.دننه(ID($specify2), ID($specify3))) {
				param_bool(ID::ډک);
				param_bool(ID::SRC_DST_PEN);
				param_bool(ID::SRC_DST_POL);
				param(ID::T_RISE_MIN);
				param(ID::T_RISE_TYP);
				param(ID::T_RISE_MAX);
				param(ID::T_FALL_MIN);
				param(ID::T_FALL_TYP);
				param(ID::T_FALL_MAX);
				پورټ(ID::EN, 1);
				پورټ(ID::SRC, param(ID::SRC_پلنوالی));
				پورټ(ID::DST, param(ID::DST_پلنوالی));
				إذا (cell->type == ID($specify3)) {
					param_bool(ID::څنډه_EN);
					param_bool(ID::څنډه_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					پورټ(ID::DAT, param(ID::DST_پلنوالی));
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
				param(ID::T_حد_MIN);
				param(ID::T_حد_TYP);
				param(ID::T_حد_MAX);
				param(ID::T_LIMIT2_MIN);
				param(ID::T_LIMIT2_TYP);
				param(ID::T_LIMIT2_MAX);
				پورټ(ID::SRC_EN, 1);
				پورټ(ID::DST_EN, 1);
				پورټ(ID::SRC, param(ID::SRC_پلنوالی));
				پورټ(ID::DST, param(ID::DST_پلنوالی));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::لومړیتوب);
				پورټ(ID::EN, 1);
				پورټ(ID::TRG, param(ID::TRG_پلنوالی));
				پورټ(ID::دلیلونه, param(ID::دلیلونه_پلنوالی));
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
				param(ID::لومړیتوب);
				پورټ(ID::A, 1);
				پورټ(ID::EN, 1);
				پورټ(ID::TRG, param(ID::TRG_پلنوالی));
				پورټ(ID::دلیلونه, param(ID::دلیلونه_پلنوالی));
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

			إذا (cell->type == ID($_BUF_))    { پورټ(ID::A,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NOT_))    { پورټ(ID::A,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AND_))    { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NAND_))   { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OR_))     { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NOR_))    { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_XOR_))    { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_XNOR_))   { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_ANDNOT_)) { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_ORNOT_))  { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX_))    { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::S,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NMUX_))   { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::S,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AOI3_))   { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::C,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OAI3_))   { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::C,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AOI4_))   { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::C,1); پورټ(ID::D,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OAI4_))   { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::C,1); پورټ(ID::D,1); پورټ(ID::Y,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_TBUF_))  { پورټ(ID::A,1); پورټ(ID::Y,1); پورټ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_MUX4_))  { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::C,1); پورټ(ID::D,1); پورټ(ID::S,1); پورټ(ID::T,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX8_))  { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::C,1); پورټ(ID::D,1); پورټ(ID::E,1); پورټ(ID::F,1); پورټ(ID::G,1); پورټ(ID::H,1); پورټ(ID::S,1); پورټ(ID::T,1); پورټ(ID::U,1); پورټ(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX16_)) { پورټ(ID::A,1); پورټ(ID::B,1); پورټ(ID::C,1); پورټ(ID::D,1); پورټ(ID::E,1); پورټ(ID::F,1); پورټ(ID::G,1); پورټ(ID::H,1); پورټ(ID::I,1); پورټ(ID::J,1); پورټ(ID::K,1); پورټ(ID::L,1); پورټ(ID::M,1); پورټ(ID::N,1); پورټ(ID::O,1); پورټ(ID::P,1); پورټ(ID::S,1); پورټ(ID::T,1); پورټ(ID::U,1); پورټ(ID::V,1); پورټ(ID::Y,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ پورټ(ID::S,1); پورټ(ID::R,1); پورټ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_FF_)) { پورټ(ID::D,1); پورټ(ID::Q,1); check_expected();  أرجع; }

			إذا (cell->type.دننه(ID($_DFF_N_), ID($_DFF_P_)))
				{ پورټ(ID::D,1); پورټ(ID::Q,1); پورټ(ID::C,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ پورټ(ID::D,1); پورټ(ID::Q,1); پورټ(ID::C,1); پورټ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ پورټ(ID::D,1); پورټ(ID::Q,1); پورټ(ID::C,1); پورټ(ID::R,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ پورټ(ID::D,1); پورټ(ID::Q,1); پورټ(ID::C,1); پورټ(ID::R,1); پورټ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ پورټ(ID::D,1); پورټ(ID::Q,1); پورټ(ID::C,1); پورټ(ID::L,1); پورټ(ID::AD,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ پورټ(ID::D,1); پورټ(ID::Q,1); پورټ(ID::C,1); پورټ(ID::L,1); پورټ(ID::AD,1); پورټ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ پورټ(ID::C,1); پورټ(ID::S,1); پورټ(ID::R,1); پورټ(ID::D,1); پورټ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ پورټ(ID::C,1); پورټ(ID::S,1); پورټ(ID::R,1); پورټ(ID::D,1); پورټ(ID::E,1); پورټ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ پورټ(ID::D,1); پورټ(ID::Q,1); پورټ(ID::C,1); پورټ(ID::R,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ پورټ(ID::D,1); پورټ(ID::Q,1); پورټ(ID::C,1); پورټ(ID::R,1); پورټ(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ پورټ(ID::E,1); پورټ(ID::D,1); پورټ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ پورټ(ID::E,1); پورټ(ID::R,1); پورټ(ID::D,1); پورټ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ پورټ(ID::E,1); پورټ(ID::S,1); پورټ(ID::R,1); پورټ(ID::D,1); پورټ(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.دننه(ID($سېټ_نښان))) {
				param(ID::پلنوالی_2);
				param(ID::نښان_2);
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::سېټ_2, param(ID::پلنوالی_2));
				پورټ(ID::CLR, param(ID::پلنوالی_2));
				پورټ(ID::Y, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.دننه(ID($get_نښان),ID($original_نښان))) {
				param(ID::پلنوالی_2);
				param(ID::نښان_2);
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::Y, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.دننه(ID($overwrite_نښان))) {
				param(ID::پلنوالی_2);
				param(ID::نښان_2);
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::سېټ_2, param(ID::پلنوالی_2));
				پورټ(ID::CLR, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.دننه(ID($future_ff))) {
				param(ID::پلنوالی_2);
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::Y, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.دننه(ID($ننوت_پورټ))) {
				param(ID::پلنوالی_2);
				پورټ(ID::Y, param(ID::پلنوالی_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.دننه(ID($connect))) {
				param(ID::پلنوالی_2);
				پورټ(ID::A, param(ID::پلنوالی_2));
				پورټ(ID::B, param(ID::پلنوالی_2));
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
#د_که_پای

فراغ RTLIL::Module::ترتیبول()
{
	wires_.ترتیبول(ترتیبول_by_id_str(design->twines()));
	cells_.ترتیبول(ترتیبول_by_id_str(design->twines()));
	parameter_default_ارزښتونه.ترتیبول(ترتیبول_by_id_str(design->twines()));
	memories.ترتیبول(ترتیبول_by_id_str(design->twines()));
	processes.ترتیبول(ترتیبول_by_id_str(design->twines()));
	لكل (خپلکاری &it : cells_)
		it.دوهم->ترتیبول();
	لكل (خپلکاری &it : wires_)
		it.دوهم->ځانګړتیاوې.ترتیبول(ترتیبول_by_id_str(design->twines()));
	لكل (خپلکاری &it : memories)
		it.دوهم->ځانګړتیاوې.ترتیبول(ترتیبول_by_id_str(design->twines()));
}

#که_نه_تعریف_شوی NDEBUG
ساكن فراغ check_id_کلیګانې(ثابت dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	لكل (خپلکاری &it : keyed)
		log_assert(!it.لومړی.تشه());
}
#د_که_پای

فراغ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#که_نه_تعریف_شوی NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_اندازه(0, module->cells_اندازه(), 1000));
	ثابت RTLIL::Module *const_module = module;

	pool<std::string> حافظه_مزي_ګانې;
	لكل (خپلکاری &it : module->memories) {
		log_assert(it.لومړی == it.دوهم->name);
		log_assert(it.لومړی != IdString::Null);
		log_assert(it.دوهم->پلنوالی >= 0);
		log_assert(it.دوهم->اندازه >= 0);
		check_id_کلیګانې(it.دوهم->ځانګړتیاوې);
		حافظه_مزي_ګانې.ورننباسه(it.دوهم->name.str());
	}

	std::ویکټور<MonotonicFlag> ports_declared(Get_اندازه(module->ports));
	Sharded_ویکټور<std::string> memids(subpool);
	subpool.پرلپسې([const_module, &ports_declared, &حافظه_مزي_ګانې, &memids](ثابت ParallelDispatchThreadPool::پرلپسې_Ctx &ctx) {
		لكل (صحيح i : ctx.توکی_range(const_module->cells_اندازه())) {
			خپلکاری it = *const_module->cells_.element(i);
			log_assert(const_module == it.دوهم->module);
			log_assert(it.لومړی == it.دوهم->name);
			log_assert(it.لومړی != IdString::Null);
			log_assert(!it.دوهم->type.تشه());
			لكل (خپلکاری &it2 : it.دوهم->connections()) {
				log_assert(it2.لومړی != IdString::Null);
				it2.دوهم.check(const_module);
			}
			check_id_کلیګانې(it.دوهم->ځانګړتیاوې);
			check_id_کلیګانې(it.دوهم->parameters);
			دننی_Cell_Checker checker(const_module, it.دوهم);
			checker.check();
			إذا (it.دوهم->has_memid()) {
				log_assert(حافظه_مزي_ګانې.شمېر(it.دوهم->parameters.at(ID::MEMID).decode_string()));
			} وإلا إذا (it.دوهم->is_mem_cell()) {
				std::string memid = it.دوهم->parameters.at(ID::MEMID).decode_string();
				log_assert(!حافظه_مزي_ګانې.شمېر(memid));
				memids.ورننباسه(ctx, std::move(memid));
			}
			خپلکاری cell_mod = const_module->design->module(it.دوهم->name);
			إذا (cell_mod != تش_اشاره) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#که_تعریف_شوی __GNUC__
				#لارښوونه GCC diagnostic ټېل
				#لارښوونه GCC diagnostic ignored "-Wdeprecated-declarations"
				#د_که_پای
				log_assert(!it.دوهم->get_blackbox_attribute());
				#که_تعریف_شوی __GNUC__
				#لارښوونه GCC diagnostic راوباسه
				#د_که_پای
			}
		}

		لكل (صحيح i : ctx.توکی_range(const_module->wires_اندازه())) {
			خپلکاری it = *const_module->wires_.element(i);
			log_assert(const_module == it.دوهم->module);
			log_assert(it.لومړی == it.دوهم->name);
			log_assert(it.لومړی != IdString::Null);
			log_assert(it.دوهم->پلنوالی >= 0);
			log_assert(it.دوهم->پورټ_id >= 0);
			check_id_کلیګانې(it.دوهم->ځانګړتیاوې);
			إذا (it.دوهم->پورټ_id) {
				log_assert(Get_اندازه(const_module->ports) >= it.دوهم->پورټ_id);
				log_assert(const_module->ports.at(it.دوهم->پورټ_id-1) == it.دوهم->name);
				log_assert(it.دوهم->پورټ_ننوت || it.دوهم->پورټ_output);
				log_assert(it.دوهم->پورټ_id <= Get_اندازه(ports_declared));
				منطقي previously_declared = ports_declared[it.دوهم->پورټ_id-1].سېټ_and_return_old();
				log_assert(previously_declared == خطأ);
			} وإلا
				log_assert(!it.دوهم->پورټ_ننوت && !it.دوهم->پورټ_output);
		}
	});
	لكل (ثابت MonotonicFlag &پورټ_declared : ports_declared)
		log_assert(پورټ_declared.بارول() == ریښتیا);
	pool<std::string> memids_pool;
	لكل (std::string &memid : memids)
		log_assert(memids_pool.ورننباسه(memid).دوهم);

	لكل (خپلکاری &it : module->processes) {
		log_assert(it.لومړی == it.دوهم->name);
		log_assert(it.لومړی != IdString::Null);
		log_assert(it.دوهم->ریښه_case.compare.تشه());
		std::ویکټور<RTLIL::Case_قاعده*> all_cases = {&it.دوهم->ریښه_case};
		لكل (اندازه_t i = 0; i < all_cases.اندازه(); i++) {
			لكل (خپلکاری &switch_it : all_cases[i]->switches) {
				لكل (خپلکاری &case_it : switch_it->cases) {
					لكل (خپلکاری &compare_it : case_it->compare) {
						log_assert(switch_it->إشارة.اندازه() == compare_it.اندازه());
					}
					all_cases.ټېل_شا(case_it);
				}
			}
		}
		لكل (خپلکاری &sync_it : it.دوهم->syncs) {
			بدل (sync_it->type) {
				حالة RTLIL::SyncType::ST0:
				حالة RTLIL::SyncType::ST1:
				حالة RTLIL::SyncType::STp:
				حالة RTLIL::SyncType::STn:
				حالة RTLIL::SyncType::STe:
					log_assert(!sync_it->إشارة.تشه());
					اكسر;
				حالة RTLIL::SyncType::STa:
				حالة RTLIL::SyncType::STg:
				حالة RTLIL::SyncType::STi:
					log_assert(sync_it->إشارة.تشه());
					اكسر;
			}
		}
	}

	لكل (خپلکاری &it : module->connections_) {
		log_assert(it.لومړی.اندازه() == it.دوهم.اندازه());
		log_assert(!it.لومړی.has_const());
		it.لومړی.check(module);
		it.دوهم.check(module);
	}

	check_id_کلیګانې(module->ځانګړتیاوې);
#د_که_پای
}

فراغ RTLIL::Module::check()
{
	صحيح pool_اندازه = ThreadPool::work_pool_اندازه(0, cells_اندازه(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_اندازه);
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
	new_mod->parameter_default_ارزښتونه.clear();
	لكل (خپلکاری &it : parameter_default_ارزښتونه)
		new_mod->parameter_default_ارزښتونه[dst_twines.copy_from(design->twines(), it.لومړی)] = it.دوهم;

	لكل (خپلکاری &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->ځانګړتیاوې, ځانګړتیاوې, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	لكل (خپلکاری it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->لومړی);
		wire_map[it->دوهم] = new_mod->جمع_کړه_Wire(dst_id, it->دوهم);
	}

	لكل (خپلکاری it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->لومړی);
		new_mod->جمع_کړه_حافظه(dst_id, it->دوهم);
	}

	لكل (خپلکاری it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->لومړی);
		new_mod->جمع_کړه_Cell(dst_id, it->دوهم);
	}

	لكل (خپلکاری it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->لومړی);
		new_mod->جمع_کړه_Process(dst_id, it->دوهم);
	}

	بنية Rewrite_Sig_Spec_کارګر
	{
		ثابت dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ثابت RTLIL::Module *new_mod;
		فراغ چلوونکی()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([هذا](RTLIL::Wire *&wire) {
				خپلکاری it = wire_map.پیدا_کړه(wire);
				إذا (it != wire_map.پای_ټکی())
					wire = it->دوهم;
				وإلا
					log_assert(wire->module == new_mod);
			});
		}
	};

	Rewrite_Sig_Spec_کارګر rewrite_Sig_Spec_کارګر{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewrite_Sig_Spec_کارګر);
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
	dst->جمع_کړه(new_mod);
	أرجع new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString موخه_name) ثابت
{
	RTLIL::Module *new_mod = جديد RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = موخه_name;
	cloneInto(new_mod);
	dst->جمع_کړه(new_mod);
	أرجع new_mod;
}

منطقي RTLIL::Module::has_memories() ثابت
{
	أرجع !memories.تشه();
}

منطقي RTLIL::Module::has_processes() ثابت
{
	أرجع !processes.تشه();
}

منطقي RTLIL::Module::has_memories_warn() ثابت
{
	إذا (!memories.تشه())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", هذا);
	أرجع !memories.تشه();
}

منطقي RTLIL::Module::has_processes_warn() ثابت
{
	إذا (!processes.تشه())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", هذا);
	أرجع !processes.تشه();
}

منطقي RTLIL::Module::is_selected() ثابت
{
	أرجع design->selected_module(هذا->name);
}

منطقي RTLIL::Module::is_selected_ټولیز() ثابت
{
	أرجع design->selected_ټولیز_module(هذا->name);
}

std::ویکټور<RTLIL::Wire*> RTLIL::Module::selected_wires() ثابت
{
	std::ویکټور<RTLIL::Wire*> پایله;
	پایله.reserve(wires_.اندازه());
	لكل (خپلکاری &it : wires_)
		إذا (design->selected(هذا, it.دوهم))
			پایله.ټېل_شا(it.دوهم);
	أرجع پایله;
}

std::ویکټور<RTLIL::Cell*> RTLIL::Module::selected_cells() ثابت
{
	std::ویکټور<RTLIL::Cell*> پایله;
	پایله.reserve(cells_.اندازه());
	لكل (خپلکاری &it : cells_)
		إذا (design->selected(هذا, it.دوهم))
			پایله.ټېل_شا(it.دوهم);
	أرجع پایله;
}

std::ویکټور<RTLIL::حافظه*> RTLIL::Module::selected_memories() ثابت
{
	std::ویکټور<RTLIL::حافظه*> پایله;
	پایله.reserve(memories.اندازه());
	لكل (خپلکاری &it : memories)
		إذا (design->selected(هذا, it.دوهم))
			پایله.ټېل_شا(it.دوهم);
	أرجع پایله;
}

std::ویکټور<RTLIL::Process*> RTLIL::Module::selected_processes() ثابت
{
	std::ویکټور<RTLIL::Process*> پایله;
	پایله.reserve(processes.اندازه());
	لكل (خپلکاری &it : processes)
		إذا (design->selected(هذا, it.دوهم))
			پایله.ټېل_شا(it.دوهم);
	أرجع پایله;
}

std::ویکټور<RTLIL::SelectedMember> RTLIL::Module::selected_members() ثابت
{
	std::ویکټور<RTLIL::SelectedMember> پایله;
	لكل (خپلکاری cell : selected_cells())
		پایله.ټېل_شا({cell, cell->name});
	لكل (خپلکاری حافظه_3 : selected_memories())
		پایله.ټېل_شا({حافظه_3, حافظه_3->name});
	لكل (خپلکاری wire : selected_wires())
		پایله.ټېل_شا({wire, wire->name});
	لكل (خپلکاری process : selected_processes())
		پایله.ټېل_شا({process, process->name});
	أرجع پایله;
}

فراغ RTLIL::Module::جمع_کړه(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(شمېر_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = هذا;
}

فراغ RTLIL::Module::جمع_کړه(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(شمېر_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = هذا;
}

فراغ RTLIL::Module::جمع_کړه(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(شمېر_id(process->name) == 0);
	processes[process->name] = process;
	process->module = هذا;
}

فراغ RTLIL::Module::remove(ثابت pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	إذا (wires.تشه())
		أرجع;

	بنية Delete_Wire_کارګر
	{
		RTLIL::Module *module;
		ثابت pool<RTLIL::Wire*> *wires_p;

		فراغ چلوونکی()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([هذا](RTLIL::Wire *&wire) {
				إذا (wires_p->شمېر(wire))
					wire = module->جمع_کړه_Wire(stringf("$delete_wire$%d", (صحيح)autoidx++), wire->پلنوالی);
			});
		}

		فراغ چلوونکی()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	Delete_Wire_کارګر delete_wire_کارګر;
	delete_wire_کارګر.module = هذا;
	delete_wire_کارګر.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_کارګر);

	إذا (design->flagBufferedNormalized) {
		لكل (خپلکاری wire : wires) {
			buf_norm_wire_کتار.erase(wire);
			buf_norm_connect_شاخص.erase(wire);
		}
	}

	لكل (خپلکاری &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.شمېر(id) != 0);
		wires_.erase(id);
		ړنګول it;
	}
}

فراغ RTLIL::Module::remove(RTLIL::Cell *cell)
{
	طالما (!cell->connections_.تشه())
		cell->unset_پورټ(cell->connections_.begin()->لومړی);

	log_assert(cells_.شمېر(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	إذا (design && design->flagBufferedNormalized && buf_norm_cell_کتار.شمېر(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.ورننباسه(cell);
	} وإلا {
		ړنګول cell;
	}
}

فراغ RTLIL::Module::remove(RTLIL::حافظه *حافظه_3)
{
	log_assert(memories.شمېر(حافظه_3->name) != 0);
	memories.erase(حافظه_3->name);
	ړنګول حافظه_3;
}

فراغ RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.شمېر(process->name) != 0);
	processes.erase(process->name);
	ړنګول process;
}

فراغ RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	جمع_کړه(wire);
}

فراغ RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	جمع_کړه(cell);
}

فراغ RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	إذا (old_id != IdString::Null && wires_.شمېر(old_id))
		rename(wires_.at(old_id), new_name);
	وإلا إذا (old_id != IdString::Null && cells_.شمېر(old_id))
		rename(cells_.at(old_id), new_name);
	وإلا
		log_abort();
}

فراغ RTLIL::Module::بدلون_نومونه(RTLIL::Wire *w1, RTLIL::Wire *w2)
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
	بدلون(w1->name, w2->name);
}

فراغ RTLIL::Module::بدلون_نومونه(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	بدلون(c1->name, c2->name);
}

RTLIL::IdString RTLIL::Module::uniquify(RTLIL::IdString name)
{
	صحيح شاخص = 0;
	أرجع uniquify(name, شاخص);
}

IdString RTLIL::Module::uniquify(IdString name, صحيح &شاخص)
{
	إذا (شاخص == 0) {
		إذا (شمېر_id(name) == 0)
			أرجع name;
		شاخص++;
	}

	طالما (1) {
		IdString new_name = design->twines().جمع_کړه(TwineSpec::Suffix{name, stringf("_%d", شاخص)});
		إذا (شمېر_id(new_name) == 0)
			أرجع new_name;
		شاخص++;
	}
}

ساكن منطقي fixup_ports_compare(ثابت RTLIL::Wire *a, ثابت RTLIL::Wire *b)
{
	إذا (a->پورټ_id && !b->پورټ_id)
		أرجع ریښتیا;
	إذا (!a->پورټ_id && b->پورټ_id)
		أرجع خطأ;

	إذا (a->پورټ_id == b->پورټ_id)
		أرجع a->name < b->name;
	أرجع a->پورټ_id < b->پورټ_id;
}

فراغ RTLIL::Module::connect(ثابت RTLIL::SigSig &conn)
{
	لكل (خپلکاری mon : monitors)
		mon->notify_connect(هذا, conn);

	إذا (design)
		لكل (خپلکاری mon : design->monitors)
			mon->notify_connect(هذا, conn);

	// ignore all attempts to assign constants to other constants
	إذا (conn.لومړی.has_const()) {
		RTLIL::SigSig new_conn;
		لكل (صحيح i = 0; i < Get_اندازه(conn.لومړی); i++)
			إذا (conn.لومړی[i].wire) {
				new_conn.لومړی.append(conn.لومړی[i]);
				new_conn.دوهم.append(conn.دوهم[i]);
			}
		إذا (Get_اندازه(new_conn.لومړی))
			connect(new_conn);
		أرجع;
	}

	إذا (yosys_xtrace) {
		لوګاریتم("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", هذا, log_signal(conn.لومړی), log_signal(conn.دوهم), Get_اندازه(conn.لومړی));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(Get_اندازه(conn.لومړی) == Get_اندازه(conn.دوهم));
	connections_.ټېل_شا(conn);
}

فراغ RTLIL::Module::connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

فراغ RTLIL::Module::new_connections(ثابت std::ویکټور<RTLIL::SigSig> &new_conn)
{
	لكل (خپلکاری mon : monitors)
		mon->notify_connect(هذا, new_conn);

	إذا (design)
		لكل (خپلکاری mon : design->monitors)
			mon->notify_connect(هذا, new_conn);

	إذا (yosys_xtrace) {
		لوګاریتم("#X# New connections vector in %s:\n", هذا);
		لكل (خپلکاری &conn: new_conn)
			لوګاریتم("#X#    %s = %s (%d bits)\n", log_signal(conn.لومړی), log_signal(conn.دوهم), Get_اندازه(conn.لومړی));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ثابت std::ویکټور<RTLIL::SigSig> &RTLIL::Module::connections() ثابت
{
	أرجع connections_;
}

فراغ RTLIL::Module::fixup_ports()
{
	std::ویکټور<RTLIL::Wire*> all_ports;

	لكل (خپلکاری &w : wires_)
		إذا (w.دوهم->پورټ_ننوت || w.دوهم->پورټ_output)
			all_ports.ټېل_شا(w.دوهم);
		وإلا
			w.دوهم->پورټ_id = 0;

	std::ترتیبول(all_ports.begin(), all_ports.پای_ټکی(), fixup_ports_compare);

	إذا (design && design->flagBufferedNormalized) {
		لكل (خپلکاری &w : wires_)
			إذا (w.دوهم->driverCell_ && w.دوهم->driverCell_->type == ID($ننوت_پورټ))
				buf_norm_wire_کتار.ورننباسه(w.دوهم);

		buf_norm_wire_کتار.ورننباسه(all_ports.begin(), all_ports.پای_ټکی());
	}

	ports.clear();
	لكل (اندازه_t i = 0; i < all_ports.اندازه(); i++) {
		ports.ټېل_شا(all_ports[i]->name);
		all_ports[i]->پورټ_id = i+1;
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
	لكل (صحيح i = Get_اندازه(src) - 1; i >= 0; i--) {
		خپلکاری &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.لومړی)] = it.دوهم;
	}
}

RTLIL::Wire *RTLIL::Module::جمع_کړه_Wire(RTLIL::IdString name, صحيح پلنوالی)
{
	log_assert(design);
	log_assert(پلنوالی >= 0 && پلنوالی < RTLIL::پلنوالی_حد);
	RTLIL::Wire *wire = جديد RTLIL::Wire;
	wire->پلنوالی = پلنوالی;
	wire->name = name;
	جمع_کړه(wire);
	أرجع wire;
}

RTLIL::Wire *RTLIL::Module::جمع_کړه_Wire(IdString name, ثابت RTLIL::Wire *other)
{
	RTLIL::Wire *wire = جمع_کړه_Wire(name);
	wire->پلنوالی = other->پلنوالی;
	wire->پیل_دننه_واټن = other->پیل_دننه_واټن;
	wire->پورټ_id = other->پورټ_id;
	wire->پورټ_ننوت = other->پورټ_ننوت;
	wire->پورټ_output = other->پورټ_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ثابت RTLIL::Design *src_design = other->module ? other->module->design : تش_اشاره;
		RTLIL::copy_attr_dict(wire->ځانګړتیاوې, other->ځانګړتیاوې, src_design, هذا->design);
	}
	أرجع wire;
}

RTLIL::Cell *RTLIL::Module::جمع_کړه_Cell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = جديد RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	جمع_کړه(cell);
	أرجع cell;
}

RTLIL::Cell *RTLIL::Module::جمع_کړه_Cell(IdString name, ثابت RTLIL::Cell *other)
{
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : تش_اشاره;
	منطقي cross_pool = src_design && هذا->design && src_design != هذا->design;

	IdString type = other->type.ref();
	إذا (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = جمع_کړه_Cell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, هذا->design);
	RTLIL::copy_attr_dict(cell->ځانګړتیاوې, other->ځانګړتیاوې, src_design, هذا->design);

	إذا (!cross_pool)
		cell->connections_ = other->connections_;
	وإلا
		لكل (صحيح i = Get_اندازه(other->connections_) - 1; i >= 0; i--) {
			خپلکاری &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.لومړی)] = c.دوهم;
		}
	أرجع cell;
}

RTLIL::حافظه *RTLIL::Module::جمع_کړه_حافظه(IdString name)
{
	log_assert(design);
	RTLIL::حافظه *mem = جديد RTLIL::حافظه;
	mem->module = هذا;
	mem->name = name;
	memories[name] = mem;
	أرجع mem;
}

RTLIL::حافظه *RTLIL::Module::جمع_کړه_حافظه(IdString name, ثابت RTLIL::حافظه *other)
{
	log_assert(design);
	RTLIL::حافظه *mem = جديد RTLIL::حافظه;
	mem->module = هذا;
	mem->name = name;
	mem->پلنوالی = other->پلنوالی;
	mem->پیل_دننه_واټن = other->پیل_دننه_واټن;
	mem->اندازه = other->اندازه;
	RTLIL::copy_attr_dict(mem->ځانګړتیاوې, other->ځانګړتیاوې,
			other->module ? other->module->design : تش_اشاره, design);
	memories[name] = mem;
	أرجع mem;
}

RTLIL::Process *RTLIL::Module::جمع_کړه_Process(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = جديد RTLIL::Process;
	proc->name = name;
	جمع_کړه(proc);
	أرجع proc;
}

RTLIL::Process *RTLIL::Module::جمع_کړه_Process(RTLIL::IdString name, ثابت RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : تش_اشاره;
	إذا (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> ځانګړتیاوې;
			RTLIL::copy_attr_dict(ځانګړتیاوې, obj->ځانګړتیاوې, src_design, design);
			obj->ځانګړتیاوې = std::move(ځانګړتیاوې);
		});
		لكل (خپلکاری *متزامن : proc->syncs)
			لكل (خپلکاری &mwa : متزامن->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	جمع_کړه(proc);
	أرجع proc;
}

#تعریف_کړه DEF_لاره(_func, y_اندازه, _type) \
	RTLIL::Cell* RTLIL::Module::جمع_کړه ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کړه_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_پلنوالی] = sig_a.اندازه();       \
		cell->parameters[ID::Y_پلنوالی] = sig_y.اندازه();       \
		cell->سېټ_پورټ(ID::A, sig_a);                        \
		cell->سېټ_پورټ(ID::Y, sig_y);                        \
		cell->سېټ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کړه_Wire(NEW_ID, y_اندازه);    \
		جمع_کړه ## _func(name, sig_a, sig_y, is_signed, src);   \
		أرجع sig_y;                                       \
	}
DEF_لاره(Not,        sig_a.اندازه(), ID($not))
DEF_لاره(Pos,        sig_a.اندازه(), ID($pos))
DEF_لاره(Neg,        sig_a.اندازه(), ID($neg))
DEF_لاره(ReduceAnd,  1, ID($reduce_and))
DEF_لاره(ReduceOr,   1, ID($reduce_or))
DEF_لاره(ReduceXor,  1, ID($reduce_xor))
DEF_لاره(ReduceXnor, 1, ID($reduce_xnor))
DEF_لاره(ReduceBool, 1, ID($reduce_bool))
DEF_لاره(LogicNot,   1, ID($logic_not))
#تعریف_لغوه DEF_لاره

#تعریف_کړه DEF_لاره(_func, y_اندازه, _type) \
	RTLIL::Cell* RTLIL::Module::جمع_کړه ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقي /* is_signed */, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کړه_Cell(name, _type);           \
		cell->parameters[ID::پلنوالی_2] = sig_a.اندازه();         \
		cell->سېټ_پورټ(ID::A, sig_a);                        \
		cell->سېټ_پورټ(ID::Y, sig_y);                        \
		cell->سېټ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کړه_Wire(NEW_ID, y_اندازه);    \
		جمع_کړه ## _func(name, sig_a, sig_y, is_signed, src);   \
		أرجع sig_y;                                       \
	}
DEF_لاره(Buf,     sig_a.اندازه(), ID($buf))
DEF_لاره(Barrier, sig_a.اندازه(), ID($barrier))
#تعریف_لغوه DEF_لاره

#تعریف_کړه DEF_لاره(_func, y_اندازه, _type) \
	RTLIL::Cell* RTLIL::Module::جمع_کړه ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کړه_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_پلنوالی] = sig_a.اندازه();       \
		cell->parameters[ID::B_پلنوالی] = sig_b.اندازه();       \
		cell->parameters[ID::Y_پلنوالی] = sig_y.اندازه();       \
		cell->سېټ_پورټ(ID::A, sig_a);                        \
		cell->سېټ_پورټ(ID::B, sig_b);                        \
		cell->سېټ_پورټ(ID::Y, sig_y);                        \
		cell->سېټ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کړه_Wire(NEW_ID, y_اندازه);         \
		جمع_کړه ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_لاره(And,      max(sig_a.اندازه(), sig_b.اندازه()), ID($and))
DEF_لاره(Or,       max(sig_a.اندازه(), sig_b.اندازه()), ID($or))
DEF_لاره(Xor,      max(sig_a.اندازه(), sig_b.اندازه()), ID($xor))
DEF_لاره(Xnor,     max(sig_a.اندازه(), sig_b.اندازه()), ID($xnor))
DEF_لاره(ښويدنه_2,    sig_a.اندازه(), ID($ښويدنه))
DEF_لاره(Lt,       1, ID($lt))
DEF_لاره(Le,       1, ID($le))
DEF_لاره(Eq,       1, ID($eq))
DEF_لاره(Ne,       1, ID($ne))
DEF_لاره(Eqx,      1, ID($eqx))
DEF_لاره(Nex,      1, ID($nex))
DEF_لاره(Ge,       1, ID($ge))
DEF_لاره(Gt,       1, ID($gt))
DEF_لاره(جمع_کړه_2,      max(sig_a.اندازه(), sig_b.اندازه()), ID($جمع_کړه))
DEF_لاره(Sub,      max(sig_a.اندازه(), sig_b.اندازه()), ID($sub))
DEF_لاره(Mul,      max(sig_a.اندازه(), sig_b.اندازه()), ID($mul))
DEF_لاره(Div,      max(sig_a.اندازه(), sig_b.اندازه()), ID($div))
DEF_لاره(Mod,      max(sig_a.اندازه(), sig_b.اندازه()), ID($mod))
DEF_لاره(DivFloor, max(sig_a.اندازه(), sig_b.اندازه()), ID($divfloor))
DEF_لاره(ModFloor, max(sig_a.اندازه(), sig_b.اندازه()), ID($modfloor))
DEF_لاره(LogicAnd, 1, ID($logic_and))
DEF_لاره(LogicOr,  1, ID($logic_or))
#تعریف_لغوه DEF_لاره

#تعریف_کړه DEF_لاره(_func, y_اندازه, _type) \
	RTLIL::Cell* RTLIL::Module::جمع_کړه ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کړه_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = خطأ;             \
		cell->parameters[ID::A_پلنوالی] = sig_a.اندازه();       \
		cell->parameters[ID::B_پلنوالی] = sig_b.اندازه();       \
		cell->parameters[ID::Y_پلنوالی] = sig_y.اندازه();       \
		cell->سېټ_پورټ(ID::A, sig_a);                        \
		cell->سېټ_پورټ(ID::B, sig_b);                        \
		cell->سېټ_پورټ(ID::Y, sig_y);                        \
		cell->سېټ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کړه_Wire(NEW_ID, y_اندازه);         \
		جمع_کړه ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_لاره(Shl,      sig_a.اندازه(), ID($shl))
DEF_لاره(Shr,      sig_a.اندازه(), ID($shr))
DEF_لاره(Sshl,     sig_a.اندازه(), ID($sshl))
DEF_لاره(Sshr,     sig_a.اندازه(), ID($sshr))
#تعریف_لغوه DEF_لاره

#تعریف_کړه DEF_لاره(_func, y_اندازه, _type) \
	RTLIL::Cell* RTLIL::Module::جمع_کړه ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کړه_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = خطأ;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_پلنوالی] = sig_a.اندازه();       \
		cell->parameters[ID::B_پلنوالی] = sig_b.اندازه();       \
		cell->parameters[ID::Y_پلنوالی] = sig_y.اندازه();       \
		cell->سېټ_پورټ(ID::A, sig_a);                        \
		cell->سېټ_پورټ(ID::B, sig_b);                        \
		cell->سېټ_پورټ(ID::Y, sig_y);                        \
		cell->سېټ_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کړه_Wire(NEW_ID, y_اندازه);         \
		جمع_کړه ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_لاره(Shiftx,      sig_a.اندازه(), ID($shiftx))
#تعریف_لغوه DEF_لاره

#تعریف_کړه DEF_لاره(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::جمع_کړه ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کړه_Cell(name, _type);                 \
		cell->parameters[ID::پلنوالی_2] = sig_a.اندازه();               \
		إذا (_pmux) cell->parameters[ID::S_پلنوالی] = sig_s.اندازه();  \
		cell->سېټ_پورټ(ID::A, sig_a);                              \
		cell->سېټ_پورټ(ID::B, sig_b);                              \
		cell->سېټ_پورټ(ID::S, sig_s);                              \
		cell->سېټ_پورټ(ID::Y, sig_y);                              \
		cell->سېټ_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کړه_Wire(NEW_ID, sig_a.اندازه());     \
		جمع_کړه ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		أرجع sig_y;                                             \
	}
DEF_لاره(Mux,      ID($mux),        0)
DEF_لاره(Bwmux,    ID($bwmux),      0)
DEF_لاره(Pmux,     ID($pmux),       1)
#تعریف_لغوه DEF_لاره

#تعریف_کړه DEF_لاره(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::جمع_کړه ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کړه_Cell(name, _type);                 \
		cell->parameters[ID::پلنوالی_2] = _demux ? sig_a.اندازه() : sig_y.اندازه(); \
		cell->parameters[ID::S_پلنوالی] = sig_s.اندازه();             \
		cell->سېټ_پورټ(ID::A, sig_a);                              \
		cell->سېټ_پورټ(ID::S, sig_s);                              \
		cell->سېټ_پورټ(ID::Y, sig_y);                              \
		cell->سېټ_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کړه_Wire(NEW_ID, _demux ? sig_a.اندازه() << sig_s.اندازه() : sig_a.اندازه() >> sig_s.اندازه()); \
		جمع_کړه ## _func(name, sig_a, sig_s, sig_y, src);             \
		أرجع sig_y;                                             \
	}
DEF_لاره(Bmux,     ID($bmux),       0)
DEF_لاره(Demux,    ID($demux),      1)
#تعریف_لغوه DEF_لاره

#تعریف_کړه DEF_لاره(_func, _type) \
	RTLIL::Cell* RTLIL::Module::جمع_کړه ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کړه_Cell(name, _type);                 \
		cell->parameters[ID::پلنوالی_2] = sig_a.اندازه();               \
		cell->سېټ_پورټ(ID::A, sig_a);                              \
		cell->سېټ_پورټ(ID::B, sig_b);                              \
		cell->سېټ_پورټ(ID::Y, sig_y);                              \
		cell->سېټ_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کړه_Wire(NEW_ID, sig_a.اندازه());     \
		جمع_کړه ## _func(name, sig_a, sig_s, sig_y, src);             \
		أرجع sig_y;                                             \
	}
DEF_لاره(Bweqx,    ID($bweqx))
#تعریف_لغوه DEF_لاره

#تعریف_کړه DEF_لاره_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::جمع_کړه ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig1, ثابت RTLIL::Sig_بټ &sig2, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کړه_Cell(name, _type);         \
		cell->سېټ_پورټ("\\" #_P1, sig1);                   \
		cell->سېټ_پورټ("\\" #_P2, sig2);                   \
		cell->سېټ_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_بټ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig1, ثابت std::string &src) { \
		RTLIL::Sig_بټ sig2 = جمع_کړه_Wire(NEW_ID);             \
		جمع_کړه ## _func(name, sig1, sig2, src);              \
		أرجع sig2;                                      \
	}
#تعریف_کړه DEF_لاره_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::جمع_کړه ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig1, ثابت RTLIL::Sig_بټ &sig2, ثابت RTLIL::Sig_بټ &sig3, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کړه_Cell(name, _type);         \
		cell->سېټ_پورټ("\\" #_P1, sig1);                   \
		cell->سېټ_پورټ("\\" #_P2, sig2);                   \
		cell->سېټ_پورټ("\\" #_P3, sig3);                   \
		cell->سېټ_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_بټ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig1, ثابت RTLIL::Sig_بټ &sig2, ثابت std::string &src) { \
		RTLIL::Sig_بټ sig3 = جمع_کړه_Wire(NEW_ID);             \
		جمع_کړه ## _func(name, sig1, sig2, sig3, src);        \
		أرجع sig3;                                      \
	}
#تعریف_کړه DEF_لاره_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::جمع_کړه ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig1, ثابت RTLIL::Sig_بټ &sig2, ثابت RTLIL::Sig_بټ &sig3, ثابت RTLIL::Sig_بټ &sig4, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کړه_Cell(name, _type);         \
		cell->سېټ_پورټ("\\" #_P1, sig1);                   \
		cell->سېټ_پورټ("\\" #_P2, sig2);                   \
		cell->سېټ_پورټ("\\" #_P3, sig3);                   \
		cell->سېټ_پورټ("\\" #_P4, sig4);                   \
		cell->سېټ_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_بټ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig1, ثابت RTLIL::Sig_بټ &sig2, ثابت RTLIL::Sig_بټ &sig3, ثابت std::string &src) { \
		RTLIL::Sig_بټ sig4 = جمع_کړه_Wire(NEW_ID);             \
		جمع_کړه ## _func(name, sig1, sig2, sig3, sig4, src);  \
		أرجع sig4;                                      \
	}
#تعریف_کړه DEF_لاره_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::جمع_کړه ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig1, ثابت RTLIL::Sig_بټ &sig2, ثابت RTLIL::Sig_بټ &sig3, ثابت RTLIL::Sig_بټ &sig4, ثابت RTLIL::Sig_بټ &sig5, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کړه_Cell(name, _type);         \
		cell->سېټ_پورټ("\\" #_P1, sig1);                   \
		cell->سېټ_پورټ("\\" #_P2, sig2);                   \
		cell->سېټ_پورټ("\\" #_P3, sig3);                   \
		cell->سېټ_پورټ("\\" #_P4, sig4);                   \
		cell->سېټ_پورټ("\\" #_P5, sig5);                   \
		cell->سېټ_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_بټ RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig1, ثابت RTLIL::Sig_بټ &sig2, ثابت RTLIL::Sig_بټ &sig3, ثابت RTLIL::Sig_بټ &sig4, ثابت std::string &src) { \
		RTLIL::Sig_بټ sig5 = جمع_کړه_Wire(NEW_ID);                  \
		جمع_کړه ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		أرجع sig5;                                           \
	}
DEF_لاره_2(BufGate,    ID($_BUF_),    A, Y)
DEF_لاره_2(NotGate,    ID($_NOT_),    A, Y)
DEF_لاره_3(AndGate,    ID($_AND_),    A, B, Y)
DEF_لاره_3(NandGate,   ID($_NAND_),   A, B, Y)
DEF_لاره_3(OrGate,     ID($_OR_),     A, B, Y)
DEF_لاره_3(NorGate,    ID($_NOR_),    A, B, Y)
DEF_لاره_3(XorGate,    ID($_XOR_),    A, B, Y)
DEF_لاره_3(XnorGate,   ID($_XNOR_),   A, B, Y)
DEF_لاره_3(AndnotGate, ID($_ANDNOT_), A, B, Y)
DEF_لاره_3(OrnotGate,  ID($_ORNOT_),  A, B, Y)
DEF_لاره_4(MuxGate,    ID($_MUX_),    A, B, S, Y)
DEF_لاره_4(NmuxGate,   ID($_NMUX_),   A, B, S, Y)
DEF_لاره_4(Aoi3Gate,   ID($_AOI3_),   A, B, C, Y)
DEF_لاره_4(Oai3Gate,   ID($_OAI3_),   A, B, C, Y)
DEF_لاره_5(Aoi4Gate,   ID($_AOI4_),   A, B, C, D, Y)
DEF_لاره_5(Oai4Gate,   ID($_OAI4_),   A, B, C, D, Y)
#تعریف_لغوه DEF_لاره_2
#تعریف_لغوه DEF_لاره_3
#تعریف_لغوه DEF_لاره_4
#تعریف_لغوه DEF_لاره_5

RTLIL::Cell* RTLIL::Module::جمع_کړه_Pow(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي a_signed, منطقي b_signed, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($توان));
	cell->parameters[ID::A_SIGNED] = a_signed;
	cell->parameters[ID::B_SIGNED] = b_signed;
	cell->parameters[ID::A_پلنوالی] = sig_a.اندازه();
	cell->parameters[ID::B_پلنوالی] = sig_b.اندازه();
	cell->parameters[ID::Y_پلنوالی] = sig_y.اندازه();
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::B, sig_b);
	cell->سېټ_پورټ(ID::Y, sig_y);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Fa(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_x, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($fa));
	cell->parameters[ID::پلنوالی_2] = sig_a.اندازه();
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::B, sig_b);
	cell->سېټ_پورټ(ID::C, sig_c);
	cell->سېټ_پورټ(ID::X, sig_x);
	cell->سېټ_پورټ(ID::Y, sig_y);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_ټوټه(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const دننه_واټن, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($ټوټه));
	cell->parameters[ID::A_پلنوالی] = sig_a.اندازه();
	cell->parameters[ID::Y_پلنوالی] = sig_y.اندازه();
	cell->parameters[ID::دننه_واټن_2] = دننه_واټن;
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::Y, sig_y);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Concat(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($concat));
	cell->parameters[ID::A_پلنوالی] = sig_a.اندازه();
	cell->parameters[ID::B_پلنوالی] = sig_b.اندازه();
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::B, sig_b);
	cell->سېټ_پورټ(ID::Y, sig_y);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Lut(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::پلنوالی_2] = sig_a.اندازه();
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::Y, sig_y);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Tribuf(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($tribuf));
	cell->parameters[ID::پلنوالی_2] = sig_a.اندازه();
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_پورټ(ID::Y, sig_y);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Assert(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($أكد));
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Assume(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($assume));
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_ژوندی(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($ژوندی));
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Fair(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($fair));
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Cover(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($cover));
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Equiv(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($equiv));
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::B, sig_b);
	cell->سېټ_پورټ(ID::Y, sig_y);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Sr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr, ثابت RTLIL::SigSpec &sig_q, منطقي سېټ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($sr));
	cell->parameters[ID::سېټ_POLARITY] = سېټ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::سېټ_2, sig_سېټ);
	cell->سېټ_پورټ(ID::CLR, sig_clr);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Ff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($ff));
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Dff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::CLK, sig_clk);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Dffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($dffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::CLK, sig_clk);
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Dffsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي سېټ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($dffsr));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::سېټ_POLARITY] = سېټ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::CLK, sig_clk);
	cell->سېټ_پورټ(ID::سېټ_2, sig_سېټ);
	cell->سېټ_پورټ(ID::CLR, sig_clr);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Dffsre(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, منطقي سېټ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($dffsre));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::سېټ_POLARITY] = سېټ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::CLK, sig_clk);
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_پورټ(ID::سېټ_2, sig_سېټ);
	cell->سېټ_پورټ(ID::CLR, sig_clr);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Adff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ارزښت, منطقي clk_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ارزښت] = arst_ارزښت;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::CLK, sig_clk);
	cell->سېټ_پورټ(ID::ARST, sig_arst);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Adffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ارزښت, منطقي clk_polarity, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ارزښت] = arst_ارزښت;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::CLK, sig_clk);
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_پورټ(ID::ARST, sig_arst);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Aldff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($aldff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::CLK, sig_clk);
	cell->سېټ_پورټ(ID::ALOAD, sig_aload);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::AD, sig_ad);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Aldffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي en_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($aldffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::CLK, sig_clk);
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_پورټ(ID::ALOAD, sig_aload);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::AD, sig_ad);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Sdff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ارزښت, منطقي clk_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ارزښت] = srst_ارزښت;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::CLK, sig_clk);
	cell->سېټ_پورټ(ID::SRST, sig_srst);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Sdffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ارزښت, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ارزښت] = srst_ارزښت;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::CLK, sig_clk);
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_پورټ(ID::SRST, sig_srst);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Sdffce(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ارزښت, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ارزښت] = srst_ارزښت;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::CLK, sig_clk);
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_پورټ(ID::SRST, sig_srst);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Dlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Adlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ارزښت, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ارزښت] = arst_ارزښت;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_پورټ(ID::ARST, sig_arst);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Dlatchsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, منطقي سېټ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($dlatchsr));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::سېټ_POLARITY] = سېټ_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::EN, sig_en);
	cell->سېټ_پورټ(ID::سېټ_2, sig_سېټ);
	cell->سېټ_پورټ(ID::CLR, sig_clr);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Sr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr,
		ثابت RTLIL::SigSpec &sig_q, منطقي سېټ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_SR_%c%c_", سېټ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->سېټ_پورټ(ID::S, sig_سېټ);
	cell->سېټ_پورټ(ID::R, sig_clr);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Ff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($_FF_));
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Dff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->سېټ_پورټ(ID::C, sig_clk);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Dffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->سېټ_پورټ(ID::C, sig_clk);
	cell->سېټ_پورټ(ID::E, sig_en);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Dffsr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي سېټ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', سېټ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->سېټ_پورټ(ID::C, sig_clk);
	cell->سېټ_پورټ(ID::S, sig_سېټ);
	cell->سېټ_پورټ(ID::R, sig_clr);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Dffsre_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, منطقي سېټ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', سېټ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->سېټ_پورټ(ID::C, sig_clk);
	cell->سېټ_پورټ(ID::S, sig_سېټ);
	cell->سېټ_پورټ(ID::R, sig_clr);
	cell->سېټ_پورټ(ID::E, sig_en);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Adff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_ارزښت, منطقي clk_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ارزښت ? '1' : '0'));
	cell->سېټ_پورټ(ID::C, sig_clk);
	cell->سېټ_پورټ(ID::R, sig_arst);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Adffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_ارزښت, منطقي clk_polarity, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ارزښت ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->سېټ_پورټ(ID::C, sig_clk);
	cell->سېټ_پورټ(ID::R, sig_arst);
	cell->سېټ_پورټ(ID::E, sig_en);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Aldff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->سېټ_پورټ(ID::C, sig_clk);
	cell->سېټ_پورټ(ID::L, sig_aload);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::AD, sig_ad);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Aldffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي en_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->سېټ_پورټ(ID::C, sig_clk);
	cell->سېټ_پورټ(ID::L, sig_aload);
	cell->سېټ_پورټ(ID::E, sig_en);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::AD, sig_ad);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Sdff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_ارزښت, منطقي clk_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ارزښت ? '1' : '0'));
	cell->سېټ_پورټ(ID::C, sig_clk);
	cell->سېټ_پورټ(ID::R, sig_srst);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Sdffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_ارزښت, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ارزښت ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->سېټ_پورټ(ID::C, sig_clk);
	cell->سېټ_پورټ(ID::R, sig_srst);
	cell->سېټ_پورټ(ID::E, sig_en);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Sdffce_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_ارزښت, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ارزښت ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->سېټ_پورټ(ID::C, sig_clk);
	cell->سېټ_پورټ(ID::R, sig_srst);
	cell->سېټ_پورټ(ID::E, sig_en);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Dlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->سېټ_پورټ(ID::E, sig_en);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Adlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_ارزښت, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ارزښت ? '1' : '0'));
	cell->سېټ_پورټ(ID::E, sig_en);
	cell->سېټ_پورټ(ID::R, sig_arst);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Dlatchsr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, منطقي سېټ_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', سېټ_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->سېټ_پورټ(ID::E, sig_en);
	cell->سېټ_پورټ(ID::S, sig_سېټ);
	cell->سېټ_پورټ(ID::R, sig_clr);
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Anyinit(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($anyinit));
	cell->parameters[ID::پلنوالی_2] = sig_q.اندازه();
	cell->سېټ_پورټ(ID::D, sig_d);
	cell->سېټ_پورټ(ID::Q, sig_q);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, صحيح پلنوالی, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کړه_Wire(NEW_ID, پلنوالی);
	Cell *cell = جمع_کړه_Cell(name, ID($anyconst));
	cell->سېټ_Param(ID::پلنوالی_2, پلنوالی);
	cell->سېټ_پورټ(ID::Y, sig);
	cell->سېټ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, صحيح پلنوالی, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کړه_Wire(NEW_ID, پلنوالی);
	Cell *cell = جمع_کړه_Cell(name, ID($anyseq));
	cell->سېټ_Param(ID::پلنوالی_2, پلنوالی);
	cell->سېټ_پورټ(ID::Y, sig);
	cell->سېټ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, صحيح پلنوالی, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کړه_Wire(NEW_ID, پلنوالی);
	Cell *cell = جمع_کړه_Cell(name, ID($allconst));
	cell->سېټ_Param(ID::پلنوالی_2, پلنوالی);
	cell->سېټ_پورټ(ID::Y, sig);
	cell->سېټ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, صحيح پلنوالی, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کړه_Wire(NEW_ID, پلنوالی);
	Cell *cell = جمع_کړه_Cell(name, ID($allseq));
	cell->سېټ_Param(ID::پلنوالی_2, پلنوالی);
	cell->سېټ_پورټ(ID::Y, sig);
	cell->سېټ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کړه_Wire(NEW_ID);
	Cell *cell = جمع_کړه_Cell(name, ID($initstate));
	cell->سېټ_پورټ(ID::Y, sig);
	cell->سېټ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::سېټ_نښان_2(RTLIL::IdString name, ثابت std::string &نښان, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کړه_Wire(NEW_ID, sig_a.اندازه());
	Cell *cell = جمع_کړه_Cell(name, ID($سېټ_نښان));
	cell->parameters[ID::پلنوالی_2] = sig_a.اندازه();
	cell->parameters[ID::نښان_2] = نښان;
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::سېټ_2, sig_s);
	cell->سېټ_پورټ(ID::CLR, sig_c);
	cell->سېټ_پورټ(ID::Y, sig);
	cell->سېټ_src_attribute(src);
	أرجع sig;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_سېټ_نښان(RTLIL::IdString name, ثابت std::string &نښان, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	Cell *cell = جمع_کړه_Cell(name, ID($سېټ_نښان));
	cell->parameters[ID::پلنوالی_2] = sig_a.اندازه();
	cell->parameters[ID::نښان_2] = نښان;
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::سېټ_2, sig_s);
	cell->سېټ_پورټ(ID::CLR, sig_c);
	cell->سېټ_پورټ(ID::Y, sig_y);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::Get_نښان(RTLIL::IdString name, ثابت std::string &نښان, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کړه_Wire(NEW_ID, sig_a.اندازه());
	Cell *cell = جمع_کړه_Cell(name, ID($get_نښان));
	cell->parameters[ID::پلنوالی_2] = sig_a.اندازه();
	cell->parameters[ID::نښان_2] = نښان;
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::Y, sig);
	cell->سېټ_src_attribute(src);
	أرجع sig;
}

RTLIL::Cell* RTLIL::Module::جمع_کړه_Overwrite_نښان(RTLIL::IdString name, ثابت std::string &نښان, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کړه_Cell(name, ID($overwrite_نښان));
	cell->parameters[ID::پلنوالی_2] = sig_a.اندازه();
	cell->parameters[ID::نښان_2] = نښان;
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::سېټ_2, sig_s);
	cell->سېټ_پورټ(ID::CLR, sig_c);
	cell->سېټ_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::Original_نښان(RTLIL::IdString name, ثابت std::string &نښان, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کړه_Wire(NEW_ID, sig_a.اندازه());
	Cell *cell = جمع_کړه_Cell(name, ID($original_نښان));
	cell->parameters[ID::پلنوالی_2] = sig_a.اندازه();
	cell->parameters[ID::نښان_2] = نښان;
	cell->سېټ_پورټ(ID::A, sig_a);
	cell->سېټ_پورټ(ID::Y, sig);
	cell->سېټ_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کړه_Wire(NEW_ID, sig_e.اندازه());
	Cell *cell = جمع_کړه_Cell(name, ID($future_ff));
	cell->parameters[ID::پلنوالی_2] = sig_e.اندازه();
	cell->سېټ_پورټ(ID::A, sig_e);
	cell->سېټ_پورټ(ID::Y, sig);
	cell->سېټ_src_attribute(src);
	أرجع sig;
}

std::string RTLIL::Module::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(هذا), design, خطأ, ریښتیا, خطأ, RTLIL_BACKEND::Dump_ډېرپېښ::Readable);
	أرجع f.str();
}

RTLIL::Wire::Wire()
{
	ساكن بینښه صحيح hashidx_شمېر = 123456789;
	hashidx_شمېر = mkhash_xorshift(hashidx_شمېر);
	hashidx_ = hashidx_شمېر;

	module = تش_اشاره;
	پلنوالی = 1;
	پیل_دننه_واټن = 0;
	پورټ_id = 0;
	پورټ_ننوت = خطأ;
	پورټ_output = خطأ;
	upto = خطأ;
	is_signed = خطأ;

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->ورننباسه(std::pair<بینښه صحيح, RTLIL::Wire*>(hashidx_, هذا));
#د_که_پای
}

RTLIL::Wire::~Wire()
{
#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#د_که_پای
}

std::string RTLIL::Wire::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", هذا, design(), RTLIL_BACKEND::Dump_ډېرپېښ::Readable);
	أرجع f.str();
}

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
ساكن std::map<بینښه صحيح, RTLIL::Wire*> all_wires;
std::map<بینښه صحيح, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(فراغ)
{
	أرجع &all_wires;
}
#د_که_پای

RTLIL::حافظه::حافظه()
{
	ساكن بینښه صحيح hashidx_شمېر = 123456789;
	hashidx_شمېر = mkhash_xorshift(hashidx_شمېر);
	hashidx_ = hashidx_شمېر;

	پلنوالی = 1;
	پیل_دننه_واټن = 0;
	اندازه = 0;
#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	RTLIL::حافظه::get_all_memorys()->ورننباسه(std::pair<بینښه صحيح, RTLIL::حافظه*>(hashidx_, هذا));
#د_که_پای
}

std::string RTLIL::حافظه::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_حافظه(f, "", هذا, design(), RTLIL_BACKEND::Dump_ډېرپېښ::Readable);
	أرجع f.str();
}

RTLIL::Process::Process() : module(تش_اشاره)
{
	ساكن بینښه صحيح hashidx_شمېر = 123456789;
	hashidx_شمېر = mkhash_xorshift(hashidx_شمېر);
	hashidx_ = hashidx_شمېر;
}

std::string RTLIL::Process::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", هذا, design(), RTLIL_BACKEND::Dump_ډېرپېښ::Readable);
	أرجع f.str();
}

RTLIL::Cell::Cell() : module(تش_اشاره)
{
	ساكن بینښه صحيح hashidx_شمېر = 123456789;
	hashidx_شمېر = mkhash_xorshift(hashidx_شمېر);
	hashidx_ = hashidx_شمېر;

	// log("#memtrace# %p\n", this);
	memhasher();

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->ورننباسه(std::pair<بینښه صحيح, RTLIL::Cell*>(hashidx_, هذا));
#د_که_پای
}

RTLIL::Cell::~Cell()
{
#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#د_که_پای
}

std::string RTLIL::Cell::منزل_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", هذا, design(), RTLIL_BACKEND::Dump_ډېرپېښ::Readable);
	أرجع f.str();
}

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
ساكن std::map<بینښه صحيح, RTLIL::Cell*> all_cells;
std::map<بینښه صحيح, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(فراغ)
{
	أرجع &all_cells;
}
#د_که_پای

منطقي RTLIL::Cell::has_پورټ(RTLIL::IdString portname) ثابت
{
	أرجع connections_.شمېر(portname) != 0;
}

// bufnorm

ثابت RTLIL::SigSpec &RTLIL::Cell::get_پورټ(RTLIL::IdString portname) ثابت
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
		أرجع ریښتیا;
	إذا (module && module->design && module->design->module(type))
		أرجع ریښتیا;
	أرجع خطأ;
}

منطقي RTLIL::Cell::ننوت(IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_معلوم(type))
		أرجع yosys_celltypes.cell_ننوت(type.ref(), portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : تش_اشاره;
		أرجع w && w->پورټ_ننوت;
	}
	أرجع خطأ;
}

منطقي RTLIL::Cell::output(RTLIL::IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_معلوم(type))
		أرجع yosys_celltypes.cell_output(type, portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : تش_اشاره;
		أرجع w && w->پورټ_output;
	}
	أرجع خطأ;
}

RTLIL::پورټ_Dir RTLIL::Cell::پورټ_dir(RTLIL::IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_معلوم(type.ref()))
		أرجع yosys_celltypes.cell_پورټ_dir(type, portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		إذا (m == تش_اشاره)
			أرجع پورټ_Dir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		إذا (w == تش_اشاره)
			أرجع پورټ_Dir::PD_UNKNOWN;
		أرجع پورټ_Dir(w->پورټ_ننوت + w->پورټ_output * 2);
	}
	أرجع پورټ_Dir::PD_UNKNOWN;
}

منطقي RTLIL::Cell::hasParam(RTLIL::IdString paramname) ثابت
{
	أرجع parameters.شمېر(paramname) != 0;
}

فراغ RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

فراغ RTLIL::Cell::سېټ_Param(RTLIL::IdString paramname, RTLIL::Const ارزښت)
{
	parameters[paramname] = std::move(ارزښت);
}

ثابت RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ثابت
{
	ثابت خپلکاری &it = parameters.پیدا_کړه(paramname);
	إذا (it != parameters.پای_ټکی())
		أرجع it->دوهم;
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		إذا (m)
			أرجع m->parameter_default_ارزښتونه.at(paramname);
	}
	ارم std::بهر_د_range("Cell::getParam()");
}

فراغ RTLIL::Cell::ترتیبول()
{
	connections_.ترتیبول(ترتیبول_by_id_str(module->twines()));
	parameters.ترتیبول(ترتیبول_by_id_str(module->twines()));
	ځانګړتیاوې.ترتیبول(ترتیبول_by_id_str(module->twines()));
}

فراغ RTLIL::Cell::check()
{
#که_نه_تعریف_شوی NDEBUG
	دننی_Cell_Checker checker(عدم, هذا);
	checker.check();
#د_که_پای
}

فراغ RTLIL::Cell::fixup_parameters(منطقي سېټ_a_signed, منطقي سېټ_b_signed)
{
	إذا (type.is_عامه() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		أرجع;

	إذا (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::پلنوالی_2] = Get_اندازه(connections_[ID::Y]);
		إذا (type.دننه(ID($pmux), ID($bmux)))
			parameters[ID::S_پلنوالی] = Get_اندازه(connections_[ID::S]);
		check();
		أرجع;
	}

	إذا (type == ID($demux)) {
		parameters[ID::پلنوالی_2] = Get_اندازه(connections_[ID::A]);
		parameters[ID::S_پلنوالی] = Get_اندازه(connections_[ID::S]);
		check();
		أرجع;
	}

	إذا (type == ID($lut) || type == ID($sop)) {
		parameters[ID::پلنوالی_2] = Get_اندازه(connections_[ID::A]);
		أرجع;
	}

	إذا (type == ID($fa)) {
		parameters[ID::پلنوالی_2] = Get_اندازه(connections_[ID::Y]);
		أرجع;
	}

	إذا (type == ID($lcu)) {
		parameters[ID::پلنوالی_2] = Get_اندازه(connections_[ID::CO]);
		أرجع;
	}

	إذا (type == ID($macc_v2)) {
		parameters[ID::Y_پلنوالی] = Get_اندازه(connections_[ID::Y]);
		أرجع;
	}

	منطقي signedness_ab = !type.دننه(ID($ټوټه), ID($concat), ID($macc));

	إذا (connections_.شمېر(ID::A)) {
		إذا (signedness_ab) {
			إذا (سېټ_a_signed)
				parameters[ID::A_SIGNED] = ریښتیا;
			وإلا إذا (parameters.شمېر(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = خطأ;
		}
		parameters[ID::A_پلنوالی] = Get_اندازه(connections_[ID::A]);
	}

	إذا (connections_.شمېر(ID::B)) {
		إذا (signedness_ab) {
			إذا (سېټ_b_signed)
				parameters[ID::B_SIGNED] = ریښتیا;
			وإلا إذا (parameters.شمېر(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = خطأ;
		}
		parameters[ID::B_پلنوالی] = Get_اندازه(connections_[ID::B]);
	}

	إذا (connections_.شمېر(ID::Y) && type != ID($concat))
		parameters[ID::Y_پلنوالی] = Get_اندازه(connections_[ID::Y]);

	إذا (connections_.شمېر(ID::Q))
		parameters[ID::پلنوالی_2] = Get_اندازه(connections_[ID::Q]);

	check();
}

منطقي RTLIL::Cell::has_keep_attr() ثابت {
	أرجع get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

منطقي RTLIL::Cell::has_memid() ثابت
{
	أرجع type.دننه(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

منطقي RTLIL::Cell::is_mem_cell() ثابت
{
	أرجع type.دننه(ID($mem), ID($mem_v2)) || has_memid();
}

منطقي RTLIL::Cell::is_builtin_ff() ثابت {
	أرجع StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ثابت RTLIL::Sig_بټ &بټ)
{
	wire = بټ.wire;
	دننه_واټن = 0;
	إذا (wire == عدم)
		معلومات = {بټ.معلومات};
	وإلا
		دننه_واټن = بټ.دننه_واټن;
	پلنوالی = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::ایستل(صحيح دننه_واټن, صحيح اوږدتیا) ثابت
{
	log_assert(دننه_واټن >= 0);
	log_assert(اوږدتیا >= 0);
	log_assert(دننه_واټن + اوږدتیا <= پلنوالی);
	RTLIL::SigChunk ret;
	إذا (wire) {
		ret.wire = wire;
		ret.دننه_واټن = هذا->دننه_واټن + دننه_واټن;
		ret.پلنوالی = اوږدتیا;
	} وإلا {
		لكل (صحيح i = 0; i < اوږدتیا; i++)
			ret.معلومات.ټېل_شا(معلومات[دننه_واټن+i]);
		ret.پلنوالی = اوږدتیا;
	}
	أرجع ret;
}

RTLIL::Sig_بټ RTLIL::SigChunk::چلوونکی[](صحيح دننه_واټن) ثابت
{
	log_assert(دننه_واټن >= 0);
	log_assert(دننه_واټن < پلنوالی);
	RTLIL::Sig_بټ ret;
	إذا (wire) {
		ret.wire = wire;
		ret.دننه_واټن = هذا->دننه_واټن + دننه_واټن;
	} وإلا {
		ret.معلومات = معلومات[دننه_واټن];
	}
	أرجع ret;
}

منطقي RTLIL::SigChunk::چلوونکی <(ثابت RTLIL::SigChunk &other) ثابت
{
	إذا (wire && other.wire)
		إذا (wire->name != other.wire->name)
			أرجع wire->name < other.wire->name;

	إذا (wire != other.wire)
		أرجع wire < other.wire;

	إذا (دننه_واټن != other.دننه_واټن)
		أرجع دننه_واټن < other.دننه_واټن;

	إذا (پلنوالی != other.پلنوالی)
		أرجع پلنوالی < other.پلنوالی;

	أرجع معلومات < other.معلومات;
}

منطقي RTLIL::SigChunk::چلوونکی ==(ثابت RTLIL::SigChunk &other) ثابت
{
	أرجع wire == other.wire && پلنوالی == other.پلنوالی && دننه_واټن == other.دننه_واټن && معلومات == other.معلومات;
}

منطقي RTLIL::SigChunk::چلوونکی !=(ثابت RTLIL::SigChunk &other) ثابت
{
	إذا (*هذا == other)
		أرجع خطأ;
	أرجع ریښتیا;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_تشه_بټونه();
	log_assert(parts.اندازه() > 0);
	خپلکاری ie = parts.begin();
	خپلکاری it = ie + parts.اندازه() - 1;
	طالما (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::Const &ارزښت)
{
	إذا (Get_اندازه(ارزښت) != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(ارزښت);
	} وإلا {
		init_تشه_بټونه();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&ارزښت)
{
	إذا (Get_اندازه(ارزښت) != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(ارزښت);
	} وإلا {
		init_تشه_بټونه();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::SigChunk &chunk)
{
	إذا (chunk.پلنوالی != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(chunk);
	} وإلا {
		init_تشه_بټونه();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	إذا (chunk.پلنوالی != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(chunk);
	} وإلا {
		init_تشه_بټونه();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	إذا (wire->پلنوالی != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(wire);
	} وإلا {
		init_تشه_بټونه();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, صحيح دننه_واټن, صحيح پلنوالی)
{
	إذا (پلنوالی != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(wire, دننه_واټن, پلنوالی);
	} وإلا {
		init_تشه_بټونه();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::string &str)
{
	إذا (str.اندازه() != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(str);
	} وإلا {
		init_تشه_بټونه();
	}
	check();
}

RTLIL::SigSpec::SigSpec(صحيح val, صحيح پلنوالی)
{
	إذا (پلنوالی != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(val, پلنوالی);
	} وإلا
		init_تشه_بټونه();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State بټ, صحيح پلنوالی)
{
	إذا (پلنوالی != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(بټ, پلنوالی);
	} وإلا
		init_تشه_بټونه();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::Sig_بټ &بټ, صحيح پلنوالی)
{
	إذا (پلنوالی != 0) {
		إذا (بټ.wire == عدم) {
			rep_ = CHUNK;
			جديد (&chunk_) RTLIL::SigChunk(بټ.معلومات, پلنوالی);
		} وإلا إذا (پلنوالی == 1) {
			rep_ = CHUNK;
			جديد (&chunk_) RTLIL::SigChunk(بټ);
		} وإلا {
			init_تشه_بټونه();
			بټونه_2.reserve(پلنوالی);
			لكل (صحيح i = 0; i < پلنوالی; i++)
				بټونه_2.ټېل_شا(بټ);
		}
	} وإلا
		init_تشه_بټونه();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::ویکټور<RTLIL::SigChunk> &chunks)
{
	init_تشه_بټونه();
	لكل (ثابت خپلکاری &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::ویکټور<RTLIL::Sig_بټ> &بټونه)
{
	init_تشه_بټونه();
	لكل (ثابت خپلکاری &بټ : بټونه)
		append(بټ);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت pool<RTLIL::Sig_بټ> &بټونه)
{
	init_تشه_بټونه();
	لكل (ثابت خپلکاری &بټ : بټونه)
		append(بټ);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::سېټ<RTLIL::Sig_بټ> &بټونه)
{
	init_تشه_بټونه();
	لكل (ثابت خپلکاری &بټ : بټونه)
		append(بټ);
	check();
}

RTLIL::SigSpec::SigSpec(منطقي بټ)
{
	rep_ = CHUNK;
	جديد (&chunk_) RTLIL::SigChunk(بټ ? RTLIL::S1 : RTLIL::S0);
	check();
}

فراغ RTLIL::SigSpec::Chunks::const_iterator::بل_chunk_بټونه()
{
	صحيح بټونه_اندازه = Get_اندازه(spec.بټونه_2);
	إذا (بټ_شاخص >= بټونه_اندازه)
		أرجع;
	صحيح i = بټ_شاخص;
	ثابت Sig_بټ &بټ = spec.بټونه_2[i++];
	chunk.wire = بټ.wire;
	chunk.معلومات.clear();
	إذا (بټ.is_wire()) {
		chunk.دننه_واټن = بټ.دننه_واټن;
		طالما (i < بټونه_اندازه && spec.بټونه_2[i].wire == بټ.wire &&
			spec.بټونه_2[i].دننه_واټن == بټ.دننه_واټن + i - بټ_شاخص)
			++i;
	} وإلا {
		chunk.دننه_واټن = 0;
		chunk.معلومات.ټېل_شا(بټ.معلومات);
		طالما (i < بټونه_اندازه && !spec.بټونه_2[i].is_wire()) {
			chunk.معلومات.ټېل_شا(spec.بټونه_2[i].معلومات);
			++i;
		}
	}
	chunk.پلنوالی = i - بټ_شاخص;
}

فراغ RTLIL::SigSpec::unpack()
{
	إذا (rep_ == بټونه_3)
		أرجع;

	std::ویکټور<RTLIL::Sig_بټ> بټونه;
	بټونه.reserve(chunk_.پلنوالی);
	لكل (صحيح i = 0; i < chunk_.پلنوالی; i++)
		بټونه.emplace_شا(chunk_, i);

	chunk_.~SigChunk();
	rep_ = بټونه_3;
	جديد (&بټونه_2) std::ویکټور<RTLIL::Sig_بټ>(std::move(بټونه));
}

فراغ RTLIL::SigSpec::try_repack()
{
	إذا (rep_ != بټونه_3)
		أرجع;

	صحيح بټونه_اندازه = Get_اندازه(بټونه_2);
	إذا (بټونه_اندازه == 0)
		أرجع;
	إذا (بټونه_2[0].is_wire()) {
		لكل (صحيح i = 1; i < بټونه_اندازه; i++)
			إذا (بټونه_2[0].wire != بټونه_2[i].wire || بټونه_2[0].دننه_واټن + i != بټونه_2[i].دننه_واټن)
				أرجع;
		SigChunk chunk(بټونه_2[0].wire, بټونه_2[0].دننه_واټن, بټونه_اندازه);
		بټونه_2.~ویکټور();
		rep_ = CHUNK;
		جديد (&chunk_) SigChunk(std::move(chunk));
		أرجع;
	}
	std::ویکټور<RTLIL::State> بټونه;
	بټونه.reserve(بټونه_اندازه);
	بټونه.ټېل_شا(بټونه_2[0].معلومات);
	لكل (صحيح i = 1; i < بټونه_اندازه; i++) {
		إذا (بټونه_2[i].is_wire())
			أرجع;
		بټونه.ټېل_شا(بټونه_2[i].معلومات);
	}
	بټونه_2.~ویکټور();
	rep_ = CHUNK;
	جديد (&chunk_) SigChunk(std::move(بټونه));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ثابت
{
	Hasher h;
	لكل (خپلکاری &c : chunks())
		إذا (c.wire == عدم) {
			لكل (خپلکاری &v : c.معلومات)
				h.eat(v);
		} وإلا {
			h.eat(c.wire->name.ref());
			h.eat(c.دننه_واټن);
			h.eat(c.پلنوالی);
		}
	Hasher::hash_t پایله = h.yield();
	إذا (پایله == 0)
		پایله = 1;
	hash_.سېټ(پایله);
	أرجع پایله;
}

فراغ RTLIL::SigSpec::ترتیبول()
{
	unpack();
	std::ترتیبول(بټونه_2.begin(), بټونه_2.پای_ټکی());
	hash_.clear();
	try_repack();
}

فراغ RTLIL::SigSpec::ترتیبول_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::ویکټور<Sig_بټ> unique_بټونه = بټونه_2;
	std::ترتیبول(unique_بټونه.begin(), unique_بټونه.پای_ټکی());
	خپلکاری وروستی = std::unique(unique_بټونه.begin(), unique_بټونه.پای_ټکی());
	unique_بټونه.erase(وروستی, unique_بټونه.پای_ټکی());

	*هذا = unique_بټونه;
	hash_.clear();
	try_repack();
}

فراغ RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &نمونه, ثابت RTLIL::SigSpec &with)
{
	replace(نمونه, with, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &نمونه, ثابت RTLIL::SigSpec &with, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(اندازه() == other->اندازه());
	log_assert(نمونه.اندازه() == with.اندازه());

	dict<RTLIL::Sig_بټ, صحيح> نمونه_منزل_with;
	صحيح نمونه_اندازه = نمونه.اندازه();
	لكل (صحيح i = 0; i < نمونه_اندازه; i++) {
		Sig_بټ نمونه_بټ = نمونه[i];
		إذا (نمونه_بټ.wire != عدم) {
			نمونه_منزل_with.emplace(نمونه_بټ, i);
		}
	}

	صحيح this_اندازه = اندازه();
	منطقي other_modified = خطأ;
	لكل (صحيح j = 0; j < this_اندازه; j++) {
		خپلکاری it = نمونه_منزل_with.پیدا_کړه((*هذا)[j]);
		إذا (it != نمونه_منزل_with.پای_ټکی()) {
			other_modified = ریښتیا;
			(*other)[j] = with[it->دوهم];
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::replace(ثابت dict<RTLIL::Sig_بټ, RTLIL::Sig_بټ> &قواعد)
{
	replace(قواعد, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت dict<RTLIL::Sig_بټ, RTLIL::Sig_بټ> &قواعد, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(اندازه() == other->اندازه());

	إذا (قواعد.تشه()) أرجع;

	صحيح this_اندازه = اندازه();
	منطقي other_modified = خطأ;
	لكل (صحيح i = 0; i < this_اندازه; i++) {
		خپلکاری it = قواعد.پیدا_کړه((*هذا)[i]);
		إذا (it != قواعد.پای_ټکی()) {
			other_modified = ریښتیا;
			(*other)[i] = it->دوهم;
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::replace(ثابت std::map<RTLIL::Sig_بټ, RTLIL::Sig_بټ> &قواعد)
{
	replace(قواعد, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت std::map<RTLIL::Sig_بټ, RTLIL::Sig_بټ> &قواعد, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(اندازه() == other->اندازه());

	إذا (قواعد.تشه()) أرجع;

	صحيح this_اندازه = اندازه();
	منطقي other_modified = خطأ;
	لكل (صحيح i = 0; i < this_اندازه; i++) {
		خپلکاری it = قواعد.پیدا_کړه((*هذا)[i]);
		إذا (it != قواعد.پای_ټکی()) {
			other_modified = ریښتیا;
			(*other)[i] = it->دوهم;
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &نمونه)
{
	remove2(نمونه, عدم);
}

فراغ RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &نمونه, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *هذا;
	tmp.remove2(نمونه, other);
}

فراغ RTLIL::SigSpec::remove2(ثابت RTLIL::SigSpec &نمونه, RTLIL::SigSpec *other)
{
	unpack();
	إذا (other != عدم) {
		log_assert(اندازه() == other->اندازه());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<Sig_بټ> نمونه_بټونه;
	نمونه_بټونه.reserve(نمونه.اندازه());
	لكل (خپلکاری &بټ : نمونه)
		إذا (بټ.wire != عدم)
			نمونه_بټونه.ورننباسه(بټ);

	// Compact in-place to avoid O(n^2) erase operations
	اندازه_t write_idx = 0;
	لكل (اندازه_t read_idx = 0; read_idx < بټونه_2.اندازه(); read_idx++)
	{
		إذا (!(بټونه_2[read_idx].wire != عدم && نمونه_بټونه.شمېر(بټونه_2[read_idx]))) {
			إذا (write_idx != read_idx) {
				بټونه_2[write_idx] = بټونه_2[read_idx];
				إذا (other != عدم)
					other->بټونه_2[write_idx] = other->بټونه_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < بټونه_2.اندازه());
	إذا (modified) {
		بټونه_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->بټونه_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove(ثابت pool<RTLIL::Sig_بټ> &نمونه)
{
	remove2(نمونه, عدم);
}

فراغ RTLIL::SigSpec::remove(ثابت pool<RTLIL::Sig_بټ> &نمونه, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *هذا;
	tmp.remove2(نمونه, other);
}

فراغ RTLIL::SigSpec::remove2(ثابت pool<RTLIL::Sig_بټ> &نمونه, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(اندازه() == other->اندازه());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	اندازه_t write_idx = 0;
	لكل (اندازه_t read_idx = 0; read_idx < بټونه_2.اندازه(); read_idx++) {
		إذا (!(بټونه_2[read_idx].wire != عدم && نمونه.شمېر(بټونه_2[read_idx]))) {
			إذا (write_idx != read_idx) {
				بټونه_2[write_idx] = بټونه_2[read_idx];
				إذا (other != عدم)
					other->بټونه_2[write_idx] = other->بټونه_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < بټونه_2.اندازه());
	إذا (modified) {
		بټونه_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->بټونه_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove2(ثابت std::سېټ<RTLIL::Sig_بټ> &نمونه, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(اندازه() == other->اندازه());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	اندازه_t write_idx = 0;
	لكل (اندازه_t read_idx = 0; read_idx < بټونه_2.اندازه(); read_idx++) {
		إذا (!(بټونه_2[read_idx].wire != عدم && نمونه.شمېر(بټونه_2[read_idx]))) {
			إذا (write_idx != read_idx) {
				بټونه_2[write_idx] = بټونه_2[read_idx];
				إذا (other != عدم)
					other->بټونه_2[write_idx] = other->بټونه_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < بټونه_2.اندازه());
	إذا (modified) {
		بټونه_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->بټونه_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove2(ثابت pool<RTLIL::Wire*> &نمونه, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(اندازه() == other->اندازه());
		other->unpack();
	}

	منطقي modified = خطأ;
	منطقي other_modified = خطأ;
	لكل (صحيح i = Get_اندازه(بټونه_2) - 1; i >= 0; i--) {
		إذا (بټونه_2[i].wire != عدم && نمونه.شمېر(بټونه_2[i].wire)) {
			modified = ریښتیا;
			بټونه_2.erase(بټونه_2.begin() + i);
			إذا (other != عدم) {
				other_modified = ریښتیا;
				other->بټونه_2.erase(other->بټونه_2.begin() + i);
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

RTLIL::SigSpec RTLIL::SigSpec::ایستل(ثابت RTLIL::SigSpec &نمونه, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == عدم || اندازه() == other->اندازه());

	RTLIL::SigSpec ret;
	std::ویکټور<RTLIL::Sig_بټ> بټونه_سمون = منزل_sigbit_ویکټور();

	لكل (خپلکاری& نمونه_chunk : نمونه.chunks()) {
		إذا (other) {
			std::ویکټور<RTLIL::Sig_بټ> بټونه_other = other->منزل_sigbit_ویکټور();
			صحيح i = 0;
			لكل (ثابت RTLIL::Sig_بټ &بټ : بټونه_سمون) {
				إذا (بټ.wire &&
					بټ.wire == نمونه_chunk.wire &&
					بټ.دننه_واټن >= نمونه_chunk.دننه_واټن &&
					بټ.دننه_واټن < نمونه_chunk.دننه_واټن + نمونه_chunk.پلنوالی)
					ret.append(بټونه_other[i]);
				++i;
			}
		} وإلا {
			لكل (ثابت RTLIL::Sig_بټ &بټ : بټونه_سمون)
				إذا (بټ.wire &&
					بټ.wire == نمونه_chunk.wire &&
					بټ.دننه_واټن >= نمونه_chunk.دننه_واټن &&
					بټ.دننه_واټن < نمونه_chunk.دننه_واټن + نمونه_chunk.پلنوالی)
					ret.append(بټ);
		}
	}

	ret.check();
	أرجع ret;
}

RTLIL::SigSpec RTLIL::SigSpec::ایستل(ثابت pool<RTLIL::Sig_بټ> &نمونه, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == عدم || اندازه() == other->اندازه());

	std::ویکټور<RTLIL::Sig_بټ> بټونه_سمون = منزل_sigbit_ویکټور();
	RTLIL::SigSpec ret;

	إذا (other) {
		std::ویکټور<RTLIL::Sig_بټ> بټونه_other = other->منزل_sigbit_ویکټور();
		صحيح i = 0;
		لكل (ثابت RTLIL::Sig_بټ &بټ : بټونه_سمون) {
			إذا (بټ.wire && نمونه.شمېر(بټ))
				ret.append(بټونه_other[i]);
			++i;
		}
	} وإلا {
		لكل (ثابت RTLIL::Sig_بټ &بټ : بټونه_سمون)
			إذا (بټ.wire && نمونه.شمېر(بټ))
				ret.append(بټ);
	}

	ret.check();
	أرجع ret;
}

فراغ RTLIL::SigSpec::replace(صحيح دننه_واټن, ثابت RTLIL::SigSpec &with)
{
	إذا (with.اندازه() == 0)
		أرجع;

	unpack();

	log_assert(دننه_واټن >= 0);
	log_assert(with.اندازه() >= 0);
	log_assert(دننه_واټن+with.اندازه() <= اندازه());

	صحيح i = 0;
	لكل (ثابت RTLIL::Sig_بټ &بټ : with.بټونه()) {
		بټونه_2.at(دننه_واټن + i) = بټ;
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
			init_تشه_بټونه();
			hash_.clear();
		}
	}
	وإلا
	{
		std::ویکټور<RTLIL::Sig_بټ> new_بټونه;
		new_بټونه.reserve(بټونه_2.اندازه());
		لكل (خپلکاری &بټ : بټونه_2)
			إذا (بټ.wire != عدم)
				new_بټونه.ټېل_شا(بټ);
		إذا (Get_اندازه(new_بټونه) != Get_اندازه(بټونه_2)) {
			بټونه_2.بدلون(new_بټونه);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

فراغ RTLIL::SigSpec::remove(صحيح دننه_واټن, صحيح اوږدتیا)
{
	إذا (اوږدتیا == 0)
		أرجع;

	unpack();

	log_assert(دننه_واټن >= 0);
	log_assert(اوږدتیا >= 0);
	log_assert(دننه_واټن + اوږدتیا <= اندازه());

	بټونه_2.erase(بټونه_2.begin() + دننه_واټن, بټونه_2.begin() + دننه_واټن + اوږدتیا);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::ایستل(صحيح دننه_واټن, صحيح اوږدتیا) ثابت
{
	log_assert(دننه_واټن >= 0);
	log_assert(اوږدتیا >= 0);
	log_assert(دننه_واټن + اوږدتیا <= اندازه());

	std::ویکټور<Sig_بټ> extracted;
	Sig_بټ لومړی;
	منطقي is_packing = ریښتیا;
	لكل (صحيح i = دننه_واټن; i < دننه_واټن + اوږدتیا; i++) {
		منطقي was_packing_before = is_packing;
		Sig_بټ بټ = (*هذا)[i];
		إذا (i == دننه_واټن) {
			لومړی = بټ;
			إذا (!بټ.wire)
				is_packing = خطأ;
		} وإلا {
			إذا (بټ.wire != لومړی.wire)
				is_packing = خطأ;
			إذا (بټ.wire)
				إذا (بټ.دننه_واټن != لومړی.دننه_واټن + (i - دننه_واټن))
					is_packing = خطأ;
		}
		إذا (was_packing_before && !is_packing)
			لكل (صحيح j = دننه_واټن; j < i; j++)
				extracted.ټېل_شا((*هذا)[j]);
		إذا (!is_packing)
			extracted.ټېل_شا((*هذا)[i]);
	}
	إذا (is_packing)
		أرجع SigChunk(لومړی.wire, لومړی.دننه_واټن, اوږدتیا);

	أرجع extracted;
}

فراغ RTLIL::SigSpec::rewrite_wires(std::function<فراغ(RTLIL::Wire*& wire)> rewrite)
{
	إذا (rep_ == CHUNK) {
		إذا (chunk_.wire != تش_اشاره)
			rewrite(chunk_.wire);
		أرجع;
	}

	std::ویکټور<RTLIL::Sig_بټ> new_بټونه;
	لكل (ثابت RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		إذا (c.wire != تش_اشاره)
			rewrite(c.wire);
		لكل (صحيح i = 0; i < c.پلنوالی; i++)
			new_بټونه.emplace_شا(c, i);
	}
	بټونه_2 = std::move(new_بټونه);
	hash_.clear();
}

فراغ RTLIL::SigSpec::append(ثابت RTLIL::SigSpec &إشارة)
{
	إذا (إشارة.اندازه() == 0)
		أرجع;

	إذا (اندازه() == 0) {
		*هذا = إشارة;
		أرجع;
	}

	hash_.clear();
	إذا (rep_ == CHUNK && إشارة.rep_ == CHUNK && chunk_.wire == إشارة.chunk_.wire) {
		إذا (chunk_.wire == عدم) {
			chunk_.معلومات.ورننباسه(chunk_.معلومات.پای_ټکی(), إشارة.chunk_.معلومات.begin(), إشارة.chunk_.معلومات.پای_ټکی());
			chunk_.پلنوالی = Get_اندازه(chunk_.معلومات);
			أرجع;
		}
		إذا (chunk_.دننه_واټن + chunk_.پلنوالی == إشارة.chunk_.دننه_واټن) {
			chunk_.پلنوالی += إشارة.chunk_.پلنوالی;
			أرجع;
		}
	}

	unpack();
	لكل (ثابت Sig_بټ &بټ : إشارة.بټونه())
		بټونه_2.ټېل_شا(بټ);
	check();
}

فراغ RTLIL::SigSpec::append(ثابت RTLIL::Sig_بټ &بټ)
{
	hash_.clear();

	إذا (اندازه() == 0) {
		destroy();
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(بټ);
		أرجع;
	}

	إذا (rep_ == CHUNK && chunk_.wire == بټ.wire) {
		إذا (chunk_.wire == عدم) {
			chunk_.معلومات.ټېل_شا(بټ.معلومات);
			chunk_.پلنوالی++;
			أرجع;
		}
		إذا (chunk_.دننه_واټن + chunk_.پلنوالی == بټ.دننه_واټن) {
			chunk_.پلنوالی++;
			أرجع;
		}
	}

	unpack();

	بټونه_2.ټېل_شا(بټ);
	check();
}

فراغ RTLIL::SigSpec::extend_u0(صحيح پلنوالی, منطقي is_signed)
{
	إذا (اندازه() > پلنوالی)
		remove(پلنوالی, اندازه() - پلنوالی);

	إذا (اندازه() < پلنوالی) {
		RTLIL::Sig_بټ padding = اندازه() > 0 ? (*هذا)[اندازه() - 1] : RTLIL::State::Sx;
		إذا (!is_signed)
			padding = RTLIL::State::S0;
		طالما (اندازه() < پلنوالی)
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

#که_نه_تعریف_شوی NDEBUG
فراغ RTLIL::SigSpec::check(ثابت Module *mod) ثابت
{
	إذا (rep_ == CHUNK)
	{
		log_assert(chunk_.پلنوالی != 0);
		إذا (chunk_.wire == عدم) {
			log_assert(chunk_.دننه_واټن == 0);
			log_assert(chunk_.معلومات.اندازه() == (اندازه_t)chunk_.پلنوالی);
		} وإلا {
			log_assert(chunk_.دننه_واټن >= 0);
			log_assert(chunk_.پلنوالی >= 0);
			log_assert(chunk_.دننه_واټن + chunk_.پلنوالی <= chunk_.wire->پلنوالی);
			log_assert(chunk_.معلومات.اندازه() == 0);
			إذا (mod != تش_اشاره)
				log_assert(chunk_.wire->module == mod);
		}
	}
	وإلا إذا (اندازه() <= 64)
	{
		إذا (mod != تش_اشاره) {
			لكل (ثابت RTLIL::Sig_بټ &بټ : بټونه_2)
				إذا (بټ.wire != تش_اشاره)
					log_assert(بټ.wire->module == mod);
		}
	}
}
#د_که_پای

منطقي RTLIL::SigSpec::چلوونکی <(ثابت RTLIL::SigSpec &other) ثابت
{
	إذا (هذا == &other)
		أرجع خطأ;

	إذا (اندازه() != other.اندازه())
		أرجع اندازه() < other.اندازه();

	خپلکاری other_it = other.chunks().begin();
	لكل (ثابت SigChunk &c : chunks()) {
		إذا (c != *other_it)
			أرجع c < *other_it;
		++other_it;
	}

	أرجع خطأ;
}

منطقي RTLIL::SigSpec::چلوونکی ==(ثابت RTLIL::SigSpec &other) ثابت
{
	إذا (هذا == &other)
		أرجع ریښتیا;

	إذا (اندازه() != other.اندازه())
		أرجع خطأ;

	خپلکاری other_it = other.chunks().begin();
	لكل (ثابت SigChunk &c : chunks()) {
		إذا (c != *other_it)
			أرجع خطأ;
		++other_it;
	}

	أرجع ریښتیا;
}

منطقي RTLIL::SigSpec::is_wire() ثابت
{
	Chunks cs = chunks();
	خپلکاری it = cs.begin();
	إذا (it == cs.پای_ټکی())
		أرجع خطأ;
	ثابت RTLIL::SigChunk &chunk = *it;
	أرجع chunk.wire && chunk.wire->پلنوالی == اندازه() && ++it == cs.پای_ټکی();
}

منطقي RTLIL::SigSpec::is_chunk() ثابت
{
	Chunks cs = chunks();
	خپلکاری it = cs.begin();
	إذا (it == cs.پای_ټکی())
		أرجع خطأ;
	أرجع ++it == cs.پای_ټکی();
}

منطقي RTLIL::SigSpec::معلوم_driver() ثابت
{
	لكل (خپلکاری &chunk : chunks())
		إذا (chunk.is_wire() && !chunk.wire->معلوم_driver())
			أرجع خطأ;
	أرجع ریښتیا;
}

منطقي RTLIL::SigSpec::is_fully_const() ثابت
{
	لكل (خپلکاری &chunk : chunks())
		إذا (chunk.پلنوالی > 0 && chunk.wire != عدم)
			أرجع خطأ;
	أرجع ریښتیا;
}

منطقي RTLIL::SigSpec::is_fully_zero() ثابت
{
	لكل (خپلکاری &chunk : chunks()) {
		إذا (chunk.پلنوالی > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.معلومات)
			إذا (d != RTLIL::State::S0)
				أرجع خطأ;
	}
	أرجع ریښتیا;
}

منطقي RTLIL::SigSpec::is_fully_ones() ثابت
{
	لكل (خپلکاری &chunk : chunks()) {
		إذا (chunk.پلنوالی > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.معلومات)
			إذا (d != RTLIL::State::S1)
				أرجع خطأ;
	}
	أرجع ریښتیا;
}

منطقي RTLIL::SigSpec::is_fully_def() ثابت
{
	لكل (خپلکاری &chunk : chunks()) {
		إذا (chunk.پلنوالی > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.معلومات)
			إذا (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				أرجع خطأ;
	}
	أرجع ریښتیا;
}

منطقي RTLIL::SigSpec::is_fully_undef() ثابت
{
	لكل (خپلکاری &chunk : chunks()) {
		إذا (chunk.پلنوالی > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.معلومات)
			إذا (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				أرجع خطأ;
	}
	أرجع ریښتیا;
}

منطقي RTLIL::SigSpec::has_const() ثابت
{
	لكل (خپلکاری &chunk : chunks())
		إذا (chunk.پلنوالی > 0 && chunk.wire == عدم)
			أرجع ریښتیا;
	أرجع خطأ;
}

منطقي RTLIL::SigSpec::has_const(State state) ثابت
{
	لكل (خپلکاری &chunk : chunks())
		إذا (chunk.پلنوالی > 0 && chunk.wire == عدم && std::پیدا_کړه(chunk.معلومات.begin(), chunk.معلومات.پای_ټکی(), state) != chunk.معلومات.پای_ټکی())
			أرجع ریښتیا;
	أرجع خطأ;
}


منطقي RTLIL::SigSpec::has_marked_بټونه() ثابت
{
	لكل (خپلکاری &chunk : chunks())
		إذا (chunk.پلنوالی > 0 && chunk.wire == عدم) {
			لكل (RTLIL::State d : chunk.معلومات)
				إذا (d == RTLIL::State::Sm)
					أرجع ریښتیا;
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
	log_assert(c.has_ارزښت());
	أرجع c->as_bool();
}

صحيح RTLIL::SigSpec::as_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ارزښت());
	أرجع c->as_int(is_signed);
}

منطقي RTLIL::SigSpec::convertible_منزل_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	إذا (!c.has_ارزښت())
		أرجع خطأ;
	أرجع c->convertible_منزل_int(is_signed);
}

std::optional<صحيح> RTLIL::SigSpec::try_as_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	إذا (!c.has_ارزښت())
		أرجع std::nullopt;
	أرجع c->try_as_int(is_signed);
}

صحيح RTLIL::SigSpec::as_int_saturating(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ارزښت());
	أرجع c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ثابت
{
	std::string str;
	str.reserve(اندازه());
	std::ویکټور<RTLIL::SigChunk> chunks = *هذا;
	لكل (اندازه_t i = chunks.اندازه(); i > 0; i--) {
		ثابت RTLIL::SigChunk &chunk = chunks[i-1];
		إذا (chunk.wire != عدم)
			str.append(chunk.پلنوالی, '?');
		وإلا
			str += RTLIL::Const(chunk.معلومات).as_string();
	}
	أرجع str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ثابت
{
	Chunks cs = chunks();
	خپلکاری it = cs.begin();
	إذا (it == cs.پای_ټکی())
		أرجع RTLIL::Const();
	SigChunk chunk = *it;
	إذا (chunk.wire != عدم || ++it != cs.پای_ټکی())
		أرجع std::nullopt;
	أرجع RTLIL::Const(std::move(chunk.معلومات));
}

RTLIL::Const RTLIL::SigSpec::as_const() ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ارزښت());
	أرجع *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ثابت
{
	Chunks cs = chunks();
	خپلکاری it = cs.begin();
	log_assert(it != cs.پای_ټکی());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.پای_ټکی() && chunk.wire && chunk.wire->پلنوالی == اندازه());
	أرجع chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ثابت
{
	Chunks cs = chunks();
	خپلکاری it = cs.begin();
	log_assert(it != cs.پای_ټکی());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.پای_ټکی());
	أرجع chunk;
}

RTLIL::Sig_بټ RTLIL::SigSpec::as_بټ() ثابت
{
	أرجع RTLIL::Sig_بټ(*هذا);
}

منطقي RTLIL::SigSpec::سمون(ثابت حرف* نمونه) ثابت
{
	صحيح نمونه_len = مزي_اوږدوالی(نمونه);
	log_assert(نمونه_len == اندازه());

	لكل (صحيح i = 0; i < نمونه_len; ++i) {
		حرف ch = نمونه[i];
		إذا (ch == ' ')
			استمر;
		RTLIL::Sig_بټ بټ = (*هذا)[نمونه_len - 1 - i];
		إذا (ch == '*') {
			إذا (بټ != State::Sz && بټ != State::Sx)
				أرجع خطأ;
			استمر;
		}
		إذا (ch == '0') {
			إذا (بټ != State::S0)
				أرجع خطأ;
		} وإلا
		إذا (ch == '1') {
			إذا (بټ != State::S1)
				أرجع خطأ;
		} وإلا
			log_abort();
	}

	أرجع ریښتیا;
}

std::سېټ<RTLIL::Sig_بټ> RTLIL::SigSpec::منزل_sigbit_سېټ() ثابت
{
	std::سېټ<RTLIL::Sig_بټ> sigbits;
	لكل (خپلکاری &c : chunks())
		لكل (صحيح i = 0; i < c.پلنوالی; i++)
			sigbits.ورننباسه(RTLIL::Sig_بټ(c, i));
	أرجع sigbits;
}

pool<RTLIL::Sig_بټ> RTLIL::SigSpec::منزل_sigbit_pool() ثابت
{
	pool<RTLIL::Sig_بټ> sigbits;
	sigbits.reserve(اندازه());
	لكل (خپلکاری &c : chunks())
		لكل (صحيح i = 0; i < c.پلنوالی; i++)
			sigbits.ورننباسه(RTLIL::Sig_بټ(c, i));
	أرجع sigbits;
}

std::ویکټور<RTLIL::Sig_بټ> RTLIL::SigSpec::منزل_sigbit_ویکټور() ثابت
{
	std::ویکټور<RTLIL::Sig_بټ> پایله;
	پایله.reserve(اندازه());
	لكل (Sig_بټ بټ : *هذا)
		پایله.ټېل_شا(بټ);
	أرجع پایله;
}

std::map<RTLIL::Sig_بټ, RTLIL::Sig_بټ> RTLIL::SigSpec::منزل_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت
{
	صحيح this_اندازه = اندازه();
	log_assert(this_اندازه == other.اندازه());

	std::map<RTLIL::Sig_بټ, RTLIL::Sig_بټ> new_map;
	لكل (صحيح i = 0; i < this_اندازه; i++)
		new_map[(*هذا)[i]] = other[i];

	أرجع new_map;
}

dict<RTLIL::Sig_بټ, RTLIL::Sig_بټ> RTLIL::SigSpec::منزل_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت
{
	صحيح this_اندازه = اندازه();
	log_assert(this_اندازه == other.اندازه());

	dict<RTLIL::Sig_بټ, RTLIL::Sig_بټ> new_map;
	new_map.reserve(this_اندازه);
	لكل (صحيح i = 0; i < this_اندازه; i++)
		new_map[(*هذا)[i]] = other[i];

	أرجع new_map;
}

ساكن فراغ sigspec_parse_وېشل(std::ویکټور<std::string> &نښې, ثابت std::string &متن, حرف sep)
{
	اندازه_t پیل = 0, پای_ټکی = 0;
	طالما ((پای_ټکی = متن.پیدا_کړه(sep, پیل)) != std::string::npos) {
		نښې.ټېل_شا(متن.substr(پیل, پای_ټکی - پیل));
		پیل = پای_ټکی + 1;
	}
	نښې.ټېل_شا(متن.substr(پیل));
}

ساكن RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ثابت std::string &netname)
{
	أرجع module->wire(netname);
}

منطقي RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::ویکټور<std::string> نښې;
	sigspec_parse_وېشل(نښې, str, ',');

	sig = RTLIL::SigSpec();
	لكل (صحيح tokidx = صحيح(نښې.اندازه())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = نښې[tokidx];
		std::string indices;

		إذا (netname.اندازه() == 0)
			استمر;

		إذا (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			خپلکاری ast = p.const2ast(netname);
			إذا (ast == تش_اشاره)
				أرجع خطأ;
			sig.append(RTLIL::Const(ast->بټونه));
			استمر;
		}

		إذا (module == عدم)
			أرجع خطأ;

		إذا (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		إذا (sigspec_parse_wire(module, netname) == تش_اشاره) {
			اندازه_t indices_pos = netname.اندازه()-1;
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
		إذا (wire == تش_اشاره)
			أرجع خطأ;

		إذا (!indices.تشه()) {
			std::ویکټور<std::string> شاخص_نښې;
			sigspec_parse_وېشل(شاخص_نښې, indices.substr(1, indices.اندازه()-2), ':');
			إذا (شاخص_نښې.اندازه() == 1) {
				صحيح a = atoi(شاخص_نښې.at(0).c_str());
				إذا (a < 0 || a >= wire->پلنوالی)
					أرجع خطأ;
				sig.append(RTLIL::SigSpec(wire, a));
			} وإلا {
				صحيح a = atoi(شاخص_نښې.at(0).c_str());
				صحيح b = atoi(شاخص_نښې.at(1).c_str());
				إذا (a > b) {
					صحيح tmp = a;
					a = b, b = tmp;
				}
				إذا (a < 0 || a >= wire->پلنوالی)
					أرجع خطأ;
				إذا (b < 0 || b >= wire->پلنوالی)
					أرجع خطأ;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} وإلا
			sig.append(wire);
	}

	أرجع ریښتیا;
}

منطقي RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	إذا (str.تشه() || str[0] != '@')
		أرجع parse(sig, module, str);

	IdString sel_name = design->twines().پیدا_کړه(RTLIL::escape_id(str.substr(1)));
	إذا (sel_name == IdString::Null || design->selection_vars.شمېر(sel_name) == 0)
		أرجع خطأ;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	لكل (خپلکاری &it : module->wires_)
		إذا (sel.selected_member(module->name, it.دوهم->name))
			sig.append(it.دوهم);

	أرجع ریښتیا;
}

منطقي RTLIL::SigSpec::parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	إذا (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.اندازه());
		أرجع ریښتیا;
	}

	إذا (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.اندازه());
		أرجع ریښتیا;
	}

	إذا (lhs.is_chunk()) {
		حرف *p = (حرف*)str.c_str(), *endptr;
		اوږد صحيح val = strtol(p, &endptr, 10);
		إذا (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.اندازه());
			أرجع ریښتیا;
		}
	}

	إذا (!parse(sig, module, str))
		أرجع خطأ;
	إذا (sig.اندازه() > lhs.اندازه())
		sig.remove(lhs.اندازه(), sig.اندازه() - lhs.اندازه());
	أرجع ریښتیا;
}

RTLIL::SigSpec::چلوونکی std::ویکټور<RTLIL::SigChunk>() ثابت
{
	std::ویکټور<RTLIL::SigChunk> پایله;
	لكل (ثابت RTLIL::SigChunk &c : chunks())
		پایله.ټېل_شا(c);
	أرجع پایله;
}

RTLIL::Case_قاعده::~Case_قاعده()
{
	لكل (خپلکاری it = switches.begin(); it != switches.پای_ټکی(); it++)
		ړنګول *it;
}

منطقي RTLIL::Case_قاعده::تشه() ثابت
{
	أرجع actions.تشه() && switches.تشه();
}

RTLIL::Case_قاعده *RTLIL::Case_قاعده::clone() ثابت
{
	RTLIL::Case_قاعده *new_caserule = جديد RTLIL::Case_قاعده;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->ځانګړتیاوې = ځانګړتیاوې;
	لكل (خپلکاری &it : switches)
		new_caserule->switches.ټېل_شا(it->clone());
	أرجع new_caserule;
}

RTLIL::Switch_قاعده::~Switch_قاعده()
{
	لكل (خپلکاری it = cases.begin(); it != cases.پای_ټکی(); it++)
		ړنګول *it;
}

منطقي RTLIL::Switch_قاعده::تشه() ثابت
{
	أرجع cases.تشه();
}

RTLIL::Switch_قاعده *RTLIL::Switch_قاعده::clone() ثابت
{
	RTLIL::Switch_قاعده *new_switchrule = جديد RTLIL::Switch_قاعده;
	new_switchrule->إشارة = إشارة;
	new_switchrule->ځانګړتیاوې = ځانګړتیاوې;
	لكل (خپلکاری &it : cases)
		new_switchrule->cases.ټېل_شا(it->clone());
	أرجع new_switchrule;

}

RTLIL::Sync_قاعده *RTLIL::Sync_قاعده::clone() ثابت
{
	RTLIL::Sync_قاعده *new_syncrule = جديد RTLIL::Sync_قاعده;
	new_syncrule->type = type;
	new_syncrule->إشارة = إشارة;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	أرجع new_syncrule;
}

RTLIL::Process::~Process()
{
	لكل (خپلکاری it = syncs.begin(); it != syncs.پای_ټکی(); it++)
		ړنګول *it;
}

RTLIL::Process *RTLIL::Process::clone() ثابت
{
	RTLIL::Process *new_proc = جديد RTLIL::Process;

	new_proc->ځانګړتیاوې = ځانګړتیاوې;

	RTLIL::Case_قاعده *rc_ptr = ریښه_case.clone();
	new_proc->ریښه_case = *rc_ptr;
	rc_ptr->switches.clear();
	ړنګول rc_ptr;

	لكل (خپلکاری &it : syncs)
		new_proc->syncs.ټېل_شا(it->clone());

	أرجع new_proc;
}

RTLIL::حافظه::~حافظه()
{
#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	RTLIL::حافظه::get_all_memorys()->erase(hashidx_);
#د_که_پای
}

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
ساكن std::map<بینښه صحيح, RTLIL::حافظه*> all_memorys;
std::map<بینښه صحيح, RTLIL::حافظه*> *RTLIL::حافظه::get_all_memorys(فراغ)
{
	أرجع &all_memorys;
}
#د_که_پای
YOSYS_NAMESPACE_پای_ټکی
