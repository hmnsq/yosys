/* -*- c++ -*-
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

#ifndef RTLIL_H
#define RTLIL_H

#અન્તર્ભાવ "kernel/yosys_common.h"
#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/twine.h"

#અન્તર્ભાવ <deque>
#અન્તર્ભાવ <string_view>
#અન્તર્ભાવ <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	ગણના State : unsigned અક્ષર {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	ગણના SyncType : unsigned અક્ષર {
		ST0 = 0, // level sensitive: 0
		ST1 = 1, // level sensitive: 1
		STp = 2, // edge sensitive: posedge
		STn = 3, // edge sensitive: negedge
		STe = 4, // edge sensitive: both edges
		STa = 5, // always active
		STg = 6, // global clock
		STi = 7  // init
	};

	// Semantic metadata - how can this constant be interpreted?
	// Values may be generally non-exclusive
	ગણના ConstFlags : unsigned અક્ષર {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	ગણના SelectPartials : unsigned અક્ષર {
		SELECT_ALL = 0,          // include partial modules
		SELECT_WHOLE_ONLY = 1,   // ignore partial modules
		SELECT_WHOLE_WARN = 2,   // call log_warning on partial module
		SELECT_WHOLE_ERR = 3,    // call log_error on partial module
		SELECT_WHOLE_CMDERR = 4  // call log_cmd_error on partial module
	};

	ગણના SelectBoxes : unsigned અક્ષર {
		SB_ALL = 0,            // include boxed modules
		SB_WARN = 1,           // helper for log_warning (not for direct use)
		SB_ERR = 2,            // helper for log_error (not for direct use)
		SB_CMDERR = 3,         // helper for log_cmd_error (not for direct use)
		SB_UNBOXED_ONLY = 4,   // ignore boxed modules
		SB_UNBOXED_WARN = 5,   // call log_warning on boxed module
		SB_UNBOXED_ERR = 6,    // call log_error on boxed module
		SB_UNBOXED_CMDERR = 7, // call log_cmd_error on boxed module
		SB_INCL_WB = 8,        // helper for white boxes (not for direct use)
		SB_EXCL_BB_ONLY = 12,  // ignore black boxes, but not white boxes
		SB_EXCL_BB_WARN = 13,  // call log_warning on black boxed module
		SB_EXCL_BB_ERR = 14,   // call log_error on black boxed module
		SB_EXCL_BB_CMDERR = 15 // call log_cmd_error on black boxed module
	};

	ગણના PortDir : unsigned અક્ષર  {
		PD_UNKNOWN = 0,
		PD_આગત = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr પૂર્ણાંક WIDTH_LIMIT = 1 << 30;

	સંરચના Const;
	સંરચના AttrObject;
	સંરચના SelectedMember;
	સંરચના Selection;
	સંરચના Monitor;
	સંરચના Design;
	સંરચના Module;
	સંરચના Wire;
	સંરચના Memory;
	સંરચના Cell;
	સંરચના SigChunk;
	સંરચના SigBit;
	સંરચના SigSpecIterator;
	સંરચના SigSpecConstIterator;
	સંરચના SigSpec;
	સંરચના CaseRule;
	સંરચના SwitchRule;
	સંરચના MemWriteAction;
	સંરચના SyncRule;
	સંરચના Process;

	પ્રકારવ્યાખ્યા std::pair<SigSpec, SigSpec> SigSig;
};

સંરચના SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_PREFIX ID; }
namespace RTLIL { using YOSYS_NAMESPACE_PREFIX IdString; }

namespace RTLIL {
	શૂન્ય copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			અચલ dict<IdString, RTLIL::Const> &src,
			અચલ RTLIL::Design *src_design, RTLIL::Design *dst_design);

	બાહ્ય dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	અચલ pool<IdString> &builtin_ff_cell_types();

	સ્થિર અંતઃરેખ std::string escape_id(અચલ std::string &str) {
		જો (str.size() > 0 && str[0] != '\\' && str[0] != '$')
			પાછા "\\" + str;
		પાછા str;
	}

	સ્થિર અંતઃરેખ std::string unescape_id(અચલ std::string &str) {
		જો (str.size() < 2)
			પાછા str;
		જો (str[0] != '\\')
			પાછા str;
		જો (str[1] == '$' || str[1] == '\\')
			પાછા str;
		જો (str[1] >= '0' && str[1] <= '9')
			પાછા str;
		પાછા str.substr(1);
	}

	template <typename T> using sort_by_name_id = IdString::compare_ptr_by_name<T>;

	template <typename T> સંરચના sort_by_name_str {
		bool operator()(T *a, T *b) અચલ {
			પાછા a->name.lt_by_name(b->name);
		}
	};

	સંરચના sort_by_id_str {
		અચલ TwinePool& pool;
		explicit sort_by_id_str(અચલ TwinePool& pool) : pool(pool) {}
		bool operator()(RTLIL::IdString a, RTLIL::IdString b) અચલ {
			પાછા pool.compare_by_name(a, b) < 0;
		}
	};

	સ્થિર અંતઃરેખ std::string encode_filename(અચલ std::string &filename)
	{
		std::stringstream val;
		જો (!std::any_of(filename.begin(), filename.end(), [](અક્ષર c) {
			પાછા static_cast<unsigned અક્ષર>(c) < 33 || static_cast<unsigned અક્ષર>(c) > 126;
		})) પાછા filename;
		માટે (unsigned અક્ષર અચલ c : filename) {
			જો (c < 33 || c > 126)
				val << stringf("$%02x", c);
			નહીંતર
				val << c;
		}
		પાછા val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_and         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_or          (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_xor         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_xnor        (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);

	RTLIL::Const const_reduce_and  (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_reduce_or   (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_reduce_xor  (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_reduce_xnor (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_reduce_bool (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);

	RTLIL::Const const_logic_not   (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_logic_and   (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_logic_or    (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);

	RTLIL::Const const_shl         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_shr         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_sshl        (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_sshr        (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_shift       (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_shiftx      (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);

	RTLIL::Const const_lt          (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_le          (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_eq          (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_ne          (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_eqx         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_nex         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_ge          (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_gt          (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);

	RTLIL::Const const_add         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_sub         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_mul         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_div         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_divfloor    (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_modfloor    (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_mod         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_pow         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);

	RTLIL::Const const_pos         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_buf         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);
	RTLIL::Const const_neg         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, bool signed1, bool signed2, પૂર્ણાંક result_len);

	RTLIL::Const const_mux         (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, અચલ RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, અચલ RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2);
	RTLIL::Const const_demux       (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, અચલ RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	template<typename T, typename ચાવી_2 = IdString>
	સંરચના ObjIterator {
		using iterator_category = std::forward_iterator_tag;
		using કિંમત_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		typename dict<ચાવી_2, T>::iterator it;
		dict<ચાવી_2, T> *list_p;
		પૂર્ણાંક *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, પૂર્ણાંક *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			જો (list_p->empty()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} નહીંતર {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(અચલ RTLIL::ObjIterator<T, ચાવી_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			જો (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &operator=(અચલ RTLIL::ObjIterator<T, ચાવી_2> &other) {
			જો (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			જો (refcount_p)
				(*refcount_p)++;
			પાછા *this;
		}

		~ObjIterator() {
			જો (refcount_p)
				(*refcount_p)--;
		}

		અંતઃરેખ T operator*() અચલ {
			log_assert(list_p != nullptr);
			પાછા it->second;
		}

		અંતઃરેખ bool operator!=(અચલ RTLIL::ObjIterator<T, ચાવી_2> &other) અચલ {
			જો (list_p == nullptr || other.list_p == nullptr)
				પાછા list_p != other.list_p;
			પાછા it != other.it;
		}


		અંતઃરેખ bool operator==(અચલ RTLIL::ObjIterator<T, ચાવી_2> &other) અચલ {
			પાછા !(*this != other);
		}

		અંતઃરેખ ObjIterator<T, ચાવી_2>& operator++() {
			log_assert(list_p != nullptr);
			જો (++it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			પાછા *this;
		}

		અંતઃરેખ ObjIterator<T, ચાવી_2>& operator+=(પૂર્ણાંક amt) {
			log_assert(list_p != nullptr);
			it += amt;
			જો (it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			પાછા *this;
		}

		અંતઃરેખ ObjIterator<T, ચાવી_2> operator+(પૂર્ણાંક amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, ચાવી_2> new_obj(*this);
			new_obj.it += amt;
			જો (new_obj.it == list_p->end()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			પાછા new_obj;
		}

		અંતઃરેખ અચલ ObjIterator<T, ચાવી_2> operator++(પૂર્ણાંક) {
			ObjIterator<T, ચાવી_2> result(*this);
			++(*this);
			પાછા result;
		}
	};

	template<typename T, typename ચાવી_2 = IdString>
	સંરચના ObjRange
	{
		dict<ચાવી_2, T> *list_p;
		પૂર્ણાંક *refcount_p;

		ObjRange(decltype(list_p) list_p, પૂર્ણાંક *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, ચાવી_2> begin() { પાછા RTLIL::ObjIterator<T, ચાવી_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, ચાવી_2> end() { પાછા RTLIL::ObjIterator<T, ચાવી_2>(); }

		size_t size() અચલ {
			પાછા list_p->size();
		}

		operator pool<T>() અચલ {
			pool<T> result;
			માટે (auto &it : *list_p)
				result.ઉમેરો(it.second);
			પાછા result;
		}

		operator std::vector<T>() અચલ {
			std::vector<T> result;
			result.reserve(list_p->size());
			માટે (auto &it : *list_p)
				result.ધકેલો_back(it.second);
			પાછા result;
		}

		pool<T> to_pool() અચલ { પાછા *this; }
		std::vector<T> to_vector() અચલ { પાછા *this; }
	};
};

સંરચના RTLIL::Const
{
	short પૂર્ણાંક flags;
ખાનગી:
	friend class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::vector<RTLIL::State>;
	ગણના class backing_tag: bool { bits, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_tag tag;
	સંઘ {
		bitvectype bits_;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_bits() અચલ { પાછા tag == backing_tag::bits; }
	bool is_str() અચલ { પાછા tag == backing_tag::string; }

	bitvectype* get_if_bits() { પાછા is_bits() ? &bits_ : ખાલી; }
	std::string* get_if_str() { પાછા is_str() ? &str_ : ખાલી; }
	અચલ bitvectype* get_if_bits() અચલ { પાછા is_bits() ? &bits_ : ખાલી; }
	અચલ std::string* get_if_str() અચલ { પાછા is_str() ? &str_ : ખાલી; }

	bitvectype& get_bits();
	std::string& get_str();
	અચલ bitvectype& get_bits() અચલ;
	અચલ std::string& get_str() અચલ;
	std::vector<RTLIL::State>& bits_internal();
	શૂન્ય bitvectorize_internal();

જાહેર:
	Const() : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::vector<RTLIL::State>()) {}
	Const(std::string str);
	Const(long long val); // default width is 32
	Const(long long val, પૂર્ણાંક width);
	Const(RTLIL::State bit, પૂર્ણાંક width = 1);
	Const(std::vector<RTLIL::State> bits) : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::move(bits)) {}
	Const(અચલ std::vector<bool> &bits);
	Const(અચલ RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &operator =(અચલ RTLIL::Const &other);
	~Const();

	સંરચના Builder
	{
		Builder() {}
		Builder(પૂર્ણાંક expected_width) { bits.reserve(expected_width); }
		શૂન્ય ધકેલો_back(RTLIL::State b) { bits.ધકેલો_back(b); }
		પૂર્ણાંક size() અચલ { પાછા static_cast<પૂર્ણાંક>(bits.size()); }
		Const build() { પાછા Const(std::move(bits)); }
	ખાનગી:
		std::vector<RTLIL::State> bits;
	};

	bool operator <(અચલ RTLIL::Const &other) અચલ;
	bool operator ==(અચલ RTLIL::Const &other) અચલ;
	bool operator !=(અચલ RTLIL::Const &other) અચલ;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::vector<RTLIL::State>& bits() { પાછા bits_internal(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	શૂન્ય bitvectorize() અચલ { const_cast<Const*>(this)->bitvectorize_internal(); }

	bool as_bool() અચલ;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	પૂર્ણાંક as_int(bool is_signed = false) અચલ;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_to_int(bool is_signed = false) અચલ;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<પૂર્ણાંક> try_as_int(bool is_signed = false) અચલ;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	પૂર્ણાંક as_int_saturating(bool is_signed = false) અચલ;

	શૂન્ય tag_bare_integer_const(અચલ std::string &કિંમત);

	std::string as_string(અચલ અક્ષર* any = "-") અચલ;
	સ્થિર Const from_string(અચલ std::string &str);
	std::vector<RTLIL::State> to_bits() અચલ;

	std::string decode_string() અચલ;
	પૂર્ણાંક size() અચલ;
	bool empty() અચલ;

	શૂન્ય append(અચલ RTLIL::Const &other);
	શૂન્ય set(પૂર્ણાંક i, RTLIL::State state) {
		bits_internal()[i] = state;
	}
	શૂન્ય resize(પૂર્ણાંક size, RTLIL::State fill) {
    log_assert(size >= 0 && size < RTLIL::WIDTH_LIMIT);
		bits_internal().resize(size, fill);
	}

	class const_iterator {
	ખાનગી:
		અચલ Const* parent;
		size_t idx;

	જાહેર:
		using iterator_category = std::bidirectional_iterator_tag;
		using કિંમત_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = અચલ State*;
		using reference = અચલ State&;

		const_iterator(અચલ Const& c, size_t i) : parent(&c), idx(i) {}

		State operator*() અચલ;

		const_iterator& operator++() { ++idx; પાછા *this; }
		const_iterator& operator--() { --idx; પાછા *this; }
		const_iterator operator++(પૂર્ણાંક) { const_iterator result(*this); ++idx; પાછા result; }
		const_iterator operator--(પૂર્ણાંક) { const_iterator result(*this); --idx; પાછા result; }
		const_iterator& operator+=(પૂર્ણાંક i) { idx += i; પાછા *this; }

		const_iterator operator+(પૂર્ણાંક add) {
			પાછા const_iterator(*parent, idx + add);
		}
		const_iterator operator-(પૂર્ણાંક sub) {
			પાછા const_iterator(*parent, idx - sub);
		}
		પૂર્ણાંક operator-(અચલ const_iterator& other) {
			પાછા idx - other.idx;
		}

		bool operator==(અચલ const_iterator& other) અચલ {
			પાછા idx == other.idx;
		}

		bool operator!=(અચલ const_iterator& other) અચલ {
			પાછા !(*this == other);
		}
	};

	class iterator {
	ખાનગી:
		Const* parent;
		size_t idx;

	જાહેર:
		class proxy {
		ખાનગી:
			Const* parent;
			size_t idx;
		જાહેર:
			proxy(Const* parent, size_t idx) : parent(parent), idx(idx) {}
			operator State() અચલ { પાછા (*parent)[idx]; }
			proxy& operator=(State s) { parent->set(idx, s); પાછા *this; }
			proxy& operator=(અચલ proxy& other) { parent->set(idx, (*other.parent)[other.idx]); પાછા *this; }
		};

		using iterator_category = std::bidirectional_iterator_tag;
		using કિંમત_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = proxy*;
		using reference = proxy;

		iterator(Const& c, size_t i) : parent(&c), idx(i) {}

		proxy operator*() અચલ { પાછા proxy(parent, idx); }
		iterator& operator++() { ++idx; પાછા *this; }
		iterator& operator--() { --idx; પાછા *this; }
		iterator operator++(પૂર્ણાંક) { iterator result(*this); ++idx; પાછા result; }
		iterator operator--(પૂર્ણાંક) { iterator result(*this); --idx; પાછા result; }
		iterator& operator+=(પૂર્ણાંક i) { idx += i; પાછા *this; }

		iterator operator+(પૂર્ણાંક add) {
			પાછા iterator(*parent, idx + add);
		}
		iterator operator-(પૂર્ણાંક sub) {
			પાછા iterator(*parent, idx - sub);
		}
		પૂર્ણાંક operator-(અચલ iterator& other) {
			પાછા idx - other.idx;
		}

		bool operator==(અચલ iterator& other) અચલ {
			પાછા idx == other.idx;
		}

		bool operator!=(અચલ iterator& other) અચલ {
			પાછા !(*this == other);
		}
	};

	const_iterator begin() અચલ {
		પાછા const_iterator(*this, 0);
	}
	const_iterator end() અચલ {
		પાછા const_iterator(*this, size());
	}
	iterator begin() {
		પાછા iterator(*this, 0);
	}
	iterator end() {
		પાછા iterator(*this, size());
	}
	State back() અચલ {
		પાછા *(end() - 1);
	}
	State front() અચલ {
		પાછા *begin();
	}
	State at(size_t i) અચલ {
		પાછા *const_iterator(*this, i);
	}
	State operator[](size_t i) અચલ {
		પાછા *const_iterator(*this, i);
	}

	bool is_fully_zero() અચલ;
	bool is_fully_ones() અચલ;
	bool is_fully_def() અચલ;
	bool is_fully_undef() અચલ;
	bool is_fully_undef_x_only() અચલ;
	bool is_onehot(પૂર્ણાંક *pos = nullptr) અચલ;

	RTLIL::Const extract(પૂર્ણાંક offset, પૂર્ણાંક len = 1, RTLIL::State padding = RTLIL::State::S0) અચલ;

	// find the MSB without redundant leading bits
	પૂર્ણાંક get_min_size(bool is_signed) અચલ;

	// compress representation to the minimum required bits
	શૂન્ય compress(bool is_signed = false);

	std::optional<પૂર્ણાંક> as_int_compress(bool is_signed) અચલ;

	શૂન્ય extu(પૂર્ણાંક width) {
		resize(width, RTLIL::State::S0);
	}

	શૂન્ય exts(પૂર્ણાંક width) {
		resize(width, empty() ? RTLIL::State::Sx : back());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ;
};

સંરચના RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	bool has_attribute(RTLIL::IdString id) અચલ;

	શૂન્ય set_bool_attribute(RTLIL::IdString id, bool કિંમત=true);
	bool get_bool_attribute(RTLIL::IdString id) અચલ;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) અચલ {
		પાછા get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	શૂન્ય set_string_attribute(RTLIL::IdString id, string કિંમત);
	string get_string_attribute(RTLIL::IdString id) અચલ;

	શૂન્ય set_strpool_attribute(RTLIL::IdString id, અચલ pool<string> &data);
	શૂન્ય add_strpool_attribute(RTLIL::IdString id, અચલ pool<string> &data);
	pool<string> get_strpool_attribute(RTLIL::IdString id) અચલ;

	શૂન્ય set_src_attribute(અચલ std::string &src) {
		set_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() અચલ {
		પાછા get_string_attribute(ID::src);
	}

	શૂન્ય set_hdlname_attribute(અચલ vector<string> &hierarchy);
	vector<string> get_hdlname_attribute() અચલ;

	શૂન્ય set_intvec_attribute(RTLIL::IdString id, અચલ vector<પૂર્ણાંક> &data);
	vector<પૂર્ણાંક> get_intvec_attribute(RTLIL::IdString id) અચલ;
};

#અન્તર્ભાવ "kernel/rtlil_twine_compat.h"

સંરચના RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

સંરચના RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::vector<RTLIL::State> data; // only used if wire == NULL, LSB at index 0
	પૂર્ણાંક width, offset;

	SigChunk() : wire(nullptr), width(0), offset(0) {}
	SigChunk(અચલ RTLIL::Const &કિંમત) : wire(nullptr), data(કિંમત.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Const &&કિંમત) : wire(nullptr), data(કિંમત.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), width(GetSize(wire)), offset(0) {}
	SigChunk(RTLIL::Wire *wire, પૂર્ણાંક offset, પૂર્ણાંક width = 1) : wire(wire), width(width), offset(offset) {}
	SigChunk(અચલ std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(પૂર્ણાંક val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(પૂર્ણાંક val, પૂર્ણાંક width) : SigChunk(RTLIL::Const(val, width)) {}
	SigChunk(RTLIL::State bit, પૂર્ણાંક width = 1) : SigChunk(RTLIL::Const(bit, width)) {}
	SigChunk(અચલ RTLIL::SigBit &bit);

	RTLIL::SigChunk extract(પૂર્ણાંક offset, પૂર્ણાંક length) અચલ;
	RTLIL::SigBit operator[](પૂર્ણાંક offset) અચલ;
	અંતઃરેખ પૂર્ણાંક size() અચલ { પાછા width; }
	અંતઃરેખ bool is_wire() અચલ { પાછા wire != ખાલી; }

	bool operator <(અચલ RTLIL::SigChunk &other) અચલ;
	bool operator ==(અચલ RTLIL::SigChunk &other) અચલ;
	bool operator !=(અચલ RTLIL::SigChunk &other) અચલ;
};

સંરચના RTLIL::SigBit
{
	RTLIL::Wire *wire;
	સંઘ {
		RTLIL::State data; // used if wire == NULL
		પૂર્ણાંક offset;        // used if wire != NULL
	};

	SigBit();
	SigBit(RTLIL::State bit);
	explicit SigBit(bool bit);
	SigBit(RTLIL::Wire *wire);
	SigBit(RTLIL::Wire *wire, પૂર્ણાંક offset);
	SigBit(અચલ RTLIL::SigChunk &chunk);
	SigBit(અચલ RTLIL::SigChunk &chunk, પૂર્ણાંક index);
	SigBit(અચલ RTLIL::SigSpec &sig);
	SigBit(અચલ RTLIL::SigBit &sigbit) = મૂળભૂત;
	RTLIL::SigBit &operator =(અચલ RTLIL::SigBit &other) = મૂળભૂત;

	અંતઃરેખ bool is_wire() અચલ { પાછા wire != ખાલી; }

	bool operator <(અચલ RTLIL::SigBit &other) અચલ;
	bool operator ==(અચલ RTLIL::SigBit &other) અચલ;
	bool operator !=(અચલ RTLIL::SigBit &other) અચલ;
	[[nodiscard]] Hasher hash_into(Hasher h) અચલ;
	[[nodiscard]] Hasher hash_ટોચ() અચલ;
};

namespace hashlib {
	template <>
	સંરચના hash_ops<RTLIL::SigBit> {
		સ્થિર અંતઃરેખ bool cmp(અચલ RTLIL::SigBit &a, અચલ RTLIL::SigBit &b) {
			પાછા a == b;
		}
		[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash(અચલ RTLIL::SigBit sb) {
			પાછા sb.hash_ટોચ();
		}
		[[nodiscard]] સ્થિર અંતઃરેખ Hasher hash_into(અચલ RTLIL::SigBit sb, Hasher h) {
			પાછા sb.hash_into(h);
		}
	};
};

સંરચના RTLIL::SigSpecIterator
{
	પ્રકારવ્યાખ્યા std::આગત_iterator_tag iterator_category;
	પ્રકારવ્યાખ્યા RTLIL::SigBit કિંમત_type;
	પ્રકારવ્યાખ્યા ptrdiff_t difference_type;
	પ્રકારવ્યાખ્યા RTLIL::SigBit* pointer;
	પ્રકારવ્યાખ્યા RTLIL::SigBit& reference;

	RTLIL::SigSpec *sig_p;
	પૂર્ણાંક index;

	અંતઃરેખ RTLIL::SigBit &operator*() અચલ;
	અંતઃરેખ bool operator!=(અચલ RTLIL::SigSpecIterator &other) અચલ { પાછા index != other.index; }
	અંતઃરેખ bool operator==(અચલ RTLIL::SigSpecIterator &other) અચલ { પાછા index == other.index; }
	અંતઃરેખ શૂન્ય operator++() { index++; }
};

સંરચના RTLIL::SigSpecConstIterator
{
	પ્રકારવ્યાખ્યા std::આગત_iterator_tag iterator_category;
	પ્રકારવ્યાખ્યા RTLIL::SigBit કિંમત_type;
	પ્રકારવ્યાખ્યા ptrdiff_t difference_type;
	પ્રકારવ્યાખ્યા RTLIL::SigBit* pointer;
	પ્રકારવ્યાખ્યા RTLIL::SigBit& reference;

	અચલ RTLIL::SigSpec *sig_p;
	RTLIL::SigBit bit;
	પૂર્ણાંક index;

	અંતઃરેખ અચલ RTLIL::SigBit &operator*();
	અંતઃરેખ bool operator!=(અચલ RTLIL::SigSpecConstIterator &other) અચલ { પાછા index != other.index; }
	અંતઃરેખ bool operator==(અચલ RTLIL::SigSpecIterator &other) અચલ { પાછા index == other.index; }
	અંતઃરેખ શૂન્ય operator++() { index++; }
};

સંરચના RTLIL::SigSpec
{
ખાનગી:
	friend class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, Extract);
	ગણના Representation : અક્ષર {
		CHUNK,
		BITS,
	};
	// An AtomicHash is either clear or a nonzero integer.
	સંરચના AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(અચલ AtomicHash &rhs) : atomic_(rhs.load()) {}
		AtomicHash &operator=(અચલ AtomicHash &rhs) { store(rhs.load()); પાછા *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> પઠ() અચલ {
			Hasher::hash_t કિંમત = load();
			જો (કિંમત == 0)
				પાછા std::nullopt;
			પાછા કિંમત;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		શૂન્ય set(Hasher::hash_t કિંમત) અચલ {
			log_assert(કિંમત != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(કિંમત, std::memory_order_relaxed);
			log_assert(old == 0 || old == કિંમત);
		}
		શૂન્ય clear() { store(0); }
	ખાનગી:
		પૂર્ણાંક load() અચલ { પાછા atomic_.load(std::memory_order_relaxed); }
		શૂન્ય store(Hasher::hash_t કિંમત) અચલ {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(કિંમત, std::memory_order_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	સંઘ {
		RTLIL::SigChunk chunk_;
		std::vector<RTLIL::SigBit> bits_; // LSB at index 0
	};

	શૂન્ય init_empty_bits() {
		rep_ = BITS;
		new (&bits_) std::vector<RTLIL::SigBit>;
	}

	શૂન્ય unpack();
	અંતઃરેખ શૂન્ય inline_unpack() {
		જો (rep_ == CHUNK)
			unpack();
	}
	શૂન્ય try_repack();

	Hasher::hash_t updhash() અચલ;
	શૂન્ય destroy() {
		જો (rep_ == CHUNK)
			chunk_.~SigChunk();
		નહીંતર
			bits_.~vector();
	}
	friend સંરચના Chunks;

જાહેર:
	SigSpec() { init_empty_bits(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(અચલ SigSpec &કિંમત) : rep_(કિંમત.rep_), hash_(કિંમત.hash_) {
		જો (કિંમત.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(કિંમત.chunk_);
		નહીંતર
			new (&bits_) std::vector<RTLIL::SigBit>(કિંમત.bits_);
	}
	SigSpec(SigSpec &&કિંમત) : rep_(કિંમત.rep_), hash_(કિંમત.hash_) {
		જો (કિંમત.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(કિંમત.chunk_));
		નહીંતર
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(કિંમત.bits_));
	}
	SigSpec(અચલ RTLIL::Const &કિંમત);
	SigSpec(RTLIL::Const &&કિંમત);
	SigSpec(અચલ RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, પૂર્ણાંક offset, પૂર્ણાંક width = 1);
	SigSpec(અચલ std::string &str);
	SigSpec(પૂર્ણાંક val, પૂર્ણાંક width = 32);
	SigSpec(RTLIL::State bit, પૂર્ણાંક width = 1);
	SigSpec(અચલ RTLIL::SigBit &bit, પૂર્ણાંક width = 1);
	SigSpec(અચલ std::vector<RTLIL::SigChunk> &chunks);
	SigSpec(અચલ std::vector<RTLIL::SigBit> &bits);
	SigSpec(અચલ pool<RTLIL::SigBit> &bits);
	SigSpec(અચલ std::set<RTLIL::SigBit> &bits);
	explicit SigSpec(bool bit);
	~SigSpec() {
		destroy();
	}

	SigSpec &operator=(અચલ SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		જો (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		નહીંતર
			new (&bits_) std::vector<RTLIL::SigBit>(rhs.bits_);
		પાછા *this;
	}
	SigSpec &operator=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		જો (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		નહીંતર
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(rhs.bits_));
		પાછા *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	સંરચના Chunks {
		Chunks(અચલ SigSpec &spec) : spec(spec) {}
		સંરચના const_iterator {
			using iterator_category = std::forward_iterator_tag;
			using કિંમત_type = અચલ SigChunk &;
			using difference_type = std::ptrdiff_t;
			using pointer = અચલ SigChunk *;
			using reference = અચલ SigChunk &;

			અચલ SigSpec &spec;
			પૂર્ણાંક bit_index;
			SigChunk chunk;

			const_iterator(અચલ SigSpec &spec) : spec(spec) {
				bit_index = 0;
				જો (spec.rep_ == BITS)
					next_chunk_bits();
			}
			ગણના End { END };
			const_iterator(અચલ SigSpec &spec, End) : spec(spec) {
				bit_index = spec.size();
			}
			શૂન્ય next_chunk_bits();

			અચલ SigChunk &operator*() {
				જો (spec.rep_ == CHUNK)
					પાછા spec.chunk_;
				પાછા chunk;
			};
			અચલ SigChunk *operator->() { પાછા &**this; }
			const_iterator &operator++() {
				bit_index += (**this).width;
				જો (spec.rep_ == BITS)
					next_chunk_bits();
				પાછા *this;
			}
			bool operator==(અચલ const_iterator &rhs) અચલ { પાછા bit_index == rhs.bit_index; }
			bool operator!=(અચલ const_iterator &rhs) અચલ { પાછા !(*this == rhs); }
		};
		const_iterator begin() અચલ { પાછા const_iterator(spec); }
		const_iterator end() અચલ {
			const_iterator it(spec, const_iterator::END);
			પાછા it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rbegin() {
			ensure_chunk_vector();
			પાછા chunk_vector.rbegin();
		}
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rend() {
			ensure_chunk_vector();
			પાછા chunk_vector.rend();
		}
		પૂર્ણાંક size() {
			ensure_chunk_vector();
			પાછા chunk_vector.size();
		}
		પૂર્ણાંક size() અચલ {
			પાછા std::distance(begin(), end());
		}
		અચલ SigChunk &at(પૂર્ણાંક index) {
			ensure_chunk_vector();
			પાછા chunk_vector.at(index);
		}
		operator અચલ std::vector<RTLIL::SigChunk>&() {
			ensure_chunk_vector();
			પાછા chunk_vector;
		}
	ખાનગી:
		શૂન્ય ensure_chunk_vector() {
			જો (spec.size() > 0 && chunk_vector.empty()) {
				માટે (અચલ RTLIL::SigChunk &c : *this)
					chunk_vector.ધકેલો_back(c);
			}
		}
		અચલ SigSpec &spec;
		std::vector<RTLIL::SigChunk> chunk_vector;
	};
	friend સંરચના Chunks::const_iterator;

	અંતઃરેખ Chunks chunks() અચલ { પાછા {*this}; }
	અંતઃરેખ અચલ SigSpec &bits() અચલ { પાછા *this; }

	અંતઃરેખ પૂર્ણાંક size() અચલ { પાછા rep_ == CHUNK ? chunk_.width : GetSize(bits_); }
	અંતઃરેખ bool empty() અચલ { પાછા size() == 0; };

	અંતઃરેખ RTLIL::SigBit &operator[](પૂર્ણાંક index) { inline_unpack(); hash_.clear(); પાછા bits_.at(index); }
	અંતઃરેખ RTLIL::SigBit operator[](પૂર્ણાંક index) અચલ {
		જો (rep_ == CHUNK) {
			જો (index < 0 || index >= chunk_.width)
				throw std::out_of_range("SigSpec::operator[]");
			જો (chunk_.wire)
				પાછા RTLIL::SigBit(chunk_.wire, chunk_.offset + index);
			પાછા RTLIL::SigBit(chunk_.data[index]);
		}
		પાછા bits_.at(index);
	}

	અંતઃરેખ RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = 0; પાછા it; }
	અંતઃરેખ RTLIL::SigSpecIterator end() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = size(); પાછા it; }

	અંતઃરેખ RTLIL::SigSpecConstIterator begin() અચલ { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = 0; પાછા it; }
	અંતઃરેખ RTLIL::SigSpecConstIterator end() અચલ { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = size(); પાછા it; }

	શૂન્ય sort();
	શૂન્ય sort_and_unify();

	શૂન્ય replace(અચલ RTLIL::SigSpec &pattern, અચલ RTLIL::SigSpec &with);
	શૂન્ય replace(અચલ RTLIL::SigSpec &pattern, અચલ RTLIL::SigSpec &with, RTLIL::SigSpec *other) અચલ;

	શૂન્ય replace(અચલ dict<RTLIL::SigBit, RTLIL::SigBit> &rules);
	શૂન્ય replace(અચલ dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) અચલ;

	શૂન્ય replace(અચલ std::map<RTLIL::SigBit, RTLIL::SigBit> &rules);
	શૂન્ય replace(અચલ std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) અચલ;

	શૂન્ય replace(પૂર્ણાંક offset, અચલ RTLIL::SigSpec &with);

	શૂન્ય remove(અચલ RTLIL::SigSpec &pattern);
	શૂન્ય remove(અચલ RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) અચલ;
	શૂન્ય remove2(અચલ RTLIL::SigSpec &pattern, RTLIL::SigSpec *other);

	શૂન્ય remove(અચલ pool<RTLIL::SigBit> &pattern);
	શૂન્ય remove(અચલ pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) અચલ;
	શૂન્ય remove2(અચલ pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	શૂન્ય remove2(અચલ std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	શૂન્ય remove2(અચલ pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other);

	શૂન્ય remove(પૂર્ણાંક offset, પૂર્ણાંક length = 1);
	શૂન્ય remove_const();

	RTLIL::SigSpec extract(અચલ RTLIL::SigSpec &pattern, અચલ RTLIL::SigSpec *other = ખાલી) અચલ;
	RTLIL::SigSpec extract(અચલ pool<RTLIL::SigBit> &pattern, અચલ RTLIL::SigSpec *other = ખાલી) અચલ;
	RTLIL::SigSpec extract(પૂર્ણાંક offset, પૂર્ણાંક length = 1) અચલ;
	RTLIL::SigSpec extract_end(પૂર્ણાંક offset) અચલ { પાછા extract(offset, size() - offset); }

	શૂન્ય rewrite_wires(std::function<શૂન્ય(RTLIL::Wire*& wire)> rewrite);

	RTLIL::SigBit lsb() અચલ { log_assert(size()); પાછા (*this)[0]; };
	RTLIL::SigBit msb() અચલ { log_assert(size()); પાછા (*this)[size() - 1]; };
	RTLIL::SigBit front() અચલ { પાછા (*this)[0]; }
	RTLIL::SigBit back() અચલ { પાછા (*this)[size() - 1]; }

	શૂન્ય append(અચલ RTLIL::SigSpec &સંકેત);
	અંતઃરેખ શૂન્ય append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	અંતઃરેખ શૂન્ય append(અચલ RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	અંતઃરેખ શૂન્ય append(અચલ RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	શૂન્ય append(અચલ RTLIL::SigBit &bit);
	અંતઃરેખ શૂન્ય append(RTLIL::State state) { append(RTLIL::SigBit(state)); }
	અંતઃરેખ શૂન્ય append(bool bool_) { append(RTLIL::SigBit(bool_)); }

	શૂન્ય extend_u0(પૂર્ણાંક width, bool is_signed = false);

	RTLIL::SigSpec repeat(પૂર્ણાંક અંક) અચલ;

	શૂન્ય reverse() { inline_unpack(); std::reverse(bits_.begin(), bits_.end()); }

	bool operator <(અચલ RTLIL::SigSpec &other) અચલ;
	bool operator ==(અચલ RTLIL::SigSpec &other) અચલ;
	અંતઃરેખ bool operator !=(અચલ RTLIL::SigSpec &other) અચલ { પાછા !(*this == other); }

	bool is_wire() અચલ;
	bool is_chunk() અચલ;
	અંતઃરેખ bool is_bit() અચલ { પાછા size() == 1; }

	bool known_driver() અચલ;

	bool is_fully_const() અચલ;
	bool is_fully_zero() અચલ;
	bool is_fully_ones() અચલ;
	bool is_fully_def() અચલ;
	bool is_fully_undef() અચલ;
	bool has_const() અચલ;
	bool has_const(State state) અચલ;
	bool has_marked_bits() અચલ;
	bool is_onehot(પૂર્ણાંક *pos = nullptr) અચલ;

	bool as_bool() અચલ;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	પૂર્ણાંક as_int(bool is_signed = false) અચલ;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_to_int(bool is_signed = false) અચલ;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<પૂર્ણાંક> try_as_int(bool is_signed = false) અચલ;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	પૂર્ણાંક as_int_saturating(bool is_signed = false) અચલ;

	std::string as_string() અચલ;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() અચલ;
	RTLIL::Const as_const() અચલ;
	RTLIL::Wire *as_wire() અચલ;
	RTLIL::SigChunk as_chunk() અચલ;
	RTLIL::SigBit as_bit() અચલ;

	bool match(અચલ અક્ષર* pattern) અચલ;

	std::set<RTLIL::SigBit> to_sigbit_set() અચલ;
	pool<RTLIL::SigBit> to_sigbit_pool() અચલ;
	std::vector<RTLIL::SigBit> to_sigbit_vector() અચલ;
	std::map<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_map(અચલ RTLIL::SigSpec &other) અચલ;
	dict<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_dict(અચલ RTLIL::SigSpec &other) અચલ;

	સ્થિર bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	સ્થિર bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	સ્થિર bool parse_rhs(અચલ RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	operator std::vector<RTLIL::SigChunk>() અચલ;
	operator std::vector<RTLIL::SigBit>() અચલ { પાછા to_sigbit_vector(); }
	અચલ RTLIL::SigBit &at(પૂર્ણાંક offset, અચલ RTLIL::SigBit &defval) { પાછા offset < size() ? (*this)[offset] : defval; }
	RTLIL::SigBit& at(પૂર્ણાંક offset) { પાછા (*this)[offset]; }
	RTLIL::SigBit at(પૂર્ણાંક offset) અચલ { પાછા (*this)[offset]; }

	[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
		Hasher::hash_t val;
		જો (std::optional<Hasher::hash_t> current = hash_.પઠ())
			val = *current;
		નહીંતર
			val = updhash();
		h.eat(val);
		પાછા h;
	}

#ifndef NDEBUG
	શૂન્ય check(અચલ Module *mod = nullptr) અચલ;
#નહીંતર
	શૂન્ય check(અચલ Module *mod = nullptr) અચલ { (શૂન્ય)mod; }
#endif
};

સંરચના RTLIL::Selection
{
	// selection includes boxed modules
	bool selects_boxes;
	// selection covers full design, including boxed modules
	bool complete_selection;
	// selection covers full design, not including boxed modules
	bool full_selection;
	pool<RTLIL::IdString> selected_modules;
	dict<RTLIL::IdString, pool<RTLIL::IdString>> selected_members;
	RTLIL::Design *current_design;

	// create a new selection
	Selection(
		// should the selection cover the full design
		bool full = true,
		// should the selection include boxed modules
		bool boxes = false,
		// the design to select from
		RTLIL::Design *design = nullptr
	) :
		selects_boxes(boxes), complete_selection(full && boxes), full_selection(full && !boxes), current_design(design) { }

	// checks if the given module exists in the current design and is a
	// boxed module, warning the user if the current design is not set
	bool boxed_module(RTLIL::IdString mod_name) અચલ;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) અચલ;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) અચલ;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) અચલ;

	// optimizes this selection for the given design by:
	// - removing non-existent modules and members, any boxed modules and
	//   their members (if selection does not include boxes), and any
	//   partially selected modules with no selected members;
	// - marking partially selected modules as wholly selected if all
	//   members of that module are selected; and
	// - marking selection as a complete_selection if all modules in the
	//   given design are selected, or a full_selection if it does not
	//   include boxes.
	શૂન્ય optimize(RTLIL::Design *design);

	// checks if selection covers full design (may or may not include
	// boxed-modules)
	bool selects_all() અચલ {
		પાછા full_selection || complete_selection;
	}

	// add whole module to this selection
	template<typename T1> શૂન્ય select(T1 *module) {
		જો (!selects_all() && selected_modules.ગણતરી(module->name) == 0) {
			IdString name = module->name;
			selected_modules.ઉમેરો(name);
			selected_members.erase(name);
			જો (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	template<typename T1, typename T2> શૂન્ય select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	template<typename T1> શૂન્ય select(T1 *module, RTLIL::IdString member) {
		જો (!selects_all() && selected_modules.ગણતરી(module->name.ref()) == 0) {
			selected_members[module->name.ref()].ઉમેરો(member);
			જો (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool empty() અચલ {
		પાછા !selects_all() && selected_modules.empty() && selected_members.empty();
	}

	// clear this selection, leaving it empty
	શૂન્ય clear();

	// create a new selection which is empty
	સ્થિર Selection EmptySelection(RTLIL::Design *design = nullptr) { પાછા Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	સ્થિર Selection FullSelection(RTLIL::Design *design = nullptr) { પાછા Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	સ્થિર Selection CompleteSelection(RTLIL::Design *design = nullptr) { પાછા Selection(true, true, design); };
};

સંરચના RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) અચલ { h.eat(hashidx_); પાછા h; }

	Monitor() {
		સ્થિર unsigned પૂર્ણાંક hashidx_ગણતરી = 123456789;
		hashidx_ગણતરી = mkhash_xorshift(hashidx_ગણતરી);
		hashidx_ = hashidx_ગણતરી;
	}

	આભાસી ~Monitor() { }
	આભાસી શૂન્ય notify_module_add(RTLIL::Module*) { }
	આભાસી શૂન્ય notify_module_del(RTLIL::Module*) { }
	આભાસી શૂન્ય notify_connect(RTLIL::Cell*, RTLIL::IdString, અચલ RTLIL::SigSpec&, અચલ RTLIL::SigSpec&) { }
	આભાસી શૂન્ય notify_connect(RTLIL::Module*, અચલ RTLIL::SigSig&) { }
	આભાસી શૂન્ય notify_connect(RTLIL::Module*, અચલ std::vector<RTLIL::SigSig>&) { }
	આભાસી શૂન્ય notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
સંરચના define_map_t;

template<typename N>
અંતઃરેખ constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::Leaf> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, અચલ અક્ષર*> || std::is_same_v<std::decay_t<N>, અક્ષર*>;

#define YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, પૂર્ણાંક> = 0

template<typename N>
અંતઃરેખ constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, અચલ અક્ષર *> || std::is_same_v<std::decay_t<N>, અક્ષર *>;

#define YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, પૂર્ણાંક> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#define YS_NAME_FWD_POOL(_func, _pool) \
	template<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(N name, Rest&&... rest) \
		{ પાછા _func(_pool.add(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#define YS_NAME_FWD_2ND_POOL(_func, _pool) \
	template<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(T &&first, N name) \
		{ પાછા _func(std::forward<T>(first), _pool.add(std::move(name))); }

#define YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#define YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

સંરચના RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) અચલ { h.eat(hashidx_); પાછા h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	શૂન્ય bufNormalize(bool enable=true);

	પૂર્ણાંક refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

ખાનગી:
	TwinePool twines_;

જાહેર:
	TwinePool &twines() { પાછા twines_; }
	અચલ TwinePool &twines() અચલ { પાછા twines_; }

	શૂન્ય absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	size_t gc_twines();

	std::vector<std::unique_ptr<AST::Ast_ગાંઠ>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_defines;

	std::vector<RTLIL::Selection> selection_ઢગલો;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_active_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	અચલ RTLIL::Module *module(RTLIL::IdString name) અચલ;
	template<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ પાછા module(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> અચલ RTLIL::Module *module(N name) અચલ
		{ પાછા module(twines().find(std::string_view(name))); }
	RTLIL::Module *ટોચ_module() અચલ;

	bool has(RTLIL::IdString id) અચલ {
		twines_.check_owned(id);
		પાછા modules_.ગણતરી(id) != 0;
	}
	template<typename N, YS_NAME_STRING(N)> bool has(N name) અચલ
		{ પાછા has(twines().find(std::string_view(name))); }

	શૂન્ય add(RTLIL::Module *module);

	RTLIL::Module *addModule(RTLIL::IdString name);
	YS_NAME_FWD_POOL(addModule, twines())
	શૂન્ય remove(RTLIL::Module *module);
	શૂન્ય rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	શૂન્ય scratchpad_unset(અચલ std::string &varname);

	શૂન્ય scratchpad_set_int(અચલ std::string &varname, પૂર્ણાંક કિંમત);
	શૂન્ય scratchpad_set_bool(અચલ std::string &varname, bool કિંમત);
	શૂન્ય scratchpad_set_string(અચલ std::string &varname, std::string કિંમત);

	પૂર્ણાંક scratchpad_get_int(અચલ std::string &varname, પૂર્ણાંક default_કિંમત_2 = 0) અચલ;
	bool scratchpad_get_bool(અચલ std::string &varname, bool default_કિંમત_2 = false) અચલ;
	std::string scratchpad_get_string(અચલ std::string &varname, અચલ std::string &default_કિંમત_2 = std::string()) અચલ;

	શૂન્ય sort();
	શૂન્ય sort_modules();
	શૂન્ય check();
	શૂન્ય optimize();

	શૂન્ય clone_into(RTLIL::Design *dst) અચલ;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) અચલ;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) અચલ;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) અચલ;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) અચલ;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::Module *mod) અચલ;

	// push the given selection to the selection stack
	શૂન્ય ધકેલો_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	શૂન્ય ધકેલો_empty_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	શૂન્ય ધકેલો_full_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	શૂન્ય ધકેલો_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	શૂન્ય કાઢો_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		પાછા selection_ઢગલો.back();
	}

	// get the current selection
	અચલ RTLIL::Selection &selection() અચલ {
		પાછા selection_ઢગલો.back();
	}

	// is the current selection a full selection (no boxes)
	bool full_selection() અચલ {
		પાછા selection().full_selection;
	}

	// is the given module in the current selection
	template<typename T1> bool selected(T1 *module) અચલ {
		પાછા selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	template<typename T1, typename T2> bool selected(T1 *module, T2 *member) અચલ {
		પાછા selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	template<typename T1> શૂન્ય select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	template<typename T1, typename T2> શૂન્ય select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::vector<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) અચલ;

	// returns all selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_modules() અચલ { પાછા selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::vector<RTLIL::Module*> selected_unboxed_modules() અચલ { પાછા selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_modules_warn() અચલ { પાછા selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::vector<RTLIL::Module*> selected_whole_modules() અચલ { પાછા selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_whole_modules() અચલ { પાછા selected_modules(SELECT_WHOLE_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::vector<RTLIL::Module*> selected_whole_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) અચલ { પાછા selected_modules(SELECT_WHOLE_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules() અચલ { પાછા selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules_warn() અચલ { પાછા selected_modules(SELECT_WHOLE_WARN, SB_UNBOXED_WARN); }

	સ્થિર std::map<unsigned પૂર્ણાંક, RTLIL::Design*> *get_all_designs(શૂન્ય);

	std::string to_rtlil_str(bool only_selected = true) અચલ;
};

સંરચના RTLIL::Module : જાહેર RTLIL::AttrObject
{
	friend સંરચના RTLIL::Cell;
	friend સંરચના RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) અચલ { h.eat(hashidx_); પાછા h; }

સુરક્ષિત:
	શૂન્ય add(RTLIL::Wire *wire);
	શૂન્ય add(RTLIL::Cell *cell);
	શૂન્ય add(RTLIL::Process *process);

જાહેર:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() અચલ { પાછા design->twines(); }

	પૂર્ણાંક refcount_wires_;
	પૂર્ણાંક refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::vector<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_કિંમતો;
	dict<RTLIL::IdString, RTLIL::Memory*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	આભાસી ~Module();
	આભાસી RTLIL::IdString derive(RTLIL::Design *design, અચલ dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	આભાસી RTLIL::IdString derive(RTLIL::Design *design, અચલ dict<RTLIL::IdString, RTLIL::Const> &parameters, અચલ dict<RTLIL::IdString, RTLIL::Module*> &interfaces, અચલ dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	આભાસી size_t ગણતરી_id(RTLIL::IdString id);
	આભાસી શૂન્ય expand_interfaces(RTLIL::Design *design, અચલ dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	આભાસી bool reprocess_if_necessary(RTLIL::Design *design);

	આભાસી શૂન્ય sort();
	આભાસી શૂન્ય check();
	આભાસી શૂન્ય optimize();
	આભાસી શૂન્ય makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) અચલ {
		પાછા get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	શૂન્ય connect(અચલ RTLIL::SigSig &conn);
	શૂન્ય connect(અચલ RTLIL::SigSpec &lhs, અચલ RTLIL::SigSpec &rhs);
	શૂન્ય new_connections(અચલ std::vector<RTLIL::SigSig> &new_conn);
	અચલ std::vector<RTLIL::SigSig> &connections() અચલ;

	std::vector<RTLIL::IdString> ports;
	શૂન્ય fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_queue;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_port_queue;
	pool<RTLIL::Wire *> buf_norm_wire_queue;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_index;
	શૂન્ય bufNormalize();

	template<typename T> શૂન્ય rewrite_sigspecs(T &functor);
	template<typename T> શૂન્ય rewrite_sigspecs2(T &functor);
	શૂન્ય cloneInto(RTLIL::Module *new_mod) અચલ;
	આભાસી RTLIL::Module *clone() અચલ;
	આભાસી RTLIL::Module *clone(RTLIL::Design *dst) અચલ;
	આભાસી RTLIL::Module *clone(RTLIL::Design *dst, IdString target_name) અચલ;

	bool has_memories() અચલ;
	bool has_processes() અચલ;

	bool has_memories_warn() અચલ;
	bool has_processes_warn() અચલ;

	bool is_selected() અચલ;
	bool is_selected_whole() અચલ;

	std::vector<RTLIL::Wire*> selected_wires() અચલ;
	std::vector<RTLIL::Cell*> selected_cells() અચલ;
	std::vector<RTLIL::Memory*> selected_memories() અચલ;
	std::vector<RTLIL::Process*> selected_processes() અચલ;
	std::vector<RTLIL::SelectedMember> selected_members() અચલ;

	template<typename T> bool selected(T *member) અચલ {
		પાછા design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		જો (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		પાછા it == wires_.end() ? nullptr : it->second;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		જો (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		પાછા it == cells_.end() ? nullptr : it->second;
	}

	અચલ RTLIL::Wire* wire(RTLIL::IdString id) અચલ{
		જો (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		પાછા it == wires_.end() ? nullptr : it->second;
	}
	અચલ RTLIL::Cell* cell(RTLIL::IdString id) અચલ {
		જો (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		પાછા it == cells_.end() ? nullptr : it->second;
	}

	template<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ પાછા wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ પાછા cell(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> અચલ RTLIL::Wire *wire(N name) અચલ
		{ પાછા wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> અચલ RTLIL::Cell *cell(N name) અચલ
		{ પાછા cell(twines().find(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { પાછા RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	પૂર્ણાંક wires_size() અચલ { પાછા wires_.size(); }
	RTLIL::Wire* wire_at(પૂર્ણાંક index) અચલ { પાછા wires_.element(index)->second; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { પાછા RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	પૂર્ણાંક cells_size() અચલ { પાછા cells_.size(); }
	RTLIL::Cell* cell_at(પૂર્ણાંક index) અચલ { પાછા cells_.element(index)->second; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	શૂન્ય remove(અચલ pool<RTLIL::Wire*> &wires);
	શૂન્ય remove(RTLIL::Cell *cell);
	શૂન્ય remove(RTLIL::Memory *memory);
	શૂન્ય remove(RTLIL::Process *process);

	શૂન્ય rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	શૂન્ય rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	શૂન્ય rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	શૂન્ય swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2);
	શૂન્ય swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, પૂર્ણાંક &index);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *addWire(RTLIL::IdString name, પૂર્ણાંક width = 1);
	RTLIL::Wire *addWire(RTLIL::IdString name, અચલ RTLIL::Wire *other);
	YS_NAME_FWD(addWire)

	RTLIL::Cell *addCell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *addCell(RTLIL::IdString name, અચલ RTLIL::Cell *other);
	YS_NAME_FWD(addCell)
	template<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *addCell(RTLIL::IdString name, T type) { પાછા addCell(name, design->twines().add(std::move(type))); }

	RTLIL::Memory *addMemory(RTLIL::IdString name);
	YS_NAME_FWD(addMemory)
	RTLIL::Memory *addMemory(RTLIL::IdString name, અચલ RTLIL::Memory *other);

	RTLIL::Process *addProcess(RTLIL::IdString name);
	YS_NAME_FWD(addProcess)
	RTLIL::Process *addProcess(RTLIL::IdString name, અચલ RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* addNot     (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addPos     (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addBuf     (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addBarrier (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addNeg     (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");

	RTLIL::Cell* addAnd  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addOr   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addXor  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addXnor (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");

	RTLIL::Cell* addReduceAnd  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addReduceOr   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addReduceXor  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addReduceXnor (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addReduceBool (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");

	RTLIL::Cell* addShl    (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addShr    (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addSshl   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addSshr   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addShift  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addShiftx (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");

	RTLIL::Cell* addLt  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addLe  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addEq  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addNe  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addEqx (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addNex (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addGe  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addGt  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");

	RTLIL::Cell* addAdd (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addSub (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addMul (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	// truncating division
	RTLIL::Cell* addDiv (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	// truncating modulo
	RTLIL::Cell* addMod (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addDivFloor (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addModFloor (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addPow (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, અચલ std::string &src = "");

	RTLIL::Cell* addFa (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_c, અચલ RTLIL::SigSpec &sig_x, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src = "");

	RTLIL::Cell* addLogicNot (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addLogicAnd (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::Cell* addLogicOr  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, bool is_signed = false, અચલ std::string &src = "");

	RTLIL::Cell* addMux  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_s, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addPmux (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_s, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addBmux (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_s, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addDemux (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_s, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src = "");

	RTLIL::Cell* addBweqx  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addBwmux  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_s, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src = "");

	RTLIL::Cell* addSlice  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, RTLIL::Const offset, અચલ std::string &src = "");
	RTLIL::Cell* addConcat (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addLut    (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_y, RTLIL::Const lut, અચલ std::string &src = "");
	RTLIL::Cell* addTribuf (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addAssert (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_en, અચલ std::string &src = "");
	RTLIL::Cell* addAssume (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_en, અચલ std::string &src = "");
	RTLIL::Cell* addLive   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_en, અચલ std::string &src = "");
	RTLIL::Cell* addFair   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_en, અચલ std::string &src = "");
	RTLIL::Cell* addCover  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_en, અચલ std::string &src = "");
	RTLIL::Cell* addEquiv  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src = "");

	RTLIL::Cell* addSr    (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr, અચલ RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addFf    (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, અચલ std::string &src = "");
	RTLIL::Cell* addDff   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_d,   અચલ RTLIL::SigSpec &sig_q, bool clk_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addDffe  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en,  અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addDffsr (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, અચલ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addDffsre (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, અચલ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addAdff (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_arst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, RTLIL::Const arst_કિંમત, bool clk_polarity = true, bool arst_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addAdffe (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_arst,  અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, RTLIL::Const arst_કિંમત, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addAldff (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_aload, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, અચલ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addAldffe (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_aload,  અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, અચલ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addSdff (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_srst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, RTLIL::Const srst_કિંમત, bool clk_polarity = true, bool srst_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addSdffe (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_srst,  અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, RTLIL::Const srst_કિંમત, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addSdffce (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_srst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, RTLIL::Const srst_કિંમત, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addDlatch (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, bool en_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addAdlatch (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_arst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, RTLIL::Const arst_કિંમત, bool en_polarity = true, bool arst_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addDlatchsr (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, અચલ RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, અચલ std::string &src = "");

	RTLIL::Cell* addBufGate    (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addNotGate    (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addAndGate    (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addNandGate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addOrGate     (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addNorGate    (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addXorGate    (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addXnorGate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addAndnotGate (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addOrnotGate  (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addMuxGate    (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_s, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addNmuxGate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_s, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addAoi3Gate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_c, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addOai3Gate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_c, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addAoi4Gate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_c, અચલ RTLIL::SigBit &sig_d, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");
	RTLIL::Cell* addOai4Gate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_c, અચલ RTLIL::SigBit &sig_d, અચલ RTLIL::SigBit &sig_y, અચલ std::string &src = "");

	RTLIL::Cell* addSrGate     (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr,
			અચલ RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addFfGate     (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, અચલ std::string &src = "");
	RTLIL::Cell* addDffGate    (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, bool clk_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addDffeGate   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addDffsrGate  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, અચલ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addDffsreGate (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, અચલ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addAdffGate   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_arst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
			bool arst_કિંમત = false, bool clk_polarity = true, bool arst_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addAdffeGate  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_arst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
			bool arst_કિંમત = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addAldffGate   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_aload, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
			અચલ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addAldffeGate  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_aload, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
			અચલ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addSdffGate   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_srst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
			bool srst_કિંમત = false, bool clk_polarity = true, bool srst_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addSdffeGate  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_srst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
			bool srst_કિંમત = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addSdffceGate (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_clk, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_srst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
			bool srst_કિંમત = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addDlatchGate (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, bool en_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addAdlatchGate(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_arst, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q,
			bool arst_કિંમત = false, bool en_polarity = true, bool arst_polarity = true, અચલ std::string &src = "");
	RTLIL::Cell* addDlatchsrGate  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_en, અચલ RTLIL::SigSpec &sig_set, અચલ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, અચલ RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, અચલ std::string &src = "");

	RTLIL::Cell* addAnyinit(RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_d, અચલ RTLIL::SigSpec &sig_q, અચલ std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, bool is_signed = false, અચલ std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, bool is_signed = false, અચલ std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Shift  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");

	RTLIL::SigSpec Add (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, અચલ std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, bool is_signed = false, અચલ std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_s, અચલ std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_s, અચલ std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_s, અચલ std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_s, અચલ std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_b, અચલ RTLIL::SigSpec &sig_s, અચલ std::string &src = "");

	RTLIL::SigBit BufGate    (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ std::string &src = "");
	RTLIL::SigBit NotGate    (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ std::string &src = "");
	RTLIL::SigBit AndGate    (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ std::string &src = "");
	RTLIL::SigBit NandGate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ std::string &src = "");
	RTLIL::SigBit OrGate     (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ std::string &src = "");
	RTLIL::SigBit NorGate    (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ std::string &src = "");
	RTLIL::SigBit XorGate    (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ std::string &src = "");
	RTLIL::SigBit XnorGate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ std::string &src = "");
	RTLIL::SigBit AndnotGate (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ std::string &src = "");
	RTLIL::SigBit OrnotGate  (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ std::string &src = "");
	RTLIL::SigBit MuxGate    (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_s, અચલ std::string &src = "");
	RTLIL::SigBit NmuxGate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_s, અચલ std::string &src = "");
	RTLIL::SigBit Aoi3Gate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_c, અચલ std::string &src = "");
	RTLIL::SigBit Oai3Gate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_c, અચલ std::string &src = "");
	RTLIL::SigBit Aoi4Gate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_c, અચલ RTLIL::SigBit &sig_d, અચલ std::string &src = "");
	RTLIL::SigBit Oai4Gate   (RTLIL::IdString name, અચલ RTLIL::SigBit &sig_a, અચલ RTLIL::SigBit &sig_b, અચલ RTLIL::SigBit &sig_c, અચલ RTLIL::SigBit &sig_d, અચલ std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, પૂર્ણાંક width = 1, અચલ std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, પૂર્ણાંક width = 1, અચલ std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, પૂર્ણાંક width = 1, અચલ std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, પૂર્ણાંક width = 1, અચલ std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, અચલ std::string &src = "");

	RTLIL::SigSpec SetTag          (RTLIL::IdString name, અચલ std::string &tag, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_s, અચલ RTLIL::SigSpec &sig_c, અચલ std::string &src = "");
	RTLIL::Cell*   addSetTag       (RTLIL::IdString name, અચલ std::string &tag, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_s, અચલ RTLIL::SigSpec &sig_c, અચલ RTLIL::SigSpec &sig_y, અચલ std::string &src = "");
	RTLIL::SigSpec GetTag          (RTLIL::IdString name, અચલ std::string &tag, અચલ RTLIL::SigSpec &sig_a, અચલ std::string &src = "");
	RTLIL::Cell*   addOverwriteTag (RTLIL::IdString name, અચલ std::string &tag, અચલ RTLIL::SigSpec &sig_a, અચલ RTLIL::SigSpec &sig_s, અચલ RTLIL::SigSpec &sig_c, અચલ std::string &src = "");
	RTLIL::SigSpec OriginalTag     (RTLIL::IdString name, અચલ std::string &tag, અચલ RTLIL::SigSpec &sig_a, અચલ std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, અચલ RTLIL::SigSpec &sig_e, અચલ std::string &src = "");
	YS_NAME_FWD(addAnyinit)
	YS_NAME_FWD(Anyconst) YS_NAME_FWD(Anyseq) YS_NAME_FWD(Allconst) YS_NAME_FWD(Allseq)
	YS_NAME_FWD(Initstate)
	YS_NAME_FWD(SetTag) YS_NAME_FWD(addSetTag) YS_NAME_FWD(GetTag)
	YS_NAME_FWD(addOverwriteTag) YS_NAME_FWD(OriginalTag) YS_NAME_FWD(FutureFF)
	YS_NAME_FWD(addNot) YS_NAME_FWD(addPos) YS_NAME_FWD(addBuf) YS_NAME_FWD(addBarrier) YS_NAME_FWD(addNeg)
	YS_NAME_FWD(addAnd) YS_NAME_FWD(addOr) YS_NAME_FWD(addXor) YS_NAME_FWD(addXnor)
	YS_NAME_FWD(addReduceAnd) YS_NAME_FWD(addReduceOr) YS_NAME_FWD(addReduceXor) YS_NAME_FWD(addReduceXnor)
	YS_NAME_FWD(addReduceBool) YS_NAME_FWD(addShl) YS_NAME_FWD(addShr) YS_NAME_FWD(addSshl)
	YS_NAME_FWD(addSshr) YS_NAME_FWD(addShift) YS_NAME_FWD(addShiftx) YS_NAME_FWD(addLt)
	YS_NAME_FWD(addLe) YS_NAME_FWD(addEq) YS_NAME_FWD(addNe) YS_NAME_FWD(addEqx)
	YS_NAME_FWD(addNex) YS_NAME_FWD(addGe) YS_NAME_FWD(addGt) YS_NAME_FWD(addAdd)
	YS_NAME_FWD(addSub) YS_NAME_FWD(addMul) YS_NAME_FWD(addDiv) YS_NAME_FWD(addMod)
	YS_NAME_FWD(addDivFloor) YS_NAME_FWD(addModFloor) YS_NAME_FWD(addPow) YS_NAME_FWD(addFa)
	YS_NAME_FWD(addLogicNot) YS_NAME_FWD(addLogicAnd) YS_NAME_FWD(addLogicOr) YS_NAME_FWD(addMux)
	YS_NAME_FWD(addPmux) YS_NAME_FWD(addBmux) YS_NAME_FWD(addDemux) YS_NAME_FWD(addBweqx)
	YS_NAME_FWD(addBwmux) YS_NAME_FWD(addSlice) YS_NAME_FWD(addConcat) YS_NAME_FWD(addLut)
	YS_NAME_FWD(addTribuf) YS_NAME_FWD(addAssert) YS_NAME_FWD(addAssume) YS_NAME_FWD(addLive)
	YS_NAME_FWD(addFair) YS_NAME_FWD(addCover) YS_NAME_FWD(addEquiv) YS_NAME_FWD(addSr)
	YS_NAME_FWD(addFf) YS_NAME_FWD(addDff) YS_NAME_FWD(addDffe) YS_NAME_FWD(addDffsr)
	YS_NAME_FWD(addDffsre) YS_NAME_FWD(addAdff) YS_NAME_FWD(addAdffe) YS_NAME_FWD(addAldff)
	YS_NAME_FWD(addAldffe) YS_NAME_FWD(addSdff) YS_NAME_FWD(addSdffe) YS_NAME_FWD(addSdffce)
	YS_NAME_FWD(addDlatch) YS_NAME_FWD(addAdlatch) YS_NAME_FWD(addDlatchsr) YS_NAME_FWD(addBufGate)
	YS_NAME_FWD(addNotGate) YS_NAME_FWD(addAndGate) YS_NAME_FWD(addNandGate) YS_NAME_FWD(addOrGate)
	YS_NAME_FWD(addNorGate) YS_NAME_FWD(addXorGate) YS_NAME_FWD(addXnorGate) YS_NAME_FWD(addAndnotGate)
	YS_NAME_FWD(addOrnotGate) YS_NAME_FWD(addMuxGate) YS_NAME_FWD(addNmuxGate) YS_NAME_FWD(addAoi3Gate)
	YS_NAME_FWD(addOai3Gate) YS_NAME_FWD(addAoi4Gate) YS_NAME_FWD(addOai4Gate) YS_NAME_FWD(addSrGate)
	YS_NAME_FWD(addFfGate) YS_NAME_FWD(addDffGate) YS_NAME_FWD(addDffeGate) YS_NAME_FWD(addDffsrGate)
	YS_NAME_FWD(addDffsreGate) YS_NAME_FWD(addAdffGate) YS_NAME_FWD(addAdffeGate) YS_NAME_FWD(addAldffGate)
	YS_NAME_FWD(addAldffeGate) YS_NAME_FWD(addSdffGate) YS_NAME_FWD(addSdffeGate) YS_NAME_FWD(addSdffceGate)
	YS_NAME_FWD(addDlatchGate) YS_NAME_FWD(addAdlatchGate) YS_NAME_FWD(addDlatchsrGate) YS_NAME_FWD(Not)
	YS_NAME_FWD(Pos) YS_NAME_FWD(Buf) YS_NAME_FWD(Barrier) YS_NAME_FWD(Neg) YS_NAME_FWD(And)
	YS_NAME_FWD(Or) YS_NAME_FWD(Xor) YS_NAME_FWD(Xnor) YS_NAME_FWD(ReduceAnd)
	YS_NAME_FWD(ReduceOr) YS_NAME_FWD(ReduceXor) YS_NAME_FWD(ReduceXnor) YS_NAME_FWD(ReduceBool)
	YS_NAME_FWD(Shl) YS_NAME_FWD(Shr) YS_NAME_FWD(Sshl) YS_NAME_FWD(Sshr)
	YS_NAME_FWD(Shift) YS_NAME_FWD(Shiftx) YS_NAME_FWD(Lt) YS_NAME_FWD(Le)
	YS_NAME_FWD(Eq) YS_NAME_FWD(Ne) YS_NAME_FWD(Eqx) YS_NAME_FWD(Nex)
	YS_NAME_FWD(Ge) YS_NAME_FWD(Gt) YS_NAME_FWD(Add) YS_NAME_FWD(Sub)
	YS_NAME_FWD(Mul) YS_NAME_FWD(Div) YS_NAME_FWD(Mod) YS_NAME_FWD(DivFloor)
	YS_NAME_FWD(ModFloor) YS_NAME_FWD(Pow) YS_NAME_FWD(LogicNot) YS_NAME_FWD(LogicAnd)
	YS_NAME_FWD(LogicOr) YS_NAME_FWD(Mux) YS_NAME_FWD(Pmux) YS_NAME_FWD(Bmux)
	YS_NAME_FWD(Demux) YS_NAME_FWD(Bweqx) YS_NAME_FWD(Bwmux) YS_NAME_FWD(BufGate)
	YS_NAME_FWD(NotGate) YS_NAME_FWD(AndGate) YS_NAME_FWD(NandGate) YS_NAME_FWD(OrGate)
	YS_NAME_FWD(NorGate) YS_NAME_FWD(XorGate) YS_NAME_FWD(XnorGate) YS_NAME_FWD(AndnotGate)
	YS_NAME_FWD(OrnotGate) YS_NAME_FWD(MuxGate) YS_NAME_FWD(NmuxGate) YS_NAME_FWD(Aoi3Gate)
	YS_NAME_FWD(Oai3Gate) YS_NAME_FWD(Aoi4Gate) YS_NAME_FWD(Oai4Gate)

	std::string to_rtlil_str() અચલ;
#ifdef YOSYS_ENABLE_PYTHON
	સ્થિર std::map<unsigned પૂર્ણાંક, RTLIL::Module*> *get_all_modules(શૂન્ય);
#endif
};

સંરચના RTLIL::Wire : જાહેર RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) અચલ { h.eat(hashidx_); પાછા h; }

સુરક્ષિત:
	// use module->addWire() and module->remove() to create or destroy wires
	friend સંરચના RTLIL::Module;
	Wire();
	~Wire();

	friend સંરચના RTLIL::Design;
	friend સંરચના RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driverPort_;

જાહેર:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	શૂન્ય operator=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	પૂર્ણાંક width, start_offset, port_id;
	bool port_આગત, port_output, upto, is_signed;

	RTLIL::Design *design() અચલ { પાછા module ? module->design : nullptr; }
	TwinePool &twines() અચલ { પાછા module->twines(); }

	bool known_driver() અચલ { પાછા driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() અચલ    { log_assert(driverCell_); પાછા driverCell_; };
	RTLIL::IdString driverPort() અચલ { log_assert(driverCell_); પાછા driverPort_; };

	પૂર્ણાંક from_hdl_index(પૂર્ણાંક hdl_index) {
		પૂર્ણાંક zero_index = hdl_index - start_offset;
		પૂર્ણાંક rtlil_index = upto ? width - 1 - zero_index : zero_index;
		પાછા rtlil_index >= 0 && rtlil_index < width ? rtlil_index : INT_MIN;
	}

	પૂર્ણાંક to_hdl_index(પૂર્ણાંક rtlil_index) {
		જો (rtlil_index < 0 || rtlil_index >= width)
			પાછા INT_MIN;
		પૂર્ણાંક zero_index = upto ? width - 1 - rtlil_index : rtlil_index;
		પાછા zero_index + start_offset;
	}

	std::string to_rtlil_str() અચલ;

#ifdef YOSYS_ENABLE_PYTHON
	સ્થિર std::map<unsigned પૂર્ણાંક, RTLIL::Wire*> *get_all_wires(શૂન્ય);
#endif
};

અંતઃરેખ પૂર્ણાંક GetSize(RTLIL::Wire *wire) {
	પાછા wire->width;
}

સંરચના RTLIL::Memory : જાહેર RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) અચલ { h.eat(hashidx_); પાછા h; }

	Memory();
	~Memory();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() અચલ { પાછા module ? module->design : nullptr; }
	TwinePool &twines() અચલ { પાછા module->twines(); }

	RTLIL::MemoryNameId name;

	પૂર્ણાંક width, start_offset, size;
#ifdef YOSYS_ENABLE_PYTHON
	સ્થિર std::map<unsigned પૂર્ણાંક, RTLIL::Memory*> *get_all_memorys(શૂન્ય);
#endif

	std::string to_rtlil_str() અચલ;
};

સંરચના RTLIL::Cell : જાહેર RTLIL::AttrObject
{
ખાનગી:
	શૂન્ય initIndex();

	bool bufnorm_handle_setPort(IdString portname, RTLIL::SigSpec &સંકેત, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
જાહેર:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) અચલ { h.eat(hashidx_); પાછા h; }

સુરક્ષિત:
	// use module->addCell() and module->remove() to create or destroy cells
	friend સંરચના RTLIL::Module;
	Cell();
	~Cell();

જાહેર:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	શૂન્ય operator=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() અચલ { પાછા module ? module->design : nullptr; }
	TwinePool &twines() અચલ { પાછા module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool hasPort(RTLIL::IdString portname) અચલ;
	શૂન્ય unsetPort(RTLIL::IdString portname);
	શૂન્ય setPort(RTLIL::IdString portname, RTLIL::SigSpec સંકેત);
	template<typename N, YS_NAME_STRING(N)> શૂન્ય setPort(N portname, RTLIL::SigSpec સંકેત)
		{ setPort(module->twines().add(std::move(portname)), std::move(સંકેત)); }
	અચલ RTLIL::SigSpec &getPort(RTLIL::IdString portname) અચલ;
	અચલ dict<RTLIL::IdString, RTLIL::SigSpec> &connections() અચલ;

	bool known() અચલ;
	bool આગત(RTLIL::IdString portname) અચલ;
	bool output(RTLIL::IdString portname) અચલ;
	PortDir port_dir(RTLIL::IdString portname) અચલ;

	bool hasParam(RTLIL::IdString paramname) અચલ;
	શૂન્ય unsetParam(RTLIL::IdString paramname);
	શૂન્ય setParam(RTLIL::IdString paramname, RTLIL::Const કિંમત);
	અચલ RTLIL::Const &getParam(RTLIL::IdString paramname) અચલ;

	template<typename N, YS_NAME_STRING(N)> bool hasParam(N name) અચલ
		{ પાછા hasParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> શૂન્ય unsetParam(N name)
		{ unsetParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> શૂન્ય setParam(N name, RTLIL::Const કિંમત)
		{ setParam(module->twines().add(std::move(name)), std::move(કિંમત)); }
	template<typename N, YS_NAME_STRING(N)> અચલ RTLIL::Const &getParam(N name) અચલ
		{ પાછા getParam(module->twines().find(std::string_view(name))); }

	શૂન્ય sort();
	શૂન્ય check();
	શૂન્ય fixup_parameters(bool set_a_signed = false, bool set_b_signed = false);

	bool has_keep_attr() અચલ;

	template<typename T> શૂન્ય rewrite_sigspecs(T &functor);
	template<typename T> શૂન્ય rewrite_sigspecs2(T &functor);

#ifdef YOSYS_ENABLE_PYTHON
	સ્થિર std::map<unsigned પૂર્ણાંક, RTLIL::Cell*> *get_all_cells(શૂન્ય);
#endif

	bool has_memid() અચલ;
	bool is_mem_cell() અચલ;
	bool is_builtin_ff() અચલ;

	std::string to_rtlil_str() અચલ;
};

સંરચના RTLIL::CaseRule : જાહેર RTLIL::AttrObject
{
	std::vector<RTLIL::SigSpec> compare;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::SwitchRule*> switches;

	~CaseRule();

	bool empty() અચલ;

	template<typename T> શૂન્ય rewrite_sigspecs(T &functor);
	template<typename T> શૂન્ય rewrite_sigspecs2(T &functor);
	RTLIL::CaseRule *clone() અચલ;
};

સંરચના RTLIL::SwitchRule : જાહેર RTLIL::AttrObject
{
	RTLIL::SigSpec સંકેત;
	std::vector<RTLIL::CaseRule*> cases;

	~SwitchRule();

	bool empty() અચલ;

	template<typename T> શૂન્ય rewrite_sigspecs(T &functor);
	template<typename T> શૂન્ય rewrite_sigspecs2(T &functor);
	RTLIL::SwitchRule *clone() અચલ;
};

સંરચના RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec address;
	RTLIL::SigSpec data;
	RTLIL::SigSpec enable;
	RTLIL::Const priority_mask;
};

સંરચના RTLIL::SyncRule
{
	RTLIL::SyncType type;
	RTLIL::SigSpec સંકેત;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::MemWriteAction> mem_write_actions;

	template<typename T> શૂન્ય rewrite_sigspecs(T &functor);
	template<typename T> શૂન્ય rewrite_sigspecs2(T &functor);
	RTLIL::SyncRule *clone() અચલ;
};

સંરચના RTLIL::Process : જાહેર RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) અચલ { h.eat(hashidx_); પાછા h; }

સુરક્ષિત:
	// use module->addProcess() and module->remove() to create or destroy processes
	friend સંરચના RTLIL::Module;
	Process();
	~Process();

જાહેર:
	RTLIL::Module *module;
	RTLIL::CaseRule મૂળ_case;
	std::vector<RTLIL::SyncRule*> syncs;

	RTLIL::Design *design() અચલ { પાછા module ? module->design : nullptr; }
	TwinePool &twines() અચલ { પાછા module->twines(); }

	template<typename T> શૂન્ય rewrite_sigspecs(T &functor);
	template<typename T> શૂન્ય rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() અચલ;

	std::string to_rtlil_str() અચલ;
};

અંતઃરેખ RTLIL::SigBit::SigBit() : wire(ખાલી), data(RTLIL::State::S0) { }
અંતઃરેખ RTLIL::SigBit::SigBit(RTLIL::State bit) : wire(ખાલી), data(bit) { }
અંતઃરેખ RTLIL::SigBit::SigBit(bool bit) : wire(ખાલી), data(bit ? State::S1 : State::S0) { }
અંતઃરેખ RTLIL::SigBit::SigBit(RTLIL::Wire *wire) : wire(wire), offset(0) { log_assert(wire && wire->width == 1); }
અંતઃરેખ RTLIL::SigBit::SigBit(RTLIL::Wire *wire, પૂર્ણાંક offset) : wire(wire), offset(offset) { log_assert(wire != nullptr); }
અંતઃરેખ RTLIL::SigBit::SigBit(અચલ RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.width == 1); જો (wire) offset = chunk.offset; નહીંતર data = chunk.data[0]; }
અંતઃરેખ RTLIL::SigBit::SigBit(અચલ RTLIL::SigChunk &chunk, પૂર્ણાંક index) : wire(chunk.wire) { જો (wire) offset = chunk.offset + index; નહીંતર data = chunk.data[index]; }

અંતઃરેખ bool RTLIL::SigBit::operator<(અચલ RTLIL::SigBit &other) અચલ {
	જો (wire == other.wire)
		પાછા wire ? (offset < other.offset) : (data < other.data);
	જો (wire != nullptr && other.wire != nullptr)
		પાછા wire->name < other.wire->name;
	પાછા (wire != nullptr) < (other.wire != nullptr);
}

અંતઃરેખ bool RTLIL::SigBit::operator==(અચલ RTLIL::SigBit &other) અચલ {
	પાછા (wire == other.wire) && (wire ? (offset == other.offset) : (data == other.data));
}

અંતઃરેખ bool RTLIL::SigBit::operator!=(અચલ RTLIL::SigBit &other) અચલ {
	પાછા (wire != other.wire) || (wire ? (offset != other.offset) : (data != other.data));
}

અંતઃરેખ Hasher RTLIL::SigBit::hash_into(Hasher h) અચલ {
	જો (wire) {
		h.eat(offset);
		h.eat(wire->name);
		પાછા h;
	}
	h.eat(data);
	પાછા h;
}


અંતઃરેખ Hasher RTLIL::SigBit::hash_ટોચ() અચલ {
	Hasher h;
	જો (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.bits();
		h.force(hashlib::legacy::djb2_add(n, offset));
		પાછા h;
	}
	h.force(data);
	પાછા h;
}

અંતઃરેખ RTLIL::SigBit &RTLIL::SigSpecIterator::operator*() અચલ {
	પાછા (*sig_p)[index];
}

અંતઃરેખ અચલ RTLIL::SigBit &RTLIL::SigSpecConstIterator::operator*() {
	bit = (*sig_p)[index];
	પાછા bit;
}

અંતઃરેખ RTLIL::SigBit::SigBit(અચલ RTLIL::SigSpec &sig) {
	log_assert(sig.size() == 1);
	auto it = sig.chunks().begin();
	*this = SigBit(*it);
}

template<typename T>
શૂન્ય RTLIL::Module::rewrite_sigspecs(T &functor)
{
	માટે (auto &it : cells_)
		it.second->rewrite_sigspecs(functor);
	માટે (auto &it : processes)
		it.second->rewrite_sigspecs(functor);
	માટે (auto &it : connections_) {
		functor(it.first);
		functor(it.second);
	}
}

template<typename T>
શૂન્ય RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	માટે (auto &it : cells_)
		it.second->rewrite_sigspecs2(functor);
	માટે (auto &it : processes)
		it.second->rewrite_sigspecs2(functor);
	માટે (auto &it : connections_) {
		functor(it.first, it.second);
	}
}

template<typename T>
શૂન્ય RTLIL::Cell::rewrite_sigspecs(T &functor) {
	માટે (auto &it : connections_)
		functor(it.second);
}

template<typename T>
શૂન્ય RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	માટે (auto &it : connections_)
		functor(it.second);
}

template<typename T>
શૂન્ય RTLIL::CaseRule::rewrite_sigspecs(T &functor) {
	માટે (auto &it : compare)
		functor(it);
	માટે (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	માટે (auto it : switches)
		it->rewrite_sigspecs(functor);
}

template<typename T>
શૂન્ય RTLIL::CaseRule::rewrite_sigspecs2(T &functor) {
	માટે (auto &it : compare)
		functor(it);
	માટે (auto &it : actions) {
		functor(it.first, it.second);
	}
	માટે (auto it : switches)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
શૂન્ય RTLIL::SwitchRule::rewrite_sigspecs(T &functor)
{
	functor(સંકેત);
	માટે (auto it : cases)
		it->rewrite_sigspecs(functor);
}

template<typename T>
શૂન્ય RTLIL::SwitchRule::rewrite_sigspecs2(T &functor)
{
	functor(સંકેત);
	માટે (auto it : cases)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
શૂન્ય RTLIL::SyncRule::rewrite_sigspecs(T &functor)
{
	functor(સંકેત);
	માટે (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	માટે (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
શૂન્ય RTLIL::SyncRule::rewrite_sigspecs2(T &functor)
{
	functor(સંકેત);
	માટે (auto &it : actions) {
		functor(it.first, it.second);
	}
	માટે (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
શૂન્ય RTLIL::Process::rewrite_sigspecs(T &functor)
{
	મૂળ_case.rewrite_sigspecs(functor);
	માટે (auto it : syncs)
		it->rewrite_sigspecs(functor);
}

template<typename T>
શૂન્ય RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	મૂળ_case.rewrite_sigspecs2(functor);
	માટે (auto it : syncs)
		it->rewrite_sigspecs2(functor);
}

#અન્તર્ભાવ "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_END

#endif
