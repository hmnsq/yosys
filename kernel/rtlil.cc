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

#અન્તર્ભાવ "kernel/rtlil.h"
#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/macc.h"
#અન્તર્ભાવ "kernel/newcelltypes.h"
#અન્તર્ભાવ "kernel/sigtools.h"
#અન્તર્ભાવ "kernel/threading.h"
#અન્તર્ભાવ "frontends/verilog/verilog_frontend.h"
#અન્તર્ભાવ "frontends/verilog/preproc.h"
#અન્તર્ભાવ "backends/rtlil/rtlil_backend.h"

#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <algorithm>
#અન્તર્ભાવ <charconv>
#અન્તર્ભાવ <optional>
#અન્તર્ભાવ <string_view>
#અન્તર્ભાવ <sstream>

YOSYS_NAMESPACE_BEGIN

dict<std::string, std::string> RTLIL::constpad;

અચલ pool<IdString> &RTLIL::builtin_ff_cell_types() {
	સ્થિર અચલ pool<IdString> res = []() {
		pool<IdString> r;
		માટે (size_t i = 0; i < StaticCellTypes::builder.ગણતરી; i++) {
			auto &cell = StaticCellTypes::builder.cells[i];
			જો (cell.features.is_ff)
				r.ઉમેરો(cell.type);
		}
		પાછા r;
	}();
	પાછા res;
}

#define check(condition) log_assert(condition && "malformed Const union")

અચલ Const::bitvectype& Const::get_bits() અચલ {
	check(is_bits());
	પાછા *get_if_bits();
}

