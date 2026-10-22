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
	ایستا ثابت pool<IdString> res = []() {
		pool<IdString> r;
		برای (size_t i = 0; i < StaticCellTypes::builder.شمار; i++) {
			auto &cell = StaticCellTypes::builder.cells[i];
			اگر (cell.features.is_ff)
				r.درج(cell.type);
		}
		بازگشت r;
	}();
	بازگشت res;
}

#define check(condition) log_assert(condition && "malformed Const union")

ثابت Const::bitvectype& Const::get_bits() ثابت {
	check(is_bits());
	بازگشت *get_if_bits();
}

ثابت std::string& Const::get_str() ثابت {
	check(is_str());
	بازگشت *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	بازگشت *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	بازگشت *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	جدید ((تهی*)&str_) std::string(std::move(str));
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	نویسه bytes[] = {
		(نویسه)(val >> 24), (نویسه)(val >> 16), (نویسه)(val >> 8), (نویسه)val
	};
	جدید ((تهی*)&str_) std::string(bytes, 4);
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val, صحیح width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	اگر ((width & 7) == 0) {
		جدید ((تهی*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		صحیح bytes = width >> 3;
		signed نویسه sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<صحیح>(bytes, اندازه(val));
		برای (صحیح i = 0; i < bytes; i++) {
			str[str.size() - 1 - i] = val;
			val = val >> 8;
		}
		بازگشت;
	}

	جدید ((تهی*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	برای (صحیح i = 0; i < width; i++) {
		bv.افزودن_back((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, صحیح width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	جدید ((تهی*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	برای (صحیح i = 0; i < width; i++)
		bv.افزودن_back(bit);
}

RTLIL::Const::Const(ثابت std::vector<منطقی> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	جدید ((تهی*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.size());
	برای (ثابت auto &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ثابت RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	اگر (is_str())
		جدید ((تهی*)&str_) std::string(other.get_str());
	وگرنه اگر (is_bits())
		جدید ((تهی*)&bits_) bitvectype(other.get_bits());
	وگرنه
		check(نادرست);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	اگر (is_str())
		جدید ((تهی*)&str_) std::string(std::move(other.get_str()));
	وگرنه اگر (is_bits())
		جدید ((تهی*)&bits_) bitvectype(std::move(other.get_bits()));
	وگرنه
		check(نادرست);
}

RTLIL::Const &RTLIL::Const::operator =(ثابت RTLIL::Const &other) {
	flags = other.flags;
	اگر (other.is_str()) {
		اگر (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(تهی)جدید ((تهی*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} وگرنه اگر (other.is_bits()) {
		اگر (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(تهی)جدید ((تهی*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} وگرنه {
		check(نادرست);
	}
	بازگشت *این;
}

RTLIL::Const::~Const() {
	اگر (is_bits())
		bits_.~bitvectype();
	وگرنه اگر (is_str())
		str_.~string();
	وگرنه
		check(نادرست);
}

منطقی RTLIL::Const::operator<(ثابت RTLIL::Const &other) ثابت
{
	اگر (size() != other.size())
		بازگشت size() < other.size();

	برای (صحیح i = 0; i < size(); i++)
		اگر ((*این)[i] != other[i])
			بازگشت (*این)[i] < other[i];

	بازگشت نادرست;
}

منطقی RTLIL::Const::operator ==(ثابت RTLIL::Const &other) ثابت
{
	اگر (is_str() && other.is_str())
		بازگشت get_str() == other.get_str();
	اگر (is_bits() && other.is_bits())
		بازگشت get_bits() == other.get_bits();

	اگر (size() != other.size())
		بازگشت نادرست;

	برای (صحیح i = 0; i < size(); i++)
	اگر ((*این)[i] != other[i])
		بازگشت نادرست;

	بازگشت درست;
}

منطقی RTLIL::Const::operator !=(ثابت RTLIL::Const &other) ثابت
{
	بازگشت !(*این == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	بازگشت get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() ثابت
{
	std::vector<State> v;
	برای (auto bit : *این)
		v.افزودن_back(bit);
	بازگشت v;
}

منطقی RTLIL::Const::as_bool() ثابت
{
	اگر (is_str()) {
		برای (نویسه ch : get_str())
			اگر (ch != 0)
				بازگشت درست;
		بازگشت نادرست;
	}

	ثابت bitvectype& bv = get_bits();
	برای (size_t i = 0; i < bv.size(); i++)
		اگر (bv[i] == State::S1)
			بازگشت درست;
	بازگشت نادرست;
}

صحیح RTLIL::Const::as_int(منطقی is_signed) ثابت
{
	int32_t ret = 0;
	اگر (ثابت std::string *s = get_if_str()) {
		صحیح size = GetSize(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		برای (صحیح i = std::min(4, size); i > 0; i--)
			ret |= static_cast<unsigned نویسه>((*s)[size - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		اگر (is_signed && size > 0 && size < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << size*8;
		بازگشت ret;
	}

	ثابت bitvectype& bv = get_bits();
	صحیح significant_bits = std::min(GetSize(bv), 32);
	برای (صحیح i = 0; i < significant_bits; i++)
		اگر (bv[i] == State::S1)
			ret |= 1 << i;
	اگر (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	بازگشت ret;
}

منطقی RTLIL::Const::convertible_to_int(منطقی is_signed) ثابت
{
	auto size = get_min_size(is_signed);

	اگر (size < 0)
		بازگشت نادرست;

	// If it fits in 31 bits it is definitely convertible
	اگر (size <= 31)
		بازگشت درست;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	اگر (size == 32) {
		اگر (is_signed)
			بازگشت درست;
		بازگشت (*این)[size - 1] != State::S1;
	}

	بازگشت نادرست;
}

std::optional<صحیح> RTLIL::Const::try_as_int(منطقی is_signed) ثابت
{
	اگر (!convertible_to_int(is_signed))
		بازگشت std::nullopt;
	بازگشت as_int(is_signed);
}

صحیح RTLIL::Const::as_int_saturating(منطقی is_signed) ثابت
{
	اگر (!convertible_to_int(is_signed)) {
		اگر (!is_signed)
			بازگشت std::numeric_limits<صحیح>::max();

		ثابت auto min_size = get_min_size(is_signed);
		log_assert(min_size > 0);
		ثابت auto neg = (*این)[min_size - 1];
		بازگشت neg ? std::numeric_limits<صحیح>::min() : std::numeric_limits<صحیح>::max();
	}
	بازگشت as_int(is_signed);
}

تهی RTLIL::Const::tag_bare_integer_const(ثابت std::string &مقدار)
{
	اگر (مقدار.empty() || مقدار.find('\'') != std::string::npos)
		بازگشت;
	size_t start = (مقدار[0] == '-' || مقدار[0] == '+') ? 1 : 0;
	اگر (start < مقدار.size() && std::all_of(مقدار.begin() + start, مقدار.end(), ::isdigit))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

صحیح RTLIL::Const::get_min_size(منطقی is_signed) ثابت
{
	اگر (empty()) بازگشت 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	اگر (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	وگرنه
		leading_bit = RTLIL::State::S0;

	auto idx = size();
	وقتی (idx > 0 && (*این)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	اگر (is_signed && idx < size()) {
		idx++;
	}
	// must be at least one bit
	بازگشت (idx == 0) ? 1 : idx;
}

تهی RTLIL::Const::compress(منطقی is_signed)
{
	auto idx = get_min_size(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<صحیح> RTLIL::Const::as_int_compress(منطقی is_signed) ثابت
{
	بازگشت try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ثابت نویسه* any) ثابت
{
	صحیح sz = size();
	std::string ret;
	ret.reserve(sz);
	برای (صحیح i = sz - 1; i >= 0; --i)
		انتخاب ((*این)[i]) {
			حالت S0: ret.افزودن_back('0'); بشکن;
			حالت S1: ret.افزودن_back('1'); بشکن;
			حالت Sx: ret.افزودن_back('x'); بشکن;
			حالت Sz: ret.افزودن_back('z'); بشکن;
			حالت Sa: ret += any; بشکن;
			حالت Sm: ret.افزودن_back('m'); بشکن;
		}
	بازگشت ret;
}

RTLIL::Const RTLIL::Const::from_string(ثابت std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.size());
	برای (auto it = str.rbegin(); it != str.rend(); it++)
		انتخاب (*it) {
			حالت '0': bv.افزودن_back(State::S0); بشکن;
			حالت '1': bv.افزودن_back(State::S1); بشکن;
			حالت 'x': bv.افزودن_back(State::Sx); بشکن;
			حالت 'z': bv.افزودن_back(State::Sz); بشکن;
			حالت 'm': bv.افزودن_back(State::Sm); بشکن;
			پیشفرض: bv.افزودن_back(State::Sa);
		}
	بازگشت c;
}

std::string RTLIL::Const::decode_string() ثابت
{
	اگر (auto str = get_if_str())
		بازگشت *str;

	ثابت bitvectype& bv = get_bits();
	ثابت صحیح n = GetSize(bv);
	ثابت صحیح n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	صحیح i = n_over_8 * 8;
	اگر (i < n) {
		نویسه ch = 0;
		برای (صحیح j = 0; j < (n - i); j++) {
			اگر (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		اگر (ch != 0)
			s.append({ch});
	}
	i -= 8;
	برای (; i >= 0; i -= 8) {
		نویسه ch = 0;
		برای (صحیح j = 0; j < 8; j++) {
			اگر (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		اگر (ch != 0)
			s.append({ch});
	}
	بازگشت s;
}

صحیح RTLIL::Const::size() ثابت {
	اگر (is_str())
		بازگشت 8 * str_.size();
	وگرنه {
		check(is_bits());
		بازگشت bits_.size();
	}
}

منطقی RTLIL::Const::empty() ثابت {
	اگر (is_str())
		بازگشت str_.empty();
	وگرنه {
		check(is_bits());
		بازگشت bits_.empty();
	}
}

تهی RTLIL::Const::bitvectorize_internal() {
	اگر (tag == backing_tag::bits)
		بازگشت;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.size() * 8);
	برای (صحیح i = str_.size() - 1; i >= 0; i--) {
		unsigned نویسه ch = str_[i];
		برای (صحیح j = 0; j < 8; j++) {
			new_bits.افزودن_back((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(تهی)جدید ((تهی*)&bits_) bitvectype(std::move(new_bits));
		tag = backing_tag::bits;
	}
}

تهی RTLIL::Const::append(ثابت RTLIL::Const &other) {
	bitvectorize_internal();
	bitvectype& bv = get_bits();
	bv.درج(bv.end(), other.begin(), other.end());
}

RTLIL::State RTLIL::Const::const_iterator::operator*() ثابت {
	اگر (auto bv = parent->get_if_bits())
		بازگشت (*bv)[idx];

	صحیح char_idx = parent->get_str().size() - idx / 8 - 1;
	منطقی bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	بازگشت bit ? State::S1 : State::S0;
}

منطقی RTLIL::Const::is_fully_zero() ثابت
{
	اگر (auto str = get_if_str()) {
		برای (نویسه ch : *str)
			اگر (ch != 0)
				بازگشت نادرست;
		بازگشت درست;
	}

	ثابت bitvectype& bv = get_bits();

	برای (ثابت auto &bit : bv)
		اگر (bit != RTLIL::State::S0)
			بازگشت نادرست;

	بازگشت درست;
}

منطقی RTLIL::Const::is_fully_ones() ثابت
{
	اگر (auto str = get_if_str()) {
		برای (نویسه ch : *str)
			اگر (ch != (نویسه)0xff)
				بازگشت نادرست;
		بازگشت درست;
	}

	ثابت bitvectype& bv = get_bits();
	برای (ثابت auto &bit : bv)
		اگر (bit != RTLIL::State::S1)
			بازگشت نادرست;

	بازگشت درست;
}

منطقی RTLIL::Const::is_fully_def() ثابت
{
	اگر (is_str())
		بازگشت درست;

	ثابت bitvectype& bv = get_bits();
	برای (ثابت auto &bit : bv)
		اگر (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			بازگشت نادرست;

	بازگشت درست;
}

منطقی RTLIL::Const::is_fully_undef() ثابت
{
	اگر (auto str = get_if_str())
		بازگشت str->empty();

	ثابت bitvectype& bv = get_bits();
	برای (ثابت auto &bit : bv)
		اگر (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			بازگشت نادرست;

	بازگشت درست;
}

منطقی RTLIL::Const::is_fully_undef_x_only() ثابت
{
	اگر (auto str = get_if_str())
		بازگشت str->empty();

	ثابت bitvectype& bv = get_bits();
	برای (ثابت auto &bit : bv)
		اگر (bit != RTLIL::State::Sx)
			بازگشت نادرست;

	بازگشت درست;
}

منطقی RTLIL::Const::is_onehot(صحیح *pos) ثابت
{
	منطقی found = نادرست;
	صحیح size = GetSize(*این);
	برای (صحیح i = 0; i < size; i++) {
		State bit = (*این)[i];
		اگر (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			بازگشت نادرست;
		اگر (bit == RTLIL::State::S1) {
			اگر (found)
				بازگشت نادرست;
			اگر (pos)
				*pos = i;
			found = درست;
		}
	}
	بازگشت found;
}

Hasher RTLIL::Const::hash_into(Hasher h) ثابت
{
	اگر (auto str = get_if_str())
		بازگشت hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ثابت bitvectype &bv = get_bits();
	صحیح size = GetSize(bv);
	std::string packed;
	صحیح packed_size = (size + 7) >> 3;
	packed.resize(packed_size, 0);
	برای (صحیح bi = 0; bi < packed_size; ++bi) {
		نویسه ch = 0;
		صحیح end = std::min((bi + 1)*8, size);
		برای (صحیح i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			اگر (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<نویسه>::hash_into(ch, h);
				برای (; i < size; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(size);
				بازگشت h;
			}
			ch |= static_cast<صحیح>(b) << (i & 7);
		}
		packed[packed_size - 1 - bi] = ch;
	}
	بازگشت hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(صحیح offset, صحیح len, RTLIL::State padding) ثابت {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	برای (صحیح i = offset; i < offset + len; i++)
		ret_bv.افزودن_back(i < GetSize(*این) ? (*این)[i] : padding);
	بازگشت RTLIL::Const(ret_bv);
}
#undef check /* check(condition) for Const */

منطقی RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ثابت
{
	بازگشت attributes.شمار(id);
}

تهی RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, منطقی مقدار)
{
	اگر (مقدار)
		attributes[id] = RTLIL::Const(1);
	وگرنه
		attributes.erase(id);
}

منطقی RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ثابت
{
	ثابت auto it = attributes.find(id);
	اگر (it == attributes.end())
		بازگشت نادرست;
	بازگشت it->second.as_bool();
}

تهی RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string مقدار)
{
	اگر (مقدار.empty())
		attributes.erase(id);
	وگرنه
		attributes[id] = مقدار;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ثابت
{
	std::string مقدار;
	ثابت auto it = attributes.find(id);
	اگر (it != attributes.end())
		مقدار = it->second.decode_string();
	بازگشت مقدار;
}

تهی RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &data)
{
	string attrval;
	برای (ثابت auto &s : data) {
		اگر (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

تهی RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &data)
{
	pool<string> union_data = get_strpool_attribute(id);
	union_data.درج(data.begin(), data.end());
	اگر (!union_data.empty())
		set_strpool_attribute(id, union_data);
}

تهی RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->attributes = std::move(buf);
}

فضاینام {
	template<typename F>
	تهی walk_process_attr_objects(RTLIL::Process *process, F visit) {
		visit(process);
		std::vector<RTLIL::CaseRule*> case_پشته{&process->ریشه_case};
		وقتی (!case_پشته.empty()) {
			RTLIL::CaseRule *cs = case_پشته.back();
			case_پشته.برداشت_back();
			visit(cs);
			برای (auto *sw : cs->switches) {
				visit(sw);
				برای (auto *case_ : sw->cases)
					case_پشته.افزودن_back(case_);
			}
		}
		برای (auto *همگام : process->syncs)
			برای (auto &mwa : همگام->mem_write_actions)
				visit(&mwa);
	}

	template<typename F>
	تهی walk_attr_objects(RTLIL::Design *design, F visit) {
		برای (auto &[_, module] : design->modules_) {
			visit(module);
			برای (auto &[_, wire] : module->wires_)
				visit(wire);
			برای (auto &[_, mem] : module->memories)
				visit(mem);
			برای (auto &[_, cell] : module->cells_)
				visit(cell);
			برای (auto &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


size_t RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	auto ریشه = [&](IdString ref) {
		اگر (ref != IdString::Null)
			live.درج(ref);
	};

	walk_attr_objects(این, [&](ثابت RTLIL::AttrObject *obj) {
		برای (auto &attr : obj->attributes)
			ریشه(attr.first);
	});

	ریشه(selected_active_module);

	برای (auto &[name, module] : modules_) {
		ریشه(name);
		برای (IdString port : module->ports)
			ریشه(port);
		برای (IdString param : module->avail_parameters)
			ریشه(param);
		برای (auto &[param, _] : module->parameter_default_مقادیر)
			ریشه(param);
		برای (auto &[name, _] : module->memories)
			ریشه(name);
		برای (auto &[name, _] : module->processes)
			ریشه(name);
		برای (auto &[name, wire] : module->wires_) {
			ریشه(name);
			اگر (wire->known_driver())
				ریشه(wire->driverPort());
		}
		برای (auto &[name, cell] : module->cells_) {
			ریشه(name);
			ریشه(cell->type);
			برای (auto &conn : cell->connections())
				ریشه(conn.first);
			برای (auto &param : cell->parameters)
				ریشه(param.first);
		}
		برای (auto &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_پشته{&process->ریشه_case};
			وقتی (!case_پشته.empty()) {
				RTLIL::CaseRule *cs = case_پشته.back();
				case_پشته.برداشت_back();
				برای (auto *sw : cs->switches) {
					برای (auto *case_ : sw->cases)
						case_پشته.افزودن_back(case_);
				}
			}
			برای (auto *همگام : process->syncs) {
				برای (auto &mwa : همگام->mem_write_actions)
					ریشه(mwa.memid);
			}
		}
	}

	auto ریشه_selection = [&](ثابت RTLIL::Selection &sel) {
		برای (IdString m : sel.selected_modules)
			ریشه(m);
		برای (auto &[m, members] : sel.selected_members) {
			ریشه(m);
			برای (IdString member : members)
				ریشه(member);
		}
	};

	برای (auto &[name, sel] : selection_vars) {
		ریشه(name);
		ریشه_selection(sel);
	}

	برای (auto &sel : selection_پشته)
		ریشه_selection(sel);

	size_t erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_شمار;
	بازگشت erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ثابت
{
	pool<string> data;
	اگر (attributes.شمار(id) != 0)
		برای (auto s : split_tokens(get_string_attribute(id), "|"))
			data.درج(s);
	بازگشت data;
}

تهی RTLIL::AttrObject::set_hdlname_attribute(ثابت vector<string> &hierarchy)
{
	string attrval;
	برای (ثابت auto &ident : hierarchy) {
		اگر (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() ثابت
{
	بازگشت split_tokens(get_string_attribute(ID::hdlname), " ");
}

تهی RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, ثابت vector<صحیح> &data)
{
	std::stringstream attrval;
	برای (auto &i : data) {
		اگر (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<صحیح> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ثابت
{
	vector<صحیح> data;
	auto it = attributes.find(id);
	اگر (it != attributes.end())
		برای (ثابت auto &s : split_tokens(attributes.at(id).decode_string())) {
			نویسه *end = nullptr;
			errno = 0;
			long مقدار = strtol(s.c_str(), &end, 10);
			اگر (end != s.c_str() + s.size())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			اگر (errno == ERANGE || مقدار < INT_MIN || مقدار > INT_MAX)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			data.افزودن_back(مقدار);
		}
	بازگشت data;
}

منطقی RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ثابت
{
	اگر (current_design != nullptr) {
		auto module = current_design->module(mod_name);
		بازگشت module && module->get_blackbox_attribute();
	} وگرنه {
		log_warning("Unable to check if module is boxed for null design.\n");
		بازگشت نادرست;
	}
}

منطقی RTLIL::Selection::selected_module(RTLIL::IdString mod_name) ثابت
{
	اگر (complete_selection)
		بازگشت درست;
	اگر (!selects_boxes && boxed_module(mod_name))
		بازگشت نادرست;
	اگر (full_selection)
		بازگشت درست;
	اگر (selected_modules.شمار(mod_name) > 0)
		بازگشت درست;
	اگر (selected_members.شمار(mod_name) > 0)
		بازگشت درست;
	بازگشت نادرست;
}

منطقی RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) ثابت
{
	اگر (complete_selection)
		بازگشت درست;
	اگر (!selects_boxes && boxed_module(mod_name))
		بازگشت نادرست;
	اگر (full_selection)
		بازگشت درست;
	اگر (selected_modules.شمار(mod_name) > 0)
		بازگشت درست;
	بازگشت نادرست;
}

منطقی RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	اگر (complete_selection)
		بازگشت درست;
	اگر (!selects_boxes && boxed_module(mod_name))
		بازگشت نادرست;
	اگر (full_selection)
		بازگشت درست;
	اگر (selected_modules.شمار(mod_name) > 0)
		بازگشت درست;
	اگر (selected_members.شمار(mod_name) > 0)
		اگر (selected_members.at(mod_name).شمار(memb_name) > 0)
			بازگشت درست;
	بازگشت نادرست;
}

تهی RTLIL::Selection::optimize(RTLIL::Design *design)
{
	اگر (design != current_design) {
		current_design = design;
	}

	اگر (selects_boxes && full_selection)
		complete_selection = درست;
	اگر (complete_selection) {
		full_selection = نادرست;
		selects_boxes = درست;
	}
	اگر (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		بازگشت;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.clear();
	برای (auto mod_name : selected_modules) {
		اگر (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.افزودن_back(mod_name);
		selected_members.erase(mod_name);
	}
	برای (auto mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	برای (auto &it : selected_members)
		اگر (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.افزودن_back(it.first);
	برای (auto mod_name : del_list)
		selected_members.erase(mod_name);

	برای (auto &it : selected_members) {
		del_list.clear();
		برای (auto memb_name : it.second)
			اگر (current_design->module(it.first)->شمار_id(memb_name) == 0)
				del_list.افزودن_back(memb_name);
		برای (auto memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	add_list.clear();
	برای (auto &it : selected_members)
		اگر (it.second.size() == 0)
			del_list.افزودن_back(it.first);
		وگرنه اگر (it.second.size() == current_design->module(it.first)->wires_.size() + current_design->module(it.first)->memories.size() +
				current_design->module(it.first)->cells_.size() + current_design->module(it.first)->processes.size())
			add_list.افزودن_back(it.first);
	برای (auto mod_name : del_list)
		selected_members.erase(mod_name);
	برای (auto mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.درج(mod_name);
	}

	اگر (selected_modules.size() == current_design->modules_.size()) {
		selected_modules.clear();
		selected_members.clear();
		اگر (selects_boxes)
			complete_selection = درست;
		وگرنه
			full_selection = درست;
	}
}

تهی RTLIL::Selection::clear()
{
	full_selection = نادرست;
	complete_selection = نادرست;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_defines (جدید define_map_t)
{
	ایستا unsigned صحیح hashidx_شمار = 123456789;
	hashidx_شمار = mkhash_xorshift(hashidx_شمار);
	hashidx_ = hashidx_شمار;

	refcount_modules_ = 0;
	selected_active_module.clear();
	افزودن_full_selection();

	RTLIL::Design::get_all_designs()->درج(std::pair<unsigned صحیح, RTLIL::Design*>(hashidx_, این));
}

RTLIL::Design::~Design()
{
	برای (auto &pr : modules_)
		delete pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

ایستا std::map<unsigned صحیح, RTLIL::Design*> all_designs;
std::map<unsigned صحیح, RTLIL::Design*> *RTLIL::Design::get_all_designs(تهی)
{
	بازگشت &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	بازگشت RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ثابت RTLIL::Module *RTLIL::Design::module(IdString id) ثابت {
	twines_.check_owned(id);
	بازگشت modules_.شمار(id) ? modules_.at(id) : پوچ;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	بازگشت modules_.شمار(id) ? modules_.at(id) : پوچ;
}

RTLIL::Module *RTLIL::Design::بالاترین_module() ثابت
{
	RTLIL::Module *module = nullptr;
	صحیح module_شمار = 0;

	برای (auto mod : selected_modules()) {
		اگر (mod->get_bool_attribute(ID::بالاترین))
			بازگشت mod;
		module_شمار++;
		module = mod;
	}

	بازگشت module_شمار == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	اگر (modules_.شمار(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = جدید RTLIL::Module;
	modules_[name] = module;
	module->design = این;
	module->name = name;

	برای (auto mon : monitors)
		mon->notify_module_add(module);

	اگر (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	بازگشت module;
}

تهی RTLIL::Design::scratchpad_unset(ثابت std::string &varname)
{
	scratchpad.erase(varname);
}

تهی RTLIL::Design::scratchpad_set_int(ثابت std::string &varname, صحیح مقدار)
{
	scratchpad[varname] = stringf("%d", مقدار);
}

تهی RTLIL::Design::scratchpad_set_bool(ثابت std::string &varname, منطقی مقدار)
{
	scratchpad[varname] = مقدار ? "true" : "false";
}

تهی RTLIL::Design::scratchpad_set_string(ثابت std::string &varname, std::string مقدار)
{
	scratchpad[varname] = std::move(مقدار);
}

صحیح RTLIL::Design::scratchpad_get_int(ثابت std::string &varname, صحیح default_مقدار_2) ثابت
{
	auto it = scratchpad.find(varname);
	اگر (it == scratchpad.end())
		بازگشت default_مقدار_2;

	ثابت std::string &str = it->second;

	اگر (str == "0" || str == "false")
		بازگشت 0;

	اگر (str == "1" || str == "true")
		بازگشت 1;

	نویسه *endptr = nullptr;
	long صحیح parsed_مقدار = strtol(str.c_str(), &endptr, 10);
	بازگشت *endptr ? default_مقدار_2 : parsed_مقدار;
}

منطقی RTLIL::Design::scratchpad_get_bool(ثابت std::string &varname, منطقی default_مقدار_2) ثابت
{
	auto it = scratchpad.find(varname);
	اگر (it == scratchpad.end())
		بازگشت default_مقدار_2;

	ثابت std::string &str = it->second;

	اگر (str == "0" || str == "false")
		بازگشت نادرست;

	اگر (str == "1" || str == "true")
		بازگشت درست;

	بازگشت default_مقدار_2;
}

std::string RTLIL::Design::scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_مقدار_2) ثابت
{
	auto it = scratchpad.find(varname);
	اگر (it == scratchpad.end())
		بازگشت default_مقدار_2;

	بازگشت it->second;
}

تهی RTLIL::Design::remove(RTLIL::Module *module)
{
	برای (auto mon : monitors)
		mon->notify_module_del(module);

	اگر (yosys_xtrace) {
		log("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	delete module;
}

تهی RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	add(module);
}

تهی RTLIL::Design::sort()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
	برای (auto &it : modules_)
		it.second->sort();
}

تهی RTLIL::Design::sort_modules()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
}

تهی check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

تهی RTLIL::Design::check()
{
#ifndef NDEBUG
	log_assert(!selection_پشته.empty());
	صحیح pool_size = 0;
	برای (auto &it : modules_)
		pool_size = std::max(pool_size, ThreadPool::work_pool_size(0, it.second->cells_size(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_size);
	برای (auto &it : modules_) {
		log_assert(این == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#endif
}

تهی RTLIL::Design::optimize()
{
	برای (auto &it : modules_)
		it.second->optimize();
	برای (auto &it : selection_پشته)
		it.optimize(این);
	برای (auto &it : selection_vars)
		it.second.optimize(این);
}

تهی RTLIL::Design::clone_into(RTLIL::Design *dst) ثابت
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	برای (auto it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

منطقی RTLIL::Design::selected_module(IdString mod_name) ثابت
{
	اگر (!selected_active_module.empty() && mod_name != selected_active_module)
		بازگشت نادرست;
	بازگشت selection().selected_module(mod_name);
}

منطقی RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) ثابت
{
	اگر (!selected_active_module.empty() && mod_name != selected_active_module)
		بازگشت نادرست;
	بازگشت selection().selected_whole_module(mod_name);
}

منطقی RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	اگر (!selected_active_module.empty() && mod_name != selected_active_module)
		بازگشت نادرست;
	بازگشت selection().selected_member(mod_name, memb_name);
}

منطقی RTLIL::Design::selected_module(RTLIL::Module *mod) ثابت
{
	بازگشت selected_module(mod->name);
}

منطقی RTLIL::Design::selected_whole_module(RTLIL::Module *mod) ثابت
{
	بازگشت selected_whole_module(mod->name);
}

تهی RTLIL::Design::افزودن_selection(RTLIL::Selection sel)
{
	sel.current_design = این;
	selection_پشته.افزودن_back(sel);
}

تهی RTLIL::Design::افزودن_empty_selection()
{
	افزودن_selection(RTLIL::Selection::EmptySelection(این));
}

تهی RTLIL::Design::افزودن_full_selection()
{
	افزودن_selection(RTLIL::Selection::FullSelection(این));
}

تهی RTLIL::Design::افزودن_complete_selection()
{
	افزودن_selection(RTLIL::Selection::CompleteSelection(این));
}

تهی RTLIL::Design::برداشت_selection()
{
	selection_پشته.برداشت_back();
	// Default to a full_selection if we ran out of stack
	اگر (selection_پشته.empty())
		افزودن_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(منطقی only_selected) ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(این), only_selected, درست, نادرست, RTLIL_BACKEND::DumpMode::Readable);
	بازگشت f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ثابت
{
	منطقی include_partials = partials == RTLIL::SELECT_ALL;
	منطقی exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	منطقی ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> result;
	result.reserve(modules_.size());
	برای (auto &it : modules_) {
		اگر (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			اگر (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				result.افزودن_back(it.second);
			وگرنه
				انتخاب (boxes)
				{
				حالت RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.second);
					بشکن;
				حالت RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.second);
					بشکن;
				حالت RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.second);
					بشکن;
				حالت RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.second);
					بشکن;
				حالت RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.second);
					بشکن;
				حالت RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.second);
					بشکن;
				پیشفرض:
					بشکن;
				}
		} وگرنه اگر (!include_partials && selected_module(it.first)) {
			انتخاب(partials)
			{
			حالت RTLIL::SELECT_WHOLE_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.second);
				بشکن;
			حالت RTLIL::SELECT_WHOLE_ERR:
				log_error("Unsupported partially selected module %s.\n", it.second);
				بشکن;
			حالت RTLIL::SELECT_WHOLE_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.second);
				بشکن;
			پیشفرض:
				بشکن;
			}
		}
	}
	بازگشت result;
}

RTLIL::Module::Module()
{
	ایستا unsigned صحیح hashidx_شمار = 123456789;
	hashidx_شمار = mkhash_xorshift(hashidx_شمار);
	hashidx_ = hashidx_شمار;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->درج(std::pair<unsigned صحیح, RTLIL::Module*>(hashidx_, این));
#endif
}

RTLIL::Module::~Module()
{
	برای (auto &pr : wires_)
		delete pr.second;
	برای (auto &pr : memories)
		delete pr.second;
	برای (auto &pr : cells_)
		delete pr.second;
	برای (auto &pr : processes)
		delete pr.second;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
ایستا std::map<unsigned صحیح, RTLIL::Module*> all_modules;
std::map<unsigned صحیح, RTLIL::Module*> *RTLIL::Module::get_all_modules(تهی)
{
	بازگشت &all_modules;
}
#endif

تهی RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	برای (auto it = wires_.begin(); it != wires_.end(); ++it)
		اگر (!it->second->port_درون_داد && !it->second->port_output)
			delwires.درج(it->second);

	برای (auto it = memories.begin(); it != memories.end(); ++it)
		delete it->second;
	memories.clear();

	برای (auto it = cells_.begin(); it != cells_.end(); ++it)
		delete it->second;
	cells_.clear();

	برای (auto it = processes.begin(); it != processes.end(); ++it)
		delete it->second;
	processes.clear();

	connections_.clear();

	remove(delwires);
	set_bool_attribute(ID::blackbox);
}

تهی RTLIL::Module::expand_interfaces(RTLIL::Design *, ثابت dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

منطقی RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	بازگشت نادرست;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ثابت dict<RTLIL::IdString, RTLIL::Const> &, منطقی mayfail)
{
	اگر (mayfail)
		بازگشت IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ثابت dict<RTLIL::IdString, RTLIL::Const> &, ثابت dict<RTLIL::IdString, RTLIL::Module*> &, ثابت dict<RTLIL::IdString, RTLIL::IdString> &, منطقی mayfail)
{
	اگر (mayfail)
		بازگشت IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

size_t RTLIL::Module::شمار_id(RTLIL::IdString id)
{
	بازگشت wires_.شمار(id) + cells_.شمار(id) + memories.شمار(id) + processes.شمار(id);
}

#ifndef NDEBUG
فضاینام {
	ساختار InternalCellChecker
	{
		ثابت RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		InternalCellChecker(ثابت RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		تهی error(صحیح linenr, std::optional<std::string> error_پیام = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			اگر (error_پیام) {
				log_error("%s in internal cell %s\n", *error_پیام, cell_loc);
			} وگرنه {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		صحیح param(RTLIL::IdString name)
		{
			auto it = cell->parameters.find(name);
			اگر (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.درج(name);
			بازگشت it->second.as_int();
		}

		صحیح param_bool(RTLIL::IdString name)
		{
			صحیح v = param(name);
			اگر (GetSize(cell->parameters.at(name)) > 32)
				error(__LINE__);
			اگر (v != 0 && v != 1)
				error(__LINE__);
			بازگشت v;
		}

		صحیح param_bool(RTLIL::IdString name, منطقی expected)
		{
			صحیح v = param_bool(name);
			اگر (v != expected)
				error(__LINE__);
			بازگشت v;
		}

		تهی param_bits(RTLIL::IdString name, صحیح width)
		{
			param(name);
			اگر (GetSize(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			بازگشت cell->parameters.at(name).decode_string();
		}

		تهی port(RTLIL::IdString name, صحیح width)
		{
			auto it = cell->connections_.find(name);
			اگر (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			اگر (GetSize(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.درج(name);
		}

		تهی check_expected(منطقی check_matched_sign = نادرست)
		{
			برای (auto &para : cell->parameters)
				اگر (expected_params.شمار(para.first) == 0)
					error(__LINE__);
			برای (auto &conn : cell->connections())
				اگر (expected_ports.شمار(conn.first) == 0)
					error(__LINE__);

			اگر (check_matched_sign) {
				log_assert(expected_params.شمار(ID::A_SIGNED) != 0 && expected_params.شمار(ID::B_SIGNED) != 0);
				منطقی a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				منطقی b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				اگر (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		تهی check()
		{
			اگر (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				بازگشت;

			اگر (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(درست);
				بازگشت;
			}

			اگر (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/نادرست);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/نادرست);
				بازگشت;
			}

			اگر (cell->type.in(ID($shift), ID($shiftx))) {
				اگر (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/نادرست);
				} وگرنه {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/نادرست);
				بازگشت;
			}

			اگر (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(درست);
				بازگشت;
			}

			اگر (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($pow));
				بازگشت;
			}

			اگر (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::CI, 1);
				port(ID::BI, 1);
				port(ID::X, param(ID::Y_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				port(ID::CO, param(ID::Y_WIDTH));
				check_expected(درست);
				بازگشت;
			}

			اگر (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				بازگشت;
			}

			اگر (cell->type == ID($macc_v2)) {
				اگر (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				اگر (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				اگر (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ثابت Const &a_width = cell->getParam(ID::A_WIDTHS);
				ثابت Const &b_width = cell->getParam(ID::B_WIDTHS);
				ثابت Const &c_width = cell->getParam(ID::C_WIDTHS);
				صحیح a_width_sum = 0, b_width_sum = 0, c_width_sum = 0;
				برای (صحیح i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_sum += a_width.extract(16 * i, 16).as_int(نادرست);
					b_width_sum += b_width.extract(16 * i, 16).as_int(نادرست);
				}
				برای (صحیح i = 0; i < param(ID::NADDENDS); i++) {
					c_width_sum += c_width.extract(16 * i, 16).as_int(نادرست);
				}
				port(ID::A, a_width_sum);
				port(ID::B, b_width_sum);
				port(ID::C, c_width_sum);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/نادرست);
				بازگشت;
			}

			اگر (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				اگر (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($dffsre)) {
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
				بازگشت;
			}

			اگر (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_مقدار, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_مقدار, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type.in(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_مقدار, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_مقدار, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($aldffe)) {
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
				بازگشت;
			}

			اگر (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_مقدار, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_BITS);
				param(ID::STATE_رقم);
				param(ID::STATE_رقم_LOG2);
				param(ID::STATE_RST);
				param_bits(ID::STATE_TABLE, param(ID::STATE_BITS) * param(ID::STATE_رقم));
				param(ID::TRANS_رقم);
				param_bits(ID::TRANS_TABLE, param(ID::TRANS_رقم) * (2*param(ID::STATE_رقم_LOG2) + param(ID::CTRL_IN_WIDTH) + param(ID::CTRL_OUT_WIDTH)));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::CTRL_IN, param(ID::CTRL_IN_WIDTH));
				port(ID::CTRL_OUT, param(ID::CTRL_OUT_WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_MASK);
				param(ID::COLLISION_X_MASK);
				param_bool(ID::CE_OVER_SRST);
				param_bits(ID::ARST_مقدار, param(ID::WIDTH));
				param_bits(ID::SRST_مقدار, param(ID::WIDTH));
				param_bits(ID::INIT_مقدار, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::SRST, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($memwr_v2)) {
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
				بازگشت;
			}

			اگر (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::واژگان));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::واژگان));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($mem)) {
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
				بازگشت;
			}

			اگر (cell->type == ID($mem_v2)) {
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
				param_bits(ID::RD_ARST_مقدار, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_SRST_مقدار, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_INIT_مقدار, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
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
				بازگشت;
			}

			اگر (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type.in(ID($أكد), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				بازگشت;
			}

			اگر (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				بازگشت;
			}

			اگر (cell->type.in(ID($specify2), ID($specify3))) {
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
				اگر (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
				}
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($specrule)) {
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
				بازگشت;
			}

			اگر (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				بازگشت;
			}

			اگر (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				اگر (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
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
				بازگشت;
			}

			اگر (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				اگر (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				بازگشت;
			}

			اگر (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); بازگشت; }

			اگر (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); بازگشت; }

			اگر (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); بازگشت; }
			اگر (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); بازگشت; }

			اگر (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); بازگشت; }

			اگر (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  بازگشت; }

			اگر (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); بازگشت; }

			اگر (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); بازگشت; }

			اگر (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); بازگشت; }

			اگر (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); بازگشت; }

			اگر (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); بازگشت; }

			اگر (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); بازگشت; }

			اگر (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); بازگشت; }

			اگر (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); بازگشت; }

			اگر (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); بازگشت; }

			اگر (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); بازگشت; }

			اگر (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); بازگشت; }

			اگر (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); بازگشت; }

			اگر (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); بازگشت; }

			اگر (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}
			اگر (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}
			اگر (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}
			اگر (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}
			اگر (cell->type.in(ID($درون_داد_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				بازگشت;
			}
			اگر (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				check_expected();
				بازگشت;
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

تهی RTLIL::Module::sort()
{
	wires_.sort(sort_by_id_str(design->twines()));
	cells_.sort(sort_by_id_str(design->twines()));
	parameter_default_مقادیر.sort(sort_by_id_str(design->twines()));
	memories.sort(sort_by_id_str(design->twines()));
	processes.sort(sort_by_id_str(design->twines()));
	برای (auto &it : cells_)
		it.second->sort();
	برای (auto &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	برای (auto &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#ifndef NDEBUG
ایستا تهی check_id_کلیدها(ثابت dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	برای (auto &it : keyed)
		log_assert(!it.first.empty());
}
#endif

تهی check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ifndef NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->cells_size(), 1000));
	ثابت RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	برای (auto &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->width >= 0);
		log_assert(it.second->size >= 0);
		check_id_کلیدها(it.second->attributes);
		memory_strings.درج(it.second->name.str());
	}

	std::vector<MonotonicFlag> ports_declared(GetSize(module->ports));
	ShardedVector<std::string> memids(subpool);
	subpool.run([const_module, &ports_declared, &memory_strings, &memids](ثابت ParallelDispatchThreadPool::RunCtx &ctx) {
		برای (صحیح i : ctx.item_range(const_module->cells_size())) {
			auto it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			برای (auto &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_کلیدها(it.second->attributes);
			check_id_کلیدها(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			اگر (it.second->has_memid()) {
				log_assert(memory_strings.شمار(it.second->parameters.at(ID::MEMID).decode_string()));
			} وگرنه اگر (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.شمار(memid));
				memids.درج(ctx, std::move(memid));
			}
			auto cell_mod = const_module->design->module(it.second->name);
			اگر (cell_mod != nullptr) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#ifdef __GNUC__
				#pragma GCC diagnostic افزودن
				#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				#endif
				log_assert(!it.second->get_blackbox_attribute());
				#ifdef __GNUC__
				#pragma GCC diagnostic برداشت
				#endif
			}
		}

		برای (صحیح i : ctx.item_range(const_module->wires_size())) {
			auto it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_کلیدها(it.second->attributes);
			اگر (it.second->port_id) {
				log_assert(GetSize(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_درون_داد || it.second->port_output);
				log_assert(it.second->port_id <= GetSize(ports_declared));
				منطقی previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == نادرست);
			} وگرنه
				log_assert(!it.second->port_درون_داد && !it.second->port_output);
		}
	});
	برای (ثابت MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == درست);
	pool<std::string> memids_pool;
	برای (std::string &memid : memids)
		log_assert(memids_pool.درج(memid).second);

	برای (auto &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->ریشه_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->ریشه_case};
		برای (size_t i = 0; i < all_cases.size(); i++) {
			برای (auto &switch_it : all_cases[i]->switches) {
				برای (auto &case_it : switch_it->cases) {
					برای (auto &compare_it : case_it->compare) {
						log_assert(switch_it->سیگنال.size() == compare_it.size());
					}
					all_cases.افزودن_back(case_it);
				}
			}
		}
		برای (auto &sync_it : it.second->syncs) {
			انتخاب (sync_it->type) {
				حالت RTLIL::SyncType::ST0:
				حالت RTLIL::SyncType::ST1:
				حالت RTLIL::SyncType::STp:
				حالت RTLIL::SyncType::STn:
				حالت RTLIL::SyncType::STe:
					log_assert(!sync_it->سیگنال.empty());
					بشکن;
				حالت RTLIL::SyncType::STa:
				حالت RTLIL::SyncType::STg:
				حالت RTLIL::SyncType::STi:
					log_assert(sync_it->سیگنال.empty());
					بشکن;
			}
		}
	}

	برای (auto &it : module->connections_) {
		log_assert(it.first.size() == it.second.size());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_کلیدها(module->attributes);
#endif
}

تهی RTLIL::Module::check()
{
	صحیح pool_size = ThreadPool::work_pool_size(0, cells_size(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_size);
	check_module(این, thread_pool);
}

تهی RTLIL::Module::optimize()
{
}

تهی RTLIL::Module::cloneInto(RTLIL::Module *new_mod) ثابت
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	برای (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_مقادیر.clear();
	برای (auto &it : parameter_default_مقادیر)
		new_mod->parameter_default_مقادیر[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	برای (auto &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	برای (auto it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	برای (auto it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	برای (auto it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	برای (auto it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addProcess(dst_id, it->second);
	}

	ساختار RewriteSigSpecWorker
	{
		ثابت dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ثابت RTLIL::Module *new_mod;
		تهی operator()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([این](RTLIL::Wire *&wire) {
				auto it = wire_map.find(wire);
				اگر (it != wire_map.end())
					wire = it->second;
				وگرنه
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
	RTLIL::Module *new_mod = جدید RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	بازگشت new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) ثابت
{
	RTLIL::Module *new_mod = جدید RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->add(new_mod);
	بازگشت new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) ثابت
{
	RTLIL::Module *new_mod = جدید RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	بازگشت new_mod;
}

منطقی RTLIL::Module::has_memories() ثابت
{
	بازگشت !memories.empty();
}

منطقی RTLIL::Module::has_processes() ثابت
{
	بازگشت !processes.empty();
}

منطقی RTLIL::Module::has_memories_warn() ثابت
{
	اگر (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", این);
	بازگشت !memories.empty();
}

منطقی RTLIL::Module::has_processes_warn() ثابت
{
	اگر (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", این);
	بازگشت !processes.empty();
}

منطقی RTLIL::Module::is_selected() ثابت
{
	بازگشت design->selected_module(این->name);
}

منطقی RTLIL::Module::is_selected_whole() ثابت
{
	بازگشت design->selected_whole_module(این->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() ثابت
{
	std::vector<RTLIL::Wire*> result;
	result.reserve(wires_.size());
	برای (auto &it : wires_)
		اگر (design->selected(این, it.second))
			result.افزودن_back(it.second);
	بازگشت result;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() ثابت
{
	std::vector<RTLIL::Cell*> result;
	result.reserve(cells_.size());
	برای (auto &it : cells_)
		اگر (design->selected(این, it.second))
			result.افزودن_back(it.second);
	بازگشت result;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() ثابت
{
	std::vector<RTLIL::Memory*> result;
	result.reserve(memories.size());
	برای (auto &it : memories)
		اگر (design->selected(این, it.second))
			result.افزودن_back(it.second);
	بازگشت result;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() ثابت
{
	std::vector<RTLIL::Process*> result;
	result.reserve(processes.size());
	برای (auto &it : processes)
		اگر (design->selected(این, it.second))
			result.افزودن_back(it.second);
	بازگشت result;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() ثابت
{
	std::vector<RTLIL::SelectedMember> result;
	برای (auto cell : selected_cells())
		result.افزودن_back({cell, cell->name});
	برای (auto memory : selected_memories())
		result.افزودن_back({memory, memory->name});
	برای (auto wire : selected_wires())
		result.افزودن_back({wire, wire->name});
	برای (auto process : selected_processes())
		result.افزودن_back({process, process->name});
	بازگشت result;
}

تهی RTLIL::Module::add(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(شمار_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = این;
}

تهی RTLIL::Module::add(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(شمار_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = این;
}

تهی RTLIL::Module::add(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(شمار_id(process->name) == 0);
	processes[process->name] = process;
	process->module = این;
}

تهی RTLIL::Module::remove(ثابت pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	اگر (wires.empty())
		بازگشت;

	ساختار DeleteWireWorker
	{
		RTLIL::Module *module;
		ثابت pool<RTLIL::Wire*> *wires_p;

		تهی operator()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([این](RTLIL::Wire *&wire) {
				اگر (wires_p->شمار(wire))
					wire = module->addWire(stringf("$delete_wire$%d", (صحیح)autoidx++), wire->width);
			});
		}

		تهی operator()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	DeleteWireWorker delete_wire_worker;
	delete_wire_worker.module = این;
	delete_wire_worker.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_worker);

	اگر (design->flagBufferedNormalized) {
		برای (auto wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_index.erase(wire);
		}
	}

	برای (auto &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.شمار(id) != 0);
		wires_.erase(id);
		delete it;
	}
}

تهی RTLIL::Module::remove(RTLIL::Cell *cell)
{
	وقتی (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.شمار(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	اگر (design && design->flagBufferedNormalized && buf_norm_cell_queue.شمار(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.درج(cell);
	} وگرنه {
		delete cell;
	}
}

تهی RTLIL::Module::remove(RTLIL::Memory *memory)
{
	log_assert(memories.شمار(memory->name) != 0);
	memories.erase(memory->name);
	delete memory;
}

تهی RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.شمار(process->name) != 0);
	processes.erase(process->name);
	delete process;
}

تهی RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	add(wire);
}

تهی RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	add(cell);
}

تهی RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	اگر (old_id != IdString::Null && wires_.شمار(old_id))
		rename(wires_.at(old_id), new_name);
	وگرنه اگر (old_id != IdString::Null && cells_.شمار(old_id))
		rename(cells_.at(old_id), new_name);
	وگرنه
		log_abort();
}

تهی RTLIL::Module::swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2)
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

تهی RTLIL::Module::swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	صحیح index = 0;
	بازگشت uniquify(name, index);
}

IdString RTLIL::Module::uniquify(IdString name, صحیح &index)
{
	اگر (index == 0) {
		اگر (شمار_id(name) == 0)
			بازگشت name;
		index++;
	}

	وقتی (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", index)});
		اگر (شمار_id(new_name) == 0)
			بازگشت new_name;
		index++;
	}
}

ایستا منطقی fixup_ports_compare(ثابت RTLIL::Wire *a, ثابت RTLIL::Wire *b)
{
	اگر (a->port_id && !b->port_id)
		بازگشت درست;
	اگر (!a->port_id && b->port_id)
		بازگشت نادرست;

	اگر (a->port_id == b->port_id)
		بازگشت a->name < b->name;
	بازگشت a->port_id < b->port_id;
}

تهی RTLIL::Module::connect(ثابت RTLIL::SigSig &conn)
{
	برای (auto mon : monitors)
		mon->notify_connect(این, conn);

	اگر (design)
		برای (auto mon : design->monitors)
			mon->notify_connect(این, conn);

	// ignore all attempts to assign constants to other constants
	اگر (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		برای (صحیح i = 0; i < GetSize(conn.first); i++)
			اگر (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		اگر (GetSize(new_conn.first))
			connect(new_conn);
		بازگشت;
	}

	اگر (yosys_xtrace) {
		log("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", این, log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(GetSize(conn.first) == GetSize(conn.second));
	connections_.افزودن_back(conn);
}

تهی RTLIL::Module::connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

تهی RTLIL::Module::new_connections(ثابت std::vector<RTLIL::SigSig> &new_conn)
{
	برای (auto mon : monitors)
		mon->notify_connect(این, new_conn);

	اگر (design)
		برای (auto mon : design->monitors)
			mon->notify_connect(این, new_conn);

	اگر (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", این);
		برای (auto &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ثابت std::vector<RTLIL::SigSig> &RTLIL::Module::connections() ثابت
{
	بازگشت connections_;
}

تهی RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	برای (auto &w : wires_)
		اگر (w.second->port_درون_داد || w.second->port_output)
			all_ports.افزودن_back(w.second);
		وگرنه
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	اگر (design && design->flagBufferedNormalized) {
		برای (auto &w : wires_)
			اگر (w.second->driverCell_ && w.second->driverCell_->type == ID($درون_داد_port))
				buf_norm_wire_queue.درج(w.second);

		buf_norm_wire_queue.درج(all_ports.begin(), all_ports.end());
	}

	ports.clear();
	برای (size_t i = 0; i < all_ports.size(); i++) {
		ports.افزودن_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
	}
}

تهی RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		ثابت dict<IdString, RTLIL::Const> &src,
		ثابت RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	اگر (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		بازگشت;
	}
	dst.clear();
	برای (صحیح i = GetSize(src) - 1; i >= 0; i--) {
		auto &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::addWire(RTLIL::IdString name, صحیح width)
{
	log_assert(design);
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	RTLIL::Wire *wire = جدید RTLIL::Wire;
	wire->width = width;
	wire->name = name;
	add(wire);
	بازگشت wire;
}

RTLIL::Wire *RTLIL::Module::addWire(IdString name, ثابت RTLIL::Wire *other)
{
	RTLIL::Wire *wire = addWire(name);
	wire->width = other->width;
	wire->start_offset = other->start_offset;
	wire->port_id = other->port_id;
	wire->port_درون_داد = other->port_درون_داد;
	wire->port_output = other->port_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ثابت RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
		RTLIL::copy_attr_dict(wire->attributes, other->attributes, src_design, این->design);
	}
	بازگشت wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = جدید RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	بازگشت cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, ثابت RTLIL::Cell *other)
{
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	منطقی cross_pool = src_design && این->design && src_design != این->design;

	IdString type = other->type.ref();
	اگر (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, این->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, این->design);

	اگر (!cross_pool)
		cell->connections_ = other->connections_;
	وگرنه
		برای (صحیح i = GetSize(other->connections_) - 1; i >= 0; i--) {
			auto &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	بازگشت cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = جدید RTLIL::Memory;
	mem->module = این;
	mem->name = name;
	memories[name] = mem;
	بازگشت mem;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name, ثابت RTLIL::Memory *other)
{
	log_assert(design);
	RTLIL::Memory *mem = جدید RTLIL::Memory;
	mem->module = این;
	mem->name = name;
	mem->width = other->width;
	mem->start_offset = other->start_offset;
	mem->size = other->size;
	RTLIL::copy_attr_dict(mem->attributes, other->attributes,
			other->module ? other->module->design : nullptr, design);
	memories[name] = mem;
	بازگشت mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = جدید RTLIL::Process;
	proc->name = name;
	add(proc);
	بازگشت proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, ثابت RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	اگر (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		برای (auto *همگام : proc->syncs)
			برای (auto &mwa : همگام->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	بازگشت proc;
}

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		بازگشت cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		بازگشت sig_y;                                       \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, منطقی /* is_signed */, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::WIDTH] = sig_a.size();         \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		بازگشت cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		بازگشت sig_y;                                       \
	}
DEF_METHOD(Buf,     sig_a.size(), ID($buf))
DEF_METHOD(Barrier, sig_a.size(), ID($barrier))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقی is_signed, ثابت std::string &src) { \
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
		بازگشت cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		بازگشت sig_y;                                            \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = نادرست;             \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		بازگشت cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		بازگشت sig_y;                                            \
	}
DEF_METHOD(Shl,      sig_a.size(), ID($shl))
DEF_METHOD(Shr,      sig_a.size(), ID($shr))
DEF_METHOD(Sshl,     sig_a.size(), ID($sshl))
DEF_METHOD(Sshr,     sig_a.size(), ID($sshr))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = نادرست;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		بازگشت cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقی is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		بازگشت sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.size(), ID($shiftx))
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		اگر (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.size();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		بازگشت cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		بازگشت sig_y;                                             \
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
		بازگشت cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.size() << sig_s.size() : sig_a.size() >> sig_s.size()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		بازگشت sig_y;                                             \
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
		بازگشت cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		بازگشت sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#undef DEF_METHOD

#define DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		بازگشت cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		بازگشت sig2;                                      \
	}
#define DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت RTLIL::SigBit &sig3, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		بازگشت cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		بازگشت sig3;                                      \
	}
#define DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت RTLIL::SigBit &sig3, ثابت RTLIL::SigBit &sig4, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		بازگشت cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت RTLIL::SigBit &sig3, ثابت std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		بازگشت sig4;                                      \
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
		بازگشت cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت RTLIL::SigBit &sig3, ثابت RTLIL::SigBit &sig4, ثابت std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		بازگشت sig5;                                           \
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

RTLIL::Cell* RTLIL::Module::addPow(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقی a_signed, منطقی b_signed, ثابت std::string &src)
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
	بازگشت cell;
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
	بازگشت cell;
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
	بازگشت cell;
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
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($أكد));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addSr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr, ثابت RTLIL::SigSpec &sig_q, منطقی set_polarity, منطقی clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sr));
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addDff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addDffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, منطقی en_polarity, ثابت std::string &src)
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
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addDffsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, منطقی set_polarity, منطقی clr_polarity, ثابت std::string &src)
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
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addDffsre(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, منطقی en_polarity, منطقی set_polarity, منطقی clr_polarity, ثابت std::string &src)
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
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addAdff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_مقدار, منطقی clk_polarity, منطقی arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_مقدار] = arst_مقدار;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addAdffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_مقدار, منطقی clk_polarity, منطقی en_polarity, منطقی arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_مقدار] = arst_مقدار;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addAldff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقی clk_polarity, منطقی aload_polarity, ثابت std::string &src)
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
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addAldffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقی clk_polarity, منطقی en_polarity, منطقی aload_polarity, ثابت std::string &src)
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
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addSdff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_مقدار, منطقی clk_polarity, منطقی srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_مقدار] = srst_مقدار;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addSdffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_مقدار, منطقی clk_polarity, منطقی en_polarity, منطقی srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_مقدار] = srst_مقدار;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addSdffce(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_مقدار, منطقی clk_polarity, منطقی en_polarity, منطقی srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_مقدار] = srst_مقدار;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addDlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_مقدار, منطقی en_polarity, منطقی arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_مقدار] = arst_مقدار;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsr(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی en_polarity, منطقی set_polarity, منطقی clr_polarity, ثابت std::string &src)
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
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		ثابت RTLIL::SigSpec &sig_q, منطقی set_polarity, منطقی clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, منطقی en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addDffsrGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, منطقی set_polarity, منطقی clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addDffsreGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی clk_polarity, منطقی en_polarity, منطقی set_polarity, منطقی clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addAdffGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقی arst_مقدار, منطقی clk_polarity, منطقی arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_مقدار ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addAdffeGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقی arst_مقدار, منطقی clk_polarity, منطقی en_polarity, منطقی arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_مقدار ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addAldffGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقی clk_polarity, منطقی aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addAldffeGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		ثابت RTLIL::SigSpec &sig_ad, منطقی clk_polarity, منطقی en_polarity, منطقی aload_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addSdffGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقی srst_مقدار, منطقی clk_polarity, منطقی srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_مقدار ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addSdffeGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقی srst_مقدار, منطقی clk_polarity, منطقی en_polarity, منطقی srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_مقدار ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addSdffceGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقی srst_مقدار, منطقی clk_polarity, منطقی en_polarity, منطقی srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_مقدار ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatchGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقی arst_مقدار, منطقی en_polarity, منطقی arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_مقدار ? '1' : '0'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsrGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, منطقی en_polarity, منطقی set_polarity, منطقی clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	بازگشت cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, صحیح width, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	بازگشت sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, صحیح width, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	بازگشت sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, صحیح width, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	بازگشت sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, صحیح width, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	بازگشت sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	بازگشت sig;
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
	بازگشت sig;
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
	بازگشت cell;
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
	بازگشت sig;
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
	بازگشت cell;
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
	بازگشت sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.size());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.size();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	بازگشت sig;
}

std::string RTLIL::Module::to_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(این), design, نادرست, درست, نادرست, RTLIL_BACKEND::DumpMode::Readable);
	بازگشت f.str();
}

RTLIL::Wire::Wire()
{
	ایستا unsigned صحیح hashidx_شمار = 123456789;
	hashidx_شمار = mkhash_xorshift(hashidx_شمار);
	hashidx_ = hashidx_شمار;

	module = nullptr;
	width = 1;
	start_offset = 0;
	port_id = 0;
	port_درون_داد = نادرست;
	port_output = نادرست;
	upto = نادرست;
	is_signed = نادرست;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->درج(std::pair<unsigned صحیح, RTLIL::Wire*>(hashidx_, این));
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
	RTLIL_BACKEND::dump_wire(f, "", این, design(), RTLIL_BACKEND::DumpMode::Readable);
	بازگشت f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
ایستا std::map<unsigned صحیح, RTLIL::Wire*> all_wires;
std::map<unsigned صحیح, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(تهی)
{
	بازگشت &all_wires;
}
#endif

RTLIL::Memory::Memory()
{
	ایستا unsigned صحیح hashidx_شمار = 123456789;
	hashidx_شمار = mkhash_xorshift(hashidx_شمار);
	hashidx_ = hashidx_شمار;

	width = 1;
	start_offset = 0;
	size = 0;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->درج(std::pair<unsigned صحیح, RTLIL::Memory*>(hashidx_, این));
#endif
}

std::string RTLIL::Memory::to_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", این, design(), RTLIL_BACKEND::DumpMode::Readable);
	بازگشت f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	ایستا unsigned صحیح hashidx_شمار = 123456789;
	hashidx_شمار = mkhash_xorshift(hashidx_شمار);
	hashidx_ = hashidx_شمار;
}

std::string RTLIL::Process::to_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", این, design(), RTLIL_BACKEND::DumpMode::Readable);
	بازگشت f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	ایستا unsigned صحیح hashidx_شمار = 123456789;
	hashidx_شمار = mkhash_xorshift(hashidx_شمار);
	hashidx_ = hashidx_شمار;

	// log("#memtrace# %p\n", this);
	memhasher();

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->درج(std::pair<unsigned صحیح, RTLIL::Cell*>(hashidx_, این));
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
	RTLIL_BACKEND::dump_cell(f, "", این, design(), RTLIL_BACKEND::DumpMode::Readable);
	بازگشت f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
ایستا std::map<unsigned صحیح, RTLIL::Cell*> all_cells;
std::map<unsigned صحیح, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(تهی)
{
	بازگشت &all_cells;
}
#endif

منطقی RTLIL::Cell::hasPort(RTLIL::IdString portname) ثابت
{
	بازگشت connections_.شمار(portname) != 0;
}

// bufnorm

ثابت RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) ثابت
{
	بازگشت connections_.at(portname);
}

ثابت dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() ثابت
{
	بازگشت connections_;
}

منطقی RTLIL::Cell::known() ثابت
{
	اگر (yosys_celltypes.cell_known(type))
		بازگشت درست;
	اگر (module && module->design && module->design->module(type))
		بازگشت درست;
	بازگشت نادرست;
}

منطقی RTLIL::Cell::درون_داد(IdString portname) ثابت
{
	اگر (yosys_celltypes.cell_known(type))
		بازگشت yosys_celltypes.cell_درون_داد(type.ref(), portname);
	اگر (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		بازگشت w && w->port_درون_داد;
	}
	بازگشت نادرست;
}

منطقی RTLIL::Cell::output(RTLIL::IdString portname) ثابت
{
	اگر (yosys_celltypes.cell_known(type))
		بازگشت yosys_celltypes.cell_output(type, portname);
	اگر (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		بازگشت w && w->port_output;
	}
	بازگشت نادرست;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) ثابت
{
	اگر (yosys_celltypes.cell_known(type.ref()))
		بازگشت yosys_celltypes.cell_port_dir(type, portname);
	اگر (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		اگر (m == nullptr)
			بازگشت PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		اگر (w == nullptr)
			بازگشت PortDir::PD_UNKNOWN;
		بازگشت PortDir(w->port_درون_داد + w->port_output * 2);
	}
	بازگشت PortDir::PD_UNKNOWN;
}

منطقی RTLIL::Cell::hasParam(RTLIL::IdString paramname) ثابت
{
	بازگشت parameters.شمار(paramname) != 0;
}

تهی RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

تهی RTLIL::Cell::setParam(RTLIL::IdString paramname, RTLIL::Const مقدار)
{
	parameters[paramname] = std::move(مقدار);
}

ثابت RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ثابت
{
	ثابت auto &it = parameters.find(paramname);
	اگر (it != parameters.end())
		بازگشت it->second;
	اگر (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		اگر (m)
			بازگشت m->parameter_default_مقادیر.at(paramname);
	}
	پرتابکن std::out_of_range("Cell::getParam()");
}

تهی RTLIL::Cell::sort()
{
	connections_.sort(sort_by_id_str(module->twines()));
	parameters.sort(sort_by_id_str(module->twines()));
	attributes.sort(sort_by_id_str(module->twines()));
}

تهی RTLIL::Cell::check()
{
#ifndef NDEBUG
	InternalCellChecker checker(پوچ, این);
	checker.check();
#endif
}

تهی RTLIL::Cell::fixup_parameters(منطقی set_a_signed, منطقی set_b_signed)
{
	اگر (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		بازگشت;

	اگر (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		اگر (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		بازگشت;
	}

	اگر (type == ID($demux)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		بازگشت;
	}

	اگر (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		بازگشت;
	}

	اگر (type == ID($fa)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		بازگشت;
	}

	اگر (type == ID($lcu)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::CO]);
		بازگشت;
	}

	اگر (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);
		بازگشت;
	}

	منطقی signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	اگر (connections_.شمار(ID::A)) {
		اگر (signedness_ab) {
			اگر (set_a_signed)
				parameters[ID::A_SIGNED] = درست;
			وگرنه اگر (parameters.شمار(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = نادرست;
		}
		parameters[ID::A_WIDTH] = GetSize(connections_[ID::A]);
	}

	اگر (connections_.شمار(ID::B)) {
		اگر (signedness_ab) {
			اگر (set_b_signed)
				parameters[ID::B_SIGNED] = درست;
			وگرنه اگر (parameters.شمار(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = نادرست;
		}
		parameters[ID::B_WIDTH] = GetSize(connections_[ID::B]);
	}

	اگر (connections_.شمار(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);

	اگر (connections_.شمار(ID::Q))
		parameters[ID::WIDTH] = GetSize(connections_[ID::Q]);

	check();
}

منطقی RTLIL::Cell::has_keep_attr() ثابت {
	بازگشت get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

منطقی RTLIL::Cell::has_memid() ثابت
{
	بازگشت type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

منطقی RTLIL::Cell::is_mem_cell() ثابت
{
	بازگشت type.in(ID($mem), ID($mem_v2)) || has_memid();
}

منطقی RTLIL::Cell::is_builtin_ff() ثابت {
	بازگشت StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ثابت RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	اگر (wire == پوچ)
		data = {bit.data};
	وگرنه
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(صحیح offset, صحیح length) ثابت
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= width);
	RTLIL::SigChunk ret;
	اگر (wire) {
		ret.wire = wire;
		ret.offset = این->offset + offset;
		ret.width = length;
	} وگرنه {
		برای (صحیح i = 0; i < length; i++)
			ret.data.افزودن_back(data[offset+i]);
		ret.width = length;
	}
	بازگشت ret;
}

RTLIL::SigBit RTLIL::SigChunk::operator[](صحیح offset) ثابت
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	اگر (wire) {
		ret.wire = wire;
		ret.offset = این->offset + offset;
	} وگرنه {
		ret.data = data[offset];
	}
	بازگشت ret;
}

منطقی RTLIL::SigChunk::operator <(ثابت RTLIL::SigChunk &other) ثابت
{
	اگر (wire && other.wire)
		اگر (wire->name != other.wire->name)
			بازگشت wire->name < other.wire->name;

	اگر (wire != other.wire)
		بازگشت wire < other.wire;

	اگر (offset != other.offset)
		بازگشت offset < other.offset;

	اگر (width != other.width)
		بازگشت width < other.width;

	بازگشت data < other.data;
}

منطقی RTLIL::SigChunk::operator ==(ثابت RTLIL::SigChunk &other) ثابت
{
	بازگشت wire == other.wire && width == other.width && offset == other.offset && data == other.data;
}

منطقی RTLIL::SigChunk::operator !=(ثابت RTLIL::SigChunk &other) ثابت
{
	اگر (*این == other)
		بازگشت نادرست;
	بازگشت درست;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.size() > 0);
	auto ie = parts.begin();
	auto it = ie + parts.size() - 1;
	وقتی (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::Const &مقدار)
{
	اگر (GetSize(مقدار) != 0) {
		rep_ = CHUNK;
		جدید (&chunk_) RTLIL::SigChunk(مقدار);
	} وگرنه {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&مقدار)
{
	اگر (GetSize(مقدار) != 0) {
		rep_ = CHUNK;
		جدید (&chunk_) RTLIL::SigChunk(مقدار);
	} وگرنه {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::SigChunk &chunk)
{
	اگر (chunk.width != 0) {
		rep_ = CHUNK;
		جدید (&chunk_) RTLIL::SigChunk(chunk);
	} وگرنه {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	اگر (chunk.width != 0) {
		rep_ = CHUNK;
		جدید (&chunk_) RTLIL::SigChunk(chunk);
	} وگرنه {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	اگر (wire->width != 0) {
		rep_ = CHUNK;
		جدید (&chunk_) RTLIL::SigChunk(wire);
	} وگرنه {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, صحیح offset, صحیح width)
{
	اگر (width != 0) {
		rep_ = CHUNK;
		جدید (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} وگرنه {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::string &str)
{
	اگر (str.size() != 0) {
		rep_ = CHUNK;
		جدید (&chunk_) RTLIL::SigChunk(str);
	} وگرنه {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(صحیح val, صحیح width)
{
	اگر (width != 0) {
		rep_ = CHUNK;
		جدید (&chunk_) RTLIL::SigChunk(val, width);
	} وگرنه
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, صحیح width)
{
	اگر (width != 0) {
		rep_ = CHUNK;
		جدید (&chunk_) RTLIL::SigChunk(bit, width);
	} وگرنه
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::SigBit &bit, صحیح width)
{
	اگر (width != 0) {
		اگر (bit.wire == پوچ) {
			rep_ = CHUNK;
			جدید (&chunk_) RTLIL::SigChunk(bit.data, width);
		} وگرنه اگر (width == 1) {
			rep_ = CHUNK;
			جدید (&chunk_) RTLIL::SigChunk(bit);
		} وگرنه {
			init_empty_bits();
			bits_.reserve(width);
			برای (صحیح i = 0; i < width; i++)
				bits_.افزودن_back(bit);
		}
	} وگرنه
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	برای (ثابت auto &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	برای (ثابت auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	برای (ثابت auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	برای (ثابت auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(منطقی bit)
{
	rep_ = CHUNK;
	جدید (&chunk_) RTLIL::SigChunk(bit ? RTLIL::S1 : RTLIL::S0);
	check();
}

تهی RTLIL::SigSpec::Chunks::const_iterator::next_chunk_bits()
{
	صحیح bits_size = GetSize(spec.bits_);
	اگر (bit_index >= bits_size)
		بازگشت;
	صحیح i = bit_index;
	ثابت SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.data.clear();
	اگر (bit.is_wire()) {
		chunk.offset = bit.offset;
		وقتی (i < bits_size && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_index)
			++i;
	} وگرنه {
		chunk.offset = 0;
		chunk.data.افزودن_back(bit.data);
		وقتی (i < bits_size && !spec.bits_[i].is_wire()) {
			chunk.data.افزودن_back(spec.bits_[i].data);
			++i;
		}
	}
	chunk.width = i - bit_index;
}

تهی RTLIL::SigSpec::unpack()
{
	اگر (rep_ == BITS)
		بازگشت;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	برای (صحیح i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	جدید (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

تهی RTLIL::SigSpec::try_repack()
{
	اگر (rep_ != BITS)
		بازگشت;

	صحیح bits_size = GetSize(bits_);
	اگر (bits_size == 0)
		بازگشت;
	اگر (bits_[0].is_wire()) {
		برای (صحیح i = 1; i < bits_size; i++)
			اگر (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				بازگشت;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_size);
		bits_.~vector();
		rep_ = CHUNK;
		جدید (&chunk_) SigChunk(std::move(chunk));
		بازگشت;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_size);
	bits.افزودن_back(bits_[0].data);
	برای (صحیح i = 1; i < bits_size; i++) {
		اگر (bits_[i].is_wire())
			بازگشت;
		bits.افزودن_back(bits_[i].data);
	}
	bits_.~vector();
	rep_ = CHUNK;
	جدید (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ثابت
{
	Hasher h;
	برای (auto &c : chunks())
		اگر (c.wire == پوچ) {
			برای (auto &v : c.data)
				h.eat(v);
		} وگرنه {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t result = h.yield();
	اگر (result == 0)
		result = 1;
	hash_.set(result);
	بازگشت result;
}

تهی RTLIL::SigSpec::sort()
{
	unpack();
	std::sort(bits_.begin(), bits_.end());
	hash_.clear();
	try_repack();
}

تهی RTLIL::SigSpec::sort_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::vector<SigBit> unique_bits = bits_;
	std::sort(unique_bits.begin(), unique_bits.end());
	auto last = std::unique(unique_bits.begin(), unique_bits.end());
	unique_bits.erase(last, unique_bits.end());

	*این = unique_bits;
	hash_.clear();
	try_repack();
}

تهی RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &pattern, ثابت RTLIL::SigSpec &with)
{
	replace(pattern, with, این);
}

تهی RTLIL::SigSpec::replace(ثابت RTLIL::SigSpec &pattern, ثابت RTLIL::SigSpec &with, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != پوچ);
	log_assert(size() == other->size());
	log_assert(pattern.size() == with.size());

	dict<RTLIL::SigBit, صحیح> pattern_to_with;
	صحیح pattern_size = pattern.size();
	برای (صحیح i = 0; i < pattern_size; i++) {
		SigBit pattern_bit = pattern[i];
		اگر (pattern_bit.wire != پوچ) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	صحیح this_size = size();
	منطقی other_modified = نادرست;
	برای (صحیح j = 0; j < this_size; j++) {
		auto it = pattern_to_with.find((*این)[j]);
		اگر (it != pattern_to_with.end()) {
			other_modified = درست;
			(*other)[j] = with[it->second];
		}
	}

	اگر (other_modified)
		other->try_repack();
	other->check();
}

تهی RTLIL::SigSpec::replace(ثابت dict<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, این);
}

تهی RTLIL::SigSpec::replace(ثابت dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != پوچ);
	log_assert(size() == other->size());

	اگر (rules.empty()) بازگشت;

	صحیح this_size = size();
	منطقی other_modified = نادرست;
	برای (صحیح i = 0; i < this_size; i++) {
		auto it = rules.find((*این)[i]);
		اگر (it != rules.end()) {
			other_modified = درست;
			(*other)[i] = it->second;
		}
	}

	اگر (other_modified)
		other->try_repack();
	other->check();
}

تهی RTLIL::SigSpec::replace(ثابت std::map<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, این);
}

تهی RTLIL::SigSpec::replace(ثابت std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ثابت
{
	log_assert(other != پوچ);
	log_assert(size() == other->size());

	اگر (rules.empty()) بازگشت;

	صحیح this_size = size();
	منطقی other_modified = نادرست;
	برای (صحیح i = 0; i < this_size; i++) {
		auto it = rules.find((*این)[i]);
		اگر (it != rules.end()) {
			other_modified = درست;
			(*other)[i] = it->second;
		}
	}

	اگر (other_modified)
		other->try_repack();
	other->check();
}

تهی RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &pattern)
{
	remove2(pattern, پوچ);
}

تهی RTLIL::SigSpec::remove(ثابت RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *این;
	tmp.remove2(pattern, other);
}

تهی RTLIL::SigSpec::remove2(ثابت RTLIL::SigSpec &pattern, RTLIL::SigSpec *other)
{
	unpack();
	اگر (other != پوچ) {
		log_assert(size() == other->size());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.size());
	برای (auto &bit : pattern)
		اگر (bit.wire != پوچ)
			pattern_bits.درج(bit);

	// Compact in-place to avoid O(n^2) erase operations
	size_t write_idx = 0;
	برای (size_t read_idx = 0; read_idx < bits_.size(); read_idx++)
	{
		اگر (!(bits_[read_idx].wire != پوچ && pattern_bits.شمار(bits_[read_idx]))) {
			اگر (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				اگر (other != پوچ)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	منطقی modified = (write_idx < bits_.size());
	اگر (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	اگر (other != پوچ && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

تهی RTLIL::SigSpec::remove(ثابت pool<RTLIL::SigBit> &pattern)
{
	remove2(pattern, پوچ);
}

تهی RTLIL::SigSpec::remove(ثابت pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) ثابت
{
	RTLIL::SigSpec tmp = *این;
	tmp.remove2(pattern, other);
}

تهی RTLIL::SigSpec::remove2(ثابت pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	اگر (other != پوچ) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	برای (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		اگر (!(bits_[read_idx].wire != پوچ && pattern.شمار(bits_[read_idx]))) {
			اگر (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				اگر (other != پوچ)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	منطقی modified = (write_idx < bits_.size());
	اگر (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	اگر (other != پوچ && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

تهی RTLIL::SigSpec::remove2(ثابت std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	اگر (other != پوچ) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	برای (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		اگر (!(bits_[read_idx].wire != پوچ && pattern.شمار(bits_[read_idx]))) {
			اگر (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				اگر (other != پوچ)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	منطقی modified = (write_idx < bits_.size());
	اگر (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	اگر (other != پوچ && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

تهی RTLIL::SigSpec::remove2(ثابت pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	اگر (other != پوچ) {
		log_assert(size() == other->size());
		other->unpack();
	}

	منطقی modified = نادرست;
	منطقی other_modified = نادرست;
	برای (صحیح i = GetSize(bits_) - 1; i >= 0; i--) {
		اگر (bits_[i].wire != پوچ && pattern.شمار(bits_[i].wire)) {
			modified = درست;
			bits_.erase(bits_.begin() + i);
			اگر (other != پوچ) {
				other_modified = درست;
				other->bits_.erase(other->bits_.begin() + i);
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

RTLIL::SigSpec RTLIL::SigSpec::extract(ثابت RTLIL::SigSpec &pattern, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == پوچ || size() == other->size());

	RTLIL::SigSpec ret;
	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();

	برای (auto& pattern_chunk : pattern.chunks()) {
		اگر (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			صحیح i = 0;
			برای (ثابت RTLIL::SigBit &bit : bits_match) {
				اگر (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} وگرنه {
			برای (ثابت RTLIL::SigBit &bit : bits_match)
				اگر (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	بازگشت ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ثابت pool<RTLIL::SigBit> &pattern, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == پوچ || size() == other->size());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	اگر (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		صحیح i = 0;
		برای (ثابت RTLIL::SigBit &bit : bits_match) {
			اگر (bit.wire && pattern.شمار(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} وگرنه {
		برای (ثابت RTLIL::SigBit &bit : bits_match)
			اگر (bit.wire && pattern.شمار(bit))
				ret.append(bit);
	}

	ret.check();
	بازگشت ret;
}

تهی RTLIL::SigSpec::replace(صحیح offset, ثابت RTLIL::SigSpec &with)
{
	اگر (with.size() == 0)
		بازگشت;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.size() >= 0);
	log_assert(offset+with.size() <= size());

	صحیح i = 0;
	برای (ثابت RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

تهی RTLIL::SigSpec::remove_const()
{
	اگر (rep_ == CHUNK)
	{
		اگر (chunk_.wire == پوچ) {
			chunk_.~SigChunk();
			init_empty_bits();
			hash_.clear();
		}
	}
	وگرنه
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.size());
		برای (auto &bit : bits_)
			اگر (bit.wire != پوچ)
				new_bits.افزودن_back(bit);
		اگر (GetSize(new_bits) != GetSize(bits_)) {
			bits_.swap(new_bits);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

تهی RTLIL::SigSpec::remove(صحیح offset, صحیح length)
{
	اگر (length == 0)
		بازگشت;

	unpack();

	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	bits_.erase(bits_.begin() + offset, bits_.begin() + offset + length);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(صحیح offset, صحیح length) ثابت
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	std::vector<SigBit> extracted;
	SigBit first;
	منطقی is_packing = درست;
	برای (صحیح i = offset; i < offset + length; i++) {
		منطقی was_packing_before = is_packing;
		SigBit bit = (*این)[i];
		اگر (i == offset) {
			first = bit;
			اگر (!bit.wire)
				is_packing = نادرست;
		} وگرنه {
			اگر (bit.wire != first.wire)
				is_packing = نادرست;
			اگر (bit.wire)
				اگر (bit.offset != first.offset + (i - offset))
					is_packing = نادرست;
		}
		اگر (was_packing_before && !is_packing)
			برای (صحیح j = offset; j < i; j++)
				extracted.افزودن_back((*این)[j]);
		اگر (!is_packing)
			extracted.افزودن_back((*این)[i]);
	}
	اگر (is_packing)
		بازگشت SigChunk(first.wire, first.offset, length);

	بازگشت extracted;
}

تهی RTLIL::SigSpec::rewrite_wires(std::function<تهی(RTLIL::Wire*& wire)> rewrite)
{
	اگر (rep_ == CHUNK) {
		اگر (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		بازگشت;
	}

	std::vector<RTLIL::SigBit> new_bits;
	برای (ثابت RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		اگر (c.wire != nullptr)
			rewrite(c.wire);
		برای (صحیح i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.clear();
}

تهی RTLIL::SigSpec::append(ثابت RTLIL::SigSpec &سیگنال)
{
	اگر (سیگنال.size() == 0)
		بازگشت;

	اگر (size() == 0) {
		*این = سیگنال;
		بازگشت;
	}

	hash_.clear();
	اگر (rep_ == CHUNK && سیگنال.rep_ == CHUNK && chunk_.wire == سیگنال.chunk_.wire) {
		اگر (chunk_.wire == پوچ) {
			chunk_.data.درج(chunk_.data.end(), سیگنال.chunk_.data.begin(), سیگنال.chunk_.data.end());
			chunk_.width = GetSize(chunk_.data);
			بازگشت;
		}
		اگر (chunk_.offset + chunk_.width == سیگنال.chunk_.offset) {
			chunk_.width += سیگنال.chunk_.width;
			بازگشت;
		}
	}

	unpack();
	برای (ثابت SigBit &bit : سیگنال.bits())
		bits_.افزودن_back(bit);
	check();
}

تهی RTLIL::SigSpec::append(ثابت RTLIL::SigBit &bit)
{
	hash_.clear();

	اگر (size() == 0) {
		destroy();
		rep_ = CHUNK;
		جدید (&chunk_) RTLIL::SigChunk(bit);
		بازگشت;
	}

	اگر (rep_ == CHUNK && chunk_.wire == bit.wire) {
		اگر (chunk_.wire == پوچ) {
			chunk_.data.افزودن_back(bit.data);
			chunk_.width++;
			بازگشت;
		}
		اگر (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			بازگشت;
		}
	}

	unpack();

	bits_.افزودن_back(bit);
	check();
}

تهی RTLIL::SigSpec::extend_u0(صحیح width, منطقی is_signed)
{
	اگر (size() > width)
		remove(width, size() - width);

	اگر (size() < width) {
		RTLIL::SigBit padding = size() > 0 ? (*این)[size() - 1] : RTLIL::State::Sx;
		اگر (!is_signed)
			padding = RTLIL::State::S0;
		وقتی (size() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(صحیح رقم) ثابت
{
	RTLIL::SigSpec sig;
	برای (صحیح i = 0; i < رقم; i++)
		sig.append(*این);
	بازگشت sig;
}

#ifndef NDEBUG
تهی RTLIL::SigSpec::check(ثابت Module *mod) ثابت
{
	اگر (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		اگر (chunk_.wire == پوچ) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.data.size() == (size_t)chunk_.width);
		} وگرنه {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.data.size() == 0);
			اگر (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	وگرنه اگر (size() <= 64)
	{
		اگر (mod != nullptr) {
			برای (ثابت RTLIL::SigBit &bit : bits_)
				اگر (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#endif

منطقی RTLIL::SigSpec::operator <(ثابت RTLIL::SigSpec &other) ثابت
{
	اگر (این == &other)
		بازگشت نادرست;

	اگر (size() != other.size())
		بازگشت size() < other.size();

	auto other_it = other.chunks().begin();
	برای (ثابت SigChunk &c : chunks()) {
		اگر (c != *other_it)
			بازگشت c < *other_it;
		++other_it;
	}

	بازگشت نادرست;
}

منطقی RTLIL::SigSpec::operator ==(ثابت RTLIL::SigSpec &other) ثابت
{
	اگر (این == &other)
		بازگشت درست;

	اگر (size() != other.size())
		بازگشت نادرست;

	auto other_it = other.chunks().begin();
	برای (ثابت SigChunk &c : chunks()) {
		اگر (c != *other_it)
			بازگشت نادرست;
		++other_it;
	}

	بازگشت درست;
}

منطقی RTLIL::SigSpec::is_wire() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	اگر (it == cs.end())
		بازگشت نادرست;
	ثابت RTLIL::SigChunk &chunk = *it;
	بازگشت chunk.wire && chunk.wire->width == size() && ++it == cs.end();
}

منطقی RTLIL::SigSpec::is_chunk() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	اگر (it == cs.end())
		بازگشت نادرست;
	بازگشت ++it == cs.end();
}

منطقی RTLIL::SigSpec::known_driver() ثابت
{
	برای (auto &chunk : chunks())
		اگر (chunk.is_wire() && !chunk.wire->known_driver())
			بازگشت نادرست;
	بازگشت درست;
}

منطقی RTLIL::SigSpec::is_fully_const() ثابت
{
	برای (auto &chunk : chunks())
		اگر (chunk.width > 0 && chunk.wire != پوچ)
			بازگشت نادرست;
	بازگشت درست;
}

منطقی RTLIL::SigSpec::is_fully_zero() ثابت
{
	برای (auto &chunk : chunks()) {
		اگر (chunk.width > 0 && chunk.wire != پوچ)
			بازگشت نادرست;
		برای (RTLIL::State d : chunk.data)
			اگر (d != RTLIL::State::S0)
				بازگشت نادرست;
	}
	بازگشت درست;
}

منطقی RTLIL::SigSpec::is_fully_ones() ثابت
{
	برای (auto &chunk : chunks()) {
		اگر (chunk.width > 0 && chunk.wire != پوچ)
			بازگشت نادرست;
		برای (RTLIL::State d : chunk.data)
			اگر (d != RTLIL::State::S1)
				بازگشت نادرست;
	}
	بازگشت درست;
}

منطقی RTLIL::SigSpec::is_fully_def() ثابت
{
	برای (auto &chunk : chunks()) {
		اگر (chunk.width > 0 && chunk.wire != پوچ)
			بازگشت نادرست;
		برای (RTLIL::State d : chunk.data)
			اگر (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				بازگشت نادرست;
	}
	بازگشت درست;
}

منطقی RTLIL::SigSpec::is_fully_undef() ثابت
{
	برای (auto &chunk : chunks()) {
		اگر (chunk.width > 0 && chunk.wire != پوچ)
			بازگشت نادرست;
		برای (RTLIL::State d : chunk.data)
			اگر (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				بازگشت نادرست;
	}
	بازگشت درست;
}

منطقی RTLIL::SigSpec::has_const() ثابت
{
	برای (auto &chunk : chunks())
		اگر (chunk.width > 0 && chunk.wire == پوچ)
			بازگشت درست;
	بازگشت نادرست;
}

منطقی RTLIL::SigSpec::has_const(State state) ثابت
{
	برای (auto &chunk : chunks())
		اگر (chunk.width > 0 && chunk.wire == پوچ && std::find(chunk.data.begin(), chunk.data.end(), state) != chunk.data.end())
			بازگشت درست;
	بازگشت نادرست;
}


منطقی RTLIL::SigSpec::has_marked_bits() ثابت
{
	برای (auto &chunk : chunks())
		اگر (chunk.width > 0 && chunk.wire == پوچ) {
			برای (RTLIL::State d : chunk.data)
				اگر (d == RTLIL::State::Sm)
					بازگشت درست;
		}
	بازگشت نادرست;
}

منطقی RTLIL::SigSpec::is_onehot(صحیح *pos) ثابت
{
	اگر (std::optional<RTLIL::Const> c = try_as_const())
		بازگشت c->is_onehot(pos);
	بازگشت نادرست;
}

منطقی RTLIL::SigSpec::as_bool() ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_مقدار());
	بازگشت c->as_bool();
}

صحیح RTLIL::SigSpec::as_int(منطقی is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_مقدار());
	بازگشت c->as_int(is_signed);
}

منطقی RTLIL::SigSpec::convertible_to_int(منطقی is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	اگر (!c.has_مقدار())
		بازگشت نادرست;
	بازگشت c->convertible_to_int(is_signed);
}

std::optional<صحیح> RTLIL::SigSpec::try_as_int(منطقی is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	اگر (!c.has_مقدار())
		بازگشت std::nullopt;
	بازگشت c->try_as_int(is_signed);
}

صحیح RTLIL::SigSpec::as_int_saturating(منطقی is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_مقدار());
	بازگشت c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ثابت
{
	std::string str;
	str.reserve(size());
	std::vector<RTLIL::SigChunk> chunks = *این;
	برای (size_t i = chunks.size(); i > 0; i--) {
		ثابت RTLIL::SigChunk &chunk = chunks[i-1];
		اگر (chunk.wire != پوچ)
			str.append(chunk.width, '?');
		وگرنه
			str += RTLIL::Const(chunk.data).as_string();
	}
	بازگشت str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	اگر (it == cs.end())
		بازگشت RTLIL::Const();
	SigChunk chunk = *it;
	اگر (chunk.wire != پوچ || ++it != cs.end())
		بازگشت std::nullopt;
	بازگشت RTLIL::Const(std::move(chunk.data));
}

RTLIL::Const RTLIL::SigSpec::as_const() ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_مقدار());
	بازگشت *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == size());
	بازگشت chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	بازگشت chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() ثابت
{
	بازگشت RTLIL::SigBit(*این);
}

منطقی RTLIL::SigSpec::match(ثابت نویسه* pattern) ثابت
{
	صحیح pattern_len = strlen(pattern);
	log_assert(pattern_len == size());

	برای (صحیح i = 0; i < pattern_len; ++i) {
		نویسه ch = pattern[i];
		اگر (ch == ' ')
			ادامه;
		RTLIL::SigBit bit = (*این)[pattern_len - 1 - i];
		اگر (ch == '*') {
			اگر (bit != State::Sz && bit != State::Sx)
				بازگشت نادرست;
			ادامه;
		}
		اگر (ch == '0') {
			اگر (bit != State::S0)
				بازگشت نادرست;
		} وگرنه
		اگر (ch == '1') {
			اگر (bit != State::S1)
				بازگشت نادرست;
		} وگرنه
			log_abort();
	}

	بازگشت درست;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() ثابت
{
	std::set<RTLIL::SigBit> sigbits;
	برای (auto &c : chunks())
		برای (صحیح i = 0; i < c.width; i++)
			sigbits.درج(RTLIL::SigBit(c, i));
	بازگشت sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() ثابت
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(size());
	برای (auto &c : chunks())
		برای (صحیح i = 0; i < c.width; i++)
			sigbits.درج(RTLIL::SigBit(c, i));
	بازگشت sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() ثابت
{
	std::vector<RTLIL::SigBit> result;
	result.reserve(size());
	برای (SigBit bit : *این)
		result.افزودن_back(bit);
	بازگشت result;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت
{
	صحیح this_size = size();
	log_assert(this_size == other.size());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	برای (صحیح i = 0; i < this_size; i++)
		new_map[(*این)[i]] = other[i];

	بازگشت new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت
{
	صحیح this_size = size();
	log_assert(this_size == other.size());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_size);
	برای (صحیح i = 0; i < this_size; i++)
		new_map[(*این)[i]] = other[i];

	بازگشت new_map;
}

ایستا تهی sigspec_parse_split(std::vector<std::string> &tokens, ثابت std::string &text, نویسه sep)
{
	size_t start = 0, end = 0;
	وقتی ((end = text.find(sep, start)) != std::string::npos) {
		tokens.افزودن_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.افزودن_back(text.substr(start));
}

ایستا RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ثابت std::string &netname)
{
	بازگشت module->wire(netname);
}

منطقی RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	برای (صحیح tokidx = صحیح(tokens.size())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		اگر (netname.size() == 0)
			ادامه;

		اگر (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			auto ast = p.const2ast(netname);
			اگر (ast == nullptr)
				بازگشت نادرست;
			sig.append(RTLIL::Const(ast->bits));
			ادامه;
		}

		اگر (module == پوچ)
			بازگشت نادرست;

		اگر (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		اگر (sigspec_parse_wire(module, netname) == nullptr) {
			size_t indices_pos = netname.size()-1;
			اگر (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				وقتی (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				اگر (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					وقتی (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				اگر (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		اگر (wire == nullptr)
			بازگشت نادرست;

		اگر (!indices.empty()) {
			std::vector<std::string> index_tokens;
			sigspec_parse_split(index_tokens, indices.substr(1, indices.size()-2), ':');
			اگر (index_tokens.size() == 1) {
				صحیح a = atoi(index_tokens.at(0).c_str());
				اگر (a < 0 || a >= wire->width)
					بازگشت نادرست;
				sig.append(RTLIL::SigSpec(wire, a));
			} وگرنه {
				صحیح a = atoi(index_tokens.at(0).c_str());
				صحیح b = atoi(index_tokens.at(1).c_str());
				اگر (a > b) {
					صحیح tmp = a;
					a = b, b = tmp;
				}
				اگر (a < 0 || a >= wire->width)
					بازگشت نادرست;
				اگر (b < 0 || b >= wire->width)
					بازگشت نادرست;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} وگرنه
			sig.append(wire);
	}

	بازگشت درست;
}

منطقی RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	اگر (str.empty() || str[0] != '@')
		بازگشت parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	اگر (sel_name == IdString::Null || design->selection_vars.شمار(sel_name) == 0)
		بازگشت نادرست;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	برای (auto &it : module->wires_)
		اگر (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	بازگشت درست;
}

منطقی RTLIL::SigSpec::parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	اگر (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.size());
		بازگشت درست;
	}

	اگر (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.size());
		بازگشت درست;
	}

	اگر (lhs.is_chunk()) {
		نویسه *p = (نویسه*)str.c_str(), *endptr;
		long صحیح val = strtol(p, &endptr, 10);
		اگر (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.size());
			بازگشت درست;
		}
	}

	اگر (!parse(sig, module, str))
		بازگشت نادرست;
	اگر (sig.size() > lhs.size())
		sig.remove(lhs.size(), sig.size() - lhs.size());
	بازگشت درست;
}

RTLIL::SigSpec::operator std::vector<RTLIL::SigChunk>() ثابت
{
	std::vector<RTLIL::SigChunk> result;
	برای (ثابت RTLIL::SigChunk &c : chunks())
		result.افزودن_back(c);
	بازگشت result;
}

RTLIL::CaseRule::~CaseRule()
{
	برای (auto it = switches.begin(); it != switches.end(); it++)
		delete *it;
}

منطقی RTLIL::CaseRule::empty() ثابت
{
	بازگشت actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() ثابت
{
	RTLIL::CaseRule *new_caserule = جدید RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	برای (auto &it : switches)
		new_caserule->switches.افزودن_back(it->clone());
	بازگشت new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	برای (auto it = cases.begin(); it != cases.end(); it++)
		delete *it;
}

منطقی RTLIL::SwitchRule::empty() ثابت
{
	بازگشت cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() ثابت
{
	RTLIL::SwitchRule *new_switchrule = جدید RTLIL::SwitchRule;
	new_switchrule->سیگنال = سیگنال;
	new_switchrule->attributes = attributes;
	برای (auto &it : cases)
		new_switchrule->cases.افزودن_back(it->clone());
	بازگشت new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() ثابت
{
	RTLIL::SyncRule *new_syncrule = جدید RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->سیگنال = سیگنال;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	بازگشت new_syncrule;
}

RTLIL::Process::~Process()
{
	برای (auto it = syncs.begin(); it != syncs.end(); it++)
		delete *it;
}

RTLIL::Process *RTLIL::Process::clone() ثابت
{
	RTLIL::Process *new_proc = جدید RTLIL::Process;

	new_proc->attributes = attributes;

	RTLIL::CaseRule *rc_ptr = ریشه_case.clone();
	new_proc->ریشه_case = *rc_ptr;
	rc_ptr->switches.clear();
	delete rc_ptr;

	برای (auto &it : syncs)
		new_proc->syncs.افزودن_back(it->clone());

	بازگشت new_proc;
}

RTLIL::Memory::~Memory()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
ایستا std::map<unsigned صحیح, RTLIL::Memory*> all_memorys;
std::map<unsigned صحیح, RTLIL::Memory*> *RTLIL::Memory::get_all_memorys(تهی)
{
	بازگشت &all_memorys;
}
#endif
YOSYS_NAMESPACE_END
