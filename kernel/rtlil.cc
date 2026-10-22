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

#అన్తర్భావ "kernel/rtlil.h"
#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/macc.h"
#అన్తర్భావ "kernel/newcelltypes.h"
#అన్తర్భావ "kernel/sigtools.h"
#అన్తర్భావ "kernel/threading.h"
#అన్తర్భావ "frontends/verilog/verilog_frontend.h"
#అన్తర్భావ "frontends/verilog/preproc.h"
#అన్తర్భావ "backends/rtlil/rtlil_backend.h"

#అన్తర్భావ <string.h>
#అన్తర్భావ <algorithm>
#అన్తర్భావ <charconv>
#అన్తర్భావ <optional>
#అన్తర్భావ <string_view>
#అన్తర్భావ <sstream>

YOSYS_NAMESPACE_BEGIN

dict<std::string, std::string> RTLIL::constpad;

స్థిరరాశి pool<IdString> &RTLIL::builtin_ff_cell_types() {
	స్థిర స్థిరరాశి pool<IdString> res = []() {
		pool<IdString> r;
		కొరకు (కొలత_t i = 0; i < StaticCellTypes::builder.లెక్క; i++) {
			స్వయంచాలక &cell = StaticCellTypes::builder.cells[i];
			ఒకవేళ (cell.features.is_ff)
				r.చొప్పించు(cell.type);
		}
		తిరిగి r;
	}();
	తిరిగి res;
}

#నిర్వచించు check(condition) log_assert(condition && "malformed Const union")

స్థిరరాశి Const::bitvectype& Const::get_బిట్లు() స్థిరరాశి {
	check(is_బిట్లు());
	తిరిగి *get_if_బిట్లు();
}

స్థిరరాశి std::string& Const::get_str() స్థిరరాశి {
	check(is_str());
	తిరిగి *get_if_str();
}

Const::bitvectype& Const::get_బిట్లు() {
	check(is_బిట్లు());
	తిరిగి *get_if_బిట్లు();
}

std::string& Const::get_str() {
	check(is_str());
	తిరిగి *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	కొత్త ((శూన్య*)&str_) std::string(std::move(str));
	ట్యాగ్ = backing_ట్యాగ్::string;
}

RTLIL::Const::Const(దీర్ఘ దీర్ఘ val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	అక్షరం బైట్లు[] = {
		(అక్షరం)(val >> 24), (అక్షరం)(val >> 16), (అక్షరం)(val >> 8), (అక్షరం)val
	};
	కొత్త ((శూన్య*)&str_) std::string(బైట్లు, 4);
	ట్యాగ్ = backing_ట్యాగ్::string;
}

