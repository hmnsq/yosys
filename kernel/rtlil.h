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

#अगर_अघोषित RTLIL_H
#घोषणा RTLIL_H

#समावेश "kernel/yosys_common.h"
#समावेश "kernel/yosys.h"
#समावेश "kernel/twine.h"

#समावेश <deque>
#समावेश <string_view>
#समावेश <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	क्रमागत State : अचिन्हित अक्षर {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	क्रमागत SyncType : अचिन्हित अक्षर {
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
	क्रमागत ConstFlags : अचिन्हित अक्षर {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	क्रमागत SelectPartials : अचिन्हित अक्षर {
		SELECT_ALL = 0,          // include partial modules
		SELECT_WHOLE_ONLY = 1,   // ignore partial modules
		SELECT_WHOLE_WARN = 2,   // call log_warning on partial module
		SELECT_WHOLE_ERR = 3,    // call log_error on partial module
		SELECT_WHOLE_CMDERR = 4  // call log_cmd_error on partial module
	};

	क्रमागत SelectBoxes : अचिन्हित अक्षर {
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

	क्रमागत PortDir : अचिन्हित अक्षर  {
		PD_UNKNOWN = 0,
		PD_निवेश = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr पूर्णांक WIDTH_LIMIT = 1 << 30;

	काष्ठा Const;
	काष्ठा AttrObject;
	काष्ठा SelectedMember;
	काष्ठा Selection;
	काष्ठा Monitor;
	काष्ठा Design;
	काष्ठा Module;
	काष्ठा Wire;
	काष्ठा Memory;
	काष्ठा Cell;
	काष्ठा SigChunk;
	काष्ठा SigBit;
	काष्ठा SigSpecIterator;
	काष्ठा SigSpecConstIterator;
	काष्ठा SigSpec;
	काष्ठा CaseRule;
	काष्ठा SwitchRule;
	काष्ठा MemWriteAction;
	काष्ठा SyncRule;
	काष्ठा Process;

	प्रकार std::pair<SigSpec, SigSpec> SigSig;
};

काष्ठा SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_PREFIX ID; }
namespace RTLIL { using YOSYS_NAMESPACE_PREFIX IdString; }

namespace RTLIL {
	व्योम copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			स्थिर dict<IdString, RTLIL::Const> &src,
			स्थिर RTLIL::Design *src_design, RTLIL::Design *dst_design);

	बाह्य dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	स्थिर pool<IdString> &builtin_ff_cell_types();

	अटल अंतरभूत std::string escape_id(स्थिर std::string &str) {
		अगर (str.आकार() > 0 && str[0] != '\\' && str[0] != '$')
			वापस "\\" + str;
		वापस str;
	}

	अटल अंतरभूत std::string unescape_id(स्थिर std::string &str) {
		अगर (str.आकार() < 2)
			वापस str;
		अगर (str[0] != '\\')
			वापस str;
		अगर (str[1] == '$' || str[1] == '\\')
			वापस str;
		अगर (str[1] >= '0' && str[1] <= '9')
			वापस str;
		वापस str.substr(1);
	}

	ढाँचा <typename T> using sort_by_name_id = IdString::compare_ptr_by_name<T>;

	ढाँचा <typename T> काष्ठा sort_by_name_str {
		bool चालक()(T *a, T *b) स्थिर {
			वापस a->name.lt_by_name(b->name);
		}
	};

	काष्ठा sort_by_id_str {
		स्थिर TwinePool& pool;
		explicit sort_by_id_str(स्थिर TwinePool& pool) : pool(pool) {}
		bool चालक()(RTLIL::IdString a, RTLIL::IdString b) स्थिर {
			वापस pool.compare_by_name(a, b) < 0;
		}
	};

	अटल अंतरभूत std::string encode_filename(स्थिर std::string &filename)
	{
		std::stringstream val;
		अगर (!std::any_of(filename.begin(), filename.end(), [](अक्षर c) {
			वापस static_cast<अचिन्हित अक्षर>(c) < 33 || static_cast<अचिन्हित अक्षर>(c) > 126;
		})) वापस filename;
		क्रम (अचिन्हित अक्षर स्थिर c : filename) {
			अगर (c < 33 || c > 126)
				val << stringf("$%02x", c);
			अन्यथा
				val << c;
		}
		वापस val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_and         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_or          (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_xor         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_xnor        (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);

	RTLIL::Const const_reduce_and  (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_reduce_or   (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_reduce_xor  (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_reduce_xnor (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_reduce_bool (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);

	RTLIL::Const const_logic_not   (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_logic_and   (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_logic_or    (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);

	RTLIL::Const const_shl         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_shr         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_sshl        (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_sshr        (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_shift       (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_shiftx      (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);

	RTLIL::Const const_lt          (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_le          (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_eq          (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_ne          (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_eqx         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_nex         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_ge          (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_gt          (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);

	RTLIL::Const const_add         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_sub         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_mul         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_div         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_divfloor    (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_modfloor    (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_mod         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_pow         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);

	RTLIL::Const const_pos         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_buf         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);
	RTLIL::Const const_neg         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len);

	RTLIL::Const const_mux         (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, स्थिर RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, स्थिर RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2);
	RTLIL::Const const_demux       (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, स्थिर RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	ढाँचा<typename T, typename कुंजी_2 = IdString>
	काष्ठा ObjIterator {
		using iterator_category = std::forward_iterator_tag;
		using मूल्य_type = T;
		using difference_type = सूचक_भेद_प्रकार;
		using pointer = T*;
		using reference = T&;
		typename dict<कुंजी_2, T>::iterator it;
		dict<कुंजी_2, T> *list_p;
		पूर्णांक *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, पूर्णांक *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			अगर (list_p->empty()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} अन्यथा {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(स्थिर RTLIL::ObjIterator<T, कुंजी_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			अगर (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &चालक=(स्थिर RTLIL::ObjIterator<T, कुंजी_2> &other) {
			अगर (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			अगर (refcount_p)
				(*refcount_p)++;
			वापस *this;
		}

		~ObjIterator() {
			अगर (refcount_p)
				(*refcount_p)--;
		}

		अंतरभूत T चालक*() स्थिर {
			log_assert(list_p != nullptr);
			वापस it->second;
		}

		अंतरभूत bool चालक!=(स्थिर RTLIL::ObjIterator<T, कुंजी_2> &other) स्थिर {
			अगर (list_p == nullptr || other.list_p == nullptr)
				वापस list_p != other.list_p;
			वापस it != other.it;
		}


		अंतरभूत bool चालक==(स्थिर RTLIL::ObjIterator<T, कुंजी_2> &other) स्थिर {
			वापस !(*this != other);
		}

		अंतरभूत ObjIterator<T, कुंजी_2>& चालक++() {
			log_assert(list_p != nullptr);
			अगर (++it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			वापस *this;
		}

		अंतरभूत ObjIterator<T, कुंजी_2>& चालक+=(पूर्णांक amt) {
			log_assert(list_p != nullptr);
			it += amt;
			अगर (it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			वापस *this;
		}

		अंतरभूत ObjIterator<T, कुंजी_2> चालक+(पूर्णांक amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, कुंजी_2> new_obj(*this);
			new_obj.it += amt;
			अगर (new_obj.it == list_p->end()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			वापस new_obj;
		}

		अंतरभूत स्थिर ObjIterator<T, कुंजी_2> चालक++(पूर्णांक) {
			ObjIterator<T, कुंजी_2> परिणाम(*this);
			++(*this);
			वापस परिणाम;
		}
	};

	ढाँचा<typename T, typename कुंजी_2 = IdString>
	काष्ठा ObjRange
	{
		dict<कुंजी_2, T> *list_p;
		पूर्णांक *refcount_p;

		ObjRange(decltype(list_p) list_p, पूर्णांक *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, कुंजी_2> begin() { वापस RTLIL::ObjIterator<T, कुंजी_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, कुंजी_2> end() { वापस RTLIL::ObjIterator<T, कुंजी_2>(); }

		माप_प्रकार आकार() स्थिर {
			वापस list_p->आकार();
		}

		चालक pool<T>() स्थिर {
			pool<T> परिणाम;
			क्रम (स्वतः &it : *list_p)
				परिणाम.जोड़ो(it.second);
			वापस परिणाम;
		}

		चालक std::vector<T>() स्थिर {
			std::vector<T> परिणाम;
			परिणाम.reserve(list_p->आकार());
			क्रम (स्वतः &it : *list_p)
				परिणाम.धकेलो_back(it.second);
			वापस परिणाम;
		}

		pool<T> to_pool() स्थिर { वापस *this; }
		std::vector<T> to_vector() स्थिर { वापस *this; }
	};
};

काष्ठा RTLIL::Const
{
	लघु पूर्णांक flags;
निजी:
	मित्र class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::vector<RTLIL::State>;
	क्रमागत class backing_tag: bool { bits, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_tag tag;
	जोड़ {
		bitvectype bits_;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_bits() स्थिर { वापस tag == backing_tag::bits; }
	bool is_str() स्थिर { वापस tag == backing_tag::string; }

	bitvectype* get_if_bits() { वापस is_bits() ? &bits_ : शून्य; }
	std::string* get_if_str() { वापस is_str() ? &str_ : शून्य; }
	स्थिर bitvectype* get_if_bits() स्थिर { वापस is_bits() ? &bits_ : शून्य; }
	स्थिर std::string* get_if_str() स्थिर { वापस is_str() ? &str_ : शून्य; }

	bitvectype& get_bits();
	std::string& get_str();
	स्थिर bitvectype& get_bits() स्थिर;
	स्थिर std::string& get_str() स्थिर;
	std::vector<RTLIL::State>& bits_internal();
	व्योम bitvectorize_internal();

खुला:
	Const() : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::vector<RTLIL::State>()) {}
	Const(std::string str);
	Const(दीर्घ दीर्घ val); // default width is 32
	Const(दीर्घ दीर्घ val, पूर्णांक width);
	Const(RTLIL::State bit, पूर्णांक width = 1);
	Const(std::vector<RTLIL::State> bits) : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::move(bits)) {}
	Const(स्थिर std::vector<bool> &bits);
	Const(स्थिर RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &चालक =(स्थिर RTLIL::Const &other);
	~Const();

	काष्ठा Builder
	{
		Builder() {}
		Builder(पूर्णांक expected_width) { bits.reserve(expected_width); }
		व्योम धकेलो_back(RTLIL::State b) { bits.धकेलो_back(b); }
		पूर्णांक आकार() स्थिर { वापस static_cast<पूर्णांक>(bits.आकार()); }
		Const build() { वापस Const(std::move(bits)); }
	निजी:
		std::vector<RTLIL::State> bits;
	};

	bool चालक <(स्थिर RTLIL::Const &other) स्थिर;
	bool चालक ==(स्थिर RTLIL::Const &other) स्थिर;
	bool चालक !=(स्थिर RTLIL::Const &other) स्थिर;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::vector<RTLIL::State>& bits() { वापस bits_internal(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	व्योम bitvectorize() स्थिर { const_cast<Const*>(this)->bitvectorize_internal(); }

	bool as_bool() स्थिर;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	पूर्णांक as_int(bool is_signed = false) स्थिर;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_to_int(bool is_signed = false) स्थिर;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<पूर्णांक> try_as_int(bool is_signed = false) स्थिर;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	पूर्णांक as_int_saturating(bool is_signed = false) स्थिर;

	व्योम tag_bare_integer_const(स्थिर std::string &मूल्य);

	std::string as_string(स्थिर अक्षर* any = "-") स्थिर;
	अटल Const from_string(स्थिर std::string &str);
	std::vector<RTLIL::State> to_bits() स्थिर;

	std::string decode_string() स्थिर;
	पूर्णांक आकार() स्थिर;
	bool empty() स्थिर;

	व्योम append(स्थिर RTLIL::Const &other);
	व्योम set(पूर्णांक i, RTLIL::State state) {
		bits_internal()[i] = state;
	}
	व्योम resize(पूर्णांक आकार, RTLIL::State fill) {
    log_assert(आकार >= 0 && आकार < RTLIL::WIDTH_LIMIT);
		bits_internal().resize(आकार, fill);
	}

	class const_iterator {
	निजी:
		स्थिर Const* parent;
		माप_प्रकार idx;

	खुला:
		using iterator_category = std::bidirectional_iterator_tag;
		using मूल्य_type = State;
		using difference_type = std::सूचक_भेद_प्रकार;
		using pointer = स्थिर State*;
		using reference = स्थिर State&;

		const_iterator(स्थिर Const& c, माप_प्रकार i) : parent(&c), idx(i) {}

		State चालक*() स्थिर;

		const_iterator& चालक++() { ++idx; वापस *this; }
		const_iterator& चालक--() { --idx; वापस *this; }
		const_iterator चालक++(पूर्णांक) { const_iterator परिणाम(*this); ++idx; वापस परिणाम; }
		const_iterator चालक--(पूर्णांक) { const_iterator परिणाम(*this); --idx; वापस परिणाम; }
		const_iterator& चालक+=(पूर्णांक i) { idx += i; वापस *this; }

		const_iterator चालक+(पूर्णांक add) {
			वापस const_iterator(*parent, idx + add);
		}
		const_iterator चालक-(पूर्णांक sub) {
			वापस const_iterator(*parent, idx - sub);
		}
		पूर्णांक चालक-(स्थिर const_iterator& other) {
			वापस idx - other.idx;
		}

		bool चालक==(स्थिर const_iterator& other) स्थिर {
			वापस idx == other.idx;
		}

		bool चालक!=(स्थिर const_iterator& other) स्थिर {
			वापस !(*this == other);
		}
	};

	class iterator {
	निजी:
		Const* parent;
		माप_प्रकार idx;

	खुला:
		class proxy {
		निजी:
			Const* parent;
			माप_प्रकार idx;
		खुला:
			proxy(Const* parent, माप_प्रकार idx) : parent(parent), idx(idx) {}
			चालक State() स्थिर { वापस (*parent)[idx]; }
			proxy& चालक=(State s) { parent->set(idx, s); वापस *this; }
			proxy& चालक=(स्थिर proxy& other) { parent->set(idx, (*other.parent)[other.idx]); वापस *this; }
		};

		using iterator_category = std::bidirectional_iterator_tag;
		using मूल्य_type = State;
		using difference_type = std::सूचक_भेद_प्रकार;
		using pointer = proxy*;
		using reference = proxy;

		iterator(Const& c, माप_प्रकार i) : parent(&c), idx(i) {}

		proxy चालक*() स्थिर { वापस proxy(parent, idx); }
		iterator& चालक++() { ++idx; वापस *this; }
		iterator& चालक--() { --idx; वापस *this; }
		iterator चालक++(पूर्णांक) { iterator परिणाम(*this); ++idx; वापस परिणाम; }
		iterator चालक--(पूर्णांक) { iterator परिणाम(*this); --idx; वापस परिणाम; }
		iterator& चालक+=(पूर्णांक i) { idx += i; वापस *this; }

		iterator चालक+(पूर्णांक add) {
			वापस iterator(*parent, idx + add);
		}
		iterator चालक-(पूर्णांक sub) {
			वापस iterator(*parent, idx - sub);
		}
		पूर्णांक चालक-(स्थिर iterator& other) {
			वापस idx - other.idx;
		}

		bool चालक==(स्थिर iterator& other) स्थिर {
			वापस idx == other.idx;
		}

		bool चालक!=(स्थिर iterator& other) स्थिर {
			वापस !(*this == other);
		}
	};

	const_iterator begin() स्थिर {
		वापस const_iterator(*this, 0);
	}
	const_iterator end() स्थिर {
		वापस const_iterator(*this, आकार());
	}
	iterator begin() {
		वापस iterator(*this, 0);
	}
	iterator end() {
		वापस iterator(*this, आकार());
	}
	State back() स्थिर {
		वापस *(end() - 1);
	}
	State front() स्थिर {
		वापस *begin();
	}
	State at(माप_प्रकार i) स्थिर {
		वापस *const_iterator(*this, i);
	}
	State चालक[](माप_प्रकार i) स्थिर {
		वापस *const_iterator(*this, i);
	}

	bool is_fully_zero() स्थिर;
	bool is_fully_ones() स्थिर;
	bool is_fully_def() स्थिर;
	bool is_fully_undef() स्थिर;
	bool is_fully_undef_x_only() स्थिर;
	bool is_onehot(पूर्णांक *pos = nullptr) स्थिर;

	RTLIL::Const extract(पूर्णांक offset, पूर्णांक len = 1, RTLIL::State padding = RTLIL::State::S0) स्थिर;

	// find the MSB without redundant leading bits
	पूर्णांक get_min_आकार(bool is_signed) स्थिर;

	// compress representation to the minimum required bits
	व्योम compress(bool is_signed = false);

	std::optional<पूर्णांक> as_int_compress(bool is_signed) स्थिर;

	व्योम extu(पूर्णांक width) {
		resize(width, RTLIL::State::S0);
	}

	व्योम exts(पूर्णांक width) {
		resize(width, empty() ? RTLIL::State::Sx : back());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;
};

काष्ठा RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	bool has_attribute(RTLIL::IdString id) स्थिर;

	व्योम set_bool_attribute(RTLIL::IdString id, bool मूल्य=true);
	bool get_bool_attribute(RTLIL::IdString id) स्थिर;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) स्थिर {
		वापस get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	व्योम set_string_attribute(RTLIL::IdString id, string मूल्य);
	string get_string_attribute(RTLIL::IdString id) स्थिर;

	व्योम set_strpool_attribute(RTLIL::IdString id, स्थिर pool<string> &तथ्य);
	व्योम add_strpool_attribute(RTLIL::IdString id, स्थिर pool<string> &तथ्य);
	pool<string> get_strpool_attribute(RTLIL::IdString id) स्थिर;

	व्योम set_src_attribute(स्थिर std::string &src) {
		set_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() स्थिर {
		वापस get_string_attribute(ID::src);
	}

	व्योम set_hdlname_attribute(स्थिर vector<string> &hierarchy);
	vector<string> get_hdlname_attribute() स्थिर;

	व्योम set_intvec_attribute(RTLIL::IdString id, स्थिर vector<पूर्णांक> &तथ्य);
	vector<पूर्णांक> get_intvec_attribute(RTLIL::IdString id) स्थिर;
};

#समावेश "kernel/rtlil_twine_compat.h"

काष्ठा RTLIL::SelectedMember
{
	RTLIL::AttrObject *obj;
	RTLIL::PooledName name;
};

काष्ठा RTLIL::SigChunk
{
	RTLIL::Wire *wire;
	std::vector<RTLIL::State> तथ्य; // only used if wire == NULL, LSB at index 0
	पूर्णांक width, offset;

	SigChunk() : wire(nullptr), width(0), offset(0) {}
	SigChunk(स्थिर RTLIL::Const &मूल्य) : wire(nullptr), तथ्य(मूल्य.to_bits()), width(Get_आकार(तथ्य)), offset(0) {}
	SigChunk(RTLIL::Const &&मूल्य) : wire(nullptr), तथ्य(मूल्य.to_bits()), width(Get_आकार(तथ्य)), offset(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), width(Get_आकार(wire)), offset(0) {}
	SigChunk(RTLIL::Wire *wire, पूर्णांक offset, पूर्णांक width = 1) : wire(wire), width(width), offset(offset) {}
	SigChunk(स्थिर std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(पूर्णांक val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(पूर्णांक val, पूर्णांक width) : SigChunk(RTLIL::Const(val, width)) {}
	SigChunk(RTLIL::State bit, पूर्णांक width = 1) : SigChunk(RTLIL::Const(bit, width)) {}
	SigChunk(स्थिर RTLIL::SigBit &bit);

	RTLIL::SigChunk extract(पूर्णांक offset, पूर्णांक लंबाई) स्थिर;
	RTLIL::SigBit चालक[](पूर्णांक offset) स्थिर;
	अंतरभूत पूर्णांक आकार() स्थिर { वापस width; }
	अंतरभूत bool is_wire() स्थिर { वापस wire != शून्य; }

	bool चालक <(स्थिर RTLIL::SigChunk &other) स्थिर;
	bool चालक ==(स्थिर RTLIL::SigChunk &other) स्थिर;
	bool चालक !=(स्थिर RTLIL::SigChunk &other) स्थिर;
};

काष्ठा RTLIL::SigBit
{
	RTLIL::Wire *wire;
	जोड़ {
		RTLIL::State तथ्य; // used if wire == NULL
		पूर्णांक offset;        // used if wire != NULL
	};

	SigBit();
	SigBit(RTLIL::State bit);
	explicit SigBit(bool bit);
	SigBit(RTLIL::Wire *wire);
	SigBit(RTLIL::Wire *wire, पूर्णांक offset);
	SigBit(स्थिर RTLIL::SigChunk &chunk);
	SigBit(स्थिर RTLIL::SigChunk &chunk, पूर्णांक सूचकांक);
	SigBit(स्थिर RTLIL::SigSpec &sig);
	SigBit(स्थिर RTLIL::SigBit &sigbit) = शेष;
	RTLIL::SigBit &चालक =(स्थिर RTLIL::SigBit &other) = शेष;

	अंतरभूत bool is_wire() स्थिर { वापस wire != शून्य; }

	bool चालक <(स्थिर RTLIL::SigBit &other) स्थिर;
	bool चालक ==(स्थिर RTLIL::SigBit &other) स्थिर;
	bool चालक !=(स्थिर RTLIL::SigBit &other) स्थिर;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर;
	[[nodiscard]] Hasher hash_शीर्ष() स्थिर;
};

namespace hashlib {
	ढाँचा <>
	काष्ठा hash_ops<RTLIL::SigBit> {
		अटल अंतरभूत bool cmp(स्थिर RTLIL::SigBit &a, स्थिर RTLIL::SigBit &b) {
			वापस a == b;
		}
		[[nodiscard]] अटल अंतरभूत Hasher hash(स्थिर RTLIL::SigBit sb) {
			वापस sb.hash_शीर्ष();
		}
		[[nodiscard]] अटल अंतरभूत Hasher hash_into(स्थिर RTLIL::SigBit sb, Hasher h) {
			वापस sb.hash_into(h);
		}
	};
};

काष्ठा RTLIL::SigSpecIterator
{
	प्रकार std::निवेश_iterator_tag iterator_category;
	प्रकार RTLIL::SigBit मूल्य_type;
	प्रकार सूचक_भेद_प्रकार difference_type;
	प्रकार RTLIL::SigBit* pointer;
	प्रकार RTLIL::SigBit& reference;

	RTLIL::SigSpec *sig_p;
	पूर्णांक सूचकांक;

	अंतरभूत RTLIL::SigBit &चालक*() स्थिर;
	अंतरभूत bool चालक!=(स्थिर RTLIL::SigSpecIterator &other) स्थिर { वापस सूचकांक != other.सूचकांक; }
	अंतरभूत bool चालक==(स्थिर RTLIL::SigSpecIterator &other) स्थिर { वापस सूचकांक == other.सूचकांक; }
	अंतरभूत व्योम चालक++() { सूचकांक++; }
};

काष्ठा RTLIL::SigSpecConstIterator
{
	प्रकार std::निवेश_iterator_tag iterator_category;
	प्रकार RTLIL::SigBit मूल्य_type;
	प्रकार सूचक_भेद_प्रकार difference_type;
	प्रकार RTLIL::SigBit* pointer;
	प्रकार RTLIL::SigBit& reference;

	स्थिर RTLIL::SigSpec *sig_p;
	RTLIL::SigBit bit;
	पूर्णांक सूचकांक;

	अंतरभूत स्थिर RTLIL::SigBit &चालक*();
	अंतरभूत bool चालक!=(स्थिर RTLIL::SigSpecConstIterator &other) स्थिर { वापस सूचकांक != other.सूचकांक; }
	अंतरभूत bool चालक==(स्थिर RTLIL::SigSpecIterator &other) स्थिर { वापस सूचकांक == other.सूचकांक; }
	अंतरभूत व्योम चालक++() { सूचकांक++; }
};

काष्ठा RTLIL::SigSpec
{
निजी:
	मित्र class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, Extract);
	क्रमागत Representation : अक्षर {
		CHUNK,
		BITS,
	};
	// An AtomicHash is either clear or a nonzero integer.
	काष्ठा AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(स्थिर AtomicHash &rhs) : atomic_(rhs.load()) {}
		AtomicHash &चालक=(स्थिर AtomicHash &rhs) { store(rhs.load()); वापस *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> पढ़ो() स्थिर {
			Hasher::hash_t मूल्य = load();
			अगर (मूल्य == 0)
				वापस std::nullopt;
			वापस मूल्य;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		व्योम set(Hasher::hash_t मूल्य) स्थिर {
			log_assert(मूल्य != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(मूल्य, std::memory_order_relaxed);
			log_assert(old == 0 || old == मूल्य);
		}
		व्योम clear() { store(0); }
	निजी:
		पूर्णांक load() स्थिर { वापस atomic_.load(std::memory_order_relaxed); }
		व्योम store(Hasher::hash_t मूल्य) स्थिर {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(मूल्य, std::memory_order_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	जोड़ {
		RTLIL::SigChunk chunk_;
		std::vector<RTLIL::SigBit> bits_; // LSB at index 0
	};

	व्योम init_empty_bits() {
		rep_ = BITS;
		new (&bits_) std::vector<RTLIL::SigBit>;
	}

	व्योम unpack();
	अंतरभूत व्योम inline_unpack() {
		अगर (rep_ == CHUNK)
			unpack();
	}
	व्योम try_repack();

	Hasher::hash_t updhash() स्थिर;
	व्योम destroy() {
		अगर (rep_ == CHUNK)
			chunk_.~SigChunk();
		अन्यथा
			bits_.~vector();
	}
	मित्र काष्ठा Chunks;

खुला:
	SigSpec() { init_empty_bits(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(स्थिर SigSpec &मूल्य) : rep_(मूल्य.rep_), hash_(मूल्य.hash_) {
		अगर (मूल्य.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(मूल्य.chunk_);
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(मूल्य.bits_);
	}
	SigSpec(SigSpec &&मूल्य) : rep_(मूल्य.rep_), hash_(मूल्य.hash_) {
		अगर (मूल्य.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(मूल्य.chunk_));
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(मूल्य.bits_));
	}
	SigSpec(स्थिर RTLIL::Const &मूल्य);
	SigSpec(RTLIL::Const &&मूल्य);
	SigSpec(स्थिर RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, पूर्णांक offset, पूर्णांक width = 1);
	SigSpec(स्थिर std::string &str);
	SigSpec(पूर्णांक val, पूर्णांक width = 32);
	SigSpec(RTLIL::State bit, पूर्णांक width = 1);
	SigSpec(स्थिर RTLIL::SigBit &bit, पूर्णांक width = 1);
	SigSpec(स्थिर std::vector<RTLIL::SigChunk> &chunks);
	SigSpec(स्थिर std::vector<RTLIL::SigBit> &bits);
	SigSpec(स्थिर pool<RTLIL::SigBit> &bits);
	SigSpec(स्थिर std::set<RTLIL::SigBit> &bits);
	explicit SigSpec(bool bit);
	~SigSpec() {
		destroy();
	}

	SigSpec &चालक=(स्थिर SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		अगर (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(rhs.bits_);
		वापस *this;
	}
	SigSpec &चालक=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		अगर (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(rhs.bits_));
		वापस *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	काष्ठा Chunks {
		Chunks(स्थिर SigSpec &spec) : spec(spec) {}
		काष्ठा const_iterator {
			using iterator_category = std::forward_iterator_tag;
			using मूल्य_type = स्थिर SigChunk &;
			using difference_type = std::सूचक_भेद_प्रकार;
			using pointer = स्थिर SigChunk *;
			using reference = स्थिर SigChunk &;

			स्थिर SigSpec &spec;
			पूर्णांक bit_सूचकांक;
			SigChunk chunk;

			const_iterator(स्थिर SigSpec &spec) : spec(spec) {
				bit_सूचकांक = 0;
				अगर (spec.rep_ == BITS)
					अग्र_chunk_bits();
			}
			क्रमागत End { END };
			const_iterator(स्थिर SigSpec &spec, End) : spec(spec) {
				bit_सूचकांक = spec.आकार();
			}
			व्योम अग्र_chunk_bits();

			स्थिर SigChunk &चालक*() {
				अगर (spec.rep_ == CHUNK)
					वापस spec.chunk_;
				वापस chunk;
			};
			स्थिर SigChunk *चालक->() { वापस &**this; }
			const_iterator &चालक++() {
				bit_सूचकांक += (**this).width;
				अगर (spec.rep_ == BITS)
					अग्र_chunk_bits();
				वापस *this;
			}
			bool चालक==(स्थिर const_iterator &rhs) स्थिर { वापस bit_सूचकांक == rhs.bit_सूचकांक; }
			bool चालक!=(स्थिर const_iterator &rhs) स्थिर { वापस !(*this == rhs); }
		};
		const_iterator begin() स्थिर { वापस const_iterator(spec); }
		const_iterator end() स्थिर {
			const_iterator it(spec, const_iterator::END);
			वापस it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rbegin() {
			ensure_chunk_vector();
			वापस chunk_vector.rbegin();
		}
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rend() {
			ensure_chunk_vector();
			वापस chunk_vector.rend();
		}
		पूर्णांक आकार() {
			ensure_chunk_vector();
			वापस chunk_vector.आकार();
		}
		पूर्णांक आकार() स्थिर {
			वापस std::distance(begin(), end());
		}
		स्थिर SigChunk &at(पूर्णांक सूचकांक) {
			ensure_chunk_vector();
			वापस chunk_vector.at(सूचकांक);
		}
		चालक स्थिर std::vector<RTLIL::SigChunk>&() {
			ensure_chunk_vector();
			वापस chunk_vector;
		}
	निजी:
		व्योम ensure_chunk_vector() {
			अगर (spec.आकार() > 0 && chunk_vector.empty()) {
				क्रम (स्थिर RTLIL::SigChunk &c : *this)
					chunk_vector.धकेलो_back(c);
			}
		}
		स्थिर SigSpec &spec;
		std::vector<RTLIL::SigChunk> chunk_vector;
	};
	मित्र काष्ठा Chunks::const_iterator;

	अंतरभूत Chunks chunks() स्थिर { वापस {*this}; }
	अंतरभूत स्थिर SigSpec &bits() स्थिर { वापस *this; }

	अंतरभूत पूर्णांक आकार() स्थिर { वापस rep_ == CHUNK ? chunk_.width : Get_आकार(bits_); }
	अंतरभूत bool empty() स्थिर { वापस आकार() == 0; };

	अंतरभूत RTLIL::SigBit &चालक[](पूर्णांक सूचकांक) { inline_unpack(); hash_.clear(); वापस bits_.at(सूचकांक); }
	अंतरभूत RTLIL::SigBit चालक[](पूर्णांक सूचकांक) स्थिर {
		अगर (rep_ == CHUNK) {
			अगर (सूचकांक < 0 || सूचकांक >= chunk_.width)
				throw std::out_of_range("SigSpec::operator[]");
			अगर (chunk_.wire)
				वापस RTLIL::SigBit(chunk_.wire, chunk_.offset + सूचकांक);
			वापस RTLIL::SigBit(chunk_.तथ्य[सूचकांक]);
		}
		वापस bits_.at(सूचकांक);
	}

	अंतरभूत RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.सूचकांक = 0; वापस it; }
	अंतरभूत RTLIL::SigSpecIterator end() { RTLIL::SigSpecIterator it; it.sig_p = this; it.सूचकांक = आकार(); वापस it; }

	अंतरभूत RTLIL::SigSpecConstIterator begin() स्थिर { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.सूचकांक = 0; वापस it; }
	अंतरभूत RTLIL::SigSpecConstIterator end() स्थिर { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.सूचकांक = आकार(); वापस it; }

	व्योम sort();
	व्योम sort_and_unify();

	व्योम replace(स्थिर RTLIL::SigSpec &pattern, स्थिर RTLIL::SigSpec &with);
	व्योम replace(स्थिर RTLIL::SigSpec &pattern, स्थिर RTLIL::SigSpec &with, RTLIL::SigSpec *other) स्थिर;

	व्योम replace(स्थिर dict<RTLIL::SigBit, RTLIL::SigBit> &rules);
	व्योम replace(स्थिर dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) स्थिर;

	व्योम replace(स्थिर std::map<RTLIL::SigBit, RTLIL::SigBit> &rules);
	व्योम replace(स्थिर std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) स्थिर;

	व्योम replace(पूर्णांक offset, स्थिर RTLIL::SigSpec &with);

	व्योम हटाओ(स्थिर RTLIL::SigSpec &pattern);
	व्योम हटाओ(स्थिर RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) स्थिर;
	व्योम remove2(स्थिर RTLIL::SigSpec &pattern, RTLIL::SigSpec *other);

	व्योम हटाओ(स्थिर pool<RTLIL::SigBit> &pattern);
	व्योम हटाओ(स्थिर pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) स्थिर;
	व्योम remove2(स्थिर pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	व्योम remove2(स्थिर std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	व्योम remove2(स्थिर pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other);

	व्योम हटाओ(पूर्णांक offset, पूर्णांक लंबाई = 1);
	व्योम remove_const();

	RTLIL::SigSpec extract(स्थिर RTLIL::SigSpec &pattern, स्थिर RTLIL::SigSpec *other = शून्य) स्थिर;
	RTLIL::SigSpec extract(स्थिर pool<RTLIL::SigBit> &pattern, स्थिर RTLIL::SigSpec *other = शून्य) स्थिर;
	RTLIL::SigSpec extract(पूर्णांक offset, पूर्णांक लंबाई = 1) स्थिर;
	RTLIL::SigSpec extract_end(पूर्णांक offset) स्थिर { वापस extract(offset, आकार() - offset); }

	व्योम rewrite_wires(std::function<व्योम(RTLIL::Wire*& wire)> rewrite);

	RTLIL::SigBit lsb() स्थिर { log_assert(आकार()); वापस (*this)[0]; };
	RTLIL::SigBit msb() स्थिर { log_assert(आकार()); वापस (*this)[आकार() - 1]; };
	RTLIL::SigBit front() स्थिर { वापस (*this)[0]; }
	RTLIL::SigBit back() स्थिर { वापस (*this)[आकार() - 1]; }

	व्योम append(स्थिर RTLIL::SigSpec &संकेत);
	अंतरभूत व्योम append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	अंतरभूत व्योम append(स्थिर RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	अंतरभूत व्योम append(स्थिर RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	व्योम append(स्थिर RTLIL::SigBit &bit);
	अंतरभूत व्योम append(RTLIL::State state) { append(RTLIL::SigBit(state)); }
	अंतरभूत व्योम append(bool bool_) { append(RTLIL::SigBit(bool_)); }

	व्योम extend_u0(पूर्णांक width, bool is_signed = false);

	RTLIL::SigSpec repeat(पूर्णांक अंक) स्थिर;

	व्योम reverse() { inline_unpack(); std::reverse(bits_.begin(), bits_.end()); }

	bool चालक <(स्थिर RTLIL::SigSpec &other) स्थिर;
	bool चालक ==(स्थिर RTLIL::SigSpec &other) स्थिर;
	अंतरभूत bool चालक !=(स्थिर RTLIL::SigSpec &other) स्थिर { वापस !(*this == other); }

	bool is_wire() स्थिर;
	bool is_chunk() स्थिर;
	अंतरभूत bool is_bit() स्थिर { वापस आकार() == 1; }

	bool known_driver() स्थिर;

	bool is_fully_const() स्थिर;
	bool is_fully_zero() स्थिर;
	bool is_fully_ones() स्थिर;
	bool is_fully_def() स्थिर;
	bool is_fully_undef() स्थिर;
	bool has_const() स्थिर;
	bool has_const(State state) स्थिर;
	bool has_marked_bits() स्थिर;
	bool is_onehot(पूर्णांक *pos = nullptr) स्थिर;

	bool as_bool() स्थिर;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	पूर्णांक as_int(bool is_signed = false) स्थिर;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_to_int(bool is_signed = false) स्थिर;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<पूर्णांक> try_as_int(bool is_signed = false) स्थिर;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	पूर्णांक as_int_saturating(bool is_signed = false) स्थिर;

	std::string as_string() स्थिर;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() स्थिर;
	RTLIL::Const as_const() स्थिर;
	RTLIL::Wire *as_wire() स्थिर;
	RTLIL::SigChunk as_chunk() स्थिर;
	RTLIL::SigBit as_bit() स्थिर;

	bool match(स्थिर अक्षर* pattern) स्थिर;

	std::set<RTLIL::SigBit> to_sigbit_set() स्थिर;
	pool<RTLIL::SigBit> to_sigbit_pool() स्थिर;
	std::vector<RTLIL::SigBit> to_sigbit_vector() स्थिर;
	std::map<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_map(स्थिर RTLIL::SigSpec &other) स्थिर;
	dict<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_dict(स्थिर RTLIL::SigSpec &other) स्थिर;

	अटल bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	अटल bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	अटल bool parse_rhs(स्थिर RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	चालक std::vector<RTLIL::SigChunk>() स्थिर;
	चालक std::vector<RTLIL::SigBit>() स्थिर { वापस to_sigbit_vector(); }
	स्थिर RTLIL::SigBit &at(पूर्णांक offset, स्थिर RTLIL::SigBit &defval) { वापस offset < आकार() ? (*this)[offset] : defval; }
	RTLIL::SigBit& at(पूर्णांक offset) { वापस (*this)[offset]; }
	RTLIL::SigBit at(पूर्णांक offset) स्थिर { वापस (*this)[offset]; }

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
		Hasher::hash_t val;
		अगर (std::optional<Hasher::hash_t> current = hash_.पढ़ो())
			val = *current;
		अन्यथा
			val = updhash();
		h.eat(val);
		वापस h;
	}

#अगर_अघोषित न_संशोधन
	व्योम check(स्थिर Module *mod = nullptr) स्थिर;
#अन्यथा
	व्योम check(स्थिर Module *mod = nullptr) स्थिर { (व्योम)mod; }
#पूर्ण_अगर
};

काष्ठा RTLIL::Selection
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
	bool boxed_module(RTLIL::IdString mod_name) स्थिर;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) स्थिर;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) स्थिर;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) स्थिर;

	// optimizes this selection for the given design by:
	// - removing non-existent modules and members, any boxed modules and
	//   their members (if selection does not include boxes), and any
	//   partially selected modules with no selected members;
	// - marking partially selected modules as wholly selected if all
	//   members of that module are selected; and
	// - marking selection as a complete_selection if all modules in the
	//   given design are selected, or a full_selection if it does not
	//   include boxes.
	व्योम optimize(RTLIL::Design *design);

	// checks if selection covers full design (may or may not include
	// boxed-modules)
	bool selects_all() स्थिर {
		वापस full_selection || complete_selection;
	}

	// add whole module to this selection
	ढाँचा<typename T1> व्योम select(T1 *module) {
		अगर (!selects_all() && selected_modules.गिनती(module->name) == 0) {
			IdString name = module->name;
			selected_modules.जोड़ो(name);
			selected_members.erase(name);
			अगर (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	ढाँचा<typename T1, typename T2> व्योम select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	ढाँचा<typename T1> व्योम select(T1 *module, RTLIL::IdString member) {
		अगर (!selects_all() && selected_modules.गिनती(module->name.ref()) == 0) {
			selected_members[module->name.ref()].जोड़ो(member);
			अगर (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool empty() स्थिर {
		वापस !selects_all() && selected_modules.empty() && selected_members.empty();
	}

	// clear this selection, leaving it empty
	व्योम clear();

	// create a new selection which is empty
	अटल Selection EmptySelection(RTLIL::Design *design = nullptr) { वापस Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	अटल Selection FullSelection(RTLIL::Design *design = nullptr) { वापस Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	अटल Selection CompleteSelection(RTLIL::Design *design = nullptr) { वापस Selection(true, true, design); };
};

काष्ठा RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर { h.eat(hashidx_); वापस h; }

	Monitor() {
		अटल अचिन्हित पूर्णांक hashidx_गिनती = 123456789;
		hashidx_गिनती = mkhash_xorshift(hashidx_गिनती);
		hashidx_ = hashidx_गिनती;
	}

	भव ~Monitor() { }
	भव व्योम notify_module_add(RTLIL::Module*) { }
	भव व्योम notify_module_del(RTLIL::Module*) { }
	भव व्योम notify_connect(RTLIL::Cell*, RTLIL::IdString, स्थिर RTLIL::SigSpec&, स्थिर RTLIL::SigSpec&) { }
	भव व्योम notify_connect(RTLIL::Module*, स्थिर RTLIL::SigSig&) { }
	भव व्योम notify_connect(RTLIL::Module*, स्थिर std::vector<RTLIL::SigSig>&) { }
	भव व्योम notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
काष्ठा define_map_t;

ढाँचा<typename N>
अंतरभूत constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::Leaf> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, स्थिर अक्षर*> || std::is_same_v<std::decay_t<N>, अक्षर*>;

#घोषणा YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, पूर्णांक> = 0

ढाँचा<typename N>
अंतरभूत constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, स्थिर अक्षर *> || std::is_same_v<std::decay_t<N>, अक्षर *>;

#घोषणा YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, पूर्णांक> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#घोषणा YS_NAME_FWD_POOL(_func, _pool) \
	ढाँचा<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(स्वतः) _func(N name, Rest&&... rest) \
		{ वापस _func(_pool.add(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#घोषणा YS_NAME_FWD_2ND_POOL(_func, _pool) \
	ढाँचा<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(स्वतः) _func(T &&first, N name) \
		{ वापस _func(std::forward<T>(first), _pool.add(std::move(name))); }

#घोषणा YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#घोषणा YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

काष्ठा RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर { h.eat(hashidx_); वापस h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	व्योम bufNormalize(bool enable=true);

	पूर्णांक refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

निजी:
	TwinePool twines_;

खुला:
	TwinePool &twines() { वापस twines_; }
	स्थिर TwinePool &twines() स्थिर { वापस twines_; }

	व्योम absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	माप_प्रकार gc_twines();

	std::vector<std::unique_ptr<AST::Ast_गाँठ>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_defines;

	std::vector<RTLIL::Selection> selection_ढेर;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_active_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	स्थिर RTLIL::Module *module(RTLIL::IdString name) स्थिर;
	ढाँचा<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ वापस module(twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> स्थिर RTLIL::Module *module(N name) स्थिर
		{ वापस module(twines().find(std::string_view(name))); }
	RTLIL::Module *शीर्ष_module() स्थिर;

	bool has(RTLIL::IdString id) स्थिर {
		twines_.check_owned(id);
		वापस modules_.गिनती(id) != 0;
	}
	ढाँचा<typename N, YS_NAME_STRING(N)> bool has(N name) स्थिर
		{ वापस has(twines().find(std::string_view(name))); }

	व्योम add(RTLIL::Module *module);

	RTLIL::Module *addModule(RTLIL::IdString name);
	YS_NAME_FWD_POOL(addModule, twines())
	व्योम हटाओ(RTLIL::Module *module);
	व्योम नाम(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(नाम, twines())

	व्योम scratchpad_unset(स्थिर std::string &varname);

	व्योम scratchpad_set_int(स्थिर std::string &varname, पूर्णांक मूल्य);
	व्योम scratchpad_set_bool(स्थिर std::string &varname, bool मूल्य);
	व्योम scratchpad_set_string(स्थिर std::string &varname, std::string मूल्य);

	पूर्णांक scratchpad_get_int(स्थिर std::string &varname, पूर्णांक default_मूल्य_2 = 0) स्थिर;
	bool scratchpad_get_bool(स्थिर std::string &varname, bool default_मूल्य_2 = false) स्थिर;
	std::string scratchpad_get_string(स्थिर std::string &varname, स्थिर std::string &default_मूल्य_2 = std::string()) स्थिर;

	व्योम sort();
	व्योम sort_modules();
	व्योम check();
	व्योम optimize();

	व्योम clone_into(RTLIL::Design *dst) स्थिर;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) स्थिर;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) स्थिर;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) स्थिर;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) स्थिर;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::Module *mod) स्थिर;

	// push the given selection to the selection stack
	व्योम धकेलो_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	व्योम धकेलो_empty_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	व्योम धकेलो_full_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	व्योम धकेलो_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	व्योम निकालो_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		वापस selection_ढेर.back();
	}

	// get the current selection
	स्थिर RTLIL::Selection &selection() स्थिर {
		वापस selection_ढेर.back();
	}

	// is the current selection a full selection (no boxes)
	bool full_selection() स्थिर {
		वापस selection().full_selection;
	}

	// is the given module in the current selection
	ढाँचा<typename T1> bool selected(T1 *module) स्थिर {
		वापस selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	ढाँचा<typename T1, typename T2> bool selected(T1 *module, T2 *member) स्थिर {
		वापस selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	ढाँचा<typename T1> व्योम select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	ढाँचा<typename T1, typename T2> व्योम select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::vector<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) स्थिर;

	// returns all selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_modules() स्थिर { वापस selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::vector<RTLIL::Module*> selected_unboxed_modules() स्थिर { वापस selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_modules_warn() स्थिर { वापस selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::vector<RTLIL::Module*> selected_whole_modules() स्थिर { वापस selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_whole_modules() स्थिर { वापस selected_modules(SELECT_WHOLE_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::vector<RTLIL::Module*> selected_whole_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) स्थिर { वापस selected_modules(SELECT_WHOLE_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules() स्थिर { वापस selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules_warn() स्थिर { वापस selected_modules(SELECT_WHOLE_WARN, SB_UNBOXED_WARN); }

	अटल std::map<अचिन्हित पूर्णांक, RTLIL::Design*> *get_all_designs(व्योम);

	std::string to_rtlil_str(bool only_selected = true) स्थिर;
};

काष्ठा RTLIL::Module : खुला RTLIL::AttrObject
{
	मित्र काष्ठा RTLIL::Cell;
	मित्र काष्ठा RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर { h.eat(hashidx_); वापस h; }

रक्षित:
	व्योम add(RTLIL::Wire *wire);
	व्योम add(RTLIL::Cell *cell);
	व्योम add(RTLIL::Process *process);

खुला:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() स्थिर { वापस design->twines(); }

	पूर्णांक refcount_wires_;
	पूर्णांक refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::vector<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_मूल्यावली;
	dict<RTLIL::IdString, RTLIL::Memory*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	भव ~Module();
	भव RTLIL::IdString derive(RTLIL::Design *design, स्थिर dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	भव RTLIL::IdString derive(RTLIL::Design *design, स्थिर dict<RTLIL::IdString, RTLIL::Const> &parameters, स्थिर dict<RTLIL::IdString, RTLIL::Module*> &interfaces, स्थिर dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	भव माप_प्रकार गिनती_id(RTLIL::IdString id);
	भव व्योम expand_interfaces(RTLIL::Design *design, स्थिर dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	भव bool reprocess_if_necessary(RTLIL::Design *design);

	भव व्योम sort();
	भव व्योम check();
	भव व्योम optimize();
	भव व्योम makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) स्थिर {
		वापस get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	व्योम connect(स्थिर RTLIL::SigSig &conn);
	व्योम connect(स्थिर RTLIL::SigSpec &lhs, स्थिर RTLIL::SigSpec &rhs);
	व्योम new_connections(स्थिर std::vector<RTLIL::SigSig> &new_conn);
	स्थिर std::vector<RTLIL::SigSig> &connections() स्थिर;

	std::vector<RTLIL::IdString> ports;
	व्योम fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_queue;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_port_queue;
	pool<RTLIL::Wire *> buf_norm_wire_queue;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_सूचकांक;
	व्योम bufNormalize();

	ढाँचा<typename T> व्योम rewrite_sigspecs(T &functor);
	ढाँचा<typename T> व्योम rewrite_sigspecs2(T &functor);
	व्योम cloneInto(RTLIL::Module *new_mod) स्थिर;
	भव RTLIL::Module *clone() स्थिर;
	भव RTLIL::Module *clone(RTLIL::Design *dst) स्थिर;
	भव RTLIL::Module *clone(RTLIL::Design *dst, IdString target_name) स्थिर;

	bool has_memories() स्थिर;
	bool has_processes() स्थिर;

	bool has_memories_warn() स्थिर;
	bool has_processes_warn() स्थिर;

	bool is_selected() स्थिर;
	bool is_selected_whole() स्थिर;

	std::vector<RTLIL::Wire*> selected_wires() स्थिर;
	std::vector<RTLIL::Cell*> selected_cells() स्थिर;
	std::vector<RTLIL::Memory*> selected_memories() स्थिर;
	std::vector<RTLIL::Process*> selected_processes() स्थिर;
	std::vector<RTLIL::SelectedMember> selected_members() स्थिर;

	ढाँचा<typename T> bool selected(T *member) स्थिर {
		वापस design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		अगर (design)
			design->twines().check_owned(id);
		स्वतः it = wires_.find(id);
		वापस it == wires_.end() ? nullptr : it->second;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		अगर (design)
			design->twines().check_owned(id);
		स्वतः it = cells_.find(id);
		वापस it == cells_.end() ? nullptr : it->second;
	}

	स्थिर RTLIL::Wire* wire(RTLIL::IdString id) स्थिर{
		अगर (design)
			design->twines().check_owned(id);
		स्वतः it = wires_.find(id);
		वापस it == wires_.end() ? nullptr : it->second;
	}
	स्थिर RTLIL::Cell* cell(RTLIL::IdString id) स्थिर {
		अगर (design)
			design->twines().check_owned(id);
		स्वतः it = cells_.find(id);
		वापस it == cells_.end() ? nullptr : it->second;
	}

	ढाँचा<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ वापस wire(twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ वापस cell(twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> स्थिर RTLIL::Wire *wire(N name) स्थिर
		{ वापस wire(twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> स्थिर RTLIL::Cell *cell(N name) स्थिर
		{ वापस cell(twines().find(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { वापस RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	पूर्णांक wires_आकार() स्थिर { वापस wires_.आकार(); }
	RTLIL::Wire* wire_at(पूर्णांक सूचकांक) स्थिर { वापस wires_.element(सूचकांक)->second; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { वापस RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	पूर्णांक cells_आकार() स्थिर { वापस cells_.आकार(); }
	RTLIL::Cell* cell_at(पूर्णांक सूचकांक) स्थिर { वापस cells_.element(सूचकांक)->second; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	व्योम हटाओ(स्थिर pool<RTLIL::Wire*> &wires);
	व्योम हटाओ(RTLIL::Cell *cell);
	व्योम हटाओ(RTLIL::Memory *memory);
	व्योम हटाओ(RTLIL::Process *process);

	व्योम नाम(RTLIL::Wire *wire, RTLIL::IdString new_name);
	व्योम नाम(RTLIL::Cell *cell, RTLIL::IdString new_name);
	व्योम नाम(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(नाम)

	व्योम swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2);
	व्योम swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, पूर्णांक &सूचकांक);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *addWire(RTLIL::IdString name, पूर्णांक width = 1);
	RTLIL::Wire *addWire(RTLIL::IdString name, स्थिर RTLIL::Wire *other);
	YS_NAME_FWD(addWire)

	RTLIL::Cell *addCell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *addCell(RTLIL::IdString name, स्थिर RTLIL::Cell *other);
	YS_NAME_FWD(addCell)
	ढाँचा<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *addCell(RTLIL::IdString name, T type) { वापस addCell(name, design->twines().add(std::move(type))); }

	RTLIL::Memory *addMemory(RTLIL::IdString name);
	YS_NAME_FWD(addMemory)
	RTLIL::Memory *addMemory(RTLIL::IdString name, स्थिर RTLIL::Memory *other);

	RTLIL::Process *addProcess(RTLIL::IdString name);
	YS_NAME_FWD(addProcess)
	RTLIL::Process *addProcess(RTLIL::IdString name, स्थिर RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* addNot     (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addPos     (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addBuf     (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addBarrier (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addNeg     (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");

	RTLIL::Cell* addAnd  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addOr   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addXor  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addXnor (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");

	RTLIL::Cell* addReduceAnd  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addReduceOr   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addReduceXor  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addReduceXnor (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addReduceBool (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");

	RTLIL::Cell* addShl    (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addShr    (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addSshl   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addSshr   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addShift  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addShiftx (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");

	RTLIL::Cell* addLt  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addLe  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addEq  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addNe  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addEqx (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addNex (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addGe  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addGt  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");

	RTLIL::Cell* addAdd (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addSub (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addMul (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	// truncating division
	RTLIL::Cell* addDiv (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	// truncating modulo
	RTLIL::Cell* addMod (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addDivFloor (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addModFloor (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addPow (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, स्थिर std::string &src = "");

	RTLIL::Cell* addFa (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_c, स्थिर RTLIL::SigSpec &sig_x, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src = "");

	RTLIL::Cell* addLogicNot (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addLogicAnd (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::Cell* addLogicOr  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिर std::string &src = "");

	RTLIL::Cell* addMux  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_s, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addPmux (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_s, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addBmux (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_s, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addDemux (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_s, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src = "");

	RTLIL::Cell* addBweqx  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addBwmux  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_s, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src = "");

	RTLIL::Cell* addSlice  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, RTLIL::Const offset, स्थिर std::string &src = "");
	RTLIL::Cell* addConcat (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addLut    (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_y, RTLIL::Const lut, स्थिर std::string &src = "");
	RTLIL::Cell* addTribuf (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addAssert (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_en, स्थिर std::string &src = "");
	RTLIL::Cell* addAssume (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_en, स्थिर std::string &src = "");
	RTLIL::Cell* addLive   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_en, स्थिर std::string &src = "");
	RTLIL::Cell* addFair   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_en, स्थिर std::string &src = "");
	RTLIL::Cell* addCover  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_en, स्थिर std::string &src = "");
	RTLIL::Cell* addEquiv  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src = "");

	RTLIL::Cell* addSr    (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr, स्थिर RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addFf    (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, स्थिर std::string &src = "");
	RTLIL::Cell* addDff   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_d,   स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addDffe  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en,  स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addDffsr (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addDffsre (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addAdff (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_arst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, RTLIL::Const arst_मूल्य, bool clk_polarity = true, bool arst_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addAdffe (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_arst,  स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, RTLIL::Const arst_मूल्य, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addAldff (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_aload, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, स्थिर RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addAldffe (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_aload,  स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, स्थिर RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addSdff (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_srst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, RTLIL::Const srst_मूल्य, bool clk_polarity = true, bool srst_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addSdffe (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_srst,  स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, RTLIL::Const srst_मूल्य, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addSdffce (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_srst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, RTLIL::Const srst_मूल्य, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addDlatch (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, bool en_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addAdlatch (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_arst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, RTLIL::Const arst_मूल्य, bool en_polarity = true, bool arst_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addDlatchsr (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, स्थिर RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, स्थिर std::string &src = "");

	RTLIL::Cell* addBufGate    (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addNotGate    (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addAndGate    (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addNandGate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addOrGate     (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addNorGate    (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addXorGate    (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addXnorGate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addAndnotGate (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addOrnotGate  (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addMuxGate    (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_s, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addNmuxGate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_s, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addAoi3Gate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_c, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addOai3Gate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_c, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addAoi4Gate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_c, स्थिर RTLIL::SigBit &sig_d, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");
	RTLIL::Cell* addOai4Gate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_c, स्थिर RTLIL::SigBit &sig_d, स्थिर RTLIL::SigBit &sig_y, स्थिर std::string &src = "");

	RTLIL::Cell* addSrGate     (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr,
			स्थिर RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addFfGate     (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, स्थिर std::string &src = "");
	RTLIL::Cell* addDffGate    (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addDffeGate   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addDffsrGate  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addDffsreGate (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, स्थिर RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addAdffGate   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_arst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
			bool arst_मूल्य = false, bool clk_polarity = true, bool arst_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addAdffeGate  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_arst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
			bool arst_मूल्य = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addAldffGate   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_aload, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
			स्थिर RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addAldffeGate  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_aload, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
			स्थिर RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addSdffGate   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_srst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
			bool srst_मूल्य = false, bool clk_polarity = true, bool srst_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addSdffeGate  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_srst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
			bool srst_मूल्य = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addSdffceGate (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_clk, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_srst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
			bool srst_मूल्य = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addDlatchGate (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, bool en_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addAdlatchGate(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_arst, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q,
			bool arst_मूल्य = false, bool en_polarity = true, bool arst_polarity = true, स्थिर std::string &src = "");
	RTLIL::Cell* addDlatchsrGate  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_en, स्थिर RTLIL::SigSpec &sig_set, स्थिर RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, स्थिर RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, स्थिर std::string &src = "");

	RTLIL::Cell* addAnyinit(RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_d, स्थिर RTLIL::SigSpec &sig_q, स्थिर std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिर std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिर std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Shift  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");

	RTLIL::SigSpec Add (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, स्थिर std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिर std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_s, स्थिर std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_s, स्थिर std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_s, स्थिर std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_s, स्थिर std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_b, स्थिर RTLIL::SigSpec &sig_s, स्थिर std::string &src = "");

	RTLIL::SigBit BufGate    (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर std::string &src = "");
	RTLIL::SigBit NotGate    (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर std::string &src = "");
	RTLIL::SigBit AndGate    (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर std::string &src = "");
	RTLIL::SigBit NandGate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर std::string &src = "");
	RTLIL::SigBit OrGate     (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर std::string &src = "");
	RTLIL::SigBit NorGate    (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर std::string &src = "");
	RTLIL::SigBit XorGate    (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर std::string &src = "");
	RTLIL::SigBit XnorGate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर std::string &src = "");
	RTLIL::SigBit AndnotGate (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर std::string &src = "");
	RTLIL::SigBit OrnotGate  (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर std::string &src = "");
	RTLIL::SigBit MuxGate    (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_s, स्थिर std::string &src = "");
	RTLIL::SigBit NmuxGate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_s, स्थिर std::string &src = "");
	RTLIL::SigBit Aoi3Gate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_c, स्थिर std::string &src = "");
	RTLIL::SigBit Oai3Gate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_c, स्थिर std::string &src = "");
	RTLIL::SigBit Aoi4Gate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_c, स्थिर RTLIL::SigBit &sig_d, स्थिर std::string &src = "");
	RTLIL::SigBit Oai4Gate   (RTLIL::IdString name, स्थिर RTLIL::SigBit &sig_a, स्थिर RTLIL::SigBit &sig_b, स्थिर RTLIL::SigBit &sig_c, स्थिर RTLIL::SigBit &sig_d, स्थिर std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, पूर्णांक width = 1, स्थिर std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, पूर्णांक width = 1, स्थिर std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, पूर्णांक width = 1, स्थिर std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, पूर्णांक width = 1, स्थिर std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, स्थिर std::string &src = "");

	RTLIL::SigSpec SetTag          (RTLIL::IdString name, स्थिर std::string &tag, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_s, स्थिर RTLIL::SigSpec &sig_c, स्थिर std::string &src = "");
	RTLIL::Cell*   addSetTag       (RTLIL::IdString name, स्थिर std::string &tag, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_s, स्थिर RTLIL::SigSpec &sig_c, स्थिर RTLIL::SigSpec &sig_y, स्थिर std::string &src = "");
	RTLIL::SigSpec GetTag          (RTLIL::IdString name, स्थिर std::string &tag, स्थिर RTLIL::SigSpec &sig_a, स्थिर std::string &src = "");
	RTLIL::Cell*   addOverwriteTag (RTLIL::IdString name, स्थिर std::string &tag, स्थिर RTLIL::SigSpec &sig_a, स्थिर RTLIL::SigSpec &sig_s, स्थिर RTLIL::SigSpec &sig_c, स्थिर std::string &src = "");
	RTLIL::SigSpec OriginalTag     (RTLIL::IdString name, स्थिर std::string &tag, स्थिर RTLIL::SigSpec &sig_a, स्थिर std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, स्थिर RTLIL::SigSpec &sig_e, स्थिर std::string &src = "");
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

	std::string to_rtlil_str() स्थिर;
#अगर_घोषित YOSYS_ENABLE_PYTHON
	अटल std::map<अचिन्हित पूर्णांक, RTLIL::Module*> *get_all_modules(व्योम);
#पूर्ण_अगर
};

काष्ठा RTLIL::Wire : खुला RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर { h.eat(hashidx_); वापस h; }

रक्षित:
	// use module->addWire() and module->remove() to create or destroy wires
	मित्र काष्ठा RTLIL::Module;
	Wire();
	~Wire();

	मित्र काष्ठा RTLIL::Design;
	मित्र काष्ठा RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driverPort_;

खुला:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	व्योम चालक=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	पूर्णांक width, start_offset, port_id;
	bool port_निवेश, port_output, upto, is_signed;

	RTLIL::Design *design() स्थिर { वापस module ? module->design : nullptr; }
	TwinePool &twines() स्थिर { वापस module->twines(); }

	bool known_driver() स्थिर { वापस driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() स्थिर    { log_assert(driverCell_); वापस driverCell_; };
	RTLIL::IdString driverPort() स्थिर { log_assert(driverCell_); वापस driverPort_; };

	पूर्णांक from_hdl_सूचकांक(पूर्णांक hdl_सूचकांक) {
		पूर्णांक zero_सूचकांक = hdl_सूचकांक - start_offset;
		पूर्णांक rtlil_सूचकांक = upto ? width - 1 - zero_सूचकांक : zero_सूचकांक;
		वापस rtlil_सूचकांक >= 0 && rtlil_सूचकांक < width ? rtlil_सूचकांक : पूर्णांक_न्यून;
	}

	पूर्णांक to_hdl_सूचकांक(पूर्णांक rtlil_सूचकांक) {
		अगर (rtlil_सूचकांक < 0 || rtlil_सूचकांक >= width)
			वापस पूर्णांक_न्यून;
		पूर्णांक zero_सूचकांक = upto ? width - 1 - rtlil_सूचकांक : rtlil_सूचकांक;
		वापस zero_सूचकांक + start_offset;
	}

	std::string to_rtlil_str() स्थिर;

#अगर_घोषित YOSYS_ENABLE_PYTHON
	अटल std::map<अचिन्हित पूर्णांक, RTLIL::Wire*> *get_all_wires(व्योम);
#पूर्ण_अगर
};

अंतरभूत पूर्णांक Get_आकार(RTLIL::Wire *wire) {
	वापस wire->width;
}

काष्ठा RTLIL::Memory : खुला RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर { h.eat(hashidx_); वापस h; }

	Memory();
	~Memory();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() स्थिर { वापस module ? module->design : nullptr; }
	TwinePool &twines() स्थिर { वापस module->twines(); }

	RTLIL::MemoryNameId name;

	पूर्णांक width, start_offset, आकार;
#अगर_घोषित YOSYS_ENABLE_PYTHON
	अटल std::map<अचिन्हित पूर्णांक, RTLIL::Memory*> *get_all_memorys(व्योम);
#पूर्ण_अगर

	std::string to_rtlil_str() स्थिर;
};

काष्ठा RTLIL::Cell : खुला RTLIL::AttrObject
{
निजी:
	व्योम init_सूचकांक();

	bool bufnorm_handle_setPort(IdString portname, RTLIL::SigSpec &संकेत, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
खुला:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर { h.eat(hashidx_); वापस h; }

रक्षित:
	// use module->addCell() and module->remove() to create or destroy cells
	मित्र काष्ठा RTLIL::Module;
	Cell();
	~Cell();

खुला:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	व्योम चालक=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() स्थिर { वापस module ? module->design : nullptr; }
	TwinePool &twines() स्थिर { वापस module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool hasPort(RTLIL::IdString portname) स्थिर;
	व्योम unsetPort(RTLIL::IdString portname);
	व्योम setPort(RTLIL::IdString portname, RTLIL::SigSpec संकेत);
	ढाँचा<typename N, YS_NAME_STRING(N)> व्योम setPort(N portname, RTLIL::SigSpec संकेत)
		{ setPort(module->twines().add(std::move(portname)), std::move(संकेत)); }
	स्थिर RTLIL::SigSpec &getPort(RTLIL::IdString portname) स्थिर;
	स्थिर dict<RTLIL::IdString, RTLIL::SigSpec> &connections() स्थिर;

	bool known() स्थिर;
	bool निवेश(RTLIL::IdString portname) स्थिर;
	bool output(RTLIL::IdString portname) स्थिर;
	PortDir port_dir(RTLIL::IdString portname) स्थिर;

	bool hasParam(RTLIL::IdString paramname) स्थिर;
	व्योम unsetParam(RTLIL::IdString paramname);
	व्योम setParam(RTLIL::IdString paramname, RTLIL::Const मूल्य);
	स्थिर RTLIL::Const &getParam(RTLIL::IdString paramname) स्थिर;

	ढाँचा<typename N, YS_NAME_STRING(N)> bool hasParam(N name) स्थिर
		{ वापस hasParam(module->twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> व्योम unsetParam(N name)
		{ unsetParam(module->twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> व्योम setParam(N name, RTLIL::Const मूल्य)
		{ setParam(module->twines().add(std::move(name)), std::move(मूल्य)); }
	ढाँचा<typename N, YS_NAME_STRING(N)> स्थिर RTLIL::Const &getParam(N name) स्थिर
		{ वापस getParam(module->twines().find(std::string_view(name))); }

	व्योम sort();
	व्योम check();
	व्योम fixup_parameters(bool set_a_signed = false, bool set_b_signed = false);

	bool has_keep_attr() स्थिर;

	ढाँचा<typename T> व्योम rewrite_sigspecs(T &functor);
	ढाँचा<typename T> व्योम rewrite_sigspecs2(T &functor);

#अगर_घोषित YOSYS_ENABLE_PYTHON
	अटल std::map<अचिन्हित पूर्णांक, RTLIL::Cell*> *get_all_cells(व्योम);
#पूर्ण_अगर

	bool has_memid() स्थिर;
	bool is_mem_cell() स्थिर;
	bool is_builtin_ff() स्थिर;

	std::string to_rtlil_str() स्थिर;
};

काष्ठा RTLIL::CaseRule : खुला RTLIL::AttrObject
{
	std::vector<RTLIL::SigSpec> compare;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::SwitchRule*> switches;

	~CaseRule();

	bool empty() स्थिर;

	ढाँचा<typename T> व्योम rewrite_sigspecs(T &functor);
	ढाँचा<typename T> व्योम rewrite_sigspecs2(T &functor);
	RTLIL::CaseRule *clone() स्थिर;
};

काष्ठा RTLIL::SwitchRule : खुला RTLIL::AttrObject
{
	RTLIL::SigSpec संकेत;
	std::vector<RTLIL::CaseRule*> cases;

	~SwitchRule();

	bool empty() स्थिर;

	ढाँचा<typename T> व्योम rewrite_sigspecs(T &functor);
	ढाँचा<typename T> व्योम rewrite_sigspecs2(T &functor);
	RTLIL::SwitchRule *clone() स्थिर;
};

काष्ठा RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec address;
	RTLIL::SigSpec तथ्य;
	RTLIL::SigSpec enable;
	RTLIL::Const priority_mask;
};

काष्ठा RTLIL::SyncRule
{
	RTLIL::SyncType type;
	RTLIL::SigSpec संकेत;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::MemWriteAction> mem_write_actions;

	ढाँचा<typename T> व्योम rewrite_sigspecs(T &functor);
	ढाँचा<typename T> व्योम rewrite_sigspecs2(T &functor);
	RTLIL::SyncRule *clone() स्थिर;
};

काष्ठा RTLIL::Process : खुला RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिर { h.eat(hashidx_); वापस h; }

रक्षित:
	// use module->addProcess() and module->remove() to create or destroy processes
	मित्र काष्ठा RTLIL::Module;
	Process();
	~Process();

खुला:
	RTLIL::Module *module;
	RTLIL::CaseRule मूल_case;
	std::vector<RTLIL::SyncRule*> syncs;

	RTLIL::Design *design() स्थिर { वापस module ? module->design : nullptr; }
	TwinePool &twines() स्थिर { वापस module->twines(); }

	ढाँचा<typename T> व्योम rewrite_sigspecs(T &functor);
	ढाँचा<typename T> व्योम rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() स्थिर;

	std::string to_rtlil_str() स्थिर;
};

अंतरभूत RTLIL::SigBit::SigBit() : wire(शून्य), तथ्य(RTLIL::State::S0) { }
अंतरभूत RTLIL::SigBit::SigBit(RTLIL::State bit) : wire(शून्य), तथ्य(bit) { }
अंतरभूत RTLIL::SigBit::SigBit(bool bit) : wire(शून्य), तथ्य(bit ? State::S1 : State::S0) { }
अंतरभूत RTLIL::SigBit::SigBit(RTLIL::Wire *wire) : wire(wire), offset(0) { log_assert(wire && wire->width == 1); }
अंतरभूत RTLIL::SigBit::SigBit(RTLIL::Wire *wire, पूर्णांक offset) : wire(wire), offset(offset) { log_assert(wire != nullptr); }
अंतरभूत RTLIL::SigBit::SigBit(स्थिर RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.width == 1); अगर (wire) offset = chunk.offset; अन्यथा तथ्य = chunk.तथ्य[0]; }
अंतरभूत RTLIL::SigBit::SigBit(स्थिर RTLIL::SigChunk &chunk, पूर्णांक सूचकांक) : wire(chunk.wire) { अगर (wire) offset = chunk.offset + सूचकांक; अन्यथा तथ्य = chunk.तथ्य[सूचकांक]; }

अंतरभूत bool RTLIL::SigBit::चालक<(स्थिर RTLIL::SigBit &other) स्थिर {
	अगर (wire == other.wire)
		वापस wire ? (offset < other.offset) : (तथ्य < other.तथ्य);
	अगर (wire != nullptr && other.wire != nullptr)
		वापस wire->name < other.wire->name;
	वापस (wire != nullptr) < (other.wire != nullptr);
}

अंतरभूत bool RTLIL::SigBit::चालक==(स्थिर RTLIL::SigBit &other) स्थिर {
	वापस (wire == other.wire) && (wire ? (offset == other.offset) : (तथ्य == other.तथ्य));
}

अंतरभूत bool RTLIL::SigBit::चालक!=(स्थिर RTLIL::SigBit &other) स्थिर {
	वापस (wire != other.wire) || (wire ? (offset != other.offset) : (तथ्य != other.तथ्य));
}

अंतरभूत Hasher RTLIL::SigBit::hash_into(Hasher h) स्थिर {
	अगर (wire) {
		h.eat(offset);
		h.eat(wire->name);
		वापस h;
	}
	h.eat(तथ्य);
	वापस h;
}


अंतरभूत Hasher RTLIL::SigBit::hash_शीर्ष() स्थिर {
	Hasher h;
	अगर (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.bits();
		h.force(hashlib::legacy::djb2_add(n, offset));
		वापस h;
	}
	h.force(तथ्य);
	वापस h;
}

अंतरभूत RTLIL::SigBit &RTLIL::SigSpecIterator::चालक*() स्थिर {
	वापस (*sig_p)[सूचकांक];
}

अंतरभूत स्थिर RTLIL::SigBit &RTLIL::SigSpecConstIterator::चालक*() {
	bit = (*sig_p)[सूचकांक];
	वापस bit;
}

अंतरभूत RTLIL::SigBit::SigBit(स्थिर RTLIL::SigSpec &sig) {
	log_assert(sig.आकार() == 1);
	स्वतः it = sig.chunks().begin();
	*this = SigBit(*it);
}

ढाँचा<typename T>
व्योम RTLIL::Module::rewrite_sigspecs(T &functor)
{
	क्रम (स्वतः &it : cells_)
		it.second->rewrite_sigspecs(functor);
	क्रम (स्वतः &it : processes)
		it.second->rewrite_sigspecs(functor);
	क्रम (स्वतः &it : connections_) {
		functor(it.first);
		functor(it.second);
	}
}

ढाँचा<typename T>
व्योम RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	क्रम (स्वतः &it : cells_)
		it.second->rewrite_sigspecs2(functor);
	क्रम (स्वतः &it : processes)
		it.second->rewrite_sigspecs2(functor);
	क्रम (स्वतः &it : connections_) {
		functor(it.first, it.second);
	}
}

ढाँचा<typename T>
व्योम RTLIL::Cell::rewrite_sigspecs(T &functor) {
	क्रम (स्वतः &it : connections_)
		functor(it.second);
}

ढाँचा<typename T>
व्योम RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	क्रम (स्वतः &it : connections_)
		functor(it.second);
}

ढाँचा<typename T>
व्योम RTLIL::CaseRule::rewrite_sigspecs(T &functor) {
	क्रम (स्वतः &it : compare)
		functor(it);
	क्रम (स्वतः &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	क्रम (स्वतः it : switches)
		it->rewrite_sigspecs(functor);
}

ढाँचा<typename T>
व्योम RTLIL::CaseRule::rewrite_sigspecs2(T &functor) {
	क्रम (स्वतः &it : compare)
		functor(it);
	क्रम (स्वतः &it : actions) {
		functor(it.first, it.second);
	}
	क्रम (स्वतः it : switches)
		it->rewrite_sigspecs2(functor);
}

ढाँचा<typename T>
व्योम RTLIL::SwitchRule::rewrite_sigspecs(T &functor)
{
	functor(संकेत);
	क्रम (स्वतः it : cases)
		it->rewrite_sigspecs(functor);
}

ढाँचा<typename T>
व्योम RTLIL::SwitchRule::rewrite_sigspecs2(T &functor)
{
	functor(संकेत);
	क्रम (स्वतः it : cases)
		it->rewrite_sigspecs2(functor);
}

ढाँचा<typename T>
व्योम RTLIL::SyncRule::rewrite_sigspecs(T &functor)
{
	functor(संकेत);
	क्रम (स्वतः &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	क्रम (स्वतः &it : mem_write_actions) {
		functor(it.address);
		functor(it.तथ्य);
		functor(it.enable);
	}
}

ढाँचा<typename T>
व्योम RTLIL::SyncRule::rewrite_sigspecs2(T &functor)
{
	functor(संकेत);
	क्रम (स्वतः &it : actions) {
		functor(it.first, it.second);
	}
	क्रम (स्वतः &it : mem_write_actions) {
		functor(it.address);
		functor(it.तथ्य);
		functor(it.enable);
	}
}

ढाँचा<typename T>
व्योम RTLIL::Process::rewrite_sigspecs(T &functor)
{
	मूल_case.rewrite_sigspecs(functor);
	क्रम (स्वतः it : syncs)
		it->rewrite_sigspecs(functor);
}

ढाँचा<typename T>
व्योम RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	मूल_case.rewrite_sigspecs2(functor);
	क्रम (स्वतः it : syncs)
		it->rewrite_sigspecs2(functor);
}

#समावेश "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_END

#पूर्ण_अगर
