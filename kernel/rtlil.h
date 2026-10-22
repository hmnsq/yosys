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

#جيڪڏهن_وصف_نه_ٿيل RTLIL_H
#وصف_ڪريو RTLIL_H

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/yosys.h"
#ضمن "kernel/twine.h"

#ضمن <deque>
#ضمن <string_view>
#ضمن <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	تعداد State : بنانشان حرف {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	تعداد SyncType : بنانشان حرف {
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
	تعداد ConstFlags : بنانشان حرف {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	تعداد SelectPartials : بنانشان حرف {
		SELECT_ALL = 0,          // include partial modules
		SELECT_سڄو_ONLY = 1,   // ignore partial modules
		SELECT_سڄو_WARN = 2,   // call log_warning on partial module
		SELECT_سڄو_ERR = 3,    // call log_error on partial module
		SELECT_سڄو_CMDERR = 4  // call log_cmd_error on partial module
	};

	تعداد SelectBoxes : بنانشان حرف {
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

	تعداد پورٽ_Dir : بنانشان حرف  {
		PD_UNKNOWN = 0,
		PD_داخلا = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr صحيح ويڪر_حد_بندي = 1 << 30;

	بنية Const;
	بنية AttrObject;
	بنية SelectedMember;
	بنية Selection;
	بنية Monitor;
	بنية Design;
	بنية Module;
	بنية Wire;
	بنية يادگيري;
	بنية Cell;
	بنية SigChunk;
	بنية Sig_ٻٽو_انگ;
	بنية SigSpecIterator;
	بنية SigSpecConstIterator;
	بنية SigSpec;
	بنية Case_قاعدو;
	بنية Switch_قاعدو;
	بنية MemWriteAction;
	بنية Sync_قاعدو;
	بنية Process;

	تعريفنوع std::pair<SigSpec, SigSpec> SigSig;
};

بنية SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_اڳياڙي ID; }
namespace RTLIL { using YOSYS_NAMESPACE_اڳياڙي IdString; }

namespace RTLIL {
	فراغ copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			ثابت dict<IdString, RTLIL::Const> &src,
			ثابت RTLIL::Design *src_design, RTLIL::Design *dst_design);

	خارجي dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	ثابت pool<IdString> &builtin_ff_cell_types();

	ساكن مضمن std::string escape_id(ثابت std::string &str) {
		إذا (str.سائيز() > 0 && str[0] != '\\' && str[0] != '$')
			أرجع "\\" + str;
		أرجع str;
	}

	ساكن مضمن std::string unescape_id(ثابت std::string &str) {
		إذا (str.سائيز() < 2)
			أرجع str;
		إذا (str[0] != '\\')
			أرجع str;
		إذا (str[1] == '$' || str[1] == '\\')
			أرجع str;
		إذا (str[1] >= '0' && str[1] <= '9')
			أرجع str;
		أرجع str.substr(1);
	}

	سانچو <typename T> using ترتيب_ڏيو_by_name_id = IdString::compare_ptr_by_name<T>;

	سانچو <typename T> بنية ترتيب_ڏيو_by_name_str {
		bool عامل()(T *a, T *b) ثابت {
			أرجع a->name.lt_by_name(b->name);
		}
	};

	بنية ترتيب_ڏيو_by_id_str {
		ثابت TwinePool& pool;
		explicit ترتيب_ڏيو_by_id_str(ثابت TwinePool& pool) : pool(pool) {}
		bool عامل()(RTLIL::IdString a, RTLIL::IdString b) ثابت {
			أرجع pool.compare_by_name(a, b) < 0;
		}
	};

	ساكن مضمن std::string encode_filename(ثابت std::string &filename)
	{
		std::stringstream val;
		إذا (!std::any_جو(filename.begin(), filename.پڇاڙي(), [](حرف c) {
			أرجع static_cast<بنانشان حرف>(c) < 33 || static_cast<بنانشان حرف>(c) > 126;
		})) أرجع filename;
		لكل (بنانشان حرف ثابت c : filename) {
			إذا (c < 33 || c > 126)
				val << stringf("$%02x", c);
			وإلا
				val << c;
		}
		أرجع val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_and         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_or          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_xor         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_xnor        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);

	RTLIL::Const const_reduce_and  (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_reduce_or   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_reduce_xor  (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_reduce_xnor (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_reduce_bool (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);

	RTLIL::Const const_logic_not   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_logic_and   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_logic_or    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);

	RTLIL::Const const_shl         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_shr         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_sshl        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_sshr        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_سرڪايو       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_shiftx      (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);

	RTLIL::Const const_lt          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_le          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_eq          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_ne          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_eqx         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_nex         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_ge          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_gt          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);

	RTLIL::Const const_جوڙيو         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_sub         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_mul         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_div         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_divfloor    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_modfloor    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_mod         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_pow         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);

	RTLIL::Const const_pos         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_buf         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);
	RTLIL::Const const_neg         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتيجو_len);

	RTLIL::Const const_mux         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	سانچو<typename T, typename ڪنجي_2 = IdString>
	بنية ObjIterator {
		using iterator_category = std::forward_iterator_ٽيگ;
		using قدر_type = T;
		using difference_type = ptrdiff_t;
		using اشارو = T*;
		using reference = T&;
		typename dict<ڪنجي_2, T>::iterator it;
		dict<ڪنجي_2, T> *list_p;
		صحيح *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, صحيح *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			إذا (list_p->خالي_جايون()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} وإلا {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(ثابت RTLIL::ObjIterator<T, ڪنجي_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			إذا (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &عامل=(ثابت RTLIL::ObjIterator<T, ڪنجي_2> &other) {
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

		مضمن T عامل*() ثابت {
			log_assert(list_p != nullptr);
			أرجع it->ٻيو;
		}

		مضمن bool عامل!=(ثابت RTLIL::ObjIterator<T, ڪنجي_2> &other) ثابت {
			إذا (list_p == nullptr || other.list_p == nullptr)
				أرجع list_p != other.list_p;
			أرجع it != other.it;
		}


		مضمن bool عامل==(ثابت RTLIL::ObjIterator<T, ڪنجي_2> &other) ثابت {
			أرجع !(*this != other);
		}

		مضمن ObjIterator<T, ڪنجي_2>& عامل++() {
			log_assert(list_p != nullptr);
			إذا (++it == list_p->پڇاڙي()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			أرجع *this;
		}

		مضمن ObjIterator<T, ڪنجي_2>& عامل+=(صحيح amt) {
			log_assert(list_p != nullptr);
			it += amt;
			إذا (it == list_p->پڇاڙي()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			أرجع *this;
		}

		مضمن ObjIterator<T, ڪنجي_2> عامل+(صحيح amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, ڪنجي_2> new_obj(*this);
			new_obj.it += amt;
			إذا (new_obj.it == list_p->پڇاڙي()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			أرجع new_obj;
		}

		مضمن ثابت ObjIterator<T, ڪنجي_2> عامل++(صحيح) {
			ObjIterator<T, ڪنجي_2> نتيجو(*this);
			++(*this);
			أرجع نتيجو;
		}
	};

	سانچو<typename T, typename ڪنجي_2 = IdString>
	بنية ObjRange
	{
		dict<ڪنجي_2, T> *list_p;
		صحيح *refcount_p;

		ObjRange(decltype(list_p) list_p, صحيح *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, ڪنجي_2> begin() { أرجع RTLIL::ObjIterator<T, ڪنجي_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, ڪنجي_2> پڇاڙي() { أرجع RTLIL::ObjIterator<T, ڪنجي_2>(); }

		سائيز_t سائيز() ثابت {
			أرجع list_p->سائيز();
		}

		عامل pool<T>() ثابت {
			pool<T> نتيجو;
			لكل (خودڪار &it : *list_p)
				نتيجو.وجهو(it.ٻيو);
			أرجع نتيجو;
		}

		عامل std::ويڪٽر<T>() ثابت {
			std::ويڪٽر<T> نتيجو;
			نتيجو.reserve(list_p->سائيز());
			لكل (خودڪار &it : *list_p)
				نتيجو.ڌڪو_پٺ(it.ٻيو);
			أرجع نتيجو;
		}

		pool<T> منزل_pool() ثابت { أرجع *this; }
		std::ويڪٽر<T> منزل_ويڪٽر() ثابت { أرجع *this; }
	};
};

بنية RTLIL::Const
{
	ننڍو صحيح flags;
خاص:
	دوست class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::ويڪٽر<RTLIL::State>;
	تعداد class backing_ٽيگ: bool { بٽ, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_ٽيگ ٽيگ;
	اتحاد {
		bitvectype بٽ_2;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_بٽ() ثابت { أرجع ٽيگ == backing_ٽيگ::بٽ; }
	bool is_str() ثابت { أرجع ٽيگ == backing_ٽيگ::string; }

	bitvectype* get_if_بٽ() { أرجع is_بٽ() ? &بٽ_2 : عدم; }
	std::string* get_if_str() { أرجع is_str() ? &str_ : عدم; }
	ثابت bitvectype* get_if_بٽ() ثابت { أرجع is_بٽ() ? &بٽ_2 : عدم; }
	ثابت std::string* get_if_str() ثابت { أرجع is_str() ? &str_ : عدم; }

	bitvectype& get_بٽ();
	std::string& get_str();
	ثابت bitvectype& get_بٽ() ثابت;
	ثابت std::string& get_str() ثابت;
	std::ويڪٽر<RTLIL::State>& بٽ_اندروني();
	فراغ bitvectorize_اندروني();

عام:
	Const() : flags(RTLIL::CONST_FLAG_NONE), ٽيگ(backing_ٽيگ::بٽ), بٽ_2(std::ويڪٽر<RTLIL::State>()) {}
	Const(std::string str);
	Const(ڊگهو ڊگهو val); // default width is 32
	Const(ڊگهو ڊگهو val, صحيح ويڪر);
	Const(RTLIL::State ٻٽو_انگ, صحيح ويڪر = 1);
	Const(std::ويڪٽر<RTLIL::State> بٽ) : flags(RTLIL::CONST_FLAG_NONE), ٽيگ(backing_ٽيگ::بٽ), بٽ_2(std::move(بٽ)) {}
	Const(ثابت std::ويڪٽر<bool> &بٽ);
	Const(ثابت RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &عامل =(ثابت RTLIL::Const &other);
	~Const();

	بنية Builder
	{
		Builder() {}
		Builder(صحيح expected_ويڪر) { بٽ.reserve(expected_ويڪر); }
		فراغ ڌڪو_پٺ(RTLIL::State b) { بٽ.ڌڪو_پٺ(b); }
		صحيح سائيز() ثابت { أرجع static_cast<صحيح>(بٽ.سائيز()); }
		Const build() { أرجع Const(std::move(بٽ)); }
	خاص:
		std::ويڪٽر<RTLIL::State> بٽ;
	};

	bool عامل <(ثابت RTLIL::Const &other) ثابت;
	bool عامل ==(ثابت RTLIL::Const &other) ثابت;
	bool عامل !=(ثابت RTLIL::Const &other) ثابت;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::ويڪٽر<RTLIL::State>& بٽ() { أرجع بٽ_اندروني(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	فراغ bitvectorize() ثابت { const_cast<Const*>(this)->bitvectorize_اندروني(); }

	bool as_bool() ثابت;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	صحيح as_int(bool is_signed = false) ثابت;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_منزل_int(bool is_signed = false) ثابت;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<صحيح> try_as_int(bool is_signed = false) ثابت;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	صحيح as_int_saturating(bool is_signed = false) ثابت;

	فراغ ٽيگ_bare_integer_const(ثابت std::string &قدر);

	std::string as_string(ثابت حرف* any = "-") ثابت;
	ساكن Const from_string(ثابت std::string &str);
	std::ويڪٽر<RTLIL::State> منزل_بٽ() ثابت;

	std::string decode_string() ثابت;
	صحيح سائيز() ثابت;
	bool خالي_جايون() ثابت;

	فراغ append(ثابت RTLIL::Const &other);
	فراغ سيٽ(صحيح i, RTLIL::State state) {
		بٽ_اندروني()[i] = state;
	}
	فراغ resize(صحيح سائيز, RTLIL::State fill) {
    log_assert(سائيز >= 0 && سائيز < RTLIL::ويڪر_حد_بندي);
		بٽ_اندروني().resize(سائيز, fill);
	}

	class const_iterator {
	خاص:
		ثابت Const* والد;
		سائيز_t idx;

	عام:
		using iterator_category = std::bidirectional_iterator_ٽيگ;
		using قدر_type = State;
		using difference_type = std::ptrdiff_t;
		using اشارو = ثابت State*;
		using reference = ثابت State&;

		const_iterator(ثابت Const& c, سائيز_t i) : والد(&c), idx(i) {}

		State عامل*() ثابت;

		const_iterator& عامل++() { ++idx; أرجع *this; }
		const_iterator& عامل--() { --idx; أرجع *this; }
		const_iterator عامل++(صحيح) { const_iterator نتيجو(*this); ++idx; أرجع نتيجو; }
		const_iterator عامل--(صحيح) { const_iterator نتيجو(*this); --idx; أرجع نتيجو; }
		const_iterator& عامل+=(صحيح i) { idx += i; أرجع *this; }

		const_iterator عامل+(صحيح جوڙيو) {
			أرجع const_iterator(*والد, idx + جوڙيو);
		}
		const_iterator عامل-(صحيح sub) {
			أرجع const_iterator(*والد, idx - sub);
		}
		صحيح عامل-(ثابت const_iterator& other) {
			أرجع idx - other.idx;
		}

		bool عامل==(ثابت const_iterator& other) ثابت {
			أرجع idx == other.idx;
		}

		bool عامل!=(ثابت const_iterator& other) ثابت {
			أرجع !(*this == other);
		}
	};

	class iterator {
	خاص:
		Const* والد;
		سائيز_t idx;

	عام:
		class proxy {
		خاص:
			Const* والد;
			سائيز_t idx;
		عام:
			proxy(Const* والد, سائيز_t idx) : والد(والد), idx(idx) {}
			عامل State() ثابت { أرجع (*والد)[idx]; }
			proxy& عامل=(State s) { والد->سيٽ(idx, s); أرجع *this; }
			proxy& عامل=(ثابت proxy& other) { والد->سيٽ(idx, (*other.والد)[other.idx]); أرجع *this; }
		};

		using iterator_category = std::bidirectional_iterator_ٽيگ;
		using قدر_type = State;
		using difference_type = std::ptrdiff_t;
		using اشارو = proxy*;
		using reference = proxy;

		iterator(Const& c, سائيز_t i) : والد(&c), idx(i) {}

		proxy عامل*() ثابت { أرجع proxy(والد, idx); }
		iterator& عامل++() { ++idx; أرجع *this; }
		iterator& عامل--() { --idx; أرجع *this; }
		iterator عامل++(صحيح) { iterator نتيجو(*this); ++idx; أرجع نتيجو; }
		iterator عامل--(صحيح) { iterator نتيجو(*this); --idx; أرجع نتيجو; }
		iterator& عامل+=(صحيح i) { idx += i; أرجع *this; }

		iterator عامل+(صحيح جوڙيو) {
			أرجع iterator(*والد, idx + جوڙيو);
		}
		iterator عامل-(صحيح sub) {
			أرجع iterator(*والد, idx - sub);
		}
		صحيح عامل-(ثابت iterator& other) {
			أرجع idx - other.idx;
		}

		bool عامل==(ثابت iterator& other) ثابت {
			أرجع idx == other.idx;
		}

		bool عامل!=(ثابت iterator& other) ثابت {
			أرجع !(*this == other);
		}
	};

	const_iterator begin() ثابت {
		أرجع const_iterator(*this, 0);
	}
	const_iterator پڇاڙي() ثابت {
		أرجع const_iterator(*this, سائيز());
	}
	iterator begin() {
		أرجع iterator(*this, 0);
	}
	iterator پڇاڙي() {
		أرجع iterator(*this, سائيز());
	}
	State پٺ() ثابت {
		أرجع *(پڇاڙي() - 1);
	}
	State front() ثابت {
		أرجع *begin();
	}
	State at(سائيز_t i) ثابت {
		أرجع *const_iterator(*this, i);
	}
	State عامل[](سائيز_t i) ثابت {
		أرجع *const_iterator(*this, i);
	}

	bool is_fully_zero() ثابت;
	bool is_fully_ones() ثابت;
	bool is_fully_def() ثابت;
	bool is_fully_undef() ثابت;
	bool is_fully_undef_x_only() ثابت;
	bool is_onehot(صحيح *pos = nullptr) ثابت;

	RTLIL::Const ڪڍو_2(صحيح وٿي, صحيح len = 1, RTLIL::State padding = RTLIL::State::S0) ثابت;

	// find the MSB without redundant leading bits
	صحيح get_min_سائيز(bool is_signed) ثابت;

	// compress representation to the minimum required bits
	فراغ compress(bool is_signed = false);

	std::optional<صحيح> as_int_compress(bool is_signed) ثابت;

	فراغ extu(صحيح ويڪر) {
		resize(ويڪر, RTLIL::State::S0);
	}

	فراغ exts(صحيح ويڪر) {
		resize(ويڪر, خالي_جايون() ? RTLIL::State::Sx : پٺ());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> خاصيتون;

	bool has_attribute(RTLIL::IdString id) ثابت;

	فراغ سيٽ_bool_attribute(RTLIL::IdString id, bool قدر=true);
	bool get_bool_attribute(RTLIL::IdString id) ثابت;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ثابت {
		أرجع get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	فراغ سيٽ_string_attribute(RTLIL::IdString id, string قدر);
	string get_string_attribute(RTLIL::IdString id) ثابت;

	فراغ سيٽ_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &معلومات);
	فراغ جوڙيو_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &معلومات);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ثابت;

	فراغ سيٽ_src_attribute(ثابت std::string &src) {
		سيٽ_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ثابت {
		أرجع get_string_attribute(ID::src);
	}

	فراغ سيٽ_hdlname_attribute(ثابت ويڪٽر<string> &hierarchy);
	ويڪٽر<string> get_hdlname_attribute() ثابت;

	فراغ سيٽ_intvec_attribute(RTLIL::IdString id, ثابت ويڪٽر<صحيح> &معلومات);
	ويڪٽر<صحيح> get_intvec_attribute(RTLIL::IdString id) ثابت;
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
	std::ويڪٽر<RTLIL::State> معلومات; // only used if wire == NULL, LSB at index 0
	صحيح ويڪر, وٿي;

	SigChunk() : wire(nullptr), ويڪر(0), وٿي(0) {}
	SigChunk(ثابت RTLIL::Const &قدر) : wire(nullptr), معلومات(قدر.منزل_بٽ()), ويڪر(Get_سائيز(معلومات)), وٿي(0) {}
	SigChunk(RTLIL::Const &&قدر) : wire(nullptr), معلومات(قدر.منزل_بٽ()), ويڪر(Get_سائيز(معلومات)), وٿي(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), ويڪر(Get_سائيز(wire)), وٿي(0) {}
	SigChunk(RTLIL::Wire *wire, صحيح وٿي, صحيح ويڪر = 1) : wire(wire), ويڪر(ويڪر), وٿي(وٿي) {}
	SigChunk(ثابت std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(صحيح val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(صحيح val, صحيح ويڪر) : SigChunk(RTLIL::Const(val, ويڪر)) {}
	SigChunk(RTLIL::State ٻٽو_انگ, صحيح ويڪر = 1) : SigChunk(RTLIL::Const(ٻٽو_انگ, ويڪر)) {}
	SigChunk(ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ);

	RTLIL::SigChunk ڪڍو_2(صحيح وٿي, صحيح ڊيگهائي) ثابت;
	RTLIL::Sig_ٻٽو_انگ عامل[](صحيح وٿي) ثابت;
	مضمن صحيح سائيز() ثابت { أرجع ويڪر; }
	مضمن bool is_wire() ثابت { أرجع wire != عدم; }

	bool عامل <(ثابت RTLIL::SigChunk &other) ثابت;
	bool عامل ==(ثابت RTLIL::SigChunk &other) ثابت;
	bool عامل !=(ثابت RTLIL::SigChunk &other) ثابت;
};

بنية RTLIL::Sig_ٻٽو_انگ
{
	RTLIL::Wire *wire;
	اتحاد {
		RTLIL::State معلومات; // used if wire == NULL
		صحيح وٿي;        // used if wire != NULL
	};

	Sig_ٻٽو_انگ();
	Sig_ٻٽو_انگ(RTLIL::State ٻٽو_انگ);
	explicit Sig_ٻٽو_انگ(bool ٻٽو_انگ);
	Sig_ٻٽو_انگ(RTLIL::Wire *wire);
	Sig_ٻٽو_انگ(RTLIL::Wire *wire, صحيح وٿي);
	Sig_ٻٽو_انگ(ثابت RTLIL::SigChunk &chunk);
	Sig_ٻٽو_انگ(ثابت RTLIL::SigChunk &chunk, صحيح اشاريو);
	Sig_ٻٽو_انگ(ثابت RTLIL::SigSpec &sig);
	Sig_ٻٽو_انگ(ثابت RTLIL::Sig_ٻٽو_انگ &sigbit) = افتراضي;
	RTLIL::Sig_ٻٽو_انگ &عامل =(ثابت RTLIL::Sig_ٻٽو_انگ &other) = افتراضي;

	مضمن bool is_wire() ثابت { أرجع wire != عدم; }

	bool عامل <(ثابت RTLIL::Sig_ٻٽو_انگ &other) ثابت;
	bool عامل ==(ثابت RTLIL::Sig_ٻٽو_انگ &other) ثابت;
	bool عامل !=(ثابت RTLIL::Sig_ٻٽو_انگ &other) ثابت;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
	[[nodiscard]] Hasher hash_چوٽي() ثابت;
};

namespace hashlib {
	سانچو <>
	بنية hash_ops<RTLIL::Sig_ٻٽو_انگ> {
		ساكن مضمن bool cmp(ثابت RTLIL::Sig_ٻٽو_انگ &a, ثابت RTLIL::Sig_ٻٽو_انگ &b) {
			أرجع a == b;
		}
		[[nodiscard]] ساكن مضمن Hasher hash(ثابت RTLIL::Sig_ٻٽو_انگ sb) {
			أرجع sb.hash_چوٽي();
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت RTLIL::Sig_ٻٽو_انگ sb, Hasher h) {
			أرجع sb.hash_into(h);
		}
	};
};

بنية RTLIL::SigSpecIterator
{
	تعريفنوع std::داخلا_iterator_ٽيگ iterator_category;
	تعريفنوع RTLIL::Sig_ٻٽو_انگ قدر_type;
	تعريفنوع ptrdiff_t difference_type;
	تعريفنوع RTLIL::Sig_ٻٽو_انگ* اشارو;
	تعريفنوع RTLIL::Sig_ٻٽو_انگ& reference;

	RTLIL::SigSpec *sig_p;
	صحيح اشاريو;

	مضمن RTLIL::Sig_ٻٽو_انگ &عامل*() ثابت;
	مضمن bool عامل!=(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع اشاريو != other.اشاريو; }
	مضمن bool عامل==(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع اشاريو == other.اشاريو; }
	مضمن فراغ عامل++() { اشاريو++; }
};

بنية RTLIL::SigSpecConstIterator
{
	تعريفنوع std::داخلا_iterator_ٽيگ iterator_category;
	تعريفنوع RTLIL::Sig_ٻٽو_انگ قدر_type;
	تعريفنوع ptrdiff_t difference_type;
	تعريفنوع RTLIL::Sig_ٻٽو_انگ* اشارو;
	تعريفنوع RTLIL::Sig_ٻٽو_انگ& reference;

	ثابت RTLIL::SigSpec *sig_p;
	RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ;
	صحيح اشاريو;

	مضمن ثابت RTLIL::Sig_ٻٽو_انگ &عامل*();
	مضمن bool عامل!=(ثابت RTLIL::SigSpecConstIterator &other) ثابت { أرجع اشاريو != other.اشاريو; }
	مضمن bool عامل==(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع اشاريو == other.اشاريو; }
	مضمن فراغ عامل++() { اشاريو++; }
};

بنية RTLIL::SigSpec
{
خاص:
	دوست class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, ڪڍو_3);
	تعداد Representation : حرف {
		CHUNK,
		بٽ_3,
	};
	// An AtomicHash is either clear or a nonzero integer.
	بنية AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ثابت AtomicHash &rhs) : atomic_(rhs.ڀريو()) {}
		AtomicHash &عامل=(ثابت AtomicHash &rhs) { store(rhs.ڀريو()); أرجع *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> اقرأ() ثابت {
			Hasher::hash_t قدر = ڀريو();
			إذا (قدر == 0)
				أرجع std::nullopt;
			أرجع قدر;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		فراغ سيٽ(Hasher::hash_t قدر) ثابت {
			log_assert(قدر != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(قدر, std::يادگيري_ترتيب_relaxed);
			log_assert(old == 0 || old == قدر);
		}
		فراغ clear() { store(0); }
	خاص:
		صحيح ڀريو() ثابت { أرجع atomic_.ڀريو(std::يادگيري_ترتيب_relaxed); }
		فراغ store(Hasher::hash_t قدر) ثابت {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(قدر, std::يادگيري_ترتيب_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	اتحاد {
		RTLIL::SigChunk chunk_;
		std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> بٽ_2; // LSB at index 0
	};

	فراغ init_خالي_جايون_بٽ() {
		rep_ = بٽ_3;
		new (&بٽ_2) std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ>;
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
			بٽ_2.~ويڪٽر();
	}
	دوست بنية Chunks;

عام:
	SigSpec() { init_خالي_جايون_بٽ(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ثابت SigSpec &قدر) : rep_(قدر.rep_), hash_(قدر.hash_) {
		إذا (قدر.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(قدر.chunk_);
		وإلا
			new (&بٽ_2) std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ>(قدر.بٽ_2);
	}
	SigSpec(SigSpec &&قدر) : rep_(قدر.rep_), hash_(قدر.hash_) {
		إذا (قدر.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(قدر.chunk_));
		وإلا
			new (&بٽ_2) std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ>(std::move(قدر.بٽ_2));
	}
	SigSpec(ثابت RTLIL::Const &قدر);
	SigSpec(RTLIL::Const &&قدر);
	SigSpec(ثابت RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, صحيح وٿي, صحيح ويڪر = 1);
	SigSpec(ثابت std::string &str);
	SigSpec(صحيح val, صحيح ويڪر = 32);
	SigSpec(RTLIL::State ٻٽو_انگ, صحيح ويڪر = 1);
	SigSpec(ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ, صحيح ويڪر = 1);
	SigSpec(ثابت std::ويڪٽر<RTLIL::SigChunk> &chunks);
	SigSpec(ثابت std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> &بٽ);
	SigSpec(ثابت pool<RTLIL::Sig_ٻٽو_انگ> &بٽ);
	SigSpec(ثابت std::سيٽ<RTLIL::Sig_ٻٽو_انگ> &بٽ);
	explicit SigSpec(bool ٻٽو_انگ);
	~SigSpec() {
		destroy();
	}

	SigSpec &عامل=(ثابت SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		إذا (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		وإلا
			new (&بٽ_2) std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ>(rhs.بٽ_2);
		أرجع *this;
	}
	SigSpec &عامل=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		إذا (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		وإلا
			new (&بٽ_2) std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ>(std::move(rhs.بٽ_2));
		أرجع *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	بنية Chunks {
		Chunks(ثابت SigSpec &spec) : spec(spec) {}
		بنية const_iterator {
			using iterator_category = std::forward_iterator_ٽيگ;
			using قدر_type = ثابت SigChunk &;
			using difference_type = std::ptrdiff_t;
			using اشارو = ثابت SigChunk *;
			using reference = ثابت SigChunk &;

			ثابت SigSpec &spec;
			صحيح ٻٽو_انگ_اشاريو;
			SigChunk chunk;

			const_iterator(ثابت SigSpec &spec) : spec(spec) {
				ٻٽو_انگ_اشاريو = 0;
				إذا (spec.rep_ == بٽ_3)
					ايندڙ_chunk_بٽ();
			}
			تعداد پڇاڙي_2 { پڇاڙي_3 };
			const_iterator(ثابت SigSpec &spec, پڇاڙي_2) : spec(spec) {
				ٻٽو_انگ_اشاريو = spec.سائيز();
			}
			فراغ ايندڙ_chunk_بٽ();

			ثابت SigChunk &عامل*() {
				إذا (spec.rep_ == CHUNK)
					أرجع spec.chunk_;
				أرجع chunk;
			};
			ثابت SigChunk *عامل->() { أرجع &**this; }
			const_iterator &عامل++() {
				ٻٽو_انگ_اشاريو += (**this).ويڪر;
				إذا (spec.rep_ == بٽ_3)
					ايندڙ_chunk_بٽ();
				أرجع *this;
			}
			bool عامل==(ثابت const_iterator &rhs) ثابت { أرجع ٻٽو_انگ_اشاريو == rhs.ٻٽو_انگ_اشاريو; }
			bool عامل!=(ثابت const_iterator &rhs) ثابت { أرجع !(*this == rhs); }
		};
		const_iterator begin() ثابت { أرجع const_iterator(spec); }
		const_iterator پڇاڙي() ثابت {
			const_iterator it(spec, const_iterator::پڇاڙي_3);
			أرجع it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::ويڪٽر<RTLIL::SigChunk>::const_ابتو_iterator rbegin() {
			ensure_chunk_ويڪٽر();
			أرجع chunk_ويڪٽر.rbegin();
		}
		std::ويڪٽر<RTLIL::SigChunk>::const_ابتو_iterator rend() {
			ensure_chunk_ويڪٽر();
			أرجع chunk_ويڪٽر.rend();
		}
		صحيح سائيز() {
			ensure_chunk_ويڪٽر();
			أرجع chunk_ويڪٽر.سائيز();
		}
		صحيح سائيز() ثابت {
			أرجع std::مفاصلو(begin(), پڇاڙي());
		}
		ثابت SigChunk &at(صحيح اشاريو) {
			ensure_chunk_ويڪٽر();
			أرجع chunk_ويڪٽر.at(اشاريو);
		}
		عامل ثابت std::ويڪٽر<RTLIL::SigChunk>&() {
			ensure_chunk_ويڪٽر();
			أرجع chunk_ويڪٽر;
		}
	خاص:
		فراغ ensure_chunk_ويڪٽر() {
			إذا (spec.سائيز() > 0 && chunk_ويڪٽر.خالي_جايون()) {
				لكل (ثابت RTLIL::SigChunk &c : *this)
					chunk_ويڪٽر.ڌڪو_پٺ(c);
			}
		}
		ثابت SigSpec &spec;
		std::ويڪٽر<RTLIL::SigChunk> chunk_ويڪٽر;
	};
	دوست بنية Chunks::const_iterator;

	مضمن Chunks chunks() ثابت { أرجع {*this}; }
	مضمن ثابت SigSpec &بٽ() ثابت { أرجع *this; }

	مضمن صحيح سائيز() ثابت { أرجع rep_ == CHUNK ? chunk_.ويڪر : Get_سائيز(بٽ_2); }
	مضمن bool خالي_جايون() ثابت { أرجع سائيز() == 0; };

	مضمن RTLIL::Sig_ٻٽو_انگ &عامل[](صحيح اشاريو) { inline_unpack(); hash_.clear(); أرجع بٽ_2.at(اشاريو); }
	مضمن RTLIL::Sig_ٻٽو_انگ عامل[](صحيح اشاريو) ثابت {
		إذا (rep_ == CHUNK) {
			إذا (اشاريو < 0 || اشاريو >= chunk_.ويڪر)
				throw std::ٻاهريون_جو_range("SigSpec::operator[]");
			إذا (chunk_.wire)
				أرجع RTLIL::Sig_ٻٽو_انگ(chunk_.wire, chunk_.وٿي + اشاريو);
			أرجع RTLIL::Sig_ٻٽو_انگ(chunk_.معلومات[اشاريو]);
		}
		أرجع بٽ_2.at(اشاريو);
	}

	مضمن RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.اشاريو = 0; أرجع it; }
	مضمن RTLIL::SigSpecIterator پڇاڙي() { RTLIL::SigSpecIterator it; it.sig_p = this; it.اشاريو = سائيز(); أرجع it; }

	مضمن RTLIL::SigSpecConstIterator begin() ثابت { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.اشاريو = 0; أرجع it; }
	مضمن RTLIL::SigSpecConstIterator پڇاڙي() ثابت { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.اشاريو = سائيز(); أرجع it; }

	فراغ ترتيب_ڏيو();
	فراغ ترتيب_ڏيو_and_unify();

	فراغ replace(ثابت RTLIL::SigSpec &نمونو, ثابت RTLIL::SigSpec &with);
	فراغ replace(ثابت RTLIL::SigSpec &نمونو, ثابت RTLIL::SigSpec &with, RTLIL::SigSpec *other) ثابت;

	فراغ replace(ثابت dict<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> &قاعدا);
	فراغ replace(ثابت dict<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> &قاعدا, RTLIL::SigSpec *other) ثابت;

	فراغ replace(ثابت std::map<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> &قاعدا);
	فراغ replace(ثابت std::map<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> &قاعدا, RTLIL::SigSpec *other) ثابت;

	فراغ replace(صحيح وٿي, ثابت RTLIL::SigSpec &with);

	فراغ remove(ثابت RTLIL::SigSpec &نمونو);
	فراغ remove(ثابت RTLIL::SigSpec &نمونو, RTLIL::SigSpec *other) ثابت;
	فراغ remove2(ثابت RTLIL::SigSpec &نمونو, RTLIL::SigSpec *other);

	فراغ remove(ثابت pool<RTLIL::Sig_ٻٽو_انگ> &نمونو);
	فراغ remove(ثابت pool<RTLIL::Sig_ٻٽو_انگ> &نمونو, RTLIL::SigSpec *other) ثابت;
	فراغ remove2(ثابت pool<RTLIL::Sig_ٻٽو_انگ> &نمونو, RTLIL::SigSpec *other);
	فراغ remove2(ثابت std::سيٽ<RTLIL::Sig_ٻٽو_انگ> &نمونو, RTLIL::SigSpec *other);
	فراغ remove2(ثابت pool<RTLIL::Wire*> &نمونو, RTLIL::SigSpec *other);

	فراغ remove(صحيح وٿي, صحيح ڊيگهائي = 1);
	فراغ remove_const();

	RTLIL::SigSpec ڪڍو_2(ثابت RTLIL::SigSpec &نمونو, ثابت RTLIL::SigSpec *other = عدم) ثابت;
	RTLIL::SigSpec ڪڍو_2(ثابت pool<RTLIL::Sig_ٻٽو_انگ> &نمونو, ثابت RTLIL::SigSpec *other = عدم) ثابت;
	RTLIL::SigSpec ڪڍو_2(صحيح وٿي, صحيح ڊيگهائي = 1) ثابت;
	RTLIL::SigSpec ڪڍو_پڇاڙي(صحيح وٿي) ثابت { أرجع ڪڍو_2(وٿي, سائيز() - وٿي); }

	فراغ rewrite_wires(std::function<فراغ(RTLIL::Wire*& wire)> rewrite);

	RTLIL::Sig_ٻٽو_انگ lsb() ثابت { log_assert(سائيز()); أرجع (*this)[0]; };
	RTLIL::Sig_ٻٽو_انگ msb() ثابت { log_assert(سائيز()); أرجع (*this)[سائيز() - 1]; };
	RTLIL::Sig_ٻٽو_انگ front() ثابت { أرجع (*this)[0]; }
	RTLIL::Sig_ٻٽو_انگ پٺ() ثابت { أرجع (*this)[سائيز() - 1]; }

	فراغ append(ثابت RTLIL::SigSpec &إشارة);
	مضمن فراغ append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	مضمن فراغ append(ثابت RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	مضمن فراغ append(ثابت RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	فراغ append(ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ);
	مضمن فراغ append(RTLIL::State state) { append(RTLIL::Sig_ٻٽو_انگ(state)); }
	مضمن فراغ append(bool bool_) { append(RTLIL::Sig_ٻٽو_انگ(bool_)); }

	فراغ extend_u0(صحيح ويڪر, bool is_signed = false);

	RTLIL::SigSpec repeat(صحيح انگ) ثابت;

	فراغ ابتو() { inline_unpack(); std::ابتو(بٽ_2.begin(), بٽ_2.پڇاڙي()); }

	bool عامل <(ثابت RTLIL::SigSpec &other) ثابت;
	bool عامل ==(ثابت RTLIL::SigSpec &other) ثابت;
	مضمن bool عامل !=(ثابت RTLIL::SigSpec &other) ثابت { أرجع !(*this == other); }

	bool is_wire() ثابت;
	bool is_chunk() ثابت;
	مضمن bool is_ٻٽو_انگ() ثابت { أرجع سائيز() == 1; }

	bool معلوم_driver() ثابت;

	bool is_fully_const() ثابت;
	bool is_fully_zero() ثابت;
	bool is_fully_ones() ثابت;
	bool is_fully_def() ثابت;
	bool is_fully_undef() ثابت;
	bool has_const() ثابت;
	bool has_const(State state) ثابت;
	bool has_marked_بٽ() ثابت;
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
	bool convertible_منزل_int(bool is_signed = false) ثابت;

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
	RTLIL::Sig_ٻٽو_انگ as_ٻٽو_انگ() ثابت;

	bool ميلاپ(ثابت حرف* نمونو) ثابت;

	std::سيٽ<RTLIL::Sig_ٻٽو_انگ> منزل_sigbit_سيٽ() ثابت;
	pool<RTLIL::Sig_ٻٽو_انگ> منزل_sigbit_pool() ثابت;
	std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ> منزل_sigbit_ويڪٽر() ثابت;
	std::map<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> منزل_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت;
	dict<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> منزل_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت;

	ساكن bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	ساكن bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	ساكن bool parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	عامل std::ويڪٽر<RTLIL::SigChunk>() ثابت;
	عامل std::ويڪٽر<RTLIL::Sig_ٻٽو_انگ>() ثابت { أرجع منزل_sigbit_ويڪٽر(); }
	ثابت RTLIL::Sig_ٻٽو_انگ &at(صحيح وٿي, ثابت RTLIL::Sig_ٻٽو_انگ &defval) { أرجع وٿي < سائيز() ? (*this)[وٿي] : defval; }
	RTLIL::Sig_ٻٽو_انگ& at(صحيح وٿي) { أرجع (*this)[وٿي]; }
	RTLIL::Sig_ٻٽو_انگ at(صحيح وٿي) ثابت { أرجع (*this)[وٿي]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		Hasher::hash_t val;
		إذا (std::optional<Hasher::hash_t> current = hash_.اقرأ())
			val = *current;
		وإلا
			val = updhash();
		h.eat(val);
		أرجع h;
	}

#جيڪڏهن_وصف_نه_ٿيل NDEBUG
	فراغ check(ثابت Module *mod = nullptr) ثابت;
#وإلا
	فراغ check(ثابت Module *mod = nullptr) ثابت { (فراغ)mod; }
#جيڪڏهن_پورو
};

بنية RTLIL::Selection
{
	// selection includes boxed modules
	bool selects_boxes;
	// selection covers full design, including boxed modules
	bool complete_selection;
	// selection covers full design, not including boxed modules
	bool ڀريل_selection;
	pool<RTLIL::IdString> selected_modules;
	dict<RTLIL::IdString, pool<RTLIL::IdString>> selected_members;
	RTLIL::Design *current_design;

	// create a new selection
	Selection(
		// should the selection cover the full design
		bool ڀريل_2 = true,
		// should the selection include boxed modules
		bool boxes = false,
		// the design to select from
		RTLIL::Design *design = nullptr
	) :
		selects_boxes(boxes), complete_selection(ڀريل_2 && boxes), ڀريل_selection(ڀريل_2 && !boxes), current_design(design) { }

	// checks if the given module exists in the current design and is a
	// boxed module, warning the user if the current design is not set
	bool boxed_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_سڄو_module(RTLIL::IdString mod_name) ثابت;

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
		أرجع ڀريل_selection || complete_selection;
	}

	// add whole module to this selection
	سانچو<typename T1> فراغ select(T1 *module) {
		إذا (!selects_all() && selected_modules.ڳڻپ(module->name) == 0) {
			IdString name = module->name;
			selected_modules.وجهو(name);
			selected_members.erase(name);
			إذا (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	سانچو<typename T1, typename T2> فراغ select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	سانچو<typename T1> فراغ select(T1 *module, RTLIL::IdString member) {
		إذا (!selects_all() && selected_modules.ڳڻپ(module->name.ref()) == 0) {
			selected_members[module->name.ref()].وجهو(member);
			إذا (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool خالي_جايون() ثابت {
		أرجع !selects_all() && selected_modules.خالي_جايون() && selected_members.خالي_جايون();
	}

	// clear this selection, leaving it empty
	فراغ clear();

	// create a new selection which is empty
	ساكن Selection خالي_جايون_Selection(RTLIL::Design *design = nullptr) { أرجع Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	ساكن Selection ڀريل_Selection(RTLIL::Design *design = nullptr) { أرجع Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	ساكن Selection CompleteSelection(RTLIL::Design *design = nullptr) { أرجع Selection(true, true, design); };
};

بنية RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

	Monitor() {
		ساكن بنانشان صحيح hashidx_ڳڻپ = 123456789;
		hashidx_ڳڻپ = mkhash_xorshift(hashidx_ڳڻپ);
		hashidx_ = hashidx_ڳڻپ;
	}

	مجازي ~Monitor() { }
	مجازي فراغ notify_module_جوڙيو(RTLIL::Module*) { }
	مجازي فراغ notify_module_del(RTLIL::Module*) { }
	مجازي فراغ notify_connect(RTLIL::Cell*, RTLIL::IdString, ثابت RTLIL::SigSpec&, ثابت RTLIL::SigSpec&) { }
	مجازي فراغ notify_connect(RTLIL::Module*, ثابت RTLIL::SigSig&) { }
	مجازي فراغ notify_connect(RTLIL::Module*, ثابت std::ويڪٽر<RTLIL::SigSig>&) { }
	مجازي فراغ notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
بنية define_map_t;

سانچو<typename N>
مضمن constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::پن> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ثابت حرف*> || std::is_same_v<std::decay_t<N>, حرف*>;

#وصف_ڪريو YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, صحيح> = 0

سانچو<typename N>
مضمن constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ثابت حرف *> || std::is_same_v<std::decay_t<N>, حرف *>;

#وصف_ڪريو YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, صحيح> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#وصف_ڪريو YS_NAME_FWD_POOL(_func, _pool) \
	سانچو<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(خودڪار) _func(N name, Rest&&... rest) \
		{ أرجع _func(_pool.جوڙيو(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#وصف_ڪريو YS_NAME_FWD_2ND_POOL(_func, _pool) \
	سانچو<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(خودڪار) _func(T &&پهريون, N name) \
		{ أرجع _func(std::forward<T>(پهريون), _pool.جوڙيو(std::move(name))); }

#وصف_ڪريو YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#وصف_ڪريو YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

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

	سائيز_t gc_twines();

	std::ويڪٽر<std::unique_ptr<AST::Ast_ڳنڍ>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_مقرر_ڪري;

	std::ويڪٽر<RTLIL::Selection> selection_ڍير;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_سرگرم_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ثابت RTLIL::Module *module(RTLIL::IdString name) ثابت;
	سانچو<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ أرجع module(twines().ڳوليو(std::string_view(name))); }
	سانچو<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Module *module(N name) ثابت
		{ أرجع module(twines().ڳوليو(std::string_view(name))); }
	RTLIL::Module *چوٽي_module() ثابت;

	bool has(RTLIL::IdString id) ثابت {
		twines_.check_owned(id);
		أرجع modules_.ڳڻپ(id) != 0;
	}
	سانچو<typename N, YS_NAME_STRING(N)> bool has(N name) ثابت
		{ أرجع has(twines().ڳوليو(std::string_view(name))); }

	فراغ جوڙيو(RTLIL::Module *module);

	RTLIL::Module *جوڙيو_Module(RTLIL::IdString name);
	YS_NAME_FWD_POOL(جوڙيو_Module, twines())
	فراغ remove(RTLIL::Module *module);
	فراغ rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	فراغ scratchpad_unset(ثابت std::string &varname);

	فراغ scratchpad_سيٽ_int(ثابت std::string &varname, صحيح قدر);
	فراغ scratchpad_سيٽ_bool(ثابت std::string &varname, bool قدر);
	فراغ scratchpad_سيٽ_string(ثابت std::string &varname, std::string قدر);

	صحيح scratchpad_get_int(ثابت std::string &varname, صحيح default_قدر_2 = 0) ثابت;
	bool scratchpad_get_bool(ثابت std::string &varname, bool default_قدر_2 = false) ثابت;
	std::string scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_قدر_2 = std::string()) ثابت;

	فراغ ترتيب_ڏيو();
	فراغ ترتيب_ڏيو_modules();
	فراغ check();
	فراغ optimize();

	فراغ clone_into(RTLIL::Design *dst) ثابت;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_سڄو_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ثابت;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_سڄو_module(RTLIL::Module *mod) ثابت;

	// push the given selection to the selection stack
	فراغ ڌڪو_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	فراغ ڌڪو_خالي_جايون_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	فراغ ڌڪو_ڀريل_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	فراغ ڌڪو_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	فراغ ڪڍو_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		أرجع selection_ڍير.پٺ();
	}

	// get the current selection
	ثابت RTLIL::Selection &selection() ثابت {
		أرجع selection_ڍير.پٺ();
	}

	// is the current selection a full selection (no boxes)
	bool ڀريل_selection() ثابت {
		أرجع selection().ڀريل_selection;
	}

	// is the given module in the current selection
	سانچو<typename T1> bool selected(T1 *module) ثابت {
		أرجع selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	سانچو<typename T1, typename T2> bool selected(T1 *module, T2 *member) ثابت {
		أرجع selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	سانچو<typename T1> فراغ select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	سانچو<typename T1, typename T2> فراغ select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::ويڪٽر<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) ثابت;

	// returns all selected modules, and may include boxes
	std::ويڪٽر<RTLIL::Module*> all_selected_modules() ثابت { أرجع selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::ويڪٽر<RTLIL::Module*> selected_unboxed_modules() ثابت { أرجع selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::ويڪٽر<RTLIL::Module*> selected_unboxed_modules_warn() ثابت { أرجع selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::ويڪٽر<RTLIL::Module*> selected_سڄو_modules() ثابت { أرجع selected_modules(SELECT_سڄو_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::ويڪٽر<RTLIL::Module*> all_selected_سڄو_modules() ثابت { أرجع selected_modules(SELECT_سڄو_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::ويڪٽر<RTLIL::Module*> selected_سڄو_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ثابت { أرجع selected_modules(SELECT_سڄو_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::ويڪٽر<RTLIL::Module*> selected_unboxed_سڄو_modules() ثابت { أرجع selected_modules(SELECT_سڄو_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::ويڪٽر<RTLIL::Module*> selected_unboxed_سڄو_modules_warn() ثابت { أرجع selected_modules(SELECT_سڄو_WARN, SB_UNBOXED_WARN); }

	ساكن std::map<بنانشان صحيح, RTLIL::Design*> *get_all_designs(فراغ);

	std::string منزل_rtlil_str(bool only_selected = true) ثابت;
};

بنية RTLIL::Module : عام RTLIL::AttrObject
{
	دوست بنية RTLIL::Cell;
	دوست بنية RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	فراغ جوڙيو(RTLIL::Wire *wire);
	فراغ جوڙيو(RTLIL::Cell *cell);
	فراغ جوڙيو(RTLIL::Process *process);

عام:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ثابت { أرجع design->twines(); }

	صحيح refcount_wires_;
	صحيح refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::ويڪٽر<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_قدرون;
	dict<RTLIL::IdString, RTLIL::يادگيري*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	مجازي ~Module();
	مجازي RTLIL::IdString derive(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	مجازي RTLIL::IdString derive(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Const> &parameters, ثابت dict<RTLIL::IdString, RTLIL::Module*> &interfaces, ثابت dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	مجازي سائيز_t ڳڻپ_id(RTLIL::IdString id);
	مجازي فراغ expand_interfaces(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	مجازي bool reprocess_if_necessary(RTLIL::Design *design);

	مجازي فراغ ترتيب_ڏيو();
	مجازي فراغ check();
	مجازي فراغ optimize();
	مجازي فراغ makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ثابت {
		أرجع get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	فراغ connect(ثابت RTLIL::SigSig &conn);
	فراغ connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs);
	فراغ new_connections(ثابت std::ويڪٽر<RTLIL::SigSig> &new_conn);
	ثابت std::ويڪٽر<RTLIL::SigSig> &connections() ثابت;

	std::ويڪٽر<RTLIL::IdString> ports;
	فراغ fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_قطار;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_پورٽ_قطار;
	pool<RTLIL::Wire *> buf_norm_wire_قطار;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_اشاريو;
	فراغ bufNormalize();

	سانچو<typename T> فراغ rewrite_sigspecs(T &functor);
	سانچو<typename T> فراغ rewrite_sigspecs2(T &functor);
	فراغ cloneInto(RTLIL::Module *new_mod) ثابت;
	مجازي RTLIL::Module *clone() ثابت;
	مجازي RTLIL::Module *clone(RTLIL::Design *dst) ثابت;
	مجازي RTLIL::Module *clone(RTLIL::Design *dst, IdString هدف_name) ثابت;

	bool has_memories() ثابت;
	bool has_processes() ثابت;

	bool has_memories_warn() ثابت;
	bool has_processes_warn() ثابت;

	bool is_selected() ثابت;
	bool is_selected_سڄو() ثابت;

	std::ويڪٽر<RTLIL::Wire*> selected_wires() ثابت;
	std::ويڪٽر<RTLIL::Cell*> selected_cells() ثابت;
	std::ويڪٽر<RTLIL::يادگيري*> selected_memories() ثابت;
	std::ويڪٽر<RTLIL::Process*> selected_processes() ثابت;
	std::ويڪٽر<RTLIL::SelectedMember> selected_members() ثابت;

	سانچو<typename T> bool selected(T *member) ثابت {
		أرجع design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		إذا (design)
			design->twines().check_owned(id);
		خودڪار it = wires_.ڳوليو(id);
		أرجع it == wires_.پڇاڙي() ? nullptr : it->ٻيو;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		إذا (design)
			design->twines().check_owned(id);
		خودڪار it = cells_.ڳوليو(id);
		أرجع it == cells_.پڇاڙي() ? nullptr : it->ٻيو;
	}

	ثابت RTLIL::Wire* wire(RTLIL::IdString id) ثابت{
		إذا (design)
			design->twines().check_owned(id);
		خودڪار it = wires_.ڳوليو(id);
		أرجع it == wires_.پڇاڙي() ? nullptr : it->ٻيو;
	}
	ثابت RTLIL::Cell* cell(RTLIL::IdString id) ثابت {
		إذا (design)
			design->twines().check_owned(id);
		خودڪار it = cells_.ڳوليو(id);
		أرجع it == cells_.پڇاڙي() ? nullptr : it->ٻيو;
	}

	سانچو<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ أرجع wire(twines().ڳوليو(std::string_view(name))); }
	سانچو<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ أرجع cell(twines().ڳوليو(std::string_view(name))); }
	سانچو<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Wire *wire(N name) ثابت
		{ أرجع wire(twines().ڳوليو(std::string_view(name))); }
	سانچو<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Cell *cell(N name) ثابت
		{ أرجع cell(twines().ڳوليو(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { أرجع RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	صحيح wires_سائيز() ثابت { أرجع wires_.سائيز(); }
	RTLIL::Wire* wire_at(صحيح اشاريو) ثابت { أرجع wires_.element(اشاريو)->ٻيو; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { أرجع RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	صحيح cells_سائيز() ثابت { أرجع cells_.سائيز(); }
	RTLIL::Cell* cell_at(صحيح اشاريو) ثابت { أرجع cells_.element(اشاريو)->ٻيو; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	فراغ remove(ثابت pool<RTLIL::Wire*> &wires);
	فراغ remove(RTLIL::Cell *cell);
	فراغ remove(RTLIL::يادگيري *يادگيري_3);
	فراغ remove(RTLIL::Process *process);

	فراغ rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	فراغ rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	فراغ rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	فراغ مٽا_سٽا_نالا(RTLIL::Wire *w1, RTLIL::Wire *w2);
	فراغ مٽا_سٽا_نالا(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, صحيح &اشاريو);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *جوڙيو_Wire(RTLIL::IdString name, صحيح ويڪر = 1);
	RTLIL::Wire *جوڙيو_Wire(RTLIL::IdString name, ثابت RTLIL::Wire *other);
	YS_NAME_FWD(جوڙيو_Wire)

	RTLIL::Cell *جوڙيو_Cell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *جوڙيو_Cell(RTLIL::IdString name, ثابت RTLIL::Cell *other);
	YS_NAME_FWD(جوڙيو_Cell)
	سانچو<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *جوڙيو_Cell(RTLIL::IdString name, T type) { أرجع جوڙيو_Cell(name, design->twines().جوڙيو(std::move(type))); }

	RTLIL::يادگيري *جوڙيو_يادگيري(RTLIL::IdString name);
	YS_NAME_FWD(جوڙيو_يادگيري)
	RTLIL::يادگيري *جوڙيو_يادگيري(RTLIL::IdString name, ثابت RTLIL::يادگيري *other);

	RTLIL::Process *جوڙيو_Process(RTLIL::IdString name);
	YS_NAME_FWD(جوڙيو_Process)
	RTLIL::Process *جوڙيو_Process(RTLIL::IdString name, ثابت RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* جوڙيو_Not     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Pos     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Buf     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Barrier (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Neg     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_And  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Or   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Xor  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Xnor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_Reduce_And  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Reduce_Or   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Reduce_Xor  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Reduce_Xnor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Reduce_Bool (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_Shl    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Shr    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Sshl   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Sshr   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_سرڪايو  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Shiftx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_Lt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Le  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Eq  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Ne  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Eqx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Nex (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Ge  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Gt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_جوڙيو (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Sub (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Mul (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	// truncating division
	RTLIL::Cell* جوڙيو_Div (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	// truncating modulo
	RTLIL::Cell* جوڙيو_Mod (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Div_Floor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Mod_Floor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Pow (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_Fa (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_x, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_Logic_Not (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Logic_And (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Logic_Or  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_Mux  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Pmux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Bmux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Demux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_Bweqx  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Bwmux  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_ٽڪرو  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const وٿي, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Concat (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Lut    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Tribuf (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Assert (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Assume (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_جيئرو   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Fair   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Cover  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Equiv  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_Sr    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr, ثابت RTLIL::SigSpec &sig_q, bool سيٽ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Ff    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Dff   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d,   ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Dffe  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Dffsr (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool سيٽ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Dffsre (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool سيٽ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Adff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_قدر, bool clk_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Adffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_قدر, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Aldff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Aldffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Sdff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_قدر, bool clk_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Sdffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_قدر, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Sdffce (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_قدر, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Dlatch (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Adlatch (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_قدر, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Dlatchsr (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, bool سيٽ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_Buf_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Not_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_And_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Nand_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Or_Gate     (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Nor_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Xor_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Xnor_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Andnot_Gate (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Ornot_Gate  (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Mux_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_s, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Nmux_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_s, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Aoi3_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_c, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Oai3_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_c, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Aoi4_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_c, ثابت RTLIL::Sig_ٻٽو_انگ &sig_d, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Oai4_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_c, ثابت RTLIL::Sig_ٻٽو_انگ &sig_d, ثابت RTLIL::Sig_ٻٽو_انگ &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_Sr_Gate     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr,
			ثابت RTLIL::SigSpec &sig_q, bool سيٽ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Ff_Gate     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Dff_Gate    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Dffe_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Dffsr_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool سيٽ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Dffsre_Gate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool سيٽ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Adff_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_قدر = false, bool clk_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Adffe_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_قدر = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Aldff_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Aldffe_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Sdff_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_قدر = false, bool clk_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Sdffe_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_قدر = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Sdffce_Gate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_قدر = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Dlatch_Gate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Adlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_قدر = false, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڙيو_Dlatchsr_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سيٽ, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, bool سيٽ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");

	RTLIL::Cell* جوڙيو_Anyinit(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");

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
	RTLIL::SigSpec سرڪايو_2  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec جوڙيو_2 (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
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

	RTLIL::Sig_ٻٽو_انگ BufGate    (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ NotGate    (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ AndGate    (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ NandGate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ OrGate     (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ NorGate    (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ XorGate    (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ XnorGate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ AndnotGate (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ OrnotGate  (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ MuxGate    (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_s, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ NmuxGate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_s, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ Aoi3Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_c, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ Oai3Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_c, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ Aoi4Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_c, ثابت RTLIL::Sig_ٻٽو_انگ &sig_d, ثابت std::string &src = "");
	RTLIL::Sig_ٻٽو_انگ Oai4Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ٻٽو_انگ &sig_a, ثابت RTLIL::Sig_ٻٽو_انگ &sig_b, ثابت RTLIL::Sig_ٻٽو_انگ &sig_c, ثابت RTLIL::Sig_ٻٽو_انگ &sig_d, ثابت std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, صحيح ويڪر = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, صحيح ويڪر = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, صحيح ويڪر = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, صحيح ويڪر = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ثابت std::string &src = "");

	RTLIL::SigSpec سيٽ_ٽيگ_2          (RTLIL::IdString name, ثابت std::string &ٽيگ, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src = "");
	RTLIL::Cell*   جوڙيو_سيٽ_ٽيگ       (RTLIL::IdString name, ثابت std::string &ٽيگ, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::SigSpec Get_ٽيگ          (RTLIL::IdString name, ثابت std::string &ٽيگ, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src = "");
	RTLIL::Cell*   جوڙيو_Overwrite_ٽيگ (RTLIL::IdString name, ثابت std::string &ٽيگ, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src = "");
	RTLIL::SigSpec Original_ٽيگ     (RTLIL::IdString name, ثابت std::string &ٽيگ, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src = "");
	YS_NAME_FWD(جوڙيو_Anyinit)
	YS_NAME_FWD(Anyconst) YS_NAME_FWD(Anyseq) YS_NAME_FWD(Allconst) YS_NAME_FWD(Allseq)
	YS_NAME_FWD(Initstate)
	YS_NAME_FWD(سيٽ_ٽيگ_2) YS_NAME_FWD(جوڙيو_سيٽ_ٽيگ) YS_NAME_FWD(Get_ٽيگ)
	YS_NAME_FWD(جوڙيو_Overwrite_ٽيگ) YS_NAME_FWD(Original_ٽيگ) YS_NAME_FWD(FutureFF)
	YS_NAME_FWD(جوڙيو_Not) YS_NAME_FWD(جوڙيو_Pos) YS_NAME_FWD(جوڙيو_Buf) YS_NAME_FWD(جوڙيو_Barrier) YS_NAME_FWD(جوڙيو_Neg)
	YS_NAME_FWD(جوڙيو_And) YS_NAME_FWD(جوڙيو_Or) YS_NAME_FWD(جوڙيو_Xor) YS_NAME_FWD(جوڙيو_Xnor)
	YS_NAME_FWD(جوڙيو_Reduce_And) YS_NAME_FWD(جوڙيو_Reduce_Or) YS_NAME_FWD(جوڙيو_Reduce_Xor) YS_NAME_FWD(جوڙيو_Reduce_Xnor)
	YS_NAME_FWD(جوڙيو_Reduce_Bool) YS_NAME_FWD(جوڙيو_Shl) YS_NAME_FWD(جوڙيو_Shr) YS_NAME_FWD(جوڙيو_Sshl)
	YS_NAME_FWD(جوڙيو_Sshr) YS_NAME_FWD(جوڙيو_سرڪايو) YS_NAME_FWD(جوڙيو_Shiftx) YS_NAME_FWD(جوڙيو_Lt)
	YS_NAME_FWD(جوڙيو_Le) YS_NAME_FWD(جوڙيو_Eq) YS_NAME_FWD(جوڙيو_Ne) YS_NAME_FWD(جوڙيو_Eqx)
	YS_NAME_FWD(جوڙيو_Nex) YS_NAME_FWD(جوڙيو_Ge) YS_NAME_FWD(جوڙيو_Gt) YS_NAME_FWD(جوڙيو_جوڙيو)
	YS_NAME_FWD(جوڙيو_Sub) YS_NAME_FWD(جوڙيو_Mul) YS_NAME_FWD(جوڙيو_Div) YS_NAME_FWD(جوڙيو_Mod)
	YS_NAME_FWD(جوڙيو_Div_Floor) YS_NAME_FWD(جوڙيو_Mod_Floor) YS_NAME_FWD(جوڙيو_Pow) YS_NAME_FWD(جوڙيو_Fa)
	YS_NAME_FWD(جوڙيو_Logic_Not) YS_NAME_FWD(جوڙيو_Logic_And) YS_NAME_FWD(جوڙيو_Logic_Or) YS_NAME_FWD(جوڙيو_Mux)
	YS_NAME_FWD(جوڙيو_Pmux) YS_NAME_FWD(جوڙيو_Bmux) YS_NAME_FWD(جوڙيو_Demux) YS_NAME_FWD(جوڙيو_Bweqx)
	YS_NAME_FWD(جوڙيو_Bwmux) YS_NAME_FWD(جوڙيو_ٽڪرو) YS_NAME_FWD(جوڙيو_Concat) YS_NAME_FWD(جوڙيو_Lut)
	YS_NAME_FWD(جوڙيو_Tribuf) YS_NAME_FWD(جوڙيو_Assert) YS_NAME_FWD(جوڙيو_Assume) YS_NAME_FWD(جوڙيو_جيئرو)
	YS_NAME_FWD(جوڙيو_Fair) YS_NAME_FWD(جوڙيو_Cover) YS_NAME_FWD(جوڙيو_Equiv) YS_NAME_FWD(جوڙيو_Sr)
	YS_NAME_FWD(جوڙيو_Ff) YS_NAME_FWD(جوڙيو_Dff) YS_NAME_FWD(جوڙيو_Dffe) YS_NAME_FWD(جوڙيو_Dffsr)
	YS_NAME_FWD(جوڙيو_Dffsre) YS_NAME_FWD(جوڙيو_Adff) YS_NAME_FWD(جوڙيو_Adffe) YS_NAME_FWD(جوڙيو_Aldff)
	YS_NAME_FWD(جوڙيو_Aldffe) YS_NAME_FWD(جوڙيو_Sdff) YS_NAME_FWD(جوڙيو_Sdffe) YS_NAME_FWD(جوڙيو_Sdffce)
	YS_NAME_FWD(جوڙيو_Dlatch) YS_NAME_FWD(جوڙيو_Adlatch) YS_NAME_FWD(جوڙيو_Dlatchsr) YS_NAME_FWD(جوڙيو_Buf_Gate)
	YS_NAME_FWD(جوڙيو_Not_Gate) YS_NAME_FWD(جوڙيو_And_Gate) YS_NAME_FWD(جوڙيو_Nand_Gate) YS_NAME_FWD(جوڙيو_Or_Gate)
	YS_NAME_FWD(جوڙيو_Nor_Gate) YS_NAME_FWD(جوڙيو_Xor_Gate) YS_NAME_FWD(جوڙيو_Xnor_Gate) YS_NAME_FWD(جوڙيو_Andnot_Gate)
	YS_NAME_FWD(جوڙيو_Ornot_Gate) YS_NAME_FWD(جوڙيو_Mux_Gate) YS_NAME_FWD(جوڙيو_Nmux_Gate) YS_NAME_FWD(جوڙيو_Aoi3_Gate)
	YS_NAME_FWD(جوڙيو_Oai3_Gate) YS_NAME_FWD(جوڙيو_Aoi4_Gate) YS_NAME_FWD(جوڙيو_Oai4_Gate) YS_NAME_FWD(جوڙيو_Sr_Gate)
	YS_NAME_FWD(جوڙيو_Ff_Gate) YS_NAME_FWD(جوڙيو_Dff_Gate) YS_NAME_FWD(جوڙيو_Dffe_Gate) YS_NAME_FWD(جوڙيو_Dffsr_Gate)
	YS_NAME_FWD(جوڙيو_Dffsre_Gate) YS_NAME_FWD(جوڙيو_Adff_Gate) YS_NAME_FWD(جوڙيو_Adffe_Gate) YS_NAME_FWD(جوڙيو_Aldff_Gate)
	YS_NAME_FWD(جوڙيو_Aldffe_Gate) YS_NAME_FWD(جوڙيو_Sdff_Gate) YS_NAME_FWD(جوڙيو_Sdffe_Gate) YS_NAME_FWD(جوڙيو_Sdffce_Gate)
	YS_NAME_FWD(جوڙيو_Dlatch_Gate) YS_NAME_FWD(جوڙيو_Adlatch_Gate) YS_NAME_FWD(جوڙيو_Dlatchsr_Gate) YS_NAME_FWD(Not)
	YS_NAME_FWD(Pos) YS_NAME_FWD(Buf) YS_NAME_FWD(Barrier) YS_NAME_FWD(Neg) YS_NAME_FWD(And)
	YS_NAME_FWD(Or) YS_NAME_FWD(Xor) YS_NAME_FWD(Xnor) YS_NAME_FWD(ReduceAnd)
	YS_NAME_FWD(ReduceOr) YS_NAME_FWD(ReduceXor) YS_NAME_FWD(ReduceXnor) YS_NAME_FWD(ReduceBool)
	YS_NAME_FWD(Shl) YS_NAME_FWD(Shr) YS_NAME_FWD(Sshl) YS_NAME_FWD(Sshr)
	YS_NAME_FWD(سرڪايو_2) YS_NAME_FWD(Shiftx) YS_NAME_FWD(Lt) YS_NAME_FWD(Le)
	YS_NAME_FWD(Eq) YS_NAME_FWD(Ne) YS_NAME_FWD(Eqx) YS_NAME_FWD(Nex)
	YS_NAME_FWD(Ge) YS_NAME_FWD(Gt) YS_NAME_FWD(جوڙيو_2) YS_NAME_FWD(Sub)
	YS_NAME_FWD(Mul) YS_NAME_FWD(Div) YS_NAME_FWD(Mod) YS_NAME_FWD(DivFloor)
	YS_NAME_FWD(ModFloor) YS_NAME_FWD(Pow) YS_NAME_FWD(LogicNot) YS_NAME_FWD(LogicAnd)
	YS_NAME_FWD(LogicOr) YS_NAME_FWD(Mux) YS_NAME_FWD(Pmux) YS_NAME_FWD(Bmux)
	YS_NAME_FWD(Demux) YS_NAME_FWD(Bweqx) YS_NAME_FWD(Bwmux) YS_NAME_FWD(BufGate)
	YS_NAME_FWD(NotGate) YS_NAME_FWD(AndGate) YS_NAME_FWD(NandGate) YS_NAME_FWD(OrGate)
	YS_NAME_FWD(NorGate) YS_NAME_FWD(XorGate) YS_NAME_FWD(XnorGate) YS_NAME_FWD(AndnotGate)
	YS_NAME_FWD(OrnotGate) YS_NAME_FWD(MuxGate) YS_NAME_FWD(NmuxGate) YS_NAME_FWD(Aoi3Gate)
	YS_NAME_FWD(Oai3Gate) YS_NAME_FWD(Aoi4Gate) YS_NAME_FWD(Oai4Gate)

	std::string منزل_rtlil_str() ثابت;
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	ساكن std::map<بنانشان صحيح, RTLIL::Module*> *get_all_modules(فراغ);
#جيڪڏهن_پورو
};

بنية RTLIL::Wire : عام RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	// use module->addWire() and module->remove() to create or destroy wires
	دوست بنية RTLIL::Module;
	Wire();
	~Wire();

	دوست بنية RTLIL::Design;
	دوست بنية RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driver_پورٽ_2;

عام:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	فراغ عامل=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	صحيح ويڪر, شروعات_وٿي, پورٽ_id;
	bool پورٽ_داخلا, پورٽ_output, upto, is_signed;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	bool معلوم_driver() ثابت { أرجع driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ثابت    { log_assert(driverCell_); أرجع driverCell_; };
	RTLIL::IdString driver_پورٽ() ثابت { log_assert(driverCell_); أرجع driver_پورٽ_2; };

	صحيح from_hdl_اشاريو(صحيح hdl_اشاريو) {
		صحيح zero_اشاريو = hdl_اشاريو - شروعات_وٿي;
		صحيح rtlil_اشاريو = upto ? ويڪر - 1 - zero_اشاريو : zero_اشاريو;
		أرجع rtlil_اشاريو >= 0 && rtlil_اشاريو < ويڪر ? rtlil_اشاريو : INT_MIN;
	}

	صحيح منزل_hdl_اشاريو(صحيح rtlil_اشاريو) {
		إذا (rtlil_اشاريو < 0 || rtlil_اشاريو >= ويڪر)
			أرجع INT_MIN;
		صحيح zero_اشاريو = upto ? ويڪر - 1 - rtlil_اشاريو : rtlil_اشاريو;
		أرجع zero_اشاريو + شروعات_وٿي;
	}

	std::string منزل_rtlil_str() ثابت;

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	ساكن std::map<بنانشان صحيح, RTLIL::Wire*> *get_all_wires(فراغ);
#جيڪڏهن_پورو
};

مضمن صحيح Get_سائيز(RTLIL::Wire *wire) {
	أرجع wire->ويڪر;
}

بنية RTLIL::يادگيري : عام RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

	يادگيري();
	~يادگيري();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	RTLIL::يادگيري_Name_Id name;

	صحيح ويڪر, شروعات_وٿي, سائيز;
#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	ساكن std::map<بنانشان صحيح, RTLIL::يادگيري*> *get_all_memorys(فراغ);
#جيڪڏهن_پورو

	std::string منزل_rtlil_str() ثابت;
};

بنية RTLIL::Cell : عام RTLIL::AttrObject
{
خاص:
	فراغ init_اشاريو();

	bool bufnorm_handle_سيٽ_پورٽ(IdString portname, RTLIL::SigSpec &إشارة, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
عام:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	// use module->addCell() and module->remove() to create or destroy cells
	دوست بنية RTLIL::Module;
	Cell();
	~Cell();

عام:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	فراغ عامل=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool has_پورٽ(RTLIL::IdString portname) ثابت;
	فراغ unset_پورٽ(RTLIL::IdString portname);
	فراغ سيٽ_پورٽ(RTLIL::IdString portname, RTLIL::SigSpec إشارة);
	سانچو<typename N, YS_NAME_STRING(N)> فراغ سيٽ_پورٽ(N portname, RTLIL::SigSpec إشارة)
		{ سيٽ_پورٽ(module->twines().جوڙيو(std::move(portname)), std::move(إشارة)); }
	ثابت RTLIL::SigSpec &get_پورٽ(RTLIL::IdString portname) ثابت;
	ثابت dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ثابت;

	bool معلوم() ثابت;
	bool داخلا(RTLIL::IdString portname) ثابت;
	bool output(RTLIL::IdString portname) ثابت;
	پورٽ_Dir پورٽ_dir(RTLIL::IdString portname) ثابت;

	bool hasParam(RTLIL::IdString paramname) ثابت;
	فراغ unsetParam(RTLIL::IdString paramname);
	فراغ سيٽ_Param(RTLIL::IdString paramname, RTLIL::Const قدر);
	ثابت RTLIL::Const &getParam(RTLIL::IdString paramname) ثابت;

	سانچو<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ثابت
		{ أرجع hasParam(module->twines().ڳوليو(std::string_view(name))); }
	سانچو<typename N, YS_NAME_STRING(N)> فراغ unsetParam(N name)
		{ unsetParam(module->twines().ڳوليو(std::string_view(name))); }
	سانچو<typename N, YS_NAME_STRING(N)> فراغ سيٽ_Param(N name, RTLIL::Const قدر)
		{ سيٽ_Param(module->twines().جوڙيو(std::move(name)), std::move(قدر)); }
	سانچو<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Const &getParam(N name) ثابت
		{ أرجع getParam(module->twines().ڳوليو(std::string_view(name))); }

	فراغ ترتيب_ڏيو();
	فراغ check();
	فراغ fixup_parameters(bool سيٽ_a_signed = false, bool سيٽ_b_signed = false);

	bool has_keep_attr() ثابت;

	سانچو<typename T> فراغ rewrite_sigspecs(T &functor);
	سانچو<typename T> فراغ rewrite_sigspecs2(T &functor);

#جيڪڏهن_وصف_ٿيل YOSYS_ENABLE_PYTHON
	ساكن std::map<بنانشان صحيح, RTLIL::Cell*> *get_all_cells(فراغ);
#جيڪڏهن_پورو

	bool has_memid() ثابت;
	bool is_mem_cell() ثابت;
	bool is_builtin_ff() ثابت;

	std::string منزل_rtlil_str() ثابت;
};

بنية RTLIL::Case_قاعدو : عام RTLIL::AttrObject
{
	std::ويڪٽر<RTLIL::SigSpec> compare;
	std::ويڪٽر<RTLIL::SigSig> actions;
	std::ويڪٽر<RTLIL::Switch_قاعدو*> switches;

	~Case_قاعدو();

	bool خالي_جايون() ثابت;

	سانچو<typename T> فراغ rewrite_sigspecs(T &functor);
	سانچو<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Case_قاعدو *clone() ثابت;
};

بنية RTLIL::Switch_قاعدو : عام RTLIL::AttrObject
{
	RTLIL::SigSpec إشارة;
	std::ويڪٽر<RTLIL::Case_قاعدو*> cases;

	~Switch_قاعدو();

	bool خالي_جايون() ثابت;

	سانچو<typename T> فراغ rewrite_sigspecs(T &functor);
	سانچو<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Switch_قاعدو *clone() ثابت;
};

بنية RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec پتو;
	RTLIL::SigSpec معلومات;
	RTLIL::SigSpec enable;
	RTLIL::Const ترجيح_نقاب;
};

بنية RTLIL::Sync_قاعدو
{
	RTLIL::SyncType type;
	RTLIL::SigSpec إشارة;
	std::ويڪٽر<RTLIL::SigSig> actions;
	std::ويڪٽر<RTLIL::MemWriteAction> mem_write_actions;

	سانچو<typename T> فراغ rewrite_sigspecs(T &functor);
	سانچو<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Sync_قاعدو *clone() ثابت;
};

بنية RTLIL::Process : عام RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	// use module->addProcess() and module->remove() to create or destroy processes
	دوست بنية RTLIL::Module;
	Process();
	~Process();

عام:
	RTLIL::Module *module;
	RTLIL::Case_قاعدو پاڙ_case;
	std::ويڪٽر<RTLIL::Sync_قاعدو*> syncs;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	سانچو<typename T> فراغ rewrite_sigspecs(T &functor);
	سانچو<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ثابت;

	std::string منزل_rtlil_str() ثابت;
};

مضمن RTLIL::Sig_ٻٽو_انگ::Sig_ٻٽو_انگ() : wire(عدم), معلومات(RTLIL::State::S0) { }
مضمن RTLIL::Sig_ٻٽو_انگ::Sig_ٻٽو_انگ(RTLIL::State ٻٽو_انگ) : wire(عدم), معلومات(ٻٽو_انگ) { }
مضمن RTLIL::Sig_ٻٽو_انگ::Sig_ٻٽو_انگ(bool ٻٽو_انگ) : wire(عدم), معلومات(ٻٽو_انگ ? State::S1 : State::S0) { }
مضمن RTLIL::Sig_ٻٽو_انگ::Sig_ٻٽو_انگ(RTLIL::Wire *wire) : wire(wire), وٿي(0) { log_assert(wire && wire->ويڪر == 1); }
مضمن RTLIL::Sig_ٻٽو_انگ::Sig_ٻٽو_انگ(RTLIL::Wire *wire, صحيح وٿي) : wire(wire), وٿي(وٿي) { log_assert(wire != nullptr); }
مضمن RTLIL::Sig_ٻٽو_انگ::Sig_ٻٽو_انگ(ثابت RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.ويڪر == 1); إذا (wire) وٿي = chunk.وٿي; وإلا معلومات = chunk.معلومات[0]; }
مضمن RTLIL::Sig_ٻٽو_انگ::Sig_ٻٽو_انگ(ثابت RTLIL::SigChunk &chunk, صحيح اشاريو) : wire(chunk.wire) { إذا (wire) وٿي = chunk.وٿي + اشاريو; وإلا معلومات = chunk.معلومات[اشاريو]; }

مضمن bool RTLIL::Sig_ٻٽو_انگ::عامل<(ثابت RTLIL::Sig_ٻٽو_انگ &other) ثابت {
	إذا (wire == other.wire)
		أرجع wire ? (وٿي < other.وٿي) : (معلومات < other.معلومات);
	إذا (wire != nullptr && other.wire != nullptr)
		أرجع wire->name < other.wire->name;
	أرجع (wire != nullptr) < (other.wire != nullptr);
}

مضمن bool RTLIL::Sig_ٻٽو_انگ::عامل==(ثابت RTLIL::Sig_ٻٽو_انگ &other) ثابت {
	أرجع (wire == other.wire) && (wire ? (وٿي == other.وٿي) : (معلومات == other.معلومات));
}

مضمن bool RTLIL::Sig_ٻٽو_انگ::عامل!=(ثابت RTLIL::Sig_ٻٽو_انگ &other) ثابت {
	أرجع (wire != other.wire) || (wire ? (وٿي != other.وٿي) : (معلومات != other.معلومات));
}

مضمن Hasher RTLIL::Sig_ٻٽو_انگ::hash_into(Hasher h) ثابت {
	إذا (wire) {
		h.eat(وٿي);
		h.eat(wire->name);
		أرجع h;
	}
	h.eat(معلومات);
	أرجع h;
}


مضمن Hasher RTLIL::Sig_ٻٽو_انگ::hash_چوٽي() ثابت {
	Hasher h;
	إذا (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.بٽ();
		h.force(hashlib::legacy::djb2_جوڙيو(n, وٿي));
		أرجع h;
	}
	h.force(معلومات);
	أرجع h;
}

مضمن RTLIL::Sig_ٻٽو_انگ &RTLIL::SigSpecIterator::عامل*() ثابت {
	أرجع (*sig_p)[اشاريو];
}

مضمن ثابت RTLIL::Sig_ٻٽو_انگ &RTLIL::SigSpecConstIterator::عامل*() {
	ٻٽو_انگ = (*sig_p)[اشاريو];
	أرجع ٻٽو_انگ;
}

مضمن RTLIL::Sig_ٻٽو_انگ::Sig_ٻٽو_انگ(ثابت RTLIL::SigSpec &sig) {
	log_assert(sig.سائيز() == 1);
	خودڪار it = sig.chunks().begin();
	*this = Sig_ٻٽو_انگ(*it);
}

سانچو<typename T>
فراغ RTLIL::Module::rewrite_sigspecs(T &functor)
{
	لكل (خودڪار &it : cells_)
		it.ٻيو->rewrite_sigspecs(functor);
	لكل (خودڪار &it : processes)
		it.ٻيو->rewrite_sigspecs(functor);
	لكل (خودڪار &it : connections_) {
		functor(it.پهريون);
		functor(it.ٻيو);
	}
}

سانچو<typename T>
فراغ RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	لكل (خودڪار &it : cells_)
		it.ٻيو->rewrite_sigspecs2(functor);
	لكل (خودڪار &it : processes)
		it.ٻيو->rewrite_sigspecs2(functor);
	لكل (خودڪار &it : connections_) {
		functor(it.پهريون, it.ٻيو);
	}
}

سانچو<typename T>
فراغ RTLIL::Cell::rewrite_sigspecs(T &functor) {
	لكل (خودڪار &it : connections_)
		functor(it.ٻيو);
}

سانچو<typename T>
فراغ RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	لكل (خودڪار &it : connections_)
		functor(it.ٻيو);
}

سانچو<typename T>
فراغ RTLIL::Case_قاعدو::rewrite_sigspecs(T &functor) {
	لكل (خودڪار &it : compare)
		functor(it);
	لكل (خودڪار &it : actions) {
		functor(it.پهريون);
		functor(it.ٻيو);
	}
	لكل (خودڪار it : switches)
		it->rewrite_sigspecs(functor);
}

سانچو<typename T>
فراغ RTLIL::Case_قاعدو::rewrite_sigspecs2(T &functor) {
	لكل (خودڪار &it : compare)
		functor(it);
	لكل (خودڪار &it : actions) {
		functor(it.پهريون, it.ٻيو);
	}
	لكل (خودڪار it : switches)
		it->rewrite_sigspecs2(functor);
}

سانچو<typename T>
فراغ RTLIL::Switch_قاعدو::rewrite_sigspecs(T &functor)
{
	functor(إشارة);
	لكل (خودڪار it : cases)
		it->rewrite_sigspecs(functor);
}

سانچو<typename T>
فراغ RTLIL::Switch_قاعدو::rewrite_sigspecs2(T &functor)
{
	functor(إشارة);
	لكل (خودڪار it : cases)
		it->rewrite_sigspecs2(functor);
}

سانچو<typename T>
فراغ RTLIL::Sync_قاعدو::rewrite_sigspecs(T &functor)
{
	functor(إشارة);
	لكل (خودڪار &it : actions) {
		functor(it.پهريون);
		functor(it.ٻيو);
	}
	لكل (خودڪار &it : mem_write_actions) {
		functor(it.پتو);
		functor(it.معلومات);
		functor(it.enable);
	}
}

سانچو<typename T>
فراغ RTLIL::Sync_قاعدو::rewrite_sigspecs2(T &functor)
{
	functor(إشارة);
	لكل (خودڪار &it : actions) {
		functor(it.پهريون, it.ٻيو);
	}
	لكل (خودڪار &it : mem_write_actions) {
		functor(it.پتو);
		functor(it.معلومات);
		functor(it.enable);
	}
}

سانچو<typename T>
فراغ RTLIL::Process::rewrite_sigspecs(T &functor)
{
	پاڙ_case.rewrite_sigspecs(functor);
	لكل (خودڪار it : syncs)
		it->rewrite_sigspecs(functor);
}

سانچو<typename T>
فراغ RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	پاڙ_case.rewrite_sigspecs2(functor);
	لكل (خودڪار it : syncs)
		it->rewrite_sigspecs2(functor);
}

#ضمن "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
