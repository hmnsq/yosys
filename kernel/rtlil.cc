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

#অন্তর্ভাব "kernel/rtlil.h"
#অন্তর্ভাব "kernel/yosys.h"
#অন্তর্ভাব "kernel/macc.h"
#অন্তর্ভাব "kernel/newcelltypes.h"
#অন্তর্ভাব "kernel/sigtools.h"
#অন্তর্ভাব "kernel/threading.h"
#অন্তর্ভাব "frontends/verilog/verilog_frontend.h"
#অন্তর্ভাব "frontends/verilog/preproc.h"
#অন্তর্ভাব "backends/rtlil/rtlil_backend.h"

#অন্তর্ভাব <string.h>
#অন্তর্ভাব <algorithm>
#অন্তর্ভাব <charconv>
#অন্তর্ভাব <optional>
#অন্তর্ভাব <string_view>
#অন্তর্ভাব <sstream>

YOSYS_NAMESPACE_BEGIN

dict<std::string, std::string> RTLIL::constpad;

ধ্রুব pool<IdString> &RTLIL::builtin_ff_cell_types() {
	স্থির ধ্রুব pool<IdString> res = []() {
		pool<IdString> r;
		প্রতি (size_t i = 0; i < StaticCellTypes::builder.গুনতি; i++) {
			auto &cell = StaticCellTypes::builder.cells[i];
			যদি (cell.features.is_ff)
				r.ঢোকাও(cell.type);
		}
		প্রত্যাগম r;
	}();
	প্রত্যাগম res;
}

#define check(condition) log_assert(condition && "malformed Const union")

ধ্রুব Const::bitvectype& Const::get_bits() ধ্রুব {
	check(is_bits());
	প্রত্যাগম *get_if_bits();
}

