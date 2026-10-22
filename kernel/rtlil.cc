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

#समावेश "kernel/rtlil.h"
#समावेश "kernel/yosys.h"
#समावेश "kernel/macc.h"
#समावेश "kernel/newcelltypes.h"
#समावेश "kernel/sigtools.h"
#समावेश "kernel/threading.h"
#समावेश "frontends/verilog/verilog_frontend.h"
#समावेश "frontends/verilog/preproc.h"
#समावेश "backends/rtlil/rtlil_backend.h"

#समावेश <string.h>
#समावेश <algorithm>
#समावेश <charconv>
#समावेश <optional>
#समावेश <string_view>
#समावेश <sstream>

YOSYS_NAMESPACE_BEGIN

dict<std::string, std::string> RTLIL::constpad;

स्थिर pool<IdString> &RTLIL::builtin_ff_cell_types() {
	अटल स्थिर pool<IdString> res = []() {
		pool<IdString> r;
		क्रम (माप_प्रकार i = 0; i < StaticCellTypes::builder.गिनती; i++) {
			स्वतः &cell = StaticCellTypes::builder.cells[i];
			अगर (cell.features.is_ff)
				r.जोड़ो(cell.type);
		}
		वापस r;
	}();
	वापस res;
}

#घोषणा check(condition) log_assert(condition && "malformed Const union")

स्थिर Const::bitvectype& Const::get_bits() स्थिर {
	check(is_bits());
	वापस *get_if_bits();
}

स्थिर std::string& Const::get_str() स्थिर {
	check(is_str());
	वापस *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	वापस *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	वापस *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	नया ((व्योम*)&str_) std::string(std::move(str));
	tag = backing_tag::string;
}

RTLIL::Const::Const(दीर्घ दीर्घ val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	अक्षर bytes[] = {
		(अक्षर)(val >> 24), (अक्षर)(val >> 16), (अक्षर)(val >> 8), (अक्षर)val
	};
	नया ((व्योम*)&str_) std::string(bytes, 4);
	tag = backing_tag::string;
}

