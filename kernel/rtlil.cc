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

#अन्तर्भाव "kernel/rtlil.h"
#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/macc.h"
#अन्तर्भाव "kernel/newcelltypes.h"
#अन्तर्भाव "kernel/sigtools.h"
#अन्तर्भाव "kernel/threading.h"
#अन्तर्भाव "frontends/verilog/verilog_frontend.h"
#अन्तर्भाव "frontends/verilog/preproc.h"
#अन्तर्भाव "backends/rtlil/rtlil_backend.h"

#अन्तर्भाव <string.h>
#अन्तर्भाव <algorithm>
#अन्तर्भाव <charconv>
#अन्तर्भाव <optional>
#अन्तर्भाव <string_view>
#अन्तर्भाव <sstream>

YOSYS_NAMESPACE_BEGIN

dict<std::string, std::string> RTLIL::constpad;

ध्रुव pool<IdString> &RTLIL::builtin_ff_cell_types() {
	स्थिर ध्रुव pool<IdString> res = []() {
		pool<IdString> r;
		प्रति (size_t i = 0; i < StaticCellTypes::builder.गिनती; i++) {
			auto &cell = StaticCellTypes::builder.cells[i];
			यदि (cell.features.is_ff)
				r.जोड़ो(cell.type);
		}
		प्रत्यागम r;
	}();
	प्रत्यागम res;
}

#define check(condition) log_assert(condition && "malformed Const union")

ध्रुव Const::bitvectype& Const::get_bits() ध्रुव {
	check(is_bits());
	प्रत्यागम *get_if_bits();
}

