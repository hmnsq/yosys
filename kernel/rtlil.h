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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RTLIL_H
#ವ್ಯಾಖ್ಯಾನಿಸು RTLIL_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"
#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/twine.h"

#ಅನ್ತರ್ಭಾವ <deque>
#ಅನ್ತರ್ಭಾವ <string_view>
#ಅನ್ತರ್ಭಾವ <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	ಎಣಿಕೆ State : ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	ಎಣಿಕೆ SyncType : ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ {
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
	ಎಣಿಕೆ ConstFlags : ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	ಎಣಿಕೆ SelectPartials : ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ {
		SELECT_ALL = 0,          // include partial modules
		SELECT_ಸಂಪೂರ್ಣ_ONLY = 1,   // ignore partial modules
		SELECT_ಸಂಪೂರ್ಣ_WARN = 2,   // call log_warning on partial module
		SELECT_ಸಂಪೂರ್ಣ_ERR = 3,    // call log_error on partial module
		SELECT_ಸಂಪೂರ್ಣ_CMDERR = 4  // call log_cmd_error on partial module
	};

	ಎಣಿಕೆ SelectBoxes : ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ {
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

	ಎಣಿಕೆ ಪೋರ್ಟ್_Dir : ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ  {
		PD_UNKNOWN = 0,
		PD_ಒಳಹರಿವು = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr ಪೂರ್ಣಾಂಕ ಅಗಲ_ಎಲ್ಲೆ = 1 << 30;

	ರಚನೆ Const;
	ರಚನೆ AttrObject;
	ರಚನೆ SelectedMember;
	ರಚನೆ Selection;
	ರಚನೆ Monitor;
	ರಚನೆ Design;
	ರಚನೆ Module;
	ರಚನೆ Wire;
	ರಚನೆ ಸ್ಮರಣೆ;
	ರಚನೆ Cell;
	ರಚನೆ SigChunk;
	ರಚನೆ Sig_ಬಿಟ್;
	ರಚನೆ SigSpecIterator;
	ರಚನೆ SigSpecConstIterator;
	ರಚನೆ SigSpec;
	ರಚನೆ Case_ನಿಯಮ;
	ರಚನೆ Switch_ನಿಯಮ;
	ರಚನೆ MemWriteAction;
	ರಚನೆ Sync_ನಿಯಮ;
	ರಚನೆ Process;

	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::pair<SigSpec, SigSpec> SigSig;
};

ರಚನೆ SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ ID; }
namespace RTLIL { using YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ IdString; }

