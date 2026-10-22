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

#جے_بنا_تعریف RTLIL_H
#تعریف_کرو RTLIL_H

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/yosys.h"
#ضمن "kernel/twine.h"

#ضمن <deque>
#ضمن <string_view>
#ضمن <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	تعداد State : بنانشانی حرف {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	تعداد SyncType : بنانشانی حرف {
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
	تعداد ConstFlags : بنانشانی حرف {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	تعداد SelectPartials : بنانشانی حرف {
		SELECT_ALL = 0,          // include partial modules
		SELECT_سارا_ONLY = 1,   // ignore partial modules
		SELECT_سارا_WARN = 2,   // call log_warning on partial module
		SELECT_سارا_ERR = 3,    // call log_error on partial module
		SELECT_سارا_CMDERR = 4  // call log_cmd_error on partial module
	};

	تعداد SelectBoxes : بنانشانی حرف {
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

	تعداد پورٹ_Dir : بنانشانی حرف  {
		PD_UNKNOWN = 0,
		PD_داخلہ = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr صحيح چوڑائی_حد_بندی = 1 << 30;

	بنية Const;
	بنية AttrObject;
	بنية SelectedMember;
	بنية Selection;
	بنية Monitor;
	بنية Design;
	بنية Module;
	بنية Wire;
	بنية حافظہ;
	بنية Cell;
	بنية SigChunk;
	بنية Sig_ثنائی_ہندسہ;
	بنية SigSpecIterator;
	بنية SigSpecConstIterator;
	بنية SigSpec;
	بنية Case_قاعدہ;
	بنية Switch_قاعدہ;
	بنية MemWriteAction;
	بنية Sync_قاعدہ;
	بنية Process;

	تعريفنوع std::pair<SigSpec, SigSpec> SigSig;
};

بنية SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_اگیتر ID; }
namespace RTLIL { using YOSYS_NAMESPACE_اگیتر IdString; }

namespace RTLIL {
	فراغ copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			ثابت dict<IdString, RTLIL::Const> &src,
			ثابت RTLIL::Design *src_design, RTLIL::Design *dst_design);

	خارجي dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	ثابت pool<IdString> &builtin_ff_cell_types();

	ساكن مضمن std::string escape_id(ثابت std::string &str) {
		إذا (str.ناپ() > 0 && str[0] != '\\' && str[0] != '$')
			أرجع "\\" + str;
		أرجع str;
	}

	ساكن مضمن std::string unescape_id(ثابت std::string &str) {
		إذا (str.ناپ() < 2)
			أرجع str;
		إذا (str[0] != '\\')
			أرجع str;
		إذا (str[1] == '$' || str[1] == '\\')
			أرجع str;
		إذا (str[1] >= '0' && str[1] <= '9')
			أرجع str;
		أرجع str.substr(1);
	}

	سانچا <typename T> using ترتیب_دیو_by_name_id = IdString::compare_ptr_by_name<T>;

	سانچا <typename T> بنية ترتیب_دیو_by_name_str {
		bool عامل()(T *a, T *b) ثابت {
			أرجع a->name.lt_by_name(b->name);
		}
	};

	بنية ترتیب_دیو_by_id_str {
		ثابت TwinePool& pool;
		explicit ترتیب_دیو_by_id_str(ثابت TwinePool& pool) : pool(pool) {}
		bool عامل()(RTLIL::IdString a, RTLIL::IdString b) ثابت {
			أرجع pool.compare_by_name(a, b) < 0;
		}
	};

	ساكن مضمن std::string encode_filename(ثابت std::string &filename)
	{
		std::stringstream val;
		إذا (!std::any_دا(filename.begin(), filename.سرا(), [](حرف c) {
			أرجع static_cast<بنانشانی حرف>(c) < 33 || static_cast<بنانشانی حرف>(c) > 126;
		})) أرجع filename;
		لكل (بنانشانی حرف ثابت c : filename) {
			إذا (c < 33 || c > 126)
				val << stringf("$%02x", c);
			وإلا
				val << c;
		}
		أرجع val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_and         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_or          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_xor         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_xnor        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);

	RTLIL::Const const_reduce_and  (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_reduce_or   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_reduce_xor  (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_reduce_xnor (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_reduce_bool (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);

	RTLIL::Const const_logic_not   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_logic_and   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_logic_or    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);

	RTLIL::Const const_shl         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_shr         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_sshl        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_sshr        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_کھسکاؤ       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_shiftx      (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);

	RTLIL::Const const_lt          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_le          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_eq          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_ne          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_eqx         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_nex         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_ge          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_gt          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);

	RTLIL::Const const_جوڑو         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_sub         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_mul         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_div         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_divfloor    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_modfloor    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_mod         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_pow         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);

	RTLIL::Const const_pos         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_buf         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);
	RTLIL::Const const_neg         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len);

	RTLIL::Const const_mux         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	سانچا<typename T, typename کلید_2 = IdString>
	بنية ObjIterator {
		using iterator_category = std::forward_iterator_ٹیگ;
		using قدر_type = T;
		using difference_type = ptrdiff_t;
		using اشارہ_گر = T*;
		using reference = T&;
		typename dict<کلید_2, T>::iterator it;
		dict<کلید_2, T> *list_p;
		صحيح *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, صحيح *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			إذا (list_p->خالی_تھاواں()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} وإلا {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(ثابت RTLIL::ObjIterator<T, کلید_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			إذا (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &عامل=(ثابت RTLIL::ObjIterator<T, کلید_2> &other) {
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
			أرجع it->دوجا;
		}

		مضمن bool عامل!=(ثابت RTLIL::ObjIterator<T, کلید_2> &other) ثابت {
			إذا (list_p == nullptr || other.list_p == nullptr)
				أرجع list_p != other.list_p;
			أرجع it != other.it;
		}


		مضمن bool عامل==(ثابت RTLIL::ObjIterator<T, کلید_2> &other) ثابت {
			أرجع !(*this != other);
		}

		مضمن ObjIterator<T, کلید_2>& عامل++() {
			log_assert(list_p != nullptr);
			إذا (++it == list_p->سرا()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			أرجع *this;
		}

		مضمن ObjIterator<T, کلید_2>& عامل+=(صحيح amt) {
			log_assert(list_p != nullptr);
			it += amt;
			إذا (it == list_p->سرا()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			أرجع *this;
		}

		مضمن ObjIterator<T, کلید_2> عامل+(صحيح amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, کلید_2> new_obj(*this);
			new_obj.it += amt;
			إذا (new_obj.it == list_p->سرا()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			أرجع new_obj;
		}

		مضمن ثابت ObjIterator<T, کلید_2> عامل++(صحيح) {
			ObjIterator<T, کلید_2> نتیجہ(*this);
			++(*this);
			أرجع نتیجہ;
		}
	};

	سانچا<typename T, typename کلید_2 = IdString>
	بنية ObjRange
	{
		dict<کلید_2, T> *list_p;
		صحيح *refcount_p;

		ObjRange(decltype(list_p) list_p, صحيح *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, کلید_2> begin() { أرجع RTLIL::ObjIterator<T, کلید_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, کلید_2> سرا() { أرجع RTLIL::ObjIterator<T, کلید_2>(); }

		ناپ_t ناپ() ثابت {
			أرجع list_p->ناپ();
		}

		عامل pool<T>() ثابت {
			pool<T> نتیجہ;
			لكل (خودکار &it : *list_p)
				نتیجہ.داخل_کرو(it.دوجا);
			أرجع نتیجہ;
		}

		عامل std::سمتیہ<T>() ثابت {
			std::سمتیہ<T> نتیجہ;
			نتیجہ.reserve(list_p->ناپ());
			لكل (خودکار &it : *list_p)
				نتیجہ.ڈالو_پٹھ(it.دوجا);
			أرجع نتیجہ;
		}

		pool<T> منزل_pool() ثابت { أرجع *this; }
		std::سمتیہ<T> منزل_سمتیہ() ثابت { أرجع *this; }
	};
};

بنية RTLIL::Const
{
	چھوٹا صحيح flags;
خاص:
	یار class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::سمتیہ<RTLIL::State>;
	تعداد class backing_ٹیگ: bool { بٹ, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_ٹیگ ٹیگ;
	اتحاد {
		bitvectype بٹ_2;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_بٹ() ثابت { أرجع ٹیگ == backing_ٹیگ::بٹ; }
	bool is_str() ثابت { أرجع ٹیگ == backing_ٹیگ::string; }

	bitvectype* get_if_بٹ() { أرجع is_بٹ() ? &بٹ_2 : عدم; }
	std::string* get_if_str() { أرجع is_str() ? &str_ : عدم; }
	ثابت bitvectype* get_if_بٹ() ثابت { أرجع is_بٹ() ? &بٹ_2 : عدم; }
	ثابت std::string* get_if_str() ثابت { أرجع is_str() ? &str_ : عدم; }

	bitvectype& get_بٹ();
	std::string& get_str();
	ثابت bitvectype& get_بٹ() ثابت;
	ثابت std::string& get_str() ثابت;
	std::سمتیہ<RTLIL::State>& بٹ_اندرونی();
	فراغ bitvectorize_اندرونی();

عام:
	Const() : flags(RTLIL::CONST_FLAG_NONE), ٹیگ(backing_ٹیگ::بٹ), بٹ_2(std::سمتیہ<RTLIL::State>()) {}
	Const(std::string str);
	Const(لما لما val); // default width is 32
	Const(لما لما val, صحيح چوڑائی);
	Const(RTLIL::State ثنائی_ہندسہ, صحيح چوڑائی = 1);
	Const(std::سمتیہ<RTLIL::State> بٹ) : flags(RTLIL::CONST_FLAG_NONE), ٹیگ(backing_ٹیگ::بٹ), بٹ_2(std::move(بٹ)) {}
	Const(ثابت std::سمتیہ<bool> &بٹ);
	Const(ثابت RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &عامل =(ثابت RTLIL::Const &other);
	~Const();

	بنية Builder
	{
		Builder() {}
		Builder(صحيح expected_چوڑائی) { بٹ.reserve(expected_چوڑائی); }
		فراغ ڈالو_پٹھ(RTLIL::State b) { بٹ.ڈالو_پٹھ(b); }
		صحيح ناپ() ثابت { أرجع static_cast<صحيح>(بٹ.ناپ()); }
		Const build() { أرجع Const(std::move(بٹ)); }
	خاص:
		std::سمتیہ<RTLIL::State> بٹ;
	};

	bool عامل <(ثابت RTLIL::Const &other) ثابت;
	bool عامل ==(ثابت RTLIL::Const &other) ثابت;
	bool عامل !=(ثابت RTLIL::Const &other) ثابت;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::سمتیہ<RTLIL::State>& بٹ() { أرجع بٹ_اندرونی(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	فراغ bitvectorize() ثابت { const_cast<Const*>(this)->bitvectorize_اندرونی(); }

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

	فراغ ٹیگ_bare_integer_const(ثابت std::string &قدر);

	std::string as_string(ثابت حرف* any = "-") ثابت;
	ساكن Const from_string(ثابت std::string &str);
	std::سمتیہ<RTLIL::State> منزل_بٹ() ثابت;

	std::string decode_string() ثابت;
	صحيح ناپ() ثابت;
	bool خالی_تھاواں() ثابت;

	فراغ append(ثابت RTLIL::Const &other);
	فراغ سیٹ(صحيح i, RTLIL::State state) {
		بٹ_اندرونی()[i] = state;
	}
	فراغ resize(صحيح ناپ, RTLIL::State fill) {
    log_assert(ناپ >= 0 && ناپ < RTLIL::چوڑائی_حد_بندی);
		بٹ_اندرونی().resize(ناپ, fill);
	}

	class const_iterator {
	خاص:
		ثابت Const* ماپا;
		ناپ_t idx;

	عام:
		using iterator_category = std::bidirectional_iterator_ٹیگ;
		using قدر_type = State;
		using difference_type = std::ptrdiff_t;
		using اشارہ_گر = ثابت State*;
		using reference = ثابت State&;

		const_iterator(ثابت Const& c, ناپ_t i) : ماپا(&c), idx(i) {}

		State عامل*() ثابت;

		const_iterator& عامل++() { ++idx; أرجع *this; }
		const_iterator& عامل--() { --idx; أرجع *this; }
		const_iterator عامل++(صحيح) { const_iterator نتیجہ(*this); ++idx; أرجع نتیجہ; }
		const_iterator عامل--(صحيح) { const_iterator نتیجہ(*this); --idx; أرجع نتیجہ; }
		const_iterator& عامل+=(صحيح i) { idx += i; أرجع *this; }

		const_iterator عامل+(صحيح جوڑو) {
			أرجع const_iterator(*ماپا, idx + جوڑو);
		}
		const_iterator عامل-(صحيح sub) {
			أرجع const_iterator(*ماپا, idx - sub);
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
		Const* ماپا;
		ناپ_t idx;

	عام:
		class proxy {
		خاص:
			Const* ماپا;
			ناپ_t idx;
		عام:
			proxy(Const* ماپا, ناپ_t idx) : ماپا(ماپا), idx(idx) {}
			عامل State() ثابت { أرجع (*ماپا)[idx]; }
			proxy& عامل=(State s) { ماپا->سیٹ(idx, s); أرجع *this; }
			proxy& عامل=(ثابت proxy& other) { ماپا->سیٹ(idx, (*other.ماپا)[other.idx]); أرجع *this; }
		};

		using iterator_category = std::bidirectional_iterator_ٹیگ;
		using قدر_type = State;
		using difference_type = std::ptrdiff_t;
		using اشارہ_گر = proxy*;
		using reference = proxy;

		iterator(Const& c, ناپ_t i) : ماپا(&c), idx(i) {}

		proxy عامل*() ثابت { أرجع proxy(ماپا, idx); }
		iterator& عامل++() { ++idx; أرجع *this; }
		iterator& عامل--() { --idx; أرجع *this; }
		iterator عامل++(صحيح) { iterator نتیجہ(*this); ++idx; أرجع نتیجہ; }
		iterator عامل--(صحيح) { iterator نتیجہ(*this); --idx; أرجع نتیجہ; }
		iterator& عامل+=(صحيح i) { idx += i; أرجع *this; }

		iterator عامل+(صحيح جوڑو) {
			أرجع iterator(*ماپا, idx + جوڑو);
		}
		iterator عامل-(صحيح sub) {
			أرجع iterator(*ماپا, idx - sub);
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
	const_iterator سرا() ثابت {
		أرجع const_iterator(*this, ناپ());
	}
	iterator begin() {
		أرجع iterator(*this, 0);
	}
	iterator سرا() {
		أرجع iterator(*this, ناپ());
	}
	State پٹھ() ثابت {
		أرجع *(سرا() - 1);
	}
	State front() ثابت {
		أرجع *begin();
	}
	State at(ناپ_t i) ثابت {
		أرجع *const_iterator(*this, i);
	}
	State عامل[](ناپ_t i) ثابت {
		أرجع *const_iterator(*this, i);
	}

	bool is_fully_zero() ثابت;
	bool is_fully_ones() ثابت;
	bool is_fully_def() ثابت;
	bool is_fully_undef() ثابت;
	bool is_fully_undef_x_only() ثابت;
	bool is_onehot(صحيح *pos = nullptr) ثابت;

	RTLIL::Const کڈھو(صحيح وتھ, صحيح len = 1, RTLIL::State padding = RTLIL::State::S0) ثابت;

	// find the MSB without redundant leading bits
	صحيح get_min_ناپ(bool is_signed) ثابت;

	// compress representation to the minimum required bits
	فراغ compress(bool is_signed = false);

	std::optional<صحيح> as_int_compress(bool is_signed) ثابت;

	فراغ extu(صحيح چوڑائی) {
		resize(چوڑائی, RTLIL::State::S0);
	}

	فراغ exts(صحيح چوڑائی) {
		resize(چوڑائی, خالی_تھاواں() ? RTLIL::State::Sx : پٹھ());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> گن;

	bool has_attribute(RTLIL::IdString id) ثابت;

	فراغ سیٹ_bool_attribute(RTLIL::IdString id, bool قدر=true);
	bool get_bool_attribute(RTLIL::IdString id) ثابت;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ثابت {
		أرجع get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	فراغ سیٹ_string_attribute(RTLIL::IdString id, string قدر);
	string get_string_attribute(RTLIL::IdString id) ثابت;

	فراغ سیٹ_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &انکڑے);
	فراغ جوڑو_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &انکڑے);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ثابت;

	فراغ سیٹ_src_attribute(ثابت std::string &src) {
		سیٹ_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ثابت {
		أرجع get_string_attribute(ID::src);
	}

	فراغ سیٹ_hdlname_attribute(ثابت سمتیہ<string> &hierarchy);
	سمتیہ<string> get_hdlname_attribute() ثابت;

	فراغ سیٹ_intvec_attribute(RTLIL::IdString id, ثابت سمتیہ<صحيح> &انکڑے);
	سمتیہ<صحيح> get_intvec_attribute(RTLIL::IdString id) ثابت;
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
	std::سمتیہ<RTLIL::State> انکڑے; // only used if wire == NULL, LSB at index 0
	صحيح چوڑائی, وتھ;

	SigChunk() : wire(nullptr), چوڑائی(0), وتھ(0) {}
	SigChunk(ثابت RTLIL::Const &قدر) : wire(nullptr), انکڑے(قدر.منزل_بٹ()), چوڑائی(Get_ناپ(انکڑے)), وتھ(0) {}
	SigChunk(RTLIL::Const &&قدر) : wire(nullptr), انکڑے(قدر.منزل_بٹ()), چوڑائی(Get_ناپ(انکڑے)), وتھ(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), چوڑائی(Get_ناپ(wire)), وتھ(0) {}
	SigChunk(RTLIL::Wire *wire, صحيح وتھ, صحيح چوڑائی = 1) : wire(wire), چوڑائی(چوڑائی), وتھ(وتھ) {}
	SigChunk(ثابت std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(صحيح val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(صحيح val, صحيح چوڑائی) : SigChunk(RTLIL::Const(val, چوڑائی)) {}
	SigChunk(RTLIL::State ثنائی_ہندسہ, صحيح چوڑائی = 1) : SigChunk(RTLIL::Const(ثنائی_ہندسہ, چوڑائی)) {}
	SigChunk(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ);

	RTLIL::SigChunk کڈھو(صحيح وتھ, صحيح لمائی) ثابت;
	RTLIL::Sig_ثنائی_ہندسہ عامل[](صحيح وتھ) ثابت;
	مضمن صحيح ناپ() ثابت { أرجع چوڑائی; }
	مضمن bool is_wire() ثابت { أرجع wire != عدم; }

	bool عامل <(ثابت RTLIL::SigChunk &other) ثابت;
	bool عامل ==(ثابت RTLIL::SigChunk &other) ثابت;
	bool عامل !=(ثابت RTLIL::SigChunk &other) ثابت;
};

بنية RTLIL::Sig_ثنائی_ہندسہ
{
	RTLIL::Wire *wire;
	اتحاد {
		RTLIL::State انکڑے; // used if wire == NULL
		صحيح وتھ;        // used if wire != NULL
	};

	Sig_ثنائی_ہندسہ();
	Sig_ثنائی_ہندسہ(RTLIL::State ثنائی_ہندسہ);
	explicit Sig_ثنائی_ہندسہ(bool ثنائی_ہندسہ);
	Sig_ثنائی_ہندسہ(RTLIL::Wire *wire);
	Sig_ثنائی_ہندسہ(RTLIL::Wire *wire, صحيح وتھ);
	Sig_ثنائی_ہندسہ(ثابت RTLIL::SigChunk &chunk);
	Sig_ثنائی_ہندسہ(ثابت RTLIL::SigChunk &chunk, صحيح اشاریہ);
	Sig_ثنائی_ہندسہ(ثابت RTLIL::SigSpec &sig);
	Sig_ثنائی_ہندسہ(ثابت RTLIL::Sig_ثنائی_ہندسہ &sigbit) = افتراضي;
	RTLIL::Sig_ثنائی_ہندسہ &عامل =(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) = افتراضي;

	مضمن bool is_wire() ثابت { أرجع wire != عدم; }

	bool عامل <(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت;
	bool عامل ==(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت;
	bool عامل !=(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
	[[nodiscard]] Hasher hash_چوٹی() ثابت;
};

namespace hashlib {
	سانچا <>
	بنية hash_ops<RTLIL::Sig_ثنائی_ہندسہ> {
		ساكن مضمن bool cmp(ثابت RTLIL::Sig_ثنائی_ہندسہ &a, ثابت RTLIL::Sig_ثنائی_ہندسہ &b) {
			أرجع a == b;
		}
		[[nodiscard]] ساكن مضمن Hasher hash(ثابت RTLIL::Sig_ثنائی_ہندسہ sb) {
			أرجع sb.hash_چوٹی();
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت RTLIL::Sig_ثنائی_ہندسہ sb, Hasher h) {
			أرجع sb.hash_into(h);
		}
	};
};

بنية RTLIL::SigSpecIterator
{
	تعريفنوع std::داخلہ_iterator_ٹیگ iterator_category;
	تعريفنوع RTLIL::Sig_ثنائی_ہندسہ قدر_type;
	تعريفنوع ptrdiff_t difference_type;
	تعريفنوع RTLIL::Sig_ثنائی_ہندسہ* اشارہ_گر;
	تعريفنوع RTLIL::Sig_ثنائی_ہندسہ& reference;

	RTLIL::SigSpec *sig_p;
	صحيح اشاریہ;

	مضمن RTLIL::Sig_ثنائی_ہندسہ &عامل*() ثابت;
	مضمن bool عامل!=(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع اشاریہ != other.اشاریہ; }
	مضمن bool عامل==(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع اشاریہ == other.اشاریہ; }
	مضمن فراغ عامل++() { اشاریہ++; }
};

بنية RTLIL::SigSpecConstIterator
{
	تعريفنوع std::داخلہ_iterator_ٹیگ iterator_category;
	تعريفنوع RTLIL::Sig_ثنائی_ہندسہ قدر_type;
	تعريفنوع ptrdiff_t difference_type;
	تعريفنوع RTLIL::Sig_ثنائی_ہندسہ* اشارہ_گر;
	تعريفنوع RTLIL::Sig_ثنائی_ہندسہ& reference;

	ثابت RTLIL::SigSpec *sig_p;
	RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ;
	صحيح اشاریہ;

	مضمن ثابت RTLIL::Sig_ثنائی_ہندسہ &عامل*();
	مضمن bool عامل!=(ثابت RTLIL::SigSpecConstIterator &other) ثابت { أرجع اشاریہ != other.اشاریہ; }
	مضمن bool عامل==(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع اشاریہ == other.اشاریہ; }
	مضمن فراغ عامل++() { اشاریہ++; }
};

بنية RTLIL::SigSpec
{
خاص:
	یار class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, کڈھو_2);
	تعداد Representation : حرف {
		CHUNK,
		بٹ_3,
	};
	// An AtomicHash is either clear or a nonzero integer.
	بنية AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ثابت AtomicHash &rhs) : atomic_(rhs.لدو()) {}
		AtomicHash &عامل=(ثابت AtomicHash &rhs) { store(rhs.لدو()); أرجع *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> اقرأ() ثابت {
			Hasher::hash_t قدر = لدو();
			إذا (قدر == 0)
				أرجع std::nullopt;
			أرجع قدر;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		فراغ سیٹ(Hasher::hash_t قدر) ثابت {
			log_assert(قدر != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(قدر, std::حافظہ_ترتیب_relaxed);
			log_assert(old == 0 || old == قدر);
		}
		فراغ clear() { store(0); }
	خاص:
		صحيح لدو() ثابت { أرجع atomic_.لدو(std::حافظہ_ترتیب_relaxed); }
		فراغ store(Hasher::hash_t قدر) ثابت {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(قدر, std::حافظہ_ترتیب_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	اتحاد {
		RTLIL::SigChunk chunk_;
		std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> بٹ_2; // LSB at index 0
	};

	فراغ init_خالی_تھاواں_بٹ() {
		rep_ = بٹ_3;
		new (&بٹ_2) std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>;
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
			بٹ_2.~سمتیہ();
	}
	یار بنية Chunks;

عام:
	SigSpec() { init_خالی_تھاواں_بٹ(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ثابت SigSpec &قدر) : rep_(قدر.rep_), hash_(قدر.hash_) {
		إذا (قدر.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(قدر.chunk_);
		وإلا
			new (&بٹ_2) std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>(قدر.بٹ_2);
	}
	SigSpec(SigSpec &&قدر) : rep_(قدر.rep_), hash_(قدر.hash_) {
		إذا (قدر.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(قدر.chunk_));
		وإلا
			new (&بٹ_2) std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>(std::move(قدر.بٹ_2));
	}
	SigSpec(ثابت RTLIL::Const &قدر);
	SigSpec(RTLIL::Const &&قدر);
	SigSpec(ثابت RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, صحيح وتھ, صحيح چوڑائی = 1);
	SigSpec(ثابت std::string &str);
	SigSpec(صحيح val, صحيح چوڑائی = 32);
	SigSpec(RTLIL::State ثنائی_ہندسہ, صحيح چوڑائی = 1);
	SigSpec(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ, صحيح چوڑائی = 1);
	SigSpec(ثابت std::سمتیہ<RTLIL::SigChunk> &chunks);
	SigSpec(ثابت std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> &بٹ);
	SigSpec(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &بٹ);
	SigSpec(ثابت std::سیٹ<RTLIL::Sig_ثنائی_ہندسہ> &بٹ);
	explicit SigSpec(bool ثنائی_ہندسہ);
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
			new (&بٹ_2) std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>(rhs.بٹ_2);
		أرجع *this;
	}
	SigSpec &عامل=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		إذا (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		وإلا
			new (&بٹ_2) std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>(std::move(rhs.بٹ_2));
		أرجع *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	بنية Chunks {
		Chunks(ثابت SigSpec &spec) : spec(spec) {}
		بنية const_iterator {
			using iterator_category = std::forward_iterator_ٹیگ;
			using قدر_type = ثابت SigChunk &;
			using difference_type = std::ptrdiff_t;
			using اشارہ_گر = ثابت SigChunk *;
			using reference = ثابت SigChunk &;

			ثابت SigSpec &spec;
			صحيح ثنائی_ہندسہ_اشاریہ;
			SigChunk chunk;

			const_iterator(ثابت SigSpec &spec) : spec(spec) {
				ثنائی_ہندسہ_اشاریہ = 0;
				إذا (spec.rep_ == بٹ_3)
					اگلا_chunk_بٹ();
			}
			تعداد سرا_2 { سرا_3 };
			const_iterator(ثابت SigSpec &spec, سرا_2) : spec(spec) {
				ثنائی_ہندسہ_اشاریہ = spec.ناپ();
			}
			فراغ اگلا_chunk_بٹ();

			ثابت SigChunk &عامل*() {
				إذا (spec.rep_ == CHUNK)
					أرجع spec.chunk_;
				أرجع chunk;
			};
			ثابت SigChunk *عامل->() { أرجع &**this; }
			const_iterator &عامل++() {
				ثنائی_ہندسہ_اشاریہ += (**this).چوڑائی;
				إذا (spec.rep_ == بٹ_3)
					اگلا_chunk_بٹ();
				أرجع *this;
			}
			bool عامل==(ثابت const_iterator &rhs) ثابت { أرجع ثنائی_ہندسہ_اشاریہ == rhs.ثنائی_ہندسہ_اشاریہ; }
			bool عامل!=(ثابت const_iterator &rhs) ثابت { أرجع !(*this == rhs); }
		};
		const_iterator begin() ثابت { أرجع const_iterator(spec); }
		const_iterator سرا() ثابت {
			const_iterator it(spec, const_iterator::سرا_3);
			أرجع it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::سمتیہ<RTLIL::SigChunk>::const_الٹ_iterator rbegin() {
			ensure_chunk_سمتیہ();
			أرجع chunk_سمتیہ.rbegin();
		}
		std::سمتیہ<RTLIL::SigChunk>::const_الٹ_iterator rend() {
			ensure_chunk_سمتیہ();
			أرجع chunk_سمتیہ.rend();
		}
		صحيح ناپ() {
			ensure_chunk_سمتیہ();
			أرجع chunk_سمتیہ.ناپ();
		}
		صحيح ناپ() ثابت {
			أرجع std::فاصلہ(begin(), سرا());
		}
		ثابت SigChunk &at(صحيح اشاریہ) {
			ensure_chunk_سمتیہ();
			أرجع chunk_سمتیہ.at(اشاریہ);
		}
		عامل ثابت std::سمتیہ<RTLIL::SigChunk>&() {
			ensure_chunk_سمتیہ();
			أرجع chunk_سمتیہ;
		}
	خاص:
		فراغ ensure_chunk_سمتیہ() {
			إذا (spec.ناپ() > 0 && chunk_سمتیہ.خالی_تھاواں()) {
				لكل (ثابت RTLIL::SigChunk &c : *this)
					chunk_سمتیہ.ڈالو_پٹھ(c);
			}
		}
		ثابت SigSpec &spec;
		std::سمتیہ<RTLIL::SigChunk> chunk_سمتیہ;
	};
	یار بنية Chunks::const_iterator;

	مضمن Chunks chunks() ثابت { أرجع {*this}; }
	مضمن ثابت SigSpec &بٹ() ثابت { أرجع *this; }

	مضمن صحيح ناپ() ثابت { أرجع rep_ == CHUNK ? chunk_.چوڑائی : Get_ناپ(بٹ_2); }
	مضمن bool خالی_تھاواں() ثابت { أرجع ناپ() == 0; };

	مضمن RTLIL::Sig_ثنائی_ہندسہ &عامل[](صحيح اشاریہ) { inline_unpack(); hash_.clear(); أرجع بٹ_2.at(اشاریہ); }
	مضمن RTLIL::Sig_ثنائی_ہندسہ عامل[](صحيح اشاریہ) ثابت {
		إذا (rep_ == CHUNK) {
			إذا (اشاریہ < 0 || اشاریہ >= chunk_.چوڑائی)
				throw std::باہرلا_دا_range("SigSpec::operator[]");
			إذا (chunk_.wire)
				أرجع RTLIL::Sig_ثنائی_ہندسہ(chunk_.wire, chunk_.وتھ + اشاریہ);
			أرجع RTLIL::Sig_ثنائی_ہندسہ(chunk_.انکڑے[اشاریہ]);
		}
		أرجع بٹ_2.at(اشاریہ);
	}

	مضمن RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.اشاریہ = 0; أرجع it; }
	مضمن RTLIL::SigSpecIterator سرا() { RTLIL::SigSpecIterator it; it.sig_p = this; it.اشاریہ = ناپ(); أرجع it; }

	مضمن RTLIL::SigSpecConstIterator begin() ثابت { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.اشاریہ = 0; أرجع it; }
	مضمن RTLIL::SigSpecConstIterator سرا() ثابت { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.اشاریہ = ناپ(); أرجع it; }

	فراغ ترتیب_دیو();
	فراغ ترتیب_دیو_and_unify();

	فراغ replace(ثابت RTLIL::SigSpec &نمونہ, ثابت RTLIL::SigSpec &with);
	فراغ replace(ثابت RTLIL::SigSpec &نمونہ, ثابت RTLIL::SigSpec &with, RTLIL::SigSpec *other) ثابت;

	فراغ replace(ثابت dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قاعدے);
	فراغ replace(ثابت dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قاعدے, RTLIL::SigSpec *other) ثابت;

	فراغ replace(ثابت std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قاعدے);
	فراغ replace(ثابت std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قاعدے, RTLIL::SigSpec *other) ثابت;

	فراغ replace(صحيح وتھ, ثابت RTLIL::SigSpec &with);

	فراغ remove(ثابت RTLIL::SigSpec &نمونہ);
	فراغ remove(ثابت RTLIL::SigSpec &نمونہ, RTLIL::SigSpec *other) ثابت;
	فراغ remove2(ثابت RTLIL::SigSpec &نمونہ, RTLIL::SigSpec *other);

	فراغ remove(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ);
	فراغ remove(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, RTLIL::SigSpec *other) ثابت;
	فراغ remove2(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, RTLIL::SigSpec *other);
	فراغ remove2(ثابت std::سیٹ<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, RTLIL::SigSpec *other);
	فراغ remove2(ثابت pool<RTLIL::Wire*> &نمونہ, RTLIL::SigSpec *other);

	فراغ remove(صحيح وتھ, صحيح لمائی = 1);
	فراغ remove_const();

	RTLIL::SigSpec کڈھو(ثابت RTLIL::SigSpec &نمونہ, ثابت RTLIL::SigSpec *other = عدم) ثابت;
	RTLIL::SigSpec کڈھو(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, ثابت RTLIL::SigSpec *other = عدم) ثابت;
	RTLIL::SigSpec کڈھو(صحيح وتھ, صحيح لمائی = 1) ثابت;
	RTLIL::SigSpec کڈھو_سرا(صحيح وتھ) ثابت { أرجع کڈھو(وتھ, ناپ() - وتھ); }

	فراغ rewrite_wires(std::function<فراغ(RTLIL::Wire*& wire)> rewrite);

	RTLIL::Sig_ثنائی_ہندسہ lsb() ثابت { log_assert(ناپ()); أرجع (*this)[0]; };
	RTLIL::Sig_ثنائی_ہندسہ msb() ثابت { log_assert(ناپ()); أرجع (*this)[ناپ() - 1]; };
	RTLIL::Sig_ثنائی_ہندسہ front() ثابت { أرجع (*this)[0]; }
	RTLIL::Sig_ثنائی_ہندسہ پٹھ() ثابت { أرجع (*this)[ناپ() - 1]; }

	فراغ append(ثابت RTLIL::SigSpec &إشارة);
	مضمن فراغ append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	مضمن فراغ append(ثابت RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	مضمن فراغ append(ثابت RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	فراغ append(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ);
	مضمن فراغ append(RTLIL::State state) { append(RTLIL::Sig_ثنائی_ہندسہ(state)); }
	مضمن فراغ append(bool bool_) { append(RTLIL::Sig_ثنائی_ہندسہ(bool_)); }

	فراغ extend_u0(صحيح چوڑائی, bool is_signed = false);

	RTLIL::SigSpec repeat(صحيح عدد) ثابت;

	فراغ الٹ() { inline_unpack(); std::الٹ(بٹ_2.begin(), بٹ_2.سرا()); }

	bool عامل <(ثابت RTLIL::SigSpec &other) ثابت;
	bool عامل ==(ثابت RTLIL::SigSpec &other) ثابت;
	مضمن bool عامل !=(ثابت RTLIL::SigSpec &other) ثابت { أرجع !(*this == other); }

	bool is_wire() ثابت;
	bool is_chunk() ثابت;
	مضمن bool is_ثنائی_ہندسہ() ثابت { أرجع ناپ() == 1; }

	bool معلوم_driver() ثابت;

	bool is_fully_const() ثابت;
	bool is_fully_zero() ثابت;
	bool is_fully_ones() ثابت;
	bool is_fully_def() ثابت;
	bool is_fully_undef() ثابت;
	bool has_const() ثابت;
	bool has_const(State state) ثابت;
	bool has_marked_بٹ() ثابت;
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
	RTLIL::Sig_ثنائی_ہندسہ as_ثنائی_ہندسہ() ثابت;

	bool میل(ثابت حرف* نمونہ) ثابت;

	std::سیٹ<RTLIL::Sig_ثنائی_ہندسہ> منزل_sigbit_سیٹ() ثابت;
	pool<RTLIL::Sig_ثنائی_ہندسہ> منزل_sigbit_pool() ثابت;
	std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> منزل_sigbit_سمتیہ() ثابت;
	std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> منزل_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت;
	dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> منزل_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت;

	ساكن bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	ساكن bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	ساكن bool parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	عامل std::سمتیہ<RTLIL::SigChunk>() ثابت;
	عامل std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>() ثابت { أرجع منزل_sigbit_سمتیہ(); }
	ثابت RTLIL::Sig_ثنائی_ہندسہ &at(صحيح وتھ, ثابت RTLIL::Sig_ثنائی_ہندسہ &defval) { أرجع وتھ < ناپ() ? (*this)[وتھ] : defval; }
	RTLIL::Sig_ثنائی_ہندسہ& at(صحيح وتھ) { أرجع (*this)[وتھ]; }
	RTLIL::Sig_ثنائی_ہندسہ at(صحيح وتھ) ثابت { أرجع (*this)[وتھ]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		Hasher::hash_t val;
		إذا (std::optional<Hasher::hash_t> current = hash_.اقرأ())
			val = *current;
		وإلا
			val = updhash();
		h.eat(val);
		أرجع h;
	}

#جے_بنا_تعریف NDEBUG
	فراغ check(ثابت Module *mod = nullptr) ثابت;
#وإلا
	فراغ check(ثابت Module *mod = nullptr) ثابت { (فراغ)mod; }
#جے_انت
};

بنية RTLIL::Selection
{
	// selection includes boxed modules
	bool selects_boxes;
	// selection covers full design, including boxed modules
	bool complete_selection;
	// selection covers full design, not including boxed modules
	bool بھریا_selection;
	pool<RTLIL::IdString> selected_modules;
	dict<RTLIL::IdString, pool<RTLIL::IdString>> selected_members;
	RTLIL::Design *current_design;

	// create a new selection
	Selection(
		// should the selection cover the full design
		bool بھریا_2 = true,
		// should the selection include boxed modules
		bool boxes = false,
		// the design to select from
		RTLIL::Design *design = nullptr
	) :
		selects_boxes(boxes), complete_selection(بھریا_2 && boxes), بھریا_selection(بھریا_2 && !boxes), current_design(design) { }

	// checks if the given module exists in the current design and is a
	// boxed module, warning the user if the current design is not set
	bool boxed_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_سارا_module(RTLIL::IdString mod_name) ثابت;

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
		أرجع بھریا_selection || complete_selection;
	}

	// add whole module to this selection
	سانچا<typename T1> فراغ select(T1 *module) {
		إذا (!selects_all() && selected_modules.گنتی(module->name) == 0) {
			IdString name = module->name;
			selected_modules.داخل_کرو(name);
			selected_members.erase(name);
			إذا (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	سانچا<typename T1, typename T2> فراغ select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	سانچا<typename T1> فراغ select(T1 *module, RTLIL::IdString member) {
		إذا (!selects_all() && selected_modules.گنتی(module->name.ref()) == 0) {
			selected_members[module->name.ref()].داخل_کرو(member);
			إذا (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool خالی_تھاواں() ثابت {
		أرجع !selects_all() && selected_modules.خالی_تھاواں() && selected_members.خالی_تھاواں();
	}

	// clear this selection, leaving it empty
	فراغ clear();

	// create a new selection which is empty
	ساكن Selection خالی_تھاواں_Selection(RTLIL::Design *design = nullptr) { أرجع Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	ساكن Selection بھریا_Selection(RTLIL::Design *design = nullptr) { أرجع Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	ساكن Selection CompleteSelection(RTLIL::Design *design = nullptr) { أرجع Selection(true, true, design); };
};

بنية RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

	Monitor() {
		ساكن بنانشانی صحيح hashidx_گنتی = 123456789;
		hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
		hashidx_ = hashidx_گنتی;
	}

	مجازی ~Monitor() { }
	مجازی فراغ notify_module_جوڑو(RTLIL::Module*) { }
	مجازی فراغ notify_module_del(RTLIL::Module*) { }
	مجازی فراغ notify_connect(RTLIL::Cell*, RTLIL::IdString, ثابت RTLIL::SigSpec&, ثابت RTLIL::SigSpec&) { }
	مجازی فراغ notify_connect(RTLIL::Module*, ثابت RTLIL::SigSig&) { }
	مجازی فراغ notify_connect(RTLIL::Module*, ثابت std::سمتیہ<RTLIL::SigSig>&) { }
	مجازی فراغ notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
بنية define_map_t;

سانچا<typename N>
مضمن constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::پتی> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ثابت حرف*> || std::is_same_v<std::decay_t<N>, حرف*>;

#تعریف_کرو YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, صحيح> = 0

سانچا<typename N>
مضمن constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ثابت حرف *> || std::is_same_v<std::decay_t<N>, حرف *>;

#تعریف_کرو YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, صحيح> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#تعریف_کرو YS_NAME_FWD_POOL(_func, _pool) \
	سانچا<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(خودکار) _func(N name, Rest&&... rest) \
		{ أرجع _func(_pool.جوڑو(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#تعریف_کرو YS_NAME_FWD_2ND_POOL(_func, _pool) \
	سانچا<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(خودکار) _func(T &&پہلا, N name) \
		{ أرجع _func(std::forward<T>(پہلا), _pool.جوڑو(std::move(name))); }

#تعریف_کرو YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#تعریف_کرو YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

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

	ناپ_t gc_twines();

	std::سمتیہ<std::unique_ptr<AST::Ast_گرہ>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_مقرر_کردا;

	std::سمتیہ<RTLIL::Selection> selection_ڈھیر;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_سرگرم_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ثابت RTLIL::Module *module(RTLIL::IdString name) ثابت;
	سانچا<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ أرجع module(twines().لبھو(std::string_view(name))); }
	سانچا<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Module *module(N name) ثابت
		{ أرجع module(twines().لبھو(std::string_view(name))); }
	RTLIL::Module *چوٹی_module() ثابت;

	bool has(RTLIL::IdString id) ثابت {
		twines_.check_owned(id);
		أرجع modules_.گنتی(id) != 0;
	}
	سانچا<typename N, YS_NAME_STRING(N)> bool has(N name) ثابت
		{ أرجع has(twines().لبھو(std::string_view(name))); }

	فراغ جوڑو(RTLIL::Module *module);

	RTLIL::Module *جوڑو_Module(RTLIL::IdString name);
	YS_NAME_FWD_POOL(جوڑو_Module, twines())
	فراغ remove(RTLIL::Module *module);
	فراغ rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	فراغ scratchpad_unset(ثابت std::string &varname);

	فراغ scratchpad_سیٹ_int(ثابت std::string &varname, صحيح قدر);
	فراغ scratchpad_سیٹ_bool(ثابت std::string &varname, bool قدر);
	فراغ scratchpad_سیٹ_string(ثابت std::string &varname, std::string قدر);

	صحيح scratchpad_get_int(ثابت std::string &varname, صحيح default_قدر_2 = 0) ثابت;
	bool scratchpad_get_bool(ثابت std::string &varname, bool default_قدر_2 = false) ثابت;
	std::string scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_قدر_2 = std::string()) ثابت;

	فراغ ترتیب_دیو();
	فراغ ترتیب_دیو_modules();
	فراغ check();
	فراغ optimize();

	فراغ clone_into(RTLIL::Design *dst) ثابت;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_سارا_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ثابت;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_سارا_module(RTLIL::Module *mod) ثابت;

	// push the given selection to the selection stack
	فراغ ڈالو_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	فراغ ڈالو_خالی_تھاواں_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	فراغ ڈالو_بھریا_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	فراغ ڈالو_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	فراغ نکالو_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		أرجع selection_ڈھیر.پٹھ();
	}

	// get the current selection
	ثابت RTLIL::Selection &selection() ثابت {
		أرجع selection_ڈھیر.پٹھ();
	}

	// is the current selection a full selection (no boxes)
	bool بھریا_selection() ثابت {
		أرجع selection().بھریا_selection;
	}

	// is the given module in the current selection
	سانچا<typename T1> bool selected(T1 *module) ثابت {
		أرجع selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	سانچا<typename T1, typename T2> bool selected(T1 *module, T2 *member) ثابت {
		أرجع selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	سانچا<typename T1> فراغ select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	سانچا<typename T1, typename T2> فراغ select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::سمتیہ<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) ثابت;

	// returns all selected modules, and may include boxes
	std::سمتیہ<RTLIL::Module*> all_selected_modules() ثابت { أرجع selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::سمتیہ<RTLIL::Module*> selected_unboxed_modules() ثابت { أرجع selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::سمتیہ<RTLIL::Module*> selected_unboxed_modules_warn() ثابت { أرجع selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::سمتیہ<RTLIL::Module*> selected_سارا_modules() ثابت { أرجع selected_modules(SELECT_سارا_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::سمتیہ<RTLIL::Module*> all_selected_سارا_modules() ثابت { أرجع selected_modules(SELECT_سارا_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::سمتیہ<RTLIL::Module*> selected_سارا_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ثابت { أرجع selected_modules(SELECT_سارا_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::سمتیہ<RTLIL::Module*> selected_unboxed_سارا_modules() ثابت { أرجع selected_modules(SELECT_سارا_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::سمتیہ<RTLIL::Module*> selected_unboxed_سارا_modules_warn() ثابت { أرجع selected_modules(SELECT_سارا_WARN, SB_UNBOXED_WARN); }

	ساكن std::map<بنانشانی صحيح, RTLIL::Design*> *get_all_designs(فراغ);

	std::string منزل_rtlil_str(bool only_selected = true) ثابت;
};

بنية RTLIL::Module : عام RTLIL::AttrObject
{
	یار بنية RTLIL::Cell;
	یار بنية RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	فراغ جوڑو(RTLIL::Wire *wire);
	فراغ جوڑو(RTLIL::Cell *cell);
	فراغ جوڑو(RTLIL::Process *process);

عام:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ثابت { أرجع design->twines(); }

	صحيح refcount_wires_;
	صحيح refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::سمتیہ<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_اقدار;
	dict<RTLIL::IdString, RTLIL::حافظہ*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	مجازی ~Module();
	مجازی RTLIL::IdString derive(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	مجازی RTLIL::IdString derive(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Const> &parameters, ثابت dict<RTLIL::IdString, RTLIL::Module*> &interfaces, ثابت dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	مجازی ناپ_t گنتی_id(RTLIL::IdString id);
	مجازی فراغ expand_interfaces(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	مجازی bool reprocess_if_necessary(RTLIL::Design *design);

	مجازی فراغ ترتیب_دیو();
	مجازی فراغ check();
	مجازی فراغ optimize();
	مجازی فراغ makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ثابت {
		أرجع get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	فراغ connect(ثابت RTLIL::SigSig &conn);
	فراغ connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs);
	فراغ new_connections(ثابت std::سمتیہ<RTLIL::SigSig> &new_conn);
	ثابت std::سمتیہ<RTLIL::SigSig> &connections() ثابت;

	std::سمتیہ<RTLIL::IdString> ports;
	فراغ fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_قطار;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_پورٹ_قطار;
	pool<RTLIL::Wire *> buf_norm_wire_قطار;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_اشاریہ;
	فراغ bufNormalize();

	سانچا<typename T> فراغ rewrite_sigspecs(T &functor);
	سانچا<typename T> فراغ rewrite_sigspecs2(T &functor);
	فراغ cloneInto(RTLIL::Module *new_mod) ثابت;
	مجازی RTLIL::Module *clone() ثابت;
	مجازی RTLIL::Module *clone(RTLIL::Design *dst) ثابت;
	مجازی RTLIL::Module *clone(RTLIL::Design *dst, IdString نشانہ_name) ثابت;

	bool has_memories() ثابت;
	bool has_processes() ثابت;

	bool has_memories_warn() ثابت;
	bool has_processes_warn() ثابت;

	bool is_selected() ثابت;
	bool is_selected_سارا() ثابت;

	std::سمتیہ<RTLIL::Wire*> selected_wires() ثابت;
	std::سمتیہ<RTLIL::Cell*> selected_cells() ثابت;
	std::سمتیہ<RTLIL::حافظہ*> selected_memories() ثابت;
	std::سمتیہ<RTLIL::Process*> selected_processes() ثابت;
	std::سمتیہ<RTLIL::SelectedMember> selected_members() ثابت;

	سانچا<typename T> bool selected(T *member) ثابت {
		أرجع design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		إذا (design)
			design->twines().check_owned(id);
		خودکار it = wires_.لبھو(id);
		أرجع it == wires_.سرا() ? nullptr : it->دوجا;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		إذا (design)
			design->twines().check_owned(id);
		خودکار it = cells_.لبھو(id);
		أرجع it == cells_.سرا() ? nullptr : it->دوجا;
	}

	ثابت RTLIL::Wire* wire(RTLIL::IdString id) ثابت{
		إذا (design)
			design->twines().check_owned(id);
		خودکار it = wires_.لبھو(id);
		أرجع it == wires_.سرا() ? nullptr : it->دوجا;
	}
	ثابت RTLIL::Cell* cell(RTLIL::IdString id) ثابت {
		إذا (design)
			design->twines().check_owned(id);
		خودکار it = cells_.لبھو(id);
		أرجع it == cells_.سرا() ? nullptr : it->دوجا;
	}

	سانچا<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ أرجع wire(twines().لبھو(std::string_view(name))); }
	سانچا<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ أرجع cell(twines().لبھو(std::string_view(name))); }
	سانچا<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Wire *wire(N name) ثابت
		{ أرجع wire(twines().لبھو(std::string_view(name))); }
	سانچا<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Cell *cell(N name) ثابت
		{ أرجع cell(twines().لبھو(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { أرجع RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	صحيح wires_ناپ() ثابت { أرجع wires_.ناپ(); }
	RTLIL::Wire* wire_at(صحيح اشاریہ) ثابت { أرجع wires_.element(اشاریہ)->دوجا; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { أرجع RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	صحيح cells_ناپ() ثابت { أرجع cells_.ناپ(); }
	RTLIL::Cell* cell_at(صحيح اشاریہ) ثابت { أرجع cells_.element(اشاریہ)->دوجا; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	فراغ remove(ثابت pool<RTLIL::Wire*> &wires);
	فراغ remove(RTLIL::Cell *cell);
	فراغ remove(RTLIL::حافظہ *حافظہ_3);
	فراغ remove(RTLIL::Process *process);

	فراغ rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	فراغ rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	فراغ rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	فراغ وٹاندرا_ناں(RTLIL::Wire *w1, RTLIL::Wire *w2);
	فراغ وٹاندرا_ناں(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, صحيح &اشاریہ);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *جوڑو_Wire(RTLIL::IdString name, صحيح چوڑائی = 1);
	RTLIL::Wire *جوڑو_Wire(RTLIL::IdString name, ثابت RTLIL::Wire *other);
	YS_NAME_FWD(جوڑو_Wire)

	RTLIL::Cell *جوڑو_Cell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *جوڑو_Cell(RTLIL::IdString name, ثابت RTLIL::Cell *other);
	YS_NAME_FWD(جوڑو_Cell)
	سانچا<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *جوڑو_Cell(RTLIL::IdString name, T type) { أرجع جوڑو_Cell(name, design->twines().جوڑو(std::move(type))); }

	RTLIL::حافظہ *جوڑو_حافظہ(RTLIL::IdString name);
	YS_NAME_FWD(جوڑو_حافظہ)
	RTLIL::حافظہ *جوڑو_حافظہ(RTLIL::IdString name, ثابت RTLIL::حافظہ *other);

	RTLIL::Process *جوڑو_Process(RTLIL::IdString name);
	YS_NAME_FWD(جوڑو_Process)
	RTLIL::Process *جوڑو_Process(RTLIL::IdString name, ثابت RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* جوڑو_Not     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Pos     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Buf     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Barrier (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Neg     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_And  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Or   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Xor  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Xnor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_Reduce_And  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Reduce_Or   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Reduce_Xor  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Reduce_Xnor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Reduce_Bool (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_Shl    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Shr    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Sshl   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Sshr   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_کھسکاؤ  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Shiftx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_Lt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Le  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Eq  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Ne  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Eqx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Nex (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Ge  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Gt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_جوڑو (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Sub (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Mul (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	// truncating division
	RTLIL::Cell* جوڑو_Div (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	// truncating modulo
	RTLIL::Cell* جوڑو_Mod (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Div_Floor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Mod_Floor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Pow (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_Fa (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_x, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_Logic_Not (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Logic_And (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Logic_Or  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_Mux  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Pmux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Bmux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Demux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_Bweqx  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Bwmux  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_ٹکڑا  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const وتھ, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Concat (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Lut    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Tribuf (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Assert (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Assume (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_جیوندا   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Fair   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Cover  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Equiv  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_Sr    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr, ثابت RTLIL::SigSpec &sig_q, bool سیٹ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Ff    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Dff   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d,   ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Dffe  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Dffsr (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool سیٹ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Dffsre (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool سیٹ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Adff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_قدر, bool clk_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Adffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_قدر, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Aldff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Aldffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Sdff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_قدر, bool clk_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Sdffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_قدر, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Sdffce (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_قدر, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Dlatch (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Adlatch (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_قدر, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Dlatchsr (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, bool سیٹ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_Buf_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Not_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_And_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Nand_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Or_Gate     (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Nor_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Xor_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Xnor_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Andnot_Gate (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Ornot_Gate  (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Mux_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_s, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Nmux_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_s, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Aoi3_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_c, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Oai3_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_c, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Aoi4_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_c, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_d, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Oai4_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_c, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_d, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_Sr_Gate     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
			ثابت RTLIL::SigSpec &sig_q, bool سیٹ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Ff_Gate     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Dff_Gate    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Dffe_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Dffsr_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool سیٹ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Dffsre_Gate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool سیٹ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Adff_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_قدر = false, bool clk_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Adffe_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_قدر = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Aldff_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Aldffe_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Sdff_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_قدر = false, bool clk_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Sdffe_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_قدر = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Sdffce_Gate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_قدر = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Dlatch_Gate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Adlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_قدر = false, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Dlatchsr_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سیٹ, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, bool سیٹ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");

	RTLIL::Cell* جوڑو_Anyinit(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");

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
	RTLIL::SigSpec کھسکاؤ_2  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec جوڑو_2 (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
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

	RTLIL::Sig_ثنائی_ہندسہ BufGate    (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ NotGate    (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ AndGate    (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ NandGate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ OrGate     (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ NorGate    (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ XorGate    (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ XnorGate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ AndnotGate (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ OrnotGate  (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ MuxGate    (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_s, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ NmuxGate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_s, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ Aoi3Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_c, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ Oai3Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_c, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ Aoi4Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_c, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_d, ثابت std::string &src = "");
	RTLIL::Sig_ثنائی_ہندسہ Oai4Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_a, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_b, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_c, ثابت RTLIL::Sig_ثنائی_ہندسہ &sig_d, ثابت std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, صحيح چوڑائی = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, صحيح چوڑائی = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, صحيح چوڑائی = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, صحيح چوڑائی = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ثابت std::string &src = "");

	RTLIL::SigSpec سیٹ_ٹیگ_2          (RTLIL::IdString name, ثابت std::string &ٹیگ, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src = "");
	RTLIL::Cell*   جوڑو_سیٹ_ٹیگ       (RTLIL::IdString name, ثابت std::string &ٹیگ, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::SigSpec Get_ٹیگ          (RTLIL::IdString name, ثابت std::string &ٹیگ, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src = "");
	RTLIL::Cell*   جوڑو_Overwrite_ٹیگ (RTLIL::IdString name, ثابت std::string &ٹیگ, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src = "");
	RTLIL::SigSpec Original_ٹیگ     (RTLIL::IdString name, ثابت std::string &ٹیگ, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src = "");
	YS_NAME_FWD(جوڑو_Anyinit)
	YS_NAME_FWD(Anyconst) YS_NAME_FWD(Anyseq) YS_NAME_FWD(Allconst) YS_NAME_FWD(Allseq)
	YS_NAME_FWD(Initstate)
	YS_NAME_FWD(سیٹ_ٹیگ_2) YS_NAME_FWD(جوڑو_سیٹ_ٹیگ) YS_NAME_FWD(Get_ٹیگ)
	YS_NAME_FWD(جوڑو_Overwrite_ٹیگ) YS_NAME_FWD(Original_ٹیگ) YS_NAME_FWD(FutureFF)
	YS_NAME_FWD(جوڑو_Not) YS_NAME_FWD(جوڑو_Pos) YS_NAME_FWD(جوڑو_Buf) YS_NAME_FWD(جوڑو_Barrier) YS_NAME_FWD(جوڑو_Neg)
	YS_NAME_FWD(جوڑو_And) YS_NAME_FWD(جوڑو_Or) YS_NAME_FWD(جوڑو_Xor) YS_NAME_FWD(جوڑو_Xnor)
	YS_NAME_FWD(جوڑو_Reduce_And) YS_NAME_FWD(جوڑو_Reduce_Or) YS_NAME_FWD(جوڑو_Reduce_Xor) YS_NAME_FWD(جوڑو_Reduce_Xnor)
	YS_NAME_FWD(جوڑو_Reduce_Bool) YS_NAME_FWD(جوڑو_Shl) YS_NAME_FWD(جوڑو_Shr) YS_NAME_FWD(جوڑو_Sshl)
	YS_NAME_FWD(جوڑو_Sshr) YS_NAME_FWD(جوڑو_کھسکاؤ) YS_NAME_FWD(جوڑو_Shiftx) YS_NAME_FWD(جوڑو_Lt)
	YS_NAME_FWD(جوڑو_Le) YS_NAME_FWD(جوڑو_Eq) YS_NAME_FWD(جوڑو_Ne) YS_NAME_FWD(جوڑو_Eqx)
	YS_NAME_FWD(جوڑو_Nex) YS_NAME_FWD(جوڑو_Ge) YS_NAME_FWD(جوڑو_Gt) YS_NAME_FWD(جوڑو_جوڑو)
	YS_NAME_FWD(جوڑو_Sub) YS_NAME_FWD(جوڑو_Mul) YS_NAME_FWD(جوڑو_Div) YS_NAME_FWD(جوڑو_Mod)
	YS_NAME_FWD(جوڑو_Div_Floor) YS_NAME_FWD(جوڑو_Mod_Floor) YS_NAME_FWD(جوڑو_Pow) YS_NAME_FWD(جوڑو_Fa)
	YS_NAME_FWD(جوڑو_Logic_Not) YS_NAME_FWD(جوڑو_Logic_And) YS_NAME_FWD(جوڑو_Logic_Or) YS_NAME_FWD(جوڑو_Mux)
	YS_NAME_FWD(جوڑو_Pmux) YS_NAME_FWD(جوڑو_Bmux) YS_NAME_FWD(جوڑو_Demux) YS_NAME_FWD(جوڑو_Bweqx)
	YS_NAME_FWD(جوڑو_Bwmux) YS_NAME_FWD(جوڑو_ٹکڑا) YS_NAME_FWD(جوڑو_Concat) YS_NAME_FWD(جوڑو_Lut)
	YS_NAME_FWD(جوڑو_Tribuf) YS_NAME_FWD(جوڑو_Assert) YS_NAME_FWD(جوڑو_Assume) YS_NAME_FWD(جوڑو_جیوندا)
	YS_NAME_FWD(جوڑو_Fair) YS_NAME_FWD(جوڑو_Cover) YS_NAME_FWD(جوڑو_Equiv) YS_NAME_FWD(جوڑو_Sr)
	YS_NAME_FWD(جوڑو_Ff) YS_NAME_FWD(جوڑو_Dff) YS_NAME_FWD(جوڑو_Dffe) YS_NAME_FWD(جوڑو_Dffsr)
	YS_NAME_FWD(جوڑو_Dffsre) YS_NAME_FWD(جوڑو_Adff) YS_NAME_FWD(جوڑو_Adffe) YS_NAME_FWD(جوڑو_Aldff)
	YS_NAME_FWD(جوڑو_Aldffe) YS_NAME_FWD(جوڑو_Sdff) YS_NAME_FWD(جوڑو_Sdffe) YS_NAME_FWD(جوڑو_Sdffce)
	YS_NAME_FWD(جوڑو_Dlatch) YS_NAME_FWD(جوڑو_Adlatch) YS_NAME_FWD(جوڑو_Dlatchsr) YS_NAME_FWD(جوڑو_Buf_Gate)
	YS_NAME_FWD(جوڑو_Not_Gate) YS_NAME_FWD(جوڑو_And_Gate) YS_NAME_FWD(جوڑو_Nand_Gate) YS_NAME_FWD(جوڑو_Or_Gate)
	YS_NAME_FWD(جوڑو_Nor_Gate) YS_NAME_FWD(جوڑو_Xor_Gate) YS_NAME_FWD(جوڑو_Xnor_Gate) YS_NAME_FWD(جوڑو_Andnot_Gate)
	YS_NAME_FWD(جوڑو_Ornot_Gate) YS_NAME_FWD(جوڑو_Mux_Gate) YS_NAME_FWD(جوڑو_Nmux_Gate) YS_NAME_FWD(جوڑو_Aoi3_Gate)
	YS_NAME_FWD(جوڑو_Oai3_Gate) YS_NAME_FWD(جوڑو_Aoi4_Gate) YS_NAME_FWD(جوڑو_Oai4_Gate) YS_NAME_FWD(جوڑو_Sr_Gate)
	YS_NAME_FWD(جوڑو_Ff_Gate) YS_NAME_FWD(جوڑو_Dff_Gate) YS_NAME_FWD(جوڑو_Dffe_Gate) YS_NAME_FWD(جوڑو_Dffsr_Gate)
	YS_NAME_FWD(جوڑو_Dffsre_Gate) YS_NAME_FWD(جوڑو_Adff_Gate) YS_NAME_FWD(جوڑو_Adffe_Gate) YS_NAME_FWD(جوڑو_Aldff_Gate)
	YS_NAME_FWD(جوڑو_Aldffe_Gate) YS_NAME_FWD(جوڑو_Sdff_Gate) YS_NAME_FWD(جوڑو_Sdffe_Gate) YS_NAME_FWD(جوڑو_Sdffce_Gate)
	YS_NAME_FWD(جوڑو_Dlatch_Gate) YS_NAME_FWD(جوڑو_Adlatch_Gate) YS_NAME_FWD(جوڑو_Dlatchsr_Gate) YS_NAME_FWD(Not)
	YS_NAME_FWD(Pos) YS_NAME_FWD(Buf) YS_NAME_FWD(Barrier) YS_NAME_FWD(Neg) YS_NAME_FWD(And)
	YS_NAME_FWD(Or) YS_NAME_FWD(Xor) YS_NAME_FWD(Xnor) YS_NAME_FWD(ReduceAnd)
	YS_NAME_FWD(ReduceOr) YS_NAME_FWD(ReduceXor) YS_NAME_FWD(ReduceXnor) YS_NAME_FWD(ReduceBool)
	YS_NAME_FWD(Shl) YS_NAME_FWD(Shr) YS_NAME_FWD(Sshl) YS_NAME_FWD(Sshr)
	YS_NAME_FWD(کھسکاؤ_2) YS_NAME_FWD(Shiftx) YS_NAME_FWD(Lt) YS_NAME_FWD(Le)
	YS_NAME_FWD(Eq) YS_NAME_FWD(Ne) YS_NAME_FWD(Eqx) YS_NAME_FWD(Nex)
	YS_NAME_FWD(Ge) YS_NAME_FWD(Gt) YS_NAME_FWD(جوڑو_2) YS_NAME_FWD(Sub)
	YS_NAME_FWD(Mul) YS_NAME_FWD(Div) YS_NAME_FWD(Mod) YS_NAME_FWD(DivFloor)
	YS_NAME_FWD(ModFloor) YS_NAME_FWD(Pow) YS_NAME_FWD(LogicNot) YS_NAME_FWD(LogicAnd)
	YS_NAME_FWD(LogicOr) YS_NAME_FWD(Mux) YS_NAME_FWD(Pmux) YS_NAME_FWD(Bmux)
	YS_NAME_FWD(Demux) YS_NAME_FWD(Bweqx) YS_NAME_FWD(Bwmux) YS_NAME_FWD(BufGate)
	YS_NAME_FWD(NotGate) YS_NAME_FWD(AndGate) YS_NAME_FWD(NandGate) YS_NAME_FWD(OrGate)
	YS_NAME_FWD(NorGate) YS_NAME_FWD(XorGate) YS_NAME_FWD(XnorGate) YS_NAME_FWD(AndnotGate)
	YS_NAME_FWD(OrnotGate) YS_NAME_FWD(MuxGate) YS_NAME_FWD(NmuxGate) YS_NAME_FWD(Aoi3Gate)
	YS_NAME_FWD(Oai3Gate) YS_NAME_FWD(Aoi4Gate) YS_NAME_FWD(Oai4Gate)

	std::string منزل_rtlil_str() ثابت;
#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	ساكن std::map<بنانشانی صحيح, RTLIL::Module*> *get_all_modules(فراغ);
#جے_انت
};

بنية RTLIL::Wire : عام RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	// use module->addWire() and module->remove() to create or destroy wires
	یار بنية RTLIL::Module;
	Wire();
	~Wire();

	یار بنية RTLIL::Design;
	یار بنية RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driver_پورٹ_2;

عام:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	فراغ عامل=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	صحيح چوڑائی, شروع_وتھ, پورٹ_id;
	bool پورٹ_داخلہ, پورٹ_output, upto, is_signed;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	bool معلوم_driver() ثابت { أرجع driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ثابت    { log_assert(driverCell_); أرجع driverCell_; };
	RTLIL::IdString driver_پورٹ() ثابت { log_assert(driverCell_); أرجع driver_پورٹ_2; };

	صحيح from_hdl_اشاریہ(صحيح hdl_اشاریہ) {
		صحيح zero_اشاریہ = hdl_اشاریہ - شروع_وتھ;
		صحيح rtlil_اشاریہ = upto ? چوڑائی - 1 - zero_اشاریہ : zero_اشاریہ;
		أرجع rtlil_اشاریہ >= 0 && rtlil_اشاریہ < چوڑائی ? rtlil_اشاریہ : INT_MIN;
	}

	صحيح منزل_hdl_اشاریہ(صحيح rtlil_اشاریہ) {
		إذا (rtlil_اشاریہ < 0 || rtlil_اشاریہ >= چوڑائی)
			أرجع INT_MIN;
		صحيح zero_اشاریہ = upto ? چوڑائی - 1 - rtlil_اشاریہ : rtlil_اشاریہ;
		أرجع zero_اشاریہ + شروع_وتھ;
	}

	std::string منزل_rtlil_str() ثابت;

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	ساكن std::map<بنانشانی صحيح, RTLIL::Wire*> *get_all_wires(فراغ);
#جے_انت
};

مضمن صحيح Get_ناپ(RTLIL::Wire *wire) {
	أرجع wire->چوڑائی;
}

بنية RTLIL::حافظہ : عام RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

	حافظہ();
	~حافظہ();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	RTLIL::حافظہ_Name_Id name;

	صحيح چوڑائی, شروع_وتھ, ناپ;
#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	ساكن std::map<بنانشانی صحيح, RTLIL::حافظہ*> *get_all_memorys(فراغ);
#جے_انت

	std::string منزل_rtlil_str() ثابت;
};

بنية RTLIL::Cell : عام RTLIL::AttrObject
{
خاص:
	فراغ init_اشاریہ();

	bool bufnorm_handle_سیٹ_پورٹ(IdString portname, RTLIL::SigSpec &إشارة, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
عام:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	// use module->addCell() and module->remove() to create or destroy cells
	یار بنية RTLIL::Module;
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
	bool has_پورٹ(RTLIL::IdString portname) ثابت;
	فراغ unset_پورٹ(RTLIL::IdString portname);
	فراغ سیٹ_پورٹ(RTLIL::IdString portname, RTLIL::SigSpec إشارة);
	سانچا<typename N, YS_NAME_STRING(N)> فراغ سیٹ_پورٹ(N portname, RTLIL::SigSpec إشارة)
		{ سیٹ_پورٹ(module->twines().جوڑو(std::move(portname)), std::move(إشارة)); }
	ثابت RTLIL::SigSpec &get_پورٹ(RTLIL::IdString portname) ثابت;
	ثابت dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ثابت;

	bool معلوم() ثابت;
	bool داخلہ(RTLIL::IdString portname) ثابت;
	bool output(RTLIL::IdString portname) ثابت;
	پورٹ_Dir پورٹ_dir(RTLIL::IdString portname) ثابت;

	bool hasParam(RTLIL::IdString paramname) ثابت;
	فراغ unsetParam(RTLIL::IdString paramname);
	فراغ سیٹ_Param(RTLIL::IdString paramname, RTLIL::Const قدر);
	ثابت RTLIL::Const &getParam(RTLIL::IdString paramname) ثابت;

	سانچا<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ثابت
		{ أرجع hasParam(module->twines().لبھو(std::string_view(name))); }
	سانچا<typename N, YS_NAME_STRING(N)> فراغ unsetParam(N name)
		{ unsetParam(module->twines().لبھو(std::string_view(name))); }
	سانچا<typename N, YS_NAME_STRING(N)> فراغ سیٹ_Param(N name, RTLIL::Const قدر)
		{ سیٹ_Param(module->twines().جوڑو(std::move(name)), std::move(قدر)); }
	سانچا<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Const &getParam(N name) ثابت
		{ أرجع getParam(module->twines().لبھو(std::string_view(name))); }

	فراغ ترتیب_دیو();
	فراغ check();
	فراغ fixup_parameters(bool سیٹ_a_signed = false, bool سیٹ_b_signed = false);

	bool has_keep_attr() ثابت;

	سانچا<typename T> فراغ rewrite_sigspecs(T &functor);
	سانچا<typename T> فراغ rewrite_sigspecs2(T &functor);

#جے_تعریف_شدہ YOSYS_ENABLE_PYTHON
	ساكن std::map<بنانشانی صحيح, RTLIL::Cell*> *get_all_cells(فراغ);
#جے_انت

	bool has_memid() ثابت;
	bool is_mem_cell() ثابت;
	bool is_builtin_ff() ثابت;

	std::string منزل_rtlil_str() ثابت;
};

بنية RTLIL::Case_قاعدہ : عام RTLIL::AttrObject
{
	std::سمتیہ<RTLIL::SigSpec> compare;
	std::سمتیہ<RTLIL::SigSig> actions;
	std::سمتیہ<RTLIL::Switch_قاعدہ*> switches;

	~Case_قاعدہ();

	bool خالی_تھاواں() ثابت;

	سانچا<typename T> فراغ rewrite_sigspecs(T &functor);
	سانچا<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Case_قاعدہ *clone() ثابت;
};

بنية RTLIL::Switch_قاعدہ : عام RTLIL::AttrObject
{
	RTLIL::SigSpec إشارة;
	std::سمتیہ<RTLIL::Case_قاعدہ*> cases;

	~Switch_قاعدہ();

	bool خالی_تھاواں() ثابت;

	سانچا<typename T> فراغ rewrite_sigspecs(T &functor);
	سانچا<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Switch_قاعدہ *clone() ثابت;
};

بنية RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec پتہ;
	RTLIL::SigSpec انکڑے;
	RTLIL::SigSpec enable;
	RTLIL::Const ترجیح_نقاب;
};

بنية RTLIL::Sync_قاعدہ
{
	RTLIL::SyncType type;
	RTLIL::SigSpec إشارة;
	std::سمتیہ<RTLIL::SigSig> actions;
	std::سمتیہ<RTLIL::MemWriteAction> mem_write_actions;

	سانچا<typename T> فراغ rewrite_sigspecs(T &functor);
	سانچا<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Sync_قاعدہ *clone() ثابت;
};

بنية RTLIL::Process : عام RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	// use module->addProcess() and module->remove() to create or destroy processes
	یار بنية RTLIL::Module;
	Process();
	~Process();

عام:
	RTLIL::Module *module;
	RTLIL::Case_قاعدہ جڑ_case;
	std::سمتیہ<RTLIL::Sync_قاعدہ*> syncs;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	سانچا<typename T> فراغ rewrite_sigspecs(T &functor);
	سانچا<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ثابت;

	std::string منزل_rtlil_str() ثابت;
};

مضمن RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ() : wire(عدم), انکڑے(RTLIL::State::S0) { }
مضمن RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(RTLIL::State ثنائی_ہندسہ) : wire(عدم), انکڑے(ثنائی_ہندسہ) { }
مضمن RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(bool ثنائی_ہندسہ) : wire(عدم), انکڑے(ثنائی_ہندسہ ? State::S1 : State::S0) { }
مضمن RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(RTLIL::Wire *wire) : wire(wire), وتھ(0) { log_assert(wire && wire->چوڑائی == 1); }
مضمن RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(RTLIL::Wire *wire, صحيح وتھ) : wire(wire), وتھ(وتھ) { log_assert(wire != nullptr); }
مضمن RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(ثابت RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.چوڑائی == 1); إذا (wire) وتھ = chunk.وتھ; وإلا انکڑے = chunk.انکڑے[0]; }
مضمن RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(ثابت RTLIL::SigChunk &chunk, صحيح اشاریہ) : wire(chunk.wire) { إذا (wire) وتھ = chunk.وتھ + اشاریہ; وإلا انکڑے = chunk.انکڑے[اشاریہ]; }

مضمن bool RTLIL::Sig_ثنائی_ہندسہ::عامل<(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت {
	إذا (wire == other.wire)
		أرجع wire ? (وتھ < other.وتھ) : (انکڑے < other.انکڑے);
	إذا (wire != nullptr && other.wire != nullptr)
		أرجع wire->name < other.wire->name;
	أرجع (wire != nullptr) < (other.wire != nullptr);
}

مضمن bool RTLIL::Sig_ثنائی_ہندسہ::عامل==(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت {
	أرجع (wire == other.wire) && (wire ? (وتھ == other.وتھ) : (انکڑے == other.انکڑے));
}

مضمن bool RTLIL::Sig_ثنائی_ہندسہ::عامل!=(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت {
	أرجع (wire != other.wire) || (wire ? (وتھ != other.وتھ) : (انکڑے != other.انکڑے));
}

مضمن Hasher RTLIL::Sig_ثنائی_ہندسہ::hash_into(Hasher h) ثابت {
	إذا (wire) {
		h.eat(وتھ);
		h.eat(wire->name);
		أرجع h;
	}
	h.eat(انکڑے);
	أرجع h;
}


مضمن Hasher RTLIL::Sig_ثنائی_ہندسہ::hash_چوٹی() ثابت {
	Hasher h;
	إذا (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.بٹ();
		h.force(hashlib::legacy::djb2_جوڑو(n, وتھ));
		أرجع h;
	}
	h.force(انکڑے);
	أرجع h;
}

مضمن RTLIL::Sig_ثنائی_ہندسہ &RTLIL::SigSpecIterator::عامل*() ثابت {
	أرجع (*sig_p)[اشاریہ];
}

مضمن ثابت RTLIL::Sig_ثنائی_ہندسہ &RTLIL::SigSpecConstIterator::عامل*() {
	ثنائی_ہندسہ = (*sig_p)[اشاریہ];
	أرجع ثنائی_ہندسہ;
}

مضمن RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(ثابت RTLIL::SigSpec &sig) {
	log_assert(sig.ناپ() == 1);
	خودکار it = sig.chunks().begin();
	*this = Sig_ثنائی_ہندسہ(*it);
}

سانچا<typename T>
فراغ RTLIL::Module::rewrite_sigspecs(T &functor)
{
	لكل (خودکار &it : cells_)
		it.دوجا->rewrite_sigspecs(functor);
	لكل (خودکار &it : processes)
		it.دوجا->rewrite_sigspecs(functor);
	لكل (خودکار &it : connections_) {
		functor(it.پہلا);
		functor(it.دوجا);
	}
}

سانچا<typename T>
فراغ RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	لكل (خودکار &it : cells_)
		it.دوجا->rewrite_sigspecs2(functor);
	لكل (خودکار &it : processes)
		it.دوجا->rewrite_sigspecs2(functor);
	لكل (خودکار &it : connections_) {
		functor(it.پہلا, it.دوجا);
	}
}

سانچا<typename T>
فراغ RTLIL::Cell::rewrite_sigspecs(T &functor) {
	لكل (خودکار &it : connections_)
		functor(it.دوجا);
}

سانچا<typename T>
فراغ RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	لكل (خودکار &it : connections_)
		functor(it.دوجا);
}

سانچا<typename T>
فراغ RTLIL::Case_قاعدہ::rewrite_sigspecs(T &functor) {
	لكل (خودکار &it : compare)
		functor(it);
	لكل (خودکار &it : actions) {
		functor(it.پہلا);
		functor(it.دوجا);
	}
	لكل (خودکار it : switches)
		it->rewrite_sigspecs(functor);
}

سانچا<typename T>
فراغ RTLIL::Case_قاعدہ::rewrite_sigspecs2(T &functor) {
	لكل (خودکار &it : compare)
		functor(it);
	لكل (خودکار &it : actions) {
		functor(it.پہلا, it.دوجا);
	}
	لكل (خودکار it : switches)
		it->rewrite_sigspecs2(functor);
}

سانچا<typename T>
فراغ RTLIL::Switch_قاعدہ::rewrite_sigspecs(T &functor)
{
	functor(إشارة);
	لكل (خودکار it : cases)
		it->rewrite_sigspecs(functor);
}

سانچا<typename T>
فراغ RTLIL::Switch_قاعدہ::rewrite_sigspecs2(T &functor)
{
	functor(إشارة);
	لكل (خودکار it : cases)
		it->rewrite_sigspecs2(functor);
}

سانچا<typename T>
فراغ RTLIL::Sync_قاعدہ::rewrite_sigspecs(T &functor)
{
	functor(إشارة);
	لكل (خودکار &it : actions) {
		functor(it.پہلا);
		functor(it.دوجا);
	}
	لكل (خودکار &it : mem_write_actions) {
		functor(it.پتہ);
		functor(it.انکڑے);
		functor(it.enable);
	}
}

سانچا<typename T>
فراغ RTLIL::Sync_قاعدہ::rewrite_sigspecs2(T &functor)
{
	functor(إشارة);
	لكل (خودکار &it : actions) {
		functor(it.پہلا, it.دوجا);
	}
	لكل (خودکار &it : mem_write_actions) {
		functor(it.پتہ);
		functor(it.انکڑے);
		functor(it.enable);
	}
}

سانچا<typename T>
فراغ RTLIL::Process::rewrite_sigspecs(T &functor)
{
	جڑ_case.rewrite_sigspecs(functor);
	لكل (خودکار it : syncs)
		it->rewrite_sigspecs(functor);
}

سانچا<typename T>
فراغ RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	جڑ_case.rewrite_sigspecs2(functor);
	لكل (خودکار it : syncs)
		it->rewrite_sigspecs2(functor);
}

#ضمن "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_سرا

#جے_انت
