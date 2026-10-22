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

स्थिरांक pool<IdString> &RTLIL::builtin_ff_cell_types() {
	स्थिर स्थिरांक pool<IdString> res = []() {
		pool<IdString> r;
		साठी (माप_प्रकार i = 0; i < StaticCellTypes::builder.मोजणी; i++) {
			स्वतः &cell = StaticCellTypes::builder.cells[i];
			जर (cell.features.is_ff)
				r.घाला(cell.type);
		}
		परत r;
	}();
	परत res;
}

#define check(condition) log_assert(condition && "malformed Const union")

स्थिरांक Const::bitvectype& Const::get_bits() स्थिरांक {
	check(is_bits());
	परत *get_if_bits();
}

स्थिरांक std::string& Const::get_str() स्थिरांक {
	check(is_str());
	परत *get_if_str();
}

Const::bitvectype& Const::get_bits() {
	check(is_bits());
	परत *get_if_bits();
}

std::string& Const::get_str() {
	check(is_str());
	परत *get_if_str();
}

RTLIL::Const::Const(std::string str)
{
	flags = RTLIL::CONST_FLAG_STRING;
	नवीन ((शून्य*)&str_) std::string(std::move(str));
	tag = backing_tag::string;
}

RTLIL::Const::Const(दीर्घ दीर्घ val) // default width 32
{
	flags = RTLIL::CONST_FLAG_NONE;
	अक्षर bytes[] = {
		(अक्षर)(val >> 24), (अक्षर)(val >> 16), (अक्षर)(val >> 8), (अक्षर)val
	};
	नवीन ((शून्य*)&str_) std::string(bytes, 4);
	tag = backing_tag::string;
}

