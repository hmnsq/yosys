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

#اگر_غیر_تعریف_شدہ RTLIL_H
#تعریف_کرو RTLIL_H

#شامل "kernel/yosys_common.h"
#شامل "kernel/yosys.h"
#شامل "kernel/twine.h"

#شامل <deque>
#شامل <string_view>
#شامل <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	شمار State : غیرعلامتی حرف {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	شمار SyncType : غیرعلامتی حرف {
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
	شمار ConstFlags : غیرعلامتی حرف {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	شمار SelectPartials : غیرعلامتی حرف {
		SELECT_ALL = 0,          // include partial modules
		SELECT_پورا_ONLY = 1,   // ignore partial modules
		SELECT_پورا_WARN = 2,   // call log_warning on partial module
		SELECT_پورا_ERR = 3,    // call log_error on partial module
		SELECT_پورا_CMDERR = 4  // call log_cmd_error on partial module
	};

	شمار SelectBoxes : غیرعلامتی حرف {
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

	شمار پورٹ_Dir : غیرعلامتی حرف  {
		PD_UNKNOWN = 0,
		PD_داخلہ = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr صحیح چوڑائی_حد_بندی = 1 << 30;

	ساخت Const;
	ساخت AttrObject;
	ساخت SelectedMember;
	ساخت Selection;
	ساخت Monitor;
	ساخت Design;
	ساخت Module;
	ساخت Wire;
	ساخت حافظہ;
	ساخت Cell;
	ساخت SigChunk;
	ساخت Sig_ثنائی_ہندسہ;
	ساخت SigSpecIterator;
	ساخت SigSpecConstIterator;
	ساخت SigSpec;
	ساخت Case_قاعدہ;
	ساخت Switch_قاعدہ;
	ساخت MemWriteAction;
	ساخت Sync_قاعدہ;
	ساخت Process;

	قسمتعریف std::pair<SigSpec, SigSpec> SigSig;
};

ساخت SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_سابقہ ID; }
namespace RTLIL { using YOSYS_NAMESPACE_سابقہ IdString; }

namespace RTLIL {
	خالی copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			ثابت dict<IdString, RTLIL::Const> &src,
			ثابت RTLIL::Design *src_design, RTLIL::Design *dst_design);

	خارجی dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	ثابت pool<IdString> &builtin_ff_cell_types();

	ساکن اندرونی std::string escape_id(ثابت std::string &str) {
		اگر (str.سائز() > 0 && str[0] != '\\' && str[0] != '$')
			واپس "\\" + str;
		واپس str;
	}

	ساکن اندرونی std::string unescape_id(ثابت std::string &str) {
		اگر (str.سائز() < 2)
			واپس str;
		اگر (str[0] != '\\')
			واپس str;
		اگر (str[1] == '$' || str[1] == '\\')
			واپس str;
		اگر (str[1] >= '0' && str[1] <= '9')
			واپس str;
		واپس str.substr(1);
	}

	سانچہ <typename T> using ترتیب_دو_by_name_id = IdString::compare_ptr_by_name<T>;

	سانچہ <typename T> ساخت ترتیب_دو_by_name_str {
		bool عامل()(T *a, T *b) ثابت {
			واپس a->name.lt_by_name(b->name);
		}
	};

	ساخت ترتیب_دو_by_id_str {
		ثابت TwinePool& pool;
		explicit ترتیب_دو_by_id_str(ثابت TwinePool& pool) : pool(pool) {}
		bool عامل()(RTLIL::IdString a, RTLIL::IdString b) ثابت {
			واپس pool.compare_by_name(a, b) < 0;
		}
	};

	ساکن اندرونی std::string encode_filename(ثابت std::string &filename)
	{
		std::stringstream val;
		اگر (!std::any_کا(filename.begin(), filename.انتہا(), [](حرف c) {
			واپس static_cast<غیرعلامتی حرف>(c) < 33 || static_cast<غیرعلامتی حرف>(c) > 126;
		})) واپس filename;
		برائے (غیرعلامتی حرف ثابت c : filename) {
			اگر (c < 33 || c > 126)
				val << stringf("$%02x", c);
			ورنہ
				val << c;
		}
		واپس val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_and         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_or          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_xor         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_xnor        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);

	RTLIL::Const const_reduce_and  (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_reduce_or   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_reduce_xor  (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_reduce_xnor (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_reduce_bool (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);

	RTLIL::Const const_logic_not   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_logic_and   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_logic_or    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);

	RTLIL::Const const_shl         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_shr         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_sshl        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_sshr        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_سرکاؤ       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_shiftx      (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);

	RTLIL::Const const_lt          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_le          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_eq          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_ne          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_eqx         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_nex         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_ge          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_gt          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);

	RTLIL::Const const_جوڑو         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_sub         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_mul         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_div         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_divfloor    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_modfloor    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_mod         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_pow         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);

	RTLIL::Const const_pos         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_buf         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);
	RTLIL::Const const_neg         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحیح نتیجہ_len);

	RTLIL::Const const_mux         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	سانچہ<typename T, typename کلید_2 = IdString>
	ساخت ObjIterator {
		using iterator_category = std::forward_iterator_لیبل;
		using قدر_type = T;
		using difference_type = ptrdiff_t;
		using اشارہ_گر = T*;
		using reference = T&;
		typename dict<کلید_2, T>::iterator it;
		dict<کلید_2, T> *list_p;
		صحیح *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, صحیح *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			اگر (list_p->خالی_خانے()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} ورنہ {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(ثابت RTLIL::ObjIterator<T, کلید_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			اگر (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &عامل=(ثابت RTLIL::ObjIterator<T, کلید_2> &other) {
			اگر (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			اگر (refcount_p)
				(*refcount_p)++;
			واپس *this;
		}

		~ObjIterator() {
			اگر (refcount_p)
				(*refcount_p)--;
		}

		اندرونی T عامل*() ثابت {
			log_assert(list_p != nullptr);
			واپس it->دوسرا;
		}

		اندرونی bool عامل!=(ثابت RTLIL::ObjIterator<T, کلید_2> &other) ثابت {
			اگر (list_p == nullptr || other.list_p == nullptr)
				واپس list_p != other.list_p;
			واپس it != other.it;
		}


		اندرونی bool عامل==(ثابت RTLIL::ObjIterator<T, کلید_2> &other) ثابت {
			واپس !(*this != other);
		}

		اندرونی ObjIterator<T, کلید_2>& عامل++() {
			log_assert(list_p != nullptr);
			اگر (++it == list_p->انتہا()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			واپس *this;
		}

		اندرونی ObjIterator<T, کلید_2>& عامل+=(صحیح amt) {
			log_assert(list_p != nullptr);
			it += amt;
			اگر (it == list_p->انتہا()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			واپس *this;
		}

		اندرونی ObjIterator<T, کلید_2> عامل+(صحیح amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, کلید_2> new_obj(*this);
			new_obj.it += amt;
			اگر (new_obj.it == list_p->انتہا()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			واپس new_obj;
		}

		اندرونی ثابت ObjIterator<T, کلید_2> عامل++(صحیح) {
			ObjIterator<T, کلید_2> نتیجہ(*this);
			++(*this);
			واپس نتیجہ;
		}
	};

	سانچہ<typename T, typename کلید_2 = IdString>
	ساخت ObjRange
	{
		dict<کلید_2, T> *list_p;
		صحیح *refcount_p;

		ObjRange(decltype(list_p) list_p, صحیح *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, کلید_2> begin() { واپس RTLIL::ObjIterator<T, کلید_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, کلید_2> انتہا() { واپس RTLIL::ObjIterator<T, کلید_2>(); }

		سائز_t سائز() ثابت {
			واپس list_p->سائز();
		}

		عامل pool<T>() ثابت {
			pool<T> نتیجہ;
			برائے (خودکار &it : *list_p)
				نتیجہ.داخل_کرو(it.دوسرا);
			واپس نتیجہ;
		}

		عامل std::سمتیہ<T>() ثابت {
			std::سمتیہ<T> نتیجہ;
			نتیجہ.reserve(list_p->سائز());
			برائے (خودکار &it : *list_p)
				نتیجہ.ڈالو_عقب(it.دوسرا);
			واپس نتیجہ;
		}

		pool<T> مقصد_pool() ثابت { واپس *this; }
		std::سمتیہ<T> مقصد_سمتیہ() ثابت { واپس *this; }
	};
};

ساخت RTLIL::Const
{
	مختصر صحیح flags;
نجی:
	دوست class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::سمتیہ<RTLIL::State>;
	شمار class backing_لیبل: bool { بٹ, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_لیبل لیبل;
	اتحاد {
		bitvectype بٹ_2;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_بٹ() ثابت { واپس لیبل == backing_لیبل::بٹ; }
	bool is_str() ثابت { واپس لیبل == backing_لیبل::string; }

	bitvectype* get_if_بٹ() { واپس is_بٹ() ? &بٹ_2 : تہی; }
	std::string* get_if_str() { واپس is_str() ? &str_ : تہی; }
	ثابت bitvectype* get_if_بٹ() ثابت { واپس is_بٹ() ? &بٹ_2 : تہی; }
	ثابت std::string* get_if_str() ثابت { واپس is_str() ? &str_ : تہی; }

	bitvectype& get_بٹ();
	std::string& get_str();
	ثابت bitvectype& get_بٹ() ثابت;
	ثابت std::string& get_str() ثابت;
	std::سمتیہ<RTLIL::State>& بٹ_اندرونی();
	خالی bitvectorize_اندرونی();

عوامی:
	Const() : flags(RTLIL::CONST_FLAG_NONE), لیبل(backing_لیبل::بٹ), بٹ_2(std::سمتیہ<RTLIL::State>()) {}
	Const(std::string str);
	Const(طویل طویل val); // default width is 32
	Const(طویل طویل val, صحیح چوڑائی);
	Const(RTLIL::State ثنائی_ہندسہ, صحیح چوڑائی = 1);
	Const(std::سمتیہ<RTLIL::State> بٹ) : flags(RTLIL::CONST_FLAG_NONE), لیبل(backing_لیبل::بٹ), بٹ_2(std::move(بٹ)) {}
	Const(ثابت std::سمتیہ<bool> &بٹ);
	Const(ثابت RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &عامل =(ثابت RTLIL::Const &other);
	~Const();

	ساخت Builder
	{
		Builder() {}
		Builder(صحیح expected_چوڑائی) { بٹ.reserve(expected_چوڑائی); }
		خالی ڈالو_عقب(RTLIL::State b) { بٹ.ڈالو_عقب(b); }
		صحیح سائز() ثابت { واپس static_cast<صحیح>(بٹ.سائز()); }
		Const build() { واپس Const(std::move(بٹ)); }
	نجی:
		std::سمتیہ<RTLIL::State> بٹ;
	};

	bool عامل <(ثابت RTLIL::Const &other) ثابت;
	bool عامل ==(ثابت RTLIL::Const &other) ثابت;
	bool عامل !=(ثابت RTLIL::Const &other) ثابت;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::سمتیہ<RTLIL::State>& بٹ() { واپس بٹ_اندرونی(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	خالی bitvectorize() ثابت { const_cast<Const*>(this)->bitvectorize_اندرونی(); }

	bool as_bool() ثابت;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	صحیح as_int(bool is_signed = false) ثابت;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_مقصد_int(bool is_signed = false) ثابت;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<صحیح> try_as_int(bool is_signed = false) ثابت;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	صحیح as_int_saturating(bool is_signed = false) ثابت;

	خالی لیبل_bare_integer_const(ثابت std::string &قدر);

	std::string as_string(ثابت حرف* any = "-") ثابت;
	ساکن Const from_string(ثابت std::string &str);
	std::سمتیہ<RTLIL::State> مقصد_بٹ() ثابت;

	std::string decode_string() ثابت;
	صحیح سائز() ثابت;
	bool خالی_خانے() ثابت;

	خالی append(ثابت RTLIL::Const &other);
	خالی سیٹ(صحیح i, RTLIL::State state) {
		بٹ_اندرونی()[i] = state;
	}
	خالی resize(صحیح سائز, RTLIL::State fill) {
    log_assert(سائز >= 0 && سائز < RTLIL::چوڑائی_حد_بندی);
		بٹ_اندرونی().resize(سائز, fill);
	}

	class const_iterator {
	نجی:
		ثابت Const* والد;
		سائز_t idx;

	عوامی:
		using iterator_category = std::bidirectional_iterator_لیبل;
		using قدر_type = State;
		using difference_type = std::ptrdiff_t;
		using اشارہ_گر = ثابت State*;
		using reference = ثابت State&;

		const_iterator(ثابت Const& c, سائز_t i) : والد(&c), idx(i) {}

		State عامل*() ثابت;

		const_iterator& عامل++() { ++idx; واپس *this; }
		const_iterator& عامل--() { --idx; واپس *this; }
		const_iterator عامل++(صحیح) { const_iterator نتیجہ(*this); ++idx; واپس نتیجہ; }
		const_iterator عامل--(صحیح) { const_iterator نتیجہ(*this); --idx; واپس نتیجہ; }
		const_iterator& عامل+=(صحیح i) { idx += i; واپس *this; }

		const_iterator عامل+(صحیح جوڑو) {
			واپس const_iterator(*والد, idx + جوڑو);
		}
		const_iterator عامل-(صحیح sub) {
			واپس const_iterator(*والد, idx - sub);
		}
		صحیح عامل-(ثابت const_iterator& other) {
			واپس idx - other.idx;
		}

		bool عامل==(ثابت const_iterator& other) ثابت {
			واپس idx == other.idx;
		}

		bool عامل!=(ثابت const_iterator& other) ثابت {
			واپس !(*this == other);
		}
	};

	class iterator {
	نجی:
		Const* والد;
		سائز_t idx;

	عوامی:
		class proxy {
		نجی:
			Const* والد;
			سائز_t idx;
		عوامی:
			proxy(Const* والد, سائز_t idx) : والد(والد), idx(idx) {}
			عامل State() ثابت { واپس (*والد)[idx]; }
			proxy& عامل=(State s) { والد->سیٹ(idx, s); واپس *this; }
			proxy& عامل=(ثابت proxy& other) { والد->سیٹ(idx, (*other.والد)[other.idx]); واپس *this; }
		};

		using iterator_category = std::bidirectional_iterator_لیبل;
		using قدر_type = State;
		using difference_type = std::ptrdiff_t;
		using اشارہ_گر = proxy*;
		using reference = proxy;

		iterator(Const& c, سائز_t i) : والد(&c), idx(i) {}

		proxy عامل*() ثابت { واپس proxy(والد, idx); }
		iterator& عامل++() { ++idx; واپس *this; }
		iterator& عامل--() { --idx; واپس *this; }
		iterator عامل++(صحیح) { iterator نتیجہ(*this); ++idx; واپس نتیجہ; }
		iterator عامل--(صحیح) { iterator نتیجہ(*this); --idx; واپس نتیجہ; }
		iterator& عامل+=(صحیح i) { idx += i; واپس *this; }

		iterator عامل+(صحیح جوڑو) {
			واپس iterator(*والد, idx + جوڑو);
		}
		iterator عامل-(صحیح sub) {
			واپس iterator(*والد, idx - sub);
		}
		صحیح عامل-(ثابت iterator& other) {
			واپس idx - other.idx;
		}

		bool عامل==(ثابت iterator& other) ثابت {
			واپس idx == other.idx;
		}

		bool عامل!=(ثابت iterator& other) ثابت {
			واپس !(*this == other);
		}
	};

	const_iterator begin() ثابت {
		واپس const_iterator(*this, 0);
	}
	const_iterator انتہا() ثابت {
		واپس const_iterator(*this, سائز());
	}
	iterator begin() {
		واپس iterator(*this, 0);
	}
	iterator انتہا() {
		واپس iterator(*this, سائز());
	}
	State عقب() ثابت {
		واپس *(انتہا() - 1);
	}
	State front() ثابت {
		واپس *begin();
	}
	State at(سائز_t i) ثابت {
		واپس *const_iterator(*this, i);
	}
	State عامل[](سائز_t i) ثابت {
		واپس *const_iterator(*this, i);
	}

	bool is_fully_zero() ثابت;
	bool is_fully_ones() ثابت;
	bool is_fully_def() ثابت;
	bool is_fully_undef() ثابت;
	bool is_fully_undef_x_only() ثابت;
	bool is_onehot(صحیح *pos = nullptr) ثابت;

	RTLIL::Const نکالو_2(صحیح انحراف, صحیح len = 1, RTLIL::State padding = RTLIL::State::S0) ثابت;

	// find the MSB without redundant leading bits
	صحیح get_min_سائز(bool is_signed) ثابت;

	// compress representation to the minimum required bits
	خالی compress(bool is_signed = false);

	std::optional<صحیح> as_int_compress(bool is_signed) ثابت;

	خالی extu(صحیح چوڑائی) {
		resize(چوڑائی, RTLIL::State::S0);
	}

	خالی exts(صحیح چوڑائی) {
		resize(چوڑائی, خالی_خانے() ? RTLIL::State::Sx : عقب());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

ساخت RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> اوصاف;

	bool has_attribute(RTLIL::IdString id) ثابت;

	خالی سیٹ_bool_attribute(RTLIL::IdString id, bool قدر=true);
	bool get_bool_attribute(RTLIL::IdString id) ثابت;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ثابت {
		واپس get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	خالی سیٹ_string_attribute(RTLIL::IdString id, string قدر);
	string get_string_attribute(RTLIL::IdString id) ثابت;

	خالی سیٹ_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &ڈیٹا);
	خالی جوڑو_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &ڈیٹا);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ثابت;

	خالی سیٹ_src_attribute(ثابت std::string &src) {
		سیٹ_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ثابت {
		واپس get_string_attribute(ID::src);
	}

	خالی سیٹ_hdlname_attribute(ثابت سمتیہ<string> &hierarchy);
	سمتیہ<string> get_hdlname_attribute() ثابت;

	خالی سیٹ_intvec_attribute(RTLIL::IdString id, ثابت سمتیہ<صحیح> &ڈیٹا);
	سمتیہ<صحیح> get_intvec_attribute(RTLIL::IdString id) ثابت;
};

#شامل "kernel/rtlil_twine_compat.h"

ساخت RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

ساخت RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::سمتیہ<RTLIL::State> ڈیٹا; // only used if wire == NULL, LSB at index 0
	صحیح چوڑائی, انحراف;

	SigChunk() : wire(nullptr), چوڑائی(0), انحراف(0) {}
	SigChunk(ثابت RTLIL::Const &قدر) : wire(nullptr), ڈیٹا(قدر.مقصد_بٹ()), چوڑائی(Get_سائز(ڈیٹا)), انحراف(0) {}
	SigChunk(RTLIL::Const &&قدر) : wire(nullptr), ڈیٹا(قدر.مقصد_بٹ()), چوڑائی(Get_سائز(ڈیٹا)), انحراف(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), چوڑائی(Get_سائز(wire)), انحراف(0) {}
	SigChunk(RTLIL::Wire *wire, صحیح انحراف, صحیح چوڑائی = 1) : wire(wire), چوڑائی(چوڑائی), انحراف(انحراف) {}
	SigChunk(ثابت std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(صحیح val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(صحیح val, صحیح چوڑائی) : SigChunk(RTLIL::Const(val, چوڑائی)) {}
	SigChunk(RTLIL::State ثنائی_ہندسہ, صحیح چوڑائی = 1) : SigChunk(RTLIL::Const(ثنائی_ہندسہ, چوڑائی)) {}
	SigChunk(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ);

	RTLIL::SigChunk نکالو_2(صحیح انحراف, صحیح لمبائی_2) ثابت;
	RTLIL::Sig_ثنائی_ہندسہ عامل[](صحیح انحراف) ثابت;
	اندرونی صحیح سائز() ثابت { واپس چوڑائی; }
	اندرونی bool is_wire() ثابت { واپس wire != تہی; }

	bool عامل <(ثابت RTLIL::SigChunk &other) ثابت;
	bool عامل ==(ثابت RTLIL::SigChunk &other) ثابت;
	bool عامل !=(ثابت RTLIL::SigChunk &other) ثابت;
};

ساخت RTLIL::Sig_ثنائی_ہندسہ
{
	RTLIL::Wire *wire;
	اتحاد {
		RTLIL::State ڈیٹا; // used if wire == NULL
		صحیح انحراف;        // used if wire != NULL
	};

	Sig_ثنائی_ہندسہ();
	Sig_ثنائی_ہندسہ(RTLIL::State ثنائی_ہندسہ);
	explicit Sig_ثنائی_ہندسہ(bool ثنائی_ہندسہ);
	Sig_ثنائی_ہندسہ(RTLIL::Wire *wire);
	Sig_ثنائی_ہندسہ(RTLIL::Wire *wire, صحیح انحراف);
	Sig_ثنائی_ہندسہ(ثابت RTLIL::SigChunk &chunk);
	Sig_ثنائی_ہندسہ(ثابت RTLIL::SigChunk &chunk, صحیح اشاریہ);
	Sig_ثنائی_ہندسہ(ثابت RTLIL::SigSpec &sig);
	Sig_ثنائی_ہندسہ(ثابت RTLIL::Sig_ثنائی_ہندسہ &sigbit) = طےشدہ;
	RTLIL::Sig_ثنائی_ہندسہ &عامل =(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) = طےشدہ;

	اندرونی bool is_wire() ثابت { واپس wire != تہی; }

	bool عامل <(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت;
	bool عامل ==(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت;
	bool عامل !=(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
	[[nodiscard]] Hasher hash_چوٹی() ثابت;
};

namespace hashlib {
	سانچہ <>
	ساخت hash_ops<RTLIL::Sig_ثنائی_ہندسہ> {
		ساکن اندرونی bool cmp(ثابت RTLIL::Sig_ثنائی_ہندسہ &a, ثابت RTLIL::Sig_ثنائی_ہندسہ &b) {
			واپس a == b;
		}
		[[nodiscard]] ساکن اندرونی Hasher hash(ثابت RTLIL::Sig_ثنائی_ہندسہ sb) {
			واپس sb.hash_چوٹی();
		}
		[[nodiscard]] ساکن اندرونی Hasher hash_into(ثابت RTLIL::Sig_ثنائی_ہندسہ sb, Hasher h) {
			واپس sb.hash_into(h);
		}
	};
};

ساخت RTLIL::SigSpecIterator
{
	قسمتعریف std::داخلہ_iterator_لیبل iterator_category;
	قسمتعریف RTLIL::Sig_ثنائی_ہندسہ قدر_type;
	قسمتعریف ptrdiff_t difference_type;
	قسمتعریف RTLIL::Sig_ثنائی_ہندسہ* اشارہ_گر;
	قسمتعریف RTLIL::Sig_ثنائی_ہندسہ& reference;

	RTLIL::SigSpec *sig_p;
	صحیح اشاریہ;

	اندرونی RTLIL::Sig_ثنائی_ہندسہ &عامل*() ثابت;
	اندرونی bool عامل!=(ثابت RTLIL::SigSpecIterator &other) ثابت { واپس اشاریہ != other.اشاریہ; }
	اندرونی bool عامل==(ثابت RTLIL::SigSpecIterator &other) ثابت { واپس اشاریہ == other.اشاریہ; }
	اندرونی خالی عامل++() { اشاریہ++; }
};

ساخت RTLIL::SigSpecConstIterator
{
	قسمتعریف std::داخلہ_iterator_لیبل iterator_category;
	قسمتعریف RTLIL::Sig_ثنائی_ہندسہ قدر_type;
	قسمتعریف ptrdiff_t difference_type;
	قسمتعریف RTLIL::Sig_ثنائی_ہندسہ* اشارہ_گر;
	قسمتعریف RTLIL::Sig_ثنائی_ہندسہ& reference;

	ثابت RTLIL::SigSpec *sig_p;
	RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ;
	صحیح اشاریہ;

	اندرونی ثابت RTLIL::Sig_ثنائی_ہندسہ &عامل*();
	اندرونی bool عامل!=(ثابت RTLIL::SigSpecConstIterator &other) ثابت { واپس اشاریہ != other.اشاریہ; }
	اندرونی bool عامل==(ثابت RTLIL::SigSpecIterator &other) ثابت { واپس اشاریہ == other.اشاریہ; }
	اندرونی خالی عامل++() { اشاریہ++; }
};

ساخت RTLIL::SigSpec
{
نجی:
	دوست class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, نکالو_3);
	شمار Representation : حرف {
		CHUNK,
		بٹ_3,
	};
	// An AtomicHash is either clear or a nonzero integer.
	ساخت AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ثابت AtomicHash &rhs) : atomic_(rhs.بھرو()) {}
		AtomicHash &عامل=(ثابت AtomicHash &rhs) { store(rhs.بھرو()); واپس *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> اقرأ() ثابت {
			Hasher::hash_t قدر = بھرو();
			اگر (قدر == 0)
				واپس std::nullopt;
			واپس قدر;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		خالی سیٹ(Hasher::hash_t قدر) ثابت {
			log_assert(قدر != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(قدر, std::حافظہ_ترتیب_relaxed);
			log_assert(old == 0 || old == قدر);
		}
		خالی clear() { store(0); }
	نجی:
		صحیح بھرو() ثابت { واپس atomic_.بھرو(std::حافظہ_ترتیب_relaxed); }
		خالی store(Hasher::hash_t قدر) ثابت {
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

	خالی init_خالی_خانے_بٹ() {
		rep_ = بٹ_3;
		new (&بٹ_2) std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>;
	}

	خالی unpack();
	اندرونی خالی inline_unpack() {
		اگر (rep_ == CHUNK)
			unpack();
	}
	خالی try_repack();

	Hasher::hash_t updhash() ثابت;
	خالی destroy() {
		اگر (rep_ == CHUNK)
			chunk_.~SigChunk();
		ورنہ
			بٹ_2.~سمتیہ();
	}
	دوست ساخت Chunks;

عوامی:
	SigSpec() { init_خالی_خانے_بٹ(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ثابت SigSpec &قدر) : rep_(قدر.rep_), hash_(قدر.hash_) {
		اگر (قدر.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(قدر.chunk_);
		ورنہ
			new (&بٹ_2) std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>(قدر.بٹ_2);
	}
	SigSpec(SigSpec &&قدر) : rep_(قدر.rep_), hash_(قدر.hash_) {
		اگر (قدر.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(قدر.chunk_));
		ورنہ
			new (&بٹ_2) std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>(std::move(قدر.بٹ_2));
	}
	SigSpec(ثابت RTLIL::Const &قدر);
	SigSpec(RTLIL::Const &&قدر);
	SigSpec(ثابت RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, صحیح انحراف, صحیح چوڑائی = 1);
	SigSpec(ثابت std::string &str);
	SigSpec(صحیح val, صحیح چوڑائی = 32);
	SigSpec(RTLIL::State ثنائی_ہندسہ, صحیح چوڑائی = 1);
	SigSpec(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ, صحیح چوڑائی = 1);
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
		اگر (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		ورنہ
			new (&بٹ_2) std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>(rhs.بٹ_2);
		واپس *this;
	}
	SigSpec &عامل=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		اگر (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		ورنہ
			new (&بٹ_2) std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>(std::move(rhs.بٹ_2));
		واپس *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	ساخت Chunks {
		Chunks(ثابت SigSpec &spec) : spec(spec) {}
		ساخت const_iterator {
			using iterator_category = std::forward_iterator_لیبل;
			using قدر_type = ثابت SigChunk &;
			using difference_type = std::ptrdiff_t;
			using اشارہ_گر = ثابت SigChunk *;
			using reference = ثابت SigChunk &;

			ثابت SigSpec &spec;
			صحیح ثنائی_ہندسہ_اشاریہ;
			SigChunk chunk;

			const_iterator(ثابت SigSpec &spec) : spec(spec) {
				ثنائی_ہندسہ_اشاریہ = 0;
				اگر (spec.rep_ == بٹ_3)
					اگلا_chunk_بٹ();
			}
			شمار انتہا_2 { انتہا_3 };
			const_iterator(ثابت SigSpec &spec, انتہا_2) : spec(spec) {
				ثنائی_ہندسہ_اشاریہ = spec.سائز();
			}
			خالی اگلا_chunk_بٹ();

			ثابت SigChunk &عامل*() {
				اگر (spec.rep_ == CHUNK)
					واپس spec.chunk_;
				واپس chunk;
			};
			ثابت SigChunk *عامل->() { واپس &**this; }
			const_iterator &عامل++() {
				ثنائی_ہندسہ_اشاریہ += (**this).چوڑائی;
				اگر (spec.rep_ == بٹ_3)
					اگلا_chunk_بٹ();
				واپس *this;
			}
			bool عامل==(ثابت const_iterator &rhs) ثابت { واپس ثنائی_ہندسہ_اشاریہ == rhs.ثنائی_ہندسہ_اشاریہ; }
			bool عامل!=(ثابت const_iterator &rhs) ثابت { واپس !(*this == rhs); }
		};
		const_iterator begin() ثابت { واپس const_iterator(spec); }
		const_iterator انتہا() ثابت {
			const_iterator it(spec, const_iterator::انتہا_3);
			واپس it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::سمتیہ<RTLIL::SigChunk>::const_الٹ_iterator rbegin() {
			ensure_chunk_سمتیہ();
			واپس chunk_سمتیہ.rbegin();
		}
		std::سمتیہ<RTLIL::SigChunk>::const_الٹ_iterator rend() {
			ensure_chunk_سمتیہ();
			واپس chunk_سمتیہ.rend();
		}
		صحیح سائز() {
			ensure_chunk_سمتیہ();
			واپس chunk_سمتیہ.سائز();
		}
		صحیح سائز() ثابت {
			واپس std::فاصلہ(begin(), انتہا());
		}
		ثابت SigChunk &at(صحیح اشاریہ) {
			ensure_chunk_سمتیہ();
			واپس chunk_سمتیہ.at(اشاریہ);
		}
		عامل ثابت std::سمتیہ<RTLIL::SigChunk>&() {
			ensure_chunk_سمتیہ();
			واپس chunk_سمتیہ;
		}
	نجی:
		خالی ensure_chunk_سمتیہ() {
			اگر (spec.سائز() > 0 && chunk_سمتیہ.خالی_خانے()) {
				برائے (ثابت RTLIL::SigChunk &c : *this)
					chunk_سمتیہ.ڈالو_عقب(c);
			}
		}
		ثابت SigSpec &spec;
		std::سمتیہ<RTLIL::SigChunk> chunk_سمتیہ;
	};
	دوست ساخت Chunks::const_iterator;

	اندرونی Chunks chunks() ثابت { واپس {*this}; }
	اندرونی ثابت SigSpec &بٹ() ثابت { واپس *this; }

	اندرونی صحیح سائز() ثابت { واپس rep_ == CHUNK ? chunk_.چوڑائی : Get_سائز(بٹ_2); }
	اندرونی bool خالی_خانے() ثابت { واپس سائز() == 0; };

	اندرونی RTLIL::Sig_ثنائی_ہندسہ &عامل[](صحیح اشاریہ) { inline_unpack(); hash_.clear(); واپس بٹ_2.at(اشاریہ); }
	اندرونی RTLIL::Sig_ثنائی_ہندسہ عامل[](صحیح اشاریہ) ثابت {
		اگر (rep_ == CHUNK) {
			اگر (اشاریہ < 0 || اشاریہ >= chunk_.چوڑائی)
				throw std::برآمد_کا_range("SigSpec::operator[]");
			اگر (chunk_.wire)
				واپس RTLIL::Sig_ثنائی_ہندسہ(chunk_.wire, chunk_.انحراف + اشاریہ);
			واپس RTLIL::Sig_ثنائی_ہندسہ(chunk_.ڈیٹا[اشاریہ]);
		}
		واپس بٹ_2.at(اشاریہ);
	}

	اندرونی RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.اشاریہ = 0; واپس it; }
	اندرونی RTLIL::SigSpecIterator انتہا() { RTLIL::SigSpecIterator it; it.sig_p = this; it.اشاریہ = سائز(); واپس it; }

	اندرونی RTLIL::SigSpecConstIterator begin() ثابت { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.اشاریہ = 0; واپس it; }
	اندرونی RTLIL::SigSpecConstIterator انتہا() ثابت { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.اشاریہ = سائز(); واپس it; }

	خالی ترتیب_دو();
	خالی ترتیب_دو_and_unify();

	خالی replace(ثابت RTLIL::SigSpec &نمونہ, ثابت RTLIL::SigSpec &with);
	خالی replace(ثابت RTLIL::SigSpec &نمونہ, ثابت RTLIL::SigSpec &with, RTLIL::SigSpec *other) ثابت;

	خالی replace(ثابت dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قواعد);
	خالی replace(ثابت dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قواعد, RTLIL::SigSpec *other) ثابت;

	خالی replace(ثابت std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قواعد);
	خالی replace(ثابت std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> &قواعد, RTLIL::SigSpec *other) ثابت;

	خالی replace(صحیح انحراف, ثابت RTLIL::SigSpec &with);

	خالی remove(ثابت RTLIL::SigSpec &نمونہ);
	خالی remove(ثابت RTLIL::SigSpec &نمونہ, RTLIL::SigSpec *other) ثابت;
	خالی remove2(ثابت RTLIL::SigSpec &نمونہ, RTLIL::SigSpec *other);

	خالی remove(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ);
	خالی remove(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, RTLIL::SigSpec *other) ثابت;
	خالی remove2(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, RTLIL::SigSpec *other);
	خالی remove2(ثابت std::سیٹ<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, RTLIL::SigSpec *other);
	خالی remove2(ثابت pool<RTLIL::Wire*> &نمونہ, RTLIL::SigSpec *other);

	خالی remove(صحیح انحراف, صحیح لمبائی_2 = 1);
	خالی remove_const();

	RTLIL::SigSpec نکالو_2(ثابت RTLIL::SigSpec &نمونہ, ثابت RTLIL::SigSpec *other = تہی) ثابت;
	RTLIL::SigSpec نکالو_2(ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &نمونہ, ثابت RTLIL::SigSpec *other = تہی) ثابت;
	RTLIL::SigSpec نکالو_2(صحیح انحراف, صحیح لمبائی_2 = 1) ثابت;
	RTLIL::SigSpec نکالو_انتہا(صحیح انحراف) ثابت { واپس نکالو_2(انحراف, سائز() - انحراف); }

	خالی rewrite_wires(std::function<خالی(RTLIL::Wire*& wire)> rewrite);

	RTLIL::Sig_ثنائی_ہندسہ lsb() ثابت { log_assert(سائز()); واپس (*this)[0]; };
	RTLIL::Sig_ثنائی_ہندسہ msb() ثابت { log_assert(سائز()); واپس (*this)[سائز() - 1]; };
	RTLIL::Sig_ثنائی_ہندسہ front() ثابت { واپس (*this)[0]; }
	RTLIL::Sig_ثنائی_ہندسہ عقب() ثابت { واپس (*this)[سائز() - 1]; }

	خالی append(ثابت RTLIL::SigSpec &اشارہ);
	اندرونی خالی append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	اندرونی خالی append(ثابت RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	اندرونی خالی append(ثابت RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	خالی append(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ);
	اندرونی خالی append(RTLIL::State state) { append(RTLIL::Sig_ثنائی_ہندسہ(state)); }
	اندرونی خالی append(bool bool_) { append(RTLIL::Sig_ثنائی_ہندسہ(bool_)); }

	خالی extend_u0(صحیح چوڑائی, bool is_signed = false);

	RTLIL::SigSpec repeat(صحیح عدد) ثابت;

	خالی الٹ() { inline_unpack(); std::الٹ(بٹ_2.begin(), بٹ_2.انتہا()); }

	bool عامل <(ثابت RTLIL::SigSpec &other) ثابت;
	bool عامل ==(ثابت RTLIL::SigSpec &other) ثابت;
	اندرونی bool عامل !=(ثابت RTLIL::SigSpec &other) ثابت { واپس !(*this == other); }

	bool is_wire() ثابت;
	bool is_chunk() ثابت;
	اندرونی bool is_ثنائی_ہندسہ() ثابت { واپس سائز() == 1; }

	bool معلوم_driver() ثابت;

	bool is_fully_const() ثابت;
	bool is_fully_zero() ثابت;
	bool is_fully_ones() ثابت;
	bool is_fully_def() ثابت;
	bool is_fully_undef() ثابت;
	bool has_const() ثابت;
	bool has_const(State state) ثابت;
	bool has_marked_بٹ() ثابت;
	bool is_onehot(صحیح *pos = nullptr) ثابت;

	bool as_bool() ثابت;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	صحیح as_int(bool is_signed = false) ثابت;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_مقصد_int(bool is_signed = false) ثابت;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<صحیح> try_as_int(bool is_signed = false) ثابت;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	صحیح as_int_saturating(bool is_signed = false) ثابت;

	std::string as_string() ثابت;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() ثابت;
	RTLIL::Const as_const() ثابت;
	RTLIL::Wire *as_wire() ثابت;
	RTLIL::SigChunk as_chunk() ثابت;
	RTLIL::Sig_ثنائی_ہندسہ as_ثنائی_ہندسہ() ثابت;

	bool مطابقت(ثابت حرف* نمونہ) ثابت;

	std::سیٹ<RTLIL::Sig_ثنائی_ہندسہ> مقصد_sigbit_سیٹ() ثابت;
	pool<RTLIL::Sig_ثنائی_ہندسہ> مقصد_sigbit_pool() ثابت;
	std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ> مقصد_sigbit_سمتیہ() ثابت;
	std::map<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> مقصد_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت;
	dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> مقصد_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت;

	ساکن bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	ساکن bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	ساکن bool parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	عامل std::سمتیہ<RTLIL::SigChunk>() ثابت;
	عامل std::سمتیہ<RTLIL::Sig_ثنائی_ہندسہ>() ثابت { واپس مقصد_sigbit_سمتیہ(); }
	ثابت RTLIL::Sig_ثنائی_ہندسہ &at(صحیح انحراف, ثابت RTLIL::Sig_ثنائی_ہندسہ &defval) { واپس انحراف < سائز() ? (*this)[انحراف] : defval; }
	RTLIL::Sig_ثنائی_ہندسہ& at(صحیح انحراف) { واپس (*this)[انحراف]; }
	RTLIL::Sig_ثنائی_ہندسہ at(صحیح انحراف) ثابت { واپس (*this)[انحراف]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		Hasher::hash_t val;
		اگر (std::optional<Hasher::hash_t> current = hash_.اقرأ())
			val = *current;
		ورنہ
			val = updhash();
		h.eat(val);
		واپس h;
	}

#اگر_غیر_تعریف_شدہ NDEBUG
	خالی check(ثابت Module *mod = nullptr) ثابت;
#ورنہ
	خالی check(ثابت Module *mod = nullptr) ثابت { (خالی)mod; }
#اگر_ختم
};

ساخت RTLIL::Selection
{
	// selection includes boxed modules
	bool selects_boxes;
	// selection covers full design, including boxed modules
	bool complete_selection;
	// selection covers full design, not including boxed modules
	bool بھرا_selection;
	pool<RTLIL::IdString> selected_modules;
	dict<RTLIL::IdString, pool<RTLIL::IdString>> selected_members;
	RTLIL::Design *current_design;

	// create a new selection
	Selection(
		// should the selection cover the full design
		bool بھرا_2 = true,
		// should the selection include boxed modules
		bool boxes = false,
		// the design to select from
		RTLIL::Design *design = nullptr
	) :
		selects_boxes(boxes), complete_selection(بھرا_2 && boxes), بھرا_selection(بھرا_2 && !boxes), current_design(design) { }

	// checks if the given module exists in the current design and is a
	// boxed module, warning the user if the current design is not set
	bool boxed_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_پورا_module(RTLIL::IdString mod_name) ثابت;

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
	خالی optimize(RTLIL::Design *design);

	// checks if selection covers full design (may or may not include
	// boxed-modules)
	bool selects_all() ثابت {
		واپس بھرا_selection || complete_selection;
	}

	// add whole module to this selection
	سانچہ<typename T1> خالی select(T1 *module) {
		اگر (!selects_all() && selected_modules.گنتی(module->name) == 0) {
			IdString name = module->name;
			selected_modules.داخل_کرو(name);
			selected_members.erase(name);
			اگر (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	سانچہ<typename T1, typename T2> خالی select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	سانچہ<typename T1> خالی select(T1 *module, RTLIL::IdString member) {
		اگر (!selects_all() && selected_modules.گنتی(module->name.ref()) == 0) {
			selected_members[module->name.ref()].داخل_کرو(member);
			اگر (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool خالی_خانے() ثابت {
		واپس !selects_all() && selected_modules.خالی_خانے() && selected_members.خالی_خانے();
	}

	// clear this selection, leaving it empty
	خالی clear();

	// create a new selection which is empty
	ساکن Selection خالی_خانے_Selection(RTLIL::Design *design = nullptr) { واپس Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	ساکن Selection بھرا_Selection(RTLIL::Design *design = nullptr) { واپس Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	ساکن Selection CompleteSelection(RTLIL::Design *design = nullptr) { واپس Selection(true, true, design); };
};

ساخت RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); واپس h; }

	Monitor() {
		ساکن غیرعلامتی صحیح hashidx_گنتی = 123456789;
		hashidx_گنتی = mkhash_xorshift(hashidx_گنتی);
		hashidx_ = hashidx_گنتی;
	}

	مجازی ~Monitor() { }
	مجازی خالی notify_module_جوڑو(RTLIL::Module*) { }
	مجازی خالی notify_module_del(RTLIL::Module*) { }
	مجازی خالی notify_connect(RTLIL::Cell*, RTLIL::IdString, ثابت RTLIL::SigSpec&, ثابت RTLIL::SigSpec&) { }
	مجازی خالی notify_connect(RTLIL::Module*, ثابت RTLIL::SigSig&) { }
	مجازی خالی notify_connect(RTLIL::Module*, ثابت std::سمتیہ<RTLIL::SigSig>&) { }
	مجازی خالی notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
ساخت define_map_t;

سانچہ<typename N>
اندرونی constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::برگ> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ثابت حرف*> || std::is_same_v<std::decay_t<N>, حرف*>;

#تعریف_کرو YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, صحیح> = 0

سانچہ<typename N>
اندرونی constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ثابت حرف *> || std::is_same_v<std::decay_t<N>, حرف *>;

#تعریف_کرو YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, صحیح> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#تعریف_کرو YS_NAME_FWD_POOL(_func, _pool) \
	سانچہ<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(خودکار) _func(N name, Rest&&... rest) \
		{ واپس _func(_pool.جوڑو(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#تعریف_کرو YS_NAME_FWD_2ND_POOL(_func, _pool) \
	سانچہ<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(خودکار) _func(T &&پہلا, N name) \
		{ واپس _func(std::forward<T>(پہلا), _pool.جوڑو(std::move(name))); }

#تعریف_کرو YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#تعریف_کرو YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

ساخت RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); واپس h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	خالی bufNormalize(bool enable=true);

	صحیح refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

نجی:
	TwinePool twines_;

عوامی:
	TwinePool &twines() { واپس twines_; }
	ثابت TwinePool &twines() ثابت { واپس twines_; }

	خالی absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	سائز_t gc_twines();

	std::سمتیہ<std::unique_ptr<AST::Ast_گرہ>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_متعین_کرتا;

	std::سمتیہ<RTLIL::Selection> selection_ڈھیر;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_فعال_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ثابت RTLIL::Module *module(RTLIL::IdString name) ثابت;
	سانچہ<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ واپس module(twines().ڈھونڈو(std::string_view(name))); }
	سانچہ<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Module *module(N name) ثابت
		{ واپس module(twines().ڈھونڈو(std::string_view(name))); }
	RTLIL::Module *چوٹی_module() ثابت;

	bool has(RTLIL::IdString id) ثابت {
		twines_.check_owned(id);
		واپس modules_.گنتی(id) != 0;
	}
	سانچہ<typename N, YS_NAME_STRING(N)> bool has(N name) ثابت
		{ واپس has(twines().ڈھونڈو(std::string_view(name))); }

	خالی جوڑو(RTLIL::Module *module);

	RTLIL::Module *جوڑو_Module(RTLIL::IdString name);
	YS_NAME_FWD_POOL(جوڑو_Module, twines())
	خالی remove(RTLIL::Module *module);
	خالی rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	خالی scratchpad_unset(ثابت std::string &varname);

	خالی scratchpad_سیٹ_int(ثابت std::string &varname, صحیح قدر);
	خالی scratchpad_سیٹ_bool(ثابت std::string &varname, bool قدر);
	خالی scratchpad_سیٹ_string(ثابت std::string &varname, std::string قدر);

	صحیح scratchpad_get_int(ثابت std::string &varname, صحیح default_قدر_2 = 0) ثابت;
	bool scratchpad_get_bool(ثابت std::string &varname, bool default_قدر_2 = false) ثابت;
	std::string scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_قدر_2 = std::string()) ثابت;

	خالی ترتیب_دو();
	خالی ترتیب_دو_modules();
	خالی check();
	خالی optimize();

	خالی clone_into(RTLIL::Design *dst) ثابت;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_پورا_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ثابت;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_پورا_module(RTLIL::Module *mod) ثابت;

	// push the given selection to the selection stack
	خالی ڈالو_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	خالی ڈالو_خالی_خانے_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	خالی ڈالو_بھرا_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	خالی ڈالو_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	خالی نکالو_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		واپس selection_ڈھیر.عقب();
	}

	// get the current selection
	ثابت RTLIL::Selection &selection() ثابت {
		واپس selection_ڈھیر.عقب();
	}

	// is the current selection a full selection (no boxes)
	bool بھرا_selection() ثابت {
		واپس selection().بھرا_selection;
	}

	// is the given module in the current selection
	سانچہ<typename T1> bool selected(T1 *module) ثابت {
		واپس selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	سانچہ<typename T1, typename T2> bool selected(T1 *module, T2 *member) ثابت {
		واپس selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	سانچہ<typename T1> خالی select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	سانچہ<typename T1, typename T2> خالی select(T1 *module, T2 *member) {
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
	std::سمتیہ<RTLIL::Module*> all_selected_modules() ثابت { واپس selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::سمتیہ<RTLIL::Module*> selected_unboxed_modules() ثابت { واپس selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::سمتیہ<RTLIL::Module*> selected_unboxed_modules_warn() ثابت { واپس selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::سمتیہ<RTLIL::Module*> selected_پورا_modules() ثابت { واپس selected_modules(SELECT_پورا_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::سمتیہ<RTLIL::Module*> all_selected_پورا_modules() ثابت { واپس selected_modules(SELECT_پورا_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::سمتیہ<RTLIL::Module*> selected_پورا_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ثابت { واپس selected_modules(SELECT_پورا_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::سمتیہ<RTLIL::Module*> selected_unboxed_پورا_modules() ثابت { واپس selected_modules(SELECT_پورا_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::سمتیہ<RTLIL::Module*> selected_unboxed_پورا_modules_warn() ثابت { واپس selected_modules(SELECT_پورا_WARN, SB_UNBOXED_WARN); }

	ساکن std::map<غیرعلامتی صحیح, RTLIL::Design*> *get_all_designs(خالی);

	std::string مقصد_rtlil_str(bool only_selected = true) ثابت;
};

ساخت RTLIL::Module : عوامی RTLIL::AttrObject
{
	دوست ساخت RTLIL::Cell;
	دوست ساخت RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); واپس h; }

محفوظ:
	خالی جوڑو(RTLIL::Wire *wire);
	خالی جوڑو(RTLIL::Cell *cell);
	خالی جوڑو(RTLIL::Process *process);

عوامی:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ثابت { واپس design->twines(); }

	صحیح refcount_wires_;
	صحیح refcount_cells_;

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
	مجازی سائز_t گنتی_id(RTLIL::IdString id);
	مجازی خالی expand_interfaces(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	مجازی bool reprocess_if_necessary(RTLIL::Design *design);

	مجازی خالی ترتیب_دو();
	مجازی خالی check();
	مجازی خالی optimize();
	مجازی خالی makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ثابت {
		واپس get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	خالی connect(ثابت RTLIL::SigSig &conn);
	خالی connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs);
	خالی new_connections(ثابت std::سمتیہ<RTLIL::SigSig> &new_conn);
	ثابت std::سمتیہ<RTLIL::SigSig> &connections() ثابت;

	std::سمتیہ<RTLIL::IdString> ports;
	خالی fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_قطار;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_پورٹ_قطار;
	pool<RTLIL::Wire *> buf_norm_wire_قطار;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_اشاریہ;
	خالی bufNormalize();

	سانچہ<typename T> خالی rewrite_sigspecs(T &functor);
	سانچہ<typename T> خالی rewrite_sigspecs2(T &functor);
	خالی cloneInto(RTLIL::Module *new_mod) ثابت;
	مجازی RTLIL::Module *clone() ثابت;
	مجازی RTLIL::Module *clone(RTLIL::Design *dst) ثابت;
	مجازی RTLIL::Module *clone(RTLIL::Design *dst, IdString ہدف_name) ثابت;

	bool has_memories() ثابت;
	bool has_processes() ثابت;

	bool has_memories_warn() ثابت;
	bool has_processes_warn() ثابت;

	bool is_selected() ثابت;
	bool is_selected_پورا() ثابت;

	std::سمتیہ<RTLIL::Wire*> selected_wires() ثابت;
	std::سمتیہ<RTLIL::Cell*> selected_cells() ثابت;
	std::سمتیہ<RTLIL::حافظہ*> selected_memories() ثابت;
	std::سمتیہ<RTLIL::Process*> selected_processes() ثابت;
	std::سمتیہ<RTLIL::SelectedMember> selected_members() ثابت;

	سانچہ<typename T> bool selected(T *member) ثابت {
		واپس design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		اگر (design)
			design->twines().check_owned(id);
		خودکار it = wires_.ڈھونڈو(id);
		واپس it == wires_.انتہا() ? nullptr : it->دوسرا;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		اگر (design)
			design->twines().check_owned(id);
		خودکار it = cells_.ڈھونڈو(id);
		واپس it == cells_.انتہا() ? nullptr : it->دوسرا;
	}

	ثابت RTLIL::Wire* wire(RTLIL::IdString id) ثابت{
		اگر (design)
			design->twines().check_owned(id);
		خودکار it = wires_.ڈھونڈو(id);
		واپس it == wires_.انتہا() ? nullptr : it->دوسرا;
	}
	ثابت RTLIL::Cell* cell(RTLIL::IdString id) ثابت {
		اگر (design)
			design->twines().check_owned(id);
		خودکار it = cells_.ڈھونڈو(id);
		واپس it == cells_.انتہا() ? nullptr : it->دوسرا;
	}

	سانچہ<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ واپس wire(twines().ڈھونڈو(std::string_view(name))); }
	سانچہ<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ واپس cell(twines().ڈھونڈو(std::string_view(name))); }
	سانچہ<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Wire *wire(N name) ثابت
		{ واپس wire(twines().ڈھونڈو(std::string_view(name))); }
	سانچہ<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Cell *cell(N name) ثابت
		{ واپس cell(twines().ڈھونڈو(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { واپس RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	صحیح wires_سائز() ثابت { واپس wires_.سائز(); }
	RTLIL::Wire* wire_at(صحیح اشاریہ) ثابت { واپس wires_.element(اشاریہ)->دوسرا; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { واپس RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	صحیح cells_سائز() ثابت { واپس cells_.سائز(); }
	RTLIL::Cell* cell_at(صحیح اشاریہ) ثابت { واپس cells_.element(اشاریہ)->دوسرا; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	خالی remove(ثابت pool<RTLIL::Wire*> &wires);
	خالی remove(RTLIL::Cell *cell);
	خالی remove(RTLIL::حافظہ *حافظہ_3);
	خالی remove(RTLIL::Process *process);

	خالی rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	خالی rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	خالی rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	خالی تبادلہ_نام(RTLIL::Wire *w1, RTLIL::Wire *w2);
	خالی تبادلہ_نام(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, صحیح &اشاریہ);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *جوڑو_Wire(RTLIL::IdString name, صحیح چوڑائی = 1);
	RTLIL::Wire *جوڑو_Wire(RTLIL::IdString name, ثابت RTLIL::Wire *other);
	YS_NAME_FWD(جوڑو_Wire)

	RTLIL::Cell *جوڑو_Cell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *جوڑو_Cell(RTLIL::IdString name, ثابت RTLIL::Cell *other);
	YS_NAME_FWD(جوڑو_Cell)
	سانچہ<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *جوڑو_Cell(RTLIL::IdString name, T type) { واپس جوڑو_Cell(name, design->twines().جوڑو(std::move(type))); }

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
	RTLIL::Cell* جوڑو_سرکاؤ  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
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

	RTLIL::Cell* جوڑو_قاش  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const انحراف, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Concat (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Lut    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Tribuf (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Assert (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_Assume (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جوڑو_زندہ   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
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
	RTLIL::SigSpec سرکاؤ_2  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
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

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, صحیح چوڑائی = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, صحیح چوڑائی = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, صحیح چوڑائی = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, صحیح چوڑائی = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ثابت std::string &src = "");

	RTLIL::SigSpec سیٹ_لیبل_2          (RTLIL::IdString name, ثابت std::string &لیبل, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src = "");
	RTLIL::Cell*   جوڑو_سیٹ_لیبل       (RTLIL::IdString name, ثابت std::string &لیبل, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::SigSpec Get_لیبل          (RTLIL::IdString name, ثابت std::string &لیبل, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src = "");
	RTLIL::Cell*   جوڑو_Overwrite_لیبل (RTLIL::IdString name, ثابت std::string &لیبل, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src = "");
	RTLIL::SigSpec Original_لیبل     (RTLIL::IdString name, ثابت std::string &لیبل, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src = "");
	YS_NAME_FWD(جوڑو_Anyinit)
	YS_NAME_FWD(Anyconst) YS_NAME_FWD(Anyseq) YS_NAME_FWD(Allconst) YS_NAME_FWD(Allseq)
	YS_NAME_FWD(Initstate)
	YS_NAME_FWD(سیٹ_لیبل_2) YS_NAME_FWD(جوڑو_سیٹ_لیبل) YS_NAME_FWD(Get_لیبل)
	YS_NAME_FWD(جوڑو_Overwrite_لیبل) YS_NAME_FWD(Original_لیبل) YS_NAME_FWD(FutureFF)
	YS_NAME_FWD(جوڑو_Not) YS_NAME_FWD(جوڑو_Pos) YS_NAME_FWD(جوڑو_Buf) YS_NAME_FWD(جوڑو_Barrier) YS_NAME_FWD(جوڑو_Neg)
	YS_NAME_FWD(جوڑو_And) YS_NAME_FWD(جوڑو_Or) YS_NAME_FWD(جوڑو_Xor) YS_NAME_FWD(جوڑو_Xnor)
	YS_NAME_FWD(جوڑو_Reduce_And) YS_NAME_FWD(جوڑو_Reduce_Or) YS_NAME_FWD(جوڑو_Reduce_Xor) YS_NAME_FWD(جوڑو_Reduce_Xnor)
	YS_NAME_FWD(جوڑو_Reduce_Bool) YS_NAME_FWD(جوڑو_Shl) YS_NAME_FWD(جوڑو_Shr) YS_NAME_FWD(جوڑو_Sshl)
	YS_NAME_FWD(جوڑو_Sshr) YS_NAME_FWD(جوڑو_سرکاؤ) YS_NAME_FWD(جوڑو_Shiftx) YS_NAME_FWD(جوڑو_Lt)
	YS_NAME_FWD(جوڑو_Le) YS_NAME_FWD(جوڑو_Eq) YS_NAME_FWD(جوڑو_Ne) YS_NAME_FWD(جوڑو_Eqx)
	YS_NAME_FWD(جوڑو_Nex) YS_NAME_FWD(جوڑو_Ge) YS_NAME_FWD(جوڑو_Gt) YS_NAME_FWD(جوڑو_جوڑو)
	YS_NAME_FWD(جوڑو_Sub) YS_NAME_FWD(جوڑو_Mul) YS_NAME_FWD(جوڑو_Div) YS_NAME_FWD(جوڑو_Mod)
	YS_NAME_FWD(جوڑو_Div_Floor) YS_NAME_FWD(جوڑو_Mod_Floor) YS_NAME_FWD(جوڑو_Pow) YS_NAME_FWD(جوڑو_Fa)
	YS_NAME_FWD(جوڑو_Logic_Not) YS_NAME_FWD(جوڑو_Logic_And) YS_NAME_FWD(جوڑو_Logic_Or) YS_NAME_FWD(جوڑو_Mux)
	YS_NAME_FWD(جوڑو_Pmux) YS_NAME_FWD(جوڑو_Bmux) YS_NAME_FWD(جوڑو_Demux) YS_NAME_FWD(جوڑو_Bweqx)
	YS_NAME_FWD(جوڑو_Bwmux) YS_NAME_FWD(جوڑو_قاش) YS_NAME_FWD(جوڑو_Concat) YS_NAME_FWD(جوڑو_Lut)
	YS_NAME_FWD(جوڑو_Tribuf) YS_NAME_FWD(جوڑو_Assert) YS_NAME_FWD(جوڑو_Assume) YS_NAME_FWD(جوڑو_زندہ)
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
	YS_NAME_FWD(سرکاؤ_2) YS_NAME_FWD(Shiftx) YS_NAME_FWD(Lt) YS_NAME_FWD(Le)
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

	std::string مقصد_rtlil_str() ثابت;
#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	ساکن std::map<غیرعلامتی صحیح, RTLIL::Module*> *get_all_modules(خالی);
#اگر_ختم
};

ساخت RTLIL::Wire : عوامی RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); واپس h; }

محفوظ:
	// use module->addWire() and module->remove() to create or destroy wires
	دوست ساخت RTLIL::Module;
	Wire();
	~Wire();

	دوست ساخت RTLIL::Design;
	دوست ساخت RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driver_پورٹ_2;

عوامی:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	خالی عامل=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	صحیح چوڑائی, آغاز_انحراف, پورٹ_id;
	bool پورٹ_داخلہ, پورٹ_output, upto, is_signed;

	RTLIL::Design *design() ثابت { واپس module ? module->design : nullptr; }
	TwinePool &twines() ثابت { واپس module->twines(); }

	bool معلوم_driver() ثابت { واپس driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ثابت    { log_assert(driverCell_); واپس driverCell_; };
	RTLIL::IdString driver_پورٹ() ثابت { log_assert(driverCell_); واپس driver_پورٹ_2; };

	صحیح from_hdl_اشاریہ(صحیح hdl_اشاریہ) {
		صحیح zero_اشاریہ = hdl_اشاریہ - آغاز_انحراف;
		صحیح rtlil_اشاریہ = upto ? چوڑائی - 1 - zero_اشاریہ : zero_اشاریہ;
		واپس rtlil_اشاریہ >= 0 && rtlil_اشاریہ < چوڑائی ? rtlil_اشاریہ : INT_MIN;
	}

	صحیح مقصد_hdl_اشاریہ(صحیح rtlil_اشاریہ) {
		اگر (rtlil_اشاریہ < 0 || rtlil_اشاریہ >= چوڑائی)
			واپس INT_MIN;
		صحیح zero_اشاریہ = upto ? چوڑائی - 1 - rtlil_اشاریہ : rtlil_اشاریہ;
		واپس zero_اشاریہ + آغاز_انحراف;
	}

	std::string مقصد_rtlil_str() ثابت;

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	ساکن std::map<غیرعلامتی صحیح, RTLIL::Wire*> *get_all_wires(خالی);
#اگر_ختم
};

اندرونی صحیح Get_سائز(RTLIL::Wire *wire) {
	واپس wire->چوڑائی;
}

ساخت RTLIL::حافظہ : عوامی RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); واپس h; }

	حافظہ();
	~حافظہ();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() ثابت { واپس module ? module->design : nullptr; }
	TwinePool &twines() ثابت { واپس module->twines(); }

	RTLIL::حافظہ_Name_Id name;

	صحیح چوڑائی, آغاز_انحراف, سائز;
#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	ساکن std::map<غیرعلامتی صحیح, RTLIL::حافظہ*> *get_all_memorys(خالی);
#اگر_ختم

	std::string مقصد_rtlil_str() ثابت;
};

ساخت RTLIL::Cell : عوامی RTLIL::AttrObject
{
نجی:
	خالی init_اشاریہ();

	bool bufnorm_handle_سیٹ_پورٹ(IdString portname, RTLIL::SigSpec &اشارہ, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
عوامی:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); واپس h; }

محفوظ:
	// use module->addCell() and module->remove() to create or destroy cells
	دوست ساخت RTLIL::Module;
	Cell();
	~Cell();

عوامی:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	خالی عامل=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() ثابت { واپس module ? module->design : nullptr; }
	TwinePool &twines() ثابت { واپس module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool has_پورٹ(RTLIL::IdString portname) ثابت;
	خالی unset_پورٹ(RTLIL::IdString portname);
	خالی سیٹ_پورٹ(RTLIL::IdString portname, RTLIL::SigSpec اشارہ);
	سانچہ<typename N, YS_NAME_STRING(N)> خالی سیٹ_پورٹ(N portname, RTLIL::SigSpec اشارہ)
		{ سیٹ_پورٹ(module->twines().جوڑو(std::move(portname)), std::move(اشارہ)); }
	ثابت RTLIL::SigSpec &get_پورٹ(RTLIL::IdString portname) ثابت;
	ثابت dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ثابت;

	bool معلوم() ثابت;
	bool داخلہ(RTLIL::IdString portname) ثابت;
	bool output(RTLIL::IdString portname) ثابت;
	پورٹ_Dir پورٹ_dir(RTLIL::IdString portname) ثابت;

	bool hasParam(RTLIL::IdString paramname) ثابت;
	خالی unsetParam(RTLIL::IdString paramname);
	خالی سیٹ_Param(RTLIL::IdString paramname, RTLIL::Const قدر);
	ثابت RTLIL::Const &getParam(RTLIL::IdString paramname) ثابت;

	سانچہ<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ثابت
		{ واپس hasParam(module->twines().ڈھونڈو(std::string_view(name))); }
	سانچہ<typename N, YS_NAME_STRING(N)> خالی unsetParam(N name)
		{ unsetParam(module->twines().ڈھونڈو(std::string_view(name))); }
	سانچہ<typename N, YS_NAME_STRING(N)> خالی سیٹ_Param(N name, RTLIL::Const قدر)
		{ سیٹ_Param(module->twines().جوڑو(std::move(name)), std::move(قدر)); }
	سانچہ<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Const &getParam(N name) ثابت
		{ واپس getParam(module->twines().ڈھونڈو(std::string_view(name))); }

	خالی ترتیب_دو();
	خالی check();
	خالی fixup_parameters(bool سیٹ_a_signed = false, bool سیٹ_b_signed = false);

	bool has_keep_attr() ثابت;

	سانچہ<typename T> خالی rewrite_sigspecs(T &functor);
	سانچہ<typename T> خالی rewrite_sigspecs2(T &functor);

#اگر_تعریف_شدہ YOSYS_ENABLE_PYTHON
	ساکن std::map<غیرعلامتی صحیح, RTLIL::Cell*> *get_all_cells(خالی);
#اگر_ختم

	bool has_memid() ثابت;
	bool is_mem_cell() ثابت;
	bool is_builtin_ff() ثابت;

	std::string مقصد_rtlil_str() ثابت;
};

ساخت RTLIL::Case_قاعدہ : عوامی RTLIL::AttrObject
{
	std::سمتیہ<RTLIL::SigSpec> compare;
	std::سمتیہ<RTLIL::SigSig> actions;
	std::سمتیہ<RTLIL::Switch_قاعدہ*> switches;

	~Case_قاعدہ();

	bool خالی_خانے() ثابت;

	سانچہ<typename T> خالی rewrite_sigspecs(T &functor);
	سانچہ<typename T> خالی rewrite_sigspecs2(T &functor);
	RTLIL::Case_قاعدہ *clone() ثابت;
};

ساخت RTLIL::Switch_قاعدہ : عوامی RTLIL::AttrObject
{
	RTLIL::SigSpec اشارہ;
	std::سمتیہ<RTLIL::Case_قاعدہ*> cases;

	~Switch_قاعدہ();

	bool خالی_خانے() ثابت;

	سانچہ<typename T> خالی rewrite_sigspecs(T &functor);
	سانچہ<typename T> خالی rewrite_sigspecs2(T &functor);
	RTLIL::Switch_قاعدہ *clone() ثابت;
};

ساخت RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec پتہ;
	RTLIL::SigSpec ڈیٹا;
	RTLIL::SigSpec enable;
	RTLIL::Const ترجیح_نقاب;
};

ساخت RTLIL::Sync_قاعدہ
{
	RTLIL::SyncType type;
	RTLIL::SigSpec اشارہ;
	std::سمتیہ<RTLIL::SigSig> actions;
	std::سمتیہ<RTLIL::MemWriteAction> mem_write_actions;

	سانچہ<typename T> خالی rewrite_sigspecs(T &functor);
	سانچہ<typename T> خالی rewrite_sigspecs2(T &functor);
	RTLIL::Sync_قاعدہ *clone() ثابت;
};

ساخت RTLIL::Process : عوامی RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); واپس h; }

محفوظ:
	// use module->addProcess() and module->remove() to create or destroy processes
	دوست ساخت RTLIL::Module;
	Process();
	~Process();

عوامی:
	RTLIL::Module *module;
	RTLIL::Case_قاعدہ جڑ_case;
	std::سمتیہ<RTLIL::Sync_قاعدہ*> syncs;

	RTLIL::Design *design() ثابت { واپس module ? module->design : nullptr; }
	TwinePool &twines() ثابت { واپس module->twines(); }

	سانچہ<typename T> خالی rewrite_sigspecs(T &functor);
	سانچہ<typename T> خالی rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ثابت;

	std::string مقصد_rtlil_str() ثابت;
};

اندرونی RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ() : wire(تہی), ڈیٹا(RTLIL::State::S0) { }
اندرونی RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(RTLIL::State ثنائی_ہندسہ) : wire(تہی), ڈیٹا(ثنائی_ہندسہ) { }
اندرونی RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(bool ثنائی_ہندسہ) : wire(تہی), ڈیٹا(ثنائی_ہندسہ ? State::S1 : State::S0) { }
اندرونی RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(RTLIL::Wire *wire) : wire(wire), انحراف(0) { log_assert(wire && wire->چوڑائی == 1); }
اندرونی RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(RTLIL::Wire *wire, صحیح انحراف) : wire(wire), انحراف(انحراف) { log_assert(wire != nullptr); }
اندرونی RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(ثابت RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.چوڑائی == 1); اگر (wire) انحراف = chunk.انحراف; ورنہ ڈیٹا = chunk.ڈیٹا[0]; }
اندرونی RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(ثابت RTLIL::SigChunk &chunk, صحیح اشاریہ) : wire(chunk.wire) { اگر (wire) انحراف = chunk.انحراف + اشاریہ; ورنہ ڈیٹا = chunk.ڈیٹا[اشاریہ]; }

اندرونی bool RTLIL::Sig_ثنائی_ہندسہ::عامل<(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت {
	اگر (wire == other.wire)
		واپس wire ? (انحراف < other.انحراف) : (ڈیٹا < other.ڈیٹا);
	اگر (wire != nullptr && other.wire != nullptr)
		واپس wire->name < other.wire->name;
	واپس (wire != nullptr) < (other.wire != nullptr);
}

اندرونی bool RTLIL::Sig_ثنائی_ہندسہ::عامل==(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت {
	واپس (wire == other.wire) && (wire ? (انحراف == other.انحراف) : (ڈیٹا == other.ڈیٹا));
}

اندرونی bool RTLIL::Sig_ثنائی_ہندسہ::عامل!=(ثابت RTLIL::Sig_ثنائی_ہندسہ &other) ثابت {
	واپس (wire != other.wire) || (wire ? (انحراف != other.انحراف) : (ڈیٹا != other.ڈیٹا));
}

اندرونی Hasher RTLIL::Sig_ثنائی_ہندسہ::hash_into(Hasher h) ثابت {
	اگر (wire) {
		h.eat(انحراف);
		h.eat(wire->name);
		واپس h;
	}
	h.eat(ڈیٹا);
	واپس h;
}


اندرونی Hasher RTLIL::Sig_ثنائی_ہندسہ::hash_چوٹی() ثابت {
	Hasher h;
	اگر (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.بٹ();
		h.force(hashlib::legacy::djb2_جوڑو(n, انحراف));
		واپس h;
	}
	h.force(ڈیٹا);
	واپس h;
}

اندرونی RTLIL::Sig_ثنائی_ہندسہ &RTLIL::SigSpecIterator::عامل*() ثابت {
	واپس (*sig_p)[اشاریہ];
}

اندرونی ثابت RTLIL::Sig_ثنائی_ہندسہ &RTLIL::SigSpecConstIterator::عامل*() {
	ثنائی_ہندسہ = (*sig_p)[اشاریہ];
	واپس ثنائی_ہندسہ;
}

اندرونی RTLIL::Sig_ثنائی_ہندسہ::Sig_ثنائی_ہندسہ(ثابت RTLIL::SigSpec &sig) {
	log_assert(sig.سائز() == 1);
	خودکار it = sig.chunks().begin();
	*this = Sig_ثنائی_ہندسہ(*it);
}

سانچہ<typename T>
خالی RTLIL::Module::rewrite_sigspecs(T &functor)
{
	برائے (خودکار &it : cells_)
		it.دوسرا->rewrite_sigspecs(functor);
	برائے (خودکار &it : processes)
		it.دوسرا->rewrite_sigspecs(functor);
	برائے (خودکار &it : connections_) {
		functor(it.پہلا);
		functor(it.دوسرا);
	}
}

سانچہ<typename T>
خالی RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	برائے (خودکار &it : cells_)
		it.دوسرا->rewrite_sigspecs2(functor);
	برائے (خودکار &it : processes)
		it.دوسرا->rewrite_sigspecs2(functor);
	برائے (خودکار &it : connections_) {
		functor(it.پہلا, it.دوسرا);
	}
}

سانچہ<typename T>
خالی RTLIL::Cell::rewrite_sigspecs(T &functor) {
	برائے (خودکار &it : connections_)
		functor(it.دوسرا);
}

سانچہ<typename T>
خالی RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	برائے (خودکار &it : connections_)
		functor(it.دوسرا);
}

سانچہ<typename T>
خالی RTLIL::Case_قاعدہ::rewrite_sigspecs(T &functor) {
	برائے (خودکار &it : compare)
		functor(it);
	برائے (خودکار &it : actions) {
		functor(it.پہلا);
		functor(it.دوسرا);
	}
	برائے (خودکار it : switches)
		it->rewrite_sigspecs(functor);
}

سانچہ<typename T>
خالی RTLIL::Case_قاعدہ::rewrite_sigspecs2(T &functor) {
	برائے (خودکار &it : compare)
		functor(it);
	برائے (خودکار &it : actions) {
		functor(it.پہلا, it.دوسرا);
	}
	برائے (خودکار it : switches)
		it->rewrite_sigspecs2(functor);
}

سانچہ<typename T>
خالی RTLIL::Switch_قاعدہ::rewrite_sigspecs(T &functor)
{
	functor(اشارہ);
	برائے (خودکار it : cases)
		it->rewrite_sigspecs(functor);
}

سانچہ<typename T>
خالی RTLIL::Switch_قاعدہ::rewrite_sigspecs2(T &functor)
{
	functor(اشارہ);
	برائے (خودکار it : cases)
		it->rewrite_sigspecs2(functor);
}

سانچہ<typename T>
خالی RTLIL::Sync_قاعدہ::rewrite_sigspecs(T &functor)
{
	functor(اشارہ);
	برائے (خودکار &it : actions) {
		functor(it.پہلا);
		functor(it.دوسرا);
	}
	برائے (خودکار &it : mem_write_actions) {
		functor(it.پتہ);
		functor(it.ڈیٹا);
		functor(it.enable);
	}
}

سانچہ<typename T>
خالی RTLIL::Sync_قاعدہ::rewrite_sigspecs2(T &functor)
{
	functor(اشارہ);
	برائے (خودکار &it : actions) {
		functor(it.پہلا, it.دوسرا);
	}
	برائے (خودکار &it : mem_write_actions) {
		functor(it.پتہ);
		functor(it.ڈیٹا);
		functor(it.enable);
	}
}

سانچہ<typename T>
خالی RTLIL::Process::rewrite_sigspecs(T &functor)
{
	جڑ_case.rewrite_sigspecs(functor);
	برائے (خودکار it : syncs)
		it->rewrite_sigspecs(functor);
}

سانچہ<typename T>
خالی RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	جڑ_case.rewrite_sigspecs2(functor);
	برائے (خودکار it : syncs)
		it->rewrite_sigspecs2(functor);
}

#شامل "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_انتہا

#اگر_ختم
