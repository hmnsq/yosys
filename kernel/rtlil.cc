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
		لكل (اندازه_t i = 0; i < StaticCellTypes::builder.شمار; i++) {
			خودکار &cell = StaticCellTypes::builder.cells[i];
			إذا (cell.features.is_ff)
				r.درج(cell.type);
		}
		أرجع r;
	}();
	أرجع res;
}

#تعریف_کن check(condition) log_assert(condition && "malformed Const union")

ثابت Const::bitvectype& Const::get_بیتها() ثابت {
	check(is_بیتها());
	أرجع *get_if_بیتها();
}

ثابت std::string& Const::get_str() ثابت {
	check(is_str());
	أرجع *get_if_str();
}

Const::bitvectype& Const::get_بیتها() {
	check(is_بیتها());
	أرجع *get_if_بیتها();
}

std::string& Const::get_str() {
	check(is_str());
	أرجع *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	جديد ((فراغ*)&str_) std::string(std::move(str));
	برچسب = backing_برچسب::string;
}

RTLIL::Const::Const(دراز دراز val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	حرف بایتها[] = {
		(حرف)(val >> 24), (حرف)(val >> 16), (حرف)(val >> 8), (حرف)val
	};
	جديد ((فراغ*)&str_) std::string(بایتها, 4);
	برچسب = backing_برچسب::string;
}

