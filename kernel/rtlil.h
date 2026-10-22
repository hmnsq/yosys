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
	গণনা State : unsigned অক্ষর {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	গণনা SyncType : unsigned অক্ষর {
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
	গণনা ConstFlags : unsigned অক্ষর {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	গণনা SelectPartials : unsigned অক্ষর {
		SELECT_ALL = 0,          // include partial modules
		SELECT_WHOLE_ONLY = 1,   // ignore partial modules
		SELECT_WHOLE_WARN = 2,   // call log_warning on partial module
		SELECT_WHOLE_ERR = 3,    // call log_error on partial module
		SELECT_WHOLE_CMDERR = 4  // call log_cmd_error on partial module
	};

	গণনা SelectBoxes : unsigned অক্ষর {
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

	গণনা PortDir : unsigned অক্ষর  {
		PD_UNKNOWN = 0,
		PD_প্রবেশ = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr পূর্ণসংখ্যা WIDTH_LIMIT = 1 << 30;

	কাঠামো Const;
	কাঠামো AttrObject;
	কাঠামো SelectedMember;
	কাঠামো Selection;
	কাঠামো Monitor;
	কাঠামো Design;
	কাঠামো Module;
	কাঠামো Wire;
	কাঠামো Memory;
	কাঠামো Cell;
	কাঠামো SigChunk;
	কাঠামো SigBit;
	কাঠামো SigSpecIterator;
	কাঠামো SigSpecConstIterator;
	কাঠামো SigSpec;
	কাঠামো CaseRule;
	কাঠামো SwitchRule;
	কাঠামো MemWriteAction;
	কাঠামো SyncRule;
	কাঠামো Process;

	প্রকারলেখ std::pair<SigSpec, SigSpec> SigSig;
};

কাঠামো SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_PREFIX ID; }
namespace RTLIL { using YOSYS_NAMESPACE_PREFIX IdString; }

namespace RTLIL {
	শূন্য copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			ধ্রুবক dict<IdString, RTLIL::Const> &src,
			ধ্রুবক RTLIL::Design *src_design, RTLIL::Design *dst_design);

	বাহ্যিক dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	ধ্রুবক pool<IdString> &builtin_ff_cell_types();

	স্থির অন্তঃরেখ std::string escape_id(ধ্রুবক std::string &str) {
		যদি (str.আকার_2() > 0 && str[0] != '\\' && str[0] != '$')
			ফেরত "\\" + str;
		ফেরত str;
	}

	স্থির অন্তঃরেখ std::string unescape_id(ধ্রুবক std::string &str) {
		যদি (str.আকার_2() < 2)
			ফেরত str;
		যদি (str[0] != '\\')
			ফেরত str;
		যদি (str[1] == '$' || str[1] == '\\')
			ফেরত str;
		যদি (str[1] >= '0' && str[1] <= '9')
			ফেরত str;
		ফেরত str.substr(1);
	}

	template <typename T> using sort_by_name_id = IdString::compare_ptr_by_name<T>;

	template <typename T> কাঠামো sort_by_name_str {
		bool operator()(T *a, T *b) ধ্রুবক {
			ফেরত a->name.lt_by_name(b->name);
		}
	};

	কাঠামো sort_by_id_str {
		ধ্রুবক TwinePool& pool;
		explicit sort_by_id_str(ধ্রুবক TwinePool& pool) : pool(pool) {}
		bool operator()(RTLIL::IdString a, RTLIL::IdString b) ধ্রুবক {
			ফেরত pool.compare_by_name(a, b) < 0;
		}
	};

	স্থির অন্তঃরেখ std::string encode_filename(ধ্রুবক std::string &filename)
	{
		std::stringstream val;
		যদি (!std::any_of(filename.begin(), filename.end(), [](অক্ষর c) {
			ফেরত static_cast<unsigned অক্ষর>(c) < 33 || static_cast<unsigned অক্ষর>(c) > 126;
		})) ফেরত filename;
		জন্য (unsigned অক্ষর ধ্রুবক c : filename) {
			যদি (c < 33 || c > 126)
				val << stringf("$%02x", c);
			অন্যথা
				val << c;
		}
		ফেরত val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_and         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_or          (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_xor         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_xnor        (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);

	RTLIL::Const const_reduce_and  (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_reduce_or   (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_reduce_xor  (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_reduce_xnor (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_reduce_bool (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);

	RTLIL::Const const_logic_not   (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_logic_and   (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_logic_or    (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);

	RTLIL::Const const_shl         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_shr         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_sshl        (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_sshr        (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_shift       (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_shiftx      (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);

	RTLIL::Const const_lt          (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_le          (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_eq          (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_ne          (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_eqx         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_nex         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_ge          (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_gt          (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);

	RTLIL::Const const_add         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_sub         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_mul         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_div         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_divfloor    (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_modfloor    (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_mod         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_pow         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);

	RTLIL::Const const_pos         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_buf         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);
	RTLIL::Const const_neg         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণসংখ্যা ফলাফল_len);

	RTLIL::Const const_mux         (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, ধ্রুবক RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, ধ্রুবক RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ধ্রুবক RTLIL::Const &arg1, ধ্রুবক RTLIL::Const &arg2, ধ্রুবক RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	template<typename T, typename চাবি_2 = IdString>
	কাঠামো ObjIterator {
		using iterator_category = std::forward_iterator_tag;
		using মান_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		typename dict<চাবি_2, T>::iterator it;
		dict<চাবি_2, T> *list_p;
		পূর্ণসংখ্যা *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, পূর্ণসংখ্যা *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			যদি (list_p->empty()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} অন্যথা {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(ধ্রুবক RTLIL::ObjIterator<T, চাবি_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			যদি (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &operator=(ধ্রুবক RTLIL::ObjIterator<T, চাবি_2> &other) {
			যদি (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			যদি (refcount_p)
				(*refcount_p)++;
			ফেরত *this;
		}

		~ObjIterator() {
			যদি (refcount_p)
				(*refcount_p)--;
		}

		অন্তঃরেখ T operator*() ধ্রুবক {
			log_assert(list_p != nullptr);
			ফেরত it->second;
		}

		অন্তঃরেখ bool operator!=(ধ্রুবক RTLIL::ObjIterator<T, চাবি_2> &other) ধ্রুবক {
			যদি (list_p == nullptr || other.list_p == nullptr)
				ফেরত list_p != other.list_p;
			ফেরত it != other.it;
		}


		অন্তঃরেখ bool operator==(ধ্রুবক RTLIL::ObjIterator<T, চাবি_2> &other) ধ্রুবক {
			ফেরত !(*this != other);
		}

		অন্তঃরেখ ObjIterator<T, চাবি_2>& operator++() {
			log_assert(list_p != nullptr);
			যদি (++it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			ফেরত *this;
		}

		অন্তঃরেখ ObjIterator<T, চাবি_2>& operator+=(পূর্ণসংখ্যা amt) {
			log_assert(list_p != nullptr);
			it += amt;
			যদি (it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			ফেরত *this;
		}

		অন্তঃরেখ ObjIterator<T, চাবি_2> operator+(পূর্ণসংখ্যা amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, চাবি_2> new_obj(*this);
			new_obj.it += amt;
			যদি (new_obj.it == list_p->end()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			ফেরত new_obj;
		}

		অন্তঃরেখ ধ্রুবক ObjIterator<T, চাবি_2> operator++(পূর্ণসংখ্যা) {
			ObjIterator<T, চাবি_2> ফলাফল(*this);
			++(*this);
			ফেরত ফলাফল;
		}
	};

	template<typename T, typename চাবি_2 = IdString>
	কাঠামো ObjRange
	{
		dict<চাবি_2, T> *list_p;
		পূর্ণসংখ্যা *refcount_p;

		ObjRange(decltype(list_p) list_p, পূর্ণসংখ্যা *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, চাবি_2> begin() { ফেরত RTLIL::ObjIterator<T, চাবি_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, চাবি_2> end() { ফেরত RTLIL::ObjIterator<T, চাবি_2>(); }

		আকার_t আকার_2() ধ্রুবক {
			ফেরত list_p->আকার_2();
		}

		operator pool<T>() ধ্রুবক {
			pool<T> ফলাফল;
			জন্য (auto &it : *list_p)
				ফলাফল.ঢোকাও(it.second);
			ফেরত ফলাফল;
		}

		operator std::vector<T>() ধ্রুবক {
			std::vector<T> ফলাফল;
			ফলাফল.reserve(list_p->আকার_2());
			জন্য (auto &it : *list_p)
				ফলাফল.ঠেলো_back(it.second);
			ফেরত ফলাফল;
		}

		pool<T> to_pool() ধ্রুবক { ফেরত *this; }
		std::vector<T> to_vector() ধ্রুবক { ফেরত *this; }
	};
};

কাঠামো RTLIL::Const
{
	short পূর্ণসংখ্যা flags;
ব্যক্তিগত:
	friend class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::vector<RTLIL::State>;
	গণনা class backing_tag: bool { bits, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_tag tag;
	সংঘ {
		bitvectype bits_;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_bits() ধ্রুবক { ফেরত tag == backing_tag::bits; }
	bool is_str() ধ্রুবক { ফেরত tag == backing_tag::string; }

	bitvectype* get_if_bits() { ফেরত is_bits() ? &bits_ : ফাঁকা; }
	std::string* get_if_str() { ফেরত is_str() ? &str_ : ফাঁকা; }
	ধ্রুবক bitvectype* get_if_bits() ধ্রুবক { ফেরত is_bits() ? &bits_ : ফাঁকা; }
	ধ্রুবক std::string* get_if_str() ধ্রুবক { ফেরত is_str() ? &str_ : ফাঁকা; }

	bitvectype& get_bits();
	std::string& get_str();
	ধ্রুবক bitvectype& get_bits() ধ্রুবক;
	ধ্রুবক std::string& get_str() ধ্রুবক;
	std::vector<RTLIL::State>& bits_internal();
	শূন্য bitvectorize_internal();

সর্বজনীন:
	Const() : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::vector<RTLIL::State>()) {}
	Const(std::string str);
	Const(long long val); // default width is 32
	Const(long long val, পূর্ণসংখ্যা width);
	Const(RTLIL::State bit, পূর্ণসংখ্যা width = 1);
	Const(std::vector<RTLIL::State> bits) : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::move(bits)) {}
	Const(ধ্রুবক std::vector<bool> &bits);
	Const(ধ্রুবক RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &operator =(ধ্রুবক RTLIL::Const &other);
	~Const();

	কাঠামো Builder
	{
		Builder() {}
		Builder(পূর্ণসংখ্যা expected_width) { bits.reserve(expected_width); }
		শূন্য ঠেলো_back(RTLIL::State b) { bits.ঠেলো_back(b); }
		পূর্ণসংখ্যা আকার_2() ধ্রুবক { ফেরত static_cast<পূর্ণসংখ্যা>(bits.আকার_2()); }
		Const build() { ফেরত Const(std::move(bits)); }
	ব্যক্তিগত:
		std::vector<RTLIL::State> bits;
	};

	bool operator <(ধ্রুবক RTLIL::Const &other) ধ্রুবক;
	bool operator ==(ধ্রুবক RTLIL::Const &other) ধ্রুবক;
	bool operator !=(ধ্রুবক RTLIL::Const &other) ধ্রুবক;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::vector<RTLIL::State>& bits() { ফেরত bits_internal(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	শূন্য bitvectorize() ধ্রুবক { const_cast<Const*>(this)->bitvectorize_internal(); }

	bool as_bool() ধ্রুবক;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	পূর্ণসংখ্যা as_int(bool is_signed = false) ধ্রুবক;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_to_int(bool is_signed = false) ধ্রুবক;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<পূর্ণসংখ্যা> try_as_int(bool is_signed = false) ধ্রুবক;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	পূর্ণসংখ্যা as_int_saturating(bool is_signed = false) ধ্রুবক;

	শূন্য tag_bare_integer_const(ধ্রুবক std::string &মান);

	std::string as_string(ধ্রুবক অক্ষর* any = "-") ধ্রুবক;
	স্থির Const from_string(ধ্রুবক std::string &str);
	std::vector<RTLIL::State> to_bits() ধ্রুবক;

	std::string decode_string() ধ্রুবক;
	পূর্ণসংখ্যা আকার_2() ধ্রুবক;
	bool empty() ধ্রুবক;

	শূন্য append(ধ্রুবক RTLIL::Const &other);
	শূন্য set(পূর্ণসংখ্যা i, RTLIL::State state) {
		bits_internal()[i] = state;
	}
	শূন্য resize(পূর্ণসংখ্যা আকার_2, RTLIL::State fill) {
    log_assert(আকার_2 >= 0 && আকার_2 < RTLIL::WIDTH_LIMIT);
		bits_internal().resize(আকার_2, fill);
	}

	class const_iterator {
	ব্যক্তিগত:
		ধ্রুবক Const* parent;
		আকার_t idx;

	সর্বজনীন:
		using iterator_category = std::bidirectional_iterator_tag;
		using মান_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = ধ্রুবক State*;
		using reference = ধ্রুবক State&;

		const_iterator(ধ্রুবক Const& c, আকার_t i) : parent(&c), idx(i) {}

		State operator*() ধ্রুবক;

		const_iterator& operator++() { ++idx; ফেরত *this; }
		const_iterator& operator--() { --idx; ফেরত *this; }
		const_iterator operator++(পূর্ণসংখ্যা) { const_iterator ফলাফল(*this); ++idx; ফেরত ফলাফল; }
		const_iterator operator--(পূর্ণসংখ্যা) { const_iterator ফলাফল(*this); --idx; ফেরত ফলাফল; }
		const_iterator& operator+=(পূর্ণসংখ্যা i) { idx += i; ফেরত *this; }

		const_iterator operator+(পূর্ণসংখ্যা add) {
			ফেরত const_iterator(*parent, idx + add);
		}
		const_iterator operator-(পূর্ণসংখ্যা sub) {
			ফেরত const_iterator(*parent, idx - sub);
		}
		পূর্ণসংখ্যা operator-(ধ্রুবক const_iterator& other) {
			ফেরত idx - other.idx;
		}

		bool operator==(ধ্রুবক const_iterator& other) ধ্রুবক {
			ফেরত idx == other.idx;
		}

		bool operator!=(ধ্রুবক const_iterator& other) ধ্রুবক {
			ফেরত !(*this == other);
		}
	};

	class iterator {
	ব্যক্তিগত:
		Const* parent;
		আকার_t idx;

	সর্বজনীন:
		class proxy {
		ব্যক্তিগত:
			Const* parent;
			আকার_t idx;
		সর্বজনীন:
			proxy(Const* parent, আকার_t idx) : parent(parent), idx(idx) {}
			operator State() ধ্রুবক { ফেরত (*parent)[idx]; }
			proxy& operator=(State s) { parent->set(idx, s); ফেরত *this; }
			proxy& operator=(ধ্রুবক proxy& other) { parent->set(idx, (*other.parent)[other.idx]); ফেরত *this; }
		};

		using iterator_category = std::bidirectional_iterator_tag;
		using মান_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = proxy*;
		using reference = proxy;

		iterator(Const& c, আকার_t i) : parent(&c), idx(i) {}

		proxy operator*() ধ্রুবক { ফেরত proxy(parent, idx); }
		iterator& operator++() { ++idx; ফেরত *this; }
		iterator& operator--() { --idx; ফেরত *this; }
		iterator operator++(পূর্ণসংখ্যা) { iterator ফলাফল(*this); ++idx; ফেরত ফলাফল; }
		iterator operator--(পূর্ণসংখ্যা) { iterator ফলাফল(*this); --idx; ফেরত ফলাফল; }
		iterator& operator+=(পূর্ণসংখ্যা i) { idx += i; ফেরত *this; }

		iterator operator+(পূর্ণসংখ্যা add) {
			ফেরত iterator(*parent, idx + add);
		}
		iterator operator-(পূর্ণসংখ্যা sub) {
			ফেরত iterator(*parent, idx - sub);
		}
		পূর্ণসংখ্যা operator-(ধ্রুবক iterator& other) {
			ফেরত idx - other.idx;
		}

		bool operator==(ধ্রুবক iterator& other) ধ্রুবক {
			ফেরত idx == other.idx;
		}

		bool operator!=(ধ্রুবক iterator& other) ধ্রুবক {
			ফেরত !(*this == other);
		}
	};

	const_iterator begin() ধ্রুবক {
		ফেরত const_iterator(*this, 0);
	}
	const_iterator end() ধ্রুবক {
		ফেরত const_iterator(*this, আকার_2());
	}
	iterator begin() {
		ফেরত iterator(*this, 0);
	}
	iterator end() {
		ফেরত iterator(*this, আকার_2());
	}
	State back() ধ্রুবক {
		ফেরত *(end() - 1);
	}
	State front() ধ্রুবক {
		ফেরত *begin();
	}
	State at(আকার_t i) ধ্রুবক {
		ফেরত *const_iterator(*this, i);
	}
	State operator[](আকার_t i) ধ্রুবক {
		ফেরত *const_iterator(*this, i);
	}

	bool is_fully_zero() ধ্রুবক;
	bool is_fully_ones() ধ্রুবক;
	bool is_fully_def() ধ্রুবক;
	bool is_fully_undef() ধ্রুবক;
	bool is_fully_undef_x_only() ধ্রুবক;
	bool is_onehot(পূর্ণসংখ্যা *pos = nullptr) ধ্রুবক;

	RTLIL::Const extract(পূর্ণসংখ্যা offset, পূর্ণসংখ্যা len = 1, RTLIL::State padding = RTLIL::State::S0) ধ্রুবক;

	// find the MSB without redundant leading bits
	পূর্ণসংখ্যা get_min_আকার(bool is_signed) ধ্রুবক;

	// compress representation to the minimum required bits
	শূন্য compress(bool is_signed = false);

	std::optional<পূর্ণসংখ্যা> as_int_compress(bool is_signed) ধ্রুবক;

	শূন্য extu(পূর্ণসংখ্যা width) {
		resize(width, RTLIL::State::S0);
	}

	শূন্য exts(পূর্ণসংখ্যা width) {
		resize(width, empty() ? RTLIL::State::Sx : back());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;
};

কাঠামো RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	bool has_attribute(RTLIL::IdString id) ধ্রুবক;

	শূন্য set_bool_attribute(RTLIL::IdString id, bool মান=true);
	bool get_bool_attribute(RTLIL::IdString id) ধ্রুবক;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ধ্রুবক {
		ফেরত get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	শূন্য set_string_attribute(RTLIL::IdString id, string মান);
	string get_string_attribute(RTLIL::IdString id) ধ্রুবক;

	শূন্য set_strpool_attribute(RTLIL::IdString id, ধ্রুবক pool<string> &তথ্য);
	শূন্য add_strpool_attribute(RTLIL::IdString id, ধ্রুবক pool<string> &তথ্য);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ধ্রুবক;

	শূন্য set_src_attribute(ধ্রুবক std::string &src) {
		set_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ধ্রুবক {
		ফেরত get_string_attribute(ID::src);
	}

	শূন্য set_hdlname_attribute(ধ্রুবক vector<string> &hierarchy);
	vector<string> get_hdlname_attribute() ধ্রুবক;

	শূন্য set_intvec_attribute(RTLIL::IdString id, ধ্রুবক vector<পূর্ণসংখ্যা> &তথ্য);
	vector<পূর্ণসংখ্যা> get_intvec_attribute(RTLIL::IdString id) ধ্রুবক;
};

#অন্তর্ভাব "kernel/rtlil_twine_compat.h"

কাঠামো RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

কাঠামো RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::vector<RTLIL::State> তথ্য; // only used if wire == NULL, LSB at index 0
	পূর্ণসংখ্যা width, offset;

	SigChunk() : wire(nullptr), width(0), offset(0) {}
	SigChunk(ধ্রুবক RTLIL::Const &মান) : wire(nullptr), তথ্য(মান.to_bits()), width(Get_আকার(তথ্য)), offset(0) {}
	SigChunk(RTLIL::Const &&মান) : wire(nullptr), তথ্য(মান.to_bits()), width(Get_আকার(তথ্য)), offset(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), width(Get_আকার(wire)), offset(0) {}
	SigChunk(RTLIL::Wire *wire, পূর্ণসংখ্যা offset, পূর্ণসংখ্যা width = 1) : wire(wire), width(width), offset(offset) {}
	SigChunk(ধ্রুবক std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(পূর্ণসংখ্যা val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(পূর্ণসংখ্যা val, পূর্ণসংখ্যা width) : SigChunk(RTLIL::Const(val, width)) {}
	SigChunk(RTLIL::State bit, পূর্ণসংখ্যা width = 1) : SigChunk(RTLIL::Const(bit, width)) {}
	SigChunk(ধ্রুবক RTLIL::SigBit &bit);

	RTLIL::SigChunk extract(পূর্ণসংখ্যা offset, পূর্ণসংখ্যা দৈর্ঘ্য) ধ্রুবক;
	RTLIL::SigBit operator[](পূর্ণসংখ্যা offset) ধ্রুবক;
	অন্তঃরেখ পূর্ণসংখ্যা আকার_2() ধ্রুবক { ফেরত width; }
	অন্তঃরেখ bool is_wire() ধ্রুবক { ফেরত wire != ফাঁকা; }

	bool operator <(ধ্রুবক RTLIL::SigChunk &other) ধ্রুবক;
	bool operator ==(ধ্রুবক RTLIL::SigChunk &other) ধ্রুবক;
	bool operator !=(ধ্রুবক RTLIL::SigChunk &other) ধ্রুবক;
};

কাঠামো RTLIL::SigBit
{
	RTLIL::Wire *wire;
	সংঘ {
		RTLIL::State তথ্য; // used if wire == NULL
		পূর্ণসংখ্যা offset;        // used if wire != NULL
	};

	SigBit();
	SigBit(RTLIL::State bit);
	explicit SigBit(bool bit);
	SigBit(RTLIL::Wire *wire);
	SigBit(RTLIL::Wire *wire, পূর্ণসংখ্যা offset);
	SigBit(ধ্রুবক RTLIL::SigChunk &chunk);
	SigBit(ধ্রুবক RTLIL::SigChunk &chunk, পূর্ণসংখ্যা সূচক);
	SigBit(ধ্রুবক RTLIL::SigSpec &sig);
	SigBit(ধ্রুবক RTLIL::SigBit &sigbit) = পূর্বনির্ধারিত;
	RTLIL::SigBit &operator =(ধ্রুবক RTLIL::SigBit &other) = পূর্বনির্ধারিত;

	অন্তঃরেখ bool is_wire() ধ্রুবক { ফেরত wire != ফাঁকা; }

	bool operator <(ধ্রুবক RTLIL::SigBit &other) ধ্রুবক;
	bool operator ==(ধ্রুবক RTLIL::SigBit &other) ধ্রুবক;
	bool operator !=(ধ্রুবক RTLIL::SigBit &other) ধ্রুবক;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক;
	[[nodiscard]] Hasher hash_শীর্ষ() ধ্রুবক;
};

namespace hashlib {
	template <>
	কাঠামো hash_ops<RTLIL::SigBit> {
		স্থির অন্তঃরেখ bool cmp(ধ্রুবক RTLIL::SigBit &a, ধ্রুবক RTLIL::SigBit &b) {
			ফেরত a == b;
		}
		[[nodiscard]] স্থির অন্তঃরেখ Hasher hash(ধ্রুবক RTLIL::SigBit sb) {
			ফেরত sb.hash_শীর্ষ();
		}
		[[nodiscard]] স্থির অন্তঃরেখ Hasher hash_into(ধ্রুবক RTLIL::SigBit sb, Hasher h) {
			ফেরত sb.hash_into(h);
		}
	};
};

কাঠামো RTLIL::SigSpecIterator
{
	প্রকারলেখ std::প্রবেশ_iterator_tag iterator_category;
	প্রকারলেখ RTLIL::SigBit মান_type;
	প্রকারলেখ ptrdiff_t difference_type;
	প্রকারলেখ RTLIL::SigBit* pointer;
	প্রকারলেখ RTLIL::SigBit& reference;

	RTLIL::SigSpec *sig_p;
	পূর্ণসংখ্যা সূচক;

	অন্তঃরেখ RTLIL::SigBit &operator*() ধ্রুবক;
	অন্তঃরেখ bool operator!=(ধ্রুবক RTLIL::SigSpecIterator &other) ধ্রুবক { ফেরত সূচক != other.সূচক; }
	অন্তঃরেখ bool operator==(ধ্রুবক RTLIL::SigSpecIterator &other) ধ্রুবক { ফেরত সূচক == other.সূচক; }
	অন্তঃরেখ শূন্য operator++() { সূচক++; }
};

কাঠামো RTLIL::SigSpecConstIterator
{
	প্রকারলেখ std::প্রবেশ_iterator_tag iterator_category;
	প্রকারলেখ RTLIL::SigBit মান_type;
	প্রকারলেখ ptrdiff_t difference_type;
	প্রকারলেখ RTLIL::SigBit* pointer;
	প্রকারলেখ RTLIL::SigBit& reference;

	ধ্রুবক RTLIL::SigSpec *sig_p;
	RTLIL::SigBit bit;
	পূর্ণসংখ্যা সূচক;

	অন্তঃরেখ ধ্রুবক RTLIL::SigBit &operator*();
	অন্তঃরেখ bool operator!=(ধ্রুবক RTLIL::SigSpecConstIterator &other) ধ্রুবক { ফেরত সূচক != other.সূচক; }
	অন্তঃরেখ bool operator==(ধ্রুবক RTLIL::SigSpecIterator &other) ধ্রুবক { ফেরত সূচক == other.সূচক; }
	অন্তঃরেখ শূন্য operator++() { সূচক++; }
};

কাঠামো RTLIL::SigSpec
{
ব্যক্তিগত:
	friend class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, Extract);
	গণনা Representation : অক্ষর {
		CHUNK,
		BITS,
	};
	// An AtomicHash is either clear or a nonzero integer.
	কাঠামো AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ধ্রুবক AtomicHash &rhs) : atomic_(rhs.load()) {}
		AtomicHash &operator=(ধ্রুবক AtomicHash &rhs) { store(rhs.load()); ফেরত *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> পঠ() ধ্রুবক {
			Hasher::hash_t মান = load();
			যদি (মান == 0)
				ফেরত std::nullopt;
			ফেরত মান;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		শূন্য set(Hasher::hash_t মান) ধ্রুবক {
			log_assert(মান != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(মান, std::memory_order_relaxed);
			log_assert(old == 0 || old == মান);
		}
		শূন্য clear() { store(0); }
	ব্যক্তিগত:
		পূর্ণসংখ্যা load() ধ্রুবক { ফেরত atomic_.load(std::memory_order_relaxed); }
		শূন্য store(Hasher::hash_t মান) ধ্রুবক {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(মান, std::memory_order_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	সংঘ {
		RTLIL::SigChunk chunk_;
		std::vector<RTLIL::SigBit> bits_; // LSB at index 0
	};

	শূন্য init_empty_bits() {
		rep_ = BITS;
		new (&bits_) std::vector<RTLIL::SigBit>;
	}

	শূন্য unpack();
	অন্তঃরেখ শূন্য inline_unpack() {
		যদি (rep_ == CHUNK)
			unpack();
	}
	শূন্য try_repack();

	Hasher::hash_t updhash() ধ্রুবক;
	শূন্য destroy() {
		যদি (rep_ == CHUNK)
			chunk_.~SigChunk();
		অন্যথা
			bits_.~vector();
	}
	friend কাঠামো Chunks;

সর্বজনীন:
	SigSpec() { init_empty_bits(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ধ্রুবক SigSpec &মান) : rep_(মান.rep_), hash_(মান.hash_) {
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
	SigSpec(ধ্রুবক RTLIL::Const &মান);
	SigSpec(RTLIL::Const &&মান);
	SigSpec(ধ্রুবক RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, পূর্ণসংখ্যা offset, পূর্ণসংখ্যা width = 1);
	SigSpec(ধ্রুবক std::string &str);
	SigSpec(পূর্ণসংখ্যা val, পূর্ণসংখ্যা width = 32);
	SigSpec(RTLIL::State bit, পূর্ণসংখ্যা width = 1);
	SigSpec(ধ্রুবক RTLIL::SigBit &bit, পূর্ণসংখ্যা width = 1);
	SigSpec(ধ্রুবক std::vector<RTLIL::SigChunk> &chunks);
	SigSpec(ধ্রুবক std::vector<RTLIL::SigBit> &bits);
	SigSpec(ধ্রুবক pool<RTLIL::SigBit> &bits);
	SigSpec(ধ্রুবক std::set<RTLIL::SigBit> &bits);
	explicit SigSpec(bool bit);
	~SigSpec() {
		destroy();
	}

	SigSpec &operator=(ধ্রুবক SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		যদি (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		অন্যথা
			new (&bits_) std::vector<RTLIL::SigBit>(rhs.bits_);
		ফেরত *this;
	}
	SigSpec &operator=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		যদি (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		অন্যথা
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(rhs.bits_));
		ফেরত *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	কাঠামো Chunks {
		Chunks(ধ্রুবক SigSpec &spec) : spec(spec) {}
		কাঠামো const_iterator {
			using iterator_category = std::forward_iterator_tag;
			using মান_type = ধ্রুবক SigChunk &;
			using difference_type = std::ptrdiff_t;
			using pointer = ধ্রুবক SigChunk *;
			using reference = ধ্রুবক SigChunk &;

			ধ্রুবক SigSpec &spec;
			পূর্ণসংখ্যা bit_সূচক;
			SigChunk chunk;

			const_iterator(ধ্রুবক SigSpec &spec) : spec(spec) {
				bit_সূচক = 0;
				যদি (spec.rep_ == BITS)
					পরবর্তী_chunk_bits();
			}
			গণনা End { END };
			const_iterator(ধ্রুবক SigSpec &spec, End) : spec(spec) {
				bit_সূচক = spec.আকার_2();
			}
			শূন্য পরবর্তী_chunk_bits();

			ধ্রুবক SigChunk &operator*() {
				যদি (spec.rep_ == CHUNK)
					ফেরত spec.chunk_;
				ফেরত chunk;
			};
			ধ্রুবক SigChunk *operator->() { ফেরত &**this; }
			const_iterator &operator++() {
				bit_সূচক += (**this).width;
				যদি (spec.rep_ == BITS)
					পরবর্তী_chunk_bits();
				ফেরত *this;
			}
			bool operator==(ধ্রুবক const_iterator &rhs) ধ্রুবক { ফেরত bit_সূচক == rhs.bit_সূচক; }
			bool operator!=(ধ্রুবক const_iterator &rhs) ধ্রুবক { ফেরত !(*this == rhs); }
		};
		const_iterator begin() ধ্রুবক { ফেরত const_iterator(spec); }
		const_iterator end() ধ্রুবক {
			const_iterator it(spec, const_iterator::END);
			ফেরত it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rbegin() {
			ensure_chunk_vector();
			ফেরত chunk_vector.rbegin();
		}
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rend() {
			ensure_chunk_vector();
			ফেরত chunk_vector.rend();
		}
		পূর্ণসংখ্যা আকার_2() {
			ensure_chunk_vector();
			ফেরত chunk_vector.আকার_2();
		}
		পূর্ণসংখ্যা আকার_2() ধ্রুবক {
			ফেরত std::distance(begin(), end());
		}
		ধ্রুবক SigChunk &at(পূর্ণসংখ্যা সূচক) {
			ensure_chunk_vector();
			ফেরত chunk_vector.at(সূচক);
		}
		operator ধ্রুবক std::vector<RTLIL::SigChunk>&() {
			ensure_chunk_vector();
			ফেরত chunk_vector;
		}
	ব্যক্তিগত:
		শূন্য ensure_chunk_vector() {
			যদি (spec.আকার_2() > 0 && chunk_vector.empty()) {
				জন্য (ধ্রুবক RTLIL::SigChunk &c : *this)
					chunk_vector.ঠেলো_back(c);
			}
		}
		ধ্রুবক SigSpec &spec;
		std::vector<RTLIL::SigChunk> chunk_vector;
	};
	friend কাঠামো Chunks::const_iterator;

	অন্তঃরেখ Chunks chunks() ধ্রুবক { ফেরত {*this}; }
	অন্তঃরেখ ধ্রুবক SigSpec &bits() ধ্রুবক { ফেরত *this; }

	অন্তঃরেখ পূর্ণসংখ্যা আকার_2() ধ্রুবক { ফেরত rep_ == CHUNK ? chunk_.width : Get_আকার(bits_); }
	অন্তঃরেখ bool empty() ধ্রুবক { ফেরত আকার_2() == 0; };

	অন্তঃরেখ RTLIL::SigBit &operator[](পূর্ণসংখ্যা সূচক) { inline_unpack(); hash_.clear(); ফেরত bits_.at(সূচক); }
	অন্তঃরেখ RTLIL::SigBit operator[](পূর্ণসংখ্যা সূচক) ধ্রুবক {
		যদি (rep_ == CHUNK) {
			যদি (সূচক < 0 || সূচক >= chunk_.width)
				throw std::out_of_range("SigSpec::operator[]");
			যদি (chunk_.wire)
				ফেরত RTLIL::SigBit(chunk_.wire, chunk_.offset + সূচক);
			ফেরত RTLIL::SigBit(chunk_.তথ্য[সূচক]);
		}
		ফেরত bits_.at(সূচক);
	}

	অন্তঃরেখ RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.সূচক = 0; ফেরত it; }
	অন্তঃরেখ RTLIL::SigSpecIterator end() { RTLIL::SigSpecIterator it; it.sig_p = this; it.সূচক = আকার_2(); ফেরত it; }

	অন্তঃরেখ RTLIL::SigSpecConstIterator begin() ধ্রুবক { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.সূচক = 0; ফেরত it; }
	অন্তঃরেখ RTLIL::SigSpecConstIterator end() ধ্রুবক { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.সূচক = আকার_2(); ফেরত it; }

	শূন্য sort();
	শূন্য sort_and_unify();

	শূন্য replace(ধ্রুবক RTLIL::SigSpec &pattern, ধ্রুবক RTLIL::SigSpec &with);
	শূন্য replace(ধ্রুবক RTLIL::SigSpec &pattern, ধ্রুবক RTLIL::SigSpec &with, RTLIL::SigSpec *other) ধ্রুবক;

	শূন্য replace(ধ্রুবক dict<RTLIL::SigBit, RTLIL::SigBit> &rules);
	শূন্য replace(ধ্রুবক dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ধ্রুবক;

	শূন্য replace(ধ্রুবক std::map<RTLIL::SigBit, RTLIL::SigBit> &rules);
	শূন্য replace(ধ্রুবক std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ধ্রুবক;

	শূন্য replace(পূর্ণসংখ্যা offset, ধ্রুবক RTLIL::SigSpec &with);

	শূন্য remove(ধ্রুবক RTLIL::SigSpec &pattern);
	শূন্য remove(ধ্রুবক RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) ধ্রুবক;
	শূন্য remove2(ধ্রুবক RTLIL::SigSpec &pattern, RTLIL::SigSpec *other);

	শূন্য remove(ধ্রুবক pool<RTLIL::SigBit> &pattern);
	শূন্য remove(ধ্রুবক pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) ধ্রুবক;
	শূন্য remove2(ধ্রুবক pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	শূন্য remove2(ধ্রুবক std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	শূন্য remove2(ধ্রুবক pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other);

	শূন্য remove(পূর্ণসংখ্যা offset, পূর্ণসংখ্যা দৈর্ঘ্য = 1);
	শূন্য remove_const();

	RTLIL::SigSpec extract(ধ্রুবক RTLIL::SigSpec &pattern, ধ্রুবক RTLIL::SigSpec *other = ফাঁকা) ধ্রুবক;
	RTLIL::SigSpec extract(ধ্রুবক pool<RTLIL::SigBit> &pattern, ধ্রুবক RTLIL::SigSpec *other = ফাঁকা) ধ্রুবক;
	RTLIL::SigSpec extract(পূর্ণসংখ্যা offset, পূর্ণসংখ্যা দৈর্ঘ্য = 1) ধ্রুবক;
	RTLIL::SigSpec extract_end(পূর্ণসংখ্যা offset) ধ্রুবক { ফেরত extract(offset, আকার_2() - offset); }

	শূন্য rewrite_wires(std::function<শূন্য(RTLIL::Wire*& wire)> rewrite);

	RTLIL::SigBit lsb() ধ্রুবক { log_assert(আকার_2()); ফেরত (*this)[0]; };
	RTLIL::SigBit msb() ধ্রুবক { log_assert(আকার_2()); ফেরত (*this)[আকার_2() - 1]; };
	RTLIL::SigBit front() ধ্রুবক { ফেরত (*this)[0]; }
	RTLIL::SigBit back() ধ্রুবক { ফেরত (*this)[আকার_2() - 1]; }

	শূন্য append(ধ্রুবক RTLIL::SigSpec &সংকেত);
	অন্তঃরেখ শূন্য append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	অন্তঃরেখ শূন্য append(ধ্রুবক RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	অন্তঃরেখ শূন্য append(ধ্রুবক RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	শূন্য append(ধ্রুবক RTLIL::SigBit &bit);
	অন্তঃরেখ শূন্য append(RTLIL::State state) { append(RTLIL::SigBit(state)); }
	অন্তঃরেখ শূন্য append(bool bool_) { append(RTLIL::SigBit(bool_)); }

	শূন্য extend_u0(পূর্ণসংখ্যা width, bool is_signed = false);

	RTLIL::SigSpec repeat(পূর্ণসংখ্যা অঙ্ক) ধ্রুবক;

	শূন্য reverse() { inline_unpack(); std::reverse(bits_.begin(), bits_.end()); }

	bool operator <(ধ্রুবক RTLIL::SigSpec &other) ধ্রুবক;
	bool operator ==(ধ্রুবক RTLIL::SigSpec &other) ধ্রুবক;
	অন্তঃরেখ bool operator !=(ধ্রুবক RTLIL::SigSpec &other) ধ্রুবক { ফেরত !(*this == other); }

	bool is_wire() ধ্রুবক;
	bool is_chunk() ধ্রুবক;
	অন্তঃরেখ bool is_bit() ধ্রুবক { ফেরত আকার_2() == 1; }

	bool known_driver() ধ্রুবক;

	bool is_fully_const() ধ্রুবক;
	bool is_fully_zero() ধ্রুবক;
	bool is_fully_ones() ধ্রুবক;
	bool is_fully_def() ধ্রুবক;
	bool is_fully_undef() ধ্রুবক;
	bool has_const() ধ্রুবক;
	bool has_const(State state) ধ্রুবক;
	bool has_marked_bits() ধ্রুবক;
	bool is_onehot(পূর্ণসংখ্যা *pos = nullptr) ধ্রুবক;

	bool as_bool() ধ্রুবক;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	পূর্ণসংখ্যা as_int(bool is_signed = false) ধ্রুবক;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_to_int(bool is_signed = false) ধ্রুবক;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<পূর্ণসংখ্যা> try_as_int(bool is_signed = false) ধ্রুবক;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	পূর্ণসংখ্যা as_int_saturating(bool is_signed = false) ধ্রুবক;

	std::string as_string() ধ্রুবক;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() ধ্রুবক;
	RTLIL::Const as_const() ধ্রুবক;
	RTLIL::Wire *as_wire() ধ্রুবক;
	RTLIL::SigChunk as_chunk() ধ্রুবক;
	RTLIL::SigBit as_bit() ধ্রুবক;

	bool match(ধ্রুবক অক্ষর* pattern) ধ্রুবক;

	std::set<RTLIL::SigBit> to_sigbit_set() ধ্রুবক;
	pool<RTLIL::SigBit> to_sigbit_pool() ধ্রুবক;
	std::vector<RTLIL::SigBit> to_sigbit_vector() ধ্রুবক;
	std::map<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_map(ধ্রুবক RTLIL::SigSpec &other) ধ্রুবক;
	dict<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_dict(ধ্রুবক RTLIL::SigSpec &other) ধ্রুবক;

	স্থির bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	স্থির bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	স্থির bool parse_rhs(ধ্রুবক RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	operator std::vector<RTLIL::SigChunk>() ধ্রুবক;
	operator std::vector<RTLIL::SigBit>() ধ্রুবক { ফেরত to_sigbit_vector(); }
	ধ্রুবক RTLIL::SigBit &at(পূর্ণসংখ্যা offset, ধ্রুবক RTLIL::SigBit &defval) { ফেরত offset < আকার_2() ? (*this)[offset] : defval; }
	RTLIL::SigBit& at(পূর্ণসংখ্যা offset) { ফেরত (*this)[offset]; }
	RTLIL::SigBit at(পূর্ণসংখ্যা offset) ধ্রুবক { ফেরত (*this)[offset]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক {
		Hasher::hash_t val;
		যদি (std::optional<Hasher::hash_t> current = hash_.পঠ())
			val = *current;
		অন্যথা
			val = updhash();
		h.eat(val);
		ফেরত h;
	}

#ifndef NDEBUG
	শূন্য check(ধ্রুবক Module *mod = nullptr) ধ্রুবক;
#অন্যথা
	শূন্য check(ধ্রুবক Module *mod = nullptr) ধ্রুবক { (শূন্য)mod; }
#endif
};

কাঠামো RTLIL::Selection
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
	bool boxed_module(RTLIL::IdString mod_name) ধ্রুবক;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ধ্রুবক;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) ধ্রুবক;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ধ্রুবক;

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
	bool selects_all() ধ্রুবক {
		ফেরত full_selection || complete_selection;
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
	bool empty() ধ্রুবক {
		ফেরত !selects_all() && selected_modules.empty() && selected_members.empty();
	}

	// clear this selection, leaving it empty
	শূন্য clear();

	// create a new selection which is empty
	স্থির Selection EmptySelection(RTLIL::Design *design = nullptr) { ফেরত Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	স্থির Selection FullSelection(RTLIL::Design *design = nullptr) { ফেরত Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	স্থির Selection CompleteSelection(RTLIL::Design *design = nullptr) { ফেরত Selection(true, true, design); };
};

কাঠামো RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক { h.eat(hashidx_); ফেরত h; }

	Monitor() {
		স্থির unsigned পূর্ণসংখ্যা hashidx_গুনতি = 123456789;
		hashidx_গুনতি = mkhash_xorshift(hashidx_গুনতি);
		hashidx_ = hashidx_গুনতি;
	}

	আভাসী ~Monitor() { }
	আভাসী শূন্য notify_module_add(RTLIL::Module*) { }
	আভাসী শূন্য notify_module_del(RTLIL::Module*) { }
	আভাসী শূন্য notify_connect(RTLIL::Cell*, RTLIL::IdString, ধ্রুবক RTLIL::SigSpec&, ধ্রুবক RTLIL::SigSpec&) { }
	আভাসী শূন্য notify_connect(RTLIL::Module*, ধ্রুবক RTLIL::SigSig&) { }
	আভাসী শূন্য notify_connect(RTLIL::Module*, ধ্রুবক std::vector<RTLIL::SigSig>&) { }
	আভাসী শূন্য notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
কাঠামো define_map_t;

template<typename N>
অন্তঃরেখ constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::Leaf> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ধ্রুবক অক্ষর*> || std::is_same_v<std::decay_t<N>, অক্ষর*>;

#define YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, পূর্ণসংখ্যা> = 0

template<typename N>
অন্তঃরেখ constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ধ্রুবক অক্ষর *> || std::is_same_v<std::decay_t<N>, অক্ষর *>;

#define YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, পূর্ণসংখ্যা> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#define YS_NAME_FWD_POOL(_func, _pool) \
	template<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(N name, Rest&&... rest) \
		{ ফেরত _func(_pool.add(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#define YS_NAME_FWD_2ND_POOL(_func, _pool) \
	template<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(T &&first, N name) \
		{ ফেরত _func(std::forward<T>(first), _pool.add(std::move(name))); }

#define YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#define YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

কাঠামো RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক { h.eat(hashidx_); ফেরত h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	শূন্য bufNormalize(bool enable=true);

	পূর্ণসংখ্যা refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

ব্যক্তিগত:
	TwinePool twines_;

সর্বজনীন:
	TwinePool &twines() { ফেরত twines_; }
	ধ্রুবক TwinePool &twines() ধ্রুবক { ফেরত twines_; }

	শূন্য absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	আকার_t gc_twines();

	std::vector<std::unique_ptr<AST::Ast_গ্রন্থি>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_defines;

	std::vector<RTLIL::Selection> selection_স্তূপ;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_active_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ধ্রুবক RTLIL::Module *module(RTLIL::IdString name) ধ্রুবক;
	template<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ ফেরত module(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ধ্রুবক RTLIL::Module *module(N name) ধ্রুবক
		{ ফেরত module(twines().find(std::string_view(name))); }
	RTLIL::Module *শীর্ষ_module() ধ্রুবক;

	bool has(RTLIL::IdString id) ধ্রুবক {
		twines_.check_owned(id);
		ফেরত modules_.গুনতি(id) != 0;
	}
	template<typename N, YS_NAME_STRING(N)> bool has(N name) ধ্রুবক
		{ ফেরত has(twines().find(std::string_view(name))); }

	শূন্য add(RTLIL::Module *module);

	RTLIL::Module *addModule(RTLIL::IdString name);
	YS_NAME_FWD_POOL(addModule, twines())
	শূন্য remove(RTLIL::Module *module);
	শূন্য rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	শূন্য scratchpad_unset(ধ্রুবক std::string &varname);

	শূন্য scratchpad_set_int(ধ্রুবক std::string &varname, পূর্ণসংখ্যা মান);
	শূন্য scratchpad_set_bool(ধ্রুবক std::string &varname, bool মান);
	শূন্য scratchpad_set_string(ধ্রুবক std::string &varname, std::string মান);

	পূর্ণসংখ্যা scratchpad_get_int(ধ্রুবক std::string &varname, পূর্ণসংখ্যা default_মান_2 = 0) ধ্রুবক;
	bool scratchpad_get_bool(ধ্রুবক std::string &varname, bool default_মান_2 = false) ধ্রুবক;
	std::string scratchpad_get_string(ধ্রুবক std::string &varname, ধ্রুবক std::string &default_মান_2 = std::string()) ধ্রুবক;

	শূন্য sort();
	শূন্য sort_modules();
	শূন্য check();
	শূন্য optimize();

	শূন্য clone_into(RTLIL::Design *dst) ধ্রুবক;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ধ্রুবক;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) ধ্রুবক;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ধ্রুবক;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ধ্রুবক;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::Module *mod) ধ্রুবক;

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
		ফেরত selection_স্তূপ.back();
	}

	// get the current selection
	ধ্রুবক RTLIL::Selection &selection() ধ্রুবক {
		ফেরত selection_স্তূপ.back();
	}

	// is the current selection a full selection (no boxes)
	bool full_selection() ধ্রুবক {
		ফেরত selection().full_selection;
	}

	// is the given module in the current selection
	template<typename T1> bool selected(T1 *module) ধ্রুবক {
		ফেরত selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	template<typename T1, typename T2> bool selected(T1 *module, T2 *member) ধ্রুবক {
		ফেরত selected_member(module->name, member->name);
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
	) ধ্রুবক;

	// returns all selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_modules() ধ্রুবক { ফেরত selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::vector<RTLIL::Module*> selected_unboxed_modules() ধ্রুবক { ফেরত selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_modules_warn() ধ্রুবক { ফেরত selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::vector<RTLIL::Module*> selected_whole_modules() ধ্রুবক { ফেরত selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_whole_modules() ধ্রুবক { ফেরত selected_modules(SELECT_WHOLE_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::vector<RTLIL::Module*> selected_whole_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ধ্রুবক { ফেরত selected_modules(SELECT_WHOLE_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules() ধ্রুবক { ফেরত selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules_warn() ধ্রুবক { ফেরত selected_modules(SELECT_WHOLE_WARN, SB_UNBOXED_WARN); }

	স্থির std::map<unsigned পূর্ণসংখ্যা, RTLIL::Design*> *get_all_designs(শূন্য);

	std::string to_rtlil_str(bool only_selected = true) ধ্রুবক;
};

কাঠামো RTLIL::Module : সর্বজনীন RTLIL::AttrObject
{
	friend কাঠামো RTLIL::Cell;
	friend কাঠামো RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক { h.eat(hashidx_); ফেরত h; }

সুরক্ষিত:
	শূন্য add(RTLIL::Wire *wire);
	শূন্য add(RTLIL::Cell *cell);
	শূন্য add(RTLIL::Process *process);

সর্বজনীন:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ধ্রুবক { ফেরত design->twines(); }

	পূর্ণসংখ্যা refcount_wires_;
	পূর্ণসংখ্যা refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::vector<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_মানসমূহ;
	dict<RTLIL::IdString, RTLIL::Memory*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	আভাসী ~Module();
	আভাসী RTLIL::IdString derive(RTLIL::Design *design, ধ্রুবক dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	আভাসী RTLIL::IdString derive(RTLIL::Design *design, ধ্রুবক dict<RTLIL::IdString, RTLIL::Const> &parameters, ধ্রুবক dict<RTLIL::IdString, RTLIL::Module*> &interfaces, ধ্রুবক dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	আভাসী আকার_t গুনতি_id(RTLIL::IdString id);
	আভাসী শূন্য expand_interfaces(RTLIL::Design *design, ধ্রুবক dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	আভাসী bool reprocess_if_necessary(RTLIL::Design *design);

	আভাসী শূন্য sort();
	আভাসী শূন্য check();
	আভাসী শূন্য optimize();
	আভাসী শূন্য makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ধ্রুবক {
		ফেরত get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	শূন্য connect(ধ্রুবক RTLIL::SigSig &conn);
	শূন্য connect(ধ্রুবক RTLIL::SigSpec &lhs, ধ্রুবক RTLIL::SigSpec &rhs);
	শূন্য new_connections(ধ্রুবক std::vector<RTLIL::SigSig> &new_conn);
	ধ্রুবক std::vector<RTLIL::SigSig> &connections() ধ্রুবক;

	std::vector<RTLIL::IdString> ports;
	শূন্য fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_queue;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_port_queue;
	pool<RTLIL::Wire *> buf_norm_wire_queue;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_সূচক;
	শূন্য bufNormalize();

	template<typename T> শূন্য rewrite_sigspecs(T &functor);
	template<typename T> শূন্য rewrite_sigspecs2(T &functor);
	শূন্য cloneInto(RTLIL::Module *new_mod) ধ্রুবক;
	আভাসী RTLIL::Module *clone() ধ্রুবক;
	আভাসী RTLIL::Module *clone(RTLIL::Design *dst) ধ্রুবক;
	আভাসী RTLIL::Module *clone(RTLIL::Design *dst, IdString target_name) ধ্রুবক;

	bool has_memories() ধ্রুবক;
	bool has_processes() ধ্রুবক;

	bool has_memories_warn() ধ্রুবক;
	bool has_processes_warn() ধ্রুবক;

	bool is_selected() ধ্রুবক;
	bool is_selected_whole() ধ্রুবক;

	std::vector<RTLIL::Wire*> selected_wires() ধ্রুবক;
	std::vector<RTLIL::Cell*> selected_cells() ধ্রুবক;
	std::vector<RTLIL::Memory*> selected_memories() ধ্রুবক;
	std::vector<RTLIL::Process*> selected_processes() ধ্রুবক;
	std::vector<RTLIL::SelectedMember> selected_members() ধ্রুবক;

	template<typename T> bool selected(T *member) ধ্রুবক {
		ফেরত design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		যদি (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		ফেরত it == wires_.end() ? nullptr : it->second;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		যদি (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		ফেরত it == cells_.end() ? nullptr : it->second;
	}

	ধ্রুবক RTLIL::Wire* wire(RTLIL::IdString id) ধ্রুবক{
		যদি (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		ফেরত it == wires_.end() ? nullptr : it->second;
	}
	ধ্রুবক RTLIL::Cell* cell(RTLIL::IdString id) ধ্রুবক {
		যদি (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		ফেরত it == cells_.end() ? nullptr : it->second;
	}

	template<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ ফেরত wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ ফেরত cell(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ধ্রুবক RTLIL::Wire *wire(N name) ধ্রুবক
		{ ফেরত wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ধ্রুবক RTLIL::Cell *cell(N name) ধ্রুবক
		{ ফেরত cell(twines().find(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { ফেরত RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	পূর্ণসংখ্যা wires_আকার() ধ্রুবক { ফেরত wires_.আকার_2(); }
	RTLIL::Wire* wire_at(পূর্ণসংখ্যা সূচক) ধ্রুবক { ফেরত wires_.element(সূচক)->second; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { ফেরত RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	পূর্ণসংখ্যা cells_আকার() ধ্রুবক { ফেরত cells_.আকার_2(); }
	RTLIL::Cell* cell_at(পূর্ণসংখ্যা সূচক) ধ্রুবক { ফেরত cells_.element(সূচক)->second; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	শূন্য remove(ধ্রুবক pool<RTLIL::Wire*> &wires);
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
	RTLIL::IdString uniquify(RTLIL::IdString name, পূর্ণসংখ্যা &সূচক);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *addWire(RTLIL::IdString name, পূর্ণসংখ্যা width = 1);
	RTLIL::Wire *addWire(RTLIL::IdString name, ধ্রুবক RTLIL::Wire *other);
	YS_NAME_FWD(addWire)

	RTLIL::Cell *addCell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *addCell(RTLIL::IdString name, ধ্রুবক RTLIL::Cell *other);
	YS_NAME_FWD(addCell)
	template<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *addCell(RTLIL::IdString name, T type) { ফেরত addCell(name, design->twines().add(std::move(type))); }

	RTLIL::Memory *addMemory(RTLIL::IdString name);
	YS_NAME_FWD(addMemory)
	RTLIL::Memory *addMemory(RTLIL::IdString name, ধ্রুবক RTLIL::Memory *other);

	RTLIL::Process *addProcess(RTLIL::IdString name);
	YS_NAME_FWD(addProcess)
	RTLIL::Process *addProcess(RTLIL::IdString name, ধ্রুবক RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* addNot     (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addPos     (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addBuf     (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addBarrier (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addNeg     (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addAnd  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addOr   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addXor  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addXnor (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addReduceAnd  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addReduceOr   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addReduceXor  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addReduceXnor (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addReduceBool (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addShl    (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addShr    (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addSshl   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addSshr   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addShift  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addShiftx (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addLt  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addLe  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addEq  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addNe  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addEqx (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addNex (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addGe  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addGt  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addAdd (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addSub (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addMul (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	// truncating division
	RTLIL::Cell* addDiv (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	// truncating modulo
	RTLIL::Cell* addMod (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDivFloor (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addModFloor (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addPow (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addFa (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_c, ধ্রুবক RTLIL::SigSpec &sig_x, ধ্রুবক RTLIL::SigSpec &sig_y, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addLogicNot (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addLogicAnd (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addLogicOr  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, bool is_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addMux  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_s, ধ্রুবক RTLIL::SigSpec &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addPmux (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_s, ধ্রুবক RTLIL::SigSpec &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addBmux (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_s, ধ্রুবক RTLIL::SigSpec &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDemux (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_s, ধ্রুবক RTLIL::SigSpec &sig_y, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addBweqx  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addBwmux  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_s, ধ্রুবক RTLIL::SigSpec &sig_y, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addSlice  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_y, RTLIL::Const offset, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addConcat (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addLut    (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_y, RTLIL::Const lut, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addTribuf (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAssert (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAssume (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addLive   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addFair   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addCover  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addEquiv  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_y, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addSr    (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_set, ধ্রুবক RTLIL::SigSpec &sig_clr, ধ্রুবক RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addFf    (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDff   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_d,   ধ্রুবক RTLIL::SigSpec &sig_q, bool clk_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDffe  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_en,  ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDffsr (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_set, ধ্রুবক RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDffsre (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_set, ধ্রুবক RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAdff (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_arst, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, RTLIL::Const arst_মান, bool clk_polarity = true, bool arst_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAdffe (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_arst,  ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, RTLIL::Const arst_মান, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAldff (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_aload, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, ধ্রুবক RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAldffe (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_aload,  ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, ধ্রুবক RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addSdff (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_srst, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, RTLIL::Const srst_মান, bool clk_polarity = true, bool srst_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addSdffe (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_srst,  ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, RTLIL::Const srst_মান, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addSdffce (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_srst, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, RTLIL::Const srst_মান, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDlatch (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, bool en_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAdlatch (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_arst, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, RTLIL::Const arst_মান, bool en_polarity = true, bool arst_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDlatchsr (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_set, ধ্রুবক RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addBufGate    (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addNotGate    (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAndGate    (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addNandGate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addOrGate     (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addNorGate    (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addXorGate    (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addXnorGate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAndnotGate (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addOrnotGate  (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addMuxGate    (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_s, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addNmuxGate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_s, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAoi3Gate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_c, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addOai3Gate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_c, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAoi4Gate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_c, ধ্রুবক RTLIL::SigBit &sig_d, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addOai4Gate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_c, ধ্রুবক RTLIL::SigBit &sig_d, ধ্রুবক RTLIL::SigBit &sig_y, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addSrGate     (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_set, ধ্রুবক RTLIL::SigSpec &sig_clr,
			ধ্রুবক RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addFfGate     (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDffGate    (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, bool clk_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDffeGate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDffsrGate  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_set, ধ্রুবক RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDffsreGate (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_set, ধ্রুবক RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAdffGate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_arst, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q,
			bool arst_মান = false, bool clk_polarity = true, bool arst_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAdffeGate  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_arst, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q,
			bool arst_মান = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAldffGate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_aload, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q,
			ধ্রুবক RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAldffeGate  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_aload, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q,
			ধ্রুবক RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addSdffGate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_srst, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q,
			bool srst_মান = false, bool clk_polarity = true, bool srst_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addSdffeGate  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_srst, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q,
			bool srst_মান = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addSdffceGate (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_clk, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_srst, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q,
			bool srst_মান = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDlatchGate (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, bool en_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addAdlatchGate(RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_arst, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q,
			bool arst_মান = false, bool en_polarity = true, bool arst_polarity = true, ধ্রুবক std::string &src = "");
	RTLIL::Cell* addDlatchsrGate  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_en, ধ্রুবক RTLIL::SigSpec &sig_set, ধ্রুবক RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ধ্রুবক std::string &src = "");

	RTLIL::Cell* addAnyinit(RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_d, ধ্রুবক RTLIL::SigSpec &sig_q, ধ্রুবক std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Shift  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::SigSpec Add (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, bool is_signed = false, ধ্রুবক std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_s, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_s, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_s, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_s, ধ্রুবক std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_b, ধ্রুবক RTLIL::SigSpec &sig_s, ধ্রুবক std::string &src = "");

	RTLIL::SigBit BufGate    (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক std::string &src = "");
	RTLIL::SigBit NotGate    (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক std::string &src = "");
	RTLIL::SigBit AndGate    (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক std::string &src = "");
	RTLIL::SigBit NandGate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক std::string &src = "");
	RTLIL::SigBit OrGate     (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক std::string &src = "");
	RTLIL::SigBit NorGate    (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক std::string &src = "");
	RTLIL::SigBit XorGate    (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক std::string &src = "");
	RTLIL::SigBit XnorGate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক std::string &src = "");
	RTLIL::SigBit AndnotGate (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক std::string &src = "");
	RTLIL::SigBit OrnotGate  (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক std::string &src = "");
	RTLIL::SigBit MuxGate    (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_s, ধ্রুবক std::string &src = "");
	RTLIL::SigBit NmuxGate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_s, ধ্রুবক std::string &src = "");
	RTLIL::SigBit Aoi3Gate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_c, ধ্রুবক std::string &src = "");
	RTLIL::SigBit Oai3Gate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_c, ধ্রুবক std::string &src = "");
	RTLIL::SigBit Aoi4Gate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_c, ধ্রুবক RTLIL::SigBit &sig_d, ধ্রুবক std::string &src = "");
	RTLIL::SigBit Oai4Gate   (RTLIL::IdString name, ধ্রুবক RTLIL::SigBit &sig_a, ধ্রুবক RTLIL::SigBit &sig_b, ধ্রুবক RTLIL::SigBit &sig_c, ধ্রুবক RTLIL::SigBit &sig_d, ধ্রুবক std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, পূর্ণসংখ্যা width = 1, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, পূর্ণসংখ্যা width = 1, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, পূর্ণসংখ্যা width = 1, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, পূর্ণসংখ্যা width = 1, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ধ্রুবক std::string &src = "");

	RTLIL::SigSpec SetTag          (RTLIL::IdString name, ধ্রুবক std::string &tag, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_s, ধ্রুবক RTLIL::SigSpec &sig_c, ধ্রুবক std::string &src = "");
	RTLIL::Cell*   addSetTag       (RTLIL::IdString name, ধ্রুবক std::string &tag, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_s, ধ্রুবক RTLIL::SigSpec &sig_c, ধ্রুবক RTLIL::SigSpec &sig_y, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec GetTag          (RTLIL::IdString name, ধ্রুবক std::string &tag, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক std::string &src = "");
	RTLIL::Cell*   addOverwriteTag (RTLIL::IdString name, ধ্রুবক std::string &tag, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক RTLIL::SigSpec &sig_s, ধ্রুবক RTLIL::SigSpec &sig_c, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec OriginalTag     (RTLIL::IdString name, ধ্রুবক std::string &tag, ধ্রুবক RTLIL::SigSpec &sig_a, ধ্রুবক std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ধ্রুবক RTLIL::SigSpec &sig_e, ধ্রুবক std::string &src = "");
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

	std::string to_rtlil_str() ধ্রুবক;
#ifdef YOSYS_ENABLE_PYTHON
	স্থির std::map<unsigned পূর্ণসংখ্যা, RTLIL::Module*> *get_all_modules(শূন্য);
#endif
};

কাঠামো RTLIL::Wire : সর্বজনীন RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক { h.eat(hashidx_); ফেরত h; }

সুরক্ষিত:
	// use module->addWire() and module->remove() to create or destroy wires
	friend কাঠামো RTLIL::Module;
	Wire();
	~Wire();

	friend কাঠামো RTLIL::Design;
	friend কাঠামো RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driverPort_;

সর্বজনীন:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	শূন্য operator=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	পূর্ণসংখ্যা width, start_offset, port_id;
	bool port_প্রবেশ, port_output, upto, is_signed;

	RTLIL::Design *design() ধ্রুবক { ফেরত module ? module->design : nullptr; }
	TwinePool &twines() ধ্রুবক { ফেরত module->twines(); }

	bool known_driver() ধ্রুবক { ফেরত driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ধ্রুবক    { log_assert(driverCell_); ফেরত driverCell_; };
	RTLIL::IdString driverPort() ধ্রুবক { log_assert(driverCell_); ফেরত driverPort_; };

	পূর্ণসংখ্যা from_hdl_সূচক(পূর্ণসংখ্যা hdl_সূচক) {
		পূর্ণসংখ্যা zero_সূচক = hdl_সূচক - start_offset;
		পূর্ণসংখ্যা rtlil_সূচক = upto ? width - 1 - zero_সূচক : zero_সূচক;
		ফেরত rtlil_সূচক >= 0 && rtlil_সূচক < width ? rtlil_সূচক : INT_MIN;
	}

	পূর্ণসংখ্যা to_hdl_সূচক(পূর্ণসংখ্যা rtlil_সূচক) {
		যদি (rtlil_সূচক < 0 || rtlil_সূচক >= width)
			ফেরত INT_MIN;
		পূর্ণসংখ্যা zero_সূচক = upto ? width - 1 - rtlil_সূচক : rtlil_সূচক;
		ফেরত zero_সূচক + start_offset;
	}

	std::string to_rtlil_str() ধ্রুবক;

#ifdef YOSYS_ENABLE_PYTHON
	স্থির std::map<unsigned পূর্ণসংখ্যা, RTLIL::Wire*> *get_all_wires(শূন্য);
#endif
};

অন্তঃরেখ পূর্ণসংখ্যা Get_আকার(RTLIL::Wire *wire) {
	ফেরত wire->width;
}

কাঠামো RTLIL::Memory : সর্বজনীন RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক { h.eat(hashidx_); ফেরত h; }

	Memory();
	~Memory();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() ধ্রুবক { ফেরত module ? module->design : nullptr; }
	TwinePool &twines() ধ্রুবক { ফেরত module->twines(); }

	RTLIL::MemoryNameId name;

	পূর্ণসংখ্যা width, start_offset, আকার_2;
#ifdef YOSYS_ENABLE_PYTHON
	স্থির std::map<unsigned পূর্ণসংখ্যা, RTLIL::Memory*> *get_all_memorys(শূন্য);
#endif

	std::string to_rtlil_str() ধ্রুবক;
};

কাঠামো RTLIL::Cell : সর্বজনীন RTLIL::AttrObject
{
ব্যক্তিগত:
	শূন্য init_সূচক();

	bool bufnorm_handle_setPort(IdString portname, RTLIL::SigSpec &সংকেত, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
সর্বজনীন:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক { h.eat(hashidx_); ফেরত h; }

সুরক্ষিত:
	// use module->addCell() and module->remove() to create or destroy cells
	friend কাঠামো RTLIL::Module;
	Cell();
	~Cell();

সর্বজনীন:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	শূন্য operator=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() ধ্রুবক { ফেরত module ? module->design : nullptr; }
	TwinePool &twines() ধ্রুবক { ফেরত module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool hasPort(RTLIL::IdString portname) ধ্রুবক;
	শূন্য unsetPort(RTLIL::IdString portname);
	শূন্য setPort(RTLIL::IdString portname, RTLIL::SigSpec সংকেত);
	template<typename N, YS_NAME_STRING(N)> শূন্য setPort(N portname, RTLIL::SigSpec সংকেত)
		{ setPort(module->twines().add(std::move(portname)), std::move(সংকেত)); }
	ধ্রুবক RTLIL::SigSpec &getPort(RTLIL::IdString portname) ধ্রুবক;
	ধ্রুবক dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ধ্রুবক;

	bool known() ধ্রুবক;
	bool প্রবেশ(RTLIL::IdString portname) ধ্রুবক;
	bool output(RTLIL::IdString portname) ধ্রুবক;
	PortDir port_dir(RTLIL::IdString portname) ধ্রুবক;

	bool hasParam(RTLIL::IdString paramname) ধ্রুবক;
	শূন্য unsetParam(RTLIL::IdString paramname);
	শূন্য setParam(RTLIL::IdString paramname, RTLIL::Const মান);
	ধ্রুবক RTLIL::Const &getParam(RTLIL::IdString paramname) ধ্রুবক;

	template<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ধ্রুবক
		{ ফেরত hasParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> শূন্য unsetParam(N name)
		{ unsetParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> শূন্য setParam(N name, RTLIL::Const মান)
		{ setParam(module->twines().add(std::move(name)), std::move(মান)); }
	template<typename N, YS_NAME_STRING(N)> ধ্রুবক RTLIL::Const &getParam(N name) ধ্রুবক
		{ ফেরত getParam(module->twines().find(std::string_view(name))); }

	শূন্য sort();
	শূন্য check();
	শূন্য fixup_parameters(bool set_a_signed = false, bool set_b_signed = false);

	bool has_keep_attr() ধ্রুবক;

	template<typename T> শূন্য rewrite_sigspecs(T &functor);
	template<typename T> শূন্য rewrite_sigspecs2(T &functor);

#ifdef YOSYS_ENABLE_PYTHON
	স্থির std::map<unsigned পূর্ণসংখ্যা, RTLIL::Cell*> *get_all_cells(শূন্য);
#endif

	bool has_memid() ধ্রুবক;
	bool is_mem_cell() ধ্রুবক;
	bool is_builtin_ff() ধ্রুবক;

	std::string to_rtlil_str() ধ্রুবক;
};

কাঠামো RTLIL::CaseRule : সর্বজনীন RTLIL::AttrObject
{
	std::vector<RTLIL::SigSpec> compare;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::SwitchRule*> switches;

	~CaseRule();

	bool empty() ধ্রুবক;

	template<typename T> শূন্য rewrite_sigspecs(T &functor);
	template<typename T> শূন্য rewrite_sigspecs2(T &functor);
	RTLIL::CaseRule *clone() ধ্রুবক;
};

কাঠামো RTLIL::SwitchRule : সর্বজনীন RTLIL::AttrObject
{
	RTLIL::SigSpec সংকেত;
	std::vector<RTLIL::CaseRule*> cases;

	~SwitchRule();

	bool empty() ধ্রুবক;

	template<typename T> শূন্য rewrite_sigspecs(T &functor);
	template<typename T> শূন্য rewrite_sigspecs2(T &functor);
	RTLIL::SwitchRule *clone() ধ্রুবক;
};

কাঠামো RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec address;
	RTLIL::SigSpec তথ্য;
	RTLIL::SigSpec enable;
	RTLIL::Const priority_mask;
};

কাঠামো RTLIL::SyncRule
{
	RTLIL::SyncType type;
	RTLIL::SigSpec সংকেত;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::MemWriteAction> mem_write_actions;

	template<typename T> শূন্য rewrite_sigspecs(T &functor);
	template<typename T> শূন্য rewrite_sigspecs2(T &functor);
	RTLIL::SyncRule *clone() ধ্রুবক;
};

কাঠামো RTLIL::Process : সর্বজনীন RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক { h.eat(hashidx_); ফেরত h; }

সুরক্ষিত:
	// use module->addProcess() and module->remove() to create or destroy processes
	friend কাঠামো RTLIL::Module;
	Process();
	~Process();

সর্বজনীন:
	RTLIL::Module *module;
	RTLIL::CaseRule মূল_case;
	std::vector<RTLIL::SyncRule*> syncs;

	RTLIL::Design *design() ধ্রুবক { ফেরত module ? module->design : nullptr; }
	TwinePool &twines() ধ্রুবক { ফেরত module->twines(); }

	template<typename T> শূন্য rewrite_sigspecs(T &functor);
	template<typename T> শূন্য rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ধ্রুবক;

	std::string to_rtlil_str() ধ্রুবক;
};

অন্তঃরেখ RTLIL::SigBit::SigBit() : wire(ফাঁকা), তথ্য(RTLIL::State::S0) { }
অন্তঃরেখ RTLIL::SigBit::SigBit(RTLIL::State bit) : wire(ফাঁকা), তথ্য(bit) { }
অন্তঃরেখ RTLIL::SigBit::SigBit(bool bit) : wire(ফাঁকা), তথ্য(bit ? State::S1 : State::S0) { }
অন্তঃরেখ RTLIL::SigBit::SigBit(RTLIL::Wire *wire) : wire(wire), offset(0) { log_assert(wire && wire->width == 1); }
অন্তঃরেখ RTLIL::SigBit::SigBit(RTLIL::Wire *wire, পূর্ণসংখ্যা offset) : wire(wire), offset(offset) { log_assert(wire != nullptr); }
অন্তঃরেখ RTLIL::SigBit::SigBit(ধ্রুবক RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.width == 1); যদি (wire) offset = chunk.offset; অন্যথা তথ্য = chunk.তথ্য[0]; }
অন্তঃরেখ RTLIL::SigBit::SigBit(ধ্রুবক RTLIL::SigChunk &chunk, পূর্ণসংখ্যা সূচক) : wire(chunk.wire) { যদি (wire) offset = chunk.offset + সূচক; অন্যথা তথ্য = chunk.তথ্য[সূচক]; }

অন্তঃরেখ bool RTLIL::SigBit::operator<(ধ্রুবক RTLIL::SigBit &other) ধ্রুবক {
	যদি (wire == other.wire)
		ফেরত wire ? (offset < other.offset) : (তথ্য < other.তথ্য);
	যদি (wire != nullptr && other.wire != nullptr)
		ফেরত wire->name < other.wire->name;
	ফেরত (wire != nullptr) < (other.wire != nullptr);
}

অন্তঃরেখ bool RTLIL::SigBit::operator==(ধ্রুবক RTLIL::SigBit &other) ধ্রুবক {
	ফেরত (wire == other.wire) && (wire ? (offset == other.offset) : (তথ্য == other.তথ্য));
}

অন্তঃরেখ bool RTLIL::SigBit::operator!=(ধ্রুবক RTLIL::SigBit &other) ধ্রুবক {
	ফেরত (wire != other.wire) || (wire ? (offset != other.offset) : (তথ্য != other.তথ্য));
}

অন্তঃরেখ Hasher RTLIL::SigBit::hash_into(Hasher h) ধ্রুবক {
	যদি (wire) {
		h.eat(offset);
		h.eat(wire->name);
		ফেরত h;
	}
	h.eat(তথ্য);
	ফেরত h;
}


অন্তঃরেখ Hasher RTLIL::SigBit::hash_শীর্ষ() ধ্রুবক {
	Hasher h;
	যদি (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.bits();
		h.force(hashlib::legacy::djb2_add(n, offset));
		ফেরত h;
	}
	h.force(তথ্য);
	ফেরত h;
}

অন্তঃরেখ RTLIL::SigBit &RTLIL::SigSpecIterator::operator*() ধ্রুবক {
	ফেরত (*sig_p)[সূচক];
}

অন্তঃরেখ ধ্রুবক RTLIL::SigBit &RTLIL::SigSpecConstIterator::operator*() {
	bit = (*sig_p)[সূচক];
	ফেরত bit;
}

অন্তঃরেখ RTLIL::SigBit::SigBit(ধ্রুবক RTLIL::SigSpec &sig) {
	log_assert(sig.আকার_2() == 1);
	auto it = sig.chunks().begin();
	*this = SigBit(*it);
}

template<typename T>
শূন্য RTLIL::Module::rewrite_sigspecs(T &functor)
{
	জন্য (auto &it : cells_)
		it.second->rewrite_sigspecs(functor);
	জন্য (auto &it : processes)
		it.second->rewrite_sigspecs(functor);
	জন্য (auto &it : connections_) {
		functor(it.first);
		functor(it.second);
	}
}

template<typename T>
শূন্য RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	জন্য (auto &it : cells_)
		it.second->rewrite_sigspecs2(functor);
	জন্য (auto &it : processes)
		it.second->rewrite_sigspecs2(functor);
	জন্য (auto &it : connections_) {
		functor(it.first, it.second);
	}
}

template<typename T>
শূন্য RTLIL::Cell::rewrite_sigspecs(T &functor) {
	জন্য (auto &it : connections_)
		functor(it.second);
}

template<typename T>
শূন্য RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	জন্য (auto &it : connections_)
		functor(it.second);
}

template<typename T>
শূন্য RTLIL::CaseRule::rewrite_sigspecs(T &functor) {
	জন্য (auto &it : compare)
		functor(it);
	জন্য (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	জন্য (auto it : switches)
		it->rewrite_sigspecs(functor);
}

template<typename T>
শূন্য RTLIL::CaseRule::rewrite_sigspecs2(T &functor) {
	জন্য (auto &it : compare)
		functor(it);
	জন্য (auto &it : actions) {
		functor(it.first, it.second);
	}
	জন্য (auto it : switches)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
শূন্য RTLIL::SwitchRule::rewrite_sigspecs(T &functor)
{
	functor(সংকেত);
	জন্য (auto it : cases)
		it->rewrite_sigspecs(functor);
}

template<typename T>
শূন্য RTLIL::SwitchRule::rewrite_sigspecs2(T &functor)
{
	functor(সংকেত);
	জন্য (auto it : cases)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
শূন্য RTLIL::SyncRule::rewrite_sigspecs(T &functor)
{
	functor(সংকেত);
	জন্য (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	জন্য (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.তথ্য);
		functor(it.enable);
	}
}

template<typename T>
শূন্য RTLIL::SyncRule::rewrite_sigspecs2(T &functor)
{
	functor(সংকেত);
	জন্য (auto &it : actions) {
		functor(it.first, it.second);
	}
	জন্য (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.তথ্য);
		functor(it.enable);
	}
}

template<typename T>
শূন্য RTLIL::Process::rewrite_sigspecs(T &functor)
{
	মূল_case.rewrite_sigspecs(functor);
	জন্য (auto it : syncs)
		it->rewrite_sigspecs(functor);
}

template<typename T>
শূন্য RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	মূল_case.rewrite_sigspecs2(functor);
	জন্য (auto it : syncs)
		it->rewrite_sigspecs2(functor);
}

#অন্তর্ভাব "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_END

#endif
