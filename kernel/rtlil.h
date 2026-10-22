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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ RTLIL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RTLIL_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/twine.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <deque>
#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>
#ਅਨ੍ਤਰ੍ਭਾਵ <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	ਗਿਣਤੀ State : ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	ਗਿਣਤੀ SyncType : ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ {
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
	ਗਿਣਤੀ ConstFlags : ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	ਗਿਣਤੀ SelectPartials : ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ {
		SELECT_ALL = 0,          // include partial modules
		SELECT_ਸਾਰਾ_ONLY = 1,   // ignore partial modules
		SELECT_ਸਾਰਾ_WARN = 2,   // call log_warning on partial module
		SELECT_ਸਾਰਾ_ERR = 3,    // call log_error on partial module
		SELECT_ਸਾਰਾ_CMDERR = 4  // call log_cmd_error on partial module
	};

	ਗਿਣਤੀ SelectBoxes : ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ {
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

	ਗਿਣਤੀ ਪੋਰਟ_Dir : ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ  {
		PD_UNKNOWN = 0,
		PD_ਨਿਵੇਸ਼ = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr ਪੂਰਨ ਚੌੜਾਈ_ਹੱਦਬੰਦੀ = 1 << 30;

	ਬਣਤਰ Const;
	ਬਣਤਰ AttrObject;
	ਬਣਤਰ SelectedMember;
	ਬਣਤਰ Selection;
	ਬਣਤਰ Monitor;
	ਬਣਤਰ Design;
	ਬਣਤਰ Module;
	ਬਣਤਰ Wire;
	ਬਣਤਰ ਸਮ੍ਰਿਤੀ;
	ਬਣਤਰ Cell;
	ਬਣਤਰ SigChunk;
	ਬਣਤਰ Sig_ਦੋਆਧਾਰੀ_ਅੰਕ;
	ਬਣਤਰ SigSpecIterator;
	ਬਣਤਰ SigSpecConstIterator;
	ਬਣਤਰ SigSpec;
	ਬਣਤਰ Case_ਨਿਯਮ;
	ਬਣਤਰ Switch_ਨਿਯਮ;
	ਬਣਤਰ MemWriteAction;
	ਬਣਤਰ Sync_ਨਿਯਮ;
	ਬਣਤਰ Process;

	ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::pair<SigSpec, SigSpec> SigSig;
};

ਬਣਤਰ SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_ਅਗੇਤਰ ID; }
namespace RTLIL { using YOSYS_NAMESPACE_ਅਗੇਤਰ IdString; }