ध्रुव std::string& Const::get_str() ध्रुव {
	check(is_str());
	प्रत्यागम *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	प्रत्यागम *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	प्रत्यागम *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	नूतन ((शून्य*)&str_) std::string(std::move(str));
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	वर्ण bytes[] = {
		(वर्ण)(val >> 24), (वर्ण)(val >> 16), (वर्ण)(val >> 8), (वर्ण)val
	};
	नूतन ((शून्य*)&str_) std::string(bytes, 4);
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val, पूर्णाङ्क width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	यदि ((width & 7) == 0) {
		नूतन ((शून्य*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		पूर्णाङ्क bytes = width >> 3;
		signed वर्ण sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<पूर्णाङ्क>(bytes, परिमाण(val));
		प्रति (पूर्णाङ्क i = 0; i < bytes; i++) {
			str[str.size() - 1 - i] = val;
			val = val >> 8;
		}
		प्रत्यागम;
	}

	नूतन ((शून्य*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	प्रति (पूर्णाङ्क i = 0; i < width; i++) {
		bv.धकेलो_back((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, पूर्णाङ्क width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	नूतन ((शून्य*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	प्रति (पूर्णाङ्क i = 0; i < width; i++)
		bv.धकेलो_back(bit);
}

RTLIL::Const::Const(ध्रुव std::vector<तार्किक> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	नूतन ((शून्य*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.size());
	प्रति (ध्रुव auto &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ध्रुव RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	यदि (is_str())
		नूतन ((शून्य*)&str_) std::string(other.get_str());
	अन्यथा यदि (is_bits())
		नूतन ((शून्य*)&bits_) bitvectype(other.get_bits());
	अन्यथा
		check(असत्य);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	यदि (is_str())
		नूतन ((शून्य*)&str_) std::string(std::move(other.get_str()));
	अन्यथा यदि (is_bits())
		नूतन ((शून्य*)&bits_) bitvectype(std::move(other.get_bits()));
	अन्यथा
		check(असत्य);
}

RTLIL::Const &RTLIL::Const::operator =(ध्रुव RTLIL::Const &other) {
	flags = other.flags;
	यदि (other.is_str()) {
		यदि (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(शून्य)नूतन ((शून्य*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} अन्यथा यदि (other.is_bits()) {
		यदि (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(शून्य)नूतन ((शून्य*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} अन्यथा {
		check(असत्य);
	}
	प्रत्यागम *एतत्;
}

RTLIL::Const::~Const() {
	यदि (is_bits())
		bits_.~bitvectype();
	अन्यथा यदि (is_str())
		str_.~string();
	अन्यथा
		check(असत्य);
}

तार्किक RTLIL::Const::operator<(ध्रुव RTLIL::Const &other) ध्रुव
{
	यदि (size() != other.size())
		प्रत्यागम size() < other.size();

	प्रति (पूर्णाङ्क i = 0; i < size(); i++)
		यदि ((*एतत्)[i] != other[i])
			प्रत्यागम (*एतत्)[i] < other[i];

	प्रत्यागम असत्य;
}

तार्किक RTLIL::Const::operator ==(ध्रुव RTLIL::Const &other) ध्रुव
{
	यदि (is_str() && other.is_str())
		प्रत्यागम get_str() == other.get_str();
	यदि (is_bits() && other.is_bits())
		प्रत्यागम get_bits() == other.get_bits();

	यदि (size() != other.size())
		प्रत्यागम असत्य;

	प्रति (पूर्णाङ्क i = 0; i < size(); i++)
	यदि ((*एतत्)[i] != other[i])
		प्रत्यागम असत्य;

	प्रत्यागम सत्य;
}

तार्किक RTLIL::Const::operator !=(ध्रुव RTLIL::Const &other) ध्रुव
{
	प्रत्यागम !(*एतत् == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	प्रत्यागम get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() ध्रुव
{
	std::vector<State> v;
	प्रति (auto bit : *एतत्)
		v.धकेलो_back(bit);
	प्रत्यागम v;
}

तार्किक RTLIL::Const::as_bool() ध्रुव
{
	यदि (is_str()) {
		प्रति (वर्ण ch : get_str())
			यदि (ch != 0)
				प्रत्यागम सत्य;
		प्रत्यागम असत्य;
	}

	ध्रुव bitvectype& bv = get_bits();
	प्रति (size_t i = 0; i < bv.size(); i++)
		यदि (bv[i] == State::S1)
			प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}

पूर्णाङ्क RTLIL::Const::as_int(तार्किक is_signed) ध्रुव
{
	int32_t ret = 0;
	यदि (ध्रुव std::string *s = get_if_str()) {
		पूर्णाङ्क size = GetSize(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		प्रति (पूर्णाङ्क i = std::min(4, size); i > 0; i--)
			ret |= static_cast<unsigned वर्ण>((*s)[size - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		यदि (is_signed && size > 0 && size < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << size*8;
		प्रत्यागम ret;
	}

	ध्रुव bitvectype& bv = get_bits();
	पूर्णाङ्क significant_bits = std::min(GetSize(bv), 32);
	प्रति (पूर्णाङ्क i = 0; i < significant_bits; i++)
		यदि (bv[i] == State::S1)
			ret |= 1 << i;
	यदि (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	प्रत्यागम ret;
}

तार्किक RTLIL::Const::convertible_to_int(तार्किक is_signed) ध्रुव
{
	auto size = get_min_size(is_signed);

	यदि (size < 0)
		प्रत्यागम असत्य;

	// If it fits in 31 bits it is definitely convertible
	यदि (size <= 31)
		प्रत्यागम सत्य;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	यदि (size == 32) {
		यदि (is_signed)
			प्रत्यागम सत्य;
		प्रत्यागम (*एतत्)[size - 1] != State::S1;
	}

	प्रत्यागम असत्य;
}

std::optional<पूर्णाङ्क> RTLIL::Const::try_as_int(तार्किक is_signed) ध्रुव
{
	यदि (!convertible_to_int(is_signed))
		प्रत्यागम std::nullopt;
	प्रत्यागम as_int(is_signed);
}

पूर्णाङ्क RTLIL::Const::as_int_saturating(तार्किक is_signed) ध्रुव
{
	यदि (!convertible_to_int(is_signed)) {
		यदि (!is_signed)
			प्रत्यागम std::numeric_limits<पूर्णाङ्क>::max();

		ध्रुव auto min_size = get_min_size(is_signed);
		log_assert(min_size > 0);
		ध्रुव auto neg = (*एतत्)[min_size - 1];
		प्रत्यागम neg ? std::numeric_limits<पूर्णाङ्क>::min() : std::numeric_limits<पूर्णाङ्क>::max();
	}
	प्रत्यागम as_int(is_signed);
}

शून्य RTLIL::Const::tag_bare_integer_const(ध्रुव std::string &मूल्य)
{
	यदि (मूल्य.empty() || मूल्य.find('\'') != std::string::npos)
		प्रत्यागम;
	size_t start = (मूल्य[0] == '-' || मूल्य[0] == '+') ? 1 : 0;
	यदि (start < मूल्य.size() && std::all_of(मूल्य.begin() + start, मूल्य.end(), ::isdigit))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

पूर्णाङ्क RTLIL::Const::get_min_size(तार्किक is_signed) ध्रुव
{
	यदि (empty()) प्रत्यागम 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	यदि (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	अन्यथा
		leading_bit = RTLIL::State::S0;

	auto idx = size();
	यावत् (idx > 0 && (*एतत्)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	यदि (is_signed && idx < size()) {
		idx++;
	}
	// must be at least one bit
	प्रत्यागम (idx == 0) ? 1 : idx;
}

शून्य RTLIL::Const::compress(तार्किक is_signed)
{
	auto idx = get_min_size(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<पूर्णाङ्क> RTLIL::Const::as_int_compress(तार्किक is_signed) ध्रुव
{
	प्रत्यागम try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ध्रुव वर्ण* any) ध्रुव
{
	पूर्णाङ्क sz = size();
	std::string ret;
	ret.reserve(sz);
	प्रति (पूर्णाङ्क i = sz - 1; i >= 0; --i)
		विकल्प ((*एतत्)[i]) {
			पक्ष S0: ret.धकेलो_back('0'); भङ्ग;
			पक्ष S1: ret.धकेलो_back('1'); भङ्ग;
			पक्ष Sx: ret.धकेलो_back('x'); भङ्ग;
			पक्ष Sz: ret.धकेलो_back('z'); भङ्ग;
			पक्ष Sa: ret += any; भङ्ग;
			पक्ष Sm: ret.धकेलो_back('m'); भङ्ग;
		}
	प्रत्यागम ret;
}

RTLIL::Const RTLIL::Const::from_string(ध्रुव std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.size());
	प्रति (auto it = str.rbegin(); it != str.rend(); it++)
		विकल्प (*it) {
			पक्ष '0': bv.धकेलो_back(State::S0); भङ्ग;
			पक्ष '1': bv.धकेलो_back(State::S1); भङ्ग;
			पक्ष 'x': bv.धकेलो_back(State::Sx); भङ्ग;
			पक्ष 'z': bv.धकेलो_back(State::Sz); भङ्ग;
			पक्ष 'm': bv.धकेलो_back(State::Sm); भङ्ग;
			स्वभाव: bv.धकेलो_back(State::Sa);
		}
	प्रत्यागम c;
}

std::string RTLIL::Const::decode_string() ध्रुव
{
	यदि (auto str = get_if_str())
		प्रत्यागम *str;

	ध्रुव bitvectype& bv = get_bits();
	ध्रुव पूर्णाङ्क n = GetSize(bv);
	ध्रुव पूर्णाङ्क n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	पूर्णाङ्क i = n_over_8 * 8;
	यदि (i < n) {
		वर्ण ch = 0;
		प्रति (पूर्णाङ्क j = 0; j < (n - i); j++) {
			यदि (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		यदि (ch != 0)
			s.append({ch});
	}
	i -= 8;
	प्रति (; i >= 0; i -= 8) {
		वर्ण ch = 0;
		प्रति (पूर्णाङ्क j = 0; j < 8; j++) {
			यदि (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		यदि (ch != 0)
			s.append({ch});
	}
	प्रत्यागम s;
}

पूर्णाङ्क RTLIL::Const::size() ध्रुव {
	यदि (is_str())
		प्रत्यागम 8 * str_.size();
	अन्यथा {
		check(is_bits());
		प्रत्यागम bits_.size();
	}
}

तार्किक RTLIL::Const::empty() ध्रुव {
	यदि (is_str())
		प्रत्यागम str_.empty();
	अन्यथा {
		check(is_bits());
		प्रत्यागम bits_.empty();
	}
}

शून्य RTLIL::Const::bitvectorize_internal() {
	यदि (tag == backing_tag::bits)
		प्रत्यागम;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.size() * 8);
	प्रति (पूर्णाङ्क i = str_.size() - 1; i >= 0; i--) {
		unsigned वर्ण ch = str_[i];
		प्रति (पूर्णाङ्क j = 0; j < 8; j++) {
			new_bits.धकेलो_back((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(शून्य)नूतन ((शून्य*)&bits_) bitvectype(std::move(new_bits));
		tag = backing_tag::bits;
	}
}

शून्य RTLIL::Const::append(ध्रुव RTLIL::Const &other) {
	bitvectorize_internal();
	bitvectype& bv = get_bits();
	bv.जोड़ो(bv.end(), other.begin(), other.end());
}

RTLIL::State RTLIL::Const::const_iterator::operator*() ध्रुव {
	यदि (auto bv = parent->get_if_bits())
		प्रत्यागम (*bv)[idx];

	पूर्णाङ्क char_idx = parent->get_str().size() - idx / 8 - 1;
	तार्किक bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	प्रत्यागम bit ? State::S1 : State::S0;
}

तार्किक RTLIL::Const::is_fully_zero() ध्रुव
{
	यदि (auto str = get_if_str()) {
		प्रति (वर्ण ch : *str)
			यदि (ch != 0)
				प्रत्यागम असत्य;
		प्रत्यागम सत्य;
	}

	ध्रुव bitvectype& bv = get_bits();

	प्रति (ध्रुव auto &bit : bv)
		यदि (bit != RTLIL::State::S0)
			प्रत्यागम असत्य;

	प्रत्यागम सत्य;
}

तार्किक RTLIL::Const::is_fully_ones() ध्रुव
{
	यदि (auto str = get_if_str()) {
		प्रति (वर्ण ch : *str)
			यदि (ch != (वर्ण)0xff)
				प्रत्यागम असत्य;
		प्रत्यागम सत्य;
	}

	ध्रुव bitvectype& bv = get_bits();
	प्रति (ध्रुव auto &bit : bv)
		यदि (bit != RTLIL::State::S1)
			प्रत्यागम असत्य;

	प्रत्यागम सत्य;
}

तार्किक RTLIL::Const::is_fully_def() ध्रुव
{
	यदि (is_str())
		प्रत्यागम सत्य;

	ध्रुव bitvectype& bv = get_bits();
	प्रति (ध्रुव auto &bit : bv)
		यदि (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			प्रत्यागम असत्य;

	प्रत्यागम सत्य;
}

तार्किक RTLIL::Const::is_fully_undef() ध्रुव
{
	यदि (auto str = get_if_str())
		प्रत्यागम str->empty();

	ध्रुव bitvectype& bv = get_bits();
	प्रति (ध्रुव auto &bit : bv)
		यदि (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			प्रत्यागम असत्य;

	प्रत्यागम सत्य;
}

तार्किक RTLIL::Const::is_fully_undef_x_only() ध्रुव
{
	यदि (auto str = get_if_str())
		प्रत्यागम str->empty();

	ध्रुव bitvectype& bv = get_bits();
	प्रति (ध्रुव auto &bit : bv)
		यदि (bit != RTLIL::State::Sx)
			प्रत्यागम असत्य;

	प्रत्यागम सत्य;
}

तार्किक RTLIL::Const::is_onehot(पूर्णाङ्क *pos) ध्रुव
{
	तार्किक found = असत्य;
	पूर्णाङ्क size = GetSize(*एतत्);
	प्रति (पूर्णाङ्क i = 0; i < size; i++) {
		State bit = (*एतत्)[i];
		यदि (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			प्रत्यागम असत्य;
		यदि (bit == RTLIL::State::S1) {
			यदि (found)
				प्रत्यागम असत्य;
			यदि (pos)
				*pos = i;
			found = सत्य;
		}
	}
	प्रत्यागम found;
}

Hasher RTLIL::Const::hash_into(Hasher h) ध्रुव
{
	यदि (auto str = get_if_str())
		प्रत्यागम hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ध्रुव bitvectype &bv = get_bits();
	पूर्णाङ्क size = GetSize(bv);
	std::string packed;
	पूर्णाङ्क packed_size = (size + 7) >> 3;
	packed.resize(packed_size, 0);
	प्रति (पूर्णाङ्क bi = 0; bi < packed_size; ++bi) {
		वर्ण ch = 0;
		पूर्णाङ्क end = std::min((bi + 1)*8, size);
		प्रति (पूर्णाङ्क i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			यदि (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<वर्ण>::hash_into(ch, h);
				प्रति (; i < size; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(size);
				प्रत्यागम h;
			}
			ch |= static_cast<पूर्णाङ्क>(b) << (i & 7);
		}
		packed[packed_size - 1 - bi] = ch;
	}
	प्रत्यागम hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(पूर्णाङ्क offset, पूर्णाङ्क len, RTLIL::State padding) ध्रुव {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	प्रति (पूर्णाङ्क i = offset; i < offset + len; i++)
		ret_bv.धकेलो_back(i < GetSize(*एतत्) ? (*एतत्)[i] : padding);
	प्रत्यागम RTLIL::Const(ret_bv);
}
#undef check /* check(condition) for Const */

तार्किक RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ध्रुव
{
	प्रत्यागम attributes.गिनती(id);
}

शून्य RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, तार्किक मूल्य)
{
	यदि (मूल्य)
		attributes[id] = RTLIL::Const(1);
	अन्यथा
		attributes.erase(id);
}

तार्किक RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ध्रुव
{
	ध्रुव auto it = attributes.find(id);
	यदि (it == attributes.end())
		प्रत्यागम असत्य;
	प्रत्यागम it->second.as_bool();
}

शून्य RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string मूल्य)
{
	यदि (मूल्य.empty())
		attributes.erase(id);
	अन्यथा
		attributes[id] = मूल्य;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ध्रुव
{
	std::string मूल्य;
	ध्रुव auto it = attributes.find(id);
	यदि (it != attributes.end())
		मूल्य = it->second.decode_string();
	प्रत्यागम मूल्य;
}

शून्य RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, ध्रुव pool<string> &data)
{
	string attrval;
	प्रति (ध्रुव auto &s : data) {
		यदि (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

शून्य RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, ध्रुव pool<string> &data)
{
	pool<string> union_data = get_strpool_attribute(id);
	union_data.जोड़ो(data.begin(), data.end());
	यदि (!union_data.empty())
		set_strpool_attribute(id, union_data);
}

शून्य RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->attributes = std::move(buf);
}

नामाकाश {
	template<typename F>
	शून्य walk_process_attr_objects(RTLIL::Process *process, F visit) {
		visit(process);
		std::vector<RTLIL::CaseRule*> case_ढेर{&process->मूल_case};
		यावत् (!case_ढेर.empty()) {
			RTLIL::CaseRule *cs = case_ढेर.back();
			case_ढेर.निकालो_back();
			visit(cs);
			प्रति (auto *sw : cs->switches) {
				visit(sw);
				प्रति (auto *case_ : sw->cases)
					case_ढेर.धकेलो_back(case_);
			}
		}
		प्रति (auto *समकाल : process->syncs)
			प्रति (auto &mwa : समकाल->mem_write_actions)
				visit(&mwa);
	}

	template<typename F>
	शून्य walk_attr_objects(RTLIL::Design *design, F visit) {
		प्रति (auto &[_, module] : design->modules_) {
			visit(module);
			प्रति (auto &[_, wire] : module->wires_)
				visit(wire);
			प्रति (auto &[_, mem] : module->memories)
				visit(mem);
			प्रति (auto &[_, cell] : module->cells_)
				visit(cell);
			प्रति (auto &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


size_t RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	auto मूल = [&](IdString ref) {
		यदि (ref != IdString::Null)
			live.जोड़ो(ref);
	};

	walk_attr_objects(एतत्, [&](ध्रुव RTLIL::AttrObject *obj) {
		प्रति (auto &attr : obj->attributes)
			मूल(attr.first);
	});

	मूल(selected_active_module);

	प्रति (auto &[name, module] : modules_) {
		मूल(name);
		प्रति (IdString port : module->ports)
			मूल(port);
		प्रति (IdString param : module->avail_parameters)
			मूल(param);
		प्रति (auto &[param, _] : module->parameter_default_मूल्यावली)
			मूल(param);
		प्रति (auto &[name, _] : module->memories)
			मूल(name);
		प्रति (auto &[name, _] : module->processes)
			मूल(name);
		प्रति (auto &[name, wire] : module->wires_) {
			मूल(name);
			यदि (wire->known_driver())
				मूल(wire->driverPort());
		}
		प्रति (auto &[name, cell] : module->cells_) {
			मूल(name);
			मूल(cell->type);
			प्रति (auto &conn : cell->connections())
				मूल(conn.first);
			प्रति (auto &param : cell->parameters)
				मूल(param.first);
		}
		प्रति (auto &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_ढेर{&process->मूल_case};
			यावत् (!case_ढेर.empty()) {
				RTLIL::CaseRule *cs = case_ढेर.back();
				case_ढेर.निकालो_back();
				प्रति (auto *sw : cs->switches) {
					प्रति (auto *case_ : sw->cases)
						case_ढेर.धकेलो_back(case_);
				}
			}
			प्रति (auto *समकाल : process->syncs) {
				प्रति (auto &mwa : समकाल->mem_write_actions)
					मूल(mwa.memid);
			}
		}
	}

	auto मूल_selection = [&](ध्रुव RTLIL::Selection &sel) {
		प्रति (IdString m : sel.selected_modules)
			मूल(m);
		प्रति (auto &[m, members] : sel.selected_members) {
			मूल(m);
			प्रति (IdString member : members)
				मूल(member);
		}
	};

	प्रति (auto &[name, sel] : selection_vars) {
		मूल(name);
		मूल_selection(sel);
	}

	प्रति (auto &sel : selection_ढेर)
		मूल_selection(sel);

	size_t erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_गिनती;
	प्रत्यागम erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ध्रुव
{
	pool<string> data;
	यदि (attributes.गिनती(id) != 0)
		प्रति (auto s : split_tokens(get_string_attribute(id), "|"))
			data.जोड़ो(s);
	प्रत्यागम data;
}

शून्य RTLIL::AttrObject::set_hdlname_attribute(ध्रुव vector<string> &hierarchy)
{
	string attrval;
	प्रति (ध्रुव auto &ident : hierarchy) {
		यदि (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() ध्रुव
{
	प्रत्यागम split_tokens(get_string_attribute(ID::hdlname), " ");
}

शून्य RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, ध्रुव vector<पूर्णाङ्क> &data)
{
	std::stringstream attrval;
	प्रति (auto &i : data) {
		यदि (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<पूर्णाङ्क> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ध्रुव
{
	vector<पूर्णाङ्क> data;
	auto it = attributes.find(id);
	यदि (it != attributes.end())
		प्रति (ध्रुव auto &s : split_tokens(attributes.at(id).decode_string())) {
			वर्ण *end = nullptr;
			errno = 0;
			long मूल्य = strtol(s.c_str(), &end, 10);
			यदि (end != s.c_str() + s.size())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			यदि (errno == ERANGE || मूल्य < INT_MIN || मूल्य > INT_MAX)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			data.धकेलो_back(मूल्य);
		}
	प्रत्यागम data;
}

तार्किक RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ध्रुव
{
	यदि (current_design != nullptr) {
		auto module = current_design->module(mod_name);
		प्रत्यागम module && module->get_blackbox_attribute();
	} अन्यथा {
		log_warning("Unable to check if module is boxed for null design.\n");
		प्रत्यागम असत्य;
	}
}

तार्किक RTLIL::Selection::selected_module(RTLIL::IdString mod_name) ध्रुव
{
	यदि (complete_selection)
		प्रत्यागम सत्य;
	यदि (!selects_boxes && boxed_module(mod_name))
		प्रत्यागम असत्य;
	यदि (full_selection)
		प्रत्यागम सत्य;
	यदि (selected_modules.गिनती(mod_name) > 0)
		प्रत्यागम सत्य;
	यदि (selected_members.गिनती(mod_name) > 0)
		प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}

तार्किक RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) ध्रुव
{
	यदि (complete_selection)
		प्रत्यागम सत्य;
	यदि (!selects_boxes && boxed_module(mod_name))
		प्रत्यागम असत्य;
	यदि (full_selection)
		प्रत्यागम सत्य;
	यदि (selected_modules.गिनती(mod_name) > 0)
		प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}

तार्किक RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ध्रुव
{
	यदि (complete_selection)
		प्रत्यागम सत्य;
	यदि (!selects_boxes && boxed_module(mod_name))
		प्रत्यागम असत्य;
	यदि (full_selection)
		प्रत्यागम सत्य;
	यदि (selected_modules.गिनती(mod_name) > 0)
		प्रत्यागम सत्य;
	यदि (selected_members.गिनती(mod_name) > 0)
		यदि (selected_members.at(mod_name).गिनती(memb_name) > 0)
			प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}

शून्य RTLIL::Selection::optimize(RTLIL::Design *design)
{
	यदि (design != current_design) {
		current_design = design;
	}

	यदि (selects_boxes && full_selection)
		complete_selection = सत्य;
	यदि (complete_selection) {
		full_selection = असत्य;
		selects_boxes = सत्य;
	}
	यदि (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		प्रत्यागम;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.clear();
	प्रति (auto mod_name : selected_modules) {
		यदि (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.धकेलो_back(mod_name);
		selected_members.erase(mod_name);
	}
	प्रति (auto mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	प्रति (auto &it : selected_members)
		यदि (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.धकेलो_back(it.first);
	प्रति (auto mod_name : del_list)
		selected_members.erase(mod_name);

	प्रति (auto &it : selected_members) {
		del_list.clear();
		प्रति (auto memb_name : it.second)
			यदि (current_design->module(it.first)->गिनती_id(memb_name) == 0)
				del_list.धकेलो_back(memb_name);
		प्रति (auto memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	add_list.clear();
	प्रति (auto &it : selected_members)
		यदि (it.second.size() == 0)
			del_list.धकेलो_back(it.first);
		अन्यथा यदि (it.second.size() == current_design->module(it.first)->wires_.size() + current_design->module(it.first)->memories.size() +
				current_design->module(it.first)->cells_.size() + current_design->module(it.first)->processes.size())
			add_list.धकेलो_back(it.first);
	प्रति (auto mod_name : del_list)
		selected_members.erase(mod_name);
	प्रति (auto mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.जोड़ो(mod_name);
	}

	यदि (selected_modules.size() == current_design->modules_.size()) {
		selected_modules.clear();
		selected_members.clear();
		यदि (selects_boxes)
			complete_selection = सत्य;
		अन्यथा
			full_selection = सत्य;
	}
}

शून्य RTLIL::Selection::clear()
{
	full_selection = असत्य;
	complete_selection = असत्य;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_defines (नूतन define_map_t)
{
	स्थिर unsigned पूर्णाङ्क hashidx_गिनती = 123456789;
	hashidx_गिनती = mkhash_xorshift(hashidx_गिनती);
	hashidx_ = hashidx_गिनती;

	refcount_modules_ = 0;
	selected_active_module.clear();
	धकेलो_full_selection();

	RTLIL::Design::get_all_designs()->जोड़ो(std::pair<unsigned पूर्णाङ्क, RTLIL::Design*>(hashidx_, एतत्));
}

RTLIL::Design::~Design()
{
	प्रति (auto &pr : modules_)
		delete pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

स्थिर std::map<unsigned पूर्णाङ्क, RTLIL::Design*> all_designs;
std::map<unsigned पूर्णाङ्क, RTLIL::Design*> *RTLIL::Design::get_all_designs(शून्य)
{
	प्रत्यागम &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	प्रत्यागम RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ध्रुव RTLIL::Module *RTLIL::Design::module(IdString id) ध्रुव {
	twines_.check_owned(id);
	प्रत्यागम modules_.गिनती(id) ? modules_.at(id) : रिक्त;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	प्रत्यागम modules_.गिनती(id) ? modules_.at(id) : रिक्त;
}

RTLIL::Module *RTLIL::Design::शीर्ष_module() ध्रुव
{
	RTLIL::Module *module = nullptr;
	पूर्णाङ्क module_गिनती = 0;

	प्रति (auto mod : selected_modules()) {
		यदि (mod->get_bool_attribute(ID::शीर्ष))
			प्रत्यागम mod;
		module_गिनती++;
		module = mod;
	}

	प्रत्यागम module_गिनती == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	यदि (modules_.गिनती(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = नूतन RTLIL::Module;
	modules_[name] = module;
	module->design = एतत्;
	module->name = name;

	प्रति (auto mon : monitors)
		mon->notify_module_add(module);

	यदि (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	प्रत्यागम module;
}

शून्य RTLIL::Design::scratchpad_unset(ध्रुव std::string &varname)
{
	scratchpad.erase(varname);
}

शून्य RTLIL::Design::scratchpad_set_int(ध्रुव std::string &varname, पूर्णाङ्क मूल्य)
{
	scratchpad[varname] = stringf("%d", मूल्य);
}

शून्य RTLIL::Design::scratchpad_set_bool(ध्रुव std::string &varname, तार्किक मूल्य)
{
	scratchpad[varname] = मूल्य ? "true" : "false";
}

शून्य RTLIL::Design::scratchpad_set_string(ध्रुव std::string &varname, std::string मूल्य)
{
	scratchpad[varname] = std::move(मूल्य);
}

पूर्णाङ्क RTLIL::Design::scratchpad_get_int(ध्रुव std::string &varname, पूर्णाङ्क default_मूल्य_2) ध्रुव
{
	auto it = scratchpad.find(varname);
	यदि (it == scratchpad.end())
		प्रत्यागम default_मूल्य_2;

	ध्रुव std::string &str = it->second;

	यदि (str == "0" || str == "false")
		प्रत्यागम 0;

	यदि (str == "1" || str == "true")
		प्रत्यागम 1;

	वर्ण *endptr = nullptr;
	long पूर्णाङ्क parsed_मूल्य = strtol(str.c_str(), &endptr, 10);
	प्रत्यागम *endptr ? default_मूल्य_2 : parsed_मूल्य;
}

तार्किक RTLIL::Design::scratchpad_get_bool(ध्रुव std::string &varname, तार्किक default_मूल्य_2) ध्रुव
{
	auto it = scratchpad.find(varname);
	यदि (it == scratchpad.end())
		प्रत्यागम default_मूल्य_2;

	ध्रुव std::string &str = it->second;

	यदि (str == "0" || str == "false")
		प्रत्यागम असत्य;

	यदि (str == "1" || str == "true")
		प्रत्यागम सत्य;

	प्रत्यागम default_मूल्य_2;
}

std::string RTLIL::Design::scratchpad_get_string(ध्रुव std::string &varname, ध्रुव std::string &default_मूल्य_2) ध्रुव
{
	auto it = scratchpad.find(varname);
	यदि (it == scratchpad.end())
		प्रत्यागम default_मूल्य_2;

	प्रत्यागम it->second;
}

शून्य RTLIL::Design::remove(RTLIL::Module *module)
{
	प्रति (auto mon : monitors)
		mon->notify_module_del(module);

	यदि (yosys_xtrace) {
		log("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	delete module;
}

शून्य RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	add(module);
}

शून्य RTLIL::Design::sort()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
	प्रति (auto &it : modules_)
		it.second->sort();
}

शून्य RTLIL::Design::sort_modules()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
}

शून्य check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

शून्य RTLIL::Design::check()
{
#ifndef NDEBUG
	log_assert(!selection_ढेर.empty());
	पूर्णाङ्क pool_size = 0;
	प्रति (auto &it : modules_)
		pool_size = std::max(pool_size, ThreadPool::work_pool_size(0, it.second->cells_size(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_size);
	प्रति (auto &it : modules_) {
		log_assert(एतत् == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#endif
}

शून्य RTLIL::Design::optimize()
{
	प्रति (auto &it : modules_)
		it.second->optimize();
	प्रति (auto &it : selection_ढेर)
		it.optimize(एतत्);
	प्रति (auto &it : selection_vars)
		it.second.optimize(एतत्);
}

शून्य RTLIL::Design::clone_into(RTLIL::Design *dst) ध्रुव
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	प्रति (auto it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

तार्किक RTLIL::Design::selected_module(IdString mod_name) ध्रुव
{
	यदि (!selected_active_module.empty() && mod_name != selected_active_module)
		प्रत्यागम असत्य;
	प्रत्यागम selection().selected_module(mod_name);
}

तार्किक RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) ध्रुव
{
	यदि (!selected_active_module.empty() && mod_name != selected_active_module)
		प्रत्यागम असत्य;
	प्रत्यागम selection().selected_whole_module(mod_name);
}

तार्किक RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ध्रुव
{
	यदि (!selected_active_module.empty() && mod_name != selected_active_module)
		प्रत्यागम असत्य;
	प्रत्यागम selection().selected_member(mod_name, memb_name);
}

तार्किक RTLIL::Design::selected_module(RTLIL::Module *mod) ध्रुव
{
	प्रत्यागम selected_module(mod->name);
}

तार्किक RTLIL::Design::selected_whole_module(RTLIL::Module *mod) ध्रुव
{
	प्रत्यागम selected_whole_module(mod->name);
}

शून्य RTLIL::Design::धकेलो_selection(RTLIL::Selection sel)
{
	sel.current_design = एतत्;
	selection_ढेर.धकेलो_back(sel);
}

शून्य RTLIL::Design::धकेलो_empty_selection()
{
	धकेलो_selection(RTLIL::Selection::EmptySelection(एतत्));
}

शून्य RTLIL::Design::धकेलो_full_selection()
{
	धकेलो_selection(RTLIL::Selection::FullSelection(एतत्));
}

शून्य RTLIL::Design::धकेलो_complete_selection()
{
	धकेलो_selection(RTLIL::Selection::CompleteSelection(एतत्));
}

शून्य RTLIL::Design::निकालो_selection()
{
	selection_ढेर.निकालो_back();
	// Default to a full_selection if we ran out of stack
	यदि (selection_ढेर.empty())
		धकेलो_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(तार्किक only_selected) ध्रुव
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(एतत्), only_selected, सत्य, असत्य, RTLIL_BACKEND::DumpMode::Readable);
	प्रत्यागम f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ध्रुव
{
	तार्किक include_partials = partials == RTLIL::SELECT_ALL;
	तार्किक exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	तार्किक ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> result;
	result.reserve(modules_.size());
	प्रति (auto &it : modules_) {
		यदि (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			यदि (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				result.धकेलो_back(it.second);
			अन्यथा
				विकल्प (boxes)
				{
				पक्ष RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.second);
					भङ्ग;
				पक्ष RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.second);
					भङ्ग;
				पक्ष RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.second);
					भङ्ग;
				पक्ष RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.second);
					भङ्ग;
				पक्ष RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.second);
					भङ्ग;
				पक्ष RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.second);
					भङ्ग;
				स्वभाव:
					भङ्ग;
				}
		} अन्यथा यदि (!include_partials && selected_module(it.first)) {
			विकल्प(partials)
			{
			पक्ष RTLIL::SELECT_WHOLE_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.second);
				भङ्ग;
			पक्ष RTLIL::SELECT_WHOLE_ERR:
				log_error("Unsupported partially selected module %s.\n", it.second);
				भङ्ग;
			पक्ष RTLIL::SELECT_WHOLE_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.second);
				भङ्ग;
			स्वभाव:
				भङ्ग;
			}
		}
	}
	प्रत्यागम result;
}

RTLIL::Module::Module()
{
	स्थिर unsigned पूर्णाङ्क hashidx_गिनती = 123456789;
	hashidx_गिनती = mkhash_xorshift(hashidx_गिनती);
	hashidx_ = hashidx_गिनती;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->जोड़ो(std::pair<unsigned पूर्णाङ्क, RTLIL::Module*>(hashidx_, एतत्));
#endif
}

RTLIL::Module::~Module()
{
	प्रति (auto &pr : wires_)
		delete pr.second;
	प्रति (auto &pr : memories)
		delete pr.second;
	प्रति (auto &pr : cells_)
		delete pr.second;
	प्रति (auto &pr : processes)
		delete pr.second;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
स्थिर std::map<unsigned पूर्णाङ्क, RTLIL::Module*> all_modules;
std::map<unsigned पूर्णाङ्क, RTLIL::Module*> *RTLIL::Module::get_all_modules(शून्य)
{
	प्रत्यागम &all_modules;
}
#endif

शून्य RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	प्रति (auto it = wires_.begin(); it != wires_.end(); ++it)
		यदि (!it->second->port_निवेश && !it->second->port_output)
			delwires.जोड़ो(it->second);

	प्रति (auto it = memories.begin(); it != memories.end(); ++it)
		delete it->second;
	memories.clear();

	प्रति (auto it = cells_.begin(); it != cells_.end(); ++it)
		delete it->second;
	cells_.clear();

	प्रति (auto it = processes.begin(); it != processes.end(); ++it)
		delete it->second;
	processes.clear();

	connections_.clear();

	remove(delwires);
	set_bool_attribute(ID::blackbox);
}

शून्य RTLIL::Module::expand_interfaces(RTLIL::Design *, ध्रुव dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

तार्किक RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	प्रत्यागम असत्य;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ध्रुव dict<RTLIL::IdString, RTLIL::Const> &, तार्किक mayfail)
{
	यदि (mayfail)
		प्रत्यागम IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ध्रुव dict<RTLIL::IdString, RTLIL::Const> &, ध्रुव dict<RTLIL::IdString, RTLIL::Module*> &, ध्रुव dict<RTLIL::IdString, RTLIL::IdString> &, तार्किक mayfail)
{
	यदि (mayfail)
		प्रत्यागम IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

size_t RTLIL::Module::गिनती_id(RTLIL::IdString id)
{
	प्रत्यागम wires_.गिनती(id) + cells_.गिनती(id) + memories.गिनती(id) + processes.गिनती(id);
}

#ifndef NDEBUG
नामाकाश {
	संरचना InternalCellChecker
	{
		ध्रुव RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		InternalCellChecker(ध्रुव RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		शून्य error(पूर्णाङ्क linenr, std::optional<std::string> error_संदेश = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			यदि (error_संदेश) {
				log_error("%s in internal cell %s\n", *error_संदेश, cell_loc);
			} अन्यथा {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		पूर्णाङ्क param(RTLIL::IdString name)
		{
			auto it = cell->parameters.find(name);
			यदि (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.जोड़ो(name);
			प्रत्यागम it->second.as_int();
		}

		पूर्णाङ्क param_bool(RTLIL::IdString name)
		{
			पूर्णाङ्क v = param(name);
			यदि (GetSize(cell->parameters.at(name)) > 32)
				error(__LINE__);
			यदि (v != 0 && v != 1)
				error(__LINE__);
			प्रत्यागम v;
		}

		पूर्णाङ्क param_bool(RTLIL::IdString name, तार्किक expected)
		{
			पूर्णाङ्क v = param_bool(name);
			यदि (v != expected)
				error(__LINE__);
			प्रत्यागम v;
		}

		शून्य param_bits(RTLIL::IdString name, पूर्णाङ्क width)
		{
			param(name);
			यदि (GetSize(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			प्रत्यागम cell->parameters.at(name).decode_string();
		}

		शून्य port(RTLIL::IdString name, पूर्णाङ्क width)
		{
			auto it = cell->connections_.find(name);
			यदि (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			यदि (GetSize(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.जोड़ो(name);
		}

		शून्य check_expected(तार्किक check_matched_sign = असत्य)
		{
			प्रति (auto &para : cell->parameters)
				यदि (expected_params.गिनती(para.first) == 0)
					error(__LINE__);
			प्रति (auto &conn : cell->connections())
				यदि (expected_ports.गिनती(conn.first) == 0)
					error(__LINE__);

			यदि (check_matched_sign) {
				log_assert(expected_params.गिनती(ID::A_SIGNED) != 0 && expected_params.गिनती(ID::B_SIGNED) != 0);
				तार्किक a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				तार्किक b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				यदि (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		शून्य check()
		{
			यदि (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				प्रत्यागम;

			यदि (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(सत्य);
				प्रत्यागम;
			}

			यदि (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/असत्य);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/असत्य);
				प्रत्यागम;
			}

			यदि (cell->type.in(ID($shift), ID($shiftx))) {
				यदि (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/असत्य);
				} अन्यथा {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/असत्य);
				प्रत्यागम;
			}

			यदि (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(सत्य);
				प्रत्यागम;
			}

			यदि (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($pow));
				प्रत्यागम;
			}

			यदि (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::CI, 1);
				port(ID::BI, 1);
				port(ID::X, param(ID::Y_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				port(ID::CO, param(ID::Y_WIDTH));
				check_expected(सत्य);
				प्रत्यागम;
			}

			यदि (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				प्रत्यागम;
			}

			यदि (cell->type == ID($macc_v2)) {
				यदि (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				यदि (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				यदि (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ध्रुव Const &a_width = cell->getParam(ID::A_WIDTHS);
				ध्रुव Const &b_width = cell->getParam(ID::B_WIDTHS);
				ध्रुव Const &c_width = cell->getParam(ID::C_WIDTHS);
				पूर्णाङ्क a_width_sum = 0, b_width_sum = 0, c_width_sum = 0;
				प्रति (पूर्णाङ्क i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_sum += a_width.extract(16 * i, 16).as_int(असत्य);
					b_width_sum += b_width.extract(16 * i, 16).as_int(असत्य);
				}
				प्रति (पूर्णाङ्क i = 0; i < param(ID::NADDENDS); i++) {
					c_width_sum += c_width.extract(16 * i, 16).as_int(असत्य);
				}
				port(ID::A, a_width_sum);
				port(ID::B, b_width_sum);
				port(ID::C, c_width_sum);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/असत्य);
				प्रत्यागम;
			}

			यदि (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				यदि (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($dffsre)) {
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
				प्रत्यागम;
			}

			यदि (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_मूल्य, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_मूल्य, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.in(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_मूल्य, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_मूल्य, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($aldffe)) {
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
				प्रत्यागम;
			}

			यदि (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_मूल्य, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_BITS);
				param(ID::STATE_अंक);
				param(ID::STATE_अंक_LOG2);
				param(ID::STATE_RST);
				param_bits(ID::STATE_TABLE, param(ID::STATE_BITS) * param(ID::STATE_अंक));
				param(ID::TRANS_अंक);
				param_bits(ID::TRANS_TABLE, param(ID::TRANS_अंक) * (2*param(ID::STATE_अंक_LOG2) + param(ID::CTRL_IN_WIDTH) + param(ID::CTRL_OUT_WIDTH)));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::CTRL_IN, param(ID::CTRL_IN_WIDTH));
				port(ID::CTRL_OUT, param(ID::CTRL_OUT_WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_MASK);
				param(ID::COLLISION_X_MASK);
				param_bool(ID::CE_OVER_SRST);
				param_bits(ID::ARST_मूल्य, param(ID::WIDTH));
				param_bits(ID::SRST_मूल्य, param(ID::WIDTH));
				param_bits(ID::INIT_मूल्य, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::SRST, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($memwr_v2)) {
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
				प्रत्यागम;
			}

			यदि (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::शब्दगण));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::शब्दगण));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($mem)) {
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
				प्रत्यागम;
			}

			यदि (cell->type == ID($mem_v2)) {
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
				param_bits(ID::RD_ARST_मूल्य, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_SRST_मूल्य, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_INIT_मूल्य, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
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
				प्रत्यागम;
			}

			यदि (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.in(ID($प्रतिज्ञा), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.in(ID($specify2), ID($specify3))) {
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
				यदि (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
				}
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($specrule)) {
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
				प्रत्यागम;
			}

			यदि (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				यदि (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
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
				प्रत्यागम;
			}

			यदि (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				यदि (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				प्रत्यागम;
			}

			यदि (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); प्रत्यागम; }

			यदि (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); प्रत्यागम; }

			यदि (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); प्रत्यागम; }

			यदि (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  प्रत्यागम; }

			यदि (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}
			यदि (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}
			यदि (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}
			यदि (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}
			यदि (cell->type.in(ID($निवेश_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
			}
			यदि (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				check_expected();
				प्रत्यागम;
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

शून्य RTLIL::Module::sort()
{
	wires_.sort(sort_by_id_str(design->twines()));
	cells_.sort(sort_by_id_str(design->twines()));
	parameter_default_मूल्यावली.sort(sort_by_id_str(design->twines()));
	memories.sort(sort_by_id_str(design->twines()));
	processes.sort(sort_by_id_str(design->twines()));
	प्रति (auto &it : cells_)
		it.second->sort();
	प्रति (auto &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	प्रति (auto &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#ifndef NDEBUG
स्थिर शून्य check_id_कुंजियाँ(ध्रुव dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	प्रति (auto &it : keyed)
		log_assert(!it.first.empty());
}
#endif

शून्य check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ifndef NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->cells_size(), 1000));
	ध्रुव RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	प्रति (auto &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->width >= 0);
		log_assert(it.second->size >= 0);
		check_id_कुंजियाँ(it.second->attributes);
		memory_strings.जोड़ो(it.second->name.str());
	}

	std::vector<MonotonicFlag> ports_declared(GetSize(module->ports));
	ShardedVector<std::string> memids(subpool);
	subpool.run([const_module, &ports_declared, &memory_strings, &memids](ध्रुव ParallelDispatchThreadPool::RunCtx &ctx) {
		प्रति (पूर्णाङ्क i : ctx.item_range(const_module->cells_size())) {
			auto it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			प्रति (auto &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_कुंजियाँ(it.second->attributes);
			check_id_कुंजियाँ(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			यदि (it.second->has_memid()) {
				log_assert(memory_strings.गिनती(it.second->parameters.at(ID::MEMID).decode_string()));
			} अन्यथा यदि (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.गिनती(memid));
				memids.जोड़ो(ctx, std::move(memid));
			}
			auto cell_mod = const_module->design->module(it.second->name);
			यदि (cell_mod != nullptr) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#ifdef __GNUC__
				#pragma GCC diagnostic धकेलो
				#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				#endif
				log_assert(!it.second->get_blackbox_attribute());
				#ifdef __GNUC__
				#pragma GCC diagnostic निकालो
				#endif
			}
		}

		प्रति (पूर्णाङ्क i : ctx.item_range(const_module->wires_size())) {
			auto it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_कुंजियाँ(it.second->attributes);
			यदि (it.second->port_id) {
				log_assert(GetSize(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_निवेश || it.second->port_output);
				log_assert(it.second->port_id <= GetSize(ports_declared));
				तार्किक previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == असत्य);
			} अन्यथा
				log_assert(!it.second->port_निवेश && !it.second->port_output);
		}
	});
	प्रति (ध्रुव MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == सत्य);
	pool<std::string> memids_pool;
	प्रति (std::string &memid : memids)
		log_assert(memids_pool.जोड़ो(memid).second);

	प्रति (auto &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->मूल_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->मूल_case};
		प्रति (size_t i = 0; i < all_cases.size(); i++) {
			प्रति (auto &switch_it : all_cases[i]->switches) {
				प्रति (auto &case_it : switch_it->cases) {
					प्रति (auto &compare_it : case_it->compare) {
						log_assert(switch_it->सङ्केत.size() == compare_it.size());
					}
					all_cases.धकेलो_back(case_it);
				}
			}
		}
		प्रति (auto &sync_it : it.second->syncs) {
			विकल्प (sync_it->type) {
				पक्ष RTLIL::SyncType::ST0:
				पक्ष RTLIL::SyncType::ST1:
				पक्ष RTLIL::SyncType::STp:
				पक्ष RTLIL::SyncType::STn:
				पक्ष RTLIL::SyncType::STe:
					log_assert(!sync_it->सङ्केत.empty());
					भङ्ग;
				पक्ष RTLIL::SyncType::STa:
				पक्ष RTLIL::SyncType::STg:
				पक्ष RTLIL::SyncType::STi:
					log_assert(sync_it->सङ्केत.empty());
					भङ्ग;
			}
		}
	}

	प्रति (auto &it : module->connections_) {
		log_assert(it.first.size() == it.second.size());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_कुंजियाँ(module->attributes);
#endif
}

शून्य RTLIL::Module::check()
{
	पूर्णाङ्क pool_size = ThreadPool::work_pool_size(0, cells_size(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_size);
	check_module(एतत्, thread_pool);
}

शून्य RTLIL::Module::optimize()
{
}

शून्य RTLIL::Module::cloneInto(RTLIL::Module *new_mod) ध्रुव
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	प्रति (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_मूल्यावली.clear();
	प्रति (auto &it : parameter_default_मूल्यावली)
		new_mod->parameter_default_मूल्यावली[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	प्रति (auto &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	प्रति (auto it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	प्रति (auto it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	प्रति (auto it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	प्रति (auto it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addProcess(dst_id, it->second);
	}

	संरचना RewriteSigSpecWorker
	{
		ध्रुव dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ध्रुव RTLIL::Module *new_mod;
		शून्य operator()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([एतत्](RTLIL::Wire *&wire) {
				auto it = wire_map.find(wire);
				यदि (it != wire_map.end())
					wire = it->second;
				अन्यथा
					log_assert(wire->module == new_mod);
			});
		}
	};

	RewriteSigSpecWorker rewriteSigSpecWorker{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewriteSigSpecWorker);
	new_mod->fixup_ports();
}

RTLIL::Module *RTLIL::Module::clone() ध्रुव
{
	RTLIL::Module *new_mod = नूतन RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	प्रत्यागम new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) ध्रुव
{
	RTLIL::Module *new_mod = नूतन RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->add(new_mod);
	प्रत्यागम new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) ध्रुव
{
	RTLIL::Module *new_mod = नूतन RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	प्रत्यागम new_mod;
}

तार्किक RTLIL::Module::has_memories() ध्रुव
{
	प्रत्यागम !memories.empty();
}

तार्किक RTLIL::Module::has_processes() ध्रुव
{
	प्रत्यागम !processes.empty();
}

तार्किक RTLIL::Module::has_memories_warn() ध्रुव
{
	यदि (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", एतत्);
	प्रत्यागम !memories.empty();
}

तार्किक RTLIL::Module::has_processes_warn() ध्रुव
{
	यदि (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", एतत्);
	प्रत्यागम !processes.empty();
}

तार्किक RTLIL::Module::is_selected() ध्रुव
{
	प्रत्यागम design->selected_module(एतत्->name);
}

तार्किक RTLIL::Module::is_selected_whole() ध्रुव
{
	प्रत्यागम design->selected_whole_module(एतत्->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() ध्रुव
{
	std::vector<RTLIL::Wire*> result;
	result.reserve(wires_.size());
	प्रति (auto &it : wires_)
		यदि (design->selected(एतत्, it.second))
			result.धकेलो_back(it.second);
	प्रत्यागम result;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() ध्रुव
{
	std::vector<RTLIL::Cell*> result;
	result.reserve(cells_.size());
	प्रति (auto &it : cells_)
		यदि (design->selected(एतत्, it.second))
			result.धकेलो_back(it.second);
	प्रत्यागम result;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() ध्रुव
{
	std::vector<RTLIL::Memory*> result;
	result.reserve(memories.size());
	प्रति (auto &it : memories)
		यदि (design->selected(एतत्, it.second))
			result.धकेलो_back(it.second);
	प्रत्यागम result;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() ध्रुव
{
	std::vector<RTLIL::Process*> result;
	result.reserve(processes.size());
	प्रति (auto &it : processes)
		यदि (design->selected(एतत्, it.second))
			result.धकेलो_back(it.second);
	प्रत्यागम result;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() ध्रुव
{
	std::vector<RTLIL::SelectedMember> result;
	प्रति (auto cell : selected_cells())
		result.धकेलो_back({cell, cell->name});
	प्रति (auto memory : selected_memories())
		result.धकेलो_back({memory, memory->name});
	प्रति (auto wire : selected_wires())
		result.धकेलो_back({wire, wire->name});
	प्रति (auto process : selected_processes())
		result.धकेलो_back({process, process->name});
	प्रत्यागम result;
}

शून्य RTLIL::Module::add(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(गिनती_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = एतत्;
}

शून्य RTLIL::Module::add(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(गिनती_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = एतत्;
}

शून्य RTLIL::Module::add(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(गिनती_id(process->name) == 0);
	processes[process->name] = process;
	process->module = एतत्;
}

शून्य RTLIL::Module::remove(ध्रुव pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	यदि (wires.empty())
		प्रत्यागम;

	संरचना DeleteWireWorker
	{
		RTLIL::Module *module;
		ध्रुव pool<RTLIL::Wire*> *wires_p;

		शून्य operator()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([एतत्](RTLIL::Wire *&wire) {
				यदि (wires_p->गिनती(wire))
					wire = module->addWire(stringf("$delete_wire$%d", (पूर्णाङ्क)autoidx++), wire->width);
			});
		}

		शून्य operator()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	DeleteWireWorker delete_wire_worker;
	delete_wire_worker.module = एतत्;
	delete_wire_worker.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_worker);

	यदि (design->flagBufferedNormalized) {
		प्रति (auto wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_index.erase(wire);
		}
	}

	प्रति (auto &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.गिनती(id) != 0);
		wires_.erase(id);
		delete it;
	}
}

शून्य RTLIL::Module::remove(RTLIL::Cell *cell)
{
	यावत् (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.गिनती(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	यदि (design && design->flagBufferedNormalized && buf_norm_cell_queue.गिनती(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.जोड़ो(cell);
	} अन्यथा {
		delete cell;
	}
}

शून्य RTLIL::Module::remove(RTLIL::Memory *memory)
{
	log_assert(memories.गिनती(memory->name) != 0);
	memories.erase(memory->name);
	delete memory;
}

शून्य RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.गिनती(process->name) != 0);
	processes.erase(process->name);
	delete process;
}

शून्य RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	add(wire);
}

शून्य RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	add(cell);
}

शून्य RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	यदि (old_id != IdString::Null && wires_.गिनती(old_id))
		rename(wires_.at(old_id), new_name);
	अन्यथा यदि (old_id != IdString::Null && cells_.गिनती(old_id))
		rename(cells_.at(old_id), new_name);
	अन्यथा
		log_abort();
}

शून्य RTLIL::Module::swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2)
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

शून्य RTLIL::Module::swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	पूर्णाङ्क index = 0;
	प्रत्यागम uniquify(name, index);
}

IdString RTLIL::Module::uniquify(IdString name, पूर्णाङ्क &index)
{
	यदि (index == 0) {
		यदि (गिनती_id(name) == 0)
			प्रत्यागम name;
		index++;
	}

	यावत् (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", index)});
		यदि (गिनती_id(new_name) == 0)
			प्रत्यागम new_name;
		index++;
	}
}

स्थिर तार्किक fixup_ports_compare(ध्रुव RTLIL::Wire *a, ध्रुव RTLIL::Wire *b)
{
	यदि (a->port_id && !b->port_id)
		प्रत्यागम सत्य;
	यदि (!a->port_id && b->port_id)
		प्रत्यागम असत्य;

	यदि (a->port_id == b->port_id)
		प्रत्यागम a->name < b->name;
	प्रत्यागम a->port_id < b->port_id;
}

शून्य RTLIL::Module::connect(ध्रुव RTLIL::SigSig &conn)
{
	प्रति (auto mon : monitors)
		mon->notify_connect(एतत्, conn);

	यदि (design)
		प्रति (auto mon : design->monitors)
			mon->notify_connect(एतत्, conn);

	// ignore all attempts to assign constants to other constants
	यदि (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		प्रति (पूर्णाङ्क i = 0; i < GetSize(conn.first); i++)
			यदि (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		यदि (GetSize(new_conn.first))
			connect(new_conn);
		प्रत्यागम;
	}

	यदि (yosys_xtrace) {
		log("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", एतत्, log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(GetSize(conn.first) == GetSize(conn.second));
	connections_.धकेलो_back(conn);
}

शून्य RTLIL::Module::connect(ध्रुव RTLIL::SigSpec &lhs, ध्रुव RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

शून्य RTLIL::Module::new_connections(ध्रुव std::vector<RTLIL::SigSig> &new_conn)
{
	प्रति (auto mon : monitors)
		mon->notify_connect(एतत्, new_conn);

	यदि (design)
		प्रति (auto mon : design->monitors)
			mon->notify_connect(एतत्, new_conn);

	यदि (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", एतत्);
		प्रति (auto &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ध्रुव std::vector<RTLIL::SigSig> &RTLIL::Module::connections() ध्रुव
{
	प्रत्यागम connections_;
}

शून्य RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	प्रति (auto &w : wires_)
		यदि (w.second->port_निवेश || w.second->port_output)
			all_ports.धकेलो_back(w.second);
		अन्यथा
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	यदि (design && design->flagBufferedNormalized) {
		प्रति (auto &w : wires_)
			यदि (w.second->driverCell_ && w.second->driverCell_->type == ID($निवेश_port))
				buf_norm_wire_queue.जोड़ो(w.second);

		buf_norm_wire_queue.जोड़ो(all_ports.begin(), all_ports.end());
	}

	ports.clear();
	प्रति (size_t i = 0; i < all_ports.size(); i++) {
		ports.धकेलो_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
	}
}

शून्य RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		ध्रुव dict<IdString, RTLIL::Const> &src,
		ध्रुव RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	यदि (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		प्रत्यागम;
	}
	dst.clear();
	प्रति (पूर्णाङ्क i = GetSize(src) - 1; i >= 0; i--) {
		auto &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::addWire(RTLIL::IdString name, पूर्णाङ्क width)
{
	log_assert(design);
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	RTLIL::Wire *wire = नूतन RTLIL::Wire;
	wire->width = width;
	wire->name = name;
	add(wire);
	प्रत्यागम wire;
}

RTLIL::Wire *RTLIL::Module::addWire(IdString name, ध्रुव RTLIL::Wire *other)
{
	RTLIL::Wire *wire = addWire(name);
	wire->width = other->width;
	wire->start_offset = other->start_offset;
	wire->port_id = other->port_id;
	wire->port_निवेश = other->port_निवेश;
	wire->port_output = other->port_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ध्रुव RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
		RTLIL::copy_attr_dict(wire->attributes, other->attributes, src_design, एतत्->design);
	}
	प्रत्यागम wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = नूतन RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	प्रत्यागम cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, ध्रुव RTLIL::Cell *other)
{
	ध्रुव RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	तार्किक cross_pool = src_design && एतत्->design && src_design != एतत्->design;

	IdString type = other->type.ref();
	यदि (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, एतत्->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, एतत्->design);

	यदि (!cross_pool)
		cell->connections_ = other->connections_;
	अन्यथा
		प्रति (पूर्णाङ्क i = GetSize(other->connections_) - 1; i >= 0; i--) {
			auto &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	प्रत्यागम cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = नूतन RTLIL::Memory;
	mem->module = एतत्;
	mem->name = name;
	memories[name] = mem;
	प्रत्यागम mem;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name, ध्रुव RTLIL::Memory *other)
{
	log_assert(design);
	RTLIL::Memory *mem = नूतन RTLIL::Memory;
	mem->module = एतत्;
	mem->name = name;
	mem->width = other->width;
	mem->start_offset = other->start_offset;
	mem->size = other->size;
	RTLIL::copy_attr_dict(mem->attributes, other->attributes,
			other->module ? other->module->design : nullptr, design);
	memories[name] = mem;
	प्रत्यागम mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = नूतन RTLIL::Process;
	proc->name = name;
	add(proc);
	प्रत्यागम proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, ध्रुव RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ध्रुव RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	यदि (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		प्रति (auto *समकाल : proc->syncs)
			प्रति (auto &mwa : समकाल->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	प्रत्यागम proc;
}

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		प्रत्यागम cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		प्रत्यागम sig_y;                                       \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, तार्किक /* is_signed */, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::WIDTH] = sig_a.size();         \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		प्रत्यागम cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		प्रत्यागम sig_y;                                       \
	}
DEF_METHOD(Buf,     sig_a.size(), ID($buf))
DEF_METHOD(Barrier, sig_a.size(), ID($barrier))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, तार्किक is_signed, ध्रुव std::string &src) { \
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
		प्रत्यागम cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		प्रत्यागम sig_y;                                            \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = असत्य;             \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		प्रत्यागम cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		प्रत्यागम sig_y;                                            \
	}
DEF_METHOD(Shl,      sig_a.size(), ID($shl))
DEF_METHOD(Shr,      sig_a.size(), ID($shr))
DEF_METHOD(Sshl,     sig_a.size(), ID($sshl))
DEF_METHOD(Sshr,     sig_a.size(), ID($sshr))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = असत्य;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		प्रत्यागम cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		प्रत्यागम sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.size(), ID($shiftx))
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		यदि (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.size();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		प्रत्यागम cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		प्रत्यागम sig_y;                                             \
	}
DEF_METHOD(Mux,      ID($mux),        0)
DEF_METHOD(Bwmux,    ID($bwmux),      0)
DEF_METHOD(Pmux,     ID($pmux),       1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = _demux ? sig_a.size() : sig_y.size(); \
		cell->parameters[ID::S_WIDTH] = sig_s.size();             \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		प्रत्यागम cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.size() << sig_s.size() : sig_a.size() >> sig_s.size()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		प्रत्यागम sig_y;                                             \
	}
DEF_METHOD(Bmux,     ID($bmux),       0)
DEF_METHOD(Demux,    ID($demux),      1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		प्रत्यागम cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		प्रत्यागम sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#undef DEF_METHOD

#define DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		प्रत्यागम cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		प्रत्यागम sig2;                                      \
	}
#define DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव RTLIL::SigBit &sig3, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		प्रत्यागम cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		प्रत्यागम sig3;                                      \
	}
#define DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव RTLIL::SigBit &sig3, ध्रुव RTLIL::SigBit &sig4, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		प्रत्यागम cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव RTLIL::SigBit &sig3, ध्रुव std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		प्रत्यागम sig4;                                      \
	}
#define DEF_METHOD_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव RTLIL::SigBit &sig3, ध्रुव RTLIL::SigBit &sig4, ध्रुव RTLIL::SigBit &sig5, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->setPort("\\" #_P5, sig5);                   \
		cell->set_src_attribute(src);                     \
		प्रत्यागम cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव RTLIL::SigBit &sig3, ध्रुव RTLIL::SigBit &sig4, ध्रुव std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		प्रत्यागम sig5;                                           \
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

RTLIL::Cell* RTLIL::Module::addPow(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, तार्किक a_signed, तार्किक b_signed, ध्रुव std::string &src)
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
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addFa(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव RTLIL::SigSpec &sig_x, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fa));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::C, sig_c);
	cell->setPort(ID::X, sig_x);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addSlice(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, RTLIL::Const offset, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($slice));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::Y_WIDTH] = sig_y.size();
	cell->parameters[ID::OFFSET] = offset;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addConcat(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($concat));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::B_WIDTH] = sig_b.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, RTLIL::Const lut, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($प्रतिज्ञा));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addSr(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr, ध्रुव RTLIL::SigSpec &sig_q, तार्किक set_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sr));
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addDff(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addDffe(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक en_polarity, ध्रुव std::string &src)
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
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addDffsr(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक set_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
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
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addDffsre(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक en_polarity, तार्किक set_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
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
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addAdff(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_मूल्य, तार्किक clk_polarity, तार्किक arst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_मूल्य] = arst_मूल्य;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addAdffe(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_मूल्य, तार्किक clk_polarity, तार्किक en_polarity, तार्किक arst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_मूल्य] = arst_मूल्य;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addAldff(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		ध्रुव RTLIL::SigSpec &sig_ad, तार्किक clk_polarity, तार्किक aload_polarity, ध्रुव std::string &src)
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
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addAldffe(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		ध्रुव RTLIL::SigSpec &sig_ad, तार्किक clk_polarity, तार्किक en_polarity, तार्किक aload_polarity, ध्रुव std::string &src)
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
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addSdff(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_मूल्य, तार्किक clk_polarity, तार्किक srst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_मूल्य] = srst_मूल्य;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addSdffe(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_मूल्य, तार्किक clk_polarity, तार्किक en_polarity, तार्किक srst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_मूल्य] = srst_मूल्य;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addSdffce(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_मूल्य, तार्किक clk_polarity, तार्किक en_polarity, तार्किक srst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_मूल्य] = srst_मूल्य;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addDlatch(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक en_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatch(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_मूल्य, तार्किक en_polarity, तार्किक arst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_मूल्य] = arst_मूल्य;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsr(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक en_polarity, तार्किक set_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
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
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr,
		ध्रुव RTLIL::SigSpec &sig_q, तार्किक set_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक en_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addDffsrGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक set_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addDffsreGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक en_polarity, तार्किक set_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addAdffGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		तार्किक arst_मूल्य, तार्किक clk_polarity, तार्किक arst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_मूल्य ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addAdffeGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		तार्किक arst_मूल्य, तार्किक clk_polarity, तार्किक en_polarity, तार्किक arst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_मूल्य ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addAldffGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		ध्रुव RTLIL::SigSpec &sig_ad, तार्किक clk_polarity, तार्किक aload_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addAldffeGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		ध्रुव RTLIL::SigSpec &sig_ad, तार्किक clk_polarity, तार्किक en_polarity, तार्किक aload_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addSdffGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		तार्किक srst_मूल्य, तार्किक clk_polarity, तार्किक srst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_मूल्य ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addSdffeGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		तार्किक srst_मूल्य, तार्किक clk_polarity, तार्किक en_polarity, तार्किक srst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_मूल्य ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addSdffceGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		तार्किक srst_मूल्य, तार्किक clk_polarity, तार्किक en_polarity, तार्किक srst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_मूल्य ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक en_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatchGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		तार्किक arst_मूल्य, तार्किक en_polarity, तार्किक arst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_मूल्य ? '1' : '0'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsrGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक en_polarity, तार्किक set_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, पूर्णाङ्क width, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, पूर्णाङ्क width, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, पूर्णाङ्क width, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, पूर्णाङ्क width, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::SigSpec RTLIL::Module::SetTag(RTLIL::IdString name, ध्रुव std::string &tag, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव std::string &src)
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
	प्रत्यागम sig;
}

RTLIL::Cell* RTLIL::Module::addSetTag(RTLIL::IdString name, ध्रुव std::string &tag, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src)
{
	Cell *cell = addCell(name, ID($set_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::SigSpec RTLIL::Module::GetTag(RTLIL::IdString name, ध्रुव std::string &tag, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($get_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::Cell* RTLIL::Module::addOverwriteTag(RTLIL::IdString name, ध्रुव std::string &tag, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($overwrite_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->set_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::SigSpec RTLIL::Module::OriginalTag(RTLIL::IdString name, ध्रुव std::string &tag, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($original_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_e, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.size());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.size();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	प्रत्यागम sig;
}

std::string RTLIL::Module::to_rtlil_str() ध्रुव
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(एतत्), design, असत्य, सत्य, असत्य, RTLIL_BACKEND::DumpMode::Readable);
	प्रत्यागम f.str();
}

RTLIL::Wire::Wire()
{
	स्थिर unsigned पूर्णाङ्क hashidx_गिनती = 123456789;
	hashidx_गिनती = mkhash_xorshift(hashidx_गिनती);
	hashidx_ = hashidx_गिनती;

	module = nullptr;
	width = 1;
	start_offset = 0;
	port_id = 0;
	port_निवेश = असत्य;
	port_output = असत्य;
	upto = असत्य;
	is_signed = असत्य;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->जोड़ो(std::pair<unsigned पूर्णाङ्क, RTLIL::Wire*>(hashidx_, एतत्));
#endif
}

RTLIL::Wire::~Wire()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#endif
}

std::string RTLIL::Wire::to_rtlil_str() ध्रुव
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", एतत्, design(), RTLIL_BACKEND::DumpMode::Readable);
	प्रत्यागम f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
स्थिर std::map<unsigned पूर्णाङ्क, RTLIL::Wire*> all_wires;
std::map<unsigned पूर्णाङ्क, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(शून्य)
{
	प्रत्यागम &all_wires;
}
#endif

RTLIL::Memory::Memory()
{
	स्थिर unsigned पूर्णाङ्क hashidx_गिनती = 123456789;
	hashidx_गिनती = mkhash_xorshift(hashidx_गिनती);
	hashidx_ = hashidx_गिनती;

	width = 1;
	start_offset = 0;
	size = 0;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->जोड़ो(std::pair<unsigned पूर्णाङ्क, RTLIL::Memory*>(hashidx_, एतत्));
#endif
}

std::string RTLIL::Memory::to_rtlil_str() ध्रुव
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", एतत्, design(), RTLIL_BACKEND::DumpMode::Readable);
	प्रत्यागम f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	स्थिर unsigned पूर्णाङ्क hashidx_गिनती = 123456789;
	hashidx_गिनती = mkhash_xorshift(hashidx_गिनती);
	hashidx_ = hashidx_गिनती;
}

std::string RTLIL::Process::to_rtlil_str() ध्रुव
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", एतत्, design(), RTLIL_BACKEND::DumpMode::Readable);
	प्रत्यागम f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	स्थिर unsigned पूर्णाङ्क hashidx_गिनती = 123456789;
	hashidx_गिनती = mkhash_xorshift(hashidx_गिनती);
	hashidx_ = hashidx_गिनती;

	// log("#memtrace# %p\n", this);
	memhasher();

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->जोड़ो(std::pair<unsigned पूर्णाङ्क, RTLIL::Cell*>(hashidx_, एतत्));
#endif
}

RTLIL::Cell::~Cell()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#endif
}

std::string RTLIL::Cell::to_rtlil_str() ध्रुव
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", एतत्, design(), RTLIL_BACKEND::DumpMode::Readable);
	प्रत्यागम f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
स्थिर std::map<unsigned पूर्णाङ्क, RTLIL::Cell*> all_cells;
std::map<unsigned पूर्णाङ्क, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(शून्य)
{
	प्रत्यागम &all_cells;
}
#endif

तार्किक RTLIL::Cell::hasPort(RTLIL::IdString portname) ध्रुव
{
	प्रत्यागम connections_.गिनती(portname) != 0;
}

// bufnorm

ध्रुव RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) ध्रुव
{
	प्रत्यागम connections_.at(portname);
}

ध्रुव dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() ध्रुव
{
	प्रत्यागम connections_;
}

तार्किक RTLIL::Cell::known() ध्रुव
{
	यदि (yosys_celltypes.cell_known(type))
		प्रत्यागम सत्य;
	यदि (module && module->design && module->design->module(type))
		प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}

तार्किक RTLIL::Cell::निवेश(IdString portname) ध्रुव
{
	यदि (yosys_celltypes.cell_known(type))
		प्रत्यागम yosys_celltypes.cell_निवेश(type.ref(), portname);
	यदि (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		प्रत्यागम w && w->port_निवेश;
	}
	प्रत्यागम असत्य;
}

तार्किक RTLIL::Cell::output(RTLIL::IdString portname) ध्रुव
{
	यदि (yosys_celltypes.cell_known(type))
		प्रत्यागम yosys_celltypes.cell_output(type, portname);
	यदि (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		प्रत्यागम w && w->port_output;
	}
	प्रत्यागम असत्य;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) ध्रुव
{
	यदि (yosys_celltypes.cell_known(type.ref()))
		प्रत्यागम yosys_celltypes.cell_port_dir(type, portname);
	यदि (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		यदि (m == nullptr)
			प्रत्यागम PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		यदि (w == nullptr)
			प्रत्यागम PortDir::PD_UNKNOWN;
		प्रत्यागम PortDir(w->port_निवेश + w->port_output * 2);
	}
	प्रत्यागम PortDir::PD_UNKNOWN;
}

तार्किक RTLIL::Cell::hasParam(RTLIL::IdString paramname) ध्रुव
{
	प्रत्यागम parameters.गिनती(paramname) != 0;
}

शून्य RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

शून्य RTLIL::Cell::setParam(RTLIL::IdString paramname, RTLIL::Const मूल्य)
{
	parameters[paramname] = std::move(मूल्य);
}

ध्रुव RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ध्रुव
{
	ध्रुव auto &it = parameters.find(paramname);
	यदि (it != parameters.end())
		प्रत्यागम it->second;
	यदि (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		यदि (m)
			प्रत्यागम m->parameter_default_मूल्यावली.at(paramname);
	}
	क्षिप std::out_of_range("Cell::getParam()");
}

शून्य RTLIL::Cell::sort()
{
	connections_.sort(sort_by_id_str(module->twines()));
	parameters.sort(sort_by_id_str(module->twines()));
	attributes.sort(sort_by_id_str(module->twines()));
}

शून्य RTLIL::Cell::check()
{
#ifndef NDEBUG
	InternalCellChecker checker(रिक्त, एतत्);
	checker.check();
#endif
}

शून्य RTLIL::Cell::fixup_parameters(तार्किक set_a_signed, तार्किक set_b_signed)
{
	यदि (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		प्रत्यागम;

	यदि (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		यदि (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		प्रत्यागम;
	}

	यदि (type == ID($demux)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		प्रत्यागम;
	}

	यदि (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		प्रत्यागम;
	}

	यदि (type == ID($fa)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		प्रत्यागम;
	}

	यदि (type == ID($lcu)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::CO]);
		प्रत्यागम;
	}

	यदि (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);
		प्रत्यागम;
	}

	तार्किक signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	यदि (connections_.गिनती(ID::A)) {
		यदि (signedness_ab) {
			यदि (set_a_signed)
				parameters[ID::A_SIGNED] = सत्य;
			अन्यथा यदि (parameters.गिनती(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = असत्य;
		}
		parameters[ID::A_WIDTH] = GetSize(connections_[ID::A]);
	}

	यदि (connections_.गिनती(ID::B)) {
		यदि (signedness_ab) {
			यदि (set_b_signed)
				parameters[ID::B_SIGNED] = सत्य;
			अन्यथा यदि (parameters.गिनती(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = असत्य;
		}
		parameters[ID::B_WIDTH] = GetSize(connections_[ID::B]);
	}

	यदि (connections_.गिनती(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);

	यदि (connections_.गिनती(ID::Q))
		parameters[ID::WIDTH] = GetSize(connections_[ID::Q]);

	check();
}

तार्किक RTLIL::Cell::has_keep_attr() ध्रुव {
	प्रत्यागम get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

तार्किक RTLIL::Cell::has_memid() ध्रुव
{
	प्रत्यागम type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

तार्किक RTLIL::Cell::is_mem_cell() ध्रुव
{
	प्रत्यागम type.in(ID($mem), ID($mem_v2)) || has_memid();
}

तार्किक RTLIL::Cell::is_builtin_ff() ध्रुव {
	प्रत्यागम StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ध्रुव RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	यदि (wire == रिक्त)
		data = {bit.data};
	अन्यथा
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(पूर्णाङ्क offset, पूर्णाङ्क length) ध्रुव
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= width);
	RTLIL::SigChunk ret;
	यदि (wire) {
		ret.wire = wire;
		ret.offset = एतत्->offset + offset;
		ret.width = length;
	} अन्यथा {
		प्रति (पूर्णाङ्क i = 0; i < length; i++)
			ret.data.धकेलो_back(data[offset+i]);
		ret.width = length;
	}
	प्रत्यागम ret;
}

RTLIL::SigBit RTLIL::SigChunk::operator[](पूर्णाङ्क offset) ध्रुव
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	यदि (wire) {
		ret.wire = wire;
		ret.offset = एतत्->offset + offset;
	} अन्यथा {
		ret.data = data[offset];
	}
	प्रत्यागम ret;
}

तार्किक RTLIL::SigChunk::operator <(ध्रुव RTLIL::SigChunk &other) ध्रुव
{
	यदि (wire && other.wire)
		यदि (wire->name != other.wire->name)
			प्रत्यागम wire->name < other.wire->name;

	यदि (wire != other.wire)
		प्रत्यागम wire < other.wire;

	यदि (offset != other.offset)
		प्रत्यागम offset < other.offset;

	यदि (width != other.width)
		प्रत्यागम width < other.width;

	प्रत्यागम data < other.data;
}

तार्किक RTLIL::SigChunk::operator ==(ध्रुव RTLIL::SigChunk &other) ध्रुव
{
	प्रत्यागम wire == other.wire && width == other.width && offset == other.offset && data == other.data;
}

तार्किक RTLIL::SigChunk::operator !=(ध्रुव RTLIL::SigChunk &other) ध्रुव
{
	यदि (*एतत् == other)
		प्रत्यागम असत्य;
	प्रत्यागम सत्य;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.size() > 0);
	auto ie = parts.begin();
	auto it = ie + parts.size() - 1;
	यावत् (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ध्रुव RTLIL::Const &मूल्य)
{
	यदि (GetSize(मूल्य) != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(मूल्य);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&मूल्य)
{
	यदि (GetSize(मूल्य) != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(मूल्य);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव RTLIL::SigChunk &chunk)
{
	यदि (chunk.width != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(chunk);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	यदि (chunk.width != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(chunk);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	यदि (wire->width != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(wire);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, पूर्णाङ्क offset, पूर्णाङ्क width)
{
	यदि (width != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव std::string &str)
{
	यदि (str.size() != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(str);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(पूर्णाङ्क val, पूर्णाङ्क width)
{
	यदि (width != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(val, width);
	} अन्यथा
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, पूर्णाङ्क width)
{
	यदि (width != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(bit, width);
	} अन्यथा
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव RTLIL::SigBit &bit, पूर्णाङ्क width)
{
	यदि (width != 0) {
		यदि (bit.wire == रिक्त) {
			rep_ = CHUNK;
			नूतन (&chunk_) RTLIL::SigChunk(bit.data, width);
		} अन्यथा यदि (width == 1) {
			rep_ = CHUNK;
			नूतन (&chunk_) RTLIL::SigChunk(bit);
		} अन्यथा {
			init_empty_bits();
			bits_.reserve(width);
			प्रति (पूर्णाङ्क i = 0; i < width; i++)
				bits_.धकेलो_back(bit);
		}
	} अन्यथा
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	प्रति (ध्रुव auto &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	प्रति (ध्रुव auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	प्रति (ध्रुव auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	प्रति (ध्रुव auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(तार्किक bit)
{
	rep_ = CHUNK;
	नूतन (&chunk_) RTLIL::SigChunk(bit ? RTLIL::S1 : RTLIL::S0);
	check();
}

शून्य RTLIL::SigSpec::Chunks::const_iterator::next_chunk_bits()
{
	पूर्णाङ्क bits_size = GetSize(spec.bits_);
	यदि (bit_index >= bits_size)
		प्रत्यागम;
	पूर्णाङ्क i = bit_index;
	ध्रुव SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.data.clear();
	यदि (bit.is_wire()) {
		chunk.offset = bit.offset;
		यावत् (i < bits_size && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_index)
			++i;
	} अन्यथा {
		chunk.offset = 0;
		chunk.data.धकेलो_back(bit.data);
		यावत् (i < bits_size && !spec.bits_[i].is_wire()) {
			chunk.data.धकेलो_back(spec.bits_[i].data);
			++i;
		}
	}
	chunk.width = i - bit_index;
}

शून्य RTLIL::SigSpec::unpack()
{
	यदि (rep_ == BITS)
		प्रत्यागम;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	प्रति (पूर्णाङ्क i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	नूतन (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

शून्य RTLIL::SigSpec::try_repack()
{
	यदि (rep_ != BITS)
		प्रत्यागम;

	पूर्णाङ्क bits_size = GetSize(bits_);
	यदि (bits_size == 0)
		प्रत्यागम;
	यदि (bits_[0].is_wire()) {
		प्रति (पूर्णाङ्क i = 1; i < bits_size; i++)
			यदि (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				प्रत्यागम;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_size);
		bits_.~vector();
		rep_ = CHUNK;
		नूतन (&chunk_) SigChunk(std::move(chunk));
		प्रत्यागम;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_size);
	bits.धकेलो_back(bits_[0].data);
	प्रति (पूर्णाङ्क i = 1; i < bits_size; i++) {
		यदि (bits_[i].is_wire())
			प्रत्यागम;
		bits.धकेलो_back(bits_[i].data);
	}
	bits_.~vector();
	rep_ = CHUNK;
	नूतन (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ध्रुव
{
	Hasher h;
	प्रति (auto &c : chunks())
		यदि (c.wire == रिक्त) {
			प्रति (auto &v : c.data)
				h.eat(v);
		} अन्यथा {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t result = h.yield();
	यदि (result == 0)
		result = 1;
	hash_.set(result);
	प्रत्यागम result;
}

शून्य RTLIL::SigSpec::sort()
{
	unpack();
	std::sort(bits_.begin(), bits_.end());
	hash_.clear();
	try_repack();
}

शून्य RTLIL::SigSpec::sort_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::vector<SigBit> unique_bits = bits_;
	std::sort(unique_bits.begin(), unique_bits.end());
	auto last = std::unique(unique_bits.begin(), unique_bits.end());
	unique_bits.erase(last, unique_bits.end());

	*एतत् = unique_bits;
	hash_.clear();
	try_repack();
}

शून्य RTLIL::SigSpec::replace(ध्रुव RTLIL::SigSpec &pattern, ध्रुव RTLIL::SigSpec &with)
{
	replace(pattern, with, एतत्);
}

शून्य RTLIL::SigSpec::replace(ध्रुव RTLIL::SigSpec &pattern, ध्रुव RTLIL::SigSpec &with, RTLIL::SigSpec *other) ध्रुव
{
	log_assert(other != रिक्त);
	log_assert(size() == other->size());
	log_assert(pattern.size() == with.size());

	dict<RTLIL::SigBit, पूर्णाङ्क> pattern_to_with;
	पूर्णाङ्क pattern_size = pattern.size();
	प्रति (पूर्णाङ्क i = 0; i < pattern_size; i++) {
		SigBit pattern_bit = pattern[i];
		यदि (pattern_bit.wire != रिक्त) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	पूर्णाङ्क this_size = size();
	तार्किक other_modified = असत्य;
	प्रति (पूर्णाङ्क j = 0; j < this_size; j++) {
		auto it = pattern_to_with.find((*एतत्)[j]);
		यदि (it != pattern_to_with.end()) {
			other_modified = सत्य;
			(*other)[j] = with[it->second];
		}
	}

	यदि (other_modified)
		other->try_repack();
	other->check();
}

शून्य RTLIL::SigSpec::replace(ध्रुव dict<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, एतत्);
}

शून्य RTLIL::SigSpec::replace(ध्रुव dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ध्रुव
{
	log_assert(other != रिक्त);
	log_assert(size() == other->size());

	यदि (rules.empty()) प्रत्यागम;

	पूर्णाङ्क this_size = size();
	तार्किक other_modified = असत्य;
	प्रति (पूर्णाङ्क i = 0; i < this_size; i++) {
		auto it = rules.find((*एतत्)[i]);
		यदि (it != rules.end()) {
			other_modified = सत्य;
			(*other)[i] = it->second;
		}
	}

	यदि (other_modified)
		other->try_repack();
	other->check();
}

शून्य RTLIL::SigSpec::replace(ध्रुव std::map<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, एतत्);
}

शून्य RTLIL::SigSpec::replace(ध्रुव std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ध्रुव
{
	log_assert(other != रिक्त);
	log_assert(size() == other->size());

	यदि (rules.empty()) प्रत्यागम;

	पूर्णाङ्क this_size = size();
	तार्किक other_modified = असत्य;
	प्रति (पूर्णाङ्क i = 0; i < this_size; i++) {
		auto it = rules.find((*एतत्)[i]);
		यदि (it != rules.end()) {
			other_modified = सत्य;
			(*other)[i] = it->second;
		}
	}

	यदि (other_modified)
		other->try_repack();
	other->check();
}

शून्य RTLIL::SigSpec::remove(ध्रुव RTLIL::SigSpec &pattern)
{
	remove2(pattern, रिक्त);
}

शून्य RTLIL::SigSpec::remove(ध्रुव RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) ध्रुव
{
	RTLIL::SigSpec tmp = *एतत्;
	tmp.remove2(pattern, other);
}

शून्य RTLIL::SigSpec::remove2(ध्रुव RTLIL::SigSpec &pattern, RTLIL::SigSpec *other)
{
	unpack();
	यदि (other != रिक्त) {
		log_assert(size() == other->size());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.size());
	प्रति (auto &bit : pattern)
		यदि (bit.wire != रिक्त)
			pattern_bits.जोड़ो(bit);

	// Compact in-place to avoid O(n^2) erase operations
	size_t write_idx = 0;
	प्रति (size_t read_idx = 0; read_idx < bits_.size(); read_idx++)
	{
		यदि (!(bits_[read_idx].wire != रिक्त && pattern_bits.गिनती(bits_[read_idx]))) {
			यदि (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				यदि (other != रिक्त)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	तार्किक modified = (write_idx < bits_.size());
	यदि (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	यदि (other != रिक्त && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

शून्य RTLIL::SigSpec::remove(ध्रुव pool<RTLIL::SigBit> &pattern)
{
	remove2(pattern, रिक्त);
}

शून्य RTLIL::SigSpec::remove(ध्रुव pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) ध्रुव
{
	RTLIL::SigSpec tmp = *एतत्;
	tmp.remove2(pattern, other);
}

शून्य RTLIL::SigSpec::remove2(ध्रुव pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	यदि (other != रिक्त) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	प्रति (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		यदि (!(bits_[read_idx].wire != रिक्त && pattern.गिनती(bits_[read_idx]))) {
			यदि (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				यदि (other != रिक्त)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	तार्किक modified = (write_idx < bits_.size());
	यदि (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	यदि (other != रिक्त && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

शून्य RTLIL::SigSpec::remove2(ध्रुव std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	यदि (other != रिक्त) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	प्रति (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		यदि (!(bits_[read_idx].wire != रिक्त && pattern.गिनती(bits_[read_idx]))) {
			यदि (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				यदि (other != रिक्त)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	तार्किक modified = (write_idx < bits_.size());
	यदि (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	यदि (other != रिक्त && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

शून्य RTLIL::SigSpec::remove2(ध्रुव pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	यदि (other != रिक्त) {
		log_assert(size() == other->size());
		other->unpack();
	}

	तार्किक modified = असत्य;
	तार्किक other_modified = असत्य;
	प्रति (पूर्णाङ्क i = GetSize(bits_) - 1; i >= 0; i--) {
		यदि (bits_[i].wire != रिक्त && pattern.गिनती(bits_[i].wire)) {
			modified = सत्य;
			bits_.erase(bits_.begin() + i);
			यदि (other != रिक्त) {
				other_modified = सत्य;
				other->bits_.erase(other->bits_.begin() + i);
			}
		}
	}

	यदि (modified) {
		hash_.clear();
		try_repack();
	}
	यदि (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ध्रुव RTLIL::SigSpec &pattern, ध्रुव RTLIL::SigSpec *other) ध्रुव
{
	log_assert(other == रिक्त || size() == other->size());

	RTLIL::SigSpec ret;
	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();

	प्रति (auto& pattern_chunk : pattern.chunks()) {
		यदि (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			पूर्णाङ्क i = 0;
			प्रति (ध्रुव RTLIL::SigBit &bit : bits_match) {
				यदि (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} अन्यथा {
			प्रति (ध्रुव RTLIL::SigBit &bit : bits_match)
				यदि (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	प्रत्यागम ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ध्रुव pool<RTLIL::SigBit> &pattern, ध्रुव RTLIL::SigSpec *other) ध्रुव
{
	log_assert(other == रिक्त || size() == other->size());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	यदि (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		पूर्णाङ्क i = 0;
		प्रति (ध्रुव RTLIL::SigBit &bit : bits_match) {
			यदि (bit.wire && pattern.गिनती(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} अन्यथा {
		प्रति (ध्रुव RTLIL::SigBit &bit : bits_match)
			यदि (bit.wire && pattern.गिनती(bit))
				ret.append(bit);
	}

	ret.check();
	प्रत्यागम ret;
}

शून्य RTLIL::SigSpec::replace(पूर्णाङ्क offset, ध्रुव RTLIL::SigSpec &with)
{
	यदि (with.size() == 0)
		प्रत्यागम;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.size() >= 0);
	log_assert(offset+with.size() <= size());

	पूर्णाङ्क i = 0;
	प्रति (ध्रुव RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

शून्य RTLIL::SigSpec::remove_const()
{
	यदि (rep_ == CHUNK)
	{
		यदि (chunk_.wire == रिक्त) {
			chunk_.~SigChunk();
			init_empty_bits();
			hash_.clear();
		}
	}
	अन्यथा
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.size());
		प्रति (auto &bit : bits_)
			यदि (bit.wire != रिक्त)
				new_bits.धकेलो_back(bit);
		यदि (GetSize(new_bits) != GetSize(bits_)) {
			bits_.swap(new_bits);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

शून्य RTLIL::SigSpec::remove(पूर्णाङ्क offset, पूर्णाङ्क length)
{
	यदि (length == 0)
		प्रत्यागम;

	unpack();

	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	bits_.erase(bits_.begin() + offset, bits_.begin() + offset + length);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(पूर्णाङ्क offset, पूर्णाङ्क length) ध्रुव
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	std::vector<SigBit> extracted;
	SigBit first;
	तार्किक is_packing = सत्य;
	प्रति (पूर्णाङ्क i = offset; i < offset + length; i++) {
		तार्किक was_packing_before = is_packing;
		SigBit bit = (*एतत्)[i];
		यदि (i == offset) {
			first = bit;
			यदि (!bit.wire)
				is_packing = असत्य;
		} अन्यथा {
			यदि (bit.wire != first.wire)
				is_packing = असत्य;
			यदि (bit.wire)
				यदि (bit.offset != first.offset + (i - offset))
					is_packing = असत्य;
		}
		यदि (was_packing_before && !is_packing)
			प्रति (पूर्णाङ्क j = offset; j < i; j++)
				extracted.धकेलो_back((*एतत्)[j]);
		यदि (!is_packing)
			extracted.धकेलो_back((*एतत्)[i]);
	}
	यदि (is_packing)
		प्रत्यागम SigChunk(first.wire, first.offset, length);

	प्रत्यागम extracted;
}

शून्य RTLIL::SigSpec::rewrite_wires(std::function<शून्य(RTLIL::Wire*& wire)> rewrite)
{
	यदि (rep_ == CHUNK) {
		यदि (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		प्रत्यागम;
	}

	std::vector<RTLIL::SigBit> new_bits;
	प्रति (ध्रुव RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		यदि (c.wire != nullptr)
			rewrite(c.wire);
		प्रति (पूर्णाङ्क i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.clear();
}

शून्य RTLIL::SigSpec::append(ध्रुव RTLIL::SigSpec &सङ्केत)
{
	यदि (सङ्केत.size() == 0)
		प्रत्यागम;

	यदि (size() == 0) {
		*एतत् = सङ्केत;
		प्रत्यागम;
	}

	hash_.clear();
	यदि (rep_ == CHUNK && सङ्केत.rep_ == CHUNK && chunk_.wire == सङ्केत.chunk_.wire) {
		यदि (chunk_.wire == रिक्त) {
			chunk_.data.जोड़ो(chunk_.data.end(), सङ्केत.chunk_.data.begin(), सङ्केत.chunk_.data.end());
			chunk_.width = GetSize(chunk_.data);
			प्रत्यागम;
		}
		यदि (chunk_.offset + chunk_.width == सङ्केत.chunk_.offset) {
			chunk_.width += सङ्केत.chunk_.width;
			प्रत्यागम;
		}
	}

	unpack();
	प्रति (ध्रुव SigBit &bit : सङ्केत.bits())
		bits_.धकेलो_back(bit);
	check();
}

शून्य RTLIL::SigSpec::append(ध्रुव RTLIL::SigBit &bit)
{
	hash_.clear();

	यदि (size() == 0) {
		destroy();
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(bit);
		प्रत्यागम;
	}

	यदि (rep_ == CHUNK && chunk_.wire == bit.wire) {
		यदि (chunk_.wire == रिक्त) {
			chunk_.data.धकेलो_back(bit.data);
			chunk_.width++;
			प्रत्यागम;
		}
		यदि (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			प्रत्यागम;
		}
	}

	unpack();

	bits_.धकेलो_back(bit);
	check();
}

शून्य RTLIL::SigSpec::extend_u0(पूर्णाङ्क width, तार्किक is_signed)
{
	यदि (size() > width)
		remove(width, size() - width);

	यदि (size() < width) {
		RTLIL::SigBit padding = size() > 0 ? (*एतत्)[size() - 1] : RTLIL::State::Sx;
		यदि (!is_signed)
			padding = RTLIL::State::S0;
		यावत् (size() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(पूर्णाङ्क अंक) ध्रुव
{
	RTLIL::SigSpec sig;
	प्रति (पूर्णाङ्क i = 0; i < अंक; i++)
		sig.append(*एतत्);
	प्रत्यागम sig;
}

#ifndef NDEBUG
शून्य RTLIL::SigSpec::check(ध्रुव Module *mod) ध्रुव
{
	यदि (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		यदि (chunk_.wire == रिक्त) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.data.size() == (size_t)chunk_.width);
		} अन्यथा {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.data.size() == 0);
			यदि (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	अन्यथा यदि (size() <= 64)
	{
		यदि (mod != nullptr) {
			प्रति (ध्रुव RTLIL::SigBit &bit : bits_)
				यदि (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#endif

तार्किक RTLIL::SigSpec::operator <(ध्रुव RTLIL::SigSpec &other) ध्रुव
{
	यदि (एतत् == &other)
		प्रत्यागम असत्य;

	यदि (size() != other.size())
		प्रत्यागम size() < other.size();

	auto other_it = other.chunks().begin();
	प्रति (ध्रुव SigChunk &c : chunks()) {
		यदि (c != *other_it)
			प्रत्यागम c < *other_it;
		++other_it;
	}

	प्रत्यागम असत्य;
}

तार्किक RTLIL::SigSpec::operator ==(ध्रुव RTLIL::SigSpec &other) ध्रुव
{
	यदि (एतत् == &other)
		प्रत्यागम सत्य;

	यदि (size() != other.size())
		प्रत्यागम असत्य;

	auto other_it = other.chunks().begin();
	प्रति (ध्रुव SigChunk &c : chunks()) {
		यदि (c != *other_it)
			प्रत्यागम असत्य;
		++other_it;
	}

	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::is_wire() ध्रुव
{
	Chunks cs = chunks();
	auto it = cs.begin();
	यदि (it == cs.end())
		प्रत्यागम असत्य;
	ध्रुव RTLIL::SigChunk &chunk = *it;
	प्रत्यागम chunk.wire && chunk.wire->width == size() && ++it == cs.end();
}

तार्किक RTLIL::SigSpec::is_chunk() ध्रुव
{
	Chunks cs = chunks();
	auto it = cs.begin();
	यदि (it == cs.end())
		प्रत्यागम असत्य;
	प्रत्यागम ++it == cs.end();
}

तार्किक RTLIL::SigSpec::known_driver() ध्रुव
{
	प्रति (auto &chunk : chunks())
		यदि (chunk.is_wire() && !chunk.wire->known_driver())
			प्रत्यागम असत्य;
	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_const() ध्रुव
{
	प्रति (auto &chunk : chunks())
		यदि (chunk.width > 0 && chunk.wire != रिक्त)
			प्रत्यागम असत्य;
	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_zero() ध्रुव
{
	प्रति (auto &chunk : chunks()) {
		यदि (chunk.width > 0 && chunk.wire != रिक्त)
			प्रत्यागम असत्य;
		प्रति (RTLIL::State d : chunk.data)
			यदि (d != RTLIL::State::S0)
				प्रत्यागम असत्य;
	}
	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_ones() ध्रुव
{
	प्रति (auto &chunk : chunks()) {
		यदि (chunk.width > 0 && chunk.wire != रिक्त)
			प्रत्यागम असत्य;
		प्रति (RTLIL::State d : chunk.data)
			यदि (d != RTLIL::State::S1)
				प्रत्यागम असत्य;
	}
	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_def() ध्रुव
{
	प्रति (auto &chunk : chunks()) {
		यदि (chunk.width > 0 && chunk.wire != रिक्त)
			प्रत्यागम असत्य;
		प्रति (RTLIL::State d : chunk.data)
			यदि (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				प्रत्यागम असत्य;
	}
	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_undef() ध्रुव
{
	प्रति (auto &chunk : chunks()) {
		यदि (chunk.width > 0 && chunk.wire != रिक्त)
			प्रत्यागम असत्य;
		प्रति (RTLIL::State d : chunk.data)
			यदि (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				प्रत्यागम असत्य;
	}
	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::has_const() ध्रुव
{
	प्रति (auto &chunk : chunks())
		यदि (chunk.width > 0 && chunk.wire == रिक्त)
			प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}

तार्किक RTLIL::SigSpec::has_const(State state) ध्रुव
{
	प्रति (auto &chunk : chunks())
		यदि (chunk.width > 0 && chunk.wire == रिक्त && std::find(chunk.data.begin(), chunk.data.end(), state) != chunk.data.end())
			प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}


तार्किक RTLIL::SigSpec::has_marked_bits() ध्रुव
{
	प्रति (auto &chunk : chunks())
		यदि (chunk.width > 0 && chunk.wire == रिक्त) {
			प्रति (RTLIL::State d : chunk.data)
				यदि (d == RTLIL::State::Sm)
					प्रत्यागम सत्य;
		}
	प्रत्यागम असत्य;
}

तार्किक RTLIL::SigSpec::is_onehot(पूर्णाङ्क *pos) ध्रुव
{
	यदि (std::optional<RTLIL::Const> c = try_as_const())
		प्रत्यागम c->is_onehot(pos);
	प्रत्यागम असत्य;
}

तार्किक RTLIL::SigSpec::as_bool() ध्रुव
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मूल्य());
	प्रत्यागम c->as_bool();
}

पूर्णाङ्क RTLIL::SigSpec::as_int(तार्किक is_signed) ध्रुव
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मूल्य());
	प्रत्यागम c->as_int(is_signed);
}

तार्किक RTLIL::SigSpec::convertible_to_int(तार्किक is_signed) ध्रुव
{
	std::optional<RTLIL::Const> c = try_as_const();
	यदि (!c.has_मूल्य())
		प्रत्यागम असत्य;
	प्रत्यागम c->convertible_to_int(is_signed);
}

std::optional<पूर्णाङ्क> RTLIL::SigSpec::try_as_int(तार्किक is_signed) ध्रुव
{
	std::optional<RTLIL::Const> c = try_as_const();
	यदि (!c.has_मूल्य())
		प्रत्यागम std::nullopt;
	प्रत्यागम c->try_as_int(is_signed);
}

पूर्णाङ्क RTLIL::SigSpec::as_int_saturating(तार्किक is_signed) ध्रुव
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मूल्य());
	प्रत्यागम c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ध्रुव
{
	std::string str;
	str.reserve(size());
	std::vector<RTLIL::SigChunk> chunks = *एतत्;
	प्रति (size_t i = chunks.size(); i > 0; i--) {
		ध्रुव RTLIL::SigChunk &chunk = chunks[i-1];
		यदि (chunk.wire != रिक्त)
			str.append(chunk.width, '?');
		अन्यथा
			str += RTLIL::Const(chunk.data).as_string();
	}
	प्रत्यागम str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ध्रुव
{
	Chunks cs = chunks();
	auto it = cs.begin();
	यदि (it == cs.end())
		प्रत्यागम RTLIL::Const();
	SigChunk chunk = *it;
	यदि (chunk.wire != रिक्त || ++it != cs.end())
		प्रत्यागम std::nullopt;
	प्रत्यागम RTLIL::Const(std::move(chunk.data));
}

RTLIL::Const RTLIL::SigSpec::as_const() ध्रुव
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मूल्य());
	प्रत्यागम *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ध्रुव
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == size());
	प्रत्यागम chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ध्रुव
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	प्रत्यागम chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() ध्रुव
{
	प्रत्यागम RTLIL::SigBit(*एतत्);
}

तार्किक RTLIL::SigSpec::match(ध्रुव वर्ण* pattern) ध्रुव
{
	पूर्णाङ्क pattern_len = strlen(pattern);
	log_assert(pattern_len == size());

	प्रति (पूर्णाङ्क i = 0; i < pattern_len; ++i) {
		वर्ण ch = pattern[i];
		यदि (ch == ' ')
			अनुवर्त;
		RTLIL::SigBit bit = (*एतत्)[pattern_len - 1 - i];
		यदि (ch == '*') {
			यदि (bit != State::Sz && bit != State::Sx)
				प्रत्यागम असत्य;
			अनुवर्त;
		}
		यदि (ch == '0') {
			यदि (bit != State::S0)
				प्रत्यागम असत्य;
		} अन्यथा
		यदि (ch == '1') {
			यदि (bit != State::S1)
				प्रत्यागम असत्य;
		} अन्यथा
			log_abort();
	}

	प्रत्यागम सत्य;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() ध्रुव
{
	std::set<RTLIL::SigBit> sigbits;
	प्रति (auto &c : chunks())
		प्रति (पूर्णाङ्क i = 0; i < c.width; i++)
			sigbits.जोड़ो(RTLIL::SigBit(c, i));
	प्रत्यागम sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() ध्रुव
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(size());
	प्रति (auto &c : chunks())
		प्रति (पूर्णाङ्क i = 0; i < c.width; i++)
			sigbits.जोड़ो(RTLIL::SigBit(c, i));
	प्रत्यागम sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() ध्रुव
{
	std::vector<RTLIL::SigBit> result;
	result.reserve(size());
	प्रति (SigBit bit : *एतत्)
		result.धकेलो_back(bit);
	प्रत्यागम result;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(ध्रुव RTLIL::SigSpec &other) ध्रुव
{
	पूर्णाङ्क this_size = size();
	log_assert(this_size == other.size());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	प्रति (पूर्णाङ्क i = 0; i < this_size; i++)
		new_map[(*एतत्)[i]] = other[i];

	प्रत्यागम new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(ध्रुव RTLIL::SigSpec &other) ध्रुव
{
	पूर्णाङ्क this_size = size();
	log_assert(this_size == other.size());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_size);
	प्रति (पूर्णाङ्क i = 0; i < this_size; i++)
		new_map[(*एतत्)[i]] = other[i];

	प्रत्यागम new_map;
}

स्थिर शून्य sigspec_parse_split(std::vector<std::string> &tokens, ध्रुव std::string &text, वर्ण sep)
{
	size_t start = 0, end = 0;
	यावत् ((end = text.find(sep, start)) != std::string::npos) {
		tokens.धकेलो_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.धकेलो_back(text.substr(start));
}

स्थिर RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ध्रुव std::string &netname)
{
	प्रत्यागम module->wire(netname);
}

तार्किक RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	प्रति (पूर्णाङ्क tokidx = पूर्णाङ्क(tokens.size())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		यदि (netname.size() == 0)
			अनुवर्त;

		यदि (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			auto ast = p.const2ast(netname);
			यदि (ast == nullptr)
				प्रत्यागम असत्य;
			sig.append(RTLIL::Const(ast->bits));
			अनुवर्त;
		}

		यदि (module == रिक्त)
			प्रत्यागम असत्य;

		यदि (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		यदि (sigspec_parse_wire(module, netname) == nullptr) {
			size_t indices_pos = netname.size()-1;
			यदि (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				यावत् (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				यदि (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					यावत् (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				यदि (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		यदि (wire == nullptr)
			प्रत्यागम असत्य;

		यदि (!indices.empty()) {
			std::vector<std::string> index_tokens;
			sigspec_parse_split(index_tokens, indices.substr(1, indices.size()-2), ':');
			यदि (index_tokens.size() == 1) {
				पूर्णाङ्क a = atoi(index_tokens.at(0).c_str());
				यदि (a < 0 || a >= wire->width)
					प्रत्यागम असत्य;
				sig.append(RTLIL::SigSpec(wire, a));
			} अन्यथा {
				पूर्णाङ्क a = atoi(index_tokens.at(0).c_str());
				पूर्णाङ्क b = atoi(index_tokens.at(1).c_str());
				यदि (a > b) {
					पूर्णाङ्क tmp = a;
					a = b, b = tmp;
				}
				यदि (a < 0 || a >= wire->width)
					प्रत्यागम असत्य;
				यदि (b < 0 || b >= wire->width)
					प्रत्यागम असत्य;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} अन्यथा
			sig.append(wire);
	}

	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	यदि (str.empty() || str[0] != '@')
		प्रत्यागम parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	यदि (sel_name == IdString::Null || design->selection_vars.गिनती(sel_name) == 0)
		प्रत्यागम असत्य;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	प्रति (auto &it : module->wires_)
		यदि (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::parse_rhs(ध्रुव RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	यदि (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.size());
		प्रत्यागम सत्य;
	}

	यदि (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.size());
		प्रत्यागम सत्य;
	}

	यदि (lhs.is_chunk()) {
		वर्ण *p = (वर्ण*)str.c_str(), *endptr;
		long पूर्णाङ्क val = strtol(p, &endptr, 10);
		यदि (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.size());
			प्रत्यागम सत्य;
		}
	}

	यदि (!parse(sig, module, str))
		प्रत्यागम असत्य;
	यदि (sig.size() > lhs.size())
		sig.remove(lhs.size(), sig.size() - lhs.size());
	प्रत्यागम सत्य;
}

RTLIL::SigSpec::operator std::vector<RTLIL::SigChunk>() ध्रुव
{
	std::vector<RTLIL::SigChunk> result;
	प्रति (ध्रुव RTLIL::SigChunk &c : chunks())
		result.धकेलो_back(c);
	प्रत्यागम result;
}

RTLIL::CaseRule::~CaseRule()
{
	प्रति (auto it = switches.begin(); it != switches.end(); it++)
		delete *it;
}

तार्किक RTLIL::CaseRule::empty() ध्रुव
{
	प्रत्यागम actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() ध्रुव
{
	RTLIL::CaseRule *new_caserule = नूतन RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	प्रति (auto &it : switches)
		new_caserule->switches.धकेलो_back(it->clone());
	प्रत्यागम new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	प्रति (auto it = cases.begin(); it != cases.end(); it++)
		delete *it;
}

तार्किक RTLIL::SwitchRule::empty() ध्रुव
{
	प्रत्यागम cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() ध्रुव
{
	RTLIL::SwitchRule *new_switchrule = नूतन RTLIL::SwitchRule;
	new_switchrule->सङ्केत = सङ्केत;
	new_switchrule->attributes = attributes;
	प्रति (auto &it : cases)
		new_switchrule->cases.धकेलो_back(it->clone());
	प्रत्यागम new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() ध्रुव
{
	RTLIL::SyncRule *new_syncrule = नूतन RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->सङ्केत = सङ्केत;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	प्रत्यागम new_syncrule;
}

RTLIL::Process::~Process()
{
	प्रति (auto it = syncs.begin(); it != syncs.end(); it++)
		delete *it;
}

RTLIL::Process *RTLIL::Process::clone() ध्रुव
{
	RTLIL::Process *new_proc = नूतन RTLIL::Process;

	new_proc->attributes = attributes;

	RTLIL::CaseRule *rc_ptr = मूल_case.clone();
	new_proc->मूल_case = *rc_ptr;
	rc_ptr->switches.clear();
	delete rc_ptr;

	प्रति (auto &it : syncs)
		new_proc->syncs.धकेलो_back(it->clone());

	प्रत्यागम new_proc;
}

RTLIL::Memory::~Memory()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
स्थिर std::map<unsigned पूर्णाङ्क, RTLIL::Memory*> all_memorys;
std::map<unsigned पूर्णाङ्क, RTLIL::Memory*> *RTLIL::Memory::get_all_memorys(शून्य)
{
	प्रत्यागम &all_memorys;
}
#endif
YOSYS_NAMESPACE_END
