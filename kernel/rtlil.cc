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
		ꦏꦁꦒꦺꦴ (size_t i = 0; i < StaticCellTypes::builder.count; i++) {
			auto &cell = StaticCellTypes::builder.cells[i];
			ꦪꦼꦤ꧀ (cell.features.is_ff)
				r.insert(cell.type);
		}
		ꦮꦭꦶ r;
	}();
	ꦮꦭꦶ res;
}

#define check(condition) log_assert(condition && "malformed Const union")

const Const::bitvectype& Const::get_bits() const {
	check(is_bits());
	ꦮꦭꦶ *get_if_bits();
}

const std::string& Const::get_str() const {
	check(is_str());
	ꦮꦭꦶ *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	ꦮꦭꦶ *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	ꦮꦭꦶ *get_if_str();
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

RTLIL::Const::Const(long long val, puur_naa_nka width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	ꦪꦼꦤ꧀ ((width & 7) == 0) {
		new ((void*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		puur_naa_nka bytes = width >> 3;
		signed var_na sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<puur_naa_nka>(bytes, parimaa_na(val));
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < bytes; i++) {
			str[str.size() - 1 - i] = val;
			val = val >> 8;
		}
		ꦮꦭꦶ;
	}

	new ((void*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < width; i++) {
		bv.push_back((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, puur_naa_nka width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	new ((void*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < width; i++)
		bv.push_back(bit);
}

RTLIL::Const::Const(const std::vector<bool> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	new ((void*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.size());
	ꦏꦁꦒꦺꦴ (const auto &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(const RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	ꦪꦼꦤ꧀ (is_str())
		new ((void*)&str_) std::string(other.get_str());
	ꦭꦶꦪ ꦪꦼꦤ꧀ (is_bits())
		new ((void*)&bits_) bitvectype(other.get_bits());
	ꦭꦶꦪ
		check(false);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	ꦪꦼꦤ꧀ (is_str())
		new ((void*)&str_) std::string(std::move(other.get_str()));
	ꦭꦶꦪ ꦪꦼꦤ꧀ (is_bits())
		new ((void*)&bits_) bitvectype(std::move(other.get_bits()));
	ꦭꦶꦪ
		check(false);
}

RTLIL::Const &RTLIL::Const::operator =(const RTLIL::Const &other) {
	flags = other.flags;
	ꦪꦼꦤ꧀ (other.is_str()) {
		ꦪꦼꦤ꧀ (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(void)new ((void*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} ꦭꦶꦪ ꦪꦼꦤ꧀ (other.is_bits()) {
		ꦪꦼꦤ꧀ (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(void)new ((void*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} ꦭꦶꦪ {
		check(false);
	}
	ꦮꦭꦶ *this;
}

RTLIL::Const::~Const() {
	ꦪꦼꦤ꧀ (is_bits())
		bits_.~bitvectype();
	ꦭꦶꦪ ꦪꦼꦤ꧀ (is_str())
		str_.~string();
	ꦭꦶꦪ
		check(false);
}

bool RTLIL::Const::operator<(const RTLIL::Const &other) const
{
	ꦪꦼꦤ꧀ (size() != other.size())
		ꦮꦭꦶ size() < other.size();

	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < size(); i++)
		ꦪꦼꦤ꧀ ((*this)[i] != other[i])
			ꦮꦭꦶ (*this)[i] < other[i];

	ꦮꦭꦶ false;
}

bool RTLIL::Const::operator ==(const RTLIL::Const &other) const
{
	ꦪꦼꦤ꧀ (is_str() && other.is_str())
		ꦮꦭꦶ get_str() == other.get_str();
	ꦪꦼꦤ꧀ (is_bits() && other.is_bits())
		ꦮꦭꦶ get_bits() == other.get_bits();

	ꦪꦼꦤ꧀ (size() != other.size())
		ꦮꦭꦶ false;

	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < size(); i++)
	ꦪꦼꦤ꧀ ((*this)[i] != other[i])
		ꦮꦭꦶ false;

	ꦮꦭꦶ true;
}

bool RTLIL::Const::operator !=(const RTLIL::Const &other) const
{
	ꦮꦭꦶ !(*this == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	ꦮꦭꦶ get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() const
{
	std::vector<State> v;
	ꦏꦁꦒꦺꦴ (auto bit : *this)
		v.push_back(bit);
	ꦮꦭꦶ v;
}

bool RTLIL::Const::as_bool() const
{
	ꦪꦼꦤ꧀ (is_str()) {
		ꦏꦁꦒꦺꦴ (var_na ch : get_str())
			ꦪꦼꦤ꧀ (ch != 0)
				ꦮꦭꦶ true;
		ꦮꦭꦶ false;
	}

	const bitvectype& bv = get_bits();
	ꦏꦁꦒꦺꦴ (size_t i = 0; i < bv.size(); i++)
		ꦪꦼꦤ꧀ (bv[i] == State::S1)
			ꦮꦭꦶ true;
	ꦮꦭꦶ false;
}

puur_naa_nka RTLIL::Const::as_int(bool is_signed) const
{
	int32_t ret = 0;
	ꦪꦼꦤ꧀ (const std::string *s = get_if_str()) {
		puur_naa_nka size = GetSize(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = std::min(4, size); i > 0; i--)
			ret |= static_cast<unsigned var_na>((*s)[size - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		ꦪꦼꦤ꧀ (is_signed && size > 0 && size < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << size*8;
		ꦮꦭꦶ ret;
	}

	const bitvectype& bv = get_bits();
	puur_naa_nka significant_bits = std::min(GetSize(bv), 32);
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < significant_bits; i++)
		ꦪꦼꦤ꧀ (bv[i] == State::S1)
			ret |= 1 << i;
	ꦪꦼꦤ꧀ (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	ꦮꦭꦶ ret;
}

bool RTLIL::Const::convertible_to_int(bool is_signed) const
{
	auto size = get_min_size(is_signed);

	ꦪꦼꦤ꧀ (size < 0)
		ꦮꦭꦶ false;

	// If it fits in 31 bits it is definitely convertible
	ꦪꦼꦤ꧀ (size <= 31)
		ꦮꦭꦶ true;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	ꦪꦼꦤ꧀ (size == 32) {
		ꦪꦼꦤ꧀ (is_signed)
			ꦮꦭꦶ true;
		ꦮꦭꦶ (*this)[size - 1] != State::S1;
	}

	ꦮꦭꦶ false;
}

std::optional<puur_naa_nka> RTLIL::Const::try_as_int(bool is_signed) const
{
	ꦪꦼꦤ꧀ (!convertible_to_int(is_signed))
		ꦮꦭꦶ std::nullopt;
	ꦮꦭꦶ as_int(is_signed);
}

puur_naa_nka RTLIL::Const::as_int_saturating(bool is_signed) const
{
	ꦪꦼꦤ꧀ (!convertible_to_int(is_signed)) {
		ꦪꦼꦤ꧀ (!is_signed)
			ꦮꦭꦶ std::numeric_limits<puur_naa_nka>::max();

		const auto min_size = get_min_size(is_signed);
		log_assert(min_size > 0);
		const auto neg = (*this)[min_size - 1];
		ꦮꦭꦶ neg ? std::numeric_limits<puur_naa_nka>::min() : std::numeric_limits<puur_naa_nka>::max();
	}
	ꦮꦭꦶ as_int(is_signed);
}

void RTLIL::Const::tag_bare_integer_const(const std::string &value)
{
	ꦪꦼꦤ꧀ (value.empty() || value.find('\'') != std::string::npos)
		ꦮꦭꦶ;
	size_t start = (value[0] == '-' || value[0] == '+') ? 1 : 0;
	ꦪꦼꦤ꧀ (start < value.size() && std::all_of(value.begin() + start, value.end(), ::isdigit))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

puur_naa_nka RTLIL::Const::get_min_size(bool is_signed) const
{
	ꦪꦼꦤ꧀ (empty()) ꦮꦭꦶ 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	ꦪꦼꦤ꧀ (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	ꦭꦶꦪ
		leading_bit = RTLIL::State::S0;

	auto idx = size();
	ꦱꦼꦢꦼꦭ (idx > 0 && (*this)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	ꦪꦼꦤ꧀ (is_signed && idx < size()) {
		idx++;
	}
	// must be at least one bit
	ꦮꦭꦶ (idx == 0) ? 1 : idx;
}

void RTLIL::Const::compress(bool is_signed)
{
	auto idx = get_min_size(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<puur_naa_nka> RTLIL::Const::as_int_compress(bool is_signed) const
{
	ꦮꦭꦶ try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(const var_na* any) const
{
	puur_naa_nka sz = size();
	std::string ret;
	ret.reserve(sz);
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = sz - 1; i >= 0; --i)
		switch ((*this)[i]) {
			pak_sa S0: ret.push_back('0'); bha_nga;
			pak_sa S1: ret.push_back('1'); bha_nga;
			pak_sa Sx: ret.push_back('x'); bha_nga;
			pak_sa Sz: ret.push_back('z'); bha_nga;
			pak_sa Sa: ret += any; bha_nga;
			pak_sa Sm: ret.push_back('m'); bha_nga;
		}
	ꦮꦭꦶ ret;
}

RTLIL::Const RTLIL::Const::from_string(const std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.size());
	ꦏꦁꦒꦺꦴ (auto it = str.rbegin(); it != str.rend(); it++)
		switch (*it) {
			pak_sa '0': bv.push_back(State::S0); bha_nga;
			pak_sa '1': bv.push_back(State::S1); bha_nga;
			pak_sa 'x': bv.push_back(State::Sx); bha_nga;
			pak_sa 'z': bv.push_back(State::Sz); bha_nga;
			pak_sa 'm': bv.push_back(State::Sm); bha_nga;
			default: bv.push_back(State::Sa);
		}
	ꦮꦭꦶ c;
}

std::string RTLIL::Const::decode_string() const
{
	ꦪꦼꦤ꧀ (auto str = get_if_str())
		ꦮꦭꦶ *str;

	const bitvectype& bv = get_bits();
	const puur_naa_nka n = GetSize(bv);
	const puur_naa_nka n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	puur_naa_nka i = n_over_8 * 8;
	ꦪꦼꦤ꧀ (i < n) {
		var_na ch = 0;
		ꦏꦁꦒꦺꦴ (puur_naa_nka j = 0; j < (n - i); j++) {
			ꦪꦼꦤ꧀ (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		ꦪꦼꦤ꧀ (ch != 0)
			s.append({ch});
	}
	i -= 8;
	ꦏꦁꦒꦺꦴ (; i >= 0; i -= 8) {
		var_na ch = 0;
		ꦏꦁꦒꦺꦴ (puur_naa_nka j = 0; j < 8; j++) {
			ꦪꦼꦤ꧀ (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		ꦪꦼꦤ꧀ (ch != 0)
			s.append({ch});
	}
	ꦮꦭꦶ s;
}

puur_naa_nka RTLIL::Const::size() const {
	ꦪꦼꦤ꧀ (is_str())
		ꦮꦭꦶ 8 * str_.size();
	ꦭꦶꦪ {
		check(is_bits());
		ꦮꦭꦶ bits_.size();
	}
}

bool RTLIL::Const::empty() const {
	ꦪꦼꦤ꧀ (is_str())
		ꦮꦭꦶ str_.empty();
	ꦭꦶꦪ {
		check(is_bits());
		ꦮꦭꦶ bits_.empty();
	}
}

void RTLIL::Const::bitvectorize_internal() {
	ꦪꦼꦤ꧀ (tag == backing_tag::bits)
		ꦮꦭꦶ;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.size() * 8);
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = str_.size() - 1; i >= 0; i--) {
		unsigned var_na ch = str_[i];
		ꦏꦁꦒꦺꦴ (puur_naa_nka j = 0; j < 8; j++) {
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
	ꦪꦼꦤ꧀ (auto bv = parent->get_if_bits())
		ꦮꦭꦶ (*bv)[idx];

	puur_naa_nka char_idx = parent->get_str().size() - idx / 8 - 1;
	bool bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	ꦮꦭꦶ bit ? State::S1 : State::S0;
}

bool RTLIL::Const::is_fully_zero() const
{
	ꦪꦼꦤ꧀ (auto str = get_if_str()) {
		ꦏꦁꦒꦺꦴ (var_na ch : *str)
			ꦪꦼꦤ꧀ (ch != 0)
				ꦮꦭꦶ false;
		ꦮꦭꦶ true;
	}

	const bitvectype& bv = get_bits();

	ꦏꦁꦒꦺꦴ (const auto &bit : bv)
		ꦪꦼꦤ꧀ (bit != RTLIL::State::S0)
			ꦮꦭꦶ false;

	ꦮꦭꦶ true;
}

bool RTLIL::Const::is_fully_ones() const
{
	ꦪꦼꦤ꧀ (auto str = get_if_str()) {
		ꦏꦁꦒꦺꦴ (var_na ch : *str)
			ꦪꦼꦤ꧀ (ch != (var_na)0xff)
				ꦮꦭꦶ false;
		ꦮꦭꦶ true;
	}

	const bitvectype& bv = get_bits();
	ꦏꦁꦒꦺꦴ (const auto &bit : bv)
		ꦪꦼꦤ꧀ (bit != RTLIL::State::S1)
			ꦮꦭꦶ false;

	ꦮꦭꦶ true;
}

bool RTLIL::Const::is_fully_def() const
{
	ꦪꦼꦤ꧀ (is_str())
		ꦮꦭꦶ true;

	const bitvectype& bv = get_bits();
	ꦏꦁꦒꦺꦴ (const auto &bit : bv)
		ꦪꦼꦤ꧀ (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			ꦮꦭꦶ false;

	ꦮꦭꦶ true;
}

bool RTLIL::Const::is_fully_undef() const
{
	ꦪꦼꦤ꧀ (auto str = get_if_str())
		ꦮꦭꦶ str->empty();

	const bitvectype& bv = get_bits();
	ꦏꦁꦒꦺꦴ (const auto &bit : bv)
		ꦪꦼꦤ꧀ (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			ꦮꦭꦶ false;

	ꦮꦭꦶ true;
}

bool RTLIL::Const::is_fully_undef_x_only() const
{
	ꦪꦼꦤ꧀ (auto str = get_if_str())
		ꦮꦭꦶ str->empty();

	const bitvectype& bv = get_bits();
	ꦏꦁꦒꦺꦴ (const auto &bit : bv)
		ꦪꦼꦤ꧀ (bit != RTLIL::State::Sx)
			ꦮꦭꦶ false;

	ꦮꦭꦶ true;
}

bool RTLIL::Const::is_onehot(puur_naa_nka *pos) const
{
	bool found = false;
	puur_naa_nka size = GetSize(*this);
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < size; i++) {
		State bit = (*this)[i];
		ꦪꦼꦤ꧀ (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			ꦮꦭꦶ false;
		ꦪꦼꦤ꧀ (bit == RTLIL::State::S1) {
			ꦪꦼꦤ꧀ (found)
				ꦮꦭꦶ false;
			ꦪꦼꦤ꧀ (pos)
				*pos = i;
			found = true;
		}
	}
	ꦮꦭꦶ found;
}

Hasher RTLIL::Const::hash_into(Hasher h) const
{
	ꦪꦼꦤ꧀ (auto str = get_if_str())
		ꦮꦭꦶ hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	const bitvectype &bv = get_bits();
	puur_naa_nka size = GetSize(bv);
	std::string packed;
	puur_naa_nka packed_size = (size + 7) >> 3;
	packed.resize(packed_size, 0);
	ꦏꦁꦒꦺꦴ (puur_naa_nka bi = 0; bi < packed_size; ++bi) {
		var_na ch = 0;
		puur_naa_nka end = std::min((bi + 1)*8, size);
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			ꦪꦼꦤ꧀ (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<var_na>::hash_into(ch, h);
				ꦏꦁꦒꦺꦴ (; i < size; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(size);
				ꦮꦭꦶ h;
			}
			ch |= static_cast<puur_naa_nka>(b) << (i & 7);
		}
		packed[packed_size - 1 - bi] = ch;
	}
	ꦮꦭꦶ hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(puur_naa_nka offset, puur_naa_nka len, RTLIL::State padding) const {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = offset; i < offset + len; i++)
		ret_bv.push_back(i < GetSize(*this) ? (*this)[i] : padding);
	ꦮꦭꦶ RTLIL::Const(ret_bv);
}
#undef check /* check(condition) for Const */

bool RTLIL::AttrObject::has_attribute(RTLIL::IdString id) const
{
	ꦮꦭꦶ attributes.count(id);
}

void RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, bool value)
{
	ꦪꦼꦤ꧀ (value)
		attributes[id] = RTLIL::Const(1);
	ꦭꦶꦪ
		attributes.erase(id);
}

bool RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) const
{
	const auto it = attributes.find(id);
	ꦪꦼꦤ꧀ (it == attributes.end())
		ꦮꦭꦶ false;
	ꦮꦭꦶ it->second.as_bool();
}

void RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string value)
{
	ꦪꦼꦤ꧀ (value.empty())
		attributes.erase(id);
	ꦭꦶꦪ
		attributes[id] = value;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) const
{
	std::string value;
	const auto it = attributes.find(id);
	ꦪꦼꦤ꧀ (it != attributes.end())
		value = it->second.decode_string();
	ꦮꦭꦶ value;
}

void RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, const pool<string> &data)
{
	string attrval;
	ꦏꦁꦒꦺꦴ (const auto &s : data) {
		ꦪꦼꦤ꧀ (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

void RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, const pool<string> &data)
{
	pool<string> union_data = get_strpool_attribute(id);
	union_data.insert(data.begin(), data.end());
	ꦪꦼꦤ꧀ (!union_data.empty())
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
		ꦱꦼꦢꦼꦭ (!case_stack.empty()) {
			RTLIL::CaseRule *cs = case_stack.back();
			case_stack.pop_back();
			visit(cs);
			ꦏꦁꦒꦺꦴ (auto *sw : cs->switches) {
				visit(sw);
				ꦏꦁꦒꦺꦴ (auto *case_ : sw->cases)
					case_stack.push_back(case_);
			}
		}
		ꦏꦁꦒꦺꦴ (auto *sync : process->syncs)
			ꦏꦁꦒꦺꦴ (auto &mwa : sync->mem_write_actions)
				visit(&mwa);
	}

	template<typename F>
	void walk_attr_objects(RTLIL::Design *design, F visit) {
		ꦏꦁꦒꦺꦴ (auto &[_, module] : design->modules_) {
			visit(module);
			ꦏꦁꦒꦺꦴ (auto &[_, wire] : module->wires_)
				visit(wire);
			ꦏꦁꦒꦺꦴ (auto &[_, mem] : module->memories)
				visit(mem);
			ꦏꦁꦒꦺꦴ (auto &[_, cell] : module->cells_)
				visit(cell);
			ꦏꦁꦒꦺꦴ (auto &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


size_t RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	auto root = [&](IdString ref) {
		ꦪꦼꦤ꧀ (ref != IdString::Null)
			live.insert(ref);
	};

	walk_attr_objects(this, [&](const RTLIL::AttrObject *obj) {
		ꦏꦁꦒꦺꦴ (auto &attr : obj->attributes)
			root(attr.first);
	});

	root(selected_active_module);

	ꦏꦁꦒꦺꦴ (auto &[name, module] : modules_) {
		root(name);
		ꦏꦁꦒꦺꦴ (IdString port : module->ports)
			root(port);
		ꦏꦁꦒꦺꦴ (IdString param : module->avail_parameters)
			root(param);
		ꦏꦁꦒꦺꦴ (auto &[param, _] : module->parameter_default_values)
			root(param);
		ꦏꦁꦒꦺꦴ (auto &[name, _] : module->memories)
			root(name);
		ꦏꦁꦒꦺꦴ (auto &[name, _] : module->processes)
			root(name);
		ꦏꦁꦒꦺꦴ (auto &[name, wire] : module->wires_) {
			root(name);
			ꦪꦼꦤ꧀ (wire->known_driver())
				root(wire->driverPort());
		}
		ꦏꦁꦒꦺꦴ (auto &[name, cell] : module->cells_) {
			root(name);
			root(cell->type);
			ꦏꦁꦒꦺꦴ (auto &conn : cell->connections())
				root(conn.first);
			ꦏꦁꦒꦺꦴ (auto &param : cell->parameters)
				root(param.first);
		}
		ꦏꦁꦒꦺꦴ (auto &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_stack{&process->root_case};
			ꦱꦼꦢꦼꦭ (!case_stack.empty()) {
				RTLIL::CaseRule *cs = case_stack.back();
				case_stack.pop_back();
				ꦏꦁꦒꦺꦴ (auto *sw : cs->switches) {
					ꦏꦁꦒꦺꦴ (auto *case_ : sw->cases)
						case_stack.push_back(case_);
				}
			}
			ꦏꦁꦒꦺꦴ (auto *sync : process->syncs) {
				ꦏꦁꦒꦺꦴ (auto &mwa : sync->mem_write_actions)
					root(mwa.memid);
			}
		}
	}

	auto root_selection = [&](const RTLIL::Selection &sel) {
		ꦏꦁꦒꦺꦴ (IdString m : sel.selected_modules)
			root(m);
		ꦏꦁꦒꦺꦴ (auto &[m, members] : sel.selected_members) {
			root(m);
			ꦏꦁꦒꦺꦴ (IdString member : members)
				root(member);
		}
	};

	ꦏꦁꦒꦺꦴ (auto &[name, sel] : selection_vars) {
		root(name);
		root_selection(sel);
	}

	ꦏꦁꦒꦺꦴ (auto &sel : selection_stack)
		root_selection(sel);

	size_t erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_count;
	ꦮꦭꦶ erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) const
{
	pool<string> data;
	ꦪꦼꦤ꧀ (attributes.count(id) != 0)
		ꦏꦁꦒꦺꦴ (auto s : split_tokens(get_string_attribute(id), "|"))
			data.insert(s);
	ꦮꦭꦶ data;
}

void RTLIL::AttrObject::set_hdlname_attribute(const vector<string> &hierarchy)
{
	string attrval;
	ꦏꦁꦒꦺꦴ (const auto &ident : hierarchy) {
		ꦪꦼꦤ꧀ (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() const
{
	ꦮꦭꦶ split_tokens(get_string_attribute(ID::hdlname), " ");
}

void RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, const vector<puur_naa_nka> &data)
{
	std::stringstream attrval;
	ꦏꦁꦒꦺꦴ (auto &i : data) {
		ꦪꦼꦤ꧀ (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<puur_naa_nka> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) const
{
	vector<puur_naa_nka> data;
	auto it = attributes.find(id);
	ꦪꦼꦤ꧀ (it != attributes.end())
		ꦏꦁꦒꦺꦴ (const auto &s : split_tokens(attributes.at(id).decode_string())) {
			var_na *end = nullptr;
			errno = 0;
			long value = strtol(s.c_str(), &end, 10);
			ꦪꦼꦤ꧀ (end != s.c_str() + s.size())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			ꦪꦼꦤ꧀ (errno == ERANGE || value < INT_MIN || value > INT_MAX)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			data.push_back(value);
		}
	ꦮꦭꦶ data;
}

bool RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) const
{
	ꦪꦼꦤ꧀ (current_design != nullptr) {
		auto module = current_design->module(mod_name);
		ꦮꦭꦶ module && module->get_blackbox_attribute();
	} ꦭꦶꦪ {
		log_warning("Unable to check if module is boxed for null design.\n");
		ꦮꦭꦶ false;
	}
}

bool RTLIL::Selection::selected_module(RTLIL::IdString mod_name) const
{
	ꦪꦼꦤ꧀ (complete_selection)
		ꦮꦭꦶ true;
	ꦪꦼꦤ꧀ (!selects_boxes && boxed_module(mod_name))
		ꦮꦭꦶ false;
	ꦪꦼꦤ꧀ (full_selection)
		ꦮꦭꦶ true;
	ꦪꦼꦤ꧀ (selected_modules.count(mod_name) > 0)
		ꦮꦭꦶ true;
	ꦪꦼꦤ꧀ (selected_members.count(mod_name) > 0)
		ꦮꦭꦶ true;
	ꦮꦭꦶ false;
}

bool RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) const
{
	ꦪꦼꦤ꧀ (complete_selection)
		ꦮꦭꦶ true;
	ꦪꦼꦤ꧀ (!selects_boxes && boxed_module(mod_name))
		ꦮꦭꦶ false;
	ꦪꦼꦤ꧀ (full_selection)
		ꦮꦭꦶ true;
	ꦪꦼꦤ꧀ (selected_modules.count(mod_name) > 0)
		ꦮꦭꦶ true;
	ꦮꦭꦶ false;
}

bool RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) const
{
	ꦪꦼꦤ꧀ (complete_selection)
		ꦮꦭꦶ true;
	ꦪꦼꦤ꧀ (!selects_boxes && boxed_module(mod_name))
		ꦮꦭꦶ false;
	ꦪꦼꦤ꧀ (full_selection)
		ꦮꦭꦶ true;
	ꦪꦼꦤ꧀ (selected_modules.count(mod_name) > 0)
		ꦮꦭꦶ true;
	ꦪꦼꦤ꧀ (selected_members.count(mod_name) > 0)
		ꦪꦼꦤ꧀ (selected_members.at(mod_name).count(memb_name) > 0)
			ꦮꦭꦶ true;
	ꦮꦭꦶ false;
}

void RTLIL::Selection::optimize(RTLIL::Design *design)
{
	ꦪꦼꦤ꧀ (design != current_design) {
		current_design = design;
	}

	ꦪꦼꦤ꧀ (selects_boxes && full_selection)
		complete_selection = true;
	ꦪꦼꦤ꧀ (complete_selection) {
		full_selection = false;
		selects_boxes = true;
	}
	ꦪꦼꦤ꧀ (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		ꦮꦭꦶ;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.clear();
	ꦏꦁꦒꦺꦴ (auto mod_name : selected_modules) {
		ꦪꦼꦤ꧀ (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.push_back(mod_name);
		selected_members.erase(mod_name);
	}
	ꦏꦁꦒꦺꦴ (auto mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	ꦏꦁꦒꦺꦴ (auto &it : selected_members)
		ꦪꦼꦤ꧀ (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.push_back(it.first);
	ꦏꦁꦒꦺꦴ (auto mod_name : del_list)
		selected_members.erase(mod_name);

	ꦏꦁꦒꦺꦴ (auto &it : selected_members) {
		del_list.clear();
		ꦏꦁꦒꦺꦴ (auto memb_name : it.second)
			ꦪꦼꦤ꧀ (current_design->module(it.first)->count_id(memb_name) == 0)
				del_list.push_back(memb_name);
		ꦏꦁꦒꦺꦴ (auto memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	add_list.clear();
	ꦏꦁꦒꦺꦴ (auto &it : selected_members)
		ꦪꦼꦤ꧀ (it.second.size() == 0)
			del_list.push_back(it.first);
		ꦭꦶꦪ ꦪꦼꦤ꧀ (it.second.size() == current_design->module(it.first)->wires_.size() + current_design->module(it.first)->memories.size() +
				current_design->module(it.first)->cells_.size() + current_design->module(it.first)->processes.size())
			add_list.push_back(it.first);
	ꦏꦁꦒꦺꦴ (auto mod_name : del_list)
		selected_members.erase(mod_name);
	ꦏꦁꦒꦺꦴ (auto mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.insert(mod_name);
	}

	ꦪꦼꦤ꧀ (selected_modules.size() == current_design->modules_.size()) {
		selected_modules.clear();
		selected_members.clear();
		ꦪꦼꦤ꧀ (selects_boxes)
			complete_selection = true;
		ꦭꦶꦪ
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
	static unsigned puur_naa_nka hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	refcount_modules_ = 0;
	selected_active_module.clear();
	push_full_selection();

	RTLIL::Design::get_all_designs()->insert(std::pair<unsigned puur_naa_nka, RTLIL::Design*>(hashidx_, this));
}

RTLIL::Design::~Design()
{
	ꦏꦁꦒꦺꦴ (auto &pr : modules_)
		delete pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

static std::map<unsigned puur_naa_nka, RTLIL::Design*> all_designs;
std::map<unsigned puur_naa_nka, RTLIL::Design*> *RTLIL::Design::get_all_designs(void)
{
	ꦮꦭꦶ &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	ꦮꦭꦶ RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

const RTLIL::Module *RTLIL::Design::module(IdString id) const {
	twines_.check_owned(id);
	ꦮꦭꦶ modules_.count(id) ? modules_.at(id) : NULL;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	ꦮꦭꦶ modules_.count(id) ? modules_.at(id) : NULL;
}

RTLIL::Module *RTLIL::Design::top_module() const
{
	RTLIL::Module *module = nullptr;
	puur_naa_nka module_count = 0;

	ꦏꦁꦒꦺꦴ (auto mod : selected_modules()) {
		ꦪꦼꦤ꧀ (mod->get_bool_attribute(ID::top))
			ꦮꦭꦶ mod;
		module_count++;
		module = mod;
	}

	ꦮꦭꦶ module_count == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	ꦪꦼꦤ꧀ (modules_.count(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = new RTLIL::Module;
	modules_[name] = module;
	module->design = this;
	module->name = name;

	ꦏꦁꦒꦺꦴ (auto mon : monitors)
		mon->notify_module_add(module);

	ꦪꦼꦤ꧀ (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	ꦮꦭꦶ module;
}

void RTLIL::Design::scratchpad_unset(const std::string &varname)
{
	scratchpad.erase(varname);
}

void RTLIL::Design::scratchpad_set_int(const std::string &varname, puur_naa_nka value)
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

puur_naa_nka RTLIL::Design::scratchpad_get_int(const std::string &varname, puur_naa_nka default_value) const
{
	auto it = scratchpad.find(varname);
	ꦪꦼꦤ꧀ (it == scratchpad.end())
		ꦮꦭꦶ default_value;

	const std::string &str = it->second;

	ꦪꦼꦤ꧀ (str == "0" || str == "false")
		ꦮꦭꦶ 0;

	ꦪꦼꦤ꧀ (str == "1" || str == "true")
		ꦮꦭꦶ 1;

	var_na *endptr = nullptr;
	long puur_naa_nka parsed_value = strtol(str.c_str(), &endptr, 10);
	ꦮꦭꦶ *endptr ? default_value : parsed_value;
}

bool RTLIL::Design::scratchpad_get_bool(const std::string &varname, bool default_value) const
{
	auto it = scratchpad.find(varname);
	ꦪꦼꦤ꧀ (it == scratchpad.end())
		ꦮꦭꦶ default_value;

	const std::string &str = it->second;

	ꦪꦼꦤ꧀ (str == "0" || str == "false")
		ꦮꦭꦶ false;

	ꦪꦼꦤ꧀ (str == "1" || str == "true")
		ꦮꦭꦶ true;

	ꦮꦭꦶ default_value;
}

std::string RTLIL::Design::scratchpad_get_string(const std::string &varname, const std::string &default_value) const
{
	auto it = scratchpad.find(varname);
	ꦪꦼꦤ꧀ (it == scratchpad.end())
		ꦮꦭꦶ default_value;

	ꦮꦭꦶ it->second;
}

void RTLIL::Design::remove(RTLIL::Module *module)
{
	ꦏꦁꦒꦺꦴ (auto mon : monitors)
		mon->notify_module_del(module);

	ꦪꦼꦤ꧀ (yosys_xtrace) {
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
	ꦏꦁꦒꦺꦴ (auto &it : modules_)
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
	puur_naa_nka pool_size = 0;
	ꦏꦁꦒꦺꦴ (auto &it : modules_)
		pool_size = std::max(pool_size, ThreadPool::work_pool_size(0, it.second->cells_size(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_size);
	ꦏꦁꦒꦺꦴ (auto &it : modules_) {
		log_assert(this == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#endif
}

void RTLIL::Design::optimize()
{
	ꦏꦁꦒꦺꦴ (auto &it : modules_)
		it.second->optimize();
	ꦏꦁꦒꦺꦴ (auto &it : selection_stack)
		it.optimize(this);
	ꦏꦁꦒꦺꦴ (auto &it : selection_vars)
		it.second.optimize(this);
}

void RTLIL::Design::clone_into(RTLIL::Design *dst) const
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	ꦏꦁꦒꦺꦴ (auto it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

bool RTLIL::Design::selected_module(IdString mod_name) const
{
	ꦪꦼꦤ꧀ (!selected_active_module.empty() && mod_name != selected_active_module)
		ꦮꦭꦶ false;
	ꦮꦭꦶ selection().selected_module(mod_name);
}

bool RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) const
{
	ꦪꦼꦤ꧀ (!selected_active_module.empty() && mod_name != selected_active_module)
		ꦮꦭꦶ false;
	ꦮꦭꦶ selection().selected_whole_module(mod_name);
}

bool RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) const
{
	ꦪꦼꦤ꧀ (!selected_active_module.empty() && mod_name != selected_active_module)
		ꦮꦭꦶ false;
	ꦮꦭꦶ selection().selected_member(mod_name, memb_name);
}

bool RTLIL::Design::selected_module(RTLIL::Module *mod) const
{
	ꦮꦭꦶ selected_module(mod->name);
}

bool RTLIL::Design::selected_whole_module(RTLIL::Module *mod) const
{
	ꦮꦭꦶ selected_whole_module(mod->name);
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
	ꦪꦼꦤ꧀ (selection_stack.empty())
		push_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(bool only_selected) const
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(this), only_selected, true, false, RTLIL_BACKEND::DumpMode::Readable);
	ꦮꦭꦶ f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) const
{
	bool include_partials = partials == RTLIL::SELECT_ALL;
	bool exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	bool ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> result;
	result.reserve(modules_.size());
	ꦏꦁꦒꦺꦴ (auto &it : modules_) {
		ꦪꦼꦤ꧀ (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			ꦪꦼꦤ꧀ (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				result.push_back(it.second);
			ꦭꦶꦪ
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
		} ꦭꦶꦪ ꦪꦼꦤ꧀ (!include_partials && selected_module(it.first)) {
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
	ꦮꦭꦶ result;
}

RTLIL::Module::Module()
{
	static unsigned puur_naa_nka hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->insert(std::pair<unsigned puur_naa_nka, RTLIL::Module*>(hashidx_, this));
#endif
}

RTLIL::Module::~Module()
{
	ꦏꦁꦒꦺꦴ (auto &pr : wires_)
		delete pr.second;
	ꦏꦁꦒꦺꦴ (auto &pr : memories)
		delete pr.second;
	ꦏꦁꦒꦺꦴ (auto &pr : cells_)
		delete pr.second;
	ꦏꦁꦒꦺꦴ (auto &pr : processes)
		delete pr.second;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
static std::map<unsigned puur_naa_nka, RTLIL::Module*> all_modules;
std::map<unsigned puur_naa_nka, RTLIL::Module*> *RTLIL::Module::get_all_modules(void)
{
	ꦮꦭꦶ &all_modules;
}
#endif

void RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	ꦏꦁꦒꦺꦴ (auto it = wires_.begin(); it != wires_.end(); ++it)
		ꦪꦼꦤ꧀ (!it->second->port_input && !it->second->port_output)
			delwires.insert(it->second);

	ꦏꦁꦒꦺꦴ (auto it = memories.begin(); it != memories.end(); ++it)
		delete it->second;
	memories.clear();

	ꦏꦁꦒꦺꦴ (auto it = cells_.begin(); it != cells_.end(); ++it)
		delete it->second;
	cells_.clear();

	ꦏꦁꦒꦺꦴ (auto it = processes.begin(); it != processes.end(); ++it)
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
	ꦮꦭꦶ false;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, const dict<RTLIL::IdString, RTLIL::Const> &, bool mayfail)
{
	ꦪꦼꦤ꧀ (mayfail)
		ꦮꦭꦶ IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, const dict<RTLIL::IdString, RTLIL::Const> &, const dict<RTLIL::IdString, RTLIL::Module*> &, const dict<RTLIL::IdString, RTLIL::IdString> &, bool mayfail)
{
	ꦪꦼꦤ꧀ (mayfail)
		ꦮꦭꦶ IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

size_t RTLIL::Module::count_id(RTLIL::IdString id)
{
	ꦮꦭꦶ wires_.count(id) + cells_.count(id) + memories.count(id) + processes.count(id);
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

		void error(puur_naa_nka linenr, std::optional<std::string> error_msg = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			ꦪꦼꦤ꧀ (error_msg) {
				log_error("%s in internal cell %s\n", *error_msg, cell_loc);
			} ꦭꦶꦪ {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		puur_naa_nka param(RTLIL::IdString name)
		{
			auto it = cell->parameters.find(name);
			ꦪꦼꦤ꧀ (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.insert(name);
			ꦮꦭꦶ it->second.as_int();
		}

		puur_naa_nka param_bool(RTLIL::IdString name)
		{
			puur_naa_nka v = param(name);
			ꦪꦼꦤ꧀ (GetSize(cell->parameters.at(name)) > 32)
				error(__LINE__);
			ꦪꦼꦤ꧀ (v != 0 && v != 1)
				error(__LINE__);
			ꦮꦭꦶ v;
		}

		puur_naa_nka param_bool(RTLIL::IdString name, bool expected)
		{
			puur_naa_nka v = param_bool(name);
			ꦪꦼꦤ꧀ (v != expected)
				error(__LINE__);
			ꦮꦭꦶ v;
		}

		void param_bits(RTLIL::IdString name, puur_naa_nka width)
		{
			param(name);
			ꦪꦼꦤ꧀ (GetSize(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			ꦮꦭꦶ cell->parameters.at(name).decode_string();
		}

		void port(RTLIL::IdString name, puur_naa_nka width)
		{
			auto it = cell->connections_.find(name);
			ꦪꦼꦤ꧀ (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			ꦪꦼꦤ꧀ (GetSize(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.insert(name);
		}

		void check_expected(bool check_matched_sign = false)
		{
			ꦏꦁꦒꦺꦴ (auto &para : cell->parameters)
				ꦪꦼꦤ꧀ (expected_params.count(para.first) == 0)
					error(__LINE__);
			ꦏꦁꦒꦺꦴ (auto &conn : cell->connections())
				ꦪꦼꦤ꧀ (expected_ports.count(conn.first) == 0)
					error(__LINE__);

			ꦪꦼꦤ꧀ (check_matched_sign) {
				log_assert(expected_params.count(ID::A_SIGNED) != 0 && expected_params.count(ID::B_SIGNED) != 0);
				bool a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				bool b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				ꦪꦼꦤ꧀ (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		void check()
		{
			ꦪꦼꦤ꧀ (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				ꦮꦭꦶ;

			ꦪꦼꦤ꧀ (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(true);
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/false);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/false);
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type.in(ID($shift), ID($shiftx))) {
				ꦪꦼꦤ꧀ (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/false);
				} ꦭꦶꦪ {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/false);
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(true);
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($pow));
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($alu)) {
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
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($macc_v2)) {
				ꦪꦼꦤ꧀ (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				ꦪꦼꦤ꧀ (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				ꦪꦼꦤ꧀ (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				const Const &a_width = cell->getParam(ID::A_WIDTHS);
				const Const &b_width = cell->getParam(ID::B_WIDTHS);
				const Const &c_width = cell->getParam(ID::C_WIDTHS);
				puur_naa_nka a_width_sum = 0, b_width_sum = 0, c_width_sum = 0;
				ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_sum += a_width.extract(16 * i, 16).as_int(false);
					b_width_sum += b_width.extract(16 * i, 16).as_int(false);
				}
				ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < param(ID::NADDENDS); i++) {
					c_width_sum += c_width.extract(16 * i, 16).as_int(false);
				}
				port(ID::A, a_width_sum);
				port(ID::B, b_width_sum);
				port(ID::C, c_width_sum);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/false);
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				ꦪꦼꦤ꧀ (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($dffsre)) {
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
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_VALUE, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_VALUE, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type.in(ID($sdffe), ID($sdffce))) {
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
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($adffe)) {
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
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($aldffe)) {
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
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_VALUE, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($fsm)) {
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
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($memrd_v2)) {
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
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($memwr_v2)) {
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
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::WORDS));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::WORDS));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($mem)) {
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
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($mem_v2)) {
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
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type.in(ID($assert), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type.in(ID($specify2), ID($specify3))) {
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
				ꦪꦼꦤ꧀ (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
				}
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($specrule)) {
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
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				ꦪꦼꦤ꧀ (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
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
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				ꦪꦼꦤ꧀ (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				ꦮꦭꦶ;
			}

			ꦪꦼꦤ꧀ (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }
			ꦪꦼꦤ꧀ (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); ꦮꦭꦶ; }

			ꦪꦼꦤ꧀ (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}
			ꦪꦼꦤ꧀ (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}
			ꦪꦼꦤ꧀ (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}
			ꦪꦼꦤ꧀ (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}
			ꦪꦼꦤ꧀ (cell->type.in(ID($input_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
			}
			ꦪꦼꦤ꧀ (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				check_expected();
				ꦮꦭꦶ;
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
	ꦏꦁꦒꦺꦴ (auto &it : cells_)
		it.second->sort();
	ꦏꦁꦒꦺꦴ (auto &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	ꦏꦁꦒꦺꦴ (auto &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#ifndef NDEBUG
static void check_id_keys(const dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	ꦏꦁꦒꦺꦴ (auto &it : keyed)
		log_assert(!it.first.empty());
}
#endif

void check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ifndef NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->cells_size(), 1000));
	const RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	ꦏꦁꦒꦺꦴ (auto &it : module->memories) {
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
		ꦏꦁꦒꦺꦴ (puur_naa_nka i : ctx.item_range(const_module->cells_size())) {
			auto it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			ꦏꦁꦒꦺꦴ (auto &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_keys(it.second->attributes);
			check_id_keys(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			ꦪꦼꦤ꧀ (it.second->has_memid()) {
				log_assert(memory_strings.count(it.second->parameters.at(ID::MEMID).decode_string()));
			} ꦭꦶꦪ ꦪꦼꦤ꧀ (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.count(memid));
				memids.insert(ctx, std::move(memid));
			}
			auto cell_mod = const_module->design->module(it.second->name);
			ꦪꦼꦤ꧀ (cell_mod != nullptr) {
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

		ꦏꦁꦒꦺꦴ (puur_naa_nka i : ctx.item_range(const_module->wires_size())) {
			auto it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_keys(it.second->attributes);
			ꦪꦼꦤ꧀ (it.second->port_id) {
				log_assert(GetSize(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_input || it.second->port_output);
				log_assert(it.second->port_id <= GetSize(ports_declared));
				bool previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == false);
			} ꦭꦶꦪ
				log_assert(!it.second->port_input && !it.second->port_output);
		}
	});
	ꦏꦁꦒꦺꦴ (const MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == true);
	pool<std::string> memids_pool;
	ꦏꦁꦒꦺꦴ (std::string &memid : memids)
		log_assert(memids_pool.insert(memid).second);

	ꦏꦁꦒꦺꦴ (auto &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->root_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->root_case};
		ꦏꦁꦒꦺꦴ (size_t i = 0; i < all_cases.size(); i++) {
			ꦏꦁꦒꦺꦴ (auto &switch_it : all_cases[i]->switches) {
				ꦏꦁꦒꦺꦴ (auto &case_it : switch_it->cases) {
					ꦏꦁꦒꦺꦴ (auto &compare_it : case_it->compare) {
						log_assert(switch_it->sa_nkeeta.size() == compare_it.size());
					}
					all_cases.push_back(case_it);
				}
			}
		}
		ꦏꦁꦒꦺꦴ (auto &sync_it : it.second->syncs) {
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

	ꦏꦁꦒꦺꦴ (auto &it : module->connections_) {
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
	puur_naa_nka pool_size = ThreadPool::work_pool_size(0, cells_size(), 1000);
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
	ꦏꦁꦒꦺꦴ (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_values.clear();
	ꦏꦁꦒꦺꦴ (auto &it : parameter_default_values)
		new_mod->parameter_default_values[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	ꦏꦁꦒꦺꦴ (auto &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	ꦏꦁꦒꦺꦴ (auto it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	ꦏꦁꦒꦺꦴ (auto it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	ꦏꦁꦒꦺꦴ (auto it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	ꦏꦁꦒꦺꦴ (auto it = processes.rbegin(); it != processes.rend(); ++it) {
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
				ꦪꦼꦤ꧀ (it != wire_map.end())
					wire = it->second;
				ꦭꦶꦪ
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
	ꦮꦭꦶ new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) const
{
	RTLIL::Module *new_mod = new RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->add(new_mod);
	ꦮꦭꦶ new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) const
{
	RTLIL::Module *new_mod = new RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	ꦮꦭꦶ new_mod;
}

bool RTLIL::Module::has_memories() const
{
	ꦮꦭꦶ !memories.empty();
}

bool RTLIL::Module::has_processes() const
{
	ꦮꦭꦶ !processes.empty();
}

bool RTLIL::Module::has_memories_warn() const
{
	ꦪꦼꦤ꧀ (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", this);
	ꦮꦭꦶ !memories.empty();
}

bool RTLIL::Module::has_processes_warn() const
{
	ꦪꦼꦤ꧀ (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", this);
	ꦮꦭꦶ !processes.empty();
}

bool RTLIL::Module::is_selected() const
{
	ꦮꦭꦶ design->selected_module(this->name);
}

bool RTLIL::Module::is_selected_whole() const
{
	ꦮꦭꦶ design->selected_whole_module(this->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() const
{
	std::vector<RTLIL::Wire*> result;
	result.reserve(wires_.size());
	ꦏꦁꦒꦺꦴ (auto &it : wires_)
		ꦪꦼꦤ꧀ (design->selected(this, it.second))
			result.push_back(it.second);
	ꦮꦭꦶ result;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() const
{
	std::vector<RTLIL::Cell*> result;
	result.reserve(cells_.size());
	ꦏꦁꦒꦺꦴ (auto &it : cells_)
		ꦪꦼꦤ꧀ (design->selected(this, it.second))
			result.push_back(it.second);
	ꦮꦭꦶ result;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() const
{
	std::vector<RTLIL::Memory*> result;
	result.reserve(memories.size());
	ꦏꦁꦒꦺꦴ (auto &it : memories)
		ꦪꦼꦤ꧀ (design->selected(this, it.second))
			result.push_back(it.second);
	ꦮꦭꦶ result;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() const
{
	std::vector<RTLIL::Process*> result;
	result.reserve(processes.size());
	ꦏꦁꦒꦺꦴ (auto &it : processes)
		ꦪꦼꦤ꧀ (design->selected(this, it.second))
			result.push_back(it.second);
	ꦮꦭꦶ result;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() const
{
	std::vector<RTLIL::SelectedMember> result;
	ꦏꦁꦒꦺꦴ (auto cell : selected_cells())
		result.push_back({cell, cell->name});
	ꦏꦁꦒꦺꦴ (auto memory : selected_memories())
		result.push_back({memory, memory->name});
	ꦏꦁꦒꦺꦴ (auto wire : selected_wires())
		result.push_back({wire, wire->name});
	ꦏꦁꦒꦺꦴ (auto process : selected_processes())
		result.push_back({process, process->name});
	ꦮꦭꦶ result;
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
	ꦪꦼꦤ꧀ (wires.empty())
		ꦮꦭꦶ;

	sa_nracanaa DeleteWireWorker
	{
		RTLIL::Module *module;
		const pool<RTLIL::Wire*> *wires_p;

		void operator()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([this](RTLIL::Wire *&wire) {
				ꦪꦼꦤ꧀ (wires_p->count(wire))
					wire = module->addWire(stringf("$delete_wire$%d", (puur_naa_nka)autoidx++), wire->width);
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

	ꦪꦼꦤ꧀ (design->flagBufferedNormalized) {
		ꦏꦁꦒꦺꦴ (auto wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_index.erase(wire);
		}
	}

	ꦏꦁꦒꦺꦴ (auto &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.count(id) != 0);
		wires_.erase(id);
		delete it;
	}
}

void RTLIL::Module::remove(RTLIL::Cell *cell)
{
	ꦱꦼꦢꦼꦭ (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.count(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	ꦪꦼꦤ꧀ (design && design->flagBufferedNormalized && buf_norm_cell_queue.count(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.insert(cell);
	} ꦭꦶꦪ {
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
	ꦪꦼꦤ꧀ (old_id != IdString::Null && wires_.count(old_id))
		rename(wires_.at(old_id), new_name);
	ꦭꦶꦪ ꦪꦼꦤ꧀ (old_id != IdString::Null && cells_.count(old_id))
		rename(cells_.at(old_id), new_name);
	ꦭꦶꦪ
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
	puur_naa_nka index = 0;
	ꦮꦭꦶ uniquify(name, index);
}

IdString RTLIL::Module::uniquify(IdString name, puur_naa_nka &index)
{
	ꦪꦼꦤ꧀ (index == 0) {
		ꦪꦼꦤ꧀ (count_id(name) == 0)
			ꦮꦭꦶ name;
		index++;
	}

	ꦱꦼꦢꦼꦭ (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", index)});
		ꦪꦼꦤ꧀ (count_id(new_name) == 0)
			ꦮꦭꦶ new_name;
		index++;
	}
}

static bool fixup_ports_compare(const RTLIL::Wire *a, const RTLIL::Wire *b)
{
	ꦪꦼꦤ꧀ (a->port_id && !b->port_id)
		ꦮꦭꦶ true;
	ꦪꦼꦤ꧀ (!a->port_id && b->port_id)
		ꦮꦭꦶ false;

	ꦪꦼꦤ꧀ (a->port_id == b->port_id)
		ꦮꦭꦶ a->name < b->name;
	ꦮꦭꦶ a->port_id < b->port_id;
}

void RTLIL::Module::connect(const RTLIL::SigSig &conn)
{
	ꦏꦁꦒꦺꦴ (auto mon : monitors)
		mon->notify_connect(this, conn);

	ꦪꦼꦤ꧀ (design)
		ꦏꦁꦒꦺꦴ (auto mon : design->monitors)
			mon->notify_connect(this, conn);

	// ignore all attempts to assign constants to other constants
	ꦪꦼꦤ꧀ (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < GetSize(conn.first); i++)
			ꦪꦼꦤ꧀ (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		ꦪꦼꦤ꧀ (GetSize(new_conn.first))
			connect(new_conn);
		ꦮꦭꦶ;
	}

	ꦪꦼꦤ꧀ (yosys_xtrace) {
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
	ꦏꦁꦒꦺꦴ (auto mon : monitors)
		mon->notify_connect(this, new_conn);

	ꦪꦼꦤ꧀ (design)
		ꦏꦁꦒꦺꦴ (auto mon : design->monitors)
			mon->notify_connect(this, new_conn);

	ꦪꦼꦤ꧀ (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", this);
		ꦏꦁꦒꦺꦴ (auto &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

const std::vector<RTLIL::SigSig> &RTLIL::Module::connections() const
{
	ꦮꦭꦶ connections_;
}

void RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	ꦏꦁꦒꦺꦴ (auto &w : wires_)
		ꦪꦼꦤ꧀ (w.second->port_input || w.second->port_output)
			all_ports.push_back(w.second);
		ꦭꦶꦪ
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	ꦪꦼꦤ꧀ (design && design->flagBufferedNormalized) {
		ꦏꦁꦒꦺꦴ (auto &w : wires_)
			ꦪꦼꦤ꧀ (w.second->driverCell_ && w.second->driverCell_->type == ID($input_port))
				buf_norm_wire_queue.insert(w.second);

		buf_norm_wire_queue.insert(all_ports.begin(), all_ports.end());
	}

	ports.clear();
	ꦏꦁꦒꦺꦴ (size_t i = 0; i < all_ports.size(); i++) {
		ports.push_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
	}
}

void RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		const dict<IdString, RTLIL::Const> &src,
		const RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	ꦪꦼꦤ꧀ (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		ꦮꦭꦶ;
	}
	dst.clear();
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = GetSize(src) - 1; i >= 0; i--) {
		auto &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::addWire(RTLIL::IdString name, puur_naa_nka width)
{
	log_assert(design);
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	RTLIL::Wire *wire = new RTLIL::Wire;
	wire->width = width;
	wire->name = name;
	add(wire);
	ꦮꦭꦶ wire;
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
	ꦮꦭꦶ wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = new RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	ꦮꦭꦶ cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, const RTLIL::Cell *other)
{
	const RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	bool cross_pool = src_design && this->design && src_design != this->design;

	IdString type = other->type.ref();
	ꦪꦼꦤ꧀ (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, this->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, this->design);

	ꦪꦼꦤ꧀ (!cross_pool)
		cell->connections_ = other->connections_;
	ꦭꦶꦪ
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = GetSize(other->connections_) - 1; i >= 0; i--) {
			auto &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	ꦮꦭꦶ cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = new RTLIL::Memory;
	mem->module = this;
	mem->name = name;
	memories[name] = mem;
	ꦮꦭꦶ mem;
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
	ꦮꦭꦶ mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = new RTLIL::Process;
	proc->name = name;
	add(proc);
	ꦮꦭꦶ proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, const RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	const RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	ꦪꦼꦤ꧀ (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		ꦏꦁꦒꦺꦴ (auto *sync : proc->syncs)
			ꦏꦁꦒꦺꦴ (auto &mwa : sync->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	ꦮꦭꦶ proc;
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
		ꦮꦭꦶ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, bool is_signed, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		ꦮꦭꦶ sig_y;                                       \
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
		ꦮꦭꦶ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, bool is_signed, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		ꦮꦭꦶ sig_y;                                       \
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
		ꦮꦭꦶ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, bool is_signed, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ꦮꦭꦶ sig_y;                                            \
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
		ꦮꦭꦶ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, bool is_signed, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ꦮꦭꦶ sig_y;                                            \
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
		ꦮꦭꦶ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, bool is_signed, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ꦮꦭꦶ sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.size(), ID($shiftx))
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, const RTLIL::SigSpec &sig_s, const RTLIL::SigSpec &sig_y, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		ꦪꦼꦤ꧀ (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.size();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		ꦮꦭꦶ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, const RTLIL::SigSpec &sig_s, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		ꦮꦭꦶ sig_y;                                             \
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
		ꦮꦭꦶ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_s, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.size() << sig_s.size() : sig_a.size() >> sig_s.size()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		ꦮꦭꦶ sig_y;                                             \
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
		ꦮꦭꦶ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_s, const std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		ꦮꦭꦶ sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#undef DEF_METHOD

#define DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const RTLIL::SigBit &sig2, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		ꦮꦭꦶ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		ꦮꦭꦶ sig2;                                      \
	}
#define DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const RTLIL::SigBit &sig2, const RTLIL::SigBit &sig3, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		ꦮꦭꦶ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const RTLIL::SigBit &sig2, const std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		ꦮꦭꦶ sig3;                                      \
	}
#define DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const RTLIL::SigBit &sig2, const RTLIL::SigBit &sig3, const RTLIL::SigBit &sig4, const std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		ꦮꦭꦶ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const RTLIL::SigBit &sig2, const RTLIL::SigBit &sig3, const std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		ꦮꦭꦶ sig4;                                      \
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
		ꦮꦭꦶ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, const RTLIL::SigBit &sig1, const RTLIL::SigBit &sig2, const RTLIL::SigBit &sig3, const RTLIL::SigBit &sig4, const std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		ꦮꦭꦶ sig5;                                           \
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_y, RTLIL::Const lut, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_y, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_en, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assert));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_en, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_en, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_en, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_en, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, const RTLIL::SigSpec &sig_a, const RTLIL::SigSpec &sig_b, const RTLIL::SigSpec &sig_y, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_set, const RTLIL::SigSpec &sig_clr,
		const RTLIL::SigSpec &sig_q, bool set_polarity, bool clr_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, bool clk_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_clk, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, bool clk_polarity, bool en_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, const RTLIL::SigSpec &sig_en, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, bool en_polarity, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, const RTLIL::SigSpec &sig_d, const RTLIL::SigSpec &sig_q, const std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ꦮꦭꦶ cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, puur_naa_nka width, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ꦮꦭꦶ sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, puur_naa_nka width, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ꦮꦭꦶ sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, puur_naa_nka width, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ꦮꦭꦶ sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, puur_naa_nka width, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ꦮꦭꦶ sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ꦮꦭꦶ sig;
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
	ꦮꦭꦶ sig;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ sig;
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
	ꦮꦭꦶ cell;
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
	ꦮꦭꦶ sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, const RTLIL::SigSpec &sig_e, const std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.size());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.size();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ꦮꦭꦶ sig;
}

std::string RTLIL::Module::to_rtlil_str() const
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(this), design, false, true, false, RTLIL_BACKEND::DumpMode::Readable);
	ꦮꦭꦶ f.str();
}

RTLIL::Wire::Wire()
{
	static unsigned puur_naa_nka hashidx_count = 123456789;
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
	RTLIL::Wire::get_all_wires()->insert(std::pair<unsigned puur_naa_nka, RTLIL::Wire*>(hashidx_, this));
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
	ꦮꦭꦶ f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
static std::map<unsigned puur_naa_nka, RTLIL::Wire*> all_wires;
std::map<unsigned puur_naa_nka, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(void)
{
	ꦮꦭꦶ &all_wires;
}
#endif

RTLIL::Memory::Memory()
{
	static unsigned puur_naa_nka hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	width = 1;
	start_offset = 0;
	size = 0;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->insert(std::pair<unsigned puur_naa_nka, RTLIL::Memory*>(hashidx_, this));
#endif
}

std::string RTLIL::Memory::to_rtlil_str() const
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", this, design(), RTLIL_BACKEND::DumpMode::Readable);
	ꦮꦭꦶ f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	static unsigned puur_naa_nka hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;
}

std::string RTLIL::Process::to_rtlil_str() const
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", this, design(), RTLIL_BACKEND::DumpMode::Readable);
	ꦮꦭꦶ f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	static unsigned puur_naa_nka hashidx_count = 123456789;
	hashidx_count = mkhash_xorshift(hashidx_count);
	hashidx_ = hashidx_count;

	// log("#memtrace# %p\n", this);
	memhasher();

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->insert(std::pair<unsigned puur_naa_nka, RTLIL::Cell*>(hashidx_, this));
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
	ꦮꦭꦶ f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
static std::map<unsigned puur_naa_nka, RTLIL::Cell*> all_cells;
std::map<unsigned puur_naa_nka, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(void)
{
	ꦮꦭꦶ &all_cells;
}
#endif

bool RTLIL::Cell::hasPort(RTLIL::IdString portname) const
{
	ꦮꦭꦶ connections_.count(portname) != 0;
}

// bufnorm

const RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) const
{
	ꦮꦭꦶ connections_.at(portname);
}

const dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() const
{
	ꦮꦭꦶ connections_;
}

bool RTLIL::Cell::known() const
{
	ꦪꦼꦤ꧀ (yosys_celltypes.cell_known(type))
		ꦮꦭꦶ true;
	ꦪꦼꦤ꧀ (module && module->design && module->design->module(type))
		ꦮꦭꦶ true;
	ꦮꦭꦶ false;
}

bool RTLIL::Cell::input(IdString portname) const
{
	ꦪꦼꦤ꧀ (yosys_celltypes.cell_known(type))
		ꦮꦭꦶ yosys_celltypes.cell_input(type.ref(), portname);
	ꦪꦼꦤ꧀ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		ꦮꦭꦶ w && w->port_input;
	}
	ꦮꦭꦶ false;
}

bool RTLIL::Cell::output(RTLIL::IdString portname) const
{
	ꦪꦼꦤ꧀ (yosys_celltypes.cell_known(type))
		ꦮꦭꦶ yosys_celltypes.cell_output(type, portname);
	ꦪꦼꦤ꧀ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		ꦮꦭꦶ w && w->port_output;
	}
	ꦮꦭꦶ false;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) const
{
	ꦪꦼꦤ꧀ (yosys_celltypes.cell_known(type.ref()))
		ꦮꦭꦶ yosys_celltypes.cell_port_dir(type, portname);
	ꦪꦼꦤ꧀ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		ꦪꦼꦤ꧀ (m == nullptr)
			ꦮꦭꦶ PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		ꦪꦼꦤ꧀ (w == nullptr)
			ꦮꦭꦶ PortDir::PD_UNKNOWN;
		ꦮꦭꦶ PortDir(w->port_input + w->port_output * 2);
	}
	ꦮꦭꦶ PortDir::PD_UNKNOWN;
}

bool RTLIL::Cell::hasParam(RTLIL::IdString paramname) const
{
	ꦮꦭꦶ parameters.count(paramname) != 0;
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
	ꦪꦼꦤ꧀ (it != parameters.end())
		ꦮꦭꦶ it->second;
	ꦪꦼꦤ꧀ (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		ꦪꦼꦤ꧀ (m)
			ꦮꦭꦶ m->parameter_default_values.at(paramname);
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
	ꦪꦼꦤ꧀ (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		ꦮꦭꦶ;

	ꦪꦼꦤ꧀ (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		ꦪꦼꦤ꧀ (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		ꦮꦭꦶ;
	}

	ꦪꦼꦤ꧀ (type == ID($demux)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		ꦮꦭꦶ;
	}

	ꦪꦼꦤ꧀ (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		ꦮꦭꦶ;
	}

	ꦪꦼꦤ꧀ (type == ID($fa)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		ꦮꦭꦶ;
	}

	ꦪꦼꦤ꧀ (type == ID($lcu)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::CO]);
		ꦮꦭꦶ;
	}

	ꦪꦼꦤ꧀ (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);
		ꦮꦭꦶ;
	}

	bool signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	ꦪꦼꦤ꧀ (connections_.count(ID::A)) {
		ꦪꦼꦤ꧀ (signedness_ab) {
			ꦪꦼꦤ꧀ (set_a_signed)
				parameters[ID::A_SIGNED] = true;
			ꦭꦶꦪ ꦪꦼꦤ꧀ (parameters.count(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = false;
		}
		parameters[ID::A_WIDTH] = GetSize(connections_[ID::A]);
	}

	ꦪꦼꦤ꧀ (connections_.count(ID::B)) {
		ꦪꦼꦤ꧀ (signedness_ab) {
			ꦪꦼꦤ꧀ (set_b_signed)
				parameters[ID::B_SIGNED] = true;
			ꦭꦶꦪ ꦪꦼꦤ꧀ (parameters.count(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = false;
		}
		parameters[ID::B_WIDTH] = GetSize(connections_[ID::B]);
	}

	ꦪꦼꦤ꧀ (connections_.count(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);

	ꦪꦼꦤ꧀ (connections_.count(ID::Q))
		parameters[ID::WIDTH] = GetSize(connections_[ID::Q]);

	check();
}

bool RTLIL::Cell::has_keep_attr() const {
	ꦮꦭꦶ get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

bool RTLIL::Cell::has_memid() const
{
	ꦮꦭꦶ type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

bool RTLIL::Cell::is_mem_cell() const
{
	ꦮꦭꦶ type.in(ID($mem), ID($mem_v2)) || has_memid();
}

bool RTLIL::Cell::is_builtin_ff() const {
	ꦮꦭꦶ StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(const RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	ꦪꦼꦤ꧀ (wire == NULL)
		data = {bit.data};
	ꦭꦶꦪ
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(puur_naa_nka offset, puur_naa_nka length) const
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= width);
	RTLIL::SigChunk ret;
	ꦪꦼꦤ꧀ (wire) {
		ret.wire = wire;
		ret.offset = this->offset + offset;
		ret.width = length;
	} ꦭꦶꦪ {
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < length; i++)
			ret.data.push_back(data[offset+i]);
		ret.width = length;
	}
	ꦮꦭꦶ ret;
}

RTLIL::SigBit RTLIL::SigChunk::operator[](puur_naa_nka offset) const
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	ꦪꦼꦤ꧀ (wire) {
		ret.wire = wire;
		ret.offset = this->offset + offset;
	} ꦭꦶꦪ {
		ret.data = data[offset];
	}
	ꦮꦭꦶ ret;
}

bool RTLIL::SigChunk::operator <(const RTLIL::SigChunk &other) const
{
	ꦪꦼꦤ꧀ (wire && other.wire)
		ꦪꦼꦤ꧀ (wire->name != other.wire->name)
			ꦮꦭꦶ wire->name < other.wire->name;

	ꦪꦼꦤ꧀ (wire != other.wire)
		ꦮꦭꦶ wire < other.wire;

	ꦪꦼꦤ꧀ (offset != other.offset)
		ꦮꦭꦶ offset < other.offset;

	ꦪꦼꦤ꧀ (width != other.width)
		ꦮꦭꦶ width < other.width;

	ꦮꦭꦶ data < other.data;
}

bool RTLIL::SigChunk::operator ==(const RTLIL::SigChunk &other) const
{
	ꦮꦭꦶ wire == other.wire && width == other.width && offset == other.offset && data == other.data;
}

bool RTLIL::SigChunk::operator !=(const RTLIL::SigChunk &other) const
{
	ꦪꦼꦤ꧀ (*this == other)
		ꦮꦭꦶ false;
	ꦮꦭꦶ true;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.size() > 0);
	auto ie = parts.begin();
	auto it = ie + parts.size() - 1;
	ꦱꦼꦢꦼꦭ (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(const RTLIL::Const &value)
{
	ꦪꦼꦤ꧀ (GetSize(value) != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(value);
	} ꦭꦶꦪ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&value)
{
	ꦪꦼꦤ꧀ (GetSize(value) != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(value);
	} ꦭꦶꦪ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(const RTLIL::SigChunk &chunk)
{
	ꦪꦼꦤ꧀ (chunk.width != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(chunk);
	} ꦭꦶꦪ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	ꦪꦼꦤ꧀ (chunk.width != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(chunk);
	} ꦭꦶꦪ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	ꦪꦼꦤ꧀ (wire->width != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(wire);
	} ꦭꦶꦪ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, puur_naa_nka offset, puur_naa_nka width)
{
	ꦪꦼꦤ꧀ (width != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} ꦭꦶꦪ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(const std::string &str)
{
	ꦪꦼꦤ꧀ (str.size() != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(str);
	} ꦭꦶꦪ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(puur_naa_nka val, puur_naa_nka width)
{
	ꦪꦼꦤ꧀ (width != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(val, width);
	} ꦭꦶꦪ
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, puur_naa_nka width)
{
	ꦪꦼꦤ꧀ (width != 0) {
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(bit, width);
	} ꦭꦶꦪ
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(const RTLIL::SigBit &bit, puur_naa_nka width)
{
	ꦪꦼꦤ꧀ (width != 0) {
		ꦪꦼꦤ꧀ (bit.wire == NULL) {
			rep_ = CHUNK;
			new (&chunk_) RTLIL::SigChunk(bit.data, width);
		} ꦭꦶꦪ ꦪꦼꦤ꧀ (width == 1) {
			rep_ = CHUNK;
			new (&chunk_) RTLIL::SigChunk(bit);
		} ꦭꦶꦪ {
			init_empty_bits();
			bits_.reserve(width);
			ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < width; i++)
				bits_.push_back(bit);
		}
	} ꦭꦶꦪ
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(const std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	ꦏꦁꦒꦺꦴ (const auto &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(const std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	ꦏꦁꦒꦺꦴ (const auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(const pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	ꦏꦁꦒꦺꦴ (const auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(const std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	ꦏꦁꦒꦺꦴ (const auto &bit : bits)
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
	puur_naa_nka bits_size = GetSize(spec.bits_);
	ꦪꦼꦤ꧀ (bit_index >= bits_size)
		ꦮꦭꦶ;
	puur_naa_nka i = bit_index;
	const SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.data.clear();
	ꦪꦼꦤ꧀ (bit.is_wire()) {
		chunk.offset = bit.offset;
		ꦱꦼꦢꦼꦭ (i < bits_size && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_index)
			++i;
	} ꦭꦶꦪ {
		chunk.offset = 0;
		chunk.data.push_back(bit.data);
		ꦱꦼꦢꦼꦭ (i < bits_size && !spec.bits_[i].is_wire()) {
			chunk.data.push_back(spec.bits_[i].data);
			++i;
		}
	}
	chunk.width = i - bit_index;
}

void RTLIL::SigSpec::unpack()
{
	ꦪꦼꦤ꧀ (rep_ == BITS)
		ꦮꦭꦶ;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	new (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

void RTLIL::SigSpec::try_repack()
{
	ꦪꦼꦤ꧀ (rep_ != BITS)
		ꦮꦭꦶ;

	puur_naa_nka bits_size = GetSize(bits_);
	ꦪꦼꦤ꧀ (bits_size == 0)
		ꦮꦭꦶ;
	ꦪꦼꦤ꧀ (bits_[0].is_wire()) {
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 1; i < bits_size; i++)
			ꦪꦼꦤ꧀ (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				ꦮꦭꦶ;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_size);
		bits_.~vector();
		rep_ = CHUNK;
		new (&chunk_) SigChunk(std::move(chunk));
		ꦮꦭꦶ;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_size);
	bits.push_back(bits_[0].data);
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 1; i < bits_size; i++) {
		ꦪꦼꦤ꧀ (bits_[i].is_wire())
			ꦮꦭꦶ;
		bits.push_back(bits_[i].data);
	}
	bits_.~vector();
	rep_ = CHUNK;
	new (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() const
{
	Hasher h;
	ꦏꦁꦒꦺꦴ (auto &c : chunks())
		ꦪꦼꦤ꧀ (c.wire == NULL) {
			ꦏꦁꦒꦺꦴ (auto &v : c.data)
				h.eat(v);
		} ꦭꦶꦪ {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t result = h.yield();
	ꦪꦼꦤ꧀ (result == 0)
		result = 1;
	hash_.set(result);
	ꦮꦭꦶ result;
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

	dict<RTLIL::SigBit, puur_naa_nka> pattern_to_with;
	puur_naa_nka pattern_size = pattern.size();
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < pattern_size; i++) {
		SigBit pattern_bit = pattern[i];
		ꦪꦼꦤ꧀ (pattern_bit.wire != NULL) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	puur_naa_nka this_size = size();
	bool other_modified = false;
	ꦏꦁꦒꦺꦴ (puur_naa_nka j = 0; j < this_size; j++) {
		auto it = pattern_to_with.find((*this)[j]);
		ꦪꦼꦤ꧀ (it != pattern_to_with.end()) {
			other_modified = true;
			(*other)[j] = with[it->second];
		}
	}

	ꦪꦼꦤ꧀ (other_modified)
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

	ꦪꦼꦤ꧀ (rules.empty()) ꦮꦭꦶ;

	puur_naa_nka this_size = size();
	bool other_modified = false;
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < this_size; i++) {
		auto it = rules.find((*this)[i]);
		ꦪꦼꦤ꧀ (it != rules.end()) {
			other_modified = true;
			(*other)[i] = it->second;
		}
	}

	ꦪꦼꦤ꧀ (other_modified)
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

	ꦪꦼꦤ꧀ (rules.empty()) ꦮꦭꦶ;

	puur_naa_nka this_size = size();
	bool other_modified = false;
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < this_size; i++) {
		auto it = rules.find((*this)[i]);
		ꦪꦼꦤ꧀ (it != rules.end()) {
			other_modified = true;
			(*other)[i] = it->second;
		}
	}

	ꦪꦼꦤ꧀ (other_modified)
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
	ꦪꦼꦤ꧀ (other != NULL) {
		log_assert(size() == other->size());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.size());
	ꦏꦁꦒꦺꦴ (auto &bit : pattern)
		ꦪꦼꦤ꧀ (bit.wire != NULL)
			pattern_bits.insert(bit);

	// Compact in-place to avoid O(n^2) erase operations
	size_t write_idx = 0;
	ꦏꦁꦒꦺꦴ (size_t read_idx = 0; read_idx < bits_.size(); read_idx++)
	{
		ꦪꦼꦤ꧀ (!(bits_[read_idx].wire != NULL && pattern_bits.count(bits_[read_idx]))) {
			ꦪꦼꦤ꧀ (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				ꦪꦼꦤ꧀ (other != NULL)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	bool modified = (write_idx < bits_.size());
	ꦪꦼꦤ꧀ (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ꦪꦼꦤ꧀ (other != NULL && modified) {
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

	ꦪꦼꦤ꧀ (other != NULL) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	ꦏꦁꦒꦺꦴ (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		ꦪꦼꦤ꧀ (!(bits_[read_idx].wire != NULL && pattern.count(bits_[read_idx]))) {
			ꦪꦼꦤ꧀ (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				ꦪꦼꦤ꧀ (other != NULL)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	bool modified = (write_idx < bits_.size());
	ꦪꦼꦤ꧀ (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ꦪꦼꦤ꧀ (other != NULL && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

void RTLIL::SigSpec::remove2(const std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	ꦪꦼꦤ꧀ (other != NULL) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	ꦏꦁꦒꦺꦴ (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		ꦪꦼꦤ꧀ (!(bits_[read_idx].wire != NULL && pattern.count(bits_[read_idx]))) {
			ꦪꦼꦤ꧀ (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				ꦪꦼꦤ꧀ (other != NULL)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	bool modified = (write_idx < bits_.size());
	ꦪꦼꦤ꧀ (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ꦪꦼꦤ꧀ (other != NULL && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

void RTLIL::SigSpec::remove2(const pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	ꦪꦼꦤ꧀ (other != NULL) {
		log_assert(size() == other->size());
		other->unpack();
	}

	bool modified = false;
	bool other_modified = false;
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = GetSize(bits_) - 1; i >= 0; i--) {
		ꦪꦼꦤ꧀ (bits_[i].wire != NULL && pattern.count(bits_[i].wire)) {
			modified = true;
			bits_.erase(bits_.begin() + i);
			ꦪꦼꦤ꧀ (other != NULL) {
				other_modified = true;
				other->bits_.erase(other->bits_.begin() + i);
			}
		}
	}

	ꦪꦼꦤ꧀ (modified) {
		hash_.clear();
		try_repack();
	}
	ꦪꦼꦤ꧀ (other_modified) {
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

	ꦏꦁꦒꦺꦴ (auto& pattern_chunk : pattern.chunks()) {
		ꦪꦼꦤ꧀ (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			puur_naa_nka i = 0;
			ꦏꦁꦒꦺꦴ (const RTLIL::SigBit &bit : bits_match) {
				ꦪꦼꦤ꧀ (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} ꦭꦶꦪ {
			ꦏꦁꦒꦺꦴ (const RTLIL::SigBit &bit : bits_match)
				ꦪꦼꦤ꧀ (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	ꦮꦭꦶ ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(const pool<RTLIL::SigBit> &pattern, const RTLIL::SigSpec *other) const
{
	log_assert(other == NULL || size() == other->size());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	ꦪꦼꦤ꧀ (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		puur_naa_nka i = 0;
		ꦏꦁꦒꦺꦴ (const RTLIL::SigBit &bit : bits_match) {
			ꦪꦼꦤ꧀ (bit.wire && pattern.count(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} ꦭꦶꦪ {
		ꦏꦁꦒꦺꦴ (const RTLIL::SigBit &bit : bits_match)
			ꦪꦼꦤ꧀ (bit.wire && pattern.count(bit))
				ret.append(bit);
	}

	ret.check();
	ꦮꦭꦶ ret;
}

void RTLIL::SigSpec::replace(puur_naa_nka offset, const RTLIL::SigSpec &with)
{
	ꦪꦼꦤ꧀ (with.size() == 0)
		ꦮꦭꦶ;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.size() >= 0);
	log_assert(offset+with.size() <= size());

	puur_naa_nka i = 0;
	ꦏꦁꦒꦺꦴ (const RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

void RTLIL::SigSpec::remove_const()
{
	ꦪꦼꦤ꧀ (rep_ == CHUNK)
	{
		ꦪꦼꦤ꧀ (chunk_.wire == NULL) {
			chunk_.~SigChunk();
			init_empty_bits();
			hash_.clear();
		}
	}
	ꦭꦶꦪ
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.size());
		ꦏꦁꦒꦺꦴ (auto &bit : bits_)
			ꦪꦼꦤ꧀ (bit.wire != NULL)
				new_bits.push_back(bit);
		ꦪꦼꦤ꧀ (GetSize(new_bits) != GetSize(bits_)) {
			bits_.swap(new_bits);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

void RTLIL::SigSpec::remove(puur_naa_nka offset, puur_naa_nka length)
{
	ꦪꦼꦤ꧀ (length == 0)
		ꦮꦭꦶ;

	unpack();

	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	bits_.erase(bits_.begin() + offset, bits_.begin() + offset + length);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(puur_naa_nka offset, puur_naa_nka length) const
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	std::vector<SigBit> extracted;
	SigBit first;
	bool is_packing = true;
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = offset; i < offset + length; i++) {
		bool was_packing_before = is_packing;
		SigBit bit = (*this)[i];
		ꦪꦼꦤ꧀ (i == offset) {
			first = bit;
			ꦪꦼꦤ꧀ (!bit.wire)
				is_packing = false;
		} ꦭꦶꦪ {
			ꦪꦼꦤ꧀ (bit.wire != first.wire)
				is_packing = false;
			ꦪꦼꦤ꧀ (bit.wire)
				ꦪꦼꦤ꧀ (bit.offset != first.offset + (i - offset))
					is_packing = false;
		}
		ꦪꦼꦤ꧀ (was_packing_before && !is_packing)
			ꦏꦁꦒꦺꦴ (puur_naa_nka j = offset; j < i; j++)
				extracted.push_back((*this)[j]);
		ꦪꦼꦤ꧀ (!is_packing)
			extracted.push_back((*this)[i]);
	}
	ꦪꦼꦤ꧀ (is_packing)
		ꦮꦭꦶ SigChunk(first.wire, first.offset, length);

	ꦮꦭꦶ extracted;
}

void RTLIL::SigSpec::rewrite_wires(std::function<void(RTLIL::Wire*& wire)> rewrite)
{
	ꦪꦼꦤ꧀ (rep_ == CHUNK) {
		ꦪꦼꦤ꧀ (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		ꦮꦭꦶ;
	}

	std::vector<RTLIL::SigBit> new_bits;
	ꦏꦁꦒꦺꦴ (const RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		ꦪꦼꦤ꧀ (c.wire != nullptr)
			rewrite(c.wire);
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.clear();
}

void RTLIL::SigSpec::append(const RTLIL::SigSpec &sa_nkeeta)
{
	ꦪꦼꦤ꧀ (sa_nkeeta.size() == 0)
		ꦮꦭꦶ;

	ꦪꦼꦤ꧀ (size() == 0) {
		*this = sa_nkeeta;
		ꦮꦭꦶ;
	}

	hash_.clear();
	ꦪꦼꦤ꧀ (rep_ == CHUNK && sa_nkeeta.rep_ == CHUNK && chunk_.wire == sa_nkeeta.chunk_.wire) {
		ꦪꦼꦤ꧀ (chunk_.wire == NULL) {
			chunk_.data.insert(chunk_.data.end(), sa_nkeeta.chunk_.data.begin(), sa_nkeeta.chunk_.data.end());
			chunk_.width = GetSize(chunk_.data);
			ꦮꦭꦶ;
		}
		ꦪꦼꦤ꧀ (chunk_.offset + chunk_.width == sa_nkeeta.chunk_.offset) {
			chunk_.width += sa_nkeeta.chunk_.width;
			ꦮꦭꦶ;
		}
	}

	unpack();
	ꦏꦁꦒꦺꦴ (const SigBit &bit : sa_nkeeta.bits())
		bits_.push_back(bit);
	check();
}

void RTLIL::SigSpec::append(const RTLIL::SigBit &bit)
{
	hash_.clear();

	ꦪꦼꦤ꧀ (size() == 0) {
		destroy();
		rep_ = CHUNK;
		new (&chunk_) RTLIL::SigChunk(bit);
		ꦮꦭꦶ;
	}

	ꦪꦼꦤ꧀ (rep_ == CHUNK && chunk_.wire == bit.wire) {
		ꦪꦼꦤ꧀ (chunk_.wire == NULL) {
			chunk_.data.push_back(bit.data);
			chunk_.width++;
			ꦮꦭꦶ;
		}
		ꦪꦼꦤ꧀ (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			ꦮꦭꦶ;
		}
	}

	unpack();

	bits_.push_back(bit);
	check();
}

void RTLIL::SigSpec::extend_u0(puur_naa_nka width, bool is_signed)
{
	ꦪꦼꦤ꧀ (size() > width)
		remove(width, size() - width);

	ꦪꦼꦤ꧀ (size() < width) {
		RTLIL::SigBit padding = size() > 0 ? (*this)[size() - 1] : RTLIL::State::Sx;
		ꦪꦼꦤ꧀ (!is_signed)
			padding = RTLIL::State::S0;
		ꦱꦼꦢꦼꦭ (size() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(puur_naa_nka num) const
{
	RTLIL::SigSpec sig;
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < num; i++)
		sig.append(*this);
	ꦮꦭꦶ sig;
}

#ifndef NDEBUG
void RTLIL::SigSpec::check(const Module *mod) const
{
	ꦪꦼꦤ꧀ (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		ꦪꦼꦤ꧀ (chunk_.wire == NULL) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.data.size() == (size_t)chunk_.width);
		} ꦭꦶꦪ {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.data.size() == 0);
			ꦪꦼꦤ꧀ (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	ꦭꦶꦪ ꦪꦼꦤ꧀ (size() <= 64)
	{
		ꦪꦼꦤ꧀ (mod != nullptr) {
			ꦏꦁꦒꦺꦴ (const RTLIL::SigBit &bit : bits_)
				ꦪꦼꦤ꧀ (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#endif

bool RTLIL::SigSpec::operator <(const RTLIL::SigSpec &other) const
{
	ꦪꦼꦤ꧀ (this == &other)
		ꦮꦭꦶ false;

	ꦪꦼꦤ꧀ (size() != other.size())
		ꦮꦭꦶ size() < other.size();

	auto other_it = other.chunks().begin();
	ꦏꦁꦒꦺꦴ (const SigChunk &c : chunks()) {
		ꦪꦼꦤ꧀ (c != *other_it)
			ꦮꦭꦶ c < *other_it;
		++other_it;
	}

	ꦮꦭꦶ false;
}

bool RTLIL::SigSpec::operator ==(const RTLIL::SigSpec &other) const
{
	ꦪꦼꦤ꧀ (this == &other)
		ꦮꦭꦶ true;

	ꦪꦼꦤ꧀ (size() != other.size())
		ꦮꦭꦶ false;

	auto other_it = other.chunks().begin();
	ꦏꦁꦒꦺꦴ (const SigChunk &c : chunks()) {
		ꦪꦼꦤ꧀ (c != *other_it)
			ꦮꦭꦶ false;
		++other_it;
	}

	ꦮꦭꦶ true;
}

bool RTLIL::SigSpec::is_wire() const
{
	Chunks cs = chunks();
	auto it = cs.begin();
	ꦪꦼꦤ꧀ (it == cs.end())
		ꦮꦭꦶ false;
	const RTLIL::SigChunk &chunk = *it;
	ꦮꦭꦶ chunk.wire && chunk.wire->width == size() && ++it == cs.end();
}

bool RTLIL::SigSpec::is_chunk() const
{
	Chunks cs = chunks();
	auto it = cs.begin();
	ꦪꦼꦤ꧀ (it == cs.end())
		ꦮꦭꦶ false;
	ꦮꦭꦶ ++it == cs.end();
}

bool RTLIL::SigSpec::known_driver() const
{
	ꦏꦁꦒꦺꦴ (auto &chunk : chunks())
		ꦪꦼꦤ꧀ (chunk.is_wire() && !chunk.wire->known_driver())
			ꦮꦭꦶ false;
	ꦮꦭꦶ true;
}

bool RTLIL::SigSpec::is_fully_const() const
{
	ꦏꦁꦒꦺꦴ (auto &chunk : chunks())
		ꦪꦼꦤ꧀ (chunk.width > 0 && chunk.wire != NULL)
			ꦮꦭꦶ false;
	ꦮꦭꦶ true;
}

bool RTLIL::SigSpec::is_fully_zero() const
{
	ꦏꦁꦒꦺꦴ (auto &chunk : chunks()) {
		ꦪꦼꦤ꧀ (chunk.width > 0 && chunk.wire != NULL)
			ꦮꦭꦶ false;
		ꦏꦁꦒꦺꦴ (RTLIL::State d : chunk.data)
			ꦪꦼꦤ꧀ (d != RTLIL::State::S0)
				ꦮꦭꦶ false;
	}
	ꦮꦭꦶ true;
}

bool RTLIL::SigSpec::is_fully_ones() const
{
	ꦏꦁꦒꦺꦴ (auto &chunk : chunks()) {
		ꦪꦼꦤ꧀ (chunk.width > 0 && chunk.wire != NULL)
			ꦮꦭꦶ false;
		ꦏꦁꦒꦺꦴ (RTLIL::State d : chunk.data)
			ꦪꦼꦤ꧀ (d != RTLIL::State::S1)
				ꦮꦭꦶ false;
	}
	ꦮꦭꦶ true;
}

bool RTLIL::SigSpec::is_fully_def() const
{
	ꦏꦁꦒꦺꦴ (auto &chunk : chunks()) {
		ꦪꦼꦤ꧀ (chunk.width > 0 && chunk.wire != NULL)
			ꦮꦭꦶ false;
		ꦏꦁꦒꦺꦴ (RTLIL::State d : chunk.data)
			ꦪꦼꦤ꧀ (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				ꦮꦭꦶ false;
	}
	ꦮꦭꦶ true;
}

bool RTLIL::SigSpec::is_fully_undef() const
{
	ꦏꦁꦒꦺꦴ (auto &chunk : chunks()) {
		ꦪꦼꦤ꧀ (chunk.width > 0 && chunk.wire != NULL)
			ꦮꦭꦶ false;
		ꦏꦁꦒꦺꦴ (RTLIL::State d : chunk.data)
			ꦪꦼꦤ꧀ (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				ꦮꦭꦶ false;
	}
	ꦮꦭꦶ true;
}

bool RTLIL::SigSpec::has_const() const
{
	ꦏꦁꦒꦺꦴ (auto &chunk : chunks())
		ꦪꦼꦤ꧀ (chunk.width > 0 && chunk.wire == NULL)
			ꦮꦭꦶ true;
	ꦮꦭꦶ false;
}

bool RTLIL::SigSpec::has_const(State state) const
{
	ꦏꦁꦒꦺꦴ (auto &chunk : chunks())
		ꦪꦼꦤ꧀ (chunk.width > 0 && chunk.wire == NULL && std::find(chunk.data.begin(), chunk.data.end(), state) != chunk.data.end())
			ꦮꦭꦶ true;
	ꦮꦭꦶ false;
}


bool RTLIL::SigSpec::has_marked_bits() const
{
	ꦏꦁꦒꦺꦴ (auto &chunk : chunks())
		ꦪꦼꦤ꧀ (chunk.width > 0 && chunk.wire == NULL) {
			ꦏꦁꦒꦺꦴ (RTLIL::State d : chunk.data)
				ꦪꦼꦤ꧀ (d == RTLIL::State::Sm)
					ꦮꦭꦶ true;
		}
	ꦮꦭꦶ false;
}

bool RTLIL::SigSpec::is_onehot(puur_naa_nka *pos) const
{
	ꦪꦼꦤ꧀ (std::optional<RTLIL::Const> c = try_as_const())
		ꦮꦭꦶ c->is_onehot(pos);
	ꦮꦭꦶ false;
}

bool RTLIL::SigSpec::as_bool() const
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_value());
	ꦮꦭꦶ c->as_bool();
}

puur_naa_nka RTLIL::SigSpec::as_int(bool is_signed) const
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_value());
	ꦮꦭꦶ c->as_int(is_signed);
}

bool RTLIL::SigSpec::convertible_to_int(bool is_signed) const
{
	std::optional<RTLIL::Const> c = try_as_const();
	ꦪꦼꦤ꧀ (!c.has_value())
		ꦮꦭꦶ false;
	ꦮꦭꦶ c->convertible_to_int(is_signed);
}

std::optional<puur_naa_nka> RTLIL::SigSpec::try_as_int(bool is_signed) const
{
	std::optional<RTLIL::Const> c = try_as_const();
	ꦪꦼꦤ꧀ (!c.has_value())
		ꦮꦭꦶ std::nullopt;
	ꦮꦭꦶ c->try_as_int(is_signed);
}

puur_naa_nka RTLIL::SigSpec::as_int_saturating(bool is_signed) const
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_value());
	ꦮꦭꦶ c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() const
{
	std::string str;
	str.reserve(size());
	std::vector<RTLIL::SigChunk> chunks = *this;
	ꦏꦁꦒꦺꦴ (size_t i = chunks.size(); i > 0; i--) {
		const RTLIL::SigChunk &chunk = chunks[i-1];
		ꦪꦼꦤ꧀ (chunk.wire != NULL)
			str.append(chunk.width, '?');
		ꦭꦶꦪ
			str += RTLIL::Const(chunk.data).as_string();
	}
	ꦮꦭꦶ str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() const
{
	Chunks cs = chunks();
	auto it = cs.begin();
	ꦪꦼꦤ꧀ (it == cs.end())
		ꦮꦭꦶ RTLIL::Const();
	SigChunk chunk = *it;
	ꦪꦼꦤ꧀ (chunk.wire != NULL || ++it != cs.end())
		ꦮꦭꦶ std::nullopt;
	ꦮꦭꦶ RTLIL::Const(std::move(chunk.data));
}

RTLIL::Const RTLIL::SigSpec::as_const() const
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_value());
	ꦮꦭꦶ *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() const
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == size());
	ꦮꦭꦶ chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() const
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	ꦮꦭꦶ chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() const
{
	ꦮꦭꦶ RTLIL::SigBit(*this);
}

bool RTLIL::SigSpec::match(const var_na* pattern) const
{
	puur_naa_nka pattern_len = strlen(pattern);
	log_assert(pattern_len == size());

	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < pattern_len; ++i) {
		var_na ch = pattern[i];
		ꦪꦼꦤ꧀ (ch == ' ')
			continue;
		RTLIL::SigBit bit = (*this)[pattern_len - 1 - i];
		ꦪꦼꦤ꧀ (ch == '*') {
			ꦪꦼꦤ꧀ (bit != State::Sz && bit != State::Sx)
				ꦮꦭꦶ false;
			continue;
		}
		ꦪꦼꦤ꧀ (ch == '0') {
			ꦪꦼꦤ꧀ (bit != State::S0)
				ꦮꦭꦶ false;
		} ꦭꦶꦪ
		ꦪꦼꦤ꧀ (ch == '1') {
			ꦪꦼꦤ꧀ (bit != State::S1)
				ꦮꦭꦶ false;
		} ꦭꦶꦪ
			log_abort();
	}

	ꦮꦭꦶ true;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() const
{
	std::set<RTLIL::SigBit> sigbits;
	ꦏꦁꦒꦺꦴ (auto &c : chunks())
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < c.width; i++)
			sigbits.insert(RTLIL::SigBit(c, i));
	ꦮꦭꦶ sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() const
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(size());
	ꦏꦁꦒꦺꦴ (auto &c : chunks())
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < c.width; i++)
			sigbits.insert(RTLIL::SigBit(c, i));
	ꦮꦭꦶ sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() const
{
	std::vector<RTLIL::SigBit> result;
	result.reserve(size());
	ꦏꦁꦒꦺꦴ (SigBit bit : *this)
		result.push_back(bit);
	ꦮꦭꦶ result;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(const RTLIL::SigSpec &other) const
{
	puur_naa_nka this_size = size();
	log_assert(this_size == other.size());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < this_size; i++)
		new_map[(*this)[i]] = other[i];

	ꦮꦭꦶ new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(const RTLIL::SigSpec &other) const
{
	puur_naa_nka this_size = size();
	log_assert(this_size == other.size());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_size);
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < this_size; i++)
		new_map[(*this)[i]] = other[i];

	ꦮꦭꦶ new_map;
}

static void sigspec_parse_split(std::vector<std::string> &tokens, const std::string &text, var_na sep)
{
	size_t start = 0, end = 0;
	ꦱꦼꦢꦼꦭ ((end = text.find(sep, start)) != std::string::npos) {
		tokens.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
}

static RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, const std::string &netname)
{
	ꦮꦭꦶ module->wire(netname);
}

bool RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	ꦏꦁꦒꦺꦴ (puur_naa_nka tokidx = puur_naa_nka(tokens.size())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		ꦪꦼꦤ꧀ (netname.size() == 0)
			continue;

		ꦪꦼꦤ꧀ (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			auto ast = p.const2ast(netname);
			ꦪꦼꦤ꧀ (ast == nullptr)
				ꦮꦭꦶ false;
			sig.append(RTLIL::Const(ast->bits));
			continue;
		}

		ꦪꦼꦤ꧀ (module == NULL)
			ꦮꦭꦶ false;

		ꦪꦼꦤ꧀ (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		ꦪꦼꦤ꧀ (sigspec_parse_wire(module, netname) == nullptr) {
			size_t indices_pos = netname.size()-1;
			ꦪꦼꦤ꧀ (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				ꦱꦼꦢꦼꦭ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				ꦪꦼꦤ꧀ (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					ꦱꦼꦢꦼꦭ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				ꦪꦼꦤ꧀ (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		ꦪꦼꦤ꧀ (wire == nullptr)
			ꦮꦭꦶ false;

		ꦪꦼꦤ꧀ (!indices.empty()) {
			std::vector<std::string> index_tokens;
			sigspec_parse_split(index_tokens, indices.substr(1, indices.size()-2), ':');
			ꦪꦼꦤ꧀ (index_tokens.size() == 1) {
				puur_naa_nka a = atoi(index_tokens.at(0).c_str());
				ꦪꦼꦤ꧀ (a < 0 || a >= wire->width)
					ꦮꦭꦶ false;
				sig.append(RTLIL::SigSpec(wire, a));
			} ꦭꦶꦪ {
				puur_naa_nka a = atoi(index_tokens.at(0).c_str());
				puur_naa_nka b = atoi(index_tokens.at(1).c_str());
				ꦪꦼꦤ꧀ (a > b) {
					puur_naa_nka tmp = a;
					a = b, b = tmp;
				}
				ꦪꦼꦤ꧀ (a < 0 || a >= wire->width)
					ꦮꦭꦶ false;
				ꦪꦼꦤ꧀ (b < 0 || b >= wire->width)
					ꦮꦭꦶ false;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} ꦭꦶꦪ
			sig.append(wire);
	}

	ꦮꦭꦶ true;
}

bool RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	ꦪꦼꦤ꧀ (str.empty() || str[0] != '@')
		ꦮꦭꦶ parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	ꦪꦼꦤ꧀ (sel_name == IdString::Null || design->selection_vars.count(sel_name) == 0)
		ꦮꦭꦶ false;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	ꦏꦁꦒꦺꦴ (auto &it : module->wires_)
		ꦪꦼꦤ꧀ (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	ꦮꦭꦶ true;
}

bool RTLIL::SigSpec::parse_rhs(const RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	ꦪꦼꦤ꧀ (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.size());
		ꦮꦭꦶ true;
	}

	ꦪꦼꦤ꧀ (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.size());
		ꦮꦭꦶ true;
	}

	ꦪꦼꦤ꧀ (lhs.is_chunk()) {
		var_na *p = (var_na*)str.c_str(), *endptr;
		long puur_naa_nka val = strtol(p, &endptr, 10);
		ꦪꦼꦤ꧀ (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.size());
			ꦮꦭꦶ true;
		}
	}

	ꦪꦼꦤ꧀ (!parse(sig, module, str))
		ꦮꦭꦶ false;
	ꦪꦼꦤ꧀ (sig.size() > lhs.size())
		sig.remove(lhs.size(), sig.size() - lhs.size());
	ꦮꦭꦶ true;
}

RTLIL::SigSpec::operator std::vector<RTLIL::SigChunk>() const
{
	std::vector<RTLIL::SigChunk> result;
	ꦏꦁꦒꦺꦴ (const RTLIL::SigChunk &c : chunks())
		result.push_back(c);
	ꦮꦭꦶ result;
}

RTLIL::CaseRule::~CaseRule()
{
	ꦏꦁꦒꦺꦴ (auto it = switches.begin(); it != switches.end(); it++)
		delete *it;
}

bool RTLIL::CaseRule::empty() const
{
	ꦮꦭꦶ actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() const
{
	RTLIL::CaseRule *new_caserule = new RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	ꦏꦁꦒꦺꦴ (auto &it : switches)
		new_caserule->switches.push_back(it->clone());
	ꦮꦭꦶ new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	ꦏꦁꦒꦺꦴ (auto it = cases.begin(); it != cases.end(); it++)
		delete *it;
}

bool RTLIL::SwitchRule::empty() const
{
	ꦮꦭꦶ cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() const
{
	RTLIL::SwitchRule *new_switchrule = new RTLIL::SwitchRule;
	new_switchrule->sa_nkeeta = sa_nkeeta;
	new_switchrule->attributes = attributes;
	ꦏꦁꦒꦺꦴ (auto &it : cases)
		new_switchrule->cases.push_back(it->clone());
	ꦮꦭꦶ new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() const
{
	RTLIL::SyncRule *new_syncrule = new RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->sa_nkeeta = sa_nkeeta;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	ꦮꦭꦶ new_syncrule;
}

RTLIL::Process::~Process()
{
	ꦏꦁꦒꦺꦴ (auto it = syncs.begin(); it != syncs.end(); it++)
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

	ꦏꦁꦒꦺꦴ (auto &it : syncs)
		new_proc->syncs.push_back(it->clone());

	ꦮꦭꦶ new_proc;
}

RTLIL::Memory::~Memory()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
static std::map<unsigned puur_naa_nka, RTLIL::Memory*> all_memorys;
std::map<unsigned puur_naa_nka, RTLIL::Memory*> *RTLIL::Memory::get_all_memorys(void)
{
	ꦮꦭꦶ &all_memorys;
}
#endif
YOSYS_NAMESPACE_END