RTLIL::Const::Const(दीर्घ दीर्घ val, पूर्णांक width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	जर ((width & 7) == 0) {
		नवीन ((शून्य*)&str_) std::string();
		tag = backing_tag::string;
		std::string& str = get_str();
		पूर्णांक bytes = width >> 3;
		चिन्हित अक्षर sign_byte = val < 0 ? -1 : 0;
		str.resize(bytes, sign_byte);
		bytes = std::min<पूर्णांक>(bytes, आकार(val));
		साठी (पूर्णांक i = 0; i < bytes; i++) {
			str[str.size() - 1 - i] = val;
			val = val >> 8;
		}
		परत;
	}

	नवीन ((शून्य*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	साठी (पूर्णांक i = 0; i < width; i++) {
		bv.ढकला_back((val & 1) != 0 ? State::S1 : State::S0);
		val = val >> 1;
	}
}

RTLIL::Const::Const(RTLIL::State bit, पूर्णांक width)
{
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	flags = RTLIL::CONST_FLAG_NONE;
	नवीन ((शून्य*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(width);
	साठी (पूर्णांक i = 0; i < width; i++)
		bv.ढकला_back(bit);
}

RTLIL::Const::Const(स्थिरांक std::vector<तार्किक> &bits)
{
	flags = RTLIL::CONST_FLAG_NONE;
	नवीन ((शून्य*)&bits_) bitvectype();
	tag = backing_tag::bits;
	bitvectype& bv = get_bits();
	bv.reserve(bits.size());
	साठी (स्थिरांक स्वतः &b : bits)
		bv.emplace_back(b ? State::S1 : State::S0);
}

RTLIL::Const::Const(स्थिरांक RTLIL::Const &other) {
	tag = other.tag;
	flags = other.flags;
	जर (is_str())
		नवीन ((शून्य*)&str_) std::string(other.get_str());
	अन्यथा जर (is_bits())
		नवीन ((शून्य*)&bits_) bitvectype(other.get_bits());
	अन्यथा
		check(असत्य);
}

RTLIL::Const::Const(RTLIL::Const &&other) {
	tag = other.tag;
	flags = other.flags;
	जर (is_str())
		नवीन ((शून्य*)&str_) std::string(std::move(other.get_str()));
	अन्यथा जर (is_bits())
		नवीन ((शून्य*)&bits_) bitvectype(std::move(other.get_bits()));
	अन्यथा
		check(असत्य);
}

RTLIL::Const &RTLIL::Const::चालक =(स्थिरांक RTLIL::Const &other) {
	flags = other.flags;
	जर (other.is_str()) {
		जर (!is_str()) {
			// sketchy zone
			check(is_bits());
			bits_.~bitvectype();
			(शून्य)नवीन ((शून्य*)&str_) std::string();
		}
		tag = other.tag;
		get_str() = other.get_str();
	} अन्यथा जर (other.is_bits()) {
		जर (!is_bits()) {
			// sketchy zone
			check(is_str());
			str_.~string();
			(शून्य)नवीन ((शून्य*)&bits_) bitvectype();
		}
		tag = other.tag;
		get_bits() = other.get_bits();
	} अन्यथा {
		check(असत्य);
	}
	परत *हे;
}

RTLIL::Const::~Const() {
	जर (is_bits())
		bits_.~bitvectype();
	अन्यथा जर (is_str())
		str_.~string();
	अन्यथा
		check(असत्य);
}

तार्किक RTLIL::Const::चालक<(स्थिरांक RTLIL::Const &other) स्थिरांक
{
	जर (size() != other.size())
		परत size() < other.size();

	साठी (पूर्णांक i = 0; i < size(); i++)
		जर ((*हे)[i] != other[i])
			परत (*हे)[i] < other[i];

	परत असत्य;
}

तार्किक RTLIL::Const::चालक ==(स्थिरांक RTLIL::Const &other) स्थिरांक
{
	जर (is_str() && other.is_str())
		परत get_str() == other.get_str();
	जर (is_bits() && other.is_bits())
		परत get_bits() == other.get_bits();

	जर (size() != other.size())
		परत असत्य;

	साठी (पूर्णांक i = 0; i < size(); i++)
	जर ((*हे)[i] != other[i])
		परत असत्य;

	परत सत्य;
}

तार्किक RTLIL::Const::चालक !=(स्थिरांक RTLIL::Const &other) स्थिरांक
{
	परत !(*हे == other);
}

std::vector<RTLIL::State>& RTLIL::Const::bits_internal()
{
	bitvectorize_internal();
	परत get_bits();
}

std::vector<RTLIL::State> RTLIL::Const::to_bits() स्थिरांक
{
	std::vector<State> v;
	साठी (स्वतः bit : *हे)
		v.ढकला_back(bit);
	परत v;
}

तार्किक RTLIL::Const::as_bool() स्थिरांक
{
	जर (is_str()) {
		साठी (अक्षर ch : get_str())
			जर (ch != 0)
				परत सत्य;
		परत असत्य;
	}

	स्थिरांक bitvectype& bv = get_bits();
	साठी (माप_प्रकार i = 0; i < bv.size(); i++)
		जर (bv[i] == State::S1)
			परत सत्य;
	परत असत्य;
}

पूर्णांक RTLIL::Const::as_int(तार्किक is_signed) स्थिरांक
{
	int32_t ret = 0;
	जर (स्थिरांक std::string *s = get_if_str()) {
		पूर्णांक size = GetSize(*s);
		// Ignore any bytes after the first 4 since bits beyond 32 are truncated.
		साठी (पूर्णांक i = std::min(4, size); i > 0; i--)
			ret |= static_cast<अचिन्हित अक्षर>((*s)[size - i]) << ((i - 1) * 8);
		// If is_signed and the string is shorter than 4 bytes then apply sign extension.
		जर (is_signed && size > 0 && size < 4 && ((*s)[0] & 0x80))
			ret |= UINT32_MAX << size*8;
		परत ret;
	}

	स्थिरांक bitvectype& bv = get_bits();
	पूर्णांक significant_bits = std::min(GetSize(bv), 32);
	साठी (पूर्णांक i = 0; i < significant_bits; i++)
		जर (bv[i] == State::S1)
			ret |= 1 << i;
	जर (is_signed && significant_bits > 0 && significant_bits < 32 && bv.back() == State::S1 )
		ret |= UINT32_MAX << significant_bits;
	परत ret;
}

तार्किक RTLIL::Const::convertible_to_int(तार्किक is_signed) स्थिरांक
{
	स्वतः size = get_min_size(is_signed);

	जर (size < 0)
		परत असत्य;

	// If it fits in 31 bits it is definitely convertible
	जर (size <= 31)
		परत सत्य;

	// If it fits in 32 bits, it is convertible if signed or if unsigned and the
	// leading bit is not 1
	जर (size == 32) {
		जर (is_signed)
			परत सत्य;
		परत (*हे)[size - 1] != State::S1;
	}

	परत असत्य;
}

std::optional<पूर्णांक> RTLIL::Const::try_as_int(तार्किक is_signed) स्थिरांक
{
	जर (!convertible_to_int(is_signed))
		परत std::nullopt;
	परत as_int(is_signed);
}

पूर्णांक RTLIL::Const::as_int_saturating(तार्किक is_signed) स्थिरांक
{
	जर (!convertible_to_int(is_signed)) {
		जर (!is_signed)
			परत std::numeric_limits<पूर्णांक>::max();

		स्थिरांक स्वतः min_size = get_min_size(is_signed);
		log_assert(min_size > 0);
		स्थिरांक स्वतः neg = (*हे)[min_size - 1];
		परत neg ? std::numeric_limits<पूर्णांक>::min() : std::numeric_limits<पूर्णांक>::max();
	}
	परत as_int(is_signed);
}

शून्य RTLIL::Const::tag_bare_integer_const(स्थिरांक std::string &मूल्य)
{
	जर (मूल्य.empty() || मूल्य.find('\'') != std::string::npos)
		परत;
	माप_प्रकार start = (मूल्य[0] == '-' || मूल्य[0] == '+') ? 1 : 0;
	जर (start < मूल्य.size() && std::all_of(मूल्य.begin() + start, मूल्य.end(), ::है_अंक))
		flags |= RTLIL::CONST_FLAG_SIGNED;
}

पूर्णांक RTLIL::Const::get_min_size(तार्किक is_signed) स्थिरांक
{
	जर (empty()) परत 0;

	// back to front (MSB to LSB)
	RTLIL::State leading_bit;
	जर (is_signed)
		leading_bit = (back() == RTLIL::State::Sx) ? RTLIL::State::S0 : back();
	अन्यथा
		leading_bit = RTLIL::State::S0;

	स्वतः idx = size();
	पर्यंत (idx > 0 && (*हे)[idx -1] == leading_bit) {
		idx--;
	}

	// signed needs one leading bit
	जर (is_signed && idx < size()) {
		idx++;
	}
	// must be at least one bit
	परत (idx == 0) ? 1 : idx;
}

शून्य RTLIL::Const::compress(तार्किक is_signed)
{
	स्वतः idx = get_min_size(is_signed);
	resize(idx, RTLIL::State::S0);
}

std::optional<पूर्णांक> RTLIL::Const::as_int_compress(तार्किक is_signed) स्थिरांक
{
	परत try_as_int(is_signed);
}

std::string RTLIL::Const::as_string(स्थिरांक अक्षर* any) स्थिरांक
{
	पूर्णांक sz = size();
	std::string ret;
	ret.reserve(sz);
	साठी (पूर्णांक i = sz - 1; i >= 0; --i)
		निवड ((*हे)[i]) {
			स्थिती S0: ret.ढकला_back('0'); थांबा;
			स्थिती S1: ret.ढकला_back('1'); थांबा;
			स्थिती Sx: ret.ढकला_back('x'); थांबा;
			स्थिती Sz: ret.ढकला_back('z'); थांबा;
			स्थिती Sa: ret += any; थांबा;
			स्थिती Sm: ret.ढकला_back('m'); थांबा;
		}
	परत ret;
}

RTLIL::Const RTLIL::Const::from_string(स्थिरांक std::string &str)
{
	Const c;
	bitvectype& bv = c.get_bits();
	bv.reserve(str.size());
	साठी (स्वतः it = str.rbegin(); it != str.rend(); it++)
		निवड (*it) {
			स्थिती '0': bv.ढकला_back(State::S0); थांबा;
			स्थिती '1': bv.ढकला_back(State::S1); थांबा;
			स्थिती 'x': bv.ढकला_back(State::Sx); थांबा;
			स्थिती 'z': bv.ढकला_back(State::Sz); थांबा;
			स्थिती 'm': bv.ढकला_back(State::Sm); थांबा;
			पूर्वनिर्धारित: bv.ढकला_back(State::Sa);
		}
	परत c;
}

std::string RTLIL::Const::decode_string() स्थिरांक
{
	जर (स्वतः str = get_if_str())
		परत *str;

	स्थिरांक bitvectype& bv = get_bits();
	स्थिरांक पूर्णांक n = GetSize(bv);
	स्थिरांक पूर्णांक n_over_8 = n / 8;
	std::string s;
	s.reserve(n_over_8);
	पूर्णांक i = n_over_8 * 8;
	जर (i < n) {
		अक्षर ch = 0;
		साठी (पूर्णांक j = 0; j < (n - i); j++) {
			जर (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		जर (ch != 0)
			s.append({ch});
	}
	i -= 8;
	साठी (; i >= 0; i -= 8) {
		अक्षर ch = 0;
		साठी (पूर्णांक j = 0; j < 8; j++) {
			जर (bv[i + j] == RTLIL::State::S1) {
				ch |= 1 << j;
			}
		}
		जर (ch != 0)
			s.append({ch});
	}
	परत s;
}

पूर्णांक RTLIL::Const::size() स्थिरांक {
	जर (is_str())
		परत 8 * str_.size();
	अन्यथा {
		check(is_bits());
		परत bits_.size();
	}
}

तार्किक RTLIL::Const::empty() स्थिरांक {
	जर (is_str())
		परत str_.empty();
	अन्यथा {
		check(is_bits());
		परत bits_.empty();
	}
}

शून्य RTLIL::Const::bitvectorize_internal() {
	जर (tag == backing_tag::bits)
		परत;

	check(is_str());

	bitvectype new_bits;

	new_bits.reserve(str_.size() * 8);
	साठी (पूर्णांक i = str_.size() - 1; i >= 0; i--) {
		अचिन्हित अक्षर ch = str_[i];
		साठी (पूर्णांक j = 0; j < 8; j++) {
			new_bits.ढकला_back((ch & 1) != 0 ? State::S1 : State::S0);
			ch = ch >> 1;
		}
	}

	{
		// sketchy zone
		str_.~string();
		(शून्य)नवीन ((शून्य*)&bits_) bitvectype(std::move(new_bits));
		tag = backing_tag::bits;
	}
}

शून्य RTLIL::Const::append(स्थिरांक RTLIL::Const &other) {
	bitvectorize_internal();
	bitvectype& bv = get_bits();
	bv.घाला(bv.end(), other.begin(), other.end());
}

RTLIL::State RTLIL::Const::const_iterator::चालक*() स्थिरांक {
	जर (स्वतः bv = parent->get_if_bits())
		परत (*bv)[idx];

	पूर्णांक char_idx = parent->get_str().size() - idx / 8 - 1;
	तार्किक bit = (parent->get_str()[char_idx] & (1 << (idx % 8)));
	परत bit ? State::S1 : State::S0;
}

तार्किक RTLIL::Const::is_fully_zero() स्थिरांक
{
	जर (स्वतः str = get_if_str()) {
		साठी (अक्षर ch : *str)
			जर (ch != 0)
				परत असत्य;
		परत सत्य;
	}

	स्थिरांक bitvectype& bv = get_bits();

	साठी (स्थिरांक स्वतः &bit : bv)
		जर (bit != RTLIL::State::S0)
			परत असत्य;

	परत सत्य;
}

तार्किक RTLIL::Const::is_fully_ones() स्थिरांक
{
	जर (स्वतः str = get_if_str()) {
		साठी (अक्षर ch : *str)
			जर (ch != (अक्षर)0xff)
				परत असत्य;
		परत सत्य;
	}

	स्थिरांक bitvectype& bv = get_bits();
	साठी (स्थिरांक स्वतः &bit : bv)
		जर (bit != RTLIL::State::S1)
			परत असत्य;

	परत सत्य;
}

तार्किक RTLIL::Const::is_fully_def() स्थिरांक
{
	जर (is_str())
		परत सत्य;

	स्थिरांक bitvectype& bv = get_bits();
	साठी (स्थिरांक स्वतः &bit : bv)
		जर (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			परत असत्य;

	परत सत्य;
}

तार्किक RTLIL::Const::is_fully_undef() स्थिरांक
{
	जर (स्वतः str = get_if_str())
		परत str->empty();

	स्थिरांक bitvectype& bv = get_bits();
	साठी (स्थिरांक स्वतः &bit : bv)
		जर (bit != RTLIL::State::Sx && bit != RTLIL::State::Sz)
			परत असत्य;

	परत सत्य;
}

तार्किक RTLIL::Const::is_fully_undef_x_only() स्थिरांक
{
	जर (स्वतः str = get_if_str())
		परत str->empty();

	स्थिरांक bitvectype& bv = get_bits();
	साठी (स्थिरांक स्वतः &bit : bv)
		जर (bit != RTLIL::State::Sx)
			परत असत्य;

	परत सत्य;
}

तार्किक RTLIL::Const::is_onehot(पूर्णांक *pos) स्थिरांक
{
	तार्किक found = असत्य;
	पूर्णांक size = GetSize(*हे);
	साठी (पूर्णांक i = 0; i < size; i++) {
		State bit = (*हे)[i];
		जर (bit != RTLIL::State::S0 && bit != RTLIL::State::S1)
			परत असत्य;
		जर (bit == RTLIL::State::S1) {
			जर (found)
				परत असत्य;
			जर (pos)
				*pos = i;
			found = सत्य;
		}
	}
	परत found;
}

Hasher RTLIL::Const::hash_into(Hasher h) स्थिरांक
{
	जर (स्वतः str = get_if_str())
		परत hashlib::hash_ops<std::string>::hash_into(*str, h);

	// If the bits are all 0/1, hash packed bits using the string hash.
	// Otherwise hash the leading packed bits with the rest of the bits individually.
	स्थिरांक bitvectype &bv = get_bits();
	पूर्णांक size = GetSize(bv);
	std::string packed;
	पूर्णांक packed_size = (size + 7) >> 3;
	packed.resize(packed_size, 0);
	साठी (पूर्णांक bi = 0; bi < packed_size; ++bi) {
		अक्षर ch = 0;
		पूर्णांक end = std::min((bi + 1)*8, size);
		साठी (पूर्णांक i = bi*8; i < end; ++i) {
			RTLIL::State b = bv[i];
			जर (b > RTLIL::State::S1) {
				// Hash the packed bits we've seen so far, plus the remaining bits.
				h = hashlib::hash_ops<std::string>::hash_into(packed, h);
				h = hashlib::hash_ops<अक्षर>::hash_into(ch, h);
				साठी (; i < size; ++i) {
					h = hashlib::hash_ops<RTLIL::State>::hash_into(bv[i], h);
				}
				h.eat(size);
				परत h;
			}
			ch |= static_cast<पूर्णांक>(b) << (i & 7);
		}
		packed[packed_size - 1 - bi] = ch;
	}
	परत hashlib::hash_ops<std::string>::hash_into(packed, h);
}

RTLIL::Const RTLIL::Const::extract(पूर्णांक offset, पूर्णांक len, RTLIL::State padding) स्थिरांक {
	bitvectype ret_bv;
	ret_bv.reserve(len);
	साठी (पूर्णांक i = offset; i < offset + len; i++)
		ret_bv.ढकला_back(i < GetSize(*हे) ? (*हे)[i] : padding);
	परत RTLIL::Const(ret_bv);
}
#undef check /* check(condition) for Const */

तार्किक RTLIL::AttrObject::has_attribute(RTLIL::IdString id) स्थिरांक
{
	परत attributes.मोजणी(id);
}

शून्य RTLIL::AttrObject::set_bool_attribute(RTLIL::IdString id, तार्किक मूल्य)
{
	जर (मूल्य)
		attributes[id] = RTLIL::Const(1);
	अन्यथा
		attributes.erase(id);
}

तार्किक RTLIL::AttrObject::get_bool_attribute(RTLIL::IdString id) स्थिरांक
{
	स्थिरांक स्वतः it = attributes.find(id);
	जर (it == attributes.end())
		परत असत्य;
	परत it->second.as_bool();
}

शून्य RTLIL::AttrObject::set_string_attribute(RTLIL::IdString id, string मूल्य)
{
	जर (मूल्य.empty())
		attributes.erase(id);
	अन्यथा
		attributes[id] = मूल्य;
}

string RTLIL::AttrObject::get_string_attribute(RTLIL::IdString id) स्थिरांक
{
	std::string मूल्य;
	स्थिरांक स्वतः it = attributes.find(id);
	जर (it != attributes.end())
		मूल्य = it->second.decode_string();
	परत मूल्य;
}

शून्य RTLIL::AttrObject::set_strpool_attribute(RTLIL::IdString id, स्थिरांक pool<string> &data)
{
	string attrval;
	साठी (स्थिरांक स्वतः &s : data) {
		जर (!attrval.empty())
			attrval += "|";
		attrval += s;
	}
	set_string_attribute(id, attrval);
}

शून्य RTLIL::AttrObject::add_strpool_attribute(RTLIL::IdString id, स्थिरांक pool<string> &data)
{
	pool<string> union_data = get_strpool_attribute(id);
	union_data.घाला(data.begin(), data.end());
	जर (!union_data.empty())
		set_strpool_attribute(id, union_data);
}

शून्य RTLIL::Design::absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf)
{
	obj->attributes = std::move(buf);
}

नामाकाश {
	ढाँचा<typename F>
	शून्य walk_process_attr_objects(RTLIL::Process *process, F visit) {
		visit(process);
		std::vector<RTLIL::CaseRule*> case_चळत{&process->मूळ_case};
		पर्यंत (!case_चळत.empty()) {
			RTLIL::CaseRule *cs = case_चळत.back();
			case_चळत.काढा_back();
			visit(cs);
			साठी (स्वतः *sw : cs->switches) {
				visit(sw);
				साठी (स्वतः *case_ : sw->cases)
					case_चळत.ढकला_back(case_);
			}
		}
		साठी (स्वतः *समकालिक : process->syncs)
			साठी (स्वतः &mwa : समकालिक->mem_write_actions)
				visit(&mwa);
	}

	ढाँचा<typename F>
	शून्य walk_attr_objects(RTLIL::Design *design, F visit) {
		साठी (स्वतः &[_, module] : design->modules_) {
			visit(module);
			साठी (स्वतः &[_, wire] : module->wires_)
				visit(wire);
			साठी (स्वतः &[_, mem] : module->memories)
				visit(mem);
			साठी (स्वतः &[_, cell] : module->cells_)
				visit(cell);
			साठी (स्वतः &[_, process] : module->processes)
				walk_process_attr_objects(process, visit);
		}
	}
}


माप_प्रकार RTLIL::Design::gc_twines()
{
	int64_t start = PerformanceTimer::query();
	pool<IdString> live;
	स्वतः मूळ = [&](IdString ref) {
		जर (ref != IdString::Null)
			live.घाला(ref);
	};

	walk_attr_objects(हे, [&](स्थिरांक RTLIL::AttrObject *obj) {
		साठी (स्वतः &attr : obj->attributes)
			मूळ(attr.first);
	});

	मूळ(selected_active_module);

	साठी (स्वतः &[name, module] : modules_) {
		मूळ(name);
		साठी (IdString port : module->ports)
			मूळ(port);
		साठी (IdString param : module->avail_parameters)
			मूळ(param);
		साठी (स्वतः &[param, _] : module->parameter_default_मूल्ये)
			मूळ(param);
		साठी (स्वतः &[name, _] : module->memories)
			मूळ(name);
		साठी (स्वतः &[name, _] : module->processes)
			मूळ(name);
		साठी (स्वतः &[name, wire] : module->wires_) {
			मूळ(name);
			जर (wire->known_driver())
				मूळ(wire->driverPort());
		}
		साठी (स्वतः &[name, cell] : module->cells_) {
			मूळ(name);
			मूळ(cell->type);
			साठी (स्वतः &conn : cell->connections())
				मूळ(conn.first);
			साठी (स्वतः &param : cell->parameters)
				मूळ(param.first);
		}
		साठी (स्वतः &[_, process] : module->processes) {
			std::vector<RTLIL::CaseRule*> case_चळत{&process->मूळ_case};
			पर्यंत (!case_चळत.empty()) {
				RTLIL::CaseRule *cs = case_चळत.back();
				case_चळत.काढा_back();
				साठी (स्वतः *sw : cs->switches) {
					साठी (स्वतः *case_ : sw->cases)
						case_चळत.ढकला_back(case_);
				}
			}
			साठी (स्वतः *समकालिक : process->syncs) {
				साठी (स्वतः &mwa : समकालिक->mem_write_actions)
					मूळ(mwa.memid);
			}
		}
	}

	स्वतः मूळ_selection = [&](स्थिरांक RTLIL::Selection &sel) {
		साठी (IdString m : sel.selected_modules)
			मूळ(m);
		साठी (स्वतः &[m, members] : sel.selected_members) {
			मूळ(m);
			साठी (IdString member : members)
				मूळ(member);
		}
	};

	साठी (स्वतः &[name, sel] : selection_vars) {
		मूळ(name);
		मूळ_selection(sel);
	}

	साठी (स्वतः &sel : selection_चळत)
		मूळ_selection(sel);

	माप_प्रकार erased = twines_.gc(live);

	int64_t time_ns = PerformanceTimer::query() - start;
	Pass::subtract_from_current_runtime_ns(time_ns);
	twine_gc_ns += time_ns;
	++twine_gc_मोजणी;
	परत erased;
}




pool<string> RTLIL::AttrObject::get_strpool_attribute(RTLIL::IdString id) स्थिरांक
{
	pool<string> data;
	जर (attributes.मोजणी(id) != 0)
		साठी (स्वतः s : split_tokens(get_string_attribute(id), "|"))
			data.घाला(s);
	परत data;
}

शून्य RTLIL::AttrObject::set_hdlname_attribute(स्थिरांक vector<string> &hierarchy)
{
	string attrval;
	साठी (स्थिरांक स्वतः &ident : hierarchy) {
		जर (!attrval.empty())
			attrval += " ";
		attrval += ident;
	}
	set_string_attribute(ID::hdlname, attrval);
}

vector<string> RTLIL::AttrObject::get_hdlname_attribute() स्थिरांक
{
	परत split_tokens(get_string_attribute(ID::hdlname), " ");
}

शून्य RTLIL::AttrObject::set_intvec_attribute(RTLIL::IdString id, स्थिरांक vector<पूर्णांक> &data)
{
	std::stringstream attrval;
	साठी (स्वतः &i : data) {
		जर (attrval.tellp() > 0)
			attrval << " ";
		attrval << i;
	}
	attributes[id] = RTLIL::Const(attrval.str());
}

vector<पूर्णांक> RTLIL::AttrObject::get_intvec_attribute(RTLIL::IdString id) स्थिरांक
{
	vector<पूर्णांक> data;
	स्वतः it = attributes.find(id);
	जर (it != attributes.end())
		साठी (स्थिरांक स्वतः &s : split_tokens(attributes.at(id).decode_string())) {
			अक्षर *end = nullptr;
			त्रुटि_सं = 0;
			दीर्घ मूल्य = म_से_दीर्घ(s.c_str(), &end, 10);
			जर (end != s.c_str() + s.size())
				log_cmd_error("Literal for intvec attribute has invalid format\n");
			जर (त्रुटि_सं == दुस्फल || मूल्य < पूर्णांक_न्यून || मूल्य > पूर्णांक_उच्च)
				log_cmd_error("Literal for intvec attribute is out of range\n");
			data.ढकला_back(मूल्य);
		}
	परत data;
}

तार्किक RTLIL::Selection::boxed_module(RTLIL::IdString mod_name) स्थिरांक
{
	जर (current_design != nullptr) {
		स्वतः module = current_design->module(mod_name);
		परत module && module->get_blackbox_attribute();
	} अन्यथा {
		log_warning("Unable to check if module is boxed for null design.\n");
		परत असत्य;
	}
}

तार्किक RTLIL::Selection::selected_module(RTLIL::IdString mod_name) स्थिरांक
{
	जर (complete_selection)
		परत सत्य;
	जर (!selects_boxes && boxed_module(mod_name))
		परत असत्य;
	जर (full_selection)
		परत सत्य;
	जर (selected_modules.मोजणी(mod_name) > 0)
		परत सत्य;
	जर (selected_members.मोजणी(mod_name) > 0)
		परत सत्य;
	परत असत्य;
}

तार्किक RTLIL::Selection::selected_whole_module(RTLIL::IdString mod_name) स्थिरांक
{
	जर (complete_selection)
		परत सत्य;
	जर (!selects_boxes && boxed_module(mod_name))
		परत असत्य;
	जर (full_selection)
		परत सत्य;
	जर (selected_modules.मोजणी(mod_name) > 0)
		परत सत्य;
	परत असत्य;
}

तार्किक RTLIL::Selection::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) स्थिरांक
{
	जर (complete_selection)
		परत सत्य;
	जर (!selects_boxes && boxed_module(mod_name))
		परत असत्य;
	जर (full_selection)
		परत सत्य;
	जर (selected_modules.मोजणी(mod_name) > 0)
		परत सत्य;
	जर (selected_members.मोजणी(mod_name) > 0)
		जर (selected_members.at(mod_name).मोजणी(memb_name) > 0)
			परत सत्य;
	परत असत्य;
}

शून्य RTLIL::Selection::optimize(RTLIL::Design *design)
{
	जर (design != current_design) {
		current_design = design;
	}

	जर (selects_boxes && full_selection)
		complete_selection = सत्य;
	जर (complete_selection) {
		full_selection = असत्य;
		selects_boxes = सत्य;
	}
	जर (selects_all()) {
		selected_modules.clear();
		selected_members.clear();
		परत;
	}

	std::vector<RTLIL::IdString> del_list, add_list;

	del_list.clear();
	साठी (स्वतः mod_name : selected_modules) {
		जर (current_design->module(mod_name) == nullptr || (!selects_boxes && boxed_module(mod_name)))
			del_list.ढकला_back(mod_name);
		selected_members.erase(mod_name);
	}
	साठी (स्वतः mod_name : del_list)
		selected_modules.erase(mod_name);

	del_list.clear();
	साठी (स्वतः &it : selected_members)
		जर (current_design->module(it.first) == nullptr || (!selects_boxes && boxed_module(it.first)))
			del_list.ढकला_back(it.first);
	साठी (स्वतः mod_name : del_list)
		selected_members.erase(mod_name);

	साठी (स्वतः &it : selected_members) {
		del_list.clear();
		साठी (स्वतः memb_name : it.second)
			जर (current_design->module(it.first)->मोजणी_id(memb_name) == 0)
				del_list.ढकला_back(memb_name);
		साठी (स्वतः memb_name : del_list)
			it.second.erase(memb_name);
	}

	del_list.clear();
	add_list.clear();
	साठी (स्वतः &it : selected_members)
		जर (it.second.size() == 0)
			del_list.ढकला_back(it.first);
		अन्यथा जर (it.second.size() == current_design->module(it.first)->wires_.size() + current_design->module(it.first)->memories.size() +
				current_design->module(it.first)->cells_.size() + current_design->module(it.first)->processes.size())
			add_list.ढकला_back(it.first);
	साठी (स्वतः mod_name : del_list)
		selected_members.erase(mod_name);
	साठी (स्वतः mod_name : add_list) {
		selected_members.erase(mod_name);
		selected_modules.घाला(mod_name);
	}

	जर (selected_modules.size() == current_design->modules_.size()) {
		selected_modules.clear();
		selected_members.clear();
		जर (selects_boxes)
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
  : verilog_defines (नवीन define_map_t)
{
	स्थिर अचिन्हित पूर्णांक hashidx_मोजणी = 123456789;
	hashidx_मोजणी = mkhash_xorshift(hashidx_मोजणी);
	hashidx_ = hashidx_मोजणी;

	refcount_modules_ = 0;
	selected_active_module.clear();
	ढकला_full_selection();

	RTLIL::Design::get_all_designs()->घाला(std::pair<अचिन्हित पूर्णांक, RTLIL::Design*>(hashidx_, हे));
}

RTLIL::Design::~Design()
{
	साठी (स्वतः &pr : modules_)
		delete pr.second;
	RTLIL::Design::get_all_designs()->erase(hashidx_);
}

स्थिर std::map<अचिन्हित पूर्णांक, RTLIL::Design*> all_designs;
std::map<अचिन्हित पूर्णांक, RTLIL::Design*> *RTLIL::Design::get_all_designs(शून्य)
{
	परत &all_designs;
}

RTLIL::ObjRange<RTLIL::Module*, IdString> RTLIL::Design::modules()
{
	परत RTLIL::ObjRange<RTLIL::Module*, IdString>(&modules_, &refcount_modules_);
}

स्थिरांक RTLIL::Module *RTLIL::Design::module(IdString id) स्थिरांक {
	twines_.check_owned(id);
	परत modules_.मोजणी(id) ? modules_.at(id) : रिक्त;
}
RTLIL::Module *RTLIL::Design::module(IdString id) {
	twines_.check_owned(id);
	परत modules_.मोजणी(id) ? modules_.at(id) : रिक्त;
}

RTLIL::Module *RTLIL::Design::शीर्ष_module() स्थिरांक
{
	RTLIL::Module *module = nullptr;
	पूर्णांक module_मोजणी = 0;

	साठी (स्वतः mod : selected_modules()) {
		जर (mod->get_bool_attribute(ID::शीर्ष))
			परत mod;
		module_मोजणी++;
		module = mod;
	}

	परत module_मोजणी == 1 ? module : nullptr;
}

RTLIL::Module *RTLIL::Design::addModule(RTLIL::IdString name)
{
	जर (modules_.मोजणी(name) != 0)
		log_error("Attempted to add new module named '%s', but a module by that name already exists\n", twines_.str(name));
	log_assert(refcount_modules_ == 0);

	RTLIL::Module *module = नवीन RTLIL::Module;
	modules_[name] = module;
	module->design = हे;
	module->name = name;

	साठी (स्वतः mon : monitors)
		mon->notify_module_add(module);

	जर (yosys_xtrace) {
		log("#X# New Module: %s\n", twines_.str(name));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	परत module;
}

शून्य RTLIL::Design::scratchpad_unset(स्थिरांक std::string &varname)
{
	scratchpad.erase(varname);
}

शून्य RTLIL::Design::scratchpad_set_int(स्थिरांक std::string &varname, पूर्णांक मूल्य)
{
	scratchpad[varname] = stringf("%d", मूल्य);
}

शून्य RTLIL::Design::scratchpad_set_bool(स्थिरांक std::string &varname, तार्किक मूल्य)
{
	scratchpad[varname] = मूल्य ? "true" : "false";
}

शून्य RTLIL::Design::scratchpad_set_string(स्थिरांक std::string &varname, std::string मूल्य)
{
	scratchpad[varname] = std::move(मूल्य);
}

पूर्णांक RTLIL::Design::scratchpad_get_int(स्थिरांक std::string &varname, पूर्णांक default_मूल्य_2) स्थिरांक
{
	स्वतः it = scratchpad.find(varname);
	जर (it == scratchpad.end())
		परत default_मूल्य_2;

	स्थिरांक std::string &str = it->second;

	जर (str == "0" || str == "false")
		परत 0;

	जर (str == "1" || str == "true")
		परत 1;

	अक्षर *endptr = nullptr;
	दीर्घ पूर्णांक parsed_मूल्य = म_से_दीर्घ(str.c_str(), &endptr, 10);
	परत *endptr ? default_मूल्य_2 : parsed_मूल्य;
}

तार्किक RTLIL::Design::scratchpad_get_bool(स्थिरांक std::string &varname, तार्किक default_मूल्य_2) स्थिरांक
{
	स्वतः it = scratchpad.find(varname);
	जर (it == scratchpad.end())
		परत default_मूल्य_2;

	स्थिरांक std::string &str = it->second;

	जर (str == "0" || str == "false")
		परत असत्य;

	जर (str == "1" || str == "true")
		परत सत्य;

	परत default_मूल्य_2;
}

std::string RTLIL::Design::scratchpad_get_string(स्थिरांक std::string &varname, स्थिरांक std::string &default_मूल्य_2) स्थिरांक
{
	स्वतः it = scratchpad.find(varname);
	जर (it == scratchpad.end())
		परत default_मूल्य_2;

	परत it->second;
}

शून्य RTLIL::Design::हटाओ(RTLIL::Module *module)
{
	साठी (स्वतः mon : monitors)
		mon->notify_module_del(module);

	जर (yosys_xtrace) {
		log("#X# Remove Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(modules_.at(module->name) == module);
	log_assert(refcount_modules_ == 0);
	modules_.erase(module->name);
	delete module;
}

शून्य RTLIL::Design::नाम(RTLIL::Module *module, RTLIL::IdString new_name)
{
	modules_.erase(module->name);
	module->name = new_name;
	add(module);
}

शून्य RTLIL::Design::sort()
{
	scratchpad.sort();
	modules_.sort(sort_by_id_str(twines_));
	साठी (स्वतः &it : modules_)
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
#ifndef न_संशोधन
	log_assert(!selection_चळत.empty());
	पूर्णांक pool_size = 0;
	साठी (स्वतः &it : modules_)
		pool_size = std::max(pool_size, ThreadPool::work_pool_size(0, it.second->cells_size(), 1000));
	ParallelDispatchThreadPool thread_pool(pool_size);
	साठी (स्वतः &it : modules_) {
		log_assert(हे == it.second->design);
		log_assert(it.first == it.second->name);
		check_module(it.second, thread_pool);
	}
#endif
}

शून्य RTLIL::Design::optimize()
{
	साठी (स्वतः &it : modules_)
		it.second->optimize();
	साठी (स्वतः &it : selection_चळत)
		it.optimize(हे);
	साठी (स्वतः &it : selection_vars)
		it.second.optimize(हे);
}

शून्य RTLIL::Design::clone_into(RTLIL::Design *dst) स्थिरांक
{
	log_assert(dst->modules_.empty());
	dst->twines_ = twines_;
	साठी (स्वतः it = modules_.rbegin(); it != modules_.rend(); ++it)
		it->second->clone(dst);
}

तार्किक RTLIL::Design::selected_module(IdString mod_name) स्थिरांक
{
	जर (!selected_active_module.empty() && mod_name != selected_active_module)
		परत असत्य;
	परत selection().selected_module(mod_name);
}

तार्किक RTLIL::Design::selected_whole_module(RTLIL::IdString mod_name) स्थिरांक
{
	जर (!selected_active_module.empty() && mod_name != selected_active_module)
		परत असत्य;
	परत selection().selected_whole_module(mod_name);
}

तार्किक RTLIL::Design::selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) स्थिरांक
{
	जर (!selected_active_module.empty() && mod_name != selected_active_module)
		परत असत्य;
	परत selection().selected_member(mod_name, memb_name);
}

तार्किक RTLIL::Design::selected_module(RTLIL::Module *mod) स्थिरांक
{
	परत selected_module(mod->name);
}

तार्किक RTLIL::Design::selected_whole_module(RTLIL::Module *mod) स्थिरांक
{
	परत selected_whole_module(mod->name);
}

शून्य RTLIL::Design::ढकला_selection(RTLIL::Selection sel)
{
	sel.current_design = हे;
	selection_चळत.ढकला_back(sel);
}

शून्य RTLIL::Design::ढकला_empty_selection()
{
	ढकला_selection(RTLIL::Selection::EmptySelection(हे));
}

शून्य RTLIL::Design::ढकला_full_selection()
{
	ढकला_selection(RTLIL::Selection::FullSelection(हे));
}

शून्य RTLIL::Design::ढकला_complete_selection()
{
	ढकला_selection(RTLIL::Selection::CompleteSelection(हे));
}

शून्य RTLIL::Design::काढा_selection()
{
	selection_चळत.काढा_back();
	// Default to a full_selection if we ran out of stack
	जर (selection_चळत.empty())
		ढकला_full_selection();
}

std::string RTLIL::Design::to_rtlil_str(तार्किक only_selected) स्थिरांक
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_design(f, const_cast<RTLIL::Design*>(हे), only_selected, सत्य, असत्य, RTLIL_BACKEND::DumpMode::Readable);
	परत f.str();
}

std::vector<RTLIL::Module*> RTLIL::Design::selected_modules(RTLIL::SelectPartials partials, RTLIL::SelectBoxes boxes) स्थिरांक
{
	तार्किक include_partials = partials == RTLIL::SELECT_ALL;
	तार्किक exclude_boxes = (boxes & RTLIL::SB_UNBOXED_ONLY) != 0;
	तार्किक ignore_wb = (boxes & RTLIL::SB_INCL_WB) != 0;
	std::vector<RTLIL::Module*> result;
	result.reserve(modules_.size());
	साठी (स्वतः &it : modules_) {
		जर (selected_whole_module(it.first) || (include_partials && selected_module(it.first))) {
			जर (!(exclude_boxes && it.second->get_blackbox_attribute(ignore_wb)))
				result.ढकला_back(it.second);
			अन्यथा
				निवड (boxes)
				{
				स्थिती RTLIL::SB_UNBOXED_WARN:
					log_warning("Ignoring boxed module %s.\n", it.second);
					थांबा;
				स्थिती RTLIL::SB_EXCL_BB_WARN:
					log_warning("Ignoring blackbox module %s.\n", it.second);
					थांबा;
				स्थिती RTLIL::SB_UNBOXED_ERR:
					log_error("Unsupported boxed module %s.\n", it.second);
					थांबा;
				स्थिती RTLIL::SB_EXCL_BB_ERR:
					log_error("Unsupported blackbox module %s.\n", it.second);
					थांबा;
				स्थिती RTLIL::SB_UNBOXED_CMDERR:
					log_cmd_error("Unsupported boxed module %s.\n", it.second);
					थांबा;
				स्थिती RTLIL::SB_EXCL_BB_CMDERR:
					log_cmd_error("Unsupported blackbox module %s.\n", it.second);
					थांबा;
				पूर्वनिर्धारित:
					थांबा;
				}
		} अन्यथा जर (!include_partials && selected_module(it.first)) {
			निवड(partials)
			{
			स्थिती RTLIL::SELECT_WHOLE_WARN:
				log_warning("Ignoring partially selected module %s.\n", it.second);
				थांबा;
			स्थिती RTLIL::SELECT_WHOLE_ERR:
				log_error("Unsupported partially selected module %s.\n", it.second);
				थांबा;
			स्थिती RTLIL::SELECT_WHOLE_CMDERR:
				log_cmd_error("Unsupported partially selected module %s.\n", it.second);
				थांबा;
			पूर्वनिर्धारित:
				थांबा;
			}
		}
	}
	परत result;
}

RTLIL::Module::Module()
{
	स्थिर अचिन्हित पूर्णांक hashidx_मोजणी = 123456789;
	hashidx_मोजणी = mkhash_xorshift(hashidx_मोजणी);
	hashidx_ = hashidx_मोजणी;

	design = nullptr;
	refcount_wires_ = 0;
	refcount_cells_ = 0;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->घाला(std::pair<अचिन्हित पूर्णांक, RTLIL::Module*>(hashidx_, हे));
#endif
}

RTLIL::Module::~Module()
{
	साठी (स्वतः &pr : wires_)
		delete pr.second;
	साठी (स्वतः &pr : memories)
		delete pr.second;
	साठी (स्वतः &pr : cells_)
		delete pr.second;
	साठी (स्वतः &pr : processes)
		delete pr.second;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Module::get_all_modules()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
स्थिर std::map<अचिन्हित पूर्णांक, RTLIL::Module*> all_modules;
std::map<अचिन्हित पूर्णांक, RTLIL::Module*> *RTLIL::Module::get_all_modules(शून्य)
{
	परत &all_modules;
}
#endif

शून्य RTLIL::Module::makeblackbox()
{
	pool<RTLIL::Wire*> delwires;

	साठी (स्वतः it = wires_.begin(); it != wires_.end(); ++it)
		जर (!it->second->port_आदान && !it->second->port_output)
			delwires.घाला(it->second);

	साठी (स्वतः it = memories.begin(); it != memories.end(); ++it)
		delete it->second;
	memories.clear();

	साठी (स्वतः it = cells_.begin(); it != cells_.end(); ++it)
		delete it->second;
	cells_.clear();

	साठी (स्वतः it = processes.begin(); it != processes.end(); ++it)
		delete it->second;
	processes.clear();

	connections_.clear();

	हटाओ(delwires);
	set_bool_attribute(ID::blackbox);
}

शून्य RTLIL::Module::expand_interfaces(RTLIL::Design *, स्थिरांक dict<RTLIL::IdString, RTLIL::Module *> &)
{
	log_error("Class doesn't support expand_interfaces (module: `%s')!\n", name.unescape());
}

तार्किक RTLIL::Module::reprocess_if_necessary(RTLIL::Design *)
{
	परत असत्य;
}

RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, स्थिरांक dict<RTLIL::IdString, RTLIL::Const> &, तार्किक mayfail)
{
	जर (mayfail)
		परत IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}


RTLIL::IdString RTLIL::Module::derive(RTLIL::Design*, स्थिरांक dict<RTLIL::IdString, RTLIL::Const> &, स्थिरांक dict<RTLIL::IdString, RTLIL::Module*> &, स्थिरांक dict<RTLIL::IdString, RTLIL::IdString> &, तार्किक mayfail)
{
	जर (mayfail)
		परत IdString::Null;
	log_error("Module `%s' is used with parameters but is not parametric!\n", name.unescape());
}

माप_प्रकार RTLIL::Module::मोजणी_id(RTLIL::IdString id)
{
	परत wires_.मोजणी(id) + cells_.मोजणी(id) + memories.मोजणी(id) + processes.मोजणी(id);
}

#ifndef न_संशोधन
नामाकाश {
	संरचना InternalCellChecker
	{
		स्थिरांक RTLIL::Module *module;
		RTLIL::Cell *cell;
		pool<IdString> expected_params;
		pool<IdString> expected_ports;

		InternalCellChecker(स्थिरांक RTLIL::Module *module, RTLIL::Cell *cell) : module(module), cell(cell) { }

		शून्य error(पूर्णांक linenr, std::optional<std::string> error_संदेश = std::nullopt)
		{
			std::stringstream buf;
			RTLIL_BACKEND::dump_cell(buf, "  ", cell, cell->module->design);

			std::string mod_name = module ? module->name.str() : std::string();
			std::string cell_name = cell->name.unescape();
			std::string cell_loc = stringf("%s%s%s (%s) at %s:%d:\n%s", mod_name, module ? "." : "",
							   cell_name, cell->type.str(), __FILE__, linenr, buf.str());

			जर (error_संदेश) {
				log_error("%s in internal cell %s\n", *error_संदेश, cell_loc);
			} अन्यथा {
				log_error("Found error in internal cell %s\n", cell_loc);
			}
		}

		पूर्णांक param(RTLIL::IdString name)
		{
			स्वतः it = cell->parameters.find(name);
			जर (it == cell->parameters.end()) {
				std::string err = stringf("Expected to find parameter %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			expected_params.घाला(name);
			परत it->second.as_int();
		}

		पूर्णांक param_bool(RTLIL::IdString name)
		{
			पूर्णांक v = param(name);
			जर (GetSize(cell->parameters.at(name)) > 32)
				error(__LINE__);
			जर (v != 0 && v != 1)
				error(__LINE__);
			परत v;
		}

		पूर्णांक param_bool(RTLIL::IdString name, तार्किक expected)
		{
			पूर्णांक v = param_bool(name);
			जर (v != expected)
				error(__LINE__);
			परत v;
		}

		शून्य param_bits(RTLIL::IdString name, पूर्णांक width)
		{
			param(name);
			जर (GetSize(cell->parameters.at(name)) != width)
				error(__LINE__);
		}

		std::string param_string(RTLIL::IdString name)
		{
			param(name);
			परत cell->parameters.at(name).decode_string();
		}

		शून्य port(RTLIL::IdString name, पूर्णांक width)
		{
			स्वतः it = cell->connections_.find(name);
			जर (it == cell->connections_.end()) {
				std::string err = stringf("Expected to find port %s", cell->module->design->twines().str(name));
				error(__LINE__, err);
			}
			जर (GetSize(it->second) != width) {
				std::string err = stringf("Expected port to have width %d", width);
				error(__LINE__, err);
			}
			expected_ports.घाला(name);
		}

		शून्य check_expected(तार्किक check_matched_sign = असत्य)
		{
			साठी (स्वतः &para : cell->parameters)
				जर (expected_params.मोजणी(para.first) == 0)
					error(__LINE__);
			साठी (स्वतः &conn : cell->connections())
				जर (expected_ports.मोजणी(conn.first) == 0)
					error(__LINE__);

			जर (check_matched_sign) {
				log_assert(expected_params.मोजणी(ID::A_SIGNED) != 0 && expected_params.मोजणी(ID::B_SIGNED) != 0);
				तार्किक a_is_signed = cell->parameters.at(ID::A_SIGNED).as_bool();
				तार्किक b_is_signed = cell->parameters.at(ID::B_SIGNED).as_bool();
				जर (a_is_signed != b_is_signed)
					error(__LINE__);
			}
		}

		शून्य check()
		{
			जर (cell->type.isPublic() || cell->type.begins_with("$__") || cell->type.begins_with("$paramod") || cell->type.begins_with("$fmcombine") ||
					cell->type.begins_with("$verific$") || cell->type.begins_with("$array:") || cell->type.begins_with("$extern:"))
				परत;

			जर (cell->type.in(ID($buf), ID($barrier))) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type.in(ID($not), ID($pos), ID($neg))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(सत्य);
				परत;
			}

			जर (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool))) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED, /*expected=*/असत्य);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/असत्य);
				परत;
			}

			जर (cell->type.in(ID($shift), ID($shiftx))) {
				जर (cell->type == ID($shiftx)) {
					param_bool(ID::A_SIGNED, /*expected=*/असत्य);
				} अन्यथा {
					param_bool(ID::A_SIGNED);
				}
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/असत्य);
				परत;
			}

			जर (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(सत्य);
				परत;
			}

			जर (cell->type.in(ID($add), ID($sub), ID($mul), ID($भाग), ID($mod), ID($divfloor), ID($modfloor), ID($घात))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(cell->type != ID($घात));
				परत;
			}

			जर (cell->type == ID($fa)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::C, param(ID::WIDTH));
				port(ID::X, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($lcu)) {
				port(ID::P, param(ID::WIDTH));
				port(ID::G, param(ID::WIDTH));
				port(ID::CI, 1);
				port(ID::CO, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($alu)) {
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
				परत;
			}

			जर (cell->type == ID($macc)) {
				param(ID::CONFIG);
				param(ID::CONFIG_WIDTH);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				Macc().from_cell(cell);
				परत;
			}

			जर (cell->type == ID($macc_v2)) {
				जर (param(ID::NPRODUCTS) < 0)
					error(__LINE__);
				जर (param(ID::NADDENDS) < 0)
					error(__LINE__);
				param_bits(ID::PRODUCT_NEGATED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::ADDEND_NEGATED, max(param(ID::NADDENDS), 1));
				param_bits(ID::A_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::B_SIGNED, max(param(ID::NPRODUCTS), 1));
				param_bits(ID::C_SIGNED, max(param(ID::NADDENDS), 1));
				जर (cell->getParam(ID::A_SIGNED) != cell->getParam(ID::B_SIGNED))
					error(__LINE__);
				param_bits(ID::A_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::B_WIDTHS, max(param(ID::NPRODUCTS) * 16, 1));
				param_bits(ID::C_WIDTHS, max(param(ID::NADDENDS) * 16, 1));
				स्थिरांक Const &a_width = cell->getParam(ID::A_WIDTHS);
				स्थिरांक Const &b_width = cell->getParam(ID::B_WIDTHS);
				स्थिरांक Const &c_width = cell->getParam(ID::C_WIDTHS);
				पूर्णांक a_width_sum = 0, b_width_sum = 0, c_width_sum = 0;
				साठी (पूर्णांक i = 0; i < param(ID::NPRODUCTS); i++) {
					a_width_sum += a_width.extract(16 * i, 16).as_int(असत्य);
					b_width_sum += b_width.extract(16 * i, 16).as_int(असत्य);
				}
				साठी (पूर्णांक i = 0; i < param(ID::NADDENDS); i++) {
					c_width_sum += c_width.extract(16 * i, 16).as_int(असत्य);
				}
				port(ID::A, a_width_sum);
				port(ID::B, b_width_sum);
				port(ID::C, c_width_sum);
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($logic_not)) {
				param_bool(ID::A_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type.in(ID($logic_and), ID($logic_or))) {
				param_bool(ID::A_SIGNED);
				param_bool(ID::B_SIGNED);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				check_expected(/*check_matched_sign=*/असत्य);
				परत;
			}

			जर (cell->type == ID($slice)) {
				param(ID::OFFSET);
				port(ID::A, param(ID::A_WIDTH));
				port(ID::Y, param(ID::Y_WIDTH));
				जर (param(ID::OFFSET) + param(ID::Y_WIDTH) > param(ID::A_WIDTH))
					error(__LINE__);
				check_expected();
				परत;
			}

			जर (cell->type == ID($concat)) {
				port(ID::A, param(ID::A_WIDTH));
				port(ID::B, param(ID::B_WIDTH));
				port(ID::Y, param(ID::A_WIDTH) + param(ID::B_WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($mux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, 1);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($pmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH) * param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($bmux)) {
				port(ID::A, param(ID::WIDTH) << param(ID::S_WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($demux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::S, param(ID::S_WIDTH));
				port(ID::Y, param(ID::WIDTH) << param(ID::S_WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($lut)) {
				param(ID::LUT);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				परत;
			}

			जर (cell->type == ID($sop)) {
				param(ID::DEPTH);
				param(ID::TABLE);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, 1);
				check_expected();
				परत;
			}

			जर (cell->type == ID($sr)) {
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Q,   param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($ff)) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($dff)) {
				param_bool(ID::CLK_POLARITY);
				port(ID::CLK, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($dffe)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::EN_POLARITY);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($dffsr)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::CLK, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($dffsre)) {
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
				परत;
			}

			जर (cell->type == ID($adff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_मूल्य, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($sdff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::SRST_POLARITY);
				param_bits(ID::SRST_मूल्य, param(ID::WIDTH));
				port(ID::CLK, 1);
				port(ID::SRST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type.in(ID($sdffe), ID($sdffce))) {
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
				परत;
			}

			जर (cell->type == ID($adffe)) {
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
				परत;
			}

			जर (cell->type == ID($aldff)) {
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::ALOAD_POLARITY);
				port(ID::CLK, 1);
				port(ID::ALOAD, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::AD, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($aldffe)) {
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
				परत;
			}

			जर (cell->type == ID($dlatch)) {
				param_bool(ID::EN_POLARITY);
				port(ID::EN, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($adlatch)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::ARST_POLARITY);
				param_bits(ID::ARST_मूल्य, param(ID::WIDTH));
				port(ID::EN, 1);
				port(ID::ARST, 1);
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($dlatchsr)) {
				param_bool(ID::EN_POLARITY);
				param_bool(ID::SET_POLARITY);
				param_bool(ID::CLR_POLARITY);
				port(ID::EN, 1);
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($fsm)) {
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
				परत;
			}

			जर (cell->type == ID($memrd)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param_bool(ID::TRANSPARENT);
				port(ID::CLK, 1);
				port(ID::EN, 1);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($memrd_v2)) {
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
				परत;
			}

			जर (cell->type == ID($memwr)) {
				param(ID::MEMID);
				param_bool(ID::CLK_ENABLE);
				param_bool(ID::CLK_POLARITY);
				param(ID::PRIORITY);
				port(ID::CLK, 1);
				port(ID::EN, param(ID::WIDTH));
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($memwr_v2)) {
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
				परत;
			}

			जर (cell->type == ID($meminit)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::शब्द));
				check_expected();
				परत;
			}

			जर (cell->type == ID($meminit_v2)) {
				param(ID::MEMID);
				param(ID::PRIORITY);
				port(ID::ADDR, param(ID::ABITS));
				port(ID::DATA, param(ID::WIDTH) * param(ID::शब्द));
				port(ID::EN, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($mem)) {
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
				परत;
			}

			जर (cell->type == ID($mem_v2)) {
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
				परत;
			}

			जर (cell->type == ID($tribuf)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				port(ID::EN, 1);
				check_expected();
				परत;
			}

			जर (cell->type == ID($bweqx)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($bwmux)) {
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				port(ID::S, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type.in(ID($प्रतिज्ञा), ID($assume), ID($live), ID($fair), ID($cover))) {
				port(ID::A, 1);
				port(ID::EN, 1);
				check_expected();
				परत;
			}

			जर (cell->type == ID($initstate)) {
				port(ID::Y, 1);
				check_expected();
				परत;
			}

			जर (cell->type.in(ID($anyconst), ID($anyseq), ID($allconst), ID($allseq))) {
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type.in(ID($anyinit))) {
				port(ID::D, param(ID::WIDTH));
				port(ID::Q, param(ID::WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($equiv)) {
				port(ID::A, 1);
				port(ID::B, 1);
				port(ID::Y, 1);
				check_expected();
				परत;
			}

			जर (cell->type.in(ID($specify2), ID($specify3))) {
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
				जर (cell->type == ID($specify3)) {
					param_bool(ID::EDGE_EN);
					param_bool(ID::EDGE_POL);
					param_bool(ID::DAT_DST_PEN);
					param_bool(ID::DAT_DST_POL);
					port(ID::DAT, param(ID::DST_WIDTH));
				}
				check_expected();
				परत;
			}

			जर (cell->type == ID($specrule)) {
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
				परत;
			}

			जर (cell->type == ID($print)) {
				param(ID(FORMAT));
				param_bool(ID::TRG_ENABLE);
				param(ID::TRG_POLARITY);
				param(ID::PRIORITY);
				port(ID::EN, 1);
				port(ID::TRG, param(ID::TRG_WIDTH));
				port(ID::ARGS, param(ID::ARGS_WIDTH));
				check_expected();
				परत;
			}

			जर (cell->type == ID($check)) {
				std::string flavor = param_string(ID(FLAVOR));
				जर (!(flavor == "assert" || flavor == "assume" || flavor == "live" || flavor == "fair" || flavor == "cover"))
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
				परत;
			}

			जर (cell->type == ID($scopeinfo)) {
				param(ID::TYPE);
				check_expected();
				std::string scope_type = cell->getParam(ID::TYPE).decode_string();
				जर (scope_type != "module" && scope_type != "struct" && scope_type != "blackbox")
					error(__LINE__);
				परत;
			}

			जर (cell->type == ID($_BUF_))    { port(ID::A,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_NOT_))    { port(ID::A,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_AND_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_NAND_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_OR_))     { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_NOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_XOR_))    { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_XNOR_))   { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_ANDNOT_)) { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_ORNOT_))  { port(ID::A,1); port(ID::B,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_MUX_))    { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_NMUX_))   { port(ID::A,1); port(ID::B,1); port(ID::S,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_AOI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_OAI3_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_AOI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_OAI4_))   { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::Y,1); check_expected(); परत; }

			जर (cell->type == ID($_TBUF_))  { port(ID::A,1); port(ID::Y,1); port(ID::E,1); check_expected(); परत; }

			जर (cell->type == ID($_MUX4_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::S,1); port(ID::T,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_MUX8_))  { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::Y,1); check_expected(); परत; }
			जर (cell->type == ID($_MUX16_)) { port(ID::A,1); port(ID::B,1); port(ID::C,1); port(ID::D,1); port(ID::E,1); port(ID::F,1); port(ID::G,1); port(ID::H,1); port(ID::I,1); port(ID::J,1); port(ID::K,1); port(ID::L,1); port(ID::M,1); port(ID::N,1); port(ID::O,1); port(ID::P,1); port(ID::S,1); port(ID::T,1); port(ID::U,1); port(ID::V,1); port(ID::Y,1); check_expected(); परत; }

			जर (cell->type.in(ID($_SR_NN_), ID($_SR_NP_), ID($_SR_PN_), ID($_SR_PP_)))
				{ port(ID::S,1); port(ID::R,1); port(ID::Q,1); check_expected(); परत; }

			जर (cell->type == ID($_FF_)) { port(ID::D,1); port(ID::Q,1); check_expected();  परत; }

			जर (cell->type.in(ID($_DFF_N_), ID($_DFF_P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); check_expected(); परत; }

			जर (cell->type.in(ID($_DFFE_NN_), ID($_DFFE_NP_), ID($_DFFE_PN_), ID($_DFFE_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::E,1); check_expected(); परत; }

			जर (cell->type.in(
					ID($_DFF_NN0_), ID($_DFF_NN1_), ID($_DFF_NP0_), ID($_DFF_NP1_),
					ID($_DFF_PN0_), ID($_DFF_PN1_), ID($_DFF_PP0_), ID($_DFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); परत; }

			जर (cell->type.in(
					ID($_DFFE_NN0N_), ID($_DFFE_NN0P_), ID($_DFFE_NN1N_), ID($_DFFE_NN1P_),
					ID($_DFFE_NP0N_), ID($_DFFE_NP0P_), ID($_DFFE_NP1N_), ID($_DFFE_NP1P_),
					ID($_DFFE_PN0N_), ID($_DFFE_PN0P_), ID($_DFFE_PN1N_), ID($_DFFE_PN1P_),
					ID($_DFFE_PP0N_), ID($_DFFE_PP0P_), ID($_DFFE_PP1N_), ID($_DFFE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); परत; }

			जर (cell->type.in(
					ID($_ALDFF_NN_), ID($_ALDFF_NP_), ID($_ALDFF_PN_), ID($_ALDFF_PP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); check_expected(); परत; }

			जर (cell->type.in(
					ID($_ALDFFE_NNN_), ID($_ALDFFE_NNP_), ID($_ALDFFE_NPN_), ID($_ALDFFE_NPP_),
					ID($_ALDFFE_PNN_), ID($_ALDFFE_PNP_), ID($_ALDFFE_PPN_), ID($_ALDFFE_PPP_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::L,1); port(ID::AD,1); port(ID::E,1); check_expected(); परत; }

			जर (cell->type.in(
					ID($_DFFSR_NNN_), ID($_DFFSR_NNP_), ID($_DFFSR_NPN_), ID($_DFFSR_NPP_),
					ID($_DFFSR_PNN_), ID($_DFFSR_PNP_), ID($_DFFSR_PPN_), ID($_DFFSR_PPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); परत; }

			जर (cell->type.in(
					ID($_DFFSRE_NNNN_), ID($_DFFSRE_NNNP_), ID($_DFFSRE_NNPN_), ID($_DFFSRE_NNPP_),
					ID($_DFFSRE_NPNN_), ID($_DFFSRE_NPNP_), ID($_DFFSRE_NPPN_), ID($_DFFSRE_NPPP_),
					ID($_DFFSRE_PNNN_), ID($_DFFSRE_PNNP_), ID($_DFFSRE_PNPN_), ID($_DFFSRE_PNPP_),
					ID($_DFFSRE_PPNN_), ID($_DFFSRE_PPNP_), ID($_DFFSRE_PPPN_), ID($_DFFSRE_PPPP_)))
				{ port(ID::C,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::E,1); port(ID::Q,1); check_expected(); परत; }

			जर (cell->type.in(
					ID($_SDFF_NN0_), ID($_SDFF_NN1_), ID($_SDFF_NP0_), ID($_SDFF_NP1_),
					ID($_SDFF_PN0_), ID($_SDFF_PN1_), ID($_SDFF_PP0_), ID($_SDFF_PP1_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); check_expected(); परत; }

			जर (cell->type.in(
					ID($_SDFFE_NN0N_), ID($_SDFFE_NN0P_), ID($_SDFFE_NN1N_), ID($_SDFFE_NN1P_),
					ID($_SDFFE_NP0N_), ID($_SDFFE_NP0P_), ID($_SDFFE_NP1N_), ID($_SDFFE_NP1P_),
					ID($_SDFFE_PN0N_), ID($_SDFFE_PN0P_), ID($_SDFFE_PN1N_), ID($_SDFFE_PN1P_),
					ID($_SDFFE_PP0N_), ID($_SDFFE_PP0P_), ID($_SDFFE_PP1N_), ID($_SDFFE_PP1P_),
					ID($_SDFFCE_NN0N_), ID($_SDFFCE_NN0P_), ID($_SDFFCE_NN1N_), ID($_SDFFCE_NN1P_),
					ID($_SDFFCE_NP0N_), ID($_SDFFCE_NP0P_), ID($_SDFFCE_NP1N_), ID($_SDFFCE_NP1P_),
					ID($_SDFFCE_PN0N_), ID($_SDFFCE_PN0P_), ID($_SDFFCE_PN1N_), ID($_SDFFCE_PN1P_),
					ID($_SDFFCE_PP0N_), ID($_SDFFCE_PP0P_), ID($_SDFFCE_PP1N_), ID($_SDFFCE_PP1P_)))
				{ port(ID::D,1); port(ID::Q,1); port(ID::C,1); port(ID::R,1); port(ID::E,1); check_expected(); परत; }

			जर (cell->type.in(ID($_DLATCH_N_), ID($_DLATCH_P_)))
				{ port(ID::E,1); port(ID::D,1); port(ID::Q,1); check_expected(); परत; }

			जर (cell->type.in(
					ID($_DLATCH_NN0_), ID($_DLATCH_NN1_), ID($_DLATCH_NP0_), ID($_DLATCH_NP1_),
					ID($_DLATCH_PN0_), ID($_DLATCH_PN1_), ID($_DLATCH_PP0_), ID($_DLATCH_PP1_)))
				{ port(ID::E,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); परत; }

			जर (cell->type.in(
					ID($_DLATCHSR_NNN_), ID($_DLATCHSR_NNP_), ID($_DLATCHSR_NPN_), ID($_DLATCHSR_NPP_),
					ID($_DLATCHSR_PNN_), ID($_DLATCHSR_PNP_), ID($_DLATCHSR_PPN_), ID($_DLATCHSR_PPP_)))
				{ port(ID::E,1); port(ID::S,1); port(ID::R,1); port(ID::D,1); port(ID::Q,1); check_expected(); परत; }

			जर (cell->type.in(ID($set_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				परत;
			}
			जर (cell->type.in(ID($get_tag),ID($original_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				परत;
			}
			जर (cell->type.in(ID($overwrite_tag))) {
				param(ID::WIDTH);
				param(ID::TAG);
				port(ID::A, param(ID::WIDTH));
				port(ID::SET, param(ID::WIDTH));
				port(ID::CLR, param(ID::WIDTH));
				check_expected();
				परत;
			}
			जर (cell->type.in(ID($future_ff))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				परत;
			}
			जर (cell->type.in(ID($आदान_port))) {
				param(ID::WIDTH);
				port(ID::Y, param(ID::WIDTH));
				check_expected();
				परत;
			}
			जर (cell->type.in(ID($connect))) {
				param(ID::WIDTH);
				port(ID::A, param(ID::WIDTH));
				port(ID::B, param(ID::WIDTH));
				check_expected();
				परत;
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
	parameter_default_मूल्ये.sort(sort_by_id_str(design->twines()));
	memories.sort(sort_by_id_str(design->twines()));
	processes.sort(sort_by_id_str(design->twines()));
	साठी (स्वतः &it : cells_)
		it.second->sort();
	साठी (स्वतः &it : wires_)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
	साठी (स्वतः &it : memories)
		it.second->attributes.sort(sort_by_id_str(design->twines()));
}

#ifndef न_संशोधन
स्थिर शून्य check_id_किल्ल्या(स्थिरांक dict<RTLIL::IdString, RTLIL::Const> &keyed)
{
	साठी (स्वतः &it : keyed)
		log_assert(!it.first.empty());
}
#endif

शून्य check_module(RTLIL::Module *module, ParallelDispatchThreadPool &thread_pool)
{
#ifndef न_संशोधन
	ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->cells_size(), 1000));
	स्थिरांक RTLIL::Module *const_module = module;

	pool<std::string> memory_strings;
	साठी (स्वतः &it : module->memories) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->width >= 0);
		log_assert(it.second->size >= 0);
		check_id_किल्ल्या(it.second->attributes);
		memory_strings.घाला(it.second->name.str());
	}

	std::vector<MonotonicFlag> ports_declared(GetSize(module->ports));
	ShardedVector<std::string> memids(subpool);
	subpool.run([const_module, &ports_declared, &memory_strings, &memids](स्थिरांक ParallelDispatchThreadPool::RunCtx &ctx) {
		साठी (पूर्णांक i : ctx.item_range(const_module->cells_size())) {
			स्वतः it = *const_module->cells_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(!it.second->type.empty());
			साठी (स्वतः &it2 : it.second->connections()) {
				log_assert(it2.first != IdString::Null);
				it2.second.check(const_module);
			}
			check_id_किल्ल्या(it.second->attributes);
			check_id_किल्ल्या(it.second->parameters);
			InternalCellChecker checker(const_module, it.second);
			checker.check();
			जर (it.second->has_memid()) {
				log_assert(memory_strings.मोजणी(it.second->parameters.at(ID::MEMID).decode_string()));
			} अन्यथा जर (it.second->is_mem_cell()) {
				std::string memid = it.second->parameters.at(ID::MEMID).decode_string();
				log_assert(!memory_strings.मोजणी(memid));
				memids.घाला(ctx, std::move(memid));
			}
			स्वतः cell_mod = const_module->design->module(it.second->name);
			जर (cell_mod != nullptr) {
				// assertion check below to make sure that there are no
				// cases where a cell has a blackbox attribute since
				// that is deprecated
				#ifdef __GNUC__
				#pragma GCC diagnostic ढकला
				#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				#endif
				log_assert(!it.second->get_blackbox_attribute());
				#ifdef __GNUC__
				#pragma GCC diagnostic काढा
				#endif
			}
		}

		साठी (पूर्णांक i : ctx.item_range(const_module->wires_size())) {
			स्वतः it = *const_module->wires_.element(i);
			log_assert(const_module == it.second->module);
			log_assert(it.first == it.second->name);
			log_assert(it.first != IdString::Null);
			log_assert(it.second->width >= 0);
			log_assert(it.second->port_id >= 0);
			check_id_किल्ल्या(it.second->attributes);
			जर (it.second->port_id) {
				log_assert(GetSize(const_module->ports) >= it.second->port_id);
				log_assert(const_module->ports.at(it.second->port_id-1) == it.second->name);
				log_assert(it.second->port_आदान || it.second->port_output);
				log_assert(it.second->port_id <= GetSize(ports_declared));
				तार्किक previously_declared = ports_declared[it.second->port_id-1].set_and_return_old();
				log_assert(previously_declared == असत्य);
			} अन्यथा
				log_assert(!it.second->port_आदान && !it.second->port_output);
		}
	});
	साठी (स्थिरांक MonotonicFlag &port_declared : ports_declared)
		log_assert(port_declared.load() == सत्य);
	pool<std::string> memids_pool;
	साठी (std::string &memid : memids)
		log_assert(memids_pool.घाला(memid).second);

	साठी (स्वतः &it : module->processes) {
		log_assert(it.first == it.second->name);
		log_assert(it.first != IdString::Null);
		log_assert(it.second->मूळ_case.compare.empty());
		std::vector<RTLIL::CaseRule*> all_cases = {&it.second->मूळ_case};
		साठी (माप_प्रकार i = 0; i < all_cases.size(); i++) {
			साठी (स्वतः &switch_it : all_cases[i]->switches) {
				साठी (स्वतः &case_it : switch_it->cases) {
					साठी (स्वतः &compare_it : case_it->compare) {
						log_assert(switch_it->संकेत.size() == compare_it.size());
					}
					all_cases.ढकला_back(case_it);
				}
			}
		}
		साठी (स्वतः &sync_it : it.second->syncs) {
			निवड (sync_it->type) {
				स्थिती RTLIL::SyncType::ST0:
				स्थिती RTLIL::SyncType::ST1:
				स्थिती RTLIL::SyncType::STp:
				स्थिती RTLIL::SyncType::STn:
				स्थिती RTLIL::SyncType::STe:
					log_assert(!sync_it->संकेत.empty());
					थांबा;
				स्थिती RTLIL::SyncType::STa:
				स्थिती RTLIL::SyncType::STg:
				स्थिती RTLIL::SyncType::STi:
					log_assert(sync_it->संकेत.empty());
					थांबा;
			}
		}
	}

	साठी (स्वतः &it : module->connections_) {
		log_assert(it.first.size() == it.second.size());
		log_assert(!it.first.has_const());
		it.first.check(module);
		it.second.check(module);
	}

	check_id_किल्ल्या(module->attributes);
#endif
}

शून्य RTLIL::Module::check()
{
	पूर्णांक pool_size = ThreadPool::work_pool_size(0, cells_size(), 1000);
	ParallelDispatchThreadPool thread_pool(pool_size);
	check_module(हे, thread_pool);
}

शून्य RTLIL::Module::optimize()
{
}

शून्य RTLIL::Module::cloneInto(RTLIL::Module *new_mod) स्थिरांक
{
	log_assert(new_mod->refcount_wires_ == 0);
	log_assert(new_mod->refcount_cells_ == 0);

	TwinePool &dst_twines = new_mod->twines();

	new_mod->avail_parameters.clear();
	साठी (IdString param : avail_parameters)
		new_mod->avail_parameters(dst_twines.copy_from(design->twines(), param));
	new_mod->parameter_default_मूल्ये.clear();
	साठी (स्वतः &it : parameter_default_मूल्ये)
		new_mod->parameter_default_मूल्ये[dst_twines.copy_from(design->twines(), it.first)] = it.second;

	साठी (स्वतः &conn : connections())
		new_mod->connect(conn);

	RTLIL::copy_attr_dict(new_mod->attributes, attributes, design, new_mod->design);

	dict<RTLIL::Wire*, RTLIL::Wire*> wire_map;

	साठी (स्वतः it = wires_.rbegin(); it != wires_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		wire_map[it->second] = new_mod->addWire(dst_id, it->second);
	}

	साठी (स्वतः it = memories.rbegin(); it != memories.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addMemory(dst_id, it->second);
	}

	साठी (स्वतः it = cells_.rbegin(); it != cells_.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addCell(dst_id, it->second);
	}

	साठी (स्वतः it = processes.rbegin(); it != processes.rend(); ++it) {
		IdString dst_id = dst_twines.copy_from(design->twines(), it->first);
		new_mod->addProcess(dst_id, it->second);
	}

	संरचना RewriteSigSpecWorker
	{
		स्थिरांक dict<RTLIL::Wire*, RTLIL::Wire*> &wire_map;
		स्थिरांक RTLIL::Module *new_mod;
		शून्य चालक()(RTLIL::SigSpec &sig)
		{
			sig.rewrite_wires([हे](RTLIL::Wire *&wire) {
				स्वतः it = wire_map.find(wire);
				जर (it != wire_map.end())
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

RTLIL::Module *RTLIL::Module::clone() स्थिरांक
{
	RTLIL::Module *new_mod = नवीन RTLIL::Module;
	new_mod->design = design;
	new_mod->name = name;
	cloneInto(new_mod);
	परत new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst) स्थिरांक
{
	RTLIL::Module *new_mod = नवीन RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = dst->twines().copy_from(design->twines(), name);
	cloneInto(new_mod);
	dst->add(new_mod);
	परत new_mod;
}

RTLIL::Module *RTLIL::Module::clone(RTLIL::Design *dst, IdString target_name) स्थिरांक
{
	RTLIL::Module *new_mod = नवीन RTLIL::Module;
	new_mod->design = dst;
	new_mod->name = target_name;
	cloneInto(new_mod);
	dst->add(new_mod);
	परत new_mod;
}

तार्किक RTLIL::Module::has_memories() स्थिरांक
{
	परत !memories.empty();
}

तार्किक RTLIL::Module::has_processes() स्थिरांक
{
	परत !processes.empty();
}

तार्किक RTLIL::Module::has_memories_warn() स्थिरांक
{
	जर (!memories.empty())
		log_warning("Ignoring module %s because it contains memories (run 'memory' command first).\n", हे);
	परत !memories.empty();
}

तार्किक RTLIL::Module::has_processes_warn() स्थिरांक
{
	जर (!processes.empty())
		log_warning("Ignoring module %s because it contains processes (run 'proc' command first).\n", हे);
	परत !processes.empty();
}

तार्किक RTLIL::Module::is_selected() स्थिरांक
{
	परत design->selected_module(हे->name);
}

तार्किक RTLIL::Module::is_selected_whole() स्थिरांक
{
	परत design->selected_whole_module(हे->name);
}

std::vector<RTLIL::Wire*> RTLIL::Module::selected_wires() स्थिरांक
{
	std::vector<RTLIL::Wire*> result;
	result.reserve(wires_.size());
	साठी (स्वतः &it : wires_)
		जर (design->selected(हे, it.second))
			result.ढकला_back(it.second);
	परत result;
}

std::vector<RTLIL::Cell*> RTLIL::Module::selected_cells() स्थिरांक
{
	std::vector<RTLIL::Cell*> result;
	result.reserve(cells_.size());
	साठी (स्वतः &it : cells_)
		जर (design->selected(हे, it.second))
			result.ढकला_back(it.second);
	परत result;
}

std::vector<RTLIL::Memory*> RTLIL::Module::selected_memories() स्थिरांक
{
	std::vector<RTLIL::Memory*> result;
	result.reserve(memories.size());
	साठी (स्वतः &it : memories)
		जर (design->selected(हे, it.second))
			result.ढकला_back(it.second);
	परत result;
}

std::vector<RTLIL::Process*> RTLIL::Module::selected_processes() स्थिरांक
{
	std::vector<RTLIL::Process*> result;
	result.reserve(processes.size());
	साठी (स्वतः &it : processes)
		जर (design->selected(हे, it.second))
			result.ढकला_back(it.second);
	परत result;
}

std::vector<RTLIL::SelectedMember> RTLIL::Module::selected_members() स्थिरांक
{
	std::vector<RTLIL::SelectedMember> result;
	साठी (स्वतः cell : selected_cells())
		result.ढकला_back({cell, cell->name});
	साठी (स्वतः memory : selected_memories())
		result.ढकला_back({memory, memory->name});
	साठी (स्वतः wire : selected_wires())
		result.ढकला_back({wire, wire->name});
	साठी (स्वतः process : selected_processes())
		result.ढकला_back({process, process->name});
	परत result;
}

शून्य RTLIL::Module::add(RTLIL::Wire *wire)
{
	log_assert(wire->name != IdString::Null);
	IdString id = wire->name;
	log_assert(मोजणी_id(id) == 0);
	log_assert(refcount_wires_ == 0);
	wires_[id] = wire;
	wire->module = हे;
}

शून्य RTLIL::Module::add(RTLIL::Cell *cell)
{
	log_assert(cell->name != IdString::Null);
	IdString id = cell->name;
	log_assert(मोजणी_id(id) == 0);
	log_assert(refcount_cells_ == 0);
	cells_[id] = cell;
	cell->module = हे;
}

शून्य RTLIL::Module::add(RTLIL::Process *process)
{
	log_assert(process->name != IdString::Null);
	log_assert(मोजणी_id(process->name) == 0);
	processes[process->name] = process;
	process->module = हे;
}

शून्य RTLIL::Module::हटाओ(स्थिरांक pool<RTLIL::Wire*> &wires)
{
	log_assert(refcount_wires_ == 0);
	जर (wires.empty())
		परत;

	संरचना DeleteWireWorker
	{
		RTLIL::Module *module;
		स्थिरांक pool<RTLIL::Wire*> *wires_p;

		शून्य चालक()(RTLIL::SigSpec &sig) {
			sig.rewrite_wires([हे](RTLIL::Wire *&wire) {
				जर (wires_p->मोजणी(wire))
					wire = module->addWire(stringf("$delete_wire$%d", (पूर्णांक)autoidx++), wire->width);
			});
		}

		शून्य चालक()(RTLIL::SigSpec &lhs, RTLIL::SigSpec &rhs) {
			// If a deleted wire occurs on the lhs or rhs we just remove that part
			// of the assignment
			lhs.remove2(*wires_p, &rhs);
			rhs.remove2(*wires_p, &lhs);
		}
	};

	DeleteWireWorker delete_wire_worker;
	delete_wire_worker.module = हे;
	delete_wire_worker.wires_p = &wires;
	rewrite_sigspecs2(delete_wire_worker);

	जर (design->flagBufferedNormalized) {
		साठी (स्वतः wire : wires) {
			buf_norm_wire_queue.erase(wire);
			buf_norm_connect_index.erase(wire);
		}
	}

	साठी (स्वतः &it : wires) {
		log_assert(it->name != IdString::Null);
		IdString id = it->name;
		log_assert(wires_.मोजणी(id) != 0);
		wires_.erase(id);
		delete it;
	}
}

शून्य RTLIL::Module::हटाओ(RTLIL::Cell *cell)
{
	पर्यंत (!cell->connections_.empty())
		cell->unsetPort(cell->connections_.begin()->first);

	log_assert(cells_.मोजणी(cell->name) != 0);
	log_assert(refcount_cells_ == 0);
	cells_.erase(cell->name);
	जर (design && design->flagBufferedNormalized && buf_norm_cell_queue.मोजणी(cell)) {
		cell->type = IdString::Null;
		cell->name = IdString::Null;
		pending_deleted_cells.घाला(cell);
	} अन्यथा {
		delete cell;
	}
}

शून्य RTLIL::Module::हटाओ(RTLIL::Memory *memory)
{
	log_assert(memories.मोजणी(memory->name) != 0);
	memories.erase(memory->name);
	delete memory;
}

शून्य RTLIL::Module::हटाओ(RTLIL::Process *process)
{
	log_assert(processes.मोजणी(process->name) != 0);
	processes.erase(process->name);
	delete process;
}

शून्य RTLIL::Module::नाम(RTLIL::Wire *wire, RTLIL::IdString new_name)
{
	log_assert(wire->name != IdString::Null);
	IdString old_id = wire->name;
	log_assert(wires_[old_id] == wire);
	log_assert(refcount_wires_ == 0);
	wires_.erase(old_id);
	wire->name = new_name;
	add(wire);
}

शून्य RTLIL::Module::नाम(RTLIL::Cell *cell, RTLIL::IdString new_name)
{
	log_assert(cell->name != IdString::Null);
	IdString old_id = cell->name;
	log_assert(cells_[old_id] == cell);
	log_assert(refcount_cells_ == 0);
	cells_.erase(old_id);
	cell->name = new_name;
	add(cell);
}

शून्य RTLIL::Module::नाम(RTLIL::IdString old_name, RTLIL::IdString new_name)
{
	IdString old_id = old_name;
	जर (old_id != IdString::Null && wires_.मोजणी(old_id))
		नाम(wires_.at(old_id), new_name);
	अन्यथा जर (old_id != IdString::Null && cells_.मोजणी(old_id))
		नाम(cells_.at(old_id), new_name);
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
	पूर्णांक index = 0;
	परत uniquify(name, index);
}

IdString RTLIL::Module::uniquify(IdString name, पूर्णांक &index)
{
	जर (index == 0) {
		जर (मोजणी_id(name) == 0)
			परत name;
		index++;
	}

	पर्यंत (1) {
		IdString new_name = design->twines().add(TwineSpec::Suffix{name, stringf("_%d", index)});
		जर (मोजणी_id(new_name) == 0)
			परत new_name;
		index++;
	}
}

स्थिर तार्किक fixup_ports_compare(स्थिरांक RTLIL::Wire *a, स्थिरांक RTLIL::Wire *b)
{
	जर (a->port_id && !b->port_id)
		परत सत्य;
	जर (!a->port_id && b->port_id)
		परत असत्य;

	जर (a->port_id == b->port_id)
		परत a->name < b->name;
	परत a->port_id < b->port_id;
}

शून्य RTLIL::Module::connect(स्थिरांक RTLIL::SigSig &conn)
{
	साठी (स्वतः mon : monitors)
		mon->notify_connect(हे, conn);

	जर (design)
		साठी (स्वतः mon : design->monitors)
			mon->notify_connect(हे, conn);

	// ignore all attempts to assign constants to other constants
	जर (conn.first.has_const()) {
		RTLIL::SigSig new_conn;
		साठी (पूर्णांक i = 0; i < GetSize(conn.first); i++)
			जर (conn.first[i].wire) {
				new_conn.first.append(conn.first[i]);
				new_conn.second.append(conn.second[i]);
			}
		जर (GetSize(new_conn.first))
			connect(new_conn);
		परत;
	}

	जर (yosys_xtrace) {
		log("#X# Connect (SigSig) in %s: %s = %s (%d bits)\n", हे, log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	log_assert(GetSize(conn.first) == GetSize(conn.second));
	connections_.ढकला_back(conn);
}

शून्य RTLIL::Module::connect(स्थिरांक RTLIL::SigSpec &lhs, स्थिरांक RTLIL::SigSpec &rhs)
{
	connect(RTLIL::SigSig(lhs, rhs));
}

शून्य RTLIL::Module::new_connections(स्थिरांक std::vector<RTLIL::SigSig> &new_conn)
{
	साठी (स्वतः mon : monitors)
		mon->notify_connect(हे, new_conn);

	जर (design)
		साठी (स्वतः mon : design->monitors)
			mon->notify_connect(हे, new_conn);

	जर (yosys_xtrace) {
		log("#X# New connections vector in %s:\n", हे);
		साठी (स्वतः &conn: new_conn)
			log("#X#    %s = %s (%d bits)\n", log_signal(conn.first), log_signal(conn.second), GetSize(conn.first));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	connections_ = new_conn;
}

स्थिरांक std::vector<RTLIL::SigSig> &RTLIL::Module::connections() स्थिरांक
{
	परत connections_;
}

शून्य RTLIL::Module::fixup_ports()
{
	std::vector<RTLIL::Wire*> all_ports;

	साठी (स्वतः &w : wires_)
		जर (w.second->port_आदान || w.second->port_output)
			all_ports.ढकला_back(w.second);
		अन्यथा
			w.second->port_id = 0;

	std::sort(all_ports.begin(), all_ports.end(), fixup_ports_compare);

	जर (design && design->flagBufferedNormalized) {
		साठी (स्वतः &w : wires_)
			जर (w.second->driverCell_ && w.second->driverCell_->type == ID($आदान_port))
				buf_norm_wire_queue.घाला(w.second);

		buf_norm_wire_queue.घाला(all_ports.begin(), all_ports.end());
	}

	ports.clear();
	साठी (माप_प्रकार i = 0; i < all_ports.size(); i++) {
		ports.ढकला_back(all_ports[i]->name);
		all_ports[i]->port_id = i+1;
	}
}

शून्य RTLIL::copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
		स्थिरांक dict<IdString, RTLIL::Const> &src,
		स्थिरांक RTLIL::Design *src_design, RTLIL::Design *dst_design)
{
	जर (!src_design || !dst_design || src_design == dst_design) {
		dst = src;
		परत;
	}
	dst.clear();
	साठी (पूर्णांक i = GetSize(src) - 1; i >= 0; i--) {
		स्वतः &it = *src.element(i);
		dst[dst_design->twines().copy_from(src_design->twines(), it.first)] = it.second;
	}
}

RTLIL::Wire *RTLIL::Module::addWire(RTLIL::IdString name, पूर्णांक width)
{
	log_assert(design);
	log_assert(width >= 0 && width < RTLIL::WIDTH_LIMIT);
	RTLIL::Wire *wire = नवीन RTLIL::Wire;
	wire->width = width;
	wire->name = name;
	add(wire);
	परत wire;
}

RTLIL::Wire *RTLIL::Module::addWire(IdString name, स्थिरांक RTLIL::Wire *other)
{
	RTLIL::Wire *wire = addWire(name);
	wire->width = other->width;
	wire->start_offset = other->start_offset;
	wire->port_id = other->port_id;
	wire->port_आदान = other->port_आदान;
	wire->port_output = other->port_output;
	wire->upto = other->upto;
	wire->is_signed = other->is_signed;
	{
		स्थिरांक RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
		RTLIL::copy_attr_dict(wire->attributes, other->attributes, src_design, हे->design);
	}
	परत wire;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, IdString type)
{
	log_assert(design);
	RTLIL::Cell *cell = नवीन RTLIL::Cell;
	cell->type = type;
	cell->name = name;
	add(cell);
	परत cell;
}

RTLIL::Cell *RTLIL::Module::addCell(IdString name, स्थिरांक RTLIL::Cell *other)
{
	स्थिरांक RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	तार्किक cross_pool = src_design && हे->design && src_design != हे->design;

	IdString type = other->type.ref();
	जर (cross_pool)
		type = twines().copy_from(src_design->twines(), other->type.ref());

	RTLIL::Cell *cell = addCell(name, type);
	RTLIL::copy_attr_dict(cell->parameters, other->parameters, src_design, हे->design);
	RTLIL::copy_attr_dict(cell->attributes, other->attributes, src_design, हे->design);

	जर (!cross_pool)
		cell->connections_ = other->connections_;
	अन्यथा
		साठी (पूर्णांक i = GetSize(other->connections_) - 1; i >= 0; i--) {
			स्वतः &c = *other->connections_.element(i);
			cell->connections_[twines().copy_from(src_design->twines(), c.first)] = c.second;
		}
	परत cell;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name)
{
	log_assert(design);
	RTLIL::Memory *mem = नवीन RTLIL::Memory;
	mem->module = हे;
	mem->name = name;
	memories[name] = mem;
	परत mem;
}

RTLIL::Memory *RTLIL::Module::addMemory(IdString name, स्थिरांक RTLIL::Memory *other)
{
	log_assert(design);
	RTLIL::Memory *mem = नवीन RTLIL::Memory;
	mem->module = हे;
	mem->name = name;
	mem->width = other->width;
	mem->start_offset = other->start_offset;
	mem->size = other->size;
	RTLIL::copy_attr_dict(mem->attributes, other->attributes,
			other->module ? other->module->design : nullptr, design);
	memories[name] = mem;
	परत mem;
}

RTLIL::Process *RTLIL::Module::addProcess(IdString name)
{
	log_assert(design);
	RTLIL::Process *proc = नवीन RTLIL::Process;
	proc->name = name;
	add(proc);
	परत proc;
}

RTLIL::Process *RTLIL::Module::addProcess(RTLIL::IdString name, स्थिरांक RTLIL::Process *other)
{
	log_assert(design);
	RTLIL::Process *proc = other->clone();
	proc->name = name;
	स्थिरांक RTLIL::Design *src_design = other->module ? other->module->design : nullptr;
	जर (src_design && src_design != design) {
		walk_process_attr_objects(proc, [&](RTLIL::AttrObject *obj) {
			dict<IdString, RTLIL::Const> attributes;
			RTLIL::copy_attr_dict(attributes, obj->attributes, src_design, design);
			obj->attributes = std::move(attributes);
		});
		साठी (स्वतः *समकालिक : proc->syncs)
			साठी (स्वतः &mwa : समकालिक->mem_write_actions)
				mwa.memid = design->twines().copy_from(src_design->twines(), mwa.memid);
	}
	add(proc);
	परत proc;
}

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, तार्किक is_signed, स्थिरांक std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::A_SIGNED] = is_signed;         \
		cell->parameters[ID::A_WIDTH] = sig_a.size();       \
		cell->parameters[ID::Y_WIDTH] = sig_y.size();       \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		परत cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, तार्किक is_signed, स्थिरांक std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		परत sig_y;                                       \
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
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, तार्किक /* is_signed */, स्थिरांक std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);           \
		cell->parameters[ID::WIDTH] = sig_a.size();         \
		cell->setPort(ID::A, sig_a);                        \
		cell->setPort(ID::Y, sig_y);                        \
		cell->set_src_attribute(src);                       \
		परत cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, तार्किक is_signed, स्थिरांक std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);    \
		add ## _func(name, sig_a, sig_y, is_signed, src);   \
		परत sig_y;                                       \
	}
DEF_METHOD(Buf,     sig_a.size(), ID($buf))
DEF_METHOD(Barrier, sig_a.size(), ID($barrier))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, तार्किक is_signed, स्थिरांक std::string &src) { \
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
		परत cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, तार्किक is_signed, स्थिरांक std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		परत sig_y;                                            \
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
DEF_METHOD(Div,      max(sig_a.size(), sig_b.size()), ID($भाग))
DEF_METHOD(Mod,      max(sig_a.size(), sig_b.size()), ID($mod))
DEF_METHOD(DivFloor, max(sig_a.size(), sig_b.size()), ID($divfloor))
DEF_METHOD(ModFloor, max(sig_a.size(), sig_b.size()), ID($modfloor))
DEF_METHOD(LogicAnd, 1, ID($logic_and))
DEF_METHOD(LogicOr,  1, ID($logic_or))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, तार्किक is_signed, स्थिरांक std::string &src) { \
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
		परत cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, तार्किक is_signed, स्थिरांक std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		परत sig_y;                                            \
	}
DEF_METHOD(Shl,      sig_a.size(), ID($shl))
DEF_METHOD(Shr,      sig_a.size(), ID($shr))
DEF_METHOD(Sshl,     sig_a.size(), ID($sshl))
DEF_METHOD(Sshr,     sig_a.size(), ID($sshr))
#undef DEF_METHOD

#define DEF_METHOD(_func, _y_size, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, तार्किक is_signed, स्थिरांक std::string &src) { \
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
		परत cell;                                        \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, तार्किक is_signed, स्थिरांक std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _y_size);         \
		add ## _func(name, sig_a, sig_b, sig_y, is_signed, src); \
		परत sig_y;                                            \
	}
DEF_METHOD(Shiftx,      sig_a.size(), ID($shiftx))
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _pmux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		जर (_pmux) cell->parameters[ID::S_WIDTH] = sig_s.size();  \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		परत cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_b, sig_s, sig_y, src);      \
		परत sig_y;                                             \
	}
DEF_METHOD(Mux,      ID($mux),        0)
DEF_METHOD(Bwmux,    ID($bwmux),      0)
DEF_METHOD(Pmux,     ID($pmux),       1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type, _demux) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = _demux ? sig_a.size() : sig_y.size(); \
		cell->parameters[ID::S_WIDTH] = sig_s.size();             \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::S, sig_s);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		परत cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, _demux ? sig_a.size() << sig_s.size() : sig_a.size() >> sig_s.size()); \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		परत sig_y;                                             \
	}
DEF_METHOD(Bmux,     ID($bmux),       0)
DEF_METHOD(Demux,    ID($demux),      1)
#undef DEF_METHOD

#define DEF_METHOD(_func, _type) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);                 \
		cell->parameters[ID::WIDTH] = sig_a.size();               \
		cell->setPort(ID::A, sig_a);                              \
		cell->setPort(ID::B, sig_b);                              \
		cell->setPort(ID::Y, sig_y);                              \
		cell->set_src_attribute(src);                             \
		परत cell;                                              \
	} \
	RTLIL::SigSpec RTLIL::Module::_func(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक std::string &src) { \
		RTLIL::SigSpec sig_y = addWire(NEW_ID, sig_a.size());     \
		add ## _func(name, sig_a, sig_s, sig_y, src);             \
		परत sig_y;                                             \
	}
DEF_METHOD(Bweqx,    ID($bweqx))
#undef DEF_METHOD

#define DEF_METHOD_2(_func, _type, _P1, _P2) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig1, स्थिरांक RTLIL::SigBit &sig2, स्थिरांक std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->set_src_attribute(src);                     \
		परत cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig1, स्थिरांक std::string &src) { \
		RTLIL::SigBit sig2 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, src);              \
		परत sig2;                                      \
	}
#define DEF_METHOD_3(_func, _type, _P1, _P2, _P3) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig1, स्थिरांक RTLIL::SigBit &sig2, स्थिरांक RTLIL::SigBit &sig3, स्थिरांक std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->set_src_attribute(src);                     \
		परत cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig1, स्थिरांक RTLIL::SigBit &sig2, स्थिरांक std::string &src) { \
		RTLIL::SigBit sig3 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, src);        \
		परत sig3;                                      \
	}
#define DEF_METHOD_4(_func, _type, _P1, _P2, _P3, _P4) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig1, स्थिरांक RTLIL::SigBit &sig2, स्थिरांक RTLIL::SigBit &sig3, स्थिरांक RTLIL::SigBit &sig4, स्थिरांक std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->set_src_attribute(src);                     \
		परत cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig1, स्थिरांक RTLIL::SigBit &sig2, स्थिरांक RTLIL::SigBit &sig3, स्थिरांक std::string &src) { \
		RTLIL::SigBit sig4 = addWire(NEW_ID);             \
		add ## _func(name, sig1, sig2, sig3, sig4, src);  \
		परत sig4;                                      \
	}
#define DEF_METHOD_5(_func, _type, _P1, _P2, _P3, _P4, _P5) \
	RTLIL::Cell* RTLIL::Module::add ## _func(RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig1, स्थिरांक RTLIL::SigBit &sig2, स्थिरांक RTLIL::SigBit &sig3, स्थिरांक RTLIL::SigBit &sig4, स्थिरांक RTLIL::SigBit &sig5, स्थिरांक std::string &src) { \
		RTLIL::Cell *cell = addCell(name, _type);         \
		cell->setPort("\\" #_P1, sig1);                   \
		cell->setPort("\\" #_P2, sig2);                   \
		cell->setPort("\\" #_P3, sig3);                   \
		cell->setPort("\\" #_P4, sig4);                   \
		cell->setPort("\\" #_P5, sig5);                   \
		cell->set_src_attribute(src);                     \
		परत cell;                                      \
	} \
	RTLIL::SigBit RTLIL::Module::_func(RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig1, स्थिरांक RTLIL::SigBit &sig2, स्थिरांक RTLIL::SigBit &sig3, स्थिरांक RTLIL::SigBit &sig4, स्थिरांक std::string &src) { \
		RTLIL::SigBit sig5 = addWire(NEW_ID);                  \
		add ## _func(name, sig1, sig2, sig3, sig4, sig5, src); \
		परत sig5;                                           \
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

RTLIL::Cell* RTLIL::Module::addPow(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, तार्किक a_signed, तार्किक b_signed, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($घात));
	cell->parameters[ID::A_SIGNED] = a_signed;
	cell->parameters[ID::B_SIGNED] = b_signed;
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::B_WIDTH] = sig_b.size();
	cell->parameters[ID::Y_WIDTH] = sig_y.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addFa(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_c, स्थिरांक RTLIL::SigSpec &sig_x, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fa));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::C, sig_c);
	cell->setPort(ID::X, sig_x);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addSlice(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, RTLIL::Const offset, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($slice));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::Y_WIDTH] = sig_y.size();
	cell->parameters[ID::OFFSET] = offset;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addConcat(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($concat));
	cell->parameters[ID::A_WIDTH] = sig_a.size();
	cell->parameters[ID::B_WIDTH] = sig_b.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addLut(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, RTLIL::Const lut, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($lut));
	cell->parameters[ID::LUT] = lut;
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addTribuf(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($tribuf));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addAssert(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($प्रतिज्ञा));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addAssume(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($assume));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addLive(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($live));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addFair(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($fair));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addCover(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($cover));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::EN, sig_en);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addEquiv(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($equiv));
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::B, sig_b);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addSr(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr, स्थिरांक RTLIL::SigSpec &sig_q, तार्किक set_polarity, तार्किक clr_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($sr));
	cell->parameters[ID::SET_POLARITY] = set_polarity;
	cell->parameters[ID::CLR_POLARITY] = clr_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::SET, sig_set);
	cell->setPort(ID::CLR, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addFf(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($ff));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addDff(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, तार्किक clk_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dff));
	cell->parameters[ID::CLK_POLARITY] = clk_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::CLK, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addDffe(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक en_polarity, स्थिरांक std::string &src)
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
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addDffsr(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, स्थिरांक RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक set_polarity, तार्किक clr_polarity, स्थिरांक std::string &src)
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
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addDffsre(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, स्थिरांक RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक en_polarity, तार्किक set_polarity, तार्किक clr_polarity, स्थिरांक std::string &src)
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
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addAdff(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_arst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_मूल्य, तार्किक clk_polarity, तार्किक arst_polarity, स्थिरांक std::string &src)
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
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addAdffe(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_arst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_मूल्य, तार्किक clk_polarity, तार्किक en_polarity, तार्किक arst_polarity, स्थिरांक std::string &src)
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
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addAldff(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_aload, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		स्थिरांक RTLIL::SigSpec &sig_ad, तार्किक clk_polarity, तार्किक aload_polarity, स्थिरांक std::string &src)
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
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addAldffe(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_aload, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		स्थिरांक RTLIL::SigSpec &sig_ad, तार्किक clk_polarity, तार्किक en_polarity, तार्किक aload_polarity, स्थिरांक std::string &src)
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
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addSdff(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_srst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_मूल्य, तार्किक clk_polarity, तार्किक srst_polarity, स्थिरांक std::string &src)
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
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addSdffe(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_srst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_मूल्य, तार्किक clk_polarity, तार्किक en_polarity, तार्किक srst_polarity, स्थिरांक std::string &src)
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
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addSdffce(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_srst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		RTLIL::Const srst_मूल्य, तार्किक clk_polarity, तार्किक en_polarity, तार्किक srst_polarity, स्थिरांक std::string &src)
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
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addDlatch(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, तार्किक en_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($dlatch));
	cell->parameters[ID::EN_POLARITY] = en_polarity;
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::EN, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatch(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_arst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		RTLIL::Const arst_मूल्य, तार्किक en_polarity, तार्किक arst_polarity, स्थिरांक std::string &src)
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
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsr(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, स्थिरांक RTLIL::SigSpec &sig_q, तार्किक en_polarity, तार्किक set_polarity, तार्किक clr_polarity, स्थिरांक std::string &src)
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
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addSrGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr,
		स्थिरांक RTLIL::SigSpec &sig_q, तार्किक set_polarity, तार्किक clr_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SR_%c%c_", set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addFfGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($_FF_));
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addDffGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, तार्किक clk_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c_", clk_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addDffeGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक en_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c_", clk_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addDffsrGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, स्थिरांक RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक set_polarity, तार्किक clr_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSR_%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addDffsreGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, स्थिरांक RTLIL::SigSpec &sig_q, तार्किक clk_polarity, तार्किक en_polarity, तार्किक set_polarity, तार्किक clr_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFSRE_%c%c%c%c_", clk_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addAdffGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_arst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		तार्किक arst_मूल्य, तार्किक clk_polarity, तार्किक arst_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFF_%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_मूल्य ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addAdffeGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_arst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		तार्किक arst_मूल्य, तार्किक clk_polarity, तार्किक en_polarity, तार्किक arst_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_मूल्य ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addAldffGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_aload, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		स्थिरांक RTLIL::SigSpec &sig_ad, तार्किक clk_polarity, तार्किक aload_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFF_%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addAldffeGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_aload, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		स्थिरांक RTLIL::SigSpec &sig_ad, तार्किक clk_polarity, तार्किक en_polarity, तार्किक aload_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_ALDFFE_%c%c%c_", clk_polarity ? 'P' : 'N', aload_polarity ? 'P' : 'N', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::L, sig_aload);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::AD, sig_ad);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addSdffGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_srst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		तार्किक srst_मूल्य, तार्किक clk_polarity, तार्किक srst_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFF_%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_मूल्य ? '1' : '0'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addSdffeGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_srst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		तार्किक srst_मूल्य, तार्किक clk_polarity, तार्किक en_polarity, तार्किक srst_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_मूल्य ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addSdffceGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_srst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		तार्किक srst_मूल्य, तार्किक clk_polarity, तार्किक en_polarity, तार्किक srst_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_SDFFCE_%c%c%c%c_", clk_polarity ? 'P' : 'N', srst_polarity ? 'P' : 'N', srst_मूल्य ? '1' : '0', en_polarity ? 'P' : 'N'));
	cell->setPort(ID::C, sig_clk);
	cell->setPort(ID::R, sig_srst);
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, तार्किक en_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c_", en_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addAdlatchGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_arst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
		तार्किक arst_मूल्य, तार्किक en_polarity, तार्किक arst_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCH_%c%c%c_", en_polarity ? 'P' : 'N', arst_polarity ? 'P' : 'N', arst_मूल्य ? '1' : '0'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::R, sig_arst);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addDlatchsrGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr,
		RTLIL::SigSpec sig_d, स्थिरांक RTLIL::SigSpec &sig_q, तार्किक en_polarity, तार्किक set_polarity, तार्किक clr_polarity, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, stringf("$_DLATCHSR_%c%c%c_", en_polarity ? 'P' : 'N', set_polarity ? 'P' : 'N', clr_polarity ? 'P' : 'N'));
	cell->setPort(ID::E, sig_en);
	cell->setPort(ID::S, sig_set);
	cell->setPort(ID::R, sig_clr);
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::Cell* RTLIL::Module::addAnyinit(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($anyinit));
	cell->parameters[ID::WIDTH] = sig_q.size();
	cell->setPort(ID::D, sig_d);
	cell->setPort(ID::Q, sig_q);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::SigSpec RTLIL::Module::Anyconst(RTLIL::IdString name, पूर्णांक width, स्थिरांक std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	परत sig;
}

RTLIL::SigSpec RTLIL::Module::Anyseq(RTLIL::IdString name, पूर्णांक width, स्थिरांक std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($anyseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	परत sig;
}

RTLIL::SigSpec RTLIL::Module::Allconst(RTLIL::IdString name, पूर्णांक width, स्थिरांक std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allconst));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	परत sig;
}

