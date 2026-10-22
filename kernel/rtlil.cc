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

#അന്തര്ഭാവ "kernel/rtlil.h"
#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "kernel/macc.h"
#അന്തര്ഭാവ "kernel/newcelltypes.h"
#അന്തര്ഭാവ "kernel/sigtools.h"
#അന്തര്ഭാവ "kernel/threading.h"
#അന്തര്ഭാവ "frontends/verilog/verilog_frontend.h"
#അന്തര്ഭാവ "frontends/verilog/preproc.h"
#അന്തര്ഭാവ "backends/rtlil/rtlil_backend.h"

#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <algorithm>
#അന്തര്ഭാവ <charconv>
#അന്തര്ഭാവ <optional>
#അന്തര്ഭാവ <string_view>
#അന്തര്ഭാവ <sstream>

YOSYS_NAMESPACE_BEGIN

dict<std::string, std::string> RTLIL::constpad;

സ്ഥിരം pool<IdString> &RTLIL::builtin_ff_cell_types() {
	സ്ഥിതം സ്ഥിരം pool<IdString> res = []() {
		pool<IdString> r;
		വേണ്ടി (size_t i = 0; i < StaticCellTypes::builder.എണ്ണം; i++) {
			auto &cell = StaticCellTypes::builder.cells[i];
			എങ്കിൽ (cell.features.is_ff)
				r.തിരുകുക(cell.type);
		}
		തിരികെ r;
	}();
	തിരികെ res;
}

#define check(condition) log_assert(condition && "malformed Const union")

സ്ഥിരം Const::bitvectype& Const::get_bits() സ്ഥിരം {
	check(is_bits());
	തിരികെ *get_if_bits();
}

