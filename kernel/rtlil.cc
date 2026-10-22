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

#include "kernel/rtlil.h"
#include "kernel/yosys.h"
#include "kernel/macc.h"
#include "kernel/newcelltypes.h"
#include "kernel/sigtools.h"
#include "kernel/threading.h"
#include "frontends/verilog/verilog_frontend.h"
#include "frontends/verilog/preproc.h"
#include "backends/rtlil/rtlil_backend.h"

#include <string.h>
#include <algorithm>
#include <charconv>
#include <optional>
#include <string_view>
#include <sstream>

YOSYS_NAMESPACE_BEGIN

dict<std::string, std::string> RTLIL::constpad;

const pool<IdString> &RTLIL::builtin_ff_cell_types() {
	static const pool<IdString> res = []() {
		pool<IdString> r;
		དོན་ལུ (size_t i = 0; i < StaticCellTypes::builder.count; i++) {
			auto &cell = StaticCellTypes::builder.cells[i];
			གལ་སྲིད (cell.features.is_ff)
				r.insert(cell.type);
		}
		ལོག r;
	}();
	ལོག res;
}

#define check(condition) log_assert(condition && "malformed Const union")

const Const::bitvectype& Const::get_bits() const {
	check(is_bits());
	ལོག *get_if_bits();
}