RTLIL::SigSpec RTLIL::Module::Allseq(RTLIL::IdString name, पूर्णांक width, स्थिरांक std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, width);
	Cell *cell = addCell(name, ID($allseq));
	cell->setParam(ID::WIDTH, width);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	परत sig;
}

RTLIL::SigSpec RTLIL::Module::Initstate(RTLIL::IdString name, स्थिरांक std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID);
	Cell *cell = addCell(name, ID($initstate));
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	परत sig;
}

RTLIL::SigSpec RTLIL::Module::SetTag(RTLIL::IdString name, स्थिरांक std::string &tag, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक RTLIL::SigSpec &sig_c, स्थिरांक std::string &src)
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
	परत sig;
}

RTLIL::Cell* RTLIL::Module::addSetTag(RTLIL::IdString name, स्थिरांक std::string &tag, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक RTLIL::SigSpec &sig_c, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src)
{
	Cell *cell = addCell(name, ID($set_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->setPort(ID::Y, sig_y);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::SigSpec RTLIL::Module::GetTag(RTLIL::IdString name, स्थिरांक std::string &tag, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($get_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	परत sig;
}

RTLIL::Cell* RTLIL::Module::addOverwriteTag(RTLIL::IdString name, स्थिरांक std::string &tag, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक RTLIL::SigSpec &sig_c, स्थिरांक std::string &src)
{
	RTLIL::Cell *cell = addCell(name, ID($overwrite_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::SET, sig_s);
	cell->setPort(ID::CLR, sig_c);
	cell->set_src_attribute(src);
	परत cell;
}

RTLIL::SigSpec RTLIL::Module::OriginalTag(RTLIL::IdString name, स्थिरांक std::string &tag, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_a.size());
	Cell *cell = addCell(name, ID($original_tag));
	cell->parameters[ID::WIDTH] = sig_a.size();
	cell->parameters[ID::TAG] = tag;
	cell->setPort(ID::A, sig_a);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	परत sig;
}

RTLIL::SigSpec RTLIL::Module::FutureFF(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_e, स्थिरांक std::string &src)
{
	RTLIL::SigSpec sig = addWire(NEW_ID, sig_e.size());
	Cell *cell = addCell(name, ID($future_ff));
	cell->parameters[ID::WIDTH] = sig_e.size();
	cell->setPort(ID::A, sig_e);
	cell->setPort(ID::Y, sig);
	cell->set_src_attribute(src);
	परत sig;
}

std::string RTLIL::Module::to_rtlil_str() स्थिरांक
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_module(f, "", const_cast<RTLIL::Module*>(हे), design, असत्य, सत्य, असत्य, RTLIL_BACKEND::DumpMode::Readable);
	परत f.str();
}

RTLIL::Wire::Wire()
{
	स्थिर अचिन्हित पूर्णांक hashidx_मोजणी = 123456789;
	hashidx_मोजणी = mkhash_xorshift(hashidx_मोजणी);
	hashidx_ = hashidx_मोजणी;

	module = nullptr;
	width = 1;
	start_offset = 0;
	port_id = 0;
	port_आदान = असत्य;
	port_output = असत्य;
	upto = असत्य;
	is_signed = असत्य;

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->घाला(std::pair<अचिन्हित पूर्णांक, RTLIL::Wire*>(hashidx_, हे));
#endif
}

RTLIL::Wire::~Wire()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Wire::get_all_wires()->erase(hashidx_);
#endif
}

std::string RTLIL::Wire::to_rtlil_str() स्थिरांक
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_wire(f, "", हे, design(), RTLIL_BACKEND::DumpMode::Readable);
	परत f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
स्थिर std::map<अचिन्हित पूर्णांक, RTLIL::Wire*> all_wires;
std::map<अचिन्हित पूर्णांक, RTLIL::Wire*> *RTLIL::Wire::get_all_wires(शून्य)
{
	परत &all_wires;
}
#endif

RTLIL::Memory::Memory()
{
	स्थिर अचिन्हित पूर्णांक hashidx_मोजणी = 123456789;
	hashidx_मोजणी = mkhash_xorshift(hashidx_मोजणी);
	hashidx_ = hashidx_मोजणी;

	width = 1;
	start_offset = 0;
	size = 0;
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->घाला(std::pair<अचिन्हित पूर्णांक, RTLIL::Memory*>(hashidx_, हे));
#endif
}

std::string RTLIL::Memory::to_rtlil_str() स्थिरांक
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_memory(f, "", हे, design(), RTLIL_BACKEND::DumpMode::Readable);
	परत f.str();
}

RTLIL::Process::Process() : module(nullptr)
{
	स्थिर अचिन्हित पूर्णांक hashidx_मोजणी = 123456789;
	hashidx_मोजणी = mkhash_xorshift(hashidx_मोजणी);
	hashidx_ = hashidx_मोजणी;
}

std::string RTLIL::Process::to_rtlil_str() स्थिरांक
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_proc(f, "", हे, design(), RTLIL_BACKEND::DumpMode::Readable);
	परत f.str();
}