ধ্রুব std::string& Const::get_str() ধ্রুব {
	check(is_str());
	প্রত্যাগম *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	প্রত্যাগম *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	প্রত্যাগম *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	নূতন ((শূন্য*)&str_) std::string(std::move(str));
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	বর্ণ bytes[] = {
		(বর্ণ)(val >> 24), (বর্ণ)(val >> 16), (বর্ণ)(val >> 8), (বর্ণ)val
	};
	নূতন ((শূন্য*)&str_) std::string(bytes, 4);
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val, পূর্ণাঙ্ক width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	যদি ((width & 7) == 0) {
		নূতন ((শূন্য*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		পূর্ণাঙ্ক bytes = width >> 3;
		signed বর্ণ sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<পূর্ণাঙ্ক>(bytes, পরিমাণ(val));
		প্রতি (পূর্ণাঙ্ক i = 0; i < bytes; i++) {
			str[str.size() - 1 - i] = val;
			val = val >> 8;
		}
		প্রত্যাগম;
	}

	নূতন ((শূন্য*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	প্রতি (পূর্ণাঙ্ক i = 0; i < width; i++) {
		bv.ঠেলো_back((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, পূর্ণাঙ্ক width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	নূতন ((শূন্য*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	প্রতি (পূর্ণাঙ্ক i = 0; i < width; i++)
		bv.ঠেলো_back(bit);
}

RTLIL::Const::Const(ধ্রুব std::vector<তার্কিক> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	নূতন ((শূন্য*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.size());
	প্রতি (ধ্রুব auto &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ধ্রুব RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	যদি (is_str())
		নূতন ((শূন্য*)&str_) std::string(other.get_str());
	অন্যথা যদি (is_bits())
		নূতন ((শূন্য*)&bits_) bitvectype(other.get_bits());
	অন্যথা
		check(অসত্য);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	যদি (is_str())
		নূতন ((শূন্য*)&str_) std::string(std::move(other.get_str()));
	অন্যথা যদি (is_bits())
		নূতন ((শূন্য*)&bits_) bitvectype(std::move(other.get_bits()));
	অন্যথা
		check(অসত্য);
}

RTLIL::Const &RTLIL::Const::operator =(ধ্রুব RTLIL::Const &other) {
	flags = other.flags;
	যদি (other.is_str()) {
		যদি (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(শূন্য)নূতন ((শূন্য*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} অন্যথা যদি (other.is_bits()) {
		যদি (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(শূন্য)নূতন ((শূন্য*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} অন্যথা {
		check(অসত্য);
	}
	প্রত্যাগম *এতত্;
}

RTLIL::Const::~Const() {
	যদি (is_bits())
		bits_.~bitvectype();
	অন্যথা যদি (is_str())
		str_.~string();
	অন্যথা
		check(অসত্য);
}

তার্কিক RTLIL::Const::operator<(ধ্রুব RTLIL::Const &other) ধ্রুব
{
	যদি (size() != other.size())
		প্রত্যাগম size() < other.size();

	প্রতি (পূর্ণাঙ্ক i = 0; i < size(); i++)
		যদি ((*এতত্)[i] != other[i])
			প্রত্যাগম (*এতত্)[i] < other[i];

	প্রত্যাগম অসত্য;
}

তার্কিক RTLIL::Const::operator ==(ধ্রুব RTLIL::Const &other) ধ্রুব
{
	যদি (is_str() && other.is_str())
		প্রত্যাগম get_str() == other.get_str();
	যদি (is_bits() && other.is_bits())
		প্রত্যাগম get_bits() == other.get_bits();

	যদি (size() != other.size())
		প্রত্যাগম অসত্য;

	প্রতি (পূর্ণাঙ্ক i = 0; i < size(); i++)
	যদি ((*এতত্)[i] != other[i])
		প্রত্যাগম অসত্য;

	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::Const::operator !=(ধ্রুব RTLIL::Const &other) ধ্রুব
{
	প্রত্যাগম !(*এতত্ == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	প্রত্যাগম get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() ধ্রুব
{
	std::vector<State> v;
	প্রতি (auto bit : *এতত্)
		v.ঠেলো_back(bit);
	প্রত্যাগম v;
}

তার্কিক RTLIL::Const::as_bool() ধ্রুব
{
	যদি (is_str()) {
		প্রতি (বর্ণ ch : get_str())
			যদি (ch != 0)
				প্রত্যাগম সত্য;
		প্রত্যাগম অসত্য;
	}

	ধ্রুব bitvectype& bv = get_bits();
	প্রতি (size_t i = 0; i < bv.size(); i++)
		যদি (bv[i] == State::S1)
			প্রত্যাগম সত্য;
	প্রত্যাগম অসত্য;
}

পূর্ণাঙ্ক RTLIL::Const::as_int(তার্কিক is_signed) ধ্রুব
{
	int32_t ret = 0;
	যদি (ধ্রুব std::string *s = get_if_str()) {
		পূর্ণাঙ্ক size = GetSize(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		প্রতি (পূর্ণাঙ্ক i = std::min(4, size); i > 0; i--)
			ret |= static_cast<unsigned বর্ণ>((*s)[size - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		যদি (is_signed && size > 0 && size < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << size*8;
		প্রত্যাগম ret;
	}

	ধ্রুব bitvectype& bv = get_bits();
	পূর্ণাঙ্ক significant_bits = std::min(GetSize(bv), 32);
	প্রতি (পূর্ণাঙ্ক i = 0; i < significant_bits; i++)
		যদি (bv[i] == State::S1)
			ret |= 1 << i;
	যদি (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	প্রত্যাগম ret;
}

তার্কিক RTLIL::Const::convertible_to_int(তার্কিক is_signed) ধ্রুব
{
	auto size = get_min_size(is_signed);

	যদি (size < 0)
		প্রত্যাগম অসত্য;

	// If it fits in 31 bits it is definitely convertible
	যদি (size <= 31)
		প্রত্যাগম সত্য;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	যদি (size == 32) {
		যদি (is_signed)
			প্রত্যাগম সত্য;
		প্রত্যাগম (*এতত্)[size - 1] != State::S1;
	}

	প্রত্যাগম অসত্য;
}

std::optional<পূর্ণাঙ্ক> RTLIL::Const::try_as_int(তার্কিক is_signed) ধ্রুব
{
	যদি (!convertible_to_int(is_signed))
		প্রত্যাগম std::nullopt;
	প্রত্যাগম as_int(is_signed);
}

পূর্ণাঙ্ক RTLIL::Const::as_int_saturating(তার্কিক is_signed) ধ্রুব
{
	যদি (!convertible_to_int(is_signed)) {
		যদি (!is_signed)
			প্রত্যাগম std::numeric_limits<পূর্ণাঙ্ক>::max();

		ধ্রুব auto min_size = get_min_size(is_signed);
		log_assert(min_size > 0);
		ধ্রুব auto neg = (*এতত্)[min_size - 1];
		প্রত্যাগম neg ? std::numeric_limits<পূর্ণাঙ্ক>::min() : std::numeric_limits<পূর্ণাঙ্ক>::max();
	}
	প্রত্যাগম as_int(is_signed);
}

শূন্য RTLIL::Const::tag_bare_integer_const(ধ্রুব std::string &মান)
{
	যদি (মান.empty() || মান.find('\'') != std::string::npos)
		প্রত্যাগম;
	size_t start = (মান[0] == '-' || মান[0] == '+') ? 1 : 0;
	যদি (start < মান.size() && std::all_of(মান.begin() + start, মান.end(), ::isdigit))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

পূর্ণাঙ্ক RTLIL::Const::get_min_size(তার্কিক is_signed) ধ্রুব
{
	যদি (empty()) প্রত্যাগম 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	যদি (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	অন্যথা
		leading_bit = RTLIL::State::S0;

	auto idx = size();
	যাবত্ (idx > 0 && (*এতত্)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	যদি (is_signed && idx < size()) {
		idx++;
	}
	// must be at least one bit
	প্রত্যাগম (idx == 0) ? 1 : idx;
}

শূন্য RTLIL::Const::compress(তার্কিক is_signed)
{
	auto idx = get_min_size(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<পূর্ণাঙ্ক> RTLIL::Const::as_int_compress(তার্কিক is_signed) ধ্রুব
{
	প্রত্যাগম try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ধ্রুব বর্ণ* any) ধ্রুব
{
	পূর্ণাঙ্ক sz = size();
	std::string ret;
	ret.reserve(sz);
	প্রতি (পূর্ণাঙ্ক i = sz - 1; i >= 0; --i)
		বিকল্প ((*এতত্)[i]) {
			পক্ষ S0: ret.ঠেলো_back('0'); ভঙ্গ;
			পক্ষ S1: ret.ঠেলো_back('1'); ভঙ্গ;
			পক্ষ Sx: ret.ঠেলো_back('x'); ভঙ্গ;
			পক্ষ Sz: ret.ঠেলো_back('z'); ভঙ্গ;
			পক্ষ Sa: ret += any; ভঙ্গ;
			পক্ষ Sm: ret.ঠেলো_back('m'); ভঙ্গ;
		}
	প্রত্যাগম ret;
}

RTLIL::Const RTLIL::Const::from_string(ধ্রুব std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.size());
	প্রতি (auto it = str.rbegin(); it != str.rend(); it++)
		বিকল্প (*it) {
			পক্ষ '0': bv.ঠেলো_back(State::S0); ভঙ্গ;
			পক্ষ '1': bv.ঠেলো_back(State::S1); ভঙ্গ;
			পক্ষ 'x': bv.ঠেলো_back(State::Sx); ভঙ্গ;
			পক্ষ 'z': bv.ঠেলো_back(State::Sz); ভঙ্গ;
			পক্ষ 'm': bv.ঠেলো_back(State::Sm); ভঙ্গ;
			স্বভাব: bv.ঠেলো_back(State::Sa);
		}
	প্রত্যাগম c;
}

std::string RTLIL::Const::decode_string() ধ্রুব
{
	যদি (auto str = get_if_str())
		প্রত্যাগম *str;

	ধ্রুব bitvectype& bv = get_bits();
	ধ্রুব পূর্ণাঙ্ক n = GetSize(bv);
	ধ্রুব পূর্ণাঙ্ক n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	পূর্ণাঙ্ক i = n_over_8 * 8;
	যদি (i < n) {
		বর্ণ ch = 0;
		প্রতি (পূর্ণাঙ্ক j = 0; j < (n - i); j++) {
			যদি (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		যদি (ch != 0)
			s.append({ch});
	}
	i -= 8;
	প্রতি (; i >= 0; i -= 8) {
		বর্ণ ch = 0;
		প্রতি (পূর্ণাঙ্ক j = 0; j < 8; j++) {
			যদি (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		যদি (ch != 0)
			s.append({ch});
	}
	প্রত্যাগম s;
}

পূর্ণাঙ্ক RTLIL::Const::size() ধ্রুব {
	যদি (is_str())
		প্রত্যাগম 8 * str_.size();
	অন্যথা {
		check(is_bits());
		প্রত্যাগম bits_.size();
	}
}

তার্কিক RTLIL::Const::empty() ধ্রুব {
	যদি (is_str())
		প্রত্যাগম str_.empty();
	অন্যথা {
		check(is_bits());
		প্রত্যাগম bits_.empty();
	}
}

শূন্য RTLIL::Const::bitvectorize_internal() {
	যদি (tag == backing_tag::bits)
		প্রত্যাগম;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.size() * 8);
	প্রতি (পূর্ণাঙ্ক i = str_.size() - 1; i >= 0; i--) {
		unsigned বর্ণ ch = str_[i];
		প্রতি (পূর্ণাঙ্ক j = 0; j < 8; j++) {
			new_bits.ঠেলো_back((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(শূন্য)নূতন ((শূন্য*)&bits_) bitvectype(std::move(new_bits));
		tag = backing_tag::bits;
	}
}

শূন্য RTLIL::Const::append(ধ্রুব RTLIL::Const &other) {
	bitvectorize_internal();
	bitvectype& bv = get_bits();
	bv.ঢোকাও(bv.end(), other.begin(), other.end());
}

RTLIL::State RTLIL::Const::const_iterator::operator*() ধ্রুব {
	যদি (auto bv = parent->get_if_bits())
		প্রত্যাগম (*bv)[idx];

	পূর্ণাঙ্ক char_idx = parent->get_str().size() - idx / 8 - 1;
	তার্কিক bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	প্রত্যাগম bit ? State::S1 : State::S0;
}

তার্কিক RTLIL::Const::is_fully_zero() ধ্রুব
{
	যদি (auto str = get_if_str()) {
		প্রতি (বর্ণ ch : *str)
			যদি (ch != 0)
				প্রত্যাগম অসত্য;
		প্রত্যাগম সত্য;
	}

	ধ্রুব bitvectype& bv = get_bits();

	প্রতি (ধ্রুব auto &bit : bv)
		যদি (bit != RTLIL::State::S0)
			প্রত্যাগম অসত্য;

	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::Const::is_fully_ones() ধ্রুব
{
	যদি (auto str = get_if_str()) {
		প্রতি (বর্ণ ch : *str)
			যদি (ch != (বর্ণ)0xff)
				প্রত্যাগম অসত্য;
		প্রত্যাগম সত্য;
	}

	ধ্রুব bitvectype& bv = get_bits();
	প্রতি (ধ্রুব auto &bit : bv)
		যদি (bit != RTLIL::State::S1)
			প্রত্যাগম অসত্য;

	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::Const::is_fully_def() ধ্রুব
{
	যদি (is_str())
		প্রত্যাগম সত্য;

	ধ্রুব bitvectype& bv = get_bits();
	প্রতি (ধ্রুব auto &bit : bv)
		যদি (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			প্রত্যাগম অসত্য;

	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::Const::is_fully_undef() ধ্রুব
{
	যদি (auto str = get_if_str())
		প্রত্যাগম str->empty();

	ধ্রুব bitvectype& bv = get_bits();
	প্রতি (ধ্রুব auto &bit : bv)
		যদি (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			প্রত্যাগম অসত্য;

	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::Const::is_fully_undef_x_only() ধ্রুব
{
	যদি (auto str = get_if_str())
		প্রত্যাগম str->empty();

	ধ্রুব bitvectype& bv = get_bits();
	প্রতি (ধ্রুব auto &bit : bv)
		যদি (bit != RTLIL::State::Sx)
			প্রত্যাগম অসত্য;

	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::Const::is_onehot(পূর্ণাঙ্ক *pos) ধ্রুব
{
	তার্কিক found = অসত্য;
	পূর্ণাঙ্ক size = GetSize(*এতত্);
	প্রতি (পূর্ণাঙ্ক i = 0; i < size; i++) {
		State bit = (*এতত্)[i];
		যদি (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			প্রত্যাগম অসত্য;
		যদি (bit == RTLIL::State::S1) {
			যদি (found)
				প্রত্যাগম অসত্য;
			যদি (pos)
				*pos = i;
			found = সত্য;
		}
	}
	প্রত্যাগম found;
}

Hasher RTLIL::Const::hash_into(Hasher h) ধ্রুব
{
	যদি (auto str = get_if_str())
		প্রত্যাগম hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ধ্রুব bitvectype &bv = get_bits();
	পূর্ণাঙ্ক size = GetSize(bv);
	std::string packed;
	পূর্ণাঙ্ক packed_size = (size + 7) >> 3;
	packed.resize(packed_size, 0);
	প্রতি (পূর্ণাঙ্ক bi = 0; bi < packed_size; ++bi) {
		বর্ণ ch = 0;
		পূর্ণাঙ্ক end = std::min((bi + 1)*8, size);
		প্রতি (পূর্ণাঙ্ক i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			যদি (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<বর্ণ>::hash_into(ch, h);
				প্রতি (; i < size; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(size);
				প্রত্যাগম h;
			}
			ch |= static_cast<পূর্ণাঙ্ক>(b) << (i & 7);
		}
		packed[packed_size - 1 - bi] = ch;
	}
	প্রত্যাগম hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(পূর্ণাঙ্ক offset, পূর্ণাঙ্ক len, RTLIL::State padding) ধ্রুব {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	প্রতি (পূর্ণাঙ্ক i = offset; i < offset + len; i++)
		ret_bv.ঠেলো_back(i < GetSize(*এতত্) ? (*এতত্)[i] : padding);
	প্রত্যাগম RTLIL::Const(ret_bv);
}
#undef check /* check(condition) for Const */

তার্কিক RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ধ্রুব
{
	প্রত্যাগম attributes.গুনতি(id);
}

শূন্য RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, তার্কিক মান)
{
	যদি (মান)
		attributes[id] = RTLIL::Const(1);
	অন্যথা
		attributes.erase(id);
}

তার্কিক RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ধ্রুব
{
	ধ্রুব auto it = attributes.find(id);
	যদি (it == attributes.end())
		প্রত্যাগম অসত্য;
	প্রত্যাগম it->second.as_bool();
}

শূন্য RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string মান)
{
	যদি (মান.empty())
		attributes.erase(id);
	অন্যথা
		attributes[id] = মান;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ধ্রুব
{
	std::string মান;
	ধ্রুব auto it = attributes.find(id);
	যদি (it != attributes.end())
		মান = it->second.decode_string();
	প্রত্যাগম মান;
}

শূন্য RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, ধ্রুব pool<string> &data)
{
	string attrval;
	প্রতি (ধ্রুব auto &s : data) {
		যদি (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

শূন্য RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, ধ্রুব pool<string> &data)
{
	pool<string> union_data = get_strpool_attribute(id);
	union_data.ঢোকাও(data.begin(), data.end());
	যদি (!union_data.empty())
		set_strpool_attribute(id, union_data);
}

শূন্য RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->attributes = std::move(buf);
}

নামাকাশ {
	template<typename F>
	শূন্য walk_process_attr_objects(RTLIL::Process *process, F visit) {
		visit(process);
		std::vector<RTLIL::CaseRule*> case_স্তূপ{&process->মূল_case};
		যাবত্ (!case_স্তূপ.empty()) {
			RTLIL::CaseRule *cs = case_স্তূপ.back();
			case_স্তূপ.বের_করো_back();
			visit(cs);
			প্রতি (auto *sw : cs->switches) {
				visit(sw);
				প্রতি (auto *case_ : sw->cases)
					case_স্তূপ.ঠেলো_back(case_);
			}
		}
		প্রতি (auto *সমকাল : process->syncs)
			প্রতি (auto &mwa : সমকাল->mem_write_actions)
				visit(&mwa);
	}

	template<typename F>
	শূন্য walk_attr_objects(RTLIL::Design *design, F visit) {
		প্রতি (auto &[_, module] : design->modules_) {
			visit(module);
			প্রতি (auto &[_, wire] : module->wires_)
				visit(wire);
			প্রতি (auto &[_, mem] : module->memories)
				visit(mem);
			প্রতি (auto &[_, cell] : module->cells_)
				visit(cell);
			প্রতি (auto &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


size_t RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	auto মূল = [&](IdString ref) {
		যদি (ref != IdString::Null)
			live.ঢোকাও(ref);
	};

	walk_attr_objects(এতত্, [&](ধ্রুব RTLIL::AttrObject *obj) {
		প্রতি (auto &attr : obj->attributes)
			মূল(attr.first);
	});

	মূল(selected_active_module);

	প্রতি (auto &[name, module] : modules_) {
		মূল(name);
		প্রতি (IdString port : module->ports)
			মূল(port);
		প্রতি (IdString param : module->avail_parameters)
			মূল(param);
		প্রতি (auto &[param, _] : module->parameter_default_মানসমূহ)
			মূল(param);
		প্রতি (auto &[name, _] : module->memories)
			মূল(name);
		প্রতি (auto &[name, _] : module->processes)
			মূল(name);
		প্রতি (auto &[name, wire] : module->wires_) {
			মূল(name);
			যদি (wire->known_driver())
				মূল(wire->driverPort());
		}
		প্রতি (auto &[name, cell] : module->cells_) {
			মূল(name);
			মূল(cell->type);
			প্রতি (auto &conn : cell->connections())
				মূল(conn.first);
			প্রতি (auto &param : cell->parameters)
				মূল(param.first);
		}
		প্রতি (auto &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_স্তূপ{&process->মূল_case};
			যাবত্ (!case_স্তূপ.empty()) {
				RTLIL::CaseRule *cs = case_স্তূপ.back();
				case_স্তূপ.বের_করো_back();
				প্রতি (auto *sw : cs->switches) {
					প্রতি (auto *case_ : sw->cases)
						case_স্তূপ.ঠেলো_back(case_);
				}
			}
			প্রতি (auto *সমকাল : process->syncs) {
				প্রতি (auto &mwa : সমকাল->mem_write_actions)
					মূল(mwa.memid);
			}
		}
	}

	auto মূল_selection = [&](ধ্রুব RTLIL::Selection &sel) {
		প্রতি (IdString m : sel.selected_modules)
			মূল(m);
		প্রতি (auto &[m, members] : sel.selected_members) {
			মূল(m);
			প্রতি (IdString member : members)
				মূল(member);
		}
	};

	প্রতি (auto &[name, sel] : selection_vars) {
		মূল(name);
		মূল_selection(sel);
	}

	প্রতি (auto &sel : selection_স্তূপ)
		মূল_selection(sel);

	size_t erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_গুনতি;
	প্রত্যাগম erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ধ্রুব
{
	pool<string> data;
	যদি (attributes.গুনতি(id) != 0)
		প্রতি (auto s : split_tokens(get_string_attribute(id), "|"))
			data.ঢোকাও(s);
	প্রত্যাগম data;
}

শূন্য RTLIL::AttrObject::set_hdlname_attribute(ধ্রুব vector<string> &hierarchy)
{
	string attrval;
	প্রতি (ধ্রুব auto &ident : hierarchy) {
		যদি (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() ধ্রুব
{
	প্রত্যাগম split_tokens(get_string_attribute(ID::hdlname), " ");
}

শূন্য RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, ধ্রুব vector<পূর্ণাঙ্ক> &data)
{
	std::stringstream attrval;
	প্রতি (auto &i : data) {
		যদি (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<পূর্ণাঙ্ক> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ধ্রুব
{
	vector<পূর্ণাঙ্ক> data;
	auto it = attributes.find(id);
	যদি (it != attributes.end())
		প্রতি (ধ্রুব auto &s : split_tokens(attributes.at(id).decode_string())) {
			বর্ণ *end = nullptr;
			errno = 0;
			long মান = strtol(s.c_str(), &end, 10);
			যদি (end != s.c_str() + s.size())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			যদি (errno == ERANGE || মান < INT_MIN || মান > INT_MAX)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			data.ঠেলো_back(মান);
		}
	প্রত্যাগম data;
}

তার্কিক RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ধ্রুব
{
	যদি (current_design != nullptr) {
		auto module = current_design->module(mod_name);
		প্রত্যাগম module && module->get_blackbox_attribute();
	} অন্যথা {
		log_warning("Unable to check if module is boxed for null design.\n");
		প্রত্যাগম অসত্য;
	}
}

তার্কিক RTLIL::Selection::selected_module(RTLIL::IdString mod_name) ধ্রুব
{
	যদি (complete_selection)
		প্রত্যাগম সত্য;
	যদি (!selects_boxes && boxed_module(mod_name))
		প্রত্যাগম অসত্য;
	যদি (full_selection)
		প্রত্যাগম সত্য;
	যদি (selected_modules.গুনতি(mod_name) > 0)
		প্রত্যাগম সত্য;
	যদি (selected_members.গুনতি(mod_name) > 0)
		প্রত্যাগম সত্য;
	প্রত্যাগম অসত্য;
}

তার্কিক RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) ধ্রুব
{
	যদি (complete_selection)
		প্রত্যাগম সত্য;
	যদি (!selects_boxes && boxed_module(mod_name))
		প্রত্যাগম অসত্য;
	যদি (full_selection)
		প্রত্যাগম সত্য;
	যদি (selected_modules.গুনতি(mod_name) > 0)
		প্রত্যাগম সত্য;
	প্রত্যাগম অসত্য;
}

তার্কিক RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ধ্রুব
{
	যদি (complete_selection)
		প্রত্যাগম সত্য;
	যদি (!selects_boxes && boxed_module(mod_name))
		প্রত্যাগম অসত্য;
	যদি (full_selection)
		প্রত্যাগম সত্য;
	যদি (selected_modules.গুনতি(mod_name) > 0)
		প্রত্যাগম সত্য;
	যদি (selected_members.গুনতি(mod_name) > 0)
		যদি (selected_members.at(mod_name).গুনতি(memb_name) > 0)
			প্রত্যাগম সত্য;
	প্রত্যাগম অসত্য;
}

শূন্য RTLIL::Selection::optimize(RTLIL::Design *design)
{
	যদি (design != current_design) {
		current_design = design;
	}

	যদি (selects_boxes && full_selection)
		complete_selection = সত্য;
	যদি (complete_selection) {
		full_selection = অসত্য;
		selects_boxes = সত্য;
	}
	যদি (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		প্রত্যাগম;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.clear();
	প্রতি (auto mod_name : selected_modules) {
		যদি (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.ঠেলো_back(mod_name);
		selected_members.erase(mod_name);
	}
	প্রতি (auto mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	প্রতি (auto &it : selected_members)
		যদি (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.ঠেলো_back(it.first);
	প্রতি (auto mod_name : del_list)
		selected_members.erase(mod_name);

	প্রতি (auto &it : selected_members) {
		del_list.clear();
		প্রতি (auto memb_name : it.second)
			যদি (current_design->module(it.first)->গুনতি_id(memb_name) == 0)
				del_list.ঠেলো_back(memb_name);
		প্রতি (auto memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	add_list.clear();
	প্রতি (auto &it : selected_members)
		যদি (it.second.size() == 0)
			del_list.ঠেলো_back(it.first);
		অন্যথা যদি (it.second.size() == current_design->module(it.first)->wires_.size() + current_design->module(it.first)->memories.size() +
				current_design->module(it.first)->cells_.size() + current_design->module(it.first)->processes.size())
			add_list.ঠেলো_back(it.first);
	প্রতি (auto mod_name : del_list)
		selected_members.erase(mod_name);
	প্রতি (auto mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.ঢোকাও(mod_name);
	}

	যদি (selected_modules.size() == current_design->modules_.size()) {
		selected_modules.clear();
		selected_members.clear();
		যদি (selects_boxes)
			complete_selection = সত্য;
		অন্যথা
			full_selection = সত্য;
	}
}

শূন্য RTLIL::Selection::clear()
{
	full_selection = অসত্য;
	complete_selection = অসত্য;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_defines (নূতন define_map_t)
{
	স্থির unsigned পূর্ণাঙ্ক hashidx_গুনতি = 123456789;
	hashidx_গুনতি = mkhash_xorshift(hashidx_গুনতি);
	hashidx_ = hashidx_গুনতি;

	refcount_modules_ = 0;
	selected_active_module.clear();
	ঠেলো_full_selection();

	RTLIL::Design::get_all_designs()->ঢোকাও(std::pair<unsigned পূর্ণাঙ্ক, RTLIL::Design*>(hashidx_, এতত্));
}

RTLIL::Design::~Design()
{
	প্রতি (auto &pr : modules_)
		delete pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

স্থির std::map<unsigned পূর্ণাঙ্ক, RTLIL::Design*> all_designs;
std::map<unsigned পূর্ণাঙ্ক, RTLIL::Design*> *RTLIL::Design::get_all_designs(শূন্য)
{
	প্রত্যাগম &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	প্রত্যাগম RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ধ্রুব RTLIL::Module *RTLIL::Design::module(IdString id) ধ্রুব {
	twines_.check_owned(id);
	প্রত্যাগম modules_.গুনতি(id) ? modules_.at(id) : রিক্ত;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	প্রত্যাগম modules_.গুনতি(id) ? modules_.at(id) : রিক্ত;
}

RTLIL::Module *RTLIL::Design::শীর্ষ_module() ধ্রুব
{
	RTLIL::Module *module = nullptr;
	পূর্ণাঙ্ক module_গুনতি = 0;

	প্রতি (auto mod : selected_modules()) {
		যদি (mod->get_bool_attribute(ID::শীর্ষ))
			প্রত্যাগম mod;
		module_গুনতি++;
		module = mod;
	}

	প্রত্যাগম module_গুনতি == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	যদি (modules_.গুনতি(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = নূতন RTLIL::Module;
	modules_[name] = module;
	module->design = এতত্;
	module->name = name;

	প্রতি (auto mon : monitors)
		mon->notify_module_add(module);

	যদি (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	প্রত্যাগম module;
}

শূন্য RTLIL::Design::scratchpad_unset(ধ্রুব std::string &varname)
{
	scratchpad.erase(varname);
}

শূন্য RTLIL::Design::scratchpad_set_int(ধ্রুব std::string &varname, পূর্ণাঙ্ক মান)
{
	scratchpad[varname] = stringf("%d", মান);
}

শূন্য RTLIL::Design::scratchpad_set_bool(ধ্রুব std::string &varname, তার্কিক মান)
{
	scratchpad[varname] = মান ? "true" : "false";
}

শূন্য RTLIL::Design::scratchpad_set_string(ধ্রুব std::string &varname, std::string মান)
{
	scratchpad[varname] = std::move(মান);
}

পূর্ণাঙ্ক RTLIL::Design::scratchpad_get_int(ধ্রুব std::string &varname, পূর্ণাঙ্ক default_মান_2) ধ্রুব
{
	auto it = scratchpad.find(varname);
	যদি (it == scratchpad.end())
		প্রত্যাগম default_মান_2;

	ধ্রুব std::string &str = it->second;

	যদি (str == "0" || str == "false")
		প্রত্যাগম 0;

	যদি (str == "1" || str == "true")
		প্রত্যাগম 1;

	বর্ণ *endptr = nullptr;
	long পূর্ণাঙ্ক parsed_মান = strtol(str.c_str(), &endptr, 10);
	প্রত্যাগম *endptr ? default_মান_2 : parsed_মান;
}

তার্কিক RTLIL::Design::scratchpad_get_bool(ধ্রুব std::string &varname, তার্কিক default_মান_2) ধ্রুব
{
	auto it = scratchpad.find(varname);
	যদি (it == scratchpad.end())
		প্রত্যাগম default_মান_2;

	ধ্রুব std::string &str = it->second;

	যদি (str == "0" || str == "false")
		প্রত্যাগম অসত্য;

	যদি (str == "1" || str == "true")
		প্রত্যাগম সত্য;

	প্রত্যাগম default_মান_2;
}

std::string RTLIL::Design::scratchpad_get_string(ধ্রুব std::string &varname, ধ্রুব std::string &default_মান_2) ধ্রুব
{
	auto it = scratchpad.find(varname);
	যদি (it == scratchpad.end())
		প্রত্যাগম default_মান_2;

	প্রত্যাগম it->second;
}

শূন্য RTLIL::Design::remove(RTLIL::Module *module)
{
	প্রতি (auto mon : monitors)
		mon->notify_module_del(module);

	যদি (yosys_xtrace) {
		log("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	delete module;
}

শূন্য RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	add(module);
}

শূন্য RTLIL::Design::sort()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
	প্রতি (auto &it : modules_)
		it.second->sort();
}

শূন্য RTLIL::Design::sort_modules()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
}

শূন্য check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

শূন্য RTLIL::Design::check()
{
#ifndef NDEBUG
	log_assert(!selection_স্তূপ.empty());
	পূর্ণাঙ্ক pool_size = 0;
	প্রতি (auto &it : modules_)
		pool_size = std::max(pool_size, ThreadPool::work_pool_size(0, it.second->cells_size(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_size);
	প্রতি (auto &it : modules_) {
		log_assert(এতত্ == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#endif
}

শূন্য RTLIL::Design::optimize()
{
	প্রতি (auto &it : modules_)
		it.second->optimize();
	প্রতি (auto &it : selection_স্তূপ)
		it.optimize(এতত্);
	প্রতি (auto &it : selection_vars)
		it.second.optimize(এতত্);
}

শূন্য RTLIL::Design::clone_into(RTLIL::Design *dst) ধ্রুব
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	প্রতি (auto it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

তার্কিক RTLIL::Design::selected_module(IdString mod_name) ধ্রুব
{
	যদি (!selected_active_module.empty() && mod_name != selected_active_module)
		প্রত্যাগম অসত্য;
	প্রত্যাগম selection().selected_module(mod_name);
}

তার্কিক RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) ধ্রুব
{
	যদি (!selected_active_module.empty() && mod_name != selected_active_module)
		প্রত্যাগম অসত্য;
	প্রত্যাগম selection().selected_whole_module(mod_name);
}

তার্কিক RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ধ্রুব
{
	যদি (!selected_active_module.empty() && mod_name != selected_active_module)
		প্রত্যাগম অসত্য;
	প্রত্যাগম selection().selected_member(mod_name, memb_name);
}

তার্কিক RTLIL::Design::selected_module(RTLIL::Module *mod) ধ্রুব
{
	প্রত্যাগম selected_module(mod->name);
}

তার্কিক RTLIL::Design::selected_whole_module(RTLIL::Module *mod) ধ্রুব
{
	প্রত্যাগম selected_whole_module(mod->name);
}

শূন্য RTLIL::Design::ঠেলো_selection(RTLIL::Selection sel)
{
	sel.current_design = এতত্;
	selection_স্তূপ.ঠেলো_back(sel);
}

শূন্য RTLIL::Design::ঠেলো_empty_selection()
{
	ঠেলো_selection(RTLIL::Selection::EmptySelection(এতত্));
}

শূন্য RTLIL::Design::ঠেলো_full_selection()
{
	ঠেলো_selection(RTLIL::Selection::FullSelection(এতত্));
}

শূন্য RTLIL::Design::ঠেলো_complete_selection()
{
	ঠেলো_selection(RTLIL::Selection::CompleteSelection(এতত্));
}

শূন্য RTLIL::Design::বের_করো_selection()
{
	selection_স্তূপ.বের_করো_back();
	// Default to a full_selection if we ran out of stack
	যদি (selection_স্তূপ.empty())
		ঠেলো_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(তার্কিক only_selected) ধ্রুব
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(এতত্), only_selected, সত্য, অসত্য, RTLIL_BACKEND::DumpMode::Readable);
	প্রত্যাগম f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ধ্রুব
{
	তার্কিক include_partials = partials == RTLIL::SELECT_ALL;
	তার্কিক exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	তার্কিক ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> result;
	result.reserve(modules_.size());
	প্রতি (auto &it : modules_) {
		যদি (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			যদি (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				result.ঠেলো_back(it.second);
			অন্যথা
				বিকল্প (boxes)
				{
				পক্ষ RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.second);
					ভঙ্গ;
				পক্ষ RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.second);
					ভঙ্গ;
				পক্ষ RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.second);
					ভঙ্গ;
				পক্ষ RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.second);
					ভঙ্গ;
				পক্ষ RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.second);
					ভঙ্গ;
				পক্ষ RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.second);
					ভঙ্গ;
				স্বভাব:
					ভঙ্গ;
				}
		} অন্যথা যদি (!include_partials && selected_module(it.first)) {
			বিকল্প(partials)
			{
			পক্ষ RTLIL::SELECT_WHOLE_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.second);
				ভঙ্গ;
			পক্ষ RTLIL::SELECT_WHOLE_ERR:
				log_error("Unsupported partially selected module %s.\n", it.second);
				ভঙ্গ;
			পক্ষ RTLIL::SELECT_WHOLE_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.second);
				ভঙ্গ;
			স্বভাব:
				ভঙ্গ;
			}
		}
	}
	প্রত্যাগম result;
}

RTLIL::Module::Module()
{
	স্থির unsigned পূর্ণাঙ্ক hashidx_গুনতি = 123456789;
	hashidx_গুনতি = mkhash_xorshift(hashidx_গুনতি);
	hashidx_ = hashidx_গুনতি;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->ঢোকাও(std::pair<unsigned পূর্ণাঙ্ক, RTLIL::Module*>(hashidx_, এতত্));
#endif
}

RTLIL::Module::~Module()
{
	প্রতি (auto &pr : wires_)
		delete pr.second;
	প্রতি (auto &pr : memories)
		delete pr.second;
	প্রতি (auto &pr : cells_)
		delete pr.second;
	প্রতি (auto &pr : processes)
		delete pr.second;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
স্থির std::map<unsigned পূর্ণাঙ্ক, RTLIL::Module*> all_modules;
std::map<unsigned পূর্ণাঙ্ক, RTLIL::Module*> *RTLIL::Module::get_all_modules(শূন্য)
{
	প্রত্যাগম &all_modules;
}
#endif

শূন্য RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	প্রতি (auto it = wires_.begin(); it != wires_.end(); ++it)
		যদি (!it->second->port_প্রবেশ && !it->second->port_output)
			delwires.ঢোকাও(it->second);

	প্রতি (auto it = memories.begin(); it != memories.end(); ++it)
		delete it->second;
	memories.clear();

	প্রতি (auto it = cells_.begin(); it != cells_.end(); ++it)
		delete it->second;
	cells_.clear();

	প্রতি (auto it = processes.begin(); it != processes.end(); ++it)
		delete it->second;
	processes.clear();

	connections_.clear();

	remove(delwires);
	set_bool_attribute(ID::blackbox);
}

শূন্য RTLIL::Module::expand_interfaces(RTLIL::Design *, ধ্রুব dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

তার্কিক RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	প্রত্যাগম অসত্য;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ধ্রুব dict<RTLIL::IdString, RTLIL::Const> &, তার্কিক mayfail)
{
	যদি (mayfail)
		প্রত্যাগম IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ধ্রুব dict<RTLIL::IdString, RTLIL::Const> &, ধ্রুব dict<RTLIL::IdString, RTLIL::Module*> &, ধ্রুব dict<RTLIL::IdString, RTLIL::IdString> &, তার্কিক mayfail)
{
	যদি (mayfail)
		প্রত্যাগম IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

size_t RTLIL::Module::গুনতি_id(RTLIL::IdString id)
{
	প্রত্যাগম wires_.গুনতি(id) + cells_.গুনতি(id) + memories.গুনতি(id) + processes.গুনতি(id);
}

#ifndef NDEBUG
নামাকাশ {
	সংরচনা InternalCellChecker
	{
		ধ্রুব RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		InternalCellChecker(ধ্রুব RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		শূন্য error(পূর্ণাঙ্ক linenr, std::optional<std::string> error_বার্তা = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			যদি (error_বার্তা) {
				log_error("%s in internal cell %s\n", *error_বার্তা, cell_loc);
			} অন্যথা {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		পূর্ণাঙ্ক param(RTLIL::IdString name)
		{
			auto it = cell->parameters.find(name);
			যদি (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.ঢোকাও(name);
			প্রত্যাগম it->second.as_int();
		}

		পূর্ণাঙ্ক param_bool(RTLIL::IdString name)
		{
			পূর্ণাঙ্ক v = param(name);
			যদি (GetSize(cell->parameters.at(name)) > 32)
				error(__LINE__);
			যদি (v != 0 && v != 1)
				error(__LINE__);
			প্রত্যাগম v;
		}

		পূর্ণাঙ্ক param_bool(RTLIL::IdString name, তার্কিক expected)
		{
			পূর্ণাঙ্ক v = param_bool(name);
			যদি (v != expected)
				error(__LINE__);
			প্রত্যাগম v;
		}

		শূন্য param_bits(RTLIL::IdString name, পূর্ণাঙ্ক width)
		{
			param(name);
			যদি (GetSize(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			প্রত্যাগম cell->parameters.at(name).decode_string();
		}

		শূন্য port(RTLIL::IdString name, পূর্ণাঙ্ক width)
		{
			auto it = cell->connections_.find(name);
			যদি (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			যদি (GetSize(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.ঢোকাও(name);
		}

		শূন্য check_expected(তার্কিক check_matched_sign = অসত্য)
		{
			প্রতি (auto &para : cell->parameters)
				যদি (expected_params.গুনতি(para.first) == 0)
					error(__LINE__);
			প্রতি (auto &conn : cell->connections())
				যদি (expected_ports.গুনতি(conn.first) == 0)
					error(__LINE__);

			যদি (check_matched_sign) {
				log_assert(expected_params.গুনতি(ID::A_SIGNED) != 0 && expected_params.গুনতি(ID::B_SIGNED) != 0);
				তার্কিক a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				তার্কিক b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				যদি (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		শূন্য check()
		{
			যদি (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				প্রত্যাগম;

			যদি (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(সত্য);
				প্রত্যাগম;
			}

			যদি (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/অসত্য);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/অসত্য);
				প্রত্যাগম;
			}

			যদি (cell->type.in(ID($shift), ID($shiftx))) {
				যদি (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/অসত্য);
				} অন্যথা {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/অসত্য);
				প্রত্যাগম;
			}

			যদি (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(সত্য);
				প্রত্যাগম;
			}

			যদি (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($pow));
				প্রত্যাগম;
			}

			যদি (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::CI, 1);
				port(ID::BI, 1);
				port(ID::X, param(ID::Y_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				port(ID::CO, param(ID::Y_WIDTH));
				check_expected(সত্য);
				প্রত্যাগম;
			}

			যদি (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				প্রত্যাগম;
			}

			যদি (cell->type == ID($macc_v2)) {
				যদি (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				যদি (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				যদি (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ধ্রুব Const &a_width = cell->getParam(ID::A_WIDTHS);
				ধ্রুব Const &b_width = cell->getParam(ID::B_WIDTHS);
				ধ্রুব Const &c_width = cell->getParam(ID::C_WIDTHS);
				পূর্ণাঙ্ক a_width_sum = 0, b_width_sum = 0, c_width_sum = 0;
				প্রতি (পূর্ণাঙ্ক i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_sum += a_width.extract(16 * i, 16).as_int(অসত্য);
					b_width_sum += b_width.extract(16 * i, 16).as_int(অসত্য);
				}
				প্রতি (পূর্ণাঙ্ক i = 0; i < param(ID::NADDENDS); i++) {
					c_width_sum += c_width.extract(16 * i, 16).as_int(অসত্য);
				}
				port(ID::A, a_width_sum);
				port(ID::B, b_width_sum);
				port(ID::C, c_width_sum);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/অসত্য);
				প্রত্যাগম;
			}

			যদি (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				যদি (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($dffsre)) {
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
				প্রত্যাগম;
			}

			যদি (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_মান, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_মান, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type.in(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_মান, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_মান, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($aldffe)) {
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
				প্রত্যাগম;
			}

			যদি (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_মান, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_BITS);
				param(ID::STATE_অঙ্ক);
				param(ID::STATE_অঙ্ক_LOG2);
				param(ID::STATE_RST);
				param_bits(ID::STATE_TABLE, param(ID::STATE_BITS) * param(ID::STATE_অঙ্ক));
				param(ID::TRANS_অঙ্ক);
				param_bits(ID::TRANS_TABLE, param(ID::TRANS_অঙ্ক) * (2*param(ID::STATE_অঙ্ক_LOG2) + param(ID::CTRL_IN_WIDTH) + param(ID::CTRL_OUT_WIDTH)));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::CTRL_IN, param(ID::CTRL_IN_WIDTH));
				port(ID::CTRL_OUT, param(ID::CTRL_OUT_WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_MASK);
				param(ID::COLLISION_X_MASK);
				param_bool(ID::CE_OVER_SRST);
				param_bits(ID::ARST_মান, param(ID::WIDTH));
				param_bits(ID::SRST_মান, param(ID::WIDTH));
				param_bits(ID::INIT_মান, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::SRST, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($memwr_v2)) {
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
				প্রত্যাগম;
			}

			যদি (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::শব্দগুলি));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::শব্দগুলি));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($mem)) {
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
				প্রত্যাগম;
			}

			যদি (cell->type == ID($mem_v2)) {
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
				param_bits(ID::RD_ARST_মান, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_SRST_মান, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_INIT_মান, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
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
				প্রত্যাগম;
			}

			যদি (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type.in(ID($প্রতিজ্ঞা), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type.in(ID($specify2), ID($specify3))) {
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
				যদি (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
				}
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($specrule)) {
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
				প্রত্যাগম;
			}

			যদি (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				প্রত্যাগম;
			}

			যদি (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				যদি (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
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
				প্রত্যাগম;
			}

			যদি (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				যদি (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				প্রত্যাগম;
			}

			যদি (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }
			যদি (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  প্রত্যাগম; }

			যদি (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); প্রত্যাগম; }

			যদি (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}
			যদি (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}
			যদি (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}
			যদি (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}
			যদি (cell->type.in(ID($প্রবেশ_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
			}
			যদি (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				check_expected();
				প্রত্যাগম;
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

শূন্য RTLIL::Module::sort()
{
	wires_.sort(sort_by_id_str(design->twines()));
	cells_.sort(sort_by_id_str(design->twines()));
	parameter_default_মানসমূহ.sort(sort_by_id_str(design->twines()));
	memories.sort(sort_by_id_str(design->twines()));
	processes.sort(sort_by_id_str(design->twines()));
	প্রতি (auto &it : cells_)
		it.second->sort();
	প্রতি (auto &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	প্রতি (auto &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#ifndef NDEBUG
স্থির শূন্য check_id_চাবিগুলি(ধ্রুব dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	প্রতি (auto &it : keyed)
		log_assert(!it.first.empty());
}
#endif

শূন্য check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ifndef NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->cells_size(), 1000));
	ধ্রুব RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	প্রতি (auto &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->width >= 0);
		log_assert(it.second->size >= 0);
		check_id_চাবিগুলি(it.second->attributes);
		memory_strings.ঢোকাও(it.second->name.str());
	}

	std::vector<MonotonicFlag> ports_declared(GetSize(module->ports));
	ShardedVector<std::string> memids(subpool);
	subpool.run([const_module, &ports_declared, &memory_strings, &memids](ধ্রুব ParallelDispatchThreadPool::RunCtx &ctx) {
		প্রতি (পূর্ণাঙ্ক i : ctx.item_range(const_module->cells_size())) {
			auto it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			প্রতি (auto &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_চাবিগুলি(it.second->attributes);
			check_id_চাবিগুলি(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			যদি (it.second->has_memid()) {
				log_assert(memory_strings.গুনতি(it.second->parameters.at(ID::MEMID).decode_string()));
			} অন্যথা যদি (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.গুনতি(memid));
				memids.ঢোকাও(ctx, std::move(memid));
			}
			auto cell_mod = const_module->design->module(it.second->name);
			যদি (cell_mod != nullptr) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#ifdef __GNUC__
				#pragma GCC diagnostic ঠেলো
				#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				#endif
				log_assert(!it.second->get_blackbox_attribute());
				#ifdef __GNUC__
				#pragma GCC diagnostic বের_করো
				#endif
			}
		}

		প্রতি (পূর্ণাঙ্ক i : ctx.item_range(const_module->wires_size())) {
			auto it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_চাবিগুলি(it.second->attributes);
			যদি (it.second->port_id) {
				log_assert(GetSize(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_প্রবেশ || it.second->port_output);
				log_assert(it.second->port_id <= GetSize(ports_declared));
				তার্কিক previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == অসত্য);
			} অন্যথা
				log_assert(!it.second->port_প্রবেশ && !it.second->port_output);
		}
	});
	প্রতি (ধ্রুব MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == সত্য);
	pool<std::string> memids_pool;
	প্রতি (std::string &memid : memids)
		log_assert(memids_pool.ঢোকাও(memid).second);

	প্রতি (auto &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->মূল_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->মূল_case};
		প্রতি (size_t i = 0; i < all_cases.size(); i++) {
			প্রতি (auto &switch_it : all_cases[i]->switches) {
				প্রতি (auto &case_it : switch_it->cases) {
					প্রতি (auto &compare_it : case_it->compare) {
						log_assert(switch_it->সঙ্কেত.size() == compare_it.size());
					}
					all_cases.ঠেলো_back(case_it);
				}
			}
		}
		প্রতি (auto &sync_it : it.second->syncs) {
			বিকল্প (sync_it->type) {
				পক্ষ RTLIL::SyncType::ST0:
				পক্ষ RTLIL::SyncType::ST1:
				পক্ষ RTLIL::SyncType::STp:
				পক্ষ RTLIL::SyncType::STn:
				পক্ষ RTLIL::SyncType::STe:
					log_assert(!sync_it->সঙ্কেত.empty());
					ভঙ্গ;
				পক্ষ RTLIL::SyncType::STa:
				পক্ষ RTLIL::SyncType::STg:
				পক্ষ RTLIL::SyncType::STi:
					log_assert(sync_it->সঙ্কেত.empty());
					ভঙ্গ;
			}
		}
	}

	প্রতি (auto &it : module->connections_) {
		log_assert(it.first.size() == it.second.size());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_চাবিগুলি(module->attributes);
#endif
}

শূন্য RTLIL::Module::check()
{
	পূর্ণাঙ্ক pool_size = ThreadPool::work_pool_size(0, cells_size(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_size);
	check_module(এতত্, thread_pool);
}

শূন্য RTLIL::Module::optimize()
{
}

শূন্য RTLIL::Module::cloneInto(RTLIL::Module *new_mod) ধ্রুব
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	প্রতি (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_মানসমূহ.clear();
	প্রতি (auto &it : parameter_default_মানসমূহ)
		new_mod->parameter_default_মানসমূহ[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	প্রতি (auto &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	প্রতি (auto it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	প্রতি (auto it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	প্রতি (auto it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	প্রতি (auto it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addProcess(dst_id, it->second);
	}

	সংরচনা RewriteSigSpecWorker
	{
		ধ্রুব dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ধ্রুব RTLIL::Module *new_mod;
		শূন্য operator()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([এতত্](RTLIL::Wire *&wire) {
				auto it = wire_map.find(wire);
				যদি (it != wire_map.end())
					wire = it->second;
				অন্যথা
					log_assert(wire->module == new_mod);
			});
		}
	};

	RewriteSigSpecWorker rewriteSigSpecWorker{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewriteSigSpecWorker);
	new_mod->fixup_ports();
}

RTLIL::Module *RTLIL::Module::clone() ধ্রুব
{
	RTLIL::Module *new_mod = নূতন RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	প্রত্যাগম new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) ধ্রুব
{
	RTLIL::Module *new_mod = নূতন RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->add(new_mod);
	প্রত্যাগম new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) ধ্রুব
{
	RTLIL::Module *new_mod = নূতন RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	প্রত্যাগম new_mod;
}

তার্কিক RTLIL::Module::has_memories() ধ্রুব
{
	প্রত্যাগম !memories.empty();
}

তার্কিক RTLIL::Module::has_processes() ধ্রুব
{
	প্রত্যাগম !processes.empty();
}

তার্কিক RTLIL::Module::has_memories_warn() ধ্রুব
{
	যদি (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", এতত্);
	প্রত্যাগম !memories.empty();
}

তার্কিক RTLIL::Module::has_processes_warn() ধ্রুব
{
	যদি (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", এতত্);
	প্রত্যাগম !processes.empty();
}

তার্কিক RTLIL::Module::is_selected() ধ্রুব
{
	প্রত্যাগম design->selected_module(এতত্->name);
}

তার্কিক RTLIL::Module::is_selected_whole() ধ্রুব
{
	প্রত্যাগম design->selected_whole_module(এতত্->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() ধ্রুব
{
	std::vector<RTLIL::Wire*> result;
	result.reserve(wires_.size());
	প্রতি (auto &it : wires_)
		যদি (design->selected(এতত্, it.second))
			result.ঠেলো_back(it.second);
	প্রত্যাগম result;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() ধ্রুব
{
	std::vector<RTLIL::Cell*> result;
	result.reserve(cells_.size());
	প্রতি (auto &it : cells_)
		যদি (design->selected(এতত্, it.second))
			result.ঠেলো_back(it.second);
	প্রত্যাগম result;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() ধ্রুব
{
	std::vector<RTLIL::Memory*> result;
	result.reserve(memories.size());
	প্রতি (auto &it : memories)
		যদি (design->selected(এতত্, it.second))
			result.ঠেলো_back(it.second);
	প্রত্যাগম result;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() ধ্রুব
{
	std::vector<RTLIL::Process*> result;
	result.reserve(processes.size());
	প্রতি (auto &it : processes)
		যদি (design->selected(এতত্, it.second))
			result.ঠেলো_back(it.second);
	প্রত্যাগম result;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() ধ্রুব
{
	std::vector<RTLIL::SelectedMember> result;
	প্রতি (auto cell : selected_cells())
		result.ঠেলো_back({cell, cell->name});
	প্রতি (auto memory : selected_memories())
		result.ঠেলো_back({memory, memory->name});
	প্রতি (auto wire : selected_wires())
		result.ঠেলো_back({wire, wire->name});
	প্রতি (auto process : selected_processes())
		result.ঠেলো_back({process, process->name});
	প্রত্যাগম result;
}

শূন্য RTLIL::Module::add(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(গুনতি_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = এতত্;
}

শূন্য RTLIL::Module::add(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(গুনতি_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = এতত্;
}

শূন্য RTLIL::Module::add(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(গুনতি_id(process->name) == 0);
	processes[process->name] = process;
	process->module = এতত্;
}

শূন্য RTLIL::Module::remove(ধ্রুব pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	যদি (wires.empty())
		প্রত্যাগম;

	সংরচনা DeleteWireWorker
	{
		RTLIL::Module *module;
		ধ্রুব pool<RTLIL::Wire*> *wires_p;

		শূন্য operator()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([এতত্](RTLIL::Wire *&wire) {
				যদি (wires_p->গুনতি(wire))
					wire = module->addWire(stringf("$delete_wire$%d", (পূর্ণাঙ্ক)autoidx++), wire->width);
			});
		}

		শূন্য operator()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	DeleteWireWorker delete_wire_worker;
	delete_wire_worker.module = এতত্;
	delete_wire_worker.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_worker);

	যদি (design->flagBufferedNormalized) {
		প্রতি (auto wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_index.erase(wire);
		}
	}

	প্রতি (auto &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.গুনতি(id) != 0);
		wires_.erase(id);
		delete it;
	}
}

শূন্য RTLIL::Module::remove(RTLIL::Cell *cell)
{
	যাবত্ (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.গুনতি(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	যদি (design && design->flagBufferedNormalized && buf_norm_cell_queue.গুনতি(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.ঢোকাও(cell);
	} অন্যথা {
		delete cell;
	}
}

শূন্য RTLIL::Module::remove(RTLIL::Memory *memory)
{
	log_assert(memories.গুনতি(memory->name) != 0);
	memories.erase(memory->name);
	delete memory;
}

শূন্য RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.গুনতি(process->name) != 0);
	processes.erase(process->name);
	delete process;
}

শূন্য RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	add(wire);
}

শূন্য RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	add(cell);
}

শূন্য RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	যদি (old_id != IdString::Null && wires_.গুনতি(old_id))
		rename(wires_.at(old_id), new_name);
	অন্যথা যদি (old_id != IdString::Null && cells_.গুনতি(old_id))
		rename(cells_.at(old_id), new_name);
	অন্যথা
		log_abort();
}

শূন্য RTLIL::Module::swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2)
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

শূন্য RTLIL::Module::swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	পূর্ণাঙ্ক index = 0;
	প্রত্যাগম uniquify(name, index);
}

IdString RTLIL::Module::uniquify(IdString name, পূর্ণাঙ্ক &index)
{
	যদি (index == 0) {
		যদি (গুনতি_id(name) == 0)
			প্রত্যাগম name;
		index++;
	}

	যাবত্ (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", index)});
		যদি (গুনতি_id(new_name) == 0)
			প্রত্যাগম new_name;
		index++;
	}
}

স্থির তার্কিক fixup_ports_compare(ধ্রুব RTLIL::Wire *a, ধ্রুব RTLIL::Wire *b)
{
	যদি (a->port_id && !b->port_id)
		প্রত্যাগম সত্য;
	যদি (!a->port_id && b->port_id)
		প্রত্যাগম অসত্য;

	যদি (a->port_id == b->port_id)
		প্রত্যাগম a->name < b->name;
	প্রত্যাগম a->port_id < b->port_id;
}

শূন্য RTLIL::Module::connect(ধ্রুব RTLIL::SigSig &conn)
{
	প্রতি (auto mon : monitors)
		mon->notify_connect(এতত্, conn);

	যদি (design)
		প্রতি (auto mon : design->monitors)
			mon->notify_connect(এতত্, conn);

	// ignore all attempts to assign constants to other constants
	যদি (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(conn.first); i++)
			যদি (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		যদি (GetSize(new_conn.first))
			connect(new_conn);
		প্রত্যাগম;
	}

	যদি (yosys_xtrace) {
		log("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", এতত্, log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(GetSize(conn.first) == GetSize(conn.second));
	connections_.ঠেলো_back(conn);
}

শূন্য RTLIL::Module::connect(ধ্রুব RTLIL::SigSpec &lhs, ধ্রুব RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

শূন্য RTLIL::Module::new_connections(ধ্রুব std::vector<RTLIL::SigSig> &new_conn)
{
	প্রতি (auto mon : monitors)
		mon->notify_connect(এতত্, new_conn);

	যদি (design)
		প্রতি (auto mon : design->monitors)
			mon->notify_connect(এতত্, new_conn);

	যদি (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", এতত্);
		প্রতি (auto &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ধ্রুব std::vector<RTLIL::SigSig> &RTLIL::Module::connections() ধ্রুব
{
	প্রত্যাগম connections_;
}

শূন্য RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	প্রতি (auto &w : wires_)
		যদি (w.second->port_প্রবেশ || w.second->port_output)
			all_ports.ঠেলো_back(w.second);
		অন্যথা
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	যদি (design && design->flagBufferedNormalized) {
		প্রতি (auto &w : wires_)
			যদি (w.second->driverCell_ && w.second->driverCell_->type == ID($প্রবেশ_port))
				buf_norm_wire_queue.ঢোকাও(w.second);

		buf_norm_wire_queue.ঢোকাও(all_ports.begin(), all_ports.end());
	}

	ports.clear();
	প্রতি (size_t i = 0; i < all_ports.size(); i++) {
		ports.ঠেলো_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
	}
}

শূন্য RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		ধ্রুব dict<IdString, RTLIL::Const> &src,
		ধ্রুব RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	যদি (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		প্রত্যাগম;
	}
	dst.clear();
	প্রতি (পূর্ণাঙ্ক i = GetSize(src) - 1; i >= 0; i--) {
		auto &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::addWire(RTLIL::IdString name, পূর্ণাঙ্ক width)
{
	log_assert(design);
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	RTLIL::Wire *wire = নূতন RTLIL::Wire;
	wire->width = width;
	wire->name = name;
	add(wire);
	প্রত্যাগম wire;
}

RTLIL::Wire *RTLIL::Module::addWire(IdString name, ধ্রুব RTLIL::Wire *other)
{
	RTLIL::Wire *wire = addWire(name);
	wire->width = other->width;
	wire->start_offset = other->start_offset;
	wire->port_id = other->port_id;
	wire->port_প্রবেশ = other->port_প্রবেশ;
	wire->port_output = other->port_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ধ্রুব RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
		RTLIL::copy_attr_dict(wire->attributes, other->attributes, src_design, এতত্->design);
	}
	প্রত্যাগম wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = নূতন RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	প্রত্যাগম cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, ধ্রুব RTLIL::Cell *other)
{
	ধ্রুব RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	তার্কিক cross_pool = src_design && এতত্->design && src_design != এতত্->design;

	IdString type = other->type.ref();
	যদি (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, এতত্->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, এতত্->design);

	যদি (!cross_pool)
		cell->connections_ = other->connections_;
	অন্যথা
		প্রতি (পূর্ণাঙ্ক i = GetSize(other->connections_) - 1; i >= 0; i--) {
			auto &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	প্রত্যাগম cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = নূতন RTLIL::Memory;
	mem->module = এতত্;
	mem->name = name;
	memories[name] = mem;
	প্রত্যাগম mem;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name, ধ্রুব RTLIL::Memory *other)
{
	log_assert(design);
	RTLIL::Memory *mem = নূতন RTLIL::Memory;
	mem->module = এতত্;
	mem->name = name;
	mem->width = other->width;
	mem->start_offset = other->start_offset;
	mem->size = other->size;
	RTLIL::copy_attr_dict(mem->attributes, other->attributes,
			other->module ? other->module->design : nullptr, design);
	memories[name] = mem;
	প্রত্যাগম mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = নূতন RTLIL::Process;
	proc->name = name;
	add(proc);
	প্রত্যাগম proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, ধ্রুব RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ধ্রুব RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	যদি (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		প্রতি (auto *সমকাল : proc->syncs)
			প্রতি (auto &mwa : সমকাল->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	প্রত্যাগম proc;
}

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, তার্কিক is_signed, ধ্রুব std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		প্রত্যাগম cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, তার্কিক is_signed, ধ্রুব std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		প্রত্যাগম sig_y;                                       \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, তার্কিক /* is_signed */, ধ্রুব std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::WIDTH] = sig_a.size();         \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		প্রত্যাগম cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, তার্কিক is_signed, ধ্রুব std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		প্রত্যাগম sig_y;                                       \
	}
DEF_METHOD(Buf,     sig_a.size(), ID($buf))
DEF_METHOD(Barrier, sig_a.size(), ID($barrier))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, তার্কিক is_signed, ধ্রুব std::string &src) { \
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
		প্রত্যাগম cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, তার্কিক is_signed, ধ্রুব std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		প্রত্যাগম sig_y;                                            \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, তার্কিক is_signed, ধ্রুব std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = অসত্য;             \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		প্রত্যাগম cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, তার্কিক is_signed, ধ্রুব std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		প্রত্যাগম sig_y;                                            \
	}
DEF_METHOD(Shl,      sig_a.size(), ID($shl))
DEF_METHOD(Shr,      sig_a.size(), ID($shr))
DEF_METHOD(Sshl,     sig_a.size(), ID($sshl))
DEF_METHOD(Sshr,     sig_a.size(), ID($sshr))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, তার্কিক is_signed, ধ্রুব std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = অসত্য;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		প্রত্যাগম cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, তার্কিক is_signed, ধ্রুব std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		প্রত্যাগম sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.size(), ID($shiftx))
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		যদি (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.size();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		প্রত্যাগম cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		প্রত্যাগম sig_y;                                             \
	}
DEF_METHOD(Mux,      ID($mux),        0)
DEF_METHOD(Bwmux,    ID($bwmux),      0)
DEF_METHOD(Pmux,     ID($pmux),       1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = _demux ? sig_a.size() : sig_y.size(); \
		cell->parameters[ID::S_WIDTH] = sig_s.size();             \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		প্রত্যাগম cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.size() << sig_s.size() : sig_a.size() >> sig_s.size()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		প্রত্যাগম sig_y;                                             \
	}
DEF_METHOD(Bmux,     ID($bmux),       0)
DEF_METHOD(Demux,    ID($demux),      1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		প্রত্যাগম cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		প্রত্যাগম sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#undef DEF_METHOD

#define DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig1, ধ্রুব RTLIL::SigBit &sig2, ধ্রুব std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		প্রত্যাগম cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig1, ধ্রুব std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		প্রত্যাগম sig2;                                      \
	}
#define DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig1, ধ্রুব RTLIL::SigBit &sig2, ধ্রুব RTLIL::SigBit &sig3, ধ্রুব std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		প্রত্যাগম cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig1, ধ্রুব RTLIL::SigBit &sig2, ধ্রুব std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		প্রত্যাগম sig3;                                      \
	}
#define DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig1, ধ্রুব RTLIL::SigBit &sig2, ধ্রুব RTLIL::SigBit &sig3, ধ্রুব RTLIL::SigBit &sig4, ধ্রুব std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		প্রত্যাগম cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig1, ধ্রুব RTLIL::SigBit &sig2, ধ্রুব RTLIL::SigBit &sig3, ধ্রুব std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		প্রত্যাগম sig4;                                      \
	}
#define DEF_METHOD_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig1, ধ্রুব RTLIL::SigBit &sig2, ধ্রুব RTLIL::SigBit &sig3, ধ্রুব RTLIL::SigBit &sig4, ধ্রুব RTLIL::SigBit &sig5, ধ্রুব std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->setPort("\\" #_P5, sig5);                   \
		cell->set_src_attribute(src);                     \
		প্রত্যাগম cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig1, ধ্রুব RTLIL::SigBit &sig2, ধ্রুব RTLIL::SigBit &sig3, ধ্রুব RTLIL::SigBit &sig4, ধ্রুব std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		প্রত্যাগম sig5;                                           \
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

RTLIL::Cell* RTLIL::Module::addPow(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, তার্কিক a_signed, তার্কিক b_signed, ধ্রুব std::string &src)
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
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addFa(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_c, ধ্রুব RTLIL::SigSpec &sig_x, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fa));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::C, sig_c);
	cell->setPort(ID::X, sig_x);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addSlice(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, RTLIL::Const offset, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($slice));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::Y_WIDTH] = sig_y.size();
	cell->parameters[ID::OFFSET] = offset;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addConcat(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($concat));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::B_WIDTH] = sig_b.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, RTLIL::Const lut, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($প্রতিজ্ঞা));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addSr(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr, ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক set_polarity, তার্কিক clr_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sr));
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addDff(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক clk_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addDffe(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক clk_polarity, তার্কিক en_polarity, ধ্রুব std::string &src)
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
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addDffsr(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক clk_polarity, তার্কিক set_polarity, তার্কিক clr_polarity, ধ্রুব std::string &src)
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
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addDffsre(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক clk_polarity, তার্কিক en_polarity, তার্কিক set_polarity, তার্কিক clr_polarity, ধ্রুব std::string &src)
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
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addAdff(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_arst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_মান, তার্কিক clk_polarity, তার্কিক arst_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_মান] = arst_মান;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addAdffe(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_arst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_মান, তার্কিক clk_polarity, তার্কিক en_polarity, তার্কিক arst_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_মান] = arst_মান;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addAldff(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_aload, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		ধ্রুব RTLIL::SigSpec &sig_ad, তার্কিক clk_polarity, তার্কিক aload_polarity, ধ্রুব std::string &src)
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
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addAldffe(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_aload, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		ধ্রুব RTLIL::SigSpec &sig_ad, তার্কিক clk_polarity, তার্কিক en_polarity, তার্কিক aload_polarity, ধ্রুব std::string &src)
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
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addSdff(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_srst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_মান, তার্কিক clk_polarity, তার্কিক srst_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_মান] = srst_মান;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addSdffe(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_srst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_মান, তার্কিক clk_polarity, তার্কিক en_polarity, তার্কিক srst_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_মান] = srst_মান;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addSdffce(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_srst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_মান, তার্কিক clk_polarity, তার্কিক en_polarity, তার্কিক srst_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_মান] = srst_মান;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addDlatch(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক en_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatch(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_arst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_মান, তার্কিক en_polarity, তার্কিক arst_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_মান] = arst_মান;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsr(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক en_polarity, তার্কিক set_polarity, তার্কিক clr_polarity, ধ্রুব std::string &src)
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
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr,
		ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক set_polarity, তার্কিক clr_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক clk_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক clk_polarity, তার্কিক en_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addDffsrGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক clk_polarity, তার্কিক set_polarity, তার্কিক clr_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addDffsreGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক clk_polarity, তার্কিক en_polarity, তার্কিক set_polarity, তার্কিক clr_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addAdffGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_arst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		তার্কিক arst_মান, তার্কিক clk_polarity, তার্কিক arst_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_মান ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addAdffeGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_arst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		তার্কিক arst_মান, তার্কিক clk_polarity, তার্কিক en_polarity, তার্কিক arst_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_মান ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addAldffGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_aload, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		ধ্রুব RTLIL::SigSpec &sig_ad, তার্কিক clk_polarity, তার্কিক aload_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addAldffeGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_aload, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		ধ্রুব RTLIL::SigSpec &sig_ad, তার্কিক clk_polarity, তার্কিক en_polarity, তার্কিক aload_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addSdffGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_srst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		তার্কিক srst_মান, তার্কিক clk_polarity, তার্কিক srst_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_মান ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addSdffeGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_srst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		তার্কিক srst_মান, তার্কিক clk_polarity, তার্কিক en_polarity, তার্কিক srst_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_মান ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addSdffceGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_srst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		তার্কিক srst_মান, তার্কিক clk_polarity, তার্কিক en_polarity, তার্কিক srst_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_মান ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক en_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatchGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_arst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
		তার্কিক arst_মান, তার্কিক en_polarity, তার্কিক arst_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_মান ? '1' : '0'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsrGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ধ্রুব RTLIL::SigSpec &sig_q, তার্কিক en_polarity, তার্কিক set_polarity, তার্কিক clr_polarity, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, পূর্ণাঙ্ক width, ধ্রুব std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	প্রত্যাগম sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, পূর্ণাঙ্ক width, ধ্রুব std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	প্রত্যাগম sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, পূর্ণাঙ্ক width, ধ্রুব std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	প্রত্যাগম sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, পূর্ণাঙ্ক width, ধ্রুব std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	প্রত্যাগম sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ধ্রুব std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	প্রত্যাগম sig;
}

RTLIL::SigSpec RTLIL::Module::SetTag(RTLIL::IdString name, ধ্রুব std::string &tag, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব RTLIL::SigSpec &sig_c, ধ্রুব std::string &src)
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
	প্রত্যাগম sig;
}

RTLIL::Cell* RTLIL::Module::addSetTag(RTLIL::IdString name, ধ্রুব std::string &tag, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব RTLIL::SigSpec &sig_c, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src)
{
	Cell *cell = addCell(name, ID($set_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::SigSpec RTLIL::Module::GetTag(RTLIL::IdString name, ধ্রুব std::string &tag, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($get_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	প্রত্যাগম sig;
}

RTLIL::Cell* RTLIL::Module::addOverwriteTag(RTLIL::IdString name, ধ্রুব std::string &tag, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব RTLIL::SigSpec &sig_c, ধ্রুব std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($overwrite_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->set_src_attribute(src);
	প্রত্যাগম cell;
}

RTLIL::SigSpec RTLIL::Module::OriginalTag(RTLIL::IdString name, ধ্রুব std::string &tag, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($original_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	প্রত্যাগম sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_e, ধ্রুব std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.size());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.size();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	প্রত্যাগম sig;
}

std::string RTLIL::Module::to_rtlil_str() ধ্রুব
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(এতত্), design, অসত্য, সত্য, অসত্য, RTLIL_BACKEND::DumpMode::Readable);
	প্রত্যাগম f.str();
}

RTLIL::Wire::Wire()
{
	স্থির unsigned পূর্ণাঙ্ক hashidx_গুনতি = 123456789;
	hashidx_গুনতি = mkhash_xorshift(hashidx_গুনতি);
	hashidx_ = hashidx_গুনতি;

	module = nullptr;
	width = 1;
	start_offset = 0;
	port_id = 0;
	port_প্রবেশ = অসত্য;
	port_output = অসত্য;
	upto = অসত্য;
	is_signed = অসত্য;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->ঢোকাও(std::pair<unsigned পূর্ণাঙ্ক, RTLIL::Wire*>(hashidx_, এতত্));
#endif
}

RTLIL::Wire::~Wire()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#endif
}

std::string RTLIL::Wire::to_rtlil_str() ধ্রুব
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", এতত্, design(), RTLIL_BACKEND::DumpMode::Readable);
	প্রত্যাগম f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
স্থির std::map<unsigned পূর্ণাঙ্ক, RTLIL::Wire*> all_wires;
std::map<unsigned পূর্ণাঙ্ক, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(শূন্য)
{
	প্রত্যাগম &all_wires;
}
#endif

RTLIL::Memory::Memory()
{
	স্থির unsigned পূর্ণাঙ্ক hashidx_গুনতি = 123456789;
	hashidx_গুনতি = mkhash_xorshift(hashidx_গুনতি);
	hashidx_ = hashidx_গুনতি;

	width = 1;
	start_offset = 0;
	size = 0;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->ঢোকাও(std::pair<unsigned পূর্ণাঙ্ক, RTLIL::Memory*>(hashidx_, এতত্));
#endif
}

std::string RTLIL::Memory::to_rtlil_str() ধ্রুব
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", এতত্, design(), RTLIL_BACKEND::DumpMode::Readable);
	প্রত্যাগম f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	স্থির unsigned পূর্ণাঙ্ক hashidx_গুনতি = 123456789;
	hashidx_গুনতি = mkhash_xorshift(hashidx_গুনতি);
	hashidx_ = hashidx_গুনতি;
}

std::string RTLIL::Process::to_rtlil_str() ধ্রুব
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", এতত্, design(), RTLIL_BACKEND::DumpMode::Readable);
	প্রত্যাগম f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	স্থির unsigned পূর্ণাঙ্ক hashidx_গুনতি = 123456789;
	hashidx_গুনতি = mkhash_xorshift(hashidx_গুনতি);
	hashidx_ = hashidx_গুনতি;

	// log("#memtrace# %p\n", this);
	memhasher();

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->ঢোকাও(std::pair<unsigned পূর্ণাঙ্ক, RTLIL::Cell*>(hashidx_, এতত্));
#endif
}

RTLIL::Cell::~Cell()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#endif
}

std::string RTLIL::Cell::to_rtlil_str() ধ্রুব
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", এতত্, design(), RTLIL_BACKEND::DumpMode::Readable);
	প্রত্যাগম f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
স্থির std::map<unsigned পূর্ণাঙ্ক, RTLIL::Cell*> all_cells;
std::map<unsigned পূর্ণাঙ্ক, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(শূন্য)
{
	প্রত্যাগম &all_cells;
}
#endif

তার্কিক RTLIL::Cell::hasPort(RTLIL::IdString portname) ধ্রুব
{
	প্রত্যাগম connections_.গুনতি(portname) != 0;
}

// bufnorm

ধ্রুব RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) ধ্রুব
{
	প্রত্যাগম connections_.at(portname);
}

ধ্রুব dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() ধ্রুব
{
	প্রত্যাগম connections_;
}

তার্কিক RTLIL::Cell::known() ধ্রুব
{
	যদি (yosys_celltypes.cell_known(type))
		প্রত্যাগম সত্য;
	যদি (module && module->design && module->design->module(type))
		প্রত্যাগম সত্য;
	প্রত্যাগম অসত্য;
}

তার্কিক RTLIL::Cell::প্রবেশ(IdString portname) ধ্রুব
{
	যদি (yosys_celltypes.cell_known(type))
		প্রত্যাগম yosys_celltypes.cell_প্রবেশ(type.ref(), portname);
	যদি (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		প্রত্যাগম w && w->port_প্রবেশ;
	}
	প্রত্যাগম অসত্য;
}

তার্কিক RTLIL::Cell::output(RTLIL::IdString portname) ধ্রুব
{
	যদি (yosys_celltypes.cell_known(type))
		প্রত্যাগম yosys_celltypes.cell_output(type, portname);
	যদি (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		প্রত্যাগম w && w->port_output;
	}
	প্রত্যাগম অসত্য;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) ধ্রুব
{
	যদি (yosys_celltypes.cell_known(type.ref()))
		প্রত্যাগম yosys_celltypes.cell_port_dir(type, portname);
	যদি (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		যদি (m == nullptr)
			প্রত্যাগম PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		যদি (w == nullptr)
			প্রত্যাগম PortDir::PD_UNKNOWN;
		প্রত্যাগম PortDir(w->port_প্রবেশ + w->port_output * 2);
	}
	প্রত্যাগম PortDir::PD_UNKNOWN;
}

তার্কিক RTLIL::Cell::hasParam(RTLIL::IdString paramname) ধ্রুব
{
	প্রত্যাগম parameters.গুনতি(paramname) != 0;
}

শূন্য RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

শূন্য RTLIL::Cell::setParam(RTLIL::IdString paramname, RTLIL::Const মান)
{
	parameters[paramname] = std::move(মান);
}

ধ্রুব RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ধ্রুব
{
	ধ্রুব auto &it = parameters.find(paramname);
	যদি (it != parameters.end())
		প্রত্যাগম it->second;
	যদি (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		যদি (m)
			প্রত্যাগম m->parameter_default_মানসমূহ.at(paramname);
	}
	ক্ষিপ std::out_of_range("Cell::getParam()");
}

শূন্য RTLIL::Cell::sort()
{
	connections_.sort(sort_by_id_str(module->twines()));
	parameters.sort(sort_by_id_str(module->twines()));
	attributes.sort(sort_by_id_str(module->twines()));
}

শূন্য RTLIL::Cell::check()
{
#ifndef NDEBUG
	InternalCellChecker checker(রিক্ত, এতত্);
	checker.check();
#endif
}

শূন্য RTLIL::Cell::fixup_parameters(তার্কিক set_a_signed, তার্কিক set_b_signed)
{
	যদি (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		প্রত্যাগম;

	যদি (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		যদি (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		প্রত্যাগম;
	}

	যদি (type == ID($demux)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		প্রত্যাগম;
	}

	যদি (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		প্রত্যাগম;
	}

	যদি (type == ID($fa)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		প্রত্যাগম;
	}

	যদি (type == ID($lcu)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::CO]);
		প্রত্যাগম;
	}

	যদি (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);
		প্রত্যাগম;
	}

	তার্কিক signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	যদি (connections_.গুনতি(ID::A)) {
		যদি (signedness_ab) {
			যদি (set_a_signed)
				parameters[ID::A_SIGNED] = সত্য;
			অন্যথা যদি (parameters.গুনতি(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = অসত্য;
		}
		parameters[ID::A_WIDTH] = GetSize(connections_[ID::A]);
	}

	যদি (connections_.গুনতি(ID::B)) {
		যদি (signedness_ab) {
			যদি (set_b_signed)
				parameters[ID::B_SIGNED] = সত্য;
			অন্যথা যদি (parameters.গুনতি(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = অসত্য;
		}
		parameters[ID::B_WIDTH] = GetSize(connections_[ID::B]);
	}

	যদি (connections_.গুনতি(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);

	যদি (connections_.গুনতি(ID::Q))
		parameters[ID::WIDTH] = GetSize(connections_[ID::Q]);

	check();
}

তার্কিক RTLIL::Cell::has_keep_attr() ধ্রুব {
	প্রত্যাগম get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

তার্কিক RTLIL::Cell::has_memid() ধ্রুব
{
	প্রত্যাগম type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

তার্কিক RTLIL::Cell::is_mem_cell() ধ্রুব
{
	প্রত্যাগম type.in(ID($mem), ID($mem_v2)) || has_memid();
}

তার্কিক RTLIL::Cell::is_builtin_ff() ধ্রুব {
	প্রত্যাগম StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ধ্রুব RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	যদি (wire == রিক্ত)
		data = {bit.data};
	অন্যথা
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(পূর্ণাঙ্ক offset, পূর্ণাঙ্ক length) ধ্রুব
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= width);
	RTLIL::SigChunk ret;
	যদি (wire) {
		ret.wire = wire;
		ret.offset = এতত্->offset + offset;
		ret.width = length;
	} অন্যথা {
		প্রতি (পূর্ণাঙ্ক i = 0; i < length; i++)
			ret.data.ঠেলো_back(data[offset+i]);
		ret.width = length;
	}
	প্রত্যাগম ret;
}

RTLIL::SigBit RTLIL::SigChunk::operator[](পূর্ণাঙ্ক offset) ধ্রুব
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	যদি (wire) {
		ret.wire = wire;
		ret.offset = এতত্->offset + offset;
	} অন্যথা {
		ret.data = data[offset];
	}
	প্রত্যাগম ret;
}

তার্কিক RTLIL::SigChunk::operator <(ধ্রুব RTLIL::SigChunk &other) ধ্রুব
{
	যদি (wire && other.wire)
		যদি (wire->name != other.wire->name)
			প্রত্যাগম wire->name < other.wire->name;

	যদি (wire != other.wire)
		প্রত্যাগম wire < other.wire;

	যদি (offset != other.offset)
		প্রত্যাগম offset < other.offset;

	যদি (width != other.width)
		প্রত্যাগম width < other.width;

	প্রত্যাগম data < other.data;
}

তার্কিক RTLIL::SigChunk::operator ==(ধ্রুব RTLIL::SigChunk &other) ধ্রুব
{
	প্রত্যাগম wire == other.wire && width == other.width && offset == other.offset && data == other.data;
}

তার্কিক RTLIL::SigChunk::operator !=(ধ্রুব RTLIL::SigChunk &other) ধ্রুব
{
	যদি (*এতত্ == other)
		প্রত্যাগম অসত্য;
	প্রত্যাগম সত্য;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.size() > 0);
	auto ie = parts.begin();
	auto it = ie + parts.size() - 1;
	যাবত্ (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ধ্রুব RTLIL::Const &মান)
{
	যদি (GetSize(মান) != 0) {
		rep_ = CHUNK;
		নূতন (&chunk_) RTLIL::SigChunk(মান);
	} অন্যথা {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&মান)
{
	যদি (GetSize(মান) != 0) {
		rep_ = CHUNK;
		নূতন (&chunk_) RTLIL::SigChunk(মান);
	} অন্যথা {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ধ্রুব RTLIL::SigChunk &chunk)
{
	যদি (chunk.width != 0) {
		rep_ = CHUNK;
		নূতন (&chunk_) RTLIL::SigChunk(chunk);
	} অন্যথা {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	যদি (chunk.width != 0) {
		rep_ = CHUNK;
		নূতন (&chunk_) RTLIL::SigChunk(chunk);
	} অন্যথা {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	যদি (wire->width != 0) {
		rep_ = CHUNK;
		নূতন (&chunk_) RTLIL::SigChunk(wire);
	} অন্যথা {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, পূর্ণাঙ্ক offset, পূর্ণাঙ্ক width)
{
	যদি (width != 0) {
		rep_ = CHUNK;
		নূতন (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} অন্যথা {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ধ্রুব std::string &str)
{
	যদি (str.size() != 0) {
		rep_ = CHUNK;
		নূতন (&chunk_) RTLIL::SigChunk(str);
	} অন্যথা {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(পূর্ণাঙ্ক val, পূর্ণাঙ্ক width)
{
	যদি (width != 0) {
		rep_ = CHUNK;
		নূতন (&chunk_) RTLIL::SigChunk(val, width);
	} অন্যথা
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, পূর্ণাঙ্ক width)
{
	যদি (width != 0) {
		rep_ = CHUNK;
		নূতন (&chunk_) RTLIL::SigChunk(bit, width);
	} অন্যথা
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ধ্রুব RTLIL::SigBit &bit, পূর্ণাঙ্ক width)
{
	যদি (width != 0) {
		যদি (bit.wire == রিক্ত) {
			rep_ = CHUNK;
			নূতন (&chunk_) RTLIL::SigChunk(bit.data, width);
		} অন্যথা যদি (width == 1) {
			rep_ = CHUNK;
			নূতন (&chunk_) RTLIL::SigChunk(bit);
		} অন্যথা {
			init_empty_bits();
			bits_.reserve(width);
			প্রতি (পূর্ণাঙ্ক i = 0; i < width; i++)
				bits_.ঠেলো_back(bit);
		}
	} অন্যথা
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ধ্রুব std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	প্রতি (ধ্রুব auto &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ধ্রুব std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	প্রতি (ধ্রুব auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ধ্রুব pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	প্রতি (ধ্রুব auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ধ্রুব std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	প্রতি (ধ্রুব auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(তার্কিক bit)
{
	rep_ = CHUNK;
	নূতন (&chunk_) RTLIL::SigChunk(bit ? RTLIL::S1 : RTLIL::S0);
	check();
}

শূন্য RTLIL::SigSpec::Chunks::const_iterator::next_chunk_bits()
{
	পূর্ণাঙ্ক bits_size = GetSize(spec.bits_);
	যদি (bit_index >= bits_size)
		প্রত্যাগম;
	পূর্ণাঙ্ক i = bit_index;
	ধ্রুব SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.data.clear();
	যদি (bit.is_wire()) {
		chunk.offset = bit.offset;
		যাবত্ (i < bits_size && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_index)
			++i;
	} অন্যথা {
		chunk.offset = 0;
		chunk.data.ঠেলো_back(bit.data);
		যাবত্ (i < bits_size && !spec.bits_[i].is_wire()) {
			chunk.data.ঠেলো_back(spec.bits_[i].data);
			++i;
		}
	}
	chunk.width = i - bit_index;
}

শূন্য RTLIL::SigSpec::unpack()
{
	যদি (rep_ == BITS)
		প্রত্যাগম;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	প্রতি (পূর্ণাঙ্ক i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	নূতন (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

শূন্য RTLIL::SigSpec::try_repack()
{
	যদি (rep_ != BITS)
		প্রত্যাগম;

	পূর্ণাঙ্ক bits_size = GetSize(bits_);
	যদি (bits_size == 0)
		প্রত্যাগম;
	যদি (bits_[0].is_wire()) {
		প্রতি (পূর্ণাঙ্ক i = 1; i < bits_size; i++)
			যদি (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				প্রত্যাগম;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_size);
		bits_.~vector();
		rep_ = CHUNK;
		নূতন (&chunk_) SigChunk(std::move(chunk));
		প্রত্যাগম;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_size);
	bits.ঠেলো_back(bits_[0].data);
	প্রতি (পূর্ণাঙ্ক i = 1; i < bits_size; i++) {
		যদি (bits_[i].is_wire())
			প্রত্যাগম;
		bits.ঠেলো_back(bits_[i].data);
	}
	bits_.~vector();
	rep_ = CHUNK;
	নূতন (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ধ্রুব
{
	Hasher h;
	প্রতি (auto &c : chunks())
		যদি (c.wire == রিক্ত) {
			প্রতি (auto &v : c.data)
				h.eat(v);
		} অন্যথা {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t result = h.yield();
	যদি (result == 0)
		result = 1;
	hash_.set(result);
	প্রত্যাগম result;
}

শূন্য RTLIL::SigSpec::sort()
{
	unpack();
	std::sort(bits_.begin(), bits_.end());
	hash_.clear();
	try_repack();
}

শূন্য RTLIL::SigSpec::sort_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::vector<SigBit> unique_bits = bits_;
	std::sort(unique_bits.begin(), unique_bits.end());
	auto last = std::unique(unique_bits.begin(), unique_bits.end());
	unique_bits.erase(last, unique_bits.end());

	*এতত্ = unique_bits;
	hash_.clear();
	try_repack();
}

শূন্য RTLIL::SigSpec::replace(ধ্রুব RTLIL::SigSpec &pattern, ধ্রুব RTLIL::SigSpec &with)
{
	replace(pattern, with, এতত্);
}

শূন্য RTLIL::SigSpec::replace(ধ্রুব RTLIL::SigSpec &pattern, ধ্রুব RTLIL::SigSpec &with, RTLIL::SigSpec *other) ধ্রুব
{
	log_assert(other != রিক্ত);
	log_assert(size() == other->size());
	log_assert(pattern.size() == with.size());

	dict<RTLIL::SigBit, পূর্ণাঙ্ক> pattern_to_with;
	পূর্ণাঙ্ক pattern_size = pattern.size();
	প্রতি (পূর্ণাঙ্ক i = 0; i < pattern_size; i++) {
		SigBit pattern_bit = pattern[i];
		যদি (pattern_bit.wire != রিক্ত) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	পূর্ণাঙ্ক this_size = size();
	তার্কিক other_modified = অসত্য;
	প্রতি (পূর্ণাঙ্ক j = 0; j < this_size; j++) {
		auto it = pattern_to_with.find((*এতত্)[j]);
		যদি (it != pattern_to_with.end()) {
			other_modified = সত্য;
			(*other)[j] = with[it->second];
		}
	}

	যদি (other_modified)
		other->try_repack();
	other->check();
}

শূন্য RTLIL::SigSpec::replace(ধ্রুব dict<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, এতত্);
}

শূন্য RTLIL::SigSpec::replace(ধ্রুব dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ধ্রুব
{
	log_assert(other != রিক্ত);
	log_assert(size() == other->size());

	যদি (rules.empty()) প্রত্যাগম;

	পূর্ণাঙ্ক this_size = size();
	তার্কিক other_modified = অসত্য;
	প্রতি (পূর্ণাঙ্ক i = 0; i < this_size; i++) {
		auto it = rules.find((*এতত্)[i]);
		যদি (it != rules.end()) {
			other_modified = সত্য;
			(*other)[i] = it->second;
		}
	}

	যদি (other_modified)
		other->try_repack();
	other->check();
}

শূন্য RTLIL::SigSpec::replace(ধ্রুব std::map<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, এতত্);
}

শূন্য RTLIL::SigSpec::replace(ধ্রুব std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ধ্রুব
{
	log_assert(other != রিক্ত);
	log_assert(size() == other->size());

	যদি (rules.empty()) প্রত্যাগম;

	পূর্ণাঙ্ক this_size = size();
	তার্কিক other_modified = অসত্য;
	প্রতি (পূর্ণাঙ্ক i = 0; i < this_size; i++) {
		auto it = rules.find((*এতত্)[i]);
		যদি (it != rules.end()) {
			other_modified = সত্য;
			(*other)[i] = it->second;
		}
	}

	যদি (other_modified)
		other->try_repack();
	other->check();
}

শূন্য RTLIL::SigSpec::remove(ধ্রুব RTLIL::SigSpec &pattern)
{
	remove2(pattern, রিক্ত);
}

শূন্য RTLIL::SigSpec::remove(ধ্রুব RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) ধ্রুব
{
	RTLIL::SigSpec tmp = *এতত্;
	tmp.remove2(pattern, other);
}

শূন্য RTLIL::SigSpec::remove2(ধ্রুব RTLIL::SigSpec &pattern, RTLIL::SigSpec *other)
{
	unpack();
	যদি (other != রিক্ত) {
		log_assert(size() == other->size());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.size());
	প্রতি (auto &bit : pattern)
		যদি (bit.wire != রিক্ত)
			pattern_bits.ঢোকাও(bit);

	// Compact in-place to avoid O(n^2) erase operations
	size_t write_idx = 0;
	প্রতি (size_t read_idx = 0; read_idx < bits_.size(); read_idx++)
	{
		যদি (!(bits_[read_idx].wire != রিক্ত && pattern_bits.গুনতি(bits_[read_idx]))) {
			যদি (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				যদি (other != রিক্ত)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	তার্কিক modified = (write_idx < bits_.size());
	যদি (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	যদি (other != রিক্ত && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

শূন্য RTLIL::SigSpec::remove(ধ্রুব pool<RTLIL::SigBit> &pattern)
{
	remove2(pattern, রিক্ত);
}

শূন্য RTLIL::SigSpec::remove(ধ্রুব pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) ধ্রুব
{
	RTLIL::SigSpec tmp = *এতত্;
	tmp.remove2(pattern, other);
}

শূন্য RTLIL::SigSpec::remove2(ধ্রুব pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	যদি (other != রিক্ত) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	প্রতি (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		যদি (!(bits_[read_idx].wire != রিক্ত && pattern.গুনতি(bits_[read_idx]))) {
			যদি (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				যদি (other != রিক্ত)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	তার্কিক modified = (write_idx < bits_.size());
	যদি (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	যদি (other != রিক্ত && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

শূন্য RTLIL::SigSpec::remove2(ধ্রুব std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	যদি (other != রিক্ত) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	প্রতি (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		যদি (!(bits_[read_idx].wire != রিক্ত && pattern.গুনতি(bits_[read_idx]))) {
			যদি (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				যদি (other != রিক্ত)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	তার্কিক modified = (write_idx < bits_.size());
	যদি (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	যদি (other != রিক্ত && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

শূন্য RTLIL::SigSpec::remove2(ধ্রুব pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	যদি (other != রিক্ত) {
		log_assert(size() == other->size());
		other->unpack();
	}

	তার্কিক modified = অসত্য;
	তার্কিক other_modified = অসত্য;
	প্রতি (পূর্ণাঙ্ক i = GetSize(bits_) - 1; i >= 0; i--) {
		যদি (bits_[i].wire != রিক্ত && pattern.গুনতি(bits_[i].wire)) {
			modified = সত্য;
			bits_.erase(bits_.begin() + i);
			যদি (other != রিক্ত) {
				other_modified = সত্য;
				other->bits_.erase(other->bits_.begin() + i);
			}
		}
	}

	যদি (modified) {
		hash_.clear();
		try_repack();
	}
	যদি (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ধ্রুব RTLIL::SigSpec &pattern, ধ্রুব RTLIL::SigSpec *other) ধ্রুব
{
	log_assert(other == রিক্ত || size() == other->size());

	RTLIL::SigSpec ret;
	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();

	প্রতি (auto& pattern_chunk : pattern.chunks()) {
		যদি (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			পূর্ণাঙ্ক i = 0;
			প্রতি (ধ্রুব RTLIL::SigBit &bit : bits_match) {
				যদি (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} অন্যথা {
			প্রতি (ধ্রুব RTLIL::SigBit &bit : bits_match)
				যদি (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	প্রত্যাগম ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ধ্রুব pool<RTLIL::SigBit> &pattern, ধ্রুব RTLIL::SigSpec *other) ধ্রুব
{
	log_assert(other == রিক্ত || size() == other->size());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	যদি (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		পূর্ণাঙ্ক i = 0;
		প্রতি (ধ্রুব RTLIL::SigBit &bit : bits_match) {
			যদি (bit.wire && pattern.গুনতি(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} অন্যথা {
		প্রতি (ধ্রুব RTLIL::SigBit &bit : bits_match)
			যদি (bit.wire && pattern.গুনতি(bit))
				ret.append(bit);
	}

	ret.check();
	প্রত্যাগম ret;
}

শূন্য RTLIL::SigSpec::replace(পূর্ণাঙ্ক offset, ধ্রুব RTLIL::SigSpec &with)
{
	যদি (with.size() == 0)
		প্রত্যাগম;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.size() >= 0);
	log_assert(offset+with.size() <= size());

	পূর্ণাঙ্ক i = 0;
	প্রতি (ধ্রুব RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

শূন্য RTLIL::SigSpec::remove_const()
{
	যদি (rep_ == CHUNK)
	{
		যদি (chunk_.wire == রিক্ত) {
			chunk_.~SigChunk();
			init_empty_bits();
			hash_.clear();
		}
	}
	অন্যথা
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.size());
		প্রতি (auto &bit : bits_)
			যদি (bit.wire != রিক্ত)
				new_bits.ঠেলো_back(bit);
		যদি (GetSize(new_bits) != GetSize(bits_)) {
			bits_.swap(new_bits);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

শূন্য RTLIL::SigSpec::remove(পূর্ণাঙ্ক offset, পূর্ণাঙ্ক length)
{
	যদি (length == 0)
		প্রত্যাগম;

	unpack();

	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	bits_.erase(bits_.begin() + offset, bits_.begin() + offset + length);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(পূর্ণাঙ্ক offset, পূর্ণাঙ্ক length) ধ্রুব
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	std::vector<SigBit> extracted;
	SigBit first;
	তার্কিক is_packing = সত্য;
	প্রতি (পূর্ণাঙ্ক i = offset; i < offset + length; i++) {
		তার্কিক was_packing_before = is_packing;
		SigBit bit = (*এতত্)[i];
		যদি (i == offset) {
			first = bit;
			যদি (!bit.wire)
				is_packing = অসত্য;
		} অন্যথা {
			যদি (bit.wire != first.wire)
				is_packing = অসত্য;
			যদি (bit.wire)
				যদি (bit.offset != first.offset + (i - offset))
					is_packing = অসত্য;
		}
		যদি (was_packing_before && !is_packing)
			প্রতি (পূর্ণাঙ্ক j = offset; j < i; j++)
				extracted.ঠেলো_back((*এতত্)[j]);
		যদি (!is_packing)
			extracted.ঠেলো_back((*এতত্)[i]);
	}
	যদি (is_packing)
		প্রত্যাগম SigChunk(first.wire, first.offset, length);

	প্রত্যাগম extracted;
}

শূন্য RTLIL::SigSpec::rewrite_wires(std::function<শূন্য(RTLIL::Wire*& wire)> rewrite)
{
	যদি (rep_ == CHUNK) {
		যদি (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		প্রত্যাগম;
	}

	std::vector<RTLIL::SigBit> new_bits;
	প্রতি (ধ্রুব RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		যদি (c.wire != nullptr)
			rewrite(c.wire);
		প্রতি (পূর্ণাঙ্ক i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.clear();
}

শূন্য RTLIL::SigSpec::append(ধ্রুব RTLIL::SigSpec &সঙ্কেত)
{
	যদি (সঙ্কেত.size() == 0)
		প্রত্যাগম;

	যদি (size() == 0) {
		*এতত্ = সঙ্কেত;
		প্রত্যাগম;
	}

	hash_.clear();
	যদি (rep_ == CHUNK && সঙ্কেত.rep_ == CHUNK && chunk_.wire == সঙ্কেত.chunk_.wire) {
		যদি (chunk_.wire == রিক্ত) {
			chunk_.data.ঢোকাও(chunk_.data.end(), সঙ্কেত.chunk_.data.begin(), সঙ্কেত.chunk_.data.end());
			chunk_.width = GetSize(chunk_.data);
			প্রত্যাগম;
		}
		যদি (chunk_.offset + chunk_.width == সঙ্কেত.chunk_.offset) {
			chunk_.width += সঙ্কেত.chunk_.width;
			প্রত্যাগম;
		}
	}

	unpack();
	প্রতি (ধ্রুব SigBit &bit : সঙ্কেত.bits())
		bits_.ঠেলো_back(bit);
	check();
}

শূন্য RTLIL::SigSpec::append(ধ্রুব RTLIL::SigBit &bit)
{
	hash_.clear();

	যদি (size() == 0) {
		destroy();
		rep_ = CHUNK;
		নূতন (&chunk_) RTLIL::SigChunk(bit);
		প্রত্যাগম;
	}

	যদি (rep_ == CHUNK && chunk_.wire == bit.wire) {
		যদি (chunk_.wire == রিক্ত) {
			chunk_.data.ঠেলো_back(bit.data);
			chunk_.width++;
			প্রত্যাগম;
		}
		যদি (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			প্রত্যাগম;
		}
	}

	unpack();

	bits_.ঠেলো_back(bit);
	check();
}

শূন্য RTLIL::SigSpec::extend_u0(পূর্ণাঙ্ক width, তার্কিক is_signed)
{
	যদি (size() > width)
		remove(width, size() - width);

	যদি (size() < width) {
		RTLIL::SigBit padding = size() > 0 ? (*এতত্)[size() - 1] : RTLIL::State::Sx;
		যদি (!is_signed)
			padding = RTLIL::State::S0;
		যাবত্ (size() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(পূর্ণাঙ্ক অঙ্ক) ধ্রুব
{
	RTLIL::SigSpec sig;
	প্রতি (পূর্ণাঙ্ক i = 0; i < অঙ্ক; i++)
		sig.append(*এতত্);
	প্রত্যাগম sig;
}

#ifndef NDEBUG
শূন্য RTLIL::SigSpec::check(ধ্রুব Module *mod) ধ্রুব
{
	যদি (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		যদি (chunk_.wire == রিক্ত) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.data.size() == (size_t)chunk_.width);
		} অন্যথা {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.data.size() == 0);
			যদি (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	অন্যথা যদি (size() <= 64)
	{
		যদি (mod != nullptr) {
			প্রতি (ধ্রুব RTLIL::SigBit &bit : bits_)
				যদি (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#endif

তার্কিক RTLIL::SigSpec::operator <(ধ্রুব RTLIL::SigSpec &other) ধ্রুব
{
	যদি (এতত্ == &other)
		প্রত্যাগম অসত্য;

	যদি (size() != other.size())
		প্রত্যাগম size() < other.size();

	auto other_it = other.chunks().begin();
	প্রতি (ধ্রুব SigChunk &c : chunks()) {
		যদি (c != *other_it)
			প্রত্যাগম c < *other_it;
		++other_it;
	}

	প্রত্যাগম অসত্য;
}

তার্কিক RTLIL::SigSpec::operator ==(ধ্রুব RTLIL::SigSpec &other) ধ্রুব
{
	যদি (এতত্ == &other)
		প্রত্যাগম সত্য;

	যদি (size() != other.size())
		প্রত্যাগম অসত্য;

	auto other_it = other.chunks().begin();
	প্রতি (ধ্রুব SigChunk &c : chunks()) {
		যদি (c != *other_it)
			প্রত্যাগম অসত্য;
		++other_it;
	}

	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::SigSpec::is_wire() ধ্রুব
{
	Chunks cs = chunks();
	auto it = cs.begin();
	যদি (it == cs.end())
		প্রত্যাগম অসত্য;
	ধ্রুব RTLIL::SigChunk &chunk = *it;
	প্রত্যাগম chunk.wire && chunk.wire->width == size() && ++it == cs.end();
}

তার্কিক RTLIL::SigSpec::is_chunk() ধ্রুব
{
	Chunks cs = chunks();
	auto it = cs.begin();
	যদি (it == cs.end())
		প্রত্যাগম অসত্য;
	প্রত্যাগম ++it == cs.end();
}

তার্কিক RTLIL::SigSpec::known_driver() ধ্রুব
{
	প্রতি (auto &chunk : chunks())
		যদি (chunk.is_wire() && !chunk.wire->known_driver())
			প্রত্যাগম অসত্য;
	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::SigSpec::is_fully_const() ধ্রুব
{
	প্রতি (auto &chunk : chunks())
		যদি (chunk.width > 0 && chunk.wire != রিক্ত)
			প্রত্যাগম অসত্য;
	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::SigSpec::is_fully_zero() ধ্রুব
{
	প্রতি (auto &chunk : chunks()) {
		যদি (chunk.width > 0 && chunk.wire != রিক্ত)
			প্রত্যাগম অসত্য;
		প্রতি (RTLIL::State d : chunk.data)
			যদি (d != RTLIL::State::S0)
				প্রত্যাগম অসত্য;
	}
	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::SigSpec::is_fully_ones() ধ্রুব
{
	প্রতি (auto &chunk : chunks()) {
		যদি (chunk.width > 0 && chunk.wire != রিক্ত)
			প্রত্যাগম অসত্য;
		প্রতি (RTLIL::State d : chunk.data)
			যদি (d != RTLIL::State::S1)
				প্রত্যাগম অসত্য;
	}
	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::SigSpec::is_fully_def() ধ্রুব
{
	প্রতি (auto &chunk : chunks()) {
		যদি (chunk.width > 0 && chunk.wire != রিক্ত)
			প্রত্যাগম অসত্য;
		প্রতি (RTLIL::State d : chunk.data)
			যদি (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				প্রত্যাগম অসত্য;
	}
	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::SigSpec::is_fully_undef() ধ্রুব
{
	প্রতি (auto &chunk : chunks()) {
		যদি (chunk.width > 0 && chunk.wire != রিক্ত)
			প্রত্যাগম অসত্য;
		প্রতি (RTLIL::State d : chunk.data)
			যদি (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				প্রত্যাগম অসত্য;
	}
	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::SigSpec::has_const() ধ্রুব
{
	প্রতি (auto &chunk : chunks())
		যদি (chunk.width > 0 && chunk.wire == রিক্ত)
			প্রত্যাগম সত্য;
	প্রত্যাগম অসত্য;
}

তার্কিক RTLIL::SigSpec::has_const(State state) ধ্রুব
{
	প্রতি (auto &chunk : chunks())
		যদি (chunk.width > 0 && chunk.wire == রিক্ত && std::find(chunk.data.begin(), chunk.data.end(), state) != chunk.data.end())
			প্রত্যাগম সত্য;
	প্রত্যাগম অসত্য;
}


তার্কিক RTLIL::SigSpec::has_marked_bits() ধ্রুব
{
	প্রতি (auto &chunk : chunks())
		যদি (chunk.width > 0 && chunk.wire == রিক্ত) {
			প্রতি (RTLIL::State d : chunk.data)
				যদি (d == RTLIL::State::Sm)
					প্রত্যাগম সত্য;
		}
	প্রত্যাগম অসত্য;
}

তার্কিক RTLIL::SigSpec::is_onehot(পূর্ণাঙ্ক *pos) ধ্রুব
{
	যদি (std::optional<RTLIL::Const> c = try_as_const())
		প্রত্যাগম c->is_onehot(pos);
	প্রত্যাগম অসত্য;
}

তার্কিক RTLIL::SigSpec::as_bool() ধ্রুব
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_মান());
	প্রত্যাগম c->as_bool();
}

পূর্ণাঙ্ক RTLIL::SigSpec::as_int(তার্কিক is_signed) ধ্রুব
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_মান());
	প্রত্যাগম c->as_int(is_signed);
}

তার্কিক RTLIL::SigSpec::convertible_to_int(তার্কিক is_signed) ধ্রুব
{
	std::optional<RTLIL::Const> c = try_as_const();
	যদি (!c.has_মান())
		প্রত্যাগম অসত্য;
	প্রত্যাগম c->convertible_to_int(is_signed);
}

std::optional<পূর্ণাঙ্ক> RTLIL::SigSpec::try_as_int(তার্কিক is_signed) ধ্রুব
{
	std::optional<RTLIL::Const> c = try_as_const();
	যদি (!c.has_মান())
		প্রত্যাগম std::nullopt;
	প্রত্যাগম c->try_as_int(is_signed);
}

পূর্ণাঙ্ক RTLIL::SigSpec::as_int_saturating(তার্কিক is_signed) ধ্রুব
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_মান());
	প্রত্যাগম c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ধ্রুব
{
	std::string str;
	str.reserve(size());
	std::vector<RTLIL::SigChunk> chunks = *এতত্;
	প্রতি (size_t i = chunks.size(); i > 0; i--) {
		ধ্রুব RTLIL::SigChunk &chunk = chunks[i-1];
		যদি (chunk.wire != রিক্ত)
			str.append(chunk.width, '?');
		অন্যথা
			str += RTLIL::Const(chunk.data).as_string();
	}
	প্রত্যাগম str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ধ্রুব
{
	Chunks cs = chunks();
	auto it = cs.begin();
	যদি (it == cs.end())
		প্রত্যাগম RTLIL::Const();
	SigChunk chunk = *it;
	যদি (chunk.wire != রিক্ত || ++it != cs.end())
		প্রত্যাগম std::nullopt;
	প্রত্যাগম RTLIL::Const(std::move(chunk.data));
}

RTLIL::Const RTLIL::SigSpec::as_const() ধ্রুব
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_মান());
	প্রত্যাগম *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ধ্রুব
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == size());
	প্রত্যাগম chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ধ্রুব
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	প্রত্যাগম chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() ধ্রুব
{
	প্রত্যাগম RTLIL::SigBit(*এতত্);
}

তার্কিক RTLIL::SigSpec::match(ধ্রুব বর্ণ* pattern) ধ্রুব
{
	পূর্ণাঙ্ক pattern_len = strlen(pattern);
	log_assert(pattern_len == size());

	প্রতি (পূর্ণাঙ্ক i = 0; i < pattern_len; ++i) {
		বর্ণ ch = pattern[i];
		যদি (ch == ' ')
			অনুবর্ত;
		RTLIL::SigBit bit = (*এতত্)[pattern_len - 1 - i];
		যদি (ch == '*') {
			যদি (bit != State::Sz && bit != State::Sx)
				প্রত্যাগম অসত্য;
			অনুবর্ত;
		}
		যদি (ch == '0') {
			যদি (bit != State::S0)
				প্রত্যাগম অসত্য;
		} অন্যথা
		যদি (ch == '1') {
			যদি (bit != State::S1)
				প্রত্যাগম অসত্য;
		} অন্যথা
			log_abort();
	}

	প্রত্যাগম সত্য;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() ধ্রুব
{
	std::set<RTLIL::SigBit> sigbits;
	প্রতি (auto &c : chunks())
		প্রতি (পূর্ণাঙ্ক i = 0; i < c.width; i++)
			sigbits.ঢোকাও(RTLIL::SigBit(c, i));
	প্রত্যাগম sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() ধ্রুব
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(size());
	প্রতি (auto &c : chunks())
		প্রতি (পূর্ণাঙ্ক i = 0; i < c.width; i++)
			sigbits.ঢোকাও(RTLIL::SigBit(c, i));
	প্রত্যাগম sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() ধ্রুব
{
	std::vector<RTLIL::SigBit> result;
	result.reserve(size());
	প্রতি (SigBit bit : *এতত্)
		result.ঠেলো_back(bit);
	প্রত্যাগম result;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(ধ্রুব RTLIL::SigSpec &other) ধ্রুব
{
	পূর্ণাঙ্ক this_size = size();
	log_assert(this_size == other.size());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	প্রতি (পূর্ণাঙ্ক i = 0; i < this_size; i++)
		new_map[(*এতত্)[i]] = other[i];

	প্রত্যাগম new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(ধ্রুব RTLIL::SigSpec &other) ধ্রুব
{
	পূর্ণাঙ্ক this_size = size();
	log_assert(this_size == other.size());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_size);
	প্রতি (পূর্ণাঙ্ক i = 0; i < this_size; i++)
		new_map[(*এতত্)[i]] = other[i];

	প্রত্যাগম new_map;
}

স্থির শূন্য sigspec_parse_split(std::vector<std::string> &tokens, ধ্রুব std::string &text, বর্ণ sep)
{
	size_t start = 0, end = 0;
	যাবত্ ((end = text.find(sep, start)) != std::string::npos) {
		tokens.ঠেলো_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.ঠেলো_back(text.substr(start));
}

স্থির RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ধ্রুব std::string &netname)
{
	প্রত্যাগম module->wire(netname);
}

তার্কিক RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	প্রতি (পূর্ণাঙ্ক tokidx = পূর্ণাঙ্ক(tokens.size())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		যদি (netname.size() == 0)
			অনুবর্ত;

		যদি (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			auto ast = p.const2ast(netname);
			যদি (ast == nullptr)
				প্রত্যাগম অসত্য;
			sig.append(RTLIL::Const(ast->bits));
			অনুবর্ত;
		}

		যদি (module == রিক্ত)
			প্রত্যাগম অসত্য;

		যদি (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		যদি (sigspec_parse_wire(module, netname) == nullptr) {
			size_t indices_pos = netname.size()-1;
			যদি (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				যাবত্ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				যদি (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					যাবত্ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				যদি (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		যদি (wire == nullptr)
			প্রত্যাগম অসত্য;

		যদি (!indices.empty()) {
			std::vector<std::string> index_tokens;
			sigspec_parse_split(index_tokens, indices.substr(1, indices.size()-2), ':');
			যদি (index_tokens.size() == 1) {
				পূর্ণাঙ্ক a = atoi(index_tokens.at(0).c_str());
				যদি (a < 0 || a >= wire->width)
					প্রত্যাগম অসত্য;
				sig.append(RTLIL::SigSpec(wire, a));
			} অন্যথা {
				পূর্ণাঙ্ক a = atoi(index_tokens.at(0).c_str());
				পূর্ণাঙ্ক b = atoi(index_tokens.at(1).c_str());
				যদি (a > b) {
					পূর্ণাঙ্ক tmp = a;
					a = b, b = tmp;
				}
				যদি (a < 0 || a >= wire->width)
					প্রত্যাগম অসত্য;
				যদি (b < 0 || b >= wire->width)
					প্রত্যাগম অসত্য;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} অন্যথা
			sig.append(wire);
	}

	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	যদি (str.empty() || str[0] != '@')
		প্রত্যাগম parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	যদি (sel_name == IdString::Null || design->selection_vars.গুনতি(sel_name) == 0)
		প্রত্যাগম অসত্য;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	প্রতি (auto &it : module->wires_)
		যদি (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	প্রত্যাগম সত্য;
}

তার্কিক RTLIL::SigSpec::parse_rhs(ধ্রুব RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	যদি (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.size());
		প্রত্যাগম সত্য;
	}

	যদি (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.size());
		প্রত্যাগম সত্য;
	}

	যদি (lhs.is_chunk()) {
		বর্ণ *p = (বর্ণ*)str.c_str(), *endptr;
		long পূর্ণাঙ্ক val = strtol(p, &endptr, 10);
		যদি (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.size());
			প্রত্যাগম সত্য;
		}
	}

	যদি (!parse(sig, module, str))
		প্রত্যাগম অসত্য;
	যদি (sig.size() > lhs.size())
		sig.remove(lhs.size(), sig.size() - lhs.size());
	প্রত্যাগম সত্য;
}

RTLIL::SigSpec::operator std::vector<RTLIL::SigChunk>() ধ্রুব
{
	std::vector<RTLIL::SigChunk> result;
	প্রতি (ধ্রুব RTLIL::SigChunk &c : chunks())
		result.ঠেলো_back(c);
	প্রত্যাগম result;
}

RTLIL::CaseRule::~CaseRule()
{
	প্রতি (auto it = switches.begin(); it != switches.end(); it++)
		delete *it;
}

তার্কিক RTLIL::CaseRule::empty() ধ্রুব
{
	প্রত্যাগম actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() ধ্রুব
{
	RTLIL::CaseRule *new_caserule = নূতন RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	প্রতি (auto &it : switches)
		new_caserule->switches.ঠেলো_back(it->clone());
	প্রত্যাগম new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	প্রতি (auto it = cases.begin(); it != cases.end(); it++)
		delete *it;
}

তার্কিক RTLIL::SwitchRule::empty() ধ্রুব
{
	প্রত্যাগম cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() ধ্রুব
{
	RTLIL::SwitchRule *new_switchrule = নূতন RTLIL::SwitchRule;
	new_switchrule->সঙ্কেত = সঙ্কেত;
	new_switchrule->attributes = attributes;
	প্রতি (auto &it : cases)
		new_switchrule->cases.ঠেলো_back(it->clone());
	প্রত্যাগম new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() ধ্রুব
{
	RTLIL::SyncRule *new_syncrule = নূতন RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->সঙ্কেত = সঙ্কেত;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	প্রত্যাগম new_syncrule;
}

RTLIL::Process::~Process()
{
	প্রতি (auto it = syncs.begin(); it != syncs.end(); it++)
		delete *it;
}

RTLIL::Process *RTLIL::Process::clone() ধ্রুব
{
	RTLIL::Process *new_proc = নূতন RTLIL::Process;

	new_proc->attributes = attributes;

	RTLIL::CaseRule *rc_ptr = মূল_case.clone();
	new_proc->মূল_case = *rc_ptr;
	rc_ptr->switches.clear();
	delete rc_ptr;

	প্রতি (auto &it : syncs)
		new_proc->syncs.ঠেলো_back(it->clone());

	প্রত্যাগম new_proc;
}

RTLIL::Memory::~Memory()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
স্থির std::map<unsigned পূর্ণাঙ্ক, RTLIL::Memory*> all_memorys;
std::map<unsigned পূর্ণাঙ্ক, RTLIL::Memory*> *RTLIL::Memory::get_all_memorys(শূন্য)
{
	প্রত্যাগম &all_memorys;
}
#endif
YOSYS_NAMESPACE_END
