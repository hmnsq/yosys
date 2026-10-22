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

#அந்தர்பாவ "kernel/rtlil.h"
#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "kernel/macc.h"
#அந்தர்பாவ "kernel/newcelltypes.h"
#அந்தர்பாவ "kernel/sigtools.h"
#அந்தர்பாவ "kernel/threading.h"
#அந்தர்பாவ "frontends/verilog/verilog_frontend.h"
#அந்தர்பாவ "frontends/verilog/preproc.h"
#அந்தர்பாவ "backends/rtlil/rtlil_backend.h"

#அந்தர்பாவ <string.h>
#அந்தர்பாவ <algorithm>
#அந்தர்பாவ <charconv>
#அந்தர்பாவ <optional>
#அந்தர்பாவ <string_view>
#அந்தர்பாவ <sstream>

YOSYS_NAMESPACE_BEGIN

dict<std::string, std::string> RTLIL::constpad;

மாறிலி pool<IdString> &RTLIL::builtin_ff_cell_types() {
	நிலையான மாறிலி pool<IdString> res = []() {
		pool<IdString> r;
		க்கு (size_t i = 0; i < StaticCellTypes::builder.மொத்தம்; i++) {
			auto &cell = StaticCellTypes::builder.cells[i];
			எனில் (cell.features.is_ff)
				r.செருகு(cell.type);
		}
		திரும்பு r;
	}();
	திரும்பு res;
}

#define check(condition) log_assert(condition && "malformed Const union")

மாறிலி Const::bitvectype& Const::get_bits() மாறிலி {
	check(is_bits());
	திரும்பு *get_if_bits();
}