namespace RTLIL {
	ಶೂನ್ಯ copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			ಸ್ಥಿರಾಂಕ dict<IdString, RTLIL::Const> &src,
			ಸ್ಥಿರಾಂಕ RTLIL::Design *src_design, RTLIL::Design *dst_design);

	ಬಾಹ್ಯ dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	ಸ್ಥಿರಾಂಕ pool<IdString> &builtin_ff_cell_types();

	ಸ್ಥಿರ ಸಾಲೊಳಗೆ std::string escape_id(ಸ್ಥಿರಾಂಕ std::string &str) {
		ಆದರೆ (str.ಗಾತ್ರ_2() > 0 && str[0] != '\\' && str[0] != '$')
			ಹಿಂತಿರುಗಿ "\\" + str;
		ಹಿಂತಿರುಗಿ str;
	}

	ಸ್ಥಿರ ಸಾಲೊಳಗೆ std::string unescape_id(ಸ್ಥಿರಾಂಕ std::string &str) {
		ಆದರೆ (str.ಗಾತ್ರ_2() < 2)
			ಹಿಂತಿರುಗಿ str;
		ಆದರೆ (str[0] != '\\')
			ಹಿಂತಿರುಗಿ str;
		ಆದರೆ (str[1] == '$' || str[1] == '\\')
			ಹಿಂತಿರುಗಿ str;
		ಆದರೆ (str[1] >= '0' && str[1] <= '9')
			ಹಿಂತಿರುಗಿ str;
		ಹಿಂತಿರುಗಿ str.substr(1);
	}

	ಮಾದರಿ <typename T> using ವಿಂಗಡಿಸು_by_name_id = IdString::compare_ptr_by_name<T>;

	ಮಾದರಿ <typename T> ರಚನೆ ವಿಂಗಡಿಸು_by_name_str {
		bool ಕ್ರಿಯಾಕಾರಕ()(T *a, T *b) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ a->name.lt_by_name(b->name);
		}
	};

	ರಚನೆ ವಿಂಗಡಿಸು_by_id_str {
		ಸ್ಥಿರಾಂಕ TwinePool& pool;
		explicit ವಿಂಗಡಿಸು_by_id_str(ಸ್ಥಿರಾಂಕ TwinePool& pool) : pool(pool) {}
		bool ಕ್ರಿಯಾಕಾರಕ()(RTLIL::IdString a, RTLIL::IdString b) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ pool.compare_by_name(a, b) < 0;
		}
	};

	ಸ್ಥಿರ ಸಾಲೊಳಗೆ std::string encode_filename(ಸ್ಥಿರಾಂಕ std::string &filename)
	{
		std::stringstream val;
		ಆದರೆ (!std::any_ದ(filename.begin(), filename.ಕೊನೆ(), [](ಅಕ್ಷರ c) {
			ಹಿಂತಿರುಗಿ static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>(c) < 33 || static_cast<ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ>(c) > 126;
		})) ಹಿಂತಿರುಗಿ filename;
		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ c : filename) {
			ಆದರೆ (c < 33 || c > 126)
				val << stringf("$%02x", c);
			ಇಲ್ಲದಿದ್ದರೆ
				val << c;
		}
		ಹಿಂತಿರುಗಿ val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_and         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_or          (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_xor         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_xnor        (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);

	RTLIL::Const const_reduce_and  (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_reduce_or   (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_reduce_xor  (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_reduce_xnor (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_reduce_bool (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);

	RTLIL::Const const_logic_not   (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_logic_and   (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_logic_or    (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);

	RTLIL::Const const_shl         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_shr         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_sshl        (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_sshr        (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_ಸರಿಸು       (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_shiftx      (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);

	RTLIL::Const const_lt          (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_le          (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_eq          (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_ne          (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_eqx         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_nex         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_ge          (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_gt          (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);

	RTLIL::Const const_ಕೂಡಿಸು         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_sub         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_mul         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_div         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_divfloor    (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_modfloor    (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_mod         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_pow         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);

	RTLIL::Const const_pos         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_buf         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);
	RTLIL::Const const_neg         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len);

	RTLIL::Const const_mux         (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	ಮಾದರಿ<typename T, typename ಕೀಲಿ_2 = IdString>
	ರಚನೆ ObjIterator {
		using iterator_category = std::forward_iterator_ಟ್ಯಾಗ್;
		using ಮೌಲ್ಯ_type = T;
		using difference_type = ptrdiff_t;
		using ಸೂಚಕ = T*;
		using reference = T&;
		typename dict<ಕೀಲಿ_2, T>::iterator it;
		dict<ಕೀಲಿ_2, T> *list_p;
		ಪೂರ್ಣಾಂಕ *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, ಪೂರ್ಣಾಂಕ *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			ಆದರೆ (list_p->ಖಾಲಿಗಳು()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} ಇಲ್ಲದಿದ್ದರೆ {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(ಸ್ಥಿರಾಂಕ RTLIL::ObjIterator<T, ಕೀಲಿ_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			ಆದರೆ (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ RTLIL::ObjIterator<T, ಕೀಲಿ_2> &other) {
			ಆದರೆ (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			ಆದರೆ (refcount_p)
				(*refcount_p)++;
			ಹಿಂತಿರುಗಿ *this;
		}

		~ObjIterator() {
			ಆದರೆ (refcount_p)
				(*refcount_p)--;
		}

		ಸಾಲೊಳಗೆ T ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ {
			log_assert(list_p != nullptr);
			ಹಿಂತಿರುಗಿ it->ಎರಡನೇ;
		}

		ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ RTLIL::ObjIterator<T, ಕೀಲಿ_2> &other) ಸ್ಥಿರಾಂಕ {
			ಆದರೆ (list_p == nullptr || other.list_p == nullptr)
				ಹಿಂತಿರುಗಿ list_p != other.list_p;
			ಹಿಂತಿರುಗಿ it != other.it;
		}


		ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ RTLIL::ObjIterator<T, ಕೀಲಿ_2> &other) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ !(*this != other);
		}

		ಸಾಲೊಳಗೆ ObjIterator<T, ಕೀಲಿ_2>& ಕ್ರಿಯಾಕಾರಕ++() {
			log_assert(list_p != nullptr);
			ಆದರೆ (++it == list_p->ಕೊನೆ()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			ಹಿಂತಿರುಗಿ *this;
		}

		ಸಾಲೊಳಗೆ ObjIterator<T, ಕೀಲಿ_2>& ಕ್ರಿಯಾಕಾರಕ+=(ಪೂರ್ಣಾಂಕ amt) {
			log_assert(list_p != nullptr);
			it += amt;
			ಆದರೆ (it == list_p->ಕೊನೆ()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			ಹಿಂತಿರುಗಿ *this;
		}

		ಸಾಲೊಳಗೆ ObjIterator<T, ಕೀಲಿ_2> ಕ್ರಿಯಾಕಾರಕ+(ಪೂರ್ಣಾಂಕ amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, ಕೀಲಿ_2> new_obj(*this);
			new_obj.it += amt;
			ಆದರೆ (new_obj.it == list_p->ಕೊನೆ()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			ಹಿಂತಿರುಗಿ new_obj;
		}

		ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ObjIterator<T, ಕೀಲಿ_2> ಕ್ರಿಯಾಕಾರಕ++(ಪೂರ್ಣಾಂಕ) {
			ObjIterator<T, ಕೀಲಿ_2> ಫಲಿತಾಂಶ(*this);
			++(*this);
			ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
		}
	};

	ಮಾದರಿ<typename T, typename ಕೀಲಿ_2 = IdString>
	ರಚನೆ ObjRange
	{
		dict<ಕೀಲಿ_2, T> *list_p;
		ಪೂರ್ಣಾಂಕ *refcount_p;

		ObjRange(decltype(list_p) list_p, ಪೂರ್ಣಾಂಕ *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, ಕೀಲಿ_2> begin() { ಹಿಂತಿರುಗಿ RTLIL::ObjIterator<T, ಕೀಲಿ_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, ಕೀಲಿ_2> ಕೊನೆ() { ಹಿಂತಿರುಗಿ RTLIL::ObjIterator<T, ಕೀಲಿ_2>(); }

		ಗಾತ್ರ_t ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ list_p->ಗಾತ್ರ_2();
		}

		ಕ್ರಿಯಾಕಾರಕ pool<T>() ಸ್ಥಿರಾಂಕ {
			pool<T> ಫಲಿತಾಂಶ;
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : *list_p)
				ಫಲಿತಾಂಶ.ಸೇರಿಸು(it.ಎರಡನೇ);
			ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
		}

		ಕ್ರಿಯಾಕಾರಕ std::ಸದಿಶ<T>() ಸ್ಥಿರಾಂಕ {
			std::ಸದಿಶ<T> ಫಲಿತಾಂಶ;
			ಫಲಿತಾಂಶ.reserve(list_p->ಗಾತ್ರ_2());
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : *list_p)
				ಫಲಿತಾಂಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(it.ಎರಡನೇ);
			ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
		}

		pool<T> ಗಮ್ಯ_pool() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ *this; }
		std::ಸದಿಶ<T> ಗಮ್ಯ_ಸದಿಶ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ *this; }
	};
};

ರಚನೆ RTLIL::Const
{
	ಗಿಡ್ಡ ಪೂರ್ಣಾಂಕ flags;
ಖಾಸಗಿ:
	ಮಿತ್ರ class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::ಸದಿಶ<RTLIL::State>;
	ಎಣಿಕೆ class backing_ಟ್ಯಾಗ್: bool { ಬಿಟ್ಟುಗಳು, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_ಟ್ಯಾಗ್ ಟ್ಯಾಗ್;
	ಒಕ್ಕೂಟ {
		bitvectype ಬಿಟ್ಟುಗಳು_2;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_ಬಿಟ್ಟುಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್ == backing_ಟ್ಯಾಗ್::ಬಿಟ್ಟುಗಳು; }
	bool is_str() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಟ್ಯಾಗ್ == backing_ಟ್ಯಾಗ್::string; }

	bitvectype* get_if_ಬಿಟ್ಟುಗಳು() { ಹಿಂತಿರುಗಿ is_ಬಿಟ್ಟುಗಳು() ? &ಬಿಟ್ಟುಗಳು_2 : ಖಾಲಿ; }
	std::string* get_if_str() { ಹಿಂತಿರುಗಿ is_str() ? &str_ : ಖಾಲಿ; }
	ಸ್ಥಿರಾಂಕ bitvectype* get_if_ಬಿಟ್ಟುಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ is_ಬಿಟ್ಟುಗಳು() ? &ಬಿಟ್ಟುಗಳು_2 : ಖಾಲಿ; }
	ಸ್ಥಿರಾಂಕ std::string* get_if_str() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ is_str() ? &str_ : ಖಾಲಿ; }

	bitvectype& get_ಬಿಟ್ಟುಗಳು();
	std::string& get_str();
	ಸ್ಥಿರಾಂಕ bitvectype& get_ಬಿಟ್ಟುಗಳು() ಸ್ಥಿರಾಂಕ;
	ಸ್ಥಿರಾಂಕ std::string& get_str() ಸ್ಥಿರಾಂಕ;
	std::ಸದಿಶ<RTLIL::State>& ಬಿಟ್ಟುಗಳು_ಆಂತರಿಕ();
	ಶೂನ್ಯ bitvectorize_ಆಂತರಿಕ();

ಸಾರ್ವಜನಿಕ:
	Const() : flags(RTLIL::CONST_FLAG_NONE), ಟ್ಯಾಗ್(backing_ಟ್ಯಾಗ್::ಬಿಟ್ಟುಗಳು), ಬಿಟ್ಟುಗಳು_2(std::ಸದಿಶ<RTLIL::State>()) {}
	Const(std::string str);
	Const(ದೀರ್ಘ ದೀರ್ಘ val); // default width is 32
	Const(ದೀರ್ಘ ದೀರ್ಘ val, ಪೂರ್ಣಾಂಕ ಅಗಲ);
	Const(RTLIL::State ಬಿಟ್, ಪೂರ್ಣಾಂಕ ಅಗಲ = 1);
	Const(std::ಸದಿಶ<RTLIL::State> ಬಿಟ್ಟುಗಳು) : flags(RTLIL::CONST_FLAG_NONE), ಟ್ಯಾಗ್(backing_ಟ್ಯಾಗ್::ಬಿಟ್ಟುಗಳು), ಬಿಟ್ಟುಗಳು_2(std::move(ಬಿಟ್ಟುಗಳು)) {}
	Const(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<bool> &ಬಿಟ್ಟುಗಳು);
	Const(ಸ್ಥಿರಾಂಕ RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &ಕ್ರಿಯಾಕಾರಕ =(ಸ್ಥಿರಾಂಕ RTLIL::Const &other);
	~Const();

	ರಚನೆ Builder
	{
		Builder() {}
		Builder(ಪೂರ್ಣಾಂಕ expected_ಅಗಲ) { ಬಿಟ್ಟುಗಳು.reserve(expected_ಅಗಲ); }
		ಶೂನ್ಯ ತಳ್ಳು_ಹಿಂಭಾಗ(RTLIL::State b) { ಬಿಟ್ಟುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(b); }
		ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ static_cast<ಪೂರ್ಣಾಂಕ>(ಬಿಟ್ಟುಗಳು.ಗಾತ್ರ_2()); }
		Const build() { ಹಿಂತಿರುಗಿ Const(std::move(ಬಿಟ್ಟುಗಳು)); }
	ಖಾಸಗಿ:
		std::ಸದಿಶ<RTLIL::State> ಬಿಟ್ಟುಗಳು;
	};

	bool ಕ್ರಿಯಾಕಾರಕ <(ಸ್ಥಿರಾಂಕ RTLIL::Const &other) ಸ್ಥಿರಾಂಕ;
	bool ಕ್ರಿಯಾಕಾರಕ ==(ಸ್ಥಿರಾಂಕ RTLIL::Const &other) ಸ್ಥಿರಾಂಕ;
	bool ಕ್ರಿಯಾಕಾರಕ !=(ಸ್ಥಿರಾಂಕ RTLIL::Const &other) ಸ್ಥಿರಾಂಕ;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::ಸದಿಶ<RTLIL::State>& ಬಿಟ್ಟುಗಳು() { ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_ಆಂತರಿಕ(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	ಶೂನ್ಯ bitvectorize() ಸ್ಥಿರಾಂಕ { const_cast<Const*>(this)->bitvectorize_ಆಂತರಿಕ(); }

	bool as_bool() ಸ್ಥಿರಾಂಕ;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	ಪೂರ್ಣಾಂಕ as_int(bool is_signed = false) ಸ್ಥಿರಾಂಕ;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_ಗಮ್ಯ_int(bool is_signed = false) ಸ್ಥಿರಾಂಕ;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<ಪೂರ್ಣಾಂಕ> try_as_int(bool is_signed = false) ಸ್ಥಿರಾಂಕ;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	ಪೂರ್ಣಾಂಕ as_int_saturating(bool is_signed = false) ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ ಟ್ಯಾಗ್_bare_integer_const(ಸ್ಥಿರಾಂಕ std::string &ಮೌಲ್ಯ);

	std::string as_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* any = "-") ಸ್ಥಿರಾಂಕ;
	ಸ್ಥಿರ Const from_string(ಸ್ಥಿರಾಂಕ std::string &str);
	std::ಸದಿಶ<RTLIL::State> ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು() ಸ್ಥಿರಾಂಕ;

	std::string decode_string() ಸ್ಥಿರಾಂಕ;
	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ;
	bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ append(ಸ್ಥಿರಾಂಕ RTLIL::Const &other);
	ಶೂನ್ಯ ಗಣ(ಪೂರ್ಣಾಂಕ i, RTLIL::State state) {
		ಬಿಟ್ಟುಗಳು_ಆಂತರಿಕ()[i] = state;
	}
	ಶೂನ್ಯ resize(ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2, RTLIL::State fill) {
    log_assert(ಗಾತ್ರ_2 >= 0 && ಗಾತ್ರ_2 < RTLIL::ಅಗಲ_ಎಲ್ಲೆ);
		ಬಿಟ್ಟುಗಳು_ಆಂತರಿಕ().resize(ಗಾತ್ರ_2, fill);
	}

	class const_iterator {
	ಖಾಸಗಿ:
		ಸ್ಥಿರಾಂಕ Const* ಪೋಷಕ;
		ಗಾತ್ರ_t idx;

	ಸಾರ್ವಜನಿಕ:
		using iterator_category = std::bidirectional_iterator_ಟ್ಯಾಗ್;
		using ಮೌಲ್ಯ_type = State;
		using difference_type = std::ptrdiff_t;
		using ಸೂಚಕ = ಸ್ಥಿರಾಂಕ State*;
		using reference = ಸ್ಥಿರಾಂಕ State&;

		const_iterator(ಸ್ಥಿರಾಂಕ Const& c, ಗಾತ್ರ_t i) : ಪೋಷಕ(&c), idx(i) {}

		State ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ;

		const_iterator& ಕ್ರಿಯಾಕಾರಕ++() { ++idx; ಹಿಂತಿರುಗಿ *this; }
		const_iterator& ಕ್ರಿಯಾಕಾರಕ--() { --idx; ಹಿಂತಿರುಗಿ *this; }
		const_iterator ಕ್ರಿಯಾಕಾರಕ++(ಪೂರ್ಣಾಂಕ) { const_iterator ಫಲಿತಾಂಶ(*this); ++idx; ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ; }
		const_iterator ಕ್ರಿಯಾಕಾರಕ--(ಪೂರ್ಣಾಂಕ) { const_iterator ಫಲಿತಾಂಶ(*this); --idx; ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ; }
		const_iterator& ಕ್ರಿಯಾಕಾರಕ+=(ಪೂರ್ಣಾಂಕ i) { idx += i; ಹಿಂತಿರುಗಿ *this; }

		const_iterator ಕ್ರಿಯಾಕಾರಕ+(ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು) {
			ಹಿಂತಿರುಗಿ const_iterator(*ಪೋಷಕ, idx + ಕೂಡಿಸು);
		}
		const_iterator ಕ್ರಿಯಾಕಾರಕ-(ಪೂರ್ಣಾಂಕ sub) {
			ಹಿಂತಿರುಗಿ const_iterator(*ಪೋಷಕ, idx - sub);
		}
		ಪೂರ್ಣಾಂಕ ಕ್ರಿಯಾಕಾರಕ-(ಸ್ಥಿರಾಂಕ const_iterator& other) {
			ಹಿಂತಿರುಗಿ idx - other.idx;
		}

		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ const_iterator& other) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ idx == other.idx;
		}

		bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ const_iterator& other) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ !(*this == other);
		}
	};

	class iterator {
	ಖಾಸಗಿ:
		Const* ಪೋಷಕ;
		ಗಾತ್ರ_t idx;

	ಸಾರ್ವಜನಿಕ:
		class proxy {
		ಖಾಸಗಿ:
			Const* ಪೋಷಕ;
			ಗಾತ್ರ_t idx;
		ಸಾರ್ವಜನಿಕ:
			proxy(Const* ಪೋಷಕ, ಗಾತ್ರ_t idx) : ಪೋಷಕ(ಪೋಷಕ), idx(idx) {}
			ಕ್ರಿಯಾಕಾರಕ State() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (*ಪೋಷಕ)[idx]; }
			proxy& ಕ್ರಿಯಾಕಾರಕ=(State s) { ಪೋಷಕ->ಗಣ(idx, s); ಹಿಂತಿರುಗಿ *this; }
			proxy& ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ proxy& other) { ಪೋಷಕ->ಗಣ(idx, (*other.ಪೋಷಕ)[other.idx]); ಹಿಂತಿರುಗಿ *this; }
		};

		using iterator_category = std::bidirectional_iterator_ಟ್ಯಾಗ್;
		using ಮೌಲ್ಯ_type = State;
		using difference_type = std::ptrdiff_t;
		using ಸೂಚಕ = proxy*;
		using reference = proxy;

		iterator(Const& c, ಗಾತ್ರ_t i) : ಪೋಷಕ(&c), idx(i) {}

		proxy ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ proxy(ಪೋಷಕ, idx); }
		iterator& ಕ್ರಿಯಾಕಾರಕ++() { ++idx; ಹಿಂತಿರುಗಿ *this; }
		iterator& ಕ್ರಿಯಾಕಾರಕ--() { --idx; ಹಿಂತಿರುಗಿ *this; }
		iterator ಕ್ರಿಯಾಕಾರಕ++(ಪೂರ್ಣಾಂಕ) { iterator ಫಲಿತಾಂಶ(*this); ++idx; ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ; }
		iterator ಕ್ರಿಯಾಕಾರಕ--(ಪೂರ್ಣಾಂಕ) { iterator ಫಲಿತಾಂಶ(*this); --idx; ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ; }
		iterator& ಕ್ರಿಯಾಕಾರಕ+=(ಪೂರ್ಣಾಂಕ i) { idx += i; ಹಿಂತಿರುಗಿ *this; }

		iterator ಕ್ರಿಯಾಕಾರಕ+(ಪೂರ್ಣಾಂಕ ಕೂಡಿಸು) {
			ಹಿಂತಿರುಗಿ iterator(*ಪೋಷಕ, idx + ಕೂಡಿಸು);
		}
		iterator ಕ್ರಿಯಾಕಾರಕ-(ಪೂರ್ಣಾಂಕ sub) {
			ಹಿಂತಿರುಗಿ iterator(*ಪೋಷಕ, idx - sub);
		}
		ಪೂರ್ಣಾಂಕ ಕ್ರಿಯಾಕಾರಕ-(ಸ್ಥಿರಾಂಕ iterator& other) {
			ಹಿಂತಿರುಗಿ idx - other.idx;
		}

		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ iterator& other) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ idx == other.idx;
		}

		bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ iterator& other) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ !(*this == other);
		}
	};

	const_iterator begin() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ const_iterator(*this, 0);
	}
	const_iterator ಕೊನೆ() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ const_iterator(*this, ಗಾತ್ರ_2());
	}
	iterator begin() {
		ಹಿಂತಿರುಗಿ iterator(*this, 0);
	}
	iterator ಕೊನೆ() {
		ಹಿಂತಿರುಗಿ iterator(*this, ಗಾತ್ರ_2());
	}
	State ಹಿಂಭಾಗ() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ *(ಕೊನೆ() - 1);
	}
	State front() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ *begin();
	}
	State at(ಗಾತ್ರ_t i) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ *const_iterator(*this, i);
	}
	State ಕ್ರಿಯಾಕಾರಕ[](ಗಾತ್ರ_t i) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ *const_iterator(*this, i);
	}

	bool is_fully_zero() ಸ್ಥಿರಾಂಕ;
	bool is_fully_ones() ಸ್ಥಿರಾಂಕ;
	bool is_fully_def() ಸ್ಥಿರಾಂಕ;
	bool is_fully_undef() ಸ್ಥಿರಾಂಕ;
	bool is_fully_undef_x_only() ಸ್ಥಿರಾಂಕ;
	bool is_onehot(ಪೂರ್ಣಾಂಕ *pos = nullptr) ಸ್ಥಿರಾಂಕ;

	RTLIL::Const ಹೊರತೆಗೆ(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ len = 1, RTLIL::State padding = RTLIL::State::S0) ಸ್ಥಿರಾಂಕ;

	// find the MSB without redundant leading bits
	ಪೂರ್ಣಾಂಕ get_min_ಗಾತ್ರ(bool is_signed) ಸ್ಥಿರಾಂಕ;

	// compress representation to the minimum required bits
	ಶೂನ್ಯ compress(bool is_signed = false);

	std::optional<ಪೂರ್ಣಾಂಕ> as_int_compress(bool is_signed) ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ extu(ಪೂರ್ಣಾಂಕ ಅಗಲ) {
		resize(ಅಗಲ, RTLIL::State::S0);
	}

	ಶೂನ್ಯ exts(ಪೂರ್ಣಾಂಕ ಅಗಲ) {
		resize(ಅಗಲ, ಖಾಲಿಗಳು() ? RTLIL::State::Sx : ಹಿಂಭಾಗ());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> ಗುಣಲಕ್ಷಣಗಳು;

	bool has_attribute(RTLIL::IdString id) ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ ಗಣ_bool_attribute(RTLIL::IdString id, bool ಮೌಲ್ಯ=true);
	bool get_bool_attribute(RTLIL::IdString id) ಸ್ಥಿರಾಂಕ;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	ಶೂನ್ಯ ಗಣ_string_attribute(RTLIL::IdString id, string ಮೌಲ್ಯ);
	string get_string_attribute(RTLIL::IdString id) ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ ಗಣ_strpool_attribute(RTLIL::IdString id, ಸ್ಥಿರಾಂಕ pool<string> &ದತ್ತಾಂಶ);
	ಶೂನ್ಯ ಕೂಡಿಸು_strpool_attribute(RTLIL::IdString id, ಸ್ಥಿರಾಂಕ pool<string> &ದತ್ತಾಂಶ);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ ಗಣ_src_attribute(ಸ್ಥಿರಾಂಕ std::string &src) {
		ಗಣ_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ get_string_attribute(ID::src);
	}

	ಶೂನ್ಯ ಗಣ_hdlname_attribute(ಸ್ಥಿರಾಂಕ ಸದಿಶ<string> &hierarchy);
	ಸದಿಶ<string> get_hdlname_attribute() ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ ಗಣ_intvec_attribute(RTLIL::IdString id, ಸ್ಥಿರಾಂಕ ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &ದತ್ತಾಂಶ);
	ಸದಿಶ<ಪೂರ್ಣಾಂಕ> get_intvec_attribute(RTLIL::IdString id) ಸ್ಥಿರಾಂಕ;
};

#ಅನ್ತರ್ಭಾವ "kernel/rtlil_twine_compat.h"

ರಚನೆ RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

