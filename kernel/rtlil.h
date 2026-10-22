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
	गणना State : unsigned वर्ण {
		S0 = 0,
		S1 = 1,
		Sx = 2, // undefined value or conflict
		Sz = 3, // high-impedance / not-connected
		Sa = 4, // don't care (used only in cases)
		Sm = 5  // marker (used internally by some passes)
	};

	गणना SyncType : unsigned वर्ण {
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
	गणना ConstFlags : unsigned वर्ण {
		CONST_FLAG_NONE    = 0,
		CONST_FLAG_STRING  = 1,
		CONST_FLAG_SIGNED  = 2,  // only used for parameters
		CONST_FLAG_REAL    = 4,  // only used for parameters
		CONST_FLAG_UNSIZED = 8,  // only used for parameters
	};

	गणना SelectPartials : unsigned वर्ण {
		SELECT_ALL = 0,          // include partial modules
		SELECT_WHOLE_ONLY = 1,   // ignore partial modules
		SELECT_WHOLE_WARN = 2,   // call log_warning on partial module
		SELECT_WHOLE_ERR = 3,    // call log_error on partial module
		SELECT_WHOLE_CMDERR = 4  // call log_cmd_error on partial module
	};

	गणना SelectBoxes : unsigned वर्ण {
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

	गणना PortDir : unsigned वर्ण  {
		PD_UNKNOWN = 0,
		PD_आदान = 1,
		PD_OUTPUT = 2,
		PD_INOUT = 3
	};

	// Maximum width in bits of RTLIL::Wire or RTLIL::Const
	constexpr पूर्णाङ्क WIDTH_LIMIT = 1 << 30;

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

	प्रकारनिर्देश std::pair<SigSpec, SigSpec> SigSig;
};

संरचना SigMap;


namespace RTLIL { using YOSYS_NAMESPACE_PREFIX ID; }
namespace RTLIL { using YOSYS_NAMESPACE_PREFIX IdString; }

namespace RTLIL {
	शून्य copy_attr_dict(dict<IdString, RTLIL::Const> &dst,
			ध्रुव dict<IdString, RTLIL::Const> &src,
			ध्रुव RTLIL::Design *src_design, RTLIL::Design *dst_design);

	बाह्य dict<std::string, std::string> constpad;

	[[deprecated("use StaticCellTypes::categories.is_ff() instead")]]
	ध्रुव pool<IdString> &builtin_ff_cell_types();

	स्थिर अन्तःस्थ std::string escape_id(ध्रुव std::string &str) {
		यदि (str.size() > 0 && str[0] != '\\' && str[0] != '$')
			प्रत्यागम "\\" + str;
		प्रत्यागम str;
	}

	स्थिर अन्तःस्थ std::string unescape_id(ध्रुव std::string &str) {
		यदि (str.size() < 2)
			प्रत्यागम str;
		यदि (str[0] != '\\')
			प्रत्यागम str;
		यदि (str[1] == '$' || str[1] == '\\')
			प्रत्यागम str;
		यदि (str[1] >= '0' && str[1] <= '9')
			प्रत्यागम str;
		प्रत्यागम str.substr(1);
	}

	template <typename T> using sort_by_name_id = IdString::compare_ptr_by_name<T>;

	template <typename T> संरचना sort_by_name_str {
		bool operator()(T *a, T *b) ध्रुव {
			प्रत्यागम a->name.lt_by_name(b->name);
		}
	};

	संरचना sort_by_id_str {
		ध्रुव TwinePool& pool;
		explicit sort_by_id_str(ध्रुव TwinePool& pool) : pool(pool) {}
		bool operator()(RTLIL::IdString a, RTLIL::IdString b) ध्रुव {
			प्रत्यागम pool.compare_by_name(a, b) < 0;
		}
	};

	स्थिर अन्तःस्थ std::string encode_filename(ध्रुव std::string &filename)
	{
		std::stringstream val;
		यदि (!std::any_of(filename.begin(), filename.end(), [](वर्ण c) {
			प्रत्यागम static_cast<unsigned वर्ण>(c) < 33 || static_cast<unsigned वर्ण>(c) > 126;
		})) प्रत्यागम filename;
		प्रति (unsigned वर्ण ध्रुव c : filename) {
			यदि (c < 33 || c > 126)
				val << stringf("$%02x", c);
			अन्यथा
				val << c;
		}
		प्रत्यागम val.str();
	}

	// see calc.cc for the implementation of this functions
	RTLIL::Const const_not         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_and         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_or          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_xor         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_xnor        (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_reduce_and  (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_reduce_or   (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_reduce_xor  (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_reduce_xnor (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_reduce_bool (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_logic_not   (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_logic_and   (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_logic_or    (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_shl         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_shr         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_sshl        (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_sshr        (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_shift       (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_shiftx      (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_lt          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_le          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_eq          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_ne          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_eqx         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_nex         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_ge          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_gt          (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_add         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_sub         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_mul         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_div         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_divfloor    (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_modfloor    (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_mod         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_pow         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_pos         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_buf         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);
	RTLIL::Const const_neg         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णाङ्क result_len);

	RTLIL::Const const_mux         (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, ध्रुव RTLIL::Const &arg3);
	RTLIL::Const const_pmux        (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, ध्रुव RTLIL::Const &arg3);
	RTLIL::Const const_bmux        (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2);
	RTLIL::Const const_demux       (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2);

	RTLIL::Const const_bweqx       (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2);
	RTLIL::Const const_bwmux       (ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, ध्रुव RTLIL::Const &arg3);


	// This iterator-range-pair is used for Design::modules(), Module::wires() and Module::cells().
	// It maintains a reference counter that is used to make sure that the container is not modified while being iterated over.

	template<typename T, typename किल्ली_2 = IdString>
	संरचना ObjIterator {
		using iterator_category = std::forward_iterator_tag;
		using मूल्य_type = T;
		using difference_type = ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		typename dict<किल्ली_2, T>::iterator it;
		dict<किल्ली_2, T> *list_p;
		पूर्णाङ्क *refcount_p;

		ObjIterator() : list_p(nullptr), refcount_p(nullptr) {
		}

		ObjIterator(decltype(list_p) list_p, पूर्णाङ्क *refcount_p) : list_p(list_p), refcount_p(refcount_p) {
			यदि (list_p->empty()) {
				this->list_p = nullptr;
				this->refcount_p = nullptr;
			} अन्यथा {
				it = list_p->begin();
				(*refcount_p)++;
			}
		}

		ObjIterator(ध्रुव RTLIL::ObjIterator<T, किल्ली_2> &other) {
			it = other.it;
			list_p = other.list_p;
			refcount_p = other.refcount_p;
			यदि (refcount_p)
				(*refcount_p)++;
		}

		ObjIterator &operator=(ध्रुव RTLIL::ObjIterator<T, किल्ली_2> &other) {
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

		अन्तःस्थ T operator*() ध्रुव {
			log_assert(list_p != nullptr);
			प्रत्यागम it->second;
		}

		अन्तःस्थ bool operator!=(ध्रुव RTLIL::ObjIterator<T, किल्ली_2> &other) ध्रुव {
			यदि (list_p == nullptr || other.list_p == nullptr)
				प्रत्यागम list_p != other.list_p;
			प्रत्यागम it != other.it;
		}


		अन्तःस्थ bool operator==(ध्रुव RTLIL::ObjIterator<T, किल्ली_2> &other) ध्रुव {
			प्रत्यागम !(*this != other);
		}

		अन्तःस्थ ObjIterator<T, किल्ली_2>& operator++() {
			log_assert(list_p != nullptr);
			यदि (++it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			प्रत्यागम *this;
		}

		अन्तःस्थ ObjIterator<T, किल्ली_2>& operator+=(पूर्णाङ्क amt) {
			log_assert(list_p != nullptr);
			it += amt;
			यदि (it == list_p->end()) {
				(*refcount_p)--;
				list_p = nullptr;
				refcount_p = nullptr;
			}
			प्रत्यागम *this;
		}

		अन्तःस्थ ObjIterator<T, किल्ली_2> operator+(पूर्णाङ्क amt) {
			log_assert(list_p != nullptr);
			ObjIterator<T, किल्ली_2> new_obj(*this);
			new_obj.it += amt;
			यदि (new_obj.it == list_p->end()) {
				(*(new_obj.refcount_p))--;
				new_obj.list_p = nullptr;
				new_obj.refcount_p = nullptr;
			}
			प्रत्यागम new_obj;
		}

		अन्तःस्थ ध्रुव ObjIterator<T, किल्ली_2> operator++(पूर्णाङ्क) {
			ObjIterator<T, किल्ली_2> result(*this);
			++(*this);
			प्रत्यागम result;
		}
	};

	template<typename T, typename किल्ली_2 = IdString>
	संरचना ObjRange
	{
		dict<किल्ली_2, T> *list_p;
		पूर्णाङ्क *refcount_p;

		ObjRange(decltype(list_p) list_p, पूर्णाङ्क *refcount_p) : list_p(list_p), refcount_p(refcount_p) { }
		RTLIL::ObjIterator<T, किल्ली_2> begin() { प्रत्यागम RTLIL::ObjIterator<T, किल्ली_2>(list_p, refcount_p); }
		RTLIL::ObjIterator<T, किल्ली_2> end() { प्रत्यागम RTLIL::ObjIterator<T, किल्ली_2>(); }

		size_t size() ध्रुव {
			प्रत्यागम list_p->size();
		}

		operator pool<T>() ध्रुव {
			pool<T> result;
			प्रति (auto &it : *list_p)
				result.घाला(it.second);
			प्रत्यागम result;
		}

		operator std::vector<T>() ध्रुव {
			std::vector<T> result;
			result.reserve(list_p->size());
			प्रति (auto &it : *list_p)
				result.ढकला_back(it.second);
			प्रत्यागम result;
		}

		pool<T> to_pool() ध्रुव { प्रत्यागम *this; }
		std::vector<T> to_vector() ध्रुव { प्रत्यागम *this; }
	};
};

संरचना RTLIL::Const
{
	short पूर्णाङ्क flags;
गुप्त:
	friend class KernelRtlilTest;
	FRIEND_TEST(KernelRtlilTest, ConstStr);
	using bitvectype = std::vector<RTLIL::State>;
	गणना class backing_tag: bool { bits, string };
	// Do not access the union or tag even in Const methods unless necessary
	backing_tag tag;
	सङ्घ {
		bitvectype bits_;
		std::string str_;
	};

	// Use these private utilities instead
	bool is_bits() ध्रुव { प्रत्यागम tag == backing_tag::bits; }
	bool is_str() ध्रुव { प्रत्यागम tag == backing_tag::string; }

	bitvectype* get_if_bits() { प्रत्यागम is_bits() ? &bits_ : रिक्त; }
	std::string* get_if_str() { प्रत्यागम is_str() ? &str_ : रिक्त; }
	ध्रुव bitvectype* get_if_bits() ध्रुव { प्रत्यागम is_bits() ? &bits_ : रिक्त; }
	ध्रुव std::string* get_if_str() ध्रुव { प्रत्यागम is_str() ? &str_ : रिक्त; }

	bitvectype& get_bits();
	std::string& get_str();
	ध्रुव bitvectype& get_bits() ध्रुव;
	ध्रुव std::string& get_str() ध्रुव;
	std::vector<RTLIL::State>& bits_internal();
	शून्य bitvectorize_internal();

सार्वजनिक:
	Const() : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::vector<RTLIL::State>()) {}
	Const(std::string str);
	Const(long long val); // default width is 32
	Const(long long val, पूर्णाङ्क width);
	Const(RTLIL::State bit, पूर्णाङ्क width = 1);
	Const(std::vector<RTLIL::State> bits) : flags(RTLIL::CONST_FLAG_NONE), tag(backing_tag::bits), bits_(std::move(bits)) {}
	Const(ध्रुव std::vector<bool> &bits);
	Const(ध्रुव RTLIL::Const &other);
	Const(RTLIL::Const &&other);
	RTLIL::Const &operator =(ध्रुव RTLIL::Const &other);
	~Const();

	संरचना Builder
	{
		Builder() {}
		Builder(पूर्णाङ्क expected_width) { bits.reserve(expected_width); }
		शून्य ढकला_back(RTLIL::State b) { bits.ढकला_back(b); }
		पूर्णाङ्क size() ध्रुव { प्रत्यागम static_cast<पूर्णाङ्क>(bits.size()); }
		Const build() { प्रत्यागम Const(std::move(bits)); }
	गुप्त:
		std::vector<RTLIL::State> bits;
	};

	bool operator <(ध्रुव RTLIL::Const &other) ध्रुव;
	bool operator ==(ध्रुव RTLIL::Const &other) ध्रुव;
	bool operator !=(ध्रुव RTLIL::Const &other) ध्रुव;

	[[deprecated("Don't use direct access to the internal std::vector<State>, that's an implementation detail.")]]
	std::vector<RTLIL::State>& bits() { प्रत्यागम bits_internal(); }
	[[deprecated("Don't call bitvectorize() directly, it's an implementation detail.")]]
	शून्य bitvectorize() ध्रुव { const_cast<Const*>(this)->bitvectorize_internal(); }

	bool as_bool() ध्रुव;

	// Convert the constant value to a C++ int.
	// NOTE: If the constant is too wide to fit in int (32 bits) this will
	// truncate any higher bits, potentially over/underflowing. Consider using
	// try_as_int, as_int_saturating, or guarding behind convertible_to_int
	// instead.
	पूर्णाङ्क as_int(bool is_signed = false) ध्रुव;

	// Returns true iff the constant can be converted to an int without
	// over/underflow.
	bool convertible_to_int(bool is_signed = false) ध्रुव;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, or std::nullopt otherwise.
	std::optional<पूर्णाङ्क> try_as_int(bool is_signed = false) ध्रुव;

	// Returns the constant's value as an int if it can be represented without
	// over/underflow, otherwise the max/min value for int depending on the sign.
	पूर्णाङ्क as_int_saturating(bool is_signed = false) ध्रुव;

	शून्य tag_bare_integer_const(ध्रुव std::string &मूल्य);

	std::string as_string(ध्रुव वर्ण* any = "-") ध्रुव;
	स्थिर Const from_string(ध्रुव std::string &str);
	std::vector<RTLIL::State> to_bits() ध्रुव;

	std::string decode_string() ध्रुव;
	पूर्णाङ्क size() ध्रुव;
	bool empty() ध्रुव;

	शून्य append(ध्रुव RTLIL::Const &other);
	शून्य set(पूर्णाङ्क i, RTLIL::State state) {
		bits_internal()[i] = state;
	}
	शून्य resize(पूर्णाङ्क size, RTLIL::State fill) {
    log_assert(size >= 0 && size < RTLIL::WIDTH_LIMIT);
		bits_internal().resize(size, fill);
	}

	class const_iterator {
	गुप्त:
		ध्रुव Const* parent;
		size_t idx;

	सार्वजनिक:
		using iterator_category = std::bidirectional_iterator_tag;
		using मूल्य_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = ध्रुव State*;
		using reference = ध्रुव State&;

		const_iterator(ध्रुव Const& c, size_t i) : parent(&c), idx(i) {}

		State operator*() ध्रुव;

		const_iterator& operator++() { ++idx; प्रत्यागम *this; }
		const_iterator& operator--() { --idx; प्रत्यागम *this; }
		const_iterator operator++(पूर्णाङ्क) { const_iterator result(*this); ++idx; प्रत्यागम result; }
		const_iterator operator--(पूर्णाङ्क) { const_iterator result(*this); --idx; प्रत्यागम result; }
		const_iterator& operator+=(पूर्णाङ्क i) { idx += i; प्रत्यागम *this; }

		const_iterator operator+(पूर्णाङ्क add) {
			प्रत्यागम const_iterator(*parent, idx + add);
		}
		const_iterator operator-(पूर्णाङ्क sub) {
			प्रत्यागम const_iterator(*parent, idx - sub);
		}
		पूर्णाङ्क operator-(ध्रुव const_iterator& other) {
			प्रत्यागम idx - other.idx;
		}

		bool operator==(ध्रुव const_iterator& other) ध्रुव {
			प्रत्यागम idx == other.idx;
		}

		bool operator!=(ध्रुव const_iterator& other) ध्रुव {
			प्रत्यागम !(*this == other);
		}
	};

	class iterator {
	गुप्त:
		Const* parent;
		size_t idx;

	सार्वजनिक:
		class proxy {
		गुप्त:
			Const* parent;
			size_t idx;
		सार्वजनिक:
			proxy(Const* parent, size_t idx) : parent(parent), idx(idx) {}
			operator State() ध्रुव { प्रत्यागम (*parent)[idx]; }
			proxy& operator=(State s) { parent->set(idx, s); प्रत्यागम *this; }
			proxy& operator=(ध्रुव proxy& other) { parent->set(idx, (*other.parent)[other.idx]); प्रत्यागम *this; }
		};

		using iterator_category = std::bidirectional_iterator_tag;
		using मूल्य_type = State;
		using difference_type = std::ptrdiff_t;
		using pointer = proxy*;
		using reference = proxy;

		iterator(Const& c, size_t i) : parent(&c), idx(i) {}

		proxy operator*() ध्रुव { प्रत्यागम proxy(parent, idx); }
		iterator& operator++() { ++idx; प्रत्यागम *this; }
		iterator& operator--() { --idx; प्रत्यागम *this; }
		iterator operator++(पूर्णाङ्क) { iterator result(*this); ++idx; प्रत्यागम result; }
		iterator operator--(पूर्णाङ्क) { iterator result(*this); --idx; प्रत्यागम result; }
		iterator& operator+=(पूर्णाङ्क i) { idx += i; प्रत्यागम *this; }

		iterator operator+(पूर्णाङ्क add) {
			प्रत्यागम iterator(*parent, idx + add);
		}
		iterator operator-(पूर्णाङ्क sub) {
			प्रत्यागम iterator(*parent, idx - sub);
		}
		पूर्णाङ्क operator-(ध्रुव iterator& other) {
			प्रत्यागम idx - other.idx;
		}

		bool operator==(ध्रुव iterator& other) ध्रुव {
			प्रत्यागम idx == other.idx;
		}

		bool operator!=(ध्रुव iterator& other) ध्रुव {
			प्रत्यागम !(*this == other);
		}
	};

	const_iterator begin() ध्रुव {
		प्रत्यागम const_iterator(*this, 0);
	}
	const_iterator end() ध्रुव {
		प्रत्यागम const_iterator(*this, size());
	}
	iterator begin() {
		प्रत्यागम iterator(*this, 0);
	}
	iterator end() {
		प्रत्यागम iterator(*this, size());
	}
	State back() ध्रुव {
		प्रत्यागम *(end() - 1);
	}
	State front() ध्रुव {
		प्रत्यागम *begin();
	}
	State at(size_t i) ध्रुव {
		प्रत्यागम *const_iterator(*this, i);
	}
	State operator[](size_t i) ध्रुव {
		प्रत्यागम *const_iterator(*this, i);
	}

	bool is_fully_zero() ध्रुव;
	bool is_fully_ones() ध्रुव;
	bool is_fully_def() ध्रुव;
	bool is_fully_undef() ध्रुव;
	bool is_fully_undef_x_only() ध्रुव;
	bool is_onehot(पूर्णाङ्क *pos = nullptr) ध्रुव;

	RTLIL::Const extract(पूर्णाङ्क offset, पूर्णाङ्क len = 1, RTLIL::State padding = RTLIL::State::S0) ध्रुव;

	// find the MSB without redundant leading bits
	पूर्णाङ्क get_min_size(bool is_signed) ध्रुव;

	// compress representation to the minimum required bits
	शून्य compress(bool is_signed = false);

	std::optional<पूर्णाङ्क> as_int_compress(bool is_signed) ध्रुव;

	शून्य extu(पूर्णाङ्क width) {
		resize(width, RTLIL::State::S0);
	}

	शून्य exts(पूर्णाङ्क width) {
		resize(width, empty() ? RTLIL::State::Sx : back());
	}

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;
};

संरचना RTLIL::AttrObject
{
	dict<RTLIL::IdString, RTLIL::Const> attributes;

	bool has_attribute(RTLIL::IdString id) ध्रुव;

	शून्य set_bool_attribute(RTLIL::IdString id, bool मूल्य=true);
	bool get_bool_attribute(RTLIL::IdString id) ध्रुव;

	[[deprecated("Use Module::get_blackbox_attribute() instead.")]]
	bool get_blackbox_attribute(bool ignore_wb=false) ध्रुव {
		प्रत्यागम get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	शून्य set_string_attribute(RTLIL::IdString id, string मूल्य);
	string get_string_attribute(RTLIL::IdString id) ध्रुव;

	शून्य set_strpool_attribute(RTLIL::IdString id, ध्रुव pool<string> &data);
	शून्य add_strpool_attribute(RTLIL::IdString id, ध्रुव pool<string> &data);
	pool<string> get_strpool_attribute(RTLIL::IdString id) ध्रुव;

	शून्य set_src_attribute(ध्रुव std::string &src) {
		set_string_attribute(ID::src, src);
	}
	std::string get_src_attribute() ध्रुव {
		प्रत्यागम get_string_attribute(ID::src);
	}

	शून्य set_hdlname_attribute(ध्रुव vector<string> &hierarchy);
	vector<string> get_hdlname_attribute() ध्रुव;

	शून्य set_intvec_attribute(RTLIL::IdString id, ध्रुव vector<पूर्णाङ्क> &data);
	vector<पूर्णाङ्क> get_intvec_attribute(RTLIL::IdString id) ध्रुव;
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
	पूर्णाङ्क width, offset;

	SigChunk() : wire(nullptr), width(0), offset(0) {}
	SigChunk(ध्रुव RTLIL::Const &मूल्य) : wire(nullptr), data(मूल्य.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Const &&मूल्य) : wire(nullptr), data(मूल्य.to_bits()), width(GetSize(data)), offset(0) {}
	SigChunk(RTLIL::Wire *wire) : wire(wire), width(GetSize(wire)), offset(0) {}
	SigChunk(RTLIL::Wire *wire, पूर्णाङ्क offset, पूर्णाङ्क width = 1) : wire(wire), width(width), offset(offset) {}
	SigChunk(ध्रुव std::string &str) : SigChunk(RTLIL::Const(str)) {}
	SigChunk(पूर्णाङ्क val) /*default width 32*/ : SigChunk(RTLIL::Const(val)) {}
	SigChunk(पूर्णाङ्क val, पूर्णाङ्क width) : SigChunk(RTLIL::Const(val, width)) {}
	SigChunk(RTLIL::State bit, पूर्णाङ्क width = 1) : SigChunk(RTLIL::Const(bit, width)) {}
	SigChunk(ध्रुव RTLIL::SigBit &bit);

	RTLIL::SigChunk extract(पूर्णाङ्क offset, पूर्णाङ्क length) ध्रुव;
	RTLIL::SigBit operator[](पूर्णाङ्क offset) ध्रुव;
	अन्तःस्थ पूर्णाङ्क size() ध्रुव { प्रत्यागम width; }
	अन्तःस्थ bool is_wire() ध्रुव { प्रत्यागम wire != रिक्त; }

	bool operator <(ध्रुव RTLIL::SigChunk &other) ध्रुव;
	bool operator ==(ध्रुव RTLIL::SigChunk &other) ध्रुव;
	bool operator !=(ध्रुव RTLIL::SigChunk &other) ध्रुव;
};

संरचना RTLIL::SigBit
{
	RTLIL::Wire *wire;
	सङ्घ {
		RTLIL::State data; // used if wire == NULL
		पूर्णाङ्क offset;        // used if wire != NULL
	};

	SigBit();
	SigBit(RTLIL::State bit);
	explicit SigBit(bool bit);
	SigBit(RTLIL::Wire *wire);
	SigBit(RTLIL::Wire *wire, पूर्णाङ्क offset);
	SigBit(ध्रुव RTLIL::SigChunk &chunk);
	SigBit(ध्रुव RTLIL::SigChunk &chunk, पूर्णाङ्क index);
	SigBit(ध्रुव RTLIL::SigSpec &sig);
	SigBit(ध्रुव RTLIL::SigBit &sigbit) = स्वभाव;
	RTLIL::SigBit &operator =(ध्रुव RTLIL::SigBit &other) = स्वभाव;

	अन्तःस्थ bool is_wire() ध्रुव { प्रत्यागम wire != रिक्त; }

	bool operator <(ध्रुव RTLIL::SigBit &other) ध्रुव;
	bool operator ==(ध्रुव RTLIL::SigBit &other) ध्रुव;
	bool operator !=(ध्रुव RTLIL::SigBit &other) ध्रुव;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव;
	[[nodiscard]] Hasher hash_शीर्ष() ध्रुव;
};

namespace hashlib {
	template <>
	संरचना hash_ops<RTLIL::SigBit> {
		स्थिर अन्तःस्थ bool cmp(ध्रुव RTLIL::SigBit &a, ध्रुव RTLIL::SigBit &b) {
			प्रत्यागम a == b;
		}
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash(ध्रुव RTLIL::SigBit sb) {
			प्रत्यागम sb.hash_शीर्ष();
		}
		[[nodiscard]] स्थिर अन्तःस्थ Hasher hash_into(ध्रुव RTLIL::SigBit sb, Hasher h) {
			प्रत्यागम sb.hash_into(h);
		}
	};
};

संरचना RTLIL::SigSpecIterator
{
	प्रकारनिर्देश std::आदान_iterator_tag iterator_category;
	प्रकारनिर्देश RTLIL::SigBit मूल्य_type;
	प्रकारनिर्देश ptrdiff_t difference_type;
	प्रकारनिर्देश RTLIL::SigBit* pointer;
	प्रकारनिर्देश RTLIL::SigBit& reference;

	RTLIL::SigSpec *sig_p;
	पूर्णाङ्क index;

	अन्तःस्थ RTLIL::SigBit &operator*() ध्रुव;
	अन्तःस्थ bool operator!=(ध्रुव RTLIL::SigSpecIterator &other) ध्रुव { प्रत्यागम index != other.index; }
	अन्तःस्थ bool operator==(ध्रुव RTLIL::SigSpecIterator &other) ध्रुव { प्रत्यागम index == other.index; }
	अन्तःस्थ शून्य operator++() { index++; }
};

संरचना RTLIL::SigSpecConstIterator
{
	प्रकारनिर्देश std::आदान_iterator_tag iterator_category;
	प्रकारनिर्देश RTLIL::SigBit मूल्य_type;
	प्रकारनिर्देश ptrdiff_t difference_type;
	प्रकारनिर्देश RTLIL::SigBit* pointer;
	प्रकारनिर्देश RTLIL::SigBit& reference;

	ध्रुव RTLIL::SigSpec *sig_p;
	RTLIL::SigBit bit;
	पूर्णाङ्क index;

	अन्तःस्थ ध्रुव RTLIL::SigBit &operator*();
	अन्तःस्थ bool operator!=(ध्रुव RTLIL::SigSpecConstIterator &other) ध्रुव { प्रत्यागम index != other.index; }
	अन्तःस्थ bool operator==(ध्रुव RTLIL::SigSpecIterator &other) ध्रुव { प्रत्यागम index == other.index; }
	अन्तःस्थ शून्य operator++() { index++; }
};

संरचना RTLIL::SigSpec
{
गुप्त:
	friend class SigSpecRepTest;
	FRIEND_TEST(SigSpecRepTest, Extract);
	गणना Representation : वर्ण {
		CHUNK,
		BITS,
	};
	// An AtomicHash is either clear or a nonzero integer.
	संरचना AtomicHash {
		// Create an initially clear value.
		AtomicHash() : atomic_(0) {}
		AtomicHash(ध्रुव AtomicHash &rhs) : atomic_(rhs.load()) {}
		AtomicHash &operator=(ध्रुव AtomicHash &rhs) { store(rhs.load()); प्रत्यागम *this; }
		// Read the hash. Returns nullopt if the hash is clear.
		std::optional<Hasher::hash_t> पठ() ध्रुव {
			Hasher::hash_t मूल्य = load();
			यदि (मूल्य == 0)
				प्रत्यागम std::nullopt;
			प्रत्यागम मूल्य;
		}
		// Set the hash. If the value is already set, then the new value must
		// equal the current value.
		शून्य set(Hasher::hash_t मूल्य) ध्रुव {
			log_assert(मूल्य != 0);
			Hasher::hash_t old = const_cast<std::atomic<Hasher::hash_t>&>(atomic_)
					.exchange(मूल्य, std::memory_order_relaxed);
			log_assert(old == 0 || old == मूल्य);
		}
		शून्य clear() { store(0); }
	गुप्त:
		पूर्णाङ्क load() ध्रुव { प्रत्यागम atomic_.load(std::memory_order_relaxed); }
		शून्य store(Hasher::hash_t मूल्य) ध्रुव {
			const_cast<std::atomic<Hasher::hash_t>&>(atomic_).store(मूल्य, std::memory_order_relaxed);
		}

		std::atomic<Hasher::hash_t> atomic_;
	};

	Representation rep_;
	AtomicHash hash_;
	सङ्घ {
		RTLIL::SigChunk chunk_;
		std::vector<RTLIL::SigBit> bits_; // LSB at index 0
	};

	शून्य init_empty_bits() {
		rep_ = BITS;
		new (&bits_) std::vector<RTLIL::SigBit>;
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
			bits_.~vector();
	}
	friend संरचना Chunks;

सार्वजनिक:
	SigSpec() { init_empty_bits(); }
	SigSpec(std::initializer_list<RTLIL::SigSpec> parts);
	SigSpec(ध्रुव SigSpec &मूल्य) : rep_(मूल्य.rep_), hash_(मूल्य.hash_) {
		यदि (मूल्य.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(मूल्य.chunk_);
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(मूल्य.bits_);
	}
	SigSpec(SigSpec &&मूल्य) : rep_(मूल्य.rep_), hash_(मूल्य.hash_) {
		यदि (मूल्य.rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(मूल्य.chunk_));
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(मूल्य.bits_));
	}
	SigSpec(ध्रुव RTLIL::Const &मूल्य);
	SigSpec(RTLIL::Const &&मूल्य);
	SigSpec(ध्रुव RTLIL::SigChunk &chunk);
	SigSpec(RTLIL::SigChunk &&chunk);
	SigSpec(RTLIL::Wire *wire);
	SigSpec(RTLIL::Wire *wire, पूर्णाङ्क offset, पूर्णाङ्क width = 1);
	SigSpec(ध्रुव std::string &str);
	SigSpec(पूर्णाङ्क val, पूर्णाङ्क width = 32);
	SigSpec(RTLIL::State bit, पूर्णाङ्क width = 1);
	SigSpec(ध्रुव RTLIL::SigBit &bit, पूर्णाङ्क width = 1);
	SigSpec(ध्रुव std::vector<RTLIL::SigChunk> &chunks);
	SigSpec(ध्रुव std::vector<RTLIL::SigBit> &bits);
	SigSpec(ध्रुव pool<RTLIL::SigBit> &bits);
	SigSpec(ध्रुव std::set<RTLIL::SigBit> &bits);
	explicit SigSpec(bool bit);
	~SigSpec() {
		destroy();
	}

	SigSpec &operator=(ध्रुव SigSpec &rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		यदि (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(rhs.chunk_);
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(rhs.bits_);
		प्रत्यागम *this;
	}
	SigSpec &operator=(SigSpec &&rhs) {
		destroy();
		rep_ = rhs.rep_;
		hash_ = rhs.hash_;
		यदि (rep_ == CHUNK)
			new (&chunk_) RTLIL::SigChunk(std::move(rhs.chunk_));
		अन्यथा
			new (&bits_) std::vector<RTLIL::SigBit>(std::move(rhs.bits_));
		प्रत्यागम *this;
	}

	// SigSpec::Chunks holds one reconstructed chunk at a time
	// to provide the SigSpec::chunks() read-only chunks view
	// since vector<SigChunk> SigSpec::chunks_ has been removed
	संरचना Chunks {
		Chunks(ध्रुव SigSpec &spec) : spec(spec) {}
		संरचना const_iterator {
			using iterator_category = std::forward_iterator_tag;
			using मूल्य_type = ध्रुव SigChunk &;
			using difference_type = std::ptrdiff_t;
			using pointer = ध्रुव SigChunk *;
			using reference = ध्रुव SigChunk &;

			ध्रुव SigSpec &spec;
			पूर्णाङ्क bit_index;
			SigChunk chunk;

			const_iterator(ध्रुव SigSpec &spec) : spec(spec) {
				bit_index = 0;
				यदि (spec.rep_ == BITS)
					next_chunk_bits();
			}
			गणना End { END };
			const_iterator(ध्रुव SigSpec &spec, End) : spec(spec) {
				bit_index = spec.size();
			}
			शून्य next_chunk_bits();

			ध्रुव SigChunk &operator*() {
				यदि (spec.rep_ == CHUNK)
					प्रत्यागम spec.chunk_;
				प्रत्यागम chunk;
			};
			ध्रुव SigChunk *operator->() { प्रत्यागम &**this; }
			const_iterator &operator++() {
				bit_index += (**this).width;
				यदि (spec.rep_ == BITS)
					next_chunk_bits();
				प्रत्यागम *this;
			}
			bool operator==(ध्रुव const_iterator &rhs) ध्रुव { प्रत्यागम bit_index == rhs.bit_index; }
			bool operator!=(ध्रुव const_iterator &rhs) ध्रुव { प्रत्यागम !(*this == rhs); }
		};
		const_iterator begin() ध्रुव { प्रत्यागम const_iterator(spec); }
		const_iterator end() ध्रुव {
			const_iterator it(spec, const_iterator::END);
			प्रत्यागम it;
		}
		// Later we should deprecate these and remove their in-tree calls,
		// so we can eventually remove chunk_vector.
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rbegin() {
			ensure_chunk_vector();
			प्रत्यागम chunk_vector.rbegin();
		}
		std::vector<RTLIL::SigChunk>::const_reverse_iterator rend() {
			ensure_chunk_vector();
			प्रत्यागम chunk_vector.rend();
		}
		पूर्णाङ्क size() {
			ensure_chunk_vector();
			प्रत्यागम chunk_vector.size();
		}
		पूर्णाङ्क size() ध्रुव {
			प्रत्यागम std::distance(begin(), end());
		}
		ध्रुव SigChunk &at(पूर्णाङ्क index) {
			ensure_chunk_vector();
			प्रत्यागम chunk_vector.at(index);
		}
		operator ध्रुव std::vector<RTLIL::SigChunk>&() {
			ensure_chunk_vector();
			प्रत्यागम chunk_vector;
		}
	गुप्त:
		शून्य ensure_chunk_vector() {
			यदि (spec.size() > 0 && chunk_vector.empty()) {
				प्रति (ध्रुव RTLIL::SigChunk &c : *this)
					chunk_vector.ढकला_back(c);
			}
		}
		ध्रुव SigSpec &spec;
		std::vector<RTLIL::SigChunk> chunk_vector;
	};
	friend संरचना Chunks::const_iterator;

	अन्तःस्थ Chunks chunks() ध्रुव { प्रत्यागम {*this}; }
	अन्तःस्थ ध्रुव SigSpec &bits() ध्रुव { प्रत्यागम *this; }

	अन्तःस्थ पूर्णाङ्क size() ध्रुव { प्रत्यागम rep_ == CHUNK ? chunk_.width : GetSize(bits_); }
	अन्तःस्थ bool empty() ध्रुव { प्रत्यागम size() == 0; };

	अन्तःस्थ RTLIL::SigBit &operator[](पूर्णाङ्क index) { inline_unpack(); hash_.clear(); प्रत्यागम bits_.at(index); }
	अन्तःस्थ RTLIL::SigBit operator[](पूर्णाङ्क index) ध्रुव {
		यदि (rep_ == CHUNK) {
			यदि (index < 0 || index >= chunk_.width)
				throw std::out_of_range("SigSpec::operator[]");
			यदि (chunk_.wire)
				प्रत्यागम RTLIL::SigBit(chunk_.wire, chunk_.offset + index);
			प्रत्यागम RTLIL::SigBit(chunk_.data[index]);
		}
		प्रत्यागम bits_.at(index);
	}

	अन्तःस्थ RTLIL::SigSpecIterator begin() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = 0; प्रत्यागम it; }
	अन्तःस्थ RTLIL::SigSpecIterator end() { RTLIL::SigSpecIterator it; it.sig_p = this; it.index = size(); प्रत्यागम it; }

	अन्तःस्थ RTLIL::SigSpecConstIterator begin() ध्रुव { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = 0; प्रत्यागम it; }
	अन्तःस्थ RTLIL::SigSpecConstIterator end() ध्रुव { RTLIL::SigSpecConstIterator it; it.sig_p = this; it.index = size(); प्रत्यागम it; }

	शून्य sort();
	शून्य sort_and_unify();

	शून्य replace(ध्रुव RTLIL::SigSpec &pattern, ध्रुव RTLIL::SigSpec &with);
	शून्य replace(ध्रुव RTLIL::SigSpec &pattern, ध्रुव RTLIL::SigSpec &with, RTLIL::SigSpec *other) ध्रुव;

	शून्य replace(ध्रुव dict<RTLIL::SigBit, RTLIL::SigBit> &rules);
	शून्य replace(ध्रुव dict<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ध्रुव;

	शून्य replace(ध्रुव std::map<RTLIL::SigBit, RTLIL::SigBit> &rules);
	शून्य replace(ध्रुव std::map<RTLIL::SigBit, RTLIL::SigBit> &rules, RTLIL::SigSpec *other) ध्रुव;

	शून्य replace(पूर्णाङ्क offset, ध्रुव RTLIL::SigSpec &with);

	शून्य remove(ध्रुव RTLIL::SigSpec &pattern);
	शून्य remove(ध्रुव RTLIL::SigSpec &pattern, RTLIL::SigSpec *other) ध्रुव;
	शून्य remove2(ध्रुव RTLIL::SigSpec &pattern, RTLIL::SigSpec *other);

	शून्य remove(ध्रुव pool<RTLIL::SigBit> &pattern);
	शून्य remove(ध्रुव pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other) ध्रुव;
	शून्य remove2(ध्रुव pool<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	शून्य remove2(ध्रुव std::set<RTLIL::SigBit> &pattern, RTLIL::SigSpec *other);
	शून्य remove2(ध्रुव pool<RTLIL::Wire*> &pattern, RTLIL::SigSpec *other);

	शून्य remove(पूर्णाङ्क offset, पूर्णाङ्क length = 1);
	शून्य remove_const();

	RTLIL::SigSpec extract(ध्रुव RTLIL::SigSpec &pattern, ध्रुव RTLIL::SigSpec *other = रिक्त) ध्रुव;
	RTLIL::SigSpec extract(ध्रुव pool<RTLIL::SigBit> &pattern, ध्रुव RTLIL::SigSpec *other = रिक्त) ध्रुव;
	RTLIL::SigSpec extract(पूर्णाङ्क offset, पूर्णाङ्क length = 1) ध्रुव;
	RTLIL::SigSpec extract_end(पूर्णाङ्क offset) ध्रुव { प्रत्यागम extract(offset, size() - offset); }

	शून्य rewrite_wires(std::function<शून्य(RTLIL::Wire*& wire)> rewrite);

	RTLIL::SigBit lsb() ध्रुव { log_assert(size()); प्रत्यागम (*this)[0]; };
	RTLIL::SigBit msb() ध्रुव { log_assert(size()); प्रत्यागम (*this)[size() - 1]; };
	RTLIL::SigBit front() ध्रुव { प्रत्यागम (*this)[0]; }
	RTLIL::SigBit back() ध्रुव { प्रत्यागम (*this)[size() - 1]; }

	शून्य append(ध्रुव RTLIL::SigSpec &सङ्केत);
	अन्तःस्थ शून्य append(Wire *wire) { append(RTLIL::SigSpec(wire)); }
	अन्तःस्थ शून्य append(ध्रुव RTLIL::SigChunk &chunk) { append(RTLIL::SigSpec(chunk)); }
	अन्तःस्थ शून्य append(ध्रुव RTLIL::Const &const_) { append(RTLIL::SigSpec(const_)); }

	शून्य append(ध्रुव RTLIL::SigBit &bit);
	अन्तःस्थ शून्य append(RTLIL::State state) { append(RTLIL::SigBit(state)); }
	अन्तःस्थ शून्य append(bool bool_) { append(RTLIL::SigBit(bool_)); }

	शून्य extend_u0(पूर्णाङ्क width, bool is_signed = false);

	RTLIL::SigSpec repeat(पूर्णाङ्क अंक) ध्रुव;

	शून्य reverse() { inline_unpack(); std::reverse(bits_.begin(), bits_.end()); }

	bool operator <(ध्रुव RTLIL::SigSpec &other) ध्रुव;
	bool operator ==(ध्रुव RTLIL::SigSpec &other) ध्रुव;
	अन्तःस्थ bool operator !=(ध्रुव RTLIL::SigSpec &other) ध्रुव { प्रत्यागम !(*this == other); }

	bool is_wire() ध्रुव;
	bool is_chunk() ध्रुव;
	अन्तःस्थ bool is_bit() ध्रुव { प्रत्यागम size() == 1; }

	bool known_driver() ध्रुव;

	bool is_fully_const() ध्रुव;
	bool is_fully_zero() ध्रुव;
	bool is_fully_ones() ध्रुव;
	bool is_fully_def() ध्रुव;
	bool is_fully_undef() ध्रुव;
	bool has_const() ध्रुव;
	bool has_const(State state) ध्रुव;
	bool has_marked_bits() ध्रुव;
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
	bool convertible_to_int(bool is_signed = false) ध्रुव;

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

	bool match(ध्रुव वर्ण* pattern) ध्रुव;

	std::set<RTLIL::SigBit> to_sigbit_set() ध्रुव;
	pool<RTLIL::SigBit> to_sigbit_pool() ध्रुव;
	std::vector<RTLIL::SigBit> to_sigbit_vector() ध्रुव;
	std::map<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_map(ध्रुव RTLIL::SigSpec &other) ध्रुव;
	dict<RTLIL::SigBit, RTLIL::SigBit> to_sigbit_dict(ध्रुव RTLIL::SigSpec &other) ध्रुव;

	स्थिर bool parse(RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);
	स्थिर bool parse_sel(RTLIL::SigSpec &sig, RTLIL::Design *design, RTLIL::Module *module, std::string str);
	स्थिर bool parse_rhs(ध्रुव RTLIL::SigSpec &lhs, RTLIL::SigSpec &sig, RTLIL::Module *module, std::string str);

	operator std::vector<RTLIL::SigChunk>() ध्रुव;
	operator std::vector<RTLIL::SigBit>() ध्रुव { प्रत्यागम to_sigbit_vector(); }
	ध्रुव RTLIL::SigBit &at(पूर्णाङ्क offset, ध्रुव RTLIL::SigBit &defval) { प्रत्यागम offset < size() ? (*this)[offset] : defval; }
	RTLIL::SigBit& at(पूर्णाङ्क offset) { प्रत्यागम (*this)[offset]; }
	RTLIL::SigBit at(पूर्णाङ्क offset) ध्रुव { प्रत्यागम (*this)[offset]; }

	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
		Hasher::hash_t val;
		यदि (std::optional<Hasher::hash_t> current = hash_.पठ())
			val = *current;
		अन्यथा
			val = updhash();
		h.eat(val);
		प्रत्यागम h;
	}

#ifndef NDEBUG
	शून्य check(ध्रुव Module *mod = nullptr) ध्रुव;
#अन्यथा
	शून्य check(ध्रुव Module *mod = nullptr) ध्रुव { (शून्य)mod; }
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
	bool boxed_module(RTLIL::IdString mod_name) ध्रुव;

	// checks if the given module is included in this selection
	bool selected_module(RTLIL::IdString mod_name) ध्रुव;

	// checks if the given module is wholly included in this selection,
	// i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) ध्रुव;

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
		प्रत्यागम full_selection || complete_selection;
	}

	// add whole module to this selection
	template<typename T1> शून्य select(T1 *module) {
		यदि (!selects_all() && selected_modules.मोजणी(module->name) == 0) {
			IdString name = module->name;
			selected_modules.घाला(name);
			selected_members.erase(name);
			यदि (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// add member of module to this selection
	template<typename T1, typename T2> शून्य select(T1 *module, T2 *member) {
		select(module, member->name.ref());
	}

	template<typename T1> शून्य select(T1 *module, RTLIL::IdString member) {
		यदि (!selects_all() && selected_modules.मोजणी(module->name.ref()) == 0) {
			selected_members[module->name.ref()].घाला(member);
			यदि (module->get_blackbox_attribute())
				selects_boxes = true;
		}
	}

	// checks if selection is empty
	bool empty() ध्रुव {
		प्रत्यागम !selects_all() && selected_modules.empty() && selected_members.empty();
	}

	// clear this selection, leaving it empty
	शून्य clear();

	// create a new selection which is empty
	स्थिर Selection EmptySelection(RTLIL::Design *design = nullptr) { प्रत्यागम Selection(false, false, design); };

	// create a new selection with all non-boxed modules
	स्थिर Selection FullSelection(RTLIL::Design *design = nullptr) { प्रत्यागम Selection(true, false, design); };

	// create a new selection with all modules, including boxes
	स्थिर Selection CompleteSelection(RTLIL::Design *design = nullptr) { प्रत्यागम Selection(true, true, design); };
};

संरचना RTLIL::Monitor
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(hashidx_); प्रत्यागम h; }

	Monitor() {
		स्थिर unsigned पूर्णाङ्क hashidx_मोजणी = 123456789;
		hashidx_मोजणी = mkhash_xorshift(hashidx_मोजणी);
		hashidx_ = hashidx_मोजणी;
	}

	आभास ~Monitor() { }
	आभास शून्य notify_module_add(RTLIL::Module*) { }
	आभास शून्य notify_module_del(RTLIL::Module*) { }
	आभास शून्य notify_connect(RTLIL::Cell*, RTLIL::IdString, ध्रुव RTLIL::SigSpec&, ध्रुव RTLIL::SigSpec&) { }
	आभास शून्य notify_connect(RTLIL::Module*, ध्रुव RTLIL::SigSig&) { }
	आभास शून्य notify_connect(RTLIL::Module*, ध्रुव std::vector<RTLIL::SigSig>&) { }
	आभास शून्य notify_blackout(RTLIL::Module*) { }
};

// Forward declaration; defined in preproc.h.
संरचना define_map_t;

template<typename N>
अन्तःस्थ constexpr bool is_unpooled_name_v =
	std::is_same_v<std::decay_t<N>, TwineSpec> || std::is_same_v<std::decay_t<N>, TwineSpec::Leaf> ||
	std::is_same_v<std::decay_t<N>, TwineSpec::Suffix> || std::is_same_v<std::decay_t<N>, TwineSpec::AutoSuffix> ||
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ध्रुव वर्ण*> || std::is_same_v<std::decay_t<N>, वर्ण*>;

#define YS_UNPOOLED_NAME(N) std::enable_if_t<is_unpooled_name_v<N>, पूर्णाङ्क> = 0

template<typename N>
अन्तःस्थ constexpr bool is_name_string_v =
	std::is_same_v<std::decay_t<N>, std::string> || std::is_same_v<std::decay_t<N>, std::string_view> ||
	std::is_same_v<std::decay_t<N>, ध्रुव वर्ण *> || std::is_same_v<std::decay_t<N>, वर्ण *>;

#define YS_NAME_STRING(N) std::enable_if_t<is_name_string_v<N>, पूर्णाङ्क> = 0

// Forwarders from various string-representing types into a canonical IdString method

// Forwards _func(name, arg1...)
#define YS_NAME_FWD_POOL(_func, _pool) \
	template<typename N, typename... Rest, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(N name, Rest&&... rest) \
		{ प्रत्यागम _func(_pool.add(std::move(name)), std::forward<Rest>(rest)...); }

// Forwards _func(arg0, name, arg2...)
#define YS_NAME_FWD_2ND_POOL(_func, _pool) \
	template<typename T, typename N, YS_UNPOOLED_NAME(N)> \
	decltype(auto) _func(T &&first, N name) \
		{ प्रत्यागम _func(std::forward<T>(first), _pool.add(std::move(name))); }

#define YS_NAME_FWD(_func) YS_NAME_FWD_POOL(_func, design->twines())
#define YS_NAME_FWD_2ND(_func) YS_NAME_FWD_2ND_POOL(_func, design->twines())

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

	size_t gc_twines();

	std::vector<std::unique_ptr<AST::Ast_गाठ>> verilog_packages, verilog_globals;
	std::unique_ptr<define_map_t> verilog_defines;

	std::vector<RTLIL::Selection> selection_चळत;
	dict<RTLIL::IdString, RTLIL::Selection> selection_vars;
	IdString selected_active_module;

	Design();
	~Design();

	RTLIL::ObjRange<RTLIL::Module*, IdString> modules();
	RTLIL::Module *module(RTLIL::IdString name);
	ध्रुव RTLIL::Module *module(RTLIL::IdString name) ध्रुव;
	template<typename N, YS_NAME_STRING(N)> RTLIL::Module *module(N name)
		{ प्रत्यागम module(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ध्रुव RTLIL::Module *module(N name) ध्रुव
		{ प्रत्यागम module(twines().find(std::string_view(name))); }
	RTLIL::Module *शीर्ष_module() ध्रुव;

	bool has(RTLIL::IdString id) ध्रुव {
		twines_.check_owned(id);
		प्रत्यागम modules_.मोजणी(id) != 0;
	}
	template<typename N, YS_NAME_STRING(N)> bool has(N name) ध्रुव
		{ प्रत्यागम has(twines().find(std::string_view(name))); }

	शून्य add(RTLIL::Module *module);

	RTLIL::Module *addModule(RTLIL::IdString name);
	YS_NAME_FWD_POOL(addModule, twines())
	शून्य remove(RTLIL::Module *module);
	शून्य rename(RTLIL::Module *module, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND_POOL(rename, twines())

	शून्य scratchpad_unset(ध्रुव std::string &varname);

	शून्य scratchpad_set_int(ध्रुव std::string &varname, पूर्णाङ्क मूल्य);
	शून्य scratchpad_set_bool(ध्रुव std::string &varname, bool मूल्य);
	शून्य scratchpad_set_string(ध्रुव std::string &varname, std::string मूल्य);

	पूर्णाङ्क scratchpad_get_int(ध्रुव std::string &varname, पूर्णाङ्क default_मूल्य_2 = 0) ध्रुव;
	bool scratchpad_get_bool(ध्रुव std::string &varname, bool default_मूल्य_2 = false) ध्रुव;
	std::string scratchpad_get_string(ध्रुव std::string &varname, ध्रुव std::string &default_मूल्य_2 = std::string()) ध्रुव;

	शून्य sort();
	शून्य sort_modules();
	शून्य check();
	शून्य optimize();

	शून्य clone_into(RTLIL::Design *dst) ध्रुव;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::IdString mod_name) ध्रुव;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::IdString mod_name) ध्रुव;

	// checks if the given member from the given module is included in the
	// current selection
	bool selected_member(RTLIL::IdString mod_name, RTLIL::IdString memb_name) ध्रुव;

	// checks if the given module is included in the current selection
	bool selected_module(RTLIL::Module *mod) ध्रुव;

	// checks if the given module is wholly included in the current
	// selection, i.e. not partially selected
	bool selected_whole_module(RTLIL::Module *mod) ध्रुव;

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
		प्रत्यागम selection_चळत.back();
	}

	// get the current selection
	ध्रुव RTLIL::Selection &selection() ध्रुव {
		प्रत्यागम selection_चळत.back();
	}

	// is the current selection a full selection (no boxes)
	bool full_selection() ध्रुव {
		प्रत्यागम selection().full_selection;
	}

	// is the given module in the current selection
	template<typename T1> bool selected(T1 *module) ध्रुव {
		प्रत्यागम selected_module(module->name);
	}

	// is the given member of the given module in the current selection
	template<typename T1, typename T2> bool selected(T1 *module, T2 *member) ध्रुव {
		प्रत्यागम selected_member(module->name, member->name);
	}

	// add whole module to the current selection
	template<typename T1> शून्य select(T1 *module) {
		RTLIL::Selection &sel = selection();
		sel.select(module);
	}

	// add member of module to the current selection
	template<typename T1, typename T2> शून्य select(T1 *module, T2 *member) {
		RTLIL::Selection &sel = selection();
		sel.select(module, member);
	}


	// returns all selected modules
	std::vector<RTLIL::Module*> selected_modules(
		// controls if partially selected modules are included
		RTLIL::SelectPartials partials = SELECT_ALL,
		// controls if boxed modules are included
		RTLIL::SelectBoxes boxes = SB_UNBOXED_WARN
	) ध्रुव;

	// returns all selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_modules() ध्रुव { प्रत्यागम selected_modules(SELECT_ALL, SB_ALL); }
	// returns all selected unboxed modules, silently ignoring any boxed
	// modules in the selection
	std::vector<RTLIL::Module*> selected_unboxed_modules() ध्रुव { प्रत्यागम selected_modules(SELECT_ALL, SB_UNBOXED_ONLY); }
	// returns all selected unboxed modules, warning the user if any boxed
	// modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_modules_warn() ध्रुव { प्रत्यागम selected_modules(SELECT_ALL, SB_UNBOXED_WARN); }

	[[deprecated("Use select_unboxed_whole_modules() to maintain prior behaviour, or consider one of the other selected whole module helpers.")]]
	std::vector<RTLIL::Module*> selected_whole_modules() ध्रुव { प्रत्यागम selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_WARN); }
	// returns all selected whole modules, silently ignoring partially
	// selected modules, and may include boxes
	std::vector<RTLIL::Module*> all_selected_whole_modules() ध्रुव { प्रत्यागम selected_modules(SELECT_WHOLE_ONLY, SB_ALL); }
	// returns all selected whole modules, warning the user if any partially
	// selected or boxed modules have been ignored; optionally includes
	// selected whole modules with the 'whitebox' attribute
	std::vector<RTLIL::Module*> selected_whole_modules_warn(
		// should whole modules with the 'whitebox' attribute be
		// included
		bool include_wb = false
	) ध्रुव { प्रत्यागम selected_modules(SELECT_WHOLE_WARN, include_wb ? SB_EXCL_BB_WARN : SB_UNBOXED_WARN); }
	// returns all selected unboxed whole modules, silently ignoring
	// partially selected or boxed modules
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules() ध्रुव { प्रत्यागम selected_modules(SELECT_WHOLE_ONLY, SB_UNBOXED_ONLY); }
	// returns all selected unboxed whole modules, warning the user if any
	// partially selected or boxed modules have been ignored
	std::vector<RTLIL::Module*> selected_unboxed_whole_modules_warn() ध्रुव { प्रत्यागम selected_modules(SELECT_WHOLE_WARN, SB_UNBOXED_WARN); }

	स्थिर std::map<unsigned पूर्णाङ्क, RTLIL::Design*> *get_all_designs(शून्य);

	std::string to_rtlil_str(bool only_selected = true) ध्रुव;
};

संरचना RTLIL::Module : सार्वजनिक RTLIL::AttrObject
{
	friend संरचना RTLIL::Cell;
	friend संरचना RTLIL::Design;

	RTLIL::ModuleNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(hashidx_); प्रत्यागम h; }

रक्षित:
	शून्य add(RTLIL::Wire *wire);
	शून्य add(RTLIL::Cell *cell);
	शून्य add(RTLIL::Process *process);

सार्वजनिक:
	RTLIL::Design *design;
	pool<RTLIL::Monitor*> monitors;

	TwinePool &twines() ध्रुव { प्रत्यागम design->twines(); }

	पूर्णाङ्क refcount_wires_;
	पूर्णाङ्क refcount_cells_;

	dict<RTLIL::IdString, RTLIL::Wire*> wires_;
	dict<RTLIL::IdString, RTLIL::Cell*> cells_;

	std::vector<RTLIL::SigSig>   connections_;

	idict<RTLIL::IdString> avail_parameters;
	dict<RTLIL::IdString, RTLIL::Const> parameter_default_मूल्ये;
	dict<RTLIL::IdString, RTLIL::Memory*> memories;
	dict<RTLIL::IdString, RTLIL::Process*> processes;

	Module();
	आभास ~Module();
	आभास RTLIL::IdString derive(RTLIL::Design *design, ध्रुव dict<RTLIL::IdString, RTLIL::Const> &parameters, bool mayfail = false);
	आभास RTLIL::IdString derive(RTLIL::Design *design, ध्रुव dict<RTLIL::IdString, RTLIL::Const> &parameters, ध्रुव dict<RTLIL::IdString, RTLIL::Module*> &interfaces, ध्रुव dict<RTLIL::IdString, RTLIL::IdString> &modports, bool mayfail = false);
	आभास size_t मोजणी_id(RTLIL::IdString id);
	आभास शून्य expand_interfaces(RTLIL::Design *design, ध्रुव dict<RTLIL::IdString, RTLIL::Module *> &local_interfaces);
	आभास bool reprocess_if_necessary(RTLIL::Design *design);

	आभास शून्य sort();
	आभास शून्य check();
	आभास शून्य optimize();
	आभास शून्य makeblackbox();

	bool get_blackbox_attribute(bool ignore_wb=false) ध्रुव {
		प्रत्यागम get_bool_attribute(ID::blackbox) || (!ignore_wb && get_bool_attribute(ID::whitebox));
	}

	शून्य connect(ध्रुव RTLIL::SigSig &conn);
	शून्य connect(ध्रुव RTLIL::SigSpec &lhs, ध्रुव RTLIL::SigSpec &rhs);
	शून्य new_connections(ध्रुव std::vector<RTLIL::SigSig> &new_conn);
	ध्रुव std::vector<RTLIL::SigSig> &connections() ध्रुव;

	std::vector<RTLIL::IdString> ports;
	शून्य fixup_ports();

	pool<RTLIL::Cell *> buf_norm_cell_queue;
	pool<pair<RTLIL::Cell *, RTLIL::IdString>> buf_norm_cell_port_queue;
	pool<RTLIL::Wire *> buf_norm_wire_queue;
	pool<RTLIL::Cell *> pending_deleted_cells;
	dict<RTLIL::Wire *, pool<RTLIL::Cell *>> buf_norm_connect_index;
	शून्य bufNormalize();

	template<typename T> शून्य rewrite_sigspecs(T &functor);
	template<typename T> शून्य rewrite_sigspecs2(T &functor);
	शून्य cloneInto(RTLIL::Module *new_mod) ध्रुव;
	आभास RTLIL::Module *clone() ध्रुव;
	आभास RTLIL::Module *clone(RTLIL::Design *dst) ध्रुव;
	आभास RTLIL::Module *clone(RTLIL::Design *dst, IdString target_name) ध्रुव;

	bool has_memories() ध्रुव;
	bool has_processes() ध्रुव;

	bool has_memories_warn() ध्रुव;
	bool has_processes_warn() ध्रुव;

	bool is_selected() ध्रुव;
	bool is_selected_whole() ध्रुव;

	std::vector<RTLIL::Wire*> selected_wires() ध्रुव;
	std::vector<RTLIL::Cell*> selected_cells() ध्रुव;
	std::vector<RTLIL::Memory*> selected_memories() ध्रुव;
	std::vector<RTLIL::Process*> selected_processes() ध्रुव;
	std::vector<RTLIL::SelectedMember> selected_members() ध्रुव;

	template<typename T> bool selected(T *member) ध्रुव {
		प्रत्यागम design->selected_member(name.ref(), member->name.ref());
	}

	RTLIL::Wire* wire(RTLIL::IdString id) {
		यदि (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		प्रत्यागम it == wires_.end() ? nullptr : it->second;
	}
	RTLIL::Cell* cell(RTLIL::IdString id) {
		यदि (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		प्रत्यागम it == cells_.end() ? nullptr : it->second;
	}

	ध्रुव RTLIL::Wire* wire(RTLIL::IdString id) ध्रुव{
		यदि (design)
			design->twines().check_owned(id);
		auto it = wires_.find(id);
		प्रत्यागम it == wires_.end() ? nullptr : it->second;
	}
	ध्रुव RTLIL::Cell* cell(RTLIL::IdString id) ध्रुव {
		यदि (design)
			design->twines().check_owned(id);
		auto it = cells_.find(id);
		प्रत्यागम it == cells_.end() ? nullptr : it->second;
	}

	template<typename N, YS_NAME_STRING(N)> RTLIL::Wire *wire(N name)
		{ प्रत्यागम wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> RTLIL::Cell *cell(N name)
		{ प्रत्यागम cell(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ध्रुव RTLIL::Wire *wire(N name) ध्रुव
		{ प्रत्यागम wire(twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> ध्रुव RTLIL::Cell *cell(N name) ध्रुव
		{ प्रत्यागम cell(twines().find(std::string_view(name))); }

	RTLIL::ObjRange<RTLIL::Wire*> wires() { प्रत्यागम RTLIL::ObjRange<RTLIL::Wire*>(&wires_, &refcount_wires_); }
	पूर्णाङ्क wires_size() ध्रुव { प्रत्यागम wires_.size(); }
	RTLIL::Wire* wire_at(पूर्णाङ्क index) ध्रुव { प्रत्यागम wires_.element(index)->second; }
	RTLIL::ObjRange<RTLIL::Cell*> cells() { प्रत्यागम RTLIL::ObjRange<RTLIL::Cell*>(&cells_, &refcount_cells_); }
	पूर्णाङ्क cells_size() ध्रुव { प्रत्यागम cells_.size(); }
	RTLIL::Cell* cell_at(पूर्णाङ्क index) ध्रुव { प्रत्यागम cells_.element(index)->second; }

	// Removing wires is expensive. If you have to remove wires, remove them all at once.
	शून्य remove(ध्रुव pool<RTLIL::Wire*> &wires);
	शून्य remove(RTLIL::Cell *cell);
	शून्य remove(RTLIL::Memory *memory);
	शून्य remove(RTLIL::Process *process);

	शून्य rename(RTLIL::Wire *wire, RTLIL::IdString new_name);
	शून्य rename(RTLIL::Cell *cell, RTLIL::IdString new_name);
	शून्य rename(RTLIL::IdString old_name, RTLIL::IdString new_name);
	YS_NAME_FWD_2ND(rename)

	शून्य swap_names(RTLIL::Wire *w1, RTLIL::Wire *w2);
	शून्य swap_names(RTLIL::Cell *c1, RTLIL::Cell *c2);

	RTLIL::IdString uniquify(RTLIL::IdString name);
	RTLIL::IdString uniquify(RTLIL::IdString name, पूर्णाङ्क &index);
	YS_NAME_FWD(uniquify)

	RTLIL::Wire *addWire(RTLIL::IdString name, पूर्णाङ्क width = 1);
	RTLIL::Wire *addWire(RTLIL::IdString name, ध्रुव RTLIL::Wire *other);
	YS_NAME_FWD(addWire)

	RTLIL::Cell *addCell(RTLIL::IdString name, RTLIL::IdString type);
	RTLIL::Cell *addCell(RTLIL::IdString name, ध्रुव RTLIL::Cell *other);
	YS_NAME_FWD(addCell)
	template<typename T, YS_UNPOOLED_NAME(T)>
	RTLIL::Cell *addCell(RTLIL::IdString name, T type) { प्रत्यागम addCell(name, design->twines().add(std::move(type))); }

	RTLIL::Memory *addMemory(RTLIL::IdString name);
	YS_NAME_FWD(addMemory)
	RTLIL::Memory *addMemory(RTLIL::IdString name, ध्रुव RTLIL::Memory *other);

	RTLIL::Process *addProcess(RTLIL::IdString name);
	YS_NAME_FWD(addProcess)
	RTLIL::Process *addProcess(RTLIL::IdString name, ध्रुव RTLIL::Process *other);

	// The add* methods create a cell and return the created cell. All signals must exist in advance.

	RTLIL::Cell* addNot     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addPos     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addBuf     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addBarrier (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addNeg     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* addAnd  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addOr   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addXor  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addXnor (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* addReduceAnd  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addReduceOr   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addReduceXor  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addReduceXnor (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addReduceBool (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* addShl    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addShr    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addSshl   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addSshr   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addShift  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addShiftx (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* addLt  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addLe  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addEq  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addNe  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addEqx (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addNex (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addGe  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addGt  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* addAdd (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addSub (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addMul (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	// truncating division
	RTLIL::Cell* addDiv (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	// truncating modulo
	RTLIL::Cell* addMod (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addDivFloor (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addModFloor (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addPow (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool a_signed = false, bool b_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* addFa (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव RTLIL::SigSpec &sig_x, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");

	RTLIL::Cell* addLogicNot (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addLogicAnd (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::Cell* addLogicOr  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::Cell* addMux  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addPmux (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addBmux (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addDemux (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");

	RTLIL::Cell* addBweqx  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addBwmux  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");

	RTLIL::Cell* addSlice  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, RTLIL::Const offset, ध्रुव std::string &src = "");
	RTLIL::Cell* addConcat (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addLut    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_y, RTLIL::Const lut, ध्रुव std::string &src = "");
	RTLIL::Cell* addTribuf (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addAssert (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src = "");
	RTLIL::Cell* addAssume (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src = "");
	RTLIL::Cell* addLive   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src = "");
	RTLIL::Cell* addFair   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src = "");
	RTLIL::Cell* addCover  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव std::string &src = "");
	RTLIL::Cell* addEquiv  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");

	RTLIL::Cell* addSr    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr, ध्रुव RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addFf    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव std::string &src = "");
	RTLIL::Cell* addDff   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_d,   ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addDffe  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en,  ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addDffsr (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addDffsre (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addAdff (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, RTLIL::Const arst_मूल्य, bool clk_polarity = true, bool arst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addAdffe (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst,  ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, RTLIL::Const arst_मूल्य, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addAldff (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addAldffe (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_aload,  ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addSdff (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, RTLIL::Const srst_मूल्य, bool clk_polarity = true, bool srst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addSdffe (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst,  ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, RTLIL::Const srst_मूल्य, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addSdffce (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, RTLIL::Const srst_मूल्य, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addDlatch (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool en_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addAdlatch (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, RTLIL::Const arst_मूल्य, bool en_polarity = true, bool arst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addDlatchsr (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr, RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");

	RTLIL::Cell* addBufGate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addNotGate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addAndGate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addNandGate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addOrGate     (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addNorGate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addXorGate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addXnorGate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addAndnotGate (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addOrnotGate  (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addMuxGate    (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_s, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addNmuxGate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_s, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addAoi3Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_c, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addOai3Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_c, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addAoi4Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_c, ध्रुव RTLIL::SigBit &sig_d, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");
	RTLIL::Cell* addOai4Gate   (RTLIL::IdString name, ध्रुव RTLIL::SigBit &sig_a, ध्रुव RTLIL::SigBit &sig_b, ध्रुव RTLIL::SigBit &sig_c, ध्रुव RTLIL::SigBit &sig_d, ध्रुव RTLIL::SigBit &sig_y, ध्रुव std::string &src = "");

	RTLIL::Cell* addSrGate     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr,
			ध्रुव RTLIL::SigSpec &sig_q, bool set_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addFfGate     (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव std::string &src = "");
	RTLIL::Cell* addDffGate    (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addDffeGate   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addDffsrGate  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool set_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addDffsreGate (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool clk_polarity = true, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addAdffGate   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			bool arst_मूल्य = false, bool clk_polarity = true, bool arst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addAdffeGate  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			bool arst_मूल्य = false, bool clk_polarity = true, bool en_polarity = true, bool arst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addAldffGate   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			ध्रुव RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool aload_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addAldffeGate  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_aload, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			ध्रुव RTLIL::SigSpec &sig_ad, bool clk_polarity = true, bool en_polarity = true, bool aload_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addSdffGate   (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			bool srst_मूल्य = false, bool clk_polarity = true, bool srst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addSdffeGate  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			bool srst_मूल्य = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addSdffceGate (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_clk, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_srst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			bool srst_मूल्य = false, bool clk_polarity = true, bool en_polarity = true, bool srst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addDlatchGate (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool en_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addAdlatchGate(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_arst, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q,
			bool arst_मूल्य = false, bool en_polarity = true, bool arst_polarity = true, ध्रुव std::string &src = "");
	RTLIL::Cell* addDlatchsrGate  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_en, ध्रुव RTLIL::SigSpec &sig_set, ध्रुव RTLIL::SigSpec &sig_clr,
			RTLIL::SigSpec sig_d, ध्रुव RTLIL::SigSpec &sig_q, bool en_polarity = true, bool set_polarity = true, bool clr_polarity = true, ध्रुव std::string &src = "");

	RTLIL::Cell* addAnyinit(RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_d, ध्रुव RTLIL::SigSpec &sig_q, ध्रुव std::string &src = "");

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
	RTLIL::SigSpec Shift  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Shiftx (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::SigSpec Lt  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Le  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Eq  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Ne  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Eqx (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Nex (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Ge  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
	RTLIL::SigSpec Gt  (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");

	RTLIL::SigSpec Add (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_b, bool is_signed = false, ध्रुव std::string &src = "");
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

	RTLIL::SigSpec Anyconst  (RTLIL::IdString name, पूर्णाङ्क width = 1, ध्रुव std::string &src = "");
	RTLIL::SigSpec Anyseq    (RTLIL::IdString name, पूर्णाङ्क width = 1, ध्रुव std::string &src = "");
	RTLIL::SigSpec Allconst  (RTLIL::IdString name, पूर्णाङ्क width = 1, ध्रुव std::string &src = "");
	RTLIL::SigSpec Allseq    (RTLIL::IdString name, पूर्णाङ्क width = 1, ध्रुव std::string &src = "");
	RTLIL::SigSpec Initstate (RTLIL::IdString name, ध्रुव std::string &src = "");

	RTLIL::SigSpec SetTag          (RTLIL::IdString name, ध्रुव std::string &tag, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव std::string &src = "");
	RTLIL::Cell*   addSetTag       (RTLIL::IdString name, ध्रुव std::string &tag, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव RTLIL::SigSpec &sig_y, ध्रुव std::string &src = "");
	RTLIL::SigSpec GetTag          (RTLIL::IdString name, ध्रुव std::string &tag, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव std::string &src = "");
	RTLIL::Cell*   addOverwriteTag (RTLIL::IdString name, ध्रुव std::string &tag, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव RTLIL::SigSpec &sig_s, ध्रुव RTLIL::SigSpec &sig_c, ध्रुव std::string &src = "");
	RTLIL::SigSpec OriginalTag     (RTLIL::IdString name, ध्रुव std::string &tag, ध्रुव RTLIL::SigSpec &sig_a, ध्रुव std::string &src = "");
	RTLIL::SigSpec FutureFF        (RTLIL::IdString name, ध्रुव RTLIL::SigSpec &sig_e, ध्रुव std::string &src = "");
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

	std::string to_rtlil_str() ध्रुव;
#ifdef YOSYS_ENABLE_PYTHON
	स्थिर std::map<unsigned पूर्णाङ्क, RTLIL::Module*> *get_all_modules(शून्य);
#endif
};

संरचना RTLIL::Wire : सार्वजनिक RTLIL::AttrObject
{
	RTLIL::WireNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(hashidx_); प्रत्यागम h; }

रक्षित:
	// use module->addWire() and module->remove() to create or destroy wires
	friend संरचना RTLIL::Module;
	Wire();
	~Wire();

	friend संरचना RTLIL::Design;
	friend संरचना RTLIL::Cell;
	RTLIL::Cell *driverCell_ = nullptr;
	RTLIL::IdString driverPort_;

सार्वजनिक:
	// do not simply copy wires
	Wire(RTLIL::Wire &other) = delete;
	शून्य operator=(RTLIL::Wire &other) = delete;

	RTLIL::Module *module;
	पूर्णाङ्क width, start_offset, port_id;
	bool port_आदान, port_output, upto, is_signed;

	RTLIL::Design *design() ध्रुव { प्रत्यागम module ? module->design : nullptr; }
	TwinePool &twines() ध्रुव { प्रत्यागम module->twines(); }

	bool known_driver() ध्रुव { प्रत्यागम driverCell_ != nullptr; }

	RTLIL::Cell *driverCell() ध्रुव    { log_assert(driverCell_); प्रत्यागम driverCell_; };
	RTLIL::IdString driverPort() ध्रुव { log_assert(driverCell_); प्रत्यागम driverPort_; };

	पूर्णाङ्क from_hdl_index(पूर्णाङ्क hdl_index) {
		पूर्णाङ्क zero_index = hdl_index - start_offset;
		पूर्णाङ्क rtlil_index = upto ? width - 1 - zero_index : zero_index;
		प्रत्यागम rtlil_index >= 0 && rtlil_index < width ? rtlil_index : INT_MIN;
	}

	पूर्णाङ्क to_hdl_index(पूर्णाङ्क rtlil_index) {
		यदि (rtlil_index < 0 || rtlil_index >= width)
			प्रत्यागम INT_MIN;
		पूर्णाङ्क zero_index = upto ? width - 1 - rtlil_index : rtlil_index;
		प्रत्यागम zero_index + start_offset;
	}

	std::string to_rtlil_str() ध्रुव;

#ifdef YOSYS_ENABLE_PYTHON
	स्थिर std::map<unsigned पूर्णाङ्क, RTLIL::Wire*> *get_all_wires(शून्य);
#endif
};

अन्तःस्थ पूर्णाङ्क GetSize(RTLIL::Wire *wire) {
	प्रत्यागम wire->width;
}

संरचना RTLIL::Memory : सार्वजनिक RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(hashidx_); प्रत्यागम h; }

	Memory();
	~Memory();

	RTLIL::Module *module = nullptr;

	RTLIL::Design *design() ध्रुव { प्रत्यागम module ? module->design : nullptr; }
	TwinePool &twines() ध्रुव { प्रत्यागम module->twines(); }

	RTLIL::MemoryNameId name;

	पूर्णाङ्क width, start_offset, size;
#ifdef YOSYS_ENABLE_PYTHON
	स्थिर std::map<unsigned पूर्णाङ्क, RTLIL::Memory*> *get_all_memorys(शून्य);
#endif

	std::string to_rtlil_str() ध्रुव;
};

संरचना RTLIL::Cell : सार्वजनिक RTLIL::AttrObject
{
गुप्त:
	शून्य initIndex();

	bool bufnorm_handle_setPort(IdString portname, RTLIL::SigSpec &सङ्केत, dict<IdString, RTLIL::SigSpec>::iterator conn_it);
सार्वजनिक:
	RTLIL::CellNameId name;

	Hasher::hash_t hashidx_;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(hashidx_); प्रत्यागम h; }

रक्षित:
	// use module->addCell() and module->remove() to create or destroy cells
	friend संरचना RTLIL::Module;
	Cell();
	~Cell();

सार्वजनिक:
	// do not simply copy cells
	Cell(RTLIL::Cell &other) = delete;
	शून्य operator=(RTLIL::Cell &other) = delete;

	RTLIL::Module *module;

	RTLIL::Design *design() ध्रुव { प्रत्यागम module ? module->design : nullptr; }
	TwinePool &twines() ध्रुव { प्रत्यागम module->twines(); }

	RTLIL::CellTypeId type;
	dict<RTLIL::IdString, RTLIL::SigSpec> connections_;
	dict<RTLIL::IdString, RTLIL::Const> parameters;

	// access cell ports
	bool hasPort(RTLIL::IdString portname) ध्रुव;
	शून्य unsetPort(RTLIL::IdString portname);
	शून्य setPort(RTLIL::IdString portname, RTLIL::SigSpec सङ्केत);
	template<typename N, YS_NAME_STRING(N)> शून्य setPort(N portname, RTLIL::SigSpec सङ्केत)
		{ setPort(module->twines().add(std::move(portname)), std::move(सङ्केत)); }
	ध्रुव RTLIL::SigSpec &getPort(RTLIL::IdString portname) ध्रुव;
	ध्रुव dict<RTLIL::IdString, RTLIL::SigSpec> &connections() ध्रुव;

	bool known() ध्रुव;
	bool आदान(RTLIL::IdString portname) ध्रुव;
	bool output(RTLIL::IdString portname) ध्रुव;
	PortDir port_dir(RTLIL::IdString portname) ध्रुव;

	bool hasParam(RTLIL::IdString paramname) ध्रुव;
	शून्य unsetParam(RTLIL::IdString paramname);
	शून्य setParam(RTLIL::IdString paramname, RTLIL::Const मूल्य);
	ध्रुव RTLIL::Const &getParam(RTLIL::IdString paramname) ध्रुव;

	template<typename N, YS_NAME_STRING(N)> bool hasParam(N name) ध्रुव
		{ प्रत्यागम hasParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> शून्य unsetParam(N name)
		{ unsetParam(module->twines().find(std::string_view(name))); }
	template<typename N, YS_NAME_STRING(N)> शून्य setParam(N name, RTLIL::Const मूल्य)
		{ setParam(module->twines().add(std::move(name)), std::move(मूल्य)); }
	template<typename N, YS_NAME_STRING(N)> ध्रुव RTLIL::Const &getParam(N name) ध्रुव
		{ प्रत्यागम getParam(module->twines().find(std::string_view(name))); }

	शून्य sort();
	शून्य check();
	शून्य fixup_parameters(bool set_a_signed = false, bool set_b_signed = false);

	bool has_keep_attr() ध्रुव;

	template<typename T> शून्य rewrite_sigspecs(T &functor);
	template<typename T> शून्य rewrite_sigspecs2(T &functor);

#ifdef YOSYS_ENABLE_PYTHON
	स्थिर std::map<unsigned पूर्णाङ्क, RTLIL::Cell*> *get_all_cells(शून्य);
#endif

	bool has_memid() ध्रुव;
	bool is_mem_cell() ध्रुव;
	bool is_builtin_ff() ध्रुव;

	std::string to_rtlil_str() ध्रुव;
};

संरचना RTLIL::CaseRule : सार्वजनिक RTLIL::AttrObject
{
	std::vector<RTLIL::SigSpec> compare;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::SwitchRule*> switches;

	~CaseRule();

	bool empty() ध्रुव;

	template<typename T> शून्य rewrite_sigspecs(T &functor);
	template<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::CaseRule *clone() ध्रुव;
};

संरचना RTLIL::SwitchRule : सार्वजनिक RTLIL::AttrObject
{
	RTLIL::SigSpec सङ्केत;
	std::vector<RTLIL::CaseRule*> cases;

	~SwitchRule();

	bool empty() ध्रुव;

	template<typename T> शून्य rewrite_sigspecs(T &functor);
	template<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::SwitchRule *clone() ध्रुव;
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
	RTLIL::SigSpec सङ्केत;
	std::vector<RTLIL::SigSig> actions;
	std::vector<RTLIL::MemWriteAction> mem_write_actions;

	template<typename T> शून्य rewrite_sigspecs(T &functor);
	template<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::SyncRule *clone() ध्रुव;
};

संरचना RTLIL::Process : सार्वजनिक RTLIL::AttrObject
{
	Hasher::hash_t hashidx_;
	RTLIL::ProcessNameId name;
	[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(hashidx_); प्रत्यागम h; }

रक्षित:
	// use module->addProcess() and module->remove() to create or destroy processes
	friend संरचना RTLIL::Module;
	Process();
	~Process();

सार्वजनिक:
	RTLIL::Module *module;
	RTLIL::CaseRule मूळ_case;
	std::vector<RTLIL::SyncRule*> syncs;

	RTLIL::Design *design() ध्रुव { प्रत्यागम module ? module->design : nullptr; }
	TwinePool &twines() ध्रुव { प्रत्यागम module->twines(); }

	template<typename T> शून्य rewrite_sigspecs(T &functor);
	template<typename T> शून्य rewrite_sigspecs2(T &functor);
	RTLIL::Process *clone() ध्रुव;

	std::string to_rtlil_str() ध्रुव;
};

अन्तःस्थ RTLIL::SigBit::SigBit() : wire(रिक्त), data(RTLIL::State::S0) { }
अन्तःस्थ RTLIL::SigBit::SigBit(RTLIL::State bit) : wire(रिक्त), data(bit) { }
अन्तःस्थ RTLIL::SigBit::SigBit(bool bit) : wire(रिक्त), data(bit ? State::S1 : State::S0) { }
अन्तःस्थ RTLIL::SigBit::SigBit(RTLIL::Wire *wire) : wire(wire), offset(0) { log_assert(wire && wire->width == 1); }
अन्तःस्थ RTLIL::SigBit::SigBit(RTLIL::Wire *wire, पूर्णाङ्क offset) : wire(wire), offset(offset) { log_assert(wire != nullptr); }
अन्तःस्थ RTLIL::SigBit::SigBit(ध्रुव RTLIL::SigChunk &chunk) : wire(chunk.wire) { log_assert(chunk.width == 1); यदि (wire) offset = chunk.offset; अन्यथा data = chunk.data[0]; }
अन्तःस्थ RTLIL::SigBit::SigBit(ध्रुव RTLIL::SigChunk &chunk, पूर्णाङ्क index) : wire(chunk.wire) { यदि (wire) offset = chunk.offset + index; अन्यथा data = chunk.data[index]; }

अन्तःस्थ bool RTLIL::SigBit::operator<(ध्रुव RTLIL::SigBit &other) ध्रुव {
	यदि (wire == other.wire)
		प्रत्यागम wire ? (offset < other.offset) : (data < other.data);
	यदि (wire != nullptr && other.wire != nullptr)
		प्रत्यागम wire->name < other.wire->name;
	प्रत्यागम (wire != nullptr) < (other.wire != nullptr);
}

अन्तःस्थ bool RTLIL::SigBit::operator==(ध्रुव RTLIL::SigBit &other) ध्रुव {
	प्रत्यागम (wire == other.wire) && (wire ? (offset == other.offset) : (data == other.data));
}

अन्तःस्थ bool RTLIL::SigBit::operator!=(ध्रुव RTLIL::SigBit &other) ध्रुव {
	प्रत्यागम (wire != other.wire) || (wire ? (offset != other.offset) : (data != other.data));
}

अन्तःस्थ Hasher RTLIL::SigBit::hash_into(Hasher h) ध्रुव {
	यदि (wire) {
		h.eat(offset);
		h.eat(wire->name);
		प्रत्यागम h;
	}
	h.eat(data);
	प्रत्यागम h;
}


अन्तःस्थ Hasher RTLIL::SigBit::hash_शीर्ष() ध्रुव {
	Hasher h;
	यदि (wire) {
		IdString name = wire->name.ref();
		uint32_t n = (uint32_t)name.bits();
		h.force(hashlib::legacy::djb2_add(n, offset));
		प्रत्यागम h;
	}
	h.force(data);
	प्रत्यागम h;
}

अन्तःस्थ RTLIL::SigBit &RTLIL::SigSpecIterator::operator*() ध्रुव {
	प्रत्यागम (*sig_p)[index];
}

अन्तःस्थ ध्रुव RTLIL::SigBit &RTLIL::SigSpecConstIterator::operator*() {
	bit = (*sig_p)[index];
	प्रत्यागम bit;
}

अन्तःस्थ RTLIL::SigBit::SigBit(ध्रुव RTLIL::SigSpec &sig) {
	log_assert(sig.size() == 1);
	auto it = sig.chunks().begin();
	*this = SigBit(*it);
}

template<typename T>
शून्य RTLIL::Module::rewrite_sigspecs(T &functor)
{
	प्रति (auto &it : cells_)
		it.second->rewrite_sigspecs(functor);
	प्रति (auto &it : processes)
		it.second->rewrite_sigspecs(functor);
	प्रति (auto &it : connections_) {
		functor(it.first);
		functor(it.second);
	}
}

template<typename T>
शून्य RTLIL::Module::rewrite_sigspecs2(T &functor)
{
	प्रति (auto &it : cells_)
		it.second->rewrite_sigspecs2(functor);
	प्रति (auto &it : processes)
		it.second->rewrite_sigspecs2(functor);
	प्रति (auto &it : connections_) {
		functor(it.first, it.second);
	}
}

template<typename T>
शून्य RTLIL::Cell::rewrite_sigspecs(T &functor) {
	प्रति (auto &it : connections_)
		functor(it.second);
}

template<typename T>
शून्य RTLIL::Cell::rewrite_sigspecs2(T &functor) {
	प्रति (auto &it : connections_)
		functor(it.second);
}

template<typename T>
शून्य RTLIL::CaseRule::rewrite_sigspecs(T &functor) {
	प्रति (auto &it : compare)
		functor(it);
	प्रति (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	प्रति (auto it : switches)
		it->rewrite_sigspecs(functor);
}

template<typename T>
शून्य RTLIL::CaseRule::rewrite_sigspecs2(T &functor) {
	प्रति (auto &it : compare)
		functor(it);
	प्रति (auto &it : actions) {
		functor(it.first, it.second);
	}
	प्रति (auto it : switches)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
शून्य RTLIL::SwitchRule::rewrite_sigspecs(T &functor)
{
	functor(सङ्केत);
	प्रति (auto it : cases)
		it->rewrite_sigspecs(functor);
}

template<typename T>
शून्य RTLIL::SwitchRule::rewrite_sigspecs2(T &functor)
{
	functor(सङ्केत);
	प्रति (auto it : cases)
		it->rewrite_sigspecs2(functor);
}

template<typename T>
शून्य RTLIL::SyncRule::rewrite_sigspecs(T &functor)
{
	functor(सङ्केत);
	प्रति (auto &it : actions) {
		functor(it.first);
		functor(it.second);
	}
	प्रति (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
शून्य RTLIL::SyncRule::rewrite_sigspecs2(T &functor)
{
	functor(सङ्केत);
	प्रति (auto &it : actions) {
		functor(it.first, it.second);
	}
	प्रति (auto &it : mem_write_actions) {
		functor(it.address);
		functor(it.data);
		functor(it.enable);
	}
}

template<typename T>
शून्य RTLIL::Process::rewrite_sigspecs(T &functor)
{
	मूळ_case.rewrite_sigspecs(functor);
	प्रति (auto it : syncs)
		it->rewrite_sigspecs(functor);
}

template<typename T>
शून्य RTLIL::Process::rewrite_sigspecs2(T &functor)
{
	मूळ_case.rewrite_sigspecs2(functor);
	प्रति (auto it : syncs)
		it->rewrite_sigspecs2(functor);
}

#अन्तर्भाव "kernel/rtlil_twine_compat_impl.h"

YOSYS_NAMESPACE_END

#endif
