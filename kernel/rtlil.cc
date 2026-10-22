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
		प्रति (आकार_t i = 0; i < StaticCellTypes::builder.गणनम्; i++) {
			स्वचालित &cell = StaticCellTypes::builder.cells[i];
			यदि (cell.features.is_ff)
				r.निवेशय(cell.type);
		}
		प्रत्यागम r;
	}();
	प्रत्यागम res;
}

#परिभाषय check(condition) log_assert(condition && "malformed Const union")

ध्रुव Const::bitvectype& Const::get_द्वयङ्काः() ध्रुव {
	check(is_द्वयङ्काः());
	प्रत्यागम *get_if_द्वयङ्काः();
}

ध्रुव std::string& Const::get_str() ध्रुव {
	check(is_str());
	प्रत्यागम *get_if_str();
}

Const::bitvectype& Const::get_द्वयङ्काः() {
	check(is_द्वयङ्काः());
	प्रत्यागम *get_if_द्वयङ्काः();
}

std::string& Const::get_str() {
	check(is_str());
	प्रत्यागम *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	नूतन ((शून्य*)&str_) std::string(std::move(str));
	चिह्नकम् = backing_चिह्नकम्::string;
}

RTLIL::Const::Const(दीर्घ दीर्घ val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	वर्ण अष्टकानि[] = {
		(वर्ण)(val >> 24), (वर्ण)(val >> 16), (वर्ण)(val >> 8), (वर्ण)val
	};
	नूतन ((शून्य*)&str_) std::string(अष्टकानि, 4);
	चिह्नकम् = backing_चिह्नकम्::string;
}