RTLIL::Const::Const(దీర్ఘ దీర్ఘ val, పూర్ణాంకం వెడల్పు)
{
	log_assert(వెడల్పు >= 0 && వెడల్పు < RTLIL::వెడల్పు_హద్దు);
	flags = RTLIL::CONST_FLAG_NONE;
	ఒకవేళ ((వెడల్పు & 7) == 0) {
		కొత్త ((శూన్య*)&str_) std::string();
		ట్యాగ్ = backing_ట్యాగ్::string;
		std::string& str = get_str();
		పూర్ణాంకం బైట్లు = వెడల్పు >> 3;
		గుర్తుగల అక్షరం sign_byte = val < 0 ? -1 : 0;
		str.resize(బైట్లు, sign_byte);
		బైట్లు = std::min<పూర్ణాంకం>(బైట్లు, పరిమాణం(val));
		కొరకు (పూర్ణాంకం i = 0; i < బైట్లు; i++) {
			str[str.కొలత() - 1 - i] = val;
			val = val >> 8;
		}
		తిరిగి;
	}

	కొత్త ((శూన్య*)&బిట్లు_2) bitvectype();
	ట్యాగ్ = backing_ట్యాగ్::బిట్లు;
	bitvectype& bv = get_బిట్లు();
	bv.reserve(వెడల్పు);
	కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++) {
		bv.నెట్టు_వెనుక((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State బిట్టు, పూర్ణాంకం వెడల్పు)
{
	log_assert(వెడల్పు >= 0 && వెడల్పు < RTLIL::వెడల్పు_హద్దు);
	flags = RTLIL::CONST_FLAG_NONE;
	కొత్త ((శూన్య*)&బిట్లు_2) bitvectype();
	ట్యాగ్ = backing_ట్యాగ్::బిట్లు;
	bitvectype& bv = get_బిట్లు();
	bv.reserve(వెడల్పు);
	కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++)
		bv.నెట్టు_వెనుక(బిట్టు);
}

RTLIL::Const::Const(స్థిరరాశి std::సదిశ<తార్కిక> &బిట్లు)
{
	flags = RTLIL::CONST_FLAG_NONE;
	కొత్త ((శూన్య*)&బిట్లు_2) bitvectype();
	ట్యాగ్ = backing_ట్యాగ్::బిట్లు;
	bitvectype& bv = get_బిట్లు();
	bv.reserve(బిట్లు.కొలత());
	కొరకు (స్థిరరాశి స్వయంచాలక &b : బిట్లు)
		bv.emplace_వెనుక(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(స్థిరరాశి RTLIL::Const &other) {
	ట్యాగ్ = other.ట్యాగ్;
	flags = other.flags;
	ఒకవేళ (is_str())
		కొత్త ((శూన్య*)&str_) std::string(other.get_str());
	లేకపోతే ఒకవేళ (is_బిట్లు())
		కొత్త ((శూన్య*)&బిట్లు_2) bitvectype(other.get_బిట్లు());
	లేకపోతే
		check(అబద్ధం);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	ట్యాగ్ = other.ట్యాగ్;
	flags = other.flags;
	ఒకవేళ (is_str())
		కొత్త ((శూన్య*)&str_) std::string(std::move(other.get_str()));
	లేకపోతే ఒకవేళ (is_బిట్లు())
		కొత్త ((శూన్య*)&బిట్లు_2) bitvectype(std::move(other.get_బిట్లు()));
	లేకపోతే
		check(అబద్ధం);
}

RTLIL::Const &RTLIL::Const::క్రియాకారకం =(స్థిరరాశి RTLIL::Const &other) {
	flags = other.flags;
	ఒకవేళ (other.is_str()) {
		ఒకవేళ (!is_str()) {
			// sketchy zone
			check(is_బిట్లు());
			బిట్లు_2.~bitvectype();
			(శూన్య)కొత్త ((శూన్య*)&str_) std::string();
		}
		ట్యాగ్ = other.ట్యాగ్;
		get_str() = other.get_str();
	} లేకపోతే ఒకవేళ (other.is_బిట్లు()) {
		ఒకవేళ (!is_బిట్లు()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(శూన్య)కొత్త ((శూన్య*)&బిట్లు_2) bitvectype();
		}
		ట్యాగ్ = other.ట్యాగ్;
		get_బిట్లు() = other.get_బిట్లు();
	} లేకపోతే {
		check(అబద్ధం);
	}
	తిరిగి *ఇది;
}

RTLIL::Const::~Const() {
	ఒకవేళ (is_బిట్లు())
		బిట్లు_2.~bitvectype();
	లేకపోతే ఒకవేళ (is_str())
		str_.~string();
	లేకపోతే
		check(అబద్ధం);
}

తార్కిక RTLIL::Const::క్రియాకారకం<(స్థిరరాశి RTLIL::Const &other) స్థిరరాశి
{
	ఒకవేళ (కొలత() != other.కొలత())
		తిరిగి కొలత() < other.కొలత();

	కొరకు (పూర్ణాంకం i = 0; i < కొలత(); i++)
		ఒకవేళ ((*ఇది)[i] != other[i])
			తిరిగి (*ఇది)[i] < other[i];

	తిరిగి అబద్ధం;
}

తార్కిక RTLIL::Const::క్రియాకారకం ==(స్థిరరాశి RTLIL::Const &other) స్థిరరాశి
{
	ఒకవేళ (is_str() && other.is_str())
		తిరిగి get_str() == other.get_str();
	ఒకవేళ (is_బిట్లు() && other.is_బిట్లు())
		తిరిగి get_బిట్లు() == other.get_బిట్లు();

	ఒకవేళ (కొలత() != other.కొలత())
		తిరిగి అబద్ధం;

	కొరకు (పూర్ణాంకం i = 0; i < కొలత(); i++)
	ఒకవేళ ((*ఇది)[i] != other[i])
		తిరిగి అబద్ధం;

	తిరిగి నిజం;
}

తార్కిక RTLIL::Const::క్రియాకారకం !=(స్థిరరాశి RTLIL::Const &other) స్థిరరాశి
{
	తిరిగి !(*ఇది == other);
}

std::సదిశ<RTLIL::State>& RTLIL::Const::బిట్లు_అంతర్గత()
{
	bitvectorize_అంతర్గత();
	తిరిగి get_బిట్లు();
}

std::సదిశ<RTLIL::State> RTLIL::Const::గమ్యం_బిట్లు() స్థిరరాశి
{
	std::సదిశ<State> v;
	కొరకు (స్వయంచాలక బిట్టు : *ఇది)
		v.నెట్టు_వెనుక(బిట్టు);
	తిరిగి v;
}

తార్కిక RTLIL::Const::as_bool() స్థిరరాశి
{
	ఒకవేళ (is_str()) {
		కొరకు (అక్షరం ch : get_str())
			ఒకవేళ (ch != 0)
				తిరిగి నిజం;
		తిరిగి అబద్ధం;
	}

	స్థిరరాశి bitvectype& bv = get_బిట్లు();
	కొరకు (కొలత_t i = 0; i < bv.కొలత(); i++)
		ఒకవేళ (bv[i] == State::S1)
			తిరిగి నిజం;
	తిరిగి అబద్ధం;
}

పూర్ణాంకం RTLIL::Const::as_int(తార్కిక is_signed) స్థిరరాశి
{
	int32_t ret = 0;
	ఒకవేళ (స్థిరరాశి std::string *s = get_if_str()) {
		పూర్ణాంకం కొలత = Get_కొలత(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		కొరకు (పూర్ణాంకం i = std::min(4, కొలత); i > 0; i--)
			ret |= static_cast<గుర్తులేని అక్షరం>((*s)[కొలత - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		ఒకవేళ (is_signed && కొలత > 0 && కొలత < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << కొలత*8;
		తిరిగి ret;
	}

	స్థిరరాశి bitvectype& bv = get_బిట్లు();
	పూర్ణాంకం significant_బిట్లు = std::min(Get_కొలత(bv), 32);
	కొరకు (పూర్ణాంకం i = 0; i < significant_బిట్లు; i++)
		ఒకవేళ (bv[i] == State::S1)
			ret |= 1 << i;
	ఒకవేళ (is_signed && significant_బిట్లు > 0 && significant_బిట్లు < 32 && bv.వెనుక() == State::S1 )
		ret |= UINT32_MAX << significant_బిట్లు;
	తిరిగి ret;
}

తార్కిక RTLIL::Const::convertible_గమ్యం_int(తార్కిక is_signed) స్థిరరాశి
{
	స్వయంచాలక కొలత = get_min_కొలత(is_signed);

	ఒకవేళ (కొలత < 0)
		తిరిగి అబద్ధం;

	// If it fits in 31 bits it is definitely convertible
	ఒకవేళ (కొలత <= 31)
		తిరిగి నిజం;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	ఒకవేళ (కొలత == 32) {
		ఒకవేళ (is_signed)
			తిరిగి నిజం;
		తిరిగి (*ఇది)[కొలత - 1] != State::S1;
	}

	తిరిగి అబద్ధం;
}

std::optional<పూర్ణాంకం> RTLIL::Const::try_as_int(తార్కిక is_signed) స్థిరరాశి
{
	ఒకవేళ (!convertible_గమ్యం_int(is_signed))
		తిరిగి std::nullopt;
	తిరిగి as_int(is_signed);
}

పూర్ణాంకం RTLIL::Const::as_int_saturating(తార్కిక is_signed) స్థిరరాశి
{
	ఒకవేళ (!convertible_గమ్యం_int(is_signed)) {
		ఒకవేళ (!is_signed)
			తిరిగి std::numeric_limits<పూర్ణాంకం>::max();

		స్థిరరాశి స్వయంచాలక min_కొలత = get_min_కొలత(is_signed);
		log_assert(min_కొలత > 0);
		స్థిరరాశి స్వయంచాలక neg = (*ఇది)[min_కొలత - 1];
		తిరిగి neg ? std::numeric_limits<పూర్ణాంకం>::min() : std::numeric_limits<పూర్ణాంకం>::max();
	}
	తిరిగి as_int(is_signed);
}

శూన్య RTLIL::Const::ట్యాగ్_bare_integer_const(స్థిరరాశి std::string &విలువ)
{
	ఒకవేళ (విలువ.ఖాళీలు() || విలువ.కనుగొను('\'') != std::string::npos)
		తిరిగి;
	కొలత_t ప్రారంభం = (విలువ[0] == '-' || విలువ[0] == '+') ? 1 : 0;
	ఒకవేళ (ప్రారంభం < విలువ.కొలత() && std::all_యొక్క(విలువ.begin() + ప్రారంభం, విలువ.చివర(), ::అంకెనా))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

పూర్ణాంకం RTLIL::Const::get_min_కొలత(తార్కిక is_signed) స్థిరరాశి
{
	ఒకవేళ (ఖాళీలు()) తిరిగి 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_బిట్టు;
	ఒకవేళ (is_signed)
		leading_బిట్టు = (వెనుక() == RTLIL::State::Sx) ? RTLIL::State::S0 : వెనుక();
	లేకపోతే
		leading_బిట్టు = RTLIL::State::S0;

	స్వయంచాలక idx = కొలత();
	అయితే (idx > 0 && (*ఇది)[idx -1] == leading_బిట్టు) {
		idx--;
	}

	// signed needs one leading bit
	ఒకవేళ (is_signed && idx < కొలత()) {
		idx++;
	}
	// must be at least one bit
	తిరిగి (idx == 0) ? 1 : idx;
}

శూన్య RTLIL::Const::compress(తార్కిక is_signed)
{
	స్వయంచాలక idx = get_min_కొలత(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<పూర్ణాంకం> RTLIL::Const::as_int_compress(తార్కిక is_signed) స్థిరరాశి
{
	తిరిగి try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(స్థిరరాశి అక్షరం* any) స్థిరరాశి
{
	పూర్ణాంకం sz = కొలత();
	std::string ret;
	ret.reserve(sz);
	కొరకు (పూర్ణాంకం i = sz - 1; i >= 0; --i)
		ఎంపిక ((*ఇది)[i]) {
			స్థితి S0: ret.నెట్టు_వెనుక('0'); ఆపు;
			స్థితి S1: ret.నెట్టు_వెనుక('1'); ఆపు;
			స్థితి Sx: ret.నెట్టు_వెనుక('x'); ఆపు;
			స్థితి Sz: ret.నెట్టు_వెనుక('z'); ఆపు;
			స్థితి Sa: ret += any; ఆపు;
			స్థితి Sm: ret.నెట్టు_వెనుక('m'); ఆపు;
		}
	తిరిగి ret;
}

RTLIL::Const RTLIL::Const::from_string(స్థిరరాశి std::string &str)
{
	Const c;
	bitvectype& bv = c.get_బిట్లు();
	bv.reserve(str.కొలత());
	కొరకు (స్వయంచాలక it = str.rbegin(); it != str.rend(); it++)
		ఎంపిక (*it) {
			స్థితి '0': bv.నెట్టు_వెనుక(State::S0); ఆపు;
			స్థితి '1': bv.నెట్టు_వెనుక(State::S1); ఆపు;
			స్థితి 'x': bv.నెట్టు_వెనుక(State::Sx); ఆపు;
			స్థితి 'z': bv.నెట్టు_వెనుక(State::Sz); ఆపు;
			స్థితి 'm': bv.నెట్టు_వెనుక(State::Sm); ఆపు;
			అప్రమేయ: bv.నెట్టు_వెనుక(State::Sa);
		}
	తిరిగి c;
}

std::string RTLIL::Const::decode_string() స్థిరరాశి
{
	ఒకవేళ (స్వయంచాలక str = get_if_str())
		తిరిగి *str;

	స్థిరరాశి bitvectype& bv = get_బిట్లు();
	స్థిరరాశి పూర్ణాంకం n = Get_కొలత(bv);
	స్థిరరాశి పూర్ణాంకం n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	పూర్ణాంకం i = n_over_8 * 8;
	ఒకవేళ (i < n) {
		అక్షరం ch = 0;
		కొరకు (పూర్ణాంకం j = 0; j < (n - i); j++) {
			ఒకవేళ (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		ఒకవేళ (ch != 0)
			s.append({ch});
	}
	i -= 8;
	కొరకు (; i >= 0; i -= 8) {
		అక్షరం ch = 0;
		కొరకు (పూర్ణాంకం j = 0; j < 8; j++) {
			ఒకవేళ (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		ఒకవేళ (ch != 0)
			s.append({ch});
	}
	తిరిగి s;
}

పూర్ణాంకం RTLIL::Const::కొలత() స్థిరరాశి {
	ఒకవేళ (is_str())
		తిరిగి 8 * str_.కొలత();
	లేకపోతే {
		check(is_బిట్లు());
		తిరిగి బిట్లు_2.కొలత();
	}
}

తార్కిక RTLIL::Const::ఖాళీలు() స్థిరరాశి {
	ఒకవేళ (is_str())
		తిరిగి str_.ఖాళీలు();
	లేకపోతే {
		check(is_బిట్లు());
		తిరిగి బిట్లు_2.ఖాళీలు();
	}
}

శూన్య RTLIL::Const::bitvectorize_అంతర్గత() {
	ఒకవేళ (ట్యాగ్ == backing_ట్యాగ్::బిట్లు)
		తిరిగి;

	check(is_str());

	bitvectype new_బిట్లు;

	new_బిట్లు.reserve(str_.కొలత() * 8);
	కొరకు (పూర్ణాంకం i = str_.కొలత() - 1; i >= 0; i--) {
		గుర్తులేని అక్షరం ch = str_[i];
		కొరకు (పూర్ణాంకం j = 0; j < 8; j++) {
			new_బిట్లు.నెట్టు_వెనుక((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(శూన్య)కొత్త ((శూన్య*)&బిట్లు_2) bitvectype(std::move(new_బిట్లు));
		ట్యాగ్ = backing_ట్యాగ్::బిట్లు;
	}
}

శూన్య RTLIL::Const::append(స్థిరరాశి RTLIL::Const &other) {
	bitvectorize_అంతర్గత();
	bitvectype& bv = get_బిట్లు();
	bv.చొప్పించు(bv.చివర(), other.begin(), other.చివర());
}

RTLIL::State RTLIL::Const::const_iterator::క్రియాకారకం*() స్థిరరాశి {
	ఒకవేళ (స్వయంచాలక bv = మాతృ->get_if_బిట్లు())
		తిరిగి (*bv)[idx];

	పూర్ణాంకం char_idx = మాతృ->get_str().కొలత() - idx / 8 - 1;
	తార్కిక బిట్టు = (మాతృ->get_str()[char_idx] & (1 << (idx % 8)));
	తిరిగి బిట్టు ? State::S1 : State::S0;
}

తార్కిక RTLIL::Const::is_fully_zero() స్థిరరాశి
{
	ఒకవేళ (స్వయంచాలక str = get_if_str()) {
		కొరకు (అక్షరం ch : *str)
			ఒకవేళ (ch != 0)
				తిరిగి అబద్ధం;
		తిరిగి నిజం;
	}

	స్థిరరాశి bitvectype& bv = get_బిట్లు();

	కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : bv)
		ఒకవేళ (బిట్టు != RTLIL::State::S0)
			తిరిగి అబద్ధం;

	తిరిగి నిజం;
}

తార్కిక RTLIL::Const::is_fully_ones() స్థిరరాశి
{
	ఒకవేళ (స్వయంచాలక str = get_if_str()) {
		కొరకు (అక్షరం ch : *str)
			ఒకవేళ (ch != (అక్షరం)0xff)
				తిరిగి అబద్ధం;
		తిరిగి నిజం;
	}

	స్థిరరాశి bitvectype& bv = get_బిట్లు();
	కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : bv)
		ఒకవేళ (బిట్టు != RTLIL::State::S1)
			తిరిగి అబద్ధం;

	తిరిగి నిజం;
}

తార్కిక RTLIL::Const::is_fully_def() స్థిరరాశి
{
	ఒకవేళ (is_str())
		తిరిగి నిజం;

	స్థిరరాశి bitvectype& bv = get_బిట్లు();
	కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : bv)
		ఒకవేళ (బిట్టు != RTLIL::State::S0 && బిట్టు != RTLIL::State::S1)
			తిరిగి అబద్ధం;

	తిరిగి నిజం;
}

తార్కిక RTLIL::Const::is_fully_undef() స్థిరరాశి
{
	ఒకవేళ (స్వయంచాలక str = get_if_str())
		తిరిగి str->ఖాళీలు();

	స్థిరరాశి bitvectype& bv = get_బిట్లు();
	కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : bv)
		ఒకవేళ (బిట్టు != RTLIL::State::Sx && బిట్టు != RTLIL::State::Sz)
			తిరిగి అబద్ధం;

	తిరిగి నిజం;
}

తార్కిక RTLIL::Const::is_fully_undef_x_only() స్థిరరాశి
{
	ఒకవేళ (స్వయంచాలక str = get_if_str())
		తిరిగి str->ఖాళీలు();

	స్థిరరాశి bitvectype& bv = get_బిట్లు();
	కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : bv)
		ఒకవేళ (బిట్టు != RTLIL::State::Sx)
			తిరిగి అబద్ధం;

	తిరిగి నిజం;
}

తార్కిక RTLIL::Const::is_onehot(పూర్ణాంకం *pos) స్థిరరాశి
{
	తార్కిక దొరికింది = అబద్ధం;
	పూర్ణాంకం కొలత = Get_కొలత(*ఇది);
	కొరకు (పూర్ణాంకం i = 0; i < కొలత; i++) {
		State బిట్టు = (*ఇది)[i];
		ఒకవేళ (బిట్టు != RTLIL::State::S0 && బిట్టు != RTLIL::State::S1)
			తిరిగి అబద్ధం;
		ఒకవేళ (బిట్టు == RTLIL::State::S1) {
			ఒకవేళ (దొరికింది)
				తిరిగి అబద్ధం;
			ఒకవేళ (pos)
				*pos = i;
			దొరికింది = నిజం;
		}
	}
	తిరిగి దొరికింది;
}

Hasher RTLIL::Const::hash_into(Hasher h) స్థిరరాశి
{
	ఒకవేళ (స్వయంచాలక str = get_if_str())
		తిరిగి hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	స్థిరరాశి bitvectype &bv = get_బిట్లు();
	పూర్ణాంకం కొలత = Get_కొలత(bv);
	std::string packed;
	పూర్ణాంకం packed_కొలత = (కొలత + 7) >> 3;
	packed.resize(packed_కొలత, 0);
	కొరకు (పూర్ణాంకం bi = 0; bi < packed_కొలత; ++bi) {
		అక్షరం ch = 0;
		పూర్ణాంకం చివర = std::min((bi + 1)*8, కొలత);
		కొరకు (పూర్ణాంకం i = bi*8; i < చివర; ++i) {
			RTLIL::State b = bv[i];
			ఒకవేళ (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<అక్షరం>::hash_into(ch, h);
				కొరకు (; i < కొలత; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(కొలత);
				తిరిగి h;
			}
			ch |= static_cast<పూర్ణాంకం>(b) << (i & 7);
		}
		packed[packed_కొలత - 1 - bi] = ch;
	}
	తిరిగి hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::వెలికితీయి(పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం len, RTLIL::State padding) స్థిరరాశి {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	కొరకు (పూర్ణాంకం i = స్థానభ్రంశం; i < స్థానభ్రంశం + len; i++)
		ret_bv.నెట్టు_వెనుక(i < Get_కొలత(*ఇది) ? (*ఇది)[i] : padding);
	తిరిగి RTLIL::Const(ret_bv);
}
#నిర్వచనం_రద్దు check /* check(condition) for Const */

తార్కిక RTLIL::AttrObject::has_attribute(RTLIL::IdString id) స్థిరరాశి
{
	తిరిగి లక్షణాలు.లెక్క(id);
}

శూన్య RTLIL::AttrObject::సమితి_bool_attribute(RTLIL::IdString id, తార్కిక విలువ)
{
	ఒకవేళ (విలువ)
		లక్షణాలు[id] = RTLIL::Const(1);
	లేకపోతే
		లక్షణాలు.erase(id);
}

తార్కిక RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) స్థిరరాశి
{
	స్థిరరాశి స్వయంచాలక it = లక్షణాలు.కనుగొను(id);
	ఒకవేళ (it == లక్షణాలు.చివర())
		తిరిగి అబద్ధం;
	తిరిగి it->రెండవ.as_bool();
}

శూన్య RTLIL::AttrObject::సమితి_string_attribute(RTLIL::IdString id, string విలువ)
{
	ఒకవేళ (విలువ.ఖాళీలు())
		లక్షణాలు.erase(id);
	లేకపోతే
		లక్షణాలు[id] = విలువ;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) స్థిరరాశి
{
	std::string విలువ;
	స్థిరరాశి స్వయంచాలక it = లక్షణాలు.కనుగొను(id);
	ఒకవేళ (it != లక్షణాలు.చివర())
		విలువ = it->రెండవ.decode_string();
	తిరిగి విలువ;
}

శూన్య RTLIL::AttrObject::సమితి_strpool_attribute(RTLIL::IdString id, స్థిరరాశి pool<string> &దత్తాంశం)
{
	string attrval;
	కొరకు (స్థిరరాశి స్వయంచాలక &s : దత్తాంశం) {
		ఒకవేళ (!attrval.ఖాళీలు())
			attrval += "|";
		attrval += s;
	}
	సమితి_string_attribute(id, attrval);
}

శూన్య RTLIL::AttrObject::కూడు_strpool_attribute(RTLIL::IdString id, స్థిరరాశి pool<string> &దత్తాంశం)
{
	pool<string> union_దత్తాంశం = get_strpool_attribute(id);
	union_దత్తాంశం.చొప్పించు(దత్తాంశం.begin(), దత్తాంశం.చివర());
	ఒకవేళ (!union_దత్తాంశం.ఖాళీలు())
		సమితి_strpool_attribute(id, union_దత్తాంశం);
}

శూన్య RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->లక్షణాలు = std::move(buf);
}

నామాకాశ {
	మూస<రకం_పేరు F>
	శూన్య walk_process_attr_objects(RTLIL::Process *process, F సందర్శించు) {
		సందర్శించు(process);
		std::సదిశ<RTLIL::Case_నియమం*> case_దొంతర{&process->మూలం_case};
		అయితే (!case_దొంతర.ఖాళీలు()) {
			RTLIL::Case_నియమం *cs = case_దొంతర.వెనుక();
			case_దొంతర.తీయి_వెనుక();
			సందర్శించు(cs);
			కొరకు (స్వయంచాలక *sw : cs->switches) {
				సందర్శించు(sw);
				కొరకు (స్వయంచాలక *case_ : sw->cases)
					case_దొంతర.నెట్టు_వెనుక(case_);
			}
		}
		కొరకు (స్వయంచాలక *సమకాలీకరణ : process->syncs)
			కొరకు (స్వయంచాలక &mwa : సమకాలీకరణ->mem_write_actions)
				సందర్శించు(&mwa);
	}

	మూస<రకం_పేరు F>
	శూన్య walk_attr_objects(RTLIL::Design *design, F సందర్శించు) {
		కొరకు (స్వయంచాలక &[_, module] : design->modules_) {
			సందర్శించు(module);
			కొరకు (స్వయంచాలక &[_, wire] : module->wires_)
				సందర్శించు(wire);
			కొరకు (స్వయంచాలక &[_, mem] : module->memories)
				సందర్శించు(mem);
			కొరకు (స్వయంచాలక &[_, cell] : module->cells_)
				సందర్శించు(cell);
			కొరకు (స్వయంచాలక &[_, process] : module->processes)
				walk_process_attr_objects(process, సందర్శించు);
		}
	}
}


కొలత_t RTLIL::Design::gc_twines()
{
	int64_t ప్రారంభం = PerformanceTimer::query();
	pool<IdString> సజీవ;
	స్వయంచాలక మూలం = [&](IdString ref) {
		ఒకవేళ (ref != IdString::Null)
			సజీవ.చొప్పించు(ref);
	};

	walk_attr_objects(ఇది, [&](స్థిరరాశి RTLIL::AttrObject *obj) {
		కొరకు (స్వయంచాలక &attr : obj->లక్షణాలు)
			మూలం(attr.మొదటి);
	});

	మూలం(selected_చురుకైన_module);

	కొరకు (స్వయంచాలక &[name, module] : modules_) {
		మూలం(name);
		కొరకు (IdString పోర్టు : module->ports)
			మూలం(పోర్టు);
		కొరకు (IdString param : module->avail_parameters)
			మూలం(param);
		కొరకు (స్వయంచాలక &[param, _] : module->parameter_default_విలువలు)
			మూలం(param);
		కొరకు (స్వయంచాలక &[name, _] : module->memories)
			మూలం(name);
		కొరకు (స్వయంచాలక &[name, _] : module->processes)
			మూలం(name);
		కొరకు (స్వయంచాలక &[name, wire] : module->wires_) {
			మూలం(name);
			ఒకవేళ (wire->తెలిసిన_driver())
				మూలం(wire->driver_పోర్టు());
		}
		కొరకు (స్వయంచాలక &[name, cell] : module->cells_) {
			మూలం(name);
			మూలం(cell->type);
			కొరకు (స్వయంచాలక &conn : cell->connections())
				మూలం(conn.మొదటి);
			కొరకు (స్వయంచాలక &param : cell->parameters)
				మూలం(param.మొదటి);
		}
		కొరకు (స్వయంచాలక &[_, process] : module->processes) {
			std::సదిశ<RTLIL::Case_నియమం*> case_దొంతర{&process->మూలం_case};
			అయితే (!case_దొంతర.ఖాళీలు()) {
				RTLIL::Case_నియమం *cs = case_దొంతర.వెనుక();
				case_దొంతర.తీయి_వెనుక();
				కొరకు (స్వయంచాలక *sw : cs->switches) {
					కొరకు (స్వయంచాలక *case_ : sw->cases)
						case_దొంతర.నెట్టు_వెనుక(case_);
				}
			}
			కొరకు (స్వయంచాలక *సమకాలీకరణ : process->syncs) {
				కొరకు (స్వయంచాలక &mwa : సమకాలీకరణ->mem_write_actions)
					మూలం(mwa.memid);
			}
		}
	}

	స్వయంచాలక మూలం_selection = [&](స్థిరరాశి RTLIL::Selection &sel) {
		కొరకు (IdString m : sel.selected_modules)
			మూలం(m);
		కొరకు (స్వయంచాలక &[m, members] : sel.selected_members) {
			మూలం(m);
			కొరకు (IdString member : members)
				మూలం(member);
		}
	};

	కొరకు (స్వయంచాలక &[name, sel] : selection_vars) {
		మూలం(name);
		మూలం_selection(sel);
	}

	కొరకు (స్వయంచాలక &sel : selection_దొంతర)
		మూలం_selection(sel);

	కొలత_t erased = twines_.gc(సజీవ);

	int64_t సమయం_ns = PerformanceTimer::query() - ప్రారంభం;
	Pass::వ్యవకలనం_from_current_runtime_ns(సమయం_ns);
	twine_gc_ns += సమయం_ns;
	++twine_gc_లెక్క;
	తిరిగి erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) స్థిరరాశి
{
	pool<string> దత్తాంశం;
	ఒకవేళ (లక్షణాలు.లెక్క(id) != 0)
		కొరకు (స్వయంచాలక s : విభజించు_టోకెన్లు(get_string_attribute(id), "|"))
			దత్తాంశం.చొప్పించు(s);
	తిరిగి దత్తాంశం;
}

శూన్య RTLIL::AttrObject::సమితి_hdlname_attribute(స్థిరరాశి సదిశ<string> &hierarchy)
{
	string attrval;
	కొరకు (స్థిరరాశి స్వయంచాలక &ident : hierarchy) {
		ఒకవేళ (!attrval.ఖాళీలు())
			attrval += " ";
		attrval += ident;
	}
	సమితి_string_attribute(ID::hdlname, attrval);
}

సదిశ<string> RTLIL::AttrObject::get_hdlname_attribute() స్థిరరాశి
{
	తిరిగి విభజించు_టోకెన్లు(get_string_attribute(ID::hdlname), " ");
}

శూన్య RTLIL::AttrObject::సమితి_intvec_attribute(RTLIL::IdString id, స్థిరరాశి సదిశ<పూర్ణాంకం> &దత్తాంశం)
{
	std::stringstream attrval;
	కొరకు (స్వయంచాలక &i : దత్తాంశం) {
		ఒకవేళ (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	లక్షణాలు[id] = RTLIL::Const(attrval.str());
}

సదిశ<పూర్ణాంకం> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) స్థిరరాశి
{
	సదిశ<పూర్ణాంకం> దత్తాంశం;
	స్వయంచాలక it = లక్షణాలు.కనుగొను(id);
	ఒకవేళ (it != లక్షణాలు.చివర())
		కొరకు (స్థిరరాశి స్వయంచాలక &s : విభజించు_టోకెన్లు(లక్షణాలు.at(id).decode_string())) {
			అక్షరం *చివర = శూన్యసూచిక;
			errno = 0;
			దీర్ఘ విలువ = strtol(s.c_str(), &చివర, 10);
			ఒకవేళ (చివర != s.c_str() + s.కొలత())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			ఒకవేళ (errno == ERANGE || విలువ < INT_MIN || విలువ > గరిష్ఠ_పూర్ణాంకం)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			దత్తాంశం.నెట్టు_వెనుక(విలువ);
		}
	తిరిగి దత్తాంశం;
}

తార్కిక RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) స్థిరరాశి
{
	ఒకవేళ (current_design != శూన్యసూచిక) {
		స్వయంచాలక module = current_design->module(mod_name);
		తిరిగి module && module->get_blackbox_attribute();
	} లేకపోతే {
		log_warning("Unable to check if module is boxed for null design.\n");
		తిరిగి అబద్ధం;
	}
}

తార్కిక RTLIL::Selection::selected_module(RTLIL::IdString mod_name) స్థిరరాశి
{
	ఒకవేళ (complete_selection)
		తిరిగి నిజం;
	ఒకవేళ (!selects_boxes && boxed_module(mod_name))
		తిరిగి అబద్ధం;
	ఒకవేళ (నిండిన_selection)
		తిరిగి నిజం;
	ఒకవేళ (selected_modules.లెక్క(mod_name) > 0)
		తిరిగి నిజం;
	ఒకవేళ (selected_members.లెక్క(mod_name) > 0)
		తిరిగి నిజం;
	తిరిగి అబద్ధం;
}

తార్కిక RTLIL::Selection::selected_సంపూర్ణ_module(RTLIL::IdString mod_name) స్థిరరాశి
{
	ఒకవేళ (complete_selection)
		తిరిగి నిజం;
	ఒకవేళ (!selects_boxes && boxed_module(mod_name))
		తిరిగి అబద్ధం;
	ఒకవేళ (నిండిన_selection)
		తిరిగి నిజం;
	ఒకవేళ (selected_modules.లెక్క(mod_name) > 0)
		తిరిగి నిజం;
	తిరిగి అబద్ధం;
}

తార్కిక RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) స్థిరరాశి
{
	ఒకవేళ (complete_selection)
		తిరిగి నిజం;
	ఒకవేళ (!selects_boxes && boxed_module(mod_name))
		తిరిగి అబద్ధం;
	ఒకవేళ (నిండిన_selection)
		తిరిగి నిజం;
	ఒకవేళ (selected_modules.లెక్క(mod_name) > 0)
		తిరిగి నిజం;
	ఒకవేళ (selected_members.లెక్క(mod_name) > 0)
		ఒకవేళ (selected_members.at(mod_name).లెక్క(memb_name) > 0)
			తిరిగి నిజం;
	తిరిగి అబద్ధం;
}

శూన్య RTLIL::Selection::optimize(RTLIL::Design *design)
{
	ఒకవేళ (design != current_design) {
		current_design = design;
	}

	ఒకవేళ (selects_boxes && నిండిన_selection)
		complete_selection = నిజం;
	ఒకవేళ (complete_selection) {
		నిండిన_selection = అబద్ధం;
		selects_boxes = నిజం;
	}
	ఒకవేళ (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		తిరిగి;
	}

	std::సదిశ<RTLIL::IdString> del_list, కూడు_list;

	del_list.clear();
	కొరకు (స్వయంచాలక mod_name : selected_modules) {
		ఒకవేళ (current_design->module(mod_name) == శూన్యసూచిక || (!selects_boxes && boxed_module(mod_name)))
			del_list.నెట్టు_వెనుక(mod_name);
		selected_members.erase(mod_name);
	}
	కొరకు (స్వయంచాలక mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	కొరకు (స్వయంచాలక &it : selected_members)
		ఒకవేళ (current_design->module(it.మొదటి) == శూన్యసూచిక || (!selects_boxes && boxed_module(it.మొదటి)))
			del_list.నెట్టు_వెనుక(it.మొదటి);
	కొరకు (స్వయంచాలక mod_name : del_list)
		selected_members.erase(mod_name);

	కొరకు (స్వయంచాలక &it : selected_members) {
		del_list.clear();
		కొరకు (స్వయంచాలక memb_name : it.రెండవ)
			ఒకవేళ (current_design->module(it.మొదటి)->లెక్క_id(memb_name) == 0)
				del_list.నెట్టు_వెనుక(memb_name);
		కొరకు (స్వయంచాలక memb_name : del_list)
			it.రెండవ.erase(memb_name);
	}

	del_list.clear();
	కూడు_list.clear();
	కొరకు (స్వయంచాలక &it : selected_members)
		ఒకవేళ (it.రెండవ.కొలత() == 0)
			del_list.నెట్టు_వెనుక(it.మొదటి);
		లేకపోతే ఒకవేళ (it.రెండవ.కొలత() == current_design->module(it.మొదటి)->wires_.కొలత() + current_design->module(it.మొదటి)->memories.కొలత() +
				current_design->module(it.మొదటి)->cells_.కొలత() + current_design->module(it.మొదటి)->processes.కొలత())
			కూడు_list.నెట్టు_వెనుక(it.మొదటి);
	కొరకు (స్వయంచాలక mod_name : del_list)
		selected_members.erase(mod_name);
	కొరకు (స్వయంచాలక mod_name : కూడు_list) {
		selected_members.erase(mod_name);
		selected_modules.చొప్పించు(mod_name);
	}

	ఒకవేళ (selected_modules.కొలత() == current_design->modules_.కొలత()) {
		selected_modules.clear();
		selected_members.clear();
		ఒకవేళ (selects_boxes)
			complete_selection = నిజం;
		లేకపోతే
			నిండిన_selection = నిజం;
	}
}

శూన్య RTLIL::Selection::clear()
{
	నిండిన_selection = అబద్ధం;
	complete_selection = అబద్ధం;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_నిర్వచిస్తుంది (కొత్త define_map_t)
{
	స్థిర గుర్తులేని పూర్ణాంకం hashidx_లెక్క = 123456789;
	hashidx_లెక్క = mkhash_xorshift(hashidx_లెక్క);
	hashidx_ = hashidx_లెక్క;

	refcount_modules_ = 0;
	selected_చురుకైన_module.clear();
	నెట్టు_నిండిన_selection();

	RTLIL::Design::get_all_designs()->చొప్పించు(std::pair<గుర్తులేని పూర్ణాంకం, RTLIL::Design*>(hashidx_, ఇది));
}

RTLIL::Design::~Design()
{
	కొరకు (స్వయంచాలక &pr : modules_)
		తొలగించు pr.రెండవ;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

స్థిర std::map<గుర్తులేని పూర్ణాంకం, RTLIL::Design*> all_designs;
std::map<గుర్తులేని పూర్ణాంకం, RTLIL::Design*> *RTLIL::Design::get_all_designs(శూన్య)
{
	తిరిగి &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	తిరిగి RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

స్థిరరాశి RTLIL::Module *RTLIL::Design::module(IdString id) స్థిరరాశి {
	twines_.check_owned(id);
	తిరిగి modules_.లెక్క(id) ? modules_.at(id) : ఖాళీ;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	తిరిగి modules_.లెక్క(id) ? modules_.at(id) : ఖాళీ;
}

RTLIL::Module *RTLIL::Design::శిఖరం_module() స్థిరరాశి
{
	RTLIL::Module *module = శూన్యసూచిక;
	పూర్ణాంకం module_లెక్క = 0;

	కొరకు (స్వయంచాలక mod : selected_modules()) {
		ఒకవేళ (mod->get_bool_attribute(ID::శిఖరం))
			తిరిగి mod;
		module_లెక్క++;
		module = mod;
	}

	తిరిగి module_లెక్క == 1 ? module : శూన్యసూచిక;
}

RTLIL::Module *RTLIL::Design::కూడు_Module(RTLIL::IdString name)
{
	ఒకవేళ (modules_.లెక్క(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = కొత్త RTLIL::Module;
	modules_[name] = module;
	module->design = ఇది;
	module->name = name;

	కొరకు (స్వయంచాలక mon : monitors)
		mon->notify_module_కూడు(module);

	ఒకవేళ (yosys_xtrace) {
		సంవర్గమానం("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	తిరిగి module;
}

శూన్య RTLIL::Design::scratchpad_unset(స్థిరరాశి std::string &varname)
{
	scratchpad.erase(varname);
}

శూన్య RTLIL::Design::scratchpad_సమితి_int(స్థిరరాశి std::string &varname, పూర్ణాంకం విలువ)
{
	scratchpad[varname] = stringf("%d", విలువ);
}

శూన్య RTLIL::Design::scratchpad_సమితి_bool(స్థిరరాశి std::string &varname, తార్కిక విలువ)
{
	scratchpad[varname] = విలువ ? "true" : "false";
}

శూన్య RTLIL::Design::scratchpad_సమితి_string(స్థిరరాశి std::string &varname, std::string విలువ)
{
	scratchpad[varname] = std::move(విలువ);
}

పూర్ణాంకం RTLIL::Design::scratchpad_get_int(స్థిరరాశి std::string &varname, పూర్ణాంకం default_విలువ_2) స్థిరరాశి
{
	స్వయంచాలక it = scratchpad.కనుగొను(varname);
	ఒకవేళ (it == scratchpad.చివర())
		తిరిగి default_విలువ_2;

	స్థిరరాశి std::string &str = it->రెండవ;

	ఒకవేళ (str == "0" || str == "false")
		తిరిగి 0;

	ఒకవేళ (str == "1" || str == "true")
		తిరిగి 1;

	అక్షరం *endptr = శూన్యసూచిక;
	దీర్ఘ పూర్ణాంకం parsed_విలువ = strtol(str.c_str(), &endptr, 10);
	తిరిగి *endptr ? default_విలువ_2 : parsed_విలువ;
}

తార్కిక RTLIL::Design::scratchpad_get_bool(స్థిరరాశి std::string &varname, తార్కిక default_విలువ_2) స్థిరరాశి
{
	స్వయంచాలక it = scratchpad.కనుగొను(varname);
	ఒకవేళ (it == scratchpad.చివర())
		తిరిగి default_విలువ_2;

	స్థిరరాశి std::string &str = it->రెండవ;

	ఒకవేళ (str == "0" || str == "false")
		తిరిగి అబద్ధం;

	ఒకవేళ (str == "1" || str == "true")
		తిరిగి నిజం;

	తిరిగి default_విలువ_2;
}

std::string RTLIL::Design::scratchpad_get_string(స్థిరరాశి std::string &varname, స్థిరరాశి std::string &default_విలువ_2) స్థిరరాశి
{
	స్వయంచాలక it = scratchpad.కనుగొను(varname);
	ఒకవేళ (it == scratchpad.చివర())
		తిరిగి default_విలువ_2;

	తిరిగి it->రెండవ;
}

శూన్య RTLIL::Design::remove(RTLIL::Module *module)
{
	కొరకు (స్వయంచాలక mon : monitors)
		mon->notify_module_del(module);

	ఒకవేళ (yosys_xtrace) {
		సంవర్గమానం("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	తొలగించు module;
}

శూన్య RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	కూడు(module);
}

శూన్య RTLIL::Design::క్రమీకరించు()
{
	scratchpad.క్రమీకరించు();
	modules_.క్రమీకరించు(క్రమీకరించు_by_id_str(twines_));
	కొరకు (స్వయంచాలక &it : modules_)
		it.రెండవ->క్రమీకరించు();
}

శూన్య RTLIL::Design::క్రమీకరించు_modules()
{
	scratchpad.క్రమీకరించు();
	modules_.క్రమీకరించు(క్రమీకరించు_by_id_str(twines_));
}

శూన్య check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

శూన్య RTLIL::Design::check()
{
#నిర్వచితం_కాకపోతే NDEBUG
	log_assert(!selection_దొంతర.ఖాళీలు());
	పూర్ణాంకం pool_కొలత = 0;
	కొరకు (స్వయంచాలక &it : modules_)
		pool_కొలత = std::max(pool_కొలత, ThreadPool::work_pool_కొలత(0, it.రెండవ->cells_కొలత(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_కొలత);
	కొరకు (స్వయంచాలక &it : modules_) {
		log_assert(ఇది == it.రెండవ->design);
		log_assert(it.మొదటి == it.రెండవ->name);
		check_module(it.రెండవ, thread_pool);
	}
#అయితే_ముగింపు
}

శూన్య RTLIL::Design::optimize()
{
	కొరకు (స్వయంచాలక &it : modules_)
		it.రెండవ->optimize();
	కొరకు (స్వయంచాలక &it : selection_దొంతర)
		it.optimize(ఇది);
	కొరకు (స్వయంచాలక &it : selection_vars)
		it.రెండవ.optimize(ఇది);
}

శూన్య RTLIL::Design::clone_into(RTLIL::Design *dst) స్థిరరాశి
{
	log_assert(dst->modules_.ఖాళీలు());
	dst->twines_ = twines_;
	కొరకు (స్వయంచాలక it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->రెండవ->clone(dst);
}

తార్కిక RTLIL::Design::selected_module(IdString mod_name) స్థిరరాశి
{
	ఒకవేళ (!selected_చురుకైన_module.ఖాళీలు() && mod_name != selected_చురుకైన_module)
		తిరిగి అబద్ధం;
	తిరిగి selection().selected_module(mod_name);
}

తార్కిక RTLIL::Design::selected_సంపూర్ణ_module(RTLIL::IdString mod_name) స్థిరరాశి
{
	ఒకవేళ (!selected_చురుకైన_module.ఖాళీలు() && mod_name != selected_చురుకైన_module)
		తిరిగి అబద్ధం;
	తిరిగి selection().selected_సంపూర్ణ_module(mod_name);
}

తార్కిక RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) స్థిరరాశి
{
	ఒకవేళ (!selected_చురుకైన_module.ఖాళీలు() && mod_name != selected_చురుకైన_module)
		తిరిగి అబద్ధం;
	తిరిగి selection().selected_member(mod_name, memb_name);
}

తార్కిక RTLIL::Design::selected_module(RTLIL::Module *mod) స్థిరరాశి
{
	తిరిగి selected_module(mod->name);
}

తార్కిక RTLIL::Design::selected_సంపూర్ణ_module(RTLIL::Module *mod) స్థిరరాశి
{
	తిరిగి selected_సంపూర్ణ_module(mod->name);
}

శూన్య RTLIL::Design::నెట్టు_selection(RTLIL::Selection sel)
{
	sel.current_design = ఇది;
	selection_దొంతర.నెట్టు_వెనుక(sel);
}

శూన్య RTLIL::Design::నెట్టు_ఖాళీలు_selection()
{
	నెట్టు_selection(RTLIL::Selection::ఖాళీలు_Selection(ఇది));
}

శూన్య RTLIL::Design::నెట్టు_నిండిన_selection()
{
	నెట్టు_selection(RTLIL::Selection::నిండిన_Selection(ఇది));
}

శూన్య RTLIL::Design::నెట్టు_complete_selection()
{
	నెట్టు_selection(RTLIL::Selection::CompleteSelection(ఇది));
}

శూన్య RTLIL::Design::తీయి_selection()
{
	selection_దొంతర.తీయి_వెనుక();
	// Default to a full_selection if we ran out of stack
	ఒకవేళ (selection_దొంతర.ఖాళీలు())
		నెట్టు_నిండిన_selection();
}

std::string RTLIL::Design::గమ్యం_rtlil_str(తార్కిక only_selected) స్థిరరాశి
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(ఇది), only_selected, నిజం, అబద్ధం, RTLIL_BACKEND::Dump_బహుళకం::Readable);
	తిరిగి f.str();
}

std::సదిశ<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) స్థిరరాశి
{
	తార్కిక include_partials = partials == RTLIL::SELECT_ALL;
	తార్కిక exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	తార్కిక ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::సదిశ<RTLIL::Module*> ఫలితం;
	ఫలితం.reserve(modules_.కొలత());
	కొరకు (స్వయంచాలక &it : modules_) {
		ఒకవేళ (selected_సంపూర్ణ_module(it.మొదటి) || (include_partials && selected_module(it.మొదటి))) {
			ఒకవేళ (!(exclude_boxes && it.రెండవ->get_blackbox_attribute(ignore_wb)))
				ఫలితం.నెట్టు_వెనుక(it.రెండవ);
			లేకపోతే
				ఎంపిక (boxes)
				{
				స్థితి RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.రెండవ);
					ఆపు;
				స్థితి RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.రెండవ);
					ఆపు;
				స్థితి RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.రెండవ);
					ఆపు;
				స్థితి RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.రెండవ);
					ఆపు;
				స్థితి RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.రెండవ);
					ఆపు;
				స్థితి RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.రెండవ);
					ఆపు;
				అప్రమేయ:
					ఆపు;
				}
		} లేకపోతే ఒకవేళ (!include_partials && selected_module(it.మొదటి)) {
			ఎంపిక(partials)
			{
			స్థితి RTLIL::SELECT_సంపూర్ణ_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.రెండవ);
				ఆపు;
			స్థితి RTLIL::SELECT_సంపూర్ణ_ERR:
				log_error("Unsupported partially selected module %s.\n", it.రెండవ);
				ఆపు;
			స్థితి RTLIL::SELECT_సంపూర్ణ_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.రెండవ);
				ఆపు;
			అప్రమేయ:
				ఆపు;
			}
		}
	}
	తిరిగి ఫలితం;
}

RTLIL::Module::Module()
{
	స్థిర గుర్తులేని పూర్ణాంకం hashidx_లెక్క = 123456789;
	hashidx_లెక్క = mkhash_xorshift(hashidx_లెక్క);
	hashidx_ = hashidx_లెక్క;

	design = శూన్యసూచిక;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->చొప్పించు(std::pair<గుర్తులేని పూర్ణాంకం, RTLIL::Module*>(hashidx_, ఇది));
#అయితే_ముగింపు
}

RTLIL::Module::~Module()
{
	కొరకు (స్వయంచాలక &pr : wires_)
		తొలగించు pr.రెండవ;
	కొరకు (స్వయంచాలక &pr : memories)
		తొలగించు pr.రెండవ;
	కొరకు (స్వయంచాలక &pr : cells_)
		తొలగించు pr.రెండవ;
	కొరకు (స్వయంచాలక &pr : processes)
		తొలగించు pr.రెండవ;
#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#అయితే_ముగింపు
}

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
స్థిర std::map<గుర్తులేని పూర్ణాంకం, RTLIL::Module*> all_modules;
std::map<గుర్తులేని పూర్ణాంకం, RTLIL::Module*> *RTLIL::Module::get_all_modules(శూన్య)
{
	తిరిగి &all_modules;
}
#అయితే_ముగింపు

శూన్య RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	కొరకు (స్వయంచాలక it = wires_.begin(); it != wires_.చివర(); ++it)
		ఒకవేళ (!it->రెండవ->పోర్టు_ఆదానం && !it->రెండవ->పోర్టు_output)
			delwires.చొప్పించు(it->రెండవ);

	కొరకు (స్వయంచాలక it = memories.begin(); it != memories.చివర(); ++it)
		తొలగించు it->రెండవ;
	memories.clear();

	కొరకు (స్వయంచాలక it = cells_.begin(); it != cells_.చివర(); ++it)
		తొలగించు it->రెండవ;
	cells_.clear();

	కొరకు (స్వయంచాలక it = processes.begin(); it != processes.చివర(); ++it)
		తొలగించు it->రెండవ;
	processes.clear();

	connections_.clear();

	remove(delwires);
	సమితి_bool_attribute(ID::blackbox);
}

శూన్య RTLIL::Module::expand_interfaces(RTLIL::Design *, స్థిరరాశి dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

తార్కిక RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	తిరిగి అబద్ధం;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, స్థిరరాశి dict<RTLIL::IdString, RTLIL::Const> &, తార్కిక mayfail)
{
	ఒకవేళ (mayfail)
		తిరిగి IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, స్థిరరాశి dict<RTLIL::IdString, RTLIL::Const> &, స్థిరరాశి dict<RTLIL::IdString, RTLIL::Module*> &, స్థిరరాశి dict<RTLIL::IdString, RTLIL::IdString> &, తార్కిక mayfail)
{
	ఒకవేళ (mayfail)
		తిరిగి IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

కొలత_t RTLIL::Module::లెక్క_id(RTLIL::IdString id)
{
	తిరిగి wires_.లెక్క(id) + cells_.లెక్క(id) + memories.లెక్క(id) + processes.లెక్క(id);
}

#నిర్వచితం_కాకపోతే NDEBUG
నామాకాశ {
	నిర్మాణం అంతర్గత_Cell_Checker
	{
		స్థిరరాశి RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		అంతర్గత_Cell_Checker(స్థిరరాశి RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		శూన్య error(పూర్ణాంకం linenr, std::optional<std::string> error_సందేశం = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			ఒకవేళ (error_సందేశం) {
				log_error("%s in internal cell %s\n", *error_సందేశం, cell_loc);
			} లేకపోతే {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		పూర్ణాంకం param(RTLIL::IdString name)
		{
			స్వయంచాలక it = cell->parameters.కనుగొను(name);
			ఒకవేళ (it == cell->parameters.చివర()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.చొప్పించు(name);
			తిరిగి it->రెండవ.as_int();
		}

		పూర్ణాంకం param_bool(RTLIL::IdString name)
		{
			పూర్ణాంకం v = param(name);
			ఒకవేళ (Get_కొలత(cell->parameters.at(name)) > 32)
				error(__LINE__);
			ఒకవేళ (v != 0 && v != 1)
				error(__LINE__);
			తిరిగి v;
		}

		పూర్ణాంకం param_bool(RTLIL::IdString name, తార్కిక expected)
		{
			పూర్ణాంకం v = param_bool(name);
			ఒకవేళ (v != expected)
				error(__LINE__);
			తిరిగి v;
		}

		శూన్య param_బిట్లు(RTLIL::IdString name, పూర్ణాంకం వెడల్పు)
		{
			param(name);
			ఒకవేళ (Get_కొలత(cell->parameters.at(name)) != వెడల్పు)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			తిరిగి cell->parameters.at(name).decode_string();
		}

		శూన్య పోర్టు(RTLIL::IdString name, పూర్ణాంకం వెడల్పు)
		{
			స్వయంచాలక it = cell->connections_.కనుగొను(name);
			ఒకవేళ (it == cell->connections_.చివర()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			ఒకవేళ (Get_కొలత(it->రెండవ) != వెడల్పు) {
				std::string err = stringf("Expected port to have width %d", వెడల్పు);
				error(__LINE__, err);
			}
			expected_ports.చొప్పించు(name);
		}

		శూన్య check_expected(తార్కిక check_matched_sign = అబద్ధం)
		{
			కొరకు (స్వయంచాలక &para : cell->parameters)
				ఒకవేళ (expected_params.లెక్క(para.మొదటి) == 0)
					error(__LINE__);
			కొరకు (స్వయంచాలక &conn : cell->connections())
				ఒకవేళ (expected_ports.లెక్క(conn.మొదటి) == 0)
					error(__LINE__);

			ఒకవేళ (check_matched_sign) {
				log_assert(expected_params.లెక్క(ID::A_SIGNED) != 0 && expected_params.లెక్క(ID::B_SIGNED) != 0);
				తార్కిక a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				తార్కిక b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				ఒకవేళ (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		శూన్య check()
		{
			ఒకవేళ (cell->type.is_ప్రజా() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				తిరిగి;

			ఒకవేళ (cell->type.లోపల(ID($buf), ID($barrier))) {
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::Y, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type.లోపల(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				పోర్టు(ID::A, param(ID::A_వెడల్పు));
				పోర్టు(ID::Y, param(ID::Y_వెడల్పు));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type.లోపల(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				పోర్టు(ID::A, param(ID::A_వెడల్పు));
				పోర్టు(ID::B, param(ID::B_వెడల్పు));
				పోర్టు(ID::Y, param(ID::Y_వెడల్పు));
				check_expected(నిజం);
				తిరిగి;
			}

			ఒకవేళ (cell->type.లోపల(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				పోర్టు(ID::A, param(ID::A_వెడల్పు));
				పోర్టు(ID::Y, param(ID::Y_వెడల్పు));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type.లోపల(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/అబద్ధం);
				పోర్టు(ID::A, param(ID::A_వెడల్పు));
				పోర్టు(ID::B, param(ID::B_వెడల్పు));
				పోర్టు(ID::Y, param(ID::Y_వెడల్పు));
				check_expected(/*check_matched_sign=*/అబద్ధం);
				తిరిగి;
			}

			ఒకవేళ (cell->type.లోపల(ID($జరుపు), ID($shiftx))) {
				ఒకవేళ (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/అబద్ధం);
				} లేకపోతే {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				పోర్టు(ID::A, param(ID::A_వెడల్పు));
				పోర్టు(ID::B, param(ID::B_వెడల్పు));
				పోర్టు(ID::Y, param(ID::Y_వెడల్పు));
				check_expected(/*check_matched_sign=*/అబద్ధం);
				తిరిగి;
			}

			ఒకవేళ (cell->type.లోపల(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				పోర్టు(ID::A, param(ID::A_వెడల్పు));
				పోర్టు(ID::B, param(ID::B_వెడల్పు));
				పోర్టు(ID::Y, param(ID::Y_వెడల్పు));
				check_expected(నిజం);
				తిరిగి;
			}

			ఒకవేళ (cell->type.లోపల(ID($కూడు), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ఘాతం))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				పోర్టు(ID::A, param(ID::A_వెడల్పు));
				పోర్టు(ID::B, param(ID::B_వెడల్పు));
				పోర్టు(ID::Y, param(ID::Y_వెడల్పు));
				check_expected(cell->type != ID($ఘాతం));
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($fa)) {
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::B, param(ID::వెడల్పు_2));
				పోర్టు(ID::C, param(ID::వెడల్పు_2));
				పోర్టు(ID::X, param(ID::వెడల్పు_2));
				పోర్టు(ID::Y, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($lcu)) {
				పోర్టు(ID::P, param(ID::వెడల్పు_2));
				పోర్టు(ID::G, param(ID::వెడల్పు_2));
				పోర్టు(ID::CI, 1);
				పోర్టు(ID::CO, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				పోర్టు(ID::A, param(ID::A_వెడల్పు));
				పోర్టు(ID::B, param(ID::B_వెడల్పు));
				పోర్టు(ID::CI, 1);
				పోర్టు(ID::BI, 1);
				పోర్టు(ID::X, param(ID::Y_వెడల్పు));
				పోర్టు(ID::Y, param(ID::Y_వెడల్పు));
				పోర్టు(ID::CO, param(ID::Y_వెడల్పు));
				check_expected(నిజం);
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_వెడల్పు);
				పోర్టు(ID::A, param(ID::A_వెడల్పు));
				పోర్టు(ID::B, param(ID::B_వెడల్పు));
				పోర్టు(ID::Y, param(ID::Y_వెడల్పు));
				check_expected();
				Macc().from_cell(cell);
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($macc_v2)) {
				ఒకవేళ (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				ఒకవేళ (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_బిట్లు(ID::లబ్ధం_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_బిట్లు(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_బిట్లు(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_బిట్లు(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_బిట్లు(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				ఒకవేళ (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_బిట్లు(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_బిట్లు(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_బిట్లు(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				స్థిరరాశి Const &a_వెడల్పు = cell->getParam(ID::A_WIDTHS);
				స్థిరరాశి Const &b_వెడల్పు = cell->getParam(ID::B_WIDTHS);
				స్థిరరాశి Const &c_వెడల్పు = cell->getParam(ID::C_WIDTHS);
				పూర్ణాంకం a_వెడల్పు_కూడిక = 0, b_వెడల్పు_కూడిక = 0, c_వెడల్పు_కూడిక = 0;
				కొరకు (పూర్ణాంకం i = 0; i < param(ID::NPRODUCTS); i++) {
					a_వెడల్పు_కూడిక += a_వెడల్పు.వెలికితీయి(16 * i, 16).as_int(అబద్ధం);
					b_వెడల్పు_కూడిక += b_వెడల్పు.వెలికితీయి(16 * i, 16).as_int(అబద్ధం);
				}
				కొరకు (పూర్ణాంకం i = 0; i < param(ID::NADDENDS); i++) {
					c_వెడల్పు_కూడిక += c_వెడల్పు.వెలికితీయి(16 * i, 16).as_int(అబద్ధం);
				}
				పోర్టు(ID::A, a_వెడల్పు_కూడిక);
				పోర్టు(ID::B, b_వెడల్పు_కూడిక);
				పోర్టు(ID::C, c_వెడల్పు_కూడిక);
				పోర్టు(ID::Y, param(ID::Y_వెడల్పు));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				పోర్టు(ID::A, param(ID::A_వెడల్పు));
				పోర్టు(ID::Y, param(ID::Y_వెడల్పు));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type.లోపల(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				పోర్టు(ID::A, param(ID::A_వెడల్పు));
				పోర్టు(ID::B, param(ID::B_వెడల్పు));
				పోర్టు(ID::Y, param(ID::Y_వెడల్పు));
				check_expected(/*check_matched_sign=*/అబద్ధం);
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($ముక్క)) {
				param(ID::స్థానభ్రంశం_2);
				పోర్టు(ID::A, param(ID::A_వెడల్పు));
				పోర్టు(ID::Y, param(ID::Y_వెడల్పు));
				ఒకవేళ (param(ID::స్థానభ్రంశం_2) + param(ID::Y_వెడల్పు) > param(ID::A_వెడల్పు))
					error(__LINE__);
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($concat)) {
				పోర్టు(ID::A, param(ID::A_వెడల్పు));
				పోర్టు(ID::B, param(ID::B_వెడల్పు));
				పోర్టు(ID::Y, param(ID::A_వెడల్పు) + param(ID::B_వెడల్పు));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($mux)) {
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::B, param(ID::వెడల్పు_2));
				పోర్టు(ID::S, 1);
				పోర్టు(ID::Y, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($pmux)) {
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::B, param(ID::వెడల్పు_2) * param(ID::S_వెడల్పు));
				పోర్టు(ID::S, param(ID::S_వెడల్పు));
				పోర్టు(ID::Y, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($bmux)) {
				పోర్టు(ID::A, param(ID::వెడల్పు_2) << param(ID::S_వెడల్పు));
				పోర్టు(ID::S, param(ID::S_వెడల్పు));
				పోర్టు(ID::Y, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($demux)) {
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::S, param(ID::S_వెడల్పు));
				పోర్టు(ID::Y, param(ID::వెడల్పు_2) << param(ID::S_వెడల్పు));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($lut)) {
				param(ID::LUT);
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::Y, 1);
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($sop)) {
				param(ID::లోతు_2);
				param(ID::పట్టిక);
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::Y, 1);
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($sr)) {
				param_bool(ID::సమితి_POLARITY);
				param_bool(ID::CLR_POLARITY);
				పోర్టు(ID::సమితి_2, param(ID::వెడల్పు_2));
				పోర్టు(ID::CLR, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q,   param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($ff)) {
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::EN, 1);
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::సమితి_POLARITY);
				param_bool(ID::CLR_POLARITY);
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::సమితి_2, param(ID::వెడల్పు_2));
				పోర్టు(ID::CLR, param(ID::వెడల్పు_2));
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($dffsre)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::సమితి_POLARITY);
				param_bool(ID::CLR_POLARITY);
				param_bool(ID::EN_POLARITY);
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::EN, 1);
				పోర్టు(ID::సమితి_2, param(ID::వెడల్పు_2));
				పోర్టు(ID::CLR, param(ID::వెడల్పు_2));
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_బిట్లు(ID::ARST_విలువ, param(ID::వెడల్పు_2));
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::ARST, 1);
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_బిట్లు(ID::SRST_విలువ, param(ID::వెడల్పు_2));
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::SRST, 1);
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type.లోపల(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_బిట్లు(ID::SRST_విలువ, param(ID::వెడల్పు_2));
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::EN, 1);
				పోర్టు(ID::SRST, 1);
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_బిట్లు(ID::ARST_విలువ, param(ID::వెడల్పు_2));
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::EN, 1);
				పోర్టు(ID::ARST, 1);
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::ALOAD, 1);
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::AD, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($aldffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::EN, 1);
				పోర్టు(ID::ALOAD, 1);
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::AD, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				పోర్టు(ID::EN, 1);
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_బిట్లు(ID::ARST_విలువ, param(ID::వెడల్పు_2));
				పోర్టు(ID::EN, 1);
				పోర్టు(ID::ARST, 1);
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::సమితి_POLARITY);
				param_bool(ID::CLR_POLARITY);
				పోర్టు(ID::EN, 1);
				పోర్టు(ID::సమితి_2, param(ID::వెడల్పు_2));
				పోర్టు(ID::CLR, param(ID::వెడల్పు_2));
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_బిట్లు);
				param(ID::STATE_అంకె);
				param(ID::STATE_అంకె_LOG2);
				param(ID::STATE_RST);
				param_బిట్లు(ID::STATE_పట్టిక, param(ID::STATE_బిట్లు) * param(ID::STATE_అంకె));
				param(ID::TRANS_అంకె);
				param_బిట్లు(ID::TRANS_పట్టిక, param(ID::TRANS_అంకె) * (2*param(ID::STATE_అంకె_LOG2) + param(ID::CTRL_లోపల_వెడల్పు) + param(ID::CTRL_బహిర్గతం_వెడల్పు)));
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::ARST, 1);
				పోర్టు(ID::CTRL_లోపల, param(ID::CTRL_లోపల_వెడల్పు));
				పోర్టు(ID::CTRL_బహిర్గతం, param(ID::CTRL_బహిర్గతం_వెడల్పు));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::EN, 1);
				పోర్టు(ID::ADDR, param(ID::ABITS));
				పోర్టు(ID::దత్తాంశం_2, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_ముసుగు);
				param(ID::COLLISION_X_ముసుగు);
				param_bool(ID::CE_OVER_SRST);
				param_బిట్లు(ID::ARST_విలువ, param(ID::వెడల్పు_2));
				param_బిట్లు(ID::SRST_విలువ, param(ID::వెడల్పు_2));
				param_బిట్లు(ID::INIT_విలువ, param(ID::వెడల్పు_2));
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::EN, 1);
				పోర్టు(ID::ARST, 1);
				పోర్టు(ID::SRST, 1);
				పోర్టు(ID::ADDR, param(ID::ABITS));
				పోర్టు(ID::దత్తాంశం_2, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::ప్రాధాన్యత);
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::EN, param(ID::వెడల్పు_2));
				పోర్టు(ID::ADDR, param(ID::ABITS));
				పోర్టు(ID::దత్తాంశం_2, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($memwr_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PORTID);
				param(ID::ప్రాధాన్యత_ముసుగు_2);
				పోర్టు(ID::CLK, 1);
				పోర్టు(ID::EN, param(ID::వెడల్పు_2));
				పోర్టు(ID::ADDR, param(ID::ABITS));
				పోర్టు(ID::దత్తాంశం_2, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::ప్రాధాన్యత);
				పోర్టు(ID::ADDR, param(ID::ABITS));
				పోర్టు(ID::దత్తాంశం_2, param(ID::వెడల్పు_2) * param(ID::పదాలు));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::ప్రాధాన్యత);
				పోర్టు(ID::ADDR, param(ID::ABITS));
				పోర్టు(ID::దత్తాంశం_2, param(ID::వెడల్పు_2) * param(ID::పదాలు));
				పోర్టు(ID::EN, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($mem)) {
				param(ID::MEMID);
				param(ID::కొలత_2);
				param(ID::స్థానభ్రంశం_2);
				param(ID::INIT);
				param_బిట్లు(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_బిట్లు(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_బిట్లు(ID::RD_TRANSPARENT, max(1, param(ID::RD_PORTS)));
				param_బిట్లు(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_బిట్లు(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				పోర్టు(ID::RD_CLK, param(ID::RD_PORTS));
				పోర్టు(ID::RD_EN, param(ID::RD_PORTS));
				పోర్టు(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				పోర్టు(ID::RD_దత్తాంశం, param(ID::RD_PORTS) * param(ID::వెడల్పు_2));
				పోర్టు(ID::WR_CLK, param(ID::WR_PORTS));
				పోర్టు(ID::WR_EN, param(ID::WR_PORTS) * param(ID::వెడల్పు_2));
				పోర్టు(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				పోర్టు(ID::WR_దత్తాంశం, param(ID::WR_PORTS) * param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($mem_v2)) {
				param(ID::MEMID);
				param(ID::కొలత_2);
				param(ID::స్థానభ్రంశం_2);
				param(ID::INIT);
				param_బిట్లు(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_బిట్లు(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_బిట్లు(ID::RD_TRANSPARENCY_ముసుగు, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_బిట్లు(ID::RD_COLLISION_X_ముసుగు, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_బిట్లు(ID::RD_WIDE_CONTINUATION, max(1, param(ID::RD_PORTS)));
				param_బిట్లు(ID::RD_CE_OVER_SRST, max(1, param(ID::RD_PORTS)));
				param_బిట్లు(ID::RD_ARST_విలువ, max(1, param(ID::RD_PORTS) * param(ID::వెడల్పు_2)));
				param_బిట్లు(ID::RD_SRST_విలువ, max(1, param(ID::RD_PORTS) * param(ID::వెడల్పు_2)));
				param_బిట్లు(ID::RD_INIT_విలువ, max(1, param(ID::RD_PORTS) * param(ID::వెడల్పు_2)));
				param_బిట్లు(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_బిట్లు(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				param_బిట్లు(ID::WR_WIDE_CONTINUATION, max(1, param(ID::WR_PORTS)));
				param_బిట్లు(ID::WR_ప్రాధాన్యత_ముసుగు, max(1, param(ID::WR_PORTS) * param(ID::WR_PORTS)));
				పోర్టు(ID::RD_CLK, param(ID::RD_PORTS));
				పోర్టు(ID::RD_EN, param(ID::RD_PORTS));
				పోర్టు(ID::RD_ARST, param(ID::RD_PORTS));
				పోర్టు(ID::RD_SRST, param(ID::RD_PORTS));
				పోర్టు(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				పోర్టు(ID::RD_దత్తాంశం, param(ID::RD_PORTS) * param(ID::వెడల్పు_2));
				పోర్టు(ID::WR_CLK, param(ID::WR_PORTS));
				పోర్టు(ID::WR_EN, param(ID::WR_PORTS) * param(ID::వెడల్పు_2));
				పోర్టు(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				పోర్టు(ID::WR_దత్తాంశం, param(ID::WR_PORTS) * param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($tribuf)) {
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::Y, param(ID::వెడల్పు_2));
				పోర్టు(ID::EN, 1);
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($bweqx)) {
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::B, param(ID::వెడల్పు_2));
				పోర్టు(ID::Y, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($bwmux)) {
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::B, param(ID::వెడల్పు_2));
				పోర్టు(ID::S, param(ID::వెడల్పు_2));
				పోర్టు(ID::Y, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type.లోపల(ID($ప్రతిజ్ఞా), ID($assume), ID($సజీవ), ID($fair), ID($cover))) {
				పోర్టు(ID::A, 1);
				పోర్టు(ID::EN, 1);
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($initstate)) {
				పోర్టు(ID::Y, 1);
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type.లోపల(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				పోర్టు(ID::Y, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type.లోపల(ID($anyinit))) {
				పోర్టు(ID::D, param(ID::వెడల్పు_2));
				పోర్టు(ID::Q, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($equiv)) {
				పోర్టు(ID::A, 1);
				పోర్టు(ID::B, 1);
				పోర్టు(ID::Y, 1);
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type.లోపల(ID($specify2), ID($specify3))) {
				param_bool(ID::నిండిన);
				param_bool(ID::SRC_DST_PEN);
				param_bool(ID::SRC_DST_POL);
				param(ID::T_RISE_MIN);
				param(ID::T_RISE_TYP);
				param(ID::T_RISE_MAX);
				param(ID::T_FALL_MIN);
				param(ID::T_FALL_TYP);
				param(ID::T_FALL_MAX);
				పోర్టు(ID::EN, 1);
				పోర్టు(ID::SRC, param(ID::SRC_వెడల్పు));
				పోర్టు(ID::DST, param(ID::DST_వెడల్పు));
				ఒకవేళ (cell->type == ID($specify3)) {
					param_bool(ID::అంచు_EN);
					param_bool(ID::అంచు_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					పోర్టు(ID::DAT, param(ID::DST_వెడల్పు));
				}
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($specrule)) {
				param(ID::TYPE);
				param_bool(ID::SRC_PEN);
				param_bool(ID::SRC_POL);
				param_bool(ID::DST_PEN);
				param_bool(ID::DST_POL);
				param(ID::T_హద్దు_MIN);
				param(ID::T_హద్దు_TYP);
				param(ID::T_హద్దు_MAX);
				param(ID::T_LIMIT2_MIN);
				param(ID::T_LIMIT2_TYP);
				param(ID::T_LIMIT2_MAX);
				పోర్టు(ID::SRC_EN, 1);
				పోర్టు(ID::DST_EN, 1);
				పోర్టు(ID::SRC, param(ID::SRC_వెడల్పు));
				పోర్టు(ID::DST, param(ID::DST_వెడల్పు));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::ప్రాధాన్యత);
				పోర్టు(ID::EN, 1);
				పోర్టు(ID::TRG, param(ID::TRG_వెడల్పు));
				పోర్టు(ID::వాదనలు, param(ID::వాదనలు_వెడల్పు));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				ఒకవేళ (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
					error(__LINE__);
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::ప్రాధాన్యత);
				పోర్టు(ID::A, 1);
				పోర్టు(ID::EN, 1);
				పోర్టు(ID::TRG, param(ID::TRG_వెడల్పు));
				పోర్టు(ID::వాదనలు, param(ID::వాదనలు_వెడల్పు));
				check_expected();
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				ఒకవేళ (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				తిరిగి;
			}

			ఒకవేళ (cell->type == ID($_BUF_))    { పోర్టు(ID::A,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_NOT_))    { పోర్టు(ID::A,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_AND_))    { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_NAND_))   { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_OR_))     { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_NOR_))    { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_XOR_))    { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_XNOR_))   { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_ANDNOT_)) { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_ORNOT_))  { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_MUX_))    { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::S,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_NMUX_))   { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::S,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_AOI3_))   { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::C,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_OAI3_))   { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::C,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_AOI4_))   { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::C,1); పోర్టు(ID::D,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_OAI4_))   { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::C,1); పోర్టు(ID::D,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type == ID($_TBUF_))  { పోర్టు(ID::A,1); పోర్టు(ID::Y,1); పోర్టు(ID::E,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type == ID($_MUX4_))  { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::C,1); పోర్టు(ID::D,1); పోర్టు(ID::S,1); పోర్టు(ID::T,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_MUX8_))  { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::C,1); పోర్టు(ID::D,1); పోర్టు(ID::E,1); పోర్టు(ID::F,1); పోర్టు(ID::G,1); పోర్టు(ID::H,1); పోర్టు(ID::S,1); పోర్టు(ID::T,1); పోర్టు(ID::U,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }
			ఒకవేళ (cell->type == ID($_MUX16_)) { పోర్టు(ID::A,1); పోర్టు(ID::B,1); పోర్టు(ID::C,1); పోర్టు(ID::D,1); పోర్టు(ID::E,1); పోర్టు(ID::F,1); పోర్టు(ID::G,1); పోర్టు(ID::H,1); పోర్టు(ID::I,1); పోర్టు(ID::J,1); పోర్టు(ID::K,1); పోర్టు(ID::L,1); పోర్టు(ID::M,1); పోర్టు(ID::N,1); పోర్టు(ID::O,1); పోర్టు(ID::P,1); పోర్టు(ID::S,1); పోర్టు(ID::T,1); పోర్టు(ID::U,1); పోర్టు(ID::V,1); పోర్టు(ID::Y,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ పోర్టు(ID::S,1); పోర్టు(ID::R,1); పోర్టు(ID::Q,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type == ID($_FF_)) { పోర్టు(ID::D,1); పోర్టు(ID::Q,1); check_expected();  తిరిగి; }

			ఒకవేళ (cell->type.లోపల(ID($_DFF_N_), ID($_DFF_P_)))
				{ పోర్టు(ID::D,1); పోర్టు(ID::Q,1); పోర్టు(ID::C,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ పోర్టు(ID::D,1); పోర్టు(ID::Q,1); పోర్టు(ID::C,1); పోర్టు(ID::E,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ పోర్టు(ID::D,1); పోర్టు(ID::Q,1); పోర్టు(ID::C,1); పోర్టు(ID::R,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ పోర్టు(ID::D,1); పోర్టు(ID::Q,1); పోర్టు(ID::C,1); పోర్టు(ID::R,1); పోర్టు(ID::E,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ పోర్టు(ID::D,1); పోర్టు(ID::Q,1); పోర్టు(ID::C,1); పోర్టు(ID::L,1); పోర్టు(ID::AD,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ పోర్టు(ID::D,1); పోర్టు(ID::Q,1); పోర్టు(ID::C,1); పోర్టు(ID::L,1); పోర్టు(ID::AD,1); పోర్టు(ID::E,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ పోర్టు(ID::C,1); పోర్టు(ID::S,1); పోర్టు(ID::R,1); పోర్టు(ID::D,1); పోర్టు(ID::Q,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ పోర్టు(ID::C,1); పోర్టు(ID::S,1); పోర్టు(ID::R,1); పోర్టు(ID::D,1); పోర్టు(ID::E,1); పోర్టు(ID::Q,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ పోర్టు(ID::D,1); పోర్టు(ID::Q,1); పోర్టు(ID::C,1); పోర్టు(ID::R,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ పోర్టు(ID::D,1); పోర్టు(ID::Q,1); పోర్టు(ID::C,1); పోర్టు(ID::R,1); పోర్టు(ID::E,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ పోర్టు(ID::E,1); పోర్టు(ID::D,1); పోర్టు(ID::Q,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ పోర్టు(ID::E,1); పోర్టు(ID::R,1); పోర్టు(ID::D,1); పోర్టు(ID::Q,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ పోర్టు(ID::E,1); పోర్టు(ID::S,1); పోర్టు(ID::R,1); పోర్టు(ID::D,1); పోర్టు(ID::Q,1); check_expected(); తిరిగి; }

			ఒకవేళ (cell->type.లోపల(ID($సమితి_ట్యాగ్))) {
				param(ID::వెడల్పు_2);
				param(ID::ట్యాగ్_2);
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::సమితి_2, param(ID::వెడల్పు_2));
				పోర్టు(ID::CLR, param(ID::వెడల్పు_2));
				పోర్టు(ID::Y, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}
			ఒకవేళ (cell->type.లోపల(ID($get_ట్యాగ్),ID($original_ట్యాగ్))) {
				param(ID::వెడల్పు_2);
				param(ID::ట్యాగ్_2);
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::Y, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}
			ఒకవేళ (cell->type.లోపల(ID($overwrite_ట్యాగ్))) {
				param(ID::వెడల్పు_2);
				param(ID::ట్యాగ్_2);
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::సమితి_2, param(ID::వెడల్పు_2));
				పోర్టు(ID::CLR, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}
			ఒకవేళ (cell->type.లోపల(ID($future_ff))) {
				param(ID::వెడల్పు_2);
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::Y, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}
			ఒకవేళ (cell->type.లోపల(ID($ఆదానం_పోర్టు))) {
				param(ID::వెడల్పు_2);
				పోర్టు(ID::Y, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
			}
			ఒకవేళ (cell->type.లోపల(ID($connect))) {
				param(ID::వెడల్పు_2);
				పోర్టు(ID::A, param(ID::వెడల్పు_2));
				పోర్టు(ID::B, param(ID::వెడల్పు_2));
				check_expected();
				తిరిగి;
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
#అయితే_ముగింపు

శూన్య RTLIL::Module::క్రమీకరించు()
{
	wires_.క్రమీకరించు(క్రమీకరించు_by_id_str(design->twines()));
	cells_.క్రమీకరించు(క్రమీకరించు_by_id_str(design->twines()));
	parameter_default_విలువలు.క్రమీకరించు(క్రమీకరించు_by_id_str(design->twines()));
	memories.క్రమీకరించు(క్రమీకరించు_by_id_str(design->twines()));
	processes.క్రమీకరించు(క్రమీకరించు_by_id_str(design->twines()));
	కొరకు (స్వయంచాలక &it : cells_)
		it.రెండవ->క్రమీకరించు();
	కొరకు (స్వయంచాలక &it : wires_)
		it.రెండవ->లక్షణాలు.క్రమీకరించు(క్రమీకరించు_by_id_str(design->twines()));
	కొరకు (స్వయంచాలక &it : memories)
		it.రెండవ->లక్షణాలు.క్రమీకరించు(క్రమీకరించు_by_id_str(design->twines()));
}

#నిర్వచితం_కాకపోతే NDEBUG
స్థిర శూన్య check_id_కీలకాలు(స్థిరరాశి dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	కొరకు (స్వయంచాలక &it : keyed)
		log_assert(!it.మొదటి.ఖాళీలు());
}
#అయితే_ముగింపు

శూన్య check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#నిర్వచితం_కాకపోతే NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_కొలత(0, module->cells_కొలత(), 1000));
	స్థిరరాశి RTLIL::Module *const_module = module;

	pool<std::string> జ్ఞాపకం_అక్షరశ్రేణులు;
	కొరకు (స్వయంచాలక &it : module->memories) {
		log_assert(it.మొదటి == it.రెండవ->name);
		log_assert(it.మొదటి != IdString::Null);
		log_assert(it.రెండవ->వెడల్పు >= 0);
		log_assert(it.రెండవ->కొలత >= 0);
		check_id_కీలకాలు(it.రెండవ->లక్షణాలు);
		జ్ఞాపకం_అక్షరశ్రేణులు.చొప్పించు(it.రెండవ->name.str());
	}

	std::సదిశ<MonotonicFlag> ports_declared(Get_కొలత(module->ports));
	Sharded_సదిశ<std::string> memids(subpool);
	subpool.నిరంతరం([const_module, &ports_declared, &జ్ఞాపకం_అక్షరశ్రేణులు, &memids](స్థిరరాశి ParallelDispatchThreadPool::నిరంతరం_Ctx &ctx) {
		కొరకు (పూర్ణాంకం i : ctx.అంశం_range(const_module->cells_కొలత())) {
			స్వయంచాలక it = *const_module->cells_.element(i);
			log_assert(const_module == it.రెండవ->module);
			log_assert(it.మొదటి == it.రెండవ->name);
			log_assert(it.మొదటి != IdString::Null);
			log_assert(!it.రెండవ->type.ఖాళీలు());
			కొరకు (స్వయంచాలక &it2 : it.రెండవ->connections()) {
				log_assert(it2.మొదటి != IdString::Null);
				it2.రెండవ.check(const_module);
			}
			check_id_కీలకాలు(it.రెండవ->లక్షణాలు);
			check_id_కీలకాలు(it.రెండవ->parameters);
			అంతర్గత_Cell_Checker checker(const_module, it.రెండవ);
			checker.check();
			ఒకవేళ (it.రెండవ->has_memid()) {
				log_assert(జ్ఞాపకం_అక్షరశ్రేణులు.లెక్క(it.రెండవ->parameters.at(ID::MEMID).decode_string()));
			} లేకపోతే ఒకవేళ (it.రెండవ->is_mem_cell()) {
				std::string memid = it.రెండవ->parameters.at(ID::MEMID).decode_string();
				log_assert(!జ్ఞాపకం_అక్షరశ్రేణులు.లెక్క(memid));
				memids.చొప్పించు(ctx, std::move(memid));
			}
			స్వయంచాలక cell_mod = const_module->design->module(it.రెండవ->name);
			ఒకవేళ (cell_mod != శూన్యసూచిక) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#నిర్వచితమైతే __GNUC__
				#సూచన GCC diagnostic నెట్టు
				#సూచన GCC diagnostic ignored "-Wdeprecated-declarations"
				#అయితే_ముగింపు
				log_assert(!it.రెండవ->get_blackbox_attribute());
				#నిర్వచితమైతే __GNUC__
				#సూచన GCC diagnostic తీయి
				#అయితే_ముగింపు
			}
		}

		కొరకు (పూర్ణాంకం i : ctx.అంశం_range(const_module->wires_కొలత())) {
			స్వయంచాలక it = *const_module->wires_.element(i);
			log_assert(const_module == it.రెండవ->module);
			log_assert(it.మొదటి == it.రెండవ->name);
			log_assert(it.మొదటి != IdString::Null);
			log_assert(it.రెండవ->వెడల్పు >= 0);
			log_assert(it.రెండవ->పోర్టు_id >= 0);
			check_id_కీలకాలు(it.రెండవ->లక్షణాలు);
			ఒకవేళ (it.రెండవ->పోర్టు_id) {
				log_assert(Get_కొలత(const_module->ports) >= it.రెండవ->పోర్టు_id);
				log_assert(const_module->ports.at(it.రెండవ->పోర్టు_id-1) == it.రెండవ->name);
				log_assert(it.రెండవ->పోర్టు_ఆదానం || it.రెండవ->పోర్టు_output);
				log_assert(it.రెండవ->పోర్టు_id <= Get_కొలత(ports_declared));
				తార్కిక previously_declared = ports_declared[it.రెండవ->పోర్టు_id-1].సమితి_and_return_old();
				log_assert(previously_declared == అబద్ధం);
			} లేకపోతే
				log_assert(!it.రెండవ->పోర్టు_ఆదానం && !it.రెండవ->పోర్టు_output);
		}
	});
	కొరకు (స్థిరరాశి MonotonicFlag &పోర్టు_declared : ports_declared)
		log_assert(పోర్టు_declared.ఎక్కించు() == నిజం);
	pool<std::string> memids_pool;
	కొరకు (std::string &memid : memids)
		log_assert(memids_pool.చొప్పించు(memid).రెండవ);

	కొరకు (స్వయంచాలక &it : module->processes) {
		log_assert(it.మొదటి == it.రెండవ->name);
		log_assert(it.మొదటి != IdString::Null);
		log_assert(it.రెండవ->మూలం_case.compare.ఖాళీలు());
		std::సదిశ<RTLIL::Case_నియమం*> all_cases = {&it.రెండవ->మూలం_case};
		కొరకు (కొలత_t i = 0; i < all_cases.కొలత(); i++) {
			కొరకు (స్వయంచాలక &switch_it : all_cases[i]->switches) {
				కొరకు (స్వయంచాలక &case_it : switch_it->cases) {
					కొరకు (స్వయంచాలక &compare_it : case_it->compare) {
						log_assert(switch_it->సంకేతం.కొలత() == compare_it.కొలత());
					}
					all_cases.నెట్టు_వెనుక(case_it);
				}
			}
		}
		కొరకు (స్వయంచాలక &sync_it : it.రెండవ->syncs) {
			ఎంపిక (sync_it->type) {
				స్థితి RTLIL::SyncType::ST0:
				స్థితి RTLIL::SyncType::ST1:
				స్థితి RTLIL::SyncType::STp:
				స్థితి RTLIL::SyncType::STn:
				స్థితి RTLIL::SyncType::STe:
					log_assert(!sync_it->సంకేతం.ఖాళీలు());
					ఆపు;
				స్థితి RTLIL::SyncType::STa:
				స్థితి RTLIL::SyncType::STg:
				స్థితి RTLIL::SyncType::STi:
					log_assert(sync_it->సంకేతం.ఖాళీలు());
					ఆపు;
			}
		}
	}

	కొరకు (స్వయంచాలక &it : module->connections_) {
		log_assert(it.మొదటి.కొలత() == it.రెండవ.కొలత());
		log_assert(!it.మొదటి.has_const());
		it.మొదటి.check(module);
		it.రెండవ.check(module);
	}

	check_id_కీలకాలు(module->లక్షణాలు);
#అయితే_ముగింపు
}

శూన్య RTLIL::Module::check()
{
	పూర్ణాంకం pool_కొలత = ThreadPool::work_pool_కొలత(0, cells_కొలత(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_కొలత);
	check_module(ఇది, thread_pool);
}

శూన్య RTLIL::Module::optimize()
{
}

శూన్య RTLIL::Module::cloneInto(RTLIL::Module *new_mod) స్థిరరాశి
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	కొరకు (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_విలువలు.clear();
	కొరకు (స్వయంచాలక &it : parameter_default_విలువలు)
		new_mod->parameter_default_విలువలు[dst_twines.copy_from(design->twines(), it.మొదటి)] = it.రెండవ;

	కొరకు (స్వయంచాలక &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->లక్షణాలు, లక్షణాలు, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	కొరకు (స్వయంచాలక it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->మొదటి);
		wire_map[it->రెండవ] = new_mod->కూడు_Wire(dst_id, it->రెండవ);
	}

	కొరకు (స్వయంచాలక it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->మొదటి);
		new_mod->కూడు_జ్ఞాపకం(dst_id, it->రెండవ);
	}

	కొరకు (స్వయంచాలక it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->మొదటి);
		new_mod->కూడు_Cell(dst_id, it->రెండవ);
	}

	కొరకు (స్వయంచాలక it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->మొదటి);
		new_mod->కూడు_Process(dst_id, it->రెండవ);
	}

	నిర్మాణం Rewrite_Sig_Spec_కార్మికుడు
	{
		స్థిరరాశి dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		స్థిరరాశి RTLIL::Module *new_mod;
		శూన్య క్రియాకారకం()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([ఇది](RTLIL::Wire *&wire) {
				స్వయంచాలక it = wire_map.కనుగొను(wire);
				ఒకవేళ (it != wire_map.చివర())
					wire = it->రెండవ;
				లేకపోతే
					log_assert(wire->module == new_mod);
			});
		}
	};

	Rewrite_Sig_Spec_కార్మికుడు rewrite_Sig_Spec_కార్మికుడు{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewrite_Sig_Spec_కార్మికుడు);
	new_mod->fixup_ports();
}

RTLIL::Module *RTLIL::Module::clone() స్థిరరాశి
{
	RTLIL::Module *new_mod = కొత్త RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	తిరిగి new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) స్థిరరాశి
{
	RTLIL::Module *new_mod = కొత్త RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->కూడు(new_mod);
	తిరిగి new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString లక్ష్యం_name) స్థిరరాశి
{
	RTLIL::Module *new_mod = కొత్త RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = లక్ష్యం_name;
	cloneInto(new_mod);
	dst->కూడు(new_mod);
	తిరిగి new_mod;
}

తార్కిక RTLIL::Module::has_memories() స్థిరరాశి
{
	తిరిగి !memories.ఖాళీలు();
}

తార్కిక RTLIL::Module::has_processes() స్థిరరాశి
{
	తిరిగి !processes.ఖాళీలు();
}

తార్కిక RTLIL::Module::has_memories_warn() స్థిరరాశి
{
	ఒకవేళ (!memories.ఖాళీలు())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", ఇది);
	తిరిగి !memories.ఖాళీలు();
}

తార్కిక RTLIL::Module::has_processes_warn() స్థిరరాశి
{
	ఒకవేళ (!processes.ఖాళీలు())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", ఇది);
	తిరిగి !processes.ఖాళీలు();
}

తార్కిక RTLIL::Module::is_selected() స్థిరరాశి
{
	తిరిగి design->selected_module(ఇది->name);
}

తార్కిక RTLIL::Module::is_selected_సంపూర్ణ() స్థిరరాశి
{
	తిరిగి design->selected_సంపూర్ణ_module(ఇది->name);
}

std::సదిశ<RTLIL::Wire*> RTLIL::Module::selected_wires() స్థిరరాశి
{
	std::సదిశ<RTLIL::Wire*> ఫలితం;
	ఫలితం.reserve(wires_.కొలత());
	కొరకు (స్వయంచాలక &it : wires_)
		ఒకవేళ (design->selected(ఇది, it.రెండవ))
			ఫలితం.నెట్టు_వెనుక(it.రెండవ);
	తిరిగి ఫలితం;
}

std::సదిశ<RTLIL::Cell*> RTLIL::Module::selected_cells() స్థిరరాశి
{
	std::సదిశ<RTLIL::Cell*> ఫలితం;
	ఫలితం.reserve(cells_.కొలత());
	కొరకు (స్వయంచాలక &it : cells_)
		ఒకవేళ (design->selected(ఇది, it.రెండవ))
			ఫలితం.నెట్టు_వెనుక(it.రెండవ);
	తిరిగి ఫలితం;
}

std::సదిశ<RTLIL::జ్ఞాపకం*> RTLIL::Module::selected_memories() స్థిరరాశి
{
	std::సదిశ<RTLIL::జ్ఞాపకం*> ఫలితం;
	ఫలితం.reserve(memories.కొలత());
	కొరకు (స్వయంచాలక &it : memories)
		ఒకవేళ (design->selected(ఇది, it.రెండవ))
			ఫలితం.నెట్టు_వెనుక(it.రెండవ);
	తిరిగి ఫలితం;
}

std::సదిశ<RTLIL::Process*> RTLIL::Module::selected_processes() స్థిరరాశి
{
	std::సదిశ<RTLIL::Process*> ఫలితం;
	ఫలితం.reserve(processes.కొలత());
	కొరకు (స్వయంచాలక &it : processes)
		ఒకవేళ (design->selected(ఇది, it.రెండవ))
			ఫలితం.నెట్టు_వెనుక(it.రెండవ);
	తిరిగి ఫలితం;
}

std::సదిశ<RTLIL::SelectedMember> RTLIL::Module::selected_members() స్థిరరాశి
{
	std::సదిశ<RTLIL::SelectedMember> ఫలితం;
	కొరకు (స్వయంచాలక cell : selected_cells())
		ఫలితం.నెట్టు_వెనుక({cell, cell->name});
	కొరకు (స్వయంచాలక జ్ఞాపకం_3 : selected_memories())
		ఫలితం.నెట్టు_వెనుక({జ్ఞాపకం_3, జ్ఞాపకం_3->name});
	కొరకు (స్వయంచాలక wire : selected_wires())
		ఫలితం.నెట్టు_వెనుక({wire, wire->name});
	కొరకు (స్వయంచాలక process : selected_processes())
		ఫలితం.నెట్టు_వెనుక({process, process->name});
	తిరిగి ఫలితం;
}

శూన్య RTLIL::Module::కూడు(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(లెక్క_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = ఇది;
}

శూన్య RTLIL::Module::కూడు(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(లెక్క_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = ఇది;
}

శూన్య RTLIL::Module::కూడు(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(లెక్క_id(process->name) == 0);
	processes[process->name] = process;
	process->module = ఇది;
}

శూన్య RTLIL::Module::remove(స్థిరరాశి pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	ఒకవేళ (wires.ఖాళీలు())
		తిరిగి;

	నిర్మాణం Delete_Wire_కార్మికుడు
	{
		RTLIL::Module *module;
		స్థిరరాశి pool<RTLIL::Wire*> *wires_p;

		శూన్య క్రియాకారకం()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([ఇది](RTLIL::Wire *&wire) {
				ఒకవేళ (wires_p->లెక్క(wire))
					wire = module->కూడు_Wire(stringf("$delete_wire$%d", (పూర్ణాంకం)autoidx++), wire->వెడల్పు);
			});
		}

		శూన్య క్రియాకారకం()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	Delete_Wire_కార్మికుడు delete_wire_కార్మికుడు;
	delete_wire_కార్మికుడు.module = ఇది;
	delete_wire_కార్మికుడు.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_కార్మికుడు);

	ఒకవేళ (design->flagBufferedNormalized) {
		కొరకు (స్వయంచాలక wire : wires) {
			buf_norm_wire_వరుస.erase(wire);
			buf_norm_connect_సూచీ.erase(wire);
		}
	}

	కొరకు (స్వయంచాలక &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.లెక్క(id) != 0);
		wires_.erase(id);
		తొలగించు it;
	}
}

శూన్య RTLIL::Module::remove(RTLIL::Cell *cell)
{
	అయితే (!cell->connections_.ఖాళీలు())
		cell->unset_పోర్టు(cell->connections_.begin()->మొదటి);

	log_assert(cells_.లెక్క(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	ఒకవేళ (design && design->flagBufferedNormalized && buf_norm_cell_వరుస.లెక్క(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.చొప్పించు(cell);
	} లేకపోతే {
		తొలగించు cell;
	}
}

శూన్య RTLIL::Module::remove(RTLIL::జ్ఞాపకం *జ్ఞాపకం_3)
{
	log_assert(memories.లెక్క(జ్ఞాపకం_3->name) != 0);
	memories.erase(జ్ఞాపకం_3->name);
	తొలగించు జ్ఞాపకం_3;
}

శూన్య RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.లెక్క(process->name) != 0);
	processes.erase(process->name);
	తొలగించు process;
}

శూన్య RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	కూడు(wire);
}

శూన్య RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	కూడు(cell);
}

శూన్య RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	ఒకవేళ (old_id != IdString::Null && wires_.లెక్క(old_id))
		rename(wires_.at(old_id), new_name);
	లేకపోతే ఒకవేళ (old_id != IdString::Null && cells_.లెక్క(old_id))
		rename(cells_.at(old_id), new_name);
	లేకపోతే
		log_abort();
}

శూన్య RTLIL::Module::మార్పిడి_పేర్లు(RTLIL::Wire *w1, RTLIL::Wire *w2)
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
	మార్పిడి(w1->name, w2->name);
}

శూన్య RTLIL::Module::మార్పిడి_పేర్లు(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	మార్పిడి(c1->name, c2->name);
}

RTLIL::IdString RTLIL::Module::uniquify(RTLIL::IdString name)
{
	పూర్ణాంకం సూచీ = 0;
	తిరిగి uniquify(name, సూచీ);
}

IdString RTLIL::Module::uniquify(IdString name, పూర్ణాంకం &సూచీ)
{
	ఒకవేళ (సూచీ == 0) {
		ఒకవేళ (లెక్క_id(name) == 0)
			తిరిగి name;
		సూచీ++;
	}

	అయితే (1) {
		IdString new_name = design->twines().కూడు(TwineSpec::Suffix{name, stringf("_%d", సూచీ)});
		ఒకవేళ (లెక్క_id(new_name) == 0)
			తిరిగి new_name;
		సూచీ++;
	}
}

స్థిర తార్కిక fixup_ports_compare(స్థిరరాశి RTLIL::Wire *a, స్థిరరాశి RTLIL::Wire *b)
{
	ఒకవేళ (a->పోర్టు_id && !b->పోర్టు_id)
		తిరిగి నిజం;
	ఒకవేళ (!a->పోర్టు_id && b->పోర్టు_id)
		తిరిగి అబద్ధం;

	ఒకవేళ (a->పోర్టు_id == b->పోర్టు_id)
		తిరిగి a->name < b->name;
	తిరిగి a->పోర్టు_id < b->పోర్టు_id;
}

శూన్య RTLIL::Module::connect(స్థిరరాశి RTLIL::SigSig &conn)
{
	కొరకు (స్వయంచాలక mon : monitors)
		mon->notify_connect(ఇది, conn);

	ఒకవేళ (design)
		కొరకు (స్వయంచాలక mon : design->monitors)
			mon->notify_connect(ఇది, conn);

	// ignore all attempts to assign constants to other constants
	ఒకవేళ (conn.మొదటి.has_const()) {
		RTLIL::SigSig new_conn;
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(conn.మొదటి); i++)
			ఒకవేళ (conn.మొదటి[i].wire) {
				new_conn.మొదటి.append(conn.మొదటి[i]);
				new_conn.రెండవ.append(conn.రెండవ[i]);
			}
		ఒకవేళ (Get_కొలత(new_conn.మొదటి))
			connect(new_conn);
		తిరిగి;
	}

	ఒకవేళ (yosys_xtrace) {
		సంవర్గమానం("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", ఇది, log_signal(conn.మొదటి), log_signal(conn.రెండవ), Get_కొలత(conn.మొదటి));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(Get_కొలత(conn.మొదటి) == Get_కొలత(conn.రెండవ));
	connections_.నెట్టు_వెనుక(conn);
}

శూన్య RTLIL::Module::connect(స్థిరరాశి RTLIL::SigSpec &lhs, స్థిరరాశి RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

శూన్య RTLIL::Module::new_connections(స్థిరరాశి std::సదిశ<RTLIL::SigSig> &new_conn)
{
	కొరకు (స్వయంచాలక mon : monitors)
		mon->notify_connect(ఇది, new_conn);

	ఒకవేళ (design)
		కొరకు (స్వయంచాలక mon : design->monitors)
			mon->notify_connect(ఇది, new_conn);

	ఒకవేళ (yosys_xtrace) {
		సంవర్గమానం("#X# New connections vector in %s:\n", ఇది);
		కొరకు (స్వయంచాలక &conn: new_conn)
			సంవర్గమానం("#X#    %s = %s (%d bits)\n", log_signal(conn.మొదటి), log_signal(conn.రెండవ), Get_కొలత(conn.మొదటి));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

స్థిరరాశి std::సదిశ<RTLIL::SigSig> &RTLIL::Module::connections() స్థిరరాశి
{
	తిరిగి connections_;
}

శూన్య RTLIL::Module::fixup_ports()
{
	std::సదిశ<RTLIL::Wire*> all_ports;

	కొరకు (స్వయంచాలక &w : wires_)
		ఒకవేళ (w.రెండవ->పోర్టు_ఆదానం || w.రెండవ->పోర్టు_output)
			all_ports.నెట్టు_వెనుక(w.రెండవ);
		లేకపోతే
			w.రెండవ->పోర్టు_id = 0;

	std::క్రమీకరించు(all_ports.begin(), all_ports.చివర(), fixup_ports_compare);

	ఒకవేళ (design && design->flagBufferedNormalized) {
		కొరకు (స్వయంచాలక &w : wires_)
			ఒకవేళ (w.రెండవ->driverCell_ && w.రెండవ->driverCell_->type == ID($ఆదానం_పోర్టు))
				buf_norm_wire_వరుస.చొప్పించు(w.రెండవ);

		buf_norm_wire_వరుస.చొప్పించు(all_ports.begin(), all_ports.చివర());
	}

	ports.clear();
	కొరకు (కొలత_t i = 0; i < all_ports.కొలత(); i++) {
		ports.నెట్టు_వెనుక(all_ports[i]->name);
		all_ports[i]->పోర్టు_id = i+1;
	}
}

శూన్య RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		స్థిరరాశి dict<IdString, RTLIL::Const> &src,
		స్థిరరాశి RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	ఒకవేళ (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		తిరిగి;
	}
	dst.clear();
	కొరకు (పూర్ణాంకం i = Get_కొలత(src) - 1; i >= 0; i--) {
		స్వయంచాలక &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.మొదటి)] = it.రెండవ;
	}
}

RTLIL::Wire *RTLIL::Module::కూడు_Wire(RTLIL::IdString name, పూర్ణాంకం వెడల్పు)
{
	log_assert(design);
	log_assert(వెడల్పు >= 0 && వెడల్పు < RTLIL::వెడల్పు_హద్దు);
	RTLIL::Wire *wire = కొత్త RTLIL::Wire;
	wire->వెడల్పు = వెడల్పు;
	wire->name = name;
	కూడు(wire);
	తిరిగి wire;
}

RTLIL::Wire *RTLIL::Module::కూడు_Wire(IdString name, స్థిరరాశి RTLIL::Wire *other)
{
	RTLIL::Wire *wire = కూడు_Wire(name);
	wire->వెడల్పు = other->వెడల్పు;
	wire->ప్రారంభం_స్థానభ్రంశం = other->ప్రారంభం_స్థానభ్రంశం;
	wire->పోర్టు_id = other->పోర్టు_id;
	wire->పోర్టు_ఆదానం = other->పోర్టు_ఆదానం;
	wire->పోర్టు_output = other->పోర్టు_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		స్థిరరాశి RTLIL::Design *src_design = other->module ? other->module->design : శూన్యసూచిక;
		RTLIL::copy_attr_dict(wire->లక్షణాలు, other->లక్షణాలు, src_design, ఇది->design);
	}
	తిరిగి wire;
}

RTLIL::Cell *RTLIL::Module::కూడు_Cell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = కొత్త RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	కూడు(cell);
	తిరిగి cell;
}

RTLIL::Cell *RTLIL::Module::కూడు_Cell(IdString name, స్థిరరాశి RTLIL::Cell *other)
{
	స్థిరరాశి RTLIL::Design *src_design = other->module ? other->module->design : శూన్యసూచిక;
	తార్కిక cross_pool = src_design && ఇది->design && src_design != ఇది->design;

	IdString type = other->type.ref();
	ఒకవేళ (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = కూడు_Cell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, ఇది->design);
	RTLIL::copy_attr_dict(cell->లక్షణాలు, other->లక్షణాలు, src_design, ఇది->design);

	ఒకవేళ (!cross_pool)
		cell->connections_ = other->connections_;
	లేకపోతే
		కొరకు (పూర్ణాంకం i = Get_కొలత(other->connections_) - 1; i >= 0; i--) {
			స్వయంచాలక &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.మొదటి)] = c.రెండవ;
		}
	తిరిగి cell;
}

RTLIL::జ్ఞాపకం *RTLIL::Module::కూడు_జ్ఞాపకం(IdString name)
{
	log_assert(design);
	RTLIL::జ్ఞాపకం *mem = కొత్త RTLIL::జ్ఞాపకం;
	mem->module = ఇది;
	mem->name = name;
	memories[name] = mem;
	తిరిగి mem;
}

RTLIL::జ్ఞాపకం *RTLIL::Module::కూడు_జ్ఞాపకం(IdString name, స్థిరరాశి RTLIL::జ్ఞాపకం *other)
{
	log_assert(design);
	RTLIL::జ్ఞాపకం *mem = కొత్త RTLIL::జ్ఞాపకం;
	mem->module = ఇది;
	mem->name = name;
	mem->వెడల్పు = other->వెడల్పు;
	mem->ప్రారంభం_స్థానభ్రంశం = other->ప్రారంభం_స్థానభ్రంశం;
	mem->కొలత = other->కొలత;
	RTLIL::copy_attr_dict(mem->లక్షణాలు, other->లక్షణాలు,
			other->module ? other->module->design : శూన్యసూచిక, design);
	memories[name] = mem;
	తిరిగి mem;
}

RTLIL::Process *RTLIL::Module::కూడు_Process(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = కొత్త RTLIL::Process;
	proc->name = name;
	కూడు(proc);
	తిరిగి proc;
}

RTLIL::Process *RTLIL::Module::కూడు_Process(RTLIL::IdString name, స్థిరరాశి RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	స్థిరరాశి RTLIL::Design *src_design = other->module ? other->module->design : శూన్యసూచిక;
	ఒకవేళ (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> లక్షణాలు;
			RTLIL::copy_attr_dict(లక్షణాలు, obj->లక్షణాలు, src_design, design);
			obj->లక్షణాలు = std::move(లక్షణాలు);
		});
		కొరకు (స్వయంచాలక *సమకాలీకరణ : proc->syncs)
			కొరకు (స్వయంచాలక &mwa : సమకాలీకరణ->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	కూడు(proc);
	తిరిగి proc;
}

#నిర్వచించు DEF_పద్ధతి(_func, y_కొలత, _type) \
	RTLIL::Cell* RTLIL::Module::కూడు ## _func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, తార్కిక is_signed, స్థిరరాశి std::string &src) { \
		RTLIL::Cell *cell = కూడు_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_వెడల్పు] = sig_a.కొలత();       \
		cell->parameters[ID::Y_వెడల్పు] = sig_y.కొలత();       \
		cell->సమితి_పోర్టు(ID::A, sig_a);                        \
		cell->సమితి_పోర్టు(ID::Y, sig_y);                        \
		cell->సమితి_src_attribute(src);                       \
		తిరిగి cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, తార్కిక is_signed, స్థిరరాశి std::string &src) { \
		RTLIL::SigSpec sig_y = కూడు_Wire(NEW_ID, y_కొలత);    \
		కూడు ## _func(name, sig_a, sig_y, is_signed, src);   \
		తిరిగి sig_y;                                       \
	}
DEF_పద్ధతి(Not,        sig_a.కొలత(), ID($not))
DEF_పద్ధతి(Pos,        sig_a.కొలత(), ID($pos))
DEF_పద్ధతి(Neg,        sig_a.కొలత(), ID($neg))
DEF_పద్ధతి(ReduceAnd,  1, ID($reduce_and))
DEF_పద్ధతి(ReduceOr,   1, ID($reduce_or))
DEF_పద్ధతి(ReduceXor,  1, ID($reduce_xor))
DEF_పద్ధతి(ReduceXnor, 1, ID($reduce_xnor))
DEF_పద్ధతి(ReduceBool, 1, ID($reduce_bool))
DEF_పద్ధతి(LogicNot,   1, ID($logic_not))
#నిర్వచనం_రద్దు DEF_పద్ధతి

#నిర్వచించు DEF_పద్ధతి(_func, y_కొలత, _type) \
	RTLIL::Cell* RTLIL::Module::కూడు ## _func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, తార్కిక /* is_signed */, స్థిరరాశి std::string &src) { \
		RTLIL::Cell *cell = కూడు_Cell(name, _type);           \
		cell->parameters[ID::వెడల్పు_2] = sig_a.కొలత();         \
		cell->సమితి_పోర్టు(ID::A, sig_a);                        \
		cell->సమితి_పోర్టు(ID::Y, sig_y);                        \
		cell->సమితి_src_attribute(src);                       \
		తిరిగి cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, తార్కిక is_signed, స్థిరరాశి std::string &src) { \
		RTLIL::SigSpec sig_y = కూడు_Wire(NEW_ID, y_కొలత);    \
		కూడు ## _func(name, sig_a, sig_y, is_signed, src);   \
		తిరిగి sig_y;                                       \
	}
DEF_పద్ధతి(Buf,     sig_a.కొలత(), ID($buf))
DEF_పద్ధతి(Barrier, sig_a.కొలత(), ID($barrier))
#నిర్వచనం_రద్దు DEF_పద్ధతి

#నిర్వచించు DEF_పద్ధతి(_func, y_కొలత, _type) \
	RTLIL::Cell* RTLIL::Module::కూడు ## _func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, తార్కిక is_signed, స్థిరరాశి std::string &src) { \
		RTLIL::Cell *cell = కూడు_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_వెడల్పు] = sig_a.కొలత();       \
		cell->parameters[ID::B_వెడల్పు] = sig_b.కొలత();       \
		cell->parameters[ID::Y_వెడల్పు] = sig_y.కొలత();       \
		cell->సమితి_పోర్టు(ID::A, sig_a);                        \
		cell->సమితి_పోర్టు(ID::B, sig_b);                        \
		cell->సమితి_పోర్టు(ID::Y, sig_y);                        \
		cell->సమితి_src_attribute(src);                       \
		తిరిగి cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, తార్కిక is_signed, స్థిరరాశి std::string &src) { \
		RTLIL::SigSpec sig_y = కూడు_Wire(NEW_ID, y_కొలత);         \
		కూడు ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		తిరిగి sig_y;                                            \
	}
DEF_పద్ధతి(And,      max(sig_a.కొలత(), sig_b.కొలత()), ID($and))
DEF_పద్ధతి(Or,       max(sig_a.కొలత(), sig_b.కొలత()), ID($or))
DEF_పద్ధతి(Xor,      max(sig_a.కొలత(), sig_b.కొలత()), ID($xor))
DEF_పద్ధతి(Xnor,     max(sig_a.కొలత(), sig_b.కొలత()), ID($xnor))
DEF_పద్ధతి(జరుపు_2,    sig_a.కొలత(), ID($జరుపు))
DEF_పద్ధతి(Lt,       1, ID($lt))
DEF_పద్ధతి(Le,       1, ID($le))
DEF_పద్ధతి(Eq,       1, ID($eq))
DEF_పద్ధతి(Ne,       1, ID($ne))
DEF_పద్ధతి(Eqx,      1, ID($eqx))
DEF_పద్ధతి(Nex,      1, ID($nex))
DEF_పద్ధతి(Ge,       1, ID($ge))
DEF_పద్ధతి(Gt,       1, ID($gt))
DEF_పద్ధతి(కూడు_2,      max(sig_a.కొలత(), sig_b.కొలత()), ID($కూడు))
DEF_పద్ధతి(Sub,      max(sig_a.కొలత(), sig_b.కొలత()), ID($sub))
DEF_పద్ధతి(Mul,      max(sig_a.కొలత(), sig_b.కొలత()), ID($mul))
DEF_పద్ధతి(Div,      max(sig_a.కొలత(), sig_b.కొలత()), ID($div))
DEF_పద్ధతి(Mod,      max(sig_a.కొలత(), sig_b.కొలత()), ID($mod))
DEF_పద్ధతి(DivFloor, max(sig_a.కొలత(), sig_b.కొలత()), ID($divfloor))
DEF_పద్ధతి(ModFloor, max(sig_a.కొలత(), sig_b.కొలత()), ID($modfloor))
DEF_పద్ధతి(LogicAnd, 1, ID($logic_and))
DEF_పద్ధతి(LogicOr,  1, ID($logic_or))
#నిర్వచనం_రద్దు DEF_పద్ధతి

#నిర్వచించు DEF_పద్ధతి(_func, y_కొలత, _type) \
	RTLIL::Cell* RTLIL::Module::కూడు ## _func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, తార్కిక is_signed, స్థిరరాశి std::string &src) { \
		RTLIL::Cell *cell = కూడు_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = అబద్ధం;             \
		cell->parameters[ID::A_వెడల్పు] = sig_a.కొలత();       \
		cell->parameters[ID::B_వెడల్పు] = sig_b.కొలత();       \
		cell->parameters[ID::Y_వెడల్పు] = sig_y.కొలత();       \
		cell->సమితి_పోర్టు(ID::A, sig_a);                        \
		cell->సమితి_పోర్టు(ID::B, sig_b);                        \
		cell->సమితి_పోర్టు(ID::Y, sig_y);                        \
		cell->సమితి_src_attribute(src);                       \
		తిరిగి cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, తార్కిక is_signed, స్థిరరాశి std::string &src) { \
		RTLIL::SigSpec sig_y = కూడు_Wire(NEW_ID, y_కొలత);         \
		కూడు ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		తిరిగి sig_y;                                            \
	}
DEF_పద్ధతి(Shl,      sig_a.కొలత(), ID($shl))
DEF_పద్ధతి(Shr,      sig_a.కొలత(), ID($shr))
DEF_పద్ధతి(Sshl,     sig_a.కొలత(), ID($sshl))
DEF_పద్ధతి(Sshr,     sig_a.కొలత(), ID($sshr))
#నిర్వచనం_రద్దు DEF_పద్ధతి

#నిర్వచించు DEF_పద్ధతి(_func, y_కొలత, _type) \
	RTLIL::Cell* RTLIL::Module::కూడు ## _func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, తార్కిక is_signed, స్థిరరాశి std::string &src) { \
		RTLIL::Cell *cell = కూడు_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = అబద్ధం;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_వెడల్పు] = sig_a.కొలత();       \
		cell->parameters[ID::B_వెడల్పు] = sig_b.కొలత();       \
		cell->parameters[ID::Y_వెడల్పు] = sig_y.కొలత();       \
		cell->సమితి_పోర్టు(ID::A, sig_a);                        \
		cell->సమితి_పోర్టు(ID::B, sig_b);                        \
		cell->సమితి_పోర్టు(ID::Y, sig_y);                        \
		cell->సమితి_src_attribute(src);                       \
		తిరిగి cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, తార్కిక is_signed, స్థిరరాశి std::string &src) { \
		RTLIL::SigSpec sig_y = కూడు_Wire(NEW_ID, y_కొలత);         \
		కూడు ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		తిరిగి sig_y;                                            \
	}
DEF_పద్ధతి(Shiftx,      sig_a.కొలత(), ID($shiftx))
#నిర్వచనం_రద్దు DEF_పద్ధతి

#నిర్వచించు DEF_పద్ధతి(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::కూడు ## _func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src) { \
		RTLIL::Cell *cell = కూడు_Cell(name, _type);                 \
		cell->parameters[ID::వెడల్పు_2] = sig_a.కొలత();               \
		ఒకవేళ (_pmux) cell->parameters[ID::S_వెడల్పు] = sig_s.కొలత();  \
		cell->సమితి_పోర్టు(ID::A, sig_a);                              \
		cell->సమితి_పోర్టు(ID::B, sig_b);                              \
		cell->సమితి_పోర్టు(ID::S, sig_s);                              \
		cell->సమితి_పోర్టు(ID::Y, sig_y);                              \
		cell->సమితి_src_attribute(src);                             \
		తిరిగి cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి std::string &src) { \
		RTLIL::SigSpec sig_y = కూడు_Wire(NEW_ID, sig_a.కొలత());     \
		కూడు ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		తిరిగి sig_y;                                             \
	}
DEF_పద్ధతి(Mux,      ID($mux),        0)
DEF_పద్ధతి(Bwmux,    ID($bwmux),      0)
DEF_పద్ధతి(Pmux,     ID($pmux),       1)
#నిర్వచనం_రద్దు DEF_పద్ధతి

#నిర్వచించు DEF_పద్ధతి(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::కూడు ## _func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src) { \
		RTLIL::Cell *cell = కూడు_Cell(name, _type);                 \
		cell->parameters[ID::వెడల్పు_2] = _demux ? sig_a.కొలత() : sig_y.కొలత(); \
		cell->parameters[ID::S_వెడల్పు] = sig_s.కొలత();             \
		cell->సమితి_పోర్టు(ID::A, sig_a);                              \
		cell->సమితి_పోర్టు(ID::S, sig_s);                              \
		cell->సమితి_పోర్టు(ID::Y, sig_y);                              \
		cell->సమితి_src_attribute(src);                             \
		తిరిగి cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి std::string &src) { \
		RTLIL::SigSpec sig_y = కూడు_Wire(NEW_ID, _demux ? sig_a.కొలత() << sig_s.కొలత() : sig_a.కొలత() >> sig_s.కొలత()); \
		కూడు ## _func(name, sig_a, sig_s, sig_y, src);             \
		తిరిగి sig_y;                                             \
	}
DEF_పద్ధతి(Bmux,     ID($bmux),       0)
DEF_పద్ధతి(Demux,    ID($demux),      1)
#నిర్వచనం_రద్దు DEF_పద్ధతి

#నిర్వచించు DEF_పద్ధతి(_func, _type) \
	RTLIL::Cell* RTLIL::Module::కూడు ## _func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src) { \
		RTLIL::Cell *cell = కూడు_Cell(name, _type);                 \
		cell->parameters[ID::వెడల్పు_2] = sig_a.కొలత();               \
		cell->సమితి_పోర్టు(ID::A, sig_a);                              \
		cell->సమితి_పోర్టు(ID::B, sig_b);                              \
		cell->సమితి_పోర్టు(ID::Y, sig_y);                              \
		cell->సమితి_src_attribute(src);                             \
		తిరిగి cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి std::string &src) { \
		RTLIL::SigSpec sig_y = కూడు_Wire(NEW_ID, sig_a.కొలత());     \
		కూడు ## _func(name, sig_a, sig_s, sig_y, src);             \
		తిరిగి sig_y;                                             \
	}
DEF_పద్ధతి(Bweqx,    ID($bweqx))
#నిర్వచనం_రద్దు DEF_పద్ధతి

#నిర్వచించు DEF_పద్ధతి_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::కూడు ## _func(RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig1, స్థిరరాశి RTLIL::Sig_బిట్టు &sig2, స్థిరరాశి std::string &src) { \
		RTLIL::Cell *cell = కూడు_Cell(name, _type);         \
		cell->సమితి_పోర్టు("\\" #_P1, sig1);                   \
		cell->సమితి_పోర్టు("\\" #_P2, sig2);                   \
		cell->సమితి_src_attribute(src);                     \
		తిరిగి cell;                                      \
	} \
	RTLIL::Sig_బిట్టు RTLIL::Module::_func(RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig1, స్థిరరాశి std::string &src) { \
		RTLIL::Sig_బిట్టు sig2 = కూడు_Wire(NEW_ID);             \
		కూడు ## _func(name, sig1, sig2, src);              \
		తిరిగి sig2;                                      \
	}
#నిర్వచించు DEF_పద్ధతి_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::కూడు ## _func(RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig1, స్థిరరాశి RTLIL::Sig_బిట్టు &sig2, స్థిరరాశి RTLIL::Sig_బిట్టు &sig3, స్థిరరాశి std::string &src) { \
		RTLIL::Cell *cell = కూడు_Cell(name, _type);         \
		cell->సమితి_పోర్టు("\\" #_P1, sig1);                   \
		cell->సమితి_పోర్టు("\\" #_P2, sig2);                   \
		cell->సమితి_పోర్టు("\\" #_P3, sig3);                   \
		cell->సమితి_src_attribute(src);                     \
		తిరిగి cell;                                      \
	} \
	RTLIL::Sig_బిట్టు RTLIL::Module::_func(RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig1, స్థిరరాశి RTLIL::Sig_బిట్టు &sig2, స్థిరరాశి std::string &src) { \
		RTLIL::Sig_బిట్టు sig3 = కూడు_Wire(NEW_ID);             \
		కూడు ## _func(name, sig1, sig2, sig3, src);        \
		తిరిగి sig3;                                      \
	}
#నిర్వచించు DEF_పద్ధతి_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::కూడు ## _func(RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig1, స్థిరరాశి RTLIL::Sig_బిట్టు &sig2, స్థిరరాశి RTLIL::Sig_బిట్టు &sig3, స్థిరరాశి RTLIL::Sig_బిట్టు &sig4, స్థిరరాశి std::string &src) { \
		RTLIL::Cell *cell = కూడు_Cell(name, _type);         \
		cell->సమితి_పోర్టు("\\" #_P1, sig1);                   \
		cell->సమితి_పోర్టు("\\" #_P2, sig2);                   \
		cell->సమితి_పోర్టు("\\" #_P3, sig3);                   \
		cell->సమితి_పోర్టు("\\" #_P4, sig4);                   \
		cell->సమితి_src_attribute(src);                     \
		తిరిగి cell;                                      \
	} \
	RTLIL::Sig_బిట్టు RTLIL::Module::_func(RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig1, స్థిరరాశి RTLIL::Sig_బిట్టు &sig2, స్థిరరాశి RTLIL::Sig_బిట్టు &sig3, స్థిరరాశి std::string &src) { \
		RTLIL::Sig_బిట్టు sig4 = కూడు_Wire(NEW_ID);             \
		కూడు ## _func(name, sig1, sig2, sig3, sig4, src);  \
		తిరిగి sig4;                                      \
	}
#నిర్వచించు DEF_పద్ధతి_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::కూడు ## _func(RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig1, స్థిరరాశి RTLIL::Sig_బిట్టు &sig2, స్థిరరాశి RTLIL::Sig_బిట్టు &sig3, స్థిరరాశి RTLIL::Sig_బిట్టు &sig4, స్థిరరాశి RTLIL::Sig_బిట్టు &sig5, స్థిరరాశి std::string &src) { \
		RTLIL::Cell *cell = కూడు_Cell(name, _type);         \
		cell->సమితి_పోర్టు("\\" #_P1, sig1);                   \
		cell->సమితి_పోర్టు("\\" #_P2, sig2);                   \
		cell->సమితి_పోర్టు("\\" #_P3, sig3);                   \
		cell->సమితి_పోర్టు("\\" #_P4, sig4);                   \
		cell->సమితి_పోర్టు("\\" #_P5, sig5);                   \
		cell->సమితి_src_attribute(src);                     \
		తిరిగి cell;                                      \
	} \
	RTLIL::Sig_బిట్టు RTLIL::Module::_func(RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig1, స్థిరరాశి RTLIL::Sig_బిట్టు &sig2, స్థిరరాశి RTLIL::Sig_బిట్టు &sig3, స్థిరరాశి RTLIL::Sig_బిట్టు &sig4, స్థిరరాశి std::string &src) { \
		RTLIL::Sig_బిట్టు sig5 = కూడు_Wire(NEW_ID);                  \
		కూడు ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		తిరిగి sig5;                                           \
	}
DEF_పద్ధతి_2(BufGate,    ID($_BUF_),    A, Y)
DEF_పద్ధతి_2(NotGate,    ID($_NOT_),    A, Y)
DEF_పద్ధతి_3(AndGate,    ID($_AND_),    A, B, Y)
DEF_పద్ధతి_3(NandGate,   ID($_NAND_),   A, B, Y)
DEF_పద్ధతి_3(OrGate,     ID($_OR_),     A, B, Y)
DEF_పద్ధతి_3(NorGate,    ID($_NOR_),    A, B, Y)
DEF_పద్ధతి_3(XorGate,    ID($_XOR_),    A, B, Y)
DEF_పద్ధతి_3(XnorGate,   ID($_XNOR_),   A, B, Y)
DEF_పద్ధతి_3(AndnotGate, ID($_ANDNOT_), A, B, Y)
DEF_పద్ధతి_3(OrnotGate,  ID($_ORNOT_),  A, B, Y)
DEF_పద్ధతి_4(MuxGate,    ID($_MUX_),    A, B, S, Y)
DEF_పద్ధతి_4(NmuxGate,   ID($_NMUX_),   A, B, S, Y)
DEF_పద్ధతి_4(Aoi3Gate,   ID($_AOI3_),   A, B, C, Y)
DEF_పద్ధతి_4(Oai3Gate,   ID($_OAI3_),   A, B, C, Y)
DEF_పద్ధతి_5(Aoi4Gate,   ID($_AOI4_),   A, B, C, D, Y)
DEF_పద్ధతి_5(Oai4Gate,   ID($_OAI4_),   A, B, C, D, Y)
#నిర్వచనం_రద్దు DEF_పద్ధతి_2
#నిర్వచనం_రద్దు DEF_పద్ధతి_3
#నిర్వచనం_రద్దు DEF_పద్ధతి_4
#నిర్వచనం_రద్దు DEF_పద్ధతి_5

RTLIL::Cell* RTLIL::Module::కూడు_Pow(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, తార్కిక a_signed, తార్కిక b_signed, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($ఘాతం));
	cell->parameters[ID::A_SIGNED] = a_signed;
	cell->parameters[ID::B_SIGNED] = b_signed;
	cell->parameters[ID::A_వెడల్పు] = sig_a.కొలత();
	cell->parameters[ID::B_వెడల్పు] = sig_b.కొలత();
	cell->parameters[ID::Y_వెడల్పు] = sig_y.కొలత();
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::B, sig_b);
	cell->సమితి_పోర్టు(ID::Y, sig_y);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Fa(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_c, స్థిరరాశి RTLIL::SigSpec &sig_x, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($fa));
	cell->parameters[ID::వెడల్పు_2] = sig_a.కొలత();
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::B, sig_b);
	cell->సమితి_పోర్టు(ID::C, sig_c);
	cell->సమితి_పోర్టు(ID::X, sig_x);
	cell->సమితి_పోర్టు(ID::Y, sig_y);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_ముక్క(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, RTLIL::Const స్థానభ్రంశం, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($ముక్క));
	cell->parameters[ID::A_వెడల్పు] = sig_a.కొలత();
	cell->parameters[ID::Y_వెడల్పు] = sig_y.కొలత();
	cell->parameters[ID::స్థానభ్రంశం_2] = స్థానభ్రంశం;
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::Y, sig_y);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Concat(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($concat));
	cell->parameters[ID::A_వెడల్పు] = sig_a.కొలత();
	cell->parameters[ID::B_వెడల్పు] = sig_b.కొలత();
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::B, sig_b);
	cell->సమితి_పోర్టు(ID::Y, sig_y);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Lut(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, RTLIL::Const lut, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::వెడల్పు_2] = sig_a.కొలత();
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::Y, sig_y);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Tribuf(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($tribuf));
	cell->parameters[ID::వెడల్పు_2] = sig_a.కొలత();
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_పోర్టు(ID::Y, sig_y);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Assert(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($ప్రతిజ్ఞా));
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Assume(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($assume));
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_సజీవ(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($సజీవ));
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Fair(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($fair));
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Cover(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($cover));
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Equiv(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($equiv));
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::B, sig_b);
	cell->సమితి_పోర్టు(ID::Y, sig_y);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Sr(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr, స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక సమితి_polarity, తార్కిక clr_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($sr));
	cell->parameters[ID::సమితి_POLARITY] = సమితి_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::సమితి_2, sig_సమితి);
	cell->సమితి_పోర్టు(ID::CLR, sig_clr);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Ff(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($ff));
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Dff(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక clk_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::CLK, sig_clk);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Dffe(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక clk_polarity, తార్కిక en_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($dffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::CLK, sig_clk);
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Dffsr(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక clk_polarity, తార్కిక సమితి_polarity, తార్కిక clr_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($dffsr));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::సమితి_POLARITY] = సమితి_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::CLK, sig_clk);
	cell->సమితి_పోర్టు(ID::సమితి_2, sig_సమితి);
	cell->సమితి_పోర్టు(ID::CLR, sig_clr);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Dffsre(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక clk_polarity, తార్కిక en_polarity, తార్కిక సమితి_polarity, తార్కిక clr_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($dffsre));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::సమితి_POLARITY] = సమితి_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::CLK, sig_clk);
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_పోర్టు(ID::సమితి_2, sig_సమితి);
	cell->సమితి_పోర్టు(ID::CLR, sig_clr);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Adff(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_arst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_విలువ, తార్కిక clk_polarity, తార్కిక arst_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_విలువ] = arst_విలువ;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::CLK, sig_clk);
	cell->సమితి_పోర్టు(ID::ARST, sig_arst);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Adffe(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_arst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_విలువ, తార్కిక clk_polarity, తార్కిక en_polarity, తార్కిక arst_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_విలువ] = arst_విలువ;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::CLK, sig_clk);
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_పోర్టు(ID::ARST, sig_arst);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Aldff(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_aload, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		స్థిరరాశి RTLIL::SigSpec &sig_ad, తార్కిక clk_polarity, తార్కిక aload_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($aldff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::CLK, sig_clk);
	cell->సమితి_పోర్టు(ID::ALOAD, sig_aload);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::AD, sig_ad);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Aldffe(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_aload, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		స్థిరరాశి RTLIL::SigSpec &sig_ad, తార్కిక clk_polarity, తార్కిక en_polarity, తార్కిక aload_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($aldffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::CLK, sig_clk);
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_పోర్టు(ID::ALOAD, sig_aload);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::AD, sig_ad);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Sdff(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_srst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_విలువ, తార్కిక clk_polarity, తార్కిక srst_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_విలువ] = srst_విలువ;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::CLK, sig_clk);
	cell->సమితి_పోర్టు(ID::SRST, sig_srst);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Sdffe(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_srst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_విలువ, తార్కిక clk_polarity, తార్కిక en_polarity, తార్కిక srst_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_విలువ] = srst_విలువ;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::CLK, sig_clk);
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_పోర్టు(ID::SRST, sig_srst);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Sdffce(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_srst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_విలువ, తార్కిక clk_polarity, తార్కిక en_polarity, తార్కిక srst_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_విలువ] = srst_విలువ;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::CLK, sig_clk);
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_పోర్టు(ID::SRST, sig_srst);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Dlatch(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక en_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Adlatch(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_arst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_విలువ, తార్కిక en_polarity, తార్కిక arst_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_విలువ] = arst_విలువ;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_పోర్టు(ID::ARST, sig_arst);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Dlatchsr(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక en_polarity, తార్కిక సమితి_polarity, తార్కిక clr_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($dlatchsr));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::సమితి_POLARITY] = సమితి_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::EN, sig_en);
	cell->సమితి_పోర్టు(ID::సమితి_2, sig_సమితి);
	cell->సమితి_పోర్టు(ID::CLR, sig_clr);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Sr_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr,
		స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక సమితి_polarity, తార్కిక clr_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_SR_%c%c_", సమితి_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->సమితి_పోర్టు(ID::S, sig_సమితి);
	cell->సమితి_పోర్టు(ID::R, sig_clr);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Ff_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($_FF_));
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Dff_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక clk_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->సమితి_పోర్టు(ID::C, sig_clk);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Dffe_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక clk_polarity, తార్కిక en_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->సమితి_పోర్టు(ID::C, sig_clk);
	cell->సమితి_పోర్టు(ID::E, sig_en);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Dffsr_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక clk_polarity, తార్కిక సమితి_polarity, తార్కిక clr_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', సమితి_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->సమితి_పోర్టు(ID::C, sig_clk);
	cell->సమితి_పోర్టు(ID::S, sig_సమితి);
	cell->సమితి_పోర్టు(ID::R, sig_clr);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Dffsre_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక clk_polarity, తార్కిక en_polarity, తార్కిక సమితి_polarity, తార్కిక clr_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', సమితి_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->సమితి_పోర్టు(ID::C, sig_clk);
	cell->సమితి_పోర్టు(ID::S, sig_సమితి);
	cell->సమితి_పోర్టు(ID::R, sig_clr);
	cell->సమితి_పోర్టు(ID::E, sig_en);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Adff_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_arst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		తార్కిక arst_విలువ, తార్కిక clk_polarity, తార్కిక arst_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_విలువ ? '1' : '0'));
	cell->సమితి_పోర్టు(ID::C, sig_clk);
	cell->సమితి_పోర్టు(ID::R, sig_arst);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Adffe_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_arst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		తార్కిక arst_విలువ, తార్కిక clk_polarity, తార్కిక en_polarity, తార్కిక arst_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_విలువ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->సమితి_పోర్టు(ID::C, sig_clk);
	cell->సమితి_పోర్టు(ID::R, sig_arst);
	cell->సమితి_పోర్టు(ID::E, sig_en);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Aldff_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_aload, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		స్థిరరాశి RTLIL::SigSpec &sig_ad, తార్కిక clk_polarity, తార్కిక aload_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->సమితి_పోర్టు(ID::C, sig_clk);
	cell->సమితి_పోర్టు(ID::L, sig_aload);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::AD, sig_ad);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Aldffe_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_aload, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		స్థిరరాశి RTLIL::SigSpec &sig_ad, తార్కిక clk_polarity, తార్కిక en_polarity, తార్కిక aload_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->సమితి_పోర్టు(ID::C, sig_clk);
	cell->సమితి_పోర్టు(ID::L, sig_aload);
	cell->సమితి_పోర్టు(ID::E, sig_en);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::AD, sig_ad);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Sdff_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_srst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		తార్కిక srst_విలువ, తార్కిక clk_polarity, తార్కిక srst_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_విలువ ? '1' : '0'));
	cell->సమితి_పోర్టు(ID::C, sig_clk);
	cell->సమితి_పోర్టు(ID::R, sig_srst);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Sdffe_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_srst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		తార్కిక srst_విలువ, తార్కిక clk_polarity, తార్కిక en_polarity, తార్కిక srst_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_విలువ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->సమితి_పోర్టు(ID::C, sig_clk);
	cell->సమితి_పోర్టు(ID::R, sig_srst);
	cell->సమితి_పోర్టు(ID::E, sig_en);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Sdffce_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_srst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		తార్కిక srst_విలువ, తార్కిక clk_polarity, తార్కిక en_polarity, తార్కిక srst_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_విలువ ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->సమితి_పోర్టు(ID::C, sig_clk);
	cell->సమితి_పోర్టు(ID::R, sig_srst);
	cell->సమితి_పోర్టు(ID::E, sig_en);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Dlatch_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక en_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->సమితి_పోర్టు(ID::E, sig_en);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Adlatch_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_arst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
		తార్కిక arst_విలువ, తార్కిక en_polarity, తార్కిక arst_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_విలువ ? '1' : '0'));
	cell->సమితి_పోర్టు(ID::E, sig_en);
	cell->సమితి_పోర్టు(ID::R, sig_arst);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Dlatchsr_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, తార్కిక en_polarity, తార్కిక సమితి_polarity, తార్కిక clr_polarity, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', సమితి_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->సమితి_పోర్టు(ID::E, sig_en);
	cell->సమితి_పోర్టు(ID::S, sig_సమితి);
	cell->సమితి_పోర్టు(ID::R, sig_clr);
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::Cell* RTLIL::Module::కూడు_Anyinit(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($anyinit));
	cell->parameters[ID::వెడల్పు_2] = sig_q.కొలత();
	cell->సమితి_పోర్టు(ID::D, sig_d);
	cell->సమితి_పోర్టు(ID::Q, sig_q);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, పూర్ణాంకం వెడల్పు, స్థిరరాశి std::string &src)
{
	RTLIL::SigSpec sig = కూడు_Wire(NEW_ID, వెడల్పు);
	Cell *cell = కూడు_Cell(name, ID($anyconst));
	cell->సమితి_Param(ID::వెడల్పు_2, వెడల్పు);
	cell->సమితి_పోర్టు(ID::Y, sig);
	cell->సమితి_src_attribute(src);
	తిరిగి sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, పూర్ణాంకం వెడల్పు, స్థిరరాశి std::string &src)
{
	RTLIL::SigSpec sig = కూడు_Wire(NEW_ID, వెడల్పు);
	Cell *cell = కూడు_Cell(name, ID($anyseq));
	cell->సమితి_Param(ID::వెడల్పు_2, వెడల్పు);
	cell->సమితి_పోర్టు(ID::Y, sig);
	cell->సమితి_src_attribute(src);
	తిరిగి sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, పూర్ణాంకం వెడల్పు, స్థిరరాశి std::string &src)
{
	RTLIL::SigSpec sig = కూడు_Wire(NEW_ID, వెడల్పు);
	Cell *cell = కూడు_Cell(name, ID($allconst));
	cell->సమితి_Param(ID::వెడల్పు_2, వెడల్పు);
	cell->సమితి_పోర్టు(ID::Y, sig);
	cell->సమితి_src_attribute(src);
	తిరిగి sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, పూర్ణాంకం వెడల్పు, స్థిరరాశి std::string &src)
{
	RTLIL::SigSpec sig = కూడు_Wire(NEW_ID, వెడల్పు);
	Cell *cell = కూడు_Cell(name, ID($allseq));
	cell->సమితి_Param(ID::వెడల్పు_2, వెడల్పు);
	cell->సమితి_పోర్టు(ID::Y, sig);
	cell->సమితి_src_attribute(src);
	తిరిగి sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, స్థిరరాశి std::string &src)
{
	RTLIL::SigSpec sig = కూడు_Wire(NEW_ID);
	Cell *cell = కూడు_Cell(name, ID($initstate));
	cell->సమితి_పోర్టు(ID::Y, sig);
	cell->సమితి_src_attribute(src);
	తిరిగి sig;
}

RTLIL::SigSpec RTLIL::Module::సమితి_ట్యాగ్_2(RTLIL::IdString name, స్థిరరాశి std::string &ట్యాగ్, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి RTLIL::SigSpec &sig_c, స్థిరరాశి std::string &src)
{
	RTLIL::SigSpec sig = కూడు_Wire(NEW_ID, sig_a.కొలత());
	Cell *cell = కూడు_Cell(name, ID($సమితి_ట్యాగ్));
	cell->parameters[ID::వెడల్పు_2] = sig_a.కొలత();
	cell->parameters[ID::ట్యాగ్_2] = ట్యాగ్;
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::సమితి_2, sig_s);
	cell->సమితి_పోర్టు(ID::CLR, sig_c);
	cell->సమితి_పోర్టు(ID::Y, sig);
	cell->సమితి_src_attribute(src);
	తిరిగి sig;
}

RTLIL::Cell* RTLIL::Module::కూడు_సమితి_ట్యాగ్(RTLIL::IdString name, స్థిరరాశి std::string &ట్యాగ్, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి RTLIL::SigSpec &sig_c, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src)
{
	Cell *cell = కూడు_Cell(name, ID($సమితి_ట్యాగ్));
	cell->parameters[ID::వెడల్పు_2] = sig_a.కొలత();
	cell->parameters[ID::ట్యాగ్_2] = ట్యాగ్;
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::సమితి_2, sig_s);
	cell->సమితి_పోర్టు(ID::CLR, sig_c);
	cell->సమితి_పోర్టు(ID::Y, sig_y);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::SigSpec RTLIL::Module::Get_ట్యాగ్(RTLIL::IdString name, స్థిరరాశి std::string &ట్యాగ్, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి std::string &src)
{
	RTLIL::SigSpec sig = కూడు_Wire(NEW_ID, sig_a.కొలత());
	Cell *cell = కూడు_Cell(name, ID($get_ట్యాగ్));
	cell->parameters[ID::వెడల్పు_2] = sig_a.కొలత();
	cell->parameters[ID::ట్యాగ్_2] = ట్యాగ్;
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::Y, sig);
	cell->సమితి_src_attribute(src);
	తిరిగి sig;
}

RTLIL::Cell* RTLIL::Module::కూడు_Overwrite_ట్యాగ్(RTLIL::IdString name, స్థిరరాశి std::string &ట్యాగ్, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి RTLIL::SigSpec &sig_c, స్థిరరాశి std::string &src)
{
	RTLIL::Cell *cell = కూడు_Cell(name, ID($overwrite_ట్యాగ్));
	cell->parameters[ID::వెడల్పు_2] = sig_a.కొలత();
	cell->parameters[ID::ట్యాగ్_2] = ట్యాగ్;
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::సమితి_2, sig_s);
	cell->సమితి_పోర్టు(ID::CLR, sig_c);
	cell->సమితి_src_attribute(src);
	తిరిగి cell;
}

RTLIL::SigSpec RTLIL::Module::Original_ట్యాగ్(RTLIL::IdString name, స్థిరరాశి std::string &ట్యాగ్, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి std::string &src)
{
	RTLIL::SigSpec sig = కూడు_Wire(NEW_ID, sig_a.కొలత());
	Cell *cell = కూడు_Cell(name, ID($original_ట్యాగ్));
	cell->parameters[ID::వెడల్పు_2] = sig_a.కొలత();
	cell->parameters[ID::ట్యాగ్_2] = ట్యాగ్;
	cell->సమితి_పోర్టు(ID::A, sig_a);
	cell->సమితి_పోర్టు(ID::Y, sig);
	cell->సమితి_src_attribute(src);
	తిరిగి sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_e, స్థిరరాశి std::string &src)
{
	RTLIL::SigSpec sig = కూడు_Wire(NEW_ID, sig_e.కొలత());
	Cell *cell = కూడు_Cell(name, ID($future_ff));
	cell->parameters[ID::వెడల్పు_2] = sig_e.కొలత();
	cell->సమితి_పోర్టు(ID::A, sig_e);
	cell->సమితి_పోర్టు(ID::Y, sig);
	cell->సమితి_src_attribute(src);
	తిరిగి sig;
}

std::string RTLIL::Module::గమ్యం_rtlil_str() స్థిరరాశి
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(ఇది), design, అబద్ధం, నిజం, అబద్ధం, RTLIL_BACKEND::Dump_బహుళకం::Readable);
	తిరిగి f.str();
}

RTLIL::Wire::Wire()
{
	స్థిర గుర్తులేని పూర్ణాంకం hashidx_లెక్క = 123456789;
	hashidx_లెక్క = mkhash_xorshift(hashidx_లెక్క);
	hashidx_ = hashidx_లెక్క;

	module = శూన్యసూచిక;
	వెడల్పు = 1;
	ప్రారంభం_స్థానభ్రంశం = 0;
	పోర్టు_id = 0;
	పోర్టు_ఆదానం = అబద్ధం;
	పోర్టు_output = అబద్ధం;
	upto = అబద్ధం;
	is_signed = అబద్ధం;

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->చొప్పించు(std::pair<గుర్తులేని పూర్ణాంకం, RTLIL::Wire*>(hashidx_, ఇది));
#అయితే_ముగింపు
}

RTLIL::Wire::~Wire()
{
#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#అయితే_ముగింపు
}

std::string RTLIL::Wire::గమ్యం_rtlil_str() స్థిరరాశి
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", ఇది, design(), RTLIL_BACKEND::Dump_బహుళకం::Readable);
	తిరిగి f.str();
}

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
స్థిర std::map<గుర్తులేని పూర్ణాంకం, RTLIL::Wire*> all_wires;
std::map<గుర్తులేని పూర్ణాంకం, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(శూన్య)
{
	తిరిగి &all_wires;
}
#అయితే_ముగింపు

RTLIL::జ్ఞాపకం::జ్ఞాపకం()
{
	స్థిర గుర్తులేని పూర్ణాంకం hashidx_లెక్క = 123456789;
	hashidx_లెక్క = mkhash_xorshift(hashidx_లెక్క);
	hashidx_ = hashidx_లెక్క;

	వెడల్పు = 1;
	ప్రారంభం_స్థానభ్రంశం = 0;
	కొలత = 0;
#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	RTLIL::జ్ఞాపకం::get_all_memorys()->చొప్పించు(std::pair<గుర్తులేని పూర్ణాంకం, RTLIL::జ్ఞాపకం*>(hashidx_, ఇది));
#అయితే_ముగింపు
}

std::string RTLIL::జ్ఞాపకం::గమ్యం_rtlil_str() స్థిరరాశి
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_జ్ఞాపకం(f, "", ఇది, design(), RTLIL_BACKEND::Dump_బహుళకం::Readable);
	తిరిగి f.str();
}