ರಚನೆ RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::ಸದಿಶ<RTLIL::State> ದತ್ತಾಂಶ; // only used if wire == NULL, LSB at index 0
	ಪೂರ್ಣಾಂಕ ಅಗಲ, ಸ್ಥಾನಾಂತರ;

	SigChunk() : wire(nullptr), ಅಗಲ(0), ಸ್ಥಾನಾಂತರ(0) {}
	SigChunk(ಸ್ಥಿರಾಂಕ RTLIL::Const &ಮೌಲ್ಯ) : wire(nullptr), ದತ್ತಾಂಶ(ಮೌಲ್ಯ.ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು()), ಅಗಲ(Get_ಗಾತ್ರ(ದತ್ತಾಂಶ)), ಸ್ಥಾನಾಂತರ(0) {}
	SigChunk(RTLIL::Const &&ಮೌಲ್ಯ) : wire(nullptr), ದತ್ತಾಂಶ(ಮೌಲ್ಯ.ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು()), ಅಗಲ(Get_ಗಾತ್ರ(ದತ್ತಾಂಶ)), ಸ್ಥಾನಾಂತರ(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), ಅಗಲ(Get_ಗಾತ್ರ(wire)), ಸ್ಥಾನಾಂತರ(0) {}
	SigChunk(RTLIL::Wire *wire, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಅಗಲ = 1) : wire(wire), ಅಗಲ(ಅಗಲ), ಸ್ಥಾನಾಂತರ(ಸ್ಥಾನಾಂತರ) {}
	SigChunk(ಸ್ಥಿರಾಂಕ std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(ಪೂರ್ಣಾಂಕ val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(ಪೂರ್ಣಾಂಕ val, ಪೂರ್ಣಾಂಕ ಅಗಲ) : SigChunk(RTLIL::Const(val, ಅಗಲ)) {}
	SigChunk(RTLIL::State ಬಿಟ್, ಪೂರ್ಣಾಂಕ ಅಗಲ = 1) : SigChunk(RTLIL::Const(ಬಿಟ್, ಅಗಲ)) {}
	SigChunk(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್);

	RTLIL::SigChunk ಹೊರತೆಗೆ(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ) ಸ್ಥಿರಾಂಕ;
	RTLIL::Sig_ಬಿಟ್ ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ) ಸ್ಥಿರಾಂಕ;
	ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಅಗಲ; }
	ಸಾಲೊಳಗೆ bool is_wire() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ wire != ಖಾಲಿ; }

	bool ಕ್ರಿಯಾಕಾರಕ <(ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &other) ಸ್ಥಿರಾಂಕ;
	bool ಕ್ರಿಯಾಕಾರಕ ==(ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &other) ಸ್ಥಿರಾಂಕ;
	bool ಕ್ರಿಯಾಕಾರಕ !=(ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &other) ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ RTLIL::Sig_ಬಿಟ್
{
	RTLIL::Wire *wire;
	ಒಕ್ಕೂಟ {
		RTLIL::State ದತ್ತಾಂಶ; // used if wire == NULL
		ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;        // used if wire != NULL
	};

	Sig_ಬಿಟ್();
	Sig_ಬಿಟ್(RTLIL::State ಬಿಟ್);
	explicit Sig_ಬಿಟ್(bool ಬಿಟ್);
	Sig_ಬಿಟ್(RTLIL::Wire *wire);
	Sig_ಬಿಟ್(RTLIL::Wire *wire, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ);
	Sig_ಬಿಟ್(ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &chunk);
	Sig_ಬಿಟ್(ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &chunk, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ);
	Sig_ಬಿಟ್(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig);
	Sig_ಬಿಟ್(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sigbit) = ಪೂರ್ವನಿಯೋಜಿತ;
	RTLIL::Sig_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ =(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &other) = ಪೂರ್ವನಿಯೋಜಿತ;

	ಸಾಲೊಳಗೆ bool is_wire() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ wire != ಖಾಲಿ; }

	bool ಕ್ರಿಯಾಕಾರಕ <(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &other) ಸ್ಥಿರಾಂಕ;
	bool ಕ್ರಿಯಾಕಾರಕ ==(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &other) ಸ್ಥಿರಾಂಕ;
	bool ಕ್ರಿಯಾಕಾರಕ !=(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &other) ಸ್ಥಿರಾಂಕ;
	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ;
	[[nodiscard]] Hasher hash_ತುದಿ() ಸ್ಥಿರಾಂಕ;
};

namespace hashlib {
	ಮಾದರಿ <>
	ರಚನೆ hash_ops<RTLIL::Sig_ಬಿಟ್> {
		ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cmp(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &b) {
			ಹಿಂತಿರುಗಿ a == b;
		}
		[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ sb) {
			ಹಿಂತಿರುಗಿ sb.hash_ತುದಿ();
		}
		[[nodiscard]] ಸ್ಥಿರ ಸಾಲೊಳಗೆ Hasher hash_into(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ sb, Hasher h) {
			ಹಿಂತಿರುಗಿ sb.hash_into(h);
		}
	};
};

ರಚನೆ RTLIL::SigSpecIterator
{
	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::ಒಳಹರಿವು_iterator_ಟ್ಯಾಗ್ iterator_category;
	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ RTLIL::Sig_ಬಿಟ್ ಮೌಲ್ಯ_type;
	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ptrdiff_t difference_type;
	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ RTLIL::Sig_ಬಿಟ್* ಸೂಚಕ;
	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ RTLIL::Sig_ಬಿಟ್& reference;

	RTLIL::SigSpec *sig_p;
	ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ;

	ಸಾಲೊಳಗೆ RTLIL::Sig_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ;
	ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ RTLIL::SigSpecIterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ != other.ಸೂಚ್ಯಂಕ; }
	ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ RTLIL::SigSpecIterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ == other.ಸೂಚ್ಯಂಕ; }
	ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ++() { ಸೂಚ್ಯಂಕ++; }
};

ರಚನೆ RTLIL::SigSpecConstIterator
{
	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ std::ಒಳಹರಿವು_iterator_ಟ್ಯಾಗ್ iterator_category;
	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ RTLIL::Sig_ಬಿಟ್ ಮೌಲ್ಯ_type;
	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ptrdiff_t difference_type;
	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ RTLIL::Sig_ಬಿಟ್* ಸೂಚಕ;
	ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ RTLIL::Sig_ಬಿಟ್& reference;

	ಸ್ಥಿರಾಂಕ RTLIL::SigSpec *sig_p;
	RTLIL::Sig_ಬಿಟ್ ಬಿಟ್;
	ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ;

	ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ*();
	ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ RTLIL::SigSpecConstIterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ != other.ಸೂಚ್ಯಂಕ; }
	ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ RTLIL::SigSpecIterator &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ == other.ಸೂಚ್ಯಂಕ; }
	ಸಾಲೊಳಗೆ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ++() { ಸೂಚ್ಯಂಕ++; }
};