സ്ഥിരം std::string& Const::get_str() സ്ഥിരം {
	check(is_str());
	തിരികെ *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	തിരികെ *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	തിരികെ *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	പുതിയ ((ശൂന്യം*)&str_) std::string(std::move(str));
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	അക്ഷരം bytes[] = {
		(അക്ഷരം)(val >> 24), (അക്ഷരം)(val >> 16), (അക്ഷരം)(val >> 8), (അക്ഷരം)val
	};
	പുതിയ ((ശൂന്യം*)&str_) std::string(bytes, 4);
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val, പൂർണ്ണസംഖ്യ width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	എങ്കിൽ ((width & 7) == 0) {
		പുതിയ ((ശൂന്യം*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		പൂർണ്ണസംഖ്യ bytes = width >> 3;
		signed അക്ഷരം sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<പൂർണ്ണസംഖ്യ>(bytes, വലുപ്പം(val));
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < bytes; i++) {
			str[str.size() - 1 - i] = val;
			val = val >> 8;
		}
		തിരികെ;
	}

	പുതിയ ((ശൂന്യം*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++) {
		bv.തള്ളുക_back((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, പൂർണ്ണസംഖ്യ width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	പുതിയ ((ശൂന്യം*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++)
		bv.തള്ളുക_back(bit);
}

RTLIL::Const::Const(സ്ഥിരം std::vector<തർക്കം> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	പുതിയ ((ശൂന്യം*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.size());
	വേണ്ടി (സ്ഥിരം auto &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(സ്ഥിരം RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	എങ്കിൽ (is_str())
		പുതിയ ((ശൂന്യം*)&str_) std::string(other.get_str());
	അല്ലെങ്കിൽ എങ്കിൽ (is_bits())
		പുതിയ ((ശൂന്യം*)&bits_) bitvectype(other.get_bits());
	അല്ലെങ്കിൽ
		check(അസത്യം);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	എങ്കിൽ (is_str())
		പുതിയ ((ശൂന്യം*)&str_) std::string(std::move(other.get_str()));
	അല്ലെങ്കിൽ എങ്കിൽ (is_bits())
		പുതിയ ((ശൂന്യം*)&bits_) bitvectype(std::move(other.get_bits()));
	അല്ലെങ്കിൽ
		check(അസത്യം);
}

RTLIL::Const &RTLIL::Const::operator =(സ്ഥിരം RTLIL::Const &other) {
	flags = other.flags;
	എങ്കിൽ (other.is_str()) {
		എങ്കിൽ (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(ശൂന്യം)പുതിയ ((ശൂന്യം*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} അല്ലെങ്കിൽ എങ്കിൽ (other.is_bits()) {
		എങ്കിൽ (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(ശൂന്യം)പുതിയ ((ശൂന്യം*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} അല്ലെങ്കിൽ {
		check(അസത്യം);
	}
	തിരികെ *ഇത്;
}

RTLIL::Const::~Const() {
	എങ്കിൽ (is_bits())
		bits_.~bitvectype();
	അല്ലെങ്കിൽ എങ്കിൽ (is_str())
		str_.~string();
	അല്ലെങ്കിൽ
		check(അസത്യം);
}

തർക്കം RTLIL::Const::operator<(സ്ഥിരം RTLIL::Const &other) സ്ഥിരം
{
	എങ്കിൽ (size() != other.size())
		തിരികെ size() < other.size();

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < size(); i++)
		എങ്കിൽ ((*ഇത്)[i] != other[i])
			തിരികെ (*ഇത്)[i] < other[i];

	തിരികെ അസത്യം;
}

തർക്കം RTLIL::Const::operator ==(സ്ഥിരം RTLIL::Const &other) സ്ഥിരം
{
	എങ്കിൽ (is_str() && other.is_str())
		തിരികെ get_str() == other.get_str();
	എങ്കിൽ (is_bits() && other.is_bits())
		തിരികെ get_bits() == other.get_bits();

	എങ്കിൽ (size() != other.size())
		തിരികെ അസത്യം;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < size(); i++)
	എങ്കിൽ ((*ഇത്)[i] != other[i])
		തിരികെ അസത്യം;

	തിരികെ സത്യം;
}

തർക്കം RTLIL::Const::operator !=(സ്ഥിരം RTLIL::Const &other) സ്ഥിരം
{
	തിരികെ !(*ഇത് == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	തിരികെ get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() സ്ഥിരം
{
	std::vector<State> v;
	വേണ്ടി (auto bit : *ഇത്)
		v.തള്ളുക_back(bit);
	തിരികെ v;
}

തർക്കം RTLIL::Const::as_bool() സ്ഥിരം
{
	എങ്കിൽ (is_str()) {
		വേണ്ടി (അക്ഷരം ch : get_str())
			എങ്കിൽ (ch != 0)
				തിരികെ സത്യം;
		തിരികെ അസത്യം;
	}

	സ്ഥിരം bitvectype& bv = get_bits();
	വേണ്ടി (size_t i = 0; i < bv.size(); i++)
		എങ്കിൽ (bv[i] == State::S1)
			തിരികെ സത്യം;
	തിരികെ അസത്യം;
}

പൂർണ്ണസംഖ്യ RTLIL::Const::as_int(തർക്കം is_signed) സ്ഥിരം
{
	int32_t ret = 0;
	എങ്കിൽ (സ്ഥിരം std::string *s = get_if_str()) {
		പൂർണ്ണസംഖ്യ size = GetSize(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = std::min(4, size); i > 0; i--)
			ret |= static_cast<unsigned അക്ഷരം>((*s)[size - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		എങ്കിൽ (is_signed && size > 0 && size < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << size*8;
		തിരികെ ret;
	}

	സ്ഥിരം bitvectype& bv = get_bits();
	പൂർണ്ണസംഖ്യ significant_bits = std::min(GetSize(bv), 32);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < significant_bits; i++)
		എങ്കിൽ (bv[i] == State::S1)
			ret |= 1 << i;
	എങ്കിൽ (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	തിരികെ ret;
}

തർക്കം RTLIL::Const::convertible_to_int(തർക്കം is_signed) സ്ഥിരം
{
	auto size = get_min_size(is_signed);

	എങ്കിൽ (size < 0)
		തിരികെ അസത്യം;

	// If it fits in 31 bits it is definitely convertible
	എങ്കിൽ (size <= 31)
		തിരികെ സത്യം;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	എങ്കിൽ (size == 32) {
		എങ്കിൽ (is_signed)
			തിരികെ സത്യം;
		തിരികെ (*ഇത്)[size - 1] != State::S1;
	}

	തിരികെ അസത്യം;
}

std::optional<പൂർണ്ണസംഖ്യ> RTLIL::Const::try_as_int(തർക്കം is_signed) സ്ഥിരം
{
	എങ്കിൽ (!convertible_to_int(is_signed))
		തിരികെ std::nullopt;
	തിരികെ as_int(is_signed);
}

പൂർണ്ണസംഖ്യ RTLIL::Const::as_int_saturating(തർക്കം is_signed) സ്ഥിരം
{
	എങ്കിൽ (!convertible_to_int(is_signed)) {
		എങ്കിൽ (!is_signed)
			തിരികെ std::numeric_limits<പൂർണ്ണസംഖ്യ>::max();

		സ്ഥിരം auto min_size = get_min_size(is_signed);
		log_assert(min_size > 0);
		സ്ഥിരം auto neg = (*ഇത്)[min_size - 1];
		തിരികെ neg ? std::numeric_limits<പൂർണ്ണസംഖ്യ>::min() : std::numeric_limits<പൂർണ്ണസംഖ്യ>::max();
	}
	തിരികെ as_int(is_signed);
}

ശൂന്യം RTLIL::Const::tag_bare_integer_const(സ്ഥിരം std::string &മൂല്യം)
{
	എങ്കിൽ (മൂല്യം.empty() || മൂല്യം.find('\'') != std::string::npos)
		തിരികെ;
	size_t start = (മൂല്യം[0] == '-' || മൂല്യം[0] == '+') ? 1 : 0;
	എങ്കിൽ (start < മൂല്യം.size() && std::all_of(മൂല്യം.begin() + start, മൂല്യം.end(), ::isdigit))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

പൂർണ്ണസംഖ്യ RTLIL::Const::get_min_size(തർക്കം is_signed) സ്ഥിരം
{
	എങ്കിൽ (empty()) തിരികെ 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	എങ്കിൽ (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	അല്ലെങ്കിൽ
		leading_bit = RTLIL::State::S0;

	auto idx = size();
	ആകുമ്പോൾ (idx > 0 && (*ഇത്)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	എങ്കിൽ (is_signed && idx < size()) {
		idx++;
	}
	// must be at least one bit
	തിരികെ (idx == 0) ? 1 : idx;
}

ശൂന്യം RTLIL::Const::compress(തർക്കം is_signed)
{
	auto idx = get_min_size(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<പൂർണ്ണസംഖ്യ> RTLIL::Const::as_int_compress(തർക്കം is_signed) സ്ഥിരം
{
	തിരികെ try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(സ്ഥിരം അക്ഷരം* any) സ്ഥിരം
{
	പൂർണ്ണസംഖ്യ sz = size();
	std::string ret;
	ret.reserve(sz);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = sz - 1; i >= 0; --i)
		തിരഞ്ഞെടുപ്പ് ((*ഇത്)[i]) {
			അവസ്ഥ S0: ret.തള്ളുക_back('0'); നിർത്തുക;
			അവസ്ഥ S1: ret.തള്ളുക_back('1'); നിർത്തുക;
			അവസ്ഥ Sx: ret.തള്ളുക_back('x'); നിർത്തുക;
			അവസ്ഥ Sz: ret.തള്ളുക_back('z'); നിർത്തുക;
			അവസ്ഥ Sa: ret += any; നിർത്തുക;
			അവസ്ഥ Sm: ret.തള്ളുക_back('m'); നിർത്തുക;
		}
	തിരികെ ret;
}

RTLIL::Const RTLIL::Const::from_string(സ്ഥിരം std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.size());
	വേണ്ടി (auto it = str.rbegin(); it != str.rend(); it++)
		തിരഞ്ഞെടുപ്പ് (*it) {
			അവസ്ഥ '0': bv.തള്ളുക_back(State::S0); നിർത്തുക;
			അവസ്ഥ '1': bv.തള്ളുക_back(State::S1); നിർത്തുക;
			അവസ്ഥ 'x': bv.തള്ളുക_back(State::Sx); നിർത്തുക;
			അവസ്ഥ 'z': bv.തള്ളുക_back(State::Sz); നിർത്തുക;
			അവസ്ഥ 'm': bv.തള്ളുക_back(State::Sm); നിർത്തുക;
			സ്ഥിരസ്ഥിതി: bv.തള്ളുക_back(State::Sa);
		}
	തിരികെ c;
}

std::string RTLIL::Const::decode_string() സ്ഥിരം
{
	എങ്കിൽ (auto str = get_if_str())
		തിരികെ *str;

	സ്ഥിരം bitvectype& bv = get_bits();
	സ്ഥിരം പൂർണ്ണസംഖ്യ n = GetSize(bv);
	സ്ഥിരം പൂർണ്ണസംഖ്യ n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	പൂർണ്ണസംഖ്യ i = n_over_8 * 8;
	എങ്കിൽ (i < n) {
		അക്ഷരം ch = 0;
		വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < (n - i); j++) {
			എങ്കിൽ (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		എങ്കിൽ (ch != 0)
			s.append({ch});
	}
	i -= 8;
	വേണ്ടി (; i >= 0; i -= 8) {
		അക്ഷരം ch = 0;
		വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < 8; j++) {
			എങ്കിൽ (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		എങ്കിൽ (ch != 0)
			s.append({ch});
	}
	തിരികെ s;
}

പൂർണ്ണസംഖ്യ RTLIL::Const::size() സ്ഥിരം {
	എങ്കിൽ (is_str())
		തിരികെ 8 * str_.size();
	അല്ലെങ്കിൽ {
		check(is_bits());
		തിരികെ bits_.size();
	}
}

തർക്കം RTLIL::Const::empty() സ്ഥിരം {
	എങ്കിൽ (is_str())
		തിരികെ str_.empty();
	അല്ലെങ്കിൽ {
		check(is_bits());
		തിരികെ bits_.empty();
	}
}

ശൂന്യം RTLIL::Const::bitvectorize_internal() {
	എങ്കിൽ (tag == backing_tag::bits)
		തിരികെ;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.size() * 8);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = str_.size() - 1; i >= 0; i--) {
		unsigned അക്ഷരം ch = str_[i];
		വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < 8; j++) {
			new_bits.തള്ളുക_back((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(ശൂന്യം)പുതിയ ((ശൂന്യം*)&bits_) bitvectype(std::move(new_bits));
		tag = backing_tag::bits;
	}
}

ശൂന്യം RTLIL::Const::append(സ്ഥിരം RTLIL::Const &other) {
	bitvectorize_internal();
	bitvectype& bv = get_bits();
	bv.തിരുകുക(bv.end(), other.begin(), other.end());
}

RTLIL::State RTLIL::Const::const_iterator::operator*() സ്ഥിരം {
	എങ്കിൽ (auto bv = parent->get_if_bits())
		തിരികെ (*bv)[idx];

	പൂർണ്ണസംഖ്യ char_idx = parent->get_str().size() - idx / 8 - 1;
	തർക്കം bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	തിരികെ bit ? State::S1 : State::S0;
}

തർക്കം RTLIL::Const::is_fully_zero() സ്ഥിരം
{
	എങ്കിൽ (auto str = get_if_str()) {
		വേണ്ടി (അക്ഷരം ch : *str)
			എങ്കിൽ (ch != 0)
				തിരികെ അസത്യം;
		തിരികെ സത്യം;
	}

	സ്ഥിരം bitvectype& bv = get_bits();

	വേണ്ടി (സ്ഥിരം auto &bit : bv)
		എങ്കിൽ (bit != RTLIL::State::S0)
			തിരികെ അസത്യം;

	തിരികെ സത്യം;
}

തർക്കം RTLIL::Const::is_fully_ones() സ്ഥിരം
{
	എങ്കിൽ (auto str = get_if_str()) {
		വേണ്ടി (അക്ഷരം ch : *str)
			എങ്കിൽ (ch != (അക്ഷരം)0xff)
				തിരികെ അസത്യം;
		തിരികെ സത്യം;
	}

	സ്ഥിരം bitvectype& bv = get_bits();
	വേണ്ടി (സ്ഥിരം auto &bit : bv)
		എങ്കിൽ (bit != RTLIL::State::S1)
			തിരികെ അസത്യം;

	തിരികെ സത്യം;
}

തർക്കം RTLIL::Const::is_fully_def() സ്ഥിരം
{
	എങ്കിൽ (is_str())
		തിരികെ സത്യം;

	സ്ഥിരം bitvectype& bv = get_bits();
	വേണ്ടി (സ്ഥിരം auto &bit : bv)
		എങ്കിൽ (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			തിരികെ അസത്യം;

	തിരികെ സത്യം;
}

തർക്കം RTLIL::Const::is_fully_undef() സ്ഥിരം
{
	എങ്കിൽ (auto str = get_if_str())
		തിരികെ str->empty();

	സ്ഥിരം bitvectype& bv = get_bits();
	വേണ്ടി (സ്ഥിരം auto &bit : bv)
		എങ്കിൽ (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			തിരികെ അസത്യം;

	തിരികെ സത്യം;
}

തർക്കം RTLIL::Const::is_fully_undef_x_only() സ്ഥിരം
{
	എങ്കിൽ (auto str = get_if_str())
		തിരികെ str->empty();

	സ്ഥിരം bitvectype& bv = get_bits();
	വേണ്ടി (സ്ഥിരം auto &bit : bv)
		എങ്കിൽ (bit != RTLIL::State::Sx)
			തിരികെ അസത്യം;

	തിരികെ സത്യം;
}

തർക്കം RTLIL::Const::is_onehot(പൂർണ്ണസംഖ്യ *pos) സ്ഥിരം
{
	തർക്കം found = അസത്യം;
	പൂർണ്ണസംഖ്യ size = GetSize(*ഇത്);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < size; i++) {
		State bit = (*ഇത്)[i];
		എങ്കിൽ (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			തിരികെ അസത്യം;
		എങ്കിൽ (bit == RTLIL::State::S1) {
			എങ്കിൽ (found)
				തിരികെ അസത്യം;
			എങ്കിൽ (pos)
				*pos = i;
			found = സത്യം;
		}
	}
	തിരികെ found;
}

Hasher RTLIL::Const::hash_into(Hasher h) സ്ഥിരം
{
	എങ്കിൽ (auto str = get_if_str())
		തിരികെ hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	സ്ഥിരം bitvectype &bv = get_bits();
	പൂർണ്ണസംഖ്യ size = GetSize(bv);
	std::string packed;
	പൂർണ്ണസംഖ്യ packed_size = (size + 7) >> 3;
	packed.resize(packed_size, 0);
	വേണ്ടി (പൂർണ്ണസംഖ്യ bi = 0; bi < packed_size; ++bi) {
		അക്ഷരം ch = 0;
		പൂർണ്ണസംഖ്യ end = std::min((bi + 1)*8, size);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			എങ്കിൽ (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<അക്ഷരം>::hash_into(ch, h);
				വേണ്ടി (; i < size; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(size);
				തിരികെ h;
			}
			ch |= static_cast<പൂർണ്ണസംഖ്യ>(b) << (i & 7);
		}
		packed[packed_size - 1 - bi] = ch;
	}
	തിരികെ hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ len, RTLIL::State padding) സ്ഥിരം {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = offset; i < offset + len; i++)
		ret_bv.തള്ളുക_back(i < GetSize(*ഇത്) ? (*ഇത്)[i] : padding);
	തിരികെ RTLIL::Const(ret_bv);
}
#undef check /* check(condition) for Const */

തർക്കം RTLIL::AttrObject::has_attribute(RTLIL::IdString id) സ്ഥിരം
{
	തിരികെ attributes.എണ്ണം(id);
}

ശൂന്യം RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, തർക്കം മൂല്യം)
{
	എങ്കിൽ (മൂല്യം)
		attributes[id] = RTLIL::Const(1);
	അല്ലെങ്കിൽ
		attributes.erase(id);
}

തർക്കം RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) സ്ഥിരം
{
	സ്ഥിരം auto it = attributes.find(id);
	എങ്കിൽ (it == attributes.end())
		തിരികെ അസത്യം;
	തിരികെ it->second.as_bool();
}

ശൂന്യം RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string മൂല്യം)
{
	എങ്കിൽ (മൂല്യം.empty())
		attributes.erase(id);
	അല്ലെങ്കിൽ
		attributes[id] = മൂല്യം;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) സ്ഥിരം
{
	std::string മൂല്യം;
	സ്ഥിരം auto it = attributes.find(id);
	എങ്കിൽ (it != attributes.end())
		മൂല്യം = it->second.decode_string();
	തിരികെ മൂല്യം;
}

ശൂന്യം RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, സ്ഥിരം pool<string> &data)
{
	string attrval;
	വേണ്ടി (സ്ഥിരം auto &s : data) {
		എങ്കിൽ (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

ശൂന്യം RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, സ്ഥിരം pool<string> &data)
{
	pool<string> union_data = get_strpool_attribute(id);
	union_data.തിരുകുക(data.begin(), data.end());
	എങ്കിൽ (!union_data.empty())
		set_strpool_attribute(id, union_data);
}

ശൂന്യം RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->attributes = std::move(buf);
}

നാമാകാശ {
	template<typename F>
	ശൂന്യം walk_process_attr_objects(RTLIL::Process *process, F visit) {
		visit(process);
		std::vector<RTLIL::CaseRule*> case_അട്ടി{&process->വേര്_case};
		ആകുമ്പോൾ (!case_അട്ടി.empty()) {
			RTLIL::CaseRule *cs = case_അട്ടി.back();
			case_അട്ടി.എടുക്കുക_back();
			visit(cs);
			വേണ്ടി (auto *sw : cs->switches) {
				visit(sw);
				വേണ്ടി (auto *case_ : sw->cases)
					case_അട്ടി.തള്ളുക_back(case_);
			}
		}
		വേണ്ടി (auto *സമന്വയം : process->syncs)
			വേണ്ടി (auto &mwa : സമന്വയം->mem_write_actions)
				visit(&mwa);
	}

	template<typename F>
	ശൂന്യം walk_attr_objects(RTLIL::Design *design, F visit) {
		വേണ്ടി (auto &[_, module] : design->modules_) {
			visit(module);
			വേണ്ടി (auto &[_, wire] : module->wires_)
				visit(wire);
			വേണ്ടി (auto &[_, mem] : module->memories)
				visit(mem);
			വേണ്ടി (auto &[_, cell] : module->cells_)
				visit(cell);
			വേണ്ടി (auto &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


size_t RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	auto വേര് = [&](IdString ref) {
		എങ്കിൽ (ref != IdString::Null)
			live.തിരുകുക(ref);
	};

	walk_attr_objects(ഇത്, [&](സ്ഥിരം RTLIL::AttrObject *obj) {
		വേണ്ടി (auto &attr : obj->attributes)
			വേര്(attr.first);
	});

	വേര്(selected_active_module);

	വേണ്ടി (auto &[name, module] : modules_) {
		വേര്(name);
		വേണ്ടി (IdString port : module->ports)
			വേര്(port);
		വേണ്ടി (IdString param : module->avail_parameters)
			വേര്(param);
		വേണ്ടി (auto &[param, _] : module->parameter_default_മൂല്യങ്ങൾ)
			വേര്(param);
		വേണ്ടി (auto &[name, _] : module->memories)
			വേര്(name);
		വേണ്ടി (auto &[name, _] : module->processes)
			വേര്(name);
		വേണ്ടി (auto &[name, wire] : module->wires_) {
			വേര്(name);
			എങ്കിൽ (wire->known_driver())
				വേര്(wire->driverPort());
		}
		വേണ്ടി (auto &[name, cell] : module->cells_) {
			വേര്(name);
			വേര്(cell->type);
			വേണ്ടി (auto &conn : cell->connections())
				വേര്(conn.first);
			വേണ്ടി (auto &param : cell->parameters)
				വേര്(param.first);
		}
		വേണ്ടി (auto &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_അട്ടി{&process->വേര്_case};
			ആകുമ്പോൾ (!case_അട്ടി.empty()) {
				RTLIL::CaseRule *cs = case_അട്ടി.back();
				case_അട്ടി.എടുക്കുക_back();
				വേണ്ടി (auto *sw : cs->switches) {
					വേണ്ടി (auto *case_ : sw->cases)
						case_അട്ടി.തള്ളുക_back(case_);
				}
			}
			വേണ്ടി (auto *സമന്വയം : process->syncs) {
				വേണ്ടി (auto &mwa : സമന്വയം->mem_write_actions)
					വേര്(mwa.memid);
			}
		}
	}

	auto വേര്_selection = [&](സ്ഥിരം RTLIL::Selection &sel) {
		വേണ്ടി (IdString m : sel.selected_modules)
			വേര്(m);
		വേണ്ടി (auto &[m, members] : sel.selected_members) {
			വേര്(m);
			വേണ്ടി (IdString member : members)
				വേര്(member);
		}
	};

	വേണ്ടി (auto &[name, sel] : selection_vars) {
		വേര്(name);
		വേര്_selection(sel);
	}

	വേണ്ടി (auto &sel : selection_അട്ടി)
		വേര്_selection(sel);

	size_t erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_എണ്ണം;
	തിരികെ erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) സ്ഥിരം
{
	pool<string> data;
	എങ്കിൽ (attributes.എണ്ണം(id) != 0)
		വേണ്ടി (auto s : split_tokens(get_string_attribute(id), "|"))
			data.തിരുകുക(s);
	തിരികെ data;
}

ശൂന്യം RTLIL::AttrObject::set_hdlname_attribute(സ്ഥിരം vector<string> &hierarchy)
{
	string attrval;
	വേണ്ടി (സ്ഥിരം auto &ident : hierarchy) {
		എങ്കിൽ (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() സ്ഥിരം
{
	തിരികെ split_tokens(get_string_attribute(ID::hdlname), " ");
}

ശൂന്യം RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, സ്ഥിരം vector<പൂർണ്ണസംഖ്യ> &data)
{
	std::stringstream attrval;
	വേണ്ടി (auto &i : data) {
		എങ്കിൽ (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<പൂർണ്ണസംഖ്യ> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) സ്ഥിരം
{
	vector<പൂർണ്ണസംഖ്യ> data;
	auto it = attributes.find(id);
	എങ്കിൽ (it != attributes.end())
		വേണ്ടി (സ്ഥിരം auto &s : split_tokens(attributes.at(id).decode_string())) {
			അക്ഷരം *end = nullptr;
			errno = 0;
			long മൂല്യം = strtol(s.c_str(), &end, 10);
			എങ്കിൽ (end != s.c_str() + s.size())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			എങ്കിൽ (errno == ERANGE || മൂല്യം < INT_MIN || മൂല്യം > INT_MAX)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			data.തള്ളുക_back(മൂല്യം);
		}
	തിരികെ data;
}

തർക്കം RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) സ്ഥിരം
{
	എങ്കിൽ (current_design != nullptr) {
		auto module = current_design->module(mod_name);
		തിരികെ module && module->get_blackbox_attribute();
	} അല്ലെങ്കിൽ {
		log_warning("Unable to check if module is boxed for null design.\n");
		തിരികെ അസത്യം;
	}
}

തർക്കം RTLIL::Selection::selected_module(RTLIL::IdString mod_name) സ്ഥിരം
{
	എങ്കിൽ (complete_selection)
		തിരികെ സത്യം;
	എങ്കിൽ (!selects_boxes && boxed_module(mod_name))
		തിരികെ അസത്യം;
	എങ്കിൽ (full_selection)
		തിരികെ സത്യം;
	എങ്കിൽ (selected_modules.എണ്ണം(mod_name) > 0)
		തിരികെ സത്യം;
	എങ്കിൽ (selected_members.എണ്ണം(mod_name) > 0)
		തിരികെ സത്യം;
	തിരികെ അസത്യം;
}

തർക്കം RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) സ്ഥിരം
{
	എങ്കിൽ (complete_selection)
		തിരികെ സത്യം;
	എങ്കിൽ (!selects_boxes && boxed_module(mod_name))
		തിരികെ അസത്യം;
	എങ്കിൽ (full_selection)
		തിരികെ സത്യം;
	എങ്കിൽ (selected_modules.എണ്ണം(mod_name) > 0)
		തിരികെ സത്യം;
	തിരികെ അസത്യം;
}

തർക്കം RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) സ്ഥിരം
{
	എങ്കിൽ (complete_selection)
		തിരികെ സത്യം;
	എങ്കിൽ (!selects_boxes && boxed_module(mod_name))
		തിരികെ അസത്യം;
	എങ്കിൽ (full_selection)
		തിരികെ സത്യം;
	എങ്കിൽ (selected_modules.എണ്ണം(mod_name) > 0)
		തിരികെ സത്യം;
	എങ്കിൽ (selected_members.എണ്ണം(mod_name) > 0)
		എങ്കിൽ (selected_members.at(mod_name).എണ്ണം(memb_name) > 0)
			തിരികെ സത്യം;
	തിരികെ അസത്യം;
}

ശൂന്യം RTLIL::Selection::optimize(RTLIL::Design *design)
{
	എങ്കിൽ (design != current_design) {
		current_design = design;
	}

	എങ്കിൽ (selects_boxes && full_selection)
		complete_selection = സത്യം;
	എങ്കിൽ (complete_selection) {
		full_selection = അസത്യം;
		selects_boxes = സത്യം;
	}
	എങ്കിൽ (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		തിരികെ;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.clear();
	വേണ്ടി (auto mod_name : selected_modules) {
		എങ്കിൽ (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.തള്ളുക_back(mod_name);
		selected_members.erase(mod_name);
	}
	വേണ്ടി (auto mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	വേണ്ടി (auto &it : selected_members)
		എങ്കിൽ (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.തള്ളുക_back(it.first);
	വേണ്ടി (auto mod_name : del_list)
		selected_members.erase(mod_name);

	വേണ്ടി (auto &it : selected_members) {
		del_list.clear();
		വേണ്ടി (auto memb_name : it.second)
			എങ്കിൽ (current_design->module(it.first)->എണ്ണം_id(memb_name) == 0)
				del_list.തള്ളുക_back(memb_name);
		വേണ്ടി (auto memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	add_list.clear();
	വേണ്ടി (auto &it : selected_members)
		എങ്കിൽ (it.second.size() == 0)
			del_list.തള്ളുക_back(it.first);
		അല്ലെങ്കിൽ എങ്കിൽ (it.second.size() == current_design->module(it.first)->wires_.size() + current_design->module(it.first)->memories.size() +
				current_design->module(it.first)->cells_.size() + current_design->module(it.first)->processes.size())
			add_list.തള്ളുക_back(it.first);
	വേണ്ടി (auto mod_name : del_list)
		selected_members.erase(mod_name);
	വേണ്ടി (auto mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.തിരുകുക(mod_name);
	}

	എങ്കിൽ (selected_modules.size() == current_design->modules_.size()) {
		selected_modules.clear();
		selected_members.clear();
		എങ്കിൽ (selects_boxes)
			complete_selection = സത്യം;
		അല്ലെങ്കിൽ
			full_selection = സത്യം;
	}
}

ശൂന്യം RTLIL::Selection::clear()
{
	full_selection = അസത്യം;
	complete_selection = അസത്യം;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_defines (പുതിയ define_map_t)
{
	സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ hashidx_എണ്ണം = 123456789;
	hashidx_എണ്ണം = mkhash_xorshift(hashidx_എണ്ണം);
	hashidx_ = hashidx_എണ്ണം;

	refcount_modules_ = 0;
	selected_active_module.clear();
	തള്ളുക_full_selection();

	RTLIL::Design::get_all_designs()->തിരുകുക(std::pair<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Design*>(hashidx_, ഇത്));
}

RTLIL::Design::~Design()
{
	വേണ്ടി (auto &pr : modules_)
		delete pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

സ്ഥിതം std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Design*> all_designs;
std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Design*> *RTLIL::Design::get_all_designs(ശൂന്യം)
{
	തിരികെ &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	തിരികെ RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

സ്ഥിരം RTLIL::Module *RTLIL::Design::module(IdString id) സ്ഥിരം {
	twines_.check_owned(id);
	തിരികെ modules_.എണ്ണം(id) ? modules_.at(id) : ശൂന്യ;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	തിരികെ modules_.എണ്ണം(id) ? modules_.at(id) : ശൂന്യ;
}

RTLIL::Module *RTLIL::Design::മുകൾ_module() സ്ഥിരം
{
	RTLIL::Module *module = nullptr;
	പൂർണ്ണസംഖ്യ module_എണ്ണം = 0;

	വേണ്ടി (auto mod : selected_modules()) {
		എങ്കിൽ (mod->get_bool_attribute(ID::മുകൾ))
			തിരികെ mod;
		module_എണ്ണം++;
		module = mod;
	}

	തിരികെ module_എണ്ണം == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	എങ്കിൽ (modules_.എണ്ണം(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = പുതിയ RTLIL::Module;
	modules_[name] = module;
	module->design = ഇത്;
	module->name = name;

	വേണ്ടി (auto mon : monitors)
		mon->notify_module_add(module);

	എങ്കിൽ (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	തിരികെ module;
}

ശൂന്യം RTLIL::Design::scratchpad_unset(സ്ഥിരം std::string &varname)
{
	scratchpad.erase(varname);
}

ശൂന്യം RTLIL::Design::scratchpad_set_int(സ്ഥിരം std::string &varname, പൂർണ്ണസംഖ്യ മൂല്യം)
{
	scratchpad[varname] = stringf("%d", മൂല്യം);
}

ശൂന്യം RTLIL::Design::scratchpad_set_bool(സ്ഥിരം std::string &varname, തർക്കം മൂല്യം)
{
	scratchpad[varname] = മൂല്യം ? "true" : "false";
}

ശൂന്യം RTLIL::Design::scratchpad_set_string(സ്ഥിരം std::string &varname, std::string മൂല്യം)
{
	scratchpad[varname] = std::move(മൂല്യം);
}

പൂർണ്ണസംഖ്യ RTLIL::Design::scratchpad_get_int(സ്ഥിരം std::string &varname, പൂർണ്ണസംഖ്യ default_മൂല്യം_2) സ്ഥിരം
{
	auto it = scratchpad.find(varname);
	എങ്കിൽ (it == scratchpad.end())
		തിരികെ default_മൂല്യം_2;

	സ്ഥിരം std::string &str = it->second;

	എങ്കിൽ (str == "0" || str == "false")
		തിരികെ 0;

	എങ്കിൽ (str == "1" || str == "true")
		തിരികെ 1;

	അക്ഷരം *endptr = nullptr;
	long പൂർണ്ണസംഖ്യ parsed_മൂല്യം = strtol(str.c_str(), &endptr, 10);
	തിരികെ *endptr ? default_മൂല്യം_2 : parsed_മൂല്യം;
}

തർക്കം RTLIL::Design::scratchpad_get_bool(സ്ഥിരം std::string &varname, തർക്കം default_മൂല്യം_2) സ്ഥിരം
{
	auto it = scratchpad.find(varname);
	എങ്കിൽ (it == scratchpad.end())
		തിരികെ default_മൂല്യം_2;

	സ്ഥിരം std::string &str = it->second;

	എങ്കിൽ (str == "0" || str == "false")
		തിരികെ അസത്യം;

	എങ്കിൽ (str == "1" || str == "true")
		തിരികെ സത്യം;

	തിരികെ default_മൂല്യം_2;
}

std::string RTLIL::Design::scratchpad_get_string(സ്ഥിരം std::string &varname, സ്ഥിരം std::string &default_മൂല്യം_2) സ്ഥിരം
{
	auto it = scratchpad.find(varname);
	എങ്കിൽ (it == scratchpad.end())
		തിരികെ default_മൂല്യം_2;

	തിരികെ it->second;
}

ശൂന്യം RTLIL::Design::remove(RTLIL::Module *module)
{
	വേണ്ടി (auto mon : monitors)
		mon->notify_module_del(module);

	എങ്കിൽ (yosys_xtrace) {
		log("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	delete module;
}

ശൂന്യം RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	add(module);
}

ശൂന്യം RTLIL::Design::sort()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
	വേണ്ടി (auto &it : modules_)
		it.second->sort();
}

ശൂന്യം RTLIL::Design::sort_modules()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
}

ശൂന്യം check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

ശൂന്യം RTLIL::Design::check()
{
#ifndef NDEBUG
	log_assert(!selection_അട്ടി.empty());
	പൂർണ്ണസംഖ്യ pool_size = 0;
	വേണ്ടി (auto &it : modules_)
		pool_size = std::max(pool_size, ThreadPool::work_pool_size(0, it.second->cells_size(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_size);
	വേണ്ടി (auto &it : modules_) {
		log_assert(ഇത് == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#endif
}

ശൂന്യം RTLIL::Design::optimize()
{
	വേണ്ടി (auto &it : modules_)
		it.second->optimize();
	വേണ്ടി (auto &it : selection_അട്ടി)
		it.optimize(ഇത്);
	വേണ്ടി (auto &it : selection_vars)
		it.second.optimize(ഇത്);
}

ശൂന്യം RTLIL::Design::clone_into(RTLIL::Design *dst) സ്ഥിരം
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	വേണ്ടി (auto it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

തർക്കം RTLIL::Design::selected_module(IdString mod_name) സ്ഥിരം
{
	എങ്കിൽ (!selected_active_module.empty() && mod_name != selected_active_module)
		തിരികെ അസത്യം;
	തിരികെ selection().selected_module(mod_name);
}

തർക്കം RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) സ്ഥിരം
{
	എങ്കിൽ (!selected_active_module.empty() && mod_name != selected_active_module)
		തിരികെ അസത്യം;
	തിരികെ selection().selected_whole_module(mod_name);
}

തർക്കം RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) സ്ഥിരം
{
	എങ്കിൽ (!selected_active_module.empty() && mod_name != selected_active_module)
		തിരികെ അസത്യം;
	തിരികെ selection().selected_member(mod_name, memb_name);
}

തർക്കം RTLIL::Design::selected_module(RTLIL::Module *mod) സ്ഥിരം
{
	തിരികെ selected_module(mod->name);
}

തർക്കം RTLIL::Design::selected_whole_module(RTLIL::Module *mod) സ്ഥിരം
{
	തിരികെ selected_whole_module(mod->name);
}

ശൂന്യം RTLIL::Design::തള്ളുക_selection(RTLIL::Selection sel)
{
	sel.current_design = ഇത്;
	selection_അട്ടി.തള്ളുക_back(sel);
}

ശൂന്യം RTLIL::Design::തള്ളുക_empty_selection()
{
	തള്ളുക_selection(RTLIL::Selection::EmptySelection(ഇത്));
}

ശൂന്യം RTLIL::Design::തള്ളുക_full_selection()
{
	തള്ളുക_selection(RTLIL::Selection::FullSelection(ഇത്));
}

ശൂന്യം RTLIL::Design::തള്ളുക_complete_selection()
{
	തള്ളുക_selection(RTLIL::Selection::CompleteSelection(ഇത്));
}

ശൂന്യം RTLIL::Design::എടുക്കുക_selection()
{
	selection_അട്ടി.എടുക്കുക_back();
	// Default to a full_selection if we ran out of stack
	എങ്കിൽ (selection_അട്ടി.empty())
		തള്ളുക_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(തർക്കം only_selected) സ്ഥിരം
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(ഇത്), only_selected, സത്യം, അസത്യം, RTLIL_BACKEND::DumpMode::Readable);
	തിരികെ f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) സ്ഥിരം
{
	തർക്കം include_partials = partials == RTLIL::SELECT_ALL;
	തർക്കം exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	തർക്കം ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> result;
	result.reserve(modules_.size());
	വേണ്ടി (auto &it : modules_) {
		എങ്കിൽ (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			എങ്കിൽ (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				result.തള്ളുക_back(it.second);
			അല്ലെങ്കിൽ
				തിരഞ്ഞെടുപ്പ് (boxes)
				{
				അവസ്ഥ RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.second);
					നിർത്തുക;
				അവസ്ഥ RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.second);
					നിർത്തുക;
				അവസ്ഥ RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.second);
					നിർത്തുക;
				അവസ്ഥ RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.second);
					നിർത്തുക;
				അവസ്ഥ RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.second);
					നിർത്തുക;
				അവസ്ഥ RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.second);
					നിർത്തുക;
				സ്ഥിരസ്ഥിതി:
					നിർത്തുക;
				}
		} അല്ലെങ്കിൽ എങ്കിൽ (!include_partials && selected_module(it.first)) {
			തിരഞ്ഞെടുപ്പ്(partials)
			{
			അവസ്ഥ RTLIL::SELECT_WHOLE_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.second);
				നിർത്തുക;
			അവസ്ഥ RTLIL::SELECT_WHOLE_ERR:
				log_error("Unsupported partially selected module %s.\n", it.second);
				നിർത്തുക;
			അവസ്ഥ RTLIL::SELECT_WHOLE_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.second);
				നിർത്തുക;
			സ്ഥിരസ്ഥിതി:
				നിർത്തുക;
			}
		}
	}
	തിരികെ result;
}

RTLIL::Module::Module()
{
	സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ hashidx_എണ്ണം = 123456789;
	hashidx_എണ്ണം = mkhash_xorshift(hashidx_എണ്ണം);
	hashidx_ = hashidx_എണ്ണം;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->തിരുകുക(std::pair<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Module*>(hashidx_, ഇത്));
#endif
}

RTLIL::Module::~Module()
{
	വേണ്ടി (auto &pr : wires_)
		delete pr.second;
	വേണ്ടി (auto &pr : memories)
		delete pr.second;
	വേണ്ടി (auto &pr : cells_)
		delete pr.second;
	വേണ്ടി (auto &pr : processes)
		delete pr.second;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
സ്ഥിതം std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Module*> all_modules;
std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Module*> *RTLIL::Module::get_all_modules(ശൂന്യം)
{
	തിരികെ &all_modules;
}
#endif

ശൂന്യം RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	വേണ്ടി (auto it = wires_.begin(); it != wires_.end(); ++it)
		എങ്കിൽ (!it->second->port_നിവേശം && !it->second->port_output)
			delwires.തിരുകുക(it->second);

	വേണ്ടി (auto it = memories.begin(); it != memories.end(); ++it)
		delete it->second;
	memories.clear();

	വേണ്ടി (auto it = cells_.begin(); it != cells_.end(); ++it)
		delete it->second;
	cells_.clear();

	വേണ്ടി (auto it = processes.begin(); it != processes.end(); ++it)
		delete it->second;
	processes.clear();

	connections_.clear();

	remove(delwires);
	set_bool_attribute(ID::blackbox);
}

ശൂന്യം RTLIL::Module::expand_interfaces(RTLIL::Design *, സ്ഥിരം dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

തർക്കം RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	തിരികെ അസത്യം;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, സ്ഥിരം dict<RTLIL::IdString, RTLIL::Const> &, തർക്കം mayfail)
{
	എങ്കിൽ (mayfail)
		തിരികെ IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, സ്ഥിരം dict<RTLIL::IdString, RTLIL::Const> &, സ്ഥിരം dict<RTLIL::IdString, RTLIL::Module*> &, സ്ഥിരം dict<RTLIL::IdString, RTLIL::IdString> &, തർക്കം mayfail)
{
	എങ്കിൽ (mayfail)
		തിരികെ IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

size_t RTLIL::Module::എണ്ണം_id(RTLIL::IdString id)
{
	തിരികെ wires_.എണ്ണം(id) + cells_.എണ്ണം(id) + memories.എണ്ണം(id) + processes.എണ്ണം(id);
}

#ifndef NDEBUG
നാമാകാശ {
	ഘടന InternalCellChecker
	{
		സ്ഥിരം RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		InternalCellChecker(സ്ഥിരം RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		ശൂന്യം error(പൂർണ്ണസംഖ്യ linenr, std::optional<std::string> error_സന്ദേശം = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			എങ്കിൽ (error_സന്ദേശം) {
				log_error("%s in internal cell %s\n", *error_സന്ദേശം, cell_loc);
			} അല്ലെങ്കിൽ {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		പൂർണ്ണസംഖ്യ param(RTLIL::IdString name)
		{
			auto it = cell->parameters.find(name);
			എങ്കിൽ (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.തിരുകുക(name);
			തിരികെ it->second.as_int();
		}

		പൂർണ്ണസംഖ്യ param_bool(RTLIL::IdString name)
		{
			പൂർണ്ണസംഖ്യ v = param(name);
			എങ്കിൽ (GetSize(cell->parameters.at(name)) > 32)
				error(__LINE__);
			എങ്കിൽ (v != 0 && v != 1)
				error(__LINE__);
			തിരികെ v;
		}

		പൂർണ്ണസംഖ്യ param_bool(RTLIL::IdString name, തർക്കം expected)
		{
			പൂർണ്ണസംഖ്യ v = param_bool(name);
			എങ്കിൽ (v != expected)
				error(__LINE__);
			തിരികെ v;
		}

		ശൂന്യം param_bits(RTLIL::IdString name, പൂർണ്ണസംഖ്യ width)
		{
			param(name);
			എങ്കിൽ (GetSize(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			തിരികെ cell->parameters.at(name).decode_string();
		}

		ശൂന്യം port(RTLIL::IdString name, പൂർണ്ണസംഖ്യ width)
		{
			auto it = cell->connections_.find(name);
			എങ്കിൽ (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			എങ്കിൽ (GetSize(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.തിരുകുക(name);
		}

		ശൂന്യം check_expected(തർക്കം check_matched_sign = അസത്യം)
		{
			വേണ്ടി (auto &para : cell->parameters)
				എങ്കിൽ (expected_params.എണ്ണം(para.first) == 0)
					error(__LINE__);
			വേണ്ടി (auto &conn : cell->connections())
				എങ്കിൽ (expected_ports.എണ്ണം(conn.first) == 0)
					error(__LINE__);

			എങ്കിൽ (check_matched_sign) {
				log_assert(expected_params.എണ്ണം(ID::A_SIGNED) != 0 && expected_params.എണ്ണം(ID::B_SIGNED) != 0);
				തർക്കം a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				തർക്കം b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				എങ്കിൽ (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		ശൂന്യം check()
		{
			എങ്കിൽ (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				തിരികെ;

			എങ്കിൽ (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(സത്യം);
				തിരികെ;
			}

			എങ്കിൽ (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/അസത്യം);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/അസത്യം);
				തിരികെ;
			}

			എങ്കിൽ (cell->type.in(ID($shift), ID($shiftx))) {
				എങ്കിൽ (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/അസത്യം);
				} അല്ലെങ്കിൽ {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/അസത്യം);
				തിരികെ;
			}

			എങ്കിൽ (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(സത്യം);
				തിരികെ;
			}

			എങ്കിൽ (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($pow));
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::CI, 1);
				port(ID::BI, 1);
				port(ID::X, param(ID::Y_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				port(ID::CO, param(ID::Y_WIDTH));
				check_expected(സത്യം);
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($macc_v2)) {
				എങ്കിൽ (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				എങ്കിൽ (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				എങ്കിൽ (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				സ്ഥിരം Const &a_width = cell->getParam(ID::A_WIDTHS);
				സ്ഥിരം Const &b_width = cell->getParam(ID::B_WIDTHS);
				സ്ഥിരം Const &c_width = cell->getParam(ID::C_WIDTHS);
				പൂർണ്ണസംഖ്യ a_width_sum = 0, b_width_sum = 0, c_width_sum = 0;
				വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_sum += a_width.extract(16 * i, 16).as_int(അസത്യം);
					b_width_sum += b_width.extract(16 * i, 16).as_int(അസത്യം);
				}
				വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < param(ID::NADDENDS); i++) {
					c_width_sum += c_width.extract(16 * i, 16).as_int(അസത്യം);
				}
				port(ID::A, a_width_sum);
				port(ID::B, b_width_sum);
				port(ID::C, c_width_sum);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/അസത്യം);
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				എങ്കിൽ (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($dffsre)) {
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
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_മൂല്യം, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_മൂല്യം, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type.in(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_മൂല്യം, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_മൂല്യം, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($aldffe)) {
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
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_മൂല്യം, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_BITS);
				param(ID::STATE_അക്കം);
				param(ID::STATE_അക്കം_LOG2);
				param(ID::STATE_RST);
				param_bits(ID::STATE_TABLE, param(ID::STATE_BITS) * param(ID::STATE_അക്കം));
				param(ID::TRANS_അക്കം);
				param_bits(ID::TRANS_TABLE, param(ID::TRANS_അക്കം) * (2*param(ID::STATE_അക്കം_LOG2) + param(ID::CTRL_IN_WIDTH) + param(ID::CTRL_OUT_WIDTH)));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::CTRL_IN, param(ID::CTRL_IN_WIDTH));
				port(ID::CTRL_OUT, param(ID::CTRL_OUT_WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_MASK);
				param(ID::COLLISION_X_MASK);
				param_bool(ID::CE_OVER_SRST);
				param_bits(ID::ARST_മൂല്യം, param(ID::WIDTH));
				param_bits(ID::SRST_മൂല്യം, param(ID::WIDTH));
				param_bits(ID::INIT_മൂല്യം, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::SRST, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($memwr_v2)) {
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
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::വാക്കുകൾ));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::വാക്കുകൾ));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($mem)) {
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
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($mem_v2)) {
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
				param_bits(ID::RD_ARST_മൂല്യം, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_SRST_മൂല്യം, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_INIT_മൂല്യം, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
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
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type.in(ID($പ്രതിജ്ഞാ), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type.in(ID($specify2), ID($specify3))) {
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
				എങ്കിൽ (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
				}
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($specrule)) {
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
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				എങ്കിൽ (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
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
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				എങ്കിൽ (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				തിരികെ;
			}

			എങ്കിൽ (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); തിരികെ; }
			എങ്കിൽ (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  തിരികെ; }

			എങ്കിൽ (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); തിരികെ; }

			എങ്കിൽ (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}
			എങ്കിൽ (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}
			എങ്കിൽ (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}
			എങ്കിൽ (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}
			എങ്കിൽ (cell->type.in(ID($നിവേശം_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				തിരികെ;
			}
			എങ്കിൽ (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				check_expected();
				തിരികെ;
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

ശൂന്യം RTLIL::Module::sort()
{
	wires_.sort(sort_by_id_str(design->twines()));
	cells_.sort(sort_by_id_str(design->twines()));
	parameter_default_മൂല്യങ്ങൾ.sort(sort_by_id_str(design->twines()));
	memories.sort(sort_by_id_str(design->twines()));
	processes.sort(sort_by_id_str(design->twines()));
	വേണ്ടി (auto &it : cells_)
		it.second->sort();
	വേണ്ടി (auto &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	വേണ്ടി (auto &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#ifndef NDEBUG
സ്ഥിതം ശൂന്യം check_id_താക്കോലുകൾ(സ്ഥിരം dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	വേണ്ടി (auto &it : keyed)
		log_assert(!it.first.empty());
}
#endif

ശൂന്യം check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ifndef NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->cells_size(), 1000));
	സ്ഥിരം RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	വേണ്ടി (auto &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->width >= 0);
		log_assert(it.second->size >= 0);
		check_id_താക്കോലുകൾ(it.second->attributes);
		memory_strings.തിരുകുക(it.second->name.str());
	}

	std::vector<MonotonicFlag> ports_declared(GetSize(module->ports));
	ShardedVector<std::string> memids(subpool);
	subpool.run([const_module, &ports_declared, &memory_strings, &memids](സ്ഥിരം ParallelDispatchThreadPool::RunCtx &ctx) {
		വേണ്ടി (പൂർണ്ണസംഖ്യ i : ctx.item_range(const_module->cells_size())) {
			auto it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			വേണ്ടി (auto &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_താക്കോലുകൾ(it.second->attributes);
			check_id_താക്കോലുകൾ(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			എങ്കിൽ (it.second->has_memid()) {
				log_assert(memory_strings.എണ്ണം(it.second->parameters.at(ID::MEMID).decode_string()));
			} അല്ലെങ്കിൽ എങ്കിൽ (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.എണ്ണം(memid));
				memids.തിരുകുക(ctx, std::move(memid));
			}
			auto cell_mod = const_module->design->module(it.second->name);
			എങ്കിൽ (cell_mod != nullptr) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#ifdef __GNUC__
				#pragma GCC diagnostic തള്ളുക
				#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				#endif
				log_assert(!it.second->get_blackbox_attribute());
				#ifdef __GNUC__
				#pragma GCC diagnostic എടുക്കുക
				#endif
			}
		}

		വേണ്ടി (പൂർണ്ണസംഖ്യ i : ctx.item_range(const_module->wires_size())) {
			auto it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_താക്കോലുകൾ(it.second->attributes);
			എങ്കിൽ (it.second->port_id) {
				log_assert(GetSize(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_നിവേശം || it.second->port_output);
				log_assert(it.second->port_id <= GetSize(ports_declared));
				തർക്കം previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == അസത്യം);
			} അല്ലെങ്കിൽ
				log_assert(!it.second->port_നിവേശം && !it.second->port_output);
		}
	});
	വേണ്ടി (സ്ഥിരം MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == സത്യം);
	pool<std::string> memids_pool;
	വേണ്ടി (std::string &memid : memids)
		log_assert(memids_pool.തിരുകുക(memid).second);

	വേണ്ടി (auto &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->വേര്_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->വേര്_case};
		വേണ്ടി (size_t i = 0; i < all_cases.size(); i++) {
			വേണ്ടി (auto &switch_it : all_cases[i]->switches) {
				വേണ്ടി (auto &case_it : switch_it->cases) {
					വേണ്ടി (auto &compare_it : case_it->compare) {
						log_assert(switch_it->സൂചന.size() == compare_it.size());
					}
					all_cases.തള്ളുക_back(case_it);
				}
			}
		}
		വേണ്ടി (auto &sync_it : it.second->syncs) {
			തിരഞ്ഞെടുപ്പ് (sync_it->type) {
				അവസ്ഥ RTLIL::SyncType::ST0:
				അവസ്ഥ RTLIL::SyncType::ST1:
				അവസ്ഥ RTLIL::SyncType::STp:
				അവസ്ഥ RTLIL::SyncType::STn:
				അവസ്ഥ RTLIL::SyncType::STe:
					log_assert(!sync_it->സൂചന.empty());
					നിർത്തുക;
				അവസ്ഥ RTLIL::SyncType::STa:
				അവസ്ഥ RTLIL::SyncType::STg:
				അവസ്ഥ RTLIL::SyncType::STi:
					log_assert(sync_it->സൂചന.empty());
					നിർത്തുക;
			}
		}
	}

	വേണ്ടി (auto &it : module->connections_) {
		log_assert(it.first.size() == it.second.size());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_താക്കോലുകൾ(module->attributes);
#endif
}

ശൂന്യം RTLIL::Module::check()
{
	പൂർണ്ണസംഖ്യ pool_size = ThreadPool::work_pool_size(0, cells_size(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_size);
	check_module(ഇത്, thread_pool);
}

ശൂന്യം RTLIL::Module::optimize()
{
}

ശൂന്യം RTLIL::Module::cloneInto(RTLIL::Module *new_mod) സ്ഥിരം
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	വേണ്ടി (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_മൂല്യങ്ങൾ.clear();
	വേണ്ടി (auto &it : parameter_default_മൂല്യങ്ങൾ)
		new_mod->parameter_default_മൂല്യങ്ങൾ[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	വേണ്ടി (auto &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	വേണ്ടി (auto it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	വേണ്ടി (auto it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	വേണ്ടി (auto it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	വേണ്ടി (auto it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addProcess(dst_id, it->second);
	}

	ഘടന RewriteSigSpecWorker
	{
		സ്ഥിരം dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		സ്ഥിരം RTLIL::Module *new_mod;
		ശൂന്യം operator()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([ഇത്](RTLIL::Wire *&wire) {
				auto it = wire_map.find(wire);
				എങ്കിൽ (it != wire_map.end())
					wire = it->second;
				അല്ലെങ്കിൽ
					log_assert(wire->module == new_mod);
			});
		}
	};

	RewriteSigSpecWorker rewriteSigSpecWorker{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewriteSigSpecWorker);
	new_mod->fixup_ports();
}

RTLIL::Module *RTLIL::Module::clone() സ്ഥിരം
{
	RTLIL::Module *new_mod = പുതിയ RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	തിരികെ new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) സ്ഥിരം
{
	RTLIL::Module *new_mod = പുതിയ RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->add(new_mod);
	തിരികെ new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) സ്ഥിരം
{
	RTLIL::Module *new_mod = പുതിയ RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	തിരികെ new_mod;
}

തർക്കം RTLIL::Module::has_memories() സ്ഥിരം
{
	തിരികെ !memories.empty();
}

തർക്കം RTLIL::Module::has_processes() സ്ഥിരം
{
	തിരികെ !processes.empty();
}

തർക്കം RTLIL::Module::has_memories_warn() സ്ഥിരം
{
	എങ്കിൽ (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", ഇത്);
	തിരികെ !memories.empty();
}

തർക്കം RTLIL::Module::has_processes_warn() സ്ഥിരം
{
	എങ്കിൽ (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", ഇത്);
	തിരികെ !processes.empty();
}

തർക്കം RTLIL::Module::is_selected() സ്ഥിരം
{
	തിരികെ design->selected_module(ഇത്->name);
}

തർക്കം RTLIL::Module::is_selected_whole() സ്ഥിരം
{
	തിരികെ design->selected_whole_module(ഇത്->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() സ്ഥിരം
{
	std::vector<RTLIL::Wire*> result;
	result.reserve(wires_.size());
	വേണ്ടി (auto &it : wires_)
		എങ്കിൽ (design->selected(ഇത്, it.second))
			result.തള്ളുക_back(it.second);
	തിരികെ result;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() സ്ഥിരം
{
	std::vector<RTLIL::Cell*> result;
	result.reserve(cells_.size());
	വേണ്ടി (auto &it : cells_)
		എങ്കിൽ (design->selected(ഇത്, it.second))
			result.തള്ളുക_back(it.second);
	തിരികെ result;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() സ്ഥിരം
{
	std::vector<RTLIL::Memory*> result;
	result.reserve(memories.size());
	വേണ്ടി (auto &it : memories)
		എങ്കിൽ (design->selected(ഇത്, it.second))
			result.തള്ളുക_back(it.second);
	തിരികെ result;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() സ്ഥിരം
{
	std::vector<RTLIL::Process*> result;
	result.reserve(processes.size());
	വേണ്ടി (auto &it : processes)
		എങ്കിൽ (design->selected(ഇത്, it.second))
			result.തള്ളുക_back(it.second);
	തിരികെ result;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() സ്ഥിരം
{
	std::vector<RTLIL::SelectedMember> result;
	വേണ്ടി (auto cell : selected_cells())
		result.തള്ളുക_back({cell, cell->name});
	വേണ്ടി (auto memory : selected_memories())
		result.തള്ളുക_back({memory, memory->name});
	വേണ്ടി (auto wire : selected_wires())
		result.തള്ളുക_back({wire, wire->name});
	വേണ്ടി (auto process : selected_processes())
		result.തള്ളുക_back({process, process->name});
	തിരികെ result;
}

ശൂന്യം RTLIL::Module::add(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(എണ്ണം_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = ഇത്;
}

ശൂന്യം RTLIL::Module::add(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(എണ്ണം_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = ഇത്;
}

ശൂന്യം RTLIL::Module::add(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(എണ്ണം_id(process->name) == 0);
	processes[process->name] = process;
	process->module = ഇത്;
}

ശൂന്യം RTLIL::Module::remove(സ്ഥിരം pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	എങ്കിൽ (wires.empty())
		തിരികെ;

	ഘടന DeleteWireWorker
	{
		RTLIL::Module *module;
		സ്ഥിരം pool<RTLIL::Wire*> *wires_p;

		ശൂന്യം operator()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([ഇത്](RTLIL::Wire *&wire) {
				എങ്കിൽ (wires_p->എണ്ണം(wire))
					wire = module->addWire(stringf("$delete_wire$%d", (പൂർണ്ണസംഖ്യ)autoidx++), wire->width);
			});
		}

		ശൂന്യം operator()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	DeleteWireWorker delete_wire_worker;
	delete_wire_worker.module = ഇത്;
	delete_wire_worker.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_worker);

	എങ്കിൽ (design->flagBufferedNormalized) {
		വേണ്ടി (auto wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_index.erase(wire);
		}
	}

	വേണ്ടി (auto &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.എണ്ണം(id) != 0);
		wires_.erase(id);
		delete it;
	}
}

ശൂന്യം RTLIL::Module::remove(RTLIL::Cell *cell)
{
	ആകുമ്പോൾ (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.എണ്ണം(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	എങ്കിൽ (design && design->flagBufferedNormalized && buf_norm_cell_queue.എണ്ണം(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.തിരുകുക(cell);
	} അല്ലെങ്കിൽ {
		delete cell;
	}
}

ശൂന്യം RTLIL::Module::remove(RTLIL::Memory *memory)
{
	log_assert(memories.എണ്ണം(memory->name) != 0);
	memories.erase(memory->name);
	delete memory;
}

ശൂന്യം RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.എണ്ണം(process->name) != 0);
	processes.erase(process->name);
	delete process;
}

ശൂന്യം RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	add(wire);
}

ശൂന്യം RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	add(cell);
}

ശൂന്യം RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	എങ്കിൽ (old_id != IdString::Null && wires_.എണ്ണം(old_id))
		rename(wires_.at(old_id), new_name);
	അല്ലെങ്കിൽ എങ്കിൽ (old_id != IdString::Null && cells_.എണ്ണം(old_id))
		rename(cells_.at(old_id), new_name);
	അല്ലെങ്കിൽ
		log_abort();
}

ശൂന്യം RTLIL::Module::swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2)
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

ശൂന്യം RTLIL::Module::swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	പൂർണ്ണസംഖ്യ index = 0;
	തിരികെ uniquify(name, index);
}

IdString RTLIL::Module::uniquify(IdString name, പൂർണ്ണസംഖ്യ &index)
{
	എങ്കിൽ (index == 0) {
		എങ്കിൽ (എണ്ണം_id(name) == 0)
			തിരികെ name;
		index++;
	}

	ആകുമ്പോൾ (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", index)});
		എങ്കിൽ (എണ്ണം_id(new_name) == 0)
			തിരികെ new_name;
		index++;
	}
}

സ്ഥിതം തർക്കം fixup_ports_compare(സ്ഥിരം RTLIL::Wire *a, സ്ഥിരം RTLIL::Wire *b)
{
	എങ്കിൽ (a->port_id && !b->port_id)
		തിരികെ സത്യം;
	എങ്കിൽ (!a->port_id && b->port_id)
		തിരികെ അസത്യം;

	എങ്കിൽ (a->port_id == b->port_id)
		തിരികെ a->name < b->name;
	തിരികെ a->port_id < b->port_id;
}

ശൂന്യം RTLIL::Module::connect(സ്ഥിരം RTLIL::SigSig &conn)
{
	വേണ്ടി (auto mon : monitors)
		mon->notify_connect(ഇത്, conn);

	എങ്കിൽ (design)
		വേണ്ടി (auto mon : design->monitors)
			mon->notify_connect(ഇത്, conn);

	// ignore all attempts to assign constants to other constants
	എങ്കിൽ (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(conn.first); i++)
			എങ്കിൽ (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		എങ്കിൽ (GetSize(new_conn.first))
			connect(new_conn);
		തിരികെ;
	}

	എങ്കിൽ (yosys_xtrace) {
		log("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", ഇത്, log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(GetSize(conn.first) == GetSize(conn.second));
	connections_.തള്ളുക_back(conn);
}

ശൂന്യം RTLIL::Module::connect(സ്ഥിരം RTLIL::SigSpec &lhs, സ്ഥിരം RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

ശൂന്യം RTLIL::Module::new_connections(സ്ഥിരം std::vector<RTLIL::SigSig> &new_conn)
{
	വേണ്ടി (auto mon : monitors)
		mon->notify_connect(ഇത്, new_conn);

	എങ്കിൽ (design)
		വേണ്ടി (auto mon : design->monitors)
			mon->notify_connect(ഇത്, new_conn);

	എങ്കിൽ (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", ഇത്);
		വേണ്ടി (auto &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

സ്ഥിരം std::vector<RTLIL::SigSig> &RTLIL::Module::connections() സ്ഥിരം
{
	തിരികെ connections_;
}

ശൂന്യം RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	വേണ്ടി (auto &w : wires_)
		എങ്കിൽ (w.second->port_നിവേശം || w.second->port_output)
			all_ports.തള്ളുക_back(w.second);
		അല്ലെങ്കിൽ
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	എങ്കിൽ (design && design->flagBufferedNormalized) {
		വേണ്ടി (auto &w : wires_)
			എങ്കിൽ (w.second->driverCell_ && w.second->driverCell_->type == ID($നിവേശം_port))
				buf_norm_wire_queue.തിരുകുക(w.second);

		buf_norm_wire_queue.തിരുകുക(all_ports.begin(), all_ports.end());
	}

	ports.clear();
	വേണ്ടി (size_t i = 0; i < all_ports.size(); i++) {
		ports.തള്ളുക_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
	}
}

ശൂന്യം RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		സ്ഥിരം dict<IdString, RTLIL::Const> &src,
		സ്ഥിരം RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	എങ്കിൽ (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		തിരികെ;
	}
	dst.clear();
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = GetSize(src) - 1; i >= 0; i--) {
		auto &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::addWire(RTLIL::IdString name, പൂർണ്ണസംഖ്യ width)
{
	log_assert(design);
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	RTLIL::Wire *wire = പുതിയ RTLIL::Wire;
	wire->width = width;
	wire->name = name;
	add(wire);
	തിരികെ wire;
}

RTLIL::Wire *RTLIL::Module::addWire(IdString name, സ്ഥിരം RTLIL::Wire *other)
{
	RTLIL::Wire *wire = addWire(name);
	wire->width = other->width;
	wire->start_offset = other->start_offset;
	wire->port_id = other->port_id;
	wire->port_നിവേശം = other->port_നിവേശം;
	wire->port_output = other->port_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		സ്ഥിരം RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
		RTLIL::copy_attr_dict(wire->attributes, other->attributes, src_design, ഇത്->design);
	}
	തിരികെ wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = പുതിയ RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	തിരികെ cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, സ്ഥിരം RTLIL::Cell *other)
{
	സ്ഥിരം RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	തർക്കം cross_pool = src_design && ഇത്->design && src_design != ഇത്->design;

	IdString type = other->type.ref();
	എങ്കിൽ (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, ഇത്->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, ഇത്->design);

	എങ്കിൽ (!cross_pool)
		cell->connections_ = other->connections_;
	അല്ലെങ്കിൽ
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = GetSize(other->connections_) - 1; i >= 0; i--) {
			auto &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	തിരികെ cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = പുതിയ RTLIL::Memory;
	mem->module = ഇത്;
	mem->name = name;
	memories[name] = mem;
	തിരികെ mem;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name, സ്ഥിരം RTLIL::Memory *other)
{
	log_assert(design);
	RTLIL::Memory *mem = പുതിയ RTLIL::Memory;
	mem->module = ഇത്;
	mem->name = name;
	mem->width = other->width;
	mem->start_offset = other->start_offset;
	mem->size = other->size;
	RTLIL::copy_attr_dict(mem->attributes, other->attributes,
			other->module ? other->module->design : nullptr, design);
	memories[name] = mem;
	തിരികെ mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = പുതിയ RTLIL::Process;
	proc->name = name;
	add(proc);
	തിരികെ proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, സ്ഥിരം RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	സ്ഥിരം RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	എങ്കിൽ (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		വേണ്ടി (auto *സമന്വയം : proc->syncs)
			വേണ്ടി (auto &mwa : സമന്വയം->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	തിരികെ proc;
}

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, തർക്കം is_signed, സ്ഥിരം std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		തിരികെ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, തർക്കം is_signed, സ്ഥിരം std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		തിരികെ sig_y;                                       \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, തർക്കം /* is_signed */, സ്ഥിരം std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::WIDTH] = sig_a.size();         \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		തിരികെ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, തർക്കം is_signed, സ്ഥിരം std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		തിരികെ sig_y;                                       \
	}
DEF_METHOD(Buf,     sig_a.size(), ID($buf))
DEF_METHOD(Barrier, sig_a.size(), ID($barrier))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, തർക്കം is_signed, സ്ഥിരം std::string &src) { \
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
		തിരികെ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, തർക്കം is_signed, സ്ഥിരം std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		തിരികെ sig_y;                                            \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, തർക്കം is_signed, സ്ഥിരം std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = അസത്യം;             \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		തിരികെ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, തർക്കം is_signed, സ്ഥിരം std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		തിരികെ sig_y;                                            \
	}
DEF_METHOD(Shl,      sig_a.size(), ID($shl))
DEF_METHOD(Shr,      sig_a.size(), ID($shr))
DEF_METHOD(Sshl,     sig_a.size(), ID($sshl))
DEF_METHOD(Sshr,     sig_a.size(), ID($sshr))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, തർക്കം is_signed, സ്ഥിരം std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = അസത്യം;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		തിരികെ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, തർക്കം is_signed, സ്ഥിരം std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		തിരികെ sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.size(), ID($shiftx))
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		എങ്കിൽ (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.size();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		തിരികെ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		തിരികെ sig_y;                                             \
	}
DEF_METHOD(Mux,      ID($mux),        0)
DEF_METHOD(Bwmux,    ID($bwmux),      0)
DEF_METHOD(Pmux,     ID($pmux),       1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = _demux ? sig_a.size() : sig_y.size(); \
		cell->parameters[ID::S_WIDTH] = sig_s.size();             \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		തിരികെ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.size() << sig_s.size() : sig_a.size() >> sig_s.size()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		തിരികെ sig_y;                                             \
	}
DEF_METHOD(Bmux,     ID($bmux),       0)
DEF_METHOD(Demux,    ID($demux),      1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		തിരികെ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		തിരികെ sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#undef DEF_METHOD

#define DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig1, സ്ഥിരം RTLIL::SigBit &sig2, സ്ഥിരം std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		തിരികെ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig1, സ്ഥിരം std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		തിരികെ sig2;                                      \
	}
#define DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig1, സ്ഥിരം RTLIL::SigBit &sig2, സ്ഥിരം RTLIL::SigBit &sig3, സ്ഥിരം std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		തിരികെ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig1, സ്ഥിരം RTLIL::SigBit &sig2, സ്ഥിരം std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		തിരികെ sig3;                                      \
	}
#define DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig1, സ്ഥിരം RTLIL::SigBit &sig2, സ്ഥിരം RTLIL::SigBit &sig3, സ്ഥിരം RTLIL::SigBit &sig4, സ്ഥിരം std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		തിരികെ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig1, സ്ഥിരം RTLIL::SigBit &sig2, സ്ഥിരം RTLIL::SigBit &sig3, സ്ഥിരം std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		തിരികെ sig4;                                      \
	}
#define DEF_METHOD_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig1, സ്ഥിരം RTLIL::SigBit &sig2, സ്ഥിരം RTLIL::SigBit &sig3, സ്ഥിരം RTLIL::SigBit &sig4, സ്ഥിരം RTLIL::SigBit &sig5, സ്ഥിരം std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->setPort("\\" #_P5, sig5);                   \
		cell->set_src_attribute(src);                     \
		തിരികെ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, സ്ഥിരം RTLIL::SigBit &sig1, സ്ഥിരം RTLIL::SigBit &sig2, സ്ഥിരം RTLIL::SigBit &sig3, സ്ഥിരം RTLIL::SigBit &sig4, സ്ഥിരം std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		തിരികെ sig5;                                           \
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

RTLIL::Cell* RTLIL::Module::addPow(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, തർക്കം a_signed, തർക്കം b_signed, സ്ഥിരം std::string &src)
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
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addFa(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_c, സ്ഥിരം RTLIL::SigSpec &sig_x, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fa));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::C, sig_c);
	cell->setPort(ID::X, sig_x);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addSlice(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, RTLIL::Const offset, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($slice));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::Y_WIDTH] = sig_y.size();
	cell->parameters[ID::OFFSET] = offset;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addConcat(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($concat));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::B_WIDTH] = sig_b.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_y, RTLIL::Const lut, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($പ്രതിജ്ഞാ));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_b, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addSr(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr, സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം set_polarity, തർക്കം clr_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sr));
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addDff(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം clk_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addDffe(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം clk_polarity, തർക്കം en_polarity, സ്ഥിരം std::string &src)
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
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addDffsr(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം clk_polarity, തർക്കം set_polarity, തർക്കം clr_polarity, സ്ഥിരം std::string &src)
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
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addDffsre(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം clk_polarity, തർക്കം en_polarity, തർക്കം set_polarity, തർക്കം clr_polarity, സ്ഥിരം std::string &src)
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
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addAdff(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_arst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_മൂല്യം, തർക്കം clk_polarity, തർക്കം arst_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_മൂല്യം] = arst_മൂല്യം;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addAdffe(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_arst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_മൂല്യം, തർക്കം clk_polarity, തർക്കം en_polarity, തർക്കം arst_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_മൂല്യം] = arst_മൂല്യം;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addAldff(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_aload, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		സ്ഥിരം RTLIL::SigSpec &sig_ad, തർക്കം clk_polarity, തർക്കം aload_polarity, സ്ഥിരം std::string &src)
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
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addAldffe(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_aload, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		സ്ഥിരം RTLIL::SigSpec &sig_ad, തർക്കം clk_polarity, തർക്കം en_polarity, തർക്കം aload_polarity, സ്ഥിരം std::string &src)
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
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addSdff(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_srst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_മൂല്യം, തർക്കം clk_polarity, തർക്കം srst_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_മൂല്യം] = srst_മൂല്യം;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffe(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_srst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_മൂല്യം, തർക്കം clk_polarity, തർക്കം en_polarity, തർക്കം srst_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_മൂല്യം] = srst_മൂല്യം;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffce(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_srst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_മൂല്യം, തർക്കം clk_polarity, തർക്കം en_polarity, തർക്കം srst_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_മൂല്യം] = srst_മൂല്യം;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addDlatch(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം en_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatch(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_arst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_മൂല്യം, തർക്കം en_polarity, തർക്കം arst_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_മൂല്യം] = arst_മൂല്യം;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsr(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം en_polarity, തർക്കം set_polarity, തർക്കം clr_polarity, സ്ഥിരം std::string &src)
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
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr,
		സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം set_polarity, തർക്കം clr_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം clk_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം clk_polarity, തർക്കം en_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addDffsrGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം clk_polarity, തർക്കം set_polarity, തർക്കം clr_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addDffsreGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം clk_polarity, തർക്കം en_polarity, തർക്കം set_polarity, തർക്കം clr_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addAdffGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_arst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		തർക്കം arst_മൂല്യം, തർക്കം clk_polarity, തർക്കം arst_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_മൂല്യം ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addAdffeGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_arst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		തർക്കം arst_മൂല്യം, തർക്കം clk_polarity, തർക്കം en_polarity, തർക്കം arst_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_മൂല്യം ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addAldffGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_aload, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		സ്ഥിരം RTLIL::SigSpec &sig_ad, തർക്കം clk_polarity, തർക്കം aload_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addAldffeGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_aload, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		സ്ഥിരം RTLIL::SigSpec &sig_ad, തർക്കം clk_polarity, തർക്കം en_polarity, തർക്കം aload_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_srst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		തർക്കം srst_മൂല്യം, തർക്കം clk_polarity, തർക്കം srst_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_മൂല്യം ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffeGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_srst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		തർക്കം srst_മൂല്യം, തർക്കം clk_polarity, തർക്കം en_polarity, തർക്കം srst_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_മൂല്യം ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffceGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_clk, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_srst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		തർക്കം srst_മൂല്യം, തർക്കം clk_polarity, തർക്കം en_polarity, തർക്കം srst_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_മൂല്യം ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം en_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatchGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_arst, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q,
		തർക്കം arst_മൂല്യം, തർക്കം en_polarity, തർക്കം arst_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_മൂല്യം ? '1' : '0'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsrGate(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_en, സ്ഥിരം RTLIL::SigSpec &sig_set, സ്ഥിരം RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, തർക്കം en_polarity, തർക്കം set_polarity, തർക്കം clr_polarity, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_d, സ്ഥിരം RTLIL::SigSpec &sig_q, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, പൂർണ്ണസംഖ്യ width, സ്ഥിരം std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	തിരികെ sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, പൂർണ്ണസംഖ്യ width, സ്ഥിരം std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	തിരികെ sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, പൂർണ്ണസംഖ്യ width, സ്ഥിരം std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	തിരികെ sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, പൂർണ്ണസംഖ്യ width, സ്ഥിരം std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	തിരികെ sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, സ്ഥിരം std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	തിരികെ sig;
}

RTLIL::SigSpec RTLIL::Module::SetTag(RTLIL::IdString name, സ്ഥിരം std::string &tag, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം RTLIL::SigSpec &sig_c, സ്ഥിരം std::string &src)
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
	തിരികെ sig;
}

RTLIL::Cell* RTLIL::Module::addSetTag(RTLIL::IdString name, സ്ഥിരം std::string &tag, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം RTLIL::SigSpec &sig_c, സ്ഥിരം RTLIL::SigSpec &sig_y, സ്ഥിരം std::string &src)
{
	Cell *cell = addCell(name, ID($set_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::SigSpec RTLIL::Module::GetTag(RTLIL::IdString name, സ്ഥിരം std::string &tag, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($get_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	തിരികെ sig;
}

RTLIL::Cell* RTLIL::Module::addOverwriteTag(RTLIL::IdString name, സ്ഥിരം std::string &tag, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം RTLIL::SigSpec &sig_s, സ്ഥിരം RTLIL::SigSpec &sig_c, സ്ഥിരം std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($overwrite_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->set_src_attribute(src);
	തിരികെ cell;
}

RTLIL::SigSpec RTLIL::Module::OriginalTag(RTLIL::IdString name, സ്ഥിരം std::string &tag, സ്ഥിരം RTLIL::SigSpec &sig_a, സ്ഥിരം std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($original_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	തിരികെ sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, സ്ഥിരം RTLIL::SigSpec &sig_e, സ്ഥിരം std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.size());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.size();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	തിരികെ sig;
}

std::string RTLIL::Module::to_rtlil_str() സ്ഥിരം
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(ഇത്), design, അസത്യം, സത്യം, അസത്യം, RTLIL_BACKEND::DumpMode::Readable);
	തിരികെ f.str();
}

RTLIL::Wire::Wire()
{
	സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ hashidx_എണ്ണം = 123456789;
	hashidx_എണ്ണം = mkhash_xorshift(hashidx_എണ്ണം);
	hashidx_ = hashidx_എണ്ണം;

	module = nullptr;
	width = 1;
	start_offset = 0;
	port_id = 0;
	port_നിവേശം = അസത്യം;
	port_output = അസത്യം;
	upto = അസത്യം;
	is_signed = അസത്യം;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->തിരുകുക(std::pair<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Wire*>(hashidx_, ഇത്));
#endif
}

RTLIL::Wire::~Wire()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#endif
}

std::string RTLIL::Wire::to_rtlil_str() സ്ഥിരം
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", ഇത്, design(), RTLIL_BACKEND::DumpMode::Readable);
	തിരികെ f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
സ്ഥിതം std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Wire*> all_wires;
std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(ശൂന്യം)
{
	തിരികെ &all_wires;
}
#endif

RTLIL::Memory::Memory()
{
	സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ hashidx_എണ്ണം = 123456789;
	hashidx_എണ്ണം = mkhash_xorshift(hashidx_എണ്ണം);
	hashidx_ = hashidx_എണ്ണം;

	width = 1;
	start_offset = 0;
	size = 0;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->തിരുകുക(std::pair<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Memory*>(hashidx_, ഇത്));
#endif
}

std::string RTLIL::Memory::to_rtlil_str() സ്ഥിരം
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", ഇത്, design(), RTLIL_BACKEND::DumpMode::Readable);
	തിരികെ f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ hashidx_എണ്ണം = 123456789;
	hashidx_എണ്ണം = mkhash_xorshift(hashidx_എണ്ണം);
	hashidx_ = hashidx_എണ്ണം;
}

std::string RTLIL::Process::to_rtlil_str() സ്ഥിരം
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", ഇത്, design(), RTLIL_BACKEND::DumpMode::Readable);
	തിരികെ f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ hashidx_എണ്ണം = 123456789;
	hashidx_എണ്ണം = mkhash_xorshift(hashidx_എണ്ണം);
	hashidx_ = hashidx_എണ്ണം;

	// log("#memtrace# %p\n", this);
	memhasher();

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->തിരുകുക(std::pair<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Cell*>(hashidx_, ഇത്));
#endif
}

RTLIL::Cell::~Cell()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#endif
}

std::string RTLIL::Cell::to_rtlil_str() സ്ഥിരം
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", ഇത്, design(), RTLIL_BACKEND::DumpMode::Readable);
	തിരികെ f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
സ്ഥിതം std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Cell*> all_cells;
std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(ശൂന്യം)
{
	തിരികെ &all_cells;
}
#endif

തർക്കം RTLIL::Cell::hasPort(RTLIL::IdString portname) സ്ഥിരം
{
	തിരികെ connections_.എണ്ണം(portname) != 0;
}

// bufnorm

സ്ഥിരം RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) സ്ഥിരം
{
	തിരികെ connections_.at(portname);
}

സ്ഥിരം dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() സ്ഥിരം
{
	തിരികെ connections_;
}

തർക്കം RTLIL::Cell::known() സ്ഥിരം
{
	എങ്കിൽ (yosys_celltypes.cell_known(type))
		തിരികെ സത്യം;
	എങ്കിൽ (module && module->design && module->design->module(type))
		തിരികെ സത്യം;
	തിരികെ അസത്യം;
}

തർക്കം RTLIL::Cell::നിവേശം(IdString portname) സ്ഥിരം
{
	എങ്കിൽ (yosys_celltypes.cell_known(type))
		തിരികെ yosys_celltypes.cell_നിവേശം(type.ref(), portname);
	എങ്കിൽ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		തിരികെ w && w->port_നിവേശം;
	}
	തിരികെ അസത്യം;
}

തർക്കം RTLIL::Cell::output(RTLIL::IdString portname) സ്ഥിരം
{
	എങ്കിൽ (yosys_celltypes.cell_known(type))
		തിരികെ yosys_celltypes.cell_output(type, portname);
	എങ്കിൽ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		തിരികെ w && w->port_output;
	}
	തിരികെ അസത്യം;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) സ്ഥിരം
{
	എങ്കിൽ (yosys_celltypes.cell_known(type.ref()))
		തിരികെ yosys_celltypes.cell_port_dir(type, portname);
	എങ്കിൽ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		എങ്കിൽ (m == nullptr)
			തിരികെ PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		എങ്കിൽ (w == nullptr)
			തിരികെ PortDir::PD_UNKNOWN;
		തിരികെ PortDir(w->port_നിവേശം + w->port_output * 2);
	}
	തിരികെ PortDir::PD_UNKNOWN;
}

തർക്കം RTLIL::Cell::hasParam(RTLIL::IdString paramname) സ്ഥിരം
{
	തിരികെ parameters.എണ്ണം(paramname) != 0;
}

ശൂന്യം RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

ശൂന്യം RTLIL::Cell::setParam(RTLIL::IdString paramname, RTLIL::Const മൂല്യം)
{
	parameters[paramname] = std::move(മൂല്യം);
}

സ്ഥിരം RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) സ്ഥിരം
{
	സ്ഥിരം auto &it = parameters.find(paramname);
	എങ്കിൽ (it != parameters.end())
		തിരികെ it->second;
	എങ്കിൽ (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		എങ്കിൽ (m)
			തിരികെ m->parameter_default_മൂല്യങ്ങൾ.at(paramname);
	}
	എറിയുക std::out_of_range("Cell::getParam()");
}

ശൂന്യം RTLIL::Cell::sort()
{
	connections_.sort(sort_by_id_str(module->twines()));
	parameters.sort(sort_by_id_str(module->twines()));
	attributes.sort(sort_by_id_str(module->twines()));
}

ശൂന്യം RTLIL::Cell::check()
{
#ifndef NDEBUG
	InternalCellChecker checker(ശൂന്യ, ഇത്);
	checker.check();
#endif
}

ശൂന്യം RTLIL::Cell::fixup_parameters(തർക്കം set_a_signed, തർക്കം set_b_signed)
{
	എങ്കിൽ (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		തിരികെ;

	എങ്കിൽ (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		എങ്കിൽ (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		തിരികെ;
	}

	എങ്കിൽ (type == ID($demux)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		തിരികെ;
	}

	എങ്കിൽ (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		തിരികെ;
	}

	എങ്കിൽ (type == ID($fa)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		തിരികെ;
	}

	എങ്കിൽ (type == ID($lcu)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::CO]);
		തിരികെ;
	}

	എങ്കിൽ (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);
		തിരികെ;
	}

	തർക്കം signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	എങ്കിൽ (connections_.എണ്ണം(ID::A)) {
		എങ്കിൽ (signedness_ab) {
			എങ്കിൽ (set_a_signed)
				parameters[ID::A_SIGNED] = സത്യം;
			അല്ലെങ്കിൽ എങ്കിൽ (parameters.എണ്ണം(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = അസത്യം;
		}
		parameters[ID::A_WIDTH] = GetSize(connections_[ID::A]);
	}

	എങ്കിൽ (connections_.എണ്ണം(ID::B)) {
		എങ്കിൽ (signedness_ab) {
			എങ്കിൽ (set_b_signed)
				parameters[ID::B_SIGNED] = സത്യം;
			അല്ലെങ്കിൽ എങ്കിൽ (parameters.എണ്ണം(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = അസത്യം;
		}
		parameters[ID::B_WIDTH] = GetSize(connections_[ID::B]);
	}

	എങ്കിൽ (connections_.എണ്ണം(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);

	എങ്കിൽ (connections_.എണ്ണം(ID::Q))
		parameters[ID::WIDTH] = GetSize(connections_[ID::Q]);

	check();
}

തർക്കം RTLIL::Cell::has_keep_attr() സ്ഥിരം {
	തിരികെ get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

തർക്കം RTLIL::Cell::has_memid() സ്ഥിരം
{
	തിരികെ type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

തർക്കം RTLIL::Cell::is_mem_cell() സ്ഥിരം
{
	തിരികെ type.in(ID($mem), ID($mem_v2)) || has_memid();
}

തർക്കം RTLIL::Cell::is_builtin_ff() സ്ഥിരം {
	തിരികെ StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(സ്ഥിരം RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	എങ്കിൽ (wire == ശൂന്യ)
		data = {bit.data};
	അല്ലെങ്കിൽ
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ length) സ്ഥിരം
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= width);
	RTLIL::SigChunk ret;
	എങ്കിൽ (wire) {
		ret.wire = wire;
		ret.offset = ഇത്->offset + offset;
		ret.width = length;
	} അല്ലെങ്കിൽ {
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < length; i++)
			ret.data.തള്ളുക_back(data[offset+i]);
		ret.width = length;
	}
	തിരികെ ret;
}

RTLIL::SigBit RTLIL::SigChunk::operator[](പൂർണ്ണസംഖ്യ offset) സ്ഥിരം
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	എങ്കിൽ (wire) {
		ret.wire = wire;
		ret.offset = ഇത്->offset + offset;
	} അല്ലെങ്കിൽ {
		ret.data = data[offset];
	}
	തിരികെ ret;
}

തർക്കം RTLIL::SigChunk::operator <(സ്ഥിരം RTLIL::SigChunk &other) സ്ഥിരം
{
	എങ്കിൽ (wire && other.wire)
		എങ്കിൽ (wire->name != other.wire->name)
			തിരികെ wire->name < other.wire->name;

	എങ്കിൽ (wire != other.wire)
		തിരികെ wire < other.wire;

	എങ്കിൽ (offset != other.offset)
		തിരികെ offset < other.offset;

	എങ്കിൽ (width != other.width)
		തിരികെ width < other.width;

	തിരികെ data < other.data;
}

തർക്കം RTLIL::SigChunk::operator ==(സ്ഥിരം RTLIL::SigChunk &other) സ്ഥിരം
{
	തിരികെ wire == other.wire && width == other.width && offset == other.offset && data == other.data;
}

തർക്കം RTLIL::SigChunk::operator !=(സ്ഥിരം RTLIL::SigChunk &other) സ്ഥിരം
{
	എങ്കിൽ (*ഇത് == other)
		തിരികെ അസത്യം;
	തിരികെ സത്യം;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.size() > 0);
	auto ie = parts.begin();
	auto it = ie + parts.size() - 1;
	ആകുമ്പോൾ (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(സ്ഥിരം RTLIL::Const &മൂല്യം)
{
	എങ്കിൽ (GetSize(മൂല്യം) != 0) {
		rep_ = CHUNK;
		പുതിയ (&chunk_) RTLIL::SigChunk(മൂല്യം);
	} അല്ലെങ്കിൽ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&മൂല്യം)
{
	എങ്കിൽ (GetSize(മൂല്യം) != 0) {
		rep_ = CHUNK;
		പുതിയ (&chunk_) RTLIL::SigChunk(മൂല്യം);
	} അല്ലെങ്കിൽ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(സ്ഥിരം RTLIL::SigChunk &chunk)
{
	എങ്കിൽ (chunk.width != 0) {
		rep_ = CHUNK;
		പുതിയ (&chunk_) RTLIL::SigChunk(chunk);
	} അല്ലെങ്കിൽ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	എങ്കിൽ (chunk.width != 0) {
		rep_ = CHUNK;
		പുതിയ (&chunk_) RTLIL::SigChunk(chunk);
	} അല്ലെങ്കിൽ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	എങ്കിൽ (wire->width != 0) {
		rep_ = CHUNK;
		പുതിയ (&chunk_) RTLIL::SigChunk(wire);
	} അല്ലെങ്കിൽ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ width)
{
	എങ്കിൽ (width != 0) {
		rep_ = CHUNK;
		പുതിയ (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} അല്ലെങ്കിൽ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(സ്ഥിരം std::string &str)
{
	എങ്കിൽ (str.size() != 0) {
		rep_ = CHUNK;
		പുതിയ (&chunk_) RTLIL::SigChunk(str);
	} അല്ലെങ്കിൽ {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(പൂർണ്ണസംഖ്യ val, പൂർണ്ണസംഖ്യ width)
{
	എങ്കിൽ (width != 0) {
		rep_ = CHUNK;
		പുതിയ (&chunk_) RTLIL::SigChunk(val, width);
	} അല്ലെങ്കിൽ
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, പൂർണ്ണസംഖ്യ width)
{
	എങ്കിൽ (width != 0) {
		rep_ = CHUNK;
		പുതിയ (&chunk_) RTLIL::SigChunk(bit, width);
	} അല്ലെങ്കിൽ
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(സ്ഥിരം RTLIL::SigBit &bit, പൂർണ്ണസംഖ്യ width)
{
	എങ്കിൽ (width != 0) {
		എങ്കിൽ (bit.wire == ശൂന്യ) {
			rep_ = CHUNK;
			പുതിയ (&chunk_) RTLIL::SigChunk(bit.data, width);
		} അല്ലെങ്കിൽ എങ്കിൽ (width == 1) {
			rep_ = CHUNK;
			പുതിയ (&chunk_) RTLIL::SigChunk(bit);
		} അല്ലെങ്കിൽ {
			init_empty_bits();
			bits_.reserve(width);
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++)
				bits_.തള്ളുക_back(bit);
		}
	} അല്ലെങ്കിൽ
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(സ്ഥിരം std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	വേണ്ടി (സ്ഥിരം auto &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(സ്ഥിരം std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	വേണ്ടി (സ്ഥിരം auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(സ്ഥിരം pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	വേണ്ടി (സ്ഥിരം auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(സ്ഥിരം std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	വേണ്ടി (സ്ഥിരം auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(തർക്കം bit)
{
	rep_ = CHUNK;
	പുതിയ (&chunk_) RTLIL::SigChunk(bit ? RTLIL::S1 : RTLIL::S0);
	check();
}

ശൂന്യം RTLIL::SigSpec::Chunks::const_iterator::next_chunk_bits()
{
	പൂർണ്ണസംഖ്യ bits_size = GetSize(spec.bits_);
	എങ്കിൽ (bit_index >= bits_size)
		തിരികെ;
	പൂർണ്ണസംഖ്യ i = bit_index;
	സ്ഥിരം SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.data.clear();
	എങ്കിൽ (bit.is_wire()) {
		chunk.offset = bit.offset;
		ആകുമ്പോൾ (i < bits_size && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_index)
			++i;
	} അല്ലെങ്കിൽ {
		chunk.offset = 0;
		chunk.data.തള്ളുക_back(bit.data);
		ആകുമ്പോൾ (i < bits_size && !spec.bits_[i].is_wire()) {
			chunk.data.തള്ളുക_back(spec.bits_[i].data);
			++i;
		}
	}
	chunk.width = i - bit_index;
}

ശൂന്യം RTLIL::SigSpec::unpack()
{
	എങ്കിൽ (rep_ == BITS)
		തിരികെ;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	പുതിയ (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

ശൂന്യം RTLIL::SigSpec::try_repack()
{
	എങ്കിൽ (rep_ != BITS)
		തിരികെ;

	പൂർണ്ണസംഖ്യ bits_size = GetSize(bits_);
	എങ്കിൽ (bits_size == 0)
		തിരികെ;
	എങ്കിൽ (bits_[0].is_wire()) {
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < bits_size; i++)
			എങ്കിൽ (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				തിരികെ;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_size);
		bits_.~vector();
		rep_ = CHUNK;
		പുതിയ (&chunk_) SigChunk(std::move(chunk));
		തിരികെ;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_size);
	bits.തള്ളുക_back(bits_[0].data);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < bits_size; i++) {
		എങ്കിൽ (bits_[i].is_wire())
			തിരികെ;
		bits.തള്ളുക_back(bits_[i].data);
	}
	bits_.~vector();
	rep_ = CHUNK;
	പുതിയ (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() സ്ഥിരം
{
	Hasher h;
	വേണ്ടി (auto &c : chunks())
		എങ്കിൽ (c.wire == ശൂന്യ) {
			വേണ്ടി (auto &v : c.data)
				h.eat(v);
		} അല്ലെങ്കിൽ {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t result = h.yield();
	എങ്കിൽ (result == 0)
		result = 1;
	hash_.set(result);
	തിരികെ result;
}

ശൂന്യം RTLIL::SigSpec::sort()
{
	unpack();
	std::sort(bits_.begin(), bits_.end());
	hash_.clear();
	try_repack();
}

ശൂന്യം RTLIL::SigSpec::sort_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::vector<SigBit> unique_bits = bits_;
	std::sort(unique_bits.begin(), unique_bits.end());
	auto last = std::unique(unique_bits.begin(), unique_bits.end());
	unique_bits.erase(last, unique_bits.end());

	*ഇത് = unique_bits;
	hash_.clear();
	try_repack();
}

ശൂന്യം RTLIL::SigSpec::replace(സ്ഥിരം RTLIL::SigSpec &pattern, സ്ഥിരം RTLIL::SigSpec &with)
{
	replace(pattern, with, ഇത്);
}

ശൂന്യം RTLIL::SigSpec::replace(സ്ഥിരം RTLIL::SigSpec &pattern, സ്ഥിരം RTLIL::SigSpec &with, RTLIL::SigSpec *other) സ്ഥിരം
{
	log_assert(other != ശൂന്യ);
	log_assert(size() == other->size());
	log_assert(pattern.size() == with.size());

	dict<RTLIL::SigBit, പൂർണ്ണസംഖ്യ> pattern_to_with;
	പൂർണ്ണസംഖ്യ pattern_size = pattern.size();
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < pattern_size; i++) {
		SigBit pattern_bit = pattern[i];
		എങ്കിൽ (pattern_bit.wire != ശൂന്യ) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	പൂർണ്ണസംഖ്യ this_size = size();
	തർക്കം other_modified = അസത്യം;
	വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < this_size; j++) {
		auto it = pattern_to_with.find((*ഇത്)[j]);
		എങ്കിൽ (it != pattern_to_with.end()) {
			other_modified = സത്യം;
			(*other)[j] = with[it->second];
		}
	}

	എങ്കിൽ (other_modified)
		other->try_repack();
	other->check();
}

ശൂന്യം RTLIL::SigSpec::replace(സ്ഥിരം dict<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, ഇത്);
}

ശൂന്യം RTLIL::SigSpec::replace(സ്ഥിരം dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) സ്ഥിരം
{
	log_assert(other != ശൂന്യ);
	log_assert(size() == other->size());

	എങ്കിൽ (rules.empty()) തിരികെ;

	പൂർണ്ണസംഖ്യ this_size = size();
	തർക്കം other_modified = അസത്യം;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < this_size; i++) {
		auto it = rules.find((*ഇത്)[i]);
		എങ്കിൽ (it != rules.end()) {
			other_modified = സത്യം;
			(*other)[i] = it->second;
		}
	}

	എങ്കിൽ (other_modified)
		other->try_repack();
	other->check();
}

ശൂന്യം RTLIL::SigSpec::replace(സ്ഥിരം std::map<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, ഇത്);
}

ശൂന്യം RTLIL::SigSpec::replace(സ്ഥിരം std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) സ്ഥിരം
{
	log_assert(other != ശൂന്യ);
	log_assert(size() == other->size());

	എങ്കിൽ (rules.empty()) തിരികെ;

	പൂർണ്ണസംഖ്യ this_size = size();
	തർക്കം other_modified = അസത്യം;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < this_size; i++) {
		auto it = rules.find((*ഇത്)[i]);
		എങ്കിൽ (it != rules.end()) {
			other_modified = സത്യം;
			(*other)[i] = it->second;
		}
	}

	എങ്കിൽ (other_modified)
		other->try_repack();
	other->check();
}

ശൂന്യം RTLIL::SigSpec::remove(സ്ഥിരം RTLIL::SigSpec &pattern)
{
	remove2(pattern, ശൂന്യ);
}

ശൂന്യം RTLIL::SigSpec::remove(സ്ഥിരം RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) സ്ഥിരം
{
	RTLIL::SigSpec tmp = *ഇത്;
	tmp.remove2(pattern, other);
}

ശൂന്യം RTLIL::SigSpec::remove2(സ്ഥിരം RTLIL::SigSpec &pattern, RTLIL::SigSpec *other)
{
	unpack();
	എങ്കിൽ (other != ശൂന്യ) {
		log_assert(size() == other->size());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.size());
	വേണ്ടി (auto &bit : pattern)
		എങ്കിൽ (bit.wire != ശൂന്യ)
			pattern_bits.തിരുകുക(bit);

	// Compact in-place to avoid O(n^2) erase operations
	size_t write_idx = 0;
	വേണ്ടി (size_t read_idx = 0; read_idx < bits_.size(); read_idx++)
	{
		എങ്കിൽ (!(bits_[read_idx].wire != ശൂന്യ && pattern_bits.എണ്ണം(bits_[read_idx]))) {
			എങ്കിൽ (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				എങ്കിൽ (other != ശൂന്യ)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	തർക്കം modified = (write_idx < bits_.size());
	എങ്കിൽ (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	എങ്കിൽ (other != ശൂന്യ && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ശൂന്യം RTLIL::SigSpec::remove(സ്ഥിരം pool<RTLIL::SigBit> &pattern)
{
	remove2(pattern, ശൂന്യ);
}

ശൂന്യം RTLIL::SigSpec::remove(സ്ഥിരം pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) സ്ഥിരം
{
	RTLIL::SigSpec tmp = *ഇത്;
	tmp.remove2(pattern, other);
}

ശൂന്യം RTLIL::SigSpec::remove2(സ്ഥിരം pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	എങ്കിൽ (other != ശൂന്യ) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	വേണ്ടി (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		എങ്കിൽ (!(bits_[read_idx].wire != ശൂന്യ && pattern.എണ്ണം(bits_[read_idx]))) {
			എങ്കിൽ (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				എങ്കിൽ (other != ശൂന്യ)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	തർക്കം modified = (write_idx < bits_.size());
	എങ്കിൽ (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	എങ്കിൽ (other != ശൂന്യ && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ശൂന്യം RTLIL::SigSpec::remove2(സ്ഥിരം std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	എങ്കിൽ (other != ശൂന്യ) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	വേണ്ടി (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		എങ്കിൽ (!(bits_[read_idx].wire != ശൂന്യ && pattern.എണ്ണം(bits_[read_idx]))) {
			എങ്കിൽ (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				എങ്കിൽ (other != ശൂന്യ)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	തർക്കം modified = (write_idx < bits_.size());
	എങ്കിൽ (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	എങ്കിൽ (other != ശൂന്യ && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ശൂന്യം RTLIL::SigSpec::remove2(സ്ഥിരം pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	എങ്കിൽ (other != ശൂന്യ) {
		log_assert(size() == other->size());
		other->unpack();
	}

	തർക്കം modified = അസത്യം;
	തർക്കം other_modified = അസത്യം;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = GetSize(bits_) - 1; i >= 0; i--) {
		എങ്കിൽ (bits_[i].wire != ശൂന്യ && pattern.എണ്ണം(bits_[i].wire)) {
			modified = സത്യം;
			bits_.erase(bits_.begin() + i);
			എങ്കിൽ (other != ശൂന്യ) {
				other_modified = സത്യം;
				other->bits_.erase(other->bits_.begin() + i);
			}
		}
	}

	എങ്കിൽ (modified) {
		hash_.clear();
		try_repack();
	}
	എങ്കിൽ (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(സ്ഥിരം RTLIL::SigSpec &pattern, സ്ഥിരം RTLIL::SigSpec *other) സ്ഥിരം
{
	log_assert(other == ശൂന്യ || size() == other->size());

	RTLIL::SigSpec ret;
	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();

	വേണ്ടി (auto& pattern_chunk : pattern.chunks()) {
		എങ്കിൽ (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			പൂർണ്ണസംഖ്യ i = 0;
			വേണ്ടി (സ്ഥിരം RTLIL::SigBit &bit : bits_match) {
				എങ്കിൽ (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} അല്ലെങ്കിൽ {
			വേണ്ടി (സ്ഥിരം RTLIL::SigBit &bit : bits_match)
				എങ്കിൽ (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	തിരികെ ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(സ്ഥിരം pool<RTLIL::SigBit> &pattern, സ്ഥിരം RTLIL::SigSpec *other) സ്ഥിരം
{
	log_assert(other == ശൂന്യ || size() == other->size());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	എങ്കിൽ (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		പൂർണ്ണസംഖ്യ i = 0;
		വേണ്ടി (സ്ഥിരം RTLIL::SigBit &bit : bits_match) {
			എങ്കിൽ (bit.wire && pattern.എണ്ണം(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} അല്ലെങ്കിൽ {
		വേണ്ടി (സ്ഥിരം RTLIL::SigBit &bit : bits_match)
			എങ്കിൽ (bit.wire && pattern.എണ്ണം(bit))
				ret.append(bit);
	}

	ret.check();
	തിരികെ ret;
}

ശൂന്യം RTLIL::SigSpec::replace(പൂർണ്ണസംഖ്യ offset, സ്ഥിരം RTLIL::SigSpec &with)
{
	എങ്കിൽ (with.size() == 0)
		തിരികെ;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.size() >= 0);
	log_assert(offset+with.size() <= size());

	പൂർണ്ണസംഖ്യ i = 0;
	വേണ്ടി (സ്ഥിരം RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

ശൂന്യം RTLIL::SigSpec::remove_const()
{
	എങ്കിൽ (rep_ == CHUNK)
	{
		എങ്കിൽ (chunk_.wire == ശൂന്യ) {
			chunk_.~SigChunk();
			init_empty_bits();
			hash_.clear();
		}
	}
	അല്ലെങ്കിൽ
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.size());
		വേണ്ടി (auto &bit : bits_)
			എങ്കിൽ (bit.wire != ശൂന്യ)
				new_bits.തള്ളുക_back(bit);
		എങ്കിൽ (GetSize(new_bits) != GetSize(bits_)) {
			bits_.swap(new_bits);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

ശൂന്യം RTLIL::SigSpec::remove(പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ length)
{
	എങ്കിൽ (length == 0)
		തിരികെ;

	unpack();

	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	bits_.erase(bits_.begin() + offset, bits_.begin() + offset + length);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ length) സ്ഥിരം
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	std::vector<SigBit> extracted;
	SigBit first;
	തർക്കം is_packing = സത്യം;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = offset; i < offset + length; i++) {
		തർക്കം was_packing_before = is_packing;
		SigBit bit = (*ഇത്)[i];
		എങ്കിൽ (i == offset) {
			first = bit;
			എങ്കിൽ (!bit.wire)
				is_packing = അസത്യം;
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (bit.wire != first.wire)
				is_packing = അസത്യം;
			എങ്കിൽ (bit.wire)
				എങ്കിൽ (bit.offset != first.offset + (i - offset))
					is_packing = അസത്യം;
		}
		എങ്കിൽ (was_packing_before && !is_packing)
			വേണ്ടി (പൂർണ്ണസംഖ്യ j = offset; j < i; j++)
				extracted.തള്ളുക_back((*ഇത്)[j]);
		എങ്കിൽ (!is_packing)
			extracted.തള്ളുക_back((*ഇത്)[i]);
	}
	എങ്കിൽ (is_packing)
		തിരികെ SigChunk(first.wire, first.offset, length);

	തിരികെ extracted;
}

ശൂന്യം RTLIL::SigSpec::rewrite_wires(std::function<ശൂന്യം(RTLIL::Wire*& wire)> rewrite)
{
	എങ്കിൽ (rep_ == CHUNK) {
		എങ്കിൽ (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		തിരികെ;
	}

	std::vector<RTLIL::SigBit> new_bits;
	വേണ്ടി (സ്ഥിരം RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		എങ്കിൽ (c.wire != nullptr)
			rewrite(c.wire);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.clear();
}

ശൂന്യം RTLIL::SigSpec::append(സ്ഥിരം RTLIL::SigSpec &സൂചന)
{
	എങ്കിൽ (സൂചന.size() == 0)
		തിരികെ;

	എങ്കിൽ (size() == 0) {
		*ഇത് = സൂചന;
		തിരികെ;
	}

	hash_.clear();
	എങ്കിൽ (rep_ == CHUNK && സൂചന.rep_ == CHUNK && chunk_.wire == സൂചന.chunk_.wire) {
		എങ്കിൽ (chunk_.wire == ശൂന്യ) {
			chunk_.data.തിരുകുക(chunk_.data.end(), സൂചന.chunk_.data.begin(), സൂചന.chunk_.data.end());
			chunk_.width = GetSize(chunk_.data);
			തിരികെ;
		}
		എങ്കിൽ (chunk_.offset + chunk_.width == സൂചന.chunk_.offset) {
			chunk_.width += സൂചന.chunk_.width;
			തിരികെ;
		}
	}

	unpack();
	വേണ്ടി (സ്ഥിരം SigBit &bit : സൂചന.bits())
		bits_.തള്ളുക_back(bit);
	check();
}

ശൂന്യം RTLIL::SigSpec::append(സ്ഥിരം RTLIL::SigBit &bit)
{
	hash_.clear();

	എങ്കിൽ (size() == 0) {
		destroy();
		rep_ = CHUNK;
		പുതിയ (&chunk_) RTLIL::SigChunk(bit);
		തിരികെ;
	}

	എങ്കിൽ (rep_ == CHUNK && chunk_.wire == bit.wire) {
		എങ്കിൽ (chunk_.wire == ശൂന്യ) {
			chunk_.data.തള്ളുക_back(bit.data);
			chunk_.width++;
			തിരികെ;
		}
		എങ്കിൽ (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			തിരികെ;
		}
	}

	unpack();

	bits_.തള്ളുക_back(bit);
	check();
}

ശൂന്യം RTLIL::SigSpec::extend_u0(പൂർണ്ണസംഖ്യ width, തർക്കം is_signed)
{
	എങ്കിൽ (size() > width)
		remove(width, size() - width);

	എങ്കിൽ (size() < width) {
		RTLIL::SigBit padding = size() > 0 ? (*ഇത്)[size() - 1] : RTLIL::State::Sx;
		എങ്കിൽ (!is_signed)
			padding = RTLIL::State::S0;
		ആകുമ്പോൾ (size() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(പൂർണ്ണസംഖ്യ അക്കം) സ്ഥിരം
{
	RTLIL::SigSpec sig;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം; i++)
		sig.append(*ഇത്);
	തിരികെ sig;
}

#ifndef NDEBUG
ശൂന്യം RTLIL::SigSpec::check(സ്ഥിരം Module *mod) സ്ഥിരം
{
	എങ്കിൽ (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		എങ്കിൽ (chunk_.wire == ശൂന്യ) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.data.size() == (size_t)chunk_.width);
		} അല്ലെങ്കിൽ {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.data.size() == 0);
			എങ്കിൽ (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	അല്ലെങ്കിൽ എങ്കിൽ (size() <= 64)
	{
		എങ്കിൽ (mod != nullptr) {
			വേണ്ടി (സ്ഥിരം RTLIL::SigBit &bit : bits_)
				എങ്കിൽ (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#endif

തർക്കം RTLIL::SigSpec::operator <(സ്ഥിരം RTLIL::SigSpec &other) സ്ഥിരം
{
	എങ്കിൽ (ഇത് == &other)
		തിരികെ അസത്യം;

	എങ്കിൽ (size() != other.size())
		തിരികെ size() < other.size();

	auto other_it = other.chunks().begin();
	വേണ്ടി (സ്ഥിരം SigChunk &c : chunks()) {
		എങ്കിൽ (c != *other_it)
			തിരികെ c < *other_it;
		++other_it;
	}

	തിരികെ അസത്യം;
}

തർക്കം RTLIL::SigSpec::operator ==(സ്ഥിരം RTLIL::SigSpec &other) സ്ഥിരം
{
	എങ്കിൽ (ഇത് == &other)
		തിരികെ സത്യം;

	എങ്കിൽ (size() != other.size())
		തിരികെ അസത്യം;

	auto other_it = other.chunks().begin();
	വേണ്ടി (സ്ഥിരം SigChunk &c : chunks()) {
		എങ്കിൽ (c != *other_it)
			തിരികെ അസത്യം;
		++other_it;
	}

	തിരികെ സത്യം;
}

തർക്കം RTLIL::SigSpec::is_wire() സ്ഥിരം
{
	Chunks cs = chunks();
	auto it = cs.begin();
	എങ്കിൽ (it == cs.end())
		തിരികെ അസത്യം;
	സ്ഥിരം RTLIL::SigChunk &chunk = *it;
	തിരികെ chunk.wire && chunk.wire->width == size() && ++it == cs.end();
}

തർക്കം RTLIL::SigSpec::is_chunk() സ്ഥിരം
{
	Chunks cs = chunks();
	auto it = cs.begin();
	എങ്കിൽ (it == cs.end())
		തിരികെ അസത്യം;
	തിരികെ ++it == cs.end();
}

തർക്കം RTLIL::SigSpec::known_driver() സ്ഥിരം
{
	വേണ്ടി (auto &chunk : chunks())
		എങ്കിൽ (chunk.is_wire() && !chunk.wire->known_driver())
			തിരികെ അസത്യം;
	തിരികെ സത്യം;
}

തർക്കം RTLIL::SigSpec::is_fully_const() സ്ഥിരം
{
	വേണ്ടി (auto &chunk : chunks())
		എങ്കിൽ (chunk.width > 0 && chunk.wire != ശൂന്യ)
			തിരികെ അസത്യം;
	തിരികെ സത്യം;
}

തർക്കം RTLIL::SigSpec::is_fully_zero() സ്ഥിരം
{
	വേണ്ടി (auto &chunk : chunks()) {
		എങ്കിൽ (chunk.width > 0 && chunk.wire != ശൂന്യ)
			തിരികെ അസത്യം;
		വേണ്ടി (RTLIL::State d : chunk.data)
			എങ്കിൽ (d != RTLIL::State::S0)
				തിരികെ അസത്യം;
	}
	തിരികെ സത്യം;
}

തർക്കം RTLIL::SigSpec::is_fully_ones() സ്ഥിരം
{
	വേണ്ടി (auto &chunk : chunks()) {
		എങ്കിൽ (chunk.width > 0 && chunk.wire != ശൂന്യ)
			തിരികെ അസത്യം;
		വേണ്ടി (RTLIL::State d : chunk.data)
			എങ്കിൽ (d != RTLIL::State::S1)
				തിരികെ അസത്യം;
	}
	തിരികെ സത്യം;
}

തർക്കം RTLIL::SigSpec::is_fully_def() സ്ഥിരം
{
	വേണ്ടി (auto &chunk : chunks()) {
		എങ്കിൽ (chunk.width > 0 && chunk.wire != ശൂന്യ)
			തിരികെ അസത്യം;
		വേണ്ടി (RTLIL::State d : chunk.data)
			എങ്കിൽ (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				തിരികെ അസത്യം;
	}
	തിരികെ സത്യം;
}

തർക്കം RTLIL::SigSpec::is_fully_undef() സ്ഥിരം
{
	വേണ്ടി (auto &chunk : chunks()) {
		എങ്കിൽ (chunk.width > 0 && chunk.wire != ശൂന്യ)
			തിരികെ അസത്യം;
		വേണ്ടി (RTLIL::State d : chunk.data)
			എങ്കിൽ (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				തിരികെ അസത്യം;
	}
	തിരികെ സത്യം;
}

തർക്കം RTLIL::SigSpec::has_const() സ്ഥിരം
{
	വേണ്ടി (auto &chunk : chunks())
		എങ്കിൽ (chunk.width > 0 && chunk.wire == ശൂന്യ)
			തിരികെ സത്യം;
	തിരികെ അസത്യം;
}

തർക്കം RTLIL::SigSpec::has_const(State state) സ്ഥിരം
{
	വേണ്ടി (auto &chunk : chunks())
		എങ്കിൽ (chunk.width > 0 && chunk.wire == ശൂന്യ && std::find(chunk.data.begin(), chunk.data.end(), state) != chunk.data.end())
			തിരികെ സത്യം;
	തിരികെ അസത്യം;
}


തർക്കം RTLIL::SigSpec::has_marked_bits() സ്ഥിരം
{
	വേണ്ടി (auto &chunk : chunks())
		എങ്കിൽ (chunk.width > 0 && chunk.wire == ശൂന്യ) {
			വേണ്ടി (RTLIL::State d : chunk.data)
				എങ്കിൽ (d == RTLIL::State::Sm)
					തിരികെ സത്യം;
		}
	തിരികെ അസത്യം;
}

തർക്കം RTLIL::SigSpec::is_onehot(പൂർണ്ണസംഖ്യ *pos) സ്ഥിരം
{
	എങ്കിൽ (std::optional<RTLIL::Const> c = try_as_const())
		തിരികെ c->is_onehot(pos);
	തിരികെ അസത്യം;
}

തർക്കം RTLIL::SigSpec::as_bool() സ്ഥിരം
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_മൂല്യം());
	തിരികെ c->as_bool();
}

പൂർണ്ണസംഖ്യ RTLIL::SigSpec::as_int(തർക്കം is_signed) സ്ഥിരം
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_മൂല്യം());
	തിരികെ c->as_int(is_signed);
}

തർക്കം RTLIL::SigSpec::convertible_to_int(തർക്കം is_signed) സ്ഥിരം
{
	std::optional<RTLIL::Const> c = try_as_const();
	എങ്കിൽ (!c.has_മൂല്യം())
		തിരികെ അസത്യം;
	തിരികെ c->convertible_to_int(is_signed);
}

std::optional<പൂർണ്ണസംഖ്യ> RTLIL::SigSpec::try_as_int(തർക്കം is_signed) സ്ഥിരം
{
	std::optional<RTLIL::Const> c = try_as_const();
	എങ്കിൽ (!c.has_മൂല്യം())
		തിരികെ std::nullopt;
	തിരികെ c->try_as_int(is_signed);
}

പൂർണ്ണസംഖ്യ RTLIL::SigSpec::as_int_saturating(തർക്കം is_signed) സ്ഥിരം
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_മൂല്യം());
	തിരികെ c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() സ്ഥിരം
{
	std::string str;
	str.reserve(size());
	std::vector<RTLIL::SigChunk> chunks = *ഇത്;
	വേണ്ടി (size_t i = chunks.size(); i > 0; i--) {
		സ്ഥിരം RTLIL::SigChunk &chunk = chunks[i-1];
		എങ്കിൽ (chunk.wire != ശൂന്യ)
			str.append(chunk.width, '?');
		അല്ലെങ്കിൽ
			str += RTLIL::Const(chunk.data).as_string();
	}
	തിരികെ str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() സ്ഥിരം
{
	Chunks cs = chunks();
	auto it = cs.begin();
	എങ്കിൽ (it == cs.end())
		തിരികെ RTLIL::Const();
	SigChunk chunk = *it;
	എങ്കിൽ (chunk.wire != ശൂന്യ || ++it != cs.end())
		തിരികെ std::nullopt;
	തിരികെ RTLIL::Const(std::move(chunk.data));
}

RTLIL::Const RTLIL::SigSpec::as_const() സ്ഥിരം
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_മൂല്യം());
	തിരികെ *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() സ്ഥിരം
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == size());
	തിരികെ chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() സ്ഥിരം
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	തിരികെ chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() സ്ഥിരം
{
	തിരികെ RTLIL::SigBit(*ഇത്);
}

തർക്കം RTLIL::SigSpec::match(സ്ഥിരം അക്ഷരം* pattern) സ്ഥിരം
{
	പൂർണ്ണസംഖ്യ pattern_len = strlen(pattern);
	log_assert(pattern_len == size());

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < pattern_len; ++i) {
		അക്ഷരം ch = pattern[i];
		എങ്കിൽ (ch == ' ')
			തുടരുക;
		RTLIL::SigBit bit = (*ഇത്)[pattern_len - 1 - i];
		എങ്കിൽ (ch == '*') {
			എങ്കിൽ (bit != State::Sz && bit != State::Sx)
				തിരികെ അസത്യം;
			തുടരുക;
		}
		എങ്കിൽ (ch == '0') {
			എങ്കിൽ (bit != State::S0)
				തിരികെ അസത്യം;
		} അല്ലെങ്കിൽ
		എങ്കിൽ (ch == '1') {
			എങ്കിൽ (bit != State::S1)
				തിരികെ അസത്യം;
		} അല്ലെങ്കിൽ
			log_abort();
	}

	തിരികെ സത്യം;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() സ്ഥിരം
{
	std::set<RTLIL::SigBit> sigbits;
	വേണ്ടി (auto &c : chunks())
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < c.width; i++)
			sigbits.തിരുകുക(RTLIL::SigBit(c, i));
	തിരികെ sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() സ്ഥിരം
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(size());
	വേണ്ടി (auto &c : chunks())
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < c.width; i++)
			sigbits.തിരുകുക(RTLIL::SigBit(c, i));
	തിരികെ sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() സ്ഥിരം
{
	std::vector<RTLIL::SigBit> result;
	result.reserve(size());
	വേണ്ടി (SigBit bit : *ഇത്)
		result.തള്ളുക_back(bit);
	തിരികെ result;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(സ്ഥിരം RTLIL::SigSpec &other) സ്ഥിരം
{
	പൂർണ്ണസംഖ്യ this_size = size();
	log_assert(this_size == other.size());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < this_size; i++)
		new_map[(*ഇത്)[i]] = other[i];

	തിരികെ new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(സ്ഥിരം RTLIL::SigSpec &other) സ്ഥിരം
{
	പൂർണ്ണസംഖ്യ this_size = size();
	log_assert(this_size == other.size());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_size);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < this_size; i++)
		new_map[(*ഇത്)[i]] = other[i];

	തിരികെ new_map;
}

സ്ഥിതം ശൂന്യം sigspec_parse_split(std::vector<std::string> &tokens, സ്ഥിരം std::string &text, അക്ഷരം sep)
{
	size_t start = 0, end = 0;
	ആകുമ്പോൾ ((end = text.find(sep, start)) != std::string::npos) {
		tokens.തള്ളുക_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.തള്ളുക_back(text.substr(start));
}

സ്ഥിതം RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, സ്ഥിരം std::string &netname)
{
	തിരികെ module->wire(netname);
}

തർക്കം RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	വേണ്ടി (പൂർണ്ണസംഖ്യ tokidx = പൂർണ്ണസംഖ്യ(tokens.size())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		എങ്കിൽ (netname.size() == 0)
			തുടരുക;

		എങ്കിൽ (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			auto ast = p.const2ast(netname);
			എങ്കിൽ (ast == nullptr)
				തിരികെ അസത്യം;
			sig.append(RTLIL::Const(ast->bits));
			തുടരുക;
		}

		എങ്കിൽ (module == ശൂന്യ)
			തിരികെ അസത്യം;

		എങ്കിൽ (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		എങ്കിൽ (sigspec_parse_wire(module, netname) == nullptr) {
			size_t indices_pos = netname.size()-1;
			എങ്കിൽ (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				ആകുമ്പോൾ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				എങ്കിൽ (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					ആകുമ്പോൾ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				എങ്കിൽ (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		എങ്കിൽ (wire == nullptr)
			തിരികെ അസത്യം;

		എങ്കിൽ (!indices.empty()) {
			std::vector<std::string> index_tokens;
			sigspec_parse_split(index_tokens, indices.substr(1, indices.size()-2), ':');
			എങ്കിൽ (index_tokens.size() == 1) {
				പൂർണ്ണസംഖ്യ a = atoi(index_tokens.at(0).c_str());
				എങ്കിൽ (a < 0 || a >= wire->width)
					തിരികെ അസത്യം;
				sig.append(RTLIL::SigSpec(wire, a));
			} അല്ലെങ്കിൽ {
				പൂർണ്ണസംഖ്യ a = atoi(index_tokens.at(0).c_str());
				പൂർണ്ണസംഖ്യ b = atoi(index_tokens.at(1).c_str());
				എങ്കിൽ (a > b) {
					പൂർണ്ണസംഖ്യ tmp = a;
					a = b, b = tmp;
				}
				എങ്കിൽ (a < 0 || a >= wire->width)
					തിരികെ അസത്യം;
				എങ്കിൽ (b < 0 || b >= wire->width)
					തിരികെ അസത്യം;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} അല്ലെങ്കിൽ
			sig.append(wire);
	}

	തിരികെ സത്യം;
}

തർക്കം RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	എങ്കിൽ (str.empty() || str[0] != '@')
		തിരികെ parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	എങ്കിൽ (sel_name == IdString::Null || design->selection_vars.എണ്ണം(sel_name) == 0)
		തിരികെ അസത്യം;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	വേണ്ടി (auto &it : module->wires_)
		എങ്കിൽ (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	തിരികെ സത്യം;
}

തർക്കം RTLIL::SigSpec::parse_rhs(സ്ഥിരം RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	എങ്കിൽ (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.size());
		തിരികെ സത്യം;
	}

	എങ്കിൽ (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.size());
		തിരികെ സത്യം;
	}

	എങ്കിൽ (lhs.is_chunk()) {
		അക്ഷരം *p = (അക്ഷരം*)str.c_str(), *endptr;
		long പൂർണ്ണസംഖ്യ val = strtol(p, &endptr, 10);
		എങ്കിൽ (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.size());
			തിരികെ സത്യം;
		}
	}

	എങ്കിൽ (!parse(sig, module, str))
		തിരികെ അസത്യം;
	എങ്കിൽ (sig.size() > lhs.size())
		sig.remove(lhs.size(), sig.size() - lhs.size());
	തിരികെ സത്യം;
}

RTLIL::SigSpec::operator std::vector<RTLIL::SigChunk>() സ്ഥിരം
{
	std::vector<RTLIL::SigChunk> result;
	വേണ്ടി (സ്ഥിരം RTLIL::SigChunk &c : chunks())
		result.തള്ളുക_back(c);
	തിരികെ result;
}

RTLIL::CaseRule::~CaseRule()
{
	വേണ്ടി (auto it = switches.begin(); it != switches.end(); it++)
		delete *it;
}

തർക്കം RTLIL::CaseRule::empty() സ്ഥിരം
{
	തിരികെ actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() സ്ഥിരം
{
	RTLIL::CaseRule *new_caserule = പുതിയ RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	വേണ്ടി (auto &it : switches)
		new_caserule->switches.തള്ളുക_back(it->clone());
	തിരികെ new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	വേണ്ടി (auto it = cases.begin(); it != cases.end(); it++)
		delete *it;
}

തർക്കം RTLIL::SwitchRule::empty() സ്ഥിരം
{
	തിരികെ cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() സ്ഥിരം
{
	RTLIL::SwitchRule *new_switchrule = പുതിയ RTLIL::SwitchRule;
	new_switchrule->സൂചന = സൂചന;
	new_switchrule->attributes = attributes;
	വേണ്ടി (auto &it : cases)
		new_switchrule->cases.തള്ളുക_back(it->clone());
	തിരികെ new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() സ്ഥിരം
{
	RTLIL::SyncRule *new_syncrule = പുതിയ RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->സൂചന = സൂചന;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	തിരികെ new_syncrule;
}

RTLIL::Process::~Process()
{
	വേണ്ടി (auto it = syncs.begin(); it != syncs.end(); it++)
		delete *it;
}

RTLIL::Process *RTLIL::Process::clone() സ്ഥിരം
{
	RTLIL::Process *new_proc = പുതിയ RTLIL::Process;

	new_proc->attributes = attributes;

	RTLIL::CaseRule *rc_ptr = വേര്_case.clone();
	new_proc->വേര്_case = *rc_ptr;
	rc_ptr->switches.clear();
	delete rc_ptr;

	വേണ്ടി (auto &it : syncs)
		new_proc->syncs.തള്ളുക_back(it->clone());

	തിരികെ new_proc;
}

RTLIL::Memory::~Memory()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
സ്ഥിതം std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Memory*> all_memorys;
std::map<unsigned പൂർണ്ണസംഖ്യ, RTLIL::Memory*> *RTLIL::Memory::get_all_memorys(ശൂന്യം)
{
	തിരികെ &all_memorys;
}
#endif
YOSYS_NAMESPACE_END