RTLIL::Process::Process() : module(శూన్యసూచిక)
{
	స్థిర గుర్తులేని పూర్ణాంకం hashidx_లెక్క = 123456789;
	hashidx_లెక్క = mkhash_xorshift(hashidx_లెక్క);
	hashidx_ = hashidx_లెక్క;
}

std::string RTLIL::Process::గమ్యం_rtlil_str() స్థిరరాశి
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", ఇది, design(), RTLIL_BACKEND::Dump_బహుళకం::Readable);
	తిరిగి f.str();
}

RTLIL::Cell::Cell() : module(శూన్యసూచిక)
{
	స్థిర గుర్తులేని పూర్ణాంకం hashidx_లెక్క = 123456789;
	hashidx_లెక్క = mkhash_xorshift(hashidx_లెక్క);
	hashidx_ = hashidx_లెక్క;

	// log("#memtrace# %p\n", this);
	memhasher();

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->చొప్పించు(std::pair<గుర్తులేని పూర్ణాంకం, RTLIL::Cell*>(hashidx_, ఇది));
#అయితే_ముగింపు
}

RTLIL::Cell::~Cell()
{
#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#అయితే_ముగింపు
}

std::string RTLIL::Cell::గమ్యం_rtlil_str() స్థిరరాశి
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", ఇది, design(), RTLIL_BACKEND::Dump_బహుళకం::Readable);
	తిరిగి f.str();
}

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
స్థిర std::map<గుర్తులేని పూర్ణాంకం, RTLIL::Cell*> all_cells;
std::map<గుర్తులేని పూర్ణాంకం, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(శూన్య)
{
	తిరిగి &all_cells;
}
#అయితే_ముగింపు

