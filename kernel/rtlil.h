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

#که_نه_تعریف_شوی RTLIL_H
#تعریف_کړه RTLIL_H

#ضمن "kernel/yosys_common.h"
#ضمن "kernel/yosys.h"
#ضمن "kernel/twine.h"

#ضمن <deque>
#ضمن <string_view>
#ضمن <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	تعداد State : بینښه حرف {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	تعداد SyncType : بینښه حرف {
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
	تعداد ConstFlags : بینښه حرف {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	تعداد SelectPartials : بینښه حرف {
		SELECT_ALL = 0,          // include partial modules
		SELECT_ټولیز_ONLY = 1,   // ignore partial modules
		SELECT_ټولیز_WARN = 2,   // call log_warning on partial module
		SELECT_ټولیز_ERR = 3,    // call log_error on partial module
		SELECT_ټولیز_CMDERR = 4  // call log_cmd_error on partial module
	};

	تعداد SelectBoxes : بینښه حرف {
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

	تعداد پورټ_Dir : بینښه حرف  {
		PD_UNKNOWN = 0,
		PD_ننوت = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr صحيح پلنوالی_حد = 1 << 30;

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
	بنية Sig_بټ;
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


namespace RTLIL { using YOSYS_NAMESPACE_مختاړی ID; }
namespace RTLIL { using YOSYS_NAMESPACE_مختاړی IdString; }

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

	کينډۍ <typename T> using ترتیبول_by_name_id = IdString::compare_ptr_by_name<T>;

	کينډۍ <typename T> بنية ترتیبول_by_name_str {
		bool چلوونکی()(T *a, T *b) ثابت {
			أرجع a->name.lt_by_name(b->name);
		}
	};

	بنية ترتیبول_by_id_str {
		ثابت TwinePool& pool;
		explicit ترتیبول_by_id_str(ثابت TwinePool& pool) : pool(pool) {}
		bool چلوونکی()(RTLIL::IdString a, RTLIL::IdString b) ثابت {
			أرجع pool.compare_by_name(a, b) < 0;
		}
	};

	ساكن مضمن std::string encode_filename(ثابت std::string &filename)
	{
		std::stringstream val;
		إذا (!std::any_د(filename.begin(), filename.پای_ټکی(), [](حرف c) {
			أرجع static_cast<بینښه حرف>(c) < 33 || static_cast<بینښه حرف>(c) > 126;
		})) أرجع filename;
		لكل (بینښه حرف ثابت c : filename) {
			إذا (c < 33 || c > 126)
				val << stringf("$%02x", c);
			وإلا
				val << c;
		}
		أرجع val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_and         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_or          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_xor         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_xnor        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);

	RTLIL::Const const_reduce_and  (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_reduce_or   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_reduce_xor  (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_reduce_xnor (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_reduce_bool (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);

	RTLIL::Const const_logic_not   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_logic_and   (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_logic_or    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);

	RTLIL::Const const_shl         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_shr         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_sshl        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_sshr        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_ښويدنه       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_shiftx      (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);

	RTLIL::Const const_lt          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_le          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_eq          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_ne          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_eqx         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_nex         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_ge          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_gt          (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);

	RTLIL::Const const_جمع_کړه         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_sub         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_mul         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_div         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_divfloor    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_modfloor    (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_mod         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_pow         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);

	RTLIL::Const const_pos         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_buf         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);
	RTLIL::Const const_neg         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح پایله_len);

	RTLIL::Const const_mux         (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	کينډۍ<typename T, typename کلی_2 = IdString>
	بنية ObjIterator {
		using iterator_category = std::forward_iterator_نښان;
		using ارزښت_type = T;
		using difference_type = ptrdiff_t;
		using اشاره_ګر = T*;
		using reference = T&;
		typename dict<کلی_2, T>::iterator it;
		dict<کلی_2, T> *list_p;
		صحيح *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, صحيح *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			إذا (list_p->تشه()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} وإلا {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(ثابت RTLIL::ObjIterator<T, کلی_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			إذا (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &چلوونکی=(ثابت RTLIL::ObjIterator<T, کلی_2> &other) {
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

		مضمن T چلوونکی*() ثابت {
			log_assert(list_p != nullptr);
			أرجع it->دوهم;
		}

		مضمن bool چلوونکی!=(ثابت RTLIL::ObjIterator<T, کلی_2> &other) ثابت {
			إذا (list_p == nullptr || other.list_p == nullptr)
				أرجع list_p != other.list_p;
			أرجع it != other.it;
		}


		مضمن bool چلوونکی==(ثابت RTLIL::ObjIterator<T, کلی_2> &other) ثابت {
			أرجع !(*this != other);
		}

		مضمن ObjIterator<T, کلی_2>& چلوونکی++() {
			log_assert(list_p != nullptr);
			إذا (++it == list_p->پای_ټکی()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			أرجع *this;
		}

		مضمن ObjIterator<T, کلی_2>& چلوونکی+=(صحيح amt) {
			log_assert(list_p != nullptr);
			it += amt;
			إذا (it == list_p->پای_ټکی()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			أرجع *this;
		}

		مضمن ObjIterator<T, کلی_2> چلوونکی+(صحيح amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, کلی_2> new_obj(*this);
			new_obj.it += amt;
			إذا (new_obj.it == list_p->پای_ټکی()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			أرجع new_obj;
		}

		مضمن ثابت ObjIterator<T, کلی_2> چلوونکی++(صحيح) {
			ObjIterator<T, کلی_2> پایله(*this);
			++(*this);
			أرجع پایله;
		}
	};

	کينډۍ<typename T, typename کلی_2 = IdString>
	بنية ObjRange
	{
		dict<کلی_2, T> *list_p;
		صحيح *refcount_p;

		ObjRange(decltype(list_p) list_p, صحيح *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, کلی_2> begin() { أرجع RTLIL::ObjIterator<T, کلی_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, کلی_2> پای_ټکی() { أرجع RTLIL::ObjIterator<T, کلی_2>(); }

		اندازه_t اندازه() ثابت {
			أرجع list_p->اندازه();
		}

		چلوونکی pool<T>() ثابت {
			pool<T> پایله;
			لكل (خپلکاری &it : *list_p)
				پایله.ورننباسه(it.دوهم);
			أرجع پایله;
		}

		چلوونکی std::ویکټور<T>() ثابت {
			std::ویکټور<T> پایله;
			پایله.reserve(list_p->اندازه());
			لكل (خپلکاری &it : *list_p)
				پایله.ټېل_شا(it.دوهم);
			أرجع پایله;
		}

		pool<T> منزل_pool() ثابت { أرجع *this; }
		std::ویکټور<T> منزل_ویکټور() ثابت { أرجع *this; }
	};
};

بنية RTLIL::Const
{
	لنډ صحيح flags;
خاص:
	ملګری class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::ویکټور<RTLIL::State>;
	تعداد class backing_نښان: bool { بټونه, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_نښان نښان;
	اتحاد {
		bitvectype بټونه_2;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_بټونه() ثابت { أرجع نښان == backing_نښان::بټونه; }
	bool is_str() ثابت { أرجع نښان == backing_نښان::string; }

	bitvectype* get_if_بټونه() { أرجع is_بټونه() ? &بټونه_2 : عدم; }
	std::string* get_if_str() { أرجع is_str() ? &str_ : عدم; }
	ثابت bitvectype* get_if_بټونه() ثابت { أرجع is_بټونه() ? &بټونه_2 : عدم; }
	ثابت std::string* get_if_str() ثابت { أرجع is_str() ? &str_ : عدم; }

	bitvectype& get_بټونه();
	std::string& get_str();
	ثابت bitvectype& get_بټونه() ثابت;
	ثابت std::string& get_str() ثابت;
	std::ویکټور<RTLIL::State>& بټونه_دننی();
	فراغ bitvectorize_دننی();

عام:
	Const() : flags(RTLIL::CONST_FLAG_NONE), نښان(backing_نښان::بټونه), بټونه_2(std::ویکټور<RTLIL::State>()) {}
	Const(std::string str);
	Const(اوږد اوږد val); // default width is 32
	Const(اوږد اوږد val, صحيح پلنوالی);
	Const(RTLIL::State بټ, صحيح پلنوالی = 1);
	Const(std::ویکټور<RTLIL::State> بټونه) : flags(RTLIL::CONST_FLAG_NONE), نښان(backing_نښان::بټونه), بټونه_2(std::move(بټونه)) {}
	Const(ثابت std::ویکټور<bool> &بټونه);
	Const(ثابت RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &چلوونکی =(ثابت RTLIL::Const &other);
	~Const();

	بنية Builder
	{
		Builder() {}
		Builder(صحيح expected_پلنوالی) { بټونه.reserve(expected_پلنوالی); }
		فراغ ټېل_شا(RTLIL::State b) { بټونه.ټېل_شا(b); }
		صحيح اندازه() ثابت { أرجع static_cast<صحيح>(بټونه.اندازه()); }
		Const build() { أرجع Const(std::move(بټونه)); }
	خاص:
		std::ویکټور<RTLIL::State> بټونه;
	};

	bool چلوونکی <(ثابت RTLIL::Const &other) ثابت;
	bool چلوونکی ==(ثابت RTLIL::Const &other) ثابت;
	bool چلوونکی !=(ثابت RTLIL::Const &other) ثابت;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::ویکټور<RTLIL::State>& بټونه() { أرجع بټونه_دننی(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	فراغ bitvectorize() ثابت { const_cast<Const*>(this)->bitvectorize_دننی(); }

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

	فراغ نښان_bare_integer_const(ثابت std::string &ارزښت);

	std::string as_string(ثابت حرف* any = "-") ثابت;
	ساكن Const from_string(ثابت std::string &str);
	std::ویکټور<RTLIL::State> منزل_بټونه() ثابت;

	std::string decode_string() ثابت;
	صحيح اندازه() ثابت;
	bool تشه() ثابت;

	فراغ append(ثابت RTLIL::Const &other);
	فراغ سېټ(صحيح i, RTLIL::State state) {
		بټونه_دننی()[i] = state;
	}
	فراغ resize(صحيح اندازه, RTLIL::State fill) {
    log_assert(اندازه >= 0 && اندازه < RTLIL::پلنوالی_حد);
		بټونه_دننی().resize(اندازه, fill);
	}

	class const_iterator {
	خاص:
		ثابت Const* پلار;
		اندازه_t idx;

	عام:
		using iterator_category = std::bidirectional_iterator_نښان;
		using ارزښت_type = State;
		using difference_type = std::ptrdiff_t;
		using اشاره_ګر = ثابت State*;
		using reference = ثابت State&;

		const_iterator(ثابت Const& c, اندازه_t i) : پلار(&c), idx(i) {}

		State چلوونکی*() ثابت;

		const_iterator& چلوونکی++() { ++idx; أرجع *this; }
		const_iterator& چلوونکی--() { --idx; أرجع *this; }
		const_iterator چلوونکی++(صحيح) { const_iterator پایله(*this); ++idx; أرجع پایله; }
		const_iterator چلوونکی--(صحيح) { const_iterator پایله(*this); --idx; أرجع پایله; }
		const_iterator& چلوونکی+=(صحيح i) { idx += i; أرجع *this; }

		const_iterator چلوونکی+(صحيح جمع_کړه) {
			أرجع const_iterator(*پلار, idx + جمع_کړه);
		}
		const_iterator چلوونکی-(صحيح sub) {
			أرجع const_iterator(*پلار, idx - sub);
		}
		صحيح چلوونکی-(ثابت const_iterator& other) {
			أرجع idx - other.idx;
		}

		bool چلوونکی==(ثابت const_iterator& other) ثابت {
			أرجع idx == other.idx;
		}

		bool چلوونکی!=(ثابت const_iterator& other) ثابت {
			أرجع !(*this == other);
		}
	};

	class iterator {
	خاص:
		Const* پلار;
		اندازه_t idx;

	عام:
		class proxy {
		خاص:
			Const* پلار;
			اندازه_t idx;
		عام:
			proxy(Const* پلار, اندازه_t idx) : پلار(پلار), idx(idx) {}
			چلوونکی State() ثابت { أرجع (*پلار)[idx]; }
			proxy& چلوونکی=(State s) { پلار->سېټ(idx, s); أرجع *this; }
			proxy& چلوونکی=(ثابت proxy& other) { پلار->سېټ(idx, (*other.پلار)[other.idx]); أرجع *this; }
		};

		using iterator_category = std::bidirectional_iterator_نښان;
		using ارزښت_type = State;
		using difference_type = std::ptrdiff_t;
		using اشاره_ګر = proxy*;
		using reference = proxy;

		iterator(Const& c, اندازه_t i) : پلار(&c), idx(i) {}

		proxy چلوونکی*() ثابت { أرجع proxy(پلار, idx); }
		iterator& چلوونکی++() { ++idx; أرجع *this; }
		iterator& چلوونکی--() { --idx; أرجع *this; }
		iterator چلوونکی++(صحيح) { iterator پایله(*this); ++idx; أرجع پایله; }
		iterator چلوونکی--(صحيح) { iterator پایله(*this); --idx; أرجع پایله; }
		iterator& چلوونکی+=(صحيح i) { idx += i; أرجع *this; }

		iterator چلوونکی+(صحيح جمع_کړه) {
			أرجع iterator(*پلار, idx + جمع_کړه);
		}
		iterator چلوونکی-(صحيح sub) {
			أرجع iterator(*پلار, idx - sub);
		}
		صحيح چلوونکی-(ثابت iterator& other) {
			أرجع idx - other.idx;
		}

		bool چلوونکی==(ثابت iterator& other) ثابت {
			أرجع idx == other.idx;
		}

		bool چلوونکی!=(ثابت iterator& other) ثابت {
			أرجع !(*this == other);
		}
	};

	const_iterator begin() ثابت {
		أرجع const_iterator(*this, 0);
	}
	const_iterator پای_ټکی() ثابت {
		أرجع const_iterator(*this, اندازه());
	}
	iterator begin() {
		أرجع iterator(*this, 0);
	}
	iterator پای_ټکی() {
		أرجع iterator(*this, اندازه());
	}
	State شا() ثابت {
		أرجع *(پای_ټکی() - 1);
	}
	State front() ثابت {
		أرجع *begin();
	}
	State at(اندازه_t i) ثابت {
		أرجع *const_iterator(*this, i);
	}
	State چلوونکی[](اندازه_t i) ثابت {
		أرجع *const_iterator(*this, i);
	}

	bool is_fully_zero() ثابت;
	bool is_fully_ones() ثابت;
	bool is_fully_def() ثابت;
	bool is_fully_undef() ثابت;
	bool is_fully_undef_x_only() ثابت;
	bool is_onehot(صحيح *pos = nullptr) ثابت;

	RTLIL::Const ایستل(صحيح دننه_واټن, صحيح len = 1, RTLIL::State padding = RTLIL::State::S0) ثابت;

	// find the MSB without redundant leading bits
	صحيح get_min_اندازه(bool is_signed) ثابت;

	// compress representation to the minimum required bits
	فراغ compress(bool is_signed = false);

	std::optional<صحيح> as_int_compress(bool is_signed) ثابت;

	فراغ extu(صحيح پلنوالی) {
		resize(پلنوالی, RTLIL::State::S0);
	}

	فراغ exts(صحيح پلنوالی) {
		resize(پلنوالی, تشه() ? RTLIL::State::Sx : شا());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
};

بنية RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> ځانګړتیاوې;

	bool has_attribute(RTLIL::IdString id) ثابت;

	فراغ سېټ_bool_attribute(RTLIL::IdString id, bool ارزښت=true);
	bool get_bool_attribute(RTLIL::IdString id) ثابت;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ثابت {
		أرجع get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	فراغ سېټ_string_attribute(RTLIL::IdString id, string ارزښت);
	string get_string_attribute(RTLIL::IdString id) ثابت;

	فراغ سېټ_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &معلومات);
	فراغ جمع_کړه_strpool_attribute(RTLIL::IdString id, ثابت pool<string> &معلومات);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ثابت;

	فراغ سېټ_src_attribute(ثابت std::string &src) {
		سېټ_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ثابت {
		أرجع get_string_attribute(ID::src);
	}

	فراغ سېټ_hdlname_attribute(ثابت ویکټور<string> &hierarchy);
	ویکټور<string> get_hdlname_attribute() ثابت;

	فراغ سېټ_intvec_attribute(RTLIL::IdString id, ثابت ویکټور<صحيح> &معلومات);
	ویکټور<صحيح> get_intvec_attribute(RTLIL::IdString id) ثابت;
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
	std::ویکټور<RTLIL::State> معلومات; // only used if wire == NULL, LSB at index 0
	صحيح پلنوالی, دننه_واټن;

	SigChunk() : wire(nullptr), پلنوالی(0), دننه_واټن(0) {}
	SigChunk(ثابت RTLIL::Const &ارزښت) : wire(nullptr), معلومات(ارزښت.منزل_بټونه()), پلنوالی(Get_اندازه(معلومات)), دننه_واټن(0) {}
	SigChunk(RTLIL::Const &&ارزښت) : wire(nullptr), معلومات(ارزښت.منزل_بټونه()), پلنوالی(Get_اندازه(معلومات)), دننه_واټن(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), پلنوالی(Get_اندازه(wire)), دننه_واټن(0) {}
	SigChunk(RTLIL::Wire *wire, صحيح دننه_واټن, صحيح پلنوالی = 1) : wire(wire), پلنوالی(پلنوالی), دننه_واټن(دننه_واټن) {}
	SigChunk(ثابت std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(صحيح val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(صحيح val, صحيح پلنوالی) : SigChunk(RTLIL::Const(val, پلنوالی)) {}
	SigChunk(RTLIL::State بټ, صحيح پلنوالی = 1) : SigChunk(RTLIL::Const(بټ, پلنوالی)) {}
	SigChunk(ثابت RTLIL::Sig_بټ &بټ);

	RTLIL::SigChunk ایستل(صحيح دننه_واټن, صحيح اوږدتیا) ثابت;
	RTLIL::Sig_بټ چلوونکی[](صحيح دننه_واټن) ثابت;
	مضمن صحيح اندازه() ثابت { أرجع پلنوالی; }
	مضمن bool is_wire() ثابت { أرجع wire != عدم; }

	bool چلوونکی <(ثابت RTLIL::SigChunk &other) ثابت;
	bool چلوونکی ==(ثابت RTLIL::SigChunk &other) ثابت;
	bool چلوونکی !=(ثابت RTLIL::SigChunk &other) ثابت;
};

بنية RTLIL::Sig_بټ
{
	RTLIL::Wire *wire;
	اتحاد {
		RTLIL::State معلومات; // used if wire == NULL
		صحيح دننه_واټن;        // used if wire != NULL
	};

	Sig_بټ();
	Sig_بټ(RTLIL::State بټ);
	explicit Sig_بټ(bool بټ);
	Sig_بټ(RTLIL::Wire *wire);
	Sig_بټ(RTLIL::Wire *wire, صحيح دننه_واټن);
	Sig_بټ(ثابت RTLIL::SigChunk &chunk);
	Sig_بټ(ثابت RTLIL::SigChunk &chunk, صحيح شاخص);
	Sig_بټ(ثابت RTLIL::SigSpec &sig);
	Sig_بټ(ثابت RTLIL::Sig_بټ &sigbit) = افتراضي;
	RTLIL::Sig_بټ &چلوونکی =(ثابت RTLIL::Sig_بټ &other) = افتراضي;

	مضمن bool is_wire() ثابت { أرجع wire != عدم; }

	bool چلوونکی <(ثابت RTLIL::Sig_بټ &other) ثابت;
	bool چلوونکی ==(ثابت RTLIL::Sig_بټ &other) ثابت;
	bool چلوونکی !=(ثابت RTLIL::Sig_بټ &other) ثابت;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت;
	[[nodiscard]] Hasher hash_سر() ثابت;
};

namespace hashlib {
	کينډۍ <>
	بنية hash_ops<RTLIL::Sig_بټ> {
		ساكن مضمن bool cmp(ثابت RTLIL::Sig_بټ &a, ثابت RTLIL::Sig_بټ &b) {
			أرجع a == b;
		}
		[[nodiscard]] ساكن مضمن Hasher hash(ثابت RTLIL::Sig_بټ sb) {
			أرجع sb.hash_سر();
		}
		[[nodiscard]] ساكن مضمن Hasher hash_into(ثابت RTLIL::Sig_بټ sb, Hasher h) {
			أرجع sb.hash_into(h);
		}
	};
};

بنية RTLIL::SigSpecIterator
{
	تعريفنوع std::ننوت_iterator_نښان iterator_category;
	تعريفنوع RTLIL::Sig_بټ ارزښت_type;
	تعريفنوع ptrdiff_t difference_type;
	تعريفنوع RTLIL::Sig_بټ* اشاره_ګر;
	تعريفنوع RTLIL::Sig_بټ& reference;

	RTLIL::SigSpec *sig_p;
	صحيح شاخص;

	مضمن RTLIL::Sig_بټ &چلوونکی*() ثابت;
	مضمن bool چلوونکی!=(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع شاخص != other.شاخص; }
	مضمن bool چلوونکی==(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع شاخص == other.شاخص; }
	مضمن فراغ چلوونکی++() { شاخص++; }
};

بنية RTLIL::SigSpecConstIterator
{
	تعريفنوع std::ننوت_iterator_نښان iterator_category;
	تعريفنوع RTLIL::Sig_بټ ارزښت_type;
	تعريفنوع ptrdiff_t difference_type;
	تعريفنوع RTLIL::Sig_بټ* اشاره_ګر;
	تعريفنوع RTLIL::Sig_بټ& reference;

	ثابت RTLIL::SigSpec *sig_p;
	RTLIL::Sig_بټ بټ;
	صحيح شاخص;

	مضمن ثابت RTLIL::Sig_بټ &چلوونکی*();
	مضمن bool چلوونکی!=(ثابت RTLIL::SigSpecConstIterator &other) ثابت { أرجع شاخص != other.شاخص; }
	مضمن bool چلوونکی==(ثابت RTLIL::SigSpecIterator &other) ثابت { أرجع شاخص == other.شاخص; }
	مضمن فراغ چلوونکی++() { شاخص++; }
};

بنية RTLIL::SigSpec
{
خاص:
	ملګری class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, ایستل_2);
	تعداد Representation : حرف {
		CHUNK,
		بټونه_3,
	};
	// An AtomicHash is either clear or a nonzero integer.
	بنية AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ثابت AtomicHash &rhs) : atomic_(rhs.بارول()) {}
		AtomicHash &چلوونکی=(ثابت AtomicHash &rhs) { store(rhs.بارول()); أرجع *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> اقرأ() ثابت {
			Hasher::hash_t ارزښت = بارول();
			إذا (ارزښت == 0)
				أرجع std::nullopt;
			أرجع ارزښت;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		فراغ سېټ(Hasher::hash_t ارزښت) ثابت {
			log_assert(ارزښت != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(ارزښت, std::حافظه_ترتیب_relaxed);
			log_assert(old == 0 || old == ارزښت);
		}
		فراغ clear() { store(0); }
	خاص:
		صحيح بارول() ثابت { أرجع atomic_.بارول(std::حافظه_ترتیب_relaxed); }
		فراغ store(Hasher::hash_t ارزښت) ثابت {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(ارزښت, std::حافظه_ترتیب_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	اتحاد {
		RTLIL::SigChunk chunk_;
		std::ویکټور<RTLIL::Sig_بټ> بټونه_2; // LSB at index 0
	};

	فراغ init_تشه_بټونه() {
		rep_ = بټونه_3;
		new (&بټونه_2) std::ویکټور<RTLIL::Sig_بټ>;
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
			بټونه_2.~ویکټور();
	}
	ملګری بنية Chunks;

عام:
	SigSpec() { init_تشه_بټونه(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ثابت SigSpec &ارزښت) : rep_(ارزښت.rep_), hash_(ارزښت.hash_) {
		إذا (ارزښت.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(ارزښت.chunk_);
		وإلا
			new (&بټونه_2) std::ویکټور<RTLIL::Sig_بټ>(ارزښت.بټونه_2);
	}
	SigSpec(SigSpec &&ارزښت) : rep_(ارزښت.rep_), hash_(ارزښت.hash_) {
		إذا (ارزښت.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(ارزښت.chunk_));
		وإلا
			new (&بټونه_2) std::ویکټور<RTLIL::Sig_بټ>(std::move(ارزښت.بټونه_2));
	}
	SigSpec(ثابت RTLIL::Const &ارزښت);
	SigSpec(RTLIL::Const &&ارزښت);
	SigSpec(ثابت RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, صحيح دننه_واټن, صحيح پلنوالی = 1);
	SigSpec(ثابت std::string &str);
	SigSpec(صحيح val, صحيح پلنوالی = 32);
	SigSpec(RTLIL::State بټ, صحيح پلنوالی = 1);
	SigSpec(ثابت RTLIL::Sig_بټ &بټ, صحيح پلنوالی = 1);
	SigSpec(ثابت std::ویکټور<RTLIL::SigChunk> &chunks);
	SigSpec(ثابت std::ویکټور<RTLIL::Sig_بټ> &بټونه);
	SigSpec(ثابت pool<RTLIL::Sig_بټ> &بټونه);
	SigSpec(ثابت std::سېټ<RTLIL::Sig_بټ> &بټونه);
	explicit SigSpec(bool بټ);
	~SigSpec() {
		destroy();
	}

	SigSpec &چلوونکی=(ثابت SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		إذا (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		وإلا
			new (&بټونه_2) std::ویکټور<RTLIL::Sig_بټ>(rhs.بټونه_2);
		أرجع *this;
	}
	SigSpec &چلوونکی=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		إذا (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		وإلا
			new (&بټونه_2) std::ویکټور<RTLIL::Sig_بټ>(std::move(rhs.بټونه_2));
		أرجع *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	بنية Chunks {
		Chunks(ثابت SigSpec &spec) : spec(spec) {}
		بنية const_iterator {
			using iterator_category = std::forward_iterator_نښان;
			using ارزښت_type = ثابت SigChunk &;
			using difference_type = std::ptrdiff_t;
			using اشاره_ګر = ثابت SigChunk *;
			using reference = ثابت SigChunk &;

			ثابت SigSpec &spec;
			صحيح بټ_شاخص;
			SigChunk chunk;

			const_iterator(ثابت SigSpec &spec) : spec(spec) {
				بټ_شاخص = 0;
				إذا (spec.rep_ == بټونه_3)
					بل_chunk_بټونه();
			}
			تعداد پای_ټکی_2 { پای_ټکی_3 };
			const_iterator(ثابت SigSpec &spec, پای_ټکی_2) : spec(spec) {
				بټ_شاخص = spec.اندازه();
			}
			فراغ بل_chunk_بټونه();

			ثابت SigChunk &چلوونکی*() {
				إذا (spec.rep_ == CHUNK)
					أرجع spec.chunk_;
				أرجع chunk;
			};
			ثابت SigChunk *چلوونکی->() { أرجع &**this; }
			const_iterator &چلوونکی++() {
				بټ_شاخص += (**this).پلنوالی;
				إذا (spec.rep_ == بټونه_3)
					بل_chunk_بټونه();
				أرجع *this;
			}
			bool چلوونکی==(ثابت const_iterator &rhs) ثابت { أرجع بټ_شاخص == rhs.بټ_شاخص; }
			bool چلوونکی!=(ثابت const_iterator &rhs) ثابت { أرجع !(*this == rhs); }
		};
		const_iterator begin() ثابت { أرجع const_iterator(spec); }
		const_iterator پای_ټکی() ثابت {
			const_iterator it(spec, const_iterator::پای_ټکی_3);
			أرجع it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::ویکټور<RTLIL::SigChunk>::const_سرچپه_iterator rbegin() {
			ensure_chunk_ویکټور();
			أرجع chunk_ویکټور.rbegin();
		}
		std::ویکټور<RTLIL::SigChunk>::const_سرچپه_iterator rend() {
			ensure_chunk_ویکټور();
			أرجع chunk_ویکټور.rend();
		}
		صحيح اندازه() {
			ensure_chunk_ویکټور();
			أرجع chunk_ویکټور.اندازه();
		}
		صحيح اندازه() ثابت {
			أرجع std::واټن(begin(), پای_ټکی());
		}
		ثابت SigChunk &at(صحيح شاخص) {
			ensure_chunk_ویکټور();
			أرجع chunk_ویکټور.at(شاخص);
		}
		چلوونکی ثابت std::ویکټور<RTLIL::SigChunk>&() {
			ensure_chunk_ویکټور();
			أرجع chunk_ویکټور;
		}
	خاص:
		فراغ ensure_chunk_ویکټور() {
			إذا (spec.اندازه() > 0 && chunk_ویکټور.تشه()) {
				لكل (ثابت RTLIL::SigChunk &c : *this)
					chunk_ویکټور.ټېل_شا(c);
			}
		}
		ثابت SigSpec &spec;
		std::ویکټور<RTLIL::SigChunk> chunk_ویکټور;
	};
	ملګری بنية Chunks::const_iterator;

	مضمن Chunks chunks() ثابت { أرجع {*this}; }
	مضمن ثابت SigSpec &بټونه() ثابت { أرجع *this; }

	مضمن صحيح اندازه() ثابت { أرجع rep_ == CHUNK ? chunk_.پلنوالی : Get_اندازه(بټونه_2); }
	مضمن bool تشه() ثابت { أرجع اندازه() == 0; };

	مضمن RTLIL::Sig_بټ &چلوونکی[](صحيح شاخص) { inline_unpack(); hash_.clear(); أرجع بټونه_2.at(شاخص); }
	مضمن RTLIL::Sig_بټ چلوونکی[](صحيح شاخص) ثابت {
		إذا (rep_ == CHUNK) {
			إذا (شاخص < 0 || شاخص >= chunk_.پلنوالی)
				throw std::بهر_د_range("SigSpec::operator[]");
			إذا (chunk_.wire)
				أرجع RTLIL::Sig_بټ(chunk_.wire, chunk_.دننه_واټن + شاخص);
			أرجع RTLIL::Sig_بټ(chunk_.معلومات[شاخص]);
		}
		أرجع بټونه_2.at(شاخص);
	}

	مضمن RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.شاخص = 0; أرجع it; }
	مضمن RTLIL::SigSpecIterator پای_ټکی() { RTLIL::SigSpecIterator it; it.sig_p = this; it.شاخص = اندازه(); أرجع it; }

	مضمن RTLIL::SigSpecConstIterator begin() ثابت { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.شاخص = 0; أرجع it; }
	مضمن RTLIL::SigSpecConstIterator پای_ټکی() ثابت { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.شاخص = اندازه(); أرجع it; }

	فراغ ترتیبول();
	فراغ ترتیبول_and_unify();

	فراغ replace(ثابت RTLIL::SigSpec &نمونه, ثابت RTLIL::SigSpec &with);
	فراغ replace(ثابت RTLIL::SigSpec &نمونه, ثابت RTLIL::SigSpec &with, RTLIL::SigSpec *other) ثابت;

	فراغ replace(ثابت dict<RTLIL::Sig_بټ, RTLIL::Sig_بټ> &قواعد);
	فراغ replace(ثابت dict<RTLIL::Sig_بټ, RTLIL::Sig_بټ> &قواعد, RTLIL::SigSpec *other) ثابت;

	فراغ replace(ثابت std::map<RTLIL::Sig_بټ, RTLIL::Sig_بټ> &قواعد);
	فراغ replace(ثابت std::map<RTLIL::Sig_بټ, RTLIL::Sig_بټ> &قواعد, RTLIL::SigSpec *other) ثابت;

	فراغ replace(صحيح دننه_واټن, ثابت RTLIL::SigSpec &with);

	فراغ remove(ثابت RTLIL::SigSpec &نمونه);
	فراغ remove(ثابت RTLIL::SigSpec &نمونه, RTLIL::SigSpec *other) ثابت;
	فراغ remove2(ثابت RTLIL::SigSpec &نمونه, RTLIL::SigSpec *other);

	فراغ remove(ثابت pool<RTLIL::Sig_بټ> &نمونه);
	فراغ remove(ثابت pool<RTLIL::Sig_بټ> &نمونه, RTLIL::SigSpec *other) ثابت;
	فراغ remove2(ثابت pool<RTLIL::Sig_بټ> &نمونه, RTLIL::SigSpec *other);
	فراغ remove2(ثابت std::سېټ<RTLIL::Sig_بټ> &نمونه, RTLIL::SigSpec *other);
	فراغ remove2(ثابت pool<RTLIL::Wire*> &نمونه, RTLIL::SigSpec *other);

	فراغ remove(صحيح دننه_واټن, صحيح اوږدتیا = 1);
	فراغ remove_const();

	RTLIL::SigSpec ایستل(ثابت RTLIL::SigSpec &نمونه, ثابت RTLIL::SigSpec *other = عدم) ثابت;
	RTLIL::SigSpec ایستل(ثابت pool<RTLIL::Sig_بټ> &نمونه, ثابت RTLIL::SigSpec *other = عدم) ثابت;
	RTLIL::SigSpec ایستل(صحيح دننه_واټن, صحيح اوږدتیا = 1) ثابت;
	RTLIL::SigSpec ایستل_پای_ټکی(صحيح دننه_واټن) ثابت { أرجع ایستل(دننه_واټن, اندازه() - دننه_واټن); }

	فراغ rewrite_wires(std::function<فراغ(RTLIL::Wire*& wire)> rewrite);

	RTLIL::Sig_بټ lsb() ثابت { log_assert(اندازه()); أرجع (*this)[0]; };
	RTLIL::Sig_بټ msb() ثابت { log_assert(اندازه()); أرجع (*this)[اندازه() - 1]; };
	RTLIL::Sig_بټ front() ثابت { أرجع (*this)[0]; }
	RTLIL::Sig_بټ شا() ثابت { أرجع (*this)[اندازه() - 1]; }

	فراغ append(ثابت RTLIL::SigSpec &إشارة);
	مضمن فراغ append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	مضمن فراغ append(ثابت RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	مضمن فراغ append(ثابت RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	فراغ append(ثابت RTLIL::Sig_بټ &بټ);
	مضمن فراغ append(RTLIL::State state) { append(RTLIL::Sig_بټ(state)); }
	مضمن فراغ append(bool bool_) { append(RTLIL::Sig_بټ(bool_)); }

	فراغ extend_u0(صحيح پلنوالی, bool is_signed = false);

	RTLIL::SigSpec repeat(صحيح عدد) ثابت;

	فراغ سرچپه() { inline_unpack(); std::سرچپه(بټونه_2.begin(), بټونه_2.پای_ټکی()); }

	bool چلوونکی <(ثابت RTLIL::SigSpec &other) ثابت;
	bool چلوونکی ==(ثابت RTLIL::SigSpec &other) ثابت;
	مضمن bool چلوونکی !=(ثابت RTLIL::SigSpec &other) ثابت { أرجع !(*this == other); }

	bool is_wire() ثابت;
	bool is_chunk() ثابت;
	مضمن bool is_بټ() ثابت { أرجع اندازه() == 1; }

	bool معلوم_driver() ثابت;

	bool is_fully_const() ثابت;
	bool is_fully_zero() ثابت;
	bool is_fully_ones() ثابت;
	bool is_fully_def() ثابت;
	bool is_fully_undef() ثابت;
	bool has_const() ثابت;
	bool has_const(State state) ثابت;
	bool has_marked_بټونه() ثابت;
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
	RTLIL::Sig_بټ as_بټ() ثابت;

	bool سمون(ثابت حرف* نمونه) ثابت;

	std::سېټ<RTLIL::Sig_بټ> منزل_sigbit_سېټ() ثابت;
	pool<RTLIL::Sig_بټ> منزل_sigbit_pool() ثابت;
	std::ویکټور<RTLIL::Sig_بټ> منزل_sigbit_ویکټور() ثابت;
	std::map<RTLIL::Sig_بټ, RTLIL::Sig_بټ> منزل_sigbit_map(ثابت RTLIL::SigSpec &other) ثابت;
	dict<RTLIL::Sig_بټ, RTLIL::Sig_بټ> منزل_sigbit_dict(ثابت RTLIL::SigSpec &other) ثابت;

	ساكن bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	ساكن bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	ساكن bool parse_rhs(ثابت RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	چلوونکی std::ویکټور<RTLIL::SigChunk>() ثابت;
	چلوونکی std::ویکټور<RTLIL::Sig_بټ>() ثابت { أرجع منزل_sigbit_ویکټور(); }
	ثابت RTLIL::Sig_بټ &at(صحيح دننه_واټن, ثابت RTLIL::Sig_بټ &defval) { أرجع دننه_واټن < اندازه() ? (*this)[دننه_واټن] : defval; }
	RTLIL::Sig_بټ& at(صحيح دننه_واټن) { أرجع (*this)[دننه_واټن]; }
	RTLIL::Sig_بټ at(صحيح دننه_واټن) ثابت { أرجع (*this)[دننه_واټن]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
		Hasher::hash_t val;
		إذا (std::optional<Hasher::hash_t> current = hash_.اقرأ())
			val = *current;
		وإلا
			val = updhash();
		h.eat(val);
		أرجع h;
	}

#که_نه_تعریف_شوی NDEBUG
	فراغ check(ثابت Module *mod = nullptr) ثابت;
#وإلا
	فراغ check(ثابت Module *mod = nullptr) ثابت { (فراغ)mod; }
#د_که_پای
};

بنية RTLIL::Selection
{
	// selection includes boxed modules
	bool selects_boxes;
	// selection covers full design, including boxed modules
	bool complete_selection;
	// selection covers full design, not including boxed modules
	bool ډک_selection;
	pool<RTLIL::IdString> selected_modules;
	dict<RTLIL::IdString, pool<RTLIL::IdString>> selected_members;
	RTLIL::Design *current_design;

	// create a new selection
	Selection(
		// should the selection cover the full design
		bool ډک_2 = true,
		// should the selection include boxed modules
		bool boxes = false,
		// the design to select from
		RTLIL::Design *design = nullptr
	) :
		selects_boxes(boxes), complete_selection(ډک_2 && boxes), ډک_selection(ډک_2 && !boxes), current_design(design) { }

	// checks if the given module exists in the current design and is a
	// boxed module, warning the user if the current design is not set
	bool boxed_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_ټولیز_module(RTLIL::IdString mod_name) ثابت;

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
		أرجع ډک_selection || complete_selection;
	}

	// add whole module to this selection
	کينډۍ<typename T1> فراغ select(T1 *module) {
		إذا (!selects_all() && selected_modules.شمېر(module->name) == 0) {
			IdString name = module->name;
			selected_modules.ورننباسه(name);
			selected_members.erase(name);
			إذا (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	کينډۍ<typename T1, typename T2> فراغ select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	کينډۍ<typename T1> فراغ select(T1 *module, RTLIL::IdString member) {
		إذا (!selects_all() && selected_modules.شمېر(module->name.ref()) == 0) {
			selected_members[module->name.ref()].ورننباسه(member);
			إذا (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool تشه() ثابت {
		أرجع !selects_all() && selected_modules.تشه() && selected_members.تشه();
	}

	// clear this selection, leaving it empty
	فراغ clear();

	// create a new selection which is empty
	ساكن Selection تشه_Selection(RTLIL::Design *design = nullptr) { أرجع Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	ساكن Selection ډک_Selection(RTLIL::Design *design = nullptr) { أرجع Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	ساكن Selection CompleteSelection(RTLIL::Design *design = nullptr) { أرجع Selection(true, true, design); };
};

بنية RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

	Monitor() {
		ساكن بینښه صحيح hashidx_شمېر = 123456789;
		hashidx_شمېر = mkhash_xorshift(hashidx_شمېر);
		hashidx_ = hashidx_شمېر;
	}

	مجازي ~Monitor() { }
	مجازي فراغ notify_module_جمع_کړه(RTLIL::Module*) { }
	مجازي فراغ notify_module_del(RTLIL::Module*) { }
	مجازي فراغ notify_connect(RTLIL::Cell*, RTLIL::IdString, ثابت RTLIL::SigSpec&, ثابت RTLIL::SigSpec&) { }
	مجازي فراغ notify_connect(RTLIL::Module*, ثابت RTLIL::SigSig&) { }
	مجازي فراغ notify_connect(RTLIL::Module*, ثابت std::ویکټور<RTLIL::SigSig>&) { }
	مجازي فراغ notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
بنية define_map_t;

کينډۍ<typename N>
مضمن constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::ونې_پاڼه> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ثابت حرف*> || std::is_same_v<std::decay_t<N>, حرف*>;

#تعریف_کړه YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, صحيح> = 0

کينډۍ<typename N>
مضمن constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ثابت حرف *> || std::is_same_v<std::decay_t<N>, حرف *>;

#تعریف_کړه YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, صحيح> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#تعریف_کړه YS_NAME_FWD_POOL(_func, _pool) \
	کينډۍ<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(خپلکاری) _func(N name, Rest&&... rest) \
		{ أرجع _func(_pool.جمع_کړه(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#تعریف_کړه YS_NAME_FWD_2ND_POOL(_func, _pool) \
	کينډۍ<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(خپلکاری) _func(T &&لومړی, N name) \
		{ أرجع _func(std::forward<T>(لومړی), _pool.جمع_کړه(std::move(name))); }

#تعریف_کړه YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#تعریف_کړه YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

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

	std::ویکټور<std::unique_ptr<AST::Ast_غوټه>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_تعریفوي;

	std::ویکټور<RTLIL::Selection> selection_ډېری;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_فعال_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ثابت RTLIL::Module *module(RTLIL::IdString name) ثابت;
	کينډۍ<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ أرجع module(twines().پیدا_کړه(std::string_view(name))); }
	کينډۍ<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Module *module(N name) ثابت
		{ أرجع module(twines().پیدا_کړه(std::string_view(name))); }
	RTLIL::Module *سر_module() ثابت;

	bool has(RTLIL::IdString id) ثابت {
		twines_.check_owned(id);
		أرجع modules_.شمېر(id) != 0;
	}
	کينډۍ<typename N, YS_NAME_STRING(N)> bool has(N name) ثابت
		{ أرجع has(twines().پیدا_کړه(std::string_view(name))); }

	فراغ جمع_کړه(RTLIL::Module *module);

	RTLIL::Module *جمع_کړه_Module(RTLIL::IdString name);
	YS_NAME_FWD_POOL(جمع_کړه_Module, twines())
	فراغ remove(RTLIL::Module *module);
	فراغ rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	فراغ scratchpad_unset(ثابت std::string &varname);

	فراغ scratchpad_سېټ_int(ثابت std::string &varname, صحيح ارزښت);
	فراغ scratchpad_سېټ_bool(ثابت std::string &varname, bool ارزښت);
	فراغ scratchpad_سېټ_string(ثابت std::string &varname, std::string ارزښت);

	صحيح scratchpad_get_int(ثابت std::string &varname, صحيح default_ارزښت_2 = 0) ثابت;
	bool scratchpad_get_bool(ثابت std::string &varname, bool default_ارزښت_2 = false) ثابت;
	std::string scratchpad_get_string(ثابت std::string &varname, ثابت std::string &default_ارزښت_2 = std::string()) ثابت;

	فراغ ترتیبول();
	فراغ ترتیبول_modules();
	فراغ check();
	فراغ optimize();

	فراغ clone_into(RTLIL::Design *dst) ثابت;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_ټولیز_module(RTLIL::IdString mod_name) ثابت;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ثابت;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ثابت;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_ټولیز_module(RTLIL::Module *mod) ثابت;

	// push the given selection to the selection stack
	فراغ ټېل_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	فراغ ټېل_تشه_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	فراغ ټېل_ډک_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	فراغ ټېل_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	فراغ راوباسه_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		أرجع selection_ډېری.شا();
	}

	// get the current selection
	ثابت RTLIL::Selection &selection() ثابت {
		أرجع selection_ډېری.شا();
	}

	// is the current selection a full selection (no boxes)
	bool ډک_selection() ثابت {
		أرجع selection().ډک_selection;
	}

	// is the given module in the current selection
	کينډۍ<typename T1> bool selected(T1 *module) ثابت {
		أرجع selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	کينډۍ<typename T1, typename T2> bool selected(T1 *module, T2 *member) ثابت {
		أرجع selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	کينډۍ<typename T1> فراغ select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	کينډۍ<typename T1, typename T2> فراغ select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::ویکټور<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) ثابت;

	// returns all selected modules, and may include boxes
	std::ویکټور<RTLIL::Module*> all_selected_modules() ثابت { أرجع selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::ویکټور<RTLIL::Module*> selected_unboxed_modules() ثابت { أرجع selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::ویکټور<RTLIL::Module*> selected_unboxed_modules_warn() ثابت { أرجع selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::ویکټور<RTLIL::Module*> selected_ټولیز_modules() ثابت { أرجع selected_modules(SELECT_ټولیز_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::ویکټور<RTLIL::Module*> all_selected_ټولیز_modules() ثابت { أرجع selected_modules(SELECT_ټولیز_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::ویکټور<RTLIL::Module*> selected_ټولیز_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ثابت { أرجع selected_modules(SELECT_ټولیز_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::ویکټور<RTLIL::Module*> selected_unboxed_ټولیز_modules() ثابت { أرجع selected_modules(SELECT_ټولیز_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::ویکټور<RTLIL::Module*> selected_unboxed_ټولیز_modules_warn() ثابت { أرجع selected_modules(SELECT_ټولیز_WARN, SB_UNBOXED_WARN); }

	ساكن std::map<بینښه صحيح, RTLIL::Design*> *get_all_designs(فراغ);

	std::string منزل_rtlil_str(bool only_selected = true) ثابت;
};

بنية RTLIL::Module : عام RTLIL::AttrObject
{
	ملګری بنية RTLIL::Cell;
	ملګری بنية RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	فراغ جمع_کړه(RTLIL::Wire *wire);
	فراغ جمع_کړه(RTLIL::Cell *cell);
	فراغ جمع_کړه(RTLIL::Process *process);

عام:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ثابت { أرجع design->twines(); }

	صحيح refcount_wires_;
	صحيح refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::ویکټور<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_ارزښتونه;
	dict<RTLIL::IdString, RTLIL::حافظه*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	مجازي ~Module();
	مجازي RTLIL::IdString derive(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	مجازي RTLIL::IdString derive(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Const> &parameters, ثابت dict<RTLIL::IdString, RTLIL::Module*> &interfaces, ثابت dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	مجازي اندازه_t شمېر_id(RTLIL::IdString id);
	مجازي فراغ expand_interfaces(RTLIL::Design *design, ثابت dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	مجازي bool reprocess_if_necessary(RTLIL::Design *design);

	مجازي فراغ ترتیبول();
	مجازي فراغ check();
	مجازي فراغ optimize();
	مجازي فراغ makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ثابت {
		أرجع get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	فراغ connect(ثابت RTLIL::SigSig &conn);
	فراغ connect(ثابت RTLIL::SigSpec &lhs, ثابت RTLIL::SigSpec &rhs);
	فراغ new_connections(ثابت std::ویکټور<RTLIL::SigSig> &new_conn);
	ثابت std::ویکټور<RTLIL::SigSig> &connections() ثابت;

	std::ویکټور<RTLIL::IdString> ports;
	فراغ fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_کتار;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_پورټ_کتار;
	pool<RTLIL::Wire *> buf_norm_wire_کتار;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_شاخص;
	فراغ bufNormalize();

	کينډۍ<typename T> فراغ rewrite_sigspecs(T &functor);
	کينډۍ<typename T> فراغ rewrite_sigspecs2(T &functor);
	فراغ cloneInto(RTLIL::Module *new_mod) ثابت;
	مجازي RTLIL::Module *clone() ثابت;
	مجازي RTLIL::Module *clone(RTLIL::Design *dst) ثابت;
	مجازي RTLIL::Module *clone(RTLIL::Design *dst, IdString موخه_name) ثابت;

	bool has_memories() ثابت;
	bool has_processes() ثابت;

	bool has_memories_warn() ثابت;
	bool has_processes_warn() ثابت;

	bool is_selected() ثابت;
	bool is_selected_ټولیز() ثابت;

	std::ویکټور<RTLIL::Wire*> selected_wires() ثابت;
	std::ویکټور<RTLIL::Cell*> selected_cells() ثابت;
	std::ویکټور<RTLIL::حافظه*> selected_memories() ثابت;
	std::ویکټور<RTLIL::Process*> selected_processes() ثابت;
	std::ویکټور<RTLIL::SelectedMember> selected_members() ثابت;

	کينډۍ<typename T> bool selected(T *member) ثابت {
		أرجع design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		إذا (design)
			design->twines().check_owned(id);
		خپلکاری it = wires_.پیدا_کړه(id);
		أرجع it == wires_.پای_ټکی() ? nullptr : it->دوهم;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		إذا (design)
			design->twines().check_owned(id);
		خپلکاری it = cells_.پیدا_کړه(id);
		أرجع it == cells_.پای_ټکی() ? nullptr : it->دوهم;
	}

	ثابت RTLIL::Wire* wire(RTLIL::IdString id) ثابت{
		إذا (design)
			design->twines().check_owned(id);
		خپلکاری it = wires_.پیدا_کړه(id);
		أرجع it == wires_.پای_ټکی() ? nullptr : it->دوهم;
	}
	ثابت RTLIL::Cell* cell(RTLIL::IdString id) ثابت {
		إذا (design)
			design->twines().check_owned(id);
		خپلکاری it = cells_.پیدا_کړه(id);
		أرجع it == cells_.پای_ټکی() ? nullptr : it->دوهم;
	}

	کينډۍ<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ أرجع wire(twines().پیدا_کړه(std::string_view(name))); }
	کينډۍ<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ أرجع cell(twines().پیدا_کړه(std::string_view(name))); }
	کينډۍ<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Wire *wire(N name) ثابت
		{ أرجع wire(twines().پیدا_کړه(std::string_view(name))); }
	کينډۍ<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Cell *cell(N name) ثابت
		{ أرجع cell(twines().پیدا_کړه(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { أرجع RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	صحيح wires_اندازه() ثابت { أرجع wires_.اندازه(); }
	RTLIL::Wire* wire_at(صحيح شاخص) ثابت { أرجع wires_.element(شاخص)->دوهم; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { أرجع RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	صحيح cells_اندازه() ثابت { أرجع cells_.اندازه(); }
	RTLIL::Cell* cell_at(صحيح شاخص) ثابت { أرجع cells_.element(شاخص)->دوهم; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	فراغ remove(ثابت pool<RTLIL::Wire*> &wires);
	فراغ remove(RTLIL::Cell *cell);
	فراغ remove(RTLIL::حافظه *حافظه_3);
	فراغ remove(RTLIL::Process *process);

	فراغ rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	فراغ rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	فراغ rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	فراغ بدلون_نومونه(RTLIL::Wire *w1, RTLIL::Wire *w2);
	فراغ بدلون_نومونه(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, صحيح &شاخص);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *جمع_کړه_Wire(RTLIL::IdString name, صحيح پلنوالی = 1);
	RTLIL::Wire *جمع_کړه_Wire(RTLIL::IdString name, ثابت RTLIL::Wire *other);
	YS_NAME_FWD(جمع_کړه_Wire)

	RTLIL::Cell *جمع_کړه_Cell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *جمع_کړه_Cell(RTLIL::IdString name, ثابت RTLIL::Cell *other);
	YS_NAME_FWD(جمع_کړه_Cell)
	کينډۍ<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *جمع_کړه_Cell(RTLIL::IdString name, T type) { أرجع جمع_کړه_Cell(name, design->twines().جمع_کړه(std::move(type))); }

	RTLIL::حافظه *جمع_کړه_حافظه(RTLIL::IdString name);
	YS_NAME_FWD(جمع_کړه_حافظه)
	RTLIL::حافظه *جمع_کړه_حافظه(RTLIL::IdString name, ثابت RTLIL::حافظه *other);

	RTLIL::Process *جمع_کړه_Process(RTLIL::IdString name);
	YS_NAME_FWD(جمع_کړه_Process)
	RTLIL::Process *جمع_کړه_Process(RTLIL::IdString name, ثابت RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* جمع_کړه_Not     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Pos     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Buf     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Barrier (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Neg     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_And  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Or   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Xor  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Xnor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_Reduce_And  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Reduce_Or   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Reduce_Xor  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Reduce_Xnor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Reduce_Bool (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_Shl    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Shr    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Sshl   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Sshr   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_ښويدنه  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Shiftx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_Lt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Le  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Eq  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Ne  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Eqx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Nex (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Ge  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Gt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_جمع_کړه (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Sub (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Mul (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	// truncating division
	RTLIL::Cell* جمع_کړه_Div (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	// truncating modulo
	RTLIL::Cell* جمع_کړه_Mod (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Div_Floor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Mod_Floor (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Pow (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_Fa (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_x, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_Logic_Not (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Logic_And (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Logic_Or  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_Mux  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Pmux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Bmux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Demux (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_Bweqx  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Bwmux  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_ټوټه  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const دننه_واټن, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Concat (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Lut    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_y, RTLIL::Const lut, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Tribuf (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Assert (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Assume (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_ژوندی   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Fair   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Cover  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_en, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Equiv  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_Sr    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr, ثابت RTLIL::SigSpec &sig_q, bool سېټ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Ff    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Dff   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d,   ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Dffe  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Dffsr (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool سېټ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Dffsre (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool سېټ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Adff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_ارزښت, bool clk_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Adffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_ارزښت, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Aldff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Aldffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Sdff (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_ارزښت, bool clk_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Sdffe (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst,  ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_ارزښت, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Sdffce (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const srst_ارزښت, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Dlatch (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Adlatch (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, RTLIL::Const arst_ارزښت, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Dlatchsr (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, bool سېټ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_Buf_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Not_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_And_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Nand_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Or_Gate     (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Nor_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Xor_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Xnor_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Andnot_Gate (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Ornot_Gate  (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Mux_Gate    (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_s, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Nmux_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_s, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Aoi3_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_c, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Oai3_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_c, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Aoi4_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_c, ثابت RTLIL::Sig_بټ &sig_d, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Oai4_Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_c, ثابت RTLIL::Sig_بټ &sig_d, ثابت RTLIL::Sig_بټ &sig_y, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_Sr_Gate     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr,
			ثابت RTLIL::SigSpec &sig_q, bool سېټ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Ff_Gate     (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Dff_Gate    (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Dffe_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Dffsr_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool سېټ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Dffsre_Gate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool سېټ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Adff_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_ارزښت = false, bool clk_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Adffe_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_ارزښت = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Aldff_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Aldffe_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_aload, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			ثابت RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Sdff_Gate   (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_ارزښت = false, bool clk_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Sdffe_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_ارزښت = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Sdffce_Gate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_clk, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_srst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool srst_ارزښت = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Dlatch_Gate (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Adlatch_Gate(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_arst, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q,
			bool arst_ارزښت = false, bool en_polarity = true, bool arst_polarity = true, ثابت std::string &src = "");
	RTLIL::Cell* جمع_کړه_Dlatchsr_Gate  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_en, ثابت RTLIL::SigSpec &sig_سېټ, ثابت RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ثابت RTLIL::SigSpec &sig_q, bool en_polarity = true, bool سېټ_polarity = true, bool clr_polarity = true, ثابت std::string &src = "");

	RTLIL::Cell* جمع_کړه_Anyinit(RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_d, ثابت RTLIL::SigSpec &sig_q, ثابت std::string &src = "");

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
	RTLIL::SigSpec ښويدنه_2  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");

	RTLIL::SigSpec جمع_کړه_2 (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_b, bool is_signed = false, ثابت std::string &src = "");
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

	RTLIL::Sig_بټ BufGate    (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت std::string &src = "");
	RTLIL::Sig_بټ NotGate    (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت std::string &src = "");
	RTLIL::Sig_بټ AndGate    (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بټ NandGate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بټ OrGate     (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بټ NorGate    (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بټ XorGate    (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بټ XnorGate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بټ AndnotGate (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بټ OrnotGate  (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت std::string &src = "");
	RTLIL::Sig_بټ MuxGate    (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_s, ثابت std::string &src = "");
	RTLIL::Sig_بټ NmuxGate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_s, ثابت std::string &src = "");
	RTLIL::Sig_بټ Aoi3Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_c, ثابت std::string &src = "");
	RTLIL::Sig_بټ Oai3Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_c, ثابت std::string &src = "");
	RTLIL::Sig_بټ Aoi4Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_c, ثابت RTLIL::Sig_بټ &sig_d, ثابت std::string &src = "");
	RTLIL::Sig_بټ Oai4Gate   (RTLIL::IdString name, ثابت RTLIL::Sig_بټ &sig_a, ثابت RTLIL::Sig_بټ &sig_b, ثابت RTLIL::Sig_بټ &sig_c, ثابت RTLIL::Sig_بټ &sig_d, ثابت std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, صحيح پلنوالی = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, صحيح پلنوالی = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, صحيح پلنوالی = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, صحيح پلنوالی = 1, ثابت std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ثابت std::string &src = "");

	RTLIL::SigSpec سېټ_نښان_2          (RTLIL::IdString name, ثابت std::string &نښان, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src = "");
	RTLIL::Cell*   جمع_کړه_سېټ_نښان       (RTLIL::IdString name, ثابت std::string &نښان, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت RTLIL::SigSpec &sig_y, ثابت std::string &src = "");
	RTLIL::SigSpec Get_نښان          (RTLIL::IdString name, ثابت std::string &نښان, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src = "");
	RTLIL::Cell*   جمع_کړه_Overwrite_نښان (RTLIL::IdString name, ثابت std::string &نښان, ثابت RTLIL::SigSpec &sig_a, ثابت RTLIL::SigSpec &sig_s, ثابت RTLIL::SigSpec &sig_c, ثابت std::string &src = "");
	RTLIL::SigSpec Original_نښان     (RTLIL::IdString name, ثابت std::string &نښان, ثابت RTLIL::SigSpec &sig_a, ثابت std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ثابت RTLIL::SigSpec &sig_e, ثابت std::string &src = "");
	YS_NAME_FWD(جمع_کړه_Anyinit)
	YS_NAME_FWD(Anyconst) YS_NAME_FWD(Anyseq) YS_NAME_FWD(Allconst) YS_NAME_FWD(Allseq)
	YS_NAME_FWD(Initstate)
	YS_NAME_FWD(سېټ_نښان_2) YS_NAME_FWD(جمع_کړه_سېټ_نښان) YS_NAME_FWD(Get_نښان)
	YS_NAME_FWD(جمع_کړه_Overwrite_نښان) YS_NAME_FWD(Original_نښان) YS_NAME_FWD(FutureFF)
	YS_NAME_FWD(جمع_کړه_Not) YS_NAME_FWD(جمع_کړه_Pos) YS_NAME_FWD(جمع_کړه_Buf) YS_NAME_FWD(جمع_کړه_Barrier) YS_NAME_FWD(جمع_کړه_Neg)
	YS_NAME_FWD(جمع_کړه_And) YS_NAME_FWD(جمع_کړه_Or) YS_NAME_FWD(جمع_کړه_Xor) YS_NAME_FWD(جمع_کړه_Xnor)
	YS_NAME_FWD(جمع_کړه_Reduce_And) YS_NAME_FWD(جمع_کړه_Reduce_Or) YS_NAME_FWD(جمع_کړه_Reduce_Xor) YS_NAME_FWD(جمع_کړه_Reduce_Xnor)
	YS_NAME_FWD(جمع_کړه_Reduce_Bool) YS_NAME_FWD(جمع_کړه_Shl) YS_NAME_FWD(جمع_کړه_Shr) YS_NAME_FWD(جمع_کړه_Sshl)
	YS_NAME_FWD(جمع_کړه_Sshr) YS_NAME_FWD(جمع_کړه_ښويدنه) YS_NAME_FWD(جمع_کړه_Shiftx) YS_NAME_FWD(جمع_کړه_Lt)
	YS_NAME_FWD(جمع_کړه_Le) YS_NAME_FWD(جمع_کړه_Eq) YS_NAME_FWD(جمع_کړه_Ne) YS_NAME_FWD(جمع_کړه_Eqx)
	YS_NAME_FWD(جمع_کړه_Nex) YS_NAME_FWD(جمع_کړه_Ge) YS_NAME_FWD(جمع_کړه_Gt) YS_NAME_FWD(جمع_کړه_جمع_کړه)
	YS_NAME_FWD(جمع_کړه_Sub) YS_NAME_FWD(جمع_کړه_Mul) YS_NAME_FWD(جمع_کړه_Div) YS_NAME_FWD(جمع_کړه_Mod)
	YS_NAME_FWD(جمع_کړه_Div_Floor) YS_NAME_FWD(جمع_کړه_Mod_Floor) YS_NAME_FWD(جمع_کړه_Pow) YS_NAME_FWD(جمع_کړه_Fa)
	YS_NAME_FWD(جمع_کړه_Logic_Not) YS_NAME_FWD(جمع_کړه_Logic_And) YS_NAME_FWD(جمع_کړه_Logic_Or) YS_NAME_FWD(جمع_کړه_Mux)
	YS_NAME_FWD(جمع_کړه_Pmux) YS_NAME_FWD(جمع_کړه_Bmux) YS_NAME_FWD(جمع_کړه_Demux) YS_NAME_FWD(جمع_کړه_Bweqx)
	YS_NAME_FWD(جمع_کړه_Bwmux) YS_NAME_FWD(جمع_کړه_ټوټه) YS_NAME_FWD(جمع_کړه_Concat) YS_NAME_FWD(جمع_کړه_Lut)
	YS_NAME_FWD(جمع_کړه_Tribuf) YS_NAME_FWD(جمع_کړه_Assert) YS_NAME_FWD(جمع_کړه_Assume) YS_NAME_FWD(جمع_کړه_ژوندی)
	YS_NAME_FWD(جمع_کړه_Fair) YS_NAME_FWD(جمع_کړه_Cover) YS_NAME_FWD(جمع_کړه_Equiv) YS_NAME_FWD(جمع_کړه_Sr)
	YS_NAME_FWD(جمع_کړه_Ff) YS_NAME_FWD(جمع_کړه_Dff) YS_NAME_FWD(جمع_کړه_Dffe) YS_NAME_FWD(جمع_کړه_Dffsr)
	YS_NAME_FWD(جمع_کړه_Dffsre) YS_NAME_FWD(جمع_کړه_Adff) YS_NAME_FWD(جمع_کړه_Adffe) YS_NAME_FWD(جمع_کړه_Aldff)
	YS_NAME_FWD(جمع_کړه_Aldffe) YS_NAME_FWD(جمع_کړه_Sdff) YS_NAME_FWD(جمع_کړه_Sdffe) YS_NAME_FWD(جمع_کړه_Sdffce)
	YS_NAME_FWD(جمع_کړه_Dlatch) YS_NAME_FWD(جمع_کړه_Adlatch) YS_NAME_FWD(جمع_کړه_Dlatchsr) YS_NAME_FWD(جمع_کړه_Buf_Gate)
	YS_NAME_FWD(جمع_کړه_Not_Gate) YS_NAME_FWD(جمع_کړه_And_Gate) YS_NAME_FWD(جمع_کړه_Nand_Gate) YS_NAME_FWD(جمع_کړه_Or_Gate)
	YS_NAME_FWD(جمع_کړه_Nor_Gate) YS_NAME_FWD(جمع_کړه_Xor_Gate) YS_NAME_FWD(جمع_کړه_Xnor_Gate) YS_NAME_FWD(جمع_کړه_Andnot_Gate)
	YS_NAME_FWD(جمع_کړه_Ornot_Gate) YS_NAME_FWD(جمع_کړه_Mux_Gate) YS_NAME_FWD(جمع_کړه_Nmux_Gate) YS_NAME_FWD(جمع_کړه_Aoi3_Gate)
	YS_NAME_FWD(جمع_کړه_Oai3_Gate) YS_NAME_FWD(جمع_کړه_Aoi4_Gate) YS_NAME_FWD(جمع_کړه_Oai4_Gate) YS_NAME_FWD(جمع_کړه_Sr_Gate)
	YS_NAME_FWD(جمع_کړه_Ff_Gate) YS_NAME_FWD(جمع_کړه_Dff_Gate) YS_NAME_FWD(جمع_کړه_Dffe_Gate) YS_NAME_FWD(جمع_کړه_Dffsr_Gate)
	YS_NAME_FWD(جمع_کړه_Dffsre_Gate) YS_NAME_FWD(جمع_کړه_Adff_Gate) YS_NAME_FWD(جمع_کړه_Adffe_Gate) YS_NAME_FWD(جمع_کړه_Aldff_Gate)
	YS_NAME_FWD(جمع_کړه_Aldffe_Gate) YS_NAME_FWD(جمع_کړه_Sdff_Gate) YS_NAME_FWD(جمع_کړه_Sdffe_Gate) YS_NAME_FWD(جمع_کړه_Sdffce_Gate)
	YS_NAME_FWD(جمع_کړه_Dlatch_Gate) YS_NAME_FWD(جمع_کړه_Adlatch_Gate) YS_NAME_FWD(جمع_کړه_Dlatchsr_Gate) YS_NAME_FWD(Not)
	YS_NAME_FWD(Pos) YS_NAME_FWD(Buf) YS_NAME_FWD(Barrier) YS_NAME_FWD(Neg) YS_NAME_FWD(And)
	YS_NAME_FWD(Or) YS_NAME_FWD(Xor) YS_NAME_FWD(Xnor) YS_NAME_FWD(ReduceAnd)
	YS_NAME_FWD(ReduceOr) YS_NAME_FWD(ReduceXor) YS_NAME_FWD(ReduceXnor) YS_NAME_FWD(ReduceBool)
	YS_NAME_FWD(Shl) YS_NAME_FWD(Shr) YS_NAME_FWD(Sshl) YS_NAME_FWD(Sshr)
	YS_NAME_FWD(ښويدنه_2) YS_NAME_FWD(Shiftx) YS_NAME_FWD(Lt) YS_NAME_FWD(Le)
	YS_NAME_FWD(Eq) YS_NAME_FWD(Ne) YS_NAME_FWD(Eqx) YS_NAME_FWD(Nex)
	YS_NAME_FWD(Ge) YS_NAME_FWD(Gt) YS_NAME_FWD(جمع_کړه_2) YS_NAME_FWD(Sub)
	YS_NAME_FWD(Mul) YS_NAME_FWD(Div) YS_NAME_FWD(Mod) YS_NAME_FWD(DivFloor)
	YS_NAME_FWD(ModFloor) YS_NAME_FWD(Pow) YS_NAME_FWD(LogicNot) YS_NAME_FWD(LogicAnd)
	YS_NAME_FWD(LogicOr) YS_NAME_FWD(Mux) YS_NAME_FWD(Pmux) YS_NAME_FWD(Bmux)
	YS_NAME_FWD(Demux) YS_NAME_FWD(Bweqx) YS_NAME_FWD(Bwmux) YS_NAME_FWD(BufGate)
	YS_NAME_FWD(NotGate) YS_NAME_FWD(AndGate) YS_NAME_FWD(NandGate) YS_NAME_FWD(OrGate)
	YS_NAME_FWD(NorGate) YS_NAME_FWD(XorGate) YS_NAME_FWD(XnorGate) YS_NAME_FWD(AndnotGate)
	YS_NAME_FWD(OrnotGate) YS_NAME_FWD(MuxGate) YS_NAME_FWD(NmuxGate) YS_NAME_FWD(Aoi3Gate)
	YS_NAME_FWD(Oai3Gate) YS_NAME_FWD(Aoi4Gate) YS_NAME_FWD(Oai4Gate)

	std::string منزل_rtlil_str() ثابت;
#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	ساكن std::map<بینښه صحيح, RTLIL::Module*> *get_all_modules(فراغ);
#د_که_پای
};

بنية RTLIL::Wire : عام RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	// use module->addWire() and module->remove() to create or destroy wires
	ملګری بنية RTLIL::Module;
	Wire();
	~Wire();

	ملګری بنية RTLIL::Design;
	ملګری بنية RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driver_پورټ_2;

عام:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	فراغ چلوونکی=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	صحيح پلنوالی, پیل_دننه_واټن, پورټ_id;
	bool پورټ_ننوت, پورټ_output, upto, is_signed;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	bool معلوم_driver() ثابت { أرجع driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ثابت    { log_assert(driverCell_); أرجع driverCell_; };
	RTLIL::IdString driver_پورټ() ثابت { log_assert(driverCell_); أرجع driver_پورټ_2; };

	صحيح from_hdl_شاخص(صحيح hdl_شاخص) {
		صحيح zero_شاخص = hdl_شاخص - پیل_دننه_واټن;
		صحيح rtlil_شاخص = upto ? پلنوالی - 1 - zero_شاخص : zero_شاخص;
		أرجع rtlil_شاخص >= 0 && rtlil_شاخص < پلنوالی ? rtlil_شاخص : INT_MIN;
	}

	صحيح منزل_hdl_شاخص(صحيح rtlil_شاخص) {
		إذا (rtlil_شاخص < 0 || rtlil_شاخص >= پلنوالی)
			أرجع INT_MIN;
		صحيح zero_شاخص = upto ? پلنوالی - 1 - rtlil_شاخص : rtlil_شاخص;
		أرجع zero_شاخص + پیل_دننه_واټن;
	}

	std::string منزل_rtlil_str() ثابت;

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	ساكن std::map<بینښه صحيح, RTLIL::Wire*> *get_all_wires(فراغ);
#د_که_پای
};

مضمن صحيح Get_اندازه(RTLIL::Wire *wire) {
	أرجع wire->پلنوالی;
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

	صحيح پلنوالی, پیل_دننه_واټن, اندازه;
#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	ساكن std::map<بینښه صحيح, RTLIL::حافظه*> *get_all_memorys(فراغ);
#د_که_پای

	std::string منزل_rtlil_str() ثابت;
};

بنية RTLIL::Cell : عام RTLIL::AttrObject
{
خاص:
	فراغ init_شاخص();

	bool bufnorm_handle_سېټ_پورټ(IdString portname, RTLIL::SigSpec &إشارة, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
عام:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	// use module->addCell() and module->remove() to create or destroy cells
	ملګری بنية RTLIL::Module;
	Cell();
	~Cell();

عام:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	فراغ چلوونکی=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool has_پورټ(RTLIL::IdString portname) ثابت;
	فراغ unset_پورټ(RTLIL::IdString portname);
	فراغ سېټ_پورټ(RTLIL::IdString portname, RTLIL::SigSpec إشارة);
	کينډۍ<typename N, YS_NAME_STRING(N)> فراغ سېټ_پورټ(N portname, RTLIL::SigSpec إشارة)
		{ سېټ_پورټ(module->twines().جمع_کړه(std::move(portname)), std::move(إشارة)); }
	ثابت RTLIL::SigSpec &get_پورټ(RTLIL::IdString portname) ثابت;
	ثابت dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ثابت;

	bool معلوم() ثابت;
	bool ننوت(RTLIL::IdString portname) ثابت;
	bool output(RTLIL::IdString portname) ثابت;
	پورټ_Dir پورټ_dir(RTLIL::IdString portname) ثابت;

	bool hasParam(RTLIL::IdString paramname) ثابت;
	فراغ unsetParam(RTLIL::IdString paramname);
	فراغ سېټ_Param(RTLIL::IdString paramname, RTLIL::Const ارزښت);
	ثابت RTLIL::Const &getParam(RTLIL::IdString paramname) ثابت;

	کينډۍ<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ثابت
		{ أرجع hasParam(module->twines().پیدا_کړه(std::string_view(name))); }
	کينډۍ<typename N, YS_NAME_STRING(N)> فراغ unsetParam(N name)
		{ unsetParam(module->twines().پیدا_کړه(std::string_view(name))); }
	کينډۍ<typename N, YS_NAME_STRING(N)> فراغ سېټ_Param(N name, RTLIL::Const ارزښت)
		{ سېټ_Param(module->twines().جمع_کړه(std::move(name)), std::move(ارزښت)); }
	کينډۍ<typename N, YS_NAME_STRING(N)> ثابت RTLIL::Const &getParam(N name) ثابت
		{ أرجع getParam(module->twines().پیدا_کړه(std::string_view(name))); }

	فراغ ترتیبول();
	فراغ check();
	فراغ fixup_parameters(bool سېټ_a_signed = false, bool سېټ_b_signed = false);

	bool has_keep_attr() ثابت;

	کينډۍ<typename T> فراغ rewrite_sigspecs(T &functor);
	کينډۍ<typename T> فراغ rewrite_sigspecs2(T &functor);

#که_تعریف_شوی YOSYS_ENABLE_PYTHON
	ساكن std::map<بینښه صحيح, RTLIL::Cell*> *get_all_cells(فراغ);
#د_که_پای

	bool has_memid() ثابت;
	bool is_mem_cell() ثابت;
	bool is_builtin_ff() ثابت;

	std::string منزل_rtlil_str() ثابت;
};

بنية RTLIL::Case_قاعده : عام RTLIL::AttrObject
{
	std::ویکټور<RTLIL::SigSpec> compare;
	std::ویکټور<RTLIL::SigSig> actions;
	std::ویکټور<RTLIL::Switch_قاعده*> switches;

	~Case_قاعده();

	bool تشه() ثابت;

	کينډۍ<typename T> فراغ rewrite_sigspecs(T &functor);
	کينډۍ<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Case_قاعده *clone() ثابت;
};

بنية RTLIL::Switch_قاعده : عام RTLIL::AttrObject
{
	RTLIL::SigSpec إشارة;
	std::ویکټور<RTLIL::Case_قاعده*> cases;

	~Switch_قاعده();

	bool تشه() ثابت;

	کينډۍ<typename T> فراغ rewrite_sigspecs(T &functor);
	کينډۍ<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Switch_قاعده *clone() ثابت;
};

بنية RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec پته;
	RTLIL::SigSpec معلومات;
	RTLIL::SigSpec enable;
	RTLIL::Const لومړیتوب_ماسک;
};

بنية RTLIL::Sync_قاعده
{
	RTLIL::SyncType type;
	RTLIL::SigSpec إشارة;
	std::ویکټور<RTLIL::SigSig> actions;
	std::ویکټور<RTLIL::MemWriteAction> mem_write_actions;

	کينډۍ<typename T> فراغ rewrite_sigspecs(T &functor);
	کينډۍ<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Sync_قاعده *clone() ثابت;
};

بنية RTLIL::Process : عام RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(hashidx_); أرجع h; }

محمي:
	// use module->addProcess() and module->remove() to create or destroy processes
	ملګری بنية RTLIL::Module;
	Process();
	~Process();

عام:
	RTLIL::Module *module;
	RTLIL::Case_قاعده ریښه_case;
	std::ویکټور<RTLIL::Sync_قاعده*> syncs;

	RTLIL::Design *design() ثابت { أرجع module ? module->design : nullptr; }
	TwinePool &twines() ثابت { أرجع module->twines(); }

	کينډۍ<typename T> فراغ rewrite_sigspecs(T &functor);
	کينډۍ<typename T> فراغ rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ثابت;

	std::string منزل_rtlil_str() ثابت;
};

مضمن RTLIL::Sig_بټ::Sig_بټ() : wire(عدم), معلومات(RTLIL::State::S0) { }
مضمن RTLIL::Sig_بټ::Sig_بټ(RTLIL::State بټ) : wire(عدم), معلومات(بټ) { }
مضمن RTLIL::Sig_بټ::Sig_بټ(bool بټ) : wire(عدم), معلومات(بټ ? State::S1 : State::S0) { }
مضمن RTLIL::Sig_بټ::Sig_بټ(RTLIL::Wire *wire) : wire(wire), دننه_واټن(0) { log_assert(wire && wire->پلنوالی == 1); }
مضمن RTLIL::Sig_بټ::Sig_بټ(RTLIL::Wire *wire, صحيح دننه_واټن) : wire(wire), دننه_واټن(دننه_واټن) { log_assert(wire != nullptr); }
مضمن RTLIL::Sig_بټ::Sig_بټ(ثابت RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.پلنوالی == 1); إذا (wire) دننه_واټن = chunk.دننه_واټن; وإلا معلومات = chunk.معلومات[0]; }
مضمن RTLIL::Sig_بټ::Sig_بټ(ثابت RTLIL::SigChunk &chunk, صحيح شاخص) : wire(chunk.wire) { إذا (wire) دننه_واټن = chunk.دننه_واټن + شاخص; وإلا معلومات = chunk.معلومات[شاخص]; }

مضمن bool RTLIL::Sig_بټ::چلوونکی<(ثابت RTLIL::Sig_بټ &other) ثابت {
	إذا (wire == other.wire)
		أرجع wire ? (دننه_واټن < other.دننه_واټن) : (معلومات < other.معلومات);
	إذا (wire != nullptr && other.wire != nullptr)
		أرجع wire->name < other.wire->name;
	أرجع (wire != nullptr) < (other.wire != nullptr);
}

مضمن bool RTLIL::Sig_بټ::چلوونکی==(ثابت RTLIL::Sig_بټ &other) ثابت {
	أرجع (wire == other.wire) && (wire ? (دننه_واټن == other.دننه_واټن) : (معلومات == other.معلومات));
}

مضمن bool RTLIL::Sig_بټ::چلوونکی!=(ثابت RTLIL::Sig_بټ &other) ثابت {
	أرجع (wire != other.wire) || (wire ? (دننه_واټن != other.دننه_واټن) : (معلومات != other.معلومات));
}

مضمن Hasher RTLIL::Sig_بټ::hash_into(Hasher h) ثابت {
	إذا (wire) {
		h.eat(دننه_واټن);
		h.eat(wire->name);
		أرجع h;
	}
	h.eat(معلومات);
	أرجع h;
}


مضمن Hasher RTLIL::Sig_بټ::hash_سر() ثابت {
	Hasher h;
	إذا (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.بټونه();
		h.force(hashlib::legacy::djb2_جمع_کړه(n, دننه_واټن));
		أرجع h;
	}
	h.force(معلومات);
	أرجع h;
}

مضمن RTLIL::Sig_بټ &RTLIL::SigSpecIterator::چلوونکی*() ثابت {
	أرجع (*sig_p)[شاخص];
}

مضمن ثابت RTLIL::Sig_بټ &RTLIL::SigSpecConstIterator::چلوونکی*() {
	بټ = (*sig_p)[شاخص];
	أرجع بټ;
}

مضمن RTLIL::Sig_بټ::Sig_بټ(ثابت RTLIL::SigSpec &sig) {
	log_assert(sig.اندازه() == 1);
	خپلکاری it = sig.chunks().begin();
	*this = Sig_بټ(*it);
}

کينډۍ<typename T>
فراغ RTLIL::Module::rewrite_sigspecs(T &functor)
{
	لكل (خپلکاری &it : cells_)
		it.دوهم->rewrite_sigspecs(functor);
	لكل (خپلکاری &it : processes)
		it.دوهم->rewrite_sigspecs(functor);
	لكل (خپلکاری &it : connections_) {
		functor(it.لومړی);
		functor(it.دوهم);
	}
}

کينډۍ<typename T>
فراغ RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	لكل (خپلکاری &it : cells_)
		it.دوهم->rewrite_sigspecs2(functor);
	لكل (خپلکاری &it : processes)
		it.دوهم->rewrite_sigspecs2(functor);
	لكل (خپلکاری &it : connections_) {
		functor(it.لومړی, it.دوهم);
	}
}

کينډۍ<typename T>
فراغ RTLIL::Cell::rewrite_sigspecs(T &functor) {
	لكل (خپلکاری &it : connections_)
		functor(it.دوهم);
}

کينډۍ<typename T>
فراغ RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	لكل (خپلکاری &it : connections_)
		functor(it.دوهم);
}

کينډۍ<typename T>
فراغ RTLIL::Case_قاعده::rewrite_sigspecs(T &functor) {
	لكل (خپلکاری &it : compare)
		functor(it);
	لكل (خپلکاری &it : actions) {
		functor(it.لومړی);
		functor(it.دوهم);
	}
	لكل (خپلکاری it : switches)
		it->rewrite_sigspecs(functor);
}

کينډۍ<typename T>
فراغ RTLIL::Case_قاعده::rewrite_sigspecs2(T &functor) {
	لكل (خپلکاری &it : compare)
		functor(it);
	لكل (خپلکاری &it : actions) {
		functor(it.لومړی, it.دوهم);
	}
	لكل (خپلکاری it : switches)
		it->rewrite_sigspecs2(functor);
}

کينډۍ<typename T>
فراغ RTLIL::Switch_قاعده::rewrite_sigspecs(T &functor)
{
	functor(إشارة);
	لكل (خپلکاری it : cases)
		it->rewrite_sigspecs(functor);
}

کينډۍ<typename T>
فراغ RTLIL::Switch_قاعده::rewrite_sigspecs2(T &functor)
{
	functor(إشارة);
	لكل (خپلکاری it : cases)
		it->rewrite_sigspecs2(functor);
}

کينډۍ<typename T>
فراغ RTLIL::Sync_قاعده::rewrite_sigspecs(T &functor)
{
	functor(إشارة);
	لكل (خپلکاری &it : actions) {
		functor(it.لومړی);
		functor(it.دوهم);
	}
	لكل (خپلکاری &it : mem_write_actions) {
		functor(it.پته);
		functor(it.معلومات);
		functor(it.enable);
	}
}

کينډۍ<typename T>
فراغ RTLIL::Sync_قاعده::rewrite_sigspecs2(T &functor)
{
	functor(إشارة);
	لكل (خپلکاری &it : actions) {
		functor(it.لومړی, it.دوهم);
	}
	لكل (خپلکاری &it : mem_write_actions) {
		functor(it.پته);
		functor(it.معلومات);
		functor(it.enable);
	}
}

کينډۍ<typename T>
فراغ RTLIL::Process::rewrite_sigspecs(T &functor)
{
	ریښه_case.rewrite_sigspecs(functor);
	لكل (خپلکاری it : syncs)
		it->rewrite_sigspecs(functor);
}

کينډۍ<typename T>
فراغ RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	ریښه_case.rewrite_sigspecs2(functor);
	لكل (خپلکاری it : syncs)
		it->rewrite_sigspecs2(functor);
}

#ضمن "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
