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

#অন্তর্ভাব "kernel/yosys_common.h"
#অন্তর্ভাব "kernel/yosys.h"
#অন্তর্ভাব "kernel/twine.h"

#অন্তর্ভাব <deque>
#অন্তর্ভাব <string_view>
#অন্তর্ভাব <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	গণনা State : unsigned বর্ণ {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	গণনা SyncType : unsigned বর্ণ {
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
	গণনা ConstFlags : unsigned বর্ণ {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	গণনা SelectPartials : unsigned বর্ণ {
		SELECT_ALL = 0,          // include partial modules
		SELECT_WHOLE_ONLY = 1,   // ignore partial modules
		SELECT_WHOLE_WARN = 2,   // call log_warning on partial module
		SELECT_WHOLE_ERR = 3,    // call log_error on partial module
		SELECT_WHOLE_CMDERR = 4  // call log_cmd_error on partial module
	};

	গণনা SelectBoxes : unsigned বর্ণ {
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

	গণনা PortDir : unsigned বর্ণ  {
		PD_UNKNOWN = 0,
		PD_প্রবেশ = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr পূর্ণাঙ্ক WIDTH_LIMIT = 1 << 30;

	সংরচনা Const;
	সংরচনা AttrObject;
	সংরচনা SelectedMember;
	সংরচনা Selection;
	সংরচনা Monitor;
	সংরচনা Design;
	সংরচনা Module;
	সংরচনা Wire;
	সংরচনা Memory;
	সংরচনা Cell;
	সংরচনা SigChunk;
	সংরচনা SigBit;
	সংরচনা SigSpecIterator;
	সংরচনা SigSpecConstIterator;
	সংরচনা SigSpec;
	সংরচনা CaseRule;
	সংরচনা SwitchRule;
	সংরচনা MemWriteAction;
	সংরচনা SyncRule;
	সংরচনা Process;

	প্রকারনির্দেশ std::pair<SigSpec, SigSpec> SigSig;
};

সংরচনা SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_PREFIX ID; }
namespace RTLIL { using YOSYS_NAMESPACE_PREFIX IdString; }

namespace RTLIL {
	শূন্য copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			ধ্রুব dict<IdString, RTLIL::Const> &src,
			ধ্রুব RTLIL::Design *src_design, RTLIL::Design *dst_design);

	বাহ্য dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	ধ্রুব pool<IdString> &builtin_ff_cell_types();

	স্থির অন্তঃস্থ std::string escape_id(ধ্রুব std::string &str) {
		যদি (str.size() > 0 && str[0] != '\\' && str[0] != '$')
			প্রত্যাগম "\\" + str;
		প্রত্যাগম str;
	}

	স্থির অন্তঃস্থ std::string unescape_id(ধ্রুব std::string &str) {
		যদি (str.size() < 2)
			প্রত্যাগম str;
		যদি (str[0] != '\\')
			প্রত্যাগম str;
		যদি (str[1] == '$' || str[1] == '\\')
			প্রত্যাগম str;
		যদি (str[1] >= '0' && str[1] <= '9')
			প্রত্যাগম str;
		প্রত্যাগম str.substr(1);
	}

	template <typename T> using sort_by_name_id = IdString::compare_ptr_by_name<T>;

	template <typename T> সংরচনা sort_by_name_str {
		bool operator()(T *a, T *b) ধ্রুব {
			প্রত্যাগম a->name.lt_by_name(b->name);
		}
	};

	সংরচনা sort_by_id_str {
		ধ্রুব TwinePool& pool;
		explicit sort_by_id_str(ধ্রুব TwinePool& pool) : pool(pool) {}
		bool operator()(RTLIL::IdString a, RTLIL::IdString b) ধ্রুব {
			প্রত্যাগম pool.compare_by_name(a, b) < 0;
		}
	};

	স্থির অন্তঃস্থ std::string encode_filename(ধ্রুব std::string &filename)
	{
		std::stringstream val;
		যদি (!std::any_of(filename.begin(), filename.end(), [](বর্ণ c) {
			প্রত্যাগম static_cast<unsigned বর্ণ>(c) < 33 || static_cast<unsigned বর্ণ>(c) > 126;
		})) প্রত্যাগম filename;
		প্রতি (unsigned বর্ণ ধ্রুব c : filename) {
			যদি (c < 33 || c > 126)
				val << stringf("$%02x", c);
			অন্যথা
				val << c;
		}
		প্রত্যাগম val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_and         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_or          (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_xor         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_xnor        (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);

	RTLIL::Const const_reduce_and  (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_reduce_or   (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_reduce_xor  (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_reduce_xnor (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_reduce_bool (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);

	RTLIL::Const const_logic_not   (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_logic_and   (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_logic_or    (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);

	RTLIL::Const const_shl         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_shr         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_sshl        (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_sshr        (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_shift       (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_shiftx      (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);

	RTLIL::Const const_lt          (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_le          (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_eq          (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_ne          (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_eqx         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_nex         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_ge          (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_gt          (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);

	RTLIL::Const const_add         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_sub         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_mul         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_div         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_divfloor    (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_modfloor    (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_mod         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_pow         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);

	RTLIL::Const const_pos         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_buf         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);
	RTLIL::Const const_neg         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len);

	RTLIL::Const const_mux         (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, ধ্রুব RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, ধ্রুব RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ধ্রুব RTLIL::Const &arg1, ধ্রুব RTLIL::Const &arg2, ধ্রুব RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	template<typename T, typename চাবি_2 = IdString>
	সংরচনা ObjIterator {
		using iterator_category = std::forward_iterator_tag;
		using মান_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		typename dict<চাবি_2, T>::iterator it;
		dict<চাবি_2, T> *list_p;
		পূর্ণাঙ্ক *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, পূর্ণাঙ্ক *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			যদি (list_p->empty()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} অন্যথা {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(ধ্রুব RTLIL::ObjIterator<T, চাবি_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			যদি (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &operator=(ধ্রুব RTLIL::ObjIterator<T, চাবি_2> &other) {
			যদি (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			যদি (refcount_p)
				(*refcount_p)++;
			প্রত্যাগম *this;
		}

		~ObjIterator() {
			যদি (refcount_p)
				(*refcount_p)--;
		}

		অন্তঃস্থ T operator*() ধ্রুব {
			log_assert(list_p != nullptr);
			প্রত্যাগম it->second;
		}

		অন্তঃস্থ bool operator!=(ধ্রুব RTLIL::ObjIterator<T, চাবি_2> &other) ধ্রুব {
			যদি (list_p == nullptr || other.list_p == nullptr)
				প্রত্যাগম list_p != other.list_p;
			প্রত্যাগম it != other.it;
		}


		অন্তঃস্থ bool operator==(ধ্রুব RTLIL::ObjIterator<T, চাবি_2> &other) ধ্রুব {
			প্রত্যাগম !(*this != other);
		}

		অন্তঃস্থ ObjIterator<T, চাবি_2>& operator++() {
			log_assert(list_p != nullptr);
			যদি (++it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			প্রত্যাগম *this;
		}

		অন্তঃস্থ ObjIterator<T, চাবি_2>& operator+=(পূর্ণাঙ্ক amt) {
			log_assert(list_p != nullptr);
			it += amt;
			যদি (it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			প্রত্যাগম *this;
		}

		অন্তঃস্থ ObjIterator<T, চাবি_2> operator+(পূর্ণাঙ্ক amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, চাবি_2> new_obj(*this);
			new_obj.it += amt;
			যদি (new_obj.it == list_p->end()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			প্রত্যাগম new_obj;
		}

		অন্তঃস্থ ধ্রুব ObjIterator<T, চাবি_2> operator++(পূর্ণাঙ্ক) {
			ObjIterator<T, চাবি_2> result(*this);
			++(*this);
			প্রত্যাগম result;
		}
	};

	template<typename T, typename চাবি_2 = IdString>
	সংরচনা ObjRange
	{
		dict<চাবি_2, T> *list_p;
		পূর্ণাঙ্ক *refcount_p;

		ObjRange(decltype(list_p) list_p, পূর্ণাঙ্ক *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, চাবি_2> begin() { প্রত্যাগম RTLIL::ObjIterator<T, চাবি_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, চাবি_2> end() { প্রত্যাগম RTLIL::ObjIterator<T, চাবি_2>(); }

		size_t size() ধ্রুব {
			প্রত্যাগম list_p->size();
		}

		operator pool<T>() ধ্রুব {
			pool<T> result;
			প্রতি (auto &it : *list_p)
				result.ঢোকাও(it.second);
			প্রত্যাগম result;
		}

		operator std::vector<T>() ধ্রুব {
			std::vector<T> result;
			result.reserve(list_p->size());
			প্রতি (auto &it : *list_p)
				result.ঠেলো_back(it.second);
			প্রত্যাগম result;
		}

		pool<T> to_pool() ধ্রুব { প্রত্যাগম *this; }
		std::vector<T> to_vector() ধ্রুব { প্রত্যাগম *this; }
	};
};

সংরচনা RTLIL::Const
{
	short পূর্ণাঙ্ক flags;
গুপ্ত:
	friend class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::vector<RTLIL::State>;
	গণনা class backing_tag: bool { bits, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_tag tag;
	সঙ্ঘ {
		bitvectype bits_;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_bits() ধ্রুব { প্রত্যাগম tag == backing_tag::bits; }
	bool is_str() ধ্রুব { প্রত্যাগম tag == backing_tag::string; }

	bitvectype* get_if_bits() { প্রত্যাগম is_bits() ? &bits_ : রিক্ত; }
	std::string* get_if_str() { প্রত্যাগম is_str() ? &str_ : রিক্ত; }
	ধ্রুব bitvectype* get_if_bits() ধ্রুব { প্রত্যাগম is_bits() ? &bits_ : রিক্ত; }
	ধ্রুব std::string* get_if_str() ধ্রুব { প্রত্যাগম is_str() ? &str_ : রিক্ত; }

	bitvectype& get_bits();
	std::string& get_str();
	ধ্রুব bitvectype& get_bits() ধ্রুব;
	ধ্রুব std::string& get_str() ধ্রুব;
	std::vector<RTLIL::State>& bits_internal();
	শূন্য bitvectorize_internal();

সার্বজনিক:
	Const() : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::vector<RTLIL::State>()) {}
	Const(std::string str);
	Const(long long val); // default width is 32
	Const(long long val, পূর্ণাঙ্ক width);
	Const(RTLIL::State bit, পূর্ণাঙ্ক width = 1);
	Const(std::vector<RTLIL::State> bits) : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::move(bits)) {}
	Const(ধ্রুব std::vector<bool> &bits);
	Const(ধ্রুব RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &operator =(ধ্রুব RTLIL::Const &other);
	~Const();

	সংরচনা Builder
	{
		Builder() {}
		Builder(পূর্ণাঙ্ক expected_width) { bits.reserve(expected_width); }
		শূন্য ঠেলো_back(RTLIL::State b) { bits.ঠেলো_back(b); }
		পূর্ণাঙ্ক size() ধ্রুব { প্রত্যাগম static_cast<পূর্ণাঙ্ক>(bits.size()); }
		Const build() { প্রত্যাগম Const(std::move(bits)); }
	গুপ্ত:
		std::vector<RTLIL::State> bits;
	};

	bool operator <(ধ্রুব RTLIL::Const &other) ধ্রুব;
	bool operator ==(ধ্রুব RTLIL::Const &other) ধ্রুব;
	bool operator !=(ধ্রুব RTLIL::Const &other) ধ্রুব;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::vector<RTLIL::State>& bits() { প্রত্যাগম bits_internal(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	শূন্য bitvectorize() ধ্রুব { const_cast<Const*>(this)->bitvectorize_internal(); }

	bool as_bool() ধ্রুব;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	পূর্ণাঙ্ক as_int(bool is_signed = false) ধ্রুব;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_to_int(bool is_signed = false) ধ্রুব;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<পূর্ণাঙ্ক> try_as_int(bool is_signed = false) ধ্রুব;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	পূর্ণাঙ্ক as_int_saturating(bool is_signed = false) ধ্রুব;

	শূন্য tag_bare_integer_const(ধ্রুব std::string &মান);

	std::string as_string(ধ্রুব বর্ণ* any = "-") ধ্রুব;
	স্থির Const from_string(ধ্রুব std::string &str);
	std::vector<RTLIL::State> to_bits() ধ্রুব;

	std::string decode_string() ধ্রুব;
	পূর্ণাঙ্ক size() ধ্রুব;
	bool empty() ধ্রুব;

	শূন্য append(ধ্রুব RTLIL::Const &other);
	শূন্য set(পূর্ণাঙ্ক i, RTLIL::State state) {
		bits_internal()[i] = state;
	}
	শূন্য resize(পূর্ণাঙ্ক size, RTLIL::State fill) {
    log_assert(size >= 0 && size < RTLIL::WIDTH_LIMIT);
		bits_internal().resize(size, fill);
	}

	class const_iterator {
	গুপ্ত:
		ধ্রুব Const* parent;
		size_t idx;

	সার্বজনিক:
		using iterator_category = std::bidirectional_iterator_tag;
		using মান_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = ধ্রুব State*;
		using reference = ধ্রুব State&;

		const_iterator(ধ্রুব Const& c, size_t i) : parent(&c), idx(i) {}

		State operator*() ধ্রুব;

		const_iterator& operator++() { ++idx; প্রত্যাগম *this; }
		const_iterator& operator--() { --idx; প্রত্যাগম *this; }
		const_iterator operator++(পূর্ণাঙ্ক) { const_iterator result(*this); ++idx; প্রত্যাগম result; }
		const_iterator operator--(পূর্ণাঙ্ক) { const_iterator result(*this); --idx; প্রত্যাগম result; }
		const_iterator& operator+=(পূর্ণাঙ্ক i) { idx += i; প্রত্যাগম *this; }

		const_iterator operator+(পূর্ণাঙ্ক add) {
			প্রত্যাগম const_iterator(*parent, idx + add);
		}
		const_iterator operator-(পূর্ণাঙ্ক sub) {
			প্রত্যাগম const_iterator(*parent, idx - sub);
		}
		পূর্ণাঙ্ক operator-(ধ্রুব const_iterator& other) {
			প্রত্যাগম idx - other.idx;
		}

		bool operator==(ধ্রুব const_iterator& other) ধ্রুব {
			প্রত্যাগম idx == other.idx;
		}

		bool operator!=(ধ্রুব const_iterator& other) ধ্রুব {
			প্রত্যাগম !(*this == other);
		}
	};

	class iterator {
	গুপ্ত:
		Const* parent;
		size_t idx;

	সার্বজনিক:
		class proxy {
		গুপ্ত:
			Const* parent;
			size_t idx;
		সার্বজনিক:
			proxy(Const* parent, size_t idx) : parent(parent), idx(idx) {}
			operator State() ধ্রুব { প্রত্যাগম (*parent)[idx]; }
			proxy& operator=(State s) { parent->set(idx, s); প্রত্যাগম *this; }
			proxy& operator=(ধ্রুব proxy& other) { parent->set(idx, (*other.parent)[other.idx]); প্রত্যাগম *this; }
		};

		using iterator_category = std::bidirectional_iterator_tag;
		using মান_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = proxy*;
		using reference = proxy;

		iterator(Const& c, size_t i) : parent(&c), idx(i) {}

		proxy operator*() ধ্রুব { প্রত্যাগম proxy(parent, idx); }
		iterator& operator++() { ++idx; প্রত্যাগম *this; }
		iterator& operator--() { --idx; প্রত্যাগম *this; }
		iterator operator++(পূর্ণাঙ্ক) { iterator result(*this); ++idx; প্রত্যাগম result; }
		iterator operator--(পূর্ণাঙ্ক) { iterator result(*this); --idx; প্রত্যাগম result; }
		iterator& operator+=(পূর্ণাঙ্ক i) { idx += i; প্রত্যাগম *this; }

		iterator operator+(পূর্ণাঙ্ক add) {
			প্রত্যাগম iterator(*parent, idx + add);
		}
		iterator operator-(পূর্ণাঙ্ক sub) {
			প্রত্যাগম iterator(*parent, idx - sub);
		}
		পূর্ণাঙ্ক operator-(ধ্রুব iterator& other) {
			প্রত্যাগম idx - other.idx;
		}

		bool operator==(ধ্রুব iterator& other) ধ্রুব {
			প্রত্যাগম idx == other.idx;
		}

		bool operator!=(ধ্রুব iterator& other) ধ্রুব {
			প্রত্যাগম !(*this == other);
		}
	};

	const_iterator begin() ধ্রুব {
		প্রত্যাগম const_iterator(*this, 0);
	}
	const_iterator end() ধ্রুব {
		প্রত্যাগম const_iterator(*this, size());
	}
	iterator begin() {
		প্রত্যাগম iterator(*this, 0);
	}
	iterator end() {
		প্রত্যাগম iterator(*this, size());
	}
	State back() ধ্রুব {
		প্রত্যাগম *(end() - 1);
	}
	State front() ধ্রুব {
		প্রত্যাগম *begin();
	}
	State at(size_t i) ধ্রুব {
		প্রত্যাগম *const_iterator(*this, i);
	}
	State operator[](size_t i) ধ্রুব {
		প্রত্যাগম *const_iterator(*this, i);
	}

	bool is_fully_zero() ধ্রুব;
	bool is_fully_ones() ধ্রুব;
	bool is_fully_def() ধ্রুব;
	bool is_fully_undef() ধ্রুব;
	bool is_fully_undef_x_only() ধ্রুব;
	bool is_onehot(পূর্ণাঙ্ক *pos = nullptr) ধ্রুব;

	RTLIL::Const extract(পূর্ণাঙ্ক offset, পূর্ণাঙ্ক len = 1, RTLIL::State padding = RTLIL::State::S0) ধ্রুব;

	// find the MSB without redundant leading bits
	পূর্ণাঙ্ক get_min_size(bool is_signed) ধ্রুব;

	// compress representation to the minimum required bits
	শূন্য compress(bool is_signed = false);

	std::optional<পূর্ণাঙ্ক> as_int_compress(bool is_signed) ধ্রুব;

	শূন্য extu(পূর্ণাঙ্ক width) {
		resize(width, RTLIL::State::S0);
	}

	শূন্য exts(পূর্ণাঙ্ক width) {
		resize(width, empty() ? RTLIL::State::Sx : back());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;
};

সংরচনা RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	bool has_attribute(RTLIL::IdString id) ধ্রুব;

	শূন্য set_bool_attribute(RTLIL::IdString id, bool মান=true);
	bool get_bool_attribute(RTLIL::IdString id) ধ্রুব;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ধ্রুব {
		প্রত্যাগম get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	শূন্য set_string_attribute(RTLIL::IdString id, string মান);
	string get_string_attribute(RTLIL::IdString id) ধ্রুব;

	শূন্য set_strpool_attribute(RTLIL::IdString id, ধ্রুব pool<string> &data);
	শূন্য add_strpool_attribute(RTLIL::IdString id, ধ্রুব pool<string> &data);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ধ্রুব;

	শূন্য set_src_attribute(ধ্রুব std::string &src) {
		set_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ধ্রুব {
		প্রত্যাগম get_string_attribute(ID::src);
	}

	শূন্য set_hdlname_attribute(ধ্রুব vector<string> &hierarchy);
	vector<string> get_hdlname_attribute() ধ্রুব;

	শূন্য set_intvec_attribute(RTLIL::IdString id, ধ্রুব vector<পূর্ণাঙ্ক> &data);
	vector<পূর্ণাঙ্ক> get_intvec_attribute(RTLIL::IdString id) ধ্রুব;
};

#অন্তর্ভাব "kernel/rtlil_twine_compat.h"

সংরচনা RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

সংরচনা RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::vector<RTLIL::State> data; // only used if wire == NULL, LSB at index 0
	পূর্ণাঙ্ক width, offset;

	SigChunk() : wire(nullptr), width(0), offset(0) {}
	SigChunk(ধ্রুব RTLIL::Const &মান) : wire(nullptr), data(মান.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Const &&মান) : wire(nullptr), data(মান.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), width(GetSize(wire)), offset(0) {}
	SigChunk(RTLIL::Wire *wire, পূর্ণাঙ্ক offset, পূর্ণাঙ্ক width = 1) : wire(wire), width(width), offset(offset) {}
	SigChunk(ধ্রুব std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(পূর্ণাঙ্ক val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(পূর্ণাঙ্ক val, পূর্ণাঙ্ক width) : SigChunk(RTLIL::Const(val, width)) {}
	SigChunk(RTLIL::State bit, পূর্ণাঙ্ক width = 1) : SigChunk(RTLIL::Const(bit, width)) {}
	SigChunk(ধ্রুব RTLIL::SigBit &bit);

	RTLIL::SigChunk extract(পূর্ণাঙ্ক offset, পূর্ণাঙ্ক length) ধ্রুব;
	RTLIL::SigBit operator[](পূর্ণাঙ্ক offset) ধ্রুব;
	অন্তঃস্থ পূর্ণাঙ্ক size() ধ্রুব { প্রত্যাগম width; }
	অন্তঃস্থ bool is_wire() ধ্রুব { প্রত্যাগম wire != রিক্ত; }

	bool operator <(ধ্রুব RTLIL::SigChunk &other) ধ্রুব;
	bool operator ==(ধ্রুব RTLIL::SigChunk &other) ধ্রুব;
	bool operator !=(ধ্রুব RTLIL::SigChunk &other) ধ্রুব;
};

সংরচনা RTLIL::SigBit
{
	RTLIL::Wire *wire;
	সঙ্ঘ {
		RTLIL::State data; // used if wire == NULL
		পূর্ণাঙ্ক offset;        // used if wire != NULL
	};

	SigBit();
	SigBit(RTLIL::State bit);
	explicit SigBit(bool bit);
	SigBit(RTLIL::Wire *wire);
	SigBit(RTLIL::Wire *wire, পূর্ণাঙ্ক offset);
	SigBit(ধ্রুব RTLIL::SigChunk &chunk);
	SigBit(ধ্রুব RTLIL::SigChunk &chunk, পূর্ণাঙ্ক index);
	SigBit(ধ্রুব RTLIL::SigSpec &sig);
	SigBit(ধ্রুব RTLIL::SigBit &sigbit) = স্বভাব;
	RTLIL::SigBit &operator =(ধ্রুব RTLIL::SigBit &other) = স্বভাব;

	অন্তঃস্থ bool is_wire() ধ্রুব { প্রত্যাগম wire != রিক্ত; }

	bool operator <(ধ্রুব RTLIL::SigBit &other) ধ্রুব;
	bool operator ==(ধ্রুব RTLIL::SigBit &other) ধ্রুব;
	bool operator !=(ধ্রুব RTLIL::SigBit &other) ধ্রুব;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব;
	[[nodiscard]] Hasher hash_শীর্ষ() ধ্রুব;
};

namespace hashlib {
	template <>
	সংরচনা hash_ops<RTLIL::SigBit> {
		স্থির অন্তঃস্থ bool cmp(ধ্রুব RTLIL::SigBit &a, ধ্রুব RTLIL::SigBit &b) {
			প্রত্যাগম a == b;
		}
		[[nodiscard]] স্থির অন্তঃস্থ Hasher hash(ধ্রুব RTLIL::SigBit sb) {
			প্রত্যাগম sb.hash_শীর্ষ();
		}
		[[nodiscard]] স্থির অন্তঃস্থ Hasher hash_into(ধ্রুব RTLIL::SigBit sb, Hasher h) {
			প্রত্যাগম sb.hash_into(h);
		}
	};
};

সংরচনা RTLIL::SigSpecIterator
{
	প্রকারনির্দেশ std::প্রবেশ_iterator_tag iterator_category;
	প্রকারনির্দেশ RTLIL::SigBit মান_type;
	প্রকারনির্দেশ ptrdiff_t difference_type;
	প্রকারনির্দেশ RTLIL::SigBit* pointer;
	প্রকারনির্দেশ RTLIL::SigBit& reference;

	RTLIL::SigSpec *sig_p;
	পূর্ণাঙ্ক index;

	অন্তঃস্থ RTLIL::SigBit &operator*() ধ্রুব;
	অন্তঃস্থ bool operator!=(ধ্রুব RTLIL::SigSpecIterator &other) ধ্রুব { প্রত্যাগম index != other.index; }
	অন্তঃস্থ bool operator==(ধ্রুব RTLIL::SigSpecIterator &other) ধ্রুব { প্রত্যাগম index == other.index; }
	অন্তঃস্থ শূন্য operator++() { index++; }
};

সংরচনা RTLIL::SigSpecConstIterator
{
	প্রকারনির্দেশ std::প্রবেশ_iterator_tag iterator_category;
	প্রকারনির্দেশ RTLIL::SigBit মান_type;
	প্রকারনির্দেশ ptrdiff_t difference_type;
	প্রকারনির্দেশ RTLIL::SigBit* pointer;
	প্রকারনির্দেশ RTLIL::SigBit& reference;

	ধ্রুব RTLIL::SigSpec *sig_p;
	RTLIL::SigBit bit;
	পূর্ণাঙ্ক index;

	অন্তঃস্থ ধ্রুব RTLIL::SigBit &operator*();
	অন্তঃস্থ bool operator!=(ধ্রুব RTLIL::SigSpecConstIterator &other) ধ্রুব { প্রত্যাগম index != other.index; }
	অন্তঃস্থ bool operator==(ধ্রুব RTLIL::SigSpecIterator &other) ধ্রুব { প্রত্যাগম index == other.index; }
	অন্তঃস্থ শূন্য operator++() { index++; }
};

সংরচনা RTLIL::SigSpec
{
গুপ্ত:
	friend class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, Extract);
	গণনা Representation : বর্ণ {
		CHUNK,
		BITS,
	};
	// An AtomicHash is either clear or a nonzero integer.
	সংরচনা AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ধ্রুব AtomicHash &rhs) : atomic_(rhs.load()) {}
		AtomicHash &operator=(ধ্রুব AtomicHash &rhs) { store(rhs.load()); প্রত্যাগম *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> পঠ() ধ্রুব {
			Hasher::hash_t মান = load();
			যদি (মান == 0)
				প্রত্যাগম std::nullopt;
			প্রত্যাগম মান;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		শূন্য set(Hasher::hash_t মান) ধ্রুব {
			log_assert(মান != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(মান, std::memory_order_relaxed);
			log_assert(old == 0 || old == মান);
		}
		শূন্য clear() { store(0); }
	গুপ্ত:
		পূর্ণাঙ্ক load() ধ্রুব { প্রত্যাগম atomic_.load(std::memory_order_relaxed); }
		শূন্য store(Hasher::hash_t মান) ধ্রুব {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(মান, std::memory_order_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	সঙ্ঘ {
		RTLIL::SigChunk chunk_;
		std::vector<RTLIL::SigBit> bits_; // LSB at index 0
	};

	শূন্য init_empty_bits() {
		rep_ = BITS;
		new (&bits_) std::vector<RTLIL::SigBit>;
	}

	শূন্য unpack();
	অন্তঃস্থ শূন্য inline_unpack() {
		যদি (rep_ == CHUNK)
			unpack();
	}
	শূন্য try_repack();

	Hasher::hash_t updhash() ধ্রুব;
	শূন্য destroy() {
		যদি (rep_ == CHUNK)
			chunk_.~SigChunk();
		অন্যথা
			bits_.~vector();
	}
	friend সংরচনা Chunks;

সার্বজনিক:
	SigSpec() { init_empty_bits(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ধ্রুব SigSpec &মান) : rep_(মান.rep_), hash_(মান.hash_) {
		যদি (মান.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(মান.chunk_);
		অন্যথা
			new (&bits_) std::vector<RTLIL::SigBit>(মান.bits_);
	}
	SigSpec(SigSpec &&মান) : rep_(মান.rep_), hash_(মান.hash_) {
		যদি (মান.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(মান.chunk_));
		অন্যথা
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(মান.bits_));
	}
	SigSpec(ধ্রুব RTLIL::Const &মান);
	SigSpec(RTLIL::Const &&মান);
	SigSpec(ধ্রুব RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, পূর্ণাঙ্ক offset, পূর্ণাঙ্ক width = 1);
	SigSpec(ধ্রুব std::string &str);
	SigSpec(পূর্ণাঙ্ক val, পূর্ণাঙ্ক width = 32);
	SigSpec(RTLIL::State bit, পূর্ণাঙ্ক width = 1);
	SigSpec(ধ্রুব RTLIL::SigBit &bit, পূর্ণাঙ্ক width = 1);
	SigSpec(ধ্রুব std::vector<RTLIL::SigChunk> &chunks);
	SigSpec(ধ্রুব std::vector<RTLIL::SigBit> &bits);
	SigSpec(ধ্রুব pool<RTLIL::SigBit> &bits);
	SigSpec(ধ্রুব std::set<RTLIL::SigBit> &bits);
	explicit SigSpec(bool bit);
	~SigSpec() {
		destroy();
	}

	SigSpec &operator=(ধ্রুব SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		যদি (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		অন্যথা
			new (&bits_) std::vector<RTLIL::SigBit>(rhs.bits_);
		প্রত্যাগম *this;
	}
	SigSpec &operator=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		যদি (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		অন্যথা
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(rhs.bits_));
		প্রত্যাগম *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	সংরচনা Chunks {
		Chunks(ধ্রুব SigSpec &spec) : spec(spec) {}
		সংরচনা const_iterator {
			using iterator_category = std::forward_iterator_tag;
			using মান_type = ধ্রুব SigChunk &;
			using difference_type = std::ptrdiff_t;
			using pointer = ধ্রুব SigChunk *;
			using reference = ধ্রুব SigChunk &;

			ধ্রুব SigSpec &spec;
			পূর্ণাঙ্ক bit_index;
			SigChunk chunk;

			const_iterator(ধ্রুব SigSpec &spec) : spec(spec) {
				bit_index = 0;
				যদি (spec.rep_ == BITS)
					next_chunk_bits();
			}
			গণনা End { END };
			const_iterator(ধ্রুব SigSpec &spec, End) : spec(spec) {
				bit_index = spec.size();
			}
			শূন্য next_chunk_bits();

			ধ্রুব SigChunk &operator*() {
				যদি (spec.rep_ == CHUNK)
					প্রত্যাগম spec.chunk_;
				প্রত্যাগম chunk;
			};
			ধ্রুব SigChunk *operator->() { প্রত্যাগম &**this; }
			const_iterator &operator++() {
				bit_index += (**this).width;
				যদি (spec.rep_ == BITS)
					next_chunk_bits();
				প্রত্যাগম *this;
			}
			bool operator==(ধ্রুব const_iterator &rhs) ধ্রুব { প্রত্যাগম bit_index == rhs.bit_index; }
			bool operator!=(ধ্রুব const_iterator &rhs) ধ্রুব { প্রত্যাগম !(*this == rhs); }
		};
		const_iterator begin() ধ্রুব { প্রত্যাগম const_iterator(spec); }
		const_iterator end() ধ্রুব {
			const_iterator it(spec, const_iterator::END);
			প্রত্যাগম it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rbegin() {
			ensure_chunk_vector();
			প্রত্যাগম chunk_vector.rbegin();
		}
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rend() {
			ensure_chunk_vector();
			প্রত্যাগম chunk_vector.rend();
		}
		পূর্ণাঙ্ক size() {
			ensure_chunk_vector();
			প্রত্যাগম chunk_vector.size();
		}
		পূর্ণাঙ্ক size() ধ্রুব {
			প্রত্যাগম std::distance(begin(), end());
		}
		ধ্রুব SigChunk &at(পূর্ণাঙ্ক index) {
			ensure_chunk_vector();
			প্রত্যাগম chunk_vector.at(index);
		}
		operator ধ্রুব std::vector<RTLIL::SigChunk>&() {
			ensure_chunk_vector();
			প্রত্যাগম chunk_vector;
		}
	গুপ্ত:
		শূন্য ensure_chunk_vector() {
			যদি (spec.size() > 0 && chunk_vector.empty()) {
				প্রতি (ধ্রুব RTLIL::SigChunk &c : *this)
					chunk_vector.ঠেলো_back(c);
			}
		}
		ধ্রুব SigSpec &spec;
		std::vector<RTLIL::SigChunk> chunk_vector;
	};
	friend সংরচনা Chunks::const_iterator;

	অন্তঃস্থ Chunks chunks() ধ্রুব { প্রত্যাগম {*this}; }
	অন্তঃস্থ ধ্রুব SigSpec &bits() ধ্রুব { প্রত্যাগম *this; }

	অন্তঃস্থ পূর্ণাঙ্ক size() ধ্রুব { প্রত্যাগম rep_ == CHUNK ? chunk_.width : GetSize(bits_); }
	অন্তঃস্থ bool empty() ধ্রুব { প্রত্যাগম size() == 0; };

	অন্তঃস্থ RTLIL::SigBit &operator[](পূর্ণাঙ্ক index) { inline_unpack(); hash_.clear(); প্রত্যাগম bits_.at(index); }
	অন্তঃস্থ RTLIL::SigBit operator[](পূর্ণাঙ্ক index) ধ্রুব {
		যদি (rep_ == CHUNK) {
			যদি (index < 0 || index >= chunk_.width)
				throw std::out_of_range("SigSpec::operator[]");
			যদি (chunk_.wire)
				প্রত্যাগম RTLIL::SigBit(chunk_.wire, chunk_.offset + index);
			প্রত্যাগম RTLIL::SigBit(chunk_.data[index]);
		}
		প্রত্যাগম bits_.at(index);
	}

	অন্তঃস্থ RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = 0; প্রত্যাগম it; }
	অন্তঃস্থ RTLIL::SigSpecIterator end() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = size(); প্রত্যাগম it; }

	অন্তঃস্থ RTLIL::SigSpecConstIterator begin() ধ্রুব { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = 0; প্রত্যাগম it; }
	অন্তঃস্থ RTLIL::SigSpecConstIterator end() ধ্রুব { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = size(); প্রত্যাগম it; }

	শূন্য sort();
	শূন্য sort_and_unify();

	শূন্য replace(ধ্রুব RTLIL::SigSpec &pattern, ধ্রুব RTLIL::SigSpec &with);
	শূন্য replace(ধ্রুব RTLIL::SigSpec &pattern, ধ্রুব RTLIL::SigSpec &with, RTLIL::SigSpec *other) ধ্রুব;

	শূন্য replace(ধ্রুব dict<RTLIL::SigBit, RTLIL::SigBit> &rules);
	শূন্য replace(ধ্রুব dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ধ্রুব;

	শূন্য replace(ধ্রুব std::map<RTLIL::SigBit, RTLIL::SigBit> &rules);
	শূন্য replace(ধ্রুব std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ধ্রুব;

	শূন্য replace(পূর্ণাঙ্ক offset, ধ্রুব RTLIL::SigSpec &with);

	শূন্য remove(ধ্রুব RTLIL::SigSpec &pattern);
	শূন্য remove(ধ্রুব RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) ধ্রুব;
	শূন্য remove2(ধ্রুব RTLIL::SigSpec &pattern, RTLIL::SigSpec *other);

	শূন্য remove(ধ্রুব pool<RTLIL::SigBit> &pattern);
	শূন্য remove(ধ্রুব pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) ধ্রুব;
	শূন্য remove2(ধ্রুব pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	শূন্য remove2(ধ্রুব std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	শূন্য remove2(ধ্রুব pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other);

	শূন্য remove(পূর্ণাঙ্ক offset, পূর্ণাঙ্ক length = 1);
	শূন্য remove_const();

	RTLIL::SigSpec extract(ধ্রুব RTLIL::SigSpec &pattern, ধ্রুব RTLIL::SigSpec *other = রিক্ত) ধ্রুব;
	RTLIL::SigSpec extract(ধ্রুব pool<RTLIL::SigBit> &pattern, ধ্রুব RTLIL::SigSpec *other = রিক্ত) ধ্রুব;
	RTLIL::SigSpec extract(পূর্ণাঙ্ক offset, পূর্ণাঙ্ক length = 1) ধ্রুব;
	RTLIL::SigSpec extract_end(পূর্ণাঙ্ক offset) ধ্রুব { প্রত্যাগম extract(offset, size() - offset); }

	শূন্য rewrite_wires(std::function<শূন্য(RTLIL::Wire*& wire)> rewrite);

	RTLIL::SigBit lsb() ধ্রুব { log_assert(size()); প্রত্যাগম (*this)[0]; };
	RTLIL::SigBit msb() ধ্রুব { log_assert(size()); প্রত্যাগম (*this)[size() - 1]; };
	RTLIL::SigBit front() ধ্রুব { প্রত্যাগম (*this)[0]; }
	RTLIL::SigBit back() ধ্রুব { প্রত্যাগম (*this)[size() - 1]; }

	শূন্য append(ধ্রুব RTLIL::SigSpec &সঙ্কেত);
	অন্তঃস্থ শূন্য append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	অন্তঃস্থ শূন্য append(ধ্রুব RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	অন্তঃস্থ শূন্য append(ধ্রুব RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	শূন্য append(ধ্রুব RTLIL::SigBit &bit);
	অন্তঃস্থ শূন্য append(RTLIL::State state) { append(RTLIL::SigBit(state)); }
	অন্তঃস্থ শূন্য append(bool bool_) { append(RTLIL::SigBit(bool_)); }

	শূন্য extend_u0(পূর্ণাঙ্ক width, bool is_signed = false);

	RTLIL::SigSpec repeat(পূর্ণাঙ্ক অঙ্ক) ধ্রুব;

	শূন্য reverse() { inline_unpack(); std::reverse(bits_.begin(), bits_.end()); }

	bool operator <(ধ্রুব RTLIL::SigSpec &other) ধ্রুব;
	bool operator ==(ধ্রুব RTLIL::SigSpec &other) ধ্রুব;
	অন্তঃস্থ bool operator !=(ধ্রুব RTLIL::SigSpec &other) ধ্রুব { প্রত্যাগম !(*this == other); }

	bool is_wire() ধ্রুব;
	bool is_chunk() ধ্রুব;
	অন্তঃস্থ bool is_bit() ধ্রুব { প্রত্যাগম size() == 1; }

	bool known_driver() ধ্রুব;

	bool is_fully_const() ধ্রুব;
	bool is_fully_zero() ধ্রুব;
	bool is_fully_ones() ধ্রুব;
	bool is_fully_def() ধ্রুব;
	bool is_fully_undef() ধ্রুব;
	bool has_const() ধ্রুব;
	bool has_const(State state) ধ্রুব;
	bool has_marked_bits() ধ্রুব;
	bool is_onehot(পূর্ণাঙ্ক *pos = nullptr) ধ্রুব;

	bool as_bool() ধ্রুব;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	পূর্ণাঙ্ক as_int(bool is_signed = false) ধ্রুব;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_to_int(bool is_signed = false) ধ্রুব;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<পূর্ণাঙ্ক> try_as_int(bool is_signed = false) ধ্রুব;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	পূর্ণাঙ্ক as_int_saturating(bool is_signed = false) ধ্রুব;

	std::string as_string() ধ্রুব;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() ধ্রুব;
	RTLIL::Const as_const() ধ্রুব;
	RTLIL::Wire *as_wire() ধ্রুব;
	RTLIL::SigChunk as_chunk() ধ্রুব;
	RTLIL::SigBit as_bit() ধ্রুব;

	bool match(ধ্রুব বর্ণ* pattern) ধ্রুব;

	std::set<RTLIL::SigBit> to_sigbit_set() ধ্রুব;
	pool<RTLIL::SigBit> to_sigbit_pool() ধ্রুব;
	std::vector<RTLIL::SigBit> to_sigbit_vector() ধ্রুব;
	std::map<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_map(ধ্রুব RTLIL::SigSpec &other) ধ্রুব;
	dict<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_dict(ধ্রুব RTLIL::SigSpec &other) ধ্রুব;

	স্থির bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	স্থির bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	স্থির bool parse_rhs(ধ্রুব RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	operator std::vector<RTLIL::SigChunk>() ধ্রুব;
	operator std::vector<RTLIL::SigBit>() ধ্রুব { প্রত্যাগম to_sigbit_vector(); }
	ধ্রুব RTLIL::SigBit &at(পূর্ণাঙ্ক offset, ধ্রুব RTLIL::SigBit &defval) { প্রত্যাগম offset < size() ? (*this)[offset] : defval; }
	RTLIL::SigBit& at(পূর্ণাঙ্ক offset) { প্রত্যাগম (*this)[offset]; }
	RTLIL::SigBit at(পূর্ণাঙ্ক offset) ধ্রুব { প্রত্যাগম (*this)[offset]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব {
		Hasher::hash_t val;
		যদি (std::optional<Hasher::hash_t> current = hash_.পঠ())
			val = *current;
		অন্যথা
			val = updhash();
		h.eat(val);
		প্রত্যাগম h;
	}

#ifndef NDEBUG
	শূন্য check(ধ্রুব Module *mod = nullptr) ধ্রুব;
#অন্যথা
	শূন্য check(ধ্রুব Module *mod = nullptr) ধ্রুব { (শূন্য)mod; }
#endif
};

সংরচনা RTLIL::Selection
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
	bool boxed_module(RTLIL::IdString mod_name) ধ্রুব;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ধ্রুব;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) ধ্রুব;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ধ্রুব;

	// optimizes this selection for the given design by:
	// - removing non-existent modules and members, any boxed modules and
	//   their members (if selection does not include boxes), and any
	//   partially selected modules with no selected members;
	// - marking partially selected modules as wholly selected if all
	//   members of that module are selected; and
	// - marking selection as a complete_selection if all modules in the
	//   given design are selected, or a full_selection if it does not
	//   include boxes.
	শূন্য optimize(RTLIL::Design *design);

	// checks if selection covers full design (may or may not include
	// boxed-modules)
	bool selects_all() ধ্রুব {
		প্রত্যাগম full_selection || complete_selection;
	}

	// add whole module to this selection
	template<typename T1> শূন্য select(T1 *module) {
		যদি (!selects_all() && selected_modules.গুনতি(module->name) == 0) {
			IdString name = module->name;
			selected_modules.ঢোকাও(name);
			selected_members.erase(name);
			যদি (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	template<typename T1, typename T2> শূন্য select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	template<typename T1> শূন্য select(T1 *module, RTLIL::IdString member) {
		যদি (!selects_all() && selected_modules.গুনতি(module->name.ref()) == 0) {
			selected_members[module->name.ref()].ঢোকাও(member);
			যদি (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool empty() ধ্রুব {
		প্রত্যাগম !selects_all() && selected_modules.empty() && selected_members.empty();
	}

	// clear this selection, leaving it empty
	শূন্য clear();

	// create a new selection which is empty
	স্থির Selection EmptySelection(RTLIL::Design *design = nullptr) { প্রত্যাগম Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	স্থির Selection FullSelection(RTLIL::Design *design = nullptr) { প্রত্যাগম Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	স্থির Selection CompleteSelection(RTLIL::Design *design = nullptr) { প্রত্যাগম Selection(true, true, design); };
};

সংরচনা RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব { h.eat(hashidx_); প্রত্যাগম h; }

	Monitor() {
		স্থির unsigned পূর্ণাঙ্ক hashidx_গুনতি = 123456789;
		hashidx_গুনতি = mkhash_xorshift(hashidx_গুনতি);
		hashidx_ = hashidx_গুনতি;
	}

	আভাস ~Monitor() { }
	আভাস শূন্য notify_module_add(RTLIL::Module*) { }
	আভাস শূন্য notify_module_del(RTLIL::Module*) { }
	আভাস শূন্য notify_connect(RTLIL::Cell*, RTLIL::IdString, ধ্রুব RTLIL::SigSpec&, ধ্রুব RTLIL::SigSpec&) { }
	আভাস শূন্য notify_connect(RTLIL::Module*, ধ্রুব RTLIL::SigSig&) { }
	আভাস শূন্য notify_connect(RTLIL::Module*, ধ্রুব std::vector<RTLIL::SigSig>&) { }
	আভাস শূন্য notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
সংরচনা define_map_t;

template<typename N>
অন্তঃস্থ constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::Leaf> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ধ্রুব বর্ণ*> || std::is_same_v<std::decay_t<N>, বর্ণ*>;

#define YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, পূর্ণাঙ্ক> = 0

template<typename N>
অন্তঃস্থ constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ধ্রুব বর্ণ *> || std::is_same_v<std::decay_t<N>, বর্ণ *>;

#define YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, পূর্ণাঙ্ক> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#define YS_NAME_FWD_POOL(_func, _pool) \
	template<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(N name, Rest&&... rest) \
		{ প্রত্যাগম _func(_pool.add(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#define YS_NAME_FWD_2ND_POOL(_func, _pool) \
	template<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(T &&first, N name) \
		{ প্রত্যাগম _func(std::forward<T>(first), _pool.add(std::move(name))); }

#define YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#define YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

সংরচনা RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব { h.eat(hashidx_); প্রত্যাগম h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	শূন্য bufNormalize(bool enable=true);

	পূর্ণাঙ্ক refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

গুপ্ত:
	TwinePool twines_;

সার্বজনিক:
	TwinePool &twines() { প্রত্যাগম twines_; }
	ধ্রুব TwinePool &twines() ধ্রুব { প্রত্যাগম twines_; }

	শূন্য absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	size_t gc_twines();

	std::vector<std::unique_ptr<AST::Ast_গ্রন্থি>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_defines;

	std::vector<RTLIL::Selection> selection_স্তূপ;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_active_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ধ্রুব RTLIL::Module *module(RTLIL::IdString name) ধ্রুব;
	template<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ প্রত্যাগম module(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ধ্রুব RTLIL::Module *module(N name) ধ্রুব
		{ প্রত্যাগম module(twines().find(std::string_view(name))); }
	RTLIL::Module *শীর্ষ_module() ধ্রুব;

	bool has(RTLIL::IdString id) ধ্রুব {
		twines_.check_owned(id);
		প্রত্যাগম modules_.গুনতি(id) != 0;
	}
	template<typename N, YS_NAME_STRING(N)> bool has(N name) ধ্রুব
		{ প্রত্যাগম has(twines().find(std::string_view(name))); }

	শূন্য add(RTLIL::Module *module);

	RTLIL::Module *addModule(RTLIL::IdString name);
	YS_NAME_FWD_POOL(addModule, twines())
	শূন্য remove(RTLIL::Module *module);
	শূন্য rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	শূন্য scratchpad_unset(ধ্রুব std::string &varname);

	শূন্য scratchpad_set_int(ধ্রুব std::string &varname, পূর্ণাঙ্ক মান);
	শূন্য scratchpad_set_bool(ধ্রুব std::string &varname, bool মান);
	শূন্য scratchpad_set_string(ধ্রুব std::string &varname, std::string মান);

	পূর্ণাঙ্ক scratchpad_get_int(ধ্রুব std::string &varname, পূর্ণাঙ্ক default_মান_2 = 0) ধ্রুব;
	bool scratchpad_get_bool(ধ্রুব std::string &varname, bool default_মান_2 = false) ধ্রুব;
	std::string scratchpad_get_string(ধ্রুব std::string &varname, ধ্রুব std::string &default_মান_2 = std::string()) ধ্রুব;

	শূন্য sort();
	শূন্য sort_modules();
	শূন্য check();
	শূন্য optimize();

	শূন্য clone_into(RTLIL::Design *dst) ধ্রুব;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ধ্রুব;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) ধ্রুব;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ধ্রুব;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ধ্রুব;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::Module *mod) ধ্রুব;

	// push the given selection to the selection stack
	শূন্য ঠেলো_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	শূন্য ঠেলো_empty_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	শূন্য ঠেলো_full_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	শূন্য ঠেলো_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	শূন্য বের_করো_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		প্রত্যাগম selection_স্তূপ.back();
	}

	// get the current selection
	ধ্রুব RTLIL::Selection &selection() ধ্রুব {
		প্রত্যাগম selection_স্তূপ.back();
	}

	// is the current selection a full selection (no boxes)
	bool full_selection() ধ্রুব {
		প্রত্যাগম selection().full_selection;
	}

	// is the given module in the current selection
	template<typename T1> bool selected(T1 *module) ধ্রুব {
		প্রত্যাগম selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	template<typename T1, typename T2> bool selected(T1 *module, T2 *member) ধ্রুব {
		প্রত্যাগম selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	template<typename T1> শূন্য select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	template<typename T1, typename T2> শূন্য select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::vector<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) ধ্রুব;

	// returns all selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_modules() ধ্রুব { প্রত্যাগম selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::vector<RTLIL::Module*> selected_unboxed_modules() ধ্রুব { প্রত্যাগম selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_modules_warn() ধ্রুব { প্রত্যাগম selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::vector<RTLIL::Module*> selected_whole_modules() ধ্রুব { প্রত্যাগম selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_whole_modules() ধ্রুব { প্রত্যাগম selected_modules(SELECT_WHOLE_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::vector<RTLIL::Module*> selected_whole_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ধ্রুব { প্রত্যাগম selected_modules(SELECT_WHOLE_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules() ধ্রুব { প্রত্যাগম selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules_warn() ধ্রুব { প্রত্যাগম selected_modules(SELECT_WHOLE_WARN, SB_UNBOXED_WARN); }

	স্থির std::map<unsigned পূর্ণাঙ্ক, RTLIL::Design*> *get_all_designs(শূন্য);

	std::string to_rtlil_str(bool only_selected = true) ধ্রুব;
};

সংরচনা RTLIL::Module : সার্বজনিক RTLIL::AttrObject
{
	friend সংরচনা RTLIL::Cell;
	friend সংরচনা RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব { h.eat(hashidx_); প্রত্যাগম h; }

রক্ষিত:
	শূন্য add(RTLIL::Wire *wire);
	শূন্য add(RTLIL::Cell *cell);
	শূন্য add(RTLIL::Process *process);

সার্বজনিক:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ধ্রুব { প্রত্যাগম design->twines(); }

	পূর্ণাঙ্ক refcount_wires_;
	পূর্ণাঙ্ক refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::vector<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_মানসমূহ;
	dict<RTLIL::IdString, RTLIL::Memory*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	আভাস ~Module();
	আভাস RTLIL::IdString derive(RTLIL::Design *design, ধ্রুব dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	আভাস RTLIL::IdString derive(RTLIL::Design *design, ধ্রুব dict<RTLIL::IdString, RTLIL::Const> &parameters, ধ্রুব dict<RTLIL::IdString, RTLIL::Module*> &interfaces, ধ্রুব dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	আভাস size_t গুনতি_id(RTLIL::IdString id);
	আভাস শূন্য expand_interfaces(RTLIL::Design *design, ধ্রুব dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	আভাস bool reprocess_if_necessary(RTLIL::Design *design);

	আভাস শূন্য sort();
	আভাস শূন্য check();
	আভাস শূন্য optimize();
	আভাস শূন্য makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ধ্রুব {
		প্রত্যাগম get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	শূন্য connect(ধ্রুব RTLIL::SigSig &conn);
	শূন্য connect(ধ্রুব RTLIL::SigSpec &lhs, ধ্রুব RTLIL::SigSpec &rhs);
	শূন্য new_connections(ধ্রুব std::vector<RTLIL::SigSig> &new_conn);
	ধ্রুব std::vector<RTLIL::SigSig> &connections() ধ্রুব;

	std::vector<RTLIL::IdString> ports;
	শূন্য fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_queue;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_port_queue;
	pool<RTLIL::Wire *> buf_norm_wire_queue;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_index;
	শূন্য bufNormalize();

	template<typename T> শূন্য rewrite_sigspecs(T &functor);
	template<typename T> শূন্য rewrite_sigspecs2(T &functor);
	শূন্য cloneInto(RTLIL::Module *new_mod) ধ্রুব;
	আভাস RTLIL::Module *clone() ধ্রুব;
	আভাস RTLIL::Module *clone(RTLIL::Design *dst) ধ্রুব;
	আভাস RTLIL::Module *clone(RTLIL::Design *dst, IdString target_name) ধ্রুব;

	bool has_memories() ধ্রুব;
	bool has_processes() ধ্রুব;

	bool has_memories_warn() ধ্রুব;
	bool has_processes_warn() ধ্রুব;

	bool is_selected() ধ্রুব;
	bool is_selected_whole() ধ্রুব;

	std::vector<RTLIL::Wire*> selected_wires() ধ্রুব;
	std::vector<RTLIL::Cell*> selected_cells() ধ্রুব;
	std::vector<RTLIL::Memory*> selected_memories() ধ্রুব;
	std::vector<RTLIL::Process*> selected_processes() ধ্রুব;
	std::vector<RTLIL::SelectedMember> selected_members() ধ্রুব;

	template<typename T> bool selected(T *member) ধ্রুব {
		প্রত্যাগম design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		যদি (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		প্রত্যাগম it == wires_.end() ? nullptr : it->second;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		যদি (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		প্রত্যাগম it == cells_.end() ? nullptr : it->second;
	}

	ধ্রুব RTLIL::Wire* wire(RTLIL::IdString id) ধ্রুব{
		যদি (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		প্রত্যাগম it == wires_.end() ? nullptr : it->second;
	}
	ধ্রুব RTLIL::Cell* cell(RTLIL::IdString id) ধ্রুব {
		যদি (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		প্রত্যাগম it == cells_.end() ? nullptr : it->second;
	}

	template<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ প্রত্যাগম wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ প্রত্যাগম cell(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ধ্রুব RTLIL::Wire *wire(N name) ধ্রুব
		{ প্রত্যাগম wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ধ্রুব RTLIL::Cell *cell(N name) ধ্রুব
		{ প্রত্যাগম cell(twines().find(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { প্রত্যাগম RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	পূর্ণাঙ্ক wires_size() ধ্রুব { প্রত্যাগম wires_.size(); }
	RTLIL::Wire* wire_at(পূর্ণাঙ্ক index) ধ্রুব { প্রত্যাগম wires_.element(index)->second; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { প্রত্যাগম RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	পূর্ণাঙ্ক cells_size() ধ্রুব { প্রত্যাগম cells_.size(); }
	RTLIL::Cell* cell_at(পূর্ণাঙ্ক index) ধ্রুব { প্রত্যাগম cells_.element(index)->second; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	শূন্য remove(ধ্রুব pool<RTLIL::Wire*> &wires);
	শূন্য remove(RTLIL::Cell *cell);
	শূন্য remove(RTLIL::Memory *memory);
	শূন্য remove(RTLIL::Process *process);

	শূন্য rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	শূন্য rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	শূন্য rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	শূন্য swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2);
	শূন্য swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, পূর্ণাঙ্ক &index);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *addWire(RTLIL::IdString name, পূর্ণাঙ্ক width = 1);
	RTLIL::Wire *addWire(RTLIL::IdString name, ধ্রুব RTLIL::Wire *other);
	YS_NAME_FWD(addWire)

	RTLIL::Cell *addCell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *addCell(RTLIL::IdString name, ধ্রুব RTLIL::Cell *other);
	YS_NAME_FWD(addCell)
	template<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *addCell(RTLIL::IdString name, T type) { প্রত্যাগম addCell(name, design->twines().add(std::move(type))); }

	RTLIL::Memory *addMemory(RTLIL::IdString name);
	YS_NAME_FWD(addMemory)
	RTLIL::Memory *addMemory(RTLIL::IdString name, ধ্রুব RTLIL::Memory *other);

	RTLIL::Process *addProcess(RTLIL::IdString name);
	YS_NAME_FWD(addProcess)
	RTLIL::Process *addProcess(RTLIL::IdString name, ধ্রুব RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* addNot     (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addPos     (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addBuf     (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addBarrier (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addNeg     (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");

	RTLIL::Cell* addAnd  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addOr   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addXor  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addXnor (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");

	RTLIL::Cell* addReduceAnd  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addReduceOr   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addReduceXor  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addReduceXnor (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addReduceBool (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");

	RTLIL::Cell* addShl    (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addShr    (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addSshl   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addSshr   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addShift  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addShiftx (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");

	RTLIL::Cell* addLt  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addLe  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addEq  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addNe  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addEqx (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addNex (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addGe  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addGt  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");

	RTLIL::Cell* addAdd (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addSub (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addMul (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	// truncating division
	RTLIL::Cell* addDiv (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	// truncating modulo
	RTLIL::Cell* addMod (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDivFloor (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addModFloor (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addPow (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, ধ্রুব std::string &src = "");

	RTLIL::Cell* addFa (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_c, ধ্রুব RTLIL::SigSpec &sig_x, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src = "");

	RTLIL::Cell* addLogicNot (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addLogicAnd (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::Cell* addLogicOr  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুব std::string &src = "");

	RTLIL::Cell* addMux  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addPmux (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addBmux (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDemux (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src = "");

	RTLIL::Cell* addBweqx  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addBwmux  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src = "");

	RTLIL::Cell* addSlice  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, RTLIL::Const offset, ধ্রুব std::string &src = "");
	RTLIL::Cell* addConcat (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addLut    (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_y, RTLIL::Const lut, ধ্রুব std::string &src = "");
	RTLIL::Cell* addTribuf (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAssert (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAssume (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব std::string &src = "");
	RTLIL::Cell* addLive   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব std::string &src = "");
	RTLIL::Cell* addFair   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব std::string &src = "");
	RTLIL::Cell* addCover  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব std::string &src = "");
	RTLIL::Cell* addEquiv  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src = "");

	RTLIL::Cell* addSr    (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr, ধ্রুব RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addFf    (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDff   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_d,   ধ্রুব RTLIL::SigSpec &sig_q, bool clk_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDffe  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en,  ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDffsr (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ধ্রুব RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDffsre (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ধ্রুব RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAdff (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_arst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, RTLIL::Const arst_মান, bool clk_polarity = true, bool arst_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAdffe (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_arst,  ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, RTLIL::Const arst_মান, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAldff (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_aload, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, ধ্রুব RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAldffe (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_aload,  ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, ধ্রুব RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addSdff (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_srst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, RTLIL::Const srst_মান, bool clk_polarity = true, bool srst_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addSdffe (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_srst,  ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, RTLIL::Const srst_মান, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addSdffce (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_srst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, RTLIL::Const srst_মান, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDlatch (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, bool en_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAdlatch (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_arst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, RTLIL::Const arst_মান, bool en_polarity = true, bool arst_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDlatchsr (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ধ্রুব RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ধ্রুব std::string &src = "");

	RTLIL::Cell* addBufGate    (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addNotGate    (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAndGate    (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addNandGate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addOrGate     (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addNorGate    (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addXorGate    (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addXnorGate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAndnotGate (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addOrnotGate  (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addMuxGate    (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_s, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addNmuxGate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_s, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAoi3Gate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_c, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addOai3Gate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_c, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAoi4Gate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_c, ধ্রুব RTLIL::SigBit &sig_d, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");
	RTLIL::Cell* addOai4Gate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_c, ধ্রুব RTLIL::SigBit &sig_d, ধ্রুব RTLIL::SigBit &sig_y, ধ্রুব std::string &src = "");

	RTLIL::Cell* addSrGate     (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr,
			ধ্রুব RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addFfGate     (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDffGate    (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, bool clk_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDffeGate   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDffsrGate  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ধ্রুব RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDffsreGate (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ধ্রুব RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAdffGate   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_arst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
			bool arst_মান = false, bool clk_polarity = true, bool arst_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAdffeGate  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_arst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
			bool arst_মান = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAldffGate   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_aload, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
			ধ্রুব RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAldffeGate  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_aload, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
			ধ্রুব RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addSdffGate   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_srst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
			bool srst_মান = false, bool clk_polarity = true, bool srst_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addSdffeGate  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_srst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
			bool srst_মান = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addSdffceGate (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_clk, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_srst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
			bool srst_মান = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDlatchGate (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, bool en_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addAdlatchGate(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_arst, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q,
			bool arst_মান = false, bool en_polarity = true, bool arst_polarity = true, ধ্রুব std::string &src = "");
	RTLIL::Cell* addDlatchsrGate  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_en, ধ্রুব RTLIL::SigSpec &sig_set, ধ্রুব RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ধ্রুব RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ধ্রুব std::string &src = "");

	RTLIL::Cell* addAnyinit(RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_d, ধ্রুব RTLIL::SigSpec &sig_q, ধ্রুব std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুব std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুব std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Shift  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");

	RTLIL::SigSpec Add (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, ধ্রুব std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুব std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_b, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব std::string &src = "");

	RTLIL::SigBit BufGate    (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব std::string &src = "");
	RTLIL::SigBit NotGate    (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব std::string &src = "");
	RTLIL::SigBit AndGate    (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব std::string &src = "");
	RTLIL::SigBit NandGate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব std::string &src = "");
	RTLIL::SigBit OrGate     (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব std::string &src = "");
	RTLIL::SigBit NorGate    (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব std::string &src = "");
	RTLIL::SigBit XorGate    (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব std::string &src = "");
	RTLIL::SigBit XnorGate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব std::string &src = "");
	RTLIL::SigBit AndnotGate (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব std::string &src = "");
	RTLIL::SigBit OrnotGate  (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব std::string &src = "");
	RTLIL::SigBit MuxGate    (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_s, ধ্রুব std::string &src = "");
	RTLIL::SigBit NmuxGate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_s, ধ্রুব std::string &src = "");
	RTLIL::SigBit Aoi3Gate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_c, ধ্রুব std::string &src = "");
	RTLIL::SigBit Oai3Gate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_c, ধ্রুব std::string &src = "");
	RTLIL::SigBit Aoi4Gate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_c, ধ্রুব RTLIL::SigBit &sig_d, ধ্রুব std::string &src = "");
	RTLIL::SigBit Oai4Gate   (RTLIL::IdString name, ধ্রুব RTLIL::SigBit &sig_a, ধ্রুব RTLIL::SigBit &sig_b, ধ্রুব RTLIL::SigBit &sig_c, ধ্রুব RTLIL::SigBit &sig_d, ধ্রুব std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, পূর্ণাঙ্ক width = 1, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, পূর্ণাঙ্ক width = 1, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, পূর্ণাঙ্ক width = 1, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, পূর্ণাঙ্ক width = 1, ধ্রুব std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ধ্রুব std::string &src = "");

	RTLIL::SigSpec SetTag          (RTLIL::IdString name, ধ্রুব std::string &tag, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব RTLIL::SigSpec &sig_c, ধ্রুব std::string &src = "");
	RTLIL::Cell*   addSetTag       (RTLIL::IdString name, ধ্রুব std::string &tag, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব RTLIL::SigSpec &sig_c, ধ্রুব RTLIL::SigSpec &sig_y, ধ্রুব std::string &src = "");
	RTLIL::SigSpec GetTag          (RTLIL::IdString name, ধ্রুব std::string &tag, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব std::string &src = "");
	RTLIL::Cell*   addOverwriteTag (RTLIL::IdString name, ধ্রুব std::string &tag, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব RTLIL::SigSpec &sig_s, ধ্রুব RTLIL::SigSpec &sig_c, ধ্রুব std::string &src = "");
	RTLIL::SigSpec OriginalTag     (RTLIL::IdString name, ধ্রুব std::string &tag, ধ্রুব RTLIL::SigSpec &sig_a, ধ্রুব std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ধ্রুব RTLIL::SigSpec &sig_e, ধ্রুব std::string &src = "");
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

	std::string to_rtlil_str() ধ্রুব;
#ifdef YOSYS_ENABLE_PYTHON
	স্থির std::map<unsigned পূর্ণাঙ্ক, RTLIL::Module*> *get_all_modules(শূন্য);
#endif
};

সংরচনা RTLIL::Wire : সার্বজনিক RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব { h.eat(hashidx_); প্রত্যাগম h; }

রক্ষিত:
	// use module->addWire() and module->remove() to create or destroy wires
	friend সংরচনা RTLIL::Module;
	Wire();
	~Wire();

	friend সংরচনা RTLIL::Design;
	friend সংরচনা RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driverPort_;

সার্বজনিক:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	শূন্য operator=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	পূর্ণাঙ্ক width, start_offset, port_id;
	bool port_প্রবেশ, port_output, upto, is_signed;

	RTLIL::Design *design() ধ্রুব { প্রত্যাগম module ? module->design : nullptr; }
	TwinePool &twines() ধ্রুব { প্রত্যাগম module->twines(); }

	bool known_driver() ধ্রুব { প্রত্যাগম driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ধ্রুব    { log_assert(driverCell_); প্রত্যাগম driverCell_; };
	RTLIL::IdString driverPort() ধ্রুব { log_assert(driverCell_); প্রত্যাগম driverPort_; };

	পূর্ণাঙ্ক from_hdl_index(পূর্ণাঙ্ক hdl_index) {
		পূর্ণাঙ্ক zero_index = hdl_index - start_offset;
		পূর্ণাঙ্ক rtlil_index = upto ? width - 1 - zero_index : zero_index;
		প্রত্যাগম rtlil_index >= 0 && rtlil_index < width ? rtlil_index : INT_MIN;
	}

	পূর্ণাঙ্ক to_hdl_index(পূর্ণাঙ্ক rtlil_index) {
		যদি (rtlil_index < 0 || rtlil_index >= width)
			প্রত্যাগম INT_MIN;
		পূর্ণাঙ্ক zero_index = upto ? width - 1 - rtlil_index : rtlil_index;
		প্রত্যাগম zero_index + start_offset;
	}

	std::string to_rtlil_str() ধ্রুব;

#ifdef YOSYS_ENABLE_PYTHON
	স্থির std::map<unsigned পূর্ণাঙ্ক, RTLIL::Wire*> *get_all_wires(শূন্য);
#endif
};

অন্তঃস্থ পূর্ণাঙ্ক GetSize(RTLIL::Wire *wire) {
	প্রত্যাগম wire->width;
}

সংরচনা RTLIL::Memory : সার্বজনিক RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব { h.eat(hashidx_); প্রত্যাগম h; }

	Memory();
	~Memory();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() ধ্রুব { প্রত্যাগম module ? module->design : nullptr; }
	TwinePool &twines() ধ্রুব { প্রত্যাগম module->twines(); }

	RTLIL::MemoryNameId name;

	পূর্ণাঙ্ক width, start_offset, size;
#ifdef YOSYS_ENABLE_PYTHON
	স্থির std::map<unsigned পূর্ণাঙ্ক, RTLIL::Memory*> *get_all_memorys(শূন্য);
#endif

	std::string to_rtlil_str() ধ্রুব;
};

সংরচনা RTLIL::Cell : সার্বজনিক RTLIL::AttrObject
{
গুপ্ত:
	শূন্য initIndex();

	bool bufnorm_handle_setPort(IdString portname, RTLIL::SigSpec &সঙ্কেত, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
সার্বজনিক:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব { h.eat(hashidx_); প্রত্যাগম h; }

রক্ষিত:
	// use module->addCell() and module->remove() to create or destroy cells
	friend সংরচনা RTLIL::Module;
	Cell();
	~Cell();

সার্বজনিক:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	শূন্য operator=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() ধ্রুব { প্রত্যাগম module ? module->design : nullptr; }
	TwinePool &twines() ধ্রুব { প্রত্যাগম module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool hasPort(RTLIL::IdString portname) ধ্রুব;
	শূন্য unsetPort(RTLIL::IdString portname);
	শূন্য setPort(RTLIL::IdString portname, RTLIL::SigSpec সঙ্কেত);
	template<typename N, YS_NAME_STRING(N)> শূন্য setPort(N portname, RTLIL::SigSpec সঙ্কেত)
		{ setPort(module->twines().add(std::move(portname)), std::move(সঙ্কেত)); }
	ধ্রুব RTLIL::SigSpec &getPort(RTLIL::IdString portname) ধ্রুব;
	ধ্রুব dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ধ্রুব;

	bool known() ধ্রুব;
	bool প্রবেশ(RTLIL::IdString portname) ধ্রুব;
	bool output(RTLIL::IdString portname) ধ্রুব;
	PortDir port_dir(RTLIL::IdString portname) ধ্রুব;

	bool hasParam(RTLIL::IdString paramname) ধ্রুব;
	শূন্য unsetParam(RTLIL::IdString paramname);
	শূন্য setParam(RTLIL::IdString paramname, RTLIL::Const মান);
	ধ্রুব RTLIL::Const &getParam(RTLIL::IdString paramname) ধ্রুব;

	template<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ধ্রুব
		{ প্রত্যাগম hasParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> শূন্য unsetParam(N name)
		{ unsetParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> শূন্য setParam(N name, RTLIL::Const মান)
		{ setParam(module->twines().add(std::move(name)), std::move(মান)); }
	template<typename N, YS_NAME_STRING(N)> ধ্রুব RTLIL::Const &getParam(N name) ধ্রুব
		{ প্রত্যাগম getParam(module->twines().find(std::string_view(name))); }

	শূন্য sort();
	শূন্য check();
	শূন্য fixup_parameters(bool set_a_signed = false, bool set_b_signed = false);

	bool has_keep_attr() ধ্রুব;

	template<typename T> শূন্য rewrite_sigspecs(T &functor);
	template<typename T> শূন্য rewrite_sigspecs2(T &functor);

#ifdef YOSYS_ENABLE_PYTHON
	স্থির std::map<unsigned পূর্ণাঙ্ক, RTLIL::Cell*> *get_all_cells(শূন্য);
#endif

	bool has_memid() ধ্রুব;
	bool is_mem_cell() ধ্রুব;
	bool is_builtin_ff() ধ্রুব;

	std::string to_rtlil_str() ধ্রুব;
};

সংরচনা RTLIL::CaseRule : সার্বজনিক RTLIL::AttrObject
{
	std::vector<RTLIL::SigSpec> compare;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::SwitchRule*> switches;

	~CaseRule();

	bool empty() ধ্রুব;

	template<typename T> শূন্য rewrite_sigspecs(T &functor);
	template<typename T> শূন্য rewrite_sigspecs2(T &functor);
	RTLIL::CaseRule *clone() ধ্রুব;
};

সংরচনা RTLIL::SwitchRule : সার্বজনিক RTLIL::AttrObject
{
	RTLIL::SigSpec সঙ্কেত;
	std::vector<RTLIL::CaseRule*> cases;

	~SwitchRule();

	bool empty() ধ্রুব;

	template<typename T> শূন্য rewrite_sigspecs(T &functor);
	template<typename T> শূন্য rewrite_sigspecs2(T &functor);
	RTLIL::SwitchRule *clone() ধ্রুব;
};

সংরচনা RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec address;
	RTLIL::SigSpec data;
	RTLIL::SigSpec enable;
	RTLIL::Const priority_mask;
};

সংরচনা RTLIL::SyncRule
{
	RTLIL::SyncType type;
	RTLIL::SigSpec সঙ্কেত;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::MemWriteAction> mem_write_actions;

	template<typename T> শূন্য rewrite_sigspecs(T &functor);
	template<typename T> শূন্য rewrite_sigspecs2(T &functor);
	RTLIL::SyncRule *clone() ধ্রুব;
};

সংরচনা RTLIL::Process : সার্বজনিক RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুব { h.eat(hashidx_); প্রত্যাগম h; }

রক্ষিত:
	// use module->addProcess() and module->remove() to create or destroy processes
	friend সংরচনা RTLIL::Module;
	Process();
	~Process();

সার্বজনিক:
	RTLIL::Module *module;
	RTLIL::CaseRule মূল_case;
	std::vector<RTLIL::SyncRule*> syncs;

	RTLIL::Design *design() ধ্রুব { প্রত্যাগম module ? module->design : nullptr; }
	TwinePool &twines() ধ্রুব { প্রত্যাগম module->twines(); }

	template<typename T> শূন্য rewrite_sigspecs(T &functor);
	template<typename T> শূন্য rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ধ্রুব;

	std::string to_rtlil_str() ধ্রুব;
};

অন্তঃস্থ RTLIL::SigBit::SigBit() : wire(রিক্ত), data(RTLIL::State::S0) { }
অন্তঃস্থ RTLIL::SigBit::SigBit(RTLIL::State bit) : wire(রিক্ত), data(bit) { }
অন্তঃস্থ RTLIL::SigBit::SigBit(bool bit) : wire(রিক্ত), data(bit ? State::S1 : State::S0) { }
অন্তঃস্থ RTLIL::SigBit::SigBit(RTLIL::Wire *wire) : wire(wire), offset(0) { log_assert(wire && wire->width == 1); }
অন্তঃস্থ RTLIL::SigBit::SigBit(RTLIL::Wire *wire, পূর্ণাঙ্ক offset) : wire(wire), offset(offset) { log_assert(wire != nullptr); }
অন্তঃস্থ RTLIL::SigBit::SigBit(ধ্রুব RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.width == 1); যদি (wire) offset = chunk.offset; অন্যথা data = chunk.data[0]; }
অন্তঃস্থ RTLIL::SigBit::SigBit(ধ্রুব RTLIL::SigChunk &chunk, পূর্ণাঙ্ক index) : wire(chunk.wire) { যদি (wire) offset = chunk.offset + index; অন্যথা data = chunk.data[index]; }

অন্তঃস্থ bool RTLIL::SigBit::operator<(ধ্রুব RTLIL::SigBit &other) ধ্রুব {
	যদি (wire == other.wire)
		প্রত্যাগম wire ? (offset < other.offset) : (data < other.data);
	যদি (wire != nullptr && other.wire != nullptr)
		প্রত্যাগম wire->name < other.wire->name;
	প্রত্যাগম (wire != nullptr) < (other.wire != nullptr);
}

অন্তঃস্থ bool RTLIL::SigBit::operator==(ধ্রুব RTLIL::SigBit &other) ধ্রুব {
	প্রত্যাগম (wire == other.wire) && (wire ? (offset == other.offset) : (data == other.data));
}

অন্তঃস্থ bool RTLIL::SigBit::operator!=(ধ্রুব RTLIL::SigBit &other) ধ্রুব {
	প্রত্যাগম (wire != other.wire) || (wire ? (offset != other.offset) : (data != other.data));
}

অন্তঃস্থ Hasher RTLIL::SigBit::hash_into(Hasher h) ধ্রুব {
	যদি (wire) {
		h.eat(offset);
		h.eat(wire->name);
		প্রত্যাগম h;
	}
	h.eat(data);
	প্রত্যাগম h;
}


অন্তঃস্থ Hasher RTLIL::SigBit::hash_শীর্ষ() ধ্রুব {
	Hasher h;
	যদি (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.bits();
		h.force(hashlib::legacy::djb2_add(n, offset));
		প্রত্যাগম h;
	}
	h.force(data);
	প্রত্যাগম h;
}

অন্তঃস্থ RTLIL::SigBit &RTLIL::SigSpecIterator::operator*() ধ্রুব {
	প্রত্যাগম (*sig_p)[index];
}

অন্তঃস্থ ধ্রুব RTLIL::SigBit &RTLIL::SigSpecConstIterator::operator*() {
	bit = (*sig_p)[index];
	প্রত্যাগম bit;
}

অন্তঃস্থ RTLIL::SigBit::SigBit(ধ্রুব RTLIL::SigSpec &sig) {
	log_assert(sig.size() == 1);
	auto it = sig.chunks().begin();
	*this = SigBit(*it);
}

template<typename T>
শূন্য RTLIL::Module::rewrite_sigspecs(T &functor)
{
	প্রতি (auto &it : cells_)
		it.second->rewrite_sigspecs(functor);
	প্রতি (auto &it : processes)
		it.second->rewrite_sigspecs(functor);
	প্রতি (auto &it : connections_) {
		functor(it.first);
		functor(it.second);
	}
}

template<typename T>
শূন্য RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	প্রতি (auto &it : cells_)
		it.second->rewrite_sigspecs2(functor);
	প্রতি (auto &it : processes)
		it.second->rewrite_sigspecs2(functor);
	প্রতি (auto &it : connections_) {
		functor(it.first, it.second);
	}
}

template<typename T>
শূন্য RTLIL::Cell::rewrite_sigspecs(T &functor) {
	প্রতি (auto &it : connections_)
		functor(it.second);
}

template<typename T>
শূন্য RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	প্রতি (auto &it : connections_)
		functor(it.second);
}

template<typename T>
শূন্য RTLIL::CaseRule::rewrite_sigspecs(T &functor) {
	প্রতি (auto &it : compare)
		functor(it);
	প্রতি (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	প্রতি (auto it : switches)
		it->rewrite_sigspecs(functor);
}

template<typename T>
শূন্য RTLIL::CaseRule::rewrite_sigspecs2(T &functor) {
	প্রতি (auto &it : compare)
		functor(it);
	প্রতি (auto &it : actions) {
		functor(it.first, it.second);
	}
	প্রতি (auto it : switches)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
শূন্য RTLIL::SwitchRule::rewrite_sigspecs(T &functor)
{
	functor(সঙ্কেত);
	প্রতি (auto it : cases)
		it->rewrite_sigspecs(functor);
}

template<typename T>
শূন্য RTLIL::SwitchRule::rewrite_sigspecs2(T &functor)
{
	functor(সঙ্কেত);
	প্রতি (auto it : cases)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
শূন্য RTLIL::SyncRule::rewrite_sigspecs(T &functor)
{
	functor(সঙ্কেত);
	প্রতি (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	প্রতি (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
শূন্য RTLIL::SyncRule::rewrite_sigspecs2(T &functor)
{
	functor(সঙ্কেত);
	প্রতি (auto &it : actions) {
		functor(it.first, it.second);
	}
	প্রতি (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
শূন্য RTLIL::Process::rewrite_sigspecs(T &functor)
{
	মূল_case.rewrite_sigspecs(functor);
	প্রতি (auto it : syncs)
		it->rewrite_sigspecs(functor);
}

template<typename T>
শূন্য RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	মূল_case.rewrite_sigspecs2(functor);
	প্রতি (auto it : syncs)
		it->rewrite_sigspecs2(functor);
}

#অন্তর্ভাব "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_END

#endif
