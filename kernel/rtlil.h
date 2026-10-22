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

#నిర్వచితం_కాకపోతే RTLIL_H
#నిర్వచించు RTLIL_H

#అన్తర్భావ "kernel/yosys_common.h"
#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/twine.h"

#అన్తర్భావ <deque>
#అన్తర్భావ <string_view>
#అన్తర్భావ <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	గణన State : గుర్తులేని అక్షరం {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	గణన SyncType : గుర్తులేని అక్షరం {
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
	గణన ConstFlags : గుర్తులేని అక్షరం {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	గణన SelectPartials : గుర్తులేని అక్షరం {
		SELECT_ALL = 0,          // include partial modules
		SELECT_సంపూర్ణ_ONLY = 1,   // ignore partial modules
		SELECT_సంపూర్ణ_WARN = 2,   // call log_warning on partial module
		SELECT_సంపూర్ణ_ERR = 3,    // call log_error on partial module
		SELECT_సంపూర్ణ_CMDERR = 4  // call log_cmd_error on partial module
	};

	గణన SelectBoxes : గుర్తులేని అక్షరం {
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

	గణన పోర్టు_Dir : గుర్తులేని అక్షరం  {
		PD_UNKNOWN = 0,
		PD_ఆదానం = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr పూర్ణాంకం వెడల్పు_హద్దు = 1 << 30;

	నిర్మాణం Const;
	నిర్మాణం AttrObject;
	నిర్మాణం SelectedMember;
	నిర్మాణం Selection;
	నిర్మాణం Monitor;
	నిర్మాణం Design;
	నిర్మాణం Module;
	నిర్మాణం Wire;
	నిర్మాణం జ్ఞాపకం;
	నిర్మాణం Cell;
	నిర్మాణం SigChunk;
	నిర్మాణం Sig_బిట్టు;
	నిర్మాణం SigSpecIterator;
	నిర్మాణం SigSpecConstIterator;
	నిర్మాణం SigSpec;
	నిర్మాణం Case_నియమం;
	నిర్మాణం Switch_నియమం;
	నిర్మాణం MemWriteAction;
	నిర్మాణం Sync_నియమం;
	నిర్మాణం Process;

	రకనిర్వచనం std::pair<SigSpec, SigSpec> SigSig;
};

నిర్మాణం SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_ఉపసర్గ ID; }
namespace RTLIL { using YOSYS_NAMESPACE_ఉపసర్గ IdString; }

namespace RTLIL {
	శూన్య copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			స్థిరరాశి dict<IdString, RTLIL::Const> &src,
			స్థిరరాశి RTLIL::Design *src_design, RTLIL::Design *dst_design);

	బాహ్య dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	స్థిరరాశి pool<IdString> &builtin_ff_cell_types();

	స్థిర అంతఃవరుస std::string escape_id(స్థిరరాశి std::string &str) {
		ఒకవేళ (str.కొలత() > 0 && str[0] != '\\' && str[0] != '$')
			తిరిగి "\\" + str;
		తిరిగి str;
	}

	స్థిర అంతఃవరుస std::string unescape_id(స్థిరరాశి std::string &str) {
		ఒకవేళ (str.కొలత() < 2)
			తిరిగి str;
		ఒకవేళ (str[0] != '\\')
			తిరిగి str;
		ఒకవేళ (str[1] == '$' || str[1] == '\\')
			తిరిగి str;
		ఒకవేళ (str[1] >= '0' && str[1] <= '9')
			తిరిగి str;
		తిరిగి str.substr(1);
	}

	మూస <typename T> using క్రమీకరించు_by_name_id = IdString::compare_ptr_by_name<T>;

	మూస <typename T> నిర్మాణం క్రమీకరించు_by_name_str {
		bool క్రియాకారకం()(T *a, T *b) స్థిరరాశి {
			తిరిగి a->name.lt_by_name(b->name);
		}
	};

	నిర్మాణం క్రమీకరించు_by_id_str {
		స్థిరరాశి TwinePool& pool;
		explicit క్రమీకరించు_by_id_str(స్థిరరాశి TwinePool& pool) : pool(pool) {}
		bool క్రియాకారకం()(RTLIL::IdString a, RTLIL::IdString b) స్థిరరాశి {
			తిరిగి pool.compare_by_name(a, b) < 0;
		}
	};

	స్థిర అంతఃవరుస std::string encode_filename(స్థిరరాశి std::string &filename)
	{
		std::stringstream val;
		ఒకవేళ (!std::any_యొక్క(filename.begin(), filename.చివర(), [](అక్షరం c) {
			తిరిగి static_cast<గుర్తులేని అక్షరం>(c) < 33 || static_cast<గుర్తులేని అక్షరం>(c) > 126;
		})) తిరిగి filename;
		కొరకు (గుర్తులేని అక్షరం స్థిరరాశి c : filename) {
			ఒకవేళ (c < 33 || c > 126)
				val << stringf("$%02x", c);
			లేకపోతే
				val << c;
		}
		తిరిగి val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_and         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_or          (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_xor         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_xnor        (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);

	RTLIL::Const const_reduce_and  (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_reduce_or   (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_reduce_xor  (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_reduce_xnor (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_reduce_bool (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);

	RTLIL::Const const_logic_not   (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_logic_and   (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_logic_or    (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);

	RTLIL::Const const_shl         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_shr         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_sshl        (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_sshr        (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_జరుపు       (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_shiftx      (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);

	RTLIL::Const const_lt          (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_le          (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_eq          (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_ne          (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_eqx         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_nex         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_ge          (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_gt          (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);

	RTLIL::Const const_కూడు         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_sub         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_mul         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_div         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_divfloor    (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_modfloor    (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_mod         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_pow         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);

	RTLIL::Const const_pos         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_buf         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);
	RTLIL::Const const_neg         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len);

	RTLIL::Const const_mux         (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, స్థిరరాశి RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, స్థిరరాశి RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2);
	RTLIL::Const const_demux       (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, స్థిరరాశి RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	మూస<typename T, typename కీలకం_2 = IdString>
	నిర్మాణం ObjIterator {
		using iterator_category = std::forward_iterator_ట్యాగ్;
		using విలువ_type = T;
		using difference_type = ptrdiff_t;
		using సూచిక = T*;
		using reference = T&;
		typename dict<కీలకం_2, T>::iterator it;
		dict<కీలకం_2, T> *list_p;
		పూర్ణాంకం *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, పూర్ణాంకం *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			ఒకవేళ (list_p->ఖాళీలు()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} లేకపోతే {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(స్థిరరాశి RTLIL::ObjIterator<T, కీలకం_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			ఒకవేళ (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &క్రియాకారకం=(స్థిరరాశి RTLIL::ObjIterator<T, కీలకం_2> &other) {
			ఒకవేళ (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			ఒకవేళ (refcount_p)
				(*refcount_p)++;
			తిరిగి *this;
		}

		~ObjIterator() {
			ఒకవేళ (refcount_p)
				(*refcount_p)--;
		}

		అంతఃవరుస T క్రియాకారకం*() స్థిరరాశి {
			log_assert(list_p != nullptr);
			తిరిగి it->రెండవ;
		}

		అంతఃవరుస bool క్రియాకారకం!=(స్థిరరాశి RTLIL::ObjIterator<T, కీలకం_2> &other) స్థిరరాశి {
			ఒకవేళ (list_p == nullptr || other.list_p == nullptr)
				తిరిగి list_p != other.list_p;
			తిరిగి it != other.it;
		}


		అంతఃవరుస bool క్రియాకారకం==(స్థిరరాశి RTLIL::ObjIterator<T, కీలకం_2> &other) స్థిరరాశి {
			తిరిగి !(*this != other);
		}

		అంతఃవరుస ObjIterator<T, కీలకం_2>& క్రియాకారకం++() {
			log_assert(list_p != nullptr);
			ఒకవేళ (++it == list_p->చివర()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			తిరిగి *this;
		}

		అంతఃవరుస ObjIterator<T, కీలకం_2>& క్రియాకారకం+=(పూర్ణాంకం amt) {
			log_assert(list_p != nullptr);
			it += amt;
			ఒకవేళ (it == list_p->చివర()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			తిరిగి *this;
		}

		అంతఃవరుస ObjIterator<T, కీలకం_2> క్రియాకారకం+(పూర్ణాంకం amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, కీలకం_2> new_obj(*this);
			new_obj.it += amt;
			ఒకవేళ (new_obj.it == list_p->చివర()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			తిరిగి new_obj;
		}

		అంతఃవరుస స్థిరరాశి ObjIterator<T, కీలకం_2> క్రియాకారకం++(పూర్ణాంకం) {
			ObjIterator<T, కీలకం_2> ఫలితం(*this);
			++(*this);
			తిరిగి ఫలితం;
		}
	};

	మూస<typename T, typename కీలకం_2 = IdString>
	నిర్మాణం ObjRange
	{
		dict<కీలకం_2, T> *list_p;
		పూర్ణాంకం *refcount_p;

		ObjRange(decltype(list_p) list_p, పూర్ణాంకం *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, కీలకం_2> begin() { తిరిగి RTLIL::ObjIterator<T, కీలకం_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, కీలకం_2> చివర() { తిరిగి RTLIL::ObjIterator<T, కీలకం_2>(); }

		కొలత_t కొలత() స్థిరరాశి {
			తిరిగి list_p->కొలత();
		}

		క్రియాకారకం pool<T>() స్థిరరాశి {
			pool<T> ఫలితం;
			కొరకు (స్వయంచాలక &it : *list_p)
				ఫలితం.చొప్పించు(it.రెండవ);
			తిరిగి ఫలితం;
		}

		క్రియాకారకం std::సదిశ<T>() స్థిరరాశి {
			std::సదిశ<T> ఫలితం;
			ఫలితం.reserve(list_p->కొలత());
			కొరకు (స్వయంచాలక &it : *list_p)
				ఫలితం.నెట్టు_వెనుక(it.రెండవ);
			తిరిగి ఫలితం;
		}

		pool<T> గమ్యం_pool() స్థిరరాశి { తిరిగి *this; }
		std::సదిశ<T> గమ్యం_సదిశ() స్థిరరాశి { తిరిగి *this; }
	};
};

నిర్మాణం RTLIL::Const
{
	పొట్టి పూర్ణాంకం flags;
అంతరంగ:
	మిత్ర class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::సదిశ<RTLIL::State>;
	గణన class backing_ట్యాగ్: bool { బిట్లు, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_ట్యాగ్ ట్యాగ్;
	సంఘం {
		bitvectype బిట్లు_2;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_బిట్లు() స్థిరరాశి { తిరిగి ట్యాగ్ == backing_ట్యాగ్::బిట్లు; }
	bool is_str() స్థిరరాశి { తిరిగి ట్యాగ్ == backing_ట్యాగ్::string; }

	bitvectype* get_if_బిట్లు() { తిరిగి is_బిట్లు() ? &బిట్లు_2 : ఖాళీ; }
	std::string* get_if_str() { తిరిగి is_str() ? &str_ : ఖాళీ; }
	స్థిరరాశి bitvectype* get_if_బిట్లు() స్థిరరాశి { తిరిగి is_బిట్లు() ? &బిట్లు_2 : ఖాళీ; }
	స్థిరరాశి std::string* get_if_str() స్థిరరాశి { తిరిగి is_str() ? &str_ : ఖాళీ; }

	bitvectype& get_బిట్లు();
	std::string& get_str();
	స్థిరరాశి bitvectype& get_బిట్లు() స్థిరరాశి;
	స్థిరరాశి std::string& get_str() స్థిరరాశి;
	std::సదిశ<RTLIL::State>& బిట్లు_అంతర్గత();
	శూన్య bitvectorize_అంతర్గత();

బహిరంగ:
	Const() : flags(RTLIL::CONST_FLAG_NONE), ట్యాగ్(backing_ట్యాగ్::బిట్లు), బిట్లు_2(std::సదిశ<RTLIL::State>()) {}
	Const(std::string str);
	Const(దీర్ఘ దీర్ఘ val); // default width is 32
	Const(దీర్ఘ దీర్ఘ val, పూర్ణాంకం వెడల్పు);
	Const(RTLIL::State బిట్టు, పూర్ణాంకం వెడల్పు = 1);
	Const(std::సదిశ<RTLIL::State> బిట్లు) : flags(RTLIL::CONST_FLAG_NONE), ట్యాగ్(backing_ట్యాగ్::బిట్లు), బిట్లు_2(std::move(బిట్లు)) {}
	Const(స్థిరరాశి std::సదిశ<bool> &బిట్లు);
	Const(స్థిరరాశి RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &క్రియాకారకం =(స్థిరరాశి RTLIL::Const &other);
	~Const();

	నిర్మాణం Builder
	{
		Builder() {}
		Builder(పూర్ణాంకం expected_వెడల్పు) { బిట్లు.reserve(expected_వెడల్పు); }
		శూన్య నెట్టు_వెనుక(RTLIL::State b) { బిట్లు.నెట్టు_వెనుక(b); }
		పూర్ణాంకం కొలత() స్థిరరాశి { తిరిగి static_cast<పూర్ణాంకం>(బిట్లు.కొలత()); }
		Const build() { తిరిగి Const(std::move(బిట్లు)); }
	అంతరంగ:
		std::సదిశ<RTLIL::State> బిట్లు;
	};

	bool క్రియాకారకం <(స్థిరరాశి RTLIL::Const &other) స్థిరరాశి;
	bool క్రియాకారకం ==(స్థిరరాశి RTLIL::Const &other) స్థిరరాశి;
	bool క్రియాకారకం !=(స్థిరరాశి RTLIL::Const &other) స్థిరరాశి;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::సదిశ<RTLIL::State>& బిట్లు() { తిరిగి బిట్లు_అంతర్గత(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	శూన్య bitvectorize() స్థిరరాశి { const_cast<Const*>(this)->bitvectorize_అంతర్గత(); }

	bool as_bool() స్థిరరాశి;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	పూర్ణాంకం as_int(bool is_signed = false) స్థిరరాశి;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_గమ్యం_int(bool is_signed = false) స్థిరరాశి;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<పూర్ణాంకం> try_as_int(bool is_signed = false) స్థిరరాశి;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	పూర్ణాంకం as_int_saturating(bool is_signed = false) స్థిరరాశి;

	శూన్య ట్యాగ్_bare_integer_const(స్థిరరాశి std::string &విలువ);

	std::string as_string(స్థిరరాశి అక్షరం* any = "-") స్థిరరాశి;
	స్థిర Const from_string(స్థిరరాశి std::string &str);
	std::సదిశ<RTLIL::State> గమ్యం_బిట్లు() స్థిరరాశి;

	std::string decode_string() స్థిరరాశి;
	పూర్ణాంకం కొలత() స్థిరరాశి;
	bool ఖాళీలు() స్థిరరాశి;

	శూన్య append(స్థిరరాశి RTLIL::Const &other);
	శూన్య సమితి(పూర్ణాంకం i, RTLIL::State state) {
		బిట్లు_అంతర్గత()[i] = state;
	}
	శూన్య resize(పూర్ణాంకం కొలత, RTLIL::State fill) {
    log_assert(కొలత >= 0 && కొలత < RTLIL::వెడల్పు_హద్దు);
		బిట్లు_అంతర్గత().resize(కొలత, fill);
	}

	class const_iterator {
	అంతరంగ:
		స్థిరరాశి Const* మాతృ;
		కొలత_t idx;

	బహిరంగ:
		using iterator_category = std::bidirectional_iterator_ట్యాగ్;
		using విలువ_type = State;
		using difference_type = std::ptrdiff_t;
		using సూచిక = స్థిరరాశి State*;
		using reference = స్థిరరాశి State&;

		const_iterator(స్థిరరాశి Const& c, కొలత_t i) : మాతృ(&c), idx(i) {}

		State క్రియాకారకం*() స్థిరరాశి;

		const_iterator& క్రియాకారకం++() { ++idx; తిరిగి *this; }
		const_iterator& క్రియాకారకం--() { --idx; తిరిగి *this; }
		const_iterator క్రియాకారకం++(పూర్ణాంకం) { const_iterator ఫలితం(*this); ++idx; తిరిగి ఫలితం; }
		const_iterator క్రియాకారకం--(పూర్ణాంకం) { const_iterator ఫలితం(*this); --idx; తిరిగి ఫలితం; }
		const_iterator& క్రియాకారకం+=(పూర్ణాంకం i) { idx += i; తిరిగి *this; }

		const_iterator క్రియాకారకం+(పూర్ణాంకం కూడు) {
			తిరిగి const_iterator(*మాతృ, idx + కూడు);
		}
		const_iterator క్రియాకారకం-(పూర్ణాంకం sub) {
			తిరిగి const_iterator(*మాతృ, idx - sub);
		}
		పూర్ణాంకం క్రియాకారకం-(స్థిరరాశి const_iterator& other) {
			తిరిగి idx - other.idx;
		}

		bool క్రియాకారకం==(స్థిరరాశి const_iterator& other) స్థిరరాశి {
			తిరిగి idx == other.idx;
		}

		bool క్రియాకారకం!=(స్థిరరాశి const_iterator& other) స్థిరరాశి {
			తిరిగి !(*this == other);
		}
	};

	class iterator {
	అంతరంగ:
		Const* మాతృ;
		కొలత_t idx;

	బహిరంగ:
		class proxy {
		అంతరంగ:
			Const* మాతృ;
			కొలత_t idx;
		బహిరంగ:
			proxy(Const* మాతృ, కొలత_t idx) : మాతృ(మాతృ), idx(idx) {}
			క్రియాకారకం State() స్థిరరాశి { తిరిగి (*మాతృ)[idx]; }
			proxy& క్రియాకారకం=(State s) { మాతృ->సమితి(idx, s); తిరిగి *this; }
			proxy& క్రియాకారకం=(స్థిరరాశి proxy& other) { మాతృ->సమితి(idx, (*other.మాతృ)[other.idx]); తిరిగి *this; }
		};

		using iterator_category = std::bidirectional_iterator_ట్యాగ్;
		using విలువ_type = State;
		using difference_type = std::ptrdiff_t;
		using సూచిక = proxy*;
		using reference = proxy;

		iterator(Const& c, కొలత_t i) : మాతృ(&c), idx(i) {}

		proxy క్రియాకారకం*() స్థిరరాశి { తిరిగి proxy(మాతృ, idx); }
		iterator& క్రియాకారకం++() { ++idx; తిరిగి *this; }
		iterator& క్రియాకారకం--() { --idx; తిరిగి *this; }
		iterator క్రియాకారకం++(పూర్ణాంకం) { iterator ఫలితం(*this); ++idx; తిరిగి ఫలితం; }
		iterator క్రియాకారకం--(పూర్ణాంకం) { iterator ఫలితం(*this); --idx; తిరిగి ఫలితం; }
		iterator& క్రియాకారకం+=(పూర్ణాంకం i) { idx += i; తిరిగి *this; }

		iterator క్రియాకారకం+(పూర్ణాంకం కూడు) {
			తిరిగి iterator(*మాతృ, idx + కూడు);
		}
		iterator క్రియాకారకం-(పూర్ణాంకం sub) {
			తిరిగి iterator(*మాతృ, idx - sub);
		}
		పూర్ణాంకం క్రియాకారకం-(స్థిరరాశి iterator& other) {
			తిరిగి idx - other.idx;
		}

		bool క్రియాకారకం==(స్థిరరాశి iterator& other) స్థిరరాశి {
			తిరిగి idx == other.idx;
		}

		bool క్రియాకారకం!=(స్థిరరాశి iterator& other) స్థిరరాశి {
			తిరిగి !(*this == other);
		}
	};

	const_iterator begin() స్థిరరాశి {
		తిరిగి const_iterator(*this, 0);
	}
	const_iterator చివర() స్థిరరాశి {
		తిరిగి const_iterator(*this, కొలత());
	}
	iterator begin() {
		తిరిగి iterator(*this, 0);
	}
	iterator చివర() {
		తిరిగి iterator(*this, కొలత());
	}
	State వెనుక() స్థిరరాశి {
		తిరిగి *(చివర() - 1);
	}
	State front() స్థిరరాశి {
		తిరిగి *begin();
	}
	State at(కొలత_t i) స్థిరరాశి {
		తిరిగి *const_iterator(*this, i);
	}
	State క్రియాకారకం[](కొలత_t i) స్థిరరాశి {
		తిరిగి *const_iterator(*this, i);
	}

	bool is_fully_zero() స్థిరరాశి;
	bool is_fully_ones() స్థిరరాశి;
	bool is_fully_def() స్థిరరాశి;
	bool is_fully_undef() స్థిరరాశి;
	bool is_fully_undef_x_only() స్థిరరాశి;
	bool is_onehot(పూర్ణాంకం *pos = nullptr) స్థిరరాశి;

	RTLIL::Const వెలికితీయి(పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం len = 1, RTLIL::State padding = RTLIL::State::S0) స్థిరరాశి;

	// find the MSB without redundant leading bits
	పూర్ణాంకం get_min_కొలత(bool is_signed) స్థిరరాశి;

	// compress representation to the minimum required bits
	శూన్య compress(bool is_signed = false);

	std::optional<పూర్ణాంకం> as_int_compress(bool is_signed) స్థిరరాశి;

	శూన్య extu(పూర్ణాంకం వెడల్పు) {
		resize(వెడల్పు, RTLIL::State::S0);
	}

	శూన్య exts(పూర్ణాంకం వెడల్పు) {
		resize(వెడల్పు, ఖాళీలు() ? RTLIL::State::Sx : వెనుక());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;
};

నిర్మాణం RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> లక్షణాలు;

	bool has_attribute(RTLIL::IdString id) స్థిరరాశి;

	శూన్య సమితి_bool_attribute(RTLIL::IdString id, bool విలువ=true);
	bool get_bool_attribute(RTLIL::IdString id) స్థిరరాశి;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) స్థిరరాశి {
		తిరిగి get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	శూన్య సమితి_string_attribute(RTLIL::IdString id, string విలువ);
	string get_string_attribute(RTLIL::IdString id) స్థిరరాశి;

	శూన్య సమితి_strpool_attribute(RTLIL::IdString id, స్థిరరాశి pool<string> &దత్తాంశం);
	శూన్య కూడు_strpool_attribute(RTLIL::IdString id, స్థిరరాశి pool<string> &దత్తాంశం);
	pool<string> get_strpool_attribute(RTLIL::IdString id) స్థిరరాశి;

	శూన్య సమితి_src_attribute(స్థిరరాశి std::string &src) {
		సమితి_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() స్థిరరాశి {
		తిరిగి get_string_attribute(ID::src);
	}

	శూన్య సమితి_hdlname_attribute(స్థిరరాశి సదిశ<string> &hierarchy);
	సదిశ<string> get_hdlname_attribute() స్థిరరాశి;

	శూన్య సమితి_intvec_attribute(RTLIL::IdString id, స్థిరరాశి సదిశ<పూర్ణాంకం> &దత్తాంశం);
	సదిశ<పూర్ణాంకం> get_intvec_attribute(RTLIL::IdString id) స్థిరరాశి;
};

#అన్తర్భావ "kernel/rtlil_twine_compat.h"

నిర్మాణం RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

నిర్మాణం RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::సదిశ<RTLIL::State> దత్తాంశం; // only used if wire == NULL, LSB at index 0
	పూర్ణాంకం వెడల్పు, స్థానభ్రంశం;

	SigChunk() : wire(nullptr), వెడల్పు(0), స్థానభ్రంశం(0) {}
	SigChunk(స్థిరరాశి RTLIL::Const &విలువ) : wire(nullptr), దత్తాంశం(విలువ.గమ్యం_బిట్లు()), వెడల్పు(Get_కొలత(దత్తాంశం)), స్థానభ్రంశం(0) {}
	SigChunk(RTLIL::Const &&విలువ) : wire(nullptr), దత్తాంశం(విలువ.గమ్యం_బిట్లు()), వెడల్పు(Get_కొలత(దత్తాంశం)), స్థానభ్రంశం(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), వెడల్పు(Get_కొలత(wire)), స్థానభ్రంశం(0) {}
	SigChunk(RTLIL::Wire *wire, పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం వెడల్పు = 1) : wire(wire), వెడల్పు(వెడల్పు), స్థానభ్రంశం(స్థానభ్రంశం) {}
	SigChunk(స్థిరరాశి std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(పూర్ణాంకం val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(పూర్ణాంకం val, పూర్ణాంకం వెడల్పు) : SigChunk(RTLIL::Const(val, వెడల్పు)) {}
	SigChunk(RTLIL::State బిట్టు, పూర్ణాంకం వెడల్పు = 1) : SigChunk(RTLIL::Const(బిట్టు, వెడల్పు)) {}
	SigChunk(స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు);

	RTLIL::SigChunk వెలికితీయి(పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం నిడివి) స్థిరరాశి;
	RTLIL::Sig_బిట్టు క్రియాకారకం[](పూర్ణాంకం స్థానభ్రంశం) స్థిరరాశి;
	అంతఃవరుస పూర్ణాంకం కొలత() స్థిరరాశి { తిరిగి వెడల్పు; }
	అంతఃవరుస bool is_wire() స్థిరరాశి { తిరిగి wire != ఖాళీ; }

	bool క్రియాకారకం <(స్థిరరాశి RTLIL::SigChunk &other) స్థిరరాశి;
	bool క్రియాకారకం ==(స్థిరరాశి RTLIL::SigChunk &other) స్థిరరాశి;
	bool క్రియాకారకం !=(స్థిరరాశి RTLIL::SigChunk &other) స్థిరరాశి;
};

నిర్మాణం RTLIL::Sig_బిట్టు
{
	RTLIL::Wire *wire;
	సంఘం {
		RTLIL::State దత్తాంశం; // used if wire == NULL
		పూర్ణాంకం స్థానభ్రంశం;        // used if wire != NULL
	};

	Sig_బిట్టు();
	Sig_బిట్టు(RTLIL::State బిట్టు);
	explicit Sig_బిట్టు(bool బిట్టు);
	Sig_బిట్టు(RTLIL::Wire *wire);
	Sig_బిట్టు(RTLIL::Wire *wire, పూర్ణాంకం స్థానభ్రంశం);
	Sig_బిట్టు(స్థిరరాశి RTLIL::SigChunk &chunk);
	Sig_బిట్టు(స్థిరరాశి RTLIL::SigChunk &chunk, పూర్ణాంకం సూచీ);
	Sig_బిట్టు(స్థిరరాశి RTLIL::SigSpec &sig);
	Sig_బిట్టు(స్థిరరాశి RTLIL::Sig_బిట్టు &sigbit) = అప్రమేయ;
	RTLIL::Sig_బిట్టు &క్రియాకారకం =(స్థిరరాశి RTLIL::Sig_బిట్టు &other) = అప్రమేయ;

	అంతఃవరుస bool is_wire() స్థిరరాశి { తిరిగి wire != ఖాళీ; }

	bool క్రియాకారకం <(స్థిరరాశి RTLIL::Sig_బిట్టు &other) స్థిరరాశి;
	bool క్రియాకారకం ==(స్థిరరాశి RTLIL::Sig_బిట్టు &other) స్థిరరాశి;
	bool క్రియాకారకం !=(స్థిరరాశి RTLIL::Sig_బిట్టు &other) స్థిరరాశి;
	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి;
	[[nodiscard]] Hasher hash_శిఖరం() స్థిరరాశి;
};

namespace hashlib {
	మూస <>
	నిర్మాణం hash_ops<RTLIL::Sig_బిట్టు> {
		స్థిర అంతఃవరుస bool cmp(స్థిరరాశి RTLIL::Sig_బిట్టు &a, స్థిరరాశి RTLIL::Sig_బిట్టు &b) {
			తిరిగి a == b;
		}
		[[nodiscard]] స్థిర అంతఃవరుస Hasher hash(స్థిరరాశి RTLIL::Sig_బిట్టు sb) {
			తిరిగి sb.hash_శిఖరం();
		}
		[[nodiscard]] స్థిర అంతఃవరుస Hasher hash_into(స్థిరరాశి RTLIL::Sig_బిట్టు sb, Hasher h) {
			తిరిగి sb.hash_into(h);
		}
	};
};

నిర్మాణం RTLIL::SigSpecIterator
{
	రకనిర్వచనం std::ఆదానం_iterator_ట్యాగ్ iterator_category;
	రకనిర్వచనం RTLIL::Sig_బిట్టు విలువ_type;
	రకనిర్వచనం ptrdiff_t difference_type;
	రకనిర్వచనం RTLIL::Sig_బిట్టు* సూచిక;
	రకనిర్వచనం RTLIL::Sig_బిట్టు& reference;

	RTLIL::SigSpec *sig_p;
	పూర్ణాంకం సూచీ;

	అంతఃవరుస RTLIL::Sig_బిట్టు &క్రియాకారకం*() స్థిరరాశి;
	అంతఃవరుస bool క్రియాకారకం!=(స్థిరరాశి RTLIL::SigSpecIterator &other) స్థిరరాశి { తిరిగి సూచీ != other.సూచీ; }
	అంతఃవరుస bool క్రియాకారకం==(స్థిరరాశి RTLIL::SigSpecIterator &other) స్థిరరాశి { తిరిగి సూచీ == other.సూచీ; }
	అంతఃవరుస శూన్య క్రియాకారకం++() { సూచీ++; }
};

నిర్మాణం RTLIL::SigSpecConstIterator
{
	రకనిర్వచనం std::ఆదానం_iterator_ట్యాగ్ iterator_category;
	రకనిర్వచనం RTLIL::Sig_బిట్టు విలువ_type;
	రకనిర్వచనం ptrdiff_t difference_type;
	రకనిర్వచనం RTLIL::Sig_బిట్టు* సూచిక;
	రకనిర్వచనం RTLIL::Sig_బిట్టు& reference;

	స్థిరరాశి RTLIL::SigSpec *sig_p;
	RTLIL::Sig_బిట్టు బిట్టు;
	పూర్ణాంకం సూచీ;

	అంతఃవరుస స్థిరరాశి RTLIL::Sig_బిట్టు &క్రియాకారకం*();
	అంతఃవరుస bool క్రియాకారకం!=(స్థిరరాశి RTLIL::SigSpecConstIterator &other) స్థిరరాశి { తిరిగి సూచీ != other.సూచీ; }
	అంతఃవరుస bool క్రియాకారకం==(స్థిరరాశి RTLIL::SigSpecIterator &other) స్థిరరాశి { తిరిగి సూచీ == other.సూచీ; }
	అంతఃవరుస శూన్య క్రియాకారకం++() { సూచీ++; }
};

నిర్మాణం RTLIL::SigSpec
{
అంతరంగ:
	మిత్ర class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, వెలికితీయి_2);
	గణన Representation : అక్షరం {
		CHUNK,
		బిట్లు_3,
	};
	// An AtomicHash is either clear or a nonzero integer.
	నిర్మాణం AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(స్థిరరాశి AtomicHash &rhs) : atomic_(rhs.ఎక్కించు()) {}
		AtomicHash &క్రియాకారకం=(స్థిరరాశి AtomicHash &rhs) { store(rhs.ఎక్కించు()); తిరిగి *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> పఠ() స్థిరరాశి {
			Hasher::hash_t విలువ = ఎక్కించు();
			ఒకవేళ (విలువ == 0)
				తిరిగి std::nullopt;
			తిరిగి విలువ;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		శూన్య సమితి(Hasher::hash_t విలువ) స్థిరరాశి {
			log_assert(విలువ != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(విలువ, std::జ్ఞాపకం_క్రమం_relaxed);
			log_assert(old == 0 || old == విలువ);
		}
		శూన్య clear() { store(0); }
	అంతరంగ:
		పూర్ణాంకం ఎక్కించు() స్థిరరాశి { తిరిగి atomic_.ఎక్కించు(std::జ్ఞాపకం_క్రమం_relaxed); }
		శూన్య store(Hasher::hash_t విలువ) స్థిరరాశి {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(విలువ, std::జ్ఞాపకం_క్రమం_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	సంఘం {
		RTLIL::SigChunk chunk_;
		std::సదిశ<RTLIL::Sig_బిట్టు> బిట్లు_2; // LSB at index 0
	};

	శూన్య init_ఖాళీలు_బిట్లు() {
		rep_ = బిట్లు_3;
		new (&బిట్లు_2) std::సదిశ<RTLIL::Sig_బిట్టు>;
	}

	శూన్య unpack();
	అంతఃవరుస శూన్య inline_unpack() {
		ఒకవేళ (rep_ == CHUNK)
			unpack();
	}
	శూన్య try_repack();

	Hasher::hash_t updhash() స్థిరరాశి;
	శూన్య destroy() {
		ఒకవేళ (rep_ == CHUNK)
			chunk_.~SigChunk();
		లేకపోతే
			బిట్లు_2.~సదిశ();
	}
	మిత్ర నిర్మాణం Chunks;

బహిరంగ:
	SigSpec() { init_ఖాళీలు_బిట్లు(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(స్థిరరాశి SigSpec &విలువ) : rep_(విలువ.rep_), hash_(విలువ.hash_) {
		ఒకవేళ (విలువ.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(విలువ.chunk_);
		లేకపోతే
			new (&బిట్లు_2) std::సదిశ<RTLIL::Sig_బిట్టు>(విలువ.బిట్లు_2);
	}
	SigSpec(SigSpec &&విలువ) : rep_(విలువ.rep_), hash_(విలువ.hash_) {
		ఒకవేళ (విలువ.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(విలువ.chunk_));
		లేకపోతే
			new (&బిట్లు_2) std::సదిశ<RTLIL::Sig_బిట్టు>(std::move(విలువ.బిట్లు_2));
	}
	SigSpec(స్థిరరాశి RTLIL::Const &విలువ);
	SigSpec(RTLIL::Const &&విలువ);
	SigSpec(స్థిరరాశి RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం వెడల్పు = 1);
	SigSpec(స్థిరరాశి std::string &str);
	SigSpec(పూర్ణాంకం val, పూర్ణాంకం వెడల్పు = 32);
	SigSpec(RTLIL::State బిట్టు, పూర్ణాంకం వెడల్పు = 1);
	SigSpec(స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు, పూర్ణాంకం వెడల్పు = 1);
	SigSpec(స్థిరరాశి std::సదిశ<RTLIL::SigChunk> &chunks);
	SigSpec(స్థిరరాశి std::సదిశ<RTLIL::Sig_బిట్టు> &బిట్లు);
	SigSpec(స్థిరరాశి pool<RTLIL::Sig_బిట్టు> &బిట్లు);
	SigSpec(స్థిరరాశి std::సమితి<RTLIL::Sig_బిట్టు> &బిట్లు);
	explicit SigSpec(bool బిట్టు);
	~SigSpec() {
		destroy();
	}

	SigSpec &క్రియాకారకం=(స్థిరరాశి SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		ఒకవేళ (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		లేకపోతే
			new (&బిట్లు_2) std::సదిశ<RTLIL::Sig_బిట్టు>(rhs.బిట్లు_2);
		తిరిగి *this;
	}
	SigSpec &క్రియాకారకం=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		ఒకవేళ (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		లేకపోతే
			new (&బిట్లు_2) std::సదిశ<RTLIL::Sig_బిట్టు>(std::move(rhs.బిట్లు_2));
		తిరిగి *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	నిర్మాణం Chunks {
		Chunks(స్థిరరాశి SigSpec &spec) : spec(spec) {}
		నిర్మాణం const_iterator {
			using iterator_category = std::forward_iterator_ట్యాగ్;
			using విలువ_type = స్థిరరాశి SigChunk &;
			using difference_type = std::ptrdiff_t;
			using సూచిక = స్థిరరాశి SigChunk *;
			using reference = స్థిరరాశి SigChunk &;

			స్థిరరాశి SigSpec &spec;
			పూర్ణాంకం బిట్టు_సూచీ;
			SigChunk chunk;

			const_iterator(స్థిరరాశి SigSpec &spec) : spec(spec) {
				బిట్టు_సూచీ = 0;
				ఒకవేళ (spec.rep_ == బిట్లు_3)
					తదుపరి_chunk_బిట్లు();
			}
			గణన చివర_2 { చివర_3 };
			const_iterator(స్థిరరాశి SigSpec &spec, చివర_2) : spec(spec) {
				బిట్టు_సూచీ = spec.కొలత();
			}
			శూన్య తదుపరి_chunk_బిట్లు();

			స్థిరరాశి SigChunk &క్రియాకారకం*() {
				ఒకవేళ (spec.rep_ == CHUNK)
					తిరిగి spec.chunk_;
				తిరిగి chunk;
			};
			స్థిరరాశి SigChunk *క్రియాకారకం->() { తిరిగి &**this; }
			const_iterator &క్రియాకారకం++() {
				బిట్టు_సూచీ += (**this).వెడల్పు;
				ఒకవేళ (spec.rep_ == బిట్లు_3)
					తదుపరి_chunk_బిట్లు();
				తిరిగి *this;
			}
			bool క్రియాకారకం==(స్థిరరాశి const_iterator &rhs) స్థిరరాశి { తిరిగి బిట్టు_సూచీ == rhs.బిట్టు_సూచీ; }
			bool క్రియాకారకం!=(స్థిరరాశి const_iterator &rhs) స్థిరరాశి { తిరిగి !(*this == rhs); }
		};
		const_iterator begin() స్థిరరాశి { తిరిగి const_iterator(spec); }
		const_iterator చివర() స్థిరరాశి {
			const_iterator it(spec, const_iterator::చివర_3);
			తిరిగి it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::సదిశ<RTLIL::SigChunk>::const_తిరగేయి_iterator rbegin() {
			ensure_chunk_సదిశ();
			తిరిగి chunk_సదిశ.rbegin();
		}
		std::సదిశ<RTLIL::SigChunk>::const_తిరగేయి_iterator rend() {
			ensure_chunk_సదిశ();
			తిరిగి chunk_సదిశ.rend();
		}
		పూర్ణాంకం కొలత() {
			ensure_chunk_సదిశ();
			తిరిగి chunk_సదిశ.కొలత();
		}
		పూర్ణాంకం కొలత() స్థిరరాశి {
			తిరిగి std::దూరం(begin(), చివర());
		}
		స్థిరరాశి SigChunk &at(పూర్ణాంకం సూచీ) {
			ensure_chunk_సదిశ();
			తిరిగి chunk_సదిశ.at(సూచీ);
		}
		క్రియాకారకం స్థిరరాశి std::సదిశ<RTLIL::SigChunk>&() {
			ensure_chunk_సదిశ();
			తిరిగి chunk_సదిశ;
		}
	అంతరంగ:
		శూన్య ensure_chunk_సదిశ() {
			ఒకవేళ (spec.కొలత() > 0 && chunk_సదిశ.ఖాళీలు()) {
				కొరకు (స్థిరరాశి RTLIL::SigChunk &c : *this)
					chunk_సదిశ.నెట్టు_వెనుక(c);
			}
		}
		స్థిరరాశి SigSpec &spec;
		std::సదిశ<RTLIL::SigChunk> chunk_సదిశ;
	};
	మిత్ర నిర్మాణం Chunks::const_iterator;

	అంతఃవరుస Chunks chunks() స్థిరరాశి { తిరిగి {*this}; }
	అంతఃవరుస స్థిరరాశి SigSpec &బిట్లు() స్థిరరాశి { తిరిగి *this; }

	అంతఃవరుస పూర్ణాంకం కొలత() స్థిరరాశి { తిరిగి rep_ == CHUNK ? chunk_.వెడల్పు : Get_కొలత(బిట్లు_2); }
	అంతఃవరుస bool ఖాళీలు() స్థిరరాశి { తిరిగి కొలత() == 0; };

	అంతఃవరుస RTLIL::Sig_బిట్టు &క్రియాకారకం[](పూర్ణాంకం సూచీ) { inline_unpack(); hash_.clear(); తిరిగి బిట్లు_2.at(సూచీ); }
	అంతఃవరుస RTLIL::Sig_బిట్టు క్రియాకారకం[](పూర్ణాంకం సూచీ) స్థిరరాశి {
		ఒకవేళ (rep_ == CHUNK) {
			ఒకవేళ (సూచీ < 0 || సూచీ >= chunk_.వెడల్పు)
				throw std::బహిర్గతం_యొక్క_range("SigSpec::operator[]");
			ఒకవేళ (chunk_.wire)
				తిరిగి RTLIL::Sig_బిట్టు(chunk_.wire, chunk_.స్థానభ్రంశం + సూచీ);
			తిరిగి RTLIL::Sig_బిట్టు(chunk_.దత్తాంశం[సూచీ]);
		}
		తిరిగి బిట్లు_2.at(సూచీ);
	}

	అంతఃవరుస RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.సూచీ = 0; తిరిగి it; }
	అంతఃవరుస RTLIL::SigSpecIterator చివర() { RTLIL::SigSpecIterator it; it.sig_p = this; it.సూచీ = కొలత(); తిరిగి it; }

	అంతఃవరుస RTLIL::SigSpecConstIterator begin() స్థిరరాశి { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.సూచీ = 0; తిరిగి it; }
	అంతఃవరుస RTLIL::SigSpecConstIterator చివర() స్థిరరాశి { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.సూచీ = కొలత(); తిరిగి it; }

	శూన్య క్రమీకరించు();
	శూన్య క్రమీకరించు_and_unify();

	శూన్య replace(స్థిరరాశి RTLIL::SigSpec &నమూనా, స్థిరరాశి RTLIL::SigSpec &with);
	శూన్య replace(స్థిరరాశి RTLIL::SigSpec &నమూనా, స్థిరరాశి RTLIL::SigSpec &with, RTLIL::SigSpec *other) స్థిరరాశి;

	శూన్య replace(స్థిరరాశి dict<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> &నియమాలు);
	శూన్య replace(స్థిరరాశి dict<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> &నియమాలు, RTLIL::SigSpec *other) స్థిరరాశి;

	శూన్య replace(స్థిరరాశి std::map<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> &నియమాలు);
	శూన్య replace(స్థిరరాశి std::map<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> &నియమాలు, RTLIL::SigSpec *other) స్థిరరాశి;

	శూన్య replace(పూర్ణాంకం స్థానభ్రంశం, స్థిరరాశి RTLIL::SigSpec &with);

	శూన్య remove(స్థిరరాశి RTLIL::SigSpec &నమూనా);
	శూన్య remove(స్థిరరాశి RTLIL::SigSpec &నమూనా, RTLIL::SigSpec *other) స్థిరరాశి;
	శూన్య remove2(స్థిరరాశి RTLIL::SigSpec &నమూనా, RTLIL::SigSpec *other);

	శూన్య remove(స్థిరరాశి pool<RTLIL::Sig_బిట్టు> &నమూనా);
	శూన్య remove(స్థిరరాశి pool<RTLIL::Sig_బిట్టు> &నమూనా, RTLIL::SigSpec *other) స్థిరరాశి;
	శూన్య remove2(స్థిరరాశి pool<RTLIL::Sig_బిట్టు> &నమూనా, RTLIL::SigSpec *other);
	శూన్య remove2(స్థిరరాశి std::సమితి<RTLIL::Sig_బిట్టు> &నమూనా, RTLIL::SigSpec *other);
	శూన్య remove2(స్థిరరాశి pool<RTLIL::Wire*> &నమూనా, RTLIL::SigSpec *other);

	శూన్య remove(పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం నిడివి = 1);
	శూన్య remove_const();

	RTLIL::SigSpec వెలికితీయి(స్థిరరాశి RTLIL::SigSpec &నమూనా, స్థిరరాశి RTLIL::SigSpec *other = ఖాళీ) స్థిరరాశి;
	RTLIL::SigSpec వెలికితీయి(స్థిరరాశి pool<RTLIL::Sig_బిట్టు> &నమూనా, స్థిరరాశి RTLIL::SigSpec *other = ఖాళీ) స్థిరరాశి;
	RTLIL::SigSpec వెలికితీయి(పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం నిడివి = 1) స్థిరరాశి;
	RTLIL::SigSpec వెలికితీయి_చివర(పూర్ణాంకం స్థానభ్రంశం) స్థిరరాశి { తిరిగి వెలికితీయి(స్థానభ్రంశం, కొలత() - స్థానభ్రంశం); }

	శూన్య rewrite_wires(std::function<శూన్య(RTLIL::Wire*& wire)> rewrite);

	RTLIL::Sig_బిట్టు lsb() స్థిరరాశి { log_assert(కొలత()); తిరిగి (*this)[0]; };
	RTLIL::Sig_బిట్టు msb() స్థిరరాశి { log_assert(కొలత()); తిరిగి (*this)[కొలత() - 1]; };
	RTLIL::Sig_బిట్టు front() స్థిరరాశి { తిరిగి (*this)[0]; }
	RTLIL::Sig_బిట్టు వెనుక() స్థిరరాశి { తిరిగి (*this)[కొలత() - 1]; }

	శూన్య append(స్థిరరాశి RTLIL::SigSpec &సంకేతం);
	అంతఃవరుస శూన్య append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	అంతఃవరుస శూన్య append(స్థిరరాశి RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	అంతఃవరుస శూన్య append(స్థిరరాశి RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	శూన్య append(స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు);
	అంతఃవరుస శూన్య append(RTLIL::State state) { append(RTLIL::Sig_బిట్టు(state)); }
	అంతఃవరుస శూన్య append(bool bool_) { append(RTLIL::Sig_బిట్టు(bool_)); }

	శూన్య extend_u0(పూర్ణాంకం వెడల్పు, bool is_signed = false);

	RTLIL::SigSpec repeat(పూర్ణాంకం అంకె) స్థిరరాశి;

	శూన్య తిరగేయి() { inline_unpack(); std::తిరగేయి(బిట్లు_2.begin(), బిట్లు_2.చివర()); }

	bool క్రియాకారకం <(స్థిరరాశి RTLIL::SigSpec &other) స్థిరరాశి;
	bool క్రియాకారకం ==(స్థిరరాశి RTLIL::SigSpec &other) స్థిరరాశి;
	అంతఃవరుస bool క్రియాకారకం !=(స్థిరరాశి RTLIL::SigSpec &other) స్థిరరాశి { తిరిగి !(*this == other); }

	bool is_wire() స్థిరరాశి;
	bool is_chunk() స్థిరరాశి;
	అంతఃవరుస bool is_బిట్టు() స్థిరరాశి { తిరిగి కొలత() == 1; }

	bool తెలిసిన_driver() స్థిరరాశి;

	bool is_fully_const() స్థిరరాశి;
	bool is_fully_zero() స్థిరరాశి;
	bool is_fully_ones() స్థిరరాశి;
	bool is_fully_def() స్థిరరాశి;
	bool is_fully_undef() స్థిరరాశి;
	bool has_const() స్థిరరాశి;
	bool has_const(State state) స్థిరరాశి;
	bool has_marked_బిట్లు() స్థిరరాశి;
	bool is_onehot(పూర్ణాంకం *pos = nullptr) స్థిరరాశి;

	bool as_bool() స్థిరరాశి;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	పూర్ణాంకం as_int(bool is_signed = false) స్థిరరాశి;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_గమ్యం_int(bool is_signed = false) స్థిరరాశి;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<పూర్ణాంకం> try_as_int(bool is_signed = false) స్థిరరాశి;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	పూర్ణాంకం as_int_saturating(bool is_signed = false) స్థిరరాశి;

	std::string as_string() స్థిరరాశి;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() స్థిరరాశి;
	RTLIL::Const as_const() స్థిరరాశి;
	RTLIL::Wire *as_wire() స్థిరరాశి;
	RTLIL::SigChunk as_chunk() స్థిరరాశి;
	RTLIL::Sig_బిట్టు as_బిట్టు() స్థిరరాశి;

	bool సరిపోలిక(స్థిరరాశి అక్షరం* నమూనా) స్థిరరాశి;

	std::సమితి<RTLIL::Sig_బిట్టు> గమ్యం_sigbit_సమితి() స్థిరరాశి;
	pool<RTLIL::Sig_బిట్టు> గమ్యం_sigbit_pool() స్థిరరాశి;
	std::సదిశ<RTLIL::Sig_బిట్టు> గమ్యం_sigbit_సదిశ() స్థిరరాశి;
	std::map<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> గమ్యం_sigbit_map(స్థిరరాశి RTLIL::SigSpec &other) స్థిరరాశి;
	dict<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> గమ్యం_sigbit_dict(స్థిరరాశి RTLIL::SigSpec &other) స్థిరరాశి;

	స్థిర bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	స్థిర bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	స్థిర bool parse_rhs(స్థిరరాశి RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	క్రియాకారకం std::సదిశ<RTLIL::SigChunk>() స్థిరరాశి;
	క్రియాకారకం std::సదిశ<RTLIL::Sig_బిట్టు>() స్థిరరాశి { తిరిగి గమ్యం_sigbit_సదిశ(); }
	స్థిరరాశి RTLIL::Sig_బిట్టు &at(పూర్ణాంకం స్థానభ్రంశం, స్థిరరాశి RTLIL::Sig_బిట్టు &defval) { తిరిగి స్థానభ్రంశం < కొలత() ? (*this)[స్థానభ్రంశం] : defval; }
	RTLIL::Sig_బిట్టు& at(పూర్ణాంకం స్థానభ్రంశం) { తిరిగి (*this)[స్థానభ్రంశం]; }
	RTLIL::Sig_బిట్టు at(పూర్ణాంకం స్థానభ్రంశం) స్థిరరాశి { తిరిగి (*this)[స్థానభ్రంశం]; }

	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
		Hasher::hash_t val;
		ఒకవేళ (std::optional<Hasher::hash_t> current = hash_.పఠ())
			val = *current;
		లేకపోతే
			val = updhash();
		h.eat(val);
		తిరిగి h;
	}

#నిర్వచితం_కాకపోతే NDEBUG
	శూన్య check(స్థిరరాశి Module *mod = nullptr) స్థిరరాశి;
#లేకపోతే
	శూన్య check(స్థిరరాశి Module *mod = nullptr) స్థిరరాశి { (శూన్య)mod; }
#అయితే_ముగింపు
};

నిర్మాణం RTLIL::Selection
{
	// selection includes boxed modules
	bool selects_boxes;
	// selection covers full design, including boxed modules
	bool complete_selection;
	// selection covers full design, not including boxed modules
	bool నిండిన_selection;
	pool<RTLIL::IdString> selected_modules;
	dict<RTLIL::IdString, pool<RTLIL::IdString>> selected_members;
	RTLIL::Design *current_design;

	// create a new selection
	Selection(
		// should the selection cover the full design
		bool నిండిన_2 = true,
		// should the selection include boxed modules
		bool boxes = false,
		// the design to select from
		RTLIL::Design *design = nullptr
	) :
		selects_boxes(boxes), complete_selection(నిండిన_2 && boxes), నిండిన_selection(నిండిన_2 && !boxes), current_design(design) { }

	// checks if the given module exists in the current design and is a
	// boxed module, warning the user if the current design is not set
	bool boxed_module(RTLIL::IdString mod_name) స్థిరరాశి;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) స్థిరరాశి;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_సంపూర్ణ_module(RTLIL::IdString mod_name) స్థిరరాశి;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) స్థిరరాశి;

	// optimizes this selection for the given design by:
	// - removing non-existent modules and members, any boxed modules and
	//   their members (if selection does not include boxes), and any
	//   partially selected modules with no selected members;
	// - marking partially selected modules as wholly selected if all
	//   members of that module are selected; and
	// - marking selection as a complete_selection if all modules in the
	//   given design are selected, or a full_selection if it does not
	//   include boxes.
	శూన్య optimize(RTLIL::Design *design);

	// checks if selection covers full design (may or may not include
	// boxed-modules)
	bool selects_all() స్థిరరాశి {
		తిరిగి నిండిన_selection || complete_selection;
	}

	// add whole module to this selection
	మూస<typename T1> శూన్య select(T1 *module) {
		ఒకవేళ (!selects_all() && selected_modules.లెక్క(module->name) == 0) {
			IdString name = module->name;
			selected_modules.చొప్పించు(name);
			selected_members.erase(name);
			ఒకవేళ (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	మూస<typename T1, typename T2> శూన్య select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	మూస<typename T1> శూన్య select(T1 *module, RTLIL::IdString member) {
		ఒకవేళ (!selects_all() && selected_modules.లెక్క(module->name.ref()) == 0) {
			selected_members[module->name.ref()].చొప్పించు(member);
			ఒకవేళ (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool ఖాళీలు() స్థిరరాశి {
		తిరిగి !selects_all() && selected_modules.ఖాళీలు() && selected_members.ఖాళీలు();
	}

	// clear this selection, leaving it empty
	శూన్య clear();

	// create a new selection which is empty
	స్థిర Selection ఖాళీలు_Selection(RTLIL::Design *design = nullptr) { తిరిగి Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	స్థిర Selection నిండిన_Selection(RTLIL::Design *design = nullptr) { తిరిగి Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	స్థిర Selection CompleteSelection(RTLIL::Design *design = nullptr) { తిరిగి Selection(true, true, design); };
};

నిర్మాణం RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి { h.eat(hashidx_); తిరిగి h; }

	Monitor() {
		స్థిర గుర్తులేని పూర్ణాంకం hashidx_లెక్క = 123456789;
		hashidx_లెక్క = mkhash_xorshift(hashidx_లెక్క);
		hashidx_ = hashidx_లెక్క;
	}

	మాయా ~Monitor() { }
	మాయా శూన్య notify_module_కూడు(RTLIL::Module*) { }
	మాయా శూన్య notify_module_del(RTLIL::Module*) { }
	మాయా శూన్య notify_connect(RTLIL::Cell*, RTLIL::IdString, స్థిరరాశి RTLIL::SigSpec&, స్థిరరాశి RTLIL::SigSpec&) { }
	మాయా శూన్య notify_connect(RTLIL::Module*, స్థిరరాశి RTLIL::SigSig&) { }
	మాయా శూన్య notify_connect(RTLIL::Module*, స్థిరరాశి std::సదిశ<RTLIL::SigSig>&) { }
	మాయా శూన్య notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
నిర్మాణం define_map_t;

మూస<typename N>
అంతఃవరుస constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::ఆకు> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, స్థిరరాశి అక్షరం*> || std::is_same_v<std::decay_t<N>, అక్షరం*>;

#నిర్వచించు YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, పూర్ణాంకం> = 0

మూస<typename N>
అంతఃవరుస constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, స్థిరరాశి అక్షరం *> || std::is_same_v<std::decay_t<N>, అక్షరం *>;

#నిర్వచించు YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, పూర్ణాంకం> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#నిర్వచించు YS_NAME_FWD_POOL(_func, _pool) \
	మూస<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(స్వయంచాలక) _func(N name, Rest&&... rest) \
		{ తిరిగి _func(_pool.కూడు(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#నిర్వచించు YS_NAME_FWD_2ND_POOL(_func, _pool) \
	మూస<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(స్వయంచాలక) _func(T &&మొదటి, N name) \
		{ తిరిగి _func(std::forward<T>(మొదటి), _pool.కూడు(std::move(name))); }

#నిర్వచించు YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#నిర్వచించు YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

నిర్మాణం RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి { h.eat(hashidx_); తిరిగి h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	శూన్య bufNormalize(bool enable=true);

	పూర్ణాంకం refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

అంతరంగ:
	TwinePool twines_;

బహిరంగ:
	TwinePool &twines() { తిరిగి twines_; }
	స్థిరరాశి TwinePool &twines() స్థిరరాశి { తిరిగి twines_; }

	శూన్య absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	కొలత_t gc_twines();

	std::సదిశ<std::unique_ptr<AST::Ast_కణుపు>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_నిర్వచిస్తుంది;

	std::సదిశ<RTLIL::Selection> selection_దొంతర;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_చురుకైన_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	స్థిరరాశి RTLIL::Module *module(RTLIL::IdString name) స్థిరరాశి;
	మూస<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ తిరిగి module(twines().కనుగొను(std::string_view(name))); }
	మూస<typename N, YS_NAME_STRING(N)> స్థిరరాశి RTLIL::Module *module(N name) స్థిరరాశి
		{ తిరిగి module(twines().కనుగొను(std::string_view(name))); }
	RTLIL::Module *శిఖరం_module() స్థిరరాశి;

	bool has(RTLIL::IdString id) స్థిరరాశి {
		twines_.check_owned(id);
		తిరిగి modules_.లెక్క(id) != 0;
	}
	మూస<typename N, YS_NAME_STRING(N)> bool has(N name) స్థిరరాశి
		{ తిరిగి has(twines().కనుగొను(std::string_view(name))); }

	శూన్య కూడు(RTLIL::Module *module);

	RTLIL::Module *కూడు_Module(RTLIL::IdString name);
	YS_NAME_FWD_POOL(కూడు_Module, twines())
	శూన్య remove(RTLIL::Module *module);
	శూన్య rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	శూన్య scratchpad_unset(స్థిరరాశి std::string &varname);

	శూన్య scratchpad_సమితి_int(స్థిరరాశి std::string &varname, పూర్ణాంకం విలువ);
	శూన్య scratchpad_సమితి_bool(స్థిరరాశి std::string &varname, bool విలువ);
	శూన్య scratchpad_సమితి_string(స్థిరరాశి std::string &varname, std::string విలువ);

	పూర్ణాంకం scratchpad_get_int(స్థిరరాశి std::string &varname, పూర్ణాంకం default_విలువ_2 = 0) స్థిరరాశి;
	bool scratchpad_get_bool(స్థిరరాశి std::string &varname, bool default_విలువ_2 = false) స్థిరరాశి;
	std::string scratchpad_get_string(స్థిరరాశి std::string &varname, స్థిరరాశి std::string &default_విలువ_2 = std::string()) స్థిరరాశి;

	శూన్య క్రమీకరించు();
	శూన్య క్రమీకరించు_modules();
	శూన్య check();
	శూన్య optimize();

	శూన్య clone_into(RTLIL::Design *dst) స్థిరరాశి;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) స్థిరరాశి;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_సంపూర్ణ_module(RTLIL::IdString mod_name) స్థిరరాశి;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) స్థిరరాశి;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) స్థిరరాశి;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_సంపూర్ణ_module(RTLIL::Module *mod) స్థిరరాశి;

	// push the given selection to the selection stack
	శూన్య నెట్టు_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	శూన్య నెట్టు_ఖాళీలు_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	శూన్య నెట్టు_నిండిన_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	శూన్య నెట్టు_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	శూన్య తీయి_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		తిరిగి selection_దొంతర.వెనుక();
	}

	// get the current selection
	స్థిరరాశి RTLIL::Selection &selection() స్థిరరాశి {
		తిరిగి selection_దొంతర.వెనుక();
	}

	// is the current selection a full selection (no boxes)
	bool నిండిన_selection() స్థిరరాశి {
		తిరిగి selection().నిండిన_selection;
	}

	// is the given module in the current selection
	మూస<typename T1> bool selected(T1 *module) స్థిరరాశి {
		తిరిగి selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	మూస<typename T1, typename T2> bool selected(T1 *module, T2 *member) స్థిరరాశి {
		తిరిగి selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	మూస<typename T1> శూన్య select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	మూస<typename T1, typename T2> శూన్య select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::సదిశ<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) స్థిరరాశి;

	// returns all selected modules, and may include boxes
	std::సదిశ<RTLIL::Module*> all_selected_modules() స్థిరరాశి { తిరిగి selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::సదిశ<RTLIL::Module*> selected_unboxed_modules() స్థిరరాశి { తిరిగి selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::సదిశ<RTLIL::Module*> selected_unboxed_modules_warn() స్థిరరాశి { తిరిగి selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::సదిశ<RTLIL::Module*> selected_సంపూర్ణ_modules() స్థిరరాశి { తిరిగి selected_modules(SELECT_సంపూర్ణ_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::సదిశ<RTLIL::Module*> all_selected_సంపూర్ణ_modules() స్థిరరాశి { తిరిగి selected_modules(SELECT_సంపూర్ణ_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::సదిశ<RTLIL::Module*> selected_సంపూర్ణ_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) స్థిరరాశి { తిరిగి selected_modules(SELECT_సంపూర్ణ_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::సదిశ<RTLIL::Module*> selected_unboxed_సంపూర్ణ_modules() స్థిరరాశి { తిరిగి selected_modules(SELECT_సంపూర్ణ_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::సదిశ<RTLIL::Module*> selected_unboxed_సంపూర్ణ_modules_warn() స్థిరరాశి { తిరిగి selected_modules(SELECT_సంపూర్ణ_WARN, SB_UNBOXED_WARN); }

	స్థిర std::map<గుర్తులేని పూర్ణాంకం, RTLIL::Design*> *get_all_designs(శూన్య);

	std::string గమ్యం_rtlil_str(bool only_selected = true) స్థిరరాశి;
};

నిర్మాణం RTLIL::Module : బహిరంగ RTLIL::AttrObject
{
	మిత్ర నిర్మాణం RTLIL::Cell;
	మిత్ర నిర్మాణం RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి { h.eat(hashidx_); తిరిగి h; }

రక్షిత:
	శూన్య కూడు(RTLIL::Wire *wire);
	శూన్య కూడు(RTLIL::Cell *cell);
	శూన్య కూడు(RTLIL::Process *process);

బహిరంగ:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() స్థిరరాశి { తిరిగి design->twines(); }

	పూర్ణాంకం refcount_wires_;
	పూర్ణాంకం refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::సదిశ<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_విలువలు;
	dict<RTLIL::IdString, RTLIL::జ్ఞాపకం*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	మాయా ~Module();
	మాయా RTLIL::IdString derive(RTLIL::Design *design, స్థిరరాశి dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	మాయా RTLIL::IdString derive(RTLIL::Design *design, స్థిరరాశి dict<RTLIL::IdString, RTLIL::Const> &parameters, స్థిరరాశి dict<RTLIL::IdString, RTLIL::Module*> &interfaces, స్థిరరాశి dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	మాయా కొలత_t లెక్క_id(RTLIL::IdString id);
	మాయా శూన్య expand_interfaces(RTLIL::Design *design, స్థిరరాశి dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	మాయా bool reprocess_if_necessary(RTLIL::Design *design);

	మాయా శూన్య క్రమీకరించు();
	మాయా శూన్య check();
	మాయా శూన్య optimize();
	మాయా శూన్య makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) స్థిరరాశి {
		తిరిగి get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	శూన్య connect(స్థిరరాశి RTLIL::SigSig &conn);
	శూన్య connect(స్థిరరాశి RTLIL::SigSpec &lhs, స్థిరరాశి RTLIL::SigSpec &rhs);
	శూన్య new_connections(స్థిరరాశి std::సదిశ<RTLIL::SigSig> &new_conn);
	స్థిరరాశి std::సదిశ<RTLIL::SigSig> &connections() స్థిరరాశి;

	std::సదిశ<RTLIL::IdString> ports;
	శూన్య fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_వరుస;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_పోర్టు_వరుస;
	pool<RTLIL::Wire *> buf_norm_wire_వరుస;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_సూచీ;
	శూన్య bufNormalize();

	మూస<typename T> శూన్య rewrite_sigspecs(T &functor);
	మూస<typename T> శూన్య rewrite_sigspecs2(T &functor);
	శూన్య cloneInto(RTLIL::Module *new_mod) స్థిరరాశి;
	మాయా RTLIL::Module *clone() స్థిరరాశి;
	మాయా RTLIL::Module *clone(RTLIL::Design *dst) స్థిరరాశి;
	మాయా RTLIL::Module *clone(RTLIL::Design *dst, IdString లక్ష్యం_name) స్థిరరాశి;

	bool has_memories() స్థిరరాశి;
	bool has_processes() స్థిరరాశి;

	bool has_memories_warn() స్థిరరాశి;
	bool has_processes_warn() స్థిరరాశి;

	bool is_selected() స్థిరరాశి;
	bool is_selected_సంపూర్ణ() స్థిరరాశి;

	std::సదిశ<RTLIL::Wire*> selected_wires() స్థిరరాశి;
	std::సదిశ<RTLIL::Cell*> selected_cells() స్థిరరాశి;
	std::సదిశ<RTLIL::జ్ఞాపకం*> selected_memories() స్థిరరాశి;
	std::సదిశ<RTLIL::Process*> selected_processes() స్థిరరాశి;
	std::సదిశ<RTLIL::SelectedMember> selected_members() స్థిరరాశి;

	మూస<typename T> bool selected(T *member) స్థిరరాశి {
		తిరిగి design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		ఒకవేళ (design)
			design->twines().check_owned(id);
		స్వయంచాలక it = wires_.కనుగొను(id);
		తిరిగి it == wires_.చివర() ? nullptr : it->రెండవ;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		ఒకవేళ (design)
			design->twines().check_owned(id);
		స్వయంచాలక it = cells_.కనుగొను(id);
		తిరిగి it == cells_.చివర() ? nullptr : it->రెండవ;
	}

	స్థిరరాశి RTLIL::Wire* wire(RTLIL::IdString id) స్థిరరాశి{
		ఒకవేళ (design)
			design->twines().check_owned(id);
		స్వయంచాలక it = wires_.కనుగొను(id);
		తిరిగి it == wires_.చివర() ? nullptr : it->రెండవ;
	}
	స్థిరరాశి RTLIL::Cell* cell(RTLIL::IdString id) స్థిరరాశి {
		ఒకవేళ (design)
			design->twines().check_owned(id);
		స్వయంచాలక it = cells_.కనుగొను(id);
		తిరిగి it == cells_.చివర() ? nullptr : it->రెండవ;
	}

	మూస<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ తిరిగి wire(twines().కనుగొను(std::string_view(name))); }
	మూస<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ తిరిగి cell(twines().కనుగొను(std::string_view(name))); }
	మూస<typename N, YS_NAME_STRING(N)> స్థిరరాశి RTLIL::Wire *wire(N name) స్థిరరాశి
		{ తిరిగి wire(twines().కనుగొను(std::string_view(name))); }
	మూస<typename N, YS_NAME_STRING(N)> స్థిరరాశి RTLIL::Cell *cell(N name) స్థిరరాశి
		{ తిరిగి cell(twines().కనుగొను(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { తిరిగి RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	పూర్ణాంకం wires_కొలత() స్థిరరాశి { తిరిగి wires_.కొలత(); }
	RTLIL::Wire* wire_at(పూర్ణాంకం సూచీ) స్థిరరాశి { తిరిగి wires_.element(సూచీ)->రెండవ; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { తిరిగి RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	పూర్ణాంకం cells_కొలత() స్థిరరాశి { తిరిగి cells_.కొలత(); }
	RTLIL::Cell* cell_at(పూర్ణాంకం సూచీ) స్థిరరాశి { తిరిగి cells_.element(సూచీ)->రెండవ; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	శూన్య remove(స్థిరరాశి pool<RTLIL::Wire*> &wires);
	శూన్య remove(RTLIL::Cell *cell);
	శూన్య remove(RTLIL::జ్ఞాపకం *జ్ఞాపకం_3);
	శూన్య remove(RTLIL::Process *process);

	శూన్య rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	శూన్య rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	శూన్య rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	శూన్య మార్పిడి_పేర్లు(RTLIL::Wire *w1, RTLIL::Wire *w2);
	శూన్య మార్పిడి_పేర్లు(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, పూర్ణాంకం &సూచీ);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *కూడు_Wire(RTLIL::IdString name, పూర్ణాంకం వెడల్పు = 1);
	RTLIL::Wire *కూడు_Wire(RTLIL::IdString name, స్థిరరాశి RTLIL::Wire *other);
	YS_NAME_FWD(కూడు_Wire)

	RTLIL::Cell *కూడు_Cell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *కూడు_Cell(RTLIL::IdString name, స్థిరరాశి RTLIL::Cell *other);
	YS_NAME_FWD(కూడు_Cell)
	మూస<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *కూడు_Cell(RTLIL::IdString name, T type) { తిరిగి కూడు_Cell(name, design->twines().కూడు(std::move(type))); }

	RTLIL::జ్ఞాపకం *కూడు_జ్ఞాపకం(RTLIL::IdString name);
	YS_NAME_FWD(కూడు_జ్ఞాపకం)
	RTLIL::జ్ఞాపకం *కూడు_జ్ఞాపకం(RTLIL::IdString name, స్థిరరాశి RTLIL::జ్ఞాపకం *other);

	RTLIL::Process *కూడు_Process(RTLIL::IdString name);
	YS_NAME_FWD(కూడు_Process)
	RTLIL::Process *కూడు_Process(RTLIL::IdString name, స్థిరరాశి RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* కూడు_Not     (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Pos     (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Buf     (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Barrier (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Neg     (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_And  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Or   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Xor  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Xnor (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_Reduce_And  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Reduce_Or   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Reduce_Xor  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Reduce_Xnor (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Reduce_Bool (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_Shl    (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Shr    (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Sshl   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Sshr   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_జరుపు  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Shiftx (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_Lt  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Le  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Eq  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Ne  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Eqx (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Nex (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Ge  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Gt  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_కూడు (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Sub (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Mul (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	// truncating division
	RTLIL::Cell* కూడు_Div (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	// truncating modulo
	RTLIL::Cell* కూడు_Mod (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Div_Floor (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Mod_Floor (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Pow (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_Fa (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_c, స్థిరరాశి RTLIL::SigSpec &sig_x, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_Logic_Not (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Logic_And (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Logic_Or  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, bool is_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_Mux  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Pmux (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Bmux (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Demux (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_Bweqx  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Bwmux  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_ముక్క  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, RTLIL::Const స్థానభ్రంశం, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Concat (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Lut    (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_y, RTLIL::Const lut, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Tribuf (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Assert (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Assume (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_సజీవ   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Fair   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Cover  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Equiv  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_Sr    (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr, స్థిరరాశి RTLIL::SigSpec &sig_q, bool సమితి_polarity = true, bool clr_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Ff    (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Dff   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_d,   స్థిరరాశి RTLIL::SigSpec &sig_q, bool clk_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Dffe  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en,  స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Dffsr (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool సమితి_polarity = true, bool clr_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Dffsre (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool సమితి_polarity = true, bool clr_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Adff (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_arst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, RTLIL::Const arst_విలువ, bool clk_polarity = true, bool arst_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Adffe (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_arst,  స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, RTLIL::Const arst_విలువ, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Aldff (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_aload, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, స్థిరరాశి RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Aldffe (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_aload,  స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, స్థిరరాశి RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Sdff (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_srst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, RTLIL::Const srst_విలువ, bool clk_polarity = true, bool srst_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Sdffe (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_srst,  స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, RTLIL::Const srst_విలువ, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Sdffce (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_srst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, RTLIL::Const srst_విలువ, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Dlatch (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, bool en_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Adlatch (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_arst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, RTLIL::Const arst_విలువ, bool en_polarity = true, bool arst_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Dlatchsr (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, bool en_polarity = true, bool సమితి_polarity = true, bool clr_polarity = true, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_Buf_Gate    (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Not_Gate    (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_And_Gate    (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Nand_Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Or_Gate     (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Nor_Gate    (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Xor_Gate    (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Xnor_Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Andnot_Gate (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Ornot_Gate  (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Mux_Gate    (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_s, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Nmux_Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_s, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Aoi3_Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_c, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Oai3_Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_c, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Aoi4_Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_c, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_d, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Oai4_Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_c, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_d, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_y, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_Sr_Gate     (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr,
			స్థిరరాశి RTLIL::SigSpec &sig_q, bool సమితి_polarity = true, bool clr_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Ff_Gate     (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Dff_Gate    (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, bool clk_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Dffe_Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Dffsr_Gate  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool సమితి_polarity = true, bool clr_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Dffsre_Gate (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool సమితి_polarity = true, bool clr_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Adff_Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_arst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
			bool arst_విలువ = false, bool clk_polarity = true, bool arst_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Adffe_Gate  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_arst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
			bool arst_విలువ = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Aldff_Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_aload, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
			స్థిరరాశి RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Aldffe_Gate  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_aload, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
			స్థిరరాశి RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Sdff_Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_srst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
			bool srst_విలువ = false, bool clk_polarity = true, bool srst_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Sdffe_Gate  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_srst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
			bool srst_విలువ = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Sdffce_Gate (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_clk, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_srst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
			bool srst_విలువ = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Dlatch_Gate (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, bool en_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Adlatch_Gate(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_arst, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q,
			bool arst_విలువ = false, bool en_polarity = true, bool arst_polarity = true, స్థిరరాశి std::string &src = "");
	RTLIL::Cell* కూడు_Dlatchsr_Gate  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_en, స్థిరరాశి RTLIL::SigSpec &sig_సమితి, స్థిరరాశి RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, bool en_polarity = true, bool సమితి_polarity = true, bool clr_polarity = true, స్థిరరాశి std::string &src = "");

	RTLIL::Cell* కూడు_Anyinit(RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_d, స్థిరరాశి RTLIL::SigSpec &sig_q, స్థిరరాశి std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, bool is_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, bool is_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec జరుపు_2  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::SigSpec కూడు_2 (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, bool is_signed = false, స్థిరరాశి std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_b, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి std::string &src = "");

	RTLIL::Sig_బిట్టు BufGate    (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు NotGate    (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు AndGate    (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు NandGate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు OrGate     (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు NorGate    (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు XorGate    (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు XnorGate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు AndnotGate (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు OrnotGate  (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు MuxGate    (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_s, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు NmuxGate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_s, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు Aoi3Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_c, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు Oai3Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_c, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు Aoi4Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_c, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_d, స్థిరరాశి std::string &src = "");
	RTLIL::Sig_బిట్టు Oai4Gate   (RTLIL::IdString name, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_a, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_b, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_c, స్థిరరాశి RTLIL::Sig_బిట్టు &sig_d, స్థిరరాశి std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, పూర్ణాంకం వెడల్పు = 1, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, పూర్ణాంకం వెడల్పు = 1, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, పూర్ణాంకం వెడల్పు = 1, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, పూర్ణాంకం వెడల్పు = 1, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, స్థిరరాశి std::string &src = "");

	RTLIL::SigSpec సమితి_ట్యాగ్_2          (RTLIL::IdString name, స్థిరరాశి std::string &ట్యాగ్, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి RTLIL::SigSpec &sig_c, స్థిరరాశి std::string &src = "");
	RTLIL::Cell*   కూడు_సమితి_ట్యాగ్       (RTLIL::IdString name, స్థిరరాశి std::string &ట్యాగ్, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి RTLIL::SigSpec &sig_c, స్థిరరాశి RTLIL::SigSpec &sig_y, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Get_ట్యాగ్          (RTLIL::IdString name, స్థిరరాశి std::string &ట్యాగ్, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి std::string &src = "");
	RTLIL::Cell*   కూడు_Overwrite_ట్యాగ్ (RTLIL::IdString name, స్థిరరాశి std::string &ట్యాగ్, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి RTLIL::SigSpec &sig_s, స్థిరరాశి RTLIL::SigSpec &sig_c, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec Original_ట్యాగ్     (RTLIL::IdString name, స్థిరరాశి std::string &ట్యాగ్, స్థిరరాశి RTLIL::SigSpec &sig_a, స్థిరరాశి std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, స్థిరరాశి RTLIL::SigSpec &sig_e, స్థిరరాశి std::string &src = "");
	YS_NAME_FWD(కూడు_Anyinit)
	YS_NAME_FWD(Anyconst) YS_NAME_FWD(Anyseq) YS_NAME_FWD(Allconst) YS_NAME_FWD(Allseq)
	YS_NAME_FWD(Initstate)
	YS_NAME_FWD(సమితి_ట్యాగ్_2) YS_NAME_FWD(కూడు_సమితి_ట్యాగ్) YS_NAME_FWD(Get_ట్యాగ్)
	YS_NAME_FWD(కూడు_Overwrite_ట్యాగ్) YS_NAME_FWD(Original_ట్యాగ్) YS_NAME_FWD(FutureFF)
	YS_NAME_FWD(కూడు_Not) YS_NAME_FWD(కూడు_Pos) YS_NAME_FWD(కూడు_Buf) YS_NAME_FWD(కూడు_Barrier) YS_NAME_FWD(కూడు_Neg)
	YS_NAME_FWD(కూడు_And) YS_NAME_FWD(కూడు_Or) YS_NAME_FWD(కూడు_Xor) YS_NAME_FWD(కూడు_Xnor)
	YS_NAME_FWD(కూడు_Reduce_And) YS_NAME_FWD(కూడు_Reduce_Or) YS_NAME_FWD(కూడు_Reduce_Xor) YS_NAME_FWD(కూడు_Reduce_Xnor)
	YS_NAME_FWD(కూడు_Reduce_Bool) YS_NAME_FWD(కూడు_Shl) YS_NAME_FWD(కూడు_Shr) YS_NAME_FWD(కూడు_Sshl)
	YS_NAME_FWD(కూడు_Sshr) YS_NAME_FWD(కూడు_జరుపు) YS_NAME_FWD(కూడు_Shiftx) YS_NAME_FWD(కూడు_Lt)
	YS_NAME_FWD(కూడు_Le) YS_NAME_FWD(కూడు_Eq) YS_NAME_FWD(కూడు_Ne) YS_NAME_FWD(కూడు_Eqx)
	YS_NAME_FWD(కూడు_Nex) YS_NAME_FWD(కూడు_Ge) YS_NAME_FWD(కూడు_Gt) YS_NAME_FWD(కూడు_కూడు)
	YS_NAME_FWD(కూడు_Sub) YS_NAME_FWD(కూడు_Mul) YS_NAME_FWD(కూడు_Div) YS_NAME_FWD(కూడు_Mod)
	YS_NAME_FWD(కూడు_Div_Floor) YS_NAME_FWD(కూడు_Mod_Floor) YS_NAME_FWD(కూడు_Pow) YS_NAME_FWD(కూడు_Fa)
	YS_NAME_FWD(కూడు_Logic_Not) YS_NAME_FWD(కూడు_Logic_And) YS_NAME_FWD(కూడు_Logic_Or) YS_NAME_FWD(కూడు_Mux)
	YS_NAME_FWD(కూడు_Pmux) YS_NAME_FWD(కూడు_Bmux) YS_NAME_FWD(కూడు_Demux) YS_NAME_FWD(కూడు_Bweqx)
	YS_NAME_FWD(కూడు_Bwmux) YS_NAME_FWD(కూడు_ముక్క) YS_NAME_FWD(కూడు_Concat) YS_NAME_FWD(కూడు_Lut)
	YS_NAME_FWD(కూడు_Tribuf) YS_NAME_FWD(కూడు_Assert) YS_NAME_FWD(కూడు_Assume) YS_NAME_FWD(కూడు_సజీవ)
	YS_NAME_FWD(కూడు_Fair) YS_NAME_FWD(కూడు_Cover) YS_NAME_FWD(కూడు_Equiv) YS_NAME_FWD(కూడు_Sr)
	YS_NAME_FWD(కూడు_Ff) YS_NAME_FWD(కూడు_Dff) YS_NAME_FWD(కూడు_Dffe) YS_NAME_FWD(కూడు_Dffsr)
	YS_NAME_FWD(కూడు_Dffsre) YS_NAME_FWD(కూడు_Adff) YS_NAME_FWD(కూడు_Adffe) YS_NAME_FWD(కూడు_Aldff)
	YS_NAME_FWD(కూడు_Aldffe) YS_NAME_FWD(కూడు_Sdff) YS_NAME_FWD(కూడు_Sdffe) YS_NAME_FWD(కూడు_Sdffce)
	YS_NAME_FWD(కూడు_Dlatch) YS_NAME_FWD(కూడు_Adlatch) YS_NAME_FWD(కూడు_Dlatchsr) YS_NAME_FWD(కూడు_Buf_Gate)
	YS_NAME_FWD(కూడు_Not_Gate) YS_NAME_FWD(కూడు_And_Gate) YS_NAME_FWD(కూడు_Nand_Gate) YS_NAME_FWD(కూడు_Or_Gate)
	YS_NAME_FWD(కూడు_Nor_Gate) YS_NAME_FWD(కూడు_Xor_Gate) YS_NAME_FWD(కూడు_Xnor_Gate) YS_NAME_FWD(కూడు_Andnot_Gate)
	YS_NAME_FWD(కూడు_Ornot_Gate) YS_NAME_FWD(కూడు_Mux_Gate) YS_NAME_FWD(కూడు_Nmux_Gate) YS_NAME_FWD(కూడు_Aoi3_Gate)
	YS_NAME_FWD(కూడు_Oai3_Gate) YS_NAME_FWD(కూడు_Aoi4_Gate) YS_NAME_FWD(కూడు_Oai4_Gate) YS_NAME_FWD(కూడు_Sr_Gate)
	YS_NAME_FWD(కూడు_Ff_Gate) YS_NAME_FWD(కూడు_Dff_Gate) YS_NAME_FWD(కూడు_Dffe_Gate) YS_NAME_FWD(కూడు_Dffsr_Gate)
	YS_NAME_FWD(కూడు_Dffsre_Gate) YS_NAME_FWD(కూడు_Adff_Gate) YS_NAME_FWD(కూడు_Adffe_Gate) YS_NAME_FWD(కూడు_Aldff_Gate)
	YS_NAME_FWD(కూడు_Aldffe_Gate) YS_NAME_FWD(కూడు_Sdff_Gate) YS_NAME_FWD(కూడు_Sdffe_Gate) YS_NAME_FWD(కూడు_Sdffce_Gate)
	YS_NAME_FWD(కూడు_Dlatch_Gate) YS_NAME_FWD(కూడు_Adlatch_Gate) YS_NAME_FWD(కూడు_Dlatchsr_Gate) YS_NAME_FWD(Not)
	YS_NAME_FWD(Pos) YS_NAME_FWD(Buf) YS_NAME_FWD(Barrier) YS_NAME_FWD(Neg) YS_NAME_FWD(And)
	YS_NAME_FWD(Or) YS_NAME_FWD(Xor) YS_NAME_FWD(Xnor) YS_NAME_FWD(ReduceAnd)
	YS_NAME_FWD(ReduceOr) YS_NAME_FWD(ReduceXor) YS_NAME_FWD(ReduceXnor) YS_NAME_FWD(ReduceBool)
	YS_NAME_FWD(Shl) YS_NAME_FWD(Shr) YS_NAME_FWD(Sshl) YS_NAME_FWD(Sshr)
	YS_NAME_FWD(జరుపు_2) YS_NAME_FWD(Shiftx) YS_NAME_FWD(Lt) YS_NAME_FWD(Le)
	YS_NAME_FWD(Eq) YS_NAME_FWD(Ne) YS_NAME_FWD(Eqx) YS_NAME_FWD(Nex)
	YS_NAME_FWD(Ge) YS_NAME_FWD(Gt) YS_NAME_FWD(కూడు_2) YS_NAME_FWD(Sub)
	YS_NAME_FWD(Mul) YS_NAME_FWD(Div) YS_NAME_FWD(Mod) YS_NAME_FWD(DivFloor)
	YS_NAME_FWD(ModFloor) YS_NAME_FWD(Pow) YS_NAME_FWD(LogicNot) YS_NAME_FWD(LogicAnd)
	YS_NAME_FWD(LogicOr) YS_NAME_FWD(Mux) YS_NAME_FWD(Pmux) YS_NAME_FWD(Bmux)
	YS_NAME_FWD(Demux) YS_NAME_FWD(Bweqx) YS_NAME_FWD(Bwmux) YS_NAME_FWD(BufGate)
	YS_NAME_FWD(NotGate) YS_NAME_FWD(AndGate) YS_NAME_FWD(NandGate) YS_NAME_FWD(OrGate)
	YS_NAME_FWD(NorGate) YS_NAME_FWD(XorGate) YS_NAME_FWD(XnorGate) YS_NAME_FWD(AndnotGate)
	YS_NAME_FWD(OrnotGate) YS_NAME_FWD(MuxGate) YS_NAME_FWD(NmuxGate) YS_NAME_FWD(Aoi3Gate)
	YS_NAME_FWD(Oai3Gate) YS_NAME_FWD(Aoi4Gate) YS_NAME_FWD(Oai4Gate)

	std::string గమ్యం_rtlil_str() స్థిరరాశి;
#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	స్థిర std::map<గుర్తులేని పూర్ణాంకం, RTLIL::Module*> *get_all_modules(శూన్య);
#అయితే_ముగింపు
};

నిర్మాణం RTLIL::Wire : బహిరంగ RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి { h.eat(hashidx_); తిరిగి h; }

రక్షిత:
	// use module->addWire() and module->remove() to create or destroy wires
	మిత్ర నిర్మాణం RTLIL::Module;
	Wire();
	~Wire();

	మిత్ర నిర్మాణం RTLIL::Design;
	మిత్ర నిర్మాణం RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driver_పోర్టు_2;

బహిరంగ:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	శూన్య క్రియాకారకం=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	పూర్ణాంకం వెడల్పు, ప్రారంభం_స్థానభ్రంశం, పోర్టు_id;
	bool పోర్టు_ఆదానం, పోర్టు_output, upto, is_signed;

	RTLIL::Design *design() స్థిరరాశి { తిరిగి module ? module->design : nullptr; }
	TwinePool &twines() స్థిరరాశి { తిరిగి module->twines(); }

	bool తెలిసిన_driver() స్థిరరాశి { తిరిగి driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() స్థిరరాశి    { log_assert(driverCell_); తిరిగి driverCell_; };
	RTLIL::IdString driver_పోర్టు() స్థిరరాశి { log_assert(driverCell_); తిరిగి driver_పోర్టు_2; };

	పూర్ణాంకం from_hdl_సూచీ(పూర్ణాంకం hdl_సూచీ) {
		పూర్ణాంకం zero_సూచీ = hdl_సూచీ - ప్రారంభం_స్థానభ్రంశం;
		పూర్ణాంకం rtlil_సూచీ = upto ? వెడల్పు - 1 - zero_సూచీ : zero_సూచీ;
		తిరిగి rtlil_సూచీ >= 0 && rtlil_సూచీ < వెడల్పు ? rtlil_సూచీ : INT_MIN;
	}

	పూర్ణాంకం గమ్యం_hdl_సూచీ(పూర్ణాంకం rtlil_సూచీ) {
		ఒకవేళ (rtlil_సూచీ < 0 || rtlil_సూచీ >= వెడల్పు)
			తిరిగి INT_MIN;
		పూర్ణాంకం zero_సూచీ = upto ? వెడల్పు - 1 - rtlil_సూచీ : rtlil_సూచీ;
		తిరిగి zero_సూచీ + ప్రారంభం_స్థానభ్రంశం;
	}

	std::string గమ్యం_rtlil_str() స్థిరరాశి;

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	స్థిర std::map<గుర్తులేని పూర్ణాంకం, RTLIL::Wire*> *get_all_wires(శూన్య);
#అయితే_ముగింపు
};

అంతఃవరుస పూర్ణాంకం Get_కొలత(RTLIL::Wire *wire) {
	తిరిగి wire->వెడల్పు;
}

నిర్మాణం RTLIL::జ్ఞాపకం : బహిరంగ RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి { h.eat(hashidx_); తిరిగి h; }

	జ్ఞాపకం();
	~జ్ఞాపకం();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() స్థిరరాశి { తిరిగి module ? module->design : nullptr; }
	TwinePool &twines() స్థిరరాశి { తిరిగి module->twines(); }

	RTLIL::జ్ఞాపకం_Name_Id name;

	పూర్ణాంకం వెడల్పు, ప్రారంభం_స్థానభ్రంశం, కొలత;
#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	స్థిర std::map<గుర్తులేని పూర్ణాంకం, RTLIL::జ్ఞాపకం*> *get_all_memorys(శూన్య);
#అయితే_ముగింపు

	std::string గమ్యం_rtlil_str() స్థిరరాశి;
};

నిర్మాణం RTLIL::Cell : బహిరంగ RTLIL::AttrObject
{
అంతరంగ:
	శూన్య init_సూచీ();

	bool bufnorm_handle_సమితి_పోర్టు(IdString portname, RTLIL::SigSpec &సంకేతం, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
బహిరంగ:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి { h.eat(hashidx_); తిరిగి h; }

రక్షిత:
	// use module->addCell() and module->remove() to create or destroy cells
	మిత్ర నిర్మాణం RTLIL::Module;
	Cell();
	~Cell();

బహిరంగ:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	శూన్య క్రియాకారకం=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() స్థిరరాశి { తిరిగి module ? module->design : nullptr; }
	TwinePool &twines() స్థిరరాశి { తిరిగి module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool has_పోర్టు(RTLIL::IdString portname) స్థిరరాశి;
	శూన్య unset_పోర్టు(RTLIL::IdString portname);
	శూన్య సమితి_పోర్టు(RTLIL::IdString portname, RTLIL::SigSpec సంకేతం);
	మూస<typename N, YS_NAME_STRING(N)> శూన్య సమితి_పోర్టు(N portname, RTLIL::SigSpec సంకేతం)
		{ సమితి_పోర్టు(module->twines().కూడు(std::move(portname)), std::move(సంకేతం)); }
	స్థిరరాశి RTLIL::SigSpec &get_పోర్టు(RTLIL::IdString portname) స్థిరరాశి;
	స్థిరరాశి dict<RTLIL::IdString, RTLIL::SigSpec> &connections() స్థిరరాశి;

	bool తెలిసిన() స్థిరరాశి;
	bool ఆదానం(RTLIL::IdString portname) స్థిరరాశి;
	bool output(RTLIL::IdString portname) స్థిరరాశి;
	పోర్టు_Dir పోర్టు_dir(RTLIL::IdString portname) స్థిరరాశి;

	bool hasParam(RTLIL::IdString paramname) స్థిరరాశి;
	శూన్య unsetParam(RTLIL::IdString paramname);
	శూన్య సమితి_Param(RTLIL::IdString paramname, RTLIL::Const విలువ);
	స్థిరరాశి RTLIL::Const &getParam(RTLIL::IdString paramname) స్థిరరాశి;

	మూస<typename N, YS_NAME_STRING(N)> bool hasParam(N name) స్థిరరాశి
		{ తిరిగి hasParam(module->twines().కనుగొను(std::string_view(name))); }
	మూస<typename N, YS_NAME_STRING(N)> శూన్య unsetParam(N name)
		{ unsetParam(module->twines().కనుగొను(std::string_view(name))); }
	మూస<typename N, YS_NAME_STRING(N)> శూన్య సమితి_Param(N name, RTLIL::Const విలువ)
		{ సమితి_Param(module->twines().కూడు(std::move(name)), std::move(విలువ)); }
	మూస<typename N, YS_NAME_STRING(N)> స్థిరరాశి RTLIL::Const &getParam(N name) స్థిరరాశి
		{ తిరిగి getParam(module->twines().కనుగొను(std::string_view(name))); }

	శూన్య క్రమీకరించు();
	శూన్య check();
	శూన్య fixup_parameters(bool సమితి_a_signed = false, bool సమితి_b_signed = false);

	bool has_keep_attr() స్థిరరాశి;

	మూస<typename T> శూన్య rewrite_sigspecs(T &functor);
	మూస<typename T> శూన్య rewrite_sigspecs2(T &functor);

#నిర్వచితమైతే YOSYS_ENABLE_PYTHON
	స్థిర std::map<గుర్తులేని పూర్ణాంకం, RTLIL::Cell*> *get_all_cells(శూన్య);
#అయితే_ముగింపు

	bool has_memid() స్థిరరాశి;
	bool is_mem_cell() స్థిరరాశి;
	bool is_builtin_ff() స్థిరరాశి;

	std::string గమ్యం_rtlil_str() స్థిరరాశి;
};

నిర్మాణం RTLIL::Case_నియమం : బహిరంగ RTLIL::AttrObject
{
	std::సదిశ<RTLIL::SigSpec> compare;
	std::సదిశ<RTLIL::SigSig> actions;
	std::సదిశ<RTLIL::Switch_నియమం*> switches;

	~Case_నియమం();

	bool ఖాళీలు() స్థిరరాశి;

	మూస<typename T> శూన్య rewrite_sigspecs(T &functor);
	మూస<typename T> శూన్య rewrite_sigspecs2(T &functor);
	RTLIL::Case_నియమం *clone() స్థిరరాశి;
};

నిర్మాణం RTLIL::Switch_నియమం : బహిరంగ RTLIL::AttrObject
{
	RTLIL::SigSpec సంకేతం;
	std::సదిశ<RTLIL::Case_నియమం*> cases;

	~Switch_నియమం();

	bool ఖాళీలు() స్థిరరాశి;

	మూస<typename T> శూన్య rewrite_sigspecs(T &functor);
	మూస<typename T> శూన్య rewrite_sigspecs2(T &functor);
	RTLIL::Switch_నియమం *clone() స్థిరరాశి;
};

నిర్మాణం RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec చిరునామా;
	RTLIL::SigSpec దత్తాంశం;
	RTLIL::SigSpec enable;
	RTLIL::Const ప్రాధాన్యత_ముసుగు;
};

నిర్మాణం RTLIL::Sync_నియమం
{
	RTLIL::SyncType type;
	RTLIL::SigSpec సంకేతం;
	std::సదిశ<RTLIL::SigSig> actions;
	std::సదిశ<RTLIL::MemWriteAction> mem_write_actions;

	మూస<typename T> శూన్య rewrite_sigspecs(T &functor);
	మూస<typename T> శూన్య rewrite_sigspecs2(T &functor);
	RTLIL::Sync_నియమం *clone() స్థిరరాశి;
};

నిర్మాణం RTLIL::Process : బహిరంగ RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి { h.eat(hashidx_); తిరిగి h; }

రక్షిత:
	// use module->addProcess() and module->remove() to create or destroy processes
	మిత్ర నిర్మాణం RTLIL::Module;
	Process();
	~Process();

బహిరంగ:
	RTLIL::Module *module;
	RTLIL::Case_నియమం మూలం_case;
	std::సదిశ<RTLIL::Sync_నియమం*> syncs;

	RTLIL::Design *design() స్థిరరాశి { తిరిగి module ? module->design : nullptr; }
	TwinePool &twines() స్థిరరాశి { తిరిగి module->twines(); }

	మూస<typename T> శూన్య rewrite_sigspecs(T &functor);
	మూస<typename T> శూన్య rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() స్థిరరాశి;

	std::string గమ్యం_rtlil_str() స్థిరరాశి;
};

అంతఃవరుస RTLIL::Sig_బిట్టు::Sig_బిట్టు() : wire(ఖాళీ), దత్తాంశం(RTLIL::State::S0) { }
అంతఃవరుస RTLIL::Sig_బిట్టు::Sig_బిట్టు(RTLIL::State బిట్టు) : wire(ఖాళీ), దత్తాంశం(బిట్టు) { }
అంతఃవరుస RTLIL::Sig_బిట్టు::Sig_బిట్టు(bool బిట్టు) : wire(ఖాళీ), దత్తాంశం(బిట్టు ? State::S1 : State::S0) { }
అంతఃవరుస RTLIL::Sig_బిట్టు::Sig_బిట్టు(RTLIL::Wire *wire) : wire(wire), స్థానభ్రంశం(0) { log_assert(wire && wire->వెడల్పు == 1); }
అంతఃవరుస RTLIL::Sig_బిట్టు::Sig_బిట్టు(RTLIL::Wire *wire, పూర్ణాంకం స్థానభ్రంశం) : wire(wire), స్థానభ్రంశం(స్థానభ్రంశం) { log_assert(wire != nullptr); }
అంతఃవరుస RTLIL::Sig_బిట్టు::Sig_బిట్టు(స్థిరరాశి RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.వెడల్పు == 1); ఒకవేళ (wire) స్థానభ్రంశం = chunk.స్థానభ్రంశం; లేకపోతే దత్తాంశం = chunk.దత్తాంశం[0]; }
అంతఃవరుస RTLIL::Sig_బిట్టు::Sig_బిట్టు(స్థిరరాశి RTLIL::SigChunk &chunk, పూర్ణాంకం సూచీ) : wire(chunk.wire) { ఒకవేళ (wire) స్థానభ్రంశం = chunk.స్థానభ్రంశం + సూచీ; లేకపోతే దత్తాంశం = chunk.దత్తాంశం[సూచీ]; }

అంతఃవరుస bool RTLIL::Sig_బిట్టు::క్రియాకారకం<(స్థిరరాశి RTLIL::Sig_బిట్టు &other) స్థిరరాశి {
	ఒకవేళ (wire == other.wire)
		తిరిగి wire ? (స్థానభ్రంశం < other.స్థానభ్రంశం) : (దత్తాంశం < other.దత్తాంశం);
	ఒకవేళ (wire != nullptr && other.wire != nullptr)
		తిరిగి wire->name < other.wire->name;
	తిరిగి (wire != nullptr) < (other.wire != nullptr);
}

అంతఃవరుస bool RTLIL::Sig_బిట్టు::క్రియాకారకం==(స్థిరరాశి RTLIL::Sig_బిట్టు &other) స్థిరరాశి {
	తిరిగి (wire == other.wire) && (wire ? (స్థానభ్రంశం == other.స్థానభ్రంశం) : (దత్తాంశం == other.దత్తాంశం));
}

అంతఃవరుస bool RTLIL::Sig_బిట్టు::క్రియాకారకం!=(స్థిరరాశి RTLIL::Sig_బిట్టు &other) స్థిరరాశి {
	తిరిగి (wire != other.wire) || (wire ? (స్థానభ్రంశం != other.స్థానభ్రంశం) : (దత్తాంశం != other.దత్తాంశం));
}

అంతఃవరుస Hasher RTLIL::Sig_బిట్టు::hash_into(Hasher h) స్థిరరాశి {
	ఒకవేళ (wire) {
		h.eat(స్థానభ్రంశం);
		h.eat(wire->name);
		తిరిగి h;
	}
	h.eat(దత్తాంశం);
	తిరిగి h;
}


అంతఃవరుస Hasher RTLIL::Sig_బిట్టు::hash_శిఖరం() స్థిరరాశి {
	Hasher h;
	ఒకవేళ (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.బిట్లు();
		h.force(hashlib::legacy::djb2_కూడు(n, స్థానభ్రంశం));
		తిరిగి h;
	}
	h.force(దత్తాంశం);
	తిరిగి h;
}

అంతఃవరుస RTLIL::Sig_బిట్టు &RTLIL::SigSpecIterator::క్రియాకారకం*() స్థిరరాశి {
	తిరిగి (*sig_p)[సూచీ];
}

అంతఃవరుస స్థిరరాశి RTLIL::Sig_బిట్టు &RTLIL::SigSpecConstIterator::క్రియాకారకం*() {
	బిట్టు = (*sig_p)[సూచీ];
	తిరిగి బిట్టు;
}

అంతఃవరుస RTLIL::Sig_బిట్టు::Sig_బిట్టు(స్థిరరాశి RTLIL::SigSpec &sig) {
	log_assert(sig.కొలత() == 1);
	స్వయంచాలక it = sig.chunks().begin();
	*this = Sig_బిట్టు(*it);
}

మూస<typename T>
శూన్య RTLIL::Module::rewrite_sigspecs(T &functor)
{
	కొరకు (స్వయంచాలక &it : cells_)
		it.రెండవ->rewrite_sigspecs(functor);
	కొరకు (స్వయంచాలక &it : processes)
		it.రెండవ->rewrite_sigspecs(functor);
	కొరకు (స్వయంచాలక &it : connections_) {
		functor(it.మొదటి);
		functor(it.రెండవ);
	}
}

మూస<typename T>
శూన్య RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	కొరకు (స్వయంచాలక &it : cells_)
		it.రెండవ->rewrite_sigspecs2(functor);
	కొరకు (స్వయంచాలక &it : processes)
		it.రెండవ->rewrite_sigspecs2(functor);
	కొరకు (స్వయంచాలక &it : connections_) {
		functor(it.మొదటి, it.రెండవ);
	}
}

మూస<typename T>
శూన్య RTLIL::Cell::rewrite_sigspecs(T &functor) {
	కొరకు (స్వయంచాలక &it : connections_)
		functor(it.రెండవ);
}

మూస<typename T>
శూన్య RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	కొరకు (స్వయంచాలక &it : connections_)
		functor(it.రెండవ);
}

మూస<typename T>
శూన్య RTLIL::Case_నియమం::rewrite_sigspecs(T &functor) {
	కొరకు (స్వయంచాలక &it : compare)
		functor(it);
	కొరకు (స్వయంచాలక &it : actions) {
		functor(it.మొదటి);
		functor(it.రెండవ);
	}
	కొరకు (స్వయంచాలక it : switches)
		it->rewrite_sigspecs(functor);
}

మూస<typename T>
శూన్య RTLIL::Case_నియమం::rewrite_sigspecs2(T &functor) {
	కొరకు (స్వయంచాలక &it : compare)
		functor(it);
	కొరకు (స్వయంచాలక &it : actions) {
		functor(it.మొదటి, it.రెండవ);
	}
	కొరకు (స్వయంచాలక it : switches)
		it->rewrite_sigspecs2(functor);
}

మూస<typename T>
శూన్య RTLIL::Switch_నియమం::rewrite_sigspecs(T &functor)
{
	functor(సంకేతం);
	కొరకు (స్వయంచాలక it : cases)
		it->rewrite_sigspecs(functor);
}

మూస<typename T>
శూన్య RTLIL::Switch_నియమం::rewrite_sigspecs2(T &functor)
{
	functor(సంకేతం);
	కొరకు (స్వయంచాలక it : cases)
		it->rewrite_sigspecs2(functor);
}

మూస<typename T>
శూన్య RTLIL::Sync_నియమం::rewrite_sigspecs(T &functor)
{
	functor(సంకేతం);
	కొరకు (స్వయంచాలక &it : actions) {
		functor(it.మొదటి);
		functor(it.రెండవ);
	}
	కొరకు (స్వయంచాలక &it : mem_write_actions) {
		functor(it.చిరునామా);
		functor(it.దత్తాంశం);
		functor(it.enable);
	}
}

మూస<typename T>
శూన్య RTLIL::Sync_నియమం::rewrite_sigspecs2(T &functor)
{
	functor(సంకేతం);
	కొరకు (స్వయంచాలక &it : actions) {
		functor(it.మొదటి, it.రెండవ);
	}
	కొరకు (స్వయంచాలక &it : mem_write_actions) {
		functor(it.చిరునామా);
		functor(it.దత్తాంశం);
		functor(it.enable);
	}
}

మూస<typename T>
శూన్య RTLIL::Process::rewrite_sigspecs(T &functor)
{
	మూలం_case.rewrite_sigspecs(functor);
	కొరకు (స్వయంచాలక it : syncs)
		it->rewrite_sigspecs(functor);
}

మూస<typename T>
శూన్య RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	మూలం_case.rewrite_sigspecs2(functor);
	కొరకు (స్వయంచాలక it : syncs)
		it->rewrite_sigspecs2(functor);
}

#అన్తర్భావ "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
