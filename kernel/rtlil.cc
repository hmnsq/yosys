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

#ଅନ୍ତର୍ଭାଵ "kernel/rtlil.h"
#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "kernel/macc.h"
#ଅନ୍ତର୍ଭାଵ "kernel/newcelltypes.h"
#ଅନ୍ତର୍ଭାଵ "kernel/sigtools.h"
#ଅନ୍ତର୍ଭାଵ "kernel/threading.h"
#ଅନ୍ତର୍ଭାଵ "frontends/verilog/verilog_frontend.h"
#ଅନ୍ତର୍ଭାଵ "frontends/verilog/preproc.h"
#ଅନ୍ତର୍ଭାଵ "backends/rtlil/rtlil_backend.h"

#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <algorithm>
#ଅନ୍ତର୍ଭାଵ <charconv>
#ଅନ୍ତର୍ଭାଵ <optional>
#ଅନ୍ତର୍ଭାଵ <string_view>
#ଅନ୍ତର୍ଭାଵ <sstream>

YOSYS_NAMESPACE_BEGIN

dict<std::string, std::string> RTLIL::constpad;

ଅଚଳ pool<IdString> &RTLIL::builtin_ff_cell_types() {
	ସ୍ଥିର ଅଚଳ pool<IdString> res = []() {
		pool<IdString> r;
		ପାଇଁ (size_t i = 0; i < StaticCellTypes::builder.ଗଣନା_2; i++) {
			auto &cell = StaticCellTypes::builder.cells[i];
			ଯଦି (cell.features.is_ff)
				r.ଭର୍ତ୍ତି(cell.type);
		}
		ଫେରନ୍ତୁ r;
	}();
	ଫେରନ୍ତୁ res;
}

#define check(condition) log_assert(condition && "malformed Const union")

ଅଚଳ Const::bitvectype& Const::get_bits() ଅଚଳ {
	check(is_bits());
	ଫେରନ୍ତୁ *get_if_bits();
}

ଅଚଳ std::string& Const::get_str() ଅଚଳ {
	check(is_str());
	ଫେରନ୍ତୁ *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	ଫେରନ୍ତୁ *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	ଫେରନ୍ତୁ *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	ନୂତନ ((ଶୂନ୍ୟ*)&str_) std::string(std::move(str));
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	ଅକ୍ଷର bytes[] = {
		(ଅକ୍ଷର)(val >> 24), (ଅକ୍ଷର)(val >> 16), (ଅକ୍ଷର)(val >> 8), (ଅକ୍ଷର)val
	};
	ନୂତନ ((ଶୂନ୍ୟ*)&str_) std::string(bytes, 4);
	tag = backing_tag::string;
}

