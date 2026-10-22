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

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/yosys.h"
#ضمن "kernel/twine.h"

#ضمن <deque>
#ضمن <string_view>
#ضمن <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	تعداد State : unsigned حرف {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	تعداد SyncType : unsigned حرف {
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
	تعداد ConstFlags : unsigned حرف {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	تعداد SelectPartials : unsigned حرف {
		SELECT_ALL = 0,          // include partial modules
		SELECT_WHOLE_ONLY = 1,   // ignore partial modules
		SELECT_WHOLE_WARN = 2,   // call log_warning on partial module
		SELECT_WHOLE_ERR = 3,    // call log_error on partial module
		SELECT_WHOLE_CMDERR = 4  // call log_cmd_error on partial module
	};

	تعداد SelectBoxes : unsigned حرف {
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

	تعداد PortDir : unsigned حرف  {
		PD_UNKNOWN = 0,
		PD_تێخستن = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr صحيح WIDTH_LIMIT = 1 << 30;

	بنية Const;
	بنية AttrObject;
	بنية SelectedMember;
	بنية Selection;
	بنية Monitor;
	بنية Design;
	بنية Module;
	بنية Wire;
	بنية Memory;
	بنية Cell;
	بنية SigChunk;
	بنية SigBit;
	بنية SigSpecIterator;
	بنية SigSpecConstIterator;
	بنية SigSpec;
	بنية CaseRule;
	بنية SwitchRule;
	بنية MemWriteAction;
	بنية SyncRule;
	بنية Process;

	تعريفنوع std::pair<SigSpec, SigSpec> SigSig;
};

بنية SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_PREFIX ID; }
namespace RTLIL { using YOSYS_NAMESPACE_PREFIX IdString; }

namespace RTLIL {
	فراغ copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			ثابت dict<IdString, RTLIL::Const> &src,
			ثابت RTLIL::Design *src_design, RTLIL::Design *dst_design);

	خارجي dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	ثابت pool<IdString> &builtin_ff_cell_types();

	ساكن مضمن std::string escape_id(ثابت std::string &str) {
		إذا (str.size() > 0 && str[0] != '\\' && str[0] != '$')
			أرجع "\\" + str;
		أرجع str;
	}

	ساكن مضمن std::string unescape_id(ثابت std::string &str) {
		إذا (str.size() < 2)
			أرجع str;
		إذا (str[0] != '\\')
			أرجع str;
		إذا (str[1] == '$' || str[1] == '\\')
			أرجع str;
		إذا (str[1] >= '0' && str[1] <= '9')
			أرجع str;
		أرجع str.substr(1);
	}

	template <typename T> using sort_by_name_id = IdString::compare_ptr_by_name<T>;

	template <typename T> بنية sort_by_name_str {
		bool operator()(T *a, T *b) ثابت {
			أرجع a->name.lt_by_name(b->name);
		}
	};

	بنية sort_by_id_str {
		ثابت TwinePool& pool;
		explicit sort_by_id_str(ثابت TwinePool& pool) : pool(pool) {}
		bool operator()(RTLIL::IdString a, RTLIL::IdString b) ثابت {
			أرجع pool.compare_by_name(a, b) < 0;
		}
	};

	ساكن مضمن std::string encode_filename(ثابت std::string &filename)
	{
		std::stringstream val;
		إذا (!std::any_of(filename.begin(), filename.end(), [](حرف c) {
			أرجع static_cast<unsigned حرف>(c) < 33 || static_cast<unsigned حرف>(c) > 126;
		})) أرجع filename;
		لكل (unsigned حرف ثابت c : filename) {
			إذا (c < 33 || c > 126)
				val << stringf("$%02x", c);
			وإلا
				val << c;
		}
		أرجع val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_and         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_or          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_xor         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_xnor        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);

	RTLIL::Const const_reduce_and  (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_reduce_or   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_reduce_xor  (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_reduce_xnor (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_reduce_bool (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);

	RTLIL::Const const_logic_not   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_logic_and   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_logic_or    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);

	RTLIL::Const const_shl         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_shr         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_sshl        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_sshr        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_shift       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_shiftx      (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);

	RTLIL::Const const_lt          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_le          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_eq          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_ne          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_eqx         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_nex         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_ge          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_gt          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);

	RTLIL::Const const_add         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_sub         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_mul         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_div         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_divfloor    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_modfloor    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_mod         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_pow         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);

	RTLIL::Const const_pos         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_buf         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);
	RTLIL::Const const_neg         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح result_len);

	RTLIL::Const const_mux         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	template<typename T, typename کلیل_2 = IdString>
	بنية ObjIterator {
		using iterator_category = std::forward_iterator_tag;
		using بەها_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		typename dict<کلیل_2, T>::iterator it;
		dict<کلیل_2, T> *list_p;
		صحيح *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, صحيح *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			إذا (list_p->empty()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} وإلا {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(ثابت RTLIL::ObjIterator<T, کلیل_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			إذا (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &operator=(ثابت RTLIL::ObjIterator<T, کلیل_2> &other) {
			إذا (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			إذا (refcount_p)
				(*refcount_p)++;
			أرجع *this;
		}

		~ObjIterator() {
			إذا (refcount_p)
				(*refcount_p)--;
		}

		مضمن T operator*() ثابت {
			log_assert(list_p != nullptr);
			أرجع it->second;
		}

		مضمن bool operator!=(ثابت RTLIL::ObjIterator<T, کلیل_2> &other) ثابت {
			إذا (list_p == nullptr || other.list_p == nullptr)
				أرجع list_p != other.list_p;
			أرجع it != other.it;
		}


		مضمن bool operator==(ثابت RTLIL::ObjIterator<T, کلیل_2> &other) ثابت {
			أرجع !(*this != other);
		}

		مضمن ObjIterator<T, کلیل_2>& operator++() {
			log_assert(list_p != nullptr);
			إذا (++it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			أرجع *this;
		}

		مضمن ObjIterator<T, کلیل_2>& operator+=(صحيح amt) {
			log_assert(list_p != nullptr);
			it += amt;
			إذا (it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			أرجع *this;
		}

		مضمن ObjIterator<T, کلیل_2> operator+(صحيح amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, کلیل_2> new_obj(*this);
			new_obj.it += amt;
			إذا (new_obj.it == list_p->end()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			أرجع new_obj;
		}

		مضمن ثابت ObjIterator<T, کلیل_2> operator++(صحيح) {
			ObjIterator<T, کلیل_2> result(*this);
			++(*this);
			أرجع result;
		}
	};

	template<typename T, typename کلیل_2 = IdString>
	بنية ObjRange
	{
		dict<کلیل_2, T> *list_p;
		صحيح *refcount_p;

		ObjRange(decltype(list_p) list_p, صحيح *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, کلیل_2> begin() { أرجع RTLIL::ObjIterator<T, کلیل_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, کلیل_2> end() { أرجع RTLIL::ObjIterator<T, کلیل_2>(); }

		size_t size() ثابت {
			أرجع list_p->size();
		}

		operator pool<T>() ثابت {
			pool<T> result;
			لكل (auto &it : *list_p)
				result.زیادکردن(it.second);
			أرجع result;
		}

		operator std::vector<T>() ثابت {
			std::vector<T> result;
			result.reserve(list_p->size());
			لكل (auto &it : *list_p)
				result.پاڵنان_back(it.second);
			أرجع result;
		}

		pool<T> to_pool() ثابت { أرجع *this; }
		std::vector<T> to_vector() ثابت { أرجع *this; }
	};
};

بنية RTLIL::Const
{
	short صحيح flags;
خاص:
	friend class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::vector<RTLIL::State>;
	تعداد class backing_tag: bool { bits, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_tag tag;
	اتحاد {
		bitvectype bits_;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_bits() ثابت { أرجع tag == backing_tag::bits; }
	bool is_str() ثابت { أرجع tag == backing_tag::string; }

	bitvectype* get_if_bits() { أرجع is_bits() ? &bits_ : عدم; }
	std::string* get_if_str() { أرجع is_str() ? &str_ : عدم; }
	ثابت bitvectype* get_if_bits() ثابت { أرجع is_bits() ? &bits_ : عدم; }
	ثابت std::string* get_if_str() ثابت { أرجع is_str() ? &str_ : عدم; }

	bitvectype& get_bits();
	std::string& get_str();
	ثابت bitvectype& get_bits() ثابت;
	ثابت std::string& get_str() ثابت;
	std::vector<RTLIL::State>& bits_internal();
	فراغ bitvectorize_internal();

عام:
	Const() : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::vector<RTLIL::State>()) {}
	Const(std::string str);
	Const(long long val); // default width is 32
	Const(long long val, صحيح width);
	Const(RTLIL::State bit, صحيح width = 1);
	Const(std::vector<RTLIL::State> bits) : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::move(bits)) {}
	Const(ثابت std::vector<bool> &bits);
	Const(ثابت RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &operator =(ثابت RTLIL::Const &other);
	~Const();

	بنية Builder
	{
		Builder() {}
		Builder(صحيح expected_width) { bits.reserve(expected_width); }
		فراغ پاڵنان_back(RTLIL::State b) { bits.پاڵنان_back(b); }
		صحيح size() ثابت { أرجع static_cast<صحيح>(bits.size()); }
		Const build() { أرجع Const(std::move(bits)); }
	خاص:
		std::vector<RTLIL::State> bits;
	};

	bool operator <(ثابت RTLIL::Const &other) ثابت;
	bool operator ==(ثابت RTLIL::Const &other) ثابت;
	bool operator !=(ثابت RTLIL::Const &other) ثابت;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::vector<RTLIL::State>& bits() { أرجع bits_internal(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	فراغ bitvectorize() ثابت { const_cast<Const*>(this)->bitvectorize_internal(); }

	bool as_bool() ثابت;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	صحيح as_int(bool is_signed = false) ثابت;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_to_int(bool is_signed = false) ثابت;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<صحيح> try_as_int(bool is_signed = false) ثابت;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	صحيح as_int_saturating(bool is_signed = false) ثابت;

	فراغ tag_bare_integer_const(ثابت std::string &بەها);

	std::string as_string(ثابت حرف* any = "-") ثابت;
	ساكن Const from_string(ثابت std::string &str);
	std::vector<RTLIL::State> to_bits() ثابت;

	std::string decode_string() ثابت;
	صحيح size() ثابت;
	bool empty() ثابت;

	فراغ append(ثابت RTLIL::Const &other);
	فراغ set(صحيح i, RTLIL::State state) {
		bits_internal()[i] = state;
	}
	فراغ resize(صحيح size, RTLIL::State fill) {
    log_assert(size >= 0 && size < RTLIL::WIDTH_LIMIT);
		bits_internal().resize(size, fill);
	}

	class const_iterator {
	خاص:
		ثابت Const* parent;
		size_t idx;

	عام:
		using iterator_category = std::bidirectional_iterator_tag;
		using بەها_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = ثابت State*;
		using reference = ثابت State&;

		const_iterator(ثابت Const& c, size_t i) : parent(&c), idx(i) {}

		State operator*() ثابت;

		const_iterator& operator++() { ++idx; أرجع *this; }
		const_iterator& operator--() { --idx; أرجع *this; }
		const_iterator operator++(صحيح) { const_iterator result(*this); ++idx; أرجع result; }
		const_iterator operator--(صحيح) { const_iterator result(*this); --idx; أرجع result; }
		const_iterator& operator+=(صحيح i) { idx += i; أرجع *this; }

		const_iterator operator+(صحيح add) {
			أرجع const_iterator(*parent, idx + add);
		}
		const_iterator operator-(صحيح sub) {
			أرجع const_iterator(*parent, idx - sub);
		}
		صحيح operator-(ثابت const_iterator& other) {
			أرجع idx - other.idx;
		}

		bool operator==(ثابت const_iterator& other) ثابت {
			أرجع idx == other.idx;
		}

		bool operator!=(ثابت const_iterator& other) ثابت {
			أرجع !(*this == other);
		}
	};

	class iterator {
	خاص:
		Const* parent;
		size_t idx;

	عام:
		class proxy {
		خاص:
			Const* parent;
			size_t idx;
		عام:
			proxy(Const* parent, size_t idx) : parent(parent), idx(idx) {}
			operator State() ثابت { أرجع (*parent)[idx]; }
			proxy& operator=(State s) { parent->set(idx, s); أرجع *this; }
			proxy& operator=(ثابت proxy& other) { parent->set(idx, (*other.parent)[other.idx]); أرجع *this; }
		};

		using iterator_category = std::bidirectional_iterator_tag;
		using بەها_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = proxy*;
		using reference = proxy;

		iterator(Const& c, size_t i) : parent(&c), idx(i) {}

		proxy operator*() ثابت { أرجع proxy(parent, idx); }
		iterator& operator++() { ++idx; أرجع *this; }
		iterator& operator--() { --idx; أرجع *this; }
		iterator operator++(صحيح) { iterator result(*this); ++idx; أرجع result; }
		iterator operator--(صحيح) { iterator result(*this); --idx; أرجع result; }
		iterator& operator+=(صحيح i) { idx += i; أرجع *this; }

		iterator operator+(صحيح add) {
			أرجع iterator(*parent, idx + add);
		}
		iterator operator-(صحيح sub) {
			أرجع iterator(*parent, idx - sub);
		}
		صحيح operator-(ثابت iterator& other) {
			أرجع idx - other.idx;
		}

		bool operator==(ثابت iterator& other) ثابت {
			أرجع idx == other.idx;
		}

		bool operator!=(ثابت iterator& other) ثابت {
			أرجع !(*this == other);
		}
	};

	const_iterator begin() ثابت {
		أرجع const_iterator(*this, 0);
	}
	const_iterator end() ثابت {
		أرجع const_iterator(*this, size());
	}
	iterator begin() {
		أرجع iterator(*this, 0);
	}
	iterator end() {
		أرجع iterator(*this, size());
	}
	State back() ثابت {
		أرجع *(end() - 1);
	}
	State front() ثابت {
		أرجع *begin();
	}
	State at(size_t i) ثابت {
		أرجع *const_iterator(*this, i);
	}
	State operator[](size_t i) ثابت {
		أرجع *const_iterator(*this, i);
	}

	bool is_fully_zero() ثابت;
	bool is_fully_ones() ثابت;
	bool is_fully_def() ثابت;
	bool is_fully_undef() ثابت;
	bool is_fully_undef_x_only() ثابت;
	bool is_onehot(صحيح *pos = nullptr) ثابت;

	RTLIL::Const extract(صحيح offset, صحيح len = 1, RTLIL::State padding = RTLIL::State::S0) ثابت;

	// find the MSB without redundant leading bits
	صحيح get_min_size(bool is_signed) ثابت;

	// compress representation to the minimum required bits
	فراغ compress(bool is_signed = false);

	std::optional<صحيح> as_int_compress(bool is_signed) ثابت;

	فراغ extu(صحيح width) {
		resize(width, RTLIL::State::S0);
	}

	فراغ exts(صحيح width) {
		resize(width, empty() ? RTLIL::State::Sx : back());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	bool has_attribute(RTLIL::IdString id) ثابت;

	فراغ set_bool_attribute(RTLIL::IdString id, bool بەها=true);
	bool get_bool_attribute(RTLIL::IdString id) ثابت;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ثابت {
		أرجع get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	فراغ set_string_attribute(RTLIL::IdString id, string بەها);
	string get_string_attribute(RTLIL::IdString id) ثابت;

	فراغ set_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &data);
	فراغ add_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &data);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ثابت;

	فراغ set_src_attribute(ثابت std::string &src) {
		set_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ثابت {
		أرجع get_string_attribute(ID::src);
	}

	فراغ set_hdlname_attribute(ثابت vector<string> &hierarchy);
	vector<string> get_hdlname_attribute() ثابت;

	فراغ set_intvec_attribute(RTLIL::IdString id, ثابت vector<صحيح> &data);
	vector<صحيح> get_intvec_attribute(RTLIL::IdString id) ثابت;
};

#ضمن "kernel/rtlil_twine_compat.h"

بنية RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

بنية RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::vector<RTLIL::State> data; // only used if wire == NULL, LSB at index 0
	صحيح width, offset;

	SigChunk() : wire(nullptr), width(0), offset(0) {}
	SigChunk(ثابت RTLIL::Const &بەها) : wire(nullptr), data(بەها.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Const &&بەها) : wire(nullptr), data(بەها.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), width(GetSize(wire)), offset(0) {}
	SigChunk(RTLIL::Wire *wire, صحيح offset, صحيح width = 1) : wire(wire), width(width), offset(offset) {}
	SigChunk(ثابت std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(صحيح val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(صحيح val, صحيح width) : SigChunk(RTLIL::Const(val, width)) {}
	SigChunk(RTLIL::State bit, صحيح width = 1) : SigChunk(RTLIL::Const(bit, width)) {}
	SigChunk(ثابت RTLIL::SigBit &bit);

	RTLIL::SigChunk extract(صحيح offset, صحيح length) ثابت;
	RTLIL::SigBit operator[](صحيح offset) ثابت;
	مضمن صحيح size() ثابت { أرجع width; }
	مضمن bool is_wire() ثابت { أرجع wire != عدم; }

	bool operator <(ثابت RTLIL::SigChunk &other) ثابت;
	bool operator ==(ثابت RTLIL::SigChunk &other) ثابت;
	bool operator !=(ثابت RTLIL::SigChunk &other) ثابت;
};

بنية RTLIL::SigBit
{
	RTLIL::Wire *wire;
	اتحاد {
		RTLIL::State data; // used if wire == NULL
		صحيح offset;        // used if wire != NULL
	};

	SigBit();
	SigBit(RTLIL::State bit);
	explicit SigBit(bool bit);
	SigBit(RTLIL::Wire *wire);
	SigBit(RTLIL::Wire *wire, صحيح offset);
	SigBit(ثابت RTLIL::SigChunk &chunk);
	SigBit(ثابت RTLIL::SigChunk &chunk, صحيح index);
	SigBit(ثابت RTLIL::SigSpec &sig);
	SigBit(ثابت RTLIL::SigBit &sigbit) = افتراضي;
	RTLIL::SigBit &operator =(ثابت RTLIL::SigBit &other) = افتراضي;

	مضمن bool is_wire() ثابت { أرجع wire != عدم; }

	bool operator <(ثابت RTLIL::SigBit &other) ثابت;
	bool operator ==(ثابت RTLIL::SigBit &other) ثابت;
	bool operator !=(ثابت RTLIL::SigBit &other) ثابت;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
	[[nodiscard]] Hasher hash_لووتکە() ثابت;
};

namespace hashlib {
	template <>
	بنية hash_ops<RTLIL::SigBit> {
		ساكن مضمن bool cmp(ثابت RTLIL::SigBit &a, ثابت RTLIL::SigBit &b) {
			أرجع a == b;
		}
		[[nodiscard]] ساكن مضمن Hasher hash(ثابت RTLIL::SigBit sb) {
			أرجع sb.hash_لووتکە();
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت RTLIL::SigBit sb, Hasher h) {
			أرجع sb.hash_into(h);
		}
	};
};

بنية RTLIL::SigSpecIterator
{
	تعريفنوع std::تێخستن_iterator_tag iterator_category;
	تعريفنوع RTLIL::SigBit بەها_type;
	تعريفنوع ptrdiff_t difference_type;
	تعريفنوع RTLIL::SigBit* pointer;
	تعريفنوع RTLIL::SigBit& reference;

	RTLIL::SigSpec *sig_p;
	صحيح index;

	مضمن RTLIL::SigBit &operator*() ثابت;
	مضمن bool operator!=(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع index != other.index; }
	مضمن bool operator==(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع index == other.index; }
	مضمن فراغ operator++() { index++; }
};

بنية RTLIL::SigSpecConstIterator
{
	تعريفنوع std::تێخستن_iterator_tag iterator_category;
	تعريفنوع RTLIL::SigBit بەها_type;
	تعريفنوع ptrdiff_t difference_type;
	تعريفنوع RTLIL::SigBit* pointer;
	تعريفنوع RTLIL::SigBit& reference;

	ثابت RTLIL::SigSpec *sig_p;
	RTLIL::SigBit bit;
	صحيح index;

	مضمن ثابت RTLIL::SigBit &operator*();
	مضمن bool operator!=(ثابت RTLIL::SigSpecConstIterator &other) ثابت { أرجع index != other.index; }
	مضمن bool operator==(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع index == other.index; }
	مضمن فراغ operator++() { index++; }
};

بنية RTLIL::SigSpec
{
خاص:
	friend class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, Extract);
	تعداد Representation : حرف {
		CHUNK,
		BITS,
	};
	// An AtomicHash is either clear or a nonzero integer.
	بنية AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ثابت AtomicHash &rhs) : atomic_(rhs.load()) {}
		AtomicHash &operator=(ثابت AtomicHash &rhs) { store(rhs.load()); أرجع *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> اقرأ() ثابت {
			Hasher::hash_t بەها = load();
			إذا (بەها == 0)
				أرجع std::nullopt;
			أرجع بەها;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		فراغ set(Hasher::hash_t بەها) ثابت {
			log_assert(بەها != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(بەها, std::memory_order_relaxed);
			log_assert(old == 0 || old == بەها);
		}
		فراغ clear() { store(0); }
	خاص:
		صحيح load() ثابت { أرجع atomic_.load(std::memory_order_relaxed); }
		فراغ store(Hasher::hash_t بەها) ثابت {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(بەها, std::memory_order_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	اتحاد {
		RTLIL::SigChunk chunk_;
		std::vector<RTLIL::SigBit> bits_; // LSB at index 0
	};

	فراغ init_empty_bits() {
		rep_ = BITS;
		new (&bits_) std::vector<RTLIL::SigBit>;
	}

	فراغ unpack();
	مضمن فراغ inline_unpack() {
		إذا (rep_ == CHUNK)
			unpack();
	}
	فراغ try_repack();

	Hasher::hash_t updhash() ثابت;
	فراغ destroy() {
		إذا (rep_ == CHUNK)
			chunk_.~SigChunk();
		وإلا
			bits_.~vector();
	}
	friend بنية Chunks;

عام:
	SigSpec() { init_empty_bits(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ثابت SigSpec &بەها) : rep_(بەها.rep_), hash_(بەها.hash_) {
		إذا (بەها.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(بەها.chunk_);
		وإلا
			new (&bits_) std::vector<RTLIL::SigBit>(بەها.bits_);
	}
	SigSpec(SigSpec &&بەها) : rep_(بەها.rep_), hash_(بەها.hash_) {
		إذا (بەها.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(بەها.chunk_));
		وإلا
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(بەها.bits_));
	}
	SigSpec(ثابت RTLIL::Const &بەها);
	SigSpec(RTLIL::Const &&بەها);
	SigSpec(ثابت RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, صحيح offset, صحيح width = 1);
	SigSpec(ثابت std::string &str);
	SigSpec(صحيح val, صحيح width = 32);
	SigSpec(RTLIL::State bit, صحيح width = 1);
	SigSpec(ثابت RTLIL::SigBit &bit, صحيح width = 1);
	SigSpec(ثابت std::vector<RTLIL::SigChunk> &chunks);
	SigSpec(ثابت std::vector<RTLIL::SigBit> &bits);
	SigSpec(ثابت pool<RTLIL::SigBit> &bits);
	SigSpec(ثابت std::set<RTLIL::SigBit> &bits);
	explicit SigSpec(bool bit);
	~SigSpec() {
		destroy();
	}

	SigSpec &operator=(ثابت SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		إذا (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		وإلا
			new (&bits_) std::vector<RTLIL::SigBit>(rhs.bits_);
		أرجع *this;
	}
	SigSpec &operator=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		إذا (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		وإلا
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(rhs.bits_));
		أرجع *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	بنية Chunks {
		Chunks(ثابت SigSpec &spec) : spec(spec) {}
		بنية const_iterator {
			using iterator_category = std::forward_iterator_tag;
			using بەها_type = ثابت SigChunk &;
			using difference_type = std::ptrdiff_t;
			using pointer = ثابت SigChunk *;
			using reference = ثابت SigChunk &;

			ثابت SigSpec &spec;
			صحيح bit_index;
			SigChunk chunk;

			const_iterator(ثابت SigSpec &spec) : spec(spec) {
				bit_index = 0;
				إذا (spec.rep_ == BITS)
					next_chunk_bits();
			}
			تعداد End { END };
			const_iterator(ثابت SigSpec &spec, End) : spec(spec) {
				bit_index = spec.size();
			}
			فراغ next_chunk_bits();

			ثابت SigChunk &operator*() {
				إذا (spec.rep_ == CHUNK)
					أرجع spec.chunk_;
				أرجع chunk;
			};
			ثابت SigChunk *operator->() { أرجع &**this; }
			const_iterator &operator++() {
				bit_index += (**this).width;
				إذا (spec.rep_ == BITS)
					next_chunk_bits();
				أرجع *this;
			}
			bool operator==(ثابت const_iterator &rhs) ثابت { أرجع bit_index == rhs.bit_index; }
			bool operator!=(ثابت const_iterator &rhs) ثابت { أرجع !(*this == rhs); }
		};
		const_iterator begin() ثابت { أرجع const_iterator(spec); }
		const_iterator end() ثابت {
			const_iterator it(spec, const_iterator::END);
			أرجع it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rbegin() {
			ensure_chunk_vector();
			أرجع chunk_vector.rbegin();
		}
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rend() {
			ensure_chunk_vector();
			أرجع chunk_vector.rend();
		}
		صحيح size() {
			ensure_chunk_vector();
			أرجع chunk_vector.size();
		}
		صحيح size() ثابت {
			أرجع std::distance(begin(), end());
		}
		ثابت SigChunk &at(صحيح index) {
			ensure_chunk_vector();
			أرجع chunk_vector.at(index);
		}
		operator ثابت std::vector<RTLIL::SigChunk>&() {
			ensure_chunk_vector();
			أرجع chunk_vector;
		}
	خاص:
		فراغ ensure_chunk_vector() {
			إذا (spec.size() > 0 && chunk_vector.empty()) {
				لكل (ثابت RTLIL::SigChunk &c : *this)
					chunk_vector.پاڵنان_back(c);
			}
		}
		ثابت SigSpec &spec;
		std::vector<RTLIL::SigChunk> chunk_vector;
	};
	friend بنية Chunks::const_iterator;

	مضمن Chunks chunks() ثابت { أرجع {*this}; }
	مضمن ثابت SigSpec &bits() ثابت { أرجع *this; }

	مضمن صحيح size() ثابت { أرجع rep_ == CHUNK ? chunk_.width : GetSize(bits_); }
	مضمن bool empty() ثابت { أرجع size() == 0; };

	مضمن RTLIL::SigBit &operator[](صحيح index) { inline_unpack(); hash_.clear(); أرجع bits_.at(index); }
	مضمن RTLIL::SigBit operator[](صحيح index) ثابت {
		إذا (rep_ == CHUNK) {
			إذا (index < 0 || index >= chunk_.width)
				throw std::out_of_range("SigSpec::operator[]");
			إذا (chunk_.wire)
				أرجع RTLIL::SigBit(chunk_.wire, chunk_.offset + index);
			أرجع RTLIL::SigBit(chunk_.data[index]);
		}
		أرجع bits_.at(index);
	}

	مضمن RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = 0; أرجع it; }
	مضمن RTLIL::SigSpecIterator end() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = size(); أرجع it; }

	مضمن RTLIL::SigSpecConstIterator begin() ثابت { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = 0; أرجع it; }
	مضمن RTLIL::SigSpecConstIterator end() ثابت { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = size(); أرجع it; }

	فراغ sort();
	فراغ sort_and_unify();

	فراغ replace(ثابت RTLIL::SigSpec &pattern, ثابت RTLIL::SigSpec &with);
	فراغ replace(ثابت RTLIL::SigSpec &pattern, ثابت RTLIL::SigSpec &with, RTLIL::SigSpec *other) ثابت;

	فراغ replace(ثابت dict<RTLIL::SigBit, RTLIL::SigBit> &rules);
	فراغ replace(ثابت dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ثابت;

	فراغ replace(ثابت std::map<RTLIL::SigBit, RTLIL::SigBit> &rules);
	فراغ replace(ثابت std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ثابت;

	فراغ replace(صحيح offset, ثابت RTLIL::SigSpec &with);

	فراغ remove(ثابت RTLIL::SigSpec &pattern);
	فراغ remove(ثابت RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) ثابت;
	فراغ remove2(ثابت RTLIL::SigSpec &pattern, RTLIL::SigSpec *other);

	فراغ remove(ثابت pool<RTLIL::SigBit> &pattern);
	فراغ remove(ثابت pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) ثابت;
	فراغ remove2(ثابت pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	فراغ remove2(ثابت std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	فراغ remove2(ثابت pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other);

	فراغ remove(صحيح offset, صحيح length = 1);
	فراغ remove_const();

	RTLIL::SigSpec extract(ثابت RTLIL::SigSpec &pattern, ثابت RTLIL::SigSpec *other = عدم) ثابت;
	RTLIL::SigSpec extract(ثابت pool<RTLIL::SigBit> &pattern, ثابت RTLIL::SigSpec *other = عدم) ثابت;
	RTLIL::SigSpec extract(صحيح offset, صحيح length = 1) ثابت;
	RTLIL::SigSpec extract_end(صحيح offset) ثابت { أرجع extract(offset, size() - offset); }

	فراغ rewrite_wires(std::function<فراغ(RTLIL::Wire*& wire)> rewrite);

	RTLIL::SigBit lsb() ثابت { log_assert(size()); أرجع (*this)[0]; };
	RTLIL::SigBit msb() ثابت { log_assert(size()); أرجع (*this)[size() - 1]; };
	RTLIL::SigBit front() ثابت { أرجع (*this)[0]; }
	RTLIL::SigBit back() ثابت { أرجع (*this)[size() - 1]; }

	فراغ append(ثابت RTLIL::SigSpec &إشارة);
	مضمن فراغ append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	مضمن فراغ append(ثابت RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	مضمن فراغ append(ثابت RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	فراغ append(ثابت RTLIL::SigBit &bit);
	مضمن فراغ append(RTLIL::State state) { append(RTLIL::SigBit(state)); }
	مضمن فراغ append(bool bool_) { append(RTLIL::SigBit(bool_)); }

	فراغ extend_u0(صحيح width, bool is_signed = false);

	RTLIL::SigSpec repeat(صحيح ژمارە) ثابت;

	فراغ reverse() { inline_unpack(); std::reverse(bits_.begin(), bits_.end()); }

	bool operator <(ثابت RTLIL::SigSpec &other) ثابت;
	bool operator ==(ثابت RTLIL::SigSpec &other) ثابت;
	مضمن bool operator !=(ثابت RTLIL::SigSpec &other) ثابت { أرجع !(*this == other); }

	bool is_wire() ثابت;
	bool is_chunk() ثابت;
	مضمن bool is_bit() ثابت { أرجع size() == 1; }

	bool known_driver() ثابت;

	bool is_fully_const() ثابت;
	bool is_fully_zero() ثابت;
	bool is_fully_ones() ثابت;
	bool is_fully_def() ثابت;
	bool is_fully_undef() ثابت;
	bool has_const() ثابت;
	bool has_const(State state) ثابت;
	bool has_marked_bits() ثابت;
	bool is_onehot(صحيح *pos = nullptr) ثابت;

	bool as_bool() ثابت;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	صحيح as_int(bool is_signed = false) ثابت;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_to_int(bool is_signed = false) ثابت;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<صحيح> try_as_int(bool is_signed = false) ثابت;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	صحيح as_int_saturating(bool is_signed = false) ثابت;

	std::string as_string() ثابت;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() ثابت;
	RTLIL::Const as_const() ثابت;
	RTLIL::Wire *as_wire() ثابت;
	RTLIL::SigChunk as_chunk() ثابت;
	RTLIL::SigBit as_bit() ثابت;

	bool match(ثابت حرف* pattern) ثابت;

	std::set<RTLIL::SigBit> to_sigbit_set() ثابت;
	pool<RTLIL::SigBit> to_sigbit_pool() ثابت;
	std::vector<RTLIL::SigBit> to_sigbit_vector() ثابت;
	std::map<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت;
	dict<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت;

	ساكن bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	ساكن bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	ساكن bool parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	operator std::vector<RTLIL::SigChunk>() ثابت;
	operator std::vector<RTLIL::SigBit>() ثابت { أرجع to_sigbit_vector(); }
	ثابت RTLIL::SigBit &at(صحيح offset, ثابت RTLIL::SigBit &defval) { أرجع offset < size() ? (*this)[offset] : defval; }
	RTLIL::SigBit& at(صحيح offset) { أرجع (*this)[offset]; }
	RTLIL::SigBit at(صحيح offset) ثابت { أرجع (*this)[offset]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		Hasher::hash_t val;
		إذا (std::optional<Hasher::hash_t> current = hash_.اقرأ())
			val = *current;
		وإلا
			val = updhash();
		h.eat(val);
		أرجع h;
	}

#ifndef NDEBUG
	فراغ check(ثابت Module *mod = nullptr) ثابت;
#وإلا
	فراغ check(ثابت Module *mod = nullptr) ثابت { (فراغ)mod; }
#endif
};

بنية RTLIL::Selection
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
	bool boxed_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت;

	// optimizes this selection for the given design by:
	// - removing non-existent modules and members, any boxed modules and
	//   their members (if selection does not include boxes), and any
	//   partially selected modules with no selected members;
	// - marking partially selected modules as wholly selected if all
	//   members of that module are selected; and
	// - marking selection as a complete_selection if all modules in the
	//   given design are selected, or a full_selection if it does not
	//   include boxes.
	فراغ optimize(RTLIL::Design *design);

	// checks if selection covers full design (may or may not include
	// boxed-modules)
	bool selects_all() ثابت {
		أرجع full_selection || complete_selection;
	}

	// add whole module to this selection
	template<typename T1> فراغ select(T1 *module) {
		إذا (!selects_all() && selected_modules.ژماردن(module->name) == 0) {
			IdString name = module->name;
			selected_modules.زیادکردن(name);
			selected_members.erase(name);
			إذا (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	template<typename T1, typename T2> فراغ select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	template<typename T1> فراغ select(T1 *module, RTLIL::IdString member) {
		إذا (!selects_all() && selected_modules.ژماردن(module->name.ref()) == 0) {
			selected_members[module->name.ref()].زیادکردن(member);
			إذا (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool empty() ثابت {
		أرجع !selects_all() && selected_modules.empty() && selected_members.empty();
	}

	// clear this selection, leaving it empty
	فراغ clear();

	// create a new selection which is empty
	ساكن Selection EmptySelection(RTLIL::Design *design = nullptr) { أرجع Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	ساكن Selection FullSelection(RTLIL::Design *design = nullptr) { أرجع Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	ساكن Selection CompleteSelection(RTLIL::Design *design = nullptr) { أرجع Selection(true, true, design); };
};

بنية RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

	Monitor() {
		ساكن unsigned صحيح hashidx_ژماردن = 123456789;
		hashidx_ژماردن = mkhash_xorshift(hashidx_ژماردن);
		hashidx_ = hashidx_ژماردن;
	}

	virtual ~Monitor() { }
	virtual فراغ notify_module_add(RTLIL::Module*) { }
	virtual فراغ notify_module_del(RTLIL::Module*) { }
	virtual فراغ notify_connect(RTLIL::Cell*, RTLIL::IdString, ثابت RTLIL::SigSpec&, ثابت RTLIL::SigSpec&) { }
	virtual فراغ notify_connect(RTLIL::Module*, ثابت RTLIL::SigSig&) { }
	virtual فراغ notify_connect(RTLIL::Module*, ثابت std::vector<RTLIL::SigSig>&) { }
	virtual فراغ notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
بنية define_map_t;

template<typename N>
مضمن constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::Leaf> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ثابت حرف*> || std::is_same_v<std::decay_t<N>, حرف*>;

#define YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, صحيح> = 0

template<typename N>
مضمن constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ثابت حرف *> || std::is_same_v<std::decay_t<N>, حرف *>;

#define YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, صحيح> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#define YS_NAME_FWD_POOL(_func, _pool) \
	template<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(N name, Rest&&... rest) \
		{ أرجع _func(_pool.add(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#define YS_NAME_FWD_2ND_POOL(_func, _pool) \
	template<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(T &&first, N name) \
		{ أرجع _func(std::forward<T>(first), _pool.add(std::move(name))); }

#define YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#define YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

بنية RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	فراغ bufNormalize(bool enable=true);

	صحيح refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

خاص:
	TwinePool twines_;

عام:
	TwinePool &twines() { أرجع twines_; }
	ثابت TwinePool &twines() ثابت { أرجع twines_; }

	فراغ absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	size_t gc_twines();

	std::vector<std::unique_ptr<AST::Ast_گرێ>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_defines;

	std::vector<RTLIL::Selection> selection_کەڵەکە;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_active_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ثابت RTLIL::Module *module(RTLIL::IdString name) ثابت;
	template<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ أرجع module(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Module *module(N name) ثابت
		{ أرجع module(twines().find(std::string_view(name))); }
	RTLIL::Module *لووتکە_module() ثابت;

	bool has(RTLIL::IdString id) ثابت {
		twines_.check_owned(id);
		أرجع modules_.ژماردن(id) != 0;
	}
	template<typename N, YS_NAME_STRING(N)> bool has(N name) ثابت
		{ أرجع has(twines().find(std::string_view(name))); }

	فراغ add(RTLIL::Module *module);

	RTLIL::Module *addModule(RTLIL::IdString name);
	YS_NAME_FWD_POOL(addModule, twines())
	فراغ remove(RTLIL::Module *module);
	فراغ rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	فراغ scratchpad_unset(ثابت std::string &varname);

	فراغ scratchpad_set_int(ثابت std::string &varname, صحيح بەها);
	فراغ scratchpad_set_bool(ثابت std::string &varname, bool بەها);
	فراغ scratchpad_set_string(ثابت std::string &varname, std::string بەها);

	صحيح scratchpad_get_int(ثابت std::string &varname, صحيح default_بەها_2 = 0) ثابت;
	bool scratchpad_get_bool(ثابت std::string &varname, bool default_بەها_2 = false) ثابت;
	std::string scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_بەها_2 = std::string()) ثابت;

	فراغ sort();
	فراغ sort_modules();
	فراغ check();
	فراغ optimize();

	فراغ clone_into(RTLIL::Design *dst) ثابت;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ثابت;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::Module *mod) ثابت;

	// push the given selection to the selection stack
	فراغ پاڵنان_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	فراغ پاڵنان_empty_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	فراغ پاڵنان_full_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	فراغ پاڵنان_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	فراغ دەرهێنان_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		أرجع selection_کەڵەکە.back();
	}

	// get the current selection
	ثابت RTLIL::Selection &selection() ثابت {
		أرجع selection_کەڵەکە.back();
	}

	// is the current selection a full selection (no boxes)
	bool full_selection() ثابت {
		أرجع selection().full_selection;
	}

	// is the given module in the current selection
	template<typename T1> bool selected(T1 *module) ثابت {
		أرجع selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	template<typename T1, typename T2> bool selected(T1 *module, T2 *member) ثابت {
		أرجع selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	template<typename T1> فراغ select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	template<typename T1, typename T2> فراغ select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::vector<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) ثابت;

	// returns all selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_modules() ثابت { أرجع selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::vector<RTLIL::Module*> selected_unboxed_modules() ثابت { أرجع selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_modules_warn() ثابت { أرجع selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::vector<RTLIL::Module*> selected_whole_modules() ثابت { أرجع selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_whole_modules() ثابت { أرجع selected_modules(SELECT_WHOLE_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::vector<RTLIL::Module*> selected_whole_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ثابت { أرجع selected_modules(SELECT_WHOLE_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules() ثابت { أرجع selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules_warn() ثابت { أرجع selected_modules(SELECT_WHOLE_WARN, SB_UNBOXED_WARN); }

	ساكن std::map<unsigned صحيح, RTLIL::Design*> *get_all_designs(فراغ);

	std::string to_rtlil_str(bool only_selected = true) ثابت;
};

بنية RTLIL::Module : عام RTLIL::AttrObject
{
	friend بنية RTLIL::Cell;
	friend بنية RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	فراغ add(RTLIL::Wire *wire);
	فراغ add(RTLIL::Cell *cell);
	فراغ add(RTLIL::Process *process);

عام:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ثابت { أرجع design->twines(); }

	صحيح refcount_wires_;
	صحيح refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::vector<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_بەهاکان;
	dict<RTLIL::IdString, RTLIL::Memory*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	virtual ~Module();
	virtual RTLIL::IdString derive(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	virtual RTLIL::IdString derive(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Const> &parameters, ثابت dict<RTLIL::IdString, RTLIL::Module*> &interfaces, ثابت dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	virtual size_t ژماردن_id(RTLIL::IdString id);
	virtual فراغ expand_interfaces(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	virtual bool reprocess_if_necessary(RTLIL::Design *design);

	virtual فراغ sort();
	virtual فراغ check();
	virtual فراغ optimize();
	virtual فراغ makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ثابت {
		أرجع get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	فراغ connect(ثابت RTLIL::SigSig &conn);
	فراغ connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs);
	فراغ new_connections(ثابت std::vector<RTLIL::SigSig> &new_conn);
	ثابت std::vector<RTLIL::SigSig> &connections() ثابت;

	std::vector<RTLIL::IdString> ports;
	فراغ fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_queue;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_port_queue;
	pool<RTLIL::Wire *> buf_norm_wire_queue;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_index;
	فراغ bufNormalize();

	template<typename T> فراغ rewrite_sigspecs(T &functor);
	template<typename T> فراغ rewrite_sigspecs2(T &functor);
	فراغ cloneInto(RTLIL::Module *new_mod) ثابت;
	virtual RTLIL::Module *clone() ثابت;
	virtual RTLIL::Module *clone(RTLIL::Design *dst) ثابت;
	virtual RTLIL::Module *clone(RTLIL::Design *dst, IdString target_name) ثابت;

	bool has_memories() ثابت;
	bool has_processes() ثابت;

	bool has_memories_warn() ثابت;
	bool has_processes_warn() ثابت;

	bool is_selected() ثابت;
	bool is_selected_whole() ثابت;

	std::vector<RTLIL::Wire*> selected_wires() ثابت;
	std::vector<RTLIL::Cell*> selected_cells() ثابت;
	std::vector<RTLIL::Memory*> selected_memories() ثابت;
	std::vector<RTLIL::Process*> selected_processes() ثابت;
	std::vector<RTLIL::SelectedMember> selected_members() ثابت;

	template<typename T> bool selected(T *member) ثابت {
		أرجع design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		إذا (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		أرجع it == wires_.end() ? nullptr : it->second;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		إذا (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		أرجع it == cells_.end() ? nullptr : it->second;
	}

	ثابت RTLIL::Wire* wire(RTLIL::IdString id) ثابت{
		إذا (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		أرجع it == wires_.end() ? nullptr : it->second;
	}
	ثابت RTLIL::Cell* cell(RTLIL::IdString id) ثابت {
		إذا (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		أرجع it == cells_.end() ? nullptr : it->second;
	}

	template<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ أرجع wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ أرجع cell(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Wire *wire(N name) ثابت
		{ أرجع wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Cell *cell(N name) ثابت
		{ أرجع cell(twines().find(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { أرجع RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	صحيح wires_size() ثابت { أرجع wires_.size(); }
	RTLIL::Wire* wire_at(صحيح index) ثابت { أرجع wires_.element(index)->second; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { أرجع RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	صحيح cells_size() ثابت { أرجع cells_.size(); }
	RTLIL::Cell* cell_at(صحيح index) ثابت { أرجع cells_.element(index)->second; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	فراغ remove(ثابت pool<RTLIL::Wire*> &wires);
	فراغ remove(RTLIL::Cell *cell);
	فراغ remove(RTLIL::Memory *memory);
	فراغ remove(RTLIL::Process *process);

	فراغ rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	فراغ rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	فراغ rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	فراغ swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2);
	فراغ swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, صحيح &index);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *addWire(RTLIL::IdString name, صحيح width = 1);
	RTLIL::Wire *addWire(RTLIL::IdString name, ثابت RTLIL::Wire *other);
	YS_NAME_FWD(addWire)

	RTLIL::Cell *addCell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *addCell(RTLIL::IdString name, ثابت RTLIL::Cell *other);
	YS_NAME_FWD(addCell)
	template<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *addCell(RTLIL::IdString name, T type) { أرجع addCell(name, design->twines().add(std::move(type))); }

	RTLIL::Memory *addMemory(RTLIL::IdString name);
	YS_NAME_FWD(addMemory)
	RTLIL::Memory *addMemory(RTLIL::IdString name, ثابت RTLIL::Memory *other);

	RTLIL::Process *addProcess(RTLIL::IdString name);
	YS_NAME_FWD(addProcess)
	RTLIL::Process *addProcess(RTLIL::IdString name, ثابت RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* addNot     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addPos     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addBuf     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addBarrier (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addNeg     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* addAnd  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addOr   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addXor  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addXnor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* addReduceAnd  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addReduceOr   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addReduceXor  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addReduceXnor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addReduceBool (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* addShl    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addShr    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addSshl   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addSshr   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addShift  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addShiftx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* addLt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addLe  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addEq  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addNe  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addEqx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addNex (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addGe  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addGt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* addAdd (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addSub (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addMul (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	// truncating division
	RTLIL::Cell* addDiv (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	// truncating modulo
	RTLIL::Cell* addMod (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addDivFloor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addModFloor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addPow (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* addFa (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_x, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* addLogicNot (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addLogicAnd (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* addLogicOr  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* addMux  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addPmux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addBmux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addDemux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* addBweqx  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addBwmux  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* addSlice  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const offset, ثابت std::string &src = "");
	RTLIL::Cell* addConcat (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addLut    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src = "");
	RTLIL::Cell* addTribuf (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addAssert (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* addAssume (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* addLive   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* addFair   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* addCover  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* addEquiv  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* addSr    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr, ثابت RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addFf    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");
	RTLIL::Cell* addDff   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d,   ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addDffe  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addDffsr (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addDffsre (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addAdff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_بەها, bool clk_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addAdffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_بەها, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addAldff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addAldffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addSdff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_بەها, bool clk_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addSdffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_بەها, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addSdffce (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_بەها, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addDlatch (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addAdlatch (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_بەها, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addDlatchsr (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");

	RTLIL::Cell* addBufGate    (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addNotGate    (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addAndGate    (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addNandGate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addOrGate     (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addNorGate    (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addXorGate    (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addXnorGate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addAndnotGate (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addOrnotGate  (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addMuxGate    (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_s, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addNmuxGate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_s, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addAoi3Gate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_c, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addOai3Gate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_c, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addAoi4Gate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_c, ثابت RTLIL::SigBit &sig_d, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* addOai4Gate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_c, ثابت RTLIL::SigBit &sig_d, ثابت RTLIL::SigBit &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* addSrGate     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
			ثابت RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addFfGate     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");
	RTLIL::Cell* addDffGate    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addDffeGate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addDffsrGate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addDffsreGate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addAdffGate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_بەها = false, bool clk_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addAdffeGate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_بەها = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addAldffGate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addAldffeGate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addSdffGate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_بەها = false, bool clk_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addSdffeGate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_بەها = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addSdffceGate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_بەها = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addDlatchGate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addAdlatchGate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_بەها = false, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* addDlatchsrGate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_set, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");

	RTLIL::Cell* addAnyinit(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Shift  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec Add (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت std::string &src = "");

	RTLIL::SigBit BufGate    (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت std::string &src = "");
	RTLIL::SigBit NotGate    (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت std::string &src = "");
	RTLIL::SigBit AndGate    (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت std::string &src = "");
	RTLIL::SigBit NandGate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت std::string &src = "");
	RTLIL::SigBit OrGate     (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت std::string &src = "");
	RTLIL::SigBit NorGate    (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت std::string &src = "");
	RTLIL::SigBit XorGate    (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت std::string &src = "");
	RTLIL::SigBit XnorGate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت std::string &src = "");
	RTLIL::SigBit AndnotGate (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت std::string &src = "");
	RTLIL::SigBit OrnotGate  (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت std::string &src = "");
	RTLIL::SigBit MuxGate    (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_s, ثابت std::string &src = "");
	RTLIL::SigBit NmuxGate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_s, ثابت std::string &src = "");
	RTLIL::SigBit Aoi3Gate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_c, ثابت std::string &src = "");
	RTLIL::SigBit Oai3Gate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_c, ثابت std::string &src = "");
	RTLIL::SigBit Aoi4Gate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_c, ثابت RTLIL::SigBit &sig_d, ثابت std::string &src = "");
	RTLIL::SigBit Oai4Gate   (RTLIL::IdString name, ثابت RTLIL::SigBit &sig_a, ثابت RTLIL::SigBit &sig_b, ثابت RTLIL::SigBit &sig_c, ثابت RTLIL::SigBit &sig_d, ثابت std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, صحيح width = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, صحيح width = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, صحيح width = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, صحيح width = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ثابت std::string &src = "");

	RTLIL::SigSpec SetTag          (RTLIL::IdString name, ثابت std::string &tag, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src = "");
	RTLIL::Cell*   addSetTag       (RTLIL::IdString name, ثابت std::string &tag, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::SigSpec GetTag          (RTLIL::IdString name, ثابت std::string &tag, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src = "");
	RTLIL::Cell*   addOverwriteTag (RTLIL::IdString name, ثابت std::string &tag, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src = "");
	RTLIL::SigSpec OriginalTag     (RTLIL::IdString name, ثابت std::string &tag, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src = "");
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

	std::string to_rtlil_str() ثابت;
#ifdef YOSYS_ENABLE_PYTHON
	ساكن std::map<unsigned صحيح, RTLIL::Module*> *get_all_modules(فراغ);
#endif
};

بنية RTLIL::Wire : عام RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	// use module->addWire() and module->remove() to create or destroy wires
	friend بنية RTLIL::Module;
	Wire();
	~Wire();

	friend بنية RTLIL::Design;
	friend بنية RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driverPort_;

عام:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	فراغ operator=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	صحيح width, start_offset, port_id;
	bool port_تێخستن, port_output, upto, is_signed;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	bool known_driver() ثابت { أرجع driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ثابت    { log_assert(driverCell_); أرجع driverCell_; };
	RTLIL::IdString driverPort() ثابت { log_assert(driverCell_); أرجع driverPort_; };

	صحيح from_hdl_index(صحيح hdl_index) {
		صحيح zero_index = hdl_index - start_offset;
		صحيح rtlil_index = upto ? width - 1 - zero_index : zero_index;
		أرجع rtlil_index >= 0 && rtlil_index < width ? rtlil_index : INT_MIN;
	}

	صحيح to_hdl_index(صحيح rtlil_index) {
		إذا (rtlil_index < 0 || rtlil_index >= width)
			أرجع INT_MIN;
		صحيح zero_index = upto ? width - 1 - rtlil_index : rtlil_index;
		أرجع zero_index + start_offset;
	}

	std::string to_rtlil_str() ثابت;

#ifdef YOSYS_ENABLE_PYTHON
	ساكن std::map<unsigned صحيح, RTLIL::Wire*> *get_all_wires(فراغ);
#endif
};

مضمن صحيح GetSize(RTLIL::Wire *wire) {
	أرجع wire->width;
}

بنية RTLIL::Memory : عام RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

	Memory();
	~Memory();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	RTLIL::MemoryNameId name;

	صحيح width, start_offset, size;
#ifdef YOSYS_ENABLE_PYTHON
	ساكن std::map<unsigned صحيح, RTLIL::Memory*> *get_all_memorys(فراغ);
#endif

	std::string to_rtlil_str() ثابت;
};

بنية RTLIL::Cell : عام RTLIL::AttrObject
{
خاص:
	فراغ initIndex();

	bool bufnorm_handle_setPort(IdString portname, RTLIL::SigSpec &إشارة, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
عام:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	// use module->addCell() and module->remove() to create or destroy cells
	friend بنية RTLIL::Module;
	Cell();
	~Cell();

عام:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	فراغ operator=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool hasPort(RTLIL::IdString portname) ثابت;
	فراغ unsetPort(RTLIL::IdString portname);
	فراغ setPort(RTLIL::IdString portname, RTLIL::SigSpec إشارة);
	template<typename N, YS_NAME_STRING(N)> فراغ setPort(N portname, RTLIL::SigSpec إشارة)
		{ setPort(module->twines().add(std::move(portname)), std::move(إشارة)); }
	ثابت RTLIL::SigSpec &getPort(RTLIL::IdString portname) ثابت;
	ثابت dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ثابت;

	bool known() ثابت;
	bool تێخستن(RTLIL::IdString portname) ثابت;
	bool output(RTLIL::IdString portname) ثابت;
	PortDir port_dir(RTLIL::IdString portname) ثابت;

	bool hasParam(RTLIL::IdString paramname) ثابت;
	فراغ unsetParam(RTLIL::IdString paramname);
	فراغ setParam(RTLIL::IdString paramname, RTLIL::Const بەها);
	ثابت RTLIL::Const &getParam(RTLIL::IdString paramname) ثابت;

	template<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ثابت
		{ أرجع hasParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> فراغ unsetParam(N name)
		{ unsetParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> فراغ setParam(N name, RTLIL::Const بەها)
		{ setParam(module->twines().add(std::move(name)), std::move(بەها)); }
	template<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Const &getParam(N name) ثابت
		{ أرجع getParam(module->twines().find(std::string_view(name))); }

	فراغ sort();
	فراغ check();
	فراغ fixup_parameters(bool set_a_signed = false, bool set_b_signed = false);

	bool has_keep_attr() ثابت;

	template<typename T> فراغ rewrite_sigspecs(T &functor);
	template<typename T> فراغ rewrite_sigspecs2(T &functor);

#ifdef YOSYS_ENABLE_PYTHON
	ساكن std::map<unsigned صحيح, RTLIL::Cell*> *get_all_cells(فراغ);
#endif

	bool has_memid() ثابت;
	bool is_mem_cell() ثابت;
	bool is_builtin_ff() ثابت;

	std::string to_rtlil_str() ثابت;
};

بنية RTLIL::CaseRule : عام RTLIL::AttrObject
{
	std::vector<RTLIL::SigSpec> compare;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::SwitchRule*> switches;

	~CaseRule();

	bool empty() ثابت;

	template<typename T> فراغ rewrite_sigspecs(T &functor);
	template<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::CaseRule *clone() ثابت;
};

بنية RTLIL::SwitchRule : عام RTLIL::AttrObject
{
	RTLIL::SigSpec إشارة;
	std::vector<RTLIL::CaseRule*> cases;

	~SwitchRule();

	bool empty() ثابت;

	template<typename T> فراغ rewrite_sigspecs(T &functor);
	template<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::SwitchRule *clone() ثابت;
};

بنية RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec address;
	RTLIL::SigSpec data;
	RTLIL::SigSpec enable;
	RTLIL::Const priority_mask;
};

بنية RTLIL::SyncRule
{
	RTLIL::SyncType type;
	RTLIL::SigSpec إشارة;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::MemWriteAction> mem_write_actions;

	template<typename T> فراغ rewrite_sigspecs(T &functor);
	template<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::SyncRule *clone() ثابت;
};

بنية RTLIL::Process : عام RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	// use module->addProcess() and module->remove() to create or destroy processes
	friend بنية RTLIL::Module;
	Process();
	~Process();

عام:
	RTLIL::Module *module;
	RTLIL::CaseRule ڕەگ_case;
	std::vector<RTLIL::SyncRule*> syncs;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	template<typename T> فراغ rewrite_sigspecs(T &functor);
	template<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ثابت;

	std::string to_rtlil_str() ثابت;
};

مضمن RTLIL::SigBit::SigBit() : wire(عدم), data(RTLIL::State::S0) { }
مضمن RTLIL::SigBit::SigBit(RTLIL::State bit) : wire(عدم), data(bit) { }
مضمن RTLIL::SigBit::SigBit(bool bit) : wire(عدم), data(bit ? State::S1 : State::S0) { }
مضمن RTLIL::SigBit::SigBit(RTLIL::Wire *wire) : wire(wire), offset(0) { log_assert(wire && wire->width == 1); }
مضمن RTLIL::SigBit::SigBit(RTLIL::Wire *wire, صحيح offset) : wire(wire), offset(offset) { log_assert(wire != nullptr); }
مضمن RTLIL::SigBit::SigBit(ثابت RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.width == 1); إذا (wire) offset = chunk.offset; وإلا data = chunk.data[0]; }
مضمن RTLIL::SigBit::SigBit(ثابت RTLIL::SigChunk &chunk, صحيح index) : wire(chunk.wire) { إذا (wire) offset = chunk.offset + index; وإلا data = chunk.data[index]; }

مضمن bool RTLIL::SigBit::operator<(ثابت RTLIL::SigBit &other) ثابت {
	إذا (wire == other.wire)
		أرجع wire ? (offset < other.offset) : (data < other.data);
	إذا (wire != nullptr && other.wire != nullptr)
		أرجع wire->name < other.wire->name;
	أرجع (wire != nullptr) < (other.wire != nullptr);
}

مضمن bool RTLIL::SigBit::operator==(ثابت RTLIL::SigBit &other) ثابت {
	أرجع (wire == other.wire) && (wire ? (offset == other.offset) : (data == other.data));
}

مضمن bool RTLIL::SigBit::operator!=(ثابت RTLIL::SigBit &other) ثابت {
	أرجع (wire != other.wire) || (wire ? (offset != other.offset) : (data != other.data));
}

مضمن Hasher RTLIL::SigBit::hash_into(Hasher h) ثابت {
	إذا (wire) {
		h.eat(offset);
		h.eat(wire->name);
		أرجع h;
	}
	h.eat(data);
	أرجع h;
}


مضمن Hasher RTLIL::SigBit::hash_لووتکە() ثابت {
	Hasher h;
	إذا (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.bits();
		h.force(hashlib::legacy::djb2_add(n, offset));
		أرجع h;
	}
	h.force(data);
	أرجع h;
}

مضمن RTLIL::SigBit &RTLIL::SigSpecIterator::operator*() ثابت {
	أرجع (*sig_p)[index];
}

مضمن ثابت RTLIL::SigBit &RTLIL::SigSpecConstIterator::operator*() {
	bit = (*sig_p)[index];
	أرجع bit;
}

مضمن RTLIL::SigBit::SigBit(ثابت RTLIL::SigSpec &sig) {
	log_assert(sig.size() == 1);
	auto it = sig.chunks().begin();
	*this = SigBit(*it);
}

template<typename T>
فراغ RTLIL::Module::rewrite_sigspecs(T &functor)
{
	لكل (auto &it : cells_)
		it.second->rewrite_sigspecs(functor);
	لكل (auto &it : processes)
		it.second->rewrite_sigspecs(functor);
	لكل (auto &it : connections_) {
		functor(it.first);
		functor(it.second);
	}
}

template<typename T>
فراغ RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	لكل (auto &it : cells_)
		it.second->rewrite_sigspecs2(functor);
	لكل (auto &it : processes)
		it.second->rewrite_sigspecs2(functor);
	لكل (auto &it : connections_) {
		functor(it.first, it.second);
	}
}

template<typename T>
فراغ RTLIL::Cell::rewrite_sigspecs(T &functor) {
	لكل (auto &it : connections_)
		functor(it.second);
}

template<typename T>
فراغ RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	لكل (auto &it : connections_)
		functor(it.second);
}

template<typename T>
فراغ RTLIL::CaseRule::rewrite_sigspecs(T &functor) {
	لكل (auto &it : compare)
		functor(it);
	لكل (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	لكل (auto it : switches)
		it->rewrite_sigspecs(functor);
}

template<typename T>
فراغ RTLIL::CaseRule::rewrite_sigspecs2(T &functor) {
	لكل (auto &it : compare)
		functor(it);
	لكل (auto &it : actions) {
		functor(it.first, it.second);
	}
	لكل (auto it : switches)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
فراغ RTLIL::SwitchRule::rewrite_sigspecs(T &functor)
{
	functor(إشارة);
	لكل (auto it : cases)
		it->rewrite_sigspecs(functor);
}

template<typename T>
فراغ RTLIL::SwitchRule::rewrite_sigspecs2(T &functor)
{
	functor(إشارة);
	لكل (auto it : cases)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
فراغ RTLIL::SyncRule::rewrite_sigspecs(T &functor)
{
	functor(إشارة);
	لكل (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	لكل (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
فراغ RTLIL::SyncRule::rewrite_sigspecs2(T &functor)
{
	functor(إشارة);
	لكل (auto &it : actions) {
		functor(it.first, it.second);
	}
	لكل (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
فراغ RTLIL::Process::rewrite_sigspecs(T &functor)
{
	ڕەگ_case.rewrite_sigspecs(functor);
	لكل (auto it : syncs)
		it->rewrite_sigspecs(functor);
}

template<typename T>
فراغ RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	ڕەگ_case.rewrite_sigspecs2(functor);
	لكل (auto it : syncs)
		it->rewrite_sigspecs2(functor);
}

#ضمن "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_END

#endif
