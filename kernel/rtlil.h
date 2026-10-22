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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys_common.h"
#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "kernel/twine.h"

#ଅନ୍ତର୍ଭାଵ <deque>
#ଅନ୍ତର୍ଭାଵ <string_view>
#ଅନ୍ତର୍ଭାଵ <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	ଗଣନା State : unsigned ଅକ୍ଷର {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	ଗଣନା SyncType : unsigned ଅକ୍ଷର {
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
	ଗଣନା ConstFlags : unsigned ଅକ୍ଷର {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	ଗଣନା SelectPartials : unsigned ଅକ୍ଷର {
		SELECT_ALL = 0,          // include partial modules
		SELECT_WHOLE_ONLY = 1,   // ignore partial modules
		SELECT_WHOLE_WARN = 2,   // call log_warning on partial module
		SELECT_WHOLE_ERR = 3,    // call log_error on partial module
		SELECT_WHOLE_CMDERR = 4  // call log_cmd_error on partial module
	};

	ଗଣନା SelectBoxes : unsigned ଅକ୍ଷର {
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

	ଗଣନା PortDir : unsigned ଅକ୍ଷର  {
		PD_UNKNOWN = 0,
		PD_ନିବେଶ = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା WIDTH_LIMIT = 1 << 30;

	ସଂରଚନା Const;
	ସଂରଚନା AttrObject;
	ସଂରଚନା SelectedMember;
	ସଂରଚନା Selection;
	ସଂରଚନା Monitor;
	ସଂରଚନା Design;
	ସଂରଚନା Module;
	ସଂରଚନା Wire;
	ସଂରଚନା Memory;
	ସଂରଚନା Cell;
	ସଂରଚନା SigChunk;
	ସଂରଚନା SigBit;
	ସଂରଚନା SigSpecIterator;
	ସଂରଚନା SigSpecConstIterator;
	ସଂରଚନା SigSpec;
	ସଂରଚନା CaseRule;
	ସଂରଚନା SwitchRule;
	ସଂରଚନା MemWriteAction;
	ସଂରଚନା SyncRule;
	ସଂରଚନା Process;

	ପ୍ରକାରବ୍ୟାଖ୍ୟା std::pair<SigSpec, SigSpec> SigSig;
};

ସଂରଚନା SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_PREFIX ID; }
namespace RTLIL { using YOSYS_NAMESPACE_PREFIX IdString; }

namespace RTLIL {
	ଶୂନ୍ୟ copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			ଅଚଳ dict<IdString, RTLIL::Const> &src,
			ଅଚଳ RTLIL::Design *src_design, RTLIL::Design *dst_design);

	ବାହ୍ୟ dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	ଅଚଳ pool<IdString> &builtin_ff_cell_types();

	ସ୍ଥିର ଧାଡ଼ିଭିତରେ std::string escape_id(ଅଚଳ std::string &str) {
		ଯଦି (str.size() > 0 && str[0] != '\\' && str[0] != '$')
			ଫେରନ୍ତୁ "\\" + str;
		ଫେରନ୍ତୁ str;
	}

	ସ୍ଥିର ଧାଡ଼ିଭିତରେ std::string unescape_id(ଅଚଳ std::string &str) {
		ଯଦି (str.size() < 2)
			ଫେରନ୍ତୁ str;
		ଯଦି (str[0] != '\\')
			ଫେରନ୍ତୁ str;
		ଯଦି (str[1] == '$' || str[1] == '\\')
			ଫେରନ୍ତୁ str;
		ଯଦି (str[1] >= '0' && str[1] <= '9')
			ଫେରନ୍ତୁ str;
		ଫେରନ୍ତୁ str.substr(1);
	}

	template <typename T> using sort_by_name_id = IdString::compare_ptr_by_name<T>;

	template <typename T> ସଂରଚନା sort_by_name_str {
		bool operator()(T *a, T *b) ଅଚଳ {
			ଫେରନ୍ତୁ a->name.lt_by_name(b->name);
		}
	};

	ସଂରଚନା sort_by_id_str {
		ଅଚଳ TwinePool& pool;
		explicit sort_by_id_str(ଅଚଳ TwinePool& pool) : pool(pool) {}
		bool operator()(RTLIL::IdString a, RTLIL::IdString b) ଅଚଳ {
			ଫେରନ୍ତୁ pool.compare_by_name(a, b) < 0;
		}
	};

	ସ୍ଥିର ଧାଡ଼ିଭିତରେ std::string encode_filename(ଅଚଳ std::string &filename)
	{
		std::stringstream val;
		ଯଦି (!std::any_of(filename.begin(), filename.end(), [](ଅକ୍ଷର c) {
			ଫେରନ୍ତୁ static_cast<unsigned ଅକ୍ଷର>(c) < 33 || static_cast<unsigned ଅକ୍ଷର>(c) > 126;
		})) ଫେରନ୍ତୁ filename;
		ପାଇଁ (unsigned ଅକ୍ଷର ଅଚଳ c : filename) {
			ଯଦି (c < 33 || c > 126)
				val << stringf("$%02x", c);
			ଅନ୍ୟଥା
				val << c;
		}
		ଫେରନ୍ତୁ val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_and         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_or          (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_xor         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_xnor        (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);

	RTLIL::Const const_reduce_and  (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_reduce_or   (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_reduce_xor  (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_reduce_xnor (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_reduce_bool (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);

	RTLIL::Const const_logic_not   (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_logic_and   (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_logic_or    (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);

	RTLIL::Const const_shl         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_shr         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_sshl        (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_sshr        (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_shift       (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_shiftx      (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);

	RTLIL::Const const_lt          (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_le          (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_eq          (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_ne          (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_eqx         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_nex         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_ge          (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_gt          (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);

	RTLIL::Const const_add         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_sub         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_mul         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_div         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_divfloor    (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_modfloor    (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_mod         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_pow         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);

	RTLIL::Const const_pos         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_buf         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);
	RTLIL::Const const_neg         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, bool signed1, bool signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len);

	RTLIL::Const const_mux         (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ଅଚଳ RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ଅଚଳ RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ଅଚଳ RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	template<typename T, typename ଚାବି_2 = IdString>
	ସଂରଚନା ObjIterator {
		using iterator_category = std::forward_iterator_tag;
		using ମୂଲ୍ୟ_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		typename dict<ଚାବି_2, T>::iterator it;
		dict<ଚାବି_2, T> *list_p;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			ଯଦି (list_p->empty()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} ଅନ୍ୟଥା {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(ଅଚଳ RTLIL::ObjIterator<T, ଚାବି_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			ଯଦି (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &operator=(ଅଚଳ RTLIL::ObjIterator<T, ଚାବି_2> &other) {
			ଯଦି (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			ଯଦି (refcount_p)
				(*refcount_p)++;
			ଫେରନ୍ତୁ *this;
		}

		~ObjIterator() {
			ଯଦି (refcount_p)
				(*refcount_p)--;
		}

		ଧାଡ଼ିଭିତରେ T operator*() ଅଚଳ {
			log_assert(list_p != nullptr);
			ଫେରନ୍ତୁ it->second;
		}

		ଧାଡ଼ିଭିତରେ bool operator!=(ଅଚଳ RTLIL::ObjIterator<T, ଚାବି_2> &other) ଅଚଳ {
			ଯଦି (list_p == nullptr || other.list_p == nullptr)
				ଫେରନ୍ତୁ list_p != other.list_p;
			ଫେରନ୍ତୁ it != other.it;
		}


		ଧାଡ଼ିଭିତରେ bool operator==(ଅଚଳ RTLIL::ObjIterator<T, ଚାବି_2> &other) ଅଚଳ {
			ଫେରନ୍ତୁ !(*this != other);
		}

		ଧାଡ଼ିଭିତରେ ObjIterator<T, ଚାବି_2>& operator++() {
			log_assert(list_p != nullptr);
			ଯଦି (++it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			ଫେରନ୍ତୁ *this;
		}

		ଧାଡ଼ିଭିତରେ ObjIterator<T, ଚାବି_2>& operator+=(ପୂର୍ଣ୍ଣସଂଖ୍ୟା amt) {
			log_assert(list_p != nullptr);
			it += amt;
			ଯଦି (it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			ଫେରନ୍ତୁ *this;
		}

		ଧାଡ଼ିଭିତରେ ObjIterator<T, ଚାବି_2> operator+(ପୂର୍ଣ୍ଣସଂଖ୍ୟା amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, ଚାବି_2> new_obj(*this);
			new_obj.it += amt;
			ଯଦି (new_obj.it == list_p->end()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			ଫେରନ୍ତୁ new_obj;
		}

		ଧାଡ଼ିଭିତରେ ଅଚଳ ObjIterator<T, ଚାବି_2> operator++(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) {
			ObjIterator<T, ଚାବି_2> result(*this);
			++(*this);
			ଫେରନ୍ତୁ result;
		}
	};

	template<typename T, typename ଚାବି_2 = IdString>
	ସଂରଚନା ObjRange
	{
		dict<ଚାବି_2, T> *list_p;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା *refcount_p;

		ObjRange(decltype(list_p) list_p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, ଚାବି_2> begin() { ଫେରନ୍ତୁ RTLIL::ObjIterator<T, ଚାବି_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, ଚାବି_2> end() { ଫେରନ୍ତୁ RTLIL::ObjIterator<T, ଚାବି_2>(); }

		size_t size() ଅଚଳ {
			ଫେରନ୍ତୁ list_p->size();
		}

		operator pool<T>() ଅଚଳ {
			pool<T> result;
			ପାଇଁ (auto &it : *list_p)
				result.ଭର୍ତ୍ତି(it.second);
			ଫେରନ୍ତୁ result;
		}

		operator std::vector<T>() ଅଚଳ {
			std::vector<T> result;
			result.reserve(list_p->size());
			ପାଇଁ (auto &it : *list_p)
				result.ଠେଲ_back(it.second);
			ଫେରନ୍ତୁ result;
		}

		pool<T> to_pool() ଅଚଳ { ଫେରନ୍ତୁ *this; }
		std::vector<T> to_vector() ଅଚଳ { ଫେରନ୍ତୁ *this; }
	};
};

ସଂରଚନା RTLIL::Const
{
	short ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags;
ବ୍ୟକ୍ତିଗତ:
	friend class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::vector<RTLIL::State>;
	ଗଣନା class backing_tag: bool { bits, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_tag tag;
	ସଂଘ {
		bitvectype bits_;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_bits() ଅଚଳ { ଫେରନ୍ତୁ tag == backing_tag::bits; }
	bool is_str() ଅଚଳ { ଫେରନ୍ତୁ tag == backing_tag::string; }

	bitvectype* get_if_bits() { ଫେରନ୍ତୁ is_bits() ? &bits_ : ଖାଲି; }
	std::string* get_if_str() { ଫେରନ୍ତୁ is_str() ? &str_ : ଖାଲି; }
	ଅଚଳ bitvectype* get_if_bits() ଅଚଳ { ଫେରନ୍ତୁ is_bits() ? &bits_ : ଖାଲି; }
	ଅଚଳ std::string* get_if_str() ଅଚଳ { ଫେରନ୍ତୁ is_str() ? &str_ : ଖାଲି; }

	bitvectype& get_bits();
	std::string& get_str();
	ଅଚଳ bitvectype& get_bits() ଅଚଳ;
	ଅଚଳ std::string& get_str() ଅଚଳ;
	std::vector<RTLIL::State>& bits_internal();
	ଶୂନ୍ୟ bitvectorize_internal();

ସର୍ବସାଧାରଣ:
	Const() : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::vector<RTLIL::State>()) {}
	Const(std::string str);
	Const(long long val); // default width is 32
	Const(long long val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width);
	Const(RTLIL::State bit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 1);
	Const(std::vector<RTLIL::State> bits) : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::move(bits)) {}
	Const(ଅଚଳ std::vector<bool> &bits);
	Const(ଅଚଳ RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &operator =(ଅଚଳ RTLIL::Const &other);
	~Const();

	ସଂରଚନା Builder
	{
		Builder() {}
		Builder(ପୂର୍ଣ୍ଣସଂଖ୍ୟା expected_width) { bits.reserve(expected_width); }
		ଶୂନ୍ୟ ଠେଲ_back(RTLIL::State b) { bits.ଠେଲ_back(b); }
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ { ଫେରନ୍ତୁ static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(bits.size()); }
		Const build() { ଫେରନ୍ତୁ Const(std::move(bits)); }
	ବ୍ୟକ୍ତିଗତ:
		std::vector<RTLIL::State> bits;
	};

	bool operator <(ଅଚଳ RTLIL::Const &other) ଅଚଳ;
	bool operator ==(ଅଚଳ RTLIL::Const &other) ଅଚଳ;
	bool operator !=(ଅଚଳ RTLIL::Const &other) ଅଚଳ;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::vector<RTLIL::State>& bits() { ଫେରନ୍ତୁ bits_internal(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	ଶୂନ୍ୟ bitvectorize() ଅଚଳ { const_cast<Const*>(this)->bitvectorize_internal(); }

	bool as_bool() ଅଚଳ;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା as_int(bool is_signed = false) ଅଚଳ;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_to_int(bool is_signed = false) ଅଚଳ;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> try_as_int(bool is_signed = false) ଅଚଳ;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା as_int_saturating(bool is_signed = false) ଅଚଳ;

	ଶୂନ୍ୟ tag_bare_integer_const(ଅଚଳ std::string &ମୂଲ୍ୟ);

	std::string as_string(ଅଚଳ ଅକ୍ଷର* any = "-") ଅଚଳ;
	ସ୍ଥିର Const from_string(ଅଚଳ std::string &str);
	std::vector<RTLIL::State> to_bits() ଅଚଳ;

	std::string decode_string() ଅଚଳ;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ;
	bool empty() ଅଚଳ;

	ଶୂନ୍ୟ append(ଅଚଳ RTLIL::Const &other);
	ଶୂନ୍ୟ set(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, RTLIL::State state) {
		bits_internal()[i] = state;
	}
	ଶୂନ୍ୟ resize(ପୂର୍ଣ୍ଣସଂଖ୍ୟା size, RTLIL::State fill) {
    log_assert(size >= 0 && size < RTLIL::WIDTH_LIMIT);
		bits_internal().resize(size, fill);
	}

	class const_iterator {
	ବ୍ୟକ୍ତିଗତ:
		ଅଚଳ Const* parent;
		size_t idx;

	ସର୍ବସାଧାରଣ:
		using iterator_category = std::bidirectional_iterator_tag;
		using ମୂଲ୍ୟ_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = ଅଚଳ State*;
		using reference = ଅଚଳ State&;

		const_iterator(ଅଚଳ Const& c, size_t i) : parent(&c), idx(i) {}

		State operator*() ଅଚଳ;

		const_iterator& operator++() { ++idx; ଫେରନ୍ତୁ *this; }
		const_iterator& operator--() { --idx; ଫେରନ୍ତୁ *this; }
		const_iterator operator++(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) { const_iterator result(*this); ++idx; ଫେରନ୍ତୁ result; }
		const_iterator operator--(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) { const_iterator result(*this); --idx; ଫେରନ୍ତୁ result; }
		const_iterator& operator+=(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i) { idx += i; ଫେରନ୍ତୁ *this; }

		const_iterator operator+(ପୂର୍ଣ୍ଣସଂଖ୍ୟା add) {
			ଫେରନ୍ତୁ const_iterator(*parent, idx + add);
		}
		const_iterator operator-(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub) {
			ଫେରନ୍ତୁ const_iterator(*parent, idx - sub);
		}
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା operator-(ଅଚଳ const_iterator& other) {
			ଫେରନ୍ତୁ idx - other.idx;
		}

		bool operator==(ଅଚଳ const_iterator& other) ଅଚଳ {
			ଫେରନ୍ତୁ idx == other.idx;
		}

		bool operator!=(ଅଚଳ const_iterator& other) ଅଚଳ {
			ଫେରନ୍ତୁ !(*this == other);
		}
	};

	class iterator {
	ବ୍ୟକ୍ତିଗତ:
		Const* parent;
		size_t idx;

	ସର୍ବସାଧାରଣ:
		class proxy {
		ବ୍ୟକ୍ତିଗତ:
			Const* parent;
			size_t idx;
		ସର୍ବସାଧାରଣ:
			proxy(Const* parent, size_t idx) : parent(parent), idx(idx) {}
			operator State() ଅଚଳ { ଫେରନ୍ତୁ (*parent)[idx]; }
			proxy& operator=(State s) { parent->set(idx, s); ଫେରନ୍ତୁ *this; }
			proxy& operator=(ଅଚଳ proxy& other) { parent->set(idx, (*other.parent)[other.idx]); ଫେରନ୍ତୁ *this; }
		};

		using iterator_category = std::bidirectional_iterator_tag;
		using ମୂଲ୍ୟ_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = proxy*;
		using reference = proxy;

		iterator(Const& c, size_t i) : parent(&c), idx(i) {}

		proxy operator*() ଅଚଳ { ଫେରନ୍ତୁ proxy(parent, idx); }
		iterator& operator++() { ++idx; ଫେରନ୍ତୁ *this; }
		iterator& operator--() { --idx; ଫେରନ୍ତୁ *this; }
		iterator operator++(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) { iterator result(*this); ++idx; ଫେରନ୍ତୁ result; }
		iterator operator--(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) { iterator result(*this); --idx; ଫେରନ୍ତୁ result; }
		iterator& operator+=(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i) { idx += i; ଫେରନ୍ତୁ *this; }

		iterator operator+(ପୂର୍ଣ୍ଣସଂଖ୍ୟା add) {
			ଫେରନ୍ତୁ iterator(*parent, idx + add);
		}
		iterator operator-(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sub) {
			ଫେରନ୍ତୁ iterator(*parent, idx - sub);
		}
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା operator-(ଅଚଳ iterator& other) {
			ଫେରନ୍ତୁ idx - other.idx;
		}

		bool operator==(ଅଚଳ iterator& other) ଅଚଳ {
			ଫେରନ୍ତୁ idx == other.idx;
		}

		bool operator!=(ଅଚଳ iterator& other) ଅଚଳ {
			ଫେରନ୍ତୁ !(*this == other);
		}
	};

	const_iterator begin() ଅଚଳ {
		ଫେରନ୍ତୁ const_iterator(*this, 0);
	}
	const_iterator end() ଅଚଳ {
		ଫେରନ୍ତୁ const_iterator(*this, size());
	}
	iterator begin() {
		ଫେରନ୍ତୁ iterator(*this, 0);
	}
	iterator end() {
		ଫେରନ୍ତୁ iterator(*this, size());
	}
	State back() ଅଚଳ {
		ଫେରନ୍ତୁ *(end() - 1);
	}
	State front() ଅଚଳ {
		ଫେରନ୍ତୁ *begin();
	}
	State at(size_t i) ଅଚଳ {
		ଫେରନ୍ତୁ *const_iterator(*this, i);
	}
	State operator[](size_t i) ଅଚଳ {
		ଫେରନ୍ତୁ *const_iterator(*this, i);
	}

	bool is_fully_zero() ଅଚଳ;
	bool is_fully_ones() ଅଚଳ;
	bool is_fully_def() ଅଚଳ;
	bool is_fully_undef() ଅଚଳ;
	bool is_fully_undef_x_only() ଅଚଳ;
	bool is_onehot(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pos = nullptr) ଅଚଳ;

	RTLIL::Const extract(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = 1, RTLIL::State padding = RTLIL::State::S0) ଅଚଳ;

	// find the MSB without redundant leading bits
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_min_size(bool is_signed) ଅଚଳ;

	// compress representation to the minimum required bits
	ଶୂନ୍ୟ compress(bool is_signed = false);

	std::optional<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> as_int_compress(bool is_signed) ଅଚଳ;

	ଶୂନ୍ୟ extu(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width) {
		resize(width, RTLIL::State::S0);
	}

	ଶୂନ୍ୟ exts(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width) {
		resize(width, empty() ? RTLIL::State::Sx : back());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;
};

ସଂରଚନା RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	bool has_attribute(RTLIL::IdString id) ଅଚଳ;

	ଶୂନ୍ୟ set_bool_attribute(RTLIL::IdString id, bool ମୂଲ୍ୟ=true);
	bool get_bool_attribute(RTLIL::IdString id) ଅଚଳ;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ଅଚଳ {
		ଫେରନ୍ତୁ get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	ଶୂନ୍ୟ set_string_attribute(RTLIL::IdString id, string ମୂଲ୍ୟ);
	string get_string_attribute(RTLIL::IdString id) ଅଚଳ;

	ଶୂନ୍ୟ set_strpool_attribute(RTLIL::IdString id, ଅଚଳ pool<string> &data);
	ଶୂନ୍ୟ add_strpool_attribute(RTLIL::IdString id, ଅଚଳ pool<string> &data);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ଅଚଳ;

	ଶୂନ୍ୟ set_src_attribute(ଅଚଳ std::string &src) {
		set_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ଅଚଳ {
		ଫେରନ୍ତୁ get_string_attribute(ID::src);
	}

	ଶୂନ୍ୟ set_hdlname_attribute(ଅଚଳ vector<string> &hierarchy);
	vector<string> get_hdlname_attribute() ଅଚଳ;

	ଶୂନ୍ୟ set_intvec_attribute(RTLIL::IdString id, ଅଚଳ vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &data);
	vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> get_intvec_attribute(RTLIL::IdString id) ଅଚଳ;
};

#ଅନ୍ତର୍ଭାଵ "kernel/rtlil_twine_compat.h"

ସଂରଚନା RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

ସଂରଚନା RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::vector<RTLIL::State> data; // only used if wire == NULL, LSB at index 0
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, offset;

	SigChunk() : wire(nullptr), width(0), offset(0) {}
	SigChunk(ଅଚଳ RTLIL::Const &ମୂଲ୍ୟ) : wire(nullptr), data(ମୂଲ୍ୟ.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Const &&ମୂଲ୍ୟ) : wire(nullptr), data(ମୂଲ୍ୟ.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), width(GetSize(wire)), offset(0) {}
	SigChunk(RTLIL::Wire *wire, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 1) : wire(wire), width(width), offset(offset) {}
	SigChunk(ଅଚଳ std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(ପୂର୍ଣ୍ଣସଂଖ୍ୟା val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(ପୂର୍ଣ୍ଣସଂଖ୍ୟା val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width) : SigChunk(RTLIL::Const(val, width)) {}
	SigChunk(RTLIL::State bit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 1) : SigChunk(RTLIL::Const(bit, width)) {}
	SigChunk(ଅଚଳ RTLIL::SigBit &bit);

	RTLIL::SigChunk extract(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା length) ଅଚଳ;
	RTLIL::SigBit operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset) ଅଚଳ;
	ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ { ଫେରନ୍ତୁ width; }
	ଧାଡ଼ିଭିତରେ bool is_wire() ଅଚଳ { ଫେରନ୍ତୁ wire != ଖାଲି; }

	bool operator <(ଅଚଳ RTLIL::SigChunk &other) ଅଚଳ;
	bool operator ==(ଅଚଳ RTLIL::SigChunk &other) ଅଚଳ;
	bool operator !=(ଅଚଳ RTLIL::SigChunk &other) ଅଚଳ;
};

ସଂରଚନା RTLIL::SigBit
{
	RTLIL::Wire *wire;
	ସଂଘ {
		RTLIL::State data; // used if wire == NULL
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset;        // used if wire != NULL
	};

	SigBit();
	SigBit(RTLIL::State bit);
	explicit SigBit(bool bit);
	SigBit(RTLIL::Wire *wire);
	SigBit(RTLIL::Wire *wire, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset);
	SigBit(ଅଚଳ RTLIL::SigChunk &chunk);
	SigBit(ଅଚଳ RTLIL::SigChunk &chunk, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index);
	SigBit(ଅଚଳ RTLIL::SigSpec &sig);
	SigBit(ଅଚଳ RTLIL::SigBit &sigbit) = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	RTLIL::SigBit &operator =(ଅଚଳ RTLIL::SigBit &other) = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ଧାଡ଼ିଭିତରେ bool is_wire() ଅଚଳ { ଫେରନ୍ତୁ wire != ଖାଲି; }

	bool operator <(ଅଚଳ RTLIL::SigBit &other) ଅଚଳ;
	bool operator ==(ଅଚଳ RTLIL::SigBit &other) ଅଚଳ;
	bool operator !=(ଅଚଳ RTLIL::SigBit &other) ଅଚଳ;
	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ;
	[[nodiscard]] Hasher hash_ଶୀର୍ଷ() ଅଚଳ;
};

namespace hashlib {
	template <>
	ସଂରଚନା hash_ops<RTLIL::SigBit> {
		ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool cmp(ଅଚଳ RTLIL::SigBit &a, ଅଚଳ RTLIL::SigBit &b) {
			ଫେରନ୍ତୁ a == b;
		}
		[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash(ଅଚଳ RTLIL::SigBit sb) {
			ଫେରନ୍ତୁ sb.hash_ଶୀର୍ଷ();
		}
		[[nodiscard]] ସ୍ଥିର ଧାଡ଼ିଭିତରେ Hasher hash_into(ଅଚଳ RTLIL::SigBit sb, Hasher h) {
			ଫେରନ୍ତୁ sb.hash_into(h);
		}
	};
};

ସଂରଚନା RTLIL::SigSpecIterator
{
	ପ୍ରକାରବ୍ୟାଖ୍ୟା std::ନିବେଶ_iterator_tag iterator_category;
	ପ୍ରକାରବ୍ୟାଖ୍ୟା RTLIL::SigBit ମୂଲ୍ୟ_type;
	ପ୍ରକାରବ୍ୟାଖ୍ୟା ptrdiff_t difference_type;
	ପ୍ରକାରବ୍ୟାଖ୍ୟା RTLIL::SigBit* pointer;
	ପ୍ରକାରବ୍ୟାଖ୍ୟା RTLIL::SigBit& reference;

	RTLIL::SigSpec *sig_p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା index;

	ଧାଡ଼ିଭିତରେ RTLIL::SigBit &operator*() ଅଚଳ;
	ଧାଡ଼ିଭିତରେ bool operator!=(ଅଚଳ RTLIL::SigSpecIterator &other) ଅଚଳ { ଫେରନ୍ତୁ index != other.index; }
	ଧାଡ଼ିଭିତରେ bool operator==(ଅଚଳ RTLIL::SigSpecIterator &other) ଅଚଳ { ଫେରନ୍ତୁ index == other.index; }
	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ operator++() { index++; }
};

ସଂରଚନା RTLIL::SigSpecConstIterator
{
	ପ୍ରକାରବ୍ୟାଖ୍ୟା std::ନିବେଶ_iterator_tag iterator_category;
	ପ୍ରକାରବ୍ୟାଖ୍ୟା RTLIL::SigBit ମୂଲ୍ୟ_type;
	ପ୍ରକାରବ୍ୟାଖ୍ୟା ptrdiff_t difference_type;
	ପ୍ରକାରବ୍ୟାଖ୍ୟା RTLIL::SigBit* pointer;
	ପ୍ରକାରବ୍ୟାଖ୍ୟା RTLIL::SigBit& reference;

	ଅଚଳ RTLIL::SigSpec *sig_p;
	RTLIL::SigBit bit;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା index;

	ଧାଡ଼ିଭିତରେ ଅଚଳ RTLIL::SigBit &operator*();
	ଧାଡ଼ିଭିତରେ bool operator!=(ଅଚଳ RTLIL::SigSpecConstIterator &other) ଅଚଳ { ଫେରନ୍ତୁ index != other.index; }
	ଧାଡ଼ିଭିତରେ bool operator==(ଅଚଳ RTLIL::SigSpecIterator &other) ଅଚଳ { ଫେରନ୍ତୁ index == other.index; }
	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ operator++() { index++; }
};

ସଂରଚନା RTLIL::SigSpec
{
ବ୍ୟକ୍ତିଗତ:
	friend class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, Extract);
	ଗଣନା Representation : ଅକ୍ଷର {
		CHUNK,
		BITS,
	};
	// An AtomicHash is either clear or a nonzero integer.
	ସଂରଚନା AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ଅଚଳ AtomicHash &rhs) : atomic_(rhs.load()) {}
		AtomicHash &operator=(ଅଚଳ AtomicHash &rhs) { store(rhs.load()); ଫେରନ୍ତୁ *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> ପଠ() ଅଚଳ {
			Hasher::hash_t ମୂଲ୍ୟ = load();
			ଯଦି (ମୂଲ୍ୟ == 0)
				ଫେରନ୍ତୁ std::nullopt;
			ଫେରନ୍ତୁ ମୂଲ୍ୟ;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		ଶୂନ୍ୟ set(Hasher::hash_t ମୂଲ୍ୟ) ଅଚଳ {
			log_assert(ମୂଲ୍ୟ != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(ମୂଲ୍ୟ, std::memory_order_relaxed);
			log_assert(old == 0 || old == ମୂଲ୍ୟ);
		}
		ଶୂନ୍ୟ clear() { store(0); }
	ବ୍ୟକ୍ତିଗତ:
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା load() ଅଚଳ { ଫେରନ୍ତୁ atomic_.load(std::memory_order_relaxed); }
		ଶୂନ୍ୟ store(Hasher::hash_t ମୂଲ୍ୟ) ଅଚଳ {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(ମୂଲ୍ୟ, std::memory_order_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	ସଂଘ {
		RTLIL::SigChunk chunk_;
		std::vector<RTLIL::SigBit> bits_; // LSB at index 0
	};

	ଶୂନ୍ୟ init_empty_bits() {
		rep_ = BITS;
		new (&bits_) std::vector<RTLIL::SigBit>;
	}

	ଶୂନ୍ୟ unpack();
	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ inline_unpack() {
		ଯଦି (rep_ == CHUNK)
			unpack();
	}
	ଶୂନ୍ୟ try_repack();

	Hasher::hash_t updhash() ଅଚଳ;
	ଶୂନ୍ୟ destroy() {
		ଯଦି (rep_ == CHUNK)
			chunk_.~SigChunk();
		ଅନ୍ୟଥା
			bits_.~vector();
	}
	friend ସଂରଚନା Chunks;

ସର୍ବସାଧାରଣ:
	SigSpec() { init_empty_bits(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ଅଚଳ SigSpec &ମୂଲ୍ୟ) : rep_(ମୂଲ୍ୟ.rep_), hash_(ମୂଲ୍ୟ.hash_) {
		ଯଦି (ମୂଲ୍ୟ.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(ମୂଲ୍ୟ.chunk_);
		ଅନ୍ୟଥା
			new (&bits_) std::vector<RTLIL::SigBit>(ମୂଲ୍ୟ.bits_);
	}
	SigSpec(SigSpec &&ମୂଲ୍ୟ) : rep_(ମୂଲ୍ୟ.rep_), hash_(ମୂଲ୍ୟ.hash_) {
		ଯଦି (ମୂଲ୍ୟ.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(ମୂଲ୍ୟ.chunk_));
		ଅନ୍ୟଥା
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(ମୂଲ୍ୟ.bits_));
	}
	SigSpec(ଅଚଳ RTLIL::Const &ମୂଲ୍ୟ);
	SigSpec(RTLIL::Const &&ମୂଲ୍ୟ);
	SigSpec(ଅଚଳ RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 1);
	SigSpec(ଅଚଳ std::string &str);
	SigSpec(ପୂର୍ଣ୍ଣସଂଖ୍ୟା val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 32);
	SigSpec(RTLIL::State bit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 1);
	SigSpec(ଅଚଳ RTLIL::SigBit &bit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 1);
	SigSpec(ଅଚଳ std::vector<RTLIL::SigChunk> &chunks);
	SigSpec(ଅଚଳ std::vector<RTLIL::SigBit> &bits);
	SigSpec(ଅଚଳ pool<RTLIL::SigBit> &bits);
	SigSpec(ଅଚଳ std::set<RTLIL::SigBit> &bits);
	explicit SigSpec(bool bit);
	~SigSpec() {
		destroy();
	}

	SigSpec &operator=(ଅଚଳ SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		ଯଦି (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		ଅନ୍ୟଥା
			new (&bits_) std::vector<RTLIL::SigBit>(rhs.bits_);
		ଫେରନ୍ତୁ *this;
	}
	SigSpec &operator=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		ଯଦି (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		ଅନ୍ୟଥା
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(rhs.bits_));
		ଫେରନ୍ତୁ *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	ସଂରଚନା Chunks {
		Chunks(ଅଚଳ SigSpec &spec) : spec(spec) {}
		ସଂରଚନା const_iterator {
			using iterator_category = std::forward_iterator_tag;
			using ମୂଲ୍ୟ_type = ଅଚଳ SigChunk &;
			using difference_type = std::ptrdiff_t;
			using pointer = ଅଚଳ SigChunk *;
			using reference = ଅଚଳ SigChunk &;

			ଅଚଳ SigSpec &spec;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା bit_index;
			SigChunk chunk;

			const_iterator(ଅଚଳ SigSpec &spec) : spec(spec) {
				bit_index = 0;
				ଯଦି (spec.rep_ == BITS)
					next_chunk_bits();
			}
			ଗଣନା End { END };
			const_iterator(ଅଚଳ SigSpec &spec, End) : spec(spec) {
				bit_index = spec.size();
			}
			ଶୂନ୍ୟ next_chunk_bits();

			ଅଚଳ SigChunk &operator*() {
				ଯଦି (spec.rep_ == CHUNK)
					ଫେରନ୍ତୁ spec.chunk_;
				ଫେରନ୍ତୁ chunk;
			};
			ଅଚଳ SigChunk *operator->() { ଫେରନ୍ତୁ &**this; }
			const_iterator &operator++() {
				bit_index += (**this).width;
				ଯଦି (spec.rep_ == BITS)
					next_chunk_bits();
				ଫେରନ୍ତୁ *this;
			}
			bool operator==(ଅଚଳ const_iterator &rhs) ଅଚଳ { ଫେରନ୍ତୁ bit_index == rhs.bit_index; }
			bool operator!=(ଅଚଳ const_iterator &rhs) ଅଚଳ { ଫେରନ୍ତୁ !(*this == rhs); }
		};
		const_iterator begin() ଅଚଳ { ଫେରନ୍ତୁ const_iterator(spec); }
		const_iterator end() ଅଚଳ {
			const_iterator it(spec, const_iterator::END);
			ଫେରନ୍ତୁ it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rbegin() {
			ensure_chunk_vector();
			ଫେରନ୍ତୁ chunk_vector.rbegin();
		}
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rend() {
			ensure_chunk_vector();
			ଫେରନ୍ତୁ chunk_vector.rend();
		}
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() {
			ensure_chunk_vector();
			ଫେରନ୍ତୁ chunk_vector.size();
		}
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ {
			ଫେରନ୍ତୁ std::distance(begin(), end());
		}
		ଅଚଳ SigChunk &at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) {
			ensure_chunk_vector();
			ଫେରନ୍ତୁ chunk_vector.at(index);
		}
		operator ଅଚଳ std::vector<RTLIL::SigChunk>&() {
			ensure_chunk_vector();
			ଫେରନ୍ତୁ chunk_vector;
		}
	ବ୍ୟକ୍ତିଗତ:
		ଶୂନ୍ୟ ensure_chunk_vector() {
			ଯଦି (spec.size() > 0 && chunk_vector.empty()) {
				ପାଇଁ (ଅଚଳ RTLIL::SigChunk &c : *this)
					chunk_vector.ଠେଲ_back(c);
			}
		}
		ଅଚଳ SigSpec &spec;
		std::vector<RTLIL::SigChunk> chunk_vector;
	};
	friend ସଂରଚନା Chunks::const_iterator;

	ଧାଡ଼ିଭିତରେ Chunks chunks() ଅଚଳ { ଫେରନ୍ତୁ {*this}; }
	ଧାଡ଼ିଭିତରେ ଅଚଳ SigSpec &bits() ଅଚଳ { ଫେରନ୍ତୁ *this; }

	ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ { ଫେରନ୍ତୁ rep_ == CHUNK ? chunk_.width : GetSize(bits_); }
	ଧାଡ଼ିଭିତରେ bool empty() ଅଚଳ { ଫେରନ୍ତୁ size() == 0; };

	ଧାଡ଼ିଭିତରେ RTLIL::SigBit &operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) { inline_unpack(); hash_.clear(); ଫେରନ୍ତୁ bits_.at(index); }
	ଧାଡ଼ିଭିତରେ RTLIL::SigBit operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ {
		ଯଦି (rep_ == CHUNK) {
			ଯଦି (index < 0 || index >= chunk_.width)
				throw std::out_of_range("SigSpec::operator[]");
			ଯଦି (chunk_.wire)
				ଫେରନ୍ତୁ RTLIL::SigBit(chunk_.wire, chunk_.offset + index);
			ଫେରନ୍ତୁ RTLIL::SigBit(chunk_.data[index]);
		}
		ଫେରନ୍ତୁ bits_.at(index);
	}

	ଧାଡ଼ିଭିତରେ RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = 0; ଫେରନ୍ତୁ it; }
	ଧାଡ଼ିଭିତରେ RTLIL::SigSpecIterator end() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = size(); ଫେରନ୍ତୁ it; }

	ଧାଡ଼ିଭିତରେ RTLIL::SigSpecConstIterator begin() ଅଚଳ { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = 0; ଫେରନ୍ତୁ it; }
	ଧାଡ଼ିଭିତରେ RTLIL::SigSpecConstIterator end() ଅଚଳ { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = size(); ଫେରନ୍ତୁ it; }

	ଶୂନ୍ୟ sort();
	ଶୂନ୍ୟ sort_and_unify();

	ଶୂନ୍ୟ replace(ଅଚଳ RTLIL::SigSpec &pattern, ଅଚଳ RTLIL::SigSpec &with);
	ଶୂନ୍ୟ replace(ଅଚଳ RTLIL::SigSpec &pattern, ଅଚଳ RTLIL::SigSpec &with, RTLIL::SigSpec *other) ଅଚଳ;

	ଶୂନ୍ୟ replace(ଅଚଳ dict<RTLIL::SigBit, RTLIL::SigBit> &rules);
	ଶୂନ୍ୟ replace(ଅଚଳ dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ଅଚଳ;

	ଶୂନ୍ୟ replace(ଅଚଳ std::map<RTLIL::SigBit, RTLIL::SigBit> &rules);
	ଶୂନ୍ୟ replace(ଅଚଳ std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ଅଚଳ;

	ଶୂନ୍ୟ replace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ଅଚଳ RTLIL::SigSpec &with);

	ଶୂନ୍ୟ remove(ଅଚଳ RTLIL::SigSpec &pattern);
	ଶୂନ୍ୟ remove(ଅଚଳ RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) ଅଚଳ;
	ଶୂନ୍ୟ remove2(ଅଚଳ RTLIL::SigSpec &pattern, RTLIL::SigSpec *other);

	ଶୂନ୍ୟ remove(ଅଚଳ pool<RTLIL::SigBit> &pattern);
	ଶୂନ୍ୟ remove(ଅଚଳ pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) ଅଚଳ;
	ଶୂନ୍ୟ remove2(ଅଚଳ pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	ଶୂନ୍ୟ remove2(ଅଚଳ std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	ଶୂନ୍ୟ remove2(ଅଚଳ pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other);

	ଶୂନ୍ୟ remove(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା length = 1);
	ଶୂନ୍ୟ remove_const();

	RTLIL::SigSpec extract(ଅଚଳ RTLIL::SigSpec &pattern, ଅଚଳ RTLIL::SigSpec *other = ଖାଲି) ଅଚଳ;
	RTLIL::SigSpec extract(ଅଚଳ pool<RTLIL::SigBit> &pattern, ଅଚଳ RTLIL::SigSpec *other = ଖାଲି) ଅଚଳ;
	RTLIL::SigSpec extract(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା length = 1) ଅଚଳ;
	RTLIL::SigSpec extract_end(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset) ଅଚଳ { ଫେରନ୍ତୁ extract(offset, size() - offset); }

	ଶୂନ୍ୟ rewrite_wires(std::function<ଶୂନ୍ୟ(RTLIL::Wire*& wire)> rewrite);

	RTLIL::SigBit lsb() ଅଚଳ { log_assert(size()); ଫେରନ୍ତୁ (*this)[0]; };
	RTLIL::SigBit msb() ଅଚଳ { log_assert(size()); ଫେରନ୍ତୁ (*this)[size() - 1]; };
	RTLIL::SigBit front() ଅଚଳ { ଫେରନ୍ତୁ (*this)[0]; }
	RTLIL::SigBit back() ଅଚଳ { ଫେରନ୍ତୁ (*this)[size() - 1]; }

	ଶୂନ୍ୟ append(ଅଚଳ RTLIL::SigSpec &ସଙ୍କେତ);
	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ append(ଅଚଳ RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ append(ଅଚଳ RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	ଶୂନ୍ୟ append(ଅଚଳ RTLIL::SigBit &bit);
	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ append(RTLIL::State state) { append(RTLIL::SigBit(state)); }
	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ append(bool bool_) { append(RTLIL::SigBit(bool_)); }

	ଶୂନ୍ୟ extend_u0(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, bool is_signed = false);

	RTLIL::SigSpec repeat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ) ଅଚଳ;

	ଶୂନ୍ୟ reverse() { inline_unpack(); std::reverse(bits_.begin(), bits_.end()); }

	bool operator <(ଅଚଳ RTLIL::SigSpec &other) ଅଚଳ;
	bool operator ==(ଅଚଳ RTLIL::SigSpec &other) ଅଚଳ;
	ଧାଡ଼ିଭିତରେ bool operator !=(ଅଚଳ RTLIL::SigSpec &other) ଅଚଳ { ଫେରନ୍ତୁ !(*this == other); }

	bool is_wire() ଅଚଳ;
	bool is_chunk() ଅଚଳ;
	ଧାଡ଼ିଭିତରେ bool is_bit() ଅଚଳ { ଫେରନ୍ତୁ size() == 1; }

	bool known_driver() ଅଚଳ;

	bool is_fully_const() ଅଚଳ;
	bool is_fully_zero() ଅଚଳ;
	bool is_fully_ones() ଅଚଳ;
	bool is_fully_def() ଅଚଳ;
	bool is_fully_undef() ଅଚଳ;
	bool has_const() ଅଚଳ;
	bool has_const(State state) ଅଚଳ;
	bool has_marked_bits() ଅଚଳ;
	bool is_onehot(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pos = nullptr) ଅଚଳ;

	bool as_bool() ଅଚଳ;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା as_int(bool is_signed = false) ଅଚଳ;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_to_int(bool is_signed = false) ଅଚଳ;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> try_as_int(bool is_signed = false) ଅଚଳ;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା as_int_saturating(bool is_signed = false) ଅଚଳ;

	std::string as_string() ଅଚଳ;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() ଅଚଳ;
	RTLIL::Const as_const() ଅଚଳ;
	RTLIL::Wire *as_wire() ଅଚଳ;
	RTLIL::SigChunk as_chunk() ଅଚଳ;
	RTLIL::SigBit as_bit() ଅଚଳ;

	bool match(ଅଚଳ ଅକ୍ଷର* pattern) ଅଚଳ;

	std::set<RTLIL::SigBit> to_sigbit_set() ଅଚଳ;
	pool<RTLIL::SigBit> to_sigbit_pool() ଅଚଳ;
	std::vector<RTLIL::SigBit> to_sigbit_vector() ଅଚଳ;
	std::map<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_map(ଅଚଳ RTLIL::SigSpec &other) ଅଚଳ;
	dict<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_dict(ଅଚଳ RTLIL::SigSpec &other) ଅଚଳ;

	ସ୍ଥିର bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	ସ୍ଥିର bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	ସ୍ଥିର bool parse_rhs(ଅଚଳ RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	operator std::vector<RTLIL::SigChunk>() ଅଚଳ;
	operator std::vector<RTLIL::SigBit>() ଅଚଳ { ଫେରନ୍ତୁ to_sigbit_vector(); }
	ଅଚଳ RTLIL::SigBit &at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ଅଚଳ RTLIL::SigBit &defval) { ଫେରନ୍ତୁ offset < size() ? (*this)[offset] : defval; }
	RTLIL::SigBit& at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset) { ଫେରନ୍ତୁ (*this)[offset]; }
	RTLIL::SigBit at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset) ଅଚଳ { ଫେରନ୍ତୁ (*this)[offset]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
		Hasher::hash_t val;
		ଯଦି (std::optional<Hasher::hash_t> current = hash_.ପଠ())
			val = *current;
		ଅନ୍ୟଥା
			val = updhash();
		h.eat(val);
		ଫେରନ୍ତୁ h;
	}

#ifndef NDEBUG
	ଶୂନ୍ୟ check(ଅଚଳ Module *mod = nullptr) ଅଚଳ;
#ଅନ୍ୟଥା
	ଶୂନ୍ୟ check(ଅଚଳ Module *mod = nullptr) ଅଚଳ { (ଶୂନ୍ୟ)mod; }
#endif
};

ସଂରଚନା RTLIL::Selection
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
	bool boxed_module(RTLIL::IdString mod_name) ଅଚଳ;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ଅଚଳ;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) ଅଚଳ;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ଅଚଳ;

	// optimizes this selection for the given design by:
	// - removing non-existent modules and members, any boxed modules and
	//   their members (if selection does not include boxes), and any
	//   partially selected modules with no selected members;
	// - marking partially selected modules as wholly selected if all
	//   members of that module are selected; and
	// - marking selection as a complete_selection if all modules in the
	//   given design are selected, or a full_selection if it does not
	//   include boxes.
	ଶୂନ୍ୟ optimize(RTLIL::Design *design);

	// checks if selection covers full design (may or may not include
	// boxed-modules)
	bool selects_all() ଅଚଳ {
		ଫେରନ୍ତୁ full_selection || complete_selection;
	}

	// add whole module to this selection
	template<typename T1> ଶୂନ୍ୟ select(T1 *module) {
		ଯଦି (!selects_all() && selected_modules.ଗଣନା_2(module->name) == 0) {
			IdString name = module->name;
			selected_modules.ଭର୍ତ୍ତି(name);
			selected_members.erase(name);
			ଯଦି (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	template<typename T1, typename T2> ଶୂନ୍ୟ select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	template<typename T1> ଶୂନ୍ୟ select(T1 *module, RTLIL::IdString member) {
		ଯଦି (!selects_all() && selected_modules.ଗଣନା_2(module->name.ref()) == 0) {
			selected_members[module->name.ref()].ଭର୍ତ୍ତି(member);
			ଯଦି (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool empty() ଅଚଳ {
		ଫେରନ୍ତୁ !selects_all() && selected_modules.empty() && selected_members.empty();
	}

	// clear this selection, leaving it empty
	ଶୂନ୍ୟ clear();

	// create a new selection which is empty
	ସ୍ଥିର Selection EmptySelection(RTLIL::Design *design = nullptr) { ଫେରନ୍ତୁ Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	ସ୍ଥିର Selection FullSelection(RTLIL::Design *design = nullptr) { ଫେରନ୍ତୁ Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	ସ୍ଥିର Selection CompleteSelection(RTLIL::Design *design = nullptr) { ଫେରନ୍ତୁ Selection(true, true, design); };
};

ସଂରଚନା RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ { h.eat(hashidx_); ଫେରନ୍ତୁ h; }

	Monitor() {
		ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashidx_ଗଣନା = 123456789;
		hashidx_ଗଣନା = mkhash_xorshift(hashidx_ଗଣନା);
		hashidx_ = hashidx_ଗଣନା;
	}

	ଆଭାସୀ ~Monitor() { }
	ଆଭାସୀ ଶୂନ୍ୟ notify_module_add(RTLIL::Module*) { }
	ଆଭାସୀ ଶୂନ୍ୟ notify_module_del(RTLIL::Module*) { }
	ଆଭାସୀ ଶୂନ୍ୟ notify_connect(RTLIL::Cell*, RTLIL::IdString, ଅଚଳ RTLIL::SigSpec&, ଅଚଳ RTLIL::SigSpec&) { }
	ଆଭାସୀ ଶୂନ୍ୟ notify_connect(RTLIL::Module*, ଅଚଳ RTLIL::SigSig&) { }
	ଆଭାସୀ ଶୂନ୍ୟ notify_connect(RTLIL::Module*, ଅଚଳ std::vector<RTLIL::SigSig>&) { }
	ଆଭାସୀ ଶୂନ୍ୟ notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
ସଂରଚନା define_map_t;

template<typename N>
ଧାଡ଼ିଭିତରେ constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::Leaf> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ଅଚଳ ଅକ୍ଷର*> || std::is_same_v<std::decay_t<N>, ଅକ୍ଷର*>;

#define YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, ପୂର୍ଣ୍ଣସଂଖ୍ୟା> = 0

template<typename N>
ଧାଡ଼ିଭିତରେ constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ଅଚଳ ଅକ୍ଷର *> || std::is_same_v<std::decay_t<N>, ଅକ୍ଷର *>;

#define YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, ପୂର୍ଣ୍ଣସଂଖ୍ୟା> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#define YS_NAME_FWD_POOL(_func, _pool) \
	template<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(N name, Rest&&... rest) \
		{ ଫେରନ୍ତୁ _func(_pool.add(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#define YS_NAME_FWD_2ND_POOL(_func, _pool) \
	template<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(T &&first, N name) \
		{ ଫେରନ୍ତୁ _func(std::forward<T>(first), _pool.add(std::move(name))); }

#define YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#define YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

ସଂରଚନା RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ { h.eat(hashidx_); ଫେରନ୍ତୁ h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	ଶୂନ୍ୟ bufNormalize(bool enable=true);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

ବ୍ୟକ୍ତିଗତ:
	TwinePool twines_;

ସର୍ବସାଧାରଣ:
	TwinePool &twines() { ଫେରନ୍ତୁ twines_; }
	ଅଚଳ TwinePool &twines() ଅଚଳ { ଫେରନ୍ତୁ twines_; }

	ଶୂନ୍ୟ absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	size_t gc_twines();

	std::vector<std::unique_ptr<AST::Ast_ଗଣ୍ଠି>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_defines;

	std::vector<RTLIL::Selection> selection_ଗଦା;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_active_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ଅଚଳ RTLIL::Module *module(RTLIL::IdString name) ଅଚଳ;
	template<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ ଫେରନ୍ତୁ module(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ଅଚଳ RTLIL::Module *module(N name) ଅଚଳ
		{ ଫେରନ୍ତୁ module(twines().find(std::string_view(name))); }
	RTLIL::Module *ଶୀର୍ଷ_module() ଅଚଳ;

	bool has(RTLIL::IdString id) ଅଚଳ {
		twines_.check_owned(id);
		ଫେରନ୍ତୁ modules_.ଗଣନା_2(id) != 0;
	}
	template<typename N, YS_NAME_STRING(N)> bool has(N name) ଅଚଳ
		{ ଫେରନ୍ତୁ has(twines().find(std::string_view(name))); }

	ଶୂନ୍ୟ add(RTLIL::Module *module);

	RTLIL::Module *addModule(RTLIL::IdString name);
	YS_NAME_FWD_POOL(addModule, twines())
	ଶୂନ୍ୟ remove(RTLIL::Module *module);
	ଶୂନ୍ୟ rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	ଶୂନ୍ୟ scratchpad_unset(ଅଚଳ std::string &varname);

	ଶୂନ୍ୟ scratchpad_set_int(ଅଚଳ std::string &varname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ);
	ଶୂନ୍ୟ scratchpad_set_bool(ଅଚଳ std::string &varname, bool ମୂଲ୍ୟ);
	ଶୂନ୍ୟ scratchpad_set_string(ଅଚଳ std::string &varname, std::string ମୂଲ୍ୟ);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା scratchpad_get_int(ଅଚଳ std::string &varname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା default_ମୂଲ୍ୟ_2 = 0) ଅଚଳ;
	bool scratchpad_get_bool(ଅଚଳ std::string &varname, bool default_ମୂଲ୍ୟ_2 = false) ଅଚଳ;
	std::string scratchpad_get_string(ଅଚଳ std::string &varname, ଅଚଳ std::string &default_ମୂଲ୍ୟ_2 = std::string()) ଅଚଳ;

	ଶୂନ୍ୟ sort();
	ଶୂନ୍ୟ sort_modules();
	ଶୂନ୍ୟ check();
	ଶୂନ୍ୟ optimize();

	ଶୂନ୍ୟ clone_into(RTLIL::Design *dst) ଅଚଳ;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ଅଚଳ;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) ଅଚଳ;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ଅଚଳ;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ଅଚଳ;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::Module *mod) ଅଚଳ;

	// push the given selection to the selection stack
	ଶୂନ୍ୟ ଠେଲ_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	ଶୂନ୍ୟ ଠେଲ_empty_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	ଶୂନ୍ୟ ଠେଲ_full_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	ଶୂନ୍ୟ ଠେଲ_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	ଶୂନ୍ୟ କାଢ଼_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		ଫେରନ୍ତୁ selection_ଗଦା.back();
	}

	// get the current selection
	ଅଚଳ RTLIL::Selection &selection() ଅଚଳ {
		ଫେରନ୍ତୁ selection_ଗଦା.back();
	}

	// is the current selection a full selection (no boxes)
	bool full_selection() ଅଚଳ {
		ଫେରନ୍ତୁ selection().full_selection;
	}

	// is the given module in the current selection
	template<typename T1> bool selected(T1 *module) ଅଚଳ {
		ଫେରନ୍ତୁ selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	template<typename T1, typename T2> bool selected(T1 *module, T2 *member) ଅଚଳ {
		ଫେରନ୍ତୁ selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	template<typename T1> ଶୂନ୍ୟ select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	template<typename T1, typename T2> ଶୂନ୍ୟ select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::vector<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) ଅଚଳ;

	// returns all selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_modules() ଅଚଳ { ଫେରନ୍ତୁ selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::vector<RTLIL::Module*> selected_unboxed_modules() ଅଚଳ { ଫେରନ୍ତୁ selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_modules_warn() ଅଚଳ { ଫେରନ୍ତୁ selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::vector<RTLIL::Module*> selected_whole_modules() ଅଚଳ { ଫେରନ୍ତୁ selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_whole_modules() ଅଚଳ { ଫେରନ୍ତୁ selected_modules(SELECT_WHOLE_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::vector<RTLIL::Module*> selected_whole_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ଅଚଳ { ଫେରନ୍ତୁ selected_modules(SELECT_WHOLE_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules() ଅଚଳ { ଫେରନ୍ତୁ selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules_warn() ଅଚଳ { ଫେରନ୍ତୁ selected_modules(SELECT_WHOLE_WARN, SB_UNBOXED_WARN); }

	ସ୍ଥିର std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Design*> *get_all_designs(ଶୂନ୍ୟ);

	std::string to_rtlil_str(bool only_selected = true) ଅଚଳ;
};

ସଂରଚନା RTLIL::Module : ସର୍ବସାଧାରଣ RTLIL::AttrObject
{
	friend ସଂରଚନା RTLIL::Cell;
	friend ସଂରଚନା RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ { h.eat(hashidx_); ଫେରନ୍ତୁ h; }

ସୁରକ୍ଷିତ:
	ଶୂନ୍ୟ add(RTLIL::Wire *wire);
	ଶୂନ୍ୟ add(RTLIL::Cell *cell);
	ଶୂନ୍ୟ add(RTLIL::Process *process);

ସର୍ବସାଧାରଣ:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ଅଚଳ { ଫେରନ୍ତୁ design->twines(); }

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା refcount_wires_;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::vector<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_ମୂଲ୍ୟଗୁଡ଼ିକ;
	dict<RTLIL::IdString, RTLIL::Memory*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	ଆଭାସୀ ~Module();
	ଆଭାସୀ RTLIL::IdString derive(RTLIL::Design *design, ଅଚଳ dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	ଆଭାସୀ RTLIL::IdString derive(RTLIL::Design *design, ଅଚଳ dict<RTLIL::IdString, RTLIL::Const> &parameters, ଅଚଳ dict<RTLIL::IdString, RTLIL::Module*> &interfaces, ଅଚଳ dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	ଆଭାସୀ size_t ଗଣନା_id(RTLIL::IdString id);
	ଆଭାସୀ ଶୂନ୍ୟ expand_interfaces(RTLIL::Design *design, ଅଚଳ dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	ଆଭାସୀ bool reprocess_if_necessary(RTLIL::Design *design);

	ଆଭାସୀ ଶୂନ୍ୟ sort();
	ଆଭାସୀ ଶୂନ୍ୟ check();
	ଆଭାସୀ ଶୂନ୍ୟ optimize();
	ଆଭାସୀ ଶୂନ୍ୟ makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ଅଚଳ {
		ଫେରନ୍ତୁ get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	ଶୂନ୍ୟ connect(ଅଚଳ RTLIL::SigSig &conn);
	ଶୂନ୍ୟ connect(ଅଚଳ RTLIL::SigSpec &lhs, ଅଚଳ RTLIL::SigSpec &rhs);
	ଶୂନ୍ୟ new_connections(ଅଚଳ std::vector<RTLIL::SigSig> &new_conn);
	ଅଚଳ std::vector<RTLIL::SigSig> &connections() ଅଚଳ;

	std::vector<RTLIL::IdString> ports;
	ଶୂନ୍ୟ fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_queue;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_port_queue;
	pool<RTLIL::Wire *> buf_norm_wire_queue;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_index;
	ଶୂନ୍ୟ bufNormalize();

	template<typename T> ଶୂନ୍ୟ rewrite_sigspecs(T &functor);
	template<typename T> ଶୂନ୍ୟ rewrite_sigspecs2(T &functor);
	ଶୂନ୍ୟ cloneInto(RTLIL::Module *new_mod) ଅଚଳ;
	ଆଭାସୀ RTLIL::Module *clone() ଅଚଳ;
	ଆଭାସୀ RTLIL::Module *clone(RTLIL::Design *dst) ଅଚଳ;
	ଆଭାସୀ RTLIL::Module *clone(RTLIL::Design *dst, IdString target_name) ଅଚଳ;

	bool has_memories() ଅଚଳ;
	bool has_processes() ଅଚଳ;

	bool has_memories_warn() ଅଚଳ;
	bool has_processes_warn() ଅଚଳ;

	bool is_selected() ଅଚଳ;
	bool is_selected_whole() ଅଚଳ;

	std::vector<RTLIL::Wire*> selected_wires() ଅଚଳ;
	std::vector<RTLIL::Cell*> selected_cells() ଅଚଳ;
	std::vector<RTLIL::Memory*> selected_memories() ଅଚଳ;
	std::vector<RTLIL::Process*> selected_processes() ଅଚଳ;
	std::vector<RTLIL::SelectedMember> selected_members() ଅଚଳ;

	template<typename T> bool selected(T *member) ଅଚଳ {
		ଫେରନ୍ତୁ design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		ଯଦି (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		ଫେରନ୍ତୁ it == wires_.end() ? nullptr : it->second;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		ଯଦି (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		ଫେରନ୍ତୁ it == cells_.end() ? nullptr : it->second;
	}

	ଅଚଳ RTLIL::Wire* wire(RTLIL::IdString id) ଅଚଳ{
		ଯଦି (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		ଫେରନ୍ତୁ it == wires_.end() ? nullptr : it->second;
	}
	ଅଚଳ RTLIL::Cell* cell(RTLIL::IdString id) ଅଚଳ {
		ଯଦି (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		ଫେରନ୍ତୁ it == cells_.end() ? nullptr : it->second;
	}

	template<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ ଫେରନ୍ତୁ wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ ଫେରନ୍ତୁ cell(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ଅଚଳ RTLIL::Wire *wire(N name) ଅଚଳ
		{ ଫେରନ୍ତୁ wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ଅଚଳ RTLIL::Cell *cell(N name) ଅଚଳ
		{ ଫେରନ୍ତୁ cell(twines().find(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { ଫେରନ୍ତୁ RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା wires_size() ଅଚଳ { ଫେରନ୍ତୁ wires_.size(); }
	RTLIL::Wire* wire_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ { ଫେରନ୍ତୁ wires_.element(index)->second; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { ଫେରନ୍ତୁ RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା cells_size() ଅଚଳ { ଫେରନ୍ତୁ cells_.size(); }
	RTLIL::Cell* cell_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ { ଫେରନ୍ତୁ cells_.element(index)->second; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	ଶୂନ୍ୟ remove(ଅଚଳ pool<RTLIL::Wire*> &wires);
	ଶୂନ୍ୟ remove(RTLIL::Cell *cell);
	ଶୂନ୍ୟ remove(RTLIL::Memory *memory);
	ଶୂନ୍ୟ remove(RTLIL::Process *process);

	ଶୂନ୍ୟ rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	ଶୂନ୍ୟ rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	ଶୂନ୍ୟ rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	ଶୂନ୍ୟ swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2);
	ଶୂନ୍ୟ swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା &index);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *addWire(RTLIL::IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 1);
	RTLIL::Wire *addWire(RTLIL::IdString name, ଅଚଳ RTLIL::Wire *other);
	YS_NAME_FWD(addWire)

	RTLIL::Cell *addCell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *addCell(RTLIL::IdString name, ଅଚଳ RTLIL::Cell *other);
	YS_NAME_FWD(addCell)
	template<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *addCell(RTLIL::IdString name, T type) { ଫେରନ୍ତୁ addCell(name, design->twines().add(std::move(type))); }

	RTLIL::Memory *addMemory(RTLIL::IdString name);
	YS_NAME_FWD(addMemory)
	RTLIL::Memory *addMemory(RTLIL::IdString name, ଅଚଳ RTLIL::Memory *other);

	RTLIL::Process *addProcess(RTLIL::IdString name);
	YS_NAME_FWD(addProcess)
	RTLIL::Process *addProcess(RTLIL::IdString name, ଅଚଳ RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* addNot     (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addPos     (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addBuf     (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addBarrier (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addNeg     (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addAnd  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addOr   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addXor  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addXnor (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addReduceAnd  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addReduceOr   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addReduceXor  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addReduceXnor (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addReduceBool (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addShl    (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addShr    (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addSshl   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addSshr   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addShift  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addShiftx (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addLt  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addLe  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addEq  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addNe  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addEqx (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addNex (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addGe  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addGt  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addAdd (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addSub (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addMul (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	// truncating division
	RTLIL::Cell* addDiv (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	// truncating modulo
	RTLIL::Cell* addMod (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDivFloor (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addModFloor (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addPow (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addFa (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_c, ଅଚଳ RTLIL::SigSpec &sig_x, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addLogicNot (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addLogicAnd (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addLogicOr  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, bool is_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addMux  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addPmux (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addBmux (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDemux (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addBweqx  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addBwmux  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addSlice  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, RTLIL::Const offset, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addConcat (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addLut    (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_y, RTLIL::Const lut, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addTribuf (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAssert (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAssume (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addLive   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addFair   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addCover  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addEquiv  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addSr    (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr, ଅଚଳ RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addFf    (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDff   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_d,   ଅଚଳ RTLIL::SigSpec &sig_q, bool clk_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDffe  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en,  ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDffsr (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDffsre (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAdff (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_arst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, RTLIL::Const arst_ମୂଲ୍ୟ, bool clk_polarity = true, bool arst_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAdffe (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_arst,  ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, RTLIL::Const arst_ମୂଲ୍ୟ, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAldff (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_aload, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ଅଚଳ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAldffe (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_aload,  ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ଅଚଳ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addSdff (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_srst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, RTLIL::Const srst_ମୂଲ୍ୟ, bool clk_polarity = true, bool srst_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addSdffe (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_srst,  ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, RTLIL::Const srst_ମୂଲ୍ୟ, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addSdffce (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_srst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, RTLIL::Const srst_ମୂଲ୍ୟ, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDlatch (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, bool en_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAdlatch (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_arst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, RTLIL::Const arst_ମୂଲ୍ୟ, bool en_polarity = true, bool arst_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDlatchsr (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addBufGate    (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addNotGate    (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAndGate    (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addNandGate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addOrGate     (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addNorGate    (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addXorGate    (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addXnorGate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAndnotGate (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addOrnotGate  (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addMuxGate    (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_s, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addNmuxGate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_s, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAoi3Gate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_c, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addOai3Gate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_c, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAoi4Gate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_c, ଅଚଳ RTLIL::SigBit &sig_d, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addOai4Gate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_c, ଅଚଳ RTLIL::SigBit &sig_d, ଅଚଳ RTLIL::SigBit &sig_y, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addSrGate     (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr,
			ଅଚଳ RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addFfGate     (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDffGate    (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, bool clk_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDffeGate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDffsrGate  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDffsreGate (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAdffGate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_arst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
			bool arst_ମୂଲ୍ୟ = false, bool clk_polarity = true, bool arst_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAdffeGate  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_arst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
			bool arst_ମୂଲ୍ୟ = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAldffGate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_aload, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
			ଅଚଳ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAldffeGate  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_aload, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
			ଅଚଳ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addSdffGate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_srst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
			bool srst_ମୂଲ୍ୟ = false, bool clk_polarity = true, bool srst_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addSdffeGate  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_srst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
			bool srst_ମୂଲ୍ୟ = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addSdffceGate (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_clk, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_srst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
			bool srst_ମୂଲ୍ୟ = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDlatchGate (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, bool en_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addAdlatchGate(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_arst, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q,
			bool arst_ମୂଲ୍ୟ = false, bool en_polarity = true, bool arst_polarity = true, ଅଚଳ std::string &src = "");
	RTLIL::Cell* addDlatchsrGate  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_en, ଅଚଳ RTLIL::SigSpec &sig_set, ଅଚଳ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ଅଚଳ std::string &src = "");

	RTLIL::Cell* addAnyinit(RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_d, ଅଚଳ RTLIL::SigSpec &sig_q, ଅଚଳ std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, bool is_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, bool is_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Shift  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::SigSpec Add (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, bool is_signed = false, ଅଚଳ std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_b, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ std::string &src = "");

	RTLIL::SigBit BufGate    (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ std::string &src = "");
	RTLIL::SigBit NotGate    (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ std::string &src = "");
	RTLIL::SigBit AndGate    (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ std::string &src = "");
	RTLIL::SigBit NandGate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ std::string &src = "");
	RTLIL::SigBit OrGate     (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ std::string &src = "");
	RTLIL::SigBit NorGate    (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ std::string &src = "");
	RTLIL::SigBit XorGate    (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ std::string &src = "");
	RTLIL::SigBit XnorGate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ std::string &src = "");
	RTLIL::SigBit AndnotGate (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ std::string &src = "");
	RTLIL::SigBit OrnotGate  (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ std::string &src = "");
	RTLIL::SigBit MuxGate    (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_s, ଅଚଳ std::string &src = "");
	RTLIL::SigBit NmuxGate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_s, ଅଚଳ std::string &src = "");
	RTLIL::SigBit Aoi3Gate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_c, ଅଚଳ std::string &src = "");
	RTLIL::SigBit Oai3Gate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_c, ଅଚଳ std::string &src = "");
	RTLIL::SigBit Aoi4Gate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_c, ଅଚଳ RTLIL::SigBit &sig_d, ଅଚଳ std::string &src = "");
	RTLIL::SigBit Oai4Gate   (RTLIL::IdString name, ଅଚଳ RTLIL::SigBit &sig_a, ଅଚଳ RTLIL::SigBit &sig_b, ଅଚଳ RTLIL::SigBit &sig_c, ଅଚଳ RTLIL::SigBit &sig_d, ଅଚଳ std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 1, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 1, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 1, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 1, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ଅଚଳ std::string &src = "");

	RTLIL::SigSpec SetTag          (RTLIL::IdString name, ଅଚଳ std::string &tag, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ RTLIL::SigSpec &sig_c, ଅଚଳ std::string &src = "");
	RTLIL::Cell*   addSetTag       (RTLIL::IdString name, ଅଚଳ std::string &tag, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ RTLIL::SigSpec &sig_c, ଅଚଳ RTLIL::SigSpec &sig_y, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec GetTag          (RTLIL::IdString name, ଅଚଳ std::string &tag, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ std::string &src = "");
	RTLIL::Cell*   addOverwriteTag (RTLIL::IdString name, ଅଚଳ std::string &tag, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ RTLIL::SigSpec &sig_s, ଅଚଳ RTLIL::SigSpec &sig_c, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec OriginalTag     (RTLIL::IdString name, ଅଚଳ std::string &tag, ଅଚଳ RTLIL::SigSpec &sig_a, ଅଚଳ std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ଅଚଳ RTLIL::SigSpec &sig_e, ଅଚଳ std::string &src = "");
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

	std::string to_rtlil_str() ଅଚଳ;
#ifdef YOSYS_ENABLE_PYTHON
	ସ୍ଥିର std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Module*> *get_all_modules(ଶୂନ୍ୟ);
#endif
};

ସଂରଚନା RTLIL::Wire : ସର୍ବସାଧାରଣ RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ { h.eat(hashidx_); ଫେରନ୍ତୁ h; }

ସୁରକ୍ଷିତ:
	// use module->addWire() and module->remove() to create or destroy wires
	friend ସଂରଚନା RTLIL::Module;
	Wire();
	~Wire();

	friend ସଂରଚନା RTLIL::Design;
	friend ସଂରଚନା RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driverPort_;

ସର୍ବସାଧାରଣ:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	ଶୂନ୍ୟ operator=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, start_offset, port_id;
	bool port_ନିବେଶ, port_output, upto, is_signed;

	RTLIL::Design *design() ଅଚଳ { ଫେରନ୍ତୁ module ? module->design : nullptr; }
	TwinePool &twines() ଅଚଳ { ଫେରନ୍ତୁ module->twines(); }

	bool known_driver() ଅଚଳ { ଫେରନ୍ତୁ driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ଅଚଳ    { log_assert(driverCell_); ଫେରନ୍ତୁ driverCell_; };
	RTLIL::IdString driverPort() ଅଚଳ { log_assert(driverCell_); ଫେରନ୍ତୁ driverPort_; };

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା from_hdl_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hdl_index) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା zero_index = hdl_index - start_offset;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା rtlil_index = upto ? width - 1 - zero_index : zero_index;
		ଫେରନ୍ତୁ rtlil_index >= 0 && rtlil_index < width ? rtlil_index : INT_MIN;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା to_hdl_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rtlil_index) {
		ଯଦି (rtlil_index < 0 || rtlil_index >= width)
			ଫେରନ୍ତୁ INT_MIN;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା zero_index = upto ? width - 1 - rtlil_index : rtlil_index;
		ଫେରନ୍ତୁ zero_index + start_offset;
	}

	std::string to_rtlil_str() ଅଚଳ;

#ifdef YOSYS_ENABLE_PYTHON
	ସ୍ଥିର std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Wire*> *get_all_wires(ଶୂନ୍ୟ);
#endif
};

ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା GetSize(RTLIL::Wire *wire) {
	ଫେରନ୍ତୁ wire->width;
}

ସଂରଚନା RTLIL::Memory : ସର୍ବସାଧାରଣ RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ { h.eat(hashidx_); ଫେରନ୍ତୁ h; }

	Memory();
	~Memory();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() ଅଚଳ { ଫେରନ୍ତୁ module ? module->design : nullptr; }
	TwinePool &twines() ଅଚଳ { ଫେରନ୍ତୁ module->twines(); }

	RTLIL::MemoryNameId name;

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, start_offset, size;
#ifdef YOSYS_ENABLE_PYTHON
	ସ୍ଥିର std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Memory*> *get_all_memorys(ଶୂନ୍ୟ);
#endif

	std::string to_rtlil_str() ଅଚଳ;
};

ସଂରଚନା RTLIL::Cell : ସର୍ବସାଧାରଣ RTLIL::AttrObject
{
ବ୍ୟକ୍ତିଗତ:
	ଶୂନ୍ୟ initIndex();

	bool bufnorm_handle_setPort(IdString portname, RTLIL::SigSpec &ସଙ୍କେତ, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
ସର୍ବସାଧାରଣ:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ { h.eat(hashidx_); ଫେରନ୍ତୁ h; }

ସୁରକ୍ଷିତ:
	// use module->addCell() and module->remove() to create or destroy cells
	friend ସଂରଚନା RTLIL::Module;
	Cell();
	~Cell();

ସର୍ବସାଧାରଣ:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	ଶୂନ୍ୟ operator=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() ଅଚଳ { ଫେରନ୍ତୁ module ? module->design : nullptr; }
	TwinePool &twines() ଅଚଳ { ଫେରନ୍ତୁ module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool hasPort(RTLIL::IdString portname) ଅଚଳ;
	ଶୂନ୍ୟ unsetPort(RTLIL::IdString portname);
	ଶୂନ୍ୟ setPort(RTLIL::IdString portname, RTLIL::SigSpec ସଙ୍କେତ);
	template<typename N, YS_NAME_STRING(N)> ଶୂନ୍ୟ setPort(N portname, RTLIL::SigSpec ସଙ୍କେତ)
		{ setPort(module->twines().add(std::move(portname)), std::move(ସଙ୍କେତ)); }
	ଅଚଳ RTLIL::SigSpec &getPort(RTLIL::IdString portname) ଅଚଳ;
	ଅଚଳ dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ଅଚଳ;

	bool known() ଅଚଳ;
	bool ନିବେଶ(RTLIL::IdString portname) ଅଚଳ;
	bool output(RTLIL::IdString portname) ଅଚଳ;
	PortDir port_dir(RTLIL::IdString portname) ଅଚଳ;

	bool hasParam(RTLIL::IdString paramname) ଅଚଳ;
	ଶୂନ୍ୟ unsetParam(RTLIL::IdString paramname);
	ଶୂନ୍ୟ setParam(RTLIL::IdString paramname, RTLIL::Const ମୂଲ୍ୟ);
	ଅଚଳ RTLIL::Const &getParam(RTLIL::IdString paramname) ଅଚଳ;

	template<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ଅଚଳ
		{ ଫେରନ୍ତୁ hasParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ଶୂନ୍ୟ unsetParam(N name)
		{ unsetParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ଶୂନ୍ୟ setParam(N name, RTLIL::Const ମୂଲ୍ୟ)
		{ setParam(module->twines().add(std::move(name)), std::move(ମୂଲ୍ୟ)); }
	template<typename N, YS_NAME_STRING(N)> ଅଚଳ RTLIL::Const &getParam(N name) ଅଚଳ
		{ ଫେରନ୍ତୁ getParam(module->twines().find(std::string_view(name))); }

	ଶୂନ୍ୟ sort();
	ଶୂନ୍ୟ check();
	ଶୂନ୍ୟ fixup_parameters(bool set_a_signed = false, bool set_b_signed = false);

	bool has_keep_attr() ଅଚଳ;

	template<typename T> ଶୂନ୍ୟ rewrite_sigspecs(T &functor);
	template<typename T> ଶୂନ୍ୟ rewrite_sigspecs2(T &functor);

#ifdef YOSYS_ENABLE_PYTHON
	ସ୍ଥିର std::map<unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା, RTLIL::Cell*> *get_all_cells(ଶୂନ୍ୟ);
#endif

	bool has_memid() ଅଚଳ;
	bool is_mem_cell() ଅଚଳ;
	bool is_builtin_ff() ଅଚଳ;

	std::string to_rtlil_str() ଅଚଳ;
};

ସଂରଚନା RTLIL::CaseRule : ସର୍ବସାଧାରଣ RTLIL::AttrObject
{
	std::vector<RTLIL::SigSpec> compare;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::SwitchRule*> switches;

	~CaseRule();

	bool empty() ଅଚଳ;

	template<typename T> ଶୂନ୍ୟ rewrite_sigspecs(T &functor);
	template<typename T> ଶୂନ୍ୟ rewrite_sigspecs2(T &functor);
	RTLIL::CaseRule *clone() ଅଚଳ;
};

ସଂରଚନା RTLIL::SwitchRule : ସର୍ବସାଧାରଣ RTLIL::AttrObject
{
	RTLIL::SigSpec ସଙ୍କେତ;
	std::vector<RTLIL::CaseRule*> cases;

	~SwitchRule();

	bool empty() ଅଚଳ;

	template<typename T> ଶୂନ୍ୟ rewrite_sigspecs(T &functor);
	template<typename T> ଶୂନ୍ୟ rewrite_sigspecs2(T &functor);
	RTLIL::SwitchRule *clone() ଅଚଳ;
};

ସଂରଚନା RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec address;
	RTLIL::SigSpec data;
	RTLIL::SigSpec enable;
	RTLIL::Const priority_mask;
};

ସଂରଚନା RTLIL::SyncRule
{
	RTLIL::SyncType type;
	RTLIL::SigSpec ସଙ୍କେତ;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::MemWriteAction> mem_write_actions;

	template<typename T> ଶୂନ୍ୟ rewrite_sigspecs(T &functor);
	template<typename T> ଶୂନ୍ୟ rewrite_sigspecs2(T &functor);
	RTLIL::SyncRule *clone() ଅଚଳ;
};

ସଂରଚନା RTLIL::Process : ସର୍ବସାଧାରଣ RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ { h.eat(hashidx_); ଫେରନ୍ତୁ h; }

ସୁରକ୍ଷିତ:
	// use module->addProcess() and module->remove() to create or destroy processes
	friend ସଂରଚନା RTLIL::Module;
	Process();
	~Process();

ସର୍ବସାଧାରଣ:
	RTLIL::Module *module;
	RTLIL::CaseRule ମୂଳ_case;
	std::vector<RTLIL::SyncRule*> syncs;

	RTLIL::Design *design() ଅଚଳ { ଫେରନ୍ତୁ module ? module->design : nullptr; }
	TwinePool &twines() ଅଚଳ { ଫେରନ୍ତୁ module->twines(); }

	template<typename T> ଶୂନ୍ୟ rewrite_sigspecs(T &functor);
	template<typename T> ଶୂନ୍ୟ rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ଅଚଳ;

	std::string to_rtlil_str() ଅଚଳ;
};

ଧାଡ଼ିଭିତରେ RTLIL::SigBit::SigBit() : wire(ଖାଲି), data(RTLIL::State::S0) { }
ଧାଡ଼ିଭିତରେ RTLIL::SigBit::SigBit(RTLIL::State bit) : wire(ଖାଲି), data(bit) { }
ଧାଡ଼ିଭିତରେ RTLIL::SigBit::SigBit(bool bit) : wire(ଖାଲି), data(bit ? State::S1 : State::S0) { }
ଧାଡ଼ିଭିତରେ RTLIL::SigBit::SigBit(RTLIL::Wire *wire) : wire(wire), offset(0) { log_assert(wire && wire->width == 1); }
ଧାଡ଼ିଭିତରେ RTLIL::SigBit::SigBit(RTLIL::Wire *wire, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset) : wire(wire), offset(offset) { log_assert(wire != nullptr); }
ଧାଡ଼ିଭିତରେ RTLIL::SigBit::SigBit(ଅଚଳ RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.width == 1); ଯଦି (wire) offset = chunk.offset; ଅନ୍ୟଥା data = chunk.data[0]; }
ଧାଡ଼ିଭିତରେ RTLIL::SigBit::SigBit(ଅଚଳ RTLIL::SigChunk &chunk, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) : wire(chunk.wire) { ଯଦି (wire) offset = chunk.offset + index; ଅନ୍ୟଥା data = chunk.data[index]; }

ଧାଡ଼ିଭିତରେ bool RTLIL::SigBit::operator<(ଅଚଳ RTLIL::SigBit &other) ଅଚଳ {
	ଯଦି (wire == other.wire)
		ଫେରନ୍ତୁ wire ? (offset < other.offset) : (data < other.data);
	ଯଦି (wire != nullptr && other.wire != nullptr)
		ଫେରନ୍ତୁ wire->name < other.wire->name;
	ଫେରନ୍ତୁ (wire != nullptr) < (other.wire != nullptr);
}

ଧାଡ଼ିଭିତରେ bool RTLIL::SigBit::operator==(ଅଚଳ RTLIL::SigBit &other) ଅଚଳ {
	ଫେରନ୍ତୁ (wire == other.wire) && (wire ? (offset == other.offset) : (data == other.data));
}

ଧାଡ଼ିଭିତରେ bool RTLIL::SigBit::operator!=(ଅଚଳ RTLIL::SigBit &other) ଅଚଳ {
	ଫେରନ୍ତୁ (wire != other.wire) || (wire ? (offset != other.offset) : (data != other.data));
}

ଧାଡ଼ିଭିତରେ Hasher RTLIL::SigBit::hash_into(Hasher h) ଅଚଳ {
	ଯଦି (wire) {
		h.eat(offset);
		h.eat(wire->name);
		ଫେରନ୍ତୁ h;
	}
	h.eat(data);
	ଫେରନ୍ତୁ h;
}


ଧାଡ଼ିଭିତରେ Hasher RTLIL::SigBit::hash_ଶୀର୍ଷ() ଅଚଳ {
	Hasher h;
	ଯଦି (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.bits();
		h.force(hashlib::legacy::djb2_add(n, offset));
		ଫେରନ୍ତୁ h;
	}
	h.force(data);
	ଫେରନ୍ତୁ h;
}

ଧାଡ଼ିଭିତରେ RTLIL::SigBit &RTLIL::SigSpecIterator::operator*() ଅଚଳ {
	ଫେରନ୍ତୁ (*sig_p)[index];
}

ଧାଡ଼ିଭିତରେ ଅଚଳ RTLIL::SigBit &RTLIL::SigSpecConstIterator::operator*() {
	bit = (*sig_p)[index];
	ଫେରନ୍ତୁ bit;
}

ଧାଡ଼ିଭିତରେ RTLIL::SigBit::SigBit(ଅଚଳ RTLIL::SigSpec &sig) {
	log_assert(sig.size() == 1);
	auto it = sig.chunks().begin();
	*this = SigBit(*it);
}

template<typename T>
ଶୂନ୍ୟ RTLIL::Module::rewrite_sigspecs(T &functor)
{
	ପାଇଁ (auto &it : cells_)
		it.second->rewrite_sigspecs(functor);
	ପାଇଁ (auto &it : processes)
		it.second->rewrite_sigspecs(functor);
	ପାଇଁ (auto &it : connections_) {
		functor(it.first);
		functor(it.second);
	}
}

template<typename T>
ଶୂନ୍ୟ RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	ପାଇଁ (auto &it : cells_)
		it.second->rewrite_sigspecs2(functor);
	ପାଇଁ (auto &it : processes)
		it.second->rewrite_sigspecs2(functor);
	ପାଇଁ (auto &it : connections_) {
		functor(it.first, it.second);
	}
}

template<typename T>
ଶୂନ୍ୟ RTLIL::Cell::rewrite_sigspecs(T &functor) {
	ପାଇଁ (auto &it : connections_)
		functor(it.second);
}

template<typename T>
ଶୂନ୍ୟ RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	ପାଇଁ (auto &it : connections_)
		functor(it.second);
}

template<typename T>
ଶୂନ୍ୟ RTLIL::CaseRule::rewrite_sigspecs(T &functor) {
	ପାଇଁ (auto &it : compare)
		functor(it);
	ପାଇଁ (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	ପାଇଁ (auto it : switches)
		it->rewrite_sigspecs(functor);
}

template<typename T>
ଶୂନ୍ୟ RTLIL::CaseRule::rewrite_sigspecs2(T &functor) {
	ପାଇଁ (auto &it : compare)
		functor(it);
	ପାଇଁ (auto &it : actions) {
		functor(it.first, it.second);
	}
	ପାଇଁ (auto it : switches)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
ଶୂନ୍ୟ RTLIL::SwitchRule::rewrite_sigspecs(T &functor)
{
	functor(ସଙ୍କେତ);
	ପାଇଁ (auto it : cases)
		it->rewrite_sigspecs(functor);
}

template<typename T>
ଶୂନ୍ୟ RTLIL::SwitchRule::rewrite_sigspecs2(T &functor)
{
	functor(ସଙ୍କେତ);
	ପାଇଁ (auto it : cases)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
ଶୂନ୍ୟ RTLIL::SyncRule::rewrite_sigspecs(T &functor)
{
	functor(ସଙ୍କେତ);
	ପାଇଁ (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	ପାଇଁ (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
ଶୂନ୍ୟ RTLIL::SyncRule::rewrite_sigspecs2(T &functor)
{
	functor(ସଙ୍କେତ);
	ପାଇଁ (auto &it : actions) {
		functor(it.first, it.second);
	}
	ପାଇଁ (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
ଶୂନ୍ୟ RTLIL::Process::rewrite_sigspecs(T &functor)
{
	ମୂଳ_case.rewrite_sigspecs(functor);
	ପାଇଁ (auto it : syncs)
		it->rewrite_sigspecs(functor);
}

template<typename T>
ଶୂନ୍ୟ RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	ମୂଳ_case.rewrite_sigspecs2(functor);
	ପାଇଁ (auto it : syncs)
		it->rewrite_sigspecs2(functor);
}

#ଅନ୍ତର୍ଭାଵ "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_END

#endif