અચલ std::string& Const::get_str() અચલ {
	check(is_str());
	પાછા *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	પાછા *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	પાછા *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	નવું ((શૂન્ય*)&str_) std::string(std::move(str));
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	અક્ષર bytes[] = {
		(અક્ષર)(val >> 24), (અક્ષર)(val >> 16), (અક્ષર)(val >> 8), (અક્ષર)val
	};
	નવું ((શૂન્ય*)&str_) std::string(bytes, 4);
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val, પૂર્ણાંક width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	જો ((width & 7) == 0) {
		નવું ((શૂન્ય*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		પૂર્ણાંક bytes = width >> 3;
		signed અક્ષર sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<પૂર્ણાંક>(bytes, કદ(val));
		માટે (પૂર્ણાંક i = 0; i < bytes; i++) {
			str[str.size() - 1 - i] = val;
			val = val >> 8;
		}
		પાછા;
	}

	નવું ((શૂન્ય*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	માટે (પૂર્ણાંક i = 0; i < width; i++) {
		bv.ધકેલો_back((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, પૂર્ણાંક width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	નવું ((શૂન્ય*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	માટે (પૂર્ણાંક i = 0; i < width; i++)
		bv.ધકેલો_back(bit);
}

RTLIL::Const::Const(અચલ std::vector<તાર્કિક> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	નવું ((શૂન્ય*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.size());
	માટે (અચલ auto &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(અચલ RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	જો (is_str())
		નવું ((શૂન્ય*)&str_) std::string(other.get_str());
	નહીંતર જો (is_bits())
		નવું ((શૂન્ય*)&bits_) bitvectype(other.get_bits());
	નહીંતર
		check(ખોટું);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	જો (is_str())
		નવું ((શૂન્ય*)&str_) std::string(std::move(other.get_str()));
	નહીંતર જો (is_bits())
		નવું ((શૂન્ય*)&bits_) bitvectype(std::move(other.get_bits()));
	નહીંતર
		check(ખોટું);
}

RTLIL::Const &RTLIL::Const::operator =(અચલ RTLIL::Const &other) {
	flags = other.flags;
	જો (other.is_str()) {
		જો (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(શૂન્ય)નવું ((શૂન્ય*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} નહીંતર જો (other.is_bits()) {
		જો (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(શૂન્ય)નવું ((શૂન્ય*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} નહીંતર {
		check(ખોટું);
	}
	પાછા *આ;
}

RTLIL::Const::~Const() {
	જો (is_bits())
		bits_.~bitvectype();
	નહીંતર જો (is_str())
		str_.~string();
	નહીંતર
		check(ખોટું);
}

તાર્કિક RTLIL::Const::operator<(અચલ RTLIL::Const &other) અચલ
{
	જો (size() != other.size())
		પાછા size() < other.size();

	માટે (પૂર્ણાંક i = 0; i < size(); i++)
		જો ((*આ)[i] != other[i])
			પાછા (*આ)[i] < other[i];

	પાછા ખોટું;
}

તાર્કિક RTLIL::Const::operator ==(અચલ RTLIL::Const &other) અચલ
{
	જો (is_str() && other.is_str())
		પાછા get_str() == other.get_str();
	જો (is_bits() && other.is_bits())
		પાછા get_bits() == other.get_bits();

	જો (size() != other.size())
		પાછા ખોટું;

	માટે (પૂર્ણાંક i = 0; i < size(); i++)
	જો ((*આ)[i] != other[i])
		પાછા ખોટું;

	પાછા સાચું;
}

તાર્કિક RTLIL::Const::operator !=(અચલ RTLIL::Const &other) અચલ
{
	પાછા !(*આ == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	પાછા get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() અચલ
{
	std::vector<State> v;
	માટે (auto bit : *આ)
		v.ધકેલો_back(bit);
	પાછા v;
}

તાર્કિક RTLIL::Const::as_bool() અચલ
{
	જો (is_str()) {
		માટે (અક્ષર ch : get_str())
			જો (ch != 0)
				પાછા સાચું;
		પાછા ખોટું;
	}

	અચલ bitvectype& bv = get_bits();
	માટે (size_t i = 0; i < bv.size(); i++)
		જો (bv[i] == State::S1)
			પાછા સાચું;
	પાછા ખોટું;
}

પૂર્ણાંક RTLIL::Const::as_int(તાર્કિક is_signed) અચલ
{
	int32_t ret = 0;
	જો (અચલ std::string *s = get_if_str()) {
		પૂર્ણાંક size = GetSize(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		માટે (પૂર્ણાંક i = std::min(4, size); i > 0; i--)
			ret |= static_cast<unsigned અક્ષર>((*s)[size - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		જો (is_signed && size > 0 && size < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << size*8;
		પાછા ret;
	}

	અચલ bitvectype& bv = get_bits();
	પૂર્ણાંક significant_bits = std::min(GetSize(bv), 32);
	માટે (પૂર્ણાંક i = 0; i < significant_bits; i++)
		જો (bv[i] == State::S1)
			ret |= 1 << i;
	જો (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	પાછા ret;
}

તાર્કિક RTLIL::Const::convertible_to_int(તાર્કિક is_signed) અચલ
{
	auto size = get_min_size(is_signed);

	જો (size < 0)
		પાછા ખોટું;

	// If it fits in 31 bits it is definitely convertible
	જો (size <= 31)
		પાછા સાચું;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	જો (size == 32) {
		જો (is_signed)
			પાછા સાચું;
		પાછા (*આ)[size - 1] != State::S1;
	}

	પાછા ખોટું;
}

std::optional<પૂર્ણાંક> RTLIL::Const::try_as_int(તાર્કિક is_signed) અચલ
{
	જો (!convertible_to_int(is_signed))
		પાછા std::nullopt;
	પાછા as_int(is_signed);
}

પૂર્ણાંક RTLIL::Const::as_int_saturating(તાર્કિક is_signed) અચલ
{
	જો (!convertible_to_int(is_signed)) {
		જો (!is_signed)
			પાછા std::numeric_limits<પૂર્ણાંક>::max();

		અચલ auto min_size = get_min_size(is_signed);
		log_assert(min_size > 0);
		અચલ auto neg = (*આ)[min_size - 1];
		પાછા neg ? std::numeric_limits<પૂર્ણાંક>::min() : std::numeric_limits<પૂર્ણાંક>::max();
	}
	પાછા as_int(is_signed);
}

શૂન્ય RTLIL::Const::tag_bare_integer_const(અચલ std::string &કિંમત)
{
	જો (કિંમત.empty() || કિંમત.find('\'') != std::string::npos)
		પાછા;
	size_t start = (કિંમત[0] == '-' || કિંમત[0] == '+') ? 1 : 0;
	જો (start < કિંમત.size() && std::all_of(કિંમત.begin() + start, કિંમત.end(), ::isdigit))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

પૂર્ણાંક RTLIL::Const::get_min_size(તાર્કિક is_signed) અચલ
{
	જો (empty()) પાછા 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	જો (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	નહીંતર
		leading_bit = RTLIL::State::S0;

	auto idx = size();
	જ્યારે (idx > 0 && (*આ)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	જો (is_signed && idx < size()) {
		idx++;
	}
	// must be at least one bit
	પાછા (idx == 0) ? 1 : idx;
}

શૂન્ય RTLIL::Const::compress(તાર્કિક is_signed)
{
	auto idx = get_min_size(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<પૂર્ણાંક> RTLIL::Const::as_int_compress(તાર્કિક is_signed) અચલ
{
	પાછા try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(અચલ અક્ષર* any) અચલ
{
	પૂર્ણાંક sz = size();
	std::string ret;
	ret.reserve(sz);
	માટે (પૂર્ણાંક i = sz - 1; i >= 0; --i)
		પસંદગી ((*આ)[i]) {
			સ્થિતિ S0: ret.ધકેલો_back('0'); તોડો;
			સ્થિતિ S1: ret.ધકેલો_back('1'); તોડો;
			સ્થિતિ Sx: ret.ધકેલો_back('x'); તોડો;
			સ્થિતિ Sz: ret.ધકેલો_back('z'); તોડો;
			સ્થિતિ Sa: ret += any; તોડો;
			સ્થિતિ Sm: ret.ધકેલો_back('m'); તોડો;
		}
	પાછા ret;
}

RTLIL::Const RTLIL::Const::from_string(અચલ std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.size());
	માટે (auto it = str.rbegin(); it != str.rend(); it++)
		પસંદગી (*it) {
			સ્થિતિ '0': bv.ધકેલો_back(State::S0); તોડો;
			સ્થિતિ '1': bv.ધકેલો_back(State::S1); તોડો;
			સ્થિતિ 'x': bv.ધકેલો_back(State::Sx); તોડો;
			સ્થિતિ 'z': bv.ધકેલો_back(State::Sz); તોડો;
			સ્થિતિ 'm': bv.ધકેલો_back(State::Sm); તોડો;
			મૂળભૂત: bv.ધકેલો_back(State::Sa);
		}
	પાછા c;
}

std::string RTLIL::Const::decode_string() અચલ
{
	જો (auto str = get_if_str())
		પાછા *str;

	અચલ bitvectype& bv = get_bits();
	અચલ પૂર્ણાંક n = GetSize(bv);
	અચલ પૂર્ણાંક n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	પૂર્ણાંક i = n_over_8 * 8;
	જો (i < n) {
		અક્ષર ch = 0;
		માટે (પૂર્ણાંક j = 0; j < (n - i); j++) {
			જો (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		જો (ch != 0)
			s.append({ch});
	}
	i -= 8;
	માટે (; i >= 0; i -= 8) {
		અક્ષર ch = 0;
		માટે (પૂર્ણાંક j = 0; j < 8; j++) {
			જો (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		જો (ch != 0)
			s.append({ch});
	}
	પાછા s;
}

પૂર્ણાંક RTLIL::Const::size() અચલ {
	જો (is_str())
		પાછા 8 * str_.size();
	નહીંતર {
		check(is_bits());
		પાછા bits_.size();
	}
}

તાર્કિક RTLIL::Const::empty() અચલ {
	જો (is_str())
		પાછા str_.empty();
	નહીંતર {
		check(is_bits());
		પાછા bits_.empty();
	}
}

શૂન્ય RTLIL::Const::bitvectorize_internal() {
	જો (tag == backing_tag::bits)
		પાછા;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.size() * 8);
	માટે (પૂર્ણાંક i = str_.size() - 1; i >= 0; i--) {
		unsigned અક્ષર ch = str_[i];
		માટે (પૂર્ણાંક j = 0; j < 8; j++) {
			new_bits.ધકેલો_back((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(શૂન્ય)નવું ((શૂન્ય*)&bits_) bitvectype(std::move(new_bits));
		tag = backing_tag::bits;
	}
}

શૂન્ય RTLIL::Const::append(અચલ RTLIL::Const &other) {
	bitvectorize_internal();
	bitvectype& bv = get_bits();
	bv.ઉમેરો(bv.end(), other.begin(), other.end());
}

RTLIL::State RTLIL::Const::const_iterator::operator*() અચલ {
	જો (auto bv = parent->get_if_bits())
		પાછા (*bv)[idx];

	પૂર્ણાંક char_idx = parent->get_str().size() - idx / 8 - 1;
	તાર્કિક bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	પાછા bit ? State::S1 : State::S0;
}

તાર્કિક RTLIL::Const::is_fully_zero() અચલ
{
	જો (auto str = get_if_str()) {
		માટે (અક્ષર ch : *str)
			જો (ch != 0)
				પાછા ખોટું;
		પાછા સાચું;
	}

	અચલ bitvectype& bv = get_bits();

	માટે (અચલ auto &bit : bv)
		જો (bit != RTLIL::State::S0)
			પાછા ખોટું;

	પાછા સાચું;
}

તાર્કિક RTLIL::Const::is_fully_ones() અચલ
{
	જો (auto str = get_if_str()) {
		માટે (અક્ષર ch : *str)
			જો (ch != (અક્ષર)0xff)
				પાછા ખોટું;
		પાછા સાચું;
	}

	અચલ bitvectype& bv = get_bits();
	માટે (અચલ auto &bit : bv)
		જો (bit != RTLIL::State::S1)
			પાછા ખોટું;

	પાછા સાચું;
}

તાર્કિક RTLIL::Const::is_fully_def() અચલ
{
	જો (is_str())
		પાછા સાચું;

	અચલ bitvectype& bv = get_bits();
	માટે (અચલ auto &bit : bv)
		જો (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			પાછા ખોટું;

	પાછા સાચું;
}

તાર્કિક RTLIL::Const::is_fully_undef() અચલ
{
	જો (auto str = get_if_str())
		પાછા str->empty();

	અચલ bitvectype& bv = get_bits();
	માટે (અચલ auto &bit : bv)
		જો (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			પાછા ખોટું;

	પાછા સાચું;
}

તાર્કિક RTLIL::Const::is_fully_undef_x_only() અચલ
{
	જો (auto str = get_if_str())
		પાછા str->empty();

	અચલ bitvectype& bv = get_bits();
	માટે (અચલ auto &bit : bv)
		જો (bit != RTLIL::State::Sx)
			પાછા ખોટું;

	પાછા સાચું;
}

તાર્કિક RTLIL::Const::is_onehot(પૂર્ણાંક *pos) અચલ
{
	તાર્કિક found = ખોટું;
	પૂર્ણાંક size = GetSize(*આ);
	માટે (પૂર્ણાંક i = 0; i < size; i++) {
		State bit = (*આ)[i];
		જો (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			પાછા ખોટું;
		જો (bit == RTLIL::State::S1) {
			જો (found)
				પાછા ખોટું;
			જો (pos)
				*pos = i;
			found = સાચું;
		}
	}
	પાછા found;
}

Hasher RTLIL::Const::hash_into(Hasher h) અચલ
{
	જો (auto str = get_if_str())
		પાછા hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	અચલ bitvectype &bv = get_bits();
	પૂર્ણાંક size = GetSize(bv);
	std::string packed;
	પૂર્ણાંક packed_size = (size + 7) >> 3;
	packed.resize(packed_size, 0);
	માટે (પૂર્ણાંક bi = 0; bi < packed_size; ++bi) {
		અક્ષર ch = 0;
		પૂર્ણાંક end = std::min((bi + 1)*8, size);
		માટે (પૂર્ણાંક i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			જો (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<અક્ષર>::hash_into(ch, h);
				માટે (; i < size; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(size);
				પાછા h;
			}
			ch |= static_cast<પૂર્ણાંક>(b) << (i & 7);
		}
		packed[packed_size - 1 - bi] = ch;
	}
	પાછા hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(પૂર્ણાંક offset, પૂર્ણાંક len, RTLIL::State padding) અચલ {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	માટે (પૂર્ણાંક i = offset; i < offset + len; i++)
		ret_bv.ધકેલો_back(i < GetSize(*આ) ? (*આ)[i] : padding);
	પાછા RTLIL::Const(ret_bv);
}
#undef check /* check(condition) for Const */

તાર્કિક RTLIL::AttrObject::has_attribute(RTLIL::IdString id) અચલ
{
	પાછા attributes.ગણતરી(id);
}

શૂન્ય RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, તાર્કિક કિંમત)
{
	જો (કિંમત)
		attributes[id] = RTLIL::Const(1);
	નહીંતર
		attributes.erase(id);
}

તાર્કિક RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) અચલ
{
	અચલ auto it = attributes.find(id);
	જો (it == attributes.end())
		પાછા ખોટું;
	પાછા it->second.as_bool();
}

શૂન્ય RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string કિંમત)
{
	જો (કિંમત.empty())
		attributes.erase(id);
	નહીંતર
		attributes[id] = કિંમત;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) અચલ
{
	std::string કિંમત;
	અચલ auto it = attributes.find(id);
	જો (it != attributes.end())
		કિંમત = it->second.decode_string();
	પાછા કિંમત;
}

શૂન્ય RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, અચલ pool<string> &data)
{
	string attrval;
	માટે (અચલ auto &s : data) {
		જો (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

શૂન્ય RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, અચલ pool<string> &data)
{
	pool<string> union_data = get_strpool_attribute(id);
	union_data.ઉમેરો(data.begin(), data.end());
	જો (!union_data.empty())
		set_strpool_attribute(id, union_data);
}

શૂન્ય RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->attributes = std::move(buf);
}

નામાકાશ {
	template<typename F>
	શૂન્ય walk_process_attr_objects(RTLIL::Process *process, F visit) {
		visit(process);
		std::vector<RTLIL::CaseRule*> case_ઢગલો{&process->મૂળ_case};
		જ્યારે (!case_ઢગલો.empty()) {
			RTLIL::CaseRule *cs = case_ઢગલો.back();
			case_ઢગલો.કાઢો_back();
			visit(cs);
			માટે (auto *sw : cs->switches) {
				visit(sw);
				માટે (auto *case_ : sw->cases)
					case_ઢગલો.ધકેલો_back(case_);
			}
		}
		માટે (auto *સમકાલિક : process->syncs)
			માટે (auto &mwa : સમકાલિક->mem_write_actions)
				visit(&mwa);
	}

	template<typename F>
	શૂન્ય walk_attr_objects(RTLIL::Design *design, F visit) {
		માટે (auto &[_, module] : design->modules_) {
			visit(module);
			માટે (auto &[_, wire] : module->wires_)
				visit(wire);
			માટે (auto &[_, mem] : module->memories)
				visit(mem);
			માટે (auto &[_, cell] : module->cells_)
				visit(cell);
			માટે (auto &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


size_t RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	auto મૂળ = [&](IdString ref) {
		જો (ref != IdString::Null)
			live.ઉમેરો(ref);
	};

	walk_attr_objects(આ, [&](અચલ RTLIL::AttrObject *obj) {
		માટે (auto &attr : obj->attributes)
			મૂળ(attr.first);
	});

	મૂળ(selected_active_module);

	માટે (auto &[name, module] : modules_) {
		મૂળ(name);
		માટે (IdString port : module->ports)
			મૂળ(port);
		માટે (IdString param : module->avail_parameters)
			મૂળ(param);
		માટે (auto &[param, _] : module->parameter_default_કિંમતો)
			મૂળ(param);
		માટે (auto &[name, _] : module->memories)
			મૂળ(name);
		માટે (auto &[name, _] : module->processes)
			મૂળ(name);
		માટે (auto &[name, wire] : module->wires_) {
			મૂળ(name);
			જો (wire->known_driver())
				મૂળ(wire->driverPort());
		}
		માટે (auto &[name, cell] : module->cells_) {
			મૂળ(name);
			મૂળ(cell->type);
			માટે (auto &conn : cell->connections())
				મૂળ(conn.first);
			માટે (auto &param : cell->parameters)
				મૂળ(param.first);
		}
		માટે (auto &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_ઢગલો{&process->મૂળ_case};
			જ્યારે (!case_ઢગલો.empty()) {
				RTLIL::CaseRule *cs = case_ઢગલો.back();
				case_ઢગલો.કાઢો_back();
				માટે (auto *sw : cs->switches) {
					માટે (auto *case_ : sw->cases)
						case_ઢગલો.ધકેલો_back(case_);
				}
			}
			માટે (auto *સમકાલિક : process->syncs) {
				માટે (auto &mwa : સમકાલિક->mem_write_actions)
					મૂળ(mwa.memid);
			}
		}
	}

	auto મૂળ_selection = [&](અચલ RTLIL::Selection &sel) {
		માટે (IdString m : sel.selected_modules)
			મૂળ(m);
		માટે (auto &[m, members] : sel.selected_members) {
			મૂળ(m);
			માટે (IdString member : members)
				મૂળ(member);
		}
	};

	માટે (auto &[name, sel] : selection_vars) {
		મૂળ(name);
		મૂળ_selection(sel);
	}

	માટે (auto &sel : selection_ઢગલો)
		મૂળ_selection(sel);

	size_t erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_ગણતરી;
	પાછા erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) અચલ
{
	pool<string> data;
	જો (attributes.ગણતરી(id) != 0)
		માટે (auto s : split_tokens(get_string_attribute(id), "|"))
			data.ઉમેરો(s);
	પાછા data;
}

શૂન્ય RTLIL::AttrObject::set_hdlname_attribute(અચલ vector<string> &hierarchy)
{
	string attrval;
	માટે (અચલ auto &ident : hierarchy) {
		જો (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() અચલ
{
	પાછા split_tokens(get_string_attribute(ID::hdlname), " ");
}

શૂન્ય RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, અચલ vector<પૂર્ણાંક> &data)
{
	std::stringstream attrval;
	માટે (auto &i : data) {
		જો (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<પૂર્ણાંક> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) અચલ
{
	vector<પૂર્ણાંક> data;
	auto it = attributes.find(id);
	જો (it != attributes.end())
		માટે (અચલ auto &s : split_tokens(attributes.at(id).decode_string())) {
			અક્ષર *end = nullptr;
			errno = 0;
			long કિંમત = strtol(s.c_str(), &end, 10);
			જો (end != s.c_str() + s.size())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			જો (errno == ERANGE || કિંમત < INT_MIN || કિંમત > INT_MAX)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			data.ધકેલો_back(કિંમત);
		}
	પાછા data;
}

તાર્કિક RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) અચલ
{
	જો (current_design != nullptr) {
		auto module = current_design->module(mod_name);
		પાછા module && module->get_blackbox_attribute();
	} નહીંતર {
		log_warning("Unable to check if module is boxed for null design.\n");
		પાછા ખોટું;
	}
}

તાર્કિક RTLIL::Selection::selected_module(RTLIL::IdString mod_name) અચલ
{
	જો (complete_selection)
		પાછા સાચું;
	જો (!selects_boxes && boxed_module(mod_name))
		પાછા ખોટું;
	જો (full_selection)
		પાછા સાચું;
	જો (selected_modules.ગણતરી(mod_name) > 0)
		પાછા સાચું;
	જો (selected_members.ગણતરી(mod_name) > 0)
		પાછા સાચું;
	પાછા ખોટું;
}

તાર્કિક RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) અચલ
{
	જો (complete_selection)
		પાછા સાચું;
	જો (!selects_boxes && boxed_module(mod_name))
		પાછા ખોટું;
	જો (full_selection)
		પાછા સાચું;
	જો (selected_modules.ગણતરી(mod_name) > 0)
		પાછા સાચું;
	પાછા ખોટું;
}

તાર્કિક RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) અચલ
{
	જો (complete_selection)
		પાછા સાચું;
	જો (!selects_boxes && boxed_module(mod_name))
		પાછા ખોટું;
	જો (full_selection)
		પાછા સાચું;
	જો (selected_modules.ગણતરી(mod_name) > 0)
		પાછા સાચું;
	જો (selected_members.ગણતરી(mod_name) > 0)
		જો (selected_members.at(mod_name).ગણતરી(memb_name) > 0)
			પાછા સાચું;
	પાછા ખોટું;
}

શૂન્ય RTLIL::Selection::optimize(RTLIL::Design *design)
{
	જો (design != current_design) {
		current_design = design;
	}

	જો (selects_boxes && full_selection)
		complete_selection = સાચું;
	જો (complete_selection) {
		full_selection = ખોટું;
		selects_boxes = સાચું;
	}
	જો (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		પાછા;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.clear();
	માટે (auto mod_name : selected_modules) {
		જો (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.ધકેલો_back(mod_name);
		selected_members.erase(mod_name);
	}
	માટે (auto mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	માટે (auto &it : selected_members)
		જો (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.ધકેલો_back(it.first);
	માટે (auto mod_name : del_list)
		selected_members.erase(mod_name);

	માટે (auto &it : selected_members) {
		del_list.clear();
		માટે (auto memb_name : it.second)
			જો (current_design->module(it.first)->ગણતરી_id(memb_name) == 0)
				del_list.ધકેલો_back(memb_name);
		માટે (auto memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	add_list.clear();
	માટે (auto &it : selected_members)
		જો (it.second.size() == 0)
			del_list.ધકેલો_back(it.first);
		નહીંતર જો (it.second.size() == current_design->module(it.first)->wires_.size() + current_design->module(it.first)->memories.size() +
				current_design->module(it.first)->cells_.size() + current_design->module(it.first)->processes.size())
			add_list.ધકેલો_back(it.first);
	માટે (auto mod_name : del_list)
		selected_members.erase(mod_name);
	માટે (auto mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.ઉમેરો(mod_name);
	}

	જો (selected_modules.size() == current_design->modules_.size()) {
		selected_modules.clear();
		selected_members.clear();
		જો (selects_boxes)
			complete_selection = સાચું;
		નહીંતર
			full_selection = સાચું;
	}
}

શૂન્ય RTLIL::Selection::clear()
{
	full_selection = ખોટું;
	complete_selection = ખોટું;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_defines (નવું define_map_t)
{
	સ્થિર unsigned પૂર્ણાંક hashidx_ગણતરી = 123456789;
	hashidx_ગણતરી = mkhash_xorshift(hashidx_ગણતરી);
	hashidx_ = hashidx_ગણતરી;

	refcount_modules_ = 0;
	selected_active_module.clear();
	ધકેલો_full_selection();

	RTLIL::Design::get_all_designs()->ઉમેરો(std::pair<unsigned પૂર્ણાંક, RTLIL::Design*>(hashidx_, આ));
}

RTLIL::Design::~Design()
{
	માટે (auto &pr : modules_)
		delete pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

સ્થિર std::map<unsigned પૂર્ણાંક, RTLIL::Design*> all_designs;
std::map<unsigned પૂર્ણાંક, RTLIL::Design*> *RTLIL::Design::get_all_designs(શૂન્ય)
{
	પાછા &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	પાછા RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

અચલ RTLIL::Module *RTLIL::Design::module(IdString id) અચલ {
	twines_.check_owned(id);
	પાછા modules_.ગણતરી(id) ? modules_.at(id) : ખાલી;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	પાછા modules_.ગણતરી(id) ? modules_.at(id) : ખાલી;
}

RTLIL::Module *RTLIL::Design::ટોચ_module() અચલ
{
	RTLIL::Module *module = nullptr;
	પૂર્ણાંક module_ગણતરી = 0;

	માટે (auto mod : selected_modules()) {
		જો (mod->get_bool_attribute(ID::ટોચ))
			પાછા mod;
		module_ગણતરી++;
		module = mod;
	}

	પાછા module_ગણતરી == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	જો (modules_.ગણતરી(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = નવું RTLIL::Module;
	modules_[name] = module;
	module->design = આ;
	module->name = name;

	માટે (auto mon : monitors)
		mon->notify_module_add(module);

	જો (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	પાછા module;
}

શૂન્ય RTLIL::Design::scratchpad_unset(અચલ std::string &varname)
{
	scratchpad.erase(varname);
}

શૂન્ય RTLIL::Design::scratchpad_set_int(અચલ std::string &varname, પૂર્ણાંક કિંમત)
{
	scratchpad[varname] = stringf("%d", કિંમત);
}

શૂન્ય RTLIL::Design::scratchpad_set_bool(અચલ std::string &varname, તાર્કિક કિંમત)
{
	scratchpad[varname] = કિંમત ? "true" : "false";
}

શૂન્ય RTLIL::Design::scratchpad_set_string(અચલ std::string &varname, std::string કિંમત)
{
	scratchpad[varname] = std::move(કિંમત);
}

પૂર્ણાંક RTLIL::Design::scratchpad_get_int(અચલ std::string &varname, પૂર્ણાંક default_કિંમત_2) અચલ
{
	auto it = scratchpad.find(varname);
	જો (it == scratchpad.end())
		પાછા default_કિંમત_2;

	અચલ std::string &str = it->second;

	જો (str == "0" || str == "false")
		પાછા 0;

	જો (str == "1" || str == "true")
		પાછા 1;

	અક્ષર *endptr = nullptr;
	long પૂર્ણાંક parsed_કિંમત = strtol(str.c_str(), &endptr, 10);
	પાછા *endptr ? default_કિંમત_2 : parsed_કિંમત;
}

તાર્કિક RTLIL::Design::scratchpad_get_bool(અચલ std::string &varname, તાર્કિક default_કિંમત_2) અચલ
{
	auto it = scratchpad.find(varname);
	જો (it == scratchpad.end())
		પાછા default_કિંમત_2;

	અચલ std::string &str = it->second;

	જો (str == "0" || str == "false")
		પાછા ખોટું;

	જો (str == "1" || str == "true")
		પાછા સાચું;

	પાછા default_કિંમત_2;
}

std::string RTLIL::Design::scratchpad_get_string(અચલ std::string &varname, અચલ std::string &default_કિંમત_2) અચલ
{
	auto it = scratchpad.find(varname);
	જો (it == scratchpad.end())
		પાછા default_કિંમત_2;

	પાછા it->second;
}

શૂન્ય RTLIL::Design::remove(RTLIL::Module *module)
{
	માટે (auto mon : monitors)
		mon->notify_module_del(module);

	જો (yosys_xtrace) {
		log("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	delete module;
}

શૂન્ય RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	add(module);
}

શૂન્ય RTLIL::Design::sort()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
	માટે (auto &it : modules_)
		it.second->sort();
}

શૂન્ય RTLIL::Design::sort_modules()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
}

શૂન્ય check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

શૂન્ય RTLIL::Design::check()
{
#ifndef NDEBUG
	log_assert(!selection_ઢગલો.empty());
	પૂર્ણાંક pool_size = 0;
	માટે (auto &it : modules_)
		pool_size = std::max(pool_size, ThreadPool::work_pool_size(0, it.second->cells_size(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_size);
	માટે (auto &it : modules_) {
		log_assert(આ == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#endif
}

શૂન્ય RTLIL::Design::optimize()
{
	માટે (auto &it : modules_)
		it.second->optimize();
	માટે (auto &it : selection_ઢગલો)
		it.optimize(આ);
	માટે (auto &it : selection_vars)
		it.second.optimize(આ);
}

શૂન્ય RTLIL::Design::clone_into(RTLIL::Design *dst) અચલ
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	માટે (auto it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

તાર્કિક RTLIL::Design::selected_module(IdString mod_name) અચલ
{
	જો (!selected_active_module.empty() && mod_name != selected_active_module)
		પાછા ખોટું;
	પાછા selection().selected_module(mod_name);
}

તાર્કિક RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) અચલ
{
	જો (!selected_active_module.empty() && mod_name != selected_active_module)
		પાછા ખોટું;
	પાછા selection().selected_whole_module(mod_name);
}

તાર્કિક RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) અચલ
{
	જો (!selected_active_module.empty() && mod_name != selected_active_module)
		પાછા ખોટું;
	પાછા selection().selected_member(mod_name, memb_name);
}

તાર્કિક RTLIL::Design::selected_module(RTLIL::Module *mod) અચલ
{
	પાછા selected_module(mod->name);
}

તાર્કિક RTLIL::Design::selected_whole_module(RTLIL::Module *mod) અચલ
{
	પાછા selected_whole_module(mod->name);
}

શૂન્ય RTLIL::Design::ધકેલો_selection(RTLIL::Selection sel)
{
	sel.current_design = આ;
	selection_ઢગલો.ધકેલો_back(sel);
}

શૂન્ય RTLIL::Design::ધકેલો_empty_selection()
{
	ધકેલો_selection(RTLIL::Selection::EmptySelection(આ));
}

શૂન્ય RTLIL::Design::ધકેલો_full_selection()
{
	ધકેલો_selection(RTLIL::Selection::FullSelection(આ));
}

શૂન્ય RTLIL::Design::ધકેલો_complete_selection()
{
	ધકેલો_selection(RTLIL::Selection::CompleteSelection(આ));
}

શૂન્ય RTLIL::Design::કાઢો_selection()
{
	selection_ઢગલો.કાઢો_back();
	// Default to a full_selection if we ran out of stack
	જો (selection_ઢગલો.empty())
		ધકેલો_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(તાર્કિક only_selected) અચલ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(આ), only_selected, સાચું, ખોટું, RTLIL_BACKEND::DumpMode::Readable);
	પાછા f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) અચલ
{
	તાર્કિક include_partials = partials == RTLIL::SELECT_ALL;
	તાર્કિક exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	તાર્કિક ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> result;
	result.reserve(modules_.size());
	માટે (auto &it : modules_) {
		જો (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			જો (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				result.ધકેલો_back(it.second);
			નહીંતર
				પસંદગી (boxes)
				{
				સ્થિતિ RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.second);
					તોડો;
				સ્થિતિ RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.second);
					તોડો;
				સ્થિતિ RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.second);
					તોડો;
				સ્થિતિ RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.second);
					તોડો;
				સ્થિતિ RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.second);
					તોડો;
				સ્થિતિ RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.second);
					તોડો;
				મૂળભૂત:
					તોડો;
				}
		} નહીંતર જો (!include_partials && selected_module(it.first)) {
			પસંદગી(partials)
			{
			સ્થિતિ RTLIL::SELECT_WHOLE_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.second);
				તોડો;
			સ્થિતિ RTLIL::SELECT_WHOLE_ERR:
				log_error("Unsupported partially selected module %s.\n", it.second);
				તોડો;
			સ્થિતિ RTLIL::SELECT_WHOLE_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.second);
				તોડો;
			મૂળભૂત:
				તોડો;
			}
		}
	}
	પાછા result;
}

RTLIL::Module::Module()
{
	સ્થિર unsigned પૂર્ણાંક hashidx_ગણતરી = 123456789;
	hashidx_ગણતરી = mkhash_xorshift(hashidx_ગણતરી);
	hashidx_ = hashidx_ગણતરી;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->ઉમેરો(std::pair<unsigned પૂર્ણાંક, RTLIL::Module*>(hashidx_, આ));
#endif
}

RTLIL::Module::~Module()
{
	માટે (auto &pr : wires_)
		delete pr.second;
	માટે (auto &pr : memories)
		delete pr.second;
	માટે (auto &pr : cells_)
		delete pr.second;
	માટે (auto &pr : processes)
		delete pr.second;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
સ્થિર std::map<unsigned પૂર્ણાંક, RTLIL::Module*> all_modules;
std::map<unsigned પૂર્ણાંક, RTLIL::Module*> *RTLIL::Module::get_all_modules(શૂન્ય)
{
	પાછા &all_modules;
}
#endif

શૂન્ય RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	માટે (auto it = wires_.begin(); it != wires_.end(); ++it)
		જો (!it->second->port_આગત && !it->second->port_output)
			delwires.ઉમેરો(it->second);

	માટે (auto it = memories.begin(); it != memories.end(); ++it)
		delete it->second;
	memories.clear();

	માટે (auto it = cells_.begin(); it != cells_.end(); ++it)
		delete it->second;
	cells_.clear();

	માટે (auto it = processes.begin(); it != processes.end(); ++it)
		delete it->second;
	processes.clear();

	connections_.clear();

	remove(delwires);
	set_bool_attribute(ID::blackbox);
}

શૂન્ય RTLIL::Module::expand_interfaces(RTLIL::Design *, અચલ dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

તાર્કિક RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	પાછા ખોટું;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, અચલ dict<RTLIL::IdString, RTLIL::Const> &, તાર્કિક mayfail)
{
	જો (mayfail)
		પાછા IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, અચલ dict<RTLIL::IdString, RTLIL::Const> &, અચલ dict<RTLIL::IdString, RTLIL::Module*> &, અચલ dict<RTLIL::IdString, RTLIL::IdString> &, તાર્કિક mayfail)
{
	જો (mayfail)
		પાછા IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

size_t RTLIL::Module::ગણતરી_id(RTLIL::IdString id)
{
	પાછા wires_.ગણતરી(id) + cells_.ગણતરી(id) + memories.ગણતરી(id) + processes.ગણતરી(id);
}

#ifndef NDEBUG
નામાકાશ {
	સંરચના InternalCellChecker
	{
		અચલ RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		InternalCellChecker(અચલ RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		શૂન્ય error(પૂર્ણાંક linenr, std::optional<std::string> error_સંદેશ = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			જો (error_સંદેશ) {
				log_error("%s in internal cell %s\n", *error_સંદેશ, cell_loc);
			} નહીંતર {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		પૂર્ણાંક param(RTLIL::IdString name)
		{
			auto it = cell->parameters.find(name);
			જો (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.ઉમેરો(name);
			પાછા it->second.as_int();
		}

		પૂર્ણાંક param_bool(RTLIL::IdString name)
		{
			પૂર્ણાંક v = param(name);
			જો (GetSize(cell->parameters.at(name)) > 32)
				error(__LINE__);
			જો (v != 0 && v != 1)
				error(__LINE__);
			પાછા v;
		}

		પૂર્ણાંક param_bool(RTLIL::IdString name, તાર્કિક expected)
		{
			પૂર્ણાંક v = param_bool(name);
			જો (v != expected)
				error(__LINE__);
			પાછા v;
		}

		શૂન્ય param_bits(RTLIL::IdString name, પૂર્ણાંક width)
		{
			param(name);
			જો (GetSize(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			પાછા cell->parameters.at(name).decode_string();
		}

		શૂન્ય port(RTLIL::IdString name, પૂર્ણાંક width)
		{
			auto it = cell->connections_.find(name);
			જો (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			જો (GetSize(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.ઉમેરો(name);
		}

		શૂન્ય check_expected(તાર્કિક check_matched_sign = ખોટું)
		{
			માટે (auto &para : cell->parameters)
				જો (expected_params.ગણતરી(para.first) == 0)
					error(__LINE__);
			માટે (auto &conn : cell->connections())
				જો (expected_ports.ગણતરી(conn.first) == 0)
					error(__LINE__);

			જો (check_matched_sign) {
				log_assert(expected_params.ગણતરી(ID::A_SIGNED) != 0 && expected_params.ગણતરી(ID::B_SIGNED) != 0);
				તાર્કિક a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				તાર્કિક b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				જો (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		શૂન્ય check()
		{
			જો (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				પાછા;

			જો (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(સાચું);
				પાછા;
			}

			જો (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/ખોટું);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/ખોટું);
				પાછા;
			}

			જો (cell->type.in(ID($shift), ID($shiftx))) {
				જો (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/ખોટું);
				} નહીંતર {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/ખોટું);
				પાછા;
			}

			જો (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(સાચું);
				પાછા;
			}

			જો (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($pow));
				પાછા;
			}

			જો (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::CI, 1);
				port(ID::BI, 1);
				port(ID::X, param(ID::Y_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				port(ID::CO, param(ID::Y_WIDTH));
				check_expected(સાચું);
				પાછા;
			}

			જો (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				પાછા;
			}

			જો (cell->type == ID($macc_v2)) {
				જો (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				જો (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				જો (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				અચલ Const &a_width = cell->getParam(ID::A_WIDTHS);
				અચલ Const &b_width = cell->getParam(ID::B_WIDTHS);
				અચલ Const &c_width = cell->getParam(ID::C_WIDTHS);
				પૂર્ણાંક a_width_sum = 0, b_width_sum = 0, c_width_sum = 0;
				માટે (પૂર્ણાંક i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_sum += a_width.extract(16 * i, 16).as_int(ખોટું);
					b_width_sum += b_width.extract(16 * i, 16).as_int(ખોટું);
				}
				માટે (પૂર્ણાંક i = 0; i < param(ID::NADDENDS); i++) {
					c_width_sum += c_width.extract(16 * i, 16).as_int(ખોટું);
				}
				port(ID::A, a_width_sum);
				port(ID::B, b_width_sum);
				port(ID::C, c_width_sum);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/ખોટું);
				પાછા;
			}

			જો (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				જો (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($dffsre)) {
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
				પાછા;
			}

			જો (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_કિંમત, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_કિંમત, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type.in(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_કિંમત, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_કિંમત, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($aldffe)) {
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
				પાછા;
			}

			જો (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_કિંમત, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_BITS);
				param(ID::STATE_અંક);
				param(ID::STATE_અંક_LOG2);
				param(ID::STATE_RST);
				param_bits(ID::STATE_TABLE, param(ID::STATE_BITS) * param(ID::STATE_અંક));
				param(ID::TRANS_અંક);
				param_bits(ID::TRANS_TABLE, param(ID::TRANS_અંક) * (2*param(ID::STATE_અંક_LOG2) + param(ID::CTRL_IN_WIDTH) + param(ID::CTRL_OUT_WIDTH)));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::CTRL_IN, param(ID::CTRL_IN_WIDTH));
				port(ID::CTRL_OUT, param(ID::CTRL_OUT_WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_MASK);
				param(ID::COLLISION_X_MASK);
				param_bool(ID::CE_OVER_SRST);
				param_bits(ID::ARST_કિંમત, param(ID::WIDTH));
				param_bits(ID::SRST_કિંમત, param(ID::WIDTH));
				param_bits(ID::INIT_કિંમત, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::SRST, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($memwr_v2)) {
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
				પાછા;
			}

			જો (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::શબ્દો));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::શબ્દો));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($mem)) {
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
				પાછા;
			}

			જો (cell->type == ID($mem_v2)) {
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
				param_bits(ID::RD_ARST_કિંમત, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_SRST_કિંમત, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_INIT_કિંમત, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
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
				પાછા;
			}

			જો (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type.in(ID($પ્રતિજ્ઞા), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				પાછા;
			}

			જો (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				પાછા;
			}

			જો (cell->type.in(ID($specify2), ID($specify3))) {
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
				જો (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
				}
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($specrule)) {
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
				પાછા;
			}

			જો (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				પાછા;
			}

			જો (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				જો (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
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
				પાછા;
			}

			જો (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				જો (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				પાછા;
			}

			જો (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); પાછા; }

			જો (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); પાછા; }

			જો (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); પાછા; }
			જો (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); પાછા; }

			જો (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); પાછા; }

			જો (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  પાછા; }

			જો (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); પાછા; }

			જો (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); પાછા; }

			જો (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); પાછા; }

			જો (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); પાછા; }

			જો (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); પાછા; }

			જો (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); પાછા; }

			જો (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); પાછા; }

			જો (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); પાછા; }

			જો (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); પાછા; }

			જો (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); પાછા; }

			જો (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); પાછા; }

			જો (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); પાછા; }

			જો (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); પાછા; }

			જો (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				પાછા;
			}
			જો (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				પાછા;
			}
			જો (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				પાછા;
			}
			જો (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				પાછા;
			}
			જો (cell->type.in(ID($આગત_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				પાછા;
			}
			જો (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				check_expected();
				પાછા;
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

શૂન્ય RTLIL::Module::sort()
{
	wires_.sort(sort_by_id_str(design->twines()));
	cells_.sort(sort_by_id_str(design->twines()));
	parameter_default_કિંમતો.sort(sort_by_id_str(design->twines()));
	memories.sort(sort_by_id_str(design->twines()));
	processes.sort(sort_by_id_str(design->twines()));
	માટે (auto &it : cells_)
		it.second->sort();
	માટે (auto &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	માટે (auto &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#ifndef NDEBUG
સ્થિર શૂન્ય check_id_ચાવીઓ(અચલ dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	માટે (auto &it : keyed)
		log_assert(!it.first.empty());
}
#endif

શૂન્ય check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ifndef NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->cells_size(), 1000));
	અચલ RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	માટે (auto &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->width >= 0);
		log_assert(it.second->size >= 0);
		check_id_ચાવીઓ(it.second->attributes);
		memory_strings.ઉમેરો(it.second->name.str());
	}

	std::vector<MonotonicFlag> ports_declared(GetSize(module->ports));
	ShardedVector<std::string> memids(subpool);
	subpool.run([const_module, &ports_declared, &memory_strings, &memids](અચલ ParallelDispatchThreadPool::RunCtx &ctx) {
		માટે (પૂર્ણાંક i : ctx.item_range(const_module->cells_size())) {
			auto it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			માટે (auto &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_ચાવીઓ(it.second->attributes);
			check_id_ચાવીઓ(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			જો (it.second->has_memid()) {
				log_assert(memory_strings.ગણતરી(it.second->parameters.at(ID::MEMID).decode_string()));
			} નહીંતર જો (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.ગણતરી(memid));
				memids.ઉમેરો(ctx, std::move(memid));
			}
			auto cell_mod = const_module->design->module(it.second->name);
			જો (cell_mod != nullptr) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#ifdef __GNUC__
				#pragma GCC diagnostic ધકેલો
				#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				#endif
				log_assert(!it.second->get_blackbox_attribute());
				#ifdef __GNUC__
				#pragma GCC diagnostic કાઢો
				#endif
			}
		}

		માટે (પૂર્ણાંક i : ctx.item_range(const_module->wires_size())) {
			auto it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_ચાવીઓ(it.second->attributes);
			જો (it.second->port_id) {
				log_assert(GetSize(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_આગત || it.second->port_output);
				log_assert(it.second->port_id <= GetSize(ports_declared));
				તાર્કિક previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == ખોટું);
			} નહીંતર
				log_assert(!it.second->port_આગત && !it.second->port_output);
		}
	});
	માટે (અચલ MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == સાચું);
	pool<std::string> memids_pool;
	માટે (std::string &memid : memids)
		log_assert(memids_pool.ઉમેરો(memid).second);

	માટે (auto &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->મૂળ_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->મૂળ_case};
		માટે (size_t i = 0; i < all_cases.size(); i++) {
			માટે (auto &switch_it : all_cases[i]->switches) {
				માટે (auto &case_it : switch_it->cases) {
					માટે (auto &compare_it : case_it->compare) {
						log_assert(switch_it->સંકેત.size() == compare_it.size());
					}
					all_cases.ધકેલો_back(case_it);
				}
			}
		}
		માટે (auto &sync_it : it.second->syncs) {
			પસંદગી (sync_it->type) {
				સ્થિતિ RTLIL::SyncType::ST0:
				સ્થિતિ RTLIL::SyncType::ST1:
				સ્થિતિ RTLIL::SyncType::STp:
				સ્થિતિ RTLIL::SyncType::STn:
				સ્થિતિ RTLIL::SyncType::STe:
					log_assert(!sync_it->સંકેત.empty());
					તોડો;
				સ્થિતિ RTLIL::SyncType::STa:
				સ્થિતિ RTLIL::SyncType::STg:
				સ્થિતિ RTLIL::SyncType::STi:
					log_assert(sync_it->સંકેત.empty());
					તોડો;
			}
		}
	}

	માટે (auto &it : module->connections_) {
		log_assert(it.first.size() == it.second.size());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_ચાવીઓ(module->attributes);
#endif
}

શૂન્ય RTLIL::Module::check()
{
	પૂર્ણાંક pool_size = ThreadPool::work_pool_size(0, cells_size(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_size);
	check_module(આ, thread_pool);
}

શૂન્ય RTLIL::Module::optimize()
{
}

શૂન્ય RTLIL::Module::cloneInto(RTLIL::Module *new_mod) અચલ
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	માટે (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_કિંમતો.clear();
	માટે (auto &it : parameter_default_કિંમતો)
		new_mod->parameter_default_કિંમતો[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	માટે (auto &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	માટે (auto it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	માટે (auto it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	માટે (auto it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	માટે (auto it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addProcess(dst_id, it->second);
	}

	સંરચના RewriteSigSpecWorker
	{
		અચલ dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		અચલ RTLIL::Module *new_mod;
		શૂન્ય operator()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([આ](RTLIL::Wire *&wire) {
				auto it = wire_map.find(wire);
				જો (it != wire_map.end())
					wire = it->second;
				નહીંતર
					log_assert(wire->module == new_mod);
			});
		}
	};

	RewriteSigSpecWorker rewriteSigSpecWorker{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewriteSigSpecWorker);
	new_mod->fixup_ports();
}

RTLIL::Module *RTLIL::Module::clone() અચલ
{
	RTLIL::Module *new_mod = નવું RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	પાછા new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) અચલ
{
	RTLIL::Module *new_mod = નવું RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->add(new_mod);
	પાછા new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) અચલ
{
	RTLIL::Module *new_mod = નવું RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	પાછા new_mod;
}

તાર્કિક RTLIL::Module::has_memories() અચલ
{
	પાછા !memories.empty();
}

તાર્કિક RTLIL::Module::has_processes() અચલ
{
	પાછા !processes.empty();
}

તાર્કિક RTLIL::Module::has_memories_warn() અચલ
{
	જો (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", આ);
	પાછા !memories.empty();
}

તાર્કિક RTLIL::Module::has_processes_warn() અચલ
{
	જો (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", આ);
	પાછા !processes.empty();
}

તાર્કિક RTLIL::Module::is_selected() અચલ
{
	પાછા design->selected_module(આ->name);
}

તાર્કિક RTLIL::Module::is_selected_whole() અચલ
{
	પાછા design->selected_whole_module(આ->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() અચલ
{
	std::vector<RTLIL::Wire*> result;
	result.reserve(wires_.size());
	માટે (auto &it : wires_)
		જો (design->selected(આ, it.second))
			result.ધકેલો_back(it.second);
	પાછા result;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() અચલ
{
	std::vector<RTLIL::Cell*> result;
	result.reserve(cells_.size());
	માટે (auto &it : cells_)
		જો (design->selected(આ, it.second))
			result.ધકેલો_back(it.second);
	પાછા result;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() અચલ
{
	std::vector<RTLIL::Memory*> result;
	result.reserve(memories.size());
	માટે (auto &it : memories)
		જો (design->selected(આ, it.second))
			result.ધકેલો_back(it.second);
	પાછા result;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() અચલ
{
	std::vector<RTLIL::Process*> result;
	result.reserve(processes.size());
	માટે (auto &it : processes)
		જો (design->selected(આ, it.second))
			result.ધકેલો_back(it.second);
	પાછા result;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() અચલ
{
	std::vector<RTLIL::SelectedMember> result;
	માટે (auto cell : selected_cells())
		result.ધકેલો_back({cell, cell->name});
	માટે (auto memory : selected_memories())
		result.ધકેલો_back({memory, memory->name});
	માટે (auto wire : selected_wires())
		result.ધકેલો_back({wire, wire->name});
	માટે (auto process : selected_processes())
		result.ધકેલો_back({process, process->name});
	પાછા result;
}

શૂન્ય RTLIL::Module::add(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(ગણતરી_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = આ;
}

શૂન્ય RTLIL::Module::add(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(ગણતરી_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = આ;
}

શૂન્ય RTLIL::Module::add(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(ગણતરી_id(process->name) == 0);
	processes[process->name] = process;
	process->module = આ;
}

શૂન્ય RTLIL::Module::remove(અચલ pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	જો (wires.empty())
		પાછા;

	સંરચના DeleteWireWorker
	{
		RTLIL::Module *module;
		અચલ pool<RTLIL::Wire*> *wires_p;

		શૂન્ય operator()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([આ](RTLIL::Wire *&wire) {
				જો (wires_p->ગણતરી(wire))
					wire = module->addWire(stringf("$delete_wire$%d", (પૂર્ણાંક)autoidx++), wire->width);
			});
		}

		શૂન્ય operator()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	DeleteWireWorker delete_wire_worker;
	delete_wire_worker.module = આ;
	delete_wire_worker.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_worker);

	જો (design->flagBufferedNormalized) {
		માટે (auto wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_index.erase(wire);
		}
	}

	માટે (auto &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.ગણતરી(id) != 0);
		wires_.erase(id);
		delete it;
	}
}

શૂન્ય RTLIL::Module::remove(RTLIL::Cell *cell)
{
	જ્યારે (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.ગણતરી(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	જો (design && design->flagBufferedNormalized && buf_norm_cell_queue.ગણતરી(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.ઉમેરો(cell);
	} નહીંતર {
		delete cell;
	}
}

શૂન્ય RTLIL::Module::remove(RTLIL::Memory *memory)
{
	log_assert(memories.ગણતરી(memory->name) != 0);
	memories.erase(memory->name);
	delete memory;
}

શૂન્ય RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.ગણતરી(process->name) != 0);
	processes.erase(process->name);
	delete process;
}

શૂન્ય RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	add(wire);
}

શૂન્ય RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	add(cell);
}

શૂન્ય RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	જો (old_id != IdString::Null && wires_.ગણતરી(old_id))
		rename(wires_.at(old_id), new_name);
	નહીંતર જો (old_id != IdString::Null && cells_.ગણતરી(old_id))
		rename(cells_.at(old_id), new_name);
	નહીંતર
		log_abort();
}

શૂન્ય RTLIL::Module::swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2)
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

શૂન્ય RTLIL::Module::swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	પૂર્ણાંક index = 0;
	પાછા uniquify(name, index);
}

IdString RTLIL::Module::uniquify(IdString name, પૂર્ણાંક &index)
{
	જો (index == 0) {
		જો (ગણતરી_id(name) == 0)
			પાછા name;
		index++;
	}

	જ્યારે (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", index)});
		જો (ગણતરી_id(new_name) == 0)
			પાછા new_name;
		index++;
	}
}

સ્થિર તાર્કિક fixup_ports_compare(અચલ RTLIL::Wire *a, અચલ RTLIL::Wire *b)
{
	જો (a->port_id && !b->port_id)
		પાછા સાચું;
	જો (!a->port_id && b->port_id)
		પાછા ખોટું;

	જો (a->port_id == b->port_id)
		પાછા a->name < b->name;
	પાછા a->port_id < b->port_id;
}

શૂન્ય RTLIL::Module::connect(અચલ RTLIL::SigSig &conn)
{
	માટે (auto mon : monitors)
		mon->notify_connect(આ, conn);

	જો (design)
		માટે (auto mon : design->monitors)
			mon->notify_connect(આ, conn);

	// ignore all attempts to assign constants to other constants
	જો (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		માટે (પૂર્ણાંક i = 0; i < GetSize(conn.first); i++)
			જો (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		જો (GetSize(new_conn.first))
			connect(new_conn);
		પાછા;
	}

	જો (yosys_xtrace) {
		log("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", આ, log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(GetSize(conn.first) == GetSize(conn.second));
	connections_.ધકેલો_back(conn);
}

શૂન્ય RTLIL::Module::connect(અચલ RTLIL::SigSpec &lhs, અચલ RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

શૂન્ય RTLIL::Module::new_connections(અચલ std::vector<RTLIL::SigSig> &new_conn)
{
	માટે (auto mon : monitors)
		mon->notify_connect(આ, new_conn);

	જો (design)
		માટે (auto mon : design->monitors)
			mon->notify_connect(આ, new_conn);

	જો (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", આ);
		માટે (auto &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

અચલ std::vector<RTLIL::SigSig> &RTLIL::Module::connections() અચલ
{
	પાછા connections_;
}

શૂન્ય RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	માટે (auto &w : wires_)
		જો (w.second->port_આગત || w.second->port_output)
			all_ports.ધકેલો_back(w.second);
		નહીંતર
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	જો (design && design->flagBufferedNormalized) {
		માટે (auto &w : wires_)
			જો (w.second->driverCell_ && w.second->driverCell_->type == ID($આગત_port))
				buf_norm_wire_queue.ઉમેરો(w.second);

		buf_norm_wire_queue.ઉમેરો(all_ports.begin(), all_ports.end());
	}

	ports.clear();
	માટે (size_t i = 0; i < all_ports.size(); i++) {
		ports.ધકેલો_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
	}
}

શૂન્ય RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		અચલ dict<IdString, RTLIL::Const> &src,
		અચલ RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	જો (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		પાછા;
	}
	dst.clear();
	માટે (પૂર્ણાંક i = GetSize(src) - 1; i >= 0; i--) {
		auto &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::addWire(RTLIL::IdString name, પૂર્ણાંક width)
{
	log_assert(design);
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	RTLIL::Wire *wire = નવું RTLIL::Wire;
	wire->width = width;
	wire->name = name;
	add(wire);
	પાછા wire;
}

RTLIL::Wire *RTLIL::Module::addWire(IdString name, અચલ RTLIL::Wire *other)
{
	RTLIL::Wire *wire = addWire(name);
	wire->width = other->width;
	wire->start_offset = other->start_offset;
	wire->port_id = other->port_id;
	wire->port_આગત = other->port_આગત;
	wire->port_output = other->port_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		અચલ RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
		RTLIL::copy_attr_dict(wire->attributes, other->attributes, src_design, આ->design);
	}
	પાછા wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = નવું RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	પાછા cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, અચલ RTLIL::Cell *other)
{
	અચલ RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	તાર્કિક cross_pool = src_design && આ->design && src_design != આ->design;

	IdString type = other->type.ref();
	જો (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, આ->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, આ->design);

	જો (!cross_pool)
		cell->connections_ = other->connections_;
	નહીંતર
		માટે (પૂર્ણાંક i = GetSize(other->connections_) - 1; i >= 0; i--) {
			auto &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	પાછા cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = નવું RTLIL::Memory;
	mem->module = આ;
	mem->name = name;
	memories[name] = mem;
	પાછા mem;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name, અચલ RTLIL::Memory *other)
{
	log_assert(design);
	RTLIL::Memory *mem = નવું RTLIL::Memory;
	mem->module = આ;
	mem->name = name;
	mem->width = other->width;
	mem->start_offset = other->start_offset;
	mem->size = other->size;
	RTLIL::copy_attr_dict(mem->attributes, other->attributes,
			other->module ? other->module->design : nullptr, design);
	memories[name] = mem;
	પાછા mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = નવું RTLIL::Process;
	proc->name = name;
	add(proc);
	પાછા proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, અચલ RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	અચલ RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	જો (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		માટે (auto *સમકાલિક : proc->syncs)
			માટે (auto &mwa : સમકાલિક->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	પાછા proc;
}

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, તાર્કિક is_signed, અચલ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		પાછા cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, તાર્કિક is_signed, અચલ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		પાછા sig_y;                                       \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, તાર્કિક /* is_signed */, અચલ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::WIDTH] = sig_a.size();         \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		પાછા cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, તાર્કિક is_signed, અચલ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		પાછા sig_y;                                       \
	}
DEF_METHOD(Buf,     sig_a.size(), ID($buf))
DEF_METHOD(Barrier, sig_a.size(), ID($barrier))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, તાર્કિક is_signed, અચલ std::string &src) { \
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
		પાછા cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, તાર્કિક is_signed, અચલ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		પાછા sig_y;                                            \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, તાર્કિક is_signed, અચલ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = ખોટું;             \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		પાછા cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, તાર્કિક is_signed, અચલ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		પાછા sig_y;                                            \
	}
DEF_METHOD(Shl,      sig_a.size(), ID($shl))
DEF_METHOD(Shr,      sig_a.size(), ID($shr))
DEF_METHOD(Sshl,     sig_a.size(), ID($sshl))
DEF_METHOD(Sshr,     sig_a.size(), ID($sshr))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, તાર્કિક is_signed, અચલ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = ખોટું;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		પાછા cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, તાર્કિક is_signed, અચલ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		પાછા sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.size(), ID($shiftx))
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_s, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		જો (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.size();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		પાછા cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_s, અચલ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		પાછા sig_y;                                             \
	}
DEF_METHOD(Mux,      ID($mux),        0)
DEF_METHOD(Bwmux,    ID($bwmux),      0)
DEF_METHOD(Pmux,     ID($pmux),       1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_s, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = _demux ? sig_a.size() : sig_y.size(); \
		cell->parameters[ID::S_WIDTH] = sig_s.size();             \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		પાછા cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_s, અચલ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.size() << sig_s.size() : sig_a.size() >> sig_s.size()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		પાછા sig_y;                                             \
	}
DEF_METHOD(Bmux,     ID($bmux),       0)
DEF_METHOD(Demux,    ID($demux),      1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		પાછા cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_s, અચલ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		પાછા sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#undef DEF_METHOD

#define DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, અચલ RTLIL::SigBit &sig1, અચલ RTLIL::SigBit &sig2, અચલ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		પાછા cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, અચલ RTLIL::SigBit &sig1, અચલ std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		પાછા sig2;                                      \
	}
#define DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, અચલ RTLIL::SigBit &sig1, અચલ RTLIL::SigBit &sig2, અચલ RTLIL::SigBit &sig3, અચલ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		પાછા cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, અચલ RTLIL::SigBit &sig1, અચલ RTLIL::SigBit &sig2, અચલ std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		પાછા sig3;                                      \
	}
#define DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, અચલ RTLIL::SigBit &sig1, અચલ RTLIL::SigBit &sig2, અચલ RTLIL::SigBit &sig3, અચલ RTLIL::SigBit &sig4, અચલ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		પાછા cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, અચલ RTLIL::SigBit &sig1, અચલ RTLIL::SigBit &sig2, અચલ RTLIL::SigBit &sig3, અચલ std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		પાછા sig4;                                      \
	}
#define DEF_METHOD_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, અચલ RTLIL::SigBit &sig1, અચલ RTLIL::SigBit &sig2, અચલ RTLIL::SigBit &sig3, અચલ RTLIL::SigBit &sig4, અચલ RTLIL::SigBit &sig5, અચલ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->setPort("\\" #_P5, sig5);                   \
		cell->set_src_attribute(src);                     \
		પાછા cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, અચલ RTLIL::SigBit &sig1, અચલ RTLIL::SigBit &sig2, અચલ RTLIL::SigBit &sig3, અચલ RTLIL::SigBit &sig4, અચલ std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		પાછા sig5;                                           \
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

RTLIL::Cell* RTLIL::Module::addPow(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, તાર્કિક a_signed, તાર્કિક b_signed, અચલ std::string &src)
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
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addFa(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_c, અચલ RTLIL::SigSpec &sig_x, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fa));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::C, sig_c);
	cell->setPort(ID::X, sig_x);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addSlice(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, RTLIL::Const offset, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($slice));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::Y_WIDTH] = sig_y.size();
	cell->parameters[ID::OFFSET] = offset;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addConcat(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($concat));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::B_WIDTH] = sig_b.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, RTLIL::Const lut, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_en, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($પ્રતિજ્ઞા));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_en, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_en, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_en, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_en, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addSr(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr, અચલ RTLIL::SigSpec &sig_q, તાર્કિક set_polarity, તાર્કિક clr_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sr));
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addDff(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, તાર્કિક clk_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addDffe(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, તાર્કિક clk_polarity, તાર્કિક en_polarity, અચલ std::string &src)
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
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addDffsr(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, અચલ RTLIL::SigSpec &sig_q, તાર્કિક clk_polarity, તાર્કિક set_polarity, તાર્કિક clr_polarity, અચલ std::string &src)
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
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addDffsre(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, અચલ RTLIL::SigSpec &sig_q, તાર્કિક clk_polarity, તાર્કિક en_polarity, તાર્કિક set_polarity, તાર્કિક clr_polarity, અચલ std::string &src)
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
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addAdff(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_arst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_કિંમત, તાર્કિક clk_polarity, તાર્કિક arst_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_કિંમત] = arst_કિંમત;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addAdffe(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_arst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_કિંમત, તાર્કિક clk_polarity, તાર્કિક en_polarity, તાર્કિક arst_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_કિંમત] = arst_કિંમત;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addAldff(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_aload, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		અચલ RTLIL::SigSpec &sig_ad, તાર્કિક clk_polarity, તાર્કિક aload_polarity, અચલ std::string &src)
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
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addAldffe(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_aload, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		અચલ RTLIL::SigSpec &sig_ad, તાર્કિક clk_polarity, તાર્કિક en_polarity, તાર્કિક aload_polarity, અચલ std::string &src)
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
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addSdff(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_srst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_કિંમત, તાર્કિક clk_polarity, તાર્કિક srst_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_કિંમત] = srst_કિંમત;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addSdffe(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_srst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_કિંમત, તાર્કિક clk_polarity, તાર્કિક en_polarity, તાર્કિક srst_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_કિંમત] = srst_કિંમત;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addSdffce(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_srst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_કિંમત, તાર્કિક clk_polarity, તાર્કિક en_polarity, તાર્કિક srst_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_કિંમત] = srst_કિંમત;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addDlatch(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, તાર્કિક en_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatch(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_arst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_કિંમત, તાર્કિક en_polarity, તાર્કિક arst_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_કિંમત] = arst_કિંમત;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsr(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, અચલ RTLIL::SigSpec &sig_q, તાર્કિક en_polarity, તાર્કિક set_polarity, તાર્કિક clr_polarity, અચલ std::string &src)
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
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr,
		અચલ RTLIL::SigSpec &sig_q, તાર્કિક set_polarity, તાર્કિક clr_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, તાર્કિક clk_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, તાર્કિક clk_polarity, તાર્કિક en_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addDffsrGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, અચલ RTLIL::SigSpec &sig_q, તાર્કિક clk_polarity, તાર્કિક set_polarity, તાર્કિક clr_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addDffsreGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, અચલ RTLIL::SigSpec &sig_q, તાર્કિક clk_polarity, તાર્કિક en_polarity, તાર્કિક set_polarity, તાર્કિક clr_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addAdffGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_arst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		તાર્કિક arst_કિંમત, તાર્કિક clk_polarity, તાર્કિક arst_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_કિંમત ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addAdffeGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_arst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		તાર્કિક arst_કિંમત, તાર્કિક clk_polarity, તાર્કિક en_polarity, તાર્કિક arst_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_કિંમત ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addAldffGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_aload, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		અચલ RTLIL::SigSpec &sig_ad, તાર્કિક clk_polarity, તાર્કિક aload_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addAldffeGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_aload, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		અચલ RTLIL::SigSpec &sig_ad, તાર્કિક clk_polarity, તાર્કિક en_polarity, તાર્કિક aload_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addSdffGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_srst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		તાર્કિક srst_કિંમત, તાર્કિક clk_polarity, તાર્કિક srst_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_કિંમત ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addSdffeGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_srst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		તાર્કિક srst_કિંમત, તાર્કિક clk_polarity, તાર્કિક en_polarity, તાર્કિક srst_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_કિંમત ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addSdffceGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_srst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		તાર્કિક srst_કિંમત, તાર્કિક clk_polarity, તાર્કિક en_polarity, તાર્કિક srst_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_કિંમત ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, તાર્કિક en_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatchGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_arst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
		તાર્કિક arst_કિંમત, તાર્કિક en_polarity, તાર્કિક arst_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_કિંમત ? '1' : '0'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsrGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, અચલ RTLIL::SigSpec &sig_q, તાર્કિક en_polarity, તાર્કિક set_polarity, તાર્કિક clr_polarity, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, પૂર્ણાંક width, અચલ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	પાછા sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, પૂર્ણાંક width, અચલ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	પાછા sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, પૂર્ણાંક width, અચલ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	પાછા sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, પૂર્ણાંક width, અચલ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	પાછા sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, અચલ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	પાછા sig;
}

RTLIL::SigSpec RTLIL::Module::SetTag(RTLIL::IdString name, અચલ std::string &tag, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_s, અચલ RTLIL::SigSpec &sig_c, અચલ std::string &src)
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
	પાછા sig;
}

RTLIL::Cell* RTLIL::Module::addSetTag(RTLIL::IdString name, અચલ std::string &tag, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_s, અચલ RTLIL::SigSpec &sig_c, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src)
{
	Cell *cell = addCell(name, ID($set_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::SigSpec RTLIL::Module::GetTag(RTLIL::IdString name, અચલ std::string &tag, અચલ RTLIL::SigSpec &sig_a, અચલ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($get_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	પાછા sig;
}

RTLIL::Cell* RTLIL::Module::addOverwriteTag(RTLIL::IdString name, અચલ std::string &tag, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_s, અચલ RTLIL::SigSpec &sig_c, અચલ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($overwrite_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->set_src_attribute(src);
	પાછા cell;
}

RTLIL::SigSpec RTLIL::Module::OriginalTag(RTLIL::IdString name, અચલ std::string &tag, અચલ RTLIL::SigSpec &sig_a, અચલ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($original_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	પાછા sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_e, અચલ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.size());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.size();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	પાછા sig;
}

std::string RTLIL::Module::to_rtlil_str() અચલ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(આ), design, ખોટું, સાચું, ખોટું, RTLIL_BACKEND::DumpMode::Readable);
	પાછા f.str();
}

RTLIL::Wire::Wire()
{
	સ્થિર unsigned પૂર્ણાંક hashidx_ગણતરી = 123456789;
	hashidx_ગણતરી = mkhash_xorshift(hashidx_ગણતરી);
	hashidx_ = hashidx_ગણતરી;

	module = nullptr;
	width = 1;
	start_offset = 0;
	port_id = 0;
	port_આગત = ખોટું;
	port_output = ખોટું;
	upto = ખોટું;
	is_signed = ખોટું;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->ઉમેરો(std::pair<unsigned પૂર્ણાંક, RTLIL::Wire*>(hashidx_, આ));
#endif
}

RTLIL::Wire::~Wire()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#endif
}

std::string RTLIL::Wire::to_rtlil_str() અચલ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", આ, design(), RTLIL_BACKEND::DumpMode::Readable);
	પાછા f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
સ્થિર std::map<unsigned પૂર્ણાંક, RTLIL::Wire*> all_wires;
std::map<unsigned પૂર્ણાંક, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(શૂન્ય)
{
	પાછા &all_wires;
}
#endif

RTLIL::Memory::Memory()
{
	સ્થિર unsigned પૂર્ણાંક hashidx_ગણતરી = 123456789;
	hashidx_ગણતરી = mkhash_xorshift(hashidx_ગણતરી);
	hashidx_ = hashidx_ગણતરી;

	width = 1;
	start_offset = 0;
	size = 0;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->ઉમેરો(std::pair<unsigned પૂર્ણાંક, RTLIL::Memory*>(hashidx_, આ));
#endif
}

std::string RTLIL::Memory::to_rtlil_str() અચલ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", આ, design(), RTLIL_BACKEND::DumpMode::Readable);
	પાછા f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	સ્થિર unsigned પૂર્ણાંક hashidx_ગણતરી = 123456789;
	hashidx_ગણતરી = mkhash_xorshift(hashidx_ગણતરી);
	hashidx_ = hashidx_ગણતરી;
}

std::string RTLIL::Process::to_rtlil_str() અચલ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", આ, design(), RTLIL_BACKEND::DumpMode::Readable);
	પાછા f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	સ્થિર unsigned પૂર્ણાંક hashidx_ગણતરી = 123456789;
	hashidx_ગણતરી = mkhash_xorshift(hashidx_ગણતરી);
	hashidx_ = hashidx_ગણતરી;

	// log("#memtrace# %p\n", this);
	memhasher();

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->ઉમેરો(std::pair<unsigned પૂર્ણાંક, RTLIL::Cell*>(hashidx_, આ));
#endif
}

RTLIL::Cell::~Cell()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#endif
}

std::string RTLIL::Cell::to_rtlil_str() અચલ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", આ, design(), RTLIL_BACKEND::DumpMode::Readable);
	પાછા f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
સ્થિર std::map<unsigned પૂર્ણાંક, RTLIL::Cell*> all_cells;
std::map<unsigned પૂર્ણાંક, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(શૂન્ય)
{
	પાછા &all_cells;
}
#endif

તાર્કિક RTLIL::Cell::hasPort(RTLIL::IdString portname) અચલ
{
	પાછા connections_.ગણતરી(portname) != 0;
}

// bufnorm

અચલ RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) અચલ
{
	પાછા connections_.at(portname);
}

અચલ dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() અચલ
{
	પાછા connections_;
}

તાર્કિક RTLIL::Cell::known() અચલ
{
	જો (yosys_celltypes.cell_known(type))
		પાછા સાચું;
	જો (module && module->design && module->design->module(type))
		પાછા સાચું;
	પાછા ખોટું;
}

તાર્કિક RTLIL::Cell::આગત(IdString portname) અચલ
{
	જો (yosys_celltypes.cell_known(type))
		પાછા yosys_celltypes.cell_આગત(type.ref(), portname);
	જો (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		પાછા w && w->port_આગત;
	}
	પાછા ખોટું;
}

તાર્કિક RTLIL::Cell::output(RTLIL::IdString portname) અચલ
{
	જો (yosys_celltypes.cell_known(type))
		પાછા yosys_celltypes.cell_output(type, portname);
	જો (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		પાછા w && w->port_output;
	}
	પાછા ખોટું;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) અચલ
{
	જો (yosys_celltypes.cell_known(type.ref()))
		પાછા yosys_celltypes.cell_port_dir(type, portname);
	જો (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		જો (m == nullptr)
			પાછા PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		જો (w == nullptr)
			પાછા PortDir::PD_UNKNOWN;
		પાછા PortDir(w->port_આગત + w->port_output * 2);
	}
	પાછા PortDir::PD_UNKNOWN;
}

તાર્કિક RTLIL::Cell::hasParam(RTLIL::IdString paramname) અચલ
{
	પાછા parameters.ગણતરી(paramname) != 0;
}

શૂન્ય RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

શૂન્ય RTLIL::Cell::setParam(RTLIL::IdString paramname, RTLIL::Const કિંમત)
{
	parameters[paramname] = std::move(કિંમત);
}

અચલ RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) અચલ
{
	અચલ auto &it = parameters.find(paramname);
	જો (it != parameters.end())
		પાછા it->second;
	જો (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		જો (m)
			પાછા m->parameter_default_કિંમતો.at(paramname);
	}
	ફેંકો std::out_of_range("Cell::getParam()");
}

શૂન્ય RTLIL::Cell::sort()
{
	connections_.sort(sort_by_id_str(module->twines()));
	parameters.sort(sort_by_id_str(module->twines()));
	attributes.sort(sort_by_id_str(module->twines()));
}

શૂન્ય RTLIL::Cell::check()
{
#ifndef NDEBUG
	InternalCellChecker checker(ખાલી, આ);
	checker.check();
#endif
}

શૂન્ય RTLIL::Cell::fixup_parameters(તાર્કિક set_a_signed, તાર્કિક set_b_signed)
{
	જો (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		પાછા;

	જો (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		જો (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		પાછા;
	}

	જો (type == ID($demux)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		પાછા;
	}

	જો (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		પાછા;
	}

	જો (type == ID($fa)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		પાછા;
	}

	જો (type == ID($lcu)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::CO]);
		પાછા;
	}

	જો (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);
		પાછા;
	}

	તાર્કિક signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	જો (connections_.ગણતરી(ID::A)) {
		જો (signedness_ab) {
			જો (set_a_signed)
				parameters[ID::A_SIGNED] = સાચું;
			નહીંતર જો (parameters.ગણતરી(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = ખોટું;
		}
		parameters[ID::A_WIDTH] = GetSize(connections_[ID::A]);
	}

	જો (connections_.ગણતરી(ID::B)) {
		જો (signedness_ab) {
			જો (set_b_signed)
				parameters[ID::B_SIGNED] = સાચું;
			નહીંતર જો (parameters.ગણતરી(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = ખોટું;
		}
		parameters[ID::B_WIDTH] = GetSize(connections_[ID::B]);
	}

	જો (connections_.ગણતરી(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);

	જો (connections_.ગણતરી(ID::Q))
		parameters[ID::WIDTH] = GetSize(connections_[ID::Q]);

	check();
}

તાર્કિક RTLIL::Cell::has_keep_attr() અચલ {
	પાછા get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

તાર્કિક RTLIL::Cell::has_memid() અચલ
{
	પાછા type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

તાર્કિક RTLIL::Cell::is_mem_cell() અચલ
{
	પાછા type.in(ID($mem), ID($mem_v2)) || has_memid();
}

તાર્કિક RTLIL::Cell::is_builtin_ff() અચલ {
	પાછા StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(અચલ RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	જો (wire == ખાલી)
		data = {bit.data};
	નહીંતર
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(પૂર્ણાંક offset, પૂર્ણાંક length) અચલ
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= width);
	RTLIL::SigChunk ret;
	જો (wire) {
		ret.wire = wire;
		ret.offset = આ->offset + offset;
		ret.width = length;
	} નહીંતર {
		માટે (પૂર્ણાંક i = 0; i < length; i++)
			ret.data.ધકેલો_back(data[offset+i]);
		ret.width = length;
	}
	પાછા ret;
}

RTLIL::SigBit RTLIL::SigChunk::operator[](પૂર્ણાંક offset) અચલ
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	જો (wire) {
		ret.wire = wire;
		ret.offset = આ->offset + offset;
	} નહીંતર {
		ret.data = data[offset];
	}
	પાછા ret;
}

તાર્કિક RTLIL::SigChunk::operator <(અચલ RTLIL::SigChunk &other) અચલ
{
	જો (wire && other.wire)
		જો (wire->name != other.wire->name)
			પાછા wire->name < other.wire->name;

	જો (wire != other.wire)
		પાછા wire < other.wire;

	જો (offset != other.offset)
		પાછા offset < other.offset;

	જો (width != other.width)
		પાછા width < other.width;

	પાછા data < other.data;
}

તાર્કિક RTLIL::SigChunk::operator ==(અચલ RTLIL::SigChunk &other) અચલ
{
	પાછા wire == other.wire && width == other.width && offset == other.offset && data == other.data;
}

તાર્કિક RTLIL::SigChunk::operator !=(અચલ RTLIL::SigChunk &other) અચલ
{
	જો (*આ == other)
		પાછા ખોટું;
	પાછા સાચું;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.size() > 0);
	auto ie = parts.begin();
	auto it = ie + parts.size() - 1;
	જ્યારે (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(અચલ RTLIL::Const &કિંમત)
{
	જો (GetSize(કિંમત) != 0) {
		rep_ = CHUNK;
		નવું (&chunk_) RTLIL::SigChunk(કિંમત);
	} નહીંતર {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&કિંમત)
{
	જો (GetSize(કિંમત) != 0) {
		rep_ = CHUNK;
		નવું (&chunk_) RTLIL::SigChunk(કિંમત);
	} નહીંતર {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(અચલ RTLIL::SigChunk &chunk)
{
	જો (chunk.width != 0) {
		rep_ = CHUNK;
		નવું (&chunk_) RTLIL::SigChunk(chunk);
	} નહીંતર {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	જો (chunk.width != 0) {
		rep_ = CHUNK;
		નવું (&chunk_) RTLIL::SigChunk(chunk);
	} નહીંતર {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	જો (wire->width != 0) {
		rep_ = CHUNK;
		નવું (&chunk_) RTLIL::SigChunk(wire);
	} નહીંતર {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, પૂર્ણાંક offset, પૂર્ણાંક width)
{
	જો (width != 0) {
		rep_ = CHUNK;
		નવું (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} નહીંતર {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(અચલ std::string &str)
{
	જો (str.size() != 0) {
		rep_ = CHUNK;
		નવું (&chunk_) RTLIL::SigChunk(str);
	} નહીંતર {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(પૂર્ણાંક val, પૂર્ણાંક width)
{
	જો (width != 0) {
		rep_ = CHUNK;
		નવું (&chunk_) RTLIL::SigChunk(val, width);
	} નહીંતર
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, પૂર્ણાંક width)
{
	જો (width != 0) {
		rep_ = CHUNK;
		નવું (&chunk_) RTLIL::SigChunk(bit, width);
	} નહીંતર
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(અચલ RTLIL::SigBit &bit, પૂર્ણાંક width)
{
	જો (width != 0) {
		જો (bit.wire == ખાલી) {
			rep_ = CHUNK;
			નવું (&chunk_) RTLIL::SigChunk(bit.data, width);
		} નહીંતર જો (width == 1) {
			rep_ = CHUNK;
			નવું (&chunk_) RTLIL::SigChunk(bit);
		} નહીંતર {
			init_empty_bits();
			bits_.reserve(width);
			માટે (પૂર્ણાંક i = 0; i < width; i++)
				bits_.ધકેલો_back(bit);
		}
	} નહીંતર
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(અચલ std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	માટે (અચલ auto &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(અચલ std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	માટે (અચલ auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(અચલ pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	માટે (અચલ auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(અચલ std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	માટે (અચલ auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(તાર્કિક bit)
{
	rep_ = CHUNK;
	નવું (&chunk_) RTLIL::SigChunk(bit ? RTLIL::S1 : RTLIL::S0);
	check();
}

શૂન્ય RTLIL::SigSpec::Chunks::const_iterator::next_chunk_bits()
{
	પૂર્ણાંક bits_size = GetSize(spec.bits_);
	જો (bit_index >= bits_size)
		પાછા;
	પૂર્ણાંક i = bit_index;
	અચલ SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.data.clear();
	જો (bit.is_wire()) {
		chunk.offset = bit.offset;
		જ્યારે (i < bits_size && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_index)
			++i;
	} નહીંતર {
		chunk.offset = 0;
		chunk.data.ધકેલો_back(bit.data);
		જ્યારે (i < bits_size && !spec.bits_[i].is_wire()) {
			chunk.data.ધકેલો_back(spec.bits_[i].data);
			++i;
		}
	}
	chunk.width = i - bit_index;
}

શૂન્ય RTLIL::SigSpec::unpack()
{
	જો (rep_ == BITS)
		પાછા;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	માટે (પૂર્ણાંક i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	નવું (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

શૂન્ય RTLIL::SigSpec::try_repack()
{
	જો (rep_ != BITS)
		પાછા;

	પૂર્ણાંક bits_size = GetSize(bits_);
	જો (bits_size == 0)
		પાછા;
	જો (bits_[0].is_wire()) {
		માટે (પૂર્ણાંક i = 1; i < bits_size; i++)
			જો (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				પાછા;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_size);
		bits_.~vector();
		rep_ = CHUNK;
		નવું (&chunk_) SigChunk(std::move(chunk));
		પાછા;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_size);
	bits.ધકેલો_back(bits_[0].data);
	માટે (પૂર્ણાંક i = 1; i < bits_size; i++) {
		જો (bits_[i].is_wire())
			પાછા;
		bits.ધકેલો_back(bits_[i].data);
	}
	bits_.~vector();
	rep_ = CHUNK;
	નવું (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() અચલ
{
	Hasher h;
	માટે (auto &c : chunks())
		જો (c.wire == ખાલી) {
			માટે (auto &v : c.data)
				h.eat(v);
		} નહીંતર {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t result = h.yield();
	જો (result == 0)
		result = 1;
	hash_.set(result);
	પાછા result;
}

શૂન્ય RTLIL::SigSpec::sort()
{
	unpack();
	std::sort(bits_.begin(), bits_.end());
	hash_.clear();
	try_repack();
}

શૂન્ય RTLIL::SigSpec::sort_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::vector<SigBit> unique_bits = bits_;
	std::sort(unique_bits.begin(), unique_bits.end());
	auto last = std::unique(unique_bits.begin(), unique_bits.end());
	unique_bits.erase(last, unique_bits.end());

	*આ = unique_bits;
	hash_.clear();
	try_repack();
}

શૂન્ય RTLIL::SigSpec::replace(અચલ RTLIL::SigSpec &pattern, અચલ RTLIL::SigSpec &with)
{
	replace(pattern, with, આ);
}

શૂન્ય RTLIL::SigSpec::replace(અચલ RTLIL::SigSpec &pattern, અચલ RTLIL::SigSpec &with, RTLIL::SigSpec *other) અચલ
{
	log_assert(other != ખાલી);
	log_assert(size() == other->size());
	log_assert(pattern.size() == with.size());

	dict<RTLIL::SigBit, પૂર્ણાંક> pattern_to_with;
	પૂર્ણાંક pattern_size = pattern.size();
	માટે (પૂર્ણાંક i = 0; i < pattern_size; i++) {
		SigBit pattern_bit = pattern[i];
		જો (pattern_bit.wire != ખાલી) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	પૂર્ણાંક this_size = size();
	તાર્કિક other_modified = ખોટું;
	માટે (પૂર્ણાંક j = 0; j < this_size; j++) {
		auto it = pattern_to_with.find((*આ)[j]);
		જો (it != pattern_to_with.end()) {
			other_modified = સાચું;
			(*other)[j] = with[it->second];
		}
	}

	જો (other_modified)
		other->try_repack();
	other->check();
}

શૂન્ય RTLIL::SigSpec::replace(અચલ dict<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, આ);
}

શૂન્ય RTLIL::SigSpec::replace(અચલ dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) અચલ
{
	log_assert(other != ખાલી);
	log_assert(size() == other->size());

	જો (rules.empty()) પાછા;

	પૂર્ણાંક this_size = size();
	તાર્કિક other_modified = ખોટું;
	માટે (પૂર્ણાંક i = 0; i < this_size; i++) {
		auto it = rules.find((*આ)[i]);
		જો (it != rules.end()) {
			other_modified = સાચું;
			(*other)[i] = it->second;
		}
	}

	જો (other_modified)
		other->try_repack();
	other->check();
}

શૂન્ય RTLIL::SigSpec::replace(અચલ std::map<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, આ);
}

શૂન્ય RTLIL::SigSpec::replace(અચલ std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) અચલ
{
	log_assert(other != ખાલી);
	log_assert(size() == other->size());

	જો (rules.empty()) પાછા;

	પૂર્ણાંક this_size = size();
	તાર્કિક other_modified = ખોટું;
	માટે (પૂર્ણાંક i = 0; i < this_size; i++) {
		auto it = rules.find((*આ)[i]);
		જો (it != rules.end()) {
			other_modified = સાચું;
			(*other)[i] = it->second;
		}
	}

	જો (other_modified)
		other->try_repack();
	other->check();
}

શૂન્ય RTLIL::SigSpec::remove(અચલ RTLIL::SigSpec &pattern)
{
	remove2(pattern, ખાલી);
}

શૂન્ય RTLIL::SigSpec::remove(અચલ RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) અચલ
{
	RTLIL::SigSpec tmp = *આ;
	tmp.remove2(pattern, other);
}

શૂન્ય RTLIL::SigSpec::remove2(અચલ RTLIL::SigSpec &pattern, RTLIL::SigSpec *other)
{
	unpack();
	જો (other != ખાલી) {
		log_assert(size() == other->size());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.size());
	માટે (auto &bit : pattern)
		જો (bit.wire != ખાલી)
			pattern_bits.ઉમેરો(bit);

	// Compact in-place to avoid O(n^2) erase operations
	size_t write_idx = 0;
	માટે (size_t read_idx = 0; read_idx < bits_.size(); read_idx++)
	{
		જો (!(bits_[read_idx].wire != ખાલી && pattern_bits.ગણતરી(bits_[read_idx]))) {
			જો (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				જો (other != ખાલી)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	તાર્કિક modified = (write_idx < bits_.size());
	જો (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	જો (other != ખાલી && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

શૂન્ય RTLIL::SigSpec::remove(અચલ pool<RTLIL::SigBit> &pattern)
{
	remove2(pattern, ખાલી);
}

શૂન્ય RTLIL::SigSpec::remove(અચલ pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) અચલ
{
	RTLIL::SigSpec tmp = *આ;
	tmp.remove2(pattern, other);
}

શૂન્ય RTLIL::SigSpec::remove2(અચલ pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	જો (other != ખાલી) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	માટે (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		જો (!(bits_[read_idx].wire != ખાલી && pattern.ગણતરી(bits_[read_idx]))) {
			જો (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				જો (other != ખાલી)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	તાર્કિક modified = (write_idx < bits_.size());
	જો (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	જો (other != ખાલી && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

શૂન્ય RTLIL::SigSpec::remove2(અચલ std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	જો (other != ખાલી) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	માટે (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		જો (!(bits_[read_idx].wire != ખાલી && pattern.ગણતરી(bits_[read_idx]))) {
			જો (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				જો (other != ખાલી)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	તાર્કિક modified = (write_idx < bits_.size());
	જો (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	જો (other != ખાલી && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

શૂન્ય RTLIL::SigSpec::remove2(અચલ pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	જો (other != ખાલી) {
		log_assert(size() == other->size());
		other->unpack();
	}

	તાર્કિક modified = ખોટું;
	તાર્કિક other_modified = ખોટું;
	માટે (પૂર્ણાંક i = GetSize(bits_) - 1; i >= 0; i--) {
		જો (bits_[i].wire != ખાલી && pattern.ગણતરી(bits_[i].wire)) {
			modified = સાચું;
			bits_.erase(bits_.begin() + i);
			જો (other != ખાલી) {
				other_modified = સાચું;
				other->bits_.erase(other->bits_.begin() + i);
			}
		}
	}

	જો (modified) {
		hash_.clear();
		try_repack();
	}
	જો (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(અચલ RTLIL::SigSpec &pattern, અચલ RTLIL::SigSpec *other) અચલ
{
	log_assert(other == ખાલી || size() == other->size());

	RTLIL::SigSpec ret;
	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();

	માટે (auto& pattern_chunk : pattern.chunks()) {
		જો (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			પૂર્ણાંક i = 0;
			માટે (અચલ RTLIL::SigBit &bit : bits_match) {
				જો (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} નહીંતર {
			માટે (અચલ RTLIL::SigBit &bit : bits_match)
				જો (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	પાછા ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(અચલ pool<RTLIL::SigBit> &pattern, અચલ RTLIL::SigSpec *other) અચલ
{
	log_assert(other == ખાલી || size() == other->size());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	જો (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		પૂર્ણાંક i = 0;
		માટે (અચલ RTLIL::SigBit &bit : bits_match) {
			જો (bit.wire && pattern.ગણતરી(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} નહીંતર {
		માટે (અચલ RTLIL::SigBit &bit : bits_match)
			જો (bit.wire && pattern.ગણતરી(bit))
				ret.append(bit);
	}

	ret.check();
	પાછા ret;
}

શૂન્ય RTLIL::SigSpec::replace(પૂર્ણાંક offset, અચલ RTLIL::SigSpec &with)
{
	જો (with.size() == 0)
		પાછા;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.size() >= 0);
	log_assert(offset+with.size() <= size());

	પૂર્ણાંક i = 0;
	માટે (અચલ RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

શૂન્ય RTLIL::SigSpec::remove_const()
{
	જો (rep_ == CHUNK)
	{
		જો (chunk_.wire == ખાલી) {
			chunk_.~SigChunk();
			init_empty_bits();
			hash_.clear();
		}
	}
	નહીંતર
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.size());
		માટે (auto &bit : bits_)
			જો (bit.wire != ખાલી)
				new_bits.ધકેલો_back(bit);
		જો (GetSize(new_bits) != GetSize(bits_)) {
			bits_.swap(new_bits);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

શૂન્ય RTLIL::SigSpec::remove(પૂર્ણાંક offset, પૂર્ણાંક length)
{
	જો (length == 0)
		પાછા;

	unpack();

	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	bits_.erase(bits_.begin() + offset, bits_.begin() + offset + length);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(પૂર્ણાંક offset, પૂર્ણાંક length) અચલ
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	std::vector<SigBit> extracted;
	SigBit first;
	તાર્કિક is_packing = સાચું;
	માટે (પૂર્ણાંક i = offset; i < offset + length; i++) {
		તાર્કિક was_packing_before = is_packing;
		SigBit bit = (*આ)[i];
		જો (i == offset) {
			first = bit;
			જો (!bit.wire)
				is_packing = ખોટું;
		} નહીંતર {
			જો (bit.wire != first.wire)
				is_packing = ખોટું;
			જો (bit.wire)
				જો (bit.offset != first.offset + (i - offset))
					is_packing = ખોટું;
		}
		જો (was_packing_before && !is_packing)
			માટે (પૂર્ણાંક j = offset; j < i; j++)
				extracted.ધકેલો_back((*આ)[j]);
		જો (!is_packing)
			extracted.ધકેલો_back((*આ)[i]);
	}
	જો (is_packing)
		પાછા SigChunk(first.wire, first.offset, length);

	પાછા extracted;
}

શૂન્ય RTLIL::SigSpec::rewrite_wires(std::function<શૂન્ય(RTLIL::Wire*& wire)> rewrite)
{
	જો (rep_ == CHUNK) {
		જો (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		પાછા;
	}

	std::vector<RTLIL::SigBit> new_bits;
	માટે (અચલ RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		જો (c.wire != nullptr)
			rewrite(c.wire);
		માટે (પૂર્ણાંક i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.clear();
}

શૂન્ય RTLIL::SigSpec::append(અચલ RTLIL::SigSpec &સંકેત)
{
	જો (સંકેત.size() == 0)
		પાછા;

	જો (size() == 0) {
		*આ = સંકેત;
		પાછા;
	}

	hash_.clear();
	જો (rep_ == CHUNK && સંકેત.rep_ == CHUNK && chunk_.wire == સંકેત.chunk_.wire) {
		જો (chunk_.wire == ખાલી) {
			chunk_.data.ઉમેરો(chunk_.data.end(), સંકેત.chunk_.data.begin(), સંકેત.chunk_.data.end());
			chunk_.width = GetSize(chunk_.data);
			પાછા;
		}
		જો (chunk_.offset + chunk_.width == સંકેત.chunk_.offset) {
			chunk_.width += સંકેત.chunk_.width;
			પાછા;
		}
	}

	unpack();
	માટે (અચલ SigBit &bit : સંકેત.bits())
		bits_.ધકેલો_back(bit);
	check();
}

શૂન્ય RTLIL::SigSpec::append(અચલ RTLIL::SigBit &bit)
{
	hash_.clear();

	જો (size() == 0) {
		destroy();
		rep_ = CHUNK;
		નવું (&chunk_) RTLIL::SigChunk(bit);
		પાછા;
	}

	જો (rep_ == CHUNK && chunk_.wire == bit.wire) {
		જો (chunk_.wire == ખાલી) {
			chunk_.data.ધકેલો_back(bit.data);
			chunk_.width++;
			પાછા;
		}
		જો (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			પાછા;
		}
	}

	unpack();

	bits_.ધકેલો_back(bit);
	check();
}

શૂન્ય RTLIL::SigSpec::extend_u0(પૂર્ણાંક width, તાર્કિક is_signed)
{
	જો (size() > width)
		remove(width, size() - width);

	જો (size() < width) {
		RTLIL::SigBit padding = size() > 0 ? (*આ)[size() - 1] : RTLIL::State::Sx;
		જો (!is_signed)
			padding = RTLIL::State::S0;
		જ્યારે (size() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(પૂર્ણાંક અંક) અચલ
{
	RTLIL::SigSpec sig;
	માટે (પૂર્ણાંક i = 0; i < અંક; i++)
		sig.append(*આ);
	પાછા sig;
}

#ifndef NDEBUG
શૂન્ય RTLIL::SigSpec::check(અચલ Module *mod) અચલ
{
	જો (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		જો (chunk_.wire == ખાલી) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.data.size() == (size_t)chunk_.width);
		} નહીંતર {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.data.size() == 0);
			જો (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	નહીંતર જો (size() <= 64)
	{
		જો (mod != nullptr) {
			માટે (અચલ RTLIL::SigBit &bit : bits_)
				જો (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#endif

તાર્કિક RTLIL::SigSpec::operator <(અચલ RTLIL::SigSpec &other) અચલ
{
	જો (આ == &other)
		પાછા ખોટું;

	જો (size() != other.size())
		પાછા size() < other.size();

	auto other_it = other.chunks().begin();
	માટે (અચલ SigChunk &c : chunks()) {
		જો (c != *other_it)
			પાછા c < *other_it;
		++other_it;
	}

	પાછા ખોટું;
}

તાર્કિક RTLIL::SigSpec::operator ==(અચલ RTLIL::SigSpec &other) અચલ
{
	જો (આ == &other)
		પાછા સાચું;

	જો (size() != other.size())
		પાછા ખોટું;

	auto other_it = other.chunks().begin();
	માટે (અચલ SigChunk &c : chunks()) {
		જો (c != *other_it)
			પાછા ખોટું;
		++other_it;
	}

	પાછા સાચું;
}

તાર્કિક RTLIL::SigSpec::is_wire() અચલ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	જો (it == cs.end())
		પાછા ખોટું;
	અચલ RTLIL::SigChunk &chunk = *it;
	પાછા chunk.wire && chunk.wire->width == size() && ++it == cs.end();
}

તાર્કિક RTLIL::SigSpec::is_chunk() અચલ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	જો (it == cs.end())
		પાછા ખોટું;
	પાછા ++it == cs.end();
}

તાર્કિક RTLIL::SigSpec::known_driver() અચલ
{
	માટે (auto &chunk : chunks())
		જો (chunk.is_wire() && !chunk.wire->known_driver())
			પાછા ખોટું;
	પાછા સાચું;
}

તાર્કિક RTLIL::SigSpec::is_fully_const() અચલ
{
	માટે (auto &chunk : chunks())
		જો (chunk.width > 0 && chunk.wire != ખાલી)
			પાછા ખોટું;
	પાછા સાચું;
}

તાર્કિક RTLIL::SigSpec::is_fully_zero() અચલ
{
	માટે (auto &chunk : chunks()) {
		જો (chunk.width > 0 && chunk.wire != ખાલી)
			પાછા ખોટું;
		માટે (RTLIL::State d : chunk.data)
			જો (d != RTLIL::State::S0)
				પાછા ખોટું;
	}
	પાછા સાચું;
}

તાર્કિક RTLIL::SigSpec::is_fully_ones() અચલ
{
	માટે (auto &chunk : chunks()) {
		જો (chunk.width > 0 && chunk.wire != ખાલી)
			પાછા ખોટું;
		માટે (RTLIL::State d : chunk.data)
			જો (d != RTLIL::State::S1)
				પાછા ખોટું;
	}
	પાછા સાચું;
}

તાર્કિક RTLIL::SigSpec::is_fully_def() અચલ
{
	માટે (auto &chunk : chunks()) {
		જો (chunk.width > 0 && chunk.wire != ખાલી)
			પાછા ખોટું;
		માટે (RTLIL::State d : chunk.data)
			જો (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				પાછા ખોટું;
	}
	પાછા સાચું;
}

તાર્કિક RTLIL::SigSpec::is_fully_undef() અચલ
{
	માટે (auto &chunk : chunks()) {
		જો (chunk.width > 0 && chunk.wire != ખાલી)
			પાછા ખોટું;
		માટે (RTLIL::State d : chunk.data)
			જો (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				પાછા ખોટું;
	}
	પાછા સાચું;
}

તાર્કિક RTLIL::SigSpec::has_const() અચલ
{
	માટે (auto &chunk : chunks())
		જો (chunk.width > 0 && chunk.wire == ખાલી)
			પાછા સાચું;
	પાછા ખોટું;
}

તાર્કિક RTLIL::SigSpec::has_const(State state) અચલ
{
	માટે (auto &chunk : chunks())
		જો (chunk.width > 0 && chunk.wire == ખાલી && std::find(chunk.data.begin(), chunk.data.end(), state) != chunk.data.end())
			પાછા સાચું;
	પાછા ખોટું;
}


તાર્કિક RTLIL::SigSpec::has_marked_bits() અચલ
{
	માટે (auto &chunk : chunks())
		જો (chunk.width > 0 && chunk.wire == ખાલી) {
			માટે (RTLIL::State d : chunk.data)
				જો (d == RTLIL::State::Sm)
					પાછા સાચું;
		}
	પાછા ખોટું;
}

તાર્કિક RTLIL::SigSpec::is_onehot(પૂર્ણાંક *pos) અચલ
{
	જો (std::optional<RTLIL::Const> c = try_as_const())
		પાછા c->is_onehot(pos);
	પાછા ખોટું;
}

તાર્કિક RTLIL::SigSpec::as_bool() અચલ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_કિંમત());
	પાછા c->as_bool();
}

પૂર્ણાંક RTLIL::SigSpec::as_int(તાર્કિક is_signed) અચલ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_કિંમત());
	પાછા c->as_int(is_signed);
}

તાર્કિક RTLIL::SigSpec::convertible_to_int(તાર્કિક is_signed) અચલ
{
	std::optional<RTLIL::Const> c = try_as_const();
	જો (!c.has_કિંમત())
		પાછા ખોટું;
	પાછા c->convertible_to_int(is_signed);
}

std::optional<પૂર્ણાંક> RTLIL::SigSpec::try_as_int(તાર્કિક is_signed) અચલ
{
	std::optional<RTLIL::Const> c = try_as_const();
	જો (!c.has_કિંમત())
		પાછા std::nullopt;
	પાછા c->try_as_int(is_signed);
}

પૂર્ણાંક RTLIL::SigSpec::as_int_saturating(તાર્કિક is_signed) અચલ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_કિંમત());
	પાછા c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() અચલ
{
	std::string str;
	str.reserve(size());
	std::vector<RTLIL::SigChunk> chunks = *આ;
	માટે (size_t i = chunks.size(); i > 0; i--) {
		અચલ RTLIL::SigChunk &chunk = chunks[i-1];
		જો (chunk.wire != ખાલી)
			str.append(chunk.width, '?');
		નહીંતર
			str += RTLIL::Const(chunk.data).as_string();
	}
	પાછા str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() અચલ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	જો (it == cs.end())
		પાછા RTLIL::Const();
	SigChunk chunk = *it;
	જો (chunk.wire != ખાલી || ++it != cs.end())
		પાછા std::nullopt;
	પાછા RTLIL::Const(std::move(chunk.data));
}

RTLIL::Const RTLIL::SigSpec::as_const() અચલ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_કિંમત());
	પાછા *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() અચલ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == size());
	પાછા chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() અચલ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	પાછા chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() અચલ
{
	પાછા RTLIL::SigBit(*આ);
}

તાર્કિક RTLIL::SigSpec::match(અચલ અક્ષર* pattern) અચલ
{
	પૂર્ણાંક pattern_len = strlen(pattern);
	log_assert(pattern_len == size());

	માટે (પૂર્ણાંક i = 0; i < pattern_len; ++i) {
		અક્ષર ch = pattern[i];
		જો (ch == ' ')
			ચાલુ;
		RTLIL::SigBit bit = (*આ)[pattern_len - 1 - i];
		જો (ch == '*') {
			જો (bit != State::Sz && bit != State::Sx)
				પાછા ખોટું;
			ચાલુ;
		}
		જો (ch == '0') {
			જો (bit != State::S0)
				પાછા ખોટું;
		} નહીંતર
		જો (ch == '1') {
			જો (bit != State::S1)
				પાછા ખોટું;
		} નહીંતર
			log_abort();
	}

	પાછા સાચું;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() અચલ
{
	std::set<RTLIL::SigBit> sigbits;
	માટે (auto &c : chunks())
		માટે (પૂર્ણાંક i = 0; i < c.width; i++)
			sigbits.ઉમેરો(RTLIL::SigBit(c, i));
	પાછા sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() અચલ
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(size());
	માટે (auto &c : chunks())
		માટે (પૂર્ણાંક i = 0; i < c.width; i++)
			sigbits.ઉમેરો(RTLIL::SigBit(c, i));
	પાછા sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() અચલ
{
	std::vector<RTLIL::SigBit> result;
	result.reserve(size());
	માટે (SigBit bit : *આ)
		result.ધકેલો_back(bit);
	પાછા result;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(અચલ RTLIL::SigSpec &other) અચલ
{
	પૂર્ણાંક this_size = size();
	log_assert(this_size == other.size());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	માટે (પૂર્ણાંક i = 0; i < this_size; i++)
		new_map[(*આ)[i]] = other[i];

	પાછા new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(અચલ RTLIL::SigSpec &other) અચલ
{
	પૂર્ણાંક this_size = size();
	log_assert(this_size == other.size());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_size);
	માટે (પૂર્ણાંક i = 0; i < this_size; i++)
		new_map[(*આ)[i]] = other[i];

	પાછા new_map;
}

સ્થિર શૂન્ય sigspec_parse_split(std::vector<std::string> &tokens, અચલ std::string &text, અક્ષર sep)
{
	size_t start = 0, end = 0;
	જ્યારે ((end = text.find(sep, start)) != std::string::npos) {
		tokens.ધકેલો_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.ધકેલો_back(text.substr(start));
}

સ્થિર RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, અચલ std::string &netname)
{
	પાછા module->wire(netname);
}

તાર્કિક RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	માટે (પૂર્ણાંક tokidx = પૂર્ણાંક(tokens.size())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		જો (netname.size() == 0)
			ચાલુ;

		જો (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			auto ast = p.const2ast(netname);
			જો (ast == nullptr)
				પાછા ખોટું;
			sig.append(RTLIL::Const(ast->bits));
			ચાલુ;
		}

		જો (module == ખાલી)
			પાછા ખોટું;

		જો (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		જો (sigspec_parse_wire(module, netname) == nullptr) {
			size_t indices_pos = netname.size()-1;
			જો (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				જ્યારે (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				જો (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					જ્યારે (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				જો (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		જો (wire == nullptr)
			પાછા ખોટું;

		જો (!indices.empty()) {
			std::vector<std::string> index_tokens;
			sigspec_parse_split(index_tokens, indices.substr(1, indices.size()-2), ':');
			જો (index_tokens.size() == 1) {
				પૂર્ણાંક a = atoi(index_tokens.at(0).c_str());
				જો (a < 0 || a >= wire->width)
					પાછા ખોટું;
				sig.append(RTLIL::SigSpec(wire, a));
			} નહીંતર {
				પૂર્ણાંક a = atoi(index_tokens.at(0).c_str());
				પૂર્ણાંક b = atoi(index_tokens.at(1).c_str());
				જો (a > b) {
					પૂર્ણાંક tmp = a;
					a = b, b = tmp;
				}
				જો (a < 0 || a >= wire->width)
					પાછા ખોટું;
				જો (b < 0 || b >= wire->width)
					પાછા ખોટું;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} નહીંતર
			sig.append(wire);
	}

	પાછા સાચું;
}

તાર્કિક RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	જો (str.empty() || str[0] != '@')
		પાછા parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	જો (sel_name == IdString::Null || design->selection_vars.ગણતરી(sel_name) == 0)
		પાછા ખોટું;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	માટે (auto &it : module->wires_)
		જો (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	પાછા સાચું;
}

તાર્કિક RTLIL::SigSpec::parse_rhs(અચલ RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	જો (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.size());
		પાછા સાચું;
	}

	જો (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.size());
		પાછા સાચું;
	}

	જો (lhs.is_chunk()) {
		અક્ષર *p = (અક્ષર*)str.c_str(), *endptr;
		long પૂર્ણાંક val = strtol(p, &endptr, 10);
		જો (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.size());
			પાછા સાચું;
		}
	}

	જો (!parse(sig, module, str))
		પાછા ખોટું;
	જો (sig.size() > lhs.size())
		sig.remove(lhs.size(), sig.size() - lhs.size());
	પાછા સાચું;
}

RTLIL::SigSpec::operator std::vector<RTLIL::SigChunk>() અચલ
{
	std::vector<RTLIL::SigChunk> result;
	માટે (અચલ RTLIL::SigChunk &c : chunks())
		result.ધકેલો_back(c);
	પાછા result;
}

RTLIL::CaseRule::~CaseRule()
{
	માટે (auto it = switches.begin(); it != switches.end(); it++)
		delete *it;
}

તાર્કિક RTLIL::CaseRule::empty() અચલ
{
	પાછા actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() અચલ
{
	RTLIL::CaseRule *new_caserule = નવું RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	માટે (auto &it : switches)
		new_caserule->switches.ધકેલો_back(it->clone());
	પાછા new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	માટે (auto it = cases.begin(); it != cases.end(); it++)
		delete *it;
}

તાર્કિક RTLIL::SwitchRule::empty() અચલ
{
	પાછા cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() અચલ
{
	RTLIL::SwitchRule *new_switchrule = નવું RTLIL::SwitchRule;
	new_switchrule->સંકેત = સંકેત;
	new_switchrule->attributes = attributes;
	માટે (auto &it : cases)
		new_switchrule->cases.ધકેલો_back(it->clone());
	પાછા new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() અચલ
{
	RTLIL::SyncRule *new_syncrule = નવું RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->સંકેત = સંકેત;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	પાછા new_syncrule;
}

RTLIL::Process::~Process()
{
	માટે (auto it = syncs.begin(); it != syncs.end(); it++)
		delete *it;
}

RTLIL::Process *RTLIL::Process::clone() અચલ
{
	RTLIL::Process *new_proc = નવું RTLIL::Process;

	new_proc->attributes = attributes;

	RTLIL::CaseRule *rc_ptr = મૂળ_case.clone();
	new_proc->મૂળ_case = *rc_ptr;
	rc_ptr->switches.clear();
	delete rc_ptr;

	માટે (auto &it : syncs)
		new_proc->syncs.ધકેલો_back(it->clone());

	પાછા new_proc;
}

RTLIL::Memory::~Memory()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
સ્થિર std::map<unsigned પૂર્ણાંક, RTLIL::Memory*> all_memorys;
std::map<unsigned પૂર્ણાંક, RTLIL::Memory*> *RTLIL::Memory::get_all_memorys(શૂન્ય)
{
	પાછા &all_memorys;
}
#endif
YOSYS_NAMESPACE_END