RTLIL::Cell::Cell() : module(nullptr)
{
	स्थिर अचिन्हित पूर्णांक hashidx_मोजणी = 123456789;
	hashidx_मोजणी = mkhash_xorshift(hashidx_मोजणी);
	hashidx_ = hashidx_मोजणी;

	// log("#memtrace# %p\n", this);
	memhasher();

#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->घाला(std::pair<अचिन्हित पूर्णांक, RTLIL::Cell*>(hashidx_, हे));
#endif
}

RTLIL::Cell::~Cell()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Cell::get_all_cells()->erase(hashidx_);
#endif
}

std::string RTLIL::Cell::to_rtlil_str() स्थिरांक
{
	std::ostringstream f;
	RTLIL_BACKEND::dump_cell(f, "", हे, design(), RTLIL_BACKEND::DumpMode::Readable);
	परत f.str();
}

#ifdef YOSYS_ENABLE_PYTHON
स्थिर std::map<अचिन्हित पूर्णांक, RTLIL::Cell*> all_cells;
std::map<अचिन्हित पूर्णांक, RTLIL::Cell*> *RTLIL::Cell::get_all_cells(शून्य)
{
	परत &all_cells;
}
#endif

तार्किक RTLIL::Cell::hasPort(RTLIL::IdString portname) स्थिरांक
{
	परत connections_.मोजणी(portname) != 0;
}