namespace RTLIL {
	ਖਾਲੀ copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			ਅਚਲ dict<IdString, RTLIL::Const> &src,
			ਅਚਲ RTLIL::Design *src_design, RTLIL::Design *dst_design);

	ਬਾਹਰੀ dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	ਅਚਲ pool<IdString> &builtin_ff_cell_types();

	ਸਥਿਰ ਸਤਰਅੰਦਰ std::string escape_id(ਅਚਲ std::string &str) {
		ਜੇ (str.ਮਾਪ() > 0 && str[0] != '\\' && str[0] != '$')
			ਵਾਪਸ "\\" + str;
		ਵਾਪਸ str;
	}

	ਸਥਿਰ ਸਤਰਅੰਦਰ std::string unescape_id(ਅਚਲ std::string &str) {
		ਜੇ (str.ਮਾਪ() < 2)
			ਵਾਪਸ str;
		ਜੇ (str[0] != '\\')
			ਵਾਪਸ str;
		ਜੇ (str[1] == '$' || str[1] == '\\')
			ਵਾਪਸ str;
		ਜੇ (str[1] >= '0' && str[1] <= '9')
			ਵਾਪਸ str;
		ਵਾਪਸ str.substr(1);
	}

	ਸਾਂਚਾ <typename T> using ਛਾਂਟੋ_by_name_id = IdString::compare_ptr_by_name<T>;

	ਸਾਂਚਾ <typename T> ਬਣਤਰ ਛਾਂਟੋ_by_name_str {
		bool ਸੰਚਾਲਕ()(T *a, T *b) ਅਚਲ {
			ਵਾਪਸ a->name.lt_by_name(b->name);
		}
	};

	ਬਣਤਰ ਛਾਂਟੋ_by_id_str {
		ਅਚਲ TwinePool& pool;
		explicit ਛਾਂਟੋ_by_id_str(ਅਚਲ TwinePool& pool) : pool(pool) {}
		bool ਸੰਚਾਲਕ()(RTLIL::IdString a, RTLIL::IdString b) ਅਚਲ {
			ਵਾਪਸ pool.compare_by_name(a, b) < 0;
		}
	};

	ਸਥਿਰ ਸਤਰਅੰਦਰ std::string encode_filename(ਅਚਲ std::string &filename)
	{
		std::stringstream val;
		ਜੇ (!std::any_ਦਾ(filename.begin(), filename.ਸਿਰਾ(), [](ਅੱਖਰ c) {
			ਵਾਪਸ static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>(c) < 33 || static_cast<ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ>(c) > 126;
		})) ਵਾਪਸ filename;
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਅਚਲ c : filename) {
			ਜੇ (c < 33 || c > 126)
				val << stringf("$%02x", c);
			ਨਹੀਂਤਾਂ
				val << c;
		}
		ਵਾਪਸ val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_and         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_or          (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_xor         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_xnor        (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);

	RTLIL::Const const_reduce_and  (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_reduce_or   (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_reduce_xor  (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_reduce_xnor (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_reduce_bool (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);

	RTLIL::Const const_logic_not   (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_logic_and   (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_logic_or    (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);

	RTLIL::Const const_shl         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_shr         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_sshl        (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_sshr        (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_ਖਿਸਕਾਓ       (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_shiftx      (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);

	RTLIL::Const const_lt          (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_le          (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_eq          (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_ne          (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_eqx         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_nex         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_ge          (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_gt          (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);

	RTLIL::Const const_ਜੋੜੋ         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_sub         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_mul         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_div         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_divfloor    (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_modfloor    (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_mod         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_pow         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);

	RTLIL::Const const_pos         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_buf         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);
	RTLIL::Const const_neg         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len);

	RTLIL::Const const_mux         (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਅਚਲ RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਅਚਲ RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਅਚਲ RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	ਸਾਂਚਾ<typename T, typename ਕੁੰਜੀ_2 = IdString>
	ਬਣਤਰ ObjIterator {
		using iterator_category = std::forward_iterator_ਟੈਗ;
		using ਮੁੱਲ_type = T;
		using difference_type = ptrdiff_t;
		using ਸੂਚਕ = T*;
		using reference = T&;
		typename dict<ਕੁੰਜੀ_2, T>::iterator it;
		dict<ਕੁੰਜੀ_2, T> *list_p;
		ਪੂਰਨ *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, ਪੂਰਨ *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			ਜੇ (list_p->ਖਾਲੀ_ਥਾਵਾਂ()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} ਨਹੀਂਤਾਂ {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(ਅਚਲ RTLIL::ObjIterator<T, ਕੁੰਜੀ_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			ਜੇ (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &ਸੰਚਾਲਕ=(ਅਚਲ RTLIL::ObjIterator<T, ਕੁੰਜੀ_2> &other) {
			ਜੇ (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			ਜੇ (refcount_p)
				(*refcount_p)++;
			ਵਾਪਸ *this;
		}

		~ObjIterator() {
			ਜੇ (refcount_p)
				(*refcount_p)--;
		}

		ਸਤਰਅੰਦਰ T ਸੰਚਾਲਕ*() ਅਚਲ {
			log_assert(list_p != nullptr);
			ਵਾਪਸ it->ਦੂਜਾ;
		}

		ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ!=(ਅਚਲ RTLIL::ObjIterator<T, ਕੁੰਜੀ_2> &other) ਅਚਲ {
			ਜੇ (list_p == nullptr || other.list_p == nullptr)
				ਵਾਪਸ list_p != other.list_p;
			ਵਾਪਸ it != other.it;
		}


		ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ==(ਅਚਲ RTLIL::ObjIterator<T, ਕੁੰਜੀ_2> &other) ਅਚਲ {
			ਵਾਪਸ !(*this != other);
		}

		ਸਤਰਅੰਦਰ ObjIterator<T, ਕੁੰਜੀ_2>& ਸੰਚਾਲਕ++() {
			log_assert(list_p != nullptr);
			ਜੇ (++it == list_p->ਸਿਰਾ()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			ਵਾਪਸ *this;
		}

		ਸਤਰਅੰਦਰ ObjIterator<T, ਕੁੰਜੀ_2>& ਸੰਚਾਲਕ+=(ਪੂਰਨ amt) {
			log_assert(list_p != nullptr);
			it += amt;
			ਜੇ (it == list_p->ਸਿਰਾ()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			ਵਾਪਸ *this;
		}

		ਸਤਰਅੰਦਰ ObjIterator<T, ਕੁੰਜੀ_2> ਸੰਚਾਲਕ+(ਪੂਰਨ amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, ਕੁੰਜੀ_2> new_obj(*this);
			new_obj.it += amt;
			ਜੇ (new_obj.it == list_p->ਸਿਰਾ()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			ਵਾਪਸ new_obj;
		}

		ਸਤਰਅੰਦਰ ਅਚਲ ObjIterator<T, ਕੁੰਜੀ_2> ਸੰਚਾਲਕ++(ਪੂਰਨ) {
			ObjIterator<T, ਕੁੰਜੀ_2> ਨਤੀਜਾ(*this);
			++(*this);
			ਵਾਪਸ ਨਤੀਜਾ;
		}
	};

	ਸਾਂਚਾ<typename T, typename ਕੁੰਜੀ_2 = IdString>
	ਬਣਤਰ ObjRange
	{
		dict<ਕੁੰਜੀ_2, T> *list_p;
		ਪੂਰਨ *refcount_p;

		ObjRange(decltype(list_p) list_p, ਪੂਰਨ *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, ਕੁੰਜੀ_2> begin() { ਵਾਪਸ RTLIL::ObjIterator<T, ਕੁੰਜੀ_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, ਕੁੰਜੀ_2> ਸਿਰਾ() { ਵਾਪਸ RTLIL::ObjIterator<T, ਕੁੰਜੀ_2>(); }

		ਮਾਪ_t ਮਾਪ() ਅਚਲ {
			ਵਾਪਸ list_p->ਮਾਪ();
		}

		ਸੰਚਾਲਕ pool<T>() ਅਚਲ {
			pool<T> ਨਤੀਜਾ;
			ਲਈ (ਸਵੈਚਾਲਿਤ &it : *list_p)
				ਨਤੀਜਾ.ਪਾਓ(it.ਦੂਜਾ);
			ਵਾਪਸ ਨਤੀਜਾ;
		}

		ਸੰਚਾਲਕ std::ਸਦਿਸ਼<T>() ਅਚਲ {
			std::ਸਦਿਸ਼<T> ਨਤੀਜਾ;
			ਨਤੀਜਾ.reserve(list_p->ਮਾਪ());
			ਲਈ (ਸਵੈਚਾਲਿਤ &it : *list_p)
				ਨਤੀਜਾ.ਧੱਕੋ_ਪਿੱਠ(it.ਦੂਜਾ);
			ਵਾਪਸ ਨਤੀਜਾ;
		}

		pool<T> ਮੰਜ਼ਿਲ_pool() ਅਚਲ { ਵਾਪਸ *this; }
		std::ਸਦਿਸ਼<T> ਮੰਜ਼ਿਲ_ਸਦਿਸ਼() ਅਚਲ { ਵਾਪਸ *this; }
	};
};

ਬਣਤਰ RTLIL::Const
{
	ਛੋਟਾ ਪੂਰਨ flags;
ਨਿਜੀ:
	ਮਿੱਤਰ class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::ਸਦਿਸ਼<RTLIL::State>;
	ਗਿਣਤੀ class backing_ਟੈਗ: bool { ਬਿੱਟ, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_ਟੈਗ ਟੈਗ;
	ਸੰਘ {
		bitvectype ਬਿੱਟ_2;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_ਬਿੱਟ() ਅਚਲ { ਵਾਪਸ ਟੈਗ == backing_ਟੈਗ::ਬਿੱਟ; }
	bool is_str() ਅਚਲ { ਵਾਪਸ ਟੈਗ == backing_ਟੈਗ::string; }

	bitvectype* get_if_ਬਿੱਟ() { ਵਾਪਸ is_ਬਿੱਟ() ? &ਬਿੱਟ_2 : ਨਿਰਾ; }
	std::string* get_if_str() { ਵਾਪਸ is_str() ? &str_ : ਨਿਰਾ; }
	ਅਚਲ bitvectype* get_if_ਬਿੱਟ() ਅਚਲ { ਵਾਪਸ is_ਬਿੱਟ() ? &ਬਿੱਟ_2 : ਨਿਰਾ; }
	ਅਚਲ std::string* get_if_str() ਅਚਲ { ਵਾਪਸ is_str() ? &str_ : ਨਿਰਾ; }

	bitvectype& get_ਬਿੱਟ();
	std::string& get_str();
	ਅਚਲ bitvectype& get_ਬਿੱਟ() ਅਚਲ;
	ਅਚਲ std::string& get_str() ਅਚਲ;
	std::ਸਦਿਸ਼<RTLIL::State>& ਬਿੱਟ_ਅੰਦਰੂਨੀ();
	ਖਾਲੀ bitvectorize_ਅੰਦਰੂਨੀ();

ਜਨਤਕ:
	Const() : flags(RTLIL::CONST_FLAG_NONE), ਟੈਗ(backing_ਟੈਗ::ਬਿੱਟ), ਬਿੱਟ_2(std::ਸਦਿਸ਼<RTLIL::State>()) {}
	Const(std::string str);
	Const(ਲੰਮਾ ਲੰਮਾ val); // default width is 32
	Const(ਲੰਮਾ ਲੰਮਾ val, ਪੂਰਨ ਚੌੜਾਈ);
	Const(RTLIL::State ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ ਚੌੜਾਈ = 1);
	Const(std::ਸਦਿਸ਼<RTLIL::State> ਬਿੱਟ) : flags(RTLIL::CONST_FLAG_NONE), ਟੈਗ(backing_ਟੈਗ::ਬਿੱਟ), ਬਿੱਟ_2(std::move(ਬਿੱਟ)) {}
	Const(ਅਚਲ std::ਸਦਿਸ਼<bool> &ਬਿੱਟ);
	Const(ਅਚਲ RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &ਸੰਚਾਲਕ =(ਅਚਲ RTLIL::Const &other);
	~Const();

	ਬਣਤਰ Builder
	{
		Builder() {}
		Builder(ਪੂਰਨ expected_ਚੌੜਾਈ) { ਬਿੱਟ.reserve(expected_ਚੌੜਾਈ); }
		ਖਾਲੀ ਧੱਕੋ_ਪਿੱਠ(RTLIL::State b) { ਬਿੱਟ.ਧੱਕੋ_ਪਿੱਠ(b); }
		ਪੂਰਨ ਮਾਪ() ਅਚਲ { ਵਾਪਸ static_cast<ਪੂਰਨ>(ਬਿੱਟ.ਮਾਪ()); }
		Const build() { ਵਾਪਸ Const(std::move(ਬਿੱਟ)); }
	ਨਿਜੀ:
		std::ਸਦਿਸ਼<RTLIL::State> ਬਿੱਟ;
	};

	bool ਸੰਚਾਲਕ <(ਅਚਲ RTLIL::Const &other) ਅਚਲ;
	bool ਸੰਚਾਲਕ ==(ਅਚਲ RTLIL::Const &other) ਅਚਲ;
	bool ਸੰਚਾਲਕ !=(ਅਚਲ RTLIL::Const &other) ਅਚਲ;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::ਸਦਿਸ਼<RTLIL::State>& ਬਿੱਟ() { ਵਾਪਸ ਬਿੱਟ_ਅੰਦਰੂਨੀ(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	ਖਾਲੀ bitvectorize() ਅਚਲ { const_cast<Const*>(this)->bitvectorize_ਅੰਦਰੂਨੀ(); }

	bool as_bool() ਅਚਲ;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	ਪੂਰਨ as_int(bool is_signed = false) ਅਚਲ;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_ਮੰਜ਼ਿਲ_int(bool is_signed = false) ਅਚਲ;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<ਪੂਰਨ> try_as_int(bool is_signed = false) ਅਚਲ;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	ਪੂਰਨ as_int_saturating(bool is_signed = false) ਅਚਲ;

	ਖਾਲੀ ਟੈਗ_bare_integer_const(ਅਚਲ std::string &ਮੁੱਲ);

	std::string as_string(ਅਚਲ ਅੱਖਰ* any = "-") ਅਚਲ;
	ਸਥਿਰ Const from_string(ਅਚਲ std::string &str);
	std::ਸਦਿਸ਼<RTLIL::State> ਮੰਜ਼ਿਲ_ਬਿੱਟ() ਅਚਲ;

	std::string decode_string() ਅਚਲ;
	ਪੂਰਨ ਮਾਪ() ਅਚਲ;
	bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ;

	ਖਾਲੀ append(ਅਚਲ RTLIL::Const &other);
	ਖਾਲੀ ਸੈੱਟ(ਪੂਰਨ i, RTLIL::State state) {
		ਬਿੱਟ_ਅੰਦਰੂਨੀ()[i] = state;
	}
	ਖਾਲੀ resize(ਪੂਰਨ ਮਾਪ, RTLIL::State fill) {
    log_assert(ਮਾਪ >= 0 && ਮਾਪ < RTLIL::ਚੌੜਾਈ_ਹੱਦਬੰਦੀ);
		ਬਿੱਟ_ਅੰਦਰੂਨੀ().resize(ਮਾਪ, fill);
	}

	class const_iterator {
	ਨਿਜੀ:
		ਅਚਲ Const* ਮਾਪਾ;
		ਮਾਪ_t idx;

	ਜਨਤਕ:
		using iterator_category = std::bidirectional_iterator_ਟੈਗ;
		using ਮੁੱਲ_type = State;
		using difference_type = std::ptrdiff_t;
		using ਸੂਚਕ = ਅਚਲ State*;
		using reference = ਅਚਲ State&;

		const_iterator(ਅਚਲ Const& c, ਮਾਪ_t i) : ਮਾਪਾ(&c), idx(i) {}

		State ਸੰਚਾਲਕ*() ਅਚਲ;

		const_iterator& ਸੰਚਾਲਕ++() { ++idx; ਵਾਪਸ *this; }
		const_iterator& ਸੰਚਾਲਕ--() { --idx; ਵਾਪਸ *this; }
		const_iterator ਸੰਚਾਲਕ++(ਪੂਰਨ) { const_iterator ਨਤੀਜਾ(*this); ++idx; ਵਾਪਸ ਨਤੀਜਾ; }
		const_iterator ਸੰਚਾਲਕ--(ਪੂਰਨ) { const_iterator ਨਤੀਜਾ(*this); --idx; ਵਾਪਸ ਨਤੀਜਾ; }
		const_iterator& ਸੰਚਾਲਕ+=(ਪੂਰਨ i) { idx += i; ਵਾਪਸ *this; }

		const_iterator ਸੰਚਾਲਕ+(ਪੂਰਨ ਜੋੜੋ) {
			ਵਾਪਸ const_iterator(*ਮਾਪਾ, idx + ਜੋੜੋ);
		}
		const_iterator ਸੰਚਾਲਕ-(ਪੂਰਨ sub) {
			ਵਾਪਸ const_iterator(*ਮਾਪਾ, idx - sub);
		}
		ਪੂਰਨ ਸੰਚਾਲਕ-(ਅਚਲ const_iterator& other) {
			ਵਾਪਸ idx - other.idx;
		}

		bool ਸੰਚਾਲਕ==(ਅਚਲ const_iterator& other) ਅਚਲ {
			ਵਾਪਸ idx == other.idx;
		}

		bool ਸੰਚਾਲਕ!=(ਅਚਲ const_iterator& other) ਅਚਲ {
			ਵਾਪਸ !(*this == other);
		}
	};

	class iterator {
	ਨਿਜੀ:
		Const* ਮਾਪਾ;
		ਮਾਪ_t idx;

	ਜਨਤਕ:
		class proxy {
		ਨਿਜੀ:
			Const* ਮਾਪਾ;
			ਮਾਪ_t idx;
		ਜਨਤਕ:
			proxy(Const* ਮਾਪਾ, ਮਾਪ_t idx) : ਮਾਪਾ(ਮਾਪਾ), idx(idx) {}
			ਸੰਚਾਲਕ State() ਅਚਲ { ਵਾਪਸ (*ਮਾਪਾ)[idx]; }
			proxy& ਸੰਚਾਲਕ=(State s) { ਮਾਪਾ->ਸੈੱਟ(idx, s); ਵਾਪਸ *this; }
			proxy& ਸੰਚਾਲਕ=(ਅਚਲ proxy& other) { ਮਾਪਾ->ਸੈੱਟ(idx, (*other.ਮਾਪਾ)[other.idx]); ਵਾਪਸ *this; }
		};

		using iterator_category = std::bidirectional_iterator_ਟੈਗ;
		using ਮੁੱਲ_type = State;
		using difference_type = std::ptrdiff_t;
		using ਸੂਚਕ = proxy*;
		using reference = proxy;

		iterator(Const& c, ਮਾਪ_t i) : ਮਾਪਾ(&c), idx(i) {}

		proxy ਸੰਚਾਲਕ*() ਅਚਲ { ਵਾਪਸ proxy(ਮਾਪਾ, idx); }
		iterator& ਸੰਚਾਲਕ++() { ++idx; ਵਾਪਸ *this; }
		iterator& ਸੰਚਾਲਕ--() { --idx; ਵਾਪਸ *this; }
		iterator ਸੰਚਾਲਕ++(ਪੂਰਨ) { iterator ਨਤੀਜਾ(*this); ++idx; ਵਾਪਸ ਨਤੀਜਾ; }
		iterator ਸੰਚਾਲਕ--(ਪੂਰਨ) { iterator ਨਤੀਜਾ(*this); --idx; ਵਾਪਸ ਨਤੀਜਾ; }
		iterator& ਸੰਚਾਲਕ+=(ਪੂਰਨ i) { idx += i; ਵਾਪਸ *this; }

		iterator ਸੰਚਾਲਕ+(ਪੂਰਨ ਜੋੜੋ) {
			ਵਾਪਸ iterator(*ਮਾਪਾ, idx + ਜੋੜੋ);
		}
		iterator ਸੰਚਾਲਕ-(ਪੂਰਨ sub) {
			ਵਾਪਸ iterator(*ਮਾਪਾ, idx - sub);
		}
		ਪੂਰਨ ਸੰਚਾਲਕ-(ਅਚਲ iterator& other) {
			ਵਾਪਸ idx - other.idx;
		}

		bool ਸੰਚਾਲਕ==(ਅਚਲ iterator& other) ਅਚਲ {
			ਵਾਪਸ idx == other.idx;
		}

		bool ਸੰਚਾਲਕ!=(ਅਚਲ iterator& other) ਅਚਲ {
			ਵਾਪਸ !(*this == other);
		}
	};

	const_iterator begin() ਅਚਲ {
		ਵਾਪਸ const_iterator(*this, 0);
	}
	const_iterator ਸਿਰਾ() ਅਚਲ {
		ਵਾਪਸ const_iterator(*this, ਮਾਪ());
	}
	iterator begin() {
		ਵਾਪਸ iterator(*this, 0);
	}
	iterator ਸਿਰਾ() {
		ਵਾਪਸ iterator(*this, ਮਾਪ());
	}
	State ਪਿੱਠ() ਅਚਲ {
		ਵਾਪਸ *(ਸਿਰਾ() - 1);
	}
	State front() ਅਚਲ {
		ਵਾਪਸ *begin();
	}
	State at(ਮਾਪ_t i) ਅਚਲ {
		ਵਾਪਸ *const_iterator(*this, i);
	}
	State ਸੰਚਾਲਕ[](ਮਾਪ_t i) ਅਚਲ {
		ਵਾਪਸ *const_iterator(*this, i);
	}

	bool is_fully_zero() ਅਚਲ;
	bool is_fully_ones() ਅਚਲ;
	bool is_fully_def() ਅਚਲ;
	bool is_fully_undef() ਅਚਲ;
	bool is_fully_undef_x_only() ਅਚਲ;
	bool is_onehot(ਪੂਰਨ *pos = nullptr) ਅਚਲ;

	RTLIL::Const ਕੱਢੋ_2(ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ len = 1, RTLIL::State padding = RTLIL::State::S0) ਅਚਲ;

	// find the MSB without redundant leading bits
	ਪੂਰਨ get_min_ਮਾਪ(bool is_signed) ਅਚਲ;

	// compress representation to the minimum required bits
	ਖਾਲੀ compress(bool is_signed = false);

	std::optional<ਪੂਰਨ> as_int_compress(bool is_signed) ਅਚਲ;

	ਖਾਲੀ extu(ਪੂਰਨ ਚੌੜਾਈ) {
		resize(ਚੌੜਾਈ, RTLIL::State::S0);
	}

	ਖਾਲੀ exts(ਪੂਰਨ ਚੌੜਾਈ) {
		resize(ਚੌੜਾਈ, ਖਾਲੀ_ਥਾਵਾਂ() ? RTLIL::State::Sx : ਪਿੱਠ());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;
};

ਬਣਤਰ RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> ਗੁਣ;

	bool has_attribute(RTLIL::IdString id) ਅਚਲ;

	ਖਾਲੀ ਸੈੱਟ_bool_attribute(RTLIL::IdString id, bool ਮੁੱਲ=true);
	bool get_bool_attribute(RTLIL::IdString id) ਅਚਲ;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ਅਚਲ {
		ਵਾਪਸ get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	ਖਾਲੀ ਸੈੱਟ_string_attribute(RTLIL::IdString id, string ਮੁੱਲ);
	string get_string_attribute(RTLIL::IdString id) ਅਚਲ;

	ਖਾਲੀ ਸੈੱਟ_strpool_attribute(RTLIL::IdString id, ਅਚਲ pool<string> &ਅੰਕੜੇ);
	ਖਾਲੀ ਜੋੜੋ_strpool_attribute(RTLIL::IdString id, ਅਚਲ pool<string> &ਅੰਕੜੇ);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ਅਚਲ;

	ਖਾਲੀ ਸੈੱਟ_src_attribute(ਅਚਲ std::string &src) {
		ਸੈੱਟ_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ਅਚਲ {
		ਵਾਪਸ get_string_attribute(ID::src);
	}

	ਖਾਲੀ ਸੈੱਟ_hdlname_attribute(ਅਚਲ ਸਦਿਸ਼<string> &hierarchy);
	ਸਦਿਸ਼<string> get_hdlname_attribute() ਅਚਲ;

	ਖਾਲੀ ਸੈੱਟ_intvec_attribute(RTLIL::IdString id, ਅਚਲ ਸਦਿਸ਼<ਪੂਰਨ> &ਅੰਕੜੇ);
	ਸਦਿਸ਼<ਪੂਰਨ> get_intvec_attribute(RTLIL::IdString id) ਅਚਲ;
};

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/rtlil_twine_compat.h"

ਬਣਤਰ RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

ਬਣਤਰ RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::ਸਦਿਸ਼<RTLIL::State> ਅੰਕੜੇ; // only used if wire == NULL, LSB at index 0
	ਪੂਰਨ ਚੌੜਾਈ, ਵਿੱਥ;

	SigChunk() : wire(nullptr), ਚੌੜਾਈ(0), ਵਿੱਥ(0) {}
	SigChunk(ਅਚਲ RTLIL::Const &ਮੁੱਲ) : wire(nullptr), ਅੰਕੜੇ(ਮੁੱਲ.ਮੰਜ਼ਿਲ_ਬਿੱਟ()), ਚੌੜਾਈ(Get_ਮਾਪ(ਅੰਕੜੇ)), ਵਿੱਥ(0) {}
	SigChunk(RTLIL::Const &&ਮੁੱਲ) : wire(nullptr), ਅੰਕੜੇ(ਮੁੱਲ.ਮੰਜ਼ਿਲ_ਬਿੱਟ()), ਚੌੜਾਈ(Get_ਮਾਪ(ਅੰਕੜੇ)), ਵਿੱਥ(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), ਚੌੜਾਈ(Get_ਮਾਪ(wire)), ਵਿੱਥ(0) {}
	SigChunk(RTLIL::Wire *wire, ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਚੌੜਾਈ = 1) : wire(wire), ਚੌੜਾਈ(ਚੌੜਾਈ), ਵਿੱਥ(ਵਿੱਥ) {}
	SigChunk(ਅਚਲ std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(ਪੂਰਨ val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(ਪੂਰਨ val, ਪੂਰਨ ਚੌੜਾਈ) : SigChunk(RTLIL::Const(val, ਚੌੜਾਈ)) {}
	SigChunk(RTLIL::State ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ ਚੌੜਾਈ = 1) : SigChunk(RTLIL::Const(ਦੋਆਧਾਰੀ_ਅੰਕ, ਚੌੜਾਈ)) {}
	SigChunk(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ);

	RTLIL::SigChunk ਕੱਢੋ_2(ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਲੰਮਾਈ) ਅਚਲ;
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਸੰਚਾਲਕ[](ਪੂਰਨ ਵਿੱਥ) ਅਚਲ;
	ਸਤਰਅੰਦਰ ਪੂਰਨ ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਚੌੜਾਈ; }
	ਸਤਰਅੰਦਰ bool is_wire() ਅਚਲ { ਵਾਪਸ wire != ਨਿਰਾ; }

	bool ਸੰਚਾਲਕ <(ਅਚਲ RTLIL::SigChunk &other) ਅਚਲ;
	bool ਸੰਚਾਲਕ ==(ਅਚਲ RTLIL::SigChunk &other) ਅਚਲ;
	bool ਸੰਚਾਲਕ !=(ਅਚਲ RTLIL::SigChunk &other) ਅਚਲ;
};

ਬਣਤਰ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ
{
	RTLIL::Wire *wire;
	ਸੰਘ {
		RTLIL::State ਅੰਕੜੇ; // used if wire == NULL
		ਪੂਰਨ ਵਿੱਥ;        // used if wire != NULL
	};

	Sig_ਦੋਆਧਾਰੀ_ਅੰਕ();
	Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(RTLIL::State ਦੋਆਧਾਰੀ_ਅੰਕ);
	explicit Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(bool ਦੋਆਧਾਰੀ_ਅੰਕ);
	Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(RTLIL::Wire *wire);
	Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(RTLIL::Wire *wire, ਪੂਰਨ ਵਿੱਥ);
	Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ RTLIL::SigChunk &chunk);
	Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ RTLIL::SigChunk &chunk, ਪੂਰਨ ਸੂਚਕਾਂਕ);
	Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ RTLIL::SigSpec &sig);
	Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sigbit) = ਮੂਲ;
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ =(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &other) = ਮੂਲ;

	ਸਤਰਅੰਦਰ bool is_wire() ਅਚਲ { ਵਾਪਸ wire != ਨਿਰਾ; }

	bool ਸੰਚਾਲਕ <(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &other) ਅਚਲ;
	bool ਸੰਚਾਲਕ ==(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &other) ਅਚਲ;
	bool ਸੰਚਾਲਕ !=(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &other) ਅਚਲ;
	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ;
	[[nodiscard]] Hasher hash_ਸਿਖਰ() ਅਚਲ;
};

namespace hashlib {
	ਸਾਂਚਾ <>
	ਬਣਤਰ hash_ops<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> {
		ਸਥਿਰ ਸਤਰਅੰਦਰ bool cmp(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &b) {
			ਵਾਪਸ a == b;
		}
		[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ sb) {
			ਵਾਪਸ sb.hash_ਸਿਖਰ();
		}
		[[nodiscard]] ਸਥਿਰ ਸਤਰਅੰਦਰ Hasher hash_into(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ sb, Hasher h) {
			ਵਾਪਸ sb.hash_into(h);
		}
	};
};

ਬਣਤਰ RTLIL::SigSpecIterator
{
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::ਨਿਵੇਸ਼_iterator_ਟੈਗ iterator_category;
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਮੁੱਲ_type;
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ ptrdiff_t difference_type;
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ* ਸੂਚਕ;
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ& reference;

	RTLIL::SigSpec *sig_p;
	ਪੂਰਨ ਸੂਚਕਾਂਕ;

	ਸਤਰਅੰਦਰ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ*() ਅਚਲ;
	ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ!=(ਅਚਲ RTLIL::SigSpecIterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ != other.ਸੂਚਕਾਂਕ; }
	ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ==(ਅਚਲ RTLIL::SigSpecIterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ == other.ਸੂਚਕਾਂਕ; }
	ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੰਚਾਲਕ++() { ਸੂਚਕਾਂਕ++; }
};

ਬਣਤਰ RTLIL::SigSpecConstIterator
{
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ std::ਨਿਵੇਸ਼_iterator_ਟੈਗ iterator_category;
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਮੁੱਲ_type;
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ ptrdiff_t difference_type;
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ* ਸੂਚਕ;
	ਕਿਸਮਪਰਿਭਾਸ਼ਾ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ& reference;

	ਅਚਲ RTLIL::SigSpec *sig_p;
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ;
	ਪੂਰਨ ਸੂਚਕਾਂਕ;

	ਸਤਰਅੰਦਰ ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ*();
	ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ!=(ਅਚਲ RTLIL::SigSpecConstIterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ != other.ਸੂਚਕਾਂਕ; }
	ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ==(ਅਚਲ RTLIL::SigSpecIterator &other) ਅਚਲ { ਵਾਪਸ ਸੂਚਕਾਂਕ == other.ਸੂਚਕਾਂਕ; }
	ਸਤਰਅੰਦਰ ਖਾਲੀ ਸੰਚਾਲਕ++() { ਸੂਚਕਾਂਕ++; }
};

ਬਣਤਰ RTLIL::SigSpec
{
ਨਿਜੀ:
	ਮਿੱਤਰ class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, ਕੱਢੋ_3);
	ਗਿਣਤੀ Representation : ਅੱਖਰ {
		CHUNK,
		ਬਿੱਟ_3,
	};
	// An AtomicHash is either clear or a nonzero integer.
	ਬਣਤਰ AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ਅਚਲ AtomicHash &rhs) : atomic_(rhs.ਲੱਦੋ()) {}
		AtomicHash &ਸੰਚਾਲਕ=(ਅਚਲ AtomicHash &rhs) { store(rhs.ਲੱਦੋ()); ਵਾਪਸ *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> ਪਠ() ਅਚਲ {
			Hasher::hash_t ਮੁੱਲ = ਲੱਦੋ();
			ਜੇ (ਮੁੱਲ == 0)
				ਵਾਪਸ std::nullopt;
			ਵਾਪਸ ਮੁੱਲ;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		ਖਾਲੀ ਸੈੱਟ(Hasher::hash_t ਮੁੱਲ) ਅਚਲ {
			log_assert(ਮੁੱਲ != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(ਮੁੱਲ, std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed);
			log_assert(old == 0 || old == ਮੁੱਲ);
		}
		ਖਾਲੀ clear() { store(0); }
	ਨਿਜੀ:
		ਪੂਰਨ ਲੱਦੋ() ਅਚਲ { ਵਾਪਸ atomic_.ਲੱਦੋ(std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed); }
		ਖਾਲੀ store(Hasher::hash_t ਮੁੱਲ) ਅਚਲ {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(ਮੁੱਲ, std::ਸਮ੍ਰਿਤੀ_ਕ੍ਰਮ_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	ਸੰਘ {
		RTLIL::SigChunk chunk_;
		std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਬਿੱਟ_2; // LSB at index 0
	};

	ਖਾਲੀ init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ() {
		rep_ = ਬਿੱਟ_3;
		new (&ਬਿੱਟ_2) std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ>;
	}

	ਖਾਲੀ unpack();
	ਸਤਰਅੰਦਰ ਖਾਲੀ inline_unpack() {
		ਜੇ (rep_ == CHUNK)
			unpack();
	}
	ਖਾਲੀ try_repack();

	Hasher::hash_t updhash() ਅਚਲ;
	ਖਾਲੀ destroy() {
		ਜੇ (rep_ == CHUNK)
			chunk_.~SigChunk();
		ਨਹੀਂਤਾਂ
			ਬਿੱਟ_2.~ਸਦਿਸ਼();
	}
	ਮਿੱਤਰ ਬਣਤਰ Chunks;

ਜਨਤਕ:
	SigSpec() { init_ਖਾਲੀ_ਥਾਵਾਂ_ਬਿੱਟ(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ਅਚਲ SigSpec &ਮੁੱਲ) : rep_(ਮੁੱਲ.rep_), hash_(ਮੁੱਲ.hash_) {
		ਜੇ (ਮੁੱਲ.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(ਮੁੱਲ.chunk_);
		ਨਹੀਂਤਾਂ
			new (&ਬਿੱਟ_2) std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ>(ਮੁੱਲ.ਬਿੱਟ_2);
	}
	SigSpec(SigSpec &&ਮੁੱਲ) : rep_(ਮੁੱਲ.rep_), hash_(ਮੁੱਲ.hash_) {
		ਜੇ (ਮੁੱਲ.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(ਮੁੱਲ.chunk_));
		ਨਹੀਂਤਾਂ
			new (&ਬਿੱਟ_2) std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ>(std::move(ਮੁੱਲ.ਬਿੱਟ_2));
	}
	SigSpec(ਅਚਲ RTLIL::Const &ਮੁੱਲ);
	SigSpec(RTLIL::Const &&ਮੁੱਲ);
	SigSpec(ਅਚਲ RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਚੌੜਾਈ = 1);
	SigSpec(ਅਚਲ std::string &str);
	SigSpec(ਪੂਰਨ val, ਪੂਰਨ ਚੌੜਾਈ = 32);
	SigSpec(RTLIL::State ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ ਚੌੜਾਈ = 1);
	SigSpec(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ ਚੌੜਾਈ = 1);
	SigSpec(ਅਚਲ std::ਸਦਿਸ਼<RTLIL::SigChunk> &chunks);
	SigSpec(ਅਚਲ std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਬਿੱਟ);
	SigSpec(ਅਚਲ pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਬਿੱਟ);
	SigSpec(ਅਚਲ std::ਸੈੱਟ<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਬਿੱਟ);
	explicit SigSpec(bool ਦੋਆਧਾਰੀ_ਅੰਕ);
	~SigSpec() {
		destroy();
	}

	SigSpec &ਸੰਚਾਲਕ=(ਅਚਲ SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		ਜੇ (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		ਨਹੀਂਤਾਂ
			new (&ਬਿੱਟ_2) std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ>(rhs.ਬਿੱਟ_2);
		ਵਾਪਸ *this;
	}
	SigSpec &ਸੰਚਾਲਕ=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		ਜੇ (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		ਨਹੀਂਤਾਂ
			new (&ਬਿੱਟ_2) std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ>(std::move(rhs.ਬਿੱਟ_2));
		ਵਾਪਸ *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	ਬਣਤਰ Chunks {
		Chunks(ਅਚਲ SigSpec &spec) : spec(spec) {}
		ਬਣਤਰ const_iterator {
			using iterator_category = std::forward_iterator_ਟੈਗ;
			using ਮੁੱਲ_type = ਅਚਲ SigChunk &;
			using difference_type = std::ptrdiff_t;
			using ਸੂਚਕ = ਅਚਲ SigChunk *;
			using reference = ਅਚਲ SigChunk &;

			ਅਚਲ SigSpec &spec;
			ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ;
			SigChunk chunk;

			const_iterator(ਅਚਲ SigSpec &spec) : spec(spec) {
				ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ = 0;
				ਜੇ (spec.rep_ == ਬਿੱਟ_3)
					ਅਗਲਾ_chunk_ਬਿੱਟ();
			}
			ਗਿਣਤੀ ਸਿਰਾ_2 { ਸਿਰਾ_3 };
			const_iterator(ਅਚਲ SigSpec &spec, ਸਿਰਾ_2) : spec(spec) {
				ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ = spec.ਮਾਪ();
			}
			ਖਾਲੀ ਅਗਲਾ_chunk_ਬਿੱਟ();

			ਅਚਲ SigChunk &ਸੰਚਾਲਕ*() {
				ਜੇ (spec.rep_ == CHUNK)
					ਵਾਪਸ spec.chunk_;
				ਵਾਪਸ chunk;
			};
			ਅਚਲ SigChunk *ਸੰਚਾਲਕ->() { ਵਾਪਸ &**this; }
			const_iterator &ਸੰਚਾਲਕ++() {
				ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ += (**this).ਚੌੜਾਈ;
				ਜੇ (spec.rep_ == ਬਿੱਟ_3)
					ਅਗਲਾ_chunk_ਬਿੱਟ();
				ਵਾਪਸ *this;
			}
			bool ਸੰਚਾਲਕ==(ਅਚਲ const_iterator &rhs) ਅਚਲ { ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ == rhs.ਦੋਆਧਾਰੀ_ਅੰਕ_ਸੂਚਕਾਂਕ; }
			bool ਸੰਚਾਲਕ!=(ਅਚਲ const_iterator &rhs) ਅਚਲ { ਵਾਪਸ !(*this == rhs); }
		};
		const_iterator begin() ਅਚਲ { ਵਾਪਸ const_iterator(spec); }
		const_iterator ਸਿਰਾ() ਅਚਲ {
			const_iterator it(spec, const_iterator::ਸਿਰਾ_3);
			ਵਾਪਸ it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::ਸਦਿਸ਼<RTLIL::SigChunk>::const_ਉਲਟ_iterator rbegin() {
			ensure_chunk_ਸਦਿਸ਼();
			ਵਾਪਸ chunk_ਸਦਿਸ਼.rbegin();
		}
		std::ਸਦਿਸ਼<RTLIL::SigChunk>::const_ਉਲਟ_iterator rend() {
			ensure_chunk_ਸਦਿਸ਼();
			ਵਾਪਸ chunk_ਸਦਿਸ਼.rend();
		}
		ਪੂਰਨ ਮਾਪ() {
			ensure_chunk_ਸਦਿਸ਼();
			ਵਾਪਸ chunk_ਸਦਿਸ਼.ਮਾਪ();
		}
		ਪੂਰਨ ਮਾਪ() ਅਚਲ {
			ਵਾਪਸ std::ਦੂਰੀ(begin(), ਸਿਰਾ());
		}
		ਅਚਲ SigChunk &at(ਪੂਰਨ ਸੂਚਕਾਂਕ) {
			ensure_chunk_ਸਦਿਸ਼();
			ਵਾਪਸ chunk_ਸਦਿਸ਼.at(ਸੂਚਕਾਂਕ);
		}
		ਸੰਚਾਲਕ ਅਚਲ std::ਸਦਿਸ਼<RTLIL::SigChunk>&() {
			ensure_chunk_ਸਦਿਸ਼();
			ਵਾਪਸ chunk_ਸਦਿਸ਼;
		}
	ਨਿਜੀ:
		ਖਾਲੀ ensure_chunk_ਸਦਿਸ਼() {
			ਜੇ (spec.ਮਾਪ() > 0 && chunk_ਸਦਿਸ਼.ਖਾਲੀ_ਥਾਵਾਂ()) {
				ਲਈ (ਅਚਲ RTLIL::SigChunk &c : *this)
					chunk_ਸਦਿਸ਼.ਧੱਕੋ_ਪਿੱਠ(c);
			}
		}
		ਅਚਲ SigSpec &spec;
		std::ਸਦਿਸ਼<RTLIL::SigChunk> chunk_ਸਦਿਸ਼;
	};
	ਮਿੱਤਰ ਬਣਤਰ Chunks::const_iterator;

	ਸਤਰਅੰਦਰ Chunks chunks() ਅਚਲ { ਵਾਪਸ {*this}; }
	ਸਤਰਅੰਦਰ ਅਚਲ SigSpec &ਬਿੱਟ() ਅਚਲ { ਵਾਪਸ *this; }

	ਸਤਰਅੰਦਰ ਪੂਰਨ ਮਾਪ() ਅਚਲ { ਵਾਪਸ rep_ == CHUNK ? chunk_.ਚੌੜਾਈ : Get_ਮਾਪ(ਬਿੱਟ_2); }
	ਸਤਰਅੰਦਰ bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ { ਵਾਪਸ ਮਾਪ() == 0; };

	ਸਤਰਅੰਦਰ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਸੰਚਾਲਕ[](ਪੂਰਨ ਸੂਚਕਾਂਕ) { inline_unpack(); hash_.clear(); ਵਾਪਸ ਬਿੱਟ_2.at(ਸੂਚਕਾਂਕ); }
	ਸਤਰਅੰਦਰ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਸੰਚਾਲਕ[](ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ {
		ਜੇ (rep_ == CHUNK) {
			ਜੇ (ਸੂਚਕਾਂਕ < 0 || ਸੂਚਕਾਂਕ >= chunk_.ਚੌੜਾਈ)
				throw std::ਬਾਹਰਲਾ_ਦਾ_range("SigSpec::operator[]");
			ਜੇ (chunk_.wire)
				ਵਾਪਸ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(chunk_.wire, chunk_.ਵਿੱਥ + ਸੂਚਕਾਂਕ);
			ਵਾਪਸ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(chunk_.ਅੰਕੜੇ[ਸੂਚਕਾਂਕ]);
		}
		ਵਾਪਸ ਬਿੱਟ_2.at(ਸੂਚਕਾਂਕ);
	}

	ਸਤਰਅੰਦਰ RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.ਸੂਚਕਾਂਕ = 0; ਵਾਪਸ it; }
	ਸਤਰਅੰਦਰ RTLIL::SigSpecIterator ਸਿਰਾ() { RTLIL::SigSpecIterator it; it.sig_p = this; it.ਸੂਚਕਾਂਕ = ਮਾਪ(); ਵਾਪਸ it; }

	ਸਤਰਅੰਦਰ RTLIL::SigSpecConstIterator begin() ਅਚਲ { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.ਸੂਚਕਾਂਕ = 0; ਵਾਪਸ it; }
	ਸਤਰਅੰਦਰ RTLIL::SigSpecConstIterator ਸਿਰਾ() ਅਚਲ { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.ਸੂਚਕਾਂਕ = ਮਾਪ(); ਵਾਪਸ it; }

	ਖਾਲੀ ਛਾਂਟੋ();
	ਖਾਲੀ ਛਾਂਟੋ_and_unify();

	ਖਾਲੀ replace(ਅਚਲ RTLIL::SigSpec &ਨਮੂਨਾ, ਅਚਲ RTLIL::SigSpec &with);
	ਖਾਲੀ replace(ਅਚਲ RTLIL::SigSpec &ਨਮੂਨਾ, ਅਚਲ RTLIL::SigSpec &with, RTLIL::SigSpec *other) ਅਚਲ;

	ਖਾਲੀ replace(ਅਚਲ dict<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਿਯਮਾਂ);
	ਖਾਲੀ replace(ਅਚਲ dict<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਿਯਮਾਂ, RTLIL::SigSpec *other) ਅਚਲ;

	ਖਾਲੀ replace(ਅਚਲ std::map<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਿਯਮਾਂ);
	ਖਾਲੀ replace(ਅਚਲ std::map<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਿਯਮਾਂ, RTLIL::SigSpec *other) ਅਚਲ;

	ਖਾਲੀ replace(ਪੂਰਨ ਵਿੱਥ, ਅਚਲ RTLIL::SigSpec &with);

	ਖਾਲੀ remove(ਅਚਲ RTLIL::SigSpec &ਨਮੂਨਾ);
	ਖਾਲੀ remove(ਅਚਲ RTLIL::SigSpec &ਨਮੂਨਾ, RTLIL::SigSpec *other) ਅਚਲ;
	ਖਾਲੀ remove2(ਅਚਲ RTLIL::SigSpec &ਨਮੂਨਾ, RTLIL::SigSpec *other);

	ਖਾਲੀ remove(ਅਚਲ pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਮੂਨਾ);
	ਖਾਲੀ remove(ਅਚਲ pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਮੂਨਾ, RTLIL::SigSpec *other) ਅਚਲ;
	ਖਾਲੀ remove2(ਅਚਲ pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਮੂਨਾ, RTLIL::SigSpec *other);
	ਖਾਲੀ remove2(ਅਚਲ std::ਸੈੱਟ<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਮੂਨਾ, RTLIL::SigSpec *other);
	ਖਾਲੀ remove2(ਅਚਲ pool<RTLIL::Wire*> &ਨਮੂਨਾ, RTLIL::SigSpec *other);

	ਖਾਲੀ remove(ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਲੰਮਾਈ = 1);
	ਖਾਲੀ remove_const();

	RTLIL::SigSpec ਕੱਢੋ_2(ਅਚਲ RTLIL::SigSpec &ਨਮੂਨਾ, ਅਚਲ RTLIL::SigSpec *other = ਨਿਰਾ) ਅਚਲ;
	RTLIL::SigSpec ਕੱਢੋ_2(ਅਚਲ pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &ਨਮੂਨਾ, ਅਚਲ RTLIL::SigSpec *other = ਨਿਰਾ) ਅਚਲ;
	RTLIL::SigSpec ਕੱਢੋ_2(ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਲੰਮਾਈ = 1) ਅਚਲ;
	RTLIL::SigSpec ਕੱਢੋ_ਸਿਰਾ(ਪੂਰਨ ਵਿੱਥ) ਅਚਲ { ਵਾਪਸ ਕੱਢੋ_2(ਵਿੱਥ, ਮਾਪ() - ਵਿੱਥ); }

	ਖਾਲੀ rewrite_wires(std::function<ਖਾਲੀ(RTLIL::Wire*& wire)> rewrite);

	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ lsb() ਅਚਲ { log_assert(ਮਾਪ()); ਵਾਪਸ (*this)[0]; };
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ msb() ਅਚਲ { log_assert(ਮਾਪ()); ਵਾਪਸ (*this)[ਮਾਪ() - 1]; };
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ front() ਅਚਲ { ਵਾਪਸ (*this)[0]; }
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਪਿੱਠ() ਅਚਲ { ਵਾਪਸ (*this)[ਮਾਪ() - 1]; }

	ਖਾਲੀ append(ਅਚਲ RTLIL::SigSpec &ਸੰਕੇਤ);
	ਸਤਰਅੰਦਰ ਖਾਲੀ append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	ਸਤਰਅੰਦਰ ਖਾਲੀ append(ਅਚਲ RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	ਸਤਰਅੰਦਰ ਖਾਲੀ append(ਅਚਲ RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	ਖਾਲੀ append(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &ਦੋਆਧਾਰੀ_ਅੰਕ);
	ਸਤਰਅੰਦਰ ਖਾਲੀ append(RTLIL::State state) { append(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(state)); }
	ਸਤਰਅੰਦਰ ਖਾਲੀ append(bool bool_) { append(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(bool_)); }

	ਖਾਲੀ extend_u0(ਪੂਰਨ ਚੌੜਾਈ, bool is_signed = false);

	RTLIL::SigSpec repeat(ਪੂਰਨ ਅੰਕ) ਅਚਲ;

	ਖਾਲੀ ਉਲਟ() { inline_unpack(); std::ਉਲਟ(ਬਿੱਟ_2.begin(), ਬਿੱਟ_2.ਸਿਰਾ()); }

	bool ਸੰਚਾਲਕ <(ਅਚਲ RTLIL::SigSpec &other) ਅਚਲ;
	bool ਸੰਚਾਲਕ ==(ਅਚਲ RTLIL::SigSpec &other) ਅਚਲ;
	ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ !=(ਅਚਲ RTLIL::SigSpec &other) ਅਚਲ { ਵਾਪਸ !(*this == other); }

	bool is_wire() ਅਚਲ;
	bool is_chunk() ਅਚਲ;
	ਸਤਰਅੰਦਰ bool is_ਦੋਆਧਾਰੀ_ਅੰਕ() ਅਚਲ { ਵਾਪਸ ਮਾਪ() == 1; }

	bool ਗਿਆਤ_driver() ਅਚਲ;

	bool is_fully_const() ਅਚਲ;
	bool is_fully_zero() ਅਚਲ;
	bool is_fully_ones() ਅਚਲ;
	bool is_fully_def() ਅਚਲ;
	bool is_fully_undef() ਅਚਲ;
	bool has_const() ਅਚਲ;
	bool has_const(State state) ਅਚਲ;
	bool has_marked_ਬਿੱਟ() ਅਚਲ;
	bool is_onehot(ਪੂਰਨ *pos = nullptr) ਅਚਲ;

	bool as_bool() ਅਚਲ;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	ਪੂਰਨ as_int(bool is_signed = false) ਅਚਲ;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_ਮੰਜ਼ਿਲ_int(bool is_signed = false) ਅਚਲ;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<ਪੂਰਨ> try_as_int(bool is_signed = false) ਅਚਲ;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	ਪੂਰਨ as_int_saturating(bool is_signed = false) ਅਚਲ;

	std::string as_string() ਅਚਲ;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() ਅਚਲ;
	RTLIL::Const as_const() ਅਚਲ;
	RTLIL::Wire *as_wire() ਅਚਲ;
	RTLIL::SigChunk as_chunk() ਅਚਲ;
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ as_ਦੋਆਧਾਰੀ_ਅੰਕ() ਅਚਲ;

	bool ਮੇਲ(ਅਚਲ ਅੱਖਰ* ਨਮੂਨਾ) ਅਚਲ;

	std::ਸੈੱਟ<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਮੰਜ਼ਿਲ_sigbit_ਸੈੱਟ() ਅਚਲ;
	pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਮੰਜ਼ਿਲ_sigbit_pool() ਅਚਲ;
	std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਮੰਜ਼ਿਲ_sigbit_ਸਦਿਸ਼() ਅਚਲ;
	std::map<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਮੰਜ਼ਿਲ_sigbit_map(ਅਚਲ RTLIL::SigSpec &other) ਅਚਲ;
	dict<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਮੰਜ਼ਿਲ_sigbit_dict(ਅਚਲ RTLIL::SigSpec &other) ਅਚਲ;

	ਸਥਿਰ bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	ਸਥਿਰ bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	ਸਥਿਰ bool parse_rhs(ਅਚਲ RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	ਸੰਚਾਲਕ std::ਸਦਿਸ਼<RTLIL::SigChunk>() ਅਚਲ;
	ਸੰਚਾਲਕ std::ਸਦਿਸ਼<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ>() ਅਚਲ { ਵਾਪਸ ਮੰਜ਼ਿਲ_sigbit_ਸਦਿਸ਼(); }
	ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &at(ਪੂਰਨ ਵਿੱਥ, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &defval) { ਵਾਪਸ ਵਿੱਥ < ਮਾਪ() ? (*this)[ਵਿੱਥ] : defval; }
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ& at(ਪੂਰਨ ਵਿੱਥ) { ਵਾਪਸ (*this)[ਵਿੱਥ]; }
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ at(ਪੂਰਨ ਵਿੱਥ) ਅਚਲ { ਵਾਪਸ (*this)[ਵਿੱਥ]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
		Hasher::hash_t val;
		ਜੇ (std::optional<Hasher::hash_t> current = hash_.ਪਠ())
			val = *current;
		ਨਹੀਂਤਾਂ
			val = updhash();
		h.eat(val);
		ਵਾਪਸ h;
	}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NDEBUG
	ਖਾਲੀ check(ਅਚਲ Module *mod = nullptr) ਅਚਲ;
#ਨਹੀਂਤਾਂ
	ਖਾਲੀ check(ਅਚਲ Module *mod = nullptr) ਅਚਲ { (ਖਾਲੀ)mod; }
#ਜੇ_ਅੰਤ
};

ਬਣਤਰ RTLIL::Selection
{
	// selection includes boxed modules
	bool selects_boxes;
	// selection covers full design, including boxed modules
	bool complete_selection;
	// selection covers full design, not including boxed modules
	bool ਭਰਿਆ_selection;
	pool<RTLIL::IdString> selected_modules;
	dict<RTLIL::IdString, pool<RTLIL::IdString>> selected_members;
	RTLIL::Design *current_design;

	// create a new selection
	Selection(
		// should the selection cover the full design
		bool ਭਰਿਆ_2 = true,
		// should the selection include boxed modules
		bool boxes = false,
		// the design to select from
		RTLIL::Design *design = nullptr
	) :
		selects_boxes(boxes), complete_selection(ਭਰਿਆ_2 && boxes), ਭਰਿਆ_selection(ਭਰਿਆ_2 && !boxes), current_design(design) { }

	// checks if the given module exists in the current design and is a
	// boxed module, warning the user if the current design is not set
	bool boxed_module(RTLIL::IdString mod_name) ਅਚਲ;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ਅਚਲ;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_ਸਾਰਾ_module(RTLIL::IdString mod_name) ਅਚਲ;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ਅਚਲ;

	// optimizes this selection for the given design by:
	// - removing non-existent modules and members, any boxed modules and
	//   their members (if selection does not include boxes), and any
	//   partially selected modules with no selected members;
	// - marking partially selected modules as wholly selected if all
	//   members of that module are selected; and
	// - marking selection as a complete_selection if all modules in the
	//   given design are selected, or a full_selection if it does not
	//   include boxes.
	ਖਾਲੀ optimize(RTLIL::Design *design);

	// checks if selection covers full design (may or may not include
	// boxed-modules)
	bool selects_all() ਅਚਲ {
		ਵਾਪਸ ਭਰਿਆ_selection || complete_selection;
	}

	// add whole module to this selection
	ਸਾਂਚਾ<typename T1> ਖਾਲੀ select(T1 *module) {
		ਜੇ (!selects_all() && selected_modules.ਗਿਣਤੀ_2(module->name) == 0) {
			IdString name = module->name;
			selected_modules.ਪਾਓ(name);
			selected_members.erase(name);
			ਜੇ (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	ਸਾਂਚਾ<typename T1, typename T2> ਖਾਲੀ select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	ਸਾਂਚਾ<typename T1> ਖਾਲੀ select(T1 *module, RTLIL::IdString member) {
		ਜੇ (!selects_all() && selected_modules.ਗਿਣਤੀ_2(module->name.ref()) == 0) {
			selected_members[module->name.ref()].ਪਾਓ(member);
			ਜੇ (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ {
		ਵਾਪਸ !selects_all() && selected_modules.ਖਾਲੀ_ਥਾਵਾਂ() && selected_members.ਖਾਲੀ_ਥਾਵਾਂ();
	}

	// clear this selection, leaving it empty
	ਖਾਲੀ clear();

	// create a new selection which is empty
	ਸਥਿਰ Selection ਖਾਲੀ_ਥਾਵਾਂ_Selection(RTLIL::Design *design = nullptr) { ਵਾਪਸ Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	ਸਥਿਰ Selection ਭਰਿਆ_Selection(RTLIL::Design *design = nullptr) { ਵਾਪਸ Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	ਸਥਿਰ Selection CompleteSelection(RTLIL::Design *design = nullptr) { ਵਾਪਸ Selection(true, true, design); };
};

ਬਣਤਰ RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ { h.eat(hashidx_); ਵਾਪਸ h; }

	Monitor() {
		ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ hashidx_ਗਿਣਤੀ = 123456789;
		hashidx_ਗਿਣਤੀ = mkhash_xorshift(hashidx_ਗਿਣਤੀ);
		hashidx_ = hashidx_ਗਿਣਤੀ;
	}

	ਆਭਾਸੀ ~Monitor() { }
	ਆਭਾਸੀ ਖਾਲੀ notify_module_ਜੋੜੋ(RTLIL::Module*) { }
	ਆਭਾਸੀ ਖਾਲੀ notify_module_del(RTLIL::Module*) { }
	ਆਭਾਸੀ ਖਾਲੀ notify_connect(RTLIL::Cell*, RTLIL::IdString, ਅਚਲ RTLIL::SigSpec&, ਅਚਲ RTLIL::SigSpec&) { }
	ਆਭਾਸੀ ਖਾਲੀ notify_connect(RTLIL::Module*, ਅਚਲ RTLIL::SigSig&) { }
	ਆਭਾਸੀ ਖਾਲੀ notify_connect(RTLIL::Module*, ਅਚਲ std::ਸਦਿਸ਼<RTLIL::SigSig>&) { }
	ਆਭਾਸੀ ਖਾਲੀ notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
ਬਣਤਰ define_map_t;

ਸਾਂਚਾ<typename N>
ਸਤਰਅੰਦਰ constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::ਪੱਤੀ> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ਅਚਲ ਅੱਖਰ*> || std::is_same_v<std::decay_t<N>, ਅੱਖਰ*>;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, ਪੂਰਨ> = 0

ਸਾਂਚਾ<typename N>
ਸਤਰਅੰਦਰ constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ਅਚਲ ਅੱਖਰ *> || std::is_same_v<std::decay_t<N>, ਅੱਖਰ *>;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, ਪੂਰਨ> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YS_NAME_FWD_POOL(_func, _pool) \
	ਸਾਂਚਾ<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(ਸਵੈਚਾਲਿਤ) _func(N name, Rest&&... rest) \
		{ ਵਾਪਸ _func(_pool.ਜੋੜੋ(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YS_NAME_FWD_2ND_POOL(_func, _pool) \
	ਸਾਂਚਾ<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(ਸਵੈਚਾਲਿਤ) _func(T &&ਪਹਿਲਾ, N name) \
		{ ਵਾਪਸ _func(std::forward<T>(ਪਹਿਲਾ), _pool.ਜੋੜੋ(std::move(name))); }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

ਬਣਤਰ RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ { h.eat(hashidx_); ਵਾਪਸ h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	ਖਾਲੀ bufNormalize(bool enable=true);

	ਪੂਰਨ refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

ਨਿਜੀ:
	TwinePool twines_;

ਜਨਤਕ:
	TwinePool &twines() { ਵਾਪਸ twines_; }
	ਅਚਲ TwinePool &twines() ਅਚਲ { ਵਾਪਸ twines_; }

	ਖਾਲੀ absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	ਮਾਪ_t gc_twines();

	std::ਸਦਿਸ਼<std::unique_ptr<AST::Ast_ਗੰਢ>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_ਨਿਰਧਾਰਤ_ਕਰਦਾ;

	std::ਸਦਿਸ਼<RTLIL::Selection> selection_ਢੇਰ;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_ਸਰਗਰਮ_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ਅਚਲ RTLIL::Module *module(RTLIL::IdString name) ਅਚਲ;
	ਸਾਂਚਾ<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ ਵਾਪਸ module(twines().ਲੱਭੋ(std::string_view(name))); }
	ਸਾਂਚਾ<typename N, YS_NAME_STRING(N)> ਅਚਲ RTLIL::Module *module(N name) ਅਚਲ
		{ ਵਾਪਸ module(twines().ਲੱਭੋ(std::string_view(name))); }
	RTLIL::Module *ਸਿਖਰ_module() ਅਚਲ;

	bool has(RTLIL::IdString id) ਅਚਲ {
		twines_.check_owned(id);
		ਵਾਪਸ modules_.ਗਿਣਤੀ_2(id) != 0;
	}
	ਸਾਂਚਾ<typename N, YS_NAME_STRING(N)> bool has(N name) ਅਚਲ
		{ ਵਾਪਸ has(twines().ਲੱਭੋ(std::string_view(name))); }

	ਖਾਲੀ ਜੋੜੋ(RTLIL::Module *module);

	RTLIL::Module *ਜੋੜੋ_Module(RTLIL::IdString name);
	YS_NAME_FWD_POOL(ਜੋੜੋ_Module, twines())
	ਖਾਲੀ remove(RTLIL::Module *module);
	ਖਾਲੀ rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	ਖਾਲੀ scratchpad_unset(ਅਚਲ std::string &varname);

	ਖਾਲੀ scratchpad_ਸੈੱਟ_int(ਅਚਲ std::string &varname, ਪੂਰਨ ਮੁੱਲ);
	ਖਾਲੀ scratchpad_ਸੈੱਟ_bool(ਅਚਲ std::string &varname, bool ਮੁੱਲ);
	ਖਾਲੀ scratchpad_ਸੈੱਟ_string(ਅਚਲ std::string &varname, std::string ਮੁੱਲ);

	ਪੂਰਨ scratchpad_get_int(ਅਚਲ std::string &varname, ਪੂਰਨ default_ਮੁੱਲ_2 = 0) ਅਚਲ;
	bool scratchpad_get_bool(ਅਚਲ std::string &varname, bool default_ਮੁੱਲ_2 = false) ਅਚਲ;
	std::string scratchpad_get_string(ਅਚਲ std::string &varname, ਅਚਲ std::string &default_ਮੁੱਲ_2 = std::string()) ਅਚਲ;

	ਖਾਲੀ ਛਾਂਟੋ();
	ਖਾਲੀ ਛਾਂਟੋ_modules();
	ਖਾਲੀ check();
	ਖਾਲੀ optimize();

	ਖਾਲੀ clone_into(RTLIL::Design *dst) ਅਚਲ;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ਅਚਲ;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_ਸਾਰਾ_module(RTLIL::IdString mod_name) ਅਚਲ;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ਅਚਲ;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ਅਚਲ;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_ਸਾਰਾ_module(RTLIL::Module *mod) ਅਚਲ;

	// push the given selection to the selection stack
	ਖਾਲੀ ਧੱਕੋ_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	ਖਾਲੀ ਧੱਕੋ_ਖਾਲੀ_ਥਾਵਾਂ_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	ਖਾਲੀ ਧੱਕੋ_ਭਰਿਆ_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	ਖਾਲੀ ਧੱਕੋ_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	ਖਾਲੀ ਕੱਢੋ_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		ਵਾਪਸ selection_ਢੇਰ.ਪਿੱਠ();
	}

	// get the current selection
	ਅਚਲ RTLIL::Selection &selection() ਅਚਲ {
		ਵਾਪਸ selection_ਢੇਰ.ਪਿੱਠ();
	}

	// is the current selection a full selection (no boxes)
	bool ਭਰਿਆ_selection() ਅਚਲ {
		ਵਾਪਸ selection().ਭਰਿਆ_selection;
	}

	// is the given module in the current selection
	ਸਾਂਚਾ<typename T1> bool selected(T1 *module) ਅਚਲ {
		ਵਾਪਸ selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	ਸਾਂਚਾ<typename T1, typename T2> bool selected(T1 *module, T2 *member) ਅਚਲ {
		ਵਾਪਸ selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	ਸਾਂਚਾ<typename T1> ਖਾਲੀ select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	ਸਾਂਚਾ<typename T1, typename T2> ਖਾਲੀ select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::ਸਦਿਸ਼<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) ਅਚਲ;

	// returns all selected modules, and may include boxes
	std::ਸਦਿਸ਼<RTLIL::Module*> all_selected_modules() ਅਚਲ { ਵਾਪਸ selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::ਸਦਿਸ਼<RTLIL::Module*> selected_unboxed_modules() ਅਚਲ { ਵਾਪਸ selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::ਸਦਿਸ਼<RTLIL::Module*> selected_unboxed_modules_warn() ਅਚਲ { ਵਾਪਸ selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::ਸਦਿਸ਼<RTLIL::Module*> selected_ਸਾਰਾ_modules() ਅਚਲ { ਵਾਪਸ selected_modules(SELECT_ਸਾਰਾ_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::ਸਦਿਸ਼<RTLIL::Module*> all_selected_ਸਾਰਾ_modules() ਅਚਲ { ਵਾਪਸ selected_modules(SELECT_ਸਾਰਾ_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::ਸਦਿਸ਼<RTLIL::Module*> selected_ਸਾਰਾ_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ਅਚਲ { ਵਾਪਸ selected_modules(SELECT_ਸਾਰਾ_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::ਸਦਿਸ਼<RTLIL::Module*> selected_unboxed_ਸਾਰਾ_modules() ਅਚਲ { ਵਾਪਸ selected_modules(SELECT_ਸਾਰਾ_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::ਸਦਿਸ਼<RTLIL::Module*> selected_unboxed_ਸਾਰਾ_modules_warn() ਅਚਲ { ਵਾਪਸ selected_modules(SELECT_ਸਾਰਾ_WARN, SB_UNBOXED_WARN); }

	ਸਥਿਰ std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Design*> *get_all_designs(ਖਾਲੀ);

	std::string ਮੰਜ਼ਿਲ_rtlil_str(bool only_selected = true) ਅਚਲ;
};

ਬਣਤਰ RTLIL::Module : ਜਨਤਕ RTLIL::AttrObject
{
	ਮਿੱਤਰ ਬਣਤਰ RTLIL::Cell;
	ਮਿੱਤਰ ਬਣਤਰ RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ { h.eat(hashidx_); ਵਾਪਸ h; }

ਸੁਰੱਖਿਅਤ:
	ਖਾਲੀ ਜੋੜੋ(RTLIL::Wire *wire);
	ਖਾਲੀ ਜੋੜੋ(RTLIL::Cell *cell);
	ਖਾਲੀ ਜੋੜੋ(RTLIL::Process *process);

ਜਨਤਕ:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ਅਚਲ { ਵਾਪਸ design->twines(); }

	ਪੂਰਨ refcount_wires_;
	ਪੂਰਨ refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::ਸਦਿਸ਼<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_ਮੁੱਲਾਂ;
	dict<RTLIL::IdString, RTLIL::ਸਮ੍ਰਿਤੀ*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	ਆਭਾਸੀ ~Module();
	ਆਭਾਸੀ RTLIL::IdString derive(RTLIL::Design *design, ਅਚਲ dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	ਆਭਾਸੀ RTLIL::IdString derive(RTLIL::Design *design, ਅਚਲ dict<RTLIL::IdString, RTLIL::Const> &parameters, ਅਚਲ dict<RTLIL::IdString, RTLIL::Module*> &interfaces, ਅਚਲ dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	ਆਭਾਸੀ ਮਾਪ_t ਗਿਣਤੀ_id(RTLIL::IdString id);
	ਆਭਾਸੀ ਖਾਲੀ expand_interfaces(RTLIL::Design *design, ਅਚਲ dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	ਆਭਾਸੀ bool reprocess_if_necessary(RTLIL::Design *design);

	ਆਭਾਸੀ ਖਾਲੀ ਛਾਂਟੋ();
	ਆਭਾਸੀ ਖਾਲੀ check();
	ਆਭਾਸੀ ਖਾਲੀ optimize();
	ਆਭਾਸੀ ਖਾਲੀ makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ਅਚਲ {
		ਵਾਪਸ get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	ਖਾਲੀ connect(ਅਚਲ RTLIL::SigSig &conn);
	ਖਾਲੀ connect(ਅਚਲ RTLIL::SigSpec &lhs, ਅਚਲ RTLIL::SigSpec &rhs);
	ਖਾਲੀ new_connections(ਅਚਲ std::ਸਦਿਸ਼<RTLIL::SigSig> &new_conn);
	ਅਚਲ std::ਸਦਿਸ਼<RTLIL::SigSig> &connections() ਅਚਲ;

	std::ਸਦਿਸ਼<RTLIL::IdString> ports;
	ਖਾਲੀ fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_ਕਤਾਰ;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_ਪੋਰਟ_ਕਤਾਰ;
	pool<RTLIL::Wire *> buf_norm_wire_ਕਤਾਰ;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_ਸੂਚਕਾਂਕ;
	ਖਾਲੀ bufNormalize();

	ਸਾਂਚਾ<typename T> ਖਾਲੀ rewrite_sigspecs(T &functor);
	ਸਾਂਚਾ<typename T> ਖਾਲੀ rewrite_sigspecs2(T &functor);
	ਖਾਲੀ cloneInto(RTLIL::Module *new_mod) ਅਚਲ;
	ਆਭਾਸੀ RTLIL::Module *clone() ਅਚਲ;
	ਆਭਾਸੀ RTLIL::Module *clone(RTLIL::Design *dst) ਅਚਲ;
	ਆਭਾਸੀ RTLIL::Module *clone(RTLIL::Design *dst, IdString ਨਿਸ਼ਾਨਾ_name) ਅਚਲ;

	bool has_memories() ਅਚਲ;
	bool has_processes() ਅਚਲ;

	bool has_memories_warn() ਅਚਲ;
	bool has_processes_warn() ਅਚਲ;

	bool is_selected() ਅਚਲ;
	bool is_selected_ਸਾਰਾ() ਅਚਲ;

	std::ਸਦਿਸ਼<RTLIL::Wire*> selected_wires() ਅਚਲ;
	std::ਸਦਿਸ਼<RTLIL::Cell*> selected_cells() ਅਚਲ;
	std::ਸਦਿਸ਼<RTLIL::ਸਮ੍ਰਿਤੀ*> selected_memories() ਅਚਲ;
	std::ਸਦਿਸ਼<RTLIL::Process*> selected_processes() ਅਚਲ;
	std::ਸਦਿਸ਼<RTLIL::SelectedMember> selected_members() ਅਚਲ;

	ਸਾਂਚਾ<typename T> bool selected(T *member) ਅਚਲ {
		ਵਾਪਸ design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		ਜੇ (design)
			design->twines().check_owned(id);
		ਸਵੈਚਾਲਿਤ it = wires_.ਲੱਭੋ(id);
		ਵਾਪਸ it == wires_.ਸਿਰਾ() ? nullptr : it->ਦੂਜਾ;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		ਜੇ (design)
			design->twines().check_owned(id);
		ਸਵੈਚਾਲਿਤ it = cells_.ਲੱਭੋ(id);
		ਵਾਪਸ it == cells_.ਸਿਰਾ() ? nullptr : it->ਦੂਜਾ;
	}

	ਅਚਲ RTLIL::Wire* wire(RTLIL::IdString id) ਅਚਲ{
		ਜੇ (design)
			design->twines().check_owned(id);
		ਸਵੈਚਾਲਿਤ it = wires_.ਲੱਭੋ(id);
		ਵਾਪਸ it == wires_.ਸਿਰਾ() ? nullptr : it->ਦੂਜਾ;
	}
	ਅਚਲ RTLIL::Cell* cell(RTLIL::IdString id) ਅਚਲ {
		ਜੇ (design)
			design->twines().check_owned(id);
		ਸਵੈਚਾਲਿਤ it = cells_.ਲੱਭੋ(id);
		ਵਾਪਸ it == cells_.ਸਿਰਾ() ? nullptr : it->ਦੂਜਾ;
	}

	ਸਾਂਚਾ<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ ਵਾਪਸ wire(twines().ਲੱਭੋ(std::string_view(name))); }
	ਸਾਂਚਾ<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ ਵਾਪਸ cell(twines().ਲੱਭੋ(std::string_view(name))); }
	ਸਾਂਚਾ<typename N, YS_NAME_STRING(N)> ਅਚਲ RTLIL::Wire *wire(N name) ਅਚਲ
		{ ਵਾਪਸ wire(twines().ਲੱਭੋ(std::string_view(name))); }
	ਸਾਂਚਾ<typename N, YS_NAME_STRING(N)> ਅਚਲ RTLIL::Cell *cell(N name) ਅਚਲ
		{ ਵਾਪਸ cell(twines().ਲੱਭੋ(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { ਵਾਪਸ RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	ਪੂਰਨ wires_ਮਾਪ() ਅਚਲ { ਵਾਪਸ wires_.ਮਾਪ(); }
	RTLIL::Wire* wire_at(ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ { ਵਾਪਸ wires_.element(ਸੂਚਕਾਂਕ)->ਦੂਜਾ; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { ਵਾਪਸ RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	ਪੂਰਨ cells_ਮਾਪ() ਅਚਲ { ਵਾਪਸ cells_.ਮਾਪ(); }
	RTLIL::Cell* cell_at(ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ { ਵਾਪਸ cells_.element(ਸੂਚਕਾਂਕ)->ਦੂਜਾ; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	ਖਾਲੀ remove(ਅਚਲ pool<RTLIL::Wire*> &wires);
	ਖਾਲੀ remove(RTLIL::Cell *cell);
	ਖਾਲੀ remove(RTLIL::ਸਮ੍ਰਿਤੀ *ਸਮ੍ਰਿਤੀ_3);
	ਖਾਲੀ remove(RTLIL::Process *process);

	ਖਾਲੀ rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	ਖਾਲੀ rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	ਖਾਲੀ rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ_ਨਾਂ(RTLIL::Wire *w1, RTLIL::Wire *w2);
	ਖਾਲੀ ਅਦਲਾ_ਬਦਲੀ_ਨਾਂ(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, ਪੂਰਨ &ਸੂਚਕਾਂਕ);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *ਜੋੜੋ_Wire(RTLIL::IdString name, ਪੂਰਨ ਚੌੜਾਈ = 1);
	RTLIL::Wire *ਜੋੜੋ_Wire(RTLIL::IdString name, ਅਚਲ RTLIL::Wire *other);
	YS_NAME_FWD(ਜੋੜੋ_Wire)

	RTLIL::Cell *ਜੋੜੋ_Cell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *ਜੋੜੋ_Cell(RTLIL::IdString name, ਅਚਲ RTLIL::Cell *other);
	YS_NAME_FWD(ਜੋੜੋ_Cell)
	ਸਾਂਚਾ<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *ਜੋੜੋ_Cell(RTLIL::IdString name, T type) { ਵਾਪਸ ਜੋੜੋ_Cell(name, design->twines().ਜੋੜੋ(std::move(type))); }

	RTLIL::ਸਮ੍ਰਿਤੀ *ਜੋੜੋ_ਸਮ੍ਰਿਤੀ(RTLIL::IdString name);
	YS_NAME_FWD(ਜੋੜੋ_ਸਮ੍ਰਿਤੀ)
	RTLIL::ਸਮ੍ਰਿਤੀ *ਜੋੜੋ_ਸਮ੍ਰਿਤੀ(RTLIL::IdString name, ਅਚਲ RTLIL::ਸਮ੍ਰਿਤੀ *other);

	RTLIL::Process *ਜੋੜੋ_Process(RTLIL::IdString name);
	YS_NAME_FWD(ਜੋੜੋ_Process)
	RTLIL::Process *ਜੋੜੋ_Process(RTLIL::IdString name, ਅਚਲ RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* ਜੋੜੋ_Not     (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Pos     (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Buf     (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Barrier (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Neg     (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_And  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Or   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Xor  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Xnor (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_Reduce_And  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Reduce_Or   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Reduce_Xor  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Reduce_Xnor (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Reduce_Bool (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_Shl    (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Shr    (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Sshl   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Sshr   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_ਖਿਸਕਾਓ  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Shiftx (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_Lt  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Le  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Eq  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Ne  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Eqx (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Nex (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Ge  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Gt  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_ਜੋੜੋ (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Sub (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Mul (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	// truncating division
	RTLIL::Cell* ਜੋੜੋ_Div (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	// truncating modulo
	RTLIL::Cell* ਜੋੜੋ_Mod (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Div_Floor (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Mod_Floor (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Pow (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_Fa (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_c, ਅਚਲ RTLIL::SigSpec &sig_x, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_Logic_Not (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Logic_And (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Logic_Or  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, bool is_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_Mux  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Pmux (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Bmux (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Demux (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_Bweqx  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Bwmux  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_ਟੁਕੜਾ  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, RTLIL::Const ਵਿੱਥ, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Concat (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Lut    (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_y, RTLIL::Const lut, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Tribuf (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Assert (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Assume (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_ਜੀਵਤ   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Fair   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Cover  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Equiv  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_Sr    (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr, ਅਚਲ RTLIL::SigSpec &sig_q, bool ਸੈੱਟ_polarity = true, bool clr_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Ff    (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Dff   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_d,   ਅਚਲ RTLIL::SigSpec &sig_q, bool clk_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Dffe  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en,  ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Dffsr (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool ਸੈੱਟ_polarity = true, bool clr_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Dffsre (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool ਸੈੱਟ_polarity = true, bool clr_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Adff (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_arst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, RTLIL::Const arst_ਮੁੱਲ, bool clk_polarity = true, bool arst_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Adffe (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_arst,  ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, RTLIL::Const arst_ਮੁੱਲ, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Aldff (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_aload, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਅਚਲ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Aldffe (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_aload,  ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਅਚਲ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Sdff (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_srst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, RTLIL::Const srst_ਮੁੱਲ, bool clk_polarity = true, bool srst_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Sdffe (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_srst,  ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, RTLIL::Const srst_ਮੁੱਲ, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Sdffce (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_srst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, RTLIL::Const srst_ਮੁੱਲ, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Dlatch (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, bool en_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Adlatch (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_arst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, RTLIL::Const arst_ਮੁੱਲ, bool en_polarity = true, bool arst_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Dlatchsr (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, bool en_polarity = true, bool ਸੈੱਟ_polarity = true, bool clr_polarity = true, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_Buf_Gate    (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Not_Gate    (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_And_Gate    (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Nand_Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Or_Gate     (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Nor_Gate    (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Xor_Gate    (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Xnor_Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Andnot_Gate (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Ornot_Gate  (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Mux_Gate    (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_s, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Nmux_Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_s, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Aoi3_Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_c, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Oai3_Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_c, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Aoi4_Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_c, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_d, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Oai4_Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_c, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_d, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_y, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_Sr_Gate     (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr,
			ਅਚਲ RTLIL::SigSpec &sig_q, bool ਸੈੱਟ_polarity = true, bool clr_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Ff_Gate     (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Dff_Gate    (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, bool clk_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Dffe_Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Dffsr_Gate  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool ਸੈੱਟ_polarity = true, bool clr_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Dffsre_Gate (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool ਸੈੱਟ_polarity = true, bool clr_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Adff_Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_arst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
			bool arst_ਮੁੱਲ = false, bool clk_polarity = true, bool arst_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Adffe_Gate  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_arst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
			bool arst_ਮੁੱਲ = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Aldff_Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_aload, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
			ਅਚਲ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Aldffe_Gate  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_aload, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
			ਅਚਲ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Sdff_Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_srst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
			bool srst_ਮੁੱਲ = false, bool clk_polarity = true, bool srst_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Sdffe_Gate  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_srst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
			bool srst_ਮੁੱਲ = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Sdffce_Gate (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_clk, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_srst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
			bool srst_ਮੁੱਲ = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Dlatch_Gate (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, bool en_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Adlatch_Gate(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_arst, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q,
			bool arst_ਮੁੱਲ = false, bool en_polarity = true, bool arst_polarity = true, ਅਚਲ std::string &src = "");
	RTLIL::Cell* ਜੋੜੋ_Dlatchsr_Gate  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_en, ਅਚਲ RTLIL::SigSpec &sig_ਸੈੱਟ, ਅਚਲ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, bool en_polarity = true, bool ਸੈੱਟ_polarity = true, bool clr_polarity = true, ਅਚਲ std::string &src = "");

	RTLIL::Cell* ਜੋੜੋ_Anyinit(RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_d, ਅਚਲ RTLIL::SigSpec &sig_q, ਅਚਲ std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, bool is_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, bool is_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec ਖਿਸਕਾਓ_2  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::SigSpec ਜੋੜੋ_2 (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, bool is_signed = false, ਅਚਲ std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_b, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ std::string &src = "");

	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ BufGate    (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ NotGate    (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ AndGate    (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ NandGate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ OrGate     (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ NorGate    (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ XorGate    (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ XnorGate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ AndnotGate (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ OrnotGate  (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ MuxGate    (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_s, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ NmuxGate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_s, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ Aoi3Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_c, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ Oai3Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_c, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ Aoi4Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_c, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_d, ਅਚਲ std::string &src = "");
	RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ Oai4Gate   (RTLIL::IdString name, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_a, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_b, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_c, ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &sig_d, ਅਚਲ std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, ਪੂਰਨ ਚੌੜਾਈ = 1, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, ਪੂਰਨ ਚੌੜਾਈ = 1, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, ਪੂਰਨ ਚੌੜਾਈ = 1, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, ਪੂਰਨ ਚੌੜਾਈ = 1, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ਅਚਲ std::string &src = "");

	RTLIL::SigSpec ਸੈੱਟ_ਟੈਗ_2          (RTLIL::IdString name, ਅਚਲ std::string &ਟੈਗ, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ RTLIL::SigSpec &sig_c, ਅਚਲ std::string &src = "");
	RTLIL::Cell*   ਜੋੜੋ_ਸੈੱਟ_ਟੈਗ       (RTLIL::IdString name, ਅਚਲ std::string &ਟੈਗ, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ RTLIL::SigSpec &sig_c, ਅਚਲ RTLIL::SigSpec &sig_y, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Get_ਟੈਗ          (RTLIL::IdString name, ਅਚਲ std::string &ਟੈਗ, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ std::string &src = "");
	RTLIL::Cell*   ਜੋੜੋ_Overwrite_ਟੈਗ (RTLIL::IdString name, ਅਚਲ std::string &ਟੈਗ, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ RTLIL::SigSpec &sig_s, ਅਚਲ RTLIL::SigSpec &sig_c, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec Original_ਟੈਗ     (RTLIL::IdString name, ਅਚਲ std::string &ਟੈਗ, ਅਚਲ RTLIL::SigSpec &sig_a, ਅਚਲ std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ਅਚਲ RTLIL::SigSpec &sig_e, ਅਚਲ std::string &src = "");
	YS_NAME_FWD(ਜੋੜੋ_Anyinit)
	YS_NAME_FWD(Anyconst) YS_NAME_FWD(Anyseq) YS_NAME_FWD(Allconst) YS_NAME_FWD(Allseq)
	YS_NAME_FWD(Initstate)
	YS_NAME_FWD(ਸੈੱਟ_ਟੈਗ_2) YS_NAME_FWD(ਜੋੜੋ_ਸੈੱਟ_ਟੈਗ) YS_NAME_FWD(Get_ਟੈਗ)
	YS_NAME_FWD(ਜੋੜੋ_Overwrite_ਟੈਗ) YS_NAME_FWD(Original_ਟੈਗ) YS_NAME_FWD(FutureFF)
	YS_NAME_FWD(ਜੋੜੋ_Not) YS_NAME_FWD(ਜੋੜੋ_Pos) YS_NAME_FWD(ਜੋੜੋ_Buf) YS_NAME_FWD(ਜੋੜੋ_Barrier) YS_NAME_FWD(ਜੋੜੋ_Neg)
	YS_NAME_FWD(ਜੋੜੋ_And) YS_NAME_FWD(ਜੋੜੋ_Or) YS_NAME_FWD(ਜੋੜੋ_Xor) YS_NAME_FWD(ਜੋੜੋ_Xnor)
	YS_NAME_FWD(ਜੋੜੋ_Reduce_And) YS_NAME_FWD(ਜੋੜੋ_Reduce_Or) YS_NAME_FWD(ਜੋੜੋ_Reduce_Xor) YS_NAME_FWD(ਜੋੜੋ_Reduce_Xnor)
	YS_NAME_FWD(ਜੋੜੋ_Reduce_Bool) YS_NAME_FWD(ਜੋੜੋ_Shl) YS_NAME_FWD(ਜੋੜੋ_Shr) YS_NAME_FWD(ਜੋੜੋ_Sshl)
	YS_NAME_FWD(ਜੋੜੋ_Sshr) YS_NAME_FWD(ਜੋੜੋ_ਖਿਸਕਾਓ) YS_NAME_FWD(ਜੋੜੋ_Shiftx) YS_NAME_FWD(ਜੋੜੋ_Lt)
	YS_NAME_FWD(ਜੋੜੋ_Le) YS_NAME_FWD(ਜੋੜੋ_Eq) YS_NAME_FWD(ਜੋੜੋ_Ne) YS_NAME_FWD(ਜੋੜੋ_Eqx)
	YS_NAME_FWD(ਜੋੜੋ_Nex) YS_NAME_FWD(ਜੋੜੋ_Ge) YS_NAME_FWD(ਜੋੜੋ_Gt) YS_NAME_FWD(ਜੋੜੋ_ਜੋੜੋ)
	YS_NAME_FWD(ਜੋੜੋ_Sub) YS_NAME_FWD(ਜੋੜੋ_Mul) YS_NAME_FWD(ਜੋੜੋ_Div) YS_NAME_FWD(ਜੋੜੋ_Mod)
	YS_NAME_FWD(ਜੋੜੋ_Div_Floor) YS_NAME_FWD(ਜੋੜੋ_Mod_Floor) YS_NAME_FWD(ਜੋੜੋ_Pow) YS_NAME_FWD(ਜੋੜੋ_Fa)
	YS_NAME_FWD(ਜੋੜੋ_Logic_Not) YS_NAME_FWD(ਜੋੜੋ_Logic_And) YS_NAME_FWD(ਜੋੜੋ_Logic_Or) YS_NAME_FWD(ਜੋੜੋ_Mux)
	YS_NAME_FWD(ਜੋੜੋ_Pmux) YS_NAME_FWD(ਜੋੜੋ_Bmux) YS_NAME_FWD(ਜੋੜੋ_Demux) YS_NAME_FWD(ਜੋੜੋ_Bweqx)
	YS_NAME_FWD(ਜੋੜੋ_Bwmux) YS_NAME_FWD(ਜੋੜੋ_ਟੁਕੜਾ) YS_NAME_FWD(ਜੋੜੋ_Concat) YS_NAME_FWD(ਜੋੜੋ_Lut)
	YS_NAME_FWD(ਜੋੜੋ_Tribuf) YS_NAME_FWD(ਜੋੜੋ_Assert) YS_NAME_FWD(ਜੋੜੋ_Assume) YS_NAME_FWD(ਜੋੜੋ_ਜੀਵਤ)
	YS_NAME_FWD(ਜੋੜੋ_Fair) YS_NAME_FWD(ਜੋੜੋ_Cover) YS_NAME_FWD(ਜੋੜੋ_Equiv) YS_NAME_FWD(ਜੋੜੋ_Sr)
	YS_NAME_FWD(ਜੋੜੋ_Ff) YS_NAME_FWD(ਜੋੜੋ_Dff) YS_NAME_FWD(ਜੋੜੋ_Dffe) YS_NAME_FWD(ਜੋੜੋ_Dffsr)
	YS_NAME_FWD(ਜੋੜੋ_Dffsre) YS_NAME_FWD(ਜੋੜੋ_Adff) YS_NAME_FWD(ਜੋੜੋ_Adffe) YS_NAME_FWD(ਜੋੜੋ_Aldff)
	YS_NAME_FWD(ਜੋੜੋ_Aldffe) YS_NAME_FWD(ਜੋੜੋ_Sdff) YS_NAME_FWD(ਜੋੜੋ_Sdffe) YS_NAME_FWD(ਜੋੜੋ_Sdffce)
	YS_NAME_FWD(ਜੋੜੋ_Dlatch) YS_NAME_FWD(ਜੋੜੋ_Adlatch) YS_NAME_FWD(ਜੋੜੋ_Dlatchsr) YS_NAME_FWD(ਜੋੜੋ_Buf_Gate)
	YS_NAME_FWD(ਜੋੜੋ_Not_Gate) YS_NAME_FWD(ਜੋੜੋ_And_Gate) YS_NAME_FWD(ਜੋੜੋ_Nand_Gate) YS_NAME_FWD(ਜੋੜੋ_Or_Gate)
	YS_NAME_FWD(ਜੋੜੋ_Nor_Gate) YS_NAME_FWD(ਜੋੜੋ_Xor_Gate) YS_NAME_FWD(ਜੋੜੋ_Xnor_Gate) YS_NAME_FWD(ਜੋੜੋ_Andnot_Gate)
	YS_NAME_FWD(ਜੋੜੋ_Ornot_Gate) YS_NAME_FWD(ਜੋੜੋ_Mux_Gate) YS_NAME_FWD(ਜੋੜੋ_Nmux_Gate) YS_NAME_FWD(ਜੋੜੋ_Aoi3_Gate)
	YS_NAME_FWD(ਜੋੜੋ_Oai3_Gate) YS_NAME_FWD(ਜੋੜੋ_Aoi4_Gate) YS_NAME_FWD(ਜੋੜੋ_Oai4_Gate) YS_NAME_FWD(ਜੋੜੋ_Sr_Gate)
	YS_NAME_FWD(ਜੋੜੋ_Ff_Gate) YS_NAME_FWD(ਜੋੜੋ_Dff_Gate) YS_NAME_FWD(ਜੋੜੋ_Dffe_Gate) YS_NAME_FWD(ਜੋੜੋ_Dffsr_Gate)
	YS_NAME_FWD(ਜੋੜੋ_Dffsre_Gate) YS_NAME_FWD(ਜੋੜੋ_Adff_Gate) YS_NAME_FWD(ਜੋੜੋ_Adffe_Gate) YS_NAME_FWD(ਜੋੜੋ_Aldff_Gate)
	YS_NAME_FWD(ਜੋੜੋ_Aldffe_Gate) YS_NAME_FWD(ਜੋੜੋ_Sdff_Gate) YS_NAME_FWD(ਜੋੜੋ_Sdffe_Gate) YS_NAME_FWD(ਜੋੜੋ_Sdffce_Gate)
	YS_NAME_FWD(ਜੋੜੋ_Dlatch_Gate) YS_NAME_FWD(ਜੋੜੋ_Adlatch_Gate) YS_NAME_FWD(ਜੋੜੋ_Dlatchsr_Gate) YS_NAME_FWD(Not)
	YS_NAME_FWD(Pos) YS_NAME_FWD(Buf) YS_NAME_FWD(Barrier) YS_NAME_FWD(Neg) YS_NAME_FWD(And)
	YS_NAME_FWD(Or) YS_NAME_FWD(Xor) YS_NAME_FWD(Xnor) YS_NAME_FWD(ReduceAnd)
	YS_NAME_FWD(ReduceOr) YS_NAME_FWD(ReduceXor) YS_NAME_FWD(ReduceXnor) YS_NAME_FWD(ReduceBool)
	YS_NAME_FWD(Shl) YS_NAME_FWD(Shr) YS_NAME_FWD(Sshl) YS_NAME_FWD(Sshr)
	YS_NAME_FWD(ਖਿਸਕਾਓ_2) YS_NAME_FWD(Shiftx) YS_NAME_FWD(Lt) YS_NAME_FWD(Le)
	YS_NAME_FWD(Eq) YS_NAME_FWD(Ne) YS_NAME_FWD(Eqx) YS_NAME_FWD(Nex)
	YS_NAME_FWD(Ge) YS_NAME_FWD(Gt) YS_NAME_FWD(ਜੋੜੋ_2) YS_NAME_FWD(Sub)
	YS_NAME_FWD(Mul) YS_NAME_FWD(Div) YS_NAME_FWD(Mod) YS_NAME_FWD(DivFloor)
	YS_NAME_FWD(ModFloor) YS_NAME_FWD(Pow) YS_NAME_FWD(LogicNot) YS_NAME_FWD(LogicAnd)
	YS_NAME_FWD(LogicOr) YS_NAME_FWD(Mux) YS_NAME_FWD(Pmux) YS_NAME_FWD(Bmux)
	YS_NAME_FWD(Demux) YS_NAME_FWD(Bweqx) YS_NAME_FWD(Bwmux) YS_NAME_FWD(BufGate)
	YS_NAME_FWD(NotGate) YS_NAME_FWD(AndGate) YS_NAME_FWD(NandGate) YS_NAME_FWD(OrGate)
	YS_NAME_FWD(NorGate) YS_NAME_FWD(XorGate) YS_NAME_FWD(XnorGate) YS_NAME_FWD(AndnotGate)
	YS_NAME_FWD(OrnotGate) YS_NAME_FWD(MuxGate) YS_NAME_FWD(NmuxGate) YS_NAME_FWD(Aoi3Gate)
	YS_NAME_FWD(Oai3Gate) YS_NAME_FWD(Aoi4Gate) YS_NAME_FWD(Oai4Gate)

	std::string ਮੰਜ਼ਿਲ_rtlil_str() ਅਚਲ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	ਸਥਿਰ std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Module*> *get_all_modules(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
};

ਬਣਤਰ RTLIL::Wire : ਜਨਤਕ RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ { h.eat(hashidx_); ਵਾਪਸ h; }

ਸੁਰੱਖਿਅਤ:
	// use module->addWire() and module->remove() to create or destroy wires
	ਮਿੱਤਰ ਬਣਤਰ RTLIL::Module;
	Wire();
	~Wire();

	ਮਿੱਤਰ ਬਣਤਰ RTLIL::Design;
	ਮਿੱਤਰ ਬਣਤਰ RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driver_ਪੋਰਟ_2;

ਜਨਤਕ:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	ਖਾਲੀ ਸੰਚਾਲਕ=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	ਪੂਰਨ ਚੌੜਾਈ, ਸ਼ੁਰੂ_ਵਿੱਥ, ਪੋਰਟ_id;
	bool ਪੋਰਟ_ਨਿਵੇਸ਼, ਪੋਰਟ_output, upto, is_signed;

	RTLIL::Design *design() ਅਚਲ { ਵਾਪਸ module ? module->design : nullptr; }
	TwinePool &twines() ਅਚਲ { ਵਾਪਸ module->twines(); }

	bool ਗਿਆਤ_driver() ਅਚਲ { ਵਾਪਸ driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ਅਚਲ    { log_assert(driverCell_); ਵਾਪਸ driverCell_; };
	RTLIL::IdString driver_ਪੋਰਟ() ਅਚਲ { log_assert(driverCell_); ਵਾਪਸ driver_ਪੋਰਟ_2; };

	ਪੂਰਨ from_hdl_ਸੂਚਕਾਂਕ(ਪੂਰਨ hdl_ਸੂਚਕਾਂਕ) {
		ਪੂਰਨ zero_ਸੂਚਕਾਂਕ = hdl_ਸੂਚਕਾਂਕ - ਸ਼ੁਰੂ_ਵਿੱਥ;
		ਪੂਰਨ rtlil_ਸੂਚਕਾਂਕ = upto ? ਚੌੜਾਈ - 1 - zero_ਸੂਚਕਾਂਕ : zero_ਸੂਚਕਾਂਕ;
		ਵਾਪਸ rtlil_ਸੂਚਕਾਂਕ >= 0 && rtlil_ਸੂਚਕਾਂਕ < ਚੌੜਾਈ ? rtlil_ਸੂਚਕਾਂਕ : INT_MIN;
	}

	ਪੂਰਨ ਮੰਜ਼ਿਲ_hdl_ਸੂਚਕਾਂਕ(ਪੂਰਨ rtlil_ਸੂਚਕਾਂਕ) {
		ਜੇ (rtlil_ਸੂਚਕਾਂਕ < 0 || rtlil_ਸੂਚਕਾਂਕ >= ਚੌੜਾਈ)
			ਵਾਪਸ INT_MIN;
		ਪੂਰਨ zero_ਸੂਚਕਾਂਕ = upto ? ਚੌੜਾਈ - 1 - rtlil_ਸੂਚਕਾਂਕ : rtlil_ਸੂਚਕਾਂਕ;
		ਵਾਪਸ zero_ਸੂਚਕਾਂਕ + ਸ਼ੁਰੂ_ਵਿੱਥ;
	}

	std::string ਮੰਜ਼ਿਲ_rtlil_str() ਅਚਲ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	ਸਥਿਰ std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Wire*> *get_all_wires(ਖਾਲੀ);
#ਜੇ_ਅੰਤ
};

ਸਤਰਅੰਦਰ ਪੂਰਨ Get_ਮਾਪ(RTLIL::Wire *wire) {
	ਵਾਪਸ wire->ਚੌੜਾਈ;
}

ਬਣਤਰ RTLIL::ਸਮ੍ਰਿਤੀ : ਜਨਤਕ RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ { h.eat(hashidx_); ਵਾਪਸ h; }

	ਸਮ੍ਰਿਤੀ();
	~ਸਮ੍ਰਿਤੀ();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() ਅਚਲ { ਵਾਪਸ module ? module->design : nullptr; }
	TwinePool &twines() ਅਚਲ { ਵਾਪਸ module->twines(); }

	RTLIL::ਸਮ੍ਰਿਤੀ_Name_Id name;

	ਪੂਰਨ ਚੌੜਾਈ, ਸ਼ੁਰੂ_ਵਿੱਥ, ਮਾਪ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	ਸਥਿਰ std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::ਸਮ੍ਰਿਤੀ*> *get_all_memorys(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

	std::string ਮੰਜ਼ਿਲ_rtlil_str() ਅਚਲ;
};

ਬਣਤਰ RTLIL::Cell : ਜਨਤਕ RTLIL::AttrObject
{
ਨਿਜੀ:
	ਖਾਲੀ init_ਸੂਚਕਾਂਕ();

	bool bufnorm_handle_ਸੈੱਟ_ਪੋਰਟ(IdString portname, RTLIL::SigSpec &ਸੰਕੇਤ, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
ਜਨਤਕ:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ { h.eat(hashidx_); ਵਾਪਸ h; }

ਸੁਰੱਖਿਅਤ:
	// use module->addCell() and module->remove() to create or destroy cells
	ਮਿੱਤਰ ਬਣਤਰ RTLIL::Module;
	Cell();
	~Cell();

ਜਨਤਕ:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	ਖਾਲੀ ਸੰਚਾਲਕ=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() ਅਚਲ { ਵਾਪਸ module ? module->design : nullptr; }
	TwinePool &twines() ਅਚਲ { ਵਾਪਸ module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool has_ਪੋਰਟ(RTLIL::IdString portname) ਅਚਲ;
	ਖਾਲੀ unset_ਪੋਰਟ(RTLIL::IdString portname);
	ਖਾਲੀ ਸੈੱਟ_ਪੋਰਟ(RTLIL::IdString portname, RTLIL::SigSpec ਸੰਕੇਤ);
	ਸਾਂਚਾ<typename N, YS_NAME_STRING(N)> ਖਾਲੀ ਸੈੱਟ_ਪੋਰਟ(N portname, RTLIL::SigSpec ਸੰਕੇਤ)
		{ ਸੈੱਟ_ਪੋਰਟ(module->twines().ਜੋੜੋ(std::move(portname)), std::move(ਸੰਕੇਤ)); }
	ਅਚਲ RTLIL::SigSpec &get_ਪੋਰਟ(RTLIL::IdString portname) ਅਚਲ;
	ਅਚਲ dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ਅਚਲ;

	bool ਗਿਆਤ() ਅਚਲ;
	bool ਨਿਵੇਸ਼(RTLIL::IdString portname) ਅਚਲ;
	bool output(RTLIL::IdString portname) ਅਚਲ;
	ਪੋਰਟ_Dir ਪੋਰਟ_dir(RTLIL::IdString portname) ਅਚਲ;

	bool hasParam(RTLIL::IdString paramname) ਅਚਲ;
	ਖਾਲੀ unsetParam(RTLIL::IdString paramname);
	ਖਾਲੀ ਸੈੱਟ_Param(RTLIL::IdString paramname, RTLIL::Const ਮੁੱਲ);
	ਅਚਲ RTLIL::Const &getParam(RTLIL::IdString paramname) ਅਚਲ;

	ਸਾਂਚਾ<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ਅਚਲ
		{ ਵਾਪਸ hasParam(module->twines().ਲੱਭੋ(std::string_view(name))); }
	ਸਾਂਚਾ<typename N, YS_NAME_STRING(N)> ਖਾਲੀ unsetParam(N name)
		{ unsetParam(module->twines().ਲੱਭੋ(std::string_view(name))); }
	ਸਾਂਚਾ<typename N, YS_NAME_STRING(N)> ਖਾਲੀ ਸੈੱਟ_Param(N name, RTLIL::Const ਮੁੱਲ)
		{ ਸੈੱਟ_Param(module->twines().ਜੋੜੋ(std::move(name)), std::move(ਮੁੱਲ)); }
	ਸਾਂਚਾ<typename N, YS_NAME_STRING(N)> ਅਚਲ RTLIL::Const &getParam(N name) ਅਚਲ
		{ ਵਾਪਸ getParam(module->twines().ਲੱਭੋ(std::string_view(name))); }

	ਖਾਲੀ ਛਾਂਟੋ();
	ਖਾਲੀ check();
	ਖਾਲੀ fixup_parameters(bool ਸੈੱਟ_a_signed = false, bool ਸੈੱਟ_b_signed = false);

	bool has_keep_attr() ਅਚਲ;

	ਸਾਂਚਾ<typename T> ਖਾਲੀ rewrite_sigspecs(T &functor);
	ਸਾਂਚਾ<typename T> ਖਾਲੀ rewrite_sigspecs2(T &functor);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ YOSYS_ENABLE_PYTHON
	ਸਥਿਰ std::map<ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ, RTLIL::Cell*> *get_all_cells(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

	bool has_memid() ਅਚਲ;
	bool is_mem_cell() ਅਚਲ;
	bool is_builtin_ff() ਅਚਲ;

	std::string ਮੰਜ਼ਿਲ_rtlil_str() ਅਚਲ;
};

ਬਣਤਰ RTLIL::Case_ਨਿਯਮ : ਜਨਤਕ RTLIL::AttrObject
{
	std::ਸਦਿਸ਼<RTLIL::SigSpec> compare;
	std::ਸਦਿਸ਼<RTLIL::SigSig> actions;
	std::ਸਦਿਸ਼<RTLIL::Switch_ਨਿਯਮ*> switches;

	~Case_ਨਿਯਮ();

	bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ;

	ਸਾਂਚਾ<typename T> ਖਾਲੀ rewrite_sigspecs(T &functor);
	ਸਾਂਚਾ<typename T> ਖਾਲੀ rewrite_sigspecs2(T &functor);
	RTLIL::Case_ਨਿਯਮ *clone() ਅਚਲ;
};

ਬਣਤਰ RTLIL::Switch_ਨਿਯਮ : ਜਨਤਕ RTLIL::AttrObject
{
	RTLIL::SigSpec ਸੰਕੇਤ;
	std::ਸਦਿਸ਼<RTLIL::Case_ਨਿਯਮ*> cases;

	~Switch_ਨਿਯਮ();

	bool ਖਾਲੀ_ਥਾਵਾਂ() ਅਚਲ;

	ਸਾਂਚਾ<typename T> ਖਾਲੀ rewrite_sigspecs(T &functor);
	ਸਾਂਚਾ<typename T> ਖਾਲੀ rewrite_sigspecs2(T &functor);
	RTLIL::Switch_ਨਿਯਮ *clone() ਅਚਲ;
};

ਬਣਤਰ RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec ਪਤਾ;
	RTLIL::SigSpec ਅੰਕੜੇ;
	RTLIL::SigSpec enable;
	RTLIL::Const ਤਰਜੀਹ_ਨਕਾਬ;
};

ਬਣਤਰ RTLIL::Sync_ਨਿਯਮ
{
	RTLIL::SyncType type;
	RTLIL::SigSpec ਸੰਕੇਤ;
	std::ਸਦਿਸ਼<RTLIL::SigSig> actions;
	std::ਸਦਿਸ਼<RTLIL::MemWriteAction> mem_write_actions;

	ਸਾਂਚਾ<typename T> ਖਾਲੀ rewrite_sigspecs(T &functor);
	ਸਾਂਚਾ<typename T> ਖਾਲੀ rewrite_sigspecs2(T &functor);
	RTLIL::Sync_ਨਿਯਮ *clone() ਅਚਲ;
};

ਬਣਤਰ RTLIL::Process : ਜਨਤਕ RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ { h.eat(hashidx_); ਵਾਪਸ h; }

ਸੁਰੱਖਿਅਤ:
	// use module->addProcess() and module->remove() to create or destroy processes
	ਮਿੱਤਰ ਬਣਤਰ RTLIL::Module;
	Process();
	~Process();

ਜਨਤਕ:
	RTLIL::Module *module;
	RTLIL::Case_ਨਿਯਮ ਜੜ੍ਹ_case;
	std::ਸਦਿਸ਼<RTLIL::Sync_ਨਿਯਮ*> syncs;

	RTLIL::Design *design() ਅਚਲ { ਵਾਪਸ module ? module->design : nullptr; }
	TwinePool &twines() ਅਚਲ { ਵਾਪਸ module->twines(); }

	ਸਾਂਚਾ<typename T> ਖਾਲੀ rewrite_sigspecs(T &functor);
	ਸਾਂਚਾ<typename T> ਖਾਲੀ rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ਅਚਲ;

	std::string ਮੰਜ਼ਿਲ_rtlil_str() ਅਚਲ;
};

ਸਤਰਅੰਦਰ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ() : wire(ਨਿਰਾ), ਅੰਕੜੇ(RTLIL::State::S0) { }
ਸਤਰਅੰਦਰ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(RTLIL::State ਦੋਆਧਾਰੀ_ਅੰਕ) : wire(ਨਿਰਾ), ਅੰਕੜੇ(ਦੋਆਧਾਰੀ_ਅੰਕ) { }
ਸਤਰਅੰਦਰ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(bool ਦੋਆਧਾਰੀ_ਅੰਕ) : wire(ਨਿਰਾ), ਅੰਕੜੇ(ਦੋਆਧਾਰੀ_ਅੰਕ ? State::S1 : State::S0) { }
ਸਤਰਅੰਦਰ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(RTLIL::Wire *wire) : wire(wire), ਵਿੱਥ(0) { log_assert(wire && wire->ਚੌੜਾਈ == 1); }
ਸਤਰਅੰਦਰ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(RTLIL::Wire *wire, ਪੂਰਨ ਵਿੱਥ) : wire(wire), ਵਿੱਥ(ਵਿੱਥ) { log_assert(wire != nullptr); }
ਸਤਰਅੰਦਰ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.ਚੌੜਾਈ == 1); ਜੇ (wire) ਵਿੱਥ = chunk.ਵਿੱਥ; ਨਹੀਂਤਾਂ ਅੰਕੜੇ = chunk.ਅੰਕੜੇ[0]; }
ਸਤਰਅੰਦਰ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ RTLIL::SigChunk &chunk, ਪੂਰਨ ਸੂਚਕਾਂਕ) : wire(chunk.wire) { ਜੇ (wire) ਵਿੱਥ = chunk.ਵਿੱਥ + ਸੂਚਕਾਂਕ; ਨਹੀਂਤਾਂ ਅੰਕੜੇ = chunk.ਅੰਕੜੇ[ਸੂਚਕਾਂਕ]; }

ਸਤਰਅੰਦਰ bool RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ::ਸੰਚਾਲਕ<(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &other) ਅਚਲ {
	ਜੇ (wire == other.wire)
		ਵਾਪਸ wire ? (ਵਿੱਥ < other.ਵਿੱਥ) : (ਅੰਕੜੇ < other.ਅੰਕੜੇ);
	ਜੇ (wire != nullptr && other.wire != nullptr)
		ਵਾਪਸ wire->name < other.wire->name;
	ਵਾਪਸ (wire != nullptr) < (other.wire != nullptr);
}

ਸਤਰਅੰਦਰ bool RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ::ਸੰਚਾਲਕ==(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &other) ਅਚਲ {
	ਵਾਪਸ (wire == other.wire) && (wire ? (ਵਿੱਥ == other.ਵਿੱਥ) : (ਅੰਕੜੇ == other.ਅੰਕੜੇ));
}

ਸਤਰਅੰਦਰ bool RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ::ਸੰਚਾਲਕ!=(ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &other) ਅਚਲ {
	ਵਾਪਸ (wire != other.wire) || (wire ? (ਵਿੱਥ != other.ਵਿੱਥ) : (ਅੰਕੜੇ != other.ਅੰਕੜੇ));
}

ਸਤਰਅੰਦਰ Hasher RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ::hash_into(Hasher h) ਅਚਲ {
	ਜੇ (wire) {
		h.eat(ਵਿੱਥ);
		h.eat(wire->name);
		ਵਾਪਸ h;
	}
	h.eat(ਅੰਕੜੇ);
	ਵਾਪਸ h;
}


ਸਤਰਅੰਦਰ Hasher RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ::hash_ਸਿਖਰ() ਅਚਲ {
	Hasher h;
	ਜੇ (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.ਬਿੱਟ();
		h.force(hashlib::legacy::djb2_ਜੋੜੋ(n, ਵਿੱਥ));
		ਵਾਪਸ h;
	}
	h.force(ਅੰਕੜੇ);
	ਵਾਪਸ h;
}

ਸਤਰਅੰਦਰ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &RTLIL::SigSpecIterator::ਸੰਚਾਲਕ*() ਅਚਲ {
	ਵਾਪਸ (*sig_p)[ਸੂਚਕਾਂਕ];
}

ਸਤਰਅੰਦਰ ਅਚਲ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ &RTLIL::SigSpecConstIterator::ਸੰਚਾਲਕ*() {
	ਦੋਆਧਾਰੀ_ਅੰਕ = (*sig_p)[ਸੂਚਕਾਂਕ];
	ਵਾਪਸ ਦੋਆਧਾਰੀ_ਅੰਕ;
}

ਸਤਰਅੰਦਰ RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਚਲ RTLIL::SigSpec &sig) {
	log_assert(sig.ਮਾਪ() == 1);
	ਸਵੈਚਾਲਿਤ it = sig.chunks().begin();
	*this = Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(*it);
}

ਸਾਂਚਾ<typename T>
ਖਾਲੀ RTLIL::Module::rewrite_sigspecs(T &functor)
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : cells_)
		it.ਦੂਜਾ->rewrite_sigspecs(functor);
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : processes)
		it.ਦੂਜਾ->rewrite_sigspecs(functor);
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : connections_) {
		functor(it.ਪਹਿਲਾ);
		functor(it.ਦੂਜਾ);
	}
}

ਸਾਂਚਾ<typename T>
ਖਾਲੀ RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : cells_)
		it.ਦੂਜਾ->rewrite_sigspecs2(functor);
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : processes)
		it.ਦੂਜਾ->rewrite_sigspecs2(functor);
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : connections_) {
		functor(it.ਪਹਿਲਾ, it.ਦੂਜਾ);
	}
}

ਸਾਂਚਾ<typename T>
ਖਾਲੀ RTLIL::Cell::rewrite_sigspecs(T &functor) {
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : connections_)
		functor(it.ਦੂਜਾ);
}

ਸਾਂਚਾ<typename T>
ਖਾਲੀ RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : connections_)
		functor(it.ਦੂਜਾ);
}

ਸਾਂਚਾ<typename T>
ਖਾਲੀ RTLIL::Case_ਨਿਯਮ::rewrite_sigspecs(T &functor) {
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : compare)
		functor(it);
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : actions) {
		functor(it.ਪਹਿਲਾ);
		functor(it.ਦੂਜਾ);
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ it : switches)
		it->rewrite_sigspecs(functor);
}

ਸਾਂਚਾ<typename T>
ਖਾਲੀ RTLIL::Case_ਨਿਯਮ::rewrite_sigspecs2(T &functor) {
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : compare)
		functor(it);
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : actions) {
		functor(it.ਪਹਿਲਾ, it.ਦੂਜਾ);
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ it : switches)
		it->rewrite_sigspecs2(functor);
}

ਸਾਂਚਾ<typename T>
ਖਾਲੀ RTLIL::Switch_ਨਿਯਮ::rewrite_sigspecs(T &functor)
{
	functor(ਸੰਕੇਤ);
	ਲਈ (ਸਵੈਚਾਲਿਤ it : cases)
		it->rewrite_sigspecs(functor);
}

ਸਾਂਚਾ<typename T>
ਖਾਲੀ RTLIL::Switch_ਨਿਯਮ::rewrite_sigspecs2(T &functor)
{
	functor(ਸੰਕੇਤ);
	ਲਈ (ਸਵੈਚਾਲਿਤ it : cases)
		it->rewrite_sigspecs2(functor);
}

ਸਾਂਚਾ<typename T>
ਖਾਲੀ RTLIL::Sync_ਨਿਯਮ::rewrite_sigspecs(T &functor)
{
	functor(ਸੰਕੇਤ);
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : actions) {
		functor(it.ਪਹਿਲਾ);
		functor(it.ਦੂਜਾ);
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : mem_write_actions) {
		functor(it.ਪਤਾ);
		functor(it.ਅੰਕੜੇ);
		functor(it.enable);
	}
}

ਸਾਂਚਾ<typename T>
ਖਾਲੀ RTLIL::Sync_ਨਿਯਮ::rewrite_sigspecs2(T &functor)
{
	functor(ਸੰਕੇਤ);
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : actions) {
		functor(it.ਪਹਿਲਾ, it.ਦੂਜਾ);
	}
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : mem_write_actions) {
		functor(it.ਪਤਾ);
		functor(it.ਅੰਕੜੇ);
		functor(it.enable);
	}
}

ਸਾਂਚਾ<typename T>
ਖਾਲੀ RTLIL::Process::rewrite_sigspecs(T &functor)
{
	ਜੜ੍ਹ_case.rewrite_sigspecs(functor);
	ਲਈ (ਸਵੈਚਾਲਿਤ it : syncs)
		it->rewrite_sigspecs(functor);
}

ਸਾਂਚਾ<typename T>
ਖਾਲੀ RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	ਜੜ੍ਹ_case.rewrite_sigspecs2(functor);
	ਲਈ (ਸਵੈਚਾਲਿਤ it : syncs)
		it->rewrite_sigspecs2(functor);
}

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