RTLIL::Const::Const(दीर्घ दीर्घ val, पूर्णांक width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	अगर ((width & 7) == 0) {
		नया ((व्योम*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		पूर्णांक bytes = width >> 3;
		चिन्हित अक्षर sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<पूर्णांक>(bytes, माप(val));
		क्रम (पूर्णांक i = 0; i < bytes; i++) {
			str[str.आकार() - 1 - i] = val;
			val = val >> 8;
		}
		वापस;
	}

	नया ((व्योम*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	क्रम (पूर्णांक i = 0; i < width; i++) {
		bv.धकेलो_back((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, पूर्णांक width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	नया ((व्योम*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	क्रम (पूर्णांक i = 0; i < width; i++)
		bv.धकेलो_back(bit);
}

RTLIL::Const::Const(स्थिर std::vector<bool> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	नया ((व्योम*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.आकार());
	क्रम (स्थिर स्वतः &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(स्थिर RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	अगर (is_str())
		नया ((व्योम*)&str_) std::string(other.get_str());
	अन्यथा अगर (is_bits())
		नया ((व्योम*)&bits_) bitvectype(other.get_bits());
	अन्यथा
		check(false);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	अगर (is_str())
		नया ((व्योम*)&str_) std::string(std::move(other.get_str()));
	अन्यथा अगर (is_bits())
		नया ((व्योम*)&bits_) bitvectype(std::move(other.get_bits()));
	अन्यथा
		check(false);
}

RTLIL::Const &RTLIL::Const::चालक =(स्थिर RTLIL::Const &other) {
	flags = other.flags;
	अगर (other.is_str()) {
		अगर (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(व्योम)नया ((व्योम*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} अन्यथा अगर (other.is_bits()) {
		अगर (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(व्योम)नया ((व्योम*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} अन्यथा {
		check(false);
	}
	वापस *यह;
}

RTLIL::Const::~Const() {
	अगर (is_bits())
		bits_.~bitvectype();
	अन्यथा अगर (is_str())
		str_.~string();
	अन्यथा
		check(false);
}

bool RTLIL::Const::चालक<(स्थिर RTLIL::Const &other) स्थिर
{
	अगर (आकार() != other.आकार())
		वापस आकार() < other.आकार();

	क्रम (पूर्णांक i = 0; i < आकार(); i++)
		अगर ((*यह)[i] != other[i])
			वापस (*यह)[i] < other[i];

	वापस false;
}

bool RTLIL::Const::चालक ==(स्थिर RTLIL::Const &other) स्थिर
{
	अगर (is_str() && other.is_str())
		वापस get_str() == other.get_str();
	अगर (is_bits() && other.is_bits())
		वापस get_bits() == other.get_bits();

	अगर (आकार() != other.आकार())
		वापस false;

	क्रम (पूर्णांक i = 0; i < आकार(); i++)
	अगर ((*यह)[i] != other[i])
		वापस false;

	वापस true;
}

bool RTLIL::Const::चालक !=(स्थिर RTLIL::Const &other) स्थिर
{
	वापस !(*यह == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	वापस get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() स्थिर
{
	std::vector<State> v;
	क्रम (स्वतः bit : *यह)
		v.धकेलो_back(bit);
	वापस v;
}

bool RTLIL::Const::as_bool() स्थिर
{
	अगर (is_str()) {
		क्रम (अक्षर ch : get_str())
			अगर (ch != 0)
				वापस true;
		वापस false;
	}

	स्थिर bitvectype& bv = get_bits();
	क्रम (माप_प्रकार i = 0; i < bv.आकार(); i++)
		अगर (bv[i] == State::S1)
			वापस true;
	वापस false;
}

पूर्णांक RTLIL::Const::as_int(bool is_signed) स्थिर
{
	int32_t ret = 0;
	अगर (स्थिर std::string *s = get_if_str()) {
		पूर्णांक आकार = Get_आकार(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		क्रम (पूर्णांक i = std::min(4, आकार); i > 0; i--)
			ret |= static_cast<अचिन्हित अक्षर>((*s)[आकार - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		अगर (is_signed && आकार > 0 && आकार < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << आकार*8;
		वापस ret;
	}

	स्थिर bitvectype& bv = get_bits();
	पूर्णांक significant_bits = std::min(Get_आकार(bv), 32);
	क्रम (पूर्णांक i = 0; i < significant_bits; i++)
		अगर (bv[i] == State::S1)
			ret |= 1 << i;
	अगर (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	वापस ret;
}

bool RTLIL::Const::convertible_to_int(bool is_signed) स्थिर
{
	स्वतः आकार = get_min_आकार(is_signed);

	अगर (आकार < 0)
		वापस false;

	// If it fits in 31 bits it is definitely convertible
	अगर (आकार <= 31)
		वापस true;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	अगर (आकार == 32) {
		अगर (is_signed)
			वापस true;
		वापस (*यह)[आकार - 1] != State::S1;
	}

	वापस false;
}

std::optional<पूर्णांक> RTLIL::Const::try_as_int(bool is_signed) स्थिर
{
	अगर (!convertible_to_int(is_signed))
		वापस std::nullopt;
	वापस as_int(is_signed);
}

पूर्णांक RTLIL::Const::as_int_saturating(bool is_signed) स्थिर
{
	अगर (!convertible_to_int(is_signed)) {
		अगर (!is_signed)
			वापस std::numeric_limits<पूर्णांक>::max();

		स्थिर स्वतः min_आकार = get_min_आकार(is_signed);
		log_assert(min_आकार > 0);
		स्थिर स्वतः neg = (*यह)[min_आकार - 1];
		वापस neg ? std::numeric_limits<पूर्णांक>::min() : std::numeric_limits<पूर्णांक>::max();
	}
	वापस as_int(is_signed);
}

व्योम RTLIL::Const::tag_bare_integer_const(स्थिर std::string &मूल्य)
{
	अगर (मूल्य.empty() || मूल्य.find('\'') != std::string::npos)
		वापस;
	माप_प्रकार start = (मूल्य[0] == '-' || मूल्य[0] == '+') ? 1 : 0;
	अगर (start < मूल्य.आकार() && std::all_of(मूल्य.begin() + start, मूल्य.end(), ::है_अंक))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

पूर्णांक RTLIL::Const::get_min_आकार(bool is_signed) स्थिर
{
	अगर (empty()) वापस 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	अगर (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	अन्यथा
		leading_bit = RTLIL::State::S0;

	स्वतः idx = आकार();
	जबतक (idx > 0 && (*यह)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	अगर (is_signed && idx < आकार()) {
		idx++;
	}
	// must be at least one bit
	वापस (idx == 0) ? 1 : idx;
}

व्योम RTLIL::Const::compress(bool is_signed)
{
	स्वतः idx = get_min_आकार(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<पूर्णांक> RTLIL::Const::as_int_compress(bool is_signed) स्थिर
{
	वापस try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(स्थिर अक्षर* any) स्थिर
{
	पूर्णांक sz = आकार();
	std::string ret;
	ret.reserve(sz);
	क्रम (पूर्णांक i = sz - 1; i >= 0; --i)
		चयन ((*यह)[i]) {
			हाल S0: ret.धकेलो_back('0'); अवरोध;
			हाल S1: ret.धकेलो_back('1'); अवरोध;
			हाल Sx: ret.धकेलो_back('x'); अवरोध;
			हाल Sz: ret.धकेलो_back('z'); अवरोध;
			हाल Sa: ret += any; अवरोध;
			हाल Sm: ret.धकेलो_back('m'); अवरोध;
		}
	वापस ret;
}

RTLIL::Const RTLIL::Const::from_string(स्थिर std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.आकार());
	क्रम (स्वतः it = str.rbegin(); it != str.rend(); it++)
		चयन (*it) {
			हाल '0': bv.धकेलो_back(State::S0); अवरोध;
			हाल '1': bv.धकेलो_back(State::S1); अवरोध;
			हाल 'x': bv.धकेलो_back(State::Sx); अवरोध;
			हाल 'z': bv.धकेलो_back(State::Sz); अवरोध;
			हाल 'm': bv.धकेलो_back(State::Sm); अवरोध;
			शेष: bv.धकेलो_back(State::Sa);
		}
	वापस c;
}

std::string RTLIL::Const::decode_string() स्थिर
{
	अगर (स्वतः str = get_if_str())
		वापस *str;

	स्थिर bitvectype& bv = get_bits();
	स्थिर पूर्णांक n = Get_आकार(bv);
	स्थिर पूर्णांक n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	पूर्णांक i = n_over_8 * 8;
	अगर (i < n) {
		अक्षर ch = 0;
		क्रम (पूर्णांक j = 0; j < (n - i); j++) {
			अगर (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		अगर (ch != 0)
			s.append({ch});
	}
	i -= 8;
	क्रम (; i >= 0; i -= 8) {
		अक्षर ch = 0;
		क्रम (पूर्णांक j = 0; j < 8; j++) {
			अगर (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		अगर (ch != 0)
			s.append({ch});
	}
	वापस s;
}

पूर्णांक RTLIL::Const::आकार() स्थिर {
	अगर (is_str())
		वापस 8 * str_.आकार();
	अन्यथा {
		check(is_bits());
		वापस bits_.आकार();
	}
}

bool RTLIL::Const::empty() स्थिर {
	अगर (is_str())
		वापस str_.empty();
	अन्यथा {
		check(is_bits());
		वापस bits_.empty();
	}
}

व्योम RTLIL::Const::bitvectorize_internal() {
	अगर (tag == backing_tag::bits)
		वापस;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.आकार() * 8);
	क्रम (पूर्णांक i = str_.आकार() - 1; i >= 0; i--) {
		अचिन्हित अक्षर ch = str_[i];
		क्रम (पूर्णांक j = 0; j < 8; j++) {
			new_bits.धकेलो_back((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(व्योम)नया ((व्योम*)&bits_) bitvectype(std::move(new_bits));
		tag = backing_tag::bits;
	}
}

व्योम RTLIL::Const::append(स्थिर RTLIL::Const &other) {
	bitvectorize_internal();
	bitvectype& bv = get_bits();
	bv.जोड़ो(bv.end(), other.begin(), other.end());
}

RTLIL::State RTLIL::Const::const_iterator::चालक*() स्थिर {
	अगर (स्वतः bv = parent->get_if_bits())
		वापस (*bv)[idx];

	पूर्णांक char_idx = parent->get_str().आकार() - idx / 8 - 1;
	bool bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	वापस bit ? State::S1 : State::S0;
}

bool RTLIL::Const::is_fully_zero() स्थिर
{
	अगर (स्वतः str = get_if_str()) {
		क्रम (अक्षर ch : *str)
			अगर (ch != 0)
				वापस false;
		वापस true;
	}

	स्थिर bitvectype& bv = get_bits();

	क्रम (स्थिर स्वतः &bit : bv)
		अगर (bit != RTLIL::State::S0)
			वापस false;

	वापस true;
}

bool RTLIL::Const::is_fully_ones() स्थिर
{
	अगर (स्वतः str = get_if_str()) {
		क्रम (अक्षर ch : *str)
			अगर (ch != (अक्षर)0xff)
				वापस false;
		वापस true;
	}

	स्थिर bitvectype& bv = get_bits();
	क्रम (स्थिर स्वतः &bit : bv)
		अगर (bit != RTLIL::State::S1)
			वापस false;

	वापस true;
}

bool RTLIL::Const::is_fully_def() स्थिर
{
	अगर (is_str())
		वापस true;

	स्थिर bitvectype& bv = get_bits();
	क्रम (स्थिर स्वतः &bit : bv)
		अगर (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			वापस false;

	वापस true;
}

bool RTLIL::Const::is_fully_undef() स्थिर
{
	अगर (स्वतः str = get_if_str())
		वापस str->empty();

	स्थिर bitvectype& bv = get_bits();
	क्रम (स्थिर स्वतः &bit : bv)
		अगर (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			वापस false;

	वापस true;
}

bool RTLIL::Const::is_fully_undef_x_only() स्थिर
{
	अगर (स्वतः str = get_if_str())
		वापस str->empty();

	स्थिर bitvectype& bv = get_bits();
	क्रम (स्थिर स्वतः &bit : bv)
		अगर (bit != RTLIL::State::Sx)
			वापस false;

	वापस true;
}

bool RTLIL::Const::is_onehot(पूर्णांक *pos) स्थिर
{
	bool found = false;
	पूर्णांक आकार = Get_आकार(*यह);
	क्रम (पूर्णांक i = 0; i < आकार; i++) {
		State bit = (*यह)[i];
		अगर (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			वापस false;
		अगर (bit == RTLIL::State::S1) {
			अगर (found)
				वापस false;
			अगर (pos)
				*pos = i;
			found = true;
		}
	}
	वापस found;
}

Hasher RTLIL::Const::hash_into(Hasher h) स्थिर
{
	अगर (स्वतः str = get_if_str())
		वापस hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	स्थिर bitvectype &bv = get_bits();
	पूर्णांक आकार = Get_आकार(bv);
	std::string packed;
	पूर्णांक packed_आकार = (आकार + 7) >> 3;
	packed.resize(packed_आकार, 0);
	क्रम (पूर्णांक bi = 0; bi < packed_आकार; ++bi) {
		अक्षर ch = 0;
		पूर्णांक end = std::min((bi + 1)*8, आकार);
		क्रम (पूर्णांक i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			अगर (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<अक्षर>::hash_into(ch, h);
				क्रम (; i < आकार; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(आकार);
				वापस h;
			}
			ch |= static_cast<पूर्णांक>(b) << (i & 7);
		}
		packed[packed_आकार - 1 - bi] = ch;
	}
	वापस hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(पूर्णांक offset, पूर्णांक len, RTLIL::State padding) स्थिर {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	क्रम (पूर्णांक i = offset; i < offset + len; i++)
		ret_bv.धकेलो_back(i < Get_आकार(*यह) ? (*यह)[i] : padding);
	वापस RTLIL::Const(ret_bv);
}
#अघोषित check /* check(condition) for Const */

bool RTLIL::AttrObject::has_attribute(RTLIL::IdString id) स्थिर
{
	वापस attributes.गिनती(id);
}

व्योम RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, bool मूल्य)
{
	अगर (मूल्य)
		attributes[id] = RTLIL::Const(1);
	अन्यथा
		attributes.erase(id);
}

bool RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) स्थिर
{
	स्थिर स्वतः it = attributes.find(id);
	अगर (it == attributes.end())
		वापस false;
	वापस it->second.as_bool();
}

व्योम RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string मूल्य)
{
	अगर (मूल्य.empty())
		attributes.erase(id);
	अन्यथा
		attributes[id] = मूल्य;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) स्थिर
{
	std::string मूल्य;
	स्थिर स्वतः it = attributes.find(id);
	अगर (it != attributes.end())
		मूल्य = it->second.decode_string();
	वापस मूल्य;
}

व्योम RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, स्थिर pool<string> &तथ्य)
{
	string attrval;
	क्रम (स्थिर स्वतः &s : तथ्य) {
		अगर (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

व्योम RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, स्थिर pool<string> &तथ्य)
{
	pool<string> union_तथ्य = get_strpool_attribute(id);
	union_तथ्य.जोड़ो(तथ्य.begin(), तथ्य.end());
	अगर (!union_तथ्य.empty())
		set_strpool_attribute(id, union_तथ्य);
}

व्योम RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->attributes = std::move(buf);
}

namespace {
	ढाँचा<typename F>
	व्योम walk_process_attr_objects(RTLIL::Process *process, F visit) {
		visit(process);
		std::vector<RTLIL::CaseRule*> case_ढेर{&process->मूल_case};
		जबतक (!case_ढेर.empty()) {
			RTLIL::CaseRule *cs = case_ढेर.back();
			case_ढेर.निकालो_back();
			visit(cs);
			क्रम (स्वतः *sw : cs->switches) {
				visit(sw);
				क्रम (स्वतः *case_ : sw->cases)
					case_ढेर.धकेलो_back(case_);
			}
		}
		क्रम (स्वतः *sync : process->syncs)
			क्रम (स्वतः &mwa : sync->mem_write_actions)
				visit(&mwa);
	}

	ढाँचा<typename F>
	व्योम walk_attr_objects(RTLIL::Design *design, F visit) {
		क्रम (स्वतः &[_, module] : design->modules_) {
			visit(module);
			क्रम (स्वतः &[_, wire] : module->wires_)
				visit(wire);
			क्रम (स्वतः &[_, mem] : module->memories)
				visit(mem);
			क्रम (स्वतः &[_, cell] : module->cells_)
				visit(cell);
			क्रम (स्वतः &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


माप_प्रकार RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	स्वतः मूल = [&](IdString ref) {
		अगर (ref != IdString::Null)
			live.जोड़ो(ref);
	};

	walk_attr_objects(यह, [&](स्थिर RTLIL::AttrObject *obj) {
		क्रम (स्वतः &attr : obj->attributes)
			मूल(attr.first);
	});

	मूल(selected_active_module);

	क्रम (स्वतः &[name, module] : modules_) {
		मूल(name);
		क्रम (IdString port : module->ports)
			मूल(port);
		क्रम (IdString param : module->avail_parameters)
			मूल(param);
		क्रम (स्वतः &[param, _] : module->parameter_default_मूल्यावली)
			मूल(param);
		क्रम (स्वतः &[name, _] : module->memories)
			मूल(name);
		क्रम (स्वतः &[name, _] : module->processes)
			मूल(name);
		क्रम (स्वतः &[name, wire] : module->wires_) {
			मूल(name);
			अगर (wire->known_driver())
				मूल(wire->driverPort());
		}
		क्रम (स्वतः &[name, cell] : module->cells_) {
			मूल(name);
			मूल(cell->type);
			क्रम (स्वतः &conn : cell->connections())
				मूल(conn.first);
			क्रम (स्वतः &param : cell->parameters)
				मूल(param.first);
		}
		क्रम (स्वतः &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_ढेर{&process->मूल_case};
			जबतक (!case_ढेर.empty()) {
				RTLIL::CaseRule *cs = case_ढेर.back();
				case_ढेर.निकालो_back();
				क्रम (स्वतः *sw : cs->switches) {
					क्रम (स्वतः *case_ : sw->cases)
						case_ढेर.धकेलो_back(case_);
				}
			}
			क्रम (स्वतः *sync : process->syncs) {
				क्रम (स्वतः &mwa : sync->mem_write_actions)
					मूल(mwa.memid);
			}
		}
	}

	स्वतः मूल_selection = [&](स्थिर RTLIL::Selection &sel) {
		क्रम (IdString m : sel.selected_modules)
			मूल(m);
		क्रम (स्वतः &[m, members] : sel.selected_members) {
			मूल(m);
			क्रम (IdString member : members)
				मूल(member);
		}
	};

	क्रम (स्वतः &[name, sel] : selection_vars) {
		मूल(name);
		मूल_selection(sel);
	}

	क्रम (स्वतः &sel : selection_ढेर)
		मूल_selection(sel);

	माप_प्रकार erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_गिनती;
	वापस erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) स्थिर
{
	pool<string> तथ्य;
	अगर (attributes.गिनती(id) != 0)
		क्रम (स्वतः s : split_tokens(get_string_attribute(id), "|"))
			तथ्य.जोड़ो(s);
	वापस तथ्य;
}

व्योम RTLIL::AttrObject::set_hdlname_attribute(स्थिर vector<string> &hierarchy)
{
	string attrval;
	क्रम (स्थिर स्वतः &ident : hierarchy) {
		अगर (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() स्थिर
{
	वापस split_tokens(get_string_attribute(ID::hdlname), " ");
}

व्योम RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, स्थिर vector<पूर्णांक> &तथ्य)
{
	std::stringstream attrval;
	क्रम (स्वतः &i : तथ्य) {
		अगर (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<पूर्णांक> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) स्थिर
{
	vector<पूर्णांक> तथ्य;
	स्वतः it = attributes.find(id);
	अगर (it != attributes.end())
		क्रम (स्थिर स्वतः &s : split_tokens(attributes.at(id).decode_string())) {
			अक्षर *end = nullptr;
			त्रुटि_सं = 0;
			दीर्घ मूल्य = म_से_दीर्घ(s.c_str(), &end, 10);
			अगर (end != s.c_str() + s.आकार())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			अगर (त्रुटि_सं == दुस्फल || मूल्य < पूर्णांक_न्यून || मूल्य > पूर्णांक_उच्च)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			तथ्य.धकेलो_back(मूल्य);
		}
	वापस तथ्य;
}

bool RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) स्थिर
{
	अगर (current_design != nullptr) {
		स्वतः module = current_design->module(mod_name);
		वापस module && module->get_blackbox_attribute();
	} अन्यथा {
		log_warning("Unable to check if module is boxed for null design.\n");
		वापस false;
	}
}

bool RTLIL::Selection::selected_module(RTLIL::IdString mod_name) स्थिर
{
	अगर (complete_selection)
		वापस true;
	अगर (!selects_boxes && boxed_module(mod_name))
		वापस false;
	अगर (full_selection)
		वापस true;
	अगर (selected_modules.गिनती(mod_name) > 0)
		वापस true;
	अगर (selected_members.गिनती(mod_name) > 0)
		वापस true;
	वापस false;
}

bool RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) स्थिर
{
	अगर (complete_selection)
		वापस true;
	अगर (!selects_boxes && boxed_module(mod_name))
		वापस false;
	अगर (full_selection)
		वापस true;
	अगर (selected_modules.गिनती(mod_name) > 0)
		वापस true;
	वापस false;
}

bool RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) स्थिर
{
	अगर (complete_selection)
		वापस true;
	अगर (!selects_boxes && boxed_module(mod_name))
		वापस false;
	अगर (full_selection)
		वापस true;
	अगर (selected_modules.गिनती(mod_name) > 0)
		वापस true;
	अगर (selected_members.गिनती(mod_name) > 0)
		अगर (selected_members.at(mod_name).गिनती(memb_name) > 0)
			वापस true;
	वापस false;
}

व्योम RTLIL::Selection::optimize(RTLIL::Design *design)
{
	अगर (design != current_design) {
		current_design = design;
	}

	अगर (selects_boxes && full_selection)
		complete_selection = true;
	अगर (complete_selection) {
		full_selection = false;
		selects_boxes = true;
	}
	अगर (selects_all()) {
		selected_modules.सफा();
		selected_members.सफा();
		वापस;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.सफा();
	क्रम (स्वतः mod_name : selected_modules) {
		अगर (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.धकेलो_back(mod_name);
		selected_members.erase(mod_name);
	}
	क्रम (स्वतः mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.सफा();
	क्रम (स्वतः &it : selected_members)
		अगर (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.धकेलो_back(it.first);
	क्रम (स्वतः mod_name : del_list)
		selected_members.erase(mod_name);

	क्रम (स्वतः &it : selected_members) {
		del_list.सफा();
		क्रम (स्वतः memb_name : it.second)
			अगर (current_design->module(it.first)->गिनती_id(memb_name) == 0)
				del_list.धकेलो_back(memb_name);
		क्रम (स्वतः memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.सफा();
	add_list.सफा();
	क्रम (स्वतः &it : selected_members)
		अगर (it.second.आकार() == 0)
			del_list.धकेलो_back(it.first);
		अन्यथा अगर (it.second.आकार() == current_design->module(it.first)->wires_.आकार() + current_design->module(it.first)->memories.आकार() +
				current_design->module(it.first)->cells_.आकार() + current_design->module(it.first)->processes.आकार())
			add_list.धकेलो_back(it.first);
	क्रम (स्वतः mod_name : del_list)
		selected_members.erase(mod_name);
	क्रम (स्वतः mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.जोड़ो(mod_name);
	}

	अगर (selected_modules.आकार() == current_design->modules_.आकार()) {
		selected_modules.सफा();
		selected_members.सफा();
		अगर (selects_boxes)
			complete_selection = true;
		अन्यथा
			full_selection = true;
	}
}

व्योम RTLIL::Selection::सफा()
{
	full_selection = false;
	complete_selection = false;
	selected_modules.सफा();
	selected_members.सफा();
}

RTLIL::Design::Design()
  : verilog_defines (नया define_map_t)
{
	अटल अचिन्हित पूर्णांक hashidx_गिनती = 123456789;
	hashidx_गिनती = mkhash_xorshift(hashidx_गिनती);
	hashidx_ = hashidx_गिनती;

	refcount_modules_ = 0;
	selected_active_module.सफा();
	धकेलो_full_selection();

	RTLIL::Design::get_all_designs()->जोड़ो(std::pair<अचिन्हित पूर्णांक, RTLIL::Design*>(hashidx_, यह));
}

RTLIL::Design::~Design()
{
	क्रम (स्वतः &pr : modules_)
		मिटाओ pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

अटल std::map<अचिन्हित पूर्णांक, RTLIL::Design*> all_designs;
std::map<अचिन्हित पूर्णांक, RTLIL::Design*> *RTLIL::Design::get_all_designs(व्योम)
{
	वापस &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	वापस RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

स्थिर RTLIL::Module *RTLIL::Design::module(IdString id) स्थिर {
	twines_.check_owned(id);
	वापस modules_.गिनती(id) ? modules_.at(id) : शून्य;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	वापस modules_.गिनती(id) ? modules_.at(id) : शून्य;
}

RTLIL::Module *RTLIL::Design::शीर्ष_module() स्थिर
{
	RTLIL::Module *module = nullptr;
	पूर्णांक module_गिनती = 0;

	क्रम (स्वतः mod : selected_modules()) {
		अगर (mod->get_bool_attribute(ID::शीर्ष))
			वापस mod;
		module_गिनती++;
		module = mod;
	}

	वापस module_गिनती == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	अगर (modules_.गिनती(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = नया RTLIL::Module;
	modules_[name] = module;
	module->design = यह;
	module->name = name;

	क्रम (स्वतः mon : monitors)
		mon->notify_module_add(module);

	अगर (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	वापस module;
}

व्योम RTLIL::Design::scratchpad_unset(स्थिर std::string &varname)
{
	scratchpad.erase(varname);
}

व्योम RTLIL::Design::scratchpad_set_int(स्थिर std::string &varname, पूर्णांक मूल्य)
{
	scratchpad[varname] = stringf("%d", मूल्य);
}

व्योम RTLIL::Design::scratchpad_set_bool(स्थिर std::string &varname, bool मूल्य)
{
	scratchpad[varname] = मूल्य ? "true" : "false";
}

व्योम RTLIL::Design::scratchpad_set_string(स्थिर std::string &varname, std::string मूल्य)
{
	scratchpad[varname] = std::move(मूल्य);
}

पूर्णांक RTLIL::Design::scratchpad_get_int(स्थिर std::string &varname, पूर्णांक default_मूल्य_2) स्थिर
{
	स्वतः it = scratchpad.find(varname);
	अगर (it == scratchpad.end())
		वापस default_मूल्य_2;

	स्थिर std::string &str = it->second;

	अगर (str == "0" || str == "false")
		वापस 0;

	अगर (str == "1" || str == "true")
		वापस 1;

	अक्षर *endptr = nullptr;
	दीर्घ पूर्णांक parsed_मूल्य = म_से_दीर्घ(str.c_str(), &endptr, 10);
	वापस *endptr ? default_मूल्य_2 : parsed_मूल्य;
}

bool RTLIL::Design::scratchpad_get_bool(स्थिर std::string &varname, bool default_मूल्य_2) स्थिर
{
	स्वतः it = scratchpad.find(varname);
	अगर (it == scratchpad.end())
		वापस default_मूल्य_2;

	स्थिर std::string &str = it->second;

	अगर (str == "0" || str == "false")
		वापस false;

	अगर (str == "1" || str == "true")
		वापस true;

	वापस default_मूल्य_2;
}

std::string RTLIL::Design::scratchpad_get_string(स्थिर std::string &varname, स्थिर std::string &default_मूल्य_2) स्थिर
{
	स्वतः it = scratchpad.find(varname);
	अगर (it == scratchpad.end())
		वापस default_मूल्य_2;

	वापस it->second;
}

व्योम RTLIL::Design::हटाओ(RTLIL::Module *module)
{
	क्रम (स्वतः mon : monitors)
		mon->notify_module_del(module);

	अगर (yosys_xtrace) {
		log("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	मिटाओ module;
}

व्योम RTLIL::Design::नाम(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	add(module);
}

व्योम RTLIL::Design::sort()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
	क्रम (स्वतः &it : modules_)
		it.second->sort();
}

व्योम RTLIL::Design::sort_modules()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
}

व्योम check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool);

व्योम RTLIL::Design::check()
{
#अगर_अघोषित न_संशोधन
	log_assert(!selection_ढेर.empty());
	पूर्णांक pool_आकार = 0;
	क्रम (स्वतः &it : modules_)
		pool_आकार = std::max(pool_आकार, ThreadPool::work_pool_आकार(0, it.second->cells_आकार(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_आकार);
	क्रम (स्वतः &it : modules_) {
		log_assert(यह == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#पूर्ण_अगर
}

व्योम RTLIL::Design::optimize()
{
	क्रम (स्वतः &it : modules_)
		it.second->optimize();
	क्रम (स्वतः &it : selection_ढेर)
		it.optimize(यह);
	क्रम (स्वतः &it : selection_vars)
		it.second.optimize(यह);
}

व्योम RTLIL::Design::clone_into(RTLIL::Design *dst) स्थिर
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	क्रम (स्वतः it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

bool RTLIL::Design::selected_module(IdString mod_name) स्थिर
{
	अगर (!selected_active_module.empty() && mod_name != selected_active_module)
		वापस false;
	वापस selection().selected_module(mod_name);
}

bool RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) स्थिर
{
	अगर (!selected_active_module.empty() && mod_name != selected_active_module)
		वापस false;
	वापस selection().selected_whole_module(mod_name);
}

bool RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) स्थिर
{
	अगर (!selected_active_module.empty() && mod_name != selected_active_module)
		वापस false;
	वापस selection().selected_member(mod_name, memb_name);
}

bool RTLIL::Design::selected_module(RTLIL::Module *mod) स्थिर
{
	वापस selected_module(mod->name);
}

bool RTLIL::Design::selected_whole_module(RTLIL::Module *mod) स्थिर
{
	वापस selected_whole_module(mod->name);
}

व्योम RTLIL::Design::धकेलो_selection(RTLIL::Selection sel)
{
	sel.current_design = यह;
	selection_ढेर.धकेलो_back(sel);
}

व्योम RTLIL::Design::धकेलो_empty_selection()
{
	धकेलो_selection(RTLIL::Selection::EmptySelection(यह));
}

व्योम RTLIL::Design::धकेलो_full_selection()
{
	धकेलो_selection(RTLIL::Selection::FullSelection(यह));
}

व्योम RTLIL::Design::धकेलो_complete_selection()
{
	धकेलो_selection(RTLIL::Selection::CompleteSelection(यह));
}

व्योम RTLIL::Design::निकालो_selection()
{
	selection_ढेर.निकालो_back();
	// Default to a full_selection if we ran out of stack
	अगर (selection_ढेर.empty())
		धकेलो_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(bool only_selected) स्थिर
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(यह), only_selected, true, false, RTLIL_BACKEND::DumpMode::Readable);
	वापस f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) स्थिर
{
	bool include_partials = partials == RTLIL::SELECT_ALL;
	bool exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	bool ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> परिणाम;
	परिणाम.reserve(modules_.आकार());
	क्रम (स्वतः &it : modules_) {
		अगर (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			अगर (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				परिणाम.धकेलो_back(it.second);
			अन्यथा
				चयन (boxes)
				{
				हाल RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.second);
					अवरोध;
				हाल RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.second);
					अवरोध;
				हाल RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.second);
					अवरोध;
				हाल RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.second);
					अवरोध;
				हाल RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.second);
					अवरोध;
				हाल RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.second);
					अवरोध;
				शेष:
					अवरोध;
				}
		} अन्यथा अगर (!include_partials && selected_module(it.first)) {
			चयन(partials)
			{
			हाल RTLIL::SELECT_WHOLE_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.second);
				अवरोध;
			हाल RTLIL::SELECT_WHOLE_ERR:
				log_error("Unsupported partially selected module %s.\n", it.second);
				अवरोध;
			हाल RTLIL::SELECT_WHOLE_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.second);
				अवरोध;
			शेष:
				अवरोध;
			}
		}
	}
	वापस परिणाम;
}

RTLIL::Module::Module()
{
	अटल अचिन्हित पूर्णांक hashidx_गिनती = 123456789;
	hashidx_गिनती = mkhash_xorshift(hashidx_गिनती);
	hashidx_ = hashidx_गिनती;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#अगर_घोषित YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->जोड़ो(std::pair<अचिन्हित पूर्णांक, RTLIL::Module*>(hashidx_, यह));
#पूर्ण_अगर
}

RTLIL::Module::~Module()
{
	क्रम (स्वतः &pr : wires_)
		मिटाओ pr.second;
	क्रम (स्वतः &pr : memories)
		मिटाओ pr.second;
	क्रम (स्वतः &pr : cells_)
		मिटाओ pr.second;
	क्रम (स्वतः &pr : processes)
		मिटाओ pr.second;
#अगर_घोषित YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#पूर्ण_अगर
}

#अगर_घोषित YOSYS_ENABLE_PYTHON
अटल std::map<अचिन्हित पूर्णांक, RTLIL::Module*> all_modules;
std::map<अचिन्हित पूर्णांक, RTLIL::Module*> *RTLIL::Module::get_all_modules(व्योम)
{
	वापस &all_modules;
}
#पूर्ण_अगर

व्योम RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	क्रम (स्वतः it = wires_.begin(); it != wires_.end(); ++it)
		अगर (!it->second->port_निवेश && !it->second->port_output)
			delwires.जोड़ो(it->second);

	क्रम (स्वतः it = memories.begin(); it != memories.end(); ++it)
		मिटाओ it->second;
	memories.सफा();

	क्रम (स्वतः it = cells_.begin(); it != cells_.end(); ++it)
		मिटाओ it->second;
	cells_.सफा();

	क्रम (स्वतः it = processes.begin(); it != processes.end(); ++it)
		मिटाओ it->second;
	processes.सफा();

	connections_.सफा();

	हटाओ(delwires);
	set_bool_attribute(ID::blackbox);
}

व्योम RTLIL::Module::expand_interfaces(RTLIL::Design *, स्थिर dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

bool RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	वापस false;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, स्थिर dict<RTLIL::IdString, RTLIL::Const> &, bool mayfail)
{
	अगर (mayfail)
		वापस IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, स्थिर dict<RTLIL::IdString, RTLIL::Const> &, स्थिर dict<RTLIL::IdString, RTLIL::Module*> &, स्थिर dict<RTLIL::IdString, RTLIL::IdString> &, bool mayfail)
{
	अगर (mayfail)
		वापस IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

माप_प्रकार RTLIL::Module::गिनती_id(RTLIL::IdString id)
{
	वापस wires_.गिनती(id) + cells_.गिनती(id) + memories.गिनती(id) + processes.गिनती(id);
}

#अगर_अघोषित न_संशोधन
namespace {
	काष्ठा InternalCellChecker
	{
		स्थिर RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		InternalCellChecker(स्थिर RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		व्योम error(पूर्णांक linenr, std::optional<std::string> error_संदेश = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __खाता__, linenr, buf.str());

			अगर (error_संदेश) {
				log_error("%s in internal cell %s\n", *error_संदेश, cell_loc);
			} अन्यथा {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		पूर्णांक param(RTLIL::IdString name)
		{
			स्वतः it = cell->parameters.find(name);
			अगर (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.जोड़ो(name);
			वापस it->second.as_int();
		}

		पूर्णांक param_bool(RTLIL::IdString name)
		{
			पूर्णांक v = param(name);
			अगर (Get_आकार(cell->parameters.at(name)) > 32)
				error(__LINE__);
			अगर (v != 0 && v != 1)
				error(__LINE__);
			वापस v;
		}

		पूर्णांक param_bool(RTLIL::IdString name, bool expected)
		{
			पूर्णांक v = param_bool(name);
			अगर (v != expected)
				error(__LINE__);
			वापस v;
		}

		व्योम param_bits(RTLIL::IdString name, पूर्णांक width)
		{
			param(name);
			अगर (Get_आकार(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			वापस cell->parameters.at(name).decode_string();
		}

		व्योम port(RTLIL::IdString name, पूर्णांक width)
		{
			स्वतः it = cell->connections_.find(name);
			अगर (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			अगर (Get_आकार(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.जोड़ो(name);
		}

		व्योम check_expected(bool check_matched_sign = false)
		{
			क्रम (स्वतः &para : cell->parameters)
				अगर (expected_params.गिनती(para.first) == 0)
					error(__LINE__);
			क्रम (स्वतः &conn : cell->connections())
				अगर (expected_ports.गिनती(conn.first) == 0)
					error(__LINE__);

			अगर (check_matched_sign) {
				log_assert(expected_params.गिनती(ID::A_SIGNED) != 0 && expected_params.गिनती(ID::B_SIGNED) != 0);
				bool a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				bool b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				अगर (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		व्योम check()
		{
			अगर (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				वापस;

			अगर (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(true);
				वापस;
			}

			अगर (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/false);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/false);
				वापस;
			}

			अगर (cell->type.in(ID($shift), ID($shiftx))) {
				अगर (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/false);
				} अन्यथा {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/false);
				वापस;
			}

			अगर (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(true);
				वापस;
			}

			अगर (cell->type.in(ID($add), ID($sub), ID($mul), ID($भाग), ID($mod), ID($divfloor), ID($modfloor), ID($घात))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($घात));
				वापस;
			}

			अगर (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($alu)) {
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
				वापस;
			}

			अगर (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				वापस;
			}

			अगर (cell->type == ID($macc_v2)) {
				अगर (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				अगर (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				अगर (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				स्थिर Const &a_width = cell->getParam(ID::A_WIDTHS);
				स्थिर Const &b_width = cell->getParam(ID::B_WIDTHS);
				स्थिर Const &c_width = cell->getParam(ID::C_WIDTHS);
				पूर्णांक a_width_योग = 0, b_width_योग = 0, c_width_योग = 0;
				क्रम (पूर्णांक i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_योग += a_width.extract(16 * i, 16).as_int(false);
					b_width_योग += b_width.extract(16 * i, 16).as_int(false);
				}
				क्रम (पूर्णांक i = 0; i < param(ID::NADDENDS); i++) {
					c_width_योग += c_width.extract(16 * i, 16).as_int(false);
				}
				port(ID::A, a_width_योग);
				port(ID::B, b_width_योग);
				port(ID::C, c_width_योग);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/false);
				वापस;
			}

			अगर (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				अगर (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($dffsre)) {
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
				वापस;
			}

			अगर (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_मूल्य, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_मूल्य, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type.in(ID($sdffe), ID($sdffce))) {
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
				वापस;
			}

			अगर (cell->type == ID($adffe)) {
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
				वापस;
			}

			अगर (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($aldffe)) {
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
				वापस;
			}

			अगर (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_मूल्य, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($fsm)) {
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
				वापस;
			}

			अगर (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::तथ्य_2, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($memrd_v2)) {
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
				port(ID::तथ्य_2, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::तथ्य_2, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($memwr_v2)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PORTID);
				param(ID::PRIORITY_MASK);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::तथ्य_2, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::तथ्य_2, param(ID::WIDTH) * param(ID::शब्दगण));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::तथ्य_2, param(ID::WIDTH) * param(ID::शब्दगण));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($mem)) {
				param(ID::MEMID);
				param(ID::आकार_2);
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
				port(ID::RD_तथ्य, param(ID::RD_PORTS) * param(ID::WIDTH));
				port(ID::WR_CLK, param(ID::WR_PORTS));
				port(ID::WR_EN, param(ID::WR_PORTS) * param(ID::WIDTH));
				port(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				port(ID::WR_तथ्य, param(ID::WR_PORTS) * param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($mem_v2)) {
				param(ID::MEMID);
				param(ID::आकार_2);
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
				port(ID::RD_तथ्य, param(ID::RD_PORTS) * param(ID::WIDTH));
				port(ID::WR_CLK, param(ID::WR_PORTS));
				port(ID::WR_EN, param(ID::WR_PORTS) * param(ID::WIDTH));
				port(ID::WR_ADDR, param(ID::WR_PORTS) * param(ID::ABITS));
				port(ID::WR_तथ्य, param(ID::WR_PORTS) * param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type.in(ID($निश्चित), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				वापस;
			}

			अगर (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				वापस;
			}

			अगर (cell->type.in(ID($specify2), ID($specify3))) {
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
				अगर (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
				}
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($specrule)) {
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
				वापस;
			}

			अगर (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				वापस;
			}

			अगर (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				अगर (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
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
				वापस;
			}

			अगर (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				अगर (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				वापस;
			}

			अगर (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); वापस; }

			अगर (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); वापस; }

			अगर (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); वापस; }
			अगर (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); वापस; }

			अगर (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); वापस; }

			अगर (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  वापस; }

			अगर (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); वापस; }

			अगर (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); वापस; }

			अगर (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); वापस; }

			अगर (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); वापस; }

			अगर (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); वापस; }

			अगर (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); वापस; }

			अगर (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); वापस; }

			अगर (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); वापस; }

			अगर (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); वापस; }

			अगर (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); वापस; }

			अगर (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); वापस; }

			अगर (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); वापस; }

			अगर (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); वापस; }

			अगर (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				वापस;
			}
			अगर (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				वापस;
			}
			अगर (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				वापस;
			}
			अगर (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				वापस;
			}
			अगर (cell->type.in(ID($निवेश_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				वापस;
			}
			अगर (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				check_expected();
				वापस;
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
#पूर्ण_अगर

व्योम RTLIL::Module::sort()
{
	wires_.sort(sort_by_id_str(design->twines()));
	cells_.sort(sort_by_id_str(design->twines()));
	parameter_default_मूल्यावली.sort(sort_by_id_str(design->twines()));
	memories.sort(sort_by_id_str(design->twines()));
	processes.sort(sort_by_id_str(design->twines()));
	क्रम (स्वतः &it : cells_)
		it.second->sort();
	क्रम (स्वतः &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	क्रम (स्वतः &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#अगर_अघोषित न_संशोधन
अटल व्योम check_id_कुंजियाँ(स्थिर dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	क्रम (स्वतः &it : keyed)
		log_assert(!it.first.empty());
}
#पूर्ण_अगर

व्योम check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#अगर_अघोषित न_संशोधन
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_आकार(0, module->cells_आकार(), 1000));
	स्थिर RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	क्रम (स्वतः &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->width >= 0);
		log_assert(it.second->आकार >= 0);
		check_id_कुंजियाँ(it.second->attributes);
		memory_strings.जोड़ो(it.second->name.str());
	}

	std::vector<MonotonicFlag> ports_declared(Get_आकार(module->ports));
	ShardedVector<std::string> memids(subpool);
	subpool.run([const_module, &ports_declared, &memory_strings, &memids](स्थिर ParallelDispatchThreadPool::RunCtx &ctx) {
		क्रम (पूर्णांक i : ctx.item_range(const_module->cells_आकार())) {
			स्वतः it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			क्रम (स्वतः &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_कुंजियाँ(it.second->attributes);
			check_id_कुंजियाँ(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			अगर (it.second->has_memid()) {
				log_assert(memory_strings.गिनती(it.second->parameters.at(ID::MEMID).decode_string()));
			} अन्यथा अगर (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.गिनती(memid));
				memids.जोड़ो(ctx, std::move(memid));
			}
			स्वतः cell_mod = const_module->design->module(it.second->name);
			अगर (cell_mod != nullptr) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#अगर_घोषित __GNUC__
				#आशय GCC diagnostic धकेलो
				#आशय GCC diagnostic ignored "-Wdeprecated-declarations"
				#पूर्ण_अगर
				log_assert(!it.second->get_blackbox_attribute());
				#अगर_घोषित __GNUC__
				#आशय GCC diagnostic निकालो
				#पूर्ण_अगर
			}
		}

		क्रम (पूर्णांक i : ctx.item_range(const_module->wires_आकार())) {
			स्वतः it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_कुंजियाँ(it.second->attributes);
			अगर (it.second->port_id) {
				log_assert(Get_आकार(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_निवेश || it.second->port_output);
				log_assert(it.second->port_id <= Get_आकार(ports_declared));
				bool previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == false);
			} अन्यथा
				log_assert(!it.second->port_निवेश && !it.second->port_output);
		}
	});
	क्रम (स्थिर MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == true);
	pool<std::string> memids_pool;
	क्रम (std::string &memid : memids)
		log_assert(memids_pool.जोड़ो(memid).second);

	क्रम (स्वतः &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->मूल_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->मूल_case};
		क्रम (माप_प्रकार i = 0; i < all_cases.आकार(); i++) {
			क्रम (स्वतः &switch_it : all_cases[i]->switches) {
				क्रम (स्वतः &case_it : switch_it->cases) {
					क्रम (स्वतः &compare_it : case_it->compare) {
						log_assert(switch_it->संकेत.आकार() == compare_it.आकार());
					}
					all_cases.धकेलो_back(case_it);
				}
			}
		}
		क्रम (स्वतः &sync_it : it.second->syncs) {
			चयन (sync_it->type) {
				हाल RTLIL::SyncType::ST0:
				हाल RTLIL::SyncType::ST1:
				हाल RTLIL::SyncType::STp:
				हाल RTLIL::SyncType::STn:
				हाल RTLIL::SyncType::STe:
					log_assert(!sync_it->संकेत.empty());
					अवरोध;
				हाल RTLIL::SyncType::STa:
				हाल RTLIL::SyncType::STg:
				हाल RTLIL::SyncType::STi:
					log_assert(sync_it->संकेत.empty());
					अवरोध;
			}
		}
	}

	क्रम (स्वतः &it : module->connections_) {
		log_assert(it.first.आकार() == it.second.आकार());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_कुंजियाँ(module->attributes);
#पूर्ण_अगर
}

व्योम RTLIL::Module::check()
{
	पूर्णांक pool_आकार = ThreadPool::work_pool_आकार(0, cells_आकार(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_आकार);
	check_module(यह, thread_pool);
}

व्योम RTLIL::Module::optimize()
{
}

व्योम RTLIL::Module::cloneInto(RTLIL::Module *new_mod) स्थिर
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.सफा();
	क्रम (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_मूल्यावली.सफा();
	क्रम (स्वतः &it : parameter_default_मूल्यावली)
		new_mod->parameter_default_मूल्यावली[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	क्रम (स्वतः &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	क्रम (स्वतः it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	क्रम (स्वतः it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	क्रम (स्वतः it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	क्रम (स्वतः it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addProcess(dst_id, it->second);
	}

	काष्ठा RewriteSigSpecWorker
	{
		स्थिर dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		स्थिर RTLIL::Module *new_mod;
		व्योम चालक()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([यह](RTLIL::Wire *&wire) {
				स्वतः it = wire_map.find(wire);
				अगर (it != wire_map.end())
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

RTLIL::Module *RTLIL::Module::clone() स्थिर
{
	RTLIL::Module *new_mod = नया RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	वापस new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) स्थिर
{
	RTLIL::Module *new_mod = नया RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->add(new_mod);
	वापस new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) स्थिर
{
	RTLIL::Module *new_mod = नया RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	वापस new_mod;
}

bool RTLIL::Module::has_memories() स्थिर
{
	वापस !memories.empty();
}

bool RTLIL::Module::has_processes() स्थिर
{
	वापस !processes.empty();
}

bool RTLIL::Module::has_memories_warn() स्थिर
{
	अगर (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", यह);
	वापस !memories.empty();
}

bool RTLIL::Module::has_processes_warn() स्थिर
{
	अगर (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", यह);
	वापस !processes.empty();
}

bool RTLIL::Module::is_selected() स्थिर
{
	वापस design->selected_module(यह->name);
}

bool RTLIL::Module::is_selected_whole() स्थिर
{
	वापस design->selected_whole_module(यह->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() स्थिर
{
	std::vector<RTLIL::Wire*> परिणाम;
	परिणाम.reserve(wires_.आकार());
	क्रम (स्वतः &it : wires_)
		अगर (design->selected(यह, it.second))
			परिणाम.धकेलो_back(it.second);
	वापस परिणाम;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() स्थिर
{
	std::vector<RTLIL::Cell*> परिणाम;
	परिणाम.reserve(cells_.आकार());
	क्रम (स्वतः &it : cells_)
		अगर (design->selected(यह, it.second))
			परिणाम.धकेलो_back(it.second);
	वापस परिणाम;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() स्थिर
{
	std::vector<RTLIL::Memory*> परिणाम;
	परिणाम.reserve(memories.आकार());
	क्रम (स्वतः &it : memories)
		अगर (design->selected(यह, it.second))
			परिणाम.धकेलो_back(it.second);
	वापस परिणाम;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() स्थिर
{
	std::vector<RTLIL::Process*> परिणाम;
	परिणाम.reserve(processes.आकार());
	क्रम (स्वतः &it : processes)
		अगर (design->selected(यह, it.second))
			परिणाम.धकेलो_back(it.second);
	वापस परिणाम;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() स्थिर
{
	std::vector<RTLIL::SelectedMember> परिणाम;
	क्रम (स्वतः cell : selected_cells())
		परिणाम.धकेलो_back({cell, cell->name});
	क्रम (स्वतः memory : selected_memories())
		परिणाम.धकेलो_back({memory, memory->name});
	क्रम (स्वतः wire : selected_wires())
		परिणाम.धकेलो_back({wire, wire->name});
	क्रम (स्वतः process : selected_processes())
		परिणाम.धकेलो_back({process, process->name});
	वापस परिणाम;
}

व्योम RTLIL::Module::add(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(गिनती_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = यह;
}

व्योम RTLIL::Module::add(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(गिनती_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = यह;
}

व्योम RTLIL::Module::add(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(गिनती_id(process->name) == 0);
	processes[process->name] = process;
	process->module = यह;
}

व्योम RTLIL::Module::हटाओ(स्थिर pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	अगर (wires.empty())
		वापस;

	काष्ठा DeleteWireWorker
	{
		RTLIL::Module *module;
		स्थिर pool<RTLIL::Wire*> *wires_p;

		व्योम चालक()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([यह](RTLIL::Wire *&wire) {
				अगर (wires_p->गिनती(wire))
					wire = module->addWire(stringf("$delete_wire$%d", (पूर्णांक)autoidx++), wire->width);
			});
		}

		व्योम चालक()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	DeleteWireWorker delete_wire_worker;
	delete_wire_worker.module = यह;
	delete_wire_worker.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_worker);

	अगर (design->flagBufferedNormalized) {
		क्रम (स्वतः wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_सूचकांक.erase(wire);
		}
	}

	क्रम (स्वतः &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.गिनती(id) != 0);
		wires_.erase(id);
		मिटाओ it;
	}
}

व्योम RTLIL::Module::हटाओ(RTLIL::Cell *cell)
{
	जबतक (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.गिनती(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	अगर (design && design->flagBufferedNormalized && buf_norm_cell_queue.गिनती(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.जोड़ो(cell);
	} अन्यथा {
		मिटाओ cell;
	}
}

व्योम RTLIL::Module::हटाओ(RTLIL::Memory *memory)
{
	log_assert(memories.गिनती(memory->name) != 0);
	memories.erase(memory->name);
	मिटाओ memory;
}

व्योम RTLIL::Module::हटाओ(RTLIL::Process *process)
{
	log_assert(processes.गिनती(process->name) != 0);
	processes.erase(process->name);
	मिटाओ process;
}

व्योम RTLIL::Module::नाम(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	add(wire);
}

व्योम RTLIL::Module::नाम(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	add(cell);
}

व्योम RTLIL::Module::नाम(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	अगर (old_id != IdString::Null && wires_.गिनती(old_id))
		नाम(wires_.at(old_id), new_name);
	अन्यथा अगर (old_id != IdString::Null && cells_.गिनती(old_id))
		नाम(cells_.at(old_id), new_name);
	अन्यथा
		log_abort();
}

व्योम RTLIL::Module::swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2)
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

व्योम RTLIL::Module::swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2)
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
	पूर्णांक सूचकांक = 0;
	वापस uniquify(name, सूचकांक);
}

IdString RTLIL::Module::uniquify(IdString name, पूर्णांक &सूचकांक)
{
	अगर (सूचकांक == 0) {
		अगर (गिनती_id(name) == 0)
			वापस name;
		सूचकांक++;
	}

	जबतक (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", सूचकांक)});
		अगर (गिनती_id(new_name) == 0)
			वापस new_name;
		सूचकांक++;
	}
}

अटल bool fixup_ports_compare(स्थिर RTLIL::Wire *a, स्थिर RTLIL::Wire *b)
{
	अगर (a->port_id && !b->port_id)
		वापस true;
	अगर (!a->port_id && b->port_id)
		वापस false;

	अगर (a->port_id == b->port_id)
		वापस a->name < b->name;
	वापस a->port_id < b->port_id;
}

व्योम RTLIL::Module::connect(स्थिर RTLIL::SigSig &conn)
{
	क्रम (स्वतः mon : monitors)
		mon->notify_connect(यह, conn);

	अगर (design)
		क्रम (स्वतः mon : design->monitors)
			mon->notify_connect(यह, conn);

	// ignore all attempts to assign constants to other constants
	अगर (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		क्रम (पूर्णांक i = 0; i < Get_आकार(conn.first); i++)
			अगर (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		अगर (Get_आकार(new_conn.first))
			connect(new_conn);
		वापस;
	}

	अगर (yosys_xtrace) {
		log("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", यह, log_signal(conn.first), log_signal(conn.second), Get_आकार(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(Get_आकार(conn.first) == Get_आकार(conn.second));
	connections_.धकेलो_back(conn);
}

व्योम RTLIL::Module::connect(स्थिर RTLIL::SigSpec &lhs, स्थिर RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

व्योम RTLIL::Module::new_connections(स्थिर std::vector<RTLIL::SigSig> &new_conn)
{
	क्रम (स्वतः mon : monitors)
		mon->notify_connect(यह, new_conn);

	अगर (design)
		क्रम (स्वतः mon : design->monitors)
			mon->notify_connect(यह, new_conn);

	अगर (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", यह);
		क्रम (स्वतः &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), Get_आकार(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

स्थिर std::vector<RTLIL::SigSig> &RTLIL::Module::connections() स्थिर
{
	वापस connections_;
}

व्योम RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	क्रम (स्वतः &w : wires_)
		अगर (w.second->port_निवेश || w.second->port_output)
			all_ports.धकेलो_back(w.second);
		अन्यथा
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	अगर (design && design->flagBufferedNormalized) {
		क्रम (स्वतः &w : wires_)
			अगर (w.second->driverCell_ && w.second->driverCell_->type == ID($निवेश_port))
				buf_norm_wire_queue.जोड़ो(w.second);

		buf_norm_wire_queue.जोड़ो(all_ports.begin(), all_ports.end());
	}

	ports.सफा();
	क्रम (माप_प्रकार i = 0; i < all_ports.आकार(); i++) {
		ports.धकेलो_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
	}
}

व्योम RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		स्थिर dict<IdString, RTLIL::Const> &src,
		स्थिर RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	अगर (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		वापस;
	}
	dst.सफा();
	क्रम (पूर्णांक i = Get_आकार(src) - 1; i >= 0; i--) {
		स्वतः &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::addWire(RTLIL::IdString name, पूर्णांक width)
{
	log_assert(design);
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	RTLIL::Wire *wire = नया RTLIL::Wire;
	wire->width = width;
	wire->name = name;
	add(wire);
	वापस wire;
}

RTLIL::Wire *RTLIL::Module::addWire(IdString name, स्थिर RTLIL::Wire *other)
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
		स्थिर RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
		RTLIL::copy_attr_dict(wire->attributes, other->attributes, src_design, यह->design);
	}
	वापस wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = नया RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	वापस cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, स्थिर RTLIL::Cell *other)
{
	स्थिर RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	bool cross_pool = src_design && यह->design && src_design != यह->design;

	IdString type = other->type.ref();
	अगर (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, यह->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, यह->design);

	अगर (!cross_pool)
		cell->connections_ = other->connections_;
	अन्यथा
		क्रम (पूर्णांक i = Get_आकार(other->connections_) - 1; i >= 0; i--) {
			स्वतः &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	वापस cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = नया RTLIL::Memory;
	mem->module = यह;
	mem->name = name;
	memories[name] = mem;
	वापस mem;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name, स्थिर RTLIL::Memory *other)
{
	log_assert(design);
	RTLIL::Memory *mem = नया RTLIL::Memory;
	mem->module = यह;
	mem->name = name;
	mem->width = other->width;
	mem->start_offset = other->start_offset;
	mem->आकार = other->आकार;
	RTLIL::copy_attr_dict(mem->attributes, other->attributes,
			other->module ? other->module->design : nullptr, design);
	memories[name] = mem;
	वापस mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = नया RTLIL::Process;
	proc->name = name;
	add(proc);
	वापस proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, स्थिर RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	स्थिर RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	अगर (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		क्रम (स्वतः *sync : proc->syncs)
			क्रम (स्वतः &mwa : sync->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	वापस proc;
}

#घोषणा DEF_METHOD(_func, y_आकार, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, bool is_signed, स्थिर std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.आकार();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.आकार();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		वापस cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, bool is_signed, स्थिर std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, y_आकार);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		वापस sig_y;                                       \
	}
DEF_METHOD(Not,        sig_a.आकार(), ID($not))
DEF_METHOD(Pos,        sig_a.आकार(), ID($pos))
DEF_METHOD(Neg,        sig_a.आकार(), ID($neg))
DEF_METHOD(ReduceAnd,  1, ID($reduce_and))
DEF_METHOD(ReduceOr,   1, ID($reduce_or))
DEF_METHOD(ReduceXor,  1, ID($reduce_xor))
DEF_METHOD(ReduceXnor, 1, ID($reduce_xnor))
DEF_METHOD(ReduceBool, 1, ID($reduce_bool))
DEF_METHOD(LogicNot,   1, ID($logic_not))
#अघोषित DEF_METHOD

#घोषणा DEF_METHOD(_func, y_आकार, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, bool /* is_signed */, स्थिर std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::WIDTH] = sig_a.आकार();         \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		वापस cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, bool is_signed, स्थिर std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, y_आकार);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		वापस sig_y;                                       \
	}
DEF_METHOD(Buf,     sig_a.आकार(), ID($buf))
DEF_METHOD(Barrier, sig_a.आकार(), ID($barrier))
#अघोषित DEF_METHOD

#घोषणा DEF_METHOD(_func, y_आकार, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed, स्थिर std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.आकार();       \
		cell->parameters[ID::B_WIDTH] = sig_b.आकार();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.आकार();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		वापस cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed, स्थिर std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, y_आकार);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		वापस sig_y;                                            \
	}
DEF_METHOD(And,      max(sig_a.आकार(), sig_b.आकार()), ID($and))
DEF_METHOD(Or,       max(sig_a.आकार(), sig_b.आकार()), ID($or))
DEF_METHOD(Xor,      max(sig_a.आकार(), sig_b.आकार()), ID($xor))
DEF_METHOD(Xnor,     max(sig_a.आकार(), sig_b.आकार()), ID($xnor))
DEF_METHOD(Shift,    sig_a.आकार(), ID($shift))
DEF_METHOD(Lt,       1, ID($lt))
DEF_METHOD(Le,       1, ID($le))
DEF_METHOD(Eq,       1, ID($eq))
DEF_METHOD(Ne,       1, ID($ne))
DEF_METHOD(Eqx,      1, ID($eqx))
DEF_METHOD(Nex,      1, ID($nex))
DEF_METHOD(Ge,       1, ID($ge))
DEF_METHOD(Gt,       1, ID($gt))
DEF_METHOD(Add,      max(sig_a.आकार(), sig_b.आकार()), ID($add))
DEF_METHOD(Sub,      max(sig_a.आकार(), sig_b.आकार()), ID($sub))
DEF_METHOD(Mul,      max(sig_a.आकार(), sig_b.आकार()), ID($mul))
DEF_METHOD(Div,      max(sig_a.आकार(), sig_b.आकार()), ID($भाग))
DEF_METHOD(Mod,      max(sig_a.आकार(), sig_b.आकार()), ID($mod))
DEF_METHOD(DivFloor, max(sig_a.आकार(), sig_b.आकार()), ID($divfloor))
DEF_METHOD(ModFloor, max(sig_a.आकार(), sig_b.आकार()), ID($modfloor))
DEF_METHOD(LogicAnd, 1, ID($logic_and))
DEF_METHOD(LogicOr,  1, ID($logic_or))
#अघोषित DEF_METHOD

#घोषणा DEF_METHOD(_func, y_आकार, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed, स्थिर std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::B_SIGNED] = false;             \
		cell->parameters[ID::A_WIDTH] = sig_a.आकार();       \
		cell->parameters[ID::B_WIDTH] = sig_b.आकार();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.आकार();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		वापस cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed, स्थिर std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, y_आकार);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		वापस sig_y;                                            \
	}
DEF_METHOD(Shl,      sig_a.आकार(), ID($shl))
DEF_METHOD(Shr,      sig_a.आकार(), ID($shr))
DEF_METHOD(Sshl,     sig_a.आकार(), ID($sshl))
DEF_METHOD(Sshr,     sig_a.आकार(), ID($sshr))
#अघोषित DEF_METHOD

#घोषणा DEF_METHOD(_func, y_आकार, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed, स्थिर std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = false;             \
		cell->parameters[ID::B_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.आकार();       \
		cell->parameters[ID::B_WIDTH] = sig_b.आकार();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.आकार();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::B, sig_b);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		वापस cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed, स्थिर std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, y_आकार);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		वापस sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.आकार(), ID($shiftx))
#अघोषित DEF_METHOD

#घोषणा DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_s, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.आकार();               \
		अगर (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.आकार();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		वापस cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_s, स्थिर std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.आकार());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		वापस sig_y;                                             \
	}
DEF_METHOD(Mux,      ID($mux),        0)
DEF_METHOD(Bwmux,    ID($bwmux),      0)
DEF_METHOD(Pmux,     ID($pmux),       1)
#अघोषित DEF_METHOD

#घोषणा DEF_METHOD(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_s, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = _demux ? sig_a.आकार() : sig_y.आकार(); \
		cell->parameters[ID::S_WIDTH] = sig_s.आकार();             \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		वापस cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_s, स्थिर std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.आकार() << sig_s.आकार() : sig_a.आकार() >> sig_s.आकार()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		वापस sig_y;                                             \
	}
DEF_METHOD(Bmux,     ID($bmux),       0)
DEF_METHOD(Demux,    ID($demux),      1)
#अघोषित DEF_METHOD

#घोषणा DEF_METHOD(_func, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.आकार();               \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		वापस cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_s, स्थिर std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.आकार());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		वापस sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#अघोषित DEF_METHOD

#घोषणा DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिर RTLIL::SigBit &sig1, स्थिर RTLIL::SigBit &sig2, स्थिर std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		वापस cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, स्थिर RTLIL::SigBit &sig1, स्थिर std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		वापस sig2;                                      \
	}
#घोषणा DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिर RTLIL::SigBit &sig1, स्थिर RTLIL::SigBit &sig2, स्थिर RTLIL::SigBit &sig3, स्थिर std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		वापस cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, स्थिर RTLIL::SigBit &sig1, स्थिर RTLIL::SigBit &sig2, स्थिर std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		वापस sig3;                                      \
	}
#घोषणा DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिर RTLIL::SigBit &sig1, स्थिर RTLIL::SigBit &sig2, स्थिर RTLIL::SigBit &sig3, स्थिर RTLIL::SigBit &sig4, स्थिर std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		वापस cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, स्थिर RTLIL::SigBit &sig1, स्थिर RTLIL::SigBit &sig2, स्थिर RTLIL::SigBit &sig3, स्थिर std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		वापस sig4;                                      \
	}
#घोषणा DEF_METHOD_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिर RTLIL::SigBit &sig1, स्थिर RTLIL::SigBit &sig2, स्थिर RTLIL::SigBit &sig3, स्थिर RTLIL::SigBit &sig4, स्थिर RTLIL::SigBit &sig5, स्थिर std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->setPort("\\" #_P5, sig5);                   \
		cell->set_src_attribute(src);                     \
		वापस cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, स्थिर RTLIL::SigBit &sig1, स्थिर RTLIL::SigBit &sig2, स्थिर RTLIL::SigBit &sig3, स्थिर RTLIL::SigBit &sig4, स्थिर std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		वापस sig5;                                           \
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
#अघोषित DEF_METHOD_2
#अघोषित DEF_METHOD_3
#अघोषित DEF_METHOD_4
#अघोषित DEF_METHOD_5

RTLIL::Cell* RTLIL::Module::addPow(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool a_signed, bool b_signed, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($घात));
	cell->parameters[ID::A_SIGNED] = a_signed;
	cell->parameters[ID::B_SIGNED] = b_signed;
	cell->parameters[ID::A_WIDTH] = sig_a.आकार();
	cell->parameters[ID::B_WIDTH] = sig_b.आकार();
	cell->parameters[ID::Y_WIDTH] = sig_y.आकार();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addFa(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_c, स्थिर RTLIL::SigSpec &sig_x, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fa));
	cell->parameters[ID::WIDTH] = sig_a.आकार();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::C, sig_c);
	cell->setPort(ID::X, sig_x);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addSlice(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, RTLIL::Const offset, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($slice));
	cell->parameters[ID::A_WIDTH] = sig_a.आकार();
	cell->parameters[ID::Y_WIDTH] = sig_y.आकार();
	cell->parameters[ID::OFFSET] = offset;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addConcat(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($concat));
	cell->parameters[ID::A_WIDTH] = sig_a.आकार();
	cell->parameters[ID::B_WIDTH] = sig_b.आकार();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, RTLIL::Const lut, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.आकार();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.आकार();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_en, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($निश्चित));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_en, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_en, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_en, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_en, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addSr(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr, स्थिर RTLIL::SigSpec &sig_q, bool set_polarity, bool clr_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sr));
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addDff(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addDffe(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity, bool en_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addDffsr(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity, bool set_polarity, bool clr_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dffsr));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addDffsre(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity, bool en_polarity, bool set_polarity, bool clr_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dffsre));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addAdff(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_arst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_मूल्य, bool clk_polarity, bool arst_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_मूल्य] = arst_मूल्य;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addAdffe(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_arst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_मूल्य, bool clk_polarity, bool en_polarity, bool arst_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_मूल्य] = arst_मूल्य;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addAldff(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_aload, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		स्थिर RTLIL::SigSpec &sig_ad, bool clk_polarity, bool aload_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($aldff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::ALOAD, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addAldffe(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_aload, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		स्थिर RTLIL::SigSpec &sig_ad, bool clk_polarity, bool en_polarity, bool aload_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($aldffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ALOAD_POLARITY] = aload_polarity;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ALOAD, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addSdff(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_srst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_मूल्य, bool clk_polarity, bool srst_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_मूल्य] = srst_मूल्य;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addSdffe(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_srst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_मूल्य, bool clk_polarity, bool en_polarity, bool srst_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffe));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_मूल्य] = srst_मूल्य;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addSdffce(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_srst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_मूल्य, bool clk_polarity, bool en_polarity, bool srst_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sdffce));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SRST_POLARITY] = srst_polarity;
	cell->parameters[ID::SRST_मूल्य] = srst_मूल्य;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SRST, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addDlatch(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, bool en_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatch(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_arst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_मूल्य, bool en_polarity, bool arst_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($adlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::ARST_POLARITY] = arst_polarity;
	cell->parameters[ID::ARST_मूल्य] = arst_मूल्य;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::ARST, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsr(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, स्थिर RTLIL::SigSpec &sig_q, bool en_polarity, bool set_polarity, bool clr_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatchsr));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr,
		स्थिर RTLIL::SigSpec &sig_q, bool set_polarity, bool clr_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity, bool en_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addDffsrGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity, bool set_polarity, bool clr_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addDffsreGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity, bool en_polarity, bool set_polarity, bool clr_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addAdffGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_arst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		bool arst_मूल्य, bool clk_polarity, bool arst_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_मूल्य ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addAdffeGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_arst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		bool arst_मूल्य, bool clk_polarity, bool en_polarity, bool arst_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_मूल्य ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addAldffGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_aload, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		स्थिर RTLIL::SigSpec &sig_ad, bool clk_polarity, bool aload_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addAldffeGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_aload, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		स्थिर RTLIL::SigSpec &sig_ad, bool clk_polarity, bool en_polarity, bool aload_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addSdffGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_srst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		bool srst_मूल्य, bool clk_polarity, bool srst_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_मूल्य ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addSdffeGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_srst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		bool srst_मूल्य, bool clk_polarity, bool en_polarity, bool srst_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_मूल्य ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addSdffceGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_srst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		bool srst_मूल्य, bool clk_polarity, bool en_polarity, bool srst_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_मूल्य ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, bool en_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatchGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_arst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
		bool arst_मूल्य, bool en_polarity, bool arst_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_मूल्य ? '1' : '0'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsrGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, स्थिर RTLIL::SigSpec &sig_q, bool en_polarity, bool set_polarity, bool clr_polarity, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.आकार();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, पूर्णांक width, स्थिर std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	वापस sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, पूर्णांक width, स्थिर std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	वापस sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, पूर्णांक width, स्थिर std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	वापस sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, पूर्णांक width, स्थिर std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	वापस sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, स्थिर std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	वापस sig;
}

RTLIL::SigSpec RTLIL::Module::SetTag(RTLIL::IdString name, स्थिर std::string &tag, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_s, स्थिर RTLIL::SigSpec &sig_c, स्थिर std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.आकार());
	Cell *cell = addCell(name, ID($set_tag));
	cell->parameters[ID::WIDTH] = sig_a.आकार();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	वापस sig;
}

RTLIL::Cell* RTLIL::Module::addSetTag(RTLIL::IdString name, स्थिर std::string &tag, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_s, स्थिर RTLIL::SigSpec &sig_c, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src)
{
	Cell *cell = addCell(name, ID($set_tag));
	cell->parameters[ID::WIDTH] = sig_a.आकार();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::SigSpec RTLIL::Module::GetTag(RTLIL::IdString name, स्थिर std::string &tag, स्थिर RTLIL::SigSpec &sig_a, स्थिर std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.आकार());
	Cell *cell = addCell(name, ID($get_tag));
	cell->parameters[ID::WIDTH] = sig_a.आकार();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	वापस sig;
}

RTLIL::Cell* RTLIL::Module::addOverwriteTag(RTLIL::IdString name, स्थिर std::string &tag, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_s, स्थिर RTLIL::SigSpec &sig_c, स्थिर std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($overwrite_tag));
	cell->parameters[ID::WIDTH] = sig_a.आकार();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->set_src_attribute(src);
	वापस cell;
}

RTLIL::SigSpec RTLIL::Module::OriginalTag(RTLIL::IdString name, स्थिर std::string &tag, स्थिर RTLIL::SigSpec &sig_a, स्थिर std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.आकार());
	Cell *cell = addCell(name, ID($original_tag));
	cell->parameters[ID::WIDTH] = sig_a.आकार();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	वापस sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_e, स्थिर std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.आकार());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.आकार();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	वापस sig;
}

std::string RTLIL::Module::to_rtlil_str() स्थिर
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(यह), design, false, true, false, RTLIL_BACKEND::DumpMode::Readable);
	वापस f.str();
}

RTLIL::Wire::Wire()
{
	अटल अचिन्हित पूर्णांक hashidx_गिनती = 123456789;
	hashidx_गिनती = mkhash_xorshift(hashidx_गिनती);
	hashidx_ = hashidx_गिनती;

	module = nullptr;
	width = 1;
	start_offset = 0;
	port_id = 0;
	port_निवेश = false;
	port_output = false;
	upto = false;
	is_signed = false;

#अगर_घोषित YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->जोड़ो(std::pair<अचिन्हित पूर्णांक, RTLIL::Wire*>(hashidx_, यह));
#पूर्ण_अगर
}

RTLIL::Wire::~Wire()
{
#अगर_घोषित YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#पूर्ण_अगर
}

std::string RTLIL::Wire::to_rtlil_str() स्थिर
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", यह, design(), RTLIL_BACKEND::DumpMode::Readable);
	वापस f.str();
}

#अगर_घोषित YOSYS_ENABLE_PYTHON
अटल std::map<अचिन्हित पूर्णांक, RTLIL::Wire*> all_wires;
std::map<अचिन्हित पूर्णांक, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(व्योम)
{
	वापस &all_wires;
}
#पूर्ण_अगर

RTLIL::Memory::Memory()
{
	अटल अचिन्हित पूर्णांक hashidx_गिनती = 123456789;
	hashidx_गिनती = mkhash_xorshift(hashidx_गिनती);
	hashidx_ = hashidx_गिनती;

	width = 1;
	start_offset = 0;
	आकार = 0;
#अगर_घोषित YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->जोड़ो(std::pair<अचिन्हित पूर्णांक, RTLIL::Memory*>(hashidx_, यह));
#पूर्ण_अगर
}

std::string RTLIL::Memory::to_rtlil_str() स्थिर
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", यह, design(), RTLIL_BACKEND::DumpMode::Readable);
	वापस f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	अटल अचिन्हित पूर्णांक hashidx_गिनती = 123456789;
	hashidx_गिनती = mkhash_xorshift(hashidx_गिनती);
	hashidx_ = hashidx_गिनती;
}

std::string RTLIL::Process::to_rtlil_str() स्थिर
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", यह, design(), RTLIL_BACKEND::DumpMode::Readable);
	वापस f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	अटल अचिन्हित पूर्णांक hashidx_गिनती = 123456789;
	hashidx_गिनती = mkhash_xorshift(hashidx_गिनती);
	hashidx_ = hashidx_गिनती;

	// log("#memtrace# %p\n", this);
	memhasher();

#अगर_घोषित YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->जोड़ो(std::pair<अचिन्हित पूर्णांक, RTLIL::Cell*>(hashidx_, यह));
#पूर्ण_अगर
}

RTLIL::Cell::~Cell()
{
#अगर_घोषित YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#पूर्ण_अगर
}

std::string RTLIL::Cell::to_rtlil_str() स्थिर
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", यह, design(), RTLIL_BACKEND::DumpMode::Readable);
	वापस f.str();
}

#अगर_घोषित YOSYS_ENABLE_PYTHON
अटल std::map<अचिन्हित पूर्णांक, RTLIL::Cell*> all_cells;
std::map<अचिन्हित पूर्णांक, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(व्योम)
{
	वापस &all_cells;
}
#पूर्ण_अगर

bool RTLIL::Cell::hasPort(RTLIL::IdString portname) स्थिर
{
	वापस connections_.गिनती(portname) != 0;
}

// bufnorm

स्थिर RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) स्थिर
{
	वापस connections_.at(portname);
}

स्थिर dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() स्थिर
{
	वापस connections_;
}

bool RTLIL::Cell::known() स्थिर
{
	अगर (yosys_celltypes.cell_known(type))
		वापस true;
	अगर (module && module->design && module->design->module(type))
		वापस true;
	वापस false;
}

bool RTLIL::Cell::निवेश(IdString portname) स्थिर
{
	अगर (yosys_celltypes.cell_known(type))
		वापस yosys_celltypes.cell_निवेश(type.ref(), portname);
	अगर (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		वापस w && w->port_निवेश;
	}
	वापस false;
}

bool RTLIL::Cell::output(RTLIL::IdString portname) स्थिर
{
	अगर (yosys_celltypes.cell_known(type))
		वापस yosys_celltypes.cell_output(type, portname);
	अगर (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		वापस w && w->port_output;
	}
	वापस false;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) स्थिर
{
	अगर (yosys_celltypes.cell_known(type.ref()))
		वापस yosys_celltypes.cell_port_dir(type, portname);
	अगर (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		अगर (m == nullptr)
			वापस PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		अगर (w == nullptr)
			वापस PortDir::PD_UNKNOWN;
		वापस PortDir(w->port_निवेश + w->port_output * 2);
	}
	वापस PortDir::PD_UNKNOWN;
}

bool RTLIL::Cell::hasParam(RTLIL::IdString paramname) स्थिर
{
	वापस parameters.गिनती(paramname) != 0;
}

व्योम RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

व्योम RTLIL::Cell::setParam(RTLIL::IdString paramname, RTLIL::Const मूल्य)
{
	parameters[paramname] = std::move(मूल्य);
}

स्थिर RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) स्थिर
{
	स्थिर स्वतः &it = parameters.find(paramname);
	अगर (it != parameters.end())
		वापस it->second;
	अगर (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		अगर (m)
			वापस m->parameter_default_मूल्यावली.at(paramname);
	}
	फेंको std::out_of_range("Cell::getParam()");
}

व्योम RTLIL::Cell::sort()
{
	connections_.sort(sort_by_id_str(module->twines()));
	parameters.sort(sort_by_id_str(module->twines()));
	attributes.sort(sort_by_id_str(module->twines()));
}

व्योम RTLIL::Cell::check()
{
#अगर_अघोषित न_संशोधन
	InternalCellChecker checker(शून्य, यह);
	checker.check();
#पूर्ण_अगर
}

व्योम RTLIL::Cell::fixup_parameters(bool set_a_signed, bool set_b_signed)
{
	अगर (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		वापस;

	अगर (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = Get_आकार(connections_[ID::Y]);
		अगर (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = Get_आकार(connections_[ID::S]);
		check();
		वापस;
	}

	अगर (type == ID($demux)) {
		parameters[ID::WIDTH] = Get_आकार(connections_[ID::A]);
		parameters[ID::S_WIDTH] = Get_आकार(connections_[ID::S]);
		check();
		वापस;
	}

	अगर (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = Get_आकार(connections_[ID::A]);
		वापस;
	}

	अगर (type == ID($fa)) {
		parameters[ID::WIDTH] = Get_आकार(connections_[ID::Y]);
		वापस;
	}

	अगर (type == ID($lcu)) {
		parameters[ID::WIDTH] = Get_आकार(connections_[ID::CO]);
		वापस;
	}

	अगर (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = Get_आकार(connections_[ID::Y]);
		वापस;
	}

	bool signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	अगर (connections_.गिनती(ID::A)) {
		अगर (signedness_ab) {
			अगर (set_a_signed)
				parameters[ID::A_SIGNED] = true;
			अन्यथा अगर (parameters.गिनती(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = false;
		}
		parameters[ID::A_WIDTH] = Get_आकार(connections_[ID::A]);
	}

	अगर (connections_.गिनती(ID::B)) {
		अगर (signedness_ab) {
			अगर (set_b_signed)
				parameters[ID::B_SIGNED] = true;
			अन्यथा अगर (parameters.गिनती(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = false;
		}
		parameters[ID::B_WIDTH] = Get_आकार(connections_[ID::B]);
	}

	अगर (connections_.गिनती(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = Get_आकार(connections_[ID::Y]);

	अगर (connections_.गिनती(ID::Q))
		parameters[ID::WIDTH] = Get_आकार(connections_[ID::Q]);

	check();
}

bool RTLIL::Cell::has_keep_attr() स्थिर {
	वापस get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

bool RTLIL::Cell::has_memid() स्थिर
{
	वापस type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

bool RTLIL::Cell::is_mem_cell() स्थिर
{
	वापस type.in(ID($mem), ID($mem_v2)) || has_memid();
}

bool RTLIL::Cell::is_builtin_ff() स्थिर {
	वापस StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(स्थिर RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	अगर (wire == शून्य)
		तथ्य = {bit.तथ्य};
	अन्यथा
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(पूर्णांक offset, पूर्णांक लंबाई) स्थिर
{
	log_assert(offset >= 0);
	log_assert(लंबाई >= 0);
	log_assert(offset + लंबाई <= width);
	RTLIL::SigChunk ret;
	अगर (wire) {
		ret.wire = wire;
		ret.offset = यह->offset + offset;
		ret.width = लंबाई;
	} अन्यथा {
		क्रम (पूर्णांक i = 0; i < लंबाई; i++)
			ret.तथ्य.धकेलो_back(तथ्य[offset+i]);
		ret.width = लंबाई;
	}
	वापस ret;
}

RTLIL::SigBit RTLIL::SigChunk::चालक[](पूर्णांक offset) स्थिर
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	अगर (wire) {
		ret.wire = wire;
		ret.offset = यह->offset + offset;
	} अन्यथा {
		ret.तथ्य = तथ्य[offset];
	}
	वापस ret;
}

bool RTLIL::SigChunk::चालक <(स्थिर RTLIL::SigChunk &other) स्थिर
{
	अगर (wire && other.wire)
		अगर (wire->name != other.wire->name)
			वापस wire->name < other.wire->name;

	अगर (wire != other.wire)
		वापस wire < other.wire;

	अगर (offset != other.offset)
		वापस offset < other.offset;

	अगर (width != other.width)
		वापस width < other.width;

	वापस तथ्य < other.तथ्य;
}

bool RTLIL::SigChunk::चालक ==(स्थिर RTLIL::SigChunk &other) स्थिर
{
	वापस wire == other.wire && width == other.width && offset == other.offset && तथ्य == other.तथ्य;
}

bool RTLIL::SigChunk::चालक !=(स्थिर RTLIL::SigChunk &other) स्थिर
{
	अगर (*यह == other)
		वापस false;
	वापस true;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.आकार() > 0);
	स्वतः ie = parts.begin();
	स्वतः it = ie + parts.आकार() - 1;
	जबतक (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(स्थिर RTLIL::Const &मूल्य)
{
	अगर (Get_आकार(मूल्य) != 0) {
		rep_ = CHUNK;
		नया (&chunk_) RTLIL::SigChunk(मूल्य);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&मूल्य)
{
	अगर (Get_आकार(मूल्य) != 0) {
		rep_ = CHUNK;
		नया (&chunk_) RTLIL::SigChunk(मूल्य);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(स्थिर RTLIL::SigChunk &chunk)
{
	अगर (chunk.width != 0) {
		rep_ = CHUNK;
		नया (&chunk_) RTLIL::SigChunk(chunk);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	अगर (chunk.width != 0) {
		rep_ = CHUNK;
		नया (&chunk_) RTLIL::SigChunk(chunk);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	अगर (wire->width != 0) {
		rep_ = CHUNK;
		नया (&chunk_) RTLIL::SigChunk(wire);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, पूर्णांक offset, पूर्णांक width)
{
	अगर (width != 0) {
		rep_ = CHUNK;
		नया (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(स्थिर std::string &str)
{
	अगर (str.आकार() != 0) {
		rep_ = CHUNK;
		नया (&chunk_) RTLIL::SigChunk(str);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(पूर्णांक val, पूर्णांक width)
{
	अगर (width != 0) {
		rep_ = CHUNK;
		नया (&chunk_) RTLIL::SigChunk(val, width);
	} अन्यथा
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, पूर्णांक width)
{
	अगर (width != 0) {
		rep_ = CHUNK;
		नया (&chunk_) RTLIL::SigChunk(bit, width);
	} अन्यथा
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(स्थिर RTLIL::SigBit &bit, पूर्णांक width)
{
	अगर (width != 0) {
		अगर (bit.wire == शून्य) {
			rep_ = CHUNK;
			नया (&chunk_) RTLIL::SigChunk(bit.तथ्य, width);
		} अन्यथा अगर (width == 1) {
			rep_ = CHUNK;
			नया (&chunk_) RTLIL::SigChunk(bit);
		} अन्यथा {
			init_empty_bits();
			bits_.reserve(width);
			क्रम (पूर्णांक i = 0; i < width; i++)
				bits_.धकेलो_back(bit);
		}
	} अन्यथा
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(स्थिर std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	क्रम (स्थिर स्वतः &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(स्थिर std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	क्रम (स्थिर स्वतः &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(स्थिर pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	क्रम (स्थिर स्वतः &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(स्थिर std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	क्रम (स्थिर स्वतः &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(bool bit)
{
	rep_ = CHUNK;
	नया (&chunk_) RTLIL::SigChunk(bit ? RTLIL::S1 : RTLIL::S0);
	check();
}

व्योम RTLIL::SigSpec::Chunks::const_iterator::अग्र_chunk_bits()
{
	पूर्णांक bits_आकार = Get_आकार(spec.bits_);
	अगर (bit_सूचकांक >= bits_आकार)
		वापस;
	पूर्णांक i = bit_सूचकांक;
	स्थिर SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.तथ्य.सफा();
	अगर (bit.is_wire()) {
		chunk.offset = bit.offset;
		जबतक (i < bits_आकार && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_सूचकांक)
			++i;
	} अन्यथा {
		chunk.offset = 0;
		chunk.तथ्य.धकेलो_back(bit.तथ्य);
		जबतक (i < bits_आकार && !spec.bits_[i].is_wire()) {
			chunk.तथ्य.धकेलो_back(spec.bits_[i].तथ्य);
			++i;
		}
	}
	chunk.width = i - bit_सूचकांक;
}

व्योम RTLIL::SigSpec::unpack()
{
	अगर (rep_ == BITS)
		वापस;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	क्रम (पूर्णांक i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	नया (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

व्योम RTLIL::SigSpec::try_repack()
{
	अगर (rep_ != BITS)
		वापस;

	पूर्णांक bits_आकार = Get_आकार(bits_);
	अगर (bits_आकार == 0)
		वापस;
	अगर (bits_[0].is_wire()) {
		क्रम (पूर्णांक i = 1; i < bits_आकार; i++)
			अगर (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				वापस;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_आकार);
		bits_.~vector();
		rep_ = CHUNK;
		नया (&chunk_) SigChunk(std::move(chunk));
		वापस;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_आकार);
	bits.धकेलो_back(bits_[0].तथ्य);
	क्रम (पूर्णांक i = 1; i < bits_आकार; i++) {
		अगर (bits_[i].is_wire())
			वापस;
		bits.धकेलो_back(bits_[i].तथ्य);
	}
	bits_.~vector();
	rep_ = CHUNK;
	नया (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() स्थिर
{
	Hasher h;
	क्रम (स्वतः &c : chunks())
		अगर (c.wire == शून्य) {
			क्रम (स्वतः &v : c.तथ्य)
				h.eat(v);
		} अन्यथा {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t परिणाम = h.yield();
	अगर (परिणाम == 0)
		परिणाम = 1;
	hash_.set(परिणाम);
	वापस परिणाम;
}

व्योम RTLIL::SigSpec::sort()
{
	unpack();
	std::sort(bits_.begin(), bits_.end());
	hash_.सफा();
	try_repack();
}

व्योम RTLIL::SigSpec::sort_and_unify()
{
	unpack();
	// A copy of the bits vector is used to prevent duplicating the logic from
	// SigSpec::SigSpec(std::vector<SigBit>).  This incurrs an extra copy but
	// that isn't showing up as significant in profiles.
	std::vector<SigBit> unique_bits = bits_;
	std::sort(unique_bits.begin(), unique_bits.end());
	स्वतः last = std::unique(unique_bits.begin(), unique_bits.end());
	unique_bits.erase(last, unique_bits.end());

	*यह = unique_bits;
	hash_.सफा();
	try_repack();
}

व्योम RTLIL::SigSpec::replace(स्थिर RTLIL::SigSpec &pattern, स्थिर RTLIL::SigSpec &with)
{
	replace(pattern, with, यह);
}

व्योम RTLIL::SigSpec::replace(स्थिर RTLIL::SigSpec &pattern, स्थिर RTLIL::SigSpec &with, RTLIL::SigSpec *other) स्थिर
{
	log_assert(other != शून्य);
	log_assert(आकार() == other->आकार());
	log_assert(pattern.आकार() == with.आकार());

	dict<RTLIL::SigBit, पूर्णांक> pattern_to_with;
	पूर्णांक pattern_आकार = pattern.आकार();
	क्रम (पूर्णांक i = 0; i < pattern_आकार; i++) {
		SigBit pattern_bit = pattern[i];
		अगर (pattern_bit.wire != शून्य) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	पूर्णांक this_आकार = आकार();
	bool other_modified = false;
	क्रम (पूर्णांक j = 0; j < this_आकार; j++) {
		स्वतः it = pattern_to_with.find((*यह)[j]);
		अगर (it != pattern_to_with.end()) {
			other_modified = true;
			(*other)[j] = with[it->second];
		}
	}

	अगर (other_modified)
		other->try_repack();
	other->check();
}

व्योम RTLIL::SigSpec::replace(स्थिर dict<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, यह);
}

व्योम RTLIL::SigSpec::replace(स्थिर dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) स्थिर
{
	log_assert(other != शून्य);
	log_assert(आकार() == other->आकार());

	अगर (rules.empty()) वापस;

	पूर्णांक this_आकार = आकार();
	bool other_modified = false;
	क्रम (पूर्णांक i = 0; i < this_आकार; i++) {
		स्वतः it = rules.find((*यह)[i]);
		अगर (it != rules.end()) {
			other_modified = true;
			(*other)[i] = it->second;
		}
	}

	अगर (other_modified)
		other->try_repack();
	other->check();
}

व्योम RTLIL::SigSpec::replace(स्थिर std::map<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, यह);
}

व्योम RTLIL::SigSpec::replace(स्थिर std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) स्थिर
{
	log_assert(other != शून्य);
	log_assert(आकार() == other->आकार());

	अगर (rules.empty()) वापस;

	पूर्णांक this_आकार = आकार();
	bool other_modified = false;
	क्रम (पूर्णांक i = 0; i < this_आकार; i++) {
		स्वतः it = rules.find((*यह)[i]);
		अगर (it != rules.end()) {
			other_modified = true;
			(*other)[i] = it->second;
		}
	}

	अगर (other_modified)
		other->try_repack();
	other->check();
}

व्योम RTLIL::SigSpec::हटाओ(स्थिर RTLIL::SigSpec &pattern)
{
	remove2(pattern, शून्य);
}

व्योम RTLIL::SigSpec::हटाओ(स्थिर RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) स्थिर
{
	RTLIL::SigSpec tmp = *यह;
	tmp.remove2(pattern, other);
}

व्योम RTLIL::SigSpec::remove2(स्थिर RTLIL::SigSpec &pattern, RTLIL::SigSpec *other)
{
	unpack();
	अगर (other != शून्य) {
		log_assert(आकार() == other->आकार());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.आकार());
	क्रम (स्वतः &bit : pattern)
		अगर (bit.wire != शून्य)
			pattern_bits.जोड़ो(bit);

	// Compact in-place to avoid O(n^2) erase operations
	माप_प्रकार write_idx = 0;
	क्रम (माप_प्रकार read_idx = 0; read_idx < bits_.आकार(); read_idx++)
	{
		अगर (!(bits_[read_idx].wire != शून्य && pattern_bits.गिनती(bits_[read_idx]))) {
			अगर (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				अगर (other != शून्य)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	bool modified = (write_idx < bits_.आकार());
	अगर (modified) {
		bits_.resize(write_idx);
		hash_.सफा();
		try_repack();
	}
	अगर (other != शून्य && modified) {
		other->bits_.resize(write_idx);
		other->hash_.सफा();
		other->try_repack();
	}
	check();
}

व्योम RTLIL::SigSpec::हटाओ(स्थिर pool<RTLIL::SigBit> &pattern)
{
	remove2(pattern, शून्य);
}

व्योम RTLIL::SigSpec::हटाओ(स्थिर pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) स्थिर
{
	RTLIL::SigSpec tmp = *यह;
	tmp.remove2(pattern, other);
}

व्योम RTLIL::SigSpec::remove2(स्थिर pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	अगर (other != शून्य) {
		log_assert(आकार() == other->आकार());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	माप_प्रकार write_idx = 0;
	क्रम (माप_प्रकार read_idx = 0; read_idx < bits_.आकार(); read_idx++) {
		अगर (!(bits_[read_idx].wire != शून्य && pattern.गिनती(bits_[read_idx]))) {
			अगर (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				अगर (other != शून्य)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	bool modified = (write_idx < bits_.आकार());
	अगर (modified) {
		bits_.resize(write_idx);
		hash_.सफा();
		try_repack();
	}
	अगर (other != शून्य && modified) {
		other->bits_.resize(write_idx);
		other->hash_.सफा();
		other->try_repack();
	}
	check();
}

व्योम RTLIL::SigSpec::remove2(स्थिर std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	अगर (other != शून्य) {
		log_assert(आकार() == other->आकार());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	माप_प्रकार write_idx = 0;
	क्रम (माप_प्रकार read_idx = 0; read_idx < bits_.आकार(); read_idx++) {
		अगर (!(bits_[read_idx].wire != शून्य && pattern.गिनती(bits_[read_idx]))) {
			अगर (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				अगर (other != शून्य)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	bool modified = (write_idx < bits_.आकार());
	अगर (modified) {
		bits_.resize(write_idx);
		hash_.सफा();
		try_repack();
	}
	अगर (other != शून्य && modified) {
		other->bits_.resize(write_idx);
		other->hash_.सफा();
		other->try_repack();
	}
	check();
}

व्योम RTLIL::SigSpec::remove2(स्थिर pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	अगर (other != शून्य) {
		log_assert(आकार() == other->आकार());
		other->unpack();
	}

	bool modified = false;
	bool other_modified = false;
	क्रम (पूर्णांक i = Get_आकार(bits_) - 1; i >= 0; i--) {
		अगर (bits_[i].wire != शून्य && pattern.गिनती(bits_[i].wire)) {
			modified = true;
			bits_.erase(bits_.begin() + i);
			अगर (other != शून्य) {
				other_modified = true;
				other->bits_.erase(other->bits_.begin() + i);
			}
		}
	}

	अगर (modified) {
		hash_.सफा();
		try_repack();
	}
	अगर (other_modified) {
		other->hash_.सफा();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(स्थिर RTLIL::SigSpec &pattern, स्थिर RTLIL::SigSpec *other) स्थिर
{
	log_assert(other == शून्य || आकार() == other->आकार());

	RTLIL::SigSpec ret;
	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();

	क्रम (स्वतः& pattern_chunk : pattern.chunks()) {
		अगर (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			पूर्णांक i = 0;
			क्रम (स्थिर RTLIL::SigBit &bit : bits_match) {
				अगर (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} अन्यथा {
			क्रम (स्थिर RTLIL::SigBit &bit : bits_match)
				अगर (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	वापस ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(स्थिर pool<RTLIL::SigBit> &pattern, स्थिर RTLIL::SigSpec *other) स्थिर
{
	log_assert(other == शून्य || आकार() == other->आकार());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	अगर (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		पूर्णांक i = 0;
		क्रम (स्थिर RTLIL::SigBit &bit : bits_match) {
			अगर (bit.wire && pattern.गिनती(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} अन्यथा {
		क्रम (स्थिर RTLIL::SigBit &bit : bits_match)
			अगर (bit.wire && pattern.गिनती(bit))
				ret.append(bit);
	}

	ret.check();
	वापस ret;
}

व्योम RTLIL::SigSpec::replace(पूर्णांक offset, स्थिर RTLIL::SigSpec &with)
{
	अगर (with.आकार() == 0)
		वापस;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.आकार() >= 0);
	log_assert(offset+with.आकार() <= आकार());

	पूर्णांक i = 0;
	क्रम (स्थिर RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
		++i;
	}
	hash_.सफा();
	try_repack();

	check();
}

व्योम RTLIL::SigSpec::remove_const()
{
	अगर (rep_ == CHUNK)
	{
		अगर (chunk_.wire == शून्य) {
			chunk_.~SigChunk();
			init_empty_bits();
			hash_.सफा();
		}
	}
	अन्यथा
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.आकार());
		क्रम (स्वतः &bit : bits_)
			अगर (bit.wire != शून्य)
				new_bits.धकेलो_back(bit);
		अगर (Get_आकार(new_bits) != Get_आकार(bits_)) {
			bits_.swap(new_bits);
			hash_.सफा();
			try_repack();
		}
	}

	check();
}

व्योम RTLIL::SigSpec::हटाओ(पूर्णांक offset, पूर्णांक लंबाई)
{
	अगर (लंबाई == 0)
		वापस;

	unpack();

	log_assert(offset >= 0);
	log_assert(लंबाई >= 0);
	log_assert(offset + लंबाई <= आकार());

	bits_.erase(bits_.begin() + offset, bits_.begin() + offset + लंबाई);

	hash_.सफा();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(पूर्णांक offset, पूर्णांक लंबाई) स्थिर
{
	log_assert(offset >= 0);
	log_assert(लंबाई >= 0);
	log_assert(offset + लंबाई <= आकार());

	std::vector<SigBit> extracted;
	SigBit first;
	bool is_packing = true;
	क्रम (पूर्णांक i = offset; i < offset + लंबाई; i++) {
		bool was_packing_before = is_packing;
		SigBit bit = (*यह)[i];
		अगर (i == offset) {
			first = bit;
			अगर (!bit.wire)
				is_packing = false;
		} अन्यथा {
			अगर (bit.wire != first.wire)
				is_packing = false;
			अगर (bit.wire)
				अगर (bit.offset != first.offset + (i - offset))
					is_packing = false;
		}
		अगर (was_packing_before && !is_packing)
			क्रम (पूर्णांक j = offset; j < i; j++)
				extracted.धकेलो_back((*यह)[j]);
		अगर (!is_packing)
			extracted.धकेलो_back((*यह)[i]);
	}
	अगर (is_packing)
		वापस SigChunk(first.wire, first.offset, लंबाई);

	वापस extracted;
}

व्योम RTLIL::SigSpec::rewrite_wires(std::function<व्योम(RTLIL::Wire*& wire)> rewrite)
{
	अगर (rep_ == CHUNK) {
		अगर (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		वापस;
	}

	std::vector<RTLIL::SigBit> new_bits;
	क्रम (स्थिर RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		अगर (c.wire != nullptr)
			rewrite(c.wire);
		क्रम (पूर्णांक i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.सफा();
}

व्योम RTLIL::SigSpec::append(स्थिर RTLIL::SigSpec &संकेत)
{
	अगर (संकेत.आकार() == 0)
		वापस;

	अगर (आकार() == 0) {
		*यह = संकेत;
		वापस;
	}

	hash_.सफा();
	अगर (rep_ == CHUNK && संकेत.rep_ == CHUNK && chunk_.wire == संकेत.chunk_.wire) {
		अगर (chunk_.wire == शून्य) {
			chunk_.तथ्य.जोड़ो(chunk_.तथ्य.end(), संकेत.chunk_.तथ्य.begin(), संकेत.chunk_.तथ्य.end());
			chunk_.width = Get_आकार(chunk_.तथ्य);
			वापस;
		}
		अगर (chunk_.offset + chunk_.width == संकेत.chunk_.offset) {
			chunk_.width += संकेत.chunk_.width;
			वापस;
		}
	}

	unpack();
	क्रम (स्थिर SigBit &bit : संकेत.bits())
		bits_.धकेलो_back(bit);
	check();
}

व्योम RTLIL::SigSpec::append(स्थिर RTLIL::SigBit &bit)
{
	hash_.सफा();

	अगर (आकार() == 0) {
		destroy();
		rep_ = CHUNK;
		नया (&chunk_) RTLIL::SigChunk(bit);
		वापस;
	}

	अगर (rep_ == CHUNK && chunk_.wire == bit.wire) {
		अगर (chunk_.wire == शून्य) {
			chunk_.तथ्य.धकेलो_back(bit.तथ्य);
			chunk_.width++;
			वापस;
		}
		अगर (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			वापस;
		}
	}

	unpack();

	bits_.धकेलो_back(bit);
	check();
}

व्योम RTLIL::SigSpec::extend_u0(पूर्णांक width, bool is_signed)
{
	अगर (आकार() > width)
		हटाओ(width, आकार() - width);

	अगर (आकार() < width) {
		RTLIL::SigBit padding = आकार() > 0 ? (*यह)[आकार() - 1] : RTLIL::State::Sx;
		अगर (!is_signed)
			padding = RTLIL::State::S0;
		जबतक (आकार() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(पूर्णांक अंक) स्थिर
{
	RTLIL::SigSpec sig;
	क्रम (पूर्णांक i = 0; i < अंक; i++)
		sig.append(*यह);
	वापस sig;
}

#अगर_अघोषित न_संशोधन
व्योम RTLIL::SigSpec::check(स्थिर Module *mod) स्थिर
{
	अगर (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		अगर (chunk_.wire == शून्य) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.तथ्य.आकार() == (माप_प्रकार)chunk_.width);
		} अन्यथा {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.तथ्य.आकार() == 0);
			अगर (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	अन्यथा अगर (आकार() <= 64)
	{
		अगर (mod != nullptr) {
			क्रम (स्थिर RTLIL::SigBit &bit : bits_)
				अगर (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#पूर्ण_अगर

bool RTLIL::SigSpec::चालक <(स्थिर RTLIL::SigSpec &other) स्थिर
{
	अगर (यह == &other)
		वापस false;

	अगर (आकार() != other.आकार())
		वापस आकार() < other.आकार();

	स्वतः other_it = other.chunks().begin();
	क्रम (स्थिर SigChunk &c : chunks()) {
		अगर (c != *other_it)
			वापस c < *other_it;
		++other_it;
	}

	वापस false;
}

bool RTLIL::SigSpec::चालक ==(स्थिर RTLIL::SigSpec &other) स्थिर
{
	अगर (यह == &other)
		वापस true;

	अगर (आकार() != other.आकार())
		वापस false;

	स्वतः other_it = other.chunks().begin();
	क्रम (स्थिर SigChunk &c : chunks()) {
		अगर (c != *other_it)
			वापस false;
		++other_it;
	}

	वापस true;
}

bool RTLIL::SigSpec::is_wire() स्थिर
{
	Chunks cs = chunks();
	स्वतः it = cs.begin();
	अगर (it == cs.end())
		वापस false;
	स्थिर RTLIL::SigChunk &chunk = *it;
	वापस chunk.wire && chunk.wire->width == आकार() && ++it == cs.end();
}

bool RTLIL::SigSpec::is_chunk() स्थिर
{
	Chunks cs = chunks();
	स्वतः it = cs.begin();
	अगर (it == cs.end())
		वापस false;
	वापस ++it == cs.end();
}

bool RTLIL::SigSpec::known_driver() स्थिर
{
	क्रम (स्वतः &chunk : chunks())
		अगर (chunk.is_wire() && !chunk.wire->known_driver())
			वापस false;
	वापस true;
}

bool RTLIL::SigSpec::is_fully_const() स्थिर
{
	क्रम (स्वतः &chunk : chunks())
		अगर (chunk.width > 0 && chunk.wire != शून्य)
			वापस false;
	वापस true;
}

bool RTLIL::SigSpec::is_fully_zero() स्थिर
{
	क्रम (स्वतः &chunk : chunks()) {
		अगर (chunk.width > 0 && chunk.wire != शून्य)
			वापस false;
		क्रम (RTLIL::State d : chunk.तथ्य)
			अगर (d != RTLIL::State::S0)
				वापस false;
	}
	वापस true;
}

bool RTLIL::SigSpec::is_fully_ones() स्थिर
{
	क्रम (स्वतः &chunk : chunks()) {
		अगर (chunk.width > 0 && chunk.wire != शून्य)
			वापस false;
		क्रम (RTLIL::State d : chunk.तथ्य)
			अगर (d != RTLIL::State::S1)
				वापस false;
	}
	वापस true;
}

bool RTLIL::SigSpec::is_fully_def() स्थिर
{
	क्रम (स्वतः &chunk : chunks()) {
		अगर (chunk.width > 0 && chunk.wire != शून्य)
			वापस false;
		क्रम (RTLIL::State d : chunk.तथ्य)
			अगर (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				वापस false;
	}
	वापस true;
}

bool RTLIL::SigSpec::is_fully_undef() स्थिर
{
	क्रम (स्वतः &chunk : chunks()) {
		अगर (chunk.width > 0 && chunk.wire != शून्य)
			वापस false;
		क्रम (RTLIL::State d : chunk.तथ्य)
			अगर (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				वापस false;
	}
	वापस true;
}

bool RTLIL::SigSpec::has_const() स्थिर
{
	क्रम (स्वतः &chunk : chunks())
		अगर (chunk.width > 0 && chunk.wire == शून्य)
			वापस true;
	वापस false;
}

bool RTLIL::SigSpec::has_const(State state) स्थिर
{
	क्रम (स्वतः &chunk : chunks())
		अगर (chunk.width > 0 && chunk.wire == शून्य && std::find(chunk.तथ्य.begin(), chunk.तथ्य.end(), state) != chunk.तथ्य.end())
			वापस true;
	वापस false;
}


bool RTLIL::SigSpec::has_marked_bits() स्थिर
{
	क्रम (स्वतः &chunk : chunks())
		अगर (chunk.width > 0 && chunk.wire == शून्य) {
			क्रम (RTLIL::State d : chunk.तथ्य)
				अगर (d == RTLIL::State::Sm)
					वापस true;
		}
	वापस false;
}

bool RTLIL::SigSpec::is_onehot(पूर्णांक *pos) स्थिर
{
	अगर (std::optional<RTLIL::Const> c = try_as_const())
		वापस c->is_onehot(pos);
	वापस false;
}

bool RTLIL::SigSpec::as_bool() स्थिर
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मूल्य());
	वापस c->as_bool();
}

पूर्णांक RTLIL::SigSpec::as_int(bool is_signed) स्थिर
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मूल्य());
	वापस c->as_int(is_signed);
}

bool RTLIL::SigSpec::convertible_to_int(bool is_signed) स्थिर
{
	std::optional<RTLIL::Const> c = try_as_const();
	अगर (!c.has_मूल्य())
		वापस false;
	वापस c->convertible_to_int(is_signed);
}

std::optional<पूर्णांक> RTLIL::SigSpec::try_as_int(bool is_signed) स्थिर
{
	std::optional<RTLIL::Const> c = try_as_const();
	अगर (!c.has_मूल्य())
		वापस std::nullopt;
	वापस c->try_as_int(is_signed);
}

पूर्णांक RTLIL::SigSpec::as_int_saturating(bool is_signed) स्थिर
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मूल्य());
	वापस c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() स्थिर
{
	std::string str;
	str.reserve(आकार());
	std::vector<RTLIL::SigChunk> chunks = *यह;
	क्रम (माप_प्रकार i = chunks.आकार(); i > 0; i--) {
		स्थिर RTLIL::SigChunk &chunk = chunks[i-1];
		अगर (chunk.wire != शून्य)
			str.append(chunk.width, '?');
		अन्यथा
			str += RTLIL::Const(chunk.तथ्य).as_string();
	}
	वापस str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() स्थिर
{
	Chunks cs = chunks();
	स्वतः it = cs.begin();
	अगर (it == cs.end())
		वापस RTLIL::Const();
	SigChunk chunk = *it;
	अगर (chunk.wire != शून्य || ++it != cs.end())
		वापस std::nullopt;
	वापस RTLIL::Const(std::move(chunk.तथ्य));
}

RTLIL::Const RTLIL::SigSpec::as_const() स्थिर
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मूल्य());
	वापस *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() स्थिर
{
	Chunks cs = chunks();
	स्वतः it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == आकार());
	वापस chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() स्थिर
{
	Chunks cs = chunks();
	स्वतः it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	वापस chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() स्थिर
{
	वापस RTLIL::SigBit(*यह);
}

bool RTLIL::SigSpec::match(स्थिर अक्षर* pattern) स्थिर
{
	पूर्णांक pattern_len = म_माप(pattern);
	log_assert(pattern_len == आकार());

	क्रम (पूर्णांक i = 0; i < pattern_len; ++i) {
		अक्षर ch = pattern[i];
		अगर (ch == ' ')
			जारी;
		RTLIL::SigBit bit = (*यह)[pattern_len - 1 - i];
		अगर (ch == '*') {
			अगर (bit != State::Sz && bit != State::Sx)
				वापस false;
			जारी;
		}
		अगर (ch == '0') {
			अगर (bit != State::S0)
				वापस false;
		} अन्यथा
		अगर (ch == '1') {
			अगर (bit != State::S1)
				वापस false;
		} अन्यथा
			log_abort();
	}

	वापस true;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() स्थिर
{
	std::set<RTLIL::SigBit> sigbits;
	क्रम (स्वतः &c : chunks())
		क्रम (पूर्णांक i = 0; i < c.width; i++)
			sigbits.जोड़ो(RTLIL::SigBit(c, i));
	वापस sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() स्थिर
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(आकार());
	क्रम (स्वतः &c : chunks())
		क्रम (पूर्णांक i = 0; i < c.width; i++)
			sigbits.जोड़ो(RTLIL::SigBit(c, i));
	वापस sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() स्थिर
{
	std::vector<RTLIL::SigBit> परिणाम;
	परिणाम.reserve(आकार());
	क्रम (SigBit bit : *यह)
		परिणाम.धकेलो_back(bit);
	वापस परिणाम;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(स्थिर RTLIL::SigSpec &other) स्थिर
{
	पूर्णांक this_आकार = आकार();
	log_assert(this_आकार == other.आकार());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	क्रम (पूर्णांक i = 0; i < this_आकार; i++)
		new_map[(*यह)[i]] = other[i];

	वापस new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(स्थिर RTLIL::SigSpec &other) स्थिर
{
	पूर्णांक this_आकार = आकार();
	log_assert(this_आकार == other.आकार());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_आकार);
	क्रम (पूर्णांक i = 0; i < this_आकार; i++)
		new_map[(*यह)[i]] = other[i];

	वापस new_map;
}

अटल व्योम sigspec_parse_split(std::vector<std::string> &tokens, स्थिर std::string &text, अक्षर sep)
{
	माप_प्रकार start = 0, end = 0;
	जबतक ((end = text.find(sep, start)) != std::string::npos) {
		tokens.धकेलो_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.धकेलो_back(text.substr(start));
}

अटल RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, स्थिर std::string &netname)
{
	वापस module->wire(netname);
}

bool RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	क्रम (पूर्णांक tokidx = पूर्णांक(tokens.आकार())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		अगर (netname.आकार() == 0)
			जारी;

		अगर (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			स्वतः ast = p.const2ast(netname);
			अगर (ast == nullptr)
				वापस false;
			sig.append(RTLIL::Const(ast->bits));
			जारी;
		}

		अगर (module == शून्य)
			वापस false;

		अगर (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		अगर (sigspec_parse_wire(module, netname) == nullptr) {
			माप_प्रकार indices_pos = netname.आकार()-1;
			अगर (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				जबतक (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				अगर (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					जबतक (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				अगर (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		अगर (wire == nullptr)
			वापस false;

		अगर (!indices.empty()) {
			std::vector<std::string> सूचकांक_tokens;
			sigspec_parse_split(सूचकांक_tokens, indices.substr(1, indices.आकार()-2), ':');
			अगर (सूचकांक_tokens.आकार() == 1) {
				पूर्णांक a = म_से_प(सूचकांक_tokens.at(0).c_str());
				अगर (a < 0 || a >= wire->width)
					वापस false;
				sig.append(RTLIL::SigSpec(wire, a));
			} अन्यथा {
				पूर्णांक a = म_से_प(सूचकांक_tokens.at(0).c_str());
				पूर्णांक b = म_से_प(सूचकांक_tokens.at(1).c_str());
				अगर (a > b) {
					पूर्णांक tmp = a;
					a = b, b = tmp;
				}
				अगर (a < 0 || a >= wire->width)
					वापस false;
				अगर (b < 0 || b >= wire->width)
					वापस false;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} अन्यथा
			sig.append(wire);
	}

	वापस true;
}

bool RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	अगर (str.empty() || str[0] != '@')
		वापस parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	अगर (sel_name == IdString::Null || design->selection_vars.गिनती(sel_name) == 0)
		वापस false;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	क्रम (स्वतः &it : module->wires_)
		अगर (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	वापस true;
}

bool RTLIL::SigSpec::parse_rhs(स्थिर RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	अगर (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.आकार());
		वापस true;
	}

	अगर (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.आकार());
		वापस true;
	}

	अगर (lhs.is_chunk()) {
		अक्षर *p = (अक्षर*)str.c_str(), *endptr;
		दीर्घ पूर्णांक val = म_से_दीर्घ(p, &endptr, 10);
		अगर (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.आकार());
			वापस true;
		}
	}

	अगर (!parse(sig, module, str))
		वापस false;
	अगर (sig.आकार() > lhs.आकार())
		sig.हटाओ(lhs.आकार(), sig.आकार() - lhs.आकार());
	वापस true;
}

RTLIL::SigSpec::चालक std::vector<RTLIL::SigChunk>() स्थिर
{
	std::vector<RTLIL::SigChunk> परिणाम;
	क्रम (स्थिर RTLIL::SigChunk &c : chunks())
		परिणाम.धकेलो_back(c);
	वापस परिणाम;
}

RTLIL::CaseRule::~CaseRule()
{
	क्रम (स्वतः it = switches.begin(); it != switches.end(); it++)
		मिटाओ *it;
}

bool RTLIL::CaseRule::empty() स्थिर
{
	वापस actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() स्थिर
{
	RTLIL::CaseRule *new_caserule = नया RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	क्रम (स्वतः &it : switches)
		new_caserule->switches.धकेलो_back(it->clone());
	वापस new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	क्रम (स्वतः it = cases.begin(); it != cases.end(); it++)
		मिटाओ *it;
}

bool RTLIL::SwitchRule::empty() स्थिर
{
	वापस cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() स्थिर
{
	RTLIL::SwitchRule *new_switchrule = नया RTLIL::SwitchRule;
	new_switchrule->संकेत = संकेत;
	new_switchrule->attributes = attributes;
	क्रम (स्वतः &it : cases)
		new_switchrule->cases.धकेलो_back(it->clone());
	वापस new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() स्थिर
{
	RTLIL::SyncRule *new_syncrule = नया RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->संकेत = संकेत;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	वापस new_syncrule;
}

RTLIL::Process::~Process()
{
	क्रम (स्वतः it = syncs.begin(); it != syncs.end(); it++)
		मिटाओ *it;
}

RTLIL::Process *RTLIL::Process::clone() स्थिर
{
	RTLIL::Process *new_proc = नया RTLIL::Process;

	new_proc->attributes = attributes;

	RTLIL::CaseRule *rc_ptr = मूल_case.clone();
	new_proc->मूल_case = *rc_ptr;
	rc_ptr->switches.सफा();
	मिटाओ rc_ptr;

	क्रम (स्वतः &it : syncs)
		new_proc->syncs.धकेलो_back(it->clone());

	वापस new_proc;
}

RTLIL::Memory::~Memory()
{
#अगर_घोषित YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->erase(hashidx_);
#पूर्ण_अगर
}

#अगर_घोषित YOSYS_ENABLE_PYTHON
अटल std::map<अचिन्हित पूर्णांक, RTLIL::Memory*> all_memorys;
std::map<अचिन्हित पूर्णांक, RTLIL::Memory*> *RTLIL::Memory::get_all_memorys(व्योम)
{
	वापस &all_memorys;
}
#पूर्ण_अगर
YOSYS_NAMESPACE_END