తార్కిక RTLIL::Cell::has_పోర్టు(RTLIL::IdString portname) స్థిరరాశి
{
	తిరిగి connections_.లెక్క(portname) != 0;
}

// bufnorm

స్థిరరాశి RTLIL::SigSpec &RTLIL::Cell::get_పోర్టు(RTLIL::IdString portname) స్థిరరాశి
{
	తిరిగి connections_.at(portname);
}

స్థిరరాశి dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() స్థిరరాశి
{
	తిరిగి connections_;
}

తార్కిక RTLIL::Cell::తెలిసిన() స్థిరరాశి
{
	ఒకవేళ (yosys_celltypes.cell_తెలిసిన(type))
		తిరిగి నిజం;
	ఒకవేళ (module && module->design && module->design->module(type))
		తిరిగి నిజం;
	తిరిగి అబద్ధం;
}

తార్కిక RTLIL::Cell::ఆదానం(IdString portname) స్థిరరాశి
{
	ఒకవేళ (yosys_celltypes.cell_తెలిసిన(type))
		తిరిగి yosys_celltypes.cell_ఆదానం(type.ref(), portname);
	ఒకవేళ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : శూన్యసూచిక;
		తిరిగి w && w->పోర్టు_ఆదానం;
	}
	తిరిగి అబద్ధం;
}

