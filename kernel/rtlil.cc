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
		لكل (size_t i = 0; i < StaticCellTypes::builder.ژماردن; i++) {
			auto &cell = StaticCellTypes::builder.cells[i];
			إذا (cell.features.is_ff)
				r.زیادکردن(cell.type);
		}
		أرجع r;
	}();
	أرجع res;
}

#define check(condition) log_assert(condition && "malformed Const union")

ثابت Const::bitvectype& Const::get_bits() ثابت {
	check(is_bits());
	أرجع *get_if_bits();
}

ثابت std::string& Const::get_str() ثابت {
	check(is_str());
	أرجع *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	أرجع *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	أرجع *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	جديد ((فراغ*)&str_) std::string(std::move(str));
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	حرف bytes[] = {
		(حرف)(val >> 24), (حرف)(val >> 16), (حرف)(val >> 8), (حرف)val
	};
	جديد ((فراغ*)&str_) std::string(bytes, 4);
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val, صحيح width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	إذا ((width & 7) == 0) {
		جديد ((فراغ*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		صحيح bytes = width >> 3;
		signed حرف sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<صحيح>(bytes, حجم(val));
		لكل (صحيح i = 0; i < bytes; i++) {
			str[str.size() - 1 - i] = val;
			val = val >> 8;
		}
		أرجع;
	}

	جديد ((فراغ*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	لكل (صحيح i = 0; i < width; i++) {
		bv.پاڵنان_back((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, صحيح width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	جديد ((فراغ*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	لكل (صحيح i = 0; i < width; i++)
		bv.پاڵنان_back(bit);
}

RTLIL::Const::Const(ثابت std::vector<منطقي> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	جديد ((فراغ*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.size());
	لكل (ثابت auto &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ثابت RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	إذا (is_str())
		جديد ((فراغ*)&str_) std::string(other.get_str());
	وإلا إذا (is_bits())
		جديد ((فراغ*)&bits_) bitvectype(other.get_bits());
	وإلا
		check(خطأ);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	إذا (is_str())
		جديد ((فراغ*)&str_) std::string(std::move(other.get_str()));
	وإلا إذا (is_bits())
		جديد ((فراغ*)&bits_) bitvectype(std::move(other.get_bits()));
	وإلا
		check(خطأ);
}

RTLIL::Const &RTLIL::Const::operator =(ثابت RTLIL::Const &other) {
	flags = other.flags;
	إذا (other.is_str()) {
		إذا (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(فراغ)جديد ((فراغ*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} وإلا إذا (other.is_bits()) {
		إذا (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(فراغ)جديد ((فراغ*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} وإلا {
		check(خطأ);
	}
	أرجع *هذا;
}

RTLIL::Const::~Const() {
	إذا (is_bits())
		bits_.~bitvectype();
	وإلا إذا (is_str())
		str_.~string();
	وإلا
		check(خطأ);
}

منطقي RTLIL::Const::operator<(ثابت RTLIL::Const &other) ثابت
{
	إذا (size() != other.size())
		أرجع size() < other.size();

	لكل (صحيح i = 0; i < size(); i++)
		إذا ((*هذا)[i] != other[i])
			أرجع (*هذا)[i] < other[i];

	أرجع خطأ;
}

منطقي RTLIL::Const::operator ==(ثابت RTLIL::Const &other) ثابت
{
	إذا (is_str() && other.is_str())
		أرجع get_str() == other.get_str();
	إذا (is_bits() && other.is_bits())
		أرجع get_bits() == other.get_bits();

	إذا (size() != other.size())
		أرجع خطأ;

	لكل (صحيح i = 0; i < size(); i++)
	إذا ((*هذا)[i] != other[i])
		أرجع خطأ;

	أرجع true;
}

منطقي RTLIL::Const::operator !=(ثابت RTLIL::Const &other) ثابت
{
	أرجع !(*هذا == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	أرجع get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() ثابت
{
	std::vector<State> v;
	لكل (auto bit : *هذا)
		v.پاڵنان_back(bit);
	أرجع v;
}

منطقي RTLIL::Const::as_bool() ثابت
{
	إذا (is_str()) {
		لكل (حرف ch : get_str())
			إذا (ch != 0)
				أرجع true;
		أرجع خطأ;
	}

	ثابت bitvectype& bv = get_bits();
	لكل (size_t i = 0; i < bv.size(); i++)
		إذا (bv[i] == State::S1)
			أرجع true;
	أرجع خطأ;
}

صحيح RTLIL::Const::as_int(منطقي is_signed) ثابت
{
	int32_t ret = 0;
	إذا (ثابت std::string *s = get_if_str()) {
		صحيح size = GetSize(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		لكل (صحيح i = std::min(4, size); i > 0; i--)
			ret |= static_cast<unsigned حرف>((*s)[size - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		إذا (is_signed && size > 0 && size < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << size*8;
		أرجع ret;
	}

	ثابت bitvectype& bv = get_bits();
	صحيح significant_bits = std::min(GetSize(bv), 32);
	لكل (صحيح i = 0; i < significant_bits; i++)
		إذا (bv[i] == State::S1)
			ret |= 1 << i;
	إذا (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	أرجع ret;
}

منطقي RTLIL::Const::convertible_to_int(منطقي is_signed) ثابت
{
	auto size = get_min_size(is_signed);

	إذا (size < 0)
		أرجع خطأ;

	// If it fits in 31 bits it is definitely convertible
	إذا (size <= 31)
		أرجع true;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	إذا (size == 32) {
		إذا (is_signed)
			أرجع true;
		أرجع (*هذا)[size - 1] != State::S1;
	}

	أرجع خطأ;
}

std::optional<صحيح> RTLIL::Const::try_as_int(منطقي is_signed) ثابت
{
	إذا (!convertible_to_int(is_signed))
		أرجع std::nullopt;
	أرجع as_int(is_signed);
}

صحيح RTLIL::Const::as_int_saturating(منطقي is_signed) ثابت
{
	إذا (!convertible_to_int(is_signed)) {
		إذا (!is_signed)
			أرجع std::numeric_limits<صحيح>::max();

		ثابت auto min_size = get_min_size(is_signed);
		log_assert(min_size > 0);
		ثابت auto neg = (*هذا)[min_size - 1];
		أرجع neg ? std::numeric_limits<صحيح>::min() : std::numeric_limits<صحيح>::max();
	}
	أرجع as_int(is_signed);
}

فراغ RTLIL::Const::tag_bare_integer_const(ثابت std::string &بەها)
{
	إذا (بەها.empty() || بەها.find('\'') != std::string::npos)
		أرجع;
	size_t start = (بەها[0] == '-' || بەها[0] == '+') ? 1 : 0;
	إذا (start < بەها.size() && std::all_of(بەها.begin() + start, بەها.end(), ::isdigit))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

صحيح RTLIL::Const::get_min_size(منطقي is_signed) ثابت
{
	إذا (empty()) أرجع 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	إذا (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	وإلا
		leading_bit = RTLIL::State::S0;

	auto idx = size();
	طالما (idx > 0 && (*هذا)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	إذا (is_signed && idx < size()) {
		idx++;
	}
	// must be at least one bit
	أرجع (idx == 0) ? 1 : idx;
}

فراغ RTLIL::Const::compress(منطقي is_signed)
{
	auto idx = get_min_size(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<صحيح> RTLIL::Const::as_int_compress(منطقي is_signed) ثابت
{
	أرجع try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ثابت حرف* any) ثابت
{
	صحيح sz = size();
	std::string ret;
	ret.reserve(sz);
	لكل (صحيح i = sz - 1; i >= 0; --i)
		بدل ((*هذا)[i]) {
			حالة S0: ret.پاڵنان_back('0'); اكسر;
			حالة S1: ret.پاڵنان_back('1'); اكسر;
			حالة Sx: ret.پاڵنان_back('x'); اكسر;
			حالة Sz: ret.پاڵنان_back('z'); اكسر;
			حالة Sa: ret += any; اكسر;
			حالة Sm: ret.پاڵنان_back('m'); اكسر;
		}
	أرجع ret;
}

RTLIL::Const RTLIL::Const::from_string(ثابت std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.size());
	لكل (auto it = str.rbegin(); it != str.rend(); it++)
		بدل (*it) {
			حالة '0': bv.پاڵنان_back(State::S0); اكسر;
			حالة '1': bv.پاڵنان_back(State::S1); اكسر;
			حالة 'x': bv.پاڵنان_back(State::Sx); اكسر;
			حالة 'z': bv.پاڵنان_back(State::Sz); اكسر;
			حالة 'm': bv.پاڵنان_back(State::Sm); اكسر;
			افتراضي: bv.پاڵنان_back(State::Sa);
		}
	أرجع c;
}

std::string RTLIL::Const::decode_string() ثابت
{
	إذا (auto str = get_if_str())
		أرجع *str;

	ثابت bitvectype& bv = get_bits();
	ثابت صحيح n = GetSize(bv);
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

صحيح RTLIL::Const::size() ثابت {
	إذا (is_str())
		أرجع 8 * str_.size();
	وإلا {
		check(is_bits());
		أرجع bits_.size();
	}
}

منطقي RTLIL::Const::empty() ثابت {
	إذا (is_str())
		أرجع str_.empty();
	وإلا {
		check(is_bits());
		أرجع bits_.empty();
	}
}

فراغ RTLIL::Const::bitvectorize_internal() {
	إذا (tag == backing_tag::bits)
		أرجع;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.size() * 8);
	لكل (صحيح i = str_.size() - 1; i >= 0; i--) {
		unsigned حرف ch = str_[i];
		لكل (صحيح j = 0; j < 8; j++) {
			new_bits.پاڵنان_back((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(فراغ)جديد ((فراغ*)&bits_) bitvectype(std::move(new_bits));
		tag = backing_tag::bits;
	}
}

فراغ RTLIL::Const::append(ثابت RTLIL::Const &other) {
	bitvectorize_internal();
	bitvectype& bv = get_bits();
	bv.زیادکردن(bv.end(), other.begin(), other.end());
}

RTLIL::State RTLIL::Const::const_iterator::operator*() ثابت {
	إذا (auto bv = parent->get_if_bits())
		أرجع (*bv)[idx];

	صحيح char_idx = parent->get_str().size() - idx / 8 - 1;
	منطقي bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	أرجع bit ? State::S1 : State::S0;
}

منطقي RTLIL::Const::is_fully_zero() ثابت
{
	إذا (auto str = get_if_str()) {
		لكل (حرف ch : *str)
			إذا (ch != 0)
				أرجع خطأ;
		أرجع true;
	}

	ثابت bitvectype& bv = get_bits();

	لكل (ثابت auto &bit : bv)
		إذا (bit != RTLIL::State::S0)
			أرجع خطأ;

	أرجع true;
}

منطقي RTLIL::Const::is_fully_ones() ثابت
{
	إذا (auto str = get_if_str()) {
		لكل (حرف ch : *str)
			إذا (ch != (حرف)0xff)
				أرجع خطأ;
		أرجع true;
	}

	ثابت bitvectype& bv = get_bits();
	لكل (ثابت auto &bit : bv)
		إذا (bit != RTLIL::State::S1)
			أرجع خطأ;

	أرجع true;
}

منطقي RTLIL::Const::is_fully_def() ثابت
{
	إذا (is_str())
		أرجع true;

	ثابت bitvectype& bv = get_bits();
	لكل (ثابت auto &bit : bv)
		إذا (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			أرجع خطأ;

	أرجع true;
}

منطقي RTLIL::Const::is_fully_undef() ثابت
{
	إذا (auto str = get_if_str())
		أرجع str->empty();

	ثابت bitvectype& bv = get_bits();
	لكل (ثابت auto &bit : bv)
		إذا (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			أرجع خطأ;

	أرجع true;
}

منطقي RTLIL::Const::is_fully_undef_x_only() ثابت
{
	إذا (auto str = get_if_str())
		أرجع str->empty();

	ثابت bitvectype& bv = get_bits();
	لكل (ثابت auto &bit : bv)
		إذا (bit != RTLIL::State::Sx)
			أرجع خطأ;

	أرجع true;
}

منطقي RTLIL::Const::is_onehot(صحيح *pos) ثابت
{
	منطقي found = خطأ;
	صحيح size = GetSize(*هذا);
	لكل (صحيح i = 0; i < size; i++) {
		State bit = (*هذا)[i];
		إذا (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			أرجع خطأ;
		إذا (bit == RTLIL::State::S1) {
			إذا (found)
				أرجع خطأ;
			إذا (pos)
				*pos = i;
			found = true;
		}
	}
	أرجع found;
}

Hasher RTLIL::Const::hash_into(Hasher h) ثابت
{
	إذا (auto str = get_if_str())
		أرجع hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ثابت bitvectype &bv = get_bits();
	صحيح size = GetSize(bv);
	std::string packed;
	صحيح packed_size = (size + 7) >> 3;
	packed.resize(packed_size, 0);
	لكل (صحيح bi = 0; bi < packed_size; ++bi) {
		حرف ch = 0;
		صحيح end = std::min((bi + 1)*8, size);
		لكل (صحيح i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			إذا (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<حرف>::hash_into(ch, h);
				لكل (; i < size; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(size);
				أرجع h;
			}
			ch |= static_cast<صحيح>(b) << (i & 7);
		}
		packed[packed_size - 1 - bi] = ch;
	}
	أرجع hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(صحيح offset, صحيح len, RTLIL::State padding) ثابت {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	لكل (صحيح i = offset; i < offset + len; i++)
		ret_bv.پاڵنان_back(i < GetSize(*هذا) ? (*هذا)[i] : padding);
	أرجع RTLIL::Const(ret_bv);
}
#undef check /* check(condition) for Const */

منطقي RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ثابت
{
	أرجع attributes.ژماردن(id);
}

فراغ RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, منطقي بەها)
{
	إذا (بەها)
		attributes[id] = RTLIL::Const(1);
	وإلا
		attributes.erase(id);
}

منطقي RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ثابت
{
	ثابت auto it = attributes.find(id);
	إذا (it == attributes.end())
		أرجع خطأ;
	أرجع it->second.as_bool();
}

فراغ RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string بەها)
{
	إذا (بەها.empty())
		attributes.erase(id);
	وإلا
		attributes[id] = بەها;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ثابت
{
	std::string بەها;
	ثابت auto it = attributes.find(id);
	إذا (it != attributes.end())
		بەها = it->second.decode_string();
	أرجع بەها;
}

فراغ RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &data)
{
	string attrval;
	لكل (ثابت auto &s : data) {
		إذا (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

فراغ RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &data)
{
	pool<string> union_data = get_strpool_attribute(id);
	union_data.زیادکردن(data.begin(), data.end());
	إذا (!union_data.empty())
		set_strpool_attribute(id, union_data);
}

فراغ RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->attributes = std::move(buf);
}

نطاق {
	template<typename F>
	فراغ walk_process_attr_objects(RTLIL::Process *process, F visit) {
		visit(process);
		std::vector<RTLIL::CaseRule*> case_کەڵەکە{&process->ڕەگ_case};
		طالما (!case_کەڵەکە.empty()) {
			RTLIL::CaseRule *cs = case_کەڵەکە.back();
			case_کەڵەکە.دەرهێنان_back();
			visit(cs);
			لكل (auto *sw : cs->switches) {
				visit(sw);
				لكل (auto *case_ : sw->cases)
					case_کەڵەکە.پاڵنان_back(case_);
			}
		}
		لكل (auto *متزامن : process->syncs)
			لكل (auto &mwa : متزامن->mem_write_actions)
				visit(&mwa);
	}

	template<typename F>
	فراغ walk_attr_objects(RTLIL::Design *design, F visit) {
		لكل (auto &[_, module] : design->modules_) {
			visit(module);
			لكل (auto &[_, wire] : module->wires_)
				visit(wire);
			لكل (auto &[_, mem] : module->memories)
				visit(mem);
			لكل (auto &[_, cell] : module->cells_)
				visit(cell);
			لكل (auto &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


size_t RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	auto ڕەگ = [&](IdString ref) {
		إذا (ref != IdString::Null)
			live.زیادکردن(ref);
	};

	walk_attr_objects(هذا, [&](ثابت RTLIL::AttrObject *obj) {
		لكل (auto &attr : obj->attributes)
			ڕەگ(attr.first);
	});

	ڕەگ(selected_active_module);

	لكل (auto &[name, module] : modules_) {
		ڕەگ(name);
		لكل (IdString port : module->ports)
			ڕەگ(port);
		لكل (IdString param : module->avail_parameters)
			ڕەگ(param);
		لكل (auto &[param, _] : module->parameter_default_بەهاکان)
			ڕەگ(param);
		لكل (auto &[name, _] : module->memories)
			ڕەگ(name);
		لكل (auto &[name, _] : module->processes)
			ڕەگ(name);
		لكل (auto &[name, wire] : module->wires_) {
			ڕەگ(name);
			إذا (wire->known_driver())
				ڕەگ(wire->driverPort());
		}
		لكل (auto &[name, cell] : module->cells_) {
			ڕەگ(name);
			ڕەگ(cell->type);
			لكل (auto &conn : cell->connections())
				ڕەگ(conn.first);
			لكل (auto &param : cell->parameters)
				ڕەگ(param.first);
		}
		لكل (auto &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_کەڵەکە{&process->ڕەگ_case};
			طالما (!case_کەڵەکە.empty()) {
				RTLIL::CaseRule *cs = case_کەڵەکە.back();
				case_کەڵەکە.دەرهێنان_back();
				لكل (auto *sw : cs->switches) {
					لكل (auto *case_ : sw->cases)
						case_کەڵەکە.پاڵنان_back(case_);
				}
			}
			لكل (auto *متزامن : process->syncs) {
				لكل (auto &mwa : متزامن->mem_write_actions)
					ڕەگ(mwa.memid);
			}
		}
	}

	auto ڕەگ_selection = [&](ثابت RTLIL::Selection &sel) {
		لكل (IdString m : sel.selected_modules)
			ڕەگ(m);
		لكل (auto &[m, members] : sel.selected_members) {
			ڕەگ(m);
			لكل (IdString member : members)
				ڕەگ(member);
		}
	};

	لكل (auto &[name, sel] : selection_vars) {
		ڕەگ(name);
		ڕەگ_selection(sel);
	}

	لكل (auto &sel : selection_کەڵەکە)
		ڕەگ_selection(sel);

	size_t erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_ژماردن;
	أرجع erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ثابت
{
	pool<string> data;
	إذا (attributes.ژماردن(id) != 0)
		لكل (auto s : split_tokens(get_string_attribute(id), "|"))
			data.زیادکردن(s);
	أرجع data;
}

فراغ RTLIL::AttrObject::set_hdlname_attribute(ثابت vector<string> &hierarchy)
{
	string attrval;
	لكل (ثابت auto &ident : hierarchy) {
		إذا (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() ثابت
{
	أرجع split_tokens(get_string_attribute(ID::hdlname), " ");
}

فراغ RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, ثابت vector<صحيح> &data)
{
	std::stringstream attrval;
	لكل (auto &i : data) {
		إذا (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<صحيح> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ثابت
{
	vector<صحيح> data;
	auto it = attributes.find(id);
	إذا (it != attributes.end())
		لكل (ثابت auto &s : split_tokens(attributes.at(id).decode_string())) {
			حرف *end = nullptr;
			errno = 0;
			long بەها = strtol(s.c_str(), &end, 10);
			إذا (end != s.c_str() + s.size())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			إذا (errno == ERANGE || بەها < INT_MIN || بەها > INT_MAX)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			data.پاڵنان_back(بەها);
		}
	أرجع data;
}

منطقي RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ثابت
{
	إذا (current_design != nullptr) {
		auto module = current_design->module(mod_name);
		أرجع module && module->get_blackbox_attribute();
	} وإلا {
		log_warning("Unable to check if module is boxed for null design.\n");
		أرجع خطأ;
	}
}

منطقي RTLIL::Selection::selected_module(RTLIL::IdString mod_name) ثابت
{
	إذا (complete_selection)
		أرجع true;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (full_selection)
		أرجع true;
	إذا (selected_modules.ژماردن(mod_name) > 0)
		أرجع true;
	إذا (selected_members.ژماردن(mod_name) > 0)
		أرجع true;
	أرجع خطأ;
}

منطقي RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) ثابت
{
	إذا (complete_selection)
		أرجع true;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (full_selection)
		أرجع true;
	إذا (selected_modules.ژماردن(mod_name) > 0)
		أرجع true;
	أرجع خطأ;
}

منطقي RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	إذا (complete_selection)
		أرجع true;
	إذا (!selects_boxes && boxed_module(mod_name))
		أرجع خطأ;
	إذا (full_selection)
		أرجع true;
	إذا (selected_modules.ژماردن(mod_name) > 0)
		أرجع true;
	إذا (selected_members.ژماردن(mod_name) > 0)
		إذا (selected_members.at(mod_name).ژماردن(memb_name) > 0)
			أرجع true;
	أرجع خطأ;
}

فراغ RTLIL::Selection::optimize(RTLIL::Design *design)
{
	إذا (design != current_design) {
		current_design = design;
	}

	إذا (selects_boxes && full_selection)
		complete_selection = true;
	إذا (complete_selection) {
		full_selection = خطأ;
		selects_boxes = true;
	}
	إذا (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		أرجع;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.clear();
	لكل (auto mod_name : selected_modules) {
		إذا (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.پاڵنان_back(mod_name);
		selected_members.erase(mod_name);
	}
	لكل (auto mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	لكل (auto &it : selected_members)
		إذا (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.پاڵنان_back(it.first);
	لكل (auto mod_name : del_list)
		selected_members.erase(mod_name);

	لكل (auto &it : selected_members) {
		del_list.clear();
		لكل (auto memb_name : it.second)
			إذا (current_design->module(it.first)->ژماردن_id(memb_name) == 0)
				del_list.پاڵنان_back(memb_name);
		لكل (auto memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	add_list.clear();
	لكل (auto &it : selected_members)
		إذا (it.second.size() == 0)
			del_list.پاڵنان_back(it.first);
		وإلا إذا (it.second.size() == current_design->module(it.first)->wires_.size() + current_design->module(it.first)->memories.size() +
				current_design->module(it.first)->cells_.size() + current_design->module(it.first)->processes.size())
			add_list.پاڵنان_back(it.first);
	لكل (auto mod_name : del_list)
		selected_members.erase(mod_name);
	لكل (auto mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.زیادکردن(mod_name);
	}

	إذا (selected_modules.size() == current_design->modules_.size()) {
		selected_modules.clear();
		selected_members.clear();
		إذا (selects_boxes)
			complete_selection = true;
		وإلا
			full_selection = true;
	}
}

فراغ RTLIL::Selection::clear()
{
	full_selection = خطأ;
	complete_selection = خطأ;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_defines (جديد define_map_t)
{
	ساكن unsigned صحيح hashidx_ژماردن = 123456789;
	hashidx_ژماردن = mkhash_xorshift(hashidx_ژماردن);
	hashidx_ = hashidx_ژماردن;

	refcount_modules_ = 0;
	selected_active_module.clear();
	پاڵنان_full_selection();

	RTLIL::Design::get_all_designs()->زیادکردن(std::pair<unsigned صحيح, RTLIL::Design*>(hashidx_, هذا));
}

RTLIL::Design::~Design()
{
	لكل (auto &pr : modules_)
		delete pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

ساكن std::map<unsigned صحيح, RTLIL::Design*> all_designs;
std::map<unsigned صحيح, RTLIL::Design*> *RTLIL::Design::get_all_designs(فراغ)
{
	أرجع &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	أرجع RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ثابت RTLIL::Module *RTLIL::Design::module(IdString id) ثابت {
	twines_.check_owned(id);
	أرجع modules_.ژماردن(id) ? modules_.at(id) : عدم;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	أرجع modules_.ژماردن(id) ? modules_.at(id) : عدم;
}

RTLIL::Module *RTLIL::Design::لووتکە_module() ثابت
{
	RTLIL::Module *module = nullptr;
	صحيح module_ژماردن = 0;

	لكل (auto mod : selected_modules()) {
		إذا (mod->get_bool_attribute(ID::لووتکە))
			أرجع mod;
		module_ژماردن++;
		module = mod;
	}

	أرجع module_ژماردن == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	إذا (modules_.ژماردن(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = جديد RTLIL::Module;
	modules_[name] = module;
	module->design = هذا;
	module->name = name;

	لكل (auto mon : monitors)
		mon->notify_module_add(module);

	إذا (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	أرجع module;
}

فراغ RTLIL::Design::scratchpad_unset(ثابت std::string &varname)
{
	scratchpad.erase(varname);
}

فراغ RTLIL::Design::scratchpad_set_int(ثابت std::string &varname, صحيح بەها)
{
	scratchpad[varname] = stringf("%d", بەها);
}

فراغ RTLIL::Design::scratchpad_set_bool(ثابت std::string &varname, منطقي بەها)
{
	scratchpad[varname] = بەها ? "true" : "false";
}

فراغ RTLIL::Design::scratchpad_set_string(ثابت std::string &varname, std::string بەها)
{
	scratchpad[varname] = std::move(بەها);
}

صحيح RTLIL::Design::scratchpad_get_int(ثابت std::string &varname, صحيح default_بەها_2) ثابت
{
	auto it = scratchpad.find(varname);
	إذا (it == scratchpad.end())
		أرجع default_بەها_2;

	ثابت std::string &str = it->second;

	إذا (str == "0" || str == "false")
		أرجع 0;

	إذا (str == "1" || str == "true")
		أرجع 1;

	حرف *endptr = nullptr;
	long صحيح parsed_بەها = strtol(str.c_str(), &endptr, 10);
	أرجع *endptr ? default_بەها_2 : parsed_بەها;
}

منطقي RTLIL::Design::scratchpad_get_bool(ثابت std::string &varname, منطقي default_بەها_2) ثابت
{
	auto it = scratchpad.find(varname);
	إذا (it == scratchpad.end())
		أرجع default_بەها_2;

	ثابت std::string &str = it->second;

	إذا (str == "0" || str == "false")
		أرجع خطأ;

	إذا (str == "1" || str == "true")
		أرجع true;

	أرجع default_بەها_2;
}

std::string RTLIL::Design::scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_بەها_2) ثابت
{
	auto it = scratchpad.find(varname);
	إذا (it == scratchpad.end())
		أرجع default_بەها_2;

	أرجع it->second;
}

فراغ RTLIL::Design::remove(RTLIL::Module *module)
{
	لكل (auto mon : monitors)
		mon->notify_module_del(module);

	إذا (yosys_xtrace) {
		log("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	delete module;
}

فراغ RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	add(module);
}

فراغ RTLIL::Design::sort()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
	لكل (auto &it : modules_)
		it.second->sort();
}

فراغ RTLIL::Design::sort_modules()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
}

فراغ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

فراغ RTLIL::Design::check()
{
#ifndef NDEBUG
	log_assert(!selection_کەڵەکە.empty());
	صحيح pool_size = 0;
	لكل (auto &it : modules_)
		pool_size = std::max(pool_size, ThreadPool::work_pool_size(0, it.second->cells_size(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_size);
	لكل (auto &it : modules_) {
		log_assert(هذا == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#endif
}

فراغ RTLIL::Design::optimize()
{
	لكل (auto &it : modules_)
		it.second->optimize();
	لكل (auto &it : selection_کەڵەکە)
		it.optimize(هذا);
	لكل (auto &it : selection_vars)
		it.second.optimize(هذا);
}

فراغ RTLIL::Design::clone_into(RTLIL::Design *dst) ثابت
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	لكل (auto it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

منطقي RTLIL::Design::selected_module(IdString mod_name) ثابت
{
	إذا (!selected_active_module.empty() && mod_name != selected_active_module)
		أرجع خطأ;
	أرجع selection().selected_module(mod_name);
}

منطقي RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) ثابت
{
	إذا (!selected_active_module.empty() && mod_name != selected_active_module)
		أرجع خطأ;
	أرجع selection().selected_whole_module(mod_name);
}

منطقي RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	إذا (!selected_active_module.empty() && mod_name != selected_active_module)
		أرجع خطأ;
	أرجع selection().selected_member(mod_name, memb_name);
}

منطقي RTLIL::Design::selected_module(RTLIL::Module *mod) ثابت
{
	أرجع selected_module(mod->name);
}

منطقي RTLIL::Design::selected_whole_module(RTLIL::Module *mod) ثابت
{
	أرجع selected_whole_module(mod->name);
}

فراغ RTLIL::Design::پاڵنان_selection(RTLIL::Selection sel)
{
	sel.current_design = هذا;
	selection_کەڵەکە.پاڵنان_back(sel);
}

فراغ RTLIL::Design::پاڵنان_empty_selection()
{
	پاڵنان_selection(RTLIL::Selection::EmptySelection(هذا));
}

فراغ RTLIL::Design::پاڵنان_full_selection()
{
	پاڵنان_selection(RTLIL::Selection::FullSelection(هذا));
}

فراغ RTLIL::Design::پاڵنان_complete_selection()
{
	پاڵنان_selection(RTLIL::Selection::CompleteSelection(هذا));
}

فراغ RTLIL::Design::دەرهێنان_selection()
{
	selection_کەڵەکە.دەرهێنان_back();
	// Default to a full_selection if we ran out of stack
	إذا (selection_کەڵەکە.empty())
		پاڵنان_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(منطقي only_selected) ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(هذا), only_selected, true, خطأ, RTLIL_BACKEND::DumpMode::Readable);
	أرجع f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ثابت
{
	منطقي include_partials = partials == RTLIL::SELECT_ALL;
	منطقي exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	منطقي ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> result;
	result.reserve(modules_.size());
	لكل (auto &it : modules_) {
		إذا (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			إذا (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				result.پاڵنان_back(it.second);
			وإلا
				بدل (boxes)
				{
				حالة RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.second);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.second);
					اكسر;
				حالة RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.second);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.second);
					اكسر;
				حالة RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.second);
					اكسر;
				حالة RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.second);
					اكسر;
				افتراضي:
					اكسر;
				}
		} وإلا إذا (!include_partials && selected_module(it.first)) {
			بدل(partials)
			{
			حالة RTLIL::SELECT_WHOLE_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.second);
				اكسر;
			حالة RTLIL::SELECT_WHOLE_ERR:
				log_error("Unsupported partially selected module %s.\n", it.second);
				اكسر;
			حالة RTLIL::SELECT_WHOLE_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.second);
				اكسر;
			افتراضي:
				اكسر;
			}
		}
	}
	أرجع result;
}

RTLIL::Module::Module()
{
	ساكن unsigned صحيح hashidx_ژماردن = 123456789;
	hashidx_ژماردن = mkhash_xorshift(hashidx_ژماردن);
	hashidx_ = hashidx_ژماردن;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->زیادکردن(std::pair<unsigned صحيح, RTLIL::Module*>(hashidx_, هذا));
#endif
}

RTLIL::Module::~Module()
{
	لكل (auto &pr : wires_)
		delete pr.second;
	لكل (auto &pr : memories)
		delete pr.second;
	لكل (auto &pr : cells_)
		delete pr.second;
	لكل (auto &pr : processes)
		delete pr.second;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
ساكن std::map<unsigned صحيح, RTLIL::Module*> all_modules;
std::map<unsigned صحيح, RTLIL::Module*> *RTLIL::Module::get_all_modules(فراغ)
{
	أرجع &all_modules;
}
#endif

فراغ RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	لكل (auto it = wires_.begin(); it != wires_.end(); ++it)
		إذا (!it->second->port_تێخستن && !it->second->port_output)
			delwires.زیادکردن(it->second);

	لكل (auto it = memories.begin(); it != memories.end(); ++it)
		delete it->second;
	memories.clear();

	لكل (auto it = cells_.begin(); it != cells_.end(); ++it)
		delete it->second;
	cells_.clear();

	لكل (auto it = processes.begin(); it != processes.end(); ++it)
		delete it->second;
	processes.clear();

	connections_.clear();

	remove(delwires);
	set_bool_attribute(ID::blackbox);
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

size_t RTLIL::Module::ژماردن_id(RTLIL::IdString id)
{
	أرجع wires_.ژماردن(id) + cells_.ژماردن(id) + memories.ژماردن(id) + processes.ژماردن(id);
}

#ifndef NDEBUG
نطاق {
	بنية InternalCellChecker
	{
		ثابت RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		InternalCellChecker(ثابت RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		فراغ error(صحيح linenr, std::optional<std::string> error_پەیام = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			إذا (error_پەیام) {
				log_error("%s in internal cell %s\n", *error_پەیام, cell_loc);
			} وإلا {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		صحيح param(RTLIL::IdString name)
		{
			auto it = cell->parameters.find(name);
			إذا (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.زیادکردن(name);
			أرجع it->second.as_int();
		}

		صحيح param_bool(RTLIL::IdString name)
		{
			صحيح v = param(name);
			إذا (GetSize(cell->parameters.at(name)) > 32)
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

		فراغ param_bits(RTLIL::IdString name, صحيح width)
		{
			param(name);
			إذا (GetSize(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			أرجع cell->parameters.at(name).decode_string();
		}

		فراغ port(RTLIL::IdString name, صحيح width)
		{
			auto it = cell->connections_.find(name);
			إذا (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			إذا (GetSize(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.زیادکردن(name);
		}

		فراغ check_expected(منطقي check_matched_sign = خطأ)
		{
			لكل (auto &para : cell->parameters)
				إذا (expected_params.ژماردن(para.first) == 0)
					error(__LINE__);
			لكل (auto &conn : cell->connections())
				إذا (expected_ports.ژماردن(conn.first) == 0)
					error(__LINE__);

			إذا (check_matched_sign) {
				log_assert(expected_params.ژماردن(ID::A_SIGNED) != 0 && expected_params.ژماردن(ID::B_SIGNED) != 0);
				منطقي a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				منطقي b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				إذا (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		فراغ check()
		{
			إذا (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				أرجع;

			إذا (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(true);
				أرجع;
			}

			إذا (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/خطأ);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type.in(ID($shift), ID($shiftx))) {
				إذا (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/خطأ);
				} وإلا {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(true);
				أرجع;
			}

			إذا (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($pow));
				أرجع;
			}

			إذا (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::CI, 1);
				port(ID::BI, 1);
				port(ID::X, param(ID::Y_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				port(ID::CO, param(ID::Y_WIDTH));
				check_expected(true);
				أرجع;
			}

			إذا (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				أرجع;
			}

			إذا (cell->type == ID($macc_v2)) {
				إذا (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				إذا (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				إذا (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ثابت Const &a_width = cell->getParam(ID::A_WIDTHS);
				ثابت Const &b_width = cell->getParam(ID::B_WIDTHS);
				ثابت Const &c_width = cell->getParam(ID::C_WIDTHS);
				صحيح a_width_sum = 0, b_width_sum = 0, c_width_sum = 0;
				لكل (صحيح i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_sum += a_width.extract(16 * i, 16).as_int(خطأ);
					b_width_sum += b_width.extract(16 * i, 16).as_int(خطأ);
				}
				لكل (صحيح i = 0; i < param(ID::NADDENDS); i++) {
					c_width_sum += c_width.extract(16 * i, 16).as_int(خطأ);
				}
				port(ID::A, a_width_sum);
				port(ID::B, b_width_sum);
				port(ID::C, c_width_sum);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/خطأ);
				أرجع;
			}

			إذا (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				إذا (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dffsre)) {
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
				أرجع;
			}

			إذا (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_بەها, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_بەها, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type.in(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_بەها, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_بەها, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($aldffe)) {
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
				أرجع;
			}

			إذا (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_بەها, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_BITS);
				param(ID::STATE_ژمارە);
				param(ID::STATE_ژمارە_LOG2);
				param(ID::STATE_RST);
				param_bits(ID::STATE_TABLE, param(ID::STATE_BITS) * param(ID::STATE_ژمارە));
				param(ID::TRANS_ژمارە);
				param_bits(ID::TRANS_TABLE, param(ID::TRANS_ژمارە) * (2*param(ID::STATE_ژمارە_LOG2) + param(ID::CTRL_IN_WIDTH) + param(ID::CTRL_OUT_WIDTH)));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::CTRL_IN, param(ID::CTRL_IN_WIDTH));
				port(ID::CTRL_OUT, param(ID::CTRL_OUT_WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_MASK);
				param(ID::COLLISION_X_MASK);
				param_bool(ID::CE_OVER_SRST);
				param_bits(ID::ARST_بەها, param(ID::WIDTH));
				param_bits(ID::SRST_بەها, param(ID::WIDTH));
				param_bits(ID::INIT_بەها, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::SRST, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($memwr_v2)) {
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
				أرجع;
			}

			إذا (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::وشەکان));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::وشەکان));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($mem)) {
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
				أرجع;
			}

			إذا (cell->type == ID($mem_v2)) {
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
				param_bits(ID::RD_ARST_بەها, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_SRST_بەها, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_INIT_بەها, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
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
				أرجع;
			}

			إذا (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type.in(ID($أكد), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				أرجع;
			}

			إذا (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				أرجع;
			}

			إذا (cell->type.in(ID($specify2), ID($specify3))) {
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
				إذا (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
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
				أرجع;
			}

			إذا (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
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
				param(ID::PRIORITY);
				port(ID::A, 1);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
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

			إذا (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); أرجع; }
			إذا (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); أرجع; }

			إذا (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  أرجع; }

			إذا (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); أرجع; }

			إذا (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); أرجع; }

			إذا (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); أرجع; }

			إذا (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); أرجع; }

			إذا (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); أرجع; }

			إذا (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); أرجع; }

			إذا (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				أرجع;
			}
			إذا (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				أرجع;
			}
			إذا (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				أرجع;
			}
			إذا (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				أرجع;
			}
			إذا (cell->type.in(ID($تێخستن_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				أرجع;
			}
			إذا (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
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
#endif

فراغ RTLIL::Module::sort()
{
	wires_.sort(sort_by_id_str(design->twines()));
	cells_.sort(sort_by_id_str(design->twines()));
	parameter_default_بەهاکان.sort(sort_by_id_str(design->twines()));
	memories.sort(sort_by_id_str(design->twines()));
	processes.sort(sort_by_id_str(design->twines()));
	لكل (auto &it : cells_)
		it.second->sort();
	لكل (auto &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	لكل (auto &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#ifndef NDEBUG
ساكن فراغ check_id_کلیلەکان(ثابت dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	لكل (auto &it : keyed)
		log_assert(!it.first.empty());
}
#endif

فراغ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ifndef NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->cells_size(), 1000));
	ثابت RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	لكل (auto &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->width >= 0);
		log_assert(it.second->size >= 0);
		check_id_کلیلەکان(it.second->attributes);
		memory_strings.زیادکردن(it.second->name.str());
	}

	std::vector<MonotonicFlag> ports_declared(GetSize(module->ports));
	ShardedVector<std::string> memids(subpool);
	subpool.run([const_module, &ports_declared, &memory_strings, &memids](ثابت ParallelDispatchThreadPool::RunCtx &ctx) {
		لكل (صحيح i : ctx.item_range(const_module->cells_size())) {
			auto it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			لكل (auto &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_کلیلەکان(it.second->attributes);
			check_id_کلیلەکان(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			إذا (it.second->has_memid()) {
				log_assert(memory_strings.ژماردن(it.second->parameters.at(ID::MEMID).decode_string()));
			} وإلا إذا (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.ژماردن(memid));
				memids.زیادکردن(ctx, std::move(memid));
			}
			auto cell_mod = const_module->design->module(it.second->name);
			إذا (cell_mod != nullptr) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#ifdef __GNUC__
				#pragma GCC diagnostic پاڵنان
				#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				#endif
				log_assert(!it.second->get_blackbox_attribute());
				#ifdef __GNUC__
				#pragma GCC diagnostic دەرهێنان
				#endif
			}
		}

		لكل (صحيح i : ctx.item_range(const_module->wires_size())) {
			auto it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_کلیلەکان(it.second->attributes);
			إذا (it.second->port_id) {
				log_assert(GetSize(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_تێخستن || it.second->port_output);
				log_assert(it.second->port_id <= GetSize(ports_declared));
				منطقي previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == خطأ);
			} وإلا
				log_assert(!it.second->port_تێخستن && !it.second->port_output);
		}
	});
	لكل (ثابت MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == true);
	pool<std::string> memids_pool;
	لكل (std::string &memid : memids)
		log_assert(memids_pool.زیادکردن(memid).second);

	لكل (auto &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->ڕەگ_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->ڕەگ_case};
		لكل (size_t i = 0; i < all_cases.size(); i++) {
			لكل (auto &switch_it : all_cases[i]->switches) {
				لكل (auto &case_it : switch_it->cases) {
					لكل (auto &compare_it : case_it->compare) {
						log_assert(switch_it->إشارة.size() == compare_it.size());
					}
					all_cases.پاڵنان_back(case_it);
				}
			}
		}
		لكل (auto &sync_it : it.second->syncs) {
			بدل (sync_it->type) {
				حالة RTLIL::SyncType::ST0:
				حالة RTLIL::SyncType::ST1:
				حالة RTLIL::SyncType::STp:
				حالة RTLIL::SyncType::STn:
				حالة RTLIL::SyncType::STe:
					log_assert(!sync_it->إشارة.empty());
					اكسر;
				حالة RTLIL::SyncType::STa:
				حالة RTLIL::SyncType::STg:
				حالة RTLIL::SyncType::STi:
					log_assert(sync_it->إشارة.empty());
					اكسر;
			}
		}
	}

	لكل (auto &it : module->connections_) {
		log_assert(it.first.size() == it.second.size());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_کلیلەکان(module->attributes);
#endif
}

فراغ RTLIL::Module::check()
{
	صحيح pool_size = ThreadPool::work_pool_size(0, cells_size(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_size);
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
	new_mod->parameter_default_بەهاکان.clear();
	لكل (auto &it : parameter_default_بەهاکان)
		new_mod->parameter_default_بەهاکان[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	لكل (auto &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	لكل (auto it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	لكل (auto it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	لكل (auto it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	لكل (auto it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addProcess(dst_id, it->second);
	}

	بنية RewriteSigSpecWorker
	{
		ثابت dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ثابت RTLIL::Module *new_mod;
		فراغ operator()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([هذا](RTLIL::Wire *&wire) {
				auto it = wire_map.find(wire);
				إذا (it != wire_map.end())
					wire = it->second;
				وإلا
					log_assert(wire->module == new_mod);
			});
		}
	};

	RewriteSigSpecWorker rewriteSigSpecWorker{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewriteSigSpecWorker);
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
	dst->add(new_mod);
	أرجع new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) ثابت
{
	RTLIL::Module *new_mod = جديد RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	أرجع new_mod;
}

منطقي RTLIL::Module::has_memories() ثابت
{
	أرجع !memories.empty();
}

منطقي RTLIL::Module::has_processes() ثابت
{
	أرجع !processes.empty();
}

منطقي RTLIL::Module::has_memories_warn() ثابت
{
	إذا (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", هذا);
	أرجع !memories.empty();
}

منطقي RTLIL::Module::has_processes_warn() ثابت
{
	إذا (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", هذا);
	أرجع !processes.empty();
}

منطقي RTLIL::Module::is_selected() ثابت
{
	أرجع design->selected_module(هذا->name);
}

منطقي RTLIL::Module::is_selected_whole() ثابت
{
	أرجع design->selected_whole_module(هذا->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() ثابت
{
	std::vector<RTLIL::Wire*> result;
	result.reserve(wires_.size());
	لكل (auto &it : wires_)
		إذا (design->selected(هذا, it.second))
			result.پاڵنان_back(it.second);
	أرجع result;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() ثابت
{
	std::vector<RTLIL::Cell*> result;
	result.reserve(cells_.size());
	لكل (auto &it : cells_)
		إذا (design->selected(هذا, it.second))
			result.پاڵنان_back(it.second);
	أرجع result;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() ثابت
{
	std::vector<RTLIL::Memory*> result;
	result.reserve(memories.size());
	لكل (auto &it : memories)
		إذا (design->selected(هذا, it.second))
			result.پاڵنان_back(it.second);
	أرجع result;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() ثابت
{
	std::vector<RTLIL::Process*> result;
	result.reserve(processes.size());
	لكل (auto &it : processes)
		إذا (design->selected(هذا, it.second))
			result.پاڵنان_back(it.second);
	أرجع result;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() ثابت
{
	std::vector<RTLIL::SelectedMember> result;
	لكل (auto cell : selected_cells())
		result.پاڵنان_back({cell, cell->name});
	لكل (auto memory : selected_memories())
		result.پاڵنان_back({memory, memory->name});
	لكل (auto wire : selected_wires())
		result.پاڵنان_back({wire, wire->name});
	لكل (auto process : selected_processes())
		result.پاڵنان_back({process, process->name});
	أرجع result;
}

فراغ RTLIL::Module::add(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(ژماردن_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = هذا;
}

فراغ RTLIL::Module::add(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(ژماردن_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = هذا;
}

فراغ RTLIL::Module::add(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(ژماردن_id(process->name) == 0);
	processes[process->name] = process;
	process->module = هذا;
}

فراغ RTLIL::Module::remove(ثابت pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	إذا (wires.empty())
		أرجع;

	بنية DeleteWireWorker
	{
		RTLIL::Module *module;
		ثابت pool<RTLIL::Wire*> *wires_p;

		فراغ operator()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([هذا](RTLIL::Wire *&wire) {
				إذا (wires_p->ژماردن(wire))
					wire = module->addWire(stringf("$delete_wire$%d", (صحيح)autoidx++), wire->width);
			});
		}

		فراغ operator()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	DeleteWireWorker delete_wire_worker;
	delete_wire_worker.module = هذا;
	delete_wire_worker.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_worker);

	إذا (design->flagBufferedNormalized) {
		لكل (auto wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_index.erase(wire);
		}
	}

	لكل (auto &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.ژماردن(id) != 0);
		wires_.erase(id);
		delete it;
	}
}

فراغ RTLIL::Module::remove(RTLIL::Cell *cell)
{
	طالما (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.ژماردن(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	إذا (design && design->flagBufferedNormalized && buf_norm_cell_queue.ژماردن(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.زیادکردن(cell);
	} وإلا {
		delete cell;
	}
}

فراغ RTLIL::Module::remove(RTLIL::Memory *memory)
{
	log_assert(memories.ژماردن(memory->name) != 0);
	memories.erase(memory->name);
	delete memory;
}

فراغ RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.ژماردن(process->name) != 0);
	processes.erase(process->name);
	delete process;
}

فراغ RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	add(wire);
}

فراغ RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	add(cell);
}

فراغ RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	إذا (old_id != IdString::Null && wires_.ژماردن(old_id))
		rename(wires_.at(old_id), new_name);
	وإلا إذا (old_id != IdString::Null && cells_.ژماردن(old_id))
		rename(cells_.at(old_id), new_name);
	وإلا
		log_abort();
}

فراغ RTLIL::Module::swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2)
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

فراغ RTLIL::Module::swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	صحيح index = 0;
	أرجع uniquify(name, index);
}

IdString RTLIL::Module::uniquify(IdString name, صحيح &index)
{
	إذا (index == 0) {
		إذا (ژماردن_id(name) == 0)
			أرجع name;
		index++;
	}

	طالما (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", index)});
		إذا (ژماردن_id(new_name) == 0)
			أرجع new_name;
		index++;
	}
}

ساكن منطقي fixup_ports_compare(ثابت RTLIL::Wire *a, ثابت RTLIL::Wire *b)
{
	إذا (a->port_id && !b->port_id)
		أرجع true;
	إذا (!a->port_id && b->port_id)
		أرجع خطأ;

	إذا (a->port_id == b->port_id)
		أرجع a->name < b->name;
	أرجع a->port_id < b->port_id;
}

فراغ RTLIL::Module::connect(ثابت RTLIL::SigSig &conn)
{
	لكل (auto mon : monitors)
		mon->notify_connect(هذا, conn);

	إذا (design)
		لكل (auto mon : design->monitors)
			mon->notify_connect(هذا, conn);

	// ignore all attempts to assign constants to other constants
	إذا (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		لكل (صحيح i = 0; i < GetSize(conn.first); i++)
			إذا (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		إذا (GetSize(new_conn.first))
			connect(new_conn);
		أرجع;
	}

	إذا (yosys_xtrace) {
		log("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", هذا, log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(GetSize(conn.first) == GetSize(conn.second));
	connections_.پاڵنان_back(conn);
}

فراغ RTLIL::Module::connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

فراغ RTLIL::Module::new_connections(ثابت std::vector<RTLIL::SigSig> &new_conn)
{
	لكل (auto mon : monitors)
		mon->notify_connect(هذا, new_conn);

	إذا (design)
		لكل (auto mon : design->monitors)
			mon->notify_connect(هذا, new_conn);

	إذا (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", هذا);
		لكل (auto &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ثابت std::vector<RTLIL::SigSig> &RTLIL::Module::connections() ثابت
{
	أرجع connections_;
}

فراغ RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	لكل (auto &w : wires_)
		إذا (w.second->port_تێخستن || w.second->port_output)
			all_ports.پاڵنان_back(w.second);
		وإلا
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	إذا (design && design->flagBufferedNormalized) {
		لكل (auto &w : wires_)
			إذا (w.second->driverCell_ && w.second->driverCell_->type == ID($تێخستن_port))
				buf_norm_wire_queue.زیادکردن(w.second);

		buf_norm_wire_queue.زیادکردن(all_ports.begin(), all_ports.end());
	}

	ports.clear();
	لكل (size_t i = 0; i < all_ports.size(); i++) {
		ports.پاڵنان_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
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
	لكل (صحيح i = GetSize(src) - 1; i >= 0; i--) {
		auto &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::addWire(RTLIL::IdString name, صحيح width)
{
	log_assert(design);
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	RTLIL::Wire *wire = جديد RTLIL::Wire;
	wire->width = width;
	wire->name = name;
	add(wire);
	أرجع wire;
}

RTLIL::Wire *RTLIL::Module::addWire(IdString name, ثابت RTLIL::Wire *other)
{
	RTLIL::Wire *wire = addWire(name);
	wire->width = other->width;
	wire->start_offset = other->start_offset;
	wire->port_id = other->port_id;
	wire->port_تێخستن = other->port_تێخستن;
	wire->port_output = other->port_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ثابت RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
		RTLIL::copy_attr_dict(wire->attributes, other->attributes, src_design, هذا->design);
	}
	أرجع wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = جديد RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	أرجع cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, ثابت RTLIL::Cell *other)
{
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	منطقي cross_pool = src_design && هذا->design && src_design != هذا->design;

	IdString type = other->type.ref();
	إذا (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, هذا->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, هذا->design);

	إذا (!cross_pool)
		cell->connections_ = other->connections_;
	وإلا
		لكل (صحيح i = GetSize(other->connections_) - 1; i >= 0; i--) {
			auto &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	أرجع cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = جديد RTLIL::Memory;
	mem->module = هذا;
	mem->name = name;
	memories[name] = mem;
	أرجع mem;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name, ثابت RTLIL::Memory *other)
{
	log_assert(design);
	RTLIL::Memory *mem = جديد RTLIL::Memory;
	mem->module = هذا;
	mem->name = name;
	mem->width = other->width;
	mem->start_offset = other->start_offset;
	mem->size = other->size;
	RTLIL::copy_attr_dict(mem->attributes, other->attributes,
			other->module ? other->module->design : nullptr, design);
	memories[name] = mem;
	أرجع mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = جديد RTLIL::Process;
	proc->name = name;
	add(proc);
	أرجع proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, ثابت RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	إذا (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		لكل (auto *متزامن : proc->syncs)
			لكل (auto &mwa : متزامن->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	أرجع proc;
}

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		أرجع sig_y;                                       \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقي /* is_signed */, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::WIDTH] = sig_a.size();         \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		أرجع sig_y;                                       \
	}
DEF_METHOD(Buf,     sig_a.size(), ID($buf))
DEF_METHOD(Barrier, sig_a.size(), ID($barrier))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
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
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = خطأ;             \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_METHOD(Shl,      sig_a.size(), ID($shl))
DEF_METHOD(Shr,      sig_a.size(), ID($shr))
DEF_METHOD(Sshl,     sig_a.size(), ID($sshl))
DEF_METHOD(Sshr,     sig_a.size(), ID($sshr))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = خطأ;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		أرجع cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		أرجع sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.size(), ID($shiftx))
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		إذا (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.size();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		أرجع sig_y;                                             \
	}
DEF_METHOD(Mux,      ID($mux),        0)
DEF_METHOD(Bwmux,    ID($bwmux),      0)
DEF_METHOD(Pmux,     ID($pmux),       1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = _demux ? sig_a.size() : sig_y.size(); \
		cell->parameters[ID::S_WIDTH] = sig_s.size();             \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.size() << sig_s.size() : sig_a.size() >> sig_s.size()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		أرجع sig_y;                                             \
	}
DEF_METHOD(Bmux,     ID($bmux),       0)
DEF_METHOD(Demux,    ID($demux),      1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		أرجع cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		أرجع sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#undef DEF_METHOD

#define DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		أرجع sig2;                                      \
	}
#define DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت RTLIL::SigBit &sig3, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		أرجع sig3;                                      \
	}
#define DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت RTLIL::SigBit &sig3, ثابت RTLIL::SigBit &sig4, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت RTLIL::SigBit &sig3, ثابت std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		أرجع sig4;                                      \
	}
#define DEF_METHOD_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت RTLIL::SigBit &sig3, ثابت RTLIL::SigBit &sig4, ثابت RTLIL::SigBit &sig5, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->setPort("\\" #_P5, sig5);                   \
		cell->set_src_attribute(src);                     \
		أرجع cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت RTLIL::SigBit &sig3, ثابت RTLIL::SigBit &sig4, ثابت std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		أرجع sig5;                                           \
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

RTLIL::Cell* RTLIL::Module::addPow(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي a_signed, منطقي b_signed, ثابت std::string &src)
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
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addFa(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_x, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fa));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::C, sig_c);
	cell->setPort(ID::X, sig_x);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addSlice(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const offset, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($slice));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::Y_WIDTH] = sig_y.size();
	cell->parameters[ID::OFFSET] = offset;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addConcat(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($concat));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::B_WIDTH] = sig_b.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($أكد));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addSr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr, ثابت RTLIL::SigSpec &sig_q, منطقي set_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sr));
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addDff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addDffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, ثابت std::string &src)
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
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addDffsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي set_polarity, منطقي clr_polarity, ثابت std::string &src)
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
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addDffsre(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, منطقي set_polarity, منطقي clr_polarity, ثابت std::string &src)
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
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addAdff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_بەها, منطقي clk_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_بەها] = arst_بەها;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addAdffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_بەها, منطقي clk_polarity, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_بەها] = arst_بەها;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addAldff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي aload_polarity, ثابت std::string &src)
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
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addAldffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي en_polarity, منطقي aload_polarity, ثابت std::string &src)
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
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addSdff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_بەها, منطقي clk_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_بەها] = srst_بەها;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addSdffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_بەها, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_بەها] = srst_بەها;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addSdffce(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_بەها, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_بەها] = srst_بەها;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addDlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_بەها, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_بەها] = arst_بەها;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, منطقي set_polarity, منطقي clr_polarity, ثابت std::string &src)
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
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		ثابت RTLIL::SigSpec &sig_q, منطقي set_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addDffsrGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي set_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addDffsreGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, منطقي set_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addAdffGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_بەها, منطقي clk_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_بەها ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addAdffeGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_بەها, منطقي clk_polarity, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_بەها ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addAldffGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addAldffeGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقي clk_polarity, منطقي en_polarity, منطقي aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addSdffGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_بەها, منطقي clk_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_بەها ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addSdffeGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_بەها, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_بەها ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addSdffceGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_بەها, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_بەها ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatchGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_بەها, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_بەها ? '1' : '0'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsrGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, منطقي set_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, صحيح width, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, صحيح width, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, صحيح width, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, صحيح width, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::SetTag(RTLIL::IdString name, ثابت std::string &tag, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src)
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
	أرجع sig;
}

RTLIL::Cell* RTLIL::Module::addSetTag(RTLIL::IdString name, ثابت std::string &tag, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	Cell *cell = addCell(name, ID($set_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::GetTag(RTLIL::IdString name, ثابت std::string &tag, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($get_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	أرجع sig;
}

RTLIL::Cell* RTLIL::Module::addOverwriteTag(RTLIL::IdString name, ثابت std::string &tag, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($overwrite_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->set_src_attribute(src);
	أرجع cell;
}

RTLIL::SigSpec RTLIL::Module::OriginalTag(RTLIL::IdString name, ثابت std::string &tag, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($original_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	أرجع sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.size());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.size();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	أرجع sig;
}

std::string RTLIL::Module::to_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(هذا), design, خطأ, true, خطأ, RTLIL_BACKEND::DumpMode::Readable);
	أرجع f.str();
}

RTLIL::Wire::Wire()
{
	ساكن unsigned صحيح hashidx_ژماردن = 123456789;
	hashidx_ژماردن = mkhash_xorshift(hashidx_ژماردن);
	hashidx_ = hashidx_ژماردن;

	module = nullptr;
	width = 1;
	start_offset = 0;
	port_id = 0;
	port_تێخستن = خطأ;
	port_output = خطأ;
	upto = خطأ;
	is_signed = خطأ;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->زیادکردن(std::pair<unsigned صحيح, RTLIL::Wire*>(hashidx_, هذا));
#endif
}

RTLIL::Wire::~Wire()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#endif
}

std::string RTLIL::Wire::to_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", هذا, design(), RTLIL_BACKEND::DumpMode::Readable);
	أرجع f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
ساكن std::map<unsigned صحيح, RTLIL::Wire*> all_wires;
std::map<unsigned صحيح, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(فراغ)
{
	أرجع &all_wires;
}
#endif

RTLIL::Memory::Memory()
{
	ساكن unsigned صحيح hashidx_ژماردن = 123456789;
	hashidx_ژماردن = mkhash_xorshift(hashidx_ژماردن);
	hashidx_ = hashidx_ژماردن;

	width = 1;
	start_offset = 0;
	size = 0;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->زیادکردن(std::pair<unsigned صحيح, RTLIL::Memory*>(hashidx_, هذا));
#endif
}

std::string RTLIL::Memory::to_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", هذا, design(), RTLIL_BACKEND::DumpMode::Readable);
	أرجع f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	ساكن unsigned صحيح hashidx_ژماردن = 123456789;
	hashidx_ژماردن = mkhash_xorshift(hashidx_ژماردن);
	hashidx_ = hashidx_ژماردن;
}

std::string RTLIL::Process::to_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", هذا, design(), RTLIL_BACKEND::DumpMode::Readable);
	أرجع f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	ساكن unsigned صحيح hashidx_ژماردن = 123456789;
	hashidx_ژماردن = mkhash_xorshift(hashidx_ژماردن);
	hashidx_ = hashidx_ژماردن;

	// log("#memtrace# %p\n", this);
	memhasher();

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->زیادکردن(std::pair<unsigned صحيح, RTLIL::Cell*>(hashidx_, هذا));
#endif
}

RTLIL::Cell::~Cell()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#endif
}

std::string RTLIL::Cell::to_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", هذا, design(), RTLIL_BACKEND::DumpMode::Readable);
	أرجع f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
ساكن std::map<unsigned صحيح, RTLIL::Cell*> all_cells;
std::map<unsigned صحيح, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(فراغ)
{
	أرجع &all_cells;
}
#endif

منطقي RTLIL::Cell::hasPort(RTLIL::IdString portname) ثابت
{
	أرجع connections_.ژماردن(portname) != 0;
}

// bufnorm

ثابت RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) ثابت
{
	أرجع connections_.at(portname);
}

ثابت dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() ثابت
{
	أرجع connections_;
}

منطقي RTLIL::Cell::known() ثابت
{
	إذا (yosys_celltypes.cell_known(type))
		أرجع true;
	إذا (module && module->design && module->design->module(type))
		أرجع true;
	أرجع خطأ;
}

منطقي RTLIL::Cell::تێخستن(IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_known(type))
		أرجع yosys_celltypes.cell_تێخستن(type.ref(), portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		أرجع w && w->port_تێخستن;
	}
	أرجع خطأ;
}

منطقي RTLIL::Cell::output(RTLIL::IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_known(type))
		أرجع yosys_celltypes.cell_output(type, portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		أرجع w && w->port_output;
	}
	أرجع خطأ;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) ثابت
{
	إذا (yosys_celltypes.cell_known(type.ref()))
		أرجع yosys_celltypes.cell_port_dir(type, portname);
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		إذا (m == nullptr)
			أرجع PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		إذا (w == nullptr)
			أرجع PortDir::PD_UNKNOWN;
		أرجع PortDir(w->port_تێخستن + w->port_output * 2);
	}
	أرجع PortDir::PD_UNKNOWN;
}

منطقي RTLIL::Cell::hasParam(RTLIL::IdString paramname) ثابت
{
	أرجع parameters.ژماردن(paramname) != 0;
}

فراغ RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

فراغ RTLIL::Cell::setParam(RTLIL::IdString paramname, RTLIL::Const بەها)
{
	parameters[paramname] = std::move(بەها);
}

ثابت RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ثابت
{
	ثابت auto &it = parameters.find(paramname);
	إذا (it != parameters.end())
		أرجع it->second;
	إذا (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		إذا (m)
			أرجع m->parameter_default_بەهاکان.at(paramname);
	}
	ارم std::out_of_range("Cell::getParam()");
}

فراغ RTLIL::Cell::sort()
{
	connections_.sort(sort_by_id_str(module->twines()));
	parameters.sort(sort_by_id_str(module->twines()));
	attributes.sort(sort_by_id_str(module->twines()));
}

فراغ RTLIL::Cell::check()
{
#ifndef NDEBUG
	InternalCellChecker checker(عدم, هذا);
	checker.check();
#endif
}

فراغ RTLIL::Cell::fixup_parameters(منطقي set_a_signed, منطقي set_b_signed)
{
	إذا (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		أرجع;

	إذا (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		إذا (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		أرجع;
	}

	إذا (type == ID($demux)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		أرجع;
	}

	إذا (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		أرجع;
	}

	إذا (type == ID($fa)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		أرجع;
	}

	إذا (type == ID($lcu)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::CO]);
		أرجع;
	}

	إذا (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);
		أرجع;
	}

	منطقي signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	إذا (connections_.ژماردن(ID::A)) {
		إذا (signedness_ab) {
			إذا (set_a_signed)
				parameters[ID::A_SIGNED] = true;
			وإلا إذا (parameters.ژماردن(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = خطأ;
		}
		parameters[ID::A_WIDTH] = GetSize(connections_[ID::A]);
	}

	إذا (connections_.ژماردن(ID::B)) {
		إذا (signedness_ab) {
			إذا (set_b_signed)
				parameters[ID::B_SIGNED] = true;
			وإلا إذا (parameters.ژماردن(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = خطأ;
		}
		parameters[ID::B_WIDTH] = GetSize(connections_[ID::B]);
	}

	إذا (connections_.ژماردن(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);

	إذا (connections_.ژماردن(ID::Q))
		parameters[ID::WIDTH] = GetSize(connections_[ID::Q]);

	check();
}

منطقي RTLIL::Cell::has_keep_attr() ثابت {
	أرجع get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

منطقي RTLIL::Cell::has_memid() ثابت
{
	أرجع type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

منطقي RTLIL::Cell::is_mem_cell() ثابت
{
	أرجع type.in(ID($mem), ID($mem_v2)) || has_memid();
}

منطقي RTLIL::Cell::is_builtin_ff() ثابت {
	أرجع StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ثابت RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	إذا (wire == عدم)
		data = {bit.data};
	وإلا
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(صحيح offset, صحيح length) ثابت
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= width);
	RTLIL::SigChunk ret;
	إذا (wire) {
		ret.wire = wire;
		ret.offset = هذا->offset + offset;
		ret.width = length;
	} وإلا {
		لكل (صحيح i = 0; i < length; i++)
			ret.data.پاڵنان_back(data[offset+i]);
		ret.width = length;
	}
	أرجع ret;
}

RTLIL::SigBit RTLIL::SigChunk::operator[](صحيح offset) ثابت
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	إذا (wire) {
		ret.wire = wire;
		ret.offset = هذا->offset + offset;
	} وإلا {
		ret.data = data[offset];
	}
	أرجع ret;
}

منطقي RTLIL::SigChunk::operator <(ثابت RTLIL::SigChunk &other) ثابت
{
	إذا (wire && other.wire)
		إذا (wire->name != other.wire->name)
			أرجع wire->name < other.wire->name;

	إذا (wire != other.wire)
		أرجع wire < other.wire;

	إذا (offset != other.offset)
		أرجع offset < other.offset;

	إذا (width != other.width)
		أرجع width < other.width;

	أرجع data < other.data;
}

منطقي RTLIL::SigChunk::operator ==(ثابت RTLIL::SigChunk &other) ثابت
{
	أرجع wire == other.wire && width == other.width && offset == other.offset && data == other.data;
}

منطقي RTLIL::SigChunk::operator !=(ثابت RTLIL::SigChunk &other) ثابت
{
	إذا (*هذا == other)
		أرجع خطأ;
	أرجع true;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.size() > 0);
	auto ie = parts.begin();
	auto it = ie + parts.size() - 1;
	طالما (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::Const &بەها)
{
	إذا (GetSize(بەها) != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(بەها);
	} وإلا {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&بەها)
{
	إذا (GetSize(بەها) != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(بەها);
	} وإلا {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::SigChunk &chunk)
{
	إذا (chunk.width != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(chunk);
	} وإلا {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	إذا (chunk.width != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(chunk);
	} وإلا {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	إذا (wire->width != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(wire);
	} وإلا {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, صحيح offset, صحيح width)
{
	إذا (width != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} وإلا {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::string &str)
{
	إذا (str.size() != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(str);
	} وإلا {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(صحيح val, صحيح width)
{
	إذا (width != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(val, width);
	} وإلا
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, صحيح width)
{
	إذا (width != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(bit, width);
	} وإلا
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::SigBit &bit, صحيح width)
{
	إذا (width != 0) {
		إذا (bit.wire == عدم) {
			rep_ = CHUNK;
			جديد (&chunk_) RTLIL::SigChunk(bit.data, width);
		} وإلا إذا (width == 1) {
			rep_ = CHUNK;
			جديد (&chunk_) RTLIL::SigChunk(bit);
		} وإلا {
			init_empty_bits();
			bits_.reserve(width);
			لكل (صحيح i = 0; i < width; i++)
				bits_.پاڵنان_back(bit);
		}
	} وإلا
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	لكل (ثابت auto &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	لكل (ثابت auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	لكل (ثابت auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	لكل (ثابت auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(منطقي bit)
{
	rep_ = CHUNK;
	جديد (&chunk_) RTLIL::SigChunk(bit ? RTLIL::S1 : RTLIL::S0);
	check();
}

فراغ RTLIL::SigSpec::Chunks::const_iterator::next_chunk_bits()
{
	صحيح bits_size = GetSize(spec.bits_);
	إذا (bit_index >= bits_size)
		أرجع;
	صحيح i = bit_index;
	ثابت SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.data.clear();
	إذا (bit.is_wire()) {
		chunk.offset = bit.offset;
		طالما (i < bits_size && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_index)
			++i;
	} وإلا {
		chunk.offset = 0;
		chunk.data.پاڵنان_back(bit.data);
		طالما (i < bits_size && !spec.bits_[i].is_wire()) {
			chunk.data.پاڵنان_back(spec.bits_[i].data);
			++i;
		}
	}
	chunk.width = i - bit_index;
}

فراغ RTLIL::SigSpec::unpack()
{
	إذا (rep_ == BITS)
		أرجع;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	لكل (صحيح i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	جديد (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

فراغ RTLIL::SigSpec::try_repack()
{
	إذا (rep_ != BITS)
		أرجع;

	صحيح bits_size = GetSize(bits_);
	إذا (bits_size == 0)
		أرجع;
	إذا (bits_[0].is_wire()) {
		لكل (صحيح i = 1; i < bits_size; i++)
			إذا (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				أرجع;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_size);
		bits_.~vector();
		rep_ = CHUNK;
		جديد (&chunk_) SigChunk(std::move(chunk));
		أرجع;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_size);
	bits.پاڵنان_back(bits_[0].data);
	لكل (صحيح i = 1; i < bits_size; i++) {
		إذا (bits_[i].is_wire())
			أرجع;
		bits.پاڵنان_back(bits_[i].data);
	}
	bits_.~vector();
	rep_ = CHUNK;
	جديد (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ثابت
{
	Hasher h;
	لكل (auto &c : chunks())
		إذا (c.wire == عدم) {
			لكل (auto &v : c.data)
				h.eat(v);
		} وإلا {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t result = h.yield();
	إذا (result == 0)
		result = 1;
	hash_.set(result);
	أرجع result;
}

فراغ RTLIL::SigSpec::sort()
{
	unpack();
	std::sort(bits_.begin(), bits_.end());
	hash_.clear();
	try_repack();
}

فراغ RTLIL::SigSpec::sort_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::vector<SigBit> unique_bits = bits_;
	std::sort(unique_bits.begin(), unique_bits.end());
	auto last = std::unique(unique_bits.begin(), unique_bits.end());
	unique_bits.erase(last, unique_bits.end());

	*هذا = unique_bits;
	hash_.clear();
	try_repack();
}

فراغ RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &pattern, ثابت RTLIL::SigSpec &with)
{
	replace(pattern, with, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &pattern, ثابت RTLIL::SigSpec &with, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(size() == other->size());
	log_assert(pattern.size() == with.size());

	dict<RTLIL::SigBit, صحيح> pattern_to_with;
	صحيح pattern_size = pattern.size();
	لكل (صحيح i = 0; i < pattern_size; i++) {
		SigBit pattern_bit = pattern[i];
		إذا (pattern_bit.wire != عدم) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	صحيح this_size = size();
	منطقي other_modified = خطأ;
	لكل (صحيح j = 0; j < this_size; j++) {
		auto it = pattern_to_with.find((*هذا)[j]);
		إذا (it != pattern_to_with.end()) {
			other_modified = true;
			(*other)[j] = with[it->second];
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::replace(ثابت dict<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(size() == other->size());

	إذا (rules.empty()) أرجع;

	صحيح this_size = size();
	منطقي other_modified = خطأ;
	لكل (صحيح i = 0; i < this_size; i++) {
		auto it = rules.find((*هذا)[i]);
		إذا (it != rules.end()) {
			other_modified = true;
			(*other)[i] = it->second;
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::replace(ثابت std::map<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, هذا);
}

فراغ RTLIL::SigSpec::replace(ثابت std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != عدم);
	log_assert(size() == other->size());

	إذا (rules.empty()) أرجع;

	صحيح this_size = size();
	منطقي other_modified = خطأ;
	لكل (صحيح i = 0; i < this_size; i++) {
		auto it = rules.find((*هذا)[i]);
		إذا (it != rules.end()) {
			other_modified = true;
			(*other)[i] = it->second;
		}
	}

	إذا (other_modified)
		other->try_repack();
	other->check();
}

فراغ RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &pattern)
{
	remove2(pattern, عدم);
}

فراغ RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *هذا;
	tmp.remove2(pattern, other);
}

فراغ RTLIL::SigSpec::remove2(ثابت RTLIL::SigSpec &pattern, RTLIL::SigSpec *other)
{
	unpack();
	إذا (other != عدم) {
		log_assert(size() == other->size());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.size());
	لكل (auto &bit : pattern)
		إذا (bit.wire != عدم)
			pattern_bits.زیادکردن(bit);

	// Compact in-place to avoid O(n^2) erase operations
	size_t write_idx = 0;
	لكل (size_t read_idx = 0; read_idx < bits_.size(); read_idx++)
	{
		إذا (!(bits_[read_idx].wire != عدم && pattern_bits.ژماردن(bits_[read_idx]))) {
			إذا (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				إذا (other != عدم)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < bits_.size());
	إذا (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove(ثابت pool<RTLIL::SigBit> &pattern)
{
	remove2(pattern, عدم);
}

فراغ RTLIL::SigSpec::remove(ثابت pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *هذا;
	tmp.remove2(pattern, other);
}

فراغ RTLIL::SigSpec::remove2(ثابت pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	لكل (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		إذا (!(bits_[read_idx].wire != عدم && pattern.ژماردن(bits_[read_idx]))) {
			إذا (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				إذا (other != عدم)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < bits_.size());
	إذا (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove2(ثابت std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	لكل (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		إذا (!(bits_[read_idx].wire != عدم && pattern.ژماردن(bits_[read_idx]))) {
			إذا (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				إذا (other != عدم)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < bits_.size());
	إذا (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	إذا (other != عدم && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove2(ثابت pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	إذا (other != عدم) {
		log_assert(size() == other->size());
		other->unpack();
	}

	منطقي modified = خطأ;
	منطقي other_modified = خطأ;
	لكل (صحيح i = GetSize(bits_) - 1; i >= 0; i--) {
		إذا (bits_[i].wire != عدم && pattern.ژماردن(bits_[i].wire)) {
			modified = true;
			bits_.erase(bits_.begin() + i);
			إذا (other != عدم) {
				other_modified = true;
				other->bits_.erase(other->bits_.begin() + i);
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

RTLIL::SigSpec RTLIL::SigSpec::extract(ثابت RTLIL::SigSpec &pattern, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == عدم || size() == other->size());

	RTLIL::SigSpec ret;
	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();

	لكل (auto& pattern_chunk : pattern.chunks()) {
		إذا (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			صحيح i = 0;
			لكل (ثابت RTLIL::SigBit &bit : bits_match) {
				إذا (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} وإلا {
			لكل (ثابت RTLIL::SigBit &bit : bits_match)
				إذا (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	أرجع ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ثابت pool<RTLIL::SigBit> &pattern, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == عدم || size() == other->size());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	إذا (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		صحيح i = 0;
		لكل (ثابت RTLIL::SigBit &bit : bits_match) {
			إذا (bit.wire && pattern.ژماردن(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} وإلا {
		لكل (ثابت RTLIL::SigBit &bit : bits_match)
			إذا (bit.wire && pattern.ژماردن(bit))
				ret.append(bit);
	}

	ret.check();
	أرجع ret;
}

فراغ RTLIL::SigSpec::replace(صحيح offset, ثابت RTLIL::SigSpec &with)
{
	إذا (with.size() == 0)
		أرجع;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.size() >= 0);
	log_assert(offset+with.size() <= size());

	صحيح i = 0;
	لكل (ثابت RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
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
			init_empty_bits();
			hash_.clear();
		}
	}
	وإلا
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.size());
		لكل (auto &bit : bits_)
			إذا (bit.wire != عدم)
				new_bits.پاڵنان_back(bit);
		إذا (GetSize(new_bits) != GetSize(bits_)) {
			bits_.swap(new_bits);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

فراغ RTLIL::SigSpec::remove(صحيح offset, صحيح length)
{
	إذا (length == 0)
		أرجع;

	unpack();

	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	bits_.erase(bits_.begin() + offset, bits_.begin() + offset + length);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(صحيح offset, صحيح length) ثابت
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	std::vector<SigBit> extracted;
	SigBit first;
	منطقي is_packing = true;
	لكل (صحيح i = offset; i < offset + length; i++) {
		منطقي was_packing_before = is_packing;
		SigBit bit = (*هذا)[i];
		إذا (i == offset) {
			first = bit;
			إذا (!bit.wire)
				is_packing = خطأ;
		} وإلا {
			إذا (bit.wire != first.wire)
				is_packing = خطأ;
			إذا (bit.wire)
				إذا (bit.offset != first.offset + (i - offset))
					is_packing = خطأ;
		}
		إذا (was_packing_before && !is_packing)
			لكل (صحيح j = offset; j < i; j++)
				extracted.پاڵنان_back((*هذا)[j]);
		إذا (!is_packing)
			extracted.پاڵنان_back((*هذا)[i]);
	}
	إذا (is_packing)
		أرجع SigChunk(first.wire, first.offset, length);

	أرجع extracted;
}

فراغ RTLIL::SigSpec::rewrite_wires(std::function<فراغ(RTLIL::Wire*& wire)> rewrite)
{
	إذا (rep_ == CHUNK) {
		إذا (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		أرجع;
	}

	std::vector<RTLIL::SigBit> new_bits;
	لكل (ثابت RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		إذا (c.wire != nullptr)
			rewrite(c.wire);
		لكل (صحيح i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.clear();
}

فراغ RTLIL::SigSpec::append(ثابت RTLIL::SigSpec &إشارة)
{
	إذا (إشارة.size() == 0)
		أرجع;

	إذا (size() == 0) {
		*هذا = إشارة;
		أرجع;
	}

	hash_.clear();
	إذا (rep_ == CHUNK && إشارة.rep_ == CHUNK && chunk_.wire == إشارة.chunk_.wire) {
		إذا (chunk_.wire == عدم) {
			chunk_.data.زیادکردن(chunk_.data.end(), إشارة.chunk_.data.begin(), إشارة.chunk_.data.end());
			chunk_.width = GetSize(chunk_.data);
			أرجع;
		}
		إذا (chunk_.offset + chunk_.width == إشارة.chunk_.offset) {
			chunk_.width += إشارة.chunk_.width;
			أرجع;
		}
	}

	unpack();
	لكل (ثابت SigBit &bit : إشارة.bits())
		bits_.پاڵنان_back(bit);
	check();
}

فراغ RTLIL::SigSpec::append(ثابت RTLIL::SigBit &bit)
{
	hash_.clear();

	إذا (size() == 0) {
		destroy();
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(bit);
		أرجع;
	}

	إذا (rep_ == CHUNK && chunk_.wire == bit.wire) {
		إذا (chunk_.wire == عدم) {
			chunk_.data.پاڵنان_back(bit.data);
			chunk_.width++;
			أرجع;
		}
		إذا (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			أرجع;
		}
	}

	unpack();

	bits_.پاڵنان_back(bit);
	check();
}

فراغ RTLIL::SigSpec::extend_u0(صحيح width, منطقي is_signed)
{
	إذا (size() > width)
		remove(width, size() - width);

	إذا (size() < width) {
		RTLIL::SigBit padding = size() > 0 ? (*هذا)[size() - 1] : RTLIL::State::Sx;
		إذا (!is_signed)
			padding = RTLIL::State::S0;
		طالما (size() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(صحيح ژمارە) ثابت
{
	RTLIL::SigSpec sig;
	لكل (صحيح i = 0; i < ژمارە; i++)
		sig.append(*هذا);
	أرجع sig;
}

#ifndef NDEBUG
فراغ RTLIL::SigSpec::check(ثابت Module *mod) ثابت
{
	إذا (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		إذا (chunk_.wire == عدم) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.data.size() == (size_t)chunk_.width);
		} وإلا {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.data.size() == 0);
			إذا (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	وإلا إذا (size() <= 64)
	{
		إذا (mod != nullptr) {
			لكل (ثابت RTLIL::SigBit &bit : bits_)
				إذا (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#endif

منطقي RTLIL::SigSpec::operator <(ثابت RTLIL::SigSpec &other) ثابت
{
	إذا (هذا == &other)
		أرجع خطأ;

	إذا (size() != other.size())
		أرجع size() < other.size();

	auto other_it = other.chunks().begin();
	لكل (ثابت SigChunk &c : chunks()) {
		إذا (c != *other_it)
			أرجع c < *other_it;
		++other_it;
	}

	أرجع خطأ;
}

منطقي RTLIL::SigSpec::operator ==(ثابت RTLIL::SigSpec &other) ثابت
{
	إذا (هذا == &other)
		أرجع true;

	إذا (size() != other.size())
		أرجع خطأ;

	auto other_it = other.chunks().begin();
	لكل (ثابت SigChunk &c : chunks()) {
		إذا (c != *other_it)
			أرجع خطأ;
		++other_it;
	}

	أرجع true;
}

منطقي RTLIL::SigSpec::is_wire() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	إذا (it == cs.end())
		أرجع خطأ;
	ثابت RTLIL::SigChunk &chunk = *it;
	أرجع chunk.wire && chunk.wire->width == size() && ++it == cs.end();
}

منطقي RTLIL::SigSpec::is_chunk() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	إذا (it == cs.end())
		أرجع خطأ;
	أرجع ++it == cs.end();
}

منطقي RTLIL::SigSpec::known_driver() ثابت
{
	لكل (auto &chunk : chunks())
		إذا (chunk.is_wire() && !chunk.wire->known_driver())
			أرجع خطأ;
	أرجع true;
}

منطقي RTLIL::SigSpec::is_fully_const() ثابت
{
	لكل (auto &chunk : chunks())
		إذا (chunk.width > 0 && chunk.wire != عدم)
			أرجع خطأ;
	أرجع true;
}

منطقي RTLIL::SigSpec::is_fully_zero() ثابت
{
	لكل (auto &chunk : chunks()) {
		إذا (chunk.width > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.data)
			إذا (d != RTLIL::State::S0)
				أرجع خطأ;
	}
	أرجع true;
}

منطقي RTLIL::SigSpec::is_fully_ones() ثابت
{
	لكل (auto &chunk : chunks()) {
		إذا (chunk.width > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.data)
			إذا (d != RTLIL::State::S1)
				أرجع خطأ;
	}
	أرجع true;
}

منطقي RTLIL::SigSpec::is_fully_def() ثابت
{
	لكل (auto &chunk : chunks()) {
		إذا (chunk.width > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.data)
			إذا (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				أرجع خطأ;
	}
	أرجع true;
}

منطقي RTLIL::SigSpec::is_fully_undef() ثابت
{
	لكل (auto &chunk : chunks()) {
		إذا (chunk.width > 0 && chunk.wire != عدم)
			أرجع خطأ;
		لكل (RTLIL::State d : chunk.data)
			إذا (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				أرجع خطأ;
	}
	أرجع true;
}

منطقي RTLIL::SigSpec::has_const() ثابت
{
	لكل (auto &chunk : chunks())
		إذا (chunk.width > 0 && chunk.wire == عدم)
			أرجع true;
	أرجع خطأ;
}

منطقي RTLIL::SigSpec::has_const(State state) ثابت
{
	لكل (auto &chunk : chunks())
		إذا (chunk.width > 0 && chunk.wire == عدم && std::find(chunk.data.begin(), chunk.data.end(), state) != chunk.data.end())
			أرجع true;
	أرجع خطأ;
}


منطقي RTLIL::SigSpec::has_marked_bits() ثابت
{
	لكل (auto &chunk : chunks())
		إذا (chunk.width > 0 && chunk.wire == عدم) {
			لكل (RTLIL::State d : chunk.data)
				إذا (d == RTLIL::State::Sm)
					أرجع true;
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
	log_assert(c.has_بەها());
	أرجع c->as_bool();
}

صحيح RTLIL::SigSpec::as_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_بەها());
	أرجع c->as_int(is_signed);
}

منطقي RTLIL::SigSpec::convertible_to_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	إذا (!c.has_بەها())
		أرجع خطأ;
	أرجع c->convertible_to_int(is_signed);
}

std::optional<صحيح> RTLIL::SigSpec::try_as_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	إذا (!c.has_بەها())
		أرجع std::nullopt;
	أرجع c->try_as_int(is_signed);
}

صحيح RTLIL::SigSpec::as_int_saturating(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_بەها());
	أرجع c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ثابت
{
	std::string str;
	str.reserve(size());
	std::vector<RTLIL::SigChunk> chunks = *هذا;
	لكل (size_t i = chunks.size(); i > 0; i--) {
		ثابت RTLIL::SigChunk &chunk = chunks[i-1];
		إذا (chunk.wire != عدم)
			str.append(chunk.width, '?');
		وإلا
			str += RTLIL::Const(chunk.data).as_string();
	}
	أرجع str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	إذا (it == cs.end())
		أرجع RTLIL::Const();
	SigChunk chunk = *it;
	إذا (chunk.wire != عدم || ++it != cs.end())
		أرجع std::nullopt;
	أرجع RTLIL::Const(std::move(chunk.data));
}

RTLIL::Const RTLIL::SigSpec::as_const() ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_بەها());
	أرجع *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == size());
	أرجع chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	أرجع chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() ثابت
{
	أرجع RTLIL::SigBit(*هذا);
}

منطقي RTLIL::SigSpec::match(ثابت حرف* pattern) ثابت
{
	صحيح pattern_len = strlen(pattern);
	log_assert(pattern_len == size());

	لكل (صحيح i = 0; i < pattern_len; ++i) {
		حرف ch = pattern[i];
		إذا (ch == ' ')
			استمر;
		RTLIL::SigBit bit = (*هذا)[pattern_len - 1 - i];
		إذا (ch == '*') {
			إذا (bit != State::Sz && bit != State::Sx)
				أرجع خطأ;
			استمر;
		}
		إذا (ch == '0') {
			إذا (bit != State::S0)
				أرجع خطأ;
		} وإلا
		إذا (ch == '1') {
			إذا (bit != State::S1)
				أرجع خطأ;
		} وإلا
			log_abort();
	}

	أرجع true;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() ثابت
{
	std::set<RTLIL::SigBit> sigbits;
	لكل (auto &c : chunks())
		لكل (صحيح i = 0; i < c.width; i++)
			sigbits.زیادکردن(RTLIL::SigBit(c, i));
	أرجع sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() ثابت
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(size());
	لكل (auto &c : chunks())
		لكل (صحيح i = 0; i < c.width; i++)
			sigbits.زیادکردن(RTLIL::SigBit(c, i));
	أرجع sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() ثابت
{
	std::vector<RTLIL::SigBit> result;
	result.reserve(size());
	لكل (SigBit bit : *هذا)
		result.پاڵنان_back(bit);
	أرجع result;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت
{
	صحيح this_size = size();
	log_assert(this_size == other.size());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	لكل (صحيح i = 0; i < this_size; i++)
		new_map[(*هذا)[i]] = other[i];

	أرجع new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت
{
	صحيح this_size = size();
	log_assert(this_size == other.size());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_size);
	لكل (صحيح i = 0; i < this_size; i++)
		new_map[(*هذا)[i]] = other[i];

	أرجع new_map;
}

ساكن فراغ sigspec_parse_split(std::vector<std::string> &tokens, ثابت std::string &text, حرف sep)
{
	size_t start = 0, end = 0;
	طالما ((end = text.find(sep, start)) != std::string::npos) {
		tokens.پاڵنان_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.پاڵنان_back(text.substr(start));
}

ساكن RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ثابت std::string &netname)
{
	أرجع module->wire(netname);
}

منطقي RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	لكل (صحيح tokidx = صحيح(tokens.size())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		إذا (netname.size() == 0)
			استمر;

		إذا (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			auto ast = p.const2ast(netname);
			إذا (ast == nullptr)
				أرجع خطأ;
			sig.append(RTLIL::Const(ast->bits));
			استمر;
		}

		إذا (module == عدم)
			أرجع خطأ;

		إذا (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		إذا (sigspec_parse_wire(module, netname) == nullptr) {
			size_t indices_pos = netname.size()-1;
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
		إذا (wire == nullptr)
			أرجع خطأ;

		إذا (!indices.empty()) {
			std::vector<std::string> index_tokens;
			sigspec_parse_split(index_tokens, indices.substr(1, indices.size()-2), ':');
			إذا (index_tokens.size() == 1) {
				صحيح a = atoi(index_tokens.at(0).c_str());
				إذا (a < 0 || a >= wire->width)
					أرجع خطأ;
				sig.append(RTLIL::SigSpec(wire, a));
			} وإلا {
				صحيح a = atoi(index_tokens.at(0).c_str());
				صحيح b = atoi(index_tokens.at(1).c_str());
				إذا (a > b) {
					صحيح tmp = a;
					a = b, b = tmp;
				}
				إذا (a < 0 || a >= wire->width)
					أرجع خطأ;
				إذا (b < 0 || b >= wire->width)
					أرجع خطأ;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} وإلا
			sig.append(wire);
	}

	أرجع true;
}

منطقي RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	إذا (str.empty() || str[0] != '@')
		أرجع parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	إذا (sel_name == IdString::Null || design->selection_vars.ژماردن(sel_name) == 0)
		أرجع خطأ;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	لكل (auto &it : module->wires_)
		إذا (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	أرجع true;
}

منطقي RTLIL::SigSpec::parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	إذا (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.size());
		أرجع true;
	}

	إذا (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.size());
		أرجع true;
	}

	إذا (lhs.is_chunk()) {
		حرف *p = (حرف*)str.c_str(), *endptr;
		long صحيح val = strtol(p, &endptr, 10);
		إذا (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.size());
			أرجع true;
		}
	}

	إذا (!parse(sig, module, str))
		أرجع خطأ;
	إذا (sig.size() > lhs.size())
		sig.remove(lhs.size(), sig.size() - lhs.size());
	أرجع true;
}

RTLIL::SigSpec::operator std::vector<RTLIL::SigChunk>() ثابت
{
	std::vector<RTLIL::SigChunk> result;
	لكل (ثابت RTLIL::SigChunk &c : chunks())
		result.پاڵنان_back(c);
	أرجع result;
}

RTLIL::CaseRule::~CaseRule()
{
	لكل (auto it = switches.begin(); it != switches.end(); it++)
		delete *it;
}

منطقي RTLIL::CaseRule::empty() ثابت
{
	أرجع actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() ثابت
{
	RTLIL::CaseRule *new_caserule = جديد RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	لكل (auto &it : switches)
		new_caserule->switches.پاڵنان_back(it->clone());
	أرجع new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	لكل (auto it = cases.begin(); it != cases.end(); it++)
		delete *it;
}

منطقي RTLIL::SwitchRule::empty() ثابت
{
	أرجع cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() ثابت
{
	RTLIL::SwitchRule *new_switchrule = جديد RTLIL::SwitchRule;
	new_switchrule->إشارة = إشارة;
	new_switchrule->attributes = attributes;
	لكل (auto &it : cases)
		new_switchrule->cases.پاڵنان_back(it->clone());
	أرجع new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() ثابت
{
	RTLIL::SyncRule *new_syncrule = جديد RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->إشارة = إشارة;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	أرجع new_syncrule;
}

RTLIL::Process::~Process()
{
	لكل (auto it = syncs.begin(); it != syncs.end(); it++)
		delete *it;
}

RTLIL::Process *RTLIL::Process::clone() ثابت
{
	RTLIL::Process *new_proc = جديد RTLIL::Process;

	new_proc->attributes = attributes;

	RTLIL::CaseRule *rc_ptr = ڕەگ_case.clone();
	new_proc->ڕەگ_case = *rc_ptr;
	rc_ptr->switches.clear();
	delete rc_ptr;

	لكل (auto &it : syncs)
		new_proc->syncs.پاڵنان_back(it->clone());

	أرجع new_proc;
}

RTLIL::Memory::~Memory()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
ساكن std::map<unsigned صحيح, RTLIL::Memory*> all_memorys;
std::map<unsigned صحيح, RTLIL::Memory*> *RTLIL::Memory::get_all_memorys(فراغ)
{
	أرجع &all_memorys;
}
#endif
YOSYS_NAMESPACE_END