RTLIL::Const::Const(دراز دراز val, صحيح عرض)
{
	log_assert(عرض >= 0 && عرض < RTLIL::عرض_حد);
	flags = RTLIL::CONST_FLAG_NONE;
	إذا ((عرض & 7) == 0) {
		جديد ((فراغ*)&str_) std::string();
		برچسب = backing_برچسب::string;
		std::string& str = get_str();
		صحيح بایتها = عرض >> 3;
		علامتدار حرف sign_byte = val < 0 ? -1 : 0;
		str.resize(بایتها, sign_byte);
		بایتها = std::min<صحيح>(بایتها, حجم(val));
		لكل (صحيح i = 0; i < بایتها; i++) {
			str[str.اندازه() - 1 - i] = val;
			val = val >> 8;
		}
		أرجع;
	}

	جديد ((فراغ*)&بیتها_2) bitvectype();
	برچسب = backing_برچسب::بیتها;
	bitvectype& bv = get_بیتها();
	bv.reserve(عرض);
	لكل (صحيح i = 0; i < عرض; i++) {
		bv.افزودن_پشت((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State بیت, صحيح عرض)
{
	log_assert(عرض >= 0 && عرض < RTLIL::عرض_حد);
	flags = RTLIL::CONST_FLAG_NONE;
	جديد ((فراغ*)&بیتها_2) bitvectype();
	برچسب = backing_برچسب::بیتها;
	bitvectype& bv = get_بیتها();
	bv.reserve(عرض);
	لكل (صحيح i = 0; i < عرض; i++)
		bv.افزودن_پشت(بیت);
}

RTLIL::Const::Const(ثابت std::وکتور<منطقي> &بیتها)
{
	flags = RTLIL::CONST_FLAG_NONE;
	جديد ((فراغ*)&بیتها_2) bitvectype();
	برچسب = backing_برچسب::بیتها;
	bitvectype& bv = get_بیتها();
	bv.reserve(بیتها.اندازه());
	لكل (ثابت خودکار &b : بیتها)
		bv.emplace_پشت(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ثابت RTLIL::Const &other) {
	برچسب = other.برچسب;
	flags = other.flags;
	إذا (is_str())
		جديد ((فراغ*)&str_) std::string(other.get_str());
	وإلا إذا (is_بیتها())
		جديد ((فراغ*)&بیتها_2) bitvectype(other.get_بیتها());
	وإلا
		check(خطأ);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	برچسب = other.برچسب;
	flags = other.flags;
	إذا (is_str())
		جديد ((فراغ*)&str_) std::string(std::move(other.get_str()));
	وإلا إذا (is_بیتها())
		جديد ((فراغ*)&بیتها_2) bitvectype(std::move(other.get_بیتها()));
	وإلا
		check(خطأ);
}

RTLIL::Const &RTLIL::Const::عملگر =(ثابت RTLIL::Const &other) {
	flags = other.flags;
	إذا (other.is_str()) {
		إذا (!is_str()) {
			// sketchy zone
			check(is_بیتها());
			بیتها_2.~bitvectype();
			(فراغ)جديد ((فراغ*)&str_) std::string();
		}
		برچسب = other.برچسب;
		get_str() = other.get_str();
	} وإلا إذا (other.is_بیتها()) {
		إذا (!is_بیتها()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(فراغ)جديد ((فراغ*)&بیتها_2) bitvectype();
		}
		برچسب = other.برچسب;
		get_بیتها() = other.get_بیتها();
	} وإلا {
		check(خطأ);
	}
	أرجع *هذا;
}

RTLIL::Const::~Const() {
	إذا (is_بیتها())
		بیتها_2.~bitvectype();
	وإلا إذا (is_str())
		str_.~string();
	وإلا
		check(خطأ);
}

منطقي RTLIL::Const::عملگر<(ثابت RTLIL::Const &other) ثابت
{
	إذا (اندازه() != other.اندازه())
		أرجع اندازه() < other.اندازه();

	لكل (صحيح i = 0; i < اندازه(); i++)
		إذا ((*هذا)[i] != other[i])
			أرجع (*هذا)[i] < other[i];

	أرجع خطأ;
}

منطقي RTLIL::Const::عملگر ==(ثابت RTLIL::Const &other) ثابت
{
	إذا (is_str() && other.is_str())
		أرجع get_str() == other.get_str();
	إذا (is_بیتها() && other.is_بیتها())
		أرجع get_بیتها() == other.get_بیتها();

	إذا (اندازه() != other.اندازه())
		أرجع خطأ;

	لكل (صحيح i = 0; i < اندازه(); i++)
	إذا ((*هذا)[i] != other[i])
		أرجع خطأ;

	أرجع درست;
}

منطقي RTLIL::Const::عملگر !=(ثابت RTLIL::Const &other) ثابت
{
	أرجع !(*هذا == other);
}

std::وکتور<RTLIL::State>& RTLIL::Const::بیتها_داخلی()
{
	bitvectorize_داخلی();
	أرجع get_بیتها();
}

std::وکتور<RTLIL::State> RTLIL::Const::مقصد_بیتها() ثابت
{
	std::وکتور<State> v;
	لكل (خودکار بیت : *هذا)
		v.افزودن_پشت(بیت);
	أرجع v;
}

منطقي RTLIL::Const::as_bool() ثابت
{
	إذا (is_str()) {
		لكل (حرف ch : get_str())
			إذا (ch != 0)
				أرجع درست;
		أرجع خطأ;
	}

	ثابت bitvectype& bv = get_بیتها();
	لكل (اندازه_t i = 0; i < bv.اندازه(); i++)
		إذا (bv[i] == State::S1)
			أرجع درست;
	أرجع خطأ;
}

صحيح RTLIL::Const::as_int(منطقي is_signed) ثابت
{
	int32_t ret = 0;
	إذا (ثابت std::string *s = get_if_str()) {
		صحيح اندازه = Get_اندازه(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		لكل (صحيح i = std::min(4, اندازه); i > 0; i--)
			ret |= static_cast<بیعلامت حرف>((*s)[اندازه - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		إذا (is_signed && اندازه > 0 && اندازه < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << اندازه*8;
		أرجع ret;
	}

	ثابت bitvectype& bv = get_بیتها();
	صحيح significant_بیتها = std::min(Get_اندازه(bv), 32);
	لكل (صحيح i = 0; i < significant_بیتها; i++)
		إذا (bv[i] == State::S1)
			ret |= 1 << i;
	إذا (is_signed && significant_بیتها > 0 && significant_بیتها < 32 && bv.پشت() == State::S1 )
		ret |= UINT32_MAX << significant_بیتها;
	أرجع ret;
}

منطقي RTLIL::Const::convertible_مقصد_int(منطقي is_signed) ثابت
{
	خودکار اندازه = get_min_اندازه(is_signed);

	إذا (اندازه < 0)
		أرجع خطأ;

	// If it fits in 31 bits it is definitely convertible
	إذا (اندازه <= 31)
		أرجع درست;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	إذا (اندازه == 32) {
		إذا (is_signed)
			أرجع درست;
		أرجع (*هذا)[اندازه - 1] != State::S1;
	}

	أرجع خطأ;
}

std::optional<صحيح> RTLIL::Const::try_as_int(منطقي is_signed) ثابت
{
	إذا (!convertible_مقصد_int(is_signed))
		أرجع std::nullopt;
	أرجع as_int(is_signed);
}

صحيح RTLIL::Const::as_int_saturating(منطقي is_signed) ثابت
{
	إذا (!convertible_مقصد_int(is_signed)) {
		إذا (!is_signed)
			أرجع std::numeric_limits<صحيح>::max();

		ثابت خودکار min_اندازه = get_min_اندازه(is_signed);
		log_assert(min_اندازه > 0);
		ثابت خودکار neg = (*هذا)[min_اندازه - 1];
		أرجع neg ? std::numeric_limits<صحيح>::min() : std::numeric_limits<صحيح>::max();
	}
	أرجع as_int(is_signed);
}

فراغ RTLIL::Const::برچسب_bare_integer_const(ثابت std::string &مقدار)
{
	إذا (مقدار.خالی() || مقدار.پیدا_کن('\'') != std::string::npos)
		أرجع;
	اندازه_t شروع = (مقدار[0] == '-' || مقدار[0] == '+') ? 1 : 0;
	إذا (شروع < مقدار.اندازه() && std::all_از(مقدار.begin() + شروع, مقدار.انتها(), ::آیا_رقم))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

صحيح RTLIL::Const::get_min_اندازه(منطقي is_signed) ثابت
{
	إذا (خالی()) أرجع 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_بیت;
	إذا (is_signed)
		leading_بیت = (پشت() == RTLIL::State::Sx) ? RTLIL::State::S0 : پشت();
	وإلا
		leading_بیت = RTLIL::State::S0;

	خودکار idx = اندازه();
	طالما (idx > 0 && (*هذا)[idx -1] == leading_بیت) {
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
	خودکار idx = get_min_اندازه(is_signed);
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
			حالة S0: ret.افزودن_پشت('0'); اكسر;
			حالة S1: ret.افزودن_پشت('1'); اكسر;
			حالة Sx: ret.افزودن_پشت('x'); اكسر;
			حالة Sz: ret.افزودن_پشت('z'); اكسر;
			حالة Sa: ret += any; اكسر;
			حالة Sm: ret.افزودن_پشت('m'); اكسر;
		}
	أرجع ret;
}

RTLIL::Const RTLIL::Const::from_string(ثابت std::string &str)
{
	Const c;
	bitvectype& bv = c.get_بیتها();
	bv.reserve(str.اندازه());
	لكل (خودکار it = str.rbegin(); it != str.rend(); it++)
		بدل (*it) {
			حالة '0': bv.افزودن_پشت(State::S0); اكسر;
			حالة '1': bv.افزودن_پشت(State::S1); اكسر;
			حالة 'x': bv.افزودن_پشت(State::Sx); اكسر;
			حالة 'z': bv.افزودن_پشت(State::Sz); اكسر;
			حالة 'm': bv.افزودن_پشت(State::Sm); اكسر;
			افتراضي: bv.افزودن_پشت(State::Sa);
		}
	أرجع c;
}

std::string RTLIL::Const::decode_string() ثابت
{
	إذا (خودکار str = get_if_str())
		أرجع *str;

	ثابت bitvectype& bv = get_بیتها();
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
		check(is_بیتها());
		أرجع بیتها_2.اندازه();
	}
}

منطقي RTLIL::Const::خالی() ثابت {
	إذا (is_str())
		أرجع str_.خالی();
	وإلا {
		check(is_بیتها());
		أرجع بیتها_2.خالی();
	}
}

فراغ RTLIL::Const::bitvectorize_داخلی() {
	إذا (برچسب == backing_برچسب::بیتها)
		أرجع;

	check(is_str());

	bitvectype new_بیتها;

	new_بیتها.reserve(str_.اندازه() * 8);
	لكل (صحيح i = str_.اندازه() - 1; i >= 0; i--) {
		بیعلامت حرف ch = str_[i];
		لكل (صحيح j = 0; j < 8; j++) {
			new_بیتها.افزودن_پشت((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(فراغ)جديد ((فراغ*)&بیتها_2) bitvectype(std::move(new_بیتها));
		برچسب = backing_برچسب::بیتها;
	}
}

فراغ RTLIL::Const::append(ثابت RTLIL::Const &other) {
	bitvectorize_داخلی();
	bitvectype& bv = get_بیتها();
	bv.درج(bv.انتها(), other.begin(), other.انتها());
}

RTLIL::State RTLIL::Const::const_iterator::عملگر*() ثابت {
	إذا (خودکار bv = والد->get_if_بیتها())
		أرجع (*bv)[idx];

	صحيح char_idx = والد->get_str().اندازه() - idx / 8 - 1;
	منطقي بیت = (والد->get_str()[char_idx] & (1 << (idx % 8)));
	أرجع بیت ? State::S1 : State::S0;
}

منطقي RTLIL::Const::is_fully_zero() ثابت
{
	إذا (خودکار str = get_if_str()) {
		لكل (حرف ch : *str)
			إذا (ch != 0)
				أرجع خطأ;
		أرجع درست;
	}

	ثابت bitvectype& bv = get_بیتها();

	لكل (ثابت خودکار &بیت : bv)
		إذا (بیت != RTLIL::State::S0)
			أرجع خطأ;

	أرجع درست;
}

منطقي RTLIL::Const::is_fully_ones() ثابت
{
	إذا (خودکار str = get_if_str()) {
		لكل (حرف ch : *str)
			إذا (ch != (حرف)0xff)
				أرجع خطأ;
		أرجع درست;
	}

	ثابت bitvectype& bv = get_بیتها();
	لكل (ثابت خودکار &بیت : bv)
		إذا (بیت != RTLIL::State::S1)
			أرجع خطأ;

	أرجع درست;
}

منطقي RTLIL::Const::is_fully_def() ثابت
{
	إذا (is_str())
		أرجع درست;

	ثابت bitvectype& bv = get_بیتها();
	لكل (ثابت خودکار &بیت : bv)
		إذا (بیت != RTLIL::State::S0 && بیت != RTLIL::State::S1)
			أرجع خطأ;

	أرجع درست;
}

منطقي RTLIL::Const::is_fully_undef() ثابت
{
	إذا (خودکار str = get_if_str())
		أرجع str->خالی();

	ثابت bitvectype& bv = get_بیتها();
	لكل (ثابت خودکار &بیت : bv)
		إذا (بیت != RTLIL::State::Sx && بیت != RTLIL::State::Sz)
			أرجع خطأ;

	أرجع درست;
}

منطقي RTLIL::Const::is_fully_undef_x_only() ثابت
{
	إذا (خودکار str = get_if_str())
		أرجع str->خالی();

	ثابت bitvectype& bv = get_بیتها();
	لكل (ثابت خودکار &بیت : bv)
		إذا (بیت != RTLIL::State::Sx)
			أرجع خطأ;

	أرجع درست;
}

منطقي RTLIL::Const::is_onehot(صحيح *pos) ثابت
{
	منطقي یافت = خطأ;
	صحيح اندازه = Get_اندازه(*هذا);
	لكل (صحيح i = 0; i < اندازه; i++) {
		State بیت = (*هذا)[i];
		إذا (بیت != RTLIL::State::S0 && بیت != RTLIL::State::S1)
			أرجع خطأ;
		إذا (بیت == RTLIL::State::S1) {
			إذا (یافت)
				أرجع خطأ;
			إذا (pos)
				*pos = i;
			یافت = درست;
		}
	}
	أرجع یافت;
}

Hasher RTLIL::Const::hash_into(Hasher h) ثابت
{
	إذا (خودکار str = get_if_str())
		أرجع hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ثابت bitvectype &bv = get_بیتها();
	صحيح اندازه = Get_اندازه(bv);
	std::string packed;
	صحيح packed_اندازه = (اندازه + 7) >> 3;
	packed.resize(packed_اندازه, 0);
	لكل (صحيح bi = 0; bi < packed_اندازه; ++bi) {
		حرف ch = 0;
		صحيح انتها = std::min((bi + 1)*8, اندازه);
		لكل (صحيح i = bi*8; i < انتها; ++i) {
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

RTLIL::Const RTLIL::Const::استخراج(صحيح جابه_جایی, صحيح len, RTLIL::State padding) ثابت {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	لكل (صحيح i = جابه_جایی; i < جابه_جایی + len; i++)
		ret_bv.افزودن_پشت(i < Get_اندازه(*هذا) ? (*هذا)[i] : padding);
	أرجع RTLIL::Const(ret_bv);
}
#لغو_تعریف check /* check(condition) for Const */

منطقي RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ثابت
{
	أرجع صفات.شمار(id);
}

فراغ RTLIL::AttrObject::مجموعه_bool_attribute(RTLIL::IdString id, منطقي مقدار)
{
	إذا (مقدار)
		صفات[id] = RTLIL::Const(1);
	وإلا
		صفات.erase(id);
}

منطقي RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ثابت
{
	ثابت خودکار it = صفات.پیدا_کن(id);
	إذا (it == صفات.انتها())
		أرجع خطأ;
	أرجع it->دومی.as_bool();
}

فراغ RTLIL::AttrObject::مجموعه_string_attribute(RTLIL::IdString id, string مقدار)
{
	إذا (مقدار.خالی())
		صفات.erase(id);
	وإلا
		صفات[id] = مقدار;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ثابت
{
	std::string مقدار;
	ثابت خودکار it = صفات.پیدا_کن(id);
	إذا (it != صفات.انتها())
		مقدار = it->دومی.decode_string();
	أرجع مقدار;
}

فراغ RTLIL::AttrObject::مجموعه_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &داده)
{
	string attrval;
	لكل (ثابت خودکار &s : داده) {
		إذا (!attrval.خالی())
			attrval += "|";
		attrval += s;
	}
	مجموعه_string_attribute(id, attrval);
}

فراغ RTLIL::AttrObject::جمع_کن_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &داده)
{
	pool<string> union_داده = get_strpool_attribute(id);
	union_داده.درج(داده.begin(), داده.انتها());
	إذا (!union_داده.خالی())
		مجموعه_strpool_attribute(id, union_داده);
}

فراغ RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->صفات = std::move(buf);
}

نطاق {
	قالب<نام_نوع F>
	فراغ walk_process_attr_objects(RTLIL::Process *process, F بازدید) {
		بازدید(process);
		std::وکتور<RTLIL::Case_قاعده*> case_پشته{&process->ریشه_case};
		طالما (!case_پشته.خالی()) {
			RTLIL::Case_قاعده *cs = case_پشته.پشت();
			case_پشته.برداشت_پشت();
			بازدید(cs);
			لكل (خودکار *sw : cs->switches) {
				بازدید(sw);
				لكل (خودکار *case_ : sw->cases)
					case_پشته.افزودن_پشت(case_);
			}
		}
		لكل (خودکار *متزامن : process->syncs)
			لكل (خودکار &mwa : متزامن->mem_write_actions)
				بازدید(&mwa);
	}

	قالب<نام_نوع F>
	فراغ walk_attr_objects(RTLIL::Design *design, F بازدید) {
		لكل (خودکار &[_, module] : design->modules_) {
			بازدید(module);
			لكل (خودکار &[_, wire] : module->wires_)
				بازدید(wire);
			لكل (خودکار &[_, mem] : module->memories)
				بازدید(mem);
			لكل (خودکار &[_, cell] : module->cells_)
				بازدید(cell);
			لكل (خودکار &[_, process] : module->processes)
				walk_process_attr_objects(process, بازدید);
		}
	}
}


اندازه_t RTLIL::Design::gc_twines()
{
	int64_t شروع = PerformanceTimer::query();
	pool<IdString> زنده;
	خودکار ریشه = [&](IdString ref) {
		إذا (ref != IdString::Null)
			زنده.درج(ref);
	};

	walk_attr_objects(هذا, [&](ثابت RTLIL::AttrObject *obj) {
		لكل (خودکار &attr : obj->صفات)
			ریشه(attr.اولی);
	});

	ریشه(selected_فعال_module);

	لكل (خودکار &[name, module] : modules_) {
		ریشه(name);
		لكل (IdString پورت : module->ports)
			ریشه(پورت);
		لكل (IdString param : module->avail_parameters)
			ریشه(param);
		لكل (خودکار &[param, _] : module->parameter_default_مقادیر)
			ریشه(param);
		لكل (خودکار &[name, _] : module->memories)
			ریشه(name);
		لكل (خودکار &[name, _] : module->processes)
			ریشه(name);
		لكل (خودکار &[name, wire] : module->wires_) {
			ریشه(name);
			إذا (wire->معلوم_driver())
				ریشه(wire->driver_پورت());
		}
		لكل (خودکار &[name, cell] : module->cells_) {
			ریشه(name);
			ریشه(cell->type);
			لكل (خودکار &conn : cell->connections())
				ریشه(conn.اولی);
			لكل (خودکار &param : cell->parameters)
				ریشه(param.اولی);
		}
		لكل (خودکار &[_, process] : module->processes) {
			std::وکتور<RTLIL::Case_قاعده*> case_پشته{&process->ریشه_case};
			طالما (!case_پشته.خالی()) {
				RTLIL::Case_قاعده *cs = case_پشته.پشت();
				case_پشته.برداشت_پشت();
				لكل (خودکار *sw : cs->switches) {
					لكل (خودکار *case_ : sw->cases)
						case_پشته.افزودن_پشت(case_);
				}
			}
			لكل (خودکار *متزامن : process->syncs) {
				لكل (خودکار &mwa : متزامن->mem_write_actions)
					ریشه(mwa.memid);
			}
		}
	}

	خودکار ریشه_selection = [&](ثابت RTLIL::Selection &sel) {
		لكل (IdString m : sel.selected_modules)
			ریشه(m);
		لكل (خودکار &[m, members] : sel.selected_members) {
			ریشه(m);
			لكل (IdString member : members)
				ریشه(member);
		}
	};

	لكل (خودکار &[name, sel] : selection_vars) {
		ریشه(name);
		ریشه_selection(sel);
	}

	لكل (خودکار &sel : selection_پشته)
		ریشه_selection(sel);

	اندازه_t erased = twines_.gc(زنده);

	int64_t زمان_ns = PerformanceTimer::query() - شروع;
	Pass::تفریق_from_current_runtime_ns(زمان_ns);
	twine_gc_ns += زمان_ns;
	++twine_gc_شمار;
	أرجع erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ثابت
{
	pool<string> داده;
	إذا (صفات.شمار(id) != 0)
		لكل (خودکار s : تقسیم_نشانهها(get_string_attribute(id), "|"))
			داده.درج(s);
	أرجع داده;
}

فراغ RTLIL::AttrObject::مجموعه_hdlname_attribute(ثابت وکتور<string> &hierarchy)
{
	string attrval;
	لكل (ثابت خودکار &ident : hierarchy) {
		إذا (!attrval.خالی())
			attrval += " ";
		attrval += ident;
	}
	مجموعه_string_attribute(ID::hdlname, attrval);
}

وکتور<string> RTLIL::AttrObject::get_hdlname_attribute() ثابت
{
	أرجع تقسیم_نشانهها(get_string_attribute(ID::hdlname), " ");
}

فراغ RTLIL::AttrObject::مجموعه_intvec_attribute(RTLIL::IdString id, ثابت وکتور<صحيح> &داده)
{
	std::stringstream attrval;
	لكل (خودکار &i : داده) {
		إذا (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	صفات[id] = RTLIL::Const(attrval.str());
}

وکتور<صحيح> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ثابت
{
	وکتور<صحيح> داده;
	خودکار it = صفات.پیدا_کن(id);
	إذا (it != صفات.انتها())
		لكل (ثابت خودکار &s : تقسیم_نشانهها(صفات.at(id).decode_string())) {
			حرف *انتها = اشاره_تهی;
			errno = 0;
			دراز مقدار = strtol(s.c_str(), &انتها, 10);
			إذا (انتها != s.c_str() + s.اندازه())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			إذا (errno == ERANGE || مقدار < INT_MIN || مقدار > بیشترین_صحیح)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			داده.افزودن_پشت(مقدار);
		}
	أرجع داده;
}

منطقي RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ثابت
{
	إذا (current_design != اشاره_تهی) {
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
		أرجع درست;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (پر_selection)
		أرجع درست;
	إذا (selected_modules.شمار(mod_name) > 0)
		أرجع درست;
	إذا (selected_members.شمار(mod_name) > 0)
		أرجع درست;
	أرجع خطأ;
}

منطقي RTLIL::Selection::selected_کامل_module(RTLIL::IdString mod_name) ثابت
{
	إذا (complete_selection)
		أرجع درست;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (پر_selection)
		أرجع درست;
	إذا (selected_modules.شمار(mod_name) > 0)
		أرجع درست;
	أرجع خطأ;
}

منطقي RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	إذا (complete_selection)
		أرجع درست;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (پر_selection)
		أرجع درست;
	إذا (selected_modules.شمار(mod_name) > 0)
		أرجع درست;
	إذا (selected_members.شمار(mod_name) > 0)
		إذا (selected_members.at(mod_name).شمار(memb_name) > 0)
			أرجع درست;
	أرجع خطأ;
}

فراغ RTLIL::Selection::optimize(RTLIL::Design *design)
{
	إذا (design != current_design) {
		current_design = design;
	}

	إذا (selects_boxes && پر_selection)
		complete_selection = درست;
	إذا (complete_selection) {
		پر_selection = خطأ;
		selects_boxes = درست;
	}
	إذا (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		أرجع;
	}

	std::وکتور<RTLIL::IdString> del_list, جمع_کن_list;

	del_list.clear();
	لكل (خودکار mod_name : selected_modules) {
		إذا (current_design->module(mod_name) == اشاره_تهی || (!selects_boxes && boxed_module(mod_name)))
			del_list.افزودن_پشت(mod_name);
		selected_members.erase(mod_name);
	}
	لكل (خودکار mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	لكل (خودکار &it : selected_members)
		إذا (current_design->module(it.اولی) == اشاره_تهی || (!selects_boxes && boxed_module(it.اولی)))
			del_list.افزودن_پشت(it.اولی);
	لكل (خودکار mod_name : del_list)
		selected_members.erase(mod_name);

	لكل (خودکار &it : selected_members) {
		del_list.clear();
		لكل (خودکار memb_name : it.دومی)
			إذا (current_design->module(it.اولی)->شمار_id(memb_name) == 0)
				del_list.افزودن_پشت(memb_name);
		لكل (خودکار memb_name : del_list)
			it.دومی.erase(memb_name);
	}

	del_list.clear();
	جمع_کن_list.clear();
	لكل (خودکار &it : selected_members)
		إذا (it.دومی.اندازه() == 0)
			del_list.افزودن_پشت(it.اولی);
		وإلا إذا (it.دومی.اندازه() == current_design->module(it.اولی)->wires_.اندازه() + current_design->module(it.اولی)->memories.اندازه() +
				current_design->module(it.اولی)->cells_.اندازه() + current_design->module(it.اولی)->processes.اندازه())
			جمع_کن_list.افزودن_پشت(it.اولی);
	لكل (خودکار mod_name : del_list)
		selected_members.erase(mod_name);
	لكل (خودکار mod_name : جمع_کن_list) {
		selected_members.erase(mod_name);
		selected_modules.درج(mod_name);
	}

	إذا (selected_modules.اندازه() == current_design->modules_.اندازه()) {
		selected_modules.clear();
		selected_members.clear();
		إذا (selects_boxes)
			complete_selection = درست;
		وإلا
			پر_selection = درست;
	}
}

فراغ RTLIL::Selection::clear()
{
	پر_selection = خطأ;
	complete_selection = خطأ;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_تعریف_میکند (جديد define_map_t)
{
	ساكن بیعلامت صحيح hashidx_شمار = 123456789;
	hashidx_شمار = mkhash_xorshift(hashidx_شمار);
	hashidx_ = hashidx_شمار;

	refcount_modules_ = 0;
	selected_فعال_module.clear();
	افزودن_پر_selection();

	RTLIL::Design::get_all_designs()->درج(std::pair<بیعلامت صحيح, RTLIL::Design*>(hashidx_, هذا));
}

RTLIL::Design::~Design()
{
	لكل (خودکار &pr : modules_)
		حذف pr.دومی;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

ساكن std::map<بیعلامت صحيح, RTLIL::Design*> all_designs;
std::map<بیعلامت صحيح, RTLIL::Design*> *RTLIL::Design::get_all_designs(فراغ)
{
	أرجع &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	أرجع RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ثابت RTLIL::Module *RTLIL::Design::module(IdString id) ثابت {
	twines_.check_owned(id);
	أرجع modules_.شمار(id) ? modules_.at(id) : عدم;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	أرجع modules_.شمار(id) ? modules_.at(id) : عدم;
}

RTLIL::Module *RTLIL::Design::بالاترین_module() ثابت
{
	RTLIL::Module *module = اشاره_تهی;
	صحيح module_شمار = 0;

	لكل (خودکار mod : selected_modules()) {
		إذا (mod->get_bool_attribute(ID::بالاترین))
			أرجع mod;
		module_شمار++;
		module = mod;
	}

	أرجع module_شمار == 1 ? module : اشاره_تهی;
}

RTLIL::Module *RTLIL::Design::جمع_کن_Module(RTLIL::IdString name)
{
	إذا (modules_.شمار(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = جديد RTLIL::Module;
	modules_[name] = module;
	module->design = هذا;
	module->name = name;

	لكل (خودکار mon : monitors)
		mon->notify_module_جمع_کن(module);

	إذا (yosys_xtrace) {
		لگاریتم("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	أرجع module;
}

فراغ RTLIL::Design::scratchpad_unset(ثابت std::string &varname)
{
	scratchpad.erase(varname);
}

فراغ RTLIL::Design::scratchpad_مجموعه_int(ثابت std::string &varname, صحيح مقدار)
{
	scratchpad[varname] = stringf("%d", مقدار);
}

فراغ RTLIL::Design::scratchpad_مجموعه_bool(ثابت std::string &varname, منطقي مقدار)
{
	scratchpad[varname] = مقدار ? "true" : "false";
}

فراغ RTLIL::Design::scratchpad_مجموعه_string(ثابت std::string &varname, std::string مقدار)
{
	scratchpad[varname] = std::move(مقدار);
}

صحيح RTLIL::Design::scratchpad_get_int(ثابت std::string &varname, صحيح default_مقدار_2) ثابت
{
	خودکار it = scratchpad.پیدا_کن(varname);
	إذا (it == scratchpad.انتها())
		أرجع default_مقدار_2;

	ثابت std::string &str = it->دومی;

	إذا (str == "0" || str == "false")
		أرجع 0;

	إذا (str == "1" || str == "true")
		أرجع 1;

	حرف *endptr = اشاره_تهی;
	دراز صحيح parsed_مقدار = strtol(str.c_str(), &endptr, 10);
	أرجع *endptr ? default_مقدار_2 : parsed_مقدار;
}

منطقي RTLIL::Design::scratchpad_get_bool(ثابت std::string &varname, منطقي default_مقدار_2) ثابت
{
	خودکار it = scratchpad.پیدا_کن(varname);
	إذا (it == scratchpad.انتها())
		أرجع default_مقدار_2;

	ثابت std::string &str = it->دومی;

	إذا (str == "0" || str == "false")
		أرجع خطأ;

	إذا (str == "1" || str == "true")
		أرجع درست;

	أرجع default_مقدار_2;
}

std::string RTLIL::Design::scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_مقدار_2) ثابت
{
	خودکار it = scratchpad.پیدا_کن(varname);
	إذا (it == scratchpad.انتها())
		أرجع default_مقدار_2;

	أرجع it->دومی;
}

فراغ RTLIL::Design::remove(RTLIL::Module *module)
{
	لكل (خودکار mon : monitors)
		mon->notify_module_del(module);

	إذا (yosys_xtrace) {
		لگاریتم("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	حذف module;
}

فراغ RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	جمع_کن(module);
}

فراغ RTLIL::Design::مرتب_سازی()
{
	scratchpad.مرتب_سازی();
	modules_.مرتب_سازی(مرتب_سازی_by_id_str(twines_));
	لكل (خودکار &it : modules_)
		it.دومی->مرتب_سازی();
}

فراغ RTLIL::Design::مرتب_سازی_modules()
{
	scratchpad.مرتب_سازی();
	modules_.مرتب_سازی(مرتب_سازی_by_id_str(twines_));
}

فراغ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

فراغ RTLIL::Design::check()
{
#اگر_تعریف_نشده NDEBUG
	log_assert(!selection_پشته.خالی());
	صحيح pool_اندازه = 0;
	لكل (خودکار &it : modules_)
		pool_اندازه = std::max(pool_اندازه, ThreadPool::work_pool_اندازه(0, it.دومی->cells_اندازه(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_اندازه);
	لكل (خودکار &it : modules_) {
		log_assert(هذا == it.دومی->design);
		log_assert(it.اولی == it.دومی->name);
		check_module(it.دومی, thread_pool);
	}
#پایان_اگر
}

فراغ RTLIL::Design::optimize()
{
	لكل (خودکار &it : modules_)
		it.دومی->optimize();
	لكل (خودکار &it : selection_پشته)
		it.optimize(هذا);
	لكل (خودکار &it : selection_vars)
		it.دومی.optimize(هذا);
}

فراغ RTLIL::Design::clone_into(RTLIL::Design *dst) ثابت
{
	log_assert(dst->modules_.خالی());
	dst->twines_ = twines_;
	لكل (خودکار it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->دومی->clone(dst);
}

منطقي RTLIL::Design::selected_module(IdString mod_name) ثابت
{
	إذا (!selected_فعال_module.خالی() && mod_name != selected_فعال_module)
		أرجع خطأ;
	أرجع selection().selected_module(mod_name);
}

منطقي RTLIL::Design::selected_کامل_module(RTLIL::IdString mod_name) ثابت
{
	إذا (!selected_فعال_module.خالی() && mod_name != selected_فعال_module)
		أرجع خطأ;
	أرجع selection().selected_کامل_module(mod_name);
}

منطقي RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	إذا (!selected_فعال_module.خالی() && mod_name != selected_فعال_module)
		أرجع خطأ;
	أرجع selection().selected_member(mod_name, memb_name);
}

منطقي RTLIL::Design::selected_module(RTLIL::Module *mod) ثابت
{
	أرجع selected_module(mod->name);
}

منطقي RTLIL::Design::selected_کامل_module(RTLIL::Module *mod) ثابت
{
	أرجع selected_کامل_module(mod->name);
}

فراغ RTLIL::Design::افزودن_selection(RTLIL::Selection sel)
{
	sel.current_design = هذا;
	selection_پشته.افزودن_پشت(sel);
}

فراغ RTLIL::Design::افزودن_خالی_selection()
{
	افزودن_selection(RTLIL::Selection::خالی_Selection(هذا));
}

فراغ RTLIL::Design::افزودن_پر_selection()
{
	افزودن_selection(RTLIL::Selection::پر_Selection(هذا));
}

فراغ RTLIL::Design::افزودن_complete_selection()
{
	افزودن_selection(RTLIL::Selection::CompleteSelection(هذا));
}

فراغ RTLIL::Design::برداشت_selection()
{
	selection_پشته.برداشت_پشت();
	// Default to a full_selection if we ran out of stack
	إذا (selection_پشته.خالی())
		افزودن_پر_selection();
}

std::string RTLIL::Design::مقصد_rtlil_str(منطقي only_selected) ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(هذا), only_selected, درست, خطأ, RTLIL_BACKEND::Dump_نما::Readable);
	أرجع f.str();
}

std::وکتور<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ثابت
{
	منطقي include_partials = partials == RTLIL::SELECT_ALL;
	منطقي exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	منطقي ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::وکتور<RTLIL::Module*> نتیجه;
	نتیجه.reserve(modules_.اندازه());
	لكل (خودکار &it : modules_) {
		إذا (selected_کامل_module(it.اولی) || (include_partials && selected_module(it.اولی))) {
			إذا (!(exclude_boxes && it.دومی->get_blackbox_attribute(ignore_wb)))
				نتیجه.افزودن_پشت(it.دومی);
			وإلا
				بدل (boxes)
				{
				حالة RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.دومی);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.دومی);
					اكسر;
				حالة RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.دومی);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.دومی);
					اكسر;
				حالة RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.دومی);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.دومی);
					اكسر;
				افتراضي:
					اكسر;
				}
		} وإلا إذا (!include_partials && selected_module(it.اولی)) {
			بدل(partials)
			{
			حالة RTLIL::SELECT_کامل_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.دومی);
				اكسر;
			حالة RTLIL::SELECT_کامل_ERR:
				log_error("Unsupported partially selected module %s.\n", it.دومی);
				اكسر;
			حالة RTLIL::SELECT_کامل_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.دومی);
				اكسر;
			افتراضي:
				اكسر;
			}
		}
	}
	أرجع نتیجه;
}

RTLIL::Module::Module()
{
	ساكن بیعلامت صحيح hashidx_شمار = 123456789;
	hashidx_شمار = mkhash_xorshift(hashidx_شمار);
	hashidx_ = hashidx_شمار;

	design = اشاره_تهی;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->درج(std::pair<بیعلامت صحيح, RTLIL::Module*>(hashidx_, هذا));
#پایان_اگر
}

RTLIL::Module::~Module()
{
	لكل (خودکار &pr : wires_)
		حذف pr.دومی;
	لكل (خودکار &pr : memories)
		حذف pr.دومی;
	لكل (خودکار &pr : cells_)
		حذف pr.دومی;
	لكل (خودکار &pr : processes)
		حذف pr.دومی;
#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#پایان_اگر
}

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
ساكن std::map<بیعلامت صحيح, RTLIL::Module*> all_modules;
std::map<بیعلامت صحيح, RTLIL::Module*> *RTLIL::Module::get_all_modules(فراغ)
{
	أرجع &all_modules;
}
#پایان_اگر

فراغ RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	لكل (خودکار it = wires_.begin(); it != wires_.انتها(); ++it)
		إذا (!it->دومی->پورت_درون_داد && !it->دومی->پورت_output)
			delwires.درج(it->دومی);

	لكل (خودکار it = memories.begin(); it != memories.انتها(); ++it)
		حذف it->دومی;
	memories.clear();

	لكل (خودکار it = cells_.begin(); it != cells_.انتها(); ++it)
		حذف it->دومی;
	cells_.clear();

	لكل (خودکار it = processes.begin(); it != processes.انتها(); ++it)
		حذف it->دومی;
	processes.clear();

	connections_.clear();

	remove(delwires);
	مجموعه_bool_attribute(ID::blackbox);
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

اندازه_t RTLIL::Module::شمار_id(RTLIL::IdString id)
{
	أرجع wires_.شمار(id) + cells_.شمار(id) + memories.شمار(id) + processes.شمار(id);
}

#اگر_تعریف_نشده NDEBUG
نطاق {
	بنية داخلی_Cell_Checker
	{
		ثابت RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		داخلی_Cell_Checker(ثابت RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		فراغ error(صحيح linenr, std::optional<std::string> error_پیام = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			إذا (error_پیام) {
				log_error("%s in internal cell %s\n", *error_پیام, cell_loc);
			} وإلا {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		صحيح param(RTLIL::IdString name)
		{
			خودکار it = cell->parameters.پیدا_کن(name);
			إذا (it == cell->parameters.انتها()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.درج(name);
			أرجع it->دومی.as_int();
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

		فراغ param_بیتها(RTLIL::IdString name, صحيح عرض)
		{
			param(name);
			إذا (Get_اندازه(cell->parameters.at(name)) != عرض)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			أرجع cell->parameters.at(name).decode_string();
		}

		فراغ پورت(RTLIL::IdString name, صحيح عرض)
		{
			خودکار it = cell->connections_.پیدا_کن(name);
			إذا (it == cell->connections_.انتها()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			إذا (Get_اندازه(it->دومی) != عرض) {
				std::string err = stringf("Expected port to have width %d", عرض);
				error(__LINE__, err);
			}
			expected_ports.درج(name);
		}

		فراغ check_expected(منطقي check_matched_sign = خطأ)
		{
			لكل (خودکار &para : cell->parameters)
				إذا (expected_params.شمار(para.اولی) == 0)
					error(__LINE__);
			لكل (خودکار &conn : cell->connections())
				إذا (expected_ports.شمار(conn.اولی) == 0)
					error(__LINE__);

			إذا (check_matched_sign) {
				log_assert(expected_params.شمار(ID::A_SIGNED) != 0 && expected_params.شمار(ID::B_SIGNED) != 0);
				منطقي a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				منطقي b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				إذا (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		فراغ check()
		{
			إذا (cell->type.is_عمومی() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				أرجع;

			إذا (cell->type.درون(ID($buf), ID($barrier))) {
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::Y, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.درون(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				پورت(ID::A, param(ID::A_عرض));
				پورت(ID::Y, param(ID::Y_عرض));
				check_expected();
				أرجع;
			}

			إذا (cell->type.درون(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورت(ID::A, param(ID::A_عرض));
				پورت(ID::B, param(ID::B_عرض));
				پورت(ID::Y, param(ID::Y_عرض));
				check_expected(درست);
				أرجع;
			}

			إذا (cell->type.درون(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				پورت(ID::A, param(ID::A_عرض));
				پورت(ID::Y, param(ID::Y_عرض));
				check_expected();
				أرجع;
			}

			إذا (cell->type.درون(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/خطأ);
				پورت(ID::A, param(ID::A_عرض));
				پورت(ID::B, param(ID::B_عرض));
				پورت(ID::Y, param(ID::Y_عرض));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type.درون(ID($لغزش), ID($shiftx))) {
				إذا (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/خطأ);
				} وإلا {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				پورت(ID::A, param(ID::A_عرض));
				پورت(ID::B, param(ID::B_عرض));
				پورت(ID::Y, param(ID::Y_عرض));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type.درون(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورت(ID::A, param(ID::A_عرض));
				پورت(ID::B, param(ID::B_عرض));
				پورت(ID::Y, param(ID::Y_عرض));
				check_expected(درست);
				أرجع;
			}

			إذا (cell->type.درون(ID($جمع_کن), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($توان))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورت(ID::A, param(ID::A_عرض));
				پورت(ID::B, param(ID::B_عرض));
				پورت(ID::Y, param(ID::Y_عرض));
				check_expected(cell->type != ID($توان));
				أرجع;
			}

			إذا (cell->type == ID($fa)) {
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::B, param(ID::عرض_2));
				پورت(ID::C, param(ID::عرض_2));
				پورت(ID::X, param(ID::عرض_2));
				پورت(ID::Y, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($lcu)) {
				پورت(ID::P, param(ID::عرض_2));
				پورت(ID::G, param(ID::عرض_2));
				پورت(ID::CI, 1);
				پورت(ID::CO, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورت(ID::A, param(ID::A_عرض));
				پورت(ID::B, param(ID::B_عرض));
				پورت(ID::CI, 1);
				پورت(ID::BI, 1);
				پورت(ID::X, param(ID::Y_عرض));
				پورت(ID::Y, param(ID::Y_عرض));
				پورت(ID::CO, param(ID::Y_عرض));
				check_expected(درست);
				أرجع;
			}

			إذا (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_عرض);
				پورت(ID::A, param(ID::A_عرض));
				پورت(ID::B, param(ID::B_عرض));
				پورت(ID::Y, param(ID::Y_عرض));
				check_expected();
				Macc().from_cell(cell);
				أرجع;
			}

			إذا (cell->type == ID($macc_v2)) {
				إذا (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				إذا (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_بیتها(ID::حاصلضرب_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_بیتها(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_بیتها(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_بیتها(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_بیتها(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				إذا (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_بیتها(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_بیتها(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_بیتها(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ثابت Const &a_عرض = cell->getParam(ID::A_WIDTHS);
				ثابت Const &b_عرض = cell->getParam(ID::B_WIDTHS);
				ثابت Const &c_عرض = cell->getParam(ID::C_WIDTHS);
				صحيح a_عرض_جمع = 0, b_عرض_جمع = 0, c_عرض_جمع = 0;
				لكل (صحيح i = 0; i < param(ID::NPRODUCTS); i++) {
					a_عرض_جمع += a_عرض.استخراج(16 * i, 16).as_int(خطأ);
					b_عرض_جمع += b_عرض.استخراج(16 * i, 16).as_int(خطأ);
				}
				لكل (صحيح i = 0; i < param(ID::NADDENDS); i++) {
					c_عرض_جمع += c_عرض.استخراج(16 * i, 16).as_int(خطأ);
				}
				پورت(ID::A, a_عرض_جمع);
				پورت(ID::B, b_عرض_جمع);
				پورت(ID::C, c_عرض_جمع);
				پورت(ID::Y, param(ID::Y_عرض));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				پورت(ID::A, param(ID::A_عرض));
				پورت(ID::Y, param(ID::Y_عرض));
				check_expected();
				أرجع;
			}

			إذا (cell->type.درون(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				پورت(ID::A, param(ID::A_عرض));
				پورت(ID::B, param(ID::B_عرض));
				پورت(ID::Y, param(ID::Y_عرض));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type == ID($برش)) {
				param(ID::جابه_جایی_2);
				پورت(ID::A, param(ID::A_عرض));
				پورت(ID::Y, param(ID::Y_عرض));
				إذا (param(ID::جابه_جایی_2) + param(ID::Y_عرض) > param(ID::A_عرض))
					error(__LINE__);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($concat)) {
				پورت(ID::A, param(ID::A_عرض));
				پورت(ID::B, param(ID::B_عرض));
				پورت(ID::Y, param(ID::A_عرض) + param(ID::B_عرض));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mux)) {
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::B, param(ID::عرض_2));
				پورت(ID::S, 1);
				پورت(ID::Y, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($pmux)) {
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::B, param(ID::عرض_2) * param(ID::S_عرض));
				پورت(ID::S, param(ID::S_عرض));
				پورت(ID::Y, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bmux)) {
				پورت(ID::A, param(ID::عرض_2) << param(ID::S_عرض));
				پورت(ID::S, param(ID::S_عرض));
				پورت(ID::Y, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($demux)) {
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::S, param(ID::S_عرض));
				پورت(ID::Y, param(ID::عرض_2) << param(ID::S_عرض));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($lut)) {
				param(ID::LUT);
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sop)) {
				param(ID::عمق_2);
				param(ID::جدول);
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sr)) {
				param_bool(ID::مجموعه_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورت(ID::مجموعه_2, param(ID::عرض_2));
				پورت(ID::CLR, param(ID::عرض_2));
				پورت(ID::Q,   param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($ff)) {
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				پورت(ID::CLK, 1);
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				پورت(ID::CLK, 1);
				پورت(ID::EN, 1);
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::مجموعه_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورت(ID::CLK, 1);
				پورت(ID::مجموعه_2, param(ID::عرض_2));
				پورت(ID::CLR, param(ID::عرض_2));
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffsre)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::مجموعه_POLARITY);
				param_bool(ID::CLR_POLARITY);
				param_bool(ID::EN_POLARITY);
				پورت(ID::CLK, 1);
				پورت(ID::EN, 1);
				پورت(ID::مجموعه_2, param(ID::عرض_2));
				پورت(ID::CLR, param(ID::عرض_2));
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بیتها(ID::ARST_مقدار, param(ID::عرض_2));
				پورت(ID::CLK, 1);
				پورت(ID::ARST, 1);
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_بیتها(ID::SRST_مقدار, param(ID::عرض_2));
				پورت(ID::CLK, 1);
				پورت(ID::SRST, 1);
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.درون(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_بیتها(ID::SRST_مقدار, param(ID::عرض_2));
				پورت(ID::CLK, 1);
				پورت(ID::EN, 1);
				پورت(ID::SRST, 1);
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بیتها(ID::ARST_مقدار, param(ID::عرض_2));
				پورت(ID::CLK, 1);
				پورت(ID::EN, 1);
				پورت(ID::ARST, 1);
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				پورت(ID::CLK, 1);
				پورت(ID::ALOAD, 1);
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::AD, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($aldffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				پورت(ID::CLK, 1);
				پورت(ID::EN, 1);
				پورت(ID::ALOAD, 1);
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::AD, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				پورت(ID::EN, 1);
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_بیتها(ID::ARST_مقدار, param(ID::عرض_2));
				پورت(ID::EN, 1);
				پورت(ID::ARST, 1);
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::مجموعه_POLARITY);
				param_bool(ID::CLR_POLARITY);
				پورت(ID::EN, 1);
				پورت(ID::مجموعه_2, param(ID::عرض_2));
				پورت(ID::CLR, param(ID::عرض_2));
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_بیتها);
				param(ID::STATE_رقم);
				param(ID::STATE_رقم_LOG2);
				param(ID::STATE_RST);
				param_بیتها(ID::STATE_جدول, param(ID::STATE_بیتها) * param(ID::STATE_رقم));
				param(ID::TRANS_رقم);
				param_بیتها(ID::TRANS_جدول, param(ID::TRANS_رقم) * (2*param(ID::STATE_رقم_LOG2) + param(ID::CTRL_درون_عرض) + param(ID::CTRL_بیرون_عرض)));
				پورت(ID::CLK, 1);
				پورت(ID::ARST, 1);
				پورت(ID::CTRL_درون, param(ID::CTRL_درون_عرض));
				پورت(ID::CTRL_بیرون, param(ID::CTRL_بیرون_عرض));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				پورت(ID::CLK, 1);
				پورت(ID::EN, 1);
				پورت(ID::ADDR, param(ID::ABITS));
				پورت(ID::داده_2, param(ID::عرض_2));
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
				param_بیتها(ID::ARST_مقدار, param(ID::عرض_2));
				param_بیتها(ID::SRST_مقدار, param(ID::عرض_2));
				param_بیتها(ID::INIT_مقدار, param(ID::عرض_2));
				پورت(ID::CLK, 1);
				پورت(ID::EN, 1);
				پورت(ID::ARST, 1);
				پورت(ID::SRST, 1);
				پورت(ID::ADDR, param(ID::ABITS));
				پورت(ID::داده_2, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::اولویت);
				پورت(ID::CLK, 1);
				پورت(ID::EN, param(ID::عرض_2));
				پورت(ID::ADDR, param(ID::ABITS));
				پورت(ID::داده_2, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memwr_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PORTID);
				param(ID::اولویت_ماسک_2);
				پورت(ID::CLK, 1);
				پورت(ID::EN, param(ID::عرض_2));
				پورت(ID::ADDR, param(ID::ABITS));
				پورت(ID::داده_2, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::اولویت);
				پورت(ID::ADDR, param(ID::ABITS));
				پورت(ID::داده_2, param(ID::عرض_2) * param(ID::واژگان));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::اولویت);
				پورت(ID::ADDR, param(ID::ABITS));
				پورت(ID::داده_2, param(ID::عرض_2) * param(ID::واژگان));
				پورت(ID::EN, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mem)) {
				param(ID::MEMID);
				param(ID::اندازه_2);
				param(ID::جابه_جایی_2);
				param(ID::INIT);
				param_بیتها(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_بیتها(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_بیتها(ID::RD_TRANSPARENT, max(1, param(ID::RD_PORTS)));
				param_بیتها(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_بیتها(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				پورت(ID::RD_CLK, param(ID::RD_PORTS));
				پورت(ID::RD_EN, param(ID::RD_PORTS));
				پورت(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				پورت(ID::RD_داده, param(ID::RD_PORTS) * param(ID::عرض_2));
				پورت(ID::WR_CLK, param(ID::WR_PORTS));
				پورت(ID::WR_EN, param(ID::WR_PORTS) * param(ID::عرض_2));
				پورت(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				پورت(ID::WR_داده, param(ID::WR_PORTS) * param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mem_v2)) {
				param(ID::MEMID);
				param(ID::اندازه_2);
				param(ID::جابه_جایی_2);
				param(ID::INIT);
				param_بیتها(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_بیتها(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_بیتها(ID::RD_TRANSPARENCY_ماسک, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_بیتها(ID::RD_COLLISION_X_ماسک, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_بیتها(ID::RD_WIDE_CONTINUATION, max(1, param(ID::RD_PORTS)));
				param_بیتها(ID::RD_CE_OVER_SRST, max(1, param(ID::RD_PORTS)));
				param_بیتها(ID::RD_ARST_مقدار, max(1, param(ID::RD_PORTS) * param(ID::عرض_2)));
				param_بیتها(ID::RD_SRST_مقدار, max(1, param(ID::RD_PORTS) * param(ID::عرض_2)));
				param_بیتها(ID::RD_INIT_مقدار, max(1, param(ID::RD_PORTS) * param(ID::عرض_2)));
				param_بیتها(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_بیتها(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				param_بیتها(ID::WR_WIDE_CONTINUATION, max(1, param(ID::WR_PORTS)));
				param_بیتها(ID::WR_اولویت_ماسک, max(1, param(ID::WR_PORTS) * param(ID::WR_PORTS)));
				پورت(ID::RD_CLK, param(ID::RD_PORTS));
				پورت(ID::RD_EN, param(ID::RD_PORTS));
				پورت(ID::RD_ARST, param(ID::RD_PORTS));
				پورت(ID::RD_SRST, param(ID::RD_PORTS));
				پورت(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				پورت(ID::RD_داده, param(ID::RD_PORTS) * param(ID::عرض_2));
				پورت(ID::WR_CLK, param(ID::WR_PORTS));
				پورت(ID::WR_EN, param(ID::WR_PORTS) * param(ID::عرض_2));
				پورت(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				پورت(ID::WR_داده, param(ID::WR_PORTS) * param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($tribuf)) {
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::Y, param(ID::عرض_2));
				پورت(ID::EN, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bweqx)) {
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::B, param(ID::عرض_2));
				پورت(ID::Y, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bwmux)) {
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::B, param(ID::عرض_2));
				پورت(ID::S, param(ID::عرض_2));
				پورت(ID::Y, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.درون(ID($أكد), ID($assume), ID($زنده), ID($fair), ID($cover))) {
				پورت(ID::A, 1);
				پورت(ID::EN, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($initstate)) {
				پورت(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type.درون(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				پورت(ID::Y, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type.درون(ID($anyinit))) {
				پورت(ID::D, param(ID::عرض_2));
				پورت(ID::Q, param(ID::عرض_2));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($equiv)) {
				پورت(ID::A, 1);
				پورت(ID::B, 1);
				پورت(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type.درون(ID($specify2), ID($specify3))) {
				param_bool(ID::پر);
				param_bool(ID::SRC_DST_PEN);
				param_bool(ID::SRC_DST_POL);
				param(ID::T_RISE_MIN);
				param(ID::T_RISE_TYP);
				param(ID::T_RISE_MAX);
				param(ID::T_FALL_MIN);
				param(ID::T_FALL_TYP);
				param(ID::T_FALL_MAX);
				پورت(ID::EN, 1);
				پورت(ID::SRC, param(ID::SRC_عرض));
				پورت(ID::DST, param(ID::DST_عرض));
				إذا (cell->type == ID($specify3)) {
					param_bool(ID::یال_EN);
					param_bool(ID::یال_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					پورت(ID::DAT, param(ID::DST_عرض));
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
				پورت(ID::SRC_EN, 1);
				پورت(ID::DST_EN, 1);
				پورت(ID::SRC, param(ID::SRC_عرض));
				پورت(ID::DST, param(ID::DST_عرض));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::اولویت);
				پورت(ID::EN, 1);
				پورت(ID::TRG, param(ID::TRG_عرض));
				پورت(ID::آرگومانها, param(ID::آرگومانها_عرض));
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
				param(ID::اولویت);
				پورت(ID::A, 1);
				پورت(ID::EN, 1);
				پورت(ID::TRG, param(ID::TRG_عرض));
				پورت(ID::آرگومانها, param(ID::آرگومانها_عرض));
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

			إذا (cell->type == ID($_BUF_))    { پورت(ID::A,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NOT_))    { پورت(ID::A,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AND_))    { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NAND_))   { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OR_))     { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NOR_))    { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_XOR_))    { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_XNOR_))   { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_ANDNOT_)) { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_ORNOT_))  { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX_))    { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::S,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NMUX_))   { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::S,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AOI3_))   { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::C,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OAI3_))   { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::C,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AOI4_))   { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::C,1); پورت(ID::D,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OAI4_))   { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::C,1); پورت(ID::D,1); پورت(ID::Y,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_TBUF_))  { پورت(ID::A,1); پورت(ID::Y,1); پورت(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_MUX4_))  { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::C,1); پورت(ID::D,1); پورت(ID::S,1); پورت(ID::T,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX8_))  { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::C,1); پورت(ID::D,1); پورت(ID::E,1); پورت(ID::F,1); پورت(ID::G,1); پورت(ID::H,1); پورت(ID::S,1); پورت(ID::T,1); پورت(ID::U,1); پورت(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX16_)) { پورت(ID::A,1); پورت(ID::B,1); پورت(ID::C,1); پورت(ID::D,1); پورت(ID::E,1); پورت(ID::F,1); پورت(ID::G,1); پورت(ID::H,1); پورت(ID::I,1); پورت(ID::J,1); پورت(ID::K,1); پورت(ID::L,1); پورت(ID::M,1); پورت(ID::N,1); پورت(ID::O,1); پورت(ID::P,1); پورت(ID::S,1); پورت(ID::T,1); پورت(ID::U,1); پورت(ID::V,1); پورت(ID::Y,1); check_expected(); أرجع; }

			إذا (cell->type.درون(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ پورت(ID::S,1); پورت(ID::R,1); پورت(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_FF_)) { پورت(ID::D,1); پورت(ID::Q,1); check_expected();  أرجع; }

			إذا (cell->type.درون(ID($_DFF_N_), ID($_DFF_P_)))
				{ پورت(ID::D,1); پورت(ID::Q,1); پورت(ID::C,1); check_expected(); أرجع; }

			إذا (cell->type.درون(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ پورت(ID::D,1); پورت(ID::Q,1); پورت(ID::C,1); پورت(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.درون(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ پورت(ID::D,1); پورت(ID::Q,1); پورت(ID::C,1); پورت(ID::R,1); check_expected(); أرجع; }

			إذا (cell->type.درون(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ پورت(ID::D,1); پورت(ID::Q,1); پورت(ID::C,1); پورت(ID::R,1); پورت(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.درون(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ پورت(ID::D,1); پورت(ID::Q,1); پورت(ID::C,1); پورت(ID::L,1); پورت(ID::AD,1); check_expected(); أرجع; }

			إذا (cell->type.درون(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ پورت(ID::D,1); پورت(ID::Q,1); پورت(ID::C,1); پورت(ID::L,1); پورت(ID::AD,1); پورت(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.درون(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ پورت(ID::C,1); پورت(ID::S,1); پورت(ID::R,1); پورت(ID::D,1); پورت(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.درون(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ پورت(ID::C,1); پورت(ID::S,1); پورت(ID::R,1); پورت(ID::D,1); پورت(ID::E,1); پورت(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.درون(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ پورت(ID::D,1); پورت(ID::Q,1); پورت(ID::C,1); پورت(ID::R,1); check_expected(); أرجع; }

			إذا (cell->type.درون(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ پورت(ID::D,1); پورت(ID::Q,1); پورت(ID::C,1); پورت(ID::R,1); پورت(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.درون(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ پورت(ID::E,1); پورت(ID::D,1); پورت(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.درون(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ پورت(ID::E,1); پورت(ID::R,1); پورت(ID::D,1); پورت(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.درون(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ پورت(ID::E,1); پورت(ID::S,1); پورت(ID::R,1); پورت(ID::D,1); پورت(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.درون(ID($مجموعه_برچسب))) {
				param(ID::عرض_2);
				param(ID::برچسب_2);
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::مجموعه_2, param(ID::عرض_2));
				پورت(ID::CLR, param(ID::عرض_2));
				پورت(ID::Y, param(ID::عرض_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.درون(ID($get_برچسب),ID($original_برچسب))) {
				param(ID::عرض_2);
				param(ID::برچسب_2);
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::Y, param(ID::عرض_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.درون(ID($overwrite_برچسب))) {
				param(ID::عرض_2);
				param(ID::برچسب_2);
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::مجموعه_2, param(ID::عرض_2));
				پورت(ID::CLR, param(ID::عرض_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.درون(ID($future_ff))) {
				param(ID::عرض_2);
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::Y, param(ID::عرض_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.درون(ID($درون_داد_پورت))) {
				param(ID::عرض_2);
				پورت(ID::Y, param(ID::عرض_2));
				check_expected();
				أرجع;
			}
			إذا (cell->type.درون(ID($connect))) {
				param(ID::عرض_2);
				پورت(ID::A, param(ID::عرض_2));
				پورت(ID::B, param(ID::عرض_2));
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
#پایان_اگر

فراغ RTLIL::Module::مرتب_سازی()
{
	wires_.مرتب_سازی(مرتب_سازی_by_id_str(design->twines()));
	cells_.مرتب_سازی(مرتب_سازی_by_id_str(design->twines()));
	parameter_default_مقادیر.مرتب_سازی(مرتب_سازی_by_id_str(design->twines()));
	memories.مرتب_سازی(مرتب_سازی_by_id_str(design->twines()));
	processes.مرتب_سازی(مرتب_سازی_by_id_str(design->twines()));
	لكل (خودکار &it : cells_)
		it.دومی->مرتب_سازی();
	لكل (خودکار &it : wires_)
		it.دومی->صفات.مرتب_سازی(مرتب_سازی_by_id_str(design->twines()));
	لكل (خودکار &it : memories)
		it.دومی->صفات.مرتب_سازی(مرتب_سازی_by_id_str(design->twines()));
}

#اگر_تعریف_نشده NDEBUG
ساكن فراغ check_id_کلیدها(ثابت dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	لكل (خودکار &it : keyed)
		log_assert(!it.اولی.خالی());
}
#پایان_اگر

فراغ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#اگر_تعریف_نشده NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_اندازه(0, module->cells_اندازه(), 1000));
	ثابت RTLIL::Module *const_module = module;

	pool<std::string> حافظه_رشتهها;
	لكل (خودکار &it : module->memories) {
		log_assert(it.اولی == it.دومی->name);
		log_assert(it.اولی != IdString::Null);
		log_assert(it.دومی->عرض >= 0);
		log_assert(it.دومی->اندازه >= 0);
		check_id_کلیدها(it.دومی->صفات);
		حافظه_رشتهها.درج(it.دومی->name.str());
	}

	std::وکتور<MonotonicFlag> ports_declared(Get_اندازه(module->ports));
	Sharded_وکتور<std::string> memids(subpool);
	subpool.پیاپی([const_module, &ports_declared, &حافظه_رشتهها, &memids](ثابت ParallelDispatchThreadPool::پیاپی_Ctx &ctx) {
		لكل (صحيح i : ctx.قلم_range(const_module->cells_اندازه())) {
			خودکار it = *const_module->cells_.element(i);
			log_assert(const_module == it.دومی->module);
			log_assert(it.اولی == it.دومی->name);
			log_assert(it.اولی != IdString::Null);
			log_assert(!it.دومی->type.خالی());
			لكل (خودکار &it2 : it.دومی->connections()) {
				log_assert(it2.اولی != IdString::Null);
				it2.دومی.check(const_module);
			}
			check_id_کلیدها(it.دومی->صفات);
			check_id_کلیدها(it.دومی->parameters);
			داخلی_Cell_Checker checker(const_module, it.دومی);
			checker.check();
			إذا (it.دومی->has_memid()) {
				log_assert(حافظه_رشتهها.شمار(it.دومی->parameters.at(ID::MEMID).decode_string()));
			} وإلا إذا (it.دومی->is_mem_cell()) {
				std::string memid = it.دومی->parameters.at(ID::MEMID).decode_string();
				log_assert(!حافظه_رشتهها.شمار(memid));
				memids.درج(ctx, std::move(memid));
			}
			خودکار cell_mod = const_module->design->module(it.دومی->name);
			إذا (cell_mod != اشاره_تهی) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#اگر_تعریف_شده __GNUC__
				#رهنمود GCC diagnostic افزودن
				#رهنمود GCC diagnostic ignored "-Wdeprecated-declarations"
				#پایان_اگر
				log_assert(!it.دومی->get_blackbox_attribute());
				#اگر_تعریف_شده __GNUC__
				#رهنمود GCC diagnostic برداشت
				#پایان_اگر
			}
		}

		لكل (صحيح i : ctx.قلم_range(const_module->wires_اندازه())) {
			خودکار it = *const_module->wires_.element(i);
			log_assert(const_module == it.دومی->module);
			log_assert(it.اولی == it.دومی->name);
			log_assert(it.اولی != IdString::Null);
			log_assert(it.دومی->عرض >= 0);
			log_assert(it.دومی->پورت_id >= 0);
			check_id_کلیدها(it.دومی->صفات);
			إذا (it.دومی->پورت_id) {
				log_assert(Get_اندازه(const_module->ports) >= it.دومی->پورت_id);
				log_assert(const_module->ports.at(it.دومی->پورت_id-1) == it.دومی->name);
				log_assert(it.دومی->پورت_درون_داد || it.دومی->پورت_output);
				log_assert(it.دومی->پورت_id <= Get_اندازه(ports_declared));
				منطقي previously_declared = ports_declared[it.دومی->پورت_id-1].مجموعه_and_return_old();
				log_assert(previously_declared == خطأ);
			} وإلا
				log_assert(!it.دومی->پورت_درون_داد && !it.دومی->پورت_output);
		}
	});
	لكل (ثابت MonotonicFlag &پورت_declared : ports_declared)
		log_assert(پورت_declared.بارگیری() == درست);
	pool<std::string> memids_pool;
	لكل (std::string &memid : memids)
		log_assert(memids_pool.درج(memid).دومی);

	لكل (خودکار &it : module->processes) {
		log_assert(it.اولی == it.دومی->name);
		log_assert(it.اولی != IdString::Null);
		log_assert(it.دومی->ریشه_case.compare.خالی());
		std::وکتور<RTLIL::Case_قاعده*> all_cases = {&it.دومی->ریشه_case};
		لكل (اندازه_t i = 0; i < all_cases.اندازه(); i++) {
			لكل (خودکار &switch_it : all_cases[i]->switches) {
				لكل (خودکار &case_it : switch_it->cases) {
					لكل (خودکار &compare_it : case_it->compare) {
						log_assert(switch_it->إشارة.اندازه() == compare_it.اندازه());
					}
					all_cases.افزودن_پشت(case_it);
				}
			}
		}
		لكل (خودکار &sync_it : it.دومی->syncs) {
			بدل (sync_it->type) {
				حالة RTLIL::SyncType::ST0:
				حالة RTLIL::SyncType::ST1:
				حالة RTLIL::SyncType::STp:
				حالة RTLIL::SyncType::STn:
				حالة RTLIL::SyncType::STe:
					log_assert(!sync_it->إشارة.خالی());
					اكسر;
				حالة RTLIL::SyncType::STa:
				حالة RTLIL::SyncType::STg:
				حالة RTLIL::SyncType::STi:
					log_assert(sync_it->إشارة.خالی());
					اكسر;
			}
		}
	}

	لكل (خودکار &it : module->connections_) {
		log_assert(it.اولی.اندازه() == it.دومی.اندازه());
		log_assert(!it.اولی.has_const());
		it.اولی.check(module);
		it.دومی.check(module);
	}

	check_id_کلیدها(module->صفات);
#پایان_اگر
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
	new_mod->parameter_default_مقادیر.clear();
	لكل (خودکار &it : parameter_default_مقادیر)
		new_mod->parameter_default_مقادیر[dst_twines.copy_from(design->twines(), it.اولی)] = it.دومی;

	لكل (خودکار &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->صفات, صفات, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	لكل (خودکار it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->اولی);
		wire_map[it->دومی] = new_mod->جمع_کن_Wire(dst_id, it->دومی);
	}

	لكل (خودکار it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->اولی);
		new_mod->جمع_کن_حافظه(dst_id, it->دومی);
	}

	لكل (خودکار it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->اولی);
		new_mod->جمع_کن_Cell(dst_id, it->دومی);
	}

	لكل (خودکار it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->اولی);
		new_mod->جمع_کن_Process(dst_id, it->دومی);
	}

	بنية Rewrite_Sig_Spec_کارگر
	{
		ثابت dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ثابت RTLIL::Module *new_mod;
		فراغ عملگر()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([هذا](RTLIL::Wire *&wire) {
				خودکار it = wire_map.پیدا_کن(wire);
				إذا (it != wire_map.انتها())
					wire = it->دومی;
				وإلا
					log_assert(wire->module == new_mod);
			});
		}
	};

	Rewrite_Sig_Spec_کارگر rewrite_Sig_Spec_کارگر{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewrite_Sig_Spec_کارگر);
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
	dst->جمع_کن(new_mod);
	أرجع new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString هدف_name) ثابت
{
	RTLIL::Module *new_mod = جديد RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = هدف_name;
	cloneInto(new_mod);
	dst->جمع_کن(new_mod);
	أرجع new_mod;
}

منطقي RTLIL::Module::has_memories() ثابت
{
	أرجع !memories.خالی();
}

منطقي RTLIL::Module::has_processes() ثابت
{
	أرجع !processes.خالی();
}

منطقي RTLIL::Module::has_memories_warn() ثابت
{
	إذا (!memories.خالی())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", هذا);
	أرجع !memories.خالی();
}

منطقي RTLIL::Module::has_processes_warn() ثابت
{
	إذا (!processes.خالی())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", هذا);
	أرجع !processes.خالی();
}

منطقي RTLIL::Module::is_selected() ثابت
{
	أرجع design->selected_module(هذا->name);
}

منطقي RTLIL::Module::is_selected_کامل() ثابت
{
	أرجع design->selected_کامل_module(هذا->name);
}

std::وکتور<RTLIL::Wire*> RTLIL::Module::selected_wires() ثابت
{
	std::وکتور<RTLIL::Wire*> نتیجه;
	نتیجه.reserve(wires_.اندازه());
	لكل (خودکار &it : wires_)
		إذا (design->selected(هذا, it.دومی))
			نتیجه.افزودن_پشت(it.دومی);
	أرجع نتیجه;
}

std::وکتور<RTLIL::Cell*> RTLIL::Module::selected_cells() ثابت
{
	std::وکتور<RTLIL::Cell*> نتیجه;
	نتیجه.reserve(cells_.اندازه());
	لكل (خودکار &it : cells_)
		إذا (design->selected(هذا, it.دومی))
			نتیجه.افزودن_پشت(it.دومی);
	أرجع نتیجه;
}

std::وکتور<RTLIL::حافظه*> RTLIL::Module::selected_memories() ثابت
{
	std::وکتور<RTLIL::حافظه*> نتیجه;
	نتیجه.reserve(memories.اندازه());
	لكل (خودکار &it : memories)
		إذا (design->selected(هذا, it.دومی))
			نتیجه.افزودن_پشت(it.دومی);
	أرجع نتیجه;
}

std::وکتور<RTLIL::Process*> RTLIL::Module::selected_processes() ثابت
{
	std::وکتور<RTLIL::Process*> نتیجه;
	نتیجه.reserve(processes.اندازه());
	لكل (خودکار &it : processes)
		إذا (design->selected(هذا, it.دومی))
			نتیجه.افزودن_پشت(it.دومی);
	أرجع نتیجه;
}

std::وکتور<RTLIL::SelectedMember> RTLIL::Module::selected_members() ثابت
{
	std::وکتور<RTLIL::SelectedMember> نتیجه;
	لكل (خودکار cell : selected_cells())
		نتیجه.افزودن_پشت({cell, cell->name});
	لكل (خودکار حافظه_3 : selected_memories())
		نتیجه.افزودن_پشت({حافظه_3, حافظه_3->name});
	لكل (خودکار wire : selected_wires())
		نتیجه.افزودن_پشت({wire, wire->name});
	لكل (خودکار process : selected_processes())
		نتیجه.افزودن_پشت({process, process->name});
	أرجع نتیجه;
}

فراغ RTLIL::Module::جمع_کن(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(شمار_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = هذا;
}

فراغ RTLIL::Module::جمع_کن(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(شمار_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = هذا;
}

فراغ RTLIL::Module::جمع_کن(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(شمار_id(process->name) == 0);
	processes[process->name] = process;
	process->module = هذا;
}

فراغ RTLIL::Module::remove(ثابت pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	إذا (wires.خالی())
		أرجع;

	بنية Delete_Wire_کارگر
	{
		RTLIL::Module *module;
		ثابت pool<RTLIL::Wire*> *wires_p;

		فراغ عملگر()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([هذا](RTLIL::Wire *&wire) {
				إذا (wires_p->شمار(wire))
					wire = module->جمع_کن_Wire(stringf("$delete_wire$%d", (صحيح)autoidx++), wire->عرض);
			});
		}

		فراغ عملگر()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	Delete_Wire_کارگر delete_wire_کارگر;
	delete_wire_کارگر.module = هذا;
	delete_wire_کارگر.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_کارگر);

	إذا (design->flagBufferedNormalized) {
		لكل (خودکار wire : wires) {
			buf_norm_wire_صف.erase(wire);
			buf_norm_connect_شاخص.erase(wire);
		}
	}

	لكل (خودکار &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.شمار(id) != 0);
		wires_.erase(id);
		حذف it;
	}
}

فراغ RTLIL::Module::remove(RTLIL::Cell *cell)
{
	طالما (!cell->connections_.خالی())
		cell->unset_پورت(cell->connections_.begin()->اولی);

	log_assert(cells_.شمار(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	إذا (design && design->flagBufferedNormalized && buf_norm_cell_صف.شمار(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.درج(cell);
	} وإلا {
		حذف cell;
	}
}

فراغ RTLIL::Module::remove(RTLIL::حافظه *حافظه_3)
{
	log_assert(memories.شمار(حافظه_3->name) != 0);
	memories.erase(حافظه_3->name);
	حذف حافظه_3;
}

فراغ RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.شمار(process->name) != 0);
	processes.erase(process->name);
	حذف process;
}

فراغ RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	جمع_کن(wire);
}

فراغ RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	جمع_کن(cell);
}

فراغ RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	إذا (old_id != IdString::Null && wires_.شمار(old_id))
		rename(wires_.at(old_id), new_name);
	وإلا إذا (old_id != IdString::Null && cells_.شمار(old_id))
		rename(cells_.at(old_id), new_name);
	وإلا
		log_abort();
}

فراغ RTLIL::Module::جابجا_نامها(RTLIL::Wire *w1, RTLIL::Wire *w2)
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
	جابجا(w1->name, w2->name);
}

فراغ RTLIL::Module::جابجا_نامها(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	جابجا(c1->name, c2->name);
}

RTLIL::IdString RTLIL::Module::uniquify(RTLIL::IdString name)
{
	صحيح شاخص = 0;
	أرجع uniquify(name, شاخص);
}

IdString RTLIL::Module::uniquify(IdString name, صحيح &شاخص)
{
	إذا (شاخص == 0) {
		إذا (شمار_id(name) == 0)
			أرجع name;
		شاخص++;
	}

	طالما (1) {
		IdString new_name = design->twines().جمع_کن(TwineSpec::Suffix{name, stringf("_%d", شاخص)});
		إذا (شمار_id(new_name) == 0)
			أرجع new_name;
		شاخص++;
	}
}

ساكن منطقي fixup_ports_compare(ثابت RTLIL::Wire *a, ثابت RTLIL::Wire *b)
{
	إذا (a->پورت_id && !b->پورت_id)
		أرجع درست;
	إذا (!a->پورت_id && b->پورت_id)
		أرجع خطأ;

	إذا (a->پورت_id == b->پورت_id)
		أرجع a->name < b->name;
	أرجع a->پورت_id < b->پورت_id;
}

فراغ RTLIL::Module::connect(ثابت RTLIL::SigSig &conn)
{
	لكل (خودکار mon : monitors)
		mon->notify_connect(هذا, conn);

	إذا (design)
		لكل (خودکار mon : design->monitors)
			mon->notify_connect(هذا, conn);

	// ignore all attempts to assign constants to other constants
	إذا (conn.اولی.has_const()) {
		RTLIL::SigSig new_conn;
		لكل (صحيح i = 0; i < Get_اندازه(conn.اولی); i++)
			إذا (conn.اولی[i].wire) {
				new_conn.اولی.append(conn.اولی[i]);
				new_conn.دومی.append(conn.دومی[i]);
			}
		إذا (Get_اندازه(new_conn.اولی))
			connect(new_conn);
		أرجع;
	}

	إذا (yosys_xtrace) {
		لگاریتم("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", هذا, log_signal(conn.اولی), log_signal(conn.دومی), Get_اندازه(conn.اولی));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(Get_اندازه(conn.اولی) == Get_اندازه(conn.دومی));
	connections_.افزودن_پشت(conn);
}

فراغ RTLIL::Module::connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

فراغ RTLIL::Module::new_connections(ثابت std::وکتور<RTLIL::SigSig> &new_conn)
{
	لكل (خودکار mon : monitors)
		mon->notify_connect(هذا, new_conn);

	إذا (design)
		لكل (خودکار mon : design->monitors)
			mon->notify_connect(هذا, new_conn);

	إذا (yosys_xtrace) {
		لگاریتم("#X# New connections vector in %s:\n", هذا);
		لكل (خودکار &conn: new_conn)
			لگاریتم("#X#    %s = %s (%d bits)\n", log_signal(conn.اولی), log_signal(conn.دومی), Get_اندازه(conn.اولی));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ثابت std::وکتور<RTLIL::SigSig> &RTLIL::Module::connections() ثابت
{
	أرجع connections_;
}

فراغ RTLIL::Module::fixup_ports()
{
	std::وکتور<RTLIL::Wire*> all_ports;

	لكل (خودکار &w : wires_)
		إذا (w.دومی->پورت_درون_داد || w.دومی->پورت_output)
			all_ports.افزودن_پشت(w.دومی);
		وإلا
			w.دومی->پورت_id = 0;

	std::مرتب_سازی(all_ports.begin(), all_ports.انتها(), fixup_ports_compare);

	إذا (design && design->flagBufferedNormalized) {
		لكل (خودکار &w : wires_)
			إذا (w.دومی->driverCell_ && w.دومی->driverCell_->type == ID($درون_داد_پورت))
				buf_norm_wire_صف.درج(w.دومی);

		buf_norm_wire_صف.درج(all_ports.begin(), all_ports.انتها());
	}

	ports.clear();
	لكل (اندازه_t i = 0; i < all_ports.اندازه(); i++) {
		ports.افزودن_پشت(all_ports[i]->name);
		all_ports[i]->پورت_id = i+1;
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
		خودکار &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.اولی)] = it.دومی;
	}
}

RTLIL::Wire *RTLIL::Module::جمع_کن_Wire(RTLIL::IdString name, صحيح عرض)
{
	log_assert(design);
	log_assert(عرض >= 0 && عرض < RTLIL::عرض_حد);
	RTLIL::Wire *wire = جديد RTLIL::Wire;
	wire->عرض = عرض;
	wire->name = name;
	جمع_کن(wire);
	أرجع wire;
}

RTLIL::Wire *RTLIL::Module::جمع_کن_Wire(IdString name, ثابت RTLIL::Wire *other)
{
	RTLIL::Wire *wire = جمع_کن_Wire(name);
	wire->عرض = other->عرض;
	wire->شروع_جابه_جایی = other->شروع_جابه_جایی;
	wire->پورت_id = other->پورت_id;
	wire->پورت_درون_داد = other->پورت_درون_داد;
	wire->پورت_output = other->پورت_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ثابت RTLIL::Design *src_design = other->module ? other->module->design : اشاره_تهی;
		RTLIL::copy_attr_dict(wire->صفات, other->صفات, src_design, هذا->design);
	}
	أرجع wire;
}

RTLIL::Cell *RTLIL::Module::جمع_کن_Cell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = جديد RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	جمع_کن(cell);
	أرجع cell;
}

RTLIL::Cell *RTLIL::Module::جمع_کن_Cell(IdString name, ثابت RTLIL::Cell *other)
{
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : اشاره_تهی;
	منطقي cross_pool = src_design && هذا->design && src_design != هذا->design;

	IdString type = other->type.ref();
	إذا (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = جمع_کن_Cell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, هذا->design);
	RTLIL::copy_attr_dict(cell->صفات, other->صفات, src_design, هذا->design);

	إذا (!cross_pool)
		cell->connections_ = other->connections_;
	وإلا
		لكل (صحيح i = Get_اندازه(other->connections_) - 1; i >= 0; i--) {
			خودکار &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.اولی)] = c.دومی;
		}
	أرجع cell;
}

RTLIL::حافظه *RTLIL::Module::جمع_کن_حافظه(IdString name)
{
	log_assert(design);
	RTLIL::حافظه *mem = جديد RTLIL::حافظه;
	mem->module = هذا;
	mem->name = name;
	memories[name] = mem;
	أرجع mem;
}

RTLIL::حافظه *RTLIL::Module::جمع_کن_حافظه(IdString name, ثابت RTLIL::حافظه *other)
{
	log_assert(design);
	RTLIL::حافظه *mem = جديد RTLIL::حافظه;
	mem->module = هذا;
	mem->name = name;
	mem->عرض = other->عرض;
	mem->شروع_جابه_جایی = other->شروع_جابه_جایی;
	mem->اندازه = other->اندازه;
	RTLIL::copy_attr_dict(mem->صفات, other->صفات,
			other->module ? other->module->design : اشاره_تهی, design);
	memories[name] = mem;
	أرجع mem;
}

RTLIL::Process *RTLIL::Module::جمع_کن_Process(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = جديد RTLIL::Process;
	proc->name = name;
	جمع_کن(proc);
	أرجع proc;
}

RTLIL::Process *RTLIL::Module::جمع_کن_Process(RTLIL::IdString name, ثابت RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : اشاره_تهی;
	إذا (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> صفات;
			RTLIL::copy_attr_dict(صفات, obj->صفات, src_design, design);
			obj->صفات = std::move(صفات);
		});
		لكل (خودکار *متزامن : proc->syncs)
			لكل (خودکار &mwa : متزامن->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	جمع_کن(proc);
	أرجع proc;
}

#تعریف_کن DEF_روش(_func, y_اندازه, _type) \
	RTLIL::Cell* RTLIL::Module::جمع_کن ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کن_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_عرض] = sig_a.اندازه();       \
		cell->parameters[ID::Y_عرض] = sig_y.اندازه();       \
		cell->مجموعه_پورت(ID::A, sig_a);                        \
		cell->مجموعه_پورت(ID::Y, sig_y);                        \
		cell->مجموعه_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کن_Wire(NEW_ID, y_اندازه);    \
		جمع_کن ## _func(name, sig_a, sig_y, is_signed, src);   \
		أرجع sig_y;                                       \
	}
DEF_روش(Not,        sig_a.اندازه(), ID($not))
DEF_روش(Pos,        sig_a.اندازه(), ID($pos))
DEF_روش(Neg,        sig_a.اندازه(), ID($neg))
DEF_روش(ReduceAnd,  1, ID($reduce_and))
DEF_روش(ReduceOr,   1, ID($reduce_or))
DEF_روش(ReduceXor,  1, ID($reduce_xor))
DEF_روش(ReduceXnor, 1, ID($reduce_xnor))
DEF_روش(ReduceBool, 1, ID($reduce_bool))
DEF_روش(LogicNot,   1, ID($logic_not))
#لغو_تعریف DEF_روش

#تعریف_کن DEF_روش(_func, y_اندازه, _type) \
	RTLIL::Cell* RTLIL::Module::جمع_کن ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقي /* is_signed */, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کن_Cell(name, _type);           \
		cell->parameters[ID::عرض_2] = sig_a.اندازه();         \
		cell->مجموعه_پورت(ID::A, sig_a);                        \
		cell->مجموعه_پورت(ID::Y, sig_y);                        \
		cell->مجموعه_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کن_Wire(NEW_ID, y_اندازه);    \
		جمع_کن ## _func(name, sig_a, sig_y, is_signed, src);   \
		أرجع sig_y;                                       \
	}
DEF_روش(Buf,     sig_a.اندازه(), ID($buf))
DEF_روش(Barrier, sig_a.اندازه(), ID($barrier))
#لغو_تعریف DEF_روش

#تعریف_کن DEF_روش(_func, y_اندازه, _type) \
	RTLIL::Cell* RTLIL::Module::جمع_کن ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کن_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_عرض] = sig_a.اندازه();       \
		cell->parameters[ID::B_عرض] = sig_b.اندازه();       \
		cell->parameters[ID::Y_عرض] = sig_y.اندازه();       \
		cell->مجموعه_پورت(ID::A, sig_a);                        \
		cell->مجموعه_پورت(ID::B, sig_b);                        \
		cell->مجموعه_پورت(ID::Y, sig_y);                        \
		cell->مجموعه_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کن_Wire(NEW_ID, y_اندازه);         \
		جمع_کن ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_روش(And,      max(sig_a.اندازه(), sig_b.اندازه()), ID($and))
DEF_روش(Or,       max(sig_a.اندازه(), sig_b.اندازه()), ID($or))
DEF_روش(Xor,      max(sig_a.اندازه(), sig_b.اندازه()), ID($xor))
DEF_روش(Xnor,     max(sig_a.اندازه(), sig_b.اندازه()), ID($xnor))
DEF_روش(لغزش_2,    sig_a.اندازه(), ID($لغزش))
DEF_روش(Lt,       1, ID($lt))
DEF_روش(Le,       1, ID($le))
DEF_روش(Eq,       1, ID($eq))
DEF_روش(Ne,       1, ID($ne))
DEF_روش(Eqx,      1, ID($eqx))
DEF_روش(Nex,      1, ID($nex))
DEF_روش(Ge,       1, ID($ge))
DEF_روش(Gt,       1, ID($gt))
DEF_روش(جمع_کن_2,      max(sig_a.اندازه(), sig_b.اندازه()), ID($جمع_کن))
DEF_روش(Sub,      max(sig_a.اندازه(), sig_b.اندازه()), ID($sub))
DEF_روش(Mul,      max(sig_a.اندازه(), sig_b.اندازه()), ID($mul))
DEF_روش(Div,      max(sig_a.اندازه(), sig_b.اندازه()), ID($div))
DEF_روش(Mod,      max(sig_a.اندازه(), sig_b.اندازه()), ID($mod))
DEF_روش(DivFloor, max(sig_a.اندازه(), sig_b.اندازه()), ID($divfloor))
DEF_روش(ModFloor, max(sig_a.اندازه(), sig_b.اندازه()), ID($modfloor))
DEF_روش(LogicAnd, 1, ID($logic_and))
DEF_روش(LogicOr,  1, ID($logic_or))
#لغو_تعریف DEF_روش

#تعریف_کن DEF_روش(_func, y_اندازه, _type) \
	RTLIL::Cell* RTLIL::Module::جمع_کن ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کن_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = خطأ;             \
		cell->parameters[ID::A_عرض] = sig_a.اندازه();       \
		cell->parameters[ID::B_عرض] = sig_b.اندازه();       \
		cell->parameters[ID::Y_عرض] = sig_y.اندازه();       \
		cell->مجموعه_پورت(ID::A, sig_a);                        \
		cell->مجموعه_پورت(ID::B, sig_b);                        \
		cell->مجموعه_پورت(ID::Y, sig_y);                        \
		cell->مجموعه_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کن_Wire(NEW_ID, y_اندازه);         \
		جمع_کن ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_روش(Shl,      sig_a.اندازه(), ID($shl))
DEF_روش(Shr,      sig_a.اندازه(), ID($shr))
DEF_روش(Sshl,     sig_a.اندازه(), ID($sshl))
DEF_روش(Sshr,     sig_a.اندازه(), ID($sshr))
#لغو_تعریف DEF_روش

#تعریف_کن DEF_روش(_func, y_اندازه, _type) \
	RTLIL::Cell* RTLIL::Module::جمع_کن ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کن_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = خطأ;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_عرض] = sig_a.اندازه();       \
		cell->parameters[ID::B_عرض] = sig_b.اندازه();       \
		cell->parameters[ID::Y_عرض] = sig_y.اندازه();       \
		cell->مجموعه_پورت(ID::A, sig_a);                        \
		cell->مجموعه_پورت(ID::B, sig_b);                        \
		cell->مجموعه_پورت(ID::Y, sig_y);                        \
		cell->مجموعه_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کن_Wire(NEW_ID, y_اندازه);         \
		جمع_کن ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_روش(Shiftx,      sig_a.اندازه(), ID($shiftx))
#لغو_تعریف DEF_روش

#تعریف_کن DEF_روش(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::جمع_کن ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کن_Cell(name, _type);                 \
		cell->parameters[ID::عرض_2] = sig_a.اندازه();               \
		إذا (_pmux) cell->parameters[ID::S_عرض] = sig_s.اندازه();  \
		cell->مجموعه_پورت(ID::A, sig_a);                              \
		cell->مجموعه_پورت(ID::B, sig_b);                              \
		cell->مجموعه_پورت(ID::S, sig_s);                              \
		cell->مجموعه_پورت(ID::Y, sig_y);                              \
		cell->مجموعه_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کن_Wire(NEW_ID, sig_a.اندازه());     \
		جمع_کن ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		أرجع sig_y;                                             \
	}
DEF_روش(Mux,      ID($mux),        0)
DEF_روش(Bwmux,    ID($bwmux),      0)
DEF_روش(Pmux,     ID($pmux),       1)
#لغو_تعریف DEF_روش

#تعریف_کن DEF_روش(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::جمع_کن ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کن_Cell(name, _type);                 \
		cell->parameters[ID::عرض_2] = _demux ? sig_a.اندازه() : sig_y.اندازه(); \
		cell->parameters[ID::S_عرض] = sig_s.اندازه();             \
		cell->مجموعه_پورت(ID::A, sig_a);                              \
		cell->مجموعه_پورت(ID::S, sig_s);                              \
		cell->مجموعه_پورت(ID::Y, sig_y);                              \
		cell->مجموعه_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کن_Wire(NEW_ID, _demux ? sig_a.اندازه() << sig_s.اندازه() : sig_a.اندازه() >> sig_s.اندازه()); \
		جمع_کن ## _func(name, sig_a, sig_s, sig_y, src);             \
		أرجع sig_y;                                             \
	}
DEF_روش(Bmux,     ID($bmux),       0)
DEF_روش(Demux,    ID($demux),      1)
#لغو_تعریف DEF_روش

#تعریف_کن DEF_روش(_func, _type) \
	RTLIL::Cell* RTLIL::Module::جمع_کن ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کن_Cell(name, _type);                 \
		cell->parameters[ID::عرض_2] = sig_a.اندازه();               \
		cell->مجموعه_پورت(ID::A, sig_a);                              \
		cell->مجموعه_پورت(ID::B, sig_b);                              \
		cell->مجموعه_پورت(ID::Y, sig_y);                              \
		cell->مجموعه_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = جمع_کن_Wire(NEW_ID, sig_a.اندازه());     \
		جمع_کن ## _func(name, sig_a, sig_s, sig_y, src);             \
		أرجع sig_y;                                             \
	}
DEF_روش(Bweqx,    ID($bweqx))
#لغو_تعریف DEF_روش

#تعریف_کن DEF_روش_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::جمع_کن ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig1, ثابت RTLIL::Sig_بیت &sig2, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کن_Cell(name, _type);         \
		cell->مجموعه_پورت("\\" #_P1, sig1);                   \
		cell->مجموعه_پورت("\\" #_P2, sig2);                   \
		cell->مجموعه_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_بیت RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig1, ثابت std::string &src) { \
		RTLIL::Sig_بیت sig2 = جمع_کن_Wire(NEW_ID);             \
		جمع_کن ## _func(name, sig1, sig2, src);              \
		أرجع sig2;                                      \
	}
#تعریف_کن DEF_روش_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::جمع_کن ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig1, ثابت RTLIL::Sig_بیت &sig2, ثابت RTLIL::Sig_بیت &sig3, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کن_Cell(name, _type);         \
		cell->مجموعه_پورت("\\" #_P1, sig1);                   \
		cell->مجموعه_پورت("\\" #_P2, sig2);                   \
		cell->مجموعه_پورت("\\" #_P3, sig3);                   \
		cell->مجموعه_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_بیت RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig1, ثابت RTLIL::Sig_بیت &sig2, ثابت std::string &src) { \
		RTLIL::Sig_بیت sig3 = جمع_کن_Wire(NEW_ID);             \
		جمع_کن ## _func(name, sig1, sig2, sig3, src);        \
		أرجع sig3;                                      \
	}
#تعریف_کن DEF_روش_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::جمع_کن ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig1, ثابت RTLIL::Sig_بیت &sig2, ثابت RTLIL::Sig_بیت &sig3, ثابت RTLIL::Sig_بیت &sig4, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کن_Cell(name, _type);         \
		cell->مجموعه_پورت("\\" #_P1, sig1);                   \
		cell->مجموعه_پورت("\\" #_P2, sig2);                   \
		cell->مجموعه_پورت("\\" #_P3, sig3);                   \
		cell->مجموعه_پورت("\\" #_P4, sig4);                   \
		cell->مجموعه_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_بیت RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig1, ثابت RTLIL::Sig_بیت &sig2, ثابت RTLIL::Sig_بیت &sig3, ثابت std::string &src) { \
		RTLIL::Sig_بیت sig4 = جمع_کن_Wire(NEW_ID);             \
		جمع_کن ## _func(name, sig1, sig2, sig3, sig4, src);  \
		أرجع sig4;                                      \
	}
#تعریف_کن DEF_روش_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::جمع_کن ## _func(RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig1, ثابت RTLIL::Sig_بیت &sig2, ثابت RTLIL::Sig_بیت &sig3, ثابت RTLIL::Sig_بیت &sig4, ثابت RTLIL::Sig_بیت &sig5, ثابت std::string &src) { \
		RTLIL::Cell *cell = جمع_کن_Cell(name, _type);         \
		cell->مجموعه_پورت("\\" #_P1, sig1);                   \
		cell->مجموعه_پورت("\\" #_P2, sig2);                   \
		cell->مجموعه_پورت("\\" #_P3, sig3);                   \
		cell->مجموعه_پورت("\\" #_P4, sig4);                   \
		cell->مجموعه_پورت("\\" #_P5, sig5);                   \
		cell->مجموعه_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::Sig_بیت RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig1, ثابت RTLIL::Sig_بیت &sig2, ثابت RTLIL::Sig_بیت &sig3, ثابت RTLIL::Sig_بیت &sig4, ثابت std::string &src) { \
		RTLIL::Sig_بیت sig5 = جمع_کن_Wire(NEW_ID);                  \
		جمع_کن ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		أرجع sig5;                                           \
	}
DEF_روش_2(BufGate,    ID($_BUF_),    A, Y)
DEF_روش_2(NotGate,    ID($_NOT_),    A, Y)
DEF_روش_3(AndGate,    ID($_AND_),    A, B, Y)
DEF_روش_3(NandGate,   ID($_NAND_),   A, B, Y)
DEF_روش_3(OrGate,     ID($_OR_),     A, B, Y)
DEF_روش_3(NorGate,    ID($_NOR_),    A, B, Y)
DEF_روش_3(XorGate,    ID($_XOR_),    A, B, Y)
DEF_روش_3(XnorGate,   ID($_XNOR_),   A, B, Y)
DEF_روش_3(AndnotGate, ID($_ANDNOT_), A, B, Y)
DEF_روش_3(OrnotGate,  ID($_ORNOT_),  A, B, Y)
DEF_روش_4(MuxGate,    ID($_MUX_),    A, B, S, Y)
DEF_روش_4(NmuxGate,   ID($_NMUX_),   A, B, S, Y)
DEF_روش_4(Aoi3Gate,   ID($_AOI3_),   A, B, C, Y)
DEF_روش_4(Oai3Gate,   ID($_OAI3_),   A, B, C, Y)
DEF_روش_5(Aoi4Gate,   ID($_AOI4_),   A, B, C, D, Y)
DEF_روش_5(Oai4Gate,   ID($_OAI4_),   A, B, C, D, Y)
#لغو_تعریف DEF_روش_2
#لغو_تعریف DEF_روش_3
#لغو_تعریف DEF_روش_4
#لغو_تعریف DEF_روش_5

RTLIL::Cell* RTLIL::Module::جمع_کن_Pow(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي a_signed, منطقي b_signed, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($توان));
	cell->parameters[ID::A_SIGNED] = a_signed;
	cell->parameters[ID::B_SIGNED] = b_signed;
	cell->parameters[ID::A_عرض] = sig_a.اندازه();
	cell->parameters[ID::B_عرض] = sig_b.اندازه();
	cell->parameters[ID::Y_عرض] = sig_y.اندازه();
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::B, sig_b);
	cell->مجموعه_پورت(ID::Y, sig_y);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Fa(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_x, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($fa));
	cell->parameters[ID::عرض_2] = sig_a.اندازه();
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::B, sig_b);
	cell->مجموعه_پورت(ID::C, sig_c);
	cell->مجموعه_پورت(ID::X, sig_x);
	cell->مجموعه_پورت(ID::Y, sig_y);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_برش(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const جابه_جایی, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($برش));
	cell->parameters[ID::A_عرض] = sig_a.اندازه();
	cell->parameters[ID::Y_عرض] = sig_y.اندازه();
	cell->parameters[ID::جابه_جایی_2] = جابه_جایی;
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::Y, sig_y);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Concat(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($concat));
	cell->parameters[ID::A_عرض] = sig_a.اندازه();
	cell->parameters[ID::B_عرض] = sig_b.اندازه();
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::B, sig_b);
	cell->مجموعه_پورت(ID::Y, sig_y);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Lut(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::عرض_2] = sig_a.اندازه();
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::Y, sig_y);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Tribuf(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($tribuf));
	cell->parameters[ID::عرض_2] = sig_a.اندازه();
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_پورت(ID::Y, sig_y);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Assert(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($أكد));
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Assume(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($assume));
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_زنده(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($زنده));
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Fair(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($fair));
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Cover(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($cover));
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Equiv(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($equiv));
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::B, sig_b);
	cell->مجموعه_پورت(ID::Y, sig_y);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Sr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr, ثابت RTLIL::SigSpec &sig_q, منطقي مجموعه_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($sr));
	cell->parameters[ID::مجموعه_POLARITY] = مجموعه_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::مجموعه_2, sig_مجموعه);
	cell->مجموعه_پورت(ID::CLR, sig_clr);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Ff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($ff));
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Dff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::CLK, sig_clk);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Dffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($dffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::CLK, sig_clk);
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Dffsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي مجموعه_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($dffsr));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::مجموعه_POLARITY] = مجموعه_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::CLK, sig_clk);
	cell->مجموعه_پورت(ID::مجموعه_2, sig_مجموعه);
	cell->مجموعه_پورت(ID::CLR, sig_clr);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Dffsre(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, منطقي مجموعه_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($dffsre));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::مجموعه_POLARITY] = مجموعه_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::CLK, sig_clk);
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_پورت(ID::مجموعه_2, sig_مجموعه);
	cell->مجموعه_پورت(ID::CLR, sig_clr);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Adff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_مقدار, منطقي clk_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_مقدار] = arst_مقدار;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::CLK, sig_clk);
	cell->مجموعه_پورت(ID::ARST, sig_arst);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Adffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_مقدار, منطقي clk_polarity, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_مقدار] = arst_مقدار;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::CLK, sig_clk);
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_پورت(ID::ARST, sig_arst);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Aldff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($aldff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::CLK, sig_clk);
	cell->مجموعه_پورت(ID::ALOAD, sig_aload);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::AD, sig_ad);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Aldffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي en_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($aldffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::CLK, sig_clk);
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_پورت(ID::ALOAD, sig_aload);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::AD, sig_ad);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Sdff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_مقدار, منطقي clk_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_مقدار] = srst_مقدار;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::CLK, sig_clk);
	cell->مجموعه_پورت(ID::SRST, sig_srst);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Sdffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_مقدار, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_مقدار] = srst_مقدار;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::CLK, sig_clk);
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_پورت(ID::SRST, sig_srst);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Sdffce(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_مقدار, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_مقدار] = srst_مقدار;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::CLK, sig_clk);
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_پورت(ID::SRST, sig_srst);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Dlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Adlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_مقدار, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_مقدار] = arst_مقدار;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_پورت(ID::ARST, sig_arst);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Dlatchsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, منطقي مجموعه_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($dlatchsr));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::مجموعه_POLARITY] = مجموعه_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::EN, sig_en);
	cell->مجموعه_پورت(ID::مجموعه_2, sig_مجموعه);
	cell->مجموعه_پورت(ID::CLR, sig_clr);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Sr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr,
		ثابت RTLIL::SigSpec &sig_q, منطقي مجموعه_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_SR_%c%c_", مجموعه_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->مجموعه_پورت(ID::S, sig_مجموعه);
	cell->مجموعه_پورت(ID::R, sig_clr);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Ff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($_FF_));
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Dff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->مجموعه_پورت(ID::C, sig_clk);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Dffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->مجموعه_پورت(ID::C, sig_clk);
	cell->مجموعه_پورت(ID::E, sig_en);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Dffsr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي مجموعه_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', مجموعه_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->مجموعه_پورت(ID::C, sig_clk);
	cell->مجموعه_پورت(ID::S, sig_مجموعه);
	cell->مجموعه_پورت(ID::R, sig_clr);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Dffsre_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, منطقي مجموعه_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', مجموعه_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->مجموعه_پورت(ID::C, sig_clk);
	cell->مجموعه_پورت(ID::S, sig_مجموعه);
	cell->مجموعه_پورت(ID::R, sig_clr);
	cell->مجموعه_پورت(ID::E, sig_en);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Adff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_مقدار, منطقي clk_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_مقدار ? '1' : '0'));
	cell->مجموعه_پورت(ID::C, sig_clk);
	cell->مجموعه_پورت(ID::R, sig_arst);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Adffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_مقدار, منطقي clk_polarity, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_مقدار ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->مجموعه_پورت(ID::C, sig_clk);
	cell->مجموعه_پورت(ID::R, sig_arst);
	cell->مجموعه_پورت(ID::E, sig_en);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Aldff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->مجموعه_پورت(ID::C, sig_clk);
	cell->مجموعه_پورت(ID::L, sig_aload);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::AD, sig_ad);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Aldffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي en_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->مجموعه_پورت(ID::C, sig_clk);
	cell->مجموعه_پورت(ID::L, sig_aload);
	cell->مجموعه_پورت(ID::E, sig_en);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::AD, sig_ad);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Sdff_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_مقدار, منطقي clk_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_مقدار ? '1' : '0'));
	cell->مجموعه_پورت(ID::C, sig_clk);
	cell->مجموعه_پورت(ID::R, sig_srst);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Sdffe_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_مقدار, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_مقدار ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->مجموعه_پورت(ID::C, sig_clk);
	cell->مجموعه_پورت(ID::R, sig_srst);
	cell->مجموعه_پورت(ID::E, sig_en);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Sdffce_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_مقدار, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_مقدار ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->مجموعه_پورت(ID::C, sig_clk);
	cell->مجموعه_پورت(ID::R, sig_srst);
	cell->مجموعه_پورت(ID::E, sig_en);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Dlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->مجموعه_پورت(ID::E, sig_en);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Adlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_مقدار, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_مقدار ? '1' : '0'));
	cell->مجموعه_پورت(ID::E, sig_en);
	cell->مجموعه_پورت(ID::R, sig_arst);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Dlatchsr_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, منطقي مجموعه_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', مجموعه_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->مجموعه_پورت(ID::E, sig_en);
	cell->مجموعه_پورت(ID::S, sig_مجموعه);
	cell->مجموعه_پورت(ID::R, sig_clr);
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Anyinit(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($anyinit));
	cell->parameters[ID::عرض_2] = sig_q.اندازه();
	cell->مجموعه_پورت(ID::D, sig_d);
	cell->مجموعه_پورت(ID::Q, sig_q);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, صحيح عرض, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کن_Wire(NEW_ID, عرض);
	Cell *cell = جمع_کن_Cell(name, ID($anyconst));
	cell->مجموعه_Param(ID::عرض_2, عرض);
	cell->مجموعه_پورت(ID::Y, sig);
	cell->مجموعه_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, صحيح عرض, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کن_Wire(NEW_ID, عرض);
	Cell *cell = جمع_کن_Cell(name, ID($anyseq));
	cell->مجموعه_Param(ID::عرض_2, عرض);
	cell->مجموعه_پورت(ID::Y, sig);
	cell->مجموعه_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, صحيح عرض, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کن_Wire(NEW_ID, عرض);
	Cell *cell = جمع_کن_Cell(name, ID($allconst));
	cell->مجموعه_Param(ID::عرض_2, عرض);
	cell->مجموعه_پورت(ID::Y, sig);
	cell->مجموعه_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, صحيح عرض, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کن_Wire(NEW_ID, عرض);
	Cell *cell = جمع_کن_Cell(name, ID($allseq));
	cell->مجموعه_Param(ID::عرض_2, عرض);
	cell->مجموعه_پورت(ID::Y, sig);
	cell->مجموعه_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کن_Wire(NEW_ID);
	Cell *cell = جمع_کن_Cell(name, ID($initstate));
	cell->مجموعه_پورت(ID::Y, sig);
	cell->مجموعه_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::مجموعه_برچسب_2(RTLIL::IdString name, ثابت std::string &برچسب, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کن_Wire(NEW_ID, sig_a.اندازه());
	Cell *cell = جمع_کن_Cell(name, ID($مجموعه_برچسب));
	cell->parameters[ID::عرض_2] = sig_a.اندازه();
	cell->parameters[ID::برچسب_2] = برچسب;
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::مجموعه_2, sig_s);
	cell->مجموعه_پورت(ID::CLR, sig_c);
	cell->مجموعه_پورت(ID::Y, sig);
	cell->مجموعه_src_attribute(src);
	أرجع sig;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_مجموعه_برچسب(RTLIL::IdString name, ثابت std::string &برچسب, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	Cell *cell = جمع_کن_Cell(name, ID($مجموعه_برچسب));
	cell->parameters[ID::عرض_2] = sig_a.اندازه();
	cell->parameters[ID::برچسب_2] = برچسب;
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::مجموعه_2, sig_s);
	cell->مجموعه_پورت(ID::CLR, sig_c);
	cell->مجموعه_پورت(ID::Y, sig_y);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::Get_برچسب(RTLIL::IdString name, ثابت std::string &برچسب, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کن_Wire(NEW_ID, sig_a.اندازه());
	Cell *cell = جمع_کن_Cell(name, ID($get_برچسب));
	cell->parameters[ID::عرض_2] = sig_a.اندازه();
	cell->parameters[ID::برچسب_2] = برچسب;
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::Y, sig);
	cell->مجموعه_src_attribute(src);
	أرجع sig;
}

RTLIL::Cell* RTLIL::Module::جمع_کن_Overwrite_برچسب(RTLIL::IdString name, ثابت std::string &برچسب, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src)
{
	RTLIL::Cell *cell = جمع_کن_Cell(name, ID($overwrite_برچسب));
	cell->parameters[ID::عرض_2] = sig_a.اندازه();
	cell->parameters[ID::برچسب_2] = برچسب;
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::مجموعه_2, sig_s);
	cell->مجموعه_پورت(ID::CLR, sig_c);
	cell->مجموعه_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::Original_برچسب(RTLIL::IdString name, ثابت std::string &برچسب, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کن_Wire(NEW_ID, sig_a.اندازه());
	Cell *cell = جمع_کن_Cell(name, ID($original_برچسب));
	cell->parameters[ID::عرض_2] = sig_a.اندازه();
	cell->parameters[ID::برچسب_2] = برچسب;
	cell->مجموعه_پورت(ID::A, sig_a);
	cell->مجموعه_پورت(ID::Y, sig);
	cell->مجموعه_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src)
{
	RTLIL::SigSpec sig = جمع_کن_Wire(NEW_ID, sig_e.اندازه());
	Cell *cell = جمع_کن_Cell(name, ID($future_ff));
	cell->parameters[ID::عرض_2] = sig_e.اندازه();
	cell->مجموعه_پورت(ID::A, sig_e);
	cell->مجموعه_پورت(ID::Y, sig);
	cell->مجموعه_src_attribute(src);
	أرجع sig;
}

std::string RTLIL::Module::مقصد_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(هذا), design, خطأ, درست, خطأ, RTLIL_BACKEND::Dump_نما::Readable);
	أرجع f.str();
}

RTLIL::Wire::Wire()
{
	ساكن بیعلامت صحيح hashidx_شمار = 123456789;
	hashidx_شمار = mkhash_xorshift(hashidx_شمار);
	hashidx_ = hashidx_شمار;

	module = اشاره_تهی;
	عرض = 1;
	شروع_جابه_جایی = 0;
	پورت_id = 0;
	پورت_درون_داد = خطأ;
	پورت_output = خطأ;
	upto = خطأ;
	is_signed = خطأ;

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->درج(std::pair<بیعلامت صحيح, RTLIL::Wire*>(hashidx_, هذا));
#پایان_اگر
}

RTLIL::Wire::~Wire()
{
#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#پایان_اگر
}

std::string RTLIL::Wire::مقصد_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", هذا, design(), RTLIL_BACKEND::Dump_نما::Readable);
	أرجع f.str();
}

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
ساكن std::map<بیعلامت صحيح, RTLIL::Wire*> all_wires;
std::map<بیعلامت صحيح, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(فراغ)
{
	أرجع &all_wires;
}
#پایان_اگر

RTLIL::حافظه::حافظه()
{
	ساكن بیعلامت صحيح hashidx_شمار = 123456789;
	hashidx_شمار = mkhash_xorshift(hashidx_شمار);
	hashidx_ = hashidx_شمار;

	عرض = 1;
	شروع_جابه_جایی = 0;
	اندازه = 0;
#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	RTLIL::حافظه::get_all_memorys()->درج(std::pair<بیعلامت صحيح, RTLIL::حافظه*>(hashidx_, هذا));
#پایان_اگر
}

std::string RTLIL::حافظه::مقصد_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_حافظه(f, "", هذا, design(), RTLIL_BACKEND::Dump_نما::Readable);
	أرجع f.str();
}

RTLIL::Process::Process() : module(اشاره_تهی)
{
	ساكن بیعلامت صحيح hashidx_شمار = 123456789;
	hashidx_شمار = mkhash_xorshift(hashidx_شمار);
	hashidx_ = hashidx_شمار;
}

std::string RTLIL::Process::مقصد_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", هذا, design(), RTLIL_BACKEND::Dump_نما::Readable);
	أرجع f.str();
}

RTLIL::Cell::Cell() : module(اشاره_تهی)
{
	ساكن بیعلامت صحيح hashidx_شمار = 123456789;
	hashidx_شمار = mkhash_xorshift(hashidx_شمار);
	hashidx_ = hashidx_شمار;

	// log("#memtrace# %p\n", this);
	memhasher();

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->درج(std::pair<بیعلامت صحيح, RTLIL::Cell*>(hashidx_, هذا));
#پایان_اگر
}

RTLIL::Cell::~Cell()
{
#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#پایان_اگر
}

std::string RTLIL::Cell::مقصد_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", هذا, design(), RTLIL_BACKEND::Dump_نما::Readable);
	أرجع f.str();
}

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
ساكن std::map<بیعلامت صحيح, RTLIL::Cell*> all_cells;
std::map<بیعلامت صحيح, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(فراغ)
{
	أرجع &all_cells;
}
#پایان_اگر

منطقي RTLIL::Cell::has_پورت(RTLIL::IdString portname) ثابت
{
	أرجع connections_.شمار(portname) != 0;
}

// bufnorm

ثابت RTLIL::SigSpec &RTLIL::Cell::get_پورت(RTLIL::IdString portname) ثابت
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
		أرجع درست;
	إذا (module && module->design && module->design->module(type))
		أرجع درست;
	أرجع خطأ;
}

منطقي RTLIL::Cell::درون_داد(IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_معلوم(type))
		أرجع yosys_celltypes.cell_درون_داد(type.ref(), portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : اشاره_تهی;
		أرجع w && w->پورت_درون_داد;
	}
	أرجع خطأ;
}

منطقي RTLIL::Cell::output(RTLIL::IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_معلوم(type))
		أرجع yosys_celltypes.cell_output(type, portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : اشاره_تهی;
		أرجع w && w->پورت_output;
	}
	أرجع خطأ;
}

RTLIL::پورت_Dir RTLIL::Cell::پورت_dir(RTLIL::IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_معلوم(type.ref()))
		أرجع yosys_celltypes.cell_پورت_dir(type, portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		إذا (m == اشاره_تهی)
			أرجع پورت_Dir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		إذا (w == اشاره_تهی)
			أرجع پورت_Dir::PD_UNKNOWN;
		أرجع پورت_Dir(w->پورت_درون_داد + w->پورت_output * 2);
	}
	أرجع پورت_Dir::PD_UNKNOWN;
}

منطقي RTLIL::Cell::hasParam(RTLIL::IdString paramname) ثابت
{
	أرجع parameters.شمار(paramname) != 0;
}

فراغ RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

فراغ RTLIL::Cell::مجموعه_Param(RTLIL::IdString paramname, RTLIL::Const مقدار)
{
	parameters[paramname] = std::move(مقدار);
}

ثابت RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ثابت
{
	ثابت خودکار &it = parameters.پیدا_کن(paramname);
	إذا (it != parameters.انتها())
		أرجع it->دومی;
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		إذا (m)
			أرجع m->parameter_default_مقادیر.at(paramname);
	}
	ارم std::بیرون_از_range("Cell::getParam()");
}

فراغ RTLIL::Cell::مرتب_سازی()
{
	connections_.مرتب_سازی(مرتب_سازی_by_id_str(module->twines()));
	parameters.مرتب_سازی(مرتب_سازی_by_id_str(module->twines()));
	صفات.مرتب_سازی(مرتب_سازی_by_id_str(module->twines()));
}

فراغ RTLIL::Cell::check()
{
#اگر_تعریف_نشده NDEBUG
	داخلی_Cell_Checker checker(عدم, هذا);
	checker.check();
#پایان_اگر
}

فراغ RTLIL::Cell::fixup_parameters(منطقي مجموعه_a_signed, منطقي مجموعه_b_signed)
{
	إذا (type.is_عمومی() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		أرجع;

	إذا (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::عرض_2] = Get_اندازه(connections_[ID::Y]);
		إذا (type.درون(ID($pmux), ID($bmux)))
			parameters[ID::S_عرض] = Get_اندازه(connections_[ID::S]);
		check();
		أرجع;
	}

	إذا (type == ID($demux)) {
		parameters[ID::عرض_2] = Get_اندازه(connections_[ID::A]);
		parameters[ID::S_عرض] = Get_اندازه(connections_[ID::S]);
		check();
		أرجع;
	}

	إذا (type == ID($lut) || type == ID($sop)) {
		parameters[ID::عرض_2] = Get_اندازه(connections_[ID::A]);
		أرجع;
	}

	إذا (type == ID($fa)) {
		parameters[ID::عرض_2] = Get_اندازه(connections_[ID::Y]);
		أرجع;
	}

	إذا (type == ID($lcu)) {
		parameters[ID::عرض_2] = Get_اندازه(connections_[ID::CO]);
		أرجع;
	}

	إذا (type == ID($macc_v2)) {
		parameters[ID::Y_عرض] = Get_اندازه(connections_[ID::Y]);
		أرجع;
	}

	منطقي signedness_ab = !type.درون(ID($برش), ID($concat), ID($macc));

	إذا (connections_.شمار(ID::A)) {
		إذا (signedness_ab) {
			إذا (مجموعه_a_signed)
				parameters[ID::A_SIGNED] = درست;
			وإلا إذا (parameters.شمار(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = خطأ;
		}
		parameters[ID::A_عرض] = Get_اندازه(connections_[ID::A]);
	}

	إذا (connections_.شمار(ID::B)) {
		إذا (signedness_ab) {
			إذا (مجموعه_b_signed)
				parameters[ID::B_SIGNED] = درست;
			وإلا إذا (parameters.شمار(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = خطأ;
		}
		parameters[ID::B_عرض] = Get_اندازه(connections_[ID::B]);
	}

	إذا (connections_.شمار(ID::Y) && type != ID($concat))
		parameters[ID::Y_عرض] = Get_اندازه(connections_[ID::Y]);

	إذا (connections_.شمار(ID::Q))
		parameters[ID::عرض_2] = Get_اندازه(connections_[ID::Q]);

	check();
}

منطقي RTLIL::Cell::has_keep_attr() ثابت {
	أرجع get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

منطقي RTLIL::Cell::has_memid() ثابت
{
	أرجع type.درون(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

منطقي RTLIL::Cell::is_mem_cell() ثابت
{
	أرجع type.درون(ID($mem), ID($mem_v2)) || has_memid();
}

منطقي RTLIL::Cell::is_builtin_ff() ثابت {
	أرجع StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ثابت RTLIL::Sig_بیت &بیت)
{
	wire = بیت.wire;
	جابه_جایی = 0;
	إذا (wire == عدم)
		داده = {بیت.داده};
	وإلا
		جابه_جایی = بیت.جابه_جایی;
	عرض = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::استخراج(صحيح جابه_جایی, صحيح درازی) ثابت
{
	log_assert(جابه_جایی >= 0);
	log_assert(درازی >= 0);
	log_assert(جابه_جایی + درازی <= عرض);
	RTLIL::SigChunk ret;
	إذا (wire) {
		ret.wire = wire;
		ret.جابه_جایی = هذا->جابه_جایی + جابه_جایی;
		ret.عرض = درازی;
	} وإلا {
		لكل (صحيح i = 0; i < درازی; i++)
			ret.داده.افزودن_پشت(داده[جابه_جایی+i]);
		ret.عرض = درازی;
	}
	أرجع ret;
}

RTLIL::Sig_بیت RTLIL::SigChunk::عملگر[](صحيح جابه_جایی) ثابت
{
	log_assert(جابه_جایی >= 0);
	log_assert(جابه_جایی < عرض);
	RTLIL::Sig_بیت ret;
	إذا (wire) {
		ret.wire = wire;
		ret.جابه_جایی = هذا->جابه_جایی + جابه_جایی;
	} وإلا {
		ret.داده = داده[جابه_جایی];
	}
	أرجع ret;
}

منطقي RTLIL::SigChunk::عملگر <(ثابت RTLIL::SigChunk &other) ثابت
{
	إذا (wire && other.wire)
		إذا (wire->name != other.wire->name)
			أرجع wire->name < other.wire->name;

	إذا (wire != other.wire)
		أرجع wire < other.wire;

	إذا (جابه_جایی != other.جابه_جایی)
		أرجع جابه_جایی < other.جابه_جایی;

	إذا (عرض != other.عرض)
		أرجع عرض < other.عرض;

	أرجع داده < other.داده;
}

منطقي RTLIL::SigChunk::عملگر ==(ثابت RTLIL::SigChunk &other) ثابت
{
	أرجع wire == other.wire && عرض == other.عرض && جابه_جایی == other.جابه_جایی && داده == other.داده;
}

منطقي RTLIL::SigChunk::عملگر !=(ثابت RTLIL::SigChunk &other) ثابت
{
	إذا (*هذا == other)
		أرجع خطأ;
	أرجع درست;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_خالی_بیتها();
	log_assert(parts.اندازه() > 0);
	خودکار ie = parts.begin();
	خودکار it = ie + parts.اندازه() - 1;
	طالما (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::Const &مقدار)
{
	إذا (Get_اندازه(مقدار) != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(مقدار);
	} وإلا {
		init_خالی_بیتها();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&مقدار)
{
	إذا (Get_اندازه(مقدار) != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(مقدار);
	} وإلا {
		init_خالی_بیتها();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::SigChunk &chunk)
{
	إذا (chunk.عرض != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(chunk);
	} وإلا {
		init_خالی_بیتها();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	إذا (chunk.عرض != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(chunk);
	} وإلا {
		init_خالی_بیتها();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	إذا (wire->عرض != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(wire);
	} وإلا {
		init_خالی_بیتها();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, صحيح جابه_جایی, صحيح عرض)
{
	إذا (عرض != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(wire, جابه_جایی, عرض);
	} وإلا {
		init_خالی_بیتها();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::string &str)
{
	إذا (str.اندازه() != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(str);
	} وإلا {
		init_خالی_بیتها();
	}
	check();
}

RTLIL::SigSpec::SigSpec(صحيح val, صحيح عرض)
{
	إذا (عرض != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(val, عرض);
	} وإلا
		init_خالی_بیتها();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State بیت, صحيح عرض)
{
	إذا (عرض != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(بیت, عرض);
	} وإلا
		init_خالی_بیتها();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::Sig_بیت &بیت, صحيح عرض)
{
	إذا (عرض != 0) {
		إذا (بیت.wire == عدم) {
			rep_ = CHUNK;
			جديد (&chunk_) RTLIL::SigChunk(بیت.داده, عرض);
		} وإلا إذا (عرض == 1) {
			rep_ = CHUNK;
			جديد (&chunk_) RTLIL::SigChunk(بیت);
		} وإلا {
			init_خالی_بیتها();
			بیتها_2.reserve(عرض);
			لكل (صحيح i = 0; i < عرض; i++)
				بیتها_2.افزودن_پشت(بیت);
		}
	} وإلا
		init_خالی_بیتها();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::وکتور<RTLIL::SigChunk> &chunks)
{
	init_خالی_بیتها();
	لكل (ثابت خودکار &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::وکتور<RTLIL::Sig_بیت> &بیتها)
{
	init_خالی_بیتها();
	لكل (ثابت خودکار &بیت : بیتها)
		append(بیت);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت pool<RTLIL::Sig_بیت> &بیتها)
{
	init_خالی_بیتها();
	لكل (ثابت خودکار &بیت : بیتها)
		append(بیت);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::مجموعه<RTLIL::Sig_بیت> &بیتها)
{
	init_خالی_بیتها();
	لكل (ثابت خودکار &بیت : بیتها)
		append(بیت);
	check();
}

RTLIL::SigSpec::SigSpec(منطقي بیت)
{
	rep_ = CHUNK;
	جديد (&chunk_) RTLIL::SigChunk(بیت ? RTLIL::S1 : RTLIL::S0);
	check();
}

فراغ RTLIL::SigSpec::Chunks::const_iterator::بعدی_chunk_بیتها()
{
	صحيح بیتها_اندازه = Get_اندازه(spec.بیتها_2);
	إذا (بیت_شاخص >= بیتها_اندازه)
		أرجع;
	صحيح i = بیت_شاخص;
	ثابت Sig_بیت &بیت = spec.بیتها_2[i++];
	chunk.wire = بیت.wire;
	chunk.داده.clear();
	إذا (بیت.is_wire()) {
		chunk.جابه_جایی = بیت.جابه_جایی;
		طالما (i < بیتها_اندازه && spec.بیتها_2[i].wire == بیت.wire &&
			spec.بیتها_2[i].جابه_جایی == بیت.جابه_جایی + i - بیت_شاخص)
			++i;
	} وإلا {
		chunk.جابه_جایی = 0;
		chunk.داده.افزودن_پشت(بیت.داده);
		طالما (i < بیتها_اندازه && !spec.بیتها_2[i].is_wire()) {
			chunk.داده.افزودن_پشت(spec.بیتها_2[i].داده);
			++i;
		}
	}
	chunk.عرض = i - بیت_شاخص;
}

فراغ RTLIL::SigSpec::unpack()
{
	إذا (rep_ == بیتها_3)
		أرجع;

	std::وکتور<RTLIL::Sig_بیت> بیتها;
	بیتها.reserve(chunk_.عرض);
	لكل (صحيح i = 0; i < chunk_.عرض; i++)
		بیتها.emplace_پشت(chunk_, i);

	chunk_.~SigChunk();
	rep_ = بیتها_3;
	جديد (&بیتها_2) std::وکتور<RTLIL::Sig_بیت>(std::move(بیتها));
}

فراغ RTLIL::SigSpec::try_repack()
{
	إذا (rep_ != بیتها_3)
		أرجع;

	صحيح بیتها_اندازه = Get_اندازه(بیتها_2);
	إذا (بیتها_اندازه == 0)
		أرجع;
	إذا (بیتها_2[0].is_wire()) {
		لكل (صحيح i = 1; i < بیتها_اندازه; i++)
			إذا (بیتها_2[0].wire != بیتها_2[i].wire || بیتها_2[0].جابه_جایی + i != بیتها_2[i].جابه_جایی)
				أرجع;
		SigChunk chunk(بیتها_2[0].wire, بیتها_2[0].جابه_جایی, بیتها_اندازه);
		بیتها_2.~وکتور();
		rep_ = CHUNK;
		جديد (&chunk_) SigChunk(std::move(chunk));
		أرجع;
	}
	std::وکتور<RTLIL::State> بیتها;
	بیتها.reserve(بیتها_اندازه);
	بیتها.افزودن_پشت(بیتها_2[0].داده);
	لكل (صحيح i = 1; i < بیتها_اندازه; i++) {
		إذا (بیتها_2[i].is_wire())
			أرجع;
		بیتها.افزودن_پشت(بیتها_2[i].داده);
	}
	بیتها_2.~وکتور();
	rep_ = CHUNK;
	جديد (&chunk_) SigChunk(std::move(بیتها));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ثابت
{
	Hasher h;
	لكل (خودکار &c : chunks())
		إذا (c.wire == عدم) {
			لكل (خودکار &v : c.داده)
				h.eat(v);
		} وإلا {
			h.eat(c.wire->name.ref());
			h.eat(c.جابه_جایی);
			h.eat(c.عرض);
		}
	Hasher::hash_t نتیجه = h.yield();
	إذا (نتیجه == 0)
		نتیجه = 1;
	hash_.مجموعه(نتیجه);
	أرجع نتیجه;
}

فراغ RTLIL::SigSpec::مرتب_سازی()
{
	unpack();
	std::مرتب_سازی(بیتها_2.begin(), بیتها_2.انتها());
	hash_.clear();
	try_repack();
}

فراغ RTLIL::SigSpec::مرتب_سازی_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::وکتور<Sig_بیت> unique_بیتها = بیتها_2;
	std::مرتب_سازی(unique_بیتها.begin(), unique_بیتها.انتها());
	خودکار آخرین = std::unique(unique_بیتها.begin(), unique_بیتها.انتها());
	unique_بیتها.erase(آخرین, unique_بیتها.انتها());

	*هذا = unique_بیتها;
	hash_.clear();
	try_repack();
}

فراغ RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &الگو, ثابت RTLIL::SigSpec &with)
{
	replace(الگو, with, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &الگو, ثابت RTLIL::SigSpec &with, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(اندازه() == other->اندازه());
	log_assert(الگو.اندازه() == with.اندازه());

	dict<RTLIL::Sig_بیت, صحيح> الگو_مقصد_with;
	صحيح الگو_اندازه = الگو.اندازه();
	لكل (صحيح i = 0; i < الگو_اندازه; i++) {
		Sig_بیت الگو_بیت = الگو[i];
		إذا (الگو_بیت.wire != عدم) {
			الگو_مقصد_with.emplace(الگو_بیت, i);
		}
	}

	صحيح this_اندازه = اندازه();
	منطقي other_modified = خطأ;
	لكل (صحيح j = 0; j < this_اندازه; j++) {
		خودکار it = الگو_مقصد_with.پیدا_کن((*هذا)[j]);
		إذا (it != الگو_مقصد_with.انتها()) {
			other_modified = درست;
			(*other)[j] = with[it->دومی];
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::replace(ثابت dict<RTLIL::Sig_بیت, RTLIL::Sig_بیت> &قواعد)
{
	replace(قواعد, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت dict<RTLIL::Sig_بیت, RTLIL::Sig_بیت> &قواعد, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(اندازه() == other->اندازه());

	إذا (قواعد.خالی()) أرجع;

	صحيح this_اندازه = اندازه();
	منطقي other_modified = خطأ;
	لكل (صحيح i = 0; i < this_اندازه; i++) {
		خودکار it = قواعد.پیدا_کن((*هذا)[i]);
		إذا (it != قواعد.انتها()) {
			other_modified = درست;
			(*other)[i] = it->دومی;
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::replace(ثابت std::map<RTLIL::Sig_بیت, RTLIL::Sig_بیت> &قواعد)
{
	replace(قواعد, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت std::map<RTLIL::Sig_بیت, RTLIL::Sig_بیت> &قواعد, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(اندازه() == other->اندازه());

	إذا (قواعد.خالی()) أرجع;

	صحيح this_اندازه = اندازه();
	منطقي other_modified = خطأ;
	لكل (صحيح i = 0; i < this_اندازه; i++) {
		خودکار it = قواعد.پیدا_کن((*هذا)[i]);
		إذا (it != قواعد.انتها()) {
			other_modified = درست;
			(*other)[i] = it->دومی;
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &الگو)
{
	remove2(الگو, عدم);
}

فراغ RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &الگو, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *هذا;
	tmp.remove2(الگو, other);
}

فراغ RTLIL::SigSpec::remove2(ثابت RTLIL::SigSpec &الگو, RTLIL::SigSpec *other)
{
	unpack();
	إذا (other != عدم) {
		log_assert(اندازه() == other->اندازه());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<Sig_بیت> الگو_بیتها;
	الگو_بیتها.reserve(الگو.اندازه());
	لكل (خودکار &بیت : الگو)
		إذا (بیت.wire != عدم)
			الگو_بیتها.درج(بیت);

	// Compact in-place to avoid O(n^2) erase operations
	اندازه_t write_idx = 0;
	لكل (اندازه_t read_idx = 0; read_idx < بیتها_2.اندازه(); read_idx++)
	{
		إذا (!(بیتها_2[read_idx].wire != عدم && الگو_بیتها.شمار(بیتها_2[read_idx]))) {
			إذا (write_idx != read_idx) {
				بیتها_2[write_idx] = بیتها_2[read_idx];
				إذا (other != عدم)
					other->بیتها_2[write_idx] = other->بیتها_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < بیتها_2.اندازه());
	إذا (modified) {
		بیتها_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->بیتها_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove(ثابت pool<RTLIL::Sig_بیت> &الگو)
{
	remove2(الگو, عدم);
}

فراغ RTLIL::SigSpec::remove(ثابت pool<RTLIL::Sig_بیت> &الگو, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *هذا;
	tmp.remove2(الگو, other);
}

فراغ RTLIL::SigSpec::remove2(ثابت pool<RTLIL::Sig_بیت> &الگو, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(اندازه() == other->اندازه());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	اندازه_t write_idx = 0;
	لكل (اندازه_t read_idx = 0; read_idx < بیتها_2.اندازه(); read_idx++) {
		إذا (!(بیتها_2[read_idx].wire != عدم && الگو.شمار(بیتها_2[read_idx]))) {
			إذا (write_idx != read_idx) {
				بیتها_2[write_idx] = بیتها_2[read_idx];
				إذا (other != عدم)
					other->بیتها_2[write_idx] = other->بیتها_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < بیتها_2.اندازه());
	إذا (modified) {
		بیتها_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->بیتها_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove2(ثابت std::مجموعه<RTLIL::Sig_بیت> &الگو, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(اندازه() == other->اندازه());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	اندازه_t write_idx = 0;
	لكل (اندازه_t read_idx = 0; read_idx < بیتها_2.اندازه(); read_idx++) {
		إذا (!(بیتها_2[read_idx].wire != عدم && الگو.شمار(بیتها_2[read_idx]))) {
			إذا (write_idx != read_idx) {
				بیتها_2[write_idx] = بیتها_2[read_idx];
				إذا (other != عدم)
					other->بیتها_2[write_idx] = other->بیتها_2[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < بیتها_2.اندازه());
	إذا (modified) {
		بیتها_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->بیتها_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove2(ثابت pool<RTLIL::Wire*> &الگو, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(اندازه() == other->اندازه());
		other->unpack();
	}

	منطقي modified = خطأ;
	منطقي other_modified = خطأ;
	لكل (صحيح i = Get_اندازه(بیتها_2) - 1; i >= 0; i--) {
		إذا (بیتها_2[i].wire != عدم && الگو.شمار(بیتها_2[i].wire)) {
			modified = درست;
			بیتها_2.erase(بیتها_2.begin() + i);
			إذا (other != عدم) {
				other_modified = درست;
				other->بیتها_2.erase(other->بیتها_2.begin() + i);
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

RTLIL::SigSpec RTLIL::SigSpec::استخراج(ثابت RTLIL::SigSpec &الگو, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == عدم || اندازه() == other->اندازه());

	RTLIL::SigSpec ret;
	std::وکتور<RTLIL::Sig_بیت> بیتها_تطابق = مقصد_sigbit_وکتور();

	لكل (خودکار& الگو_chunk : الگو.chunks()) {
		إذا (other) {
			std::وکتور<RTLIL::Sig_بیت> بیتها_other = other->مقصد_sigbit_وکتور();
			صحيح i = 0;
			لكل (ثابت RTLIL::Sig_بیت &بیت : بیتها_تطابق) {
				إذا (بیت.wire &&
					بیت.wire == الگو_chunk.wire &&
					بیت.جابه_جایی >= الگو_chunk.جابه_جایی &&
					بیت.جابه_جایی < الگو_chunk.جابه_جایی + الگو_chunk.عرض)
					ret.append(بیتها_other[i]);
				++i;
			}
		} وإلا {
			لكل (ثابت RTLIL::Sig_بیت &بیت : بیتها_تطابق)
				إذا (بیت.wire &&
					بیت.wire == الگو_chunk.wire &&
					بیت.جابه_جایی >= الگو_chunk.جابه_جایی &&
					بیت.جابه_جایی < الگو_chunk.جابه_جایی + الگو_chunk.عرض)
					ret.append(بیت);
		}
	}

	ret.check();
	أرجع ret;
}

RTLIL::SigSpec RTLIL::SigSpec::استخراج(ثابت pool<RTLIL::Sig_بیت> &الگو, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == عدم || اندازه() == other->اندازه());

	std::وکتور<RTLIL::Sig_بیت> بیتها_تطابق = مقصد_sigbit_وکتور();
	RTLIL::SigSpec ret;

	إذا (other) {
		std::وکتور<RTLIL::Sig_بیت> بیتها_other = other->مقصد_sigbit_وکتور();
		صحيح i = 0;
		لكل (ثابت RTLIL::Sig_بیت &بیت : بیتها_تطابق) {
			إذا (بیت.wire && الگو.شمار(بیت))
				ret.append(بیتها_other[i]);
			++i;
		}
	} وإلا {
		لكل (ثابت RTLIL::Sig_بیت &بیت : بیتها_تطابق)
			إذا (بیت.wire && الگو.شمار(بیت))
				ret.append(بیت);
	}

	ret.check();
	أرجع ret;
}

فراغ RTLIL::SigSpec::replace(صحيح جابه_جایی, ثابت RTLIL::SigSpec &with)
{
	إذا (with.اندازه() == 0)
		أرجع;

	unpack();

	log_assert(جابه_جایی >= 0);
	log_assert(with.اندازه() >= 0);
	log_assert(جابه_جایی+with.اندازه() <= اندازه());

	صحيح i = 0;
	لكل (ثابت RTLIL::Sig_بیت &بیت : with.بیتها()) {
		بیتها_2.at(جابه_جایی + i) = بیت;
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
			init_خالی_بیتها();
			hash_.clear();
		}
	}
	وإلا
	{
		std::وکتور<RTLIL::Sig_بیت> new_بیتها;
		new_بیتها.reserve(بیتها_2.اندازه());
		لكل (خودکار &بیت : بیتها_2)
			إذا (بیت.wire != عدم)
				new_بیتها.افزودن_پشت(بیت);
		إذا (Get_اندازه(new_بیتها) != Get_اندازه(بیتها_2)) {
			بیتها_2.جابجا(new_بیتها);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

فراغ RTLIL::SigSpec::remove(صحيح جابه_جایی, صحيح درازی)
{
	إذا (درازی == 0)
		أرجع;

	unpack();

	log_assert(جابه_جایی >= 0);
	log_assert(درازی >= 0);
	log_assert(جابه_جایی + درازی <= اندازه());

	بیتها_2.erase(بیتها_2.begin() + جابه_جایی, بیتها_2.begin() + جابه_جایی + درازی);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::استخراج(صحيح جابه_جایی, صحيح درازی) ثابت
{
	log_assert(جابه_جایی >= 0);
	log_assert(درازی >= 0);
	log_assert(جابه_جایی + درازی <= اندازه());

	std::وکتور<Sig_بیت> extracted;
	Sig_بیت اولی;
	منطقي is_packing = درست;
	لكل (صحيح i = جابه_جایی; i < جابه_جایی + درازی; i++) {
		منطقي was_packing_before = is_packing;
		Sig_بیت بیت = (*هذا)[i];
		إذا (i == جابه_جایی) {
			اولی = بیت;
			إذا (!بیت.wire)
				is_packing = خطأ;
		} وإلا {
			إذا (بیت.wire != اولی.wire)
				is_packing = خطأ;
			إذا (بیت.wire)
				إذا (بیت.جابه_جایی != اولی.جابه_جایی + (i - جابه_جایی))
					is_packing = خطأ;
		}
		إذا (was_packing_before && !is_packing)
			لكل (صحيح j = جابه_جایی; j < i; j++)
				extracted.افزودن_پشت((*هذا)[j]);
		إذا (!is_packing)
			extracted.افزودن_پشت((*هذا)[i]);
	}
	إذا (is_packing)
		أرجع SigChunk(اولی.wire, اولی.جابه_جایی, درازی);

	أرجع extracted;
}

فراغ RTLIL::SigSpec::rewrite_wires(std::function<فراغ(RTLIL::Wire*& wire)> rewrite)
{
	إذا (rep_ == CHUNK) {
		إذا (chunk_.wire != اشاره_تهی)
			rewrite(chunk_.wire);
		أرجع;
	}

	std::وکتور<RTLIL::Sig_بیت> new_بیتها;
	لكل (ثابت RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		إذا (c.wire != اشاره_تهی)
			rewrite(c.wire);
		لكل (صحيح i = 0; i < c.عرض; i++)
			new_بیتها.emplace_پشت(c, i);
	}
	بیتها_2 = std::move(new_بیتها);
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
			chunk_.داده.درج(chunk_.داده.انتها(), إشارة.chunk_.داده.begin(), إشارة.chunk_.داده.انتها());
			chunk_.عرض = Get_اندازه(chunk_.داده);
			أرجع;
		}
		إذا (chunk_.جابه_جایی + chunk_.عرض == إشارة.chunk_.جابه_جایی) {
			chunk_.عرض += إشارة.chunk_.عرض;
			أرجع;
		}
	}

	unpack();
	لكل (ثابت Sig_بیت &بیت : إشارة.بیتها())
		بیتها_2.افزودن_پشت(بیت);
	check();
}

فراغ RTLIL::SigSpec::append(ثابت RTLIL::Sig_بیت &بیت)
{
	hash_.clear();

	إذا (اندازه() == 0) {
		destroy();
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(بیت);
		أرجع;
	}

	إذا (rep_ == CHUNK && chunk_.wire == بیت.wire) {
		إذا (chunk_.wire == عدم) {
			chunk_.داده.افزودن_پشت(بیت.داده);
			chunk_.عرض++;
			أرجع;
		}
		إذا (chunk_.جابه_جایی + chunk_.عرض == بیت.جابه_جایی) {
			chunk_.عرض++;
			أرجع;
		}
	}

	unpack();

	بیتها_2.افزودن_پشت(بیت);
	check();
}

فراغ RTLIL::SigSpec::extend_u0(صحيح عرض, منطقي is_signed)
{
	إذا (اندازه() > عرض)
		remove(عرض, اندازه() - عرض);

	إذا (اندازه() < عرض) {
		RTLIL::Sig_بیت padding = اندازه() > 0 ? (*هذا)[اندازه() - 1] : RTLIL::State::Sx;
		إذا (!is_signed)
			padding = RTLIL::State::S0;
		طالما (اندازه() < عرض)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(صحيح رقم) ثابت
{
	RTLIL::SigSpec sig;
	لكل (صحيح i = 0; i < رقم; i++)
		sig.append(*هذا);
	أرجع sig;
}

#اگر_تعریف_نشده NDEBUG
فراغ RTLIL::SigSpec::check(ثابت Module *mod) ثابت
{
	إذا (rep_ == CHUNK)
	{
		log_assert(chunk_.عرض != 0);
		إذا (chunk_.wire == عدم) {
			log_assert(chunk_.جابه_جایی == 0);
			log_assert(chunk_.داده.اندازه() == (اندازه_t)chunk_.عرض);
		} وإلا {
			log_assert(chunk_.جابه_جایی >= 0);
			log_assert(chunk_.عرض >= 0);
			log_assert(chunk_.جابه_جایی + chunk_.عرض <= chunk_.wire->عرض);
			log_assert(chunk_.داده.اندازه() == 0);
			إذا (mod != اشاره_تهی)
				log_assert(chunk_.wire->module == mod);
		}
	}
	وإلا إذا (اندازه() <= 64)
	{
		إذا (mod != اشاره_تهی) {
			لكل (ثابت RTLIL::Sig_بیت &بیت : بیتها_2)
				إذا (بیت.wire != اشاره_تهی)
					log_assert(بیت.wire->module == mod);
		}
	}
}
#پایان_اگر

منطقي RTLIL::SigSpec::عملگر <(ثابت RTLIL::SigSpec &other) ثابت
{
	إذا (هذا == &other)
		أرجع خطأ;

	إذا (اندازه() != other.اندازه())
		أرجع اندازه() < other.اندازه();

	خودکار other_it = other.chunks().begin();
	لكل (ثابت SigChunk &c : chunks()) {
		إذا (c != *other_it)
			أرجع c < *other_it;
		++other_it;
	}

	أرجع خطأ;
}

منطقي RTLIL::SigSpec::عملگر ==(ثابت RTLIL::SigSpec &other) ثابت
{
	إذا (هذا == &other)
		أرجع درست;

	إذا (اندازه() != other.اندازه())
		أرجع خطأ;

	خودکار other_it = other.chunks().begin();
	لكل (ثابت SigChunk &c : chunks()) {
		إذا (c != *other_it)
			أرجع خطأ;
		++other_it;
	}

	أرجع درست;
}

منطقي RTLIL::SigSpec::is_wire() ثابت
{
	Chunks cs = chunks();
	خودکار it = cs.begin();
	إذا (it == cs.انتها())
		أرجع خطأ;
	ثابت RTLIL::SigChunk &chunk = *it;
	أرجع chunk.wire && chunk.wire->عرض == اندازه() && ++it == cs.انتها();
}

منطقي RTLIL::SigSpec::is_chunk() ثابت
{
	Chunks cs = chunks();
	خودکار it = cs.begin();
	إذا (it == cs.انتها())
		أرجع خطأ;
	أرجع ++it == cs.انتها();
}

منطقي RTLIL::SigSpec::معلوم_driver() ثابت
{
	لكل (خودکار &chunk : chunks())
		إذا (chunk.is_wire() && !chunk.wire->معلوم_driver())
			أرجع خطأ;
	أرجع درست;
}

منطقي RTLIL::SigSpec::is_fully_const() ثابت
{
	لكل (خودکار &chunk : chunks())
		إذا (chunk.عرض > 0 && chunk.wire != عدم)
			أرجع خطأ;
	أرجع درست;
}

منطقي RTLIL::SigSpec::is_fully_zero() ثابت
{
	لكل (خودکار &chunk : chunks()) {
		إذا (chunk.عرض > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.داده)
			إذا (d != RTLIL::State::S0)
				أرجع خطأ;
	}
	أرجع درست;
}

منطقي RTLIL::SigSpec::is_fully_ones() ثابت
{
	لكل (خودکار &chunk : chunks()) {
		إذا (chunk.عرض > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.داده)
			إذا (d != RTLIL::State::S1)
				أرجع خطأ;
	}
	أرجع درست;
}

منطقي RTLIL::SigSpec::is_fully_def() ثابت
{
	لكل (خودکار &chunk : chunks()) {
		إذا (chunk.عرض > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.داده)
			إذا (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				أرجع خطأ;
	}
	أرجع درست;
}

منطقي RTLIL::SigSpec::is_fully_undef() ثابت
{
	لكل (خودکار &chunk : chunks()) {
		إذا (chunk.عرض > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.داده)
			إذا (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				أرجع خطأ;
	}
	أرجع درست;
}

منطقي RTLIL::SigSpec::has_const() ثابت
{
	لكل (خودکار &chunk : chunks())
		إذا (chunk.عرض > 0 && chunk.wire == عدم)
			أرجع درست;
	أرجع خطأ;
}

منطقي RTLIL::SigSpec::has_const(State state) ثابت
{
	لكل (خودکار &chunk : chunks())
		إذا (chunk.عرض > 0 && chunk.wire == عدم && std::پیدا_کن(chunk.داده.begin(), chunk.داده.انتها(), state) != chunk.داده.انتها())
			أرجع درست;
	أرجع خطأ;
}


منطقي RTLIL::SigSpec::has_marked_بیتها() ثابت
{
	لكل (خودکار &chunk : chunks())
		إذا (chunk.عرض > 0 && chunk.wire == عدم) {
			لكل (RTLIL::State d : chunk.داده)
				إذا (d == RTLIL::State::Sm)
					أرجع درست;
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
	log_assert(c.has_مقدار());
	أرجع c->as_bool();
}

صحيح RTLIL::SigSpec::as_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_مقدار());
	أرجع c->as_int(is_signed);
}

منطقي RTLIL::SigSpec::convertible_مقصد_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	إذا (!c.has_مقدار())
		أرجع خطأ;
	أرجع c->convertible_مقصد_int(is_signed);
}

std::optional<صحيح> RTLIL::SigSpec::try_as_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	إذا (!c.has_مقدار())
		أرجع std::nullopt;
	أرجع c->try_as_int(is_signed);
}

صحيح RTLIL::SigSpec::as_int_saturating(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_مقدار());
	أرجع c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ثابت
{
	std::string str;
	str.reserve(اندازه());
	std::وکتور<RTLIL::SigChunk> chunks = *هذا;
	لكل (اندازه_t i = chunks.اندازه(); i > 0; i--) {
		ثابت RTLIL::SigChunk &chunk = chunks[i-1];
		إذا (chunk.wire != عدم)
			str.append(chunk.عرض, '?');
		وإلا
			str += RTLIL::Const(chunk.داده).as_string();
	}
	أرجع str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ثابت
{
	Chunks cs = chunks();
	خودکار it = cs.begin();
	إذا (it == cs.انتها())
		أرجع RTLIL::Const();
	SigChunk chunk = *it;
	إذا (chunk.wire != عدم || ++it != cs.انتها())
		أرجع std::nullopt;
	أرجع RTLIL::Const(std::move(chunk.داده));
}

RTLIL::Const RTLIL::SigSpec::as_const() ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_مقدار());
	أرجع *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ثابت
{
	Chunks cs = chunks();
	خودکار it = cs.begin();
	log_assert(it != cs.انتها());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.انتها() && chunk.wire && chunk.wire->عرض == اندازه());
	أرجع chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ثابت
{
	Chunks cs = chunks();
	خودکار it = cs.begin();
	log_assert(it != cs.انتها());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.انتها());
	أرجع chunk;
}

RTLIL::Sig_بیت RTLIL::SigSpec::as_بیت() ثابت
{
	أرجع RTLIL::Sig_بیت(*هذا);
}

منطقي RTLIL::SigSpec::تطابق(ثابت حرف* الگو) ثابت
{
	صحيح الگو_len = طول_رشته(الگو);
	log_assert(الگو_len == اندازه());

	لكل (صحيح i = 0; i < الگو_len; ++i) {
		حرف ch = الگو[i];
		إذا (ch == ' ')
			استمر;
		RTLIL::Sig_بیت بیت = (*هذا)[الگو_len - 1 - i];
		إذا (ch == '*') {
			إذا (بیت != State::Sz && بیت != State::Sx)
				أرجع خطأ;
			استمر;
		}
		إذا (ch == '0') {
			إذا (بیت != State::S0)
				أرجع خطأ;
		} وإلا
		إذا (ch == '1') {
			إذا (بیت != State::S1)
				أرجع خطأ;
		} وإلا
			log_abort();
	}

	أرجع درست;
}

std::مجموعه<RTLIL::Sig_بیت> RTLIL::SigSpec::مقصد_sigbit_مجموعه() ثابت
{
	std::مجموعه<RTLIL::Sig_بیت> sigbits;
	لكل (خودکار &c : chunks())
		لكل (صحيح i = 0; i < c.عرض; i++)
			sigbits.درج(RTLIL::Sig_بیت(c, i));
	أرجع sigbits;
}

pool<RTLIL::Sig_بیت> RTLIL::SigSpec::مقصد_sigbit_pool() ثابت
{
	pool<RTLIL::Sig_بیت> sigbits;
	sigbits.reserve(اندازه());
	لكل (خودکار &c : chunks())
		لكل (صحيح i = 0; i < c.عرض; i++)
			sigbits.درج(RTLIL::Sig_بیت(c, i));
	أرجع sigbits;
}

std::وکتور<RTLIL::Sig_بیت> RTLIL::SigSpec::مقصد_sigbit_وکتور() ثابت
{
	std::وکتور<RTLIL::Sig_بیت> نتیجه;
	نتیجه.reserve(اندازه());
	لكل (Sig_بیت بیت : *هذا)
		نتیجه.افزودن_پشت(بیت);
	أرجع نتیجه;
}

std::map<RTLIL::Sig_بیت, RTLIL::Sig_بیت> RTLIL::SigSpec::مقصد_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت
{
	صحيح this_اندازه = اندازه();
	log_assert(this_اندازه == other.اندازه());

	std::map<RTLIL::Sig_بیت, RTLIL::Sig_بیت> new_map;
	لكل (صحيح i = 0; i < this_اندازه; i++)
		new_map[(*هذا)[i]] = other[i];

	أرجع new_map;
}

dict<RTLIL::Sig_بیت, RTLIL::Sig_بیت> RTLIL::SigSpec::مقصد_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت
{
	صحيح this_اندازه = اندازه();
	log_assert(this_اندازه == other.اندازه());

	dict<RTLIL::Sig_بیت, RTLIL::Sig_بیت> new_map;
	new_map.reserve(this_اندازه);
	لكل (صحيح i = 0; i < this_اندازه; i++)
		new_map[(*هذا)[i]] = other[i];

	أرجع new_map;
}

ساكن فراغ sigspec_parse_تقسیم(std::وکتور<std::string> &نشانهها, ثابت std::string &متن, حرف sep)
{
	اندازه_t شروع = 0, انتها = 0;
	طالما ((انتها = متن.پیدا_کن(sep, شروع)) != std::string::npos) {
		نشانهها.افزودن_پشت(متن.substr(شروع, انتها - شروع));
		شروع = انتها + 1;
	}
	نشانهها.افزودن_پشت(متن.substr(شروع));
}

ساكن RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ثابت std::string &netname)
{
	أرجع module->wire(netname);
}

منطقي RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::وکتور<std::string> نشانهها;
	sigspec_parse_تقسیم(نشانهها, str, ',');

	sig = RTLIL::SigSpec();
	لكل (صحيح tokidx = صحيح(نشانهها.اندازه())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = نشانهها[tokidx];
		std::string indices;

		إذا (netname.اندازه() == 0)
			استمر;

		إذا (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			خودکار ast = p.const2ast(netname);
			إذا (ast == اشاره_تهی)
				أرجع خطأ;
			sig.append(RTLIL::Const(ast->بیتها));
			استمر;
		}

		إذا (module == عدم)
			أرجع خطأ;

		إذا (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		إذا (sigspec_parse_wire(module, netname) == اشاره_تهی) {
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
		إذا (wire == اشاره_تهی)
			أرجع خطأ;

		إذا (!indices.خالی()) {
			std::وکتور<std::string> شاخص_نشانهها;
			sigspec_parse_تقسیم(شاخص_نشانهها, indices.substr(1, indices.اندازه()-2), ':');
			إذا (شاخص_نشانهها.اندازه() == 1) {
				صحيح a = atoi(شاخص_نشانهها.at(0).c_str());
				إذا (a < 0 || a >= wire->عرض)
					أرجع خطأ;
				sig.append(RTLIL::SigSpec(wire, a));
			} وإلا {
				صحيح a = atoi(شاخص_نشانهها.at(0).c_str());
				صحيح b = atoi(شاخص_نشانهها.at(1).c_str());
				إذا (a > b) {
					صحيح tmp = a;
					a = b, b = tmp;
				}
				إذا (a < 0 || a >= wire->عرض)
					أرجع خطأ;
				إذا (b < 0 || b >= wire->عرض)
					أرجع خطأ;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} وإلا
			sig.append(wire);
	}

	أرجع درست;
}

منطقي RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	إذا (str.خالی() || str[0] != '@')
		أرجع parse(sig, module, str);

	IdString sel_name = design->twines().پیدا_کن(RTLIL::escape_id(str.substr(1)));
	إذا (sel_name == IdString::Null || design->selection_vars.شمار(sel_name) == 0)
		أرجع خطأ;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	لكل (خودکار &it : module->wires_)
		إذا (sel.selected_member(module->name, it.دومی->name))
			sig.append(it.دومی);

	أرجع درست;
}

منطقي RTLIL::SigSpec::parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	إذا (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.اندازه());
		أرجع درست;
	}

	إذا (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.اندازه());
		أرجع درست;
	}

	إذا (lhs.is_chunk()) {
		حرف *p = (حرف*)str.c_str(), *endptr;
		دراز صحيح val = strtol(p, &endptr, 10);
		إذا (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.اندازه());
			أرجع درست;
		}
	}

	إذا (!parse(sig, module, str))
		أرجع خطأ;
	إذا (sig.اندازه() > lhs.اندازه())
		sig.remove(lhs.اندازه(), sig.اندازه() - lhs.اندازه());
	أرجع درست;
}

RTLIL::SigSpec::عملگر std::وکتور<RTLIL::SigChunk>() ثابت
{
	std::وکتور<RTLIL::SigChunk> نتیجه;
	لكل (ثابت RTLIL::SigChunk &c : chunks())
		نتیجه.افزودن_پشت(c);
	أرجع نتیجه;
}

RTLIL::Case_قاعده::~Case_قاعده()
{
	لكل (خودکار it = switches.begin(); it != switches.انتها(); it++)
		حذف *it;
}

منطقي RTLIL::Case_قاعده::خالی() ثابت
{
	أرجع actions.خالی() && switches.خالی();
}

RTLIL::Case_قاعده *RTLIL::Case_قاعده::clone() ثابت
{
	RTLIL::Case_قاعده *new_caserule = جديد RTLIL::Case_قاعده;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->صفات = صفات;
	لكل (خودکار &it : switches)
		new_caserule->switches.افزودن_پشت(it->clone());
	أرجع new_caserule;
}

RTLIL::Switch_قاعده::~Switch_قاعده()
{
	لكل (خودکار it = cases.begin(); it != cases.انتها(); it++)
		حذف *it;
}

منطقي RTLIL::Switch_قاعده::خالی() ثابت
{
	أرجع cases.خالی();
}

RTLIL::Switch_قاعده *RTLIL::Switch_قاعده::clone() ثابت
{
	RTLIL::Switch_قاعده *new_switchrule = جديد RTLIL::Switch_قاعده;
	new_switchrule->إشارة = إشارة;
	new_switchrule->صفات = صفات;
	لكل (خودکار &it : cases)
		new_switchrule->cases.افزودن_پشت(it->clone());
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
	لكل (خودکار it = syncs.begin(); it != syncs.انتها(); it++)
		حذف *it;
}

RTLIL::Process *RTLIL::Process::clone() ثابت
{
	RTLIL::Process *new_proc = جديد RTLIL::Process;

	new_proc->صفات = صفات;

	RTLIL::Case_قاعده *rc_ptr = ریشه_case.clone();
	new_proc->ریشه_case = *rc_ptr;
	rc_ptr->switches.clear();
	حذف rc_ptr;

	لكل (خودکار &it : syncs)
		new_proc->syncs.افزودن_پشت(it->clone());

	أرجع new_proc;
}

RTLIL::حافظه::~حافظه()
{
#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	RTLIL::حافظه::get_all_memorys()->erase(hashidx_);
#پایان_اگر
}

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
ساكن std::map<بیعلامت صحيح, RTLIL::حافظه*> all_memorys;
std::map<بیعلامت صحيح, RTLIL::حافظه*> *RTLIL::حافظه::get_all_memorys(فراغ)
{
	أرجع &all_memorys;
}
#پایان_اگر
YOSYS_NAMESPACE_انتها