RTLIL::Const::Const(long long val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	ଯଦି ((width & 7) == 0) {
		ନୂତନ ((ଶୂନ୍ୟ*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା bytes = width >> 3;
		signed ଅକ୍ଷର sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(bytes, ଆକାର(val));
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < bytes; i++) {
			str[str.size() - 1 - i] = val;
			val = val >> 8;
		}
		ଫେରନ୍ତୁ;
	}

	ନୂତନ ((ଶୂନ୍ୟ*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++) {
		bv.ଠେଲ_back((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	ନୂତନ ((ଶୂନ୍ୟ*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++)
		bv.ଠେଲ_back(bit);
}

RTLIL::Const::Const(ଅଚଳ std::vector<ତାର୍କିକ> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	ନୂତନ ((ଶୂନ୍ୟ*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.size());
	ପାଇଁ (ଅଚଳ auto &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ଅଚଳ RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	ଯଦି (is_str())
		ନୂତନ ((ଶୂନ୍ୟ*)&str_) std::string(other.get_str());
	ଅନ୍ୟଥା ଯଦି (is_bits())
		ନୂତନ ((ଶୂନ୍ୟ*)&bits_) bitvectype(other.get_bits());
	ଅନ୍ୟଥା
		check(ମିଥ୍ୟା);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	ଯଦି (is_str())
		ନୂତନ ((ଶୂନ୍ୟ*)&str_) std::string(std::move(other.get_str()));
	ଅନ୍ୟଥା ଯଦି (is_bits())
		ନୂତନ ((ଶୂନ୍ୟ*)&bits_) bitvectype(std::move(other.get_bits()));
	ଅନ୍ୟଥା
		check(ମିଥ୍ୟା);
}

RTLIL::Const &RTLIL::Const::operator =(ଅଚଳ RTLIL::Const &other) {
	flags = other.flags;
	ଯଦି (other.is_str()) {
		ଯଦି (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(ଶୂନ୍ୟ)ନୂତନ ((ଶୂନ୍ୟ*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} ଅନ୍ୟଥା ଯଦି (other.is_bits()) {
		ଯଦି (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(ଶୂନ୍ୟ)ନୂତନ ((ଶୂନ୍ୟ*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} ଅନ୍ୟଥା {
		check(ମିଥ୍ୟା);
	}
	ଫେରନ୍ତୁ *ଏହି;
}

RTLIL::Const::~Const() {
	ଯଦି (is_bits())
		bits_.~bitvectype();
	ଅନ୍ୟଥା ଯଦି (is_str())
		str_.~string();
	ଅନ୍ୟଥା
		check(ମିଥ୍ୟା);
}

ତାର୍କିକ RTLIL::Const::operator<(ଅଚଳ RTLIL::Const &other) ଅଚଳ
{
	ଯଦି (size() != other.size())
		ଫେରନ୍ତୁ size() < other.size();

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < size(); i++)
		ଯଦି ((*ଏହି)[i] != other[i])
			ଫେରନ୍ତୁ (*ଏହି)[i] < other[i];

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ RTLIL::Const::operator ==(ଅଚଳ RTLIL::Const &other) ଅଚଳ
{
	ଯଦି (is_str() && other.is_str())
		ଫେରନ୍ତୁ get_str() == other.get_str();
	ଯଦି (is_bits() && other.is_bits())
		ଫେରନ୍ତୁ get_bits() == other.get_bits();

	ଯଦି (size() != other.size())
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < size(); i++)
	ଯଦି ((*ଏହି)[i] != other[i])
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::Const::operator !=(ଅଚଳ RTLIL::Const &other) ଅଚଳ
{
	ଫେରନ୍ତୁ !(*ଏହି == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	ଫେରନ୍ତୁ get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() ଅଚଳ
{
	std::vector<State> v;
	ପାଇଁ (auto bit : *ଏହି)
		v.ଠେଲ_back(bit);
	ଫେରନ୍ତୁ v;
}

ତାର୍କିକ RTLIL::Const::as_bool() ଅଚଳ
{
	ଯଦି (is_str()) {
		ପାଇଁ (ଅକ୍ଷର ch : get_str())
			ଯଦି (ch != 0)
				ଫେରନ୍ତୁ ସତ୍ୟ;
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଅଚଳ bitvectype& bv = get_bits();
	ପାଇଁ (size_t i = 0; i < bv.size(); i++)
		ଯଦି (bv[i] == State::S1)
			ଫେରନ୍ତୁ ସତ୍ୟ;
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା RTLIL::Const::as_int(ତାର୍କିକ is_signed) ଅଚଳ
{
	int32_t ret = 0;
	ଯଦି (ଅଚଳ std::string *s = get_if_str()) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = GetSize(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = std::min(4, size); i > 0; i--)
			ret |= static_cast<unsigned ଅକ୍ଷର>((*s)[size - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		ଯଦି (is_signed && size > 0 && size < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << size*8;
		ଫେରନ୍ତୁ ret;
	}

	ଅଚଳ bitvectype& bv = get_bits();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା significant_bits = std::min(GetSize(bv), 32);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < significant_bits; i++)
		ଯଦି (bv[i] == State::S1)
			ret |= 1 << i;
	ଯଦି (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	ଫେରନ୍ତୁ ret;
}

ତାର୍କିକ RTLIL::Const::convertible_to_int(ତାର୍କିକ is_signed) ଅଚଳ
{
	auto size = get_min_size(is_signed);

	ଯଦି (size < 0)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	// If it fits in 31 bits it is definitely convertible
	ଯଦି (size <= 31)
		ଫେରନ୍ତୁ ସତ୍ୟ;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	ଯଦି (size == 32) {
		ଯଦି (is_signed)
			ଫେରନ୍ତୁ ସତ୍ୟ;
		ଫେରନ୍ତୁ (*ଏହି)[size - 1] != State::S1;
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

std::optional<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> RTLIL::Const::try_as_int(ତାର୍କିକ is_signed) ଅଚଳ
{
	ଯଦି (!convertible_to_int(is_signed))
		ଫେରନ୍ତୁ std::nullopt;
	ଫେରନ୍ତୁ as_int(is_signed);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା RTLIL::Const::as_int_saturating(ତାର୍କିକ is_signed) ଅଚଳ
{
	ଯଦି (!convertible_to_int(is_signed)) {
		ଯଦି (!is_signed)
			ଫେରନ୍ତୁ std::numeric_limits<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>::max();

		ଅଚଳ auto min_size = get_min_size(is_signed);
		log_assert(min_size > 0);
		ଅଚଳ auto neg = (*ଏହି)[min_size - 1];
		ଫେରନ୍ତୁ neg ? std::numeric_limits<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>::min() : std::numeric_limits<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>::max();
	}
	ଫେରନ୍ତୁ as_int(is_signed);
}

ଶୂନ୍ୟ RTLIL::Const::tag_bare_integer_const(ଅଚଳ std::string &ମୂଲ୍ୟ)
{
	ଯଦି (ମୂଲ୍ୟ.empty() || ମୂଲ୍ୟ.find('\'') != std::string::npos)
		ଫେରନ୍ତୁ;
	size_t start = (ମୂଲ୍ୟ[0] == '-' || ମୂଲ୍ୟ[0] == '+') ? 1 : 0;
	ଯଦି (start < ମୂଲ୍ୟ.size() && std::all_of(ମୂଲ୍ୟ.begin() + start, ମୂଲ୍ୟ.end(), ::isdigit))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା RTLIL::Const::get_min_size(ତାର୍କିକ is_signed) ଅଚଳ
{
	ଯଦି (empty()) ଫେରନ୍ତୁ 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	ଯଦି (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	ଅନ୍ୟଥା
		leading_bit = RTLIL::State::S0;

	auto idx = size();
	ଯେତେବେଳେ (idx > 0 && (*ଏହି)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	ଯଦି (is_signed && idx < size()) {
		idx++;
	}
	// must be at least one bit
	ଫେରନ୍ତୁ (idx == 0) ? 1 : idx;
}

ଶୂନ୍ୟ RTLIL::Const::compress(ତାର୍କିକ is_signed)
{
	auto idx = get_min_size(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> RTLIL::Const::as_int_compress(ତାର୍କିକ is_signed) ଅଚଳ
{
	ଫେରନ୍ତୁ try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ଅଚଳ ଅକ୍ଷର* any) ଅଚଳ
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା sz = size();
	std::string ret;
	ret.reserve(sz);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = sz - 1; i >= 0; --i)
		ବାଛ ((*ଏହି)[i]) {
			ସ୍ଥିତି S0: ret.ଠେଲ_back('0'); ଭାଙ୍ଗ;
			ସ୍ଥିତି S1: ret.ଠେଲ_back('1'); ଭାଙ୍ଗ;
			ସ୍ଥିତି Sx: ret.ଠେଲ_back('x'); ଭାଙ୍ଗ;
			ସ୍ଥିତି Sz: ret.ଠେଲ_back('z'); ଭାଙ୍ଗ;
			ସ୍ଥିତି Sa: ret += any; ଭାଙ୍ଗ;
			ସ୍ଥିତି Sm: ret.ଠେଲ_back('m'); ଭାଙ୍ଗ;
		}
	ଫେରନ୍ତୁ ret;
}

RTLIL::Const RTLIL::Const::from_string(ଅଚଳ std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.size());
	ପାଇଁ (auto it = str.rbegin(); it != str.rend(); it++)
		ବାଛ (*it) {
			ସ୍ଥିତି '0': bv.ଠେଲ_back(State::S0); ଭାଙ୍ଗ;
			ସ୍ଥିତି '1': bv.ଠେଲ_back(State::S1); ଭାଙ୍ଗ;
			ସ୍ଥିତି 'x': bv.ଠେଲ_back(State::Sx); ଭାଙ୍ଗ;
			ସ୍ଥିତି 'z': bv.ଠେଲ_back(State::Sz); ଭାଙ୍ଗ;
			ସ୍ଥିତି 'm': bv.ଠେଲ_back(State::Sm); ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ: bv.ଠେଲ_back(State::Sa);
		}
	ଫେରନ୍ତୁ c;
}

std::string RTLIL::Const::decode_string() ଅଚଳ
{
	ଯଦି (auto str = get_if_str())
		ଫେରନ୍ତୁ *str;

	ଅଚଳ bitvectype& bv = get_bits();
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = GetSize(bv);
	ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = n_over_8 * 8;
	ଯଦି (i < n) {
		ଅକ୍ଷର ch = 0;
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < (n - i); j++) {
			ଯଦି (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		ଯଦି (ch != 0)
			s.append({ch});
	}
	i -= 8;
	ପାଇଁ (; i >= 0; i -= 8) {
		ଅକ୍ଷର ch = 0;
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < 8; j++) {
			ଯଦି (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		ଯଦି (ch != 0)
			s.append({ch});
	}
	ଫେରନ୍ତୁ s;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା RTLIL::Const::size() ଅଚଳ {
	ଯଦି (is_str())
		ଫେରନ୍ତୁ 8 * str_.size();
	ଅନ୍ୟଥା {
		check(is_bits());
		ଫେରନ୍ତୁ bits_.size();
	}
}

ତାର୍କିକ RTLIL::Const::empty() ଅଚଳ {
	ଯଦି (is_str())
		ଫେରନ୍ତୁ str_.empty();
	ଅନ୍ୟଥା {
		check(is_bits());
		ଫେରନ୍ତୁ bits_.empty();
	}
}

ଶୂନ୍ୟ RTLIL::Const::bitvectorize_internal() {
	ଯଦି (tag == backing_tag::bits)
		ଫେରନ୍ତୁ;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.size() * 8);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = str_.size() - 1; i >= 0; i--) {
		unsigned ଅକ୍ଷର ch = str_[i];
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < 8; j++) {
			new_bits.ଠେଲ_back((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(ଶୂନ୍ୟ)ନୂତନ ((ଶୂନ୍ୟ*)&bits_) bitvectype(std::move(new_bits));
		tag = backing_tag::bits;
	}
}

ଶୂନ୍ୟ RTLIL::Const::append(ଅଚଳ RTLIL::Const &other) {
	bitvectorize_internal();
	bitvectype& bv = get_bits();
	bv.ଭର୍ତ୍ତି(bv.end(), other.begin(), other.end());
}

RTLIL::State RTLIL::Const::const_iterator::operator*() ଅଚଳ {
	ଯଦି (auto bv = parent->get_if_bits())
		ଫେରନ୍ତୁ (*bv)[idx];

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା char_idx = parent->get_str().size() - idx / 8 - 1;
	ତାର୍କିକ bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	ଫେରନ୍ତୁ bit ? State::S1 : State::S0;
}

ତାର୍କିକ RTLIL::Const::is_fully_zero() ଅଚଳ
{
	ଯଦି (auto str = get_if_str()) {
		ପାଇଁ (ଅକ୍ଷର ch : *str)
			ଯଦି (ch != 0)
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଅଚଳ bitvectype& bv = get_bits();

	ପାଇଁ (ଅଚଳ auto &bit : bv)
		ଯଦି (bit != RTLIL::State::S0)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::Const::is_fully_ones() ଅଚଳ
{
	ଯଦି (auto str = get_if_str()) {
		ପାଇଁ (ଅକ୍ଷର ch : *str)
			ଯଦି (ch != (ଅକ୍ଷର)0xff)
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଅଚଳ bitvectype& bv = get_bits();
	ପାଇଁ (ଅଚଳ auto &bit : bv)
		ଯଦି (bit != RTLIL::State::S1)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::Const::is_fully_def() ଅଚଳ
{
	ଯଦି (is_str())
		ଫେରନ୍ତୁ ସତ୍ୟ;

	ଅଚଳ bitvectype& bv = get_bits();
	ପାଇଁ (ଅଚଳ auto &bit : bv)
		ଯଦି (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::Const::is_fully_undef() ଅଚଳ
{
	ଯଦି (auto str = get_if_str())
		ଫେରନ୍ତୁ str->empty();

	ଅଚଳ bitvectype& bv = get_bits();
	ପାଇଁ (ଅଚଳ auto &bit : bv)
		ଯଦି (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::Const::is_fully_undef_x_only() ଅଚଳ
{
	ଯଦି (auto str = get_if_str())
		ଫେରନ୍ତୁ str->empty();

	ଅଚଳ bitvectype& bv = get_bits();
	ପାଇଁ (ଅଚଳ auto &bit : bv)
		ଯଦି (bit != RTLIL::State::Sx)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::Const::is_onehot(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pos) ଅଚଳ
{
	ତାର୍କିକ found = ମିଥ୍ୟା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = GetSize(*ଏହି);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < size; i++) {
		State bit = (*ଏହି)[i];
		ଯଦି (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		ଯଦି (bit == RTLIL::State::S1) {
			ଯଦି (found)
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
			ଯଦି (pos)
				*pos = i;
			found = ସତ୍ୟ;
		}
	}
	ଫେରନ୍ତୁ found;
}

Hasher RTLIL::Const::hash_into(Hasher h) ଅଚଳ
{
	ଯଦି (auto str = get_if_str())
		ଫେରନ୍ତୁ hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ଅଚଳ bitvectype &bv = get_bits();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = GetSize(bv);
	std::string packed;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା packed_size = (size + 7) >> 3;
	packed.resize(packed_size, 0);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା bi = 0; bi < packed_size; ++bi) {
		ଅକ୍ଷର ch = 0;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା end = std::min((bi + 1)*8, size);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			ଯଦି (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<ଅକ୍ଷର>::hash_into(ch, h);
				ପାଇଁ (; i < size; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(size);
				ଫେରନ୍ତୁ h;
			}
			ch |= static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(b) << (i & 7);
		}
		packed[packed_size - 1 - bi] = ch;
	}
	ଫେରନ୍ତୁ hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, RTLIL::State padding) ଅଚଳ {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = offset; i < offset + len; i++)
		ret_bv.ଠେଲ_back(i < GetSize(*ଏହି) ? (*ଏହି)[i] : padding);
	ଫେରନ୍ତୁ RTLIL::Const(ret_bv);
}
#undef check /* check(condition) for Const */

ତାର୍କିକ RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ଅଚଳ
{
	ଫେରନ୍ତୁ attributes.ଗଣନା_2(id);
}

ଶୂନ୍ୟ RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, ତାର୍କିକ ମୂଲ୍ୟ)
{
	ଯଦି (ମୂଲ୍ୟ)
		attributes[id] = RTLIL::Const(1);
	ଅନ୍ୟଥା
		attributes.erase(id);
}

ତାର୍କିକ RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ଅଚଳ
{
	ଅଚଳ auto it = attributes.find(id);
	ଯଦି (it == attributes.end())
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଫେରନ୍ତୁ it->second.as_bool();
}

ଶୂନ୍ୟ RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string ମୂଲ୍ୟ)
{
	ଯଦି (ମୂଲ୍ୟ.empty())
		attributes.erase(id);
	ଅନ୍ୟଥା
		attributes[id] = ମୂଲ୍ୟ;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ଅଚଳ
{
	std::string ମୂଲ୍ୟ;
	ଅଚଳ auto it = attributes.find(id);
	ଯଦି (it != attributes.end())
		ମୂଲ୍ୟ = it->second.decode_string();
	ଫେରନ୍ତୁ ମୂଲ୍ୟ;
}

ଶୂନ୍ୟ RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, ଅଚଳ pool<string> &data)
{
	string attrval;
	ପାଇଁ (ଅଚଳ auto &s : data) {
		ଯଦି (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

ଶୂନ୍ୟ RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, ଅଚଳ pool<string> &data)
{
	pool<string> union_data = get_strpool_attribute(id);
	union_data.ଭର୍ତ୍ତି(data.begin(), data.end());
	ଯଦି (!union_data.empty())
		set_strpool_attribute(id, union_data);
}

ଶୂନ୍ୟ RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->attributes = std::move(buf);
}

ନାମାକାଶ {
	template<typename F>
	ଶୂନ୍ୟ walk_process_attr_objects(RTLIL::Process *process, F visit) {
		visit(process);
		std::vector<RTLIL::CaseRule*> case_ଗଦା{&process->ମୂଳ_case};
		ଯେତେବେଳେ (!case_ଗଦା.empty()) {
			RTLIL::CaseRule *cs = case_ଗଦା.back();
			case_ଗଦା.କାଢ଼_back();
			visit(cs);
			ପାଇଁ (auto *sw : cs->switches) {
				visit(sw);
				ପାଇଁ (auto *case_ : sw->cases)
					case_ଗଦା.ଠେଲ_back(case_);
			}
		}
		ପାଇଁ (auto *ସମକାଳ : process->syncs)
			ପାଇଁ (auto &mwa : ସମକାଳ->mem_write_actions)
				visit(&mwa);
	}

	template<typename F>
	ଶୂନ୍ୟ walk_attr_objects(RTLIL::Design *design, F visit) {
		ପାଇଁ (auto &[_, module] : design->modules_) {
			visit(module);
			ପାଇଁ (auto &[_, wire] : module->wires_)
				visit(wire);
			ପାଇଁ (auto &[_, mem] : module->memories)
				visit(mem);
			ପାଇଁ (auto &[_, cell] : module->cells_)
				visit(cell);
			ପାଇଁ (auto &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


size_t RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	auto ମୂଳ = [&](IdString ref) {
		ଯଦି (ref != IdString::Null)
			live.ଭର୍ତ୍ତି(ref);
	};

	walk_attr_objects(ଏହି, [&](ଅଚଳ RTLIL::AttrObject *obj) {
		ପାଇଁ (auto &attr : obj->attributes)
			ମୂଳ(attr.first);
	});

	ମୂଳ(selected_active_module);

	ପାଇଁ (auto &[name, module] : modules_) {
		ମୂଳ(name);
		ପାଇଁ (IdString port : module->ports)
			ମୂଳ(port);
		ପାଇଁ (IdString param : module->avail_parameters)
			ମୂଳ(param);
		ପାଇଁ (auto &[param, _] : module->parameter_default_ମୂଲ୍ୟଗୁଡ଼ିକ)
			ମୂଳ(param);
		ପାଇଁ (auto &[name, _] : module->memories)
			ମୂଳ(name);
		ପାଇଁ (auto &[name, _] : module->processes)
			ମୂଳ(name);
		ପାଇଁ (auto &[name, wire] : module->wires_) {
			ମୂଳ(name);
			ଯଦି (wire->known_driver())
				ମୂଳ(wire->driverPort());
		}
		ପାଇଁ (auto &[name, cell] : module->cells_) {
			ମୂଳ(name);
			ମୂଳ(cell->type);
			ପାଇଁ (auto &conn : cell->connections())
				ମୂଳ(conn.first);
			ପାଇଁ (auto &param : cell->parameters)
				ମୂଳ(param.first);
		}
		ପାଇଁ (auto &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_ଗଦା{&process->ମୂଳ_case};
			ଯେତେବେଳେ (!case_ଗଦା.empty()) {
				RTLIL::CaseRule *cs = case_ଗଦା.back();
				case_ଗଦା.କାଢ଼_back();
				ପାଇଁ (auto *sw : cs->switches) {
					ପାଇଁ (auto *case_ : sw->cases)
						case_ଗଦା.ଠେଲ_back(case_);
				}
			}
			ପାଇଁ (auto *ସମକାଳ : process->syncs) {
				ପାଇଁ (auto &mwa : ସମକାଳ->mem_write_actions)
					ମୂଳ(mwa.memid);
			}
		}
	}

	auto ମୂଳ_selection = [&](ଅଚଳ RTLIL::Selection &sel) {
		ପାଇଁ (IdString m : sel.selected_modules)
			ମୂଳ(m);
		ପାଇଁ (auto &[m, members] : sel.selected_members) {
			ମୂଳ(m);
			ପାଇଁ (IdString member : members)
				ମୂଳ(member);
		}
	};

	ପାଇଁ (auto &[name, sel] : selection_vars) {
		ମୂଳ(name);
		ମୂଳ_selection(sel);
	}

	ପାଇଁ (auto &sel : selection_ଗଦା)
		ମୂଳ_selection(sel);

	size_t erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_ଗଣନା;
	ଫେରନ୍ତୁ erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ଅଚଳ
{
	pool<string> data;
	ଯଦି (attributes.ଗଣନା_2(id) != 0)
		ପାଇଁ (auto s : split_tokens(get_string_attribute(id), "|"))
			data.ଭର୍ତ୍ତି(s);
	ଫେରନ୍ତୁ data;
}

ଶୂନ୍ୟ RTLIL::AttrObject::set_hdlname_attribute(ଅଚଳ vector<string> &hierarchy)
{
	string attrval;
	ପାଇଁ (ଅଚଳ auto &ident : hierarchy) {
		ଯଦି (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() ଅଚଳ
{
	ଫେରନ୍ତୁ split_tokens(get_string_attribute(ID::hdlname), " ");
}

ଶୂନ୍ୟ RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, ଅଚଳ vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &data)
{
	std::stringstream attrval;
	ପାଇଁ (auto &i : data) {
		ଯଦି (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ଅଚଳ
{
	vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> data;
	auto it = attributes.find(id);
	ଯଦି (it != attributes.end())
		ପାଇଁ (ଅଚଳ auto &s : split_tokens(attributes.at(id).decode_string())) {
			ଅକ୍ଷର *end = nullptr;
			errno = 0;
			long ମୂଲ୍ୟ = strtol(s.c_str(), &end, 10);
			ଯଦି (end != s.c_str() + s.size())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			ଯଦି (errno == ERANGE || ମୂଲ୍ୟ < INT_MIN || ମୂଲ୍ୟ > INT_MAX)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			data.ଠେଲ_back(ମୂଲ୍ୟ);
		}
	ଫେରନ୍ତୁ data;
}

ତାର୍କିକ RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ଅଚଳ
{
	ଯଦି (current_design != nullptr) {
		auto module = current_design->module(mod_name);
		ଫେରନ୍ତୁ module && module->get_blackbox_attribute();
	} ଅନ୍ୟଥା {
		log_warning("Unable to check if module is boxed for null design.\n");
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
}

ତାର୍କିକ RTLIL::Selection::selected_module(RTLIL::IdString mod_name) ଅଚଳ
{
	ଯଦି (complete_selection)
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଯଦି (!selects_boxes && boxed_module(mod_name))
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଯଦି (full_selection)
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଯଦି (selected_modules.ଗଣନା_2(mod_name) > 0)
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଯଦି (selected_members.ଗଣନା_2(mod_name) > 0)
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) ଅଚଳ
{
	ଯଦି (complete_selection)
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଯଦି (!selects_boxes && boxed_module(mod_name))
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଯଦି (full_selection)
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଯଦି (selected_modules.ଗଣନା_2(mod_name) > 0)
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ଅଚଳ
{
	ଯଦି (complete_selection)
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଯଦି (!selects_boxes && boxed_module(mod_name))
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଯଦି (full_selection)
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଯଦି (selected_modules.ଗଣନା_2(mod_name) > 0)
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଯଦି (selected_members.ଗଣନା_2(mod_name) > 0)
		ଯଦି (selected_members.at(mod_name).ଗଣନା_2(memb_name) > 0)
			ଫେରନ୍ତୁ ସତ୍ୟ;
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ RTLIL::Selection::optimize(RTLIL::Design *design)
{
	ଯଦି (design != current_design) {
		current_design = design;
	}

	ଯଦି (selects_boxes && full_selection)
		complete_selection = ସତ୍ୟ;
	ଯଦି (complete_selection) {
		full_selection = ମିଥ୍ୟା;
		selects_boxes = ସତ୍ୟ;
	}
	ଯଦି (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		ଫେରନ୍ତୁ;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.clear();
	ପାଇଁ (auto mod_name : selected_modules) {
		ଯଦି (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.ଠେଲ_back(mod_name);
		selected_members.erase(mod_name);
	}
	ପାଇଁ (auto mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	ପାଇଁ (auto &it : selected_members)
		ଯଦି (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.ଠେଲ_back(it.first);
	ପାଇଁ (auto mod_name : del_list)
		selected_members.erase(mod_name);

	ପାଇଁ (auto &it : selected_members) {
		del_list.clear();
		ପାଇଁ (auto memb_name : it.second)
			ଯଦି (current_design->module(it.first)->ଗଣନା_id(memb_name) == 0)
				del_list.ଠେଲ_back(memb_name);
		ପାଇଁ (auto memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	add_list.clear();
	ପାଇଁ (auto &it : selected_members)
		ଯଦି (it.second.size() == 0)
			del_list.ଠେଲ_back(it.first);
		ଅନ୍ୟଥା ଯଦି (it.second.size() == current_design->module(it.first)->wires_.size() + current_design->module(it.first)->memories.size() +
				current_design->module(it.first)->cells_.size() + current_design->module(it.first)->processes.size())
			add_list.ଠେଲ_back(it.first);
	ପାଇଁ (auto mod_name : del_list)
		selected_members.erase(mod_name);
	ପାଇଁ (auto mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.ଭର୍ତ୍ତି(mod_name);
	}

	ଯଦି (selected_modules.size() == current_design->modules_.size()) {
		selected_modules.clear();
		selected_members.clear();
		ଯଦି (selects_boxes)
			complete_selection = ସତ୍ୟ;
		ଅନ୍ୟଥା
			full_selection = ସତ୍ୟ;
	}
}

ଶୂନ୍ୟ RTLIL::Selection::clear()
{
	full_selection = ମିଥ୍ୟା;
	complete_selection = ମିଥ୍ୟା;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_defines (ନୂତନ define_map_t)
{
	ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashidx_ଗଣନା = 123456789;
	hashidx_ଗଣନା = mkhash_xorshift(hashidx_ଗଣନା);
	hashidx_ = hashidx_ଗଣନା;

	refcount_modules_ = 0;
	selected_active_module.clear();
	ଠେଲ_full_selection();

	RTLIL::Design::get_all_designs()->ଭର୍ତ୍ତି(std::pair<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Design*>(hashidx_, ଏହି));
}

RTLIL::Design::~Design()
{
	ପାଇଁ (auto &pr : modules_)
		delete pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

ସ୍ଥିର std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Design*> all_designs;
std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Design*> *RTLIL::Design::get_all_designs(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	ଫେରନ୍ତୁ RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ଅଚଳ RTLIL::Module *RTLIL::Design::module(IdString id) ଅଚଳ {
	twines_.check_owned(id);
	ଫେରନ୍ତୁ modules_.ଗଣନା_2(id) ? modules_.at(id) : ଖାଲି;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	ଫେରନ୍ତୁ modules_.ଗଣନା_2(id) ? modules_.at(id) : ଖାଲି;
}

RTLIL::Module *RTLIL::Design::ଶୀର୍ଷ_module() ଅଚଳ
{
	RTLIL::Module *module = nullptr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା module_ଗଣନା = 0;

	ପାଇଁ (auto mod : selected_modules()) {
		ଯଦି (mod->get_bool_attribute(ID::ଶୀର୍ଷ))
			ଫେରନ୍ତୁ mod;
		module_ଗଣନା++;
		module = mod;
	}

	ଫେରନ୍ତୁ module_ଗଣନା == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	ଯଦି (modules_.ଗଣନା_2(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = ନୂତନ RTLIL::Module;
	modules_[name] = module;
	module->design = ଏହି;
	module->name = name;

	ପାଇଁ (auto mon : monitors)
		mon->notify_module_add(module);

	ଯଦି (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	ଫେରନ୍ତୁ module;
}

ଶୂନ୍ୟ RTLIL::Design::scratchpad_unset(ଅଚଳ std::string &varname)
{
	scratchpad.erase(varname);
}

ଶୂନ୍ୟ RTLIL::Design::scratchpad_set_int(ଅଚଳ std::string &varname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ)
{
	scratchpad[varname] = stringf("%d", ମୂଲ୍ୟ);
}

ଶୂନ୍ୟ RTLIL::Design::scratchpad_set_bool(ଅଚଳ std::string &varname, ତାର୍କିକ ମୂଲ୍ୟ)
{
	scratchpad[varname] = ମୂଲ୍ୟ ? "true" : "false";
}

ଶୂନ୍ୟ RTLIL::Design::scratchpad_set_string(ଅଚଳ std::string &varname, std::string ମୂଲ୍ୟ)
{
	scratchpad[varname] = std::move(ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା RTLIL::Design::scratchpad_get_int(ଅଚଳ std::string &varname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା default_ମୂଲ୍ୟ_2) ଅଚଳ
{
	auto it = scratchpad.find(varname);
	ଯଦି (it == scratchpad.end())
		ଫେରନ୍ତୁ default_ମୂଲ୍ୟ_2;

	ଅଚଳ std::string &str = it->second;

	ଯଦି (str == "0" || str == "false")
		ଫେରନ୍ତୁ 0;

	ଯଦି (str == "1" || str == "true")
		ଫେରନ୍ତୁ 1;

	ଅକ୍ଷର *endptr = nullptr;
	long ପୂର୍ଣ୍ଣସଂଖ୍ୟା parsed_ମୂଲ୍ୟ = strtol(str.c_str(), &endptr, 10);
	ଫେରନ୍ତୁ *endptr ? default_ମୂଲ୍ୟ_2 : parsed_ମୂଲ୍ୟ;
}

ତାର୍କିକ RTLIL::Design::scratchpad_get_bool(ଅଚଳ std::string &varname, ତାର୍କିକ default_ମୂଲ୍ୟ_2) ଅଚଳ
{
	auto it = scratchpad.find(varname);
	ଯଦି (it == scratchpad.end())
		ଫେରନ୍ତୁ default_ମୂଲ୍ୟ_2;

	ଅଚଳ std::string &str = it->second;

	ଯଦି (str == "0" || str == "false")
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ଯଦି (str == "1" || str == "true")
		ଫେରନ୍ତୁ ସତ୍ୟ;

	ଫେରନ୍ତୁ default_ମୂଲ୍ୟ_2;
}

std::string RTLIL::Design::scratchpad_get_string(ଅଚଳ std::string &varname, ଅଚଳ std::string &default_ମୂଲ୍ୟ_2) ଅଚଳ
{
	auto it = scratchpad.find(varname);
	ଯଦି (it == scratchpad.end())
		ଫେରନ୍ତୁ default_ମୂଲ୍ୟ_2;

	ଫେରନ୍ତୁ it->second;
}

ଶୂନ୍ୟ RTLIL::Design::remove(RTLIL::Module *module)
{
	ପାଇଁ (auto mon : monitors)
		mon->notify_module_del(module);

	ଯଦି (yosys_xtrace) {
		log("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	delete module;
}

ଶୂନ୍ୟ RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	add(module);
}

ଶୂନ୍ୟ RTLIL::Design::sort()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
	ପାଇଁ (auto &it : modules_)
		it.second->sort();
}

ଶୂନ୍ୟ RTLIL::Design::sort_modules()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
}

ଶୂନ୍ୟ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

ଶୂନ୍ୟ RTLIL::Design::check()
{
#ifndef NDEBUG
	log_assert(!selection_ଗଦା.empty());
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pool_size = 0;
	ପାଇଁ (auto &it : modules_)
		pool_size = std::max(pool_size, ThreadPool::work_pool_size(0, it.second->cells_size(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_size);
	ପାଇଁ (auto &it : modules_) {
		log_assert(ଏହି == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#endif
}

ଶୂନ୍ୟ RTLIL::Design::optimize()
{
	ପାଇଁ (auto &it : modules_)
		it.second->optimize();
	ପାଇଁ (auto &it : selection_ଗଦା)
		it.optimize(ଏହି);
	ପାଇଁ (auto &it : selection_vars)
		it.second.optimize(ଏହି);
}

ଶୂନ୍ୟ RTLIL::Design::clone_into(RTLIL::Design *dst) ଅଚଳ
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	ପାଇଁ (auto it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

ତାର୍କିକ RTLIL::Design::selected_module(IdString mod_name) ଅଚଳ
{
	ଯଦି (!selected_active_module.empty() && mod_name != selected_active_module)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଫେରନ୍ତୁ selection().selected_module(mod_name);
}

ତାର୍କିକ RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) ଅଚଳ
{
	ଯଦି (!selected_active_module.empty() && mod_name != selected_active_module)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଫେରନ୍ତୁ selection().selected_whole_module(mod_name);
}

ତାର୍କିକ RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ଅଚଳ
{
	ଯଦି (!selected_active_module.empty() && mod_name != selected_active_module)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଫେରନ୍ତୁ selection().selected_member(mod_name, memb_name);
}

ତାର୍କିକ RTLIL::Design::selected_module(RTLIL::Module *mod) ଅଚଳ
{
	ଫେରନ୍ତୁ selected_module(mod->name);
}

ତାର୍କିକ RTLIL::Design::selected_whole_module(RTLIL::Module *mod) ଅଚଳ
{
	ଫେରନ୍ତୁ selected_whole_module(mod->name);
}

ଶୂନ୍ୟ RTLIL::Design::ଠେଲ_selection(RTLIL::Selection sel)
{
	sel.current_design = ଏହି;
	selection_ଗଦା.ଠେଲ_back(sel);
}

ଶୂନ୍ୟ RTLIL::Design::ଠେଲ_empty_selection()
{
	ଠେଲ_selection(RTLIL::Selection::EmptySelection(ଏହି));
}

ଶୂନ୍ୟ RTLIL::Design::ଠେଲ_full_selection()
{
	ଠେଲ_selection(RTLIL::Selection::FullSelection(ଏହି));
}

ଶୂନ୍ୟ RTLIL::Design::ଠେଲ_complete_selection()
{
	ଠେଲ_selection(RTLIL::Selection::CompleteSelection(ଏହି));
}

ଶୂନ୍ୟ RTLIL::Design::କାଢ଼_selection()
{
	selection_ଗଦା.କାଢ଼_back();
	// Default to a full_selection if we ran out of stack
	ଯଦି (selection_ଗଦା.empty())
		ଠେଲ_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(ତାର୍କିକ only_selected) ଅଚଳ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(ଏହି), only_selected, ସତ୍ୟ, ମିଥ୍ୟା, RTLIL_BACKEND::DumpMode::Readable);
	ଫେରନ୍ତୁ f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ଅଚଳ
{
	ତାର୍କିକ include_partials = partials == RTLIL::SELECT_ALL;
	ତାର୍କିକ exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	ତାର୍କିକ ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> result;
	result.reserve(modules_.size());
	ପାଇଁ (auto &it : modules_) {
		ଯଦି (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			ଯଦି (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				result.ଠେଲ_back(it.second);
			ଅନ୍ୟଥା
				ବାଛ (boxes)
				{
				ସ୍ଥିତି RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.second);
					ଭାଙ୍ଗ;
				ସ୍ଥିତି RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.second);
					ଭାଙ୍ଗ;
				ସ୍ଥିତି RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.second);
					ଭାଙ୍ଗ;
				ସ୍ଥିତି RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.second);
					ଭାଙ୍ଗ;
				ସ୍ଥିତି RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.second);
					ଭାଙ୍ଗ;
				ସ୍ଥିତି RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.second);
					ଭାଙ୍ଗ;
				ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
					ଭାଙ୍ଗ;
				}
		} ଅନ୍ୟଥା ଯଦି (!include_partials && selected_module(it.first)) {
			ବାଛ(partials)
			{
			ସ୍ଥିତି RTLIL::SELECT_WHOLE_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.second);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି RTLIL::SELECT_WHOLE_ERR:
				log_error("Unsupported partially selected module %s.\n", it.second);
				ଭାଙ୍ଗ;
			ସ୍ଥିତି RTLIL::SELECT_WHOLE_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.second);
				ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
				ଭାଙ୍ଗ;
			}
		}
	}
	ଫେରନ୍ତୁ result;
}

RTLIL::Module::Module()
{
	ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashidx_ଗଣନା = 123456789;
	hashidx_ଗଣନା = mkhash_xorshift(hashidx_ଗଣନା);
	hashidx_ = hashidx_ଗଣନା;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->ଭର୍ତ୍ତି(std::pair<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Module*>(hashidx_, ଏହି));
#endif
}

RTLIL::Module::~Module()
{
	ପାଇଁ (auto &pr : wires_)
		delete pr.second;
	ପାଇଁ (auto &pr : memories)
		delete pr.second;
	ପାଇଁ (auto &pr : cells_)
		delete pr.second;
	ପାଇଁ (auto &pr : processes)
		delete pr.second;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
ସ୍ଥିର std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Module*> all_modules;
std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Module*> *RTLIL::Module::get_all_modules(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ &all_modules;
}
#endif

ଶୂନ୍ୟ RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	ପାଇଁ (auto it = wires_.begin(); it != wires_.end(); ++it)
		ଯଦି (!it->second->port_ନିବେଶ && !it->second->port_output)
			delwires.ଭର୍ତ୍ତି(it->second);

	ପାଇଁ (auto it = memories.begin(); it != memories.end(); ++it)
		delete it->second;
	memories.clear();

	ପାଇଁ (auto it = cells_.begin(); it != cells_.end(); ++it)
		delete it->second;
	cells_.clear();

	ପାଇଁ (auto it = processes.begin(); it != processes.end(); ++it)
		delete it->second;
	processes.clear();

	connections_.clear();

	remove(delwires);
	set_bool_attribute(ID::blackbox);
}

ଶୂନ୍ୟ RTLIL::Module::expand_interfaces(RTLIL::Design *, ଅଚଳ dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

ତାର୍କିକ RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ଅଚଳ dict<RTLIL::IdString, RTLIL::Const> &, ତାର୍କିକ mayfail)
{
	ଯଦି (mayfail)
		ଫେରନ୍ତୁ IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, ଅଚଳ dict<RTLIL::IdString, RTLIL::Const> &, ଅଚଳ dict<RTLIL::IdString, RTLIL::Module*> &, ଅଚଳ dict<RTLIL::IdString, RTLIL::IdString> &, ତାର୍କିକ mayfail)
{
	ଯଦି (mayfail)
		ଫେରନ୍ତୁ IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

size_t RTLIL::Module::ଗଣନା_id(RTLIL::IdString id)
{
	ଫେରନ୍ତୁ wires_.ଗଣନା_2(id) + cells_.ଗଣନା_2(id) + memories.ଗଣନା_2(id) + processes.ଗଣନା_2(id);
}

#ifndef NDEBUG
ନାମାକାଶ {
	ସଂରଚନା InternalCellChecker
	{
		ଅଚଳ RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		InternalCellChecker(ଅଚଳ RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		ଶୂନ୍ୟ error(ପୂର୍ଣ୍ଣସଂଖ୍ୟା linenr, std::optional<std::string> error_ସନ୍ଦେଶ = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			ଯଦି (error_ସନ୍ଦେଶ) {
				log_error("%s in internal cell %s\n", *error_ସନ୍ଦେଶ, cell_loc);
			} ଅନ୍ୟଥା {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା param(RTLIL::IdString name)
		{
			auto it = cell->parameters.find(name);
			ଯଦି (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.ଭର୍ତ୍ତି(name);
			ଫେରନ୍ତୁ it->second.as_int();
		}

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_bool(RTLIL::IdString name)
		{
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା v = param(name);
			ଯଦି (GetSize(cell->parameters.at(name)) > 32)
				error(__LINE__);
			ଯଦି (v != 0 && v != 1)
				error(__LINE__);
			ଫେରନ୍ତୁ v;
		}

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_bool(RTLIL::IdString name, ତାର୍କିକ expected)
		{
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା v = param_bool(name);
			ଯଦି (v != expected)
				error(__LINE__);
			ଫେରନ୍ତୁ v;
		}

		ଶୂନ୍ୟ param_bits(RTLIL::IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
		{
			param(name);
			ଯଦି (GetSize(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			ଫେରନ୍ତୁ cell->parameters.at(name).decode_string();
		}

		ଶୂନ୍ୟ port(RTLIL::IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
		{
			auto it = cell->connections_.find(name);
			ଯଦି (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			ଯଦି (GetSize(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.ଭର୍ତ୍ତି(name);
		}

		ଶୂନ୍ୟ check_expected(ତାର୍କିକ check_matched_sign = ମିଥ୍ୟା)
		{
			ପାଇଁ (auto &para : cell->parameters)
				ଯଦି (expected_params.ଗଣନା_2(para.first) == 0)
					error(__LINE__);
			ପାଇଁ (auto &conn : cell->connections())
				ଯଦି (expected_ports.ଗଣନା_2(conn.first) == 0)
					error(__LINE__);

			ଯଦି (check_matched_sign) {
				log_assert(expected_params.ଗଣନା_2(ID::A_SIGNED) != 0 && expected_params.ଗଣନା_2(ID::B_SIGNED) != 0);
				ତାର୍କିକ a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				ତାର୍କିକ b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				ଯଦି (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		ଶୂନ୍ୟ check()
		{
			ଯଦି (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				ଫେରନ୍ତୁ;

			ଯଦି (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(ସତ୍ୟ);
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/ମିଥ୍ୟା);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/ମିଥ୍ୟା);
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type.in(ID($shift), ID($shiftx))) {
				ଯଦି (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/ମିଥ୍ୟା);
				} ଅନ୍ୟଥା {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/ମିଥ୍ୟା);
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(ସତ୍ୟ);
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($pow));
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::CI, 1);
				port(ID::BI, 1);
				port(ID::X, param(ID::Y_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				port(ID::CO, param(ID::Y_WIDTH));
				check_expected(ସତ୍ୟ);
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($macc_v2)) {
				ଯଦି (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				ଯଦି (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				ଯଦି (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ଅଚଳ Const &a_width = cell->getParam(ID::A_WIDTHS);
				ଅଚଳ Const &b_width = cell->getParam(ID::B_WIDTHS);
				ଅଚଳ Const &c_width = cell->getParam(ID::C_WIDTHS);
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width_sum = 0, b_width_sum = 0, c_width_sum = 0;
				ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_sum += a_width.extract(16 * i, 16).as_int(ମିଥ୍ୟା);
					b_width_sum += b_width.extract(16 * i, 16).as_int(ମିଥ୍ୟା);
				}
				ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < param(ID::NADDENDS); i++) {
					c_width_sum += c_width.extract(16 * i, 16).as_int(ମିଥ୍ୟା);
				}
				port(ID::A, a_width_sum);
				port(ID::B, b_width_sum);
				port(ID::C, c_width_sum);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/ମିଥ୍ୟା);
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				ଯଦି (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($dffsre)) {
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
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_ମୂଲ୍ୟ, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_ମୂଲ୍ୟ, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type.in(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_ମୂଲ୍ୟ, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_ମୂଲ୍ୟ, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($aldffe)) {
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
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_ମୂଲ୍ୟ, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_BITS);
				param(ID::STATE_ଅଙ୍କ);
				param(ID::STATE_ଅଙ୍କ_LOG2);
				param(ID::STATE_RST);
				param_bits(ID::STATE_TABLE, param(ID::STATE_BITS) * param(ID::STATE_ଅଙ୍କ));
				param(ID::TRANS_ଅଙ୍କ);
				param_bits(ID::TRANS_TABLE, param(ID::TRANS_ଅଙ୍କ) * (2*param(ID::STATE_ଅଙ୍କ_LOG2) + param(ID::CTRL_IN_WIDTH) + param(ID::CTRL_OUT_WIDTH)));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::CTRL_IN, param(ID::CTRL_IN_WIDTH));
				port(ID::CTRL_OUT, param(ID::CTRL_OUT_WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_MASK);
				param(ID::COLLISION_X_MASK);
				param_bool(ID::CE_OVER_SRST);
				param_bits(ID::ARST_ମୂଲ୍ୟ, param(ID::WIDTH));
				param_bits(ID::SRST_ମୂଲ୍ୟ, param(ID::WIDTH));
				param_bits(ID::INIT_ମୂଲ୍ୟ, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::SRST, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($memwr_v2)) {
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
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::ଶବ୍ଦଗୁଡ଼ିକ));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::ଶବ୍ଦଗୁଡ଼ିକ));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($mem)) {
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
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($mem_v2)) {
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
				param_bits(ID::RD_ARST_ମୂଲ୍ୟ, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_SRST_ମୂଲ୍ୟ, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
				param_bits(ID::RD_INIT_ମୂଲ୍ୟ, max(1, param(ID::RD_PORTS) * param(ID::WIDTH)));
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
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type.in(ID($ପ୍ରତିଜ୍ଞା), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type.in(ID($specify2), ID($specify3))) {
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
				ଯଦି (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
				}
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($specrule)) {
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
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				ଯଦି (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
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
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				ଯଦି (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				ଫେରନ୍ତୁ;
			}

			ଯଦି (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }
			ଯଦି (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); ଫେରନ୍ତୁ; }

			ଯଦି (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}
			ଯଦି (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}
			ଯଦି (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}
			ଯଦି (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}
			ଯଦି (cell->type.in(ID($ନିବେଶ_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
			}
			ଯଦି (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				check_expected();
				ଫେରନ୍ତୁ;
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

ଶୂନ୍ୟ RTLIL::Module::sort()
{
	wires_.sort(sort_by_id_str(design->twines()));
	cells_.sort(sort_by_id_str(design->twines()));
	parameter_default_ମୂଲ୍ୟଗୁଡ଼ିକ.sort(sort_by_id_str(design->twines()));
	memories.sort(sort_by_id_str(design->twines()));
	processes.sort(sort_by_id_str(design->twines()));
	ପାଇଁ (auto &it : cells_)
		it.second->sort();
	ପାଇଁ (auto &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	ପାଇଁ (auto &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#ifndef NDEBUG
ସ୍ଥିର ଶୂନ୍ୟ check_id_ଚାବିଗୁଡ଼ିକ(ଅଚଳ dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	ପାଇଁ (auto &it : keyed)
		log_assert(!it.first.empty());
}
#endif

ଶୂନ୍ୟ check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ifndef NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->cells_size(), 1000));
	ଅଚଳ RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	ପାଇଁ (auto &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->width >= 0);
		log_assert(it.second->size >= 0);
		check_id_ଚାବିଗୁଡ଼ିକ(it.second->attributes);
		memory_strings.ଭର୍ତ୍ତି(it.second->name.str());
	}

	std::vector<MonotonicFlag> ports_declared(GetSize(module->ports));
	ShardedVector<std::string> memids(subpool);
	subpool.run([const_module, &ports_declared, &memory_strings, &memids](ଅଚଳ ParallelDispatchThreadPool::RunCtx &ctx) {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i : ctx.item_range(const_module->cells_size())) {
			auto it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			ପାଇଁ (auto &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_ଚାବିଗୁଡ଼ିକ(it.second->attributes);
			check_id_ଚାବିଗୁଡ଼ିକ(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			ଯଦି (it.second->has_memid()) {
				log_assert(memory_strings.ଗଣନା_2(it.second->parameters.at(ID::MEMID).decode_string()));
			} ଅନ୍ୟଥା ଯଦି (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.ଗଣନା_2(memid));
				memids.ଭର୍ତ୍ତି(ctx, std::move(memid));
			}
			auto cell_mod = const_module->design->module(it.second->name);
			ଯଦି (cell_mod != nullptr) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#ifdef __GNUC__
				#pragma GCC diagnostic ଠେଲ
				#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				#endif
				log_assert(!it.second->get_blackbox_attribute());
				#ifdef __GNUC__
				#pragma GCC diagnostic କାଢ଼
				#endif
			}
		}

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i : ctx.item_range(const_module->wires_size())) {
			auto it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_ଚାବିଗୁଡ଼ିକ(it.second->attributes);
			ଯଦି (it.second->port_id) {
				log_assert(GetSize(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_ନିବେଶ || it.second->port_output);
				log_assert(it.second->port_id <= GetSize(ports_declared));
				ତାର୍କିକ previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == ମିଥ୍ୟା);
			} ଅନ୍ୟଥା
				log_assert(!it.second->port_ନିବେଶ && !it.second->port_output);
		}
	});
	ପାଇଁ (ଅଚଳ MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == ସତ୍ୟ);
	pool<std::string> memids_pool;
	ପାଇଁ (std::string &memid : memids)
		log_assert(memids_pool.ଭର୍ତ୍ତି(memid).second);

	ପାଇଁ (auto &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->ମୂଳ_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->ମୂଳ_case};
		ପାଇଁ (size_t i = 0; i < all_cases.size(); i++) {
			ପାଇଁ (auto &switch_it : all_cases[i]->switches) {
				ପାଇଁ (auto &case_it : switch_it->cases) {
					ପାଇଁ (auto &compare_it : case_it->compare) {
						log_assert(switch_it->ସଙ୍କେତ.size() == compare_it.size());
					}
					all_cases.ଠେଲ_back(case_it);
				}
			}
		}
		ପାଇଁ (auto &sync_it : it.second->syncs) {
			ବାଛ (sync_it->type) {
				ସ୍ଥିତି RTLIL::SyncType::ST0:
				ସ୍ଥିତି RTLIL::SyncType::ST1:
				ସ୍ଥିତି RTLIL::SyncType::STp:
				ସ୍ଥିତି RTLIL::SyncType::STn:
				ସ୍ଥିତି RTLIL::SyncType::STe:
					log_assert(!sync_it->ସଙ୍କେତ.empty());
					ଭାଙ୍ଗ;
				ସ୍ଥିତି RTLIL::SyncType::STa:
				ସ୍ଥିତି RTLIL::SyncType::STg:
				ସ୍ଥିତି RTLIL::SyncType::STi:
					log_assert(sync_it->ସଙ୍କେତ.empty());
					ଭାଙ୍ଗ;
			}
		}
	}

	ପାଇଁ (auto &it : module->connections_) {
		log_assert(it.first.size() == it.second.size());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_ଚାବିଗୁଡ଼ିକ(module->attributes);
#endif
}

ଶୂନ୍ୟ RTLIL::Module::check()
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pool_size = ThreadPool::work_pool_size(0, cells_size(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_size);
	check_module(ଏହି, thread_pool);
}

ଶୂନ୍ୟ RTLIL::Module::optimize()
{
}

ଶୂନ୍ୟ RTLIL::Module::cloneInto(RTLIL::Module *new_mod) ଅଚଳ
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	ପାଇଁ (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_ମୂଲ୍ୟଗୁଡ଼ିକ.clear();
	ପାଇଁ (auto &it : parameter_default_ମୂଲ୍ୟଗୁଡ଼ିକ)
		new_mod->parameter_default_ମୂଲ୍ୟଗୁଡ଼ିକ[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	ପାଇଁ (auto &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	ପାଇଁ (auto it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	ପାଇଁ (auto it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	ପାଇଁ (auto it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	ପାଇଁ (auto it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addProcess(dst_id, it->second);
	}

	ସଂରଚନା RewriteSigSpecWorker
	{
		ଅଚଳ dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ଅଚଳ RTLIL::Module *new_mod;
		ଶୂନ୍ୟ operator()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([ଏହି](RTLIL::Wire *&wire) {
				auto it = wire_map.find(wire);
				ଯଦି (it != wire_map.end())
					wire = it->second;
				ଅନ୍ୟଥା
					log_assert(wire->module == new_mod);
			});
		}
	};

	RewriteSigSpecWorker rewriteSigSpecWorker{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewriteSigSpecWorker);
	new_mod->fixup_ports();
}

RTLIL::Module *RTLIL::Module::clone() ଅଚଳ
{
	RTLIL::Module *new_mod = ନୂତନ RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	ଫେରନ୍ତୁ new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) ଅଚଳ
{
	RTLIL::Module *new_mod = ନୂତନ RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->add(new_mod);
	ଫେରନ୍ତୁ new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) ଅଚଳ
{
	RTLIL::Module *new_mod = ନୂତନ RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	ଫେରନ୍ତୁ new_mod;
}

ତାର୍କିକ RTLIL::Module::has_memories() ଅଚଳ
{
	ଫେରନ୍ତୁ !memories.empty();
}

ତାର୍କିକ RTLIL::Module::has_processes() ଅଚଳ
{
	ଫେରନ୍ତୁ !processes.empty();
}

ତାର୍କିକ RTLIL::Module::has_memories_warn() ଅଚଳ
{
	ଯଦି (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", ଏହି);
	ଫେରନ୍ତୁ !memories.empty();
}

ତାର୍କିକ RTLIL::Module::has_processes_warn() ଅଚଳ
{
	ଯଦି (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", ଏହି);
	ଫେରନ୍ତୁ !processes.empty();
}

ତାର୍କିକ RTLIL::Module::is_selected() ଅଚଳ
{
	ଫେରନ୍ତୁ design->selected_module(ଏହି->name);
}

ତାର୍କିକ RTLIL::Module::is_selected_whole() ଅଚଳ
{
	ଫେରନ୍ତୁ design->selected_whole_module(ଏହି->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() ଅଚଳ
{
	std::vector<RTLIL::Wire*> result;
	result.reserve(wires_.size());
	ପାଇଁ (auto &it : wires_)
		ଯଦି (design->selected(ଏହି, it.second))
			result.ଠେଲ_back(it.second);
	ଫେରନ୍ତୁ result;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() ଅଚଳ
{
	std::vector<RTLIL::Cell*> result;
	result.reserve(cells_.size());
	ପାଇଁ (auto &it : cells_)
		ଯଦି (design->selected(ଏହି, it.second))
			result.ଠେଲ_back(it.second);
	ଫେରନ୍ତୁ result;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() ଅଚଳ
{
	std::vector<RTLIL::Memory*> result;
	result.reserve(memories.size());
	ପାଇଁ (auto &it : memories)
		ଯଦି (design->selected(ଏହି, it.second))
			result.ଠେଲ_back(it.second);
	ଫେରନ୍ତୁ result;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() ଅଚଳ
{
	std::vector<RTLIL::Process*> result;
	result.reserve(processes.size());
	ପାଇଁ (auto &it : processes)
		ଯଦି (design->selected(ଏହି, it.second))
			result.ଠେଲ_back(it.second);
	ଫେରନ୍ତୁ result;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() ଅଚଳ
{
	std::vector<RTLIL::SelectedMember> result;
	ପାଇଁ (auto cell : selected_cells())
		result.ଠେଲ_back({cell, cell->name});
	ପାଇଁ (auto memory : selected_memories())
		result.ଠେଲ_back({memory, memory->name});
	ପାଇଁ (auto wire : selected_wires())
		result.ଠେଲ_back({wire, wire->name});
	ପାଇଁ (auto process : selected_processes())
		result.ଠେଲ_back({process, process->name});
	ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ RTLIL::Module::add(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(ଗଣନା_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = ଏହି;
}

ଶୂନ୍ୟ RTLIL::Module::add(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(ଗଣନା_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = ଏହି;
}

ଶୂନ୍ୟ RTLIL::Module::add(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(ଗଣନା_id(process->name) == 0);
	processes[process->name] = process;
	process->module = ଏହି;
}

ଶୂନ୍ୟ RTLIL::Module::remove(ଅଚଳ pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	ଯଦି (wires.empty())
		ଫେରନ୍ତୁ;

	ସଂରଚନା DeleteWireWorker
	{
		RTLIL::Module *module;
		ଅଚଳ pool<RTLIL::Wire*> *wires_p;

		ଶୂନ୍ୟ operator()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([ଏହି](RTLIL::Wire *&wire) {
				ଯଦି (wires_p->ଗଣନା_2(wire))
					wire = module->addWire(stringf("$delete_wire$%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)autoidx++), wire->width);
			});
		}

		ଶୂନ୍ୟ operator()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	DeleteWireWorker delete_wire_worker;
	delete_wire_worker.module = ଏହି;
	delete_wire_worker.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_worker);

	ଯଦି (design->flagBufferedNormalized) {
		ପାଇଁ (auto wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_index.erase(wire);
		}
	}

	ପାଇଁ (auto &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.ଗଣନା_2(id) != 0);
		wires_.erase(id);
		delete it;
	}
}

ଶୂନ୍ୟ RTLIL::Module::remove(RTLIL::Cell *cell)
{
	ଯେତେବେଳେ (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.ଗଣନା_2(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	ଯଦି (design && design->flagBufferedNormalized && buf_norm_cell_queue.ଗଣନା_2(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.ଭର୍ତ୍ତି(cell);
	} ଅନ୍ୟଥା {
		delete cell;
	}
}

ଶୂନ୍ୟ RTLIL::Module::remove(RTLIL::Memory *memory)
{
	log_assert(memories.ଗଣନା_2(memory->name) != 0);
	memories.erase(memory->name);
	delete memory;
}

ଶୂନ୍ୟ RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.ଗଣନା_2(process->name) != 0);
	processes.erase(process->name);
	delete process;
}

ଶୂନ୍ୟ RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	add(wire);
}

ଶୂନ୍ୟ RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	add(cell);
}

ଶୂନ୍ୟ RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	ଯଦି (old_id != IdString::Null && wires_.ଗଣନା_2(old_id))
		rename(wires_.at(old_id), new_name);
	ଅନ୍ୟଥା ଯଦି (old_id != IdString::Null && cells_.ଗଣନା_2(old_id))
		rename(cells_.at(old_id), new_name);
	ଅନ୍ୟଥା
		log_abort();
}

ଶୂନ୍ୟ RTLIL::Module::swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2)
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

ଶୂନ୍ୟ RTLIL::Module::swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = 0;
	ଫେରନ୍ତୁ uniquify(name, index);
}

IdString RTLIL::Module::uniquify(IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା &index)
{
	ଯଦି (index == 0) {
		ଯଦି (ଗଣନା_id(name) == 0)
			ଫେରନ୍ତୁ name;
		index++;
	}

	ଯେତେବେଳେ (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", index)});
		ଯଦି (ଗଣନା_id(new_name) == 0)
			ଫେରନ୍ତୁ new_name;
		index++;
	}
}

ସ୍ଥିର ତାର୍କିକ fixup_ports_compare(ଅଚଳ RTLIL::Wire *a, ଅଚଳ RTLIL::Wire *b)
{
	ଯଦି (a->port_id && !b->port_id)
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଯଦି (!a->port_id && b->port_id)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ଯଦି (a->port_id == b->port_id)
		ଫେରନ୍ତୁ a->name < b->name;
	ଫେରନ୍ତୁ a->port_id < b->port_id;
}

ଶୂନ୍ୟ RTLIL::Module::connect(ଅଚଳ RTLIL::SigSig &conn)
{
	ପାଇଁ (auto mon : monitors)
		mon->notify_connect(ଏହି, conn);

	ଯଦି (design)
		ପାଇଁ (auto mon : design->monitors)
			mon->notify_connect(ଏହି, conn);

	// ignore all attempts to assign constants to other constants
	ଯଦି (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(conn.first); i++)
			ଯଦି (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		ଯଦି (GetSize(new_conn.first))
			connect(new_conn);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (yosys_xtrace) {
		log("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", ଏହି, log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(GetSize(conn.first) == GetSize(conn.second));
	connections_.ଠେଲ_back(conn);
}

ଶୂନ୍ୟ RTLIL::Module::connect(ଅଚଳ RTLIL::SigSpec &lhs, ଅଚଳ RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

ଶୂନ୍ୟ RTLIL::Module::new_connections(ଅଚଳ std::vector<RTLIL::SigSig> &new_conn)
{
	ପାଇଁ (auto mon : monitors)
		mon->notify_connect(ଏହି, new_conn);

	ଯଦି (design)
		ପାଇଁ (auto mon : design->monitors)
			mon->notify_connect(ଏହି, new_conn);

	ଯଦି (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", ଏହି);
		ପାଇଁ (auto &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ଅଚଳ std::vector<RTLIL::SigSig> &RTLIL::Module::connections() ଅଚଳ
{
	ଫେରନ୍ତୁ connections_;
}

ଶୂନ୍ୟ RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	ପାଇଁ (auto &w : wires_)
		ଯଦି (w.second->port_ନିବେଶ || w.second->port_output)
			all_ports.ଠେଲ_back(w.second);
		ଅନ୍ୟଥା
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	ଯଦି (design && design->flagBufferedNormalized) {
		ପାଇଁ (auto &w : wires_)
			ଯଦି (w.second->driverCell_ && w.second->driverCell_->type == ID($ନିବେଶ_port))
				buf_norm_wire_queue.ଭର୍ତ୍ତି(w.second);

		buf_norm_wire_queue.ଭର୍ତ୍ତି(all_ports.begin(), all_ports.end());
	}

	ports.clear();
	ପାଇଁ (size_t i = 0; i < all_ports.size(); i++) {
		ports.ଠେଲ_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
	}
}

ଶୂନ୍ୟ RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		ଅଚଳ dict<IdString, RTLIL::Const> &src,
		ଅଚଳ RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	ଯଦି (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		ଫେରନ୍ତୁ;
	}
	dst.clear();
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = GetSize(src) - 1; i >= 0; i--) {
		auto &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::addWire(RTLIL::IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
	log_assert(design);
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	RTLIL::Wire *wire = ନୂତନ RTLIL::Wire;
	wire->width = width;
	wire->name = name;
	add(wire);
	ଫେରନ୍ତୁ wire;
}

RTLIL::Wire *RTLIL::Module::addWire(IdString name, ଅଚଳ RTLIL::Wire *other)
{
	RTLIL::Wire *wire = addWire(name);
	wire->width = other->width;
	wire->start_offset = other->start_offset;
	wire->port_id = other->port_id;
	wire->port_ନିବେଶ = other->port_ନିବେଶ;
	wire->port_output = other->port_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ଅଚଳ RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
		RTLIL::copy_attr_dict(wire->attributes, other->attributes, src_design, ଏହି->design);
	}
	ଫେରନ୍ତୁ wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = ନୂତନ RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, ଅଚଳ RTLIL::Cell *other)
{
	ଅଚଳ RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	ତାର୍କିକ cross_pool = src_design && ଏହି->design && src_design != ଏହି->design;

	IdString type = other->type.ref();
	ଯଦି (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, ଏହି->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, ଏହି->design);

	ଯଦି (!cross_pool)
		cell->connections_ = other->connections_;
	ଅନ୍ୟଥା
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = GetSize(other->connections_) - 1; i >= 0; i--) {
			auto &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	ଫେରନ୍ତୁ cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = ନୂତନ RTLIL::Memory;
	mem->module = ଏହି;
	mem->name = name;
	memories[name] = mem;
	ଫେରନ୍ତୁ mem;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name, ଅଚଳ RTLIL::Memory *other)
{
	log_assert(design);
	RTLIL::Memory *mem = ନୂତନ RTLIL::Memory;
	mem->module = ଏହି;
	mem->name = name;
	mem->width = other->width;
	mem->start_offset = other->start_offset;
	mem->size = other->size;
	RTLIL::copy_attr_dict(mem->attributes, other->attributes,
			other->module ? other->module->design : nullptr, design);
	memories[name] = mem;
	ଫେରନ୍ତୁ mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = ନୂତନ RTLIL::Process;
	proc->name = name;
	add(proc);
	ଫେରନ୍ତୁ proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, ଅଚଳ RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ଅଚଳ RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	ଯଦି (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		ପାଇଁ (auto *ସମକାଳ : proc->syncs)
			ପାଇଁ (auto &mwa : ସମକାଳ->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	ଫେରନ୍ତୁ proc;
}

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, ତାର୍କିକ is_signed, ଅଚଳ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		ଫେରନ୍ତୁ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ତାର୍କିକ is_signed, ଅଚଳ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		ଫେରନ୍ତୁ sig_y;                                       \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, ତାର୍କିକ /* is_signed */, ଅଚଳ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::WIDTH] = sig_a.size();         \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		ଫେରନ୍ତୁ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ତାର୍କିକ is_signed, ଅଚଳ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		ଫେରନ୍ତୁ sig_y;                                       \
	}
DEF_METHOD(Buf,     sig_a.size(), ID($buf))
DEF_METHOD(Barrier, sig_a.size(), ID($barrier))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, ତାର୍କିକ is_signed, ଅଚଳ std::string &src) { \
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
		ଫେରନ୍ତୁ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ତାର୍କିକ is_signed, ଅଚଳ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ଫେରନ୍ତୁ sig_y;                                            \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, ତାର୍କିକ is_signed, ଅଚଳ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = ମିଥ୍ୟା;             \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		ଫେରନ୍ତୁ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ତାର୍କିକ is_signed, ଅଚଳ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ଫେରନ୍ତୁ sig_y;                                            \
	}
DEF_METHOD(Shl,      sig_a.size(), ID($shl))
DEF_METHOD(Shr,      sig_a.size(), ID($shr))
DEF_METHOD(Sshl,     sig_a.size(), ID($sshl))
DEF_METHOD(Sshr,     sig_a.size(), ID($sshr))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, ତାର୍କିକ is_signed, ଅଚଳ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = ମିଥ୍ୟା;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::B_WIDTH] = sig_b.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		ଫେରନ୍ତୁ cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ତାର୍କିକ is_signed, ଅଚଳ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		ଫେରନ୍ତୁ sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.size(), ID($shiftx))
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		ଯଦି (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.size();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		ଫେରନ୍ତୁ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		ଫେରନ୍ତୁ sig_y;                                             \
	}
DEF_METHOD(Mux,      ID($mux),        0)
DEF_METHOD(Bwmux,    ID($bwmux),      0)
DEF_METHOD(Pmux,     ID($pmux),       1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = _demux ? sig_a.size() : sig_y.size(); \
		cell->parameters[ID::S_WIDTH] = sig_s.size();             \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		ଫେରନ୍ତୁ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.size() << sig_s.size() : sig_a.size() >> sig_s.size()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		ଫେରନ୍ତୁ sig_y;                                             \
	}
DEF_METHOD(Bmux,     ID($bmux),       0)
DEF_METHOD(Demux,    ID($demux),      1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		ଫେରନ୍ତୁ cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		ଫେରନ୍ତୁ sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#undef DEF_METHOD

#define DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig1, ଅଚଳ RTLIL::SigBit &sig2, ଅଚଳ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		ଫେରନ୍ତୁ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig1, ଅଚଳ std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		ଫେରନ୍ତୁ sig2;                                      \
	}
#define DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig1, ଅଚଳ RTLIL::SigBit &sig2, ଅଚଳ RTLIL::SigBit &sig3, ଅଚଳ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		ଫେରନ୍ତୁ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig1, ଅଚଳ RTLIL::SigBit &sig2, ଅଚଳ std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		ଫେରନ୍ତୁ sig3;                                      \
	}
#define DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig1, ଅଚଳ RTLIL::SigBit &sig2, ଅଚଳ RTLIL::SigBit &sig3, ଅଚଳ RTLIL::SigBit &sig4, ଅଚଳ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		ଫେରନ୍ତୁ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig1, ଅଚଳ RTLIL::SigBit &sig2, ଅଚଳ RTLIL::SigBit &sig3, ଅଚଳ std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		ଫେରନ୍ତୁ sig4;                                      \
	}
#define DEF_METHOD_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig1, ଅଚଳ RTLIL::SigBit &sig2, ଅଚଳ RTLIL::SigBit &sig3, ଅଚଳ RTLIL::SigBit &sig4, ଅଚଳ RTLIL::SigBit &sig5, ଅଚଳ std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->setPort("\\" #_P5, sig5);                   \
		cell->set_src_attribute(src);                     \
		ଫେରନ୍ତୁ cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig1, ଅଚଳ RTLIL::SigBit &sig2, ଅଚଳ RTLIL::SigBit &sig3, ଅଚଳ RTLIL::SigBit &sig4, ଅଚଳ std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		ଫେରନ୍ତୁ sig5;                                           \
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

RTLIL::Cell* RTLIL::Module::addPow(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, ତାର୍କିକ a_signed, ତାର୍କିକ b_signed, ଅଚଳ std::string &src)
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
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addFa(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_c, ଅଚଳ RTLIL::SigSpec &sig_x, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fa));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::C, sig_c);
	cell->setPort(ID::X, sig_x);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addSlice(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, RTLIL::Const offset, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($slice));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::Y_WIDTH] = sig_y.size();
	cell->parameters[ID::OFFSET] = offset;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addConcat(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($concat));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::B_WIDTH] = sig_b.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, RTLIL::Const lut, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ପ୍ରତିଜ୍ଞା));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addSr(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr, ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ set_polarity, ତାର୍କିକ clr_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sr));
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addDff(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ clk_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addDffe(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ clk_polarity, ତାର୍କିକ en_polarity, ଅଚଳ std::string &src)
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
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addDffsr(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ clk_polarity, ତାର୍କିକ set_polarity, ତାର୍କିକ clr_polarity, ଅଚଳ std::string &src)
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
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addDffsre(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ clk_polarity, ତାର୍କିକ en_polarity, ତାର୍କିକ set_polarity, ତାର୍କିକ clr_polarity, ଅଚଳ std::string &src)
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
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addAdff(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_arst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ମୂଲ୍ୟ, ତାର୍କିକ clk_polarity, ତାର୍କିକ arst_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ମୂଲ୍ୟ] = arst_ମୂଲ୍ୟ;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addAdffe(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_arst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ମୂଲ୍ୟ, ତାର୍କିକ clk_polarity, ତାର୍କିକ en_polarity, ତାର୍କିକ arst_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ମୂଲ୍ୟ] = arst_ମୂଲ୍ୟ;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addAldff(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_aload, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		ଅଚଳ RTLIL::SigSpec &sig_ad, ତାର୍କିକ clk_polarity, ତାର୍କିକ aload_polarity, ଅଚଳ std::string &src)
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
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addAldffe(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_aload, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		ଅଚଳ RTLIL::SigSpec &sig_ad, ତାର୍କିକ clk_polarity, ତାର୍କିକ en_polarity, ତାର୍କିକ aload_polarity, ଅଚଳ std::string &src)
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
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addSdff(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_srst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ମୂଲ୍ୟ, ତାର୍କିକ clk_polarity, ତାର୍କିକ srst_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ମୂଲ୍ୟ] = srst_ମୂଲ୍ୟ;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffe(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_srst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ମୂଲ୍ୟ, ତାର୍କିକ clk_polarity, ତାର୍କିକ en_polarity, ତାର୍କିକ srst_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ମୂଲ୍ୟ] = srst_ମୂଲ୍ୟ;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffce(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_srst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_ମୂଲ୍ୟ, ତାର୍କିକ clk_polarity, ତାର୍କିକ en_polarity, ତାର୍କିକ srst_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_ମୂଲ୍ୟ] = srst_ମୂଲ୍ୟ;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addDlatch(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ en_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatch(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_arst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_ମୂଲ୍ୟ, ତାର୍କିକ en_polarity, ତାର୍କିକ arst_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_ମୂଲ୍ୟ] = arst_ମୂଲ୍ୟ;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsr(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ en_polarity, ତାର୍କିକ set_polarity, ତାର୍କିକ clr_polarity, ଅଚଳ std::string &src)
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
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr,
		ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ set_polarity, ତାର୍କିକ clr_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ clk_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ clk_polarity, ତାର୍କିକ en_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addDffsrGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ clk_polarity, ତାର୍କିକ set_polarity, ତାର୍କିକ clr_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addDffsreGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ clk_polarity, ତାର୍କିକ en_polarity, ତାର୍କିକ set_polarity, ତାର୍କିକ clr_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addAdffGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_arst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		ତାର୍କିକ arst_ମୂଲ୍ୟ, ତାର୍କିକ clk_polarity, ତାର୍କିକ arst_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ମୂଲ୍ୟ ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addAdffeGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_arst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		ତାର୍କିକ arst_ମୂଲ୍ୟ, ତାର୍କିକ clk_polarity, ତାର୍କିକ en_polarity, ତାର୍କିକ arst_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ମୂଲ୍ୟ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addAldffGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_aload, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		ଅଚଳ RTLIL::SigSpec &sig_ad, ତାର୍କିକ clk_polarity, ତାର୍କିକ aload_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addAldffeGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_aload, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		ଅଚଳ RTLIL::SigSpec &sig_ad, ତାର୍କିକ clk_polarity, ତାର୍କିକ en_polarity, ତାର୍କିକ aload_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_srst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		ତାର୍କିକ srst_ମୂଲ୍ୟ, ତାର୍କିକ clk_polarity, ତାର୍କିକ srst_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ମୂଲ୍ୟ ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffeGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_srst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		ତାର୍କିକ srst_ମୂଲ୍ୟ, ତାର୍କିକ clk_polarity, ତାର୍କିକ en_polarity, ତାର୍କିକ srst_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ମୂଲ୍ୟ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addSdffceGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_srst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		ତାର୍କିକ srst_ମୂଲ୍ୟ, ତାର୍କିକ clk_polarity, ତାର୍କିକ en_polarity, ତାର୍କିକ srst_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_ମୂଲ୍ୟ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ en_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatchGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_arst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
		ତାର୍କିକ arst_ମୂଲ୍ୟ, ତାର୍କିକ en_polarity, ତାର୍କିକ arst_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_ମୂଲ୍ୟ ? '1' : '0'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsrGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ତାର୍କିକ en_polarity, ତାର୍କିକ set_polarity, ତାର୍କିକ clr_polarity, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ଅଚଳ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ଅଚଳ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ଅଚଳ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ଅଚଳ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ଅଚଳ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ sig;
}

RTLIL::SigSpec RTLIL::Module::SetTag(RTLIL::IdString name, ଅଚଳ std::string &tag, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ RTLIL::SigSpec &sig_c, ଅଚଳ std::string &src)
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
	ଫେରନ୍ତୁ sig;
}

RTLIL::Cell* RTLIL::Module::addSetTag(RTLIL::IdString name, ଅଚଳ std::string &tag, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ RTLIL::SigSpec &sig_c, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src)
{
	Cell *cell = addCell(name, ID($set_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::SigSpec RTLIL::Module::GetTag(RTLIL::IdString name, ଅଚଳ std::string &tag, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($get_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ sig;
}

RTLIL::Cell* RTLIL::Module::addOverwriteTag(RTLIL::IdString name, ଅଚଳ std::string &tag, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ RTLIL::SigSpec &sig_c, ଅଚଳ std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($overwrite_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ cell;
}

RTLIL::SigSpec RTLIL::Module::OriginalTag(RTLIL::IdString name, ଅଚଳ std::string &tag, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($original_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_e, ଅଚଳ std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.size());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.size();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	ଫେରନ୍ତୁ sig;
}

std::string RTLIL::Module::to_rtlil_str() ଅଚଳ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(ଏହି), design, ମିଥ୍ୟା, ସତ୍ୟ, ମିଥ୍ୟା, RTLIL_BACKEND::DumpMode::Readable);
	ଫେରନ୍ତୁ f.str();
}

RTLIL::Wire::Wire()
{
	ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashidx_ଗଣନା = 123456789;
	hashidx_ଗଣନା = mkhash_xorshift(hashidx_ଗଣନା);
	hashidx_ = hashidx_ଗଣନା;

	module = nullptr;
	width = 1;
	start_offset = 0;
	port_id = 0;
	port_ନିବେଶ = ମିଥ୍ୟା;
	port_output = ମିଥ୍ୟା;
	upto = ମିଥ୍ୟା;
	is_signed = ମିଥ୍ୟା;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->ଭର୍ତ୍ତି(std::pair<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Wire*>(hashidx_, ଏହି));
#endif
}

RTLIL::Wire::~Wire()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#endif
}

std::string RTLIL::Wire::to_rtlil_str() ଅଚଳ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", ଏହି, design(), RTLIL_BACKEND::DumpMode::Readable);
	ଫେରନ୍ତୁ f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
ସ୍ଥିର std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Wire*> all_wires;
std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ &all_wires;
}
#endif

RTLIL::Memory::Memory()
{
	ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashidx_ଗଣନା = 123456789;
	hashidx_ଗଣନା = mkhash_xorshift(hashidx_ଗଣନା);
	hashidx_ = hashidx_ଗଣନା;

	width = 1;
	start_offset = 0;
	size = 0;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->ଭର୍ତ୍ତି(std::pair<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Memory*>(hashidx_, ଏହି));
#endif
}

std::string RTLIL::Memory::to_rtlil_str() ଅଚଳ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", ଏହି, design(), RTLIL_BACKEND::DumpMode::Readable);
	ଫେରନ୍ତୁ f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashidx_ଗଣନା = 123456789;
	hashidx_ଗଣନା = mkhash_xorshift(hashidx_ଗଣନା);
	hashidx_ = hashidx_ଗଣନା;
}

std::string RTLIL::Process::to_rtlil_str() ଅଚଳ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", ଏହି, design(), RTLIL_BACKEND::DumpMode::Readable);
	ଫେରନ୍ତୁ f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashidx_ଗଣନା = 123456789;
	hashidx_ଗଣନା = mkhash_xorshift(hashidx_ଗଣନା);
	hashidx_ = hashidx_ଗଣନା;

	// log("#memtrace# %p\n", this);
	memhasher();

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->ଭର୍ତ୍ତି(std::pair<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Cell*>(hashidx_, ଏହି));
#endif
}

RTLIL::Cell::~Cell()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#endif
}

std::string RTLIL::Cell::to_rtlil_str() ଅଚଳ
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", ଏହି, design(), RTLIL_BACKEND::DumpMode::Readable);
	ଫେରନ୍ତୁ f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
ସ୍ଥିର std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Cell*> all_cells;
std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ &all_cells;
}
#endif

ତାର୍କିକ RTLIL::Cell::hasPort(RTLIL::IdString portname) ଅଚଳ
{
	ଫେରନ୍ତୁ connections_.ଗଣନା_2(portname) != 0;
}

// bufnorm

ଅଚଳ RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) ଅଚଳ
{
	ଫେରନ୍ତୁ connections_.at(portname);
}

ଅଚଳ dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() ଅଚଳ
{
	ଫେରନ୍ତୁ connections_;
}

ତାର୍କିକ RTLIL::Cell::known() ଅଚଳ
{
	ଯଦି (yosys_celltypes.cell_known(type))
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଯଦି (module && module->design && module->design->module(type))
		ଫେରନ୍ତୁ ସତ୍ୟ;
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ RTLIL::Cell::ନିବେଶ(IdString portname) ଅଚଳ
{
	ଯଦି (yosys_celltypes.cell_known(type))
		ଫେରନ୍ତୁ yosys_celltypes.cell_ନିବେଶ(type.ref(), portname);
	ଯଦି (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		ଫେରନ୍ତୁ w && w->port_ନିବେଶ;
	}
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ RTLIL::Cell::output(RTLIL::IdString portname) ଅଚଳ
{
	ଯଦି (yosys_celltypes.cell_known(type))
		ଫେରନ୍ତୁ yosys_celltypes.cell_output(type, portname);
	ଯଦି (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		ଫେରନ୍ତୁ w && w->port_output;
	}
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) ଅଚଳ
{
	ଯଦି (yosys_celltypes.cell_known(type.ref()))
		ଫେରନ୍ତୁ yosys_celltypes.cell_port_dir(type, portname);
	ଯଦି (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		ଯଦି (m == nullptr)
			ଫେରନ୍ତୁ PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		ଯଦି (w == nullptr)
			ଫେରନ୍ତୁ PortDir::PD_UNKNOWN;
		ଫେରନ୍ତୁ PortDir(w->port_ନିବେଶ + w->port_output * 2);
	}
	ଫେରନ୍ତୁ PortDir::PD_UNKNOWN;
}

ତାର୍କିକ RTLIL::Cell::hasParam(RTLIL::IdString paramname) ଅଚଳ
{
	ଫେରନ୍ତୁ parameters.ଗଣନା_2(paramname) != 0;
}

ଶୂନ୍ୟ RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

ଶୂନ୍ୟ RTLIL::Cell::setParam(RTLIL::IdString paramname, RTLIL::Const ମୂଲ୍ୟ)
{
	parameters[paramname] = std::move(ମୂଲ୍ୟ);
}

ଅଚଳ RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ଅଚଳ
{
	ଅଚଳ auto &it = parameters.find(paramname);
	ଯଦି (it != parameters.end())
		ଫେରନ୍ତୁ it->second;
	ଯଦି (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		ଯଦି (m)
			ଫେରନ୍ତୁ m->parameter_default_ମୂଲ୍ୟଗୁଡ଼ିକ.at(paramname);
	}
	ଫିଙ୍ଗ std::out_of_range("Cell::getParam()");
}

ଶୂନ୍ୟ RTLIL::Cell::sort()
{
	connections_.sort(sort_by_id_str(module->twines()));
	parameters.sort(sort_by_id_str(module->twines()));
	attributes.sort(sort_by_id_str(module->twines()));
}

ଶୂନ୍ୟ RTLIL::Cell::check()
{
#ifndef NDEBUG
	InternalCellChecker checker(ଖାଲି, ଏହି);
	checker.check();
#endif
}

ଶୂନ୍ୟ RTLIL::Cell::fixup_parameters(ତାର୍କିକ set_a_signed, ତାର୍କିକ set_b_signed)
{
	ଯଦି (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		ଫେରନ୍ତୁ;

	ଯଦି (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		ଯଦି (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		ଫେରନ୍ତୁ;
	}

	ଯଦି (type == ID($demux)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		ଫେରନ୍ତୁ;
	}

	ଯଦି (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (type == ID($fa)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (type == ID($lcu)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::CO]);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);
		ଫେରନ୍ତୁ;
	}

	ତାର୍କିକ signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	ଯଦି (connections_.ଗଣନା_2(ID::A)) {
		ଯଦି (signedness_ab) {
			ଯଦି (set_a_signed)
				parameters[ID::A_SIGNED] = ସତ୍ୟ;
			ଅନ୍ୟଥା ଯଦି (parameters.ଗଣନା_2(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = ମିଥ୍ୟା;
		}
		parameters[ID::A_WIDTH] = GetSize(connections_[ID::A]);
	}

	ଯଦି (connections_.ଗଣନା_2(ID::B)) {
		ଯଦି (signedness_ab) {
			ଯଦି (set_b_signed)
				parameters[ID::B_SIGNED] = ସତ୍ୟ;
			ଅନ୍ୟଥା ଯଦି (parameters.ଗଣନା_2(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = ମିଥ୍ୟା;
		}
		parameters[ID::B_WIDTH] = GetSize(connections_[ID::B]);
	}

	ଯଦି (connections_.ଗଣନା_2(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);

	ଯଦି (connections_.ଗଣନା_2(ID::Q))
		parameters[ID::WIDTH] = GetSize(connections_[ID::Q]);

	check();
}

ତାର୍କିକ RTLIL::Cell::has_keep_attr() ଅଚଳ {
	ଫେରନ୍ତୁ get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

ତାର୍କିକ RTLIL::Cell::has_memid() ଅଚଳ
{
	ଫେରନ୍ତୁ type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

ତାର୍କିକ RTLIL::Cell::is_mem_cell() ଅଚଳ
{
	ଫେରନ୍ତୁ type.in(ID($mem), ID($mem_v2)) || has_memid();
}

ତାର୍କିକ RTLIL::Cell::is_builtin_ff() ଅଚଳ {
	ଫେରନ୍ତୁ StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ଅଚଳ RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	ଯଦି (wire == ଖାଲି)
		data = {bit.data};
	ଅନ୍ୟଥା
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା length) ଅଚଳ
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= width);
	RTLIL::SigChunk ret;
	ଯଦି (wire) {
		ret.wire = wire;
		ret.offset = ଏହି->offset + offset;
		ret.width = length;
	} ଅନ୍ୟଥା {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < length; i++)
			ret.data.ଠେଲ_back(data[offset+i]);
		ret.width = length;
	}
	ଫେରନ୍ତୁ ret;
}

RTLIL::SigBit RTLIL::SigChunk::operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset) ଅଚଳ
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	ଯଦି (wire) {
		ret.wire = wire;
		ret.offset = ଏହି->offset + offset;
	} ଅନ୍ୟଥା {
		ret.data = data[offset];
	}
	ଫେରନ୍ତୁ ret;
}

ତାର୍କିକ RTLIL::SigChunk::operator <(ଅଚଳ RTLIL::SigChunk &other) ଅଚଳ
{
	ଯଦି (wire && other.wire)
		ଯଦି (wire->name != other.wire->name)
			ଫେରନ୍ତୁ wire->name < other.wire->name;

	ଯଦି (wire != other.wire)
		ଫେରନ୍ତୁ wire < other.wire;

	ଯଦି (offset != other.offset)
		ଫେରନ୍ତୁ offset < other.offset;

	ଯଦି (width != other.width)
		ଫେରନ୍ତୁ width < other.width;

	ଫେରନ୍ତୁ data < other.data;
}

ତାର୍କିକ RTLIL::SigChunk::operator ==(ଅଚଳ RTLIL::SigChunk &other) ଅଚଳ
{
	ଫେରନ୍ତୁ wire == other.wire && width == other.width && offset == other.offset && data == other.data;
}

ତାର୍କିକ RTLIL::SigChunk::operator !=(ଅଚଳ RTLIL::SigChunk &other) ଅଚଳ
{
	ଯଦି (*ଏହି == other)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.size() > 0);
	auto ie = parts.begin();
	auto it = ie + parts.size() - 1;
	ଯେତେବେଳେ (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ଅଚଳ RTLIL::Const &ମୂଲ୍ୟ)
{
	ଯଦି (GetSize(ମୂଲ୍ୟ) != 0) {
		rep_ = CHUNK;
		ନୂତନ (&chunk_) RTLIL::SigChunk(ମୂଲ୍ୟ);
	} ଅନ୍ୟଥା {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&ମୂଲ୍ୟ)
{
	ଯଦି (GetSize(ମୂଲ୍ୟ) != 0) {
		rep_ = CHUNK;
		ନୂତନ (&chunk_) RTLIL::SigChunk(ମୂଲ୍ୟ);
	} ଅନ୍ୟଥା {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ଅଚଳ RTLIL::SigChunk &chunk)
{
	ଯଦି (chunk.width != 0) {
		rep_ = CHUNK;
		ନୂତନ (&chunk_) RTLIL::SigChunk(chunk);
	} ଅନ୍ୟଥା {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	ଯଦି (chunk.width != 0) {
		rep_ = CHUNK;
		ନୂତନ (&chunk_) RTLIL::SigChunk(chunk);
	} ଅନ୍ୟଥା {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	ଯଦି (wire->width != 0) {
		rep_ = CHUNK;
		ନୂତନ (&chunk_) RTLIL::SigChunk(wire);
	} ଅନ୍ୟଥା {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
	ଯଦି (width != 0) {
		rep_ = CHUNK;
		ନୂତନ (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} ଅନ୍ୟଥା {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ଅଚଳ std::string &str)
{
	ଯଦି (str.size() != 0) {
		rep_ = CHUNK;
		ନୂତନ (&chunk_) RTLIL::SigChunk(str);
	} ଅନ୍ୟଥା {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ପୂର୍ଣ୍ଣସଂଖ୍ୟା val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
	ଯଦି (width != 0) {
		rep_ = CHUNK;
		ନୂତନ (&chunk_) RTLIL::SigChunk(val, width);
	} ଅନ୍ୟଥା
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
	ଯଦି (width != 0) {
		rep_ = CHUNK;
		ନୂତନ (&chunk_) RTLIL::SigChunk(bit, width);
	} ଅନ୍ୟଥା
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ଅଚଳ RTLIL::SigBit &bit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
	ଯଦି (width != 0) {
		ଯଦି (bit.wire == ଖାଲି) {
			rep_ = CHUNK;
			ନୂତନ (&chunk_) RTLIL::SigChunk(bit.data, width);
		} ଅନ୍ୟଥା ଯଦି (width == 1) {
			rep_ = CHUNK;
			ନୂତନ (&chunk_) RTLIL::SigChunk(bit);
		} ଅନ୍ୟଥା {
			init_empty_bits();
			bits_.reserve(width);
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++)
				bits_.ଠେଲ_back(bit);
		}
	} ଅନ୍ୟଥା
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(ଅଚଳ std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	ପାଇଁ (ଅଚଳ auto &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ଅଚଳ std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	ପାଇଁ (ଅଚଳ auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ଅଚଳ pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	ପାଇଁ (ଅଚଳ auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ଅଚଳ std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	ପାଇଁ (ଅଚଳ auto &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ତାର୍କିକ bit)
{
	rep_ = CHUNK;
	ନୂତନ (&chunk_) RTLIL::SigChunk(bit ? RTLIL::S1 : RTLIL::S0);
	check();
}

ଶୂନ୍ୟ RTLIL::SigSpec::Chunks::const_iterator::next_chunk_bits()
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits_size = GetSize(spec.bits_);
	ଯଦି (bit_index >= bits_size)
		ଫେରନ୍ତୁ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = bit_index;
	ଅଚଳ SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.data.clear();
	ଯଦି (bit.is_wire()) {
		chunk.offset = bit.offset;
		ଯେତେବେଳେ (i < bits_size && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_index)
			++i;
	} ଅନ୍ୟଥା {
		chunk.offset = 0;
		chunk.data.ଠେଲ_back(bit.data);
		ଯେତେବେଳେ (i < bits_size && !spec.bits_[i].is_wire()) {
			chunk.data.ଠେଲ_back(spec.bits_[i].data);
			++i;
		}
	}
	chunk.width = i - bit_index;
}

ଶୂନ୍ୟ RTLIL::SigSpec::unpack()
{
	ଯଦି (rep_ == BITS)
		ଫେରନ୍ତୁ;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	ନୂତନ (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

ଶୂନ୍ୟ RTLIL::SigSpec::try_repack()
{
	ଯଦି (rep_ != BITS)
		ଫେରନ୍ତୁ;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bits_size = GetSize(bits_);
	ଯଦି (bits_size == 0)
		ଫେରନ୍ତୁ;
	ଯଦି (bits_[0].is_wire()) {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < bits_size; i++)
			ଯଦି (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				ଫେରନ୍ତୁ;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_size);
		bits_.~vector();
		rep_ = CHUNK;
		ନୂତନ (&chunk_) SigChunk(std::move(chunk));
		ଫେରନ୍ତୁ;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_size);
	bits.ଠେଲ_back(bits_[0].data);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < bits_size; i++) {
		ଯଦି (bits_[i].is_wire())
			ଫେରନ୍ତୁ;
		bits.ଠେଲ_back(bits_[i].data);
	}
	bits_.~vector();
	rep_ = CHUNK;
	ନୂତନ (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ଅଚଳ
{
	Hasher h;
	ପାଇଁ (auto &c : chunks())
		ଯଦି (c.wire == ଖାଲି) {
			ପାଇଁ (auto &v : c.data)
				h.eat(v);
		} ଅନ୍ୟଥା {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t result = h.yield();
	ଯଦି (result == 0)
		result = 1;
	hash_.set(result);
	ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ RTLIL::SigSpec::sort()
{
	unpack();
	std::sort(bits_.begin(), bits_.end());
	hash_.clear();
	try_repack();
}

ଶୂନ୍ୟ RTLIL::SigSpec::sort_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::vector<SigBit> unique_bits = bits_;
	std::sort(unique_bits.begin(), unique_bits.end());
	auto last = std::unique(unique_bits.begin(), unique_bits.end());
	unique_bits.erase(last, unique_bits.end());

	*ଏହି = unique_bits;
	hash_.clear();
	try_repack();
}

ଶୂନ୍ୟ RTLIL::SigSpec::replace(ଅଚଳ RTLIL::SigSpec &pattern, ଅଚଳ RTLIL::SigSpec &with)
{
	replace(pattern, with, ଏହି);
}

ଶୂନ୍ୟ RTLIL::SigSpec::replace(ଅଚଳ RTLIL::SigSpec &pattern, ଅଚଳ RTLIL::SigSpec &with, RTLIL::SigSpec *other) ଅଚଳ
{
	log_assert(other != ଖାଲି);
	log_assert(size() == other->size());
	log_assert(pattern.size() == with.size());

	dict<RTLIL::SigBit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା> pattern_to_with;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pattern_size = pattern.size();
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < pattern_size; i++) {
		SigBit pattern_bit = pattern[i];
		ଯଦି (pattern_bit.wire != ଖାଲି) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା this_size = size();
	ତାର୍କିକ other_modified = ମିଥ୍ୟା;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < this_size; j++) {
		auto it = pattern_to_with.find((*ଏହି)[j]);
		ଯଦି (it != pattern_to_with.end()) {
			other_modified = ସତ୍ୟ;
			(*other)[j] = with[it->second];
		}
	}

	ଯଦି (other_modified)
		other->try_repack();
	other->check();
}

ଶୂନ୍ୟ RTLIL::SigSpec::replace(ଅଚଳ dict<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, ଏହି);
}

ଶୂନ୍ୟ RTLIL::SigSpec::replace(ଅଚଳ dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ଅଚଳ
{
	log_assert(other != ଖାଲି);
	log_assert(size() == other->size());

	ଯଦି (rules.empty()) ଫେରନ୍ତୁ;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା this_size = size();
	ତାର୍କିକ other_modified = ମିଥ୍ୟା;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < this_size; i++) {
		auto it = rules.find((*ଏହି)[i]);
		ଯଦି (it != rules.end()) {
			other_modified = ସତ୍ୟ;
			(*other)[i] = it->second;
		}
	}

	ଯଦି (other_modified)
		other->try_repack();
	other->check();
}

ଶୂନ୍ୟ RTLIL::SigSpec::replace(ଅଚଳ std::map<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, ଏହି);
}

ଶୂନ୍ୟ RTLIL::SigSpec::replace(ଅଚଳ std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ଅଚଳ
{
	log_assert(other != ଖାଲି);
	log_assert(size() == other->size());

	ଯଦି (rules.empty()) ଫେରନ୍ତୁ;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା this_size = size();
	ତାର୍କିକ other_modified = ମିଥ୍ୟା;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < this_size; i++) {
		auto it = rules.find((*ଏହି)[i]);
		ଯଦି (it != rules.end()) {
			other_modified = ସତ୍ୟ;
			(*other)[i] = it->second;
		}
	}

	ଯଦି (other_modified)
		other->try_repack();
	other->check();
}

ଶୂନ୍ୟ RTLIL::SigSpec::remove(ଅଚଳ RTLIL::SigSpec &pattern)
{
	remove2(pattern, ଖାଲି);
}

ଶୂନ୍ୟ RTLIL::SigSpec::remove(ଅଚଳ RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) ଅଚଳ
{
	RTLIL::SigSpec tmp = *ଏହି;
	tmp.remove2(pattern, other);
}

ଶୂନ୍ୟ RTLIL::SigSpec::remove2(ଅଚଳ RTLIL::SigSpec &pattern, RTLIL::SigSpec *other)
{
	unpack();
	ଯଦି (other != ଖାଲି) {
		log_assert(size() == other->size());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.size());
	ପାଇଁ (auto &bit : pattern)
		ଯଦି (bit.wire != ଖାଲି)
			pattern_bits.ଭର୍ତ୍ତି(bit);

	// Compact in-place to avoid O(n^2) erase operations
	size_t write_idx = 0;
	ପାଇଁ (size_t read_idx = 0; read_idx < bits_.size(); read_idx++)
	{
		ଯଦି (!(bits_[read_idx].wire != ଖାଲି && pattern_bits.ଗଣନା_2(bits_[read_idx]))) {
			ଯଦି (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				ଯଦି (other != ଖାଲି)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	ତାର୍କିକ modified = (write_idx < bits_.size());
	ଯଦି (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ଯଦି (other != ଖାଲି && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ଶୂନ୍ୟ RTLIL::SigSpec::remove(ଅଚଳ pool<RTLIL::SigBit> &pattern)
{
	remove2(pattern, ଖାଲି);
}

ଶୂନ୍ୟ RTLIL::SigSpec::remove(ଅଚଳ pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) ଅଚଳ
{
	RTLIL::SigSpec tmp = *ଏହି;
	tmp.remove2(pattern, other);
}

ଶୂନ୍ୟ RTLIL::SigSpec::remove2(ଅଚଳ pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	ଯଦି (other != ଖାଲି) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	ପାଇଁ (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		ଯଦି (!(bits_[read_idx].wire != ଖାଲି && pattern.ଗଣନା_2(bits_[read_idx]))) {
			ଯଦି (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				ଯଦି (other != ଖାଲି)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	ତାର୍କିକ modified = (write_idx < bits_.size());
	ଯଦି (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ଯଦି (other != ଖାଲି && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ଶୂନ୍ୟ RTLIL::SigSpec::remove2(ଅଚଳ std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	ଯଦି (other != ଖାଲି) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	size_t write_idx = 0;
	ପାଇଁ (size_t read_idx = 0; read_idx < bits_.size(); read_idx++) {
		ଯଦି (!(bits_[read_idx].wire != ଖାଲି && pattern.ଗଣନା_2(bits_[read_idx]))) {
			ଯଦି (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				ଯଦି (other != ଖାଲି)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	ତାର୍କିକ modified = (write_idx < bits_.size());
	ଯଦି (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ଯଦି (other != ଖାଲି && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

ଶୂନ୍ୟ RTLIL::SigSpec::remove2(ଅଚଳ pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	ଯଦି (other != ଖାଲି) {
		log_assert(size() == other->size());
		other->unpack();
	}

	ତାର୍କିକ modified = ମିଥ୍ୟା;
	ତାର୍କିକ other_modified = ମିଥ୍ୟା;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = GetSize(bits_) - 1; i >= 0; i--) {
		ଯଦି (bits_[i].wire != ଖାଲି && pattern.ଗଣନା_2(bits_[i].wire)) {
			modified = ସତ୍ୟ;
			bits_.erase(bits_.begin() + i);
			ଯଦି (other != ଖାଲି) {
				other_modified = ସତ୍ୟ;
				other->bits_.erase(other->bits_.begin() + i);
			}
		}
	}

	ଯଦି (modified) {
		hash_.clear();
		try_repack();
	}
	ଯଦି (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ଅଚଳ RTLIL::SigSpec &pattern, ଅଚଳ RTLIL::SigSpec *other) ଅଚଳ
{
	log_assert(other == ଖାଲି || size() == other->size());

	RTLIL::SigSpec ret;
	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();

	ପାଇଁ (auto& pattern_chunk : pattern.chunks()) {
		ଯଦି (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
			ପାଇଁ (ଅଚଳ RTLIL::SigBit &bit : bits_match) {
				ଯଦି (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} ଅନ୍ୟଥା {
			ପାଇଁ (ଅଚଳ RTLIL::SigBit &bit : bits_match)
				ଯଦି (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	ଫେରନ୍ତୁ ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ଅଚଳ pool<RTLIL::SigBit> &pattern, ଅଚଳ RTLIL::SigSpec *other) ଅଚଳ
{
	log_assert(other == ଖାଲି || size() == other->size());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	ଯଦି (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
		ପାଇଁ (ଅଚଳ RTLIL::SigBit &bit : bits_match) {
			ଯଦି (bit.wire && pattern.ଗଣନା_2(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} ଅନ୍ୟଥା {
		ପାଇଁ (ଅଚଳ RTLIL::SigBit &bit : bits_match)
			ଯଦି (bit.wire && pattern.ଗଣନା_2(bit))
				ret.append(bit);
	}

	ret.check();
	ଫେରନ୍ତୁ ret;
}

ଶୂନ୍ୟ RTLIL::SigSpec::replace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ଅଚଳ RTLIL::SigSpec &with)
{
	ଯଦି (with.size() == 0)
		ଫେରନ୍ତୁ;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.size() >= 0);
	log_assert(offset+with.size() <= size());

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
	ପାଇଁ (ଅଚଳ RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

ଶୂନ୍ୟ RTLIL::SigSpec::remove_const()
{
	ଯଦି (rep_ == CHUNK)
	{
		ଯଦି (chunk_.wire == ଖାଲି) {
			chunk_.~SigChunk();
			init_empty_bits();
			hash_.clear();
		}
	}
	ଅନ୍ୟଥା
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.size());
		ପାଇଁ (auto &bit : bits_)
			ଯଦି (bit.wire != ଖାଲି)
				new_bits.ଠେଲ_back(bit);
		ଯଦି (GetSize(new_bits) != GetSize(bits_)) {
			bits_.swap(new_bits);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

ଶୂନ୍ୟ RTLIL::SigSpec::remove(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା length)
{
	ଯଦି (length == 0)
		ଫେରନ୍ତୁ;

	unpack();

	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	bits_.erase(bits_.begin() + offset, bits_.begin() + offset + length);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା length) ଅଚଳ
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	std::vector<SigBit> extracted;
	SigBit first;
	ତାର୍କିକ is_packing = ସତ୍ୟ;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = offset; i < offset + length; i++) {
		ତାର୍କିକ was_packing_before = is_packing;
		SigBit bit = (*ଏହି)[i];
		ଯଦି (i == offset) {
			first = bit;
			ଯଦି (!bit.wire)
				is_packing = ମିଥ୍ୟା;
		} ଅନ୍ୟଥା {
			ଯଦି (bit.wire != first.wire)
				is_packing = ମିଥ୍ୟା;
			ଯଦି (bit.wire)
				ଯଦି (bit.offset != first.offset + (i - offset))
					is_packing = ମିଥ୍ୟା;
		}
		ଯଦି (was_packing_before && !is_packing)
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = offset; j < i; j++)
				extracted.ଠେଲ_back((*ଏହି)[j]);
		ଯଦି (!is_packing)
			extracted.ଠେଲ_back((*ଏହି)[i]);
	}
	ଯଦି (is_packing)
		ଫେରନ୍ତୁ SigChunk(first.wire, first.offset, length);

	ଫେରନ୍ତୁ extracted;
}

ଶୂନ୍ୟ RTLIL::SigSpec::rewrite_wires(std::function<ଶୂନ୍ୟ(RTLIL::Wire*& wire)> rewrite)
{
	ଯଦି (rep_ == CHUNK) {
		ଯଦି (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		ଫେରନ୍ତୁ;
	}

	std::vector<RTLIL::SigBit> new_bits;
	ପାଇଁ (ଅଚଳ RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		ଯଦି (c.wire != nullptr)
			rewrite(c.wire);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.clear();
}

ଶୂନ୍ୟ RTLIL::SigSpec::append(ଅଚଳ RTLIL::SigSpec &ସଙ୍କେତ)
{
	ଯଦି (ସଙ୍କେତ.size() == 0)
		ଫେରନ୍ତୁ;

	ଯଦି (size() == 0) {
		*ଏହି = ସଙ୍କେତ;
		ଫେରନ୍ତୁ;
	}

	hash_.clear();
	ଯଦି (rep_ == CHUNK && ସଙ୍କେତ.rep_ == CHUNK && chunk_.wire == ସଙ୍କେତ.chunk_.wire) {
		ଯଦି (chunk_.wire == ଖାଲି) {
			chunk_.data.ଭର୍ତ୍ତି(chunk_.data.end(), ସଙ୍କେତ.chunk_.data.begin(), ସଙ୍କେତ.chunk_.data.end());
			chunk_.width = GetSize(chunk_.data);
			ଫେରନ୍ତୁ;
		}
		ଯଦି (chunk_.offset + chunk_.width == ସଙ୍କେତ.chunk_.offset) {
			chunk_.width += ସଙ୍କେତ.chunk_.width;
			ଫେରନ୍ତୁ;
		}
	}

	unpack();
	ପାଇଁ (ଅଚଳ SigBit &bit : ସଙ୍କେତ.bits())
		bits_.ଠେଲ_back(bit);
	check();
}

ଶୂନ୍ୟ RTLIL::SigSpec::append(ଅଚଳ RTLIL::SigBit &bit)
{
	hash_.clear();

	ଯଦି (size() == 0) {
		destroy();
		rep_ = CHUNK;
		ନୂତନ (&chunk_) RTLIL::SigChunk(bit);
		ଫେରନ୍ତୁ;
	}

	ଯଦି (rep_ == CHUNK && chunk_.wire == bit.wire) {
		ଯଦି (chunk_.wire == ଖାଲି) {
			chunk_.data.ଠେଲ_back(bit.data);
			chunk_.width++;
			ଫେରନ୍ତୁ;
		}
		ଯଦି (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			ଫେରନ୍ତୁ;
		}
	}

	unpack();

	bits_.ଠେଲ_back(bit);
	check();
}

ଶୂନ୍ୟ RTLIL::SigSpec::extend_u0(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ତାର୍କିକ is_signed)
{
	ଯଦି (size() > width)
		remove(width, size() - width);

	ଯଦି (size() < width) {
		RTLIL::SigBit padding = size() > 0 ? (*ଏହି)[size() - 1] : RTLIL::State::Sx;
		ଯଦି (!is_signed)
			padding = RTLIL::State::S0;
		ଯେତେବେଳେ (size() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ) ଅଚଳ
{
	RTLIL::SigSpec sig;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ଅଙ୍କ; i++)
		sig.append(*ଏହି);
	ଫେରନ୍ତୁ sig;
}

#ifndef NDEBUG
ଶୂନ୍ୟ RTLIL::SigSpec::check(ଅଚଳ Module *mod) ଅଚଳ
{
	ଯଦି (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		ଯଦି (chunk_.wire == ଖାଲି) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.data.size() == (size_t)chunk_.width);
		} ଅନ୍ୟଥା {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.data.size() == 0);
			ଯଦି (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	ଅନ୍ୟଥା ଯଦି (size() <= 64)
	{
		ଯଦି (mod != nullptr) {
			ପାଇଁ (ଅଚଳ RTLIL::SigBit &bit : bits_)
				ଯଦି (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#endif

ତାର୍କିକ RTLIL::SigSpec::operator <(ଅଚଳ RTLIL::SigSpec &other) ଅଚଳ
{
	ଯଦି (ଏହି == &other)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	ଯଦି (size() != other.size())
		ଫେରନ୍ତୁ size() < other.size();

	auto other_it = other.chunks().begin();
	ପାଇଁ (ଅଚଳ SigChunk &c : chunks()) {
		ଯଦି (c != *other_it)
			ଫେରନ୍ତୁ c < *other_it;
		++other_it;
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ RTLIL::SigSpec::operator ==(ଅଚଳ RTLIL::SigSpec &other) ଅଚଳ
{
	ଯଦି (ଏହି == &other)
		ଫେରନ୍ତୁ ସତ୍ୟ;

	ଯଦି (size() != other.size())
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	auto other_it = other.chunks().begin();
	ପାଇଁ (ଅଚଳ SigChunk &c : chunks()) {
		ଯଦି (c != *other_it)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		++other_it;
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::SigSpec::is_wire() ଅଚଳ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	ଯଦି (it == cs.end())
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଅଚଳ RTLIL::SigChunk &chunk = *it;
	ଫେରନ୍ତୁ chunk.wire && chunk.wire->width == size() && ++it == cs.end();
}

ତାର୍କିକ RTLIL::SigSpec::is_chunk() ଅଚଳ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	ଯଦି (it == cs.end())
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଫେରନ୍ତୁ ++it == cs.end();
}

ତାର୍କିକ RTLIL::SigSpec::known_driver() ଅଚଳ
{
	ପାଇଁ (auto &chunk : chunks())
		ଯଦି (chunk.is_wire() && !chunk.wire->known_driver())
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::SigSpec::is_fully_const() ଅଚଳ
{
	ପାଇଁ (auto &chunk : chunks())
		ଯଦି (chunk.width > 0 && chunk.wire != ଖାଲି)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::SigSpec::is_fully_zero() ଅଚଳ
{
	ପାଇଁ (auto &chunk : chunks()) {
		ଯଦି (chunk.width > 0 && chunk.wire != ଖାଲି)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		ପାଇଁ (RTLIL::State d : chunk.data)
			ଯଦି (d != RTLIL::State::S0)
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::SigSpec::is_fully_ones() ଅଚଳ
{
	ପାଇଁ (auto &chunk : chunks()) {
		ଯଦି (chunk.width > 0 && chunk.wire != ଖାଲି)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		ପାଇଁ (RTLIL::State d : chunk.data)
			ଯଦି (d != RTLIL::State::S1)
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::SigSpec::is_fully_def() ଅଚଳ
{
	ପାଇଁ (auto &chunk : chunks()) {
		ଯଦି (chunk.width > 0 && chunk.wire != ଖାଲି)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		ପାଇଁ (RTLIL::State d : chunk.data)
			ଯଦି (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::SigSpec::is_fully_undef() ଅଚଳ
{
	ପାଇଁ (auto &chunk : chunks()) {
		ଯଦି (chunk.width > 0 && chunk.wire != ଖାଲି)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		ପାଇଁ (RTLIL::State d : chunk.data)
			ଯଦି (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::SigSpec::has_const() ଅଚଳ
{
	ପାଇଁ (auto &chunk : chunks())
		ଯଦି (chunk.width > 0 && chunk.wire == ଖାଲି)
			ଫେରନ୍ତୁ ସତ୍ୟ;
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ RTLIL::SigSpec::has_const(State state) ଅଚଳ
{
	ପାଇଁ (auto &chunk : chunks())
		ଯଦି (chunk.width > 0 && chunk.wire == ଖାଲି && std::find(chunk.data.begin(), chunk.data.end(), state) != chunk.data.end())
			ଫେରନ୍ତୁ ସତ୍ୟ;
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}


ତାର୍କିକ RTLIL::SigSpec::has_marked_bits() ଅଚଳ
{
	ପାଇଁ (auto &chunk : chunks())
		ଯଦି (chunk.width > 0 && chunk.wire == ଖାଲି) {
			ପାଇଁ (RTLIL::State d : chunk.data)
				ଯଦି (d == RTLIL::State::Sm)
					ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ RTLIL::SigSpec::is_onehot(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pos) ଅଚଳ
{
	ଯଦି (std::optional<RTLIL::Const> c = try_as_const())
		ଫେରନ୍ତୁ c->is_onehot(pos);
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ RTLIL::SigSpec::as_bool() ଅଚଳ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ମୂଲ୍ୟ());
	ଫେରନ୍ତୁ c->as_bool();
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା RTLIL::SigSpec::as_int(ତାର୍କିକ is_signed) ଅଚଳ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ମୂଲ୍ୟ());
	ଫେରନ୍ତୁ c->as_int(is_signed);
}

ତାର୍କିକ RTLIL::SigSpec::convertible_to_int(ତାର୍କିକ is_signed) ଅଚଳ
{
	std::optional<RTLIL::Const> c = try_as_const();
	ଯଦି (!c.has_ମୂଲ୍ୟ())
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଫେରନ୍ତୁ c->convertible_to_int(is_signed);
}

std::optional<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> RTLIL::SigSpec::try_as_int(ତାର୍କିକ is_signed) ଅଚଳ
{
	std::optional<RTLIL::Const> c = try_as_const();
	ଯଦି (!c.has_ମୂଲ୍ୟ())
		ଫେରନ୍ତୁ std::nullopt;
	ଫେରନ୍ତୁ c->try_as_int(is_signed);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା RTLIL::SigSpec::as_int_saturating(ତାର୍କିକ is_signed) ଅଚଳ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ମୂଲ୍ୟ());
	ଫେରନ୍ତୁ c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ଅଚଳ
{
	std::string str;
	str.reserve(size());
	std::vector<RTLIL::SigChunk> chunks = *ଏହି;
	ପାଇଁ (size_t i = chunks.size(); i > 0; i--) {
		ଅଚଳ RTLIL::SigChunk &chunk = chunks[i-1];
		ଯଦି (chunk.wire != ଖାଲି)
			str.append(chunk.width, '?');
		ଅନ୍ୟଥା
			str += RTLIL::Const(chunk.data).as_string();
	}
	ଫେରନ୍ତୁ str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ଅଚଳ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	ଯଦି (it == cs.end())
		ଫେରନ୍ତୁ RTLIL::Const();
	SigChunk chunk = *it;
	ଯଦି (chunk.wire != ଖାଲି || ++it != cs.end())
		ଫେରନ୍ତୁ std::nullopt;
	ଫେରନ୍ତୁ RTLIL::Const(std::move(chunk.data));
}

RTLIL::Const RTLIL::SigSpec::as_const() ଅଚଳ
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_ମୂଲ୍ୟ());
	ଫେରନ୍ତୁ *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ଅଚଳ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == size());
	ଫେରନ୍ତୁ chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ଅଚଳ
{
	Chunks cs = chunks();
	auto it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	ଫେରନ୍ତୁ chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() ଅଚଳ
{
	ଫେରନ୍ତୁ RTLIL::SigBit(*ଏହି);
}

ତାର୍କିକ RTLIL::SigSpec::match(ଅଚଳ ଅକ୍ଷର* pattern) ଅଚଳ
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା pattern_len = strlen(pattern);
	log_assert(pattern_len == size());

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < pattern_len; ++i) {
		ଅକ୍ଷର ch = pattern[i];
		ଯଦି (ch == ' ')
			ଜାରି;
		RTLIL::SigBit bit = (*ଏହି)[pattern_len - 1 - i];
		ଯଦି (ch == '*') {
			ଯଦି (bit != State::Sz && bit != State::Sx)
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
			ଜାରି;
		}
		ଯଦି (ch == '0') {
			ଯଦି (bit != State::S0)
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
		} ଅନ୍ୟଥା
		ଯଦି (ch == '1') {
			ଯଦି (bit != State::S1)
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
		} ଅନ୍ୟଥା
			log_abort();
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() ଅଚଳ
{
	std::set<RTLIL::SigBit> sigbits;
	ପାଇଁ (auto &c : chunks())
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < c.width; i++)
			sigbits.ଭର୍ତ୍ତି(RTLIL::SigBit(c, i));
	ଫେରନ୍ତୁ sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() ଅଚଳ
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(size());
	ପାଇଁ (auto &c : chunks())
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < c.width; i++)
			sigbits.ଭର୍ତ୍ତି(RTLIL::SigBit(c, i));
	ଫେରନ୍ତୁ sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() ଅଚଳ
{
	std::vector<RTLIL::SigBit> result;
	result.reserve(size());
	ପାଇଁ (SigBit bit : *ଏହି)
		result.ଠେଲ_back(bit);
	ଫେରନ୍ତୁ result;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(ଅଚଳ RTLIL::SigSpec &other) ଅଚଳ
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା this_size = size();
	log_assert(this_size == other.size());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < this_size; i++)
		new_map[(*ଏହି)[i]] = other[i];

	ଫେରନ୍ତୁ new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(ଅଚଳ RTLIL::SigSpec &other) ଅଚଳ
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା this_size = size();
	log_assert(this_size == other.size());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_size);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < this_size; i++)
		new_map[(*ଏହି)[i]] = other[i];

	ଫେରନ୍ତୁ new_map;
}

ସ୍ଥିର ଶୂନ୍ୟ sigspec_parse_split(std::vector<std::string> &tokens, ଅଚଳ std::string &text, ଅକ୍ଷର sep)
{
	size_t start = 0, end = 0;
	ଯେତେବେଳେ ((end = text.find(sep, start)) != std::string::npos) {
		tokens.ଠେଲ_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.ଠେଲ_back(text.substr(start));
}

ସ୍ଥିର RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ଅଚଳ std::string &netname)
{
	ଫେରନ୍ତୁ module->wire(netname);
}

ତାର୍କିକ RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା tokidx = ପୂର୍ଣ୍ଣସଂଖ୍ୟା(tokens.size())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		ଯଦି (netname.size() == 0)
			ଜାରି;

		ଯଦି (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			auto ast = p.const2ast(netname);
			ଯଦି (ast == nullptr)
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
			sig.append(RTLIL::Const(ast->bits));
			ଜାରି;
		}

		ଯଦି (module == ଖାଲି)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;

		ଯଦି (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		ଯଦି (sigspec_parse_wire(module, netname) == nullptr) {
			size_t indices_pos = netname.size()-1;
			ଯଦି (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				ଯେତେବେଳେ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				ଯଦି (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					ଯେତେବେଳେ (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				ଯଦି (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		ଯଦି (wire == nullptr)
			ଫେରନ୍ତୁ ମିଥ୍ୟା;

		ଯଦି (!indices.empty()) {
			std::vector<std::string> index_tokens;
			sigspec_parse_split(index_tokens, indices.substr(1, indices.size()-2), ':');
			ଯଦି (index_tokens.size() == 1) {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା a = atoi(index_tokens.at(0).c_str());
				ଯଦି (a < 0 || a >= wire->width)
					ଫେରନ୍ତୁ ମିଥ୍ୟା;
				sig.append(RTLIL::SigSpec(wire, a));
			} ଅନ୍ୟଥା {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା a = atoi(index_tokens.at(0).c_str());
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା b = atoi(index_tokens.at(1).c_str());
				ଯଦି (a > b) {
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp = a;
					a = b, b = tmp;
				}
				ଯଦି (a < 0 || a >= wire->width)
					ଫେରନ୍ତୁ ମିଥ୍ୟା;
				ଯଦି (b < 0 || b >= wire->width)
					ଫେରନ୍ତୁ ମିଥ୍ୟା;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} ଅନ୍ୟଥା
			sig.append(wire);
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	ଯଦି (str.empty() || str[0] != '@')
		ଫେରନ୍ତୁ parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	ଯଦି (sel_name == IdString::Null || design->selection_vars.ଗଣନା_2(sel_name) == 0)
		ଫେରନ୍ତୁ ମିଥ୍ୟା;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	ପାଇଁ (auto &it : module->wires_)
		ଯଦି (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ RTLIL::SigSpec::parse_rhs(ଅଚଳ RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	ଯଦି (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.size());
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.size());
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (lhs.is_chunk()) {
		ଅକ୍ଷର *p = (ଅକ୍ଷର*)str.c_str(), *endptr;
		long ପୂର୍ଣ୍ଣସଂଖ୍ୟା val = strtol(p, &endptr, 10);
		ଯଦି (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.size());
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	}

	ଯଦି (!parse(sig, module, str))
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଯଦି (sig.size() > lhs.size())
		sig.remove(lhs.size(), sig.size() - lhs.size());
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

RTLIL::SigSpec::operator std::vector<RTLIL::SigChunk>() ଅଚଳ
{
	std::vector<RTLIL::SigChunk> result;
	ପାଇଁ (ଅଚଳ RTLIL::SigChunk &c : chunks())
		result.ଠେଲ_back(c);
	ଫେରନ୍ତୁ result;
}

RTLIL::CaseRule::~CaseRule()
{
	ପାଇଁ (auto it = switches.begin(); it != switches.end(); it++)
		delete *it;
}

ତାର୍କିକ RTLIL::CaseRule::empty() ଅଚଳ
{
	ଫେରନ୍ତୁ actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() ଅଚଳ
{
	RTLIL::CaseRule *new_caserule = ନୂତନ RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	ପାଇଁ (auto &it : switches)
		new_caserule->switches.ଠେଲ_back(it->clone());
	ଫେରନ୍ତୁ new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	ପାଇଁ (auto it = cases.begin(); it != cases.end(); it++)
		delete *it;
}

ତାର୍କିକ RTLIL::SwitchRule::empty() ଅଚଳ
{
	ଫେରନ୍ତୁ cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() ଅଚଳ
{
	RTLIL::SwitchRule *new_switchrule = ନୂତନ RTLIL::SwitchRule;
	new_switchrule->ସଙ୍କେତ = ସଙ୍କେତ;
	new_switchrule->attributes = attributes;
	ପାଇଁ (auto &it : cases)
		new_switchrule->cases.ଠେଲ_back(it->clone());
	ଫେରନ୍ତୁ new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() ଅଚଳ
{
	RTLIL::SyncRule *new_syncrule = ନୂତନ RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->ସଙ୍କେତ = ସଙ୍କେତ;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	ଫେରନ୍ତୁ new_syncrule;
}

RTLIL::Process::~Process()
{
	ପାଇଁ (auto it = syncs.begin(); it != syncs.end(); it++)
		delete *it;
}

RTLIL::Process *RTLIL::Process::clone() ଅଚଳ
{
	RTLIL::Process *new_proc = ନୂତନ RTLIL::Process;

	new_proc->attributes = attributes;

	RTLIL::CaseRule *rc_ptr = ମୂଳ_case.clone();
	new_proc->ମୂଳ_case = *rc_ptr;
	rc_ptr->switches.clear();
	delete rc_ptr;

	ପାଇଁ (auto &it : syncs)
		new_proc->syncs.ଠେଲ_back(it->clone());

	ଫେରନ୍ତୁ new_proc;
}

RTLIL::Memory::~Memory()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
ସ୍ଥିର std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Memory*> all_memorys;
std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Memory*> *RTLIL::Memory::get_all_memorys(ଶୂନ୍ୟ)
{
	ଫେରନ୍ତୁ &all_memorys;
}
#endif
YOSYS_NAMESPACE_END
