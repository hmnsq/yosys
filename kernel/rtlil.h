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

#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/twine.h"

#ಅನ್ತರ್ಭಾವ <deque>
#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	ಗಣನಾ State : unsigned ವರ್ಣ {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	ಗಣನಾ SyncType : unsigned ವರ್ಣ {
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
	ಗಣನಾ ConstFlags : unsigned ವರ್ಣ {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	ಗಣನಾ SelectPartials : unsigned ವರ್ಣ {
		SELECT_ALL = 0,          // include partial modules
		SELECT_WHOLE_ONLY = 1,   // ignore partial modules
		SELECT_WHOLE_WARN = 2,   // call log_warning on partial module
		SELECT_WHOLE_ERR = 3,    // call log_error on partial module
		SELECT_WHOLE_CMDERR = 4  // call log_cmd_error on partial module
	};

	ಗಣನಾ SelectBoxes : unsigned ವರ್ಣ {
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

	ಗಣನಾ PortDir : unsigned ವರ್ಣ  {
		PD_UNKNOWN = 0,
		PD_ಒಳಹರಿವು = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr ಪೂರ್ಣಾಙ್ಕ WIDTH_LIMIT = 1 << 30;

	ಸಂರಚನಾ Const;
	ಸಂರಚನಾ AttrObject;
	ಸಂರಚನಾ SelectedMember;
	ಸಂರಚನಾ Selection;
	ಸಂರಚನಾ Monitor;
	ಸಂರಚನಾ Design;
	ಸಂರಚನಾ Module;
	ಸಂರಚನಾ Wire;
	ಸಂರಚನಾ Memory;
	ಸಂರಚನಾ Cell;
	ಸಂರಚನಾ SigChunk;
	ಸಂರಚನಾ SigBit;
	ಸಂರಚನಾ SigSpecIterator;
	ಸಂರಚನಾ SigSpecConstIterator;
	ಸಂರಚನಾ SigSpec;
	ಸಂರಚನಾ CaseRule;
	ಸಂರಚನಾ SwitchRule;
	ಸಂರಚನಾ MemWriteAction;
	ಸಂರಚನಾ SyncRule;
	ಸಂರಚನಾ Process;

	ಪ್ರಕಾರನಿರ್ದೇಶ std::pair<SigSpec, SigSpec> SigSig;
};

ಸಂರಚನಾ SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_PREFIX ID; }
namespace RTLIL { using YOSYS_NAMESPACE_PREFIX IdString; }

namespace RTLIL {
	ಶೂನ್ಯ copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			ಧ್ರುವ dict<IdString, RTLIL::Const> &src,
			ಧ್ರುವ RTLIL::Design *src_design, RTLIL::Design *dst_design);

	ಬಾಹ್ಯ dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	ಧ್ರುವ pool<IdString> &builtin_ff_cell_types();

	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ std::string escape_id(ಧ್ರುವ std::string &str) {
		ಯದಿ (str.size() > 0 && str[0] != '\\' && str[0] != '$')
			ಪ್ರತ್ಯಾಗಮ "\\" + str;
		ಪ್ರತ್ಯಾಗಮ str;
	}

	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ std::string unescape_id(ಧ್ರುವ std::string &str) {
		ಯದಿ (str.size() < 2)
			ಪ್ರತ್ಯಾಗಮ str;
		ಯದಿ (str[0] != '\\')
			ಪ್ರತ್ಯಾಗಮ str;
		ಯದಿ (str[1] == '$' || str[1] == '\\')
			ಪ್ರತ್ಯಾಗಮ str;
		ಯದಿ (str[1] >= '0' && str[1] <= '9')
			ಪ್ರತ್ಯಾಗಮ str;
		ಪ್ರತ್ಯಾಗಮ str.substr(1);
	}

	template <typename T> using sort_by_name_id = IdString::compare_ptr_by_name<T>;

	template <typename T> ಸಂರಚನಾ sort_by_name_str {
		bool operator()(T *a, T *b) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ a->name.lt_by_name(b->name);
		}
	};

	ಸಂರಚನಾ sort_by_id_str {
		ಧ್ರುವ TwinePool& pool;
		explicit sort_by_id_str(ಧ್ರುವ TwinePool& pool) : pool(pool) {}
		bool operator()(RTLIL::IdString a, RTLIL::IdString b) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ pool.compare_by_name(a, b) < 0;
		}
	};

	ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ std::string encode_filename(ಧ್ರುವ std::string &filename)
	{
		std::stringstream val;
		ಯದಿ (!std::any_of(filename.begin(), filename.end(), [](ವರ್ಣ c) {
			ಪ್ರತ್ಯಾಗಮ static_cast<unsigned ವರ್ಣ>(c) < 33 || static_cast<unsigned ವರ್ಣ>(c) > 126;
		})) ಪ್ರತ್ಯಾಗಮ filename;
		ಪ್ರತಿ (unsigned ವರ್ಣ ಧ್ರುವ c : filename) {
			ಯದಿ (c < 33 || c > 126)
				val << stringf("$%02x", c);
			ಅನ್ಯಥಾ
				val << c;
		}
		ಪ್ರತ್ಯಾಗಮ val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_and         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_or          (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_xor         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_xnor        (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);

	RTLIL::Const const_reduce_and  (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_reduce_or   (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_reduce_xor  (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_reduce_xnor (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_reduce_bool (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);

	RTLIL::Const const_logic_not   (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_logic_and   (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_logic_or    (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);

	RTLIL::Const const_shl         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_shr         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_sshl        (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_sshr        (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_shift       (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_shiftx      (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);

	RTLIL::Const const_lt          (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_le          (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_eq          (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_ne          (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_eqx         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_nex         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_ge          (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_gt          (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);

	RTLIL::Const const_add         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_sub         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_mul         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_div         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_divfloor    (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_modfloor    (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_mod         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_pow         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);

	RTLIL::Const const_pos         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_buf         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);
	RTLIL::Const const_neg         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len);

	RTLIL::Const const_mux         (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ಧ್ರುವ RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ಧ್ರುವ RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ಧ್ರುವ RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	template<typename T, typename ಕೀಲಿ_2 = IdString>
	ಸಂರಚನಾ ObjIterator {
		using iterator_category = std::forward_iterator_tag;
		using ಮೌಲ್ಯ_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		typename dict<ಕೀಲಿ_2, T>::iterator it;
		dict<ಕೀಲಿ_2, T> *list_p;
		ಪೂರ್ಣಾಙ್ಕ *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, ಪೂರ್ಣಾಙ್ಕ *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			ಯದಿ (list_p->empty()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} ಅನ್ಯಥಾ {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(ಧ್ರುವ RTLIL::ObjIterator<T, ಕೀಲಿ_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			ಯದಿ (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &operator=(ಧ್ರುವ RTLIL::ObjIterator<T, ಕೀಲಿ_2> &other) {
			ಯದಿ (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			ಯದಿ (refcount_p)
				(*refcount_p)++;
			ಪ್ರತ್ಯಾಗಮ *this;
		}

		~ObjIterator() {
			ಯದಿ (refcount_p)
				(*refcount_p)--;
		}

		ಅನ್ತಃಸ್ಥ T operator*() ಧ್ರುವ {
			log_assert(list_p != nullptr);
			ಪ್ರತ್ಯಾಗಮ it->second;
		}

		ಅನ್ತಃಸ್ಥ bool operator!=(ಧ್ರುವ RTLIL::ObjIterator<T, ಕೀಲಿ_2> &other) ಧ್ರುವ {
			ಯದಿ (list_p == nullptr || other.list_p == nullptr)
				ಪ್ರತ್ಯಾಗಮ list_p != other.list_p;
			ಪ್ರತ್ಯಾಗಮ it != other.it;
		}


		ಅನ್ತಃಸ್ಥ bool operator==(ಧ್ರುವ RTLIL::ObjIterator<T, ಕೀಲಿ_2> &other) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ !(*this != other);
		}

		ಅನ್ತಃಸ್ಥ ObjIterator<T, ಕೀಲಿ_2>& operator++() {
			log_assert(list_p != nullptr);
			ಯದಿ (++it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			ಪ್ರತ್ಯಾಗಮ *this;
		}

		ಅನ್ತಃಸ್ಥ ObjIterator<T, ಕೀಲಿ_2>& operator+=(ಪೂರ್ಣಾಙ್ಕ amt) {
			log_assert(list_p != nullptr);
			it += amt;
			ಯದಿ (it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			ಪ್ರತ್ಯಾಗಮ *this;
		}

		ಅನ್ತಃಸ್ಥ ObjIterator<T, ಕೀಲಿ_2> operator+(ಪೂರ್ಣಾಙ್ಕ amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, ಕೀಲಿ_2> new_obj(*this);
			new_obj.it += amt;
			ಯದಿ (new_obj.it == list_p->end()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			ಪ್ರತ್ಯಾಗಮ new_obj;
		}

		ಅನ್ತಃಸ್ಥ ಧ್ರುವ ObjIterator<T, ಕೀಲಿ_2> operator++(ಪೂರ್ಣಾಙ್ಕ) {
			ObjIterator<T, ಕೀಲಿ_2> result(*this);
			++(*this);
			ಪ್ರತ್ಯಾಗಮ result;
		}
	};

	template<typename T, typename ಕೀಲಿ_2 = IdString>
	ಸಂರಚನಾ ObjRange
	{
		dict<ಕೀಲಿ_2, T> *list_p;
		ಪೂರ್ಣಾಙ್ಕ *refcount_p;

		ObjRange(decltype(list_p) list_p, ಪೂರ್ಣಾಙ್ಕ *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, ಕೀಲಿ_2> begin() { ಪ್ರತ್ಯಾಗಮ RTLIL::ObjIterator<T, ಕೀಲಿ_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, ಕೀಲಿ_2> end() { ಪ್ರತ್ಯಾಗಮ RTLIL::ObjIterator<T, ಕೀಲಿ_2>(); }

		size_t size() ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ list_p->size();
		}

		operator pool<T>() ಧ್ರುವ {
			pool<T> result;
			ಪ್ರತಿ (auto &it : *list_p)
				result.ಸೇರಿಸು(it.second);
			ಪ್ರತ್ಯಾಗಮ result;
		}

		operator std::vector<T>() ಧ್ರುವ {
			std::vector<T> result;
			result.reserve(list_p->size());
			ಪ್ರತಿ (auto &it : *list_p)
				result.ತಳ್ಳು_back(it.second);
			ಪ್ರತ್ಯಾಗಮ result;
		}

		pool<T> to_pool() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ *this; }
		std::vector<T> to_vector() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ *this; }
	};
};

ಸಂರಚನಾ RTLIL::Const
{
	short ಪೂರ್ಣಾಙ್ಕ flags;
ಗುಪ್ತ:
	friend class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::vector<RTLIL::State>;
	ಗಣನಾ class backing_tag: bool { bits, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_tag tag;
	ಸಙ್ಘ {
		bitvectype bits_;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_bits() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ tag == backing_tag::bits; }
	bool is_str() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ tag == backing_tag::string; }

	bitvectype* get_if_bits() { ಪ್ರತ್ಯಾಗಮ is_bits() ? &bits_ : ರಿಕ್ತ; }
	std::string* get_if_str() { ಪ್ರತ್ಯಾಗಮ is_str() ? &str_ : ರಿಕ್ತ; }
	ಧ್ರುವ bitvectype* get_if_bits() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ is_bits() ? &bits_ : ರಿಕ್ತ; }
	ಧ್ರುವ std::string* get_if_str() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ is_str() ? &str_ : ರಿಕ್ತ; }

	bitvectype& get_bits();
	std::string& get_str();
	ಧ್ರುವ bitvectype& get_bits() ಧ್ರುವ;
	ಧ್ರುವ std::string& get_str() ಧ್ರುವ;
	std::vector<RTLIL::State>& bits_internal();
	ಶೂನ್ಯ bitvectorize_internal();

ಸಾರ್ವಜನಿಕ:
	Const() : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::vector<RTLIL::State>()) {}
	Const(std::string str);
	Const(long long val); // default width is 32
	Const(long long val, ಪೂರ್ಣಾಙ್ಕ width);
	Const(RTLIL::State bit, ಪೂರ್ಣಾಙ್ಕ width = 1);
	Const(std::vector<RTLIL::State> bits) : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::move(bits)) {}
	Const(ಧ್ರುವ std::vector<bool> &bits);
	Const(ಧ್ರುವ RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &operator =(ಧ್ರುವ RTLIL::Const &other);
	~Const();

	ಸಂರಚನಾ Builder
	{
		Builder() {}
		Builder(ಪೂರ್ಣಾಙ್ಕ expected_width) { bits.reserve(expected_width); }
		ಶೂನ್ಯ ತಳ್ಳು_back(RTLIL::State b) { bits.ತಳ್ಳು_back(b); }
		ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ static_cast<ಪೂರ್ಣಾಙ್ಕ>(bits.size()); }
		Const build() { ಪ್ರತ್ಯಾಗಮ Const(std::move(bits)); }
	ಗುಪ್ತ:
		std::vector<RTLIL::State> bits;
	};

	bool operator <(ಧ್ರುವ RTLIL::Const &other) ಧ್ರುವ;
	bool operator ==(ಧ್ರುವ RTLIL::Const &other) ಧ್ರುವ;
	bool operator !=(ಧ್ರುವ RTLIL::Const &other) ಧ್ರುವ;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::vector<RTLIL::State>& bits() { ಪ್ರತ್ಯಾಗಮ bits_internal(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	ಶೂನ್ಯ bitvectorize() ಧ್ರುವ { const_cast<Const*>(this)->bitvectorize_internal(); }

	bool as_bool() ಧ್ರುವ;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	ಪೂರ್ಣಾಙ್ಕ as_int(bool is_signed = false) ಧ್ರುವ;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_to_int(bool is_signed = false) ಧ್ರುವ;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<ಪೂರ್ಣಾಙ್ಕ> try_as_int(bool is_signed = false) ಧ್ರುವ;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	ಪೂರ್ಣಾಙ್ಕ as_int_saturating(bool is_signed = false) ಧ್ರುವ;

	ಶೂನ್ಯ tag_bare_integer_const(ಧ್ರುವ std::string &ಮೌಲ್ಯ);

	std::string as_string(ಧ್ರುವ ವರ್ಣ* any = "-") ಧ್ರುವ;
	ಸ್ಥಿರ Const from_string(ಧ್ರುವ std::string &str);
	std::vector<RTLIL::State> to_bits() ಧ್ರುವ;

	std::string decode_string() ಧ್ರುವ;
	ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ;
	bool empty() ಧ್ರುವ;

	ಶೂನ್ಯ append(ಧ್ರುವ RTLIL::Const &other);
	ಶೂನ್ಯ set(ಪೂರ್ಣಾಙ್ಕ i, RTLIL::State state) {
		bits_internal()[i] = state;
	}
	ಶೂನ್ಯ resize(ಪೂರ್ಣಾಙ್ಕ size, RTLIL::State fill) {
    log_assert(size >= 0 && size < RTLIL::WIDTH_LIMIT);
		bits_internal().resize(size, fill);
	}

	class const_iterator {
	ಗುಪ್ತ:
		ಧ್ರುವ Const* parent;
		size_t idx;

	ಸಾರ್ವಜನಿಕ:
		using iterator_category = std::bidirectional_iterator_tag;
		using ಮೌಲ್ಯ_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = ಧ್ರುವ State*;
		using reference = ಧ್ರುವ State&;

		const_iterator(ಧ್ರುವ Const& c, size_t i) : parent(&c), idx(i) {}

		State operator*() ಧ್ರುವ;

		const_iterator& operator++() { ++idx; ಪ್ರತ್ಯಾಗಮ *this; }
		const_iterator& operator--() { --idx; ಪ್ರತ್ಯಾಗಮ *this; }
		const_iterator operator++(ಪೂರ್ಣಾಙ್ಕ) { const_iterator result(*this); ++idx; ಪ್ರತ್ಯಾಗಮ result; }
		const_iterator operator--(ಪೂರ್ಣಾಙ್ಕ) { const_iterator result(*this); --idx; ಪ್ರತ್ಯಾಗಮ result; }
		const_iterator& operator+=(ಪೂರ್ಣಾಙ್ಕ i) { idx += i; ಪ್ರತ್ಯಾಗಮ *this; }

		const_iterator operator+(ಪೂರ್ಣಾಙ್ಕ add) {
			ಪ್ರತ್ಯಾಗಮ const_iterator(*parent, idx + add);
		}
		const_iterator operator-(ಪೂರ್ಣಾಙ್ಕ sub) {
			ಪ್ರತ್ಯಾಗಮ const_iterator(*parent, idx - sub);
		}
		ಪೂರ್ಣಾಙ್ಕ operator-(ಧ್ರುವ const_iterator& other) {
			ಪ್ರತ್ಯಾಗಮ idx - other.idx;
		}

		bool operator==(ಧ್ರುವ const_iterator& other) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ idx == other.idx;
		}

		bool operator!=(ಧ್ರುವ const_iterator& other) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ !(*this == other);
		}
	};

	class iterator {
	ಗುಪ್ತ:
		Const* parent;
		size_t idx;

	ಸಾರ್ವಜನಿಕ:
		class proxy {
		ಗುಪ್ತ:
			Const* parent;
			size_t idx;
		ಸಾರ್ವಜನಿಕ:
			proxy(Const* parent, size_t idx) : parent(parent), idx(idx) {}
			operator State() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (*parent)[idx]; }
			proxy& operator=(State s) { parent->set(idx, s); ಪ್ರತ್ಯಾಗಮ *this; }
			proxy& operator=(ಧ್ರುವ proxy& other) { parent->set(idx, (*other.parent)[other.idx]); ಪ್ರತ್ಯಾಗಮ *this; }
		};

		using iterator_category = std::bidirectional_iterator_tag;
		using ಮೌಲ್ಯ_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = proxy*;
		using reference = proxy;

		iterator(Const& c, size_t i) : parent(&c), idx(i) {}

		proxy operator*() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ proxy(parent, idx); }
		iterator& operator++() { ++idx; ಪ್ರತ್ಯಾಗಮ *this; }
		iterator& operator--() { --idx; ಪ್ರತ್ಯಾಗಮ *this; }
		iterator operator++(ಪೂರ್ಣಾಙ್ಕ) { iterator result(*this); ++idx; ಪ್ರತ್ಯಾಗಮ result; }
		iterator operator--(ಪೂರ್ಣಾಙ್ಕ) { iterator result(*this); --idx; ಪ್ರತ್ಯಾಗಮ result; }
		iterator& operator+=(ಪೂರ್ಣಾಙ್ಕ i) { idx += i; ಪ್ರತ್ಯಾಗಮ *this; }

		iterator operator+(ಪೂರ್ಣಾಙ್ಕ add) {
			ಪ್ರತ್ಯಾಗಮ iterator(*parent, idx + add);
		}
		iterator operator-(ಪೂರ್ಣಾಙ್ಕ sub) {
			ಪ್ರತ್ಯಾಗಮ iterator(*parent, idx - sub);
		}
		ಪೂರ್ಣಾಙ್ಕ operator-(ಧ್ರುವ iterator& other) {
			ಪ್ರತ್ಯಾಗಮ idx - other.idx;
		}

		bool operator==(ಧ್ರುವ iterator& other) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ idx == other.idx;
		}

		bool operator!=(ಧ್ರುವ iterator& other) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ !(*this == other);
		}
	};

	const_iterator begin() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ const_iterator(*this, 0);
	}
	const_iterator end() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ const_iterator(*this, size());
	}
	iterator begin() {
		ಪ್ರತ್ಯಾಗಮ iterator(*this, 0);
	}
	iterator end() {
		ಪ್ರತ್ಯಾಗಮ iterator(*this, size());
	}
	State back() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ *(end() - 1);
	}
	State front() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ *begin();
	}
	State at(size_t i) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ *const_iterator(*this, i);
	}
	State operator[](size_t i) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ *const_iterator(*this, i);
	}

	bool is_fully_zero() ಧ್ರುವ;
	bool is_fully_ones() ಧ್ರುವ;
	bool is_fully_def() ಧ್ರುವ;
	bool is_fully_undef() ಧ್ರುವ;
	bool is_fully_undef_x_only() ಧ್ರುವ;
	bool is_onehot(ಪೂರ್ಣಾಙ್ಕ *pos = nullptr) ಧ್ರುವ;

	RTLIL::Const extract(ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ len = 1, RTLIL::State padding = RTLIL::State::S0) ಧ್ರುವ;

	// find the MSB without redundant leading bits
	ಪೂರ್ಣಾಙ್ಕ get_min_size(bool is_signed) ಧ್ರುವ;

	// compress representation to the minimum required bits
	ಶೂನ್ಯ compress(bool is_signed = false);

	std::optional<ಪೂರ್ಣಾಙ್ಕ> as_int_compress(bool is_signed) ಧ್ರುವ;

	ಶೂನ್ಯ extu(ಪೂರ್ಣಾಙ್ಕ width) {
		resize(width, RTLIL::State::S0);
	}

	ಶೂನ್ಯ exts(ಪೂರ್ಣಾಙ್ಕ width) {
		resize(width, empty() ? RTLIL::State::Sx : back());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;
};

ಸಂರಚನಾ RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	bool has_attribute(RTLIL::IdString id) ಧ್ರುವ;

	ಶೂನ್ಯ set_bool_attribute(RTLIL::IdString id, bool ಮೌಲ್ಯ=true);
	bool get_bool_attribute(RTLIL::IdString id) ಧ್ರುವ;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	ಶೂನ್ಯ set_string_attribute(RTLIL::IdString id, string ಮೌಲ್ಯ);
	string get_string_attribute(RTLIL::IdString id) ಧ್ರುವ;

	ಶೂನ್ಯ set_strpool_attribute(RTLIL::IdString id, ಧ್ರುವ pool<string> &data);
	ಶೂನ್ಯ add_strpool_attribute(RTLIL::IdString id, ಧ್ರುವ pool<string> &data);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ಧ್ರುವ;

	ಶೂನ್ಯ set_src_attribute(ಧ್ರುವ std::string &src) {
		set_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ get_string_attribute(ID::src);
	}

	ಶೂನ್ಯ set_hdlname_attribute(ಧ್ರುವ vector<string> &hierarchy);
	vector<string> get_hdlname_attribute() ಧ್ರುವ;

	ಶೂನ್ಯ set_intvec_attribute(RTLIL::IdString id, ಧ್ರುವ vector<ಪೂರ್ಣಾಙ್ಕ> &data);
	vector<ಪೂರ್ಣಾಙ್ಕ> get_intvec_attribute(RTLIL::IdString id) ಧ್ರುವ;
};

#ಅನ್ತರ್ಭಾವ "kernel/rtlil_twine_compat.h"

ಸಂರಚನಾ RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

ಸಂರಚನಾ RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::vector<RTLIL::State> data; // only used if wire == NULL, LSB at index 0
	ಪೂರ್ಣಾಙ್ಕ width, offset;

	SigChunk() : wire(nullptr), width(0), offset(0) {}
	SigChunk(ಧ್ರುವ RTLIL::Const &ಮೌಲ್ಯ) : wire(nullptr), data(ಮೌಲ್ಯ.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Const &&ಮೌಲ್ಯ) : wire(nullptr), data(ಮೌಲ್ಯ.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), width(GetSize(wire)), offset(0) {}
	SigChunk(RTLIL::Wire *wire, ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ width = 1) : wire(wire), width(width), offset(offset) {}
	SigChunk(ಧ್ರುವ std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(ಪೂರ್ಣಾಙ್ಕ val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(ಪೂರ್ಣಾಙ್ಕ val, ಪೂರ್ಣಾಙ್ಕ width) : SigChunk(RTLIL::Const(val, width)) {}
	SigChunk(RTLIL::State bit, ಪೂರ್ಣಾಙ್ಕ width = 1) : SigChunk(RTLIL::Const(bit, width)) {}
	SigChunk(ಧ್ರುವ RTLIL::SigBit &bit);

	RTLIL::SigChunk extract(ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ length) ಧ್ರುವ;
	RTLIL::SigBit operator[](ಪೂರ್ಣಾಙ್ಕ offset) ಧ್ರುವ;
	ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ width; }
	ಅನ್ತಃಸ್ಥ bool is_wire() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ wire != ರಿಕ್ತ; }

	bool operator <(ಧ್ರುವ RTLIL::SigChunk &other) ಧ್ರುವ;
	bool operator ==(ಧ್ರುವ RTLIL::SigChunk &other) ಧ್ರುವ;
	bool operator !=(ಧ್ರುವ RTLIL::SigChunk &other) ಧ್ರುವ;
};

ಸಂರಚನಾ RTLIL::SigBit
{
	RTLIL::Wire *wire;
	ಸಙ್ಘ {
		RTLIL::State data; // used if wire == NULL
		ಪೂರ್ಣಾಙ್ಕ offset;        // used if wire != NULL
	};

	SigBit();
	SigBit(RTLIL::State bit);
	explicit SigBit(bool bit);
	SigBit(RTLIL::Wire *wire);
	SigBit(RTLIL::Wire *wire, ಪೂರ್ಣಾಙ್ಕ offset);
	SigBit(ಧ್ರುವ RTLIL::SigChunk &chunk);
	SigBit(ಧ್ರುವ RTLIL::SigChunk &chunk, ಪೂರ್ಣಾಙ್ಕ index);
	SigBit(ಧ್ರುವ RTLIL::SigSpec &sig);
	SigBit(ಧ್ರುವ RTLIL::SigBit &sigbit) = ಸ್ವಭಾವ;
	RTLIL::SigBit &operator =(ಧ್ರುವ RTLIL::SigBit &other) = ಸ್ವಭಾವ;

	ಅನ್ತಃಸ್ಥ bool is_wire() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ wire != ರಿಕ್ತ; }

	bool operator <(ಧ್ರುವ RTLIL::SigBit &other) ಧ್ರುವ;
	bool operator ==(ಧ್ರುವ RTLIL::SigBit &other) ಧ್ರುವ;
	bool operator !=(ಧ್ರುವ RTLIL::SigBit &other) ಧ್ರುವ;
	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ;
	[[nodiscard]] Hasher hash_ತುದಿ() ಧ್ರುವ;
};

namespace hashlib {
	template <>
	ಸಂರಚನಾ hash_ops<RTLIL::SigBit> {
		ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cmp(ಧ್ರುವ RTLIL::SigBit &a, ಧ್ರುವ RTLIL::SigBit &b) {
			ಪ್ರತ್ಯಾಗಮ a == b;
		}
		[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash(ಧ್ರುವ RTLIL::SigBit sb) {
			ಪ್ರತ್ಯಾಗಮ sb.hash_ತುದಿ();
		}
		[[nodiscard]] ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ Hasher hash_into(ಧ್ರುವ RTLIL::SigBit sb, Hasher h) {
			ಪ್ರತ್ಯಾಗಮ sb.hash_into(h);
		}
	};
};

ಸಂರಚನಾ RTLIL::SigSpecIterator
{
	ಪ್ರಕಾರನಿರ್ದೇಶ std::ಒಳಹರಿವು_iterator_tag iterator_category;
	ಪ್ರಕಾರನಿರ್ದೇಶ RTLIL::SigBit ಮೌಲ್ಯ_type;
	ಪ್ರಕಾರನಿರ್ದೇಶ ptrdiff_t difference_type;
	ಪ್ರಕಾರನಿರ್ದೇಶ RTLIL::SigBit* pointer;
	ಪ್ರಕಾರನಿರ್ದೇಶ RTLIL::SigBit& reference;

	RTLIL::SigSpec *sig_p;
	ಪೂರ್ಣಾಙ್ಕ index;

	ಅನ್ತಃಸ್ಥ RTLIL::SigBit &operator*() ಧ್ರುವ;
	ಅನ್ತಃಸ್ಥ bool operator!=(ಧ್ರುವ RTLIL::SigSpecIterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index != other.index; }
	ಅನ್ತಃಸ್ಥ bool operator==(ಧ್ರುವ RTLIL::SigSpecIterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index == other.index; }
	ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ operator++() { index++; }
};

ಸಂರಚನಾ RTLIL::SigSpecConstIterator
{
	ಪ್ರಕಾರನಿರ್ದೇಶ std::ಒಳಹರಿವು_iterator_tag iterator_category;
	ಪ್ರಕಾರನಿರ್ದೇಶ RTLIL::SigBit ಮೌಲ್ಯ_type;
	ಪ್ರಕಾರನಿರ್ದೇಶ ptrdiff_t difference_type;
	ಪ್ರಕಾರನಿರ್ದೇಶ RTLIL::SigBit* pointer;
	ಪ್ರಕಾರನಿರ್ದೇಶ RTLIL::SigBit& reference;

	ಧ್ರುವ RTLIL::SigSpec *sig_p;
	RTLIL::SigBit bit;
	ಪೂರ್ಣಾಙ್ಕ index;

	ಅನ್ತಃಸ್ಥ ಧ್ರುವ RTLIL::SigBit &operator*();
	ಅನ್ತಃಸ್ಥ bool operator!=(ಧ್ರುವ RTLIL::SigSpecConstIterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index != other.index; }
	ಅನ್ತಃಸ್ಥ bool operator==(ಧ್ರುವ RTLIL::SigSpecIterator &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ index == other.index; }
	ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ operator++() { index++; }
};

ಸಂರಚನಾ RTLIL::SigSpec
{
ಗುಪ್ತ:
	friend class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, Extract);
	ಗಣನಾ Representation : ವರ್ಣ {
		CHUNK,
		BITS,
	};
	// An AtomicHash is either clear or a nonzero integer.
	ಸಂರಚನಾ AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ಧ್ರುವ AtomicHash &rhs) : atomic_(rhs.load()) {}
		AtomicHash &operator=(ಧ್ರುವ AtomicHash &rhs) { store(rhs.load()); ಪ್ರತ್ಯಾಗಮ *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> ಪಠ() ಧ್ರುವ {
			Hasher::hash_t ಮೌಲ್ಯ = load();
			ಯದಿ (ಮೌಲ್ಯ == 0)
				ಪ್ರತ್ಯಾಗಮ std::nullopt;
			ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		ಶೂನ್ಯ set(Hasher::hash_t ಮೌಲ್ಯ) ಧ್ರುವ {
			log_assert(ಮೌಲ್ಯ != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(ಮೌಲ್ಯ, std::memory_order_relaxed);
			log_assert(old == 0 || old == ಮೌಲ್ಯ);
		}
		ಶೂನ್ಯ clear() { store(0); }
	ಗುಪ್ತ:
		ಪೂರ್ಣಾಙ್ಕ load() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ atomic_.load(std::memory_order_relaxed); }
		ಶೂನ್ಯ store(Hasher::hash_t ಮೌಲ್ಯ) ಧ್ರುವ {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(ಮೌಲ್ಯ, std::memory_order_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	ಸಙ್ಘ {
		RTLIL::SigChunk chunk_;
		std::vector<RTLIL::SigBit> bits_; // LSB at index 0
	};

	ಶೂನ್ಯ init_empty_bits() {
		rep_ = BITS;
		new (&bits_) std::vector<RTLIL::SigBit>;
	}

	ಶೂನ್ಯ unpack();
	ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ inline_unpack() {
		ಯದಿ (rep_ == CHUNK)
			unpack();
	}
	ಶೂನ್ಯ try_repack();

	Hasher::hash_t updhash() ಧ್ರುವ;
	ಶೂನ್ಯ destroy() {
		ಯದಿ (rep_ == CHUNK)
			chunk_.~SigChunk();
		ಅನ್ಯಥಾ
			bits_.~vector();
	}
	friend ಸಂರಚನಾ Chunks;

ಸಾರ್ವಜನಿಕ:
	SigSpec() { init_empty_bits(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ಧ್ರುವ SigSpec &ಮೌಲ್ಯ) : rep_(ಮೌಲ್ಯ.rep_), hash_(ಮೌಲ್ಯ.hash_) {
		ಯದಿ (ಮೌಲ್ಯ.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(ಮೌಲ್ಯ.chunk_);
		ಅನ್ಯಥಾ
			new (&bits_) std::vector<RTLIL::SigBit>(ಮೌಲ್ಯ.bits_);
	}
	SigSpec(SigSpec &&ಮೌಲ್ಯ) : rep_(ಮೌಲ್ಯ.rep_), hash_(ಮೌಲ್ಯ.hash_) {
		ಯದಿ (ಮೌಲ್ಯ.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(ಮೌಲ್ಯ.chunk_));
		ಅನ್ಯಥಾ
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(ಮೌಲ್ಯ.bits_));
	}
	SigSpec(ಧ್ರುವ RTLIL::Const &ಮೌಲ್ಯ);
	SigSpec(RTLIL::Const &&ಮೌಲ್ಯ);
	SigSpec(ಧ್ರುವ RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ width = 1);
	SigSpec(ಧ್ರುವ std::string &str);
	SigSpec(ಪೂರ್ಣಾಙ್ಕ val, ಪೂರ್ಣಾಙ್ಕ width = 32);
	SigSpec(RTLIL::State bit, ಪೂರ್ಣಾಙ್ಕ width = 1);
	SigSpec(ಧ್ರುವ RTLIL::SigBit &bit, ಪೂರ್ಣಾಙ್ಕ width = 1);
	SigSpec(ಧ್ರುವ std::vector<RTLIL::SigChunk> &chunks);
	SigSpec(ಧ್ರುವ std::vector<RTLIL::SigBit> &bits);
	SigSpec(ಧ್ರುವ pool<RTLIL::SigBit> &bits);
	SigSpec(ಧ್ರುವ std::set<RTLIL::SigBit> &bits);
	explicit SigSpec(bool bit);
	~SigSpec() {
		destroy();
	}

	SigSpec &operator=(ಧ್ರುವ SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		ಯದಿ (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		ಅನ್ಯಥಾ
			new (&bits_) std::vector<RTLIL::SigBit>(rhs.bits_);
		ಪ್ರತ್ಯಾಗಮ *this;
	}
	SigSpec &operator=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		ಯದಿ (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		ಅನ್ಯಥಾ
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(rhs.bits_));
		ಪ್ರತ್ಯಾಗಮ *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	ಸಂರಚನಾ Chunks {
		Chunks(ಧ್ರುವ SigSpec &spec) : spec(spec) {}
		ಸಂರಚನಾ const_iterator {
			using iterator_category = std::forward_iterator_tag;
			using ಮೌಲ್ಯ_type = ಧ್ರುವ SigChunk &;
			using difference_type = std::ptrdiff_t;
			using pointer = ಧ್ರುವ SigChunk *;
			using reference = ಧ್ರುವ SigChunk &;

			ಧ್ರುವ SigSpec &spec;
			ಪೂರ್ಣಾಙ್ಕ bit_index;
			SigChunk chunk;

			const_iterator(ಧ್ರುವ SigSpec &spec) : spec(spec) {
				bit_index = 0;
				ಯದಿ (spec.rep_ == BITS)
					next_chunk_bits();
			}
			ಗಣನಾ End { END };
			const_iterator(ಧ್ರುವ SigSpec &spec, End) : spec(spec) {
				bit_index = spec.size();
			}
			ಶೂನ್ಯ next_chunk_bits();

			ಧ್ರುವ SigChunk &operator*() {
				ಯದಿ (spec.rep_ == CHUNK)
					ಪ್ರತ್ಯಾಗಮ spec.chunk_;
				ಪ್ರತ್ಯಾಗಮ chunk;
			};
			ಧ್ರುವ SigChunk *operator->() { ಪ್ರತ್ಯಾಗಮ &**this; }
			const_iterator &operator++() {
				bit_index += (**this).width;
				ಯದಿ (spec.rep_ == BITS)
					next_chunk_bits();
				ಪ್ರತ್ಯಾಗಮ *this;
			}
			bool operator==(ಧ್ರುವ const_iterator &rhs) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ bit_index == rhs.bit_index; }
			bool operator!=(ಧ್ರುವ const_iterator &rhs) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ !(*this == rhs); }
		};
		const_iterator begin() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ const_iterator(spec); }
		const_iterator end() ಧ್ರುವ {
			const_iterator it(spec, const_iterator::END);
			ಪ್ರತ್ಯಾಗಮ it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rbegin() {
			ensure_chunk_vector();
			ಪ್ರತ್ಯಾಗಮ chunk_vector.rbegin();
		}
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rend() {
			ensure_chunk_vector();
			ಪ್ರತ್ಯಾಗಮ chunk_vector.rend();
		}
		ಪೂರ್ಣಾಙ್ಕ size() {
			ensure_chunk_vector();
			ಪ್ರತ್ಯಾಗಮ chunk_vector.size();
		}
		ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ std::distance(begin(), end());
		}
		ಧ್ರುವ SigChunk &at(ಪೂರ್ಣಾಙ್ಕ index) {
			ensure_chunk_vector();
			ಪ್ರತ್ಯಾಗಮ chunk_vector.at(index);
		}
		operator ಧ್ರುವ std::vector<RTLIL::SigChunk>&() {
			ensure_chunk_vector();
			ಪ್ರತ್ಯಾಗಮ chunk_vector;
		}
	ಗುಪ್ತ:
		ಶೂನ್ಯ ensure_chunk_vector() {
			ಯದಿ (spec.size() > 0 && chunk_vector.empty()) {
				ಪ್ರತಿ (ಧ್ರುವ RTLIL::SigChunk &c : *this)
					chunk_vector.ತಳ್ಳು_back(c);
			}
		}
		ಧ್ರುವ SigSpec &spec;
		std::vector<RTLIL::SigChunk> chunk_vector;
	};
	friend ಸಂರಚನಾ Chunks::const_iterator;

	ಅನ್ತಃಸ್ಥ Chunks chunks() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ {*this}; }
	ಅನ್ತಃಸ್ಥ ಧ್ರುವ SigSpec &bits() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ *this; }

	ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ rep_ == CHUNK ? chunk_.width : GetSize(bits_); }
	ಅನ್ತಃಸ್ಥ bool empty() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ size() == 0; };

	ಅನ್ತಃಸ್ಥ RTLIL::SigBit &operator[](ಪೂರ್ಣಾಙ್ಕ index) { inline_unpack(); hash_.clear(); ಪ್ರತ್ಯಾಗಮ bits_.at(index); }
	ಅನ್ತಃಸ್ಥ RTLIL::SigBit operator[](ಪೂರ್ಣಾಙ್ಕ index) ಧ್ರುವ {
		ಯದಿ (rep_ == CHUNK) {
			ಯದಿ (index < 0 || index >= chunk_.width)
				throw std::out_of_range("SigSpec::operator[]");
			ಯದಿ (chunk_.wire)
				ಪ್ರತ್ಯಾಗಮ RTLIL::SigBit(chunk_.wire, chunk_.offset + index);
			ಪ್ರತ್ಯಾಗಮ RTLIL::SigBit(chunk_.data[index]);
		}
		ಪ್ರತ್ಯಾಗಮ bits_.at(index);
	}

	ಅನ್ತಃಸ್ಥ RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = 0; ಪ್ರತ್ಯಾಗಮ it; }
	ಅನ್ತಃಸ್ಥ RTLIL::SigSpecIterator end() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = size(); ಪ್ರತ್ಯಾಗಮ it; }

	ಅನ್ತಃಸ್ಥ RTLIL::SigSpecConstIterator begin() ಧ್ರುವ { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = 0; ಪ್ರತ್ಯಾಗಮ it; }
	ಅನ್ತಃಸ್ಥ RTLIL::SigSpecConstIterator end() ಧ್ರುವ { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = size(); ಪ್ರತ್ಯಾಗಮ it; }

	ಶೂನ್ಯ sort();
	ಶೂನ್ಯ sort_and_unify();

	ಶೂನ್ಯ replace(ಧ್ರುವ RTLIL::SigSpec &pattern, ಧ್ರುವ RTLIL::SigSpec &with);
	ಶೂನ್ಯ replace(ಧ್ರುವ RTLIL::SigSpec &pattern, ಧ್ರುವ RTLIL::SigSpec &with, RTLIL::SigSpec *other) ಧ್ರುವ;

	ಶೂನ್ಯ replace(ಧ್ರುವ dict<RTLIL::SigBit, RTLIL::SigBit> &rules);
	ಶೂನ್ಯ replace(ಧ್ರುವ dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ಧ್ರುವ;

	ಶೂನ್ಯ replace(ಧ್ರುವ std::map<RTLIL::SigBit, RTLIL::SigBit> &rules);
	ಶೂನ್ಯ replace(ಧ್ರುವ std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ಧ್ರುವ;

	ಶೂನ್ಯ replace(ಪೂರ್ಣಾಙ್ಕ offset, ಧ್ರುವ RTLIL::SigSpec &with);

	ಶೂನ್ಯ remove(ಧ್ರುವ RTLIL::SigSpec &pattern);
	ಶೂನ್ಯ remove(ಧ್ರುವ RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) ಧ್ರುವ;
	ಶೂನ್ಯ remove2(ಧ್ರುವ RTLIL::SigSpec &pattern, RTLIL::SigSpec *other);

	ಶೂನ್ಯ remove(ಧ್ರುವ pool<RTLIL::SigBit> &pattern);
	ಶೂನ್ಯ remove(ಧ್ರುವ pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) ಧ್ರುವ;
	ಶೂನ್ಯ remove2(ಧ್ರುವ pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	ಶೂನ್ಯ remove2(ಧ್ರುವ std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	ಶೂನ್ಯ remove2(ಧ್ರುವ pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other);

	ಶೂನ್ಯ remove(ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ length = 1);
	ಶೂನ್ಯ remove_const();

	RTLIL::SigSpec extract(ಧ್ರುವ RTLIL::SigSpec &pattern, ಧ್ರುವ RTLIL::SigSpec *other = ರಿಕ್ತ) ಧ್ರುವ;
	RTLIL::SigSpec extract(ಧ್ರುವ pool<RTLIL::SigBit> &pattern, ಧ್ರುವ RTLIL::SigSpec *other = ರಿಕ್ತ) ಧ್ರುವ;
	RTLIL::SigSpec extract(ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ length = 1) ಧ್ರುವ;
	RTLIL::SigSpec extract_end(ಪೂರ್ಣಾಙ್ಕ offset) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ extract(offset, size() - offset); }

	ಶೂನ್ಯ rewrite_wires(std::function<ಶೂನ್ಯ(RTLIL::Wire*& wire)> rewrite);

	RTLIL::SigBit lsb() ಧ್ರುವ { log_assert(size()); ಪ್ರತ್ಯಾಗಮ (*this)[0]; };
	RTLIL::SigBit msb() ಧ್ರುವ { log_assert(size()); ಪ್ರತ್ಯಾಗಮ (*this)[size() - 1]; };
	RTLIL::SigBit front() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (*this)[0]; }
	RTLIL::SigBit back() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (*this)[size() - 1]; }

	ಶೂನ್ಯ append(ಧ್ರುವ RTLIL::SigSpec &ಸಙ್ಕೇತ);
	ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ append(ಧ್ರುವ RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ append(ಧ್ರುವ RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	ಶೂನ್ಯ append(ಧ್ರುವ RTLIL::SigBit &bit);
	ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ append(RTLIL::State state) { append(RTLIL::SigBit(state)); }
	ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ append(bool bool_) { append(RTLIL::SigBit(bool_)); }

	ಶೂನ್ಯ extend_u0(ಪೂರ್ಣಾಙ್ಕ width, bool is_signed = false);

	RTLIL::SigSpec repeat(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ) ಧ್ರುವ;

	ಶೂನ್ಯ reverse() { inline_unpack(); std::reverse(bits_.begin(), bits_.end()); }

	bool operator <(ಧ್ರುವ RTLIL::SigSpec &other) ಧ್ರುವ;
	bool operator ==(ಧ್ರುವ RTLIL::SigSpec &other) ಧ್ರುವ;
	ಅನ್ತಃಸ್ಥ bool operator !=(ಧ್ರುವ RTLIL::SigSpec &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ !(*this == other); }

	bool is_wire() ಧ್ರುವ;
	bool is_chunk() ಧ್ರುವ;
	ಅನ್ತಃಸ್ಥ bool is_bit() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ size() == 1; }

	bool known_driver() ಧ್ರುವ;

	bool is_fully_const() ಧ್ರುವ;
	bool is_fully_zero() ಧ್ರುವ;
	bool is_fully_ones() ಧ್ರುವ;
	bool is_fully_def() ಧ್ರುವ;
	bool is_fully_undef() ಧ್ರುವ;
	bool has_const() ಧ್ರುವ;
	bool has_const(State state) ಧ್ರುವ;
	bool has_marked_bits() ಧ್ರುವ;
	bool is_onehot(ಪೂರ್ಣಾಙ್ಕ *pos = nullptr) ಧ್ರುವ;

	bool as_bool() ಧ್ರುವ;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	ಪೂರ್ಣಾಙ್ಕ as_int(bool is_signed = false) ಧ್ರುವ;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_to_int(bool is_signed = false) ಧ್ರುವ;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<ಪೂರ್ಣಾಙ್ಕ> try_as_int(bool is_signed = false) ಧ್ರುವ;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	ಪೂರ್ಣಾಙ್ಕ as_int_saturating(bool is_signed = false) ಧ್ರುವ;

	std::string as_string() ಧ್ರುವ;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() ಧ್ರುವ;
	RTLIL::Const as_const() ಧ್ರುವ;
	RTLIL::Wire *as_wire() ಧ್ರುವ;
	RTLIL::SigChunk as_chunk() ಧ್ರುವ;
	RTLIL::SigBit as_bit() ಧ್ರುವ;

	bool match(ಧ್ರುವ ವರ್ಣ* pattern) ಧ್ರುವ;

	std::set<RTLIL::SigBit> to_sigbit_set() ಧ್ರುವ;
	pool<RTLIL::SigBit> to_sigbit_pool() ಧ್ರುವ;
	std::vector<RTLIL::SigBit> to_sigbit_vector() ಧ್ರುವ;
	std::map<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_map(ಧ್ರುವ RTLIL::SigSpec &other) ಧ್ರುವ;
	dict<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_dict(ಧ್ರುವ RTLIL::SigSpec &other) ಧ್ರುವ;

	ಸ್ಥಿರ bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	ಸ್ಥಿರ bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	ಸ್ಥಿರ bool parse_rhs(ಧ್ರುವ RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	operator std::vector<RTLIL::SigChunk>() ಧ್ರುವ;
	operator std::vector<RTLIL::SigBit>() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ to_sigbit_vector(); }
	ಧ್ರುವ RTLIL::SigBit &at(ಪೂರ್ಣಾಙ್ಕ offset, ಧ್ರುವ RTLIL::SigBit &defval) { ಪ್ರತ್ಯಾಗಮ offset < size() ? (*this)[offset] : defval; }
	RTLIL::SigBit& at(ಪೂರ್ಣಾಙ್ಕ offset) { ಪ್ರತ್ಯಾಗಮ (*this)[offset]; }
	RTLIL::SigBit at(ಪೂರ್ಣಾಙ್ಕ offset) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (*this)[offset]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
		Hasher::hash_t val;
		ಯದಿ (std::optional<Hasher::hash_t> current = hash_.ಪಠ())
			val = *current;
		ಅನ್ಯಥಾ
			val = updhash();
		h.eat(val);
		ಪ್ರತ್ಯಾಗಮ h;
	}

#ifndef NDEBUG
	ಶೂನ್ಯ check(ಧ್ರುವ Module *mod = nullptr) ಧ್ರುವ;
#ಅನ್ಯಥಾ
	ಶೂನ್ಯ check(ಧ್ರುವ Module *mod = nullptr) ಧ್ರುವ { (ಶೂನ್ಯ)mod; }
#endif
};

ಸಂರಚನಾ RTLIL::Selection
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
	bool boxed_module(RTLIL::IdString mod_name) ಧ್ರುವ;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ಧ್ರುವ;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) ಧ್ರುವ;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ಧ್ರುವ;

	// optimizes this selection for the given design by:
	// - removing non-existent modules and members, any boxed modules and
	//   their members (if selection does not include boxes), and any
	//   partially selected modules with no selected members;
	// - marking partially selected modules as wholly selected if all
	//   members of that module are selected; and
	// - marking selection as a complete_selection if all modules in the
	//   given design are selected, or a full_selection if it does not
	//   include boxes.
	ಶೂನ್ಯ optimize(RTLIL::Design *design);

	// checks if selection covers full design (may or may not include
	// boxed-modules)
	bool selects_all() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ full_selection || complete_selection;
	}

	// add whole module to this selection
	template<typename T1> ಶೂನ್ಯ select(T1 *module) {
		ಯದಿ (!selects_all() && selected_modules.ಎಣಿಕೆ(module->name) == 0) {
			IdString name = module->name;
			selected_modules.ಸೇರಿಸು(name);
			selected_members.erase(name);
			ಯದಿ (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	template<typename T1, typename T2> ಶೂನ್ಯ select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	template<typename T1> ಶೂನ್ಯ select(T1 *module, RTLIL::IdString member) {
		ಯದಿ (!selects_all() && selected_modules.ಎಣಿಕೆ(module->name.ref()) == 0) {
			selected_members[module->name.ref()].ಸೇರಿಸು(member);
			ಯದಿ (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool empty() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ !selects_all() && selected_modules.empty() && selected_members.empty();
	}

	// clear this selection, leaving it empty
	ಶೂನ್ಯ clear();

	// create a new selection which is empty
	ಸ್ಥಿರ Selection EmptySelection(RTLIL::Design *design = nullptr) { ಪ್ರತ್ಯಾಗಮ Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	ಸ್ಥಿರ Selection FullSelection(RTLIL::Design *design = nullptr) { ಪ್ರತ್ಯಾಗಮ Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	ಸ್ಥಿರ Selection CompleteSelection(RTLIL::Design *design = nullptr) { ಪ್ರತ್ಯಾಗಮ Selection(true, true, design); };
};

ಸಂರಚನಾ RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ { h.eat(hashidx_); ಪ್ರತ್ಯಾಗಮ h; }

	Monitor() {
		ಸ್ಥಿರ unsigned ಪೂರ್ಣಾಙ್ಕ hashidx_ಎಣಿಕೆ = 123456789;
		hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
		hashidx_ = hashidx_ಎಣಿಕೆ;
	}

	ಆಭಾಸ ~Monitor() { }
	ಆಭಾಸ ಶೂನ್ಯ notify_module_add(RTLIL::Module*) { }
	ಆಭಾಸ ಶೂನ್ಯ notify_module_del(RTLIL::Module*) { }
	ಆಭಾಸ ಶೂನ್ಯ notify_connect(RTLIL::Cell*, RTLIL::IdString, ಧ್ರುವ RTLIL::SigSpec&, ಧ್ರುವ RTLIL::SigSpec&) { }
	ಆಭಾಸ ಶೂನ್ಯ notify_connect(RTLIL::Module*, ಧ್ರುವ RTLIL::SigSig&) { }
	ಆಭಾಸ ಶೂನ್ಯ notify_connect(RTLIL::Module*, ಧ್ರುವ std::vector<RTLIL::SigSig>&) { }
	ಆಭಾಸ ಶೂನ್ಯ notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
ಸಂರಚನಾ define_map_t;

template<typename N>
ಅನ್ತಃಸ್ಥ constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::Leaf> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ಧ್ರುವ ವರ್ಣ*> || std::is_same_v<std::decay_t<N>, ವರ್ಣ*>;

#define YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, ಪೂರ್ಣಾಙ್ಕ> = 0

template<typename N>
ಅನ್ತಃಸ್ಥ constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ಧ್ರುವ ವರ್ಣ *> || std::is_same_v<std::decay_t<N>, ವರ್ಣ *>;

#define YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, ಪೂರ್ಣಾಙ್ಕ> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#define YS_NAME_FWD_POOL(_func, _pool) \
	template<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(N name, Rest&&... rest) \
		{ ಪ್ರತ್ಯಾಗಮ _func(_pool.add(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#define YS_NAME_FWD_2ND_POOL(_func, _pool) \
	template<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(T &&first, N name) \
		{ ಪ್ರತ್ಯಾಗಮ _func(std::forward<T>(first), _pool.add(std::move(name))); }

#define YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#define YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

ಸಂರಚನಾ RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ { h.eat(hashidx_); ಪ್ರತ್ಯಾಗಮ h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	ಶೂನ್ಯ bufNormalize(bool enable=true);

	ಪೂರ್ಣಾಙ್ಕ refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

ಗುಪ್ತ:
	TwinePool twines_;

ಸಾರ್ವಜನಿಕ:
	TwinePool &twines() { ಪ್ರತ್ಯಾಗಮ twines_; }
	ಧ್ರುವ TwinePool &twines() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ twines_; }

	ಶೂನ್ಯ absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	size_t gc_twines();

	std::vector<std::unique_ptr<AST::Ast_ಗಂಟು>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_defines;

	std::vector<RTLIL::Selection> selection_ರಾಶಿ;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_active_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ಧ್ರುವ RTLIL::Module *module(RTLIL::IdString name) ಧ್ರುವ;
	template<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ ಪ್ರತ್ಯಾಗಮ module(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ಧ್ರುವ RTLIL::Module *module(N name) ಧ್ರುವ
		{ ಪ್ರತ್ಯಾಗಮ module(twines().find(std::string_view(name))); }
	RTLIL::Module *ತುದಿ_module() ಧ್ರುವ;

	bool has(RTLIL::IdString id) ಧ್ರುವ {
		twines_.check_owned(id);
		ಪ್ರತ್ಯಾಗಮ modules_.ಎಣಿಕೆ(id) != 0;
	}
	template<typename N, YS_NAME_STRING(N)> bool has(N name) ಧ್ರುವ
		{ ಪ್ರತ್ಯಾಗಮ has(twines().find(std::string_view(name))); }

	ಶೂನ್ಯ add(RTLIL::Module *module);

	RTLIL::Module *addModule(RTLIL::IdString name);
	YS_NAME_FWD_POOL(addModule, twines())
	ಶೂನ್ಯ remove(RTLIL::Module *module);
	ಶೂನ್ಯ rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	ಶೂನ್ಯ scratchpad_unset(ಧ್ರುವ std::string &varname);

	ಶೂನ್ಯ scratchpad_set_int(ಧ್ರುವ std::string &varname, ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ);
	ಶೂನ್ಯ scratchpad_set_bool(ಧ್ರುವ std::string &varname, bool ಮೌಲ್ಯ);
	ಶೂನ್ಯ scratchpad_set_string(ಧ್ರುವ std::string &varname, std::string ಮೌಲ್ಯ);

	ಪೂರ್ಣಾಙ್ಕ scratchpad_get_int(ಧ್ರುವ std::string &varname, ಪೂರ್ಣಾಙ್ಕ default_ಮೌಲ್ಯ_2 = 0) ಧ್ರುವ;
	bool scratchpad_get_bool(ಧ್ರುವ std::string &varname, bool default_ಮೌಲ್ಯ_2 = false) ಧ್ರುವ;
	std::string scratchpad_get_string(ಧ್ರುವ std::string &varname, ಧ್ರುವ std::string &default_ಮೌಲ್ಯ_2 = std::string()) ಧ್ರುವ;

	ಶೂನ್ಯ sort();
	ಶೂನ್ಯ sort_modules();
	ಶೂನ್ಯ check();
	ಶೂನ್ಯ optimize();

	ಶೂನ್ಯ clone_into(RTLIL::Design *dst) ಧ್ರುವ;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ಧ್ರುವ;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) ಧ್ರುವ;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ಧ್ರುವ;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ಧ್ರುವ;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::Module *mod) ಧ್ರುವ;

	// push the given selection to the selection stack
	ಶೂನ್ಯ ತಳ್ಳು_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	ಶೂನ್ಯ ತಳ್ಳು_empty_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	ಶೂನ್ಯ ತಳ್ಳು_full_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	ಶೂನ್ಯ ತಳ್ಳು_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	ಶೂನ್ಯ ತೆಗೆ_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		ಪ್ರತ್ಯಾಗಮ selection_ರಾಶಿ.back();
	}

	// get the current selection
	ಧ್ರುವ RTLIL::Selection &selection() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ selection_ರಾಶಿ.back();
	}

	// is the current selection a full selection (no boxes)
	bool full_selection() ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ selection().full_selection;
	}

	// is the given module in the current selection
	template<typename T1> bool selected(T1 *module) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	template<typename T1, typename T2> bool selected(T1 *module, T2 *member) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	template<typename T1> ಶೂನ್ಯ select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	template<typename T1, typename T2> ಶೂನ್ಯ select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::vector<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) ಧ್ರುವ;

	// returns all selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_modules() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::vector<RTLIL::Module*> selected_unboxed_modules() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_modules_warn() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::vector<RTLIL::Module*> selected_whole_modules() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_whole_modules() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ selected_modules(SELECT_WHOLE_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::vector<RTLIL::Module*> selected_whole_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ selected_modules(SELECT_WHOLE_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules_warn() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ selected_modules(SELECT_WHOLE_WARN, SB_UNBOXED_WARN); }

	ಸ್ಥಿರ std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Design*> *get_all_designs(ಶೂನ್ಯ);

	std::string to_rtlil_str(bool only_selected = true) ಧ್ರುವ;
};

ಸಂರಚನಾ RTLIL::Module : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
	friend ಸಂರಚನಾ RTLIL::Cell;
	friend ಸಂರಚನಾ RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ { h.eat(hashidx_); ಪ್ರತ್ಯಾಗಮ h; }

ರಕ್ಷಿತ:
	ಶೂನ್ಯ add(RTLIL::Wire *wire);
	ಶೂನ್ಯ add(RTLIL::Cell *cell);
	ಶೂನ್ಯ add(RTLIL::Process *process);

ಸಾರ್ವಜನಿಕ:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ design->twines(); }

	ಪೂರ್ಣಾಙ್ಕ refcount_wires_;
	ಪೂರ್ಣಾಙ್ಕ refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::vector<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_ಮೌಲ್ಯಗಳು;
	dict<RTLIL::IdString, RTLIL::Memory*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	ಆಭಾಸ ~Module();
	ಆಭಾಸ RTLIL::IdString derive(RTLIL::Design *design, ಧ್ರುವ dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	ಆಭಾಸ RTLIL::IdString derive(RTLIL::Design *design, ಧ್ರುವ dict<RTLIL::IdString, RTLIL::Const> &parameters, ಧ್ರುವ dict<RTLIL::IdString, RTLIL::Module*> &interfaces, ಧ್ರುವ dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	ಆಭಾಸ size_t ಎಣಿಕೆ_id(RTLIL::IdString id);
	ಆಭಾಸ ಶೂನ್ಯ expand_interfaces(RTLIL::Design *design, ಧ್ರುವ dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	ಆಭಾಸ bool reprocess_if_necessary(RTLIL::Design *design);

	ಆಭಾಸ ಶೂನ್ಯ sort();
	ಆಭಾಸ ಶೂನ್ಯ check();
	ಆಭಾಸ ಶೂನ್ಯ optimize();
	ಆಭಾಸ ಶೂನ್ಯ makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	ಶೂನ್ಯ connect(ಧ್ರುವ RTLIL::SigSig &conn);
	ಶೂನ್ಯ connect(ಧ್ರುವ RTLIL::SigSpec &lhs, ಧ್ರುವ RTLIL::SigSpec &rhs);
	ಶೂನ್ಯ new_connections(ಧ್ರುವ std::vector<RTLIL::SigSig> &new_conn);
	ಧ್ರುವ std::vector<RTLIL::SigSig> &connections() ಧ್ರುವ;

	std::vector<RTLIL::IdString> ports;
	ಶೂನ್ಯ fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_queue;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_port_queue;
	pool<RTLIL::Wire *> buf_norm_wire_queue;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_index;
	ಶೂನ್ಯ bufNormalize();

	template<typename T> ಶೂನ್ಯ rewrite_sigspecs(T &functor);
	template<typename T> ಶೂನ್ಯ rewrite_sigspecs2(T &functor);
	ಶೂನ್ಯ cloneInto(RTLIL::Module *new_mod) ಧ್ರುವ;
	ಆಭಾಸ RTLIL::Module *clone() ಧ್ರುವ;
	ಆಭಾಸ RTLIL::Module *clone(RTLIL::Design *dst) ಧ್ರುವ;
	ಆಭಾಸ RTLIL::Module *clone(RTLIL::Design *dst, IdString target_name) ಧ್ರುವ;

	bool has_memories() ಧ್ರುವ;
	bool has_processes() ಧ್ರುವ;

	bool has_memories_warn() ಧ್ರುವ;
	bool has_processes_warn() ಧ್ರುವ;

	bool is_selected() ಧ್ರುವ;
	bool is_selected_whole() ಧ್ರುವ;

	std::vector<RTLIL::Wire*> selected_wires() ಧ್ರುವ;
	std::vector<RTLIL::Cell*> selected_cells() ಧ್ರುವ;
	std::vector<RTLIL::Memory*> selected_memories() ಧ್ರುವ;
	std::vector<RTLIL::Process*> selected_processes() ಧ್ರುವ;
	std::vector<RTLIL::SelectedMember> selected_members() ಧ್ರುವ;

	template<typename T> bool selected(T *member) ಧ್ರುವ {
		ಪ್ರತ್ಯಾಗಮ design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		ಯದಿ (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		ಪ್ರತ್ಯಾಗಮ it == wires_.end() ? nullptr : it->second;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		ಯದಿ (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		ಪ್ರತ್ಯಾಗಮ it == cells_.end() ? nullptr : it->second;
	}

	ಧ್ರುವ RTLIL::Wire* wire(RTLIL::IdString id) ಧ್ರುವ{
		ಯದಿ (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		ಪ್ರತ್ಯಾಗಮ it == wires_.end() ? nullptr : it->second;
	}
	ಧ್ರುವ RTLIL::Cell* cell(RTLIL::IdString id) ಧ್ರುವ {
		ಯದಿ (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		ಪ್ರತ್ಯಾಗಮ it == cells_.end() ? nullptr : it->second;
	}

	template<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ ಪ್ರತ್ಯಾಗಮ wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ ಪ್ರತ್ಯಾಗಮ cell(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ಧ್ರುವ RTLIL::Wire *wire(N name) ಧ್ರುವ
		{ ಪ್ರತ್ಯಾಗಮ wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ಧ್ರುವ RTLIL::Cell *cell(N name) ಧ್ರುವ
		{ ಪ್ರತ್ಯಾಗಮ cell(twines().find(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { ಪ್ರತ್ಯಾಗಮ RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	ಪೂರ್ಣಾಙ್ಕ wires_size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ wires_.size(); }
	RTLIL::Wire* wire_at(ಪೂರ್ಣಾಙ್ಕ index) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ wires_.element(index)->second; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { ಪ್ರತ್ಯಾಗಮ RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	ಪೂರ್ಣಾಙ್ಕ cells_size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ cells_.size(); }
	RTLIL::Cell* cell_at(ಪೂರ್ಣಾಙ್ಕ index) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ cells_.element(index)->second; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	ಶೂನ್ಯ remove(ಧ್ರುವ pool<RTLIL::Wire*> &wires);
	ಶೂನ್ಯ remove(RTLIL::Cell *cell);
	ಶೂನ್ಯ remove(RTLIL::Memory *memory);
	ಶೂನ್ಯ remove(RTLIL::Process *process);

	ಶೂನ್ಯ rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	ಶೂನ್ಯ rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	ಶೂನ್ಯ rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	ಶೂನ್ಯ swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2);
	ಶೂನ್ಯ swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, ಪೂರ್ಣಾಙ್ಕ &index);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *addWire(RTLIL::IdString name, ಪೂರ್ಣಾಙ್ಕ width = 1);
	RTLIL::Wire *addWire(RTLIL::IdString name, ಧ್ರುವ RTLIL::Wire *other);
	YS_NAME_FWD(addWire)

	RTLIL::Cell *addCell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *addCell(RTLIL::IdString name, ಧ್ರುವ RTLIL::Cell *other);
	YS_NAME_FWD(addCell)
	template<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *addCell(RTLIL::IdString name, T type) { ಪ್ರತ್ಯಾಗಮ addCell(name, design->twines().add(std::move(type))); }

	RTLIL::Memory *addMemory(RTLIL::IdString name);
	YS_NAME_FWD(addMemory)
	RTLIL::Memory *addMemory(RTLIL::IdString name, ಧ್ರುವ RTLIL::Memory *other);

	RTLIL::Process *addProcess(RTLIL::IdString name);
	YS_NAME_FWD(addProcess)
	RTLIL::Process *addProcess(RTLIL::IdString name, ಧ್ರುವ RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* addNot     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addPos     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addBuf     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addBarrier (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addNeg     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addAnd  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addOr   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addXor  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addXnor (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addReduceAnd  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addReduceOr   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addReduceXor  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addReduceXnor (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addReduceBool (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addShl    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addShr    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addSshl   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addSshr   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addShift  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addShiftx (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addLt  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addLe  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addEq  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addNe  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addEqx (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addNex (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addGe  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addGt  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addAdd (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addSub (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addMul (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	// truncating division
	RTLIL::Cell* addDiv (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	// truncating modulo
	RTLIL::Cell* addMod (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDivFloor (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addModFloor (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addPow (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addFa (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_c, ಧ್ರುವ RTLIL::SigSpec &sig_x, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addLogicNot (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addLogicAnd (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addLogicOr  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, bool is_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addMux  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addPmux (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addBmux (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDemux (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addBweqx  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addBwmux  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addSlice  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, RTLIL::Const offset, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addConcat (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addLut    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_y, RTLIL::Const lut, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addTribuf (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAssert (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAssume (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addLive   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addFair   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addCover  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addEquiv  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addSr    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr, ಧ್ರುವ RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addFf    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDff   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_d,   ಧ್ರುವ RTLIL::SigSpec &sig_q, bool clk_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDffe  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en,  ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDffsr (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDffsre (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAdff (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_arst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, RTLIL::Const arst_ಮೌಲ್ಯ, bool clk_polarity = true, bool arst_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAdffe (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_arst,  ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, RTLIL::Const arst_ಮೌಲ್ಯ, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAldff (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_aload, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ಧ್ರುವ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAldffe (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_aload,  ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ಧ್ರುವ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addSdff (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_srst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, RTLIL::Const srst_ಮೌಲ್ಯ, bool clk_polarity = true, bool srst_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addSdffe (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_srst,  ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, RTLIL::Const srst_ಮೌಲ್ಯ, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addSdffce (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_srst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, RTLIL::Const srst_ಮೌಲ್ಯ, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDlatch (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, bool en_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAdlatch (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_arst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, RTLIL::Const arst_ಮೌಲ್ಯ, bool en_polarity = true, bool arst_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDlatchsr (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addBufGate    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addNotGate    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAndGate    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addNandGate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addOrGate     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addNorGate    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addXorGate    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addXnorGate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAndnotGate (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addOrnotGate  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addMuxGate    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_s, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addNmuxGate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_s, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAoi3Gate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_c, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addOai3Gate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_c, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAoi4Gate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_c, ಧ್ರುವ RTLIL::SigBit &sig_d, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addOai4Gate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_c, ಧ್ರುವ RTLIL::SigBit &sig_d, ಧ್ರುವ RTLIL::SigBit &sig_y, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addSrGate     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr,
			ಧ್ರುವ RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addFfGate     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDffGate    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, bool clk_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDffeGate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDffsrGate  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDffsreGate (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAdffGate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_arst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
			bool arst_ಮೌಲ್ಯ = false, bool clk_polarity = true, bool arst_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAdffeGate  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_arst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
			bool arst_ಮೌಲ್ಯ = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAldffGate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_aload, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
			ಧ್ರುವ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAldffeGate  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_aload, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
			ಧ್ರುವ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addSdffGate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_srst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
			bool srst_ಮೌಲ್ಯ = false, bool clk_polarity = true, bool srst_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addSdffeGate  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_srst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
			bool srst_ಮೌಲ್ಯ = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addSdffceGate (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_clk, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_srst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
			bool srst_ಮೌಲ್ಯ = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDlatchGate (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, bool en_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addAdlatchGate(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_arst, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q,
			bool arst_ಮೌಲ್ಯ = false, bool en_polarity = true, bool arst_polarity = true, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell* addDlatchsrGate  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_en, ಧ್ರುವ RTLIL::SigSpec &sig_set, ಧ್ರುವ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ಧ್ರುವ std::string &src = "");

	RTLIL::Cell* addAnyinit(RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_d, ಧ್ರುವ RTLIL::SigSpec &sig_q, ಧ್ರುವ std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, bool is_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, bool is_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Shift  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::SigSpec Add (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, bool is_signed = false, ಧ್ರುವ std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_b, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ std::string &src = "");

	RTLIL::SigBit BufGate    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit NotGate    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit AndGate    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit NandGate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit OrGate     (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit NorGate    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit XorGate    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit XnorGate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit AndnotGate (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit OrnotGate  (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit MuxGate    (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_s, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit NmuxGate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_s, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit Aoi3Gate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_c, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit Oai3Gate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_c, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit Aoi4Gate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_c, ಧ್ರುವ RTLIL::SigBit &sig_d, ಧ್ರುವ std::string &src = "");
	RTLIL::SigBit Oai4Gate   (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigBit &sig_a, ಧ್ರುವ RTLIL::SigBit &sig_b, ಧ್ರುವ RTLIL::SigBit &sig_c, ಧ್ರುವ RTLIL::SigBit &sig_d, ಧ್ರುವ std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, ಪೂರ್ಣಾಙ್ಕ width = 1, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, ಪೂರ್ಣಾಙ್ಕ width = 1, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, ಪೂರ್ಣಾಙ್ಕ width = 1, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, ಪೂರ್ಣಾಙ್ಕ width = 1, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ಧ್ರುವ std::string &src = "");

	RTLIL::SigSpec SetTag          (RTLIL::IdString name, ಧ್ರುವ std::string &tag, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ RTLIL::SigSpec &sig_c, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell*   addSetTag       (RTLIL::IdString name, ಧ್ರುವ std::string &tag, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ RTLIL::SigSpec &sig_c, ಧ್ರುವ RTLIL::SigSpec &sig_y, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec GetTag          (RTLIL::IdString name, ಧ್ರುವ std::string &tag, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ std::string &src = "");
	RTLIL::Cell*   addOverwriteTag (RTLIL::IdString name, ಧ್ರುವ std::string &tag, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ RTLIL::SigSpec &sig_s, ಧ್ರುವ RTLIL::SigSpec &sig_c, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec OriginalTag     (RTLIL::IdString name, ಧ್ರುವ std::string &tag, ಧ್ರುವ RTLIL::SigSpec &sig_a, ಧ್ರುವ std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ಧ್ರುವ RTLIL::SigSpec &sig_e, ಧ್ರುವ std::string &src = "");
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

	std::string to_rtlil_str() ಧ್ರುವ;
#ifdef YOSYS_ENABLE_PYTHON
	ಸ್ಥಿರ std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Module*> *get_all_modules(ಶೂನ್ಯ);
#endif
};

ಸಂರಚನಾ RTLIL::Wire : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ { h.eat(hashidx_); ಪ್ರತ್ಯಾಗಮ h; }

ರಕ್ಷಿತ:
	// use module->addWire() and module->remove() to create or destroy wires
	friend ಸಂರಚನಾ RTLIL::Module;
	Wire();
	~Wire();

	friend ಸಂರಚನಾ RTLIL::Design;
	friend ಸಂರಚನಾ RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driverPort_;

ಸಾರ್ವಜನಿಕ:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	ಶೂನ್ಯ operator=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	ಪೂರ್ಣಾಙ್ಕ width, start_offset, port_id;
	bool port_ಒಳಹರಿವು, port_output, upto, is_signed;

	RTLIL::Design *design() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ module ? module->design : nullptr; }
	TwinePool &twines() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ module->twines(); }

	bool known_driver() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ಧ್ರುವ    { log_assert(driverCell_); ಪ್ರತ್ಯಾಗಮ driverCell_; };
	RTLIL::IdString driverPort() ಧ್ರುವ { log_assert(driverCell_); ಪ್ರತ್ಯಾಗಮ driverPort_; };

	ಪೂರ್ಣಾಙ್ಕ from_hdl_index(ಪೂರ್ಣಾಙ್ಕ hdl_index) {
		ಪೂರ್ಣಾಙ್ಕ zero_index = hdl_index - start_offset;
		ಪೂರ್ಣಾಙ್ಕ rtlil_index = upto ? width - 1 - zero_index : zero_index;
		ಪ್ರತ್ಯಾಗಮ rtlil_index >= 0 && rtlil_index < width ? rtlil_index : INT_MIN;
	}

	ಪೂರ್ಣಾಙ್ಕ to_hdl_index(ಪೂರ್ಣಾಙ್ಕ rtlil_index) {
		ಯದಿ (rtlil_index < 0 || rtlil_index >= width)
			ಪ್ರತ್ಯಾಗಮ INT_MIN;
		ಪೂರ್ಣಾಙ್ಕ zero_index = upto ? width - 1 - rtlil_index : rtlil_index;
		ಪ್ರತ್ಯಾಗಮ zero_index + start_offset;
	}

	std::string to_rtlil_str() ಧ್ರುವ;

#ifdef YOSYS_ENABLE_PYTHON
	ಸ್ಥಿರ std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Wire*> *get_all_wires(ಶೂನ್ಯ);
#endif
};

ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ GetSize(RTLIL::Wire *wire) {
	ಪ್ರತ್ಯಾಗಮ wire->width;
}

ಸಂರಚನಾ RTLIL::Memory : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ { h.eat(hashidx_); ಪ್ರತ್ಯಾಗಮ h; }

	Memory();
	~Memory();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ module ? module->design : nullptr; }
	TwinePool &twines() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ module->twines(); }

	RTLIL::MemoryNameId name;

	ಪೂರ್ಣಾಙ್ಕ width, start_offset, size;
#ifdef YOSYS_ENABLE_PYTHON
	ಸ್ಥಿರ std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Memory*> *get_all_memorys(ಶೂನ್ಯ);
#endif

	std::string to_rtlil_str() ಧ್ರುವ;
};

ಸಂರಚನಾ RTLIL::Cell : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
ಗುಪ್ತ:
	ಶೂನ್ಯ initIndex();

	bool bufnorm_handle_setPort(IdString portname, RTLIL::SigSpec &ಸಙ್ಕೇತ, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
ಸಾರ್ವಜನಿಕ:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ { h.eat(hashidx_); ಪ್ರತ್ಯಾಗಮ h; }

ರಕ್ಷಿತ:
	// use module->addCell() and module->remove() to create or destroy cells
	friend ಸಂರಚನಾ RTLIL::Module;
	Cell();
	~Cell();

ಸಾರ್ವಜನಿಕ:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	ಶೂನ್ಯ operator=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ module ? module->design : nullptr; }
	TwinePool &twines() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool hasPort(RTLIL::IdString portname) ಧ್ರುವ;
	ಶೂನ್ಯ unsetPort(RTLIL::IdString portname);
	ಶೂನ್ಯ setPort(RTLIL::IdString portname, RTLIL::SigSpec ಸಙ್ಕೇತ);
	template<typename N, YS_NAME_STRING(N)> ಶೂನ್ಯ setPort(N portname, RTLIL::SigSpec ಸಙ್ಕೇತ)
		{ setPort(module->twines().add(std::move(portname)), std::move(ಸಙ್ಕೇತ)); }
	ಧ್ರುವ RTLIL::SigSpec &getPort(RTLIL::IdString portname) ಧ್ರುವ;
	ಧ್ರುವ dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ಧ್ರುವ;

	bool known() ಧ್ರುವ;
	bool ಒಳಹರಿವು(RTLIL::IdString portname) ಧ್ರುವ;
	bool output(RTLIL::IdString portname) ಧ್ರುವ;
	PortDir port_dir(RTLIL::IdString portname) ಧ್ರುವ;

	bool hasParam(RTLIL::IdString paramname) ಧ್ರುವ;
	ಶೂನ್ಯ unsetParam(RTLIL::IdString paramname);
	ಶೂನ್ಯ setParam(RTLIL::IdString paramname, RTLIL::Const ಮೌಲ್ಯ);
	ಧ್ರುವ RTLIL::Const &getParam(RTLIL::IdString paramname) ಧ್ರುವ;

	template<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ಧ್ರುವ
		{ ಪ್ರತ್ಯಾಗಮ hasParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ಶೂನ್ಯ unsetParam(N name)
		{ unsetParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ಶೂನ್ಯ setParam(N name, RTLIL::Const ಮೌಲ್ಯ)
		{ setParam(module->twines().add(std::move(name)), std::move(ಮೌಲ್ಯ)); }
	template<typename N, YS_NAME_STRING(N)> ಧ್ರುವ RTLIL::Const &getParam(N name) ಧ್ರುವ
		{ ಪ್ರತ್ಯಾಗಮ getParam(module->twines().find(std::string_view(name))); }

	ಶೂನ್ಯ sort();
	ಶೂನ್ಯ check();
	ಶೂನ್ಯ fixup_parameters(bool set_a_signed = false, bool set_b_signed = false);

	bool has_keep_attr() ಧ್ರುವ;

	template<typename T> ಶೂನ್ಯ rewrite_sigspecs(T &functor);
	template<typename T> ಶೂನ್ಯ rewrite_sigspecs2(T &functor);

#ifdef YOSYS_ENABLE_PYTHON
	ಸ್ಥಿರ std::map<unsigned ಪೂರ್ಣಾಙ್ಕ, RTLIL::Cell*> *get_all_cells(ಶೂನ್ಯ);
#endif

	bool has_memid() ಧ್ರುವ;
	bool is_mem_cell() ಧ್ರುವ;
	bool is_builtin_ff() ಧ್ರುವ;

	std::string to_rtlil_str() ಧ್ರುವ;
};

ಸಂರಚನಾ RTLIL::CaseRule : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
	std::vector<RTLIL::SigSpec> compare;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::SwitchRule*> switches;

	~CaseRule();

	bool empty() ಧ್ರುವ;

	template<typename T> ಶೂನ್ಯ rewrite_sigspecs(T &functor);
	template<typename T> ಶೂನ್ಯ rewrite_sigspecs2(T &functor);
	RTLIL::CaseRule *clone() ಧ್ರುವ;
};

ಸಂರಚನಾ RTLIL::SwitchRule : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
	RTLIL::SigSpec ಸಙ್ಕೇತ;
	std::vector<RTLIL::CaseRule*> cases;

	~SwitchRule();

	bool empty() ಧ್ರುವ;

	template<typename T> ಶೂನ್ಯ rewrite_sigspecs(T &functor);
	template<typename T> ಶೂನ್ಯ rewrite_sigspecs2(T &functor);
	RTLIL::SwitchRule *clone() ಧ್ರುವ;
};

ಸಂರಚನಾ RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec address;
	RTLIL::SigSpec data;
	RTLIL::SigSpec enable;
	RTLIL::Const priority_mask;
};

ಸಂರಚನಾ RTLIL::SyncRule
{
	RTLIL::SyncType type;
	RTLIL::SigSpec ಸಙ್ಕೇತ;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::MemWriteAction> mem_write_actions;

	template<typename T> ಶೂನ್ಯ rewrite_sigspecs(T &functor);
	template<typename T> ಶೂನ್ಯ rewrite_sigspecs2(T &functor);
	RTLIL::SyncRule *clone() ಧ್ರುವ;
};

ಸಂರಚನಾ RTLIL::Process : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ { h.eat(hashidx_); ಪ್ರತ್ಯಾಗಮ h; }

ರಕ್ಷಿತ:
	// use module->addProcess() and module->remove() to create or destroy processes
	friend ಸಂರಚನಾ RTLIL::Module;
	Process();
	~Process();

ಸಾರ್ವಜನಿಕ:
	RTLIL::Module *module;
	RTLIL::CaseRule ಬೇರು_case;
	std::vector<RTLIL::SyncRule*> syncs;

	RTLIL::Design *design() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ module ? module->design : nullptr; }
	TwinePool &twines() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ module->twines(); }

	template<typename T> ಶೂನ್ಯ rewrite_sigspecs(T &functor);
	template<typename T> ಶೂನ್ಯ rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ಧ್ರುವ;

	std::string to_rtlil_str() ಧ್ರುವ;
};

ಅನ್ತಃಸ್ಥ RTLIL::SigBit::SigBit() : wire(ರಿಕ್ತ), data(RTLIL::State::S0) { }
ಅನ್ತಃಸ್ಥ RTLIL::SigBit::SigBit(RTLIL::State bit) : wire(ರಿಕ್ತ), data(bit) { }
ಅನ್ತಃಸ್ಥ RTLIL::SigBit::SigBit(bool bit) : wire(ರಿಕ್ತ), data(bit ? State::S1 : State::S0) { }
ಅನ್ತಃಸ್ಥ RTLIL::SigBit::SigBit(RTLIL::Wire *wire) : wire(wire), offset(0) { log_assert(wire && wire->width == 1); }
ಅನ್ತಃಸ್ಥ RTLIL::SigBit::SigBit(RTLIL::Wire *wire, ಪೂರ್ಣಾಙ್ಕ offset) : wire(wire), offset(offset) { log_assert(wire != nullptr); }
ಅನ್ತಃಸ್ಥ RTLIL::SigBit::SigBit(ಧ್ರುವ RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.width == 1); ಯದಿ (wire) offset = chunk.offset; ಅನ್ಯಥಾ data = chunk.data[0]; }
ಅನ್ತಃಸ್ಥ RTLIL::SigBit::SigBit(ಧ್ರುವ RTLIL::SigChunk &chunk, ಪೂರ್ಣಾಙ್ಕ index) : wire(chunk.wire) { ಯದಿ (wire) offset = chunk.offset + index; ಅನ್ಯಥಾ data = chunk.data[index]; }

ಅನ್ತಃಸ್ಥ bool RTLIL::SigBit::operator<(ಧ್ರುವ RTLIL::SigBit &other) ಧ್ರುವ {
	ಯದಿ (wire == other.wire)
		ಪ್ರತ್ಯಾಗಮ wire ? (offset < other.offset) : (data < other.data);
	ಯದಿ (wire != nullptr && other.wire != nullptr)
		ಪ್ರತ್ಯಾಗಮ wire->name < other.wire->name;
	ಪ್ರತ್ಯಾಗಮ (wire != nullptr) < (other.wire != nullptr);
}

ಅನ್ತಃಸ್ಥ bool RTLIL::SigBit::operator==(ಧ್ರುವ RTLIL::SigBit &other) ಧ್ರುವ {
	ಪ್ರತ್ಯಾಗಮ (wire == other.wire) && (wire ? (offset == other.offset) : (data == other.data));
}

ಅನ್ತಃಸ್ಥ bool RTLIL::SigBit::operator!=(ಧ್ರುವ RTLIL::SigBit &other) ಧ್ರುವ {
	ಪ್ರತ್ಯಾಗಮ (wire != other.wire) || (wire ? (offset != other.offset) : (data != other.data));
}

ಅನ್ತಃಸ್ಥ Hasher RTLIL::SigBit::hash_into(Hasher h) ಧ್ರುವ {
	ಯದಿ (wire) {
		h.eat(offset);
		h.eat(wire->name);
		ಪ್ರತ್ಯಾಗಮ h;
	}
	h.eat(data);
	ಪ್ರತ್ಯಾಗಮ h;
}


ಅನ್ತಃಸ್ಥ Hasher RTLIL::SigBit::hash_ತುದಿ() ಧ್ರುವ {
	Hasher h;
	ಯದಿ (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.bits();
		h.force(hashlib::legacy::djb2_add(n, offset));
		ಪ್ರತ್ಯಾಗಮ h;
	}
	h.force(data);
	ಪ್ರತ್ಯಾಗಮ h;
}

ಅನ್ತಃಸ್ಥ RTLIL::SigBit &RTLIL::SigSpecIterator::operator*() ಧ್ರುವ {
	ಪ್ರತ್ಯಾಗಮ (*sig_p)[index];
}

ಅನ್ತಃಸ್ಥ ಧ್ರುವ RTLIL::SigBit &RTLIL::SigSpecConstIterator::operator*() {
	bit = (*sig_p)[index];
	ಪ್ರತ್ಯಾಗಮ bit;
}

ಅನ್ತಃಸ್ಥ RTLIL::SigBit::SigBit(ಧ್ರುವ RTLIL::SigSpec &sig) {
	log_assert(sig.size() == 1);
	auto it = sig.chunks().begin();
	*this = SigBit(*it);
}

template<typename T>
ಶೂನ್ಯ RTLIL::Module::rewrite_sigspecs(T &functor)
{
	ಪ್ರತಿ (auto &it : cells_)
		it.second->rewrite_sigspecs(functor);
	ಪ್ರತಿ (auto &it : processes)
		it.second->rewrite_sigspecs(functor);
	ಪ್ರತಿ (auto &it : connections_) {
		functor(it.first);
		functor(it.second);
	}
}

template<typename T>
ಶೂನ್ಯ RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	ಪ್ರತಿ (auto &it : cells_)
		it.second->rewrite_sigspecs2(functor);
	ಪ್ರತಿ (auto &it : processes)
		it.second->rewrite_sigspecs2(functor);
	ಪ್ರತಿ (auto &it : connections_) {
		functor(it.first, it.second);
	}
}

template<typename T>
ಶೂನ್ಯ RTLIL::Cell::rewrite_sigspecs(T &functor) {
	ಪ್ರತಿ (auto &it : connections_)
		functor(it.second);
}

template<typename T>
ಶೂನ್ಯ RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	ಪ್ರತಿ (auto &it : connections_)
		functor(it.second);
}

template<typename T>
ಶೂನ್ಯ RTLIL::CaseRule::rewrite_sigspecs(T &functor) {
	ಪ್ರತಿ (auto &it : compare)
		functor(it);
	ಪ್ರತಿ (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	ಪ್ರತಿ (auto it : switches)
		it->rewrite_sigspecs(functor);
}

template<typename T>
ಶೂನ್ಯ RTLIL::CaseRule::rewrite_sigspecs2(T &functor) {
	ಪ್ರತಿ (auto &it : compare)
		functor(it);
	ಪ್ರತಿ (auto &it : actions) {
		functor(it.first, it.second);
	}
	ಪ್ರತಿ (auto it : switches)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
ಶೂನ್ಯ RTLIL::SwitchRule::rewrite_sigspecs(T &functor)
{
	functor(ಸಙ್ಕೇತ);
	ಪ್ರತಿ (auto it : cases)
		it->rewrite_sigspecs(functor);
}

template<typename T>
ಶೂನ್ಯ RTLIL::SwitchRule::rewrite_sigspecs2(T &functor)
{
	functor(ಸಙ್ಕೇತ);
	ಪ್ರತಿ (auto it : cases)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
ಶೂನ್ಯ RTLIL::SyncRule::rewrite_sigspecs(T &functor)
{
	functor(ಸಙ್ಕೇತ);
	ಪ್ರತಿ (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	ಪ್ರತಿ (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
ಶೂನ್ಯ RTLIL::SyncRule::rewrite_sigspecs2(T &functor)
{
	functor(ಸಙ್ಕೇತ);
	ಪ್ರತಿ (auto &it : actions) {
		functor(it.first, it.second);
	}
	ಪ್ರತಿ (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
ಶೂನ್ಯ RTLIL::Process::rewrite_sigspecs(T &functor)
{
	ಬೇರು_case.rewrite_sigspecs(functor);
	ಪ್ರತಿ (auto it : syncs)
		it->rewrite_sigspecs(functor);
}

template<typename T>
ಶೂನ್ಯ RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	ಬೇರು_case.rewrite_sigspecs2(functor);
	ಪ್ರತಿ (auto it : syncs)
		it->rewrite_sigspecs2(functor);
}

#ಅನ್ತರ್ಭಾವ "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_END

#endif