తార్కిక RTLIL::Cell::output(RTLIL::IdString portname) స్థిరరాశి
{
	ఒకవేళ (yosys_celltypes.cell_తెలిసిన(type))
		తిరిగి yosys_celltypes.cell_output(type, portname);
	ఒకవేళ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : శూన్యసూచిక;
		తిరిగి w && w->పోర్టు_output;
	}
	తిరిగి అబద్ధం;
}

RTLIL::పోర్టు_Dir RTLIL::Cell::పోర్టు_dir(RTLIL::IdString portname) స్థిరరాశి
{
	ఒకవేళ (yosys_celltypes.cell_తెలిసిన(type.ref()))
		తిరిగి yosys_celltypes.cell_పోర్టు_dir(type, portname);
	ఒకవేళ (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		ఒకవేళ (m == శూన్యసూచిక)
			తిరిగి పోర్టు_Dir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		ఒకవేళ (w == శూన్యసూచిక)
			తిరిగి పోర్టు_Dir::PD_UNKNOWN;
		తిరిగి పోర్టు_Dir(w->పోర్టు_ఆదానం + w->పోర్టు_output * 2);
	}
	తిరిగి పోర్టు_Dir::PD_UNKNOWN;
}

తార్కిక RTLIL::Cell::hasParam(RTLIL::IdString paramname) స్థిరరాశి
{
	తిరిగి parameters.లెక్క(paramname) != 0;
}