ರಚನೆ RTLIL::SigSpec
{
ಖಾಸಗಿ:
	ಮಿತ್ರ class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, ಹೊರತೆಗೆ_2);
	ಎಣಿಕೆ Representation : ಅಕ್ಷರ {
		CHUNK,
		ಬಿಟ್ಟುಗಳು_3,
	};
	// An AtomicHash is either clear or a nonzero integer.
	ರಚನೆ AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ಸ್ಥಿರಾಂಕ AtomicHash &rhs) : atomic_(rhs.ತುಂಬು()) {}
		AtomicHash &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ AtomicHash &rhs) { store(rhs.ತುಂಬು()); ಹಿಂತಿರುಗಿ *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> ಪಠ() ಸ್ಥಿರಾಂಕ {
			Hasher::hash_t ಮೌಲ್ಯ = ತುಂಬು();
			ಆದರೆ (ಮೌಲ್ಯ == 0)
				ಹಿಂತಿರುಗಿ std::nullopt;
			ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		ಶೂನ್ಯ ಗಣ(Hasher::hash_t ಮೌಲ್ಯ) ಸ್ಥಿರಾಂಕ {
			log_assert(ಮೌಲ್ಯ != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(ಮೌಲ್ಯ, std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed);
			log_assert(old == 0 || old == ಮೌಲ್ಯ);
		}
		ಶೂನ್ಯ clear() { store(0); }
	ಖಾಸಗಿ:
		ಪೂರ್ಣಾಂಕ ತುಂಬು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ atomic_.ತುಂಬು(std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed); }
		ಶೂನ್ಯ store(Hasher::hash_t ಮೌಲ್ಯ) ಸ್ಥಿರಾಂಕ {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(ಮೌಲ್ಯ, std::ಸ್ಮರಣೆ_ಕ್ರಮ_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	ಒಕ್ಕೂಟ {
		RTLIL::SigChunk chunk_;
		std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು_2; // LSB at index 0
	};

	ಶೂನ್ಯ init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು() {
		rep_ = ಬಿಟ್ಟುಗಳು_3;
		new (&ಬಿಟ್ಟುಗಳು_2) std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್>;
	}

	ಶೂನ್ಯ unpack();
	ಸಾಲೊಳಗೆ ಶೂನ್ಯ inline_unpack() {
		ಆದರೆ (rep_ == CHUNK)
			unpack();
	}
	ಶೂನ್ಯ try_repack();

	Hasher::hash_t updhash() ಸ್ಥಿರಾಂಕ;
	ಶೂನ್ಯ destroy() {
		ಆದರೆ (rep_ == CHUNK)
			chunk_.~SigChunk();
		ಇಲ್ಲದಿದ್ದರೆ
			ಬಿಟ್ಟುಗಳು_2.~ಸದಿಶ();
	}
	ಮಿತ್ರ ರಚನೆ Chunks;

ಸಾರ್ವಜನಿಕ:
	SigSpec() { init_ಖಾಲಿಗಳು_ಬಿಟ್ಟುಗಳು(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ಸ್ಥಿರಾಂಕ SigSpec &ಮೌಲ್ಯ) : rep_(ಮೌಲ್ಯ.rep_), hash_(ಮೌಲ್ಯ.hash_) {
		ಆದರೆ (ಮೌಲ್ಯ.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(ಮೌಲ್ಯ.chunk_);
		ಇಲ್ಲದಿದ್ದರೆ
			new (&ಬಿಟ್ಟುಗಳು_2) std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್>(ಮೌಲ್ಯ.ಬಿಟ್ಟುಗಳು_2);
	}
	SigSpec(SigSpec &&ಮೌಲ್ಯ) : rep_(ಮೌಲ್ಯ.rep_), hash_(ಮೌಲ್ಯ.hash_) {
		ಆದರೆ (ಮೌಲ್ಯ.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(ಮೌಲ್ಯ.chunk_));
		ಇಲ್ಲದಿದ್ದರೆ
			new (&ಬಿಟ್ಟುಗಳು_2) std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್>(std::move(ಮೌಲ್ಯ.ಬಿಟ್ಟುಗಳು_2));
	}
	SigSpec(ಸ್ಥಿರಾಂಕ RTLIL::Const &ಮೌಲ್ಯ);
	SigSpec(RTLIL::Const &&ಮೌಲ್ಯ);
	SigSpec(ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಅಗಲ = 1);
	SigSpec(ಸ್ಥಿರಾಂಕ std::string &str);
	SigSpec(ಪೂರ್ಣಾಂಕ val, ಪೂರ್ಣಾಂಕ ಅಗಲ = 32);
	SigSpec(RTLIL::State ಬಿಟ್, ಪೂರ್ಣಾಂಕ ಅಗಲ = 1);
	SigSpec(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್, ಪೂರ್ಣಾಂಕ ಅಗಲ = 1);
	SigSpec(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<RTLIL::SigChunk> &chunks);
	SigSpec(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> &ಬಿಟ್ಟುಗಳು);
	SigSpec(ಸ್ಥಿರಾಂಕ pool<RTLIL::Sig_ಬಿಟ್> &ಬಿಟ್ಟುಗಳು);
	SigSpec(ಸ್ಥಿರಾಂಕ std::ಗಣ<RTLIL::Sig_ಬಿಟ್> &ಬಿಟ್ಟುಗಳು);
	explicit SigSpec(bool ಬಿಟ್);
	~SigSpec() {
		destroy();
	}

	SigSpec &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		ಆದರೆ (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		ಇಲ್ಲದಿದ್ದರೆ
			new (&ಬಿಟ್ಟುಗಳು_2) std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್>(rhs.ಬಿಟ್ಟುಗಳು_2);
		ಹಿಂತಿರುಗಿ *this;
	}
	SigSpec &ಕ್ರಿಯಾಕಾರಕ=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		ಆದರೆ (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		ಇಲ್ಲದಿದ್ದರೆ
			new (&ಬಿಟ್ಟುಗಳು_2) std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್>(std::move(rhs.ಬಿಟ್ಟುಗಳು_2));
		ಹಿಂತಿರುಗಿ *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	ರಚನೆ Chunks {
		Chunks(ಸ್ಥಿರಾಂಕ SigSpec &spec) : spec(spec) {}
		ರಚನೆ const_iterator {
			using iterator_category = std::forward_iterator_ಟ್ಯಾಗ್;
			using ಮೌಲ್ಯ_type = ಸ್ಥಿರಾಂಕ SigChunk &;
			using difference_type = std::ptrdiff_t;
			using ಸೂಚಕ = ಸ್ಥಿರಾಂಕ SigChunk *;
			using reference = ಸ್ಥಿರಾಂಕ SigChunk &;

			ಸ್ಥಿರಾಂಕ SigSpec &spec;
			ಪೂರ್ಣಾಂಕ ಬಿಟ್_ಸೂಚ್ಯಂಕ;
			SigChunk chunk;

			const_iterator(ಸ್ಥಿರಾಂಕ SigSpec &spec) : spec(spec) {
				ಬಿಟ್_ಸೂಚ್ಯಂಕ = 0;
				ಆದರೆ (spec.rep_ == ಬಿಟ್ಟುಗಳು_3)
					ಮುಂದಿನ_chunk_ಬಿಟ್ಟುಗಳು();
			}
			ಎಣಿಕೆ ಕೊನೆ_2 { ಕೊನೆ_3 };
			const_iterator(ಸ್ಥಿರಾಂಕ SigSpec &spec, ಕೊನೆ_2) : spec(spec) {
				ಬಿಟ್_ಸೂಚ್ಯಂಕ = spec.ಗಾತ್ರ_2();
			}
			ಶೂನ್ಯ ಮುಂದಿನ_chunk_ಬಿಟ್ಟುಗಳು();

			ಸ್ಥಿರಾಂಕ SigChunk &ಕ್ರಿಯಾಕಾರಕ*() {
				ಆದರೆ (spec.rep_ == CHUNK)
					ಹಿಂತಿರುಗಿ spec.chunk_;
				ಹಿಂತಿರುಗಿ chunk;
			};
			ಸ್ಥಿರಾಂಕ SigChunk *ಕ್ರಿಯಾಕಾರಕ->() { ಹಿಂತಿರುಗಿ &**this; }
			const_iterator &ಕ್ರಿಯಾಕಾರಕ++() {
				ಬಿಟ್_ಸೂಚ್ಯಂಕ += (**this).ಅಗಲ;
				ಆದರೆ (spec.rep_ == ಬಿಟ್ಟುಗಳು_3)
					ಮುಂದಿನ_chunk_ಬಿಟ್ಟುಗಳು();
				ಹಿಂತಿರುಗಿ *this;
			}
			bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ const_iterator &rhs) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಬಿಟ್_ಸೂಚ್ಯಂಕ == rhs.ಬಿಟ್_ಸೂಚ್ಯಂಕ; }
			bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ const_iterator &rhs) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !(*this == rhs); }
		};
		const_iterator begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ const_iterator(spec); }
		const_iterator ಕೊನೆ() ಸ್ಥಿರಾಂಕ {
			const_iterator it(spec, const_iterator::ಕೊನೆ_3);
			ಹಿಂತಿರುಗಿ it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::ಸದಿಶ<RTLIL::SigChunk>::const_ಹಿಮ್ಮುಖ_iterator rbegin() {
			ensure_chunk_ಸದಿಶ();
			ಹಿಂತಿರುಗಿ chunk_ಸದಿಶ.rbegin();
		}
		std::ಸದಿಶ<RTLIL::SigChunk>::const_ಹಿಮ್ಮುಖ_iterator rend() {
			ensure_chunk_ಸದಿಶ();
			ಹಿಂತಿರುಗಿ chunk_ಸದಿಶ.rend();
		}
		ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() {
			ensure_chunk_ಸದಿಶ();
			ಹಿಂತಿರುಗಿ chunk_ಸದಿಶ.ಗಾತ್ರ_2();
		}
		ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ std::ದೂರ(begin(), ಕೊನೆ());
		}
		ಸ್ಥಿರಾಂಕ SigChunk &at(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) {
			ensure_chunk_ಸದಿಶ();
			ಹಿಂತಿರುಗಿ chunk_ಸದಿಶ.at(ಸೂಚ್ಯಂಕ);
		}
		ಕ್ರಿಯಾಕಾರಕ ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<RTLIL::SigChunk>&() {
			ensure_chunk_ಸದಿಶ();
			ಹಿಂತಿರುಗಿ chunk_ಸದಿಶ;
		}
	ಖಾಸಗಿ:
		ಶೂನ್ಯ ensure_chunk_ಸದಿಶ() {
			ಆದರೆ (spec.ಗಾತ್ರ_2() > 0 && chunk_ಸದಿಶ.ಖಾಲಿಗಳು()) {
				ಗಾಗಿ (ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &c : *this)
					chunk_ಸದಿಶ.ತಳ್ಳು_ಹಿಂಭಾಗ(c);
			}
		}
		ಸ್ಥಿರಾಂಕ SigSpec &spec;
		std::ಸದಿಶ<RTLIL::SigChunk> chunk_ಸದಿಶ;
	};
	ಮಿತ್ರ ರಚನೆ Chunks::const_iterator;

	ಸಾಲೊಳಗೆ Chunks chunks() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ {*this}; }
	ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ SigSpec &ಬಿಟ್ಟುಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ *this; }

	ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ rep_ == CHUNK ? chunk_.ಅಗಲ : Get_ಗಾತ್ರ(ಬಿಟ್ಟುಗಳು_2); }
	ಸಾಲೊಳಗೆ bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2() == 0; };

	ಸಾಲೊಳಗೆ RTLIL::Sig_ಬಿಟ್ &ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) { inline_unpack(); hash_.clear(); ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2.at(ಸೂಚ್ಯಂಕ); }
	ಸಾಲೊಳಗೆ RTLIL::Sig_ಬಿಟ್ ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
		ಆದರೆ (rep_ == CHUNK) {
			ಆದರೆ (ಸೂಚ್ಯಂಕ < 0 || ಸೂಚ್ಯಂಕ >= chunk_.ಅಗಲ)
				throw std::ಹೊರಹೋಗು_ದ_range("SigSpec::operator[]");
			ಆದರೆ (chunk_.wire)
				ಹಿಂತಿರುಗಿ RTLIL::Sig_ಬಿಟ್(chunk_.wire, chunk_.ಸ್ಥಾನಾಂತರ + ಸೂಚ್ಯಂಕ);
			ಹಿಂತಿರುಗಿ RTLIL::Sig_ಬಿಟ್(chunk_.ದತ್ತಾಂಶ[ಸೂಚ್ಯಂಕ]);
		}
		ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು_2.at(ಸೂಚ್ಯಂಕ);
	}

	ಸಾಲೊಳಗೆ RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.ಸೂಚ್ಯಂಕ = 0; ಹಿಂತಿರುಗಿ it; }
	ಸಾಲೊಳಗೆ RTLIL::SigSpecIterator ಕೊನೆ() { RTLIL::SigSpecIterator it; it.sig_p = this; it.ಸೂಚ್ಯಂಕ = ಗಾತ್ರ_2(); ಹಿಂತಿರುಗಿ it; }

	ಸಾಲೊಳಗೆ RTLIL::SigSpecConstIterator begin() ಸ್ಥಿರಾಂಕ { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.ಸೂಚ್ಯಂಕ = 0; ಹಿಂತಿರುಗಿ it; }
	ಸಾಲೊಳಗೆ RTLIL::SigSpecConstIterator ಕೊನೆ() ಸ್ಥಿರಾಂಕ { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.ಸೂಚ್ಯಂಕ = ಗಾತ್ರ_2(); ಹಿಂತಿರುಗಿ it; }

	ಶೂನ್ಯ ವಿಂಗಡಿಸು();
	ಶೂನ್ಯ ವಿಂಗಡಿಸು_and_unify();

	ಶೂನ್ಯ replace(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ನಮೂನೆ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &with);
	ಶೂನ್ಯ replace(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ನಮೂನೆ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &with, RTLIL::SigSpec *other) ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ replace(ಸ್ಥಿರಾಂಕ dict<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> &ನಿಯಮಗಳು);
	ಶೂನ್ಯ replace(ಸ್ಥಿರಾಂಕ dict<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> &ನಿಯಮಗಳು, RTLIL::SigSpec *other) ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ replace(ಸ್ಥಿರಾಂಕ std::map<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> &ನಿಯಮಗಳು);
	ಶೂನ್ಯ replace(ಸ್ಥಿರಾಂಕ std::map<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> &ನಿಯಮಗಳು, RTLIL::SigSpec *other) ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ replace(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &with);

	ಶೂನ್ಯ remove(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ನಮೂನೆ);
	ಶೂನ್ಯ remove(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ನಮೂನೆ, RTLIL::SigSpec *other) ಸ್ಥಿರಾಂಕ;
	ಶೂನ್ಯ remove2(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ನಮೂನೆ, RTLIL::SigSpec *other);

	ಶೂನ್ಯ remove(ಸ್ಥಿರಾಂಕ pool<RTLIL::Sig_ಬಿಟ್> &ನಮೂನೆ);
	ಶೂನ್ಯ remove(ಸ್ಥಿರಾಂಕ pool<RTLIL::Sig_ಬಿಟ್> &ನಮೂನೆ, RTLIL::SigSpec *other) ಸ್ಥಿರಾಂಕ;
	ಶೂನ್ಯ remove2(ಸ್ಥಿರಾಂಕ pool<RTLIL::Sig_ಬಿಟ್> &ನಮೂನೆ, RTLIL::SigSpec *other);
	ಶೂನ್ಯ remove2(ಸ್ಥಿರಾಂಕ std::ಗಣ<RTLIL::Sig_ಬಿಟ್> &ನಮೂನೆ, RTLIL::SigSpec *other);
	ಶೂನ್ಯ remove2(ಸ್ಥಿರಾಂಕ pool<RTLIL::Wire*> &ನಮೂನೆ, RTLIL::SigSpec *other);

	ಶೂನ್ಯ remove(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ = 1);
	ಶೂನ್ಯ remove_const();

	RTLIL::SigSpec ಹೊರತೆಗೆ(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ನಮೂನೆ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec *other = ಖಾಲಿ) ಸ್ಥಿರಾಂಕ;
	RTLIL::SigSpec ಹೊರತೆಗೆ(ಸ್ಥಿರಾಂಕ pool<RTLIL::Sig_ಬಿಟ್> &ನಮೂನೆ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec *other = ಖಾಲಿ) ಸ್ಥಿರಾಂಕ;
	RTLIL::SigSpec ಹೊರತೆಗೆ(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ = 1) ಸ್ಥಿರಾಂಕ;
	RTLIL::SigSpec ಹೊರತೆಗೆ_ಕೊನೆ(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಹೊರತೆಗೆ(ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ_2() - ಸ್ಥಾನಾಂತರ); }

	ಶೂನ್ಯ rewrite_wires(std::function<ಶೂನ್ಯ(RTLIL::Wire*& wire)> rewrite);

	RTLIL::Sig_ಬಿಟ್ lsb() ಸ್ಥಿರಾಂಕ { log_assert(ಗಾತ್ರ_2()); ಹಿಂತಿರುಗಿ (*this)[0]; };
	RTLIL::Sig_ಬಿಟ್ msb() ಸ್ಥಿರಾಂಕ { log_assert(ಗಾತ್ರ_2()); ಹಿಂತಿರುಗಿ (*this)[ಗಾತ್ರ_2() - 1]; };
	RTLIL::Sig_ಬಿಟ್ front() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (*this)[0]; }
	RTLIL::Sig_ಬಿಟ್ ಹಿಂಭಾಗ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (*this)[ಗಾತ್ರ_2() - 1]; }

	ಶೂನ್ಯ append(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &ಸಂಕೇತ);
	ಸಾಲೊಳಗೆ ಶೂನ್ಯ append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	ಸಾಲೊಳಗೆ ಶೂನ್ಯ append(ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	ಸಾಲೊಳಗೆ ಶೂನ್ಯ append(ಸ್ಥಿರಾಂಕ RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	ಶೂನ್ಯ append(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &ಬಿಟ್);
	ಸಾಲೊಳಗೆ ಶೂನ್ಯ append(RTLIL::State state) { append(RTLIL::Sig_ಬಿಟ್(state)); }
	ಸಾಲೊಳಗೆ ಶೂನ್ಯ append(bool bool_) { append(RTLIL::Sig_ಬಿಟ್(bool_)); }

	ಶೂನ್ಯ extend_u0(ಪೂರ್ಣಾಂಕ ಅಗಲ, bool is_signed = false);

	RTLIL::SigSpec repeat(ಪೂರ್ಣಾಂಕ ಅಂಕೆ) ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ ಹಿಮ್ಮುಖ() { inline_unpack(); std::ಹಿಮ್ಮುಖ(ಬಿಟ್ಟುಗಳು_2.begin(), ಬಿಟ್ಟುಗಳು_2.ಕೊನೆ()); }

	bool ಕ್ರಿಯಾಕಾರಕ <(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &other) ಸ್ಥಿರಾಂಕ;
	bool ಕ್ರಿಯಾಕಾರಕ ==(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &other) ಸ್ಥಿರಾಂಕ;
	ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ !=(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !(*this == other); }

	bool is_wire() ಸ್ಥಿರಾಂಕ;
	bool is_chunk() ಸ್ಥಿರಾಂಕ;
	ಸಾಲೊಳಗೆ bool is_ಬಿಟ್() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2() == 1; }

	bool ತಿಳಿದ_driver() ಸ್ಥಿರಾಂಕ;

	bool is_fully_const() ಸ್ಥಿರಾಂಕ;
	bool is_fully_zero() ಸ್ಥಿರಾಂಕ;
	bool is_fully_ones() ಸ್ಥಿರಾಂಕ;
	bool is_fully_def() ಸ್ಥಿರಾಂಕ;
	bool is_fully_undef() ಸ್ಥಿರಾಂಕ;
	bool has_const() ಸ್ಥಿರಾಂಕ;
	bool has_const(State state) ಸ್ಥಿರಾಂಕ;
	bool has_marked_ಬಿಟ್ಟುಗಳು() ಸ್ಥಿರಾಂಕ;
	bool is_onehot(ಪೂರ್ಣಾಂಕ *pos = nullptr) ಸ್ಥಿರಾಂಕ;

	bool as_bool() ಸ್ಥಿರಾಂಕ;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	ಪೂರ್ಣಾಂಕ as_int(bool is_signed = false) ಸ್ಥಿರಾಂಕ;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_ಗಮ್ಯ_int(bool is_signed = false) ಸ್ಥಿರಾಂಕ;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<ಪೂರ್ಣಾಂಕ> try_as_int(bool is_signed = false) ಸ್ಥಿರಾಂಕ;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	ಪೂರ್ಣಾಂಕ as_int_saturating(bool is_signed = false) ಸ್ಥಿರಾಂಕ;

	std::string as_string() ಸ್ಥಿರಾಂಕ;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() ಸ್ಥಿರಾಂಕ;
	RTLIL::Const as_const() ಸ್ಥಿರಾಂಕ;
	RTLIL::Wire *as_wire() ಸ್ಥಿರಾಂಕ;
	RTLIL::SigChunk as_chunk() ಸ್ಥಿರಾಂಕ;
	RTLIL::Sig_ಬಿಟ್ as_ಬಿಟ್() ಸ್ಥಿರಾಂಕ;

	bool ಹೊಂದಿಕೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ನಮೂನೆ) ಸ್ಥಿರಾಂಕ;

	std::ಗಣ<RTLIL::Sig_ಬಿಟ್> ಗಮ್ಯ_sigbit_ಗಣ() ಸ್ಥಿರಾಂಕ;
	pool<RTLIL::Sig_ಬಿಟ್> ಗಮ್ಯ_sigbit_pool() ಸ್ಥಿರಾಂಕ;
	std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್> ಗಮ್ಯ_sigbit_ಸದಿಶ() ಸ್ಥಿರಾಂಕ;
	std::map<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> ಗಮ್ಯ_sigbit_map(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &other) ಸ್ಥಿರಾಂಕ;
	dict<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> ಗಮ್ಯ_sigbit_dict(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &other) ಸ್ಥಿರಾಂಕ;

	ಸ್ಥಿರ bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	ಸ್ಥಿರ bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	ಸ್ಥಿರ bool parse_rhs(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	ಕ್ರಿಯಾಕಾರಕ std::ಸದಿಶ<RTLIL::SigChunk>() ಸ್ಥಿರಾಂಕ;
	ಕ್ರಿಯಾಕಾರಕ std::ಸದಿಶ<RTLIL::Sig_ಬಿಟ್>() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಗಮ್ಯ_sigbit_ಸದಿಶ(); }
	ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &at(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &defval) { ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ < ಗಾತ್ರ_2() ? (*this)[ಸ್ಥಾನಾಂತರ] : defval; }
	RTLIL::Sig_ಬಿಟ್& at(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ) { ಹಿಂತಿರುಗಿ (*this)[ಸ್ಥಾನಾಂತರ]; }
	RTLIL::Sig_ಬಿಟ್ at(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (*this)[ಸ್ಥಾನಾಂತರ]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
		Hasher::hash_t val;
		ಆದರೆ (std::optional<Hasher::hash_t> current = hash_.ಪಠ())
			val = *current;
		ಇಲ್ಲದಿದ್ದರೆ
			val = updhash();
		h.eat(val);
		ಹಿಂತಿರುಗಿ h;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NDEBUG
	ಶೂನ್ಯ check(ಸ್ಥಿರಾಂಕ Module *mod = nullptr) ಸ್ಥಿರಾಂಕ;
#ಇಲ್ಲದಿದ್ದರೆ
	ಶೂನ್ಯ check(ಸ್ಥಿರಾಂಕ Module *mod = nullptr) ಸ್ಥಿರಾಂಕ { (ಶೂನ್ಯ)mod; }
#ಆದರೆ_ಅಂತ್ಯ
};

