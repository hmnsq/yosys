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
		עבור (size_t i = 0; i < StaticCellTypes::builder.count; i++) {
			auto &cell = StaticCellTypes::builder.cells[i];
			אם (cell.features.is_ff)
				r.insert(cell.type);
		}
		החזר r;
	}();
	החזר res;
}

#define check(condition) log_assert(condition && "malformed Const union")

ثابت Const::bitvectype& Const::get_bits() ثابت {
	check(is_bits());
	החזר *get_if_bits();
}

ثابت std::string& Const::get_str() ثابت {
	check(is_str());
	החזר *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	החזר *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	החזר *get_if_str();
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
	אם ((width & 7) == 0) {
		جديد ((فراغ*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		صحيح bytes = width >> 3;
		signed حرف sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<صحيح>(bytes, حجم(val));
		עבור (صحيح i = 0; i < bytes; i++) {
			str[str.size() - 1 - i] = val;
			val = val >> 8;
		}
		החזר;
	}

	جديد ((فراغ*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	עבור (صحيح i = 0; i < width; i++) {
		bv.push_back((val & 1) != 0 ? State::S1 : State::S0);
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
	עבור (صحيح i = 0; i < width; i++)
		bv.push_back(bit);
}

RTLIL::Const::Const(ثابت std::vector<منطقي> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	جديد ((فراغ*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.size());
	עבור (ثابت auto &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ثابت RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	אם (is_str())
		جديد ((فراغ*)&str_) std::string(other.get_str());
	אחרת אם (is_bits())
		جديد ((فراغ*)&bits_) bitvectype(other.get_bits());
	אחרת
		check(שקר);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	אם (is_str())
		جديد ((فراغ*)&str_) std::string(std::move(other.get_str()));
	אחרת אם (is_bits())
		جديد ((فراغ*)&bits_) bitvectype(std::move(other.get_bits()));
	אחרת
		check(שקר);
}

RTLIL::Const &RTLIL::Const::operator =(ثابت RTLIL::Const &other) {
	flags = other.flags;
	אם (other.is_str()) {
		אם (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(فراغ)جديد ((فراغ*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} אחרת אם (other.is_bits()) {
		אם (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(فراغ)جديد ((فراغ*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} אחרת {
		check(שקר);
	}
	החזר *هذا;
}

RTLIL::Const::~Const() {
	אם (is_bits())
		bits_.~bitvectype();
	אחרת אם (is_str())
		str_.~string();
	אחרת
		check(שקר);
}

منطقي RTLIL::Const::operator<(ثابت RTLIL::Const &other) ثابت
{
	אם (size() != other.size())
		החזר size() < other.size();

	עבור (صحيح i = 0; i < size(); i++)
		אם ((*هذا)[i] != other[i])
			החזר (*هذا)[i] < other[i];

	החזר שקר;
}

منطقي RTLIL::Const::operator ==(ثابت RTLIL::Const &other) ثابت
{
	אם (is_str() && other.is_str())
		החזר get_str() == other.get_str();
	אם (is_bits() && other.is_bits())
		החזר get_bits() == other.get_bits();

	אם (size() != other.size())
		החזר שקר;

	עבור (صحيح i = 0; i < size(); i++)
	אם ((*هذا)[i] != other[i])
		החזר שקר;

	החזר אמת;
}

منطقي RTLIL::Const::operator !=(ثابت RTLIL::Const &other) ثابت
{
	החזר !(*هذا == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	החזר get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() ثابت
{
	std::vector<State> v;
	עבור (auto bit : *هذا)
		v.push_back(bit);
	החזר v;
}

منطقي RTLIL::Const::as_bool() ثابت
{
	אם (is_str()) {
		עבור (حرف ch : get_str())
			אם (ch != 0)
				החזר אמת;
		החזר שקר;
	}

	ثابت bitvectype& bv = get_bits();
	עבור (size_t i = 0; i < bv.size(); i++)
		אם (bv[i] == State::S1)
			החזר אמת;
	החזר שקר;
}

صحيح RTLIL::Const::as_int(منطقي is_signed) ثابت
{
	int32_t ret = 0;
	אם (ثابت std::string *s = get_if_str()) {
		صحيح size = GetSize(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		עבור (صحيح i = std::min(4, size); i > 0; i--)
			ret |= static_cast<unsigned حرف>((*s)[size - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		אם (is_signed && size > 0 && size < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << size*8;
		החזר ret;
	}

	ثابت bitvectype& bv = get_bits();
	صحيح significant_bits = std::min(GetSize(bv), 32);
	עבור (صحيح i = 0; i < significant_bits; i++)
		אם (bv[i] == State::S1)
			ret |= 1 << i;
	אם (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	החזר ret;
}

منطقي RTLIL::Const::convertible_to_int(منطقي is_signed) ثابت
{
	auto size = get_min_size(is_signed);

	אם (size < 0)
		החזר שקר;

	// If it fits in 31 bits it is definitely convertible
	אם (size <= 31)
		החזר אמת;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	אם (size == 32) {
		אם (is_signed)
			החזר אמת;
		החזר (*هذا)[size - 1] != State::S1;
	}

	החזר שקר;
}

std::optional<صحيح> RTLIL::Const::try_as_int(منطقي is_signed) ثابت
{
	אם (!convertible_to_int(is_signed))
		החזר std::nullopt;
	החזר as_int(is_signed);
}

صحيح RTLIL::Const::as_int_saturating(منطقي is_signed) ثابت
{
	אם (!convertible_to_int(is_signed)) {
		אם (!is_signed)
			החזר std::numeric_limits<صحيح>::max();

		ثابت auto min_size = get_min_size(is_signed);
		log_assert(min_size > 0);
		ثابت auto neg = (*هذا)[min_size - 1];
		החזר neg ? std::numeric_limits<صحيح>::min() : std::numeric_limits<صحيح>::max();
	}
	החזר as_int(is_signed);
}

فراغ RTLIL::Const::tag_bare_integer_const(ثابت std::string &value)
{
	אם (value.empty() || value.find('\'') != std::string::npos)
		החזר;
	size_t start = (value[0] == '-' || value[0] == '+') ? 1 : 0;
	אם (start < value.size() && std::all_of(value.begin() + start, value.end(), ::isdigit))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

صحيح RTLIL::Const::get_min_size(منطقي is_signed) ثابت
{
	אם (empty()) החזר 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	אם (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	אחרת
		leading_bit = RTLIL::State::S0;

	auto idx = size();
	כלעוד (idx > 0 && (*هذا)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	אם (is_signed && idx < size()) {
		idx++;
	}
	// must be at least one bit
	החזר (idx == 0) ? 1 : idx;
}

فراغ RTLIL::Const::compress(منطقي is_signed)
{
	auto idx = get_min_size(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<صحيح> RTLIL::Const::as_int_compress(منطقي is_signed) ثابت
{
	החזר try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ثابت حرف* any) ثابت
{
	صحيح sz = size();
	std::string ret;
	ret.reserve(sz);
	עבור (صحيح i = sz - 1; i >= 0; --i)
		بدل ((*هذا)[i]) {
			حالة S0: ret.push_back('0'); اكسر;
			حالة S1: ret.push_back('1'); اكسر;
			حالة Sx: ret.push_back('x'); اكسر;
			حالة Sz: ret.push_back('z'); اكسر;
			حالة Sa: ret += any; اكسر;
			حالة Sm: ret.push_back('m'); اكسر;
		}
	החזר ret;
}

RTLIL::Const RTLIL::Const::from_string(ثابت std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.size());
	עבור (auto it = str.rbegin(); it != str.rend(); it++)
		بدل (*it) {
			حالة '0': bv.push_back(State::S0); اكسر;
			حالة '1': bv.push_back(State::S1); اكسر;
			حالة 'x': bv.push_back(State::Sx); اكسر;
			حالة 'z': bv.push_back(State::Sz); اكسر;
			حالة 'm': bv.push_back(State::Sm); اكسر;
			افتراضي: bv.push_back(State::Sa);
		}
	החזר c;
}

std::string RTLIL::Const::decode_string() ثابت
{
	אם (auto str = get_if_str())
		החזר *str;

	ثابت bitvectype& bv = get_bits();
	ثابت صحيح n = GetSize(bv);
	ثابت صحيح n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	صحيح i = n_over_8 * 8;
	אם (i < n) {
		حرف ch = 0;
		עבור (صحيح j = 0; j < (n - i); j++) {
			אם (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		אם (ch != 0)
			s.append({ch});
	}
	i -= 8;
	עבור (; i >= 0; i -= 8) {
		حرف ch = 0;
		עבור (صحيح j = 0; j < 8; j++) {
			אם (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		אם (ch != 0)
			s.append({ch});
	}
	החזר s;
}

صحيح RTLIL::Const::size() ثابت {
	אם (is_str())
		החזר 8 * str_.size();
	אחרת {
		check(is_bits());
		החזר bits_.size();
	}
}

منطقي RTLIL::Const::empty() ثابت {
	אם (is_str())
		החזר str_.empty();
	אחרת {
		check(is_bits());
		החזר bits_.empty();
	}
}

فراغ RTLIL::Const::bitvectorize_internal() {
	אם (tag == backing_tag::bits)
		החזר;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.size() * 8);
	עבור (صحيح i = str_.size() - 1; i >= 0; i--) {
		unsigned حرف ch = str_[i];
		עבור (صحيح j = 0; j < 8; j++) {
			new_bits.push_back((ch & 1) != 0 ? State::S1 : State::S0);
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
	bv.insert(bv.end(), other.begin(), other.end());
}

RTLIL::State RTLIL::Const::const_iterator::operator*() ثابت {
	אם (auto bv = parent->get_if_bits())
		החזר (*bv)[idx];

	صحيح char_idx = parent->get_str().size() - idx / 8 - 1;
	منطقي bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	החזר bit ? State::S1 : State::S0;
}

منطقي RTLIL::Const::is_fully_zero() ثابت
{
	אם (auto str = get_if_str()) {
		עבור (حرف ch : *str)
			אם (ch != 0)
				החזר שקר;
		החזר אמת;
	}

	ثابت bitvectype& bv = get_bits();

	עבור (ثابت auto &bit : bv)
		אם (bit != RTLIL::State::S0)
			החזר שקר;

	החזר אמת;
}

منطقي RTLIL::Const::is_fully_ones() ثابت
{
	אם (auto str = get_if_str()) {
		עבור (حرف ch : *str)
			אם (ch != (حرف)0xff)
				החזר שקר;
		החזר אמת;
	}

	ثابت bitvectype& bv = get_bits();
	עבור (ثابت auto &bit : bv)
		אם (bit != RTLIL::State::S1)
			החזר שקר;

	החזר אמת;
}

منطقي RTLIL::Const::is_fully_def() ثابت
{
	אם (is_str())
		החזר אמת;

	ثابت bitvectype& bv = get_bits();
	עבור (ثابت auto &bit : bv)
		אם (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			החזר שקר;

	החזר אמת;
}

منطقي RTLIL::Const::is_fully_undef() ثابت
{
	אם (auto str = get_if_str())
		החזר str->empty();

	ثابت bitvectype& bv = get_bits();
	עבור (ثابت auto &bit : bv)
		אם (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			החזר שקר;

	החזר אמת;
}

منطقي RTLIL::Const::is_fully_undef_x_only() ثابت
{
	אם (auto str = get_if_str())
		החזר str->empty();

	ثابت bitvectype& bv = get_bits();
	עבור (ثابت auto &bit : bv)
		אם (bit != RTLIL::State::Sx)
			החזר שקר;

	החזר אמת;
}

منطقي RTLIL::Const::is_onehot(صحيح *pos) ثابت
{
	منطقي found = שקר;
	صحيح size = GetSize(*هذا);
	עבור (صحيح i = 0; i < size; i++) {
		State bit = (*هذا)[i];
		אם (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			החזר שקר;
		אם (bit == RTLIL::State::S1) {
			אם (found)
				החזר שקר;
			אם (pos)
				*pos = i;
			found = אמת;
		}
	}
	החזר found;
}

Hasher RTLIL::Const::hash_into(Hasher h) ثابت
{
	אם (auto str = get_if_str())
		החזר hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ثابت bitvectype &bv = get_bits();
	صحيح size = GetSize(bv);
	std::string packed;
	صحيح packed_size = (size + 7) >> 3;
	packed.resize(packed_size, 0);
	עבור (صحيح bi = 0; bi < packed_size; ++bi) {
		حرف ch = 0;
		صحيح end = std::min((bi + 1)*8, size);
		עבור (صحيح i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			אם (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<حرف>::hash_into(ch, h);
				עבור (; i < size; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(size);
				החזר h;
			}
			ch |= static_cast<صحيح>(b) << (i & 7);
		}
		packed[packed_size - 1 - bi] = ch;
	}
	החזר hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(صحيح offset, صحيح len, RTLIL::State padding) ثابت {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	עבור (صحيح i = offset; i < offset + len; i++)
		ret_bv.push_back(i < GetSize(*هذا) ? (*هذا)[i] : padding);
	החזר RTLIL::Const(ret_bv);
}
#undef check /* check(condition) for Const */

منطقي RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ثابت
{
	החזר attributes.count(id);
}

فراغ RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, منطقي value)
{
	אם (value)
		attributes[id] = RTLIL::Const(1);
	אחרת
		attributes.erase(id);
}

منطقي RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ثابت
{
	ثابت auto it = attributes.find(id);
	אם (it == attributes.end())
		החזר שקר;
	החזר it->second.as_bool();
}

فراغ RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string value)
{
	אם (value.empty())
		attributes.erase(id);
	אחרת
		attributes[id] = value;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ثابت
{
	std::string value;
	ثابت auto it = attributes.find(id);
	אם (it != attributes.end())
		value = it->second.decode_string();
	החזר value;
}

فراغ RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &data)
{
	string attrval;
	עבור (ثابت auto &s : data) {
		אם (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

فراغ RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &data)
{
	pool<string> union_data = get_strpool_attribute(id);
	union_data.insert(data.begin(), data.end());
	אם (!union_data.empty())
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
		std::vector<RTLIL::CaseRule*> case_stack{&process->root_case};
		כלעוד (!case_stack.empty()) {
			RTLIL::CaseRule *cs = case_stack.back();
			case_stack.pop_back();
			visit(cs);
			עבור (auto *sw : cs->switches) {
				visit(sw);
				עבור (auto *case_ : sw->cases)
					case_stack.push_back(case_);
			}
		}
		עבור (auto *متزامن : process->syncs)
			עבור (auto &mwa : متزامن->mem_write_actions)
				visit(&mwa);
	}

	template<typename F>
	فراغ walk_attr_objects(RTLIL::Design *design, F visit) {
		עבור (auto &[_, module] : design->modules_) {
			visit(module);
			עבור (auto &[_, wire] : module->wires_)
				visit(wire);
			עבור (auto &[_, mem] : module->memories)
				visit(mem);
			עבור (auto &[_, cell] : module->cells_)
				visit(cell);
			עבור (auto &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


size_t RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	auto root = [&](IdString ref) {
		אם (ref != IdString::Null)
			live.insert(ref);
	};

	walk_attr_objects(هذا, [&](ثابت RTLIL::AttrObject *obj) {
		עבור (auto &attr : obj->attributes)
			root(attr.first);
	});

	root(selected_active_module);

	עבור (auto &[name, module] : modules_) {
		root(name);
		עבור (IdString port : module->ports)
			root(port);
		עבור (IdString param : module->avail_parameters)
			root(param);
		עבור (auto &[param, _] : module->parameter_default_values)
			root(param);
		עבור (auto &[name, _] : module->memories)
			root(name);
		עבור (auto &[name, _] : module->processes)
			root(name);
		עבור (auto &[name, wire] : module->wires_) {
			root(name);
			אם (wire->known_driver())
				root(wire->driverPort());
		}
		עבור (auto &[name, cell] : module->cells_) {
			root(name);
			root(cell->type);
			עבור (auto &conn : cell->connections())
				root(conn.first);
			עבור (auto &param : cell->parameters)
				root(param.first);
		}
		עבור (auto &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_stack{&process->root_case};
			כלעוד (!case_stack.empty()) {
				RTLIL::CaseRule *cs = case_stack.back();
				case_stack.pop_back();
				עבור (auto *sw : cs->switches) {
					עבור (auto *case_ : sw->cases)
						case_stack.push_back(case_);
				}
			}
			עבור (auto *متزامن : process->syncs) {
				עבור (auto &mwa : متزامن->mem_write_actions)
					root(mwa.memid);
			}
		}
	}

	auto root_selection = [&](ثابت RTLIL::Selection &sel) {
		עבור (IdString m : sel.selected_modules)
			root(m);
		עבור (auto &[m, members] : sel.selected_members) {
			root(m);
			עבור (IdString member : members)
				root(member);
		}
	};

	עבור (auto &[name, sel] : selection_vars) {
		root(name);
		root_selection(sel);
	}

	עבור (auto &sel : selection_stack)
		root_selection(sel);

	size_t erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_count;
	החזר erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ثابت
{
	pool<string> data;
	אם (attributes.count(id) != 0)
		עבור (auto s : split_tokens(get_string_attribute(id), "|"))
			data.insert(s);
	החזר data;
}

فراغ RTLIL::AttrObject::set_hdlname_attribute(ثابت vector<string> &hierarchy)
{
	string attrval;
	עבור (ثابت auto &ident : hierarchy) {
		אם (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() ثابت
{
	החזר split_tokens(get_string_attribute(ID::hdlname), " ");
}

فراغ RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, ثابت vector<صحيح> &data)
{
	std::stringstream attrval;
	עבור (auto &i : data) {
		אם (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<صحيح> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ثابت
{
	vector<صحيح> data;
	auto it = attributes.find(id);
	אם (it != attributes.end())
		עבור (ثابت auto &s : split_tokens(attributes.at(id).decode_string())) {
			حرف *end = nullptr;
			errno = 0;
			long value = strtol(s.c_str(), &end, 10);
			אם (end != s.c_str() + s.size())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			אם (errno == ERANGE || value < INT_MIN || value > INT_MAX)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			data.push_back(value);
		}
	החזר data;
}

منطقي RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ثابت
{
	אם (current_design != nullptr) {
		auto module = current_design->module(mod_name);
		החזר module && module->get_blackbox_attribute();
	} אחרת {
		log_warning("Unable to check if module is boxed for null design.\n");
		החזר שקר;
	}
}

منطقي RTLIL::Selection::selected_module(RTLIL::IdString mod_name) ثابت
{
	אם (complete_selection)
		החזר אמת;
	אם (!selects_boxes && boxed_module(mod_name))
		החזר שקר;
	אם (full_selection)
		החזר אמת;
	אם (selected_modules.count(mod_name) > 0)
		החזר אמת;
	אם (selected_members.count(mod_name) > 0)
		החזר אמת;
	החזר שקר;
}

منطقي RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) ثابت
{
	אם (complete_selection)
		החזר אמת;
	אם (!selects_boxes && boxed_module(mod_name))
		החזר שקר;
	אם (full_selection)
		החזר אמת;
	אם (selected_modules.count(mod_name) > 0)
		החזר אמת;
	החזר שקר;
}

منطقي RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	אם (complete_selection)
		החזר אמת;
	אם (!selects_boxes && boxed_module(mod_name))
		החזר שקר;
	אם (full_selection)
		החזר אמת;
	אם (selected_modules.count(mod_name) > 0)
		החזר אמת;
	אם (selected_members.count(mod_name) > 0)
		אם (selected_members.at(mod_name).count(memb_name) > 0)
			החזר אמת;
	החזר שקר;
}

فراغ RTLIL::Selection::optimize(RTLIL::Design *design)
{
	אם (design != current_design) {
		current_design = design;
	}

	אם (selects_boxes && full_selection)
		complete_selection = אמת;
	אם (complete_selection) {
		full_selection = שקר;
		selects_boxes = אמת;
	}
	אם (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		החזר;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.clear();
	עבור (auto mod_name : selected_modules) {
		אם (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.push_back(mod_name);
		selected_members.erase(mod_name);
	}
	עבור (auto mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	עבור (auto &it : selected_members)
		אם (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.push_back(it.first);
	עבור (auto mod_name : del_list)
		selected_members.erase(mod_name);

	עבור (auto &it : selected_members) {
		del_list.clear();
		עבור (auto memb_name : it.second)
			אם (current_design->module(it.first)->count_id(memb_name) == 0)
				del_list.push_back(memb_name);
		עבור (auto memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	add_list.clear();
	עבור (auto &it : selected_members)
		אם (it.second.size() == 0)
			del_list.push_back(it.first);
		אחרת אם (it.second.size() == current_design->module(it.first)->wires_.size() + current_design->module(it.first)->memories.size() +
				current_design->module(it.first)->cells_.size() + current_design->module(it.first)->processes.size())
			add_list.push_back(it.first);
	עבור (auto mod_name : del_list)
		selected_members.erase(mod_name);
	עבור (auto mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.insert(mod_name);
	}

	אם (selected_modules.size() == current_design->modules_.size()) {
		selected_modules.clear();
		selected_members.clear();
		אם (selects_boxes)
			complete_selection = אמת;
		אחרת
			full_selection = אמת;
	}
}

فراغ RTLIL::Selection::clear()
{
	full_selection = שקר;
	complete_selection = שקר;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_defines (جديد define_map_t)
{
	ساكن unsigned صحيح hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	refcount_modules_ = 0;
	selected_active_module.clear();
	push_full_selection();

	RTLIL::Design::get_all_designs()->insert(std::pair<unsigned صحيح, RTLIL::Design*>(hashidx_, هذا));
}

RTLIL::Design::~Design()
{
	עבור (auto &pr : modules_)
		delete pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

ساكن std::map<unsigned صحيح, RTLIL::Design*> all_designs;
std::map<unsigned صحيح, RTLIL::Design*> *RTLIL::Design::get_all_designs(فراغ)
{
	החזר &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	החזר RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ثابت RTLIL::Module *RTLIL::Design::module(IdString id) ثابت {
	twines_.check_owned(id);
	החזר modules_.count(id) ? modules_.at(id) : عدم;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	החזר modules_.count(id) ? modules_.at(id) : عدم;
}

RTLIL::Module *RTLIL::Design::top_module() ثابت
{
	RTLIL::Module *module = nullptr;
	صحيح module_count = 0;

	עבור (auto mod : selected_modules()) {
		אם (mod->get_bool_attribute(ID::top))
			החזר mod;
		module_count++;
		module = mod;
	}

	החזר module_count == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	אם (modules_.count(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = جديد RTLIL::Module;
	modules_[name] = module;
	module->design = هذا;
	module->name = name;

	עבור (auto mon : monitors)
		mon->notify_module_add(module);

	אם (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	החזר module;
}

فراغ RTLIL::Design::scratchpad_unset(ثابت std::string &varname)
{
	scratchpad.erase(varname);
}

فراغ RTLIL::Design::scratchpad_set_int(ثابت std::string &varname, صحيح value)
{
	scratchpad[varname] = stringf("%d", value);
}

فراغ RTLIL::Design::scratchpad_set_bool(ثابت std::string &varname, منطقي value)
{
	scratchpad[varname] = value ? "true" : "false";
}

فراغ RTLIL::Design::scratchpad_set_string(ثابت std::string &varname, std::string value)
{
	scratchpad[varname] = std::move(value);
}

صحيح RTLIL::Design::scratchpad_get_int(ثابت std::string &varname, صحيح default_value) ثابت
{
	auto it = scratchpad.find(varname);
	אם (it == scratchpad.end())
		החזר default_value;

	ثابت std::string &str = it->second;

	אם (str == "0" || str == "false")
		החזר 0;

	אם (str == "1" || str == "true")
		החזר 1;

	حرف *endptr = nullptr;
	long صحيح parsed_value = strtol(str.c_str(), &endptr, 10);
	החזר *endptr ? default_value : parsed_value;
}

منطقي RTLIL::Design::scratchpad_get_bool(ثابت std::string &varname, منطقي default_value) ثابت
{
	auto it = scratchpad.find(varname);
	אם (it == scratchpad.end())
		החזר default_value;

	ثابت std::string &str = it->second;

	אם (str == "0" || str == "false")
		החזר שקר;

	אם (str == "1" || str == "true")
		החזר אמת;

	החזר default_value;
}

std::string RTLIL::Design::scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_value) ثابت
{
	auto it = scratchpad.find(varname);
	אם (it == scratchpad.end())
		החזר default_value;

	החזר it->second;
}

فراغ RTLIL::Design::remove(RTLIL::Module *module)
{
	עבור (auto mon : monitors)
		mon->notify_module_del(module);

	אם (yosys_xtrace) {
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
	עבור (auto &it : modules_)
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
	log_assert(!selection_stack.empty());
	صحيح pool_size = 0;
	עבור (auto &it : modules_)
		pool_size = std::max(pool_size, ThreadPool::work_pool_size(0, it.second->cells_size(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_size);
	עבור (auto &it : modules_) {
		log_assert(هذا == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#endif
}

فراغ RTLIL::Design::optimize()
{
	עבור (auto &it : modules_)
		it.second->optimize();
	עבור (auto &it : selection_stack)
		it.optimize(هذا);
	עבור (auto &it : selection_vars)
		it.second.optimize(هذا);
}

فراغ RTLIL::Design::clone_into(RTLIL::Design *dst) ثابت
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	עבור (auto it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

منطقي RTLIL::Design::selected_module(IdString mod_name) ثابت
{
	אם (!selected_active_module.empty() && mod_name != selected_active_module)
		החזר שקר;
	החזר selection().selected_module(mod_name);
}

منطقي RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) ثابت
{
	אם (!selected_active_module.empty() && mod_name != selected_active_module)
		החזר שקר;
	החזר selection().selected_whole_module(mod_name);
}

منطقي RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت
{
	אם (!selected_active_module.empty() && mod_name != selected_active_module)
		החזר שקר;
	החזר selection().selected_member(mod_name, memb_name);
}

منطقي RTLIL::Design::selected_module(RTLIL::Module *mod) ثابت
{
	החזר selected_module(mod->name);
}

منطقي RTLIL::Design::selected_whole_module(RTLIL::Module *mod) ثابت
{
	החזר selected_whole_module(mod->name);
}

فراغ RTLIL::Design::push_selection(RTLIL::Selection sel)
{
	sel.current_design = هذا;
	selection_stack.push_back(sel);
}

فراغ RTLIL::Design::push_empty_selection()
{
	push_selection(RTLIL::Selection::EmptySelection(هذا));
}

فراغ RTLIL::Design::push_full_selection()
{
	push_selection(RTLIL::Selection::FullSelection(هذا));
}

فراغ RTLIL::Design::push_complete_selection()
{
	push_selection(RTLIL::Selection::CompleteSelection(هذا));
}

فراغ RTLIL::Design::pop_selection()
{
	selection_stack.pop_back();
	// Default to a full_selection if we ran out of stack
	אם (selection_stack.empty())
		push_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(منطقي only_selected) ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(هذا), only_selected, אמת, שקר, RTLIL_BACKEND::DumpMode::Readable);
	החזר f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ثابت
{
	منطقي include_partials = partials == RTLIL::SELECT_ALL;
	منطقي exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	منطقي ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> result;
	result.reserve(modules_.size());
	עבור (auto &it : modules_) {
		אם (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			אם (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				result.push_back(it.second);
			אחרת
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
		} אחרת אם (!include_partials && selected_module(it.first)) {
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
	החזר result;
}

RTLIL::Module::Module()
{
	ساكن unsigned صحيح hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->insert(std::pair<unsigned صحيح, RTLIL::Module*>(hashidx_, هذا));
#endif
}

RTLIL::Module::~Module()
{
	עבור (auto &pr : wires_)
		delete pr.second;
	עבור (auto &pr : memories)
		delete pr.second;
	עבור (auto &pr : cells_)
		delete pr.second;
	עבור (auto &pr : processes)
		delete pr.second;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
ساكن std::map<unsigned صحيح, RTLIL::Module*> all_modules;
std::map<unsigned صحيح, RTLIL::Module*> *RTLIL::Module::get_all_modules(فراغ)
{
	החזר &all_modules;
}
#endif

فراغ RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	עבור (auto it = wires_.begin(); it != wires_.end(); ++it)
		אם (!it->second->port_input && !it->second->port_output)
			delwires.insert(it->second);

	עבור (auto it = memories.begin(); it != memories.end(); ++it)
		delete it->second;
	memories.clear();

	עבור (auto it = cells_.begin(); it != cells_.end(); ++it)
		delete it->second;
	cells_.clear();

	עבור (auto it = processes.begin(); it != processes.end(); ++it)
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
	החזר שקר;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ثابت dict<RTLIL::IdString, RTLIL::Const> &, منطقي mayfail)
{
	אם (mayfail)
		החזר IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ثابت dict<RTLIL::IdString, RTLIL::Const> &, ثابت dict<RTLIL::IdString, RTLIL::Module*> &, ثابت dict<RTLIL::IdString, RTLIL::IdString> &, منطقي mayfail)
{
	אם (mayfail)
		החזר IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

size_t RTLIL::Module::count_id(RTLIL::IdString id)
{
	החזר wires_.count(id) + cells_.count(id) + memories.count(id) + processes.count(id);
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

		فراغ error(صحيح linenr, std::optional<std::string> error_msg = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			אם (error_msg) {
				log_error("%s in internal cell %s\n", *error_msg, cell_loc);
			} אחרת {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		صحيح param(RTLIL::IdString name)
		{
			auto it = cell->parameters.find(name);
			אם (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.insert(name);
			החזר it->second.as_int();
		}

		صحيح param_bool(RTLIL::IdString name)
		{
			صحيح v = param(name);
			אם (GetSize(cell->parameters.at(name)) > 32)
				error(__LINE__);
			אם (v != 0 && v != 1)
				error(__LINE__);
			החזר v;
		}

		صحيح param_bool(RTLIL::IdString name, منطقي expected)
		{
			صحيح v = param_bool(name);
			אם (v != expected)
				error(__LINE__);
			החזר v;
		}

		فراغ param_bits(RTLIL::IdString name, صحيح width)
		{
			param(name);
			אם (GetSize(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			החזר cell->parameters.at(name).decode_string();
		}

		فراغ port(RTLIL::IdString name, صحيح width)
		{
			auto it = cell->connections_.find(name);
			אם (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			אם (GetSize(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.insert(name);
		}

		فراغ check_expected(منطقي check_matched_sign = שקר)
		{
			עבור (auto &para : cell->parameters)
				אם (expected_params.count(para.first) == 0)
					error(__LINE__);
			עבור (auto &conn : cell->connections())
				אם (expected_ports.count(conn.first) == 0)
					error(__LINE__);

			אם (check_matched_sign) {
				log_assert(expected_params.count(ID::A_SIGNED) != 0 && expected_params.count(ID::B_SIGNED) != 0);
				منطقي a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				منطقي b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				אם (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		فراغ check()
		{
			אם (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				החזר;

			אם (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(אמת);
				החזר;
			}

			אם (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/שקר);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/שקר);
				החזר;
			}

			אם (cell->type.in(ID($shift), ID($shiftx))) {
				אם (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/שקר);
				} אחרת {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/שקר);
				החזר;
			}

			אם (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(אמת);
				החזר;
			}

			אם (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($pow));
				החזר;
			}

			אם (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::CI, 1);
				port(ID::BI, 1);
				port(ID::X, param(ID::Y_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				port(ID::CO, param(ID::Y_WIDTH));
				check_expected(אמת);
				החזר;
			}

			אם (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				החזר;
			}

			אם (cell->type == ID($macc_v2)) {
				אם (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				אם (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				אם (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ثابت Const &a_width = cell->getParam(ID::A_WIDTHS);
				ثابت Const &b_width = cell->getParam(ID::B_WIDTHS);
				ثابت Const &c_width = cell->getParam(ID::C_WIDTHS);
				صحيح a_width_sum = 0, b_width_sum = 0, c_width_sum = 0;
				עבור (صحيح i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_sum += a_width.extract(16 * i, 16).as_int(שקר);
					b_width_sum += b_width.extract(16 * i, 16).as_int(שקר);
				}
				עבור (صحيح i = 0; i < param(ID::NADDENDS); i++) {
					c_width_sum += c_width.extract(16 * i, 16).as_int(שקר);
				}
				port(ID::A, a_width_sum);
				port(ID::B, b_width_sum);
				port(ID::C, c_width_sum);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/שקר);
				החזר;
			}

			אם (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				אם (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				החזר;
			}

			אם (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				החזר;
			}

			אם (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				החזר;
			}

			אם (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($dffsre)) {
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
				החזר;
			}

			אם (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_VALUE, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_VALUE, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type.in(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_VALUE, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_VALUE, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($aldffe)) {
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
				החזר;
			}

			אם (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_VALUE, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_BITS);
				param(ID::STATE_NUM);
				param(ID::STATE_NUM_LOG2);
				param(ID::STATE_RST);
				param_bits(ID::STATE_TABLE, param(ID::STATE_BITS) * param(ID::STATE_NUM));
				param(ID::TRANS_NUM);
				param_bits(ID::TRANS_TABLE, param(ID::TRANS_NUM) * (2*param(ID::STATE_NUM_LOG2) + param(ID::CTRL_IN_WIDTH) + param(ID::CTRL_OUT_WIDTH)));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::CTRL_IN, param(ID::CTRL_IN_WIDTH));
				port(ID::CTRL_OUT, param(ID::CTRL_OUT_WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_MASK);
				param(ID::COLLISION_X_MASK);
				param_bool(ID::CE_OVER_SRST);
				param_bits(ID::ARST_VALUE, param(ID::WIDTH));
				param_bits(ID::SRST_VALUE, param(ID::WIDTH));
				param_bits(ID::INIT_VALUE, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::SRST, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($memwr_v2)) {
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
				החזר;
			}

			אם (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::WORDS));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::WORDS));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($mem)) {
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
				החזר;
			}

			אם (cell->type == ID($mem_v2)) {
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
				param_bits(ID::RD_ARST_VALUE, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_SRST_VALUE, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_INIT_VALUE, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
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
				החזר;
			}

			אם (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				החזר;
			}

			אם (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type.in(ID($أكد), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				החזר;
			}

			אם (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				החזר;
			}

			אם (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				החזר;
			}

			אם (cell->type.in(ID($specify2), ID($specify3))) {
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
				אם (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
				}
				check_expected();
				החזר;
			}

			אם (cell->type == ID($specrule)) {
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
				החזר;
			}

			אם (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				החזר;
			}

			אם (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				אם (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
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
				החזר;
			}

			אם (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				אם (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				החזר;
			}

			אם (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); החזר; }

			אם (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); החזר; }

			אם (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); החזר; }
			אם (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); החזר; }

			אם (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); החזר; }

			אם (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  החזר; }

			אם (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); החזר; }

			אם (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); החזר; }

			אם (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); החזר; }

			אם (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); החזר; }

			אם (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); החזר; }

			אם (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); החזר; }

			אם (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); החזר; }

			אם (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); החזר; }

			אם (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); החזר; }

			אם (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); החזר; }

			אם (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); החזר; }

			אם (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); החזר; }

			אם (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); החזר; }

			אם (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				החזר;
			}
			אם (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				החזר;
			}
			אם (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				החזר;
			}
			אם (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				החזר;
			}
			אם (cell->type.in(ID($input_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				החזר;
			}
			אם (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				check_expected();
				החזר;
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
	parameter_default_values.sort(sort_by_id_str(design->twines()));
	memories.sort(sort_by_id_str(design->twines()));
	processes.sort(sort_by_id_str(design->twines()));
	עבור (auto &it : cells_)
		it.second->sort();
	עבור (auto &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	עבור (auto &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#ifndef NDEBUG
ساكن فراغ check_id_keys(ثابت dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	עבור (auto &it : keyed)
		log_assert(!it.first.empty());
}
#endif

فراغ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ifndef NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->cells_size(), 1000));
	ثابت RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	עבור (auto &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->width >= 0);
		log_assert(it.second->size >= 0);
		check_id_keys(it.second->attributes);
		memory_strings.insert(it.second->name.str());
	}

	std::vector<MonotonicFlag> ports_declared(GetSize(module->ports));
	ShardedVector<std::string> memids(subpool);
	subpool.run([const_module, &ports_declared, &memory_strings, &memids](ثابت ParallelDispatchThreadPool::RunCtx &ctx) {
		עבור (صحيح i : ctx.item_range(const_module->cells_size())) {
			auto it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			עבור (auto &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_keys(it.second->attributes);
			check_id_keys(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			אם (it.second->has_memid()) {
				log_assert(memory_strings.count(it.second->parameters.at(ID::MEMID).decode_string()));
			} אחרת אם (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.count(memid));
				memids.insert(ctx, std::move(memid));
			}
			auto cell_mod = const_module->design->module(it.second->name);
			אם (cell_mod != nullptr) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#ifdef __GNUC__
				#pragma GCC diagnostic push
				#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				#endif
				log_assert(!it.second->get_blackbox_attribute());
				#ifdef __GNUC__
				#pragma GCC diagnostic pop
				#endif
			}
		}

		עבור (صحيح i : ctx.item_range(const_module->wires_size())) {
			auto it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_keys(it.second->attributes);
			אם (it.second->port_id) {
				log_assert(GetSize(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_input || it.second->port_output);
				log_assert(it.second->port_id <= GetSize(ports_declared));
				منطقي previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == שקר);
			} אחרת
				log_assert(!it.second->port_input && !it.second->port_output);
		}
	});
	עבור (ثابت MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == אמת);
	pool<std::string> memids_pool;
	עבור (std::string &memid : memids)
		log_assert(memids_pool.insert(memid).second);

	עבור (auto &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->root_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->root_case};
		עבור (size_t i = 0; i < all_cases.size(); i++) {
			עבור (auto &switch_it : all_cases[i]->switches) {
				עבור (auto &case_it : switch_it->cases) {
					עבור (auto &compare_it : case_it->compare) {
						log_assert(switch_it->إشارة.size() == compare_it.size());
					}
					all_cases.push_back(case_it);
				}
			}
		}
		עבור (auto &sync_it : it.second->syncs) {
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

	עבור (auto &it : module->connections_) {
		log_assert(it.first.size() == it.second.size());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_keys(module->attributes);
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
	עבור (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_values.clear();
	עבור (auto &it : parameter_default_values)
		new_mod->parameter_default_values[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	עבור (auto &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	עבור (auto it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	עבור (auto it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	עבור (auto it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	עבור (auto it = processes.rbegin(); it != processes.rend(); ++it) {
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
				אם (it != wire_map.end())
					wire = it->second;
				אחרת
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
	החזר new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) ثابت
{
	RTLIL::Module *new_mod = جديد RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->add(new_mod);
	החזר new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) ثابت
{
	RTLIL::Module *new_mod = جديد RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	החזר new_mod;
}

منطقي RTLIL::Module::has_memories() ثابت
{
	החזר !memories.empty();
}

منطقي RTLIL::Module::has_processes() ثابت
{
	החזר !processes.empty();
}

منطقي RTLIL::Module::has_memories_warn() ثابت
{
	אם (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", هذا);
	החזר !memories.empty();
}

منطقي RTLIL::Module::has_processes_warn() ثابت
{
	אם (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", هذا);
	החזר !processes.empty();
}

منطقي RTLIL::Module::is_selected() ثابت
{
	החזר design->selected_module(هذا->name);
}

منطقي RTLIL::Module::is_selected_whole() ثابت
{
	החזר design->selected_whole_module(هذا->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() ثابت
{
	std::vector<RTLIL::Wire*> result;
	result.reserve(wires_.size());
	עבור (auto &it : wires_)
		אם (design->selected(هذا, it.second))
			result.push_back(it.second);
	החזר result;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() ثابت
{
	std::vector<RTLIL::Cell*> result;
	result.reserve(cells_.size());
	עבור (auto &it : cells_)
		אם (design->selected(هذا, it.second))
			result.push_back(it.second);
	החזר result;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() ثابت
{
	std::vector<RTLIL::Memory*> result;
	result.reserve(memories.size());
	עבור (auto &it : memories)
		אם (design->selected(هذا, it.second))
			result.push_back(it.second);
	החזר result;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() ثابت
{
	std::vector<RTLIL::Process*> result;
	result.reserve(processes.size());
	עבור (auto &it : processes)
		אם (design->selected(هذا, it.second))
			result.push_back(it.second);
	החזר result;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() ثابت
{
	std::vector<RTLIL::SelectedMember> result;
	עבור (auto cell : selected_cells())
		result.push_back({cell, cell->name});
	עבור (auto memory : selected_memories())
		result.push_back({memory, memory->name});
	עבור (auto wire : selected_wires())
		result.push_back({wire, wire->name});
	עבור (auto process : selected_processes())
		result.push_back({process, process->name});
	החזר result;
}

فراغ RTLIL::Module::add(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(count_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = هذا;
}

فراغ RTLIL::Module::add(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(count_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = هذا;
}

فراغ RTLIL::Module::add(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(count_id(process->name) == 0);
	processes[process->name] = process;
	process->module = هذا;
}

فراغ RTLIL::Module::remove(ثابت pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	אם (wires.empty())
		החזר;

	بنية DeleteWireWorker
	{
		RTLIL::Module *module;
		ثابت pool<RTLIL::Wire*> *wires_p;

		فراغ operator()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([هذا](RTLIL::Wire *&wire) {
				אם (wires_p->count(wire))
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

	אם (design->flagBufferedNormalized) {
		עבור (auto wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_index.erase(wire);
		}
	}

	עבור (auto &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.count(id) != 0);
		wires_.erase(id);
		delete it;
	}
}

فراغ RTLIL::Module::remove(RTLIL::Cell *cell)
{
	כלעוד (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.count(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	אם (design && design->flagBufferedNormalized && buf_norm_cell_queue.count(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.insert(cell);
	} אחרת {
		delete cell;
	}
}

فراغ RTLIL::Module::remove(RTLIL::Memory *memory)
{
	log_assert(memories.count(memory->name) != 0);
	memories.erase(memory->name);
	delete memory;
}

فراغ RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.count(process->name) != 0);
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
	אם (old_id != IdString::Null && wires_.count(old_id))
		rename(wires_.at(old_id), new_name);
	אחרת אם (old_id != IdString::Null && cells_.count(old_id))
		rename(cells_.at(old_id), new_name);
	אחרת
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
	החזר uniquify(name, index);
}

IdString RTLIL::Module::uniquify(IdString name, صحيح &index)
{
	אם (index == 0) {
		אם (count_id(name) == 0)
			החזר name;
		index++;
	}

	כלעוד (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", index)});
		אם (count_id(new_name) == 0)
			החזר new_name;
		index++;
	}
}

ساكن منطقي fixup_ports_compare(ثابت RTLIL::Wire *a, ثابت RTLIL::Wire *b)
{
	אם (a->port_id && !b->port_id)
		החזר אמת;
	אם (!a->port_id && b->port_id)
		החזר שקר;

	אם (a->port_id == b->port_id)
		החזר a->name < b->name;
	החזר a->port_id < b->port_id;
}

فراغ RTLIL::Module::connect(ثابت RTLIL::SigSig &conn)
{
	עבור (auto mon : monitors)
		mon->notify_connect(هذا, conn);

	אם (design)
		עבור (auto mon : design->monitors)
			mon->notify_connect(هذا, conn);

	// ignore all attempts to assign constants to other constants
	אם (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		עבור (صحيح i = 0; i < GetSize(conn.first); i++)
			אם (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		אם (GetSize(new_conn.first))
			connect(new_conn);
		החזר;
	}

	אם (yosys_xtrace) {
		log("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", هذا, log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(GetSize(conn.first) == GetSize(conn.second));
	connections_.push_back(conn);
}

فراغ RTLIL::Module::connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

فراغ RTLIL::Module::new_connections(ثابت std::vector<RTLIL::SigSig> &new_conn)
{
	עבור (auto mon : monitors)
		mon->notify_connect(هذا, new_conn);

	אם (design)
		עבור (auto mon : design->monitors)
			mon->notify_connect(هذا, new_conn);

	אם (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", هذا);
		עבור (auto &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ثابت std::vector<RTLIL::SigSig> &RTLIL::Module::connections() ثابت
{
	החזר connections_;
}

فراغ RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	עבור (auto &w : wires_)
		אם (w.second->port_input || w.second->port_output)
			all_ports.push_back(w.second);
		אחרת
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	אם (design && design->flagBufferedNormalized) {
		עבור (auto &w : wires_)
			אם (w.second->driverCell_ && w.second->driverCell_->type == ID($input_port))
				buf_norm_wire_queue.insert(w.second);

		buf_norm_wire_queue.insert(all_ports.begin(), all_ports.end());
	}

	ports.clear();
	עבור (size_t i = 0; i < all_ports.size(); i++) {
		ports.push_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
	}
}

فراغ RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		ثابت dict<IdString, RTLIL::Const> &src,
		ثابت RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	אם (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		החזר;
	}
	dst.clear();
	עבור (صحيح i = GetSize(src) - 1; i >= 0; i--) {
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
	החזר wire;
}

RTLIL::Wire *RTLIL::Module::addWire(IdString name, ثابت RTLIL::Wire *other)
{
	RTLIL::Wire *wire = addWire(name);
	wire->width = other->width;
	wire->start_offset = other->start_offset;
	wire->port_id = other->port_id;
	wire->port_input = other->port_input;
	wire->port_output = other->port_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ثابت RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
		RTLIL::copy_attr_dict(wire->attributes, other->attributes, src_design, هذا->design);
	}
	החזר wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = جديد RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	החזר cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, ثابت RTLIL::Cell *other)
{
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	منطقي cross_pool = src_design && هذا->design && src_design != هذا->design;

	IdString type = other->type.ref();
	אם (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, هذا->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, هذا->design);

	אם (!cross_pool)
		cell->connections_ = other->connections_;
	אחרת
		עבור (صحيح i = GetSize(other->connections_) - 1; i >= 0; i--) {
			auto &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	החזר cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = جديد RTLIL::Memory;
	mem->module = هذا;
	mem->name = name;
	memories[name] = mem;
	החזר mem;
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
	החזר mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = جديد RTLIL::Process;
	proc->name = name;
	add(proc);
	החזר proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, ثابت RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ثابت RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	אם (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		עבור (auto *متزامن : proc->syncs)
			עבור (auto &mwa : متزامن->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	החזר proc;
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
		החזר cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		החזר sig_y;                                       \
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
		החזר cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		החזר sig_y;                                       \
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
		החזר cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		החזר sig_y;                                            \
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
		cell->parameters[ID::B_SIGNED] = שקר;             \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		החזר cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		החזר sig_y;                                            \
	}
DEF_METHOD(Shl,      sig_a.size(), ID($shl))
DEF_METHOD(Shr,      sig_a.size(), ID($shr))
DEF_METHOD(Sshl,     sig_a.size(), ID($sshl))
DEF_METHOD(Sshr,     sig_a.size(), ID($sshr))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = שקר;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		החזר cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, منطقي is_signed, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		החזר sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.size(), ID($shiftx))
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		אם (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.size();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		החזר cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		החזר sig_y;                                             \
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
		החזר cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.size() << sig_s.size() : sig_a.size() >> sig_s.size()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		החזר sig_y;                                             \
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
		החזר cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		החזר sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#undef DEF_METHOD

#define DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		החזר cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		החזר sig2;                                      \
	}
#define DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت RTLIL::SigBit &sig3, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		החזר cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		החזר sig3;                                      \
	}
#define DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت RTLIL::SigBit &sig3, ثابت RTLIL::SigBit &sig4, ثابت std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		החזר cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت RTLIL::SigBit &sig3, ثابت std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		החזר sig4;                                      \
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
		החזר cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ثابت RTLIL::SigBit &sig1, ثابت RTLIL::SigBit &sig2, ثابت RTLIL::SigBit &sig3, ثابت RTLIL::SigBit &sig4, ثابت std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		החזר sig5;                                           \
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
	החזר cell;
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
	החזר cell;
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
	החזר cell;
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
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($أكد));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	החזר cell;
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
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
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
	החזר cell;
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
	החזר cell;
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
	החזר cell;
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
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addAdff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_value, منطقي clk_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_VALUE] = arst_value;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addAdffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_value, منطقي clk_polarity, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_VALUE] = arst_value;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
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
	החזר cell;
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
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addSdff(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_value, منطقي clk_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_VALUE] = srst_value;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addSdffe(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_value, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_VALUE] = srst_value;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addSdffce(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_value, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_VALUE] = srst_value;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
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
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatch(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_value, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_VALUE] = arst_value;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
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
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
		ثابت RTLIL::SigSpec &sig_q, منطقي set_polarity, منطقي clr_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي clk_polarity, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
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
	החזר cell;
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
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addAdffGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_value, منطقي clk_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_value ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addAdffeGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_value, منطقي clk_polarity, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_value ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
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
	החזר cell;
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
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addSdffGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_value, منطقي clk_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_value ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addSdffeGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_value, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_value ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addSdffceGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي srst_value, منطقي clk_polarity, منطقي en_polarity, منطقي srst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_value ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, منطقي en_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatchGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
		منطقي arst_value, منطقي en_polarity, منطقي arst_polarity, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_value ? '1' : '0'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
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
	החזר cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	החזר cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, صحيح width, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	החזר sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, صحيح width, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	החזר sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, صحيح width, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	החזר sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, صحيح width, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	החזר sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	החזר sig;
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
	החזר sig;
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
	החזר cell;
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
	החזר sig;
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
	החזר cell;
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
	החזר sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.size());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.size();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	החזר sig;
}

std::string RTLIL::Module::to_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(هذا), design, שקר, אמת, שקר, RTLIL_BACKEND::DumpMode::Readable);
	החזר f.str();
}

RTLIL::Wire::Wire()
{
	ساكن unsigned صحيح hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	module = nullptr;
	width = 1;
	start_offset = 0;
	port_id = 0;
	port_input = שקר;
	port_output = שקר;
	upto = שקר;
	is_signed = שקר;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->insert(std::pair<unsigned صحيح, RTLIL::Wire*>(hashidx_, هذا));
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
	החזר f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
ساكن std::map<unsigned صحيح, RTLIL::Wire*> all_wires;
std::map<unsigned صحيح, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(فراغ)
{
	החזר &all_wires;
}
#endif

RTLIL::Memory::Memory()
{
	ساكن unsigned صحيح hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	width = 1;
	start_offset = 0;
	size = 0;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->insert(std::pair<unsigned صحيح, RTLIL::Memory*>(hashidx_, هذا));
#endif
}

std::string RTLIL::Memory::to_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", هذا, design(), RTLIL_BACKEND::DumpMode::Readable);
	החזר f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	ساكن unsigned صحيح hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;
}

std::string RTLIL::Process::to_rtlil_str() ثابت
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", هذا, design(), RTLIL_BACKEND::DumpMode::Readable);
	החזר f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	ساكن unsigned صحيح hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	// log("#memtrace# %p\n", this);
	memhasher();

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->insert(std::pair<unsigned صحيح, RTLIL::Cell*>(hashidx_, هذا));
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
	החזר f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
ساكن std::map<unsigned صحيح, RTLIL::Cell*> all_cells;
std::map<unsigned صحيح, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(فراغ)
{
	החזר &all_cells;
}
#endif

منطقي RTLIL::Cell::hasPort(RTLIL::IdString portname) ثابت
{
	החזר connections_.count(portname) != 0;
}

// bufnorm

ثابت RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) ثابت
{
	החזר connections_.at(portname);
}

ثابت dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() ثابت
{
	החזר connections_;
}

منطقي RTLIL::Cell::known() ثابت
{
	אם (yosys_celltypes.cell_known(type))
		החזר אמת;
	אם (module && module->design && module->design->module(type))
		החזר אמת;
	החזר שקר;
}

منطقي RTLIL::Cell::input(IdString portname) ثابت
{
	אם (yosys_celltypes.cell_known(type))
		החזר yosys_celltypes.cell_input(type.ref(), portname);
	אם (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		החזר w && w->port_input;
	}
	החזר שקר;
}

منطقي RTLIL::Cell::output(RTLIL::IdString portname) ثابت
{
	אם (yosys_celltypes.cell_known(type))
		החזר yosys_celltypes.cell_output(type, portname);
	אם (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		החזר w && w->port_output;
	}
	החזר שקר;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) ثابت
{
	אם (yosys_celltypes.cell_known(type.ref()))
		החזר yosys_celltypes.cell_port_dir(type, portname);
	אם (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		אם (m == nullptr)
			החזר PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		אם (w == nullptr)
			החזר PortDir::PD_UNKNOWN;
		החזר PortDir(w->port_input + w->port_output * 2);
	}
	החזר PortDir::PD_UNKNOWN;
}

منطقي RTLIL::Cell::hasParam(RTLIL::IdString paramname) ثابت
{
	החזר parameters.count(paramname) != 0;
}

فراغ RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

فراغ RTLIL::Cell::setParam(RTLIL::IdString paramname, RTLIL::Const value)
{
	parameters[paramname] = std::move(value);
}

ثابت RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ثابت
{
	ثابت auto &it = parameters.find(paramname);
	אם (it != parameters.end())
		החזר it->second;
	אם (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		אם (m)
			החזר m->parameter_default_values.at(paramname);
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
	אם (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		החזר;

	אם (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		אם (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		החזר;
	}

	אם (type == ID($demux)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		החזר;
	}

	אם (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		החזר;
	}

	אם (type == ID($fa)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		החזר;
	}

	אם (type == ID($lcu)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::CO]);
		החזר;
	}

	אם (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);
		החזר;
	}

	منطقي signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	אם (connections_.count(ID::A)) {
		אם (signedness_ab) {
			אם (set_a_signed)
				parameters[ID::A_SIGNED] = אמת;
			אחרת אם (parameters.count(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = שקר;
		}
		parameters[ID::A_WIDTH] = GetSize(connections_[ID::A]);
	}

	אם (connections_.count(ID::B)) {
		אם (signedness_ab) {
			אם (set_b_signed)
				parameters[ID::B_SIGNED] = אמת;
			אחרת אם (parameters.count(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = שקר;
		}
		parameters[ID::B_WIDTH] = GetSize(connections_[ID::B]);
	}

	אם (connections_.count(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);

	אם (connections_.count(ID::Q))
		parameters[ID::WIDTH] = GetSize(connections_[ID::Q]);

	check();
}

منطقي RTLIL::Cell::has_keep_attr() ثابت {
	החזר get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

منطقي RTLIL::Cell::has_memid() ثابت
{
	החזר type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

منطقي RTLIL::Cell::is_mem_cell() ثابت
{
	החזר type.in(ID($mem), ID($mem_v2)) || has_memid();
}

منطقي RTLIL::Cell::is_builtin_ff() ثابت {
	החזר StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ثابت RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	אם (wire == عدم)
		data = {bit.data};
	אחרת
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(صحيح offset, صحيح length) ثابت
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= width);
	RTLIL::SigChunk ret;
	אם (wire) {
		ret.wire = wire;
		ret.offset = هذا->offset + offset;
		ret.width = length;
	} אחרת {
		עבור (صحيح i = 0; i < length; i++)
			ret.data.push_back(data[offset+i]);
		ret.width = length;
	}
	החזר ret;
}

RTLIL::SigBit RTLIL::SigChunk::operator[](صحيح offset) ثابت
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	אם (wire) {
		ret.wire = wire;
		ret.offset = هذا->offset + offset;
	} אחרת {
		ret.data = data[offset];
	}
	החזר ret;
}

منطقي RTLIL::SigChunk::operator <(ثابت RTLIL::SigChunk &other) ثابت
{
	אם (wire && other.wire)
		אם (wire->name != other.wire->name)
			החזר wire->name < other.wire->name;

	אם (wire != other.wire)
		החזר wire < other.wire;

	אם (offset != other.offset)
		החזר offset < other.offset;

	אם (width != other.width)
		החזר width < other.width;

	החזר data < other.data;
}

منطقي RTLIL::SigChunk::operator ==(ثابت RTLIL::SigChunk &other) ثابت
{
	החזר wire == other.wire && width == other.width && offset == other.offset && data == other.data;
}

منطقي RTLIL::SigChunk::operator !=(ثابت RTLIL::SigChunk &other) ثابت
{
	אם (*هذا == other)
		החזר שקר;
	החזר אמת;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.size() > 0);
	auto ie = parts.begin();
	auto it = ie + parts.size() - 1;
	כלעוד (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::Const &value)
{
	אם (GetSize(value) != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(value);
	} אחרת {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&value)
{
	אם (GetSize(value) != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(value);
	} אחרת {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::SigChunk &chunk)
{
	אם (chunk.width != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(chunk);
	} אחרת {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	אם (chunk.width != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(chunk);
	} אחרת {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	אם (wire->width != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(wire);
	} אחרת {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, صحيح offset, صحيح width)
{
	אם (width != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} אחרת {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::string &str)
{
	אם (str.size() != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(str);
	} אחרת {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(صحيح val, صحيح width)
{
	אם (width != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(val, width);
	} אחרת
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, صحيح width)
{
	אם (width != 0) {
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(bit, width);
	} אחרת
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت RTLIL::SigBit &bit, صحيح width)
{
	אם (width != 0) {
		אם (bit.wire == عدم) {
			rep_ = CHUNK;
			جديد (&chunk_) RTLIL::SigChunk(bit.data, width);
		} אחרת אם (width == 1) {
			rep_ = CHUNK;
			جديد (&chunk_) RTLIL::SigChunk(bit);
		} אחרת {
			init_empty_bits();
			bits_.reserve(width);
			עבור (صحيح i = 0; i < width; i++)
				bits_.push_back(bit);
		}
	} אחרת
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	עבור (ثابت auto &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	עבור (ثابت auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	עבור (ثابت auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ثابت std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	עבור (ثابت auto &bit : bits)
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
	אם (bit_index >= bits_size)
		החזר;
	صحيح i = bit_index;
	ثابت SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.data.clear();
	אם (bit.is_wire()) {
		chunk.offset = bit.offset;
		כלעוד (i < bits_size && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_index)
			++i;
	} אחרת {
		chunk.offset = 0;
		chunk.data.push_back(bit.data);
		כלעוד (i < bits_size && !spec.bits_[i].is_wire()) {
			chunk.data.push_back(spec.bits_[i].data);
			++i;
		}
	}
	chunk.width = i - bit_index;
}

فراغ RTLIL::SigSpec::unpack()
{
	אם (rep_ == BITS)
		החזר;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	עבור (صحيح i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	جديد (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

فراغ RTLIL::SigSpec::try_repack()
{
	אם (rep_ != BITS)
		החזר;

	صحيح bits_size = GetSize(bits_);
	אם (bits_size == 0)
		החזר;
	אם (bits_[0].is_wire()) {
		עבור (صحيح i = 1; i < bits_size; i++)
			אם (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				החזר;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_size);
		bits_.~vector();
		rep_ = CHUNK;
		جديد (&chunk_) SigChunk(std::move(chunk));
		החזר;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_size);
	bits.push_back(bits_[0].data);
	עבור (صحيح i = 1; i < bits_size; i++) {
		אם (bits_[i].is_wire())
			החזר;
		bits.push_back(bits_[i].data);
	}
	bits_.~vector();
	rep_ = CHUNK;
	جديد (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ثابت
{
	Hasher h;
	עבור (auto &c : chunks())
		אם (c.wire == عدم) {
			עבור (auto &v : c.data)
				h.eat(v);
		} אחרת {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t result = h.yield();
	אם (result == 0)
		result = 1;
	hash_.set(result);
	החזר result;
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
	עבור (صحيح i = 0; i < pattern_size; i++) {
		SigBit pattern_bit = pattern[i];
		אם (pattern_bit.wire != عدم) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	صحيح this_size = size();
	منطقي other_modified = שקר;
	עבור (صحيح j = 0; j < this_size; j++) {
		auto it = pattern_to_with.find((*هذا)[j]);
		אם (it != pattern_to_with.end()) {
			other_modified = אמת;
			(*other)[j] = with[it->second];
		}
	}

	אם (other_modified)
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

	אם (rules.empty()) החזר;

	صحيح this_size = size();
	منطقي other_modified = שקר;
	עבור (صحيح i = 0; i < this_size; i++) {
		auto it = rules.find((*هذا)[i]);
		אם (it != rules.end()) {
			other_modified = אמת;
			(*other)[i] = it->second;
		}
	}

	אם (other_modified)
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

	אם (rules.empty()) החזר;

	صحيح this_size = size();
	منطقي other_modified = שקר;
	עבור (صحيح i = 0; i < this_size; i++) {
		auto it = rules.find((*هذا)[i]);
		אם (it != rules.end()) {
			other_modified = אמת;
			(*other)[i] = it->second;
		}
	}

	אם (other_modified)
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
	אם (other != عدم) {
		log_assert(size() == other->size());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.size());
	עבור (auto &bit : pattern)
		אם (bit.wire != عدم)
			pattern_bits.insert(bit);

	// Compact in-place to avoid O(n^2) erase operations
	size_t write_idx = 0;
	עבור (size_t read_idx = 0; read_idx < bits_.size(); read_idx++)
	{
		אם (!(bits_[read_idx].wire != عدم && pattern_bits.count(bits_[read_idx]))) {
			אם (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				אם (other != عدم)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < bits_.size());
	אם (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	אם (other != عدم && modified) {
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

	אם (other != عدم) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	עבור (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		אם (!(bits_[read_idx].wire != عدم && pattern.count(bits_[read_idx]))) {
			אם (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				אם (other != عدم)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < bits_.size());
	אם (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	אם (other != عدم && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove2(ثابت std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	אם (other != عدم) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	עבור (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		אם (!(bits_[read_idx].wire != عدم && pattern.count(bits_[read_idx]))) {
			אם (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				אם (other != عدم)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	منطقي modified = (write_idx < bits_.size());
	אם (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	אם (other != عدم && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

فراغ RTLIL::SigSpec::remove2(ثابت pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	אם (other != عدم) {
		log_assert(size() == other->size());
		other->unpack();
	}

	منطقي modified = שקר;
	منطقي other_modified = שקר;
	עבור (صحيح i = GetSize(bits_) - 1; i >= 0; i--) {
		אם (bits_[i].wire != عدم && pattern.count(bits_[i].wire)) {
			modified = אמת;
			bits_.erase(bits_.begin() + i);
			אם (other != عدم) {
				other_modified = אמת;
				other->bits_.erase(other->bits_.begin() + i);
			}
		}
	}

	אם (modified) {
		hash_.clear();
		try_repack();
	}
	אם (other_modified) {
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

	עבור (auto& pattern_chunk : pattern.chunks()) {
		אם (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			صحيح i = 0;
			עבור (ثابت RTLIL::SigBit &bit : bits_match) {
				אם (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} אחרת {
			עבור (ثابت RTLIL::SigBit &bit : bits_match)
				אם (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	החזר ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ثابت pool<RTLIL::SigBit> &pattern, ثابت RTLIL::SigSpec *other) ثابت
{
	log_assert(other == عدم || size() == other->size());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	אם (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		صحيح i = 0;
		עבור (ثابت RTLIL::SigBit &bit : bits_match) {
			אם (bit.wire && pattern.count(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} אחרת {
		עבור (ثابت RTLIL::SigBit &bit : bits_match)
			אם (bit.wire && pattern.count(bit))
				ret.append(bit);
	}

	ret.check();
	החזר ret;
}

فراغ RTLIL::SigSpec::replace(صحيح offset, ثابت RTLIL::SigSpec &with)
{
	אם (with.size() == 0)
		החזר;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.size() >= 0);
	log_assert(offset+with.size() <= size());

	صحيح i = 0;
	עבור (ثابت RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

فراغ RTLIL::SigSpec::remove_const()
{
	אם (rep_ == CHUNK)
	{
		אם (chunk_.wire == عدم) {
			chunk_.~SigChunk();
			init_empty_bits();
			hash_.clear();
		}
	}
	אחרת
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.size());
		עבור (auto &bit : bits_)
			אם (bit.wire != عدم)
				new_bits.push_back(bit);
		אם (GetSize(new_bits) != GetSize(bits_)) {
			bits_.swap(new_bits);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

فراغ RTLIL::SigSpec::remove(صحيح offset, صحيح length)
{
	אם (length == 0)
		החזר;

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
	منطقي is_packing = אמת;
	עבור (صحيح i = offset; i < offset + length; i++) {
		منطقي was_packing_before = is_packing;
		SigBit bit = (*هذا)[i];
		אם (i == offset) {
			first = bit;
			אם (!bit.wire)
				is_packing = שקר;
		} אחרת {
			אם (bit.wire != first.wire)
				is_packing = שקר;
			אם (bit.wire)
				אם (bit.offset != first.offset + (i - offset))
					is_packing = שקר;
		}
		אם (was_packing_before && !is_packing)
			עבור (صحيح j = offset; j < i; j++)
				extracted.push_back((*هذا)[j]);
		אם (!is_packing)
			extracted.push_back((*هذا)[i]);
	}
	אם (is_packing)
		החזר SigChunk(first.wire, first.offset, length);

	החזר extracted;
}

فراغ RTLIL::SigSpec::rewrite_wires(std::function<فراغ(RTLIL::Wire*& wire)> rewrite)
{
	אם (rep_ == CHUNK) {
		אם (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		החזר;
	}

	std::vector<RTLIL::SigBit> new_bits;
	עבור (ثابت RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		אם (c.wire != nullptr)
			rewrite(c.wire);
		עבור (صحيح i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.clear();
}

فراغ RTLIL::SigSpec::append(ثابت RTLIL::SigSpec &إشارة)
{
	אם (إشارة.size() == 0)
		החזר;

	אם (size() == 0) {
		*هذا = إشارة;
		החזר;
	}

	hash_.clear();
	אם (rep_ == CHUNK && إشارة.rep_ == CHUNK && chunk_.wire == إشارة.chunk_.wire) {
		אם (chunk_.wire == عدم) {
			chunk_.data.insert(chunk_.data.end(), إشارة.chunk_.data.begin(), إشارة.chunk_.data.end());
			chunk_.width = GetSize(chunk_.data);
			החזר;
		}
		אם (chunk_.offset + chunk_.width == إشارة.chunk_.offset) {
			chunk_.width += إشارة.chunk_.width;
			החזר;
		}
	}

	unpack();
	עבור (ثابت SigBit &bit : إشارة.bits())
		bits_.push_back(bit);
	check();
}

فراغ RTLIL::SigSpec::append(ثابت RTLIL::SigBit &bit)
{
	hash_.clear();

	אם (size() == 0) {
		destroy();
		rep_ = CHUNK;
		جديد (&chunk_) RTLIL::SigChunk(bit);
		החזר;
	}

	אם (rep_ == CHUNK && chunk_.wire == bit.wire) {
		אם (chunk_.wire == عدم) {
			chunk_.data.push_back(bit.data);
			chunk_.width++;
			החזר;
		}
		אם (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			החזר;
		}
	}

	unpack();

	bits_.push_back(bit);
	check();
}

فراغ RTLIL::SigSpec::extend_u0(صحيح width, منطقي is_signed)
{
	אם (size() > width)
		remove(width, size() - width);

	אם (size() < width) {
		RTLIL::SigBit padding = size() > 0 ? (*هذا)[size() - 1] : RTLIL::State::Sx;
		אם (!is_signed)
			padding = RTLIL::State::S0;
		כלעוד (size() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(صحيح num) ثابت
{
	RTLIL::SigSpec sig;
	עבור (صحيح i = 0; i < num; i++)
		sig.append(*هذا);
	החזר sig;
}

#ifndef NDEBUG
فراغ RTLIL::SigSpec::check(ثابت Module *mod) ثابت
{
	אם (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		אם (chunk_.wire == عدم) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.data.size() == (size_t)chunk_.width);
		} אחרת {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.data.size() == 0);
			אם (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	אחרת אם (size() <= 64)
	{
		אם (mod != nullptr) {
			עבור (ثابت RTLIL::SigBit &bit : bits_)
				אם (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#endif

منطقي RTLIL::SigSpec::operator <(ثابت RTLIL::SigSpec &other) ثابت
{
	אם (هذا == &other)
		החזר שקר;

	אם (size() != other.size())
		החזר size() < other.size();

	auto other_it = other.chunks().begin();
	עבור (ثابت SigChunk &c : chunks()) {
		אם (c != *other_it)
			החזר c < *other_it;
		++other_it;
	}

	החזר שקר;
}

منطقي RTLIL::SigSpec::operator ==(ثابت RTLIL::SigSpec &other) ثابت
{
	אם (هذا == &other)
		החזר אמת;

	אם (size() != other.size())
		החזר שקר;

	auto other_it = other.chunks().begin();
	עבור (ثابت SigChunk &c : chunks()) {
		אם (c != *other_it)
			החזר שקר;
		++other_it;
	}

	החזר אמת;
}

منطقي RTLIL::SigSpec::is_wire() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	אם (it == cs.end())
		החזר שקר;
	ثابت RTLIL::SigChunk &chunk = *it;
	החזר chunk.wire && chunk.wire->width == size() && ++it == cs.end();
}

منطقي RTLIL::SigSpec::is_chunk() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	אם (it == cs.end())
		החזר שקר;
	החזר ++it == cs.end();
}

منطقي RTLIL::SigSpec::known_driver() ثابت
{
	עבור (auto &chunk : chunks())
		אם (chunk.is_wire() && !chunk.wire->known_driver())
			החזר שקר;
	החזר אמת;
}

منطقي RTLIL::SigSpec::is_fully_const() ثابت
{
	עבור (auto &chunk : chunks())
		אם (chunk.width > 0 && chunk.wire != عدم)
			החזר שקר;
	החזר אמת;
}

منطقي RTLIL::SigSpec::is_fully_zero() ثابت
{
	עבור (auto &chunk : chunks()) {
		אם (chunk.width > 0 && chunk.wire != عدم)
			החזר שקר;
		עבור (RTLIL::State d : chunk.data)
			אם (d != RTLIL::State::S0)
				החזר שקר;
	}
	החזר אמת;
}

منطقي RTLIL::SigSpec::is_fully_ones() ثابت
{
	עבור (auto &chunk : chunks()) {
		אם (chunk.width > 0 && chunk.wire != عدم)
			החזר שקר;
		עבור (RTLIL::State d : chunk.data)
			אם (d != RTLIL::State::S1)
				החזר שקר;
	}
	החזר אמת;
}

منطقي RTLIL::SigSpec::is_fully_def() ثابت
{
	עבור (auto &chunk : chunks()) {
		אם (chunk.width > 0 && chunk.wire != عدم)
			החזר שקר;
		עבור (RTLIL::State d : chunk.data)
			אם (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				החזר שקר;
	}
	החזר אמת;
}

منطقي RTLIL::SigSpec::is_fully_undef() ثابت
{
	עבור (auto &chunk : chunks()) {
		אם (chunk.width > 0 && chunk.wire != عدم)
			החזר שקר;
		עבור (RTLIL::State d : chunk.data)
			אם (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				החזר שקר;
	}
	החזר אמת;
}

منطقي RTLIL::SigSpec::has_const() ثابت
{
	עבור (auto &chunk : chunks())
		אם (chunk.width > 0 && chunk.wire == عدم)
			החזר אמת;
	החזר שקר;
}

منطقي RTLIL::SigSpec::has_const(State state) ثابت
{
	עבור (auto &chunk : chunks())
		אם (chunk.width > 0 && chunk.wire == عدم && std::find(chunk.data.begin(), chunk.data.end(), state) != chunk.data.end())
			החזר אמת;
	החזר שקר;
}


منطقي RTLIL::SigSpec::has_marked_bits() ثابت
{
	עבור (auto &chunk : chunks())
		אם (chunk.width > 0 && chunk.wire == عدم) {
			עבור (RTLIL::State d : chunk.data)
				אם (d == RTLIL::State::Sm)
					החזר אמת;
		}
	החזר שקר;
}

منطقي RTLIL::SigSpec::is_onehot(صحيح *pos) ثابت
{
	אם (std::optional<RTLIL::Const> c = try_as_const())
		החזר c->is_onehot(pos);
	החזר שקר;
}

منطقي RTLIL::SigSpec::as_bool() ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_value());
	החזר c->as_bool();
}

صحيح RTLIL::SigSpec::as_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_value());
	החזר c->as_int(is_signed);
}

منطقي RTLIL::SigSpec::convertible_to_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	אם (!c.has_value())
		החזר שקר;
	החזר c->convertible_to_int(is_signed);
}

std::optional<صحيح> RTLIL::SigSpec::try_as_int(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	אם (!c.has_value())
		החזר std::nullopt;
	החזר c->try_as_int(is_signed);
}

صحيح RTLIL::SigSpec::as_int_saturating(منطقي is_signed) ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_value());
	החזר c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ثابت
{
	std::string str;
	str.reserve(size());
	std::vector<RTLIL::SigChunk> chunks = *هذا;
	עבור (size_t i = chunks.size(); i > 0; i--) {
		ثابت RTLIL::SigChunk &chunk = chunks[i-1];
		אם (chunk.wire != عدم)
			str.append(chunk.width, '?');
		אחרת
			str += RTLIL::Const(chunk.data).as_string();
	}
	החזר str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	אם (it == cs.end())
		החזר RTLIL::Const();
	SigChunk chunk = *it;
	אם (chunk.wire != عدم || ++it != cs.end())
		החזר std::nullopt;
	החזר RTLIL::Const(std::move(chunk.data));
}

RTLIL::Const RTLIL::SigSpec::as_const() ثابت
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_value());
	החזר *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == size());
	החזר chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ثابت
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	החזר chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() ثابت
{
	החזר RTLIL::SigBit(*هذا);
}

منطقي RTLIL::SigSpec::match(ثابت حرف* pattern) ثابت
{
	صحيح pattern_len = strlen(pattern);
	log_assert(pattern_len == size());

	עבור (صحيح i = 0; i < pattern_len; ++i) {
		حرف ch = pattern[i];
		אם (ch == ' ')
			استمر;
		RTLIL::SigBit bit = (*هذا)[pattern_len - 1 - i];
		אם (ch == '*') {
			אם (bit != State::Sz && bit != State::Sx)
				החזר שקר;
			استمر;
		}
		אם (ch == '0') {
			אם (bit != State::S0)
				החזר שקר;
		} אחרת
		אם (ch == '1') {
			אם (bit != State::S1)
				החזר שקר;
		} אחרת
			log_abort();
	}

	החזר אמת;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() ثابت
{
	std::set<RTLIL::SigBit> sigbits;
	עבור (auto &c : chunks())
		עבור (صحيح i = 0; i < c.width; i++)
			sigbits.insert(RTLIL::SigBit(c, i));
	החזר sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() ثابت
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(size());
	עבור (auto &c : chunks())
		עבור (صحيح i = 0; i < c.width; i++)
			sigbits.insert(RTLIL::SigBit(c, i));
	החזר sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() ثابت
{
	std::vector<RTLIL::SigBit> result;
	result.reserve(size());
	עבור (SigBit bit : *هذا)
		result.push_back(bit);
	החזר result;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت
{
	صحيح this_size = size();
	log_assert(this_size == other.size());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	עבור (صحيح i = 0; i < this_size; i++)
		new_map[(*هذا)[i]] = other[i];

	החזר new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت
{
	صحيح this_size = size();
	log_assert(this_size == other.size());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_size);
	עבור (صحيح i = 0; i < this_size; i++)
		new_map[(*هذا)[i]] = other[i];

	החזר new_map;
}

ساكن فراغ sigspec_parse_split(std::vector<std::string> &tokens, ثابت std::string &text, حرف sep)
{
	size_t start = 0, end = 0;
	כלעוד ((end = text.find(sep, start)) != std::string::npos) {
		tokens.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
}

ساكن RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ثابت std::string &netname)
{
	החזר module->wire(netname);
}

منطقي RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	עבור (صحيح tokidx = صحيح(tokens.size())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		אם (netname.size() == 0)
			استمر;

		אם (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			auto ast = p.const2ast(netname);
			אם (ast == nullptr)
				החזר שקר;
			sig.append(RTLIL::Const(ast->bits));
			استمر;
		}

		אם (module == عدم)
			החזר שקר;

		אם (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		אם (sigspec_parse_wire(module, netname) == nullptr) {
			size_t indices_pos = netname.size()-1;
			אם (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				כלעוד (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				אם (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					כלעוד (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				אם (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		אם (wire == nullptr)
			החזר שקר;

		אם (!indices.empty()) {
			std::vector<std::string> index_tokens;
			sigspec_parse_split(index_tokens, indices.substr(1, indices.size()-2), ':');
			אם (index_tokens.size() == 1) {
				صحيح a = atoi(index_tokens.at(0).c_str());
				אם (a < 0 || a >= wire->width)
					החזר שקר;
				sig.append(RTLIL::SigSpec(wire, a));
			} אחרת {
				صحيح a = atoi(index_tokens.at(0).c_str());
				صحيح b = atoi(index_tokens.at(1).c_str());
				אם (a > b) {
					صحيح tmp = a;
					a = b, b = tmp;
				}
				אם (a < 0 || a >= wire->width)
					החזר שקר;
				אם (b < 0 || b >= wire->width)
					החזר שקר;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} אחרת
			sig.append(wire);
	}

	החזר אמת;
}

منطقي RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	אם (str.empty() || str[0] != '@')
		החזר parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	אם (sel_name == IdString::Null || design->selection_vars.count(sel_name) == 0)
		החזר שקר;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	עבור (auto &it : module->wires_)
		אם (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	החזר אמת;
}

منطقي RTLIL::SigSpec::parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	אם (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.size());
		החזר אמת;
	}

	אם (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.size());
		החזר אמת;
	}

	אם (lhs.is_chunk()) {
		حرف *p = (حرف*)str.c_str(), *endptr;
		long صحيح val = strtol(p, &endptr, 10);
		אם (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.size());
			החזר אמת;
		}
	}

	אם (!parse(sig, module, str))
		החזר שקר;
	אם (sig.size() > lhs.size())
		sig.remove(lhs.size(), sig.size() - lhs.size());
	החזר אמת;
}

RTLIL::SigSpec::operator std::vector<RTLIL::SigChunk>() ثابت
{
	std::vector<RTLIL::SigChunk> result;
	עבור (ثابت RTLIL::SigChunk &c : chunks())
		result.push_back(c);
	החזר result;
}

RTLIL::CaseRule::~CaseRule()
{
	עבור (auto it = switches.begin(); it != switches.end(); it++)
		delete *it;
}

منطقي RTLIL::CaseRule::empty() ثابت
{
	החזר actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() ثابت
{
	RTLIL::CaseRule *new_caserule = جديد RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	עבור (auto &it : switches)
		new_caserule->switches.push_back(it->clone());
	החזר new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	עבור (auto it = cases.begin(); it != cases.end(); it++)
		delete *it;
}

منطقي RTLIL::SwitchRule::empty() ثابت
{
	החזר cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() ثابت
{
	RTLIL::SwitchRule *new_switchrule = جديد RTLIL::SwitchRule;
	new_switchrule->إشارة = إشارة;
	new_switchrule->attributes = attributes;
	עבור (auto &it : cases)
		new_switchrule->cases.push_back(it->clone());
	החזר new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() ثابت
{
	RTLIL::SyncRule *new_syncrule = جديد RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->إشارة = إشارة;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	החזר new_syncrule;
}

RTLIL::Process::~Process()
{
	עבור (auto it = syncs.begin(); it != syncs.end(); it++)
		delete *it;
}

RTLIL::Process *RTLIL::Process::clone() ثابت
{
	RTLIL::Process *new_proc = جديد RTLIL::Process;

	new_proc->attributes = attributes;

	RTLIL::CaseRule *rc_ptr = root_case.clone();
	new_proc->root_case = *rc_ptr;
	rc_ptr->switches.clear();
	delete rc_ptr;

	עבור (auto &it : syncs)
		new_proc->syncs.push_back(it->clone());

	החזר new_proc;
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
	החזר &all_memorys;
}
#endif
YOSYS_NAMESPACE_END