శూన్య RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

శూన్య RTLIL::Cell::సమితి_Param(RTLIL::IdString paramname, RTLIL::Const విలువ)
{
	parameters[paramname] = std::move(విలువ);
}

స్థిరరాశి RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) స్థిరరాశి
{
	స్థిరరాశి స్వయంచాలక &it = parameters.కనుగొను(paramname);
	ఒకవేళ (it != parameters.చివర())
		తిరిగి it->రెండవ;
	ఒకవేళ (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		ఒకవేళ (m)
			తిరిగి m->parameter_default_విలువలు.at(paramname);
	}
	విసురు std::బహిర్గతం_యొక్క_range("Cell::getParam()");
}

శూన్య RTLIL::Cell::క్రమీకరించు()
{
	connections_.క్రమీకరించు(క్రమీకరించు_by_id_str(module->twines()));
	parameters.క్రమీకరించు(క్రమీకరించు_by_id_str(module->twines()));
	లక్షణాలు.క్రమీకరించు(క్రమీకరించు_by_id_str(module->twines()));
}

శూన్య RTLIL::Cell::check()
{
#నిర్వచితం_కాకపోతే NDEBUG
	అంతర్గత_Cell_Checker checker(ఖాళీ, ఇది);
	checker.check();
#అయితే_ముగింపు
}