// bufnorm

स्थिरांक RTLIL::SigSpec &RTLIL::Cell::getPort(RTLIL::IdString portname) स्थिरांक
{
	परत connections_.at(portname);
}

स्थिरांक dict<RTLIL::IdString, RTLIL::SigSpec> &RTLIL::Cell::connections() स्थिरांक
{
	परत connections_;
}

तार्किक RTLIL::Cell::known() स्थिरांक
{
	जर (yosys_celltypes.cell_known(type))
		परत सत्य;
	जर (module && module->design && module->design->module(type))
		परत सत्य;
	परत असत्य;
}

तार्किक RTLIL::Cell::आदान(IdString portname) स्थिरांक
{
	जर (yosys_celltypes.cell_known(type))
		परत yosys_celltypes.cell_आदान(type.ref(), portname);
	जर (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		परत w && w->port_आदान;
	}
	परत असत्य;
}

तार्किक RTLIL::Cell::output(RTLIL::IdString portname) स्थिरांक
{
	जर (yosys_celltypes.cell_known(type))
		परत yosys_celltypes.cell_output(type, portname);
	जर (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		RTLIL::Wire *w = m ? m->wire(portname) : nullptr;
		परत w && w->port_output;
	}
	परत असत्य;
}

RTLIL::PortDir RTLIL::Cell::port_dir(RTLIL::IdString portname) स्थिरांक
{
	जर (yosys_celltypes.cell_known(type.ref()))
		परत yosys_celltypes.cell_port_dir(type, portname);
	जर (module && module->design) {
		RTLIL::Module *m = module->design->module(type.ref());
		जर (m == nullptr)
			परत PortDir::PD_UNKNOWN;
		RTLIL::Wire *w = m->wire(portname);
		जर (w == nullptr)
			परत PortDir::PD_UNKNOWN;
		परत PortDir(w->port_आदान + w->port_output * 2);
	}
	परत PortDir::PD_UNKNOWN;
}

