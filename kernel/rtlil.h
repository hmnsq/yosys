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

#यदि_अपरिभाषितम् RTLIL_H
#परिभाषय RTLIL_H

#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/twine.h"

#अन्तर्भाव <deque>
#अन्तर्भाव <string_view>
#अन्तर्भाव <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	गणना State : अचिह्नित वर्ण {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	गणना SyncType : अचिह्नित वर्ण {
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
	गणना ConstFlags : अचिह्नित वर्ण {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	गणना SelectPartials : अचिह्नित वर्ण {
		SELECT_ALL = 0,          // include partial modules
		SELECT_सम्पूर्णम्_ONLY = 1,   // ignore partial modules
		SELECT_सम्पूर्णम्_WARN = 2,   // call log_warning on partial module
		SELECT_सम्पूर्णम्_ERR = 3,    // call log_error on partial module
		SELECT_सम्पूर्णम्_CMDERR = 4  // call log_cmd_error on partial module
	};

	गणना SelectBoxes : अचिह्नित वर्ण {
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

	गणना पत्तनम्_Dir : अचिह्नित वर्ण  {
		PD_UNKNOWN = 0,
		PD_निवेशः = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr पूर्णाङ्क विस्तारः_सीमा = 1 << 30;

	संरचना Const;
	संरचना AttrObject;
	संरचना SelectedMember;
	संरचना Selection;
	संरचना Monitor;
	संरचना Design;
	संरचना Module;
	संरचना Wire;
	संरचना स्मृतिः;
	संरचना Cell;
	संरचना SigChunk;
	संरचना SigBit;
	संरचना SigSpecIterator;
	संरचना SigSpecConstIterator;
	संरचना SigSpec;
	संरचना Case_नियमः;
	संरचना Switch_नियमः;
	संरचना MemWriteAction;
	संरचना Sync_नियमः;
	संरचना Process;

	प्रकारनिर्देश std::pair<SigSpec, SigSpec> SigSig;
};

संरचना SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_उपसर्गः ID; }
namespace RTLIL { using YOSYS_NAMESPACE_उपसर्गः IdString; }

namespace RTLIL {
	शून्य copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			ध्रुव dict<IdString, RTLIL::Const> &src,
			ध्रुव RTLIL::Design *src_design, RTLIL::Design *dst_design);

	बाह्य dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	ध्रुव pool<IdString> &builtin_ff_cell_types();

	स्थिर अन्तःस्थ std::string escape_id(ध्रुव std::string &str) {
		यदि (str.आकार() > 0 && str[0] != '\\' && str[0] != '$')
			प्रत्यागम "\\" + str;
		प्रत्यागम str;
	}

	स्थिर अन्तःस्थ std::string unescape_id(ध्रुव std::string &str) {
		यदि (str.आकार() < 2)
			प्रत्यागम str;
		यदि (str[0] != '\\')
			प्रत्यागम str;
		यदि (str[1] == '$' || str[1] == '\\')
			प्रत्यागम str;
		यदि (str[1] >= '0' && str[1] <= '9')
			प्रत्यागम str;
		प्रत्यागम str.substr(1);
	}

	प्रतिरूपम् <typename T> using क्रमय_by_name_id = IdString::compare_ptr_by_name<T>;

	प्रतिरूपम् <typename T> संरचना क्रमय_by_name_str {
		bool संक्रियकः()(T *a, T *b) ध्रुव {
			प्रत्यागम a->name.lt_by_name(b->name);
		}
	};

	संरचना क्रमय_by_id_str {
		ध्रुव TwinePool& pool;
		explicit क्रमय_by_id_str(ध्रुव TwinePool& pool) : pool(pool) {}
		bool संक्रियकः()(RTLIL::IdString a, RTLIL::IdString b) ध्रुव {
			प्रत्यागम pool.compare_by_name(a, b) < 0;
		}
	};

	स्थिर अन्तःस्थ std::string encode_filename(ध्रुव std::string &filename)
	{
		std::stringstream val;
		यदि (!std::any_सम्बन्धि(filename.begin(), filename.अन्तः(), [](वर्ण c) {
			प्रत्यागम static_cast<अचिह्नित वर्ण>(c) < 33 || static_cast<अचिह्नित वर्ण>(c) > 126;
		})) प्रत्यागम filename;
		प्रति (अचिह्नित वर्ण ध्रुव c : filename) {
			यदि (c < 33 || c > 126)
				val << stringf("$%02x", c);
			अन्यथा
				val << c;
		}
		प्रत्यागम val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_and         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_or          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_xor         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_xnor        (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);

	RTLIL::Const const_reduce_and  (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_reduce_or   (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_reduce_xor  (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_reduce_xnor (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_reduce_bool (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);

	RTLIL::Const const_logic_not   (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_logic_and   (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_logic_or    (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);

	RTLIL::Const const_shl         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_shr         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_sshl        (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_sshr        (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_सरणम्       (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_shiftx      (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);

	RTLIL::Const const_lt          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_le          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_eq          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_ne          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_eqx         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_nex         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_ge          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_gt          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);

	RTLIL::Const const_सङ्कलय         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_sub         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_mul         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_div         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_divfloor    (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_modfloor    (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_mod         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_pow         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);

	RTLIL::Const const_pos         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_buf         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);
	RTLIL::Const const_neg         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क परिणाम_len);

	RTLIL::Const const_mux         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, ध्रुव RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, ध्रुव RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, ध्रुव RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	प्रतिरूपम्<typename T, typename कुञ्चिका_2 = IdString>
	संरचना ObjIterator {
		using iterator_category = std::forward_iterator_चिह्नकम्;
		using मानम्_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		typename dict<कुञ्चिका_2, T>::iterator it;
		dict<कुञ्चिका_2, T> *list_p;
		पूर्णाङ्क *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, पूर्णाङ्क *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			यदि (list_p->रिक्तानि()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} अन्यथा {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(ध्रुव RTLIL::ObjIterator<T, कुञ्चिका_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			यदि (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &संक्रियकः=(ध्रुव RTLIL::ObjIterator<T, कुञ्चिका_2> &other) {
			यदि (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			यदि (refcount_p)
				(*refcount_p)++;
			प्रत्यागम *this;
		}

		~ObjIterator() {
			यदि (refcount_p)
				(*refcount_p)--;
		}

		अन्तःस्थ T संक्रियकः*() ध्रुव {
			log_assert(list_p != nullptr);
			प्रत्यागम it->second;
		}

		अन्तःस्थ bool संक्रियकः!=(ध्रुव RTLIL::ObjIterator<T, कुञ्चिका_2> &other) ध्रुव {
			यदि (list_p == nullptr || other.list_p == nullptr)
				प्रत्यागम list_p != other.list_p;
			प्रत्यागम it != other.it;
		}


		अन्तःस्थ bool संक्रियकः==(ध्रुव RTLIL::ObjIterator<T, कुञ्चिका_2> &other) ध्रुव {
			प्रत्यागम !(*this != other);
		}

		अन्तःस्थ ObjIterator<T, कुञ्चिका_2>& संक्रियकः++() {
			log_assert(list_p != nullptr);
			यदि (++it == list_p->अन्तः()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			प्रत्यागम *this;
		}

		अन्तःस्थ ObjIterator<T, कुञ्चिका_2>& संक्रियकः+=(पूर्णाङ्क amt) {
			log_assert(list_p != nullptr);
			it += amt;
			यदि (it == list_p->अन्तः()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			प्रत्यागम *this;
		}

		अन्तःस्थ ObjIterator<T, कुञ्चिका_2> संक्रियकः+(पूर्णाङ्क amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, कुञ्चिका_2> new_obj(*this);
			new_obj.it += amt;
			यदि (new_obj.it == list_p->अन्तः()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			प्रत्यागम new_obj;
		}

		अन्तःस्थ ध्रुव ObjIterator<T, कुञ्चिका_2> संक्रियकः++(पूर्णाङ्क) {
			ObjIterator<T, कुञ्चिका_2> परिणाम(*this);
			++(*this);
			प्रत्यागम परिणाम;
		}
	};

	प्रतिरूपम्<typename T, typename कुञ्चिका_2 = IdString>
	संरचना ObjRange
	{
		dict<कुञ्चिका_2, T> *list_p;
		पूर्णाङ्क *refcount_p;

		ObjRange(decltype(list_p) list_p, पूर्णाङ्क *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, कुञ्चिका_2> begin() { प्रत्यागम RTLIL::ObjIterator<T, कुञ्चिका_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, कुञ्चिका_2> अन्तः() { प्रत्यागम RTLIL::ObjIterator<T, कुञ्चिका_2>(); }

		आकार_t आकार() ध्रुव {
			प्रत्यागम list_p->आकार();
		}

		संक्रियकः pool<T>() ध्रुव {
			pool<T> परिणाम;
			प्रति (स्वचालित &it : *list_p)
				परिणाम.निवेशय(it.second);
			प्रत्यागम परिणाम;
		}

		संक्रियकः std::सदिशम्<T>() ध्रुव {
			std::सदिशम्<T> परिणाम;
			परिणाम.reserve(list_p->आकार());
			प्रति (स्वचालित &it : *list_p)
				परिणाम.प्रक्षिप_पृष्ठभागः(it.second);
			प्रत्यागम परिणाम;
		}

		pool<T> गन्तव्यम्_pool() ध्रुव { प्रत्यागम *this; }
		std::सदिशम्<T> गन्तव्यम्_सदिशम्() ध्रुव { प्रत्यागम *this; }
	};
};

संरचना RTLIL::Const
{
	ह्रस्व पूर्णाङ्क flags;
गुप्त:
	मित्रम् class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::सदिशम्<RTLIL::State>;
	गणना class backing_चिह्नकम्: bool { द्वयङ्काः, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_चिह्नकम् चिह्नकम्;
	सङ्घ {
		bitvectype द्वयङ्काः_2;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_द्वयङ्काः() ध्रुव { प्रत्यागम चिह्नकम् == backing_चिह्नकम्::द्वयङ्काः; }
	bool is_str() ध्रुव { प्रत्यागम चिह्नकम् == backing_चिह्नकम्::string; }

	bitvectype* get_if_द्वयङ्काः() { प्रत्यागम is_द्वयङ्काः() ? &द्वयङ्काः_2 : रिक्त; }
	std::string* get_if_str() { प्रत्यागम is_str() ? &str_ : रिक्त; }
	ध्रुव bitvectype* get_if_द्वयङ्काः() ध्रुव { प्रत्यागम is_द्वयङ्काः() ? &द्वयङ्काः_2 : रिक्त; }
	ध्रुव std::string* get_if_str() ध्रुव { प्रत्यागम is_str() ? &str_ : रिक्त; }

	bitvectype& get_द्वयङ्काः();
	std::string& get_str();
	ध्रुव bitvectype& get_द्वयङ्काः() ध्रुव;
	ध्रुव std::string& get_str() ध्रुव;
	std::सदिशम्<RTLIL::State>& द्वयङ्काः_आन्तरिकम्();
	शून्य bitvectorize_आन्तरिकम्();

सार्वजनिक:
	Const() : flags(RTLIL::CONST_FLAG_NONE), चिह्नकम्(backing_चिह्नकम्::द्वयङ्काः), द्वयङ्काः_2(std::सदिशम्<RTLIL::State>()) {}
	Const(std::string str);
	Const(दीर्घ दीर्घ val); // default width is 32
	Const(दीर्घ दीर्घ val, पूर्णाङ्क विस्तारः);
	Const(RTLIL::State bit, पूर्णाङ्क विस्तारः = 1);
	Const(std::सदिशम्<RTLIL::State> द्वयङ्काः) : flags(RTLIL::CONST_FLAG_NONE), चिह्नकम्(backing_चिह्नकम्::द्वयङ्काः), द्वयङ्काः_2(std::move(द्वयङ्काः)) {}
	Const(ध्रुव std::सदिशम्<bool> &द्वयङ्काः);
	Const(ध्रुव RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &संक्रियकः =(ध्रुव RTLIL::Const &other);
	~Const();

	संरचना Builder
	{
		Builder() {}
		Builder(पूर्णाङ्क expected_विस्तारः) { द्वयङ्काः.reserve(expected_विस्तारः); }
		शून्य प्रक्षिप_पृष्ठभागः(RTLIL::State b) { द्वयङ्काः.प्रक्षिप_पृष्ठभागः(b); }
		पूर्णाङ्क आकार() ध्रुव { प्रत्यागम static_cast<पूर्णाङ्क>(द्वयङ्काः.आकार()); }
		Const build() { प्रत्यागम Const(std::move(द्वयङ्काः)); }
	गुप्त:
		std::सदिशम्<RTLIL::State> द्वयङ्काः;
	};

	bool संक्रियकः <(ध्रुव RTLIL::Const &other) ध्रुव;
	bool संक्रियकः ==(ध्रुव RTLIL::Const &other) ध्रुव;
	bool संक्रियकः !=(ध्रुव RTLIL::Const &other) ध्रुव;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::सदिशम्<RTLIL::State>& द्वयङ्काः() { प्रत्यागम द्वयङ्काः_आन्तरिकम्(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	शून्य bitvectorize() ध्रुव { const_cast<Const*>(this)->bitvectorize_आन्तरिकम्(); }

	bool as_bool() ध्रुव;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	पूर्णाङ्क as_int(bool is_signed = false) ध्रुव;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_गन्तव्यम्_int(bool is_signed = false) ध्रुव;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<पूर्णाङ्क> try_as_int(bool is_signed = false) ध्रुव;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	पूर्णाङ्क as_int_saturating(bool is_signed = false) ध्रुव;

	शून्य चिह्नकम्_bare_integer_const(ध्रुव std::string &मानम्);

	std::string as_string(ध्रुव वर्ण* any = "-") ध्रुव;
	स्थिर Const from_string(ध्रुव std::string &str);
	std::सदिशम्<RTLIL::State> गन्तव्यम्_द्वयङ्काः() ध्रुव;

	std::string decode_string() ध्रुव;
	पूर्णाङ्क आकार() ध्रुव;
	bool रिक्तानि() ध्रुव;

	शून्य append(ध्रुव RTLIL::Const &other);
	शून्य समुच्चयः(पूर्णाङ्क i, RTLIL::State state) {
		द्वयङ्काः_आन्तरिकम्()[i] = state;
	}
	शून्य resize(पूर्णाङ्क आकार, RTLIL::State fill) {
    log_assert(आकार >= 0 && आकार < RTLIL::विस्तारः_सीमा);
		द्वयङ्काः_आन्तरिकम्().resize(आकार, fill);
	}

	class const_iterator {
	गुप्त:
		ध्रुव Const* parent;
		आकार_t idx;

	सार्वजनिक:
		using iterator_category = std::bidirectional_iterator_चिह्नकम्;
		using मानम्_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = ध्रुव State*;
		using reference = ध्रुव State&;

		const_iterator(ध्रुव Const& c, आकार_t i) : parent(&c), idx(i) {}

		State संक्रियकः*() ध्रुव;

		const_iterator& संक्रियकः++() { ++idx; प्रत्यागम *this; }
		const_iterator& संक्रियकः--() { --idx; प्रत्यागम *this; }
		const_iterator संक्रियकः++(पूर्णाङ्क) { const_iterator परिणाम(*this); ++idx; प्रत्यागम परिणाम; }
		const_iterator संक्रियकः--(पूर्णाङ्क) { const_iterator परिणाम(*this); --idx; प्रत्यागम परिणाम; }
		const_iterator& संक्रियकः+=(पूर्णाङ्क i) { idx += i; प्रत्यागम *this; }

		const_iterator संक्रियकः+(पूर्णाङ्क सङ्कलय) {
			प्रत्यागम const_iterator(*parent, idx + सङ्कलय);
		}
		const_iterator संक्रियकः-(पूर्णाङ्क sub) {
			प्रत्यागम const_iterator(*parent, idx - sub);
		}
		पूर्णाङ्क संक्रियकः-(ध्रुव const_iterator& other) {
			प्रत्यागम idx - other.idx;
		}

		bool संक्रियकः==(ध्रुव const_iterator& other) ध्रुव {
			प्रत्यागम idx == other.idx;
		}

		bool संक्रियकः!=(ध्रुव const_iterator& other) ध्रुव {
			प्रत्यागम !(*this == other);
		}
	};

	class iterator {
	गुप्त:
		Const* parent;
		आकार_t idx;

	सार्वजनिक:
		class proxy {
		गुप्त:
			Const* parent;
			आकार_t idx;
		सार्वजनिक:
			proxy(Const* parent, आकार_t idx) : parent(parent), idx(idx) {}
			संक्रियकः State() ध्रुव { प्रत्यागम (*parent)[idx]; }
			proxy& संक्रियकः=(State s) { parent->समुच्चयः(idx, s); प्रत्यागम *this; }
			proxy& संक्रियकः=(ध्रुव proxy& other) { parent->समुच्चयः(idx, (*other.parent)[other.idx]); प्रत्यागम *this; }
		};

		using iterator_category = std::bidirectional_iterator_चिह्नकम्;
		using मानम्_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = proxy*;
		using reference = proxy;

		iterator(Const& c, आकार_t i) : parent(&c), idx(i) {}

		proxy संक्रियकः*() ध्रुव { प्रत्यागम proxy(parent, idx); }
		iterator& संक्रियकः++() { ++idx; प्रत्यागम *this; }
		iterator& संक्रियकः--() { --idx; प्रत्यागम *this; }
		iterator संक्रियकः++(पूर्णाङ्क) { iterator परिणाम(*this); ++idx; प्रत्यागम परिणाम; }
		iterator संक्रियकः--(पूर्णाङ्क) { iterator परिणाम(*this); --idx; प्रत्यागम परिणाम; }
		iterator& संक्रियकः+=(पूर्णाङ्क i) { idx += i; प्रत्यागम *this; }

		iterator संक्रियकः+(पूर्णाङ्क सङ्कलय) {
			प्रत्यागम iterator(*parent, idx + सङ्कलय);
		}
		iterator संक्रियकः-(पूर्णाङ्क sub) {
			प्रत्यागम iterator(*parent, idx - sub);
		}
		पूर्णाङ्क संक्रियकः-(ध्रुव iterator& other) {
			प्रत्यागम idx - other.idx;
		}

		bool संक्रियकः==(ध्रुव iterator& other) ध्रुव {
			प्रत्यागम idx == other.idx;
		}

		bool संक्रियकः!=(ध्रुव iterator& other) ध्रुव {
			प्रत्यागम !(*this == other);
		}
	};

	const_iterator begin() ध्रुव {
		प्रत्यागम const_iterator(*this, 0);
	}
	const_iterator अन्तः() ध्रुव {
		प्रत्यागम const_iterator(*this, आकार());
	}
	iterator begin() {
		प्रत्यागम iterator(*this, 0);
	}
	iterator अन्तः() {
		प्रत्यागम iterator(*this, आकार());
	}
	State पृष्ठभागः() ध्रुव {
		प्रत्यागम *(अन्तः() - 1);
	}
	State front() ध्रुव {
		प्रत्यागम *begin();
	}
	State at(आकार_t i) ध्रुव {
		प्रत्यागम *const_iterator(*this, i);
	}
	State संक्रियकः[](आकार_t i) ध्रुव {
		प्रत्यागम *const_iterator(*this, i);
	}

	bool is_fully_zero() ध्रुव;
	bool is_fully_ones() ध्रुव;
	bool is_fully_def() ध्रुव;
	bool is_fully_undef() ध्रुव;
	bool is_fully_undef_x_only() ध्रुव;
	bool is_onehot(पूर्णाङ्क *pos = nullptr) ध्रुव;

	RTLIL::Const extract(पूर्णाङ्क विस्थापनम्, पूर्णाङ्क len = 1, RTLIL::State padding = RTLIL::State::S0) ध्रुव;

	// find the MSB without redundant leading bits
	पूर्णाङ्क get_min_आकार(bool is_signed) ध्रुव;

	// compress representation to the minimum required bits
	शून्य compress(bool is_signed = false);

	std::optional<पूर्णाङ्क> as_int_compress(bool is_signed) ध्रुव;

	शून्य extu(पूर्णाङ्क विस्तारः) {
		resize(विस्तारः, RTLIL::State::S0);
	}

	शून्य exts(पूर्णाङ्क विस्तारः) {
		resize(विस्तारः, रिक्तानि() ? RTLIL::State::Sx : पृष्ठभागः());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;
};

संरचना RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> गुणाः;

	bool has_attribute(RTLIL::IdString id) ध्रुव;

	शून्य समुच्चयः_bool_attribute(RTLIL::IdString id, bool मानम्=true);
	bool get_bool_attribute(RTLIL::IdString id) ध्रुव;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ध्रुव {
		प्रत्यागम get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	शून्य समुच्चयः_string_attribute(RTLIL::IdString id, string मानम्);
	string get_string_attribute(RTLIL::IdString id) ध्रुव;

	शून्य समुच्चयः_strpool_attribute(RTLIL::IdString id, ध्रुव pool<string> &दत्त);
	शून्य सङ्कलय_strpool_attribute(RTLIL::IdString id, ध्रुव pool<string> &दत्त);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ध्रुव;

	शून्य समुच्चयः_src_attribute(ध्रुव std::string &src) {
		समुच्चयः_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ध्रुव {
		प्रत्यागम get_string_attribute(ID::src);
	}

	शून्य समुच्चयः_hdlname_attribute(ध्रुव सदिशम्<string> &hierarchy);
	सदिशम्<string> get_hdlname_attribute() ध्रुव;

	शून्य समुच्चयः_intvec_attribute(RTLIL::IdString id, ध्रुव सदिशम्<पूर्णाङ्क> &दत्त);
	सदिशम्<पूर्णाङ्क> get_intvec_attribute(RTLIL::IdString id) ध्रुव;
};

#अन्तर्भाव "kernel/rtlil_twine_compat.h"

संरचना RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

संरचना RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::सदिशम्<RTLIL::State> दत्त; // only used if wire == NULL, LSB at index 0
	पूर्णाङ्क विस्तारः, विस्थापनम्;

	SigChunk() : wire(nullptr), विस्तारः(0), विस्थापनम्(0) {}
	SigChunk(ध्रुव RTLIL::Const &मानम्) : wire(nullptr), दत्त(मानम्.गन्तव्यम्_द्वयङ्काः()), विस्तारः(Get_आकार(दत्त)), विस्थापनम्(0) {}
	SigChunk(RTLIL::Const &&मानम्) : wire(nullptr), दत्त(मानम्.गन्तव्यम्_द्वयङ्काः()), विस्तारः(Get_आकार(दत्त)), विस्थापनम्(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), विस्तारः(Get_आकार(wire)), विस्थापनम्(0) {}
	SigChunk(RTLIL::Wire *wire, पूर्णाङ्क विस्थापनम्, पूर्णाङ्क विस्तारः = 1) : wire(wire), विस्तारः(विस्तारः), विस्थापनम्(विस्थापनम्) {}
	SigChunk(ध्रुव std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(पूर्णाङ्क val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(पूर्णाङ्क val, पूर्णाङ्क विस्तारः) : SigChunk(RTLIL::Const(val, विस्तारः)) {}
	SigChunk(RTLIL::State bit, पूर्णाङ्क विस्तारः = 1) : SigChunk(RTLIL::Const(bit, विस्तारः)) {}
	SigChunk(ध्रुव RTLIL::SigBit &bit);

	RTLIL::SigChunk extract(पूर्णाङ्क विस्थापनम्, पूर्णाङ्क दीर्घता_2) ध्रुव;
	RTLIL::SigBit संक्रियकः[](पूर्णाङ्क विस्थापनम्) ध्रुव;
	अन्तःस्थ पूर्णाङ्क आकार() ध्रुव { प्रत्यागम विस्तारः; }
	अन्तःस्थ bool is_wire() ध्रुव { प्रत्यागम wire != रिक्त; }

	bool संक्रियकः <(ध्रुव RTLIL::SigChunk &other) ध्रुव;
	bool संक्रियकः ==(ध्रुव RTLIL::SigChunk &other) ध्रुव;
	bool संक्रियकः !=(ध्रुव RTLIL::SigChunk &other) ध्रुव;
};

संरचना RTLIL::SigBit
{
	RTLIL::Wire *wire;
	सङ्घ {
		RTLIL::State दत्त; // used if wire == NULL
		पूर्णाङ्क विस्थापनम्;        // used if wire != NULL
	};

	SigBit();
	SigBit(RTLIL::State bit);
	explicit SigBit(bool bit);
	SigBit(RTLIL::Wire *wire);
	SigBit(RTLIL::Wire *wire, पूर्णाङ्क विस्थापनम्);
	SigBit(ध्रुव RTLIL::SigChunk &chunk);
	SigBit(ध्रुव RTLIL::SigChunk &chunk, पूर्णाङ्क सूचकांक);
	SigBit(ध्रुव RTLIL::SigSpec &sig);
	SigBit(ध्रुव RTLIL::SigBit &sigbit) = स्वभाव;
	RTLIL::SigBit &संक्रियकः =(ध्रुव RTLIL::SigBit &other) = स्वभाव;

	अन्तःस्थ bool is_wire() ध्रुव { प्रत्यागम wire != रिक्त; }

	bool संक्रियकः <(ध्रुव RTLIL::SigBit &other) ध्रुव;
	bool संक्रियकः ==(ध्रुव RTLIL::SigBit &other) ध्रुव;
	bool संक्रियकः !=(ध्रुव RTLIL::SigBit &other) ध्रुव;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;
	[[nodiscard]] Hasher hash_शिखरम्() ध्रुव;
};

namespace hashlib {
	प्रतिरूपम् <>
	संरचना hash_ops<RTLIL::SigBit> {
		स्थिर अन्तःस्थ bool cmp(ध्रुव RTLIL::SigBit &a, ध्रुव RTLIL::SigBit &b) {
			प्रत्यागम a == b;
		}
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(ध्रुव RTLIL::SigBit sb) {
			प्रत्यागम sb.hash_शिखरम्();
		}
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव RTLIL::SigBit sb, Hasher h) {
			प्रत्यागम sb.hash_into(h);
		}
	};
};

संरचना RTLIL::SigSpecIterator
{
	प्रकारनिर्देश std::निवेशः_iterator_चिह्नकम् iterator_category;
	प्रकारनिर्देश RTLIL::SigBit मानम्_type;
	प्रकारनिर्देश ptrdiff_t difference_type;
	प्रकारनिर्देश RTLIL::SigBit* pointer;
	प्रकारनिर्देश RTLIL::SigBit& reference;

	RTLIL::SigSpec *sig_p;
	पूर्णाङ्क सूचकांक;

	अन्तःस्थ RTLIL::SigBit &संक्रियकः*() ध्रुव;
	अन्तःस्थ bool संक्रियकः!=(ध्रुव RTLIL::SigSpecIterator &other) ध्रुव { प्रत्यागम सूचकांक != other.सूचकांक; }
	अन्तःस्थ bool संक्रियकः==(ध्रुव RTLIL::SigSpecIterator &other) ध्रुव { प्रत्यागम सूचकांक == other.सूचकांक; }
	अन्तःस्थ शून्य संक्रियकः++() { सूचकांक++; }
};

संरचना RTLIL::SigSpecConstIterator
{
	प्रकारनिर्देश std::निवेशः_iterator_चिह्नकम् iterator_category;
	प्रकारनिर्देश RTLIL::SigBit मानम्_type;
	प्रकारनिर्देश ptrdiff_t difference_type;
	प्रकारनिर्देश RTLIL::SigBit* pointer;
	प्रकारनिर्देश RTLIL::SigBit& reference;

	ध्रुव RTLIL::SigSpec *sig_p;
	RTLIL::SigBit bit;
	पूर्णाङ्क सूचकांक;

	अन्तःस्थ ध्रुव RTLIL::SigBit &संक्रियकः*();
	अन्तःस्थ bool संक्रियकः!=(ध्रुव RTLIL::SigSpecConstIterator &other) ध्रुव { प्रत्यागम सूचकांक != other.सूचकांक; }
	अन्तःस्थ bool संक्रियकः==(ध्रुव RTLIL::SigSpecIterator &other) ध्रुव { प्रत्यागम सूचकांक == other.सूचकांक; }
	अन्तःस्थ शून्य संक्रियकः++() { सूचकांक++; }
};

संरचना RTLIL::SigSpec
{
गुप्त:
	मित्रम् class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, Extract);
	गणना Representation : वर्ण {
		CHUNK,
		द्वयङ्काः_3,
	};
	// An AtomicHash is either clear or a nonzero integer.
	संरचना AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ध्रुव AtomicHash &rhs) : atomic_(rhs.आरोपय()) {}
		AtomicHash &संक्रियकः=(ध्रुव AtomicHash &rhs) { store(rhs.आरोपय()); प्रत्यागम *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> पठ() ध्रुव {
			Hasher::hash_t मानम् = आरोपय();
			यदि (मानम् == 0)
				प्रत्यागम std::nullopt;
			प्रत्यागम मानम्;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		शून्य समुच्चयः(Hasher::hash_t मानम्) ध्रुव {
			log_assert(मानम् != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(मानम्, std::स्मृतिः_क्रमः_relaxed);
			log_assert(old == 0 || old == मानम्);
		}
		शून्य clear() { store(0); }
	गुप्त:
		पूर्णाङ्क आरोपय() ध्रुव { प्रत्यागम atomic_.आरोपय(std::स्मृतिः_क्रमः_relaxed); }
		शून्य store(Hasher::hash_t मानम्) ध्रुव {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(मानम्, std::स्मृतिः_क्रमः_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	सङ्घ {
		RTLIL::SigChunk chunk_;
		std::सदिशम्<RTLIL::SigBit> द्वयङ्काः_2; // LSB at index 0
	};

	शून्य init_रिक्तानि_द्वयङ्काः() {
		rep_ = द्वयङ्काः_3;
		new (&द्वयङ्काः_2) std::सदिशम्<RTLIL::SigBit>;
	}

	शून्य unpack();
	अन्तःस्थ शून्य inline_unpack() {
		यदि (rep_ == CHUNK)
			unpack();
	}
	शून्य try_repack();

	Hasher::hash_t updhash() ध्रुव;
	शून्य destroy() {
		यदि (rep_ == CHUNK)
			chunk_.~SigChunk();
		अन्यथा
			द्वयङ्काः_2.~सदिशम्();
	}
	मित्रम् संरचना Chunks;

सार्वजनिक:
	SigSpec() { init_रिक्तानि_द्वयङ्काः(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ध्रुव SigSpec &मानम्) : rep_(मानम्.rep_), hash_(मानम्.hash_) {
		यदि (मानम्.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(मानम्.chunk_);
		अन्यथा
			new (&द्वयङ्काः_2) std::सदिशम्<RTLIL::SigBit>(मानम्.द्वयङ्काः_2);
	}
	SigSpec(SigSpec &&मानम्) : rep_(मानम्.rep_), hash_(मानम्.hash_) {
		यदि (मानम्.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(मानम्.chunk_));
		अन्यथा
			new (&द्वयङ्काः_2) std::सदिशम्<RTLIL::SigBit>(std::move(मानम्.द्वयङ्काः_2));
	}
	SigSpec(ध्रुव RTLIL::Const &मानम्);
	SigSpec(RTLIL::Const &&मानम्);
	SigSpec(ध्रुव RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, पूर्णाङ्क विस्थापनम्, पूर्णाङ्क विस्तारः = 1);
	SigSpec(ध्रुव std::string &str);
	SigSpec(पूर्णाङ्क val, पूर्णाङ्क विस्तारः = 32);
	SigSpec(RTLIL::State bit, पूर्णाङ्क विस्तारः = 1);
	SigSpec(ध्रुव RTLIL::SigBit &bit, पूर्णाङ्क विस्तारः = 1);
	SigSpec(ध्रुव std::सदिशम्<RTLIL::SigChunk> &chunks);
	SigSpec(ध्रुव std::सदिशम्<RTLIL::SigBit> &द्वयङ्काः);
	SigSpec(ध्रुव pool<RTLIL::SigBit> &द्वयङ्काः);
	SigSpec(ध्रुव std::समुच्चयः<RTLIL::SigBit> &द्वयङ्काः);
	explicit SigSpec(bool bit);
	~SigSpec() {
		destroy();
	}

	SigSpec &संक्रियकः=(ध्रुव SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		यदि (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		अन्यथा
			new (&द्वयङ्काः_2) std::सदिशम्<RTLIL::SigBit>(rhs.द्वयङ्काः_2);
		प्रत्यागम *this;
	}
	SigSpec &संक्रियकः=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		यदि (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		अन्यथा
			new (&द्वयङ्काः_2) std::सदिशम्<RTLIL::SigBit>(std::move(rhs.द्वयङ्काः_2));
		प्रत्यागम *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	संरचना Chunks {
		Chunks(ध्रुव SigSpec &spec) : spec(spec) {}
		संरचना const_iterator {
			using iterator_category = std::forward_iterator_चिह्नकम्;
			using मानम्_type = ध्रुव SigChunk &;
			using difference_type = std::ptrdiff_t;
			using pointer = ध्रुव SigChunk *;
			using reference = ध्रुव SigChunk &;

			ध्रुव SigSpec &spec;
			पूर्णाङ्क bit_सूचकांक;
			SigChunk chunk;

			const_iterator(ध्रुव SigSpec &spec) : spec(spec) {
				bit_सूचकांक = 0;
				यदि (spec.rep_ == द्वयङ्काः_3)
					अग्र_chunk_द्वयङ्काः();
			}
			गणना अन्तः_2 { अन्तः_3 };
			const_iterator(ध्रुव SigSpec &spec, अन्तः_2) : spec(spec) {
				bit_सूचकांक = spec.आकार();
			}
			शून्य अग्र_chunk_द्वयङ्काः();

			ध्रुव SigChunk &संक्रियकः*() {
				यदि (spec.rep_ == CHUNK)
					प्रत्यागम spec.chunk_;
				प्रत्यागम chunk;
			};
			ध्रुव SigChunk *संक्रियकः->() { प्रत्यागम &**this; }
			const_iterator &संक्रियकः++() {
				bit_सूचकांक += (**this).विस्तारः;
				यदि (spec.rep_ == द्वयङ्काः_3)
					अग्र_chunk_द्वयङ्काः();
				प्रत्यागम *this;
			}
			bool संक्रियकः==(ध्रुव const_iterator &rhs) ध्रुव { प्रत्यागम bit_सूचकांक == rhs.bit_सूचकांक; }
			bool संक्रियकः!=(ध्रुव const_iterator &rhs) ध्रुव { प्रत्यागम !(*this == rhs); }
		};
		const_iterator begin() ध्रुव { प्रत्यागम const_iterator(spec); }
		const_iterator अन्तः() ध्रुव {
			const_iterator it(spec, const_iterator::अन्तः_3);
			प्रत्यागम it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::सदिशम्<RTLIL::SigChunk>::const_reverse_iterator rbegin() {
			ensure_chunk_सदिशम्();
			प्रत्यागम chunk_सदिशम्.rbegin();
		}
		std::सदिशम्<RTLIL::SigChunk>::const_reverse_iterator rend() {
			ensure_chunk_सदिशम्();
			प्रत्यागम chunk_सदिशम्.rend();
		}
		पूर्णाङ्क आकार() {
			ensure_chunk_सदिशम्();
			प्रत्यागम chunk_सदिशम्.आकार();
		}
		पूर्णाङ्क आकार() ध्रुव {
			प्रत्यागम std::distance(begin(), अन्तः());
		}
		ध्रुव SigChunk &at(पूर्णाङ्क सूचकांक) {
			ensure_chunk_सदिशम्();
			प्रत्यागम chunk_सदिशम्.at(सूचकांक);
		}
		संक्रियकः ध्रुव std::सदिशम्<RTLIL::SigChunk>&() {
			ensure_chunk_सदिशम्();
			प्रत्यागम chunk_सदिशम्;
		}
	गुप्त:
		शून्य ensure_chunk_सदिशम्() {
			यदि (spec.आकार() > 0 && chunk_सदिशम्.रिक्तानि()) {
				प्रति (ध्रुव RTLIL::SigChunk &c : *this)
					chunk_सदिशम्.प्रक्षिप_पृष्ठभागः(c);
			}
		}
		ध्रुव SigSpec &spec;
		std::सदिशम्<RTLIL::SigChunk> chunk_सदिशम्;
	};
	मित्रम् संरचना Chunks::const_iterator;

	अन्तःस्थ Chunks chunks() ध्रुव { प्रत्यागम {*this}; }
	अन्तःस्थ ध्रुव SigSpec &द्वयङ्काः() ध्रुव { प्रत्यागम *this; }

	अन्तःस्थ पूर्णाङ्क आकार() ध्रुव { प्रत्यागम rep_ == CHUNK ? chunk_.विस्तारः : Get_आकार(द्वयङ्काः_2); }
	अन्तःस्थ bool रिक्तानि() ध्रुव { प्रत्यागम आकार() == 0; };

	अन्तःस्थ RTLIL::SigBit &संक्रियकः[](पूर्णाङ्क सूचकांक) { inline_unpack(); hash_.clear(); प्रत्यागम द्वयङ्काः_2.at(सूचकांक); }
	अन्तःस्थ RTLIL::SigBit संक्रियकः[](पूर्णाङ्क सूचकांक) ध्रुव {
		यदि (rep_ == CHUNK) {
			यदि (सूचकांक < 0 || सूचकांक >= chunk_.विस्तारः)
				throw std::बहिर्गमनम्_सम्बन्धि_range("SigSpec::operator[]");
			यदि (chunk_.wire)
				प्रत्यागम RTLIL::SigBit(chunk_.wire, chunk_.विस्थापनम् + सूचकांक);
			प्रत्यागम RTLIL::SigBit(chunk_.दत्त[सूचकांक]);
		}
		प्रत्यागम द्वयङ्काः_2.at(सूचकांक);
	}

	अन्तःस्थ RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.सूचकांक = 0; प्रत्यागम it; }
	अन्तःस्थ RTLIL::SigSpecIterator अन्तः() { RTLIL::SigSpecIterator it; it.sig_p = this; it.सूचकांक = आकार(); प्रत्यागम it; }

	अन्तःस्थ RTLIL::SigSpecConstIterator begin() ध्रुव { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.सूचकांक = 0; प्रत्यागम it; }
	अन्तःस्थ RTLIL::SigSpecConstIterator अन्तः() ध्रुव { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.सूचकांक = आकार(); प्रत्यागम it; }

	शून्य क्रमय();
	शून्य क्रमय_and_unify();

	शून्य replace(ध्रुव RTLIL::SigSpec &आदर्शः, ध्रुव RTLIL::SigSpec &with);
	शून्य replace(ध्रुव RTLIL::SigSpec &आदर्शः, ध्रुव RTLIL::SigSpec &with, RTLIL::SigSpec *other) ध्रुव;

	शून्य replace(ध्रुव dict<RTLIL::SigBit, RTLIL::SigBit> &नियमाः);
	शून्य replace(ध्रुव dict<RTLIL::SigBit, RTLIL::SigBit> &नियमाः, RTLIL::SigSpec *other) ध्रुव;

	शून्य replace(ध्रुव std::map<RTLIL::SigBit, RTLIL::SigBit> &नियमाः);
	शून्य replace(ध्रुव std::map<RTLIL::SigBit, RTLIL::SigBit> &नियमाः, RTLIL::SigSpec *other) ध्रुव;

	शून्य replace(पूर्णाङ्क विस्थापनम्, ध्रुव RTLIL::SigSpec &with);

	शून्य remove(ध्रुव RTLIL::SigSpec &आदर्शः);
	शून्य remove(ध्रुव RTLIL::SigSpec &आदर्शः, RTLIL::SigSpec *other) ध्रुव;
	शून्य remove2(ध्रुव RTLIL::SigSpec &आदर्शः, RTLIL::SigSpec *other);

	शून्य remove(ध्रुव pool<RTLIL::SigBit> &आदर्शः);
	शून्य remove(ध्रुव pool<RTLIL::SigBit> &आदर्शः, RTLIL::SigSpec *other) ध्रुव;
	शून्य remove2(ध्रुव pool<RTLIL::SigBit> &आदर्शः, RTLIL::SigSpec *other);
	शून्य remove2(ध्रुव std::समुच्चयः<RTLIL::SigBit> &आदर्शः, RTLIL::SigSpec *other);
	शून्य remove2(ध्रुव pool<RTLIL::Wire*> &आदर्शः, RTLIL::SigSpec *other);

	शून्य remove(पूर्णाङ्क विस्थापनम्, पूर्णाङ्क दीर्घता_2 = 1);
	शून्य remove_const();

	RTLIL::SigSpec extract(ध्रुव RTLIL::SigSpec &आदर्शः, ध्रुव RTLIL::SigSpec *other = रिक्त) ध्रुव;
	RTLIL::SigSpec extract(ध्रुव pool<RTLIL::SigBit> &आदर्शः, ध्रुव RTLIL::SigSpec *other = रिक्त) ध्रुव;
	RTLIL::SigSpec extract(पूर्णाङ्क विस्थापनम्, पूर्णाङ्क दीर्घता_2 = 1) ध्रुव;
	RTLIL::SigSpec extract_अन्तः(पूर्णाङ्क विस्थापनम्) ध्रुव { प्रत्यागम extract(विस्थापनम्, आकार() - विस्थापनम्); }

	शून्य rewrite_wires(std::function<शून्य(RTLIL::Wire*& wire)> rewrite);

	RTLIL::SigBit lsb() ध्रुव { log_assert(आकार()); प्रत्यागम (*this)[0]; };
	RTLIL::SigBit msb() ध्रुव { log_assert(आकार()); प्रत्यागम (*this)[आकार() - 1]; };
	RTLIL::SigBit front() ध्रुव { प्रत्यागम (*this)[0]; }
	RTLIL::SigBit पृष्ठभागः() ध्रुव { प्रत्यागम (*this)[आकार() - 1]; }

	शून्य append(ध्रुव RTLIL::SigSpec &सङ्केत);
	अन्तःस्थ शून्य append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	अन्तःस्थ शून्य append(ध्रुव RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	अन्तःस्थ शून्य append(ध्रुव RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	शून्य append(ध्रुव RTLIL::SigBit &bit);
	अन्तःस्थ शून्य append(RTLIL::State state) { append(RTLIL::SigBit(state)); }
	अन्तःस्थ शून्य append(bool bool_) { append(RTLIL::SigBit(bool_)); }

	शून्य extend_u0(पूर्णाङ्क विस्तारः, bool is_signed = false);

	RTLIL::SigSpec repeat(पूर्णाङ्क अङ्कः) ध्रुव;

	शून्य reverse() { inline_unpack(); std::reverse(द्वयङ्काः_2.begin(), द्वयङ्काः_2.अन्तः()); }

	bool संक्रियकः <(ध्रुव RTLIL::SigSpec &other) ध्रुव;
	bool संक्रियकः ==(ध्रुव RTLIL::SigSpec &other) ध्रुव;
	अन्तःस्थ bool संक्रियकः !=(ध्रुव RTLIL::SigSpec &other) ध्रुव { प्रत्यागम !(*this == other); }

	bool is_wire() ध्रुव;
	bool is_chunk() ध्रुव;
	अन्तःस्थ bool is_bit() ध्रुव { प्रत्यागम आकार() == 1; }

	bool ज्ञातम्_driver() ध्रुव;

	bool is_fully_const() ध्रुव;
	bool is_fully_zero() ध्रुव;
	bool is_fully_ones() ध्रुव;
	bool is_fully_def() ध्रुव;
	bool is_fully_undef() ध्रुव;
	bool has_const() ध्रुव;
	bool has_const(State state) ध्रुव;
	bool has_marked_द्वयङ्काः() ध्रुव;
	bool is_onehot(पूर्णाङ्क *pos = nullptr) ध्रुव;

	bool as_bool() ध्रुव;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	पूर्णाङ्क as_int(bool is_signed = false) ध्रुव;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_गन्तव्यम्_int(bool is_signed = false) ध्रुव;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<पूर्णाङ्क> try_as_int(bool is_signed = false) ध्रुव;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	पूर्णाङ्क as_int_saturating(bool is_signed = false) ध्रुव;

	std::string as_string() ध्रुव;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() ध्रुव;
	RTLIL::Const as_const() ध्रुव;
	RTLIL::Wire *as_wire() ध्रुव;
	RTLIL::SigChunk as_chunk() ध्रुव;
	RTLIL::SigBit as_bit() ध्रुव;

	bool मेलनम्(ध्रुव वर्ण* आदर्शः) ध्रुव;

	std::समुच्चयः<RTLIL::SigBit> गन्तव्यम्_sigbit_समुच्चयः() ध्रुव;
	pool<RTLIL::SigBit> गन्तव्यम्_sigbit_pool() ध्रुव;
	std::सदिशम्<RTLIL::SigBit> गन्तव्यम्_sigbit_सदिशम्() ध्रुव;
	std::map<RTLIL::SigBit, RTLIL::SigBit> गन्तव्यम्_sigbit_map(ध्रुव RTLIL::SigSpec &other) ध्रुव;
	dict<RTLIL::SigBit, RTLIL::SigBit> गन्तव्यम्_sigbit_dict(ध्रुव RTLIL::SigSpec &other) ध्रुव;

	स्थिर bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	स्थिर bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	स्थिर bool parse_rhs(ध्रुव RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	संक्रियकः std::सदिशम्<RTLIL::SigChunk>() ध्रुव;
	संक्रियकः std::सदिशम्<RTLIL::SigBit>() ध्रुव { प्रत्यागम गन्तव्यम्_sigbit_सदिशम्(); }
	ध्रुव RTLIL::SigBit &at(पूर्णाङ्क विस्थापनम्, ध्रुव RTLIL::SigBit &defval) { प्रत्यागम विस्थापनम् < आकार() ? (*this)[विस्थापनम्] : defval; }
	RTLIL::SigBit& at(पूर्णाङ्क विस्थापनम्) { प्रत्यागम (*this)[विस्थापनम्]; }
	RTLIL::SigBit at(पूर्णाङ्क विस्थापनम्) ध्रुव { प्रत्यागम (*this)[विस्थापनम्]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
		Hasher::hash_t val;
		यदि (std::optional<Hasher::hash_t> current = hash_.पठ())
			val = *current;
		अन्यथा
			val = updhash();
		h.eat(val);
		प्रत्यागम h;
	}

#यदि_अपरिभाषितम् NDEBUG
	शून्य check(ध्रुव Module *mod = nullptr) ध्रुव;
#अन्यथा
	शून्य check(ध्रुव Module *mod = nullptr) ध्रुव { (शून्य)mod; }
#यदि_अन्तः
};

संरचना RTLIL::Selection
{
	// selection includes boxed modules
	bool selects_boxes;
	// selection covers full design, including boxed modules
	bool complete_selection;
	// selection covers full design, not including boxed modules
	bool पूर्णम्_selection;
	pool<RTLIL::IdString> selected_modules;
	dict<RTLIL::IdString, pool<RTLIL::IdString>> selected_members;
	RTLIL::Design *current_design;

	// create a new selection
	Selection(
		// should the selection cover the full design
		bool पूर्णम्_2 = true,
		// should the selection include boxed modules
		bool boxes = false,
		// the design to select from
		RTLIL::Design *design = nullptr
	) :
		selects_boxes(boxes), complete_selection(पूर्णम्_2 && boxes), पूर्णम्_selection(पूर्णम्_2 && !boxes), current_design(design) { }

	// checks if the given module exists in the current design and is a
	// boxed module, warning the user if the current design is not set
	bool boxed_module(RTLIL::IdString mod_name) ध्रुव;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ध्रुव;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_सम्पूर्णम्_module(RTLIL::IdString mod_name) ध्रुव;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ध्रुव;

	// optimizes this selection for the given design by:
	// - removing non-existent modules and members, any boxed modules and
	//   their members (if selection does not include boxes), and any
	//   partially selected modules with no selected members;
	// - marking partially selected modules as wholly selected if all
	//   members of that module are selected; and
	// - marking selection as a complete_selection if all modules in the
	//   given design are selected, or a full_selection if it does not
	//   include boxes.
	शून्य optimize(RTLIL::Design *design);

	// checks if selection covers full design (may or may not include
	// boxed-modules)
	bool selects_all() ध्रुव {
		प्रत्यागम पूर्णम्_selection || complete_selection;
	}

	// add whole module to this selection
	प्रतिरूपम्<typename T1> शून्य select(T1 *module) {
		यदि (!selects_all() && selected_modules.गणनम्(module->name) == 0) {
			IdString name = module->name;
			selected_modules.निवेशय(name);
			selected_members.erase(name);
			यदि (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	प्रतिरूपम्<typename T1, typename T2> शून्य select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	प्रतिरूपम्<typename T1> शून्य select(T1 *module, RTLIL::IdString member) {
		यदि (!selects_all() && selected_modules.गणनम्(module->name.ref()) == 0) {
			selected_members[module->name.ref()].निवेशय(member);
			यदि (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool रिक्तानि() ध्रुव {
		प्रत्यागम !selects_all() && selected_modules.रिक्तानि() && selected_members.रिक्तानि();
	}

	// clear this selection, leaving it empty
	शून्य clear();

	// create a new selection which is empty
	स्थिर Selection रिक्तानि_Selection(RTLIL::Design *design = nullptr) { प्रत्यागम Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	स्थिर Selection पूर्णम्_Selection(RTLIL::Design *design = nullptr) { प्रत्यागम Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	स्थिर Selection CompleteSelection(RTLIL::Design *design = nullptr) { प्रत्यागम Selection(true, true, design); };
};

संरचना RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(hashidx_); प्रत्यागम h; }

	Monitor() {
		स्थिर अचिह्नित पूर्णाङ्क hashidx_गणनम् = 123456789;
		hashidx_गणनम् = mkhash_xorshift(hashidx_गणनम्);
		hashidx_ = hashidx_गणनम्;
	}

	आभास ~Monitor() { }
	आभास शून्य notify_module_सङ्कलय(RTLIL::Module*) { }
	आभास शून्य notify_module_del(RTLIL::Module*) { }
	आभास शून्य notify_connect(RTLIL::Cell*, RTLIL::IdString, ध्रुव RTLIL::SigSpec&, ध्रुव RTLIL::SigSpec&) { }
	आभास शून्य notify_connect(RTLIL::Module*, ध्रुव RTLIL::SigSig&) { }
	आभास शून्य notify_connect(RTLIL::Module*, ध्रुव std::सदिशम्<RTLIL::SigSig>&) { }
	आभास शून्य notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
संरचना define_map_t;

प्रतिरूपम्<typename N>
अन्तःस्थ constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::पर्णम्> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ध्रुव वर्ण*> || std::is_same_v<std::decay_t<N>, वर्ण*>;

#परिभाषय YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, पूर्णाङ्क> = 0

प्रतिरूपम्<typename N>
अन्तःस्थ constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ध्रुव वर्ण *> || std::is_same_v<std::decay_t<N>, वर्ण *>;

#परिभाषय YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, पूर्णाङ्क> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#परिभाषय YS_NAME_FWD_POOL(_func, _pool) \
	प्रतिरूपम्<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(स्वचालित) _func(N name, Rest&&... rest) \
		{ प्रत्यागम _func(_pool.सङ्कलय(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#परिभाषय YS_NAME_FWD_2ND_POOL(_func, _pool) \
	प्रतिरूपम्<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(स्वचालित) _func(T &&first, N name) \
		{ प्रत्यागम _func(std::forward<T>(first), _pool.सङ्कलय(std::move(name))); }

#परिभाषय YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#परिभाषय YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

संरचना RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(hashidx_); प्रत्यागम h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	शून्य bufNormalize(bool enable=true);

	पूर्णाङ्क refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

गुप्त:
	TwinePool twines_;

सार्वजनिक:
	TwinePool &twines() { प्रत्यागम twines_; }
	ध्रुव TwinePool &twines() ध्रुव { प्रत्यागम twines_; }

	शून्य absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	आकार_t gc_twines();

	std::सदिशम्<std::unique_ptr<AST::Ast_ग्रन्थिः>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_परिभाषते;

	std::सदिशम्<RTLIL::Selection> selection_राशिः;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_सक्रिय_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ध्रुव RTLIL::Module *module(RTLIL::IdString name) ध्रुव;
	प्रतिरूपम्<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ प्रत्यागम module(twines().find(std::string_view(name))); }
	प्रतिरूपम्<typename N, YS_NAME_STRING(N)> ध्रुव RTLIL::Module *module(N name) ध्रुव
		{ प्रत्यागम module(twines().find(std::string_view(name))); }
	RTLIL::Module *शिखरम्_module() ध्रुव;

	bool has(RTLIL::IdString id) ध्रुव {
		twines_.check_owned(id);
		प्रत्यागम modules_.गणनम्(id) != 0;
	}
	प्रतिरूपम्<typename N, YS_NAME_STRING(N)> bool has(N name) ध्रुव
		{ प्रत्यागम has(twines().find(std::string_view(name))); }

	शून्य सङ्कलय(RTLIL::Module *module);

	RTLIL::Module *सङ्कलय_Module(RTLIL::IdString name);
	YS_NAME_FWD_POOL(सङ्कलय_Module, twines())
	शून्य remove(RTLIL::Module *module);
	शून्य rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	शून्य scratchpad_unset(ध्रुव std::string &varname);

	शून्य scratchpad_समुच्चयः_int(ध्रुव std::string &varname, पूर्णाङ्क मानम्);
	शून्य scratchpad_समुच्चयः_bool(ध्रुव std::string &varname, bool मानम्);
	शून्य scratchpad_समुच्चयः_string(ध्रुव std::string &varname, std::string मानम्);

	पूर्णाङ्क scratchpad_get_int(ध्रुव std::string &varname, पूर्णाङ्क default_मानम्_2 = 0) ध्रुव;
	bool scratchpad_get_bool(ध्रुव std::string &varname, bool default_मानम्_2 = false) ध्रुव;
	std::string scratchpad_get_string(ध्रुव std::string &varname, ध्रुव std::string &default_मानम्_2 = std::string()) ध्रुव;

	शून्य क्रमय();
	शून्य क्रमय_modules();
	शून्य check();
	शून्य optimize();

	शून्य clone_into(RTLIL::Design *dst) ध्रुव;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ध्रुव;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_सम्पूर्णम्_module(RTLIL::IdString mod_name) ध्रुव;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ध्रुव;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ध्रुव;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_सम्पूर्णम्_module(RTLIL::Module *mod) ध्रुव;

	// push the given selection to the selection stack
	शून्य प्रक्षिप_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	शून्य प्रक्षिप_रिक्तानि_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	शून्य प्रक्षिप_पूर्णम्_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	शून्य प्रक्षिप_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	शून्य निष्कासय_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		प्रत्यागम selection_राशिः.पृष्ठभागः();
	}

	// get the current selection
	ध्रुव RTLIL::Selection &selection() ध्रुव {
		प्रत्यागम selection_राशिः.पृष्ठभागः();
	}

	// is the current selection a full selection (no boxes)
	bool पूर्णम्_selection() ध्रुव {
		प्रत्यागम selection().पूर्णम्_selection;
	}

	// is the given module in the current selection
	प्रतिरूपम्<typename T1> bool selected(T1 *module) ध्रुव {
		प्रत्यागम selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	प्रतिरूपम्<typename T1, typename T2> bool selected(T1 *module, T2 *member) ध्रुव {
		प्रत्यागम selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	प्रतिरूपम्<typename T1> शून्य select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	प्रतिरूपम्<typename T1, typename T2> शून्य select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::सदिशम्<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) ध्रुव;

	// returns all selected modules, and may include boxes
	std::सदिशम्<RTLIL::Module*> all_selected_modules() ध्रुव { प्रत्यागम selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::सदिशम्<RTLIL::Module*> selected_unboxed_modules() ध्रुव { प्रत्यागम selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::सदिशम्<RTLIL::Module*> selected_unboxed_modules_warn() ध्रुव { प्रत्यागम selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::सदिशम्<RTLIL::Module*> selected_सम्पूर्णम्_modules() ध्रुव { प्रत्यागम selected_modules(SELECT_सम्पूर्णम्_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::सदिशम्<RTLIL::Module*> all_selected_सम्पूर्णम्_modules() ध्रुव { प्रत्यागम selected_modules(SELECT_सम्पूर्णम्_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::सदिशम्<RTLIL::Module*> selected_सम्पूर्णम्_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ध्रुव { प्रत्यागम selected_modules(SELECT_सम्पूर्णम्_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::सदिशम्<RTLIL::Module*> selected_unboxed_सम्पूर्णम्_modules() ध्रुव { प्रत्यागम selected_modules(SELECT_सम्पूर्णम्_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::सदिशम्<RTLIL::Module*> selected_unboxed_सम्पूर्णम्_modules_warn() ध्रुव { प्रत्यागम selected_modules(SELECT_सम्पूर्णम्_WARN, SB_UNBOXED_WARN); }

	स्थिर std::map<अचिह्नित पूर्णाङ्क, RTLIL::Design*> *get_all_designs(शून्य);

	std::string गन्तव्यम्_rtlil_str(bool only_selected = true) ध्रुव;
};

संरचना RTLIL::Module : सार्वजनिक RTLIL::AttrObject
{
	मित्रम् संरचना RTLIL::Cell;
	मित्रम् संरचना RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(hashidx_); प्रत्यागम h; }

रक्षित:
	शून्य सङ्कलय(RTLIL::Wire *wire);
	शून्य सङ्कलय(RTLIL::Cell *cell);
	शून्य सङ्कलय(RTLIL::Process *process);

सार्वजनिक:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ध्रुव { प्रत्यागम design->twines(); }

	पूर्णाङ्क refcount_wires_;
	पूर्णाङ्क refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::सदिशम्<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_मानानि;
	dict<RTLIL::IdString, RTLIL::स्मृतिः*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	आभास ~Module();
	आभास RTLIL::IdString derive(RTLIL::Design *design, ध्रुव dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	आभास RTLIL::IdString derive(RTLIL::Design *design, ध्रुव dict<RTLIL::IdString, RTLIL::Const> &parameters, ध्रुव dict<RTLIL::IdString, RTLIL::Module*> &interfaces, ध्रुव dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	आभास आकार_t गणनम्_id(RTLIL::IdString id);
	आभास शून्य expand_interfaces(RTLIL::Design *design, ध्रुव dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	आभास bool reprocess_if_necessary(RTLIL::Design *design);

	आभास शून्य क्रमय();
	आभास शून्य check();
	आभास शून्य optimize();
	आभास शून्य makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ध्रुव {
		प्रत्यागम get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	शून्य connect(ध्रुव RTLIL::SigSig &conn);
	शून्य connect(ध्रुव RTLIL::SigSpec &lhs, ध्रुव RTLIL::SigSpec &rhs);
	शून्य new_connections(ध्रुव std::सदिशम्<RTLIL::SigSig> &new_conn);
	ध्रुव std::सदिशम्<RTLIL::SigSig> &connections() ध्रुव;

	std::सदिशम्<RTLIL::IdString> ports;
	शून्य fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_queue;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_पत्तनम्_queue;
	pool<RTLIL::Wire *> buf_norm_wire_queue;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_सूचकांक;
	शून्य bufNormalize();

	प्रतिरूपम्<typename T> शून्य rewrite_sigspecs(T &functor);
	प्रतिरूपम्<typename T> शून्य rewrite_sigspecs2(T &functor);
	शून्य cloneInto(RTLIL::Module *new_mod) ध्रुव;
	आभास RTLIL::Module *clone() ध्रुव;
	आभास RTLIL::Module *clone(RTLIL::Design *dst) ध्रुव;
	आभास RTLIL::Module *clone(RTLIL::Design *dst, IdString target_name) ध्रुव;

	bool has_memories() ध्रुव;
	bool has_processes() ध्रुव;

	bool has_memories_warn() ध्रुव;
	bool has_processes_warn() ध्रुव;

	bool is_selected() ध्रुव;
	bool is_selected_सम्पूर्णम्() ध्रुव;

	std::सदिशम्<RTLIL::Wire*> selected_wires() ध्रुव;
	std::सदिशम्<RTLIL::Cell*> selected_cells() ध्रुव;
	std::सदिशम्<RTLIL::स्मृतिः*> selected_memories() ध्रुव;
	std::सदिशम्<RTLIL::Process*> selected_processes() ध्रुव;
	std::सदिशम्<RTLIL::SelectedMember> selected_members() ध्रुव;

	प्रतिरूपम्<typename T> bool selected(T *member) ध्रुव {
		प्रत्यागम design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		यदि (design)
			design->twines().check_owned(id);
		स्वचालित it = wires_.find(id);
		प्रत्यागम it == wires_.अन्तः() ? nullptr : it->second;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		यदि (design)
			design->twines().check_owned(id);
		स्वचालित it = cells_.find(id);
		प्रत्यागम it == cells_.अन्तः() ? nullptr : it->second;
	}

	ध्रुव RTLIL::Wire* wire(RTLIL::IdString id) ध्रुव{
		यदि (design)
			design->twines().check_owned(id);
		स्वचालित it = wires_.find(id);
		प्रत्यागम it == wires_.अन्तः() ? nullptr : it->second;
	}
	ध्रुव RTLIL::Cell* cell(RTLIL::IdString id) ध्रुव {
		यदि (design)
			design->twines().check_owned(id);
		स्वचालित it = cells_.find(id);
		प्रत्यागम it == cells_.अन्तः() ? nullptr : it->second;
	}

	प्रतिरूपम्<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ प्रत्यागम wire(twines().find(std::string_view(name))); }
	प्रतिरूपम्<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ प्रत्यागम cell(twines().find(std::string_view(name))); }
	प्रतिरूपम्<typename N, YS_NAME_STRING(N)> ध्रुव RTLIL::Wire *wire(N name) ध्रुव
		{ प्रत्यागम wire(twines().find(std::string_view(name))); }
	प्रतिरूपम्<typename N, YS_NAME_STRING(N)> ध्रुव RTLIL::Cell *cell(N name) ध्रुव
		{ प्रत्यागम cell(twines().find(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { प्रत्यागम RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	पूर्णाङ्क wires_आकार() ध्रुव { प्रत्यागम wires_.आकार(); }
	RTLIL::Wire* wire_at(पूर्णाङ्क सूचकांक) ध्रुव { प्रत्यागम wires_.element(सूचकांक)->second; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { प्रत्यागम RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	पूर्णाङ्क cells_आकार() ध्रुव { प्रत्यागम cells_.आकार(); }
	RTLIL::Cell* cell_at(पूर्णाङ्क सूचकांक) ध्रुव { प्रत्यागम cells_.element(सूचकांक)->second; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	शून्य remove(ध्रुव pool<RTLIL::Wire*> &wires);
	शून्य remove(RTLIL::Cell *cell);
	शून्य remove(RTLIL::स्मृतिः *स्मृतिः_3);
	शून्य remove(RTLIL::Process *process);

	शून्य rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	शून्य rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	शून्य rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	शून्य swap_नामानि(RTLIL::Wire *w1, RTLIL::Wire *w2);
	शून्य swap_नामानि(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, पूर्णाङ्क &सूचकांक);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *सङ्कलय_Wire(RTLIL::IdString name, पूर्णाङ्क विस्तारः = 1);
	RTLIL::Wire *सङ्कलय_Wire(RTLIL::IdString name, ध्रुव RTLIL::Wire *other);
	YS_NAME_FWD(सङ्कलय_Wire)

	RTLIL::Cell *सङ्कलय_Cell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *सङ्कलय_Cell(RTLIL::IdString name, ध्रुव RTLIL::Cell *other);
	YS_NAME_FWD(सङ्कलय_Cell)
	प्रतिरूपम्<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *सङ्कलय_Cell(RTLIL::IdString name, T type) { प्रत्यागम सङ्कलय_Cell(name, design->twines().सङ्कलय(std::move(type))); }

	RTLIL::स्मृतिः *सङ्कलय_स्मृतिः(RTLIL::IdString name);
	YS_NAME_FWD(सङ्कलय_स्मृतिः)
	RTLIL::स्मृतिः *सङ्कलय_स्मृतिः(RTLIL::IdString name, ध्रुव RTLIL::स्मृतिः *other);

	RTLIL::Process *सङ्कलय_Process(RTLIL::IdString name);
	YS_NAME_FWD(सङ्कलय_Process)
	RTLIL::Process *सङ्कलय_Process(RTLIL::IdString name, ध्रुव RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* सङ्कलय_Not     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Pos     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Buf     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Barrier (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Neg     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_And  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Or   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Xor  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Xnor (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_Reduce_And  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Reduce_Or   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Reduce_Xor  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Reduce_Xnor (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Reduce_Bool (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_Shl    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Shr    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Sshl   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Sshr   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_सरणम्  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Shiftx (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_Lt  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Le  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Eq  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Ne  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Eqx (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Nex (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Ge  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Gt  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_सङ्कलय (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Sub (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Mul (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	// truncating division
	RTLIL::Cell* सङ्कलय_Div (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	// truncating modulo
	RTLIL::Cell* सङ्कलय_Mod (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Div_Floor (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Mod_Floor (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Pow (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_Fa (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव RTLIL::SigSpec &sig_x, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_Logic_Not (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Logic_And (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Logic_Or  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_Mux  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Pmux (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Bmux (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Demux (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_Bweqx  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Bwmux  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_खण्डिका  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, RTLIL::Const विस्थापनम्, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Concat (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Lut    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, RTLIL::Const lut, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Tribuf (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Assert (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Assume (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_जीवितम्   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Fair   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Cover  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Equiv  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_Sr    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr, ध्रुव RTLIL::SigSpec &sig_q, bool समुच्चयः_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Ff    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Dff   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_d,   ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Dffe  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en,  ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Dffsr (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool समुच्चयः_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Dffsre (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool समुच्चयः_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Adff (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, RTLIL::Const arst_मानम्, bool clk_polarity = true, bool arst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Adffe (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst,  ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, RTLIL::Const arst_मानम्, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Aldff (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Aldffe (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_aload,  ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Sdff (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, RTLIL::Const srst_मानम्, bool clk_polarity = true, bool srst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Sdffe (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst,  ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, RTLIL::Const srst_मानम्, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Sdffce (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, RTLIL::Const srst_मानम्, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Dlatch (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool en_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Adlatch (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, RTLIL::Const arst_मानम्, bool en_polarity = true, bool arst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Dlatchsr (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool en_polarity = true, bool समुच्चयः_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_Buf_Gate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Not_Gate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_And_Gate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Nand_Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Or_Gate     (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Nor_Gate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Xor_Gate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Xnor_Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Andnot_Gate (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Ornot_Gate  (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Mux_Gate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_s, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Nmux_Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_s, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Aoi3_Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_c, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Oai3_Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_c, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Aoi4_Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_c, ध्रुव RTLIL::SigBit &sig_d, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Oai4_Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_c, ध्रुव RTLIL::SigBit &sig_d, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_Sr_Gate     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr,
			ध्रुव RTLIL::SigSpec &sig_q, bool समुच्चयः_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Ff_Gate     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Dff_Gate    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Dffe_Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Dffsr_Gate  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool समुच्चयः_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Dffsre_Gate (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool समुच्चयः_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Adff_Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			bool arst_मानम् = false, bool clk_polarity = true, bool arst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Adffe_Gate  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			bool arst_मानम् = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Aldff_Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			ध्रुव RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Aldffe_Gate  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			ध्रुव RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Sdff_Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			bool srst_मानम् = false, bool clk_polarity = true, bool srst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Sdffe_Gate  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			bool srst_मानम् = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Sdffce_Gate (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			bool srst_मानम् = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Dlatch_Gate (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool en_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Adlatch_Gate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			bool arst_मानम् = false, bool en_polarity = true, bool arst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* सङ्कलय_Dlatchsr_Gate  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_समुच्चयः, ध्रुव RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool en_polarity = true, bool समुच्चयः_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");

	RTLIL::Cell* सङ्कलय_Anyinit(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec सरणम्_2  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::SigSpec सङ्कलय_2 (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, ध्रुव std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव std::string &src = "");

	RTLIL::SigBit BufGate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव std::string &src = "");
	RTLIL::SigBit NotGate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव std::string &src = "");
	RTLIL::SigBit AndGate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव std::string &src = "");
	RTLIL::SigBit NandGate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव std::string &src = "");
	RTLIL::SigBit OrGate     (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव std::string &src = "");
	RTLIL::SigBit NorGate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव std::string &src = "");
	RTLIL::SigBit XorGate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव std::string &src = "");
	RTLIL::SigBit XnorGate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव std::string &src = "");
	RTLIL::SigBit AndnotGate (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव std::string &src = "");
	RTLIL::SigBit OrnotGate  (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव std::string &src = "");
	RTLIL::SigBit MuxGate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_s, ध्रुव std::string &src = "");
	RTLIL::SigBit NmuxGate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_s, ध्रुव std::string &src = "");
	RTLIL::SigBit Aoi3Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_c, ध्रुव std::string &src = "");
	RTLIL::SigBit Oai3Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_c, ध्रुव std::string &src = "");
	RTLIL::SigBit Aoi4Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_c, ध्रुव RTLIL::SigBit &sig_d, ध्रुव std::string &src = "");
	RTLIL::SigBit Oai4Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_c, ध्रुव RTLIL::SigBit &sig_d, ध्रुव std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, पूर्णाङ्क विस्तारः = 1, ध्रुव std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, पूर्णाङ्क विस्तारः = 1, ध्रुव std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, पूर्णाङ्क विस्तारः = 1, ध्रुव std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, पूर्णाङ्क विस्तारः = 1, ध्रुव std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ध्रुव std::string &src = "");

	RTLIL::SigSpec समुच्चयः_चिह्नकम्_2          (RTLIL::IdString name, ध्रुव std::string &चिह्नकम्, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव std::string &src = "");
	RTLIL::Cell*   सङ्कलय_समुच्चयः_चिह्नकम्       (RTLIL::IdString name, ध्रुव std::string &चिह्नकम्, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::SigSpec Get_चिह्नकम्          (RTLIL::IdString name, ध्रुव std::string &चिह्नकम्, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव std::string &src = "");
	RTLIL::Cell*   सङ्कलय_Overwrite_चिह्नकम् (RTLIL::IdString name, ध्रुव std::string &चिह्नकम्, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव std::string &src = "");
	RTLIL::SigSpec Original_चिह्नकम्     (RTLIL::IdString name, ध्रुव std::string &चिह्नकम्, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_e, ध्रुव std::string &src = "");
	YS_NAME_FWD(सङ्कलय_Anyinit)
	YS_NAME_FWD(Anyconst) YS_NAME_FWD(Anyseq) YS_NAME_FWD(Allconst) YS_NAME_FWD(Allseq)
	YS_NAME_FWD(Initstate)
	YS_NAME_FWD(समुच्चयः_चिह्नकम्_2) YS_NAME_FWD(सङ्कलय_समुच्चयः_चिह्नकम्) YS_NAME_FWD(Get_चिह्नकम्)
	YS_NAME_FWD(सङ्कलय_Overwrite_चिह्नकम्) YS_NAME_FWD(Original_चिह्नकम्) YS_NAME_FWD(FutureFF)
	YS_NAME_FWD(सङ्कलय_Not) YS_NAME_FWD(सङ्कलय_Pos) YS_NAME_FWD(सङ्कलय_Buf) YS_NAME_FWD(सङ्कलय_Barrier) YS_NAME_FWD(सङ्कलय_Neg)
	YS_NAME_FWD(सङ्कलय_And) YS_NAME_FWD(सङ्कलय_Or) YS_NAME_FWD(सङ्कलय_Xor) YS_NAME_FWD(सङ्कलय_Xnor)
	YS_NAME_FWD(सङ्कलय_Reduce_And) YS_NAME_FWD(सङ्कलय_Reduce_Or) YS_NAME_FWD(सङ्कलय_Reduce_Xor) YS_NAME_FWD(सङ्कलय_Reduce_Xnor)
	YS_NAME_FWD(सङ्कलय_Reduce_Bool) YS_NAME_FWD(सङ्कलय_Shl) YS_NAME_FWD(सङ्कलय_Shr) YS_NAME_FWD(सङ्कलय_Sshl)
	YS_NAME_FWD(सङ्कलय_Sshr) YS_NAME_FWD(सङ्कलय_सरणम्) YS_NAME_FWD(सङ्कलय_Shiftx) YS_NAME_FWD(सङ्कलय_Lt)
	YS_NAME_FWD(सङ्कलय_Le) YS_NAME_FWD(सङ्कलय_Eq) YS_NAME_FWD(सङ्कलय_Ne) YS_NAME_FWD(सङ्कलय_Eqx)
	YS_NAME_FWD(सङ्कलय_Nex) YS_NAME_FWD(सङ्कलय_Ge) YS_NAME_FWD(सङ्कलय_Gt) YS_NAME_FWD(सङ्कलय_सङ्कलय)
	YS_NAME_FWD(सङ्कलय_Sub) YS_NAME_FWD(सङ्कलय_Mul) YS_NAME_FWD(सङ्कलय_Div) YS_NAME_FWD(सङ्कलय_Mod)
	YS_NAME_FWD(सङ्कलय_Div_Floor) YS_NAME_FWD(सङ्कलय_Mod_Floor) YS_NAME_FWD(सङ्कलय_Pow) YS_NAME_FWD(सङ्कलय_Fa)
	YS_NAME_FWD(सङ्कलय_Logic_Not) YS_NAME_FWD(सङ्कलय_Logic_And) YS_NAME_FWD(सङ्कलय_Logic_Or) YS_NAME_FWD(सङ्कलय_Mux)
	YS_NAME_FWD(सङ्कलय_Pmux) YS_NAME_FWD(सङ्कलय_Bmux) YS_NAME_FWD(सङ्कलय_Demux) YS_NAME_FWD(सङ्कलय_Bweqx)
	YS_NAME_FWD(सङ्कलय_Bwmux) YS_NAME_FWD(सङ्कलय_खण्डिका) YS_NAME_FWD(सङ्कलय_Concat) YS_NAME_FWD(सङ्कलय_Lut)
	YS_NAME_FWD(सङ्कलय_Tribuf) YS_NAME_FWD(सङ्कलय_Assert) YS_NAME_FWD(सङ्कलय_Assume) YS_NAME_FWD(सङ्कलय_जीवितम्)
	YS_NAME_FWD(सङ्कलय_Fair) YS_NAME_FWD(सङ्कलय_Cover) YS_NAME_FWD(सङ्कलय_Equiv) YS_NAME_FWD(सङ्कलय_Sr)
	YS_NAME_FWD(सङ्कलय_Ff) YS_NAME_FWD(सङ्कलय_Dff) YS_NAME_FWD(सङ्कलय_Dffe) YS_NAME_FWD(सङ्कलय_Dffsr)
	YS_NAME_FWD(सङ्कलय_Dffsre) YS_NAME_FWD(सङ्कलय_Adff) YS_NAME_FWD(सङ्कलय_Adffe) YS_NAME_FWD(सङ्कलय_Aldff)
	YS_NAME_FWD(सङ्कलय_Aldffe) YS_NAME_FWD(सङ्कलय_Sdff) YS_NAME_FWD(सङ्कलय_Sdffe) YS_NAME_FWD(सङ्कलय_Sdffce)
	YS_NAME_FWD(सङ्कलय_Dlatch) YS_NAME_FWD(सङ्कलय_Adlatch) YS_NAME_FWD(सङ्कलय_Dlatchsr) YS_NAME_FWD(सङ्कलय_Buf_Gate)
	YS_NAME_FWD(सङ्कलय_Not_Gate) YS_NAME_FWD(सङ्कलय_And_Gate) YS_NAME_FWD(सङ्कलय_Nand_Gate) YS_NAME_FWD(सङ्कलय_Or_Gate)
	YS_NAME_FWD(सङ्कलय_Nor_Gate) YS_NAME_FWD(सङ्कलय_Xor_Gate) YS_NAME_FWD(सङ्कलय_Xnor_Gate) YS_NAME_FWD(सङ्कलय_Andnot_Gate)
	YS_NAME_FWD(सङ्कलय_Ornot_Gate) YS_NAME_FWD(सङ्कलय_Mux_Gate) YS_NAME_FWD(सङ्कलय_Nmux_Gate) YS_NAME_FWD(सङ्कलय_Aoi3_Gate)
	YS_NAME_FWD(सङ्कलय_Oai3_Gate) YS_NAME_FWD(सङ्कलय_Aoi4_Gate) YS_NAME_FWD(सङ्कलय_Oai4_Gate) YS_NAME_FWD(सङ्कलय_Sr_Gate)
	YS_NAME_FWD(सङ्कलय_Ff_Gate) YS_NAME_FWD(सङ्कलय_Dff_Gate) YS_NAME_FWD(सङ्कलय_Dffe_Gate) YS_NAME_FWD(सङ्कलय_Dffsr_Gate)
	YS_NAME_FWD(सङ्कलय_Dffsre_Gate) YS_NAME_FWD(सङ्कलय_Adff_Gate) YS_NAME_FWD(सङ्कलय_Adffe_Gate) YS_NAME_FWD(सङ्कलय_Aldff_Gate)
	YS_NAME_FWD(सङ्कलय_Aldffe_Gate) YS_NAME_FWD(सङ्कलय_Sdff_Gate) YS_NAME_FWD(सङ्कलय_Sdffe_Gate) YS_NAME_FWD(सङ्कलय_Sdffce_Gate)
	YS_NAME_FWD(सङ्कलय_Dlatch_Gate) YS_NAME_FWD(सङ्कलय_Adlatch_Gate) YS_NAME_FWD(सङ्कलय_Dlatchsr_Gate) YS_NAME_FWD(Not)
	YS_NAME_FWD(Pos) YS_NAME_FWD(Buf) YS_NAME_FWD(Barrier) YS_NAME_FWD(Neg) YS_NAME_FWD(And)
	YS_NAME_FWD(Or) YS_NAME_FWD(Xor) YS_NAME_FWD(Xnor) YS_NAME_FWD(ReduceAnd)
	YS_NAME_FWD(ReduceOr) YS_NAME_FWD(ReduceXor) YS_NAME_FWD(ReduceXnor) YS_NAME_FWD(ReduceBool)
	YS_NAME_FWD(Shl) YS_NAME_FWD(Shr) YS_NAME_FWD(Sshl) YS_NAME_FWD(Sshr)
	YS_NAME_FWD(सरणम्_2) YS_NAME_FWD(Shiftx) YS_NAME_FWD(Lt) YS_NAME_FWD(Le)
	YS_NAME_FWD(Eq) YS_NAME_FWD(Ne) YS_NAME_FWD(Eqx) YS_NAME_FWD(Nex)
	YS_NAME_FWD(Ge) YS_NAME_FWD(Gt) YS_NAME_FWD(सङ्कलय_2) YS_NAME_FWD(Sub)
	YS_NAME_FWD(Mul) YS_NAME_FWD(Div) YS_NAME_FWD(Mod) YS_NAME_FWD(DivFloor)
	YS_NAME_FWD(ModFloor) YS_NAME_FWD(Pow) YS_NAME_FWD(LogicNot) YS_NAME_FWD(LogicAnd)
	YS_NAME_FWD(LogicOr) YS_NAME_FWD(Mux) YS_NAME_FWD(Pmux) YS_NAME_FWD(Bmux)
	YS_NAME_FWD(Demux) YS_NAME_FWD(Bweqx) YS_NAME_FWD(Bwmux) YS_NAME_FWD(BufGate)
	YS_NAME_FWD(NotGate) YS_NAME_FWD(AndGate) YS_NAME_FWD(NandGate) YS_NAME_FWD(OrGate)
	YS_NAME_FWD(NorGate) YS_NAME_FWD(XorGate) YS_NAME_FWD(XnorGate) YS_NAME_FWD(AndnotGate)
	YS_NAME_FWD(OrnotGate) YS_NAME_FWD(MuxGate) YS_NAME_FWD(NmuxGate) YS_NAME_FWD(Aoi3Gate)
	YS_NAME_FWD(Oai3Gate) YS_NAME_FWD(Aoi4Gate) YS_NAME_FWD(Oai4Gate)

	std::string गन्तव्यम्_rtlil_str() ध्रुव;
#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	स्थिर std::map<अचिह्नित पूर्णाङ्क, RTLIL::Module*> *get_all_modules(शून्य);
#यदि_अन्तः
};

संरचना RTLIL::Wire : सार्वजनिक RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(hashidx_); प्रत्यागम h; }

रक्षित:
	// use module->addWire() and module->remove() to create or destroy wires
	मित्रम् संरचना RTLIL::Module;
	Wire();
	~Wire();

	मित्रम् संरचना RTLIL::Design;
	मित्रम् संरचना RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driver_पत्तनम्_2;

सार्वजनिक:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	शून्य संक्रियकः=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	पूर्णाङ्क विस्तारः, start_विस्थापनम्, पत्तनम्_id;
	bool पत्तनम्_निवेशः, पत्तनम्_output, upto, is_signed;

	RTLIL::Design *design() ध्रुव { प्रत्यागम module ? module->design : nullptr; }
	TwinePool &twines() ध्रुव { प्रत्यागम module->twines(); }

	bool ज्ञातम्_driver() ध्रुव { प्रत्यागम driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ध्रुव    { log_assert(driverCell_); प्रत्यागम driverCell_; };
	RTLIL::IdString driver_पत्तनम्() ध्रुव { log_assert(driverCell_); प्रत्यागम driver_पत्तनम्_2; };

	पूर्णाङ्क from_hdl_सूचकांक(पूर्णाङ्क hdl_सूचकांक) {
		पूर्णाङ्क zero_सूचकांक = hdl_सूचकांक - start_विस्थापनम्;
		पूर्णाङ्क rtlil_सूचकांक = upto ? विस्तारः - 1 - zero_सूचकांक : zero_सूचकांक;
		प्रत्यागम rtlil_सूचकांक >= 0 && rtlil_सूचकांक < विस्तारः ? rtlil_सूचकांक : INT_MIN;
	}

	पूर्णाङ्क गन्तव्यम्_hdl_सूचकांक(पूर्णाङ्क rtlil_सूचकांक) {
		यदि (rtlil_सूचकांक < 0 || rtlil_सूचकांक >= विस्तारः)
			प्रत्यागम INT_MIN;
		पूर्णाङ्क zero_सूचकांक = upto ? विस्तारः - 1 - rtlil_सूचकांक : rtlil_सूचकांक;
		प्रत्यागम zero_सूचकांक + start_विस्थापनम्;
	}

	std::string गन्तव्यम्_rtlil_str() ध्रुव;

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	स्थिर std::map<अचिह्नित पूर्णाङ्क, RTLIL::Wire*> *get_all_wires(शून्य);
#यदि_अन्तः
};

अन्तःस्थ पूर्णाङ्क Get_आकार(RTLIL::Wire *wire) {
	प्रत्यागम wire->विस्तारः;
}

संरचना RTLIL::स्मृतिः : सार्वजनिक RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(hashidx_); प्रत्यागम h; }

	स्मृतिः();
	~स्मृतिः();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() ध्रुव { प्रत्यागम module ? module->design : nullptr; }
	TwinePool &twines() ध्रुव { प्रत्यागम module->twines(); }

	RTLIL::स्मृतिः_Name_Id name;

	पूर्णाङ्क विस्तारः, start_विस्थापनम्, आकार;
#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	स्थिर std::map<अचिह्नित पूर्णाङ्क, RTLIL::स्मृतिः*> *get_all_memorys(शून्य);
#यदि_अन्तः

	std::string गन्तव्यम्_rtlil_str() ध्रुव;
};

संरचना RTLIL::Cell : सार्वजनिक RTLIL::AttrObject
{
गुप्त:
	शून्य init_सूचकांक();

	bool bufnorm_handle_समुच्चयः_पत्तनम्(IdString portname, RTLIL::SigSpec &सङ्केत, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
सार्वजनिक:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(hashidx_); प्रत्यागम h; }

रक्षित:
	// use module->addCell() and module->remove() to create or destroy cells
	मित्रम् संरचना RTLIL::Module;
	Cell();
	~Cell();

सार्वजनिक:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	शून्य संक्रियकः=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() ध्रुव { प्रत्यागम module ? module->design : nullptr; }
	TwinePool &twines() ध्रुव { प्रत्यागम module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool has_पत्तनम्(RTLIL::IdString portname) ध्रुव;
	शून्य unset_पत्तनम्(RTLIL::IdString portname);
	शून्य समुच्चयः_पत्तनम्(RTLIL::IdString portname, RTLIL::SigSpec सङ्केत);
	प्रतिरूपम्<typename N, YS_NAME_STRING(N)> शून्य समुच्चयः_पत्तनम्(N portname, RTLIL::SigSpec सङ्केत)
		{ समुच्चयः_पत्तनम्(module->twines().सङ्कलय(std::move(portname)), std::move(सङ्केत)); }
	ध्रुव RTLIL::SigSpec &get_पत्तनम्(RTLIL::IdString portname) ध्रुव;
	ध्रुव dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ध्रुव;

	bool ज्ञातम्() ध्रुव;
	bool निवेशः(RTLIL::IdString portname) ध्रुव;
	bool output(RTLIL::IdString portname) ध्रुव;
	पत्तनम्_Dir पत्तनम्_dir(RTLIL::IdString portname) ध्रुव;

	bool hasParam(RTLIL::IdString paramname) ध्रुव;
	शून्य unsetParam(RTLIL::IdString paramname);
	शून्य समुच्चयः_Param(RTLIL::IdString paramname, RTLIL::Const मानम्);
	ध्रुव RTLIL::Const &getParam(RTLIL::IdString paramname) ध्रुव;

	प्रतिरूपम्<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ध्रुव
		{ प्रत्यागम hasParam(module->twines().find(std::string_view(name))); }
	प्रतिरूपम्<typename N, YS_NAME_STRING(N)> शून्य unsetParam(N name)
		{ unsetParam(module->twines().find(std::string_view(name))); }
	प्रतिरूपम्<typename N, YS_NAME_STRING(N)> शून्य समुच्चयः_Param(N name, RTLIL::Const मानम्)
		{ समुच्चयः_Param(module->twines().सङ्कलय(std::move(name)), std::move(मानम्)); }
	प्रतिरूपम्<typename N, YS_NAME_STRING(N)> ध्रुव RTLIL::Const &getParam(N name) ध्रुव
		{ प्रत्यागम getParam(module->twines().find(std::string_view(name))); }

	शून्य क्रमय();
	शून्य check();
	शून्य fixup_parameters(bool समुच्चयः_a_signed = false, bool समुच्चयः_b_signed = false);

	bool has_keep_attr() ध्रुव;

	प्रतिरूपम्<typename T> शून्य rewrite_sigspecs(T &functor);
	प्रतिरूपम्<typename T> शून्य rewrite_sigspecs2(T &functor);

#यदि_परिभाषितम् YOSYS_ENABLE_PYTHON
	स्थिर std::map<अचिह्नित पूर्णाङ्क, RTLIL::Cell*> *get_all_cells(शून्य);
#यदि_अन्तः

	bool has_memid() ध्रुव;
	bool is_mem_cell() ध्रुव;
	bool is_builtin_ff() ध्रुव;

	std::string गन्तव्यम्_rtlil_str() ध्रुव;
};

संरचना RTLIL::Case_नियमः : सार्वजनिक RTLIL::AttrObject
{
	std::सदिशम्<RTLIL::SigSpec> compare;
	std::सदिशम्<RTLIL::SigSig> actions;
	std::सदिशम्<RTLIL::Switch_नियमः*> switches;

	~Case_नियमः();

	bool रिक्तानि() ध्रुव;

	प्रतिरूपम्<typename T> शून्य rewrite_sigspecs(T &functor);
	प्रतिरूपम्<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::Case_नियमः *clone() ध्रुव;
};

संरचना RTLIL::Switch_नियमः : सार्वजनिक RTLIL::AttrObject
{
	RTLIL::SigSpec सङ्केत;
	std::सदिशम्<RTLIL::Case_नियमः*> cases;

	~Switch_नियमः();

	bool रिक्तानि() ध्रुव;

	प्रतिरूपम्<typename T> शून्य rewrite_sigspecs(T &functor);
	प्रतिरूपम्<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::Switch_नियमः *clone() ध्रुव;
};

संरचना RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec स्थानाङ्कः;
	RTLIL::SigSpec दत्त;
	RTLIL::SigSpec enable;
	RTLIL::Const प्राथमिकता_आवरणम्;
};

संरचना RTLIL::Sync_नियमः
{
	RTLIL::SyncType type;
	RTLIL::SigSpec सङ्केत;
	std::सदिशम्<RTLIL::SigSig> actions;
	std::सदिशम्<RTLIL::MemWriteAction> mem_write_actions;

	प्रतिरूपम्<typename T> शून्य rewrite_sigspecs(T &functor);
	प्रतिरूपम्<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::Sync_नियमः *clone() ध्रुव;
};

संरचना RTLIL::Process : सार्वजनिक RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(hashidx_); प्रत्यागम h; }

रक्षित:
	// use module->addProcess() and module->remove() to create or destroy processes
	मित्रम् संरचना RTLIL::Module;
	Process();
	~Process();

सार्वजनिक:
	RTLIL::Module *module;
	RTLIL::Case_नियमः मूलम्_case;
	std::सदिशम्<RTLIL::Sync_नियमः*> syncs;

	RTLIL::Design *design() ध्रुव { प्रत्यागम module ? module->design : nullptr; }
	TwinePool &twines() ध्रुव { प्रत्यागम module->twines(); }

	प्रतिरूपम्<typename T> शून्य rewrite_sigspecs(T &functor);
	प्रतिरूपम्<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ध्रुव;

	std::string गन्तव्यम्_rtlil_str() ध्रुव;
};

अन्तःस्थ RTLIL::SigBit::SigBit() : wire(रिक्त), दत्त(RTLIL::State::S0) { }
अन्तःस्थ RTLIL::SigBit::SigBit(RTLIL::State bit) : wire(रिक्त), दत्त(bit) { }
अन्तःस्थ RTLIL::SigBit::SigBit(bool bit) : wire(रिक्त), दत्त(bit ? State::S1 : State::S0) { }
अन्तःस्थ RTLIL::SigBit::SigBit(RTLIL::Wire *wire) : wire(wire), विस्थापनम्(0) { log_assert(wire && wire->विस्तारः == 1); }
अन्तःस्थ RTLIL::SigBit::SigBit(RTLIL::Wire *wire, पूर्णाङ्क विस्थापनम्) : wire(wire), विस्थापनम्(विस्थापनम्) { log_assert(wire != nullptr); }
अन्तःस्थ RTLIL::SigBit::SigBit(ध्रुव RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.विस्तारः == 1); यदि (wire) विस्थापनम् = chunk.विस्थापनम्; अन्यथा दत्त = chunk.दत्त[0]; }
अन्तःस्थ RTLIL::SigBit::SigBit(ध्रुव RTLIL::SigChunk &chunk, पूर्णाङ्क सूचकांक) : wire(chunk.wire) { यदि (wire) विस्थापनम् = chunk.विस्थापनम् + सूचकांक; अन्यथा दत्त = chunk.दत्त[सूचकांक]; }

अन्तःस्थ bool RTLIL::SigBit::संक्रियकः<(ध्रुव RTLIL::SigBit &other) ध्रुव {
	यदि (wire == other.wire)
		प्रत्यागम wire ? (विस्थापनम् < other.विस्थापनम्) : (दत्त < other.दत्त);
	यदि (wire != nullptr && other.wire != nullptr)
		प्रत्यागम wire->name < other.wire->name;
	प्रत्यागम (wire != nullptr) < (other.wire != nullptr);
}

अन्तःस्थ bool RTLIL::SigBit::संक्रियकः==(ध्रुव RTLIL::SigBit &other) ध्रुव {
	प्रत्यागम (wire == other.wire) && (wire ? (विस्थापनम् == other.विस्थापनम्) : (दत्त == other.दत्त));
}

अन्तःस्थ bool RTLIL::SigBit::संक्रियकः!=(ध्रुव RTLIL::SigBit &other) ध्रुव {
	प्रत्यागम (wire != other.wire) || (wire ? (विस्थापनम् != other.विस्थापनम्) : (दत्त != other.दत्त));
}

अन्तःस्थ Hasher RTLIL::SigBit::hash_into(Hasher h) ध्रुव {
	यदि (wire) {
		h.eat(विस्थापनम्);
		h.eat(wire->name);
		प्रत्यागम h;
	}
	h.eat(दत्त);
	प्रत्यागम h;
}


अन्तःस्थ Hasher RTLIL::SigBit::hash_शिखरम्() ध्रुव {
	Hasher h;
	यदि (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.द्वयङ्काः();
		h.force(hashlib::legacy::djb2_सङ्कलय(n, विस्थापनम्));
		प्रत्यागम h;
	}
	h.force(दत्त);
	प्रत्यागम h;
}

अन्तःस्थ RTLIL::SigBit &RTLIL::SigSpecIterator::संक्रियकः*() ध्रुव {
	प्रत्यागम (*sig_p)[सूचकांक];
}

अन्तःस्थ ध्रुव RTLIL::SigBit &RTLIL::SigSpecConstIterator::संक्रियकः*() {
	bit = (*sig_p)[सूचकांक];
	प्रत्यागम bit;
}

अन्तःस्थ RTLIL::SigBit::SigBit(ध्रुव RTLIL::SigSpec &sig) {
	log_assert(sig.आकार() == 1);
	स्वचालित it = sig.chunks().begin();
	*this = SigBit(*it);
}

प्रतिरूपम्<typename T>
शून्य RTLIL::Module::rewrite_sigspecs(T &functor)
{
	प्रति (स्वचालित &it : cells_)
		it.second->rewrite_sigspecs(functor);
	प्रति (स्वचालित &it : processes)
		it.second->rewrite_sigspecs(functor);
	प्रति (स्वचालित &it : connections_) {
		functor(it.first);
		functor(it.second);
	}
}

प्रतिरूपम्<typename T>
शून्य RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	प्रति (स्वचालित &it : cells_)
		it.second->rewrite_sigspecs2(functor);
	प्रति (स्वचालित &it : processes)
		it.second->rewrite_sigspecs2(functor);
	प्रति (स्वचालित &it : connections_) {
		functor(it.first, it.second);
	}
}

प्रतिरूपम्<typename T>
शून्य RTLIL::Cell::rewrite_sigspecs(T &functor) {
	प्रति (स्वचालित &it : connections_)
		functor(it.second);
}

प्रतिरूपम्<typename T>
शून्य RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	प्रति (स्वचालित &it : connections_)
		functor(it.second);
}

प्रतिरूपम्<typename T>
शून्य RTLIL::Case_नियमः::rewrite_sigspecs(T &functor) {
	प्रति (स्वचालित &it : compare)
		functor(it);
	प्रति (स्वचालित &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	प्रति (स्वचालित it : switches)
		it->rewrite_sigspecs(functor);
}

प्रतिरूपम्<typename T>
शून्य RTLIL::Case_नियमः::rewrite_sigspecs2(T &functor) {
	प्रति (स्वचालित &it : compare)
		functor(it);
	प्रति (स्वचालित &it : actions) {
		functor(it.first, it.second);
	}
	प्रति (स्वचालित it : switches)
		it->rewrite_sigspecs2(functor);
}

प्रतिरूपम्<typename T>
शून्य RTLIL::Switch_नियमः::rewrite_sigspecs(T &functor)
{
	functor(सङ्केत);
	प्रति (स्वचालित it : cases)
		it->rewrite_sigspecs(functor);
}

प्रतिरूपम्<typename T>
शून्य RTLIL::Switch_नियमः::rewrite_sigspecs2(T &functor)
{
	functor(सङ्केत);
	प्रति (स्वचालित it : cases)
		it->rewrite_sigspecs2(functor);
}

प्रतिरूपम्<typename T>
शून्य RTLIL::Sync_नियमः::rewrite_sigspecs(T &functor)
{
	functor(सङ्केत);
	प्रति (स्वचालित &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	प्रति (स्वचालित &it : mem_write_actions) {
		functor(it.स्थानाङ्कः);
		functor(it.दत्त);
		functor(it.enable);
	}
}

प्रतिरूपम्<typename T>
शून्य RTLIL::Sync_नियमः::rewrite_sigspecs2(T &functor)
{
	functor(सङ्केत);
	प्रति (स्वचालित &it : actions) {
		functor(it.first, it.second);
	}
	प्रति (स्वचालित &it : mem_write_actions) {
		functor(it.स्थानाङ्कः);
		functor(it.दत्त);
		functor(it.enable);
	}
}

प्रतिरूपम्<typename T>
शून्य RTLIL::Process::rewrite_sigspecs(T &functor)
{
	मूलम्_case.rewrite_sigspecs(functor);
	प्रति (स्वचालित it : syncs)
		it->rewrite_sigspecs(functor);
}

प्रतिरूपम्<typename T>
शून्य RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	मूलम्_case.rewrite_sigspecs2(functor);
	प्रति (स्वचालित it : syncs)
		it->rewrite_sigspecs2(functor);
}

#अन्तर्भाव "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