RTLIL::Const::Const(दीर्घ दीर्घ val, पूर्णाङ्क विस्तारः)
{
	log_assert(विस्तारः >= 0 && विस्तारः < RTLIL::विस्तारः_सीमा);
	flags = RTLIL::CONST_FLAG_NONE;
	यदि ((विस्तारः & 7) == 0) {
		नूतन ((शून्य*)&str_) std::string();
		चिह्नकम् = backing_चिह्नकम्::string;
		std::string& str = get_str();
		पूर्णाङ्क अष्टकानि = विस्तारः >> 3;
		चिह्नित वर्ण sign_byte = val < 0 ? -1 : 0;
		str.resize(अष्टकानि, sign_byte);
		अष्टकानि = std::min<पूर्णाङ्क>(अष्टकानि, परिमाण(val));
		प्रति (पूर्णाङ्क i = 0; i < अष्टकानि; i++) {
			str[str.आकार() - 1 - i] = val;
			val = val >> 8;
		}
		प्रत्यागम;
	}

	नूतन ((शून्य*)&द्वयङ्काः_2) bitvectype();
	चिह्नकम् = backing_चिह्नकम्::द्वयङ्काः;
	bitvectype& bv = get_द्वयङ्काः();
	bv.reserve(विस्तारः);
	प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++) {
		bv.प्रक्षिप_पृष्ठभागः((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, पूर्णाङ्क विस्तारः)
{
	log_assert(विस्तारः >= 0 && विस्तारः < RTLIL::विस्तारः_सीमा);
	flags = RTLIL::CONST_FLAG_NONE;
	नूतन ((शून्य*)&द्वयङ्काः_2) bitvectype();
	चिह्नकम् = backing_चिह्नकम्::द्वयङ्काः;
	bitvectype& bv = get_द्वयङ्काः();
	bv.reserve(विस्तारः);
	प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++)
		bv.प्रक्षिप_पृष्ठभागः(bit);
}

RTLIL::Const::Const(ध्रुव std::सदिशम्<तार्किक> &द्वयङ्काः)
{
	flags = RTLIL::CONST_FLAG_NONE;
	नूतन ((शून्य*)&द्वयङ्काः_2) bitvectype();
	चिह्नकम् = backing_चिह्नकम्::द्वयङ्काः;
	bitvectype& bv = get_द्वयङ्काः();
	bv.reserve(द्वयङ्काः.आकार());
	प्रति (ध्रुव स्वचालित &b : द्वयङ्काः)
		bv.emplace_पृष्ठभागः(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(ध्रुव RTLIL::Const &other) {
	चिह्नकम् = other.चिह्नकम्;
	flags = other.flags;
	यदि (is_str())
		नूतन ((शून्य*)&str_) std::string(other.get_str());
	अन्यथा यदि (is_द्वयङ्काः())
		नूतन ((शून्य*)&द्वयङ्काः_2) bitvectype(other.get_द्वयङ्काः());
	अन्यथा
		check(असत्य);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	चिह्नकम् = other.चिह्नकम्;
	flags = other.flags;
	यदि (is_str())
		नूतन ((शून्य*)&str_) std::string(std::move(other.get_str()));
	अन्यथा यदि (is_द्वयङ्काः())
		नूतन ((शून्य*)&द्वयङ्काः_2) bitvectype(std::move(other.get_द्वयङ्काः()));
	अन्यथा
		check(असत्य);
}

RTLIL::Const &RTLIL::Const::संक्रियकः =(ध्रुव RTLIL::Const &other) {
	flags = other.flags;
	यदि (other.is_str()) {
		यदि (!is_str()) {
			// sketchy zone
			check(is_द्वयङ्काः());
			द्वयङ्काः_2.~bitvectype();
			(शून्य)नूतन ((शून्य*)&str_) std::string();
		}
		चिह्नकम् = other.चिह्नकम्;
		get_str() = other.get_str();
	} अन्यथा यदि (other.is_द्वयङ्काः()) {
		यदि (!is_द्वयङ्काः()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(शून्य)नूतन ((शून्य*)&द्वयङ्काः_2) bitvectype();
		}
		चिह्नकम् = other.चिह्नकम्;
		get_द्वयङ्काः() = other.get_द्वयङ्काः();
	} अन्यथा {
		check(असत्य);
	}
	प्रत्यागम *एतत्;
}

RTLIL::Const::~Const() {
	यदि (is_द्वयङ्काः())
		द्वयङ्काः_2.~bitvectype();
	अन्यथा यदि (is_str())
		str_.~string();
	अन्यथा
		check(असत्य);
}

तार्किक RTLIL::Const::संक्रियकः<(ध्रुव RTLIL::Const &other) ध्रुव
{
	यदि (आकार() != other.आकार())
		प्रत्यागम आकार() < other.आकार();

	प्रति (पूर्णाङ्क i = 0; i < आकार(); i++)
		यदि ((*एतत्)[i] != other[i])
			प्रत्यागम (*एतत्)[i] < other[i];

	प्रत्यागम असत्य;
}

तार्किक RTLIL::Const::संक्रियकः ==(ध्रुव RTLIL::Const &other) ध्रुव
{
	यदि (is_str() && other.is_str())
		प्रत्यागम get_str() == other.get_str();
	यदि (is_द्वयङ्काः() && other.is_द्वयङ्काः())
		प्रत्यागम get_द्वयङ्काः() == other.get_द्वयङ्काः();

	यदि (आकार() != other.आकार())
		प्रत्यागम असत्य;

	प्रति (पूर्णाङ्क i = 0; i < आकार(); i++)
	यदि ((*एतत्)[i] != other[i])
		प्रत्यागम असत्य;

	प्रत्यागम सत्य;
}

तार्किक RTLIL::Const::संक्रियकः !=(ध्रुव RTLIL::Const &other) ध्रुव
{
	प्रत्यागम !(*एतत् == other);
}

std::सदिशम्<RTLIL::State>& RTLIL::Const::द्वयङ्काः_आन्तरिकम्()
{
	bitvectorize_आन्तरिकम्();
	प्रत्यागम get_द्वयङ्काः();
}

std::सदिशम्<RTLIL::State> RTLIL::Const::गन्तव्यम्_द्वयङ्काः() ध्रुव
{
	std::सदिशम्<State> v;
	प्रति (स्वचालित bit : *एतत्)
		v.प्रक्षिप_पृष्ठभागः(bit);
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

	ध्रुव bitvectype& bv = get_द्वयङ्काः();
	प्रति (आकार_t i = 0; i < bv.आकार(); i++)
		यदि (bv[i] == State::S1)
			प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}

पूर्णाङ्क RTLIL::Const::as_int(तार्किक is_signed) ध्रुव
{
	int32_t ret = 0;
	यदि (ध्रुव std::string *s = get_if_str()) {
		पूर्णाङ्क आकार = Get_आकार(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		प्रति (पूर्णाङ्क i = std::min(4, आकार); i > 0; i--)
			ret |= static_cast<अचिह्नित वर्ण>((*s)[आकार - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		यदि (is_signed && आकार > 0 && आकार < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << आकार*8;
		प्रत्यागम ret;
	}

	ध्रुव bitvectype& bv = get_द्वयङ्काः();
	पूर्णाङ्क significant_द्वयङ्काः = std::min(Get_आकार(bv), 32);
	प्रति (पूर्णाङ्क i = 0; i < significant_द्वयङ्काः; i++)
		यदि (bv[i] == State::S1)
			ret |= 1 << i;
	यदि (is_signed && significant_द्वयङ्काः > 0 && significant_द्वयङ्काः < 32 && bv.पृष्ठभागः() == State::S1 )
		ret |= UINT32_MAX << significant_द्वयङ्काः;
	प्रत्यागम ret;
}

तार्किक RTLIL::Const::convertible_गन्तव्यम्_int(तार्किक is_signed) ध्रुव
{
	स्वचालित आकार = get_min_आकार(is_signed);

	यदि (आकार < 0)
		प्रत्यागम असत्य;

	// If it fits in 31 bits it is definitely convertible
	यदि (आकार <= 31)
		प्रत्यागम सत्य;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	यदि (आकार == 32) {
		यदि (is_signed)
			प्रत्यागम सत्य;
		प्रत्यागम (*एतत्)[आकार - 1] != State::S1;
	}

	प्रत्यागम असत्य;
}

std::optional<पूर्णाङ्क> RTLIL::Const::try_as_int(तार्किक is_signed) ध्रुव
{
	यदि (!convertible_गन्तव्यम्_int(is_signed))
		प्रत्यागम std::nullopt;
	प्रत्यागम as_int(is_signed);
}

पूर्णाङ्क RTLIL::Const::as_int_saturating(तार्किक is_signed) ध्रुव
{
	यदि (!convertible_गन्तव्यम्_int(is_signed)) {
		यदि (!is_signed)
			प्रत्यागम std::numeric_limits<पूर्णाङ्क>::max();

		ध्रुव स्वचालित min_आकार = get_min_आकार(is_signed);
		log_assert(min_आकार > 0);
		ध्रुव स्वचालित neg = (*एतत्)[min_आकार - 1];
		प्रत्यागम neg ? std::numeric_limits<पूर्णाङ्क>::min() : std::numeric_limits<पूर्णाङ्क>::max();
	}
	प्रत्यागम as_int(is_signed);
}

शून्य RTLIL::Const::चिह्नकम्_bare_integer_const(ध्रुव std::string &मानम्)
{
	यदि (मानम्.रिक्तानि() || मानम्.find('\'') != std::string::npos)
		प्रत्यागम;
	आकार_t start = (मानम्[0] == '-' || मानम्[0] == '+') ? 1 : 0;
	यदि (start < मानम्.आकार() && std::all_सम्बन्धि(मानम्.begin() + start, मानम्.अन्तः(), ::अङ्कः_वा))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

पूर्णाङ्क RTLIL::Const::get_min_आकार(तार्किक is_signed) ध्रुव
{
	यदि (रिक्तानि()) प्रत्यागम 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	यदि (is_signed)
		leading_bit = (पृष्ठभागः() == RTLIL::State::Sx) ? RTLIL::State::S0 : पृष्ठभागः();
	अन्यथा
		leading_bit = RTLIL::State::S0;

	स्वचालित idx = आकार();
	यावत् (idx > 0 && (*एतत्)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	यदि (is_signed && idx < आकार()) {
		idx++;
	}
	// must be at least one bit
	प्रत्यागम (idx == 0) ? 1 : idx;
}

शून्य RTLIL::Const::compress(तार्किक is_signed)
{
	स्वचालित idx = get_min_आकार(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<पूर्णाङ्क> RTLIL::Const::as_int_compress(तार्किक is_signed) ध्रुव
{
	प्रत्यागम try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(ध्रुव वर्ण* any) ध्रुव
{
	पूर्णाङ्क sz = आकार();
	std::string ret;
	ret.reserve(sz);
	प्रति (पूर्णाङ्क i = sz - 1; i >= 0; --i)
		विकल्प ((*एतत्)[i]) {
			पक्ष S0: ret.प्रक्षिप_पृष्ठभागः('0'); भङ्ग;
			पक्ष S1: ret.प्रक्षिप_पृष्ठभागः('1'); भङ्ग;
			पक्ष Sx: ret.प्रक्षिप_पृष्ठभागः('x'); भङ्ग;
			पक्ष Sz: ret.प्रक्षिप_पृष्ठभागः('z'); भङ्ग;
			पक्ष Sa: ret += any; भङ्ग;
			पक्ष Sm: ret.प्रक्षिप_पृष्ठभागः('m'); भङ्ग;
		}
	प्रत्यागम ret;
}

RTLIL::Const RTLIL::Const::from_string(ध्रुव std::string &str)
{
	Const c;
	bitvectype& bv = c.get_द्वयङ्काः();
	bv.reserve(str.आकार());
	प्रति (स्वचालित it = str.rbegin(); it != str.rend(); it++)
		विकल्प (*it) {
			पक्ष '0': bv.प्रक्षिप_पृष्ठभागः(State::S0); भङ्ग;
			पक्ष '1': bv.प्रक्षिप_पृष्ठभागः(State::S1); भङ्ग;
			पक्ष 'x': bv.प्रक्षिप_पृष्ठभागः(State::Sx); भङ्ग;
			पक्ष 'z': bv.प्रक्षिप_पृष्ठभागः(State::Sz); भङ्ग;
			पक्ष 'm': bv.प्रक्षिप_पृष्ठभागः(State::Sm); भङ्ग;
			स्वभाव: bv.प्रक्षिप_पृष्ठभागः(State::Sa);
		}
	प्रत्यागम c;
}

std::string RTLIL::Const::decode_string() ध्रुव
{
	यदि (स्वचालित str = get_if_str())
		प्रत्यागम *str;

	ध्रुव bitvectype& bv = get_द्वयङ्काः();
	ध्रुव पूर्णाङ्क n = Get_आकार(bv);
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

पूर्णाङ्क RTLIL::Const::आकार() ध्रुव {
	यदि (is_str())
		प्रत्यागम 8 * str_.आकार();
	अन्यथा {
		check(is_द्वयङ्काः());
		प्रत्यागम द्वयङ्काः_2.आकार();
	}
}

तार्किक RTLIL::Const::रिक्तानि() ध्रुव {
	यदि (is_str())
		प्रत्यागम str_.रिक्तानि();
	अन्यथा {
		check(is_द्वयङ्काः());
		प्रत्यागम द्वयङ्काः_2.रिक्तानि();
	}
}

शून्य RTLIL::Const::bitvectorize_आन्तरिकम्() {
	यदि (चिह्नकम् == backing_चिह्नकम्::द्वयङ्काः)
		प्रत्यागम;

	check(is_str());

	bitvectype new_द्वयङ्काः;

	new_द्वयङ्काः.reserve(str_.आकार() * 8);
	प्रति (पूर्णाङ्क i = str_.आकार() - 1; i >= 0; i--) {
		अचिह्नित वर्ण ch = str_[i];
		प्रति (पूर्णाङ्क j = 0; j < 8; j++) {
			new_द्वयङ्काः.प्रक्षिप_पृष्ठभागः((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(शून्य)नूतन ((शून्य*)&द्वयङ्काः_2) bitvectype(std::move(new_द्वयङ्काः));
		चिह्नकम् = backing_चिह्नकम्::द्वयङ्काः;
	}
}

शून्य RTLIL::Const::append(ध्रुव RTLIL::Const &other) {
	bitvectorize_आन्तरिकम्();
	bitvectype& bv = get_द्वयङ्काः();
	bv.निवेशय(bv.अन्तः(), other.begin(), other.अन्तः());
}

RTLIL::State RTLIL::Const::const_iterator::संक्रियकः*() ध्रुव {
	यदि (स्वचालित bv = parent->get_if_द्वयङ्काः())
		प्रत्यागम (*bv)[idx];

	पूर्णाङ्क char_idx = parent->get_str().आकार() - idx / 8 - 1;
	तार्किक bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	प्रत्यागम bit ? State::S1 : State::S0;
}

तार्किक RTLIL::Const::is_fully_zero() ध्रुव
{
	यदि (स्वचालित str = get_if_str()) {
		प्रति (वर्ण ch : *str)
			यदि (ch != 0)
				प्रत्यागम असत्य;
		प्रत्यागम सत्य;
	}

	ध्रुव bitvectype& bv = get_द्वयङ्काः();

	प्रति (ध्रुव स्वचालित &bit : bv)
		यदि (bit != RTLIL::State::S0)
			प्रत्यागम असत्य;

	प्रत्यागम सत्य;
}

तार्किक RTLIL::Const::is_fully_ones() ध्रुव
{
	यदि (स्वचालित str = get_if_str()) {
		प्रति (वर्ण ch : *str)
			यदि (ch != (वर्ण)0xff)
				प्रत्यागम असत्य;
		प्रत्यागम सत्य;
	}

	ध्रुव bitvectype& bv = get_द्वयङ्काः();
	प्रति (ध्रुव स्वचालित &bit : bv)
		यदि (bit != RTLIL::State::S1)
			प्रत्यागम असत्य;

	प्रत्यागम सत्य;
}

तार्किक RTLIL::Const::is_fully_def() ध्रुव
{
	यदि (is_str())
		प्रत्यागम सत्य;

	ध्रुव bitvectype& bv = get_द्वयङ्काः();
	प्रति (ध्रुव स्वचालित &bit : bv)
		यदि (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			प्रत्यागम असत्य;

	प्रत्यागम सत्य;
}

तार्किक RTLIL::Const::is_fully_undef() ध्रुव
{
	यदि (स्वचालित str = get_if_str())
		प्रत्यागम str->रिक्तानि();

	ध्रुव bitvectype& bv = get_द्वयङ्काः();
	प्रति (ध्रुव स्वचालित &bit : bv)
		यदि (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			प्रत्यागम असत्य;

	प्रत्यागम सत्य;
}

तार्किक RTLIL::Const::is_fully_undef_x_only() ध्रुव
{
	यदि (स्वचालित str = get_if_str())
		प्रत्यागम str->रिक्तानि();

	ध्रुव bitvectype& bv = get_द्वयङ्काः();
	प्रति (ध्रुव स्वचालित &bit : bv)
		यदि (bit != RTLIL::State::Sx)
			प्रत्यागम असत्य;

	प्रत्यागम सत्य;
}

तार्किक RTLIL::Const::is_onehot(पूर्णाङ्क *pos) ध्रुव
{
	तार्किक प्राप्तम् = असत्य;
	पूर्णाङ्क आकार = Get_आकार(*एतत्);
	प्रति (पूर्णाङ्क i = 0; i < आकार; i++) {
		State bit = (*एतत्)[i];
		यदि (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			प्रत्यागम असत्य;
		यदि (bit == RTLIL::State::S1) {
			यदि (प्राप्तम्)
				प्रत्यागम असत्य;
			यदि (pos)
				*pos = i;
			प्राप्तम् = सत्य;
		}
	}
	प्रत्यागम प्राप्तम्;
}

Hasher RTLIL::Const::hash_into(Hasher h) ध्रुव
{
	यदि (स्वचालित str = get_if_str())
		प्रत्यागम hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	ध्रुव bitvectype &bv = get_द्वयङ्काः();
	पूर्णाङ्क आकार = Get_आकार(bv);
	std::string packed;
	पूर्णाङ्क packed_आकार = (आकार + 7) >> 3;
	packed.resize(packed_आकार, 0);
	प्रति (पूर्णाङ्क bi = 0; bi < packed_आकार; ++bi) {
		वर्ण ch = 0;
		पूर्णाङ्क अन्तः = std::min((bi + 1)*8, आकार);
		प्रति (पूर्णाङ्क i = bi*8; i < अन्तः; ++i) {
			RTLIL::State b = bv[i];
			यदि (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<वर्ण>::hash_into(ch, h);
				प्रति (; i < आकार; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(आकार);
				प्रत्यागम h;
			}
			ch |= static_cast<पूर्णाङ्क>(b) << (i & 7);
		}
		packed[packed_आकार - 1 - bi] = ch;
	}
	प्रत्यागम hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(पूर्णाङ्क विस्थापनम्, पूर्णाङ्क len, RTLIL::State padding) ध्रुव {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	प्रति (पूर्णाङ्क i = विस्थापनम्; i < विस्थापनम् + len; i++)
		ret_bv.प्रक्षिप_पृष्ठभागः(i < Get_आकार(*एतत्) ? (*एतत्)[i] : padding);
	प्रत्यागम RTLIL::Const(ret_bv);
}
#अपरिभाषय check /* check(condition) for Const */

तार्किक RTLIL::AttrObject::has_attribute(RTLIL::IdString id) ध्रुव
{
	प्रत्यागम गुणाः.गणनम्(id);
}

शून्य RTLIL::AttrObject::समुच्चयः_bool_attribute(RTLIL::IdString id, तार्किक मानम्)
{
	यदि (मानम्)
		गुणाः[id] = RTLIL::Const(1);
	अन्यथा
		गुणाः.erase(id);
}

तार्किक RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) ध्रुव
{
	ध्रुव स्वचालित it = गुणाः.find(id);
	यदि (it == गुणाः.अन्तः())
		प्रत्यागम असत्य;
	प्रत्यागम it->second.as_bool();
}

शून्य RTLIL::AttrObject::समुच्चयः_string_attribute(RTLIL::IdString id, string मानम्)
{
	यदि (मानम्.रिक्तानि())
		गुणाः.erase(id);
	अन्यथा
		गुणाः[id] = मानम्;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) ध्रुव
{
	std::string मानम्;
	ध्रुव स्वचालित it = गुणाः.find(id);
	यदि (it != गुणाः.अन्तः())
		मानम् = it->second.decode_string();
	प्रत्यागम मानम्;
}

शून्य RTLIL::AttrObject::समुच्चयः_strpool_attribute(RTLIL::IdString id, ध्रुव pool<string> &दत्त)
{
	string attrval;
	प्रति (ध्रुव स्वचालित &s : दत्त) {
		यदि (!attrval.रिक्तानि())
			attrval += "|";
		attrval += s;
	}
	समुच्चयः_string_attribute(id, attrval);
}

शून्य RTLIL::AttrObject::सङ्कलय_strpool_attribute(RTLIL::IdString id, ध्रुव pool<string> &दत्त)
{
	pool<string> union_दत्त = get_strpool_attribute(id);
	union_दत्त.निवेशय(दत्त.begin(), दत्त.अन्तः());
	यदि (!union_दत्त.रिक्तानि())
		समुच्चयः_strpool_attribute(id, union_दत्त);
}

शून्य RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->गुणाः = std::move(buf);
}

नामाकाश {
	प्रतिरूपम्<प्रकारनाम F>
	शून्य walk_process_attr_objects(RTLIL::Process *process, F दर्शय) {
		दर्शय(process);
		std::सदिशम्<RTLIL::Case_नियमः*> case_राशिः{&process->मूलम्_case};
		यावत् (!case_राशिः.रिक्तानि()) {
			RTLIL::Case_नियमः *cs = case_राशिः.पृष्ठभागः();
			case_राशिः.निष्कासय_पृष्ठभागः();
			दर्शय(cs);
			प्रति (स्वचालित *sw : cs->switches) {
				दर्शय(sw);
				प्रति (स्वचालित *case_ : sw->cases)
					case_राशिः.प्रक्षिप_पृष्ठभागः(case_);
			}
		}
		प्रति (स्वचालित *समकाल : process->syncs)
			प्रति (स्वचालित &mwa : समकाल->mem_write_actions)
				दर्शय(&mwa);
	}

	प्रतिरूपम्<प्रकारनाम F>
	शून्य walk_attr_objects(RTLIL::Design *design, F दर्शय) {
		प्रति (स्वचालित &[_, module] : design->modules_) {
			दर्शय(module);
			प्रति (स्वचालित &[_, wire] : module->wires_)
				दर्शय(wire);
			प्रति (स्वचालित &[_, mem] : module->memories)
				दर्शय(mem);
			प्रति (स्वचालित &[_, cell] : module->cells_)
				दर्शय(cell);
			प्रति (स्वचालित &[_, process] : module->processes)
				walk_process_attr_objects(process, दर्शय);
		}
	}
}


आकार_t RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> जीवितम्;
	स्वचालित मूलम् = [&](IdString ref) {
		यदि (ref != IdString::Null)
			जीवितम्.निवेशय(ref);
	};

	walk_attr_objects(एतत्, [&](ध्रुव RTLIL::AttrObject *obj) {
		प्रति (स्वचालित &attr : obj->गुणाः)
			मूलम्(attr.first);
	});

	मूलम्(selected_सक्रिय_module);

	प्रति (स्वचालित &[name, module] : modules_) {
		मूलम्(name);
		प्रति (IdString पत्तनम् : module->ports)
			मूलम्(पत्तनम्);
		प्रति (IdString param : module->avail_parameters)
			मूलम्(param);
		प्रति (स्वचालित &[param, _] : module->parameter_default_मानानि)
			मूलम्(param);
		प्रति (स्वचालित &[name, _] : module->memories)
			मूलम्(name);
		प्रति (स्वचालित &[name, _] : module->processes)
			मूलम्(name);
		प्रति (स्वचालित &[name, wire] : module->wires_) {
			मूलम्(name);
			यदि (wire->ज्ञातम्_driver())
				मूलम्(wire->driver_पत्तनम्());
		}
		प्रति (स्वचालित &[name, cell] : module->cells_) {
			मूलम्(name);
			मूलम्(cell->type);
			प्रति (स्वचालित &conn : cell->connections())
				मूलम्(conn.first);
			प्रति (स्वचालित &param : cell->parameters)
				मूलम्(param.first);
		}
		प्रति (स्वचालित &[_, process] : module->processes) {
			std::सदिशम्<RTLIL::Case_नियमः*> case_राशिः{&process->मूलम्_case};
			यावत् (!case_राशिः.रिक्तानि()) {
				RTLIL::Case_नियमः *cs = case_राशिः.पृष्ठभागः();
				case_राशिः.निष्कासय_पृष्ठभागः();
				प्रति (स्वचालित *sw : cs->switches) {
					प्रति (स्वचालित *case_ : sw->cases)
						case_राशिः.प्रक्षिप_पृष्ठभागः(case_);
				}
			}
			प्रति (स्वचालित *समकाल : process->syncs) {
				प्रति (स्वचालित &mwa : समकाल->mem_write_actions)
					मूलम्(mwa.memid);
			}
		}
	}

	स्वचालित मूलम्_selection = [&](ध्रुव RTLIL::Selection &sel) {
		प्रति (IdString m : sel.selected_modules)
			मूलम्(m);
		प्रति (स्वचालित &[m, members] : sel.selected_members) {
			मूलम्(m);
			प्रति (IdString member : members)
				मूलम्(member);
		}
	};

	प्रति (स्वचालित &[name, sel] : selection_vars) {
		मूलम्(name);
		मूलम्_selection(sel);
	}

	प्रति (स्वचालित &sel : selection_राशिः)
		मूलम्_selection(sel);

	आकार_t erased = twines_.gc(जीवितम्);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::व्यवकलय_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_गणनम्;
	प्रत्यागम erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) ध्रुव
{
	pool<string> दत्त;
	यदि (गुणाः.गणनम्(id) != 0)
		प्रति (स्वचालित s : विभज_tokens(get_string_attribute(id), "|"))
			दत्त.निवेशय(s);
	प्रत्यागम दत्त;
}

शून्य RTLIL::AttrObject::समुच्चयः_hdlname_attribute(ध्रुव सदिशम्<string> &hierarchy)
{
	string attrval;
	प्रति (ध्रुव स्वचालित &ident : hierarchy) {
		यदि (!attrval.रिक्तानि())
			attrval += " ";
		attrval += ident;
	}
	समुच्चयः_string_attribute(ID::hdlname, attrval);
}

सदिशम्<string> RTLIL::AttrObject::get_hdlname_attribute() ध्रुव
{
	प्रत्यागम विभज_tokens(get_string_attribute(ID::hdlname), " ");
}

शून्य RTLIL::AttrObject::समुच्चयः_intvec_attribute(RTLIL::IdString id, ध्रुव सदिशम्<पूर्णाङ्क> &दत्त)
{
	std::stringstream attrval;
	प्रति (स्वचालित &i : दत्त) {
		यदि (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	गुणाः[id] = RTLIL::Const(attrval.str());
}

सदिशम्<पूर्णाङ्क> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) ध्रुव
{
	सदिशम्<पूर्णाङ्क> दत्त;
	स्वचालित it = गुणाः.find(id);
	यदि (it != गुणाः.अन्तः())
		प्रति (ध्रुव स्वचालित &s : विभज_tokens(गुणाः.at(id).decode_string())) {
			वर्ण *अन्तः = शून्यसूचकः;
			errno = 0;
			दीर्घ मानम् = strtol(s.c_str(), &अन्तः, 10);
			यदि (अन्तः != s.c_str() + s.आकार())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			यदि (errno == ERANGE || मानम् < INT_MIN || मानम् > अधिकतम_पूर्णाङ्कः)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			दत्त.प्रक्षिप_पृष्ठभागः(मानम्);
		}
	प्रत्यागम दत्त;
}

तार्किक RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) ध्रुव
{
	यदि (current_design != शून्यसूचकः) {
		स्वचालित module = current_design->module(mod_name);
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
	यदि (पूर्णम्_selection)
		प्रत्यागम सत्य;
	यदि (selected_modules.गणनम्(mod_name) > 0)
		प्रत्यागम सत्य;
	यदि (selected_members.गणनम्(mod_name) > 0)
		प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}

तार्किक RTLIL::Selection::selected_सम्पूर्णम्_module(RTLIL::IdString mod_name) ध्रुव
{
	यदि (complete_selection)
		प्रत्यागम सत्य;
	यदि (!selects_boxes && boxed_module(mod_name))
		प्रत्यागम असत्य;
	यदि (पूर्णम्_selection)
		प्रत्यागम सत्य;
	यदि (selected_modules.गणनम्(mod_name) > 0)
		प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}

तार्किक RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ध्रुव
{
	यदि (complete_selection)
		प्रत्यागम सत्य;
	यदि (!selects_boxes && boxed_module(mod_name))
		प्रत्यागम असत्य;
	यदि (पूर्णम्_selection)
		प्रत्यागम सत्य;
	यदि (selected_modules.गणनम्(mod_name) > 0)
		प्रत्यागम सत्य;
	यदि (selected_members.गणनम्(mod_name) > 0)
		यदि (selected_members.at(mod_name).गणनम्(memb_name) > 0)
			प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}

शून्य RTLIL::Selection::optimize(RTLIL::Design *design)
{
	यदि (design != current_design) {
		current_design = design;
	}

	यदि (selects_boxes && पूर्णम्_selection)
		complete_selection = सत्य;
	यदि (complete_selection) {
		पूर्णम्_selection = असत्य;
		selects_boxes = सत्य;
	}
	यदि (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		प्रत्यागम;
	}

	std::सदिशम्<RTLIL::IdString> del_list, सङ्कलय_list;

	del_list.clear();
	प्रति (स्वचालित mod_name : selected_modules) {
		यदि (current_design->module(mod_name) == शून्यसूचकः || (!selects_boxes && boxed_module(mod_name)))
			del_list.प्रक्षिप_पृष्ठभागः(mod_name);
		selected_members.erase(mod_name);
	}
	प्रति (स्वचालित mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	प्रति (स्वचालित &it : selected_members)
		यदि (current_design->module(it.first) == शून्यसूचकः || (!selects_boxes && boxed_module(it.first)))
			del_list.प्रक्षिप_पृष्ठभागः(it.first);
	प्रति (स्वचालित mod_name : del_list)
		selected_members.erase(mod_name);

	प्रति (स्वचालित &it : selected_members) {
		del_list.clear();
		प्रति (स्वचालित memb_name : it.second)
			यदि (current_design->module(it.first)->गणनम्_id(memb_name) == 0)
				del_list.प्रक्षिप_पृष्ठभागः(memb_name);
		प्रति (स्वचालित memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	सङ्कलय_list.clear();
	प्रति (स्वचालित &it : selected_members)
		यदि (it.second.आकार() == 0)
			del_list.प्रक्षिप_पृष्ठभागः(it.first);
		अन्यथा यदि (it.second.आकार() == current_design->module(it.first)->wires_.आकार() + current_design->module(it.first)->memories.आकार() +
				current_design->module(it.first)->cells_.आकार() + current_design->module(it.first)->processes.आकार())
			सङ्कलय_list.प्रक्षिप_पृष्ठभागः(it.first);
	प्रति (स्वचालित mod_name : del_list)
		selected_members.erase(mod_name);
	प्रति (स्वचालित mod_name : सङ्कलय_list) {
		selected_members.erase(mod_name);
		selected_modules.निवेशय(mod_name);
	}

	यदि (selected_modules.आकार() == current_design->modules_.आकार()) {
		selected_modules.clear();
		selected_members.clear();
		यदि (selects_boxes)
			complete_selection = सत्य;
		अन्यथा
			पूर्णम्_selection = सत्य;
	}
}

शून्य RTLIL::Selection::clear()
{
	पूर्णम्_selection = असत्य;
	complete_selection = असत्य;
	selected_modules.clear();
	selected_members.clear();
}

RTLIL::Design::Design()
  : verilog_परिभाषते (नूतन define_map_t)
{
	स्थिर अचिह्नित पूर्णाङ्क hashidx_गणनम् = 123456789;
	hashidx_गणनम् = mkhash_xorshift(hashidx_गणनम्);
	hashidx_ = hashidx_गणनम्;

	refcount_modules_ = 0;
	selected_सक्रिय_module.clear();
	प्रक्षिप_पूर्णम्_selection();

	RTLIL::Design::get_all_designs()->निवेशय(std::pair<अचिह्नित पूर्णाङ्क, RTLIL::Design*>(hashidx_, एतत्));
}

RTLIL::Design::~Design()
{
	प्रति (स्वचालित &pr : modules_)
		विलोपय pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

स्थिर std::map<अचिह्नित पूर्णाङ्क, RTLIL::Design*> all_designs;
std::map<अचिह्नित पूर्णाङ्क, RTLIL::Design*> *RTLIL::Design::get_all_designs(शून्य)
{
	प्रत्यागम &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	प्रत्यागम RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

ध्रुव RTLIL::Module *RTLIL::Design::module(IdString id) ध्रुव {
	twines_.check_owned(id);
	प्रत्यागम modules_.गणनम्(id) ? modules_.at(id) : रिक्त;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	प्रत्यागम modules_.गणनम्(id) ? modules_.at(id) : रिक्त;
}

RTLIL::Module *RTLIL::Design::शिखरम्_module() ध्रुव
{
	RTLIL::Module *module = शून्यसूचकः;
	पूर्णाङ्क module_गणनम् = 0;

	प्रति (स्वचालित mod : selected_modules()) {
		यदि (mod->get_bool_attribute(ID::शिखरम्))
			प्रत्यागम mod;
		module_गणनम्++;
		module = mod;
	}

	प्रत्यागम module_गणनम् == 1 ? module : शून्यसूचकः;
}

RTLIL::Module *RTLIL::Design::सङ्कलय_Module(RTLIL::IdString name)
{
	यदि (modules_.गणनम्(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = नूतन RTLIL::Module;
	modules_[name] = module;
	module->design = एतत्;
	module->name = name;

	प्रति (स्वचालित mon : monitors)
		mon->notify_module_सङ्कलय(module);

	यदि (yosys_xtrace) {
		लघुगणकः("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	प्रत्यागम module;
}

शून्य RTLIL::Design::scratchpad_unset(ध्रुव std::string &varname)
{
	scratchpad.erase(varname);
}

शून्य RTLIL::Design::scratchpad_समुच्चयः_int(ध्रुव std::string &varname, पूर्णाङ्क मानम्)
{
	scratchpad[varname] = stringf("%d", मानम्);
}

शून्य RTLIL::Design::scratchpad_समुच्चयः_bool(ध्रुव std::string &varname, तार्किक मानम्)
{
	scratchpad[varname] = मानम् ? "true" : "false";
}

शून्य RTLIL::Design::scratchpad_समुच्चयः_string(ध्रुव std::string &varname, std::string मानम्)
{
	scratchpad[varname] = std::move(मानम्);
}

पूर्णाङ्क RTLIL::Design::scratchpad_get_int(ध्रुव std::string &varname, पूर्णाङ्क default_मानम्_2) ध्रुव
{
	स्वचालित it = scratchpad.find(varname);
	यदि (it == scratchpad.अन्तः())
		प्रत्यागम default_मानम्_2;

	ध्रुव std::string &str = it->second;

	यदि (str == "0" || str == "false")
		प्रत्यागम 0;

	यदि (str == "1" || str == "true")
		प्रत्यागम 1;

	वर्ण *endptr = शून्यसूचकः;
	दीर्घ पूर्णाङ्क parsed_मानम् = strtol(str.c_str(), &endptr, 10);
	प्रत्यागम *endptr ? default_मानम्_2 : parsed_मानम्;
}

तार्किक RTLIL::Design::scratchpad_get_bool(ध्रुव std::string &varname, तार्किक default_मानम्_2) ध्रुव
{
	स्वचालित it = scratchpad.find(varname);
	यदि (it == scratchpad.अन्तः())
		प्रत्यागम default_मानम्_2;

	ध्रुव std::string &str = it->second;

	यदि (str == "0" || str == "false")
		प्रत्यागम असत्य;

	यदि (str == "1" || str == "true")
		प्रत्यागम सत्य;

	प्रत्यागम default_मानम्_2;
}

std::string RTLIL::Design::scratchpad_get_string(ध्रुव std::string &varname, ध्रुव std::string &default_मानम्_2) ध्रुव
{
	स्वचालित it = scratchpad.find(varname);
	यदि (it == scratchpad.अन्तः())
		प्रत्यागम default_मानम्_2;

	प्रत्यागम it->second;
}

शून्य RTLIL::Design::remove(RTLIL::Module *module)
{
	प्रति (स्वचालित mon : monitors)
		mon->notify_module_del(module);

	यदि (yosys_xtrace) {
		लघुगणकः("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	विलोपय module;
}

शून्य RTLIL::Design::rename(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	सङ्कलय(module);
}

शून्य RTLIL::Design::क्रमय()
{
	scratchpad.क्रमय();
	modules_.क्रमय(क्रमय_by_id_str(twines_));
	प्रति (स्वचालित &it : modules_)
		it.second->क्रमय();
}

शून्य RTLIL::Design::क्रमय_modules()
{
	scratchpad.क्रमय();
	modules_.क्रमय(क्रमय_by_id_str(twines_));
}

शून्य check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

शून्य RTLIL::Design::check()
{
#यदि_अपरिभाषितम् NDEBUG
	log_assert(!selection_राशिः.रिक्तानि());
	पूर्णाङ्क pool_आकार = 0;
	प्रति (स्वचालित &it : modules_)
		pool_आकार = std::max(pool_आकार, ThreadPool::work_pool_आकार(0, it.second->cells_आकार(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_आकार);
	प्रति (स्वचालित &it : modules_) {
		log_assert(एतत् == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#यदि_अन्तः
}

शून्य RTLIL::Design::optimize()
{
	प्रति (स्वचालित &it : modules_)
		it.second->optimize();
	प्रति (स्वचालित &it : selection_राशिः)
		it.optimize(एतत्);
	प्रति (स्वचालित &it : selection_vars)
		it.second.optimize(एतत्);
}

शून्य RTLIL::Design::clone_into(RTLIL::Design *dst) ध्रुव
{
	log_assert(dst->modules_.रिक्तानि());
	dst->twines_ = twines_;
	प्रति (स्वचालित it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

तार्किक RTLIL::Design::selected_module(IdString mod_name) ध्रुव
{
	यदि (!selected_सक्रिय_module.रिक्तानि() && mod_name != selected_सक्रिय_module)
		प्रत्यागम असत्य;
	प्रत्यागम selection().selected_module(mod_name);
}

तार्किक RTLIL::Design::selected_सम्पूर्णम्_module(RTLIL::IdString mod_name) ध्रुव
{
	यदि (!selected_सक्रिय_module.रिक्तानि() && mod_name != selected_सक्रिय_module)
		प्रत्यागम असत्य;
	प्रत्यागम selection().selected_सम्पूर्णम्_module(mod_name);
}

तार्किक RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ध्रुव
{
	यदि (!selected_सक्रिय_module.रिक्तानि() && mod_name != selected_सक्रिय_module)
		प्रत्यागम असत्य;
	प्रत्यागम selection().selected_member(mod_name, memb_name);
}

तार्किक RTLIL::Design::selected_module(RTLIL::Module *mod) ध्रुव
{
	प्रत्यागम selected_module(mod->name);
}

तार्किक RTLIL::Design::selected_सम्पूर्णम्_module(RTLIL::Module *mod) ध्रुव
{
	प्रत्यागम selected_सम्पूर्णम्_module(mod->name);
}

शून्य RTLIL::Design::प्रक्षिप_selection(RTLIL::Selection sel)
{
	sel.current_design = एतत्;
	selection_राशिः.प्रक्षिप_पृष्ठभागः(sel);
}

शून्य RTLIL::Design::प्रक्षिप_रिक्तानि_selection()
{
	प्रक्षिप_selection(RTLIL::Selection::रिक्तानि_Selection(एतत्));
}

शून्य RTLIL::Design::प्रक्षिप_पूर्णम्_selection()
{
	प्रक्षिप_selection(RTLIL::Selection::पूर्णम्_Selection(एतत्));
}

शून्य RTLIL::Design::प्रक्षिप_complete_selection()
{
	प्रक्षिप_selection(RTLIL::Selection::CompleteSelection(एतत्));
}

शून्य RTLIL::Design::निष्कासय_selection()
{
	selection_राशिः.निष्कासय_पृष्ठभागः();
	// Default to a full_selection if we ran out of stack
	यदि (selection_राशिः.रिक्तानि())
		प्रक्षिप_पूर्णम्_selection();
}

std::string RTLIL::Design::गन्तव्यम्_rtlil_str(तार्किक only_selected) ध्रुव
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(एतत्), only_selected, सत्य, असत्य, RTLIL_BACKEND::Dump_बहुलकम्::Readable);
	प्रत्यागम f.str();
}

std::सदिशम्<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) ध्रुव
{
	तार्किक include_partials = partials == RTLIL::SELECT_ALL;
	तार्किक exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	तार्किक ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::सदिशम्<RTLIL::Module*> परिणाम;
	परिणाम.reserve(modules_.आकार());
	प्रति (स्वचालित &it : modules_) {
		यदि (selected_सम्पूर्णम्_module(it.first) || (include_partials && selected_module(it.first))) {
			यदि (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				परिणाम.प्रक्षिप_पृष्ठभागः(it.second);
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
			पक्ष RTLIL::SELECT_सम्पूर्णम्_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.second);
				भङ्ग;
			पक्ष RTLIL::SELECT_सम्पूर्णम्_ERR:
				log_error("Unsupported partially selected module %s.\n", it.second);
				भङ्ग;
			पक्ष RTLIL::SELECT_सम्पूर्णम्_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.second);
				भङ्ग;
			स्वभाव:
				भङ्ग;
			}
		}
	}
	प्रत्यागम परिणाम;
}

RTLIL::Module::Module()
{
	स्थिर अचिह्नित पूर्णाङ्क hashidx_गणनम् = 123456789;
	hashidx_गणनम् = mkhash_xorshift(hashidx_गणनम्);
	hashidx_ = hashidx_गणनम्;

	design = शून्यसूचकः;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->निवेशय(std::pair<अचिह्नित पूर्णाङ्क, RTLIL::Module*>(hashidx_, एतत्));
#यदि_अन्तः
}

RTLIL::Module::~Module()
{
	प्रति (स्वचालित &pr : wires_)
		विलोपय pr.second;
	प्रति (स्वचालित &pr : memories)
		विलोपय pr.second;
	प्रति (स्वचालित &pr : cells_)
		विलोपय pr.second;
	प्रति (स्वचालित &pr : processes)
		विलोपय pr.second;
#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#यदि_अन्तः
}

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
स्थिर std::map<अचिह्नित पूर्णाङ्क, RTLIL::Module*> all_modules;
std::map<अचिह्नित पूर्णाङ्क, RTLIL::Module*> *RTLIL::Module::get_all_modules(शून्य)
{
	प्रत्यागम &all_modules;
}
#यदि_अन्तः

शून्य RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	प्रति (स्वचालित it = wires_.begin(); it != wires_.अन्तः(); ++it)
		यदि (!it->second->पत्तनम्_निवेशः && !it->second->पत्तनम्_output)
			delwires.निवेशय(it->second);

	प्रति (स्वचालित it = memories.begin(); it != memories.अन्तः(); ++it)
		विलोपय it->second;
	memories.clear();

	प्रति (स्वचालित it = cells_.begin(); it != cells_.अन्तः(); ++it)
		विलोपय it->second;
	cells_.clear();

	प्रति (स्वचालित it = processes.begin(); it != processes.अन्तः(); ++it)
		विलोपय it->second;
	processes.clear();

	connections_.clear();

	remove(delwires);
	समुच्चयः_bool_attribute(ID::blackbox);
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

आकार_t RTLIL::Module::गणनम्_id(RTLIL::IdString id)
{
	प्रत्यागम wires_.गणनम्(id) + cells_.गणनम्(id) + memories.गणनम्(id) + processes.गणनम्(id);
}

#यदि_अपरिभाषितम् NDEBUG
नामाकाश {
	संरचना आन्तरिकम्_Cell_Checker
	{
		ध्रुव RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		आन्तरिकम्_Cell_Checker(ध्रुव RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		शून्य error(पूर्णाङ्क linenr, std::optional<std::string> error_सन्देशः = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			यदि (error_सन्देशः) {
				log_error("%s in internal cell %s\n", *error_सन्देशः, cell_loc);
			} अन्यथा {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		पूर्णाङ्क param(RTLIL::IdString name)
		{
			स्वचालित it = cell->parameters.find(name);
			यदि (it == cell->parameters.अन्तः()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.निवेशय(name);
			प्रत्यागम it->second.as_int();
		}

		पूर्णाङ्क param_bool(RTLIL::IdString name)
		{
			पूर्णाङ्क v = param(name);
			यदि (Get_आकार(cell->parameters.at(name)) > 32)
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

		शून्य param_द्वयङ्काः(RTLIL::IdString name, पूर्णाङ्क विस्तारः)
		{
			param(name);
			यदि (Get_आकार(cell->parameters.at(name)) != विस्तारः)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			प्रत्यागम cell->parameters.at(name).decode_string();
		}

		शून्य पत्तनम्(RTLIL::IdString name, पूर्णाङ्क विस्तारः)
		{
			स्वचालित it = cell->connections_.find(name);
			यदि (it == cell->connections_.अन्तः()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			यदि (Get_आकार(it->second) != विस्तारः) {
				std::string err = stringf("Expected port to have width %d", विस्तारः);
				error(__LINE__, err);
			}
			expected_ports.निवेशय(name);
		}

		शून्य check_expected(तार्किक check_matched_sign = असत्य)
		{
			प्रति (स्वचालित &para : cell->parameters)
				यदि (expected_params.गणनम्(para.first) == 0)
					error(__LINE__);
			प्रति (स्वचालित &conn : cell->connections())
				यदि (expected_ports.गणनम्(conn.first) == 0)
					error(__LINE__);

			यदि (check_matched_sign) {
				log_assert(expected_params.गणनम्(ID::A_SIGNED) != 0 && expected_params.गणनम्(ID::B_SIGNED) != 0);
				तार्किक a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				तार्किक b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				यदि (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		शून्य check()
		{
			यदि (cell->type.is_लौकिकम्() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				प्रत्यागम;

			यदि (cell->type.अन्तर्(ID($buf), ID($barrier))) {
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::Y, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.अन्तर्(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				पत्तनम्(ID::A, param(ID::A_विस्तारः));
				पत्तनम्(ID::Y, param(ID::Y_विस्तारः));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.अन्तर्(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				पत्तनम्(ID::A, param(ID::A_विस्तारः));
				पत्तनम्(ID::B, param(ID::B_विस्तारः));
				पत्तनम्(ID::Y, param(ID::Y_विस्तारः));
				check_expected(सत्य);
				प्रत्यागम;
			}

			यदि (cell->type.अन्तर्(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				पत्तनम्(ID::A, param(ID::A_विस्तारः));
				पत्तनम्(ID::Y, param(ID::Y_विस्तारः));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.अन्तर्(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/असत्य);
				पत्तनम्(ID::A, param(ID::A_विस्तारः));
				पत्तनम्(ID::B, param(ID::B_विस्तारः));
				पत्तनम्(ID::Y, param(ID::Y_विस्तारः));
				check_expected(/*check_matched_sign=*/असत्य);
				प्रत्यागम;
			}

			यदि (cell->type.अन्तर्(ID($सरणम्), ID($shiftx))) {
				यदि (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/असत्य);
				} अन्यथा {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				पत्तनम्(ID::A, param(ID::A_विस्तारः));
				पत्तनम्(ID::B, param(ID::B_विस्तारः));
				पत्तनम्(ID::Y, param(ID::Y_विस्तारः));
				check_expected(/*check_matched_sign=*/असत्य);
				प्रत्यागम;
			}

			यदि (cell->type.अन्तर्(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				पत्तनम्(ID::A, param(ID::A_विस्तारः));
				पत्तनम्(ID::B, param(ID::B_विस्तारः));
				पत्तनम्(ID::Y, param(ID::Y_विस्तारः));
				check_expected(सत्य);
				प्रत्यागम;
			}

			यदि (cell->type.अन्तर्(ID($सङ्कलय), ID($sub), ID($mul), ID($भाजय), ID($mod), ID($divfloor), ID($modfloor), ID($घातः))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				पत्तनम्(ID::A, param(ID::A_विस्तारः));
				पत्तनम्(ID::B, param(ID::B_विस्तारः));
				पत्तनम्(ID::Y, param(ID::Y_विस्तारः));
				check_expected(cell->type != ID($घातः));
				प्रत्यागम;
			}

			यदि (cell->type == ID($fa)) {
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::B, param(ID::विस्तारः_2));
				पत्तनम्(ID::C, param(ID::विस्तारः_2));
				पत्तनम्(ID::X, param(ID::विस्तारः_2));
				पत्तनम्(ID::Y, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($lcu)) {
				पत्तनम्(ID::P, param(ID::विस्तारः_2));
				पत्तनम्(ID::G, param(ID::विस्तारः_2));
				पत्तनम्(ID::CI, 1);
				पत्तनम्(ID::CO, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($alu)) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				पत्तनम्(ID::A, param(ID::A_विस्तारः));
				पत्तनम्(ID::B, param(ID::B_विस्तारः));
				पत्तनम्(ID::CI, 1);
				पत्तनम्(ID::BI, 1);
				पत्तनम्(ID::X, param(ID::Y_विस्तारः));
				पत्तनम्(ID::Y, param(ID::Y_विस्तारः));
				पत्तनम्(ID::CO, param(ID::Y_विस्तारः));
				check_expected(सत्य);
				प्रत्यागम;
			}

			यदि (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_विस्तारः);
				पत्तनम्(ID::A, param(ID::A_विस्तारः));
				पत्तनम्(ID::B, param(ID::B_विस्तारः));
				पत्तनम्(ID::Y, param(ID::Y_विस्तारः));
				check_expected();
				Macc().from_cell(cell);
				प्रत्यागम;
			}

			यदि (cell->type == ID($macc_v2)) {
				यदि (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				यदि (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_द्वयङ्काः(ID::गुणनफलम्_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_द्वयङ्काः(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_द्वयङ्काः(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_द्वयङ्काः(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_द्वयङ्काः(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				यदि (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_द्वयङ्काः(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_द्वयङ्काः(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_द्वयङ्काः(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				ध्रुव Const &a_विस्तारः = cell->getParam(ID::A_WIDTHS);
				ध्रुव Const &b_विस्तारः = cell->getParam(ID::B_WIDTHS);
				ध्रुव Const &c_विस्तारः = cell->getParam(ID::C_WIDTHS);
				पूर्णाङ्क a_विस्तारः_योग = 0, b_विस्तारः_योग = 0, c_विस्तारः_योग = 0;
				प्रति (पूर्णाङ्क i = 0; i < param(ID::NPRODUCTS); i++) {
					a_विस्तारः_योग += a_विस्तारः.extract(16 * i, 16).as_int(असत्य);
					b_विस्तारः_योग += b_विस्तारः.extract(16 * i, 16).as_int(असत्य);
				}
				प्रति (पूर्णाङ्क i = 0; i < param(ID::NADDENDS); i++) {
					c_विस्तारः_योग += c_विस्तारः.extract(16 * i, 16).as_int(असत्य);
				}
				पत्तनम्(ID::A, a_विस्तारः_योग);
				पत्तनम्(ID::B, b_विस्तारः_योग);
				पत्तनम्(ID::C, c_विस्तारः_योग);
				पत्तनम्(ID::Y, param(ID::Y_विस्तारः));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				पत्तनम्(ID::A, param(ID::A_विस्तारः));
				पत्तनम्(ID::Y, param(ID::Y_विस्तारः));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.अन्तर्(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				पत्तनम्(ID::A, param(ID::A_विस्तारः));
				पत्तनम्(ID::B, param(ID::B_विस्तारः));
				पत्तनम्(ID::Y, param(ID::Y_विस्तारः));
				check_expected(/*check_matched_sign=*/असत्य);
				प्रत्यागम;
			}

			यदि (cell->type == ID($खण्डिका)) {
				param(ID::विस्थापनम्_2);
				पत्तनम्(ID::A, param(ID::A_विस्तारः));
				पत्तनम्(ID::Y, param(ID::Y_विस्तारः));
				यदि (param(ID::विस्थापनम्_2) + param(ID::Y_विस्तारः) > param(ID::A_विस्तारः))
					error(__LINE__);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($concat)) {
				पत्तनम्(ID::A, param(ID::A_विस्तारः));
				पत्तनम्(ID::B, param(ID::B_विस्तारः));
				पत्तनम्(ID::Y, param(ID::A_विस्तारः) + param(ID::B_विस्तारः));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($mux)) {
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::B, param(ID::विस्तारः_2));
				पत्तनम्(ID::S, 1);
				पत्तनम्(ID::Y, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($pmux)) {
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::B, param(ID::विस्तारः_2) * param(ID::S_विस्तारः));
				पत्तनम्(ID::S, param(ID::S_विस्तारः));
				पत्तनम्(ID::Y, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($bmux)) {
				पत्तनम्(ID::A, param(ID::विस्तारः_2) << param(ID::S_विस्तारः));
				पत्तनम्(ID::S, param(ID::S_विस्तारः));
				पत्तनम्(ID::Y, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($demux)) {
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::S, param(ID::S_विस्तारः));
				पत्तनम्(ID::Y, param(ID::विस्तारः_2) << param(ID::S_विस्तारः));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($lut)) {
				param(ID::LUT);
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::Y, 1);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($sop)) {
				param(ID::गभीरता_2);
				param(ID::सारणी);
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::Y, 1);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($sr)) {
				param_bool(ID::समुच्चयः_POLARITY);
				param_bool(ID::CLR_POLARITY);
				पत्तनम्(ID::समुच्चयः_2, param(ID::विस्तारः_2));
				पत्तनम्(ID::CLR, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q,   param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($ff)) {
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::EN, 1);
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::समुच्चयः_POLARITY);
				param_bool(ID::CLR_POLARITY);
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::समुच्चयः_2, param(ID::विस्तारः_2));
				पत्तनम्(ID::CLR, param(ID::विस्तारः_2));
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($dffsre)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::समुच्चयः_POLARITY);
				param_bool(ID::CLR_POLARITY);
				param_bool(ID::EN_POLARITY);
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::EN, 1);
				पत्तनम्(ID::समुच्चयः_2, param(ID::विस्तारः_2));
				पत्तनम्(ID::CLR, param(ID::विस्तारः_2));
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_द्वयङ्काः(ID::ARST_मानम्, param(ID::विस्तारः_2));
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::ARST, 1);
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_द्वयङ्काः(ID::SRST_मानम्, param(ID::विस्तारः_2));
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::SRST, 1);
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.अन्तर्(ID($sdffe), ID($sdffce))) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_द्वयङ्काः(ID::SRST_मानम्, param(ID::विस्तारः_2));
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::EN, 1);
				पत्तनम्(ID::SRST, 1);
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($adffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_द्वयङ्काः(ID::ARST_मानम्, param(ID::विस्तारः_2));
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::EN, 1);
				पत्तनम्(ID::ARST, 1);
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::ALOAD, 1);
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::AD, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($aldffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::EN, 1);
				पत्तनम्(ID::ALOAD, 1);
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::AD, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				पत्तनम्(ID::EN, 1);
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_द्वयङ्काः(ID::ARST_मानम्, param(ID::विस्तारः_2));
				पत्तनम्(ID::EN, 1);
				पत्तनम्(ID::ARST, 1);
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::समुच्चयः_POLARITY);
				param_bool(ID::CLR_POLARITY);
				पत्तनम्(ID::EN, 1);
				पत्तनम्(ID::समुच्चयः_2, param(ID::विस्तारः_2));
				पत्तनम्(ID::CLR, param(ID::विस्तारः_2));
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($fsm)) {
				param(ID::NAME);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param(ID::STATE_द्वयङ्काः);
				param(ID::STATE_अङ्कः);
				param(ID::STATE_अङ्कः_LOG2);
				param(ID::STATE_RST);
				param_द्वयङ्काः(ID::STATE_सारणी, param(ID::STATE_द्वयङ्काः) * param(ID::STATE_अङ्कः));
				param(ID::TRANS_अङ्कः);
				param_द्वयङ्काः(ID::TRANS_सारणी, param(ID::TRANS_अङ्कः) * (2*param(ID::STATE_अङ्कः_LOG2) + param(ID::CTRL_अन्तर्_विस्तारः) + param(ID::CTRL_बहिर्गमनम्_विस्तारः)));
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::ARST, 1);
				पत्तनम्(ID::CTRL_अन्तर्, param(ID::CTRL_अन्तर्_विस्तारः));
				पत्तनम्(ID::CTRL_बहिर्गमनम्, param(ID::CTRL_बहिर्गमनम्_विस्तारः));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::EN, 1);
				पत्तनम्(ID::ADDR, param(ID::ABITS));
				पत्तनम्(ID::दत्त_2, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($memrd_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::TRANSPARENCY_आवरणम्);
				param(ID::COLLISION_X_आवरणम्);
				param_bool(ID::CE_OVER_SRST);
				param_द्वयङ्काः(ID::ARST_मानम्, param(ID::विस्तारः_2));
				param_द्वयङ्काः(ID::SRST_मानम्, param(ID::विस्तारः_2));
				param_द्वयङ्काः(ID::INIT_मानम्, param(ID::विस्तारः_2));
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::EN, 1);
				पत्तनम्(ID::ARST, 1);
				पत्तनम्(ID::SRST, 1);
				पत्तनम्(ID::ADDR, param(ID::ABITS));
				पत्तनम्(ID::दत्त_2, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::प्राथमिकता);
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::EN, param(ID::विस्तारः_2));
				पत्तनम्(ID::ADDR, param(ID::ABITS));
				पत्तनम्(ID::दत्त_2, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($memwr_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PORTID);
				param(ID::प्राथमिकता_आवरणम्_2);
				पत्तनम्(ID::CLK, 1);
				पत्तनम्(ID::EN, param(ID::विस्तारः_2));
				पत्तनम्(ID::ADDR, param(ID::ABITS));
				पत्तनम्(ID::दत्त_2, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::प्राथमिकता);
				पत्तनम्(ID::ADDR, param(ID::ABITS));
				पत्तनम्(ID::दत्त_2, param(ID::विस्तारः_2) * param(ID::शब्दाः));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::प्राथमिकता);
				पत्तनम्(ID::ADDR, param(ID::ABITS));
				पत्तनम्(ID::दत्त_2, param(ID::विस्तारः_2) * param(ID::शब्दाः));
				पत्तनम्(ID::EN, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($mem)) {
				param(ID::MEMID);
				param(ID::आकार_2);
				param(ID::विस्थापनम्_2);
				param(ID::INIT);
				param_द्वयङ्काः(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_द्वयङ्काः(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_द्वयङ्काः(ID::RD_TRANSPARENT, max(1, param(ID::RD_PORTS)));
				param_द्वयङ्काः(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_द्वयङ्काः(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				पत्तनम्(ID::RD_CLK, param(ID::RD_PORTS));
				पत्तनम्(ID::RD_EN, param(ID::RD_PORTS));
				पत्तनम्(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				पत्तनम्(ID::RD_दत्त, param(ID::RD_PORTS) * param(ID::विस्तारः_2));
				पत्तनम्(ID::WR_CLK, param(ID::WR_PORTS));
				पत्तनम्(ID::WR_EN, param(ID::WR_PORTS) * param(ID::विस्तारः_2));
				पत्तनम्(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				पत्तनम्(ID::WR_दत्त, param(ID::WR_PORTS) * param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($mem_v2)) {
				param(ID::MEMID);
				param(ID::आकार_2);
				param(ID::विस्थापनम्_2);
				param(ID::INIT);
				param_द्वयङ्काः(ID::RD_CLK_ENABLE, max(1, param(ID::RD_PORTS)));
				param_द्वयङ्काः(ID::RD_CLK_POLARITY, max(1, param(ID::RD_PORTS)));
				param_द्वयङ्काः(ID::RD_TRANSPARENCY_आवरणम्, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_द्वयङ्काः(ID::RD_COLLISION_X_आवरणम्, max(1, param(ID::RD_PORTS) * param(ID::WR_PORTS)));
				param_द्वयङ्काः(ID::RD_WIDE_CONTINUATION, max(1, param(ID::RD_PORTS)));
				param_द्वयङ्काः(ID::RD_CE_OVER_SRST, max(1, param(ID::RD_PORTS)));
				param_द्वयङ्काः(ID::RD_ARST_मानम्, max(1, param(ID::RD_PORTS) * param(ID::विस्तारः_2)));
				param_द्वयङ्काः(ID::RD_SRST_मानम्, max(1, param(ID::RD_PORTS) * param(ID::विस्तारः_2)));
				param_द्वयङ्काः(ID::RD_INIT_मानम्, max(1, param(ID::RD_PORTS) * param(ID::विस्तारः_2)));
				param_द्वयङ्काः(ID::WR_CLK_ENABLE, max(1, param(ID::WR_PORTS)));
				param_द्वयङ्काः(ID::WR_CLK_POLARITY, max(1, param(ID::WR_PORTS)));
				param_द्वयङ्काः(ID::WR_WIDE_CONTINUATION, max(1, param(ID::WR_PORTS)));
				param_द्वयङ्काः(ID::WR_प्राथमिकता_आवरणम्, max(1, param(ID::WR_PORTS) * param(ID::WR_PORTS)));
				पत्तनम्(ID::RD_CLK, param(ID::RD_PORTS));
				पत्तनम्(ID::RD_EN, param(ID::RD_PORTS));
				पत्तनम्(ID::RD_ARST, param(ID::RD_PORTS));
				पत्तनम्(ID::RD_SRST, param(ID::RD_PORTS));
				पत्तनम्(ID::RD_ADDR, param(ID::RD_PORTS) * param(ID::ABITS));
				पत्तनम्(ID::RD_दत्त, param(ID::RD_PORTS) * param(ID::विस्तारः_2));
				पत्तनम्(ID::WR_CLK, param(ID::WR_PORTS));
				पत्तनम्(ID::WR_EN, param(ID::WR_PORTS) * param(ID::विस्तारः_2));
				पत्तनम्(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				पत्तनम्(ID::WR_दत्त, param(ID::WR_PORTS) * param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($tribuf)) {
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::Y, param(ID::विस्तारः_2));
				पत्तनम्(ID::EN, 1);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($bweqx)) {
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::B, param(ID::विस्तारः_2));
				पत्तनम्(ID::Y, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($bwmux)) {
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::B, param(ID::विस्तारः_2));
				पत्तनम्(ID::S, param(ID::विस्तारः_2));
				पत्तनम्(ID::Y, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.अन्तर्(ID($प्रतिज्ञा), ID($assume), ID($जीवितम्), ID($fair), ID($cover))) {
				पत्तनम्(ID::A, 1);
				पत्तनम्(ID::EN, 1);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($initstate)) {
				पत्तनम्(ID::Y, 1);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.अन्तर्(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				पत्तनम्(ID::Y, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.अन्तर्(ID($anyinit))) {
				पत्तनम्(ID::D, param(ID::विस्तारः_2));
				पत्तनम्(ID::Q, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($equiv)) {
				पत्तनम्(ID::A, 1);
				पत्तनम्(ID::B, 1);
				पत्तनम्(ID::Y, 1);
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type.अन्तर्(ID($specify2), ID($specify3))) {
				param_bool(ID::पूर्णम्);
				param_bool(ID::SRC_DST_PEN);
				param_bool(ID::SRC_DST_POL);
				param(ID::T_RISE_MIN);
				param(ID::T_RISE_TYP);
				param(ID::T_RISE_MAX);
				param(ID::T_FALL_MIN);
				param(ID::T_FALL_TYP);
				param(ID::T_FALL_MAX);
				पत्तनम्(ID::EN, 1);
				पत्तनम्(ID::SRC, param(ID::SRC_विस्तारः));
				पत्तनम्(ID::DST, param(ID::DST_विस्तारः));
				यदि (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					पत्तनम्(ID::DAT, param(ID::DST_विस्तारः));
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
				param(ID::T_सीमा_MIN);
				param(ID::T_सीमा_TYP);
				param(ID::T_सीमा_MAX);
				param(ID::T_LIMIT2_MIN);
				param(ID::T_LIMIT2_TYP);
				param(ID::T_LIMIT2_MAX);
				पत्तनम्(ID::SRC_EN, 1);
				पत्तनम्(ID::DST_EN, 1);
				पत्तनम्(ID::SRC, param(ID::SRC_विस्तारः));
				पत्तनम्(ID::DST, param(ID::DST_विस्तारः));
				check_expected();
				प्रत्यागम;
			}

			यदि (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::प्राथमिकता);
				पत्तनम्(ID::EN, 1);
				पत्तनम्(ID::TRG, param(ID::TRG_विस्तारः));
				पत्तनम्(ID::तर्काः, param(ID::तर्काः_विस्तारः));
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
				param(ID::प्राथमिकता);
				पत्तनम्(ID::A, 1);
				पत्तनम्(ID::EN, 1);
				पत्तनम्(ID::TRG, param(ID::TRG_विस्तारः));
				पत्तनम्(ID::तर्काः, param(ID::तर्काः_विस्तारः));
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

			यदि (cell->type == ID($_BUF_))    { पत्तनम्(ID::A,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_NOT_))    { पत्तनम्(ID::A,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_AND_))    { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_NAND_))   { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_OR_))     { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_NOR_))    { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_XOR_))    { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_XNOR_))   { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_ANDNOT_)) { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_ORNOT_))  { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_MUX_))    { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::S,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_NMUX_))   { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::S,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_AOI3_))   { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::C,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_OAI3_))   { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::C,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_AOI4_))   { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::C,1); पत्तनम्(ID::D,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_OAI4_))   { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::C,1); पत्तनम्(ID::D,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }

			यदि (cell->type == ID($_TBUF_))  { पत्तनम्(ID::A,1); पत्तनम्(ID::Y,1); पत्तनम्(ID::E,1); check_expected(); प्रत्यागम; }

			यदि (cell->type == ID($_MUX4_))  { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::C,1); पत्तनम्(ID::D,1); पत्तनम्(ID::S,1); पत्तनम्(ID::T,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_MUX8_))  { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::C,1); पत्तनम्(ID::D,1); पत्तनम्(ID::E,1); पत्तनम्(ID::F,1); पत्तनम्(ID::G,1); पत्तनम्(ID::H,1); पत्तनम्(ID::S,1); पत्तनम्(ID::T,1); पत्तनम्(ID::U,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }
			यदि (cell->type == ID($_MUX16_)) { पत्तनम्(ID::A,1); पत्तनम्(ID::B,1); पत्तनम्(ID::C,1); पत्तनम्(ID::D,1); पत्तनम्(ID::E,1); पत्तनम्(ID::F,1); पत्तनम्(ID::G,1); पत्तनम्(ID::H,1); पत्तनम्(ID::I,1); पत्तनम्(ID::J,1); पत्तनम्(ID::K,1); पत्तनम्(ID::L,1); पत्तनम्(ID::M,1); पत्तनम्(ID::N,1); पत्तनम्(ID::O,1); पत्तनम्(ID::P,1); पत्तनम्(ID::S,1); पत्तनम्(ID::T,1); पत्तनम्(ID::U,1); पत्तनम्(ID::V,1); पत्तनम्(ID::Y,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ पत्तनम्(ID::S,1); पत्तनम्(ID::R,1); पत्तनम्(ID::Q,1); check_expected(); प्रत्यागम; }

			यदि (cell->type == ID($_FF_)) { पत्तनम्(ID::D,1); पत्तनम्(ID::Q,1); check_expected();  प्रत्यागम; }

			यदि (cell->type.अन्तर्(ID($_DFF_N_), ID($_DFF_P_)))
				{ पत्तनम्(ID::D,1); पत्तनम्(ID::Q,1); पत्तनम्(ID::C,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ पत्तनम्(ID::D,1); पत्तनम्(ID::Q,1); पत्तनम्(ID::C,1); पत्तनम्(ID::E,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ पत्तनम्(ID::D,1); पत्तनम्(ID::Q,1); पत्तनम्(ID::C,1); पत्तनम्(ID::R,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ पत्तनम्(ID::D,1); पत्तनम्(ID::Q,1); पत्तनम्(ID::C,1); पत्तनम्(ID::R,1); पत्तनम्(ID::E,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ पत्तनम्(ID::D,1); पत्तनम्(ID::Q,1); पत्तनम्(ID::C,1); पत्तनम्(ID::L,1); पत्तनम्(ID::AD,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ पत्तनम्(ID::D,1); पत्तनम्(ID::Q,1); पत्तनम्(ID::C,1); पत्तनम्(ID::L,1); पत्तनम्(ID::AD,1); पत्तनम्(ID::E,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ पत्तनम्(ID::C,1); पत्तनम्(ID::S,1); पत्तनम्(ID::R,1); पत्तनम्(ID::D,1); पत्तनम्(ID::Q,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ पत्तनम्(ID::C,1); पत्तनम्(ID::S,1); पत्तनम्(ID::R,1); पत्तनम्(ID::D,1); पत्तनम्(ID::E,1); पत्तनम्(ID::Q,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ पत्तनम्(ID::D,1); पत्तनम्(ID::Q,1); पत्तनम्(ID::C,1); पत्तनम्(ID::R,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ पत्तनम्(ID::D,1); पत्तनम्(ID::Q,1); पत्तनम्(ID::C,1); पत्तनम्(ID::R,1); पत्तनम्(ID::E,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ पत्तनम्(ID::E,1); पत्तनम्(ID::D,1); पत्तनम्(ID::Q,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ पत्तनम्(ID::E,1); पत्तनम्(ID::R,1); पत्तनम्(ID::D,1); पत्तनम्(ID::Q,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ पत्तनम्(ID::E,1); पत्तनम्(ID::S,1); पत्तनम्(ID::R,1); पत्तनम्(ID::D,1); पत्तनम्(ID::Q,1); check_expected(); प्रत्यागम; }

			यदि (cell->type.अन्तर्(ID($समुच्चयः_चिह्नकम्))) {
				param(ID::विस्तारः_2);
				param(ID::चिह्नकम्_2);
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::समुच्चयः_2, param(ID::विस्तारः_2));
				पत्तनम्(ID::CLR, param(ID::विस्तारः_2));
				पत्तनम्(ID::Y, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}
			यदि (cell->type.अन्तर्(ID($get_चिह्नकम्),ID($original_चिह्नकम्))) {
				param(ID::विस्तारः_2);
				param(ID::चिह्नकम्_2);
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::Y, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}
			यदि (cell->type.अन्तर्(ID($overwrite_चिह्नकम्))) {
				param(ID::विस्तारः_2);
				param(ID::चिह्नकम्_2);
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::समुच्चयः_2, param(ID::विस्तारः_2));
				पत्तनम्(ID::CLR, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}
			यदि (cell->type.अन्तर्(ID($future_ff))) {
				param(ID::विस्तारः_2);
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::Y, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}
			यदि (cell->type.अन्तर्(ID($निवेशः_पत्तनम्))) {
				param(ID::विस्तारः_2);
				पत्तनम्(ID::Y, param(ID::विस्तारः_2));
				check_expected();
				प्रत्यागम;
			}
			यदि (cell->type.अन्तर्(ID($connect))) {
				param(ID::विस्तारः_2);
				पत्तनम्(ID::A, param(ID::विस्तारः_2));
				पत्तनम्(ID::B, param(ID::विस्तारः_2));
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
#यदि_अन्तः

शून्य RTLIL::Module::क्रमय()
{
	wires_.क्रमय(क्रमय_by_id_str(design->twines()));
	cells_.क्रमय(क्रमय_by_id_str(design->twines()));
	parameter_default_मानानि.क्रमय(क्रमय_by_id_str(design->twines()));
	memories.क्रमय(क्रमय_by_id_str(design->twines()));
	processes.क्रमय(क्रमय_by_id_str(design->twines()));
	प्रति (स्वचालित &it : cells_)
		it.second->क्रमय();
	प्रति (स्वचालित &it : wires_)
		it.second->गुणाः.क्रमय(क्रमय_by_id_str(design->twines()));
	प्रति (स्वचालित &it : memories)
		it.second->गुणाः.क्रमय(क्रमय_by_id_str(design->twines()));
}

#यदि_अपरिभाषितम् NDEBUG
स्थिर शून्य check_id_कुञ्चिकाः(ध्रुव dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	प्रति (स्वचालित &it : keyed)
		log_assert(!it.first.रिक्तानि());
}
#यदि_अन्तः

शून्य check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#यदि_अपरिभाषितम् NDEBUG
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_आकार(0, module->cells_आकार(), 1000));
	ध्रुव RTLIL::Module *const_module = module;

	pool<std::string> स्मृतिः_वाक्यानि;
	प्रति (स्वचालित &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->विस्तारः >= 0);
		log_assert(it.second->आकार >= 0);
		check_id_कुञ्चिकाः(it.second->गुणाः);
		स्मृतिः_वाक्यानि.निवेशय(it.second->name.str());
	}

	std::सदिशम्<MonotonicFlag> ports_declared(Get_आकार(module->ports));
	Sharded_सदिशम्<std::string> memids(subpool);
	subpool.सततम्([const_module, &ports_declared, &स्मृतिः_वाक्यानि, &memids](ध्रुव ParallelDispatchThreadPool::सततम्_Ctx &ctx) {
		प्रति (पूर्णाङ्क i : ctx.item_range(const_module->cells_आकार())) {
			स्वचालित it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.रिक्तानि());
			प्रति (स्वचालित &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_कुञ्चिकाः(it.second->गुणाः);
			check_id_कुञ्चिकाः(it.second->parameters);
			आन्तरिकम्_Cell_Checker checker(const_module, it.second);
			checker.check();
			यदि (it.second->has_memid()) {
				log_assert(स्मृतिः_वाक्यानि.गणनम्(it.second->parameters.at(ID::MEMID).decode_string()));
			} अन्यथा यदि (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!स्मृतिः_वाक्यानि.गणनम्(memid));
				memids.निवेशय(ctx, std::move(memid));
			}
			स्वचालित cell_mod = const_module->design->module(it.second->name);
			यदि (cell_mod != शून्यसूचकः) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#यदि_परिभाषितम् __GNUC__
				#निर्देशः GCC diagnostic प्रक्षिप
				#निर्देशः GCC diagnostic ignored "-Wdeprecated-declarations"
				#यदि_अन्तः
				log_assert(!it.second->get_blackbox_attribute());
				#यदि_परिभाषितम् __GNUC__
				#निर्देशः GCC diagnostic निष्कासय
				#यदि_अन्तः
			}
		}

		प्रति (पूर्णाङ्क i : ctx.item_range(const_module->wires_आकार())) {
			स्वचालित it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->विस्तारः >= 0);
			log_assert(it.second->पत्तनम्_id >= 0);
			check_id_कुञ्चिकाः(it.second->गुणाः);
			यदि (it.second->पत्तनम्_id) {
				log_assert(Get_आकार(const_module->ports) >= it.second->पत्तनम्_id);
				log_assert(const_module->ports.at(it.second->पत्तनम्_id-1) == it.second->name);
				log_assert(it.second->पत्तनम्_निवेशः || it.second->पत्तनम्_output);
				log_assert(it.second->पत्तनम्_id <= Get_आकार(ports_declared));
				तार्किक previously_declared = ports_declared[it.second->पत्तनम्_id-1].समुच्चयः_and_return_old();
				log_assert(previously_declared == असत्य);
			} अन्यथा
				log_assert(!it.second->पत्तनम्_निवेशः && !it.second->पत्तनम्_output);
		}
	});
	प्रति (ध्रुव MonotonicFlag &पत्तनम्_declared : ports_declared)
		log_assert(पत्तनम्_declared.आरोपय() == सत्य);
	pool<std::string> memids_pool;
	प्रति (std::string &memid : memids)
		log_assert(memids_pool.निवेशय(memid).second);

	प्रति (स्वचालित &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->मूलम्_case.compare.रिक्तानि());
		std::सदिशम्<RTLIL::Case_नियमः*> all_cases = {&it.second->मूलम्_case};
		प्रति (आकार_t i = 0; i < all_cases.आकार(); i++) {
			प्रति (स्वचालित &switch_it : all_cases[i]->switches) {
				प्रति (स्वचालित &case_it : switch_it->cases) {
					प्रति (स्वचालित &compare_it : case_it->compare) {
						log_assert(switch_it->सङ्केत.आकार() == compare_it.आकार());
					}
					all_cases.प्रक्षिप_पृष्ठभागः(case_it);
				}
			}
		}
		प्रति (स्वचालित &sync_it : it.second->syncs) {
			विकल्प (sync_it->type) {
				पक्ष RTLIL::SyncType::ST0:
				पक्ष RTLIL::SyncType::ST1:
				पक्ष RTLIL::SyncType::STp:
				पक्ष RTLIL::SyncType::STn:
				पक्ष RTLIL::SyncType::STe:
					log_assert(!sync_it->सङ्केत.रिक्तानि());
					भङ्ग;
				पक्ष RTLIL::SyncType::STa:
				पक्ष RTLIL::SyncType::STg:
				पक्ष RTLIL::SyncType::STi:
					log_assert(sync_it->सङ्केत.रिक्तानि());
					भङ्ग;
			}
		}
	}

	प्रति (स्वचालित &it : module->connections_) {
		log_assert(it.first.आकार() == it.second.आकार());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_कुञ्चिकाः(module->गुणाः);
#यदि_अन्तः
}

शून्य RTLIL::Module::check()
{
	पूर्णाङ्क pool_आकार = ThreadPool::work_pool_आकार(0, cells_आकार(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_आकार);
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
	new_mod->parameter_default_मानानि.clear();
	प्रति (स्वचालित &it : parameter_default_मानानि)
		new_mod->parameter_default_मानानि[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	प्रति (स्वचालित &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->गुणाः, गुणाः, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	प्रति (स्वचालित it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->सङ्कलय_Wire(dst_id, it->second);
	}

	प्रति (स्वचालित it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->सङ्कलय_स्मृतिः(dst_id, it->second);
	}

	प्रति (स्वचालित it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->सङ्कलय_Cell(dst_id, it->second);
	}

	प्रति (स्वचालित it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->सङ्कलय_Process(dst_id, it->second);
	}

	संरचना Rewrite_Sig_Spec_कार्यकर्ता
	{
		ध्रुव dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		ध्रुव RTLIL::Module *new_mod;
		शून्य संक्रियकः()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([एतत्](RTLIL::Wire *&wire) {
				स्वचालित it = wire_map.find(wire);
				यदि (it != wire_map.अन्तः())
					wire = it->second;
				अन्यथा
					log_assert(wire->module == new_mod);
			});
		}
	};

	Rewrite_Sig_Spec_कार्यकर्ता rewrite_Sig_Spec_कार्यकर्ता{wire_map, new_mod};
	new_mod->rewrite_sigspecs(rewrite_Sig_Spec_कार्यकर्ता);
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
	dst->सङ्कलय(new_mod);
	प्रत्यागम new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) ध्रुव
{
	RTLIL::Module *new_mod = नूतन RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->सङ्कलय(new_mod);
	प्रत्यागम new_mod;
}

तार्किक RTLIL::Module::has_memories() ध्रुव
{
	प्रत्यागम !memories.रिक्तानि();
}

तार्किक RTLIL::Module::has_processes() ध्रुव
{
	प्रत्यागम !processes.रिक्तानि();
}

तार्किक RTLIL::Module::has_memories_warn() ध्रुव
{
	यदि (!memories.रिक्तानि())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", एतत्);
	प्रत्यागम !memories.रिक्तानि();
}

तार्किक RTLIL::Module::has_processes_warn() ध्रुव
{
	यदि (!processes.रिक्तानि())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", एतत्);
	प्रत्यागम !processes.रिक्तानि();
}

तार्किक RTLIL::Module::is_selected() ध्रुव
{
	प्रत्यागम design->selected_module(एतत्->name);
}

तार्किक RTLIL::Module::is_selected_सम्पूर्णम्() ध्रुव
{
	प्रत्यागम design->selected_सम्पूर्णम्_module(एतत्->name);
}

std::सदिशम्<RTLIL::Wire*> RTLIL::Module::selected_wires() ध्रुव
{
	std::सदिशम्<RTLIL::Wire*> परिणाम;
	परिणाम.reserve(wires_.आकार());
	प्रति (स्वचालित &it : wires_)
		यदि (design->selected(एतत्, it.second))
			परिणाम.प्रक्षिप_पृष्ठभागः(it.second);
	प्रत्यागम परिणाम;
}

std::सदिशम्<RTLIL::Cell*> RTLIL::Module::selected_cells() ध्रुव
{
	std::सदिशम्<RTLIL::Cell*> परिणाम;
	परिणाम.reserve(cells_.आकार());
	प्रति (स्वचालित &it : cells_)
		यदि (design->selected(एतत्, it.second))
			परिणाम.प्रक्षिप_पृष्ठभागः(it.second);
	प्रत्यागम परिणाम;
}

std::सदिशम्<RTLIL::स्मृतिः*> RTLIL::Module::selected_memories() ध्रुव
{
	std::सदिशम्<RTLIL::स्मृतिः*> परिणाम;
	परिणाम.reserve(memories.आकार());
	प्रति (स्वचालित &it : memories)
		यदि (design->selected(एतत्, it.second))
			परिणाम.प्रक्षिप_पृष्ठभागः(it.second);
	प्रत्यागम परिणाम;
}

std::सदिशम्<RTLIL::Process*> RTLIL::Module::selected_processes() ध्रुव
{
	std::सदिशम्<RTLIL::Process*> परिणाम;
	परिणाम.reserve(processes.आकार());
	प्रति (स्वचालित &it : processes)
		यदि (design->selected(एतत्, it.second))
			परिणाम.प्रक्षिप_पृष्ठभागः(it.second);
	प्रत्यागम परिणाम;
}

std::सदिशम्<RTLIL::SelectedMember> RTLIL::Module::selected_members() ध्रुव
{
	std::सदिशम्<RTLIL::SelectedMember> परिणाम;
	प्रति (स्वचालित cell : selected_cells())
		परिणाम.प्रक्षिप_पृष्ठभागः({cell, cell->name});
	प्रति (स्वचालित स्मृतिः_3 : selected_memories())
		परिणाम.प्रक्षिप_पृष्ठभागः({स्मृतिः_3, स्मृतिः_3->name});
	प्रति (स्वचालित wire : selected_wires())
		परिणाम.प्रक्षिप_पृष्ठभागः({wire, wire->name});
	प्रति (स्वचालित process : selected_processes())
		परिणाम.प्रक्षिप_पृष्ठभागः({process, process->name});
	प्रत्यागम परिणाम;
}

शून्य RTLIL::Module::सङ्कलय(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(गणनम्_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = एतत्;
}

शून्य RTLIL::Module::सङ्कलय(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(गणनम्_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = एतत्;
}

शून्य RTLIL::Module::सङ्कलय(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(गणनम्_id(process->name) == 0);
	processes[process->name] = process;
	process->module = एतत्;
}

शून्य RTLIL::Module::remove(ध्रुव pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	यदि (wires.रिक्तानि())
		प्रत्यागम;

	संरचना Delete_Wire_कार्यकर्ता
	{
		RTLIL::Module *module;
		ध्रुव pool<RTLIL::Wire*> *wires_p;

		शून्य संक्रियकः()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([एतत्](RTLIL::Wire *&wire) {
				यदि (wires_p->गणनम्(wire))
					wire = module->सङ्कलय_Wire(stringf("$delete_wire$%d", (पूर्णाङ्क)autoidx++), wire->विस्तारः);
			});
		}

		शून्य संक्रियकः()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	Delete_Wire_कार्यकर्ता delete_wire_कार्यकर्ता;
	delete_wire_कार्यकर्ता.module = एतत्;
	delete_wire_कार्यकर्ता.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_कार्यकर्ता);

	यदि (design->flagBufferedNormalized) {
		प्रति (स्वचालित wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_सूचकांक.erase(wire);
		}
	}

	प्रति (स्वचालित &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.गणनम्(id) != 0);
		wires_.erase(id);
		विलोपय it;
	}
}

शून्य RTLIL::Module::remove(RTLIL::Cell *cell)
{
	यावत् (!cell->connections_.रिक्तानि())
		cell->unset_पत्तनम्(cell->connections_.begin()->first);

	log_assert(cells_.गणनम्(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	यदि (design && design->flagBufferedNormalized && buf_norm_cell_queue.गणनम्(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.निवेशय(cell);
	} अन्यथा {
		विलोपय cell;
	}
}

शून्य RTLIL::Module::remove(RTLIL::स्मृतिः *स्मृतिः_3)
{
	log_assert(memories.गणनम्(स्मृतिः_3->name) != 0);
	memories.erase(स्मृतिः_3->name);
	विलोपय स्मृतिः_3;
}

शून्य RTLIL::Module::remove(RTLIL::Process *process)
{
	log_assert(processes.गणनम्(process->name) != 0);
	processes.erase(process->name);
	विलोपय process;
}

शून्य RTLIL::Module::rename(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	सङ्कलय(wire);
}

शून्य RTLIL::Module::rename(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	सङ्कलय(cell);
}

शून्य RTLIL::Module::rename(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	यदि (old_id != IdString::Null && wires_.गणनम्(old_id))
		rename(wires_.at(old_id), new_name);
	अन्यथा यदि (old_id != IdString::Null && cells_.गणनम्(old_id))
		rename(cells_.at(old_id), new_name);
	अन्यथा
		log_abort();
}

शून्य RTLIL::Module::swap_नामानि(RTLIL::Wire *w1, RTLIL::Wire *w2)
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

शून्य RTLIL::Module::swap_नामानि(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	पूर्णाङ्क सूचकांक = 0;
	प्रत्यागम uniquify(name, सूचकांक);
}

IdString RTLIL::Module::uniquify(IdString name, पूर्णाङ्क &सूचकांक)
{
	यदि (सूचकांक == 0) {
		यदि (गणनम्_id(name) == 0)
			प्रत्यागम name;
		सूचकांक++;
	}

	यावत् (1) {
		IdString new_name = design->twines().सङ्कलय(TwineSpec::Suffix{name, stringf("_%d", सूचकांक)});
		यदि (गणनम्_id(new_name) == 0)
			प्रत्यागम new_name;
		सूचकांक++;
	}
}

स्थिर तार्किक fixup_ports_compare(ध्रुव RTLIL::Wire *a, ध्रुव RTLIL::Wire *b)
{
	यदि (a->पत्तनम्_id && !b->पत्तनम्_id)
		प्रत्यागम सत्य;
	यदि (!a->पत्तनम्_id && b->पत्तनम्_id)
		प्रत्यागम असत्य;

	यदि (a->पत्तनम्_id == b->पत्तनम्_id)
		प्रत्यागम a->name < b->name;
	प्रत्यागम a->पत्तनम्_id < b->पत्तनम्_id;
}

शून्य RTLIL::Module::connect(ध्रुव RTLIL::SigSig &conn)
{
	प्रति (स्वचालित mon : monitors)
		mon->notify_connect(एतत्, conn);

	यदि (design)
		प्रति (स्वचालित mon : design->monitors)
			mon->notify_connect(एतत्, conn);

	// ignore all attempts to assign constants to other constants
	यदि (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(conn.first); i++)
			यदि (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		यदि (Get_आकार(new_conn.first))
			connect(new_conn);
		प्रत्यागम;
	}

	यदि (yosys_xtrace) {
		लघुगणकः("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", एतत्, log_signal(conn.first), log_signal(conn.second), Get_आकार(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(Get_आकार(conn.first) == Get_आकार(conn.second));
	connections_.प्रक्षिप_पृष्ठभागः(conn);
}

शून्य RTLIL::Module::connect(ध्रुव RTLIL::SigSpec &lhs, ध्रुव RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

शून्य RTLIL::Module::new_connections(ध्रुव std::सदिशम्<RTLIL::SigSig> &new_conn)
{
	प्रति (स्वचालित mon : monitors)
		mon->notify_connect(एतत्, new_conn);

	यदि (design)
		प्रति (स्वचालित mon : design->monitors)
			mon->notify_connect(एतत्, new_conn);

	यदि (yosys_xtrace) {
		लघुगणकः("#X# New connections vector in %s:\n", एतत्);
		प्रति (स्वचालित &conn: new_conn)
			लघुगणकः("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), Get_आकार(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

ध्रुव std::सदिशम्<RTLIL::SigSig> &RTLIL::Module::connections() ध्रुव
{
	प्रत्यागम connections_;
}

शून्य RTLIL::Module::fixup_ports()
{
	std::सदिशम्<RTLIL::Wire*> all_ports;

	प्रति (स्वचालित &w : wires_)
		यदि (w.second->पत्तनम्_निवेशः || w.second->पत्तनम्_output)
			all_ports.प्रक्षिप_पृष्ठभागः(w.second);
		अन्यथा
			w.second->पत्तनम्_id = 0;

	std::क्रमय(all_ports.begin(), all_ports.अन्तः(), fixup_ports_compare);

	यदि (design && design->flagBufferedNormalized) {
		प्रति (स्वचालित &w : wires_)
			यदि (w.second->driverCell_ && w.second->driverCell_->type == ID($निवेशः_पत्तनम्))
				buf_norm_wire_queue.निवेशय(w.second);

		buf_norm_wire_queue.निवेशय(all_ports.begin(), all_ports.अन्तः());
	}

	ports.clear();
	प्रति (आकार_t i = 0; i < all_ports.आकार(); i++) {
		ports.प्रक्षिप_पृष्ठभागः(all_ports[i]->name);
		all_ports[i]->पत्तनम्_id = i+1;
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
	प्रति (पूर्णाङ्क i = Get_आकार(src) - 1; i >= 0; i--) {
		स्वचालित &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::सङ्कलय_Wire(RTLIL::IdString name, पूर्णाङ्क विस्तारः)
{
	log_assert(design);
	log_assert(विस्तारः >= 0 && विस्तारः < RTLIL::विस्तारः_सीमा);
	RTLIL::Wire *wire = नूतन RTLIL::Wire;
	wire->विस्तारः = विस्तारः;
	wire->name = name;
	सङ्कलय(wire);
	प्रत्यागम wire;
}

RTLIL::Wire *RTLIL::Module::सङ्कलय_Wire(IdString name, ध्रुव RTLIL::Wire *other)
{
	RTLIL::Wire *wire = सङ्कलय_Wire(name);
	wire->विस्तारः = other->विस्तारः;
	wire->start_विस्थापनम् = other->start_विस्थापनम्;
	wire->पत्तनम्_id = other->पत्तनम्_id;
	wire->पत्तनम्_निवेशः = other->पत्तनम्_निवेशः;
	wire->पत्तनम्_output = other->पत्तनम्_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		ध्रुव RTLIL::Design *src_design = other->module ? other->module->design : शून्यसूचकः;
		RTLIL::copy_attr_dict(wire->गुणाः, other->गुणाः, src_design, एतत्->design);
	}
	प्रत्यागम wire;
}

RTLIL::Cell *RTLIL::Module::सङ्कलय_Cell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = नूतन RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	सङ्कलय(cell);
	प्रत्यागम cell;
}

RTLIL::Cell *RTLIL::Module::सङ्कलय_Cell(IdString name, ध्रुव RTLIL::Cell *other)
{
	ध्रुव RTLIL::Design *src_design = other->module ? other->module->design : शून्यसूचकः;
	तार्किक cross_pool = src_design && एतत्->design && src_design != एतत्->design;

	IdString type = other->type.ref();
	यदि (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = सङ्कलय_Cell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, एतत्->design);
	RTLIL::copy_attr_dict(cell->गुणाः, other->गुणाः, src_design, एतत्->design);

	यदि (!cross_pool)
		cell->connections_ = other->connections_;
	अन्यथा
		प्रति (पूर्णाङ्क i = Get_आकार(other->connections_) - 1; i >= 0; i--) {
			स्वचालित &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	प्रत्यागम cell;
}

RTLIL::स्मृतिः *RTLIL::Module::सङ्कलय_स्मृतिः(IdString name)
{
	log_assert(design);
	RTLIL::स्मृतिः *mem = नूतन RTLIL::स्मृतिः;
	mem->module = एतत्;
	mem->name = name;
	memories[name] = mem;
	प्रत्यागम mem;
}

RTLIL::स्मृतिः *RTLIL::Module::सङ्कलय_स्मृतिः(IdString name, ध्रुव RTLIL::स्मृतिः *other)
{
	log_assert(design);
	RTLIL::स्मृतिः *mem = नूतन RTLIL::स्मृतिः;
	mem->module = एतत्;
	mem->name = name;
	mem->विस्तारः = other->विस्तारः;
	mem->start_विस्थापनम् = other->start_विस्थापनम्;
	mem->आकार = other->आकार;
	RTLIL::copy_attr_dict(mem->गुणाः, other->गुणाः,
			other->module ? other->module->design : शून्यसूचकः, design);
	memories[name] = mem;
	प्रत्यागम mem;
}

RTLIL::Process *RTLIL::Module::सङ्कलय_Process(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = नूतन RTLIL::Process;
	proc->name = name;
	सङ्कलय(proc);
	प्रत्यागम proc;
}

RTLIL::Process *RTLIL::Module::सङ्कलय_Process(RTLIL::IdString name, ध्रुव RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	ध्रुव RTLIL::Design *src_design = other->module ? other->module->design : शून्यसूचकः;
	यदि (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> गुणाः;
			RTLIL::copy_attr_dict(गुणाः, obj->गुणाः, src_design, design);
			obj->गुणाः = std::move(गुणाः);
		});
		प्रति (स्वचालित *समकाल : proc->syncs)
			प्रति (स्वचालित &mwa : समकाल->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	सङ्कलय(proc);
	प्रत्यागम proc;
}

#परिभाषय DEF_विधानम्(_func, y_आकार, _type) \
	RTLIL::Cell* RTLIL::Module::सङ्कलय ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = सङ्कलय_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_विस्तारः] = sig_a.आकार();       \
		cell->parameters[ID::Y_विस्तारः] = sig_y.आकार();       \
		cell->समुच्चयः_पत्तनम्(ID::A, sig_a);                        \
		cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);                        \
		cell->समुच्चयः_src_attribute(src);                       \
		प्रत्यागम cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = सङ्कलय_Wire(NEW_ID, y_आकार);    \
		सङ्कलय ## _func(name, sig_a, sig_y, is_signed, src);   \
		प्रत्यागम sig_y;                                       \
	}
DEF_विधानम्(Not,        sig_a.आकार(), ID($not))
DEF_विधानम्(Pos,        sig_a.आकार(), ID($pos))
DEF_विधानम्(Neg,        sig_a.आकार(), ID($neg))
DEF_विधानम्(ReduceAnd,  1, ID($reduce_and))
DEF_विधानम्(ReduceOr,   1, ID($reduce_or))
DEF_विधानम्(ReduceXor,  1, ID($reduce_xor))
DEF_विधानम्(ReduceXnor, 1, ID($reduce_xnor))
DEF_विधानम्(ReduceBool, 1, ID($reduce_bool))
DEF_विधानम्(LogicNot,   1, ID($logic_not))
#अपरिभाषय DEF_विधानम्

#परिभाषय DEF_विधानम्(_func, y_आकार, _type) \
	RTLIL::Cell* RTLIL::Module::सङ्कलय ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, तार्किक /* is_signed */, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = सङ्कलय_Cell(name, _type);           \
		cell->parameters[ID::विस्तारः_2] = sig_a.आकार();         \
		cell->समुच्चयः_पत्तनम्(ID::A, sig_a);                        \
		cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);                        \
		cell->समुच्चयः_src_attribute(src);                       \
		प्रत्यागम cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = सङ्कलय_Wire(NEW_ID, y_आकार);    \
		सङ्कलय ## _func(name, sig_a, sig_y, is_signed, src);   \
		प्रत्यागम sig_y;                                       \
	}
DEF_विधानम्(Buf,     sig_a.आकार(), ID($buf))
DEF_विधानम्(Barrier, sig_a.आकार(), ID($barrier))
#अपरिभाषय DEF_विधानम्

#परिभाषय DEF_विधानम्(_func, y_आकार, _type) \
	RTLIL::Cell* RTLIL::Module::सङ्कलय ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = सङ्कलय_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_विस्तारः] = sig_a.आकार();       \
		cell->parameters[ID::B_विस्तारः] = sig_b.आकार();       \
		cell->parameters[ID::Y_विस्तारः] = sig_y.आकार();       \
		cell->समुच्चयः_पत्तनम्(ID::A, sig_a);                        \
		cell->समुच्चयः_पत्तनम्(ID::B, sig_b);                        \
		cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);                        \
		cell->समुच्चयः_src_attribute(src);                       \
		प्रत्यागम cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = सङ्कलय_Wire(NEW_ID, y_आकार);         \
		सङ्कलय ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		प्रत्यागम sig_y;                                            \
	}
DEF_विधानम्(And,      max(sig_a.आकार(), sig_b.आकार()), ID($and))
DEF_विधानम्(Or,       max(sig_a.आकार(), sig_b.आकार()), ID($or))
DEF_विधानम्(Xor,      max(sig_a.आकार(), sig_b.आकार()), ID($xor))
DEF_विधानम्(Xnor,     max(sig_a.आकार(), sig_b.आकार()), ID($xnor))
DEF_विधानम्(सरणम्_2,    sig_a.आकार(), ID($सरणम्))
DEF_विधानम्(Lt,       1, ID($lt))
DEF_विधानम्(Le,       1, ID($le))
DEF_विधानम्(Eq,       1, ID($eq))
DEF_विधानम्(Ne,       1, ID($ne))
DEF_विधानम्(Eqx,      1, ID($eqx))
DEF_विधानम्(Nex,      1, ID($nex))
DEF_विधानम्(Ge,       1, ID($ge))
DEF_विधानम्(Gt,       1, ID($gt))
DEF_विधानम्(सङ्कलय_2,      max(sig_a.आकार(), sig_b.आकार()), ID($सङ्कलय))
DEF_विधानम्(Sub,      max(sig_a.आकार(), sig_b.आकार()), ID($sub))
DEF_विधानम्(Mul,      max(sig_a.आकार(), sig_b.आकार()), ID($mul))
DEF_विधानम्(Div,      max(sig_a.आकार(), sig_b.आकार()), ID($भाजय))
DEF_विधानम्(Mod,      max(sig_a.आकार(), sig_b.आकार()), ID($mod))
DEF_विधानम्(DivFloor, max(sig_a.आकार(), sig_b.आकार()), ID($divfloor))
DEF_विधानम्(ModFloor, max(sig_a.आकार(), sig_b.आकार()), ID($modfloor))
DEF_विधानम्(LogicAnd, 1, ID($logic_and))
DEF_विधानम्(LogicOr,  1, ID($logic_or))
#अपरिभाषय DEF_विधानम्

#परिभाषय DEF_विधानम्(_func, y_आकार, _type) \
	RTLIL::Cell* RTLIL::Module::सङ्कलय ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = सङ्कलय_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = असत्य;             \
		cell->parameters[ID::A_विस्तारः] = sig_a.आकार();       \
		cell->parameters[ID::B_विस्तारः] = sig_b.आकार();       \
		cell->parameters[ID::Y_विस्तारः] = sig_y.आकार();       \
		cell->समुच्चयः_पत्तनम्(ID::A, sig_a);                        \
		cell->समुच्चयः_पत्तनम्(ID::B, sig_b);                        \
		cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);                        \
		cell->समुच्चयः_src_attribute(src);                       \
		प्रत्यागम cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = सङ्कलय_Wire(NEW_ID, y_आकार);         \
		सङ्कलय ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		प्रत्यागम sig_y;                                            \
	}
DEF_विधानम्(Shl,      sig_a.आकार(), ID($shl))
DEF_विधानम्(Shr,      sig_a.आकार(), ID($shr))
DEF_विधानम्(Sshl,     sig_a.आकार(), ID($sshl))
DEF_विधानम्(Sshr,     sig_a.आकार(), ID($sshr))
#अपरिभाषय DEF_विधानम्

#परिभाषय DEF_विधानम्(_func, y_आकार, _type) \
	RTLIL::Cell* RTLIL::Module::सङ्कलय ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = सङ्कलय_Cell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = असत्य;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_विस्तारः] = sig_a.आकार();       \
		cell->parameters[ID::B_विस्तारः] = sig_b.आकार();       \
		cell->parameters[ID::Y_विस्तारः] = sig_y.आकार();       \
		cell->समुच्चयः_पत्तनम्(ID::A, sig_a);                        \
		cell->समुच्चयः_पत्तनम्(ID::B, sig_b);                        \
		cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);                        \
		cell->समुच्चयः_src_attribute(src);                       \
		प्रत्यागम cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, तार्किक is_signed, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = सङ्कलय_Wire(NEW_ID, y_आकार);         \
		सङ्कलय ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		प्रत्यागम sig_y;                                            \
	}
DEF_विधानम्(Shiftx,      sig_a.आकार(), ID($shiftx))
#अपरिभाषय DEF_विधानम्

#परिभाषय DEF_विधानम्(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::सङ्कलय ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = सङ्कलय_Cell(name, _type);                 \
		cell->parameters[ID::विस्तारः_2] = sig_a.आकार();               \
		यदि (_pmux) cell->parameters[ID::S_विस्तारः] = sig_s.आकार();  \
		cell->समुच्चयः_पत्तनम्(ID::A, sig_a);                              \
		cell->समुच्चयः_पत्तनम्(ID::B, sig_b);                              \
		cell->समुच्चयः_पत्तनम्(ID::S, sig_s);                              \
		cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);                              \
		cell->समुच्चयः_src_attribute(src);                             \
		प्रत्यागम cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = सङ्कलय_Wire(NEW_ID, sig_a.आकार());     \
		सङ्कलय ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		प्रत्यागम sig_y;                                             \
	}
DEF_विधानम्(Mux,      ID($mux),        0)
DEF_विधानम्(Bwmux,    ID($bwmux),      0)
DEF_विधानम्(Pmux,     ID($pmux),       1)
#अपरिभाषय DEF_विधानम्

#परिभाषय DEF_विधानम्(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::सङ्कलय ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = सङ्कलय_Cell(name, _type);                 \
		cell->parameters[ID::विस्तारः_2] = _demux ? sig_a.आकार() : sig_y.आकार(); \
		cell->parameters[ID::S_विस्तारः] = sig_s.आकार();             \
		cell->समुच्चयः_पत्तनम्(ID::A, sig_a);                              \
		cell->समुच्चयः_पत्तनम्(ID::S, sig_s);                              \
		cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);                              \
		cell->समुच्चयः_src_attribute(src);                             \
		प्रत्यागम cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = सङ्कलय_Wire(NEW_ID, _demux ? sig_a.आकार() << sig_s.आकार() : sig_a.आकार() >> sig_s.आकार()); \
		सङ्कलय ## _func(name, sig_a, sig_s, sig_y, src);             \
		प्रत्यागम sig_y;                                             \
	}
DEF_विधानम्(Bmux,     ID($bmux),       0)
DEF_विधानम्(Demux,    ID($demux),      1)
#अपरिभाषय DEF_विधानम्

#परिभाषय DEF_विधानम्(_func, _type) \
	RTLIL::Cell* RTLIL::Module::सङ्कलय ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = सङ्कलय_Cell(name, _type);                 \
		cell->parameters[ID::विस्तारः_2] = sig_a.आकार();               \
		cell->समुच्चयः_पत्तनम्(ID::A, sig_a);                              \
		cell->समुच्चयः_पत्तनम्(ID::B, sig_b);                              \
		cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);                              \
		cell->समुच्चयः_src_attribute(src);                             \
		प्रत्यागम cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव std::string &src) { \
		RTLIL::SigSpec sig_y = सङ्कलय_Wire(NEW_ID, sig_a.आकार());     \
		सङ्कलय ## _func(name, sig_a, sig_s, sig_y, src);             \
		प्रत्यागम sig_y;                                             \
	}
DEF_विधानम्(Bweqx,    ID($bweqx))
#अपरिभाषय DEF_विधानम्

#परिभाषय DEF_विधानम्_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::सङ्कलय ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = सङ्कलय_Cell(name, _type);         \
		cell->समुच्चयः_पत्तनम्("\\" #_P1, sig1);                   \
		cell->समुच्चयः_पत्तनम्("\\" #_P2, sig2);                   \
		cell->समुच्चयः_src_attribute(src);                     \
		प्रत्यागम cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव std::string &src) { \
		RTLIL::SigBit sig2 = सङ्कलय_Wire(NEW_ID);             \
		सङ्कलय ## _func(name, sig1, sig2, src);              \
		प्रत्यागम sig2;                                      \
	}
#परिभाषय DEF_विधानम्_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::सङ्कलय ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव RTLIL::SigBit &sig3, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = सङ्कलय_Cell(name, _type);         \
		cell->समुच्चयः_पत्तनम्("\\" #_P1, sig1);                   \
		cell->समुच्चयः_पत्तनम्("\\" #_P2, sig2);                   \
		cell->समुच्चयः_पत्तनम्("\\" #_P3, sig3);                   \
		cell->समुच्चयः_src_attribute(src);                     \
		प्रत्यागम cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव std::string &src) { \
		RTLIL::SigBit sig3 = सङ्कलय_Wire(NEW_ID);             \
		सङ्कलय ## _func(name, sig1, sig2, sig3, src);        \
		प्रत्यागम sig3;                                      \
	}
#परिभाषय DEF_विधानम्_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::सङ्कलय ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव RTLIL::SigBit &sig3, ध्रुव RTLIL::SigBit &sig4, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = सङ्कलय_Cell(name, _type);         \
		cell->समुच्चयः_पत्तनम्("\\" #_P1, sig1);                   \
		cell->समुच्चयः_पत्तनम्("\\" #_P2, sig2);                   \
		cell->समुच्चयः_पत्तनम्("\\" #_P3, sig3);                   \
		cell->समुच्चयः_पत्तनम्("\\" #_P4, sig4);                   \
		cell->समुच्चयः_src_attribute(src);                     \
		प्रत्यागम cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव RTLIL::SigBit &sig3, ध्रुव std::string &src) { \
		RTLIL::SigBit sig4 = सङ्कलय_Wire(NEW_ID);             \
		सङ्कलय ## _func(name, sig1, sig2, sig3, sig4, src);  \
		प्रत्यागम sig4;                                      \
	}
#परिभाषय DEF_विधानम्_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::सङ्कलय ## _func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव RTLIL::SigBit &sig3, ध्रुव RTLIL::SigBit &sig4, ध्रुव RTLIL::SigBit &sig5, ध्रुव std::string &src) { \
		RTLIL::Cell *cell = सङ्कलय_Cell(name, _type);         \
		cell->समुच्चयः_पत्तनम्("\\" #_P1, sig1);                   \
		cell->समुच्चयः_पत्तनम्("\\" #_P2, sig2);                   \
		cell->समुच्चयः_पत्तनम्("\\" #_P3, sig3);                   \
		cell->समुच्चयः_पत्तनम्("\\" #_P4, sig4);                   \
		cell->समुच्चयः_पत्तनम्("\\" #_P5, sig5);                   \
		cell->समुच्चयः_src_attribute(src);                     \
		प्रत्यागम cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig1, ध्रुव RTLIL::SigBit &sig2, ध्रुव RTLIL::SigBit &sig3, ध्रुव RTLIL::SigBit &sig4, ध्रुव std::string &src) { \
		RTLIL::SigBit sig5 = सङ्कलय_Wire(NEW_ID);                  \
		सङ्कलय ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		प्रत्यागम sig5;                                           \
	}
DEF_विधानम्_2(BufGate,    ID($_BUF_),    A, Y)
DEF_विधानम्_2(NotGate,    ID($_NOT_),    A, Y)
DEF_विधानम्_3(AndGate,    ID($_AND_),    A, B, Y)
DEF_विधानम्_3(NandGate,   ID($_NAND_),   A, B, Y)
DEF_विधानम्_3(OrGate,     ID($_OR_),     A, B, Y)
DEF_विधानम्_3(NorGate,    ID($_NOR_),    A, B, Y)
DEF_विधानम्_3(XorGate,    ID($_XOR_),    A, B, Y)
DEF_विधानम्_3(XnorGate,   ID($_XNOR_),   A, B, Y)
DEF_विधानम्_3(AndnotGate, ID($_ANDNOT_), A, B, Y)
DEF_विधानम्_3(OrnotGate,  ID($_ORNOT_),  A, B, Y)
DEF_विधानम्_4(MuxGate,    ID($_MUX_),    A, B, S, Y)
DEF_विधानम्_4(NmuxGate,   ID($_NMUX_),   A, B, S, Y)
DEF_विधानम्_4(Aoi3Gate,   ID($_AOI3_),   A, B, C, Y)
DEF_विधानम्_4(Oai3Gate,   ID($_OAI3_),   A, B, C, Y)
DEF_विधानम्_5(Aoi4Gate,   ID($_AOI4_),   A, B, C, D, Y)
DEF_विधानम्_5(Oai4Gate,   ID($_OAI4_),   A, B, C, D, Y)
#अपरिभाषय DEF_विधानम्_2
#अपरिभाषय DEF_विधानम्_3
#अपरिभाषय DEF_विधानम्_4
#अपरिभाषय DEF_विधानम्_5

RTLIL::Cell* RTLIL::Module::सङ्कलय_Pow(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, तार्किक a_signed, तार्किक b_signed, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($घातः));
	cell->parameters[ID::A_SIGNED] = a_signed;
	cell->parameters[ID::B_SIGNED] = b_signed;
	cell->parameters[ID::A_विस्तारः] = sig_a.आकार();
	cell->parameters[ID::B_विस्तारः] = sig_b.आकार();
	cell->parameters[ID::Y_विस्तारः] = sig_y.आकार();
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::B, sig_b);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Fa(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव RTLIL::SigSpec &sig_x, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($fa));
	cell->parameters[ID::विस्तारः_2] = sig_a.आकार();
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::B, sig_b);
	cell->समुच्चयः_पत्तनम्(ID::C, sig_c);
	cell->समुच्चयः_पत्तनम्(ID::X, sig_x);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_खण्डिका(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, RTLIL::Const विस्थापनम्, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($खण्डिका));
	cell->parameters[ID::A_विस्तारः] = sig_a.आकार();
	cell->parameters[ID::Y_विस्तारः] = sig_y.आकार();
	cell->parameters[ID::विस्थापनम्_2] = विस्थापनम्;
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Concat(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($concat));
	cell->parameters[ID::A_विस्तारः] = sig_a.आकार();
	cell->parameters[ID::B_विस्तारः] = sig_b.आकार();
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::B, sig_b);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Lut(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, RTLIL::Const lut, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::विस्तारः_2] = sig_a.आकार();
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Tribuf(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($tribuf));
	cell->parameters[ID::विस्तारः_2] = sig_a.आकार();
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Assert(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($प्रतिज्ञा));
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Assume(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($assume));
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_जीवितम्(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($जीवितम्));
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Fair(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($fair));
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Cover(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($cover));
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Equiv(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($equiv));
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::B, sig_b);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Sr(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr, ध्रुव RTLIL::SigSpec &sig_q, तार्किक समुच्चयः_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($sr));
	cell->parameters[ID::समुच्चयः_POLARITY] = समुच्चयः_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::समुच्चयः_2, sig_समुच्चयः);
	cell->समुच्चयः_पत्तनम्(ID::CLR, sig_clr);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Ff(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($ff));
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Dff(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::CLK, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Dffe(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक en_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($dffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::CLK, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Dffsr(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक समुच्चयः_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($dffsr));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::समुच्चयः_POLARITY] = समुच्चयः_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::CLK, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::समुच्चयः_2, sig_समुच्चयः);
	cell->समुच्चयः_पत्तनम्(ID::CLR, sig_clr);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Dffsre(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक en_polarity, तार्किक समुच्चयः_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($dffsre));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::समुच्चयः_POLARITY] = समुच्चयः_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::CLK, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::समुच्चयः_2, sig_समुच्चयः);
	cell->समुच्चयः_पत्तनम्(ID::CLR, sig_clr);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Adff(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_मानम्, तार्किक clk_polarity, तार्किक arst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_मानम्] = arst_मानम्;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::CLK, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::ARST, sig_arst);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Adffe(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_मानम्, तार्किक clk_polarity, तार्किक en_polarity, तार्किक arst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_मानम्] = arst_मानम्;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::CLK, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::ARST, sig_arst);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Aldff(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		ध्रुव RTLIL::SigSpec &sig_ad, तार्किक clk_polarity, तार्किक aload_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($aldff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::CLK, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::ALOAD, sig_aload);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::AD, sig_ad);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Aldffe(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		ध्रुव RTLIL::SigSpec &sig_ad, तार्किक clk_polarity, तार्किक en_polarity, तार्किक aload_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($aldffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::CLK, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::ALOAD, sig_aload);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::AD, sig_ad);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Sdff(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_मानम्, तार्किक clk_polarity, तार्किक srst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_मानम्] = srst_मानम्;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::CLK, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::SRST, sig_srst);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Sdffe(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_मानम्, तार्किक clk_polarity, तार्किक en_polarity, तार्किक srst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_मानम्] = srst_मानम्;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::CLK, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::SRST, sig_srst);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Sdffce(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_मानम्, तार्किक clk_polarity, तार्किक en_polarity, तार्किक srst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_मानम्] = srst_मानम्;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::CLK, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::SRST, sig_srst);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Dlatch(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक en_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Adlatch(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_मानम्, तार्किक en_polarity, तार्किक arst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_मानम्] = arst_मानम्;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::ARST, sig_arst);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Dlatchsr(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक en_polarity, तार्किक समुच्चयः_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($dlatchsr));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::समुच्चयः_POLARITY] = समुच्चयः_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::EN, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::समुच्चयः_2, sig_समुच्चयः);
	cell->समुच्चयः_पत्तनम्(ID::CLR, sig_clr);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Sr_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr,
		ध्रुव RTLIL::SigSpec &sig_q, तार्किक समुच्चयः_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_SR_%c%c_", समुच्चयः_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->समुच्चयः_पत्तनम्(ID::S, sig_समुच्चयः);
	cell->समुच्चयः_पत्तनम्(ID::R, sig_clr);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Ff_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($_FF_));
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Dff_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->समुच्चयः_पत्तनम्(ID::C, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Dffe_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक en_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->समुच्चयः_पत्तनम्(ID::C, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::E, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Dffsr_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक समुच्चयः_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', समुच्चयः_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->समुच्चयः_पत्तनम्(ID::C, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::S, sig_समुच्चयः);
	cell->समुच्चयः_पत्तनम्(ID::R, sig_clr);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Dffsre_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक en_polarity, तार्किक समुच्चयः_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', समुच्चयः_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->समुच्चयः_पत्तनम्(ID::C, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::S, sig_समुच्चयः);
	cell->समुच्चयः_पत्तनम्(ID::R, sig_clr);
	cell->समुच्चयः_पत्तनम्(ID::E, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Adff_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		तार्किक arst_मानम्, तार्किक clk_polarity, तार्किक arst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_मानम् ? '1' : '0'));
	cell->समुच्चयः_पत्तनम्(ID::C, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::R, sig_arst);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Adffe_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		तार्किक arst_मानम्, तार्किक clk_polarity, तार्किक en_polarity, तार्किक arst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_मानम् ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->समुच्चयः_पत्तनम्(ID::C, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::R, sig_arst);
	cell->समुच्चयः_पत्तनम्(ID::E, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Aldff_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		ध्रुव RTLIL::SigSpec &sig_ad, तार्किक clk_polarity, तार्किक aload_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->समुच्चयः_पत्तनम्(ID::C, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::L, sig_aload);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::AD, sig_ad);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Aldffe_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		ध्रुव RTLIL::SigSpec &sig_ad, तार्किक clk_polarity, तार्किक en_polarity, तार्किक aload_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->समुच्चयः_पत्तनम्(ID::C, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::L, sig_aload);
	cell->समुच्चयः_पत्तनम्(ID::E, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::AD, sig_ad);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Sdff_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		तार्किक srst_मानम्, तार्किक clk_polarity, तार्किक srst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_मानम् ? '1' : '0'));
	cell->समुच्चयः_पत्तनम्(ID::C, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::R, sig_srst);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Sdffe_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		तार्किक srst_मानम्, तार्किक clk_polarity, तार्किक en_polarity, तार्किक srst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_मानम् ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->समुच्चयः_पत्तनम्(ID::C, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::R, sig_srst);
	cell->समुच्चयः_पत्तनम्(ID::E, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Sdffce_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		तार्किक srst_मानम्, तार्किक clk_polarity, तार्किक en_polarity, तार्किक srst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_मानम् ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->समुच्चयः_पत्तनम्(ID::C, sig_clk);
	cell->समुच्चयः_पत्तनम्(ID::R, sig_srst);
	cell->समुच्चयः_पत्तनम्(ID::E, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Dlatch_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक en_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->समुच्चयः_पत्तनम्(ID::E, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Adlatch_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
		तार्किक arst_मानम्, तार्किक en_polarity, तार्किक arst_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_मानम् ? '1' : '0'));
	cell->समुच्चयः_पत्तनम्(ID::E, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::R, sig_arst);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Dlatchsr_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, तार्किक en_polarity, तार्किक समुच्चयः_polarity, तार्किक clr_polarity, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', समुच्चयः_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->समुच्चयः_पत्तनम्(ID::E, sig_en);
	cell->समुच्चयः_पत्तनम्(ID::S, sig_समुच्चयः);
	cell->समुच्चयः_पत्तनम्(ID::R, sig_clr);
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Anyinit(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($anyinit));
	cell->parameters[ID::विस्तारः_2] = sig_q.आकार();
	cell->समुच्चयः_पत्तनम्(ID::D, sig_d);
	cell->समुच्चयः_पत्तनम्(ID::Q, sig_q);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, पूर्णाङ्क विस्तारः, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = सङ्कलय_Wire(NEW_ID, विस्तारः);
	Cell *cell = सङ्कलय_Cell(name, ID($anyconst));
	cell->समुच्चयः_Param(ID::विस्तारः_2, विस्तारः);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, पूर्णाङ्क विस्तारः, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = सङ्कलय_Wire(NEW_ID, विस्तारः);
	Cell *cell = सङ्कलय_Cell(name, ID($anyseq));
	cell->समुच्चयः_Param(ID::विस्तारः_2, विस्तारः);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, पूर्णाङ्क विस्तारः, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = सङ्कलय_Wire(NEW_ID, विस्तारः);
	Cell *cell = सङ्कलय_Cell(name, ID($allconst));
	cell->समुच्चयः_Param(ID::विस्तारः_2, विस्तारः);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, पूर्णाङ्क विस्तारः, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = सङ्कलय_Wire(NEW_ID, विस्तारः);
	Cell *cell = सङ्कलय_Cell(name, ID($allseq));
	cell->समुच्चयः_Param(ID::विस्तारः_2, विस्तारः);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = सङ्कलय_Wire(NEW_ID);
	Cell *cell = सङ्कलय_Cell(name, ID($initstate));
	cell->समुच्चयः_पत्तनम्(ID::Y, sig);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::SigSpec RTLIL::Module::समुच्चयः_चिह्नकम्_2(RTLIL::IdString name, ध्रुव std::string &चिह्नकम्, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = सङ्कलय_Wire(NEW_ID, sig_a.आकार());
	Cell *cell = सङ्कलय_Cell(name, ID($समुच्चयः_चिह्नकम्));
	cell->parameters[ID::विस्तारः_2] = sig_a.आकार();
	cell->parameters[ID::चिह्नकम्_2] = चिह्नकम्;
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::समुच्चयः_2, sig_s);
	cell->समुच्चयः_पत्तनम्(ID::CLR, sig_c);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_समुच्चयः_चिह्नकम्(RTLIL::IdString name, ध्रुव std::string &चिह्नकम्, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src)
{
	Cell *cell = सङ्कलय_Cell(name, ID($समुच्चयः_चिह्नकम्));
	cell->parameters[ID::विस्तारः_2] = sig_a.आकार();
	cell->parameters[ID::चिह्नकम्_2] = चिह्नकम्;
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::समुच्चयः_2, sig_s);
	cell->समुच्चयः_पत्तनम्(ID::CLR, sig_c);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig_y);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::SigSpec RTLIL::Module::Get_चिह्नकम्(RTLIL::IdString name, ध्रुव std::string &चिह्नकम्, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = सङ्कलय_Wire(NEW_ID, sig_a.आकार());
	Cell *cell = सङ्कलय_Cell(name, ID($get_चिह्नकम्));
	cell->parameters[ID::विस्तारः_2] = sig_a.आकार();
	cell->parameters[ID::चिह्नकम्_2] = चिह्नकम्;
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::Cell* RTLIL::Module::सङ्कलय_Overwrite_चिह्नकम्(RTLIL::IdString name, ध्रुव std::string &चिह्नकम्, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव std::string &src)
{
	RTLIL::Cell *cell = सङ्कलय_Cell(name, ID($overwrite_चिह्नकम्));
	cell->parameters[ID::विस्तारः_2] = sig_a.आकार();
	cell->parameters[ID::चिह्नकम्_2] = चिह्नकम्;
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::समुच्चयः_2, sig_s);
	cell->समुच्चयः_पत्तनम्(ID::CLR, sig_c);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम cell;
}

RTLIL::SigSpec RTLIL::Module::Original_चिह्नकम्(RTLIL::IdString name, ध्रुव std::string &चिह्नकम्, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = सङ्कलय_Wire(NEW_ID, sig_a.आकार());
	Cell *cell = सङ्कलय_Cell(name, ID($original_चिह्नकम्));
	cell->parameters[ID::विस्तारः_2] = sig_a.आकार();
	cell->parameters[ID::चिह्नकम्_2] = चिह्नकम्;
	cell->समुच्चयः_पत्तनम्(ID::A, sig_a);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_e, ध्रुव std::string &src)
{
	RTLIL::SigSpec sig = सङ्कलय_Wire(NEW_ID, sig_e.आकार());
	Cell *cell = सङ्कलय_Cell(name, ID($future_ff));
	cell->parameters[ID::विस्तारः_2] = sig_e.आकार();
	cell->समुच्चयः_पत्तनम्(ID::A, sig_e);
	cell->समुच्चयः_पत्तनम्(ID::Y, sig);
	cell->समुच्चयः_src_attribute(src);
	प्रत्यागम sig;
}

std::string RTLIL::Module::गन्तव्यम्_rtlil_str() ध्रुव
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(एतत्), design, असत्य, सत्य, असत्य, RTLIL_BACKEND::Dump_बहुलकम्::Readable);
	प्रत्यागम f.str();
}

RTLIL::Wire::Wire()
{
	स्थिर अचिह्नित पूर्णाङ्क hashidx_गणनम् = 123456789;
	hashidx_गणनम् = mkhash_xorshift(hashidx_गणनम्);
	hashidx_ = hashidx_गणनम्;

	module = शून्यसूचकः;
	विस्तारः = 1;
	start_विस्थापनम् = 0;
	पत्तनम्_id = 0;
	पत्तनम्_निवेशः = असत्य;
	पत्तनम्_output = असत्य;
	upto = असत्य;
	is_signed = असत्य;

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->निवेशय(std::pair<अचिह्नित पूर्णाङ्क, RTLIL::Wire*>(hashidx_, एतत्));
#यदि_अन्तः
}

RTLIL::Wire::~Wire()
{
#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#यदि_अन्तः
}

std::string RTLIL::Wire::गन्तव्यम्_rtlil_str() ध्रुव
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", एतत्, design(), RTLIL_BACKEND::Dump_बहुलकम्::Readable);
	प्रत्यागम f.str();
}

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
स्थिर std::map<अचिह्नित पूर्णाङ्क, RTLIL::Wire*> all_wires;
std::map<अचिह्नित पूर्णाङ्क, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(शून्य)
{
	प्रत्यागम &all_wires;
}
#यदि_अन्तः

RTLIL::स्मृतिः::स्मृतिः()
{
	स्थिर अचिह्नित पूर्णाङ्क hashidx_गणनम् = 123456789;
	hashidx_गणनम् = mkhash_xorshift(hashidx_गणनम्);
	hashidx_ = hashidx_गणनम्;

	विस्तारः = 1;
	start_विस्थापनम् = 0;
	आकार = 0;
#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	RTLIL::स्मृतिः::get_all_memorys()->निवेशय(std::pair<अचिह्नित पूर्णाङ्क, RTLIL::स्मृतिः*>(hashidx_, एतत्));
#यदि_अन्तः
}

std::string RTLIL::स्मृतिः::गन्तव्यम्_rtlil_str() ध्रुव
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_स्मृतिः(f, "", एतत्, design(), RTLIL_BACKEND::Dump_बहुलकम्::Readable);
	प्रत्यागम f.str();
}

RTLIL::Process::Process() : module(शून्यसूचकः)
{
	स्थिर अचिह्नित पूर्णाङ्क hashidx_गणनम् = 123456789;
	hashidx_गणनम् = mkhash_xorshift(hashidx_गणनम्);
	hashidx_ = hashidx_गणनम्;
}

std::string RTLIL::Process::गन्तव्यम्_rtlil_str() ध्रुव
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", एतत्, design(), RTLIL_BACKEND::Dump_बहुलकम्::Readable);
	प्रत्यागम f.str();
}

RTLIL::Cell::Cell() : module(शून्यसूचकः)
{
	स्थिर अचिह्नित पूर्णाङ्क hashidx_गणनम् = 123456789;
	hashidx_गणनम् = mkhash_xorshift(hashidx_गणनम्);
	hashidx_ = hashidx_गणनम्;

	// log("#memtrace# %p\n", this);
	memhasher();

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->निवेशय(std::pair<अचिह्नित पूर्णाङ्क, RTLIL::Cell*>(hashidx_, एतत्));
#यदि_अन्तः
}

RTLIL::Cell::~Cell()
{
#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#यदि_अन्तः
}

std::string RTLIL::Cell::गन्तव्यम्_rtlil_str() ध्रुव
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", एतत्, design(), RTLIL_BACKEND::Dump_बहुलकम्::Readable);
	प्रत्यागम f.str();
}

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
स्थिर std::map<अचिह्नित पूर्णाङ्क, RTLIL::Cell*> all_cells;
std::map<अचिह्नित पूर्णाङ्क, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(शून्य)
{
	प्रत्यागम &all_cells;
}
#यदि_अन्तः

तार्किक RTLIL::Cell::has_पत्तनम्(RTLIL::IdString portname) ध्रुव
{
	प्रत्यागम connections_.गणनम्(portname) != 0;
}

// bufnorm

ध्रुव RTLIL::SigSpec &RTLIL::Cell::get_पत्तनम्(RTLIL::IdString portname) ध्रुव
{
	प्रत्यागम connections_.at(portname);
}

ध्रुव dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() ध्रुव
{
	प्रत्यागम connections_;
}

तार्किक RTLIL::Cell::ज्ञातम्() ध्रुव
{
	यदि (yosys_celltypes.cell_ज्ञातम्(type))
		प्रत्यागम सत्य;
	यदि (module && module->design && module->design->module(type))
		प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}

तार्किक RTLIL::Cell::निवेशः(IdString portname) ध्रुव
{
	यदि (yosys_celltypes.cell_ज्ञातम्(type))
		प्रत्यागम yosys_celltypes.cell_निवेशः(type.ref(), portname);
	यदि (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : शून्यसूचकः;
		प्रत्यागम w && w->पत्तनम्_निवेशः;
	}
	प्रत्यागम असत्य;
}

तार्किक RTLIL::Cell::output(RTLIL::IdString portname) ध्रुव
{
	यदि (yosys_celltypes.cell_ज्ञातम्(type))
		प्रत्यागम yosys_celltypes.cell_output(type, portname);
	यदि (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : शून्यसूचकः;
		प्रत्यागम w && w->पत्तनम्_output;
	}
	प्रत्यागम असत्य;
}

RTLIL::पत्तनम्_Dir RTLIL::Cell::पत्तनम्_dir(RTLIL::IdString portname) ध्रुव
{
	यदि (yosys_celltypes.cell_ज्ञातम्(type.ref()))
		प्रत्यागम yosys_celltypes.cell_पत्तनम्_dir(type, portname);
	यदि (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		यदि (m == शून्यसूचकः)
			प्रत्यागम पत्तनम्_Dir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		यदि (w == शून्यसूचकः)
			प्रत्यागम पत्तनम्_Dir::PD_UNKNOWN;
		प्रत्यागम पत्तनम्_Dir(w->पत्तनम्_निवेशः + w->पत्तनम्_output * 2);
	}
	प्रत्यागम पत्तनम्_Dir::PD_UNKNOWN;
}

तार्किक RTLIL::Cell::hasParam(RTLIL::IdString paramname) ध्रुव
{
	प्रत्यागम parameters.गणनम्(paramname) != 0;
}

शून्य RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

शून्य RTLIL::Cell::समुच्चयः_Param(RTLIL::IdString paramname, RTLIL::Const मानम्)
{
	parameters[paramname] = std::move(मानम्);
}

ध्रुव RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) ध्रुव
{
	ध्रुव स्वचालित &it = parameters.find(paramname);
	यदि (it != parameters.अन्तः())
		प्रत्यागम it->second;
	यदि (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		यदि (m)
			प्रत्यागम m->parameter_default_मानानि.at(paramname);
	}
	क्षिप std::बहिर्गमनम्_सम्बन्धि_range("Cell::getParam()");
}

शून्य RTLIL::Cell::क्रमय()
{
	connections_.क्रमय(क्रमय_by_id_str(module->twines()));
	parameters.क्रमय(क्रमय_by_id_str(module->twines()));
	गुणाः.क्रमय(क्रमय_by_id_str(module->twines()));
}

शून्य RTLIL::Cell::check()
{
#यदि_अपरिभाषितम् NDEBUG
	आन्तरिकम्_Cell_Checker checker(रिक्त, एतत्);
	checker.check();
#यदि_अन्तः
}

शून्य RTLIL::Cell::fixup_parameters(तार्किक समुच्चयः_a_signed, तार्किक समुच्चयः_b_signed)
{
	यदि (type.is_लौकिकम्() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		प्रत्यागम;

	यदि (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::विस्तारः_2] = Get_आकार(connections_[ID::Y]);
		यदि (type.अन्तर्(ID($pmux), ID($bmux)))
			parameters[ID::S_विस्तारः] = Get_आकार(connections_[ID::S]);
		check();
		प्रत्यागम;
	}

	यदि (type == ID($demux)) {
		parameters[ID::विस्तारः_2] = Get_आकार(connections_[ID::A]);
		parameters[ID::S_विस्तारः] = Get_आकार(connections_[ID::S]);
		check();
		प्रत्यागम;
	}

	यदि (type == ID($lut) || type == ID($sop)) {
		parameters[ID::विस्तारः_2] = Get_आकार(connections_[ID::A]);
		प्रत्यागम;
	}

	यदि (type == ID($fa)) {
		parameters[ID::विस्तारः_2] = Get_आकार(connections_[ID::Y]);
		प्रत्यागम;
	}

	यदि (type == ID($lcu)) {
		parameters[ID::विस्तारः_2] = Get_आकार(connections_[ID::CO]);
		प्रत्यागम;
	}

	यदि (type == ID($macc_v2)) {
		parameters[ID::Y_विस्तारः] = Get_आकार(connections_[ID::Y]);
		प्रत्यागम;
	}

	तार्किक signedness_ab = !type.अन्तर्(ID($खण्डिका), ID($concat), ID($macc));

	यदि (connections_.गणनम्(ID::A)) {
		यदि (signedness_ab) {
			यदि (समुच्चयः_a_signed)
				parameters[ID::A_SIGNED] = सत्य;
			अन्यथा यदि (parameters.गणनम्(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = असत्य;
		}
		parameters[ID::A_विस्तारः] = Get_आकार(connections_[ID::A]);
	}

	यदि (connections_.गणनम्(ID::B)) {
		यदि (signedness_ab) {
			यदि (समुच्चयः_b_signed)
				parameters[ID::B_SIGNED] = सत्य;
			अन्यथा यदि (parameters.गणनम्(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = असत्य;
		}
		parameters[ID::B_विस्तारः] = Get_आकार(connections_[ID::B]);
	}

	यदि (connections_.गणनम्(ID::Y) && type != ID($concat))
		parameters[ID::Y_विस्तारः] = Get_आकार(connections_[ID::Y]);

	यदि (connections_.गणनम्(ID::Q))
		parameters[ID::विस्तारः_2] = Get_आकार(connections_[ID::Q]);

	check();
}

तार्किक RTLIL::Cell::has_keep_attr() ध्रुव {
	प्रत्यागम get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

तार्किक RTLIL::Cell::has_memid() ध्रुव
{
	प्रत्यागम type.अन्तर्(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

तार्किक RTLIL::Cell::is_mem_cell() ध्रुव
{
	प्रत्यागम type.अन्तर्(ID($mem), ID($mem_v2)) || has_memid();
}

तार्किक RTLIL::Cell::is_builtin_ff() ध्रुव {
	प्रत्यागम StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(ध्रुव RTLIL::SigBit &bit)
{
	wire = bit.wire;
	विस्थापनम् = 0;
	यदि (wire == रिक्त)
		दत्त = {bit.दत्त};
	अन्यथा
		विस्थापनम् = bit.विस्थापनम्;
	विस्तारः = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(पूर्णाङ्क विस्थापनम्, पूर्णाङ्क दीर्घता_2) ध्रुव
{
	log_assert(विस्थापनम् >= 0);
	log_assert(दीर्घता_2 >= 0);
	log_assert(विस्थापनम् + दीर्घता_2 <= विस्तारः);
	RTLIL::SigChunk ret;
	यदि (wire) {
		ret.wire = wire;
		ret.विस्थापनम् = एतत्->विस्थापनम् + विस्थापनम्;
		ret.विस्तारः = दीर्घता_2;
	} अन्यथा {
		प्रति (पूर्णाङ्क i = 0; i < दीर्घता_2; i++)
			ret.दत्त.प्रक्षिप_पृष्ठभागः(दत्त[विस्थापनम्+i]);
		ret.विस्तारः = दीर्घता_2;
	}
	प्रत्यागम ret;
}

RTLIL::SigBit RTLIL::SigChunk::संक्रियकः[](पूर्णाङ्क विस्थापनम्) ध्रुव
{
	log_assert(विस्थापनम् >= 0);
	log_assert(विस्थापनम् < विस्तारः);
	RTLIL::SigBit ret;
	यदि (wire) {
		ret.wire = wire;
		ret.विस्थापनम् = एतत्->विस्थापनम् + विस्थापनम्;
	} अन्यथा {
		ret.दत्त = दत्त[विस्थापनम्];
	}
	प्रत्यागम ret;
}

तार्किक RTLIL::SigChunk::संक्रियकः <(ध्रुव RTLIL::SigChunk &other) ध्रुव
{
	यदि (wire && other.wire)
		यदि (wire->name != other.wire->name)
			प्रत्यागम wire->name < other.wire->name;

	यदि (wire != other.wire)
		प्रत्यागम wire < other.wire;

	यदि (विस्थापनम् != other.विस्थापनम्)
		प्रत्यागम विस्थापनम् < other.विस्थापनम्;

	यदि (विस्तारः != other.विस्तारः)
		प्रत्यागम विस्तारः < other.विस्तारः;

	प्रत्यागम दत्त < other.दत्त;
}

तार्किक RTLIL::SigChunk::संक्रियकः ==(ध्रुव RTLIL::SigChunk &other) ध्रुव
{
	प्रत्यागम wire == other.wire && विस्तारः == other.विस्तारः && विस्थापनम् == other.विस्थापनम् && दत्त == other.दत्त;
}

तार्किक RTLIL::SigChunk::संक्रियकः !=(ध्रुव RTLIL::SigChunk &other) ध्रुव
{
	यदि (*एतत् == other)
		प्रत्यागम असत्य;
	प्रत्यागम सत्य;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_रिक्तानि_द्वयङ्काः();
	log_assert(parts.आकार() > 0);
	स्वचालित ie = parts.begin();
	स्वचालित it = ie + parts.आकार() - 1;
	यावत् (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(ध्रुव RTLIL::Const &मानम्)
{
	यदि (Get_आकार(मानम्) != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(मानम्);
	} अन्यथा {
		init_रिक्तानि_द्वयङ्काः();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&मानम्)
{
	यदि (Get_आकार(मानम्) != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(मानम्);
	} अन्यथा {
		init_रिक्तानि_द्वयङ्काः();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव RTLIL::SigChunk &chunk)
{
	यदि (chunk.विस्तारः != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(chunk);
	} अन्यथा {
		init_रिक्तानि_द्वयङ्काः();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	यदि (chunk.विस्तारः != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(chunk);
	} अन्यथा {
		init_रिक्तानि_द्वयङ्काः();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	यदि (wire->विस्तारः != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(wire);
	} अन्यथा {
		init_रिक्तानि_द्वयङ्काः();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, पूर्णाङ्क विस्थापनम्, पूर्णाङ्क विस्तारः)
{
	यदि (विस्तारः != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(wire, विस्थापनम्, विस्तारः);
	} अन्यथा {
		init_रिक्तानि_द्वयङ्काः();
	}
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव std::string &str)
{
	यदि (str.आकार() != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(str);
	} अन्यथा {
		init_रिक्तानि_द्वयङ्काः();
	}
	check();
}

RTLIL::SigSpec::SigSpec(पूर्णाङ्क val, पूर्णाङ्क विस्तारः)
{
	यदि (विस्तारः != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(val, विस्तारः);
	} अन्यथा
		init_रिक्तानि_द्वयङ्काः();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, पूर्णाङ्क विस्तारः)
{
	यदि (विस्तारः != 0) {
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(bit, विस्तारः);
	} अन्यथा
		init_रिक्तानि_द्वयङ्काः();
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव RTLIL::SigBit &bit, पूर्णाङ्क विस्तारः)
{
	यदि (विस्तारः != 0) {
		यदि (bit.wire == रिक्त) {
			rep_ = CHUNK;
			नूतन (&chunk_) RTLIL::SigChunk(bit.दत्त, विस्तारः);
		} अन्यथा यदि (विस्तारः == 1) {
			rep_ = CHUNK;
			नूतन (&chunk_) RTLIL::SigChunk(bit);
		} अन्यथा {
			init_रिक्तानि_द्वयङ्काः();
			द्वयङ्काः_2.reserve(विस्तारः);
			प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++)
				द्वयङ्काः_2.प्रक्षिप_पृष्ठभागः(bit);
		}
	} अन्यथा
		init_रिक्तानि_द्वयङ्काः();
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव std::सदिशम्<RTLIL::SigChunk> &chunks)
{
	init_रिक्तानि_द्वयङ्काः();
	प्रति (ध्रुव स्वचालित &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव std::सदिशम्<RTLIL::SigBit> &द्वयङ्काः)
{
	init_रिक्तानि_द्वयङ्काः();
	प्रति (ध्रुव स्वचालित &bit : द्वयङ्काः)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव pool<RTLIL::SigBit> &द्वयङ्काः)
{
	init_रिक्तानि_द्वयङ्काः();
	प्रति (ध्रुव स्वचालित &bit : द्वयङ्काः)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(ध्रुव std::समुच्चयः<RTLIL::SigBit> &द्वयङ्काः)
{
	init_रिक्तानि_द्वयङ्काः();
	प्रति (ध्रुव स्वचालित &bit : द्वयङ्काः)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(तार्किक bit)
{
	rep_ = CHUNK;
	नूतन (&chunk_) RTLIL::SigChunk(bit ? RTLIL::S1 : RTLIL::S0);
	check();
}

शून्य RTLIL::SigSpec::Chunks::const_iterator::अग्र_chunk_द्वयङ्काः()
{
	पूर्णाङ्क द्वयङ्काः_आकार = Get_आकार(spec.द्वयङ्काः_2);
	यदि (bit_सूचकांक >= द्वयङ्काः_आकार)
		प्रत्यागम;
	पूर्णाङ्क i = bit_सूचकांक;
	ध्रुव SigBit &bit = spec.द्वयङ्काः_2[i++];
	chunk.wire = bit.wire;
	chunk.दत्त.clear();
	यदि (bit.is_wire()) {
		chunk.विस्थापनम् = bit.विस्थापनम्;
		यावत् (i < द्वयङ्काः_आकार && spec.द्वयङ्काः_2[i].wire == bit.wire &&
			spec.द्वयङ्काः_2[i].विस्थापनम् == bit.विस्थापनम् + i - bit_सूचकांक)
			++i;
	} अन्यथा {
		chunk.विस्थापनम् = 0;
		chunk.दत्त.प्रक्षिप_पृष्ठभागः(bit.दत्त);
		यावत् (i < द्वयङ्काः_आकार && !spec.द्वयङ्काः_2[i].is_wire()) {
			chunk.दत्त.प्रक्षिप_पृष्ठभागः(spec.द्वयङ्काः_2[i].दत्त);
			++i;
		}
	}
	chunk.विस्तारः = i - bit_सूचकांक;
}

शून्य RTLIL::SigSpec::unpack()
{
	यदि (rep_ == द्वयङ्काः_3)
		प्रत्यागम;

	std::सदिशम्<RTLIL::SigBit> द्वयङ्काः;
	द्वयङ्काः.reserve(chunk_.विस्तारः);
	प्रति (पूर्णाङ्क i = 0; i < chunk_.विस्तारः; i++)
		द्वयङ्काः.emplace_पृष्ठभागः(chunk_, i);

	chunk_.~SigChunk();
	rep_ = द्वयङ्काः_3;
	नूतन (&द्वयङ्काः_2) std::सदिशम्<RTLIL::SigBit>(std::move(द्वयङ्काः));
}

शून्य RTLIL::SigSpec::try_repack()
{
	यदि (rep_ != द्वयङ्काः_3)
		प्रत्यागम;

	पूर्णाङ्क द्वयङ्काः_आकार = Get_आकार(द्वयङ्काः_2);
	यदि (द्वयङ्काः_आकार == 0)
		प्रत्यागम;
	यदि (द्वयङ्काः_2[0].is_wire()) {
		प्रति (पूर्णाङ्क i = 1; i < द्वयङ्काः_आकार; i++)
			यदि (द्वयङ्काः_2[0].wire != द्वयङ्काः_2[i].wire || द्वयङ्काः_2[0].विस्थापनम् + i != द्वयङ्काः_2[i].विस्थापनम्)
				प्रत्यागम;
		SigChunk chunk(द्वयङ्काः_2[0].wire, द्वयङ्काः_2[0].विस्थापनम्, द्वयङ्काः_आकार);
		द्वयङ्काः_2.~सदिशम्();
		rep_ = CHUNK;
		नूतन (&chunk_) SigChunk(std::move(chunk));
		प्रत्यागम;
	}
	std::सदिशम्<RTLIL::State> द्वयङ्काः;
	द्वयङ्काः.reserve(द्वयङ्काः_आकार);
	द्वयङ्काः.प्रक्षिप_पृष्ठभागः(द्वयङ्काः_2[0].दत्त);
	प्रति (पूर्णाङ्क i = 1; i < द्वयङ्काः_आकार; i++) {
		यदि (द्वयङ्काः_2[i].is_wire())
			प्रत्यागम;
		द्वयङ्काः.प्रक्षिप_पृष्ठभागः(द्वयङ्काः_2[i].दत्त);
	}
	द्वयङ्काः_2.~सदिशम्();
	rep_ = CHUNK;
	नूतन (&chunk_) SigChunk(std::move(द्वयङ्काः));
}

Hasher::hash_t RTLIL::SigSpec::updhash() ध्रुव
{
	Hasher h;
	प्रति (स्वचालित &c : chunks())
		यदि (c.wire == रिक्त) {
			प्रति (स्वचालित &v : c.दत्त)
				h.eat(v);
		} अन्यथा {
			h.eat(c.wire->name.ref());
			h.eat(c.विस्थापनम्);
			h.eat(c.विस्तारः);
		}
	Hasher::hash_t परिणाम = h.yield();
	यदि (परिणाम == 0)
		परिणाम = 1;
	hash_.समुच्चयः(परिणाम);
	प्रत्यागम परिणाम;
}

शून्य RTLIL::SigSpec::क्रमय()
{
	unpack();
	std::क्रमय(द्वयङ्काः_2.begin(), द्वयङ्काः_2.अन्तः());
	hash_.clear();
	try_repack();
}

शून्य RTLIL::SigSpec::क्रमय_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::सदिशम्<SigBit> unique_द्वयङ्काः = द्वयङ्काः_2;
	std::क्रमय(unique_द्वयङ्काः.begin(), unique_द्वयङ्काः.अन्तः());
	स्वचालित चरमः = std::unique(unique_द्वयङ्काः.begin(), unique_द्वयङ्काः.अन्तः());
	unique_द्वयङ्काः.erase(चरमः, unique_द्वयङ्काः.अन्तः());

	*एतत् = unique_द्वयङ्काः;
	hash_.clear();
	try_repack();
}

शून्य RTLIL::SigSpec::replace(ध्रुव RTLIL::SigSpec &आदर्शः, ध्रुव RTLIL::SigSpec &with)
{
	replace(आदर्शः, with, एतत्);
}

शून्य RTLIL::SigSpec::replace(ध्रुव RTLIL::SigSpec &आदर्शः, ध्रुव RTLIL::SigSpec &with, RTLIL::SigSpec *other) ध्रुव
{
	log_assert(other != रिक्त);
	log_assert(आकार() == other->आकार());
	log_assert(आदर्शः.आकार() == with.आकार());

	dict<RTLIL::SigBit, पूर्णाङ्क> आदर्शः_गन्तव्यम्_with;
	पूर्णाङ्क आदर्शः_आकार = आदर्शः.आकार();
	प्रति (पूर्णाङ्क i = 0; i < आदर्शः_आकार; i++) {
		SigBit आदर्शः_bit = आदर्शः[i];
		यदि (आदर्शः_bit.wire != रिक्त) {
			आदर्शः_गन्तव्यम्_with.emplace(आदर्शः_bit, i);
		}
	}

	पूर्णाङ्क this_आकार = आकार();
	तार्किक other_modified = असत्य;
	प्रति (पूर्णाङ्क j = 0; j < this_आकार; j++) {
		स्वचालित it = आदर्शः_गन्तव्यम्_with.find((*एतत्)[j]);
		यदि (it != आदर्शः_गन्तव्यम्_with.अन्तः()) {
			other_modified = सत्य;
			(*other)[j] = with[it->second];
		}
	}

	यदि (other_modified)
		other->try_repack();
	other->check();
}

शून्य RTLIL::SigSpec::replace(ध्रुव dict<RTLIL::SigBit, RTLIL::SigBit> &नियमाः)
{
	replace(नियमाः, एतत्);
}

शून्य RTLIL::SigSpec::replace(ध्रुव dict<RTLIL::SigBit, RTLIL::SigBit> &नियमाः, RTLIL::SigSpec *other) ध्रुव
{
	log_assert(other != रिक्त);
	log_assert(आकार() == other->आकार());

	यदि (नियमाः.रिक्तानि()) प्रत्यागम;

	पूर्णाङ्क this_आकार = आकार();
	तार्किक other_modified = असत्य;
	प्रति (पूर्णाङ्क i = 0; i < this_आकार; i++) {
		स्वचालित it = नियमाः.find((*एतत्)[i]);
		यदि (it != नियमाः.अन्तः()) {
			other_modified = सत्य;
			(*other)[i] = it->second;
		}
	}

	यदि (other_modified)
		other->try_repack();
	other->check();
}

शून्य RTLIL::SigSpec::replace(ध्रुव std::map<RTLIL::SigBit, RTLIL::SigBit> &नियमाः)
{
	replace(नियमाः, एतत्);
}

शून्य RTLIL::SigSpec::replace(ध्रुव std::map<RTLIL::SigBit, RTLIL::SigBit> &नियमाः, RTLIL::SigSpec *other) ध्रुव
{
	log_assert(other != रिक्त);
	log_assert(आकार() == other->आकार());

	यदि (नियमाः.रिक्तानि()) प्रत्यागम;

	पूर्णाङ्क this_आकार = आकार();
	तार्किक other_modified = असत्य;
	प्रति (पूर्णाङ्क i = 0; i < this_आकार; i++) {
		स्वचालित it = नियमाः.find((*एतत्)[i]);
		यदि (it != नियमाः.अन्तः()) {
			other_modified = सत्य;
			(*other)[i] = it->second;
		}
	}

	यदि (other_modified)
		other->try_repack();
	other->check();
}

शून्य RTLIL::SigSpec::remove(ध्रुव RTLIL::SigSpec &आदर्शः)
{
	remove2(आदर्शः, रिक्त);
}

शून्य RTLIL::SigSpec::remove(ध्रुव RTLIL::SigSpec &आदर्शः, RTLIL::SigSpec *other) ध्रुव
{
	RTLIL::SigSpec tmp = *एतत्;
	tmp.remove2(आदर्शः, other);
}

शून्य RTLIL::SigSpec::remove2(ध्रुव RTLIL::SigSpec &आदर्शः, RTLIL::SigSpec *other)
{
	unpack();
	यदि (other != रिक्त) {
		log_assert(आकार() == other->आकार());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> आदर्शः_द्वयङ्काः;
	आदर्शः_द्वयङ्काः.reserve(आदर्शः.आकार());
	प्रति (स्वचालित &bit : आदर्शः)
		यदि (bit.wire != रिक्त)
			आदर्शः_द्वयङ्काः.निवेशय(bit);

	// Compact in-place to avoid O(n^2) erase operations
	आकार_t write_idx = 0;
	प्रति (आकार_t read_idx = 0; read_idx < द्वयङ्काः_2.आकार(); read_idx++)
	{
		यदि (!(द्वयङ्काः_2[read_idx].wire != रिक्त && आदर्शः_द्वयङ्काः.गणनम्(द्वयङ्काः_2[read_idx]))) {
			यदि (write_idx != read_idx) {
				द्वयङ्काः_2[write_idx] = द्वयङ्काः_2[read_idx];
				यदि (other != रिक्त)
					other->द्वयङ्काः_2[write_idx] = other->द्वयङ्काः_2[read_idx];
			}
			write_idx++;
		}
	}

	तार्किक modified = (write_idx < द्वयङ्काः_2.आकार());
	यदि (modified) {
		द्वयङ्काः_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	यदि (other != रिक्त && modified) {
		other->द्वयङ्काः_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

शून्य RTLIL::SigSpec::remove(ध्रुव pool<RTLIL::SigBit> &आदर्शः)
{
	remove2(आदर्शः, रिक्त);
}

शून्य RTLIL::SigSpec::remove(ध्रुव pool<RTLIL::SigBit> &आदर्शः, RTLIL::SigSpec *other) ध्रुव
{
	RTLIL::SigSpec tmp = *एतत्;
	tmp.remove2(आदर्शः, other);
}

शून्य RTLIL::SigSpec::remove2(ध्रुव pool<RTLIL::SigBit> &आदर्शः, RTLIL::SigSpec *other)
{
	unpack();

	यदि (other != रिक्त) {
		log_assert(आकार() == other->आकार());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	आकार_t write_idx = 0;
	प्रति (आकार_t read_idx = 0; read_idx < द्वयङ्काः_2.आकार(); read_idx++) {
		यदि (!(द्वयङ्काः_2[read_idx].wire != रिक्त && आदर्शः.गणनम्(द्वयङ्काः_2[read_idx]))) {
			यदि (write_idx != read_idx) {
				द्वयङ्काः_2[write_idx] = द्वयङ्काः_2[read_idx];
				यदि (other != रिक्त)
					other->द्वयङ्काः_2[write_idx] = other->द्वयङ्काः_2[read_idx];
			}
			write_idx++;
		}
	}

	तार्किक modified = (write_idx < द्वयङ्काः_2.आकार());
	यदि (modified) {
		द्वयङ्काः_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	यदि (other != रिक्त && modified) {
		other->द्वयङ्काः_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

शून्य RTLIL::SigSpec::remove2(ध्रुव std::समुच्चयः<RTLIL::SigBit> &आदर्शः, RTLIL::SigSpec *other)
{
	unpack();

	यदि (other != रिक्त) {
		log_assert(आकार() == other->आकार());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	आकार_t write_idx = 0;
	प्रति (आकार_t read_idx = 0; read_idx < द्वयङ्काः_2.आकार(); read_idx++) {
		यदि (!(द्वयङ्काः_2[read_idx].wire != रिक्त && आदर्शः.गणनम्(द्वयङ्काः_2[read_idx]))) {
			यदि (write_idx != read_idx) {
				द्वयङ्काः_2[write_idx] = द्वयङ्काः_2[read_idx];
				यदि (other != रिक्त)
					other->द्वयङ्काः_2[write_idx] = other->द्वयङ्काः_2[read_idx];
			}
			write_idx++;
		}
	}

	तार्किक modified = (write_idx < द्वयङ्काः_2.आकार());
	यदि (modified) {
		द्वयङ्काः_2.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	यदि (other != रिक्त && modified) {
		other->द्वयङ्काः_2.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

शून्य RTLIL::SigSpec::remove2(ध्रुव pool<RTLIL::Wire*> &आदर्शः, RTLIL::SigSpec *other)
{
	unpack();

	यदि (other != रिक्त) {
		log_assert(आकार() == other->आकार());
		other->unpack();
	}

	तार्किक modified = असत्य;
	तार्किक other_modified = असत्य;
	प्रति (पूर्णाङ्क i = Get_आकार(द्वयङ्काः_2) - 1; i >= 0; i--) {
		यदि (द्वयङ्काः_2[i].wire != रिक्त && आदर्शः.गणनम्(द्वयङ्काः_2[i].wire)) {
			modified = सत्य;
			द्वयङ्काः_2.erase(द्वयङ्काः_2.begin() + i);
			यदि (other != रिक्त) {
				other_modified = सत्य;
				other->द्वयङ्काः_2.erase(other->द्वयङ्काः_2.begin() + i);
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

RTLIL::SigSpec RTLIL::SigSpec::extract(ध्रुव RTLIL::SigSpec &आदर्शः, ध्रुव RTLIL::SigSpec *other) ध्रुव
{
	log_assert(other == रिक्त || आकार() == other->आकार());

	RTLIL::SigSpec ret;
	std::सदिशम्<RTLIL::SigBit> द्वयङ्काः_मेलनम् = गन्तव्यम्_sigbit_सदिशम्();

	प्रति (स्वचालित& आदर्शः_chunk : आदर्शः.chunks()) {
		यदि (other) {
			std::सदिशम्<RTLIL::SigBit> द्वयङ्काः_other = other->गन्तव्यम्_sigbit_सदिशम्();
			पूर्णाङ्क i = 0;
			प्रति (ध्रुव RTLIL::SigBit &bit : द्वयङ्काः_मेलनम्) {
				यदि (bit.wire &&
					bit.wire == आदर्शः_chunk.wire &&
					bit.विस्थापनम् >= आदर्शः_chunk.विस्थापनम् &&
					bit.विस्थापनम् < आदर्शः_chunk.विस्थापनम् + आदर्शः_chunk.विस्तारः)
					ret.append(द्वयङ्काः_other[i]);
				++i;
			}
		} अन्यथा {
			प्रति (ध्रुव RTLIL::SigBit &bit : द्वयङ्काः_मेलनम्)
				यदि (bit.wire &&
					bit.wire == आदर्शः_chunk.wire &&
					bit.विस्थापनम् >= आदर्शः_chunk.विस्थापनम् &&
					bit.विस्थापनम् < आदर्शः_chunk.विस्थापनम् + आदर्शः_chunk.विस्तारः)
					ret.append(bit);
		}
	}

	ret.check();
	प्रत्यागम ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(ध्रुव pool<RTLIL::SigBit> &आदर्शः, ध्रुव RTLIL::SigSpec *other) ध्रुव
{
	log_assert(other == रिक्त || आकार() == other->आकार());

	std::सदिशम्<RTLIL::SigBit> द्वयङ्काः_मेलनम् = गन्तव्यम्_sigbit_सदिशम्();
	RTLIL::SigSpec ret;

	यदि (other) {
		std::सदिशम्<RTLIL::SigBit> द्वयङ्काः_other = other->गन्तव्यम्_sigbit_सदिशम्();
		पूर्णाङ्क i = 0;
		प्रति (ध्रुव RTLIL::SigBit &bit : द्वयङ्काः_मेलनम्) {
			यदि (bit.wire && आदर्शः.गणनम्(bit))
				ret.append(द्वयङ्काः_other[i]);
			++i;
		}
	} अन्यथा {
		प्रति (ध्रुव RTLIL::SigBit &bit : द्वयङ्काः_मेलनम्)
			यदि (bit.wire && आदर्शः.गणनम्(bit))
				ret.append(bit);
	}

	ret.check();
	प्रत्यागम ret;
}

शून्य RTLIL::SigSpec::replace(पूर्णाङ्क विस्थापनम्, ध्रुव RTLIL::SigSpec &with)
{
	यदि (with.आकार() == 0)
		प्रत्यागम;

	unpack();

	log_assert(विस्थापनम् >= 0);
	log_assert(with.आकार() >= 0);
	log_assert(विस्थापनम्+with.आकार() <= आकार());

	पूर्णाङ्क i = 0;
	प्रति (ध्रुव RTLIL::SigBit &bit : with.द्वयङ्काः()) {
		द्वयङ्काः_2.at(विस्थापनम् + i) = bit;
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
			init_रिक्तानि_द्वयङ्काः();
			hash_.clear();
		}
	}
	अन्यथा
	{
		std::सदिशम्<RTLIL::SigBit> new_द्वयङ्काः;
		new_द्वयङ्काः.reserve(द्वयङ्काः_2.आकार());
		प्रति (स्वचालित &bit : द्वयङ्काः_2)
			यदि (bit.wire != रिक्त)
				new_द्वयङ्काः.प्रक्षिप_पृष्ठभागः(bit);
		यदि (Get_आकार(new_द्वयङ्काः) != Get_आकार(द्वयङ्काः_2)) {
			द्वयङ्काः_2.swap(new_द्वयङ्काः);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

शून्य RTLIL::SigSpec::remove(पूर्णाङ्क विस्थापनम्, पूर्णाङ्क दीर्घता_2)
{
	यदि (दीर्घता_2 == 0)
		प्रत्यागम;

	unpack();

	log_assert(विस्थापनम् >= 0);
	log_assert(दीर्घता_2 >= 0);
	log_assert(विस्थापनम् + दीर्घता_2 <= आकार());

	द्वयङ्काः_2.erase(द्वयङ्काः_2.begin() + विस्थापनम्, द्वयङ्काः_2.begin() + विस्थापनम् + दीर्घता_2);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(पूर्णाङ्क विस्थापनम्, पूर्णाङ्क दीर्घता_2) ध्रुव
{
	log_assert(विस्थापनम् >= 0);
	log_assert(दीर्घता_2 >= 0);
	log_assert(विस्थापनम् + दीर्घता_2 <= आकार());

	std::सदिशम्<SigBit> extracted;
	SigBit first;
	तार्किक is_packing = सत्य;
	प्रति (पूर्णाङ्क i = विस्थापनम्; i < विस्थापनम् + दीर्घता_2; i++) {
		तार्किक was_packing_before = is_packing;
		SigBit bit = (*एतत्)[i];
		यदि (i == विस्थापनम्) {
			first = bit;
			यदि (!bit.wire)
				is_packing = असत्य;
		} अन्यथा {
			यदि (bit.wire != first.wire)
				is_packing = असत्य;
			यदि (bit.wire)
				यदि (bit.विस्थापनम् != first.विस्थापनम् + (i - विस्थापनम्))
					is_packing = असत्य;
		}
		यदि (was_packing_before && !is_packing)
			प्रति (पूर्णाङ्क j = विस्थापनम्; j < i; j++)
				extracted.प्रक्षिप_पृष्ठभागः((*एतत्)[j]);
		यदि (!is_packing)
			extracted.प्रक्षिप_पृष्ठभागः((*एतत्)[i]);
	}
	यदि (is_packing)
		प्रत्यागम SigChunk(first.wire, first.विस्थापनम्, दीर्घता_2);

	प्रत्यागम extracted;
}

शून्य RTLIL::SigSpec::rewrite_wires(std::function<शून्य(RTLIL::Wire*& wire)> rewrite)
{
	यदि (rep_ == CHUNK) {
		यदि (chunk_.wire != शून्यसूचकः)
			rewrite(chunk_.wire);
		प्रत्यागम;
	}

	std::सदिशम्<RTLIL::SigBit> new_द्वयङ्काः;
	प्रति (ध्रुव RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		यदि (c.wire != शून्यसूचकः)
			rewrite(c.wire);
		प्रति (पूर्णाङ्क i = 0; i < c.विस्तारः; i++)
			new_द्वयङ्काः.emplace_पृष्ठभागः(c, i);
	}
	द्वयङ्काः_2 = std::move(new_द्वयङ्काः);
	hash_.clear();
}

शून्य RTLIL::SigSpec::append(ध्रुव RTLIL::SigSpec &सङ्केत)
{
	यदि (सङ्केत.आकार() == 0)
		प्रत्यागम;

	यदि (आकार() == 0) {
		*एतत् = सङ्केत;
		प्रत्यागम;
	}

	hash_.clear();
	यदि (rep_ == CHUNK && सङ्केत.rep_ == CHUNK && chunk_.wire == सङ्केत.chunk_.wire) {
		यदि (chunk_.wire == रिक्त) {
			chunk_.दत्त.निवेशय(chunk_.दत्त.अन्तः(), सङ्केत.chunk_.दत्त.begin(), सङ्केत.chunk_.दत्त.अन्तः());
			chunk_.विस्तारः = Get_आकार(chunk_.दत्त);
			प्रत्यागम;
		}
		यदि (chunk_.विस्थापनम् + chunk_.विस्तारः == सङ्केत.chunk_.विस्थापनम्) {
			chunk_.विस्तारः += सङ्केत.chunk_.विस्तारः;
			प्रत्यागम;
		}
	}

	unpack();
	प्रति (ध्रुव SigBit &bit : सङ्केत.द्वयङ्काः())
		द्वयङ्काः_2.प्रक्षिप_पृष्ठभागः(bit);
	check();
}

शून्य RTLIL::SigSpec::append(ध्रुव RTLIL::SigBit &bit)
{
	hash_.clear();

	यदि (आकार() == 0) {
		destroy();
		rep_ = CHUNK;
		नूतन (&chunk_) RTLIL::SigChunk(bit);
		प्रत्यागम;
	}

	यदि (rep_ == CHUNK && chunk_.wire == bit.wire) {
		यदि (chunk_.wire == रिक्त) {
			chunk_.दत्त.प्रक्षिप_पृष्ठभागः(bit.दत्त);
			chunk_.विस्तारः++;
			प्रत्यागम;
		}
		यदि (chunk_.विस्थापनम् + chunk_.विस्तारः == bit.विस्थापनम्) {
			chunk_.विस्तारः++;
			प्रत्यागम;
		}
	}

	unpack();

	द्वयङ्काः_2.प्रक्षिप_पृष्ठभागः(bit);
	check();
}

शून्य RTLIL::SigSpec::extend_u0(पूर्णाङ्क विस्तारः, तार्किक is_signed)
{
	यदि (आकार() > विस्तारः)
		remove(विस्तारः, आकार() - विस्तारः);

	यदि (आकार() < विस्तारः) {
		RTLIL::SigBit padding = आकार() > 0 ? (*एतत्)[आकार() - 1] : RTLIL::State::Sx;
		यदि (!is_signed)
			padding = RTLIL::State::S0;
		यावत् (आकार() < विस्तारः)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(पूर्णाङ्क अङ्कः) ध्रुव
{
	RTLIL::SigSpec sig;
	प्रति (पूर्णाङ्क i = 0; i < अङ्कः; i++)
		sig.append(*एतत्);
	प्रत्यागम sig;
}

#यदि_अपरिभाषितम् NDEBUG
शून्य RTLIL::SigSpec::check(ध्रुव Module *mod) ध्रुव
{
	यदि (rep_ == CHUNK)
	{
		log_assert(chunk_.विस्तारः != 0);
		यदि (chunk_.wire == रिक्त) {
			log_assert(chunk_.विस्थापनम् == 0);
			log_assert(chunk_.दत्त.आकार() == (आकार_t)chunk_.विस्तारः);
		} अन्यथा {
			log_assert(chunk_.विस्थापनम् >= 0);
			log_assert(chunk_.विस्तारः >= 0);
			log_assert(chunk_.विस्थापनम् + chunk_.विस्तारः <= chunk_.wire->विस्तारः);
			log_assert(chunk_.दत्त.आकार() == 0);
			यदि (mod != शून्यसूचकः)
				log_assert(chunk_.wire->module == mod);
		}
	}
	अन्यथा यदि (आकार() <= 64)
	{
		यदि (mod != शून्यसूचकः) {
			प्रति (ध्रुव RTLIL::SigBit &bit : द्वयङ्काः_2)
				यदि (bit.wire != शून्यसूचकः)
					log_assert(bit.wire->module == mod);
		}
	}
}
#यदि_अन्तः

तार्किक RTLIL::SigSpec::संक्रियकः <(ध्रुव RTLIL::SigSpec &other) ध्रुव
{
	यदि (एतत् == &other)
		प्रत्यागम असत्य;

	यदि (आकार() != other.आकार())
		प्रत्यागम आकार() < other.आकार();

	स्वचालित other_it = other.chunks().begin();
	प्रति (ध्रुव SigChunk &c : chunks()) {
		यदि (c != *other_it)
			प्रत्यागम c < *other_it;
		++other_it;
	}

	प्रत्यागम असत्य;
}

तार्किक RTLIL::SigSpec::संक्रियकः ==(ध्रुव RTLIL::SigSpec &other) ध्रुव
{
	यदि (एतत् == &other)
		प्रत्यागम सत्य;

	यदि (आकार() != other.आकार())
		प्रत्यागम असत्य;

	स्वचालित other_it = other.chunks().begin();
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
	स्वचालित it = cs.begin();
	यदि (it == cs.अन्तः())
		प्रत्यागम असत्य;
	ध्रुव RTLIL::SigChunk &chunk = *it;
	प्रत्यागम chunk.wire && chunk.wire->विस्तारः == आकार() && ++it == cs.अन्तः();
}

तार्किक RTLIL::SigSpec::is_chunk() ध्रुव
{
	Chunks cs = chunks();
	स्वचालित it = cs.begin();
	यदि (it == cs.अन्तः())
		प्रत्यागम असत्य;
	प्रत्यागम ++it == cs.अन्तः();
}

तार्किक RTLIL::SigSpec::ज्ञातम्_driver() ध्रुव
{
	प्रति (स्वचालित &chunk : chunks())
		यदि (chunk.is_wire() && !chunk.wire->ज्ञातम्_driver())
			प्रत्यागम असत्य;
	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_const() ध्रुव
{
	प्रति (स्वचालित &chunk : chunks())
		यदि (chunk.विस्तारः > 0 && chunk.wire != रिक्त)
			प्रत्यागम असत्य;
	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_zero() ध्रुव
{
	प्रति (स्वचालित &chunk : chunks()) {
		यदि (chunk.विस्तारः > 0 && chunk.wire != रिक्त)
			प्रत्यागम असत्य;
		प्रति (RTLIL::State d : chunk.दत्त)
			यदि (d != RTLIL::State::S0)
				प्रत्यागम असत्य;
	}
	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_ones() ध्रुव
{
	प्रति (स्वचालित &chunk : chunks()) {
		यदि (chunk.विस्तारः > 0 && chunk.wire != रिक्त)
			प्रत्यागम असत्य;
		प्रति (RTLIL::State d : chunk.दत्त)
			यदि (d != RTLIL::State::S1)
				प्रत्यागम असत्य;
	}
	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_def() ध्रुव
{
	प्रति (स्वचालित &chunk : chunks()) {
		यदि (chunk.विस्तारः > 0 && chunk.wire != रिक्त)
			प्रत्यागम असत्य;
		प्रति (RTLIL::State d : chunk.दत्त)
			यदि (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				प्रत्यागम असत्य;
	}
	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_undef() ध्रुव
{
	प्रति (स्वचालित &chunk : chunks()) {
		यदि (chunk.विस्तारः > 0 && chunk.wire != रिक्त)
			प्रत्यागम असत्य;
		प्रति (RTLIL::State d : chunk.दत्त)
			यदि (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				प्रत्यागम असत्य;
	}
	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::has_const() ध्रुव
{
	प्रति (स्वचालित &chunk : chunks())
		यदि (chunk.विस्तारः > 0 && chunk.wire == रिक्त)
			प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}

तार्किक RTLIL::SigSpec::has_const(State state) ध्रुव
{
	प्रति (स्वचालित &chunk : chunks())
		यदि (chunk.विस्तारः > 0 && chunk.wire == रिक्त && std::find(chunk.दत्त.begin(), chunk.दत्त.अन्तः(), state) != chunk.दत्त.अन्तः())
			प्रत्यागम सत्य;
	प्रत्यागम असत्य;
}


तार्किक RTLIL::SigSpec::has_marked_द्वयङ्काः() ध्रुव
{
	प्रति (स्वचालित &chunk : chunks())
		यदि (chunk.विस्तारः > 0 && chunk.wire == रिक्त) {
			प्रति (RTLIL::State d : chunk.दत्त)
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
	log_assert(c.has_मानम्());
	प्रत्यागम c->as_bool();
}

पूर्णाङ्क RTLIL::SigSpec::as_int(तार्किक is_signed) ध्रुव
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मानम्());
	प्रत्यागम c->as_int(is_signed);
}

तार्किक RTLIL::SigSpec::convertible_गन्तव्यम्_int(तार्किक is_signed) ध्रुव
{
	std::optional<RTLIL::Const> c = try_as_const();
	यदि (!c.has_मानम्())
		प्रत्यागम असत्य;
	प्रत्यागम c->convertible_गन्तव्यम्_int(is_signed);
}

std::optional<पूर्णाङ्क> RTLIL::SigSpec::try_as_int(तार्किक is_signed) ध्रुव
{
	std::optional<RTLIL::Const> c = try_as_const();
	यदि (!c.has_मानम्())
		प्रत्यागम std::nullopt;
	प्रत्यागम c->try_as_int(is_signed);
}

पूर्णाङ्क RTLIL::SigSpec::as_int_saturating(तार्किक is_signed) ध्रुव
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मानम्());
	प्रत्यागम c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() ध्रुव
{
	std::string str;
	str.reserve(आकार());
	std::सदिशम्<RTLIL::SigChunk> chunks = *एतत्;
	प्रति (आकार_t i = chunks.आकार(); i > 0; i--) {
		ध्रुव RTLIL::SigChunk &chunk = chunks[i-1];
		यदि (chunk.wire != रिक्त)
			str.append(chunk.विस्तारः, '?');
		अन्यथा
			str += RTLIL::Const(chunk.दत्त).as_string();
	}
	प्रत्यागम str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() ध्रुव
{
	Chunks cs = chunks();
	स्वचालित it = cs.begin();
	यदि (it == cs.अन्तः())
		प्रत्यागम RTLIL::Const();
	SigChunk chunk = *it;
	यदि (chunk.wire != रिक्त || ++it != cs.अन्तः())
		प्रत्यागम std::nullopt;
	प्रत्यागम RTLIL::Const(std::move(chunk.दत्त));
}

RTLIL::Const RTLIL::SigSpec::as_const() ध्रुव
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मानम्());
	प्रत्यागम *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() ध्रुव
{
	Chunks cs = chunks();
	स्वचालित it = cs.begin();
	log_assert(it != cs.अन्तः());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.अन्तः() && chunk.wire && chunk.wire->विस्तारः == आकार());
	प्रत्यागम chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() ध्रुव
{
	Chunks cs = chunks();
	स्वचालित it = cs.begin();
	log_assert(it != cs.अन्तः());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.अन्तः());
	प्रत्यागम chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() ध्रुव
{
	प्रत्यागम RTLIL::SigBit(*एतत्);
}

तार्किक RTLIL::SigSpec::मेलनम्(ध्रुव वर्ण* आदर्शः) ध्रुव
{
	पूर्णाङ्क आदर्शः_len = वाक्यदैर्घ्यम्(आदर्शः);
	log_assert(आदर्शः_len == आकार());

	प्रति (पूर्णाङ्क i = 0; i < आदर्शः_len; ++i) {
		वर्ण ch = आदर्शः[i];
		यदि (ch == ' ')
			अनुवर्त;
		RTLIL::SigBit bit = (*एतत्)[आदर्शः_len - 1 - i];
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

std::समुच्चयः<RTLIL::SigBit> RTLIL::SigSpec::गन्तव्यम्_sigbit_समुच्चयः() ध्रुव
{
	std::समुच्चयः<RTLIL::SigBit> sigbits;
	प्रति (स्वचालित &c : chunks())
		प्रति (पूर्णाङ्क i = 0; i < c.विस्तारः; i++)
			sigbits.निवेशय(RTLIL::SigBit(c, i));
	प्रत्यागम sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::गन्तव्यम्_sigbit_pool() ध्रुव
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(आकार());
	प्रति (स्वचालित &c : chunks())
		प्रति (पूर्णाङ्क i = 0; i < c.विस्तारः; i++)
			sigbits.निवेशय(RTLIL::SigBit(c, i));
	प्रत्यागम sigbits;
}

std::सदिशम्<RTLIL::SigBit> RTLIL::SigSpec::गन्तव्यम्_sigbit_सदिशम्() ध्रुव
{
	std::सदिशम्<RTLIL::SigBit> परिणाम;
	परिणाम.reserve(आकार());
	प्रति (SigBit bit : *एतत्)
		परिणाम.प्रक्षिप_पृष्ठभागः(bit);
	प्रत्यागम परिणाम;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::गन्तव्यम्_sigbit_map(ध्रुव RTLIL::SigSpec &other) ध्रुव
{
	पूर्णाङ्क this_आकार = आकार();
	log_assert(this_आकार == other.आकार());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	प्रति (पूर्णाङ्क i = 0; i < this_आकार; i++)
		new_map[(*एतत्)[i]] = other[i];

	प्रत्यागम new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::गन्तव्यम्_sigbit_dict(ध्रुव RTLIL::SigSpec &other) ध्रुव
{
	पूर्णाङ्क this_आकार = आकार();
	log_assert(this_आकार == other.आकार());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_आकार);
	प्रति (पूर्णाङ्क i = 0; i < this_आकार; i++)
		new_map[(*एतत्)[i]] = other[i];

	प्रत्यागम new_map;
}

स्थिर शून्य sigspec_parse_विभज(std::सदिशम्<std::string> &tokens, ध्रुव std::string &text, वर्ण sep)
{
	आकार_t start = 0, अन्तः = 0;
	यावत् ((अन्तः = text.find(sep, start)) != std::string::npos) {
		tokens.प्रक्षिप_पृष्ठभागः(text.substr(start, अन्तः - start));
		start = अन्तः + 1;
	}
	tokens.प्रक्षिप_पृष्ठभागः(text.substr(start));
}

स्थिर RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, ध्रुव std::string &netname)
{
	प्रत्यागम module->wire(netname);
}

तार्किक RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::सदिशम्<std::string> tokens;
	sigspec_parse_विभज(tokens, str, ',');

	sig = RTLIL::SigSpec();
	प्रति (पूर्णाङ्क tokidx = पूर्णाङ्क(tokens.आकार())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		यदि (netname.आकार() == 0)
			अनुवर्त;

		यदि (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			स्वचालित ast = p.const2ast(netname);
			यदि (ast == शून्यसूचकः)
				प्रत्यागम असत्य;
			sig.append(RTLIL::Const(ast->द्वयङ्काः));
			अनुवर्त;
		}

		यदि (module == रिक्त)
			प्रत्यागम असत्य;

		यदि (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		यदि (sigspec_parse_wire(module, netname) == शून्यसूचकः) {
			आकार_t indices_pos = netname.आकार()-1;
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
		यदि (wire == शून्यसूचकः)
			प्रत्यागम असत्य;

		यदि (!indices.रिक्तानि()) {
			std::सदिशम्<std::string> सूचकांक_tokens;
			sigspec_parse_विभज(सूचकांक_tokens, indices.substr(1, indices.आकार()-2), ':');
			यदि (सूचकांक_tokens.आकार() == 1) {
				पूर्णाङ्क a = atoi(सूचकांक_tokens.at(0).c_str());
				यदि (a < 0 || a >= wire->विस्तारः)
					प्रत्यागम असत्य;
				sig.append(RTLIL::SigSpec(wire, a));
			} अन्यथा {
				पूर्णाङ्क a = atoi(सूचकांक_tokens.at(0).c_str());
				पूर्णाङ्क b = atoi(सूचकांक_tokens.at(1).c_str());
				यदि (a > b) {
					पूर्णाङ्क tmp = a;
					a = b, b = tmp;
				}
				यदि (a < 0 || a >= wire->विस्तारः)
					प्रत्यागम असत्य;
				यदि (b < 0 || b >= wire->विस्तारः)
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
	यदि (str.रिक्तानि() || str[0] != '@')
		प्रत्यागम parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	यदि (sel_name == IdString::Null || design->selection_vars.गणनम्(sel_name) == 0)
		प्रत्यागम असत्य;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	प्रति (स्वचालित &it : module->wires_)
		यदि (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	प्रत्यागम सत्य;
}

तार्किक RTLIL::SigSpec::parse_rhs(ध्रुव RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	यदि (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.आकार());
		प्रत्यागम सत्य;
	}

	यदि (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.आकार());
		प्रत्यागम सत्य;
	}

	यदि (lhs.is_chunk()) {
		वर्ण *p = (वर्ण*)str.c_str(), *endptr;
		दीर्घ पूर्णाङ्क val = strtol(p, &endptr, 10);
		यदि (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.आकार());
			प्रत्यागम सत्य;
		}
	}

	यदि (!parse(sig, module, str))
		प्रत्यागम असत्य;
	यदि (sig.आकार() > lhs.आकार())
		sig.remove(lhs.आकार(), sig.आकार() - lhs.आकार());
	प्रत्यागम सत्य;
}

RTLIL::SigSpec::संक्रियकः std::सदिशम्<RTLIL::SigChunk>() ध्रुव
{
	std::सदिशम्<RTLIL::SigChunk> परिणाम;
	प्रति (ध्रुव RTLIL::SigChunk &c : chunks())
		परिणाम.प्रक्षिप_पृष्ठभागः(c);
	प्रत्यागम परिणाम;
}

RTLIL::Case_नियमः::~Case_नियमः()
{
	प्रति (स्वचालित it = switches.begin(); it != switches.अन्तः(); it++)
		विलोपय *it;
}

तार्किक RTLIL::Case_नियमः::रिक्तानि() ध्रुव
{
	प्रत्यागम actions.रिक्तानि() && switches.रिक्तानि();
}

RTLIL::Case_नियमः *RTLIL::Case_नियमः::clone() ध्रुव
{
	RTLIL::Case_नियमः *new_caserule = नूतन RTLIL::Case_नियमः;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->गुणाः = गुणाः;
	प्रति (स्वचालित &it : switches)
		new_caserule->switches.प्रक्षिप_पृष्ठभागः(it->clone());
	प्रत्यागम new_caserule;
}

RTLIL::Switch_नियमः::~Switch_नियमः()
{
	प्रति (स्वचालित it = cases.begin(); it != cases.अन्तः(); it++)
		विलोपय *it;
}

तार्किक RTLIL::Switch_नियमः::रिक्तानि() ध्रुव
{
	प्रत्यागम cases.रिक्तानि();
}

RTLIL::Switch_नियमः *RTLIL::Switch_नियमः::clone() ध्रुव
{
	RTLIL::Switch_नियमः *new_switchrule = नूतन RTLIL::Switch_नियमः;
	new_switchrule->सङ्केत = सङ्केत;
	new_switchrule->गुणाः = गुणाः;
	प्रति (स्वचालित &it : cases)
		new_switchrule->cases.प्रक्षिप_पृष्ठभागः(it->clone());
	प्रत्यागम new_switchrule;

}

RTLIL::Sync_नियमः *RTLIL::Sync_नियमः::clone() ध्रुव
{
	RTLIL::Sync_नियमः *new_syncrule = नूतन RTLIL::Sync_नियमः;
	new_syncrule->type = type;
	new_syncrule->सङ्केत = सङ्केत;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	प्रत्यागम new_syncrule;
}

RTLIL::Process::~Process()
{
	प्रति (स्वचालित it = syncs.begin(); it != syncs.अन्तः(); it++)
		विलोपय *it;
}

RTLIL::Process *RTLIL::Process::clone() ध्रुव
{
	RTLIL::Process *new_proc = नूतन RTLIL::Process;

	new_proc->गुणाः = गुणाः;

	RTLIL::Case_नियमः *rc_ptr = मूलम्_case.clone();
	new_proc->मूलम्_case = *rc_ptr;
	rc_ptr->switches.clear();
	विलोपय rc_ptr;

	प्रति (स्वचालित &it : syncs)
		new_proc->syncs.प्रक्षिप_पृष्ठभागः(it->clone());

	प्रत्यागम new_proc;
}

RTLIL::स्मृतिः::~स्मृतिः()
{
#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	RTLIL::स्मृतिः::get_all_memorys()->erase(hashidx_);
#यदि_अन्तः
}

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
स्थिर std::map<अचिह्नित पूर्णाङ्क, RTLIL::स्मृतिः*> all_memorys;
std::map<अचिह्नित पूर्णाङ्क, RTLIL::स्मृतिः*> *RTLIL::स्मृतिः::get_all_memorys(शून्य)
{
	प्रत्यागम &all_memorys;
}
#यदि_अन्तः
YOSYS_NAMESPACE_अन्तः
