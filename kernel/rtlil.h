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

#ifndef RTLIL_H
#define RTLIL_H

#अन्तर्भाव "kernel/yosys_common.h"
#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/twine.h"

#अन्तर्भाव <deque>
#अन्तर्भाव <string_view>
#अन्तर्भाव <unordered_map>

YOSYS_NAMESPACE_BEGIN

namespace RTLIL
{
	गणना State : अचिन्हित अक्षर {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	गणना SyncType : अचिन्हित अक्षर {
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
	गणना ConstFlags : अचिन्हित अक्षर {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	गणना SelectPartials : अचिन्हित अक्षर {
		SELECT_ALL = 0,          // include partial modules
		SELECT_WHOLE_ONLY = 1,   // ignore partial modules
		SELECT_WHOLE_WARN = 2,   // call log_warning on partial module
		SELECT_WHOLE_ERR = 3,    // call log_error on partial module
		SELECT_WHOLE_CMDERR = 4  // call log_cmd_error on partial module
	};

	गणना SelectBoxes : अचिन्हित अक्षर {
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

	गणना PortDir : अचिन्हित अक्षर  {
		PD_UNKNOWN = 0,
		PD_आदान = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr पूर्णांक WIDTH_LIMIT = 1 << 30;

	संरचना Const;
	संरचना AttrObject;
	संरचना SelectedMember;
	संरचना Selection;
	संरचना Monitor;
	संरचना Design;
	संरचना Module;
	संरचना Wire;
	संरचना Memory;
	संरचना Cell;
	संरचना SigChunk;
	संरचना SigBit;
	संरचना SigSpecIterator;
	संरचना SigSpecConstIterator;
	संरचना SigSpec;
	संरचना CaseRule;
	संरचना SwitchRule;
	संरचना MemWriteAction;
	संरचना SyncRule;
	संरचना Process;

	प्रकारव्याख्या std::pair<SigSpec, SigSpec> SigSig;
};

संरचना SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_PREFIX ID; }
namespace RTLIL { using YOSYS_NAMESPACE_PREFIX IdString; }

namespace RTLIL {
	शून्य copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			स्थिरांक dict<IdString, RTLIL::Const> &src,
			स्थिरांक RTLIL::Design *src_design, RTLIL::Design *dst_design);

	बाह्य dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	स्थिरांक pool<IdString> &builtin_ff_cell_types();

	स्थिर ओळीत std::string escape_id(स्थिरांक std::string &str) {
		जर (str.size() > 0 && str[0] != '\\' && str[0] != '$')
			परत "\\" + str;
		परत str;
	}

	स्थिर ओळीत std::string unescape_id(स्थिरांक std::string &str) {
		जर (str.size() < 2)
			परत str;
		जर (str[0] != '\\')
			परत str;
		जर (str[1] == '$' || str[1] == '\\')
			परत str;
		जर (str[1] >= '0' && str[1] <= '9')
			परत str;
		परत str.substr(1);
	}

	ढाँचा <typename T> using sort_by_name_id = IdString::compare_ptr_by_name<T>;

	ढाँचा <typename T> संरचना sort_by_name_str {
		bool चालक()(T *a, T *b) स्थिरांक {
			परत a->name.lt_by_name(b->name);
		}
	};

	संरचना sort_by_id_str {
		स्थिरांक TwinePool& pool;
		explicit sort_by_id_str(स्थिरांक TwinePool& pool) : pool(pool) {}
		bool चालक()(RTLIL::IdString a, RTLIL::IdString b) स्थिरांक {
			परत pool.compare_by_name(a, b) < 0;
		}
	};

	स्थिर ओळीत std::string encode_filename(स्थिरांक std::string &filename)
	{
		std::stringstream val;
		जर (!std::any_of(filename.begin(), filename.end(), [](अक्षर c) {
			परत static_cast<अचिन्हित अक्षर>(c) < 33 || static_cast<अचिन्हित अक्षर>(c) > 126;
		})) परत filename;
		साठी (अचिन्हित अक्षर स्थिरांक c : filename) {
			जर (c < 33 || c > 126)
				val << stringf("$%02x", c);
			अन्यथा
				val << c;
		}
		परत val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_and         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_or          (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_xor         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_xnor        (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);

	RTLIL::Const const_reduce_and  (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_reduce_or   (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_reduce_xor  (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_reduce_xnor (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_reduce_bool (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);

	RTLIL::Const const_logic_not   (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_logic_and   (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_logic_or    (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);

	RTLIL::Const const_shl         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_shr         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_sshl        (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_sshr        (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_shift       (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_shiftx      (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);

	RTLIL::Const const_lt          (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_le          (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_eq          (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_ne          (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_eqx         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_nex         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_ge          (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_gt          (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);

	RTLIL::Const const_add         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_sub         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_mul         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_div         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_divfloor    (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_modfloor    (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_mod         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_pow         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);

	RTLIL::Const const_pos         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_buf         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);
	RTLIL::Const const_neg         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक result_len);

	RTLIL::Const const_mux         (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, स्थिरांक RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, स्थिरांक RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2);
	RTLIL::Const const_demux       (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, स्थिरांक RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	ढाँचा<typename T, typename किल्ली_2 = IdString>
	संरचना ObjIterator {
		using iterator_category = std::forward_iterator_tag;
		using मूल्य_type = T;
		using difference_type = सूचक_भेद_प्रकार;
		using pointer = T*;
		using reference = T&;
		typename dict<किल्ली_2, T>::iterator it;
		dict<किल्ली_2, T> *list_p;
		पूर्णांक *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, पूर्णांक *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			जर (list_p->empty()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} अन्यथा {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(स्थिरांक RTLIL::ObjIterator<T, किल्ली_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			जर (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &चालक=(स्थिरांक RTLIL::ObjIterator<T, किल्ली_2> &other) {
			जर (refcount_p)
				(*refcount_p)--;
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			जर (refcount_p)
				(*refcount_p)++;
			परत *this;
		}

		~ObjIterator() {
			जर (refcount_p)
				(*refcount_p)--;
		}

		ओळीत T चालक*() स्थिरांक {
			log_assert(list_p != nullptr);
			परत it->second;
		}

		ओळीत bool चालक!=(स्थिरांक RTLIL::ObjIterator<T, किल्ली_2> &other) स्थिरांक {
			जर (list_p == nullptr || other.list_p == nullptr)
				परत list_p != other.list_p;
			परत it != other.it;
		}


		ओळीत bool चालक==(स्थिरांक RTLIL::ObjIterator<T, किल्ली_2> &other) स्थिरांक {
			परत !(*this != other);
		}

		ओळीत ObjIterator<T, किल्ली_2>& चालक++() {
			log_assert(list_p != nullptr);
			जर (++it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			परत *this;
		}

		ओळीत ObjIterator<T, किल्ली_2>& चालक+=(पूर्णांक amt) {
			log_assert(list_p != nullptr);
			it += amt;
			जर (it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			परत *this;
		}

		ओळीत ObjIterator<T, किल्ली_2> चालक+(पूर्णांक amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, किल्ली_2> new_obj(*this);
			new_obj.it += amt;
			जर (new_obj.it == list_p->end()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			परत new_obj;
		}

		ओळीत स्थिरांक ObjIterator<T, किल्ली_2> चालक++(पूर्णांक) {
			ObjIterator<T, किल्ली_2> result(*this);
			++(*this);
			परत result;
		}
	};

	ढाँचा<typename T, typename किल्ली_2 = IdString>
	संरचना ObjRange
	{
		dict<किल्ली_2, T> *list_p;
		पूर्णांक *refcount_p;

		ObjRange(decltype(list_p) list_p, पूर्णांक *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, किल्ली_2> begin() { परत RTLIL::ObjIterator<T, किल्ली_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, किल्ली_2> end() { परत RTLIL::ObjIterator<T, किल्ली_2>(); }

		माप_प्रकार size() स्थिरांक {
			परत list_p->size();
		}

		चालक pool<T>() स्थिरांक {
			pool<T> result;
			साठी (स्वतः &it : *list_p)
				result.घाला(it.second);
			परत result;
		}

		चालक std::vector<T>() स्थिरांक {
			std::vector<T> result;
			result.reserve(list_p->size());
			साठी (स्वतः &it : *list_p)
				result.ढकला_back(it.second);
			परत result;
		}

		pool<T> to_pool() स्थिरांक { परत *this; }
		std::vector<T> to_vector() स्थिरांक { परत *this; }
	};
};

संरचना RTLIL::Const
{
	लघु पूर्णांक flags;
खाजगी:
	मित्र class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::vector<RTLIL::State>;
	गणना class backing_tag: bool { bits, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_tag tag;
	संघ {
		bitvectype bits_;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_bits() स्थिरांक { परत tag == backing_tag::bits; }
	bool is_str() स्थिरांक { परत tag == backing_tag::string; }

	bitvectype* get_if_bits() { परत is_bits() ? &bits_ : रिक्त; }
	std::string* get_if_str() { परत is_str() ? &str_ : रिक्त; }
	स्थिरांक bitvectype* get_if_bits() स्थिरांक { परत is_bits() ? &bits_ : रिक्त; }
	स्थिरांक std::string* get_if_str() स्थिरांक { परत is_str() ? &str_ : रिक्त; }

	bitvectype& get_bits();
	std::string& get_str();
	स्थिरांक bitvectype& get_bits() स्थिरांक;
	स्थिरांक std::string& get_str() स्थिरांक;
	std::vector<RTLIL::State>& bits_internal();
	शून्य bitvectorize_internal();

सार्वजनिक:
	Const() : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::vector<RTLIL::State>()) {}
	Const(std::string str);
	Const(दीर्घ दीर्घ val); // default width is 32
	Const(दीर्घ दीर्घ val, पूर्णांक width);
	Const(RTLIL::State bit, पूर्णांक width = 1);
	Const(std::vector<RTLIL::State> bits) : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::move(bits)) {}
	Const(स्थिरांक std::vector<bool> &bits);
	Const(स्थिरांक RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &चालक =(स्थिरांक RTLIL::Const &other);
	~Const();

	संरचना Builder
	{
		Builder() {}
		Builder(पूर्णांक expected_width) { bits.reserve(expected_width); }
		शून्य ढकला_back(RTLIL::State b) { bits.ढकला_back(b); }
		पूर्णांक size() स्थिरांक { परत static_cast<पूर्णांक>(bits.size()); }
		Const build() { परत Const(std::move(bits)); }
	खाजगी:
		std::vector<RTLIL::State> bits;
	};

	bool चालक <(स्थिरांक RTLIL::Const &other) स्थिरांक;
	bool चालक ==(स्थिरांक RTLIL::Const &other) स्थिरांक;
	bool चालक !=(स्थिरांक RTLIL::Const &other) स्थिरांक;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::vector<RTLIL::State>& bits() { परत bits_internal(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	शून्य bitvectorize() स्थिरांक { const_cast<Const*>(this)->bitvectorize_internal(); }

	bool as_bool() स्थिरांक;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	पूर्णांक as_int(bool is_signed = false) स्थिरांक;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_to_int(bool is_signed = false) स्थिरांक;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<पूर्णांक> try_as_int(bool is_signed = false) स्थिरांक;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	पूर्णांक as_int_saturating(bool is_signed = false) स्थिरांक;

	शून्य tag_bare_integer_const(स्थिरांक std::string &मूल्य);

	std::string as_string(स्थिरांक अक्षर* any = "-") स्थिरांक;
	स्थिर Const from_string(स्थिरांक std::string &str);
	std::vector<RTLIL::State> to_bits() स्थिरांक;

	std::string decode_string() स्थिरांक;
	पूर्णांक size() स्थिरांक;
	bool empty() स्थिरांक;

	शून्य append(स्थिरांक RTLIL::Const &other);
	शून्य set(पूर्णांक i, RTLIL::State state) {
		bits_internal()[i] = state;
	}
	शून्य resize(पूर्णांक size, RTLIL::State fill) {
    log_assert(size >= 0 && size < RTLIL::WIDTH_LIMIT);
		bits_internal().resize(size, fill);
	}

	class const_iterator {
	खाजगी:
		स्थिरांक Const* parent;
		माप_प्रकार idx;

	सार्वजनिक:
		using iterator_category = std::bidirectional_iterator_tag;
		using मूल्य_type = State;
		using difference_type = std::सूचक_भेद_प्रकार;
		using pointer = स्थिरांक State*;
		using reference = स्थिरांक State&;

		const_iterator(स्थिरांक Const& c, माप_प्रकार i) : parent(&c), idx(i) {}

		State चालक*() स्थिरांक;

		const_iterator& चालक++() { ++idx; परत *this; }
		const_iterator& चालक--() { --idx; परत *this; }
		const_iterator चालक++(पूर्णांक) { const_iterator result(*this); ++idx; परत result; }
		const_iterator चालक--(पूर्णांक) { const_iterator result(*this); --idx; परत result; }
		const_iterator& चालक+=(पूर्णांक i) { idx += i; परत *this; }

		const_iterator चालक+(पूर्णांक add) {
			परत const_iterator(*parent, idx + add);
		}
		const_iterator चालक-(पूर्णांक sub) {
			परत const_iterator(*parent, idx - sub);
		}
		पूर्णांक चालक-(स्थिरांक const_iterator& other) {
			परत idx - other.idx;
		}

		bool चालक==(स्थिरांक const_iterator& other) स्थिरांक {
			परत idx == other.idx;
		}

		bool चालक!=(स्थिरांक const_iterator& other) स्थिरांक {
			परत !(*this == other);
		}
	};

	class iterator {
	खाजगी:
		Const* parent;
		माप_प्रकार idx;

	सार्वजनिक:
		class proxy {
		खाजगी:
			Const* parent;
			माप_प्रकार idx;
		सार्वजनिक:
			proxy(Const* parent, माप_प्रकार idx) : parent(parent), idx(idx) {}
			चालक State() स्थिरांक { परत (*parent)[idx]; }
			proxy& चालक=(State s) { parent->set(idx, s); परत *this; }
			proxy& चालक=(स्थिरांक proxy& other) { parent->set(idx, (*other.parent)[other.idx]); परत *this; }
		};

		using iterator_category = std::bidirectional_iterator_tag;
		using मूल्य_type = State;
		using difference_type = std::सूचक_भेद_प्रकार;
		using pointer = proxy*;
		using reference = proxy;

		iterator(Const& c, माप_प्रकार i) : parent(&c), idx(i) {}

		proxy चालक*() स्थिरांक { परत proxy(parent, idx); }
		iterator& चालक++() { ++idx; परत *this; }
		iterator& चालक--() { --idx; परत *this; }
		iterator चालक++(पूर्णांक) { iterator result(*this); ++idx; परत result; }
		iterator चालक--(पूर्णांक) { iterator result(*this); --idx; परत result; }
		iterator& चालक+=(पूर्णांक i) { idx += i; परत *this; }

		iterator चालक+(पूर्णांक add) {
			परत iterator(*parent, idx + add);
		}
		iterator चालक-(पूर्णांक sub) {
			परत iterator(*parent, idx - sub);
		}
		पूर्णांक चालक-(स्थिरांक iterator& other) {
			परत idx - other.idx;
		}

		bool चालक==(स्थिरांक iterator& other) स्थिरांक {
			परत idx == other.idx;
		}

		bool चालक!=(स्थिरांक iterator& other) स्थिरांक {
			परत !(*this == other);
		}
	};

	const_iterator begin() स्थिरांक {
		परत const_iterator(*this, 0);
	}
	const_iterator end() स्थिरांक {
		परत const_iterator(*this, size());
	}
	iterator begin() {
		परत iterator(*this, 0);
	}
	iterator end() {
		परत iterator(*this, size());
	}
	State back() स्थिरांक {
		परत *(end() - 1);
	}
	State front() स्थिरांक {
		परत *begin();
	}
	State at(माप_प्रकार i) स्थिरांक {
		परत *const_iterator(*this, i);
	}
	State चालक[](माप_प्रकार i) स्थिरांक {
		परत *const_iterator(*this, i);
	}

	bool is_fully_zero() स्थिरांक;
	bool is_fully_ones() स्थिरांक;
	bool is_fully_def() स्थिरांक;
	bool is_fully_undef() स्थिरांक;
	bool is_fully_undef_x_only() स्थिरांक;
	bool is_onehot(पूर्णांक *pos = nullptr) स्थिरांक;

	RTLIL::Const extract(पूर्णांक offset, पूर्णांक len = 1, RTLIL::State padding = RTLIL::State::S0) स्थिरांक;

	// find the MSB without redundant leading bits
	पूर्णांक get_min_size(bool is_signed) स्थिरांक;

	// compress representation to the minimum required bits
	शून्य compress(bool is_signed = false);

	std::optional<पूर्णांक> as_int_compress(bool is_signed) स्थिरांक;

	शून्य extu(पूर्णांक width) {
		resize(width, RTLIL::State::S0);
	}

	शून्य exts(पूर्णांक width) {
		resize(width, empty() ? RTLIL::State::Sx : back());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;
};

संरचना RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	bool has_attribute(RTLIL::IdString id) स्थिरांक;

	शून्य set_bool_attribute(RTLIL::IdString id, bool मूल्य=true);
	bool get_bool_attribute(RTLIL::IdString id) स्थिरांक;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) स्थिरांक {
		परत get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	शून्य set_string_attribute(RTLIL::IdString id, string मूल्य);
	string get_string_attribute(RTLIL::IdString id) स्थिरांक;

	शून्य set_strpool_attribute(RTLIL::IdString id, स्थिरांक pool<string> &data);
	शून्य add_strpool_attribute(RTLIL::IdString id, स्थिरांक pool<string> &data);
	pool<string> get_strpool_attribute(RTLIL::IdString id) स्थिरांक;

	शून्य set_src_attribute(स्थिरांक std::string &src) {
		set_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() स्थिरांक {
		परत get_string_attribute(ID::src);
	}

	शून्य set_hdlname_attribute(स्थिरांक vector<string> &hierarchy);
	vector<string> get_hdlname_attribute() स्थिरांक;

	शून्य set_intvec_attribute(RTLIL::IdString id, स्थिरांक vector<पूर्णांक> &data);
	vector<पूर्णांक> get_intvec_attribute(RTLIL::IdString id) स्थिरांक;
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
	std::vector<RTLIL::State> data; // only used if wire == NULL, LSB at index 0
	पूर्णांक width, offset;

	SigChunk() : wire(nullptr), width(0), offset(0) {}
	SigChunk(स्थिरांक RTLIL::Const &मूल्य) : wire(nullptr), data(मूल्य.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Const &&मूल्य) : wire(nullptr), data(मूल्य.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), width(GetSize(wire)), offset(0) {}
	SigChunk(RTLIL::Wire *wire, पूर्णांक offset, पूर्णांक width = 1) : wire(wire), width(width), offset(offset) {}
	SigChunk(स्थिरांक std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(पूर्णांक val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(पूर्णांक val, पूर्णांक width) : SigChunk(RTLIL::Const(val, width)) {}
	SigChunk(RTLIL::State bit, पूर्णांक width = 1) : SigChunk(RTLIL::Const(bit, width)) {}
	SigChunk(स्थिरांक RTLIL::SigBit &bit);

	RTLIL::SigChunk extract(पूर्णांक offset, पूर्णांक length) स्थिरांक;
	RTLIL::SigBit चालक[](पूर्णांक offset) स्थिरांक;
	ओळीत पूर्णांक size() स्थिरांक { परत width; }
	ओळीत bool is_wire() स्थिरांक { परत wire != रिक्त; }

	bool चालक <(स्थिरांक RTLIL::SigChunk &other) स्थिरांक;
	bool चालक ==(स्थिरांक RTLIL::SigChunk &other) स्थिरांक;
	bool चालक !=(स्थिरांक RTLIL::SigChunk &other) स्थिरांक;
};

संरचना RTLIL::SigBit
{
	RTLIL::Wire *wire;
	संघ {
		RTLIL::State data; // used if wire == NULL
		पूर्णांक offset;        // used if wire != NULL
	};

	SigBit();
	SigBit(RTLIL::State bit);
	explicit SigBit(bool bit);
	SigBit(RTLIL::Wire *wire);
	SigBit(RTLIL::Wire *wire, पूर्णांक offset);
	SigBit(स्थिरांक RTLIL::SigChunk &chunk);
	SigBit(स्थिरांक RTLIL::SigChunk &chunk, पूर्णांक index);
	SigBit(स्थिरांक RTLIL::SigSpec &sig);
	SigBit(स्थिरांक RTLIL::SigBit &sigbit) = पूर्वनिर्धारित;
	RTLIL::SigBit &चालक =(स्थिरांक RTLIL::SigBit &other) = पूर्वनिर्धारित;

	ओळीत bool is_wire() स्थिरांक { परत wire != रिक्त; }

	bool चालक <(स्थिरांक RTLIL::SigBit &other) स्थिरांक;
	bool चालक ==(स्थिरांक RTLIL::SigBit &other) स्थिरांक;
	bool चालक !=(स्थिरांक RTLIL::SigBit &other) स्थिरांक;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक;
	[[nodiscard]] Hasher hash_शीर्ष() स्थिरांक;
};

namespace hashlib {
	ढाँचा <>
	संरचना hash_ops<RTLIL::SigBit> {
		स्थिर ओळीत bool cmp(स्थिरांक RTLIL::SigBit &a, स्थिरांक RTLIL::SigBit &b) {
			परत a == b;
		}
		[[nodiscard]] स्थिर ओळीत Hasher hash(स्थिरांक RTLIL::SigBit sb) {
			परत sb.hash_शीर्ष();
		}
		[[nodiscard]] स्थिर ओळीत Hasher hash_into(स्थिरांक RTLIL::SigBit sb, Hasher h) {
			परत sb.hash_into(h);
		}
	};
};

संरचना RTLIL::SigSpecIterator
{
	प्रकारव्याख्या std::आदान_iterator_tag iterator_category;
	प्रकारव्याख्या RTLIL::SigBit मूल्य_type;
	प्रकारव्याख्या सूचक_भेद_प्रकार difference_type;
	प्रकारव्याख्या RTLIL::SigBit* pointer;
	प्रकारव्याख्या RTLIL::SigBit& reference;

	RTLIL::SigSpec *sig_p;
	पूर्णांक index;

	ओळीत RTLIL::SigBit &चालक*() स्थिरांक;
	ओळीत bool चालक!=(स्थिरांक RTLIL::SigSpecIterator &other) स्थिरांक { परत index != other.index; }
	ओळीत bool चालक==(स्थिरांक RTLIL::SigSpecIterator &other) स्थिरांक { परत index == other.index; }
	ओळीत शून्य चालक++() { index++; }
};

संरचना RTLIL::SigSpecConstIterator
{
	प्रकारव्याख्या std::आदान_iterator_tag iterator_category;
	प्रकारव्याख्या RTLIL::SigBit मूल्य_type;
	प्रकारव्याख्या सूचक_भेद_प्रकार difference_type;
	प्रकारव्याख्या RTLIL::SigBit* pointer;
	प्रकारव्याख्या RTLIL::SigBit& reference;

	स्थिरांक RTLIL::SigSpec *sig_p;
	RTLIL::SigBit bit;
	पूर्णांक index;

	ओळीत स्थिरांक RTLIL::SigBit &चालक*();
	ओळीत bool चालक!=(स्थिरांक RTLIL::SigSpecConstIterator &other) स्थिरांक { परत index != other.index; }
	ओळीत bool चालक==(स्थिरांक RTLIL::SigSpecIterator &other) स्थिरांक { परत index == other.index; }
	ओळीत शून्य चालक++() { index++; }
};

संरचना RTLIL::SigSpec
{
खाजगी:
	मित्र class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, Extract);
	गणना Representation : अक्षर {
		CHUNK,
		BITS,
	};
	// An AtomicHash is either clear or a nonzero integer.
	संरचना AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(स्थिरांक AtomicHash &rhs) : atomic_(rhs.load()) {}
		AtomicHash &चालक=(स्थिरांक AtomicHash &rhs) { store(rhs.load()); परत *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> पठ() स्थिरांक {
			Hasher::hash_t मूल्य = load();
			जर (मूल्य == 0)
				परत std::nullopt;
			परत मूल्य;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		शून्य set(Hasher::hash_t मूल्य) स्थिरांक {
			log_assert(मूल्य != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(मूल्य, std::memory_order_relaxed);
			log_assert(old == 0 || old == मूल्य);
		}
		शून्य clear() { store(0); }
	खाजगी:
		पूर्णांक load() स्थिरांक { परत atomic_.load(std::memory_order_relaxed); }
		शून्य store(Hasher::hash_t मूल्य) स्थिरांक {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(मूल्य, std::memory_order_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	संघ {
		RTLIL::SigChunk chunk_;
		std::vector<RTLIL::SigBit> bits_; // LSB at index 0
	};

	शून्य init_empty_bits() {
		rep_ = BITS;
		new (&bits_) std::vector<RTLIL::SigBit>;
	}

	शून्य unpack();
	ओळीत शून्य inline_unpack() {
		जर (rep_ == CHUNK)
			unpack();
	}
	शून्य try_repack();

	Hasher::hash_t updhash() स्थिरांक;
	शून्य destroy() {
		जर (rep_ == CHUNK)
			chunk_.~SigChunk();
		अन्यथा
			bits_.~vector();
	}
	मित्र संरचना Chunks;

सार्वजनिक:
	SigSpec() { init_empty_bits(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(स्थिरांक SigSpec &मूल्य) : rep_(मूल्य.rep_), hash_(मूल्य.hash_) {
		जर (मूल्य.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(मूल्य.chunk_);
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(मूल्य.bits_);
	}
	SigSpec(SigSpec &&मूल्य) : rep_(मूल्य.rep_), hash_(मूल्य.hash_) {
		जर (मूल्य.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(मूल्य.chunk_));
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(मूल्य.bits_));
	}
	SigSpec(स्थिरांक RTLIL::Const &मूल्य);
	SigSpec(RTLIL::Const &&मूल्य);
	SigSpec(स्थिरांक RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, पूर्णांक offset, पूर्णांक width = 1);
	SigSpec(स्थिरांक std::string &str);
	SigSpec(पूर्णांक val, पूर्णांक width = 32);
	SigSpec(RTLIL::State bit, पूर्णांक width = 1);
	SigSpec(स्थिरांक RTLIL::SigBit &bit, पूर्णांक width = 1);
	SigSpec(स्थिरांक std::vector<RTLIL::SigChunk> &chunks);
	SigSpec(स्थिरांक std::vector<RTLIL::SigBit> &bits);
	SigSpec(स्थिरांक pool<RTLIL::SigBit> &bits);
	SigSpec(स्थिरांक std::set<RTLIL::SigBit> &bits);
	explicit SigSpec(bool bit);
	~SigSpec() {
		destroy();
	}

	SigSpec &चालक=(स्थिरांक SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		जर (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(rhs.bits_);
		परत *this;
	}
	SigSpec &चालक=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		जर (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(rhs.bits_));
		परत *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	संरचना Chunks {
		Chunks(स्थिरांक SigSpec &spec) : spec(spec) {}
		संरचना const_iterator {
			using iterator_category = std::forward_iterator_tag;
			using मूल्य_type = स्थिरांक SigChunk &;
			using difference_type = std::सूचक_भेद_प्रकार;
			using pointer = स्थिरांक SigChunk *;
			using reference = स्थिरांक SigChunk &;

			स्थिरांक SigSpec &spec;
			पूर्णांक bit_index;
			SigChunk chunk;

			const_iterator(स्थिरांक SigSpec &spec) : spec(spec) {
				bit_index = 0;
				जर (spec.rep_ == BITS)
					next_chunk_bits();
			}
			गणना End { END };
			const_iterator(स्थिरांक SigSpec &spec, End) : spec(spec) {
				bit_index = spec.size();
			}
			शून्य next_chunk_bits();

			स्थिरांक SigChunk &चालक*() {
				जर (spec.rep_ == CHUNK)
					परत spec.chunk_;
				परत chunk;
			};
			स्थिरांक SigChunk *चालक->() { परत &**this; }
			const_iterator &चालक++() {
				bit_index += (**this).width;
				जर (spec.rep_ == BITS)
					next_chunk_bits();
				परत *this;
			}
			bool चालक==(स्थिरांक const_iterator &rhs) स्थिरांक { परत bit_index == rhs.bit_index; }
			bool चालक!=(स्थिरांक const_iterator &rhs) स्थिरांक { परत !(*this == rhs); }
		};
		const_iterator begin() स्थिरांक { परत const_iterator(spec); }
		const_iterator end() स्थिरांक {
			const_iterator it(spec, const_iterator::END);
			परत it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rbegin() {
			ensure_chunk_vector();
			परत chunk_vector.rbegin();
		}
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rend() {
			ensure_chunk_vector();
			परत chunk_vector.rend();
		}
		पूर्णांक size() {
			ensure_chunk_vector();
			परत chunk_vector.size();
		}
		पूर्णांक size() स्थिरांक {
			परत std::distance(begin(), end());
		}
		स्थिरांक SigChunk &at(पूर्णांक index) {
			ensure_chunk_vector();
			परत chunk_vector.at(index);
		}
		चालक स्थिरांक std::vector<RTLIL::SigChunk>&() {
			ensure_chunk_vector();
			परत chunk_vector;
		}
	खाजगी:
		शून्य ensure_chunk_vector() {
			जर (spec.size() > 0 && chunk_vector.empty()) {
				साठी (स्थिरांक RTLIL::SigChunk &c : *this)
					chunk_vector.ढकला_back(c);
			}
		}
		स्थिरांक SigSpec &spec;
		std::vector<RTLIL::SigChunk> chunk_vector;
	};
	मित्र संरचना Chunks::const_iterator;

	ओळीत Chunks chunks() स्थिरांक { परत {*this}; }
	ओळीत स्थिरांक SigSpec &bits() स्थिरांक { परत *this; }

	ओळीत पूर्णांक size() स्थिरांक { परत rep_ == CHUNK ? chunk_.width : GetSize(bits_); }
	ओळीत bool empty() स्थिरांक { परत size() == 0; };

	ओळीत RTLIL::SigBit &चालक[](पूर्णांक index) { inline_unpack(); hash_.clear(); परत bits_.at(index); }
	ओळीत RTLIL::SigBit चालक[](पूर्णांक index) स्थिरांक {
		जर (rep_ == CHUNK) {
			जर (index < 0 || index >= chunk_.width)
				throw std::out_of_range("SigSpec::operator[]");
			जर (chunk_.wire)
				परत RTLIL::SigBit(chunk_.wire, chunk_.offset + index);
			परत RTLIL::SigBit(chunk_.data[index]);
		}
		परत bits_.at(index);
	}

	ओळीत RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = 0; परत it; }
	ओळीत RTLIL::SigSpecIterator end() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = size(); परत it; }

	ओळीत RTLIL::SigSpecConstIterator begin() स्थिरांक { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = 0; परत it; }
	ओळीत RTLIL::SigSpecConstIterator end() स्थिरांक { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = size(); परत it; }

	शून्य sort();
	शून्य sort_and_unify();

	शून्य replace(स्थिरांक RTLIL::SigSpec &pattern, स्थिरांक RTLIL::SigSpec &with);
	शून्य replace(स्थिरांक RTLIL::SigSpec &pattern, स्थिरांक RTLIL::SigSpec &with, RTLIL::SigSpec *other) स्थिरांक;

	शून्य replace(स्थिरांक dict<RTLIL::SigBit, RTLIL::SigBit> &rules);
	शून्य replace(स्थिरांक dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) स्थिरांक;

	शून्य replace(स्थिरांक std::map<RTLIL::SigBit, RTLIL::SigBit> &rules);
	शून्य replace(स्थिरांक std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) स्थिरांक;

	शून्य replace(पूर्णांक offset, स्थिरांक RTLIL::SigSpec &with);

	शून्य हटाओ(स्थिरांक RTLIL::SigSpec &pattern);
	शून्य हटाओ(स्थिरांक RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) स्थिरांक;
	शून्य remove2(स्थिरांक RTLIL::SigSpec &pattern, RTLIL::SigSpec *other);

	शून्य हटाओ(स्थिरांक pool<RTLIL::SigBit> &pattern);
	शून्य हटाओ(स्थिरांक pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) स्थिरांक;
	शून्य remove2(स्थिरांक pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	शून्य remove2(स्थिरांक std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	शून्य remove2(स्थिरांक pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other);

	शून्य हटाओ(पूर्णांक offset, पूर्णांक length = 1);
	शून्य remove_const();

	RTLIL::SigSpec extract(स्थिरांक RTLIL::SigSpec &pattern, स्थिरांक RTLIL::SigSpec *other = रिक्त) स्थिरांक;
	RTLIL::SigSpec extract(स्थिरांक pool<RTLIL::SigBit> &pattern, स्थिरांक RTLIL::SigSpec *other = रिक्त) स्थिरांक;
	RTLIL::SigSpec extract(पूर्णांक offset, पूर्णांक length = 1) स्थिरांक;
	RTLIL::SigSpec extract_end(पूर्णांक offset) स्थिरांक { परत extract(offset, size() - offset); }

	शून्य rewrite_wires(std::function<शून्य(RTLIL::Wire*& wire)> rewrite);

	RTLIL::SigBit lsb() स्थिरांक { log_assert(size()); परत (*this)[0]; };
	RTLIL::SigBit msb() स्थिरांक { log_assert(size()); परत (*this)[size() - 1]; };
	RTLIL::SigBit front() स्थिरांक { परत (*this)[0]; }
	RTLIL::SigBit back() स्थिरांक { परत (*this)[size() - 1]; }

	शून्य append(स्थिरांक RTLIL::SigSpec &संकेत);
	ओळीत शून्य append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	ओळीत शून्य append(स्थिरांक RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	ओळीत शून्य append(स्थिरांक RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	शून्य append(स्थिरांक RTLIL::SigBit &bit);
	ओळीत शून्य append(RTLIL::State state) { append(RTLIL::SigBit(state)); }
	ओळीत शून्य append(bool bool_) { append(RTLIL::SigBit(bool_)); }

	शून्य extend_u0(पूर्णांक width, bool is_signed = false);

	RTLIL::SigSpec repeat(पूर्णांक अंक) स्थिरांक;

	शून्य reverse() { inline_unpack(); std::reverse(bits_.begin(), bits_.end()); }

	bool चालक <(स्थिरांक RTLIL::SigSpec &other) स्थिरांक;
	bool चालक ==(स्थिरांक RTLIL::SigSpec &other) स्थिरांक;
	ओळीत bool चालक !=(स्थिरांक RTLIL::SigSpec &other) स्थिरांक { परत !(*this == other); }

	bool is_wire() स्थिरांक;
	bool is_chunk() स्थिरांक;
	ओळीत bool is_bit() स्थिरांक { परत size() == 1; }

	bool known_driver() स्थिरांक;

	bool is_fully_const() स्थिरांक;
	bool is_fully_zero() स्थिरांक;
	bool is_fully_ones() स्थिरांक;
	bool is_fully_def() स्थिरांक;
	bool is_fully_undef() स्थिरांक;
	bool has_const() स्थिरांक;
	bool has_const(State state) स्थिरांक;
	bool has_marked_bits() स्थिरांक;
	bool is_onehot(पूर्णांक *pos = nullptr) स्थिरांक;

	bool as_bool() स्थिरांक;

	// Convert the SigSpec to a C++ int, assuming all bits are constant.
	// NOTE: If the value is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	पूर्णांक as_int(bool is_signed = false) स्थिरांक;

	// Returns true iff the SigSpec is constant and can be converted to an int
	// without over/underflow.
	bool convertible_to_int(bool is_signed = false) स्थिरांक;

	// Returns the SigSpec's value as an int if it is a constant and can be
	// represented without over/underflow, or std::nullopt otherwise.
	std::optional<पूर्णांक> try_as_int(bool is_signed = false) स्थिरांक;

	// Returns an all constant SigSpec's value as an int if it can be represented
	// without over/underflow, otherwise the max/min value for int depending on
	// the sign.
	पूर्णांक as_int_saturating(bool is_signed = false) स्थिरांक;

	std::string as_string() स्थिरांक;
	// Returns std::nullopt if there are any non-constant bits. Returns an empty
	// Const if this has zero width.
	std::optional<RTLIL::Const> try_as_const() स्थिरांक;
	RTLIL::Const as_const() स्थिरांक;
	RTLIL::Wire *as_wire() स्थिरांक;
	RTLIL::SigChunk as_chunk() स्थिरांक;
	RTLIL::SigBit as_bit() स्थिरांक;

	bool match(स्थिरांक अक्षर* pattern) स्थिरांक;

	std::set<RTLIL::SigBit> to_sigbit_set() स्थिरांक;
	pool<RTLIL::SigBit> to_sigbit_pool() स्थिरांक;
	std::vector<RTLIL::SigBit> to_sigbit_vector() स्थिरांक;
	std::map<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_map(स्थिरांक RTLIL::SigSpec &other) स्थिरांक;
	dict<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_dict(स्थिरांक RTLIL::SigSpec &other) स्थिरांक;

	स्थिर bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	स्थिर bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	स्थिर bool parse_rhs(स्थिरांक RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	चालक std::vector<RTLIL::SigChunk>() स्थिरांक;
	चालक std::vector<RTLIL::SigBit>() स्थिरांक { परत to_sigbit_vector(); }
	स्थिरांक RTLIL::SigBit &at(पूर्णांक offset, स्थिरांक RTLIL::SigBit &defval) { परत offset < size() ? (*this)[offset] : defval; }
	RTLIL::SigBit& at(पूर्णांक offset) { परत (*this)[offset]; }
	RTLIL::SigBit at(पूर्णांक offset) स्थिरांक { परत (*this)[offset]; }

	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक {
		Hasher::hash_t val;
		जर (std::optional<Hasher::hash_t> current = hash_.पठ())
			val = *current;
		अन्यथा
			val = updhash();
		h.eat(val);
		परत h;
	}

#ifndef न_संशोधन
	शून्य check(स्थिरांक Module *mod = nullptr) स्थिरांक;
#अन्यथा
	शून्य check(स्थिरांक Module *mod = nullptr) स्थिरांक { (शून्य)mod; }
#endif
};

संरचना RTLIL::Selection
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
	bool boxed_module(RTLIL::IdString mod_name) स्थिरांक;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) स्थिरांक;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) स्थिरांक;

	// checks if the given member from the given module is included in this
	// selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) स्थिरांक;

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
	bool selects_all() स्थिरांक {
		परत full_selection || complete_selection;
	}

	// add whole module to this selection
	ढाँचा<typename T1> शून्य select(T1 *module) {
		जर (!selects_all() && selected_modules.मोजणी(module->name) == 0) {
			IdString name = module->name;
			selected_modules.घाला(name);
			selected_members.erase(name);
			जर (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	ढाँचा<typename T1, typename T2> शून्य select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	ढाँचा<typename T1> शून्य select(T1 *module, RTLIL::IdString member) {
		जर (!selects_all() && selected_modules.मोजणी(module->name.ref()) == 0) {
			selected_members[module->name.ref()].घाला(member);
			जर (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool empty() स्थिरांक {
		परत !selects_all() && selected_modules.empty() && selected_members.empty();
	}

	// clear this selection, leaving it empty
	शून्य clear();

	// create a new selection which is empty
	स्थिर Selection EmptySelection(RTLIL::Design *design = nullptr) { परत Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	स्थिर Selection FullSelection(RTLIL::Design *design = nullptr) { परत Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	स्थिर Selection CompleteSelection(RTLIL::Design *design = nullptr) { परत Selection(true, true, design); };
};

संरचना RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक { h.eat(hashidx_); परत h; }

	Monitor() {
		स्थिर अचिन्हित पूर्णांक hashidx_मोजणी = 123456789;
		hashidx_मोजणी = mkhash_xorshift(hashidx_मोजणी);
		hashidx_ = hashidx_मोजणी;
	}

	आभासी ~Monitor() { }
	आभासी शून्य notify_module_add(RTLIL::Module*) { }
	आभासी शून्य notify_module_del(RTLIL::Module*) { }
	आभासी शून्य notify_connect(RTLIL::Cell*, RTLIL::IdString, स्थिरांक RTLIL::SigSpec&, स्थिरांक RTLIL::SigSpec&) { }
	आभासी शून्य notify_connect(RTLIL::Module*, स्थिरांक RTLIL::SigSig&) { }
	आभासी शून्य notify_connect(RTLIL::Module*, स्थिरांक std::vector<RTLIL::SigSig>&) { }
	आभासी शून्य notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
संरचना define_map_t;

ढाँचा<typename N>
ओळीत constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::Leaf> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, स्थिरांक अक्षर*> || std::is_same_v<std::decay_t<N>, अक्षर*>;

#define YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, पूर्णांक> = 0

ढाँचा<typename N>
ओळीत constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, स्थिरांक अक्षर *> || std::is_same_v<std::decay_t<N>, अक्षर *>;

#define YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, पूर्णांक> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#define YS_NAME_FWD_POOL(_func, _pool) \
	ढाँचा<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(स्वतः) _func(N name, Rest&&... rest) \
		{ परत _func(_pool.add(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#define YS_NAME_FWD_2ND_POOL(_func, _pool) \
	ढाँचा<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(स्वतः) _func(T &&first, N name) \
		{ परत _func(std::forward<T>(first), _pool.add(std::move(name))); }

#define YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#define YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

संरचना RTLIL::Design
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक { h.eat(hashidx_); परत h; }

	pool<RTLIL::Monitor*> monitors;
	dict<std::string, std::string> scratchpad;

	bool flagBufferedNormalized = false;
	शून्य bufNormalize(bool enable=true);

	पूर्णांक refcount_modules_;
	dict<RTLIL::IdString, RTLIL::Module*> modules_;

खाजगी:
	TwinePool twines_;

सार्वजनिक:
	TwinePool &twines() { परत twines_; }
	स्थिरांक TwinePool &twines() स्थिरांक { परत twines_; }

	शून्य absorb_attrs(RTLIL::AttrObject *obj, dict<IdString, RTLIL::Const> &&buf);

	माप_प्रकार gc_twines();

	std::vector<std::unique_ptr<AST::Ast_गाठ>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_defines;

	std::vector<RTLIL::Selection> selection_चळत;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_active_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	स्थिरांक RTLIL::Module *module(RTLIL::IdString name) स्थिरांक;
	ढाँचा<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ परत module(twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> स्थिरांक RTLIL::Module *module(N name) स्थिरांक
		{ परत module(twines().find(std::string_view(name))); }
	RTLIL::Module *शीर्ष_module() स्थिरांक;

	bool has(RTLIL::IdString id) स्थिरांक {
		twines_.check_owned(id);
		परत modules_.मोजणी(id) != 0;
	}
	ढाँचा<typename N, YS_NAME_STRING(N)> bool has(N name) स्थिरांक
		{ परत has(twines().find(std::string_view(name))); }

	शून्य add(RTLIL::Module *module);

	RTLIL::Module *addModule(RTLIL::IdString name);
	YS_NAME_FWD_POOL(addModule, twines())
	शून्य हटाओ(RTLIL::Module *module);
	शून्य नाम(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(नाम, twines())

	शून्य scratchpad_unset(स्थिरांक std::string &varname);

	शून्य scratchpad_set_int(स्थिरांक std::string &varname, पूर्णांक मूल्य);
	शून्य scratchpad_set_bool(स्थिरांक std::string &varname, bool मूल्य);
	शून्य scratchpad_set_string(स्थिरांक std::string &varname, std::string मूल्य);

	पूर्णांक scratchpad_get_int(स्थिरांक std::string &varname, पूर्णांक default_मूल्य_2 = 0) स्थिरांक;
	bool scratchpad_get_bool(स्थिरांक std::string &varname, bool default_मूल्य_2 = false) स्थिरांक;
	std::string scratchpad_get_string(स्थिरांक std::string &varname, स्थिरांक std::string &default_मूल्य_2 = std::string()) स्थिरांक;

	शून्य sort();
	शून्य sort_modules();
	शून्य check();
	शून्य optimize();

	शून्य clone_into(RTLIL::Design *dst) स्थिरांक;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) स्थिरांक;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) स्थिरांक;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) स्थिरांक;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) स्थिरांक;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::Module *mod) स्थिरांक;

	// push the given selection to the selection stack
	शून्य ढकला_selection(RTLIL::Selection sel);
	// push a new selection to the selection stack, with nothing selected
	शून्य ढकला_empty_selection();
	// push a new selection to the selection stack, with all non-boxed
	// modules selected
	शून्य ढकला_full_selection();
	// push a new selection to the selection stack, with all modules
	// selected including boxes
	शून्य ढकला_complete_selection();
	// pop the current selection from the stack, returning to a full
	// selection (no boxes) if the stack is empty
	शून्य काढा_selection();

	// get the current selection
	RTLIL::Selection &selection() {
		परत selection_चळत.back();
	}

	// get the current selection
	स्थिरांक RTLIL::Selection &selection() स्थिरांक {
		परत selection_चळत.back();
	}

	// is the current selection a full selection (no boxes)
	bool full_selection() स्थिरांक {
		परत selection().full_selection;
	}

	// is the given module in the current selection
	ढाँचा<typename T1> bool selected(T1 *module) स्थिरांक {
		परत selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	ढाँचा<typename T1, typename T2> bool selected(T1 *module, T2 *member) स्थिरांक {
		परत selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	ढाँचा<typename T1> शून्य select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	ढाँचा<typename T1, typename T2> शून्य select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::vector<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) स्थिरांक;

	// returns all selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_modules() स्थिरांक { परत selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::vector<RTLIL::Module*> selected_unboxed_modules() स्थिरांक { परत selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_modules_warn() स्थिरांक { परत selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::vector<RTLIL::Module*> selected_whole_modules() स्थिरांक { परत selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_whole_modules() स्थिरांक { परत selected_modules(SELECT_WHOLE_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::vector<RTLIL::Module*> selected_whole_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) स्थिरांक { परत selected_modules(SELECT_WHOLE_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules() स्थिरांक { परत selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules_warn() स्थिरांक { परत selected_modules(SELECT_WHOLE_WARN, SB_UNBOXED_WARN); }

	स्थिर std::map<अचिन्हित पूर्णांक, RTLIL::Design*> *get_all_designs(शून्य);

	std::string to_rtlil_str(bool only_selected = true) स्थिरांक;
};

संरचना RTLIL::Module : सार्वजनिक RTLIL::AttrObject
{
	मित्र संरचना RTLIL::Cell;
	मित्र संरचना RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक { h.eat(hashidx_); परत h; }

संरक्षित:
	शून्य add(RTLIL::Wire *wire);
	शून्य add(RTLIL::Cell *cell);
	शून्य add(RTLIL::Process *process);

सार्वजनिक:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() स्थिरांक { परत design->twines(); }

	पूर्णांक refcount_wires_;
	पूर्णांक refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::vector<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_मूल्ये;
	dict<RTLIL::IdString, RTLIL::Memory*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	आभासी ~Module();
	आभासी RTLIL::IdString derive(RTLIL::Design *design, स्थिरांक dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	आभासी RTLIL::IdString derive(RTLIL::Design *design, स्थिरांक dict<RTLIL::IdString, RTLIL::Const> &parameters, स्थिरांक dict<RTLIL::IdString, RTLIL::Module*> &interfaces, स्थिरांक dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	आभासी माप_प्रकार मोजणी_id(RTLIL::IdString id);
	आभासी शून्य expand_interfaces(RTLIL::Design *design, स्थिरांक dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	आभासी bool reprocess_if_necessary(RTLIL::Design *design);

	आभासी शून्य sort();
	आभासी शून्य check();
	आभासी शून्य optimize();
	आभासी शून्य makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) स्थिरांक {
		परत get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	शून्य connect(स्थिरांक RTLIL::SigSig &conn);
	शून्य connect(स्थिरांक RTLIL::SigSpec &lhs, स्थिरांक RTLIL::SigSpec &rhs);
	शून्य new_connections(स्थिरांक std::vector<RTLIL::SigSig> &new_conn);
	स्थिरांक std::vector<RTLIL::SigSig> &connections() स्थिरांक;

	std::vector<RTLIL::IdString> ports;
	शून्य fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_queue;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_port_queue;
	pool<RTLIL::Wire *> buf_norm_wire_queue;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_index;
	शून्य bufNormalize();

	ढाँचा<typename T> शून्य rewrite_sigspecs(T &functor);
	ढाँचा<typename T> शून्य rewrite_sigspecs2(T &functor);
	शून्य cloneInto(RTLIL::Module *new_mod) स्थिरांक;
	आभासी RTLIL::Module *clone() स्थिरांक;
	आभासी RTLIL::Module *clone(RTLIL::Design *dst) स्थिरांक;
	आभासी RTLIL::Module *clone(RTLIL::Design *dst, IdString target_name) स्थिरांक;

	bool has_memories() स्थिरांक;
	bool has_processes() स्थिरांक;

	bool has_memories_warn() स्थिरांक;
	bool has_processes_warn() स्थिरांक;

	bool is_selected() स्थिरांक;
	bool is_selected_whole() स्थिरांक;

	std::vector<RTLIL::Wire*> selected_wires() स्थिरांक;
	std::vector<RTLIL::Cell*> selected_cells() स्थिरांक;
	std::vector<RTLIL::Memory*> selected_memories() स्थिरांक;
	std::vector<RTLIL::Process*> selected_processes() स्थिरांक;
	std::vector<RTLIL::SelectedMember> selected_members() स्थिरांक;

	ढाँचा<typename T> bool selected(T *member) स्थिरांक {
		परत design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		जर (design)
			design->twines().check_owned(id);
		स्वतः it = wires_.find(id);
		परत it == wires_.end() ? nullptr : it->second;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		जर (design)
			design->twines().check_owned(id);
		स्वतः it = cells_.find(id);
		परत it == cells_.end() ? nullptr : it->second;
	}

	स्थिरांक RTLIL::Wire* wire(RTLIL::IdString id) स्थिरांक{
		जर (design)
			design->twines().check_owned(id);
		स्वतः it = wires_.find(id);
		परत it == wires_.end() ? nullptr : it->second;
	}
	स्थिरांक RTLIL::Cell* cell(RTLIL::IdString id) स्थिरांक {
		जर (design)
			design->twines().check_owned(id);
		स्वतः it = cells_.find(id);
		परत it == cells_.end() ? nullptr : it->second;
	}

	ढाँचा<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ परत wire(twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ परत cell(twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> स्थिरांक RTLIL::Wire *wire(N name) स्थिरांक
		{ परत wire(twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> स्थिरांक RTLIL::Cell *cell(N name) स्थिरांक
		{ परत cell(twines().find(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { परत RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	पूर्णांक wires_size() स्थिरांक { परत wires_.size(); }
	RTLIL::Wire* wire_at(पूर्णांक index) स्थिरांक { परत wires_.element(index)->second; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { परत RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	पूर्णांक cells_size() स्थिरांक { परत cells_.size(); }
	RTLIL::Cell* cell_at(पूर्णांक index) स्थिरांक { परत cells_.element(index)->second; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	शून्य हटाओ(स्थिरांक pool<RTLIL::Wire*> &wires);
	शून्य हटाओ(RTLIL::Cell *cell);
	शून्य हटाओ(RTLIL::Memory *memory);
	शून्य हटाओ(RTLIL::Process *process);

	शून्य नाम(RTLIL::Wire *wire, RTLIL::IdString new_name);
	शून्य नाम(RTLIL::Cell *cell, RTLIL::IdString new_name);
	शून्य नाम(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(नाम)

	शून्य swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2);
	शून्य swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, पूर्णांक &index);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *addWire(RTLIL::IdString name, पूर्णांक width = 1);
	RTLIL::Wire *addWire(RTLIL::IdString name, स्थिरांक RTLIL::Wire *other);
	YS_NAME_FWD(addWire)

	RTLIL::Cell *addCell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *addCell(RTLIL::IdString name, स्थिरांक RTLIL::Cell *other);
	YS_NAME_FWD(addCell)
	ढाँचा<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *addCell(RTLIL::IdString name, T type) { परत addCell(name, design->twines().add(std::move(type))); }

	RTLIL::Memory *addMemory(RTLIL::IdString name);
	YS_NAME_FWD(addMemory)
	RTLIL::Memory *addMemory(RTLIL::IdString name, स्थिरांक RTLIL::Memory *other);

	RTLIL::Process *addProcess(RTLIL::IdString name);
	YS_NAME_FWD(addProcess)
	RTLIL::Process *addProcess(RTLIL::IdString name, स्थिरांक RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* addNot     (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addPos     (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addBuf     (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addBarrier (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addNeg     (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");

	RTLIL::Cell* addAnd  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addOr   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addXor  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addXnor (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");

	RTLIL::Cell* addReduceAnd  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addReduceOr   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addReduceXor  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addReduceXnor (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addReduceBool (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");

	RTLIL::Cell* addShl    (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addShr    (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addSshl   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addSshr   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addShift  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addShiftx (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");

	RTLIL::Cell* addLt  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addLe  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addEq  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addNe  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addEqx (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addNex (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addGe  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addGt  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");

	RTLIL::Cell* addAdd (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addSub (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addMul (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	// truncating division
	RTLIL::Cell* addDiv (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	// truncating modulo
	RTLIL::Cell* addMod (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDivFloor (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addModFloor (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addPow (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, स्थिरांक std::string &src = "");

	RTLIL::Cell* addFa (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_c, स्थिरांक RTLIL::SigSpec &sig_x, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src = "");

	RTLIL::Cell* addLogicNot (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addLogicAnd (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::Cell* addLogicOr  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, bool is_signed = false, स्थिरांक std::string &src = "");

	RTLIL::Cell* addMux  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addPmux (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addBmux (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDemux (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src = "");

	RTLIL::Cell* addBweqx  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addBwmux  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src = "");

	RTLIL::Cell* addSlice  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, RTLIL::Const offset, स्थिरांक std::string &src = "");
	RTLIL::Cell* addConcat (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addLut    (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_y, RTLIL::Const lut, स्थिरांक std::string &src = "");
	RTLIL::Cell* addTribuf (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAssert (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAssume (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक std::string &src = "");
	RTLIL::Cell* addLive   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक std::string &src = "");
	RTLIL::Cell* addFair   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक std::string &src = "");
	RTLIL::Cell* addCover  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक std::string &src = "");
	RTLIL::Cell* addEquiv  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src = "");

	RTLIL::Cell* addSr    (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr, स्थिरांक RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addFf    (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDff   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_d,   स्थिरांक RTLIL::SigSpec &sig_q, bool clk_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDffe  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en,  स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDffsr (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, स्थिरांक RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDffsre (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, स्थिरांक RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAdff (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_arst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, RTLIL::Const arst_मूल्य, bool clk_polarity = true, bool arst_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAdffe (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_arst,  स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, RTLIL::Const arst_मूल्य, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAldff (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_aload, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, स्थिरांक RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAldffe (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_aload,  स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, स्थिरांक RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addSdff (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_srst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, RTLIL::Const srst_मूल्य, bool clk_polarity = true, bool srst_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addSdffe (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_srst,  स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, RTLIL::Const srst_मूल्य, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addSdffce (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_srst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, RTLIL::Const srst_मूल्य, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDlatch (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, bool en_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAdlatch (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_arst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, RTLIL::Const arst_मूल्य, bool en_polarity = true, bool arst_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDlatchsr (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, स्थिरांक RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, स्थिरांक std::string &src = "");

	RTLIL::Cell* addBufGate    (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addNotGate    (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAndGate    (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addNandGate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addOrGate     (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addNorGate    (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addXorGate    (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addXnorGate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAndnotGate (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addOrnotGate  (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addMuxGate    (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_s, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addNmuxGate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_s, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAoi3Gate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_c, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addOai3Gate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_c, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAoi4Gate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_c, स्थिरांक RTLIL::SigBit &sig_d, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");
	RTLIL::Cell* addOai4Gate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_c, स्थिरांक RTLIL::SigBit &sig_d, स्थिरांक RTLIL::SigBit &sig_y, स्थिरांक std::string &src = "");

	RTLIL::Cell* addSrGate     (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr,
			स्थिरांक RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addFfGate     (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDffGate    (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, bool clk_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDffeGate   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDffsrGate  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, स्थिरांक RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDffsreGate (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, स्थिरांक RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAdffGate   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_arst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
			bool arst_मूल्य = false, bool clk_polarity = true, bool arst_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAdffeGate  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_arst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
			bool arst_मूल्य = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAldffGate   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_aload, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
			स्थिरांक RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAldffeGate  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_aload, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
			स्थिरांक RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addSdffGate   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_srst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
			bool srst_मूल्य = false, bool clk_polarity = true, bool srst_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addSdffeGate  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_srst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
			bool srst_मूल्य = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addSdffceGate (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_clk, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_srst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
			bool srst_मूल्य = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDlatchGate (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, bool en_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addAdlatchGate(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_arst, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q,
			bool arst_मूल्य = false, bool en_polarity = true, bool arst_polarity = true, स्थिरांक std::string &src = "");
	RTLIL::Cell* addDlatchsrGate  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_en, स्थिरांक RTLIL::SigSpec &sig_set, स्थिरांक RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, स्थिरांक RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, स्थिरांक std::string &src = "");

	RTLIL::Cell* addAnyinit(RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_d, स्थिरांक RTLIL::SigSpec &sig_q, स्थिरांक std::string &src = "");

	// The methods without the add* prefix create a cell and an output signal. They return the newly created output signal.

	RTLIL::SigSpec Not     (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Pos     (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Buf     (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Barrier (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Neg     (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिरांक std::string &src = "");

	RTLIL::SigSpec And  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Or   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Xor  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Xnor (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");

	RTLIL::SigSpec ReduceAnd  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec ReduceOr   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec ReduceXor  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec ReduceXnor (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec ReduceBool (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिरांक std::string &src = "");

	RTLIL::SigSpec Shl    (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Shr    (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Sshl   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Sshr   (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Shift  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");

	RTLIL::SigSpec Add (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Sub (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Mul (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	// truncating division
	RTLIL::SigSpec Div (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	// truncating modulo
	RTLIL::SigSpec Mod (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec DivFloor (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec ModFloor (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Pow (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool a_signed = false, bool b_signed = false, स्थिरांक std::string &src = "");

	RTLIL::SigSpec LogicNot (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec LogicAnd (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");
	RTLIL::SigSpec LogicOr  (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, bool is_signed = false, स्थिरांक std::string &src = "");

	RTLIL::SigSpec Mux      (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Pmux     (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Bmux     (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Demux     (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक std::string &src = "");

	RTLIL::SigSpec Bweqx      (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Bwmux      (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_b, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक std::string &src = "");

	RTLIL::SigBit BufGate    (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक std::string &src = "");
	RTLIL::SigBit NotGate    (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक std::string &src = "");
	RTLIL::SigBit AndGate    (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक std::string &src = "");
	RTLIL::SigBit NandGate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक std::string &src = "");
	RTLIL::SigBit OrGate     (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक std::string &src = "");
	RTLIL::SigBit NorGate    (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक std::string &src = "");
	RTLIL::SigBit XorGate    (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक std::string &src = "");
	RTLIL::SigBit XnorGate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक std::string &src = "");
	RTLIL::SigBit AndnotGate (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक std::string &src = "");
	RTLIL::SigBit OrnotGate  (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक std::string &src = "");
	RTLIL::SigBit MuxGate    (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_s, स्थिरांक std::string &src = "");
	RTLIL::SigBit NmuxGate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_s, स्थिरांक std::string &src = "");
	RTLIL::SigBit Aoi3Gate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_c, स्थिरांक std::string &src = "");
	RTLIL::SigBit Oai3Gate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_c, स्थिरांक std::string &src = "");
	RTLIL::SigBit Aoi4Gate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_c, स्थिरांक RTLIL::SigBit &sig_d, स्थिरांक std::string &src = "");
	RTLIL::SigBit Oai4Gate   (RTLIL::IdString name, स्थिरांक RTLIL::SigBit &sig_a, स्थिरांक RTLIL::SigBit &sig_b, स्थिरांक RTLIL::SigBit &sig_c, स्थिरांक RTLIL::SigBit &sig_d, स्थिरांक std::string &src = "");

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, पूर्णांक width = 1, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, पूर्णांक width = 1, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, पूर्णांक width = 1, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, पूर्णांक width = 1, स्थिरांक std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, स्थिरांक std::string &src = "");

	RTLIL::SigSpec SetTag          (RTLIL::IdString name, स्थिरांक std::string &tag, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक RTLIL::SigSpec &sig_c, स्थिरांक std::string &src = "");
	RTLIL::Cell*   addSetTag       (RTLIL::IdString name, स्थिरांक std::string &tag, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक RTLIL::SigSpec &sig_c, स्थिरांक RTLIL::SigSpec &sig_y, स्थिरांक std::string &src = "");
	RTLIL::SigSpec GetTag          (RTLIL::IdString name, स्थिरांक std::string &tag, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक std::string &src = "");
	RTLIL::Cell*   addOverwriteTag (RTLIL::IdString name, स्थिरांक std::string &tag, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक RTLIL::SigSpec &sig_s, स्थिरांक RTLIL::SigSpec &sig_c, स्थिरांक std::string &src = "");
	RTLIL::SigSpec OriginalTag     (RTLIL::IdString name, स्थिरांक std::string &tag, स्थिरांक RTLIL::SigSpec &sig_a, स्थिरांक std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, स्थिरांक RTLIL::SigSpec &sig_e, स्थिरांक std::string &src = "");
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

	std::string to_rtlil_str() स्थिरांक;
#ifdef YOSYS_ENABLE_PYTHON
	स्थिर std::map<अचिन्हित पूर्णांक, RTLIL::Module*> *get_all_modules(शून्य);
#endif
};

संरचना RTLIL::Wire : सार्वजनिक RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक { h.eat(hashidx_); परत h; }

संरक्षित:
	// use module->addWire() and module->remove() to create or destroy wires
	मित्र संरचना RTLIL::Module;
	Wire();
	~Wire();

	मित्र संरचना RTLIL::Design;
	मित्र संरचना RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driverPort_;

सार्वजनिक:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	शून्य चालक=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	पूर्णांक width, start_offset, port_id;
	bool port_आदान, port_output, upto, is_signed;

	RTLIL::Design *design() स्थिरांक { परत module ? module->design : nullptr; }
	TwinePool &twines() स्थिरांक { परत module->twines(); }

	bool known_driver() स्थिरांक { परत driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() स्थिरांक    { log_assert(driverCell_); परत driverCell_; };
	RTLIL::IdString driverPort() स्थिरांक { log_assert(driverCell_); परत driverPort_; };

	पूर्णांक from_hdl_index(पूर्णांक hdl_index) {
		पूर्णांक zero_index = hdl_index - start_offset;
		पूर्णांक rtlil_index = upto ? width - 1 - zero_index : zero_index;
		परत rtlil_index >= 0 && rtlil_index < width ? rtlil_index : पूर्णांक_न्यून;
	}

	पूर्णांक to_hdl_index(पूर्णांक rtlil_index) {
		जर (rtlil_index < 0 || rtlil_index >= width)
			परत पूर्णांक_न्यून;
		पूर्णांक zero_index = upto ? width - 1 - rtlil_index : rtlil_index;
		परत zero_index + start_offset;
	}

	std::string to_rtlil_str() स्थिरांक;

#ifdef YOSYS_ENABLE_PYTHON
	स्थिर std::map<अचिन्हित पूर्णांक, RTLIL::Wire*> *get_all_wires(शून्य);
#endif
};

ओळीत पूर्णांक GetSize(RTLIL::Wire *wire) {
	परत wire->width;
}

संरचना RTLIL::Memory : सार्वजनिक RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक { h.eat(hashidx_); परत h; }

	Memory();
	~Memory();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() स्थिरांक { परत module ? module->design : nullptr; }
	TwinePool &twines() स्थिरांक { परत module->twines(); }

	RTLIL::MemoryNameId name;

	पूर्णांक width, start_offset, size;
#ifdef YOSYS_ENABLE_PYTHON
	स्थिर std::map<अचिन्हित पूर्णांक, RTLIL::Memory*> *get_all_memorys(शून्य);
#endif

	std::string to_rtlil_str() स्थिरांक;
};

संरचना RTLIL::Cell : सार्वजनिक RTLIL::AttrObject
{
खाजगी:
	शून्य initIndex();

	bool bufnorm_handle_setPort(IdString portname, RTLIL::SigSpec &संकेत, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
सार्वजनिक:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक { h.eat(hashidx_); परत h; }

संरक्षित:
	// use module->addCell() and module->remove() to create or destroy cells
	मित्र संरचना RTLIL::Module;
	Cell();
	~Cell();

सार्वजनिक:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	शून्य चालक=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() स्थिरांक { परत module ? module->design : nullptr; }
	TwinePool &twines() स्थिरांक { परत module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool hasPort(RTLIL::IdString portname) स्थिरांक;
	शून्य unsetPort(RTLIL::IdString portname);
	शून्य setPort(RTLIL::IdString portname, RTLIL::SigSpec संकेत);
	ढाँचा<typename N, YS_NAME_STRING(N)> शून्य setPort(N portname, RTLIL::SigSpec संकेत)
		{ setPort(module->twines().add(std::move(portname)), std::move(संकेत)); }
	स्थिरांक RTLIL::SigSpec &getPort(RTLIL::IdString portname) स्थिरांक;
	स्थिरांक dict<RTLIL::IdString, RTLIL::SigSpec> &connections() स्थिरांक;

	bool known() स्थिरांक;
	bool आदान(RTLIL::IdString portname) स्थिरांक;
	bool output(RTLIL::IdString portname) स्थिरांक;
	PortDir port_dir(RTLIL::IdString portname) स्थिरांक;

	bool hasParam(RTLIL::IdString paramname) स्थिरांक;
	शून्य unsetParam(RTLIL::IdString paramname);
	शून्य setParam(RTLIL::IdString paramname, RTLIL::Const मूल्य);
	स्थिरांक RTLIL::Const &getParam(RTLIL::IdString paramname) स्थिरांक;

	ढाँचा<typename N, YS_NAME_STRING(N)> bool hasParam(N name) स्थिरांक
		{ परत hasParam(module->twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> शून्य unsetParam(N name)
		{ unsetParam(module->twines().find(std::string_view(name))); }
	ढाँचा<typename N, YS_NAME_STRING(N)> शून्य setParam(N name, RTLIL::Const मूल्य)
		{ setParam(module->twines().add(std::move(name)), std::move(मूल्य)); }
	ढाँचा<typename N, YS_NAME_STRING(N)> स्थिरांक RTLIL::Const &getParam(N name) स्थिरांक
		{ परत getParam(module->twines().find(std::string_view(name))); }

	शून्य sort();
	शून्य check();
	शून्य fixup_parameters(bool set_a_signed = false, bool set_b_signed = false);

	bool has_keep_attr() स्थिरांक;

	ढाँचा<typename T> शून्य rewrite_sigspecs(T &functor);
	ढाँचा<typename T> शून्य rewrite_sigspecs2(T &functor);

#ifdef YOSYS_ENABLE_PYTHON
	स्थिर std::map<अचिन्हित पूर्णांक, RTLIL::Cell*> *get_all_cells(शून्य);
#endif

	bool has_memid() स्थिरांक;
	bool is_mem_cell() स्थिरांक;
	bool is_builtin_ff() स्थिरांक;

	std::string to_rtlil_str() स्थिरांक;
};

संरचना RTLIL::CaseRule : सार्वजनिक RTLIL::AttrObject
{
	std::vector<RTLIL::SigSpec> compare;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::SwitchRule*> switches;

	~CaseRule();

	bool empty() स्थिरांक;

	ढाँचा<typename T> शून्य rewrite_sigspecs(T &functor);
	ढाँचा<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::CaseRule *clone() स्थिरांक;
};

संरचना RTLIL::SwitchRule : सार्वजनिक RTLIL::AttrObject
{
	RTLIL::SigSpec संकेत;
	std::vector<RTLIL::CaseRule*> cases;

	~SwitchRule();

	bool empty() स्थिरांक;

	ढाँचा<typename T> शून्य rewrite_sigspecs(T &functor);
	ढाँचा<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::SwitchRule *clone() स्थिरांक;
};

संरचना RTLIL::MemWriteAction : RTLIL::AttrObject
{
	RTLIL::IdString memid;
	RTLIL::SigSpec address;
	RTLIL::SigSpec data;
	RTLIL::SigSpec enable;
	RTLIL::Const priority_mask;
};

संरचना RTLIL::SyncRule
{
	RTLIL::SyncType type;
	RTLIL::SigSpec संकेत;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::MemWriteAction> mem_write_actions;

	ढाँचा<typename T> शून्य rewrite_sigspecs(T &functor);
	ढाँचा<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::SyncRule *clone() स्थिरांक;
};

संरचना RTLIL::Process : सार्वजनिक RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक { h.eat(hashidx_); परत h; }

संरक्षित:
	// use module->addProcess() and module->remove() to create or destroy processes
	मित्र संरचना RTLIL::Module;
	Process();
	~Process();

सार्वजनिक:
	RTLIL::Module *module;
	RTLIL::CaseRule मूळ_case;
	std::vector<RTLIL::SyncRule*> syncs;

	RTLIL::Design *design() स्थिरांक { परत module ? module->design : nullptr; }
	TwinePool &twines() स्थिरांक { परत module->twines(); }

	ढाँचा<typename T> शून्य rewrite_sigspecs(T &functor);
	ढाँचा<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() स्थिरांक;

	std::string to_rtlil_str() स्थिरांक;
};

ओळीत RTLIL::SigBit::SigBit() : wire(रिक्त), data(RTLIL::State::S0) { }
ओळीत RTLIL::SigBit::SigBit(RTLIL::State bit) : wire(रिक्त), data(bit) { }
ओळीत RTLIL::SigBit::SigBit(bool bit) : wire(रिक्त), data(bit ? State::S1 : State::S0) { }
ओळीत RTLIL::SigBit::SigBit(RTLIL::Wire *wire) : wire(wire), offset(0) { log_assert(wire && wire->width == 1); }
ओळीत RTLIL::SigBit::SigBit(RTLIL::Wire *wire, पूर्णांक offset) : wire(wire), offset(offset) { log_assert(wire != nullptr); }
ओळीत RTLIL::SigBit::SigBit(स्थिरांक RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.width == 1); जर (wire) offset = chunk.offset; अन्यथा data = chunk.data[0]; }
ओळीत RTLIL::SigBit::SigBit(स्थिरांक RTLIL::SigChunk &chunk, पूर्णांक index) : wire(chunk.wire) { जर (wire) offset = chunk.offset + index; अन्यथा data = chunk.data[index]; }

ओळीत bool RTLIL::SigBit::चालक<(स्थिरांक RTLIL::SigBit &other) स्थिरांक {
	जर (wire == other.wire)
		परत wire ? (offset < other.offset) : (data < other.data);
	जर (wire != nullptr && other.wire != nullptr)
		परत wire->name < other.wire->name;
	परत (wire != nullptr) < (other.wire != nullptr);
}

ओळीत bool RTLIL::SigBit::चालक==(स्थिरांक RTLIL::SigBit &other) स्थिरांक {
	परत (wire == other.wire) && (wire ? (offset == other.offset) : (data == other.data));
}

ओळीत bool RTLIL::SigBit::चालक!=(स्थिरांक RTLIL::SigBit &other) स्थिरांक {
	परत (wire != other.wire) || (wire ? (offset != other.offset) : (data != other.data));
}

ओळीत Hasher RTLIL::SigBit::hash_into(Hasher h) स्थिरांक {
	जर (wire) {
		h.eat(offset);
		h.eat(wire->name);
		परत h;
	}
	h.eat(data);
	परत h;
}


ओळीत Hasher RTLIL::SigBit::hash_शीर्ष() स्थिरांक {
	Hasher h;
	जर (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.bits();
		h.force(hashlib::legacy::djb2_add(n, offset));
		परत h;
	}
	h.force(data);
	परत h;
}

ओळीत RTLIL::SigBit &RTLIL::SigSpecIterator::चालक*() स्थिरांक {
	परत (*sig_p)[index];
}

ओळीत स्थिरांक RTLIL::SigBit &RTLIL::SigSpecConstIterator::चालक*() {
	bit = (*sig_p)[index];
	परत bit;
}

ओळीत RTLIL::SigBit::SigBit(स्थिरांक RTLIL::SigSpec &sig) {
	log_assert(sig.size() == 1);
	स्वतः it = sig.chunks().begin();
	*this = SigBit(*it);
}

ढाँचा<typename T>
शून्य RTLIL::Module::rewrite_sigspecs(T &functor)
{
	साठी (स्वतः &it : cells_)
		it.second->rewrite_sigspecs(functor);
	साठी (स्वतः &it : processes)
		it.second->rewrite_sigspecs(functor);
	साठी (स्वतः &it : connections_) {
		functor(it.first);
		functor(it.second);
	}
}

ढाँचा<typename T>
शून्य RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	साठी (स्वतः &it : cells_)
		it.second->rewrite_sigspecs2(functor);
	साठी (स्वतः &it : processes)
		it.second->rewrite_sigspecs2(functor);
	साठी (स्वतः &it : connections_) {
		functor(it.first, it.second);
	}
}

ढाँचा<typename T>
शून्य RTLIL::Cell::rewrite_sigspecs(T &functor) {
	साठी (स्वतः &it : connections_)
		functor(it.second);
}

ढाँचा<typename T>
शून्य RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	साठी (स्वतः &it : connections_)
		functor(it.second);
}

ढाँचा<typename T>
शून्य RTLIL::CaseRule::rewrite_sigspecs(T &functor) {
	साठी (स्वतः &it : compare)
		functor(it);
	साठी (स्वतः &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	साठी (स्वतः it : switches)
		it->rewrite_sigspecs(functor);
}

ढाँचा<typename T>
शून्य RTLIL::CaseRule::rewrite_sigspecs2(T &functor) {
	साठी (स्वतः &it : compare)
		functor(it);
	साठी (स्वतः &it : actions) {
		functor(it.first, it.second);
	}
	साठी (स्वतः it : switches)
		it->rewrite_sigspecs2(functor);
}

ढाँचा<typename T>
शून्य RTLIL::SwitchRule::rewrite_sigspecs(T &functor)
{
	functor(संकेत);
	साठी (स्वतः it : cases)
		it->rewrite_sigspecs(functor);
}

ढाँचा<typename T>
शून्य RTLIL::SwitchRule::rewrite_sigspecs2(T &functor)
{
	functor(संकेत);
	साठी (स्वतः it : cases)
		it->rewrite_sigspecs2(functor);
}

ढाँचा<typename T>
शून्य RTLIL::SyncRule::rewrite_sigspecs(T &functor)
{
	functor(संकेत);
	साठी (स्वतः &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	साठी (स्वतः &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

ढाँचा<typename T>
शून्य RTLIL::SyncRule::rewrite_sigspecs2(T &functor)
{
	functor(संकेत);
	साठी (स्वतः &it : actions) {
		functor(it.first, it.second);
	}
	साठी (स्वतः &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

ढाँचा<typename T>
शून्य RTLIL::Process::rewrite_sigspecs(T &functor)
{
	मूळ_case.rewrite_sigspecs(functor);
	साठी (स्वतः it : syncs)
		it->rewrite_sigspecs(functor);
}

ढाँचा<typename T>
शून्य RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	मूळ_case.rewrite_sigspecs2(functor);
	साठी (स्वतः it : syncs)
		it->rewrite_sigspecs2(functor);
}

#अन्तर्भाव "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_END

#endif