மாறிலி std::string& Const::get_str() மாறிலி {
	check(is_str());
	திரும்பு *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	திரும்பு *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	திரும்பு *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	புதிய ((வெறுமை*)&str_) std::string(std::move(str));
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	எழுத்து bytes[] = {
		(எழுத்து)(val >> 24), (எழுத்து)(val >> 16), (எழுத்து)(val >> 8), (எழுத்து)val
	};
	புதிய ((வெறுமை*)&str_) std::string(bytes, 4);
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val, முழு width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	எனில் ((width & 7) == 0) {
		புதிய ((வெறுமை*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		முழு bytes = width >> 3;
		signed எழுத்து sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<முழு>(bytes, அளவு(val));
		க்கு (முழு i = 0; i < bytes; i++) {
			str[str.size() - 1 - i] = val;
			val = val >> 8;
		}
		திரும்பு;
	}

	புதிய ((வெறுமை*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	க்கு (முழு i = 0; i < width; i++) {
		bv.தள்ளு_back((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, முழு width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	புதிய ((வெறுமை*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	க்கு (முழு i = 0; i < width; i++)
		bv.தள்ளு_back(bit);
}

RTLIL::Const::Const(மாறிலி std::vector<தருக்க> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	புதிய ((வெறுமை*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.size());
	க்கு (மாறிலி auto &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(மாறிலி RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	எனில் (is_str())
		புதிய ((வெறுமை*)&str_) std::string(other.get_str());
	இல்லையெனில் எனில் (is_bits())
		புதிய ((வெறுமை*)&bits_) bitvectype(other.get_bits());
	இல்லையெனில்
		check(பொய்);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	எனில் (is_str())
		புதிய ((வெறுமை*)&str_) std::string(std::move(other.get_str()));
	இல்லையெனில் எனில் (is_bits())
		புதிய ((வெறுமை*)&bits_) bitvectype(std::move(other.get_bits()));
	இல்லையெனில்
		check(பொய்);
}

RTLIL::Const &RTLIL::Const::operator =(மாறிலி RTLIL::Const &other) {
	flags = other.flags;
	எனில் (other.is_str()) {
		எனில் (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(வெறுமை)புதிய ((வெறுமை*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} இல்லையெனில் எனில் (other.is_bits()) {
		எனில் (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(வெறுமை)புதிய ((வெறுமை*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} இல்லையெனில் {
		check(பொய்);
	}
	திரும்பு *இது;
}

RTLIL::Const::~Const() {
	எனில் (is_bits())
		bits_.~bitvectype();
	இல்லையெனில் எனில் (is_str())
		str_.~string();
	இல்லையெனில்
		check(பொய்);
}

தருக்க RTLIL::Const::operator<(மாறிலி RTLIL::Const &other) மாறிலி
{
	எனில் (size() != other.size())
		திரும்பு size() < other.size();

	க்கு (முழு i = 0; i < size(); i++)
		எனில் ((*இது)[i] != other[i])
			திரும்பு (*இது)[i] < other[i];

	திரும்பு பொய்;
}

தருக்க RTLIL::Const::operator ==(மாறிலி RTLIL::Const &other) மாறிலி
{
	எனில் (is_str() && other.is_str())
		திரும்பு get_str() == other.get_str();
	எனில் (is_bits() && other.is_bits())
		திரும்பு get_bits() == other.get_bits();

	எனில் (size() != other.size())
		திரும்பு பொய்;

	க்கு (முழு i = 0; i < size(); i++)
	எனில் ((*இது)[i] != other[i])
		திரும்பு பொய்;

	திரும்பு உண்மை;
}

தருக்க RTLIL::Const::operator !=(மாறிலி RTLIL::Const &other) மாறிலி
{
	திரும்பு !(*இது == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	திரும்பு get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() மாறிலி
{
	std::vector<State> v;
	க்கு (auto bit : *இது)
		v.தள்ளு_back(bit);
	திரும்பு v;
}

தருக்க RTLIL::Const::as_bool() மாறிலி
{
	எனில் (is_str()) {
		க்கு (எழுத்து ch : get_str())
			எனில் (ch != 0)
				திரும்பு உண்மை;
		திரும்பு பொய்;
	}

	மாறிலி bitvectype& bv = get_bits();
	க்கு (size_t i = 0; i < bv.size(); i++)
		எனில் (bv[i] == State::S1)
			திரும்பு உண்மை;
	திரும்பு பொய்;
}

முழு RTLIL::Const::as_int(தருக்க is_signed) மாறிலி
{
	int32_t ret = 0;
	எனில் (மாறிலி std::string *s = get_if_str()) {
		முழு size = GetSize(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		க்கு (முழு i = std::min(4, size); i > 0; i--)
			ret |= static_cast<unsigned எழுத்து>((*s)[size - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		எனில் (is_signed && size > 0 && size < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << size*8;
		திரும்பு ret;
	}

	மாறிலி bitvectype& bv = get_bits();
	முழு significant_bits = std::min(GetSize(bv), 32);
	க்கு (முழு i = 0; i < significant_bits; i++)
		எனில் (bv[i] == State::S1)
			ret |= 1 << i;
	எனில் (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	திரும்பு ret;
}

தருக்க RTLIL::Const::convertible_to_int(தருக்க is_signed) மாறிலி
{
	auto size = get_min_size(is_signed);

	எனில் (size < 0)
		திரும்பு பொய்;

	// If it fits in 31 bits it is definitely convertible
	எனில் (size <= 31)
		திரும்பு உண்மை;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	எனில் (size == 32) {
		எனில் (is_signed)
			திரும்பு உண்மை;
		திரும்பு (*இது)[size - 1] != State::S1;
	}

	திரும்பு பொய்;
}

std::optional<முழு> RTLIL::Const::try_as_int(தருக்க is_signed) மாறிலி
{
	எனில் (!convertible_to_int(is_signed))
		திரும்பு std::nullopt;
	திரும்பு as_int(is_signed);
}

முழு RTLIL::Const::as_int_saturating(தருக்க is_signed) மாறிலி
{
	எனில் (!convertible_to_int(is_signed)) {
		எனில் (!is_signed)
			திரும்பு std::numeric_limits<முழு>::max();

		மாறிலி auto min_size = get_min_size(is_signed);
		log_assert(min_size > 0);
		மாறிலி auto neg = (*இது)[min_size - 1];
		திரும்பு neg ? std::numeric_limits<முழு>::min() : std::numeric_limits<முழு>::max();
	}
	திரும்பு as_int(is_signed);
}

வெறுமை RTLIL::Const::tag_bare_integer_const(மாறிலி std::string &மதிப்பு)
{
	எனில் (மதிப்பு.empty() || மதிப்பு.find('\'') != std::string::npos)
		திரும்பு;
	size_t start = (மதிப்பு[0] == '-' || மதிப்பு[0] == '+') ? 1 : 0;
	எனில் (start < மதிப்பு.size() && std::all_of(மதிப்பு.begin() + start, மதிப்பு.end(), ::isdigit))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

முழு RTLIL::Const::get_min_size(தருக்க is_signed) மாறிலி
{
	எனில் (empty()) திரும்பு 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	எனில் (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	இல்லையெனில்
		leading_bit = RTLIL::State::S0;

	auto idx = size();
	போது (idx > 0 && (*இது)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	எனில் (is_signed && idx < size()) {
		idx++;
	}
	// must be at least one bit
	திரும்பு (idx == 0) ? 1 : idx;
}

வெறுமை RTLIL::Const::compress(தருக்க is_signed)
{
	auto idx = get_min_size(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<முழு> RTLIL::Const::as_int_compress(தருக்க is_signed) மாறிலி
{
	திரும்பு try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(மாறிலி எழுத்து* any) மாறிலி
{
	முழு sz = size();
	std::string ret;
	ret.reserve(sz);
	க்கு (முழு i = sz - 1; i >= 0; --i)
		தேர்வு ((*இது)[i]) {
			நிலை S0: ret.தள்ளு_back('0'); நிறுத்து;
			நிலை S1: ret.தள்ளு_back('1'); நிறுத்து;
			நிலை Sx: ret.தள்ளு_back('x'); நிறுத்து;
			நிலை Sz: ret.தள்ளு_back('z'); நிறுத்து;
			நிலை Sa: ret += any; நிறுத்து;
			நிலை Sm: ret.தள்ளு_back('m'); நிறுத்து;
		}
	திரும்பு ret;
}

RTLIL::Const RTLIL::Const::from_string(மாறிலி std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.size());
	க்கு (auto it = str.rbegin(); it != str.rend(); it++)
		தேர்வு (*it) {
			நிலை '0': bv.தள்ளு_back(State::S0); நிறுத்து;
			நிலை '1': bv.தள்ளு_back(State::S1); நிறுத்து;
			நிலை 'x': bv.தள்ளு_back(State::Sx); நிறுத்து;
			நிலை 'z': bv.தள்ளு_back(State::Sz); நிறுத்து;
			நிலை 'm': bv.தள்ளு_back(State::Sm); நிறுத்து;
			இயல்பு: bv.தள்ளு_back(State::Sa);
		}
	திரும்பு c;
}

std::string RTLIL::Const::decode_string() மாறிலி
{
	எனில் (auto str = get_if_str())
		திரும்பு *str;

	மாறிலி bitvectype& bv = get_bits();
	மாறிலி முழு n = GetSize(bv);
	மாறிலி முழு n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	முழு i = n_over_8 * 8;
	எனில் (i < n) {
		எழுத்து ch = 0;
		க்கு (முழு j = 0; j < (n - i); j++) {
			எனில் (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		எனில் (ch != 0)
			s.append({ch});
	}
	i -= 8;
	க்கு (; i >= 0; i -= 8) {
		எழுத்து ch = 0;
		க்கு (முழு j = 0; j < 8; j++) {
			எனில் (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		எனில் (ch != 0)
			s.append({ch});
	}
	திரும்பு s;
}

முழு RTLIL::Const::size() மாறிலி {
	எனில் (is_str())
		திரும்பு 8 * str_.size();
	இல்லையெனில் {
		check(is_bits());
		திரும்பு bits_.size();
	}
}

தருக்க RTLIL::Const::empty() மாறிலி {
	எனில் (is_str())
		திரும்பு str_.empty();
	இல்லையெனில் {
		check(is_bits());
		திரும்பு bits_.empty();
	}
}

வெறுமை RTLIL::Const::bitvectorize_internal() {
	எனில் (tag == backing_tag::bits)
		திரும்பு;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.size() * 8);
	க்கு (முழு i = str_.size() - 1; i >= 0; i--) {
		unsigned எழுத்து ch = str_[i];
		க்கு (முழு j = 0; j < 8; j++) {
			new_bits.தள்ளு_back((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(வெறுமை)புதிய ((வெறுமை*)&bits_) bitvectype(std::move(new_bits));
		tag = backing_tag::bits;
	}
}

வெறுமை RTLIL::Const::append(மாறிலி RTLIL::Const &other) {
	bitvectorize_internal();
	bitvectype& bv = get_bits();
	bv.செருகு(bv.end(), other.begin(), other.end());
}

RTLIL::State RTLIL::Const::const_iterator::operator*() மாறிலி {
	எனில் (auto bv = parent->get_if_bits())
		திரும்பு (*bv)[idx];

	முழு char_idx = parent->get_str().size() - idx / 8 - 1;
	தருக்க bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	திரும்பு bit ? State::S1 : State::S0;
}

தருக்க RTLIL::Const::is_fully_zero() மாறிலி
{
	எனில் (auto str = get_if_str()) {
		க்கு (எழுத்து ch : *str)
			எனில் (ch != 0)
				திரும்பு பொய்;
		திரும்பு உண்மை;
	}

	மாறிலி bitvectype& bv = get_bits();

	க்கு (மாறிலி auto &bit : bv)
		எனில் (bit != RTLIL::State::S0)
			திரும்பு பொய்;

	திரும்பு உண்மை;
}

தருக்க RTLIL::Const::is_fully_ones() மாறிலி
{
	எனில் (auto str = get_if_str()) {
		க்கு (எழுத்து ch : *str)
			எனில் (ch != (எழுத்து)0xff)
				திரும்பு பொய்;
		திரும்பு உண்மை;
	}

	மாறிலி bitvectype& bv = get_bits();
	க்கு (மாறிலி auto &bit : bv)
		எனில் (bit != RTLIL::State::S1)
			திரும்பு பொய்;

	திரும்பு உண்மை;
}

தருக்க RTLIL::Const::is_fully_def() மாறிலி
{
	எனில் (is_str())
		திரும்பு உண்மை;

	மாறிலி bitvectype& bv = get_bits();
	க்கு (மாறிலி auto &bit : bv)
		எனில் (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			திரும்பு பொய்;

	திரும்பு உண்மை;
}

தருக்க RTLIL::Const::is_fully_undef() மாறிலி
{
	எனில் (auto str = get_if_str())
		திரும்பு str->empty();

	மாறிலி bitvectype& bv = get_bits();
	க்கு (மாறிலி auto &bit : bv)
		எனில் (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			திரும்பு பொய்;

	திரும்பு உண்மை;
}

தருக்க RTLIL::Const::is_fully_undef_x_only() மாறிலி
{
	எனில் (auto str = get_if_str())
		திரும்பு str->empty();

	மாறிலி bitvectype& bv = get_bits();
	க்கு (மாறிலி auto &bit : bv)
		எனில் (bit != RTLIL::State::Sx)
			திரும்பு பொய்;

	திரும்பு உண்மை;
}

தருக்க RTLIL::Const::is_onehot(முழு *pos) மாறிலி
{
	தருக்க found = பொய்;
	முழு size = GetSize(*இது);
	க்கு (முழு i = 0; i < size; i++) {
		State bit = (*இது)[i];
		எனில் (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			திரும்பு பொய்;
		எனில் (bit == RTLIL::State::S1) {
			எனில் (found)
				திரும்பு பொய்;
			எனில் (pos)
				*pos = i;
			found = உண்மை;
		}
	}
	திரும்பு found;
}

Hasher RTLIL::Const::hash_into(Hasher h) மாறிலி
{
	எனில் (auto str = get_if_str())
		திரும்பு hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	மாறிலி bitvectype &bv = get_bits();
	முழு size = GetSize(bv);
	std::string packed;
	முழு packed_size = (size + 7) >> 3;
	packed.resize(packed_size, 0);
	க்கு (முழு bi = 0; bi < packed_size; ++bi) {
		எழுத்து ch = 0;
		முழு end = std::min((bi + 1)*8, size);
		க்கு (முழு i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			எனில் (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<எழுத்து>::hash_into(ch, h);
				க்கு (; i < size; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(size);
				திரும்பு h;
			}
			ch |= static_cast<முழு>(b) << (i & 7);
		}
		packed[packed_size - 1 - bi] = ch;
	}
	திரும்பு hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(முழு offset, முழு len, RTLIL::State padding) மாறிலி {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	க்கு (முழு i = offset; i < offset + len; i++)
		ret_bv.தள்ளு_back(i < GetSize(*இது) ? (*இது)[i] : padding);
	திரும்பு RTLIL::Const(ret_bv);
}
#undef check /* check(condition) for Const */

தருக்க RTLIL::AttrObject::has_attribute(RTLIL::IdString id) மாறிலி
{
	திரும்பு attributes.மொத்தம்(id);
}

வெறுமை RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, தருக்க மதிப்பு)
{
	எனில் (மதிப்பு)
		attributes[id] = RTLIL::Const(1);
	இல்லையெனில்
		attributes.erase(id);
}

தருக்க RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) மாறிலி
{
	மாறிலி auto it = attributes.find(id);
	எனில் (it == attributes.end())
		திரும்பு பொய்;
	திரும்பு it->second.as_bool();
}

வெறுமை RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string மதிப்பு)
{
	எனில் (மதிப்பு.empty())
		attributes.erase(id);
	இல்லையெனில்
		attributes[id] = மதிப்பு;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) மாறிலி
{
	std::string மதிப்பு;
	மாறிலி auto it = attributes.find(id);
	எனில் (it != attributes.end())
		மதிப்பு = it->second.decode_string();
	திரும்பு மதிப்பு;
}

வெறுமை RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, மாறிலி pool<string> &data)
{
	string attrval;
	க்கு (மாறிலி auto &s : data) {
		எனில் (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

வெறுமை RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, மாறிலி pool<string> &data)
{
	pool<string> union_data = get_strpool_attribute(id);
	union_data.செருகு(data.begin(), data.end());
	எனில் (!union_data.empty())
		set_strpool_attribute(id, union_data);
}

வெறுமை RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->attributes = std::move(buf);
}

நாமாகாஶ {
	template<typename F>
	வெறுமை walk_process_attr_objects(RTLIL::Process *process, F visit) {
		visit(process);
		std::vector<RTLIL::CaseRule*> case_அடுக்கு{&process->வேர்_case};
		போது (!case_அடுக்கு.empty()) {
			RTLIL::CaseRule *cs = case_அடுக்கு.back();
			case_அடுக்கு.எடு_back();
			visit(cs);
			க்கு (auto *sw : cs->switches) {
				visit(sw);
				க்கு (auto *case_ : sw->cases)
					case_அடுக்கு.தள்ளு_back(case_);
			}
		}
		க்கு (auto *ஒத்திசை : process->syncs)
			க்கு (auto &mwa : ஒத்திசை->mem_write_actions)
				visit(&mwa);
	}

	template<typename F>
	வெறுமை walk_attr_objects(RTLIL::Design *design, F visit) {
		க்கு (auto &[_, module] : design->modules_) {
			visit(module);
			க்கு (auto &[_, wire] : module->wires_)
				visit(wire);
			க்கு (auto &[_, mem] : module->memories)
				visit(mem);
			க்கு (auto &[_, cell] : module->cells_)
				visit(cell);
			க்கு (auto &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


size_t RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	auto வேர் = [&](IdString ref) {
		எனில் (ref != IdString::Null)
			live.செருகு(ref);
	};

	walk_attr_objects(இது, [&](மாறிலி RTLIL::AttrObject *obj) {
		க்கு (auto &attr : obj->attributes)
			வேர்(attr.first);
	});

	வேர்(selected_active_module);

	க்கு (auto &[name, module] : modules_) {
		வேர்(name);
		க்கு (IdString port : module->ports)
			வேர்(port);
		க்கு (IdString param : module->avail_parameters)
			வேர்(param);
		க்கு (auto &[param, _] : module->parameter_default_மதிப்புகள்)
			வேர்(param);
		க்கு (auto &[name, _] : module->memories)
			வேர்(name);
		க்கு (auto &[name, _] : module->processes)
			வேர்(name);
		க்கு (auto &[name, wire] : module->wires_) {
			வேர்(name);
			எனில் (wire->known_driver())
				வேர்(wire->driverPort());
		}
		க்கு (auto &[name, cell] : module->cells_) {
			வேர்(name);
			வேர்(cell->type);
			க்கு (auto &conn : cell->connections())
				வேர்(conn.first);
			க்கு (auto &param : cell->parameters)
				வேர்(param.first);
		}
		க்கு (auto &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_அடுக்கு{&process->வேர்_case};
			போது (!case_அடுக்கு.empty()) {
				RTLIL::CaseRule *cs = case_அடுக்கு.back();
				case_அடுக்கு.எடு_back();
				க்கு (auto *sw : cs->switches) {
					க்கு (auto *case_ : sw->cases)
						case_அடுக்கு.தள்ளு_back(case_);
				}
			}
			க்கு (auto *ஒத்திசை : process->syncs) {
				க்கு (auto &mwa : ஒத்திசை->mem_write_actions)
					வேர்(mwa.memid);
			}
		}
	}

	auto வேர்_selection = [&](மாறிலி RTLIL::Selection &sel) {
		க்கு (IdString m : sel.selected_modules)
			வேர்(m);
		க்கு (auto &[m, members] : sel.selected_members) {
			வேர்(m);
			க்கு (IdString member : members)
				வேர்(member);
		}
	};

	க்கு (auto &[name, sel] : selection_vars) {
		வேர்(name);
		வேர்_selection(sel);
	}

	க்கு (auto &sel : selection_அடுக்கு)
		வேர்_selection(sel);

	size_t erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_மொத்தம்;
	திரும்பு erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) மாறிலி
{
	pool<string> data;
	எனில் (attributes.மொத்தம்(id) != 0)
		க்கு (auto s : split_tokens(get_string_attribute(id), "|"))
			data.செருகு(s);
	திரும்பு data;
}

வெறுமை RTLIL::AttrObject::set_hdlname_attribute(மாறிலி vector<string> &hierarchy)
{
	string attrval;
	க்கு (மாறிலி auto &ident : hierarchy) {
		எனில் (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() மாறிலி
{
	திரும்பு split_tokens(get_string_attribute(ID::hdlname), " ");
}

வெறுமை RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, மாறிலி vector<முழு> &data)
{
	std::stringstream attrval;
	க்கு (auto &i : data) {
		எனில் (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<முழு> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) மாறிலி
{
	vector<முழு> data;
	auto it = attributes.find(id);
	எனில் (it != attributes.end())
		க்கு (மாறிலி auto &s : split_tokens(attributes.at(id).decode_string())) {
			எழுத்து *end = nullptr;
			errno = 0;
			long மதிப்பு = strtol(s.c_str(), &end, 10);
			எனில் (end != s.c_str() + s.size())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			எனில் (errno == ERANGE || மதிப்பு < INT_MIN || மதிப்பு > INT_MAX)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			data.தள்ளு_back(மதிப்பு);
		}
	திரும்பு data;
}

தருக்க RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) மாறிலி
{
	எனில் (current_design != nullptr) {
		auto module = current_design->module(mod_name);
		திரும்பு module && module->get_blackbox_attribute();
	} இல்லையெனில் {
		log_warning("Unable to check if module is boxed for null design.\n");
		திரும்பு பொய்;
	}
}

தருக்க RTLIL::Selection::selected_module(RTLIL::IdString mod_name) மாறிலி
{
	எனில் (complete_selection)
		திரும்பு உண்மை;
	எனில் (!selects_boxes && boxed_module(mod_name))
		திரும்பு பொய்;
	எனில் (full_selection)
		திரும்பு உண்மை;
	எனில் (selected_modules.மொத்தம்(mod_name) > 0)
		திரும்பு உண்மை;
	எனில் (selected_members.மொத்தம்(mod_name) > 0)
		திரும்பு உண்மை;
	திரும்பு பொய்;
}

தருக்க RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) மாறிலி
{
	எனில் (complete_selection)
		திரும்பு உண்மை;
	எனில் (!selects_boxes && boxed_module(mod_name))
		திரும்பு பொய்;
	எனில் (full_selection)
		திரும்பு உண்மை;
	எனில் (selected_modules.மொத்தம்(mod_name) > 0)
		திரும்பு உண்மை;
	திரும்பு பொய்;
}

தருக்க RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) மாறிலி
{
	எனில் (complete_selection)
		திரும்பு உண்மை;
	எனில் (!selects_boxes && boxed_module(mod_name))
		திரும்பு பொய்;
	எனில் (full_selection)
		திரும்பு உண்மை;
	எனில் (selected_modules.மொத்தம்(mod_name) > 0)
		திரும்பு உண்மை;
	எனில் (selected_members.மொத்தம்(mod_name) > 0)
		எனில் (selected_members.at(mod_name).மொத்தம்(memb_name) > 0)
			திரும்பு உண்மை;
	திரும்பு பொய்;
}

வெறுமை RTLIL::Selection::optimize(RTLIL::Design *design)
{
	எனில் (design != current_design) {
		current_design = design;
	}

	எனில் (selects_boxes && full_selection)
		complete_selection = உண்மை;
	எனில் (complete_selection) {
		full_selection = பொய்;
		selects_boxes = உண்மை;
	}
	எனில் (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		திரும்பு;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.clear();
	க்கு (auto mod_name : selected_modules) {
		எனில் (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.தள்ளு_back(mod_name);
		selected_members.erase(mod_name);
	}
	க்கு (auto mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	க்கு (auto &it : selected_members)
		எனில் (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.தள்ளு_back(it.first);
	க்கு (auto mod_name : del_list)
		selected_members.erase(mod_name);

	க்கு (auto &it : selected_members) {
		del_list.clear();
		க்கு (auto memb_name : it.second)
			எனில் (current_design->module(it.first)->மொத்தம்_id(memb_name) == 0)
				del_list.தள்ளு_back(memb_name);
		க்கு (auto memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	add_list.clear();
	க்கு (auto &it : selected_members)
		எனில் (it.second.size() == 0)
			del_list.தள்ளு_back(it.first);
		இல்லையெனில் எனில் (it.second.size() == current_design->module(it.first)->wires_.size() + current_design->module(it.first)->memories.size() +
				current_design->module(it.first)->cells_.size() + current_design->module(it.first)->processes.size())
			add_list.தள்ளு_back(it.first);
	க்கு (auto mod_name : del_list)
		selected_members.erase(mod_name);
	க்கு (auto mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.செருகு(mod_name);
	}

	எனில் (selected_modules.size() == current_design->modules_.size()) {
		selected_modules.clear();
		selected_members.clear();
		எனில் (selects_boxes)
			complete_selection = உண்மை;
		இல்லையெனில்
			full_selection = உண்மை;
	}
}

வெறுமை RTLIL::Selection::clear()
{
	full_selection = பொய்;
	complete_selection = பொய்;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_defines (புதிய define_map_t)
{
	நிலையான unsigned முழு hashidx_மொத்தம் = 123456789;
	hashidx_மொத்தம் = mkhash_xorshift(hashidx_மொத்தம்);
	hashidx_ = hashidx_மொத்தம்;

	refcount_modules_ = 0;
	selected_active_module.clear();
	தள்ளு_full_selection();

	RTLIL::Design::get_all_designs()->செருகு(std::pair<unsigned முழு, RTLIL::Design*>(hashidx_, இது));
}

RTLIL::Design::~Design()
{
	க்கு (auto &pr : modules_)
		delete pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

நிலையான std::map<unsigned முழு, RTLIL::Design*> all_designs;
std::map<unsigned முழு, RTLIL::Design*> *RTLIL::Design::get_all_designs(வெறுமை)
{
	திரும்பு &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	திரும்பு RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

மாறிலி RTLIL::Module *RTLIL::Design::module(IdString id) மாறிலி {
	twines_.check_owned(id);
	திரும்பு modules_.மொத்தம்(id) ? modules_.at(id) : வெற்று;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	திரும்பு modules_.மொத்தம்(id) ? modules_.at(id) : வெற்று;
}

RTLIL::Module *RTLIL::Design::உச்சி_module() மாறிலி
{
	RTLIL::Module *module = nullptr;
	முழு module_மொத்தம் = 0;

	க்கு (auto mod : selected_modules()) {
		எனில் (mod->get_bool_attribute(ID::உச்சி))
			திரும்பு mod;
		module_மொத்தம்++;
		module = mod;
	}

	திரும்பு module_மொத்தம் == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	எனில் (modules_.மொத்தம்(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = புதிய RTLIL::Module;
	modules_[name] = module;
	module->design = இது;
	module->name = name;

	க்கு (auto mon : monitors)
		mon->notify_module_add(module);

	எனில் (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	திரும்பு module;
}

வெறுமை RTLIL::Design::scratchpad_unset(மாறிலி std::string &varname)
{
	scratchpad.erase(varname);
}

வெறுமை RTLIL::Design::scratchpad_set_int(மாறிலி std::string &varname, முழு மதிப்பு)
{
	scratchpad[varname] = stringf("%d", மதிப்பு);
}

வெறுமை RTLIL::Design::scratchpad_set_bool(மாறிலி std::string &varname, தருக்க மதிப்பு)
{
	scratchpad[varname] = மதிப்பு ? "true" : "false";
}

வெறுமை RTLIL::Design::scratchpad_set_string(மாறிலி std::string &varname, std::string மதிப்பு)
{
	scratchpad[varname] = std::move(மதிப்பு);
}

முழு RTLIL::Design::scratchpad_get_int(மாறிலி std::string &varname, முழு default_மதிப்பு_2) மாறிலி
{
	auto it = scratchpad.find(varname);
	எனில் (it == scratchpad.end())
		திரும்பு default_மதிப்பு_2;

	மாறிலி std::string &str = it->second;

	எனில் (str == "0" || str == "false")
		திரும்பு 0;

	எனில் (str == "1" || str == "true")
		திரும்பு 1;

	எழுத்து *endptr = nullptr;
	long முழு parsed_மதிப்பு = strtol(str.c_str(), &endptr, 10);
	திரும்பு *endptr ? default_மதிப்பு_2 : parsed_மதிப்பு;
}

தருக்க RTLIL::Design::scratchpad_get_bool(மாறிலி std::string &varname, தருக்க default_மதிப்பு_2) மாறிலி
{
	auto it = scratchpad.find(varname);
	எனில் (it == scratchpad.end())
		திரும்பு default_மதிப்பு_2;

	மாறிலி std::string &str = it->second;

	எனில் (str == "0" || str == "false")
		திரும்பு பொய்;

	எனில் (str == "1" || str == "true")
		திரும்பு உண்மை;

	திரும்பு default_மதிப்பு_2;
}

std::string RTLIL::Design::scratchpad_get_string(மாறிலி std::string &varname, மாறிலி std::string &default_மதிப்பு_2) மாறிலி
{
	auto it = scratchpad.find(varname);
	எனில் (it == scratchpad.end())
		திரும்பு default_மதிப்பு_2;

	திரும்பு it->second;
}

வெறுமை RTLIL::Design::remove(RTLIL::Module *module)
{
	க்கு (auto mon : monitors)
		mon->notify_module_del(module);

	எனில் (yosys_xtrace) {
		log("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	delete module;
}

வெறுமை RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	add(module);
}

வெறுமை RTLIL::Design::sort()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
	க்கு (auto &it : modules_)
		it.second->sort();
}

வெறுமை RTLIL::Design::sort_modules()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
}

வெறுமை check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

வெறுமை RTLIL::Design::check()
{
#ifndef NDEBUG
	log_assert(!selection_அடுக்கு.empty());
	முழு pool_size = 0;
	க்கு (auto &it : modules_)
		pool_size = std::max(pool_size, ThreadPool::work_pool_size(0, it.second->cells_size(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_size);
	க்கு (auto &it : modules_) {
		log_assert(இது == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#endif
}

வெறுமை RTLIL::Design::optimize()
{
	க்கு (auto &it : modules_)
		it.second->optimize();
	க்கு (auto &it : selection_அடுக்கு)
		it.optimize(இது);
	க்கு (auto &it : selection_vars)
		it.second.optimize(இது);
}

வெறுமை RTLIL::Design::clone_into(RTLIL::Design *dst) மாறிலி
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	க்கு (auto it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

தருக்க RTLIL::Design::selected_module(IdString mod_name) மாறிலி
{
	எனில் (!selected_active_module.empty() && mod_name != selected_active_module)
		திரும்பு பொய்;
	திரும்பு selection().selected_module(mod_name);
}

தருக்க RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) மாறிலி
{
	எனில் (!selected_active_module.empty() && mod_name != selected_active_module)
		திரும்பு பொய்;
	திரும்பு selection().selected_whole_module(mod_name);
}

தருக்க RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) மாறிலி
{
	எனில் (!selected_active_module.empty() && mod_name != selected_active_module)
		திரும்பு பொய்;
	திரும்பு selection().selected_member(mod_name, memb_name);
}

தருக்க RTLIL::Design::selected_module(RTLIL::Module *mod) மாறிலி
{
	திரும்பு selected_module(mod->name);
}

தருக்க RTLIL::Design::selected_whole_module(RTLIL::Module *mod) மாறிலி
{
	திரும்பு selected_whole_module(mod->name);
}

வெறுமை RTLIL::Design::தள்ளு_selection(RTLIL::Selection sel)
{
	sel.current_design = இது;
	selection_அடுக்கு.தள்ளு_back(sel);
}

வெறுமை RTLIL::Design::தள்ளு_empty_selection()
{
	தள்ளு_selection(RTLIL::Selection::EmptySelection(இது));
}

வெறுமை RTLIL::Design::தள்ளு_full_selection()
{
	தள்ளு_selection(RTLIL::Selection::FullSelection(இது));
}

வெறுமை RTLIL::Design::தள்ளு_complete_selection()
{
	தள்ளு_selection(RTLIL::Selection::CompleteSelection(இது));
}

வெறுமை RTLIL::Design::எடு_selection()
{
	selection_அடுக்கு.எடு_back();
	// Default to a full_selection if we ran out of stack
	எனில் (selection_அடுக்கு.empty())
		தள்ளு_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(தருக்க only_selected) மாறிலி
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(இது), only_selected, உண்மை, பொய், RTLIL_BACKEND::DumpMode::Readable);
	திரும்பு f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) மாறிலி
{
	தருக்க include_partials = partials == RTLIL::SELECT_ALL;
	தருக்க exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	தருக்க ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> result;
	result.reserve(modules_.size());
	க்கு (auto &it : modules_) {
		எனில் (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			எனில் (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				result.தள்ளு_back(it.second);
			இல்லையெனில்
				தேர்வு (boxes)
				{
				நிலை RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.second);
					நிறுத்து;
				நிலை RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.second);
					நிறுத்து;
				நிலை RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.second);
					நிறுத்து;
				நிலை RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.second);
					நிறுத்து;
				நிலை RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.second);
					நிறுத்து;
				நிலை RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.second);
					நிறுத்து;
				இயல்பு:
					நிறுத்து;
				}
		} இல்லையெனில் எனில் (!include_partials && selected_module(it.first)) {
			தேர்வு(partials)
			{
			நிலை RTLIL::SELECT_WHOLE_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.second);
				நிறுத்து;
			நிலை RTLIL::SELECT_WHOLE_ERR:
				log_error("Unsupported partially selected module %s.\n", it.second);
				நிறுத்து;
			நிலை RTLIL::SELECT_WHOLE_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.second);
				நிறுத்து;
			இயல்பு:
				நிறுத்து;
			}
		}
	}
	திரும்பு result;
}

RTLIL::Module::Module()
{
	நிலையான unsigned முழு hashidx_மொத்தம் = 123456789;
	hashidx_மொத்தம் = mkhash_xorshift(hashidx_மொத்தம்);
	hashidx_ = hashidx_மொத்தம்;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->செருகு(std::pair<unsigned முழு, RTLIL::Module*>(hashidx_, இது));
#endif
}

RTLIL::Module::~Module()
{
	க்கு (auto &pr : wires_)
		delete pr.second;
	க்கு (auto &pr : memories)
		delete pr.second;
	க்கு (auto &pr : cells_)
		delete pr.second;
	க்கு (auto &pr : processes)
		delete pr.second;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
நிலையான std::map<unsigned முழு, RTLIL::Module*> all_modules;
std::map<unsigned முழு, RTLIL::Module*> *RTLIL::Module::get_all_modules(வெறுமை)
{
	திரும்பு &all_modules;
}
#endif

வெறுமை RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	க்கு (auto it = wires_.begin(); it != wires_.end(); ++it)
		எனில் (!it->second->port_உள்ளீடு && !it->second->port_output)
			delwires.செருகு(it->second);

	க்கு (auto it = memories.begin(); it != memories.end(); ++it)
		delete it->second;
	memories.clear();

	க்கு (auto it = cells_.begin(); it != cells_.end(); ++it)
		delete it->second;
	cells_.clear();

	க்கு (auto it = processes.begin(); it != processes.end(); ++it)
		delete it->second;
	processes.clear();

	connections_.clear();

	remove(delwires);
	set_bool_attribute(ID::blackbox);
}

வெறுமை RTLIL::Module::expand_interfaces(RTLIL::Design *, மாறிலி dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

தருக்க RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	திரும்பு பொய்;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, மாறிலி dict<RTLIL::IdString, RTLIL::Const> &, தருக்க mayfail)
{
	எனில் (mayfail)
		திரும்பு IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, மாறிலி dict<RTLIL::IdString, RTLIL::Const> &, மாறிலி dict<RTLIL::IdString, RTLIL::Module*> &, மாறிலி dict<RTLIL::IdString, RTLIL::IdString> &, தருக்க mayfail)
{
	எனில் (mayfail)
		திரும்பு IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

size_t RTLIL::Module::மொத்தம்_id(RTLIL::IdString id)
{
	திரும்பு wires_.மொத்தம்(id) + cells_.மொத்தம்(id) + memories.மொத்தம்(id) + processes.மொத்தம்(id);
}

#ifndef NDEBUG
நாமாகாஶ {
	கட்டமைப்பு InternalCellChecker
	{
		மாறிலி RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		InternalCellChecker(மாறிலி RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		வெறுமை error(முழு linenr, std::optional<std::string> error_செய்தி = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			எனில் (error_செய்தி) {
				log_error("%s in internal cell %s\n", *error_செய்தி, cell_loc);
			} இல்லையெனில் {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		முழு param(RTLIL::IdString name)
		{
			auto it = cell->parameters.find(name);
			எனில் (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.செருகு(name);
			திரும்பு it->second.as_int();
		}

		முழு param_bool(RTLIL::IdString name)
		{
			முழு v = param(name);
			எனில் (GetSize(cell->parameters.at(name)) > 32)
				error(__LINE__);
			எனில் (v != 0 && v != 1)
				error(__LINE__);
			திரும்பு v;
		}

		முழு param_bool(RTLIL::IdString name, தருக்க expected)
		{
			முழு v = param_bool(name);
			எனில் (v != expected)
				error(__LINE__);
			திரும்பு v;
		}

		வெறுமை param_bits(RTLIL::IdString name, முழு width)
		{
			param(name);
			எனில் (GetSize(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			திரும்பு cell->parameters.at(name).decode_string();
		}

		வெறுமை port(RTLIL::IdString name, முழு width)
		{
			auto it = cell->connections_.find(name);
			எனில் (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			எனில் (GetSize(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.செருகு(name);
		}

		வெறுமை check_expected(தருக்க check_matched_sign = பொய்)
		{
			க்கு (auto &para : cell->parameters)
				எனில் (expected_params.மொத்தம்(para.first) == 0)
					error(__LINE__);
			க்கு (auto &conn : cell->connections())
				எனில் (expected_ports.மொத்தம்(conn.first) == 0)
					error(__LINE__);

			எனில் (check_matched_sign) {
				log_assert(expected_params.மொத்தம்(ID::A_SIGNED) != 0 && expected_params.மொத்தம்(ID::B_SIGNED) != 0);
				தருக்க a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				தருக்க b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				எனில் (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		வெறுமை check()
		{
			எனில் (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				திரும்பு;

			எனில் (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(உண்மை);
				திரும்பு;
			}

			எனில் (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/பொய்);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/பொய்);
				திரும்பு;
			}

			எனில் (cell->type.in(ID($shift), ID($shiftx))) {
				எனில் (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/பொய்);
				} இல்லையெனில் {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/பொய்);
				திரும்பு;
			}

			எனில் (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(உண்மை);
				திரும்பு;
			}

			எனில் (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($pow));
				திரும்பு;
			}

			எனில் (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::CI, 1);
				port(ID::BI, 1);
				port(ID::X, param(ID::Y_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				port(ID::CO, param(ID::Y_WIDTH));
				check_expected(உண்மை);
				திரும்பு;
			}

			எனில் (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				திரும்பு;
			}

			எனில் (cell->type == ID($macc_v2)) {
				எனில் (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				எனில் (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				எனில் (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				மாறிலி Const &a_width = cell->getParam(ID::A_WIDTHS);
				மாறிலி Const &b_width = cell->getParam(ID::B_WIDTHS);
				மாறிலி Const &c_width = cell->getParam(ID::C_WIDTHS);
				முழு a_width_sum = 0, b_width_sum = 0, c_width_sum = 0;
				க்கு (முழு i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_sum += a_width.extract(16 * i, 16).as_int(பொய்);
					b_width_sum += b_width.extract(16 * i, 16).as_int(பொய்);
				}
				க்கு (முழு i = 0; i < param(ID::NADDENDS); i++) {
					c_width_sum += c_width.extract(16 * i, 16).as_int(பொய்);
				}
				port(ID::A, a_width_sum);
				port(ID::B, b_width_sum);
				port(ID::C, c_width_sum);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/பொய்);
				திரும்பு;
			}

			எனில் (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				எனில் (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($dffsre)) {
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
				திரும்பு;
			}

			எனில் (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_மதிப்பு, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_மதிப்பு, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type.in(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_மதிப்பு, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_மதிப்பு, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($aldffe)) {
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
				திரும்பு;
			}

			எனில் (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_மதிப்பு, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_BITS);
				param(ID::STATE_இலக்கம்);
				param(ID::STATE_இலக்கம்_LOG2);
				param(ID::STATE_RST);
				param_bits(ID::STATE_TABLE, param(ID::STATE_BITS) * param(ID::STATE_இலக்கம்));
				param(ID::TRANS_இலக்கம்);
				param_bits(ID::TRANS_TABLE, param(ID::TRANS_இலக்கம்) * (2*param(ID::STATE_இலக்கம்_LOG2) + param(ID::CTRL_IN_WIDTH) + param(ID::CTRL_OUT_WIDTH)));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::CTRL_IN, param(ID::CTRL_IN_WIDTH));
				port(ID::CTRL_OUT, param(ID::CTRL_OUT_WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_MASK);
				param(ID::COLLISION_X_MASK);
				param_bool(ID::CE_OVER_SRST);
				param_bits(ID::ARST_மதிப்பு, param(ID::WIDTH));
				param_bits(ID::SRST_மதிப்பு, param(ID::WIDTH));
				param_bits(ID::INIT_மதிப்பு, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::SRST, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($memwr_v2)) {
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
				திரும்பு;
			}

			எனில் (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::சொற்கள்));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::சொற்கள்));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($mem)) {
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
				திரும்பு;
			}

			எனில் (cell->type == ID($mem_v2)) {
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
				param_bits(ID::RD_ARST_மதிப்பு, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_SRST_மதிப்பு, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_INIT_மதிப்பு, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
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
				திரும்பு;
			}

			எனில் (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type.in(ID($ப்ரதிஜ்ஞா), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type.in(ID($specify2), ID($specify3))) {
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
				எனில் (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
				}
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($specrule)) {
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
				திரும்பு;
			}

			எனில் (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				திரும்பு;
			}

			எனில் (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				எனில் (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
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
				திரும்பு;
			}

			எனில் (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				எனில் (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				திரும்பு;
			}

			எனில் (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); திரும்பு; }

			எனில் (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); திரும்பு; }

			எனில் (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); திரும்பு; }
			எனில் (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); திரும்பு; }

			எனில் (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  திரும்பு; }

			எனில் (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); திரும்பு; }

			எனில் (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}
			எனில் (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}
			எனில் (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}
			எனில் (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}
			எனில் (cell->type.in(ID($உள்ளீடு_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				திரும்பு;
			}
			எனில் (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				check_expected();
				திரும்பு;
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

வெறுமை RTLIL::Module::sort()
{
	wires_.sort(sort_by_id_str(design->twines()));
	cells_.sort(sort_by_id_str(design->twines()));
	parameter_default_மதிப்புகள்.sort(sort_by_id_str(design->twines()));
	memories.sort(sort_by_id_str(design->twines()));
	processes.sort(sort_by_id_str(design->twines()));
	க்கு (auto &it : cells_)
		it.second->sort();
	க்கு (auto &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	க்கு (auto &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#ifndef NDEBUG
நிலையான வெறுமை check_id_சாவிகள்(மாறிலி dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	க்கு (auto &it : keyed)
		log_assert(!it.first.empty());
}
#endif

வெறுமை check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ifndef NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->cells_size(), 1000));
	மாறிலி RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	க்கு (auto &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->width >= 0);
		log_assert(it.second->size >= 0);
		check_id_சாவிகள்(it.second->attributes);
		memory_strings.செருகு(it.second->name.str());
	}

	std::vector<MonotonicFlag> ports_declared(GetSize(module->ports));
	ShardedVector<std::string> memids(subpool);
	subpool.run([const_module, &ports_declared, &memory_strings, &memids](மாறிலி ParallelDispatchThreadPool::RunCtx &ctx) {
		க்கு (முழு i : ctx.item_range(const_module->cells_size())) {
			auto it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			க்கு (auto &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_சாவிகள்(it.second->attributes);
			check_id_சாவிகள்(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			எனில் (it.second->has_memid()) {
				log_assert(memory_strings.மொத்தம்(it.second->parameters.at(ID::MEMID).decode_string()));
			} இல்லையெனில் எனில் (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.மொத்தம்(memid));
				memids.செருகு(ctx, std::move(memid));
			}
			auto cell_mod = const_module->design->module(it.second->name);
			எனில் (cell_mod != nullptr) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#ifdef __GNUC__
				#pragma GCC diagnostic தள்ளு
				#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				#endif
				log_assert(!it.second->get_blackbox_attribute());
				#ifdef __GNUC__
				#pragma GCC diagnostic எடு
				#endif
			}
		}

		க்கு (முழு i : ctx.item_range(const_module->wires_size())) {
			auto it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_சாவிகள்(it.second->attributes);
			எனில் (it.second->port_id) {
				log_assert(GetSize(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_உள்ளீடு || it.second->port_output);
				log_assert(it.second->port_id <= GetSize(ports_declared));
				தருக்க previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == பொய்);
			} இல்லையெனில்
				log_assert(!it.second->port_உள்ளீடு && !it.second->port_output);
		}
	});
	க்கு (மாறிலி MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == உண்மை);
	pool<std::string> memids_pool;
	க்கு (std::string &memid : memids)
		log_assert(memids_pool.செருகு(memid).second);

	க்கு (auto &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->வேர்_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->வேர்_case};
		க்கு (size_t i = 0; i < all_cases.size(); i++) {
			க்கு (auto &switch_it : all_cases[i]->switches) {
				க்கு (auto &case_it : switch_it->cases) {
					க்கு (auto &compare_it : case_it->compare) {
						log_assert(switch_it->சமிக்ஞை.size() == compare_it.size());
					}
					all_cases.தள்ளு_back(case_it);
				}
			}
		}
		க்கு (auto &sync_it : it.second->syncs) {
			தேர்வு (sync_it->type) {
				நிலை RTLIL::SyncType::ST0:
				நிலை RTLIL::SyncType::ST1:
				நிலை RTLIL::SyncType::STp:
				நிலை RTLIL::SyncType::STn:
				நிலை RTLIL::SyncType::STe:
					log_assert(!sync_it->சமிக்ஞை.empty());
					நிறுத்து;
				நிலை RTLIL::SyncType::STa:
				நிலை RTLIL::SyncType::STg:
				நிலை RTLIL::SyncType::STi:
					log_assert(sync_it->சமிக்ஞை.empty());
					நிறுத்து;
			}
		}
	}

	க்கு (auto &it : module->connections_) {
		log_assert(it.first.size() == it.second.size());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_சாவிகள்(module->attributes);
#endif
}

வெறுமை RTLIL::Module::check()
{
	முழு pool_size = ThreadPool::work_pool_size(0, cells_size(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_size);
	check_module(இது, thread_pool);
}

வெறுமை RTLIL::Module::optimize()
{
}

வெறுமை RTLIL::Module::cloneInto(RTLIL::Module *new_mod) மாறிலி
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	க்கு (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_மதிப்புகள்.clear();
	க்கு (auto &it : parameter_default_மதிப்புகள்)
		new_mod->parameter_default_மதிப்புகள்[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	க்கு (auto &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	க்கு (auto it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	க்கு (auto it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	க்கு (auto it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	க்கு (auto it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addProcess(dst_id, it->second);
	}

	கட்டமைப்பு RewriteSigSpecWorker
	{
		மாறிலி dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		மாறிலி RTLIL::Module *new_mod;
		வெறுமை operator()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([இது](RTLIL::Wire *&wire) {
				auto it = wire_map.find(wire);
				எனில் (it != wire_map.end())
					wire = it->second;
				இல்லையெனில்
					log_assert(wire->module == new_mod);
			});
		}
	};

	RewriteSigSpecWorker rewriteSigSpecWorker{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewriteSigSpecWorker);
	new_mod->fixup_ports();
}

RTLIL::Module *RTLIL::Module::clone() மாறிலி
{
	RTLIL::Module *new_mod = புதிய RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	திரும்பு new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) மாறிலி
{
	RTLIL::Module *new_mod = புதிய RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->add(new_mod);
	திரும்பு new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) மாறிலி
{
	RTLIL::Module *new_mod = புதிய RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	திரும்பு new_mod;
}

தருக்க RTLIL::Module::has_memories() மாறிலி
{
	திரும்பு !memories.empty();
}

தருக்க RTLIL::Module::has_processes() மாறிலி
{
	திரும்பு !processes.empty();
}

தருக்க RTLIL::Module::has_memories_warn() மாறிலி
{
	எனில் (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", இது);
	திரும்பு !memories.empty();
}

தருக்க RTLIL::Module::has_processes_warn() மாறிலி
{
	எனில் (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", இது);
	திரும்பு !processes.empty();
}

தருக்க RTLIL::Module::is_selected() மாறிலி
{
	திரும்பு design->selected_module(இது->name);
}

தருக்க RTLIL::Module::is_selected_whole() மாறிலி
{
	திரும்பு design->selected_whole_module(இது->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() மாறிலி
{
	std::vector<RTLIL::Wire*> result;
	result.reserve(wires_.size());
	க்கு (auto &it : wires_)
		எனில் (design->selected(இது, it.second))
			result.தள்ளு_back(it.second);
	திரும்பு result;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() மாறிலி
{
	std::vector<RTLIL::Cell*> result;
	result.reserve(cells_.size());
	க்கு (auto &it : cells_)
		எனில் (design->selected(இது, it.second))
			result.தள்ளு_back(it.second);
	திரும்பு result;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() மாறிலி
{
	std::vector<RTLIL::Memory*> result;
	result.reserve(memories.size());
	க்கு (auto &it : memories)
		எனில் (design->selected(இது, it.second))
			result.தள்ளு_back(it.second);
	திரும்பு result;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() மாறிலி
{
	std::vector<RTLIL::Process*> result;
	result.reserve(processes.size());
	க்கு (auto &it : processes)
		எனில் (design->selected(இது, it.second))
			result.தள்ளு_back(it.second);
	திரும்பு result;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() மாறிலி
{
	std::vector<RTLIL::SelectedMember> result;
	க்கு (auto cell : selected_cells())
		result.தள்ளு_back({cell, cell->name});
	க்கு (auto memory : selected_memories())
		result.தள்ளு_back({memory, memory->name});
	க்கு (auto wire : selected_wires())
		result.தள்ளு_back({wire, wire->name});
	க்கு (auto process : selected_processes())
		result.தள்ளு_back({process, process->name});
	திரும்பு result;
}

வெறுமை RTLIL::Module::add(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(மொத்தம்_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = இது;
}

வெறுமை RTLIL::Module::add(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(மொத்தம்_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = இது;
}

வெறுமை RTLIL::Module::add(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(மொத்தம்_id(process->name) == 0);
	processes[process->name] = process;
	process->module = இது;
}

வெறுமை RTLIL::Module::remove(மாறிலி pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	எனில் (wires.empty())
		திரும்பு;

	கட்டமைப்பு DeleteWireWorker
	{
		RTLIL::Module *module;
		மாறிலி pool<RTLIL::Wire*> *wires_p;

		வெறுமை operator()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([இது](RTLIL::Wire *&wire) {
				எனில் (wires_p->மொத்தம்(wire))
					wire = module->addWire(stringf("$delete_wire$%d", (முழு)autoidx++), wire->width);
			});
		}

		வெறுமை operator()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	DeleteWireWorker delete_wire_worker;
	delete_wire_worker.module = இது;
	delete_wire_worker.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_worker);

	எனில் (design->flagBufferedNormalized) {
		க்கு (auto wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_index.erase(wire);
		}
	}

	க்கு (auto &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.மொத்தம்(id) != 0);
		wires_.erase(id);
		delete it;
	}
}

வெறுமை RTLIL::Module::remove(RTLIL::Cell *cell)
{
	போது (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.மொத்தம்(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	எனில் (design && design->flagBufferedNormalized && buf_norm_cell_queue.மொத்தம்(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.செருகு(cell);
	} இல்லையெனில் {
		delete cell;
	}
}

வெறுமை RTLIL::Module::remove(RTLIL::Memory *memory)
{
	log_assert(memories.மொத்தம்(memory->name) != 0);
	memories.erase(memory->name);
	delete memory;
}

வெறுமை RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.மொத்தம்(process->name) != 0);
	processes.erase(process->name);
	delete process;
}

வெறுமை RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	add(wire);
}

வெறுமை RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	add(cell);
}

வெறுமை RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	எனில் (old_id != IdString::Null && wires_.மொத்தம்(old_id))
		rename(wires_.at(old_id), new_name);
	இல்லையெனில் எனில் (old_id != IdString::Null && cells_.மொத்தம்(old_id))
		rename(cells_.at(old_id), new_name);
	இல்லையெனில்
		log_abort();
}

வெறுமை RTLIL::Module::swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2)
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

வெறுமை RTLIL::Module::swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	முழு index = 0;
	திரும்பு uniquify(name, index);
}

IdString RTLIL::Module::uniquify(IdString name, முழு &index)
{
	எனில் (index == 0) {
		எனில் (மொத்தம்_id(name) == 0)
			திரும்பு name;
		index++;
	}

	போது (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", index)});
		எனில் (மொத்தம்_id(new_name) == 0)
			திரும்பு new_name;
		index++;
	}
}

நிலையான தருக்க fixup_ports_compare(மாறிலி RTLIL::Wire *a, மாறிலி RTLIL::Wire *b)
{
	எனில் (a->port_id && !b->port_id)
		திரும்பு உண்மை;
	எனில் (!a->port_id && b->port_id)
		திரும்பு பொய்;

	எனில் (a->port_id == b->port_id)
		திரும்பு a->name < b->name;
	திரும்பு a->port_id < b->port_id;
}

வெறுமை RTLIL::Module::connect(மாறிலி RTLIL::SigSig &conn)
{
	க்கு (auto mon : monitors)
		mon->notify_connect(இது, conn);

	எனில் (design)
		க்கு (auto mon : design->monitors)
			mon->notify_connect(இது, conn);

	// ignore all attempts to assign constants to other constants
	எனில் (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		க்கு (முழு i = 0; i < GetSize(conn.first); i++)
			எனில் (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		எனில் (GetSize(new_conn.first))
			connect(new_conn);
		திரும்பு;
	}

	எனில் (yosys_xtrace) {
		log("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", இது, log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(GetSize(conn.first) == GetSize(conn.second));
	connections_.தள்ளு_back(conn);
}

வெறுமை RTLIL::Module::connect(மாறிலி RTLIL::SigSpec &lhs, மாறிலி RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

வெறுமை RTLIL::Module::new_connections(மாறிலி std::vector<RTLIL::SigSig> &new_conn)
{
	க்கு (auto mon : monitors)
		mon->notify_connect(இது, new_conn);

	எனில் (design)
		க்கு (auto mon : design->monitors)
			mon->notify_connect(இது, new_conn);

	எனில் (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", இது);
		க்கு (auto &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

மாறிலி std::vector<RTLIL::SigSig> &RTLIL::Module::connections() மாறிலி
{
	திரும்பு connections_;
}

வெறுமை RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	க்கு (auto &w : wires_)
		எனில் (w.second->port_உள்ளீடு || w.second->port_output)
			all_ports.தள்ளு_back(w.second);
		இல்லையெனில்
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	எனில் (design && design->flagBufferedNormalized) {
		க்கு (auto &w : wires_)
			எனில் (w.second->driverCell_ && w.second->driverCell_->type == ID($உள்ளீடு_port))
				buf_norm_wire_queue.செருகு(w.second);

		buf_norm_wire_queue.செருகு(all_ports.begin(), all_ports.end());
	}

	ports.clear();
	க்கு (size_t i = 0; i < all_ports.size(); i++) {
		ports.தள்ளு_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
	}
}

வெறுமை RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		மாறிலி dict<IdString, RTLIL::Const> &src,
		மாறிலி RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	எனில் (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		திரும்பு;
	}
	dst.clear();
	க்கு (முழு i = GetSize(src) - 1; i >= 0; i--) {
		auto &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::addWire(RTLIL::IdString name, முழு width)
{
	log_assert(design);
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	RTLIL::Wire *wire = புதிய RTLIL::Wire;
	wire->width = width;
	wire->name = name;
	add(wire);
	திரும்பு wire;
}

RTLIL::Wire *RTLIL::Module::addWire(IdString name, மாறிலி RTLIL::Wire *other)
{
	RTLIL::Wire *wire = addWire(name);
	wire->width = other->width;
	wire->start_offset = other->start_offset;
	wire->port_id = other->port_id;
	wire->port_உள்ளீடு = other->port_உள்ளீடு;
	wire->port_output = other->port_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		மாறிலி RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
		RTLIL::copy_attr_dict(wire->attributes, other->attributes, src_design, இது->design);
	}
	திரும்பு wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = புதிய RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	திரும்பு cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, மாறிலி RTLIL::Cell *other)
{
	மாறிலி RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	தருக்க cross_pool = src_design && இது->design && src_design != இது->design;

	IdString type = other->type.ref();
	எனில் (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, இது->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, இது->design);

	எனில் (!cross_pool)
		cell->connections_ = other->connections_;
	இல்லையெனில்
		க்கு (முழு i = GetSize(other->connections_) - 1; i >= 0; i--) {
			auto &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	திரும்பு cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = புதிய RTLIL::Memory;
	mem->module = இது;
	mem->name = name;
	memories[name] = mem;
	திரும்பு mem;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name, மாறிலி RTLIL::Memory *other)
{
	log_assert(design);
	RTLIL::Memory *mem = புதிய RTLIL::Memory;
	mem->module = இது;
	mem->name = name;
	mem->width = other->width;
	mem->start_offset = other->start_offset;
	mem->size = other->size;
	RTLIL::copy_attr_dict(mem->attributes, other->attributes,
			other->module ? other->module->design : nullptr, design);
	memories[name] = mem;
	திரும்பு mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = புதிய RTLIL::Process;
	proc->name = name;
	add(proc);
	திரும்பு proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, மாறிலி RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	மாறிலி RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	எனில் (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		க்கு (auto *ஒத்திசை : proc->syncs)
			க்கு (auto &mwa : ஒத்திசை->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	திரும்பு proc;
}

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, தருக்க is_signed, மாறிலி std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		திரும்பு cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, தருக்க is_signed, மாறிலி std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		திரும்பு sig_y;                                       \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, தருக்க /* is_signed */, மாறிலி std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::WIDTH] = sig_a.size();         \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		திரும்பு cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, தருக்க is_signed, மாறிலி std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		திரும்பு sig_y;                                       \
	}
DEF_METHOD(Buf,     sig_a.size(), ID($buf))
DEF_METHOD(Barrier, sig_a.size(), ID($barrier))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, தருக்க is_signed, மாறிலி std::string &src) { \
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
		திரும்பு cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, தருக்க is_signed, மாறிலி std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		திரும்பு sig_y;                                            \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, தருக்க is_signed, மாறிலி std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = பொய்;             \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		திரும்பு cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, தருக்க is_signed, மாறிலி std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		திரும்பு sig_y;                                            \
	}
DEF_METHOD(Shl,      sig_a.size(), ID($shl))
DEF_METHOD(Shr,      sig_a.size(), ID($shr))
DEF_METHOD(Sshl,     sig_a.size(), ID($sshl))
DEF_METHOD(Sshr,     sig_a.size(), ID($sshr))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, தருக்க is_signed, மாறிலி std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = பொய்;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		திரும்பு cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, தருக்க is_signed, மாறிலி std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		திரும்பு sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.size(), ID($shiftx))
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		எனில் (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.size();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		திரும்பு cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		திரும்பு sig_y;                                             \
	}
DEF_METHOD(Mux,      ID($mux),        0)
DEF_METHOD(Bwmux,    ID($bwmux),      0)
DEF_METHOD(Pmux,     ID($pmux),       1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = _demux ? sig_a.size() : sig_y.size(); \
		cell->parameters[ID::S_WIDTH] = sig_s.size();             \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		திரும்பு cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.size() << sig_s.size() : sig_a.size() >> sig_s.size()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		திரும்பு sig_y;                                             \
	}
DEF_METHOD(Bmux,     ID($bmux),       0)
DEF_METHOD(Demux,    ID($demux),      1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		திரும்பு cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		திரும்பு sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#undef DEF_METHOD

#define DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig1, மாறிலி RTLIL::SigBit &sig2, மாறிலி std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		திரும்பு cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig1, மாறிலி std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		திரும்பு sig2;                                      \
	}
#define DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig1, மாறிலி RTLIL::SigBit &sig2, மாறிலி RTLIL::SigBit &sig3, மாறிலி std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		திரும்பு cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig1, மாறிலி RTLIL::SigBit &sig2, மாறிலி std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		திரும்பு sig3;                                      \
	}
#define DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig1, மாறிலி RTLIL::SigBit &sig2, மாறிலி RTLIL::SigBit &sig3, மாறிலி RTLIL::SigBit &sig4, மாறிலி std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		திரும்பு cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig1, மாறிலி RTLIL::SigBit &sig2, மாறிலி RTLIL::SigBit &sig3, மாறிலி std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		திரும்பு sig4;                                      \
	}
#define DEF_METHOD_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig1, மாறிலி RTLIL::SigBit &sig2, மாறிலி RTLIL::SigBit &sig3, மாறிலி RTLIL::SigBit &sig4, மாறிலி RTLIL::SigBit &sig5, மாறிலி std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->setPort("\\" #_P5, sig5);                   \
		cell->set_src_attribute(src);                     \
		திரும்பு cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, மாறிலி RTLIL::SigBit &sig1, மாறிலி RTLIL::SigBit &sig2, மாறிலி RTLIL::SigBit &sig3, மாறிலி RTLIL::SigBit &sig4, மாறிலி std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		திரும்பு sig5;                                           \
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

RTLIL::Cell* RTLIL::Module::addPow(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, தருக்க a_signed, தருக்க b_signed, மாறிலி std::string &src)
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
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addFa(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_c, மாறிலி RTLIL::SigSpec &sig_x, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fa));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::C, sig_c);
	cell->setPort(ID::X, sig_x);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addSlice(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, RTLIL::Const offset, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($slice));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::Y_WIDTH] = sig_y.size();
	cell->parameters[ID::OFFSET] = offset;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addConcat(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($concat));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::B_WIDTH] = sig_b.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_y, RTLIL::Const lut, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ப்ரதிஜ்ஞா));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_b, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addSr(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr, மாறிலி RTLIL::SigSpec &sig_q, தருக்க set_polarity, தருக்க clr_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sr));
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addDff(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, தருக்க clk_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addDffe(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, தருக்க clk_polarity, தருக்க en_polarity, மாறிலி std::string &src)
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
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addDffsr(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, மாறிலி RTLIL::SigSpec &sig_q, தருக்க clk_polarity, தருக்க set_polarity, தருக்க clr_polarity, மாறிலி std::string &src)
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
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addDffsre(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, மாறிலி RTLIL::SigSpec &sig_q, தருக்க clk_polarity, தருக்க en_polarity, தருக்க set_polarity, தருக்க clr_polarity, மாறிலி std::string &src)
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
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addAdff(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_arst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_மதிப்பு, தருக்க clk_polarity, தருக்க arst_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_மதிப்பு] = arst_மதிப்பு;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addAdffe(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_arst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_மதிப்பு, தருக்க clk_polarity, தருக்க en_polarity, தருக்க arst_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_மதிப்பு] = arst_மதிப்பு;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addAldff(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_aload, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		மாறிலி RTLIL::SigSpec &sig_ad, தருக்க clk_polarity, தருக்க aload_polarity, மாறிலி std::string &src)
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
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addAldffe(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_aload, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		மாறிலி RTLIL::SigSpec &sig_ad, தருக்க clk_polarity, தருக்க en_polarity, தருக்க aload_polarity, மாறிலி std::string &src)
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
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addSdff(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_srst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_மதிப்பு, தருக்க clk_polarity, தருக்க srst_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_மதிப்பு] = srst_மதிப்பு;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addSdffe(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_srst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_மதிப்பு, தருக்க clk_polarity, தருக்க en_polarity, தருக்க srst_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_மதிப்பு] = srst_மதிப்பு;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addSdffce(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_srst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_மதிப்பு, தருக்க clk_polarity, தருக்க en_polarity, தருக்க srst_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_மதிப்பு] = srst_மதிப்பு;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addDlatch(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, தருக்க en_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatch(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_arst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_மதிப்பு, தருக்க en_polarity, தருக்க arst_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_மதிப்பு] = arst_மதிப்பு;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsr(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, மாறிலி RTLIL::SigSpec &sig_q, தருக்க en_polarity, தருக்க set_polarity, தருக்க clr_polarity, மாறிலி std::string &src)
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
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr,
		மாறிலி RTLIL::SigSpec &sig_q, தருக்க set_polarity, தருக்க clr_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, தருக்க clk_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, தருக்க clk_polarity, தருக்க en_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addDffsrGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, மாறிலி RTLIL::SigSpec &sig_q, தருக்க clk_polarity, தருக்க set_polarity, தருக்க clr_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addDffsreGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, மாறிலி RTLIL::SigSpec &sig_q, தருக்க clk_polarity, தருக்க en_polarity, தருக்க set_polarity, தருக்க clr_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addAdffGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_arst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		தருக்க arst_மதிப்பு, தருக்க clk_polarity, தருக்க arst_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_மதிப்பு ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addAdffeGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_arst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		தருக்க arst_மதிப்பு, தருக்க clk_polarity, தருக்க en_polarity, தருக்க arst_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_மதிப்பு ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addAldffGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_aload, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		மாறிலி RTLIL::SigSpec &sig_ad, தருக்க clk_polarity, தருக்க aload_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addAldffeGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_aload, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		மாறிலி RTLIL::SigSpec &sig_ad, தருக்க clk_polarity, தருக்க en_polarity, தருக்க aload_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addSdffGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_srst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		தருக்க srst_மதிப்பு, தருக்க clk_polarity, தருக்க srst_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_மதிப்பு ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addSdffeGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_srst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		தருக்க srst_மதிப்பு, தருக்க clk_polarity, தருக்க en_polarity, தருக்க srst_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_மதிப்பு ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addSdffceGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_clk, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_srst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		தருக்க srst_மதிப்பு, தருக்க clk_polarity, தருக்க en_polarity, தருக்க srst_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_மதிப்பு ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, தருக்க en_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatchGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_arst, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q,
		தருக்க arst_மதிப்பு, தருக்க en_polarity, தருக்க arst_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_மதிப்பு ? '1' : '0'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsrGate(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_en, மாறிலி RTLIL::SigSpec &sig_set, மாறிலி RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, மாறிலி RTLIL::SigSpec &sig_q, தருக்க en_polarity, தருக்க set_polarity, தருக்க clr_polarity, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_d, மாறிலி RTLIL::SigSpec &sig_q, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, முழு width, மாறிலி std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	திரும்பு sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, முழு width, மாறிலி std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	திரும்பு sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, முழு width, மாறிலி std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	திரும்பு sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, முழு width, மாறிலி std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	திரும்பு sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, மாறிலி std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	திரும்பு sig;
}

RTLIL::SigSpec RTLIL::Module::SetTag(RTLIL::IdString name, மாறிலி std::string &tag, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி RTLIL::SigSpec &sig_c, மாறிலி std::string &src)
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
	திரும்பு sig;
}

RTLIL::Cell* RTLIL::Module::addSetTag(RTLIL::IdString name, மாறிலி std::string &tag, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி RTLIL::SigSpec &sig_c, மாறிலி RTLIL::SigSpec &sig_y, மாறிலி std::string &src)
{
	Cell *cell = addCell(name, ID($set_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::SigSpec RTLIL::Module::GetTag(RTLIL::IdString name, மாறிலி std::string &tag, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($get_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	திரும்பு sig;
}

RTLIL::Cell* RTLIL::Module::addOverwriteTag(RTLIL::IdString name, மாறிலி std::string &tag, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி RTLIL::SigSpec &sig_s, மாறிலி RTLIL::SigSpec &sig_c, மாறிலி std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($overwrite_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->set_src_attribute(src);
	திரும்பு cell;
}

RTLIL::SigSpec RTLIL::Module::OriginalTag(RTLIL::IdString name, மாறிலி std::string &tag, மாறிலி RTLIL::SigSpec &sig_a, மாறிலி std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($original_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	திரும்பு sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, மாறிலி RTLIL::SigSpec &sig_e, மாறிலி std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.size());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.size();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	திரும்பு sig;
}

std::string RTLIL::Module::to_rtlil_str() மாறிலி
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(இது), design, பொய், உண்மை, பொய், RTLIL_BACKEND::DumpMode::Readable);
	திரும்பு f.str();
}

RTLIL::Wire::Wire()
{
	நிலையான unsigned முழு hashidx_மொத்தம் = 123456789;
	hashidx_மொத்தம் = mkhash_xorshift(hashidx_மொத்தம்);
	hashidx_ = hashidx_மொத்தம்;

	module = nullptr;
	width = 1;
	start_offset = 0;
	port_id = 0;
	port_உள்ளீடு = பொய்;
	port_output = பொய்;
	upto = பொய்;
	is_signed = பொய்;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->செருகு(std::pair<unsigned முழு, RTLIL::Wire*>(hashidx_, இது));
#endif
}

RTLIL::Wire::~Wire()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#endif
}

std::string RTLIL::Wire::to_rtlil_str() மாறிலி
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", இது, design(), RTLIL_BACKEND::DumpMode::Readable);
	திரும்பு f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
நிலையான std::map<unsigned முழு, RTLIL::Wire*> all_wires;
std::map<unsigned முழு, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(வெறுமை)
{
	திரும்பு &all_wires;
}
#endif

RTLIL::Memory::Memory()
{
	நிலையான unsigned முழு hashidx_மொத்தம் = 123456789;
	hashidx_மொத்தம் = mkhash_xorshift(hashidx_மொத்தம்);
	hashidx_ = hashidx_மொத்தம்;

	width = 1;
	start_offset = 0;
	size = 0;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->செருகு(std::pair<unsigned முழு, RTLIL::Memory*>(hashidx_, இது));
#endif
}

std::string RTLIL::Memory::to_rtlil_str() மாறிலி
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", இது, design(), RTLIL_BACKEND::DumpMode::Readable);
	திரும்பு f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	நிலையான unsigned முழு hashidx_மொத்தம் = 123456789;
	hashidx_மொத்தம் = mkhash_xorshift(hashidx_மொத்தம்);
	hashidx_ = hashidx_மொத்தம்;
}

std::string RTLIL::Process::to_rtlil_str() மாறிலி
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", இது, design(), RTLIL_BACKEND::DumpMode::Readable);
	திரும்பு f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	நிலையான unsigned முழு hashidx_மொத்தம் = 123456789;
	hashidx_மொத்தம் = mkhash_xorshift(hashidx_மொத்தம்);
	hashidx_ = hashidx_மொத்தம்;

	// log("#memtrace# %p\n", this);
	memhasher();

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->செருகு(std::pair<unsigned முழு, RTLIL::Cell*>(hashidx_, இது));
#endif
}

RTLIL::Cell::~Cell()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#endif
}

std::string RTLIL::Cell::to_rtlil_str() மாறிலி
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", இது, design(), RTLIL_BACKEND::DumpMode::Readable);
	திரும்பு f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
நிலையான std::map<unsigned முழு, RTLIL::Cell*> all_cells;
std::map<unsigned முழு, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(வெறுமை)
{
	திரும்பு &all_cells;
}
#endif

தருக்க RTLIL::Cell::hasPort(RTLIL::IdString portname) மாறிலி
{
	திரும்பு connections_.மொத்தம்(portname) != 0;
}

// bufnorm

மாறிலி RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) மாறிலி
{
	திரும்பு connections_.at(portname);
}

மாறிலி dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() மாறிலி
{
	திரும்பு connections_;
}

தருக்க RTLIL::Cell::known() மாறிலி
{
	எனில் (yosys_celltypes.cell_known(type))
		திரும்பு உண்மை;
	எனில் (module && module->design && module->design->module(type))
		திரும்பு உண்மை;
	திரும்பு பொய்;
}

தருக்க RTLIL::Cell::உள்ளீடு(IdString portname) மாறிலி
{
	எனில் (yosys_celltypes.cell_known(type))
		திரும்பு yosys_celltypes.cell_உள்ளீடு(type.ref(), portname);
	எனில் (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		திரும்பு w && w->port_உள்ளீடு;
	}
	திரும்பு பொய்;
}

தருக்க RTLIL::Cell::output(RTLIL::IdString portname) மாறிலி
{
	எனில் (yosys_celltypes.cell_known(type))
		திரும்பு yosys_celltypes.cell_output(type, portname);
	எனில் (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		திரும்பு w && w->port_output;
	}
	திரும்பு பொய்;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) மாறிலி
{
	எனில் (yosys_celltypes.cell_known(type.ref()))
		திரும்பு yosys_celltypes.cell_port_dir(type, portname);
	எனில் (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		எனில் (m == nullptr)
			திரும்பு PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		எனில் (w == nullptr)
			திரும்பு PortDir::PD_UNKNOWN;
		திரும்பு PortDir(w->port_உள்ளீடு + w->port_output * 2);
	}
	திரும்பு PortDir::PD_UNKNOWN;
}

தருக்க RTLIL::Cell::hasParam(RTLIL::IdString paramname) மாறிலி
{
	திரும்பு parameters.மொத்தம்(paramname) != 0;
}

வெறுமை RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

வெறுமை RTLIL::Cell::setParam(RTLIL::IdString paramname, RTLIL::Const மதிப்பு)
{
	parameters[paramname] = std::move(மதிப்பு);
}

மாறிலி RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) மாறிலி
{
	மாறிலி auto &it = parameters.find(paramname);
	எனில் (it != parameters.end())
		திரும்பு it->second;
	எனில் (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		எனில் (m)
			திரும்பு m->parameter_default_மதிப்புகள்.at(paramname);
	}
	எறி std::out_of_range("Cell::getParam()");
}

வெறுமை RTLIL::Cell::sort()
{
	connections_.sort(sort_by_id_str(module->twines()));
	parameters.sort(sort_by_id_str(module->twines()));
	attributes.sort(sort_by_id_str(module->twines()));
}

வெறுமை RTLIL::Cell::check()
{
#ifndef NDEBUG
	InternalCellChecker checker(வெற்று, இது);
	checker.check();
#endif
}

வெறுமை RTLIL::Cell::fixup_parameters(தருக்க set_a_signed, தருக்க set_b_signed)
{
	எனில் (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		திரும்பு;

	எனில் (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		எனில் (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		திரும்பு;
	}

	எனில் (type == ID($demux)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		திரும்பு;
	}

	எனில் (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		திரும்பு;
	}

	எனில் (type == ID($fa)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		திரும்பு;
	}

	எனில் (type == ID($lcu)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::CO]);
		திரும்பு;
	}

	எனில் (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);
		திரும்பு;
	}

	தருக்க signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	எனில் (connections_.மொத்தம்(ID::A)) {
		எனில் (signedness_ab) {
			எனில் (set_a_signed)
				parameters[ID::A_SIGNED] = உண்மை;
			இல்லையெனில் எனில் (parameters.மொத்தம்(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = பொய்;
		}
		parameters[ID::A_WIDTH] = GetSize(connections_[ID::A]);
	}

	எனில் (connections_.மொத்தம்(ID::B)) {
		எனில் (signedness_ab) {
			எனில் (set_b_signed)
				parameters[ID::B_SIGNED] = உண்மை;
			இல்லையெனில் எனில் (parameters.மொத்தம்(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = பொய்;
		}
		parameters[ID::B_WIDTH] = GetSize(connections_[ID::B]);
	}

	எனில் (connections_.மொத்தம்(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);

	எனில் (connections_.மொத்தம்(ID::Q))
		parameters[ID::WIDTH] = GetSize(connections_[ID::Q]);

	check();
}

தருக்க RTLIL::Cell::has_keep_attr() மாறிலி {
	திரும்பு get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

தருக்க RTLIL::Cell::has_memid() மாறிலி
{
	திரும்பு type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

தருக்க RTLIL::Cell::is_mem_cell() மாறிலி
{
	திரும்பு type.in(ID($mem), ID($mem_v2)) || has_memid();
}

தருக்க RTLIL::Cell::is_builtin_ff() மாறிலி {
	திரும்பு StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(மாறிலி RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	எனில் (wire == வெற்று)
		data = {bit.data};
	இல்லையெனில்
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(முழு offset, முழு length) மாறிலி
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= width);
	RTLIL::SigChunk ret;
	எனில் (wire) {
		ret.wire = wire;
		ret.offset = இது->offset + offset;
		ret.width = length;
	} இல்லையெனில் {
		க்கு (முழு i = 0; i < length; i++)
			ret.data.தள்ளு_back(data[offset+i]);
		ret.width = length;
	}
	திரும்பு ret;
}

RTLIL::SigBit RTLIL::SigChunk::operator[](முழு offset) மாறிலி
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	எனில் (wire) {
		ret.wire = wire;
		ret.offset = இது->offset + offset;
	} இல்லையெனில் {
		ret.data = data[offset];
	}
	திரும்பு ret;
}

தருக்க RTLIL::SigChunk::operator <(மாறிலி RTLIL::SigChunk &other) மாறிலி
{
	எனில் (wire && other.wire)
		எனில் (wire->name != other.wire->name)
			திரும்பு wire->name < other.wire->name;

	எனில் (wire != other.wire)
		திரும்பு wire < other.wire;

	எனில் (offset != other.offset)
		திரும்பு offset < other.offset;

	எனில் (width != other.width)
		திரும்பு width < other.width;

	திரும்பு data < other.data;
}

தருக்க RTLIL::SigChunk::operator ==(மாறிலி RTLIL::SigChunk &other) மாறிலி
{
	திரும்பு wire == other.wire && width == other.width && offset == other.offset && data == other.data;
}

தருக்க RTLIL::SigChunk::operator !=(மாறிலி RTLIL::SigChunk &other) மாறிலி
{
	எனில் (*இது == other)
		திரும்பு பொய்;
	திரும்பு உண்மை;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.size() > 0);
	auto ie = parts.begin();
	auto it = ie + parts.size() - 1;
	போது (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(மாறிலி RTLIL::Const &மதிப்பு)
{
	எனில் (GetSize(மதிப்பு) != 0) {
		rep_ = CHUNK;
		புதிய (&chunk_) RTLIL::SigChunk(மதிப்பு);
	} இல்லையெனில் {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&மதிப்பு)
{
	எனில் (GetSize(மதிப்பு) != 0) {
		rep_ = CHUNK;
		புதிய (&chunk_) RTLIL::SigChunk(மதிப்பு);
	} இல்லையெனில் {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(மாறிலி RTLIL::SigChunk &chunk)
{
	எனில் (chunk.width != 0) {
		rep_ = CHUNK;
		புதிய (&chunk_) RTLIL::SigChunk(chunk);
	} இல்லையெனில் {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	எனில் (chunk.width != 0) {
		rep_ = CHUNK;
		புதிய (&chunk_) RTLIL::SigChunk(chunk);
	} இல்லையெனில் {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	எனில் (wire->width != 0) {
		rep_ = CHUNK;
		புதிய (&chunk_) RTLIL::SigChunk(wire);
	} இல்லையெனில் {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, முழு offset, முழு width)
{
	எனில் (width != 0) {
		rep_ = CHUNK;
		புதிய (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} இல்லையெனில் {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(மாறிலி std::string &str)
{
	எனில் (str.size() != 0) {
		rep_ = CHUNK;
		புதிய (&chunk_) RTLIL::SigChunk(str);
	} இல்லையெனில் {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(முழு val, முழு width)
{
	எனில் (width != 0) {
		rep_ = CHUNK;
		புதிய (&chunk_) RTLIL::SigChunk(val, width);
	} இல்லையெனில்
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, முழு width)
{
	எனில் (width != 0) {
		rep_ = CHUNK;
		புதிய (&chunk_) RTLIL::SigChunk(bit, width);
	} இல்லையெனில்
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(மாறிலி RTLIL::SigBit &bit, முழு width)
{
	எனில் (width != 0) {
		எனில் (bit.wire == வெற்று) {
			rep_ = CHUNK;
			புதிய (&chunk_) RTLIL::SigChunk(bit.data, width);
		} இல்லையெனில் எனில் (width == 1) {
			rep_ = CHUNK;
			புதிய (&chunk_) RTLIL::SigChunk(bit);
		} இல்லையெனில் {
			init_empty_bits();
			bits_.reserve(width);
			க்கு (முழு i = 0; i < width; i++)
				bits_.தள்ளு_back(bit);
		}
	} இல்லையெனில்
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(மாறிலி std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	க்கு (மாறிலி auto &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(மாறிலி std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	க்கு (மாறிலி auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(மாறிலி pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	க்கு (மாறிலி auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(மாறிலி std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	க்கு (மாறிலி auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(தருக்க bit)
{
	rep_ = CHUNK;
	புதிய (&chunk_) RTLIL::SigChunk(bit ? RTLIL::S1 : RTLIL::S0);
	check();
}

வெறுமை RTLIL::SigSpec::Chunks::const_iterator::next_chunk_bits()
{
	முழு bits_size = GetSize(spec.bits_);
	எனில் (bit_index >= bits_size)
		திரும்பு;
	முழு i = bit_index;
	மாறிலி SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.data.clear();
	எனில் (bit.is_wire()) {
		chunk.offset = bit.offset;
		போது (i < bits_size && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_index)
			++i;
	} இல்லையெனில் {
		chunk.offset = 0;
		chunk.data.தள்ளு_back(bit.data);
		போது (i < bits_size && !spec.bits_[i].is_wire()) {
			chunk.data.தள்ளு_back(spec.bits_[i].data);
			++i;
		}
	}
	chunk.width = i - bit_index;
}

வெறுமை RTLIL::SigSpec::unpack()
{
	எனில் (rep_ == BITS)
		திரும்பு;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	க்கு (முழு i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	புதிய (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

வெறுமை RTLIL::SigSpec::try_repack()
{
	எனில் (rep_ != BITS)
		திரும்பு;

	முழு bits_size = GetSize(bits_);
	எனில் (bits_size == 0)
		திரும்பு;
	எனில் (bits_[0].is_wire()) {
		க்கு (முழு i = 1; i < bits_size; i++)
			எனில் (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				திரும்பு;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_size);
		bits_.~vector();
		rep_ = CHUNK;
		புதிய (&chunk_) SigChunk(std::move(chunk));
		திரும்பு;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_size);
	bits.தள்ளு_back(bits_[0].data);
	க்கு (முழு i = 1; i < bits_size; i++) {
		எனில் (bits_[i].is_wire())
			திரும்பு;
		bits.தள்ளு_back(bits_[i].data);
	}
	bits_.~vector();
	rep_ = CHUNK;
	புதிய (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() மாறிலி
{
	Hasher h;
	க்கு (auto &c : chunks())
		எனில் (c.wire == வெற்று) {
			க்கு (auto &v : c.data)
				h.eat(v);
		} இல்லையெனில் {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t result = h.yield();
	எனில் (result == 0)
		result = 1;
	hash_.set(result);
	திரும்பு result;
}

வெறுமை RTLIL::SigSpec::sort()
{
	unpack();
	std::sort(bits_.begin(), bits_.end());
	hash_.clear();
	try_repack();
}

வெறுமை RTLIL::SigSpec::sort_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::vector<SigBit> unique_bits = bits_;
	std::sort(unique_bits.begin(), unique_bits.end());
	auto last = std::unique(unique_bits.begin(), unique_bits.end());
	unique_bits.erase(last, unique_bits.end());

	*இது = unique_bits;
	hash_.clear();
	try_repack();
}

வெறுமை RTLIL::SigSpec::replace(மாறிலி RTLIL::SigSpec &pattern, மாறிலி RTLIL::SigSpec &with)
{
	replace(pattern, with, இது);
}

வெறுமை RTLIL::SigSpec::replace(மாறிலி RTLIL::SigSpec &pattern, மாறிலி RTLIL::SigSpec &with, RTLIL::SigSpec *other) மாறிலி
{
	log_assert(other != வெற்று);
	log_assert(size() == other->size());
	log_assert(pattern.size() == with.size());

	dict<RTLIL::SigBit, முழு> pattern_to_with;
	முழு pattern_size = pattern.size();
	க்கு (முழு i = 0; i < pattern_size; i++) {
		SigBit pattern_bit = pattern[i];
		எனில் (pattern_bit.wire != வெற்று) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	முழு this_size = size();
	தருக்க other_modified = பொய்;
	க்கு (முழு j = 0; j < this_size; j++) {
		auto it = pattern_to_with.find((*இது)[j]);
		எனில் (it != pattern_to_with.end()) {
			other_modified = உண்மை;
			(*other)[j] = with[it->second];
		}
	}

	எனில் (other_modified)
		other->try_repack();
	other->check();
}

வெறுமை RTLIL::SigSpec::replace(மாறிலி dict<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, இது);
}

வெறுமை RTLIL::SigSpec::replace(மாறிலி dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) மாறிலி
{
	log_assert(other != வெற்று);
	log_assert(size() == other->size());

	எனில் (rules.empty()) திரும்பு;

	முழு this_size = size();
	தருக்க other_modified = பொய்;
	க்கு (முழு i = 0; i < this_size; i++) {
		auto it = rules.find((*இது)[i]);
		எனில் (it != rules.end()) {
			other_modified = உண்மை;
			(*other)[i] = it->second;
		}
	}

	எனில் (other_modified)
		other->try_repack();
	other->check();
}

வெறுமை RTLIL::SigSpec::replace(மாறிலி std::map<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, இது);
}

வெறுமை RTLIL::SigSpec::replace(மாறிலி std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) மாறிலி
{
	log_assert(other != வெற்று);
	log_assert(size() == other->size());

	எனில் (rules.empty()) திரும்பு;

	முழு this_size = size();
	தருக்க other_modified = பொய்;
	க்கு (முழு i = 0; i < this_size; i++) {
		auto it = rules.find((*இது)[i]);
		எனில் (it != rules.end()) {
			other_modified = உண்மை;
			(*other)[i] = it->second;
		}
	}

	எனில் (other_modified)
		other->try_repack();
	other->check();
}

வெறுமை RTLIL::SigSpec::remove(மாறிலி RTLIL::SigSpec &pattern)
{
	remove2(pattern, வெற்று);
}

வெறுமை RTLIL::SigSpec::remove(மாறிலி RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) மாறிலி
{
	RTLIL::SigSpec tmp = *இது;
	tmp.remove2(pattern, other);
}

வெறுமை RTLIL::SigSpec::remove2(மாறிலி RTLIL::SigSpec &pattern, RTLIL::SigSpec *other)
{
	unpack();
	எனில் (other != வெற்று) {
		log_assert(size() == other->size());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.size());
	க்கு (auto &bit : pattern)
		எனில் (bit.wire != வெற்று)
			pattern_bits.செருகு(bit);

	// Compact in-place to avoid O(n^2) erase operations
	size_t write_idx = 0;
	க்கு (size_t read_idx = 0; read_idx < bits_.size(); read_idx++)
	{
		எனில் (!(bits_[read_idx].wire != வெற்று && pattern_bits.மொத்தம்(bits_[read_idx]))) {
			எனில் (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				எனில் (other != வெற்று)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	தருக்க modified = (write_idx < bits_.size());
	எனில் (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	எனில் (other != வெற்று && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

வெறுமை RTLIL::SigSpec::remove(மாறிலி pool<RTLIL::SigBit> &pattern)
{
	remove2(pattern, வெற்று);
}

வெறுமை RTLIL::SigSpec::remove(மாறிலி pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) மாறிலி
{
	RTLIL::SigSpec tmp = *இது;
	tmp.remove2(pattern, other);
}

வெறுமை RTLIL::SigSpec::remove2(மாறிலி pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	எனில் (other != வெற்று) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	க்கு (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		எனில் (!(bits_[read_idx].wire != வெற்று && pattern.மொத்தம்(bits_[read_idx]))) {
			எனில் (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				எனில் (other != வெற்று)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	தருக்க modified = (write_idx < bits_.size());
	எனில் (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	எனில் (other != வெற்று && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

வெறுமை RTLIL::SigSpec::remove2(மாறிலி std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	எனில் (other != வெற்று) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	க்கு (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		எனில் (!(bits_[read_idx].wire != வெற்று && pattern.மொத்தம்(bits_[read_idx]))) {
			எனில் (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				எனில் (other != வெற்று)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	தருக்க modified = (write_idx < bits_.size());
	எனில் (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	எனில் (other != வெற்று && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

வெறுமை RTLIL::SigSpec::remove2(மாறிலி pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	எனில் (other != வெற்று) {
		log_assert(size() == other->size());
		other->unpack();
	}

	தருக்க modified = பொய்;
	தருக்க other_modified = பொய்;
	க்கு (முழு i = GetSize(bits_) - 1; i >= 0; i--) {
		எனில் (bits_[i].wire != வெற்று && pattern.மொத்தம்(bits_[i].wire)) {
			modified = உண்மை;
			bits_.erase(bits_.begin() + i);
			எனில் (other != வெற்று) {
				other_modified = உண்மை;
				other->bits_.erase(other->bits_.begin() + i);
			}
		}
	}

	எனில் (modified) {
		hash_.clear();
		try_repack();
	}
	எனில் (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(மாறிலி RTLIL::SigSpec &pattern, மாறிலி RTLIL::SigSpec *other) மாறிலி
{
	log_assert(other == வெற்று || size() == other->size());

	RTLIL::SigSpec ret;
	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();

	க்கு (auto& pattern_chunk : pattern.chunks()) {
		எனில் (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			முழு i = 0;
			க்கு (மாறிலி RTLIL::SigBit &bit : bits_match) {
				எனில் (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} இல்லையெனில் {
			க்கு (மாறிலி RTLIL::SigBit &bit : bits_match)
				எனில் (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	திரும்பு ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(மாறிலி pool<RTLIL::SigBit> &pattern, மாறிலி RTLIL::SigSpec *other) மாறிலி
{
	log_assert(other == வெற்று || size() == other->size());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	எனில் (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		முழு i = 0;
		க்கு (மாறிலி RTLIL::SigBit &bit : bits_match) {
			எனில் (bit.wire && pattern.மொத்தம்(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} இல்லையெனில் {
		க்கு (மாறிலி RTLIL::SigBit &bit : bits_match)
			எனில் (bit.wire && pattern.மொத்தம்(bit))
				ret.append(bit);
	}

	ret.check();
	திரும்பு ret;
}

வெறுமை RTLIL::SigSpec::replace(முழு offset, மாறிலி RTLIL::SigSpec &with)
{
	எனில் (with.size() == 0)
		திரும்பு;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.size() >= 0);
	log_assert(offset+with.size() <= size());

	முழு i = 0;
	க்கு (மாறிலி RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

வெறுமை RTLIL::SigSpec::remove_const()
{
	எனில் (rep_ == CHUNK)
	{
		எனில் (chunk_.wire == வெற்று) {
			chunk_.~SigChunk();
			init_empty_bits();
			hash_.clear();
		}
	}
	இல்லையெனில்
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.size());
		க்கு (auto &bit : bits_)
			எனில் (bit.wire != வெற்று)
				new_bits.தள்ளு_back(bit);
		எனில் (GetSize(new_bits) != GetSize(bits_)) {
			bits_.swap(new_bits);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

வெறுமை RTLIL::SigSpec::remove(முழு offset, முழு length)
{
	எனில் (length == 0)
		திரும்பு;

	unpack();

	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	bits_.erase(bits_.begin() + offset, bits_.begin() + offset + length);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(முழு offset, முழு length) மாறிலி
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	std::vector<SigBit> extracted;
	SigBit first;
	தருக்க is_packing = உண்மை;
	க்கு (முழு i = offset; i < offset + length; i++) {
		தருக்க was_packing_before = is_packing;
		SigBit bit = (*இது)[i];
		எனில் (i == offset) {
			first = bit;
			எனில் (!bit.wire)
				is_packing = பொய்;
		} இல்லையெனில் {
			எனில் (bit.wire != first.wire)
				is_packing = பொய்;
			எனில் (bit.wire)
				எனில் (bit.offset != first.offset + (i - offset))
					is_packing = பொய்;
		}
		எனில் (was_packing_before && !is_packing)
			க்கு (முழு j = offset; j < i; j++)
				extracted.தள்ளு_back((*இது)[j]);
		எனில் (!is_packing)
			extracted.தள்ளு_back((*இது)[i]);
	}
	எனில் (is_packing)
		திரும்பு SigChunk(first.wire, first.offset, length);

	திரும்பு extracted;
}

வெறுமை RTLIL::SigSpec::rewrite_wires(std::function<வெறுமை(RTLIL::Wire*& wire)> rewrite)
{
	எனில் (rep_ == CHUNK) {
		எனில் (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		திரும்பு;
	}

	std::vector<RTLIL::SigBit> new_bits;
	க்கு (மாறிலி RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		எனில் (c.wire != nullptr)
			rewrite(c.wire);
		க்கு (முழு i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.clear();
}

வெறுமை RTLIL::SigSpec::append(மாறிலி RTLIL::SigSpec &சமிக்ஞை)
{
	எனில் (சமிக்ஞை.size() == 0)
		திரும்பு;

	எனில் (size() == 0) {
		*இது = சமிக்ஞை;
		திரும்பு;
	}

	hash_.clear();
	எனில் (rep_ == CHUNK && சமிக்ஞை.rep_ == CHUNK && chunk_.wire == சமிக்ஞை.chunk_.wire) {
		எனில் (chunk_.wire == வெற்று) {
			chunk_.data.செருகு(chunk_.data.end(), சமிக்ஞை.chunk_.data.begin(), சமிக்ஞை.chunk_.data.end());
			chunk_.width = GetSize(chunk_.data);
			திரும்பு;
		}
		எனில் (chunk_.offset + chunk_.width == சமிக்ஞை.chunk_.offset) {
			chunk_.width += சமிக்ஞை.chunk_.width;
			திரும்பு;
		}
	}

	unpack();
	க்கு (மாறிலி SigBit &bit : சமிக்ஞை.bits())
		bits_.தள்ளு_back(bit);
	check();
}

வெறுமை RTLIL::SigSpec::append(மாறிலி RTLIL::SigBit &bit)
{
	hash_.clear();

	எனில் (size() == 0) {
		destroy();
		rep_ = CHUNK;
		புதிய (&chunk_) RTLIL::SigChunk(bit);
		திரும்பு;
	}

	எனில் (rep_ == CHUNK && chunk_.wire == bit.wire) {
		எனில் (chunk_.wire == வெற்று) {
			chunk_.data.தள்ளு_back(bit.data);
			chunk_.width++;
			திரும்பு;
		}
		எனில் (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			திரும்பு;
		}
	}

	unpack();

	bits_.தள்ளு_back(bit);
	check();
}

வெறுமை RTLIL::SigSpec::extend_u0(முழு width, தருக்க is_signed)
{
	எனில் (size() > width)
		remove(width, size() - width);

	எனில் (size() < width) {
		RTLIL::SigBit padding = size() > 0 ? (*இது)[size() - 1] : RTLIL::State::Sx;
		எனில் (!is_signed)
			padding = RTLIL::State::S0;
		போது (size() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(முழு இலக்கம்) மாறிலி
{
	RTLIL::SigSpec sig;
	க்கு (முழு i = 0; i < இலக்கம்; i++)
		sig.append(*இது);
	திரும்பு sig;
}

#ifndef NDEBUG
வெறுமை RTLIL::SigSpec::check(மாறிலி Module *mod) மாறிலி
{
	எனில் (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		எனில் (chunk_.wire == வெற்று) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.data.size() == (size_t)chunk_.width);
		} இல்லையெனில் {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.data.size() == 0);
			எனில் (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	இல்லையெனில் எனில் (size() <= 64)
	{
		எனில் (mod != nullptr) {
			க்கு (மாறிலி RTLIL::SigBit &bit : bits_)
				எனில் (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#endif

தருக்க RTLIL::SigSpec::operator <(மாறிலி RTLIL::SigSpec &other) மாறிலி
{
	எனில் (இது == &other)
		திரும்பு பொய்;

	எனில் (size() != other.size())
		திரும்பு size() < other.size();

	auto other_it = other.chunks().begin();
	க்கு (மாறிலி SigChunk &c : chunks()) {
		எனில் (c != *other_it)
			திரும்பு c < *other_it;
		++other_it;
	}

	திரும்பு பொய்;
}

தருக்க RTLIL::SigSpec::operator ==(மாறிலி RTLIL::SigSpec &other) மாறிலி
{
	எனில் (இது == &other)
		திரும்பு உண்மை;

	எனில் (size() != other.size())
		திரும்பு பொய்;

	auto other_it = other.chunks().begin();
	க்கு (மாறிலி SigChunk &c : chunks()) {
		எனில் (c != *other_it)
			திரும்பு பொய்;
		++other_it;
	}

	திரும்பு உண்மை;
}

தருக்க RTLIL::SigSpec::is_wire() மாறிலி
{
	Chunks cs = chunks();
	auto it = cs.begin();
	எனில் (it == cs.end())
		திரும்பு பொய்;
	மாறிலி RTLIL::SigChunk &chunk = *it;
	திரும்பு chunk.wire && chunk.wire->width == size() && ++it == cs.end();
}

தருக்க RTLIL::SigSpec::is_chunk() மாறிலி
{
	Chunks cs = chunks();
	auto it = cs.begin();
	எனில் (it == cs.end())
		திரும்பு பொய்;
	திரும்பு ++it == cs.end();
}

தருக்க RTLIL::SigSpec::known_driver() மாறிலி
{
	க்கு (auto &chunk : chunks())
		எனில் (chunk.is_wire() && !chunk.wire->known_driver())
			திரும்பு பொய்;
	திரும்பு உண்மை;
}

தருக்க RTLIL::SigSpec::is_fully_const() மாறிலி
{
	க்கு (auto &chunk : chunks())
		எனில் (chunk.width > 0 && chunk.wire != வெற்று)
			திரும்பு பொய்;
	திரும்பு உண்மை;
}

தருக்க RTLIL::SigSpec::is_fully_zero() மாறிலி
{
	க்கு (auto &chunk : chunks()) {
		எனில் (chunk.width > 0 && chunk.wire != வெற்று)
			திரும்பு பொய்;
		க்கு (RTLIL::State d : chunk.data)
			எனில் (d != RTLIL::State::S0)
				திரும்பு பொய்;
	}
	திரும்பு உண்மை;
}

தருக்க RTLIL::SigSpec::is_fully_ones() மாறிலி
{
	க்கு (auto &chunk : chunks()) {
		எனில் (chunk.width > 0 && chunk.wire != வெற்று)
			திரும்பு பொய்;
		க்கு (RTLIL::State d : chunk.data)
			எனில் (d != RTLIL::State::S1)
				திரும்பு பொய்;
	}
	திரும்பு உண்மை;
}

தருக்க RTLIL::SigSpec::is_fully_def() மாறிலி
{
	க்கு (auto &chunk : chunks()) {
		எனில் (chunk.width > 0 && chunk.wire != வெற்று)
			திரும்பு பொய்;
		க்கு (RTLIL::State d : chunk.data)
			எனில் (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				திரும்பு பொய்;
	}
	திரும்பு உண்மை;
}

தருக்க RTLIL::SigSpec::is_fully_undef() மாறிலி
{
	க்கு (auto &chunk : chunks()) {
		எனில் (chunk.width > 0 && chunk.wire != வெற்று)
			திரும்பு பொய்;
		க்கு (RTLIL::State d : chunk.data)
			எனில் (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				திரும்பு பொய்;
	}
	திரும்பு உண்மை;
}

தருக்க RTLIL::SigSpec::has_const() மாறிலி
{
	க்கு (auto &chunk : chunks())
		எனில் (chunk.width > 0 && chunk.wire == வெற்று)
			திரும்பு உண்மை;
	திரும்பு பொய்;
}

தருக்க RTLIL::SigSpec::has_const(State state) மாறிலி
{
	க்கு (auto &chunk : chunks())
		எனில் (chunk.width > 0 && chunk.wire == வெற்று && std::find(chunk.data.begin(), chunk.data.end(), state) != chunk.data.end())
			திரும்பு உண்மை;
	திரும்பு பொய்;
}


தருக்க RTLIL::SigSpec::has_marked_bits() மாறிலி
{
	க்கு (auto &chunk : chunks())
		எனில் (chunk.width > 0 && chunk.wire == வெற்று) {
			க்கு (RTLIL::State d : chunk.data)
				எனில் (d == RTLIL::State::Sm)
					திரும்பு உண்மை;
		}
	திரும்பு பொய்;
}

தருக்க RTLIL::SigSpec::is_onehot(முழு *pos) மாறிலி
{
	எனில் (std::optional<RTLIL::Const> c = try_as_const())
		திரும்பு c->is_onehot(pos);
	திரும்பு பொய்;
}

தருக்க RTLIL::SigSpec::as_bool() மாறிலி
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_மதிப்பு());
	திரும்பு c->as_bool();
}

முழு RTLIL::SigSpec::as_int(தருக்க is_signed) மாறிலி
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_மதிப்பு());
	திரும்பு c->as_int(is_signed);
}

தருக்க RTLIL::SigSpec::convertible_to_int(தருக்க is_signed) மாறிலி
{
	std::optional<RTLIL::Const> c = try_as_const();
	எனில் (!c.has_மதிப்பு())
		திரும்பு பொய்;
	திரும்பு c->convertible_to_int(is_signed);
}

std::optional<முழு> RTLIL::SigSpec::try_as_int(தருக்க is_signed) மாறிலி
{
	std::optional<RTLIL::Const> c = try_as_const();
	எனில் (!c.has_மதிப்பு())
		திரும்பு std::nullopt;
	திரும்பு c->try_as_int(is_signed);
}

முழு RTLIL::SigSpec::as_int_saturating(தருக்க is_signed) மாறிலி
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_மதிப்பு());
	திரும்பு c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() மாறிலி
{
	std::string str;
	str.reserve(size());
	std::vector<RTLIL::SigChunk> chunks = *இது;
	க்கு (size_t i = chunks.size(); i > 0; i--) {
		மாறிலி RTLIL::SigChunk &chunk = chunks[i-1];
		எனில் (chunk.wire != வெற்று)
			str.append(chunk.width, '?');
		இல்லையெனில்
			str += RTLIL::Const(chunk.data).as_string();
	}
	திரும்பு str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() மாறிலி
{
	Chunks cs = chunks();
	auto it = cs.begin();
	எனில் (it == cs.end())
		திரும்பு RTLIL::Const();
	SigChunk chunk = *it;
	எனில் (chunk.wire != வெற்று || ++it != cs.end())
		திரும்பு std::nullopt;
	திரும்பு RTLIL::Const(std::move(chunk.data));
}

RTLIL::Const RTLIL::SigSpec::as_const() மாறிலி
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_மதிப்பு());
	திரும்பு *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() மாறிலி
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == size());
	திரும்பு chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() மாறிலி
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	திரும்பு chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() மாறிலி
{
	திரும்பு RTLIL::SigBit(*இது);
}

தருக்க RTLIL::SigSpec::match(மாறிலி எழுத்து* pattern) மாறிலி
{
	முழு pattern_len = strlen(pattern);
	log_assert(pattern_len == size());

	க்கு (முழு i = 0; i < pattern_len; ++i) {
		எழுத்து ch = pattern[i];
		எனில் (ch == ' ')
			தொடர்;
		RTLIL::SigBit bit = (*இது)[pattern_len - 1 - i];
		எனில் (ch == '*') {
			எனில் (bit != State::Sz && bit != State::Sx)
				திரும்பு பொய்;
			தொடர்;
		}
		எனில் (ch == '0') {
			எனில் (bit != State::S0)
				திரும்பு பொய்;
		} இல்லையெனில்
		எனில் (ch == '1') {
			எனில் (bit != State::S1)
				திரும்பு பொய்;
		} இல்லையெனில்
			log_abort();
	}

	திரும்பு உண்மை;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() மாறிலி
{
	std::set<RTLIL::SigBit> sigbits;
	க்கு (auto &c : chunks())
		க்கு (முழு i = 0; i < c.width; i++)
			sigbits.செருகு(RTLIL::SigBit(c, i));
	திரும்பு sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() மாறிலி
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(size());
	க்கு (auto &c : chunks())
		க்கு (முழு i = 0; i < c.width; i++)
			sigbits.செருகு(RTLIL::SigBit(c, i));
	திரும்பு sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() மாறிலி
{
	std::vector<RTLIL::SigBit> result;
	result.reserve(size());
	க்கு (SigBit bit : *இது)
		result.தள்ளு_back(bit);
	திரும்பு result;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(மாறிலி RTLIL::SigSpec &other) மாறிலி
{
	முழு this_size = size();
	log_assert(this_size == other.size());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	க்கு (முழு i = 0; i < this_size; i++)
		new_map[(*இது)[i]] = other[i];

	திரும்பு new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(மாறிலி RTLIL::SigSpec &other) மாறிலி
{
	முழு this_size = size();
	log_assert(this_size == other.size());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_size);
	க்கு (முழு i = 0; i < this_size; i++)
		new_map[(*இது)[i]] = other[i];

	திரும்பு new_map;
}

நிலையான வெறுமை sigspec_parse_split(std::vector<std::string> &tokens, மாறிலி std::string &text, எழுத்து sep)
{
	size_t start = 0, end = 0;
	போது ((end = text.find(sep, start)) != std::string::npos) {
		tokens.தள்ளு_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.தள்ளு_back(text.substr(start));
}

நிலையான RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, மாறிலி std::string &netname)
{
	திரும்பு module->wire(netname);
}

தருக்க RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	க்கு (முழு tokidx = முழு(tokens.size())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		எனில் (netname.size() == 0)
			தொடர்;

		எனில் (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			auto ast = p.const2ast(netname);
			எனில் (ast == nullptr)
				திரும்பு பொய்;
			sig.append(RTLIL::Const(ast->bits));
			தொடர்;
		}

		எனில் (module == வெற்று)
			திரும்பு பொய்;

		எனில் (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		எனில் (sigspec_parse_wire(module, netname) == nullptr) {
			size_t indices_pos = netname.size()-1;
			எனில் (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				போது (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				எனில் (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					போது (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				எனில் (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		எனில் (wire == nullptr)
			திரும்பு பொய்;

		எனில் (!indices.empty()) {
			std::vector<std::string> index_tokens;
			sigspec_parse_split(index_tokens, indices.substr(1, indices.size()-2), ':');
			எனில் (index_tokens.size() == 1) {
				முழு a = atoi(index_tokens.at(0).c_str());
				எனில் (a < 0 || a >= wire->width)
					திரும்பு பொய்;
				sig.append(RTLIL::SigSpec(wire, a));
			} இல்லையெனில் {
				முழு a = atoi(index_tokens.at(0).c_str());
				முழு b = atoi(index_tokens.at(1).c_str());
				எனில் (a > b) {
					முழு tmp = a;
					a = b, b = tmp;
				}
				எனில் (a < 0 || a >= wire->width)
					திரும்பு பொய்;
				எனில் (b < 0 || b >= wire->width)
					திரும்பு பொய்;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} இல்லையெனில்
			sig.append(wire);
	}

	திரும்பு உண்மை;
}

தருக்க RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	எனில் (str.empty() || str[0] != '@')
		திரும்பு parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	எனில் (sel_name == IdString::Null || design->selection_vars.மொத்தம்(sel_name) == 0)
		திரும்பு பொய்;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	க்கு (auto &it : module->wires_)
		எனில் (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	திரும்பு உண்மை;
}

தருக்க RTLIL::SigSpec::parse_rhs(மாறிலி RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	எனில் (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.size());
		திரும்பு உண்மை;
	}

	எனில் (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.size());
		திரும்பு உண்மை;
	}

	எனில் (lhs.is_chunk()) {
		எழுத்து *p = (எழுத்து*)str.c_str(), *endptr;
		long முழு val = strtol(p, &endptr, 10);
		எனில் (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.size());
			திரும்பு உண்மை;
		}
	}

	எனில் (!parse(sig, module, str))
		திரும்பு பொய்;
	எனில் (sig.size() > lhs.size())
		sig.remove(lhs.size(), sig.size() - lhs.size());
	திரும்பு உண்மை;
}

RTLIL::SigSpec::operator std::vector<RTLIL::SigChunk>() மாறிலி
{
	std::vector<RTLIL::SigChunk> result;
	க்கு (மாறிலி RTLIL::SigChunk &c : chunks())
		result.தள்ளு_back(c);
	திரும்பு result;
}

RTLIL::CaseRule::~CaseRule()
{
	க்கு (auto it = switches.begin(); it != switches.end(); it++)
		delete *it;
}

தருக்க RTLIL::CaseRule::empty() மாறிலி
{
	திரும்பு actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() மாறிலி
{
	RTLIL::CaseRule *new_caserule = புதிய RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	க்கு (auto &it : switches)
		new_caserule->switches.தள்ளு_back(it->clone());
	திரும்பு new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	க்கு (auto it = cases.begin(); it != cases.end(); it++)
		delete *it;
}

தருக்க RTLIL::SwitchRule::empty() மாறிலி
{
	திரும்பு cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() மாறிலி
{
	RTLIL::SwitchRule *new_switchrule = புதிய RTLIL::SwitchRule;
	new_switchrule->சமிக்ஞை = சமிக்ஞை;
	new_switchrule->attributes = attributes;
	க்கு (auto &it : cases)
		new_switchrule->cases.தள்ளு_back(it->clone());
	திரும்பு new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() மாறிலி
{
	RTLIL::SyncRule *new_syncrule = புதிய RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->சமிக்ஞை = சமிக்ஞை;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	திரும்பு new_syncrule;
}

RTLIL::Process::~Process()
{
	க்கு (auto it = syncs.begin(); it != syncs.end(); it++)
		delete *it;
}

RTLIL::Process *RTLIL::Process::clone() மாறிலி
{
	RTLIL::Process *new_proc = புதிய RTLIL::Process;

	new_proc->attributes = attributes;

	RTLIL::CaseRule *rc_ptr = வேர்_case.clone();
	new_proc->வேர்_case = *rc_ptr;
	rc_ptr->switches.clear();
	delete rc_ptr;

	க்கு (auto &it : syncs)
		new_proc->syncs.தள்ளு_back(it->clone());

	திரும்பு new_proc;
}

RTLIL::Memory::~Memory()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
நிலையான std::map<unsigned முழு, RTLIL::Memory*> all_memorys;
std::map<unsigned முழு, RTLIL::Memory*> *RTLIL::Memory::get_all_memorys(வெறுமை)
{
	திரும்பு &all_memorys;
}
#endif
YOSYS_NAMESPACE_END
