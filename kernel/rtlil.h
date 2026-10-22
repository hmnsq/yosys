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

#اگر_تعریف_نشده RTLIL_H
#تعریف_کن RTLIL_H

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/yosys.h"
#ضمن "kernel/twine.h"

#ضمن <deque>
#ضمن <string_view>
#ضمن <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	تعداد State : بیعلامت حرف {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	تعداد SyncType : بیعلامت حرف {
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
	تعداد ConstFlags : بیعلامت حرف {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	تعداد SelectPartials : بیعلامت حرف {
		SELECT_ALL = 0,          // include partial modules
		SELECT_کامل_ONLY = 1,   // ignore partial modules
		SELECT_کامل_WARN = 2,   // call log_warning on partial module
		SELECT_کامل_ERR = 3,    // call log_error on partial module
		SELECT_کامل_CMDERR = 4  // call log_cmd_error on partial module
	};

	تعداد SelectBoxes : بیعلامت حرف {
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

	تعداد پورت_Dir : بیعلامت حرف  {
		PD_UNKNOWN = 0,
		PD_درون_داد = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr صحيح عرض_حد = 1 << 30;

	بنية Const;
	بنية AttrObject;
	بنية SelectedMember;
	بنية Selection;
	بنية Monitor;
	بنية Design;
	بنية Module;
	بنية Wire;
	بنية حافظه;
	بنية Cell;
	بنية SigChunk;
	بنية Sig_بیت;
	بنية SigSpecIterator;
	بنية SigSpecConstIterator;
	بنية SigSpec;
	بنية Case_قاعده;
	بنية Switch_قاعده;
	بنية MemWriteAction;
	بنية Sync_قاعده;
	بنية Process;

	تعريفنوع std::pair<SigSpec, SigSpec> SigSig;
};

بنية SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_پیشوند ID; }
namespace RTLIL { using YOSYS_NAMESPACE_پیشوند IdString; }

namespace RTLIL {
	فراغ copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			ثابت dict<IdString, RTLIL::Const> &src,
			ثابت RTLIL::Design *src_design, RTLIL::Design *dst_design);

	خارجي dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	ثابت pool<IdString> &builtin_ff_cell_types();

	ساكن مضمن std::string escape_id(ثابت std::string &str) {
		إذا (str.اندازه() > 0 && str[0] != '\\' && str[0] != '$')
			أرجع "\\" + str;
		أرجع str;
	}

	ساكن مضمن std::string unescape_id(ثابت std::string &str) {
		إذا (str.اندازه() < 2)
			أرجع str;
		إذا (str[0] != '\\')
			أرجع str;
		إذا (str[1] == '$' || str[1] == '\\')
			أرجع str;
		إذا (str[1] >= '0' && str[1] <= '9')
			أرجع str;
		أرجع str.substr(1);
	}

	قالب <typename T> using مرتب_سازی_by_name_id = IdString::compare_ptr_by_name<T>;

	قالب <typename T> بنية مرتب_سازی_by_name_str {
		bool عملگر()(T *a, T *b) ثابت {
			أرجع a->name.lt_by_name(b->name);
		}
	};

	بنية مرتب_سازی_by_id_str {
		ثابت TwinePool& pool;
		explicit مرتب_سازی_by_id_str(ثابت TwinePool& pool) : pool(pool) {}
		bool عملگر()(RTLIL::IdString a, RTLIL::IdString b) ثابت {
			أرجع pool.compare_by_name(a, b) < 0;
		}
	};

	ساكن مضمن std::string encode_filename(ثابت std::string &filename)
	{
		std::stringstream val;
		إذا (!std::any_از(filename.begin(), filename.انتها(), [](حرف c) {
			أرجع static_cast<بیعلامت حرف>(c) < 33 || static_cast<بیعلامت حرف>(c) > 126;
		})) أرجع filename;
		لكل (بیعلامت حرف ثابت c : filename) {
			إذا (c < 33 || c > 126)
				val << stringf("$%02x", c);
			وإلا
				val << c;
		}
		أرجع val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_and         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_or          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_xor         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_xnor        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);

	RTLIL::Const const_reduce_and  (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_reduce_or   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_reduce_xor  (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_reduce_xnor (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_reduce_bool (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);

	RTLIL::Const const_logic_not   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_logic_and   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_logic_or    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);

	RTLIL::Const const_shl         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_shr         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_sshl        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_sshr        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_لغزش       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_shiftx      (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);

	RTLIL::Const const_lt          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_le          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_eq          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_ne          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_eqx         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_nex         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_ge          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_gt          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);

	RTLIL::Const const_جمع_کن         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_sub         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_mul         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_div         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_divfloor    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_modfloor    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_mod         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_pow         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);

	RTLIL::Const const_pos         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_buf         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);
	RTLIL::Const const_neg         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجه_len);

	RTLIL::Const const_mux         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	قالب<typename T, typename کلید_2 = IdString>
	بنية ObjIterator {
		using iterator_category = std::forward_iterator_برچسب;
		using مقدار_type = T;
		using difference_type = ptrdiff_t;
		using اشاره_گر = T*;
		using reference = T&;
		typename dict<کلید_2, T>::iterator it;
		dict<کلید_2, T> *list_p;
		صحيح *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, صحيح *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			إذا (list_p->خالی()) {
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

		ObjIterator &عملگر=(ثابت RTLIL::ObjIterator<T, کلید_2> &other) {
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

		مضمن T عملگر*() ثابت {
			log_assert(list_p != nullptr);
			أرجع it->دومی;
		}

		مضمن bool عملگر!=(ثابت RTLIL::ObjIterator<T, کلید_2> &other) ثابت {
			إذا (list_p == nullptr || other.list_p == nullptr)
				أرجع list_p != other.list_p;
			أرجع it != other.it;
		}


		مضمن bool عملگر==(ثابت RTLIL::ObjIterator<T, کلید_2> &other) ثابت {
			أرجع !(*this != other);
		}

		مضمن ObjIterator<T, کلید_2>& عملگر++() {
			log_assert(list_p != nullptr);
			إذا (++it == list_p->انتها()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			أرجع *this;
		}

		مضمن ObjIterator<T, کلید_2>& عملگر+=(صحيح amt) {
			log_assert(list_p != nullptr);
			it += amt;
			إذا (it == list_p->انتها()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			أرجع *this;
		}

		مضمن ObjIterator<T, کلید_2> عملگر+(صحيح amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, کلید_2> new_obj(*this);
			new_obj.it += amt;
			إذا (new_obj.it == list_p->انتها()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			أرجع new_obj;
		}

		مضمن ثابت ObjIterator<T, کلید_2> عملگر++(صحيح) {
			ObjIterator<T, کلید_2> نتیجه(*this);
			++(*this);
			أرجع نتیجه;
		}
	};

	قالب<typename T, typename کلید_2 = IdString>
	بنية ObjRange
	{
		dict<کلید_2, T> *list_p;
		صحيح *refcount_p;

		ObjRange(decltype(list_p) list_p, صحيح *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, کلید_2> begin() { أرجع RTLIL::ObjIterator<T, کلید_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, کلید_2> انتها() { أرجع RTLIL::ObjIterator<T, کلید_2>(); }

		اندازه_t اندازه() ثابت {
			أرجع list_p->اندازه();
		}

		عملگر pool<T>() ثابت {
			pool<T> نتیجه;
			لكل (خودکار &it : *list_p)
				نتیجه.درج(it.دومی);
			أرجع نتیجه;
		}

		عملگر std::وکتور<T>() ثابت {
			std::وکتور<T> نتیجه;
			نتیجه.reserve(list_p->اندازه());
			لكل (خودکار &it : *list_p)
				نتیجه.افزودن_پشت(it.دومی);
			أرجع نتیجه;
		}

		pool<T> مقصد_pool() ثابت { أرجع *this; }
		std::وکتور<T> مقصد_وکتور() ثابت { أرجع *this; }
	};
};

بنية RTLIL::Const
{
	کوتاه صحيح flags;
خاص:
	دوست class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::وکتور<RTLIL::State>;
	تعداد class backing_برچسب: bool { بیتها, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_برچسب برچسب;
	اتحاد {
		bitvectype بیتها_2;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_بیتها() ثابت { أرجع برچسب == backing_برچسب::بیتها; }
	bool is_str() ثابت { أرجع برچسب == backing_برچسب::string; }

	bitvectype* get_if_بیتها() { أرجع is_بیتها() ? &بیتها_2 : عدم; }
	std::string* get_if_str() { أرجع is_str() ? &str_ : عدم; }
	ثابت bitvectype* get_if_بیتها() ثابت { أرجع is_بیتها() ? &بیتها_2 : عدم; }
	ثابت std::string* get_if_str() ثابت { أرجع is_str() ? &str_ : عدم; }

	bitvectype& get_بیتها();
	std::string& get_str();
	ثابت bitvectype& get_بیتها() ثابت;
	ثابت std::string& get_str() ثابت;
	std::وکتور<RTLIL::State>& بیتها_داخلی();
	فراغ bitvectorize_داخلی();

عام:
	Const() : flags(RTLIL::CONST_FLAG_NONE), برچسب(backing_برچسب::بیتها), بیتها_2(std::وکتور<RTLIL::State>()) {}
	Const(std::string str);
	Const(دراز دراز val); // default width is 32
	Const(دراز دراز val, صحيح عرض);
	Const(RTLIL::State بیت, صحيح عرض = 1);
	Const(std::وکتور<RTLIL::State> بیتها) : flags(RTLIL::CONST_FLAG_NONE), برچسب(backing_برچسب::بیتها), بیتها_2(std::move(بیتها)) {}
	Const(ثابت std::وکتور<bool> &بیتها);
	Const(ثابت RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &عملگر =(ثابت RTLIL::Const &other);
	~Const();

	بنية Builder
	{
		Builder() {}
		Builder(صحيح expected_عرض) { بیتها.reserve(expected_عرض); }
		فراغ افزودن_پشت(RTLIL::State b) { بیتها.افزودن_پشت(b); }
		صحيح اندازه() ثابت { أرجع static_cast<صحيح>(بیتها.اندازه()); }
		Const build() { أرجع Const(std::move(بیتها)); }
	خاص:
		std::وکتور<RTLIL::State> بیتها;
	};

	bool عملگر <(ثابت RTLIL::Const &other) ثابت;
	bool عملگر ==(ثابت RTLIL::Const &other) ثابت;
	bool عملگر !=(ثابت RTLIL::Const &other) ثابت;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::وکتور<RTLIL::State>& بیتها() { أرجع بیتها_داخلی(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	فراغ bitvectorize() ثابت { const_cast<Const*>(this)->bitvectorize_داخلی(); }

	bool as_bool() ثابت;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	صحيح as_int(bool is_signed = false) ثابت;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_مقصد_int(bool is_signed = false) ثابت;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<صحيح> try_as_int(bool is_signed = false) ثابت;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	صحيح as_int_saturating(bool is_signed = false) ثابت;

	فراغ برچسب_bare_integer_const(ثابت std::string &مقدار);

	std::string as_string(ثابت حرف* any = "-") ثابت;
	ساكن Const from_string(ثابت std::string &str);
	std::وکتور<RTLIL::State> مقصد_بیتها() ثابت;

	std::string decode_string() ثابت;
	صحيح اندازه() ثابت;
	bool خالی() ثابت;

	فراغ append(ثابت RTLIL::Const &other);
	فراغ مجموعه(صحيح i, RTLIL::State state) {
		بیتها_داخلی()[i] = state;
	}
	فراغ resize(صحيح اندازه, RTLIL::State fill) {
    log_assert(اندازه >= 0 && اندازه < RTLIL::عرض_حد);
		بیتها_داخلی().resize(اندازه, fill);
	}

	class const_iterator {
	خاص:
		ثابت Const* والد;
		اندازه_t idx;

	عام:
		using iterator_category = std::bidirectional_iterator_برچسب;
		using مقدار_type = State;
		using difference_type = std::ptrdiff_t;
		using اشاره_گر = ثابت State*;
		using reference = ثابت State&;

		const_iterator(ثابت Const& c, اندازه_t i) : والد(&c), idx(i) {}

		State عملگر*() ثابت;

		const_iterator& عملگر++() { ++idx; أرجع *this; }
		const_iterator& عملگر--() { --idx; أرجع *this; }
		const_iterator عملگر++(صحيح) { const_iterator نتیجه(*this); ++idx; أرجع نتیجه; }
		const_iterator عملگر--(صحيح) { const_iterator نتیجه(*this); --idx; أرجع نتیجه; }
		const_iterator& عملگر+=(صحيح i) { idx += i; أرجع *this; }

		const_iterator عملگر+(صحيح جمع_کن) {
			أرجع const_iterator(*والد, idx + جمع_کن);
		}
		const_iterator عملگر-(صحيح sub) {
			أرجع const_iterator(*والد, idx - sub);
		}
		صحيح عملگر-(ثابت const_iterator& other) {
			أرجع idx - other.idx;
		}

		bool عملگر==(ثابت const_iterator& other) ثابت {
			أرجع idx == other.idx;
		}

		bool عملگر!=(ثابت const_iterator& other) ثابت {
			أرجع !(*this == other);
		}
	};

	class iterator {
	خاص:
		Const* والد;
		اندازه_t idx;

	عام:
		class proxy {
		خاص:
			Const* والد;
			اندازه_t idx;
		عام:
			proxy(Const* والد, اندازه_t idx) : والد(والد), idx(idx) {}
			عملگر State() ثابت { أرجع (*والد)[idx]; }
			proxy& عملگر=(State s) { والد->مجموعه(idx, s); أرجع *this; }
			proxy& عملگر=(ثابت proxy& other) { والد->مجموعه(idx, (*other.والد)[other.idx]); أرجع *this; }
		};

		using iterator_category = std::bidirectional_iterator_برچسب;
		using مقدار_type = State;
		using difference_type = std::ptrdiff_t;
		using اشاره_گر = proxy*;
		using reference = proxy;

		iterator(Const& c, اندازه_t i) : والد(&c), idx(i) {}

		proxy عملگر*() ثابت { أرجع proxy(والد, idx); }
		iterator& عملگر++() { ++idx; أرجع *this; }
		iterator& عملگر--() { --idx; أرجع *this; }
		iterator عملگر++(صحيح) { iterator نتیجه(*this); ++idx; أرجع نتیجه; }
		iterator عملگر--(صحيح) { iterator نتیجه(*this); --idx; أرجع نتیجه; }
		iterator& عملگر+=(صحيح i) { idx += i; أرجع *this; }

		iterator عملگر+(صحيح جمع_کن) {
			أرجع iterator(*والد, idx + جمع_کن);
		}
		iterator عملگر-(صحيح sub) {
			أرجع iterator(*والد, idx - sub);
		}
		صحيح عملگر-(ثابت iterator& other) {
			أرجع idx - other.idx;
		}

		bool عملگر==(ثابت iterator& other) ثابت {
			أرجع idx == other.idx;
		}

		bool عملگر!=(ثابت iterator& other) ثابت {
			أرجع !(*this == other);
		}
	};

	const_iterator begin() ثابت {
		أرجع const_iterator(*this, 0);
	}
	const_iterator انتها() ثابت {
		أرجع const_iterator(*this, اندازه());
	}
	iterator begin() {
		أرجع iterator(*this, 0);
	}
	iterator انتها() {
		أرجع iterator(*this, اندازه());
	}
	State پشت() ثابت {
		أرجع *(انتها() - 1);
	}
	State front() ثابت {
		أرجع *begin();
	}
	State at(اندازه_t i) ثابت {
		أرجع *const_iterator(*this, i);
	}
	State عملگر[](اندازه_t i) ثابت {
		أرجع *const_iterator(*this, i);
	}

	bool is_fully_zero() ثابت;
	bool is_fully_ones() ثابت;
	bool is_fully_def() ثابت;
	bool is_fully_undef() ثابت;
	bool is_fully_undef_x_only() ثابت;
	bool is_onehot(صحيح *pos = nullptr) ثابت;

	RTLIL::Const استخراج(صحيح جابه_جایی, صحيح len = 1, RTLIL::State padding = RTLIL::State::S0) ثابت;

	// find the MSB without redundant leading bits
	صحيح get_min_اندازه(bool is_signed) ثابت;

	// compress representation to the minimum required bits
	فراغ compress(bool is_signed = false);

	std::optional<صحيح> as_int_compress(bool is_signed) ثابت;

	فراغ extu(صحيح عرض) {
		resize(عرض, RTLIL::State::S0);
	}

	فراغ exts(صحيح عرض) {
		resize(عرض, خالی() ? RTLIL::State::Sx : پشت());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> صفات;

	bool has_attribute(RTLIL::IdString id) ثابت;

	فراغ مجموعه_bool_attribute(RTLIL::IdString id, bool مقدار=true);
	bool get_bool_attribute(RTLIL::IdString id) ثابت;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ثابت {
		أرجع get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	فراغ مجموعه_string_attribute(RTLIL::IdString id, string مقدار);
	string get_string_attribute(RTLIL::IdString id) ثابت;

	فراغ مجموعه_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &داده);
	فراغ جمع_کن_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &داده);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ثابت;

	فراغ مجموعه_src_attribute(ثابت std::string &src) {
		مجموعه_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ثابت {
		أرجع get_string_attribute(ID::src);
	}

	فراغ مجموعه_hdlname_attribute(ثابت وکتور<string> &hierarchy);
	وکتور<string> get_hdlname_attribute() ثابت;

	فراغ مجموعه_intvec_attribute(RTLIL::IdString id, ثابت وکتور<صحيح> &داده);
	وکتور<صحيح> get_intvec_attribute(RTLIL::IdString id) ثابت;
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
	std::وکتور<RTLIL::State> داده; // only used if wire == NULL, LSB at index 0
	صحيح عرض, جابه_جایی;

	SigChunk() : wire(nullptr), عرض(0), جابه_جایی(0) {}
	SigChunk(ثابت RTLIL::Const &مقدار) : wire(nullptr), داده(مقدار.مقصد_بیتها()), عرض(Get_اندازه(داده)), جابه_جایی(0) {}
	SigChunk(RTLIL::Const &&مقدار) : wire(nullptr), داده(مقدار.مقصد_بیتها()), عرض(Get_اندازه(داده)), جابه_جایی(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), عرض(Get_اندازه(wire)), جابه_جایی(0) {}
	SigChunk(RTLIL::Wire *wire, صحيح جابه_جایی, صحيح عرض = 1) : wire(wire), عرض(عرض), جابه_جایی(جابه_جایی) {}
	SigChunk(ثابت std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(صحيح val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(صحيح val, صحيح عرض) : SigChunk(RTLIL::Const(val, عرض)) {}
	SigChunk(RTLIL::State بیت, صحيح عرض = 1) : SigChunk(RTLIL::Const(بیت, عرض)) {}
	SigChunk(ثابت RTLIL::Sig_بیت &بیت);

	RTLIL::SigChunk استخراج(صحيح جابه_جایی, صحيح درازی) ثابت;
	RTLIL::Sig_بیت عملگر[](صحيح جابه_جایی) ثابت;
	مضمن صحيح اندازه() ثابت { أرجع عرض; }
	مضمن bool is_wire() ثابت { أرجع wire != عدم; }

	bool عملگر <(ثابت RTLIL::SigChunk &other) ثابت;
	bool عملگر ==(ثابت RTLIL::SigChunk &other) ثابت;
	bool عملگر !=(ثابت RTLIL::SigChunk &other) ثابت;
};

بنية RTLIL::Sig_بیت
{
	RTLIL::Wire *wire;
	اتحاد {
		RTLIL::State داده; // used if wire == NULL
		صحيح جابه_جایی;        // used if wire != NULL
	};

	Sig_بیت();
	Sig_بیت(RTLIL::State بیت);
	explicit Sig_بیت(bool بیت);
	Sig_بیت(RTLIL::Wire *wire);
	Sig_بیت(RTLIL::Wire *wire, صحيح جابه_جایی);
	Sig_بیت(ثابت RTLIL::SigChunk &chunk);
	Sig_بیت(ثابت RTLIL::SigChunk &chunk, صحيح شاخص);
	Sig_بیت(ثابت RTLIL::SigSpec &sig);
	Sig_بیت(ثابت RTLIL::Sig_بیت &sigbit) = افتراضي;
	RTLIL::Sig_بیت &عملگر =(ثابت RTLIL::Sig_بیت &other) = افتراضي;

	مضمن bool is_wire() ثابت { أرجع wire != عدم; }

	bool عملگر <(ثابت RTLIL::Sig_بیت &other) ثابت;
	bool عملگر ==(ثابت RTLIL::Sig_بیت &other) ثابت;
	bool عملگر !=(ثابت RTLIL::Sig_بیت &other) ثابت;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
	[[nodiscard]] Hasher hash_بالاترین() ثابت;
};

namespace hashlib {
	قالب <>
	بنية hash_ops<RTLIL::Sig_بیت> {
		ساكن مضمن bool cmp(ثابت RTLIL::Sig_بیت &a, ثابت RTLIL::Sig_بیت &b) {
			أرجع a == b;
		}
		[[nodiscard]] ساكن مضمن Hasher hash(ثابت RTLIL::Sig_بیت sb) {
			أرجع sb.hash_بالاترین();
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت RTLIL::Sig_بیت sb, Hasher h) {
			أرجع sb.hash_into(h);
		}
	};
};

بنية RTLIL::SigSpecIterator
{
	تعريفنوع std::درون_داد_iterator_برچسب iterator_category;
	تعريفنوع RTLIL::Sig_بیت مقدار_type;
	تعريفنوع ptrdiff_t difference_type;
	تعريفنوع RTLIL::Sig_بیت* اشاره_گر;
	تعريفنوع RTLIL::Sig_بیت& reference;

	RTLIL::SigSpec *sig_p;
	صحيح شاخص;

	مضمن RTLIL::Sig_بیت &عملگر*() ثابت;
	مضمن bool عملگر!=(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع شاخص != other.شاخص; }
	مضمن bool عملگر==(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع شاخص == other.شاخص; }
	مضمن فراغ عملگر++() { شاخص++; }
};

بنية RTLIL::SigSpecConstIterator
{
	تعريفنوع std::درون_داد_iterator_برچسب iterator_category;
	تعريفنوع RTLIL::Sig_بیت مقدار_type;
	تعريفنوع ptrdiff_t difference_type;
	تعريفنوع RTLIL::Sig_بیت* اشاره_گر;
	تعريفنوع RTLIL::Sig_بیت& reference;

	ثابت RTLIL::SigSpec *sig_p;
	RTLIL::Sig_بیت بیت;
	صحيح شاخص;

	مضمن ثابت RTLIL::Sig_بیت &عملگر*();
	مضمن bool عملگر!=(ثابت RTLIL::SigSpecConstIterator &other) ثابت { أرجع شاخص != other.شاخص; }
	مضمن bool عملگر==(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع شاخص == other.شاخص; }
	مضمن فراغ عملگر++() { شاخص++; }
};

بنية RTLIL::SigSpec
{
خاص:
	دوست class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, استخراج_2);
	تعداد Representation : حرف {
		CHUNK,
		بیتها_3,
	};
	// An AtomicHash is either clear or a nonzero integer.
	بنية AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ثابت AtomicHash &rhs) : atomic_(rhs.بارگیری()) {}
		AtomicHash &عملگر=(ثابت AtomicHash &rhs) { store(rhs.بارگیری()); أرجع *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> اقرأ() ثابت {
			Hasher::hash_t مقدار = بارگیری();
			إذا (مقدار == 0)
				أرجع std::nullopt;
			أرجع مقدار;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		فراغ مجموعه(Hasher::hash_t مقدار) ثابت {
			log_assert(مقدار != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(مقدار, std::حافظه_ترتیب_relaxed);
			log_assert(old == 0 || old == مقدار);
		}
		فراغ clear() { store(0); }
	خاص:
		صحيح بارگیری() ثابت { أرجع atomic_.بارگیری(std::حافظه_ترتیب_relaxed); }
		فراغ store(Hasher::hash_t مقدار) ثابت {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(مقدار, std::حافظه_ترتیب_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	اتحاد {
		RTLIL::SigChunk chunk_;
		std::وکتور<RTLIL::Sig_بیت> بیتها_2; // LSB at index 0
	};

	فراغ init_خالی_بیتها() {
		rep_ = بیتها_3;
		new (&بیتها_2) std::وکتور<RTLIL::Sig_بیت>;
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
			بیتها_2.~وکتور();
	}
	دوست بنية Chunks;

عام:
	SigSpec() { init_خالی_بیتها(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ثابت SigSpec &مقدار) : rep_(مقدار.rep_), hash_(مقدار.hash_) {
		إذا (مقدار.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(مقدار.chunk_);
		وإلا
			new (&بیتها_2) std::وکتور<RTLIL::Sig_بیت>(مقدار.بیتها_2);
	}
	SigSpec(SigSpec &&مقدار) : rep_(مقدار.rep_), hash_(مقدار.hash_) {
		إذا (مقدار.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(مقدار.chunk_));
		وإلا
			new (&بیتها_2) std::وکتور<RTLIL::Sig_بیت>(std::move(مقدار.بیتها_2));
	}
	SigSpec(ثابت RTLIL::Const &مقدار);
	SigSpec(RTLIL::Const &&مقدار);
	SigSpec(ثابت RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, صحيح جابه_جایی, صحيح عرض = 1);
	SigSpec(ثابت std::string &str);
	SigSpec(صحيح val, صحيح عرض = 32);
	SigSpec(RTLIL::State بیت, صحيح عرض = 1);
	SigSpec(ثابت RTLIL::Sig_بیت &بیت, صحيح عرض = 1);
	SigSpec(ثابت std::وکتور<RTLIL::SigChunk> &chunks);
	SigSpec(ثابت std::وکتور<RTLIL::Sig_بیت> &بیتها);
	SigSpec(ثابت pool<RTLIL::Sig_بیت> &بیتها);
	SigSpec(ثابت std::مجموعه<RTLIL::Sig_بیت> &بیتها);
	explicit SigSpec(bool بیت);
	~SigSpec() {
		destroy();
	}

	SigSpec &عملگر=(ثابت SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		إذا (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		وإلا
			new (&بیتها_2) std::وکتور<RTLIL::Sig_بیت>(rhs.بیتها_2);
		أرجع *this;
	}
	SigSpec &عملگر=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		إذا (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		وإلا
			new (&بیتها_2) std::وکتور<RTLIL::Sig_بیت>(std::move(rhs.بیتها_2));
		أرجع *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	بنية Chunks {
		Chunks(ثابت SigSpec &spec) : spec(spec) {}
		بنية const_iterator {
			using iterator_category = std::forward_iterator_برچسب;
			using مقدار_type = ثابت SigChunk &;
			using difference_type = std::ptrdiff_t;
			using اشاره_گر = ثابت SigChunk *;
			using reference = ثابت SigChunk &;

			ثابت SigSpec &spec;
			صحيح بیت_شاخص;
			SigChunk chunk;

			const_iterator(ثابت SigSpec &spec) : spec(spec) {
				بیت_شاخص = 0;
				إذا (spec.rep_ == بیتها_3)
					بعدی_chunk_بیتها();
			}
			تعداد انتها_2 { انتها_3 };
			const_iterator(ثابت SigSpec &spec, انتها_2) : spec(spec) {
				بیت_شاخص = spec.اندازه();
			}
			فراغ بعدی_chunk_بیتها();

			ثابت SigChunk &عملگر*() {
				إذا (spec.rep_ == CHUNK)
					أرجع spec.chunk_;
				أرجع chunk;
			};
			ثابت SigChunk *عملگر->() { أرجع &**this; }
			const_iterator &عملگر++() {
				بیت_شاخص += (**this).عرض;
				إذا (spec.rep_ == بیتها_3)
					بعدی_chunk_بیتها();
				أرجع *this;
			}
			bool عملگر==(ثابت const_iterator &rhs) ثابت { أرجع بیت_شاخص == rhs.بیت_شاخص; }
			bool عملگر!=(ثابت const_iterator &rhs) ثابت { أرجع !(*this == rhs); }
		};
		const_iterator begin() ثابت { أرجع const_iterator(spec); }
		const_iterator انتها() ثابت {
			const_iterator it(spec, const_iterator::انتها_3);
			أرجع it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::وکتور<RTLIL::SigChunk>::const_معکوس_iterator rbegin() {
			ensure_chunk_وکتور();
			أرجع chunk_وکتور.rbegin();
		}
		std::وکتور<RTLIL::SigChunk>::const_معکوس_iterator rend() {
			ensure_chunk_وکتور();
			أرجع chunk_وکتور.rend();
		}
		صحيح اندازه() {
			ensure_chunk_وکتور();
			أرجع chunk_وکتور.اندازه();
		}
		صحيح اندازه() ثابت {
			أرجع std::فاصله(begin(), انتها());
		}
		ثابت SigChunk &at(صحيح شاخص) {
			ensure_chunk_وکتور();
			أرجع chunk_وکتور.at(شاخص);
		}
		عملگر ثابت std::وکتور<RTLIL::SigChunk>&() {
			ensure_chunk_وکتور();
			أرجع chunk_وکتور;
		}
	خاص:
		فراغ ensure_chunk_وکتور() {
			إذا (spec.اندازه() > 0 && chunk_وکتور.خالی()) {
				لكل (ثابت RTLIL::SigChunk &c : *this)
					chunk_وکتور.افزودن_پشت(c);
			}
		}
		ثابت SigSpec &spec;
		std::وکتور<RTLIL::SigChunk> chunk_وکتور;
	};
	دوست بنية Chunks::const_iterator;

	مضمن Chunks chunks() ثابت { أرجع {*this}; }
	مضمن ثابت SigSpec &بیتها() ثابت { أرجع *this; }

	مضمن صحيح اندازه() ثابت { أرجع rep_ == CHUNK ? chunk_.عرض : Get_اندازه(بیتها_2); }
	مضمن bool خالی() ثابت { أرجع اندازه() == 0; };

	مضمن RTLIL::Sig_بیت &عملگر[](صحيح شاخص) { inline_unpack(); hash_.clear(); أرجع بیتها_2.at(شاخص); }
	مضمن RTLIL::Sig_بیت عملگر[](صحيح شاخص) ثابت {
		إذا (rep_ == CHUNK) {
			إذا (شاخص < 0 || شاخص >= chunk_.عرض)
				throw std::بیرون_از_range("SigSpec::operator[]");
			إذا (chunk_.wire)
				أرجع RTLIL::Sig_بیت(chunk_.wire, chunk_.جابه_جایی + شاخص);
			أرجع RTLIL::Sig_بیت(chunk_.داده[شاخص]);
		}
		أرجع بیتها_2.at(شاخص);
	}

	مضمن RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.شاخص = 0; أرجع it; }
	مضمن RTLIL::SigSpecIterator انتها() { RTLIL::SigSpecIterator it; it.sig_p = this; it.شاخص = اندازه(); أرجع it; }

	مضمن RTLIL::SigSpecConstIterator begin() ثابت { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.شاخص = 0; أرجع it; }
	مضمن RTLIL::SigSpecConstIterator انتها() ثابت { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.شاخص = اندازه(); أرجع it; }

	فراغ مرتب_سازی();
	فراغ مرتب_سازی_and_unify();

	فراغ replace(ثابت RTLIL::SigSpec &الگو, ثابت RTLIL::SigSpec &with);
	فراغ replace(ثابت RTLIL::SigSpec &الگو, ثابت RTLIL::SigSpec &with, RTLIL::SigSpec *other) ثابت;

	فراغ replace(ثابت dict<RTLIL::Sig_بیت, RTLIL::Sig_بیت> &قواعد);
	فراغ replace(ثابت dict<RTLIL::Sig_بیت, RTLIL::Sig_بیت> &قواعد, RTLIL::SigSpec *other) ثابت;

	فراغ replace(ثابت std::map<RTLIL::Sig_بیت, RTLIL::Sig_بیت> &قواعد);
	فراغ replace(ثابت std::map<RTLIL::Sig_بیت, RTLIL::Sig_بیت> &قواعد, RTLIL::SigSpec *other) ثابت;

	فراغ replace(صحيح جابه_جایی, ثابت RTLIL::SigSpec &with);

	فراغ remove(ثابت RTLIL::SigSpec &الگو);
	فراغ remove(ثابت RTLIL::SigSpec &الگو, RTLIL::SigSpec *other) ثابت;
	فراغ remove2(ثابت RTLIL::SigSpec &الگو, RTLIL::SigSpec *other);

	فراغ remove(ثابت pool<RTLIL::Sig_بیت> &الگو);
	فراغ remove(ثابت pool<RTLIL::Sig_بیت> &الگو, RTLIL::SigSpec *other) ثابت;
	فراغ remove2(ثابت pool<RTLIL::Sig_بیت> &الگو, RTLIL::SigSpec *other);
	فراغ remove2(ثابت std::مجموعه<RTLIL::Sig_بیت> &الگو, RTLIL::SigSpec *other);
	فراغ remove2(ثابت pool<RTLIL::Wire*> &الگو, RTLIL::SigSpec *other);

	فراغ remove(صحيح جابه_جایی, صحيح درازی = 1);
	فراغ remove_const();

	RTLIL::SigSpec استخراج(ثابت RTLIL::SigSpec &الگو, ثابت RTLIL::SigSpec *other = عدم) ثابت;
	RTLIL::SigSpec استخراج(ثابت pool<RTLIL::Sig_بیت> &الگو, ثابت RTLIL::SigSpec *other = عدم) ثابت;
	RTLIL::SigSpec استخراج(صحيح جابه_جایی, صحيح درازی = 1) ثابت;
	RTLIL::SigSpec استخراج_انتها(صحيح جابه_جایی) ثابت { أرجع استخراج(جابه_جایی, اندازه() - جابه_جایی); }

	فراغ rewrite_wires(std::function<فراغ(RTLIL::Wire*& wire)> rewrite);

	RTLIL::Sig_بیت lsb() ثابت { log_assert(اندازه()); أرجع (*this)[0]; };
	RTLIL::Sig_بیت msb() ثابت { log_assert(اندازه()); أرجع (*this)[اندازه() - 1]; };
	RTLIL::Sig_بیت front() ثابت { أرجع (*this)[0]; }
	RTLIL::Sig_بیت پشت() ثابت { أرجع (*this)[اندازه() - 1]; }

	فراغ append(ثابت RTLIL::SigSpec &إشارة);
	مضمن فراغ append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	مضمن فراغ append(ثابت RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	مضمن فراغ append(ثابت RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	فراغ append(ثابت RTLIL::Sig_بیت &بیت);
	مضمن فراغ append(RTLIL::State state) { append(RTLIL::Sig_بیت(state)); }
	مضمن فراغ append(bool bool_) { append(RTLIL::Sig_بیت(bool_)); }

	فراغ extend_u0(صحيح عرض, bool is_signed = false);

	RTLIL::SigSpec repeat(صحيح رقم) ثابت;

	فراغ معکوس() { inline_unpack(); std::معکوس(بیتها_2.begin(), بیتها_2.انتها()); }

	bool عملگر <(ثابت RTLIL::SigSpec &other) ثابت;
	bool عملگر ==(ثابت RTLIL::SigSpec &other) ثابت;
	مضمن bool عملگر !=(ثابت RTLIL::SigSpec &other) ثابت { أرجع !(*this == other); }

	bool is_wire() ثابت;
	bool is_chunk() ثابت;
	مضمن bool is_بیت() ثابت { أرجع اندازه() == 1; }

	bool معلوم_driver() ثابت;

	bool is_fully_const() ثابت;
	bool is_fully_zero() ثابت;
	bool is_fully_ones() ثابت;
	bool is_fully_def() ثابت;
	bool is_fully_undef() ثابت;
	bool has_const() ثابت;
	bool has_const(State state) ثابت;
	bool has_marked_بیتها() ثابت;
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
	bool convertible_مقصد_int(bool is_signed = false) ثابت;

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
	RTLIL::Sig_بیت as_بیت() ثابت;

	bool تطابق(ثابت حرف* الگو) ثابت;

	std::مجموعه<RTLIL::Sig_بیت> مقصد_sigbit_مجموعه() ثابت;
	pool<RTLIL::Sig_بیت> مقصد_sigbit_pool() ثابت;
	std::وکتور<RTLIL::Sig_بیت> مقصد_sigbit_وکتور() ثابت;
	std::map<RTLIL::Sig_بیت, RTLIL::Sig_بیت> مقصد_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت;
	dict<RTLIL::Sig_بیت, RTLIL::Sig_بیت> مقصد_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت;

	ساكن bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	ساكن bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	ساكن bool parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	عملگر std::وکتور<RTLIL::SigChunk>() ثابت;
	عملگر std::وکتور<RTLIL::Sig_بیت>() ثابت { أرجع مقصد_sigbit_وکتور(); }
	ثابت RTLIL::Sig_بیت &at(صحيح جابه_جایی, ثابت RTLIL::Sig_بیت &defval) { أرجع جابه_جایی < اندازه() ? (*this)[جابه_جایی] : defval; }
	RTLIL::Sig_بیت& at(صحيح جابه_جایی) { أرجع (*this)[جابه_جایی]; }
	RTLIL::Sig_بیت at(صحيح جابه_جایی) ثابت { أرجع (*this)[جابه_جایی]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		Hasher::hash_t val;
		إذا (std::optional<Hasher::hash_t> current = hash_.اقرأ())
			val = *current;
		وإلا
			val = updhash();
		h.eat(val);
		أرجع h;
	}

#اگر_تعریف_نشده NDEBUG
	فراغ check(ثابت Module *mod = nullptr) ثابت;
#وإلا
	فراغ check(ثابت Module *mod = nullptr) ثابت { (فراغ)mod; }
#پایان_اگر
};

بنية RTLIL::Selection
{
	// selection includes boxed modules
	bool selects_boxes;
	// selection covers full design, including boxed modules
	bool complete_selection;
	// selection covers full design, not including boxed modules
	bool پر_selection;
	pool<RTLIL::IdString> selected_modules;
	dict<RTLIL::IdString, pool<RTLIL::IdString>> selected_members;
	RTLIL::Design *current_design;

	// create a new selection
	Selection(
		// should the selection cover the full design
		bool پر_2 = true,
		// should the selection include boxed modules
		bool boxes = false,
		// the design to select from
		RTLIL::Design *design = nullptr
	) :
		selects_boxes(boxes), complete_selection(پر_2 && boxes), پر_selection(پر_2 && !boxes), current_design(design) { }

	// checks if the given module exists in the current design and is a
	// boxed module, warning the user if the current design is not set
	bool boxed_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_کامل_module(RTLIL::IdString mod_name) ثابت;

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
		أرجع پر_selection || complete_selection;
	}

	// add whole module to this selection
	قالب<typename T1> فراغ select(T1 *module) {
		إذا (!selects_all() && selected_modules.شمار(module->name) == 0) {
			IdString name = module->name;
			selected_modules.درج(name);
			selected_members.erase(name);
			إذا (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	قالب<typename T1, typename T2> فراغ select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	قالب<typename T1> فراغ select(T1 *module, RTLIL::IdString member) {
		إذا (!selects_all() && selected_modules.شمار(module->name.ref()) == 0) {
			selected_members[module->name.ref()].درج(member);
			إذا (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool خالی() ثابت {
		أرجع !selects_all() && selected_modules.خالی() && selected_members.خالی();
	}

	// clear this selection, leaving it empty
	فراغ clear();

	// create a new selection which is empty
	ساكن Selection خالی_Selection(RTLIL::Design *design = nullptr) { أرجع Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	ساكن Selection پر_Selection(RTLIL::Design *design = nullptr) { أرجع Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	ساكن Selection CompleteSelection(RTLIL::Design *design = nullptr) { أرجع Selection(true, true, design); };
};

بنية RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

	Monitor() {
		ساكن بیعلامت صحيح hashidx_شمار = 123456789;
		hashidx_شمار = mkhash_xorshift(hashidx_شمار);
		hashidx_ = hashidx_شمار;
	}

	مجازی ~Monitor() { }
	مجازی فراغ notify_module_جمع_کن(RTLIL::Module*) { }
	مجازی فراغ notify_module_del(RTLIL::Module*) { }
	مجازی فراغ notify_connect(RTLIL::Cell*, RTLIL::IdString, ثابت RTLIL::SigSpec&, ثابت RTLIL::SigSpec&) { }
	مجازی فراغ notify_connect(RTLIL::Module*, ثابت RTLIL::SigSig&) { }
	مجازی فراغ notify_connect(RTLIL::Module*, ثابت std::وکتور<RTLIL::SigSig>&) { }
	مجازی فراغ notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
بنية define_map_t;

قالب<typename N>
مضمن constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::برگ> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ثابت حرف*> || std::is_same_v<std::decay_t<N>, حرف*>;

#تعریف_کن YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, صحيح> = 0

قالب<typename N>
مضمن constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ثابت حرف *> || std::is_same_v<std::decay_t<N>, حرف *>;

#تعریف_کن YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, صحيح> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#تعریف_کن YS_NAME_FWD_POOL(_func, _pool) \
	قالب<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(خودکار) _func(N name, Rest&&... rest) \
		{ أرجع _func(_pool.جمع_کن(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#تعریف_کن YS_NAME_FWD_2ND_POOL(_func, _pool) \
	قالب<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(خودکار) _func(T &&اولی, N name) \
		{ أرجع _func(std::forward<T>(اولی), _pool.جمع_کن(std::move(name))); }

#تعریف_کن YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#تعریف_کن YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

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

	اندازه_t gc_twines();

	std::وکتور<std::unique_ptr<AST::Ast_گره>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_تعریف_میکند;

	std::وکتور<RTLIL::Selection> selection_پشته;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_فعال_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ثابت RTLIL::Module *module(RTLIL::IdString name) ثابت;
	قالب<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ أرجع module(twines().پیدا_کن(std::string_view(name))); }
	قالب<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Module *module(N name) ثابت
		{ أرجع module(twines().پیدا_کن(std::string_view(name))); }
	RTLIL::Module *بالاترین_module() ثابت;

	bool has(RTLIL::IdString id) ثابت {
		twines_.check_owned(id);
		أرجع modules_.شمار(id) != 0;
	}
	قالب<typename N, YS_NAME_STRING(N)> bool has(N name) ثابت
		{ أرجع has(twines().پیدا_کن(std::string_view(name))); }

	فراغ جمع_کن(RTLIL::Module *module);

	RTLIL::Module *جمع_کن_Module(RTLIL::IdString name);
	YS_NAME_FWD_POOL(جمع_کن_Module, twines())
	فراغ remove(RTLIL::Module *module);
	فراغ rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	فراغ scratchpad_unset(ثابت std::string &varname);

	فراغ scratchpad_مجموعه_int(ثابت std::string &varname, صحيح مقدار);
	فراغ scratchpad_مجموعه_bool(ثابت std::string &varname, bool مقدار);
	فراغ scratchpad_مجموعه_string(ثابت std::string &varname, std::string مقدار);

	صحيح scratchpad_get_int(ثابت std::string &varname, صحيح default_مقدار_2 = 0) ثابت;
	bool scratchpad_get_bool(ثابت std::string &varname, bool default_مقدار_2 = false) ثابت;
	std::string scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_مقدار_2 = std::string()) ثابت;

	فراغ مرتب_سازی();
	فراغ مرتب_سازی_modules();
	فراغ check();
	فراغ optimize();

	فراغ clone_into(RTLIL::Design *dst) ثابت;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_کامل_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ثابت;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_کامل_module(RTLIL::Module *mod) ثابت;

	// push the given selection to the selection stack
	فراغ افزودن_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	فراغ افزودن_خالی_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	فراغ افزودن_پر_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	فراغ افزودن_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	فراغ برداشت_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		أرجع selection_پشته.پشت();
	}

	// get the current selection
	ثابت RTLIL::Selection &selection() ثابت {
		أرجع selection_پشته.پشت();
	}

	// is the current selection a full selection (no boxes)
	bool پر_selection() ثابت {
		أرجع selection().پر_selection;
	}

	// is the given module in the current selection
	قالب<typename T1> bool selected(T1 *module) ثابت {
		أرجع selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	قالب<typename T1, typename T2> bool selected(T1 *module, T2 *member) ثابت {
		أرجع selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	قالب<typename T1> فراغ select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	قالب<typename T1, typename T2> فراغ select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::وکتور<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) ثابت;

	// returns all selected modules, and may include boxes
	std::وکتور<RTLIL::Module*> all_selected_modules() ثابت { أرجع selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::وکتور<RTLIL::Module*> selected_unboxed_modules() ثابت { أرجع selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::وکتور<RTLIL::Module*> selected_unboxed_modules_warn() ثابت { أرجع selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::وکتور<RTLIL::Module*> selected_کامل_modules() ثابت { أرجع selected_modules(SELECT_کامل_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::وکتور<RTLIL::Module*> all_selected_کامل_modules() ثابت { أرجع selected_modules(SELECT_کامل_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::وکتور<RTLIL::Module*> selected_کامل_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ثابت { أرجع selected_modules(SELECT_کامل_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::وکتور<RTLIL::Module*> selected_unboxed_کامل_modules() ثابت { أرجع selected_modules(SELECT_کامل_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::وکتور<RTLIL::Module*> selected_unboxed_کامل_modules_warn() ثابت { أرجع selected_modules(SELECT_کامل_WARN, SB_UNBOXED_WARN); }

	ساكن std::map<بیعلامت صحيح, RTLIL::Design*> *get_all_designs(فراغ);

	std::string مقصد_rtlil_str(bool only_selected = true) ثابت;
};

بنية RTLIL::Module : عام RTLIL::AttrObject
{
	دوست بنية RTLIL::Cell;
	دوست بنية RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	فراغ جمع_کن(RTLIL::Wire *wire);
	فراغ جمع_کن(RTLIL::Cell *cell);
	فراغ جمع_کن(RTLIL::Process *process);

عام:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ثابت { أرجع design->twines(); }

	صحيح refcount_wires_;
	صحيح refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::وکتور<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_مقادیر;
	dict<RTLIL::IdString, RTLIL::حافظه*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	مجازی ~Module();
	مجازی RTLIL::IdString derive(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	مجازی RTLIL::IdString derive(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Const> &parameters, ثابت dict<RTLIL::IdString, RTLIL::Module*> &interfaces, ثابت dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	مجازی اندازه_t شمار_id(RTLIL::IdString id);
	مجازی فراغ expand_interfaces(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	مجازی bool reprocess_if_necessary(RTLIL::Design *design);

	مجازی فراغ مرتب_سازی();
	مجازی فراغ check();
	مجازی فراغ optimize();
	مجازی فراغ makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ثابت {
		أرجع get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	فراغ connect(ثابت RTLIL::SigSig &conn);
	فراغ connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs);
	فراغ new_connections(ثابت std::وکتور<RTLIL::SigSig> &new_conn);
	ثابت std::وکتور<RTLIL::SigSig> &connections() ثابت;

	std::وکتور<RTLIL::IdString> ports;
	فراغ fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_صف;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_پورت_صف;
	pool<RTLIL::Wire *> buf_norm_wire_صف;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_شاخص;
	فراغ bufNormalize();

	قالب<typename T> فراغ rewrite_sigspecs(T &functor);
	قالب<typename T> فراغ rewrite_sigspecs2(T &functor);
	فراغ cloneInto(RTLIL::Module *new_mod) ثابت;
	مجازی RTLIL::Module *clone() ثابت;
	مجازی RTLIL::Module *clone(RTLIL::Design *dst) ثابت;
	مجازی RTLIL::Module *clone(RTLIL::Design *dst, IdString هدف_name) ثابت;

	bool has_memories() ثابت;
	bool has_processes() ثابت;

	bool has_memories_warn() ثابت;
	bool has_processes_warn() ثابت;

	bool is_selected() ثابت;
	bool is_selected_کامل() ثابت;

	std::وکتور<RTLIL::Wire*> selected_wires() ثابت;
	std::وکتور<RTLIL::Cell*> selected_cells() ثابت;
	std::وکتور<RTLIL::حافظه*> selected_memories() ثابت;
	std::وکتور<RTLIL::Process*> selected_processes() ثابت;
	std::وکتور<RTLIL::SelectedMember> selected_members() ثابت;

	قالب<typename T> bool selected(T *member) ثابت {
		أرجع design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		إذا (design)
			design->twines().check_owned(id);
		خودکار it = wires_.پیدا_کن(id);
		أرجع it == wires_.انتها() ? nullptr : it->دومی;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		إذا (design)
			design->twines().check_owned(id);
		خودکار it = cells_.پیدا_کن(id);
		أرجع it == cells_.انتها() ? nullptr : it->دومی;
	}

	ثابت RTLIL::Wire* wire(RTLIL::IdString id) ثابت{
		إذا (design)
			design->twines().check_owned(id);
		خودکار it = wires_.پیدا_کن(id);
		أرجع it == wires_.انتها() ? nullptr : it->دومی;
	}
	ثابت RTLIL::Cell* cell(RTLIL::IdString id) ثابت {
		إذا (design)
			design->twines().check_owned(id);
		خودکار it = cells_.پیدا_کن(id);
		أرجع it == cells_.انتها() ? nullptr : it->دومی;
	}

	قالب<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ أرجع wire(twines().پیدا_کن(std::string_view(name))); }
	قالب<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ أرجع cell(twines().پیدا_کن(std::string_view(name))); }
	قالب<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Wire *wire(N name) ثابت
		{ أرجع wire(twines().پیدا_کن(std::string_view(name))); }
	قالب<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Cell *cell(N name) ثابت
		{ أرجع cell(twines().پیدا_کن(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { أرجع RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	صحيح wires_اندازه() ثابت { أرجع wires_.اندازه(); }
	RTLIL::Wire* wire_at(صحيح شاخص) ثابت { أرجع wires_.element(شاخص)->دومی; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { أرجع RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	صحيح cells_اندازه() ثابت { أرجع cells_.اندازه(); }
	RTLIL::Cell* cell_at(صحيح شاخص) ثابت { أرجع cells_.element(شاخص)->دومی; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	فراغ remove(ثابت pool<RTLIL::Wire*> &wires);
	فراغ remove(RTLIL::Cell *cell);
	فراغ remove(RTLIL::حافظه *حافظه_3);
	فراغ remove(RTLIL::Process *process);

	فراغ rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	فراغ rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	فراغ rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	فراغ جابجا_نامها(RTLIL::Wire *w1, RTLIL::Wire *w2);
	فراغ جابجا_نامها(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, صحيح &شاخص);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *جمع_کن_Wire(RTLIL::IdString name, صحيح عرض = 1);
	RTLIL::Wire *جمع_کن_Wire(RTLIL::IdString name, ثابت RTLIL::Wire *other);
	YS_NAME_FWD(جمع_کن_Wire)

	RTLIL::Cell *جمع_کن_Cell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *جمع_کن_Cell(RTLIL::IdString name, ثابت RTLIL::Cell *other);
	YS_NAME_FWD(جمع_کن_Cell)
	قالب<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *جمع_کن_Cell(RTLIL::IdString name, T type) { أرجع جمع_کن_Cell(name, design->twines().جمع_کن(std::move(type))); }

	RTLIL::حافظه *جمع_کن_حافظه(RTLIL::IdString name);
	YS_NAME_FWD(جمع_کن_حافظه)
	RTLIL::حافظه *جمع_کن_حافظه(RTLIL::IdString name, ثابت RTLIL::حافظه *other);

	RTLIL::Process *جمع_کن_Process(RTLIL::IdString name);
	YS_NAME_FWD(جمع_کن_Process)
	RTLIL::Process *جمع_کن_Process(RTLIL::IdString name, ثابت RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* جمع_کن_Not     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Pos     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Buf     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Barrier (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Neg     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_And  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Or   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Xor  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Xnor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_Reduce_And  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Reduce_Or   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Reduce_Xor  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Reduce_Xnor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Reduce_Bool (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_Shl    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Shr    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Sshl   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Sshr   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_لغزش  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Shiftx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_Lt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Le  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Eq  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Ne  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Eqx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Nex (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Ge  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Gt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_جمع_کن (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Sub (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Mul (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	// truncating division
	RTLIL::Cell* جمع_کن_Div (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	// truncating modulo
	RTLIL::Cell* جمع_کن_Mod (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Div_Floor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Mod_Floor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Pow (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_Fa (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_x, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_Logic_Not (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Logic_And (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Logic_Or  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_Mux  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Pmux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Bmux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Demux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_Bweqx  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Bwmux  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_برش  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const جابه_جایی, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Concat (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Lut    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Tribuf (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Assert (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Assume (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_زنده   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Fair   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Cover  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Equiv  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_Sr    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr, ثابت RTLIL::SigSpec &sig_q, bool مجموعه_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Ff    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Dff   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d,   ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Dffe  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Dffsr (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool مجموعه_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Dffsre (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool مجموعه_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Adff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_مقدار, bool clk_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Adffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_مقدار, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Aldff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Aldffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Sdff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_مقدار, bool clk_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Sdffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_مقدار, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Sdffce (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_مقدار, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Dlatch (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Adlatch (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_مقدار, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Dlatchsr (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, bool مجموعه_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_Buf_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Not_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_And_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Nand_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Or_Gate     (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Nor_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Xor_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Xnor_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Andnot_Gate (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Ornot_Gate  (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Mux_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_s, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Nmux_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_s, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Aoi3_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_c, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Oai3_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_c, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Aoi4_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_c, ثابت RTLIL::Sig_بیت &sig_d, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Oai4_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_c, ثابت RTLIL::Sig_بیت &sig_d, ثابت RTLIL::Sig_بیت &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_Sr_Gate     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr,
			ثابت RTLIL::SigSpec &sig_q, bool مجموعه_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Ff_Gate     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Dff_Gate    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Dffe_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Dffsr_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool مجموعه_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Dffsre_Gate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool مجموعه_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Adff_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_مقدار = false, bool clk_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Adffe_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_مقدار = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Aldff_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Aldffe_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Sdff_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_مقدار = false, bool clk_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Sdffe_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_مقدار = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Sdffce_Gate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_مقدار = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Dlatch_Gate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Adlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_مقدار = false, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کن_Dlatchsr_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_مجموعه, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, bool مجموعه_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کن_Anyinit(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");

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
	RTLIL::SigSpec لغزش_2  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec جمع_کن_2 (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
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

	RTLIL::Sig_بیت BufGate    (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت std::string &src = "");
	RTLIL::Sig_بیت NotGate    (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت std::string &src = "");
	RTLIL::Sig_بیت AndGate    (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بیت NandGate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بیت OrGate     (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بیت NorGate    (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بیت XorGate    (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بیت XnorGate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بیت AndnotGate (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بیت OrnotGate  (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بیت MuxGate    (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_s, ثابت std::string &src = "");
	RTLIL::Sig_بیت NmuxGate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_s, ثابت std::string &src = "");
	RTLIL::Sig_بیت Aoi3Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_c, ثابت std::string &src = "");
	RTLIL::Sig_بیت Oai3Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_c, ثابت std::string &src = "");
	RTLIL::Sig_بیت Aoi4Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_c, ثابت RTLIL::Sig_بیت &sig_d, ثابت std::string &src = "");
	RTLIL::Sig_بیت Oai4Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بیت &sig_a, ثابت RTLIL::Sig_بیت &sig_b, ثابت RTLIL::Sig_بیت &sig_c, ثابت RTLIL::Sig_بیت &sig_d, ثابت std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, صحيح عرض = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, صحيح عرض = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, صحيح عرض = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, صحيح عرض = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ثابت std::string &src = "");

	RTLIL::SigSpec مجموعه_برچسب_2          (RTLIL::IdString name, ثابت std::string &برچسب, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src = "");
	RTLIL::Cell*   جمع_کن_مجموعه_برچسب       (RTLIL::IdString name, ثابت std::string &برچسب, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::SigSpec Get_برچسب          (RTLIL::IdString name, ثابت std::string &برچسب, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src = "");
	RTLIL::Cell*   جمع_کن_Overwrite_برچسب (RTLIL::IdString name, ثابت std::string &برچسب, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src = "");
	RTLIL::SigSpec Original_برچسب     (RTLIL::IdString name, ثابت std::string &برچسب, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src = "");
	YS_NAME_FWD(جمع_کن_Anyinit)
	YS_NAME_FWD(Anyconst) YS_NAME_FWD(Anyseq) YS_NAME_FWD(Allconst) YS_NAME_FWD(Allseq)
	YS_NAME_FWD(Initstate)
	YS_NAME_FWD(مجموعه_برچسب_2) YS_NAME_FWD(جمع_کن_مجموعه_برچسب) YS_NAME_FWD(Get_برچسب)
	YS_NAME_FWD(جمع_کن_Overwrite_برچسب) YS_NAME_FWD(Original_برچسب) YS_NAME_FWD(FutureFF)
	YS_NAME_FWD(جمع_کن_Not) YS_NAME_FWD(جمع_کن_Pos) YS_NAME_FWD(جمع_کن_Buf) YS_NAME_FWD(جمع_کن_Barrier) YS_NAME_FWD(جمع_کن_Neg)
	YS_NAME_FWD(جمع_کن_And) YS_NAME_FWD(جمع_کن_Or) YS_NAME_FWD(جمع_کن_Xor) YS_NAME_FWD(جمع_کن_Xnor)
	YS_NAME_FWD(جمع_کن_Reduce_And) YS_NAME_FWD(جمع_کن_Reduce_Or) YS_NAME_FWD(جمع_کن_Reduce_Xor) YS_NAME_FWD(جمع_کن_Reduce_Xnor)
	YS_NAME_FWD(جمع_کن_Reduce_Bool) YS_NAME_FWD(جمع_کن_Shl) YS_NAME_FWD(جمع_کن_Shr) YS_NAME_FWD(جمع_کن_Sshl)
	YS_NAME_FWD(جمع_کن_Sshr) YS_NAME_FWD(جمع_کن_لغزش) YS_NAME_FWD(جمع_کن_Shiftx) YS_NAME_FWD(جمع_کن_Lt)
	YS_NAME_FWD(جمع_کن_Le) YS_NAME_FWD(جمع_کن_Eq) YS_NAME_FWD(جمع_کن_Ne) YS_NAME_FWD(جمع_کن_Eqx)
	YS_NAME_FWD(جمع_کن_Nex) YS_NAME_FWD(جمع_کن_Ge) YS_NAME_FWD(جمع_کن_Gt) YS_NAME_FWD(جمع_کن_جمع_کن)
	YS_NAME_FWD(جمع_کن_Sub) YS_NAME_FWD(جمع_کن_Mul) YS_NAME_FWD(جمع_کن_Div) YS_NAME_FWD(جمع_کن_Mod)
	YS_NAME_FWD(جمع_کن_Div_Floor) YS_NAME_FWD(جمع_کن_Mod_Floor) YS_NAME_FWD(جمع_کن_Pow) YS_NAME_FWD(جمع_کن_Fa)
	YS_NAME_FWD(جمع_کن_Logic_Not) YS_NAME_FWD(جمع_کن_Logic_And) YS_NAME_FWD(جمع_کن_Logic_Or) YS_NAME_FWD(جمع_کن_Mux)
	YS_NAME_FWD(جمع_کن_Pmux) YS_NAME_FWD(جمع_کن_Bmux) YS_NAME_FWD(جمع_کن_Demux) YS_NAME_FWD(جمع_کن_Bweqx)
	YS_NAME_FWD(جمع_کن_Bwmux) YS_NAME_FWD(جمع_کن_برش) YS_NAME_FWD(جمع_کن_Concat) YS_NAME_FWD(جمع_کن_Lut)
	YS_NAME_FWD(جمع_کن_Tribuf) YS_NAME_FWD(جمع_کن_Assert) YS_NAME_FWD(جمع_کن_Assume) YS_NAME_FWD(جمع_کن_زنده)
	YS_NAME_FWD(جمع_کن_Fair) YS_NAME_FWD(جمع_کن_Cover) YS_NAME_FWD(جمع_کن_Equiv) YS_NAME_FWD(جمع_کن_Sr)
	YS_NAME_FWD(جمع_کن_Ff) YS_NAME_FWD(جمع_کن_Dff) YS_NAME_FWD(جمع_کن_Dffe) YS_NAME_FWD(جمع_کن_Dffsr)
	YS_NAME_FWD(جمع_کن_Dffsre) YS_NAME_FWD(جمع_کن_Adff) YS_NAME_FWD(جمع_کن_Adffe) YS_NAME_FWD(جمع_کن_Aldff)
	YS_NAME_FWD(جمع_کن_Aldffe) YS_NAME_FWD(جمع_کن_Sdff) YS_NAME_FWD(جمع_کن_Sdffe) YS_NAME_FWD(جمع_کن_Sdffce)
	YS_NAME_FWD(جمع_کن_Dlatch) YS_NAME_FWD(جمع_کن_Adlatch) YS_NAME_FWD(جمع_کن_Dlatchsr) YS_NAME_FWD(جمع_کن_Buf_Gate)
	YS_NAME_FWD(جمع_کن_Not_Gate) YS_NAME_FWD(جمع_کن_And_Gate) YS_NAME_FWD(جمع_کن_Nand_Gate) YS_NAME_FWD(جمع_کن_Or_Gate)
	YS_NAME_FWD(جمع_کن_Nor_Gate) YS_NAME_FWD(جمع_کن_Xor_Gate) YS_NAME_FWD(جمع_کن_Xnor_Gate) YS_NAME_FWD(جمع_کن_Andnot_Gate)
	YS_NAME_FWD(جمع_کن_Ornot_Gate) YS_NAME_FWD(جمع_کن_Mux_Gate) YS_NAME_FWD(جمع_کن_Nmux_Gate) YS_NAME_FWD(جمع_کن_Aoi3_Gate)
	YS_NAME_FWD(جمع_کن_Oai3_Gate) YS_NAME_FWD(جمع_کن_Aoi4_Gate) YS_NAME_FWD(جمع_کن_Oai4_Gate) YS_NAME_FWD(جمع_کن_Sr_Gate)
	YS_NAME_FWD(جمع_کن_Ff_Gate) YS_NAME_FWD(جمع_کن_Dff_Gate) YS_NAME_FWD(جمع_کن_Dffe_Gate) YS_NAME_FWD(جمع_کن_Dffsr_Gate)
	YS_NAME_FWD(جمع_کن_Dffsre_Gate) YS_NAME_FWD(جمع_کن_Adff_Gate) YS_NAME_FWD(جمع_کن_Adffe_Gate) YS_NAME_FWD(جمع_کن_Aldff_Gate)
	YS_NAME_FWD(جمع_کن_Aldffe_Gate) YS_NAME_FWD(جمع_کن_Sdff_Gate) YS_NAME_FWD(جمع_کن_Sdffe_Gate) YS_NAME_FWD(جمع_کن_Sdffce_Gate)
	YS_NAME_FWD(جمع_کن_Dlatch_Gate) YS_NAME_FWD(جمع_کن_Adlatch_Gate) YS_NAME_FWD(جمع_کن_Dlatchsr_Gate) YS_NAME_FWD(Not)
	YS_NAME_FWD(Pos) YS_NAME_FWD(Buf) YS_NAME_FWD(Barrier) YS_NAME_FWD(Neg) YS_NAME_FWD(And)
	YS_NAME_FWD(Or) YS_NAME_FWD(Xor) YS_NAME_FWD(Xnor) YS_NAME_FWD(ReduceAnd)
	YS_NAME_FWD(ReduceOr) YS_NAME_FWD(ReduceXor) YS_NAME_FWD(ReduceXnor) YS_NAME_FWD(ReduceBool)
	YS_NAME_FWD(Shl) YS_NAME_FWD(Shr) YS_NAME_FWD(Sshl) YS_NAME_FWD(Sshr)
	YS_NAME_FWD(لغزش_2) YS_NAME_FWD(Shiftx) YS_NAME_FWD(Lt) YS_NAME_FWD(Le)
	YS_NAME_FWD(Eq) YS_NAME_FWD(Ne) YS_NAME_FWD(Eqx) YS_NAME_FWD(Nex)
	YS_NAME_FWD(Ge) YS_NAME_FWD(Gt) YS_NAME_FWD(جمع_کن_2) YS_NAME_FWD(Sub)
	YS_NAME_FWD(Mul) YS_NAME_FWD(Div) YS_NAME_FWD(Mod) YS_NAME_FWD(DivFloor)
	YS_NAME_FWD(ModFloor) YS_NAME_FWD(Pow) YS_NAME_FWD(LogicNot) YS_NAME_FWD(LogicAnd)
	YS_NAME_FWD(LogicOr) YS_NAME_FWD(Mux) YS_NAME_FWD(Pmux) YS_NAME_FWD(Bmux)
	YS_NAME_FWD(Demux) YS_NAME_FWD(Bweqx) YS_NAME_FWD(Bwmux) YS_NAME_FWD(BufGate)
	YS_NAME_FWD(NotGate) YS_NAME_FWD(AndGate) YS_NAME_FWD(NandGate) YS_NAME_FWD(OrGate)
	YS_NAME_FWD(NorGate) YS_NAME_FWD(XorGate) YS_NAME_FWD(XnorGate) YS_NAME_FWD(AndnotGate)
	YS_NAME_FWD(OrnotGate) YS_NAME_FWD(MuxGate) YS_NAME_FWD(NmuxGate) YS_NAME_FWD(Aoi3Gate)
	YS_NAME_FWD(Oai3Gate) YS_NAME_FWD(Aoi4Gate) YS_NAME_FWD(Oai4Gate)

	std::string مقصد_rtlil_str() ثابت;
#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	ساكن std::map<بیعلامت صحيح, RTLIL::Module*> *get_all_modules(فراغ);
#پایان_اگر
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
	RTLIL::IdString driver_پورت_2;

عام:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	فراغ عملگر=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	صحيح عرض, شروع_جابه_جایی, پورت_id;
	bool پورت_درون_داد, پورت_output, upto, is_signed;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	bool معلوم_driver() ثابت { أرجع driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ثابت    { log_assert(driverCell_); أرجع driverCell_; };
	RTLIL::IdString driver_پورت() ثابت { log_assert(driverCell_); أرجع driver_پورت_2; };

	صحيح from_hdl_شاخص(صحيح hdl_شاخص) {
		صحيح zero_شاخص = hdl_شاخص - شروع_جابه_جایی;
		صحيح rtlil_شاخص = upto ? عرض - 1 - zero_شاخص : zero_شاخص;
		أرجع rtlil_شاخص >= 0 && rtlil_شاخص < عرض ? rtlil_شاخص : INT_MIN;
	}

	صحيح مقصد_hdl_شاخص(صحيح rtlil_شاخص) {
		إذا (rtlil_شاخص < 0 || rtlil_شاخص >= عرض)
			أرجع INT_MIN;
		صحيح zero_شاخص = upto ? عرض - 1 - rtlil_شاخص : rtlil_شاخص;
		أرجع zero_شاخص + شروع_جابه_جایی;
	}

	std::string مقصد_rtlil_str() ثابت;

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	ساكن std::map<بیعلامت صحيح, RTLIL::Wire*> *get_all_wires(فراغ);
#پایان_اگر
};

مضمن صحيح Get_اندازه(RTLIL::Wire *wire) {
	أرجع wire->عرض;
}

بنية RTLIL::حافظه : عام RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

	حافظه();
	~حافظه();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	RTLIL::حافظه_Name_Id name;

	صحيح عرض, شروع_جابه_جایی, اندازه;
#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	ساكن std::map<بیعلامت صحيح, RTLIL::حافظه*> *get_all_memorys(فراغ);
#پایان_اگر

	std::string مقصد_rtlil_str() ثابت;
};

بنية RTLIL::Cell : عام RTLIL::AttrObject
{
خاص:
	فراغ init_شاخص();

	bool bufnorm_handle_مجموعه_پورت(IdString portname, RTLIL::SigSpec &إشارة, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
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
	فراغ عملگر=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool has_پورت(RTLIL::IdString portname) ثابت;
	فراغ unset_پورت(RTLIL::IdString portname);
	فراغ مجموعه_پورت(RTLIL::IdString portname, RTLIL::SigSpec إشارة);
	قالب<typename N, YS_NAME_STRING(N)> فراغ مجموعه_پورت(N portname, RTLIL::SigSpec إشارة)
		{ مجموعه_پورت(module->twines().جمع_کن(std::move(portname)), std::move(إشارة)); }
	ثابت RTLIL::SigSpec &get_پورت(RTLIL::IdString portname) ثابت;
	ثابت dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ثابت;

	bool معلوم() ثابت;
	bool درون_داد(RTLIL::IdString portname) ثابت;
	bool output(RTLIL::IdString portname) ثابت;
	پورت_Dir پورت_dir(RTLIL::IdString portname) ثابت;

	bool hasParam(RTLIL::IdString paramname) ثابت;
	فراغ unsetParam(RTLIL::IdString paramname);
	فراغ مجموعه_Param(RTLIL::IdString paramname, RTLIL::Const مقدار);
	ثابت RTLIL::Const &getParam(RTLIL::IdString paramname) ثابت;

	قالب<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ثابت
		{ أرجع hasParam(module->twines().پیدا_کن(std::string_view(name))); }
	قالب<typename N, YS_NAME_STRING(N)> فراغ unsetParam(N name)
		{ unsetParam(module->twines().پیدا_کن(std::string_view(name))); }
	قالب<typename N, YS_NAME_STRING(N)> فراغ مجموعه_Param(N name, RTLIL::Const مقدار)
		{ مجموعه_Param(module->twines().جمع_کن(std::move(name)), std::move(مقدار)); }
	قالب<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Const &getParam(N name) ثابت
		{ أرجع getParam(module->twines().پیدا_کن(std::string_view(name))); }

	فراغ مرتب_سازی();
	فراغ check();
	فراغ fixup_parameters(bool مجموعه_a_signed = false, bool مجموعه_b_signed = false);

	bool has_keep_attr() ثابت;

	قالب<typename T> فراغ rewrite_sigspecs(T &functor);
	قالب<typename T> فراغ rewrite_sigspecs2(T &functor);

#اگر_تعریف_شده YOSYS_ENABLE_PYTHON
	ساكن std::map<بیعلامت صحيح, RTLIL::Cell*> *get_all_cells(فراغ);
#پایان_اگر

	bool has_memid() ثابت;
	bool is_mem_cell() ثابت;
	bool is_builtin_ff() ثابت;

	std::string مقصد_rtlil_str() ثابت;
};

بنية RTLIL::Case_قاعده : عام RTLIL::AttrObject
{
	std::وکتور<RTLIL::SigSpec> compare;
	std::وکتور<RTLIL::SigSig> actions;
	std::وکتور<RTLIL::Switch_قاعده*> switches;

	~Case_قاعده();

	bool خالی() ثابت;

	قالب<typename T> فراغ rewrite_sigspecs(T &functor);
	قالب<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Case_قاعده *clone() ثابت;
};

بنية RTLIL::Switch_قاعده : عام RTLIL::AttrObject
{
	RTLIL::SigSpec إشارة;
	std::وکتور<RTLIL::Case_قاعده*> cases;

	~Switch_قاعده();

	bool خالی() ثابت;

	قالب<typename T> فراغ rewrite_sigspecs(T &functor);
	قالب<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Switch_قاعده *clone() ثابت;
};

بنية RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec آدرس;
	RTLIL::SigSpec داده;
	RTLIL::SigSpec enable;
	RTLIL::Const اولویت_ماسک;
};

بنية RTLIL::Sync_قاعده
{
	RTLIL::SyncType type;
	RTLIL::SigSpec إشارة;
	std::وکتور<RTLIL::SigSig> actions;
	std::وکتور<RTLIL::MemWriteAction> mem_write_actions;

	قالب<typename T> فراغ rewrite_sigspecs(T &functor);
	قالب<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Sync_قاعده *clone() ثابت;
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
	RTLIL::Case_قاعده ریشه_case;
	std::وکتور<RTLIL::Sync_قاعده*> syncs;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	قالب<typename T> فراغ rewrite_sigspecs(T &functor);
	قالب<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ثابت;

	std::string مقصد_rtlil_str() ثابت;
};

مضمن RTLIL::Sig_بیت::Sig_بیت() : wire(عدم), داده(RTLIL::State::S0) { }
مضمن RTLIL::Sig_بیت::Sig_بیت(RTLIL::State بیت) : wire(عدم), داده(بیت) { }
مضمن RTLIL::Sig_بیت::Sig_بیت(bool بیت) : wire(عدم), داده(بیت ? State::S1 : State::S0) { }
مضمن RTLIL::Sig_بیت::Sig_بیت(RTLIL::Wire *wire) : wire(wire), جابه_جایی(0) { log_assert(wire && wire->عرض == 1); }
مضمن RTLIL::Sig_بیت::Sig_بیت(RTLIL::Wire *wire, صحيح جابه_جایی) : wire(wire), جابه_جایی(جابه_جایی) { log_assert(wire != nullptr); }
مضمن RTLIL::Sig_بیت::Sig_بیت(ثابت RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.عرض == 1); إذا (wire) جابه_جایی = chunk.جابه_جایی; وإلا داده = chunk.داده[0]; }
مضمن RTLIL::Sig_بیت::Sig_بیت(ثابت RTLIL::SigChunk &chunk, صحيح شاخص) : wire(chunk.wire) { إذا (wire) جابه_جایی = chunk.جابه_جایی + شاخص; وإلا داده = chunk.داده[شاخص]; }

مضمن bool RTLIL::Sig_بیت::عملگر<(ثابت RTLIL::Sig_بیت &other) ثابت {
	إذا (wire == other.wire)
		أرجع wire ? (جابه_جایی < other.جابه_جایی) : (داده < other.داده);
	إذا (wire != nullptr && other.wire != nullptr)
		أرجع wire->name < other.wire->name;
	أرجع (wire != nullptr) < (other.wire != nullptr);
}

مضمن bool RTLIL::Sig_بیت::عملگر==(ثابت RTLIL::Sig_بیت &other) ثابت {
	أرجع (wire == other.wire) && (wire ? (جابه_جایی == other.جابه_جایی) : (داده == other.داده));
}

مضمن bool RTLIL::Sig_بیت::عملگر!=(ثابت RTLIL::Sig_بیت &other) ثابت {
	أرجع (wire != other.wire) || (wire ? (جابه_جایی != other.جابه_جایی) : (داده != other.داده));
}

مضمن Hasher RTLIL::Sig_بیت::hash_into(Hasher h) ثابت {
	إذا (wire) {
		h.eat(جابه_جایی);
		h.eat(wire->name);
		أرجع h;
	}
	h.eat(داده);
	أرجع h;
}


مضمن Hasher RTLIL::Sig_بیت::hash_بالاترین() ثابت {
	Hasher h;
	إذا (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.بیتها();
		h.force(hashlib::legacy::djb2_جمع_کن(n, جابه_جایی));
		أرجع h;
	}
	h.force(داده);
	أرجع h;
}

مضمن RTLIL::Sig_بیت &RTLIL::SigSpecIterator::عملگر*() ثابت {
	أرجع (*sig_p)[شاخص];
}

مضمن ثابت RTLIL::Sig_بیت &RTLIL::SigSpecConstIterator::عملگر*() {
	بیت = (*sig_p)[شاخص];
	أرجع بیت;
}

مضمن RTLIL::Sig_بیت::Sig_بیت(ثابت RTLIL::SigSpec &sig) {
	log_assert(sig.اندازه() == 1);
	خودکار it = sig.chunks().begin();
	*this = Sig_بیت(*it);
}

قالب<typename T>
فراغ RTLIL::Module::rewrite_sigspecs(T &functor)
{
	لكل (خودکار &it : cells_)
		it.دومی->rewrite_sigspecs(functor);
	لكل (خودکار &it : processes)
		it.دومی->rewrite_sigspecs(functor);
	لكل (خودکار &it : connections_) {
		functor(it.اولی);
		functor(it.دومی);
	}
}

قالب<typename T>
فراغ RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	لكل (خودکار &it : cells_)
		it.دومی->rewrite_sigspecs2(functor);
	لكل (خودکار &it : processes)
		it.دومی->rewrite_sigspecs2(functor);
	لكل (خودکار &it : connections_) {
		functor(it.اولی, it.دومی);
	}
}

قالب<typename T>
فراغ RTLIL::Cell::rewrite_sigspecs(T &functor) {
	لكل (خودکار &it : connections_)
		functor(it.دومی);
}

قالب<typename T>
فراغ RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	لكل (خودکار &it : connections_)
		functor(it.دومی);
}

قالب<typename T>
فراغ RTLIL::Case_قاعده::rewrite_sigspecs(T &functor) {
	لكل (خودکار &it : compare)
		functor(it);
	لكل (خودکار &it : actions) {
		functor(it.اولی);
		functor(it.دومی);
	}
	لكل (خودکار it : switches)
		it->rewrite_sigspecs(functor);
}

قالب<typename T>
فراغ RTLIL::Case_قاعده::rewrite_sigspecs2(T &functor) {
	لكل (خودکار &it : compare)
		functor(it);
	لكل (خودکار &it : actions) {
		functor(it.اولی, it.دومی);
	}
	لكل (خودکار it : switches)
		it->rewrite_sigspecs2(functor);
}

قالب<typename T>
فراغ RTLIL::Switch_قاعده::rewrite_sigspecs(T &functor)
{
	functor(إشارة);
	لكل (خودکار it : cases)
		it->rewrite_sigspecs(functor);
}

قالب<typename T>
فراغ RTLIL::Switch_قاعده::rewrite_sigspecs2(T &functor)
{
	functor(إشارة);
	لكل (خودکار it : cases)
		it->rewrite_sigspecs2(functor);
}

قالب<typename T>
فراغ RTLIL::Sync_قاعده::rewrite_sigspecs(T &functor)
{
	functor(إشارة);
	لكل (خودکار &it : actions) {
		functor(it.اولی);
		functor(it.دومی);
	}
	لكل (خودکار &it : mem_write_actions) {
		functor(it.آدرس);
		functor(it.داده);
		functor(it.enable);
	}
}

قالب<typename T>
فراغ RTLIL::Sync_قاعده::rewrite_sigspecs2(T &functor)
{
	functor(إشارة);
	لكل (خودکار &it : actions) {
		functor(it.اولی, it.دومی);
	}
	لكل (خودکار &it : mem_write_actions) {
		functor(it.آدرس);
		functor(it.داده);
		functor(it.enable);
	}
}

قالب<typename T>
فراغ RTLIL::Process::rewrite_sigspecs(T &functor)
{
	ریشه_case.rewrite_sigspecs(functor);
	لكل (خودکار it : syncs)
		it->rewrite_sigspecs(functor);
}

قالب<typename T>
فراغ RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	ریشه_case.rewrite_sigspecs2(functor);
	لكل (خودکار it : syncs)
		it->rewrite_sigspecs2(functor);
}

#ضمن "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_انتها

#پایان_اگر