तार्किक RTLIL::Cell::hasParam(RTLIL::IdString paramname) स्थिरांक
{
	परत parameters.मोजणी(paramname) != 0;
}

शून्य RTLIL::Cell::unsetParam(RTLIL::IdString paramname)
{
	parameters.erase(paramname);
}

शून्य RTLIL::Cell::setParam(RTLIL::IdString paramname, RTLIL::Const मूल्य)
{
	parameters[paramname] = std::move(मूल्य);
}

स्थिरांक RTLIL::Const &RTLIL::Cell::getParam(RTLIL::IdString paramname) स्थिरांक
{
	स्थिरांक स्वतः &it = parameters.find(paramname);
	जर (it != parameters.end())
		परत it->second;
	जर (module && module->design) {
		RTLIL::Module *m = module->design->module(type);
		जर (m)
			परत m->parameter_default_मूल्ये.at(paramname);
	}
	फेका std::out_of_range("Cell::getParam()");
}

शून्य RTLIL::Cell::sort()
{
	connections_.sort(sort_by_id_str(module->twines()));
	parameters.sort(sort_by_id_str(module->twines()));
	attributes.sort(sort_by_id_str(module->twines()));
}

शून्य RTLIL::Cell::check()
{
#ifndef न_संशोधन
	InternalCellChecker checker(रिक्त, हे);
	checker.check();
#endif
}

शून्य RTLIL::Cell::fixup_parameters(तार्किक set_a_signed, तार्किक set_b_signed)
{
	जर (type.isPublic() || type.begins_with("$_") || type.begins_with("$paramod") || type.begins_with("$fmcombine") ||
			type.begins_with("$verific$") || type.begins_with("$array:") || type.begins_with("$extern:"))
		परत;

	जर (type == ID($buf) || type == ID($barrier) || type == ID($mux) || type == ID($pmux) || type == ID($bmux) || type == ID($bwmux) || type == ID($bweqx)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		जर (type.in(ID($pmux), ID($bmux)))
			parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		परत;
	}

	जर (type == ID($demux)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		parameters[ID::S_WIDTH] = GetSize(connections_[ID::S]);
		check();
		परत;
	}

	जर (type == ID($lut) || type == ID($sop)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::A]);
		परत;
	}

	जर (type == ID($fa)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::Y]);
		परत;
	}

	जर (type == ID($lcu)) {
		parameters[ID::WIDTH] = GetSize(connections_[ID::CO]);
		परत;
	}

	जर (type == ID($macc_v2)) {
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);
		परत;
	}

	तार्किक signedness_ab = !type.in(ID($slice), ID($concat), ID($macc));

	जर (connections_.मोजणी(ID::A)) {
		जर (signedness_ab) {
			जर (set_a_signed)
				parameters[ID::A_SIGNED] = सत्य;
			अन्यथा जर (parameters.मोजणी(ID::A_SIGNED) == 0)
				parameters[ID::A_SIGNED] = असत्य;
		}
		parameters[ID::A_WIDTH] = GetSize(connections_[ID::A]);
	}

	जर (connections_.मोजणी(ID::B)) {
		जर (signedness_ab) {
			जर (set_b_signed)
				parameters[ID::B_SIGNED] = सत्य;
			अन्यथा जर (parameters.मोजणी(ID::B_SIGNED) == 0)
				parameters[ID::B_SIGNED] = असत्य;
		}
		parameters[ID::B_WIDTH] = GetSize(connections_[ID::B]);
	}

	जर (connections_.मोजणी(ID::Y) && type != ID($concat))
		parameters[ID::Y_WIDTH] = GetSize(connections_[ID::Y]);

	जर (connections_.मोजणी(ID::Q))
		parameters[ID::WIDTH] = GetSize(connections_[ID::Q]);

	check();
}

तार्किक RTLIL::Cell::has_keep_attr() स्थिरांक {
	परत get_bool_attribute(ID::keep) || (module && module->design && module->design->module(type.ref()) &&
			module->design->module(type.ref())->get_bool_attribute(ID::keep));
}

तार्किक RTLIL::Cell::has_memid() स्थिरांक
{
	परत type.in(ID($memwr), ID($memwr_v2), ID($memrd), ID($memrd_v2), ID($meminit), ID($meminit_v2));
}

तार्किक RTLIL::Cell::is_mem_cell() स्थिरांक
{
	परत type.in(ID($mem), ID($mem_v2)) || has_memid();
}

तार्किक RTLIL::Cell::is_builtin_ff() स्थिरांक {
	परत StaticCellTypes::categories.is_ff(type);
}

RTLIL::SigChunk::SigChunk(स्थिरांक RTLIL::SigBit &bit)
{
	wire = bit.wire;
	offset = 0;
	जर (wire == रिक्त)
		data = {bit.data};
	अन्यथा
		offset = bit.offset;
	width = 1;
}

RTLIL::SigChunk RTLIL::SigChunk::extract(पूर्णांक offset, पूर्णांक length) स्थिरांक
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= width);
	RTLIL::SigChunk ret;
	जर (wire) {
		ret.wire = wire;
		ret.offset = हे->offset + offset;
		ret.width = length;
	} अन्यथा {
		साठी (पूर्णांक i = 0; i < length; i++)
			ret.data.ढकला_back(data[offset+i]);
		ret.width = length;
	}
	परत ret;
}

RTLIL::SigBit RTLIL::SigChunk::चालक[](पूर्णांक offset) स्थिरांक
{
	log_assert(offset >= 0);
	log_assert(offset < width);
	RTLIL::SigBit ret;
	जर (wire) {
		ret.wire = wire;
		ret.offset = हे->offset + offset;
	} अन्यथा {
		ret.data = data[offset];
	}
	परत ret;
}

तार्किक RTLIL::SigChunk::चालक <(स्थिरांक RTLIL::SigChunk &other) स्थिरांक
{
	जर (wire && other.wire)
		जर (wire->name != other.wire->name)
			परत wire->name < other.wire->name;

	जर (wire != other.wire)
		परत wire < other.wire;

	जर (offset != other.offset)
		परत offset < other.offset;

	जर (width != other.width)
		परत width < other.width;

	परत data < other.data;
}

तार्किक RTLIL::SigChunk::चालक ==(स्थिरांक RTLIL::SigChunk &other) स्थिरांक
{
	परत wire == other.wire && width == other.width && offset == other.offset && data == other.data;
}

तार्किक RTLIL::SigChunk::चालक !=(स्थिरांक RTLIL::SigChunk &other) स्थिरांक
{
	जर (*हे == other)
		परत असत्य;
	परत सत्य;
}