శూన్య RTLIL::Cell::fixup_parameters(తార్కిక సమితి_a_signed, తార్కిక సమితి_b_signed)
{
	ఒకవేళ (type.is_ప్రజా() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		తిరిగి;

	ఒకవేళ (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::వెడల్పు_2] = Get_కొలత(connections_[ID::Y]);
		ఒకవేళ (type.లోపల(ID($pmux), ID($bmux)))
			parameters[ID::S_వెడల్పు] = Get_కొలత(connections_[ID::S]);
		check();
		తిరిగి;
	}

	ఒకవేళ (type == ID($demux)) {
		parameters[ID::వెడల్పు_2] = Get_కొలత(connections_[ID::A]);
		parameters[ID::S_వెడల్పు] = Get_కొలత(connections_[ID::S]);
		check();
		తిరిగి;
	}

	ఒకవేళ (type == ID($lut) || type == ID($sop)) {
		parameters[ID::వెడల్పు_2] = Get_కొలత(connections_[ID::A]);
		తిరిగి;
	}

	ఒకవేళ (type == ID($fa)) {
		parameters[ID::వెడల్పు_2] = Get_కొలత(connections_[ID::Y]);
		తిరిగి;
	}

	ఒకవేళ (type == ID($lcu)) {
		parameters[ID::వెడల్పు_2] = Get_కొలత(connections_[ID::CO]);
		తిరిగి;
	}

	ఒకవేళ (type == ID($macc_v2)) {
		parameters[ID::Y_వెడల్పు] = Get_కొలత(connections_[ID::Y]);
		తిరిగి;
	}

	తార్కిక signedness_ab = !type.లోపల(ID($ముక్క), ID($concat), ID($macc));

	ఒకవేళ (connections_.లెక్క(ID::A)) {
		ఒకవేళ (signedness_ab) {
			ఒకవేళ (సమితి_a_signed)
				parameters[ID::A_SIGNED] = నిజం;
			లేకపోతే ఒకవేళ (parameters.లెక్క(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = అబద్ధం;
		}
		parameters[ID::A_వెడల్పు] = Get_కొలత(connections_[ID::A]);
	}

	ఒకవేళ (connections_.లెక్క(ID::B)) {
		ఒకవేళ (signedness_ab) {
			ఒకవేళ (సమితి_b_signed)
				parameters[ID::B_SIGNED] = నిజం;
			లేకపోతే ఒకవేళ (parameters.లెక్క(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = అబద్ధం;
		}
		parameters[ID::B_వెడల్పు] = Get_కొలత(connections_[ID::B]);
	}

	ఒకవేళ (connections_.లెక్క(ID::Y) && type != ID($concat))
		parameters[ID::Y_వెడల్పు] = Get_కొలత(connections_[ID::Y]);

	ఒకవేళ (connections_.లెక్క(ID::Q))
		parameters[ID::వెడల్పు_2] = Get_కొలత(connections_[ID::Q]);

	check();
}

తార్కిక RTLIL::Cell::has_keep_attr() స్థిరరాశి {
	తిరిగి get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

తార్కిక RTLIL::Cell::has_memid() స్థిరరాశి
{
	తిరిగి type.లోపల(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

తార్కిక RTLIL::Cell::is_mem_cell() స్థిరరాశి
{
	తిరిగి type.లోపల(ID($mem), ID($mem_v2)) || has_memid();
}

తార్కిక RTLIL::Cell::is_builtin_ff() స్థిరరాశి {
	తిరిగి StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు)
{
	wire = బిట్టు.wire;
	స్థానభ్రంశం = 0;
	ఒకవేళ (wire == ఖాళీ)
		దత్తాంశం = {బిట్టు.దత్తాంశం};
	లేకపోతే
		స్థానభ్రంశం = బిట్టు.స్థానభ్రంశం;
	వెడల్పు = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::వెలికితీయి(పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం నిడివి) స్థిరరాశి
{
	log_assert(స్థానభ్రంశం >= 0);
	log_assert(నిడివి >= 0);
	log_assert(స్థానభ్రంశం + నిడివి <= వెడల్పు);
	RTLIL::SigChunk ret;
	ఒకవేళ (wire) {
		ret.wire = wire;
		ret.స్థానభ్రంశం = ఇది->స్థానభ్రంశం + స్థానభ్రంశం;
		ret.వెడల్పు = నిడివి;
	} లేకపోతే {
		కొరకు (పూర్ణాంకం i = 0; i < నిడివి; i++)
			ret.దత్తాంశం.నెట్టు_వెనుక(దత్తాంశం[స్థానభ్రంశం+i]);
		ret.వెడల్పు = నిడివి;
	}
	తిరిగి ret;
}

RTLIL::Sig_బిట్టు RTLIL::SigChunk::క్రియాకారకం[](పూర్ణాంకం స్థానభ్రంశం) స్థిరరాశి
{
	log_assert(స్థానభ్రంశం >= 0);
	log_assert(స్థానభ్రంశం < వెడల్పు);
	RTLIL::Sig_బిట్టు ret;
	ఒకవేళ (wire) {
		ret.wire = wire;
		ret.స్థానభ్రంశం = ఇది->స్థానభ్రంశం + స్థానభ్రంశం;
	} లేకపోతే {
		ret.దత్తాంశం = దత్తాంశం[స్థానభ్రంశం];
	}
	తిరిగి ret;
}

తార్కిక RTLIL::SigChunk::క్రియాకారకం <(స్థిరరాశి RTLIL::SigChunk &other) స్థిరరాశి
{
	ఒకవేళ (wire && other.wire)
		ఒకవేళ (wire->name != other.wire->name)
			తిరిగి wire->name < other.wire->name;

	ఒకవేళ (wire != other.wire)
		తిరిగి wire < other.wire;

	ఒకవేళ (స్థానభ్రంశం != other.స్థానభ్రంశం)
		తిరిగి స్థానభ్రంశం < other.స్థానభ్రంశం;

	ఒకవేళ (వెడల్పు != other.వెడల్పు)
		తిరిగి వెడల్పు < other.వెడల్పు;

	తిరిగి దత్తాంశం < other.దత్తాంశం;
}

తార్కిక RTLIL::SigChunk::క్రియాకారకం ==(స్థిరరాశి RTLIL::SigChunk &other) స్థిరరాశి
{
	తిరిగి wire == other.wire && వెడల్పు == other.వెడల్పు && స్థానభ్రంశం == other.స్థానభ్రంశం && దత్తాంశం == other.దత్తాంశం;
}

తార్కిక RTLIL::SigChunk::క్రియాకారకం !=(స్థిరరాశి RTLIL::SigChunk &other) స్థిరరాశి
{
	ఒకవేళ (*ఇది == other)
		తిరిగి అబద్ధం;
	తిరిగి నిజం;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_ఖాళీలు_బిట్లు();
	log_assert(parts.కొలత() > 0);
	స్వయంచాలక ie = parts.begin();
	స్వయంచాలక it = ie + parts.కొలత() - 1;
	అయితే (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(స్థిరరాశి RTLIL::Const &విలువ)
{
	ఒకవేళ (Get_కొలత(విలువ) != 0) {
		rep_ = CHUNK;
		కొత్త (&chunk_) RTLIL::SigChunk(విలువ);
	} లేకపోతే {
		init_ఖాళీలు_బిట్లు();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&విలువ)
{
	ఒకవేళ (Get_కొలత(విలువ) != 0) {
		rep_ = CHUNK;
		కొత్త (&chunk_) RTLIL::SigChunk(విలువ);
	} లేకపోతే {
		init_ఖాళీలు_బిట్లు();
	}
	check();
}

RTLIL::SigSpec::SigSpec(స్థిరరాశి RTLIL::SigChunk &chunk)
{
	ఒకవేళ (chunk.వెడల్పు != 0) {
		rep_ = CHUNK;
		కొత్త (&chunk_) RTLIL::SigChunk(chunk);
	} లేకపోతే {
		init_ఖాళీలు_బిట్లు();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	ఒకవేళ (chunk.వెడల్పు != 0) {
		rep_ = CHUNK;
		కొత్త (&chunk_) RTLIL::SigChunk(chunk);
	} లేకపోతే {
		init_ఖాళీలు_బిట్లు();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	ఒకవేళ (wire->వెడల్పు != 0) {
		rep_ = CHUNK;
		కొత్త (&chunk_) RTLIL::SigChunk(wire);
	} లేకపోతే {
		init_ఖాళీలు_బిట్లు();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం వెడల్పు)
{
	ఒకవేళ (వెడల్పు != 0) {
		rep_ = CHUNK;
		కొత్త (&chunk_) RTLIL::SigChunk(wire, స్థానభ్రంశం, వెడల్పు);
	} లేకపోతే {
		init_ఖాళీలు_బిట్లు();
	}
	check();
}

RTLIL::SigSpec::SigSpec(స్థిరరాశి std::string &str)
{
	ఒకవేళ (str.కొలత() != 0) {
		rep_ = CHUNK;
		కొత్త (&chunk_) RTLIL::SigChunk(str);
	} లేకపోతే {
		init_ఖాళీలు_బిట్లు();
	}
	check();
}

RTLIL::SigSpec::SigSpec(పూర్ణాంకం val, పూర్ణాంకం వెడల్పు)
{
	ఒకవేళ (వెడల్పు != 0) {
		rep_ = CHUNK;
		కొత్త (&chunk_) RTLIL::SigChunk(val, వెడల్పు);
	} లేకపోతే
		init_ఖాళీలు_బిట్లు();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State బిట్టు, పూర్ణాంకం వెడల్పు)
{
	ఒకవేళ (వెడల్పు != 0) {
		rep_ = CHUNK;
		కొత్త (&chunk_) RTLIL::SigChunk(బిట్టు, వెడల్పు);
	} లేకపోతే
		init_ఖాళీలు_బిట్లు();
	check();
}

RTLIL::SigSpec::SigSpec(స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు, పూర్ణాంకం వెడల్పు)
{
	ఒకవేళ (వెడల్పు != 0) {
		ఒకవేళ (బిట్టు.wire == ఖాళీ) {
			rep_ = CHUNK;
			కొత్త (&chunk_) RTLIL::SigChunk(బిట్టు.దత్తాంశం, వెడల్పు);
		} లేకపోతే ఒకవేళ (వెడల్పు == 1) {
			rep_ = CHUNK;
			కొత్త (&chunk_) RTLIL::SigChunk(బిట్టు);
		} లేకపోతే {
			init_ఖాళీలు_బిట్లు();
			బిట్లు_2.reserve(వెడల్పు);
			కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++)
				బిట్లు_2.నెట్టు_వెనుక(బిట్టు);
		}
	} లేకపోతే
		init_ఖాళీలు_బిట్లు();
	check();
}

RTLIL::SigSpec::SigSpec(స్థిరరాశి std::సదిశ<RTLIL::SigChunk> &chunks)
{
	init_ఖాళీలు_బిట్లు();
	కొరకు (స్థిరరాశి స్వయంచాలక &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(స్థిరరాశి std::సదిశ<RTLIL::Sig_బిట్టు> &బిట్లు)
{
	init_ఖాళీలు_బిట్లు();
	కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : బిట్లు)
		append(బిట్టు);
	check();
}

RTLIL::SigSpec::SigSpec(స్థిరరాశి pool<RTLIL::Sig_బిట్టు> &బిట్లు)
{
	init_ఖాళీలు_బిట్లు();
	కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : బిట్లు)
		append(బిట్టు);
	check();
}

RTLIL::SigSpec::SigSpec(స్థిరరాశి std::సమితి<RTLIL::Sig_బిట్టు> &బిట్లు)
{
	init_ఖాళీలు_బిట్లు();
	కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : బిట్లు)
		append(బిట్టు);
	check();
}

RTLIL::SigSpec::SigSpec(తార్కిక బిట్టు)
{
	rep_ = CHUNK;
	కొత్త (&chunk_) RTLIL::SigChunk(బిట్టు ? RTLIL::S1 : RTLIL::S0);
	check();
}

శూన్య RTLIL::SigSpec::Chunks::const_iterator::తదుపరి_chunk_బిట్లు()
{
	పూర్ణాంకం బిట్లు_కొలత = Get_కొలత(spec.బిట్లు_2);
	ఒకవేళ (బిట్టు_సూచీ >= బిట్లు_కొలత)
		తిరిగి;
	పూర్ణాంకం i = బిట్టు_సూచీ;
	స్థిరరాశి Sig_బిట్టు &బిట్టు = spec.బిట్లు_2[i++];
	chunk.wire = బిట్టు.wire;
	chunk.దత్తాంశం.clear();
	ఒకవేళ (బిట్టు.is_wire()) {
		chunk.స్థానభ్రంశం = బిట్టు.స్థానభ్రంశం;
		అయితే (i < బిట్లు_కొలత && spec.బిట్లు_2[i].wire == బిట్టు.wire &&
			spec.బిట్లు_2[i].స్థానభ్రంశం == బిట్టు.స్థానభ్రంశం + i - బిట్టు_సూచీ)
			++i;
	} లేకపోతే {
		chunk.స్థానభ్రంశం = 0;
		chunk.దత్తాంశం.నెట్టు_వెనుక(బిట్టు.దత్తాంశం);
		అయితే (i < బిట్లు_కొలత && !spec.బిట్లు_2[i].is_wire()) {
			chunk.దత్తాంశం.నెట్టు_వెనుక(spec.బిట్లు_2[i].దత్తాంశం);
			++i;
		}
	}
	chunk.వెడల్పు = i - బిట్టు_సూచీ;
}

శూన్య RTLIL::SigSpec::unpack()
{
	ఒకవేళ (rep_ == బిట్లు_3)
		తిరిగి;

	std::సదిశ<RTLIL::Sig_బిట్టు> బిట్లు;
	బిట్లు.reserve(chunk_.వెడల్పు);
	కొరకు (పూర్ణాంకం i = 0; i < chunk_.వెడల్పు; i++)
		బిట్లు.emplace_వెనుక(chunk_, i);

	chunk_.~SigChunk();
	rep_ = బిట్లు_3;
	కొత్త (&బిట్లు_2) std::సదిశ<RTLIL::Sig_బిట్టు>(std::move(బిట్లు));
}

శూన్య RTLIL::SigSpec::try_repack()
{
	ఒకవేళ (rep_ != బిట్లు_3)
		తిరిగి;

	పూర్ణాంకం బిట్లు_కొలత = Get_కొలత(బిట్లు_2);
	ఒకవేళ (బిట్లు_కొలత == 0)
		తిరిగి;
	ఒకవేళ (బిట్లు_2[0].is_wire()) {
		కొరకు (పూర్ణాంకం i = 1; i < బిట్లు_కొలత; i++)
			ఒకవేళ (బిట్లు_2[0].wire != బిట్లు_2[i].wire || బిట్లు_2[0].స్థానభ్రంశం + i != బిట్లు_2[i].స్థానభ్రంశం)
				తిరిగి;
		SigChunk chunk(బిట్లు_2[0].wire, బిట్లు_2[0].స్థానభ్రంశం, బిట్లు_కొలత);
		బిట్లు_2.~సదిశ();
		rep_ = CHUNK;
		కొత్త (&chunk_) SigChunk(std::move(chunk));
		తిరిగి;
	}
	std::సదిశ<RTLIL::State> బిట్లు;
	బిట్లు.reserve(బిట్లు_కొలత);
	బిట్లు.నెట్టు_వెనుక(బిట్లు_2[0].దత్తాంశం);
	కొరకు (పూర్ణాంకం i = 1; i < బిట్లు_కొలత; i++) {
		ఒకవేళ (బిట్లు_2[i].is_wire())
			తిరిగి;
		బిట్లు.నెట్టు_వెనుక(బిట్లు_2[i].దత్తాంశం);
	}
	బిట్లు_2.~సదిశ();
	rep_ = CHUNK;
	కొత్త (&chunk_) SigChunk(std::move(బిట్లు));
}

Hasher::hash_t RTLIL::SigSpec::updhash() స్థిరరాశి
{
	Hasher h;
	కొరకు (స్వయంచాలక &c : chunks())
		ఒకవేళ (c.wire == ఖాళీ) {
			కొరకు (స్వయంచాలక &v : c.దత్తాంశం)
				h.eat(v);
		} లేకపోతే {
			h.eat(c.wire->name.ref());
			h.eat(c.స్థానభ్రంశం);
			h.eat(c.వెడల్పు);
		}
	Hasher::hash_t ఫలితం = h.yield();
	ఒకవేళ (ఫలితం == 0)
		ఫలితం = 1;
	hash_.సమితి(ఫలితం);
	తిరిగి ఫలితం;
}

శూన్య RTLIL::SigSpec::క్రమీకరించు()
{
	unpack();
	std::క్రమీకరించు(బిట్లు_2.begin(), బిట్లు_2.చివర());
	hash_.clear();
	try_repack();
}

శూన్య RTLIL::SigSpec::క్రమీకరించు_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::సదిశ<Sig_బిట్టు> unique_బిట్లు = బిట్లు_2;
	std::క్రమీకరించు(unique_బిట్లు.begin(), unique_బిట్లు.చివర());
	స్వయంచాలక చివరి = std::unique(unique_బిట్లు.begin(), unique_బిట్లు.చివర());
	unique_బిట్లు.erase(చివరి, unique_బిట్లు.చివర());

	*ఇది = unique_బిట్లు;
	hash_.clear();
	try_repack();
}

శూన్య RTLIL::SigSpec::replace(స్థిరరాశి RTLIL::SigSpec &నమూనా, స్థిరరాశి RTLIL::SigSpec &with)
{
	replace(నమూనా, with, ఇది);
}

శూన్య RTLIL::SigSpec::replace(స్థిరరాశి RTLIL::SigSpec &నమూనా, స్థిరరాశి RTLIL::SigSpec &with, RTLIL::SigSpec *other) స్థిరరాశి
{
	log_assert(other != ఖాళీ);
	log_assert(కొలత() == other->కొలత());
	log_assert(నమూనా.కొలత() == with.కొలత());

	dict<RTLIL::Sig_బిట్టు, పూర్ణాంకం> నమూనా_గమ్యం_with;
	పూర్ణాంకం నమూనా_కొలత = నమూనా.కొలత();
	కొరకు (పూర్ణాంకం i = 0; i < నమూనా_కొలత; i++) {
		Sig_బిట్టు నమూనా_బిట్టు = నమూనా[i];
		ఒకవేళ (నమూనా_బిట్టు.wire != ఖాళీ) {
			నమూనా_గమ్యం_with.emplace(నమూనా_బిట్టు, i);
		}
	}

	పూర్ణాంకం this_కొలత = కొలత();
	తార్కిక other_modified = అబద్ధం;
	కొరకు (పూర్ణాంకం j = 0; j < this_కొలత; j++) {
		స్వయంచాలక it = నమూనా_గమ్యం_with.కనుగొను((*ఇది)[j]);
		ఒకవేళ (it != నమూనా_గమ్యం_with.చివర()) {
			other_modified = నిజం;
			(*other)[j] = with[it->రెండవ];
		}
	}

	ఒకవేళ (other_modified)
		other->try_repack();
	other->check();
}

శూన్య RTLIL::SigSpec::replace(స్థిరరాశి dict<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> &నియమాలు)
{
	replace(నియమాలు, ఇది);
}

శూన్య RTLIL::SigSpec::replace(స్థిరరాశి dict<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> &నియమాలు, RTLIL::SigSpec *other) స్థిరరాశి
{
	log_assert(other != ఖాళీ);
	log_assert(కొలత() == other->కొలత());

	ఒకవేళ (నియమాలు.ఖాళీలు()) తిరిగి;

	పూర్ణాంకం this_కొలత = కొలత();
	తార్కిక other_modified = అబద్ధం;
	కొరకు (పూర్ణాంకం i = 0; i < this_కొలత; i++) {
		స్వయంచాలక it = నియమాలు.కనుగొను((*ఇది)[i]);
		ఒకవేళ (it != నియమాలు.చివర()) {
			other_modified = నిజం;
			(*other)[i] = it->రెండవ;
		}
	}

	ఒకవేళ (other_modified)
		other->try_repack();
	other->check();
}

శూన్య RTLIL::SigSpec::replace(స్థిరరాశి std::map<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> &నియమాలు)
{
	replace(నియమాలు, ఇది);
}

శూన్య RTLIL::SigSpec::replace(స్థిరరాశి std::map<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> &నియమాలు, RTLIL::SigSpec *other) స్థిరరాశి
{
	log_assert(other != ఖాళీ);
	log_assert(కొలత() == other->కొలత());

	ఒకవేళ (నియమాలు.ఖాళీలు()) తిరిగి;

	పూర్ణాంకం this_కొలత = కొలత();
	తార్కిక other_modified = అబద్ధం;
	కొరకు (పూర్ణాంకం i = 0; i < this_కొలత; i++) {
		స్వయంచాలక it = నియమాలు.కనుగొను((*ఇది)[i]);
		ఒకవేళ (it != నియమాలు.చివర()) {
			other_modified = నిజం;
			(*other)[i] = it->రెండవ;
		}
	}

	ఒకవేళ (other_modified)
		other->try_repack();
	other->check();
}

శూన్య RTLIL::SigSpec::remove(స్థిరరాశి RTLIL::SigSpec &నమూనా)
{
	remove2(నమూనా, ఖాళీ);
}

శూన్య RTLIL::SigSpec::remove(స్థిరరాశి RTLIL::SigSpec &నమూనా, RTLIL::SigSpec *other) స్థిరరాశి
{
	RTLIL::SigSpec tmp = *ఇది;
	tmp.remove2(నమూనా, other);
}

శూన్య RTLIL::SigSpec::remove2(స్థిరరాశి RTLIL::SigSpec &నమూనా, RTLIL::SigSpec *other)
{
	unpack();
	ఒకవేళ (other != ఖాళీ) {
		log_assert(కొలత() == other->కొలత());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<Sig_బిట్టు> నమూనా_బిట్లు;
	నమూనా_బిట్లు.reserve(నమూనా.కొలత());
	కొరకు (స్వయంచాలక &బిట్టు : నమూనా)
		ఒకవేళ (బిట్టు.wire != ఖాళీ)
			నమూనా_బిట్లు.చొప్పించు(బిట్టు);

	// Compact in-place to avoid O(n^2) erase operations
	కొలత_t write_idx = 0;
	కొరకు (కొలత_t read_idx = 0; read_idx < బిట్లు_2.కొలత(); read_idx++)
	{
		ఒకవేళ (!(బిట్లు_2[read_idx].wire != ఖాళీ && నమూనా_బిట్లు.లెక్క(బిట్లు_2[read_idx]))) {
			ఒకవేళ (write_idx != read_idx) {
				బిట్లు_2[write_idx] = బిట్లు_2[read_idx];
				ఒకవేళ (other != ఖాళీ)
					other->బిట్లు_2[write_idx] = other->బిట్లు_2[read_idx];
			}
			write_idx++;
		}
	}

	తార్కిక modified = (write_idx < బిట్లు_2.కొలత());
	ఒకవేళ (modified) {
		బిట్లు_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ఒకవేళ (other != ఖాళీ && modified) {
		other->బిట్లు_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

శూన్య RTLIL::SigSpec::remove(స్థిరరాశి pool<RTLIL::Sig_బిట్టు> &నమూనా)
{
	remove2(నమూనా, ఖాళీ);
}

శూన్య RTLIL::SigSpec::remove(స్థిరరాశి pool<RTLIL::Sig_బిట్టు> &నమూనా, RTLIL::SigSpec *other) స్థిరరాశి
{
	RTLIL::SigSpec tmp = *ఇది;
	tmp.remove2(నమూనా, other);
}

శూన్య RTLIL::SigSpec::remove2(స్థిరరాశి pool<RTLIL::Sig_బిట్టు> &నమూనా, RTLIL::SigSpec *other)
{
	unpack();

	ఒకవేళ (other != ఖాళీ) {
		log_assert(కొలత() == other->కొలత());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	కొలత_t write_idx = 0;
	కొరకు (కొలత_t read_idx = 0; read_idx < బిట్లు_2.కొలత(); read_idx++) {
		ఒకవేళ (!(బిట్లు_2[read_idx].wire != ఖాళీ && నమూనా.లెక్క(బిట్లు_2[read_idx]))) {
			ఒకవేళ (write_idx != read_idx) {
				బిట్లు_2[write_idx] = బిట్లు_2[read_idx];
				ఒకవేళ (other != ఖాళీ)
					other->బిట్లు_2[write_idx] = other->బిట్లు_2[read_idx];
			}
			write_idx++;
		}
	}

	తార్కిక modified = (write_idx < బిట్లు_2.కొలత());
	ఒకవేళ (modified) {
		బిట్లు_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ఒకవేళ (other != ఖాళీ && modified) {
		other->బిట్లు_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

శూన్య RTLIL::SigSpec::remove2(స్థిరరాశి std::సమితి<RTLIL::Sig_బిట్టు> &నమూనా, RTLIL::SigSpec *other)
{
	unpack();

	ఒకవేళ (other != ఖాళీ) {
		log_assert(కొలత() == other->కొలత());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	కొలత_t write_idx = 0;
	కొరకు (కొలత_t read_idx = 0; read_idx < బిట్లు_2.కొలత(); read_idx++) {
		ఒకవేళ (!(బిట్లు_2[read_idx].wire != ఖాళీ && నమూనా.లెక్క(బిట్లు_2[read_idx]))) {
			ఒకవేళ (write_idx != read_idx) {
				బిట్లు_2[write_idx] = బిట్లు_2[read_idx];
				ఒకవేళ (other != ఖాళీ)
					other->బిట్లు_2[write_idx] = other->బిట్లు_2[read_idx];
			}
			write_idx++;
		}
	}

	తార్కిక modified = (write_idx < బిట్లు_2.కొలత());
	ఒకవేళ (modified) {
		బిట్లు_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	ఒకవేళ (other != ఖాళీ && modified) {
		other->బిట్లు_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

శూన్య RTLIL::SigSpec::remove2(స్థిరరాశి pool<RTLIL::Wire*> &నమూనా, RTLIL::SigSpec *other)
{
	unpack();

	ఒకవేళ (other != ఖాళీ) {
		log_assert(కొలత() == other->కొలత());
		other->unpack();
	}

	తార్కిక modified = అబద్ధం;
	తార్కిక other_modified = అబద్ధం;
	కొరకు (పూర్ణాంకం i = Get_కొలత(బిట్లు_2) - 1; i >= 0; i--) {
		ఒకవేళ (బిట్లు_2[i].wire != ఖాళీ && నమూనా.లెక్క(బిట్లు_2[i].wire)) {
			modified = నిజం;
			బిట్లు_2.erase(బిట్లు_2.begin() + i);
			ఒకవేళ (other != ఖాళీ) {
				other_modified = నిజం;
				other->బిట్లు_2.erase(other->బిట్లు_2.begin() + i);
			}
		}
	}

	ఒకవేళ (modified) {
		hash_.clear();
		try_repack();
	}
	ఒకవేళ (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::వెలికితీయి(స్థిరరాశి RTLIL::SigSpec &నమూనా, స్థిరరాశి RTLIL::SigSpec *other) స్థిరరాశి
{
	log_assert(other == ఖాళీ || కొలత() == other->కొలత());

	RTLIL::SigSpec ret;
	std::సదిశ<RTLIL::Sig_బిట్టు> బిట్లు_సరిపోలిక = గమ్యం_sigbit_సదిశ();

	కొరకు (స్వయంచాలక& నమూనా_chunk : నమూనా.chunks()) {
		ఒకవేళ (other) {
			std::సదిశ<RTLIL::Sig_బిట్టు> బిట్లు_other = other->గమ్యం_sigbit_సదిశ();
			పూర్ణాంకం i = 0;
			కొరకు (స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు : బిట్లు_సరిపోలిక) {
				ఒకవేళ (బిట్టు.wire &&
					బిట్టు.wire == నమూనా_chunk.wire &&
					బిట్టు.స్థానభ్రంశం >= నమూనా_chunk.స్థానభ్రంశం &&
					బిట్టు.స్థానభ్రంశం < నమూనా_chunk.స్థానభ్రంశం + నమూనా_chunk.వెడల్పు)
					ret.append(బిట్లు_other[i]);
				++i;
			}
		} లేకపోతే {
			కొరకు (స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు : బిట్లు_సరిపోలిక)
				ఒకవేళ (బిట్టు.wire &&
					బిట్టు.wire == నమూనా_chunk.wire &&
					బిట్టు.స్థానభ్రంశం >= నమూనా_chunk.స్థానభ్రంశం &&
					బిట్టు.స్థానభ్రంశం < నమూనా_chunk.స్థానభ్రంశం + నమూనా_chunk.వెడల్పు)
					ret.append(బిట్టు);
		}
	}

	ret.check();
	తిరిగి ret;
}

RTLIL::SigSpec RTLIL::SigSpec::వెలికితీయి(స్థిరరాశి pool<RTLIL::Sig_బిట్టు> &నమూనా, స్థిరరాశి RTLIL::SigSpec *other) స్థిరరాశి
{
	log_assert(other == ఖాళీ || కొలత() == other->కొలత());

	std::సదిశ<RTLIL::Sig_బిట్టు> బిట్లు_సరిపోలిక = గమ్యం_sigbit_సదిశ();
	RTLIL::SigSpec ret;

	ఒకవేళ (other) {
		std::సదిశ<RTLIL::Sig_బిట్టు> బిట్లు_other = other->గమ్యం_sigbit_సదిశ();
		పూర్ణాంకం i = 0;
		కొరకు (స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు : బిట్లు_సరిపోలిక) {
			ఒకవేళ (బిట్టు.wire && నమూనా.లెక్క(బిట్టు))
				ret.append(బిట్లు_other[i]);
			++i;
		}
	} లేకపోతే {
		కొరకు (స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు : బిట్లు_సరిపోలిక)
			ఒకవేళ (బిట్టు.wire && నమూనా.లెక్క(బిట్టు))
				ret.append(బిట్టు);
	}

	ret.check();
	తిరిగి ret;
}

శూన్య RTLIL::SigSpec::replace(పూర్ణాంకం స్థానభ్రంశం, స్థిరరాశి RTLIL::SigSpec &with)
{
	ఒకవేళ (with.కొలత() == 0)
		తిరిగి;

	unpack();

	log_assert(స్థానభ్రంశం >= 0);
	log_assert(with.కొలత() >= 0);
	log_assert(స్థానభ్రంశం+with.కొలత() <= కొలత());

	పూర్ణాంకం i = 0;
	కొరకు (స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు : with.బిట్లు()) {
		బిట్లు_2.at(స్థానభ్రంశం + i) = బిట్టు;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

శూన్య RTLIL::SigSpec::remove_const()
{
	ఒకవేళ (rep_ == CHUNK)
	{
		ఒకవేళ (chunk_.wire == ఖాళీ) {
			chunk_.~SigChunk();
			init_ఖాళీలు_బిట్లు();
			hash_.clear();
		}
	}
	లేకపోతే
	{
		std::సదిశ<RTLIL::Sig_బిట్టు> new_బిట్లు;
		new_బిట్లు.reserve(బిట్లు_2.కొలత());
		కొరకు (స్వయంచాలక &బిట్టు : బిట్లు_2)
			ఒకవేళ (బిట్టు.wire != ఖాళీ)
				new_బిట్లు.నెట్టు_వెనుక(బిట్టు);
		ఒకవేళ (Get_కొలత(new_బిట్లు) != Get_కొలత(బిట్లు_2)) {
			బిట్లు_2.మార్పిడి(new_బిట్లు);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

శూన్య RTLIL::SigSpec::remove(పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం నిడివి)
{
	ఒకవేళ (నిడివి == 0)
		తిరిగి;

	unpack();

	log_assert(స్థానభ్రంశం >= 0);
	log_assert(నిడివి >= 0);
	log_assert(స్థానభ్రంశం + నిడివి <= కొలత());

	బిట్లు_2.erase(బిట్లు_2.begin() + స్థానభ్రంశం, బిట్లు_2.begin() + స్థానభ్రంశం + నిడివి);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::వెలికితీయి(పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం నిడివి) స్థిరరాశి
{
	log_assert(స్థానభ్రంశం >= 0);
	log_assert(నిడివి >= 0);
	log_assert(స్థానభ్రంశం + నిడివి <= కొలత());

	std::సదిశ<Sig_బిట్టు> extracted;
	Sig_బిట్టు మొదటి;
	తార్కిక is_packing = నిజం;
	కొరకు (పూర్ణాంకం i = స్థానభ్రంశం; i < స్థానభ్రంశం + నిడివి; i++) {
		తార్కిక was_packing_before = is_packing;
		Sig_బిట్టు బిట్టు = (*ఇది)[i];
		ఒకవేళ (i == స్థానభ్రంశం) {
			మొదటి = బిట్టు;
			ఒకవేళ (!బిట్టు.wire)
				is_packing = అబద్ధం;
		} లేకపోతే {
			ఒకవేళ (బిట్టు.wire != మొదటి.wire)
				is_packing = అబద్ధం;
			ఒకవేళ (బిట్టు.wire)
				ఒకవేళ (బిట్టు.స్థానభ్రంశం != మొదటి.స్థానభ్రంశం + (i - స్థానభ్రంశం))
					is_packing = అబద్ధం;
		}
		ఒకవేళ (was_packing_before && !is_packing)
			కొరకు (పూర్ణాంకం j = స్థానభ్రంశం; j < i; j++)
				extracted.నెట్టు_వెనుక((*ఇది)[j]);
		ఒకవేళ (!is_packing)
			extracted.నెట్టు_వెనుక((*ఇది)[i]);
	}
	ఒకవేళ (is_packing)
		తిరిగి SigChunk(మొదటి.wire, మొదటి.స్థానభ్రంశం, నిడివి);

	తిరిగి extracted;
}

శూన్య RTLIL::SigSpec::rewrite_wires(std::function<శూన్య(RTLIL::Wire*& wire)> rewrite)
{
	ఒకవేళ (rep_ == CHUNK) {
		ఒకవేళ (chunk_.wire != శూన్యసూచిక)
			rewrite(chunk_.wire);
		తిరిగి;
	}

	std::సదిశ<RTLIL::Sig_బిట్టు> new_బిట్లు;
	కొరకు (స్థిరరాశి RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		ఒకవేళ (c.wire != శూన్యసూచిక)
			rewrite(c.wire);
		కొరకు (పూర్ణాంకం i = 0; i < c.వెడల్పు; i++)
			new_బిట్లు.emplace_వెనుక(c, i);
	}
	బిట్లు_2 = std::move(new_బిట్లు);
	hash_.clear();
}

శూన్య RTLIL::SigSpec::append(స్థిరరాశి RTLIL::SigSpec &సంకేతం)
{
	ఒకవేళ (సంకేతం.కొలత() == 0)
		తిరిగి;

	ఒకవేళ (కొలత() == 0) {
		*ఇది = సంకేతం;
		తిరిగి;
	}

	hash_.clear();
	ఒకవేళ (rep_ == CHUNK && సంకేతం.rep_ == CHUNK && chunk_.wire == సంకేతం.chunk_.wire) {
		ఒకవేళ (chunk_.wire == ఖాళీ) {
			chunk_.దత్తాంశం.చొప్పించు(chunk_.దత్తాంశం.చివర(), సంకేతం.chunk_.దత్తాంశం.begin(), సంకేతం.chunk_.దత్తాంశం.చివర());
			chunk_.వెడల్పు = Get_కొలత(chunk_.దత్తాంశం);
			తిరిగి;
		}
		ఒకవేళ (chunk_.స్థానభ్రంశం + chunk_.వెడల్పు == సంకేతం.chunk_.స్థానభ్రంశం) {
			chunk_.వెడల్పు += సంకేతం.chunk_.వెడల్పు;
			తిరిగి;
		}
	}

	unpack();
	కొరకు (స్థిరరాశి Sig_బిట్టు &బిట్టు : సంకేతం.బిట్లు())
		బిట్లు_2.నెట్టు_వెనుక(బిట్టు);
	check();
}

శూన్య RTLIL::SigSpec::append(స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు)
{
	hash_.clear();

	ఒకవేళ (కొలత() == 0) {
		destroy();
		rep_ = CHUNK;
		కొత్త (&chunk_) RTLIL::SigChunk(బిట్టు);
		తిరిగి;
	}

	ఒకవేళ (rep_ == CHUNK && chunk_.wire == బిట్టు.wire) {
		ఒకవేళ (chunk_.wire == ఖాళీ) {
			chunk_.దత్తాంశం.నెట్టు_వెనుక(బిట్టు.దత్తాంశం);
			chunk_.వెడల్పు++;
			తిరిగి;
		}
		ఒకవేళ (chunk_.స్థానభ్రంశం + chunk_.వెడల్పు == బిట్టు.స్థానభ్రంశం) {
			chunk_.వెడల్పు++;
			తిరిగి;
		}
	}

	unpack();

	బిట్లు_2.నెట్టు_వెనుక(బిట్టు);
	check();
}

శూన్య RTLIL::SigSpec::extend_u0(పూర్ణాంకం వెడల్పు, తార్కిక is_signed)
{
	ఒకవేళ (కొలత() > వెడల్పు)
		remove(వెడల్పు, కొలత() - వెడల్పు);

	ఒకవేళ (కొలత() < వెడల్పు) {
		RTLIL::Sig_బిట్టు padding = కొలత() > 0 ? (*ఇది)[కొలత() - 1] : RTLIL::State::Sx;
		ఒకవేళ (!is_signed)
			padding = RTLIL::State::S0;
		అయితే (కొలత() < వెడల్పు)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(పూర్ణాంకం అంకె) స్థిరరాశి
{
	RTLIL::SigSpec sig;
	కొరకు (పూర్ణాంకం i = 0; i < అంకె; i++)
		sig.append(*ఇది);
	తిరిగి sig;
}

#నిర్వచితం_కాకపోతే NDEBUG
శూన్య RTLIL::SigSpec::check(స్థిరరాశి Module *mod) స్థిరరాశి
{
	ఒకవేళ (rep_ == CHUNK)
	{
		log_assert(chunk_.వెడల్పు != 0);
		ఒకవేళ (chunk_.wire == ఖాళీ) {
			log_assert(chunk_.స్థానభ్రంశం == 0);
			log_assert(chunk_.దత్తాంశం.కొలత() == (కొలత_t)chunk_.వెడల్పు);
		} లేకపోతే {
			log_assert(chunk_.స్థానభ్రంశం >= 0);
			log_assert(chunk_.వెడల్పు >= 0);
			log_assert(chunk_.స్థానభ్రంశం + chunk_.వెడల్పు <= chunk_.wire->వెడల్పు);
			log_assert(chunk_.దత్తాంశం.కొలత() == 0);
			ఒకవేళ (mod != శూన్యసూచిక)
				log_assert(chunk_.wire->module == mod);
		}
	}
	లేకపోతే ఒకవేళ (కొలత() <= 64)
	{
		ఒకవేళ (mod != శూన్యసూచిక) {
			కొరకు (స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు : బిట్లు_2)
				ఒకవేళ (బిట్టు.wire != శూన్యసూచిక)
					log_assert(బిట్టు.wire->module == mod);
		}
	}
}
#అయితే_ముగింపు

తార్కిక RTLIL::SigSpec::క్రియాకారకం <(స్థిరరాశి RTLIL::SigSpec &other) స్థిరరాశి
{
	ఒకవేళ (ఇది == &other)
		తిరిగి అబద్ధం;

	ఒకవేళ (కొలత() != other.కొలత())
		తిరిగి కొలత() < other.కొలత();

	స్వయంచాలక other_it = other.chunks().begin();
	కొరకు (స్థిరరాశి SigChunk &c : chunks()) {
		ఒకవేళ (c != *other_it)
			తిరిగి c < *other_it;
		++other_it;
	}

	తిరిగి అబద్ధం;
}

తార్కిక RTLIL::SigSpec::క్రియాకారకం ==(స్థిరరాశి RTLIL::SigSpec &other) స్థిరరాశి
{
	ఒకవేళ (ఇది == &other)
		తిరిగి నిజం;

	ఒకవేళ (కొలత() != other.కొలత())
		తిరిగి అబద్ధం;

	స్వయంచాలక other_it = other.chunks().begin();
	కొరకు (స్థిరరాశి SigChunk &c : chunks()) {
		ఒకవేళ (c != *other_it)
			తిరిగి అబద్ధం;
		++other_it;
	}

	తిరిగి నిజం;
}

తార్కిక RTLIL::SigSpec::is_wire() స్థిరరాశి
{
	Chunks cs = chunks();
	స్వయంచాలక it = cs.begin();
	ఒకవేళ (it == cs.చివర())
		తిరిగి అబద్ధం;
	స్థిరరాశి RTLIL::SigChunk &chunk = *it;
	తిరిగి chunk.wire && chunk.wire->వెడల్పు == కొలత() && ++it == cs.చివర();
}

తార్కిక RTLIL::SigSpec::is_chunk() స్థిరరాశి
{
	Chunks cs = chunks();
	స్వయంచాలక it = cs.begin();
	ఒకవేళ (it == cs.చివర())
		తిరిగి అబద్ధం;
	తిరిగి ++it == cs.చివర();
}

తార్కిక RTLIL::SigSpec::తెలిసిన_driver() స్థిరరాశి
{
	కొరకు (స్వయంచాలక &chunk : chunks())
		ఒకవేళ (chunk.is_wire() && !chunk.wire->తెలిసిన_driver())
			తిరిగి అబద్ధం;
	తిరిగి నిజం;
}

తార్కిక RTLIL::SigSpec::is_fully_const() స్థిరరాశి
{
	కొరకు (స్వయంచాలక &chunk : chunks())
		ఒకవేళ (chunk.వెడల్పు > 0 && chunk.wire != ఖాళీ)
			తిరిగి అబద్ధం;
	తిరిగి నిజం;
}

తార్కిక RTLIL::SigSpec::is_fully_zero() స్థిరరాశి
{
	కొరకు (స్వయంచాలక &chunk : chunks()) {
		ఒకవేళ (chunk.వెడల్పు > 0 && chunk.wire != ఖాళీ)
			తిరిగి అబద్ధం;
		కొరకు (RTLIL::State d : chunk.దత్తాంశం)
			ఒకవేళ (d != RTLIL::State::S0)
				తిరిగి అబద్ధం;
	}
	తిరిగి నిజం;
}

తార్కిక RTLIL::SigSpec::is_fully_ones() స్థిరరాశి
{
	కొరకు (స్వయంచాలక &chunk : chunks()) {
		ఒకవేళ (chunk.వెడల్పు > 0 && chunk.wire != ఖాళీ)
			తిరిగి అబద్ధం;
		కొరకు (RTLIL::State d : chunk.దత్తాంశం)
			ఒకవేళ (d != RTLIL::State::S1)
				తిరిగి అబద్ధం;
	}
	తిరిగి నిజం;
}

తార్కిక RTLIL::SigSpec::is_fully_def() స్థిరరాశి
{
	కొరకు (స్వయంచాలక &chunk : chunks()) {
		ఒకవేళ (chunk.వెడల్పు > 0 && chunk.wire != ఖాళీ)
			తిరిగి అబద్ధం;
		కొరకు (RTLIL::State d : chunk.దత్తాంశం)
			ఒకవేళ (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				తిరిగి అబద్ధం;
	}
	తిరిగి నిజం;
}

తార్కిక RTLIL::SigSpec::is_fully_undef() స్థిరరాశి
{
	కొరకు (స్వయంచాలక &chunk : chunks()) {
		ఒకవేళ (chunk.వెడల్పు > 0 && chunk.wire != ఖాళీ)
			తిరిగి అబద్ధం;
		కొరకు (RTLIL::State d : chunk.దత్తాంశం)
			ఒకవేళ (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				తిరిగి అబద్ధం;
	}
	తిరిగి నిజం;
}

తార్కిక RTLIL::SigSpec::has_const() స్థిరరాశి
{
	కొరకు (స్వయంచాలక &chunk : chunks())
		ఒకవేళ (chunk.వెడల్పు > 0 && chunk.wire == ఖాళీ)
			తిరిగి నిజం;
	తిరిగి అబద్ధం;
}

తార్కిక RTLIL::SigSpec::has_const(State state) స్థిరరాశి
{
	కొరకు (స్వయంచాలక &chunk : chunks())
		ఒకవేళ (chunk.వెడల్పు > 0 && chunk.wire == ఖాళీ && std::కనుగొను(chunk.దత్తాంశం.begin(), chunk.దత్తాంశం.చివర(), state) != chunk.దత్తాంశం.చివర())
			తిరిగి నిజం;
	తిరిగి అబద్ధం;
}


తార్కిక RTLIL::SigSpec::has_marked_బిట్లు() స్థిరరాశి
{
	కొరకు (స్వయంచాలక &chunk : chunks())
		ఒకవేళ (chunk.వెడల్పు > 0 && chunk.wire == ఖాళీ) {
			కొరకు (RTLIL::State d : chunk.దత్తాంశం)
				ఒకవేళ (d == RTLIL::State::Sm)
					తిరిగి నిజం;
		}
	తిరిగి అబద్ధం;
}

తార్కిక RTLIL::SigSpec::is_onehot(పూర్ణాంకం *pos) స్థిరరాశి
{
	ఒకవేళ (std::optional<RTLIL::Const> c = try_as_const())
		తిరిగి c->is_onehot(pos);
	తిరిగి అబద్ధం;
}

తార్కిక RTLIL::SigSpec::as_bool() స్థిరరాశి
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_విలువ());
	తిరిగి c->as_bool();
}

పూర్ణాంకం RTLIL::SigSpec::as_int(తార్కిక is_signed) స్థిరరాశి
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_విలువ());
	తిరిగి c->as_int(is_signed);
}

తార్కిక RTLIL::SigSpec::convertible_గమ్యం_int(తార్కిక is_signed) స్థిరరాశి
{
	std::optional<RTLIL::Const> c = try_as_const();
	ఒకవేళ (!c.has_విలువ())
		తిరిగి అబద్ధం;
	తిరిగి c->convertible_గమ్యం_int(is_signed);
}

std::optional<పూర్ణాంకం> RTLIL::SigSpec::try_as_int(తార్కిక is_signed) స్థిరరాశి
{
	std::optional<RTLIL::Const> c = try_as_const();
	ఒకవేళ (!c.has_విలువ())
		తిరిగి std::nullopt;
	తిరిగి c->try_as_int(is_signed);
}

పూర్ణాంకం RTLIL::SigSpec::as_int_saturating(తార్కిక is_signed) స్థిరరాశి
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_విలువ());
	తిరిగి c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() స్థిరరాశి
{
	std::string str;
	str.reserve(కొలత());
	std::సదిశ<RTLIL::SigChunk> chunks = *ఇది;
	కొరకు (కొలత_t i = chunks.కొలత(); i > 0; i--) {
		స్థిరరాశి RTLIL::SigChunk &chunk = chunks[i-1];
		ఒకవేళ (chunk.wire != ఖాళీ)
			str.append(chunk.వెడల్పు, '?');
		లేకపోతే
			str += RTLIL::Const(chunk.దత్తాంశం).as_string();
	}
	తిరిగి str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() స్థిరరాశి
{
	Chunks cs = chunks();
	స్వయంచాలక it = cs.begin();
	ఒకవేళ (it == cs.చివర())
		తిరిగి RTLIL::Const();
	SigChunk chunk = *it;
	ఒకవేళ (chunk.wire != ఖాళీ || ++it != cs.చివర())
		తిరిగి std::nullopt;
	తిరిగి RTLIL::Const(std::move(chunk.దత్తాంశం));
}

RTLIL::Const RTLIL::SigSpec::as_const() స్థిరరాశి
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_విలువ());
	తిరిగి *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() స్థిరరాశి
{
	Chunks cs = chunks();
	స్వయంచాలక it = cs.begin();
	log_assert(it != cs.చివర());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.చివర() && chunk.wire && chunk.wire->వెడల్పు == కొలత());
	తిరిగి chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() స్థిరరాశి
{
	Chunks cs = chunks();
	స్వయంచాలక it = cs.begin();
	log_assert(it != cs.చివర());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.చివర());
	తిరిగి chunk;
}

RTLIL::Sig_బిట్టు RTLIL::SigSpec::as_బిట్టు() స్థిరరాశి
{
	తిరిగి RTLIL::Sig_బిట్టు(*ఇది);
}

తార్కిక RTLIL::SigSpec::సరిపోలిక(స్థిరరాశి అక్షరం* నమూనా) స్థిరరాశి
{
	పూర్ణాంకం నమూనా_len = అక్షరశ్రేణి_పొడవు(నమూనా);
	log_assert(నమూనా_len == కొలత());

	కొరకు (పూర్ణాంకం i = 0; i < నమూనా_len; ++i) {
		అక్షరం ch = నమూనా[i];
		ఒకవేళ (ch == ' ')
			కొనసాగు;
		RTLIL::Sig_బిట్టు బిట్టు = (*ఇది)[నమూనా_len - 1 - i];
		ఒకవేళ (ch == '*') {
			ఒకవేళ (బిట్టు != State::Sz && బిట్టు != State::Sx)
				తిరిగి అబద్ధం;
			కొనసాగు;
		}
		ఒకవేళ (ch == '0') {
			ఒకవేళ (బిట్టు != State::S0)
				తిరిగి అబద్ధం;
		} లేకపోతే
		ఒకవేళ (ch == '1') {
			ఒకవేళ (బిట్టు != State::S1)
				తిరిగి అబద్ధం;
		} లేకపోతే
			log_abort();
	}

	తిరిగి నిజం;
}

std::సమితి<RTLIL::Sig_బిట్టు> RTLIL::SigSpec::గమ్యం_sigbit_సమితి() స్థిరరాశి
{
	std::సమితి<RTLIL::Sig_బిట్టు> sigbits;
	కొరకు (స్వయంచాలక &c : chunks())
		కొరకు (పూర్ణాంకం i = 0; i < c.వెడల్పు; i++)
			sigbits.చొప్పించు(RTLIL::Sig_బిట్టు(c, i));
	తిరిగి sigbits;
}

pool<RTLIL::Sig_బిట్టు> RTLIL::SigSpec::గమ్యం_sigbit_pool() స్థిరరాశి
{
	pool<RTLIL::Sig_బిట్టు> sigbits;
	sigbits.reserve(కొలత());
	కొరకు (స్వయంచాలక &c : chunks())
		కొరకు (పూర్ణాంకం i = 0; i < c.వెడల్పు; i++)
			sigbits.చొప్పించు(RTLIL::Sig_బిట్టు(c, i));
	తిరిగి sigbits;
}

std::సదిశ<RTLIL::Sig_బిట్టు> RTLIL::SigSpec::గమ్యం_sigbit_సదిశ() స్థిరరాశి
{
	std::సదిశ<RTLIL::Sig_బిట్టు> ఫలితం;
	ఫలితం.reserve(కొలత());
	కొరకు (Sig_బిట్టు బిట్టు : *ఇది)
		ఫలితం.నెట్టు_వెనుక(బిట్టు);
	తిరిగి ఫలితం;
}

std::map<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> RTLIL::SigSpec::గమ్యం_sigbit_map(స్థిరరాశి RTLIL::SigSpec &other) స్థిరరాశి
{
	పూర్ణాంకం this_కొలత = కొలత();
	log_assert(this_కొలత == other.కొలత());

	std::map<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> new_map;
	కొరకు (పూర్ణాంకం i = 0; i < this_కొలత; i++)
		new_map[(*ఇది)[i]] = other[i];

	తిరిగి new_map;
}

dict<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> RTLIL::SigSpec::గమ్యం_sigbit_dict(స్థిరరాశి RTLIL::SigSpec &other) స్థిరరాశి
{
	పూర్ణాంకం this_కొలత = కొలత();
	log_assert(this_కొలత == other.కొలత());

	dict<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> new_map;
	new_map.reserve(this_కొలత);
	కొరకు (పూర్ణాంకం i = 0; i < this_కొలత; i++)
		new_map[(*ఇది)[i]] = other[i];

	తిరిగి new_map;
}

స్థిర శూన్య sigspec_parse_విభజించు(std::సదిశ<std::string> &టోకెన్లు, స్థిరరాశి std::string &పాఠ్యం, అక్షరం sep)
{
	కొలత_t ప్రారంభం = 0, చివర = 0;
	అయితే ((చివర = పాఠ్యం.కనుగొను(sep, ప్రారంభం)) != std::string::npos) {
		టోకెన్లు.నెట్టు_వెనుక(పాఠ్యం.substr(ప్రారంభం, చివర - ప్రారంభం));
		ప్రారంభం = చివర + 1;
	}
	టోకెన్లు.నెట్టు_వెనుక(పాఠ్యం.substr(ప్రారంభం));
}

స్థిర RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, స్థిరరాశి std::string &netname)
{
	తిరిగి module->wire(netname);
}

తార్కిక RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::సదిశ<std::string> టోకెన్లు;
	sigspec_parse_విభజించు(టోకెన్లు, str, ',');

	sig = RTLIL::SigSpec();
	కొరకు (పూర్ణాంకం tokidx = పూర్ణాంకం(టోకెన్లు.కొలత())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = టోకెన్లు[tokidx];
		std::string indices;

		ఒకవేళ (netname.కొలత() == 0)
			కొనసాగు;

		ఒకవేళ (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			స్వయంచాలక ast = p.const2ast(netname);
			ఒకవేళ (ast == శూన్యసూచిక)
				తిరిగి అబద్ధం;
			sig.append(RTLIL::Const(ast->బిట్లు));
			కొనసాగు;
		}

		ఒకవేళ (module == ఖాళీ)
			తిరిగి అబద్ధం;

		ఒకవేళ (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		ఒకవేళ (sigspec_parse_wire(module, netname) == శూన్యసూచిక) {
			కొలత_t indices_pos = netname.కొలత()-1;
			ఒకవేళ (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				అయితే (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				ఒకవేళ (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					అయితే (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				ఒకవేళ (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		ఒకవేళ (wire == శూన్యసూచిక)
			తిరిగి అబద్ధం;

		ఒకవేళ (!indices.ఖాళీలు()) {
			std::సదిశ<std::string> సూచీ_టోకెన్లు;
			sigspec_parse_విభజించు(సూచీ_టోకెన్లు, indices.substr(1, indices.కొలత()-2), ':');
			ఒకవేళ (సూచీ_టోకెన్లు.కొలత() == 1) {
				పూర్ణాంకం a = atoi(సూచీ_టోకెన్లు.at(0).c_str());
				ఒకవేళ (a < 0 || a >= wire->వెడల్పు)
					తిరిగి అబద్ధం;
				sig.append(RTLIL::SigSpec(wire, a));
			} లేకపోతే {
				పూర్ణాంకం a = atoi(సూచీ_టోకెన్లు.at(0).c_str());
				పూర్ణాంకం b = atoi(సూచీ_టోకెన్లు.at(1).c_str());
				ఒకవేళ (a > b) {
					పూర్ణాంకం tmp = a;
					a = b, b = tmp;
				}
				ఒకవేళ (a < 0 || a >= wire->వెడల్పు)
					తిరిగి అబద్ధం;
				ఒకవేళ (b < 0 || b >= wire->వెడల్పు)
					తిరిగి అబద్ధం;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} లేకపోతే
			sig.append(wire);
	}

	తిరిగి నిజం;
}

తార్కిక RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	ఒకవేళ (str.ఖాళీలు() || str[0] != '@')
		తిరిగి parse(sig, module, str);

	IdString sel_name = design->twines().కనుగొను(RTLIL::escape_id(str.substr(1)));
	ఒకవేళ (sel_name == IdString::Null || design->selection_vars.లెక్క(sel_name) == 0)
		తిరిగి అబద్ధం;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	కొరకు (స్వయంచాలక &it : module->wires_)
		ఒకవేళ (sel.selected_member(module->name, it.రెండవ->name))
			sig.append(it.రెండవ);

	తిరిగి నిజం;
}

తార్కిక RTLIL::SigSpec::parse_rhs(స్థిరరాశి RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	ఒకవేళ (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.కొలత());
		తిరిగి నిజం;
	}

	ఒకవేళ (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.కొలత());
		తిరిగి నిజం;
	}

	ఒకవేళ (lhs.is_chunk()) {
		అక్షరం *p = (అక్షరం*)str.c_str(), *endptr;
		దీర్ఘ పూర్ణాంకం val = strtol(p, &endptr, 10);
		ఒకవేళ (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.కొలత());
			తిరిగి నిజం;
		}
	}

	ఒకవేళ (!parse(sig, module, str))
		తిరిగి అబద్ధం;
	ఒకవేళ (sig.కొలత() > lhs.కొలత())
		sig.remove(lhs.కొలత(), sig.కొలత() - lhs.కొలత());
	తిరిగి నిజం;
}

RTLIL::SigSpec::క్రియాకారకం std::సదిశ<RTLIL::SigChunk>() స్థిరరాశి
{
	std::సదిశ<RTLIL::SigChunk> ఫలితం;
	కొరకు (స్థిరరాశి RTLIL::SigChunk &c : chunks())
		ఫలితం.నెట్టు_వెనుక(c);
	తిరిగి ఫలితం;
}

RTLIL::Case_నియమం::~Case_నియమం()
{
	కొరకు (స్వయంచాలక it = switches.begin(); it != switches.చివర(); it++)
		తొలగించు *it;
}

తార్కిక RTLIL::Case_నియమం::ఖాళీలు() స్థిరరాశి
{
	తిరిగి actions.ఖాళీలు() && switches.ఖాళీలు();
}

RTLIL::Case_నియమం *RTLIL::Case_నియమం::clone() స్థిరరాశి
{
	RTLIL::Case_నియమం *new_caserule = కొత్త RTLIL::Case_నియమం;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->లక్షణాలు = లక్షణాలు;
	కొరకు (స్వయంచాలక &it : switches)
		new_caserule->switches.నెట్టు_వెనుక(it->clone());
	తిరిగి new_caserule;
}

RTLIL::Switch_నియమం::~Switch_నియమం()
{
	కొరకు (స్వయంచాలక it = cases.begin(); it != cases.చివర(); it++)
		తొలగించు *it;
}

తార్కిక RTLIL::Switch_నియమం::ఖాళీలు() స్థిరరాశి
{
	తిరిగి cases.ఖాళీలు();
}

RTLIL::Switch_నియమం *RTLIL::Switch_నియమం::clone() స్థిరరాశి
{
	RTLIL::Switch_నియమం *new_switchrule = కొత్త RTLIL::Switch_నియమం;
	new_switchrule->సంకేతం = సంకేతం;
	new_switchrule->లక్షణాలు = లక్షణాలు;
	కొరకు (స్వయంచాలక &it : cases)
		new_switchrule->cases.నెట్టు_వెనుక(it->clone());
	తిరిగి new_switchrule;

}

RTLIL::Sync_నియమం *RTLIL::Sync_నియమం::clone() స్థిరరాశి
{
	RTLIL::Sync_నియమం *new_syncrule = కొత్త RTLIL::Sync_నియమం;
	new_syncrule->type = type;
	new_syncrule->సంకేతం = సంకేతం;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	తిరిగి new_syncrule;
}

RTLIL::Process::~Process()
{
	కొరకు (స్వయంచాలక it = syncs.begin(); it != syncs.చివర(); it++)
		తొలగించు *it;
}

RTLIL::Process *RTLIL::Process::clone() స్థిరరాశి
{
	RTLIL::Process *new_proc = కొత్త RTLIL::Process;

	new_proc->లక్షణాలు = లక్షణాలు;

	RTLIL::Case_నియమం *rc_ptr = మూలం_case.clone();
	new_proc->మూలం_case = *rc_ptr;
	rc_ptr->switches.clear();
	తొలగించు rc_ptr;

	కొరకు (స్వయంచాలక &it : syncs)
		new_proc->syncs.నెట్టు_వెనుక(it->clone());

	తిరిగి new_proc;
}

RTLIL::జ్ఞాపకం::~జ్ఞాపకం()
{
#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	RTLIL::జ్ఞాపకం::get_all_memorys()->erase(hashidx_);
#అయితే_ముగింపు
}

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
స్థిర std::map<గుర్తులేని పూర్ణాంకం, RTLIL::జ్ఞాపకం*> all_memorys;
std::map<గుర్తులేని పూర్ణాంకం, RTLIL::జ్ఞాపకం*> *RTLIL::జ్ఞాపకం::get_all_memorys(శూన్య)
{
	తిరిగి &all_memorys;
}
#అయితే_ముగింపు
YOSYS_NAMESPACE_చివర