ರಚನೆ RTLIL::Selection
{
	// selection includes boxed modules
	bool selects_boxes;
	// selection covers full design, including boxed modules
	bool complete_selection;
	// selection covers full design, not including boxed modules
	bool ತುಂಬಿದ_selection;
	pool<RTLIL::IdString> selected_modules;
	dict<RTLIL::IdString, pool<RTLIL::IdString>> selected_members;
	RTLIL::Design *current_design;

	// create a new selection
	Selection(
		// should the selection cover the full design
		bool ತುಂಬಿದ_2 = true,
		// should the selection include boxed modules
		bool boxes = false,
		// the design to select from
		RTLIL::Design *design = nullptr
	) :
		selects_boxes(boxes), complete_selection(ತುಂಬಿದ_2 && boxes), ತುಂಬಿದ_selection(ತುಂಬಿದ_2 && !boxes), current_design(design) { }

	// checks if the given module exists in the current design and is a
	// boxed module, warning the user if the current design is not set
	bool boxed_module(RTLIL::IdString mod_name) ಸ್ಥಿರಾಂಕ;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ಸ್ಥಿರಾಂಕ;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_ಸಂಪೂರ್ಣ_module(RTLIL::IdString mod_name) ಸ್ಥಿರಾಂಕ;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ಸ್ಥಿರಾಂಕ;

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
	bool selects_all() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ ತುಂಬಿದ_selection || complete_selection;
	}

	// add whole module to this selection
	ಮಾದರಿ<typename T1> ಶೂನ್ಯ select(T1 *module) {
		ಆದರೆ (!selects_all() && selected_modules.ಎಣಿಕೆ_2(module->name) == 0) {
			IdString name = module->name;
			selected_modules.ಸೇರಿಸು(name);
			selected_members.erase(name);
			ಆದರೆ (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	ಮಾದರಿ<typename T1, typename T2> ಶೂನ್ಯ select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	ಮಾದರಿ<typename T1> ಶೂನ್ಯ select(T1 *module, RTLIL::IdString member) {
		ಆದರೆ (!selects_all() && selected_modules.ಎಣಿಕೆ_2(module->name.ref()) == 0) {
			selected_members[module->name.ref()].ಸೇರಿಸು(member);
			ಆದರೆ (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ !selects_all() && selected_modules.ಖಾಲಿಗಳು() && selected_members.ಖಾಲಿಗಳು();
	}

	// clear this selection, leaving it empty
	ಶೂನ್ಯ clear();

	// create a new selection which is empty
	ಸ್ಥಿರ Selection ಖಾಲಿಗಳು_Selection(RTLIL::Design *design = nullptr) { ಹಿಂತಿರುಗಿ Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	ಸ್ಥಿರ Selection ತುಂಬಿದ_Selection(RTLIL::Design *design = nullptr) { ಹಿಂತಿರುಗಿ Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	ಸ್ಥಿರ Selection CompleteSelection(RTLIL::Design *design = nullptr) { ಹಿಂತಿರುಗಿ Selection(true, true, design); };
};

ರಚನೆ RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ { h.eat(hashidx_); ಹಿಂತಿರುಗಿ h; }

	Monitor() {
		ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ hashidx_ಎಣಿಕೆ = 123456789;
		hashidx_ಎಣಿಕೆ = mkhash_xorshift(hashidx_ಎಣಿಕೆ);
		hashidx_ = hashidx_ಎಣಿಕೆ;
	}

	ವಾಸ್ತವಿಕ ~Monitor() { }
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ notify_module_ಕೂಡಿಸು(RTLIL::Module*) { }
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ notify_module_del(RTLIL::Module*) { }
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ notify_connect(RTLIL::Cell*, RTLIL::IdString, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec&, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec&) { }
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ notify_connect(RTLIL::Module*, ಸ್ಥಿರಾಂಕ RTLIL::SigSig&) { }
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ notify_connect(RTLIL::Module*, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<RTLIL::SigSig>&) { }
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
ರಚನೆ define_map_t;

ಮಾದರಿ<typename N>
ಸಾಲೊಳಗೆ constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::ಎಲೆ> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*> || std::is_same_v<std::decay_t<N>, ಅಕ್ಷರ*>;

#ವ್ಯಾಖ್ಯಾನಿಸು YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, ಪೂರ್ಣಾಂಕ> = 0

ಮಾದರಿ<typename N>
ಸಾಲೊಳಗೆ constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *> || std::is_same_v<std::decay_t<N>, ಅಕ್ಷರ *>;

#ವ್ಯಾಖ್ಯಾನಿಸು YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, ಪೂರ್ಣಾಂಕ> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#ವ್ಯಾಖ್ಯಾನಿಸು YS_NAME_FWD_POOL(_func, _pool) \
	ಮಾದರಿ<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(ಸ್ವಯಂಚಾಲಿತ) _func(N name, Rest&&... rest) \
		{ ಹಿಂತಿರುಗಿ _func(_pool.ಕೂಡಿಸು(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#ವ್ಯಾಖ್ಯಾನಿಸು YS_NAME_FWD_2ND_POOL(_func, _pool) \
	ಮಾದರಿ<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(ಸ್ವಯಂಚಾಲಿತ) _func(T &&ಮೊದಲ, N name) \
		{ ಹಿಂತಿರುಗಿ _func(std::forward<T>(ಮೊದಲ), _pool.ಕೂಡಿಸು(std::move(name))); }

#ವ್ಯಾಖ್ಯಾನಿಸು YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#ವ್ಯಾಖ್ಯಾನಿಸು YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

ರಚನೆ RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ { h.eat(hashidx_); ಹಿಂತಿರುಗಿ h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	ಶೂನ್ಯ bufNormalize(bool enable=true);

	ಪೂರ್ಣಾಂಕ refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

ಖಾಸಗಿ:
	TwinePool twines_;

ಸಾರ್ವಜನಿಕ:
	TwinePool &twines() { ಹಿಂತಿರುಗಿ twines_; }
	ಸ್ಥಿರಾಂಕ TwinePool &twines() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ twines_; }

	ಶೂನ್ಯ absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	ಗಾತ್ರ_t gc_twines();

	std::ಸದಿಶ<std::unique_ptr<AST::Ast_ಗಂಟು>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_ನಿರೂಪಿಸುತ್ತದೆ;

	std::ಸದಿಶ<RTLIL::Selection> selection_ರಾಶಿ;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_ಸಕ್ರಿಯ_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ಸ್ಥಿರಾಂಕ RTLIL::Module *module(RTLIL::IdString name) ಸ್ಥಿರಾಂಕ;
	ಮಾದರಿ<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ ಹಿಂತಿರುಗಿ module(twines().ಕಂಡುಹಿಡಿ(std::string_view(name))); }
	ಮಾದರಿ<typename N, YS_NAME_STRING(N)> ಸ್ಥಿರಾಂಕ RTLIL::Module *module(N name) ಸ್ಥಿರಾಂಕ
		{ ಹಿಂತಿರುಗಿ module(twines().ಕಂಡುಹಿಡಿ(std::string_view(name))); }
	RTLIL::Module *ತುದಿ_module() ಸ್ಥಿರಾಂಕ;

	bool has(RTLIL::IdString id) ಸ್ಥಿರಾಂಕ {
		twines_.check_owned(id);
		ಹಿಂತಿರುಗಿ modules_.ಎಣಿಕೆ_2(id) != 0;
	}
	ಮಾದರಿ<typename N, YS_NAME_STRING(N)> bool has(N name) ಸ್ಥಿರಾಂಕ
		{ ಹಿಂತಿರುಗಿ has(twines().ಕಂಡುಹಿಡಿ(std::string_view(name))); }

	ಶೂನ್ಯ ಕೂಡಿಸು(RTLIL::Module *module);

	RTLIL::Module *ಕೂಡಿಸು_Module(RTLIL::IdString name);
	YS_NAME_FWD_POOL(ಕೂಡಿಸು_Module, twines())
	ಶೂನ್ಯ remove(RTLIL::Module *module);
	ಶೂನ್ಯ rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	ಶೂನ್ಯ scratchpad_unset(ಸ್ಥಿರಾಂಕ std::string &varname);

	ಶೂನ್ಯ scratchpad_ಗಣ_int(ಸ್ಥಿರಾಂಕ std::string &varname, ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ);
	ಶೂನ್ಯ scratchpad_ಗಣ_bool(ಸ್ಥಿರಾಂಕ std::string &varname, bool ಮೌಲ್ಯ);
	ಶೂನ್ಯ scratchpad_ಗಣ_string(ಸ್ಥಿರಾಂಕ std::string &varname, std::string ಮೌಲ್ಯ);

	ಪೂರ್ಣಾಂಕ scratchpad_get_int(ಸ್ಥಿರಾಂಕ std::string &varname, ಪೂರ್ಣಾಂಕ default_ಮೌಲ್ಯ_2 = 0) ಸ್ಥಿರಾಂಕ;
	bool scratchpad_get_bool(ಸ್ಥಿರಾಂಕ std::string &varname, bool default_ಮೌಲ್ಯ_2 = false) ಸ್ಥಿರಾಂಕ;
	std::string scratchpad_get_string(ಸ್ಥಿರಾಂಕ std::string &varname, ಸ್ಥಿರಾಂಕ std::string &default_ಮೌಲ್ಯ_2 = std::string()) ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ ವಿಂಗಡಿಸು();
	ಶೂನ್ಯ ವಿಂಗಡಿಸು_modules();
	ಶೂನ್ಯ check();
	ಶೂನ್ಯ optimize();

	ಶೂನ್ಯ clone_into(RTLIL::Design *dst) ಸ್ಥಿರಾಂಕ;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ಸ್ಥಿರಾಂಕ;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_ಸಂಪೂರ್ಣ_module(RTLIL::IdString mod_name) ಸ್ಥಿರಾಂಕ;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ಸ್ಥಿರಾಂಕ;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ಸ್ಥಿರಾಂಕ;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_ಸಂಪೂರ್ಣ_module(RTLIL::Module *mod) ಸ್ಥಿರಾಂಕ;

	// push the given selection to the selection stack
	ಶೂನ್ಯ ತಳ್ಳು_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	ಶೂನ್ಯ ತಳ್ಳು_ಖಾಲಿಗಳು_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	ಶೂನ್ಯ ತಳ್ಳು_ತುಂಬಿದ_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	ಶೂನ್ಯ ತಳ್ಳು_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	ಶೂನ್ಯ ತೆಗೆ_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		ಹಿಂತಿರುಗಿ selection_ರಾಶಿ.ಹಿಂಭಾಗ();
	}

	// get the current selection
	ಸ್ಥಿರಾಂಕ RTLIL::Selection &selection() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ selection_ರಾಶಿ.ಹಿಂಭಾಗ();
	}

	// is the current selection a full selection (no boxes)
	bool ತುಂಬಿದ_selection() ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ selection().ತುಂಬಿದ_selection;
	}

	// is the given module in the current selection
	ಮಾದರಿ<typename T1> bool selected(T1 *module) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	ಮಾದರಿ<typename T1, typename T2> bool selected(T1 *module, T2 *member) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	ಮಾದರಿ<typename T1> ಶೂನ್ಯ select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	ಮಾದರಿ<typename T1, typename T2> ಶೂನ್ಯ select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::ಸದಿಶ<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) ಸ್ಥಿರಾಂಕ;

	// returns all selected modules, and may include boxes
	std::ಸದಿಶ<RTLIL::Module*> all_selected_modules() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::ಸದಿಶ<RTLIL::Module*> selected_unboxed_modules() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::ಸದಿಶ<RTLIL::Module*> selected_unboxed_modules_warn() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::ಸದಿಶ<RTLIL::Module*> selected_ಸಂಪೂರ್ಣ_modules() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ selected_modules(SELECT_ಸಂಪೂರ್ಣ_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::ಸದಿಶ<RTLIL::Module*> all_selected_ಸಂಪೂರ್ಣ_modules() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ selected_modules(SELECT_ಸಂಪೂರ್ಣ_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::ಸದಿಶ<RTLIL::Module*> selected_ಸಂಪೂರ್ಣ_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ selected_modules(SELECT_ಸಂಪೂರ್ಣ_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::ಸದಿಶ<RTLIL::Module*> selected_unboxed_ಸಂಪೂರ್ಣ_modules() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ selected_modules(SELECT_ಸಂಪೂರ್ಣ_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::ಸದಿಶ<RTLIL::Module*> selected_unboxed_ಸಂಪೂರ್ಣ_modules_warn() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ selected_modules(SELECT_ಸಂಪೂರ್ಣ_WARN, SB_UNBOXED_WARN); }

	ಸ್ಥಿರ std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Design*> *get_all_designs(ಶೂನ್ಯ);

	std::string ಗಮ್ಯ_rtlil_str(bool only_selected = true) ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ RTLIL::Module : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
	ಮಿತ್ರ ರಚನೆ RTLIL::Cell;
	ಮಿತ್ರ ರಚನೆ RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ { h.eat(hashidx_); ಹಿಂತಿರುಗಿ h; }

ಸಂರಕ್ಷಿತ:
	ಶೂನ್ಯ ಕೂಡಿಸು(RTLIL::Wire *wire);
	ಶೂನ್ಯ ಕೂಡಿಸು(RTLIL::Cell *cell);
	ಶೂನ್ಯ ಕೂಡಿಸು(RTLIL::Process *process);

ಸಾರ್ವಜನಿಕ:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ design->twines(); }

	ಪೂರ್ಣಾಂಕ refcount_wires_;
	ಪೂರ್ಣಾಂಕ refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::ಸದಿಶ<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_ಮೌಲ್ಯಗಳು;
	dict<RTLIL::IdString, RTLIL::ಸ್ಮರಣೆ*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	ವಾಸ್ತವಿಕ ~Module();
	ವಾಸ್ತವಿಕ RTLIL::IdString derive(RTLIL::Design *design, ಸ್ಥಿರಾಂಕ dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	ವಾಸ್ತವಿಕ RTLIL::IdString derive(RTLIL::Design *design, ಸ್ಥಿರಾಂಕ dict<RTLIL::IdString, RTLIL::Const> &parameters, ಸ್ಥಿರಾಂಕ dict<RTLIL::IdString, RTLIL::Module*> &interfaces, ಸ್ಥಿರಾಂಕ dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	ವಾಸ್ತವಿಕ ಗಾತ್ರ_t ಎಣಿಕೆ_id(RTLIL::IdString id);
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ expand_interfaces(RTLIL::Design *design, ಸ್ಥಿರಾಂಕ dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	ವಾಸ್ತವಿಕ bool reprocess_if_necessary(RTLIL::Design *design);

	ವಾಸ್ತವಿಕ ಶೂನ್ಯ ವಿಂಗಡಿಸು();
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ check();
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ optimize();
	ವಾಸ್ತವಿಕ ಶೂನ್ಯ makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	ಶೂನ್ಯ connect(ಸ್ಥಿರಾಂಕ RTLIL::SigSig &conn);
	ಶೂನ್ಯ connect(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &lhs, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &rhs);
	ಶೂನ್ಯ new_connections(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<RTLIL::SigSig> &new_conn);
	ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<RTLIL::SigSig> &connections() ಸ್ಥಿರಾಂಕ;

	std::ಸದಿಶ<RTLIL::IdString> ports;
	ಶೂನ್ಯ fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_ಸರತಿ;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_ಪೋರ್ಟ್_ಸರತಿ;
	pool<RTLIL::Wire *> buf_norm_wire_ಸರತಿ;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_ಸೂಚ್ಯಂಕ;
	ಶೂನ್ಯ bufNormalize();

	ಮಾದರಿ<typename T> ಶೂನ್ಯ rewrite_sigspecs(T &functor);
	ಮಾದರಿ<typename T> ಶೂನ್ಯ rewrite_sigspecs2(T &functor);
	ಶೂನ್ಯ cloneInto(RTLIL::Module *new_mod) ಸ್ಥಿರಾಂಕ;
	ವಾಸ್ತವಿಕ RTLIL::Module *clone() ಸ್ಥಿರಾಂಕ;
	ವಾಸ್ತವಿಕ RTLIL::Module *clone(RTLIL::Design *dst) ಸ್ಥಿರಾಂಕ;
	ವಾಸ್ತವಿಕ RTLIL::Module *clone(RTLIL::Design *dst, IdString ಗುರಿ_name) ಸ್ಥಿರಾಂಕ;

	bool has_memories() ಸ್ಥಿರಾಂಕ;
	bool has_processes() ಸ್ಥಿರಾಂಕ;

	bool has_memories_warn() ಸ್ಥಿರಾಂಕ;
	bool has_processes_warn() ಸ್ಥಿರಾಂಕ;

	bool is_selected() ಸ್ಥಿರಾಂಕ;
	bool is_selected_ಸಂಪೂರ್ಣ() ಸ್ಥಿರಾಂಕ;

	std::ಸದಿಶ<RTLIL::Wire*> selected_wires() ಸ್ಥಿರಾಂಕ;
	std::ಸದಿಶ<RTLIL::Cell*> selected_cells() ಸ್ಥಿರಾಂಕ;
	std::ಸದಿಶ<RTLIL::ಸ್ಮರಣೆ*> selected_memories() ಸ್ಥಿರಾಂಕ;
	std::ಸದಿಶ<RTLIL::Process*> selected_processes() ಸ್ಥಿರಾಂಕ;
	std::ಸದಿಶ<RTLIL::SelectedMember> selected_members() ಸ್ಥಿರಾಂಕ;

	ಮಾದರಿ<typename T> bool selected(T *member) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		ಆದರೆ (design)
			design->twines().check_owned(id);
		ಸ್ವಯಂಚಾಲಿತ it = wires_.ಕಂಡುಹಿಡಿ(id);
		ಹಿಂತಿರುಗಿ it == wires_.ಕೊನೆ() ? nullptr : it->ಎರಡನೇ;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		ಆದರೆ (design)
			design->twines().check_owned(id);
		ಸ್ವಯಂಚಾಲಿತ it = cells_.ಕಂಡುಹಿಡಿ(id);
		ಹಿಂತಿರುಗಿ it == cells_.ಕೊನೆ() ? nullptr : it->ಎರಡನೇ;
	}

	ಸ್ಥಿರಾಂಕ RTLIL::Wire* wire(RTLIL::IdString id) ಸ್ಥಿರಾಂಕ{
		ಆದರೆ (design)
			design->twines().check_owned(id);
		ಸ್ವಯಂಚಾಲಿತ it = wires_.ಕಂಡುಹಿಡಿ(id);
		ಹಿಂತಿರುಗಿ it == wires_.ಕೊನೆ() ? nullptr : it->ಎರಡನೇ;
	}
	ಸ್ಥಿರಾಂಕ RTLIL::Cell* cell(RTLIL::IdString id) ಸ್ಥಿರಾಂಕ {
		ಆದರೆ (design)
			design->twines().check_owned(id);
		ಸ್ವಯಂಚಾಲಿತ it = cells_.ಕಂಡುಹಿಡಿ(id);
		ಹಿಂತಿರುಗಿ it == cells_.ಕೊನೆ() ? nullptr : it->ಎರಡನೇ;
	}

	ಮಾದರಿ<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ ಹಿಂತಿರುಗಿ wire(twines().ಕಂಡುಹಿಡಿ(std::string_view(name))); }
	ಮಾದರಿ<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ ಹಿಂತಿರುಗಿ cell(twines().ಕಂಡುಹಿಡಿ(std::string_view(name))); }
	ಮಾದರಿ<typename N, YS_NAME_STRING(N)> ಸ್ಥಿರಾಂಕ RTLIL::Wire *wire(N name) ಸ್ಥಿರಾಂಕ
		{ ಹಿಂತಿರುಗಿ wire(twines().ಕಂಡುಹಿಡಿ(std::string_view(name))); }
	ಮಾದರಿ<typename N, YS_NAME_STRING(N)> ಸ್ಥಿರಾಂಕ RTLIL::Cell *cell(N name) ಸ್ಥಿರಾಂಕ
		{ ಹಿಂತಿರುಗಿ cell(twines().ಕಂಡುಹಿಡಿ(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { ಹಿಂತಿರುಗಿ RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	ಪೂರ್ಣಾಂಕ wires_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ wires_.ಗಾತ್ರ_2(); }
	RTLIL::Wire* wire_at(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ wires_.element(ಸೂಚ್ಯಂಕ)->ಎರಡನೇ; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { ಹಿಂತಿರುಗಿ RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	ಪೂರ್ಣಾಂಕ cells_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ cells_.ಗಾತ್ರ_2(); }
	RTLIL::Cell* cell_at(ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ cells_.element(ಸೂಚ್ಯಂಕ)->ಎರಡನೇ; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	ಶೂನ್ಯ remove(ಸ್ಥಿರಾಂಕ pool<RTLIL::Wire*> &wires);
	ಶೂನ್ಯ remove(RTLIL::Cell *cell);
	ಶೂನ್ಯ remove(RTLIL::ಸ್ಮರಣೆ *ಸ್ಮರಣೆ_3);
	ಶೂನ್ಯ remove(RTLIL::Process *process);

	ಶೂನ್ಯ rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	ಶೂನ್ಯ rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	ಶೂನ್ಯ rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	ಶೂನ್ಯ ಅದಲುಬದಲು_ಹೆಸರುಗಳು(RTLIL::Wire *w1, RTLIL::Wire *w2);
	ಶೂನ್ಯ ಅದಲುಬದಲು_ಹೆಸರುಗಳು(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, ಪೂರ್ಣಾಂಕ &ಸೂಚ್ಯಂಕ);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *ಕೂಡಿಸು_Wire(RTLIL::IdString name, ಪೂರ್ಣಾಂಕ ಅಗಲ = 1);
	RTLIL::Wire *ಕೂಡಿಸು_Wire(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Wire *other);
	YS_NAME_FWD(ಕೂಡಿಸು_Wire)

	RTLIL::Cell *ಕೂಡಿಸು_Cell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *ಕೂಡಿಸು_Cell(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Cell *other);
	YS_NAME_FWD(ಕೂಡಿಸು_Cell)
	ಮಾದರಿ<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *ಕೂಡಿಸು_Cell(RTLIL::IdString name, T type) { ಹಿಂತಿರುಗಿ ಕೂಡಿಸು_Cell(name, design->twines().ಕೂಡಿಸು(std::move(type))); }

	RTLIL::ಸ್ಮರಣೆ *ಕೂಡಿಸು_ಸ್ಮರಣೆ(RTLIL::IdString name);
	YS_NAME_FWD(ಕೂಡಿಸು_ಸ್ಮರಣೆ)
	RTLIL::ಸ್ಮರಣೆ *ಕೂಡಿಸು_ಸ್ಮರಣೆ(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::ಸ್ಮರಣೆ *other);

	RTLIL::Process *ಕೂಡಿಸು_Process(RTLIL::IdString name);
	YS_NAME_FWD(ಕೂಡಿಸು_Process)
	RTLIL::Process *ಕೂಡಿಸು_Process(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* ಕೂಡಿಸು_Not     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Pos     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Buf     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Barrier (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Neg     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_And  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Or   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Xor  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Xnor (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_Reduce_And  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Reduce_Or   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Reduce_Xor  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Reduce_Xnor (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Reduce_Bool (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_Shl    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Shr    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Sshl   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Sshr   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_ಸರಿಸು  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Shiftx (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_Lt  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Le  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Eq  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Ne  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Eqx (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Nex (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Ge  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Gt  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_ಕೂಡಿಸು (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Sub (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Mul (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	// truncating division
	RTLIL::Cell* ಕೂಡಿಸು_Div (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	// truncating modulo
	RTLIL::Cell* ಕೂಡಿಸು_Mod (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Div_Floor (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Mod_Floor (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Pow (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_Fa (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_c, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_x, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_Logic_Not (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Logic_And (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Logic_Or  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_Mux  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Pmux (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Bmux (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Demux (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_Bweqx  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Bwmux  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_ತುಂಡು  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, RTLIL::Const ಸ್ಥಾನಾಂತರ, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Concat (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Lut    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, RTLIL::Const lut, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Tribuf (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Assert (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Assume (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_ಜೀವಂತ   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Fair   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Cover  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Equiv  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_Sr    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool ಗಣ_polarity = true, bool clr_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Ff    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Dff   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d,   ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool clk_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Dffe  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en,  ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Dffsr (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool ಗಣ_polarity = true, bool clr_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Dffsre (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool ಗಣ_polarity = true, bool clr_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Adff (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_arst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, RTLIL::Const arst_ಮೌಲ್ಯ, bool clk_polarity = true, bool arst_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Adffe (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_arst,  ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, RTLIL::Const arst_ಮೌಲ್ಯ, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Aldff (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_aload, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Aldffe (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_aload,  ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Sdff (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_srst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, RTLIL::Const srst_ಮೌಲ್ಯ, bool clk_polarity = true, bool srst_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Sdffe (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_srst,  ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, RTLIL::Const srst_ಮೌಲ್ಯ, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Sdffce (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_srst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, RTLIL::Const srst_ಮೌಲ್ಯ, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Dlatch (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool en_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Adlatch (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_arst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, RTLIL::Const arst_ಮೌಲ್ಯ, bool en_polarity = true, bool arst_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Dlatchsr (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool en_polarity = true, bool ಗಣ_polarity = true, bool clr_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_Buf_Gate    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Not_Gate    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_And_Gate    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Nand_Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Or_Gate     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Nor_Gate    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Xor_Gate    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Xnor_Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Andnot_Gate (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Ornot_Gate  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Mux_Gate    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Nmux_Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Aoi3_Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_c, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Oai3_Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_c, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Aoi4_Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_c, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Oai4_Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_c, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_Sr_Gate     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr,
			ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool ಗಣ_polarity = true, bool clr_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Ff_Gate     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Dff_Gate    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool clk_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Dffe_Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Dffsr_Gate  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool ಗಣ_polarity = true, bool clr_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Dffsre_Gate (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool ಗಣ_polarity = true, bool clr_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Adff_Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_arst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
			bool arst_ಮೌಲ್ಯ = false, bool clk_polarity = true, bool arst_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Adffe_Gate  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_arst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
			bool arst_ಮೌಲ್ಯ = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Aldff_Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_aload, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
			ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Aldffe_Gate  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_aload, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
			ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Sdff_Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_srst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
			bool srst_ಮೌಲ್ಯ = false, bool clk_polarity = true, bool srst_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Sdffe_Gate  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_srst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
			bool srst_ಮೌಲ್ಯ = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Sdffce_Gate (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clk, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_srst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
			bool srst_ಮೌಲ್ಯ = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Dlatch_Gate (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool en_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Adlatch_Gate(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_arst, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q,
			bool arst_ಮೌಲ್ಯ = false, bool en_polarity = true, bool arst_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell* ಕೂಡಿಸು_Dlatchsr_Gate  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_en, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_ಗಣ, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, bool en_polarity = true, bool ಗಣ_polarity = true, bool clr_polarity = true, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Cell* ಕೂಡಿಸು_Anyinit(RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_d, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_q, ಸ್ಥಿರಾಂಕ std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec ಸರಿಸು_2  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::SigSpec ಕೂಡಿಸು_2 (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, bool is_signed = false, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::Sig_ಬಿಟ್ BufGate    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ NotGate    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ AndGate    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ NandGate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ OrGate     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ NorGate    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ XorGate    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ XnorGate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ AndnotGate (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ OrnotGate  (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ MuxGate    (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_s, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ NmuxGate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_s, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ Aoi3Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_c, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ Oai3Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_c, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ Aoi4Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_c, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_d, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Sig_ಬಿಟ್ Oai4Gate   (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_b, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_c, ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &sig_d, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, ಪೂರ್ಣಾಂಕ ಅಗಲ = 1, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, ಪೂರ್ಣಾಂಕ ಅಗಲ = 1, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, ಪೂರ್ಣಾಂಕ ಅಗಲ = 1, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, ಪೂರ್ಣಾಂಕ ಅಗಲ = 1, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ std::string &src = "");

	RTLIL::SigSpec ಗಣ_ಟ್ಯಾಗ್_2          (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ std::string &ಟ್ಯಾಗ್, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_c, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell*   ಕೂಡಿಸು_ಗಣ_ಟ್ಯಾಗ್       (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ std::string &ಟ್ಯಾಗ್, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_c, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_y, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Get_ಟ್ಯಾಗ್          (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ std::string &ಟ್ಯಾಗ್, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::Cell*   ಕೂಡಿಸು_Overwrite_ಟ್ಯಾಗ್ (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ std::string &ಟ್ಯಾಗ್, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_s, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_c, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec Original_ಟ್ಯಾಗ್     (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ std::string &ಟ್ಯಾಗ್, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_a, ಸ್ಥಿರಾಂಕ std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig_e, ಸ್ಥಿರಾಂಕ std::string &src = "");
	YS_NAME_FWD(ಕೂಡಿಸು_Anyinit)
	YS_NAME_FWD(Anyconst) YS_NAME_FWD(Anyseq) YS_NAME_FWD(Allconst) YS_NAME_FWD(Allseq)
	YS_NAME_FWD(Initstate)
	YS_NAME_FWD(ಗಣ_ಟ್ಯಾಗ್_2) YS_NAME_FWD(ಕೂಡಿಸು_ಗಣ_ಟ್ಯಾಗ್) YS_NAME_FWD(Get_ಟ್ಯಾಗ್)
	YS_NAME_FWD(ಕೂಡಿಸು_Overwrite_ಟ್ಯಾಗ್) YS_NAME_FWD(Original_ಟ್ಯಾಗ್) YS_NAME_FWD(FutureFF)
	YS_NAME_FWD(ಕೂಡಿಸು_Not) YS_NAME_FWD(ಕೂಡಿಸು_Pos) YS_NAME_FWD(ಕೂಡಿಸು_Buf) YS_NAME_FWD(ಕೂಡಿಸು_Barrier) YS_NAME_FWD(ಕೂಡಿಸು_Neg)
	YS_NAME_FWD(ಕೂಡಿಸು_And) YS_NAME_FWD(ಕೂಡಿಸು_Or) YS_NAME_FWD(ಕೂಡಿಸು_Xor) YS_NAME_FWD(ಕೂಡಿಸು_Xnor)
	YS_NAME_FWD(ಕೂಡಿಸು_Reduce_And) YS_NAME_FWD(ಕೂಡಿಸು_Reduce_Or) YS_NAME_FWD(ಕೂಡಿಸು_Reduce_Xor) YS_NAME_FWD(ಕೂಡಿಸು_Reduce_Xnor)
	YS_NAME_FWD(ಕೂಡಿಸು_Reduce_Bool) YS_NAME_FWD(ಕೂಡಿಸು_Shl) YS_NAME_FWD(ಕೂಡಿಸು_Shr) YS_NAME_FWD(ಕೂಡಿಸು_Sshl)
	YS_NAME_FWD(ಕೂಡಿಸು_Sshr) YS_NAME_FWD(ಕೂಡಿಸು_ಸರಿಸು) YS_NAME_FWD(ಕೂಡಿಸು_Shiftx) YS_NAME_FWD(ಕೂಡಿಸು_Lt)
	YS_NAME_FWD(ಕೂಡಿಸು_Le) YS_NAME_FWD(ಕೂಡಿಸು_Eq) YS_NAME_FWD(ಕೂಡಿಸು_Ne) YS_NAME_FWD(ಕೂಡಿಸು_Eqx)
	YS_NAME_FWD(ಕೂಡಿಸು_Nex) YS_NAME_FWD(ಕೂಡಿಸು_Ge) YS_NAME_FWD(ಕೂಡಿಸು_Gt) YS_NAME_FWD(ಕೂಡಿಸು_ಕೂಡಿಸು)
	YS_NAME_FWD(ಕೂಡಿಸು_Sub) YS_NAME_FWD(ಕೂಡಿಸು_Mul) YS_NAME_FWD(ಕೂಡಿಸು_Div) YS_NAME_FWD(ಕೂಡಿಸು_Mod)
	YS_NAME_FWD(ಕೂಡಿಸು_Div_Floor) YS_NAME_FWD(ಕೂಡಿಸು_Mod_Floor) YS_NAME_FWD(ಕೂಡಿಸು_Pow) YS_NAME_FWD(ಕೂಡಿಸು_Fa)
	YS_NAME_FWD(ಕೂಡಿಸು_Logic_Not) YS_NAME_FWD(ಕೂಡಿಸು_Logic_And) YS_NAME_FWD(ಕೂಡಿಸು_Logic_Or) YS_NAME_FWD(ಕೂಡಿಸು_Mux)
	YS_NAME_FWD(ಕೂಡಿಸು_Pmux) YS_NAME_FWD(ಕೂಡಿಸು_Bmux) YS_NAME_FWD(ಕೂಡಿಸು_Demux) YS_NAME_FWD(ಕೂಡಿಸು_Bweqx)
	YS_NAME_FWD(ಕೂಡಿಸು_Bwmux) YS_NAME_FWD(ಕೂಡಿಸು_ತುಂಡು) YS_NAME_FWD(ಕೂಡಿಸು_Concat) YS_NAME_FWD(ಕೂಡಿಸು_Lut)
	YS_NAME_FWD(ಕೂಡಿಸು_Tribuf) YS_NAME_FWD(ಕೂಡಿಸು_Assert) YS_NAME_FWD(ಕೂಡಿಸು_Assume) YS_NAME_FWD(ಕೂಡಿಸು_ಜೀವಂತ)
	YS_NAME_FWD(ಕೂಡಿಸು_Fair) YS_NAME_FWD(ಕೂಡಿಸು_Cover) YS_NAME_FWD(ಕೂಡಿಸು_Equiv) YS_NAME_FWD(ಕೂಡಿಸು_Sr)
	YS_NAME_FWD(ಕೂಡಿಸು_Ff) YS_NAME_FWD(ಕೂಡಿಸು_Dff) YS_NAME_FWD(ಕೂಡಿಸು_Dffe) YS_NAME_FWD(ಕೂಡಿಸು_Dffsr)
	YS_NAME_FWD(ಕೂಡಿಸು_Dffsre) YS_NAME_FWD(ಕೂಡಿಸು_Adff) YS_NAME_FWD(ಕೂಡಿಸು_Adffe) YS_NAME_FWD(ಕೂಡಿಸು_Aldff)
	YS_NAME_FWD(ಕೂಡಿಸು_Aldffe) YS_NAME_FWD(ಕೂಡಿಸು_Sdff) YS_NAME_FWD(ಕೂಡಿಸು_Sdffe) YS_NAME_FWD(ಕೂಡಿಸು_Sdffce)
	YS_NAME_FWD(ಕೂಡಿಸು_Dlatch) YS_NAME_FWD(ಕೂಡಿಸು_Adlatch) YS_NAME_FWD(ಕೂಡಿಸು_Dlatchsr) YS_NAME_FWD(ಕೂಡಿಸು_Buf_Gate)
	YS_NAME_FWD(ಕೂಡಿಸು_Not_Gate) YS_NAME_FWD(ಕೂಡಿಸು_And_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Nand_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Or_Gate)
	YS_NAME_FWD(ಕೂಡಿಸು_Nor_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Xor_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Xnor_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Andnot_Gate)
	YS_NAME_FWD(ಕೂಡಿಸು_Ornot_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Mux_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Nmux_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Aoi3_Gate)
	YS_NAME_FWD(ಕೂಡಿಸು_Oai3_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Aoi4_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Oai4_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Sr_Gate)
	YS_NAME_FWD(ಕೂಡಿಸು_Ff_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Dff_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Dffe_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Dffsr_Gate)
	YS_NAME_FWD(ಕೂಡಿಸು_Dffsre_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Adff_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Adffe_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Aldff_Gate)
	YS_NAME_FWD(ಕೂಡಿಸು_Aldffe_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Sdff_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Sdffe_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Sdffce_Gate)
	YS_NAME_FWD(ಕೂಡಿಸು_Dlatch_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Adlatch_Gate) YS_NAME_FWD(ಕೂಡಿಸು_Dlatchsr_Gate) YS_NAME_FWD(Not)
	YS_NAME_FWD(Pos) YS_NAME_FWD(Buf) YS_NAME_FWD(Barrier) YS_NAME_FWD(Neg) YS_NAME_FWD(And)
	YS_NAME_FWD(Or) YS_NAME_FWD(Xor) YS_NAME_FWD(Xnor) YS_NAME_FWD(ReduceAnd)
	YS_NAME_FWD(ReduceOr) YS_NAME_FWD(ReduceXor) YS_NAME_FWD(ReduceXnor) YS_NAME_FWD(ReduceBool)
	YS_NAME_FWD(Shl) YS_NAME_FWD(Shr) YS_NAME_FWD(Sshl) YS_NAME_FWD(Sshr)
	YS_NAME_FWD(ಸರಿಸು_2) YS_NAME_FWD(Shiftx) YS_NAME_FWD(Lt) YS_NAME_FWD(Le)
	YS_NAME_FWD(Eq) YS_NAME_FWD(Ne) YS_NAME_FWD(Eqx) YS_NAME_FWD(Nex)
	YS_NAME_FWD(Ge) YS_NAME_FWD(Gt) YS_NAME_FWD(ಕೂಡಿಸು_2) YS_NAME_FWD(Sub)
	YS_NAME_FWD(Mul) YS_NAME_FWD(Div) YS_NAME_FWD(Mod) YS_NAME_FWD(DivFloor)
	YS_NAME_FWD(ModFloor) YS_NAME_FWD(Pow) YS_NAME_FWD(LogicNot) YS_NAME_FWD(LogicAnd)
	YS_NAME_FWD(LogicOr) YS_NAME_FWD(Mux) YS_NAME_FWD(Pmux) YS_NAME_FWD(Bmux)
	YS_NAME_FWD(Demux) YS_NAME_FWD(Bweqx) YS_NAME_FWD(Bwmux) YS_NAME_FWD(BufGate)
	YS_NAME_FWD(NotGate) YS_NAME_FWD(AndGate) YS_NAME_FWD(NandGate) YS_NAME_FWD(OrGate)
	YS_NAME_FWD(NorGate) YS_NAME_FWD(XorGate) YS_NAME_FWD(XnorGate) YS_NAME_FWD(AndnotGate)
	YS_NAME_FWD(OrnotGate) YS_NAME_FWD(MuxGate) YS_NAME_FWD(NmuxGate) YS_NAME_FWD(Aoi3Gate)
	YS_NAME_FWD(Oai3Gate) YS_NAME_FWD(Aoi4Gate) YS_NAME_FWD(Oai4Gate)

	std::string ಗಮ್ಯ_rtlil_str() ಸ್ಥಿರಾಂಕ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	ಸ್ಥಿರ std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Module*> *get_all_modules(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
};

ರಚನೆ RTLIL::Wire : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ { h.eat(hashidx_); ಹಿಂತಿರುಗಿ h; }

ಸಂರಕ್ಷಿತ:
	// use module->addWire() and module->remove() to create or destroy wires
	ಮಿತ್ರ ರಚನೆ RTLIL::Module;
	Wire();
	~Wire();

	ಮಿತ್ರ ರಚನೆ RTLIL::Design;
	ಮಿತ್ರ ರಚನೆ RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driver_ಪೋರ್ಟ್_2;

ಸಾರ್ವಜನಿಕ:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	ಪೂರ್ಣಾಂಕ ಅಗಲ, ಆರಂಭ_ಸ್ಥಾನಾಂತರ, ಪೋರ್ಟ್_id;
	bool ಪೋರ್ಟ್_ಒಳಹರಿವು, ಪೋರ್ಟ್_output, upto, is_signed;

	RTLIL::Design *design() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ module ? module->design : nullptr; }
	TwinePool &twines() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ module->twines(); }

	bool ತಿಳಿದ_driver() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ಸ್ಥಿರಾಂಕ    { log_assert(driverCell_); ಹಿಂತಿರುಗಿ driverCell_; };
	RTLIL::IdString driver_ಪೋರ್ಟ್() ಸ್ಥಿರಾಂಕ { log_assert(driverCell_); ಹಿಂತಿರುಗಿ driver_ಪೋರ್ಟ್_2; };

	ಪೂರ್ಣಾಂಕ from_hdl_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ hdl_ಸೂಚ್ಯಂಕ) {
		ಪೂರ್ಣಾಂಕ zero_ಸೂಚ್ಯಂಕ = hdl_ಸೂಚ್ಯಂಕ - ಆರಂಭ_ಸ್ಥಾನಾಂತರ;
		ಪೂರ್ಣಾಂಕ rtlil_ಸೂಚ್ಯಂಕ = upto ? ಅಗಲ - 1 - zero_ಸೂಚ್ಯಂಕ : zero_ಸೂಚ್ಯಂಕ;
		ಹಿಂತಿರುಗಿ rtlil_ಸೂಚ್ಯಂಕ >= 0 && rtlil_ಸೂಚ್ಯಂಕ < ಅಗಲ ? rtlil_ಸೂಚ್ಯಂಕ : INT_MIN;
	}

	ಪೂರ್ಣಾಂಕ ಗಮ್ಯ_hdl_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ rtlil_ಸೂಚ್ಯಂಕ) {
		ಆದರೆ (rtlil_ಸೂಚ್ಯಂಕ < 0 || rtlil_ಸೂಚ್ಯಂಕ >= ಅಗಲ)
			ಹಿಂತಿರುಗಿ INT_MIN;
		ಪೂರ್ಣಾಂಕ zero_ಸೂಚ್ಯಂಕ = upto ? ಅಗಲ - 1 - rtlil_ಸೂಚ್ಯಂಕ : rtlil_ಸೂಚ್ಯಂಕ;
		ಹಿಂತಿರುಗಿ zero_ಸೂಚ್ಯಂಕ + ಆರಂಭ_ಸ್ಥಾನಾಂತರ;
	}

	std::string ಗಮ್ಯ_rtlil_str() ಸ್ಥಿರಾಂಕ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	ಸ್ಥಿರ std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Wire*> *get_all_wires(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
};

ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ Get_ಗಾತ್ರ(RTLIL::Wire *wire) {
	ಹಿಂತಿರುಗಿ wire->ಅಗಲ;
}

ರಚನೆ RTLIL::ಸ್ಮರಣೆ : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ { h.eat(hashidx_); ಹಿಂತಿರುಗಿ h; }

	ಸ್ಮರಣೆ();
	~ಸ್ಮರಣೆ();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ module ? module->design : nullptr; }
	TwinePool &twines() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ module->twines(); }

	RTLIL::ಸ್ಮರಣೆ_Name_Id name;

	ಪೂರ್ಣಾಂಕ ಅಗಲ, ಆರಂಭ_ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ_2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	ಸ್ಥಿರ std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::ಸ್ಮರಣೆ*> *get_all_memorys(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

	std::string ಗಮ್ಯ_rtlil_str() ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ RTLIL::Cell : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
ಖಾಸಗಿ:
	ಶೂನ್ಯ init_ಸೂಚ್ಯಂಕ();

	bool bufnorm_handle_ಗಣ_ಪೋರ್ಟ್(IdString portname, RTLIL::SigSpec &ಸಂಕೇತ, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
ಸಾರ್ವಜನಿಕ:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ { h.eat(hashidx_); ಹಿಂತಿರುಗಿ h; }

ಸಂರಕ್ಷಿತ:
	// use module->addCell() and module->remove() to create or destroy cells
	ಮಿತ್ರ ರಚನೆ RTLIL::Module;
	Cell();
	~Cell();

ಸಾರ್ವಜನಿಕ:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ module ? module->design : nullptr; }
	TwinePool &twines() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool has_ಪೋರ್ಟ್(RTLIL::IdString portname) ಸ್ಥಿರಾಂಕ;
	ಶೂನ್ಯ unset_ಪೋರ್ಟ್(RTLIL::IdString portname);
	ಶೂನ್ಯ ಗಣ_ಪೋರ್ಟ್(RTLIL::IdString portname, RTLIL::SigSpec ಸಂಕೇತ);
	ಮಾದರಿ<typename N, YS_NAME_STRING(N)> ಶೂನ್ಯ ಗಣ_ಪೋರ್ಟ್(N portname, RTLIL::SigSpec ಸಂಕೇತ)
		{ ಗಣ_ಪೋರ್ಟ್(module->twines().ಕೂಡಿಸು(std::move(portname)), std::move(ಸಂಕೇತ)); }
	ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &get_ಪೋರ್ಟ್(RTLIL::IdString portname) ಸ್ಥಿರಾಂಕ;
	ಸ್ಥಿರಾಂಕ dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ಸ್ಥಿರಾಂಕ;

	bool ತಿಳಿದ() ಸ್ಥಿರಾಂಕ;
	bool ಒಳಹರಿವು(RTLIL::IdString portname) ಸ್ಥಿರಾಂಕ;
	bool output(RTLIL::IdString portname) ಸ್ಥಿರಾಂಕ;
	ಪೋರ್ಟ್_Dir ಪೋರ್ಟ್_dir(RTLIL::IdString portname) ಸ್ಥಿರಾಂಕ;

	bool hasParam(RTLIL::IdString paramname) ಸ್ಥಿರಾಂಕ;
	ಶೂನ್ಯ unsetParam(RTLIL::IdString paramname);
	ಶೂನ್ಯ ಗಣ_Param(RTLIL::IdString paramname, RTLIL::Const ಮೌಲ್ಯ);
	ಸ್ಥಿರಾಂಕ RTLIL::Const &getParam(RTLIL::IdString paramname) ಸ್ಥಿರಾಂಕ;

	ಮಾದರಿ<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ಸ್ಥಿರಾಂಕ
		{ ಹಿಂತಿರುಗಿ hasParam(module->twines().ಕಂಡುಹಿಡಿ(std::string_view(name))); }
	ಮಾದರಿ<typename N, YS_NAME_STRING(N)> ಶೂನ್ಯ unsetParam(N name)
		{ unsetParam(module->twines().ಕಂಡುಹಿಡಿ(std::string_view(name))); }
	ಮಾದರಿ<typename N, YS_NAME_STRING(N)> ಶೂನ್ಯ ಗಣ_Param(N name, RTLIL::Const ಮೌಲ್ಯ)
		{ ಗಣ_Param(module->twines().ಕೂಡಿಸು(std::move(name)), std::move(ಮೌಲ್ಯ)); }
	ಮಾದರಿ<typename N, YS_NAME_STRING(N)> ಸ್ಥಿರಾಂಕ RTLIL::Const &getParam(N name) ಸ್ಥಿರಾಂಕ
		{ ಹಿಂತಿರುಗಿ getParam(module->twines().ಕಂಡುಹಿಡಿ(std::string_view(name))); }

	ಶೂನ್ಯ ವಿಂಗಡಿಸು();
	ಶೂನ್ಯ check();
	ಶೂನ್ಯ fixup_parameters(bool ಗಣ_a_signed = false, bool ಗಣ_b_signed = false);

	bool has_keep_attr() ಸ್ಥಿರಾಂಕ;

	ಮಾದರಿ<typename T> ಶೂನ್ಯ rewrite_sigspecs(T &functor);
	ಮಾದರಿ<typename T> ಶೂನ್ಯ rewrite_sigspecs2(T &functor);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ YOSYS_ENABLE_PYTHON
	ಸ್ಥಿರ std::map<ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ, RTLIL::Cell*> *get_all_cells(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

	bool has_memid() ಸ್ಥಿರಾಂಕ;
	bool is_mem_cell() ಸ್ಥಿರಾಂಕ;
	bool is_builtin_ff() ಸ್ಥಿರಾಂಕ;

	std::string ಗಮ್ಯ_rtlil_str() ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ RTLIL::Case_ನಿಯಮ : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
	std::ಸದಿಶ<RTLIL::SigSpec> compare;
	std::ಸದಿಶ<RTLIL::SigSig> actions;
	std::ಸದಿಶ<RTLIL::Switch_ನಿಯಮ*> switches;

	~Case_ನಿಯಮ();

	bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ;

	ಮಾದರಿ<typename T> ಶೂನ್ಯ rewrite_sigspecs(T &functor);
	ಮಾದರಿ<typename T> ಶೂನ್ಯ rewrite_sigspecs2(T &functor);
	RTLIL::Case_ನಿಯಮ *clone() ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ RTLIL::Switch_ನಿಯಮ : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
	RTLIL::SigSpec ಸಂಕೇತ;
	std::ಸದಿಶ<RTLIL::Case_ನಿಯಮ*> cases;

	~Switch_ನಿಯಮ();

	bool ಖಾಲಿಗಳು() ಸ್ಥಿರಾಂಕ;

	ಮಾದರಿ<typename T> ಶೂನ್ಯ rewrite_sigspecs(T &functor);
	ಮಾದರಿ<typename T> ಶೂನ್ಯ rewrite_sigspecs2(T &functor);
	RTLIL::Switch_ನಿಯಮ *clone() ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec ವಿಳಾಸ;
	RTLIL::SigSpec ದತ್ತಾಂಶ;
	RTLIL::SigSpec enable;
	RTLIL::Const ಆದ್ಯತೆ_ಮುಖವಾಡ;
};

ರಚನೆ RTLIL::Sync_ನಿಯಮ
{
	RTLIL::SyncType type;
	RTLIL::SigSpec ಸಂಕೇತ;
	std::ಸದಿಶ<RTLIL::SigSig> actions;
	std::ಸದಿಶ<RTLIL::MemWriteAction> mem_write_actions;

	ಮಾದರಿ<typename T> ಶೂನ್ಯ rewrite_sigspecs(T &functor);
	ಮಾದರಿ<typename T> ಶೂನ್ಯ rewrite_sigspecs2(T &functor);
	RTLIL::Sync_ನಿಯಮ *clone() ಸ್ಥಿರಾಂಕ;
};

ರಚನೆ RTLIL::Process : ಸಾರ್ವಜನಿಕ RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ { h.eat(hashidx_); ಹಿಂತಿರುಗಿ h; }

ಸಂರಕ್ಷಿತ:
	// use module->addProcess() and module->remove() to create or destroy processes
	ಮಿತ್ರ ರಚನೆ RTLIL::Module;
	Process();
	~Process();

ಸಾರ್ವಜನಿಕ:
	RTLIL::Module *module;
	RTLIL::Case_ನಿಯಮ ಬೇರು_case;
	std::ಸದಿಶ<RTLIL::Sync_ನಿಯಮ*> syncs;

	RTLIL::Design *design() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ module ? module->design : nullptr; }
	TwinePool &twines() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ module->twines(); }

	ಮಾದರಿ<typename T> ಶೂನ್ಯ rewrite_sigspecs(T &functor);
	ಮಾದರಿ<typename T> ಶೂನ್ಯ rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ಸ್ಥಿರಾಂಕ;

	std::string ಗಮ್ಯ_rtlil_str() ಸ್ಥಿರಾಂಕ;
};

ಸಾಲೊಳಗೆ RTLIL::Sig_ಬಿಟ್::Sig_ಬಿಟ್() : wire(ಖಾಲಿ), ದತ್ತಾಂಶ(RTLIL::State::S0) { }
ಸಾಲೊಳಗೆ RTLIL::Sig_ಬಿಟ್::Sig_ಬಿಟ್(RTLIL::State ಬಿಟ್) : wire(ಖಾಲಿ), ದತ್ತಾಂಶ(ಬಿಟ್) { }
ಸಾಲೊಳಗೆ RTLIL::Sig_ಬಿಟ್::Sig_ಬಿಟ್(bool ಬಿಟ್) : wire(ಖಾಲಿ), ದತ್ತಾಂಶ(ಬಿಟ್ ? State::S1 : State::S0) { }
ಸಾಲೊಳಗೆ RTLIL::Sig_ಬಿಟ್::Sig_ಬಿಟ್(RTLIL::Wire *wire) : wire(wire), ಸ್ಥಾನಾಂತರ(0) { log_assert(wire && wire->ಅಗಲ == 1); }
ಸಾಲೊಳಗೆ RTLIL::Sig_ಬಿಟ್::Sig_ಬಿಟ್(RTLIL::Wire *wire, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ) : wire(wire), ಸ್ಥಾನಾಂತರ(ಸ್ಥಾನಾಂತರ) { log_assert(wire != nullptr); }
ಸಾಲೊಳಗೆ RTLIL::Sig_ಬಿಟ್::Sig_ಬಿಟ್(ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.ಅಗಲ == 1); ಆದರೆ (wire) ಸ್ಥಾನಾಂತರ = chunk.ಸ್ಥಾನಾಂತರ; ಇಲ್ಲದಿದ್ದರೆ ದತ್ತಾಂಶ = chunk.ದತ್ತಾಂಶ[0]; }
ಸಾಲೊಳಗೆ RTLIL::Sig_ಬಿಟ್::Sig_ಬಿಟ್(ಸ್ಥಿರಾಂಕ RTLIL::SigChunk &chunk, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) : wire(chunk.wire) { ಆದರೆ (wire) ಸ್ಥಾನಾಂತರ = chunk.ಸ್ಥಾನಾಂತರ + ಸೂಚ್ಯಂಕ; ಇಲ್ಲದಿದ್ದರೆ ದತ್ತಾಂಶ = chunk.ದತ್ತಾಂಶ[ಸೂಚ್ಯಂಕ]; }

ಸಾಲೊಳಗೆ bool RTLIL::Sig_ಬಿಟ್::ಕ್ರಿಯಾಕಾರಕ<(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &other) ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (wire == other.wire)
		ಹಿಂತಿರುಗಿ wire ? (ಸ್ಥಾನಾಂತರ < other.ಸ್ಥಾನಾಂತರ) : (ದತ್ತಾಂಶ < other.ದತ್ತಾಂಶ);
	ಆದರೆ (wire != nullptr && other.wire != nullptr)
		ಹಿಂತಿರುಗಿ wire->name < other.wire->name;
	ಹಿಂತಿರುಗಿ (wire != nullptr) < (other.wire != nullptr);
}

ಸಾಲೊಳಗೆ bool RTLIL::Sig_ಬಿಟ್::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &other) ಸ್ಥಿರಾಂಕ {
	ಹಿಂತಿರುಗಿ (wire == other.wire) && (wire ? (ಸ್ಥಾನಾಂತರ == other.ಸ್ಥಾನಾಂತರ) : (ದತ್ತಾಂಶ == other.ದತ್ತಾಂಶ));
}

ಸಾಲೊಳಗೆ bool RTLIL::Sig_ಬಿಟ್::ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &other) ಸ್ಥಿರಾಂಕ {
	ಹಿಂತಿರುಗಿ (wire != other.wire) || (wire ? (ಸ್ಥಾನಾಂತರ != other.ಸ್ಥಾನಾಂತರ) : (ದತ್ತಾಂಶ != other.ದತ್ತಾಂಶ));
}

ಸಾಲೊಳಗೆ Hasher RTLIL::Sig_ಬಿಟ್::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
	ಆದರೆ (wire) {
		h.eat(ಸ್ಥಾನಾಂತರ);
		h.eat(wire->name);
		ಹಿಂತಿರುಗಿ h;
	}
	h.eat(ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ h;
}


ಸಾಲೊಳಗೆ Hasher RTLIL::Sig_ಬಿಟ್::hash_ತುದಿ() ಸ್ಥಿರಾಂಕ {
	Hasher h;
	ಆದರೆ (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.ಬಿಟ್ಟುಗಳು();
		h.force(hashlib::legacy::djb2_ಕೂಡಿಸು(n, ಸ್ಥಾನಾಂತರ));
		ಹಿಂತಿರುಗಿ h;
	}
	h.force(ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ h;
}

ಸಾಲೊಳಗೆ RTLIL::Sig_ಬಿಟ್ &RTLIL::SigSpecIterator::ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ {
	ಹಿಂತಿರುಗಿ (*sig_p)[ಸೂಚ್ಯಂಕ];
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ RTLIL::Sig_ಬಿಟ್ &RTLIL::SigSpecConstIterator::ಕ್ರಿಯಾಕಾರಕ*() {
	ಬಿಟ್ = (*sig_p)[ಸೂಚ್ಯಂಕ];
	ಹಿಂತಿರುಗಿ ಬಿಟ್;
}

ಸಾಲೊಳಗೆ RTLIL::Sig_ಬಿಟ್::Sig_ಬಿಟ್(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig) {
	log_assert(sig.ಗಾತ್ರ_2() == 1);
	ಸ್ವಯಂಚಾಲಿತ it = sig.chunks().begin();
	*this = Sig_ಬಿಟ್(*it);
}

ಮಾದರಿ<typename T>
ಶೂನ್ಯ RTLIL::Module::rewrite_sigspecs(T &functor)
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : cells_)
		it.ಎರಡನೇ->rewrite_sigspecs(functor);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : processes)
		it.ಎರಡನೇ->rewrite_sigspecs(functor);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : connections_) {
		functor(it.ಮೊದಲ);
		functor(it.ಎರಡನೇ);
	}
}

ಮಾದರಿ<typename T>
ಶೂನ್ಯ RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : cells_)
		it.ಎರಡನೇ->rewrite_sigspecs2(functor);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : processes)
		it.ಎರಡನೇ->rewrite_sigspecs2(functor);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : connections_) {
		functor(it.ಮೊದಲ, it.ಎರಡನೇ);
	}
}

ಮಾದರಿ<typename T>
ಶೂನ್ಯ RTLIL::Cell::rewrite_sigspecs(T &functor) {
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : connections_)
		functor(it.ಎರಡನೇ);
}

ಮಾದರಿ<typename T>
ಶೂನ್ಯ RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : connections_)
		functor(it.ಎರಡನೇ);
}

ಮಾದರಿ<typename T>
ಶೂನ್ಯ RTLIL::Case_ನಿಯಮ::rewrite_sigspecs(T &functor) {
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : compare)
		functor(it);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : actions) {
		functor(it.ಮೊದಲ);
		functor(it.ಎರಡನೇ);
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it : switches)
		it->rewrite_sigspecs(functor);
}

ಮಾದರಿ<typename T>
ಶೂನ್ಯ RTLIL::Case_ನಿಯಮ::rewrite_sigspecs2(T &functor) {
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : compare)
		functor(it);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : actions) {
		functor(it.ಮೊದಲ, it.ಎರಡನೇ);
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it : switches)
		it->rewrite_sigspecs2(functor);
}

ಮಾದರಿ<typename T>
ಶೂನ್ಯ RTLIL::Switch_ನಿಯಮ::rewrite_sigspecs(T &functor)
{
	functor(ಸಂಕೇತ);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it : cases)
		it->rewrite_sigspecs(functor);
}

ಮಾದರಿ<typename T>
ಶೂನ್ಯ RTLIL::Switch_ನಿಯಮ::rewrite_sigspecs2(T &functor)
{
	functor(ಸಂಕೇತ);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it : cases)
		it->rewrite_sigspecs2(functor);
}

ಮಾದರಿ<typename T>
ಶೂನ್ಯ RTLIL::Sync_ನಿಯಮ::rewrite_sigspecs(T &functor)
{
	functor(ಸಂಕೇತ);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : actions) {
		functor(it.ಮೊದಲ);
		functor(it.ಎರಡನೇ);
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : mem_write_actions) {
		functor(it.ವಿಳಾಸ);
		functor(it.ದತ್ತಾಂಶ);
		functor(it.enable);
	}
}

ಮಾದರಿ<typename T>
ಶೂನ್ಯ RTLIL::Sync_ನಿಯಮ::rewrite_sigspecs2(T &functor)
{
	functor(ಸಂಕೇತ);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : actions) {
		functor(it.ಮೊದಲ, it.ಎರಡನೇ);
	}
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : mem_write_actions) {
		functor(it.ವಿಳಾಸ);
		functor(it.ದತ್ತಾಂಶ);
		functor(it.enable);
	}
}

ಮಾದರಿ<typename T>
ಶೂನ್ಯ RTLIL::Process::rewrite_sigspecs(T &functor)
{
	ಬೇರು_case.rewrite_sigspecs(functor);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it : syncs)
		it->rewrite_sigspecs(functor);
}

ಮಾದರಿ<typename T>
ಶೂನ್ಯ RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	ಬೇರು_case.rewrite_sigspecs2(functor);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it : syncs)
		it->rewrite_sigspecs2(functor);
}

#ಅನ್ತರ್ಭಾವ "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