RTLIL::SigSpec::SigSpec(std::initializer_list<RTLIL::SigSpec> parts)
{
	init_empty_bits();
	log_assert(parts.size() > 0);
	स्वतः ie = parts.begin();
	स्वतः it = ie + parts.size() - 1;
	पर्यंत (it >= ie)
		append(*it--);
}

RTLIL::SigSpec::SigSpec(स्थिरांक RTLIL::Const &मूल्य)
{
	जर (GetSize(मूल्य) != 0) {
		rep_ = CHUNK;
		नवीन (&chunk_) RTLIL::SigChunk(मूल्य);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Const &&मूल्य)
{
	जर (GetSize(मूल्य) != 0) {
		rep_ = CHUNK;
		नवीन (&chunk_) RTLIL::SigChunk(मूल्य);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(स्थिरांक RTLIL::SigChunk &chunk)
{
	जर (chunk.width != 0) {
		rep_ = CHUNK;
		नवीन (&chunk_) RTLIL::SigChunk(chunk);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::SigChunk &&chunk)
{
	जर (chunk.width != 0) {
		rep_ = CHUNK;
		नवीन (&chunk_) RTLIL::SigChunk(chunk);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire)
{
	जर (wire->width != 0) {
		rep_ = CHUNK;
		नवीन (&chunk_) RTLIL::SigChunk(wire);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::Wire *wire, पूर्णांक offset, पूर्णांक width)
{
	जर (width != 0) {
		rep_ = CHUNK;
		नवीन (&chunk_) RTLIL::SigChunk(wire, offset, width);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(स्थिरांक std::string &str)
{
	जर (str.size() != 0) {
		rep_ = CHUNK;
		नवीन (&chunk_) RTLIL::SigChunk(str);
	} अन्यथा {
		init_empty_bits();
	}
	check();
}

RTLIL::SigSpec::SigSpec(पूर्णांक val, पूर्णांक width)
{
	जर (width != 0) {
		rep_ = CHUNK;
		नवीन (&chunk_) RTLIL::SigChunk(val, width);
	} अन्यथा
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(RTLIL::State bit, पूर्णांक width)
{
	जर (width != 0) {
		rep_ = CHUNK;
		नवीन (&chunk_) RTLIL::SigChunk(bit, width);
	} अन्यथा
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(स्थिरांक RTLIL::SigBit &bit, पूर्णांक width)
{
	जर (width != 0) {
		जर (bit.wire == रिक्त) {
			rep_ = CHUNK;
			नवीन (&chunk_) RTLIL::SigChunk(bit.data, width);
		} अन्यथा जर (width == 1) {
			rep_ = CHUNK;
			नवीन (&chunk_) RTLIL::SigChunk(bit);
		} अन्यथा {
			init_empty_bits();
			bits_.reserve(width);
			साठी (पूर्णांक i = 0; i < width; i++)
				bits_.ढकला_back(bit);
		}
	} अन्यथा
		init_empty_bits();
	check();
}

RTLIL::SigSpec::SigSpec(स्थिरांक std::vector<RTLIL::SigChunk> &chunks)
{
	init_empty_bits();
	साठी (स्थिरांक स्वतः &c : chunks)
		append(c);
	check();
}

RTLIL::SigSpec::SigSpec(स्थिरांक std::vector<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	साठी (स्थिरांक स्वतः &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(स्थिरांक pool<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	साठी (स्थिरांक स्वतः &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(स्थिरांक std::set<RTLIL::SigBit> &bits)
{
	init_empty_bits();
	साठी (स्थिरांक स्वतः &bit : bits)
		append(bit);
	check();
}

RTLIL::SigSpec::SigSpec(तार्किक bit)
{
	rep_ = CHUNK;
	नवीन (&chunk_) RTLIL::SigChunk(bit ? RTLIL::S1 : RTLIL::S0);
	check();
}

शून्य RTLIL::SigSpec::Chunks::const_iterator::next_chunk_bits()
{
	पूर्णांक bits_size = GetSize(spec.bits_);
	जर (bit_index >= bits_size)
		परत;
	पूर्णांक i = bit_index;
	स्थिरांक SigBit &bit = spec.bits_[i++];
	chunk.wire = bit.wire;
	chunk.data.clear();
	जर (bit.is_wire()) {
		chunk.offset = bit.offset;
		पर्यंत (i < bits_size && spec.bits_[i].wire == bit.wire &&
			spec.bits_[i].offset == bit.offset + i - bit_index)
			++i;
	} अन्यथा {
		chunk.offset = 0;
		chunk.data.ढकला_back(bit.data);
		पर्यंत (i < bits_size && !spec.bits_[i].is_wire()) {
			chunk.data.ढकला_back(spec.bits_[i].data);
			++i;
		}
	}
	chunk.width = i - bit_index;
}

शून्य RTLIL::SigSpec::unpack()
{
	जर (rep_ == BITS)
		परत;

	std::vector<RTLIL::SigBit> bits;
	bits.reserve(chunk_.width);
	साठी (पूर्णांक i = 0; i < chunk_.width; i++)
		bits.emplace_back(chunk_, i);

	chunk_.~SigChunk();
	rep_ = BITS;
	नवीन (&bits_) std::vector<RTLIL::SigBit>(std::move(bits));
}

शून्य RTLIL::SigSpec::try_repack()
{
	जर (rep_ != BITS)
		परत;

	पूर्णांक bits_size = GetSize(bits_);
	जर (bits_size == 0)
		परत;
	जर (bits_[0].is_wire()) {
		साठी (पूर्णांक i = 1; i < bits_size; i++)
			जर (bits_[0].wire != bits_[i].wire || bits_[0].offset + i != bits_[i].offset)
				परत;
		SigChunk chunk(bits_[0].wire, bits_[0].offset, bits_size);
		bits_.~vector();
		rep_ = CHUNK;
		नवीन (&chunk_) SigChunk(std::move(chunk));
		परत;
	}
	std::vector<RTLIL::State> bits;
	bits.reserve(bits_size);
	bits.ढकला_back(bits_[0].data);
	साठी (पूर्णांक i = 1; i < bits_size; i++) {
		जर (bits_[i].is_wire())
			परत;
		bits.ढकला_back(bits_[i].data);
	}
	bits_.~vector();
	rep_ = CHUNK;
	नवीन (&chunk_) SigChunk(std::move(bits));
}

Hasher::hash_t RTLIL::SigSpec::updhash() स्थिरांक
{
	Hasher h;
	साठी (स्वतः &c : chunks())
		जर (c.wire == रिक्त) {
			साठी (स्वतः &v : c.data)
				h.eat(v);
		} अन्यथा {
			h.eat(c.wire->name.ref());
			h.eat(c.offset);
			h.eat(c.width);
		}
	Hasher::hash_t result = h.yield();
	जर (result == 0)
		result = 1;
	hash_.set(result);
	परत result;
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
	स्वतः last = std::unique(unique_bits.begin(), unique_bits.end());
	unique_bits.erase(last, unique_bits.end());

	*हे = unique_bits;
	hash_.clear();
	try_repack();
}

शून्य RTLIL::SigSpec::replace(स्थिरांक RTLIL::SigSpec &pattern, स्थिरांक RTLIL::SigSpec &with)
{
	replace(pattern, with, हे);
}

शून्य RTLIL::SigSpec::replace(स्थिरांक RTLIL::SigSpec &pattern, स्थिरांक RTLIL::SigSpec &with, RTLIL::SigSpec *other) स्थिरांक
{
	log_assert(other != रिक्त);
	log_assert(size() == other->size());
	log_assert(pattern.size() == with.size());

	dict<RTLIL::SigBit, पूर्णांक> pattern_to_with;
	पूर्णांक pattern_size = pattern.size();
	साठी (पूर्णांक i = 0; i < pattern_size; i++) {
		SigBit pattern_bit = pattern[i];
		जर (pattern_bit.wire != रिक्त) {
			pattern_to_with.emplace(pattern_bit, i);
		}
	}

	पूर्णांक this_size = size();
	तार्किक other_modified = असत्य;
	साठी (पूर्णांक j = 0; j < this_size; j++) {
		स्वतः it = pattern_to_with.find((*हे)[j]);
		जर (it != pattern_to_with.end()) {
			other_modified = सत्य;
			(*other)[j] = with[it->second];
		}
	}

	जर (other_modified)
		other->try_repack();
	other->check();
}

शून्य RTLIL::SigSpec::replace(स्थिरांक dict<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, हे);
}

शून्य RTLIL::SigSpec::replace(स्थिरांक dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) स्थिरांक
{
	log_assert(other != रिक्त);
	log_assert(size() == other->size());

	जर (rules.empty()) परत;

	पूर्णांक this_size = size();
	तार्किक other_modified = असत्य;
	साठी (पूर्णांक i = 0; i < this_size; i++) {
		स्वतः it = rules.find((*हे)[i]);
		जर (it != rules.end()) {
			other_modified = सत्य;
			(*other)[i] = it->second;
		}
	}

	जर (other_modified)
		other->try_repack();
	other->check();
}

शून्य RTLIL::SigSpec::replace(स्थिरांक std::map<RTLIL::SigBit, RTLIL::SigBit> &rules)
{
	replace(rules, हे);
}

शून्य RTLIL::SigSpec::replace(स्थिरांक std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) स्थिरांक
{
	log_assert(other != रिक्त);
	log_assert(size() == other->size());

	जर (rules.empty()) परत;

	पूर्णांक this_size = size();
	तार्किक other_modified = असत्य;
	साठी (पूर्णांक i = 0; i < this_size; i++) {
		स्वतः it = rules.find((*हे)[i]);
		जर (it != rules.end()) {
			other_modified = सत्य;
			(*other)[i] = it->second;
		}
	}

	जर (other_modified)
		other->try_repack();
	other->check();
}

शून्य RTLIL::SigSpec::हटाओ(स्थिरांक RTLIL::SigSpec &pattern)
{
	remove2(pattern, रिक्त);
}

शून्य RTLIL::SigSpec::हटाओ(स्थिरांक RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) स्थिरांक
{
	RTLIL::SigSpec tmp = *हे;
	tmp.remove2(pattern, other);
}

शून्य RTLIL::SigSpec::remove2(स्थिरांक RTLIL::SigSpec &pattern, RTLIL::SigSpec *other)
{
	unpack();
	जर (other != रिक्त) {
		log_assert(size() == other->size());
		other->unpack();
	}

	//  Convert pattern to pool for O(1) lookup, avoiding O(n*m) chunk iteration
	pool<SigBit> pattern_bits;
	pattern_bits.reserve(pattern.size());
	साठी (स्वतः &bit : pattern)
		जर (bit.wire != रिक्त)
			pattern_bits.घाला(bit);

	// Compact in-place to avoid O(n^2) erase operations
	माप_प्रकार write_idx = 0;
	साठी (माप_प्रकार read_idx = 0; read_idx < bits_.size(); read_idx++)
	{
		जर (!(bits_[read_idx].wire != रिक्त && pattern_bits.मोजणी(bits_[read_idx]))) {
			जर (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				जर (other != रिक्त)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	तार्किक modified = (write_idx < bits_.size());
	जर (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	जर (other != रिक्त && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

शून्य RTLIL::SigSpec::हटाओ(स्थिरांक pool<RTLIL::SigBit> &pattern)
{
	remove2(pattern, रिक्त);
}

शून्य RTLIL::SigSpec::हटाओ(स्थिरांक pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) स्थिरांक
{
	RTLIL::SigSpec tmp = *हे;
	tmp.remove2(pattern, other);
}

शून्य RTLIL::SigSpec::remove2(स्थिरांक pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	जर (other != रिक्त) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	माप_प्रकार write_idx = 0;
	साठी (माप_प्रकार read_idx = 0; read_idx < bits_.size(); read_idx++) {
		जर (!(bits_[read_idx].wire != रिक्त && pattern.मोजणी(bits_[read_idx]))) {
			जर (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				जर (other != रिक्त)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	तार्किक modified = (write_idx < bits_.size());
	जर (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	जर (other != रिक्त && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

शून्य RTLIL::SigSpec::remove2(स्थिरांक std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	जर (other != रिक्त) {
		log_assert(size() == other->size());
		other->unpack();
	}

	// Avoid O(n^2) complexity by compacting in-place
	माप_प्रकार write_idx = 0;
	साठी (माप_प्रकार read_idx = 0; read_idx < bits_.size(); read_idx++) {
		जर (!(bits_[read_idx].wire != रिक्त && pattern.मोजणी(bits_[read_idx]))) {
			जर (write_idx != read_idx) {
				bits_[write_idx] = bits_[read_idx];
				जर (other != रिक्त)
					other->bits_[write_idx] = other->bits_[read_idx];
			}
			write_idx++;
		}
	}

	तार्किक modified = (write_idx < bits_.size());
	जर (modified) {
		bits_.resize(write_idx);
		hash_.clear();
		try_repack();
	}
	जर (other != रिक्त && modified) {
		other->bits_.resize(write_idx);
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

शून्य RTLIL::SigSpec::remove2(स्थिरांक pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other)
{
	unpack();

	जर (other != रिक्त) {
		log_assert(size() == other->size());
		other->unpack();
	}

	तार्किक modified = असत्य;
	तार्किक other_modified = असत्य;
	साठी (पूर्णांक i = GetSize(bits_) - 1; i >= 0; i--) {
		जर (bits_[i].wire != रिक्त && pattern.मोजणी(bits_[i].wire)) {
			modified = सत्य;
			bits_.erase(bits_.begin() + i);
			जर (other != रिक्त) {
				other_modified = सत्य;
				other->bits_.erase(other->bits_.begin() + i);
			}
		}
	}

	जर (modified) {
		hash_.clear();
		try_repack();
	}
	जर (other_modified) {
		other->hash_.clear();
		other->try_repack();
	}
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(स्थिरांक RTLIL::SigSpec &pattern, स्थिरांक RTLIL::SigSpec *other) स्थिरांक
{
	log_assert(other == रिक्त || size() == other->size());

	RTLIL::SigSpec ret;
	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();

	साठी (स्वतः& pattern_chunk : pattern.chunks()) {
		जर (other) {
			std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
			पूर्णांक i = 0;
			साठी (स्थिरांक RTLIL::SigBit &bit : bits_match) {
				जर (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bits_other[i]);
				++i;
			}
		} अन्यथा {
			साठी (स्थिरांक RTLIL::SigBit &bit : bits_match)
				जर (bit.wire &&
					bit.wire == pattern_chunk.wire &&
					bit.offset >= pattern_chunk.offset &&
					bit.offset < pattern_chunk.offset + pattern_chunk.width)
					ret.append(bit);
		}
	}

	ret.check();
	परत ret;
}

RTLIL::SigSpec RTLIL::SigSpec::extract(स्थिरांक pool<RTLIL::SigBit> &pattern, स्थिरांक RTLIL::SigSpec *other) स्थिरांक
{
	log_assert(other == रिक्त || size() == other->size());

	std::vector<RTLIL::SigBit> bits_match = to_sigbit_vector();
	RTLIL::SigSpec ret;

	जर (other) {
		std::vector<RTLIL::SigBit> bits_other = other->to_sigbit_vector();
		पूर्णांक i = 0;
		साठी (स्थिरांक RTLIL::SigBit &bit : bits_match) {
			जर (bit.wire && pattern.मोजणी(bit))
				ret.append(bits_other[i]);
			++i;
		}
	} अन्यथा {
		साठी (स्थिरांक RTLIL::SigBit &bit : bits_match)
			जर (bit.wire && pattern.मोजणी(bit))
				ret.append(bit);
	}

	ret.check();
	परत ret;
}

शून्य RTLIL::SigSpec::replace(पूर्णांक offset, स्थिरांक RTLIL::SigSpec &with)
{
	जर (with.size() == 0)
		परत;

	unpack();

	log_assert(offset >= 0);
	log_assert(with.size() >= 0);
	log_assert(offset+with.size() <= size());

	पूर्णांक i = 0;
	साठी (स्थिरांक RTLIL::SigBit &bit : with.bits()) {
		bits_.at(offset + i) = bit;
		++i;
	}
	hash_.clear();
	try_repack();

	check();
}

शून्य RTLIL::SigSpec::remove_const()
{
	जर (rep_ == CHUNK)
	{
		जर (chunk_.wire == रिक्त) {
			chunk_.~SigChunk();
			init_empty_bits();
			hash_.clear();
		}
	}
	अन्यथा
	{
		std::vector<RTLIL::SigBit> new_bits;
		new_bits.reserve(bits_.size());
		साठी (स्वतः &bit : bits_)
			जर (bit.wire != रिक्त)
				new_bits.ढकला_back(bit);
		जर (GetSize(new_bits) != GetSize(bits_)) {
			bits_.swap(new_bits);
			hash_.clear();
			try_repack();
		}
	}

	check();
}

शून्य RTLIL::SigSpec::हटाओ(पूर्णांक offset, पूर्णांक length)
{
	जर (length == 0)
		परत;

	unpack();

	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	bits_.erase(bits_.begin() + offset, bits_.begin() + offset + length);

	hash_.clear();
	try_repack();
	check();
}

RTLIL::SigSpec RTLIL::SigSpec::extract(पूर्णांक offset, पूर्णांक length) स्थिरांक
{
	log_assert(offset >= 0);
	log_assert(length >= 0);
	log_assert(offset + length <= size());

	std::vector<SigBit> extracted;
	SigBit first;
	तार्किक is_packing = सत्य;
	साठी (पूर्णांक i = offset; i < offset + length; i++) {
		तार्किक was_packing_before = is_packing;
		SigBit bit = (*हे)[i];
		जर (i == offset) {
			first = bit;
			जर (!bit.wire)
				is_packing = असत्य;
		} अन्यथा {
			जर (bit.wire != first.wire)
				is_packing = असत्य;
			जर (bit.wire)
				जर (bit.offset != first.offset + (i - offset))
					is_packing = असत्य;
		}
		जर (was_packing_before && !is_packing)
			साठी (पूर्णांक j = offset; j < i; j++)
				extracted.ढकला_back((*हे)[j]);
		जर (!is_packing)
			extracted.ढकला_back((*हे)[i]);
	}
	जर (is_packing)
		परत SigChunk(first.wire, first.offset, length);

	परत extracted;
}

शून्य RTLIL::SigSpec::rewrite_wires(std::function<शून्य(RTLIL::Wire*& wire)> rewrite)
{
	जर (rep_ == CHUNK) {
		जर (chunk_.wire != nullptr)
			rewrite(chunk_.wire);
		परत;
	}

	std::vector<RTLIL::SigBit> new_bits;
	साठी (स्थिरांक RTLIL::SigChunk &chunk : chunks()) {
		RTLIL::SigChunk c = chunk;
		जर (c.wire != nullptr)
			rewrite(c.wire);
		साठी (पूर्णांक i = 0; i < c.width; i++)
			new_bits.emplace_back(c, i);
	}
	bits_ = std::move(new_bits);
	hash_.clear();
}

शून्य RTLIL::SigSpec::append(स्थिरांक RTLIL::SigSpec &संकेत)
{
	जर (संकेत.size() == 0)
		परत;

	जर (size() == 0) {
		*हे = संकेत;
		परत;
	}

	hash_.clear();
	जर (rep_ == CHUNK && संकेत.rep_ == CHUNK && chunk_.wire == संकेत.chunk_.wire) {
		जर (chunk_.wire == रिक्त) {
			chunk_.data.घाला(chunk_.data.end(), संकेत.chunk_.data.begin(), संकेत.chunk_.data.end());
			chunk_.width = GetSize(chunk_.data);
			परत;
		}
		जर (chunk_.offset + chunk_.width == संकेत.chunk_.offset) {
			chunk_.width += संकेत.chunk_.width;
			परत;
		}
	}

	unpack();
	साठी (स्थिरांक SigBit &bit : संकेत.bits())
		bits_.ढकला_back(bit);
	check();
}

शून्य RTLIL::SigSpec::append(स्थिरांक RTLIL::SigBit &bit)
{
	hash_.clear();

	जर (size() == 0) {
		destroy();
		rep_ = CHUNK;
		नवीन (&chunk_) RTLIL::SigChunk(bit);
		परत;
	}

	जर (rep_ == CHUNK && chunk_.wire == bit.wire) {
		जर (chunk_.wire == रिक्त) {
			chunk_.data.ढकला_back(bit.data);
			chunk_.width++;
			परत;
		}
		जर (chunk_.offset + chunk_.width == bit.offset) {
			chunk_.width++;
			परत;
		}
	}

	unpack();

	bits_.ढकला_back(bit);
	check();
}

शून्य RTLIL::SigSpec::extend_u0(पूर्णांक width, तार्किक is_signed)
{
	जर (size() > width)
		हटाओ(width, size() - width);

	जर (size() < width) {
		RTLIL::SigBit padding = size() > 0 ? (*हे)[size() - 1] : RTLIL::State::Sx;
		जर (!is_signed)
			padding = RTLIL::State::S0;
		पर्यंत (size() < width)
			append(padding);
	}
}

RTLIL::SigSpec RTLIL::SigSpec::repeat(पूर्णांक अंक) स्थिरांक
{
	RTLIL::SigSpec sig;
	साठी (पूर्णांक i = 0; i < अंक; i++)
		sig.append(*हे);
	परत sig;
}

#ifndef न_संशोधन
शून्य RTLIL::SigSpec::check(स्थिरांक Module *mod) स्थिरांक
{
	जर (rep_ == CHUNK)
	{
		log_assert(chunk_.width != 0);
		जर (chunk_.wire == रिक्त) {
			log_assert(chunk_.offset == 0);
			log_assert(chunk_.data.size() == (माप_प्रकार)chunk_.width);
		} अन्यथा {
			log_assert(chunk_.offset >= 0);
			log_assert(chunk_.width >= 0);
			log_assert(chunk_.offset + chunk_.width <= chunk_.wire->width);
			log_assert(chunk_.data.size() == 0);
			जर (mod != nullptr)
				log_assert(chunk_.wire->module == mod);
		}
	}
	अन्यथा जर (size() <= 64)
	{
		जर (mod != nullptr) {
			साठी (स्थिरांक RTLIL::SigBit &bit : bits_)
				जर (bit.wire != nullptr)
					log_assert(bit.wire->module == mod);
		}
	}
}
#endif

तार्किक RTLIL::SigSpec::चालक <(स्थिरांक RTLIL::SigSpec &other) स्थिरांक
{
	जर (हे == &other)
		परत असत्य;

	जर (size() != other.size())
		परत size() < other.size();

	स्वतः other_it = other.chunks().begin();
	साठी (स्थिरांक SigChunk &c : chunks()) {
		जर (c != *other_it)
			परत c < *other_it;
		++other_it;
	}

	परत असत्य;
}

तार्किक RTLIL::SigSpec::चालक ==(स्थिरांक RTLIL::SigSpec &other) स्थिरांक
{
	जर (हे == &other)
		परत सत्य;

	जर (size() != other.size())
		परत असत्य;

	स्वतः other_it = other.chunks().begin();
	साठी (स्थिरांक SigChunk &c : chunks()) {
		जर (c != *other_it)
			परत असत्य;
		++other_it;
	}

	परत सत्य;
}

तार्किक RTLIL::SigSpec::is_wire() स्थिरांक
{
	Chunks cs = chunks();
	स्वतः it = cs.begin();
	जर (it == cs.end())
		परत असत्य;
	स्थिरांक RTLIL::SigChunk &chunk = *it;
	परत chunk.wire && chunk.wire->width == size() && ++it == cs.end();
}

तार्किक RTLIL::SigSpec::is_chunk() स्थिरांक
{
	Chunks cs = chunks();
	स्वतः it = cs.begin();
	जर (it == cs.end())
		परत असत्य;
	परत ++it == cs.end();
}

तार्किक RTLIL::SigSpec::known_driver() स्थिरांक
{
	साठी (स्वतः &chunk : chunks())
		जर (chunk.is_wire() && !chunk.wire->known_driver())
			परत असत्य;
	परत सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_const() स्थिरांक
{
	साठी (स्वतः &chunk : chunks())
		जर (chunk.width > 0 && chunk.wire != रिक्त)
			परत असत्य;
	परत सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_zero() स्थिरांक
{
	साठी (स्वतः &chunk : chunks()) {
		जर (chunk.width > 0 && chunk.wire != रिक्त)
			परत असत्य;
		साठी (RTLIL::State d : chunk.data)
			जर (d != RTLIL::State::S0)
				परत असत्य;
	}
	परत सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_ones() स्थिरांक
{
	साठी (स्वतः &chunk : chunks()) {
		जर (chunk.width > 0 && chunk.wire != रिक्त)
			परत असत्य;
		साठी (RTLIL::State d : chunk.data)
			जर (d != RTLIL::State::S1)
				परत असत्य;
	}
	परत सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_def() स्थिरांक
{
	साठी (स्वतः &chunk : chunks()) {
		जर (chunk.width > 0 && chunk.wire != रिक्त)
			परत असत्य;
		साठी (RTLIL::State d : chunk.data)
			जर (d != RTLIL::State::S0 && d != RTLIL::State::S1)
				परत असत्य;
	}
	परत सत्य;
}

तार्किक RTLIL::SigSpec::is_fully_undef() स्थिरांक
{
	साठी (स्वतः &chunk : chunks()) {
		जर (chunk.width > 0 && chunk.wire != रिक्त)
			परत असत्य;
		साठी (RTLIL::State d : chunk.data)
			जर (d != RTLIL::State::Sx && d != RTLIL::State::Sz)
				परत असत्य;
	}
	परत सत्य;
}

तार्किक RTLIL::SigSpec::has_const() स्थिरांक
{
	साठी (स्वतः &chunk : chunks())
		जर (chunk.width > 0 && chunk.wire == रिक्त)
			परत सत्य;
	परत असत्य;
}

तार्किक RTLIL::SigSpec::has_const(State state) स्थिरांक
{
	साठी (स्वतः &chunk : chunks())
		जर (chunk.width > 0 && chunk.wire == रिक्त && std::find(chunk.data.begin(), chunk.data.end(), state) != chunk.data.end())
			परत सत्य;
	परत असत्य;
}


तार्किक RTLIL::SigSpec::has_marked_bits() स्थिरांक
{
	साठी (स्वतः &chunk : chunks())
		जर (chunk.width > 0 && chunk.wire == रिक्त) {
			साठी (RTLIL::State d : chunk.data)
				जर (d == RTLIL::State::Sm)
					परत सत्य;
		}
	परत असत्य;
}

तार्किक RTLIL::SigSpec::is_onehot(पूर्णांक *pos) स्थिरांक
{
	जर (std::optional<RTLIL::Const> c = try_as_const())
		परत c->is_onehot(pos);
	परत असत्य;
}

तार्किक RTLIL::SigSpec::as_bool() स्थिरांक
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मूल्य());
	परत c->as_bool();
}

पूर्णांक RTLIL::SigSpec::as_int(तार्किक is_signed) स्थिरांक
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मूल्य());
	परत c->as_int(is_signed);
}

तार्किक RTLIL::SigSpec::convertible_to_int(तार्किक is_signed) स्थिरांक
{
	std::optional<RTLIL::Const> c = try_as_const();
	जर (!c.has_मूल्य())
		परत असत्य;
	परत c->convertible_to_int(is_signed);
}

std::optional<पूर्णांक> RTLIL::SigSpec::try_as_int(तार्किक is_signed) स्थिरांक
{
	std::optional<RTLIL::Const> c = try_as_const();
	जर (!c.has_मूल्य())
		परत std::nullopt;
	परत c->try_as_int(is_signed);
}

पूर्णांक RTLIL::SigSpec::as_int_saturating(तार्किक is_signed) स्थिरांक
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मूल्य());
	परत c->as_int_saturating(is_signed);
}

std::string RTLIL::SigSpec::as_string() स्थिरांक
{
	std::string str;
	str.reserve(size());
	std::vector<RTLIL::SigChunk> chunks = *हे;
	साठी (माप_प्रकार i = chunks.size(); i > 0; i--) {
		स्थिरांक RTLIL::SigChunk &chunk = chunks[i-1];
		जर (chunk.wire != रिक्त)
			str.append(chunk.width, '?');
		अन्यथा
			str += RTLIL::Const(chunk.data).as_string();
	}
	परत str;
}

std::optional<RTLIL::Const> RTLIL::SigSpec::try_as_const() स्थिरांक
{
	Chunks cs = chunks();
	स्वतः it = cs.begin();
	जर (it == cs.end())
		परत RTLIL::Const();
	SigChunk chunk = *it;
	जर (chunk.wire != रिक्त || ++it != cs.end())
		परत std::nullopt;
	परत RTLIL::Const(std::move(chunk.data));
}

RTLIL::Const RTLIL::SigSpec::as_const() स्थिरांक
{
	std::optional<RTLIL::Const> c = try_as_const();
	log_assert(c.has_मूल्य());
	परत *c;
}

RTLIL::Wire *RTLIL::SigSpec::as_wire() स्थिरांक
{
	Chunks cs = chunks();
	स्वतः it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end() && chunk.wire && chunk.wire->width == size());
	परत chunk.wire;
}

RTLIL::SigChunk RTLIL::SigSpec::as_chunk() स्थिरांक
{
	Chunks cs = chunks();
	स्वतः it = cs.begin();
	log_assert(it != cs.end());
	RTLIL::SigChunk chunk = *it;
	log_assert(++it == cs.end());
	परत chunk;
}

RTLIL::SigBit RTLIL::SigSpec::as_bit() स्थिरांक
{
	परत RTLIL::SigBit(*हे);
}

तार्किक RTLIL::SigSpec::match(स्थिरांक अक्षर* pattern) स्थिरांक
{
	पूर्णांक pattern_len = म_माप(pattern);
	log_assert(pattern_len == size());

	साठी (पूर्णांक i = 0; i < pattern_len; ++i) {
		अक्षर ch = pattern[i];
		जर (ch == ' ')
			सुरूठेवा;
		RTLIL::SigBit bit = (*हे)[pattern_len - 1 - i];
		जर (ch == '*') {
			जर (bit != State::Sz && bit != State::Sx)
				परत असत्य;
			सुरूठेवा;
		}
		जर (ch == '0') {
			जर (bit != State::S0)
				परत असत्य;
		} अन्यथा
		जर (ch == '1') {
			जर (bit != State::S1)
				परत असत्य;
		} अन्यथा
			log_abort();
	}

	परत सत्य;
}

std::set<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_set() स्थिरांक
{
	std::set<RTLIL::SigBit> sigbits;
	साठी (स्वतः &c : chunks())
		साठी (पूर्णांक i = 0; i < c.width; i++)
			sigbits.घाला(RTLIL::SigBit(c, i));
	परत sigbits;
}

pool<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_pool() स्थिरांक
{
	pool<RTLIL::SigBit> sigbits;
	sigbits.reserve(size());
	साठी (स्वतः &c : chunks())
		साठी (पूर्णांक i = 0; i < c.width; i++)
			sigbits.घाला(RTLIL::SigBit(c, i));
	परत sigbits;
}

std::vector<RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_vector() स्थिरांक
{
	std::vector<RTLIL::SigBit> result;
	result.reserve(size());
	साठी (SigBit bit : *हे)
		result.ढकला_back(bit);
	परत result;
}

std::map<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_map(स्थिरांक RTLIL::SigSpec &other) स्थिरांक
{
	पूर्णांक this_size = size();
	log_assert(this_size == other.size());

	std::map<RTLIL::SigBit, RTLIL::SigBit> new_map;
	साठी (पूर्णांक i = 0; i < this_size; i++)
		new_map[(*हे)[i]] = other[i];

	परत new_map;
}

dict<RTLIL::SigBit, RTLIL::SigBit> RTLIL::SigSpec::to_sigbit_dict(स्थिरांक RTLIL::SigSpec &other) स्थिरांक
{
	पूर्णांक this_size = size();
	log_assert(this_size == other.size());

	dict<RTLIL::SigBit, RTLIL::SigBit> new_map;
	new_map.reserve(this_size);
	साठी (पूर्णांक i = 0; i < this_size; i++)
		new_map[(*हे)[i]] = other[i];

	परत new_map;
}

स्थिर शून्य sigspec_parse_split(std::vector<std::string> &tokens, स्थिरांक std::string &text, अक्षर sep)
{
	माप_प्रकार start = 0, end = 0;
	पर्यंत ((end = text.find(sep, start)) != std::string::npos) {
		tokens.ढकला_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.ढकला_back(text.substr(start));
}

स्थिर RTLIL::Wire *sigspec_parse_wire(RTLIL::Module *module, स्थिरांक std::string &netname)
{
	परत module->wire(netname);
}

तार्किक RTLIL::SigSpec::parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	std::vector<std::string> tokens;
	sigspec_parse_split(tokens, str, ',');

	sig = RTLIL::SigSpec();
	साठी (पूर्णांक tokidx = पूर्णांक(tokens.size())-1; tokidx >= 0; tokidx--)
	{
		std::string netname = tokens[tokidx];
		std::string indices;

		जर (netname.size() == 0)
			सुरूठेवा;

		जर (('0' <= netname[0] && netname[0] <= '9') || netname[0] == '\'') {
			VERILOG_FRONTEND::ConstParser p{Location()};
			स्वतः ast = p.const2ast(netname);
			जर (ast == nullptr)
				परत असत्य;
			sig.append(RTLIL::Const(ast->bits));
			सुरूठेवा;
		}

		जर (module == रिक्त)
			परत असत्य;

		जर (netname[0] != '$' && netname[0] != '\\')
			netname = "\\" + netname;

		जर (sigspec_parse_wire(module, netname) == nullptr) {
			माप_प्रकार indices_pos = netname.size()-1;
			जर (indices_pos > 2 && netname[indices_pos] == ']')
			{
				indices_pos--;
				पर्यंत (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				जर (indices_pos > 0 && netname[indices_pos] == ':') {
					indices_pos--;
					पर्यंत (indices_pos > 0 && ('0' <= netname[indices_pos] && netname[indices_pos] <= '9')) indices_pos--;
				}
				जर (indices_pos > 0 && netname[indices_pos] == '[') {
					indices = netname.substr(indices_pos);
					netname = netname.substr(0, indices_pos);
				}
			}
		}

		RTLIL::Wire *wire = sigspec_parse_wire(module, netname);
		जर (wire == nullptr)
			परत असत्य;

		जर (!indices.empty()) {
			std::vector<std::string> index_tokens;
			sigspec_parse_split(index_tokens, indices.substr(1, indices.size()-2), ':');
			जर (index_tokens.size() == 1) {
				पूर्णांक a = म_से_प(index_tokens.at(0).c_str());
				जर (a < 0 || a >= wire->width)
					परत असत्य;
				sig.append(RTLIL::SigSpec(wire, a));
			} अन्यथा {
				पूर्णांक a = म_से_प(index_tokens.at(0).c_str());
				पूर्णांक b = म_से_प(index_tokens.at(1).c_str());
				जर (a > b) {
					पूर्णांक tmp = a;
					a = b, b = tmp;
				}
				जर (a < 0 || a >= wire->width)
					परत असत्य;
				जर (b < 0 || b >= wire->width)
					परत असत्य;
				sig.append(RTLIL::SigSpec(wire, a, b-a+1));
			}
		} अन्यथा
			sig.append(wire);
	}

	परत सत्य;
}

तार्किक RTLIL::SigSpec::parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str)
{
	जर (str.empty() || str[0] != '@')
		परत parse(sig, module, str);

	IdString sel_name = design->twines().find(RTLIL::escape_id(str.substr(1)));
	जर (sel_name == IdString::Null || design->selection_vars.मोजणी(sel_name) == 0)
		परत असत्य;

	sig = RTLIL::SigSpec();
	RTLIL::Selection &sel = design->selection_vars.at(sel_name);
	साठी (स्वतः &it : module->wires_)
		जर (sel.selected_member(module->name, it.second->name))
			sig.append(it.second);

	परत सत्य;
}

तार्किक RTLIL::SigSpec::parse_rhs(स्थिरांक RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str)
{
	जर (str == "0") {
		sig = RTLIL::SigSpec(RTLIL::State::S0, lhs.size());
		परत सत्य;
	}

	जर (str == "~0") {
		sig = RTLIL::SigSpec(RTLIL::State::S1, lhs.size());
		परत सत्य;
	}

	जर (lhs.is_chunk()) {
		अक्षर *p = (अक्षर*)str.c_str(), *endptr;
		दीर्घ पूर्णांक val = म_से_दीर्घ(p, &endptr, 10);
		जर (endptr && endptr != p && *endptr == 0) {
			sig = RTLIL::SigSpec(val, lhs.size());
			परत सत्य;
		}
	}

	जर (!parse(sig, module, str))
		परत असत्य;
	जर (sig.size() > lhs.size())
		sig.हटाओ(lhs.size(), sig.size() - lhs.size());
	परत सत्य;
}

RTLIL::SigSpec::चालक std::vector<RTLIL::SigChunk>() स्थिरांक
{
	std::vector<RTLIL::SigChunk> result;
	साठी (स्थिरांक RTLIL::SigChunk &c : chunks())
		result.ढकला_back(c);
	परत result;
}

RTLIL::CaseRule::~CaseRule()
{
	साठी (स्वतः it = switches.begin(); it != switches.end(); it++)
		delete *it;
}

तार्किक RTLIL::CaseRule::empty() स्थिरांक
{
	परत actions.empty() && switches.empty();
}

RTLIL::CaseRule *RTLIL::CaseRule::clone() स्थिरांक
{
	RTLIL::CaseRule *new_caserule = नवीन RTLIL::CaseRule;
	new_caserule->compare = compare;
	new_caserule->actions = actions;
	new_caserule->attributes = attributes;
	साठी (स्वतः &it : switches)
		new_caserule->switches.ढकला_back(it->clone());
	परत new_caserule;
}

RTLIL::SwitchRule::~SwitchRule()
{
	साठी (स्वतः it = cases.begin(); it != cases.end(); it++)
		delete *it;
}

तार्किक RTLIL::SwitchRule::empty() स्थिरांक
{
	परत cases.empty();
}

RTLIL::SwitchRule *RTLIL::SwitchRule::clone() स्थिरांक
{
	RTLIL::SwitchRule *new_switchrule = नवीन RTLIL::SwitchRule;
	new_switchrule->संकेत = संकेत;
	new_switchrule->attributes = attributes;
	साठी (स्वतः &it : cases)
		new_switchrule->cases.ढकला_back(it->clone());
	परत new_switchrule;

}

RTLIL::SyncRule *RTLIL::SyncRule::clone() स्थिरांक
{
	RTLIL::SyncRule *new_syncrule = नवीन RTLIL::SyncRule;
	new_syncrule->type = type;
	new_syncrule->संकेत = संकेत;
	new_syncrule->actions = actions;
	new_syncrule->mem_write_actions = mem_write_actions;
	परत new_syncrule;
}

RTLIL::Process::~Process()
{
	साठी (स्वतः it = syncs.begin(); it != syncs.end(); it++)
		delete *it;
}

RTLIL::Process *RTLIL::Process::clone() स्थिरांक
{
	RTLIL::Process *new_proc = नवीन RTLIL::Process;

	new_proc->attributes = attributes;

	RTLIL::CaseRule *rc_ptr = मूळ_case.clone();
	new_proc->मूळ_case = *rc_ptr;
	rc_ptr->switches.clear();
	delete rc_ptr;

	साठी (स्वतः &it : syncs)
		new_proc->syncs.ढकला_back(it->clone());

	परत new_proc;
}

RTLIL::Memory::~Memory()
{
#ifdef YOSYS_ENABLE_PYTHON
	RTLIL::Memory::get_all_memorys()->erase(hashidx_);
#endif
}

#ifdef YOSYS_ENABLE_PYTHON
स्थिर std::map<अचिन्हित पूर्णांक, RTLIL::Memory*> all_memorys;
std::map<अचिन्हित पूर्णांक, RTLIL::Memory*> *RTLIL::Memory::get_all_memorys(शून्य)
{
	परत &all_memorys;
}
#endif
YOSYS_NAMESPACE_END