const std::string& Const::get_str() const {
	check(is_str());
	ལོག *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	ལོག *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	ལོག *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	new ((void*)&str_) std::string(std::move(str));
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	var_na bytes[] = {
		(var_na)(val >> 24), (var_na)(val >> 16), (var_na)(val >> 8), (var_na)val
	};
	new ((void*)&str_) std::string(bytes, 4);
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val, ཧྲིལ width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	གལ་སྲིད ((width & 7) == 0) {
		new ((void*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		ཧྲིལ bytes = width >> 3;
		signed var_na sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<ཧྲིལ>(bytes, parimaa_na(val));
		དོན་ལུ (ཧྲིལ i = 0; i < bytes; i++) {
			str[str.size() - 1 - i] = val;
			val = val >> 8;
		}
		ལོག;
	}

	new ((void*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	དོན་ལུ (ཧྲིལ i = 0; i < width; i++) {
		bv.push_back((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, ཧྲིལ width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	new ((void*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	དོན་ལུ (ཧྲིལ i = 0; i < width; i++)
		bv.push_back(bit);
}

RTLIL::Const::Const(const std::vector<bool> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	new ((void*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.size());
	དོན་ལུ (const auto &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(const RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	གལ་སྲིད (is_str())
		new ((void*)&str_) std::string(other.get_str());
	དེ་མིན གལ་སྲིད (is_bits())
		new ((void*)&bits_) bitvectype(other.get_bits());
	དེ་མིན
		check(false);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	གལ་སྲིད (is_str())
		new ((void*)&str_) std::string(std::move(other.get_str()));
	དེ་མིན གལ་སྲིད (is_bits())
		new ((void*)&bits_) bitvectype(std::move(other.get_bits()));
	དེ་མིན
		check(false);
}

RTLIL::Const &RTLIL::Const::operator =(const RTLIL::Const &other) {
	flags = other.flags;
	གལ་སྲིད (other.is_str()) {
		གལ་སྲིད (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(void)new ((void*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} དེ་མིན གལ་སྲིད (other.is_bits()) {
		གལ་སྲིད (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(void)new ((void*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} དེ་མིན {
		check(false);
	}
	ལོག *this;
}

RTLIL::Const::~Const() {
	གལ་སྲིད (is_bits())
		bits_.~bitvectype();
	དེ་མིན གལ་སྲིད (is_str())
		str_.~string();
	དེ་མིན
		check(false);
}

bool RTLIL::Const::operator<(const RTLIL::Const &other) const
{
	གལ་སྲིད (size() != other.size())
		ལོག size() < other.size();

	དོན་ལུ (ཧྲིལ i = 0; i < size(); i++)
		གལ་སྲིད ((*this)[i] != other[i])
			ལོག (*this)[i] < other[i];

	ལོག false;
}

bool RTLIL::Const::operator ==(const RTLIL::Const &other) const
{
	གལ་སྲིད (is_str() && other.is_str())
		ལོག get_str() == other.get_str();
	གལ་སྲིད (is_bits() && other.is_bits())
		ལོག get_bits() == other.get_bits();

	གལ་སྲིད (size() != other.size())
		ལོག false;

	དོན་ལུ (ཧྲིལ i = 0; i < size(); i++)
	གལ་སྲིད ((*this)[i] != other[i])
		ལོག false;

	ལོག true;
}

bool RTLIL::Const::operator !=(const RTLIL::Const &other) const
{
	ལོག !(*this == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	ལོག get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() const
{
	std::vector<State> v;
	དོན་ལུ (auto bit : *this)
		v.push_back(bit);
	ལོག v;
}

bool RTLIL::Const::as_bool() const
{
	གལ་སྲིད (is_str()) {
		དོན་ལུ (var_na ch : get_str())
			གལ་སྲིད (ch != 0)
				ལོག true;
		ལོག false;
	}

	const bitvectype& bv = get_bits();
	དོན་ལུ (size_t i = 0; i < bv.size(); i++)
		གལ་སྲིད (bv[i] == State::S1)
			ལོག true;
	ལོག false;
}

ཧྲིལ RTLIL::Const::as_int(bool is_signed) const
{
	int32_t ret = 0;
	གལ་སྲིད (const std::string *s = get_if_str()) {
		ཧྲིལ size = GetSize(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		དོན་ལུ (ཧྲིལ i = std::min(4, size); i > 0; i--)
			ret |= static_cast<unsigned var_na>((*s)[size - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		གལ་སྲིད (is_signed && size > 0 && size < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << size*8;
		ལོག ret;
	}

	const bitvectype& bv = get_bits();
	ཧྲིལ significant_bits = std::min(GetSize(bv), 32);
	དོན་ལུ (ཧྲིལ i = 0; i < significant_bits; i++)
		གལ་སྲིད (bv[i] == State::S1)
			ret |= 1 << i;
	གལ་སྲིད (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	ལོག ret;
}

bool RTLIL::Const::convertible_to_int(bool is_signed) const
{
	auto size = get_min_size(is_signed);

	གལ་སྲིད (size < 0)
		ལོག false;

	// If it fits in 31 bits it is definitely convertible
	གལ་སྲིད (size <= 31)
		ལོག true;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	གལ་སྲིད (size == 32) {
		གལ་སྲིད (is_signed)
			ལོག true;
		ལོག (*this)[size - 1] != State::S1;
	}

	ལོག false;
}

std::optional<ཧྲིལ> RTLIL::Const::try_as_int(bool is_signed) const
{
	གལ་སྲིད (!convertible_to_int(is_signed))
		ལོག std::nullopt;
	ལོག as_int(is_signed);
}

ཧྲིལ RTLIL::Const::as_int_saturating(bool is_signed) const
{
	གལ་སྲིད (!convertible_to_int(is_signed)) {
		གལ་སྲིད (!is_signed)
			ལོག std::numeric_limits<ཧྲིལ>::max();

		const auto min_size = get_min_size(is_signed);
		log_assert(min_size > 0);
		const auto neg = (*this)[min_size - 1];
		ལོག neg ? std::numeric_limits<ཧྲིལ>::min() : std::numeric_limits<ཧྲིལ>::max();
	}
	ལོག as_int(is_signed);
}

void RTLIL::Const::tag_bare_integer_const(const std::string &value)
{
	གལ་སྲིད (value.empty() || value.find('\'') != std::string::npos)
		ལོག;
	size_t start = (value[0] == '-' || value[0] == '+') ? 1 : 0;
	གལ་སྲིད (start < value.size() && std::all_of(value.begin() + start, value.end(), ::isdigit))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

ཧྲིལ RTLIL::Const::get_min_size(bool is_signed) const
{
	གལ་སྲིད (empty()) ལོག 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	གལ་སྲིད (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	དེ་མིན
		leading_bit = RTLIL::State::S0;

	auto idx = size();
	སྐབས (idx > 0 && (*this)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	གལ་སྲིད (is_signed && idx < size()) {
		idx++;
	}
	// must be at least one bit
	ལོག (idx == 0) ? 1 : idx;
}

void RTLIL::Const::compress(bool is_signed)
{
	auto idx = get_min_size(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<ཧྲིལ> RTLIL::Const::as_int_compress(bool is_signed) const
{
	ལོག try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(const var_na* any) const
{
	ཧྲིལ sz = size();
	std::string ret;
	ret.reserve(sz);
	དོན་ལུ (ཧྲིལ i = sz - 1; i >= 0; --i)
		switch ((*this)[i]) {
			pak_sa S0: ret.push_back('0'); bha_nga;
			pak_sa S1: ret.push_back('1'); bha_nga;
			pak_sa Sx: ret.push_back('x'); bha_nga;
			pak_sa Sz: ret.push_back('z'); bha_nga;
			pak_sa Sa: ret += any; bha_nga;
			pak_sa Sm: ret.push_back('m'); bha_nga;
		}
	ལོག ret;
}

RTLIL::Const RTLIL::Const::from_string(const std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.size());
	དོན་ལུ (auto it = str.rbegin(); it != str.rend(); it++)
		switch (*it) {
			pak_sa '0': bv.push_back(State::S0); bha_nga;
			pak_sa '1': bv.push_back(State::S1); bha_nga;
			pak_sa 'x': bv.push_back(State::Sx); bha_nga;
			pak_sa 'z': bv.push_back(State::Sz); bha_nga;
			pak_sa 'm': bv.push_back(State::Sm); bha_nga;
			default: bv.push_back(State::Sa);
		}
	ལོག c;
}

std::string RTLIL::Const::decode_string() const
{
	གལ་སྲིད (auto str = get_if_str())
		ལོག *str;

	const bitvectype& bv = get_bits();
	const ཧྲིལ n = GetSize(bv);
	const ཧྲིལ n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	ཧྲིལ i = n_over_8 * 8;
	གལ་སྲིད (i < n) {
		var_na ch = 0;
		དོན་ལུ (ཧྲིལ j = 0; j < (n - i); j++) {
			གལ་སྲིད (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		གལ་སྲིད (ch != 0)
			s.append({ch});
	}
	i -= 8;
	དོན་ལུ (; i >= 0; i -= 8) {
		var_na ch = 0;
		དོན་ལུ (ཧྲིལ j = 0; j < 8; j++) {
			གལ་སྲིད (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		གལ་སྲིད (ch != 0)
			s.append({ch});
	}
	ལོག s;
}

ཧྲིལ RTLIL::Const::size() const {
	གལ་སྲིད (is_str())
		ལོག 8 * str_.size();
	དེ་མིན {
		check(is_bits());
		ལོག bits_.size();
	}
}

bool RTLIL::Const::empty() const {
	གལ་སྲིད (is_str())
		ལོག str_.empty();
	དེ་མིན {
		check(is_bits());
		ལོག bits_.empty();
	}
}

void RTLIL::Const::bitvectorize_internal() {
	གལ་སྲིད (tag == backing_tag::bits)
		ལོག;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.size() * 8);
	དོན་ལུ (ཧྲིལ i = str_.size() - 1; i >= 0; i--) {
		unsigned var_na ch = str_[i];
		དོན་ལུ (ཧྲིལ j = 0; j < 8; j++) {
			new_bits.push_back((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(void)new ((void*)&bits_) bitvectype(std::move(new_bits));
		tag = backing_tag::bits;
	}
}

void RTLIL::Const::append(const RTLIL::Const &other) {
	bitvectorize_internal();
	bitvectype& bv = get_bits();
	bv.insert(bv.end(), other.begin(), other.end());
}

RTLIL::State RTLIL::Const::const_iterator::operator*() const {
	གལ་སྲིད (auto bv = parent->get_if_bits())
		ལོག (*bv)[idx];

	ཧྲིལ char_idx = parent->get_str().size() - idx / 8 - 1;
	bool bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	ལོག bit ? State::S1 : State::S0;
}

bool RTLIL::Const::is_fully_zero() const
{
	གལ་སྲིད (auto str = get_if_str()) {
		དོན་ལུ (var_na ch : *str)
			གལ་སྲིད (ch != 0)
				ལོག false;
		ལོག true;
	}

	const bitvectype& bv = get_bits();

	དོན་ལུ (const auto &bit : bv)
		གལ་སྲིད (bit != RTLIL::State::S0)
			ལོག false;

	ལོག true;
}

bool RTLIL::Const::is_fully_ones() const
{
	གལ་སྲིད (auto str = get_if_str()) {
		དོན་ལུ (var_na ch : *str)
			གལ་སྲིད (ch != (var_na)0xff)
				ལོག false;
		ལོག true;
	}

	const bitvectype& bv = get_bits();
	དོན་ལུ (const auto &bit : bv)
		གལ་སྲིད (bit != RTLIL::State::S1)
			ལོག false;

	ལོག true;
}

bool RTLIL::Const::is_fully_def() const
{
	གལ་སྲིད (is_str())
		ལོག true;

	const bitvectype& bv = get_bits();
	དོན་ལུ (const auto &bit : bv)
		གལ་སྲིད (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			ལོག false;

	ལོག true;
}

bool RTLIL::Const::is_fully_undef() const
{
	གལ་སྲིད (auto str = get_if_str())
		ལོག str->empty();

	const bitvectype& bv = get_bits();
	དོན་ལུ (const auto &bit : bv)
		གལ་སྲིད (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			ལོག false;

	ལོག true;
}

bool RTLIL::Const::is_fully_undef_x_only() const
{
	གལ་སྲིད (auto str = get_if_str())
		ལོག str->empty();

	const bitvectype& bv = get_bits();
	དོན་ལུ (const auto &bit : bv)
		གལ་སྲིད (bit != RTLIL::State::Sx)
			ལོག false;

	ལོག true;
}

bool RTLIL::Const::is_onehot(ཧྲིལ *pos) const
{
	bool found = false;
	ཧྲིལ size = GetSize(*this);
	དོན་ལུ (ཧྲིལ i = 0; i < size; i++) {
		State bit = (*this)[i];
		གལ་སྲིད (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			ལོག false;
		གལ་སྲིད (bit == RTLIL::State::S1) {
			གལ་སྲིད (found)
				ལོག false;
			གལ་སྲིད (pos)
				*pos = i;
			found = true;
		}
	}
	ལོག found;
}

Hasher RTLIL::Const::hash_into(Hasher h) const
{
	གལ་སྲིད (auto str = get_if_str())
		ལོག hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	const bitvectype &bv = get_bits();
	ཧྲིལ size = GetSize(bv);
	std::string packed;
	ཧྲིལ packed_size = (size + 7) >> 3;
	packed.resize(packed_size, 0);
	དོན་ལུ (ཧྲིལ bi = 0; bi < packed_size; ++bi) {
		var_na ch = 0;
		ཧྲིལ end = std::min((bi + 1)*8, size);
		དོན་ལུ (ཧྲིལ i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			གལ་སྲིད (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<var_na>::hash_into(ch, h);
				དོན་ལུ (; i < size; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(size);
				ལོག h;
			}
			ch |= static_cast<ཧྲིལ>(b) << (i & 7);
		}
		packed[packed_size - 1 - bi] = ch;
	}
	ལོག hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(ཧྲིལ offset, ཧྲིལ len, RTLIL::State padding) const {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	དོན་ལུ (ཧྲིལ i = offset; i < offset + len; i++)
		ret_bv.push_back(i < GetSize(*this) ? (*this)[i] : padding);
	ལོག RTLIL::Const(ret_bv);
}
#undef check /* check(condition) for Const */

bool RTLIL::AttrObject::has_attribute(RTLIL::IdString id) const
{
	ལོག attributes.count(id);
}

void RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, bool value)
{
	གལ་སྲིད (value)
		attributes[id] = RTLIL::Const(1);
	དེ་མིན
		attributes.erase(id);
}

bool RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) const
{
	const auto it = attributes.find(id);
	གལ་སྲིད (it == attributes.end())
		ལོག false;
	ལོག it->second.as_bool();
}

void RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string value)
{
	གལ་སྲིད (value.empty())
		attributes.erase(id);
	དེ་མིན
		attributes[id] = value;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) const
{
	std::string value;
	const auto it = attributes.find(id);
	གལ་སྲིད (it != attributes.end())
		value = it->second.decode_string();
	ལོག value;
}

void RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, const pool<string> &data)
{
	string attrval;
	དོན་ལུ (const auto &s : data) {
		གལ་སྲིད (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

void RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, const pool<string> &data)
{
	pool<string> union_data = get_strpool_attribute(id);
	union_data.insert(data.begin(), data.end());
	གལ་སྲིད (!union_data.empty())
		set_strpool_attribute(id, union_data);
}

void RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->attributes = std::move(buf);
}

namespace {
	template<typename F>
	void walk_process_attr_objects(RTLIL::Process *process, F visit) {
		visit(process);
		std::vector<RTLIL::CaseRule*> case_stack{&process->root_case};
		སྐབས (!case_stack.empty()) {
			RTLIL::CaseRule *cs = case_stack.back();
			case_stack.pop_back();
			visit(cs);
			དོན་ལུ (auto *sw : cs->switches) {
				visit(sw);
				དོན་ལུ (auto *case_ : sw->cases)
					case_stack.push_back(case_);
			}
		}
		དོན་ལུ (auto *sync : process->syncs)
			དོན་ལུ (auto &mwa : sync->mem_write_actions)
				visit(&mwa);
	}

	template<typename F>
	void walk_attr_objects(RTLIL::Design *design, F visit) {
		དོན་ལུ (auto &[_, module] : design->modules_) {
			visit(module);
			དོན་ལུ (auto &[_, wire] : module->wires_)
				visit(wire);
			དོན་ལུ (auto &[_, mem] : module->memories)
				visit(mem);
			དོན་ལུ (auto &[_, cell] : module->cells_)
				visit(cell);
			དོན་ལུ (auto &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


size_t RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	auto root = [&](IdString ref) {
		གལ་སྲིད (ref != IdString::Null)
			live.insert(ref);
	};

	walk_attr_objects(this, [&](const RTLIL::AttrObject *obj) {
		དོན་ལུ (auto &attr : obj->attributes)
			root(attr.first);
	});

	root(selected_active_module);

	དོན་ལུ (auto &[name, module] : modules_) {
		root(name);
		དོན་ལུ (IdString port : module->ports)
			root(port);
		དོན་ལུ (IdString param : module->avail_parameters)
			root(param);
		དོན་ལུ (auto &[param, _] : module->parameter_default_values)
			root(param);
		དོན་ལུ (auto &[name, _] : module->memories)
			root(name);
		དོན་ལུ (auto &[name, _] : module->processes)
			root(name);
		དོན་ལུ (auto &[name, wire] : module->wires_) {
			root(name);
			གལ་སྲིད (wire->known_driver())
				root(wire->driverPort());
		}
		དོན་ལུ (auto &[name, cell] : module->cells_) {
			root(name);
			root(cell->type);
			དོན་ལུ (auto &conn : cell->connections())
				root(conn.first);
			དོན་ལུ (auto &param : cell->parameters)
				root(param.first);
		}
		དོན་ལུ (auto &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_stack{&process->root_case};
			སྐབས (!case_stack.empty()) {
				RTLIL::CaseRule *cs = case_stack.back();
				case_stack.pop_back();
				དོན་ལུ (auto *sw : cs->switches) {
					དོན་ལུ (auto *case_ : sw->cases)
						case_stack.push_back(case_);
				}
			}
			དོན་ལུ (auto *sync : process->syncs) {
				དོན་ལུ (auto &mwa : sync->mem_write_actions)
					root(mwa.memid);
			}
		}
	}

	auto root_selection = [&](const RTLIL::Selection &sel) {
		དོན་ལུ (IdString m : sel.selected_modules)
			root(m);
		དོན་ལུ (auto &[m, members] : sel.selected_members) {
			root(m);
			དོན་ལུ (IdString member : members)
				root(member);
		}
	};

	དོན་ལུ (auto &[name, sel] : selection_vars) {
		root(name);
		root_selection(sel);
	}

	དོན་ལུ (auto &sel : selection_stack)
		root_selection(sel);

	size_t erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_count;
	ལོག erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) const
{
	pool<string> data;
	གལ་སྲིད (attributes.count(id) != 0)
		དོན་ལུ (auto s : split_tokens(get_string_attribute(id), "|"))
			data.insert(s);
	ལོག data;
}

void RTLIL::AttrObject::set_hdlname_attribute(const vector<string> &hierarchy)
{
	string attrval;
	དོན་ལུ (const auto &ident : hierarchy) {
		གལ་སྲིད (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() const
{
	ལོག split_tokens(get_string_attribute(ID::hdlname), " ");
}

void RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, const vector<ཧྲིལ> &data)
{
	std::stringstream attrval;
	དོན་ལུ (auto &i : data) {
		གལ་སྲིད (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<ཧྲིལ> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) const
{
	vector<ཧྲིལ> data;
	auto it = attributes.find(id);
	གལ་སྲིད (it != attributes.end())
		དོན་ལུ (const auto &s : split_tokens(attributes.at(id).decode_string())) {
			var_na *end = nullptr;
			errno = 0;
			long value = strtol(s.c_str(), &end, 10);
			གལ་སྲིད (end != s.c_str() + s.size())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			གལ་སྲིད (errno == ERANGE || value < INT_MIN || value > INT_MAX)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			data.push_back(value);
		}
	ལོག data;
}

bool RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) const
{
	གལ་སྲིད (current_design != nullptr) {
		auto module = current_design->module(mod_name);
		ལོག module && module->get_blackbox_attribute();
	} དེ་མིན {
		log_warning("Unable to check if module is boxed for null design.\n");
		ལོག false;
	}
}

bool RTLIL::Selection::selected_module(RTLIL::IdString mod_name) const
{
	གལ་སྲིད (complete_selection)
		ལོག true;
	གལ་སྲིད (!selects_boxes && boxed_module(mod_name))
		ལོག false;
	གལ་སྲིད (full_selection)
		ལོག true;
	གལ་སྲིད (selected_modules.count(mod_name) > 0)
		ལོག true;
	གལ་སྲིད (selected_members.count(mod_name) > 0)
		ལོག true;
	ལོག false;
}

bool RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) const
{
	གལ་སྲིད (complete_selection)
		ལོག true;
	གལ་སྲིད (!selects_boxes && boxed_module(mod_name))
		ལོག false;
	གལ་སྲིད (full_selection)
		ལོག true;
	གལ་སྲིད (selected_modules.count(mod_name) > 0)
		ལོག true;
	ལོག false;
}

bool RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) const
{
	གལ་སྲིད (complete_selection)
		ལོག true;
	གལ་སྲིད (!selects_boxes && boxed_module(mod_name))
		ལོག false;
	གལ་སྲིད (full_selection)
		ལོག true;
	གལ་སྲིད (selected_modules.count(mod_name) > 0)
		ལོག true;
	གལ་སྲིད (selected_members.count(mod_name) > 0)
		གལ་སྲིད (selected_members.at(mod_name).count(memb_name) > 0)
			ལོག true;
	ལོག false;
}

void RTLIL::Selection::optimize(RTLIL::Design *design)
{
	གལ་སྲིད (design != current_design) {
		current_design = design;
	}

	གལ་སྲིད (selects_boxes && full_selection)
		complete_selection = true;
	གལ་སྲིད (complete_selection) {
		full_selection = false;
		selects_boxes = true;
	}
	གལ་སྲིད (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		ལོག;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.clear();
	དོན་ལུ (auto mod_name : selected_modules) {
		གལ་སྲིད (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.push_back(mod_name);
		selected_members.erase(mod_name);
	}
	དོན་ལུ (auto mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	དོན་ལུ (auto &it : selected_members)
		གལ་སྲིད (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.push_back(it.first);
	དོན་ལུ (auto mod_name : del_list)
		selected_members.erase(mod_name);

	དོན་ལུ (auto &it : selected_members) {
		del_list.clear();
		དོན་ལུ (auto memb_name : it.second)
			གལ་སྲིད (current_design->module(it.first)->count_id(memb_name) == 0)
				del_list.push_back(memb_name);
		དོན་ལུ (auto memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	add_list.clear();
	དོན་ལུ (auto &it : selected_members)
		གལ་སྲིད (it.second.size() == 0)
			del_list.push_back(it.first);
		དེ་མིན གལ་སྲིད (it.second.size() == current_design->module(it.first)->wires_.size() + current_design->module(it.first)->memories.size() +
				current_design->module(it.first)->cells_.size() + current_design->module(it.first)->processes.size())
			add_list.push_back(it.first);
	དོན་ལུ (auto mod_name : del_list)
		selected_members.erase(mod_name);
	དོན་ལུ (auto mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.insert(mod_name);
	}

	གལ་སྲིད (selected_modules.size() == current_design->modules_.size()) {
		selected_modules.clear();
		selected_members.clear();
		གལ་སྲིད (selects_boxes)
			complete_selection = true;
		དེ་མིན
			full_selection = true;
	}
}

void RTLIL::Selection::clear()
{
	full_selection = false;
	complete_selection = false;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_defines (new define_map_t)
{
	static unsigned ཧྲིལ hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	refcount_modules_ = 0;
	selected_active_module.clear();
	push_full_selection();

	RTLIL::Design::get_all_designs()->insert(std::pair<unsigned ཧྲིལ, RTLIL::Design*>(hashidx_, this));
}

RTLIL::Design::~Design()
{
	དོན་ལུ (auto &pr : modules_)
		delete pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

static std::map<unsigned ཧྲིལ, RTLIL::Design*> all_designs;
std::map<unsigned ཧྲིལ, RTLIL::Design*> *RTLIL::Design::get_all_designs(void)
{
	ལོག &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	ལོག RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

const RTLIL::Module *RTLIL::Design::module(IdString id) const {
	twines_.check_owned(id);
	ལོག modules_.count(id) ? modules_.at(id) : NULL;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	ལོག modules_.count(id) ? modules_.at(id) : NULL;
}

RTLIL::Module *RTLIL::Design::top_module() const
{
	RTLIL::Module *module = nullptr;
	ཧྲིལ module_count = 0;

	དོན་ལུ (auto mod : selected_modules()) {
		གལ་སྲིད (mod->get_bool_attribute(ID::top))
			ལོག mod;
		module_count++;
		module = mod;
	}

	ལོག module_count == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	གལ་སྲིད (modules_.count(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = new RTLIL::Module;
	modules_[name] = module;
	module->design = this;
	module->name = name;

	དོན་ལུ (auto mon : monitors)
		mon->notify_module_add(module);

	གལ་སྲིད (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	ལོག module;
}

void RTLIL::Design::scratchpad_unset(const std::string &varname)
{
	scratchpad.erase(varname);
}

void RTLIL::Design::scratchpad_set_int(const std::string &varname, ཧྲིལ value)
{
	scratchpad[varname] = stringf("%d", value);
}

void RTLIL::Design::scratchpad_set_bool(const std::string &varname, bool value)
{
	scratchpad[varname] = value ? "true" : "false";
}

void RTLIL::Design::scratchpad_set_string(const std::string &varname, std::string value)
{
	scratchpad[varname] = std::move(value);
}

ཧྲིལ RTLIL::Design::scratchpad_get_int(const std::string &varname, ཧྲིལ default_value) const
{
	auto it = scratchpad.find(varname);
	གལ་སྲིད (it == scratchpad.end())
		ལོག default_value;

	const std::string &str = it->second;

	གལ་སྲིད (str == "0" || str == "false")
		ལོག 0;

	གལ་སྲིད (str == "1" || str == "true")
		ལོག 1;

	var_na *endptr = nullptr;
	long ཧྲིལ parsed_value = strtol(str.c_str(), &endptr, 10);
	ལོག *endptr ? default_value : parsed_value;
}

bool RTLIL::Design::scratchpad_get_bool(const std::string &varname, bool default_value) const
{
	auto it = scratchpad.find(varname);
	གལ་སྲིད (it == scratchpad.end())
		ལོག default_value;

	const std::string &str = it->second;

	གལ་སྲིད (str == "0" || str == "false")
		ལོག false;

	གལ་སྲིད (str == "1" || str == "true")
		ལོག true;

	ལོག default_value;
}

std::string RTLIL::Design::scratchpad_get_string(const std::string &varname, const std::string &default_value) const
{
	auto it = scratchpad.find(varname);
	གལ་སྲིད (it == scratchpad.end())
		ལོག default_value;

	ལོག it->second;
}

void RTLIL::Design::remove(RTLIL::Module *module)
{
	དོན་ལུ (auto mon : monitors)
		mon->notify_module_del(module);

	གལ་སྲིད (yosys_xtrace) {
		log("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	delete module;
}

void RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	add(module);
}

void RTLIL::Design::sort()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
	དོན་ལུ (auto &it : modules_)
		it.second->sort();
}

void RTLIL::Design::sort_modules()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
}

void check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

void RTLIL::Design::check()
{
#ifndef NDEBUG
	log_assert(!selection_stack.empty());
	ཧྲིལ pool_size = 0;
	དོན་ལུ (auto &it : modules_)
		pool_size = std::max(pool_size, ThreadPool::work_pool_size(0, it.second->cells_size(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_size);
	དོན་ལུ (auto &it : modules_) {
		log_assert(this == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#endif
}

void RTLIL::Design::optimize()
{
	དོན་ལུ (auto &it : modules_)
		it.second->optimize();
	དོན་ལུ (auto &it : selection_stack)
		it.optimize(this);
	དོན་ལུ (auto &it : selection_vars)
		it.second.optimize(this);
}

void RTLIL::Design::clone_into(RTLIL::Design *dst) const
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	དོན་ལུ (auto it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

bool RTLIL::Design::selected_module(IdString mod_name) const
{
	གལ་སྲིད (!selected_active_module.empty() && mod_name != selected_active_module)
		ལོག false;
	ལོག selection().selected_module(mod_name);
}

bool RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) const
{
	གལ་སྲིད (!selected_active_module.empty() && mod_name != selected_active_module)
		ལོག false;
	ལོག selection().selected_whole_module(mod_name);
}

bool RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) const
{
	གལ་སྲིད (!selected_active_module.empty() && mod_name != selected_active_module)
		ལོག false;
	ལོག selection().selected_member(mod_name, memb_name);
}

bool RTLIL::Design::selected_module(RTLIL::Module *mod) const
{
	ལོག selected_module(mod->name);
}

bool RTLIL::Design::selected_whole_module(RTLIL::Module *mod) const
{
	ལོག selected_whole_module(mod->name);
}

void RTLIL::Design::push_selection(RTLIL::Selection sel)
{
	sel.current_design = this;
	selection_stack.push_back(sel);
}

void RTLIL::Design::push_empty_selection()
{
	push_selection(RTLIL::Selection::EmptySelection(this));
}

void RTLIL::Design::push_full_selection()
{
	push_selection(RTLIL::Selection::FullSelection(this));
}

void RTLIL::Design::push_complete_selection()
{
	push_selection(RTLIL::Selection::CompleteSelection(this));
}

void RTLIL::Design::pop_selection()
{
	selection_stack.pop_back();
	// Default to a full_selection if we ran out of stack
	གལ་སྲིད (selection_stack.empty())
		push_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(bool only_selected) const
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(this), only_selected, true, false, RTLIL_BACKEND::DumpMode::Readable);
	ལོག f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) const
{
	bool include_partials = partials == RTLIL::SELECT_ALL;
	bool exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	bool ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> result;
	result.reserve(modules_.size());
	དོན་ལུ (auto &it : modules_) {
		གལ་སྲིད (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			གལ་སྲིད (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				result.push_back(it.second);
			དེ་མིན
				switch (boxes)
				{
				pak_sa RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.second);
					bha_nga;
				pak_sa RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.second);
					bha_nga;
				pak_sa RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.second);
					bha_nga;
				pak_sa RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.second);
					bha_nga;
				pak_sa RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.second);
					bha_nga;
				pak_sa RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.second);
					bha_nga;
				default:
					bha_nga;
				}
		} དེ་མིན གལ་སྲིད (!include_partials && selected_module(it.first)) {
			switch(partials)
			{
			pak_sa RTLIL::SELECT_WHOLE_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.second);
				bha_nga;
			pak_sa RTLIL::SELECT_WHOLE_ERR:
				log_error("Unsupported partially selected module %s.\n", it.second);
				bha_nga;
			pak_sa RTLIL::SELECT_WHOLE_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.second);
				bha_nga;
			default:
				bha_nga;
			}
		}
	}
	ལོག result;
}

RTLIL::Module::Module()
{
	static unsigned ཧྲིལ hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->insert(std::pair<unsigned ཧྲིལ, RTLIL::Module*>(hashidx_, this));
#endif
}

RTLIL::Module::~Module()
{
	དོན་ལུ (auto &pr : wires_)
		delete pr.second;
	དོན་ལུ (auto &pr : memories)
		delete pr.second;
	དོན་ལུ (auto &pr : cells_)
		delete pr.second;
	དོན་ལུ (auto &pr : processes)
		delete pr.second;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
static std::map<unsigned ཧྲིལ, RTLIL::Module*> all_modules;
std::map<unsigned ཧྲིལ, RTLIL::Module*> *RTLIL::Module::get_all_modules(void)
{
	ལོག &all_modules;
}
#endif

void RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	དོན་ལུ (auto it = wires_.begin(); it != wires_.end(); ++it)
		གལ་སྲིད (!it->second->port_input && !it->second->port_output)
			delwires.insert(it->second);

	དོན་ལུ (auto it = memories.begin(); it != memories.end(); ++it)
		delete it->second;
	memories.clear();

	དོན་ལུ (auto it = cells_.begin(); it != cells_.end(); ++it)
		delete it->second;
	cells_.clear();

	དོན་ལུ (auto it = processes.begin(); it != processes.end(); ++it)
		delete it->second;
	processes.clear();

	connections_.clear();

	remove(delwires);
	set_bool_attribute(ID::blackbox);
}

void RTLIL::Module::expand_interfaces(RTLIL::Design *, const dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

bool RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	ལོག false;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, const dict<RTLIL::IdString, RTLIL::Const> &, bool mayfail)
{
	གལ་སྲིད (mayfail)
		ལོག IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, const dict<RTLIL::IdString, RTLIL::Const> &, const dict<RTLIL::IdString, RTLIL::Module*> &, const dict<RTLIL::IdString, RTLIL::IdString> &, bool mayfail)
{
	གལ་སྲིད (mayfail)
		ལོག IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

size_t RTLIL::Module::count_id(RTLIL::IdString id)
{
	ལོག wires_.count(id) + cells_.count(id) + memories.count(id) + processes.count(id);
}

#ifndef NDEBUG
namespace {
	sa_nracanaa InternalCellChecker
	{
		const RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		InternalCellChecker(const RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		void error(ཧྲིལ linenr, std::optional<std::string> error_msg = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			གལ་སྲིད (error_msg) {
				log_error("%s in internal cell %s\n", *error_msg, cell_loc);
			} དེ་མིན {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		ཧྲིལ param(RTLIL::IdString name)
		{
			auto it = cell->parameters.find(name);
			གལ་སྲིད (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.insert(name);
			ལོག it->second.as_int();
		}

		ཧྲིལ param_bool(RTLIL::IdString name)
		{
			ཧྲིལ v = param(name);
			གལ་སྲིད (GetSize(cell->parameters.at(name)) > 32)
				error(__LINE__);
			གལ་སྲིད (v != 0 && v != 1)
				error(__LINE__);
			ལོག v;
		}

		ཧྲིལ param_bool(RTLIL::IdString name, bool expected)
		{
			ཧྲིལ v = param_bool(name);
			གལ་སྲིད (v != expected)
				error(__LINE__);
			ལོག v;
		}

		void param_bits(RTLIL::IdString name, ཧྲིལ width)
		{
			param(name);
			གལ་སྲིད (GetSize(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			ལོག cell->parameters.at(name).decode_string();
		}

		void port(RTLIL::IdString name, ཧྲིལ width)
		{
			auto it = cell->connections_.find(name);
			གལ་སྲིད (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			གལ་སྲིད (GetSize(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.insert(name);
		}

		void check_expected(bool check_matched_sign = false)
		{
			དོན་ལུ (auto &para : cell->parameters)
				གལ་སྲིད (expected_params.count(para.first) == 0)
					error(__LINE__);
			དོན་ལུ (auto &conn : cell->connections())
				གལ་སྲིད (expected_ports.count(conn.first) == 0)
					error(__LINE__);

			གལ་སྲིད (check_matched_sign) {
				log_assert(expected_params.count(ID::A_SIGNED) != 0 && expected_params.count(ID::B_SIGNED) != 0);
				bool a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				bool b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				གལ་སྲིད (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		void check()
		{
			གལ་སྲིད (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				ལོག;

			གལ་སྲིད (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(true);
				ལོག;
			}

			གལ་སྲིད (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/false);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/false);
				ལོག;
			}

			གལ་སྲིད (cell->type.in(ID($shift), ID($shiftx))) {
				གལ་སྲིད (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/false);
				} དེ་མིན {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/false);
				ལོག;
			}

			གལ་སྲིད (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(true);
				ལོག;
			}

			གལ་སྲིད (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($pow));
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($alu)) {
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
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($macc_v2)) {
				གལ་སྲིད (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				གལ་སྲིད (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				གལ་སྲིད (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				const Const &a_width = cell->getParam(ID::A_WIDTHS);
				const Const &b_width = cell->getParam(ID::B_WIDTHS);
				const Const &c_width = cell->getParam(ID::C_WIDTHS);
				ཧྲིལ a_width_sum = 0, b_width_sum = 0, c_width_sum = 0;
				དོན་ལུ (ཧྲིལ i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_sum += a_width.extract(16 * i, 16).as_int(false);
					b_width_sum += b_width.extract(16 * i, 16).as_int(false);
				}
				དོན་ལུ (ཧྲིལ i = 0; i < param(ID::NADDENDS); i++) {
					c_width_sum += c_width.extract(16 * i, 16).as_int(false);
				}
				port(ID::A, a_width_sum);
				port(ID::B, b_width_sum);
				port(ID::C, c_width_sum);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/false);
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				གལ་སྲིད (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($dffsre)) {
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
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_VALUE, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_VALUE, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type.in(ID($sdffe), ID($sdffce))) {
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
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($adffe)) {
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
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($aldffe)) {
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
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_VALUE, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($fsm)) {
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
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($memrd_v2)) {
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
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($memwr_v2)) {
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
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::WORDS));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::WORDS));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($mem)) {
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
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($mem_v2)) {
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
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type.in(ID($assert), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type.in(ID($specify2), ID($specify3))) {
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
				གལ་སྲིད (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
				}
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($specrule)) {
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
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				གལ་སྲིད (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
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
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				གལ་སྲིད (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				ལོག;
			}

			གལ་སྲིད (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); ལོག; }
			གལ་སྲིད (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  ལོག; }

			གལ་སྲིད (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); ལོག; }

			གལ་སྲིད (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ལོག;
			}
			གལ་སྲིད (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ལོག;
			}
			གལ་སྲིད (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				ལོག;
			}
			གལ་སྲིད (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ལོག;
			}
			གལ་སྲིད (cell->type.in(ID($input_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ལོག;
			}
			གལ་སྲིད (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				check_expected();
				ལོག;
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

void RTLIL::Module::sort()
{
	wires_.sort(sort_by_id_str(design->twines()));
	cells_.sort(sort_by_id_str(design->twines()));
	parameter_default_values.sort(sort_by_id_str(design->twines()));
	memories.sort(sort_by_id_str(design->twines()));
	processes.sort(sort_by_id_str(design->twines()));
	དོན་ལུ (auto &it : cells_)
		it.second->sort();
	དོན་ལུ (auto &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	དོན་ལུ (auto &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#ifndef NDEBUG
static void check_id_keys(const dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	དོན་ལུ (auto &it : keyed)
		log_assert(!it.first.empty());
}
#endif

void check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ifndef NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->cells_size(), 1000));
	const RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	དོན་ལུ (auto &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->width >= 0);
		log_assert(it.second->size >= 0);
		check_id_keys(it.second->attributes);
		memory_strings.insert(it.second->name.str());
	}

	std::vector<MonotonicFlag> ports_declared(GetSize(module->ports));
	ShardedVector<std::string> memids(subpool);
	subpool.run([const_module, &ports_declared, &memory_strings, &memids](const ParallelDispatchThreadPool::RunCtx &ctx) {
		དོན་ལུ (ཧྲིལ i : ctx.item_range(const_module->cells_size())) {
			auto it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			དོན་ལུ (auto &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_keys(it.second->attributes);
			check_id_keys(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			གལ་སྲིད (it.second->has_memid()) {
				log_assert(memory_strings.count(it.second->parameters.at(ID::MEMID).decode_string()));
			} དེ་མིན གལ་སྲིད (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.count(memid));
				memids.insert(ctx, std::move(memid));
			}
			auto cell_mod = const_module->design->module(it.second->name);
			གལ་སྲིད (cell_mod != nullptr) {
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

		དོན་ལུ (ཧྲིལ i : ctx.item_range(const_module->wires_size())) {
			auto it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_keys(it.second->attributes);
			གལ་སྲིད (it.second->port_id) {
				log_assert(GetSize(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_input || it.second->port_output);
				log_assert(it.second->port_id <= GetSize(ports_declared));
				bool previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == false);
			} དེ་མིན
				log_assert(!it.second->port_input && !it.second->port_output);
		}
	});
	དོན་ལུ (const MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == true);
	pool<std::string> memids_pool;
	དོན་ལུ (std::string &memid : memids)
		log_assert(memids_pool.insert(memid).second);

	དོན་ལུ (auto &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->root_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->root_case};
		དོན་ལུ (size_t i = 0; i < all_cases.size(); i++) {
			དོན་ལུ (auto &switch_it : all_cases[i]->switches) {
				དོན་ལུ (auto &case_it : switch_it->cases) {
					དོན་ལུ (auto &compare_it : case_it->compare) {
						log_assert(switch_it->sa_nkeeta.size() == compare_it.size());
					}
					all_cases.push_back(case_it);
				}
			}
		}
		དོན་ལུ (auto &sync_it : it.second->syncs) {
			switch (sync_it->type) {
				pak_sa RTLIL::SyncType::ST0:
				pak_sa RTLIL::SyncType::ST1:
				pak_sa RTLIL::SyncType::STp:
				pak_sa RTLIL::SyncType::STn:
				pak_sa RTLIL::SyncType::STe:
					log_assert(!sync_it->sa_nkeeta.empty());
					bha_nga;
				pak_sa RTLIL::SyncType::STa:
				pak_sa RTLIL::SyncType::STg:
				pak_sa RTLIL::SyncType::STi:
					log_assert(sync_it->sa_nkeeta.empty());
					bha_nga;
			}
		}
	}

	དོན་ལུ (auto &it : module->connections_) {
		log_assert(it.first.size() == it.second.size());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_keys(module->attributes);
#endif
}

void RTLIL::Module::check()
{
	ཧྲིལ pool_size = ThreadPool::work_pool_size(0, cells_size(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_size);
	check_module(this, thread_pool);
}

void RTLIL::Module::optimize()
{
}

void RTLIL::Module::cloneInto(RTLIL::Module *new_mod) const
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	དོན་ལུ (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_values.clear();
	དོན་ལུ (auto &it : parameter_default_values)
		new_mod->parameter_default_values[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	དོན་ལུ (auto &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	དོན་ལུ (auto it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	དོན་ལུ (auto it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	དོན་ལུ (auto it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	དོན་ལུ (auto it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addProcess(dst_id, it->second);
	}

	sa_nracanaa RewriteSigSpecWorker
	{
		const dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		const RTLIL::Module *new_mod;
		void operator()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([this](RTLIL::Wire *&wire) {
				auto it = wire_map.find(wire);
				གལ་སྲིད (it != wire_map.end())
					wire = it->second;
				དེ་མིན
					log_assert(wire->module == new_mod);
			});
		}
	};

	RewriteSigSpecWorker rewriteSigSpecWorker{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewriteSigSpecWorker);
	new_mod->fixup_ports();
}

RTLIL::Module *RTLIL::Module::clone() const
{
	RTLIL::Module *new_mod = new RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	ལོག new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) const
{
	RTLIL::Module *new_mod = new RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->add(new_mod);
	ལོག new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) const
{
	RTLIL::Module *new_mod = new RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	ལོག new_mod;
}

bool RTLIL::Module::has_memories() const
{
	ལོག !memories.empty();
}

bool RTLIL::Module::has_processes() const
{
	ལོག !processes.empty();
}

bool RTLIL::Module::has_memories_warn() const
{
	གལ་སྲིད (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", this);
	ལོག !memories.empty();
}

bool RTLIL::Module::has_processes_warn() const
{
	གལ་སྲིད (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", this);
	ལོག !processes.empty();
}

bool RTLIL::Module::is_selected() const
{
	ལོག design->selected_module(this->name);
}

bool RTLIL::Module::is_selected_whole() const
{
	ལོག design->selected_whole_module(this->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() const
{
	std::vector<RTLIL::Wire*> result;
	result.reserve(wires_.size());
	དོན་ལུ (auto &it : wires_)
		གལ་སྲིད (design->selected(this, it.second))
			result.push_back(it.second);
	ལོག result;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() const
{
	std::vector<RTLIL::Cell*> result;
	result.reserve(cells_.size());
	དོན་ལུ (auto &it : cells_)
		གལ་སྲིད (design->selected(this, it.second))
			result.push_back(it.second);
	ལོག result;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() const
{
	std::vector<RTLIL::Memory*> result;
	result.reserve(memories.size());
	དོན་ལུ (auto &it : memories)
		གལ་སྲིད (design->selected(this, it.second))
			result.push_back(it.second);
	ལོག result;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() const
{
	std::vector<RTLIL::Process*> result;
	result.reserve(processes.size());
	དོན་ལུ (auto &it : processes)
		གལ་སྲིད (design->selected(this, it.second))
			result.push_back(it.second);
	ལོག result;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() const
{
	std::vector<RTLIL::SelectedMember> result;
	དོན་ལུ (auto cell : selected_cells())
		result.push_back({cell, cell->name});
	དོན་ལུ (auto memory : selected_memories())
		result.push_back({memory, memory->name});
	དོན་ལུ (auto wire : selected_wires())
		result.push_back({wire, wire->name});
	དོན་ལུ (auto process : selected_processes())
		result.push_back({process, process->name});
	ལོག result;
}

void RTLIL::Module::add(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(count_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = this;
}

void RTLIL::Module::add(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(count_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = this;
}

void RTLIL::Module::add(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(count_id(process->name) == 0);
	processes[process->name] = process;
	process->module = this;
}

void RTLIL::Module::remove(const pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	གལ་སྲིད (wires.empty())
		ལོག;

	sa_nracanaa DeleteWireWorker
	{
		RTLIL::Module *module;
		const pool<RTLIL::Wire*> *wires_p;

		void operator()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([this](RTLIL::Wire *&wire) {
				གལ་སྲིད (wires_p->count(wire))
					wire = module->addWire(stringf("$delete_wire$%d", (ཧྲིལ)autoidx++), wire->width);
			});
		}

		void operator()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	DeleteWireWorker delete_wire_worker;
	delete_wire_worker.module = this;
	delete_wire_worker.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_worker);

	གལ་སྲིད (design->flagBufferedNormalized) {
		དོན་ལུ (auto wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_index.erase(wire);
		}
	}

	དོན་ལུ (auto &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.count(id) != 0);
		wires_.erase(id);
		delete it;
	}
}

void RTLIL::Module::remove(RTLIL::Cell *cell)
{
	སྐབས (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.count(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	གལ་སྲིད (design && design->flagBufferedNormalized && buf_norm_cell_queue.count(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.insert(cell);
	} དེ་མིན {
		delete cell;
	}
}

void RTLIL::Module::remove(RTLIL::Memory *memory)
{
	log_assert(memories.count(memory->name) != 0);
	memories.erase(memory->name);
	delete memory;
}

void RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.count(process->name) != 0);
	processes.erase(process->name);
	delete process;
}

void RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	add(wire);
}

void RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	add(cell);
}

void RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	གལ་སྲིད (old_id != IdString::Null && wires_.count(old_id))
		rename(wires_.at(old_id), new_name);
	དེ་མིན གལ་སྲིད (old_id != IdString::Null && cells_.count(old_id))
		rename(cells_.at(old_id), new_name);
	དེ་མིན
		log_abort();
}

void RTLIL::Module::swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2)
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

void RTLIL::Module::swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	ཧྲིལ index = 0;
	ལོག uniquify(name, index);
}

IdString RTLIL::Module::uniquify(IdString name, ཧྲིལ &index)
{
	གལ་སྲིད (index == 0) {
		གལ་སྲིད (count_id(name) == 0)
			ལོག name;
		index++;
	}

	སྐབས (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", index)});
		གལ་སྲིད (count_id(new_name) == 0)
			ལོག new_name;
		index++;
	}
}

static bool fixup_ports_compare(const RTLIL::Wire *a, const RTLIL::Wire *b)
{
	གལ་སྲིད (a->port_id && !b->port_id)
		ལོག true;
	གལ་སྲིད (!a->port_id && b->port_id)
		ལོག false;

	གལ་སྲིད (a->port_id == b->port_id)
		ལོག a->name < b->name;
	ལོག a->port_id < b->port_id;
}

void RTLIL::Module::connect(const RTLIL::SigSig &conn)
{
	དོན་ལུ (auto mon : monitors)
		mon->notify_connect(this, conn);

	གལ་སྲིད (design)
		དོན་ལུ (auto mon : design->monitors)
			mon->notify_connect(this, conn);

	// ignore all attempts to assign constants to other constants
	གལ་སྲིད (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		དོན་ལུ (ཧྲིལ i = 0; i < GetSize(conn.first); i++)
			གལ་སྲིད (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		གལ་སྲིད (GetSize(new_conn.first))
			connect(new_conn);
		ལོག;
	}

	གལ་སྲིད (yosys_xtrace) {
		log("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", this, log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(GetSize(conn.first) == GetSize(conn.second));
	connections_.push_back(conn);
}

void RTLIL::Module::connect(const RTLIL::SigSpec &lhs, const RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

void RTLIL::Module::new_connections(const std::vector<RTLIL::SigSig> &new_conn)
{
	དོན་ལུ (auto mon : monitors)
		mon->notify_connect(this, new_conn);

	གལ་སྲིད (design)
		དོན་ལུ (auto mon : design->monitors)
			mon->notify_connect(this, new_conn);

	གལ་སྲིད (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", this);
		དོན་ལུ (auto &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

const std::vector<RTLIL::SigSig> &RTLIL::Module::connections() const
{
	ལོག connections_;
}

void RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	དོན་ལུ (auto &w : wires_)
		གལ་སྲིད (w.second->port_input || w.second->port_output)
			all_ports.push_back(w.second);
		དེ་མིན
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	གལ་སྲིད (design && design->flagBufferedNormalized) {
		དོན་ལུ (auto &w : wires_)
			གལ་སྲིད (w.second->driverCell_ && w.second->driverCell_->type == ID($input_port))
				buf_norm_wire_queue.insert(w.second);

		buf_norm_wire_queue.insert(all_ports.begin(), all_ports.end());
	}

	ports.clear();
	དོན་ལུ (size_t i = 0; i < all_ports.size(); i++) {
		ports.push_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
	}
}

void RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		const dict<IdString, RTLIL::Const> &src,
		const RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	གལ་སྲིད (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		ལོག;
	}
	dst.clear();
	དོན་ལུ (ཧྲིལ i = GetSize(src) - 1; i >= 0; i--) {
		auto &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::addWire(RTLIL::IdString name, ཧྲིལ width)
{
	log_assert(design);
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	RTLIL::Wire *wire = new RTLIL::Wire;
	wire->width = width;
	wire->name = name;
	add(wire);
	ལོག wire;
}

RTLIL::Wire *RTLIL::Module::addWire(IdString name, const RTLIL::Wire *other)
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
		const RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
		RTLIL::copy_attr_dict(wire->attributes, other->attributes, src_design, this->design);
	}
	ལོག wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = new RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	ལོག cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, const RTLIL::Cell *other)
{
	const RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	bool cross_pool = src_design && this->design && src_design != this->design;

	IdString type = other->type.ref();
	གལ་སྲིད (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, this->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, this->design);

	གལ་སྲིད (!cross_pool)
		cell->connections_ = other->connections_;
	དེ་མིན
		དོན་ལུ (ཧྲིལ i = GetSize(other->connections_) - 1; i >= 0; i--) {
			auto &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	ལོག cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = new RTLIL::Memory;
	mem->module = this;
	mem->name = name;
	memories[name] = mem;
	ལོག mem;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name, const RTLIL::Memory *other)
{
	log_assert(design);
	RTLIL::Memory *mem = new RTLIL::Memory;
	mem->module = this;
	mem->name = name;
	mem->width = other->width;
	mem->start_offset = other->start_offset;
	mem->size = other->size;
	RTLIL::copy_attr_dict(mem->attributes, other->attributes,
			other->module ? other->module->design : nullptr, design);
	memories[name] = mem;
	ལོག mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = new RTLIL::Process;
	proc->name = name;
	add(proc);
	ལོག proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, const RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	const RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	གལ་སྲིད (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		དོན་ལུ (auto *sync : proc->syncs)
			དོན་ལུ (auto &mwa : sync->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	ལོག proc;
}

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_y, bool is_signed, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		ལོག cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, bool is_signed, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		ལོག sig_y;                                       \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_y, bool /* is_signed */, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::WIDTH] = sig_a.size();         \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		ལོག cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, bool is_signed, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		ལོག sig_y;                                       \
	}
DEF_METHOD(Buf,     sig_a.size(), ID($buf))
DEF_METHOD(Barrier, sig_a.size(), ID($barrier))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, const RTLIL::SigSpec &sig_y, bool is_signed, const std::string &src) { \
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
		ལོག cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, bool is_signed, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ལོག sig_y;                                            \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, const RTLIL::SigSpec &sig_y, bool is_signed, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = false;             \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		ལོག cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, bool is_signed, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ལོག sig_y;                                            \
	}
DEF_METHOD(Shl,      sig_a.size(), ID($shl))
DEF_METHOD(Shr,      sig_a.size(), ID($shr))
DEF_METHOD(Sshl,     sig_a.size(), ID($sshl))
DEF_METHOD(Sshr,     sig_a.size(), ID($sshr))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, const RTLIL::SigSpec &sig_y, bool is_signed, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = false;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		ལོག cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, bool is_signed, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ལོག sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.size(), ID($shiftx))
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, const RTLIL::SigSpec &sig_s, const RTLIL::SigSpec &sig_y, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		གལ་སྲིད (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.size();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		ལོག cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, const RTLIL::SigSpec &sig_s, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		ལོག sig_y;                                             \
	}
DEF_METHOD(Mux,      ID($mux),        0)
DEF_METHOD(Bwmux,    ID($bwmux),      0)
DEF_METHOD(Pmux,     ID($pmux),       1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_s, const RTLIL::SigSpec &sig_y, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = _demux ? sig_a.size() : sig_y.size(); \
		cell->parameters[ID::S_WIDTH] = sig_s.size();             \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		ལོག cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_s, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.size() << sig_s.size() : sig_a.size() >> sig_s.size()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		ལོག sig_y;                                             \
	}
DEF_METHOD(Bmux,     ID($bmux),       0)
DEF_METHOD(Demux,    ID($demux),      1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, const RTLIL::SigSpec &sig_y, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		ལོག cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_s, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		ལོག sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#undef DEF_METHOD

#define DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const RTLIL::SigBit &sig2, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		ལོག cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		ལོག sig2;                                      \
	}
#define DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const RTLIL::SigBit &sig2, const RTLIL::SigBit &sig3, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		ལོག cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const RTLIL::SigBit &sig2, const std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		ལོག sig3;                                      \
	}
#define DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const RTLIL::SigBit &sig2, const RTLIL::SigBit &sig3, const RTLIL::SigBit &sig4, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		ལོག cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const RTLIL::SigBit &sig2, const RTLIL::SigBit &sig3, const std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		ལོག sig4;                                      \
	}
#define DEF_METHOD_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const RTLIL::SigBit &sig2, const RTLIL::SigBit &sig3, const RTLIL::SigBit &sig4, const RTLIL::SigBit &sig5, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->setPort("\\" #_P5, sig5);                   \
		cell->set_src_attribute(src);                     \
		ལོག cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const RTLIL::SigBit &sig2, const RTLIL::SigBit &sig3, const RTLIL::SigBit &sig4, const std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		ལོག sig5;                                           \
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

RTLIL::Cell* RTLIL::Module::addPow(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, const RTLIL::SigSpec &sig_y, bool a_signed, bool b_signed, const std::string &src)
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
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addFa(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, const RTLIL::SigSpec &sig_c, const RTLIL::SigSpec &sig_x, const RTLIL::SigSpec &sig_y, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fa));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::C, sig_c);
	cell->setPort(ID::X, sig_x);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addSlice(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_y, RTLIL::Const offset, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($slice));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::Y_WIDTH] = sig_y.size();
	cell->parameters[ID::OFFSET] = offset;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addConcat(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, const RTLIL::SigSpec &sig_y, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($concat));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::B_WIDTH] = sig_b.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_y, RTLIL::Const lut, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_y, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_en, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assert));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_en, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_en, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_en, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_en, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, const RTLIL::SigSpec &sig_y, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addSr(RTLIL::IdString name, const RTLIL::SigSpec &sig_set, const RTLIL::SigSpec &sig_clr, const RTLIL::SigSpec &sig_q, bool set_polarity, bool clr_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sr));
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addDff(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, bool clk_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addDffe(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, bool clk_polarity, bool en_polarity, const std::string &src)
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
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addDffsr(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_set, const RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, const RTLIL::SigSpec &sig_q, bool clk_polarity, bool set_polarity, bool clr_polarity, const std::string &src)
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
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addDffsre(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_set, const RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, const RTLIL::SigSpec &sig_q, bool clk_polarity, bool en_polarity, bool set_polarity, bool clr_polarity, const std::string &src)
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
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addAdff(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_arst, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_value, bool clk_polarity, bool arst_polarity, const std::string &src)
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
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addAdffe(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_arst, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_value, bool clk_polarity, bool en_polarity, bool arst_polarity, const std::string &src)
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
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addAldff(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_aload, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		const RTLIL::SigSpec &sig_ad, bool clk_polarity, bool aload_polarity, const std::string &src)
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
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addAldffe(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_aload, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		const RTLIL::SigSpec &sig_ad, bool clk_polarity, bool en_polarity, bool aload_polarity, const std::string &src)
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
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addSdff(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_srst, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_value, bool clk_polarity, bool srst_polarity, const std::string &src)
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
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addSdffe(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_srst, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_value, bool clk_polarity, bool en_polarity, bool srst_polarity, const std::string &src)
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
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addSdffce(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_srst, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_value, bool clk_polarity, bool en_polarity, bool srst_polarity, const std::string &src)
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
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addDlatch(RTLIL::IdString name, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, bool en_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatch(RTLIL::IdString name, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_arst, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_value, bool en_polarity, bool arst_polarity, const std::string &src)
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
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsr(RTLIL::IdString name, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_set, const RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, const RTLIL::SigSpec &sig_q, bool en_polarity, bool set_polarity, bool clr_polarity, const std::string &src)
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
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_set, const RTLIL::SigSpec &sig_clr,
		const RTLIL::SigSpec &sig_q, bool set_polarity, bool clr_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, bool clk_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, bool clk_polarity, bool en_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addDffsrGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_set, const RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, const RTLIL::SigSpec &sig_q, bool clk_polarity, bool set_polarity, bool clr_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addDffsreGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_set, const RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, const RTLIL::SigSpec &sig_q, bool clk_polarity, bool en_polarity, bool set_polarity, bool clr_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addAdffGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_arst, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		bool arst_value, bool clk_polarity, bool arst_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_value ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addAdffeGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_arst, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		bool arst_value, bool clk_polarity, bool en_polarity, bool arst_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_value ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addAldffGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_aload, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		const RTLIL::SigSpec &sig_ad, bool clk_polarity, bool aload_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addAldffeGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_aload, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		const RTLIL::SigSpec &sig_ad, bool clk_polarity, bool en_polarity, bool aload_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addSdffGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_srst, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		bool srst_value, bool clk_polarity, bool srst_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_value ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addSdffeGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_srst, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		bool srst_value, bool clk_polarity, bool en_polarity, bool srst_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_value ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addSdffceGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_srst, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		bool srst_value, bool clk_polarity, bool en_polarity, bool srst_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_value ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, bool en_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatchGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_arst, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q,
		bool arst_value, bool en_polarity, bool arst_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_value ? '1' : '0'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsrGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_set, const RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, const RTLIL::SigSpec &sig_q, bool en_polarity, bool set_polarity, bool clr_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, ཧྲིལ width, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ལོག sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, ཧྲིལ width, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ལོག sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, ཧྲིལ width, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ལོག sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, ཧྲིལ width, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ལོག sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ལོག sig;
}

RTLIL::SigSpec RTLIL::Module::SetTag(RTLIL::IdString name, const std::string &tag, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_s, const RTLIL::SigSpec &sig_c, const std::string &src)
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
	ལོག sig;
}

RTLIL::Cell* RTLIL::Module::addSetTag(RTLIL::IdString name, const std::string &tag, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_s, const RTLIL::SigSpec &sig_c, const RTLIL::SigSpec &sig_y, const std::string &src)
{
	Cell *cell = addCell(name, ID($set_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::SigSpec RTLIL::Module::GetTag(RTLIL::IdString name, const std::string &tag, const RTLIL::SigSpec &sig_a, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($get_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ལོག sig;
}

RTLIL::Cell* RTLIL::Module::addOverwriteTag(RTLIL::IdString name, const std::string &tag, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_s, const RTLIL::SigSpec &sig_c, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($overwrite_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->set_src_attribute(src);
	ལོག cell;
}

RTLIL::SigSpec RTLIL::Module::OriginalTag(RTLIL::IdString name, const std::string &tag, const RTLIL::SigSpec &sig_a, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($original_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ལོག sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, const RTLIL::SigSpec &sig_e, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.size());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.size();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ལོག sig;
}

std::string RTLIL::Module::to_rtlil_str() const
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(this), design, false, true, false, RTLIL_BACKEND::DumpMode::Readable);
	ལོག f.str();
}

RTLIL::Wire::Wire()
{
	static unsigned ཧྲིལ hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	module = nullptr;
	width = 1;
	start_offset = 0;
	port_id = 0;
	port_input = false;
	port_output = false;
	upto = false;
	is_signed = false;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->insert(std::pair<unsigned ཧྲིལ, RTLIL::Wire*>(hashidx_, this));
#endif
}

RTLIL::Wire::~Wire()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#endif
}

std::string RTLIL::Wire::to_rtlil_str() const
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", this, design(), RTLIL_BACKEND::DumpMode::Readable);
	ལོག f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
static std::map<unsigned ཧྲིལ, RTLIL::Wire*> all_wires;
std::map<unsigned ཧྲིལ, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(void)
{
	ལོག &all_wires;
}
#endif

RTLIL::Memory::Memory()
{
	static unsigned ཧྲིལ hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	width = 1;
	start_offset = 0;
	size = 0;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->insert(std::pair<unsigned ཧྲིལ, RTLIL::Memory*>(hashidx_, this));
#endif
}

std::string RTLIL::Memory::to_rtlil_str() const
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", this, design(), RTLIL_BACKEND::DumpMode::Readable);
	ལོག f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	static unsigned ཧྲིལ hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;
}

std::string RTLIL::Process::to_rtlil_str() const
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", this, design(), RTLIL_BACKEND::DumpMode::Readable);
	ལོག f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	static unsigned ཧྲིལ hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	// log("#memtrace# %p\n", this);
	memhasher();

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->insert(std::pair<unsigned ཧྲིལ, RTLIL::Cell*>(hashidx_, this));
#endif
}

RTLIL::Cell::~Cell()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#endif
}

std::string RTLIL::Cell::to_rtlil_str() const
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", this, design(), RTLIL_BACKEND::DumpMode::Readable);
	ལོག f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
static std::map<unsigned ཧྲིལ, RTLIL::Cell*> all_cells;
std::map<unsigned ཧྲིལ, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(void)
{
	ལོག &all_cells;
}
#endif

bool RTLIL::Cell::hasPort(RTLIL::IdString portname) const
{
	ལོག connections_.count(portname) != 0;
}

// bufnorm

const RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) const
{
	ལོག connections_.at(portname);
}

const dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() const
{
	ལོག connections_;
}

bool RTLIL::Cell::known() const
{
	གལ་སྲིད (yosys_celltypes.cell_known(type))
		ལོག true;
	གལ་སྲིད (module && module->design && module->design->module(type))
		ལོག true;
	ལོག false;
}

bool RTLIL::Cell::input(IdString portname) const
{
	གལ་སྲིད (yosys_celltypes.cell_known(type))
		ལོག yosys_celltypes.cell_input(type.ref(), portname);
	གལ་སྲིད (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		ལོག w && w->port_input;
	}
	ལོག false;
}

bool RTLIL::Cell::output(RTLIL::IdString portname) const
{
	གལ་སྲིད (yosys_celltypes.cell_known(type))
		ལོག yosys_celltypes.cell_output(type, portname);
	གལ་སྲིད (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		ལོག w && w->port_output;
	}
	ལོག false;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) const
{
	གལ་སྲིད (yosys_celltypes.cell_known(type.ref()))
		ལོག yosys_celltypes.cell_port_dir(type, portname);
	གལ་སྲིད (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		གལ་སྲིད (m == nullptr)
			ལོག PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		གལ་སྲིད (w == nullptr)
			ལོག PortDir::PD_UNKNOWN;
		ལོག PortDir(w->port_input + w->port_output * 2);
	}
	ལོག PortDir::PD_UNKNOWN;
}

bool RTLIL::Cell::hasParam(RTLIL::IdString paramname) const
{
	ལོག parameters.count(paramname) != 0;
}

void RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

void RTLIL::Cell::setParam(RTLIL::IdString paramname, RTLIL::Const value)
{
	parameters[paramname] = std::move(value);
}

const RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) const
{
	const auto &it = parameters.find(paramname);
	གལ་སྲིད (it != parameters.end())
		ལོག it->second;
	གལ་སྲིད (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		གལ་སྲིད (m)
			ལོག m->parameter_default_values.at(paramname);
	}
	k_sipa std::out_of_range("Cell::getParam()");
}

void RTLIL::Cell::sort()
{
	connections_.sort(sort_by_id_str(module->twines()));
	parameters.sort(sort_by_id_str(module->twines()));
	attributes.sort(sort_by_id_str(module->twines()));
}

void RTLIL::Cell::check()
{
#ifndef NDEBUG
	InternalCellChecker checker(NULL, this);
	checker.check();
#endif
}

void RTLIL::Cell::fixup_parameters(bool set_a_signed, bool set_b_signed)
{
	གལ་སྲིད (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		ལོག;

	གལ་སྲིད (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		གལ་སྲིད (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		ལོག;
	}

	གལ་སྲིད (type == ID($demux)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		ལོག;
	}

	གལ་སྲིད (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		ལོག;
	}

	གལ་སྲིད (type == ID($fa)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		ལོག;
	}

	གལ་སྲིད (type == ID($lcu)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::CO]);
		ལོག;
	}

	གལ་སྲིད (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);
		ལོག;
	}

	bool signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	གལ་སྲིད (connections_.count(ID::A)) {
		གལ་སྲིད (signedness_ab) {
			གལ་སྲིད (set_a_signed)
				parameters[ID::A_SIGNED] = true;
			དེ་མིན གལ་སྲིད (parameters.count(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = false;
		}
		parameters[ID::A_WIDTH] = GetSize(connections_[ID::A]);
	}

	གལ་སྲིད (connections_.count(ID::B)) {
		གལ་སྲིད (signedness_ab) {
			གལ་སྲིད (set_b_signed)
				parameters[ID::B_SIGNED] = true;
			དེ་མིན གལ་སྲིད (parameters.count(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = false;
		}
		parameters[ID::B_WIDTH] = GetSize(connections_[ID::B]);
	}

	གལ་སྲིད (connections_.count(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);

	གལ་སྲིད (connections_.count(ID::Q))
		parameters[ID::WIDTH] = GetSize(connections_[ID::Q]);

	check();
}

bool RTLIL::Cell::has_keep_attr() const {
	ལོག get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

bool RTLIL::Cell::has_memid() const
{
	ལོག type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

bool RTLIL::Cell::is_mem_cell() const
{
	ལོག type.in(ID($mem), ID($mem_v2)) || has_memid();
}

bool RTLIL::Cell::is_builtin_ff() const {
	ལོག StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(const RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	གལ་སྲིད (wire == NULL)
		data = {bit.data};
	དེ་མིན
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(ཧྲིལ offset, ཧྲིལ length) const
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= width);
	RTLIL::SigChunk ret;
	གལ་སྲིད (wire) {
		ret.wire = wire;
		ret.offset = this->offset + offset;
		ret.width = length;
	} དེ་མིན {
		དོན་ལུ (ཧྲིལ i = 0; i < length; i++)
			ret.data.push_back(data[offset+i]);
		ret.width = length;
	}
	ལོག ret;
}

RTLIL::SigBit RTLIL::SigChunk::operator[](ཧྲིལ offset) const
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	གལ་སྲིད (wire) {
		ret.wire = wire;
		ret.offset = this->offset + offset;
	} དེ་མིན {
		ret.data = data[offset];
	}
	ལོག ret;
}

bool RTLIL::SigChunk::operator <(const RTLIL::SigChunk &other) const
{
	གལ་སྲིད (wire && other.wire)
		གལ་སྲིད (wire->name != other.wire->name)
			ལོག wire->name < other.wire->name;

	གལ་སྲིད (wire != other.wire)
		ལོག wire < other.wire;

	གལ་སྲིད (offset != other.offset)
		ལོག offset < other.offset;

	གལ་སྲིད (width != other.width)
		ལོག width < other.width;

	ལོག data < other.data;
}

bool RTLIL::SigChunk::operator ==(const RTLIL::SigChunk &other) const
{
	ལོག wire == other.wire && width == other.width && offset == other.offset && data == other.data;
}

bool RTLIL::SigChunk::operator !=(const RTLIL::SigChunk &other) const
{
	གལ་སྲིད (*this == other)
		ལོག false;
	ལོག true;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.size() > 0);
	auto ie = parts.begin();
	auto it = ie + parts.size() - 1;
	སྐབས (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(const RTLIL::Const &value)
{
	གལ་སྲིད (GetSize(value) != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(value);
	} དེ་མིན {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&value)
{
	གལ་སྲིད (GetSize(value) != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(value);
	} དེ་མིན {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(const RTLIL::SigChunk &chunk)
{
	གལ་སྲིད (chunk.width != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(chunk);
	} དེ་མིན {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	གལ་སྲིད (chunk.width != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(chunk);
	} དེ་མིན {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	གལ་སྲིད (wire->width != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(wire);
	} དེ་མིན {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, ཧྲིལ offset, ཧྲིལ width)
{
	གལ་སྲིད (width != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} དེ་མིན {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(const std::string &str)
{
	གལ་སྲིད (str.size() != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(str);
	} དེ་མིན {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ཧྲིལ val, ཧྲིལ width)
{
	གལ་སྲིད (width != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(val, width);
	} དེ་མིན
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, ཧྲིལ width)
{
	གལ་སྲིད (width != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(bit, width);
	} དེ་མིན
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(const RTLIL::SigBit &bit, ཧྲིལ width)
{
	གལ་སྲིད (width != 0) {
		གལ་སྲིད (bit.wire == NULL) {
			rep_ = CHUNK;
			new (&chunk_) RTLIL::SigChunk(bit.data, width);
		} དེ་མིན གལ་སྲིད (width == 1) {
			rep_ = CHUNK;
			new (&chunk_) RTLIL::SigChunk(bit);
		} དེ་མིན {
			init_empty_bits();
			bits_.reserve(width);
			དོན་ལུ (ཧྲིལ i = 0; i < width; i++)
				bits_.push_back(bit);
		}
	} དེ་མིན
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(const std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	དོན་ལུ (const auto &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(const std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	དོན་ལུ (const auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(const pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	དོན་ལུ (const auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(const std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	དོན་ལུ (const auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(bool bit)
{
	rep_ = CHUNK;
	new (&chunk_) RTLIL::SigChunk(bit ? RTLIL::S1 : RTLIL::S0);
	check();
}

void RTLIL::SigSpec::Chunks::const_iterator::next_chunk_bits()
{
	ཧྲིལ bits_size = GetSize(spec.bits_);
	གལ་སྲིད (bit_index >= bits_size)
		ལོག;
	ཧྲིལ i = bit_index;
	const SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.data.clear();
	གལ་སྲིད (bit.is_wire()) {
		chunk.offset = bit.offset;
		སྐབས (i < bits_size && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_index)
			++i;
	} དེ་མིན {
		chunk.offset = 0;
		chunk.data.push_back(bit.data);
		སྐབས (i < bits_size && !spec.bits_[i].is_wire()) {
			chunk.data.push_back(spec.bits_[i].data);
			++i;
		}
	}
	chunk.width = i - bit_index;
}

void RTLIL::SigSpec::unpack()
{
	གལ་སྲིད (rep_ == BITS)
		ལོག;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	དོན་ལུ (ཧྲིལ i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	new (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

void RTLIL::SigSpec::try_repack()
{
	གལ་སྲིད (rep_ != BITS)
		ལོག;

	ཧྲིལ bits_size = GetSize(bits_);
	གལ་སྲིད (bits_size == 0)
		ལོག;
	གལ་སྲིད (bits_[0].is_wire()) {
		དོན་ལུ (ཧྲིལ i = 1; i < bits_size; i++)
			གལ་སྲིད (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				ལོག;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_size);
		bits_.~vector();
		rep_ = CHUNK;
		new (&chunk_) SigChunk(std::move(chunk));
		ལོག;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_size);
	bits.push_back(bits_[0].data);
	དོན་ལུ (ཧྲིལ i = 1; i < bits_size; i++) {
		གལ་སྲིད (bits_[i].is_wire())
			ལོག;
		bits.push_back(bits_[i].data);
	}
	bits_.~vector();
	rep_ = CHUNK;
	new (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() const
{
	Hasher h;
	དོན་ལུ (auto &c : chunks())
		གལ་སྲིད (c.wire == NULL) {
			དོན་ལུ (auto &v : c.data)
				h.eat(v);
		} དེ་མིན {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t result = h.yield();
	གལ་སྲིད (result == 0)
		result = 1;
	hash_.set(result);
	ལོག result;
}

void RTLIL::SigSpec::sort()
{
	unpack();
	std::sort(bits_.begin(), bits_.end());
	hash_.clear();
	try_repack();
}

void RTLIL::SigSpec::sort_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::vector<SigBit> unique_bits = bits_;
	std::sort(unique_bits.begin(), unique_bits.end());
	auto last = std::unique(unique_bits.begin(), unique_bits.end());
	unique_bits.erase(last, unique_bits.end());

	*this = unique_bits;
	hash_.clear();
	try_repack();
}

void RTLIL::SigSpec::replace(const RTLIL::SigSpec &pattern, const RTLIL::SigSpec &with)
{
	replace(pattern, with, this);
}

void RTLIL::SigSpec::replace(const RTLIL::SigSpec &pattern, const RTLIL::SigSpec &with, RTLIL::SigSpec *other) const
{
	log_assert(other != NULL);
	log_assert(size() == other->size());
	log_assert(pattern.size() == with.size());

	dict<RTLIL::SigBit, ཧྲིལ> pattern_to_with;
	ཧྲིལ pattern_size = pattern.size();
	དོན་ལུ (ཧྲིལ i = 0; i < pattern_size; i++) {
		SigBit pattern_bit = pattern[i];
		གལ་སྲིད (pattern_bit.wire != NULL) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	ཧྲིལ this_size = size();
	bool other_modified = false;
	དོན་ལུ (ཧྲིལ j = 0; j < this_size; j++) {
		auto it = pattern_to_with.find((*this)[j]);
		གལ་སྲིད (it != pattern_to_with.end()) {
			other_modified = true;
			(*other)[j] = with[it->second];
		}
	}

	གལ་སྲིད (other_modified)
		other->try_repack();
	other->check();
}

void RTLIL::SigSpec::replace(const dict<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, this);
}

void RTLIL::SigSpec::replace(const dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) const
{
	log_assert(other != NULL);
	log_assert(size() == other->size());

	གལ་སྲིད (rules.empty()) ལོག;

	ཧྲིལ this_size = size();
	bool other_modified = false;
	དོན་ལུ (ཧྲིལ i = 0; i < this_size; i++) {
		auto it = rules.find((*this)[i]);
		གལ་སྲིད (it != rules.end()) {
			other_modified = true;
			(*other)[i] = it->second;
		}
	}

	གལ་སྲིད (other_modified)
		other->try_repack();
	other->check();
}

void RTLIL::SigSpec::replace(const std::map<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, this);
}

void RTLIL::SigSpec::replace(const std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) const
{
	log_assert(other != NULL);
	log_assert(size() == other->size());

	གལ་སྲིད (rules.empty()) ལོག;

	ཧྲིལ this_size = size();
	bool other_modified = false;
	དོན་ལུ (ཧྲིལ i = 0; i < this_size; i++) {
		auto it = rules.find((*this)[i]);
		གལ་སྲིད (it != rules.end()) {
			other_modified = true;
			(*other)[i] = it->second;
		}
	}

	གལ་སྲིད (other_modified)
		other->try_repack();
	other->check();
}

void RTLIL::SigSpec::remove(const RTLIL::SigSpec &pattern)
{
	remove2(pattern, NULL);
}

void RTLIL::SigSpec::remove(const RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) const
{
	RTLIL::SigSpec tmp = *this;
	tmp.remove2(pattern, other);
}

void RTLIL::SigSpec::remove2(const RTLIL::SigSpec &pattern, RTLIL::SigSpec *other)
{
	unpack();
	གལ་སྲིད (other != NULL) {
		log_assert(size() == other->size());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.size());
	དོན་ལུ (auto &bit : pattern)
		གལ་སྲིད (bit.wire != NULL)
			pattern_bits.insert(bit);

	// Compact in-place to avoid O(n^2) erase operations
	size_t write_idx = 0;
	དོན་ལུ (size_t read_idx = 0; read_idx < bits_.size(); read_idx++)
	{
		གལ་སྲིད (!(bits_[read_idx].wire != NULL && pattern_bits.count(bits_[read_idx]))) {
			གལ་སྲིད (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				གལ་སྲིད (other != NULL)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	bool modified = (write_idx < bits_.size());
	གལ་སྲིད (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	གལ་སྲིད (other != NULL && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

void RTLIL::SigSpec::remove(const pool<RTLIL::SigBit> &pattern)
{
	remove2(pattern, NULL);
}

void RTLIL::SigSpec::remove(const pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) const
{
	RTLIL::SigSpec tmp = *this;
	tmp.remove2(pattern, other);
}

void RTLIL::SigSpec::remove2(const pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	གལ་སྲིད (other != NULL) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	དོན་ལུ (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		གལ་སྲིད (!(bits_[read_idx].wire != NULL && pattern.count(bits_[read_idx]))) {
			གལ་སྲིད (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				གལ་སྲིད (other != NULL)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	bool modified = (write_idx < bits_.size());
	གལ་སྲིད (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	གལ་སྲིད (other != NULL && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

void RTLIL::SigSpec::remove2(const std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	གལ་སྲིད (other != NULL) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	དོན་ལུ (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		གལ་སྲིད (!(bits_[read_idx].wire != NULL && pattern.count(bits_[read_idx]))) {
			གལ་སྲིད (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				གལ་སྲིད (other != NULL)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	bool modified = (write_idx < bits_.size());
	གལ་སྲིད (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	གལ་སྲིད (other != NULL && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

void RTLIL::SigSpec::remove2(const pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	གལ་སྲིད (other != NULL) {
		log_assert(size() == other->size());
		other->unpack();
	}

	bool modified = false;
	bool other_modified = false;
	དོན་ལུ (ཧྲིལ i = GetSize(bits_) - 1; i >= 0; i--) {
		གལ་སྲིད (bits_[i].wire != NULL && pattern.count(bits_[i].wire)) {
			modified = true;
			bits_.erase(bits_.begin() + i);
			གལ་སྲིད (other != NULL) {
				other_modified = true;
				other->bits_.erase(other->bits_.begin() + i);
			}
		}
	}

	གལ་སྲིད (modified) {
		hash_.clear();
		try_repack();
	}
	གལ་སྲིད (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(const RTLIL::SigSpec &pattern, const RTLIL::SigSpec *other) const
{
	log_assert(other == NULL || size() == other->size());

	RTLIL::SigSpec ret;
	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();

	དོན་ལུ (auto& pattern_chunk : pattern.chunks()) {
		གལ་སྲིད (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			ཧྲིལ i = 0;
			དོན་ལུ (const RTLIL::SigBit &bit : bits_match) {
				གལ་སྲིད (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} དེ་མིན {
			དོན་ལུ (const RTLIL::SigBit &bit : bits_match)
				གལ་སྲིད (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	ལོག ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(const pool<RTLIL::SigBit> &pattern, const RTLIL::SigSpec *other) const
{
	log_assert(other == NULL || size() == other->size());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	གལ་སྲིད (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		ཧྲིལ i = 0;
		དོན་ལུ (const RTLIL::SigBit &bit : bits_match) {
			གལ་སྲིད (bit.wire && pattern.count(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} དེ་མིན {
		དོན་ལུ (const RTLIL::SigBit &bit : bits_match)
			གལ་སྲིད (bit.wire && pattern.count(bit))
				ret.append(bit);
	}

	ret.check();
	ལོག ret;
}

void RTLIL::SigSpec::replace(ཧྲིལ offset, const RTLIL::SigSpec &with)
{
	གལ་སྲིད (with.size() == 0)
		ལོག;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.size() >= 0);
	log_assert(offset+with.size() <= size());

	ཧྲིལ i = 0;
	དོན་ལུ (const RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

void RTLIL::SigSpec::remove_const()
{
	གལ་སྲིད (rep_ == CHUNK)
	{
		གལ་སྲིད (chunk_.wire == NULL) {
			chunk_.~SigChunk();
			init_empty_bits();
			hash_.clear();
		}
	}
	དེ་མིན
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.size());
		དོན་ལུ (auto &bit : bits_)
			གལ་སྲིད (bit.wire != NULL)
				new_bits.push_back(bit);
		གལ་སྲིད (GetSize(new_bits) != GetSize(bits_)) {
			bits_.swap(new_bits);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

void RTLIL::SigSpec::remove(ཧྲིལ offset, ཧྲིལ length)
{
	གལ་སྲིད (length == 0)
		ལོག;

	unpack();

	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	bits_.erase(bits_.begin() + offset, bits_.begin() + offset + length);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ཧྲིལ offset, ཧྲིལ length) const
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	std::vector<SigBit> extracted;
	SigBit first;
	bool is_packing = true;
	དོན་ལུ (ཧྲིལ i = offset; i < offset + length; i++) {
		bool was_packing_before = is_packing;
		SigBit bit = (*this)[i];
		གལ་སྲིད (i == offset) {
			first = bit;
			གལ་སྲིད (!bit.wire)
				is_packing = false;
		} དེ་མིན {
			གལ་སྲིད (bit.wire != first.wire)
				is_packing = false;
			གལ་སྲིད (bit.wire)
				གལ་སྲིད (bit.offset != first.offset + (i - offset))
					is_packing = false;
		}
		གལ་སྲིད (was_packing_before && !is_packing)
			དོན་ལུ (ཧྲིལ j = offset; j < i; j++)
				extracted.push_back((*this)[j]);
		གལ་སྲིད (!is_packing)
			extracted.push_back((*this)[i]);
	}
	གལ་སྲིད (is_packing)
		ལོག SigChunk(first.wire, first.offset, length);

	ལོག extracted;
}

void RTLIL::SigSpec::rewrite_wires(std::function<void(RTLIL::Wire*& wire)> rewrite)
{
	གལ་སྲིད (rep_ == CHUNK) {
		གལ་སྲིད (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		ལོག;
	}

	std::vector<RTLIL::SigBit> new_bits;
	དོན་ལུ (const RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		གལ་སྲིད (c.wire != nullptr)
			rewrite(c.wire);
		དོན་ལུ (ཧྲིལ i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.clear();
}

void RTLIL::SigSpec::append(const RTLIL::SigSpec &sa_nkeeta)
{
	གལ་སྲིད (sa_nkeeta.size() == 0)
		ལོག;

	གལ་སྲིད (size() == 0) {
		*this = sa_nkeeta;
		ལོག;
	}

	hash_.clear();
	གལ་སྲིད (rep_ == CHUNK && sa_nkeeta.rep_ == CHUNK && chunk_.wire == sa_nkeeta.chunk_.wire) {
		གལ་སྲིད (chunk_.wire == NULL) {
			chunk_.data.insert(chunk_.data.end(), sa_nkeeta.chunk_.data.begin(), sa_nkeeta.chunk_.data.end());
			chunk_.width = GetSize(chunk_.data);
			ལོག;
		}
		གལ་སྲིད (chunk_.offset + chunk_.width == sa_nkeeta.chunk_.offset) {
			chunk_.width += sa_nkeeta.chunk_.width;
			ལོག;
		}
	}

	unpack();
	དོན་ལུ (const SigBit &bit : sa_nkeeta.bits())
		bits_.push_back(bit);
	check();
}

void RTLIL::SigSpec::append(const RTLIL::SigBit &bit)
{
	hash_.clear();

	གལ་སྲིད (size() == 0) {
		destroy();
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(bit);
		ལོག;
	}

	གལ་སྲིད (rep_ == CHUNK && chunk_.wire == bit.wire) {
		གལ་སྲིད (chunk_.wire == NULL) {
			chunk_.data.push_back(bit.data);
			chunk_.width++;
			ལོག;
		}
		གལ་སྲིད (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			ལོག;
		}
	}

	unpack();

	bits_.push_back(bit);
	check();
}

void RTLIL::SigSpec::extend_u0(ཧྲིལ width, bool is_signed)
{
	གལ་སྲིད (size() > width)
		remove(width, size() - width);

	གལ་སྲིད (size() < width) {
		RTLIL::SigBit padding = size() > 0 ? (*this)[size() - 1] : RTLIL::State::Sx;
		གལ་སྲིད (!is_signed)
			padding = RTLIL::State::S0;
		སྐབས (size() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(ཧྲིལ num) const
{
	RTLIL::SigSpec sig;
	དོན་ལུ (ཧྲིལ i = 0; i < num; i++)
		sig.append(*this);
	ལོག sig;
}

#ifndef NDEBUG
void RTLIL::SigSpec::check(const Module *mod) const
{
	གལ་སྲིད (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		གལ་སྲིད (chunk_.wire == NULL) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.data.size() == (size_t)chunk_.width);
		} དེ་མིན {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.data.size() == 0);
			གལ་སྲིད (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	དེ་མིན གལ་སྲིད (size() <= 64)
	{
		གལ་སྲིད (mod != nullptr) {
			དོན་ལུ (const RTLIL::SigBit &bit : bits_)
				གལ་སྲིད (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#endif

bool RTLIL::SigSpec::operator <(const RTLIL::SigSpec &other) const
{
	གལ་སྲིད (this == &other)
		ལོག false;

	གལ་སྲིད (size() != other.size())
		ལོག size() < other.size();

	auto other_it = other.chunks().begin();
	དོན་ལུ (const SigChunk &c : chunks()) {
		གལ་སྲིད (c != *other_it)
			ལོག c < *other_it;
		++other_it;
	}

	ལོག false;
}

bool RTLIL::SigSpec::operator ==(const RTLIL::SigSpec &other) const
{
	གལ་སྲིད (this == &other)
		ལོག true;

	གལ་སྲིད (size() != other.size())
		ལོག false;

	auto other_it = other.chunks().begin();
	དོན་ལུ (const SigChunk &c : chunks()) {
		གལ་སྲིད (c != *other_it)
			ལོག false;
		++other_it;
	}

	ལོག true;
}

bool RTLIL::SigSpec::is_wire() const
{
	Chunks cs = chunks();
	auto it = cs.begin();
	གལ་སྲིད (it == cs.end())
		ལོག false;
	const RTLIL::SigChunk &chunk = *it;
	ལོག chunk.wire && chunk.wire->width == size() && ++it == cs.end();
}

bool RTLIL::SigSpec::is_chunk() const
{
	Chunks cs = chunks();
	auto it = cs.begin();
	གལ་སྲིད (it == cs.end())
		ལོག false;
	ལོག ++it == cs.end();
}

bool RTLIL::SigSpec::known_driver() const
{
	དོན་ལུ (auto &chunk : chunks())
		གལ་སྲིད (chunk.is_wire() && !chunk.wire->known_driver())
			ལོག false;
	ལོག true;
}

bool RTLIL::SigSpec::is_fully_const() const
{
	དོན་ལུ (auto &chunk : chunks())
		གལ་སྲིད (chunk.width > 0 && chunk.wire != NULL)
			ལོག false;
	ལོག true;
}

bool RTLIL::SigSpec::is_fully_zero() const
{
	དོན་ལུ (auto &chunk : chunks()) {
		གལ་སྲིད (chunk.width > 0 && chunk.wire != NULL)
			ལོག false;
		དོན་ལུ (RTLIL::State d : chunk.data)
			གལ་སྲིད (d != RTLIL::State::S0)
				ལོག false;
	}
	ལོག true;
}

bool RTLIL::SigSpec::is_fully_ones() const
{
	དོན་ལུ (auto &chunk : chunks()) {
		གལ་སྲིད (chunk.width > 0 && chunk.wire != NULL)
			ལོག false;
		དོན་ལུ (RTLIL::State d : chunk.data)
			གལ་སྲིད (d != RTLIL::State::S1)
				ལོག false;
	}
	ལོག true;
}

bool RTLIL::SigSpec::is_fully_def() const
{
	དོན་ལུ (auto &chunk : chunks()) {
		གལ་སྲིད (chunk.width > 0 && chunk.wire != NULL)
			ལོག false;
		དོན་ལུ (RTLIL::State d : chunk.data)
			གལ་སྲིད (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				ལོག false;
	}
	ལོག true;
}

bool RTLIL::SigSpec::is_fully_undef() const
{
	དོན་ལུ (auto &chunk : chunks()) {
		གལ་སྲིད (chunk.width > 0 && chunk.wire != NULL)
			ལོག false;
		དོན་ལུ (RTLIL::State d : chunk.data)
			གལ་སྲིད (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				ལོག false;
	}
	ལོག true;
}

bool RTLIL::SigSpec::has_const() const
{
	དོན་ལུ (auto &chunk : chunks())
		གལ་སྲིད (chunk.width > 0 && chunk.wire == NULL)
			ལོག true;
	ལོག false;
}

bool RTLIL::SigSpec::has_const(State state) const
{
	དོན་ལུ (auto &chunk : chunks())
		གལ་སྲིད (chunk.width > 0 && chunk.wire == NULL && std::find(chunk.data.begin(), chunk.data.end(), state) != chunk.data.end())
			ལོག true;
	ལོག false;
}


bool RTLIL::SigSpec::has_marked_bits() const
{
	དོན་ལུ (auto &chunk : chunks())
		གལ་སྲིད (chunk.width > 0 && chunk.wire == NULL) {
			དོན་ལུ (RTLIL::State d : chunk.data)
				གལ་སྲིད (d == RTLIL::State::Sm)
					ལོག true;
		}
	ལོག false;
}

bool RTLIL::SigSpec::is_onehot(ཧྲིལ *pos) const
{
	གལ་སྲིད (std::optional<RTLIL::Const> c = try_as_const())
		ལོག c->is_onehot(pos);
	ལོག false;
}

bool RTLIL::SigSpec::as_bool() const
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_value());
	ལོག c->as_bool();
}

ཧྲིལ RTLIL::SigSpec::as_int(bool is_signed) const
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_value());
	ལོག c->as_int(is_signed);
}

bool RTLIL::SigSpec::convertible_to_int(bool is_signed) const
{
	std::optional<RTLIL::Const> c = try_as_const();
	གལ་སྲིད (!c.has_value())
		ལོག false;
	ལོག c->convertible_to_int(is_signed);
}

std::optional<ཧྲིལ> RTLIL::SigSpec::try_as_int(bool is_signed) const
{
	std::optional<RTLIL::Const> c = try_as_const();
	གལ་སྲིད (!c.has_value())
		ལོག std::nullopt;
	ལོག c->try_as_int(is_signed);
}

ཧྲིལ RTLIL::SigSpec::as_int_saturating(bool is_signed) const
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_value());
	ལོག c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() const
{
	std::string str;
	str.reserve(size());
	std::vector<RTLIL::SigChunk> chunks = *this;
	དོན་ལུ (size_t i = chunks.size(); i > 0; i--) {
		const RTLIL::SigChunk &chunk = chunks[i-1];
		གལ་སྲིད (chunk.wire != NULL)
			str.append(chunk.width, '?');
		དེ་མིན
			str += RTLIL::Const(chunk.data).as_string();
	}
	ལོག str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() const
{
	Chunks cs = chunks();
	auto it = cs.begin();
	གལ་སྲིད (it == cs.end())
		ལོག RTLIL::Const();
	SigChunk chunk = *it;
	གལ་སྲིད (chunk.wire != NULL || ++it != cs.end())
		ལོག std::nullopt;
	ལོག RTLIL::Const(std::move(chunk.data));
}

RTLIL::Const RTLIL::SigSpec::as_const() const
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_value());
	ལོག *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() const
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == size());
	ལོག chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() const
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	ལོག chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() const
{
	ལོག RTLIL::SigBit(*this);
}

bool RTLIL::SigSpec::match(const var_na* pattern) const
{
	ཧྲིལ pattern_len = strlen(pattern);
	log_assert(pattern_len == size());

	དོན་ལུ (ཧྲིལ i = 0; i < pattern_len; ++i) {
		var_na ch = pattern[i];
		གལ་སྲིད (ch == ' ')
			continue;
		RTLIL::SigBit bit = (*this)[pattern_len - 1 - i];
		གལ་སྲིད (ch == '*') {
			གལ་སྲིད (bit != State::Sz && bit != State::Sx)
				ལོག false;
			continue;
		}
		གལ་སྲིད (ch == '0') {
			གལ་སྲིད (bit != State::S0)
				ལོག false;
		} དེ་མིན
		གལ་སྲིད (ch == '1') {
			གལ་སྲིད (bit != State::S1)
				ལོག false;
		} དེ་མིན
			log_abort();
	}

	ལོག true;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() const
{
	std::set<RTLIL::SigBit> sigbits;
	དོན་ལུ (auto &c : chunks())
		དོན་ལུ (ཧྲིལ i = 0; i < c.width; i++)
			sigbits.insert(RTLIL::SigBit(c, i));
	ལོག sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() const
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(size());
	དོན་ལུ (auto &c : chunks())
		དོན་ལུ (ཧྲིལ i = 0; i < c.width; i++)
			sigbits.insert(RTLIL::SigBit(c, i));
	ལོག sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() const
{
	std::vector<RTLIL::SigBit> result;
	result.reserve(size());
	དོན་ལུ (SigBit bit : *this)
		result.push_back(bit);
	ལོག result;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(const RTLIL::SigSpec &other) const
{
	ཧྲིལ this_size = size();
	log_assert(this_size == other.size());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	དོན་ལུ (ཧྲིལ i = 0; i < this_size; i++)
		new_map[(*this)[i]] = other[i];

	ལོག new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(const RTLIL::SigSpec &other) const
{
	ཧྲིལ this_size = size();
	log_assert(this_size == other.size());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_size);
	དོན་ལུ (ཧྲིལ i = 0; i < this_size; i++)
		new_map[(*this)[i]] = other[i];

	ལོག new_map;
}

static void sigspec_parse_split(std::vector<std::string> &tokens, const std::string &text, var_na sep)
{
	size_t start = 0, end = 0;
	སྐབས ((end = text.find(sep, start)) != std::string::npos) {
		tokens.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
}

static RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, const std::string &netname)
{
	ལོག module->wire(netname);
}

bool RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	དོན་ལུ (ཧྲིལ tokidx = ཧྲིལ(tokens.size())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		གལ་སྲིད (netname.size() == 0)
			continue;

		གལ་སྲིད (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			auto ast = p.const2ast(netname);
			གལ་སྲིད (ast == nullptr)
				ལོག false;
			sig.append(RTLIL::Const(ast->bits));
			continue;
		}

		གལ་སྲིད (module == NULL)
			ལོག false;

		གལ་སྲིད (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		གལ་སྲིད (sigspec_parse_wire(module, netname) == nullptr) {
			size_t indices_pos = netname.size()-1;
			གལ་སྲིད (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				སྐབས (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				གལ་སྲིད (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					སྐབས (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				གལ་སྲིད (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		གལ་སྲིད (wire == nullptr)
			ལོག false;

		གལ་སྲིད (!indices.empty()) {
			std::vector<std::string> index_tokens;
			sigspec_parse_split(index_tokens, indices.substr(1, indices.size()-2), ':');
			གལ་སྲིད (index_tokens.size() == 1) {
				ཧྲིལ a = atoi(index_tokens.at(0).c_str());
				གལ་སྲིད (a < 0 || a >= wire->width)
					ལོག false;
				sig.append(RTLIL::SigSpec(wire, a));
			} དེ་མིན {
				ཧྲིལ a = atoi(index_tokens.at(0).c_str());
				ཧྲིལ b = atoi(index_tokens.at(1).c_str());
				གལ་སྲིད (a > b) {
					ཧྲིལ tmp = a;
					a = b, b = tmp;
				}
				གལ་སྲིད (a < 0 || a >= wire->width)
					ལོག false;
				གལ་སྲིད (b < 0 || b >= wire->width)
					ལོག false;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} དེ་མིན
			sig.append(wire);
	}

	ལོག true;
}

bool RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	གལ་སྲིད (str.empty() || str[0] != '@')
		ལོག parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	གལ་སྲིད (sel_name == IdString::Null || design->selection_vars.count(sel_name) == 0)
		ལོག false;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	དོན་ལུ (auto &it : module->wires_)
		གལ་སྲིད (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	ལོག true;
}

bool RTLIL::SigSpec::parse_rhs(const RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	གལ་སྲིད (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.size());
		ལོག true;
	}

	གལ་སྲིད (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.size());
		ལོག true;
	}

	གལ་སྲིད (lhs.is_chunk()) {
		var_na *p = (var_na*)str.c_str(), *endptr;
		long ཧྲིལ val = strtol(p, &endptr, 10);
		གལ་སྲིད (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.size());
			ལོག true;
		}
	}

	གལ་སྲིད (!parse(sig, module, str))
		ལོག false;
	གལ་སྲིད (sig.size() > lhs.size())
		sig.remove(lhs.size(), sig.size() - lhs.size());
	ལོག true;
}

RTLIL::SigSpec::operator std::vector<RTLIL::SigChunk>() const
{
	std::vector<RTLIL::SigChunk> result;
	དོན་ལུ (const RTLIL::SigChunk &c : chunks())
		result.push_back(c);
	ལོག result;
}

RTLIL::CaseRule::~CaseRule()
{
	དོན་ལུ (auto it = switches.begin(); it != switches.end(); it++)
		delete *it;
}

bool RTLIL::CaseRule::empty() const
{
	ལོག actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() const
{
	RTLIL::CaseRule *new_caserule = new RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	དོན་ལུ (auto &it : switches)
		new_caserule->switches.push_back(it->clone());
	ལོག new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	དོན་ལུ (auto it = cases.begin(); it != cases.end(); it++)
		delete *it;
}

bool RTLIL::SwitchRule::empty() const
{
	ལོག cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() const
{
	RTLIL::SwitchRule *new_switchrule = new RTLIL::SwitchRule;
	new_switchrule->sa_nkeeta = sa_nkeeta;
	new_switchrule->attributes = attributes;
	དོན་ལུ (auto &it : cases)
		new_switchrule->cases.push_back(it->clone());
	ལོག new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() const
{
	RTLIL::SyncRule *new_syncrule = new RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->sa_nkeeta = sa_nkeeta;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	ལོག new_syncrule;
}

RTLIL::Process::~Process()
{
	དོན་ལུ (auto it = syncs.begin(); it != syncs.end(); it++)
		delete *it;
}

RTLIL::Process *RTLIL::Process::clone() const
{
	RTLIL::Process *new_proc = new RTLIL::Process;

	new_proc->attributes = attributes;

	RTLIL::CaseRule *rc_ptr = root_case.clone();
	new_proc->root_case = *rc_ptr;
	rc_ptr->switches.clear();
	delete rc_ptr;

	དོན་ལུ (auto &it : syncs)
		new_proc->syncs.push_back(it->clone());

	ལོག new_proc;
}

RTLIL::Memory::~Memory()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
static std::map<unsigned ཧྲིལ, RTLIL::Memory*> all_memorys;
std::map<unsigned ཧྲིལ, RTLIL::Memory*> *RTLIL::Memory::get_all_memorys(void)
{
	ལོག &all_memorys;
}
#endif
YOSYS_NAMESPACE_END
